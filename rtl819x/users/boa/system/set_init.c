#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include "apmib.h"
#include "mibtbl.h"
#include "upmib.h"
#include "sysconf.h"
#include "sys_utility.h"
#include "syswan.h"
//extern int wlan_idx;	// interface index 
//extern int vwlan_idx;	// initially set interface index to root   
extern int set_QoS(int operation, int wan_type, int wisp_wan_id);
extern int setbridge(char *argv);
extern int setWlan_Applications(char *action, char *argv);
extern int SetWlan_idx(char *wlan_iface_name);
extern int setFirewallIptablesRules(int argc, char** argv);
extern void set_lan_dhcpd(char *interface, int mode);
#ifdef MULTI_PPPOE
extern void wan_disconnect(char *option , char *conncetOrder);
#else
extern void wan_disconnect(char *option);
#endif
extern void set_ipv6();
void set_log(void);
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
extern void Init_Domain_Query_settings(int operation_mode, int wlan_mode, int lan_dhcp_mode, char *lan_mac);
extern void domain_query_start_dnrd(int wlan_mode, int start_dnrd);
#endif
#ifdef CONFIG_SMART_REPEATER
int getWispRptIfaceName(char*pIface,int wlanId);
#endif
#if defined(CONFIG_APP_TR069)
extern void start_tr069(void);
#endif

//#define SDEBUG(fmt, args...) printf("[%s %d]"fmt,__FUNCTION__,__LINE__,## args)
#define SDEBUG(fmt, args...) {}

/*

//eth0 eth1 eth2 eth3 eth4 wlan0 wlan0-msh wlan0-va0 wlan0-va1 wlan0-va2 wlan0-va3 wlan0-vxd
//wlan0-wds0 wlan0-wds1 wlan0-wds2 wlan0-wds3 wlan0-wds4 wlan0-wds5 wlan0-wds6 wlan0-wds7

WLAN=>>> wlan0
WLANVXD=>>>wlan0-vxd
WLANVIRTUAL=>> wlan0-va0 wlan0-va1 wlan0-va2 wlan0-va3 wlan0-vxd
NUM_=>>>1
VIRTUALNUL=>>>4

*/

int gateway=0;
int enable_wan=0;
int enable_br=0;
char br_interface[16]={0};
char br_lan1_interface[16]={0};
char br_lan2_interface[16]={0};
char wan_interface[16]={0};
char vlan_interface[32]={0};
static char wlan_interface[16]={0};
char wlan_valid_interface[512]={0};
char wlan_virtual_interface[80]={0};
static char wlan_vxd_interface[32]={0};
int num_wlan_interface=0;
int num_wlan_virtual_interface=0;
int num_wlan_vxd_interface=0;


#if defined(CONFIG_RTK_MESH)
char wlan_mesh_interface[22]={0};
int num_wlan_mesh_interface=0;
#endif

#ifdef CONFIG_APP_TR069
char acsURLStr[CWMP_ACS_URL_LEN+1];
#endif //#ifdef CONFIG_APP_TR069

//#define UPGRADE_BOOT_FROM_ROOT
#ifdef SAMBA_WEB_SUPPORT
void storage_UpdateSambaConf()
{
	char 				tmpBuff[1024];
	int					number,i;
	STORAGE_GROUP_T		group_info;
	int					anonAccess,anonSambaAccess;
	FILE				*fp;
	memset(tmpBuff,'\0',1024);
	fp = fopen("/etc/samba/smb.conf","w+");

	apmib_get(MIB_STORAGE_ANON_ENABLE,(void*)&anonAccess);
	apmib_get(MIB_STORAGE_ANON_DISK_ENABLE,(void*)&anonSambaAccess);
	
	strcpy(tmpBuff,"[global]\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tserver string\t= RTCN 8190 Samba Server\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tlog level\t= 0\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tmax xmit\t= 65536\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\taio write size\t= 65536\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\taio read size\t= 65536\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tlarge readwrite =\tyes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tgetwd cache =\tyes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tread raw\t= yes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\twrite raw\t= yes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tlpq cache\t= 30\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\toplocks =\tyes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\twinbind nested groups\t= no\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tdomain master\t= no\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tlocal master\t= yes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tpublic\t= yes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tinterfaces\t= br0\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tload printers\t= no\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tprinting\t= bsd\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tprintcap name\t= /dev/null\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tdisable spoolss\t= yes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tlog file\t= /var/log/log.%m\n");
	if(anonAccess == 1 && anonSambaAccess == 1)
		strcpy(tmpBuff+strlen(tmpBuff),"\tsecurity\t= share\n");
	else
		strcpy(tmpBuff+strlen(tmpBuff),"\tsecurity\t= user\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tsocket options\t= IPTOS_LOWDELAY IPTOS_THROUGHPUT TCP_NODELAY SO_KEEPALIVE TCP_FASTACK SO_RCVBUF=65536 SO_SNDBUF=65536\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tgetwd cache\t= yes\n");
	strcpy(tmpBuff+strlen(tmpBuff),"\tdns proxy\t= no\n");

	//anonymous shared folder informatio
	if(anonAccess == 1 && anonSambaAccess == 1){
		strcpy(tmpBuff+strlen(tmpBuff),"[share]\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tpath\t=/tmp/usb\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tcomment\t= anonymous user's share folder\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tread only\t= no\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\twritable\t= yes\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tpublic\t= yes\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\toplocks\t= no\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tkernel oplocks\t= no\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tcreate mask\t= 0777\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tbrowseable\t= yes\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tguest ok\t= yes\n");
		strcpy(tmpBuff+strlen(tmpBuff),"\tdirectory mask\t= 0777\n");
		goto setOk_SambaConfUpdate;
	}

	//shared folder information
	apmib_get(MIB_STORAGE_GROUP_TBL_NUM,(void*)&number);
	for(i = 0;i < number;i++)
	{
		*((char*)&group_info) = (char)(i+1);
		apmib_get(MIB_STORAGE_GROUP_TBL,(void*)&group_info);

		if(group_info.storage_group_sharefolder_flag == 1){
			snprintf(tmpBuff+strlen(tmpBuff),1024-strlen(tmpBuff),"[%s]\n",group_info.storage_group_displayname);
			snprintf(tmpBuff+strlen(tmpBuff),1024-strlen(tmpBuff),"\tpath\t=%s\n",group_info.storage_group_sharefolder);
			strcpy(tmpBuff+strlen(tmpBuff),"\tcomment\t= smbuser's share folder\n");
			snprintf(tmpBuff+strlen(tmpBuff),1024-strlen(tmpBuff),"\tvalid users\t=@%s\n",group_info.storage_group_name);			
			
			if(!strcmp(group_info.storage_group_access,"r-w"))
				snprintf(tmpBuff+strlen(tmpBuff),1024-strlen(tmpBuff),"\twrite list\t=@%s\n",group_info.storage_group_name);
			else if(!strcmp(group_info.storage_group_access,"r"))
				snprintf(tmpBuff+strlen(tmpBuff),1024-strlen(tmpBuff),"\tread list\t=@%s\n",group_info.storage_group_name);
		}
	}
	
setOk_SambaConfUpdate:

	fwrite(tmpBuff,strlen(tmpBuff),1,fp);
	fclose(fp);

	system("killall smbd");
	system("smbd -D");
}
#endif
#ifdef TR181_SUPPORT
static int init_dns_client_server_table = 0;
#endif

void set_br_interface(unsigned char *brif)
{

	int opmode=-1;
	unsigned char tmpBuff[512]={0};
  apmib_get(MIB_OP_MODE,(void *)&opmode);
	
	if(vlan_interface[0]){
		strcat(tmpBuff, vlan_interface);
		strcat(tmpBuff, " ");
	}
	if(wlan_interface[0]){
		strcat(tmpBuff, wlan_interface);
		strcat(tmpBuff, " ");
	}			
	if(wlan_virtual_interface[0]){
		strcat(tmpBuff, wlan_virtual_interface);
		strcat(tmpBuff, " ");
	}
#if defined(CONFIG_RTL_8198_AP_ROOT) || defined(CONFIG_RTL_8197D_AP)
	if(br_lan2_interface[0]){
		strcat(tmpBuff, br_lan2_interface);
		strcat(tmpBuff, " ");
	}
#endif
	if(wlan_vxd_interface[0]){
		strcat(tmpBuff, wlan_vxd_interface);
		strcat(tmpBuff, " ");
	}
#if defined(CONFIG_RTK_MESH)
	if(wlan_mesh_interface[0]){
		strcat(tmpBuff, wlan_mesh_interface);
		strcat(tmpBuff, " ");
	}
#endif

	strcat(tmpBuff, br_interface);
	strcat(tmpBuff, " ");
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
	if(opmode == GATEWAY_MODE)
	{
		strcat(tmpBuff, "eth0 eth2 eth3 eth4");
	}
	else
	{
		strcat(tmpBuff, "eth0 eth1 eth2 eth3 eth4");
	}	
#else
	
	strcat(tmpBuff, br_lan1_interface);

#if defined(CONFIG_RTL_IVL_SUPPORT)
	/*add eth1 to br0 when in bridge&wisp mode*/
	if(opmode == BRIDGE_MODE || opmode == WISP_MODE) 
	{
		strcat(tmpBuff, " ");
		strcat(tmpBuff, br_lan2_interface);
	}
#endif
	#if !defined(CONFIG_RTL_MULTI_LAN_DEV) && !defined(CONFIG_RTL_IVL_SUPPORT)
	/* rtk vlan enable and in bridge&wisp mode and disable ivl and disable multi lan, add eth1 to br0 */
	if (vlan_interface[0])
	{
		if(opmode == BRIDGE_MODE || opmode == WISP_MODE) 
		{
			strcat(tmpBuff, " ");
			strcat(tmpBuff, br_lan2_interface);
		}
	}
	#endif
#endif

#if defined(CONFIG_RTL_ULINKER)
	strcat(tmpBuff, " ");
	strcat(tmpBuff, "usb0");
#endif
	memcpy(brif, tmpBuff, sizeof(tmpBuff));
	return;

}
int _is_hex(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||
            ((c >= 'a') && (c <= 'f')));
}
int _string_to_hex(char *string, unsigned char *key, int len)
{
	char tmpBuf[4];
	int idx, ii=0;
	for (idx=0; idx<len; idx+=2) {
		tmpBuf[0] = string[idx];
		tmpBuf[1] = string[idx+1];
		tmpBuf[2] = 0;
		if ( !_is_hex(tmpBuf[0]) || !_is_hex(tmpBuf[1]))
		{
			return 0;
		}

		key[ii++] = (unsigned char) strtol(tmpBuf, (char**)NULL, 16);
	}
	return 1;
}

char * strToValue(char *str,TYPE_T type)
{
	switch(type)
	{
		case BYTE_T:
		case WORD_T:
		case DWORD_T:
		{
			int i=atoi(str);
			*((int*)str)=i;
			return str;
		}
		case STRING_T:
			return str;
		case IA_T:
		{
			struct in_addr addr;
			inet_aton(str,&addr);
			*((struct in_addr*)str)=addr;
			return str;
		}
		case DHCPRSVDIP_ARRY_T:
		{
			DHCPRSVDIP_T staticIPEntry={0};
			char ipAddr[32]={0};
			char macAddr[32]={0};
#ifdef SUPPORT_DHCP_PORT_IP_BIND
			char portId[8]={0};
			sscanf(str,"%s\t%s\t%s\t%s",ipAddr,portId,macAddr,staticIPEntry.hostName);
			staticIPEntry.portId=atoi(portId);
#else
			sscanf(str,"%s\t%s\t%s",ipAddr,macAddr,staticIPEntry.hostName);
#endif
			inet_aton(ipAddr,(struct in_addr *)staticIPEntry.ipAddr);
			_string_to_hex(macAddr,staticIPEntry.macAddr,12);
			*((DHCPRSVDIP_T*)str)=staticIPEntry;
			return str;
		}
		default:
			return str;
	}
}
#if defined(UPGRADE_BOOT_FROM_ROOT)
void upgrade_boot()
{
	int filelen;
	char *msg;
	FILE *src, *dst;
	struct stat buf;

	stat("/etc/boot.bin", &buf);
	filelen=buf.st_size;

	src = fopen("/etc/boot.bin", "rb");
	dst = fopen("/dev/mtdblock0", "wb");

	if(src && dst) 
	{
		msg = (char *)malloc(filelen*sizeof(char));
		fread(msg, filelen, 1, src);
		if(!memcmp(msg, "boot", 4))
		{
			fwrite(&msg[sizeof(IMG_HEADER_T)], filelen, 1, dst);
		}
		free(msg);
		fclose(src);
		fclose(dst);
	}

}
#endif

int up_mib_value()
{
        int old_ver=0;
        int new_ver=0;
        int i=0;
 
        apmib_get(MIB_MIB_VER, (void *)&old_ver);
#ifdef RTL_DEF_SETTING_IN_FW
		apmib_getDef(MIB_MIB_VER,(void *)&new_ver);
#else
        new_ver = atoi(update_mib[0].value);
#endif

        if(old_ver == new_ver)
        {
                return -1;
        }
        else
                printf("MIB Version update!\n");
		
		
 
#if defined(UPGRADE_BOOT_FROM_ROOT)
	 upgrade_boot();
#endif
#ifdef RTL_DEF_SETTING_IN_FW
		if(new_mib[0].id==0&&update_mib[1].id==0)
		{
			system("flash reset");
			return 0;
		}
#endif		
        i=0;
        while(new_mib[i].id != 0)
        {
        		apmib_set(new_mib[i].id,strToValue(new_mib[i].value,new_mib[i].type)); 
                //RunSystemCmd(NULL_FILE, "flash", "set", new_mib[i].name, new_mib[i].value, NULL_STR);
                i++;
        }
 
        i=0;
        while(update_mib[i].id != 0)
        {
        	apmib_set(update_mib[i].id,strToValue(update_mib[i].value,update_mib[i].type));        	
              //  RunSystemCmd(NULL_FILE, "flash", "set", update_mib[i].name, update_mib[i].value, NULL_STR);
                i++;
        }
 		apmib_update(CURRENT_SETTING);
        return 0;
 
}


