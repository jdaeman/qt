#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>

#include <arpa/inet.h>

#define NETLINK_BUFSIZE 4096

#define AA

struct gateway
{
	unsigned int route;
	unsigned char neigh[6];
};

static struct gateway * table[8];
static int count;

static unsigned char * neigh_lookup(int address)
{
	int cnt = 0;

	for (; cnt < count; cnt++)
	{
		if (table[cnt]->route == address)
			return table[cnt]->neigh;
	}

	return NULL;
}

static int send_req(int nl_sock, int type, int * nlseq)
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

static int recv_rep(int nl_sock, unsigned char * buf, int nlseq)
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

static void parse_route(struct nlmsghdr * nlmsg)
{
	struct rtmsg * rtmsg;
	struct rtattr * attr;
	unsigned int ip;
	int len;
	
	rtmsg = (struct rtmsg *)(NLMSG_DATA(nlmsg));
	//Not IPv4 or Not Main routing table
	if (rtmsg->rtm_family != PF_INET || rtmsg->rtm_table != RT_TABLE_MAIN)
		return;

	attr = (struct rtattr *)(RTM_RTA(rtmsg));
	len = RTM_PAYLOAD(nlmsg);

	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
	{
		if (attr->rta_type != RTA_GATEWAY)
			continue;

		ip = *(unsigned int *)(RTA_DATA(attr));

		if (count >= 8)
			return;

		table[count] = malloc(sizeof(struct gateway));
		table[count++]->route = ip;
	}
}

static void parse_neigh(struct nlmsghdr * nlmsg)
{
	struct ndmsg * ndmsg;
	struct rtattr * attr;
	int len;
	unsigned int ip;
	unsigned char * ptr;

	ndmsg = (struct ndmsg *)(NLMSG_DATA(nlmsg));
	if (ndmsg->ndm_family != PF_INET)
		return;

	attr = (struct rtattr *)(RTM_RTA(ndmsg));
	len = RTM_PAYLOAD(nlmsg);

	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
	{
		if (attr->rta_type == NDA_LLADDR) //Link-Layer ADDRess
		{
			memcpy(ptr, RTA_DATA(attr), 6);
		}
		else if (attr->rta_type == NDA_DST) //DeSTinsation
		{
			ip = *(unsigned int *)(RTA_DATA(attr));
			if (!(ptr = neigh_lookup(ip)))
				return;
		}
		else
			return;
	}
}

static void parse_rep(unsigned char * buf, int tot_len,
		void (parse)(struct nlmsghdr *))
{
	struct nlmsghdr * nlmsg;

	nlmsg = (struct nlmsghdr *)buf;
	for (; NLMSG_OK(nlmsg, tot_len); nlmsg = NLMSG_NEXT(nlmsg, tot_len))
	{
		parse(nlmsg);
	}
}

static int get_gw_info(void)
{
	int nl_sock;
	int nlseq = 0, msg_len = 0, err;
	unsigned char buf[NETLINK_BUFSIZE];
	
	nl_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (nl_sock < 0)
		return -errno;

	//Lookup Routing table
	if (send_req(nl_sock, RTM_GETROUTE, &nlseq) < 0)
		goto err;
	if ((msg_len = recv_rep(nl_sock, buf, nlseq)) < 0)
		goto err;
	parse_rep(buf, msg_len, parse_route);

	//Lookup Neighbour table
	if (send_req(nl_sock, RTM_GETNEIGH, &nlseq) < 0)
		goto err;
	if ((msg_len = recv_rep(nl_sock, buf, nlseq)) < 0)
		goto err;
	parse_rep(buf, msg_len, parse_neigh);

	close(nl_sock);
	return 0;

err:
	err = -errno;
	close(nl_sock);
	return err;
}

static void table_clear()
{
	int cnt = 0;
	for (; cnt < count; cnt++)
	{
		free(table[cnt]);
		table[cnt] = NULL;
	}
	count = 0;
}

int route_init(void)
{
	table_clear();	
	return get_gw_info();
}

void route_exit(void)
{
	table_clear();
}

int default_route_lookup(unsigned int ip, unsigned int subnet, unsigned int * route, unsigned char * neigh)
{
	int cnt = 0;
	unsigned int host_network = ip & subnet;

	for (; cnt < count; cnt++)
	{
		if (host_network != ((table[cnt]->route) & subnet)) //Careful, operation priority
			continue;

		if (route)
			memcpy(route, &table[cnt]->route, 4);
		if (neigh)
			memcpy(neigh, table[cnt]->neigh, 6);
		return 0;
	}

	return -512;
}
