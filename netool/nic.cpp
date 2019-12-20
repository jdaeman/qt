#include <net/if.h> //if_nameindex()
#include <sys/ioctl.h> //ioctl()
#include <string.h> //strcpy()
#include <arpa/inet.h> //inet_ntoa()
#include <unistd.h> //close()
#include <netinet/ether.h> //ether_ntoa()

#include <nic.h> //struct nic
#include <string.h> //strcpy()
#include <stdlib.h> //malloc()

#include <linux/netlink.h> //struct nlmsghdr
#include <linux/rtnetlink.h> //struct rtmsg, struct rtattr

#include <QDebug>

#define BUF_SIZE 8192

struct route_info
{
    unsigned int dest_addr;
    unsigned int src_addr;
    unsigned int gateway;

    char if_name[IF_NAMESIZE];
};

int get_gateway(unsigned int * gateway);
int read_nl_sock(int sock, char * buf, unsigned int seq, unsigned int pid);
void parse_routes(struct nlmsghdr * nlhdr, struct route_info * rt_info);

int nic_init(struct nic * list, unsigned int len)
{
    struct if_nameindex * if_arr, * itf;
    struct ifreq ifr;
    struct sockaddr_in * sin;
    struct sockaddr * sa;
    struct nic * cur;
    int sock, cnt = 0;
    unsigned int gateway;

    if_arr = if_nameindex();

    if(!if_arr)
        return -1;

    sock = socket(PF_INET, SOCK_DGRAM, 0); //create temp socket
    if(sock < 0)
        return -1;

    if(get_gateway(&gateway) < 0)
        return -1;

    for(itf = if_arr; itf->if_index != 0 || itf->if_name != NULL; itf++)
    {
        if(itf->if_index >= len) //out of array(struct nic * list)
            break;

        cnt++;
        cur = &list[itf->if_index];

        strcpy(ifr.ifr_ifrn.ifrn_name, itf->if_name); //copy interface name for ioctl

        ioctl(sock, SIOCGIFADDR, &ifr); //get interface logcial address
        sin = (struct sockaddr_in *)(&(ifr.ifr_addr));

        strcpy(cur->if_name, itf->if_name); //copy interface name
        cur->if_idx = itf->if_index; //copy interface index
        cur->if_laddr = sin->sin_addr.s_addr; //copy interface ip address

        ioctl(sock, SIOCGIFNETMASK, &ifr); //get interface net mask
        cur->if_netmask = sin->sin_addr.s_addr; //copy interface net mask

        ioctl(sock, SIOCGIFHWADDR, &ifr); //get interface physical address
        sa = (struct sockaddr *)sin;
        memcpy(cur->if_paddr, (u_int8_t *)(sa->sa_data), 6); //copy interface mac address

        cur->gateway = gateway;
    }

    close(sock);
    return cnt;
}

int get_gateway(unsigned int * gateway)
{
    int ret = -1;

    struct nlmsghdr * nlmsg; //net link message
    //struct rtmsg * rtmsg; //route message
    struct route_info * rt_info;

    char buf[BUF_SIZE];
    int sock, len, msg_seq = 0;

    //create net link socket
    sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_ROUTE);
    if(sock < 0)
        return -1;

    //buffer init
    memset(buf, 0, BUF_SIZE);

    //point the header and the msg structure pointers into the buffer
    nlmsg = (struct nlmsghdr *)buf;
    //rtmsg = (struct rtmsg *)NLMSG_DATA(nlmsg);

    //fill in the nlmsg header
    nlmsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); //length of message.
    nlmsg->nlmsg_type = RTM_GETROUTE; //get the routes from kernel routing table .
    nlmsg->nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP; //the message is a request for dump.
    nlmsg->nlmsg_seq = msg_seq++; //sequence of the message packet.
    nlmsg->nlmsg_pid = getpid(); //pid

    //send the request
    if(send(sock, nlmsg, nlmsg->nlmsg_len, 0) < 0)
        return -1;

    //read the response
    if((len = read_nl_sock(sock, buf, msg_seq, getpid())) < 0)
        return -1;

    rt_info = (struct route_info *)malloc(sizeof(struct route_info));

    //parse response
    for( ;NLMSG_OK(nlmsg, len); nlmsg = NLMSG_NEXT(nlmsg,len)) //iteration
    {
        memset(rt_info, 0, sizeof(struct route_info));
        parse_routes(nlmsg, rt_info); //extract nlmsg about address information

        //check if default gateway
        if (rt_info->dest_addr == 0x00000000) //dest address is "0.0.0.0"
        {
            *gateway = rt_info->gateway;
            ret = 0;
            break;
        }
    }

    free(rt_info);
    close(sock);

    return ret;
}

int read_nl_sock(int sock, char * buf, unsigned int seq, unsigned int pid)
{
    struct nlmsghdr * nlhdr;
    int read_len = 0, msg_len = 0;

    do
    {
        //recieve response from the kernel
        if((read_len = recv(sock, buf, BUF_SIZE - msg_len, 0)) < 0)
            return -1;

        nlhdr = (struct nlmsghdr *)buf;

        //check if the header is valid
        if((NLMSG_OK(nlhdr, read_len) == 0) || (nlhdr->nlmsg_type == NLMSG_ERROR))
            return -1;

        //check if the its the last message
        if(nlhdr->nlmsg_type == NLMSG_DONE)
            break;
        else
        {
            //move the pointer to buffer appropriately
            buf += read_len;
            msg_len += read_len;
        }

        //check if its a multi part message
        if((nlhdr->nlmsg_flags & NLM_F_MULTI) == 0)
            break; //return if its not

    }while((nlhdr->nlmsg_seq != seq) || (nlhdr->nlmsg_pid != pid));

    return msg_len;
}

void parse_routes(struct nlmsghdr * nlhdr, struct route_info * rt_info)
{
    struct rtmsg * rtmsg;
    struct rtattr * rtattr;
    int rtlen;

    rtmsg = (struct rtmsg *)NLMSG_DATA(nlhdr); //get data

    //if the route is not for IPv4 or does not belong to main routing table then return
    if((rtmsg->rtm_family != AF_INET) || (rtmsg->rtm_table != RT_TABLE_MAIN))
        return;

    //get the rtattr field
    rtattr = (struct rtattr *)RTM_RTA(rtmsg);
    rtlen = RTM_PAYLOAD(nlhdr);

    for( ;RTA_OK(rtattr, rtlen); rtattr = RTA_NEXT(rtattr, rtlen)) //iteration
    {
        switch(rtattr->rta_type)
        {
        case RTA_OIF:
            if_indextoname(*(int *)RTA_DATA(rtattr), rt_info->if_name);
            break;
        case RTA_GATEWAY:
            memcpy(&rt_info->gateway, RTA_DATA(rtattr), sizeof(rt_info->gateway));
            break;
        case RTA_PREFSRC:
            memcpy(&rt_info->src_addr, RTA_DATA(rtattr), sizeof(rt_info->src_addr));
            break;
        case RTA_DST:
            memcpy(&rt_info->dest_addr, RTA_DATA(rtattr), sizeof(rt_info->dest_addr));
            break;
        }
    }
}