void set_log(void)
{
	int intValue=0,  intValue1=0;
	char tmpBuffer[32];
	char syslog_para[32];
	char localServer[32];
	
	apmib_get(MIB_SCRLOG_ENABLED, (void*)&intValue);
	if(intValue !=0 && intValue !=2 && intValue !=4 && intValue !=6 && intValue !=8 &&
		intValue !=10 && intValue !=12 && intValue !=14) {
			apmib_get(MIB_REMOTELOG_ENABLED, (void*)&intValue1);
			if(intValue1 != 0){
				apmib_get(MIB_REMOTELOG_SERVER,  (void *)tmpBuffer);
				if (memcmp(tmpBuffer, "\x0\x0\x0\x0", 4)){
#ifdef RINGLOG /* ring log */
					sprintf(localServer, "%s", inet_ntoa(*((struct in_addr *)tmpBuffer)));
					system("rm /var/log/log_split >/dev/null 2>&1");
					sprintf(tmpBuffer, "echo %d > /var/log/log_split", LOG_SPLIT);
					system(tmpBuffer);
					sprintf(tmpBuffer, "%d", MAX_LOG_SIZE);
					sprintf(syslog_para, "%d", LOG_SPLIT);
					fprintf(stderr, "syslog will use %dKB for log(%d rotate, 1 original, %dKB for each)\n",
						MAX_LOG_SIZE * (LOG_SPLIT+1), LOG_SPLIT, MAX_LOG_SIZE);
					RunSystemCmd(NULL_FILE, "syslogd", "-L","-R", localServer, "-s", tmpBuffer, "-b", syslog_para, NULL_STR);
#else
					sprintf(syslog_para, "%s", inet_ntoa(*((struct in_addr *)tmpBuffer)));
					RunSystemCmd(NULL_FILE, "syslogd", "-L", "-R", syslog_para, NULL_STR);
#endif					
				}
			}else{
			
//### add by sen_liu 2011.4.21 sync the system log update (enlarge from 1 pcs to 8 pcs) to	SDKv2.5 from kernel 2.4
#ifdef RINGLOG /* ring log */
				system("rm /var/log/log_split >/dev/null 2>&1");
				sprintf(tmpBuffer, "echo %d > /var/log/log_split", LOG_SPLIT);
				system(tmpBuffer);
				sprintf(tmpBuffer, "%d", MAX_LOG_SIZE);
				sprintf(syslog_para, "%d", LOG_SPLIT);
				fprintf(stderr, "syslog will use %dKB for log(%d rotate, 1 original, %dKB for each)\n",
					MAX_LOG_SIZE * (LOG_SPLIT+1), LOG_SPLIT, MAX_LOG_SIZE);
				RunSystemCmd(NULL_FILE, "syslogd", "-L", "-s", tmpBuffer, "-b", syslog_para, NULL_STR);
#else
//### end
				RunSystemCmd(NULL_FILE, "syslogd", "-L", NULL_STR);
#endif
			}
			RunSystemCmd(NULL_FILE, "klogd", NULL_STR);
		} 
		 
	
	return;
}

void start_wlanapp(int action)
{
	char tmpBuff[128];

#if defined(CONFIG_RTL_ULINKER_WLAN_DELAY_INIT)
	int ulinker_auto = 0;
	apmib_get(MIB_ULINKER_AUTO,  (void *)&ulinker_auto);
	if (ulinker_auto == 1) {
		if (isFileExist("/var/ulinker_init2")==0) {
			BDBG_FPRINT(stderr, "[%s:%d] break!\n", __FUNCTION__, __LINE__);
			return 0;
		}
		else {
			BDBG_FPRINT(stderr, "[%s:%d] exec\n", __FUNCTION__, __LINE__);
		}
	}
#endif

#if defined(CONFIG_RTL_ULINKER)
	/* notice ulinker_process to reset domain name query */
	system("echo 1 > /var/ulinker_reset_domain");
#endif

	memset(tmpBuff, 0x00, sizeof(tmpBuff));
	if(action==1){
		if(wlan_interface[0] && wlan_virtual_interface[0] && wlan_vxd_interface[0] && br_interface[0])
		sprintf(tmpBuff, "%s %s %s %s", wlan_interface, wlan_virtual_interface, wlan_vxd_interface, br_interface); 			
		else if(wlan_interface[0] && wlan_virtual_interface[0] && !wlan_vxd_interface[0] && br_interface[0])
			sprintf(tmpBuff, "%s %s %s", wlan_interface, wlan_virtual_interface, br_interface); 
		else if(wlan_interface[0] && !wlan_virtual_interface[0] && wlan_vxd_interface[0] && br_interface[0])
			sprintf(tmpBuff, "%s %s %s", wlan_interface, wlan_vxd_interface, br_interface); 	
		else if(wlan_interface[0] && !wlan_virtual_interface[0] && !wlan_vxd_interface[0] && br_interface[0])
			sprintf(tmpBuff, "%s %s", wlan_interface, br_interface); 						
	}else {
		//V_WLAN_APP_ENABLE=0 or para2=wlan_app
		if(wlan_interface[0] && br_interface[0])
		sprintf(tmpBuff, "%s %s", wlan_interface, br_interface); 	
	}

	RunSystemCmd(PROC_GPIO, "echo", "I", NULL_STR);

//printf("\r\n tmpBuff=[%s],__[%s-%u]\r\n",tmpBuff,__FILE__,__LINE__);

	if(tmpBuff[0])
		setWlan_Applications("start", tmpBuff);

	#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT) || defined(CONFIG_APP_APPLE_MFI_WAC)
		system("rm -f  /var/system/start_init 2> /dev/null");
	#endif

}

void start_upnpd(int isgateway, int sys_op)
{
#ifdef   HOME_GATEWAY	
	int intValue=0,  intValue1=0;
	if(SetWlan_idx("wlan0")){
		apmib_get(MIB_WLAN_WSC_DISABLE, (void *)&intValue);
	}
	if(isgateway==1 && sys_op !=1)
		apmib_get(MIB_UPNP_ENABLED, (void *)&intValue1);
	else 
		intValue1=0;
	if(intValue==0 && intValue1==0){//wps enabled and upnp igd is disabled
		RunSystemCmd(NULL_FILE, "mini_upnpd", "-wsc", "/tmp/wscd_config", "-daemon", NULL_STR);
		//system("mini_upnpd -wsc /tmp/wscd_config &");
	}else if(intValue==1 && intValue1==1){//wps is disabled, and upnp igd is enabled
		RunSystemCmd(NULL_FILE, "mini_upnpd", "-igd", "/tmp/igd_config", "-daemon", NULL_STR);
		//system("mini_upnpd -igd /tmp/igd_config &");
	}else if(intValue==0 && intValue1==1){//both wps and upnp igd are enabled
		RunSystemCmd(NULL_FILE, "mini_upnpd", "-wsc", "/tmp/wscd_config", "-igd", "/tmp/igd_config","-daemon",  NULL_STR);
		//system("mini_upnpd -wsc /tmp/wscd_config -igd /tmp/igd_config &");
	}else if(intValue==1 && intValue1==0){//both wps and upnp igd are disabled
		/*do nothing*/
	}
#endif		
}
/*method to start reload is co-operate to parse rule of reload.c*/
void start_wlan_by_schedule(int index)
{
	int intValue=0,  intValue1=0, i=0, entryNum=0, bak_idx=0, bak_vidx=0;
	char tmp1[64]={0};
	SCHEDULE_T wlan_sched;
	int newfile=1;

	bak_idx=wlan_idx;
	wlan_idx=index;
	bak_idx=vwlan_idx;
	vwlan_idx=0;
	apmib_get(MIB_WLAN_WLAN_DISABLED, (void *)&intValue);
#if defined(NEW_SCHEDULE_SUPPORT)
	#define WLAN_SCHEDULE_FILE "/var/wlsch.conf"
	sprintf(tmp1,WLAN_SCHEDULE_FILE"%d",index);
	unlink(tmp1);
#endif	

	if(intValue==0){
		apmib_get(MIB_WLAN_SCHEDULE_ENABLED, (void *)&intValue1);
		apmib_get(MIB_WLAN_SCHEDULE_TBL_NUM, (void *)&entryNum);

		if(intValue1==1 && entryNum > 0){
			
			for (i=1; i<=entryNum; i++) {
				*((char *)&wlan_sched) = (char)i;
				apmib_get(MIB_WLAN_SCHEDULE_TBL, (void *)&wlan_sched);
#if defined(NEW_SCHEDULE_SUPPORT)

				char line_buffer[100]={0};
				if(wlan_sched.eco == 1 && !(wlan_sched.fTime == 0 && wlan_sched.tTime == 0))
				{
					sprintf(line_buffer,"%d,%d,%d,%d\n",i,wlan_sched.day,wlan_sched.fTime, wlan_sched.tTime);
					sprintf(tmp1,WLAN_SCHEDULE_FILE"%d",index);
					write_line_to_file(tmp1, (newfile==1?1:2), line_buffer);
					newfile = 2;
				}
#endif				
			}
			if(index == (NUM_WLAN_INTERFACE-1)){
#if defined(NEW_SCHEDULE_SUPPORT)
				sprintf(tmp1, "reload -k %s &", WLAN_SCHEDULE_FILE);
#else			
				sprintf(tmp1, "reload -e %d,%d,%d,%d,%s &", wlan_sched.eco, wlan_sched.fTime, wlan_sched.tTime, wlan_sched.day, wlan_sched.text);
#endif			
				system(tmp1);
			}
		}
		else{ /* do not care schedule*/

			if(index == (NUM_WLAN_INTERFACE-1)){
#if defined(NEW_SCHEDULE_SUPPORT)
				sprintf(tmp1, "reload -k %s &", WLAN_SCHEDULE_FILE);
				system(tmp1);
#else
				system("reload &");
#endif
			}
		}
	}
	else{
		/*wlan is disabled, we do not care wlan schedule*/
		if(index ==(NUM_WLAN_INTERFACE-1)){
#if defined(NEW_SCHEDULE_SUPPORT)
			sprintf(tmp1, "reload -k %s &", WLAN_SCHEDULE_FILE);
			system(tmp1);
#else
			system("reload &");
#endif
		}
	}

	vwlan_idx=bak_vidx;
	wlan_idx=bak_idx;
}
/* 
* dhcpc daemon pid file name must be /etc/udhcpc/udhcpc-*.pid
*/
int killDhcpcDaemons()
{
	DIR *dir;
	struct dirent *next;
	char dhcpPidPath[256]={"/etc/udhcpc/"};

	dir = opendir("/etc/udhcpc");
	if (!dir) 
	{
		printf("Cannot open /etc/udhcpc");
		return -1;
	}
	
	while ((next = readdir(dir)) != NULL)
	{
		if(strncmp("udhcpc-",next->d_name,strlen("udhcpc-"))==0
		&& strncmp(".pid",next->d_name+strlen(next->d_name)-strlen(".pid"),strlen(".pid"))==0)
		{//begin with "udhcpc-" end with ".pid" 
			strcpy(dhcpPidPath,"/etc/udhcpc/");
			strcat(dhcpPidPath,next->d_name);
			killDaemonByPidFile(dhcpPidPath);
		}
	}
	closedir(dir);
	return 0;
}
void clean_process(int sys_opmode,int wan_dhcp_mode,int gateway, int enable_wan, char *lanInterface, char *wlanInterface, char *wanInterface)
{
	char strPID[10], tmpBuff[200];
	int pid=-1;
	int i;
/*clean the process before take new setting*/	
#ifdef   HOME_GATEWAY
#ifdef CONFIG_POCKET_AP_SUPPORT
#else
	if(isFileExist(HW_NAT_FILE)){/*hw nat supported*/
		/*cleanup hardware tables*/
		if(sys_opmode==0)
			RunSystemCmd(HW_NAT_FILE, "echo", "1", NULL_STR);	/*gateway mode*/
		else if(sys_opmode==1)
			RunSystemCmd(HW_NAT_FILE, "echo", "2", NULL_STR);	/*bridge mode*/
		else if(sys_opmode==2)
			RunSystemCmd(HW_NAT_FILE, "echo", "3", NULL_STR);	/*wisp mode*/
		else if(sys_opmode==3)
			RunSystemCmd(HW_NAT_FILE, "echo", "4", NULL_STR);	/*bridge mode with multiple vlan*/
		else
			RunSystemCmd(HW_NAT_FILE, "echo", "5", NULL_STR); /*wisp mode with multiple vlan*/
		
	}else{/*software nat supported*/ 
	if(isFileExist(SOFTWARE_NAT_FILE)){	
		if(sys_opmode==0)
		{
#ifdef RTK_USB3G
			if(wan_dhcp_mode == USB3G)
      	RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "1", NULL_STR);
			else
#endif
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "0", NULL_STR);
		}
		if(sys_opmode==1)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "1", NULL_STR);
		if(sys_opmode==2)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "2", NULL_STR);
		if(sys_opmode==3)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "3", NULL_STR);
		if(sys_opmode==4)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "4", NULL_STR);
	}
		
	}
#endif	//CONFIG_POCKET_AP_SUPPORT
#endif	
	
		RunSystemCmd(NULL_FILE, "killall", "-15", "miniigd", NULL_STR);
		if(isFileExist(IGD_PID_FILE)){
			unlink(IGD_PID_FILE);
		}		
		RunSystemCmd(NULL_FILE, "killall", "-15", "routed", NULL_STR);
		if(isFileExist(RIP_PID_FILE)){
			unlink(RIP_PID_FILE);
		}	
