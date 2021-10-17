/*WIFI API
  *
  */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/route.h>
#include <linux/wireless.h>

#include "rtk_api.h"
#include "rtk_adapter.h"

#ifdef HAVE_RTK_DRVMIB

#include "rtk_wifi_drvmib.h"
#include "../../../linux-2.6.30/drivers/net/wireless/rtl8192cd/ieee802_mib.h"

#endif

#define perror		printf
static char cmdBuff[128] = {0};


static inline int
iw_get_ext(int                  skfd,           /* Socket to the kernel */
           char *               ifname,         /* Device name */
           int                  request,        /* WE ID */
           struct iwreq *       pwrq)           /* Fixed part of the request */
{
  /* Set device name */
  strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
  /* Do the request */
  return(ioctl(skfd, request, pwrq));
}

int getWlStaInfo( char *interface,  RTK_WLAN_STA_INFO_Tp pInfo )
{
#ifndef NO_ACTION
    int skfd=0;
    struct iwreq wrq;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd==-1)
		return -1;
    /* Get wireless name */
    if ( iw_get_ext(skfd, interface, SIOCGIWNAME, &wrq) < 0){
      /* If no wireless name : no wireless extensions */
      close( skfd );
        return -1;
	}
    wrq.u.data.pointer = (caddr_t)pInfo;
    wrq.u.data.length = sizeof(RTK_WLAN_STA_INFO_T) * (MAX_STA_NUM+1);
    memset(pInfo, 0, sizeof(RTK_WLAN_STA_INFO_T) * (MAX_STA_NUM+1));

    if (iw_get_ext(skfd, interface, SIOCGIWRTLSTAINFO, &wrq) < 0){
    	close( skfd );
		return -1;
	}
    close( skfd );
#else
    return -1;
#endif
    return 0;
}



#ifdef HAVE_RTK_APMIB
#define TYPE_INT 0
#define TYPE_ARRAY 1
#define TYPE_TBL 2
static int get_wlan_idx_by_wlanif(char * ifname)
{
	int idx;

	idx = atoi(&ifname[4]);
	if (idx >= NUM_WLAN_INTERFACE) {
	 perror("invalid wlan interface index number!\n");
	 return -1;
	}
	return idx;
}

static int set_wlan_idx_by_wlanIf(char * ifname)
{
	int idx;

	idx = atoi(&ifname[4]);
	if (idx >= NUM_WLAN_INTERFACE) {
	 perror("invalid wlan interface index number!\n");
	 return -1;
	}
	wlan_idx = idx;
	vwlan_idx = 0;
#ifdef MBSSID		
	if (strlen(ifname) >= 9 && ifname[5] == '-' &&
	ifname[6] == 'v' && ifname[7] == 'a') {
	 idx = atoi(&ifname[8]);
	 if (idx >= NUM_VWLAN_INTERFACE) {
		 perror("invalid virtual wlan interface index number!\n");
		 return -1;
	 }
	 vwlan_idx = idx+1;
	 idx = atoi(&ifname[4]);
	 wlan_idx = idx;
	}
#endif	
#ifdef UNIVERSAL_REPEATER
	if (strlen(ifname) >= 9 && ifname[5] == '-' &&
	!memcmp(&ifname[6], "vxd", 3)) {
	 vwlan_idx = NUM_VWLAN_INTERFACE;
	 idx = atoi(&ifname[4]);
	 wlan_idx = idx;
	}
#endif				
		 
	return 0;		 
}	 


int apmib_get_rtkapi(char *ifname, int MIB_ID, void *value, int type)
{
	int ret = 0;
	apmib_save_wlanIdx();

	if(set_wlan_idx_by_wlanIf(ifname))
	 return -1;

	if(type == TYPE_INT)
	{
	 int *intVal = (int*) value;
	 
	 ret= apmib_get(MIB_ID, (void *)intVal);
	}
	else if(type == TYPE_ARRAY)
	{
	ret = apmib_get(MIB_ID, (void *)value);
	}
	else if(type == TYPE_TBL)
	{
		ret = apmib_get(MIB_ID, (void *)value);
	}

	apmib_recov_wlanIdx();
	return ret;
}

int apmib_set_rtkapi(char *ifname, int MIB_ID, void *value, int type)
{
	int ret = 0;
	apmib_save_wlanIdx();

	if(set_wlan_idx_by_wlanIf(ifname))
	 return -1;

	if(type == TYPE_INT)
	{
	 int intVal = 0;

	 intVal = (int)(*((int *)value));
	 
	 ret = apmib_set(MIB_ID, (void *)&intVal);
	}
	else if(type == TYPE_ARRAY)
	{
	 char tmpStr[65] = {0};	
	 //sprintf(tmpStr, value);
	 strcpy(tmpStr, value);
	 tmpStr[64]=0;
	 ret = apmib_set(MIB_ID, (void *)tmpStr);
	}
	else if(type == TYPE_TBL)
	{
		ret = apmib_set(MIB_ID, (void *)value);
	}

	if(!ret)
	 perror("[%s][%s][MIB_ID=%d] error !\n", __FUNCTION__, ifname, MIB_ID);
	 
	apmib_recov_wlanIdx();
	return ret;
}
#endif
void wlanif_down_up(char *ifname, int On)
{

	if(On)
	{
		sprintf(cmdBuff,"ifconfig %s up", ifname);
		system(cmdBuff);
	}
	else
	{
		sprintf(cmdBuff,"ifconfig %s down", ifname);
		system(cmdBuff);
	}

}

/**************************************************
* @NAME:
* 	rtk_wlan_disable
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Disable wireless network serivce 
*
***************************************************/
int rtk_wlan_disable(unsigned char *ifname)
{
	int intValue = 1,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WLAN_DISABLED,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#endif
	return ret;	
}

/**************************************************
* @NAME:
* 	rtk_wlan_enable
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Enable wireless network serivce 
*
***************************************************/

int rtk_wlan_enable(unsigned char *ifname)
{
	int intValue = 0,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WLAN_DISABLED,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_down_interface
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Down wireless interface 
*
***************************************************/

int rtk_wlan_down_interface(unsigned char *ifname)
{
	if(!ifname)
		return RTK_FAILED;
	wlanif_down_up(ifname,0);
	return RTK_SUCCESS;
}
/**************************************************
* @NAME:
* 	rtk_wlan_up_interface
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Up wireless interface 
*
***************************************************/

int rtk_wlan_up_interface(unsigned char *ifname)
{
	if(!ifname)
		return RTK_FAILED;
	wlanif_down_up(ifname,1);
	return RTK_SUCCESS;
}
/**************************************************
* @NAME:
* 	rtk_wlan_immediately_work
* 
* @PARAMETERS:
* 	@Input
* 		enable: Wireless interface name
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Up wireless interface 
*
***************************************************/

int rtk_wlan_immediately_work(unsigned char *ifname)
{
	unsigned char buffer[64];
	unsigned int wlan_disabled = 0;
	#define TEMP_API_FLAG_FILE "/tmp/wlan_immediately_work"

	sprintf(buffer, "ifconfig %s down", ifname);
	system(buffer);
	sprintf(buffer, "echo 1 > %s", TEMP_API_FLAG_FILE);
	system(buffer);
	sprintf(buffer, "flash set_mib %s", ifname);
	system(buffer);
	sprintf(buffer, "rm %s", TEMP_API_FLAG_FILE);
	system(buffer);
	
	apmib_get_rtkapi(ifname,MIB_WLAN_WLAN_DISABLED,(void *)&wlan_disabled,TYPE_INT);
	if(wlan_disabled)
		return RTK_SUCCESS;
	
	sprintf(buffer, "ifconfig %s up", ifname);
	system(buffer);
	return RTK_SUCCESS;
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_mode
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		mode: operation mode, 
*		0-AP, 1-Client, 2-WDS, 3-AP+WDS
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the operation mode of wlan 
*
***************************************************/

int rtk_wlan_set_mode(unsigned char *ifname, unsigned int mode)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = mode;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_MODE,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL || mode>3 || mode<0 ){
		ret = RTK_FAILED;
	}else{
		switch(mode){
		case 0:
			pmib->dot11OperationEntry.opmode = 0x10;
			break;
		case 1:
			pmib->dot11OperationEntry.opmode = 0x08;
			break;
		case 2:
			pmib->dot11OperationEntry.opmode = 0x1000;
			break;
		case 3:
			pmib->dot11OperationEntry.opmode = 0x1010;
			break;
		}
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_ssid
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		ssid: the SSID of wlan
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the SSID of wlan
*
***************************************************/

int  rtk_wlan_set_ssid(unsigned char *ifname, unsigned char *ssid)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !ssid)
		return RTK_FAILED;
	if(strlen(ssid) > 64 || strlen(ssid) <= 0)
		return RTK_FAILED;
		
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_SSID,(void *)ssid,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	int ssidlen = strlen(ssid);
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL || ssidlen>sizeof(pmib->dot11StationConfigEntry.dot11DesiredSSID)){
		ret = RTK_FAILED;
	}else{
		pmib->dot11StationConfigEntry.dot11DesiredSSIDLen = ssidlen;
		memcpy(pmib->dot11StationConfigEntry.dot11DesiredSSID, ssid, ssidlen);
	}
#endif
		return ret; 
}
/**************************************************
* @NAME:
* 	rtk_wlan_set_passwd
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		passwd: the password of wlan
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the password of wlan
*
***************************************************/

int rtk_wlan_set_passwd(unsigned char *ifname, unsigned char *passwd)
{}

/**************************************************
* @NAME:
* 	rtk_wlan_set_band
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		band: Bit mask of band selection,
*			1-11B, 2-11G, 4-11A, 8-11N,64-11AC
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the band of  wlan
*
***************************************************/

int rtk_wlan_set_band(unsigned char *ifname, unsigned int band)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = band;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_BAND,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11BssType.net_work_type = band;
	}
#endif
		return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_set_band_immediately
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		band: Bit mask of band selection,
*			1-11B, 2-11G, 4-11A, 8-11N,64-11AC
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	 Set the band of  wlan to take effort immediately.
*
***************************************************/

