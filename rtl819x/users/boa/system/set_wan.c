

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "apmib.h"
#include "mibtbl.h"
#include "sysconf.h"
#include "sys_utility.h"
#include "syswan.h"
#if defined(CONFIG_GET_SERVER_IP_BY_DOMAIN)
#include <netdb.h>
#include <sys/socket.h>
#endif
#define RTL_L2TP_POWEROFF_PATCH 1

extern int setFirewallIptablesRules(int argc, char** argv);
extern int Last_WAN_Mode;
void start_dns_relay(void);
void start_igmpproxy(char *wan_iface, char *lan_iface);
void del_routing(void);
#ifdef CONFIG_IPV6
extern void start_mldproxy(char *wan_iface, char *lan_iface);

#endif
#define DHCPD_CONF_FILE "/var/udhcpd.conf"

#if defined(CONFIG_GET_SERVER_IP_BY_DOMAIN)
unsigned char tmp_default_gw[32], tmp_wan_if[8];
#endif

#ifdef SEND_GRATUITOUS_ARP
#include <net/if_arp.h>
#include <linux/if_ether.h>


#define _CONFIG_SCRIPT_PATH	"/bin"
#define _FIREWALL_SCRIPT_PROG	"firewall.sh"


#define ARP_TABLE_FILE "/proc/net/arp"
#define WAN_STATUS_FILE "/proc/eth1/up_event"
#define GRATUITOUS_ARP_NUM 3

struct arpMsg {
	struct ethhdr ethhdr;	 		/* Ethernet header */
	u_short htype;				/* hardware type (must be ARPHRD_ETHER) */
	u_short ptype;				/* protocol type (must be ETH_P_IP) */
	u_char  hlen;				/* hardware address length (must be 6) */
	u_char  plen;				/* protocol address length (must be 4) */
	u_short operation;			/* ARP opcode */
	u_char  sHaddr[6];			/* sender's hardware address */
	u_char  sInaddr[4];			/* sender's IP address */
	u_char  tHaddr[6];			/* target's hardware address */
	u_char  tInaddr[4];			/* target's IP address */
	u_char  pad[18];			/* pad for min. Ethernet payload (60 bytes) */
};
#define MAC_BCAST_ADDR		(unsigned char *) "\xff\xff\xff\xff\xff\xff"
int sendArpPack(unsigned char *mac, u_int32_t srcIp, u_int32_t targetIp)
{

	int 	optval = 1;
	int	s;			/* socket */
	int	rv = 1;			/* return value */
	struct sockaddr addr;		/* for interface name */
	struct arpMsg	arp;

	if ((s = socket (PF_PACKET, SOCK_PACKET, htons(ETH_P_ARP))) == -1) {
		return -1;
	}
	
	if (setsockopt(s, SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)) == -1) {
		close(s);
		return -1;
	}

	/* send arp request */
	memset(&arp, 0, sizeof(arp));
	memcpy(arp.ethhdr.h_dest, MAC_BCAST_ADDR, 6);	/* MAC DA */
	memcpy(arp.ethhdr.h_source, mac, 6);		/* MAC SA */
	arp.ethhdr.h_proto = htons(ETH_P_ARP);		/* protocol type (Ethernet) */
	arp.htype = htons(ARPHRD_ETHER);		/* hardware type */
	arp.ptype = htons(ETH_P_IP);			/* protocol type (ARP message) */
	arp.hlen = 6;					/* hardware address length */
	arp.plen = 4;					/* protocol address length */
	arp.operation = htons(ARPOP_REQUEST);		/* ARP op code */
	memcpy(arp.sInaddr, &srcIp, sizeof(srcIp));		/* source IP address */
	memcpy(arp.sHaddr, mac, 6);			/* source hardware address */
	memcpy(arp.tInaddr, &targetIp, sizeof(targetIp));	/* target IP address */
	
	memset(&addr, 0, sizeof(addr));
	strcpy(addr.sa_data, "eth1");//interface);

	if (sendto(s, &arp, sizeof(arp), 0, &addr, sizeof(addr)) < 0)
		rv = 0;

	close(s);
	//DEBUG(LOG_INFO, "%salid arp replies for this address", rv ? "No v" : "V");	 
	return rv;
}

int sendArp()
{
	int i;
	char ip[24];
	char wanMacAddr[24];
	struct in_addr wanaddr;

	getInAddr("eth1", IP_ADDR_T, (void *)&wanaddr);
	sprintf(ip, "%s", inet_ntoa(wanaddr));

	bzero(wanMacAddr,sizeof(wanMacAddr));
	apmib_get(MIB_WAN_MAC_ADDR,  (void *)wanMacAddr);
	if(!memcmp(wanMacAddr, "\x00\x00\x00\x00\x00\x00", 6)){
		apmib_get(MIB_HW_NIC1_ADDR,  (void *)wanMacAddr);
	}
	for(i=0;i<GRATUITOUS_ARP_NUM;i++)
	{
		sendArpPack(wanMacAddr,wanaddr.s_addr, wanaddr.s_addr);
		sleep(1);
	}
}

int checkWanStatus()
{ 
	FILE *pfile = NULL;
	int status = -1;
	int wan_type = -1;
	struct in_addr wanaddr;
	char ip[24];
	char tmpBuf[128];
	char wanMacAddr[24];
	int i;
	
	if(!isFileExist(WAN_STATUS_FILE))
	{
		printf("%s: %s is not exist!!\n",__FUNCTION__, WAN_STATUS_FILE);
		return -1;
	}
	apmib_get(MIB_WAN_DHCP,(void *)&wan_type);
	if(DHCP_DISABLED != wan_type)
	{	
		return -1;
	}
	if((pfile = fopen(WAN_STATUS_FILE,"r+"))!= NULL)
	{
		fscanf(pfile,"%d",&status);
		if(status == 1)
		{		
			RunSystemCmd(WAN_STATUS_FILE, "echo", "0", NULL_STR);	/*bridge mode with multiple vlan*/
			sendArp();
		}
		fclose(pfile);
	}
	else
	{
		return -1;
	}
	return 0;
}