#ifdef RIP6_SUPPORT
		RunSystemCmd(NULL_FILE, "killall", "-15", "bird6", NULL_STR);
#endif
		
#if defined(CONFIG_APP_TR069) // Move to webpage form handler when user modify setting
		/* Keep Tr069 alive unless 1.ACSURL changed 2.Turn off tr069 */
		if ((pid=find_pid_by_name("cwmpClient")) > 0)
		//if(isFileExist(TR069_PID_FILE))
		{									
			unsigned char acsUrltmp[CWMP_ACS_URL_LEN+1];
			int tr069Flag;
			
			apmib_get( MIB_CWMP_ACS_URL_OLD, (void *)acsUrltmp);
			apmib_get( MIB_CWMP_ACS_URL, (void *)acsURLStr);
			apmib_get( MIB_CWMP_FLAG, (void *)&tr069Flag);
			
			if(strcmp(acsUrltmp, acsURLStr) != 0 || ((tr069Flag & CWMP_FLAG_AUTORUN) == 0))
			{
			//pid=getPid_fromFile(TR069_PID_FILE);
			if(pid != -1){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-15", strPID, NULL_STR);
			}
			unlink(TR069_PID_FILE);
		}
		}
#endif //#if defined(CONFIG_APP_TR069)	
	
		//RunSystemCmd(NULL_FILE, "killall", "-9", "pptp.sh", NULL_STR);
		//RunSystemCmd(NULL_FILE, "killall", "-9", "pppoe.sh", NULL_STR);
		//RunSystemCmd(NULL_FILE, "killall", "-9", "l2tp.sh", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/first", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstpptp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstl2tp", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/etc/ppp/firstdemand", NULL_STR);
		//RunSystemCmd(NULL_FILE, "disconnect.sh", "all", NULL_STR);
		RunSystemCmd(NULL_FILE, "killall", "-9", "timelycheck", NULL_STR); //LZQ
		#ifdef CONFIG_AUTO_DHCP_CHECK
		RunSystemCmd(NULL_FILE, "killall", "-9", "Auto_DHCP_Check", NULL_STR);
		#endif


#if defined(CONFIG_APP_FWD)
		RunSystemCmd(NULL_FILE, "killall", "-9", "fwd", NULL_STR); //LZQ
#endif		
#ifdef   HOME_GATEWAY		
#ifdef MULTI_PPPOE
				wan_disconnect("all","");
#else
				wan_disconnect("all");
#endif
#endif			
	RunSystemCmd(NULL_FILE, "killall", "-9", "ntp_inet", NULL_STR);	
	RunSystemCmd(NULL_FILE, "killall", "-9", "ddns.sh", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "syslogd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "klogd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "mini_upnpd", NULL_STR);
	RunSystemCmd(NULL_FILE, "killall", "-9", "reload", NULL_STR);
#if defined(CONFIG_APP_RTK_INBAND_CTL)
   	RunSystemCmd(NULL_FILE, "killall", "-9", "hcd", NULL_STR);
