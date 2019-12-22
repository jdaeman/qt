#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/ip.h>

#include "sniff.h"

static int sniffer = -1; //sniffer socket
static long long protos = 1; //default is all

int set_dev_bind(int sock, int ifindex)
{
	struct sockaddr_ll sll;
	
	memset(&sll, 0, sizeof(sll));
	sll.sll_family = PF_PACKET;
	sll.sll_ifindex = ifindex;
	sll.sll_protocol = htons(ETH_P_ALL);
	
	if (bind(sock, (struct sockaddr *)&sll, sizeof(sll)) < 0)
		return -1;

	return 0;
}

int set_dev_promisc(int sock, int ifindex)
{
	struct packet_mreq pm;
	
	memset(&pm, 0, sizeof(pm));
	pm.mr_ifindex = ifindex;
	pm.mr_type = PACKET_MR_PROMISC;

	if (setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &pm, sizeof(pm)) < 0)
		return -1;

	return 0;
}

int sniff(unsigned char * buff, int buff_len)
{
	int len = recvfrom(sniffer, buff, buff_len - 1, 0, NULL, NULL);	
	int off;
	struct ethhdr * eth;
	struct iphdr * ip;

	if (len <= 0)
		return -errno;

	buff[len] = 0;

	if (protos & ALL) //ALL
		return len;
			
	eth = (struct ethhdr *)buff;
	if (ntohs(eth->h_proto) == ETH_P_ARP && protos & (1 << ARP))
		return len;

	ip = (struct iphdr *)(eth + 1);
	switch(ip->protocol)
	{
	case IPPROTO_ICMP:
		off = ICMP;
		break;
	case IPPROTO_IGMP:
		off = IGMP;
		break;
	case IPPROTO_TCP:
		off = TCP;
		break;
	case IPPROTO_UDP:
		off = UDP;
		break;
	deafult:
		return 0;	
	}

	if (protos & (1 << off))
		return len;

	return 0; 
}

int sniff_init(int ifindex, int promisc)
{
	int err;

	if (sniffer != -1)
		return -512;

	sniffer = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (sniffer < 0)
		return -errno;
	if (set_dev_bind(sniffer, ifindex) < 0)
		goto err;
	if (promisc != 0 && set_dev_promisc(sniffer, ifindex) < 0)
		goto err;

	return 0;

err:
	err = errno;
	close(sniffer);
	return -err;
}

int sniff_exit(void)
{
	if (close(sniffer) < 0)
		return -errno;

	sniffer = -1;
	return 0;
}

void sniff_set_filter(int * filter, int len)
{
	long long list = 0;
	int s;

	for (s = 0; s < len; s++)
	{
		int v = filter[s];

		list |= (1 << v);
	}
	
	protos = list;	
}

/*int main()
{
	if (sniff(2, 1, SIGINT, NULL, NULL) < 0)
	{
		
	}
	return 0;
}*/