#endif
int avoid_confliction_ip(char *wanIp, char *wanMask)
{
	char line_buffer[100]={0};
	char *strtmp=NULL;
	char tmp1[64]={0};
	unsigned int tmp1Val;
	struct in_addr inIp, inMask, inGateway;
	struct in_addr myIp, myMask, mask;
	unsigned int inIpVal, inMaskVal, myIpVal, myMaskVal, maskVal;
	char tmpBufIP[64]={0}, tmpBufMask[64]={0};
	DHCP_T dhcp;
	
	apmib_get( MIB_DHCP, (void *)&dhcp);
	
	if(isFileExist(DHCPD_PID_FILE) == 0 || dhcp == DHCP_SERVER){

	}else{
		return 0; //no dhcpd or dhcp server is disable
	}
	
	if ( !inet_aton(wanIp, &inIp) ) {
		printf("\r\n Invalid IP-address value!__[%s-%u]\r\n",__FILE__,__LINE__);
		return 0;
	}
	
	if ( !inet_aton(wanMask, &inMask) ) {
		printf("\r\n Invalid IP-address value!__[%s-%u]\r\n",__FILE__,__LINE__);
		return 0;
	}
	
	memcpy(&inIpVal, &inIp, 4);
	memcpy(&inMaskVal, &inMask, 4);


	getInAddr("br0", IP_ADDR_T, (void *)&myIp );	
	getInAddr("br0", NET_MASK_T, (void *)&myMask );
		
	
	memcpy(&myIpVal, &myIp, 4);
	memcpy(&myMaskVal, &myMask, 4);

//printf("\r\n inIpVal=[0x%x],__[%s-%u]\r\n",inIpVal,__FILE__,__LINE__);
//printf("\r\n inMaskVal=[0x%x],__[%s-%u]\r\n",inMaskVal,__FILE__,__LINE__);
//printf("\r\n myIpVal=[0x%x],__[%s-%u]\r\n",myIpVal,__FILE__,__LINE__);
//printf("\r\n myMaskVal=[0x%x],__[%s-%u]\r\n",myMaskVal,__FILE__,__LINE__);

	memcpy(&maskVal,myMaskVal>inMaskVal?&inMaskVal:&myMaskVal,4);
	
//printf("\r\n maskVal=[0x%x],__[%s-%u]\r\n",maskVal,__FILE__,__LINE__);
	
	if((inIpVal & maskVal) == (myIpVal & maskVal)) //wan ip conflict lan ip 
	{
		int i=0, j=0;
//printf("\r\n wan ip conflict lan ip!,__[%s-%u]\r\n",__FILE__,__LINE__);

		for(i=0; i<32; i++)
		{
			if((maskVal & (1<<i)) != 0)
				break;
		}
		
		if((myIpVal & (1<<i)) == 0)
		{
			myIpVal = myIpVal+(1<<i);
		}
		else
		{
			myIpVal = myIpVal-(1<<i);
		}
		
		memcpy(&myIp, &myIpVal, 4);
				
						
		for(j=0; j<32; j++)
		{
			if((maskVal & (1<<j)) != 0)
				break;
		}
		
	//	j=(32-j)/8;

		system("killall -9 udhcpd 2> /dev/null");
		system("rm -f /var/run/udhcpd.pid 2> /dev/null");
		system("rm -f /var/udhcpd.conf");
		
		sprintf(line_buffer,"interface %s\n","br0");
		write_line_to_file(DHCPD_CONF_FILE, 1, line_buffer);
		
		apmib_get(MIB_DHCP_CLIENT_START,  (void *)tmp1);		
	//	memcpy(tmp1, &myIpVal,  j);
		*(unsigned int*)tmp1 ^= (1<<(j));
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
//printf("\r\n start ip=[%s],__[%s-%u]\r\n",strtmp,__FILE__,__LINE__);		
		sprintf(line_buffer,"start %s\n",strtmp);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		
		apmib_get(MIB_DHCP_CLIENT_END,  (void *)tmp1);		
		//memcpy(tmp1, &myIpVal,  j);
		*(unsigned int*)tmp1 ^= (1<<(j));
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
//printf("\r\n end ip=[%s],__[%s-%u]\r\n",strtmp,__FILE__,__LINE__);		
		sprintf(line_buffer,"end %s\n",strtmp);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	
//printf("\r\n subnet mask=[%s],__[%s-%u]\r\n",inet_ntoa(myMask),__FILE__,__LINE__);			
		sprintf(line_buffer,"opt subnet %s\n",inet_ntoa(myMask));
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		
		apmib_get(MIB_DHCP_LEASE_TIME, (void *)&tmp1Val);
		if( (tmp1Val==0) || (tmp1Val<0) || (tmp1Val>10080))
		{
			tmp1Val = 480; //8 hours
			if(!apmib_set(MIB_DHCP_LEASE_TIME, (void *)&tmp1Val))
			{
				printf("set MIB_DHCP_LEASE_TIME error\n");
			}
		
			apmib_update(CURRENT_SETTING);
		}
		tmp1Val *= 60;

		sprintf(line_buffer,"opt lease %ld\n",tmp1Val);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

//printf("\r\n gateway ip=[%s],__[%s-%u]\r\n",inet_ntoa(myIp),__FILE__,__LINE__);					
		sprintf(line_buffer,"opt router %s\n",inet_ntoa(myIp));
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

//printf("\r\n dns ip=[%s],__[%s-%u]\r\n",inet_ntoa(myIp),__FILE__,__LINE__);							
		sprintf(line_buffer,"opt dns %s\n",inet_ntoa(myIp)); /*now strtmp is ip address value */
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		
		memset(tmp1,0x00,sizeof(tmp1));
		apmib_get( MIB_DOMAIN_NAME, (void *)&tmp1);
		if(tmp1[0]){
			sprintf(line_buffer,"opt domain %s\n",tmp1);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		
		memset(tmp1,0x00,sizeof(tmp1));
		memcpy(tmp1, &myIpVal,  4);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(tmpBufIP,"%s",strtmp);
//printf("\r\n tmpBufIP=[%s],__[%s-%u]\r\n",tmpBufIP,__FILE__,__LINE__);

		memset(tmp1,0x00,sizeof(tmp1));
		memcpy(tmp1, &myMaskVal,  4);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(tmpBufMask,"%s",strtmp);
//printf("\r\n tmpBufMask=[%s],__[%s-%u]\r\n",tmpBufMask,__FILE__,__LINE__);

		memset(line_buffer,0x00,sizeof(line_buffer));
		sprintf(line_buffer, "ifconfig br0 %s netmask %s", tmpBufIP, tmpBufMask);
//printf("\r\n line_buffer=[%s],__[%s-%u]\r\n",line_buffer,__FILE__,__LINE__);									
		system(line_buffer);

		sprintf(line_buffer, "udhcpd %s", DHCPD_CONF_FILE);
		system(line_buffer);
		//start_dnrd();
		return 1;
	}

	return 0;
}
#if defined(CONFIG_GET_SERVER_IP_BY_DOMAIN)
int translate_domain_to_ip(unsigned char *server_domain, struct in_addr *server_ip)
{
	unsigned char tmp_server_ip[32];	
	unsigned char str[32], tmp_cmd[128];
	char   **pptr;
	struct hostent *hptr;
	int count=0;
		
	while(count<=3)
	{
		if((hptr = gethostbyname(server_domain)) != NULL)
		{
			sprintf(tmp_server_ip, "%s", inet_ntop(hptr->h_addrtype, hptr->h_addr, str, sizeof(str)));
			inet_aton(tmp_server_ip, (void *)server_ip);
			return 0;		
		}else
		{
			printf(" gethostbyname error for host:%s try again!\n", server_domain);
			count++;
		}
	}
	return -1;
}

#endif

#ifdef CONFIG_IPV6
void ppp_connect_ipv6(char *ifname, char *option)
{
	char tmpStr[256];
	char gateway[64];
	FILE *fp =NULL;
	int val;
	addr6CfgParam_t	addr6_wan;
	if(!apmib_get(MIB_IPV6_WAN_ENABLE,&val)){		
		fprintf(stderr, "get mib %d error!\n", MIB_IPV6_WAN_ENABLE);			return ;			
	}
	else if(val==0)
		return;
		
	sprintf(tmpStr,"/var/gateway_ipv6");
	fp=fopen(tmpStr,"r");		
	if(fp!=NULL){
		fscanf(fp,"%s",gateway);
		fclose(fp);
		/*add default gateway*/					
		sprintf(tmpStr,"route -A inet6 add default gw %s dev %s",gateway,ifname);
		system(tmpStr);			
	}

	if(!apmib_get(MIB_IPV6_ORIGIN_TYPE,&val)){	
		fprintf(stderr, "get mib %d error!\n", MIB_IPV6_ORIGIN_TYPE);
		return;			
	}

		
	switch(val){			
		case IPV6_ORIGIN_DHCP:
			/*disable forwarding proc to make slaac enable in kernel*/
			sprintf(tmpStr,"echo 0 > /proc/sys/net/ipv6/conf/%s/forwarding",ifname);
			system(tmpStr);
			set_dhcp6c();								
			break;
	
		case IPV6_ORIGIN_STATIC:					
			/*ifconfig ipv6 address*/
			if ( !apmib_get(MIB_IPV6_ADDR_WAN_PARAM,(void *)&addr6_wan)){
				fprintf(stderr, "get mib %d error!\n", MIB_IPV6_ADDR_WAN_PARAM);
				return ;        
			}

			sprintf(tmpStr,"ifconfig %s %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d",
				ifname,
				addr6_wan.addrIPv6[0],addr6_wan.addrIPv6[1],addr6_wan.addrIPv6[2],
				addr6_wan.addrIPv6[3],addr6_wan.addrIPv6[4],addr6_wan.addrIPv6[5],
				addr6_wan.addrIPv6[6],addr6_wan.addrIPv6[7],addr6_wan.prefix_len);
			system(tmpStr);						
			break;
				
		default:
			break;
	}	

	//mldproxy
	start_mldproxy(ifname,"br0");
	return;
}
#endif

#ifdef SUPPORT_ZIONCOM_RUSSIA
void appendDnsAddr(char *ppp_resolv_file, char *resolv_file)
{	
	if(!ppp_resolv_file || !resolv_file)
		return ;
	FILE *fp1=NULL, *fp2=NULL;
	if((fp1=fopen(ppp_resolv_file, "r"))==NULL)
		goto OUT;	
	
	char tmpbuf[64], tmpbuf1[64], tmpbuf2[64];
	int found, i;
	while(fgets(tmpbuf1, sizeof(tmpbuf1), fp1))
	{
		for(i=0;tmpbuf1[i]!='\n' && tmpbuf1[i]!='\0'; i++);
		tmpbuf1[i]='\0';		
		
		found=0;
		
		if((fp2=fopen(resolv_file, "r+"))==NULL)
			goto OUT;
		
		while(fgets(tmpbuf2, sizeof(tmpbuf2), fp2))
		{
			for(i=0;tmpbuf2[i]!='\n' && tmpbuf2[i]!='\0'; i++);
			tmpbuf2[i]='\0';			
			
			if(strcmp(tmpbuf1, tmpbuf2)==0)
			{
				found=1;
				break;
			}
		}
		if(found==0)
		{
			sprintf(tmpbuf,"%s\n", tmpbuf1);
			write_line_to_file(resolv_file, 2, tmpbuf);
		}		
		fclose(fp2);
		fp2=NULL;
	}
	
OUT:
	
	if(fp1!=NULL)
	{
		fclose(fp1);
		fp1=NULL;
	}
	if(fp2!=NULL)
	{
		fclose(fp2);
		fp2=NULL;
	}
	return;
}
#endif

void wan_connect(char *interface, char *option)
{
	char line[128], arg_buff[200];
	char *cmd_opt[16];
	int cmd_cnt = 0, intValue=0, x, dns_mode=0, index=0;
	int dns_found=0, wan_type=0, conn_type=0, ppp_mtu=0;
#ifdef TR181_SUPPORT
	int dnsEnable, value;
#endif
	struct in_addr wanaddr, lanaddr;
	char *strtmp=NULL;
	char wanip[32]={0}, mask[32]={0},remoteip[32]={0};
	char nameserver[32], nameserver_ip[32];
	char dns_server[5][32];
	char tmp_args[16]={0};
	char *token=NULL, *savestr1=NULL;
	FILE *fp1, *fp2;
	unsigned char domanin_name[MAX_NAME_LEN]={0};
	unsigned char cmdBuffer[100]={0};
	unsigned char tmpBuff[200]={0};
	unsigned char dynip[32]={0};
	int lan_type=0;
	int op_mode=0;
	int ret = 0;
//	printf("%s(%d): wan_connect option=%s\n",__FUNCTION__,__LINE__, option);//Added for test
//	printf("%s(%d): wan_connect interface=%s\n",__FUNCTION__,__LINE__, interface);//Added for test
	#if defined(CONFIG_DYNAMIC_WAN_IP)
	int opmode=0, wisp_wan_id=0;
	char tmp_buf[64]={0};
	char ServerIp[32],netIp[32];
	unsigned int serverAddr,netAddr;
	struct in_addr tmpInAddr;
	unsigned int wanIpAddr, maskAddr, remoteIpAddr;
	#endif
	apmib_get(MIB_WAN_DHCP,(void *)&wan_type);
	apmib_get( MIB_DNS_MODE, (void *)&dns_mode);
	apmib_get(MIB_DHCP,(void *)&lan_type);
	apmib_get(MIB_OP_MODE, (void *)&op_mode);
#if defined(CONFIG_DYNAMIC_WAN_IP)
	apmib_get(MIB_OP_MODE, (void *)&opmode);
	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_wan_id);	
#endif

#ifdef TR181_SUPPORT
	if ( !apmib_get(MIB_DNS_CLIENT_ENABLE,(void *)&dnsEnable)){
			fprintf(stderr,"get MIB_DNS_CLIENT_ENABLE failed\n");
			return;  
	}
	DNS_CLIENT_SERVER_T entry[2]={0};
	int y = 0;
	
	for(x=0; x<6; x++)
	{	
		y = x+1;
		*((char*)entry)=(char)y;
		if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
		{
			printf("%s(%d): get MIB_DNS_CLIENT_SERVER_TBL fail!\n",__FUNCTION__,__LINE__);
			return;
		}
		memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
		entry[1].index = x;
		entry[1].enable = 0;
		entry[1].status = 0;
//		strcpy(entry[1].alias, "");
//		strcpy(entry[1].ipAddr, "");
//		strcpy(entry[1].interface, "");
//		entry[1].type = 0; //Unknown
		if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
		{
			printf("%s(%d): set MIB_DNS_CLIENT_SERVER_TBL fail!\n",__FUNCTION__,__LINE__);
			return;
		}
	}
#endif

	//when lan set dhcp client,only br0 con allowed.wan conn make no sense
	if(lan_type==DHCP_CLIENT && strcmp(interface, "br0")!=0)
		return;
	
#ifdef MULTI_PPPOE
	if(wan_type > 2 && (!strncmp(interface, "ppp",3))){
#else
#if defined(CONFIG_DYNAMIC_WAN_IP)
	if(!strcmp(interface, "ppp0")){
#else
	if(wan_type > 2 && !strcmp(interface, "ppp0")){
#endif
#endif
		
#if 1//AVOID_CONFLICTION_IP
#ifdef MULTI_PPPOE
		getInAddr(interface, IP_ADDR_T, (void *)&wanaddr);
#else
		getInAddr("ppp0", IP_ADDR_T, (void *)&wanaddr);
#endif		
		strtmp = inet_ntoa(wanaddr);
		sprintf(wanip, "%s",strtmp); 
#ifdef MULTI_PPPOE
		getInAddr(interface, NET_MASK_T, (void *)&wanaddr);
#else
		getInAddr("ppp0", NET_MASK_T, (void *)&wanaddr);
#endif			
		strtmp = inet_ntoa(wanaddr);
		sprintf(mask, "%s",strtmp); 
		ret = avoid_confliction_ip(wanip,mask);
#endif

#if defined(CONFIG_DYNAMIC_WAN_IP)
		if(wan_type==PPTP || wan_type==L2TP){
			if(opmode==GATEWAY_MODE)
				RunSystemCmd(NULL_FILE, "route", "del", "default", "dev", "eth1", NULL_STR);
			if(opmode==WISP_MODE)
				RunSystemCmd(NULL_FILE, "route", "del", "default", "dev", "wlan0", NULL_STR);
		}
#endif

		if(wan_type==PPTP){
			apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&conn_type);
			if(intValue==1){
				RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "5", NULL_STR);
			}else{
				RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "0", NULL_STR);
			}
		}
		if((wan_type==PPPOE)||(wan_type==PPTP)||(wan_type==L2TP))
		{
#ifdef MULTI_PPPOE
			intValue = getInAddr(interface, 0, (void *)&wanaddr);
#else
			intValue = getInAddr("ppp0", 0, (void *)&wanaddr);
#endif			
			if(intValue==1){

				strtmp = inet_ntoa(wanaddr);
				sprintf(remoteip, "%s",strtmp); 
#ifdef MULTI_PPPOE
#else
				RunSystemCmd(NULL_FILE, "route", "del", "default", NULL_STR);
#endif

#ifdef MULTI_PPPOE
			RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", remoteip, "dev", interface, NULL_STR);
#else
			RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", remoteip, "dev", "ppp0", NULL_STR);
#endif				
			}			
		}
		if(wan_type==PPTP || wan_type==L2TP){
			token=NULL;
			savestr1=NULL;	     
			sprintf(arg_buff, "%s", option);
		
			token = strtok_r(arg_buff," ", &savestr1);
			x=0;
			do{
				if (token == NULL){/*check if the first arg is NULL*/
					break;
				}else{   
					if(x==1){
						ppp_mtu = atoi(token);
						break;
					}
					if(!strcmp(token, "mtu"))
						x=1;
				}
			
				token = strtok_r(NULL, " ", &savestr1);
			}while(token !=NULL);  
		
		}
		if(wan_type==PPTP){
			apmib_get(MIB_PPTP_MTU_SIZE, (void *)&intValue);
			if(ppp_mtu > 0 && intValue > ppp_mtu)
				intValue = ppp_mtu;
			sprintf(tmp_args, "%d", intValue);
		}else if(wan_type==L2TP){
			apmib_get(MIB_L2TP_MTU_SIZE, (void *)&intValue);
			if(ppp_mtu > 0 && intValue > ppp_mtu)
				intValue = ppp_mtu;
			sprintf(tmp_args, "%d", intValue);
		}else if(wan_type==PPPOE){
			apmib_get(MIB_PPP_MTU_SIZE, (void *)&intValue);
			sprintf(tmp_args, "%d", intValue);			
		}
#ifdef MULTI_PPPOE
		/* Do not set mtu by ifconfig, pppd negotiates about mtu by itself */
		//RunSystemCmd(NULL_FILE, "ifconfig", interface, "mtu", tmp_args, "txqueuelen", "25",NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", interface, "txqueuelen", "64",NULL_STR);
#else
		/* Do not set mtu by ifconfig, pppd negotiates about mtu by itself */
		//RunSystemCmd(NULL_FILE, "ifconfig", "ppp0", "mtu", tmp_args, "txqueuelen", "25",NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "ppp0", "txqueuelen", "64",NULL_STR);
#endif
//		printf("%s(%d): wan_type=%d,dns_mode=%d\n",__FUNCTION__,__LINE__, wan_type,dns_mode);//Added for test
#ifdef TR181_SUPPORT
		if(dnsEnable==1)
#endif
		{
#ifdef SUPPORT_ZIONCOM_RUSSIA
		if(wan_type!=PPTP && wan_type!=L2TP && dns_mode==1)
#else
		if(dns_mode==1)
#endif
		{
			start_dns_relay();
		}else{
#ifndef SUPPORT_ZIONCOM_RUSSIA
			fp1= fopen(PPP_RESOLV_FILE, "r");
#else
			
			appendDnsAddr(PPP_RESOLV_FILE, "/var/resolv.conf");
//			system("cat /etc/ppp/resolv.conf >> /var/resolv.conf");
			fp1= fopen("/var/resolv.conf", "r");
#endif
			if (fp1 != NULL){
				for (x=0;x<5;x++){
					memset(dns_server[x], '\0', 32);
				}
				while (fgets(line, sizeof(line), fp1) != NULL) {
						memset(nameserver_ip, '\0', 32);
						dns_found = 0;
						sscanf(line, "%s %s", nameserver, nameserver_ip);
						for(x=0;x<5;x++){
							if(dns_server[x][0] != '\0'){
								if(!strcmp(dns_server[x],nameserver_ip)){
									dns_found = 1; 
									break;
								}
							}
						}
						if(dns_found ==0){
							for(x=0;x<5;x++){
								if(dns_server[x][0] == '\0'){
									sprintf(dns_server[x], "%s", nameserver_ip);
									break;
								}
							}
						}
				}
				fclose(fp1);
			}else
			{//PPP_RESOLV_FILE not exist, use default dns
				sprintf(dns_server[0], "%s", "168.95.1.1");	
				
				//printf("---%s---\n","168.95.1.1");
			}
#ifdef TR181_SUPPORT
			for(x=0; x<3; x++)
			{
				if(dns_server[x][0] == '\0')
					continue;
				y = x+1;
				*((char*)entry)=(char)y;

				if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
				{
					printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
					return;
				}
				memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
				entry[1].index = x;
				entry[1].enable = 1;
				entry[1].status = 1;
				strcpy(entry[1].ipAddr, dns_server[x]);
//				strcpy(entry[1].interface, interface);
				entry[1].type = 6; //Unkown
				if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
				{
					printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
					return;
				}
			}
#endif
			//for (x=0;x<5;x++){
			//	if(dns_server[x]){
			//		fprintf(stderr, "name server=%s\n", dns_server[x]);
			//	}
			//}
			RunSystemCmd(NULL_FILE, "killall", "dnrd", NULL_STR);
			if(isFileExist(DNRD_PID_FILE)){
				unlink(DNRD_PID_FILE);
			}		
			apmib_get( MIB_DOMAIN_NAME,  (void *)domanin_name);					
			getInAddr("br0", IP_ADDR_T, (void *)&lanaddr);
			strtmp = inet_ntoa(lanaddr);
			sprintf((char *)dynip, "%s",strtmp); 						
		#if !defined(CONFIG_RTL_ULINKER)
			RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/hosts", NULL_STR);
			memset(cmdBuffer, 0x00, sizeof(cmdBuffer));
			if(domanin_name[0])
			{
				sprintf((char *)cmdBuffer,"%s\\%s%s%s%s", dynip, domanin_name, "AP.com|",domanin_name, "AP.net");				
			}
			else
			{
				sprintf((char *)cmdBuffer,"%s\\%s%s%s%s", dynip, "realtek", "AP.com|","realtek", "AP.net");
			}
			RunSystemCmd("/etc/hosts", "echo",cmdBuffer,NULL_STR);
		#else
			//usb0_up();
		#endif /* #if !defined(CONFIG_RTL_ULINKER) */
			cmd_opt[cmd_cnt++]="dnrd";
			cmd_opt[cmd_cnt++]="--cache=off";

			for(x=0;x<5;x++){
				if(dns_server[x][0] != '\0'){
//#ifdef TR181_SUPPORT
#if 0
					sprintf(line, "Device.DNS.Client.Server.{%d}.Enable", x);
					if(tr181_ipv6_get(line, (void*)&value) == -1)
					{
						printf("get %s fail!\n", line);
						return;
					}
					if(value != 1)
						continue;
#endif
					cmd_opt[cmd_cnt++]="-s";
					cmd_opt[cmd_cnt++]=&dns_server[x][0];
				}
			}

			cmd_opt[cmd_cnt++] = 0;
			//for (x=0; x<cmd_cnt;x++)
			//	fprintf(stderr, "cmd index=%d, opt=%s \n", x, cmd_opt[x]);
			
#ifndef SUPPORT_ZIONCOM_RUSSIA

			RunSystemCmd(NULL_FILE, "cp", PPP_RESOLV_FILE, "/var/resolv.conf", NULL_STR);			
#else			
//			system("cat /etc/ppp/resolv.conf >> /var/resolv.conf");
			strcpy(line, cmd_opt[0]);
			for (x=1; x<cmd_cnt-1;x++)
			{
			   strcat(line, " ");
			   strcat(line, cmd_opt[x]);
			}			
			write_line_to_file("/var/dnrd_cmd_line", 1, line);
#endif
			DoCmd(cmd_opt, NULL_FILE);
		}
		}
	}else 
