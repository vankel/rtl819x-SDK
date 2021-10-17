/*================================================================*/
/* System Include Files */

#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
#include <sys/types.h> 
#include <sys/uio.h> 
#include <unistd.h> 
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/wait.h> 
#include <net/if.h>
#include <linux/if_packet.h>
#include "../../inband_lib/wireless_copy.h"
#if defined(CONFIG_APP_ADAPTER_API)
#include "../../adapter-api/rtk_api/rtk_disk_adapter.h"
#include "../../adapter-api/rtk_api/rtk_api.h"
#include "../../adapter-api/rtk_api/rtk_firewall_adapter.h"

#endif
/*================================================================*/
/* Local Include Files */

#include <sys/stat.h>

#include "hcd.h"
#include "mib.h"
#include "cmd.h"
#include "wlan_if.h"
#include "site_survey.h"

#ifdef KERNEL_3_10
#include <linux/fs.h>
#endif
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
#include "RgbLedBlink.h"
#endif
//#include "inband_if.h"
#define MAX_INBAND_PAYLOAD_LEN 1480


/*=Local variables===============================================================*/
WLAN_RATE_T rate_11n_table_20M_LONG[]={
	{MCS0, 	"6.5"},
	{MCS1, 	"13"},
	{MCS2, 	"19.5"},
	{MCS3, 	"26"},
	{MCS4, 	"39"},
	{MCS5, 	"52"},
	{MCS6, 	"58.5"},
	{MCS7, 	"65"},
	{MCS8, 	"13"},
	{MCS9, 	"26"},
	{MCS10, 	"39"},
	{MCS11, 	"52"},
	{MCS12, 	"78"},
	{MCS13, 	"104"},
	{MCS14, 	"117"},
	{MCS15, 	"130"},
	{0}
};
WLAN_RATE_T rate_11n_table_20M_SHORT[]={
	{MCS0, 	"7.2"},
	{MCS1, 	"14.4"},
	{MCS2, 	"21.7"},
	{MCS3, 	"28.9"},
	{MCS4, 	"43.3"},
	{MCS5, 	"57.8"},
	{MCS6, 	"65"},
	{MCS7, 	"72.2"},
	{MCS8, 	"14.4"},
	{MCS9, 	"28.9"},
	{MCS10, 	"43.3"},
	{MCS11, 	"57.8"},
	{MCS12, 	"86.7"},
	{MCS13, 	"115.6"},
	{MCS14, 	"130"},
	{MCS15, 	"144.5"},
	{0}
};
WLAN_RATE_T rate_11n_table_40M_LONG[]={
	{MCS0, 	"13.5"},
	{MCS1, 	"27"},
	{MCS2, 	"40.5"},
	{MCS3, 	"54"},
	{MCS4, 	"81"},
	{MCS5, 	"108"},
	{MCS6, 	"121.5"},
	{MCS7, 	"135"},
	{MCS8, 	"27"},
	{MCS9, 	"54"},
	{MCS10, 	"81"},
	{MCS11, 	"108"},
	{MCS12, 	"162"},
	{MCS13, 	"216"},
	{MCS14, 	"243"},
	{MCS15, 	"270"},
	{0}
};
WLAN_RATE_T rate_11n_table_40M_SHORT[]={
	{MCS0, 	"15"},
	{MCS1, 	"30"},
	{MCS2, 	"45"},
	{MCS3, 	"60"},
	{MCS4, 	"90"},
	{MCS5, 	"120"},
	{MCS6, 	"135"},
	{MCS7, 	"150"},
	{MCS8, 	"30"},
	{MCS9, 	"60"},
	{MCS10, 	"90"},
	{MCS11, 	"120"},
	{MCS12, 	"180"},
	{MCS13, 	"240"},
	{MCS14, 	"270"},
	{MCS15, 	"300"},
	{0}
};
//changes in following table should be synced to VHT_MCS_DATA_RATE[] in 8812_vht_gen.c
const unsigned short VHT_MCS_DATA_RATE[3][2][20] = 
	{	{	{13, 26, 39, 52, 78, 104, 117, 130, 156, 156,
			 26, 52, 78, 104, 156, 208, 234, 260, 312, 312},			// Long GI, 20MHz
			{14, 29, 43, 58, 87, 116, 130, 144, 173, 173,
			29, 58, 87, 116, 173, 231, 260, 289, 347, 347}	},		// Short GI, 20MHz
		{	{27, 54, 81, 108, 162, 216, 243, 270, 324, 360, 
			54, 108, 162, 216, 324, 432, 486, 540, 648, 720}, 		// Long GI, 40MHz
			{30, 60, 90, 120, 180, 240, 270, 300,360, 400, 
			60, 120, 180, 240, 360, 480, 540, 600, 720, 800}},		// Short GI, 40MHz
		{	{59, 117,  176, 234, 351, 468, 527, 585, 702, 780,
			117, 234, 351, 468, 702, 936, 1053, 1170, 1404, 1560}, 	// Long GI, 80MHz
			{65, 130, 195, 260, 390, 520, 585, 650, 780, 867, 
			130, 260, 390, 520, 780, 1040, 1170, 1300, 1560,1733}	}	// Short GI, 80MHz
	};

#define CFG_FILE_IN "/etc/Wireless/realtekap.conf"

char *lan_link_spec[2] = {"DOWN","UP"};
char *lan_speed_spec[3] = {"10M","100M","1G"};
char *enable_spec[2] = {"DIsable","Enable"};
//#define FLASH_DEVICE_KERNEL               ("/dev/mtd")
#define FLASH_DEVICE_KERNEL				("/dev/mtdblock0")

#define FLASH_DEVICE_ROOTFS              ("/dev/mtdblock1")

#define HDR_INFO_OFFSET 4
#define HDR_IOCTL_DATA_OFFSET 4+2
#define LOCAL_NETIF  ("br0")
static int ioctl_sock=-1;
static int hcd_inband_ioctl_chan=-1;
#define CMD_INBAND_IOCTL 0x03
#define CMD_INBAND_SYSTEMCALL 0x04



extern int hcd_inband_chan;

#define SIGNATURE_ERROR		(1<<0)
#define WEBCHECKSUM_ERROR	(1<<1)
#define ROOTCHECKSUM_ERROR	(1<<2)
#define LINUXCHECKSUM_ERROR	(1<<3)

//flash|memory data cmp
#define FLASHDATA_ERROR		(1<<8)
#define WEBDATA_ERROR		(1<<9)
#define LINUXDATA_ERROR		(1<<10)
#define ROOTDATA_ERROR		(1<<11)

static int firm_upgrade_status = 0;

/**/
static int _get_mib(char *cmd , int cmd_len);
static int _set_mib(char *cmd , int cmd_len);
static int _set_mibs(char *cmd , int cmd_len);
//static int _set_mibs(void);
static int _getstainfo(char *cmd , int cmd_len);
static int _getassostanum(char *cmd , int cmd_len);
static int _getbssinfo(char *cmd , int cmd_len);
static int _sysinit(char *cmd , int cmd_len);
static int _getstats(char *cmd , int cmd_len);	
static int _getlanstatus(char *cmd , int cmd_len);
static int _getlanRate(char *cmd,int cmd_len);
static int _setlanBandwidth(char *cmd, int cmd_len);

//static int _sendfile(char *cmd , int cmd_len);
static int _firm_upgrade(char *cmd , int cmd_len);
static int _firm_check_signature_checksum(char *cmd , int cmd_len);
static int _firm_check_flash_data(char *cmd , int cmd_len);
static int _firm_upgrade_reboot(char *cmd , int cmd_len);
static int host_ioctl_receive(char *ioctl_data_p,int ioctl_data_len);
static int host_systemcall_receive(char *data,int data_len);
#ifdef HOST_SEND_CONFIG_FILE
static int _send_config_file(char *cmd , int cmd_len);
static int _get_config_file(char *data , int dat_len);
#endif
#ifdef INBAND_GET_FILE_SUPPOPRT
static int _get_file(char *cmd , int cmd_len);
#endif
#if defined(CONFIG_APP_ADAPTER_API)
static int _sync_to_server(char *cmd, int cmd_len);
#endif
static int _request_scan(char *cmd , int cmd_len);

static int _get_scan_result(char *cmd , int cmd_len);
#ifdef CONFIG_RTL_LITE_CLNT
static int 	_start_lite_clnt_connect(char *cmd , int cmd_len);
static int 	_wlan_sync(char *cmd , int cmd_len);
static int 	_set_wlan_off(char *cmd , int cmd_len);
static int 	_set_wlan_on(char *cmd , int cmd_len);


#endif

#if defined(CONFIG_APP_ADAPTER_API)
static int _get_storage_info(char *cmd , int cmd_len);
static int _format_partition(char *cmd , int cmd_len);

static int _get_wan_status_By_Url(char *cmd , int cmd_len);
static int _get_lan_terminal_info(char *cmd , int cmd_len);
static int _get_upload_speed(char *cmd , int cmd_len);
static int _get_download_speed(char *cmd , int cmd_len);
static int _get_phy_port_status(char *cmd , int cmd_len);
static int _get_lan_drop_rate(char *cmd , int cmd_len);
static int _get_wan_drop_rate(char *cmd , int cmd_len);
static int _get_wlan_drop_rate(char *cmd , int cmd_len);
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)

static int _get_qos_rule_monopoly(char* cmd, int cmd_len);
static int _set_qos_rule_monopoly(char* cmd, int cmd_len);
static int _set_qos_rule_monopoly_imm(char* cmd, int cmd_len);
#endif
#if defined (QOS_BY_BANDWIDTH)
static int _get_qos_rule(char *cmd , int cmd_len);
static int _add_qos_rule_imm(char *cmd, int cmd_len);
static int _del_qos_rule_imm(char *cmd, int cmd_len);
#endif
static int _restart_wan(char *cmd, int cmd_len);
static int _get_pppoe_err_code(char *cmd, int cmd_len);
static int _wlan_immediately_work(char *cmd, int cmd_len);
#if CONFIG_RTK_NAS_FILTER
static int  _enable_nas_filter(char *cmd, int cmd_len);
static int  _get_status_nas_filter(char *cmd, int cmd_len);
static int  _add_nas_filter(char *cmd, int cmd_len);
static int  _del_nas_filter(char *cmd, int cmd_len);
static int  _get_nas_filter_entry(char *cmd, int cmd_len);
#endif
static int _get_macfilter_rule(char *cmd , int cmd_len);
static int  _add_macfilter_rule(char *cmd, int cmd_len);
static int _del_macfilter_rule(char *cmd, int cmd_len);
#endif

static int _get_firmware_version(char *cmd, int cmd_len);
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
static int _set_RGBLed_blinkStart(char *cmd , int cmd_len);
static int _set_RGBLed_blinkEnd(char *cmd , int cmd_len);
#endif

struct cmd_entry cmd_table[]={ \
/*Action cmd - ( name, func) */
	CMD_DEF(set_mib, _set_mib),
	CMD_DEF(get_mib, _get_mib),	
	CMD_DEF(getstainfo, _getstainfo),
	CMD_DEF(getassostanum, _getassostanum),
	CMD_DEF(getbssinfo, _getbssinfo),
	CMD_DEF(sysinit, _sysinit),
	CMD_DEF(getstats, _getstats),
	CMD_DEF(getlanstatus, _getlanstatus),
	CMD_DEF(getlanRate, _getlanRate),
	CMD_DEF(setlanBandwidth, _setlanBandwidth),
	CMD_DEF(set_mibs, _set_mibs),
    //CMD_DEF(sendfile, _sendfile),
#ifdef CONFIG_HCD_FLASH_SUPPORT  
    CMD_DEF(firm_upgrade, _firm_upgrade),
    CMD_DEF(firm_check_sig_checksum,_firm_check_signature_checksum),
    CMD_DEF(firm_check_flash_data,_firm_check_flash_data),
    CMD_DEF(firm_upgrade_reboot,_firm_upgrade_reboot),
#endif
	CMD_DEF(ioctl, host_ioctl_receive),
	CMD_DEF(syscall, host_systemcall_receive),
#ifdef HOST_SEND_CONFIG_FILE
	CMD_DEF(send_config_file,_send_config_file),
	CMD_DEF(get_config_file,_get_config_file),
#endif
#ifdef INBAND_GET_FILE_SUPPOPRT
	CMD_DEF(get_file,_get_file),
#endif
#if defined(CONFIG_APP_ADAPTER_API)
	CMD_DEF(sync_to_server,_sync_to_server),
#endif	
	CMD_DEF(request_scan,_request_scan),
	CMD_DEF(get_scan_result,_get_scan_result),
#if defined(CONFIG_RTL_LITE_CLNT)
	CMD_DEF(wlan_sync,_wlan_sync),
	CMD_DEF(start_lite_clnt_connect,_start_lite_clnt_connect),
	CMD_DEF(set_wlan_on,_set_wlan_on),
	CMD_DEF(set_wlan_off,_set_wlan_off),
#endif

#if defined(CONFIG_APP_ADAPTER_API)
	CMD_DEF(get_storage_info,_get_storage_info),
	CMD_DEF(format_partition,_format_partition),	
	CMD_DEF(get_wan_status,_get_wan_status_By_Url),
	CMD_DEF(get_lan_terminal_info,_get_lan_terminal_info),
	CMD_DEF(get_upload_speed,_get_upload_speed),
	CMD_DEF(get_download_speed,_get_download_speed),
	CMD_DEF(get_phy_port_status,_get_phy_port_status),
	CMD_DEF(get_lan_drop_rate,_get_lan_drop_rate),
	CMD_DEF(get_wan_drop_rate,_get_wan_drop_rate),
	CMD_DEF(get_wlan_drop_rate,_get_wlan_drop_rate),
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
	CMD_DEF(get_qos_rule_monopoly,_get_qos_rule_monopoly),
	CMD_DEF(set_qos_rule_monopoly,_set_qos_rule_monopoly),
	CMD_DEF(set_qos_rule_monopoly_imm,_set_qos_rule_monopoly_imm),
#endif
#if defined(QOS_BY_BANDWIDTH)
	CMD_DEF(get_qos_rule,_get_qos_rule),
	CMD_DEF(add_qos_rule_imm,_add_qos_rule_imm),
	CMD_DEF(del_qos_rule_imm,_del_qos_rule_imm),
#endif
	CMD_DEF(restart_wan,_restart_wan),
	CMD_DEF(get_pppoe_err_code,_get_pppoe_err_code),
	CMD_DEF(wlan_immediately_work,_wlan_immediately_work),
	CMD_DEF(get_macfilter_rule,_get_macfilter_rule),
	CMD_DEF(add_macfilter_rule_imm,_add_macfilter_rule),
	CMD_DEF(del_macfilter_rule_imm,_del_macfilter_rule),
	CMD_DEF(get_fw_version, _get_firmware_version),
#endif
#if CONFIG_RTK_NAS_FILTER
    CMD_DEF(enable_nas_filter, _enable_nas_filter),
    CMD_DEF(get_status_nas_filter, _get_status_nas_filter),
    CMD_DEF(add_nas_filter, _add_nas_filter),
    CMD_DEF(del_nas_filter, _del_nas_filter),
    CMD_DEF(get_nas_filter_entry, _get_nas_filter_entry),
#endif
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
	CMD_DEF(set_rgb_blink_start,_set_RGBLed_blinkStart),
	CMD_DEF(set_rgb_blink_end,_set_RGBLed_blinkEnd),
#endif
	/* last one type should be LAST_ENTRY - */   
	{0}
};

int do_cmd(int id , char *cmd ,int cmd_len ,int relply)
{
	int i=0,ret=-1,len=0;
	
	//printf("[%d],%d,cmd:%d,%s [%s]:[%d].\n",i,id,cmd_len,cmd,__FUNCTION__,__LINE__);
	while (cmd_table[i].id != LAST_ENTRY_ID) {
		if ((cmd_table[i].id == id))	{
			ret = cmd_table[i].func(cmd,cmd_len);
			//printf("[%d],%d,%s ret:%d,[%s]:[%d].\n",i,id,cmd,ret,__FUNCTION__,__LINE__);
			break;
		}	
		i++;
	}
	//no reply
	
	if(!relply || cmd_table[i].id == id_ioctl )
		return ret;

	if(ret > MAX_INBAND_PAYLOAD_LEN) //mark_issue , it will be reply in it's func in command table
		return ret;

	//reply rsp pkt
	if (ret >= 0) { 
		if (ret == 0) { 
			cmd[0] = '\0';
			ret = 1;
		}

		inband_write(hcd_inband_chan,0,id,cmd,ret,1); //good reply
		//printf("%d,%s[%s]:[%d].\n",id,cmd,__FUNCTION__,__LINE__);
	}
	else{ //error rsp	
		cmd[0] = (unsigned char)( ~ret + 1);			
		inband_write(hcd_inband_chan,0,id,cmd,1,2); //error reply
		//printf(" %d,%s[%s]:[%d].\n",id,cmd,__FUNCTION__,__LINE__);
	}			
	
	return ret;
}

#if 0 //mark_firm
static inline int CHECKSUM_OK(unsigned char *data, int len)
{
        int i;
        unsigned char sum=0;

        for (i=0; i<len; i++)
                sum += data[i];

        if (sum == 0)
                return 1;
        else
                return 0;
}
#endif
static int get_mib_value(int type, int vidx,unsigned char *value, unsigned char *mib_name, unsigned int is_string)
{
	FILE *stream;
	char line[256];
	char string[64];
	unsigned char cmd_buf[256];
	unsigned char *p;
	if(type ==0 || type ==1)
		if(vidx == 0)
			sprintf(cmd_buf, "flash get wlan%d %s > %s", type, mib_name, TEMP_MIB_FILE);
		else if(vidx == -1)
			sprintf(cmd_buf, "flash get wlan%d-vxd %s > %s", type, mib_name, TEMP_MIB_FILE);
		else
			sprintf(cmd_buf, "flash get wlan%d-va%d %s > %s", type,vidx, mib_name, TEMP_MIB_FILE);
	else
		sprintf(cmd_buf, "flash get %s > %s", mib_name, TEMP_MIB_FILE);
	system(cmd_buf);
	stream = fopen (TEMP_MIB_FILE, "r" );
	if ( stream != NULL ) {		
		if(fgets(line, sizeof(line), stream))
		{
			sscanf(line, "%*[^=]=%[^\n]", string);
			p=string;
			while(*p == ' ')
				p++;
			if((is_string == 1))
				p++;	//skip "
			strcpy(value, p);
			if(is_string == 1)
			{
				value[strlen(value)-1] = '\0';
			}
		}
		fclose(stream );	
	}	
	return 0;
}


