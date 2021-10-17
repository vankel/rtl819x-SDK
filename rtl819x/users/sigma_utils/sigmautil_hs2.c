//=============================================================================
// Copyright (c) 2013 Realtek Semiconductor Corporation.	All Rights Reserved.
//
//	Title:
//		Sigmautil_hs2.c
//	Desc:
//		Main Program for HS2.0 Release 1 WFA Sigma Test
//=============================================================================


#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <linux/wireless.h>
#include "sigmautil.h"

char ifname[20];
char sec_enable;
unsigned int dbglevel;
char is8021x;
char isOSEN;
// NAI Realm List
char NAI_Realm[7][450]={
	//"NAIRealmData1={\n\tNAIRealm=mail.example.com\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\nNAIRealmData2={\n\tNAIRealm=cisco.com\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\nNAIRealmData3={\n\tNAIRealm=wi-fi.org\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n\tEAPMethod2=EAP-TLS\n\tAuthParam1=5;6\n}\nNAIRealmData4={\n\tNAIRealm=example.com\n\tEAPMethod1=EAP-TLS\n\tAuthParam1=5;6\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\n",
	"NAIRealmData1={\n\tNAIRealm=mail.example.com\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\nNAIRealmData2={\n\tNAIRealm=cisco.com\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\nNAIRealmData3={\n\tNAIRealm=wi-fi.org\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n\tEAPMethod2=EAP-TLS\n\tAuthParam1=5;6\n}\nNAIRealmData4={\n\tNAIRealm=example.com\n\tEAPMethod1=EAP-TLS\n\tAuthParam1=5;6\n}\n",
	"NAIRealmData1={\n\tNAIRealm=wi-fi.org\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\n",
	"NAIRealmData1={\n\tNAIRealm=cisco.com\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\nNAIRealmData2={\n\tNAIRealm=wi-fi.org\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n\tEAPMethod2=EAP-TLS\n\tAuthParam1=5;6\n}\nNAIRealmData3={\n\tNAIRealm=example.com\n\tEAPMethod1=EAP-TLS\n\tAuthParam1=5;6\n}\n",
	"NAIRealmData1={\n\tNAIRealm=mail.example.com\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\nNAIRealmData2={\n\tNAIRealm=mail.example.com\n\tEAPMethod1=EAP-TLS\n\tAuthParam1=5;6\n}\n",
	"NAIRealmData1={\n\tNAIRealm=wi-fi.org\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n\tEAPMethod2=EAP-TLS\n\tAuthParam1=5;6\n}\nNAIRealmData2={\n\tNAIRealm=ruckuswireless.com\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\n",
	"NAIRealmData1={\n\tNAIRealm=wi-fi.org\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n\tEAPMethod2=EAP-TLS\n\tAuthParam1=5;6\n}\nNAIRealmData2={\n\tNAIRealm=mail.example.com\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n}\n",
	"NAIRealmData1={\n\tNAIRealm=wi-fi.org\n\tEAPMethod1=EAP-TTLS\n\tAuthParam1=2;4\n\tAuthParam2=5;7\n\tEAPMethod2=EAP-TLS\n\tAuthParam1=5;6\n}\n"
};
// Multi-band network configuration: 2.4GHz / 5GHz
// 0x51: 2.4G, 0x73: 5G, 5173: 2.4G and 5G
char OperClass[3][40]={
	"operation_band=51\n",
	"operation_band=73\n",
	"operation_band=5173\n",
};
// Sigma Enabled
char sig_Default[]={"sigma_test=1\n"};
// 3GPP Cellular Network Information
char PLMN_list[]={"PLMN1={\n\tMCC=310\n\tMNC=026\n}\nPLMN2={\n\tMCC=208\n\tMNC=00\n}\nPLMN3={\n\tMCC=208\n\tMNC=01\n}\nPLMN4={\n\tMCC=208\n\tMNC=02\n}\nPLMN5={\n\tMCC=450\n\tMNC=02\n}\nPLMN6={\n\tMCC=450\n\tMNC=04\n}\n"};
// Domain Name List
//unsigned char domainName[]={"domainname=wi-fi.org\n"};
// Operator Friendly Name
char opFName[]={"operatorfriendlyname=id1\n"};
// IP Address Type Availability Information
char IPAvail[]={"ipv4type=3\nipv6type=0\n"};
// WAN Metrics
char WAN_Metrics[5][150]={"wan_metrics={\n\tlink_status=1\n\tat_capacity=0\n\tdl_speed=2500\n\tul_speed=384\n\tdl_load=0\n\tup_load=0\n\tlmd=0\n}\n",
						  "wan_metrics={\n\tlink_status=1\n\tat_capacity=0\n\tdl_speed=1500\n\tul_speed=384\n\tdl_load=20\n\tup_load=20\n\tlmd=0\n}\n",
						  "wan_metrics={\n\tlink_status=1\n\tat_capacity=0\n\tdl_speed=2000\n\tul_speed=1000\n\tdl_load=20\n\tup_load=20\n\tlmd=0\n}\n",
						  "wan_metrics={\n\tlink_status=1\n\tat_capacity=0\n\tdl_speed=8000\n\tul_speed=1000\n\tdl_load=20\n\tup_load=20\n\tlmd=0\n}\n",
						  "wan_metrics={\n\tlink_status=1\n\tat_capacity=0\n\tdl_speed=9000\n\tul_speed=5000\n\tdl_load=20\n\tup_load=20\n\tlmd=0\n}\n"
};
//Anonymous NAI
char AnonyNAI[]={"Anonymous_NAI=mail.example.com\n"};

