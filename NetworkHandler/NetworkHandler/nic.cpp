#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include "nic.h"
#include <util.h>

struct nic nics[8];

int init_nic(void)
{
    struct if_nameindex * if_arr, * itf;
    struct ifreq ifr;
    struct sockaddr_in * saddr;
    int sock, cnt = 0;

    if_arr = if_nameindex();
    if(!if_arr)
        return -1;

    sock = socket(PF_INET, SOCK_DGRAM, 0); //create temp socket
    if(sock < 0)
        return -1;

    for(itf = if_arr; itf->if_index != 0 || itf->if_name != NULL; itf++)
    {
        nics[cnt].index = itf->if_index;
        strcpy(nics[cnt].name, itf->if_name);

        strcpy(ifr.ifr_name, itf->if_name);

        ioctl(sock, SIOCGIFADDR, &ifr); //get interface ip
        saddr = (struct sockaddr_in *)(&ifr.ifr_addr);
        nics[cnt].ip = saddr->sin_addr.s_addr;

        ioctl(sock, SIOCGIFNETMASK, &ifr); //get ip netmask
        saddr = (struct sockaddr_in *)(&ifr.ifr_netmask);
        nics[cnt].subnet = saddr->sin_addr.s_addr;

        ioctl(sock, SIOCGIFHWADDR, &ifr); //get interface mac
        memcpy(nics[cnt].mac, ifr.ifr_hwaddr.sa_data, 6);

        get_gw_info(itf->if_index, &nics[cnt].route, nics[cnt].neigh);

        cnt++;
    }

    close(sock);
    if_freenameindex(if_arr);
    return cnt;
}

struct nic * get_nic(int idx)
{
    if (idx >= 8)
        return NULL;

    return nics + idx;
}