#ifdef CONFIG_HCD_FLASH_SUPPORT
static int fwChecksumOk(char *data, int len)
{
        unsigned short sum=0;
        int i;

        for (i=0; i<len; i+=2) {
#ifdef _LITTLE_ENDIAN_
                sum += WORD_SWAP( *((unsigned short *)&data[i]) );
#else
                sum += *((unsigned short *)&data[i]);
#endif

        }
        return( (sum==0) ? 1 : 0);
}

void rtk_check_flash_mem(int linuxOffset,int linuxlen,int webOffset,int weblen,int rootOffset,int rootlen,char* upload_data,
						int webhead,int roothead,int linuxhead)
{
	int fh;
	char *flash_mem = NULL;

	/* mtdblock0 */
	fh = open(FLASH_DEVICE_KERNEL, O_RDWR);
	if(fh == -1)
	{
		firm_upgrade_status |= FLASHDATA_ERROR;
		return;
	}

	/* linux */
	if(linuxOffset  != -1)
	{
		lseek(fh, linuxOffset, SEEK_SET);
		flash_mem = (char*)malloc(linuxlen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,linuxlen);
		if(memcmp(&(upload_data[linuxhead]),flash_mem,linuxlen) != 0)
		{
			firm_upgrade_status |= LINUXDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}

	/* web */
	if(webOffset != -1)
	{
		lseek(fh, webOffset, SEEK_SET);
		flash_mem = (char*)malloc(weblen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,weblen);
		if(memcmp(&(upload_data[webhead]),flash_mem,weblen) != 0)
		{
			firm_upgrade_status |= WEBDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}
	close(fh);

	/* mtdblock1 */
	/* root */
	fh = open(FLASH_DEVICE_ROOTFS, O_RDWR);
	if(fh == -1)
	{
		firm_upgrade_status |= FLASHDATA_ERROR;
		return;
	}
	
	if(rootOffset != -1)
	{
		lseek(fh, rootOffset, SEEK_SET);
		flash_mem = (char*)malloc(rootlen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,rootlen);
		if(memcmp(&(upload_data[roothead]),flash_mem,rootlen) != 0)
		{
			firm_upgrade_status |= ROOTDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}

	close(fh);
}


void rtk_all_interface(int action)
{
	switch(action)
	{
	case 0:
	system("ifconfig ppp0 down> /dev/console");
	system("ifconfig eth0 down> /dev/console");
	system("ifconfig eth1 down> /dev/console");
	system("ifconfig eth2 down> /dev/console");
	system("ifconfig eth3 down> /dev/console");
	system("ifconfig eth4 down> /dev/console");
	system("ifconfig wlan0 down> /dev/console");
	system("ifconfig wlan1 down> /dev/console");

	break;
	case 1:
	system("ifconfig eth0 up > /dev/console");
	system("ifconfig eth1 up > /dev/console");
	system("ifconfig eth2 up > /dev/console");
	system("ifconfig eth3 up > /dev/console");
	system("ifconfig eth4 up> /dev/console");
	system("ifconfig wlan0 up> /dev/console");
	system("ifconfig wlan1 up> /dev/console");
	system("ifconfig ppp0 up> /dev/console");

	break;
	default:
	printf("error rtk_all_interface\n");
	break;
	}

}

int rtk_FirmwareCheck(char *upload_data, int upload_len)
{
	int head_offset=0 ;
	int isIncludeRoot=0;
	int		 len;
    int          locWrite;
    int          numLeft;
    int          numWrite;
    IMG_HEADER_Tp pHeader;
	int flag=0, startAddr=-1, startAddrWeb=-1;	
    int fh;
   	unsigned char buffer[200];
   	firm_upgrade_status = 0;
   	int webOffset = -1,weblen,rootOffset = -1,rootlen,linuxOffset = -1,linuxlen;
	int webhead,roothead,linuxhead;

	//printf("[%s]:%d\n",__func__,__LINE__);

	int fwSizeLimit = 0x800000;
	
while(head_offset <   upload_len) {
    locWrite = 0;
    pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
    len = pHeader->len;
    numLeft = len + sizeof(IMG_HEADER_T) ;

    
    // check header and checksum
    if (!memcmp(&upload_data[head_offset], FW_HEADER, SIGNATURE_LEN) ||
			!memcmp(&upload_data[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN))
    	flag = 1;
    else if (!memcmp(&upload_data[head_offset], WEB_HEADER, SIGNATURE_LEN))
    	flag = 2;
    else if (!memcmp(&upload_data[head_offset], ROOT_HEADER, SIGNATURE_LEN)){
    	flag = 3;
    	isIncludeRoot = 1;
    }	
#if 0		
	else if ( !memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) )	 {
		int type, status, cfg_len;
		cfg_len = updateConfigIntoFlash(&upload_data[head_offset], 0, &type, &status);
		
		if (status == 0 || type == 0) { // checksum error
			strcpy(buffer, "Invalid configuration file!");
			goto ret_upload;
		}
		else { // upload success
			strcpy(buffer, "Update successfully!");
			head_offset += cfg_len;
			update_cfg = 1;
		}    	
		continue;
	}
#endif	
    else {
       	strcpy(buffer, "Invalid file format!");
    	//winfred_wang
    	firm_upgrade_status |= SIGNATURE_ERROR;
		goto ret_upload;
    }

       if(len > fwSizeLimit){ //len check by sc_yang 
      		sprintf(buffer, "Image len exceed max size 0x%x ! len=0x%x",fwSizeLimit, len);
		goto ret_upload;
    }
    if ( (flag == 1) || (flag == 3)) {
    	if ( !fwChecksumOk(&upload_data[sizeof(IMG_HEADER_T)+head_offset], len)) {
      		sprintf(buffer, "Image checksum mismatched! len=0x%x, checksum=0x%x", len,
			*((unsigned short *)&upload_data[len-2]) );
			if(flag == 1)
				firm_upgrade_status |= LINUXCHECKSUM_ERROR;
			else if(flag == 3)
				firm_upgrade_status |= ROOTCHECKSUM_ERROR;
				
		goto ret_upload;
		}
    }
    else {
    	char *ptr = &upload_data[sizeof(IMG_HEADER_T)+head_offset];
    	if ( !CHECKSUM_OK(ptr, len) ) {
     		sprintf(buffer, "Image checksum mismatched! len=0x%x", len);
     		firm_upgrade_status |= WEBCHECKSUM_ERROR;
		goto ret_upload;
		}
    }

	head_offset += len + sizeof(IMG_HEADER_T) ;
	startAddr = -1 ; //by sc_yang to reset the startAddr for next image	
    }
	return 0;

 ret_upload:	
  	fprintf(stderr, "%s\n", buffer);	
	return -1;	
}

int rtk_FirmwareUpgrade(char *upload_data, int upload_len)
{
	int head_offset=0 ;
	int isIncludeRoot=0;
	int		 len;
    int          locWrite;
    int          numLeft;
    int          numWrite;
    IMG_HEADER_Tp pHeader;
	int flag=0, startAddr=-1, startAddrWeb=-1;	
    int fh;
   	unsigned char buffer[200];
   	firm_upgrade_status = 0;
   	int webOffset = -1,weblen,rootOffset = -1,rootlen,linuxOffset = -1,linuxlen;
	int webhead,roothead,linuxhead;

	//printf("[%s]:%d\n",__func__,__LINE__);

	int fwSizeLimit = 0x800000;

	/* do interface down */
	rtk_all_interface(0);
	
	
while(head_offset <   upload_len) {
    locWrite = 0;
    pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
    len = pHeader->len;
    numLeft = len + sizeof(IMG_HEADER_T) ;

    
    // check header and checksum
    if (!memcmp(&upload_data[head_offset], FW_HEADER, SIGNATURE_LEN) ||
			!memcmp(&upload_data[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN))
    	flag = 1;
    else if (!memcmp(&upload_data[head_offset], WEB_HEADER, SIGNATURE_LEN))
    	flag = 2;
    else if (!memcmp(&upload_data[head_offset], ROOT_HEADER, SIGNATURE_LEN)){
    	flag = 3;
    	isIncludeRoot = 1;
    }	
#if 0		
	else if ( !memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) )	 {
		int type, status, cfg_len;
		cfg_len = updateConfigIntoFlash(&upload_data[head_offset], 0, &type, &status);
		
		if (status == 0 || type == 0) { // checksum error
			strcpy(buffer, "Invalid configuration file!");
			goto ret_upload;
		}
		else { // upload success
			strcpy(buffer, "Update successfully!");
			head_offset += cfg_len;
			update_cfg = 1;
		}    	
		continue;
	}
#endif	
    else {
       	strcpy(buffer, "Invalid file format!");
    	//winfred_wang
    	firm_upgrade_status |= SIGNATURE_ERROR;
		goto ret_upload;
    }

	/*do checksum in rtk_FirmwareCheck */
	#if 0
       if(len > fwSizeLimit){ //len check by sc_yang 
      		sprintf(buffer, "Image len exceed max size 0x%x ! len=0x%x",fwSizeLimit, len);
		goto ret_upload;
    }
    if ( (flag == 1) || (flag == 3)) {
    	if ( !fwChecksumOk(&upload_data[sizeof(IMG_HEADER_T)+head_offset], len)) {
      		sprintf(buffer, "Image checksum mismatched! len=0x%x, checksum=0x%x", len,
			*((unsigned short *)&upload_data[len-2]) );
			if(flag == 1)
				firm_upgrade_status |= LINUXCHECKSUM_ERROR;
			else if(flag == 3)
				firm_upgrade_status |= ROOTCHECKSUM_ERROR;
				
		goto ret_upload;
	}
    }
    else {
    	char *ptr = &upload_data[sizeof(IMG_HEADER_T)+head_offset];
    	if ( !CHECKSUM_OK(ptr, len) ) {
     		sprintf(buffer, "Image checksum mismatched! len=0x%x", len);
     		firm_upgrade_status |= WEBCHECKSUM_ERROR;
		goto ret_upload;
	}
    }
	#endif


    if(flag == 3)
    	fh = open(FLASH_DEVICE_ROOTFS, O_RDWR);
    else
    fh = open(FLASH_DEVICE_KERNEL, O_RDWR);

    if ( fh == -1 ) {
       	strcpy(buffer, "File open failed!");
		goto ret_upload;
    } else {

	if (flag == 1) {
		if ( startAddr == -1){
			//startAddr = CODE_IMAGE_OFFSET;
			startAddr = pHeader->burnAddr ;
		}
		linuxOffset = startAddr;
	}
	else if (flag == 3) {
		if ( startAddr == -1){
			startAddr = 0; // always start from offset 0 for 2nd FLASH partition
		}
		rootOffset = startAddr;
	}
	else {
		if ( startAddrWeb == -1){
			//startAddr = WEB_PAGE_OFFSET;
			startAddr = pHeader->burnAddr ;
		}
		else
			startAddr = startAddrWeb;

		webOffset = startAddr;
	}
	lseek(fh, startAddr, SEEK_SET);
	if(flag == 3){
		fprintf(stderr,"\r\n close all interface");		
		locWrite += sizeof(IMG_HEADER_T); // remove header
		numLeft -=  sizeof(IMG_HEADER_T);
		//kill_processes();
		//sleep(2);
	}	
	//fprintf(stderr,"\r\n flash write");	

	//winfred_wang
	if(flag == 1){
		linuxlen = numLeft;
		linuxhead = locWrite+head_offset;
	}else if(flag == 2){
		weblen  = numLeft;
		webhead = locWrite+head_offset;
	}else if(flag == 3){
		rootlen = numLeft;
		roothead = locWrite+head_offset;
	}
	
	numWrite = write(fh, &(upload_data[locWrite+head_offset]), numLeft);
	//numWrite = numLeft;
	/*sprintf(buffer,"write flash flag=%d,locWrite+head_offset=%d,numLeft=%d,startAddr=%x\n",
				flag,locWrite+head_offset,numLeft,startAddr);
	fprintf(stderr, "%s\n", buffer);
	hex_dump(&(upload_data[locWrite+head_offset]),16);*/

	if (numWrite < numLeft) {

		sprintf(buffer, "File write failed. locWrite=%d numLeft=%d numWrite=%d Size=%d bytes.", locWrite, numLeft, numWrite, upload_len);

	goto ret_upload;
	}

	locWrite += numWrite;
 	numLeft -= numWrite;
	sync();
#ifdef KERNEL_3_10
	if(ioctl(fh,BLKFLSBUF,NULL) < 0){
		printf("flush mtd system cache error\n");
	}
#endif
	close(fh);

	head_offset += len + sizeof(IMG_HEADER_T) ;
	startAddr = -1 ; //by sc_yang to reset the startAddr for next image	
    }
} //while //sc_yang 
	//winfred_wang 
	rtk_check_flash_mem(linuxOffset,linuxlen,webOffset,weblen,rootOffset,rootlen,upload_data,webhead,roothead,linuxhead);

	/* ifconfig interface up */
	rtk_all_interface(1);
	
  return 0;
  ret_upload:	
  	fprintf(stderr, "%s\n", buffer);	

	rtk_all_interface(1);
  	
	return -1;
}

static int _firm_upgrade(char *cmd , int cmd_len)
{
	int fd,ret=0;
	unsigned char cmd2[2];
	ret=rtk_FirmwareCheck(cmd,cmd_len);
	if(ret < 0 ) {
		/*let do_cmd issue failed rsp*/
		return ret;
	}
	else
	{
		ret=MAX_INBAND_PAYLOAD_LEN+1;
		memset(cmd2,0x0,sizeof(cmd2));
		inband_write(hcd_inband_chan,0,id_firm_upgrade,cmd2,1,1);
	}

	//winfred_wang
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
	rgb_led_blink_setTriggerMode(FLASH_TRIGGER);
#endif
		
	//printf("[%s]:%d\n",__func__,__LINE__);
	rtk_FirmwareUpgrade(cmd,cmd_len);		

	//winfred_wang
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
	rgb_led_blink_setTriggerMode(TIMER_TRIGGER);
#endif

	//autoreboot;
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
	ret = rgb_led_blink_reboot_check();
	if(!ret)
		system("reboot");
#else
	system("reboot");
#endif

	return ret;
}

static int _firm_check_signature_checksum(char *cmd , int cmd_len)
{
	char tmpBuf[128] = {0};
	int len = 0;
	//printf("[%s]:%d\n",__func__,__LINE__);

	//printf("firm_upgrade_status=%x\n",firm_upgrade_status);
	
	if((firm_upgrade_status & 0xff) != 0)
	{
		if(firm_upgrade_status & SIGNATURE_ERROR)
			len += snprintf(tmpBuf+len,128 -len,"signature error\n");
		if(firm_upgrade_status & WEBCHECKSUM_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"web checksum error\n");
		if(firm_upgrade_status & ROOTCHECKSUM_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"root checksum error\n");
		if(firm_upgrade_status & LINUXCHECKSUM_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"linux checksum error\n");
	}else{
		len += snprintf(tmpBuf+len,128-len,"firmware signature checksum correct\n");
	}

	

	memcpy(cmd,tmpBuf,len);
	return len;
}


static int _firm_check_flash_data(char *cmd , int cmd_len)
{
	char tmpBuf[128] = {0};
	int len = 0;
	//printf("[%s]:%d\n",__func__,__LINE__);
	
	if((firm_upgrade_status & 0xff00) != 0)
	{
		if(firm_upgrade_status & WEBDATA_ERROR)
			len += snprintf(tmpBuf+len,128 -len,"web data error\n");
		if(firm_upgrade_status & LINUXDATA_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"linux data error\n");
		if(firm_upgrade_status & ROOTDATA_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"root data error\n");
		if(firm_upgrade_status & FLASHDATA_ERROR)
			len += snprintf(tmpBuf+len,128 - len,"other error\n");
	}else{
		len += snprintf(tmpBuf+len,128-len,"firmware data correct\n");
	}

	

	memcpy(cmd,tmpBuf,len);
	return len;
}

static int _firm_upgrade_reboot(char *cmd , int cmd_len)
{
	//printf("[%s]:%d\n",__func__,__LINE__);
	system("reboot");
	return 0;
}

#endif


static int host_ioctl_receive(char *ioctl_data_p,int ioctl_data_len)
{
	int rx_len=0;
	int ret=-1;
	struct iwreq iwr;
	struct ifreq ifr;
	struct iw_point *data;
	int ioctl_op=0;
	int error_code=0;
	int data_len=0;
	char *data_ptr;
	int opt_len=0;	
	unsigned char *is_iwreq;
	unsigned char *use_pointer;
	//int reply=1; //good reply
			 

	 //rcv ioctl succefully
	 memcpy(&ioctl_op,ioctl_data_p,sizeof(int));

	 is_iwreq = ioctl_data_p+HDR_INFO_OFFSET;
	 use_pointer = is_iwreq + 1;

	 //printf("%s print_len=%d\n",__FUNCTION__,ioctl_data_len);
	 //hex_dump(ioctl_data_p, ioctl_data_len);

	 if( !(*is_iwreq) )//use ifreq
	 {
		//printf("ifreq op_code = %x \n",ioctl_op);
		data_ptr = (char *)&ifr;
		data_len = sizeof(struct ifreq);
			   memcpy(data_ptr,ioctl_data_p+HDR_IOCTL_DATA_OFFSET,data_len);
	 }	 
	 else
	 {	
		//printf("iwreq op_code = %x \n",ioctl_op);
		data_ptr = (char *)&iwr;
		data_len = sizeof(struct iwreq);
		memcpy(data_ptr,ioctl_data_p+HDR_IOCTL_DATA_OFFSET,data_len);
		
		//check iwreq data pointer
		if( *use_pointer )
		{
			  iwr.u.data.pointer = (caddr_t) (ioctl_data_p +HDR_IOCTL_DATA_OFFSET+data_len);
			  //printf("use data pointer ioh_obj_p->rx_data=%x, iwr.u.data.pointer=%x , iwr.u.data.length=%d!!!\n",ioctl_data_p,iwr.u.data.pointer,iwr.u.data.length);
			  opt_len = iwr.u.data.length;
		}		

		if( ioctl_op == 0xffffffff ){
			int n=0;
			struct ifreq ifr;
			struct sockaddr_ll addr;
			int ifindex;
			int sock=-1;

			sock = socket(PF_PACKET, SOCK_RAW, 0);
			if (socket < 0) {
				perror("socket[PF_PACKET,SOCK_RAW]");
				return -1;
			}	
			memset(&ifr, 0, sizeof(ifr));
			strncpy(ifr.ifr_name, "wlan0", sizeof(ifr.ifr_name));

			while (ioctl(ioctl_sock , SIOCGIFINDEX, &ifr) != 0) {
				printf(" ioctl(SIOCGIFINDEX) failed!\n");
				sleep(1);
			}
			ifindex = ifr.ifr_ifindex;	
			memset(&addr, 0, sizeof(addr));
			addr.sll_family = AF_PACKET;
			addr.sll_ifindex = ifindex;
			if (bind(sock, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
				perror("bind[PACKET]");
				return -1;
			}
			
		 	//send wlan
		 	if ((n = send(sock, iwr.u.data.pointer, iwr.u.data.length, 0)) < 0) {
				printf("send_wlan failed!");
				return -1;
			}

			close(sock);
			return n;
		 }
	 }

	 if( ioctl_sock < 0 ) {
	 	ioctl_sock = socket(PF_INET, SOCK_DGRAM, 0);
		if (ioctl_sock < 0) {
			printf("\nioctl socket open failed !!!!!!!!!!!!\n");
			perror("socket[PF_INET,SOCK_DGRAM]");
			return -1;
		}
	}

	 if (ioctl(ioctl_sock, ioctl_op, data_ptr) < 0) {
		printf("\nioctl fail  ioctl_op=%x!!!!!!!!!!!!\n",ioctl_op);
		error_code = -1;
		//reply =2 ; //bad reply
		ret = 2;
	}

	//send reply		
	//copy erro code to reply (now in rx_data)
	if( *use_pointer ) {
		//printf("%s %d iwr.u.data.length:%d\n",__FUNCTION__,__LINE__,iwr.u.data.length);
		memcpy(ioctl_data_p,(char *)&error_code,sizeof(int));	
		memcpy(ioctl_data_p+HDR_IOCTL_DATA_OFFSET+sizeof(iwr),iwr.u.data.pointer,iwr.u.data.length);	//mark_test 		ret = iwr.u.data.length;
		ioctl_data_len += iwr.u.data.length;
	} else {
		memcpy(ioctl_data_p,(char *)&error_code,sizeof(int));
		memcpy(ioctl_data_p+HDR_IOCTL_DATA_OFFSET,data_ptr,data_len);	//mark_test	}

	//return ret;
	/*
	 if( hcd_inband_ioctl_chan < 0 ) {
	 	hcd_inband_ioctl_chan = inband_open(LOCAL_NETIF,NULL,ETH_P_RTK_NOTIFY,0);

		if(hcd_inband_ioctl_chan < 0)
		{
		       printf(" inband_open failed!\n");
		        return -1;
		}
	}*/
	//inband_write(hcd_inband_ioctl_chan,0,CMD_INBAND_IOCTL,ioctl_data_p,ioctl_data_len,reply);   
	//inband_write(hcd_inband_ioctl_chan,0,CMD_INBAND_IOCTL,ioctl_data_p,ioctl_data_len,ret);
	inband_write(hcd_inband_chan,0,CMD_INBAND_IOCTL,ioctl_data_p,ioctl_data_len,ret); //good reply
	//inband_write(hcd_inband_ioctl_chan,0,CMD_INBAND_IOCTL,ioctl_data_p,ioctl_data_len,ret);
	return ret;
}

static int host_systemcall_receive(char *cmd , int cmd_len) //mark_cmd
{
	char *param;		
	char *tmp;
	FILE *fp;
	int resp_len=0;
	char res[64*200]; //mark_issue
	char tmp_cmd[cmd_len+1];
	int retry_count=0;
	
	cmd[cmd_len]='\0';//mark_patch
	memset(res,0,sizeof(res));
	if(strstr(cmd,"init.sh") || strstr(cmd,"reboot"))
	{
		strcpy(tmp_cmd,cmd);
		res[0] = '\0';
		inband_write(hcd_inband_chan,0,id_syscall,res,1,1); //good reply
		sleep(1);
		system(tmp_cmd);
		return MAX_INBAND_PAYLOAD_LEN+1;
	}
	else if(strstr(cmd,"sysconf"))
	{
		strcpy(tmp_cmd,cmd);
		res[0] = '\0';
		inband_write(hcd_inband_chan,0,id_syscall,res,1,1); //good reply
		system(tmp_cmd);
		return MAX_INBAND_PAYLOAD_LEN+1;
	}
RETRY_SYSTEM_CALL:	
    //printf("system call = %s\n",data);
	fp = popen(cmd, "r");
	if (fp)
	{
		while (!feof(fp)&&resp_len+MAX_INBAND_PAYLOAD_LEN<sizeof(res)) 
		{
			// 4 bytes for FCS
			//if (resp_len + sizeof(*obj->rx_header) + 4 == sizeof(obj->rx_buffer))
			//break;	// out of buffer

			resp_len += fread(	&res[resp_len],
							sizeof(char), 
							MAX_INBAND_PAYLOAD_LEN, 
							fp);
		}
	}
	else
	{
		printf("popen error!!!\n");
		return -1; //error reply in do_cmd
	}
	/*retry more for rebust inband cmd execution*/
	if(resp_len <= 0){
		if (fp)
		{
			pclose(fp);
		}
		if(retry_count++ < 3){
			//system call fail, do cmd again
			printf("host_systemcall_receive: len=%d retry system call\n",resp_len);
			goto RETRY_SYSTEM_CALL;
		}
		else
		{
			return -1;
		}
	}	
	//syscmd reply here , not in do_cmd bcz some reply is very long (site_survey!)
	inband_write(hcd_inband_chan,0,id_syscall,res,resp_len,1); 
	pclose(fp);
	//mark_issue , always syscmd reply here so , ret set to MAX_INBAND_PAYLOAD_LEN +1
	return MAX_INBAND_PAYLOAD_LEN+1;
}
static int _request_scan(char *cmd , int cmd_len)
{
	int status;
	char errbuf[100];
	cmd[cmd_len] = '\0';
	if(getWlSiteSurveyRequest(cmd, &status) < 0)
		sprintf(errbuf,"Scan request failed!\n");
	else
		sprintf(errbuf,"Auto Scan running!\n");
	printf("%s",errbuf);
	inband_write(hcd_inband_chan,0,id_request_scan,errbuf,strlen(errbuf)+1,1); //reply
	return MAX_INBAND_PAYLOAD_LEN+1;

}
static int _get_scan_result(char *cmd , int cmd_len)
{	
  char errBuf[100]; //per bssinfo or error msg  
  char tmpbuf[100],tmpbuf2[50]; //per bssinfo or error msg
  char ssr_result[MAX_BSS_DESC*200]; //mark_issue , use alloc ?
  SS_STATUS_Tp pStatus=NULL;
  BssDscr *pBss;
  int i,wait_time =0;
  cmd[cmd_len] = '\0';
  if (pStatus==NULL) {
		pStatus = calloc(1, sizeof(SS_STATUS_T));
		if ( pStatus == NULL ) {
			strcpy(errBuf, "Allocate buffer failed!\n");
			goto ssr_err_out;
		}
  }

  pStatus->number = 0; // request BSS DB
  while(getWlSiteSurveyResult(cmd, pStatus) < 0){
	  if ( wait_time++ < 2 ) {
	  		sleep(1);
	  }
	  else{
	  	strcpy(errBuf, "Read site-survey status failed!\n");
			goto ssr_err;
	  }
  }

  if(pStatus->number<0 || pStatus->number > MAX_BSS_DESC)
  {
  	strcpy(errBuf, "invalid scanned ap num!\n");
  	goto ssr_err;
  }	

  sprintf(ssr_result,"total_ap_num=%d\n",pStatus->number);  

  for (i=0; i<pStatus->number && pStatus->number!=0xff; i++) {

		//add ##########################
		strcat(ssr_result,"##########\n");

		pBss = &pStatus->bssdb[i];		
		// fill ap index
		sprintf(tmpbuf,"ap_index=%d\n",i+1);
		strcat(ssr_result,tmpbuf);

		// fill ap ssid
   		
		memcpy(tmpbuf2, pBss->bdSsIdBuf, pBss->bdSsId.Length);
		tmpbuf2[pBss->bdSsId.Length] = '\0';
		
		sprintf(tmpbuf,"ap_ssid=%s\n",tmpbuf2);		
		strcat(ssr_result,tmpbuf);
   
		// fill ap mac
		sprintf(tmpbuf,"ap_bssid=%02x:%02x:%02x:%02x:%02x:%02x\n",
			pBss->bdBssId[0], pBss->bdBssId[1], pBss->bdBssId[2],
			pBss->bdBssId[3], pBss->bdBssId[4], pBss->bdBssId[5]);
		strcat(ssr_result,tmpbuf);
		
		// fill ap mode B,G,N
		get_ap_mode(pBss->network,tmpbuf2);
		sprintf(tmpbuf,"ap_mode=%s\n",tmpbuf2);		
		strcat(ssr_result,tmpbuf);
	
		// fill ap channel
		sprintf(tmpbuf,"ap_channel=%d\n",pBss->ChannelNumber);
		strcat(ssr_result,tmpbuf);
	
		// fill ap encryption type
		get_ap_encrypt(pBss,tmpbuf2);
		sprintf(tmpbuf,"ap_encrypt=%s\n",tmpbuf2);		
		strcat(ssr_result,tmpbuf);
	
		// fill ap signal strength
		sprintf(tmpbuf,"ap_signal=%d\n",pBss->rssi);
		strcat(ssr_result,tmpbuf);

  }		

 	
  	free(pStatus); 
	inband_write(hcd_inband_chan,0,id_get_scan_result,ssr_result,strlen(ssr_result)+1,1); 
	return MAX_INBAND_PAYLOAD_LEN+1;
ssr_err :
	free(pStatus); 
ssr_err_out :
	printf("%s",errBuf);
	inband_write(hcd_inband_chan,0,id_get_scan_result,errBuf,strlen(errBuf),1);
	return -1;


}
#if defined(CONFIG_RTL_LITE_CLNT)
int set_wlan_onoff(char *wlanif,int onoff)
{
	char cmd_buf[100];
	char wlan_vxd[16] = {0};
	char value[10] = {0};
	int rptenabled = 0;
	if(!wlanif)
		return -1;
	if(!strcmp(wlanif,"wlan0"))
		get_mib_value(2,0,&value,"REPEATER_ENABLED1",0);
	else if(!strcmp(wlanif,"wlan1"))
		get_mib_value(2,0,&value,"REPEATER_ENABLED2",0);
	if(value[0])
		rptenabled = atoi(value);
	
	if(rptenabled){
		strcpy(wlan_vxd,wlanif);
		strcat(wlan_vxd,"-vxd");
	}
	
	sprintf(cmd_buf,"flash set %s FUNC_OFF %d",wlanif,onoff);
	system(cmd_buf);
	if(rptenabled){
		sprintf(cmd_buf,"ifconfig %s down",wlan_vxd);
		system(cmd_buf);
	}
	sprintf(cmd_buf,"ifconfig %s down",wlanif);
	system(cmd_buf);
	sprintf(cmd_buf,"iwpriv %s set_mib func_off=%d",wlanif,onoff);
	system(cmd_buf);
	sprintf(cmd_buf,"ifconfig %s up",wlanif);
	system(cmd_buf);
	if(rptenabled){
		sprintf(cmd_buf,"ifconfig %s up",wlan_vxd);
		system(cmd_buf);
	}
	return 0;
	
}

static int 	_set_wlan_on(char *cmd , int cmd_len)
{
		char errbuf[100];
		cmd[cmd_len] = '\0';
		if(strncmp("wlan",cmd,4)){
			sprintf(errbuf,"Invild wlan interface!\n");
			goto err_end;
		}
		if(set_wlan_onoff(cmd,0) < 0)
			sprintf(errbuf,"Set %s function on fail\n",cmd);
		else
			sprintf(errbuf,"Set %s function on successful\n",cmd);
err_end:
		printf("%s",errbuf);
		inband_write(hcd_inband_chan,0,id_set_wlan_on,errbuf,strlen(errbuf)+1,1); //reply
		return MAX_INBAND_PAYLOAD_LEN+1;

}
static int 	_set_wlan_off(char *cmd , int cmd_len)
{
		char errbuf[100];
		cmd[cmd_len] = '\0';
		if(strncmp("wlan",cmd,4)){
			sprintf(errbuf,"Invild wlan interface!\n");
			goto err_end;
		}
		if(set_wlan_onoff(cmd,1) < 0)
			sprintf(errbuf,"Set %s function off fail\n",cmd);
		else
			sprintf(errbuf,"Set %s function off successful\n",cmd);
err_end:
		printf("%s",errbuf);
		inband_write(hcd_inband_chan,0,id_set_wlan_off,errbuf,strlen(errbuf)+1,1); //reply
		return MAX_INBAND_PAYLOAD_LEN+1;

}

int get_encrypt_type(int encrypt,int passwd_len)
{
	int type;
	switch(encrypt){
		case 0:
			type = 0;
		break;
		case 1:
			if(passwd_len == 5)
				type = 1;
			else if(passwd_len == 10)
				type = 2;
			else if (passwd_len == 13)
				type = 3;
			else if(passwd_len == 26)
				type = 4;
			else
				type = -1;
		break;
		case 4:
			type = 9;
		break;
		case 2:
			type = 10;
		break;
		case 6:
			type = 11;
		break;
		default:
			type = -1;
		break;
	}
	return type;
}

static int set_profile_to_flash(char *wlan_if, char *ssid,char *passwd,int encrypt_type)
{
	unsigned char buffer[128]={0};
	unsigned char value[128];
	char config_prefix[16];
	//unsigned char tmp[128];
	unsigned char *p;
	//unsigned int security_type=0;
	//unsigned int wsc_auth=0, wsc_enc=0;
	//unsigned char wsc_psk[65] = {0};

	system("flash setconf start");
	if(!strcmp(wlan_if,"wlan0"))
		sprintf(config_prefix,"WLAN0_");
	else if(!strcmp(wlan_if,"wlan0-vxd"))
		sprintf(config_prefix,"WLAN0_VXD_");
	else if(!strcmp(wlan_if,"wlan1"))
		sprintf(config_prefix,"WLAN1_");
	else if(!strcmp(wlan_if,"wlan1-vxd"))
		sprintf(config_prefix,"WLAN1_VXD_");

	sprintf(buffer, "flash setconf %sSSID %s", config_prefix,ssid);
	system(buffer);	
	switch(encrypt_type)
	{
		case 0:
			sprintf(buffer, "flash setconf %sENCRYPT 0", config_prefix);
			system(buffer);
			break;
		case 1:
			sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP 1", config_prefix);
			system(buffer);
			sprintf(value, "%02x%02x%02x%02x%02x", passwd[0], passwd[1], passwd[2], passwd[3],passwd[4]);
			value[10] = 0;
			sprintf(buffer, "flash setconf %sWEP64_KEY1  %s", config_prefix, value);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 0", config_prefix);
			system(buffer);			
			sprintf(buffer, "flash setconf %sAUTH_TYPE  2", config_prefix);
			system(buffer);
			break;
		case 2:
			sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP64_KEY1	%s", config_prefix, passwd);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 1", config_prefix);
			system(buffer); 		
			sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
			system(buffer); 		
			break;
		case 3:
			sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP 2", config_prefix);
			system(buffer);
			sprintf(value, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
				passwd[0], passwd[1], passwd[2], passwd[3], passwd[4], passwd[5], passwd[6],
				passwd[7], passwd[8], passwd[9], passwd[10], passwd[11], passwd[12]
				);
			value[26] = 0;
			sprintf(buffer, "flash setconf %sWEP128_KEY1 %s", config_prefix, value);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 0", config_prefix);
			system(buffer); 		
			sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
			system(buffer); 		
			break;
		case 4:
			sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP128_KEY1 %s", config_prefix, passwd);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 1", config_prefix);
			system(buffer); 		
			sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
			system(buffer); 		
			break;
		case 5:
			sprintf(buffer, "flash setconf %sENCRYPT 4", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
			system(buffer);

			break;
		case 6:
			sprintf(buffer, "flash setconf %sENCRYPT 4", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
			system(buffer);
			break;
		case 7:
			sprintf(buffer, "flash setconf %sENCRYPT 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
			system(buffer);
			break;
		case 8:
			sprintf(buffer, "flash setconf %sENCRYPT 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
			system(buffer);
		
			break;
		case 9:
			sprintf(buffer, "flash setconf %sENCRYPT 4", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 3", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
			system(buffer);
			break;
		case 10:
			sprintf(buffer, "flash setconf %sENCRYPT 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 3", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
			system(buffer);
		
			break;
		case 11:
			sprintf(buffer, "flash setconf %sENCRYPT 6", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 3", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 3", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK %s", config_prefix, passwd);
			system(buffer);
		default:
			sprintf(buffer, "flash setconf %sENCRYPT 0", config_prefix);
			system(buffer);
			break;
	}
	
	//sprintf(buffer, "flash setconf %sSC_SAVE_PROFILE 2", config_prefix);
	//system(buffer);
	//pCtx->sc_save_profile = 2;
	
	system("flash setconf end");
	return 1;
	
}

static int 	_wlan_sync(char *cmd , int cmd_len)
{
		int intValue,passwd_len,encrypt_type,encrypt,status,idx,vidx = 0;
		char errbuf[100],value[65],cmd_buf[256],ssid[65],passwd[65];
		char wlan_root[16] = {0};
		cmd[cmd_len] = '\0';
		if(strncmp("wlan",cmd,4)){
			sprintf(errbuf,"Invild wlan interface!\n");
			goto err_end;
		}
		idx = cmd[4] - '0';
		if(strstr(cmd,"vxd")){
			vidx = -1;
		}
		if(strstr(cmd,"vxd"))
			snprintf(wlan_root,6,"%s",cmd);
		get_mib_value(idx,vidx,&value,"LITE_CLNT_PASSWD",1);
		passwd_len = strlen(value);
		strcpy(passwd,value);
		get_mib_value(idx,vidx,&value,"ENCRYPT",0);
		encrypt = atoi(value);
		get_mib_value(idx,vidx,&ssid,"SSID",1);
		
		//encrypt_type = get_encrypt_type_by_ssid(idx,ssid,passwd_len);
		encrypt_type = get_encrypt_type(encrypt,passwd_len);
		printf("%s -->%d,encrypt_type = %d,passwd_len = %d\n",__FUNCTION__,__LINE__,encrypt_type,passwd_len);
		if(encrypt_type < 0){
			sprintf(errbuf,"Encrypt type wrone!\n");
			goto err_end;
		}
		set_profile_to_flash(cmd,ssid,passwd,encrypt_type);
		if(wlan_root[0]){
			get_mib_value(2,0,&value,"LITE_CLNT_SYNC_VXD",0);
			intValue = atoi(value);
			if(intValue)
				set_profile_to_flash(wlan_root,ssid,passwd,encrypt_type);
		}
		if(idx == 0)
			get_mib_value(2,0,&value,"PROFILE_ENABLED1",0);
		else
			get_mib_value(2,0,&value,"PROFILE_ENABLED2",0);
		intValue = atoi(value);
		if(intValue){
			sprintf(cmd_buf,"sysconf wlprofile add %d",idx);
			system(cmd_buf);
		}
		sprintf(errbuf,"Wlan sync complete!\n");
err_end:
		printf("%s",errbuf);
		inband_write(hcd_inband_chan,0,id_wlan_sync,errbuf,strlen(errbuf)+1,1); //reply
		return MAX_INBAND_PAYLOAD_LEN+1;

}


static int 	_start_lite_clnt_connect(char *cmd , int cmd_len)
{
		int intValue,passwd_len,encrypt_type,encrypt,status,idx,vidx = 0;
		char errbuf[100],value[65],cmd_buf[256],ssid[65],passwd[65];
		char wlan_root[16] = {0};
		cmd[cmd_len] = '\0';
		if(strncmp("wlan",cmd,4)){
			sprintf(errbuf,"Invild wlan interface!\n");
			goto err_end;
		}
		idx = cmd[4] - '0';
		if(strstr(cmd,"vxd")){
			vidx = -1;
		}

		get_mib_value(idx,vidx,&value,"LITE_CLNT_ENABLE",0);
		intValue = atoi(value);
		if(!intValue){
			sprintf(errbuf,"Lite Client mode is disabled!\n");
			goto err_end;
		}
	
		if(strstr(cmd,"vxd"))
			snprintf(wlan_root,6,"%s",cmd);
	
		sprintf(cmd_buf,"ifconfig %s down",cmd);
		system(cmd_buf);
		sprintf(cmd_buf,"iwpriv %s set_mib lite_clnt_enabled=%d",cmd,intValue);
		system(cmd_buf);
		
		get_mib_value(idx,vidx,&value,"SSID",1);
		sprintf(cmd_buf,"iwpriv %s set_mib ssid=\"%s\"",cmd,value);
		system(cmd_buf);
		get_mib_value(idx,vidx,&value,"LITE_CLNT_PASSWD",1);
		sprintf(cmd_buf,"iwpriv %s set_mib lite_clnt_passwd=\"%s\"",cmd,value);
		system(cmd_buf);
		if(wlan_root[0]){
			sprintf(cmd_buf,"ifconfig %s down",wlan_root);
			system(cmd_buf);
			sprintf(cmd_buf,"ifconfig %s up",wlan_root);
			system(cmd_buf);
		}
		sprintf(cmd_buf,"ifconfig %s up",cmd);
		system(cmd_buf);
		sprintf(errbuf,"Wlan client is connecting!\n");
err_end:
		printf("%s",errbuf);
		inband_write(hcd_inband_chan,0,id_start_lite_clnt_connect,errbuf,strlen(errbuf)+1,1); //reply
		return MAX_INBAND_PAYLOAD_LEN+1;

}
#endif

#if defined(CONFIG_APP_ADAPTER_API)
static int _get_storage_info(char *cmd , int cmd_len)
{	
	char errbuff[128]; //error msg  
	RTK_DEVICEINFO_T *info = NULL;
	int number = 0, ret = 0, length = 0;
	cmd[cmd_len] = '\0';

	memset(errbuff, '\0', sizeof(errbuff));

	info = malloc(MAX_DEVICE_NUMBER*sizeof(RTK_DEVICEINFO_T));	
	if(info != NULL)
	{	
		memset((void *)info, '\0', (MAX_DEVICE_NUMBER*sizeof(RTK_DEVICEINFO_T)));
		ret = rtk_get_storage_info(&number, info, MAX_DEVICE_NUMBER);
		if (ret == 0)
		{
			strcpy(errbuff, "rtk_get_storage_info return failed\n");
			goto ssr_err_out;
		}

		if (number > MAX_DEVICE_NUMBER)
		{
			sprintf(errbuff, " usb number greater than the max %d \n", MAX_DEVICE_NUMBER);
			goto ssr_err_out;
		}
	}
	else
	{		
		sprintf(errbuff,"malloc fail,no free memory\n");		
		goto ssr_err_out;
	}


	length = number*sizeof(RTK_DEVICEINFO_T);
	inband_write(hcd_inband_chan, 0, id_get_storage_info, (char *)info, length,1); 
	free(info);
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	if(info!=NULL)		
		free(info);
	inband_write(hcd_inband_chan, 0, id_get_storage_info, errbuff, strlen(errbuff), 2);
	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
}

static int _format_partition(char *cmd , int cmd_len)
{	
	char errbuff[128] = {0}, rspbuff[128] = {0}; //error msg 
	unsigned char partition_name[16] = {0}, systype[16] = {0}; 
	int ret = 0, length = 0;
	
	cmd[cmd_len] = '\0';
	memset(errbuff, '\0', sizeof(errbuff));
	memset(rspbuff, '\0', sizeof(rspbuff));
	memset(partition_name, '\0', sizeof(partition_name));
	memset(systype, '\0', sizeof(systype));
	
	sscanf(cmd, "%s %s", partition_name, systype);
	//printf("%s %d partition_name=%s systype=%s \n", __FUNCTION__, __LINE__, partition_name, systype);
	if (!partition_name[0])
	{
		strcpy(errbuff, " partition name is NULL\n");
		goto ssr_err_out;
	}
	if (!systype[0])
	{
		strcpy(errbuff, " file system type is NULL\n");
		goto ssr_err_out;
	}
	ret = rtk_disk_format(partition_name, systype);
	if (ret == 0)
	{
		strcpy(errbuff, " rtk_disk_format return failed\n");
		goto ssr_err_out;
	}
	sprintf(rspbuff, " format success! \n");
	length = strlen(rspbuff);
	//printf("%s %d rspbuff=%s length=%d \n", __FUNCTION__, __LINE__, rspbuff, length);
	inband_write(hcd_inband_chan, 0, id_format_partition, rspbuff, length,1); 
	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_format_partition, errbuff, strlen(errbuff), 2);
	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
}

static int _get_wan_status_By_Url(char *cmd , int cmd_len)
{
	int ret,length;
	int connected;
	unsigned char *url;
	unsigned char errbuff[48] = {0};
	cmd[cmd_len]='\0';//mark_patch
	ret=  rtk_get_wan_status_by_url(&connected,cmd);
	if (ret == 0)
	{
		strcpy(errbuff, "error,rtk_get_wan_status_by_url failed\n");
		goto ssr_err_out;
	}
	
	length = sizeof(connected);	
	inband_write(hcd_inband_chan, 0, id_get_wan_status, (char *)&connected, length,1);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_get_wan_status, errbuff, strlen(errbuff), 2);
	
	return (MAX_INBAND_PAYLOAD_LEN + 1);	
}

static int _get_lan_terminal_info(char *cmd , int cmd_len)
{
	int number = 0;
	char errbuff[128] = {0}; //error msg 
	int ret = 0, length = 0;
	struct rtk_link_type_info *rtk_info = NULL;
	rtk_info = malloc(MAX_TERM_NUMBER*sizeof(struct rtk_link_type_info));
	if(rtk_info != NULL)
	{
		memset(rtk_info,0,(MAX_TERM_NUMBER*sizeof(struct rtk_link_type_info)));
		ret = rtk_get_terminal_info(&number,rtk_info,MAX_TERM_NUMBER);
		if(ret ==0)
		{
			strcpy(errbuff, "error,rtk_get_terminal_info failed\n");
			goto ssr_err_out;
		}
	}
	else
	{
		printf("error.malloc fail,no free memory\n");		
		goto ssr_err_out;
	}
	
	length = number*sizeof(struct rtk_link_type_info);
	inband_write(hcd_inband_chan, 0, id_get_lan_terminal_info, (char *)rtk_info , length,1);
	free(rtk_info);
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	
	if(rtk_info != NULL)
		free(rtk_info);
	
	inband_write(hcd_inband_chan, 0, id_get_lan_terminal_info, errbuff, strlen(errbuff), 2);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);	
	
}
static int _get_upload_speed(char *cmd , int cmd_len)
{
	int ret ,length;
	char errbuff[128] = {0}; //error msg 
	RTK_UPLOAD_STATICS *rtk_info=NULL;
	rtk_info = malloc(sizeof(RTK_UPLOAD_STATICS));
	if(rtk_info != NULL)
	{
		memset(rtk_info,0,sizeof(RTK_UPLOAD_STATICS));
		ret = rtk_get_upload_statics(rtk_info);
		if(ret ==0)
		{
			strcpy(errbuff, "error,rtk_get_upload_statics failed\n");
			goto ssr_err_out;
		}	
	}
	else
	{
		printf("error.malloc fail,no free memory\n");		
		goto ssr_err_out;
	}
	
	length = sizeof(RTK_UPLOAD_STATICS);
	inband_write(hcd_inband_chan, 0, id_get_upload_speed, (char *)rtk_info, length,1);	
	free(rtk_info);
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	if(rtk_info != NULL)
		free(rtk_info);
	
	inband_write(hcd_inband_chan, 0, id_get_upload_speed, errbuff, strlen(errbuff), 2);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);		
}
static int _get_download_speed(char *cmd , int cmd_len)
{
	int ret ,length;
	char errbuff[128] = {0}; //error msg 
	RTK_DOWNLOAD_STATICS *rtk_info=NULL;
	rtk_info = malloc(sizeof(RTK_DOWNLOAD_STATICS));
	if(rtk_info != NULL)
	{
		memset(rtk_info,0,sizeof(RTK_DOWNLOAD_STATICS));
		ret = rtk_get_download_statics(rtk_info);
		if(ret ==0)
		{
			strcpy(errbuff, "error,rtk_get_download_statics failed\n");
			goto ssr_err_out;
		}	
	}
	else
	{
		printf("error.malloc fail,no free memory\n");		
		goto ssr_err_out;
	}
	
	length = sizeof(RTK_DOWNLOAD_STATICS);
	inband_write(hcd_inband_chan, 0, id_get_download_speed, (char *)rtk_info, length,1);	
	free(rtk_info);
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	if(rtk_info != NULL)
		free(rtk_info);
	
	inband_write(hcd_inband_chan, 0, id_get_download_speed, errbuff, strlen(errbuff), 2);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);	
	
}
static int _get_phy_port_status(char *cmd , int cmd_len)
{
	int number = 0;
	char errbuff[128] = {0}; //error msg 
	int ret = 0, length = 0;
	struct rtk_port_status *rtk_info = NULL;
	rtk_info = malloc(MAX_PORT_NUMBER*sizeof(struct rtk_port_status));
	
	if(rtk_info != NULL)
	{
		memset(rtk_info,0,MAX_PORT_NUMBER*sizeof(struct rtk_port_status));
		ret = rtk_get_ports_status(&number,rtk_info,MAX_PORT_NUMBER);
		if(ret ==0)
		{
			strcpy(errbuff, "error,rtk_get_ports_status failed\n");
			goto ssr_err_out;
		}
	}
	else
	{
		printf("error.malloc fail,no free memory\n");		
		goto ssr_err_out;
	}

	length = MAX_PORT_NUMBER*sizeof(struct rtk_port_status);
	inband_write(hcd_inband_chan, 0, id_get_phy_port_status, (char *)rtk_info , length,1);
	free(rtk_info);
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	if(rtk_info != NULL)
		free(rtk_info);
	
	inband_write(hcd_inband_chan, 0, id_get_phy_port_status, errbuff, strlen(errbuff), 2);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
}

static int _get_lan_drop_rate(char *cmd , int cmd_len)
{
	int ret,length;
	int drop_rate;
	unsigned char errbuff[48] = {0};

	ret=  rtk_get_lan_drop_rate(&drop_rate);
	if (ret == 0)
	{
		strcpy(errbuff, "error,rtk_get_lan_drop_rate failed\n");
		goto ssr_err_out;
	}
	
	length = sizeof(drop_rate);	
	
	//printf("%s.%d.return to client.drop_rate(%d),length(%d)\n",__FUNCTION__,__LINE__,drop_rate,length);
	inband_write(hcd_inband_chan, 0, id_get_lan_drop_rate, (char *)&drop_rate, length,1);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_get_lan_drop_rate, errbuff, strlen(errbuff), 2);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
}
static int _get_wan_drop_rate(char *cmd , int cmd_len)
{
	int ret,length;
	int drop_rate;
	unsigned char errbuff[48] = {0};

	ret=  rtk_get_wan_drop_rate(&drop_rate);
	if (ret == 0)
	{
		strcpy(errbuff, "error,rtk_get_wan_drop_rate failed\n");
		goto ssr_err_out;
	}
	
	length = sizeof(drop_rate); 	
	inband_write(hcd_inband_chan, 0, id_get_wan_drop_rate, (char *)&drop_rate, length,1);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_get_wan_drop_rate, errbuff, strlen(errbuff), 2);
	return (MAX_INBAND_PAYLOAD_LEN + 1);	

}
static int _get_wlan_drop_rate(char *cmd , int cmd_len)
{
	int ret,length;
	int drop_rate;
	unsigned char errbuff[48] = {0};

	ret=  rtk_get_wlan_drop_rate(&drop_rate,cmd);
	if (ret == 0)
	{
		strcpy(errbuff, "error,rtk_get_wlan_drop_rate failed\n");
		goto ssr_err_out;
	}
	
	length = sizeof(drop_rate); 	
	inband_write(hcd_inband_chan, 0, id_get_wlan_drop_rate, (char *)&drop_rate, length,1);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_get_wlan_drop_rate, errbuff, strlen(errbuff), 2);
	return (MAX_INBAND_PAYLOAD_LEN + 1);	

}
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)||defined (QOS_BY_BANDWIDTH)

static int string_to_hexmac(char *string, unsigned char *key, int len)
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
#endif
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
static int _get_qos_rule_monopoly(char *cmd , int cmd_len)
{
	int ret,length;
	RTK_QOSMNP_INFO rtk_info;
	unsigned char errbuff[48] = {0};

	ret=rtk_get_qos_rule_monopoly_info(&rtk_info);
	if (ret == 0)
	{
		strcpy(errbuff, "error,rtk_get_qos_rule_monopoly failed\n");
		goto ssr_err_out;
	}
	
	length = sizeof(rtk_info); 	
	inband_write(hcd_inband_chan, 0, id_get_qos_rule_monopoly, (char *)(&rtk_info), length,1);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_get_qos_rule_monopoly, errbuff, strlen(errbuff), 2);
	return (MAX_INBAND_PAYLOAD_LEN + 1);	

}