#endif
	//RunSystemCmd(NULL_FILE, "killall", "-9", "ntfs-3g", NULL_STR);
	if(isFileExist(L2TPD_PID_FILE)){
			pid=getPid_fromFile(L2TPD_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(L2TPD_PID_FILE);
	}
	/* kill dhcp client */
	killDhcpcDaemons();
	sprintf(tmpBuff, "/etc/udhcpc/udhcpc-%s.pid", "br0");
	if(isFileExist(tmpBuff)){
		unlink(tmpBuff);
	}
#if 0
/*kill dhcp client if br interface is dhcp client*/	
	sprintf(tmpBuff, "/etc/udhcpc/udhcpc-%s.pid", lanInterface);
	if(isFileExist(tmpBuff)){
			pid=getPid_fromFile(tmpBuff);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(tmpBuff);
	}
	sprintf(tmpBuff, "/etc/udhcpc/udhcpc-%s.pid", wanInterface);
	if(isFileExist(tmpBuff)){
			pid=getPid_fromFile(tmpBuff);
			if(pid !=0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(tmpBuff);
	}
	
	#if defined(CONFIG_RTL_92D_SUPPORT)
	for(i=0;i<NUM_WLAN_INTERFACE;i++)
	{
		sprintf(tmpBuff, "/etc/udhcpc/udhcpc-wlan%d.pid",i);
		if(isFileExist(tmpBuff)){
				pid=getPid_fromFile(tmpBuff);
				if(pid != 0){
					sprintf(strPID, "%d", pid);
					RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
				}
				unlink(tmpBuff);
		}
	}
	#else
	if(wlanInterface[0]){
	sprintf(tmpBuff, "/etc/udhcpc/udhcpc-%s.pid", wlanInterface);
	if(isFileExist(tmpBuff)){
			pid=getPid_fromFile(tmpBuff);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				printf("%s:%d,tmpBuff is %s\n",__FUNCTION__,__LINE__,tmpBuff);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(tmpBuff);
	}
	}
	#endif
#endif
	
	if(isFileExist(DNRD_PID_FILE)){
			pid=getPid_fromFile(DNRD_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(DNRD_PID_FILE);
	}
	if(isFileExist(IGMPPROXY_PID_FILE)){
			pid=getPid_fromFile(IGMPPROXY_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(IGMPPROXY_PID_FILE);
			RunSystemCmd(PROC_BR_MCASTFASTFWD, "echo", "1,1", NULL_STR);
	}
	if(isFileExist(LLTD_PID_FILE)){
			pid=getPid_fromFile(LLTD_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(LLTD_PID_FILE);
	}
	if(isFileExist(DHCPD_PID_FILE)){
			pid=getPid_fromFile(DHCPD_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-16", strPID, NULL_STR);/*inform dhcp server write lease table to file*/
				sleep(1);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(DHCPD_PID_FILE);
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

	/*MLD proxy*/
	if(isFileExist(MLDPROXY_PID_FILE)){
			pid=getPid_fromFile(MLDPROXY_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "kill", "-9", strPID, NULL_STR);
			}
			unlink(MLDPROXY_PID_FILE);			
	}

	/* add for rm the configure file */
	RunSystemCmd(NULL_FILE, "rm", "-f", DHCP6S_CONF_FILE, NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", "-f", DHCP6C_CONF_FILE, NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", "-f", DNSV6_CONF_FILE, NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", "-f", RADVD_CONF_FILE, NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", "-f", DHCP6PD_CONF_FILE, NULL_STR);
	RunSystemCmd(NULL_FILE, "rm", "-f", DNSV6_ADDR_FILE, NULL_STR);
	/* end add */
#endif
/*end of clean the process before take new setting*/		
	
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
	/* Reset Firewall Rules*/
	system("iptables -F");
	system("iptables -F -t nat");
	system("iptables -F -t mangle");
	sprintf(tmpBuff, "iptables -A INPUT -i %s -j ACCEPT", lanInterface);
	system(tmpBuff);
	if(sys_opmode==1){
		system("iptables -P INPUT ACCEPT");
		system("iptables -P FORWARD ACCEPT");
	}
#endif	
	
#if defined(CONFIG_APP_SIMPLE_CONFIG)
	system("killall simple_config");
#endif
#if defined(CONFIG_APP_APPLE_MFI_WAC)
	system("killall wfaudio");
	system("killall WACServer");
#if defined(CONFIG_APPLE_HOMEKIT)
	system("killall hapserver");
#endif
#endif
}
#if defined(CONFIG_APP_USBMOUNT)
#define	PARTITION_FILE "/proc/partitions"

int get_blockDevPartition(char *str, char *partition)
{

	unsigned char tk[50];
	unsigned int i,j;
	unsigned int curCnt,preCnt;
	
	if(str==NULL)
	{
		return -1;
	}
	
	memset(tk,0, sizeof(tk));

	/*	partition table format:
		major minor  #blocks  name
	*/
	
	preCnt=0;
	curCnt=0;
	for (i=0;i<strlen(str);i++)
	{          
		if( (str[i]!=' ') && (str[i]!='\n') && (str[i]!='\r'))
		{
			if(preCnt==curCnt)
			{
				tk[curCnt]=i;
				curCnt++;
			}
		}
		else if((str[i]==' ') || (str[i]=='\n') ||(str[i]=='\r') )
		{
			preCnt=curCnt;
		}
	}
	
	/*to check device major number is 8*/
	
	if(!isdigit(str[tk[0]]))
	{
		return -1;
	}

	if(tk[1]==0)
	{
		return -1;
	}

	if(tk[1]<=tk[0])
	{
		return -1;
	}

	if((str[tk[0]]!='8') ||(str[tk[0]+1]!=' '))
	{
		return -1;
	}
	
	if(tk[3]==0)
	{
		return -1;
	}

	/*to get partition name*/
	j=0;
	for(i=tk[3]; i<strlen(str); i++)
	{
		
		if((str[i]==' ') || (str[i]=='\n') ||(str[i]=='\n'))
		{
			partition[j]='\0';
			return 0;
		}
		else
		{
			partition[j]=str[i];
			j++;
		}
			
	}
	return 0;
}
int Check_shouldMount(char *partition_name)
{
	DIR *dir=NULL;
	struct dirent *next;
	int found=0;
	dir = opendir("/tmp/usb");
	if (!dir) {
		printf("Cannot open %s", "/tmp/usb");
		return -1;
	}
	while ((next = readdir(dir)) != NULL) {
		//printf("Check_shouldMount:next->d_reclen=%d, next->d_name=%s\n",next->d_reclen, next->d_name);
			/* Must skip ".." */
			if (strcmp(next->d_name, "..") == 0)
				continue;
			if (strcmp(next->d_name, ".") == 0)
				continue;
			if (strcmp(next->d_name, "mnt_map") == 0)
				continue;
			if(!strcmp(next->d_name, partition_name)){
				found=1;
				break;
			}
	}
	closedir(dir);
	return found;
}
void autoMountOnBootUp(void)
{
	FILE *fp;
	
	int line=0;
	char buf[512];
	char partition[32];
	char usbMntCmd[64];
	int ret=-1;
	if(isFileExist(PARTITION_FILE)){
		fp= fopen(PARTITION_FILE, "r");
		if (!fp) {
	        	printf("can not  open /proc/partitions\n");
			return; 
	   	}

		while (fgets(buf, sizeof(buf), fp)) 
		{
			ret=get_blockDevPartition(buf, &partition);
			if(ret==0)
			{
				if(Check_shouldMount(partition)==0){
				sprintf(usbMntCmd, "DEVPATH=/sys/block/sda/%s ACTION=add usbmount block", partition);
				RunSystemCmd(NULL_FILE,  "echo", usbMntCmd, NULL_STR);
				system(usbMntCmd);
			}
			}
			
		}
	
		fclose(fp);
	}
	

}

void start_mount()
{
#if defined(HTTP_FILE_SERVER_SUPPORTED) || defined(RTL_USB_IP_HOST_SPEEDUP)
	RunSystemCmd("/proc/sys/vm/min_free_kbytes", "echo", "2048", NULL_STR);
	RunSystemCmd("/proc/sys/net/core/rmem_max", "echo", "1048576", NULL_STR);
	RunSystemCmd("/proc/sys/net/core/wmem_max", "echo", "1048576", NULL_STR);
	RunSystemCmd("/proc/sys/net/ipv4/tcp_rmem", "echo", "4096 108544 4194304", NULL_STR);
	RunSystemCmd("/proc/sys/net/ipv4/tcp_wmem", "echo", "4096 108544 4194304", NULL_STR);
	RunSystemCmd("/proc/sys/net/ipv4/tcp_moderate_rcvbuf", "echo", "0", NULL_STR);
#else	
	/*config linux parameter for improving samba performance*/
	RunSystemCmd("/proc/sys/vm/min_free_kbytes", "echo", "1024", NULL_STR);
	
	RunSystemCmd("/proc/sys/net/core/netdev_max_backlog", "echo", "8192", NULL_STR);
	RunSystemCmd("/proc/sys/net/core/optmem_max", "echo", "131072", NULL_STR);
	RunSystemCmd("/proc/sys/net/core/rmem_default", "echo", "524288", NULL_STR);
	RunSystemCmd("/proc/sys/net/core/rmem_max", "echo", "524288", NULL_STR);
	RunSystemCmd("/proc/sys/net/core/wmem_default", "echo", "524288", NULL_STR);
	RunSystemCmd("/proc/sys/net/core/wmem_max", "echo", "524288", NULL_STR);
	RunSystemCmd("/proc/sys/net/ipv4/tcp_rmem", "echo", "131072 262144 393216", NULL_STR);
	RunSystemCmd("/proc/sys/net/ipv4/tcp_wmem", "echo", "131072 262144 393216", NULL_STR);
	RunSystemCmd("/proc/sys/net/ipv4/tcp_mem", "echo", "768 1024 1380", NULL_STR);
#endif
	/*config hot plug and auto-mount*/
	RunSystemCmd("/proc/sys/kernel/hotplug", "echo", "/usr/hotplug", NULL_STR);
	RunSystemCmd(NULL_FILE, "mkdir", "-p /tmp/usb/", NULL_STR);

	/*force kernel to write data to disk, don't cache in memory for a long time*/
	RunSystemCmd("/proc/sys/vm/vfs_cache_pressure", "echo", "10000", NULL_STR);
	RunSystemCmd("/proc/sys/vm/dirty_background_ratio", "echo", "5", NULL_STR);
	RunSystemCmd("/proc/sys/vm/dirty_writeback_centisecs", "echo", "100", NULL_STR);
	/*automatically mount partions listed in /proc/partitions*/
	autoMountOnBootUp();

	system("killall usbStorageAppController 2>/dev/null");
	system("usbStorageAppController&");
//	RunSystemCmd("/var/group",  "echo", " ",  NULL_STR);
//	RunSystemCmd(NULL_FILE,  "cp", "/etc/group", "/var/group",  NULL_STR);

}
#endif

#if defined(CONFIG_RTL_HIGH_PERFORMANCE_FILESYSTEM)
void load_rtl_fs_module(void)
{
	RunSystemCmd(NULL_FILE, "insmod", "/lib/modules/jnl.ko", NULL_STR);
	RunSystemCmd(NULL_FILE, "insmod", "/lib/modules/ufsd.ko", NULL_STR);
}
#endif

#if defined(CONFIG_APP_SAMBA)
void start_samba()
{
	/*start samba*/
	//RunSystemCmd(NULL_FILE,  "echo", "start samba", NULL_STR);
	RunSystemCmd(NULL_FILE,  "mkdir", "/var/samba", NULL_STR);
	RunSystemCmd(NULL_FILE,  "cp", "/etc/samba/smb.conf", "/var/samba/smb.conf",  NULL_STR);
	RunSystemCmd("/var/group",  "echo", " ",  NULL_STR);
        RunSystemCmd(NULL_FILE,  "cp", "/etc/group", "/var/group",  NULL_STR);
	//RunSystemCmd(NULL_FILE,  "smbd", "-D", NULL_STR);
	//RunSystemCmd(NULL_FILE,  "nmbd", "-D", NULL_STR);
}
#endif

#if defined(CONFIG_APP_VSFTPD)
void start_vsftpd()
{
	RunSystemCmd(NULL_FILE, "echo","start vsftpd", NULL_STR);
	RunSystemCmd(NULL_FILE,  "cp", "/etc/vsftpd.conf", "/var/config/vsftpd.conf",  NULL_STR);
	system("vsftpd /var/config/vsftpd.conf &");
}
#endif
#ifdef MULTI_PPPOE
void setInitMultiPPPoE()
{
	/*
		-----------------------Multi PPPoE--------------------------
		when something would lock global resource, should clear here,		
	*/
	
	system("rm /etc/ppp/firewall_lock >/dev/null 2>&1");
	system("echo 1 > /etc/ppp/firewall_lock ");
//	RunSystemCmd("tmp/firewall_lock", "echo", "1", NULL_STR);	
	//remove("/etc/ppp/hasPppoedevice");
	//remove("/etc/ppp/ppp_order_info");
	/*
	if((flushF = fopen("/etc/ppp/flushCmds","r+")) != NULL)
	{
		while(!feof(flushF))
		{
			// get the line and execute it , just for clear the ip policy rules
			fgets(cmd,sizeof(cmd),flushF);		
			system(cmd);
		}
		// delete flushCmds file
		unlink("/etc/ppp/flushCmds");
	}		
	*/
}

#endif
#if defined(CONFIG_RTL_ETH_802DOT1X_SUPPORT) 
int init_EthDot1x(int wan_mode, int sys_op, char *wan_iface, char *lan_iface)
{
	int val, dot1xenable, dot1xmode, proxy_port_mask = 0 , client_port_mask = 0, maxport = 0, i = 0;
	int type, unicast_enable, server_port;
	ETHDOT1X_T entry;
	unsigned char cmdBuffer[100];
	
	RunSystemCmd(NULL_FILE, "killall", "-9", "auth_eth", NULL_STR);	
	
	apmib_get( MIB_ELAN_ENABLE_1X, (void *)&dot1xenable);
	apmib_get( MIB_ELAN_DOT1X_MODE, (void *)&dot1xmode);
	apmib_get( MIB_ELAN_DOT1X_PROXY_TYPE, (void *)&type);
	apmib_get( MIB_ELAN_EAPOL_UNICAST_ENABLED, (void *)&unicast_enable);
	apmib_get( MIB_ELAN_DOT1X_SERVER_PORT, (void *)&server_port);
	if (dot1xenable)
	{
		//client mode enable check in flash.c
		//if (dot1xenable & ETH_DOT1X_PROXY_SNOOPING_MODE_ENABLE_BIT)
		{
			maxport =  MAX_ELAN_DOT1X_PORTNUM - 1;
			if (sys_op ==BRIDGE_MODE || sys_op == WISP_MODE)
			{
				#if !defined(CONFIG_RTL_IVL_SUPPORT)
				maxport =  MAX_ELAN_DOT1X_PORTNUM;
				#endif
			}
			if ((dot1xenable & ETH_DOT1X_PROXY_SNOOPING_MODE_ENABLE_BIT)&&
				(dot1xmode & ETH_DOT1X_SNOOPING_MODE_BIT))//snooping mode enable
			{
				sprintf(cmdBuffer,"echo \"1 1\" > /proc/802dot1x/enable");
				system(cmdBuffer);
				sprintf(cmdBuffer,"echo \"%d\" > /proc/802dot1x/server_port", server_port);
				system(cmdBuffer);
			}
			else if ((dot1xenable & ETH_DOT1X_PROXY_SNOOPING_MODE_ENABLE_BIT)&&
				(dot1xmode & ETH_DOT1X_PROXY_MODE_BIT))//proxy mode enable
			{
				
				sprintf(cmdBuffer,"echo \"1 %d\" > /proc/802dot1x/enable", unicast_enable);
				system(cmdBuffer);
			}			
			else if ((dot1xenable & ETH_DOT1X_CLIENT_MODE_ENABLE_BIT)&&
				(dot1xmode & ETH_DOT1X_CLIENT_MODE_BIT))//client mode enable
			{
				sprintf(cmdBuffer,"echo \"1 1\" > /proc/802dot1x/enable");
				system(cmdBuffer);
			}
			
			sprintf(cmdBuffer,"echo \"%d\" > /proc/802dot1x/type", type);
			system(cmdBuffer);
			
			for(i=1; i<=maxport ; i++)
			{
				memset(&entry, '\0', sizeof(entry));
				//	printf("--%s(%d)--i is %d\n", __FUNCTION__, __LINE__, i);

				*((char *)&entry) = (char)i;
				apmib_get(MIB_ELAN_DOT1X_TBL, (void *)&entry);
				if (!entry.enabled)
					continue;
				
				printf("%s %d entry.enabled=%d entry.portnum=%d\n", __FUNCTION__, __LINE__, entry.enabled, entry.portnum);
				proxy_port_mask |= (1<<entry.portnum);
				if (dot1xmode & ETH_DOT1X_SNOOPING_MODE_BIT)
				{
					sprintf(cmdBuffer,"echo \"1 %d 1 1\" > /proc/802dot1x/mode", entry.portnum);
				}
				else if (dot1xmode & ETH_DOT1X_PROXY_MODE_BIT)
				{
					if (unicast_enable)
						sprintf(cmdBuffer,"echo \"1 %d 2 1\" > /proc/802dot1x/mode", entry.portnum);
					else
						sprintf(cmdBuffer,"echo \"1 %d 2 0\" > /proc/802dot1x/mode", entry.portnum);
				}
				system(cmdBuffer);
				
			}
			if (((dot1xenable & ETH_DOT1X_PROXY_SNOOPING_MODE_ENABLE_BIT)&&
				(dot1xmode & ETH_DOT1X_PROXY_MODE_BIT))||
				((dot1xenable & ETH_DOT1X_CLIENT_MODE_ENABLE_BIT)&&
				(dot1xmode & ETH_DOT1X_CLIENT_MODE_BIT)))
				{
					val = 1;
					apmib_set( MIB_ELAN_MAC_AUTH_ENABLED, (void *)&val);
					val = 3;
					apmib_set( MIB_ELAN_ACCOUNT_RS_MAXRETRY, (void *)&val);
					val = 0;
					apmib_set( MIB_ELAN_RS_REAUTH_TO, (void *)&val);
					val = 3;
					apmib_set( MIB_ELAN_RS_MAXRETRY, (void *)&val);
					val = 3;
					apmib_set( MIB_ELAN_RS_INTERVAL_TIME, (void *)&val);
					val = 3;
					apmib_set( MIB_ELAN_ACCOUNT_RS_MAXRETRY, (void *)&val);

					if ((dot1xenable & ETH_DOT1X_PROXY_SNOOPING_MODE_ENABLE_BIT)&&
						(dot1xmode & ETH_DOT1X_PROXY_MODE_BIT))
					{
						apmib_set( MIB_ELAN_DOT1X_PROXY_MODE_PORT_MASK, (void *)&proxy_port_mask);
					}
					
					if ((dot1xenable & ETH_DOT1X_CLIENT_MODE_ENABLE_BIT)&&
						(dot1xmode & ETH_DOT1X_CLIENT_MODE_BIT))
					{
						client_port_mask = 1<<ETH_DOT1X_CLIENT_PORT;//default .....
						apmib_set( MIB_ELAN_DOT1X_CLIENT_MODE_PORT_MASK, (void *)&client_port_mask);
						sprintf(cmdBuffer,"echo \"1 %d 3 0\" > /proc/802dot1x/mode", ETH_DOT1X_CLIENT_PORT);
						system(cmdBuffer);
						sprintf(cmdBuffer,"rsCert -rd");
						system(cmdBuffer);
					}
					sprintf(cmdBuffer,"flash ethdot1x /var/1x/eth_1x.conf");
					system(cmdBuffer);
					sprintf(cmdBuffer,"auth_eth eth0 br0 eth /var/1x/eth_1x.conf");
					system(cmdBuffer);
				}
			
		}
		
	}

	return 0;
	
}
#endif

#ifdef RTK_CAPWAP
#define CAPWAP_APP_VAR_DIR "/var/capwap"
#define CAPWAP_APP_ETC_DIR "/etc/capwap"
static int capwap_config_changed(const char *var_str, const char *var_original_filename)
{
	char *old_var_str = NULL;
	FILE *fp = NULL;
	size_t bufsize, bufread;
	char filepath[64];

	if(!isFileExist(CAPWAP_APP_VAR_DIR)) return 1;
	
	sprintf(filepath, "%s/%s", CAPWAP_APP_VAR_DIR, var_original_filename);
	fp = fopen(filepath, "r");
	if (fp == NULL) return 1;
	if (fseek(fp, 0L, SEEK_END) != 0) {
		fclose(fp);
		return 1;
	}
	bufsize = ftell(fp);
	if (bufsize <= 0) {
		fclose(fp);
		return 1;
	}
	if (fseek(fp, 0L, SEEK_SET) != 0) {
		fclose(fp);
		return 1;
	}

	old_var_str = malloc(sizeof(char) * (bufsize + 1));
	old_var_str[0] = '\0';

	bufread = fread(old_var_str, sizeof(char), bufsize, fp);
	if (bufread < bufsize) {
		printf("bufread < bufsize, %u, %u, %s\n", bufread, bufsize, old_var_str);
	}
	old_var_str[bufread] = '\0';
	fclose(fp);

	if(strcmp(old_var_str, var_str)!=0) {
		free(old_var_str);
		return 1;
	} else {
		free(old_var_str);
		return 0;
	}

}

static void capwap_set_config_to_file(const char *var_str, const char *var_original_filename)
{
	char cmd[128];
	
	if(!isFileExist(CAPWAP_APP_VAR_DIR)) {		
		system("mkdir "CAPWAP_APP_VAR_DIR);
	}
		
	sprintf(cmd, "echo \"%s\" > %s/%s", var_str, CAPWAP_APP_VAR_DIR, var_original_filename);
	system(cmd);	
}

void capwap_app()
{	
	int capwapMode;
	apmib_get(MIB_CAPWAP_MODE, &capwapMode);

	// for wtp
	printf("babylon test MIB_CAPWAP_MODE=%d, CAPWAP_WTP_ENABLE=%d, CAPWAP_AC_ENABLE=%d\n", capwapMode, CAPWAP_WTP_ENABLE, CAPWAP_AC_ENABLE);
	if (capwapMode & CAPWAP_WTP_ENABLE) {
		char ac_ip_str[16], wtp_id_str[8];		
		int restart_flag;
		unsigned char tmp_ip[4];
		int tmp_int_val;
		
		// get config from flash
		apmib_get(MIB_CAPWAP_AC_IP, tmp_ip);
		sprintf(ac_ip_str, "%d.%d.%d.%d", tmp_ip[0], tmp_ip[1], tmp_ip[2], tmp_ip[3]);
		apmib_get(MIB_CAPWAP_WTP_ID, &tmp_int_val);
		sprintf(wtp_id_str, "%d.%d.%d.%d", tmp_ip[0], tmp_ip[1], tmp_ip[2], tmp_ip[3]);

		// get original config & compare
		restart_flag = 0;
		if (capwap_config_changed("wtp_started", "wtp_status")) {			
			restart_flag = 1;
		}
		
		if (capwap_config_changed(ac_ip_str, "ac_ip")) {			
			restart_flag = 1;
		}

		if (capwap_config_changed(wtp_id_str, "wtp_id")) {			
			restart_flag = 1;
		}

		// restart wtp
		if (restart_flag) {
			system("killall -9 WTP");
			capwap_set_config_to_file("wtp_started", "wtp_status");
			capwap_set_config_to_file(ac_ip_str, "ac_ip");
			capwap_set_config_to_file(wtp_id_str, "wtp_id");
			sleep(2);
			system("WTP "CAPWAP_APP_ETC_DIR);
		}
		
	} else {
		system("killall -9 WTP");
		capwap_set_config_to_file("wtp_disabled", "wtp_status");
	}

	// for AC
	if (capwapMode & CAPWAP_AC_ENABLE) {		
		int restart_flag = 0;		
		if (capwap_config_changed("ac_started", "ac_status")) {			
			restart_flag = 1;
		}
		if (restart_flag) {
			system("killall -9 AC");			
			capwap_set_config_to_file("ac_started", "ac_status");
			sleep(2);
			system("AC "CAPWAP_APP_ETC_DIR);
		}
	} else {
		system("killall -9 AC");
		capwap_set_config_to_file("ac_disabled", "ac_status");
	}
	
}
#endif

#if defined(CONFIG_RPS)
static void rtl_configRps(void)
{
	system("mount -t sysfs sysfs /sys");
	system("echo 2 > /sys/class/net/eth0/queues/rx-0/rps_cpus");
	system("echo 2 > /sys/class/net/eth1/queues/rx-0/rps_cpus");
	system("echo 2 > /sys/class/net/eth2/queues/rx-0/rps_cpus");
	system("echo 2 > /sys/class/net/eth3/queues/rx-0/rps_cpus");
	system("echo 2 > /sys/class/net/eth4/queues/rx-0/rps_cpus");
	system("echo 2 > /sys/class/net/wlan0/queues/rx-0/rps_cpus");
	system("echo 2 > /sys/class/net/wlan1/queues/rx-0/rps_cpus");
	system("echo 4096 > /sys/class/net/eth0/queues/rx-0/rps_flow_cnt");
	system("echo 4096 > /sys/class/net/eth1/queues/rx-0/rps_flow_cnt");
	system("echo 4096 > /sys/class/net/eth2/queues/rx-0/rps_flow_cnt");
	system("echo 4096 > /sys/class/net/eth3/queues/rx-0/rps_flow_cnt");
	system("echo 4096 > /sys/class/net/eth4/queues/rx-0/rps_flow_cnt");
	system("echo 4096 > /proc/sys/net/core/rps_sock_flow_entries");
}
#endif

int setinit(int argc, char** argv)
{
	int i, cmdRet=-1;
	int opmode=-1, v_wlan_app_enabled=0, intValue=0, intValue1=0;
	char cmdBuffer[100], tmpBuff[512];
	int repeater_enabled1=0, repeater_enabled2=0;
	char *token=NULL, *savestr1=NULL;
	char tmp_args[16];
	int wisp_wan_id=0;
	int lan_dhcp_mode=0;
	int wan_dhcp_mode=0;
	char Ip[32], Mask[32], Gateway[32];
	int wlan_mode_root=0, wlan_root_disabled=0;
	int br_wlan_block=0;
	char strPID[32];
	int pid = 0;
	int wlan_support = 0;
	int index; 
	int old_wlan_idx;
	int repeater_enable1=0;
	int repeater_enable2=0;
#if defined(CONFIG_RTL_92D_SUPPORT)
	int wlan_mode_root1=0, wlan_root1_disabled=0;
#endif
#if defined(CONFIG_RTL_92D_SUPPORT)
	int wispWanId=0;
#endif
	int reinit=1;

#ifdef SAMBA_WEB_SUPPORT
	STORAGE_USER_T 	user_info;
	STORAGE_GROUP_T	group_info;
	int 			number;
#endif
#if defined(CONFIG_RTL_ULINKER_WLAN_DELAY_INIT)
	int ulinker_auto = 0;
#endif

//for 2.4g-wlan0 5g-wlan1, not normal 5g-wlan0 2.4g-wlan1
#if defined(CONFIG_BAND_2G_ON_WLAN0) || defined(CONFIG_BAND_5G_ON_WLAN0)
	int wlanBand2G5GSelect;
	int phyBandSelect;
	apmib_get(MIB_WLAN_BAND2G5G_SELECT,(void *)&wlanBand2G5GSelect);
	if(wlanBand2G5GSelect == BANDMODEBOTH)//dual band
	{
		old_wlan_idx = wlan_idx;
		wlan_idx = 0;
		apmib_get(MIB_WLAN_PHY_BAND_SELECT, (void *)&phyBandSelect);	
#if defined(CONFIG_BAND_2G_ON_WLAN0)
		if(phyBandSelect != PHYBAND_2G)
			swapWlanMibSetting(0,1);
#else
		if(phyBandSelect != PHYBAND_5G)
			swapWlanMibSetting(0,1);
#endif
		wlan_idx = old_wlan_idx;
	}
#endif

	if(strcmp(argv[2], "ap")||strcmp(argv[3], "wlan_app")){
		//it will take about 7s for LAN PC to send DHCP request packet, down eth0 early
		RunSystemCmd(NULL_FILE, "ifconfig", "eth0", "down", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "br0", "down", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "br0", "up", NULL_STR);
	}
	
#ifdef MULTI_PPPOE
	//now modify. if need , add parameter
	setInitMultiPPPoE();
#endif
	if(isFileExist(SET_TIME)==0){
		RunSystemCmd(NULL_FILE, "flash", "settime", NULL_STR);
	}

#ifdef CONFIG_CPU_UTILIZATION
	int enable_cpu=0;
	apmib_get(MIB_ENABLE_CPU_UTILIZATION, (void *)&enable_cpu);
	if(enable_cpu)
	{
		if(isFileExist(CPU_LOAD_FILE)==0)
		{
			RunSystemCmd(NULL_FILE, "cpu", NULL_STR);
		}
	}
	else
	{
		if(isFileExist(CPU_LOAD_FILE)!=0)
		{
			RunSystemCmd(NULL_FILE, "killall", "cpu", NULL_STR);
//			RunSystemCmd(NULL_FILE, "rm", "-f", "/web/*.cpudat", NULL_STR);
			system("rm -f  /web/*.cpudat 2> /dev/null");
		}
	}
#endif

	if(isFileExist(REINIT_FILE)==0){
		up_mib_value();
		RunSystemCmd(REINIT_FILE, "echo", "1", NULL_STR);
		reinit = 0;
	}
	else
		reinit = 1;

#if defined(CONFIG_RTL_819XD)
	if(reinit == 1)
	{
		//The powerful CPU won't take 7s from interface down to up, so sleep 1s to force LAN/WLAN PC to renew IP.
		//The timeout is 25s in WEB UI, it can redirect to new IP after count down timeout, so sleep 1s is not side effect.
		sleep(1);
	}	
#endif

#ifdef CONFIG_RTL_802_1X_CLIENT_SUPPORT
	if((isFileExist(RS_USER_CERT_5G)==0) && (isFileExist(RS_ROOT_CERT_5G)==0) &&  (isFileExist(RS_USER_CERT_2G)==0) && (isFileExist(RS_ROOT_CERT_2G)==0)){
		RunSystemCmd(NULL_FILE, "rsCert","-rd", NULL_STR);
	}
#endif

#ifdef CONFIG_RTL_WAPI_SUPPORT
#ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
	if(isFileExist(CA_CERT_FILE)==0){
		RunSystemCmd(NULL_FILE, "loadWapiFiles", NULL_STR);
	}
#else
	if((isFileExist(CA4AP_CERT)==0 && isFileExist(AP_CERT)==0 )
		&& (isFileExist(CA4AP_CERT_AS0)==0 && isFileExist(AP_CERT_AS0)==0 )
		&& (isFileExist(CA4AP_CERT_AS1)==0 && isFileExist(AP_CERT_AS1)==0 )){
		RunSystemCmd(NULL_FILE, "loadWapiFiles", NULL_STR);
	}
#endif
#endif	
	#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT) || defined(CONFIG_APP_APPLE_MFI_WAC)
		system("echo 1 > /var/system/start_init");
	#endif 	
	
#if defined(CONFIG_DYNAMIC_WAN_IP)
	//Added for pptp/l2tp use dynamic wan ip
	if(isFileExist(TEMP_WAN_CHECK))
		unlink(TEMP_WAN_CHECK);
	if(isFileExist(TEMP_WAN_DHCP_INFO))
		unlink(TEMP_WAN_DHCP_INFO);
	if(isFileExist(MANUAL_CONNECT_NOW))
		unlink(MANUAL_CONNECT_NOW);
#endif

#ifdef TR181_SUPPORT
	intValue = 1;
	if(init_dns_client_server_table == 0)
	{
		for(i=0; i<DNS_CLIENT_SERVER_NUM; i++)
		{
			sprintf(tmpBuff, "Device.DNS.Client.Server.{%d}.Enable", i);
			if(tr181_ipv6_set(tmpBuff ,(void *) &intValue) == -1)
			{
				printf("%s(%d): set %s fail!\n",__FUNCTION__,__LINE__, tmpBuff);
				return -1;
			}
		}
	}
	init_dns_client_server_table = 1;
#endif

	printf("Init Start...\n");

/*#if defined(CONFIG_APP_APPLE_MFI_WAC)
	{
        char wac_nameBuf[64]={0};
        char hostname_cmd[80]={0};

	apmib_get(MIB_MFI_WAC_DEVICE_NAME,  (void *)wac_nameBuf);
	if(wac_nameBuf[0]){
		sprintf(hostname_cmd,"hostname \'%s\'",wac_nameBuf);
		system(hostname_cmd);
	}
	}		
#endif*/
	apmib_get(MIB_OP_MODE,(void *)&opmode);
	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_wan_id);
	apmib_get(MIB_DHCP,(void *)&lan_dhcp_mode);
#ifdef   HOME_GATEWAY	
	apmib_get(MIB_WAN_DHCP,(void *)&wan_dhcp_mode);
#endif	
	memset(br_lan2_interface, 0x00, sizeof(br_lan2_interface));
	memset(vlan_interface, 0x00, sizeof(vlan_interface));
	if(opmode==GATEWAY_MODE)
		RunSystemCmd("/var/sys_op", "echo", "0", NULL_STR);
	else if(opmode==BRIDGE_MODE)
		RunSystemCmd("/var/sys_op", "echo", "1", NULL_STR);
	else if(opmode==WISP_MODE)
		RunSystemCmd("/var/sys_op", "echo", "2", NULL_STR);
		
/* set interface name  start*/		
	sprintf(tmp_args,"%s", argv[2]);  
	if(strcmp(tmp_args, "ap") == 0){
		sprintf(br_interface, "%s", "br0");
		sprintf(br_lan1_interface, "%s" , "eth0");
#if !defined(CONFIG_RTL_AP_PACKAGE) 
		if(opmode==BRIDGE_MODE)
			sprintf(br_lan2_interface, "%s", "eth1");
#endif
		gateway=0;
	}	
	if(!strcmp(tmp_args, "gw")){		
		gateway=1;
		if(opmode==WISP_MODE)
		{
			sprintf(wan_interface, "wlan%d", wisp_wan_id);
#if defined(CONFIG_SMART_REPEATER)	
			getWispRptIfaceName(wan_interface,wisp_wan_id);

			//strcat(wan_interface,"-vxd");
#endif			
		}
		else
		{
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
			if(opmode == GATEWAY_MODE)
				sprintf(wan_interface, "%s", "eth1");
			
#else
			sprintf(wan_interface, "%s", "eth1");
#endif
		}
			
//printf("\r\n wan_interface=[%s],__[%s-%u]\r\n",wan_interface,__FILE__,__LINE__);
			
		sprintf(br_interface, "%s", "br0");
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
		//if(opmode != 0)
#ifdef RTK_USB3G
 		if((wan_dhcp_mode == USB3G) || (opmode != 0))
			sprintf(br_lan1_interface, "%s" , "eth0");
		else
#endif
		if(opmode != GATEWAY_MODE)
			sprintf(br_lan1_interface, "%s" , "eth0");
			
#else
		sprintf(br_lan1_interface, "%s" , "eth0");
#endif

		if(opmode ==BRIDGE_MODE || opmode == WISP_MODE) {
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
#else
			sprintf(br_lan2_interface, "%s", "eth1");
#endif
		}
		/**/
		//RunSystemCmd("/proc/wan_port", "echo", "4", NULL_STR);	
	}
	/*
	if(isFileExist(ETH_VLAN_SWITCH)){//vlan init file is exist
		sprintf(vlan_interface, "%s %s %s", "eth2", "eth3", "eth4");
	}
	*/
	
	RunSystemCmd(PROC_BR_IGMPPROXY, "echo", "0", NULL_STR);
	
#if defined(VLAN_CONFIG_SUPPORTED)
	apmib_get(MIB_VLANCONFIG_ENABLED, (void *)&intValue);
	if(intValue !=0) {
#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
#elif defined(CONFIG_RTK_VLAN_NEW_FEATURE)
		sprintf(vlan_interface, "%s %s %s %s", "eth2", "eth3", "eth4", "eth7");
#else
		sprintf(vlan_interface, "%s %s %s", "eth2", "eth3", "eth4");
#endif
	}
	else		
		memset(vlan_interface, 0x00, sizeof(vlan_interface));

#endif //#if defined(VLAN_CONFIG_SUPPORTED)

	memset(wlan_interface, 0x00, sizeof(wlan_interface));
	memset(wlan_virtual_interface, 0x00, sizeof(wlan_virtual_interface));
	memset(wlan_vxd_interface, 0x00, sizeof(wlan_vxd_interface));
	memset(wlan_valid_interface, 0x00, sizeof(wlan_valid_interface));
	
	for(i=0;i<NUM_WLAN_INTERFACE;i++){
		if(wlan_interface[0]==0x00)
			sprintf(wlan_interface, "wlan%d", i);
		else{
			sprintf(tmp_args, " wlan%d", i);
			strcat(wlan_interface, tmp_args); 
		}
	}
	/* collect all wlan interface for clean_process() */
	num_wlan_interface=NUM_WLAN_INTERFACE;
	num_wlan_virtual_interface=if_readlist_proc(wlan_virtual_interface, "va", 0);
	num_wlan_vxd_interface=if_readlist_proc(wlan_vxd_interface, "vxd", 0);
#if defined(CONFIG_RTK_MESH)
	num_wlan_mesh_interface=if_readlist_proc(wlan_mesh_interface, "msh", 0);
#endif
	wlan_support = if_readlist_proc(wlan_valid_interface, "wlan", 0);
	if(wlan_support==0)
		memset(wlan_interface, 0x00, sizeof(wlan_interface));
		
//printf("\r\n wlan_vxd_interface=[%s],__[%s-%u]\r\n",wlan_vxd_interface,__FILE__,__LINE__);
		
/* set interface name  end*/			
#if defined(CONFIG_POCKET_AP_SUPPORT) && defined(HOME_GATEWAY)
	sprintf(tmp_args,"%s", argv[3]);
	if(strcmp(tmp_args, "wlan_app") != 0)
	{
		clean_process(opmode,wan_dhcp_mode,gateway, enable_wan, br_interface, wlan_interface, wan_interface);
	
		/*init wlan interface*/
		if (wlan_support != 0)
		{
			memset(wlan_interface, 0x00, sizeof(wlan_interface));
			for(i=0;i<NUM_WLAN_INTERFACE;i++)
			{
				int wlan_disable = 1;			
				unsigned char wlan_name[10];
				memset(wlan_name,0x00,sizeof(wlan_name));
				sprintf(wlan_name, "wlan%d",i);
				if(SetWlan_idx(wlan_name))
				{			
					apmib_get( MIB_WLAN_WLAN_DISABLED, (void *)&wlan_disable);	  

				
					if(wlan_disable == 1)
					{
						RunSystemCmd(NULL_FILE, "iwpriv", wlan_name, "radio_off", NULL_STR);					
					}
					else
					{
						if(wlan_interface[0]==0x00)
							sprintf(wlan_interface, "%s", wlan_name);
						else
						{
							sprintf(tmp_args, " %s", wlan_name);
							strcat(wlan_interface, tmp_args); 
						}							
					}
				}
			}				
		}
	}
#endif	//#if defined(CONFIG_POCKET_AP_SUPPORT) && defined(HOME_GATEWAY)
/*currently, we just support init gw/ap all */	
	sprintf(tmp_args,"%s", argv[3]);  
	
	if(!strcmp(tmp_args, "all")){
		enable_wan=1;
		enable_br=1;
	}else if(!strcmp(tmp_args, "wan")){
		enable_wan=1;
		enable_br=1;
	}else if(!strcmp(tmp_args, "bridge")){
		enable_wan=1;
		enable_br=1;
	}else if(!strcmp(tmp_args, "wlan_app")){
		start_wlanapp(0);
		return 0;
	}

	/*save the last wan type*/ /*no this operate in shell script*/


#if defined(CONFIG_POCKET_ROUTER_SUPPORT)
	clean_process(opmode,wan_dhcp_mode,gateway, enable_wan, br_interface, wlan_interface, wan_interface);
	RunSystemCmd(NULL_FILE, "ifconfig", "eth1", "down", NULL_STR);
	RunSystemCmd(NULL_FILE, "ifconfig", "peth0", "down", NULL_STR);
#if defined(CONFIG_SMART_REPEATER)
	getWispRptIfaceName(wan_interface,wisp_wan_id);
	if(strstr(wan_interface,"-vxd"))
			RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "0.0.0.0", NULL_STR);
	RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "down", NULL_STR);
#endif
#else
	if(reinit == 1)
	{
		clean_process(opmode,wan_dhcp_mode,gateway, enable_wan, br_interface, wlan_interface, wan_interface);
		
#if !defined(CONFIG_RTL_AP_PACKAGE) 
		//MUST down ppp related interface before eth1 down, otherwise it will cause some puzzling problems.
		RunSystemCmd(NULL_FILE, "ifconfig", "eth1", "down", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "peth0", "down", NULL_STR);
#if defined(CONFIG_SMART_REPEATER)
		if(strstr(wan_interface,"-vxd"))
			RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "0.0.0.0", NULL_STR);
#endif
		if(wan_interface[0])
			RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "down", NULL_STR);
#endif
	}
	else
	{
#ifdef   HOME_GATEWAY
#ifdef CONFIG_POCKET_AP_SUPPORT
#else
	if(isFileExist(HW_NAT_FILE)){/*hw nat supported*/
		/*cleanup hardware tables*/
		if(opmode==GATEWAY_MODE)
			RunSystemCmd(HW_NAT_FILE, "echo", "1", NULL_STR);	/*gateway mode*/
		else if(opmode==BRIDGE_MODE)
			RunSystemCmd(HW_NAT_FILE, "echo", "2", NULL_STR);	/*bridge mode*/
		else if(opmode==WISP_MODE)
			RunSystemCmd(HW_NAT_FILE, "echo", "3", NULL_STR);	/*wisp mode*/
		else if(opmode==3)
			RunSystemCmd(HW_NAT_FILE, "echo", "4", NULL_STR);	/*bridge mode with multiple vlan*/
		else
			RunSystemCmd(HW_NAT_FILE, "echo", "5", NULL_STR); /*wisp mode with multiple vlan*/
		
	}else{/*software nat supported*/ 
		if(isFileExist(SOFTWARE_NAT_FILE)){
		if(opmode==GATEWAY_MODE)
		{
#ifdef RTK_USB3G
			if(wan_dhcp_mode == USB3G)
      	RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "1", NULL_STR);
			else
#endif
				RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "0", NULL_STR);
		}
		else if(opmode==BRIDGE_MODE)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "1", NULL_STR);
		else if(opmode==WISP_MODE)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "2", NULL_STR);
		else if(opmode==3)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "3", NULL_STR);
		else if(opmode==4)
			RunSystemCmd(SOFTWARE_NAT_FILE, "echo", "4", NULL_STR);
	}
		
	}
