/* $Id: upnpsoap.c,v 1.9 2009/10/26 12:09:23 bert Exp $ */
/* (c) 2006 Thomas Bernard */
/* project: miniupnp
 * webpage: http://miniupnp.free.fr/
 * This software is subject to the conditions detailed in the
 * LICENCE file provided with this distribution */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <syslog.h>
#include <sys/file.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/sysinfo.h> 
#include <errno.h>
#include "upnppinhole.h"

#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
#include <sys/stat.h>
#endif
#include "upnpglobalvars.h"
#include "upnphttp.h"
#include "upnpsoap.h"
#include "upnpreplyparse.h"
#include "upnpredirect.h"
#include "openbsd/getifstats.h"
#define MaxEntry 30

#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
#define UPNP_FILE "/tmp/upnp_info"
#define TMP_FILE "/tmp/upnp_tmp"
#endif

#if defined(CONFIG_RTL8186_GR)
extern int pppCutEnabled;
#endif


#define IS_DIGIT(c) ((c) >= '0' && (c) <= '9') 
#define IS_BLANK(c)((c)==' ')
#define IS_ALPHA(c) ( ((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z') ) 
#define IS_HEX_DIGIT(c) (((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f')) 
  
/* Whether string s is a number.  
   Returns 0 for non-number, 1 for integer, 2 for hex-integer, 3 for float */
int is_number(char * s) 
{ 
    int base = 10; 
    char *ptr; 
    int type = 0; 
  
    if (s==NULL) return 0; 
  
    ptr = s; 
  
    /* skip blank */
    while (IS_BLANK(*ptr)) { 
        ptr++; 
    } 
  
    /* skip sign */
    if (*ptr == '-' || *ptr == '+') { 
        ptr++; 
    } 
  
    /* first char should be digit or dot*/
    if (IS_DIGIT(*ptr) || ptr[0]=='.') { 
  
        if (ptr[0]!='.') { 
            /* handle hex numbers */
            if (ptr[0] == '0' && ptr[1] && (ptr[1] == 'x' || ptr[1] == 'X')) { 
                type = 2; 
                base = 16; 
                ptr += 2; 
            } 
  
            /* Skip any leading 0s */
            while (*ptr == '0') { 
                ptr++; 
            } 
  
            /* Skip digit */
            while (IS_DIGIT(*ptr) || (base == 16 && IS_HEX_DIGIT(*ptr))) { 
                    ptr++; 
            } 
        } 
  
        /* Handle dot */
        if (base == 10 && *ptr && ptr[0]=='.') { 
            type = 3; 
            ptr++; 
        } 
  
        /* Skip digit */
        while (type==3 && base == 10 && IS_DIGIT(*ptr)) { 
            ptr++; 
        } 
  
        /* if end with 0, it is number */
        if (*ptr==0)  
            return (type>0) ? type : 1; 
        else
            type = 0; 
    } 
    return type; 
}

/* Whether string s is a ip.  
   Returns 0 for non-ip, 1 for ip */
int is_ipv4str(const char *str)
{
	int i, a[4];
	char end;
	if( sscanf(str, "%d.%d.%d.%d%c", &a[0], &a[1], &a[2], &a[3], &end) != 4 )
		return 0;
	for(i=0; i<4; i++) if (a[i] < 0 || a[i] > 255) return 0;
	return 1;
}


int is_valid_protocol(char *protocol)
{
	int ret=0;
	if(strcmp(protocol, "UDP") == 0)
		ret=1;
	if(strcmp(protocol, "TCP") == 0)
		ret=1;
	return ret;
}
extern int wan_type;
extern int wisp_interface_num;
typedef enum {LINK_INIT=0, LINK_NO, LINK_DOWN, LINK_WAIT, LINK_UP};
struct object
{
  char int_ip[32];
  unsigned short iport;
  unsigned short eport;
  char protocol[4];
  char enabled;
  int valid;
  char desc[60];
};
struct object entry[MaxEntry];

struct in_addr IPaddr;  


extern int is_wan_connected(void);
	
int SearchEntry(unsigned short eport, char * protocol,char * int_ip,unsigned short iport, const char * desc, char enabled,int valid);

extern char *get_token(char *, char *);
extern int get_value(char *, char *);

/*void test_gen_rules(void){
	int i=0;
	
	for(i=0; i< 5; i++){
		memcpy(entry[i].int_ip,"192.168.1.200",14);
		entry[i].iport = 12345;
		entry[i].eport = 1000+i;
		strcpy(entry[i].protocol,"tcp");
		entry[i].enabled = 1;
		entry[i].valid = 1;
		strcpy(entry[i].desc,"test");
		upnp_redirect(entry[i].eport, entry[i].int_ip, entry[i].iport, entry[i].protocol, entry[i].desc, entry[i].enabled);
	}
}*/

int backup_rules(char * filename)
{
	FILE *fp;
	int i=0;
	unsigned char rule[300];

	if( !(fp = fopen(filename,"w")) )
		return -1;

	for(i=0 ;i < MaxEntry ; i++){
		if( entry[i].valid ){
			sprintf(rule,"int_ip=%s\niport=%d\neport=%d\nprotocol=%s\nenabled=%d\ndesc=%s\n#\n",entry[i].int_ip,entry[i].iport,entry[i].eport,entry[i].protocol,entry[i].enabled,entry[i].desc);
			fputs(rule,fp);
		}
	}
	fclose(fp);
	return 0;
}

int recover_rules(void){
	int index=0;
	char *ptr=NULL;
	char line[300], token[20], value[60];
	FILE *fp=NULL;
	struct stat status;
	/* FILE *tmp; */

	if( !stat(BACKUP_RULES_FILENAME, &status) ){
		if( (fp = fopen(BACKUP_RULES_FILENAME,"r")) ){
			/* tmp = fopen("/tmp/recover","w"); */
			while( fgets(line,100,fp) ){
				if( line[0] == '#' ){
					entry[index].valid = 1;
					/*
					sprintf(line,"recover rule:%s,%s,%d,%d,%d,%d,%s\n",entry[index].int_ip,entry[index].protocol,entry[index].iport,\
											entry[index].eport,entry[index].enabled,entry[index].valid,entry[index].desc);
					if( tmp )
						fputs(line,tmp);
					*/
					upnp_redirect(entry[index].eport, entry[index].int_ip, entry[index].iport, entry[index].protocol, entry[index].desc, entry[index].enabled);
					index++;
				}

				ptr=get_token(line,token);
				if( !ptr )
					continue;

				if( !get_value(ptr,value) )
					continue;

				if( !strcmp(token,"int_ip") )
					strcpy(entry[index].int_ip,value);
				else if( !strcmp(token,"protocol") )
					strcpy(entry[index].protocol,value);
				else if( !strcmp(token,"iport") )
					entry[index].iport = atoi(value);
				else if( !strcmp(token,"eport") )
					entry[index].eport = atoi(value);
				else if( !strcmp(token,"enabled") )
					entry[index].enabled = atoi(value);
				else if( !strcmp(token,"desc") )
					strcpy(entry[index].desc,value);
				else
					printf("unknow token\n");
			}
			fclose(fp);
		}
		else{
			syslog(LOG_ERR,"miniigd try to open backed up file but failed\n");
		}
		/* fclose(tmp); */
		remove(BACKUP_RULES_FILENAME);
	}
	else{
		for(index=0 ;index < MaxEntry ; index++)
			if( entry[index].valid )
				upnp_redirect(entry[index].eport, entry[index].int_ip, entry[index].iport, entry[index].protocol, entry[index].desc, entry[index].enabled);
	}
	return 0;
}

static void
BuildSendAndCloseSoapResp(struct upnphttp * h,
                          const char * body, int bodylen)
{
	static const char beforebody[] =
		"<?xml version=\"1.0\"?>\n"
		"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
		"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
		"<s:Body>";
	static const char afterbody[] =
		"</s:Body>"
		"</s:Envelope>";

	BuildHeader_upnphttp(h, 200, "OK",  sizeof(beforebody) - 1
	                                  + sizeof(afterbody) - 1
									  + bodylen );
	memcpy(h->res_buf + h->res_buflen, beforebody, sizeof(beforebody) - 1);
	h->res_buflen += sizeof(beforebody) - 1;
	memcpy(h->res_buf + h->res_buflen, body, bodylen);
	h->res_buflen += bodylen;
	memcpy(h->res_buf + h->res_buflen, afterbody, sizeof(afterbody) - 1);
	h->res_buflen += sizeof(afterbody) - 1;
	SendResp_upnphttp(h);
	CloseSocket_upnphttp(h);
}

