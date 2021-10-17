#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>    
#include <sys/stat.h> 
#include <arpa/inet.h>

#include "apmib.h"
#include "mibtbl.h"

#include "utility.h"

struct wanStatusinfo
{
	char strWanIfname[16];
	char strWanCntTyt[64];
	char strWanIP[16];
	char strWanMask[16];
	char strWanDefIP[16];	
	char strWanHWAddr[18];
};
struct wanStatusinfo wanStsInfo;

#define _DHCPC_PID_PATH		"/etc/udhcpc"
#define _DHCPC_PROG_NAME	"udhcpc"


//extern int getWanInfo(char *pWanIP, char *pWanMask, char *pWanDefIP, char *pWanHWAddr);

int isConnectPPP()
{
	struct stat status;
	
	if ( stat("/etc/ppp/link", &status) < 0)
		return 0;

	return 1;
}

int getPid(char *filename)
{
	struct stat status;
	char buff[100];
	FILE *fp;

	if ( stat(filename, &status) < 0)
		return -1;
	fp = fopen(filename, "r");
	if (!fp) {
        	fprintf(stderr, "Read pid file error!\n");
		return -1;
   	}
	fgets(buff, 100, fp);
	fclose(fp);

	return (atoi(buff));
}

int isDhcpClientExist(char *name)
{
	char tmpBuf[100];
	struct in_addr intaddr;

	if ( getInAddr(name, IP_ADDR, (void *)&intaddr ) ) {
		snprintf(tmpBuf, 100, "%s/%s-%s.pid", _DHCPC_PID_PATH, _DHCPC_PROG_NAME, name);
		if ( getPid(tmpBuf) > 0)
			return 1;
	}
	return 0;
}

int  getWanDnsAdress(char dnsIp[][16])
{
	if(!isFileExist("/etc/resolv.conf"))
		return;

	FILE *fp=NULL;
	char tmpbuf[64]={0};
	char *pch=NULL;
	int i=0;
	
	fp = fopen("/etc/resolv.conf", "r");
	if (!fp) 
	{
        	fprintf(stderr, "Read DNS file error!\n");
		return;
   	}
	while(fgets(tmpbuf, 64, fp))
	{
		tmpbuf[strlen(tmpbuf)-1]='\0';
		pch=strchr(tmpbuf, ' ');
		if(pch!=NULL)
		{
			pch++;
			strcpy(dnsIp[i], pch);
			i++;
		}		
	}
	fclose(fp); 
	return i;	
}

void getWanStatus(char *strWanIP, char *strWanMask, char *strWanDefIP, char *strWanHWAddr)
{
	getWanInfo(strWanIP,strWanMask,strWanDefIP,strWanHWAddr);
}

