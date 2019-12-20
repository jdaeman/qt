#include <net/ethernet.h>
#include <netinet/ether.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netinet/igmp.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <QString>

void icmp_type(struct icmphdr * icmp, char * cpy);
void tcp_type(struct tcphdr * tcp, char * cpy);

unsigned short ether_handle(struct ether_header * eth, QString& str)
{
    QString temp;

    str.sprintf("----------Ethernet----------\n");
    str.append(temp.sprintf("\tdest: %s\t", ether_ntoa((struct ether_addr *)eth->ether_dhost)));
    str.append(temp.sprintf("src: %s\n", ether_ntoa((struct ether_addr *)eth->ether_shost)));

    return ntohs(eth->ether_type);
}

void arp_handle(struct ether_arp * arp, QString& str)
{
    QString temp;
    unsigned short type = ntohs(arp->ea_hdr.ar_op);

    switch(type)
    {
    case ARPOP_REQUEST:
        temp.sprintf("Request----------\n");
        break;
    case ARPOP_REPLY:
        temp.sprintf("Reply----------\n");
        break;
    default:
        temp.sprintf("%#04x----------\n", type);
    }

    str.sprintf("----------ARP "); str.append(temp);
    str.append(temp.sprintf("\tsrc: %s(%s)\n", ether_ntoa((struct ether_addr *)arp->arp_sha),
                            inet_ntoa(*(struct in_addr *)arp->arp_spa)));
    str.append(temp.sprintf("\tdest: %s(%s)\n", ether_ntoa((struct ether_addr *)arp->arp_tha),
                            inet_ntoa(*(struct in_addr *)arp->arp_tpa)));
}

unsigned char ip_handle(struct iphdr * ip, QString& str)
{
    QString temp;
    unsigned short frag = ntohs(ip->frag_off);
    unsigned short more_frag = (frag >> 13) & 0x0001;
    unsigned short dont_frag = (frag >> 14) & 0x0001;
    frag = frag & 0x1fff;

    str.sprintf("----------IP----------\n");
    str.append(temp.sprintf("\ttotal length: %d\tttl: %d\n", ntohs(ip->tot_len), ip->ttl));
    str.append(temp.sprintf("\tsrc: %s\t", inet_ntoa(*(struct in_addr *)&ip->saddr)));
    str.append(temp.sprintf("dest: %s\n", inet_ntoa(*(struct in_addr *)&ip->daddr)));
    str.append(temp.sprintf("\tid: %d\n", ntohs(ip->id)));
    str.append(temp.sprintf("\tDF: %d\tMF: %d\toffset: %d\n", dont_frag, more_frag, frag));

    return ip->protocol;
}

void icmp_handle(struct icmphdr * icmp, QString& str)
{
    QString temp;
    unsigned char * t = (unsigned char *)(icmp + 1);

    str.sprintf("----------ICMP----------\n");
    str.append(temp.sprintf("\ttype: %d\tcode: %d\n", icmp->type, icmp->code));

    str.append("----------DATA(Hexa)----------\n");
    while(t && *t)
        str.append(temp.sprintf("%02x ", *(t++)));
}

void igmp_handle(struct igmp * igmp, QString& str)
{
    QString temp;

    str.sprintf("----------IGMP----------\n");
    str.append(temp.sprintf("\ttype: %d\tcode: %d\n", igmp->igmp_type, igmp->igmp_code));
    str.append(temp.sprintf("\tgroup address: %s\n", inet_ntoa(*(struct in_addr *)&igmp->igmp_group)));
}

unsigned short tcp_handle(struct tcphdr * tcp, QString& str)
{
    QString temp;
    unsigned char * t = (unsigned char *)(tcp + 1);

    str.sprintf("----------TCP----------\n");
    str.append(temp.sprintf("\tsrc: %d\tdest: %d\n", ntohs(tcp->source), ntohs(tcp->dest)));
    str.append(temp.sprintf("\tseq: %d\tack_seq: %d\n", ntohs(tcp->seq), ntohs(tcp->ack_seq)));
    str.append(temp.sprintf("\toffset: %d\n", tcp->doff));
    str.append(temp.sprintf("\tFIN(%d)\tSYN(%d)\tACK(%d)\tRST(%d)\n",
                            tcp->fin, tcp->syn, tcp->ack, tcp->rst));
    str.append(temp.sprintf("\tPSH(%d)\tURG(%d)\tECE(%d)\tCWR(%d)\n",
                            tcp->psh, tcp->urg, tcp->ece, tcp->cwr));

    str.append("----------DATA(Hexa)----------\n");
    while(t && *t)
        str.append(temp.sprintf("%02x ", *(t++)));

    return 0;
}