static int _set_qos_rule_monopoly(char *cmd, int cmd_len)
{

	char *tmpInfo = NULL;
	char *ptr;
	char		*strptr, *cmd_addr;
	char		*tokptr;
	int flag=0;
	int ret=0;
	int index=0;
	unsigned int enabled;
	unsigned char macAddr[6]={0};
	unsigned int qosTime;
	char errbuf[100];
	char cmd_buf[100]={0};
	cmd[cmd_len]='\0';
	//printf("cmd:%s,[%s]:[%d].\n",cmd,__FUNCTION__,__LINE__);


	strptr=cmd;
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	sscanf( tokptr, "%d", &enabled ); 
	//printf("tokptr:%s,%d[%s]:[%d].\n",tokptr,enabled,__FUNCTION__,__LINE__);
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	
	if (strlen(tokptr)!=12 || !string_to_hexmac(tokptr, macAddr, 12)) {
		goto out;
	}

	//printf("tokptr:%s,%x:%x:%x:%x:%x:%x [%s]:[%d].\n",tokptr,macAddr[0], macAddr[1], 
	//	macAddr[2], macAddr[3],macAddr[4],macAddr[5],__FUNCTION__,__LINE__);
		
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	
	sscanf(tokptr, "%d", &qosTime);
	//printf("tokptr:%s,%d[%s]:[%d].\n",tokptr,qosTime,__FUNCTION__,__LINE__);
	
	ret=rtk_set_qos_rule_monopoly(enabled, macAddr, qosTime);
out:	
	if(ret ==0)
		sprintf(errbuf,"Set rtk_set_qos_rule_monopoly fail\n");
	else
		sprintf(errbuf,"Set rtk_set_qos_rule_monopoly successful\n");
	
	printf("%s",errbuf);
	
	inband_write(hcd_inband_chan,0,id_set_qos_rule_monopoly,errbuf,strlen(errbuf)+1,1); //reply
	return MAX_INBAND_PAYLOAD_LEN+1;

	

	
}