char ADVID[]={"advertisementproid=0\n"};
// Info of WNM Notification
char RemedServer[]={"REMED_SERVER=https://remediation-server.R2-testbed.wi-fi.org\n"}; 
// Info of Interworking element
char IW_INFO[]={"internet=0\nvenuegroup=2\nvenuetype=8\n"}; 
// Venue Name Information
char VEN_INFO[]={"venuename=id1\n"}; 
//char PROTO_PORT[]={"proto_port=id1\n"}; // Connection Capability

char OSU_Provider_end[9][250]={"\tOSUMethodList=1\n\tIconMetadata1=128;61;zxx;image/png;icon_red_zxx.png\n\tIconMetadata1=160;76;eng;image/png;icon_red_eng.png\n\tOSU_SRV_Desc1=eng;Free service for test purpose\n\tOSU_SRV_Desc2=kor;테스트 목적으로 무료 서비스\n}\n",
							   "\tOSUMethodList=1\n\tIconMetadata1=128;61;zxx;image/png;icon_orange_zxx.png\n\tOSU_SRV_Desc1=eng;Free service for test purpose\n\tOSU_SRV_Desc2=kor;Free service for test purpose\n}\n",
							   "\tOSUMethodList=1\n\tIconMetadata1=128;61;spa;image/png;icon_red_zxx.png\n\tOSU_SRV_Desc1=eng;Free service for test purpose\n\tOSU_SRV_Desc2=kor;Free service for test purpose\n}\n",
							   "\tOSUMethodList=1\n\tIconMetadata1=128;128;eng;image/png;icon_red.png\n\tIconMetadata2=128;128;zxx;image/png;icon_red.png\n\tOSU_SRV_Desc1=eng;Free service for test purpose\n\tOSU_SRV_Desc2=kor;Free service for test purpose\n}\n",
							   "\tOSUMethodList=1\n\tIconMetadata1=128;128;eng;image/png;WiFi-alliancelogo_3D.png\n\tOSU_NAI=test-anonymous@wi-fi.org\n}\n",
							   "\tOSUMethodList=1\n\tIconMetadata1=128;128;eng;image/png;WiFi-alliancelogo_3D.png\n\tOSU_SRV_Desc1=eng;Free service for test purpose\n\tOSU_SRV_Desc2=kor;Free service for test purpose\n}\n", 
							   "",
							   "",
							   "\tOSUMethodList=1\n\tIconMetadata1=128;61;zxx;image/png;icon_orange_zxx.png\n\tOSU_NAI=test-anonymous@wi-fi.org\n}\n"
};
char OSU_Provider_begin[9][200]={"MBSSID_CAP=0\nOSUProvider1={\n\tOSU_Friendly_Name1=eng;SP Red Test Only\n\tOSU_Friendly_Name2=kor;SP ��강 테스��� 전용\n\t",
								 "MBSSID_CAP=0\nOSUProvider1={\n\tOSU_Friendly_Name1=eng;Wireless Broadband Alliance\n\tOSU_Friendly_Name2=kor;Wireless Broadband Alliance\n\t",
								 "MBSSID_CAP=0\nOSUProvider1={\n\tOSU_Friendly_Name1=eng;SP Red Test Only\n\tOSU_Friendly_Name2=kor;Wi-Fi Alliance OSU\n\t",
								 "MBSSID_CAP=0\nOSUProvider1={\n\tOSU_Friendly_Name1=eng;SP Red Test Only OSU\n\t",
								 "MBSSID_CAP=0\nOSUProvider1={\n\tOSU_Friendly_Name1=eng;Wi-Fi Alliance\n\t",
								 "MBSSID_CAP=0\nOSUProvider1={\n\tOSU_Friendly_Name1=eng;Wi-Fi Alliance OSU\n\tOSU_Friendly_Name2=kor;Wi-Fi Alliance OSU\n\t",
								 "",
								 "",
								 "MBSSID_CAP=0\nOSUProvider1={\n\tOSU_Friendly_Name1=eng;SP Orange Test Only\n\t"
};