#if defined(CONFIG_DYNAMIC_WAN_IP)
	if(strcmp(interface, "ppp0")){	
#else
	if(wan_type == 1 && (strncmp(interface, "ppp",3))){//dhcp conn
#endif
		for (x=0;x<5;x++){
			memset(dns_server[x], '\0', 32);
		}
		token=NULL;
		savestr1=NULL;	     
		sprintf(arg_buff, "%s", option);
	
		token = strtok_r(arg_buff," ", &savestr1);
		index=1;
		do{
			dns_found=0;
			if (token == NULL){/*check if the first arg is NULL*/
				break;
			}else{   
				if(index==2)
					sprintf(wanip, "%s", token); /*wan ip address */
				if(index==3)
					sprintf(mask, "%s", token); /*subnet mask*/
				if(index==4)
					sprintf(remoteip, "%s", token); /*gateway ip*/			
				if(index > 4){
					for(x=0;x<5;x++){
						if(dns_server[x][0] != '\0'){
							if(!strcmp(dns_server[x], token)){
								dns_found = 1; 
								break;
							}
						}
					}
					if(dns_found ==0){
						for(x=0;x<5;x++){
							if(dns_server[x][0] == '\0'){
								sprintf(dns_server[x], "%s", token);
								break;
							}
						}
					}
				}
			}
			index++;
			token = strtok_r(NULL, " ", &savestr1);
		}while(token !=NULL);  
		
#if 1//AVOID_CONFLICTION_IP
		/*if br0 get ip need to check*/
 		if(strcmp(interface, "br0")){
			ret = avoid_confliction_ip(wanip,mask);
		}
#endif

		RunSystemCmd(NULL_FILE, "ifconfig", interface, wanip, "netmask", mask, NULL_STR);	
#ifdef SUPPORT_ZIONCOM_RUSSIA
		if(strcmp(interface, "br0"))
			setFirewallIptablesRules(-1, NULL);
#endif
#if defined(CONFIG_DYNAMIC_WAN_IP)
		if(wan_type != PPTP && wan_type != L2TP) {
#endif
		RunSystemCmd(NULL_FILE, "route", "del", "default", NULL_STR);
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", remoteip, "dev", interface, NULL_STR);
//		printf("%s(%d): wan_type=%d,dns_mode=%d\n",__FUNCTION__,__LINE__, wan_type,dns_mode);//Added for test

#ifdef TR181_SUPPORT
		if(dnsEnable==1)
#endif
		{
		if(dns_mode==1){
			start_dns_relay();
		}else{
RunSystemCmd(NULL_FILE, "killall", "dnrd", NULL_STR);
			if(isFileExist(DNRD_PID_FILE)){
				unlink(DNRD_PID_FILE);
			}
			apmib_get( MIB_DOMAIN_NAME,  (void *)domanin_name);
						
			getInAddr("br0", IP_ADDR_T, (void *)&lanaddr);
			strtmp = inet_ntoa(lanaddr);
			sprintf((char *)dynip, "%s",strtmp); 						
			
			RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/hosts", NULL_STR);
			memset(cmdBuffer, 0x00, sizeof(cmdBuffer));
			
			if(domanin_name[0])
			{
				sprintf((char *)cmdBuffer,"%s\\%s%s%s%s", dynip, domanin_name, "AP.com|",domanin_name, "AP.net");				
			}
			else
			{
				sprintf((char *)cmdBuffer,"%s\\%s%s%s%s", dynip, "realtek", "AP.com|","realtek", "AP.net");
			}
			RunSystemCmd("/etc/hosts", "echo",cmdBuffer,NULL_STR);
#ifdef TR181_SUPPORT		
			for(x=0; x<3; x++)
			{
				if(dns_server[x][0] == '\0')
					continue;
				
				y = x+1;
				*((char*)entry)=(char)y;
				if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
				{
					printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
					return;
				}
				memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
				entry[1].index = x;
				entry[1].enable = 1;
				entry[1].status = 1;
				strcpy(entry[1].ipAddr, dns_server[x]);
//				strcpy(entry[1].interface, interface);
				entry[1].type = 1; //DHCPv4
				if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
				{
					printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
					return;
				}
			}		
#endif

			cmd_opt[cmd_cnt++]="dnrd";
			cmd_opt[cmd_cnt++]="--cache=off";
			for(x=0;x<5;x++){
				if(dns_server[x][0] != '\0'){
//#ifdef TR181_SUPPORT
#if 0
					sprintf(line, "Device.DNS.Client.Server.{%d}.Enable", x);
					if(tr181_ipv6_get(line, (void*)&value) == -1)
					{
						printf("get %s fail!\n", line);
						return;
					}
					if(value != 1)
						continue;
#endif
					cmd_opt[cmd_cnt++]="-s";
					cmd_opt[cmd_cnt++]=&dns_server[x][0];
					sprintf(line,"nameserver %s\n", dns_server[x]);
					if(x==0)
						write_line_to_file(RESOLV_CONF, 1, line);
					else
						write_line_to_file(RESOLV_CONF, 2, line);
				}
			}
			cmd_opt[cmd_cnt++] = 0;
			//for (x=0; x<cmd_cnt;x++)
			//	printf("cmd index=%d, opt=%s \n", x, cmd_opt[x]);
 		if(strcmp(interface, "br0")){
			DoCmd(cmd_opt, NULL_FILE);
		}
			
		}
		}
#ifdef CONFIG_POCKET_AP_SUPPORT
#else
	 if(strcmp(interface, "br0")){	
		setFirewallIptablesRules(0, NULL);
	 }
     else
     {
        unsigned char restart_iapp[100] = {0};
        FILE *fp;
        if(isFileExist(RESTART_IAPP)){
            fp= fopen(RESTART_IAPP, "r");
            if (!fp) {
                printf("can not open /var/restart_iapp\n");
                return;
            }
            fgets(restart_iapp, sizeof(restart_iapp), fp);
            fclose(fp);
            system(restart_iapp);
        }
     }
#endif	//CONFIG_POCKET_AP_SUPPORT
#if defined(CONFIG_DYNAMIC_WAN_IP)
	}
#endif
#if defined(CONFIG_DYNAMIC_WAN_IP)
	if(wan_type == PPTP || wan_type == L2TP){

#ifdef TR181_SUPPORT
		if(dnsEnable==1)
#endif
		{
		if(dns_mode==1)
			start_dns_relay();
		else
		{
			for(x=0;x<5;x++){
				if(dns_server[x][0] != '\0'){
					sprintf(line,"nameserver %s\n", dns_server[x]);
					if(x==0){
						write_line_to_file(RESOLV_CONF, 1, line);
		//						write_line_to_file(DHCP_RESOLV_FILE, 1, line);
						
					}else{
						write_line_to_file(RESOLV_CONF, 2, line);
		//						write_line_to_file(DHCP_RESOLV_FILE, 2, line);
					}
				}
			}
		}
		}
		
		RunSystemCmd(NULL_FILE, "route", "del", "default", "dev", interface, NULL_STR);
#if defined(CONFIG_GET_SERVER_IP_BY_DOMAIN)	
		//set tmp default gw for get ip from domain
		sprintf(tmp_default_gw, "%s", remoteip);
		sprintf(tmp_wan_if, "%s", interface);
#endif
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", remoteip, "dev", interface, NULL_STR); //redundant, but safe
		//printf("%s:%d route add -net default gw %s dev %s\n",__FUNCTION__,__LINE__,remoteip,interface);
		if(isFileExist(TEMP_WAN_CHECK) && isFileExist(TEMP_WAN_DHCP_INFO)){
			if(wan_type == PPTP){				
#if defined(CONFIG_GET_SERVER_IP_BY_DOMAIN)	
					unsigned char pptp_server_domain[32];
					struct in_addr server_ip;
					int enable_pptp_server_domain=0;
					//printf("%s:%d pptpServerDomain=%s\n",__FUNCTION__,__LINE__,pptp_server_domain);
				
					apmib_get(MIB_PPTP_GET_SERV_BY_DOMAIN,(void*)&enable_pptp_server_domain);
					if(enable_pptp_server_domain)
					{	
						apmib_get(MIB_PPTP_SERVER_DOMAIN, pptp_server_domain);
						//printf("%s:%d pptpServerDomain=%s\n",__FUNCTION__,__LINE__,pptp_server_domain);
						if(translate_domain_to_ip(pptp_server_domain, &server_ip) == 0)
						{			
							//printf("%s:%d server_ip=%s\n",__FUNCTION__,__LINE__,inet_ntoa(server_ip));
						//	inet_aton("192.168.2.200",&server_ip);
							apmib_set(MIB_PPTP_SERVER_IP_ADDR, (void *)&server_ip);
							apmib_update(CURRENT_SETTING);
						}else
						{
							printf("can't get pptpServerDomain:%s 's IP",pptp_server_domain);
							return 0;
						}
					}
#endif
				apmib_get(MIB_PPTP_SERVER_IP_ADDR,	(void *)tmp_buf);
				strtmp= inet_ntoa(*((struct in_addr *)tmp_buf));
				sprintf(ServerIp, "%s", strtmp);
				serverAddr=((struct in_addr *)tmp_buf)->s_addr;
				
				inet_aton(wanip, &tmpInAddr);
				wanIpAddr=tmpInAddr.s_addr;

				inet_aton(mask, &tmpInAddr);
				maskAddr=tmpInAddr.s_addr;

				inet_aton(remoteip, &tmpInAddr);
				remoteIpAddr=tmpInAddr.s_addr;

				if((serverAddr & maskAddr) != (wanIpAddr & maskAddr)){
					//Patch for our router under another router to dial up pptp
					//let pptp pkts via pptp default gateway
					netAddr = (serverAddr & maskAddr);
					((struct in_addr *)tmp_buf)->s_addr=netAddr;
					strtmp= inet_ntoa(*((struct in_addr *)tmp_buf));
					sprintf(netIp, "%s", strtmp);
					RunSystemCmd(NULL_FILE, "route", "add", "-net", netIp, "netmask", mask,"gw", remoteip,NULL_STR);
				}
			}
			else if(wan_type == L2TP){			
#if defined(CONFIG_GET_SERVER_IP_BY_DOMAIN)	
				unsigned char l2tp_server_domain[32];
				struct in_addr server_ip;
				int enable_l2tp_server_domain=0;
			
				apmib_get(MIB_L2TP_GET_SERV_BY_DOMAIN,(void*)&enable_l2tp_server_domain);
				if(enable_l2tp_server_domain)
				{	
					apmib_get(MIB_L2TP_SERVER_DOMAIN, l2tp_server_domain);
					//printf("%s:%d l2tpServerDomain=%s\n",__FUNCTION__,__LINE__,l2tp_server_domain);
					if(translate_domain_to_ip(l2tp_server_domain, &server_ip) == 0)
					{
						//printf("%s:%d server_ip=%s\n",__FUNCTION__,__LINE__,inet_ntoa(server_ip));
						//inet_aton("192.168.2.200",&server_ip);
						apmib_set(MIB_L2TP_SERVER_IP_ADDR, (void *)&server_ip);
						apmib_update(CURRENT_SETTING);
					}else
					{
						printf("can't get l2tpServerDomain:%s 's IP",l2tp_server_domain);
						return 0;
					}
				}
#endif
				apmib_get(MIB_L2TP_SERVER_IP_ADDR,	(void *)tmp_buf);
				strtmp= inet_ntoa(*((struct in_addr *)tmp_buf));
				sprintf(ServerIp, "%s", strtmp);
				serverAddr=((struct in_addr *)tmp_buf)->s_addr;
				
				inet_aton(wanip, &tmpInAddr);
				wanIpAddr=tmpInAddr.s_addr;

				inet_aton(mask, &tmpInAddr);
				maskAddr=tmpInAddr.s_addr;

				inet_aton(remoteip, &tmpInAddr);
				remoteIpAddr=tmpInAddr.s_addr;

				if((serverAddr & maskAddr) != (wanIpAddr & maskAddr)){
					//Patch for our router under another router to dial up pptp
					//let l2tp pkts via pptp default gateway
					netAddr = (serverAddr & maskAddr);
					((struct in_addr *)tmp_buf)->s_addr=netAddr;
					strtmp= inet_ntoa(*((struct in_addr *)tmp_buf));
					sprintf(netIp, "%s", strtmp);
					RunSystemCmd(NULL_FILE, "route", "add", "-net", netIp, "netmask", mask,"gw", remoteip,NULL_STR);
				}
			}
			
			unlink(TEMP_WAN_CHECK);
			unlink(TEMP_WAN_DHCP_INFO);
		}

		if(isFileExist(PPP_CONNECT_FILE)){
			unlink(PPP_CONNECT_FILE);
		}
		//system("killall -9 udhcpc 2>/dev/null");	
		if(wan_type == PPTP){
			set_pptp(opmode, interface, "br0", wisp_wan_id, 1);
		}
		if(wan_type == L2TP){
			set_l2tp(opmode, interface, "br0", wisp_wan_id, 1);
		}
		return;
	}
#endif

	}
	else if(lan_type == 1 && strcmp(interface, "br0")==0){
		
		for (x=0;x<5;x++){
			memset(dns_server[x], '\0', 32);
		}
		token=NULL;
		savestr1=NULL;	     
		sprintf(arg_buff, "%s", option);
	
		token = strtok_r(arg_buff," ", &savestr1);
		index=1;
		do{
			dns_found=0;
			if (token == NULL){/*check if the first arg is NULL*/
				break;
			}else{   
				if(index==2)
					sprintf(wanip, "%s", token); /*wan ip address */
				if(index==3)
					sprintf(mask, "%s", token); /*subnet mask*/
				if(index==4)
					sprintf(remoteip, "%s", token); /*gateway ip*/			
				if(index > 4){
					for(x=0;x<5;x++){
						if(dns_server[x][0] != '\0'){
							if(!strcmp(dns_server[x], token)){
								dns_found = 1; 
								break;
							}
						}
					}
					if(dns_found ==0){
						for(x=0;x<5;x++){
							if(dns_server[x][0] == '\0'){
								sprintf(dns_server[x], "%s", token);
								break;
							}
						}
					}
				}
			}
			index++;
			token = strtok_r(NULL, " ", &savestr1);
		}while(token !=NULL);  
		
		RunSystemCmd(NULL_FILE, "ifconfig", interface, wanip, "netmask", mask, NULL_STR);	
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", remoteip, "dev", interface, NULL_STR);
//		printf("%s(%d): wan_type=%d,dns_mode=%d\n",__FUNCTION__,__LINE__, wan_type,dns_mode);//Added for test
#ifdef TR181_SUPPORT
		if(dnsEnable==1)
#endif
		{
		if(dns_mode==1){
			start_dns_relay();
		}else{
#ifdef TR181_SUPPORT
			for(x=0; x<3; x++)
			{
				if(dns_server[x][0] == '\0')
					continue;
				
				y = x+1;
				*((char*)entry)=(char)y;
				if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
				{
					printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
					return;
				}
				memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
				entry[1].index = x;
				entry[1].enable = 1;
				entry[1].status = 1;
				strcpy(entry[1].ipAddr, dns_server[x]);
//				strcpy(entry[1].interface, interface);
				entry[1].type = 1; //DHCPv4
				if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
				{
					printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
					return;
				}
			}		
#endif
			cmd_opt[cmd_cnt++]="dnrd";
			cmd_opt[cmd_cnt++]="--cache=off";
			for(x=0;x<5;x++){
				if(dns_server[x][0] != '\0'){
//#ifdef TR181_SUPPORT
#if 0
					sprintf(line, "Device.DNS.Client.Server.{%d}.Enable", x);
					if(tr181_ipv6_get(line, (void*)&value) == -1)
					{
						printf("get %s fail!\n", line);
						return;
					}
					if(value != 1)
						continue;
#endif
					cmd_opt[cmd_cnt++]="-s";
					cmd_opt[cmd_cnt++]=&dns_server[x][0];
					sprintf(line,"nameserver %s\n", dns_server[x]);
					if(x==0)
						write_line_to_file(RESOLV_CONF, 1, line);
					else
						write_line_to_file(RESOLV_CONF, 2, line);
				}
			}
			cmd_opt[cmd_cnt++] = 0;
			DoCmd(cmd_opt, NULL_FILE);

		}
		}
		if(op_mode!=1)
		{
			start_igmpproxy(interface, "br0");
		}
	}
#ifdef CONFIG_POCKET_AP_SUPPORT
#else

	if(strcmp(interface, "br0")){
		
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
		if((ret == 1) && (op_mode == GATEWAY_MODE)) //AP/client mode won't call this function
#else
		if(ret == 1)
#endif			
		{
			if(op_mode!=WISP_MODE)
			{
				system("ifconfig wlan0 down");
				apmib_get(MIB_WLAN_BAND2G5G_SELECT,(void *)&intValue);
				if(intValue == BANDMODEBOTH)
				{
					system("ifconfig wlan1 down");
				}
			}
			
#if !defined(CONFIG_POCKET_ROUTER_SUPPORT)
			//when op_mode== GATEWAY_MODE for pocket AP, there isn't interface eth0
			system("ifconfig eth0 down");
#endif		
			sleep(10);
			
#if !defined(CONFIG_POCKET_ROUTER_SUPPORT)
			system("ifconfig eth0 up");
#endif			
			if(op_mode!=WISP_MODE)
			{
				system("ifconfig wlan0 up");
				if(intValue == BANDMODEBOTH)
				{
					system("ifconfig wlan1 up");
				}
			}
		}

		printf("WAN Connected\n");
		start_ntp();
		start_ddns();
#ifdef MULTI_PPPOE
		if(!strcmp(interface,"ppp0"))
			start_igmpproxy(interface, "br0");
#else 
		start_igmpproxy(interface, "br0");
#endif
	}
#endif	//CONFIG_POCKET_AP_SUPPORT
#if defined(ROUTE_SUPPORT)
	if(strcmp(interface, "br0")){	
	del_routing();
	start_routing(interface);
	}
#endif

}