static int _set_qos_rule_monopoly_imm(char *cmd, int cmd_len)
{

	char *tmpInfo = NULL;
	char *ptr;
	char		*strptr, *cmd_addr;
	char		*tokptr;
	int flag=0;
	int ret=0;
	int index=0;
	unsigned int enabled;
	unsigned char macAddr[6];
	unsigned int qosTime;
	char errbuf[100];
	char cmd_buf[100]={0};
	cmd[cmd_len]='\0';
	//printf("cmd:%s,[%s]:[%d].\n",cmd,__FUNCTION__,__LINE__);

	strptr=cmd;
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	sscanf( tokptr, "%d", &enabled ); 
	//printf("tokptr:%s,%d[%s]:[%d].\n",tokptr,enabled,__FUNCTION__,__LINE__);
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	if (strlen(tokptr)!=12 || !string_to_hexmac(tokptr, macAddr, 12)) {
		goto out;
	}
	//printf("tokptr:%s,%x:%x:%x:%x:%x:%x [%s]:[%d].\n",tokptr,macAddr[0], macAddr[1], 
	//	macAddr[2], macAddr[3],macAddr[4],macAddr[5],__FUNCTION__,__LINE__);
		
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	
	sscanf(tokptr, "%d", &qosTime);
	//printf("tokptr:%s,%d[%s]:[%d].\n",tokptr,qosTime,__FUNCTION__,__LINE__);
	
	ret=rtk_set_qos_rule_monopoly_immediately(enabled, macAddr, qosTime);
	
	
out:	
	if(ret ==0)
		sprintf(errbuf,"Set rtk_set_qos_rule_monopoly_imm fail\n");
	else
		sprintf(errbuf,"Set rtk_set_qos_rule_monopoly_imm successful\n");
	
	printf("%s",errbuf);
	inband_write(hcd_inband_chan,0,id_set_qos_rule_monopoly_imm,errbuf,strlen(errbuf)+1,1); //reply
	return MAX_INBAND_PAYLOAD_LEN+1;

	
}
#endif
#if defined (QOS_BY_BANDWIDTH)
static int _get_qos_rule(char *cmd , int cmd_len)
{
	int ret,length;
	int i=0;
	unsigned char *rtk_info=NULL;
	unsigned char errbuff[48] = {0};
	unsigned int num=1;
	unsigned int allFlag=0;
	RTK_IPQOS_T tmpentry;
	RTK_IPQOS_T entry[MAX_QOS_RULE_NUM]={0};
	char		*strptr, *cmd_addr;
	char		*tokptr;
	unsigned char macAddr[6]={0};
	int mode=0;
	unsigned int index=0;
	cmd[cmd_len]='\0';
	strptr=cmd;
	//printf("cmd:%s,[%s]:[%d].\n",cmd,__FUNCTION__,__LINE__);
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	if(strcmp(tokptr,"mac")==0)
	{
		mode= QOS_RESTRICT_MAC;
		
	}
	else if(strcmp(tokptr,"all")==0)
	{
		allFlag= 1;
		goto process;
	}	
	else
	{
		printf("not support mode!\n");
		goto out;
	}
	
	if(mode==QOS_RESTRICT_MAC)
	{
		tokptr = strsep(&strptr," ");
		
		if (tokptr==NULL)
		{
			goto out;
		}
		
		if (strlen(tokptr)!=12 || !string_to_hexmac(tokptr, macAddr, 12)) {
			printf("invalid mac address:%s\n", tokptr);
			goto out;
		}
	}
	
process:
	if(allFlag)
	{
		ret=rtk_get_qos_rule_entry_num(&num);
		if (ret == 0)
		{
			strcpy(errbuff, "error,rtk_get_qos_rule failed\n");
			goto out;
		}
		ret=get_qos_rule_entry(&num, &entry[0], num);
		if (ret == 0)
		{
			strcpy(errbuff, "error,rtk_get_qos_rule failed\n");
			goto out;
		}
	}
	else if(mode==QOS_RESTRICT_MAC)
	{
		memset(&tmpentry,0,sizeof(RTK_IPQOS_T));
		tmpentry.mode =mode;
		memcpy(tmpentry.mac,macAddr,6);
		tmpentry.enabled =1;	
		//printf("mode:%x, mac:%2x:%2x:%2x:%2x:%2x:%2x[%s]:[%d].\n",mode,macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],__FUNCTION__,__LINE__);
		index=rtk_get_qos_rule_entry_index(&tmpentry);
		if(index){
			ret=rtk_get_qos_rule_entry_by_index(&entry[0],index);
			if (ret == 0)
			{
				strcpy(errbuff, "error,rtk_get_qos_rule failed\n");
				goto out;
			}
		}	
	}
	
	length = num*sizeof(RTK_IPQOS_T)+1; 	
	rtk_info=malloc(length+1);
	if(rtk_info == NULL){
		strcpy(errbuff, "error,not enough memory\n");
		goto out;
	}
	
	memset(rtk_info,"\0",(length+1));
	rtk_info[0]=num;
	memcpy(&rtk_info[1],&entry[0],(num*sizeof(RTK_IPQOS_T)));
	#if 0
	for(i=0;i<length+1;i++)
	{
		printf("%x ",rtk_info[i]);
		if(((i+1)%8)==0)
			printf("\n");
	}
	#endif
	inband_write(hcd_inband_chan, 0, id_get_qos_rule, (char *)(rtk_info), (length+1),1);	
	free(rtk_info);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
out :
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_get_qos_rule, errbuff, strlen(errbuff), 2);
	return (MAX_INBAND_PAYLOAD_LEN + 1);	

}