char QoSMAP[2][100]={"QOSMAP1={\n\tDSCPEpt=53;2;22;6\n\tDSCPRange=8;15;0;7;255;255;16;31;32;39;255;255;40;47;255;255\n}\n",
				  "QOSMAP2={\n\tDSCPRange=8;15;0;7;255;255;16;31;32;39;255;255;40;47;48;63\n}\n"	};

void __inline__ WRITE_WPA_FILE(int fh, unsigned char *buf)
{	
	if ( write(fh, buf, strlen((char *)buf)) != strlen((char *)buf) ) {
		printf("Write WPA config file error!\n");
		close(fh);
		exit(1);
	}
}

int wlioctl_QoSMapConfigure(char *iface, char *mac, unsigned char index) 
{
	int retVal = 0;
	struct iwreq          	wrq;
	DOT11_QoSMAPConf	QoSMAPConf;
	int i;
	int skfd;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0) {
		printf("socket() error!\n");
		return -1;
	}
	
	strncpy(wrq.ifr_name, iface, IFNAMSIZ);
	wrq.u.data.pointer = (caddr_t)&QoSMAPConf;
	wrq.u.data.length = sizeof(QoSMAPConf);
	QoSMAPConf.EventId = DOT11_EVENT_QOS_MAP_CONF;
	QoSMAPConf.IsMoreEvent = 0;

	memcpy(QoSMAPConf.macAddr,mac,6);	
	QoSMAPConf.indexQoSMAP = index;
		
	if(ioctl(skfd, SIOCGIWIND, &wrq) < 0)
        // If no wireless name : no wireless extensions
                retVal = -1;
	else
		retVal = 0;

    return retVal;
}

int wlioctl_SessionInfo_URL(char *iface, char *mac, unsigned char SWT, unsigned char * URL)
{
	int retVal = 0;
	struct iwreq          	wrq;
	DOT11_BSS_SessInfo_URL	SessInfo_URL;
	int i;
	int skfd;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (skfd < 0) {
		printf("socket() error!\n");
		return -1;
	}
	strncpy(wrq.ifr_name, iface, IFNAMSIZ);
	wrq.u.data.pointer = (caddr_t)&SessInfo_URL;
	wrq.u.data.length = sizeof(DOT11_BSS_SessInfo_URL);
	SessInfo_URL.EventId = DOT11_EVENT_HS2_TSM_REQ;
	SessInfo_URL.IsMoreEvent = 0;
	memcpy(SessInfo_URL.macAddr,mac,6);	
	SessInfo_URL.SWT = SWT;
	if(URL)
		strcpy(SessInfo_URL.URL, URL);
	else
		SessInfo_URL.URL[0] = '\0';

	if(ioctl(skfd, SIOCGIWIND, &wrq) < 0)
        // If no wireless name : no wireless extensions
                retVal = -1;
	else
		retVal = 0;

        return retVal;
}

