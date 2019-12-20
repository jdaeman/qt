#include "sniffer.h"

#include <unistd.h> //close()
#include <sys/socket.h> //socket()
#include <arpa/inet.h> //htons()
#include <net/ethernet.h> //
#include <linux/if_packet.h> //
#include <netinet/if_ether.h> //
#include <netinet/ip.h> //

#include <errno.h>
#include <consts.h>
#include <string.h>
#include <lock.h>

#include <QDebug>

#define BUF_SIZE 65536

sniffer::sniffer(struct nic * nic, unsigned short proto)
{
    this->sel = nic;
    this->protocol = proto;
    this->sock = -1;
}

sniffer::~sniffer()
{
    if(this->sock > 0)
        ::close(this->sock);
}

void sniffer::run()
{
    //qDebug() << getpid() << " " << pthread_self() << " " << QThread::currentThreadId();

    struct sockaddr_ll device;
    memset(&device, 0, sizeof(device));
    device.sll_ifindex = this->sel->if_idx;
    device.sll_halen = ETH_ALEN;
    memcpy(device.sll_addr, this->sel->if_paddr, 6);
    device.sll_family = PF_PACKET;
    device.sll_protocol = ETH_P_ALL;
    unsigned int device_len = sizeof(device);

    this->sock = this->create_sniff_sock(this->sel->if_idx);
    if(this->sock < 0)
        goto err_handle;

    char buf[BUF_SIZE], cpy[BUF_SIZE];
    int size;
    while(true) //maybe need to synchronization, but i ignore this thing
    {
        memset(buf, 0, BUF_SIZE);
        size = recvfrom(this->sock, buf, BUF_SIZE, 0, (struct sockaddr *)&device, &device_len);
        if(size <= 0)
            goto err_handle;

        struct ether_header * eth = (struct ether_header *)buf;
        unsigned short type = ntohs(eth->ether_type);

        if(protocol == ETH_P_ALL)
            goto send_to_dialog;
        else if(protocol == type)
            goto send_to_dialog;
        else if(type == ETH_P_IP)
        {
            struct iphdr * ip = (struct iphdr *)(eth + 1);

            if(protocol == ip->protocol)
                goto send_to_dialog;
            else
                continue;
        }
        else
            continue;

send_to_dialog:
        memset(cpy, 0, BUF_SIZE);

        pthread_mutex_lock(&mutex);
        memcpy(cpy, buf, size);
        pthread_mutex_unlock(&mutex);

        emit sig_to_sniff_dialog(to_sniff_dialog::PKT_CAP, cpy, size);
    }

    this->sniff_sock_close();
    return;

err_handle:
        if(this->sock > 0)
            this->sniff_sock_close();
        emit sig_to_sniff_dialog(to_sniff_dialog::ERR, strerror(errno), 0);
}

int sniffer::create_sniff_sock(int if_index)
{
    int sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(sock < 0)
        return -1;

    struct sockaddr_ll sll;
    struct packet_mreq mr;

    memset(&sll, 0, sizeof(sll));
    sll.sll_family = PF_PACKET;
    sll.sll_ifindex = if_index;
    sll.sll_protocol = htons(ETH_P_ALL);

    memset(&mr, 0, sizeof(mr));
    mr.mr_ifindex = if_index;
    mr.mr_type = PACKET_MR_PROMISC;

    if(bind(sock, (struct sockaddr *)&sll, sizeof(sll)) < 0)
        goto err_handle;

    if(setsockopt(sock, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) < 0)
        goto err_handle;

    return sock;

err_handle:
    ::close(sock);
    return -1;
}

void sniffer::sniff_sock_close()
{
    ::close(this->sock);
    this->sock = -1;
}