static int _add_qos_rule_imm(char *cmd, int cmd_len)
{

	char *tmpInfo = NULL;
	char *ptr;
	char		*strptr, *cmd_addr;
	char		*tokptr;
	int flag=0;
	int ret=0;
	int index=0;
	unsigned int enabled;
	unsigned char macAddr[6];
	unsigned int upbw=0;
	unsigned int downbw=0;
	int mode=0;
	char errbuf[100];
	char cmd_buf[100]={0};
	RTK_IPQOS_T entry;
	cmd[cmd_len]='\0';
	//printf("cmd:%s,[%s]:[%d].\n",cmd,__FUNCTION__,__LINE__);
	
	strptr=cmd;
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	if(strcmp(tokptr,"mac")==0)
	{
		mode= QOS_RESTRICT_MAC;
	}
	else
	{
		printf("not support mode!\n");
		goto out;
	}
	
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	if(mode==QOS_RESTRICT_MAC){
		if (strlen(tokptr)!=12 || !string_to_hexmac(tokptr, macAddr, 12)) {
			printf("invalid mac address:%s\n", tokptr);
			goto out;
		}
	}
	//printf("tokptr:%s,%x:%x:%x:%x:%x:%x [%s]:[%d].\n",tokptr,macAddr[0], macAddr[1], 
	//	macAddr[2], macAddr[3],macAddr[4],macAddr[5],__FUNCTION__,__LINE__);
		
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	
	sscanf(tokptr, "%d", &upbw);

	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	
	sscanf(tokptr, "%d", &downbw);
	
	memset(&entry,0,sizeof(RTK_IPQOS_T));
	entry.mode =mode |QOS_RESTRICT_MAX;
	memcpy(entry.mac,macAddr,6);
	entry.enabled =1;
	entry.bandwidth =upbw;
	entry.bandwidth_downlink =downbw;
	ret=rtk_add_qos_rule_entry_imm_inband(&entry);
		
out:	
	if(ret ==0)
		sprintf(errbuf,"Set rtk_add_qos_rule_imm fail\n");
	else
		sprintf(errbuf,"Set rtk_add_qos_rule_imm successful\n");
	
	printf("%s",errbuf);
	inband_write(hcd_inband_chan,0,id_add_qos_rule_imm,errbuf,strlen(errbuf)+1,1); //reply
	return MAX_INBAND_PAYLOAD_LEN+1;

	
}
static int _del_qos_rule_imm(char *cmd, int cmd_len)
{

	char *tmpInfo = NULL;
	char *ptr;
	char		*strptr, *cmd_addr;
	char		*tokptr;
	int flag=0;
	int ret=0;
	int index=0;
	unsigned int enabled;
	unsigned char macAddr[6];
	
	int mode=0;
	char errbuf[100];
	char cmd_buf[100]={0};
	int delall =0;
	RTK_IPQOS_T entry;
	cmd[cmd_len]='\0';
	//printf("cmd:%s,[%s]:[%d].\n",cmd,__FUNCTION__,__LINE__);
	
	strptr=cmd;
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	if(strcmp(tokptr,"mac")==0)
	{
		mode= QOS_RESTRICT_MAC;
	}
	else if(strcmp(tokptr,"all")==0)
	{
		delall= 1;
		goto process;
	}	
	else
	{
		printf("not support mode!\n");
		goto out;
	}
	
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	
	if(mode==QOS_RESTRICT_MAC)
	{
		if (strlen(tokptr)!=12 || !string_to_hexmac(tokptr, macAddr, 12)) {
			printf("invalid mac address:%s\n", tokptr);
			goto out;
		}
	}
	
process:	
	memset(&entry,0,sizeof(RTK_IPQOS_T));
	entry.mode =mode |QOS_RESTRICT_MAX;
	memcpy(entry.mac,macAddr,6);
	entry.enabled =1;
	
	ret=rtk_del_qos_rule_entry_imm_inband(&entry,delall);
	
out:	
	if(ret ==0)
		sprintf(errbuf,"Set rtk_del_qos_rule_imm fail\n");
	else
		sprintf(errbuf,"Set rtk_del_qos_rule_imm successful\n");
	
	printf("%s",errbuf);
	inband_write(hcd_inband_chan,0,id_del_qos_rule_imm,errbuf,strlen(errbuf)+1,1); //reply

	return MAX_INBAND_PAYLOAD_LEN+1;
}
#endif
static int _get_macfilter_rule(char *cmd , int cmd_len)
{
	int entryNum=0;
	int ret=0,length=0;
	unsigned char * rtk_info=NULL;
	char		*strptr, *cmd_addr;
	char		*tokptr;
	unsigned char macAddr[6]={0};
	unsigned char errbuff[48] = {0};
	int allFlag=0,index =0;
	RTK_MACFILTER_T entry[MAX_FILTER_NUM]={0};
	
	cmd[cmd_len]='\0';
	strptr=cmd;
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	if(strcmp(tokptr,"mac")==0)
	{
		tokptr = strsep(&strptr," ");
		
		if (tokptr==NULL)
		{
			goto out;
		}
		
		if (strlen(tokptr)!=12 || !string_to_hexmac(tokptr, macAddr, 12)) {
			printf("invalid mac address:%s\n", tokptr);
			goto out;
		}
		
	}
	else if(strcmp(tokptr,"all")==0)
	{
		allFlag= 1;
		goto process;
	}	
	else
	{
		printf("not support mode!\n");
		goto out;
	}
process:
	
	if(allFlag)
	{
		ret=rtk_get_mac_filter_entry(&entryNum, &entry[0], MAX_FILTER_NUM);
		if(ret==0)
		{
			strcpy(errbuff, "error,rtk_get_mac_filter_entry failed\n");
			goto out;
		}
	}
	else
	{
		memset(&entry[0],0,sizeof(RTK_MACFILTER_T));
		memcpy(entry[0].macAddr,macAddr,6);
		//printf("mac:%2x:%2x:%2x:%2x:%2x:%2x[%s]:[%d].\n",macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5],__FUNCTION__,__LINE__);
		index=rtk_find_macfilter_rule_by_mac(&entry[0]);
		if(index==0){
			entryNum=0;
		}	
		else
		{
			entryNum =1;
		}
	}

	length = entryNum*sizeof(RTK_MACFILTER_T)+1; 	
	rtk_info=malloc(length+1);
	if(rtk_info == NULL){
		strcpy(errbuff, "error,not enough memory\n");
		goto out;
	}
	memset(rtk_info,"\0",(length+1));
	
	rtk_info[0]=entryNum;
	if(entryNum)
	{
		memcpy(&rtk_info[1],&entry[0],(entryNum*sizeof(RTK_MACFILTER_T)));
	}
	#if 0
	printf("----Dump macfilter info:entry:%d,length:%d [%s]:[%d].\n",entryNum,length,__FUNCTION__,__LINE__);
	int i;
	for(i=0;i<length+1;i++)
	{
		printf("%02x ",rtk_info[i]);
		if(((i+1)%8)==0)
			printf("\n");
	}
	printf("\n");
	#endif
	
	inband_write(hcd_inband_chan, 0, id_get_macfilter_rule, (char *)(rtk_info), (length+1),1);	
	free(rtk_info);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
out:
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_get_macfilter_rule, errbuff, strlen(errbuff), 2);
	return (MAX_INBAND_PAYLOAD_LEN + 1);	
}

static int _add_macfilter_rule(char *cmd , int cmd_len)
{
	char		*strptr, *cmd_addr;
	char		*tokptr;
	int flag=0;
	int ret=0;
	int index=0;
	unsigned int enabled;
	unsigned char macAddr[6];
	unsigned int upbw=0;
	unsigned int downbw=0;
	int mode=0;
	char errbuf[100];
	char cmd_buf[100]={0};
	RTK_MACFILTER_T entry;
	cmd[cmd_len]='\0';
	//printf("cmd:%s,[%s]:[%d].\n",cmd,__FUNCTION__,__LINE__);
	
	strptr=cmd;
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	if(strcmp(tokptr,"mac")!=0)
	{
		printf("not support mode!\n");
		goto out;
	}
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	
	if (strlen(tokptr)!=12 || !string_to_hexmac(tokptr, macAddr, 12)) {
		printf("invalid mac address:%s\n", tokptr);
		goto out;
	}
	#if 0
	printf("tokptr:%s,%x:%x:%x:%x:%x:%x [%s]:[%d].\n",tokptr,macAddr[0], macAddr[1], 
		macAddr[2], macAddr[3],macAddr[4],macAddr[5],__FUNCTION__,__LINE__);
	#endif
	memset(&entry,0,sizeof(RTK_MACFILTER_T));
	memcpy(entry.macAddr,macAddr,6);
	ret=rtk_add_macfilter_rule_entry_imm_inband(&entry);
out:	
	if(ret ==0){
		sprintf(errbuf,"Set rtk_add_macfilter_rule_entry_imm_inband fail\n");
		inband_write(hcd_inband_chan,0,id_add_macfilter_rule_imm,errbuf,strlen(errbuf)+1,2); //fail reply
	}	
	else{
		sprintf(errbuf,"Set rtk_add_macfilter_rule_entry_imm_inband successful\n");
		inband_write(hcd_inband_chan,0,id_add_macfilter_rule_imm,errbuf,strlen(errbuf)+1,1); // reply
	}	
	
	printf("%s",errbuf);

	return MAX_INBAND_PAYLOAD_LEN+1;

	
}



static int _del_macfilter_rule(char *cmd , int cmd_len)
{
	char		*strptr, *cmd_addr;
	char		*tokptr;
	int flag=0;
	int ret=0;
	int index=0;
	unsigned char macAddr[6]={0};
	int mode=0;
	char errbuf[100];
	char cmd_buf[100]={0};
	int delall =0;
	RTK_MACFILTER_T entry;
	cmd[cmd_len]='\0';
	//printf("cmd:%s,[%s]:[%d].\n",cmd,__FUNCTION__,__LINE__);
	
	strptr=cmd;
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	if(strcmp(tokptr,"mac")==0)
	{
		delall=0;
	}
	else if(strcmp(tokptr,"all")==0)
	{
		delall= 1;
		goto process;
	}	
	else
	{
		printf("not support command!\n");
		goto out;
	}
	
	tokptr = strsep(&strptr," ");
	if (tokptr==NULL)
	{
		goto out;
	}
	
	if (strlen(tokptr)!=12 || !string_to_hexmac(tokptr, macAddr, 12)) {
		printf("invalid mac address:%s\n", tokptr);
		goto out;
	}
	
process:	
	#if 0
	printf("tokptr:%s,%x:%x:%x:%x:%x:%x [%s]:[%d].\n",tokptr,macAddr[0], macAddr[1], 
	macAddr[2], macAddr[3],macAddr[4],macAddr[5],__FUNCTION__,__LINE__);
	#endif
	memset(&entry,0,sizeof(RTK_MACFILTER_T));
	memcpy(entry.macAddr,macAddr,6);
		
	ret=rtk_del_macfilter_rule_entry_imm_inband(&entry,delall);
	
out:	
	if(ret ==0){
		sprintf(errbuf,"Set rtk_del_macfilter_rule_entry_imm_inband fail\n");
		inband_write(hcd_inband_chan,0,id_del_macfilter_rule_imm,errbuf,strlen(errbuf)+1,2); //fail reply
	}	
	else{
		sprintf(errbuf,"Set rtk_del_macfilter_rule_entry_imm_inband successful\n");
		inband_write(hcd_inband_chan,0,id_del_macfilter_rule_imm,errbuf,strlen(errbuf)+1,1); //reply
	}	
	
	printf("%s",errbuf);
	return MAX_INBAND_PAYLOAD_LEN+1;
}


