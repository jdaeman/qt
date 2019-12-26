#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include <curl/curl.h>

#define NETLINK_BUFSIZE 4096

int get_domain_ip(const char * domain, unsigned int * als, int len)
{
	struct hostent * hostent = gethostbyname(domain);
	int count;

	if (!hostent)
		return -1;

	for (count = 0; count < len && hostent->h_addr_list[count]; count++)
		als[count] = *(unsigned int *)hostent->h_addr_list[count];

	return count;
}

unsigned short cksum(unsigned char * buff, int len)
{
	unsigned short * buf = (unsigned short *)buff;
	unsigned int sum = 0;

	for (; len > 0; len -= 2)
	{
		if (len == 1)
			sum += (unsigned short)(*(unsigned char *)buf);
		else
			sum += *(buff++);

		while (sum & 0xffff0000)
		{
			unsigned int carry = (sum & 0xffff0000) >> 16;
			sum &= 0x0000fffff;
			sum += carry;
		}
	}
	
	return (unsigned short)~sum;
}

unsigned char * ether_ntoa_e(unsigned char * mac, int len)
{
	static unsigned char hf_mac[32];

	if (len < 6)
		return NULL;

	sprintf(hf_mac, "%02x:%02x:%02x:%02x:%02x:%02x",
			mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

	return hf_mac;
}

unsigned char * inet_ntoa_e(unsigned int ip)
{
	return inet_ntoa(*(struct in_addr *)&ip);
}

int send_req(int nl_sock, int type, int * nlseq)
{
	struct nlmsghdr * nlmsg;
	unsigned char buf[NETLINK_BUFSIZE];

	memset(buf, 0, sizeof(buf));
	nlmsg = (struct nlmsghdr *)buf;

	nlmsg->nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)); //routing message length
	nlmsg->nlmsg_type = type;
	nlmsg->nlmsg_flags = NLM_F_DUMP | NLM_F_REQUEST; //default request setting
	nlmsg->nlmsg_seq = (*nlseq)++; //sequence number
	nlmsg->nlmsg_pid = getpid(); //per process

	return send(nl_sock, buf, nlmsg->nlmsg_len, 0); //send to netlink socket
}

int recv_rep(int nl_sock, unsigned char * buf, int nlseq)
{
	unsigned char * ptr;
	int len, tot_len = 0;
	struct nlmsghdr * nlmsg;
	pid_t pid = getpid();
	
	ptr = buf;
	do
	{
		if ((len = recv(nl_sock, ptr, NETLINK_BUFSIZE - tot_len, 0)) < 0)
			return -1;
		
		nlmsg = (struct nlmsghdr *)ptr;
		if (NLMSG_OK(nlmsg, len) == 0 || nlmsg->nlmsg_type == NLMSG_ERROR)
			return -1;
		if (nlmsg->nlmsg_type == NLMSG_DONE)
			break;

		ptr += len; //next position
		tot_len += len;

		if ((nlmsg->nlmsg_flags & NLM_F_MULTI) == 0)
			break;
	} while (nlmsg->nlmsg_seq != nlseq || nlmsg->nlmsg_pid != pid);

	return tot_len;
}

int parse_route(struct nlmsghdr * nlmsg, void * gw)
{
	struct rtmsg * rtmsg;
	struct rtattr * attr;
	int len, index;
	unsigned int ip;
	char ** ref = (char **)gw;

	static int cnt = 1;
	
	if (!gw)
	{
		cnt = 1;
		return -1;
	}

	index = *((int *)ref[0]);
	rtmsg = (struct rtmsg *)(NLMSG_DATA(nlmsg));
	if (rtmsg->rtm_family != PF_INET || rtmsg->rtm_table != RT_TABLE_MAIN)
		return -1;

	attr = (struct rtattr *)(RTM_RTA(rtmsg));
	len = RTM_PAYLOAD(nlmsg);

	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
	{
		if (attr->rta_type != RTA_GATEWAY)
			continue;

		ip = *(unsigned int *)(RTA_DATA(attr));

		if (index == cnt)
		{
			cnt = 1;
			memcpy(ref[1], &ip, sizeof(ip));
			return 0;
		}
		else
		{
			cnt += 1;
			break;
		}

		//printf("%d, %s\n", cnt++, inet_ntoa(*(struct in_addr *)&ip));
	}

	return cnt;
}

int parse_neigh(struct nlmsghdr * nlmsg, void * gw)
{
	struct ndmsg * ndmsg;
	struct rtattr * attr;
	int len;
	unsigned int ip;
	unsigned char mac[6];
	char ** ref = (char **)gw;

	if (!gw)
		return -1;

	ndmsg = (struct ndmsg *)(NLMSG_DATA(nlmsg));
	if (ndmsg->ndm_family != PF_INET)
		return -1;

	attr = (struct rtattr *)(RTM_RTA(ndmsg));
	len = RTM_PAYLOAD(nlmsg);

	for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len))
	{
		if (attr->rta_type == NDA_LLADDR) //Link-Layer ADDRess
		{
			memcpy(mac, RTA_DATA(attr), 6);
			//printf("%d, %02x:%02x:%02x:%02x:%02x:%02x\n", cnt,
				//mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
		}

		if (attr->rta_type == NDA_DST) //DeSTinsation
		{
			ip = *(unsigned int *)(RTA_DATA(attr));
			//printf("%d, %s\n", cnt, inet_ntoa(*(struct in_addr *)&ip));
		}
	}

	if (*((unsigned int *)ref[1]) == ip)
	{
		memcpy(ref[2], mac, sizeof(mac));
		return 0;
	}
	
	return 1; //cont
}

