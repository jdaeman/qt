#ifndef _SNIFF_H_
#define _SNIFF_H_


#ifdef __cplusplus
	extern "C"
	{
#endif
	int sniff(unsigned char * buff, int buff_len);

	int sniff_init(int ifindex, int promisc);

	int sniff_exit(void);

	void sniff_set_filter(int * filter, int len);

	enum {
		ALL = 0,
		ARP,
		ICMP,
		IGMP,
		UDP,
		TCP
	};

#ifdef __cplusplus
	}
#endif

#endif
