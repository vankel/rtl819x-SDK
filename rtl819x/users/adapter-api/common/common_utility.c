#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/stat.h>
//#include <net/if.h>
#include <linux/sockios.h>
#include <netinet/in.h>
#include <linux/wireless.h>

#include <fcntl.h>

#include "common_utility.h"
#include "../../boa/apmib/apmib.h"

#define _PATH_PROCNET_ROUTE	"/proc/net/route"
#define IFNAMSIZ 16
#define _DHCPC_PROG_NAME	"udhcpc"
#define _DHCPC_PID_PATH		"/etc/udhcpc"
#define RTL8651_IOCTL_GETWANLINKSTATUS 2000
#define RTF_UP			0x0001          /* route usable                 */
#define RTF_GATEWAY		0x0002          /* destination is a gateway     */

extern int wlan_idx;
extern int vwlan_idx;

#ifdef CONFIG_RTL_8196B
#ifdef CONFIG_RTL8196B_TLD
 unsigned char *fwVersion="v1.4_TD";
#else
 unsigned char *fwVersion="v1.4";
#endif
#elif defined(CONFIG_RTL_8196C) || defined(CONFIG_RTL_8198) || defined(CONFIG_RTL_819XD) || defined(CONFIG_RTL_8196E) || defined(CONFIG_RTL_8198C)
//unsigned char *fwVersion="v3.0";
unsigned char *fwVersion="v3.4.6.4";
#else
 unsigned char *fwVersion="v1.2f";
#endif

int rtk_getInAddr( char *interface, int type, void *pAddr )
{
    struct ifreq ifr;
    int skfd, found=0;
	struct sockaddr_in *addr;
    skfd = socket(AF_INET, SOCK_DGRAM, 0);

    strcpy(ifr.ifr_name, interface);
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0){
    		close( skfd );
		return (0);
	}
    if (type == HW_ADDR_T) {
    	if (ioctl(skfd, SIOCGIFHWADDR, &ifr) >= 0) {
		memcpy(pAddr, &ifr.ifr_hwaddr, sizeof(struct sockaddr));
		found = 1;
	}
    }
    else if (type == IP_ADDR_T) {
	if (ioctl(skfd, SIOCGIFADDR, &ifr) == 0) {
		addr = ((struct sockaddr_in *)&ifr.ifr_addr);
		*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
		found = 1;
	}
    }
    else if (type == NET_MASK_T) {
	if (ioctl(skfd, SIOCGIFNETMASK, &ifr) >= 0) {
		addr = ((struct sockaddr_in *)&ifr.ifr_addr);
		*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
		found = 1;
	}
    }else {
    	
    	if (ioctl(skfd, SIOCGIFDSTADDR, &ifr) >= 0) {
		addr = ((struct sockaddr_in *)&ifr.ifr_addr);
		*((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
		found = 1;
	}
    	
    }
    close( skfd );
    return found;

}

int isConnectPPP()
{
	struct stat status;

	if ( stat("/etc/ppp/link", &status) < 0)
		return 0;

	return 1;
}

/*      IOCTL system call */
static int re865xIoctl(char *name, unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
	unsigned int args[4];
	struct ifreq ifr;
	int sockfd;

	args[0] = arg0;
	args[1] = arg1;
	args[2] = arg2;
	args[3] = arg3;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("fatal error socket\n");
		return -3;
	}

	strcpy((char*)&ifr.ifr_name, name);
	((unsigned int *)(&ifr.ifr_data))[0] = (unsigned int)args;

	if (ioctl(sockfd, SIOCDEVPRIVATE, &ifr)<0)
	{
		perror("device ioctl:");
		close(sockfd);
		return -1;
	}
	close(sockfd);
	return 0;
} /* end re865xIoctl */

int getWanLink(char *interface)
{
	unsigned int    ret;
	unsigned int    args[0];

	re865xIoctl(interface, RTL8651_IOCTL_GETWANLINKSTATUS, (unsigned int)(args), 0, (unsigned int)&ret) ;
	return ret;
}

int getDefaultRoute(char *interface, struct in_addr *route)
{
	char buff[1024], iface[16];
	char gate_addr[128], net_addr[128], mask_addr[128];
	int num, iflags, metric, refcnt, use, mss, window, irtt;
	FILE *fp = fopen(_PATH_PROCNET_ROUTE, "r");
	char *fmt;
	int found=0;
	unsigned long addr;

	if (!fp) {
		printf("Open %s file error.\n", _PATH_PROCNET_ROUTE);
		return 0;
    }

	fmt = "%16s %128s %128s %X %d %d %d %128s %d %d %d";

	while (fgets(buff, 1023, fp)) {
		num = sscanf(buff, fmt, iface, net_addr, gate_addr,
		     		&iflags, &refcnt, &use, &metric, mask_addr, &mss, &window, &irtt);
		if (num < 10 || !(iflags & RTF_UP) || !(iflags & RTF_GATEWAY) || strcmp(iface, interface))
	    		continue;
		sscanf(gate_addr, "%lx", &addr );
		*route = *((struct in_addr *)&addr);

		found = 1;
		break;
	}

    	fclose(fp);
    	return found;
}

