/* project: miniUPnP
 * webpage: http://miniupnp.free.fr/
 * (c) 2006 Thomas Bernard
 * This software is subject to the conditions detailed in the
 * LICENCE file provided in the distribution */
/* $Id: miniigd.c,v 1.19 2009/10/26 12:09:23 bert Exp $ */
/* system or libc include : */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/file.h>
#include <syslog.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
/* for BSD's sysctl */
#include <sys/param.h>
#include <sys/sysctl.h>
#include <errno.h>
#include <ctype.h>
#if 0
//#ifdef SUPPORT_HNAP
#include <sys/un.h>
#define UNIX_SOCK_PATH "/tmp/mysocket"
#endif
#include <linux/wireless.h>
#include <sys/ioctl.h>
#ifdef KERNEL_3_10
#include <linux/sysinfo.h>
#endif
#ifdef KERNEL_2_6_30
#include <sys/sysinfo.h>
#endif 
/* miniupnp includes : */
#include "upnpglobalvars.h"
#include "upnphttp.h"
#include "upnpdescgen.h"
#include "miniupnpdpath.h"
#include "getifaddr.h"
#include "daemonize.h"
#include "upnpsoap.h"
#include "built_time"
#include "upnputils.h"
#define VERSION_STR	"v1.09.1"
#define DISPLAY_BANNER \
	printf("\nMiniIGD %s (%s).\n\n", VERSION_STR, BUILT_TIME)
#if defined(DEBUG)
#define DEBUG_ERR(fmt, args...) printf(fmt, ## args)
#else
#define DEBUG_ERR(fmt, args...)
#endif
#ifdef ENABLE_EVENTS
#include "upnpevents.h"
#endif

#if defined(ENABLE_NATPMP)
#include "natpmp.h"
#endif

#define IGD_FILE "/tmp/igd_config"
#define IGD_FIREWALL "/tmp/firewall_igd"
#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
#define UPNP_INIT "/tmp/upnp_init"
#endif
/* The amount of time before advertisements
   will expire */
#define DEFAULT_ADVR_EXPIRE 180
//Brad add 20081205
#define PICS_INIT_PORT		52869
#define WAN_IF_1 ("eth1")
#define WAN_IF_2 ("ppp0")
#define WAN_IF_2_1 ("ppp1")
#define WAN_IF_2_2 ("ppp2")
#define WAN_IF_2_3 ("ppp3")
#define WAN_IF_3 ("wlan")
#define CHECK_WAN_INTERVAL 1 /* seconds to check WAN status */
typedef enum { DHCP_DISABLED=0, DHCP_CLIENT=1, DHCP_SERVER=2, PPPOE=3, PPTP=4, BIGPOND=5, L2TP=6, PPPOE_RU=7, PPTP_RU=8, L2TP_RU=9 } DHCP_T;

#ifdef STAND_ALONE
#define PICS_DESC_NAME "picsdesc"
#define MINIIGD_SERVER_STRING "OS 1.0 UPnP/1.0 Realtek/V1.3"
#endif
//#define PSIMPLECFG_SERVICE_DESC_DOC "simplecfgservice"

/* ip et port pour le SSDP */
#define PORT (1900)
#define UPNP_MCAST_ADDR ("239.255.255.250")
//#ifdef ENABLE_IPV6
#define LL_SSDP_MCAST_ADDR "FF02::C"
#define SL_SSDP_MCAST_ADDR "FF05::C"
//#endif

//#undef ENABLE_6FC_SERVICE

#ifdef ENABLE_6FC_SERVICE
void init_iptpinhole(void);
#endif

#define MAX_ADD_LISTEN_ADDR (4)

static volatile int quitting = 0;
char internal_if[MAX_LEN_IF_NAME];
//static int wan_type = -1;
//static int wisp_interface_num = -1;
//static char wisp_if_name[8];
int wan_type = -1;
int wisp_interface_num = -1;
char wisp_if_name[8];
static int need_add_iptables=0;
static int sessionSel=-1;	//0: ppp0; 1: ppp1; 2: ppp0 & ppp1.
#ifdef MULTI_PPPOE
static int sessionNumber = 1; //default one ppp connect
static int rule_set_number = 0; //
int cur_session = -1;	//used for loop pppx,eg: ppp0->ppp1->ppp2->ppp3->ppp0
char deviceName[4][32];
#endif

#ifdef CONFIG_RTL8186_GR
int pppCutEnabled=1;	//(only used for pppoe) 0: disabled; 1: enabled (default)
#endif
#ifdef ENABLE_IPV6
/* ipv6 address used for HTTP */
char ipv6_addr_for_http_with_brackets[64];
#endif

extern int is_wan_connected(void);

extern int backup_rules(char *);
extern int recover_rules(void);

char *get_token(char *data, char *token)
{
	char *ptr=data;
	int len=0, idx=0;

	while (*ptr && *ptr != '\n' ) {
		if (*ptr == '=') {
			if (len <= 1)
				return NULL;
			memcpy(token, data, len);

			/* delete ending space */
			for (idx=len-1; idx>=0; idx--) {
				if (token[idx] !=  ' ')
					break;
			}
			token[idx+1] = '\0';

			return ptr+1;
		}
		len++;
		ptr++;
	}
	return NULL;
}

int get_value(char *data, char *value)
{
	char *ptr=data;	
	int len=0, idx, i;

	while (*ptr && *ptr != '\n' && *ptr != '\r') {
		len++;
		ptr++;
	}

	/* delete leading space */
	idx = 0;
	while (len-idx > 0) {
		if (data[idx] != ' ') 
			break;	
		idx++;
	}
	len -= idx;

	/* delete bracing '"' */
	if (data[idx] == '"') {
		for (i=idx+len-1; i>idx; i--) {
			if (data[i] == '"') {
				idx++;
				len = i - idx;
			}
			break;
		}
	}

	if (len > 0) {
		memcpy(value, &data[idx], len);
		value[len] = '\0';
	}
	return len;
}

static unsigned char convert_atob(char *data, int base)
{
	char tmpbuf[10];
	int bin;

	memcpy(tmpbuf, data, 2);
	tmpbuf[2]='\0';
	if (base == 16)
		sscanf(tmpbuf, "%02x", &bin);
	else
		sscanf(tmpbuf, "%02d", &bin);
	return((unsigned char)bin);
}
static int read_config_file(char *filename, int port_num)
{
	FILE *fp;
	char line[200], token[40], value[100], *ptr=NULL;
	int i;
	char tmp_url[100];
	char url_port[10];
	fp = fopen(filename, "r");
	if (fp == NULL) {
		DEBUG_ERR("read config file [%s] failed!\n", filename);
		return -1;
	}
	while ( fgets(line, 200, fp) ) {
		if (line[0] == '#')
			continue;
		ptr = get_token(line, token);
		if (ptr == NULL)
			continue;
		if (get_value(ptr, value)==0)
			continue;
		else if (!strcmp(token, "uuid")) {
			if (strlen(value) != UUID_LEN) {
				DEBUG_ERR("Invalid uuid length!\n");
				return -1;
			}
			sprintf(uuidvalue, "uuid:%s", value);
		}	
		else if (!strcmp(token, "server_name")) {
			if (strlen(value) > (MAX_SERVER_LEN-1)) {
				DEBUG_ERR("Invalid manufacturer length [%d]!\n", strlen(value));
				return -1;
			}
			sprintf(server_id, "%s", value);
		}
		else if (!strcmp(token, "location_url")) {
			if (strlen(value) > (MAX_SERVER_LEN-1)) {
				DEBUG_ERR("Invalid location URL length [%d]!\n", strlen(value));
				return -1;
			}
			ptr=strstr(value, "PORT");
			if(ptr!=NULL){
			snprintf(tmp_url, (ptr-value+1), "%s",value);
			sprintf(url_port, "%d", port_num);
			strcat(tmp_url, url_port);
			strcat(tmp_url, ptr+4);
			}
			sprintf(location_url, "%s", tmp_url);
		}
		else if (!strcmp(token, "ssdp_ext")) {
			if (strlen(value) > (MAX_SERVER_LEN-1)) {
				DEBUG_ERR("Invalid SSDP EXT length [%d]!\n", strlen(value));
				return -1;
			}
			sprintf(ssdp_ext, "%s", value);
		}
		else if (!strcmp(token,"ssdp_mx")) 
					ssdp_mx = atoi(value);		
	}
	fclose(fp);

	return 0;
}





static int OpenAndConfHTTPSocket(unsigned short port)
{
	int s;
	int i = 1;
#ifndef ENABLE_IPV6
	struct sockaddr_in listenname;
#else
	struct sockaddr_in6 listenname;
#endif
	socklen_t listenname_len;
	s = socket(
#ifndef ENABLE_IPV6
	PF_INET, 
#else
	PF_INET6,
#endif
	SOCK_STREAM, 0);
	if(s<0)
	{
		syslog(LOG_ERR, "socket(http): %m");
		return s;
	}
	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
	{
		syslog(LOG_WARNING, "setsockopt(http, SO_REUSEADDR): %m");
	}
#ifdef ENABLE_IPV6
	memset(&listenname, 0, sizeof(struct sockaddr_in6));
	listenname.sin6_family = AF_INET6;
	listenname.sin6_port = htons(port);
	listenname.sin6_addr = in6addr_any;
	listenname_len =  sizeof(struct sockaddr_in6);
#else
	memset(&listenname, 0, sizeof(struct sockaddr_in));
	listenname.sin_family = AF_INET;
	listenname.sin_port = htons(port);
	listenname.sin_addr.s_addr = htonl(INADDR_ANY);
	listenname_len =  sizeof(struct sockaddr_in);
#endif
	if(bind(s, (struct sockaddr *)&listenname, listenname_len) < 0)
	{
		syslog(LOG_ERR, "bind(http): %m");
		close(s);
		return -1;
	}
	if(listen(s, 6) < 0)
	{
		syslog(LOG_ERR, "listen(http): %m");
		close(s);
		return -1;
	}
	return s;
}