int rtk_wlan_set_band_immediately(unsigned char *ifname, unsigned int band)
{
	int ret = RTK_SUCCESS;
	char cmd[100];
	
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)	
	ret=rtk_wlan_set_band(ifname,band);
	rtk_wlan_immediately_work(ifname);
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	int legacy;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}
	if(RTK_SUCCESS==ret)
	{	
		sprintf(cmd, "ifconfig %s down", ifname);
		system(cmd);
		if ((!pmib->dot11StationConfigEntry.sc_enabled) && (pmib->dot11OperationEntry.wifi_specific == 1) && (band == 2))
			band = 3;
		if (band == 8) {
#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)
			if(pmib->dot11RFEntry.phyBandSelect == PHYBAND_5G){
				band += 4;
				legacy = 4;
			}
			else if (pmib->dot11RFEntry.phyBandSelect == PHYBAND_2G)
#endif
			{
				band += 3;
				legacy = 3;
			}
		}
		else if (band == 2) {
			band += 1;
			legacy = 1;
		}
		else if (band == 10) {
			band += 1;
			legacy = 1;
		}
		else if (band == 64) {
			band += 12;
			legacy = 12;
		}
		else if (band == 72) {
			band += 4;
			legacy = 4;			
		}
		else
			legacy = 0;
				
		sprintf(cmd, "iwpriv %s set_mib band=%d", ifname, band);
		system(cmd);
		sprintf(cmd, "iwpriv %s set_mib deny_legacy=%d", ifname, legacy);
		system(cmd);
		sprintf(cmd, "ifconfig %s up", ifname);
		system(cmd);			
	}
#endif	
		return ret; 
}
/**************************************************
* @NAME:
* 	rtk_wlan_set_network_type
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		type: the network type of wlan,
*			0-Infrastructure,1-Adhoc
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the network type of wlan
*
***************************************************/

int rtk_wlan_set_network_type(unsigned char *ifname, unsigned int type)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = type;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_NETWORK_TYPE,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11OperationEntry.opmode = type==0 ? 8 : 32;
	}
#endif

	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_channel_bandwidth
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		bandwidth: the bandwidth of wlan,
*		 	0-20M, 1-40M, 2-80M
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the bandwidth of wlan
*
***************************************************/

int rtk_wlan_set_channel_bandwidth(unsigned char *ifname, unsigned int bandwidth)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = bandwidth;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_CHANNEL_BONDING,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11nConfigEntry.dot11nUse40M = bandwidth;
	}
#endif
		return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_set_channel_bandwidth_immediately
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		bandwidth: the bandwidth of wlan,
*		 	0-20M, 1-40M, 2-80M
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	 Set the bandwidth of wlan to take effect immediately.
*
***************************************************/

int rtk_wlan_set_channel_bandwidth_immediately(unsigned char *ifname, unsigned int bandwidth)
{
	int ret = RTK_SUCCESS;
	char cmd[100];
	ret=rtk_wlan_set_channel_bandwidth(ifname,bandwidth);
	if(RTK_SUCCESS==ret)
	{	sprintf(cmd, "ifconfig %s down", ifname);
		system(cmd);
		sprintf(cmd, "iwpriv %s set_mib use40M=%d", ifname, bandwidth);
		system(cmd);
		sprintf(cmd, "ifconfig %s up", ifname);
		system(cmd);
	}
		return ret; 
}
/**************************************************
* @NAME:
* 	rtk_wlan_set_2ndchoff
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		offset: control sideband offset, 0-Upper, 1-Lower
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the control sideband of wlan
*
***************************************************/

int rtk_wlan_set_2ndchoff(unsigned char *ifname, unsigned int offset)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = offset;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_CONTROL_SIDEBAND,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11nConfigEntry.dot11n2ndChOffset = offset;
	}
#endif
		return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_channel
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		channel: the channel number of wlan
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the channel of wlan 
*
***************************************************/

int rtk_wlan_set_channel(unsigned char *ifname, unsigned int channel)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = channel;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_CHANNEL,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11RFEntry.dot11channel = channel;
	}
#endif
		return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_set_channel_immediately
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		channel: the channel number of wlan
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	 Set the channel of wlan to take effect immediately.
*
***************************************************/

int rtk_wlan_set_channel_immediately(unsigned char *ifname, unsigned int channel)
{
	int ret = RTK_SUCCESS;
	char cmd[100];
	ret=rtk_wlan_set_channel(ifname,channel);
	if(RTK_SUCCESS==ret)
	{	sprintf(cmd, "ifconfig %s down", ifname);
		system(cmd);
		sprintf(cmd, "iwpriv %s set_mib channel=%d", ifname, channel);
		system(cmd);
		sprintf(cmd, "ifconfig %s up", ifname);
		system(cmd);
	}
		return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_set_hidden_ssid
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		hidden:  the flag of hidden ssid, 1 -enable, 0-disable 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set wlan to enable/disable hidden ssid
*
***************************************************/

int rtk_wlan_set_hidden_ssid(unsigned char *ifname, unsigned int hidden)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = hidden;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_HIDDEN_SSID,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11OperationEntry.hiddenAP = hidden;
	}
#endif
	return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_set_fixrate
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		rate:  the bitmask of transmission rate 
*			0-auto rate,
*			bit0~bit11 for rate 1, 2, 5.5, 11, 6, 9,  12, 18, 24, 36,48,54M,
*			bit12~bit27 for rate MCS0~MCS15, 
*			bit31+(0~19) for rate NSS1-MCS(0~9), NSS2-MCS(0~9)
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the transmission rate of wlan
*
***************************************************/

int rtk_wlan_set_fixrate(unsigned char *ifname, unsigned int rate)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = rate;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_FIX_RATE,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11StationConfigEntry.fixedTxRate = rate;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_txrestrict
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		bandwith:  the limit value of transmission bandwith 
*			0-disable
*			
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the limit transmission bandwith of wlan
*
***************************************************/