#ifdef MULTI_PPPOE
void wan_disconnect(char *option , char *conncetOrder)
#else
void wan_disconnect(char *option)
#endif
{
	int intValue=0;
	int wan_type=0;
	int Last_WAN_Mode=0;
	FILE *fp;
	
#ifdef CONFIG_IPV6
	char strPID[10];
	int pid=-1;
#endif
	//printf("WAN Disconnect option=%s\n", option);//Added for test

	apmib_get( MIB_WAN_DHCP,(void *)&wan_type);

#ifdef MULTI_PPPOE
	int connnect_num,IsRuningNum = 0;
	char cmd[50];
	//when one pppoe timeout,execute disconnect.sh ,just return to let it go on connecting
	//only if all pppoe disconnect			
	//if(getRuningNum(wan_type,conncetOrder) >=1)

	if(wan_type == PPPOE && strcmp(conncetOrder,"NOMULPPPOE") && strcmp(conncetOrder,""))
	{
		
		FILE *pF;
		apmib_get(MIB_PPP_CONNECT_COUNT, (void *)&connnect_num);	
		if(connnect_num >= 1){
			if(isFileExist("/etc/ppp/link")){
				if(strcmp(conncetOrder,"1"))
					++IsRuningNum;
				else
					system("rm /etc/ppp/link >/dev/null 2>&1");
			}
		}	
				
		if(connnect_num >= 2){
			if(isFileExist("/etc/ppp/link2")){
				if(strcmp(conncetOrder,"2"))
					++IsRuningNum;
				else
					system("rm /etc/ppp/link2 >/dev/null 2>&1");			
			}		
		}		
		if(connnect_num >= 3){
			if(isFileExist("/etc/ppp/link3")){
				if(strcmp(conncetOrder,"3"))
					++IsRuningNum;
				else
					system("rm /etc/ppp/link3 >/dev/null 2>&1");			
			}	
		}
	
		if(connnect_num >= 4){
			if(isFileExist("/etc/ppp/link4")){
				if(strcmp(conncetOrder,"4"))
					++IsRuningNum;
				else
					system("rm /etc/ppp/link4 >/dev/null 2>&1");			
			}	
		}
	
		if((pF = fopen("/etc/ppp/ppp_order_info","r+"))!= NULL){				
			FILE* ftmp=fopen("/etc/ppp/tmp","wt");			
			int match,order;
			char name[10];
            if(ftmp == NULL)
            {
                   printf("can't open the file \n");
                   return ;
            }
			sscanf(conncetOrder,"%d",&match);			
			while( fscanf(pF,"%d--%s",&order,name) > 0 )
			{			    
			   if(match != order){
			   	
				  fprintf(ftmp,"%d--%s\n",order,name);				  
			   }else{			   	 
			   	
				   //clear the iptables rule 
				   char flushcmd[100];
				   char buf[100];
				   FILE *pRule;			   
				   //clear filter chain
				   sprintf(buf,"iptables -t filter -S | grep %s | cut -d ' ' -f 2- > /etc/ppp/filterrule",
					   name);
				   system(buf);
				   if((pRule = fopen("/etc/ppp/filterrule","r+"))!= NULL){
					   while(fgets(buf, 100, pRule)){
						   sprintf(flushcmd,"iptables -t filter -D %s >/dev/null 2>&1",buf);
						   system(flushcmd);
					   }
					   fclose(pRule);	
				   }
				   system("rm /etc/ppp/filterrule >/dev/null 2>&1");
				   //clear nat chain		   
				   sprintf(buf,"iptables -t nat -S | grep %s | cut -d ' ' -f 2- > /etc/ppp/natrule",
					   name);
				   system(buf);
				   if((pRule = fopen("/etc/ppp/natrule","r+"))!= NULL){
					   while(fgets(buf, 100, pRule)){
						   sprintf(flushcmd,"iptables -t nat -D %s >/dev/null 2>&1",buf);
						   system(flushcmd);
					   }
					   fclose(pRule); 
				   }
				   system("rm /etc/ppp/natrule >/dev/null 2>&1");				   
				   //clear mangle chain			   
				   sprintf(buf,"iptables -t mangle -S | grep %s | cut -d ' ' -f 2- > /etc/ppp/manglerule",
					   name);
				   system(buf); 				   
				   if((pRule = fopen("/etc/ppp/manglerule","r+"))!= NULL){
					   while(fgets(buf, 100, pRule)){
						   sprintf(flushcmd,"iptables -t nat -D %s >/dev/null 2>&1",buf);
						   system(flushcmd);
					   }
					   fclose(pRule); 
				   }
				   system("rm /etc/ppp/manglerule >/dev/null 2>&1");				
					
				   //clear ip policy rule				   
				   sprintf(buf,"/etc/ppp/%s.cmd",name);
				   if((pRule = fopen(buf,"r+")) != NULL){
				   		while(fgets(buf, 100, pRule)){
						   system(buf);
					   }						
						fclose(pRule);
				   }				   
				   sprintf(flushcmd,"rm %s >/dev/null 2>&1",buf);
				   system(flushcmd);
				   
			   }			   
			}			
			fclose(ftmp);
			fclose(pF);					
			system("cp /etc/ppp/tmp /etc/ppp/ppp_order_info >/dev/null 2>&1");			
			system("rm /etc/ppp/tmp >/dev/null 2>&1");	
		}		
		
		if(IsRuningNum >=1 )
		{
			return;
		}			
	    system("ip rule del table 100 >/dev/null 2>&1"); 
	    system("ip route del table 100 >/dev/null 2>&1");		
		system("rm /etc/ppp/ppp_order_info >/dev/null 2>&1");
		system("rm /etc/ppp/hasPppoedevice >/dev/null 2>&1");
		system("rm /etc/ppp/AC_Names >/dev/null 2>&1");
		system("rm /etc/ppp/SubInfos >/dev/null 2>&1");	
		return ;
	}
	
#endif
	if(isFileExist(LAST_WAN_TYPE_FILE)){
		fp= fopen(LAST_WAN_TYPE_FILE, "r");
		if (!fp) {
	        	printf("can not /var/system/last_wan\n");
			return; 
	   	}
		fscanf(fp,"%d",&Last_WAN_Mode);
		fclose(fp);
	}
	RunSystemCmd("/var/disc", "echo", "enter", NULL_STR); 
	
//	apmib_get(MIB_WAN_DHCP,(void *)&wan_type);
	
	RunSystemCmd(NULL_FILE, "killall", "-15", "routed", NULL_STR); 
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "ntp_inet", NULL_STR);
	if(isFileExist("/var/ntp_run")){
		unlink("/var/ntp_run");
	} 
	
	RunSystemCmd(NULL_FILE, "killall", "-15", "ddns_inet", NULL_STR); 
	RunSystemCmd(NULL_FILE, "killall", "-9", "updatedd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "ntpclient", NULL_STR);
	//RunSystemCmd("/proc/pptp_src_ip", "echo", "0 0", NULL_STR);
	
	#if	defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT) || defined(CONFIG_RTL_ULINKER)
		if(!strcmp(option, "all")){
	RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
	if(isFileExist(DNRD_PID_FILE)){
		unlink(DNRD_PID_FILE);
	}
	}
#if 0	//it is for pocket AP wan connect? but it has done the related operations in wan_connect() when IP conflict 
	else if(!strcmp(option, "dhcpc"))
	{
		unsigned char dynip[32]={0};
		struct in_addr	intaddr;
		unsigned char cmdBuffer[100]={0};
		unsigned char tmpBuff[200]={0};
		unsigned char domain_name[32]={0};
		
		if ( getInAddr("eth1", IP_ADDR_T, (void *)&intaddr ) )
			sprintf(dynip,"%s",inet_ntoa(intaddr));
		else
			sprintf(dynip,"%s","0.0.0.0");
			
		if(strcmp(dynip, "0.0.0.0") != 0) //do nothing at first time
		{
			system("echo \"WAN Disconnected\n\" > var/wanlink");
			system("killall -9 dnrd 2> /dev/null");
			system("rm -f /var/hosts 2> /dev/null");
			
			if ( getInAddr("br0", IP_ADDR_T, (void *)&intaddr ) )
				sprintf(dynip,"%s",inet_ntoa(intaddr));
			else
				sprintf(dynip,"%s","0.0.0.0");
			
			apmib_get( MIB_DOMAIN_NAME,  (void *)domain_name);
			sprintf(cmdBuffer,"%s\\%s%s%s%s", dynip, domain_name, "AP.com|",domain_name, "AP.net");
			//RunSystemCmd("/etc/hosts", "echo",cmdBuffer,NULL_STR);
			sprintf(tmpBuff, "echo \"%s\" > /etc/hosts", cmdBuffer);
			system(tmpBuff);
			
			system("ifconfig eth0 down");
			system("ifconfig wlan0 down");
	
			sleep(10);
			
			system("ifconfig eth0 up");
			system("ifconfig wlan0 up");
			
			system("dnrd --cache=off -s 168.95.1.1");
		}
	}
#endif
	else
	{
			if(isFileExist(PPPLINKFILE)){ //Last state, ppp0 is not connected, we do not kill dnrd
				RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
				if(isFileExist(DNRD_PID_FILE)){
					unlink(DNRD_PID_FILE);
				}
			}
		}
	#else

	RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
	if(isFileExist(DNRD_PID_FILE)){
		unlink(DNRD_PID_FILE);
	}
	#endif
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "igmpproxy", NULL_STR);
	if(isFileExist(IGMPPROXY_PID_FILE)){
		unlink(IGMPPROXY_PID_FILE);
	}

#ifdef CONFIG_IPV6
	if(isFileExist(DHCP6S_PID_FILE)) {
		pid=getPid_fromFile(DHCP6S_PID_FILE);
		if(pid){
			sprintf(strPID, "%d", pid);
			RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);							
		}
		unlink(DHCP6S_PID_FILE);
	}
	
	if(isFileExist(DHCP6C_PID_FILE)) {
		pid=getPid_fromFile(DHCP6C_PID_FILE);
		if(pid){
			sprintf(strPID, "%d", pid);
			RunSystemCmd(NULL_FILE, "kill", "-16", strPID, NULL_STR);/*inform dhcp server write lease table to file*/
			sleep(1);
			RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);						
		}
		unlink(DHCP6C_PID_FILE);
	}
		
	if(isFileExist(DNSV6_PID_FILE)) {
		pid=getPid_fromFile(DNSV6_PID_FILE);
		if(pid){
			sprintf(strPID, "%d", pid);
			RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);						
		}
		unlink(DNSV6_PID_FILE);
	}
	
	if(isFileExist(RADVD_PID_FILE)) {
		pid=getPid_fromFile(RADVD_PID_FILE);
		if(pid){
			sprintf(strPID, "%d", pid);
			RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);						
		}
		unlink(RADVD_PID_FILE);
	}
	
	if(isFileExist(ECMH_PID_FILE)) {
		pid=getPid_fromFile(ECMH_PID_FILE);
		if(pid){
			sprintf(strPID, "%d", pid);
			RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);						
		}
		unlink(ECMH_PID_FILE);
	}	
#endif

#ifdef CONFIG_RTK_VOIP
	RunSystemCmd(NULL_FILE, "killall", "-9", "fwupdate", NULL_STR);
	if(isFileExist(FWUPDATE_PID_FILE)){
		unlink(FWUPDATE_PID_FILE);
	}
#endif
	RunSystemCmd(PROC_BR_MCASTFASTFWD, "echo", "1,1", NULL_STR);
	//printf("Last_WAN_Mode==%d\n", Last_WAN_Mode);//Added for test
	if(!strcmp(option, "all"))
		RunSystemCmd(NULL_FILE, "killall", "-9", "ppp_inet", NULL_STR); 
	//if(Last_WAN_Mode==PPPOE)
	if(1)
	{
		RunSystemCmd(NULL_FILE, "killall", "-15", "pppd", NULL_STR);
	}else{
		RunSystemCmd(NULL_FILE, "killall", "-9", "pppd", NULL_STR);
	}
	
	if(wan_type==L2TP)
	{
		system("echo 0 >/proc/fast_l2tp");
		system("echo 1 >/proc/fast_l2tp");
	}
	sleep(3);

	if((wan_type!=L2TP)&&(Last_WAN_Mode==L2TP)){
	RunSystemCmd(NULL_FILE, "killall", "-9", "l2tpd", NULL_STR);
	}
	RunSystemCmd(NULL_FILE, "killall", "-9", "pptp", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "pppoe", NULL_STR);
	if(isFileExist(PPPD_PID_FILE)){
		unlink(PPPD_PID_FILE);
	} 

	if(wan_type==L2TP && !strcmp(option, "option") && isFileExist(PPPLINKFILE)){
		apmib_get( MIB_L2TP_CONNECTION_TYPE, (void *)&intValue);
		if(intValue==1){
			if(isFileExist("/var/disc_l2tp")){
				system("echo\"d client\" > /var/run/l2tp-control &");
				system("echo \"l2tpdisc\" > /var/disc_l2tp");
			}
		}
	}
/*clean pptp_info in fastpptp*/
	if(wan_type==PPTP)
		system("echo 1 > /proc/fast_pptp");

	if(isFileExist(FIRSTDDNS)){
	 	unlink(FIRSTDDNS);
	}

	if(!strcmp(option, "option") && isFileExist(PPPLINKFILE)){
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/first", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstpptp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstl2tp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstdemand", NULL_STR);
	}
	if(isFileExist(PPPLINKFILE)){
	 	unlink(PPPLINKFILE);
	}
	/*in PPPOE and PPTP mode do this in pppd , not here !!*/
	//if(wan_type !=PPPOE || strcmp(option, "option"))
	{
		if(isFileExist(PPP_CONNECT_FILE)){
	 		unlink(PPP_CONNECT_FILE);
		}
	}
	if(wan_type==PPTP){
		apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&intValue);
		if(intValue==1){
			RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "3", NULL_STR);
		}else{
			RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "0", NULL_STR);
		}
	}
	RunSystemCmd(NULL_FILE, "rm", "-f", "/var/disc", NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", "-f", "/var/disc_l2tp", NULL_STR);
	
}

#ifdef CONFIG_IPV6
void checkDhcp6pd();
void radvd_reconfig();
void checkDnsv6();

struct dhcp6_pd_t {		/* IA_PA */
	uint32 pltime;
	uint32 vltime;
	uint16 addr6[8];
	int plen;
	uint8 flag;
};

struct dhcp6_pd_t dhcp6_pd;
char dns_addr6[64];
//note: set prefix/64 to br0 
void checkDhcp6pd()
{
	FILE *fp=NULL;
	uint32 pltime=0;
	uint32 vltime=0;
	char addr6[64]={0};
	uint8	prefix[16]={0};
	int plen=0;
	//printf("%s:%d\n",__FUNCTION__,__LINE__);

	if(access("/var/dhcp6pd_need_update",0)<0)
		return;
	//printf("%s:%d\n",__FUNCTION__,__LINE__);
	fp=fopen(DHCP6PD_CONF_FILE, "r");
	if(fp==NULL)
		return;
	//printf("%s:%d\n",__FUNCTION__,__LINE__);
	fscanf(fp,"%s %d %u %u",addr6,&plen,&pltime,&vltime);
	fclose(fp);
	sscanf(addr6,"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
			&prefix[0],&prefix[1],&prefix[2],&prefix[3],
			&prefix[4],&prefix[5],&prefix[6],&prefix[7],
			&prefix[8],&prefix[9],&prefix[10],&prefix[11],
			&prefix[12],&prefix[13],&prefix[14],&prefix[15]);
		bzero(&dhcp6_pd,sizeof(dhcp6_pd));
		memcpy(dhcp6_pd.addr6,prefix,16);
		dhcp6_pd.plen=plen;
		dhcp6_pd.pltime=pltime;
		dhcp6_pd.vltime=vltime;
		dhcp6_pd.flag=1;
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		radvd_reconfig();
		system("rm /var/dhcp6pd_need_update");
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		return;
}

void checkDnsv6()
{
		dhcp6sCfgParam_t dhcp6sCfgParam;
		char dns_server[64];
		int pid=-1;
		FILE *fp;
		char prefix[64];
		int flag=1;
		char serverName[64]={0};

		if(access("/var/dhcp6_dns_need_update",0)<0)
			return;
		if ( !apmib_get(MIB_IPV6_DHCPV6S_PARAM,(void *)&dhcp6sCfgParam)){
			fprintf(stderr,"get MIB_IPV6_DHCPV6S_PARAM failed\n");
			return;  
		}

		fp=fopen(DNSV6_ADDR_FILE,"r");
		if(fp==NULL)
			return;
		memset(dns_server,0,64);
		fscanf(fp,"%s %s",serverName,dns_server);
		fclose(fp);
		
		
		fp=fopen(DHCP6S_CONF_FILE,"w+");
		if(fp==NULL)
			return;
		if(dhcp6sCfgParam.enabled){	
			bzero(&dns_addr6,sizeof(dns_addr6));
			strcpy(dns_addr6,dns_server);		
			fprintf(fp, "option domain-name-servers %s;\n", dns_server);
			fprintf(fp, "interface %s {\n", dhcp6sCfgParam.interfaceNameds);
			fprintf(fp, "  address-pool pool1 3600;\n");
			fprintf(fp, "};\n");
			fprintf(fp, "pool pool1 {\n");
			fprintf(fp, "  range %s to %s ;\n", dhcp6sCfgParam.addr6PoolS, dhcp6sCfgParam.addr6PoolE);
			fprintf(fp, "};\n");	
			flag=1;
		}
		else{
			if(dhcp6_pd.flag){
				strcpy(dns_addr6,dns_server);
				fprintf(fp, "option domain-name-servers %s;\n", dns_server);
				fprintf(fp, "interface br0 {\n");
				fprintf(fp, "  address-pool pool1 3600;\n");
				fprintf(fp, "};\n");
				fprintf(fp, "pool pool1 {\n");
				sprintf(dhcp6sCfgParam.addr6PoolS,"%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
				dhcp6_pd.addr6[0],dhcp6_pd.addr6[1],
				dhcp6_pd.addr6[2],dhcp6_pd.addr6[3],
				dhcp6_pd.addr6[4],dhcp6_pd.addr6[5],
				dhcp6_pd.addr6[6],dhcp6_pd.addr6[7]+1);
				sprintf(dhcp6sCfgParam.addr6PoolE,"%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
				dhcp6_pd.addr6[0],dhcp6_pd.addr6[1],
				dhcp6_pd.addr6[2],dhcp6_pd.addr6[3],
				dhcp6_pd.addr6[4],dhcp6_pd.addr6[5],
				dhcp6_pd.addr6[6],dhcp6_pd.addr6[7]+254);			
				fprintf(fp, "  range %s to %s ;\n", dhcp6sCfgParam.addr6PoolS, dhcp6sCfgParam.addr6PoolE);
				fprintf(fp, "};\n");
				flag=1;
			}
		}
		fclose(fp);
		
		/*start daemon*/
		if(flag){
			if(isFileExist(DHCP6S_PID_FILE)) {
				if ((fp = fopen(DHCP6S_PID_FILE, "r")) != NULL) {
					fscanf(fp, "%d\n", &pid);
					fclose(fp);
					kill(pid,1);	/*sighup radvd to reload config file*/
				}					
			}
			else{
				system("dhcp6s br0 2> /dev/null");
			}
		}
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		system("rm /var/dhcp6_dns_need_update;");
		return;
}