/*
 * response from a LiveBox (Wanadoo)
HTTP/1.1 200 OK
CACHE-CONTROL: max-age=1800
DATE: Thu, 01 Jan 1970 04:03:23 GMT
EXT:
LOCATION: http://192.168.0.1:49152/gatedesc.xml
SERVER: Linux/2.4.17, UPnP/1.0, Intel SDK for UPnP devices /1.2
ST: upnp:rootdevice
USN: uuid:75802409-bccb-40e7-8e6c-fa095ecce13e::upnp:rootdevice

 * response from a Linksys 802.11b :
HTTP/1.1 200 OK
Cache-Control:max-age=120
Location:http://192.168.5.1:5678/rootDesc.xml
Server:NT/5.0 UPnP/1.0
ST:upnp:rootdevice
USN:uuid:upnp-InternetGatewayDevice-1_0-0090a2777777::upnp:rootdevice
EXT:
 */

/* not really an SSDP "announce" as it is the response
 * to a SSDP "M-SEARCH" */
static void SendSSDPAnnounce2(int s, struct sockaddr* sockname,
                              const char * st, int st_len,
							  const char * host, unsigned short port)
{
	int l, n;
	char buf[512];
	socklen_t addrlen;
	/* TODO :
	 * follow guideline from document "UPnP Device Architecture 1.0"
	 * put in uppercase.
	 * DATE: is recommended
	 * SERVER: OS/ver UPnP/1.0 miniupnpd/1.0
	 * */
	l = snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\n"
		"Cache-Control: max-age=120\r\n"
		"ST: %.*s\r\n"
		"USN: %s::%.*s\r\n"
		"EXT:\r\n"
		"Server: miniupnpd/1.0 UPnP/1.0\r\n"
		"Location: http://%s:%u" ROOTDESC_PATH "\r\n"
		"\r\n",
		st_len, st,
		uuidvalue, st_len, st,
		host, (unsigned int)port);
	addrlen = (sockname ->sa_family == AF_INET6) ? 
		sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
	n = sendto(s, buf, l, 0,sockname, addrlen );
	if(n<0)
	{
		syslog(LOG_ERR, "sendto: %m");
	}
}

static const char * const known_service_types[] =
{
	"upnp:rootdevice",
	"urn:schemas-upnp-org:device:InternetGatewayDevice:",
	"urn:schemas-upnp-org:device:WANConnectionDevice:",
	"urn:schemas-upnp-org:device:WANDevice:",
	"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:",
	"urn:schemas-upnp-org:service:WANIPConnection:",
	"urn:schemas-upnp-org:service:WANPPPConnection:",
#ifdef ENABLE_6FC_SERVICE
	"urn:schemas-upnp-org:service:WANIPv6FirewallControl:",
#endif
	//"urn:schemas-upnp-org:service:Layer3Forwarding:",
	//"urn:schemas-microsoft-com:service:OSInfo:",
	0
};

#ifdef STAND_ALONE
static void
SendSSDPNotifies(int s, const char * host, unsigned short type,
                 unsigned int lifetime, int ipv6)
{
#ifdef ENABLE_IPV6
	struct sockaddr_storage sockname;
#else
	struct sockaddr_in sockname;
#endif
	int l, n, i=0;
	char* bufr =NULL;
	bufr = (char *) malloc(512);
	if (bufr == NULL) {
		syslog(LOG_ERR, "SendSSDPNotifies: out of memory!");
		return;
	}
	memset(bufr, 0, 512);
	memset(&sockname, 0, sizeof(sockname));
#ifdef ENABLE_IPV6
	if(ipv6)
	{
		struct sockaddr_in6 * p = (struct sockaddr_in6 *)&sockname;
		p->sin6_family = AF_INET6;
		p->sin6_port = htons(PORT);
		inet_pton(AF_INET6, LL_SSDP_MCAST_ADDR, &(p->sin6_addr));
	}
	else
#endif
	{
		struct sockaddr_in *p = (struct sockaddr_in *)&sockname;
		p->sin_family = AF_INET;
		p->sin_port = htons(PORT);
		p->sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);
	}

	while(known_service_types[i])
	{
		if(i==1){
		 l= sprintf(bufr,
					"NOTIFY * HTTP/1.1\r\n"
					"Host:%s:%d\r\n"
					"Cache-Control:max-age=%d\r\n"
					"Location:http://%s:%d/%s.xml" "\r\n"
					"Server:" MINIIGD_SERVER_STRING "\r\n"
					"NT:%s\r\n"
					"USN:%s\r\n"
					"NTS:ssdp:%s\r\n"
					"\r\n",
					ipv6 ? "[" LL_SSDP_MCAST_ADDR "]" : UPNP_MCAST_ADDR,
					PORT, (ssdp_mx)?ssdp_mx:DEFAULT_ADVR_EXPIRE,
					host, PICS_INIT_PORT, PICS_DESC_NAME,
					uuidvalue, uuidvalue,
					(type==0?"alive":"byebye"));
		if(l<0)
		{
			syslog(LOG_ERR, "SendSSDPNotifies() snprintf error");
			continue;
		}
		if((unsigned int)l >= 512)
		{
			syslog(LOG_WARNING, "SendSSDPNotifies(): truncated output");
			l = 512;
		}
		n = sendto(s, bufr, l, 0,
			(struct sockaddr *)&sockname,
#ifdef ENABLE_IPV6
			ipv6 ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in)
#else
			sizeof(struct sockaddr_in)
#endif
			);
		if(n < 0)
		{
			/* XXX handle EINTR, EAGAIN, EWOULDBLOCK */
			syslog(LOG_ERR, "sendto(udp_notify=%d, %s): %m", s,
			       host ? host : "NULL");
		}
	}
		l=sprintf(bufr,
				"NOTIFY * HTTP/1.1\r\n"
				"Host:%s:%d\r\n"
				"Cache-Control:max-age=%d\r\n"
				"Location:http://%s:%d/%s.xml" "\r\n"
				"Server:" MINIIGD_SERVER_STRING "\r\n"
				"NT:%s%s\r\n"
				"USN:%s::%s%s\r\n"
				"NTS:ssdp:%s\r\n"
				"\r\n",
				ipv6 ? "[" LL_SSDP_MCAST_ADDR "]" : UPNP_MCAST_ADDR, 
				PORT, (ssdp_mx)?ssdp_mx:DEFAULT_ADVR_EXPIRE,
				host, PICS_INIT_PORT, PICS_DESC_NAME,
				known_service_types[i], (i==0?"":"1"),
				uuidvalue, known_service_types[i], (i==0?"":"1"),
				(type==0?"alive":"byebye"));
		if(l<0)
		{
			syslog(LOG_ERR, "SendSSDPNotifies() snprintf error");
			continue;
		}
		if((unsigned int)l >= 512)
		{
			syslog(LOG_WARNING, "SendSSDPNotifies(): truncated output");
			l = 512;
		}
		n = sendto(s, bufr, l, 0,
			(struct sockaddr *)&sockname,
#ifdef ENABLE_IPV6
			ipv6 ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in)
#else
			sizeof(struct sockaddr_in)
