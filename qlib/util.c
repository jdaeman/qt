#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <arpa/inet.h>

//#include <curl/curl.h>

#define NETLINK_BUFSIZE 4096

int get_domain_ip(const char * domain, unsigned int * als, int len)
{
	struct hostent * hostent = gethostbyname(domain);
	int count;

	if (!hostent)
		return -1;

	for (count = 0; count < len && hostent->h_addr_list[count]; count++)
		als[count] = *(unsigned int *)hostent->h_addr_list[count];

	return count;
}

unsigned short cksum(unsigned char * buff, int len)
{
	unsigned short * buf = (unsigned short *)buff;
	unsigned int sum = 0;

	for (; len > 0; len -= 2)
	{
		if (len == 1)
			sum += (unsigned short)(*(unsigned char *)buf);
		else
			sum += *(buff++);

		while (sum & 0xffff0000)
		{
			unsigned int carry = (sum & 0xffff0000) >> 16;
			sum &= 0x0000fffff;
			sum += carry;
		}
	}
	
	return (unsigned short)~sum;
}

unsigned char * ether_ntoa_e(unsigned char * mac, int len)
{
	static unsigned char hf_mac[32];

	if (len < 6)
		return NULL;

	sprintf(hf_mac, "%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return hf_mac;
}

int send_req(int nl_sock, int type, int * nlseq)
{
	struct nlmsghdr * nlmsg;
	unsigned char buf[NETLINK_BUFSIZE];

	memset(buf, 0, sizeof(buf));
	nlmsg = (struct nlmsghdr *)buf;

	nlmsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); //routing message length
	nlmsg->nlmsg_type = type;
	nlmsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; //default request setting
	nlmsg->nlmsg_seq = (*nlseq)++; //sequence number
	nlmsg->nlmsg_pid = getpid(); //per process

	return send(nl_sock, buf, nlmsg->nlmsg_len, 0); //send to netlink socket
}

int recv_rep(int nl_sock, unsigned char * buf, int nlseq)
{
	unsigned char * ptr;
	int len, tot_len = 0;
	struct nlmsghdr * nlmsg;
	pid_t pid = getpid();
	
	ptr = buf;
	do
	{
		if ((len = recv(nl_sock, ptr, NETLINK_BUFSIZE - tot_len, 0)) < 0)
			return -1;
		
		nlmsg = (struct nlmsghdr *)ptr;
		if (NLMSG_OK(nlmsg, len) == 0 || nlmsg->nlmsg_type == NLMSG_ERROR)
			return -1;
		if (nlmsg->nlmsg_type == NLMSG_DONE)
			break;

		ptr += len; //next position
		tot_len += len;

		if ((nlmsg->nlmsg_flags & NLM_F_MULTI) == 0)
			break;
	} while (nlmsg->nlmsg_seq != nlseq || nlmsg->nlmsg_pid != pid);

	return tot_len;
}

void parse_route(struct nlmsghdr * nlmsg, void * gw)
{
	struct rtmsg * rtmsg;
	struct rtattr * attr;
	int len;
	unsigned int ip;

	static int cnt = 1;

	rtmsg = (struct rtmsg *)(NLMSG_DATA(nlmsg));
	if (rtmsg->rtm_family != PF_INET || rtmsg->rtm_table != RT_TABLE_MAIN)
		return;

	attr = (struct rtattr *)(RTM_RTA(rtmsg));
	len = RTM_PAYLOAD(nlmsg);

	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
	{
		if (attr->rta_type != RTA_GATEWAY)
			continue;

		ip = *(unsigned int *)(RTA_DATA(attr));
		printf("%d, %s\n", cnt++, inet_ntoa(*(struct in_addr *)&ip));
	}
}

void parse_neigh(struct nlmsghdr * nlmsg, void * gw)
{
	struct ndmsg * ndmsg;
	struct rtattr * attr;
	int len;
	unsigned int ip;
	unsigned char mac[6];

	static int cnt = 1;

	ndmsg = (struct ndmsg *)(NLMSG_DATA(nlmsg));
	if (ndmsg->ndm_family != PF_INET)
		return;

	attr = (struct rtattr *)(RTM_RTA(ndmsg));
	len = RTM_PAYLOAD(nlmsg);

	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
	{
		if (attr->rta_type == NDA_LLADDR) //Link-Layer ADDRess
		{
			memcpy(mac, RTA_DATA(attr), 6);
			printf("%d, %02x:%02x:%02x:%02x:%02x:%02x\n", cnt,
				mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		}

		if (attr->rta_type == NDA_DST) //DeSTinsation
		{
			ip = *(unsigned int *)(RTA_DATA(attr));
			printf("%d, %s\n", cnt, inet_ntoa(*(struct in_addr *)&ip));
		}
	}

	cnt++;
}

void parse_rep(unsigned char * buf, int tot_len, void * arg,
		void (parse)(struct nlmsghdr *, void *))
{
	struct nlmsghdr * nlmsg;
	struct rtmsg * rtmsg;
	struct ndmsg * ndmsg;
	struct rtattr * attr;

	nlmsg = (struct nlmsghdr *)buf;
	for (; NLMSG_OK(nlmsg, tot_len); nlmsg = NLMSG_NEXT(nlmsg, tot_len))
		parse(nlmsg, arg);
}

int get_gw_info(int ifindex, unsigned int * ip, unsigned char * mac)
{
	int nl_sock;
	int nlseq = 0, msg_len = 0;
	unsigned char buf[NETLINK_BUFSIZE];
	
	nl_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (nl_sock < 0)
		return -1;

	//Lookup Routing table
	if (send_req(nl_sock, RTM_GETROUTE, &nlseq) < 0)
		goto err;
	if ((msg_len = recv_rep(nl_sock, buf, nlseq)) < 0)
		goto err;
	parse_rep(buf, msg_len, ip, parse_route);

	puts("");
	
	//Lookup Neighbour table
	if (send_req(nl_sock, RTM_GETNEIGH, &nlseq) < 0)
		goto err;
	if ((msg_len = recv_rep(nl_sock, buf, nlseq)) < 0)
		goto err;
	parse_rep(buf, msg_len, mac, parse_neigh);

	close(nl_sock);
	return 0;

err:
	close(nl_sock);
	return -1;
}


/*size_t write_callback(char * ptr, size_t size, size_t nmemb, void * userdata)
{
	strncpy(userdata, ptr, 63);
	return size * nmemb;
}

char * get_vendor(unsigned char * mac)
{
	static char vendor[64];
	char url[128] = "https://api.macvendors.com/";
	CURL * curl = curl_easy_init();
	CURLcode res;	

	if (!curl)
		return NULL;

	strcat(url, mac);
	memset(vendor, 0, sizeof(vendor));

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, vendor);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		return NULL;
	
	return vendor;
}*/	