void radvd_reconfig()
{
		radvdCfgParam_t radvdCfgParam;
		FILE *fp;
		char tmpBuf[256];
		unsigned short tmpNum[8];
		int pid;
		if ( !apmib_get(MIB_IPV6_RADVD_PARAM,(void *)&radvdCfgParam)){
			fprintf(stderr,"get MIB_IPV6_RADVD_PARAM failed\n");
			return;  
		}

		fp=fopen(RADVD_CONF_FILE,"w+");
		if(fp==NULL){
			fprintf(stderr, "Create %s file error!\n", RADVD_CONF_FILE);
			return;
		}
		
		if(radvdCfgParam.enabled){
			fprintf(fp, "interface %s\n", radvdCfgParam.interface.Name);
			fprintf(fp, "{\n");
			fprintf(fp, "AdvSendAdvert on;\n");			
			fprintf(fp, "MaxRtrAdvInterval %d;\n", radvdCfgParam.interface.MaxRtrAdvInterval);
			fprintf(fp, "MinRtrAdvInterval %d;\n", radvdCfgParam.interface.MinRtrAdvInterval);
			fprintf(fp, "MinDelayBetweenRAs %d;\n", radvdCfgParam.interface.MinDelayBetweenRAs);
			if(radvdCfgParam.interface.AdvManagedFlag > 0) {
				fprintf(fp, "AdvManagedFlag on;\n");					
			}
			if(radvdCfgParam.interface.AdvOtherConfigFlag > 0){
				fprintf(fp, "AdvOtherConfigFlag on;\n");				
			}
			fprintf(fp, "AdvLinkMTU %d;\n", radvdCfgParam.interface.AdvLinkMTU);
			fprintf(fp, "AdvReachableTime %u;\n", radvdCfgParam.interface.AdvReachableTime);
			fprintf(fp, "AdvRetransTimer %u;\n", radvdCfgParam.interface.AdvRetransTimer);
			fprintf(fp, "AdvCurHopLimit %d;\n", radvdCfgParam.interface.AdvCurHopLimit);
			fprintf(fp, "AdvDefaultLifetime %d;\n", radvdCfgParam.interface.AdvDefaultLifetime);			
			fprintf(fp, "AdvDefaultPreference %s;\n", radvdCfgParam.interface.AdvDefaultPreference);		
			if(radvdCfgParam.interface.AdvSourceLLAddress > 0) {
				fprintf(fp, "AdvSourceLLAddress on;\n");				
			}		
			if(radvdCfgParam.interface.UnicastOnly > 0){
				fprintf(fp, "UnicastOnly on;\n");			
			}

			if(radvdCfgParam.interface.prefix[0].enabled > 0){
				memcpy(tmpNum,radvdCfgParam.interface.prefix[0].Prefix, sizeof(radvdCfgParam.interface.prefix[0].Prefix));
				sprintf(tmpBuf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", tmpNum[0], tmpNum[1], 
					tmpNum[2], tmpNum[3], tmpNum[4], tmpNum[5],tmpNum[6],tmpNum[7]);
				strcat(tmpBuf, "\0");
				fprintf(fp, "prefix %s/%d\n", tmpBuf, radvdCfgParam.interface.prefix[0].PrefixLen);				
				fprintf(fp, "{\n");				
				if(radvdCfgParam.interface.prefix[0].AdvOnLinkFlag > 0){
					fprintf(fp, "AdvOnLink on;\n");							
				}
				if(radvdCfgParam.interface.prefix[0].AdvAutonomousFlag > 0){
					fprintf(fp, "AdvAutonomous on;\n");					
				}
				fprintf(fp, "AdvValidLifetime %u;\n", radvdCfgParam.interface.prefix[0].AdvValidLifetime);
				fprintf(fp, "AdvPreferredLifetime %u;\n", radvdCfgParam.interface.prefix[0].AdvPreferredLifetime);
				
				if(radvdCfgParam.interface.prefix[0].AdvRouterAddr > 0){
					fprintf(fp, "AdvRouterAddr on;\n");							
				}
				if(radvdCfgParam.interface.prefix[0].if6to4[0]){
					fprintf(fp, "Base6to4Interface %s\n;", radvdCfgParam.interface.prefix[0].if6to4);									
				}
				fprintf(fp, "};\n");									
			}

			if(radvdCfgParam.interface.prefix[1].enabled > 0){
				memcpy(tmpNum,radvdCfgParam.interface.prefix[1].Prefix, sizeof(radvdCfgParam.interface.prefix[1].Prefix));
				sprintf(tmpBuf, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x", tmpNum[0], tmpNum[1], 
					tmpNum[2], tmpNum[3], tmpNum[4], tmpNum[5],tmpNum[6],tmpNum[7]);
				strcat(tmpBuf, "\0");
				fprintf(fp, "prefix %s/%d\n", tmpBuf, radvdCfgParam.interface.prefix[1].PrefixLen);				
				fprintf(fp, "{\n");				
				if(radvdCfgParam.interface.prefix[1].AdvOnLinkFlag > 0){
					fprintf(fp, "AdvOnLink on;\n");							
				}
				if(radvdCfgParam.interface.prefix[1].AdvAutonomousFlag > 0){
					fprintf(fp, "AdvAutonomous on;\n");					
				}
				fprintf(fp, "AdvValidLifetime %u;\n", radvdCfgParam.interface.prefix[1].AdvValidLifetime);
				fprintf(fp, "AdvPreferredLifetime %u;\n", radvdCfgParam.interface.prefix[1].AdvPreferredLifetime);
				
				if(radvdCfgParam.interface.prefix[1].AdvRouterAddr > 0){
					fprintf(fp, "AdvRouterAddr on;\n");							
				}
				if(radvdCfgParam.interface.prefix[1].if6to4[0]){
					fprintf(fp, "Base6to4Interface %s\n;", radvdCfgParam.interface.prefix[1].if6to4);									
				}
				fprintf(fp, "};\n");									
			}			
		}
		else{
			/*create radvd's configure file and set parameters to default value*/
			fprintf(fp, "interface %s\n","br0");
			fprintf(fp, "{\n");
			fprintf(fp, "AdvSendAdvert on;\n");			
			fprintf(fp, "MaxRtrAdvInterval 600;\n");
			fprintf(fp, "MinRtrAdvInterval 198;\n");
			fprintf(fp, "MinDelayBetweenRAs 3;\n");
			fprintf(fp, "AdvLinkMTU 1500;\n");
			fprintf(fp, "AdvCurHopLimit 64;\n");
			fprintf(fp, "AdvDefaultLifetime 1800;\n");			
			fprintf(fp, "AdvDefaultPreference medium;\n");			
		}
		/*add prefix information*/
		sprintf(tmpBuf,"%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d",
				dhcp6_pd.addr6[0],dhcp6_pd.addr6[1],
				dhcp6_pd.addr6[2],dhcp6_pd.addr6[3],
				dhcp6_pd.addr6[4],dhcp6_pd.addr6[5],
				dhcp6_pd.addr6[6],dhcp6_pd.addr6[7],
				64);
		fprintf(fp, "prefix %s\n", tmpBuf);				
		fprintf(fp, "{\n");	
		fprintf(fp, "	AdvOnLink on;\n");	
		fprintf(fp, "	AdvAutonomous on;\n");	
		fprintf(fp, "	AdvValidLifetime %u;\n", dhcp6_pd.vltime);
		fprintf(fp, "	AdvPreferredLifetime %u;\n", dhcp6_pd.pltime);
		fprintf(fp, "	AdvRouterAddr on;\n");
		fprintf(fp, "};\n");
			
		fprintf(fp, "};\n");
		fclose(fp);
		
		if(isFileExist(RADVD_PID_FILE)){
			if ((fp = fopen(RADVD_PID_FILE, "r")) != NULL) {
				fscanf(fp, "%d\n", &pid);
				fclose(fp);
				kill(pid,1);	/*sighup radvd to reload config file*/
			}		
		} 
		else{			
			system("radvd -C /var/radvd.conf 2> /dev/null"); 						
		}
		
		return;
}
#endif
/*write dns server ip address to resolv.conf file and start dnrd
* 
*/
void start_dns_relay(void)
{
	char tmpBuff1[32]={0}, tmpBuff2[32]={0}, tmpBuff3[32]={0};
	int intValue=0, cmd_cnt=0;
	char line_buffer[100]={0};
	char tmp1[32]={0}, tmp2[32]={0}, tmp3[32]={0};
	char *strtmp=NULL;
	char *cmd_opt[16];
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "dnrd", NULL_STR);
	apmib_get( MIB_DNS1,  (void *)tmpBuff1);
	apmib_get( MIB_DNS2,  (void *)tmpBuff2);
	apmib_get( MIB_DNS3,  (void *)tmpBuff3);
	
	if (memcmp(tmpBuff1, "\x0\x0\x0\x0", 4))
		intValue++;
	if (memcmp(tmpBuff2, "\x0\x0\x0\x0", 4))
		intValue++;
	if (memcmp(tmpBuff3, "\x0\x0\x0\x0", 4))
		intValue++;	

	cmd_opt[cmd_cnt++] = "dnrd";
	cmd_opt[cmd_cnt++] = "--cache=off";
		
#ifdef TR181_SUPPORT
		DNS_CLIENT_SERVER_T entry[2]={0};
		int x, y=0;
//		char interface[]="eth1";
		
	
		for(x=0; x<6; x++)
		{
			y = x+1;
			*((char*)entry)=(char)y;
			if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
			{
				printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
				return;
			}
			memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
			entry[1].index = x;
			entry[1].enable =0;
			entry[1].status = 0;
//			strcpy(entry[1].alias, "");
//			strcpy(entry[1].ipAddr, "");
//			strcpy(entry[1].interface, "");
//			entry[1].type = 0; //Unknown
			if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
			{
				printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
				return;
			}
		}