#endif
			);
		if(n < 0)
		{
			/* XXX handle EINTR, EAGAIN, EWOULDBLOCK */
			syslog(LOG_ERR, "sendto(udp_notify=%d, %s): %m", s,
			       host ? host : "NULL");
		}
		i++;
	}
	free(bufr);
}
#if 0
void SendSSDPNotifies(int s, const char * host, unsigned char type)
{
	struct sockaddr_in sockname;
	int n, i, j;
	char *bufr=NULL;

	if (host == NULL )
		return;

	bufr = (char *) malloc(512);
	if (bufr == NULL) {
		syslog(LOG_ERR, "SendSSDPNotifies: out of memory!");
		return;
	}
	memset(bufr, 0, 512);
	
	memset(&sockname, 0, sizeof(struct sockaddr_in));
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(PORT);
	sockname.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);

	for (j=0; j <2; j++) {
		i = 0;
		while(known_service_types[i])
		{
			if (i == 1) {
				sprintf(bufr,
					"NOTIFY * HTTP/1.1\r\n"
					"Host:%s:%d\r\n"
					"Cache-Control:max-age=%d\r\n"
					"Location:http://%s:%d/%s.xml" "\r\n"
					"Server:" MINIIGD_SERVER_STRING "\r\n"
					"NT:%s\r\n"
					"USN:%s\r\n"
					"NTS:ssdp:%s\r\n"
					"\r\n",
					UPNP_MCAST_ADDR, PORT, (ssdp_mx)?ssdp_mx:DEFAULT_ADVR_EXPIRE,
					host, PICS_INIT_PORT, PICS_DESC_NAME,
					uuidvalue, uuidvalue,
					(type==0?"alive":"byebye"));
				n = sendto(s, bufr, strlen(bufr), 0,
					(struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
				if(n<0)
				{
					syslog(LOG_ERR, "sendto: %m");
				}
			}
			sprintf(bufr,
				"NOTIFY * HTTP/1.1\r\n"
				"Host:%s:%d\r\n"
				"Cache-Control:max-age=%d\r\n"
				"Location:http://%s:%d/%s.xml" "\r\n"
				"Server:" MINIIGD_SERVER_STRING "\r\n"
				"NT:%s%s\r\n"
				"USN:%s::%s%s\r\n"
				"NTS:ssdp:%s\r\n"
				"\r\n",
				UPNP_MCAST_ADDR, PORT, (ssdp_mx)?ssdp_mx:DEFAULT_ADVR_EXPIRE,
				host, PICS_INIT_PORT, PICS_DESC_NAME,
				known_service_types[i], (i==0?"":"1"),
				uuidvalue, known_service_types[i], (i==0?"":"1"),
				(type==0?"alive":"byebye"));
			n = sendto(s, bufr, strlen(bufr), 0,
				(struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
			if(n<0)
			{
				syslog(LOG_ERR, "sendto: %m");
			}
			i++;
		}
	}

	free(bufr);
}
#endif
/* AddMulticastMembershipIPv6()
 * param s	socket (IPv6)
 * To be improved to target specific network interfaces */
#ifdef ENABLE_IPV6
static int
AddMulticastMembershipIPv6(int s)
{
	struct ipv6_mreq mr;
	/*unsigned int ifindex;*/

	memset(&mr, 0, sizeof(mr));
	inet_pton(AF_INET6, LL_SSDP_MCAST_ADDR, &mr.ipv6mr_multiaddr);
	/*mr.ipv6mr_interface = ifindex;*/
	mr.ipv6mr_interface = 0; /* 0 : all interfaces */
#ifndef IPV6_ADD_MEMBERSHIP
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#endif
	if(setsockopt(s, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mr, sizeof(struct ipv6_mreq)) < 0)
	{
		syslog(LOG_ERR, "setsockopt(udp, IPV6_ADD_MEMBERSHIP): %m");
		return -1;
	}
	inet_pton(AF_INET6, SL_SSDP_MCAST_ADDR, &mr.ipv6mr_multiaddr);
	if(setsockopt(s, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mr, sizeof(struct ipv6_mreq)) < 0)
	{
		syslog(LOG_ERR, "setsockopt(udp, IPV6_ADD_MEMBERSHIP): %m");
		return -1;
	}
	return 0;
}
#endif

int AddMulticastMembership(int s, const char * ifaddr)
{
	struct ip_mreq imr;	/* Ip multicast membership */

    	/* setting up imr structure */
    	imr.imr_multiaddr.s_addr = inet_addr(UPNP_MCAST_ADDR);
    	/*imr.imr_interface.s_addr = htonl(INADDR_ANY);*/
    	imr.imr_interface.s_addr = inet_addr(ifaddr);
	
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&imr, sizeof(struct ip_mreq)) < 0)
	{
        	syslog(LOG_ERR, "setsockopt(udp, IP_ADD_MEMBERSHIP): %m");
		return -1;
    	}

	return 0;
}
/* Open and configure the socket listening for
 * SSDP udp packets sent on 239.255.255.250 port 1900
 * SSDP v6 udp packets sent on FF02::C, or FF05::C, port 1900 */

int OpenAndConfUdpSocket(int ipv6)
{
	int s, on=1;
	struct sockaddr_storage sockname;
	socklen_t sockname_len;
	if( (s = socket(ipv6 ? PF_INET6 : PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		syslog(LOG_ERR, "socket(udp): %m");
		return -1;
	}


	memset(&sockname, 0, sizeof(struct sockaddr_storage));
	if(ipv6) {
		struct sockaddr_in6 * saddr = (struct sockaddr_in6 *)&sockname;
		saddr->sin6_family = AF_INET6;
		saddr->sin6_port = htons(PORT);
		saddr->sin6_addr = in6addr_any;
		sockname_len = sizeof(struct sockaddr_in6);
	}else{
		struct sockaddr_in *saddr = (struct sockaddr_in *)&sockname;
    	saddr->sin_family = AF_INET;
    	saddr->sin_port = htons(PORT);
	/* NOTE : it seems it doesnt work when binding on the specific address */
    	/* sockname.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR); */
    	saddr->sin_addr.s_addr = htonl(INADDR_ANY);
		sockname_len = sizeof(struct sockaddr_in);
    	/* sockname.sin_addr.s_addr = inet_addr(ifaddr); */
	}
	if( setsockopt( s, SOL_SOCKET, SO_REUSEADDR,(char *)&on, sizeof(on) ) != 0 ) {
                syslog(LOG_ERR, "setsockopt(udp): %m");
                return -1;
	}
    	if(bind(s, (struct sockaddr *)&sockname, sockname_len) < 0)
	{
		syslog(LOG_ERR, "bind(udp): %m");
		close(s);
		return -1;
    	}

#ifdef ENABLE_IPV6
	if(ipv6){
		if(AddMulticastMembershipIPv6(s) < 0){
			close(s);
			return -1;
		}
	}else{
#endif
		if(AddMulticastMembership(s, listen_addr) < 0){
			close(s);
			return -1;
		}
#ifdef ENABLE_IPV6

	}
#endif

	return s;
}

#ifdef ENABLE_IPV6
/* open the UDP socket used to send SSDP notifications to
 * the multicast group reserved for them. IPv6 */
static int
OpenAndConfSSDPNotifySocketIPv6(unsigned int if_index)
{
	int s;
	unsigned int loop = 0;

	s = socket(PF_INET6, SOCK_DGRAM, 0);
	if(s < 0)
	{
		syslog(LOG_ERR, "socket(udp_notify IPv6): %m");
		return -1;
	}
	if(setsockopt(s, IPPROTO_IPV6, IPV6_MULTICAST_IF, &if_index, sizeof(if_index)) < 0)
	{
		syslog(LOG_ERR, "setsockopt(udp_notify IPv6, IPV6_MULTICAST_IF, %u): %m", if_index);
		close(s);
		return -1;
	}
	if(setsockopt(s, IPPROTO_IPV6, IPV6_MULTICAST_LOOP, &loop, sizeof(loop)) < 0)
	{
		syslog(LOG_ERR, "setsockopt(udp_notify, IPV6_MULTICAST_LOOP): %m");
		close(s);
		return -1;
	}
	return s;
}
#endif


/* open the UDP socket used to send SSDP notifications to
 * the multicast group reserved for them */
int OpenAndConfNotifySocket(const char * addr)
{
	int s;
	unsigned char loopchar = 0;
	struct in_addr mc_if;
	struct sockaddr_in sockname;
	
	if( (s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		syslog(LOG_ERR, "socket(udp_notify): %m");
		return -1;
	}

	mc_if.s_addr = inet_addr(addr);

	if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopchar, sizeof(loopchar)) < 0)
	{
		syslog(LOG_ERR, "setsockopt(udp_notify, IP_MULTICAST_LOOP): %m");
		close(s);
		return -1;
	}

	if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, (char *)&mc_if, sizeof(mc_if)) < 0)
	{
		syslog(LOG_ERR, "setsockopt(udp_notify, IP_MULTICAST_IF): %m");
		close(s);
		return -1;
	}

	memset(&sockname, 0, sizeof(struct sockaddr_in));
    	sockname.sin_family = AF_INET;
    	sockname.sin_addr.s_addr = inet_addr(addr);

    	if (bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in)) < 0)
	{
		syslog(LOG_ERR, "bind(udp_notify): %m");
		close(s);
		return -1;
    	}

	return s;
}
#endif

void ProcessSSDPRequest(int s, const char * host, unsigned short port)
{
	int n;
	char bufr[2048];
	int i, l;
	char * st = 0;
	int st_len = 0;
	socklen_t len_r;
	//char sender_str[64];
	const char * announced_host = NULL;
#ifdef ENABLE_IPV6
	struct sockaddr_storage sendername;
	len_r = sizeof(struct sockaddr_storage);
#else
	struct sockaddr_in sendername;
	len_r = sizeof(struct sockaddr_in);
#endif
	n = recvfrom(s, bufr, sizeof(bufr), 0,
	             (struct sockaddr *)&sendername, &len_r);
	if(n<0)
	{
		syslog(LOG_ERR, "recvfrom: %m");
		return;
	}
	if(memcmp(bufr, "NOTIFY", 6) == 0)
	{
		/* ignore NOTIFY packets. We could log the sender and device type */
		return;
	}
	else if(memcmp(bufr, "M-SEARCH", 8) == 0)
	{
		i = 0;
		while(i<n)
		{
			while((i<n-1) && (bufr[i] != '\r' || bufr[i+1] != '\n'))
				i++;
			i += 2;
			if((i < n - 3) && (strncasecmp(bufr+i, "st:", 3) == 0))
			{
				st = bufr+i+3;
				st_len = 0;
				while((*st == ' ' || *st == '\t') && (st < bufr + n)) st++;
				while(st[st_len]!='\r' && st[st_len]!='\n' && (st + st_len < bufr + n)) st_len++;
				/*syslog(LOG_INFO, "ST: %.*s", st_len, st);*/
				/*j = 0;*/
				/*while(bufr[i+j]!='\r') j++;*/
				/*syslog(LOG_INFO, "%.*s", j, bufr+i);*/
			}
		}
		//syslog(LOG_INFO, "SSDP M-SEARCH packet received from %s:%d",
	           //inet_ntoa(sendername.sin_addr),
	          // ntohs(sendername.sin_port) );
		if(st && st_len >0)
		{
			/* TODO : doesnt answer at once but wait for a random time */
			syslog(LOG_INFO, "ST: %.*s", st_len, st);
			
#ifdef ENABLE_IPV6

			if(sendername.ss_family == AF_INET){
				announced_host = listen_addr;
			}

			else{
				announced_host = ipv6_addr_for_http_with_brackets;
			}
#else
			
				announced_host = listen_addr;
			
#endif
			i = 0;
			while(known_service_types[i])
			{
				l = (int)strlen(known_service_types[i]);
				if(l<=st_len && (0 == memcmp(st, known_service_types[i], l)))
				{
					SendSSDPAnnounce2(s,(struct sockaddr*)&sendername, st, st_len, announced_host, port);
					break;
				}
				i++;
			}
			l = (int)strlen(uuidvalue);
			if(l==st_len && (0 == memcmp(st, uuidvalue, l)))
			{
				SendSSDPAnnounce2(s, (struct sockaddr*)&sendername, st, st_len, announced_host, port);
			}
		}
	}
	else
	{
		syslog(LOG_NOTICE, "Unknown udp packet received from \n");//%s:%d"
		       //inet_ntoa(sendername.sin_addr),
			   //ntohs(sendername.sin_port) );
	}
}

