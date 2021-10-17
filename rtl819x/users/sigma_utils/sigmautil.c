//=============================================================================
// Copyright (c) 2013 Realtek Semiconductor Corporation.	All Rights Reserved.
//
//	Title:
//		Sigmautil.c
//	Desc:
//		Main Program for IEEE 802.11ac WFA Sigma Test
//=============================================================================

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>



#define SIGMA_OUTPUTFILE "/var/sigma-wlan0-wpa.conf"

static void __inline__ WRITE_WPA_FILE(int fh, unsigned char *buf)
{
	if ( write(fh, buf, strlen((char *)buf)) != strlen((char *)buf) ) {
		printf("Write WPA config file error!\n");
		close(fh);
		exit(1);
	}
}
static void generateWpaConf2(int argc, char** argv)
{
	int fh ;
	int idx=0;
	unsigned char buf1[1200];	
	unsigned char buf2[100];
	unsigned char* pStr=NULL;

#if 1
	fh = open(SIGMA_OUTPUTFILE, O_RDWR|O_CREAT|O_TRUNC);
	if (fh == -1) {
		printf("[%s %d]Create WPA config file error!\n",__FUNCTION__,__LINE__);
		return;
	}
#endif

	printf("[%s %d]\n",__FUNCTION__,__LINE__);	

	for(idx=0;idx<argc;idx++){
		printf("[%s %d]argv:%s\n",__FUNCTION__,__LINE__,argv[idx]);
	}

	/*
	for example:
	flash startAuth ssid,hello,encrypt,6,wpa,3,wpa2,3,rsip,192.168.100.20,rsport,1997,rspw,667788
	*/
	
	pStr = strtok(argv[2],",");
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
	strcat(buf1,  "authentication = 1\n");			
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

int main(int argc, char *argv[])
{

	if ( argc > 1 ) {
		if(!strcmp(argv[1], "startAuth")){	/*sigma test*/
			char cmdbuf2[128];					
			
			sprintf(cmdbuf2,"killall -9 auth");
			system(cmdbuf2);
			sprintf(cmdbuf2,"killall -9 iapp");
			system(cmdbuf2);			

			generateWpaConf2(argc,argv);
			sprintf(cmdbuf2,"auth wlan0 br0 auth %s",SIGMA_OUTPUTFILE);
			system(cmdbuf2);
			sprintf(cmdbuf2,"auth wlan1 br0 auth %s",SIGMA_OUTPUTFILE);
			system(cmdbuf2);

			sprintf(cmdbuf2,"iapp br0 wlan0 wlan1");
			system(cmdbuf2);
			sprintf(cmdbuf2,"iwcontrol wlan0 wlan1");
			system(cmdbuf2);

			return 0;			
		}		
	}
	return 0;
}