void parse_rep(unsigned char * buf, int tot_len, void * arg,
		int (parse)(struct nlmsghdr *, void *))
{
	struct nlmsghdr * nlmsg;

	nlmsg = (struct nlmsghdr *)buf;
	for (; NLMSG_OK(nlmsg, tot_len); nlmsg = NLMSG_NEXT(nlmsg, tot_len))
	{
		if (!parse(nlmsg, arg))
			break;
	}

	parse(nlmsg, NULL);
}

int get_gw_info(int ifindex, unsigned int * ip, unsigned char * mac)
{
	int nl_sock;
	int nlseq = 0, msg_len = 0;
	unsigned char buf[NETLINK_BUFSIZE];
	void * ref[] = {&ifindex, ip, mac};

	if (!ip || !mac)
		return -1;
	ifindex -= 1;
	
	nl_sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
	if (nl_sock < 0)
		return -1;

	//Lookup Routing table
	if (send_req(nl_sock, RTM_GETROUTE, &nlseq) < 0)
		goto err;
	if ((msg_len = recv_rep(nl_sock, buf, nlseq)) < 0)
		goto err;
	parse_rep(buf, msg_len, ref, parse_route);
	
	//Lookup Neighbour table
	if (send_req(nl_sock, RTM_GETNEIGH, &nlseq) < 0)
		goto err;
	if ((msg_len = recv_rep(nl_sock, buf, nlseq)) < 0)
		goto err;
	parse_rep(buf, msg_len, ref, parse_neigh);

	close(nl_sock);
	return 0;

err:
	close(nl_sock);
	return -1;
}

char * get_public_ip(void)
{
	int sock, off;
	struct sockaddr_in serv;
	const char * query = "GET /ip HTTP/1.1\r\n"
			"Host: ifconfig.me\r\n"
			"Connection: keep-alive\r\n"
			"\r\n";
	char * cur, * prev = NULL;
	//checkip.amazonaws.com//

#ifndef BUFSZ
#define BUFSZ 512
#endif
	static unsigned int ip[1];	
	static char ans[BUFSZ];

	if (!ip[0] && (get_domain_ip("ifconfig.me", ip, 1) < 0))
		return NULL;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
		return NULL;

	memset(ans, 0, sizeof(ans));	
	memset(&serv, 0, sizeof(serv));
	serv.sin_family = PF_INET;
	serv.sin_addr.s_addr = ip[0];
	serv.sin_port = htons(80); //HTTP
	
	if (connect(sock, (struct sockaddr *)&serv, sizeof(serv)) < 0)
		goto err;
	if (send(sock, query, strlen(query), 0) < 0)
		goto err;
	if (recv(sock, ans, BUFSZ - 1, 0) < 0)
		goto err;

	cur = strtok(ans, "\r\n");
	prev = cur;

	//check HTTP code
	for (off = 0; cur[off] != 0 && cur[off++] != ' ';);
	if (cur[off] != '2')
		goto err;
	
	while ((cur = strtok(NULL, "\r\n")))
		prev = cur;

	close(sock);
	return prev;

err:
	close(sock);
	return NULL;
}

/*size_t write_callback(char * ptr, size_t size, size_t nmemb, void * userdata)
{
	strncpy(userdata, ptr, 63);
	return size * nmemb;
}*/

/*char * get_vendor(unsigned char * mac)
{
	static char vendor[64];
	char url[128] = "https://api.macvendors.com/";
	CURL * curl = curl_easy_init();
	CURLcode res;	

	if (!curl)
		return NULL;

	strcat(url, mac);
	memset(vendor, 0, sizeof(vendor));

	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, vendor);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

	res = curl_easy_perform(curl);
	if (res != CURLE_OK)
		return NULL;
	
	return vendor;
}*/	


size_t write_callback(char * ptr, size_t size, size_t nmemb, void * userdata)
{
    //printf("%s\n", ptr);
    //qDebug() << size * nmemb << endl << endl;

    return size * nmemb;
}

void testtest()
{
    CURL * ctx = curl_easy_init();

    curl_easy_setopt(ctx, CURLOPT_URL, "http://ip-api.com/line/24.48.0.1");
    //curl_easy_setopt(ctx, CURLOPT_WRITEDATA, pkt1);
    curl_easy_setopt(ctx, CURLOPT_WRITEFUNCTION, write_callback);

    const CURLcode rc = curl_easy_perform(ctx);
    if (rc != CURLE_OK)
        return;
    else
        return;
}