#if CONFIG_RTK_NAS_FILTER
static int  _enable_nas_filter(char *cmd, int cmd_len)
{
    // 1-enable, 0-disable
    int enable, ret;
    char errbuf[64];
    printf("[%s:%d] cmd=%s, cmd_len=%d\n", __FUNCTION__, __LINE__, cmd, cmd_len);

    cmd[cmd_len] = '\0';
    enable = atoi(cmd);
    printf("[%s:%d] enable=%d\n", __FUNCTION__, __LINE__, enable);
    ret = rtk_set_nas_filter_enable(enable);
    
	if(ret == 0)
		sprintf(errbuf,"Enable nas filter fail\n");
	else
		sprintf(errbuf,"Enable nas filter successful\n");
	
	printf("[%s:%d]%s\n", __FUNCTION__, __LINE__, errbuf);
	inband_write(hcd_inband_chan,0,id_enable_nas_filter, errbuf,strlen(errbuf)+1,1); //reply

	return MAX_INBAND_PAYLOAD_LEN+1;
}

static int  _get_status_nas_filter(char *cmd, int cmd_len)
{
    // 1 - enable, 0 -disable
    int enabled, ret;
    char errbuf[64] = {0};

    ret = rtk_get_nas_filter_enable(&enabled);
    if(ret!=1){
        sprintf(errbuf, "Get nas filter status failed!\n");
    }else{
        sprintf(errbuf, "%d", enabled);
    }

	printf("[%s:%d]%s\n", __FUNCTION__, __LINE__, errbuf);
	inband_write(hcd_inband_chan,0,id_get_status_nas_filter, errbuf,strlen(errbuf)+1,1); //reply
	return MAX_INBAND_PAYLOAD_LEN+1;
}

static char _nas_filter_build_char(char c1, char c2)
{
    char result = 0;
    if(c1>='0' && c1<='9'){
        result |= ((c1-'0')&0xF)<<4;
    }else if(c1>='a'&&c1<='f'){
        result |= ((c1-'a'+10)&0xF)<<4;
    }else if(c1>='A'&&c1<='F'){
        result |= ((c1-'A'+10)&0xF)<<4;
    }else{
        printf("[%s:%d]%02x is wrong!\n", __FUNCTION__, __LINE__, c1);
    }

    if(c2>='0' && c2<='9'){
        result |= ((c2-'0')&0xF);
    }else if(c2>='a'&&c2<='f'){
        result |= ((c2-'a'+10)&0xF);
    }else if(c2>='A'&&c2<='F'){
        result |= ((c2-'A'+10)&0xF);
    }else{
        printf("[%s:%d]%02x is wrong!\n", __FUNCTION__, __LINE__, c2);
    }

    printf("[%s:%d] result=%02x\n", __FUNCTION__, __LINE__, result);
    return result&0xFF;
}

static int _add_nas_filter(char *cmd, int cmd_len)
{
    printf("[%s:%d] cmd=%s, cmd_len=%d\n", __FUNCTION__, __LINE__, cmd, cmd_len);
    int i, ret;
    RTK_NASFILTER_Tp new_entry = (RTK_NASFILTER_Tp)malloc(sizeof(RTK_NASFILTER_T));
    memset(new_entry, 0x0, sizeof(RTK_NASFILTER_T));
    char errbuf[64];
    if(new_entry==NULL){
        sprintf(errbuf, "Malloc new space error!");
        goto out;
    }

    if(cmd_len==12){
        // no comments
        for(i=0; i<6; i++)
            new_entry->macAddr[i] = _nas_filter_build_char(cmd[i*2], cmd[i*2+1]);
        memset(new_entry->comment, 0x0, RTK_FW_COMMENT_LEN);
    }else if(cmd_len>12){
        for(i=0; i<6; i++)
            new_entry->macAddr[i] = _nas_filter_build_char(cmd[i*2], cmd[i*2+1]);
        memcpy(new_entry->comment, cmd+12, cmd_len-12);
        memset(new_entry->comment+cmd_len-12, 0x0, 1);
    }

    printf("new_entry->macAddr=%02x:%02x:%02x:%02x:%02x:%02x\n", new_entry->macAddr[0], new_entry->macAddr[1],new_entry->macAddr[2],new_entry->macAddr[3],new_entry->macAddr[4],new_entry->macAddr[5]);
    if(cmd_len>12)
        printf("new_entry->comment=%s\n", new_entry->comment);
    ret = rtk_add_nas_filter_entry(new_entry);
    if(ret!=RTK_SUCCESS)
        sprintf(errbuf, "Add nas filter error!");
    else
        sprintf(errbuf, "Add nas filter succeed!");

out:
	printf("[%s:%d]%s\n", __FUNCTION__, __LINE__, errbuf);
	inband_write(hcd_inband_chan,0,id_add_nas_filter, errbuf,strlen(errbuf)+1,1); //reply
	return MAX_INBAND_PAYLOAD_LEN+1;
}

static int _del_nas_filter(char *cmd, int cmd_len)
{
    char errbuf[64] = {0};
    int ret, i;
    RTK_NASFILTER_Tp entry = (RTK_NASFILTER_Tp)malloc(sizeof(RTK_NASFILTER_T));
    if(entry==NULL){
        sprintf(errbuf, "Malloc space error!");
        goto out;
    }
    memset(entry, 0x0, sizeof(RTK_NASFILTER_T));

    printf("[%s:%d] cmd=%s, cmd_len=%d\n", __FUNCTION__, __LINE__, cmd, cmd_len);
    if(strstr(cmd, "all")!=NULL || strstr(cmd, "All")!=NULL || strstr(cmd, "ALL")!=NULL){
        // delete all
        printf("delete all\n");
        ret = rtk_del_nas_filter_entry(entry, 1);
    }else{
        for(i=0; i<6; i++)
            entry->macAddr[i] = _nas_filter_build_char(cmd[i*2], cmd[i*2+1]);
        memcpy(entry->comment, cmd+12, cmd_len-12);
        memset(entry->comment + (cmd_len-12), 0x0, 1);

        printf("delete: entry->macAddr=%02x:%02x:%02x:%02x:%02x:%02x\n", entry->macAddr[0], entry->macAddr[1],entry->macAddr[2],entry->macAddr[3],entry->macAddr[4],entry->macAddr[5]);
        printf("delete: entry->comment=%s\n", entry->comment);
        ret = rtk_del_nas_filter_entry(entry, 0);
    }

    if(ret!=RTK_SUCCESS){
        printf("[%s:%d]Delete nas entry failed\n", __FUNCTION__, __LINE__);
        sprintf(errbuf, "Delete nas filter entry failed!\n");
    }else{
        printf("[%s:%d]Delete nas entry succeed\n", __FUNCTION__, __LINE__);
        sprintf(errbuf, "Delete nas filter entry succeed!\n");
    }

out:
	inband_write(hcd_inband_chan,0,id_del_nas_filter, errbuf, strlen(errbuf)+1,1); //reply
	return MAX_INBAND_PAYLOAD_LEN+1;
}

static int _get_nas_filter_entry(char *cmd, int cmd_len)
{
    int actual_num, max_num = 40, ret; // this max_numer must < sizeof(ioh->rx_buffer)/sizeof(RTK_NASFILTER_T), which is 47.xxx, so here we at most allow 40
    char errbuf[1500] = {0};
    RTK_NASFILTER_T entries[max_num];

    ret = rtk_get_nas_filter_entry(&actual_num, entries, max_num);
    if(ret!=1){
        printf("[%s:%d]Get nas entries failed\n", __FUNCTION__, __LINE__);
        sprintf(errbuf, "Get nas filter status failed!\n");
    }else{
        printf("[%s:%d]Get nas entries succeed\n", __FUNCTION__, __LINE__);
        memcpy(errbuf, entries, actual_num*sizeof(RTK_NASFILTER_T));
    }

	inband_write(hcd_inband_chan,0,id_get_nas_filter_entry, errbuf, actual_num*sizeof(RTK_NASFILTER_T)+1,1); //reply
	return MAX_INBAND_PAYLOAD_LEN+1;
        
}
#endif
static int _restart_wan(char *cmd, int cmd_len)
{
	int ret;
	char rsp = '\0';
	//rsp frist
	inband_write(hcd_inband_chan, 0, id_restart_wan, (char *)(&rsp),1,1);		
	ret=rtk_restart_wan();
	if (ret == 0){
		printf("%s function fail\n",cmd);
	}	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
}
static int _get_pppoe_err_code(char *cmd, int cmd_len)
{
	int ret;
	int err_code;
	ret=rtk_get_pppoe_err_code(&err_code);
	if (ret == 0)
	{
		printf("%s function fail\n",cmd);
		err_code = 0xffffffff;
	}	
	inband_write(hcd_inband_chan, 0, id_get_pppoe_err_code, (char *)(&err_code),sizeof(err_code),1);	
	return (MAX_INBAND_PAYLOAD_LEN + 1);
}
static int _wlan_immediately_work(char *cmd, int cmd_len)
{
	int ret;
	cmd[cmd_len]='\0';
	if(!strstr(cmd,"wlan"))	
		return -1;
	
	ret = rtk_wlan_immediately_work(cmd);
	if(ret == 0) 
		return -1;
	else
		return 0;
}

static int _get_firmware_version(char *cmd , int cmd_len)
{
	printf("[%s:%d]\n", __FUNCTION__,__LINE__);
	int ret,length;
	unsigned char errbuff[48] = {0};
	char *version = (char *)malloc(FW_VERSION_MAX_LEN);
	if(version == NULL){
		strcpy(errbuff, "error,not enough memory\n");
		inband_write(hcd_inband_chan, 0, id_get_fw_version, errbuff, strlen(errbuff), 2);	
		return (MAX_INBAND_PAYLOAD_LEN + 1);
	}

	ret=  rtk_get_firmware_version(&version);
	if (ret == 0)
	{
		strcpy(errbuff, "error,rtk_get_firmware_version failed\n");
		goto ssr_err_out;
	}
	
	length = strlen(version);
	//printf("%s.%d.return to client. firmware_version %s\n",__FUNCTION__,__LINE__, version);
	inband_write(hcd_inband_chan, 0, id_get_fw_version, version, length,1);	
	free(version);
	return (MAX_INBAND_PAYLOAD_LEN + 1);
	
ssr_err_out :
	printf("%s",errbuff);
	inband_write(hcd_inband_chan, 0, id_get_fw_version, errbuff, strlen(errbuff), 2);	
	free(version);
	return (MAX_INBAND_PAYLOAD_LEN + 1);
}
#endif


#ifdef HOST_SEND_CONFIG_FILE
static int _send_config_file(char *data_p , int data_len)
{
	int fd,filenamelen,enable_flag_len,enable_flag_value;
	char filename[50]; 
	char *pfilenameend;
	//printf("******len=%d data=%s \n",data_len,data_p);

	char *pfilename= strstr(data_p,"filename=");
	if(NULL != pfilename)
	{//save file

		//find file name and len
		pfilenameend = strchr(data_p,'\n');
		filenamelen = (int)(pfilenameend - pfilename) - strlen("filename=");
		memcpy(filename,pfilename + strlen("filename="),filenamelen);
		filename[filenamelen] = '\0';;
		
		//open or new a file to save data
		fd = open(filename,O_RDWR|O_CREAT|O_TRUNC);
		if(fd < 0)
		{//change path to ./tmp/
			char *ptmp_name;
			char tmp_name[filenamelen];
			ptmp_name = strchr(filename,'/');
			while(NULL != ptmp_name)
			{
				strcpy(tmp_name,ptmp_name+1);
				ptmp_name = strchr(tmp_name,'/');
			}
			sprintf(filename,"/tmp/%s",tmp_name);
			fd = open(filename,O_RDWR|O_CREAT|O_TRUNC);
		}
		
		//-d:only save config; -e:save and enable config file 
		enable_flag_value = 1;
		enable_flag_len = 0;
		
		if(!strcmp(pfilenameend+1,"-e") )
		{
			enable_flag_len = strlen("-e");
			enable_flag_value = 1;
		}
		else if(!strcmp(pfilenameend+1,"-d") )
		{
			enable_flag_len = strlen("-d");
			enable_flag_value = 0;
		}
		
		write(fd,pfilenameend + 1 + enable_flag_len,data_len - filenamelen -  strlen("filename=") -1 - enable_flag_len);
		if(fd < 0)
			printf("Save file %s fail!\n",filename);
		else
			printf("Save file %s length %d successed!\n",filename,data_len);
		close(fd);
	}
	if(enable_flag_value && NULL !=pfilename)
	{
//#if defined(SET_MIB_FROM_FILE)
		//readFileSetMib(filename);//enable config file
		char cmd[60];
		sprintf(cmd,"cp %s /var/sys.conf",filename);
		system(cmd);
		system("flash setconf end");
		system("rm /var/sys.conf");
//#endif
	}
	return 0;
}

static int _get_config_file(char *cmd , int cmd_len)
{	
	int ret = -1;
	char cmd1[50];
	int fd,flen;
	char *data;
	char *line_data ;
	char *tmp_pdata1,*tmp_pdata2;
	char *snd_data;
	struct stat ffstat;
	char first_vlanconfig_tbl_flag = 0;
	char first_schedule_tbl_flag = 0;
	char schedule_wlan_idx;
	//dump all config value to a file
	char filename[]= "/tmp/config.txt";
	sprintf(cmd1,"flash all > %s",filename);
	system(cmd1);
	
	//read the config file 
	fd = open(filename,O_RDWR);
	if(fd < 0)
		return -1;
	fstat(fd, &ffstat);
	flen = ffstat.st_size;

	if((data = (char *)malloc(flen)) == NULL)
	{
		printf("data buffer allocation failed!\n");
		return -1;
	}
	if(read(fd, data , flen)< 0)
		goto GO_TO_END;
	close(fd);

	//leave config file only needed config value
	fd = open(filename,O_RDWR|O_CREAT|O_TRUNC);
	if(fd < 0)
		goto GO_TO_END;
	line_data = data;
	tmp_pdata1 = data - 1;
	flen = 0;
	if(NULL == strcmp(cmd,"all"))
	{
		while(tmp_pdata1 != NULL)
		{
			if(!(*line_data=='D' && *(line_data +1) =='E' && *(line_data +2) =='F' && *(line_data +3) =='_'))
			{
				if(!strncmp(line_data,"VLANCONFIG_TBL",strlen("VLANCONFIG_TBL")))
				{
					if(!strncmp(line_data,"VLANCONFIG_TBL_NUM",strlen("VLANCONFIG_TBL_NUM")))
						first_vlanconfig_tbl_flag=1;
					if(!first_vlanconfig_tbl_flag)
					{
						tmp_pdata1 = strchr(line_data,'\n');
						line_data = tmp_pdata1 + 1;
						continue;
					}
				}
				if(!strncmp(line_data,"SCHEDULE_TBL",strlen("SCHEDULE_TBL")) || !strncmp(line_data+6,"SCHEDULE_TBL",strlen("SCHEDULE_TBL")))
				{
					if(!strncmp(line_data+6,"SCHEDULE_TBL_NUM",strlen("SCHEDULE_TBL_NUM")))
					{
						first_schedule_tbl_flag=1;
						schedule_wlan_idx = *(line_data + 4);//wlan0 or wlan1
					}
					if(!first_schedule_tbl_flag)
					{
						tmp_pdata1 = strchr(line_data,'\n');
						line_data = tmp_pdata1 + 1;
						continue;
					}
					if(strncmp(line_data,"WLAN",strlen("WLAN")))
					{//if not include 'WLAN', add 
						sprintf(cmd1,"WLAN%c_",schedule_wlan_idx);
						memcpy(data+flen,cmd1,6);
						flen += 6 ;
					}
				}
				tmp_pdata2 = strchr(line_data,'\n');
				if(tmp_pdata2 == NULL)
					tmp_pdata2 = data + ffstat.st_size;
				memcpy(data+flen,line_data,(tmp_pdata2 - tmp_pdata1));
				flen += (tmp_pdata2 - tmp_pdata1) ;
			}
			tmp_pdata1 = strchr(line_data,'\n');
			line_data = tmp_pdata1 + 1;
		}		
	}
	else if (NULL == strcmp(cmd,"wlan"))
	{		
		while(tmp_pdata1 != NULL)
		{
			if(*line_data=='W' && *(line_data +1) =='L' && *(line_data +2) =='A' && *(line_data +3) =='N')//only WLAN 
			{
				tmp_pdata2 = strchr(line_data,'\n');
				if(tmp_pdata2 == NULL)
					tmp_pdata2 = data + ffstat.st_size;
				memcpy(data+flen,line_data,(tmp_pdata2 - tmp_pdata1));
				flen += (tmp_pdata2 - tmp_pdata1) ;
			}
			if(!strncmp(line_data,"SCHEDULE_TBL",strlen("SCHEDULE_TBL")) || !strncmp(line_data+6,"SCHEDULE_TBL",strlen("SCHEDULE_TBL")))
			{
				if(!strncmp(line_data+6,"SCHEDULE_TBL_NUM",strlen("SCHEDULE_TBL_NUM")))
				{
					first_schedule_tbl_flag=1;
					schedule_wlan_idx = *(line_data + 4);//wlan0 or wlan1
				}
				if(!first_schedule_tbl_flag)
				{
					tmp_pdata1 = strchr(line_data,'\n');
					line_data = tmp_pdata1 + 1;
					continue;
				}
				if(strncmp(line_data,"WLAN",strlen("WLAN")))
				{//if not include 'WLAN', add 
					sprintf(cmd1,"WLAN%c_",schedule_wlan_idx);
					memcpy(data+flen,cmd1,6);
					flen += 6 ;
					
					tmp_pdata2 = strchr(line_data,'\n');
					if(tmp_pdata2 == NULL)
						tmp_pdata2 = data + ffstat.st_size;
					memcpy(data+flen,line_data,(tmp_pdata2 - tmp_pdata1));
					flen += (tmp_pdata2 - tmp_pdata1) ;
				}
			}
			tmp_pdata1 = strchr(line_data,'\n');
			line_data = tmp_pdata1 + 1;
		}
	}
	else if (NULL == strcmp(cmd,"hw"))
	{	
		while(tmp_pdata1 != NULL)
		{
			if(*line_data=='H' && *(line_data +1) =='W' && *(line_data +2) =='_' )//only HW 
			{
				tmp_pdata2 = strchr(line_data,'\n');
				if(tmp_pdata2 == NULL)
					tmp_pdata2 = data + ffstat.st_size;
				memcpy(data+flen,line_data,(tmp_pdata2 - tmp_pdata1));
				flen += (tmp_pdata2 - tmp_pdata1) ;
			}
			tmp_pdata1 = strchr(line_data,'\n');
			line_data = tmp_pdata1 + 1;
		}
	}
	else
	{	
		goto GO_TO_END;
	}		
	write(fd,data,flen);

	//read form file to memory
	if(flen <= MAX_INBAND_PAYLOAD_LEN)
	{
		if(read(fd, cmd,flen)< 0) //copy to cmd[]
			goto GO_TO_END;
		ret = flen;
	}
	else
	{
		if(read(fd, data,flen)< 0)//copy to data[] 
			goto GO_TO_END;
		ret = flen;
		inband_write(hcd_inband_chan,0,id_get_config_file,data,flen,1); //reply
		printf("Send %s config file length %d end!\n",cmd,flen);
	}

GO_TO_END:
	close(fd);
	free(data);	

	//delete config file 
	sprintf(cmd1,"rm %s",filename);	
	system(cmd1);
	
	return ret;
}