unsigned short udp_handle(struct udphdr * udp, QString& str)
{
    QString temp;
    unsigned char * t = (unsigned char *)(udp + 1);

    str.sprintf("----------UDP----------\n");
    str.append(temp.sprintf("\tsrc: %d\tdest: %d\n", ntohs(udp->source), ntohs(udp->dest)));
    str.append(temp.sprintf("\tlen: %d\n", ntohs(udp->len)));

    str.append("----------DATA(Hexa)----------\n");
    while(t && *t)
        str.append(temp.sprintf("%02x ", *(t++)));

    return 0;
}

void detail_handle(struct ether_header * eth, QString& str)
{
    QString temp;

    unsigned short type = ether_handle(eth, str);

    if(type == ETH_P_ARP)
    {
        struct ether_arp * arp = (struct ether_arp *)(eth + 1);
        arp_handle(arp, temp); //ARP Header
        str.append(temp);
    }
    else if(type == ETH_P_IP)
    {
        struct iphdr * ip = (struct iphdr *)(eth + 1); //IP Header
        type = ip_handle(ip, temp);
        str.append(temp);

        switch(type)
        {
        case IPPROTO_ICMP:
            icmp_handle((struct icmphdr *)(ip + 1), temp); //ICMP Header
            str.append(temp);
            break;
        case IPPROTO_IGMP:
            igmp_handle((struct igmp *)(ip + 1), temp);
            str.append(temp);
            break;
        case IPPROTO_TCP:
            tcp_handle((struct tcphdr *)(ip + 1), temp);
            str.append(temp);
            break;
        case IPPROTO_UDP:
            udp_handle((struct udphdr *)(ip + 1), temp);
            str.append(temp);
            break;
        }
    }
}

void packet_title(char * buf, QString& str)
{
    struct ether_header * eth = (struct ether_header *)buf;
    char * next_header = (char *)(eth + 1);
    unsigned short type = ntohs(eth->ether_type);
    char temp[32];

    if(type == ETH_P_ARP)
    {
        struct ether_arp * arp = (struct ether_arp *)next_header;
        type = ntohs(arp->ea_hdr.ar_op);

        switch(type)
        {
        case ARPOP_REQUEST:
            str.sprintf("ARP Request");
            break;
        case ARPOP_REPLY:
            str.sprintf("ARP Reply");
            break;
        default:
            str.sprintf("ARP %04x", type);
            break;
        }
    }
    else if(type == ETH_P_IP)
    {
        struct iphdr * ip = (struct iphdr *)next_header;
        type = ip->protocol;
        next_header = (char *)(ip + 1);

        switch(type)
        {
        case IPPROTO_ICMP:
            icmp_type((struct icmphdr *)next_header, temp);
            str.sprintf("ICMP - %s", temp);
            break;
        case IPPROTO_IGMP:
            str.sprintf("IGMP");
            break;
        case IPPROTO_TCP:
            tcp_type((struct tcphdr *)next_header, temp);
            str.sprintf("TCP - %s", temp);
            break;
        case IPPROTO_UDP:
            str.sprintf("UDP");
            break;
        default:
            str.sprintf("%d", type);
            break;
        }
    }
    else
    {
        str.sprintf("%#04x", type);
    }
}

void icmp_type(struct icmphdr * icmp, char * cpy)
{
    if(icmp->type == 0 && icmp->code == 0)
        strcpy(cpy, "Echo Reply");
    else if(icmp->type == 8 && icmp->code == 0)
        strcpy(cpy, "Echo Request");
}

void tcp_type(struct tcphdr * tcp, char * cpy)
{
    if(tcp->fin)
        strcpy(cpy, "FIN");
    else if(tcp->syn)
        strcpy(cpy, "SYN");
    else if(tcp->ack)
        strcpy(cpy, "ACK");
    else if(tcp->urg)
        strcpy(cpy, "URG");
    else
        strcpy(cpy, "UNKNOWN");
}

int create_arp_packet(unsigned char * buf, int op, unsigned char * dhost, unsigned char * shost,
                      unsigned char * sha, unsigned char * tha,
                      unsigned int * spa, unsigned int * tpa)
{
    struct ether_header * eth = (struct ether_header *)buf;
    struct ether_arp * arp = (struct ether_arp *)(eth + 1);

    //set ethernet header
    memcpy(eth->ether_dhost, dhost, 6);
    memcpy(eth->ether_shost, shost, 6);
    eth->ether_type = htons(ETH_P_ARP);

    //set arp header
    arp->ea_hdr.ar_hrd = htons(1);
    arp->ea_hdr.ar_pro = htons(ETH_P_IP);
    arp->ea_hdr.ar_hln = 6;
    arp->ea_hdr.ar_pln = 4;
    arp->ea_hdr.ar_op = htons(op);

    //set arp information
    memcpy(arp->arp_sha, sha, 6);
    memcpy(arp->arp_spa, spa, 4);
    memcpy(arp->arp_tha, tha, 6);
    memcpy(arp->arp_tpa, tpa, 4);

    return sizeof(struct ether_header) + sizeof(struct ether_arp);
}

