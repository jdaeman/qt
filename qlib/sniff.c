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
	int len;	
	int off;
	struct ethhdr * eth;
	struct iphdr * ip;

	len = recvfrom(sniffer, buff, buff_len, 0, NULL, NULL);

	if (len <= 0)
		return -errno;

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

unsigned char * udp_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
	struct udphdr * udp = (struct udphdr *)pkt;

	next = NULL;
	if (summ)
	{
		sprintf(summ, "UDP %d -> %d", ntohs(udp->source), ntohs(udp->dest));
		return NULL;
	}

	sprintf(buf, "UDP Segment\n"
		"\tSource: %d\n"
		"\tDestination: %d\n"
		"\tLength: %d\n"
		"\tCheck sum: %#04x",
		ntohs(udp->source), ntohs(udp->dest),
		ntohs(udp->len), udp->check);

	return NULL;
}

unsigned char * tcp_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
	struct tcphdr * tcp = (struct tcphdr *)pkt;
	unsigned short flag;
	int off = 0;

	static const char * op[] = {"Fin ", "Syn ", "Rst ", "Psh ", "Ack ", "Urg ", "Ece ", "Cwr"};
	
	memcpy(&flag, pkt + 12, 2);
	flag = ntohs(flag);

	next = NULL;
	if (summ)
	{
		unsigned char flags[64];

		memset(flags, 0, sizeof(flags));
		for (; off < 8; off++)
		{
			if (flag & (1 << off))
				strcat(flags, op[off]);
		}
	
		sprintf(summ, "TCP %s %u -> %u", flags, ntohs(tcp->source), ntohs(tcp->dest));
		return NULL;
	}

	sprintf(buf, "TCP Segment\n"
		"\tSource: %d\n"
		"\tDestination: %d\n"
		"\tSequence: %d\n"
		"\tAcknowledgement: %d\n"
		"\tHeader Length: %d\n"
		"\tCwr: %d\tEce: %d\n"
		"\tUrg: %d\tAck: %d\n"
		"\tPsh: %d\tRst: %d\n"
		"\tSyn: %d\tFin: %d\n"
		"\tWindow Size: %d\n"
		"\tCheck Sum: %#04x\n"
		"\tUrgent Offset: %d",
		ntohs(tcp->source), ntohs(tcp->dest),
		ntohs(tcp->seq), ntohs(tcp->ack_seq),
		tcp->doff * 4,
		tcp->cwr, tcp->ece, tcp->urg, tcp->ack,
		tcp->psh, tcp->rst, tcp->syn, tcp->fin,
		ntohs(tcp->window), tcp->check,
		ntohs(tcp->urg_ptr)); 

	return NULL;
}

unsigned char * icmp_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
	struct icmphdr * icmp = (struct icmphdr *)pkt;
	
	if (summ)
	{
		sprintf(summ, "ICMP type: %d, code: %d", icmp->type, icmp->code);
		return NULL;
	}

	sprintf(buf, "Internet Control Message Protocol\n"
		"\tType: %d\n"
		"\tcode: %d\n"
		"\tCheck sum: %#04x\n",
		icmp->type, icmp->code, icmp->checksum);

	next = NULL;
	return NULL;
}

unsigned char * igmp_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
	struct igmphdr * igmp = (struct igmphdr *)pkt;
	unsigned char grp[32];

	next = NULL;
	if (summ)
	{
		sprintf(summ, "IGMP type: %d, code: %d", igmp->type, igmp->code);
		return NULL;
	}

	strcpy(grp, inet_ntoa_e(igmp->group));
	sprintf(buf, "Internet Group Management Protocol\n"
		"\tType: %d\n"
		"\tCode: %d\n"
		"\tCheck Sum: %#04x\n"
		"\tGroup IP: %s", 
		igmp->type, igmp->code,
		igmp->csum, grp);	

	return NULL;
}

unsigned char * ip_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
	struct iphdr * ip = (struct iphdr *)pkt;
	unsigned char src[32], dst[32];
	unsigned short frag = ntohs(ip->frag_off);

	if (ip->protocol == IPPROTO_ICMP)
		next = icmp_handle;
	else if (ip->protocol == IPPROTO_UDP)
		next = udp_handle;
	else if (ip->protocol == IPPROTO_TCP)
		next = tcp_handle;
	else if (ip->protocol == IPPROTO_IGMP)
		next = igmp_handle;
	else
		next = NULL;

	if (summ)
	{
		if (!next)
			sprintf(summ, "IP: Not-registered: %d", ip->protocol);

		return (unsigned char *)(ip + 1);
	}

	strcpy(src, inet_ntoa_e(ip->saddr));
	strcpy(dst, inet_ntoa_e(ip->daddr));

	sprintf(buf, "Internet Protocol Datagram\n"
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
		"\tDestination: %s",
		ip->version, ip->ihl * 4, ip->tos,
		ntohs(ip->tot_len),
		ntohs(ip->id),
		(frag & 0x8000) ? 1 : 0, (frag & 0x4000) ? 1 : 0, (frag & 0x2000) ? 1 : 0, frag & 0x1fff,
		ip->ttl, ip->protocol,
		ip->check,
		src, dst);

	
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
		"\tDestination IP: %s",
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
		"\tProtocol: %#04x",
		src, dst, proto);

	if (proto == ETH_P_IP)
		next = ip_handle;
	else if (proto == ETH_P_ARP)
		next = arp_handle;
	else
		next = NULL;

	if (!next && summ)
		sprintf(summ, "ETH: Not-registered: %#04x", proto);

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
