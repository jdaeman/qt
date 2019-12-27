#ifndef _ARP_H_
#define _ARP_H_

#ifdef __cplusplus
extern "C"
{
#endif

int arp_init(int idx, int ip, int netmask, unsigned char * mac);
int arp_exit();

int scanning(int start, int end, void (func)(unsigned int, unsigned char *));



#ifdef __cplusplus
}
#endif

#endif