#endif
#ifdef INBAND_GET_FILE_SUPPOPRT
static int _get_file(char *cmd , int cmd_len)
{
	char fileName[MAX_PATH_LEN+1]={0};
	int ret=-1,flen=-1;
	int fd=-1;
	char *data=NULL;
	struct stat ffstat;
	if(!cmd)
	{
		printf("invalid input!\n");
		goto GET_FILE_END;
	}
	if(strlen(cmd)>MAX_PATH_LEN)
	{
		printf("path max %d!\n",MAX_PATH_LEN);
		goto GET_FILE_END;
	}

	strcpy(fileName,cmd);
	//printf("%s:%d fileName=%s\n",__FUNCTION__,__LINE__,fileName);
	fd=open(fileName,O_RDONLY);
	if(fd<0)
	{
		printf("can't open file %s!\n",fileName);
		goto GET_FILE_END;
	}
	fstat(fd, &ffstat);
	flen = ffstat.st_size;
	
//read form file to memory
	if(flen <= MAX_INBAND_PAYLOAD_LEN)
	{
		if(read(fd, cmd,flen)< 0) //copy to cmd[]
			goto GET_FILE_END;		
	//	printf("%s:%d\n",__FUNCTION__,__LINE__);
	}
	else
	{
		if((data = (char *)malloc(flen)) == NULL)
		{
			printf("data buffer allocation failed!\n");
			goto GET_FILE_END;
		}
	//	printf("%s:%d\n",__FUNCTION__,__LINE__);
		if(read(fd, data,flen)< 0)//copy to data[] 
			goto GET_FILE_END;
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		inband_write(hcd_inband_chan,0,id_get_file,data,flen,1); //reply
		printf("Send %s file length %d end!\n",fileName,flen);
	}
	ret = flen;
GET_FILE_END:
	if(data) free(data);
	close(fd);
	return ret;
}
#endif

#if defined(CONFIG_APP_ADAPTER_API)
static int _sync_to_server(char *cmd, int cmd_len)
{
	char * name=NULL,*value=NULL;
	int i=0,result=0;
	char *nameArray[] = {"status"};
    char *outPutArray[1]={0};
	char buff[512]={0};

	//analysis cmd
	if(!cmd || cmd_len<=0)
	{
		return -1;
	}
	for(i=0;i<cmd_len;i++)
	{
		if(cmd[i]==' ')
		{
			name=(char*)malloc(i+1);
			value=(char*)malloc(cmd_len-i);
			if(!name||!value)
			{
				fprintf(stderr,"Malloc fail!\n");
				return -1;
			}
			bzero(name,i+1);
			bzero(value,cmd_len-i);
			memcpy(name,cmd,i);
			memcpy(value,cmd+i+1,cmd_len-i-1);
			break;
		}
	}
	if(!name||!value)
	{
		fprintf(stderr,"Invalid input format!\n");
		return -1;
	}
	printf("sync %s value %s to server\n",name,value);
	
	//call rtk cgi cmd
	sprintf(buff,"id=8&cmd=modify_config&title=%s&title_value=%s",name,value);
	outPutArray[0]=(char*)malloc(64);
	bzero(outPutArray[0],64);
	//result=rtk_common_cgi_api("get","letvcgi",buff,1,nameArray,outPutArray);
	//printf("status=%s\n",outPutArray[0]);

	if(name) free(name);
	if(value) free(value);
	if(outPutArray[0]) free(outPutArray[0]);
	return result;
		
}
#endif

#if 0
static int _sendfile(char *cmd , int cmd_len)
{
	int fd,ret=0;
	//char *filename="/var/hostapd.conf";
	char *filename="/var/linux.bin";
	char test_buf[64];

	fd = open(filename, O_RDWR | O_CREAT);
	printf("_sendfile cmd_len = %d \n",cmd_len);
	
	if (fd < 0)	{
		printf("Cannot Open file %s!\n", filename);
		return -1;
	}

	write( fd, cmd, cmd_len); 
	
	close(fd);

	//-------read file for test
	fd = open(filename, O_RDONLY);
	if (fd < 0)	{
		printf("Cannot Open file %s!\n", filename);
		return -1;
       }

	 read(fd, test_buf, 64);
	 hex_dump( test_buf, 64);
	 close(fd);
	return ret;
}
#endif

static int _set_mib(char *cmd , int cmd_len)
{
	char *param,*val,*next;	
	int ret = RET_OK,access_flag=ACCESS_MIB_SET | ACCESS_MIB_BY_NAME;
	char *intf, *tmp;

	cmd[cmd_len]='\0';//mark_patch
	do {
		intf = cmd;
		param = strchr(intf,'_')+1;
		if( !strchr(param,'=') ){
			printf("invalid command format:%s\n",cmd);
			return -1;
		}
		val = strchr(param,'=')+1;

		intf[param-intf-1] = '\0';
		param[val-param-1] = '\0';

		if( !strcmp(intf,"wps") )
			break;

		cmd = strchr(val,'\n');
		if(cmd)
		    *cmd = '\0';
		//printf(">>> set %s=%s to %s\n",param,val,intf);
		ret = access_config_mib(access_flag,param,val,intf);  //ret the

		if(cmd && (*(cmd+1) != '\0')) 
		    cmd++;
		else
			break;	
	} while(cmd);

	//free(intf);
	return ret;
}

int config_from_local(unsigned char *ptr)
{
	int fh = 0;
	struct stat status;

	if (stat(CFG_FILE_IN, &status) < 0) {
		printf("stat() error [%s]!\n", CFG_FILE_IN);
		return -1;
	}

	fh = open(CFG_FILE_IN,O_RDONLY);
	if( fh < 0 ){
		printf("File open failed\n");
		return -1;
	}

	/*
	ptr = (unsigned char *)calloc(0,status.st_size);
	if( !ptr ){
		printf("%d:memory alloc failed\n",status.st_size);
		return -1;
	}
	*/
	lseek(fh, 0L, SEEK_SET);

	if (read(fh, ptr, status.st_size) != status.st_size) {		
		printf("read() error [%s]!\n", CFG_FILE_IN);
		return -1;	
	}
	close(fh);
	return 0;
}

int parse_then_set(unsigned char *str_start, unsigned char *str_end)
{
	int ret;
	unsigned char *param, *value, *intf, line[100] = {0};

	memcpy(line,str_start,str_end-str_start);
	param = line;
	intf = param;
	param = strchr(intf,'_')+1;
	value = strchr(param,'=')+1;
	intf[param-intf-1] = '\0';
	param[value-param-1] = '\0';
	printf(">>> %s %s %s = %s \n",__FUNCTION__,intf,param,value);
	ret = access_config_mib(ACCESS_MIB_BY_NAME|ACCESS_MIB_SET,param,value,intf);
	return ret;
}

static int _set_mibs(char *cmd , int cmd_len)
//static int _cfgswrite(void)
{
	unsigned char cmd_buf[20480] = {0};
	unsigned char line[100] = {0}, *str_start, *str_end;
	cmd[cmd_len]='\0';//mark_patch
	if( config_from_local(cmd_buf) < 0 ) {
		printf("Read config from %s failed\n","/etc/Wireless/realtekap.conf");
		return -1;
	}

	str_start = cmd_buf;
	str_end = strchr(cmd_buf,'\n');
	while( str_end ){
		if( *str_start != '#' && *str_start != ' ' && *str_start != '\n' ){
			parse_then_set(str_start,str_end);
		}
		str_start = str_end+1;
		str_end = strchr(str_start,'\n');
	}

	free(cmd_buf);
	return 0;
}


static int _get_mib(char *cmd , int cmd_len)
{
	char *param;	
	int ret = RET_OK,access_flag=ACCESS_MIB_GET | ACCESS_MIB_BY_NAME;
	char *intf, *tmp;
	cmd[cmd_len]='\0';//mark_patch
	intf = cmd;
	param = strchr(intf,'_')+1;
	if(param<=1)
	{
		DEBUG_ERR("Invalid mib name!\n");
		return -1;
	}
		
	intf[param-intf-1] = '\0';
	//printf(">>> read %s from %s\n",param,intf);
	ret = access_config_mib(access_flag,param,cmd,intf);  //return value in cmd

	//free(intf);
	return ret;
}

static int _sysinit(char *cmd , int cmd_len)
{
	//now , only support init all
	cmd[cmd_len]='\0';//mark_patch	
	if(!strcmp(cmd, "all"))
		init_system(INIT_ALL);		
	else if(!strcmp(cmd, "lan"))		
		init_system(INIT_ALL);
	else if(!strcmp(cmd, "wlan"))
		init_system(INIT_ALL);
	else
		return -1;

	return 0;
}
//sta_info frame , {sta_num}{sta_info1}{sta_info2}........... 
//first byre will be total sta_info numer in this reply
static int _getstainfo(char *cmd , int cmd_len)
{
	char *buff,*tmpInfo;	
	WLAN_STA_INFO_Tp pInfo;
	int sta_num =0,i,ret;
	cmd[cmd_len]='\0';//mark_patch
	buff = calloc(1, sizeof(WLAN_STA_INFO_T) * (MAX_STA_NUM+1));
	tmpInfo = cmd +1 ; // first byte reserve for sta_info num

	if ( buff == 0 ) {
		printf("Allocate buffer failed!\n");
		return -1;
	}

	if ( get_wlan_stainfo(cmd,  (WLAN_STA_INFO_Tp)buff ) < 0 ) {
		printf("Read wlan sta info failed!\n");

		return -1;
	}

	for (i=1; i<=MAX_STA_NUM; i++) {
		pInfo = (WLAN_STA_INFO_Tp)&buff[i*sizeof(WLAN_STA_INFO_T)];
		if (pInfo->aid && (pInfo->flag & STA_INFO_FLAG_ASOC)) {//sta exist
			memcpy(tmpInfo+ (sta_num*sizeof(WLAN_STA_INFO_T)),(char *)pInfo, sizeof(WLAN_STA_INFO_T));
			sta_num++;
		}		
	}	

	cmd[0]= (unsigned char)(sta_num&0xff);
	ret = sta_num*sizeof(WLAN_STA_INFO_T) + 1;		
	
	return ret;
}
static int _getassostanum(char *cmd , int cmd_len)
{
	int num=0;
	cmd[cmd_len]='\0';//mark_patch
	if (get_wlan_stanum(cmd, &num) < 0)
		return -1;

	cmd[0]=(unsigned char)(num&0xff);
	cmd[1]= '\0';

	return 1; // return len=1  to show sta num
	
}
static int _getbssinfo(char *cmd , int cmd_len)
{	
	WLAN_BSS_INFO_T bss;
	int bss_len=sizeof(WLAN_BSS_INFO_T);
	cmd[cmd_len]='\0';//mark_patch
	if ( get_wlan_bssinfo(cmd, &bss) < 0)
			return -1;

	memcpy(cmd,(char *)&bss,bss_len);
	
	return bss_len;
}

int get_lanport_stats(int portnum,struct port_statistics *port_stats)
{
	struct ifreq ifr;
	 int sockfd;
	 char *name="eth0";	 
	 struct port_statistics stats;
	 unsigned int *args;	

	 if(portnum > 5)
	 	return -1;	 	
	 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
 	{
      		printf("fatal error socket\n");
      		return -3;
       }
	args = (unsigned int *)&stats;
	((unsigned int *)(&ifr.ifr_data))[0] =(struct port_statistics *)&stats;
	*args = portnum;
	
	strcpy((char*)&ifr.ifr_name, name);       

    if (ioctl(sockfd, RTL819X_IOCTL_READ_PORT_STATS, &ifr)<0)
    {
      		printf("device ioctl:");
      		close(sockfd);
     		 return -1;
     }
     close(sockfd);   	     
     memcpy(port_stats,(char *)&stats,sizeof(struct port_statistics));

    return 0;	 

}

int get_lanport_status(int portnum,struct lan_port_status *port_status)
{
	struct ifreq ifr;
	 int sockfd;
	 char *name="eth0";	 
	 struct lan_port_status status;
	 unsigned int *args;	

	 if(portnum > 5)
	 	return -1;	 	
	 
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
 	{
      		printf("fatal error socket\n");
      		return -3;
    }
	args = (unsigned int *)&status;
	((unsigned int *)(&ifr.ifr_data))[0] =(struct lan_port_status *)&status;
	*args = portnum;
	
	strcpy((char*)&ifr.ifr_name, name);       

    if (ioctl(sockfd, RTL819X_IOCTL_READ_PORT_STATUS, &ifr)<0)
    {
      		printf("device ioctl:");
      		close(sockfd);
     		 return -1;
     }
     close(sockfd);   	
     memcpy((char *)port_status,(char *)&status,sizeof(struct lan_port_status));

    return 0;	 

}
int get_lanport_rate(int portnum,struct port_rate *portRate)
{
#if 1
	

 	FILE *stream;
  	char buffer[512];
	char* ptr;
	int port=-1;
	unsigned int rx_rate=0,tx_rate=0;
	int index=0;
	struct port_rate *portRateInfo=NULL;
	unsigned int total_rx_rate=0;
	unsigned int total_tx_rate;
	int ret=0;
	//printf("portnum:%x,[%s]:[%d].\n",portnum,__FUNCTION__,__LINE__);
	stream = fopen (_RTL_PORT_RATE, "r" );
	if ( stream != NULL ) { 	
		while(fgets(buffer, sizeof(buffer), stream))
		{

			ptr = strstr(buffer, "port");
			if (ptr) 
			{
				ptr = ptr + 4;
				
				if (ptr)
				{	
					sscanf( ptr, "%d", &port );   
				#if 0
					printf("port:%x [%s]:[%d].\n",port,__FUNCTION__,__LINE__);
				#endif
				}
			}
			ptr = strstr(buffer, "rx:");
			if (ptr) 
			{
				ptr = ptr + 3;
				if (ptr)
				{
					
					sscanf(ptr, "%d", &rx_rate); 
					
					//printf("rx_rate:%d,[%s]:[%d].\n",rx_rate,__FUNCTION__,__LINE__);
				}
			}
			
			ptr = strstr(buffer, "tx:");
			if (ptr) 
			{
				ptr = ptr + 3;
				if (ptr)
				{
					
					sscanf(ptr, "%d", &tx_rate); 
					
					//printf("tx_rate:%d,[%s]:[%d].\n",tx_rate,__FUNCTION__,__LINE__);
				}
			}
			total_rx_rate+=rx_rate;
			total_tx_rate+=tx_rate;
			
			
			if(portnum >=0&&portnum<4)
			{
				if(port==portnum)
				{
					portRateInfo=portRate;
					if(portRateInfo==NULL){
						ret=-1;
						goto out;
					}		
					if((index<4)&&portRateInfo){
						portRateInfo->port_id =port;
						portRateInfo->rx_rate =rx_rate;
						portRateInfo->tx_rate =tx_rate;
						//printf("portnum:%x,%d,[%s]:[%d].\n",portnum,portRateInfo->port_id,
						//	portRateInfo->rx_rate,portRateInfo->tx_rate,__FUNCTION__,__LINE__);

						
					}
					
					break;
				}	
				
			}
			else if (portnum==0xff)
			{
				portRateInfo=portRate+index;
				if(portRateInfo==NULL){
					ret=-1;
					goto out;
				}	
				if((index<4)&&portRateInfo)
				{
					portRateInfo->port_id =port;
					portRateInfo->rx_rate =rx_rate;
					portRateInfo->tx_rate =tx_rate;
					//printf("[index]:%x,%d,[%s]:[%d].\n",portnum,portRateInfo->port_id,
					//	portRateInfo->rx_rate,portRateInfo->tx_rate,__FUNCTION__,__LINE__);
				}	
			}
			
			if(port!=-1)
				index++;
			
			port=-1;
			rx_rate=0;
			tx_rate=0;
			portRateInfo =NULL;
		}
		for(index =0;index<4;index++)
		{
			
			if(portnum >=0&&portnum<4)
			{
				if(port==portnum)
				{
					if(total_rx_rate)
					portRateInfo->rx_percent=portRateInfo->rx_rate/total_rx_rate*100;
					if(total_tx_rate)
					portRateInfo->tx_percent=portRateInfo->tx_rate/total_tx_rate*100;
					
				}
				
			}
			else if (portnum==0xff)
			{
				portRateInfo=portRate+index;
				if(portRateInfo)
				{
					if(total_rx_rate)
					portRateInfo->rx_percent=portRateInfo->rx_rate/total_rx_rate*100;
					if(total_tx_rate)
					portRateInfo->tx_percent=portRateInfo->tx_rate/total_tx_rate*100;
				}	
			}
		}
		
	}
	else
	{
		ret =-1;
	}

#endif
out:
    return ret;	 

}