/* This will broadcast ssdp:byebye notifications to inform 
 * the network that UPnP is going down. */
void miniupnpdShutdown()
{
	struct sockaddr_in sockname;
    int n,i=0;
    char bufr[512];
	int s;
	struct in_addr local_if;

	s = socket(PF_INET, SOCK_DGRAM, 0);

	local_if.s_addr = inet_addr(listen_addr);
	if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, (char *)&local_if, sizeof(local_if)) < 0)
	{
		syslog(LOG_ERR, "setsockopt - IP_MULTICAST_IF: %m");
	}

    memset(&sockname, 0, sizeof(struct sockaddr_in));
    sockname.sin_family = AF_INET;
    sockname.sin_addr.s_addr = inet_addr(listen_addr);/*INADDR_ANY;*/
	if(bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in)) < 0)
		syslog(LOG_ERR, "bind: %m");

    memset(&sockname, 0, sizeof(struct sockaddr_in));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(PORT);
    sockname.sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);

    while(known_service_types[i])
    {
        snprintf(bufr, sizeof(bufr),
                 "NOTIFY * HTTP/1.1\r\n"
                 "HOST:%s:%d\r\n"
                 "NT:%s%s\r\n"
                 "USN:%s::%s%s\r\n"
                 "NTS:ssdp:byebye\r\n"
                 "\r\n",
                 UPNP_MCAST_ADDR, PORT,
				 known_service_types[i], (i==0?"":"1"),
                 uuidvalue, known_service_types[i], (i==0?"":"1"));
        n = sendto(s, bufr, strlen(bufr), 0,
                   (struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
		if(n<0)
		{
			syslog(LOG_ERR, "shutdown: sendto: %m");
		}
        i++;
    }
	close(s);
	if(unlink(pidfilename) < 0)
	{
		syslog(LOG_ERR, "failed to remove %s : %m", pidfilename);
	}
	closelog();
	//exit(0);
}

/* Write the pid to a file */
static void
writepidfile(const char * fname, int pid)
{
	char *pidstring;
	int pidstringlen;
	int pidfile;

	if(!fname || (strlen(fname) == 0))
		return;
	
	pidfile = open(fname, O_WRONLY|O_CREAT, 0666);
	if(pidfile < 0)
	{
		syslog(LOG_ERR, "Unable to write to pidfile %s: %m", fname);
	}
	else
	{
		pidstringlen = asprintf(&pidstring, "%d\n", pid);
		if(pidstringlen < 0)
		{
			syslog(LOG_ERR,
			       "asprintf failed, Unable to write to pidfile %s",
				   fname);
		}
		else
		{
			write(pidfile, pidstring, pidstringlen);
			free(pidstring);
		}
		close(pidfile);
	}
}
static int substr(char *docinpath, char *infile, char *docoutpath, char *outfile, char *str_from, char *str_to)
{
	FILE *fpi, *fpo;
	char pathi[256], patho[256];
	char buffi[4096], buffo[4096];
	int len_buff, len_from, len_to;
	int i, j;

	sprintf(pathi, "%s%s", docinpath, infile);

	if ((fpi = fopen(pathi,"r")) == NULL) {
		printf("input file can not open\n");
		return (-1);
	}

	sprintf(patho, "%s%s", docoutpath, outfile);
	if ((fpo = fopen(patho,"w")) == NULL) {
		printf("output file can not open\n");
		fclose(fpi);
		return (-1);
	}

	len_from = strlen(str_from);
	len_to   = strlen(str_to);

	while (fgets(buffi, 4096, fpi) != NULL) {
		len_buff = strlen(buffi);
		for (i=0, j=0; i <= len_buff-len_from; i++, j++) {
			if (strncmp(buffi+i, str_from, len_from)==0) {
				strcpy (buffo+j, str_to);
				i += len_from - 1;
				j += len_to - 1;
			} else
				*(buffo + j) = *(buffi + i);
		}
		strcpy(buffo + j, buffi + i);
		fputs(buffo, fpo);
	}

	fclose(fpo);
	fclose(fpi);
	return (0);
}
/* === main === */
/* call procession of HTTP or SSDP requests */

#ifdef SUPPORT_HNAP
extern void HNAP_GetPortMappingsResponse(void);
extern void HNAP_AddPortMapping(const char *in, const int len);
extern void HNAP_DeletePortMapping(const char *in, const int len);
extern char *mini_UPnP_UploadXML(char *file_path);
#if 0
static int OpenAndConfUNIXSocket(void)
{
	int s, len;
	struct sockaddr_un local;

	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
            	perror("UNIXSocket");
            	return -1;
    	}
	
    	local.sun_family = AF_UNIX;
     	strcpy(local.sun_path, UNIX_SOCK_PATH);
     	unlink(local.sun_path);
     	//len = strlen(local.sun_path) + sizeof(local.sun_family);
     	len = sizeof(struct sockaddr_un);
    	if (bind(s, (struct sockaddr *)&local, len) == -1) {
            	perror("UNIXSocket bind");
            	return -1;
     	}

     	if (listen(s, 5) == -1) {
            	perror("UNIXSocket listen");
            	return -1;
     	}

	return s;
}
#endif
static void sigHandler_alarm(int signo)
{
	struct stat status;
	char *in;
	
	if (lstat("/tmp/hnap_igd_send", &status) == 0) {
		if ((in = mini_UPnP_UploadXML("/tmp/hnap_igd_send")) != NULL) {
			int len;
			char *p;
			char *p_end;
			char num[20];

			unlink("/tmp/hnap_igd_send");
			syslog(LOG_INFO, "remove /tmp/hnap_igd_send");
				
			if (memcmp("AddPortMapping", in, 14) == 0) {
				p = in + 15;
				p_end = p;
				while (*p_end != '\n')
					p_end++;
				memcpy(num, p, p_end-p);
				num[p_end-p] = 0;
				len = atoi(num);
				p_end++;
				HNAP_AddPortMapping(p_end, len);
			}
			else if (memcmp("DeletePortMapping", in, 17) == 0) {
				p = in + 18;
				p_end = p;
				while (*p_end != '\n')
					p_end++;
				memcpy(num, p, p_end-p);
				num[p_end-p] = 0;
				len = atoi(num);
				p_end++;
				HNAP_DeletePortMapping(p_end, len);
			}
			else if (memcmp("GetPortMappings", in, 15) == 0) {
				HNAP_GetPortMappingsResponse();
			}

			free(in);
		}
		else {
			unlink("/tmp/hnap_igd_send");
			syslog(LOG_INFO, "remove /tmp/hnap_igd_send");
		}
	}

	alarm(1);
}
#endif

static int miniupnp_WriteConfigFile(const int port)
{
	FILE *fp;
	char *buffo;

	if ((fp = fopen(IGD_FILE,"w")) == NULL) {
		printf("%s %d : open %s failed\n", __FUNCTION__, __LINE__, IGD_FILE);
		return -1;
	}
	buffo = (char *) malloc(512);
	if (buffo == NULL) {
		fclose(fp);
		return -1;
	}
	memset(buffo, 0, 512);
	sprintf(buffo, "port %d\n", port);
	fputs(buffo, fp);

	sprintf(buffo, "max_age %d\n", DEFAULT_ADVR_EXPIRE);
	fputs(buffo, fp);
	sprintf(buffo, "uuid %s\n", uuidvalue);
	fputs(buffo, fp);

	if(isCfgFromFile == 1){
		if(location_url[0]){
			sprintf(buffo, "location_url %s\n", location_url);
			fputs(buffo, fp);
		}
		if(server_id[0]){
			sprintf(buffo, "server_id %s\n", server_id);
			fputs(buffo, fp);
		}
		if(ssdp_ext[0]){
			sprintf(buffo, "ssdp_ext %s\n", ssdp_ext);
			fputs(buffo, fp);
		}
		if(ssdp_mx > 0){
			sprintf(buffo, "ssdp_mx %d\n", ssdp_mx);
			fputs(buffo, fp);
		}
	}
	fputs("root_desc_name picsdesc\n", fp);
	fputs("known_service_types upnp:rootdevice\n", fp);
	fputs("known_service_types urn:schemas-upnp-org:device:InternetGatewayDevice:\n",fp);
	fputs("known_service_types urn:schemas-upnp-org:device:WANConnectionDevice:\n",fp);
	fputs("known_service_types urn:schemas-upnp-org:device:WANDevice:\n",fp);
	fputs("known_service_types urn:schemas-upnp-org:service:WANCommonInterfaceConfig:\n",fp);
	fputs("known_service_types urn:schemas-upnp-org:service:WANIPConnection:\n",fp);
	//fputs("known_service_types urn:schemas-upnp-org:service:WANPPPConnection:\n",fp);
	//fputs("known_service_types urn:schemas-upnp-org:service:Layer3Forwarding:\n",fp);
	fclose(fp);
	free(buffo);
	return 0;
}