static void generateWpaConf2(int argc, char** argv)
{
	int fh ;
	int idx=0;
	char buf1[1200];	
	char buf2[100];
	char* pStr=NULL;

	char cfile[30];
	sprintf(cfile,"/var/sigma-%s-wpa.conf",ifname);
	
	fh = open(cfile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("[%s %d]Create WPA config file error!\n",__FUNCTION__,__LINE__);
		return;
	}

	printf("[%s %d]\n",__FUNCTION__,__LINE__);	

	for(idx=0;idx<argc;idx++){
		printf("[%s %d]argv:%s\n",__FUNCTION__,__LINE__,argv[idx]);
	}

	/*
	for example:
	flash startAuth ssid,hello,encrypt,6,wpa,3,wpa2,3,rsip,192.168.100.20,rsport,1997,rspw,667788
	*/
	
	pStr = strtok(argv[3],",");
	while(pStr != 0)
	{
		if(!strcmp(pStr,"encrypt")){

		    pStr = strtok(NULL,",");/*encryption=6(WPA + WPA2 mixed);WPA=2 ; WPA2=4 */
			sprintf(buf2, "encryption = %s\n", pStr);
			strcat(buf1,  buf2);
			
		}else if(!strcmp(pStr,"wpa")){
		
		    pStr = strtok(NULL,",");//WPA Cipher(T=1,A=2,T+A=3)
			sprintf(buf2, "unicastCipher = %s\n", pStr);
			strcat(buf1,  buf2);
			
		}else if(!strcmp(pStr,"wpa2")){
		    pStr = strtok(NULL,",");//WPA2 Cipher(T=1,A=2,T+A=3)
			sprintf(buf2, "wpa2UnicastCipher = %s\n", pStr);
			strcat(buf1,  buf2);
		}else if(!strcmp(pStr,"rsip")){

		    pStr = strtok(NULL,",");	/*Radius Server IP*/
			sprintf(buf2, "rsIP = %s\n",pStr);
			strcat(buf1,  buf2);
		}else if(!strcmp(pStr,"rsport")){

		    pStr = strtok(NULL,",");	/*Radius Server port*/
			sprintf(buf2, "rsPort = %s\n", pStr);
			strcat(buf1,  buf2);
		}else if(!strcmp(pStr,"rspw")){
		    pStr = strtok(NULL,",");	/*Radius Server password*/
			sprintf(buf2, "rsPassword = \"%s\"\n", pStr);
			strcat(buf1,  buf2);
		}else if(!strcmp(pStr,"ssid")){
		    pStr = strtok(NULL,",");	/*ssid*/		
			sprintf(buf2, "ssid = \"%s\"\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"au")){
		    pStr = strtok(NULL,",");	/*authentication*/		
			sprintf(buf2, "authentication = %d\n", atoi(pStr));
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"pmf")){
		    pStr = strtok(NULL,",");	/*authentication*/		
			sprintf(buf2, "ieee80211w = %d\n", atoi(pStr));
			strcat(buf1,  buf2);			
		}else{
			printf("[%s %d]unknown cmd:%s\n",__FUNCTION__,__LINE__,pStr);
		    pStr = strtok(NULL,",");
			printf("[%s %d]value:%s\n",__FUNCTION__,__LINE__,pStr);			
		}	   
	    pStr = strtok(NULL,",");
	}	

	strcat(buf1,  "enable1x = 1\n");
	strcat(buf1,  "enableMacAuth = 0\n");	
	strcat(buf1,  "supportNonWpaClient = 0\n");	
	
	// 64bits=1 ; 128bits=2
	strcat(buf1,  "wepKey = 1\n");		

#if 0	// now no handle Ent-WEP case
	if ( encrypt==1 && enable1x ) {
		if (wep == 1) {
			apmib_get( MIB_WLAN_WEP64_KEY1, (void *)buf1);
			sprintf(buf2, "wepGroupKey = \"%02x%02x%02x%02x%02x\"\n", buf1[0],buf1[1],buf1[2],buf1[3],buf1[4]);
		}
		else {
			apmib_get( MIB_WLAN_WEP128_KEY1, (void *)buf1);
			sprintf(buf2, "wepGroupKey = \"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x\"\n",
				buf1[0],buf1[1],buf1[2],buf1[3],buf1[4],
				buf1[5],buf1[6],buf1[7],buf1[8],buf1[9],
				buf1[10],buf1[11],buf1[12]);
		}
	}
	else{
		strcpy(buf2, "wepGroupKey = \"\"\n");
	}
#endif	
			
	strcat(buf1,  "rsReAuthTO = 0\n");
	strcat(buf1,  "wepGroupKey = \"\"\n");			
	//strcat(buf1,  "authentication = 1\n");			
	strcat(buf1,  "enablePreAuth = 0\n");		
	strcat(buf1,  "usePassphrase = 1\n");			
	strcat(buf1,  "psk = \"\"\n");	
	strcat(buf1,  "groupRekeyTime = 86400\n");			
	strcat(buf1,  "rsMaxReq = 3\n");			
	strcat(buf1,  "rsAWhile = 5\n");				
	strcat(buf1,  "accountRsEnabled = 0\n");			
	strcat(buf1,  "accountRsPort = 0\n");			
	strcat(buf1,  "accountRsIP = 0.0.0.0\n");			
	strcat(buf1,  "accountRsPassword = \"\"\n");			
	strcat(buf1,  "accountRsUpdateEnabled = 0\n");			
	strcat(buf1,  "accountRsUpdateTime = 0\n");			
	strcat(buf1,  "accountRsMaxReq = 0\n");			
	strcat(buf1,  "accountRsAWhile = 0\n");

	WRITE_WPA_FILE(fh, buf1);
	close(fh);

}