int rtk_wlan_set_txrestrict(unsigned char *ifname, unsigned int bandwith)
{
	int intValue,ret = RTK_SUCCESS;
		if(!ifname)
			return RTK_FAILED;
		intValue = bandwith;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
		if(apmib_set_rtkapi(ifname,MIB_WLAN_TX_RESTRICT,(void *)&intValue,TYPE_INT) <= 0)
			ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
		struct wifi_mib* pmib;
		pmib = drvmib_get_pmib(ifname);
		if( pmib==NULL ){
			ret = RTK_FAILED;
		}else{
			pmib->gbwcEntry.GBWCThrd_tx = bandwith*1024;
		}
#endif
	return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_rxrestrict
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		bandwith:  the limit value of receive bandwith 
*			0-disable
*			
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the limit receive bandwith of wlan
*
***************************************************/

int rtk_wlan_set_rxrestrict(unsigned char *ifname, unsigned int bandwith)
{
	int intValue,ret = RTK_SUCCESS;
		if(!ifname)
			return RTK_FAILED;
		intValue = bandwith;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
		if(apmib_set_rtkapi(ifname,MIB_WLAN_RX_RESTRICT,(void *)&intValue,TYPE_INT) <= 0)
			ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
		struct wifi_mib* pmib;
		pmib = drvmib_get_pmib(ifname);
		if( pmib==NULL ){
			ret = RTK_FAILED;
		}else{
			pmib->gbwcEntry.GBWCThrd_rx = bandwith*1024;
		}
#endif
		return ret;

}
/**************************************************
* @NAME:
* 	rtk_wlan_set_enable_repeater
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*			
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Enable smart repeater feature
*
***************************************************/

int rtk_wlan_set_enable_repeater(unsigned char *ifname)
{
	int intValue = 1,ret = RTK_SUCCESS,idx,mibid;
	unsigned char wlanvxd_if[16] = {0};
	if(!ifname ||(idx = get_wlan_idx_by_wlanif(ifname)) <0)
		return RTK_FAILED;
	sprintf(wlanvxd_if,"wlan%d-vxd",idx);	
#if defined(HAVE_RTK_APMIB)
	if(idx == 0)
		mibid = MIB_REPEATER_ENABLED1;
	else
		mibid = MIB_REPEATER_ENABLED2;
	if(apmib_set_rtkapi(ifname,mibid,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	else{
		intValue = 0;
		if(apmib_set_rtkapi(wlanvxd_if,MIB_WLAN_WLAN_DISABLED,(void *)&intValue,TYPE_INT) <= 0)
			ret = RTK_FAILED;
		else{
			intValue = 1;
			if(apmib_set_rtkapi(wlanvxd_if,MIB_WLAN_MODE,(void *)&intValue,TYPE_INT) <= 0)
				ret = RTK_FAILED;
		}
	}
		
#endif
		return ret;

}
/**************************************************
* @NAME:
* 	rtk_wlan_set_disable_repeater
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*			
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Disable smart repeater feature
*
***************************************************/

int rtk_wlan_set_disable_repeater(unsigned char *ifname)
{
	int intValue = 0,ret = RTK_SUCCESS,idx,mibid;
	unsigned char wlanvxd_if[16] = {0};
	if(!ifname ||(idx = get_wlan_idx_by_wlanif(ifname)) <0)
		return RTK_FAILED;
	sprintf(wlanvxd_if,"wlan%d-vxd",idx);	
#if defined(HAVE_RTK_APMIB)
	if(idx == 0)
		mibid = MIB_REPEATER_ENABLED1;
	else
		mibid = MIB_REPEATER_ENABLED2;
	if(apmib_set_rtkapi(ifname,mibid,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	else{
		intValue = 1;
		if(apmib_set_rtkapi(wlanvxd_if,MIB_WLAN_WLAN_DISABLED,(void *)&intValue,TYPE_INT) <= 0)
			ret = RTK_FAILED;
	}
#endif
		return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_repeater_ssid
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		ssid: the  ssid	 of smart repeater
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set the ssid of smart repeater
*
***************************************************/

int rtk_wlan_set_repeater_ssid(unsigned char *ifname,unsigned char *ssid)
{
	int ret = RTK_SUCCESS,idx,mibid;
	if(!ifname || !ssid || (idx = get_wlan_idx_by_wlanif(ifname)) <0)
		return RTK_FAILED;
		
#if defined(HAVE_RTK_APMIB)
	if(idx == 0)
		mibid = MIB_REPEATER_SSID1;
	else
		mibid = MIB_REPEATER_SSID2;
	if(apmib_set_rtkapi(ifname,mibid,(void *)ssid,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#endif
		return ret;

}
/**************************************************
* @NAME:
* 	rtk_wlan_set_fragthres
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		fragthres: fragment threshold
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set fragment threshold 
*
***************************************************/
int rtk_wlan_set_fragthres(unsigned char *ifname, unsigned int fragthres)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_FRAG_THRESHOLD,(void *)&fragthres,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11OperationEntry.dot11FragmentationThreshold = fragthres;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_rtsthres
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		rtsthres: RTS threshold
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set RTS threshold 
*
***************************************************/
int rtk_wlan_set_rtsthres(unsigned char *ifname, unsigned int rtsthres)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_RTS_THRESHOLD,(void *)&rtsthres,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11OperationEntry.dot11RTSThreshold = rtsthres;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_beacon_interval
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		interval: beacon interval
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set beacon interval
*
***************************************************/
int rtk_wlan_set_beacon_interval(unsigned char *ifname, unsigned int interval)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_BEACON_INTERVAL,(void *)&interval,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11StationConfigEntry.dot11BeaconPeriod = interval;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_preamble
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	enable wlan preamble 
*
***************************************************/
int rtk_wlan_set_preamble(unsigned char *ifname, unsigned int enable)
{
	
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_PREAMBLE_TYPE,(void *)&enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11RFEntry.shortpreamble = enable;
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_protection
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	enable wlan protection
*
***************************************************/
int rtk_wlan_set_protection(unsigned char *ifname, unsigned int enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	unsigned int disabled = (!enable);
	if(apmib_set_rtkapi(ifname,MIB_WLAN_PROTECTION_DISABLED,(void *)&disabled,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11StationConfigEntry.protectionDisabled = enable;
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_aggregation
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	enable wlan aggregation 
*
***************************************************/
int rtk_wlan_set_aggregation(unsigned char *ifname, unsigned int enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_AGGREGATION,(void *)&enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11nConfigEntry.dot11nAMPDU = (enable & 1)!=0;
		pmib->dot11nConfigEntry.dot11nAMSDU = (enable & 2)!=0;
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_shortgi
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	enable wlan aggregation 
*
***************************************************/
int rtk_wlan_set_shortgi(unsigned char *ifname, unsigned int enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_SHORT_GI,(void *)&enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11nConfigEntry.dot11nShortGIfor20M = enable;
		pmib->dot11nConfigEntry.dot11nShortGIfor40M = enable;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_isolation
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: access flag 0-lan+wan 1-wan
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set access flag
*
***************************************************/
int rtk_wlan_set_isolation(unsigned char *ifname, unsigned int enable)
{	
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_ACCESS,(void *)&enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11OperationEntry.guest_access = enable;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_stbc
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	enable STBC 
*
***************************************************/
int rtk_wlan_set_stbc(unsigned char *ifname, unsigned int enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_STBC_ENABLED,(void *)&enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11nConfigEntry.dot11nSTBC = enable;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_ldpc
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	enable LDPC 
*
***************************************************/
int rtk_wlan_set_ldpc(unsigned char *ifname, unsigned int enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_LDPC_ENABLED,(void *)&enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11nConfigEntry.dot11nLDPC = enable;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_coexist
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	enable coexist 
*
***************************************************/
int rtk_wlan_set_coexist(unsigned char *ifname, unsigned int enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_COEXIST_ENABLED,(void *)&enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11nConfigEntry.dot11nCoexist = enable;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_txbeamforming
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	enable Tx beamforming
*
***************************************************/
int rtk_wlan_set_txbeamforming(unsigned char *ifname, unsigned int enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_TX_BEAMFORMING,(void *)&enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11RFEntry.txbf = enable;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_mc2u
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	disable mutilcast to unicast
*
***************************************************/
int rtk_wlan_set_mc2u(unsigned char *ifname, unsigned int enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	unsigned int disabled = (!enable);
	if(apmib_set_rtkapi(ifname,MIB_WLAN_MC2U_DISABLED,(void *)&disabled,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	char cmd[100];
	sprintf(cmd, "iwpriv %s set_mib mc2u_disable=%d", ifname, !enable);
	system(cmd);
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_rfpower
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		level: 	RF output power level 0-100% 1-70% 2-50% 3-%35 4-15%
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set RF output power level
*
***************************************************/
int rtk_wlan_set_rfpower(unsigned char *ifname, unsigned int level)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_RFPOWER_SCALE,(void *)&level,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if (pmib->dot11RFEntry.dot11RFType == 10) { // Zebra
			int intVal = level;
			if(intVal == 1)
				intVal = 3;
			else if(intVal == 2)
					intVal = 6;
				else if(intVal == 3)
						intVal = 9;
					else if(intVal == 4)
							intVal = 17;
			if (intVal) {
				int i;
#if defined(CONFIG_RTL_8196B)
				for (i=0; i<14; i++) {
					if(pmib->dot11RFEntry.pwrlevelCCK[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevelCCK[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelCCK[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelCCK[i] = 1;
					}
				}
				for (i=0; i<162; i++) {
					if (pmib->dot11RFEntry.pwrlevelOFDM_1SS[i] != 0){
						if((pmib->dot11RFEntry.pwrlevelOFDM_1SS[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelOFDM_1SS[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelOFDM_1SS[i] = 1;
					}
					if (pmib->dot11RFEntry.pwrlevelOFDM_2SS[i] != 0){
						if((pmib->dot11RFEntry.pwrlevelOFDM_2SS[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelOFDM_2SS[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelOFDM_2SS[i] = 1;
					}
				}		
#elif defined(CONFIG_RTL_8198C)||defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198B)
				for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++) {
					if(pmib->dot11RFEntry.pwrlevelCCK_A[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevelCCK_A[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelCCK_A[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelCCK_A[i] = 1;
					}
					if(pmib->dot11RFEntry.pwrlevelCCK_B[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevelCCK_B[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelCCK_B[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelCCK_B[i] = 1;
					}
					if(pmib->dot11RFEntry.pwrlevelHT40_1S_A[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevelHT40_1S_A[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelHT40_1S_A[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelHT40_1S_A[i] = 1;
					}
					if(pmib->dot11RFEntry.pwrlevelHT40_1S_B[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevelHT40_1S_B[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelHT40_1S_B[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelHT40_1S_B[i] = 1;
					}
				}
#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)
				for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++) {
					if(pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[i] = 1;
					}
					if(pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[i] != 0){ 
						if ((pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[i] = 1;
					}
				}
#endif 
#else
				for (i=0; i<14; i++) {
					if(pmib->dot11RFEntry.pwrlevelCCK[i] != 0){ 
					    if ((pmib->dot11RFEntry.pwrlevelCCK[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelCCK[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelCCK[i] = 1;
				    }
				}
				for (i=0; i<162; i++) {
					if (pmib->dot11RFEntry.pwrlevelOFDM[i] != 0){
					    if((pmib->dot11RFEntry.pwrlevelOFDM[i] - intVal) >= 1)
							pmib->dot11RFEntry.pwrlevelOFDM[i] -= intVal;
						else
							pmib->dot11RFEntry.pwrlevelOFDM[i] = 1;
				    }
				}
#endif
			}
		}
	}
#endif
	return ret; 
}
/**************************************************
* @NAME:
* 	rtk_wlan_set_rfpower_immediately
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		level: 	RF output power level 0-100% 1-70% 2-50% 3-%35 4-15%
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	 Set RF output power level to take effort immediately.
*
***************************************************/
int rtk_wlan_set_rfpower_immediately(unsigned char *ifname, unsigned int level)
{
	int ret = RTK_SUCCESS,diff=0,i=0;
	char cmd[100];
	unsigned int dot11RFType;
	unsigned char buf_CCK_A[20]={0},buf_CCK_B[20]={0},buf_HT401S_A[20]={0},buf_HT401S_B[20]={0},buf_5GHT401S_A[200]={0},buf_5GHT401S_B[200]={0};
	char	pwrlevelCCK_A[MAX_2G_CHANNEL_NUM_MIB*2+1];
	char	pwrlevelCCK_B[MAX_2G_CHANNEL_NUM_MIB*2+1];
	char	pwrlevelHT40_1S_A[MAX_2G_CHANNEL_NUM_MIB*2+1];
	char	pwrlevelHT40_1S_B[MAX_2G_CHANNEL_NUM_MIB*2+1];
	char	pwrlevel5GHT40_1S_A[MAX_5G_CHANNEL_NUM_MIB*2+1];
	char	pwrlevel5GHT40_1S_B[MAX_5G_CHANNEL_NUM_MIB*2+1];
	
	memset(pwrlevelCCK_A,'\0',MAX_2G_CHANNEL_NUM_MIB*2+1);
	memset(pwrlevelCCK_B,'\0',MAX_2G_CHANNEL_NUM_MIB*2+1);
	memset(pwrlevelHT40_1S_A,'\0',MAX_2G_CHANNEL_NUM_MIB*2+1);
	memset(pwrlevelHT40_1S_B,'\0',MAX_2G_CHANNEL_NUM_MIB*2+1);
	memset(pwrlevel5GHT40_1S_A,'\0',MAX_5G_CHANNEL_NUM_MIB*2+1);
	memset(pwrlevel5GHT40_1S_B,'\0',MAX_5G_CHANNEL_NUM_MIB*2+1);
	
	ret=rtk_wlan_set_rfpower(ifname,level);

#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_HW_RF_TYPE, (void *)&dot11RFType,TYPE_INT)<=0)
		ret = RTK_FAILED;
	if(level == 1)
		diff = 3;
	else if(level == 2)
			diff = 6;
		else if(level == 3)
				diff = 9;
			else if(level == 4)
					diff = 17;
	#if defined(CONFIG_RTL_8196B)
	#elif defined(CONFIG_RTL_8198C)||defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198B)

		if(apmib_get_rtkapi(ifname,MIB_HW_TX_POWER_CCK_A, (void *)buf_CCK_A,TYPE_ARRAY)<=0)
			ret = RTK_FAILED;
		if(apmib_get_rtkapi(ifname,MIB_HW_TX_POWER_CCK_B, (void *)buf_CCK_B,TYPE_ARRAY)<=0)
			ret = RTK_FAILED;
		if(apmib_get_rtkapi(ifname,MIB_HW_TX_POWER_HT40_1S_A, (void *)buf_HT401S_A,TYPE_ARRAY)<=0)
			ret = RTK_FAILED;
		if(apmib_get_rtkapi(ifname,MIB_HW_TX_POWER_HT40_1S_B, (void *)buf_HT401S_B,TYPE_ARRAY)<=0)
			ret = RTK_FAILED;

		for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++) {
			sprintf(pwrlevelCCK_A+2*i,"%02x",(buf_CCK_A[i]>=diff+1)?(buf_CCK_A[i]-diff):1);
			sprintf(pwrlevelCCK_B+2*i,"%02x",(buf_CCK_B[i]>=diff+1)?(buf_CCK_B[i]-diff):1);
			sprintf(pwrlevelHT40_1S_A+2*i,"%02x",(buf_HT401S_A[i]>=diff+1)?(buf_HT401S_A[i]-diff):1);
			sprintf(pwrlevelHT40_1S_B+2*i,"%02x",(buf_HT401S_B[i]>=diff+1)?(buf_HT401S_B[i]-diff):1);
		}

		#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)
		if(apmib_get_rtkapi(ifname,MIB_HW_TX_POWER_5G_HT40_1S_A, (void *)buf_5GHT401S_A,TYPE_ARRAY)<=0)
			ret = RTK_FAILED;
		if(apmib_get_rtkapi(ifname,MIB_HW_TX_POWER_5G_HT40_1S_B, (void *)buf_5GHT401S_B,TYPE_ARRAY)<=0)
			ret = RTK_FAILED;	

		for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++) {
			sprintf(pwrlevel5GHT40_1S_A+2*i,"%02x",(buf_5GHT401S_A[i]>=diff+1)?(buf_5GHT401S_A[i]-diff):1);
			sprintf(pwrlevel5GHT40_1S_B+2*i,"%02x",(buf_5GHT401S_B[i]>=diff+1)?(buf_5GHT401S_B[i]-diff):1);
		}
		#endif
	#else
	#endif

#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	dot11RFType=pmib->dot11RFEntry.dot11RFType;
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
	#if defined(CONFIG_RTL_8196B)
	#elif defined(CONFIG_RTL_8198C)||defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198B)
		
			for (i=0; i<MAX_2G_CHANNEL_NUM_MIB; i++) {
				sprintf(pwrlevelCCK_A+2*i,"%02x",pmib->dot11RFEntry.pwrlevelCCK_A[i]);
				sprintf(pwrlevelCCK_B+2*i,"%02x",pmib->dot11RFEntry.pwrlevelCCK_B[i]);
				sprintf(pwrlevelHT40_1S_A+2*i,"%02x",pmib->dot11RFEntry.pwrlevelHT40_1S_A[i]);
				sprintf(pwrlevelHT40_1S_B+2*i,"%02x",pmib->dot11RFEntry.pwrlevelHT40_1S_B[i]);
			}
			#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)
			for (i=0; i<MAX_5G_CHANNEL_NUM_MIB; i++) {
				sprintf(pwrlevel5GHT40_1S_A+2*i,"%02x",pmib->dot11RFEntry.pwrlevel5GHT40_1S_A[i]);
				sprintf(pwrlevel5GHT40_1S_B+2*i,"%02x",pmib->dot11RFEntry.pwrlevel5GHT40_1S_B[i]);
			}
			#endif 
		
	#else
	#endif
	}

#endif

	if(RTK_SUCCESS==ret && dot11RFType==10)
	{	
		sprintf(cmd, "ifconfig %s down", ifname);
		system(cmd);
		#if defined(CONFIG_RTL_8196B)
		#elif defined(CONFIG_RTL_8198C)||defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198B)
			sprintf(cmd, "iwpriv %s set_mib pwrlevelCCK_A=%s", ifname, pwrlevelCCK_A);
			system(cmd);
			sprintf(cmd, "iwpriv %s set_mib pwrlevelCCK_B=%s", ifname, pwrlevelCCK_B);
			system(cmd);
			sprintf(cmd, "iwpriv %s set_mib pwrlevelHT40_1S_A=%s", ifname, pwrlevelHT40_1S_A);
			system(cmd);
			sprintf(cmd, "iwpriv %s set_mib pwrlevelHT40_1S_B=%s", ifname, pwrlevelHT40_1S_B);
			system(cmd);
			#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)
			sprintf(cmd, "iwpriv %s set_mib pwrlevel5GHT40_1S_A=%s", ifname, pwrlevel5GHT40_1S_A);
			system(cmd);
			sprintf(cmd, "iwpriv %s set_mib pwrlevel5GHT40_1S_B=%s", ifname, pwrlevel5GHT40_1S_B);
			system(cmd);
			#endif
		#else
		#endif
		sprintf(cmd, "ifconfig %s up", ifname);
		system(cmd);
	}
		return ret; 
}
/**************************************************
* @NAME:
* 	rtk_wlan_set_encryption
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		encrypt: encryption type 0-none 1-wep 2-wpa 4-wpa2 6-mixed
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set encryption
*
***************************************************/
int rtk_wlan_set_encryption(unsigned char *ifname,unsigned int encrypt)
{	
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_ENCRYPT,(void*)&encrypt,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if( encrypt!=1 && pmib->dot1180211AuthEntry.dot11AuthAlgrthm==1 ){
			pmib->dot1180211AuthEntry.dot11AuthAlgrthm = 0;
		}
		if(encrypt==0){
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 0;
		}else if(encrypt==1){
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 1;
		}else if(encrypt==2){
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 2;
			pmib->dot1180211AuthEntry.dot11EnablePSK = 1;
		}else if(encrypt==4){
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 2;
			pmib->dot1180211AuthEntry.dot11EnablePSK = 2;
		}else if(encrypt==6){
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 2;
			pmib->dot1180211AuthEntry.dot11EnablePSK = 3;
		}
	}
#endif
	return ret; 

}

static int _is_hex(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||
            ((c >= 'a') && (c <= 'f')));
}

static int string_to_hex(char *string, unsigned char *key, int len)
{
	char tmpBuf[4];
	int idx, ii=0;
	for (idx=0; idx<len; idx+=2) {
		tmpBuf[0] = string[idx];
		tmpBuf[1] = string[idx+1];
		tmpBuf[2] = 0;
		if ( !_is_hex(tmpBuf[0]) || !_is_hex(tmpBuf[1]))
			return 0;

		key[ii++] = (unsigned char) strtol(tmpBuf, (char**)NULL, 16);
	}
	return 1;
}
/**************************************************
* @NAME:
* 	rtk_wlan_set_security_wep
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		auth_type:	authentication 0-open 1-shared key 2-auto
*		key_len: 1-64bit 2-128bit
*		key_type: key format 0-ASCII(5/13 characters) 1-Hex(10/26 characters)
*		key: wep password
*		default_key_idx: default key number
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set security type wep
*
***************************************************/
int rtk_wlan_set_security_wep(unsigned char *ifname, unsigned int auth_type, unsigned int key_len, unsigned int key_type, unsigned int default_key_idx, unsigned char *key)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_AUTH_TYPE,(void *)&auth_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP,(void *)&key_len,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP_KEY_TYPE,(void *)&key_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP_DEFAULT_KEY,(void *)&default_key_idx,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(key_len == 1){
		if(default_key_idx == 1){
			if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP64_KEY2,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
		else if(default_key_idx == 2){
			if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP64_KEY3,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
		else if(default_key_idx == 3){
			if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP64_KEY4,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
		else{
			if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP64_KEY1,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
	}
	else if(key_len == 2){
		if(default_key_idx == 1){
			if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP128_KEY2,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
		else if(default_key_idx == 2){
			if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP128_KEY3,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
		else if(default_key_idx == 3){
			if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP128_KEY4,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
		else{
			if(apmib_set_rtkapi(ifname,MIB_WLAN_WEP128_KEY1,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
	}
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		int key_size=0;
		pmib->dot1180211AuthEntry.dot11AuthAlgrthm = auth_type;
		if(key_len==1){
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 1;
			key_size = 5;
		}else if(key_len==2){
			pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm = 5;
			key_size = 13;
		}

		if(key_type==1){
			unsigned char skey[16];
			if(string_to_hex(key, skey, key_size<<1)<=0){
				return RTK_FAILED;
			}
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[default_key_idx%4]), skey, key_size);
		}else{
			memcpy(&(pmib->dot11DefaultKeysTable.keytype[default_key_idx%4]), key, key_size);
		}
		
		pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex = default_key_idx;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_security_wpa
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		auth_type:	authentication 1-Enterprise(RADIUS) 2-Personal
*		cipher_suite: 1-TKIP 2-AES
*		key_type: psk format 0-Passphrase 1-HEX(64 characters)
*		key: wpa password
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set security type wpa
*
***************************************************/
int rtk_wlan_set_security_wpa(unsigned char *ifname, unsigned int auth_type, unsigned int cipher_suite, unsigned int key_type, unsigned char *key)
{
		int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA_AUTH,(void *)&auth_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA_CIPHER_SUITE,(void *)&cipher_suite,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_PSK_FORMAT,(void *)&key_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA_PSK,(void *)key,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{	
		if( auth_type==2 ){
			strcpy(pmib->dot1180211AuthEntry.dot11PassPhrase, key);
			pmib->dot1180211AuthEntry.dot11EnablePSK = 1;
		}else{
			pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
		}
		if(cipher_suite==1){
			pmib->dot1180211AuthEntry.dot11WPACipher = 2;
		}else if(cipher_suite==2){
			pmib->dot1180211AuthEntry.dot11WPACipher = 8;
		}else if(cipher_suite==3){
			pmib->dot1180211AuthEntry.dot11WPACipher = 10;
		}
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_security_wpa2
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		auth_type:	authentication 1-Enterprise(RADIUS) 2-Personal
*		cipher_suite: 1-TKIP 2-AES
*		key_type: psk format 0-Passphrase 1-HEX(64 characters)
*		key: wpa2 password
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set security type wpa2
*
***************************************************/
int rtk_wlan_set_security_wpa2(unsigned char *ifname, unsigned int auth_type, unsigned int cipher_suite, unsigned int key_type, unsigned char *key)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA_AUTH,(void *)&auth_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA2_CIPHER_SUITE,(void *)&cipher_suite,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_PSK_FORMAT,(void *)&key_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA_PSK,(void *)key,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{	
		if( auth_type==2 ){
			strcpy(pmib->dot1180211AuthEntry.dot11PassPhrase, key);
			pmib->dot1180211AuthEntry.dot11EnablePSK = 1;
		}else{
			pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
		}
		if(cipher_suite==1){
			pmib->dot1180211AuthEntry.dot11WPA2Cipher = 2;
		}else if(cipher_suite==2){
			pmib->dot1180211AuthEntry.dot11WPA2Cipher = 8;
		}else if(cipher_suite==3){
			pmib->dot1180211AuthEntry.dot11WPA2Cipher = 10;
		}
	}

#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_security_wpamix
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		auth_type:	authentication 1-Enterprise(RADIUS) 2-Personal
*		wpa_cipher_suite: wpa 1-TKIP 2-AES
*		wpa2_cipher_suite: wpa2 1-TKIP 2-AES
*		key_type: psk format 0-Passphrase 1-HEX(64 characters)
*		key: password
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set security type wpa-mixed
*
***************************************************/
int rtk_wlan_set_security_wpamix(unsigned char *ifname, unsigned int auth_type, unsigned int wpa_cipher_suite, unsigned int wpa2_cipher_suite, unsigned int key_type, unsigned char *key)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA_AUTH,(void *)&auth_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA_CIPHER_SUITE,(void *)&wpa_cipher_suite,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA2_CIPHER_SUITE,(void *)&wpa2_cipher_suite,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_PSK_FORMAT,(void *)&key_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WPA_PSK,(void *)key,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{	
		if( auth_type==2 ){
			strcpy(pmib->dot1180211AuthEntry.dot11PassPhrase, key);
			pmib->dot1180211AuthEntry.dot11EnablePSK = 1;
		}else{
			pmib->dot1180211AuthEntry.dot11EnablePSK = 0;
		}
		if(wpa_cipher_suite==1){
			pmib->dot1180211AuthEntry.dot11WPACipher = 2;
		}else if(wpa_cipher_suite==2){
			pmib->dot1180211AuthEntry.dot11WPACipher = 8;
		}else if(wpa_cipher_suite==3){
			pmib->dot1180211AuthEntry.dot11WPACipher = 10;
		}
		if(wpa2_cipher_suite==1){
			pmib->dot1180211AuthEntry.dot11WPA2Cipher = 2;
		}else if(wpa2_cipher_suite==2){
			pmib->dot1180211AuthEntry.dot11WPA2Cipher = 8;
		}else if(wpa2_cipher_suite==3){
			pmib->dot1180211AuthEntry.dot11WPA2Cipher = 10;
		}
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_wds
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: the flag of enable/disable wds
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Enable/Disable wds
*
***************************************************/

int rtk_wlan_set_wds(unsigned char *ifname, unsigned int enable)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = enable;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WDS_ENABLED,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11WdsInfo.wdsEnabled = enable;
	}	
#endif
	return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_add_wds_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		mac: the mac address of the entry
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Add  one wds entry 
*
***************************************************/

int rtk_wlan_add_wds_entry(unsigned char *ifname, unsigned char *mac)
{
	int entrynum,i,ret = RTK_SUCCESS;
	if(!ifname || !mac)
		return RTK_FAILED;
	
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	RTK_WDS_T entry;

	if((apmib_get_rtkapi(ifname,MIB_WLAN_WDS_NUM,(void *)&entrynum,TYPE_INT) <= 0)
		|| (entrynum >= MAX_WDS_NUM)){
		ret = RTK_FAILED;
		goto end;
	}	
	for( i = 1;i <= entrynum;i++){
		*(char *)&entry = (char)i;
		if( (apmib_get_rtkapi(ifname,MIB_WLAN_WDS,(void *)&entry,TYPE_TBL) <= 0)
			|| !memcmp(entry.macAddr,mac,6)){
			ret = RTK_FAILED;
			goto end;
		}
	}
	memset(&entry,0,sizeof(entry));
	memcpy(entry.macAddr,mac,6);
	if(apmib_set_rtkapi(ifname,MIB_WLAN_WDS_ADD,(void *)&entry,TYPE_TBL) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if(pmib->dot11WdsInfo.wdsNum >= MAX_WDS_NUM){
			ret = RTK_FAILED;
			goto end;
		}
		for(i=0;i<pmib->dot11WdsInfo.wdsNum; i++){
			if(memcmp(pmib->dot11WdsInfo.entry[i].macAddr, mac, 6)==0){
				ret = RTK_FAILED;
				goto end;
			}
		}
		memcpy(pmib->dot11WdsInfo.entry[i].macAddr, mac, 6);
		pmib->dot11WdsInfo.entry[i].txRate = 0;
		pmib->dot11WdsInfo.wdsNum++;
	}
#endif
end:
		return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_del_wds_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		mac: the mac address of entry to delete
*		delall: the flag of delelting all entry or not
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Delete  one or all wds entry  
*
***************************************************/

int rtk_wlan_del_wds_entry(unsigned char *ifname,unsigned char *mac, unsigned int delall)
{
	int entrynum,i,ret = RTK_SUCCESS;
	if(!ifname || !mac)
		return RTK_FAILED;
	
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	RTK_WDS_T entry;
	if(delall){
		if(apmib_set_rtkapi(ifname,MIB_WLAN_WDS_DELALL,(void *)&entry,TYPE_TBL) <= 0)
			ret = RTK_FAILED;
	}
	else{	
		if(apmib_get_rtkapi(ifname,MIB_WLAN_WDS_NUM,(void *)&entrynum,TYPE_INT) <= 0){
			ret = RTK_FAILED;
			goto end;
		}
		for( i = entrynum;i > 0;i--){
			*(char *)&entry = (char)i;
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WDS,(void *)&entry,TYPE_TBL) <= 0){
				ret = RTK_FAILED;
				goto end;
			}
			if(!memcmp(entry.macAddr,mac,6)){
				if(apmib_set_rtkapi(ifname,MIB_WLAN_WDS_DEL,(void *)&entry,TYPE_TBL) <= 0)
					ret = RTK_FAILED;
				break;
			}
		}
	}
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL || !pmib->dot11WdsInfo.wdsEnabled ){
		ret = RTK_FAILED;
	}else{
		if(delall){
			pmib->dot11WdsInfo.wdsNum = 0;
		}else{
			int matchFlag = 0;
			struct wdsEntry* wdsentry = &(pmib->dot11WdsInfo.entry[0]);
			for(i=0;i<pmib->dot11WdsInfo.wdsNum;i++){
				if(matchFlag==0){
					matchFlag = memcmp(wdsentry[i].macAddr, mac, 6)==0;
				}else{
					memcpy(wdsentry+i-1, wdsentry+i, sizeof(*wdsentry));
				}
			}
			if(matchFlag){
				pmib->dot11WdsInfo.wdsNum--;
			}else{
				ret = RTK_FAILED;
			}
		}
	}
#endif
end:
		return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_set_acl_mode
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		acl_mode: Wireless access control mode 0-disable 1-allow listed 2-deny listed
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set wireless access control mode 
*
***************************************************/
int rtk_wlan_set_acl_mode(unsigned char *ifname, unsigned int acl_mode)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_MACAC_ENABLED,(void *)&acl_mode,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL || acl_mode<0 || acl_mode>2 ){
		ret = RTK_FAILED;
	}else{
		pmib->dot11StationConfigEntry.dot11AclMode = acl_mode;
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_add_acl_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		mac: Mac address
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Add wireless access control list mac address 
*
***************************************************/
int rtk_wlan_add_acl_entry(unsigned char *ifname, unsigned char *mac)
{	
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	RTK_ACL_T entry;
	unsigned int mac_num;
	int i;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_MACAC_NUM,(void *)&mac_num,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	for(i=0; i<mac_num; i++){
		*(char *)&entry = (char)i;
		if(apmib_get_rtkapi(ifname,MIB_WLAN_MACAC_ADDR,(void *)&entry,TYPE_TBL) <= 0)
			ret = RTK_FAILED;
		if(!memcmp(entry.macAddr,mac,6))
			ret = RTK_FAILED;
	}
	memcpy(entry.macAddr,mac,6);
	if(apmib_set_rtkapi(ifname,MIB_WLAN_AC_ADDR_ADD,(void *)&entry,TYPE_TBL) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL || !pmib->dot11StationConfigEntry.dot11AclMode ){
		ret = RTK_FAILED;
	}else{
		int i;
		if(pmib->dot11StationConfigEntry.dot11AclNum >= NUM_ACL){
			ret = RTK_FAILED;
			goto end;
		}
		for(i=0;i<pmib->dot11StationConfigEntry.dot11AclNum; i++){
			if(memcmp(pmib->dot11StationConfigEntry.dot11AclAddr[i], mac, 6)==0){
				ret = RTK_FAILED;
				goto end;
			}
		}
		memcpy(pmib->dot11StationConfigEntry.dot11AclAddr[i], mac, 6);
		pmib->dot11StationConfigEntry.dot11AclNum++;
	}
#endif
end:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_del_acl_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		mac: Mac address
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Delete wireless access control list mac address 
*
***************************************************/
int rtk_wlan_del_acl_entry(unsigned char *ifname, unsigned char *mac, unsigned int delall)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	RTK_ACL_T entry;
	int mac_num;
	int i;
	if(delall){
		if(apmib_set_rtkapi(ifname,MIB_WLAN_AC_ADDR_DELALL,(void *)&entry,TYPE_TBL) <= 0)
			ret = RTK_FAILED;
	}
	else{
		if(apmib_get_rtkapi(ifname,MIB_WLAN_MACAC_NUM,(void *)&mac_num,TYPE_INT) <= 0)
			ret = RTK_FAILED;
		for(i=1; i<=mac_num; i++){
			*(char *)&entry = (char)i;
			if(apmib_get_rtkapi(ifname,MIB_WLAN_MACAC_ADDR,(void *)&entry,TYPE_TBL) <= 0)
				ret = RTK_FAILED;
			if(!memcmp(entry.macAddr,mac,6)){
				if(apmib_set_rtkapi(ifname,MIB_WLAN_AC_ADDR_DEL,(void *)entry.macAddr,TYPE_TBL) <= 0)
					ret = RTK_FAILED;
				break;
			}
		}
	}
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL || !pmib->dot11StationConfigEntry.dot11AclMode ){
		ret = RTK_FAILED;
	}else{
		if(delall){
			pmib->dot11StationConfigEntry.dot11AclNum = 0;
		}else{
			int matchFlag = 0;
			int i;
			struct wdsEntry* entry = &(pmib->dot11WdsInfo.entry[0]);
			for(i=0;i<pmib->dot11StationConfigEntry.dot11AclNum;i++){
				if(matchFlag==0){
					matchFlag = memcmp(pmib->dot11StationConfigEntry.dot11AclAddr[i], mac, 6)==0;
				}else{
					memcpy(pmib->dot11StationConfigEntry.dot11AclAddr[i-1], pmib->dot11StationConfigEntry.dot11AclAddr[i], MACADDRLEN);
				}
			}
			if(matchFlag){
				pmib->dot11StationConfigEntry.dot11AclNum--;
			}else{
				ret = RTK_FAILED;
			}
		}
	}

#endif
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_schdule
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: the flag of enable/disable wlan schedule
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Enable/Disable wlan schedule
*
***************************************************/

int rtk_wlan_set_schdule(unsigned char *ifname, unsigned int enable)
{
	int intValue,ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
	intValue = enable;
#if defined(HAVE_RTK_APMIB)
	if(apmib_set_rtkapi(ifname,MIB_WLAN_SCHEDULE_ENABLED,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#endif
		return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_add_schedule_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		entry: the wlan schedule entry to add
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Add  one wlan schedule entry 
*
***************************************************/

int rtk_wlan_add_schedule_entry(unsigned char *ifname, RTK_SCHEDULE_T *entry)
{
	int entrynum,i,ret = RTK_SUCCESS;
	if(!ifname || !entry)
		return RTK_FAILED;
	
#if defined(HAVE_RTK_APMIB)
	RTK_SCHEDULE_T tmp_entry;

	if((apmib_get_rtkapi(ifname,MIB_WLAN_SCHEDULE_TBL_NUM,(void *)&entrynum,TYPE_INT) <= 0)
		|| (entrynum >= MAX_SCHEDULE_NUM)){
		ret = RTK_FAILED;
		goto end;
	}	
	for( i = 1;i <= entrynum;i++){
		*(char *)&tmp_entry = (char)i;
		if( (apmib_get_rtkapi(ifname,MIB_WLAN_SCHEDULE_TBL,(void *)&tmp_entry,TYPE_TBL) <= 0)
			|| (!strcmp(tmp_entry.text,entry ->text) && tmp_entry.day == entry->day 
			&& tmp_entry.eco == entry->eco && tmp_entry.from_time == entry->from_time
			&& tmp_entry.to_time == entry ->to_time)){
			ret = RTK_FAILED;
			goto end;
		}
	}
	memset(&tmp_entry,0,sizeof(RTK_SCHEDULE_T));
	strcpy(tmp_entry.text,entry ->text);
	tmp_entry.day = entry->day;
	tmp_entry.eco = entry->eco;
	tmp_entry.from_time = entry->from_time;
	tmp_entry.to_time = entry ->to_time;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_SCHEDULE_ADD,(void *)&tmp_entry,TYPE_TBL) <= 0)
		ret = RTK_FAILED;
#endif
end:
		return ret;

}
/**************************************************
* @NAME:
* 	rtk_wlan_del_schedule_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		entry: the wlan schedule entry to delete
*		delall: the flag of delelting all entry or not
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Delete  one or all wlan schedule entry  
*
***************************************************/

int rtk_wlan_del_schedule_entry(unsigned char *ifname, RTK_SCHEDULE_T *entry, unsigned int delall)
{
	int entrynum,i,ret = RTK_SUCCESS;
	if(!ifname || !entry)
		return RTK_FAILED;
	
#if defined(HAVE_RTK_APMIB)
	RTK_SCHEDULE_T tmp_entry;
	if(delall){
		if(apmib_set_rtkapi(ifname,MIB_WLAN_SCHEDULE_DELALL,(void *)&tmp_entry,TYPE_TBL) <= 0)
			ret = RTK_FAILED;
	}
	else{	
		if(apmib_get_rtkapi(ifname,MIB_WLAN_SCHEDULE_TBL_NUM,(void *)&entrynum,TYPE_INT) <= 0){
			ret = RTK_FAILED;
			goto end;
		}
		for( i = entrynum;i > 0;i--){
			*(char *)&tmp_entry = (char)i;
			if(apmib_get_rtkapi(ifname,MIB_WLAN_SCHEDULE_TBL,(void *)&tmp_entry,TYPE_TBL) <= 0){
				ret = RTK_FAILED;
				goto end;
			}
			if(!strcmp(tmp_entry.text,entry ->text) && tmp_entry.day == entry->day 
				&& tmp_entry.eco == entry->eco && tmp_entry.from_time == entry->from_time
				&& tmp_entry.to_time == entry ->to_time){
				if(apmib_set_rtkapi(ifname,MIB_WLAN_SCHEDULE_DEL,(void *)&tmp_entry,TYPE_TBL) <= 0)
					ret = RTK_FAILED;
				break;
			}
		}
	}
#endif
end:
		return ret;

}


/**/

/**************************************************
* @NAME:
* 	rtk_wlan_get_enable
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	enable: 
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the flag of enable/disable wlan
*
***************************************************/

int rtk_wlan_get_enable(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS,intValue = 0;
	if(!ifname || !enable)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WLAN_DISABLED,(void *)&intValue,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(intValue == 0)
		*enable = 1;
	else
		*enable = 0;
#endif
		return ret; 

}


/**************************************************
* @NAME:
* 	rtk_wlan_get_mode
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	mode: the operation mode of wlan
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the operation mode  of wlan 
*
***************************************************/

int rtk_wlan_get_mode(unsigned char *ifname, unsigned int *mode)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !mode)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_MODE,(void *)mode,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if(pmib->dot11OperationEntry.opmode==0x08 || pmib->dot11OperationEntry.opmode==0x20 ){
			*mode = 1;
		}else if(pmib->dot11OperationEntry.opmode==0x10){
			*mode = 0;
		}else if(pmib->dot11OperationEntry.opmode==0x1000){
			*mode = 2;
		}else if(pmib->dot11OperationEntry.opmode==0x1010){
			*mode = 3;
		}else{
			ret = RTK_FAILED;
		}
	}
#endif
		return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_get_ssid
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	ssid: the SSID of wlan
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the SSID of wlan
*
***************************************************/

int rtk_wlan_get_ssid(unsigned char *ifname, unsigned char *ssid)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !ssid)
		return RTK_FAILED;
		
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_SSID,(void *)ssid,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	int ssidlen;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		memcpy(ssid, pmib->dot11StationConfigEntry.dot11DesiredSSID, pmib->dot11StationConfigEntry.dot11DesiredSSIDLen);
	}
#endif
	return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_get_passwd
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	passwd: the password of wlan
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the password of wlan
*
***************************************************/

int rtk_wlan_get_passwd(unsigned char *ifname, unsigned char *passwd)
{}
/**************************************************
* @NAME:
* 	rtk_wlan_get_band
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*		band: Bit mask of band selection,
*			1-11B, 2-11G, 4-11A, 8-11N,64-11AC
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the band of wlan
*
***************************************************/

int rtk_wlan_get_band(unsigned int *ifname, unsigned int *band)
{
	int ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_BAND,(void *)band,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*band = pmib->dot11BssType.net_work_type;
	}
#endif
		return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_network_type
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	type: the network type of wlan,
*			0-Infrastructure,1-Adhoc
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the network type of wlan
*
***************************************************/

int rtk_wlan_get_network_type(unsigned char  *ifname, unsigned int *type)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !type)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_NETWORK_TYPE,(void *)type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if( pmib->dot11OperationEntry.opmode==8 ){
			*type = 0;
		}else if( pmib->dot11OperationEntry.opmode==32 ){
			*type = 1;
		}else{
			ret = RTK_FAILED;
		}
	}

#endif
	return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_get_channel_bandwidth
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name

* 	@Output
*	 	bandwidth: the bandwidth of wlan,
*		 	0-20M, 1-40M, 2-80M
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the bandwidth of wlan
*
***************************************************/

int rtk_wlan_get_channel_bandwidth(unsigned char *ifname, unsigned int *bandwidth)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !bandwidth)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_CHANNEL_BONDING,(void *)bandwidth,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*bandwidth = pmib->dot11nConfigEntry.dot11nUse40M;
	}
#endif
		return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_get_2ndchoff
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	offset: control sideband offset, 0-Upper, 1-Lower
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the control sideband of wlan
*
***************************************************/

int rtk_wlan_get_2ndchoff(unsigned char *ifname, unsigned int *offset)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !offset)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_CONTROL_SIDEBAND,(void *)offset,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*offset = pmib->dot11nConfigEntry.dot11n2ndChOffset;
	}
#endif
		return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_channel
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*	 	channel: the channel number of wlan
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the channel of wlan 
*
***************************************************/

int rtk_wlan_get_channel(unsigned char *ifname, unsigned int *channel)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !channel)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_CHANNEL,(void *)channel,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*channel = pmib->dot11RFEntry.dot11channel;
	}
#endif
		return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_hidden_ssid
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name 
* 	@Output
*		hidden:  the flag of hidden ssid, 1 -enable, 0-disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the flag of enable/disable hidden ssid
*
***************************************************/
int rtk_wlan_get_hidden_ssid(unsigned char *ifname, unsigned int* hidden)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !hidden)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_HIDDEN_SSID,(void *)hidden,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*hidden = pmib->dot11OperationEntry.hiddenAP;
	}
#endif
		return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_fixrate
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name

* 	@Output
*		rate:  the bitmask of transmission rate 
*			0-auto rate,
*			bit0~bit11 for rate 1, 2, 5.5, 11, 6, 9,  12, 18, 24, 36,48,54M,
*			bit12~bit27 for rate MCS0~MCS15, 
*			bit31+(0~19) for rate NSS1-MCS(0~9), NSS2-MCS(0~9)
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the transmission rate of wlan
*
***************************************************/


int rtk_wlan_get_fixrate(unsigned char *ifname, unsigned int* rate)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !rate)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_FIX_RATE,(void *)rate,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*rate = pmib->dot11StationConfigEntry.fixedTxRate = rate;
	}
#endif
		return ret; 

}
/**************************************************
* @NAME:
* 	rtk_wlan_get_txrestrict
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*			
* 	@Output
*		bandwith:  the limit value of transmission bandwith 
*			0-disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the limit transmission bandwith of wlan
*
***************************************************/

int rtk_wlan_get_txrestrict(unsigned char *ifname, unsigned int *bandwith)
{
	int ret = RTK_SUCCESS;
		if(!ifname || !bandwith)
			return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
		if(apmib_get_rtkapi(ifname,MIB_WLAN_TX_RESTRICT,(void *)bandwith,TYPE_INT) <= 0)
			ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
		struct wifi_mib* pmib;
		pmib = drvmib_get_pmib(ifname);
		if( pmib==NULL ){
			ret = RTK_FAILED;
		}else{
			*bandwidth = pmib->gbwcEntry.GBWCThrd_tx>>10;
		}
#endif
		return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_rxrestrict
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*			
* 	@Output
*		bandwith:  the limit value of receive bandwith 
*			0-disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the limit receive bandwith of wlan
*
***************************************************/

int rtk_wlan_get_rxrestrict(unsigned char *ifname, unsigned int *bandwith)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !bandwith)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_RX_RESTRICT,(void *)bandwith,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
		struct wifi_mib* pmib;
		pmib = drvmib_get_pmib(ifname);
		if( pmib==NULL ){
			ret = RTK_FAILED;
		}else{
			*bandwidth = pmib->gbwcEntry.GBWCThrd_rx>>10;
		}
#endif
		return ret;

}
/**************************************************
* @NAME:
* 	rtk_wlan_get_enable_repeater
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*			
* 	@Output
*	 	enable: the flag of smart repeater, 1 -enable, 0-disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the flag of smart repeater feature
*
***************************************************/

int rtk_wlan_get_enable_repeater(unsigned char *ifname,unsigned int *enable)
{
	int ret = RTK_SUCCESS,idx,mibid;
	if(!ifname || !enable || (idx = get_wlan_idx_by_wlanif(ifname)) <0)
		return RTK_FAILED;
		
#if defined(HAVE_RTK_APMIB)
	if(idx == 0)
		mibid = MIB_REPEATER_ENABLED1;
	else
		mibid = MIB_REPEATER_ENABLED2;
	if(apmib_get_rtkapi(ifname,mibid,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#endif
		return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_repeater_ssid
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*		ssid: the  ssid	 of smart repeater
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the ssid of smart repeater
*
***************************************************/

int rtk_wlan_get_repeater_ssid(unsigned char *ifname,unsigned char *ssid)
{
	int ret = RTK_SUCCESS,idx,mibid;
	if(!ifname || !ssid || (idx = get_wlan_idx_by_wlanif(ifname)) <0)
		return RTK_FAILED;
		
#if defined(HAVE_RTK_APMIB)
	if(idx == 0)
		mibid = MIB_REPEATER_SSID1;
	else
		mibid = MIB_REPEATER_SSID2;
	if(apmib_get_rtkapi(ifname,mibid,(void *)ssid,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#endif
		return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_fragthres
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		fragthres: fragment threshold
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get fragment threshold 
*
***************************************************/
int rtk_wlan_get_fragthres(unsigned char *ifname, unsigned int *fragthres)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(!apmib_get_rtkapi(ifname,MIB_WLAN_FRAG_THRESHOLD,(void *)fragthres,TYPE_INT))
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*fragthres = pmib->dot11OperationEntry.dot11FragmentationThreshold ;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_rtsthres
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		rtsthres: RTS threshold
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get RTS threshold 
*
***************************************************/
int rtk_wlan_get_rtsthres(unsigned char *ifname, unsigned int *rtsthres)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(!apmib_get_rtkapi(ifname,MIB_WLAN_RTS_THRESHOLD,(void *)rtsthres,TYPE_INT))
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*rtsthres = pmib->dot11OperationEntry.dot11RTSThreshold;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_beacon_interval
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		interval: beacon interval
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get beacon interval
*
***************************************************/
int rtk_wlan_get_beacon_interval(unsigned char *ifname, unsigned int *interval)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(!apmib_get_rtkapi(ifname,MIB_WLAN_BEACON_INTERVAL,(void *)interval,TYPE_INT))
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*interval = pmib->dot11StationConfigEntry.dot11BeaconPeriod;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_preamble
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: enable flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get wlan preamble enable flag
*
***************************************************/
int rtk_wlan_get_preamble(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(!apmib_get_rtkapi(ifname,MIB_WLAN_PREAMBLE_TYPE,(void *)enable,TYPE_INT))
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11RFEntry.shortpreamble;
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_protection
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: protection flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get wlan protection enable flag
*
***************************************************/
int rtk_wlan_get_protection(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	unsigned int disable;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_PROTECTION_DISABLED,(void *)&disable,TYPE_INT) <= 0)
			ret = RTK_FAILED;
	*enable = (!disable);
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11StationConfigEntry.protectionDisabled;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_aggregation
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: aggregation flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get wlan aggregation flag
*
***************************************************/
int rtk_wlan_get_aggregation(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_AGGREGATION,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11nConfigEntry.dot11nAMPDU + pmib->dot11nConfigEntry.dot11nAMSDU*2;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_shortgi
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: short GI
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get short GI flag
*
***************************************************/
int rtk_wlan_get_shortgi(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_SHORT_GI,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	*enable = pmib->dot11nConfigEntry.dot11nShortGIfor20M + pmib->dot11nConfigEntry.dot11nShortGIfor40M ;
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_isolation
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: access flag 0-lan+wan 1-wan
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get access flag
*
***************************************************/
int rtk_wlan_get_isolation(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_ACCESS,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11OperationEntry.guest_access;
	}
#endif
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_stbc
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: STBC flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get STBC flag
*
***************************************************/	
int rtk_wlan_get_stbc(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_STBC_ENABLED,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11nConfigEntry.dot11nSTBC;
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_ldpc
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: LDPC flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get LDPC flag
*
***************************************************/
int rtk_wlan_get_ldpc(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_LDPC_ENABLED,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11nConfigEntry.dot11nLDPC;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_coexist
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: coexist flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get coexist flag
*
***************************************************/
int rtk_wlan_get_coexist(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_COEXIST_ENABLED,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11nConfigEntry.dot11nCoexist;
	}
#endif
	return ret; 

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_txbeamforming
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: TX beamforming flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get TX beamforming flag
*
***************************************************/
int rtk_wlan_get_txbeamforming(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_TX_BEAMFORMING,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11RFEntry.txbf;
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_mc2u
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: TX beamforming flag
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get disable mutilcast to unicast flag
*
***************************************************/
int rtk_wlan_get_mc2u(unsigned char *ifname, unsigned int *enable)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	unsigned int disabled;
	if(apmib_set_rtkapi(ifname,MIB_WLAN_MC2U_DISABLED,(void *)&disabled,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	*enable = (!disabled);
#elif defined(HAVE_RTK_DRVMIB)
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_rfpower
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		level: 	RF output power level  0-100% 1-70% 2-50% 3-%35 4-15%
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get RF output power level
*
***************************************************/
int rtk_wlan_get_rfpower(unsigned char *ifname, unsigned int *level)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_RFPOWER_SCALE,(void *)level,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_encryption
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*
* 	@Output
*	 	security: security type 0-disable 1-wep 2-wpa 4-wpa2 6-mixed
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get encryption type
*
***************************************************/
int rtk_wlan_get_encryption(unsigned char *ifname,unsigned int *encrypt)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_ENCRYPT,(void*)encrypt,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if(pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==0){
			*encrypt = 0;
		}else if(pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==1){
			*encrypt = 1;
		}else if(pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==2){
			if(pmib->dot1180211AuthEntry.dot11EnablePSK==1){
				*encrypt = 2;
			}else if(pmib->dot1180211AuthEntry.dot11EnablePSK==2){
				*encrypt = 4;
			}else if(pmib->dot1180211AuthEntry.dot11EnablePSK==3){
				*encrypt = 6;
			}else{
				ret = RTK_FAILED;
			}
		}else{
			ret = RTK_FAILED;
		}
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_security_wep
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		auth_type:	authentication 0-open 1-shared key 2-auto
*		key_len: 1-64bit 2-128bit
*		key_type: key format 0-ASCII(5/13 characters) 1-Hex(10/26 characters)
*		key: wep password
*		default_key_idx: default key number
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get security wep information
*
***************************************************/
int rtk_wlan_get_security_wep(unsigned char *ifname, unsigned int *auth_type, unsigned int *key_len, unsigned int *key_type, unsigned int *default_key_idx, unsigned char *key)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_AUTH_TYPE,(void *)auth_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP,(void *)key_len,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP_KEY_TYPE,(void *)key_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP_DEFAULT_KEY,(void *)default_key_idx,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(key_len == 1){
		if(*default_key_idx == 1){
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP64_KEY2,(void *)key,TYPE_ARRAY) <= 0)
			ret = RTK_FAILED;
		}
		else if(*default_key_idx == 2){
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP64_KEY3,(void *)key,TYPE_ARRAY) <= 0)
			ret = RTK_FAILED;
		}
		else if(*default_key_idx == 3){
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP64_KEY4,(void *)key,TYPE_ARRAY) <= 0)
			ret = RTK_FAILED;
		}
		else {
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP64_KEY1,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
	}
	else if(key_len == 2){
		if(*default_key_idx == 1){
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP128_KEY2,(void *)key,TYPE_ARRAY) <= 0)
			ret = RTK_FAILED;
		}
		else if(*default_key_idx == 2){
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP128_KEY3,(void *)key,TYPE_ARRAY) <= 0)
			ret = RTK_FAILED;
		}
		else if(*default_key_idx == 3){
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP128_KEY4,(void *)key,TYPE_ARRAY) <= 0)
			ret = RTK_FAILED;
		}
		else {
			if(apmib_get_rtkapi(ifname,MIB_WLAN_WEP128_KEY1,(void *)key,TYPE_ARRAY) <= 0)
				ret = RTK_FAILED;
		}
	}
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		int key_size=0;
		*auth_type = pmib->dot1180211AuthEntry.dot11AuthAlgrthm;
		if(pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==1){
			*key_len = 1;
			key_size = 5;
		}else if(pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==5){
			*key_len = 2;
			key_size = 13;
		}
		*default_key_idx = pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		memcpy(key, &(pmib->dot11DefaultKeysTable.keytype[pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex%4]), key_size)
		// no key_type
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_security_wpa
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		auth_type:	authentication 1-Enterprise(RADIUS) 2-Personal
*		cipher_suite: 1-TKIP 2-AES
*		key_type: psk format 0-Passphrase 1-HEX(64 characters)
*		key: wpa password
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get security wpa information
*
***************************************************/
int rtk_wlan_get_security_wpa(unsigned char *ifname, unsigned int *auth_type, unsigned int *cipher_suite, unsigned int *key_type, unsigned char *key)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA_AUTH,(void *)auth_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA_CIPHER_SUITE,(void *)cipher_suite,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_PSK_FORMAT,(void *)key_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA_PSK,(void *)key,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if(pmib->dot1180211AuthEntry.dot11EnablePSK){
			*auth_type = 2;
		}else{
			*auth_type = 1;
		}
		if(pmib->dot1180211AuthEntry.dot11WPACipher==2){
			*cipher_suite = 1;
		}else if(pmib->dot1180211AuthEntry.dot11WPACipher==8){
			*cipher_suite = 2;
		}else if(pmib->dot1180211AuthEntry.dot11WPACipher==10){
			*cipher_suite = 3;
		}
		strcpy(key, pmib->dot1180211AuthEntry.dot11PassPhrase);
		//no key_type
	}
#endif
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_security_wpa2
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		auth_type:	authentication 1-Enterprise(RADIUS) 2-Personal
*		cipher_suite: 1-TKIP 2-AES
*		key_type: psk format 0-Passphrase 1-HEX(64 characters)
*		key: wpa password
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get security wpa2 information
*
***************************************************/
int rtk_wlan_get_security_wpa2(unsigned char *ifname, unsigned int *auth_type, unsigned int *cipher_suite, unsigned int *key_type, unsigned char *key)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA_AUTH,(void *)auth_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA2_CIPHER_SUITE,(void *)cipher_suite,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_PSK_FORMAT,(void *)key_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA_PSK,(void *)key,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if(pmib->dot1180211AuthEntry.dot11EnablePSK){
			*auth_type = 2;
		}else{
			*auth_type = 1;
		}
		if(pmib->dot1180211AuthEntry.dot11WPA2Cipher==2){
			*cipher_suite = 1;
		}else if(pmib->dot1180211AuthEntry.dot11WPA2Cipher==8){
			*cipher_suite = 2;
		}else if(pmib->dot1180211AuthEntry.dot11WPA2Cipher==10){
			*cipher_suite = 3;
		}
		strcpy(key, pmib->dot1180211AuthEntry.dot11PassPhrase);
		//no key_type
	}
#endif
	return ret; 
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_security_wpamix
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		auth_type:	authentication 1-Enterprise(RADIUS) 2-Personal
*		cipher_suite: 1-TKIP 2-AES
*		key_type: psk format 0-Passphrase 1-HEX(64 characters)
*		key: wpa2 password
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get security wpa-mixed information
*
***************************************************/
int rtk_wlan_get_security_wpamix(unsigned char *ifname, unsigned int *auth_type, unsigned int *wpa_cipher_suite, unsigned int *wpa2_cipher_suite, unsigned int *key_type, unsigned char *key)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA_AUTH,(void *)auth_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA_CIPHER_SUITE,(void *)wpa_cipher_suite,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA2_CIPHER_SUITE,(void *)wpa2_cipher_suite,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_PSK_FORMAT,(void *)key_type,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WPA_PSK,(void *)key,TYPE_ARRAY) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		if(pmib->dot1180211AuthEntry.dot11EnablePSK){
			*auth_type = 2;
		}else{
			*auth_type = 1;
		}
		if(pmib->dot1180211AuthEntry.dot11WPACipher==2){
			*wpa_cipher_suite = 1;
		}else if(pmib->dot1180211AuthEntry.dot11WPACipher==8){
			*wpa_cipher_suite = 2;
		}else if(pmib->dot1180211AuthEntry.dot11WPACipher==10){
			*wpa_cipher_suite = 3;
		}
		if(pmib->dot1180211AuthEntry.dot11WPA2Cipher==2){
			*wpa2_cipher_suite = 1;
		}else if(pmib->dot1180211AuthEntry.dot11WPA2Cipher==8){
			*wpa2_cipher_suite = 2;
		}else if(pmib->dot1180211AuthEntry.dot11WPA2Cipher==10){
			*wpa2_cipher_suite = 3;
		}
		strcpy(key, pmib->dot1180211AuthEntry.dot11PassPhrase);
		//no key_type
	}
#endif
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_set_wds
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*		enable: the flag of enable/disable wds
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the flag of enable or disable wds
*
***************************************************/

int rtk_wlan_get_wds(unsigned char *ifname, unsigned int* enable)
{
	int ret = RTK_SUCCESS;
	if(!ifname)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WDS_ENABLED,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*enable = pmib->dot11WdsInfo.wdsEnabled;
	}	
#endif
	return ret;

}
/**************************************************
* @NAME:
* 	rtk_wlan_get_wds_entry_num
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
* 		n:	the entry number of wds.
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the entry number of wds
*
***************************************************/

int rtk_wlan_get_wds_entry_num(unsigned char *ifname,unsigned int* n)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !n)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_WDS_NUM,(void *)n,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		*n = pmib->dot11WdsInfo.wdsNum;
	}	
#endif
		return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_wds_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		empty_num: how many entries to get
* 	@Output
*		mac:  the storage of entries
* 		n:	the actully number of entry in the table
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the entry of wds
*
***************************************************/

int rtk_wlan_get_wds_entry(unsigned char *ifname,unsigned int* n, unsigned char *mac, unsigned int empty_num)
{
	int entrynum,i,ret = RTK_SUCCESS;
	if(!ifname || !mac || !n)
		return RTK_FAILED;
	
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	RTK_WDS_T entry;

	if((apmib_get_rtkapi(ifname,MIB_WLAN_WDS_NUM,(void *)&entrynum,TYPE_INT) <= 0)){
		ret = RTK_FAILED;
		goto end;
	}	
	for( i = 1;i <= entrynum && i <= empty_num;i++){
		*(char *)&entry = (char)i;
		if( (apmib_get_rtkapi(ifname,MIB_WLAN_WDS,(void *)&entry,TYPE_TBL) <= 0)){
			ret = RTK_FAILED;
			goto end;
		}
		memcpy(mac+((i-1)*6),entry.macAddr,6);
	}
	*n = entrynum;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		entrynum = pmib->dot11WdsInfo.wdsNum;
		for(i=0; i<entrynum && i<empty_num; i++){
			memcpy(mac+6*i, pmib->dot11WdsInfo.entry[i].macAddr, 6);
		}
		*n = entrynum;
	}
#endif
end:
	return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_acl_mode
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		acl_mode: Wireless access control mode 0-disable 1-allow listed 2-deny listed
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set wireless access control mode 
*
***************************************************/
int rtk_wlan_get_acl_mode(unsigned char *ifname, unsigned int *acl_mode)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_MACAC_ENABLED,(void *)acl_mode,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL || acl_mode<0 || acl_mode>2 ){
		ret = RTK_FAILED;
	}else{
		*acl_mode = pmib->dot11StationConfigEntry.dot11AclMode;
	}
#endif
	return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_acl_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		mac: Mac address
*		empty_num: save mac address space size
*		mac_num: ACL table mac number
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get wireless access control list 
*
***************************************************/
int rtk_wlan_get_acl_entry(unsigned char *ifname,unsigned int *mac_num,unsigned char* mac, unsigned int empty_num)
{
	int ret = RTK_SUCCESS;
#if defined(HAVE_RTK_APMIB) && !defined(HAVE_RTK_DRVMIB)
	RTK_ACL_T entry;
	int i;
	if(apmib_get_rtkapi(ifname,MIB_WLAN_MACAC_NUM,(void *)mac_num,TYPE_INT) <= 0)
		ret = RTK_FAILED;
	for(i=1; i<= *mac_num && i <= empty_num; i++){
		*(char *)&entry = (char)i;
		if(apmib_get_rtkapi(ifname,MIB_WLAN_MACAC_ADDR,(void *)&entry,TYPE_TBL) <= 0)
			ret = RTK_FAILED;
		memcpy(mac+(i-1)*6,entry.macAddr,6);
	}
#elif defined(HAVE_RTK_DRVMIB)
	struct wifi_mib* pmib;
	pmib = drvmib_get_pmib(ifname);
	if( pmib==NULL ){
		ret = RTK_FAILED;
	}else{
		int i,entrynum;
		entrynum = pmib->dot11StationConfigEntry.dot11AclNum;;
		for(i=0; i<entrynum && i<empty_num; i++){
			memcpy(mac+6*i, pmib->dot11StationConfigEntry.dot11AclAddr[i], 6);
		}
		*mac_num = entrynum;
	}
#endif
	return ret;

}

/**************************************************
* @NAME:
* 	rtk_wlan_get_schdule
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		enable: the flag of enable/disable wlan schedule
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the flag of enable or disable wlan schedule
*
***************************************************/

int rtk_wlan_get_schdule(unsigned char *ifname, unsigned int* enable)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !enable)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_SCHEDULE_ENABLED,(void *)enable,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#endif
		return ret;

}
/**************************************************
* @NAME:
* 	rtk_wlan_get_schdule_entry_num
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
* 		n:	the entry number of wlan schdule.
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the entry number of wlan schdule
*
***************************************************/

int rtk_wlan_get_schdule_entry_num(unsigned char *ifname,unsigned int* n)
{
	int ret = RTK_SUCCESS;
	if(!ifname || !n)
		return RTK_FAILED;
#if defined(HAVE_RTK_APMIB)
	if(apmib_get_rtkapi(ifname,MIB_WLAN_SCHEDULE_TBL_NUM,(void *)n,TYPE_INT) <= 0)
		ret = RTK_FAILED;
#endif
		return ret;
}

/**************************************************
* @NAME:
* 	rtk_wlan_get_schedule_entry
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
*		empty_num:  how many entries to get
* 	@Output
*		entry: the storage of entries
*		n:	the actully number of entry in the table
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the entry of  wlan schedule 
*
***************************************************/

int rtk_wlan_get_schedule_entry(unsigned char *ifname,unsigned int *n,RTK_SCHEDULE_T *entry, unsigned int empty_num)
{
	int entrynum,i,ret = RTK_SUCCESS;
	if(!ifname || !entry || !n)
		return RTK_FAILED;
	
#if defined(HAVE_RTK_APMIB)
	RTK_SCHEDULE_T tmp_entry;

	if(apmib_get_rtkapi(ifname,MIB_WLAN_SCHEDULE_TBL_NUM,(void *)&entrynum,TYPE_INT) <= 0){
		ret = RTK_FAILED;
		goto end;
	}	
	for( i = 1;i <= entrynum && i <= empty_num;i++){
		*(char *)&tmp_entry = (char)i;
		if(apmib_get_rtkapi(ifname,MIB_WLAN_SCHEDULE_TBL,(void *)&tmp_entry,TYPE_TBL) <= 0) {
			ret = RTK_FAILED;
			goto end;
		}
		memcpy(&entry[i-1],&tmp_entry,sizeof(RTK_SCHEDULE_T));
	}
	*n = entrynum;
#endif
end:
		return ret;
}



/**************************************************
* @NAME:
* 	rtk_get_wlan_sta
* 
* @PARAMETERS:
* 	@Input
* 		ifname: Wireless interface name
* 	@Output
*		pInfo: the storage of infomation
*		the size should be sizeof(RTK_WLAN_STA_INFO_T) * (MAX_STA_NUM+1)
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the stations infomation of wlan
*
***************************************************/


int rtk_get_wlan_sta( unsigned char *ifname,  RTK_WLAN_STA_INFO_Tp pInfo)
{
	if(!ifname || !pInfo)
		return RTK_FAILED;
	if(getWlStaInfo(ifname,pInfo) < 0)
		return RTK_FAILED;
	return RTK_SUCCESS;
}

/**************************************************
* @NAME:
* 	rtk_get_wlan_info
* 
* @PARAMETERS:
* 	@Input
* 		info: point of wlan info
*		iface: name of interface
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get wlan information
*
***************************************************/
int rtk_get_wlan_info(RTK_WLAN_INFOp info, char *iface)
{
	if(info == NULL)
		return RTK_FAILED;

	if(iface == NULL)
		return RTK_FAILED;
	
	int ret;
	RTK_BSS_INFO bss;
	int idx;
	int mibid;

	memset(info, 0, sizeof(RTK_WLAN_INFO));

	idx = atoi(&iface[4]);
	if (idx >= NUM_WLAN_INTERFACE) {
		printf("invalid wlan interface index number!\n");
		return RTK_FAILED;
	}

#ifdef HAVE_RTK_APMIB

	ret = apmib_get_rtkapi(iface, MIB_WLAN_WLAN_DISABLED, &info->enabled, TYPE_INT);
	if(ret <= 0)
		return RTK_FAILED;

	ret = apmib_get_rtkapi(iface, MIB_WLAN_MODE, &info->mode, TYPE_INT);	
	if(ret <= 0)
		return RTK_FAILED;

	ret = apmib_get_rtkapi(iface, MIB_WLAN_NETWORK_TYPE, &info->network_type, TYPE_INT);	
	if(ret <= 0)
		return RTK_FAILED;

	ret = apmib_get_rtkapi(iface, MIB_WLAN_BAND, &info->band, TYPE_INT);	
	if(ret <= 0)
		return RTK_FAILED;

	ret = apmib_get_rtkapi(iface, MIB_WLAN_CHANNEL, &info->channel_num, TYPE_INT);	
	if(ret <= 0)
		return RTK_FAILED;

	ret = apmib_get_rtkapi(iface, MIB_WLAN_CHANNEL_BONDING, &info->channel_width, TYPE_INT);	
	if(ret <= 0)
		return RTK_FAILED;

	ret = apmib_get_rtkapi(iface, MIB_WLAN_ENCRYPT, &info->encryption, TYPE_INT);	
	if(ret <= 0)
		return RTK_FAILED;

	ret = apmib_get_rtkapi(iface, MIB_WLAN_SSID, info->SSID, TYPE_ARRAY);	
	if(ret <= 0)
		return RTK_FAILED;

#endif

	if ( getWlBssInfo(iface, &bss) < 0)
		return RTK_FAILED;

	info->state = bss.state;
	memcpy(info->BBSID, bss.bssid, 6);

	if (info->enabled == 1)	// disable
		info->client_num = 0;
	else if(!check_wlan_downup(idx))//if wlanx down
		info->client_num = 0;
	else {
		if ( getWlStaNum(iface, &info->client_num) < 0)
			info->client_num = 0;
	}
	
	return RTK_SUCCESS;
}