int getInterfaces(char* lanIface,char* wanIface)
{
	int opmode=-1,wisp_wanid=0, wlan_idx_bak,vwlan_idx_bak;
	DHCP_T dhcp;
	
	if(!apmib_get( MIB_WAN_DHCP, (void *)&dhcp))
		return -1;
	if(!apmib_get(MIB_OP_MODE, (void *)&opmode))
		return -1;
	if(!lanIface||!wanIface)
	{
		fprintf(stderr,"invalid input!!\n");
		return -1;
	}
	strcpy(lanIface,"br0");
	switch(dhcp)
	{
	case DHCP_DISABLED:
	case DHCP_CLIENT:
	case DHCP_SERVER:        
		if(opmode==WISP_MODE)
		{
//#ifdef CONFIG_RTL_DUAL_PCIESLOT_BIWLAN
#ifdef CONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D
			apmib_get(MIB_WISP_WAN_ID,(void*)&wisp_wanid);
#endif
			//printf("%s %d wisp_wanid=%d\n", __FUNCTION__, __LINE__, wisp_wanid);
#if defined(CONFIG_SMART_REPEATER)				
			int wlan_mode,i=0;	
			wlan_idx_bak=wlan_idx;
			vwlan_idx_bak=vwlan_idx;
			//apmib_save_idx();
			for(i=0;i<NUM_WLAN_INTERFACE;i++)
			{
				//apmib_set_wlanidx(i);
				wlan_idx=i;
				apmib_get(MIB_WLAN_MODE, (void *)&wlan_mode);
				if(wlan_mode == CLIENT_MODE)
				{
					if(i==wisp_wanid)
						sprintf(wanIface, "wlan%d",i);													
				}else
				{
					if(i==wisp_wanid)
						sprintf(wanIface, "wlan%d-vxd",i);
				}
			}
			//apmib_revert_idx();	
			wlan_idx=wlan_idx_bak;
			vwlan_idx=vwlan_idx_bak;
			
#else
			if(wisp_wanid==0)
				strcpy(wanIface,"wlan0");
			else
				strcpy(wanIface,"wlan1");
#endif
		}
		else
			strcpy(wanIface,"eth1");
		break;
	case PPPOE:
	case L2TP:
	case PPTP:
		strcpy(wanIface,"ppp0");
		break;
	}
	return (0);
}

static inline int iw_get_ext(int skfd, char *ifname, int request, struct iwreq *pwrq)
{
	/* Set device name */
	strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
	/* Do the request */
	return(ioctl(skfd, request, pwrq));
}

int getWlBssInfo(char *interface, RTK_BSS_INFOp pInfo)
{
#ifndef NO_ACTION
	int skfd=0;
	struct iwreq wrq;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd==-1)
		return -1;
	/* Get wireless name */
	if ( iw_get_ext(skfd, interface, SIOCGIWNAME, &wrq) < 0)
	/* If no wireless name : no wireless extensions */
	{
		close( skfd );
		return -1;
	}

	wrq.u.data.pointer = (caddr_t)pInfo;
	wrq.u.data.length = sizeof(RTK_BSS_INFO);

	if (iw_get_ext(skfd, interface, SIOCGIWRTLGETBSSINFO, &wrq) < 0)
	{
		close( skfd );
		return -1;
	}
	close( skfd );
#else
	memset(pInfo, 0, sizeof(RTK_BSS_INFO)); 
#endif

	return 0;
}

static int getPid(char *filename)
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

	if ( rtk_getInAddr(name, IP_ADDR, (void *)&intaddr ) ) {
		snprintf(tmpBuf, 100, "%s/%s-%s.pid", _DHCPC_PID_PATH, _DHCPC_PROG_NAME, name);
		if ( getPid(tmpBuf) > 0)
			return 1;
	}
	return 0;
}

int getWlStaNum( char *interface, int *num )
{
#ifndef NO_ACTION
	int skfd=0;
	unsigned short staNum;
	struct iwreq wrq;

	skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd==-1)
		return -1;
	/* Get wireless name */
	if ( iw_get_ext(skfd, interface, SIOCGIWNAME, &wrq) < 0)
	{
	/* If no wireless name : no wireless extensions */
		close( skfd );
		return -1;
	}
	wrq.u.data.pointer = (caddr_t)&staNum;
	wrq.u.data.length = sizeof(staNum);

	if (iw_get_ext(skfd, interface, SIOCGIWRTLSTANUM, &wrq) < 0)
	{
		close( skfd );
		return -1;
	}
	*num  = (int)staNum;

	close( skfd );
