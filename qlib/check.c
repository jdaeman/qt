#include <stdio.h>
#include "util.h"

int main(void)
{
	unsigned int ip;
	unsigned char mac[6];

	get_gw_info(2, &ip, mac);

	printf("%x\n", ip);
	printf("%x:%x:%x:%x:%x:%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return 0;
}