void getWanConectType(char *strWanIfname, char *strWanCntTyt)
{	
#if defined(CONFIG_RTL_8198_AP_ROOT) || defined(CONFIG_RTL_8197D_AP)
	strcpy(strWanCntTyt, "Brian 5BGG");
	return;
	
#else

	int isWanPhy_Link=0;
	//char iface[16]={0};
	char *iface=NULL;
	
	int opmode, wispWanId, dhcp;		
	
	if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
		return -1;
	if( !apmib_get(MIB_WISP_WAN_ID, (void *)&wispWanId))
		return -1;
	if ( !apmib_get( MIB_WAN_DHCP, (void *)&dhcp) )
		return -1;
	
	if(opmode == BRIDGE_MODE)
	{
		strcpy(strWanCntTyt, "Disconnected");
		return;
	}

	if(opmode != WISP_MODE)
		isWanPhy_Link=getWanLink("eth1");

	if(dhcp==PPPOE || dhcp==PPTP || dhcp==L2TP || dhcp == USB3G)
		strcpy(strWanIfname, "ppp0");
	
	if ( dhcp == DHCP_CLIENT) 
	{
		if(opmode == WISP_MODE) 
		{
			if(0 == wispWanId)
				//strcpy(iface, "wlan0");
				iface="wlan0";
			else if(1 == wispWanId)
				//strcpy(iface, "wlan1");
				iface="wlan1";
			
#ifdef CONFIG_SMART_REPEATER
			if(getWispRptIface(&iface,wispWanId)<0)
				return -1;
#endif
		}
		else
			//strcpy(iface, "eth1");
			iface="eth1";
		
		if (!isDhcpClientExist(iface))
			strcpy(strWanCntTyt, "Getting IP from DHCP server...");
		else
		{
			if(isWanPhy_Link < 0)
				strcpy(strWanCntTyt, "Getting IP from DHCP server...");
			else
				strcpy(strWanCntTyt, "DHCP");
		}		
		strcpy(strWanIfname, iface);
	}
	else if ( dhcp == DHCP_DISABLED )
	{
		if (opmode == WISP_MODE)
		{
			char wan_intf[16] = {0};
			char lan_intf[16] = {0};
			bss_info bss;
			
			getInterfaces(lan_intf,wan_intf);
			memset(&bss, 0x00, sizeof(bss));
			getWlBssInfo(wan_intf, &bss);
			if (bss.state == STATE_CONNECTED)
				strcpy(strWanCntTyt, "Fixed IP Connected");
			else
				strcpy(strWanCntTyt, "Fixed IP Disconnected");

			strcpy(strWanIfname, wan_intf);
		}
		else
		{
			if(isWanPhy_Link < 0)
				strcpy(strWanCntTyt, "Fixed IP Disconnected");
			else
				strcpy(strWanCntTyt, "Fixed IP Connected");
			
			strcpy(strWanIfname, "eth1");
		}		
	}
	else if ( dhcp ==  PPPOE ) 
	{
#ifdef _ALPHA_DUAL_WAN_SUPPORT_
		int pppoeWithDhcpEnabled = 0;
		apmib_get(MIB_PPPOE_DHCP_ENABLED, (void *)&pppoeWithDhcpEnabled);
		if (pppoeWithDhcpEnabled) 
		{
			if ( isConnectPPP())
			{
				if(isWanPhy_Link < 0)
					strcpy(strWanCntTyt, "PPPoE Disconnected");
				else
					strcpy(strWanCntTyt, "PPPoE Connected");
			}
			else
				strcpy(strWanCntTyt, "PPPoE Disconnected");
		}
		else 
		{
			if ( isConnectPPP())
			{
				if(isWanPhy_Link < 0)
					strcpy(strWanCntTyt, "PPPoE Disconnected");
				else
					strcpy(strWanCntTyt, "PPPoE Connected");
			}
			else
				strcpy(strWanCntTyt, "PPPoE Disconnected");
		}
#else // _ALPHA_DUAL_WAN_SUPPORT_
		if ( isConnectPPP())
		{
			if(isWanPhy_Link < 0)
				strcpy(strWanCntTyt, "PPPoE Disconnected");
			else
				strcpy(strWanCntTyt, "PPPoE Connected");
		}
		else
			strcpy(strWanCntTyt, "PPPoE Disconnected");
#endif // _ALPHA_DUAL_WAN_SUPPORT_

#ifdef _ALPHA_DUAL_WAN_SUPPORT_
		{
			if (pppoeWithDhcpEnabled) 
			{
				//strcpy(iface, "eth1");
				iface="eth1";

				if (!isDhcpClientExist(iface))
					strcat(strWanCntTyt, " and Getting IP from DHCP server...");
				else
				{
					if(isWanPhy_Link < 0)
						strcat(strWanCntTyt, " and Getting IP from DHCP server...");
					else
						strcat(strWanCntTyt, " and DHCP");
				}
			}
		}
#endif // _ALPHA_DUAL_WAN_SUPPORT_
	}
	else if ( dhcp ==  PPTP ) 
	{
		if ( isConnectPPP())
		{
			if(isWanPhy_Link < 0)
				strcpy(strWanCntTyt, "PPTP Disconnected");
			else
				strcpy(strWanCntTyt, "PPTP Connected");
		}
		else
			strcpy(strWanCntTyt, "PPTP Disconnected");
	}
	else if ( dhcp ==  L2TP ) 
	{ /* # keith: add l2tp support. 20080515 */
		if ( isConnectPPP())
		{
			if(isWanPhy_Link < 0)
				strcpy(strWanCntTyt, "L2TP Disconnected");
			else
				strcpy(strWanCntTyt, "L2TP Connected");
		}
		else
			strcpy(strWanCntTyt, "L2TP Disconnected");
	}
#ifdef RTK_USB3G
	else if ( dhcp == USB3G ) 
	{
		int inserted = 0;
		char str[32];

		if (isConnectPPP())
			strcpy(strWanCntTyt, "USB3G Connected");
		else 
		{
			FILE *fp;
			char str[32];
			int retry = 0;

OPEN_3GSTAT_AGAIN:
			fp = fopen("/var/usb3g.stat", "r");

			if (fp !=NULL) 
			{
				fgets(str, sizeof(str),fp);
				fclose(fp);
			}
			else if (retry < 5) 
			{
				retry++;
				goto OPEN_3GSTAT_AGAIN;
			}

			if (str != NULL && strstr(str, "init")) 
				strcpy(strWanCntTyt, "USB3G Modem Initializing...");
			else if (str != NULL && strstr(str, "dial"))
				strcpy(strWanCntTyt, "USB3G Dialing...");
			else if (str != NULL && strstr(str, "remove")) 
				strcpy(strWanCntTyt, "USB3G Removed");
			else
				strcpy(strWanCntTyt, "USB3G Disconnected");
		}
	}
#endif /* #ifdef RTK_USB3G */
	//strcpy(strWanIfname, iface);
	//printf("%s:%d strWanIfname=%s\n",__FUNCTION__,__LINE__,strWanIfname);
#endif //#if defined(CONFIG_RTL_8198_AP_ROOT)
}