static void RemoveIPtables(void)
{
	syslog(LOG_NOTICE, "Remove IP tables");
	system("iptables -t nat -X MINIUPNPD");
	system("iptables -t filter -X MINIUPNPD");
}

/* Handler for the SIGTERM signal (kill) */
void sigterm(int sig)
{
	if (sig != SIGTERM)
		return;
	
	/*int save_errno = errno;*/
	signal(sig, SIG_IGN);

//	syslog(LOG_NOTICE, "received signal %d, exiting", sig);

	quitting = 1;
	/*errno = save_errno;*/

	//by brian, backup rules to file when terminated
	backup_rules(BACKUP_RULES_FILENAME);

	RemoveIPtables();
}

static void AddToChain(const char *if_name, const char *ip)
{
	char tmp[200];
#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
	char tmpBuf[200];
#endif

	syslog(LOG_INFO, "miniigd : initial iptables on MINIUPNPD");
#ifdef MULTI_PPPOE	
	strcpy(deviceName[rule_set_number],if_name);	//save the set iface
	rule_set_number = rule_set_number % sessionNumber + 1;
	
	if(1 ==rule_set_number) //when first time enter, set the rule
	{
#endif	
	system("iptables -t nat -F MINIUPNPD");
	system("iptables -t filter -F MINIUPNPD");
#ifdef MULTI_PPPOE		
	}
#endif	
	sprintf(tmp, "iptables -t nat -A PREROUTING -d %s -i %s -j MINIUPNPD", ip, if_name);
	system(tmp);	
#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
	sprintf(tmpBuf,"echo -n \"%s,\" > %s",tmp,UPNP_INIT);
	system(tmpBuf);
#endif
	syslog(LOG_INFO, "miniigd : execute [%s]", tmp);
	sprintf(tmp, "iptables -t filter -A FORWARD -i %s -o ! %s -j MINIUPNPD", if_name, if_name);
	system(tmp);
#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
	sprintf(tmpBuf,"echo -n \"%s,\" >> %s",tmp,UPNP_INIT);
	system(tmpBuf);
#endif
	syslog(LOG_INFO, "miniigd : execute [%s]", tmp);
}

static void AddIPtables(const int new_created, const char *if_name, const char *ip)
{
	struct stat status;

	if (new_created) {
		system("iptables -t nat -N MINIUPNPD");
		system("iptables -t filter -N MINIUPNPD");
		//syslog(LOG_INFO, "miniigd : create nat filter iptables");
	}
	if (strcmp(if_name, "lo")) {
		if (stat(IGD_FIREWALL, &status) == 0) {
			syslog(LOG_INFO, "miniigd : add iptables now!");
			AddToChain(if_name, ip);
			//by brian, trigger recovery when rules cleaned
			recover_rules();
#ifdef MULTI_PPPOE			
			if(rule_set_number == sessionNumber)
#endif				
			remove(IGD_FIREWALL);
			need_add_iptables = 0;
		}
		else {
			syslog(LOG_INFO, "miniigd : add iptables later!");
			need_add_iptables = 1;
		}
	}
}

static void CheckInterfaceChanged(const char *if_name)
{
	char ext_ip_addr_tmp[INET_ADDRSTRLEN];

	strcpy(ext_ip_addr_tmp, "127.0.0.1");
	if(getifaddr(if_name, ext_ip_addr_tmp, INET_ADDRSTRLEN) < 0) {
		//syslog(LOG_INFO, "miniigd : Failed to get IP of ext_if_name [%s]; changed ext_if_name to [lo]", if_name);
		if(getifaddr("lo", ext_ip_addr_tmp, INET_ADDRSTRLEN) < 0) {
			//syslog(LOG_INFO, "miniigd : Failed to get IP of ext_if_name [lo].");
		}
		if (strcmp(ext_if_name, "lo") || strcmp(ext_ip_addr, ext_ip_addr_tmp)) {
			strcpy(ext_if_name, "lo");
			strcpy(ext_ip_addr, ext_ip_addr_tmp);
			//syslog(LOG_INFO, "miniigd : Changed ext_if_name [%s]; changed ext_if_ip to [%s]", ext_if_name, ext_ip_addr);
		}
		else {
			//syslog(LOG_INFO, "miniigd : Unchanged ext_if_name [%s]; Unchanged ext_if_ip to [%s]", ext_if_name, ext_ip_addr);
		}
	}
	else {
		if (strcmp(ext_if_name, if_name) || strcmp(ext_ip_addr, ext_ip_addr_tmp)) {
			strcpy(ext_if_name, if_name);
			strcpy(ext_ip_addr, ext_ip_addr_tmp);
			syslog(LOG_INFO, "miniigd : Changed ext_if_name [%s]; changed ext_if_ip to [%s]", ext_if_name, ext_ip_addr);
			AddIPtables(0, ext_if_name, ext_ip_addr);
		}
		//else {
			//syslog(LOG_INFO, "miniigd : Unchanged ext_if_name [%s]; Unchanged ext_if_ip to [%s]", ext_if_name, ext_ip_addr);
		//}
	}
}

#ifdef MULTI_PPPOE
/*
static int sessionNumber = 1; //default one ppp connect
static int rule_set_number = 0; //
int cur_session = -1;	//used for loop pppx,eg: ppp0->ppp1->ppp2->ppp3->ppp0
char deviceName[4][32];

*/

int IsAlreadySet(const char * iface)
{
	int index ;

	for(index = 0 ; index < rule_set_number ; ++index)
	{
		if(!strcmp(deviceName[index],iface))
			return 1;
	}
	return 0;
}

#endif
static void CheckWanStatus(void)
{	
	int result=0;
	struct sysinfo system_info;
	if (need_add_iptables) {
		struct stat status;
#ifdef MULTI_PPPOE		
		if(strcmp(ext_if_name,"lo")){
#endif			
		if (stat(IGD_FIREWALL, &status) == 0) {
			AddToChain(ext_if_name, ext_ip_addr);
			//by brian, trigger recovery when rules cleaned
			recover_rules();
#ifdef MULTI_PPPOE			
				if(rule_set_number == sessionNumber)
#endif	
				remove(IGD_FIREWALL);
				need_add_iptables = 0;
			}
			else {
				//syslog(LOG_INFO, "miniigd : add iptables later!");
				need_add_iptables = 1;
			}
#ifdef MULTI_PPPOE
		}
#endif		
	}
	
	//int WANconnected = getWanLink("wlan0");
	
	//if (WANconnected > 0) {
		//syslog(LOG_INFO, "miniigd : WAN [%d] connected", wan_type);
		//if (wisp_interface_num < 0) 
		{
			switch (wan_type)
			{
				case PPPOE:
#ifdef MULTI_PPPOE						

					if(sessionSel >= 1)	//two pppoe connect
					{	
						cur_session = (cur_session+1)%sessionNumber;
						// 0 --->ppp0 : 1 --->ppp1 : 2 --->ppp2 : 3 --->ppp3
						if(0 == cur_session)
						{							
							if(IsAlreadySet(WAN_IF_2))
								return;								
							CheckInterfaceChanged(WAN_IF_2);						
						}
						else if(1 == cur_session)
						{		
							if(IsAlreadySet(WAN_IF_2_1))
								return;								
							CheckInterfaceChanged(WAN_IF_2_1);	
						}
						else if(2 == cur_session)
						{
							if(IsAlreadySet(WAN_IF_2_2))
								return;								
							CheckInterfaceChanged(WAN_IF_2_2);	
						}
						else if(3 == cur_session)
						{
							if(IsAlreadySet(WAN_IF_2_3))
								return;								
							CheckInterfaceChanged(WAN_IF_2_3);	
						}						
					}
					else 
#endif					
					if(sessionSel==1)
						CheckInterfaceChanged(WAN_IF_2_1);
					else
						CheckInterfaceChanged(WAN_IF_2);
					break;
				case L2TP:
				case PPTP:
				case PPPOE_RU:
				case PPTP_RU:
				case L2TP_RU:	
					CheckInterfaceChanged(WAN_IF_2);
					break;
				case DHCP_CLIENT:
				case DHCP_DISABLED:
				case BIGPOND:
				default:
					if (wisp_interface_num < 0) 
						CheckInterfaceChanged(WAN_IF_1);
					else
						CheckInterfaceChanged(wisp_if_name);
					break;
			}
		}
		//else {
			//CheckInterfaceChanged(wisp_if_name);
		//}
	//}
	//else {
		//syslog(LOG_INFO, "miniigd : WAN [%d] disconnected", wan_type);
		//CheckInterfaceChanged("lo");
	//}
	result = is_wan_connected();
	if(wan_states != result){
		wan_states = result;
		sysinfo(&system_info);
		if(result ==4){ //link up
			wan_uptime = system_info.uptime;
		}else
			wan_uptime = 0;
#ifdef ENABLE_EVENTS		
	upnp_event_var_change_notify(EWanCFG);
	upnp_event_var_change_notify(EWanIPC);
#endif	
	}
}

