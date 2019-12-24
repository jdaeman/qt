#include <linux/if_ether.h>
//#include <linux/if_arp.h>
struct arphdr {
    __be16		ar_hrd;		/* format of hardware address	*/
    __be16		ar_pro;		/* format of protocol address	*/
    unsigned char	ar_hln;		/* length of hardware address	*/
    unsigned char	ar_pln;		/* length of protocol address	*/
    __be16		ar_op;		/* ARP opcode (command)		*/

#if 0
     /*
      *	 Ethernet looks like this : This bit is variable sized however...
      */
    unsigned char		ar_sha[ETH_ALEN];	/* sender hardware address	*/
    unsigned char		ar_sip[4];		/* sender IP address		*/
    unsigned char		ar_tha[ETH_ALEN];	/* target hardware address	*/
    unsigned char		ar_tip[4];		/* target IP address		*/
#endif

};

#include <linux/ip.h>
#include <linux/icmp.h>
#include <linux/igmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include <stdio.h>
#include <cstring>
#include <arpa/inet.h>

#include "util.h"

typedef unsigned char * (*parse)(unsigned char *, unsigned char *, unsigned char * summ);
static parse next = NULL;

unsigned char * ip_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
    struct iphdr * ip = (struct iphdr *)pkt;
    char src[32], dst[32];
    unsigned short frag = ntohs(ip->frag_off);

    strcpy(src, (char *)inet_ntoa_e(ip->saddr));
    strcpy(dst, (char *)inet_ntoa_e(ip->daddr));

    sprintf((char *)buf, "IP Datagram\n"
                 "Version: %d\tHeader Length: %d\tToS: %d\n"
                 "Total Length: %d\n"
                 "Identification: %d\n"
                 "Flag: %d\n"
                 "Don`t Fragment: %d\n"
                 "More Fragment: %d\n"
                 "Fragment Offset: %d\n"
                 "TTL: %d\tProtocol: %d\n"
                 "Check Sum: %d"
                 "Source: %s\tDestination: %s\n",
            ip->version, ip->ihl, ip->tos,
            ntohs(ip->tot_len),
            ntohs(ip->id),
            frag & 0x8000, frag & 0x4000, frag & 0x2000, frag & 0x1fff,
            ip->ttl, ip->protocol,
            ip->check,
            src, dst);

    strcpy((char *)summ, "More headers...");
    next = NULL;

    return (unsigned char *)(ip + 1);
}

unsigned char * arp_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
    struct arphdr * arp = (struct arphdr *)pkt;
    unsigned char * payload = (unsigned char *)(arp + 1);
    unsigned short op = ntohs(arp->ar_op);

    char sha[32], tha[32];
    char sip[16], tip[16];

    strcpy(sha, (char *)ether_ntoa_e(payload, 6));
    payload += 6;
    strcpy(sip, (char *)inet_ntoa_e(*(unsigned int *)payload));
    payload += 4;
    strcpy(tha, (char *)ether_ntoa_e(payload, 6));
    payload += 6;
    strcpy(tip, (char *)inet_ntoa_e(*(unsigned int *)payload));

    sprintf((char *)buf, "%d %d %d %d %d %s %s %s %s",
            ntohs(arp->ar_hrd), ntohs(arp->ar_pro),
            arp->ar_hln, arp->ar_pln,
            op,
            sha, sip, tha, tip);


    strcpy((char *)summ, "ARP");
    return NULL;
}

unsigned char * eth_handle(unsigned char * pkt, unsigned char * buf, unsigned char * summ)
{
    struct ethhdr * eth = (struct ethhdr *)pkt;
    /*struct ethhdr * eth = (struct ethhdr *)pkt;
    unsigned char src[32], dst[32];
    unsigned short proto;

    strcpy(src, ether_ntoa_e(eth->h_source, 6));
    strcpy(dst, ether_ntoa_e(eth->h_dest, 6));
    proto = ntohs(eth->h_proto);

    sprintf(buf, "Ethernet Frame\n"
                 "Source: %s\tDestination: %s\n"
                 "Protocol: %04x\n",
            src, dst, proto);

    if (proto == ETH_P_IP)
        next = ip_handle;
    else if (proto == ETH_P_ARP)
        next = arp_handle;
    else
        next = NULL;*/

    return (unsigned char *)(eth + 1);
}

int packet_parse(unsigned char * pkt, unsigned char * summ)
{
    unsigned char parse[3][1024]; //Except Physcial, App Layer,
    unsigned char * ptr = pkt;
    int layer = 0;

    if (!pkt || !summ)
        return -1;

    ptr = eth_handle(ptr, parse[layer++], summ);
    while (ptr && next)
    {
        ptr = next(ptr, parse[layer++], summ);
    }

    return layer;
}