#endif

	if(intValue==1){
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff1));
		sprintf(tmp1,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n",strtmp);
#ifdef TR181_SUPPORT
		y = 4;
		*((char*)entry)=(char)y;
		if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
		{
			printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
		memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
		entry[1].index = 3;
		entry[1].enable = 1;
		entry[1].status = 1;
		strcpy(entry[1].ipAddr, strtmp);
//		strcpy(entry[1].interface, interface);
		entry[1].type = 5; //static
		if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
		{
			printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
#endif
		cmd_opt[cmd_cnt++] = "-s";
		cmd_opt[cmd_cnt++] = tmp1;
		write_line_to_file(RESOLV_CONF,1, line_buffer);
//		RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", tmp1, NULL_STR);
		cmd_opt[cmd_cnt++]=0;
		DoCmd(cmd_opt, NULL_FILE);	
	}else if(intValue==2){
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff1));
		sprintf(tmp1,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n",strtmp);
#ifdef TR181_SUPPORT
		y = 4;
		*((char*)entry)=(char)y;
		if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
		{
			printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
		memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
		entry[1].index = 3;
		entry[1].enable = 1;
		entry[1].status = 1;
		strcpy(entry[1].ipAddr, strtmp);
//		strcpy(entry[1].interface, interface);
		entry[1].type = 5; //static
		if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
		{
			printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
#endif
		cmd_opt[cmd_cnt++] = "-s";
		cmd_opt[cmd_cnt++] = tmp1;
		write_line_to_file(RESOLV_CONF,1, line_buffer);
		
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff2));
		sprintf(tmp2,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n", strtmp);
#ifdef TR181_SUPPORT
		y = 5;
		*((char*)entry)=(char)y;
		if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
		{
			printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
		memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
		entry[1].index = 4;
		entry[1].enable = 1;
		entry[1].status = 1;
		strcpy(entry[1].ipAddr, strtmp);
//		strcpy(entry[1].interface, interface);
		entry[1].type = 5; //static
		if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
		{
			printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
#endif
		cmd_opt[cmd_cnt++] = "-s";
		cmd_opt[cmd_cnt++] = tmp2;
		write_line_to_file(RESOLV_CONF,2, line_buffer);
//		RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", tmp1, "-s", tmp2, NULL_STR);
		cmd_opt[cmd_cnt++]=0;
		DoCmd(cmd_opt, NULL_FILE);
	}else if(intValue==3){
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff1));
		sprintf(tmp1,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n",strtmp);
#ifdef TR181_SUPPORT
		y = 4;
		*((char*)entry)=(char)y;
		if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
		{
			printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
		memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
		entry[1].index = 3;
		entry[1].enable = 1;
		entry[1].status = 1;
		strcpy(entry[1].ipAddr, strtmp);
//		strcpy(entry[1].interface, interface);
		entry[1].type = 5; //static
		if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
		{
			printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
#endif
		cmd_opt[cmd_cnt++] = "-s";
		cmd_opt[cmd_cnt++] = tmp1;
		write_line_to_file(RESOLV_CONF,1, line_buffer);
		
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff2));
		sprintf(tmp2,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n", strtmp);
#ifdef TR181_SUPPORT
		y = 5;
		*((char*)entry)=(char)y;
		if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
		{
			printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
		memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
		entry[1].index = 4;
		entry[1].enable = 1;
		entry[1].status = 1;
		strcpy(entry[1].ipAddr, strtmp);
//		strcpy(entry[1].interface, interface);
		entry[1].type = 5; //static
		if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
		{
			printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
#endif
		cmd_opt[cmd_cnt++] = "-s";
		cmd_opt[cmd_cnt++] = tmp2;
		write_line_to_file(RESOLV_CONF, 2, line_buffer);
		
		strtmp= inet_ntoa(*((struct in_addr *)tmpBuff3));
		sprintf(tmp3,"%s",strtmp);
		sprintf(line_buffer,"nameserver %s\n", strtmp);
#ifdef TR181_SUPPORT
		y = 6;
		*((char*)entry)=(char)y;
		if(apmib_get(MIB_DNS_CLIENT_SERVER_TBL,(void*)entry)==0)
		{
			printf("get MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
		memcpy(&(entry[1]), &(entry[0]), sizeof(DNS_CLIENT_SERVER_T));
		entry[1].index = 5;
		entry[1].enable = 1;
		entry[1].status = 1;
		strcpy(entry[1].ipAddr, strtmp);
//		strcpy(entry[1].interface, interface);
		entry[1].type = 5; //static
		if(apmib_set(MIB_DNS_CLIENT_SERVER_MOD,(void*)entry)==0)
		{
			printf("set MIB_DNS_CLIENT_SERVER_TBL fail!\n");
			return;
		}
#endif
		cmd_opt[cmd_cnt++] = "-s";
		cmd_opt[cmd_cnt++] = tmp3;
		write_line_to_file(RESOLV_CONF, 2, line_buffer);
		
//		RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", tmp1, "-s", tmp2, "-s", tmp3, NULL_STR);
		cmd_opt[cmd_cnt++]=0;
		DoCmd(cmd_opt, NULL_FILE);
	}else{
		printf("Invalid DNS server setting\n");
	}	
}
void start_upnp_igd(int wantype, int sys_opmode, int wisp_id, char *lan_interface)
{
	int intValue=0;
	char tmp1[16]={0};
	char tmp2[16]={0};
	apmib_get(MIB_UPNP_ENABLED, (void *)&intValue);
	RunSystemCmd(NULL_FILE, "killall", "-15", "miniigd", NULL_STR); 
	if(intValue==1){
		RunSystemCmd(NULL_FILE, "route", "del", "-net", "239.255.255.250", "netmask", "255.255.255.255", lan_interface, NULL_STR); 
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "239.255.255.250", "netmask", "255.255.255.255", lan_interface, NULL_STR); 
		sprintf(tmp1, "%d", wantype);
		sprintf(tmp2, "wlan%d", wisp_id);
		if(sys_opmode==WISP_MODE)
		{
#if defined(CONFIG_SMART_REPEATER)				
			getWispRptIfaceName(tmp2,wisp_id);
			//strcat(tmp2, "-vxd");
#endif			
			RunSystemCmd(NULL_FILE, "miniigd", "-e", tmp1, "-i", lan_interface, "-w", tmp2, NULL_STR); 
		}
		else	
		{
#ifdef MULTI_PPPOE
			int connnect_num;
			char str_connect[10];;
			apmib_get(MIB_PPP_CONNECT_COUNT, (void *)&connnect_num);
			sprintf(str_connect," %d",connnect_num);	
			if(PPPOE == wantype)
			{
				RunSystemCmd(NULL_FILE, "miniigd", "-e", tmp1, "-i", lan_interface,"-s",str_connect,NULL_STR);
			}
			else
			{
				RunSystemCmd(NULL_FILE, "miniigd", "-e", tmp1, "-i", lan_interface,NULL_STR);
			}
			
#else
			RunSystemCmd(NULL_FILE, "miniigd", "-e", tmp1, "-i", lan_interface,NULL_STR);
#endif
						
		}
		
	}
	
}
void start_ddns(void)
{
	unsigned int ddns_onoff;
	unsigned int ddns_type;
	unsigned char ddns_domanin_name[MAX_DOMAIN_LEN];
	unsigned char ddns_user_name[MAX_DOMAIN_LEN];
	unsigned char ddns_password[MAX_DOMAIN_LEN];
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "ddns_inet", NULL_STR);
	
	apmib_get( MIB_DDNS_ENABLED,  (void *)&ddns_onoff);

	if(ddns_onoff == 1)
	{
		apmib_get( MIB_DDNS_TYPE,  (void *)&ddns_type);

		apmib_get( MIB_DDNS_DOMAIN_NAME,  (void *)ddns_domanin_name);

		apmib_get( MIB_DDNS_USER,  (void *)ddns_user_name);

		apmib_get( MIB_DDNS_PASSWORD,  (void *)ddns_password);		

		if(ddns_type == 0) // 0:ddns; 1:tzo
			RunSystemCmd(NULL_FILE, "ddns_inet", "-x", "dyndns", ddns_user_name, ddns_password, ddns_domanin_name, NULL_STR);
		else if(ddns_type == 1)
			RunSystemCmd(NULL_FILE, "ddns_inet", "-x", "tzo", ddns_user_name, ddns_password, ddns_domanin_name, NULL_STR);


	}

}

#define NTPTMP_FILE "/tmp/ntp_tmp"

void start_ntp(void)
{
	unsigned int ntp_onoff=0;
	unsigned char buffer[500];

	unsigned int ntp_server_id;
	char	ntp_server[40];
	
	apmib_get(MIB_NTP_ENABLED, (void *)&ntp_onoff);
	RunSystemCmd(NULL_FILE, "rm", NTPTMP_FILE, NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "ntp_inet", "2>/dev/null", NULL_STR);	

	if(ntp_onoff == 1)
	{
		RunSystemCmd(NULL_FILE, "echo", "Start NTP daemon", NULL_STR);
		/* prepare requested info for ntp daemon */
		apmib_get( MIB_NTP_SERVER_ID,  (void *)&ntp_server_id);

		if(ntp_server_id == 0)
			apmib_get( MIB_NTP_SERVER_IP1,  (void *)buffer);
		else
			apmib_get( MIB_NTP_SERVER_IP2,  (void *)buffer);

		sprintf(ntp_server, "%s", inet_ntoa(*((struct in_addr *)buffer)));
		RunSystemCmd(NULL_FILE, "ntp_inet", "-x", ntp_server,NULL_STR);
		}
}

#if defined(ROUTE_SUPPORT)
void del_routing(void)
{
	int intValue=0, i;
	char	ip[32], netmask[32], gateway[32], *tmpStr=NULL;	
	int entry_Num=0;
	STATICROUTE_T entry;
	int rip_enabled=0;
	
	apmib_get(MIB_STATICROUTE_TBL_NUM, (void *)&entry_Num);
	if(entry_Num > 0){
		for (i=1; i<=entry_Num; i++) {
			*((char *)&entry) = (char)i;
			apmib_get(MIB_STATICROUTE_TBL, (void *)&entry);
	
			if(entry.metric < 0)				
				continue;
			
			tmpStr = inet_ntoa(*((struct in_addr *)entry.dstAddr));
			sprintf(ip, "%s", tmpStr);
			tmpStr = inet_ntoa(*((struct in_addr *)entry.netmask));
			sprintf(netmask, "%s", tmpStr);
			tmpStr = inet_ntoa(*((struct in_addr *)entry.gateway));
			sprintf(gateway, "%s", tmpStr);
			
			RunSystemCmd(NULL_FILE, "route", "del", "-net", ip, "netmask", netmask, "gw",  gateway, NULL_STR);
		}
	}

	apmib_get(MIB_RIP_ENABLED, (void *)&rip_enabled);
	if(rip_enabled)
	{
		RunSystemCmd(PROC_BR_IGMPDB, "echo", "add all ipv4 224.0.0.9 0xffffffff", NULL_STR);
	}
	else
	{
		RunSystemCmd(PROC_BR_IGMPDB, "echo", "del all ipv4 224.0.0.9 0xffffffff", NULL_STR);
	}
	
}
void start_routing(char *interface)
{
	int intValue=0, i;
	char line_buffer[64]={0};
	char tmp_args[16]={0};
	char	ip[32], netmask[32], gateway[32], *tmpStr=NULL;	
	int entry_Num=0;
	STATICROUTE_T entry;
	int nat_enabled=0, rip_enabled=0, rip_wan_tx=0;
	int rip_wan_rx=0, rip_lan_tx=0, rip_lan_rx=0;
	int start_routed=1;
	
	RunSystemCmd(NULL_FILE, "killall", "-15", "routed", NULL_STR); 
	apmib_get(MIB_NAT_ENABLED, (void *)&nat_enabled);
	apmib_get(MIB_RIP_ENABLED, (void *)&rip_enabled);
	apmib_get(MIB_RIP_LAN_TX, (void *)&rip_lan_tx);
	apmib_get(MIB_RIP_LAN_RX, (void *)&rip_lan_rx);
	apmib_get(MIB_RIP_WAN_TX, (void *)&rip_wan_tx);
	apmib_get(MIB_RIP_WAN_RX, (void *)&rip_wan_rx);
	line_buffer[0]=0x0d;
	line_buffer[1]=0x0a;
	write_line_to_file(ROUTED_CONF_FILE,1, line_buffer);
	memset(line_buffer, 0x00, 64);
	if(nat_enabled==0){
		if(rip_lan_tx !=0 && rip_lan_rx==0){
			sprintf(line_buffer,"network br0 0 %d\n",rip_lan_tx);
			write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			sprintf(line_buffer,"network %s 0 %d\n",interface, rip_lan_tx);
			write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			
		}else if(rip_lan_tx !=0 && rip_lan_rx !=0){
				sprintf(line_buffer,"network br0 %d %d\n",rip_lan_rx, rip_lan_tx);
				write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
				sprintf(line_buffer,"network %s %d %d\n",interface, rip_lan_rx, rip_lan_tx);
				write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			
		}else{
			if( rip_lan_rx !=0){
				sprintf(line_buffer,"network br0 %d 0\n",rip_lan_rx);
				write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
				sprintf(line_buffer,"network %s %d 0\n",interface, rip_lan_rx);
				write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			}else
				start_routed=0;
		}
	}else{
		if( rip_lan_rx !=0){
			sprintf(line_buffer,"network br0 %d 0\n",rip_lan_rx);
			write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
			sprintf(line_buffer,"network %s %d 0\n",interface, rip_lan_rx);
			write_line_to_file(ROUTED_CONF_FILE, 2 , line_buffer);
		}else
			start_routed=0;
	}
	apmib_get(MIB_STATICROUTE_ENABLED, (void *)&intValue);
	apmib_get(MIB_STATICROUTE_TBL_NUM, (void *)&entry_Num);
	if(intValue > 0 && entry_Num > 0){
		for (i=1; i<=entry_Num; i++) {
			*((char *)&entry) = (char)i;
			apmib_get(MIB_STATICROUTE_TBL, (void *)&entry);
	
			if(entry.metric < 0)
				continue;
				
			tmpStr = inet_ntoa(*((struct in_addr *)entry.dstAddr));
			sprintf(ip, "%s", tmpStr);
			tmpStr = inet_ntoa(*((struct in_addr *)entry.netmask));
			sprintf(netmask, "%s", tmpStr);
			tmpStr = inet_ntoa(*((struct in_addr *)entry.gateway));
			sprintf(gateway, "%s", tmpStr);
			sprintf(tmp_args, "%d", entry.metric);
			if(!strcmp(interface, "ppp0")){
				if(entry.interface==1){//wan interface
					RunSystemCmd(NULL_FILE, "route", "add", "-net", ip, "netmask", netmask, "metric", tmp_args, "dev", interface,  NULL_STR);
				}else{
					RunSystemCmd(NULL_FILE, "route", "add", "-net", ip, "netmask", netmask, "gw",  gateway, "metric", tmp_args, "dev", "br0",  NULL_STR);
				}
			}else{
				if(entry.interface==1){//wan interface
					RunSystemCmd(NULL_FILE, "route", "add", "-net", ip, "netmask", netmask, "gw",  gateway, "metric", tmp_args, "dev", interface,  NULL_STR);
				}else if(entry.interface==0){
					RunSystemCmd(NULL_FILE, "route", "add", "-net", ip, "netmask", netmask, "gw",  gateway, "metric", tmp_args, "dev", "br0",  NULL_STR);
				}
			}
		}
	}
	
	if(rip_enabled !=0 && start_routed==1)
		RunSystemCmd(NULL_FILE, "routed", "-s",  NULL_STR);
	
	if(nat_enabled==0){
		if(isFileExist(IGMPPROXY_PID_FILE)){
			unlink(IGMPPROXY_PID_FILE);
		}
		RunSystemCmd(NULL_FILE, "killall", "-9", "igmpproxy", NULL_STR);
		RunSystemCmd(PROC_BR_MCASTFASTFWD, "echo", "1,1", NULL_STR);
	}

	if(rip_enabled)
	{
		RunSystemCmd(PROC_BR_IGMPDB, "echo", "add all ipv4 224.0.0.9 0xffffffff", NULL_STR);
	}
	else
	{
		RunSystemCmd(PROC_BR_IGMPDB, "echo", "del all ipv4 224.0.0.9 0xffffffff", NULL_STR);
	}
}
#endif

void start_igmpproxy(char *wan_iface, char *lan_iface)
{
	int intValue=0;
	apmib_get(MIB_IGMP_PROXY_DISABLED, (void *)&intValue);
	RunSystemCmd(NULL_FILE, "killall", "-9", "igmpproxy", NULL_STR);
	RunSystemCmd(PROC_BR_MCASTFASTFWD, "echo", "1,1", NULL_STR);
	if(intValue==0) {
#ifdef SUPPORT_ZIONCOM_RUSSIA		
		RunSystemCmd(NULL_FILE, "igmpproxy", "eth1", lan_iface, NULL_STR);
#else
		RunSystemCmd(NULL_FILE, "igmpproxy", wan_iface, lan_iface, NULL_STR);
#endif
		RunSystemCmd(PROC_IGMP_MAX_MEMBERS, "echo", "128", NULL_STR);
		RunSystemCmd(PROC_BR_MCASTFASTFWD, "echo", "1,1", NULL_STR);
	}
	
}
void start_wan_dhcp_client(char *iface)
{
	char hostname[100];
	char cmdBuff[200];
	char script_file[100], deconfig_script[100], pid_file[100];
	
	sprintf(script_file, "/usr/share/udhcpc/%s.sh", iface); /*script path*/
	sprintf(deconfig_script, "/usr/share/udhcpc/%s.deconfig", iface);/*deconfig script path*/
	sprintf(pid_file, "/etc/udhcpc/udhcpc-%s.pid", iface); /*pid path*/
	killDaemonByPidFile(pid_file);
	Create_script(deconfig_script, iface, WAN_NETWORK, 0, 0, 0);
	memset(hostname, 0x00, 100);
	apmib_get( MIB_HOST_NAME, (void *)&hostname);

	if(hostname[0]){
		sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s -h %s -a 30 &", iface, pid_file, script_file, hostname);
		//RunSystemCmd(NULL_FILE, "udhcpc", "-i", iface, "-p", pid_file, "-s", script_file,  "-a", "30", "-h", hostname,  NULL_STR);
	}else{
		sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s -a 30 &", iface, pid_file, script_file);
		//RunSystemCmd(NULL_FILE, "udhcpc", "-i", iface, "-p", pid_file, "-s", script_file,  "-a", "30", NULL_STR);
	}
	system(cmdBuff);
}
void set_staticIP(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{	
	int intValue=0;
#ifdef TR181_SUPPORT
	int dnsEnable;
#endif
	char tmpBuff[200];
	char tmp_args[16];
	char Ip[32], Mask[32], Gateway[32];

	int wan_type;
	apmib_get( MIB_WAN_DHCP,  (void *)&wan_type);

#ifdef TR181_SUPPORT
	if ( !apmib_get(MIB_DNS_CLIENT_ENABLE,(void *)&dnsEnable)){
			fprintf(stderr,"get MIB_DNS_CLIENT_ENABLE failed\n");
			return;  
	}
#endif
	
	if(wan_type==PPTP)
		apmib_get( MIB_PPTP_IP_ADDR,  (void *)tmpBuff);
	else if(wan_type==L2TP)
		apmib_get( MIB_L2TP_IP_ADDR,  (void *)tmpBuff);	
	else
		apmib_get( MIB_WAN_IP_ADDR,  (void *)tmpBuff);
	
	sprintf(Ip, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));

	if(wan_type==PPTP)
		apmib_get( MIB_PPTP_SUBNET_MASK,  (void *)tmpBuff);
	else if(wan_type==L2TP)
		apmib_get( MIB_L2TP_SUBNET_MASK,  (void *)tmpBuff);
	else
		apmib_get( MIB_WAN_SUBNET_MASK,  (void *)tmpBuff);
	
	sprintf(Mask, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));

	if(wan_type==PPTP)
		apmib_get(MIB_PPTP_DEFAULT_GW,  (void *)tmpBuff);
	else if(wan_type==L2TP)
		apmib_get(MIB_L2TP_DEFAULT_GW,  (void *)tmpBuff);
	else
		apmib_get(MIB_WAN_DEFAULT_GATEWAY,  (void *)tmpBuff);
				
	if (!memcmp(tmpBuff, "\x0\x0\x0\x0", 4))
		memset(Gateway, 0x00, 32);
	else
		sprintf(Gateway, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
			
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, Ip, "netmask", Mask, NULL_STR);
		
	if(Gateway[0])
	{
		RunSystemCmd(NULL_FILE, "route", "del", "default", wan_iface, NULL_STR);
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", Gateway, "dev", wan_iface, NULL_STR);
	}
	
	if(wan_type!=PPTP && wan_type!=L2TP)
	{
		apmib_get(MIB_FIXED_IP_MTU_SIZE, (void *)&intValue);
		sprintf(tmp_args, "%d", intValue);
		RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, "mtu", tmp_args, NULL_STR);
	}
#ifdef TR181_SUPPORT
	if(dnsEnable==1)
#endif
		start_dns_relay();

	if(wan_type==PPTP || wan_type==L2TP)
		return ;
	
	start_upnp_igd(DHCP_DISABLED, sys_op, wisp_id, lan_iface);	
	setFirewallIptablesRules(0, NULL);
	
	start_ntp();
	start_ddns();
	start_igmpproxy(wan_iface, lan_iface);
	
#if defined(ROUTE_SUPPORT)
		del_routing();
		start_routing(wan_iface);
#endif
#ifdef SEND_GRATUITOUS_ARP
		//char tmpBuf[128];
		snprintf(tmpBuff, 128, "%s/%s %s", _CONFIG_SCRIPT_PATH, _FIREWALL_SCRIPT_PROG, "Send_GARP"); 	
		//printf("CMD is : %s \n", tmpBuff);
		system(tmpBuff);
#endif
}
void set_dhcp_client(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0;
	char tmp_args[16];
	
	apmib_get(MIB_DHCP_MTU_SIZE, (void *)&intValue);
	sprintf(tmp_args, "%d", intValue);
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, "mtu", tmp_args, NULL_STR);
	start_wan_dhcp_client(wan_iface);
	start_upnp_igd(DHCP_CLIENT, sys_op, wisp_id, lan_iface);
}
void set_pppoe(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0, cmdRet=-1;
//	int intValue1=0;
	char line_buffer[100]={0};
	char tmp_args[64]={0};
	char tmp_args1[32]={0};
	int connect_type=0, idle_time=0;
#ifdef MULTI_PPPOE
	FILE *pAC , *PSubNet;
	int connnect_num,index;
	char AC_Name[40];
	char SubNet[40];
	char command[100];
		char* wan_interface[] = {"eth1","eth5"};
	char* order2Name[] = {"FIRST","SECOND","THIRD","FORTH"};
	//dzh 2011-12-21
	system("echo eth1 br0 172.29.17.10 172.29.17.11 >> /etc/dnrd/dns_config");
	system("echo eth5 br0 172.29.17.10 172.29.17.11 >> /etc/dnrd/dns_config");	
	//dzh end
	
	char* pppoe_file_list[4][3]=
	{
		{"/etc/ppp/pap-secrets","/etc/ppp/chap-secrets","/etc/ppp/options"},
		{"/etc/ppp/pap-secrets2","/etc/ppp/chap-secrets2","/etc/ppp/options2"},
		{"/etc/ppp/pap-secrets3","/etc/ppp/chap-secrets3","/etc/ppp/options3"},
		{"/etc/ppp/pap-secrets4","/etc/ppp/chap-secrets4","/etc/ppp/options4"}	
	};
	apmib_get(MIB_PPP_CONNECT_COUNT, (void *)&connnect_num);
	sprintf(command,"echo %d > /etc/ppp/ppp_connect_number",connnect_num);
	system(command);
	
	if(isFileExist("/etc/ppp/AC_Names"))
		unlink("/etc/ppp/AC_Names");
	
	if(isFileExist("/etc/ppp/SubInfos"))
		unlink("/etc/ppp/SubInfos");
	
	pAC = fopen("/etc/ppp/AC_Names","w+");
	PSubNet = fopen("/etc/ppp/SubInfos","w+");
	
	fprintf(pAC,"%d\n",connnect_num);
	fprintf(PSubNet,"%d\n",connnect_num);	
	
	for(index = 0 ; index < connnect_num ; ++index)
	{
		if(0 == index)
		{
			apmib_get(MIB_PPP_SERVICE_NAME, (void *)&AC_Name);	
			apmib_get(MIB_PPP_SUBNET1, (void *)&SubNet);	
		}
		else if(1 == index)
		{
			apmib_get(MIB_PPP_SERVICE_NAME2, (void *)&AC_Name);
			apmib_get(MIB_PPP_SUBNET2, (void *)&SubNet);				
		}
		else if(2 == index)
		{	
			apmib_get(MIB_PPP_SERVICE_NAME3, (void *)&AC_Name);	
			apmib_get(MIB_PPP_SUBNET3, (void *)&SubNet);				
		}
		else if(3 == index)
		{
			apmib_get(MIB_PPP_SERVICE_NAME4, (void *)&AC_Name);
			apmib_get(MIB_PPP_SUBNET4, (void *)&SubNet);				
		}
		fprintf(pAC,"%s\n",AC_Name);
		fprintf(PSubNet,"%s\n",SubNet);	
	}

	close(pAC);
	close(PSubNet);
#endif

	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, "0.0.0.0", NULL_STR);
//	RunSystemCmd(NULL_FILE, "route", "del", "default", "gw", "0.0.0.0", NULL_STR);
//	cmdRet = RunSystemCmd(NULL_FILE, "flash", "gen-pppoe", PPP_OPTIONS_FILE, PPP_PAP_FILE, PPP_CHAP_FILE,NULL_STR);
#ifdef MULTI_PPPOE
	for(index = 0 ;index < connnect_num ;++index)
	{
		cmdRet = RunSystemCmd(NULL_FILE, "flash", "gen-pppoe",
			pppoe_file_list[index][2], 
			pppoe_file_list[index][0],
			pppoe_file_list[index][1], 
			order2Name[index] , NULL_STR);
		if(cmdRet==0){
			sprintf(line_buffer,"%s\n", "noauth");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "nomppc");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "noipdefault");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "hide-password");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "defaultroute");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "persist");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "ipcp-accept-remote");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "ipcp-accept-local");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "nodetach");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "usepeerdns");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			
			if(0 == index)
			{
				apmib_get(MIB_PPP_MTU_SIZE, (void *)&intValue);
				apmib_get(MIB_PPP_CONNECT_TYPE, (void *)&connect_type);
				apmib_get(MIB_PPP_IDLE_TIME, (void *)&idle_time);
			//	apmib_get( MIB_PPP_SERVICE_NAME,  (void *)tmp_args);
			}
			else if(1 == index)
			{
				apmib_get(MIB_PPP_MTU_SIZE2, (void *)&intValue);
				apmib_get(MIB_PPP_CONNECT_TYPE2, (void *)&connect_type);
				apmib_get(MIB_PPP_IDLE_TIME, (void *)&idle_time);				
			//	apmib_get( MIB_PPP_SERVICE_NAME2,  (void *)tmp_args);
			}
			else if(2 == index)
			{
				apmib_get(MIB_PPP_MTU_SIZE3, (void *)&intValue);
				apmib_get(MIB_PPP_CONNECT_TYPE3, (void *)&connect_type);
				apmib_get(MIB_PPP_IDLE_TIME3, (void *)&idle_time);				
			//	apmib_get( MIB_PPP_SERVICE_NAME3,  (void *)tmp_args);
			}
			else if(3 == index)
			{
				apmib_get(MIB_PPP_MTU_SIZE4, (void *)&intValue);
				apmib_get(MIB_PPP_CONNECT_TYPE4, (void *)&connect_type);
				apmib_get(MIB_PPP_IDLE_TIME4, (void *)&idle_time);				
			//	apmib_get( MIB_PPP_SERVICE_NAME4,  (void *)tmp_args);
			}
			
			sprintf(line_buffer,"mtu %d\n", intValue);
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"mru %d\n", intValue);
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "lcp-echo-interval 20");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "lcp-echo-failure 3");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "wantype 3");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			sprintf(line_buffer,"%s\n", "holdoff 10");
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			//apmib_get( MIB_PPP_SERVICE_NAME,  (void *)tmp_args);
			wan_iface = wan_interface[index];
			if(tmp_args[0]){
				//sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a rp_pppoe_ac 62031090091393-Seednet_240_58 rp_pppoe_service %s %s\n",tmp_args, wan_iface);
				sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a rp_pppoe_service %s %s\n",tmp_args, wan_iface);
			}else{
				sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a %s\n", wan_iface);
			}
			write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			
			//apmib_get(MIB_PPP_CONNECT_TYPE, (void *)&connect_type);
			if(connect_type==1){
				//apmib_get(MIB_PPP_IDLE_TIME, (void *)&idle_time);
				sprintf(line_buffer,"%s\n", "demand");
				write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
				sprintf(line_buffer,"idle %d\n", idle_time);
				write_line_to_file(pppoe_file_list[index][2],2, line_buffer);
			}else if(connect_type==2 && act_source==1 ) //manual mode we do not dial up from init.sh
					return;				
		}
		
	
	}
