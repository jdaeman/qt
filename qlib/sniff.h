#ifndef _SNIFF_H_
#define _SNIFF_H_

#ifdef __cplusplus
	extern "C"
	{
#endif
	int sniff(unsigned char * buff, int buff_len);

	int sniff_init(int ifindex, int promisc);

	int sniff_exit(void);

#ifdef __cplusplus
	}
#endif

#endif
