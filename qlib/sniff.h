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

	int packet_parse(unsigned char * pkt, unsigned char * summ);

	const unsigned char * get_parsed(int layer);

#ifdef __cplusplus
	}
#endif

#endif