#endif	//CONFIG_POCKET_AP_SUPPORT
#endif	
	}
#endif

	sprintf(tmp_args, "%d", wan_dhcp_mode);
	RunSystemCmd("/var/system/last_wan", "echo", tmp_args, NULL_STR);

	apmib_get(MIB_REPEATER_ENABLED1,(void *)&repeater_enabled1);
	apmib_get(MIB_REPEATER_ENABLED2,(void *)&repeater_enabled2);

#if defined(CONFIG_RTL_92D_SUPPORT)
	if(SetWlan_idx("wlan1")){
			apmib_get( MIB_WLAN_MODE, (void *)&wlan_mode_root1); 
			apmib_get( MIB_WLAN_WLAN_DISABLED, (void *)&wlan_root1_disabled);	  
	}
	if(v_wlan_app_enabled == 0 
		&& (wlan_mode_root1 == AP_MODE || wlan_mode_root1 == AP_WDS_MODE || repeater_enabled2 == 1)
	)
	{
		v_wlan_app_enabled=1; // have virtual or repeat
	}
	
#endif
	if(SetWlan_idx("wlan0")){
		apmib_get( MIB_WLAN_MODE, (void *)&wlan_mode_root); 
		apmib_get( MIB_WLAN_WLAN_DISABLED, (void *)&wlan_root_disabled);	  
	}

	if(v_wlan_app_enabled == 0 
		&& (wlan_mode_root == AP_MODE || wlan_mode_root == AP_WDS_MODE || repeater_enabled1 == 1)
	)
	{
		v_wlan_app_enabled=1; // have virtual or repeat
	}
	
	memset(wlan_vxd_interface, 0x00, sizeof(wlan_vxd_interface));
	if(repeater_enabled1 == 1 && !wlan_root_disabled){
#if defined(CONFIG_SMART_REPEATER)
		if(strcmp(wan_interface,"wlan0-vxd"))
#endif
		{
			if(strlen(wlan_vxd_interface) != 0)
				strcat(wlan_vxd_interface," ");	
			strcat(wlan_vxd_interface,"wlan0-vxd");
		}
	}
