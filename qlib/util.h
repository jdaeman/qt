#ifndef _UTIL_H_
#define _UTIL_H_

#ifdef __cplusplus
	extern "C"
	{
#endif

	//fail-> return -1
	//success -> return count
	int get_domain_ip(const char * domain, unsigned int * als, int len);

	//return check sum
	unsigned short cksum(unsigned char * buff, int len);

	//return readable mac address
	unsigned char * ether_ntoa_e(unsigned char * mac, int len);

	//return readable ip address
	unsigned char * inet_ntoa_e(unsigned int ip);

	//fail -> return -1
	//success -> return 0
	int get_gw_info(int ifindex, unsigned int * ip, unsigned char * mac);

	char * get_public_ip(void);
	
	void testtest();

	//return vendor name
	//char * get_vendor(unsigned char * mac);

#ifdef __cplusplus
	}
#endif

#endif