static unsigned char char2num(char chr)
{
	if(chr >= 'a' && chr <= 'f') {
		printf("val=%x\n",chr - 'a' + 10);
		return chr - 'a' + 10;
	}
	if(chr >= 'A' && chr <= 'F') {
		printf("val=%x\n",chr - 'A' + 10);
		return chr - 'A' + 10;
	}
	if(chr >= '0' && chr <= '9') {
		printf("val=%x\n",chr - '0');
		return chr - '0';
	}

	printf("error chr=");
	printf("%x\n",chr);
	return 0;
}
static unsigned char * macStr2num(char *mac)
{
	int i;
	printf("mac=%s\n",mac);
	if(strlen(mac)==17) {
		mac[0] = (char2num(mac[0]) << 4) + char2num(mac[1]);
		mac[1] = (char2num(mac[3]) << 4) + char2num(mac[4]);
		mac[2] = (char2num(mac[6]) << 4) + char2num(mac[7]);
		mac[3] = (char2num(mac[9]) << 4) + char2num(mac[10]);
		mac[4] = (char2num(mac[12]) << 4) + char2num(mac[13]);
		mac[5] = (char2num(mac[15]) << 4) + char2num(mac[16]);
	} else {
		memset(mac,0,6);
	}
	return mac;
}
static void generateHS2Conf(int argc, char** argv)
{
	int fh ;
	int idx=0;
	unsigned char buf1[1000];	
	unsigned char buf2[100];
	unsigned char* pStr=NULL;
	unsigned char mnc[30],mcc[30];
	unsigned char cfile[30];
	unsigned char indexOP;
	unsigned char strNAI[200];

	sprintf(cfile,"/tmp/hs2-%s.conf",ifname);
		
	fh = open(cfile, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("[%s %d]Create HS2 config file error!\n",__FUNCTION__,__LINE__);
		return;
	}


	printf("[%s %d]\n",__FUNCTION__,__LINE__);	

	for(idx=0;idx<argc;idx++){
		printf("[%s %d]argv:%s\n",__FUNCTION__,__LINE__,argv[idx]);
	}


	sprintf(buf1, "interface=%s\n", ifname);	
	strcat(buf1,  sig_Default);
	WRITE_WPA_FILE(fh, buf1);
	
	// sigmautil wlan0 startHS2 ar=1,ms=600,cd=4,dd=0,RN=1,es=1,l2=0,rd=mac addr,hi=mac addr,NR=1,OC=1,DL=wi-fi.org,IW,1,RC,HS2V_RoamingCons,IE,$HS2V_ICMPv4ECHO,IF,interfacename,mn,$mnc,mc,$mcc,1x,0,NT,0
// ar: Proxy ARP, ms: mpdu_size, cd: comeback delay, dd:dgaf_disabled, R: Release Number
// ae: anqp_enable, l2: l2_traffic_inspect, gc: enableGASComeback
// rd: redirect_dst, hi=hessid, NR: NAI Realm, OC: Operation Class, DL: DomainList, IW: Interworking
// AT: Access Network Type (0-15, 2: charageable public network), IE: ICMPv4Echo
	buf1[0]='\0';
	pStr = strtok(argv[3],",");
	while(pStr != 0)
	{
		if(!strcmp(pStr,"ar")){
		  	pStr = strtok(NULL,",");
			sprintf(buf2, "proxy_arp=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"1x")){ // if 802.1x
		  	pStr = strtok(NULL,",");		
		  	is8021x = atoi(pStr);
		}else if(!strcmp(pStr,"ON")){ // if OSEN
		  	pStr = strtok(NULL,",");		
		  	isOSEN = atoi(pStr);
		}else if(!strcmp(pStr,"ms")){ //mpdu_size
		  	pStr = strtok(NULL,",");	
			sprintf(buf2, "mmpdu_size=%s\n",pStr);
			strcat(buf1,  buf2);
		}else if(!strcmp(pStr,"gc")){ //enableGASComeback
		  	pStr = strtok(NULL,",");	
			sprintf(buf2, "enableGASComeback=%s\n",pStr);
			strcat(buf1,  buf2);
		}else if(!strcmp(pStr,"cd")){ //comeback delay
		  	pStr = strtok(NULL,",");
			sprintf(buf2, "comeback_delay=%s\n", pStr);
			strcat(buf1,  buf2);
		}else if(!strcmp(pStr,"dd")){
		  	pStr = strtok(NULL,",");
			sprintf(buf2, "dgaf_disabled=%s\n", pStr);
			strcat(buf1,  buf2);
		}else if(!strcmp(pStr,"RN")){
		  	pStr = strtok(NULL,",");	
			sprintf(buf2, "ReleaseNumber=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"ae")){
		  	pStr = strtok(NULL,",");	
			sprintf(buf2, "anqp_enable=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"l2")){
		  	pStr = strtok(NULL,",");		
			sprintf(buf2, "l2_traffic_inspect=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"IW")){
		  	pStr = strtok(NULL,",");			
			sprintf(buf2, "interworking=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"AT")){
		  	pStr = strtok(NULL,",");			
			sprintf(buf2, "access_network_type=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"mn")){
		  	pStr = strtok(NULL,",");			
			strcpy(mnc,pStr);
		}else if(!strcmp(pStr,"mc")){
		  	pStr = strtok(NULL,",");			
			strcpy(mcc,pStr);
		}else if(!strcmp(pStr,"rd")){
		  	pStr = strtok(NULL,",");			
			sprintf(buf2, "redirect_dst=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"RC")){
	        pStr = strtok(NULL,",");			
			if(strcmp(pStr,"Disabled"))
			{
				sprintf(buf2, "roamingconsortiumoi=%s\n", pStr);
				strcat(buf1,  buf2);			
			}
		}else if(!strcmp(pStr,"IE")){
		  	pStr = strtok(NULL,",");			
			sprintf(buf2, "ICMPv4ECHO=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"NT")){
	        pStr = strtok(NULL,",");			
			if(!strcmp(pStr,"1"))
				strcpy(buf2, "networkauthtype=0;https://tandc-server.wi-fi.org\n");
			else if(!strcmp(pStr,"2"))
				strcpy(buf2, "networkauthtype=1\n");
			else
				printf("Network Authentication Type ID Error (Should be 1 or 2)\n");
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"hi")){
		  pStr = strtok(NULL,",");			
			sprintf(buf2, "hessid=%s\n", pStr);
			strcat(buf1,  buf2);			
		}else if(!strcmp(pStr,"NR")){ // NAI Realm
		  pStr = strtok(NULL,",");		
			strcat(buf1,  NAI_Realm[atoi(pStr)-1]);			
		}
		//else if(!strcmp(pStr,"QM")){ // QoS MAP
		//  	pStr = strtok(NULL,",");		
		//	strcat(buf1,  QoSMAP[atoi(pStr)-1]);			
		//}
		else if(!strcmp(pStr,"OC")){
	        pStr = strtok(NULL,",");
			strcat(buf1,  OperClass[atoi(pStr)-1]);				
		}else if(!strcmp(pStr,"WA")){
	        pStr = strtok(NULL,",");			
			strcat(buf1,  WAN_Metrics[atoi(pStr)-1]);	
		}else if(!strcmp(pStr,"dg")){
			pStr = strtok(NULL,",");
		    dbglevel = atoi(pStr);
		}else if(!strcmp(pStr,"CP")){
	        pStr = strtok(NULL,",");			
			sprintf(buf2,"proto_port=id%d\n", atoi(pStr));
			strcat(buf1, buf2);
		}else if(!strcmp(pStr,"DL")) { // DomainList
			pStr = strtok(NULL,",");
			sprintf(buf2, "domainname=%s\n", pStr);
			strcat(buf1,buf2);
		} else if(!strcmp(pStr,"OS")) {
			pStr = strtok(NULL,",");
			sprintf(buf2,"L_OSU_SSID=%s\n", pStr);
			strcat(buf1,buf2);
		} else if(!strcmp(pStr,"OP")) {
			pStr = strtok(NULL,",");
			indexOP = atoi(pStr) - 1;
		} else if(!strcmp(pStr,"OU")) {
			pStr = strtok(NULL,",");
			strcat(buf1, OSU_Provider_begin[indexOP]);			
			sprintf(buf2,"OSU_URI=%s\n", pStr);
			strcat(buf1,buf2);			
			strcat(buf1, OSU_Provider_end[indexOP]);
		}		
		else{
			printf("[%s %d]unknown cmd:%s\n",__FUNCTION__,__LINE__,pStr);
		  	pStr = strtok(NULL,",");
			printf("[%s %d]value:%s\n",__FUNCTION__,__LINE__,pStr);			
		}	   
		WRITE_WPA_FILE(fh, buf1);
		buf1[0]='\0';
	    pStr = strtok(NULL,",");
	}		

	
	buf1[0]='\0';
				
	//strcat(buf1,  PLMN_list);		
	if(!strcmp(mnc,"026;00;01;02;02;04") && !strcmp(mcc,"310;208;208;208;450;450"))
		strcat(buf1,PLMN_list);
	else {		
		/*if(strlen(mcc) == 2) {
			mcc[2] = mcc[1];
			mcc[1] = mcc[0];
			mcc[0] = 0x3F;
			mcc[3] = '\0';
		}
		if(strlen(mnc) == 2) {
			mnc[2] = mnc[1];
			mnc[1] = mnc[0];
			mnc[0] = 0x3F;
			mnc[3] = '\0';
		}*/
		sprintf(buf2, "PLMN1={\n\tMCC=%s\n\tMNC=%s\n}\n",mcc,mnc);
		strcat(buf1, buf2);
	}
	strcat(buf1,  opFName);			
	strcat(buf1,  IPAvail);			
	strcat(buf1,  WAN_Metrics);			
	strcat(buf1,  AnonyNAI);				
	strcat(buf1,  ADVID);			
	strcat(buf1,  RemedServer);			
	strcat(buf1,  IW_INFO);			
	strcat(buf1,  VEN_INFO);
	strcat(buf1, QoSMAP[0]);
	strcat(buf1, QoSMAP[1]);	
	
	WRITE_WPA_FILE(fh, buf1);
	close(fh);

}

int main(int argc, char *argv[])
{
	char cmdbuf2[128];	

	is8021x = 1;
	isOSEN = 0;
	if ( argc > 1 ) {
		if(!strncmp(argv[1], "wlan",4)) {
			strcpy(ifname, argv[1]);
		}
		else {
			printf("The first argument should be interface name");
			return -1;
		}
		
		if(!strcmp(argv[2], "startAuth")){	/*sigma test*/

            printf("[%s %d]startAuth ...(sigma_utils)\n",__func__,__LINE__);
            
			sprintf(cmdbuf2,"killall -9 wscd");
			system(cmdbuf2);
			sprintf(cmdbuf2,"killall -9 auth");
			system(cmdbuf2);
			sprintf(cmdbuf2,"killall -9 iapp");
			system(cmdbuf2);						
			sprintf(cmdbuf2,"killall -9 hs2");
			system(cmdbuf2);	
			sprintf(cmdbuf2,"killall -9 iwcontrol");
			system(cmdbuf2);

			generateWpaConf2(argc,argv);
			
			sprintf(cmdbuf2,"auth %s br0 auth /var/sigma-%s-wpa.conf",ifname,ifname);
			system(cmdbuf2);

			//sprintf(cmdbuf2,"iapp br0 %s",ifname);
			//system(cmdbuf2);			
			
			sprintf(cmdbuf2,"hs2 -c /tmp/hs2-%s.conf",ifname);
			system(cmdbuf2);			
            sleep(1);
            //delay(1000);            
			sprintf(cmdbuf2,"iwcontrol %s",ifname);
			system(cmdbuf2);			


			return 0;			
		}
		else if(!strcmp(argv[2], "setAuth")){	/*sigma test*/
							
			generateWpaConf2(argc,argv);
			return 0;			
		}
		else if(!strcmp(argv[2], "disableIF")){	/*sigma test*/
			system("ifconfig wlan0 down");
			system("ifconfig wlan1 down");
			system("ifconfig wlan0-va0 down");
			system("ifconfig wlan1-va0 down");
		}
		else if(!strcmp(argv[2], "setWLAN")){
			
			sprintf(cmdbuf2,"iwpriv %s set_mib band=%s",argv[1],argv[3]);
			system(cmdbuf2);
			sprintf(cmdbuf2,"iwpriv %s set_mib phyBandSelect=%s",argv[1],argv[4]);
			system(cmdbuf2);
			sprintf(cmdbuf2,"iwpriv %s set_mib deny_legacy=%s",argv[1],argv[5]);
			system(cmdbuf2);
			sprintf(cmdbuf2,"iwpriv %s set_mib opmode=%s",argv[1],argv[6]);
			system(cmdbuf2);
			
		}
		else if(!strcmp(argv[2], "setSec")){
			sprintf(cmdbuf2,"iwpriv %s set_mib psk_enable=%s",argv[1],argv[3]);
			printf("%s\n",cmdbuf2);
			system(cmdbuf2);
			sprintf(cmdbuf2,"iwpriv %s set_mib wpa_cipher=%s",argv[1],argv[4]);
			printf("%s\n",cmdbuf2);
			system(cmdbuf2);
			sprintf(cmdbuf2,"iwpriv %s set_mib wpa2_cipher=%s",argv[1],argv[5]);
			printf("%s\n",cmdbuf2);
			system(cmdbuf2);
			sprintf(cmdbuf2,"iwpriv %s set_mib 802_1x=%s",argv[1],argv[6]);
			printf("%s\n",cmdbuf2);
			system(cmdbuf2);
			sprintf(cmdbuf2,"iwpriv %s set_mib encmode=%s",argv[1],argv[7]);
			printf("%s\n",cmdbuf2);
			system(cmdbuf2);
			
		}
		else if(!strcmp(argv[2], "sendFrame")){
			unsigned char *mac;
			FILE *in;
            printf("[%s %d]send Frame ...(sigma_utils)\n",__func__,__LINE__);

			if((in = fdopen("/tmp/sigmaIface", "r")) != NULL) {
				fgets(ifname, 20, in);
				fclose(in);
			}
			
			mac = macStr2num(argv[4]);
			printf("mac=%02x %02x %02x %02x %02x %02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
			if(!strcmp(argv[3],"BTMReq")) {
				printf("send BTMReq in %s..before\n", ifname);
				wlioctl_SessionInfo_URL(ifname,mac,atoi(argv[6]),argv[7]);				
			} else if(!strcmp(argv[3],"QoSMapConfigure")) {
				printf("send QoSMapConfigure in %s\n", ifname);
				wlioctl_QoSMapConfigure(ifname,mac,atoi(argv[5])-1);
			}
		}
		else if(!strcmp(argv[2], "startHS2")){	/*sigma test*/	
            printf("[%s %d]start HS2 ...(sigma_utils)\n",__func__,__LINE__);
            
			if(is8021x){
				sprintf(cmdbuf2,"killall -9 auth");
				system(cmdbuf2);
			}
			sprintf(cmdbuf2,"killall -9 wscd");
			system(cmdbuf2);
			sprintf(cmdbuf2,"killall -9 iapp");
			system(cmdbuf2);
			sprintf(cmdbuf2,"killall -9 hs2");
			system(cmdbuf2);			
			sprintf(cmdbuf2,"killall -9 iwcontrol");
			system(cmdbuf2);
			
			sprintf(cmdbuf2,"rm -rf /var/run/auth*.pid /var/run/iwcontrol.pid /var/run/hs2_*.pid /var/run/wscd*.pid");
			system(cmdbuf2);

			generateHS2Conf(argc,argv);

			if(is8021x) {
				sprintf(cmdbuf2,"auth %s br0 auth /var/sigma-%s-wpa.conf > /dev/ttyS0",ifname,ifname);
				printf("startHS2: %s\n",cmdbuf2);
				system(cmdbuf2);
			}
			if(isOSEN) {
				sprintf(cmdbuf2,"auth %s-va0 br0 auth /var/sigma-%s-va0-wpa.conf > /dev/ttyS0",ifname,ifname);
				printf("startHS2: %s\n",cmdbuf2);
				system(cmdbuf2);
			}
			sprintf(cmdbuf2,"hs2 -c /tmp/hs2-%s.conf -d %d > /dev/ttyS0",ifname,dbglevel);
			printf("startHS2: %s\n",cmdbuf2);
			system(cmdbuf2);

            sleep(1);
            
			sprintf(cmdbuf2,"iwcontrol wlan0 wlan1 wlan0-va0 wlan1-va0  > /dev/ttyS0");
			system(cmdbuf2);	
			sprintf(cmdbuf2,"echo %s > /tmp/sigmaIface",ifname);
			system(cmdbuf2);
		}
		else if(!strcmp(argv[2], "restartHS2")){	/*sigma test*/			
			FILE *in;

			if((in = fopen("/tmp/sigmaIface", "r")) != NULL) {
				fgets(ifname, 20, in);
				fclose(in);
			} else {
				printf("open /tmp/sigmaIface fail\n");
			}
			if(strlen(ifname) == 6)
				ifname[5] = '\0';
			else if(strlen(ifname) == 10)
				ifname[9] = '\0';
			else
				printf("ifname length error\n");
			printf("restartHS2 %s\n",ifname);
            
			sprintf(cmdbuf2,"ifconfig %s up",ifname);
			system(cmdbuf2);
			
			if(is8021x){
				sprintf(cmdbuf2,"killall -9 auth");
				system(cmdbuf2);
			}
			sprintf(cmdbuf2,"killall -9 hs2");
			system(cmdbuf2);			
			sprintf(cmdbuf2,"killall -9 iwcontrol");
			system(cmdbuf2);
			
			sprintf(cmdbuf2,"rm -rf /var/run/auth*.pid /var/run/iwcontrol.pid /var/run/hs2_*.pid /var/run/wscd*.pid");
			system(cmdbuf2);

			if(is8021x) {
				sprintf(cmdbuf2,"auth %s br0 auth /var/sigma-%s-wpa.conf > /dev/ttyS0",ifname,ifname);
				printf("restartHS2:%s\n", cmdbuf2);
				system(cmdbuf2);
			}
			sprintf(cmdbuf2,"hs2 -c /tmp/hs2-%s.conf > /dev/ttyS0",ifname);
			printf("restartHS2: %s\n",cmdbuf2);
			system(cmdbuf2);


            sleep(1);
            
			sprintf(cmdbuf2,"iwcontrol wlan0 wlan1 wlan0-va0 wlan1-va0 > /dev/ttyS0");
			system(cmdbuf2);			
		}
	}
	return 0;
}