#if defined(CONFIG_RTL_92D_SUPPORT)
	if(repeater_enabled2 == 1 && !wlan_root1_disabled)
	{
#if defined(CONFIG_SMART_REPEATER)
        if(strcmp(wan_interface,"wlan1-vxd"))
#endif
		{

			if(strlen(wlan_vxd_interface) != 0)
				strcat(wlan_vxd_interface," ");	
			strcat(wlan_vxd_interface,"wlan1-vxd");
		}	
	}
#endif

//printf("\r\n wlan_vxd_interface=[%s],__[%s-%u]\r\n",wlan_vxd_interface,__FILE__,__LINE__);

	apmib_get(MIB_ELAN_MAC_ADDR,  (void *)tmpBuff);
	if(!memcmp(tmpBuff, "\x00\x00\x00\x00\x00\x00", 6))
		apmib_get(MIB_HW_NIC0_ADDR,  (void *)tmpBuff);
	sprintf(cmdBuffer, "%02x%02x%02x%02x%02x%02x", (unsigned char)tmpBuff[0], (unsigned char)tmpBuff[1], 
		(unsigned char)tmpBuff[2], (unsigned char)tmpBuff[3], (unsigned char)tmpBuff[4], (unsigned char)tmpBuff[5]);
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
	RunSystemCmd(NULL_FILE, "ifconfig", "eth0", "hw", "ether", cmdBuffer, NULL_STR);/*set eth0 mac address*/
	RunSystemCmd(NULL_FILE, "ifconfig", "eth2", "hw", "ether", cmdBuffer, NULL_STR);/*set eth0 mac address*/
	RunSystemCmd(NULL_FILE, "ifconfig", "eth3", "hw", "ether", cmdBuffer, NULL_STR);/*set eth0 mac address*/
	RunSystemCmd(NULL_FILE, "ifconfig", "eth4", "hw", "ether", cmdBuffer, NULL_STR);/*set eth0 mac address*/
#else
	if(br_lan1_interface[0])
		RunSystemCmd(NULL_FILE, "ifconfig", br_lan1_interface, "hw", "ether", cmdBuffer, NULL_STR);/*set eth0 mac address*/
#endif
	
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
	Init_Domain_Query_settings(opmode, wlan_mode_root, lan_dhcp_mode, cmdBuffer);
#endif
	if(opmode == BRIDGE_MODE || opmode == WISP_MODE){
		apmib_get(MIB_ELAN_MAC_ADDR,  (void *)tmpBuff); // clone lan mac
		if(!memcmp(tmpBuff, "\x00\x00\x00\x00\x00\x00", 6))
		{
#if defined(CONFIG_RTL_8198_AP_ROOT) || defined(CONFIG_RTL_8197D_AP)
			apmib_get(MIB_HW_NIC0_ADDR,  (void *)tmpBuff);
#else
			apmib_get(MIB_HW_NIC1_ADDR,  (void *)tmpBuff);
#endif			
		}
		sprintf(cmdBuffer, "%02x%02x%02x%02x%02x%02x", (unsigned char)tmpBuff[0], (unsigned char)tmpBuff[1], 
			(unsigned char)tmpBuff[2], (unsigned char)tmpBuff[3], (unsigned char)tmpBuff[4], (unsigned char)tmpBuff[5]);	
		if(br_lan2_interface[0])	
			RunSystemCmd(NULL_FILE, "ifconfig", br_lan2_interface, "hw", "ether", cmdBuffer, NULL_STR);/*set eth1 mac address when bridge mode*/
	} 
#ifdef   HOME_GATEWAY	
	if(gateway == 1 && opmode != BRIDGE_MODE){
		apmib_get(MIB_WAN_MAC_ADDR,  (void *)tmpBuff); // clone wan mac
		if(!memcmp(tmpBuff, "\x00\x00\x00\x00\x00\x00", 6)){
			if(opmode == WISP_MODE)
			{
				apmib_get(MIB_WISP_WAN_ID, (void *)&index);
				old_wlan_idx=wlan_idx;
				wlan_idx = index;
				apmib_get(MIB_HW_WLAN_ADDR,  (void *)tmpBuff);
				wlan_idx=old_wlan_idx;
			}
			else
				apmib_get(MIB_HW_NIC1_ADDR,  (void *)tmpBuff);
		}
		sprintf(cmdBuffer, "%02x%02x%02x%02x%02x%02x", (unsigned char)tmpBuff[0], (unsigned char)tmpBuff[1], 
			(unsigned char)tmpBuff[2], (unsigned char)tmpBuff[3], (unsigned char)tmpBuff[4], (unsigned char)tmpBuff[5]);
		if(wan_interface[0])		
			RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "hw", "ether", cmdBuffer, NULL_STR);/*set wan mac address when it not bridge mode*/
#if defined(CONFIG_SMART_REPEATER)
			if(opmode == WISP_MODE && index == 0)
				RunSystemCmd(NULL_FILE, "ifconfig", "wlan0", "hw", "ether", cmdBuffer, NULL_STR);/*set wan mac address when it not bridge mode*/
			else if(opmode == WISP_MODE && index == 1)
				RunSystemCmd(NULL_FILE, "ifconfig", "wlan1", "hw", "ether", cmdBuffer, NULL_STR);/*set wan mac address when it not bridge mode*/
#endif
	}
#endif	

	/*init wlan interface*/
#if defined(CONFIG_RTL_ULINKER_WLAN_DELAY_INIT)
	apmib_get(MIB_ULINKER_AUTO,  (void *)&ulinker_auto);
	if (wlan_support != 0 && ulinker_auto == 0)
#else
	if (wlan_support != 0)
