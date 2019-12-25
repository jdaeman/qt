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
#include <linux/icmp.h>
#include <linux/igmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include "util.h"
#include "sniff.h"

static int sniffer = -1; //sniffer socket

typedef unsigned char * (*parse)(unsigned char *, unsigned char *, unsigned char * summ);
static parse next = NULL;

static unsigned char l2[512], l3[512], l4[512], l5[65536];
static unsigned char * parsed[] = {NULL, NULL, l2, l3, l4, l5};

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
	return len; 
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
	/*long long list = 0;
	int s;

	for (s = 0; s < len; s++)
	{
		int v = filter[s];

		list |= (1 << v);
	}
	
	protos = list;*/
}

unsigned char * ip_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
	struct iphdr * ip = (struct iphdr *)pkt;
	unsigned char src[32], dst[32];
	unsigned short frag = ntohs(ip->frag_off);

	strcpy(src, inet_ntoa_e(ip->saddr));
	strcpy(dst, inet_ntoa_e(ip->daddr));

	if (summ)
	{
		strcpy((char *)summ, "More headers...");
		return NULL;
	}

	sprintf(buf, "IP Datagram\n"
		"\tVersion: %d\tHeader Length: %d\n"
		"\tToS: %d\n"
		"\tTotal Length: %d\n"
		"\tIdentification: %0#4x\n"
		"\tFlag(ZERO): %d\n"
		"\tDon`t Fragment: %d\n"
		"\tMore Fragment: %d\n"
		"\tFragment Offset: %d\n"
		"\tTTL: %d\tProtocol: %d\n"
		"\tCheck Sum: %#04x\n"
		"\tSource: %s\n"
		"\tDestination: %s\n",
		ip->version, ip->ihl * 4, ip->tos,
		ntohs(ip->tot_len),
		ntohs(ip->id),
		(frag & 0x8000) ? 1 : 0, (frag & 0x4000) ? 1 : 0, (frag & 0x2000) ? 1 : 0, frag & 0x1fff,
		ip->ttl, ip->protocol,
		ip->check,
		src, dst);

	next = NULL;

	return (unsigned char *)(ip + 1);
}

unsigned char * arp_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
	struct arphdr * arp = (struct arphdr *)pkt;
	unsigned char * payload = (unsigned char *)(arp + 1);
	unsigned short op = ntohs(arp->ar_op);

	const char * operations[] = {"X", "Request", "Reply"};

	unsigned char sha[32], tha[32];
	unsigned char sip[16], tip[16];

	next = NULL;

	strcpy(sha, ether_ntoa_e(payload, 6));
	payload += 6;
	strcpy(sip, inet_ntoa_e(*(unsigned int *)payload));
	payload += 4;
	strcpy(tha, ether_ntoa_e(payload, 6));
	payload += 6;
	strcpy(tip, inet_ntoa_e(*(unsigned int *)payload));

	if (summ)
	{
		sprintf(summ, "ARP %s %s -> %s", operations[op], sip, tip);
		return NULL;
	}

	sprintf(buf, "Address Resolution Protocol\n"
		"\tHardware Type: %d\t\tProtocol Type: %#04x\n"
		"\tHardware Address Length: %d\tProtocol Address Length: %d\n"
		"\tOperation: %d[%s]\n"
		"\tSource Hardware: %s\n"
		"\tSource IP: %s\n"
		"\tDestination Hareware: %s\n"
		"\tDestination IP: %s\n",
		ntohs(arp->ar_hrd), ntohs(arp->ar_pro),
		arp->ar_hln, arp->ar_pln,
		op, operations[op],
		sha, sip, tha, tip);

	return NULL;
}

unsigned char * eth_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
	struct ethhdr * eth = (struct ethhdr *)pkt;
	unsigned char src[32], dst[32];
	unsigned short proto;

	strcpy(src, ether_ntoa_e(eth->h_source, 6));
	strcpy(dst, ether_ntoa_e(eth->h_dest, 6));
	proto = ntohs(eth->h_proto);

	sprintf(buf, "Ethernet Frame\n"
		"\tSource: %s\n"
		"\tDestination: %s\n"
		"\tProtocol: %#04x\n",
		src, dst, proto);

	if (proto == ETH_P_IP)
		next = ip_handle;
	else if (proto == ETH_P_ARP)
		next = arp_handle;
	else
		next = NULL;

	return (unsigned char *)(eth + 1);
}

int packet_parse(unsigned char * pkt, unsigned char * summ)
{
	unsigned char * ptr = pkt;
	int layer = 2;

	if (!pkt)
		return -1;

	ptr = eth_handle(ptr, parsed[layer], summ);
	while (ptr && next)
	{
		layer++;
		ptr = next(ptr, parsed[layer], summ);
	}

	return layer;
}

const unsigned char * get_parsed(int layer)
{
	if (layer > 5)
		return NULL;

	return parsed[layer];
}