#if defined(CONFIG_RTL865X_AC) || defined(CONFIG_RTL865X_KLD) ||  defined(CONFIG_RTL8186_TR) /* In order to accept empty character in HostName. Keith */	
#define MAXWORDLEN 256
int getword(FILE *f, char *word)
{
    int c, len, escape;
    int quoted, comment;
    int value, got;

    len = 0;
    escape = 0;
    comment = 0;

    /*
     * First skip white-space and comments.
     */
     
    for (;;) {
	c = getc(f);
	if (c == EOF)
	    break;

	/*
	 * A newline means the end of a comment; backslash-newline
	 * is ignored.  Note that we cannot have escape && comment.
	 */
	if (c == '\n') {
	    if (!escape) {
		comment = 0;
	    } else
		escape = 0;
	    continue;
	}

	/*
	 * Ignore characters other than newline in a comment.
	 */
	if (comment)
	    continue;

	/*
	 * If this character is escaped, we have a word start.
	 */
	if (escape)
	    break;

	/*
	 * If this is the escape character, look at the next character.
	 */
	if (c == '\\') {
	    escape = 1;
	    continue;
	}

	/*
	 * If this is the start of a comment, ignore the rest of the line.
	 */
	//if (c == '#') {
	//    comment = 1;
	//    continue;
	//}
	/*
	 * A non-whitespace character is the start of a word.
	 */
	if (!isspace(c))
	    break;
	
    }

    /*
     * Save the delimiter for quoted strings.
     */
    if (!escape && (c == '"' || c == '\'')) {
        quoted = c;
	c = getc(f);
    } else
        quoted = 0;

    /*
     * Process characters until the end of the word.
     */
    while (c != EOF) {
	if (escape) {
	    /*
	     * This character is escaped: backslash-newline is ignored,
	     * various other characters indicate particular values
	     * as for C backslash-escapes.
	     */
	    escape = 0;
	    if (c == '\n') {
	        c = getc(f);
		continue;
}

	    got = 0;
	    switch (c) {
	    case 'a':
		value = '\a';
		break;
	    case 'b':
		value = '\b';
		break;
	    case 'f':
		value = '\f';
		break;
	    case 'n':
		value = '\n';
		break;
	    case 'r':
		value = '\r';
		break;
	    case 's':
		value = ' ';
		break;
	    case 't':
		value = '\t';
		break;

	    default:
		/*
		 * Otherwise the character stands for itself.
		 */
		value = c;
		break;
	    }

	    /*
	     * Store the resulting character for the escape sequence.
	     */
	    if (len < MAXWORDLEN-1)
		word[len] = value;
	    ++len;

	    if (!got)
		c = getc(f);
	    continue;

	}

	/*
	 * Not escaped: see if we've reached the end of the word.
	 */
	if (quoted) {
	    if (c == quoted)
		break;
	} else {
	    //if (isspace(c) || c == '#') {
	    if (isspace(c)) {	
		ungetc (c, f);
		break;
	    }
	}

	/*
	 * Backslash starts an escape sequence.
	 */
	if (c == '\\') {
	    escape = 1;
	    c = getc(f);
	    continue;
	}

	/*
	 * An ordinary character: store it in the word and get another.
	 */
	if (len < MAXWORDLEN-1)
	    word[len] = c;
	++len;

	c = getc(f);
    }

    /*
     * End of the word: check for errors.
     */
    if (c == EOF) {
	if (ferror(f)) {
	    if (errno == 0)
		errno = EIO;
	   
	}
	/*
	 * If len is zero, then we didn't find a word before the
	 * end of the file.
	 */
	if (len == 0)
	    return 0;
    }

    
    word[len] = 0;


    return 1;



}

#endif
#ifdef ENABLE_EVENTS	
extern void upnpevents_removeSubscriber_shutdown(void);
#endif


int main(int argc, char * * argv)
{
	int i;
	int pid;
	int shttpl;
#ifdef STAND_ALONE
        int sudp=-1, snotify=-1;
#ifdef ENABLE_IPV6
	int sudp6 = -1,snotify6 = -1;
#endif
#endif
#if defined(ENABLE_NATPMP)
	int snatpmp = -1;
#endif
	LIST_HEAD(httplisthead, upnphttp) upnphttphead;
	struct upnphttp * e = 0;
	struct upnphttp * next;
	fd_set readset;	/* for select() */
	int notify_interval = 0;
	//int notify_interval = CHECK_WAN_INTERVAL;	/* seconds to check WAN status */
	struct timeval timeout={0,0}, timeofday={0,0}, lasttimeofday = {0, 0};
	int debug_flag = 0;
	int openlog_option;
	struct sigaction sa;
	int port = -1;
	int max_fd = -1;
	int enablenatpmp = 0;
#if defined(CONFIG_RTL865X_AC) || defined(CONFIG_RTL865X_KLD) ||  defined(CONFIG_RTL8186_TR) /* Add host name in Upnp. Keith */	
	char host_name[100];
	FILE	*fp;
#endif
#ifdef ENABLE_EVENTS
		fd_set writeset;
#endif
	struct sysinfo system_info;
#if defined(CONFIG_RTL8186_KB) || defined(CONFIG_RTL8186_KB_N)
	FILE	*fp1;
	char tmpBuf[50];
	int tmpLen;
#endif
	char line[200];	
#ifdef ENABLE_6FC_SERVICE
		unsigned int next_pinhole_ts;
#endif


	memset(location_url, 0x00, MAX_SERVER_LEN);
	memset(server_id, 0x00, MAX_SERVER_LEN);
	memset(ssdp_ext, 0x00, MAX_SERVER_LEN);
	ssdp_mx=0;
	isCfgFromFile=0;

	memset(internal_if, 0, MAX_LEN_IF_NAME);
	/* command line arguments processing */
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]!='-')
		{
			fprintf(stderr, "%s: Unknown option\n", argv[i]);
		}
		else switch(argv[i][1])
		{
		case 'e':
			wan_type = atoi(argv[++i]);
			break;
		case 'i':
			strcpy(internal_if, argv[++i]);
			break;
		case 'u':
			strncpy(uuidvalue+5, argv[++i], strlen(uuidvalue+5) + 1);
			break;
		case 'U':
			sysuptime = 1;
			break;
		case 'L':
			logpackets = 1;
			break;
		case 'P':
			pidfilename = argv[++i];
			break;
		case 'd':
			debug_flag = 1;
			break;
		case 'w':
			//wisp_interface_num = atoi(argv[++i]);
			//sprintf(wisp_if_name, "%s%d", WAN_IF_3, wisp_interface_num);
			sprintf(wisp_if_name, "%s", argv[++i]);
			wisp_interface_num = wisp_if_name[4] - '0';
			break;
		case 'B':
			downstream_bitrate = strtoul(argv[++i], 0, 0);
			upstream_bitrate = strtoul(argv[++i], 0, 0);
			break;
		case 's':
			if(i<(argc-1))
			{
				sessionSel=atoi(argv[++i]);
#ifdef 	MULTI_PPPOE
				sessionNumber = sessionSel;
#endif
			}
			break;
#ifdef CONFIG_RTL8186_GR
		case 'c':
			if(i<(argc-1))
			{
				pppCutEnabled=atoi(argv[++i]);
			}
			break;
#endif
#if defined(ENABLE_NATPMP)
		case 'N':
			enablenatpmp = 1;
			break;
#endif

		default:
			fprintf(stderr, "%s: Unknown option\n", argv[i]);
		}
	}

	if (wan_type < 0) {
		printf("Please specify the WAN type\n");
		return 1;
	}
	
#if 0
	if(!ext_if_name || !listen_addr || port<=0)
	{
		fprintf(stderr, "Usage:\n\t"
		        "%s -i ext_ifname [-o ext_ip] -a listening_ip -p port "
				/*"[-l logfile] " not functionnal */
				"[-u uuid] [-t notify interval] "
#if !defined(ENABLE_NATPMP)
				"[-P pidfilename] [-d] [-L] [-U] [-B down up] "
#else
				"[-P pidfilename] [-d] [-L] [-U] [-B down up] [-N] "
#endif

				"[-w url]\n"
		        "\nNotes:\n\tThere can be one or several listening_ips\n"
		        "\tNotify interval is in seconds. Default is 30sec.\n"
				"\tDefault pid file is %s\n"
				"\tWith -d option miniupnpd will run as a standard program\n"
				"\t-L option set packet log in pf on.\n"
				"\t-U option makes miniupnpd report system uptime instead "
				"of daemon uptime.\n"
				"\t-B option sets bitrates reported by daemon in Bytes "
#if defined(ENABLE_NATPMP)
				"\t-N option enable NAT-PMP method "
#endif
				"per second.\n"
				"\t-w sets the presentation url. Default is http address on port 80\n",
		        argv[0], pidfilename);
		return 1;
	}