static int get_dev_fields(int type, char *bp, struct user_net_device_stats *pStats)
{
    switch (type) {
    case 3:
	sscanf(bp,
	"%Lu %Lu %lu %lu %lu %lu %lu %lu %Lu %Lu %lu %lu %lu %lu %lu %lu",
	       &pStats->rx_bytes,
	       &pStats->rx_packets,
	       &pStats->rx_errors,
	       &pStats->rx_dropped,
	       &pStats->rx_fifo_errors,
	       &pStats->rx_frame_errors,
	       &pStats->rx_compressed,
	       &pStats->rx_multicast,

	       &pStats->tx_bytes,
	       &pStats->tx_packets,
	       &pStats->tx_errors,
	       &pStats->tx_dropped,
	       &pStats->tx_fifo_errors,
	       &pStats->collisions,
	       &pStats->tx_carrier_errors,
	       &pStats->tx_compressed);
	break;

    case 2:
	sscanf(bp, "%Lu %Lu %lu %lu %lu %lu %Lu %Lu %lu %lu %lu %lu %lu",
	       &pStats->rx_bytes,
	       &pStats->rx_packets,
	       &pStats->rx_errors,
	       &pStats->rx_dropped,
	       &pStats->rx_fifo_errors,
	       &pStats->rx_frame_errors,

	       &pStats->tx_bytes,
	       &pStats->tx_packets,
	       &pStats->tx_errors,
	       &pStats->tx_dropped,
	       &pStats->tx_fifo_errors,
	       &pStats->collisions,
	       &pStats->tx_carrier_errors);
	pStats->rx_multicast = 0;
	break;

    case 1:
	sscanf(bp, "%Lu %lu %lu %lu %lu %Lu %lu %lu %lu %lu %lu",
	       &pStats->rx_packets,
	       &pStats->rx_errors,
	       &pStats->rx_dropped,
	       &pStats->rx_fifo_errors,
	       &pStats->rx_frame_errors,

	       &pStats->tx_packets,
	       &pStats->tx_errors,
	       &pStats->tx_dropped,
	       &pStats->tx_fifo_errors,
	       &pStats->collisions,
	       &pStats->tx_carrier_errors);
	pStats->rx_bytes = 0;
	pStats->tx_bytes = 0;
	pStats->rx_multicast = 0;
	break;
    }
    return 0;
}

static char *get_name(char *name, char *p)
{
    while (isspace(*p))
	p++;
    while (*p) {
	if (isspace(*p))
	    break;
	if (*p == ':') {	/* could be an alias */
	    char *dot = p, *dotname = name;
	    *name++ = *p++;
	    while (isdigit(*p))
		*name++ = *p++;
	    if (*p != ':') {	/* it wasn't, backup */
		p = dot;
		name = dotname;
	    }
	    if (*p == '\0')
		return NULL;
	    p++;
	    break;
	}
	*name++ = *p++;
    }
    *name++ = '\0';
    return p;
}


void fill_statistics(struct user_net_device_stats *pStats, struct port_statistics *statistics)
{
	statistics->rx_bropkts = 0;
	statistics->rx_bytes = pStats->rx_bytes;
	statistics->rx_discard = pStats->rx_dropped;
	statistics->rx_error = pStats->rx_errors;
	statistics->rx_mulpkts = pStats->rx_multicast;
	statistics->rx_unipkts = pStats->rx_packets-pStats->rx_multicast;
	statistics->tx_bropkts = 0;
	statistics->tx_bytes = pStats->tx_bytes;
	statistics->tx_discard = pStats->tx_dropped;
	statistics->tx_error = pStats->tx_errors;
	statistics->tx_mulpkts = 0;
	statistics->tx_unipkts = pStats->tx_packets;
}


int get_wlan_stats(char *intf, struct port_statistics *statistic)
{
 	FILE *fh;
  	char buf[512];
	int type;
	struct user_net_device_stats pStats;

	fh = fopen(_PATH_PROCNET_DEV, "r");
	if (!fh) {
		printf("Warning: cannot open %s\n",_PATH_PROCNET_DEV);
		return -1;
	}
	fgets(buf, sizeof buf, fh);	/* eat line */
	fgets(buf, sizeof buf, fh);

  	if (strstr(buf, "compressed"))
		type = 3;
	else if (strstr(buf, "bytes"))
		type = 2;
	else
		type = 1;

	while (fgets(buf, sizeof buf, fh)) {
		char *s, name[40];
		s = get_name(name, buf);
		if ( strcmp(intf, name))
			continue;
		get_dev_fields(type, s, &pStats);
		fill_statistics(&pStats,statistic);
		fclose(fh);
		return 0;
    }
	fclose(fh);
	return -1;
}


int portname_to_num(char *name)
{
	int portnum=0;
	if(!strncmp(name,"p0",2)) 
		portnum=0;
	else if(!strncmp(name,"p1",2)) 
		portnum=1;
	else if(!strncmp(name,"p2",2)) 
		portnum=2;
	else if(!strncmp(name,"p3",2)) 
		portnum=3;
	else if(!strncmp(name,"p4",2)) 
		portnum=4;
	else if(!strncmp(name,"p5",2)) 
		portnum=5;
	else if (!strncmp(name,"all",2)) 
		portnum=0xff;

	return portnum;
}

static int _getlanstatus(char *cmd , int cmd_len)
{
	struct lan_port_status port_status;
	int len=sizeof(struct lan_port_status);
	int portnum;
	cmd[cmd_len]='\0';//mark_patch
	portnum = portname_to_num(cmd);
	
	if ( get_lanport_status(portnum, &port_status) < 0)
			return -1;
	
	memcpy(cmd,(char *)&port_status,len);
	
	return len;
}
static int _getlanRate(char *cmd , int cmd_len)
{
	struct port_rate port_rate[4]={0};
	int len1,len;
	unsigned char portnum;
	int portId;
	char *tmpInfo = cmd +1 ;
	cmd[cmd_len]='\0';
	
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	
	portId = portname_to_num(cmd);
	
	if ( get_lanport_rate(portId, &port_rate[0]) < 0)
		return -1;
	
	if(portId==0xff){	
		portnum=4;
	}	
	else
		portnum=1;
	
	len1=sizeof(struct port_rate)*portnum;
	len=len1+1;
	
	memset(cmd,0,len);
	cmd[0]= portnum;
	
	memcpy(tmpInfo,(char *)(&port_rate[0]),len);
	
	return len;
}

static int _setlanBandwidth(char *cmd, int cmd_len)
{
	#define RTL_UPLINK_BW	0x2
	#define RTL_DOWNLINK_BW	0x1
	int portId=-1;
	char *tmpInfo = NULL;
	char *ptr;
	int flag=0;
	int ret=-1;
	int index=0;
	unsigned long bandwidth;
	unsigned long bw_value;
	char cmd_buf[100]={0};
	cmd[cmd_len]='\0';
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	portId = portname_to_num(cmd);
	#if 0
	if(portId =0xff)
		tmpInfo =cmd+4;
	else 
		tmpInfo =cmd+3;
	
	if(tmpInfo,"up",2)
		flag =RTL_UPLINK_BW;
	else if(tmpInfo,"down",4)
		flag =RTL_DOWNLINK_BW;
	else if(tmpInfo,"all",3)
		flag =RTL_DOWNLINK_BW|RTL_UPLINK_BW;
	#endif
	
	//printf("portId:%x,[%s]:[%d].\n",portId,__FUNCTION__,__LINE__);
	flag =RTL_DOWNLINK_BW;
	if (portId==-1||flag==0)
		goto out;
	ptr = strstr(cmd, "=");
	if (ptr) 
	{
		ptr = ptr + 1;
		
		if (ptr)
		{	
			sscanf( ptr, "%d", &bandwidth );   
		#if 0
			printf("bandwidth:%x [%s]:[%d].\n",bandwidth,__FUNCTION__,__LINE__);
		#endif
		}
	}
	bw_value = bandwidth ;
	#if 0
	if(portId==0xFF)
	{
		for(index=0;index<4;index++)
		{
			if(flag&RTL_DOWNLINK_BW)
			{
				//sprintf(cmd_buf,"ifconfig %s down",cmd);
				//system(cmd_buf);
			}
			if (flag&RTL_UPLINK_BW)
			{
				
			}
		}
	}
	else 
	#endif	
	if(portId>=0&&portId<4)
	{
		if(bw_value)
		{
			
			printf("bandwidth:%d %x [%s]:[%d].\n",bandwidth,bw_value,__FUNCTION__,__LINE__);
			sprintf(cmd_buf,"echo port %d engress bw %d > %s", portId,bw_value,_RTL_PORT_BANDWIDTH);
			system(cmd_buf);
		}
	}
	ret =0;
	
out:	
	return ret;
	
}
static int _getstats(char *cmd , int cmd_len)
{
	struct port_statistics statistics;
	int len=sizeof(struct port_statistics);
	int portnum;
	char *intf, *tmp;
	cmd[cmd_len]='\0';//mark_patch
	//printf(">>> %s\n",cmd);
	
	if(!strncmp(cmd,"p",1)) {
		portnum = portname_to_num(cmd);
		
		if ( get_lanport_stats(portnum, &statistics) < 0)
				return -1;		
		memcpy(cmd,(char *)&statistics,len);
	} else {
		//get statistics of wlan
		if ( get_wlan_stats(cmd, &statistics) < 0)
				return -1;

		memcpy(cmd,(char *)&statistics,len);
	}
	
	return len;
}

/* winfred_wang, RGB LED blink releate function */

/* rgb blink mode function */

#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
static int _set_RGBLed_blinkStart(char *cmd , int cmd_len)
{
	int ret;

	ret = rgb_led_blink_param_set(cmd,cmd_len);
	if(ret < 0)
	{
		printf("param content is not correct\n");
		return -1;
	}

	rgb_led_blink_init();
	return 0;
}

static int _set_RGBLed_blinkEnd(char *cmd , int cmd_len)
{	
	rgb_led_blink_exit();	
	return 0;
}
#endif

//-------------------------------------------------------------------
void set_11ac_txrate(WLAN_STA_INFO_Tp pInfo,char* txrate)
{
	char channelWidth=0;//20M 0,40M 1,80M 2
	char shortGi=0;
	char rate_idx=pInfo->txOperaRates-0x90;
	if(!txrate)return;
/*
	TX_USE_40M_MODE		= BIT(0),
	TX_USE_SHORT_GI		= BIT(1),
	TX_USE_80M_MODE		= BIT(2)
*/
	if(pInfo->ht_info & 0x4)
		channelWidth=2;
	else if(pInfo->ht_info & 0x1)
		channelWidth=1;
	else
		channelWidth=0;
	if(pInfo->ht_info & 0x2)
		shortGi=1;

	sprintf(txrate, "%d", VHT_MCS_DATA_RATE[channelWidth][shortGi][rate_idx]>>1);
}

void print_stainfo(char *stainfo_rsp)
{
	WLAN_STA_INFO_Tp pInfo;
	int i=0,rateid=0,sta_num=stainfo_rsp[0];
	char mode_buf[20],txrate[20];


	if(sta_num <= 0)
		printf("No Associated  station now!!!!\n ",i+1);
	for(i=0;i<sta_num;i++)
	{
		pInfo = (WLAN_STA_INFO_T *)&(stainfo_rsp[i*sizeof(WLAN_STA_INFO_T)+1]);
		printf("-----------------------------------------------\n");
		printf("station No.%d info\n",i+1);
		printf("MAC address : %02x:%02x:%02x:%02x:%02x:%02x \n",pInfo->addr[0],pInfo->addr[1],pInfo->addr[2],pInfo->addr[3],pInfo->addr[4],pInfo->addr[5]);

		if(pInfo->network & BAND_11N)
			sprintf(mode_buf, "%s", " 11n");
		else if (pInfo->network & BAND_11G)
			sprintf(mode_buf,"%s",  " 11g");	
		else if (pInfo->network & BAND_11B)
			sprintf(mode_buf, "%s", " 11b");
		else if (pInfo->network& BAND_11A)
			sprintf(mode_buf, "%s", " 11a");
		else if (pInfo->network& BAND_11AC)
			sprintf(mode_buf, "%s", " 11ac");
		else
			sprintf(mode_buf, "%s", " ---");	

		printf("Mode:%s \n",mode_buf);

		printf("TX packets:%d , RX packets:%d \n",pInfo->tx_packets, pInfo->rx_packets);
		if(pInfo->txOperaRates >= 0x90) {
			//sprintf(txrate, "%d", pInfo->acTxOperaRate); 
			set_11ac_txrate(pInfo, txrate);
		}else if((pInfo->txOperaRates & 0x80) != 0x80){	
			if(pInfo->txOperaRates%2){
				sprintf(txrate, "%d%s",pInfo->txOperaRates/2, ".5"); 
			}else{
				sprintf(txrate, "%d",pInfo->txOperaRates/2); 
			}
		}else{
			if((pInfo->ht_info & 0x1)==0){ //20M
				if((pInfo->ht_info & 0x2)==0){//long
					for(rateid=0; rateid<16;rateid++){
						if(rate_11n_table_20M_LONG[rateid].id == pInfo->txOperaRates){
							sprintf(txrate, "%s", rate_11n_table_20M_LONG[rateid].rate);
							break;
						}
					}
				}else if((pInfo->ht_info & 0x2)==0x2){//short
					for(rateid=0; rateid<16;rateid++){
						if(rate_11n_table_20M_SHORT[rateid].id == pInfo->txOperaRates){
							sprintf(txrate, "%s", rate_11n_table_20M_SHORT[rateid].rate);
							break;
						}
					}
				}
			}else if((pInfo->ht_info & 0x1)==0x1){//40M
				if((pInfo->ht_info & 0x2)==0){//long
					
					for(rateid=0; rateid<16;rateid++){
						if(rate_11n_table_40M_LONG[rateid].id == pInfo->txOperaRates){
							sprintf(txrate, "%s", rate_11n_table_40M_LONG[rateid].rate);
							break;
						}
					}
				}else if((pInfo->ht_info & 0x2)==0x2){//short
					for(rateid=0; rateid<16;rateid++){
						if(rate_11n_table_40M_SHORT[rateid].id == pInfo->txOperaRates){
							sprintf(txrate, "%s", rate_11n_table_40M_SHORT[rateid].rate);
							break;
						}
					}
				}
			}
		   }
		printf("TX Rate : %s \n",txrate);
		printf("Sleep : %s \n",( (pInfo->flag & STA_INFO_FLAG_ASLEEP) ? "yes" : "no"));
		printf("Expired time : %d seconds \n",pInfo->expired_time/100);

	}

}

void print_bssinfo(char *bssinfo_rsp)
{
	WLAN_BSS_INFO_Tp bssInfo;
	char *pMsg;
	
	bssInfo = (WLAN_BSS_INFO_T *)bssinfo_rsp;
	printf("BSSID : %02x:%02x:%02x:%02x:%02x:%02x \n",bssInfo->bssid[0],bssInfo->bssid[1],bssInfo->bssid[2],
													bssInfo->bssid[3],bssInfo->bssid[4],bssInfo->bssid[5]);
	printf("SSID : %s \n",bssInfo->ssid);

	switch (bssInfo->state) {
		case STATE_DISABLED:
			pMsg = "Disabled";
			break;
		case STATE_IDLE:
			pMsg = "Idle";
			break;
		case STATE_STARTED:
			pMsg = "Started";
			break;
		case STATE_CONNECTED:
			pMsg = "Connected";
			break;
		case STATE_WAITFORKEY:
			pMsg = "Waiting for keys";
			break;
		case STATE_SCANNING:
			pMsg = "Scanning";
			break;
		default:
			pMsg=NULL;
		}

	printf("State : %s \n",pMsg);

	printf("Channel : %d \n",bssInfo->channel);		
	
}

void print_port_status(char *status)
{
	struct lan_port_status *port_status;
	
	port_status = (struct lan_port_status *)status;
	
	printf("Link = %s\n",lan_link_spec[port_status->link]);	
	printf("Speed = %s\n",lan_speed_spec[port_status->speed]);
	printf("Nway mode = %s\n",enable_spec[port_status->nway]);	
	printf("Duplex = %s\n",enable_spec[port_status->duplex]);
		
}
void print_port_stats(char *stats)
{
	struct port_statistics *port_stats;
	port_stats = (struct port_statistics *)stats;

	printf("rx bytes=%d\n",port_stats->rx_bytes);
	printf("rx unicast packets =%d\n",port_stats->rx_unipkts);
	printf("rx multicast packets =%d\n",port_stats->rx_mulpkts);
	printf("rx brocast packets =%d\n",port_stats->rx_bropkts);
	printf("rx discard packets  =%d\n",port_stats->rx_discard);
	printf("rx error packets  =%d\n",port_stats->rx_error);
	printf("tx bytes=%d\n",port_stats->tx_bytes);
	printf("tx unicast packets =%d\n",port_stats->tx_unipkts);
	printf("tx multicast packets =%d\n",port_stats->tx_mulpkts);
	printf("tx brocast packets =%d\n",port_stats->tx_bropkts);
	printf("tx discard packets  =%d\n",port_stats->tx_discard);
	printf("tx error packets  =%d\n",port_stats->tx_error);	
}

void print_port_rate(char *rate,int cmd_len)
{
	struct port_rate *portRate;
	portRate = (struct port_rate *)rate;
	int index=0;
	for(index =0;index<cmd_len;index++)
	{
		portRate =rate+index;
		printf("[%d] port%d: rx rate=%d(%d)  tx rate=%d(%d)\n",index,portRate->port_id,
			portRate->rx_rate,portRate->rx_percent,portRate->tx_rate,portRate->tx_percent);
	}
	return;
}