#endif
	{
		memset(wlan_interface, 0x00, sizeof(wlan_interface));
		for(i=0;i<NUM_WLAN_INTERFACE;i++)
		{
			int wlan_disable = 1;
			int wlan_blockrelay=0;
			unsigned char wlan_name[10];
			int mc2u_disabled = 0;
			memset(wlan_name,0x00,sizeof(wlan_name));
			sprintf(wlan_name, "wlan%d",i);
			apmib_save_wlanIdx();
			if(SetWlan_idx(wlan_name))
			{			
				apmib_get( MIB_WLAN_WLAN_DISABLED, (void *)&wlan_disable);	  
				if(wlan_disable == 1)
				{
					RunSystemCmd(NULL_FILE, "ifconfig", wlan_name, "down", NULL_STR);
					RunSystemCmd(NULL_FILE, "iwpriv", wlan_name, "radio_off", NULL_STR);					
				}
				else
				{
	        if(wlan_interface[0]==0x00)
	            sprintf(wlan_interface, "%s", wlan_name);
	      	else
	        {
						sprintf(tmp_args, " %s", wlan_name);
		        strcat(wlan_interface, tmp_args); 
	        }	
						
					//printf("%s:%d wlan_name=%s\n",__FUNCTION__,__LINE__,wlan_name);
#if defined(CONFIG_SMART_REPEATER)
					char vxd_interface[16]={0};
					sprintf(vxd_interface,"%s-vxd",wlan_name);
					RunSystemCmd(NULL_FILE, "ifconfig", vxd_interface, "0.0.0.0", NULL_STR);
					RunSystemCmd(NULL_FILE, "ifconfig", vxd_interface, "down", NULL_STR);
					//printf("%s:%d vxd_interface=%s\n",__FUNCTION__,__LINE__,vxd_interface);
#endif
					RunSystemCmd(NULL_FILE, "ifconfig", wlan_name, "down", NULL_STR);
					cmdRet=RunSystemCmd(NULL_FILE, "flash", "set_mib", wlan_name, NULL_STR);
			
					if(cmdRet != 0)
					{
						printf("[%s %d]init %s failed!\n", __FUNCTION__, __LINE__ ,wlan_name);
						continue;
					}
					apmib_get(MIB_WLAN_MC2U_DISABLED, (void *)&mc2u_disabled); 
					if(mc2u_disabled)
					{
						//iwpriv wlan0 set_mib mc2u_disable=1
						RunSystemCmd(NULL_FILE, "iwpriv", wlan_name,"set_mib","mc2u_disable=1", NULL_STR);
					}
					else
					{
						//iwpriv wlan0 set_mib mc2u_disable=0
						RunSystemCmd(NULL_FILE, "iwpriv", wlan_name,"set_mib","mc2u_disable=0", NULL_STR);
					}
				
				}
				
				apmib_get( MIB_WLAN_BLOCK_RELAY,(void *)&wlan_blockrelay);
				/*if all wlan interface block then enable br_wlan_block*/
				if(wlan_blockrelay)
				{
					br_wlan_block++;
				}

			}
			apmib_recov_wlanIdx();
		}
		
		if(br_wlan_block == NUM_WLAN_INTERFACE)
		{
			RunSystemCmd("/proc/br_wlanblock", "echo","1",NULL_STR);
		}
		else
		{
			RunSystemCmd("/proc/br_wlanblock", "echo","0",NULL_STR);
		}
	}


	if(wlan_interface[0]){				
		if(wlan_vxd_interface[0]) {
			char		*strptr, *tokptr;
			memcpy(tmpBuff,wlan_vxd_interface,sizeof(wlan_vxd_interface));
			strptr=tmpBuff;
			token = strsep(&strptr," ");
			while(token!=NULL)
			{
				RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);
				RunSystemCmd(NULL_FILE, "flash", "set_mib", token, NULL_STR);/*set vxd wlan iface*/
				token = strsep(&strptr," ");
			}
		}
		if(wlan_virtual_interface[0]){
			token=NULL;
			savestr1=NULL;
			sprintf(tmpBuff, "%s", wlan_virtual_interface);
			token = strtok_r(tmpBuff," ", &savestr1);
			do{
				if (token == NULL){
					break;
				}else{
					RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);
					RunSystemCmd(NULL_FILE, "flash", "set_mib", token, NULL_STR);/*set virtual wlan iface*/
				}
				token = strtok_r(NULL, " ", &savestr1);
			}while(token !=NULL);
		}
#if defined(CONFIG_SMART_REPEATER)
		if(strstr(wan_interface,"-vxd")){
			RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "down", NULL_STR);
			RunSystemCmd(NULL_FILE, "flash", "set_mib", wan_interface, NULL_STR);
		}
#endif
	}	
	
	if(gateway==1){
		if(enable_br==1){
			/*init bridge interface*/
			//hyking:sure hw initialization first..
#if 0 //for enable VLAN reboot/hang issue sometimes
#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT)
			if(opmode == 0)
			{
				RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "up", NULL_STR);
			}
#else
			RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "up", NULL_STR);
#endif
#endif 

			
			set_br_interface(tmpBuff);
			
//printf("\r\n br_interface=[%s],__[%s-%u]\r\n",tmpBuff,__FILE__,__LINE__);
			
			setbridge(tmpBuff);

#if 0
			//Not used at present in order to find the root cause!!!
			//Patch: wlan pc can't visit AP using wapi-psk or wapi-cert when AP boots up
			if(wlan_interface[0]){
				token=NULL;
				savestr1=NULL;
				sprintf(tmpBuff, "%s", wlan_interface);
				token = strtok_r(tmpBuff," ", &savestr1);
				do{
					if (token == NULL){
						break;
					}
					else
					{
						if(strncmp(token,"wlan",4)==0)
						{
							RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);
							RunSystemCmd(NULL_FILE, "ifconfig", token, "up", NULL_STR);
						}
					}
					token = strtok_r(NULL, " ", &savestr1);
				}while(token !=NULL);
			}
#endif

			/* init log setting*/
			set_log();

			/* init lan dhcp setting*/
			if(lan_dhcp_mode==DHCP_LAN_NONE){		/*DHCP disabled*/
				apmib_get( MIB_IP_ADDR,  (void *)tmpBuff);
				sprintf(Ip, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
				apmib_get( MIB_SUBNET_MASK,  (void *)tmpBuff);
				sprintf(Mask, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
				apmib_get(MIB_DEFAULT_GATEWAY,  (void *)tmpBuff);
				
				if (!memcmp(tmpBuff, "\x0\x0\x0\x0", 4))
					memset(Gateway, 0x00, sizeof(Gateway));
				else
					sprintf(Gateway, "%s", inet_ntoa(*((struct in_addr *)tmpBuff)));
					
				RunSystemCmd(NULL_FILE, "ifconfig", br_interface, Ip, "netmask", Mask, NULL_STR);
				
				if(Gateway[0]){
					RunSystemCmd(NULL_FILE, "route", "del", "default", br_interface, NULL_STR);
					RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", Gateway, "dev", br_interface, NULL_STR);
				}	
				start_wlanapp(v_wlan_app_enabled);
#if defined(CONFIG_APP_SIMPLE_CONFIG)
				system("echo 1 > /var/sc_ip_status");
#endif
			}else
				if(lan_dhcp_mode==DHCP_LAN_SERVER //dhcp disabled or server mode or auto
#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT		
				|| lan_dhcp_mode==DHCP_AUTO
#endif
				)			
				{		/*DHCP server enabled*/
					intValue1=0;
					for(i=0;i<NUM_WLAN_INTERFACE;i++){
						sprintf(tmp_args, " wlan%d", i);
						if(SetWlan_idx(tmp_args)){
							apmib_get(MIB_WLAN_WDS_ENABLED, (void *)&intValue);
							//printf("wds enabled=%d\n",intValue);
							if(intValue!=0)
								intValue1=intValue1+5;
							else
								intValue1=intValue1+1;
						}	
					}
					sleep(intValue1);/*wait wlan wds init */		
					/*start dhcp server*/
					set_lan_dhcpd(br_interface, 2);
					start_wlanapp(v_wlan_app_enabled);
				}
		}/*for init bridge interface and wlan app*/

		RunSystemCmd(NULL_FILE, "iptables", "-F", NULL_STR);
		RunSystemCmd(NULL_FILE, "iptables", "-F", "-t", "nat",  NULL_STR);
		RunSystemCmd(NULL_FILE, "iptables", "-A", "INPUT", "-j", "ACCEPT", NULL_STR);
		
#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT		
		/* start dnrd for check dns query with hostname */
		domain_query_start_dnrd(wlan_mode_root, 1);
#endif		

		
		RunSystemCmd(NULL_FILE, "rm", "-f", "/var/eth1_ip", NULL_STR);
		RunSystemCmd(NULL_FILE, "rm", "-f", "/var/ntp_run", NULL_STR);
		//RunSystemCmd("/proc/pptp_src_ip", "echo", "0 0", NULL_STR);
		if(wan_interface[0])
		{
			RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "down", NULL_STR);
			RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "up", NULL_STR);
		}

		if(enable_wan==1 && (opmode == GATEWAY_MODE || opmode==WISP_MODE)){/*for init internet wan setting*/ 
			if(opmode==WISP_MODE)
			{
//printf("\r\n wan_interface=[%s],__[%s-%u]\r\n",wan_interface,__FILE__,__LINE__);				
				if(SetWlan_idx(wan_interface)){
					apmib_get(MIB_WLAN_ENCRYPT, (void *)&intValue);
					if(intValue != 0){
#if 0 //why wlan0 is wisp_wan interface and encrypt is not none, than wlan1's keep_rsnie will also be set as 1?????
						for(i=0;i<NUM_WLAN_INTERFACE;i++)
						{
							sprintf(tmp_args, " wlan%d", i);
							RunSystemCmd(NULL_FILE, "iwpriv", tmp_args, "set_mib", "keep_rsnie=1", NULL_STR);
						}
#else
						RunSystemCmd(NULL_FILE, "iwpriv", wan_interface, "set_mib", "keep_rsnie=1", NULL_STR);
#endif						
					}
				}
			}
			RunSystemCmd(PROC_FASTNAT_FILE, "echo", "1", NULL_STR);
			
			if(wan_dhcp_mode==PPTP){
				RunSystemCmd(PROC_FASTPPTP_FILE, "echo", "1", NULL_STR);
				apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&intValue);
				if(intValue==1){
					RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "3", NULL_STR);
				}else{
					RunSystemCmd(PROC_PPTP_CONN_FILE, "echo", "0", NULL_STR);
				}
			}else{
				RunSystemCmd(PROC_FASTPPTP_FILE, "echo", "0", NULL_STR);
			}
				
			if(wan_dhcp_mode==L2TP){
				RunSystemCmd(PROC_FASTL2TP_FILE, "echo", "1", NULL_STR);
			}else{
				RunSystemCmd(PROC_FASTL2TP_FILE, "echo", "0", NULL_STR);
			}

			if(wan_dhcp_mode == PPPOE) {
				RunSystemCmd(PROC_FASTPPPOE_FILE, "echo", "1", NULL_STR);
			} else {
				RunSystemCmd(PROC_FASTPPPOE_FILE, "echo", "0", NULL_STR);
			}
			
	#ifdef HOME_GATEWAY		
			if((wan_dhcp_mode !=DHCP_SERVER && wan_dhcp_mode < 7) || (wan_dhcp_mode == USB3G) || (wan_dhcp_mode == AFTR) || (wan_dhcp_mode == DHCP_NONE)){ /* */
				start_wan(wan_dhcp_mode, opmode, wan_interface, br_interface, wisp_wan_id, 1);
			}else
				printf("Invalid wan type:wan_dhcp_mode=%d\n", wan_dhcp_mode);
	#endif			
		}
		else if(enable_wan==1 && opmode == BRIDGE_MODE ){
			/*Bridge mode, eth1 mtu should be sync with eth0 mtu (assume that eth0 mtu is 1500 here!!!)
			Otherwise, ping large pkt failed when GW mode changed to Bridge mode
			if eth1 mtu not equal with eth0 mtu.*/
			if(wan_interface[0])
				RunSystemCmd(NULL_FILE, "ifconfig", wan_interface, "mtu", "1500", NULL_STR);
		}

		//set kthreadd high priority for performance
		RunSystemCmd(NULL_FILE, "renice -20 2", NULL_STR);
		//set ksoftirqd high priority for performance
		RunSystemCmd(NULL_FILE, "renice -20 3", NULL_STR);
		//set boa high priority
		if(isFileExist(WEBS_PID_FILE)){
			pid=getPid_fromFile(WEBS_PID_FILE);
			if(pid != 0){
				sprintf(strPID, "%d", pid);
				RunSystemCmd(NULL_FILE, "renice", "-20", strPID, NULL_STR);
			}
		}
	}
	else
	{ /*gateway is 0, it is ap mode*/
		set_br_interface(tmpBuff);
		setbridge(tmpBuff);

		//Not used at present in order to find the root cause!!!
		//Patch: wlan pc can't visit AP using wapi-psk or wapi-cert when AP boots up
		//RunSystemCmd(NULL_FILE, "ifconfig", wlan_interface, "down", NULL_STR);
		//RunSystemCmd(NULL_FILE, "ifconfig", wlan_interface, "up", NULL_STR);
			
		/* init log setting*/
		set_log();
		if(lan_dhcp_mode==DHCP_LAN_SERVER){	
			sleep(1);
			//RunSystemCmd(NULL_FILE, "dhcpd.sh", br_interface, "ap", NULL_STR);
			set_lan_dhcpd(br_interface, 1);
		}	
		if(lan_dhcp_mode==DHCP_LAN_SERVER || lan_dhcp_mode==DHCP_LAN_NONE){	
			start_wlanapp(v_wlan_app_enabled);
		}	
#if defined(CONFIG_APP_APPLE_MFI_WAC)		
		// IOT dhcp client device need to give up br0 ip here , since br0 ip will be decide in IOT networking framework (dhcpc+avahi)
		// it is implement in wfaudio package 
		if(lan_dhcp_mode==DHCP_LAN_NONE || lan_dhcp_mode==DHCP_LAN_CLIENT)
		{
			//printf("give up br0 ip !!!\n");
			system("ifconfig br0 0.0.0.0");		
		}		
#endif	
	}

#ifndef STAND_ALONE_MINIUPNP
	start_upnpd(gateway, opmode);
#endif
	if(gateway==1 && opmode != BRIDGE_MODE){
		if(isFileExist(LLTD_PROCESS_FILE)){
			RunSystemCmd(NULL_FILE, "lld2d", br_interface, NULL_STR);
		}
	}
	if(isFileExist(SNMPD_PROCESS_FILE)){
		RunSystemCmd(NULL_FILE, "snmpd.sh", "restart", NULL_STR);
		//RunSystemCmd(NULL_FILE, "snmpd", "-c", SNMPD_CONF_FILE, "-p", SNMPD_PID_FILE,  NULL_STR);
	}
	if(isFileExist(NMSD_PROCESS_FILE)){
		RunSystemCmd(NULL_FILE, "nmsd", NULL_STR);
	}

	for(index=0; index<NUM_WLAN_INTERFACE; index++)
		start_wlan_by_schedule(index);