#endif

	/* record the startup time, for returning uptime */
	startup_time = time(NULL);
	if(sysuptime)
	{
		/* use system uptime instead of daemon uptime */
#if defined(__linux__)
		char buff[64];
		int uptime, fd;
		fd = open("/proc/uptime", O_RDONLY);
		if(fd<0)
		{
			syslog(LOG_ERR, "open(\"/proc/uptime\" : %m");
		}
		else
		{
			memset(buff, 0, sizeof(buff));
			read(fd, buff, sizeof(buff) - 1);
			uptime = atoi(buff);
			syslog(LOG_INFO, "system uptime is %d seconds", uptime);
			close(fd);
			startup_time -= uptime;
		}
#else
		struct timeval boottime;
		size_t size = sizeof(boottime);
		int name[2] = { CTL_KERN, KERN_BOOTTIME };
		if(sysctl(name, 2, &boottime, &size, NULL, 0) < 0)
		{
			syslog(LOG_ERR, "sysctl(\"kern.boottime\") failed");
		}
		else
		{
			startup_time = boottime.tv_sec;
		}
#endif
	}

	//srand((unsigned)time(NULL));
	//port = 50000 + (rand() % 10000);
	port = PICS_INIT_PORT;
	
	if (!internal_if[0])
		strcpy(internal_if, "br0");
	if(getifaddr(internal_if, listen_addr, INET_ADDRSTRLEN) < 0) {
			printf("miniigd : Failed to get internal_if IP. EXITING!\n");
			return 1;
	}
#ifdef ENABLE_IPV6
	if(getifaddrv6(internal_if, listen_addr6, INET6_ADDRSTRLEN) < 0) {
			printf("miniigd : Failed to get internal_if v6 IP. EXITING!\n");
			return 1;
	}
#endif

//To generate different uuid for different device according to /tmp/eth1_mac_addr
#if defined(CONFIG_RTL8186_KB) || defined(CONFIG_RTL8186_KB_N)
	if ((fp1 = fopen("/tmp/eth1_mac_addr", "r")) != NULL) 
	{
		if(fgets(tmpBuf, 50, fp1) != NULL)
		{
			tmpLen=strlen(tmpBuf);
			tmpBuf[tmpLen-1]='\0';//To remove \n
			sprintf(uuidvalue,"uuid:82350000-4000-0000-0000-%s",tmpBuf);
		}
		fclose(fp1);
    	}
	system("rm -f /tmp/eth1_mac_addr");
#endif

	substr("/etc/linuxigd/","picsdesc.skl","/etc/linuxigd/","picsdesc.xml","!ADDR!", listen_addr);

	if(read_config_file(DEFAULT_CONFIG_FILENAME, port) == 0)
		isCfgFromFile = 1;	
		
	if(isCfgFromFile){	
		system("cp /etc/linuxigd/picsdesc.xml /tmp/picsdesc.skl");
		substr("/tmp/","picsdesc.skl","/etc/linuxigd/", "picsdesc.xml","!UUID!", uuidvalue);
		system("rm /tmp/picsdesc.skl -rf");
	}

#if defined(CONFIG_RTL865X_AC) || defined(CONFIG_RTL865X_KLD) ||  defined(CONFIG_RTL8186_TR) /* Add host name in Upnp. Keith */
	memset(host_name, '\0', 100);
		
    	if ((fp = fopen("/var/hostname", "r")) != NULL) 
	{
		fseek(fp,10L, SEEK_SET);
		getword(fp, host_name);
		fclose(fp);
    	}


	if(host_name[0])
	{
		system("cp /etc/linuxigd/picsdesc.xml /tmp/picsdesc.skl");

		substr("/tmp/","picsdesc.skl","/etc/linuxigd/", "picsdesc.xml","!HOST_NAME!", host_name);
		system("rm /tmp/picsdesc.skl -rf");
	}
#endif

	DISPLAY_BANNER;

	if(debug_flag)
		pid = getpid();
	else
		pid = daemonize();

	//Patch: waiting for br0 entering forwarding state, then send out ssdp notify pkts
	sleep(3);
	sleep(3);
	sleep(3);
#ifdef ENABLE_6FC_SERVICE
		init_iptpinhole();
#endif

	/* TODO : change LOG_LOCAL0 to LOG_DAEMON */
	if(debug_flag) {
		openlog_option = LOG_PID|LOG_CONS;
		openlog_option |= LOG_PERROR;	/* also log on stderr */
		openlog("miniigd", openlog_option, LOG_USER/*LOG_LOCAL0*/);
	}

	writepidfile(pidfilename, pid);
	
#if defined(ENABLE_NATPMP)
	if(enablenatpmp)
	{
		snatpmp = OpenAndConfNATPMPSocket();
		if(snatpmp < 0)
		{
			syslog(LOG_ERR, "Failed to open socket for NAT PMP.");
			/*syslog(LOG_ERR, "Failed to open socket for NAT PMP. EXITING");
			return 1;*/
		} else {
			syslog(LOG_NOTICE, "Listening for NAT-PMP traffic on port %u",
				   NATPMP_PORT);
		}
		ScanNATPMPforExpiration();
	}
#endif

	
	/* socket d'ecoute des connections HTTP */
	shttpl = OpenAndConfHTTPSocket(port);
	if(shttpl < 0)
	{
		printf("miniigd : Failed to open socket for HTTP. EXITING!\n");
		return 1;
	}
#ifdef ENABLE_IPV6
		if(find_ipv6_addr(internal_if, ipv6_addr_for_http_with_brackets, sizeof(ipv6_addr_for_http_with_brackets)) > 0) {
			syslog(LOG_NOTICE, "HTTP IPv6 address given to control points : %s",
			       ipv6_addr_for_http_with_brackets);
		} else {
			memcpy(ipv6_addr_for_http_with_brackets, "[::1]", 6);
			syslog(LOG_WARNING, "no HTTP IPv6 address");
		}
	substr("/etc/linuxigd/","picsdesc6.skl","/etc/linuxigd/","picsdesc6.xml","!ADDR!", ipv6_addr_for_http_with_brackets);
#endif
#ifdef STAND_ALONE
       	/* socket d'ecoute pour le SSDP */
       	sudp = OpenAndConfUdpSocket(0);
       	if (sudp < 0)
       	{
       		printf("Failed to open socket for SSDP. EXITING\n");
       		return -1;
       	}
#ifdef ENABLE_IPV6
	sudp6 = OpenAndConfUdpSocket(1);
	if( sudp6 < 0 ){
		printf("Failed to open socket for SSDP IPV6. EXITING\n");
		return -1;
	}
#endif
       	/* open socket for sending notifications */
       	snotify = OpenAndConfNotifySocket(listen_addr);
       	if (snotify < 0)
       	{
       		printf("Failed to open socket for SSDP notify messages\n");
       		return -1;
       	}
#ifdef ENABLE_IPV6
	snotify6 = OpenAndConfSSDPNotifySocketIPv6(if_nametoindex(internal_if));
	if(snotify6 < 0){
		printf("Failed to open socket for SSDP IPV6 notify messages\n");
       	return -1;
	}
#endif
#endif

#ifdef SUPPORT_HNAP
#if 0
	int  unix_http;
	unix_http = OpenAndConfUNIXSocket();
	if (unix_http < 0) {
		printf("miniigd : Failed to open socket for Unix Socket. EXITING!\n");
		return 1;
	}
#endif
	signal(SIGALRM, sigHandler_alarm);
	alarm(1);
