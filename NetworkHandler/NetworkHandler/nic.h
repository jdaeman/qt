#ifndef NIC_H
#define NIC_H

struct nic
{
    int index;
    char name[16];

    unsigned int ip;
    unsigned int subnet;
    unsigned char mac[6];

    unsigned int route;
    unsigned char neigh[6];

    int mtu;
};

int init_nic(void);
struct nic * get_nic(int idx);

#endif // NIC_H