#else
	cmdRet = RunSystemCmd(NULL_FILE, "flash", "gen-pppoe", PPP_OPTIONS_FILE1, PPP_PAP_FILE1, PPP_CHAP_FILE1,NULL_STR);
	if(cmdRet==0){
		sprintf(line_buffer,"%s\n", "noauth");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "nomppc");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "noipdefault");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "hide-password");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "defaultroute");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "persist");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "ipcp-accept-remote");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "ipcp-accept-local");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "nodetach");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "usepeerdns");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		apmib_get(MIB_PPP_MTU_SIZE, (void *)&intValue);
		sprintf(line_buffer,"mtu %d\n", intValue);
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"mru %d\n", intValue);
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "lcp-echo-interval 20");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "lcp-echo-failure 3");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "wantype 3");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		sprintf(line_buffer,"%s\n", "holdoff 10");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
#ifdef CONFIG_IPV6
		apmib_get(MIB_IPV6_WAN_ENABLE, (void *)&intValue);
		if(intValue){
			sprintf(line_buffer,"%s\n", "+ipv6");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		}
#endif
		apmib_get( MIB_PPP_SERVICE_NAME,  (void *)tmp_args);
		if(tmp_args[0]){
			//sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a rp_pppoe_ac 62031090091393-Seednet_240_58 rp_pppoe_service %s %s\n",tmp_args, wan_iface);
			sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a rp_pppoe_service %s %s\n",tmp_args, wan_iface);
		}else{
			sprintf(line_buffer,"plugin /etc/ppp/plubins/libplugin.a %s\n", wan_iface);
		}
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		apmib_get(MIB_PPP_CONNECT_TYPE, (void *)&connect_type);
		if(connect_type==1){
			apmib_get(MIB_PPP_IDLE_TIME, (void *)&idle_time);
			sprintf(line_buffer,"%s\n", "demand");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			sprintf(line_buffer,"idle %d\n", idle_time);
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		}else if(connect_type==2 && act_source==1) //manual mode we do not dial up from init.sh
				return;		
	}
		

#endif
/*
		apmib_get(MIB_PPP_CONNECT_TYPE, (void *)&connect_type);
		if(connect_type==1){
			apmib_get(MIB_PPP_IDLE_TIME, (void *)&idle_time);
			sprintf(line_buffer,"%s\n", "demand");
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
			sprintf(line_buffer,"idle %d\n", idle_time);
			write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
		}else if(connect_type==2 && act_source==1) //manual mode we do not dial up from init.sh
				return;
*/			
	#if 0
		apmib_get( MIB_DNS_MODE, (void *)&intValue1);
		if(intValue1==1){
			start_dns_relay();
		}else{
			RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", "168.95.1.1",NULL_STR);
		}
	#endif
		if(isFileExist(PPP_FILE)){
			unlink(PPP_FILE);
		} 
		sprintf(tmp_args, "%s", "3");/*wan type*/
		sprintf(tmp_args1, "%d", connect_type);/*connect type*/
		RunSystemCmd(NULL_FILE, "ppp_inet", "-t", tmp_args,  "-c", tmp_args1, "-x", NULL_STR);
		start_upnp_igd(PPPOE, sys_op, wisp_id, lan_iface);
}
#ifdef SUPPORT_ZIONCOM_RUSSIA
void addOneRoute(struct in_addr *l2tp_server)
{
	FILE *fp=NULL;
	if((fp=fopen("/var/dhcpc_route.conf", "r+"))==NULL)
		return;
	
	unsigned char routebuf[16];
	unsigned char cmdbuf[128];
	
	fscanf(fp, "%s", routebuf);
	fclose(fp);
	
	sprintf(cmdbuf, "route add -host %s gw %s dev eth1", inet_ntoa(*l2tp_server), routebuf);
	system(cmdbuf);
}
#endif
void set_pptp(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0, intValue1=0, cmdRet=-1;
	char line_buffer[100]={0};
	char tmp_args[64]={0};
	char tmp_args1[32]={0};
	char Ip[32], Mask[32], ServerIp[32];
	int connect_type=0, idle_time=0;
	char *strtmp=NULL;
#if defined(CONFIG_DYNAMIC_WAN_IP)
	char pptpDefGw[32], netIp[32];
	unsigned int ipAddr, netAddr, netMask, serverAddr;
	int pptp_wanip_dynamic=0;
	

	apmib_get(MIB_PPTP_WAN_IP_DYNAMIC, (void *)&pptp_wanip_dynamic);

	apmib_get(MIB_PPTP_SERVER_IP_ADDR,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(ServerIp, "%s", strtmp);
	serverAddr=((struct in_addr *)tmp_args)->s_addr;
	
	if(pptp_wanip_dynamic==STATIC_IP){	//pptp use static wan ip
	apmib_get(MIB_PPTP_DEFAULT_GW,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(pptpDefGw, "%s", strtmp);
#ifdef SUPPORT_ZIONCOM_RUSSIA
	write_line_to_file("/var/dhcpc_route.conf", 1, strtmp);
#endif
#else
	apmib_get(MIB_PPTP_SERVER_IP_ADDR,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(ServerIp, "%s", strtmp);	
#endif
	apmib_get(MIB_PPTP_IP_ADDR,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(Ip, "%s", strtmp);	
#if defined(CONFIG_DYNAMIC_WAN_IP)
	ipAddr=((struct in_addr *)tmp_args)->s_addr;
#endif	

	apmib_get(MIB_PPTP_SUBNET_MASK,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(Mask, "%s", strtmp);
#if defined(CONFIG_DYNAMIC_WAN_IP)
	netMask=((struct in_addr *)tmp_args)->s_addr;
#endif
	
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, Ip, "netmask", Mask, NULL_STR);
	RunSystemCmd(NULL_FILE, "route", "del", "default", "gw", "0.0.0.0", NULL_STR);
#if defined(CONFIG_DYNAMIC_WAN_IP)
		if((serverAddr & netMask) != (ipAddr & netMask)){
			//Patch for our router under another router to dial up pptp
			//let pptp dialing pkt via pptp default gateway
			netAddr = (serverAddr & netMask);
			((struct in_addr *)tmp_args)->s_addr=netAddr;
			strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
			sprintf(netIp, "%s", strtmp);
			RunSystemCmd(NULL_FILE, "route", "add", "-net", netIp, "netmask", Mask,"gw", pptpDefGw,NULL_STR);
		}
	} //end for pptp use static wan ip
#endif
	
#ifdef SUPPORT_ZIONCOM_RUSSIA
	struct in_addr saddr;
	apmib_get(MIB_PPTP_SERVER_IP_ADDR,  (void *)&saddr);
	addOneRoute(&saddr);
#endif
	cmdRet = RunSystemCmd(NULL_FILE, "flash", "gen-pptp", PPP_OPTIONS_FILE1, PPP_PAP_FILE1, PPP_CHAP_FILE1,NULL_STR);
	
	if(cmdRet==0){
		sprintf(line_buffer,"%s\n", "lock");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "noauth");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);

		/*align the pptp packet*/
		sprintf(line_buffer,"%s\n", "nopcomp");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "noaccomp");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);		
		/***************************************************/
		
		sprintf(line_buffer,"%s\n", "nobsdcomp");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "nodeflate");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "usepeerdns");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);		
#ifndef SUPPORT_ZIONCOM_RUSSIA
		sprintf(line_buffer,"%s\n", "lcp-echo-interval 20");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "lcp-echo-failure 3");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
#endif		
		sprintf(line_buffer,"%s\n", "wantype 4");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		apmib_get(MIB_PPTP_MTU_SIZE, (void *)&intValue);
		sprintf(line_buffer,"mtu %d\n", intValue);
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "holdoff 2");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "refuse-eap");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "remotename PPTP");
		write_line_to_file(PPTP_PEERS_FILE,1, line_buffer);
		
		sprintf(line_buffer,"%s\n", "linkname PPTP");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "ipparam PPTP");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(tmp_args, "pty \"pptp %s --nolaunchpppd\"", ServerIp);
		sprintf(line_buffer,"%s\n", tmp_args);
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		apmib_get( MIB_PPTP_USER_NAME,  (void *)tmp_args);
		sprintf(line_buffer,"name %s\n", tmp_args);
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		apmib_get( MIB_PPTP_SECURITY_ENABLED, (void *)&intValue);
		if(intValue==1){
			sprintf(line_buffer,"%s\n", "+mppe required,stateless");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
			
			//sprintf(line_buffer,"%s\n", "+mppe no128,stateless");/*disable 128bit encrypt*/
			//write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
			//sprintf(line_buffer,"%s\n", "+mppe no56,stateless");/*disable 56bit encrypt*/
			//write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
			
		}
		apmib_get( MIB_PPTP_MPPC_ENABLED, (void *)&intValue1);
		if(intValue1==1){
			sprintf(line_buffer,"%s\n", "mppc");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
			sprintf(line_buffer,"%s\n", "stateless");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		}else{
			sprintf(line_buffer,"%s\n", "nomppc");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		}
		if(intValue ==0 && intValue1==0){
			sprintf(line_buffer,"%s\n", "noccp");
			write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		}
		
		sprintf(line_buffer,"%s\n", "persist");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "noauth");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "file /etc/ppp/options");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "nobsdcomp");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "nodetach");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "novj");
		write_line_to_file(PPTP_PEERS_FILE,2, line_buffer);
		
		
		apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&connect_type);
		if(connect_type==1){

			RunSystemCmd(NULL_FILE, "route", "del", "default", NULL_STR);
			RunSystemCmd(NULL_FILE, "route", "add", "default", "gw", "10.112.112.112", wan_iface, NULL_STR);
			
			sprintf(line_buffer,"%s\n", "persist");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "nodetach");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "connect /etc/ppp/true");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "demand");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			apmib_get(MIB_PPTP_IDLE_TIME, (void *)&idle_time);
			sprintf(line_buffer,"idle %d\n", idle_time);
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "ktune");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "ipcp-accept-remote");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "ipcp-accept-local");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "noipdefault");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "hide-password");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
			sprintf(line_buffer,"%s\n", "defaultroute");
			write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		}else if(connect_type==2 && act_source==1 && !isFileExist(MANUAL_CONNECT_NOW)) //manual mode we do not dial up from init.sh
				return;
			
	#if 0
		apmib_get( MIB_DNS_MODE, (void *)&intValue1);
		if(intValue1==1){
			start_dns_relay();
		}else{
			RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", "168.95.1.1",NULL_STR);
		}
	#endif	
		if(isFileExist(PPP_FILE)){
			unlink(PPP_FILE);
		} 
		RunSystemCmd(NULL_FILE, "killall", "pptp", NULL_STR);
		RunSystemCmd(NULL_FILE, "killall", "ppp_inet", NULL_STR);		
		sleep(2);
		sprintf(tmp_args, "%s", "4");/*wan type*/
		sprintf(tmp_args1, "%d", connect_type);/*connect type*/
#if 0//def SUPPORT_ZIONCOM_RUSSIA
		RunSystemCmd(NULL_FILE, "killall", "-9", "ppp_inet", NULL_STR);
		sleep(2);
#endif
		RunSystemCmd(NULL_FILE, "ppp_inet", "-t", tmp_args,  "-c", tmp_args1, "-x", NULL_STR);
	}
	start_upnp_igd(PPTP, sys_op, wisp_id, lan_iface);
}