#else
	*num = 0 ;
#endif

	return 0;
}

int check_wlan_downup(char wlanIndex)
{
	int sock=0,flags=0;
	struct  ifreq   ifr={0};
	snprintf(ifr.ifr_name,sizeof(ifr.ifr_name),"wlan%d",wlanIndex);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		printf("socket error!\n");
		return -1;
	}
	if (ioctl(sock, SIOCGIFFLAGS, (caddr_t)&ifr) < 0) 
	{
		printf("SIOCGIFFLAGS error!\n");
		close(sock);
		return (-1);
	}
	close(sock);
	if((ifr.ifr_flags & IFF_UP) != 0)
	{
		return 1;
	}
	return 0;
}

int SetWlan_idx(char * wlan_iface_name)
{
	int idx;
	
	idx = atoi(&wlan_iface_name[4]);
	if (idx >= NUM_WLAN_INTERFACE) {
			printf("invalid wlan interface index number!\n");
			return 0;
	}
	wlan_idx = idx;
	vwlan_idx = 0;
	
#ifdef MBSSID		
		
	if (strlen(wlan_iface_name) >= 9 && wlan_iface_name[5] == '-' &&
			wlan_iface_name[6] == 'v' && wlan_iface_name[7] == 'a') {
			idx = atoi(&wlan_iface_name[8]);
			if (idx >= NUM_VWLAN_INTERFACE) {
				printf("invalid virtual wlan interface index number!\n");
				return 0;
			}
			
			vwlan_idx = idx+1;
			idx = atoi(&wlan_iface_name[4]);
			wlan_idx = idx;
	}
#endif		

#ifdef UNIVERSAL_REPEATER
	if (strlen(wlan_iface_name) >= 9 && wlan_iface_name[5] == '-' &&
			!memcmp(&wlan_iface_name[6], "vxd", 3)) {
		vwlan_idx = NUM_VWLAN_INTERFACE;
		idx = atoi(&wlan_iface_name[4]);
		wlan_idx = idx;
	}
#endif				

	//printf("\r\n wlan_iface_name=[%s],wlan_idx=[%u],vwlan_idx=[%u],__[%s-%u]\r\n",wlan_iface_name,wlan_idx,vwlan_idx,__FILE__,__LINE__);

	return 1;		
}


#ifdef CONFIG_SMART_REPEATER
int getWispRptIface(char**pIface,int wlanId)
{
	int rptEnabled=0,wlanMode=0,opMode=0;
	char wlan_wanIfName[16]={0};
	if(wlanId == 0)
		apmib_get(MIB_REPEATER_ENABLED1, (void *)&rptEnabled);
	else if(1 == wlanId)
		apmib_get(MIB_REPEATER_ENABLED2, (void *)&rptEnabled);
	else return -1;
	apmib_get(MIB_OP_MODE,(void *)&opMode);
	if(opMode!=WISP_MODE)
		return -1;
	apmib_save_wlanIdx();
	
	sprintf(wlan_wanIfName,"wlan%d",wlanId);
	SetWlan_idx(wlan_wanIfName);
	//for wisp rpt mode,only care root ap
	apmib_get(MIB_WLAN_MODE, (void *)&wlanMode);
	if((AP_MODE==wlanMode || AP_MESH_MODE==wlanMode || MESH_MODE==wlanMode) && rptEnabled)
	{
		if(wlanId == 0)
			*pIface = "wlan0-vxd";
		else if(1 == wlanId)
			*pIface = "wlan1-vxd";
		else return -1;
	}else
	{
		char * ptmp = strstr(*pIface,"-vxd");
		if(ptmp)
			memset(ptmp,0,sizeof(char)*strlen("-vxd"));
	}
	apmib_recov_wlanIdx();
	return 0;
}
#endif

int isFileExist(char *file_name)
{
	struct stat status;

	if ( stat(file_name, &status) < 0)
		return 0;

	return 1;
}

int write_line_to_file(char *filename, int mode, char *line_data)
{
	unsigned char tmpbuf[512];
	int fh=0;

	if(mode == 1) {/* write line datato file */
		
		fh = open(filename, O_RDWR|O_CREAT|O_TRUNC);
		
	}else if(mode == 2){/*append line data to file*/
		
		fh = open(filename, O_RDWR|O_APPEND);	
	}
	
	
	if (fh < 0) {
		fprintf(stderr, "Create %s error!\n", filename);
		return 0;
	}


	sprintf((char *)tmpbuf, "%s", line_data);
	write(fh, tmpbuf, strlen((char *)tmpbuf));



	close(fh);
	return 1;
}


