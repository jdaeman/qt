#ifndef PACKET_TYPES_H
#define PACKET_TYPES_H

#include <net/ethernet.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include<QString>

extern void detail_handle(struct ether_header * eth, QString& str);
extern unsigned short ether_handle(struct ether_header * eth, QString& str);
extern void arp_handle(struct ether_arp * arp, QString& str);
extern unsigned char ip_handle(struct iphdr * ip, QString& str);
extern void icmp_handle(struct icmphdr * icmp, QString& str);
extern void igmp_handle(struct igmp * igmp, QString& str);
extern unsigned short tcp_handle(struct tcphdr * tcp, QString& str);
extern unsigned short udp_handle(struct udphdr * udp, QString& str);

extern void packet_title(char * buf, QString& str);

extern int create_arp_packet(unsigned char * buf, int op, unsigned char * dhost, unsigned char * shost,
                      unsigned char * sha, unsigned char * tha,
                      unsigned int * spa, unsigned int * tpa);

#endif // PACKET_TYPES_H