static void GetConnectionTypeInfo(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetConnectionTypeInfoResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"<NewConnectionType>IP_Routed</NewConnectionType>"
		"<NewPossibleConnectionTypes>IP_Routed</NewPossibleConnectionTypes>"
		"</u:GetConnectionTypeInfoResponse>";
	BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

static void GetRequestConnection(struct upnphttp * h)
{
	static const char resp[] =
		"<u:RequestConnectionResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"</u:RequestConnectionResponse>";
	BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

#ifdef CONFIG_RTL8186_GR
static void ForceTermination(struct upnphttp * h)
{
        static const char resp[] =
                "<u:ForceTerminationResponse "
                "xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
                "</u:ForceTerminationResponse>";
//        system("echo \"ForceTermination\" >> /tmp/upnp2");//Added for test
        if((wan_type==3)&&(pppCutEnabled==1))
        {
//        	system("echo \"To cutoff pppoe through upnp\" >> /tmp/upnp2");//Added for test
        	//To cutoff pppoe through upnp
        	system("killall -9 igmpproxy 2> /dev/null");
		system("killall -9 dnrd 2> /dev/null");
		system("killall sleep 2> /dev/null");
		system("killall ntp.sh 2> /dev/null");
		system("rm -f /tmp/ntp_tmp 2>/dev/null");
		system("killall pppoe.sh 2> /dev/null");
		system("killall mpppoe.sh 2> /dev/null");
		system("killall pppd 2> /dev/null");
		system("rm -f /etc/ppp/connectfile0 2>/dev/null");
		system("rm -f /etc/ppp/connectfile1 2>/dev/null");
		system("rm -f /etc/ppp/link 2>/dev/null");
        }
        BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

static void GetWarnDisconnectDelay(struct upnphttp * h)
{
	//NewWarnDisconnectDelay=10seconds(default)
	static const char resp[] =
		"<u:GetWarnDisconnectDelayResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"<NewWarnDisconnectDelay>10</NewWarnDisconnectDelay>"
		"</u:GetWarnDisconnectDelayResponse>";
//	system("echo \"GetWarnDisconnectDelay\" >> /tmp/upnp3");//Added for test
	BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

static void RequestTermination(struct upnphttp * h)
{
	static const char resp[] =
		"<u:RequestTerminationResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"</u:RequestTerminationResponse>";
//	system("echo \"RequestTermination\" >> /tmp/upnp4");//Added for test
	BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}
#endif

static void GetTotalBytesSent(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetTotalBytesSentResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1\">"
		"<NewTotalBytesSent>%lu</NewTotalBytesSent>"
		"</u:GetTotalBytesSentResponse>";
	char body[2048];
	int bodylen;
	struct ifdata data;
	unsigned long r;
	
	r = getifstats(ext_if_name, &data,1);
	//if (strcmp(ext_if_name, "lo") && r >= 0 && !data.obytes) // satisfy Vista DTM
		//bodylen = snprintf(body, sizeof(body), resp, (unsigned long)630);
	//else
		if(data.obytes==0)
		{
			data.obytes=5;
		}
		bodylen = snprintf(body, sizeof(body), resp, r<0?5:data.obytes);
	      //printf("eth=%s\n",ext_if_name);
	//syslog("miniigd : GetTotalBytesSent[%lu]", (unsigned long)(r<0?5:data.obytes));
	BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void GetTotalBytesReceived(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetTotalBytesReceivedResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1\">"
		"<NewTotalBytesReceived>%lu</NewTotalBytesReceived>"
		"</u:GetTotalBytesReceivedResponse>";
	char body[2048];
	int bodylen;
	struct ifdata data;
	unsigned int r;
	r = getifstats(ext_if_name, &data,2);
	//if (strcmp(ext_if_name, "lo") && r >= 0 && !data.ibytes) // satisfy Vista DTM
		//bodylen = snprintf(body, sizeof(body), resp, (unsigned long)630);
	//else
		bodylen = snprintf(body, sizeof(body), resp, r<0?0:data.ibytes);
	BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void GetTotalPacketsSent(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetTotalPacketsSentResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1\">"
		"<NewTotalPacketsSent>%lu</NewTotalPacketsSent>"
		"</u:GetTotalPacketsSentResponse>";
	char body[2048];
	int bodylen;
	struct ifdata data;
	int r;
	r = getifstats(ext_if_name, &data,3);
	//if (strcmp(ext_if_name, "lo") && r >= 0 && !data.opackets) // satisfy Vista DTM
		//bodylen = snprintf(body, sizeof(body), resp, (unsigned long)15);
	//else
		bodylen = snprintf(body, sizeof(body), resp, r<0?0:data.opackets);
	BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void GetTotalPacketsReceived(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetTotalPacketsReceivedResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1\">"
		"<NewTotalPacketsReceived>%lu</NewTotalPacketsReceived>"
		"</u:GetTotalPacketsReceivedResponse>";
	char body[2048];
	int bodylen;
	struct ifdata data;
	int r;
	r = getifstats(ext_if_name, &data,4);
	//if (strcmp(ext_if_name, "lo") && r >= 0 && !data.ipackets) // satisfy Vista DTM
		//bodylen = snprintf(body, sizeof(body), resp, (unsigned long)15);
	//else
		bodylen = snprintf(body, sizeof(body), resp, r<0?0:data.ipackets);
	BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void GetCommonLinkProperties(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetCommonLinkPropertiesResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1\">"
		/*"<NewWANAccessType>DSL</NewWANAccessType>"*/
		"<NewWANAccessType>Cable</NewWANAccessType>"
		"<NewLayer1UpstreamMaxBitRate>%lu</NewLayer1UpstreamMaxBitRate>"
		"<NewLayer1DownstreamMaxBitRate>%lu</NewLayer1DownstreamMaxBitRate>"
		"<NewPhysicalLinkStatus>%s</NewPhysicalLinkStatus>"
		"</u:GetCommonLinkPropertiesResponse>";
	char body[2048];
	int bodylen;
	char WAN_Link[10];
	//int r;
	struct ifdata data;
#if 0	//brad disable
	if((downstream_bitrate == 0) || (upstream_bitrate == 0))
	{
//jason modified 20080212
/*
	//	r = getifstats_all(ext_if_name, &data);
		r=0;
		data.baudrate=100000000;
		if(r>=0)
		{
			if(downstream_bitrate == 0)
				downstream_bitrate = data.baudrate;
			if(upstream_bitrate == 0)
				upstream_bitrate = data.baudrate;
		}
*/
		if(getifstats_all(ext_if_name, &data) >= 0)
		{
			if(downstream_bitrate == 0) downstream_bitrate = data.baudrate;
			if(upstream_bitrate == 0) upstream_bitrate = data.baudrate;
		}
	}
#endif
	if(wisp_interface_num < 0){

		if(getWanLink("eth1") < 0){
			sprintf(WAN_Link, "%s", "Down");
			upstream_bitrate=0;
			downstream_bitrate =0;
		}else{
			sprintf(WAN_Link, "%s", "Up");
			if(getifstats_all(ext_if_name, &data) >= 0){
				downstream_bitrate = data.baudrate;
				upstream_bitrate = data.baudrate;
			}
		}
	}else{
		sprintf(WAN_Link, "%s", "Up");
		upstream_bitrate=54000000;
		downstream_bitrate =54000000;
	}
	bodylen = snprintf(body, sizeof(body), resp,
	                   upstream_bitrate, downstream_bitrate, WAN_Link);
	BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void GetStatusInfo(struct upnphttp * h)
{
	static const char zz[] =
		"<u:GetStatusInfoResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"<NewConnectionStatus>%s</NewConnectionStatus>"
		"<NewLastConnectionError>ERROR_NONE</NewLastConnectionError>"
		"<NewUptime>%ld</NewUptime>"
		"</u:GetStatusInfoResponse>";
	char body[512];
	int bodylen;
	time_t uptime;
	char WAN_Link[60];
	int WAN_ret=0;
	struct sysinfo system_info;
	WAN_ret= is_wan_connected();
	
	if(WAN_ret == LINK_UP){
		sprintf(WAN_Link, "%s", "Connected");
		sysinfo(&system_info);
		uptime = (system_info.uptime - wan_uptime);
	}else if(WAN_ret == LINK_WAIT){
		sprintf(WAN_Link, "%s", "Connecting");
		uptime = 0;
	
	}else if(WAN_ret == LINK_DOWN){
		sprintf(WAN_Link, "%s", "Disconnected");
		uptime = 0;
	}
	
	bodylen = snprintf(body, sizeof(body), zz, WAN_Link, (long)uptime);	
	BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void GetNATRSIPStatus(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetNATRSIPStatusResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"<NewRSIPAvailable>0</NewRSIPAvailable>"
		"<NewNATEnabled>1</NewNATEnabled>"
		"</u:GetNATRSIPStatusResponse>";
	BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

static void GetExternalIPAddress(struct upnphttp * h)
{
	static const char zz[] =
		"<u:GetExternalIPAddressResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"<NewExternalIPAddress>%s</NewExternalIPAddress>"
		"</u:GetExternalIPAddressResponse>";
	char body[512];
	int bodylen;
	bodylen = snprintf(body, sizeof(body), zz, ext_ip_addr);
	BuildSendAndCloseSoapResp(h, body, bodylen);
}
#ifdef ENABLE_6FC_SERVICE
#ifndef ENABLE_IPV6
#error "ENABLE_6FC_SERVICE needs ENABLE_IPV6"
#endif
/* WANIPv6FirewallControl actions */
static void
GetFirewallStatus(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetFirewallStatusResponse "
		"xmlns:u=\"%s\">"
		"<FirewallEnabled>%d</FirewallEnabled>"
		"<InboundPinholeAllowed>%d</InboundPinholeAllowed>"
		"</u:GetFirewallStatusResponse>";

	char body[512];
	int bodylen;

	bodylen = snprintf(body, sizeof(body), resp,
		 "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
		ipv6fc_firewall_enabled, ipv6fc_inbound_pinhole_allowed);
	BuildSendAndCloseSoapResp(h, body, bodylen);
}

static int
CheckStatus(struct upnphttp * h)
{
	if (!ipv6fc_firewall_enabled)
	{
		SoapError(h, 702, "FirewallDisabled");
		return 0;
	}
	else if(!ipv6fc_inbound_pinhole_allowed)
	{
		SoapError(h, 703, "InboundPinholeNotAllowed");
		return 0;
	}
	else
		return 1;
}

#if 0
static int connecthostport(const char * host, unsigned short port, char * result)
{
	int s, n;
	char hostname[INET6_ADDRSTRLEN];
	char port_str[8], ifname[8], tmp[4];
	struct addrinfo *ai, *p;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	/* hints.ai_flags = AI_ADDRCONFIG; */
#ifdef AI_NUMERICSERV
	hints.ai_flags = AI_NUMERICSERV;
#endif
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC; /* AF_INET, AF_INET6 or AF_UNSPEC */
	/* hints.ai_protocol = IPPROTO_TCP; */
	snprintf(port_str, sizeof(port_str), "%hu", port);
	strcpy(hostname, host);
	if(!strncmp(host, "fe80", 4))
	{
		printf("Using an linklocal address\n");
		strcpy(ifname, "%");
		snprintf(tmp, sizeof(tmp), "%d", linklocal_index);
		strcat(ifname, tmp);
		strcat(hostname, ifname);
		printf("host: %s\n", hostname);
	}
	n = getaddrinfo(hostname, port_str, &hints, &ai);
	if(n != 0)
	{
		fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(n));
		return -1;
	}
	s = -1;
	for(p = ai; p; p = p->ai_next)
	{
#ifdef DEBUG
		char tmp_host[256];
		char tmp_service[256];
		printf("ai_family=%d ai_socktype=%d ai_protocol=%d ai_addrlen=%d\n ",
		       p->ai_family, p->ai_socktype, p->ai_protocol, p->ai_addrlen);
		getnameinfo(p->ai_addr, p->ai_addrlen, tmp_host, sizeof(tmp_host),
		            tmp_service, sizeof(tmp_service),
		            NI_NUMERICHOST | NI_NUMERICSERV);
		printf(" host=%s service=%s\n", tmp_host, tmp_service);
#endif
		inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)p->ai_addr)->sin6_addr), result, INET6_ADDRSTRLEN);
		return 0;
	}
	freeaddrinfo(ai);
}
#endif

/* Check the security policy right */
static int
PinholeVerification(struct upnphttp * h, char * int_ip, unsigned short int_port)
{
	int n;
	char senderAddr[INET6_ADDRSTRLEN]="";
	struct addrinfo hints, *ai, *p;
	struct in6_addr result_ip;

	/* Pinhole InternalClient address must correspond to the action sender */
	syslog(LOG_INFO, "Checking internal IP@ and port (Security policy purpose)");

	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_UNSPEC;

	/* if ip not valid assume hostname and convert */
	if (inet_pton(AF_INET6, int_ip, &result_ip) <= 0)
	{
		n = getaddrinfo(int_ip, NULL, &hints, &ai);
		if(!n && ai->ai_family == AF_INET6)
		{
			for(p = ai; p; p = p->ai_next)
			{
				inet_ntop(AF_INET6, (struct in6_addr *) p, int_ip, sizeof(struct in6_addr));
				result_ip = *((struct in6_addr *) p);
				/* TODO : deal with more than one ip per hostname */
				break;
			}
		}
		else
		{
			syslog(LOG_ERR, "Failed to convert hostname '%s' to ip address", int_ip);
			SoapError(h, 402, "Invalid Args");
			return -1;
		}
        freeaddrinfo(p);
	}

	if(inet_ntop(AF_INET6, &(h->clientaddr_v6), senderAddr, INET6_ADDRSTRLEN) == NULL)
	{
		syslog(LOG_ERR, "inet_ntop: %m");
	}
#ifdef DEBUG
	printf("\tPinholeVerification:\n\t\tCompare sender @: %s\n\t\t  to intClient @: %s\n", senderAddr, int_ip);
#endif
	if(strcmp(senderAddr, int_ip) != 0)
	if(h->clientaddr_v6.s6_addr != result_ip.s6_addr)
	{
		syslog(LOG_INFO, "Client %s tried to access pinhole for internal %s and is not authorized to do it",
		       senderAddr, int_ip);
		SoapError(h, 606, "Action not authorized");
		return 0;
	}

	/* Pinhole InternalPort must be greater than or equal to 1024 */
	if (int_port < 1024)
	{
		syslog(LOG_INFO, "Client %s tried to access pinhole with port < 1024 and is not authorized to do it",
		       senderAddr);
		SoapError(h, 606, "Action not authorized");
		return 0;
	}
	return 1;
}

static void
AddPinhole(struct upnphttp * h)
{
	int r;
	static const char resp[] =
		"<u:AddPinholeResponse "
		"xmlns:u=\"%s\">"
		"<UniqueID>%d</UniqueID>"
		"</u:AddPinholeResponse>";
	char body[512];
	int bodylen;
	struct NameValueParserData data;
	char * rem_host, * rem_port, * int_ip, * int_port, * protocol, * leaseTime;
	int uid = 0;
	unsigned short iport, rport;
	int ltime;
	long proto;
	char rem_ip[INET6_ADDRSTRLEN];
	if(CheckStatus(h)==0)
		return;
	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	rem_host = GetValueFromNameValueList(&data, "RemoteHost");
	rem_port = GetValueFromNameValueList(&data, "RemotePort");
	int_ip = GetValueFromNameValueList(&data, "InternalClient");
	int_port = GetValueFromNameValueList(&data, "InternalPort");
	protocol = GetValueFromNameValueList(&data, "Protocol");
	leaseTime = GetValueFromNameValueList(&data, "LeaseTime");

	rport = (unsigned short)(rem_port ? atoi(rem_port) : 0);
	iport = (unsigned short)(int_port ? atoi(int_port) : 0);
	ltime = leaseTime ? atoi(leaseTime) : -1;
	errno = 0;
	proto = protocol ? strtol(protocol, NULL, 0) : -1;
	if(errno != 0 || proto > 65535 || proto < 0)
	{
		SoapError(h, 402, "Invalid Args");
		goto clear_and_exit;
	}
	if(iport == 0)
	{
		SoapError(h, 706, "InternalPortWilcardingNotAllowed");
		goto clear_and_exit;
	}

	/* In particular, [IGD2] RECOMMENDS that unauthenticated and
	 * unauthorized control points are only allowed to invoke
	 * this action with:
	 * - InternalPort value greater than or equal to 1024,
	 * - InternalClient value equals to the control point's IP address.
	 * It is REQUIRED that InternalClient cannot be one of IPv6
	 * addresses used by the gateway. */
	if(!int_ip || 0 == strlen(int_ip) || 0 == strcmp(int_ip, "*"))
	{
		SoapError(h, 708, "WildCardNotPermittedInSrcIP");
		goto clear_and_exit;
	}
	/* I guess it is useless to convert int_ip to literal ipv6 address */
	/* rem_host should be converted to literal ipv6 : */
	if(rem_host)
	{
		struct addrinfo *ai, *p;
		struct addrinfo hints;
		int err;
		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = AF_INET6;
		/*hints.ai_flags = */
		/* hints.ai_protocol = proto; */
		err = getaddrinfo(rem_host, rem_port, &hints, &ai);
		if(err == 0)
		{
			/* take the 1st IPv6 address */
			for(p = ai; p; p = p->ai_next)
			{
				if(p->ai_family == AF_INET6)
				{
					inet_ntop(AF_INET6,
					          &(((struct sockaddr_in6 *)p->ai_addr)->sin6_addr),
					          rem_ip, sizeof(rem_ip));
					syslog(LOG_INFO, "resolved '%s' to '%s'", rem_host, rem_ip);
					rem_host = rem_ip;
					break;
				}
			}
			freeaddrinfo(ai);
		}
		else
		{
			syslog(LOG_WARNING, "AddPinhole : getaddrinfo(%s) : %s",
			       rem_host, gai_strerror(err));
#if 0
			SoapError(h, 402, "Invalid Args");
			goto clear_and_exit;
#endif
		}
	}

	if(proto == 65535)
	{
		SoapError(h, 707, "ProtocolWilcardingNotAllowed");
		goto clear_and_exit;
	}
	if(proto != IPPROTO_UDP && proto != IPPROTO_TCP
#ifdef IPPROTO_UDPITE
	   && atoi(protocol) != IPPROTO_UDPLITE
#endif
	  )
	{
		SoapError(h, 705, "ProtocolNotSupported");
		goto clear_and_exit;
	}
	if(ltime < 1 || ltime > 86400)
	{
		syslog(LOG_WARNING, " LeaseTime=%d not supported, (ip=%s)",
		        ltime, int_ip);
		SoapError(h, 402, "Invalid Args");
		goto clear_and_exit;
	}

	if(PinholeVerification(h, int_ip, iport) <= 0)
		goto clear_and_exit;

	syslog(LOG_INFO, "(inbound) from [%s]:%hu to [%s]:%hu with proto %ld during %d sec",
	        rem_host?rem_host:"any",
	       rport, int_ip, iport,
	       proto, ltime);

	/* In cases where the RemoteHost, RemotePort, InternalPort,
	 * InternalClient and Protocol are the same than an existing pinhole,
	 * but LeaseTime is different, the device MUST extend the existing
	 * pinhole's lease time and return the UniqueID of the existing pinhole. */
	r = upnp_add_inboundpinhole(rem_host, rport, int_ip, iport, proto, ltime, &uid);
	switch(r)
	{
		case 1:	        /* success */
			bodylen = snprintf(body, sizeof(body),
			                   resp, 
			                   "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
			                   uid);
			BuildSendAndCloseSoapResp(h, body, bodylen);
			break;
		case -1: 	/* not permitted */
			SoapError(h, 701, "PinholeSpaceExhausted");
			break;
		default:
			SoapError(h, 501, "ActionFailed");
			break;
	}
	/* 606 Action not authorized
	 * 701 PinholeSpaceExhausted
	 * 702 FirewallDisabled
	 * 703 InboundPinholeNotAllowed
	 * 705 ProtocolNotSupported
	 * 706 InternalPortWildcardingNotAllowed
	 * 707 ProtocolWildcardingNotAllowed
	 * 708 WildCardNotPermittedInSrcIP */
clear_and_exit:
	ClearNameValueList(&data);
}

static void
UpdatePinhole(struct upnphttp * h)
{
	static const char resp[] =
		"<u:UpdatePinholeResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPv6FirewallControl:1\">"
		"</u:UpdatePinholeResponse>";
	struct NameValueParserData data;
	const char * uid_str, * leaseTime;
	char iaddr[INET6_ADDRSTRLEN];
	unsigned short iport;
	int ltime;
	int uid;
	int n;

	if(CheckStatus(h)==0)
		return;

	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	uid_str = GetValueFromNameValueList(&data, "UniqueID");
	leaseTime = GetValueFromNameValueList(&data, "NewLeaseTime");
	uid = uid_str ? atoi(uid_str) : -1;
	ltime = leaseTime ? atoi(leaseTime) : -1;
	ClearNameValueList(&data);

	if(uid < 0 || uid > 65535 || ltime <= 0 || ltime > 86400)
	{
		SoapError(h, 402, "Invalid Args");
		return;
	}

	/* Check that client is not updating an pinhole
	 * it doesn't have access to, because of its public access */
#if 1
	n = upnp_get_pinhole_info(uid, NULL, 0, NULL,
	                         iaddr, sizeof(iaddr), &iport,
	                          NULL, NULL, NULL);
#endif
	if (n >= 0)
	{
		if(PinholeVerification(h, iaddr, iport) <= 0)
			return;
	}
	else if(n == -2)
	{
		SoapError(h, 704, "NoSuchEntry");
		return;
	}
	else
	{
		SoapError(h, 501, "ActionFailed");
		return;
	}

	syslog(LOG_INFO, "(inbound) updating lease duration to %d for pinhole with ID: %d",
	        ltime, uid);
#if 1
	n = upnp_update_inboundpinhole(uid, ltime);
#endif
	if(n == -1)
		SoapError(h, 704, "NoSuchEntry");
	else if(n < 0)
		SoapError(h, 501, "ActionFailed");
	else
		BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

static void
GetOutboundPinholeTimeout(struct upnphttp * h)
{
	int r;

	static const char resp[] =
		"<u:GetOutboundPinholeTimeoutResponse "
		"xmlns:u=\"%s\">"
		"<OutboundPinholeTimeout>%d</OutboundPinholeTimeout>"
		"</u:GetOutboundPinholeTimeoutResponse>";

	char body[512];
	int bodylen;
	struct NameValueParserData data;
	char * int_ip, * int_port, * rem_host, * rem_port, * protocol;
	int opt=0, proto=0;
	unsigned short iport, rport;

	if (!ipv6fc_firewall_enabled)
	{
		SoapError(h, 702, "FirewallDisabled");
		return;
	}

	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	int_ip = GetValueFromNameValueList(&data, "InternalClient");
	int_port = GetValueFromNameValueList(&data, "InternalPort");
	rem_host = GetValueFromNameValueList(&data, "RemoteHost");
	rem_port = GetValueFromNameValueList(&data, "RemotePort");
	protocol = GetValueFromNameValueList(&data, "Protocol");

	rport = (unsigned short)atoi(rem_port);
	iport = (unsigned short)atoi(int_port);
	proto = atoi(protocol);

	syslog(LOG_INFO, " retrieving timeout for outbound pinhole from [%s]:%hu to [%s]:%hu protocol %s",  int_ip, iport,rem_host, rport, protocol);

	/* TODO */
	r = -1;/*upnp_check_outbound_pinhole(proto, &opt);*/

	switch(r)
	{
		case 1:	/* success */
			bodylen = snprintf(body, sizeof(body), resp,
			                   "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
			                   opt);
			BuildSendAndCloseSoapResp(h, body, bodylen);
			break;
		case -5:	/* Protocol not supported */
			SoapError(h, 705, "ProtocolNotSupported");
			break;
		default:
			SoapError(h, 501, "ActionFailed");
	}
	ClearNameValueList(&data);
}

static void
DeletePinhole(struct upnphttp * h)
{
	int n;

	static const char resp[] =
		"<u:DeletePinholeResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPv6FirewallControl:1\">"
		"</u:DeletePinholeResponse>";

	struct NameValueParserData data;
	const char * uid_str;
	char iaddr[INET6_ADDRSTRLEN];
	int proto;
	unsigned short iport;
	unsigned int leasetime;
	int uid;

	if(CheckStatus(h)==0)
		return;

	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	uid_str = GetValueFromNameValueList(&data, "UniqueID");
	uid = uid_str ? atoi(uid_str) : -1;
	ClearNameValueList(&data);

	if(uid < 0 || uid > 65535)
	{
		SoapError(h, 402, "Invalid Args");
		return;
	}

	/* Check that client is not deleting an pinhole
	 * it doesn't have access to, because of its public access */
#if 1
	n = upnp_get_pinhole_info(uid, NULL, 0, NULL,
	                          iaddr, sizeof(iaddr), &iport,
	                          &proto, &leasetime, NULL);
#endif
	if (n >= 0)
	{
		if(PinholeVerification(h, iaddr, iport) <= 0)
			return;
	}
	else if(n == -2)
	{
		SoapError(h, 704, "NoSuchEntry");
		return;
	}
	else
	{
		SoapError(h, 501, "ActionFailed");
		return;
	}
#if 1
	n = upnp_delete_inboundpinhole(uid);
#endif
	if(n < 0)
	{
		syslog(LOG_INFO, " (inbound) failed to remove pinhole with ID: %d",
	            uid);
		SoapError(h, 501, "ActionFailed");
		return;
	}
	syslog(LOG_INFO, " (inbound) pinhole with ID %d successfully removed",
	        uid);
	BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

static void
CheckPinholeWorking(struct upnphttp * h)
{
	static const char resp[] =
		"<u:CheckPinholeWorkingResponse "
		"xmlns:u=\"%s\">"
		"<IsWorking>%d</IsWorking>"
		"</u:CheckPinholeWorkingResponse>";
	char body[512];
	int bodylen;
	int r;
	struct NameValueParserData data;
	const char * uid_str;
	int uid;
	char iaddr[INET6_ADDRSTRLEN];
	unsigned short iport;
	unsigned int packets;

	if(CheckStatus(h)==0)
		return;

	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	uid_str = GetValueFromNameValueList(&data, "UniqueID");
	uid = uid_str ? atoi(uid_str) : -1;
	ClearNameValueList(&data);

	if(uid < 0 || uid > 65535)
	{
		SoapError(h, 402, "Invalid Args");
		return;
	}

	/* Check that client is not checking a pinhole
	 * it doesn't have access to, because of its public access */
#if 1
	r = upnp_get_pinhole_info(uid,
	                          NULL, 0, NULL,
	                          iaddr, sizeof(iaddr), &iport,
	                          NULL, NULL, &packets);
#endif
	if (r >= 0)
	{
		if(PinholeVerification(h, iaddr, iport) <= 0)
			return ;
		if(packets == 0)
		{
			SoapError(h, 709, "NoPacketSent");
			return;
		}
		bodylen = snprintf(body, sizeof(body), resp,
						 "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
						1);
		BuildSendAndCloseSoapResp(h, body, bodylen);
	}
	else if(r == -2)
		SoapError(h, 704, "NoSuchEntry");
	else
		SoapError(h, 501, "ActionFailed");
}

static void
GetPinholePackets(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetPinholePacketsResponse "
		"xmlns:u=\"%s\">"
		"<PinholePackets>%u</PinholePackets>"
		"</u:GetPinholePacketsResponse>";
	char body[512];
	int bodylen;
	struct NameValueParserData data;
	const char * uid_str;
	int n;
	char iaddr[INET6_ADDRSTRLEN];
	unsigned short iport;
	unsigned int packets = 0;
	int uid;
	int proto;
	unsigned int leasetime;

	if(CheckStatus(h)==0)
		return;

	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	uid_str = GetValueFromNameValueList(&data, "UniqueID");
	uid = uid_str ? atoi(uid_str) : -1;
	ClearNameValueList(&data);

	if(uid < 0 || uid > 65535)
	{
		SoapError(h, 402, "Invalid Args");
		return;
	}

	/* Check that client is not getting infos of a pinhole
	 * it doesn't have access to, because of its public access */
#if 1
	n = upnp_get_pinhole_info(uid, NULL, 0, NULL,
	                          iaddr, sizeof(iaddr), &iport,
	                          &proto, &leasetime, &packets);
#endif
	if (n >= 0)
	{
		if(PinholeVerification(h, iaddr, iport)<=0)
			return ;
	}
#if 0
	else if(r == -4 || r == -1)
	{
		SoapError(h, 704, "NoSuchEntry");
	}
#endif

	bodylen = snprintf(body, sizeof(body), resp,
			 "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
			packets);
	BuildSendAndCloseSoapResp(h, body, bodylen);
}
#endif

#ifdef SUPPORT_HNAP
unsigned char TotalEntries=0;
#include <sys/stat.h>

#if 0
static int ReliableSend(int socket, const char *data, const int len)
{
	int n;
	unsigned int byte_left = len;
	int bytes_sent = 0;

	if (socket < 0 || data == NULL || len <= 0)
		return -1;

	while (byte_left > 0) {
		// write data
		n = send(socket, data + bytes_sent, byte_left,
			MSG_DONTROUTE | MSG_NOSIGNAL );
		if( n == -1 ) {
			syslog(LOG_ERR, "ReliableSend: sending failed!");
			return -1;
		}

		byte_left = byte_left - n;
		bytes_sent += n;
	}

	n = bytes_sent;
	return n;
}

static void
UnixSocketBuildSendAndClose(struct upnphttp * h,
                          const char * body, int bodylen)
{
	
	int n;

	n = ReliableSend(h->socket, body, bodylen);
	if (n<0)
	{
		syslog(LOG_ERR, "send(res_buf): %m");
	}
	else if (n < bodylen)
	{
		syslog(LOG_ERR, "send(res_buf): %d bytes sent (out of %d)",
						n, bodylen);
	}
	CloseSocket_upnphttp(h);
}
#endif

static void xml_sprintf(const char *tag_name, const char *value, char *buf)
{
	if (value)
		sprintf(buf, "<%s>%s</%s>", tag_name, value, tag_name);
	else
		sprintf(buf, "<%s></%s>", tag_name, tag_name);
}

static void xml_sprintf_int(const char *tag_name, const int value, char *buf)
{
	sprintf(buf, "<%s>%d</%s>", tag_name, value, tag_name);
}

#if 0
static void GetPortMappingsResponse(struct upnphttp * h)
{
	int index;
	int r;
	unsigned short eport, iport;
	char protocol[4];
	char iaddr[32];
	char *desc;
	char *body=NULL;
	char *tmp=NULL;
	unsigned char IsErrorOccur=0;

	body = (char *)malloc(8192);
	tmp = (char *)malloc(1024);
	if (body == NULL || tmp == NULL) {
		IsErrorOccur = 1;
		goto ErrorRes;
	}

	body[0] = 0;
	strcat(body, "<PortMappings>");
	for (index=0; index<MaxEntry; index++) {
		r = upnp_get_redirection_infos_by_index(index, &eport, protocol, &iport,
                                            iaddr, sizeof(iaddr), (const char **)&desc);
		if(r < 0)
			break;

		strcat(body, "<PortMapping>");

		r = SearchEntry(eport,protocol,NULL,0,NULL,0,0);
		if (r >= 0) {
			xml_sprintf("PortMappingDescription", entry[r].desc, tmp);
			strcat(body, tmp);
		}
		else {
			xml_sprintf("PortMappingDescription", desc, tmp);
			strcat(body, tmp);
		}

		xml_sprintf("InternalClient", iaddr, tmp);
		strcat(body, tmp);

		xml_sprintf("PortMappingProtocol", protocol, tmp);
		strcat(body, tmp);

		xml_sprintf_int("ExternalPort", eport, tmp);
		strcat(body, tmp);

		xml_sprintf_int("InternalPort", iport, tmp);
		strcat(body, tmp);
		
		strcat(body, "</PortMapping>");
	}
	strcat(body, "</PortMappings>");

ErrorRes:
	if (IsErrorOccur)
		UnixSocketBuildSendAndClose(h, "ERROR", 5);
	else
		UnixSocketBuildSendAndClose(h, body, strlen(body));
	if (body)
		free(body);
	if (tmp)
		free(tmp);
}
#endif

static void send_to_hnap(const char *in)
{
	int i;
	struct stat status;
	FILE *fpo;

	for (i=0; i<10; i++) {
		if (lstat("/tmp/hnap_igd_rcv", &status) == 0) {
			syslog(LOG_INFO, "Waiting to remove /tmp/hnap_igd_rcv");
			sleep(1);
		}
		else
			break;
	}
	if (i >= 10) {
		syslog(LOG_ERR, "%s %d waiting to remove /tmp/hnap_igd_rcv time out", __FUNCTION__, __LINE__);
		return;
	}

	if ((fpo = fopen("/tmp/hnap_igd_rcv", "w")) == NULL) {
		syslog(LOG_ERR, "%s %d open /tmp/hnap_igd_rcv fails", __FUNCTION__, __LINE__);
		return;
	}

	fputs(in, fpo);
	fclose(fpo);
	syslog(LOG_INFO, "Writing to /tmp/hnap_igd_rcv has been done!");
}

void HNAP_GetPortMappingsResponse(void)
{
	int index;
	int r;
	unsigned short eport, iport;
	char protocol[4];
	char iaddr[32];
	char *desc;
	char *body=NULL;
	char *tmp=NULL;
	unsigned char IsErrorOccur=0;

	body = (char *)malloc(8192);
	tmp = (char *)malloc(1024);
	if (body == NULL || tmp == NULL) {
		IsErrorOccur = 1;
		goto ErrorRes;
	}

	body[0] = 0;
	strcat(body, "<PortMappings>");
	for (index=0; index<MaxEntry; index++) {
		r = upnp_get_redirection_infos_by_index(index, &eport, protocol, &iport,
                                            iaddr, sizeof(iaddr), (const char **)&desc);
		if(r < 0)
			break;

		strcat(body, "<PortMapping>");

		r = SearchEntry(eport,protocol,NULL,0,NULL,0,0);
		if (r >= 0) {
			xml_sprintf("PortMappingDescription", entry[r].desc, tmp);
			strcat(body, tmp);
		}
		else {
			xml_sprintf("PortMappingDescription", desc, tmp);
			strcat(body, tmp);
		}

		xml_sprintf("InternalClient", iaddr, tmp);
		strcat(body, tmp);

		xml_sprintf("PortMappingProtocol", protocol, tmp);
		strcat(body, tmp);

		xml_sprintf_int("ExternalPort", eport, tmp);
		strcat(body, tmp);

		xml_sprintf_int("InternalPort", iport, tmp);
		strcat(body, tmp);
		
		strcat(body, "</PortMapping>");
	}
	strcat(body, "</PortMappings>");

ErrorRes:
	if (IsErrorOccur)
		send_to_hnap("ERROR");
	else {
		syslog(LOG_INFO, "HNAP_GetPortMappingsResponse : [%s]\n", body);
		send_to_hnap(body);
	}
	if (body)
		free(body);
	if (tmp)
		free(tmp);
}

void HNAP_AddPortMapping(const char *in, const int len)
{
	int r;
	struct NameValueParserData data;
	char * int_ip, * int_port, * ext_port, * protocol, * desc;
	unsigned short iport, eport;
      	int enabled;
	char str[20];

	syslog(LOG_INFO, "HNAP_AddPortMapping : [%s] len : [%d]\n", in, len);
	ParseNameValue(in, len, &data);
	int_ip = GetValueFromNameValueList(&data, "InternalClient");
	int_port = GetValueFromNameValueList(&data, "InternalPort");
	ext_port = GetValueFromNameValueList(&data, "ExternalPort");
	protocol = GetValueFromNameValueList(&data, "PortMappingProtocol");
	desc = GetValueFromNameValueList(&data, "PortMappingDescription");
	if (int_ip == NULL || int_port == NULL || desc == NULL ||
		ext_port == NULL || protocol == NULL )
		goto ErrorRes;

	enabled = 1;
	eport = (unsigned short)atoi(ext_port);
	iport = (unsigned short)atoi(int_port);
	strcpy(str,int_ip);
	if (TotalEntries >= 30)
		goto ErrorRes;
	r = SearchEntry(eport,protocol,int_ip,iport,desc,enabled,1);
	r = upnp_redirect(eport, int_ip, iport, protocol, desc,enabled);
	if (r==-2)
		goto ErrorRes;
		
	send_to_hnap("OK");
	ClearNameValueList(&data);
	return;

ErrorRes:
	send_to_hnap("ERROR");
	ClearNameValueList(&data);
	return;
}

void HNAP_DeletePortMapping(const char *in, const int len)
{
	int r;
	struct NameValueParserData data;
	const char * ext_port;
	const char * protocol;
	unsigned short eport;

	syslog(LOG_INFO, "HNAP_DeletePortMapping : [%s] len : [%d]\n", in, len);
	ParseNameValue(in, len, &data);
	ext_port = GetValueFromNameValueList(&data, "ExternalPort");
	protocol = GetValueFromNameValueList(&data, "PortMappingProtocol");
	if (ext_port == NULL || protocol == NULL )
		goto ErrorRes;

	eport = (unsigned short)atoi(ext_port);
       r = SearchEntry(eport, (char *)protocol,NULL,0,NULL,0,0);
	if(r >= 0)
	{
	     	entry[r].valid=0;
		TotalEntries--;
	}
		
	r = upnp_delete_redirection(eport, protocol);
	if(r < 0)
		goto ErrorRes;

	send_to_hnap("OK");
	ClearNameValueList(&data);
	return;

ErrorRes:
	send_to_hnap("ERROR");
	ClearNameValueList(&data);
	return;
}
#endif

static void AddPortMapping(struct upnphttp * h)
{
	int r;
	static const char resp[] =
		"<u:AddPortMappingResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"</u:AddPortMappingResponse>";
	struct NameValueParserData data;
	char * int_ip, * int_port, * ext_port, * protocol, * desc, * bool_enabled;
	unsigned short iport, eport;
//	struct hostent *hp; /* getbyhostname() */
//	char ** ptr; /* getbyhostname() */
//	unsigned char result_ip[16]; /* inet_pton() */
        int enabled;
//	struct in_addr IPaddr;
	char str[20];

#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
	char tmpBuf[200];
#endif

#if 0
//#ifdef SUPPORT_HNAP
	char *p;

	p = strstr(h->req_buf, "POST /HNAP");
	if (p) {
		ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
		int_ip = GetValueFromNameValueList(&data, "InternalClient");
		int_port = GetValueFromNameValueList(&data, "InternalPort");
		ext_port = GetValueFromNameValueList(&data, "ExternalPort");
		protocol = GetValueFromNameValueList(&data, "PortMappingProtocol");
		desc = GetValueFromNameValueList(&data, "PortMappingDescription");
		if (int_ip == NULL || int_port == NULL || desc == NULL ||
			ext_port == NULL || protocol == NULL )
			goto ErrorRes;

		enabled = 1;
		eport = (unsigned short)atoi(ext_port);
		iport = (unsigned short)atoi(int_port);
		strcpy(str,int_ip);
		if (TotalEntries >= 30)
			goto ErrorRes;
		r = SearchEntry(eport,protocol,int_ip,iport,desc,enabled,1);
		r = upnp_redirect(eport, int_ip, iport, protocol, desc,enabled);
		if (r==-2)
			goto ErrorRes;
		
		UnixSocketBuildSendAndClose(h, "OK", 2);
		ClearNameValueList(&data);
		return;

ErrorRes:
		UnixSocketBuildSendAndClose(h, "ERROR", 5);
		ClearNameValueList(&data);
		return;
	}
#endif

	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	int_ip = GetValueFromNameValueList(&data, "NewInternalClient");
	if (!int_ip)
	{
		ClearNameValueList(&data);
		SoapError(h, 402, "Invalid Args");
		return;
	}

	/* if ip not valid assume hostname and convert */
	/*
	if (inet_pton(AF_INET, int_ip, result_ip) <= 0) {
		syslog(LOG_INFO, "Received hostname %s converting to ip address", int_ip);
		hp = gethostbyname(int_ip);
		if(hp && hp->h_addrtype == AF_INET) { 
			for(ptr = hp->h_addr_list; ptr && *ptr; ptr++) {
				int_ip = inet_ntoa(*((struct in_addr *) *ptr));
				// TODO : deal with more than one ip per hostname 
				break;
			}
		} else {
			// can't determine valid ip abort 
			SoapError(h, 402, "Invalid Args");
			return;
		}				
	}
	*/
	int_port = GetValueFromNameValueList(&data, "NewInternalPort");
	ext_port = GetValueFromNameValueList(&data, "NewExternalPort");
	protocol = GetValueFromNameValueList(&data, "NewProtocol");
	desc = GetValueFromNameValueList(&data, "NewPortMappingDescription");
	bool_enabled = GetValueFromNameValueList(&data, "NewEnabled");

	if(!is_ipv4str(int_ip) || !is_number(int_port) || !is_number(ext_port) || !is_number(bool_enabled)  || !is_valid_protocol(protocol))
	{
		ClearNameValueList(&data);
		SoapError(h, 402, "Invalid Args");
		return;
	}
	
//	syslog(LOG_INFO, "AddportMapping Enabled=%d, %s, description : %s",atoi(bool_enabled), int_ip, desc);
	eport = (unsigned short)atoi(ext_port);
	iport = (unsigned short)atoi(int_port);
		
        strcpy(str,int_ip);
	r=SearchEntry(eport,protocol,int_ip,iport,desc,atoi(bool_enabled),1);
	
	enabled=atoi(bool_enabled);
       r = upnp_redirect(eport, int_ip, iport, protocol, desc,enabled);

#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
	//To record (int_ip,ext_port,int_port,protocol,remote_host,bool_enabled) in UPNP_FILE
	sprintf(tmpBuf,"echo \"%s,%s,%s,%s,NA,%s\" >> %s",int_ip,ext_port,int_port,protocol,bool_enabled,UPNP_FILE);
	system(tmpBuf);

	//To add iptables entry of mine
	//sprintf(tmpBuf,"iptables -A MINIUPNPD -d %s -p %s --dport %s -j ACCEPT",int_ip,protocol,int_port);
	//system(tmpBuf);
#endif
	
	/* TODO : handle error case */
	ClearNameValueList(&data);
        if(r==-2)	
	  SoapError(h, 401, "Invalid Action");
	else
	BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

static void GetSpecificPortMappingEntry(struct upnphttp * h)
{
	int r;
	static const char zz[] =
		"<u:GetSpecificPortMappingEntryResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"<NewInternalPort>%u</NewInternalPort>"
		"<NewInternalClient>%s</NewInternalClient>"
		"<NewEnabled>%d</NewEnabled>"
		"<NewPortMappingDescription>%s</NewPortMappingDescription>"
		"<NewLeaseDuration>0</NewLeaseDuration>"
		"</u:GetSpecificPortMappingEntryResponse>";
	char body[2048];
	int bodylen;
	struct NameValueParserData data;
	const char * r_host;
	const char * ext_port;
	const char * protocol;
	unsigned short eport;
	unsigned short iport;
	char int_ip[32];
	const char * desc;
	int bool_enabled=0;
        char str[20],rr;	
	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	r_host = GetValueFromNameValueList(&data, "NewRemoteHost");
	ext_port = GetValueFromNameValueList(&data, "NewExternalPort");
	protocol = GetValueFromNameValueList(&data, "NewProtocol");
	eport = (unsigned short)atoi(ext_port);
	r = upnp_get_redirection_infos(eport, protocol, &iport,
	                               int_ip, sizeof(int_ip), &desc);

        rr=SearchEntry(eport, (char *)protocol,NULL,0,NULL,0,0);

        strcpy(str,int_ip);
        if(!inet_aton(str,&IPaddr) && rr >=0)
	        {
		strcpy(int_ip,entry[rr].int_ip);
		iport=entry[rr].iport;
		}
        if(rr >=0)
		{
		bool_enabled=entry[rr].enabled;
	        strcpy((char *)desc,entry[rr].desc);
	  	r=0;	
		}
	

	
	/* TODO: if args are bad, return error 402 : Invalid Args */
	if(r<0)
	{
		/* error code 714 : not found in array */
		
       		SoapError(h, 714, "NoSuchEntryInArray");
	}
	else
	{
	/*	syslog(LOG_INFO,
		       "GetSpecificPortMappingEntry : "
		       "rhost='%s' %s %s found => %s:%u desc='%s'",
		       r_host, ext_port, protocol, int_ip,
		       (unsigned int)iport, desc);
		       */
              //Jason Modified 20080213		
              //ClearNameValueList(&data);
		bodylen = snprintf(body, sizeof(body),
		                   zz, (unsigned int)iport, int_ip, bool_enabled,desc);
		BuildSendAndCloseSoapResp(h, body, bodylen);
	}

	ClearNameValueList(&data);
}

static void DeletePortMapping(struct upnphttp * h)
{
	int r;
	static const char resp[] =
		"<u:DeletePortMappingResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"</u:DeletePortMappingResponse>";
	struct NameValueParserData data;
	const char * r_host;
	const char * ext_port;
	const char * protocol;
	unsigned short eport;

#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
	struct stat st;
#endif

	char pattern[30];	
	char command[80];

#if 0
//#ifdef SUPPORT_HNAP
	char *p;

	p = strstr(h->req_buf, "POST /HNAP");
	if (p) {
		ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
		ext_port = GetValueFromNameValueList(&data, "ExternalPort");
		protocol = GetValueFromNameValueList(&data, "PortMappingProtocol");
		if (ext_port == NULL || protocol == NULL )
			goto ErrorRes;

		eport = (unsigned short)atoi(ext_port);
        	r = SearchEntry(eport, (char *)protocol,NULL,0,NULL,0,0);
		if(r >= 0)
		{
	      		entry[r].valid=0;
			TotalEntries--;
		}
		
		r = upnp_delete_redirection(eport, protocol);
		if(r < 0)
			goto ErrorRes;

		UnixSocketBuildSendAndClose(h, "OK", 2);
		ClearNameValueList(&data);
		return;

ErrorRes:
		UnixSocketBuildSendAndClose(h, "ERROR", 5);
		ClearNameValueList(&data);
		return;
	}
#endif

	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	r_host = GetValueFromNameValueList(&data, "NewRemoteHost");
	ext_port = GetValueFromNameValueList(&data, "NewExternalPort");
	protocol = GetValueFromNameValueList(&data, "NewProtocol");
	eport = (unsigned short)atoi(ext_port);
        r=SearchEntry(eport, (char *)protocol,NULL,0,NULL,0,0);
	if(r>=0)
	{
	      entry[r].valid=0;

#if defined(CONFIG_RTL8186_GR) || defined(CONFIG_RTL8186_GW)
	      //syslog(LOG_INFO, "r_host=%s,ext_port=%s,protocol=%s, to delete entry %d",r_host,ext_port,protocol,r);
	      //To delete (int_ip,ext_port,int_port,protocol,remote_host,bool_enabled) in UPNP_FILE
	      FILE *fp;
	      char upnpBuf[100];
	      char tmpBuf[200];
	      char *p1;

	      fp = fopen(UPNP_FILE, "r");
	      if(!fp)
	       { 
	       	syslog(LOG_ERR,"Can't open %s", UPNP_FILE);
			//return;
	       }
		 else
	       {
	       	//To clear TMP_FILE
	       	sprintf(tmpBuf,"echo \"\" > %s",TMP_FILE);
			system(tmpBuf);
			
	       	while(1)
		      {
		      		if(!fgets(upnpBuf,sizeof(upnpBuf),fp))break;
				syslog(LOG_INFO, "upnpBuf=%s",upnpBuf);
				p1=strstr(upnpBuf,",");
				if(p1==NULL)
					continue;
				p1++;
				if(strncmp(p1,ext_port,strlen(ext_port))==0)
				{
					//To delete iptables entry of mine
					//sprintf(tmpBuf,"iptables -D MINIUPNPD -d %s -p %s --dport %d -j ACCEPT",entry[r].int_ip,entry[r].protocol,entry[r].iport);
					//system(tmpBuf);
					continue;
				}
				else
				{
					sprintf(tmpBuf,"echo -n \"%s\" >> %s", upnpBuf,TMP_FILE);
					system(tmpBuf);
				}
		     	}
			fclose(fp);
			sprintf(tmpBuf,"cp %s %s", TMP_FILE, UPNP_FILE);
			system(tmpBuf);
			sprintf(tmpBuf,"rm %s", TMP_FILE);
			system(tmpBuf);

			stat(UPNP_FILE,&st);
			if(st.st_size<2)
			{
				sprintf(tmpBuf,"rm %s", UPNP_FILE);
				system(tmpBuf);
			}
		}
#endif
	}
		
	
#ifdef CONFIG_RTL8186_KB	
	//brad modify :default disable broadcast udp/tcp forwarding when delete portmapping
		sprintf(pattern,"%s" ,"0 0.0.0.0 0");
		sprintf(command,"echo %s > /proc/filter_upnp_br",pattern);
		system(command);
#endif	
	r = upnp_delete_redirection(eport, protocol);
	ClearNameValueList(&data);
	if(r<0)
	{
	// error code 714 : not found in array
		SoapError(h, 714, "NoSuchEntryInArray");
	}
	else
		
	{
		BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
	}

	
	
}

static void GetGenericPortMappingEntry(struct upnphttp * h)
{
	static const char resp[] =
		"<u:GetGenericPortMappingEntryResponse "
		"xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
		"<NewRemoteHost></NewRemoteHost>"
		"<NewExternalPort>%u</NewExternalPort>"
		"<NewProtocol>%s</NewProtocol>"
		"<NewInternalPort>%u</NewInternalPort>"
		"<NewInternalClient>%s</NewInternalClient>"
		"<NewEnabled>1</NewEnabled>"
		"<NewPortMappingDescription>%s</NewPortMappingDescription>"
		"<NewLeaseDuration>0</NewLeaseDuration>"
		"</u:GetGenericPortMappingEntryResponse>";
	int index = 0;
	int r;
	unsigned short eport, iport;
	char protocol[4];
	char iaddr[32];
	const char * desc;
	struct NameValueParserData data;
	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	index = atoi(GetValueFromNameValueList(&data, "NewPortMappingIndex"));
	ClearNameValueList(&data);
	r = upnp_get_redirection_infos_by_index(index, &eport, protocol, &iport,
                                            iaddr, sizeof(iaddr), &desc);
	if(r < 0)
	{
		/* if not found error : 713 SpecifiedArrayIndexInvalid */
		SoapError(h, 713, "SpecifiedArrayIndexInvalid");
	}
	else
	{
		int bodylen;
		char body[2048];
		bodylen = snprintf(body, sizeof(body), resp, (unsigned int)eport,
		                   protocol, (unsigned int)iport, iaddr, desc);
		BuildSendAndCloseSoapResp(h, body, bodylen);
	}

	ClearNameValueList(&data);
}

/*
if a control point calls QueryStateVariable on a state variable that is not buffered in memory within (or otherwise available from) the service, the service must return a SOAP fault with an errorCode of 404 Invalid Var.

QueryStateVariable remains useful as a limited test tool but may not be part of some future versions of UPnP.
*/
#if 1
static void QueryStateVariable(struct upnphttp * h)
{
	static const char resp[] =
        "<u:QueryStateVariableResponse "
        "xmlns:u=\"urn:schemas-upnp-org:control-1-0\">"
		"<return>%s</return>"
        "</u:QueryStateVariableResponse>";
	char body[2048];
	int bodylen;
	struct NameValueParserData data;
	const char * var_name;
	char WAN_Link[60];
	int WAN_ret=0;
	WAN_ret= is_wan_connected();
	
	if(WAN_ret == LINK_UP){
		sprintf(WAN_Link, "%s", "Connected");
	}else if(WAN_ret == LINK_WAIT){
		sprintf(WAN_Link, "%s", "Connecting");
	
	}else if(WAN_ret == LINK_DOWN){
		sprintf(WAN_Link, "%s", "Disconnected");
	}
	ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
	/*var_name = GetValueFromNameValueList(&data, "QueryStateVariable"); */
	var_name = GetValueFromNameValueList(&data, "varName");
	//printf("QueryStateVariable(%s)", var_name);
	ClearNameValueList(&data);
	if(!var_name)
	{
		//var_name = strstr(h->req_buf + h->req_contentoff, ":varName");
		//if(var_name) var_name -= 5;
		bodylen = snprintf(body, sizeof(body), resp, WAN_Link); //brad check getwan link
		BuildSendAndCloseSoapResp(h, body, bodylen);
              //SoapError(h, 402, "Invalid Args");
	}
//Jason Modified
//	syslog(LOG_INFO, "QueryStateVariable(%.40s)", var_name);
/*
	bodylen = snprintf(body, sizeof(body), resp, "Connected");
	BuildSendAndCloseSoapResp(h, body, bodylen);
*/
#if 1
	else if(strcmp(var_name, "ConnectionStatus") == 0)
	{	
	bodylen = snprintf(body, sizeof(body), resp, "Connected");//brad check get wanlink
	BuildSendAndCloseSoapResp(h, body, bodylen);
}
#if 1
	else if(strcmp(var_name, "PortMappingNumberOfEntries") == 0)
	{
		int r = 0, index = 0;
		unsigned short eport, iport;
		char protocol[4], iaddr[32], desc[64];
		char strindex[10];

		do
		{
			protocol[0] = '\0'; iaddr[0] = '\0'; desc[0] = '\0';
#if 0
			r = upnp_get_redirection_infos_by_index(index, &eport, protocol, &iport,
													iaddr, sizeof(iaddr),
													desc, sizeof(desc));
#endif
			r = upnp_get_redirection_infos_by_index(index, &eport, protocol, &iport,
                                            iaddr, sizeof(iaddr), (const char **)&desc);
			index++;
		}
		while(r==0);

		snprintf(strindex, sizeof(strindex), "%i", index - 1);
		bodylen = snprintf(body, sizeof(body), resp, strindex);
		BuildSendAndCloseSoapResp(h, body, bodylen);
	}
#endif
	else
	{
		//syslog(LOG_NOTICE, "QueryStateVariable: Unknown: %s", var_name?var_name:"");
		SoapError(h, 404, "Invalid Var");
	}
	//bodylen = snprintf(body, sizeof(body), resp, "Connected");
	//BuildSendAndCloseSoapResp(h, body, bodylen);
	ClearNameValueList(&data);
#endif
}
#endif

/* Windows XP as client send the following requests :
 * GetConnectionTypeInfo
 * GetNATRSIPStatus
 * ? GetTotalBytesSent - WANCommonInterfaceConfig
 * ? GetTotalBytesReceived - idem
 * ? GetTotalPacketsSent - idem
 * ? GetTotalPacketsReceived - idem
 * GetCommonLinkProperties - idem
 * GetStatusInfo - WANIPConnection
 * GetExternalIPAddress
 * QueryStateVariable / ConnectionStatus!
 */
static const struct {
	const char * methodName;
	void (*methodImpl)(struct upnphttp *);
} soapMethods[] =
{
	{ "GetConnectionTypeInfo", GetConnectionTypeInfo },
	{ "GetNATRSIPStatus", GetNATRSIPStatus},
	{ "GetExternalIPAddress", GetExternalIPAddress},
	{ "AddPortMapping", AddPortMapping},
	{ "DeletePortMapping", DeletePortMapping},
	{ "GetGenericPortMappingEntry", GetGenericPortMappingEntry},
	{ "GetSpecificPortMappingEntry", GetSpecificPortMappingEntry},
	{ "QueryStateVariable", QueryStateVariable},
	{ "GetTotalBytesSent", GetTotalBytesSent},
	{ "GetTotalBytesReceived", GetTotalBytesReceived},
	{ "GetTotalPacketsSent", GetTotalPacketsSent},
	{ "GetTotalPacketsReceived", GetTotalPacketsReceived},
	{ "GetCommonLinkProperties", GetCommonLinkProperties},
	{ "GetStatusInfo", GetStatusInfo},
	{ "RequestConnection", GetRequestConnection },
#ifdef CONFIG_RTL8186_GR
	{ "ForceTermination", ForceTermination },
	{ "GetWarnDisconnectDelay", GetWarnDisconnectDelay },//Optional
	{ "RequestTermination", RequestTermination },//Optional
#endif
#if 0
//#ifdef SUPPORT_HNAP
	{ "GetPortMappings", GetPortMappingsResponse},
#endif
#ifdef ENABLE_6FC_SERVICE
	/* WANIPv6FirewallControl */
	{ "GetFirewallStatus", GetFirewallStatus},	/* Required */
	{ "AddPinhole", AddPinhole},				/* Required */
	{ "UpdatePinhole", UpdatePinhole},			/* Required */
	{ "GetOutboundPinholeTimeout", GetOutboundPinholeTimeout},	/* Optional */
	{ "DeletePinhole", DeletePinhole},			/* Required */
	{ "CheckPinholeWorking", CheckPinholeWorking},	/* Optional */
	{ "GetPinholePackets", GetPinholePackets},	/* Required */
#endif

	{ 0, 0 }
};

void ExecuteSoapAction(struct upnphttp * h, const char * action, int n)
{
	char * p;
	int i, len;
	i = 0;

#if 0
//#ifdef SUPPORT_HNAP
	len = strlen("http://purenetworks.com/HNAP");
	if (strncasecmp((char *)action, "http://purenetworks.com/HNAP", len) == 0) {
		char *pos;	
		pos = (char *)(action + len);
		p = strchr(pos, '/');
	}
	else
#endif
	p = strchr(action, '#');
	if(p)
	{
		p++;
		while(soapMethods[i].methodName)
		{
			len = strlen(soapMethods[i].methodName);
			if(strncmp(p, soapMethods[i].methodName, len) == 0)
			{
				soapMethods[i].methodImpl(h);
				return;
			}
			i++;
		}
	}
	syslog(LOG_NOTICE, "Unknown soap method");
	SoapError(h, 401, "Invalid Action");
}

/* Standard errors : 
errorCode  	errorDescription  	Description
401 	Invalid Action 	No action by that name at this service.
402 	Invalid Args 	Could be any of the following: not enough in args, too many in args, no in arg by that name, one or more in args are of the wrong data type.
403 	Out of Sync 	Out of synchronization.
501 	Action Failed 	May be returned in current state of service prevents invoking that action.
600-699 	TBD 	Common action errors. Defined by UPnP Forum Technical Committee.
700-799 	TBD 	Action-specific errors for standard actions. Defined by UPnP Forum working committee.
800-899 	TBD 	Action-specific errors for non-standard actions. Defined by UPnP vendor.
*/
void
SoapError(struct upnphttp * h, int errCode, const char * errDesc)
{
	char body[2048];
	int bodylen;
	static const char resp[] =
		"<s:Envelope "
		"xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
		"s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
		"<s:Body>"
		"<s:Fault>"
		"<faultcode>s:Client</faultcode>"
		"<faultstring>UPnPError</faultstring>"
		"<detail>"
		"<UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\">"
		"<errorCode>%d</errorCode>"
		"<errorDescription>%s</errorDescription>"
		"</UPnPError>"
		"</detail>"
		"</s:Fault>"
		"</s:Body>"
		"</s:Envelope>";
	bodylen = snprintf(body, sizeof(body), resp, errCode, errDesc);
	BuildResp2_upnphttp(h, 500, "Internal Server Error", body, bodylen);
	SendResp_upnphttp(h);
	CloseSocket_upnphttp(h);
}

int SearchEntry(unsigned short eport, char * protocol,char * int_ip,unsigned short iport, const char * desc, char enabled,int valid)
{
  int i;
  for ( i=0; i < MaxEntry; i ++)
    {
    if(eport==entry[i].eport && strcmp(protocol,entry[i].protocol)==0 && entry[i].valid==1)
        {
        return i;
        }
    }
  if(valid==0)
	  return -1;

  for ( i=0; i < MaxEntry; i ++)
    {
    if(entry[i].valid==0)
      {
#ifdef SUPPORT_HNAP
      TotalEntries++;
#endif
      entry[i].iport=iport;
      entry[i].eport=eport;
      entry[i].enabled=enabled;
      if(int_ip!=NULL)
        strcpy(entry[i].int_ip,int_ip);
      strcpy(entry[i].protocol,protocol);
      if(desc!=NULL)
      {
        strcpy( entry[i].desc,desc);
      }
      entry[i].valid=1;
      return i;
      }
    }
  return -1;
}