void set_l2tp(int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int intValue=0;
//	int intValue1=0;
	char line_buffer[100]={0};
	char tmp_args[64]={0};
	char tmp_args1[32]={0};
	char Ip[32], Mask[32], ServerIp[32];
	int connect_type=0, idle_time=0;
	char *strtmp=NULL;
	int pwd_len=0;
#if defined(CONFIG_DYNAMIC_WAN_IP)
	char l2tpDefGw[32], netIp[32];
	unsigned int ipAddr, netAddr, netMask, serverAddr;
	int l2tp_wanip_dynamic=0;
	

	apmib_get(MIB_L2TP_WAN_IP_DYNAMIC, (void *)&l2tp_wanip_dynamic);

	apmib_get(MIB_L2TP_SERVER_IP_ADDR,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(ServerIp, "%s", strtmp);
	serverAddr=((struct in_addr *)tmp_args)->s_addr;

	if(l2tp_wanip_dynamic==STATIC_IP)
	{//l2tp use static wan ip
	apmib_get(MIB_L2TP_DEFAULT_GW,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(l2tpDefGw, "%s", strtmp);
#ifdef SUPPORT_ZIONCOM_RUSSIA
	write_line_to_file("/var/dhcpc_route.conf", 1, strtmp);
#endif
#else
	apmib_get(MIB_L2TP_SERVER_IP_ADDR,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(ServerIp, "%s", strtmp);
#endif
	apmib_get(MIB_L2TP_IP_ADDR,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(Ip, "%s", strtmp);
#if defined(CONFIG_DYNAMIC_WAN_IP)
	ipAddr=((struct in_addr *)tmp_args)->s_addr;
#endif
	apmib_get(MIB_L2TP_SUBNET_MASK,  (void *)tmp_args);
	strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
	sprintf(Mask, "%s", strtmp);
#if defined(CONFIG_DYNAMIC_WAN_IP)
	netMask=((struct in_addr *)tmp_args)->s_addr;
#endif
	
	RunSystemCmd(NULL_FILE, "ifconfig", wan_iface, Ip, "netmask", Mask, NULL_STR);
	RunSystemCmd(NULL_FILE, "route", "del", "default", "gw", "0.0.0.0", NULL_STR);
#if defined(CONFIG_DYNAMIC_WAN_IP)
		if((serverAddr & netMask) != (ipAddr & netMask)){
			//Patch for our router under another router to dial up l2tp
			//let l2tp dialing pkt via l2tp default gateway
			netAddr = (serverAddr & netMask);
			((struct in_addr *)tmp_args)->s_addr=netAddr;
			strtmp= inet_ntoa(*((struct in_addr *)tmp_args));
			sprintf(netIp, "%s", strtmp);
			RunSystemCmd(NULL_FILE, "route", "add", "-net", netIp, "netmask", Mask,"gw", l2tpDefGw,NULL_STR);
		}
	} // end for l2tp static ip
#endif	

#ifdef SUPPORT_ZIONCOM_RUSSIA
	struct in_addr saddr;
	apmib_get(MIB_L2TP_SERVER_IP_ADDR,  (void *)&saddr);
	addOneRoute(&saddr);
#endif

#if defined(RTL_L2TP_POWEROFF_PATCH)    //patch for l2tp by jiawenjan
	char l2tp_cmdBuf[100];
	int buff_length = 0;
	unsigned int l2tp_ns = 0;
	unsigned char  l2tp_tmpBuff[100], lanIp_tmp[16], serverIp_tmp[16];
	memset(lanIp_tmp,0, sizeof(lanIp_tmp));
	memset(serverIp_tmp,0, sizeof(serverIp_tmp));
	memset(l2tp_tmpBuff,0, sizeof(l2tp_tmpBuff));
	
	apmib_get(MIB_L2TP_PAYLOAD_LENGTH, (void *)&buff_length);
	if(buff_length>0)
	{	
		apmib_get(MIB_L2TP_NS, (void *)&l2tp_ns);
		apmib_get(MIB_L2TP_IP_ADDR,  (void *)lanIp_tmp);	
		apmib_get(MIB_L2TP_SERVER_IP_ADDR,	(void *)serverIp_tmp);
		apmib_get(MIB_L2TP_PAYLOAD,  (void *)l2tp_tmpBuff);
	
		sprintf(l2tp_cmdBuf,"flash clearl2tp %d %d %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		l2tp_ns, buff_length, lanIp_tmp[0], lanIp_tmp[1], lanIp_tmp[2], lanIp_tmp[3], serverIp_tmp[0], serverIp_tmp[1], serverIp_tmp[2], serverIp_tmp[3], 
		l2tp_tmpBuff[0], l2tp_tmpBuff[1], l2tp_tmpBuff[2], l2tp_tmpBuff[3], l2tp_tmpBuff[4], l2tp_tmpBuff[5], l2tp_tmpBuff[6], l2tp_tmpBuff[7], 
		l2tp_tmpBuff[8], l2tp_tmpBuff[9], l2tp_tmpBuff[10], l2tp_tmpBuff[11], l2tp_tmpBuff[12], l2tp_tmpBuff[13], l2tp_tmpBuff[14], l2tp_tmpBuff[15], 
		l2tp_tmpBuff[16], l2tp_tmpBuff[17], l2tp_tmpBuff[18], l2tp_tmpBuff[19], l2tp_tmpBuff[20], l2tp_tmpBuff[21], l2tp_tmpBuff[22], l2tp_tmpBuff[23], 
		l2tp_tmpBuff[24], l2tp_tmpBuff[25], l2tp_tmpBuff[26], l2tp_tmpBuff[27], l2tp_tmpBuff[28], l2tp_tmpBuff[29], l2tp_tmpBuff[30], l2tp_tmpBuff[31], 
		l2tp_tmpBuff[32], l2tp_tmpBuff[33], l2tp_tmpBuff[34], l2tp_tmpBuff[35], l2tp_tmpBuff[36], l2tp_tmpBuff[37]);

		system(l2tp_cmdBuf); 
	}
#endif 	
	
	apmib_get( MIB_L2TP_USER_NAME,  (void *)tmp_args);
	apmib_get( MIB_L2TP_PASSWORD,  (void *)tmp_args1);
	pwd_len = strlen(tmp_args1);
	/*options file*/
	sprintf(line_buffer,"user \"%s\"\n",tmp_args);
	write_line_to_file(PPP_OPTIONS_FILE1, 1, line_buffer);
	
	/*secrets files*/
	sprintf(line_buffer,"%s\n","#################################################");
	write_line_to_file(PPP_PAP_FILE1, 1, line_buffer);
	
	sprintf(line_buffer, "\"%s\"	*	\"%s\"\n",tmp_args, tmp_args1);
	write_line_to_file(PPP_PAP_FILE1, 2, line_buffer);
	
	sprintf(line_buffer,"%s\n","#################################################");
	write_line_to_file(PPP_CHAP_FILE1, 1, line_buffer);
	
	sprintf(line_buffer, "\"%s\"	*	\"%s\"\n",tmp_args, tmp_args1);
	write_line_to_file(PPP_CHAP_FILE1, 2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "lock");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	sprintf(line_buffer,"%s\n", "noauth");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	sprintf(line_buffer,"%s\n", "defaultroute");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	sprintf(line_buffer,"%s\n", "usepeerdns");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
    sprintf(line_buffer,"%s\n", "lcp-echo-interval 20");
    write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
 
    sprintf(line_buffer,"%s\n", "lcp-echo-failure 3");
    write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);

	sprintf(line_buffer,"%s\n", "wantype 6");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	apmib_get(MIB_L2TP_MTU_SIZE, (void *)&intValue);
	sprintf(line_buffer,"mtu %d\n", intValue);
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	apmib_get( MIB_L2TP_USER_NAME,  (void *)tmp_args);
	sprintf(line_buffer,"name %s\n", tmp_args);
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
//	sprintf(line_buffer,"%s\n", "noauth");
//	write_line_to_file(PPP_OPTIONS_FILE,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "nodeflate");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "nobsdcomp");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "nodetach");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "novj");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "default-asyncmap");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "nopcomp");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "noaccomp");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "noccp");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "novj");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "refuse-eap");
	write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	
	if(pwd_len > 35){
		sprintf(line_buffer,"%s\n", "-mschap");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
		
		sprintf(line_buffer,"%s\n", "-mschap-v2");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
	}
	
	sprintf(line_buffer,"%s\n", "[global]");
	write_line_to_file(L2TPCONF,1, line_buffer);
	
	sprintf(line_buffer,"%s\n", "port = 1701");
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	sprintf(line_buffer,"auth file = %s\n", PPP_CHAP_FILE1);
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "[lac client]");
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	sprintf(line_buffer,"lns=%s\n", ServerIp);
	write_line_to_file(L2TPCONF,2, line_buffer);

	sprintf(line_buffer,"%s\n", "require chap = yes");
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	apmib_get( MIB_L2TP_USER_NAME,  (void *)tmp_args);
	sprintf(line_buffer,"name = %s\n", tmp_args);
	write_line_to_file(L2TPCONF,2, line_buffer);
	
	sprintf(line_buffer,"%s\n", "pppoptfile = /etc/ppp/options");
	write_line_to_file(L2TPCONF, 2, line_buffer);

	RunSystemCmd(NULL_FILE, "killall", "l2tpd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "ppp_inet", NULL_STR);
	sleep(1);
	//RunSystemCmd(NULL_FILE, "l2tpd", NULL_STR);	
	system("l2tpd&");
	sleep(3);
	
	apmib_get(MIB_L2TP_CONNECTION_TYPE, (void *)&connect_type);
	if(connect_type==1){

		RunSystemCmd(NULL_FILE, "route", "del", "default", NULL_STR);
		RunSystemCmd(NULL_FILE, "route", "add", "default", "gw", "10.112.112.112", wan_iface, NULL_STR);
			
		sprintf(line_buffer,"%s\n", "connect /etc/ppp/true");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
		sprintf(line_buffer,"%s\n", "demand");
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
		apmib_get(MIB_L2TP_IDLE_TIME, (void *)&idle_time);
		sprintf(line_buffer,"idle %d\n", idle_time);
		write_line_to_file(PPP_OPTIONS_FILE1,2, line_buffer);
			
		}else if(connect_type==2 && act_source==1 && !isFileExist(MANUAL_CONNECT_NOW)) //manual mode we do not dial up from init.sh
				return;
			
	#if 0
		apmib_get( MIB_DNS_MODE, (void *)&intValue1);
		if(intValue1==1){
			start_dns_relay();
		}else{
			RunSystemCmd(NULL_FILE, "dnrd", "--cache=off", "-s", "168.95.1.1",NULL_STR);
		}
	#endif	
		if(isFileExist(PPP_FILE)){
			unlink(PPP_FILE);
		} 
		sprintf(tmp_args, "%s", "6");/*wan type*/
		sprintf(tmp_args1, "%d", connect_type);/*connect type*/
		RunSystemCmd(NULL_FILE, "ppp_inet", "-t", tmp_args,  "-c", tmp_args1, "-x", NULL_STR);
		start_upnp_igd(L2TP, sys_op, wisp_id, lan_iface);
}
void domain2ip(int wan_type)
{	
	unsigned char server_domain[32];
	struct in_addr server_ip;
	int enable_server_domain=0;

	if(wan_type!=PPTP && wan_type!=L2TP)
		return;
	
	if(wan_type==PPTP)
		apmib_get(MIB_PPTP_GET_SERV_BY_DOMAIN,(void*)&enable_server_domain);
	else if(wan_type==L2TP)
		apmib_get(MIB_L2TP_GET_SERV_BY_DOMAIN,(void*)&enable_server_domain);
	
	if(enable_server_domain)
	{	
		if(wan_type==PPTP)
			apmib_get(MIB_PPTP_SERVER_DOMAIN, server_domain);
		else if(wan_type==L2TP)
			apmib_get(MIB_L2TP_SERVER_DOMAIN, server_domain);
		
		if(translate_domain_to_ip(server_domain, &server_ip) == 0)
		{			
			if(wan_type==PPTP)
				apmib_set(MIB_PPTP_SERVER_IP_ADDR, (void *)&server_ip);
			else if(wan_type==L2TP)
				apmib_set(MIB_L2TP_SERVER_IP_ADDR, (void *)&server_ip);
			
			apmib_update(CURRENT_SETTING);
		}
		else
		{
			printf("can't get ServerDomain:%s 's IP",server_domain);
			return 0;
		}
	}
}
int start_wan(int wan_mode, int sys_op, char *wan_iface, char *lan_iface, int wisp_id, int act_source)
{
	int lan_type=0;
#if defined(CONFIG_DYNAMIC_WAN_IP)
	int pptp_wanip_dynamic=0, l2tp_wanip_dynamic=0;
#endif
	printf("Init WAN Interface...\n");
	//RunSystemCmd(NULL_FILE, "ifconfig", NULL_STR);
	//RunSystemCmd(NULL_FILE, "brctl","show",NULL_STR);
	
	if(wan_mode == DHCP_DISABLED)
		set_staticIP(sys_op, wan_iface, lan_iface, wisp_id, act_source);
	else if(wan_mode == DHCP_CLIENT)
		set_dhcp_client(sys_op, wan_iface, lan_iface, wisp_id, act_source);
	else if(wan_mode == PPPOE){
		int sessid = 0;
		char cmdBuf[50],tmpBuff[30];
		memset(tmpBuff,0, sizeof(tmpBuff));
		apmib_get(MIB_PPP_SESSION_NUM, (void *)&sessid);
		apmib_get(MIB_PPP_SERVER_MAC,  (void *)tmpBuff);

		sprintf(cmdBuf,"flash clearppp %d %02x%02x%02x%02x%02x%02x",sessid,(unsigned char)tmpBuff[0],(unsigned char)tmpBuff[1],(unsigned char)tmpBuff[2],(unsigned char)tmpBuff[3],(unsigned char)tmpBuff[4],(unsigned char)tmpBuff[5]);
		system(cmdBuf);
		sleep(2);	// Wait util pppoe server reply PADT, then start pppoe dialing, otherwise pppoe server will reply PADS with PPPoE tags: Generic-Error.
		
		//RunSystemCmd(NULL_FILE, "pppoe.sh", "all", wan_iface, NULL_STR);
		system("ifconfig eth5 up");

		//vid 50
		//system("echo 1 0 1 1 50 1 0 > /proc/eth1/mib_vlan_info");
		//system("echo 1 1 1 0 50 1 0 > /proc/eth2/mib_vlan_info");
		//vid 51
		//system("echo 1 0 1 1 51 1 0 > /proc/eth5/mib_vlan_info");
		//system("echo 1 1 1 0 51 1 0 > /proc/eth3/mib_vlan_info");
		
		//wan_iface = "eth1";		
		set_pppoe(sys_op, wan_iface, lan_iface, wisp_id, act_source);
	}else if(wan_mode == PPTP){
#if defined(CONFIG_DYNAMIC_WAN_IP)
		apmib_get(MIB_PPTP_WAN_IP_DYNAMIC, (void *)&pptp_wanip_dynamic);
		if(pptp_wanip_dynamic==STATIC_IP){
			set_staticIP(sys_op, wan_iface, lan_iface, wisp_id, act_source);
#if defined(CONFIG_GET_SERVER_IP_BY_DOMAIN)	
			domain2ip(wan_mode);
#endif
			set_pptp(sys_op, wan_iface, lan_iface, wisp_id, act_source);
		}else{
			RunSystemCmd(TEMP_WAN_CHECK, "echo", "dhcpc", NULL_STR);
			RunSystemCmd(NULL, "rm -rf", MANUAL_CONNECT_NOW, " 2>/dev/null",  NULL_STR);
			if(act_source == 0)
				RunSystemCmd(MANUAL_CONNECT_NOW, "echo",  "1", NULL_STR);
			set_dhcp_client(sys_op, wan_iface, lan_iface, wisp_id, act_source);
		}
#else
		set_pptp(sys_op, wan_iface, lan_iface, wisp_id, act_source);
#endif
		//RunSystemCmd(NULL_FILE, "pptp.sh", wan_iface, NULL_STR);
	}else if(wan_mode == L2TP){
		//RunSystemCmd(NULL_FILE, "l2tp.sh", wan_iface, NULL_STR);
#if defined(CONFIG_DYNAMIC_WAN_IP)
		apmib_get(MIB_L2TP_WAN_IP_DYNAMIC, (void *)&l2tp_wanip_dynamic);
		if(l2tp_wanip_dynamic==STATIC_IP){
			set_staticIP(sys_op, wan_iface, lan_iface, wisp_id, act_source);			
#if defined(CONFIG_GET_SERVER_IP_BY_DOMAIN)	
			domain2ip(wan_mode);
#endif
			set_l2tp(sys_op, wan_iface, lan_iface, wisp_id, act_source);
		}else{
			RunSystemCmd(TEMP_WAN_CHECK, "echo", "dhcpc", NULL_STR);	
			RunSystemCmd(NULL, "rm -rf", MANUAL_CONNECT_NOW, " 2>/dev/null", NULL_STR);
			if(act_source == 0)
				RunSystemCmd(MANUAL_CONNECT_NOW, "echo", "1", NULL_STR);
			set_dhcp_client(sys_op, wan_iface, lan_iface, wisp_id, act_source);
		}
#else
		set_l2tp(sys_op, wan_iface, lan_iface, wisp_id, act_source);
#endif
	}	
	apmib_get(MIB_DHCP,(void*)&lan_type);
	if(lan_type == DHCP_CLIENT)
	{//when set lan dhcp client,default route should get from lan dhcp server.
	//otherwise,DHCP offer pocket from dhcp server would be routed to wan(default gw),and client can't complete dhcp
		RunSystemCmd(NULL_FILE, "route", "del", "default", wan_iface, NULL_STR);
	}
	return 0;
}

 