#endif
	
	/* set signal handler */
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = sigterm;
	if (sigaction(SIGTERM, &sa, NULL))
	{
		printf("miniigd : Failed to set SIGTERM handler\n");
		return 1;
	}
	if (sigaction(SIGINT, &sa, NULL))
	{
		printf("miniigd : Failed to set SIGTERM handler\n");
		return 1;
	}
	
	strcpy(ext_if_name, "lo");
	strcpy(ext_ip_addr, "127.0.0.1");
	AddIPtables(1, ext_if_name, ext_ip_addr);
	CheckWanStatus();
	
	if (miniupnp_WriteConfigFile(port) < 0)
		return 1;

	LIST_INIT(&upnphttphead);

	while (!quitting)
	{
		 
//		if(gettimeofday(&timeofday, 0) < 0)
//		{
//			syslog(LOG_ERR, "gettimeofday: %m");
//			timeout.tv_sec = notify_interval;
//			timeout.tv_usec = 0;
//		}
//		else
//		{
			/* use system uptime for time reference*/
			sysinfo(&system_info);
			timeofday.tv_sec = system_info.uptime;
			if(timeofday.tv_sec >= (lasttimeofday.tv_sec + CHECK_WAN_INTERVAL))
			{
				CheckWanStatus();
				
				memcpy(&lasttimeofday, &timeofday, sizeof(struct timeval));
				timeout.tv_sec = CHECK_WAN_INTERVAL;
				timeout.tv_usec = 0;
#ifdef STAND_ALONE
				if( snotify >= 0 && notify_interval==0 )
				    SendSSDPNotifies(snotify, listen_addr,0,0,0);
#ifdef ENABLE_IPV6
			if(snotify6 >=0 && notify_interval == 0)
				SendSSDPNotifies(snotify6,ipv6_addr_for_http_with_brackets,0,0,1);
#endif
#endif
				if(notify_interval>0)
					notify_interval--;
				else
					notify_interval=(ssdp_mx)?(ssdp_mx/CHECK_WAN_INTERVAL-1):(DEFAULT_ADVR_EXPIRE/CHECK_WAN_INTERVAL-1);
			}
			else
			{
				//timeout.tv_sec = lasttimeofday.tv_sec + notify_interval - timeofday.tv_sec;
				timeout.tv_sec = lasttimeofday.tv_sec + CHECK_WAN_INTERVAL - timeofday.tv_sec;
				if(timeofday.tv_usec > lasttimeofday.tv_usec)
				{
					timeout.tv_usec = 1000000 + lasttimeofday.tv_usec - timeofday.tv_usec;
					timeout.tv_sec--;
				}
				else
				{
					timeout.tv_usec = lasttimeofday.tv_usec - timeofday.tv_usec;
				}
			}
//		}
#if defined(ENABLE_NATPMP)
		/* Remove expired NAT-PMP mappings */
		if(enablenatpmp){
			while( nextnatpmptoclean_timestamp && (timeofday.tv_sec >= nextnatpmptoclean_timestamp + startup_time))
			{
				/*syslog(LOG_DEBUG, "cleaning expired NAT-PMP mappings");*/
				if(CleanExpiredNATPMP() < 0) {
					syslog(LOG_ERR, "CleanExpiredNATPMP() failed");
					break;
				}
			}
			if(nextnatpmptoclean_timestamp && timeout.tv_sec >= (nextnatpmptoclean_timestamp + startup_time - timeofday.tv_sec))
			{
				/*syslog(LOG_DEBUG, "setting timeout to %d sec", nextnatpmptoclean_timestamp + startup_time - timeofday.tv_sec);*/
				timeout.tv_sec = nextnatpmptoclean_timestamp + startup_time - timeofday.tv_sec;
				timeout.tv_usec = 0;
			}
		}
#endif
#ifdef ENABLE_6FC_SERVICE
		/* Clean up expired IPv6 PinHoles */
		next_pinhole_ts = 0;
#if 1
		upnp_clean_expired_pinholes(&next_pinhole_ts);
#endif
		if(next_pinhole_ts &&
		   timeout.tv_sec >= (int)(next_pinhole_ts - timeofday.tv_sec)) {
			timeout.tv_sec = next_pinhole_ts - timeofday.tv_sec;
			timeout.tv_usec = 0;
		}
#endif		
		
		/* select open sockets (SSDP, HTTP listen and all HTTP soap sockets) */
		FD_ZERO(&readset);
		//FD_SET(shttpl, &readset); //brad disable
		if (shttpl >= 0){
		        FD_SET(shttpl, &readset);
			max_fd = MAX(max_fd, shttpl);
		}		

		i = 0;        // active HTTP connections count
        for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
        {
	        if((e->socket >= 0) && (e->state <= 2))
	        {
	            FD_SET(e->socket, &readset);
	            max_fd = MAX( max_fd, e->socket);
	            i++;
	        }
        }
		if(i>1)
        {
        	sprintf(line,"echo \"\%d active incoming HTTP connections\" >> /tmp/miniigd_debug", i);
			system(line);
			syslog(LOG_WARNING, "%d active incoming HTTP connections", i);
		}
#if defined(ENABLE_NATPMP)
		if(snatpmp >= 0) {
			FD_SET(snatpmp, &readset);
			max_fd = MAX( max_fd, snatpmp);
		}
#endif
#ifdef STAND_ALONE
                if(sudp >= 0)
		{
                   FD_SET(sudp, &readset);
			max_fd = MAX( max_fd, sudp);
		}
#ifdef ENABLE_IPV6
		if (sudp6 >= 0)
		{
			FD_SET(sudp6, &readset);
			max_fd = MAX( max_fd, sudp6);
		}
#endif

#endif


#if 0
//#ifdef SUPPORT_HNAP
		//FD_SET(unix_http, &readset);
#endif

		/*i = 0;	// active HTTP connections count
		for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
		{
			if((e->socket >= 0) && (e->state <= 2))
			{
				FD_SET(e->socket, &readset);
				max_fd = MAX( max_fd, e->socket);
				i++;
			}
		}*/	//by brian, execute this block later (debug fd contension while UPnP handling busily)
		/* for debug */
		/*
		if(i>1)
		{
			syslog(LOG_WARNING, "%d active incoming HTTP connections", i);
		}
		*/
#ifdef ENABLE_EVENTS
		FD_ZERO(&writeset);
		upnpevents_selectfds(&readset, &writeset, &max_fd);
#endif

#ifdef ENABLE_EVENTS
		if(select(max_fd+1, &readset, &writeset, 0, &timeout) < 0)
#else
		if(select(max_fd+1, &readset, 0, 0, &timeout) < 0)
#endif	
		{
			if(quitting) goto shutdown;
			syslog(LOG_ERR, "select: %m");
			syslog(LOG_ERR, "Exiting...");
			//exit(1);	/* very serious cas of error */
		}
		
#if 0
//#ifdef SUPPORT_HNAP
		if(FD_ISSET(unix_http, &readset)) {
			int s2, t;
			struct sockaddr_un remote;
			struct upnphttp * tmp = 0;

			t = sizeof(remote);
			if ((s2 = accept(unix_http, (struct sockaddr *)&remote, &t)) == -1) {
                		syslog(LOG_ERR, "accept: %m");
            		}
			else {
				tmp = New_upnphttp(s2);
				LIST_INSERT_HEAD(&upnphttphead, tmp, entries);
			}
		}
#endif
#ifdef ENABLE_EVENTS
				upnpevents_processfds(&readset, &writeset);
#endif
		/* process active HTTP connections */
		/* LIST_FOREACH is not available under linux */
		for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
		{
			if(  (e->socket >= 0) && (e->state <= 2)
			   &&(FD_ISSET(e->socket, &readset)) ) {
				Process_upnphttp(e);
			}
		}

		/* process incoming HTTP connections */
		if(FD_ISSET(shttpl, &readset))
		{
			int shttp;
			socklen_t	clientnamelen;
#ifdef ENABLE_IPV6
			struct sockaddr_storage clientname;
			clientnamelen = sizeof(struct sockaddr_storage);
#else
			struct sockaddr_in clientname;
			clientnamelen = sizeof(struct sockaddr_in);
#endif
			struct upnphttp * tmp = 0;
			shttp = accept(shttpl, (struct sockaddr *)&clientname, &clientnamelen);
			if(shttp<0)
			{
				syslog(LOG_ERR, "accept: %m");
			}
			else
			{
				
				//syslog(LOG_INFO, "HTTP connection from %s:%d",
				       //inet_ntoa(clientname.sin_addr),
				   	   //ntohs(clientname.sin_port) );
					  
				if (fcntl(shttp, F_SETFL, O_NONBLOCK) < 0) {
					syslog(LOG_ERR, "fcntl F_SETFL, O_NONBLOCK");
				}
				/* Create a new upnphttp object and add it to
				 * the active upnphttp object list */
				tmp = New_upnphttp(shttp);
				if(tmp){
#ifdef ENABLE_IPV6
					if(clientname.ss_family == AF_INET)
					{
						tmp->clientaddr = ((struct sockaddr_in *)&clientname)->sin_addr;
						tmp->ipv6 = 0;
					}
					else if(clientname.ss_family == AF_INET6)
					{
						struct sockaddr_in6 * addr = (struct sockaddr_in6 *)&clientname;
						if(IN6_IS_ADDR_V4MAPPED(&addr->sin6_addr))
						{
							memcpy(&tmp->clientaddr,
							       &addr->sin6_addr.s6_addr[12],
							       4);
							tmp ->ipv6 = 0;
						}
						else
						{
							tmp->ipv6 = 1;
							memcpy(&tmp->clientaddr_v6,
							       &addr->sin6_addr,
							       sizeof(struct in6_addr));
						}
					}
#else
					tmp->clientaddr = clientname.sin_addr;
#endif
					LIST_INSERT_HEAD(&upnphttphead, tmp, entries);
				}
				else
					close(shttp);
			}
		}

#if defined(ENABLE_NATPMP)
		/* process NAT-PMP packets */
		if((snatpmp >= 0) && FD_ISSET(snatpmp, &readset))
		{
			ProcessIncomingNATPMPPacket(snatpmp);
		}
#endif
#ifdef STAND_ALONE
                if(sudp >= 0 && FD_ISSET(sudp, &readset))
		{
			/*syslog(LOG_INFO, "Received UDP Packet");*/
			ProcessSSDPRequest(sudp, listen_addr, port);
		}
#ifdef ENABLE_IPV6
		if(sudp6 >= 0 && FD_ISSET(sudp6, &readset))
		{
			syslog(LOG_INFO, "Received UDP Packet (IPv6)");
			ProcessSSDPRequest(sudp6, ipv6_addr_for_http_with_brackets,port);
		}
#endif

#endif
		/* delete finished HTTP connections */
		for(e = upnphttphead.lh_first; e != NULL; )
		{
			next = e->entries.le_next;
			if(e->state >= 100)
			{
				LIST_REMOVE(e, entries);
				Delete_upnphttp(e);
			}
			e = next;
		}
	}

shutdown:
	/* close out open sockets */
	while(upnphttphead.lh_first != NULL)
	{
		e = upnphttphead.lh_first;
		LIST_REMOVE(e, entries);
		Delete_upnphttp(e);
	}
#if defined(ENABLE_NATPMP)
	if(snatpmp>=0)
	{
		close(snatpmp);
		snatpmp = -1;
	}
#endif
#ifdef STAND_ALONE
        if(sudp >= 0)
			close(sudp);
#ifdef ENABLE_IPV6
		if(sudp6 >= 0)
			close(sudp6);
#endif
#endif
   	if(shttpl >=0 )
	close(shttpl);
#ifdef ENABLE_EVENTS		
	upnpevents_removeSubscriber_shutdown();
#endif
	SendSSDPNotifies(snotify, listen_addr,1,0,0);
#ifdef ENABLE_IPV6
	SendSSDPNotifies(snotify6,ipv6_addr_for_http_with_brackets,1,0,1);
#endif
	if(snotify >= 0)
		close(snotify);
#ifdef ENABLE_IPV6
	if(snotify6 >= 0)
		close(snotify6);
#endif
	closelog();

	//miniupnpdShutdown();
	return 0;
}