int main(int argc, char **argv)
{
	if(argc!=2)
		return 1;
	
	apmib_init();
	memset(&wanStsInfo, 0, sizeof(wanStsInfo));
	//printf("%s:%d\n",__FUNCTION__,__LINE__);
	getWanConectType(wanStsInfo.strWanIfname, wanStsInfo.strWanCntTyt);
	//printf("%s:%d\n",__FUNCTION__,__LINE__);
	
	getWanStatus(wanStsInfo.strWanIP, wanStsInfo.strWanMask, wanStsInfo.strWanDefIP, wanStsInfo.strWanHWAddr);
	//printf("%s:%d\n",__FUNCTION__,__LINE__);

	if(strcmp(argv[1], "interface_name")==0)
		printf("wan name=%s\n", wanStsInfo.strWanIfname);
	
	if(strcmp(argv[1], "link_type")==0)
		printf("wan connect type=%s\n", wanStsInfo.strWanCntTyt);
	
	if(strcmp(argv[1], "link_status")==0)
	{
		printf("wan connect type=%s\n", wanStsInfo.strWanCntTyt);
		printf("wan ip address=%s\n", wanStsInfo.strWanIP);
		printf("wan net mask=%s\n", wanStsInfo.strWanMask);
		printf("wan default gateway=%s\n", wanStsInfo.strWanDefIP);

		char dnsIp[3][16];
		int i, num;
		memset(dnsIp, 0, sizeof(dnsIp));
		num=getWanDnsAdress(dnsIp);
		for(i=0;i<num;i++)
			printf("wan dns%d=%s\n", i, dnsIp[i]);
	}	
	
	if(strcmp(argv[1], "mac_address")==0)
		printf("wan mac address=%s\n", wanStsInfo.strWanHWAddr);	
	
	return 0;
}










































