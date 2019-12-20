#ifndef NIC_H
#define NIC_H

struct nic
{
    char if_name[32]; //interface name

    int if_idx; //interface index

    unsigned int if_laddr; //interface logical address
    unsigned int if_netmask; //interface net mask
    unsigned char if_paddr[6]; //interface physical address

    unsigned int gateway; //gateway address
};

extern int nic_init(struct nic * list, unsigned int len);

#endif // NIC_H