#if 0//defined(CONFIG_RTL_WAPI_SUPPORT)//Need more test!!!!
#if !defined(CONFIG_RTL_802_1X_CLIENT_SUPPORT)	// wlan0 down-> up will make 802.1x client mode auth fail when reinit or reboot
	system("ifconfig wlan0 down");
	system("ifconfig wlan0 up");
#endif
#endif

#if defined(CONFIG_IPV6)
	system("ip tunnel del tun 2> /dev/null");
	set_ipv6();
#endif

#ifdef HOME_GATEWAY // To enable/disable ipv6 passthru no matter wan is connected or not
	if(opmode == GATEWAY_MODE)	// Gateway mode
		apmib_get(MIB_CUSTOM_PASSTHRU_ENABLED, (void *)&intValue);
	else
		intValue=0;

	RunSystemCmd("/proc/custom_Passthru", "echo", (intValue & 0x1)?"1":"0", NULL_STR);
	if (intValue == 0)
	{
		RunSystemCmd(NULL_FILE, "brctl", "delif", "br0", "peth0", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "peth0", "down", NULL_STR);
	}
	else
	{
		RunSystemCmd(NULL_FILE, "brctl", "addif", "br0", "peth0", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "peth0", "up", NULL_STR);
	}
#if defined(CONFIG_RTL_92D_SUPPORT)||defined (CONFIG_RTL_8881A)
	if(opmode == WISP_MODE)
	{
		apmib_get(MIB_WISP_WAN_ID, (void *)&wispWanId);
		apmib_get(MIB_CUSTOM_PASSTHRU_ENABLED, (void *)&intValue);		
		apmib_get(MIB_REPEATER_ENABLED1,(void *)&repeater_enable1);
		apmib_get(MIB_REPEATER_ENABLED2,(void *)&repeater_enable2);
		if(intValue != 0)
		{
			char tmpStr[16];
			/*should also config wisp wlan index for dual band wireless interface*/
			intValue=((wispWanId&0xF)<<4)|intValue;
			if(repeater_enable1==1 || repeater_enable2==1)
				intValue = intValue | 0x8;
			memset(tmpStr,0,sizeof(tmpStr));
			sprintf(tmpStr,"%d",intValue);
				
			RunSystemCmd("/proc/custom_Passthru_wlan", "echo", tmpStr, NULL_STR);
			//RunSystemCmd("/proc/custom_Passthru_wlan", "echo", (intValue & 0x1)?"1":"0", NULL_STR);
		}
		else
		{
			RunSystemCmd("/proc/custom_Passthru_wlan", "echo", (intValue & 0x1)?"1":"0", NULL_STR);
		}
	}
#else
	if(opmode == WISP_MODE)	//WISP mode, wireless enabled  and wireless client mode enabled
	{		
		apmib_get(MIB_REPEATER_ENABLED1,(void *)&repeater_enable1);

		apmib_get(MIB_CUSTOM_PASSTHRU_ENABLED, (void *)&intValue);
		if(intValue !=0)
		{
			char tmpStr[16];
			if(repeater_enable1==1)
				intValue |= 0x8;
			memset(tmpStr,0,sizeof(tmpStr));
			sprintf(tmpStr,"%d",intValue);
			RunSystemCmd("/proc/custom_Passthru_wlan", "echo", tmpStr, NULL_STR);
		}
		else
		{
			RunSystemCmd("/proc/custom_Passthru_wlan", "echo", (intValue & 0x1)?"1":"0", NULL_STR);
		}
	}
	else
	{
	intValue=0;
	RunSystemCmd("/proc/custom_Passthru_wlan", "echo", (intValue & 0x1)?"1":"0", NULL_STR);
	}
#endif
	
	if (intValue == 0)
	{
		RunSystemCmd(NULL_FILE, "brctl", "delif", "br0", "pwlan0", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "pwlan0", "down", NULL_STR);
	}
	else
	{
		RunSystemCmd(NULL_FILE, "brctl", "addif", "br0", "pwlan0", NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "pwlan0", "up", NULL_STR);
	}
#endif



	/*enable igmp snooping*/
	/*igmp snooping is independent with igmp proxy*/
#if defined (CONFIG_IGMPV3_SUPPORT)
	RunSystemCmd(PROC_BR_IGMPVERSION, "echo", "3", NULL_STR);
#else
	RunSystemCmd(PROC_BR_IGMPVERSION, "echo", "2", NULL_STR);
#endif
	RunSystemCmd(PROC_BR_IGMPSNOOP, "echo", "1", NULL_STR);
	RunSystemCmd(PROC_BR_IGMPQUERY, "echo", "1", NULL_STR);
#if defined (CONFIG_RTL_MLD_SNOOPING)	
	RunSystemCmd(PROC_BR_MLDSNOOP, "echo", "1", NULL_STR);
	RunSystemCmd(PROC_BR_MLDQUERY, "echo", "1", NULL_STR);
#endif

	intValue =0;
	apmib_get(MIB_IGMP_FAST_LEAVE_DISABLED, (void *)&intValue);
	if(intValue)
	{
		RunSystemCmd(PROC_BR_IGMPSNOOP, "echo", "fastleave","0","2", NULL_STR);
	}
	else
	{	
		RunSystemCmd(PROC_BR_IGMPSNOOP, "echo", "fastleave","1","0", NULL_STR);
	}
	
#ifdef SUPPORT_ZIONCOM_RUSSIA
	RunSystemCmd("/proc/sys/net/ipv4/conf/eth1/force_igmp_version", "echo", "2", NULL_STR);
#endif
	//RunSystemCmd("/proc/sys/net/ipv4/ip_conntrack_max", "echo", "2048", NULL_STR);

#ifdef CONFIG_RTL_HIGH_PERFORMANCE_FILESYSTEM
	load_rtl_fs_module();
#endif
	
#if defined(CONFIG_APP_USBMOUNT)
	start_mount();
#if defined (CONFIG_APP_SAMBA)
	apmib_get(MIB_SAMBA_ENABLED, (void*)&intValue);
	if(intValue==1) {
		start_samba();
	}
#endif
#if defined (CONFIG_APP_VSFTPD)
	start_vsftpd();
#endif	
#endif

#if defined(CONFIG_APP_DLNA_DMS)
	//printf("%s, %d\n", __FUNCTION__, __LINE__);
	system("ushare -f /etc/ushare.conf &");
#endif
#ifdef RTK_USB3G
    apmib_get(MIB_WAN_DHCP, (void*)&intValue);
    if(opmode == 0 && intValue == USB3G) {
        system("echo \"/sbin/hotplug\" > /proc/sys/kernel/hotplug");
        system("mount -t sysfs none /sys           >/dev/null 2>&1");
        system("mount -t usbfs none /proc/bus/usb  >/dev/null 2>&1");
        system("mount -t tmpfs none /dev           >/dev/null 2>&1");
        system("mdev -s                            >/dev/null 2>&1");
        system("echo \"remove\" > /var/usb3g.stat");
        system("mnet -d &");
    }
#endif /* #ifdef RTK_USB3G */

#if defined(HTTP_FILE_SERVER_SUPPORTED)
		RunSystemCmd("/proc/http_file/getLanIp", "echo", "1", NULL_STR);
#endif

	#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT) || defined(CONFIG_APP_APPLE_MFI_WAC)
		system("rm -f  /var/system/start_init 2> /dev/null");
	#endif
	
#ifdef CONFIG_APP_APPLE_MFI_WAC
	if(isFileExist("/var/system/mdnsd_started")==0){
		system("mdnsd");
		system("echo 1 > /var/system/mdnsd_started");
	}
	system ("wfaudio &"); //some brach start in rcs
#if defined(CONFIG_APPLE_HOMEKIT)
	system("hapserver 1 0 0 &");
#endif
#endif

#if defined(CONFIG_APP_FWD)
	//##For fwd
		//system("mount -t tmpfs none /sbin"); //put fwd to ram
		//system("cp /bin/fwd /sbin"); //put fwd to ram
		system("fwd &");
#endif
#if 0 //defined(CONFIG_APPLE_HOMEKIT)
	//simply init homekit server , 3 options with default value 
	// thet are  "useMfi" , "reset Paired list" , "reset Device Json data"	
	system("hapserver 1 0 0 &");
#endif
	//reply only if the target IP address is local address configured on the incoming interface
	RunSystemCmd("/proc/sys/net/ipv4/conf/eth1/arp_ignore", "echo", "1", NULL_STR);
	/*increase routing cache rebuild count from 4 to 2048*/
	RunSystemCmd(RT_CACHE_REBUILD_COUNT, "echo", "2048", NULL_STR);
	system("timelycheck &");
#if defined(CONFIG_AUTO_DHCP_CHECK)
	if(opmode==BRIDGE_MODE && lan_dhcp_mode == DHCP_SERVER)
	{
		system("Auto_DHCP_Check &");
	}
#endif
#ifdef SAMBA_WEB_SUPPORT
	system("cp /etc/passwd_orig /var/passwd");
	system("cp /etc/group_orig /var/group");
	system("cp /etc/samba/smbpasswd_orig /var/samba/smbpasswd");
	//system("cp /etc/samba/smb_orig.conf /var/samba/smb.conf");
	
	apmib_get(MIB_STORAGE_GROUP_TBL_NUM,(void*)&number);
	for(i = 0;i < number;i++)
	{
		memset(&group_info,'\0',sizeof(STORAGE_GROUP_T));
		*((char*)&group_info) = (char)(i+1);
		apmib_get(MIB_STORAGE_GROUP_TBL,(void*)&group_info);

		memset(cmdBuffer,'\0',100);
		snprintf(cmdBuffer,100,"addgroup %s",group_info.storage_group_name);
		system(cmdBuffer);
	}
	
	apmib_get(MIB_STORAGE_USER_TBL_NUM,(void*)&number);
	for(i = 0;i < number;i++)
	{
		memset(&user_info,'\0',sizeof(STORAGE_USER_T));
		*((char*)&user_info) = (char)(i+1);
		apmib_get(MIB_STORAGE_USER_TBL,(void*)&user_info);

		memset(cmdBuffer,'\0',100);
		if(!strcmp(user_info.storage_user_group,"--")){
			snprintf(cmdBuffer,100,"adduser %s",user_info.storage_user_name);
			system(cmdBuffer);
		}else{
			snprintf(cmdBuffer,100,"adduser -G %s %s",user_info.storage_user_group,user_info.storage_user_name);
			system(cmdBuffer);
		}

		memset(cmdBuffer,'\0',100);
		snprintf(cmdBuffer,100,"smbpasswd %s %s",user_info.storage_user_name,user_info.storage_user_password);
		system(cmdBuffer);
	}
	storage_UpdateSambaConf();
#endif

#if defined(CONFIG_APP_TR069)
	if (find_pid_by_name("cwmpClient") == 0)
		start_tr069();
#endif

#if defined(CONFIG_APP_RTK_INBAND_CTL)
	//RunSystemCmd(PROC_INBAND_CTL_ACL, "echo", "0x8899", NULL_STR);
	system("hcd -daemon &");
#endif

#if defined(CONFIG_RTL_ETH_802DOT1X_SUPPORT)
	init_EthDot1x(wan_dhcp_mode, wan_dhcp_mode, wan_interface, br_interface);
#endif

#ifdef RTK_CAPWAP
	capwap_app();
#endif

#if defined(CONFIG_RPS)
	rtl_configRps();
#endif

	return 0;
}

void Init_Internet(int argc, char** argv)
{
#ifdef   HOME_GATEWAY	
	int wisp_id=0, wan_mode=0, opmode=0;
	char br_interface[16]={0};
	char wan_interface[16]={0};
	char tmp_args[16]={0};
	
	
	if(argc < 4)
		return;
		
	apmib_get(MIB_OP_MODE,(void *)&opmode);
	if(opmode == BRIDGE_MODE)
		return;
	
	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_id);
	
	sprintf(tmp_args, "%s",argv[1]);
	if(!strcmp(tmp_args, "pppoe"))
		wan_mode=3;
	else if(!strcmp(tmp_args, "pptp"))
		wan_mode=4;	
	else if(!strcmp(tmp_args, "l2tp"))
		wan_mode=6;
	else{
		printf("Un-support wan type for init\n");
		return;
	}
	sprintf(br_interface, "%s", "br0");
	sprintf(wan_interface, "%s",argv[3]);
	start_wan(wan_mode, opmode, wan_interface, br_interface, wisp_id, 0);		
#endif	
}
 
void Init_QoS(int argc, char** argv)
{
#ifdef   HOME_GATEWAY	
	int wisp_id=0, wan_mode=0, opmode=0;
	
	
	apmib_get(MIB_OP_MODE,(void *)&opmode);
	if(opmode == 1)
		return;
		
	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_id);
	apmib_get(MIB_WAN_DHCP,(void *)&wan_mode);		
	set_QoS(opmode, wan_mode, wisp_id);
#endif	
	
} 
 
#ifdef CONFIG_SMART_REPEATER
int getWispRptIfaceName(char*pIface,int wlanId)
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
	if((AP_MODE==wlanMode || AP_WDS_MODE==wlanMode || AP_MESH_MODE==wlanMode || MESH_MODE==wlanMode ) && rptEnabled)
	{//root AP mode and rpt enabled, use -vxd as wanIf
		if(!strstr(pIface,"-vxd"))
		{
			strcat(pIface,"-vxd");
		}
	}else
	{
		char * ptmp = strstr(pIface,"-vxd");
		if(ptmp)
			memset(ptmp,0,sizeof(char)*strlen("-vxd"));
	}
	apmib_recov_wlanIdx();
	return 0;
}
#endif
 
 
 
 
 
