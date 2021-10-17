#include <stdio.h> 
#include <fcntl.h> 
#include <sys/types.h> 
#include <sys/uio.h> 
#include <unistd.h> 
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h> //mark_file

//#include "inband_if.h"
#include <netinet/in.h>  //mark_8021x

#if defined(CONFIG_APP_ADAPTER_API)
#include "../adapter-api/rtk_api/rtk_disk_adapter.h"
#include "../adapter-api/rtk_api/rtk_api.h"
#include "../adapter-api/rtk_api/rtk_firewall_adapter.h"

#define CONFIG_RTK_NAS_FILTER 1

#endif
#define HOST_SEND_CONFIG_FILE
#ifndef INBAND_GET_FILE_SUPPOPRT
#define INBAND_GET_FILE_SUPPOPRT
#endif
//#define HOST_NETIF  ("eth0")
#define HOST_NETIF  ("br0")
#define SLAVE_MAC ("ffffffffffff")
#define ETH_P_RTK		0x8899	// Realtek Remote Control Protocol (RRCP)
int hcd_inband_chan=0;

#ifdef INBANDHOST_PTHREAD_LOCK
pthread_mutex_t inband_mutex = PTHREAD_MUTEX_INITIALIZER;
static int pthread_mutex_init_flag = 0;
#endif
// command ID
#define id_set_mib						0x01
#define id_get_mib						0x02
#define id_sysinit							0x03
#define id_syscmd							0x04
#define id_trigger_wps					0x09
#ifdef HOST_SEND_CONFIG_FILE
#define id_send_config_file 					0x0c
#define id_get_config_file 					0x0d
#endif
#ifdef INBAND_GET_FILE_SUPPOPRT
#define id_get_file			0x0e
#endif
#define id_sync_to_server 0x0f
#if 1
#define id_getstainfo						0x0a
#define id_getassostanum					0x05
#define id_getbssinfo						0x06
#define id_getstats						0x07
#define id_getlanstatus					0x08

#define id_firm_upgrade						0x20
#define id_request_scan			0x21
#define id_get_scan_result		0x22
#if defined(CONFIG_RTL_LITE_CLNT)
#define id_start_lite_clnt_connect	0x23
#define id_wlan_sync	0x24
#define id_set_wlan_on 	0x25
#define id_set_wlan_off 0x26
#endif
#define id_getlanRate				0x10
#define id_getlanRatePercent		0x11
#define id_setlanBandwidth			0x12
#endif
#if defined(CONFIG_APP_ADAPTER_API)
#define id_get_storage_info		0x30
#define id_format_partition		0x31
#define id_get_wan_status		0x40
#define id_get_lan_terminal_info		0x41
#define id_get_upload_speed		0x42
#define id_get_download_speed	0x43
#define id_get_phy_port_status	0x44
#define id_get_lan_drop_rate	0x45
#define id_get_wan_drop_rate	0x46
#define id_get_wlan_drop_rate	0x51
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
#define id_get_qos_rule_monopoly	0x52
#define id_set_qos_rule_monopoly	0x53
#define id_set_qos_rule_monopoly_imm	0x54
#endif
#if defined(QOS_BY_BANDWIDTH)
#define id_add_qos_rule_imm		0x55
#define id_del_qos_rule_imm		0x56
#define id_get_qos_rule		0x57
#endif
#define id_restart_wan			0x58
#define id_get_pppoe_err_code	0x59
#define id_wlan_immediately_work 	0x5a
#if CONFIG_RTK_NAS_FILTER
#define id_enable_nas_filter     0x5b
#define id_get_status_nas_filter 0x5c
#define id_add_nas_filter_entry  0x5d
#define id_del_nas_filter_entry  0x5e
#define id_get_nas_filter_entry  0x5f
#endif
#define id_get_macfilter_rule		0x60
#define id_add_macfilter_rule_imm		0x61
#define id_del_macfilter_rule_imm		0x62

#define id_firm_check_sig_checksum	0x47
#define id_firm_check_flash_data	0x48
#define id_firm_upgrade_reboot		0x49

#define id_get_fw_version			0x50
#define FW_VERSION_MAX_LEN			24
#endif
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
#define id_set_rgb_blink_start		0x4a
#define id_set_rgb_blink_end		0x4b
#endif
// Command Table
struct inband_cmd_table_t {
	char cmd_id;
	const char *cmd;			// Input command string	
	int (*func)(int index,int argc, char *argv[]);
	void (*print_func)(char *para,char *data,int len);
	const char *msg;			// Help message
    void (*sprint_func)(char *para, char *data, int len);   //print result to buffer
};

typedef struct{
	unsigned int status;	//0:SUCCESS, 1:FAIL, 2:buf overflow
	unsigned int length;	//the buf length of return value 
	unsigned char error_info[64];
}RTK_INBAND_RESULT_HEADER,*pRTK_INBAND_RESULT_HEADER;

typedef struct{
	RTK_INBAND_RESULT_HEADER header;
	unsigned char *buf;
}RTK_INBAND_RESULT, *pRTK_INBAND_RESULT;
#define MAX_SSID_LEN			33
#define WEP128_KEY_LEN			13
#define WEP64_KEY_LEN			5
#define MAX_PSK_LEN			64
#define MAX_RS_PASS_LEN			65
#define MAX_RS_PASS_LEN			65
#define DWORD_SWAP(v) ( (((v&0xff)<<24)&0xff000000) | ((((v>>8)&0xff)<<16)&0xff0000) | \
				((((v>>16)&0xff)<<8)&0xff00) | (((v>>24)&0xff)&0xff) )
#define WORD_SWAP(v) ((unsigned short)(((v>>8)&0xff) | ((v<<8)&0xff00)))

struct wlan_8021x_mib {
	unsigned char encrypt;
	unsigned char ssid[MAX_SSID_LEN];
	unsigned char enable1X;
	unsigned char macAuthEnabled;
	unsigned char enableSuppNonWpa;
	unsigned char suppNonWpa;
	unsigned char wep;
	unsigned char wep128Key1[WEP128_KEY_LEN];
	unsigned char wep64Key1[WEP64_KEY_LEN];
	unsigned char wpaAuth;
	unsigned char wpaCipher;
	unsigned char wpa2Cipher;
	unsigned char wpa2PreAuth;
	unsigned char wpaPSKFormat;
	unsigned char wpaPSK[MAX_PSK_LEN+1];
	unsigned long wpaGroupRekeyTime;
	unsigned short rsPort;
	unsigned char rsIpAddr[4];
	unsigned char rsPassword[MAX_RS_PASS_LEN];
	unsigned char rsMaxRetry;
	unsigned short rsIntervalTime;
	unsigned char accountRsEnabled;
	unsigned short accountRsPort;
	unsigned char accountRsIpAddr[4];
	unsigned char accountRsPassword[MAX_RS_PASS_LEN];
	unsigned char accountRsUpdateEnabled;
	unsigned short accountRsUpdateDelay;
	unsigned char accountRsMaxRetry;
	unsigned short accountRsIntervalTime;
};

static void __inline__ WRITE_WPA_FILE(int fh, unsigned char *buf)
{
        if ( write(fh, buf, strlen(buf)) != strlen(buf) ) {
                printf("Write WPA config file error!\n");
                close(fh);
                exit(1);
        }
}

typedef enum _wlan_mac_state {
    STATE_DISABLED=0, STATE_IDLE, STATE_SCANNING, STATE_STARTED, STATE_CONNECTED, STATE_WAITFORKEY
} wlan_mac_state;

typedef enum { BAND_11B=1, BAND_11G=2, BAND_11BG=3, BAND_11A=4, BAND_11N=8, BAND_11AC=64 } BAND_TYPE_T;

#define SSID_LEN	32
/* flag of sta info */
#define STA_INFO_FLAG_AUTH_OPEN     	0x01
#define STA_INFO_FLAG_AUTH_WEP      	0x02
#define STA_INFO_FLAG_ASOC          	0x04
#define STA_INFO_FLAG_ASLEEP        	0x08

typedef struct wlan_bss_info {
    unsigned char state;
    unsigned char channel;
    unsigned char txRate;
    unsigned char bssid[6];
    unsigned char rssi, sq;	// RSSI  and signal strength
    unsigned char ssid[SSID_LEN+1];
} WLAN_BSS_INFO_T, *WLAN_BSS_INFO_Tp;

typedef struct wlan_sta_info {
	unsigned short	aid;
	unsigned char	addr[6];
	unsigned long	tx_packets;
	unsigned long	rx_packets;
	unsigned long	expired_time;	// 10 msec unit
	unsigned short	flag;
	unsigned char	txOperaRates;
	unsigned char	rssi;
	unsigned long	link_time;		// 1 sec unit
	unsigned long	tx_fail;
	unsigned long tx_bytes;
	unsigned long rx_bytes;
	unsigned char network;
	unsigned char ht_info;	// bit0: 0=20M mode, 1=40M mode; bit1: 0=longGI, 1=shortGI
	unsigned char 	resv[6];
} WLAN_STA_INFO_T, *WLAN_STA_INFO_Tp;

typedef struct wlan_rate{
	unsigned int id;
	unsigned char rate[20];
}WLAN_RATE_T, *WLAN_RATE_Tp;

typedef enum { 
	MCS0=0x80, 
	MCS1=0x81, 
	MCS2=0x82,
	MCS3=0x83,
	MCS4=0x84,
	MCS5=0x85,
	MCS6=0x86,
	MCS7=0x87,
	MCS8=0x88,
	MCS9=0x89,
	MCS10=0x8a,
	MCS11=0x8b,
	MCS12=0x8c,
	MCS13=0x8d,
	MCS14=0x8e,
	MCS15=0x8f
	} RATE_11N_T;

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

struct lan_port_status {
    unsigned char link;
    unsigned char speed;
    unsigned char duplex;
    unsigned char nway;    	
}; 

struct port_statistics {
	unsigned int  rx_bytes;
	unsigned int  rx_unipkts;
	unsigned int  rx_mulpkts;
	unsigned int  rx_bropkts;
	unsigned int  rx_discard;
	unsigned int  rx_error;
	unsigned int  tx_bytes;
	unsigned int  tx_unipkts;
	unsigned int  tx_mulpkts;
	unsigned int  tx_bropkts;
	unsigned int  tx_discard;
	unsigned int  tx_error;
};
struct port_rate {
	unsigned int  port_id;      // Port ID
	unsigned int  rx_rate;      // Port RX rate
	unsigned int  tx_rate;      // Port TX rate
	unsigned int  rx_percent;   // Port RX bandwidth used percentage
	unsigned int  tx_percent;   // Port TX bandwidth used percentage
};

char *lan_link_spec[2] = {"DOWN","UP"};
char *lan_speed_spec[3] = {"10M","100M","1G"};
char *enable_spec[2] = {"DIsable","Enable"};

/*firmware define*/
#define __PACK__                        __attribute__ ((packed))

#define SIGNATURE_LEN                   4
#define FW_HEADER_WITH_ROOT     ((char *)"cr6c")
#define FW_HEADER                       ((char *)"cs6c")
#define ROOT_HEADER                     ((char *)"r6cr")
#define WEB_HEADER                      ((char *)"w6cg")

#define INBAND_WPS_CONFIG ("/var/wps.conf")
#define WLAN_ROOT ("wlan0")

enum { 
		MODE_AP_UNCONFIG=1, 			// AP unconfigured (enrollee)
		MODE_CLIENT_UNCONFIG, 		// client unconfigured (enrollee) 
		MODE_CLIENT_CONFIG,			// client configured (registrar) 
		MODE_AP_PROXY, 				// AP configured (proxy)
		MODE_AP_PROXY_REGISTRAR,		// AP configured (proxy and registrar)
		MODE_CLIENT_UNCONFIG_REGISTRAR		// client unconfigured (registrar)
};

enum { AP_MODE=0, CLIENT_MODE=1 };

enum { AUTH_OPEN=0, AUTH_SHARED, AUTH_BOTH };
enum { ENCRYPT_DISABLED=0, ENCRYPT_WEP=1, ENCRYPT_WPA=2, ENCRYPT_WPA2=4, ENCRYPT_WPA2_MIXED=6 ,ENCRYPT_WAPI=7 };
enum { WSC_AUTH_OPEN=1, WSC_AUTH_WPAPSK=2, WSC_AUTH_SHARED=4, WSC_AUTH_WPA=8, WSC_AUTH_WPA2=0x10, WSC_AUTH_WPA2PSK=0x20, WSC_AUTH_WPA2PSKMIXED=0x22 };
enum { WSC_ENCRYPT_NONE=1, WSC_ENCRYPT_WEP=2, WSC_ENCRYPT_TKIP=4, WSC_ENCRYPT_AES=8, WSC_ENCRYPT_TKIPAES=12 };
enum { WPA_CIPHER_TKIP=1, WPA_CIPHER_AES=2, WPA_CIPHER_MIXED=3 };

typedef struct img_header {
        unsigned char signature[SIGNATURE_LEN] __PACK__;
        unsigned int startAddr __PACK__;
        unsigned int burnAddr __PACK__;
        unsigned int len __PACK__;
} IMG_HEADER_T, *IMG_HEADER_Tp;


//functions
#if 1
static void print_stainfo(char *para,char *data,int len);
static void print_getassostanum(char *para,char *data,int len);
static void print_bssinfo(char *para,char *data,int len);
static void print_port_stats(char *para,char *data,int len);
static void print_port_status(char *para,char *data,int len);
static void print_port_rate(char *para,char *data,int len);
static void print_firm_check_sig_checksum(char *para,char *data,int len);
static void print_firm_check_flash_data(char *para,char *data,int len);
#if defined(CONFIG_APP_ADAPTER_API)
static void print_get_pppoe_err_code(char *para,char *data,int len);
static void print_fw_version(char *para,char *data,int len);
#endif

static int cmd_firmware_upgrade(int index,int argc, char *argv[]);
static int cmd_firm_check_sig_checksum(int index,int argc, char *argv[]);
static int cmd_firm_check_flash_data(int index,int argc, char *argv[]);
static int cmd_firm_upgrade_reboot(int index,int argc, char *argv[]);
#endif

static void print_cfgread(char *para,char *data,int len);
static void print_syscmd(char *para,char *data,int len);

 static int cmd_write(int index,int argc, char *argv[]);
 static int cmd_read(int index,int argc, char *argv[]);
 static int cmd_action(int index,int argc, char *argv[]);
 static int cmd_sendconf(int index,int argc, char *argv[]);
#ifdef HOST_SEND_CONFIG_FILE
static int cmd_send_config_file(int index,int argc, char *argv[]);
static int cmd_get_config_file(int index,int argc, char *argv[]);
#endif
#ifdef INBAND_GET_FILE_SUPPOPRT
static int cmd_get_file(int index,int argc, char *argv[]);
#endif
static int cmd_sync_to_server(int index,int argc, char *argv[]);
static int cmd_trigger_wlan_action(int index,int argc, char *argv[]);
static void print_scan_result(char *para,char *data,int len);
#if defined(CONFIG_APP_ADAPTER_API)
static void print_storage_info(char *para,char *data,int len);
static void print_format_partition(char *para,char *data,int len);
int disk_cmd_read(int index,int argc, char *argv[]);
#if CONFIG_RTK_NAS_FILTER
static void print_nas_filter_enable(char *para,char *data,int len);
static void print_nas_filter_status(char *para,char *data,int len);
static void print_nas_filter_entry(char *para,char *data,int len);
static void print_add_nas_filter_entry(char *para,char *data,int len);
static void print_del_nas_filter_entry(char *para,char *data,int len);
#endif
static void print_wan_status(char *para,char *data,int len);
static void print_lan_terminal_info(char *para,char *data,int len);
static void print_upload_speed(char *para,char *data,int len);
static void print_download_speed(char *para,char *data,int len);
static void print_phyPort_status(char *para,char *data,int len);
static void print_lan_dropRate(char *para,char *data,int len);
static void print_wan_dropRate(char *para,char *data,int len);
static void print_wlan_dropRate(char *para,char *data,int len);
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
static void print_qos_mnp_rule(char *para,char *data,int len);
int qos_mnp_cmd_write(int index,int argc, char *argv[]);
#endif
#if defined (QOS_BY_BANDWIDTH)
static void print_qos_rule(char *para,char *data,int len);
int qos_rule_cmd_write(int index,int argc, char *argv[]);
#endif
int wan_cmd_read(int index,int argc, char *argv[]);
int lanwan_status_cmd_read(int index,int argc, char *argv[]);
int lan_cmd_read(int index,int argc, char *argv[]);
int upload_cmd_read(int index,int argc, char *argv[]);
int download_cmd_read(int index,int argc, char *argv[]);
#if CONFIG_RTK_NAS_FILTER
int nas_filter_enable(int index, int argc, char *argv[]);
int get_nas_filter_status(int index, int argc, char *argv[]);
int get_entry_nas_filter(int index, int argc, char *argv[]);
int add_entry_nas_filter(int index, int argc, char *argv[]);
int del_entry_nas_filter(int index, int argc, char *argv[]);
#endif
static void print_macfilter_rule(char *para,char *data,int len);
#endif
static int host_inband_write(char cmd_id,char *data,int data_len);
 static int cmd_trigger_wps(int index,int argc, char *argv[]);

 static int cmd_syscmd(int index,int argc, char *argv[]);

#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
static int rgb_led_blink_start(int index,int argc, char *argv[]);
static int rgb_led_blink_end(int index,int argc, char *argv[]);
#endif
unsigned int host_inband_seq=0; //use to do requset <-> response check

#define WRITE_WSC_PARAM(dst, tmp, str, val, len) {   \
        sprintf(tmp, str, val); \
        memcpy(dst, tmp, strlen(tmp)); \
        dst += strlen(tmp); \
        len += strlen(tmp); \
}


struct inband_cmd_table_t inband_cmd_table[]=
{
    //8198 config command	
    {id_set_mib,"set_mib", cmd_write,NULL,   "inband set_mib mibA=valA", NULL},
    {id_get_mib,"get_mib",     cmd_read, print_cfgread,  "inband get_mib mibA",  NULL},
    {id_sysinit,"sysinit",     cmd_action,  NULL, "inband sysinit all/wlan/lan", NULL},

    {id_syscmd,"syscmd",     cmd_syscmd, print_syscmd,  "inband syscmd \'cmd_string\'", NULL},
//  {id_hostcmd_only,"send_conf",     cmd_sendconf,   NULL,"inband send_conf /etc/xxx.conf", NULL}, 
#ifdef HOST_SEND_CONFIG_FILE
    {id_send_config_file,"send_config_file",     cmd_send_config_file,   NULL,"inband send_config_file xxx.txt", NULL},
    {id_get_config_file,"get_config_file",     cmd_get_config_file,   NULL,"inband get_config_file all/wlan/hw", NULL},
#endif    
#ifdef INBAND_GET_FILE_SUPPOPRT
	{id_get_file,"get_file",     cmd_get_file,   NULL,"inband get_file filePath dstPath", NULL},
#endif
	{id_sync_to_server,"syncToSer",     cmd_sync_to_server,   NULL,"inband syncToSer [name] [value]", NULL},
    {id_trigger_wps,"trigger_wps",     cmd_trigger_wps,   NULL,"inband trigger_wps pin xxxxxxxx/pbc", NULL}, 
  #if 1
    {id_getstainfo,"getstainfo",     cmd_read,  print_stainfo, "inband getstainfo wlan_interface", NULL},
    {id_getassostanum,"getassostanum",  cmd_read , print_getassostanum ,  "inband getassostanum wlan_interface", NULL},
    {id_getbssinfo,"getbssinfo",     cmd_read, print_bssinfo,  "inband getbssinfo wlan_interface", NULL},
    {id_getstats,"getstats",     cmd_read, print_port_stats,  "inband getstats p1/p2/p3/p4/p5/wlan0", NULL},
    {id_getlanstatus,"getlanstatus",     cmd_read,   print_port_status,"inband getlanstatus p1/p2/p3/p4/p5", NULL},    
  	{id_getlanRate,"getlanRate",	   cmd_read,   print_port_rate,"inband getlanRate p0/p1/p2/p3/all", NULL},    
  	{id_setlanBandwidth,"setlanBandwidth",cmd_write,	 NULL, "inband setlanBandwidth p0/p1/p2/p3/all=bwA", NULL},	  
  	{id_firm_upgrade,"firm_upgrade",     cmd_firmware_upgrade,   NULL,"inband firm_upgrade /var/fw.bin ''/wait_burn", NULL}, 
#if defined(CONFIG_APP_ADAPTER_API)
  	{id_firm_check_sig_checksum,"firm_check_sig_checksum",cmd_firm_check_sig_checksum,print_firm_check_sig_checksum,"inband firm_check_sig_checksum",NULL},
  	{id_firm_check_flash_data,"firm_check_flash_data",cmd_firm_check_flash_data,print_firm_check_flash_data,"inband firm_check_flash_data",NULL},
	{id_firm_upgrade_reboot,"firm_upgrade_reboot",cmd_firm_upgrade_reboot,NULL,"inband firm_upgrade_reboot",NULL},
#endif
	{id_request_scan,"request_scan",     cmd_trigger_wlan_action,   NULL,"inband request_scan wlan_interface", NULL},
	{id_get_scan_result,"get_scan_result",     cmd_read,  print_scan_result, "inband get_scan_result wlan_interface", NULL},
#if defined(CONFIG_RTL_LITE_CLNT)
	{id_start_lite_clnt_connect,"start_lite_clnt_connect", cmd_trigger_wlan_action,  NULL, "inband start_lite_clnt_connect wlan_interface", NULL},
	{id_wlan_sync,"wlan_sync", cmd_trigger_wlan_action,  NULL, "inband wlan_sync wlan_interface", NULL},
	{id_set_wlan_on,"set_wlan_on", cmd_trigger_wlan_action,  NULL, "inband set_wlan_on wlan_interface", NULL},
	{id_set_wlan_off,"set_wlan_off", cmd_trigger_wlan_action,  NULL, "inband set_wlan_off wlan_interface", NULL},
#endif
    //{id_sendfile,"sendfile",     cmd_sendfile,   NULL,"inband sendfile /usr/linux.bin"}, 
  #endif
	#if defined(CONFIG_APP_ADAPTER_API)
    {id_get_storage_info,"get_storage_info",disk_cmd_read,	print_storage_info, "inband get_storage_info all", NULL},
    {id_format_partition,"format_partition",disk_cmd_read,  print_format_partition, "inband format_partition sdx(eg:sda1) filesystemtype(eg:ntfs)", NULL},
	{id_get_wan_status,"getWanStatusByUrl",wan_cmd_read,  print_wan_status, "inband getWanStatusByUrl URL(eg.www.baidu.com)", NULL},
	{id_get_lan_terminal_info,"getLanStatus",lanwan_status_cmd_read,  print_lan_terminal_info, "inband getLanStatus", NULL},	
	{id_get_upload_speed,"getUploadSpeed",lanwan_status_cmd_read,  print_upload_speed, "inband getUploadSpeed", NULL},		
	{id_get_download_speed,"getDownloadSpeed",lanwan_status_cmd_read,  print_download_speed, "inband getDownloadSpeed", NULL},		
	{id_get_phy_port_status,"getPhyPortStatus",lanwan_status_cmd_read,	print_phyPort_status, "inband getPhyPortStatus", NULL},	
	{id_get_lan_drop_rate,"getLanDropRate",lanwan_status_cmd_read,	print_lan_dropRate, "inband getLanDropRate", NULL}, 	
	{id_get_wan_drop_rate,"getWanDropRate",lanwan_status_cmd_read, print_wan_dropRate, "inband getWanDropRate", NULL},
	{id_get_wlan_drop_rate,"getWlanDropRate",cmd_read, print_wlan_dropRate, "inband getWlanDropRate wlan0/wlan1", NULL}, 	
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
	{id_get_qos_rule_monopoly,"getQosMnpRule",cmd_read, print_qos_mnp_rule, "inband getQosMnpRule", NULL},	
	{id_set_qos_rule_monopoly,"setQosMnpRule",qos_mnp_cmd_write, NULL, "inband setQosMnpRule enabled macAddr time(eg: 1 000000001122 120)", NULL},	
	{id_set_qos_rule_monopoly_imm,"setQosMnpRuleImm",qos_mnp_cmd_write, NULL, "inband setQosMnpRuleImm enabled macAddr time(eg: 1 000000001122 120)", NULL},	
#endif
#if defined (QOS_BY_BANDWIDTH)
	{id_get_qos_rule,"getQosRule",cmd_read, print_qos_rule, "inband getQosRule mac macAddr/all", NULL},	
	{id_add_qos_rule_imm,"addQosRuleImm",qos_rule_cmd_write, NULL, "inband addQosRuleImm mac macAddr upbw downbw", NULL},	
	{id_del_qos_rule_imm,"delQosRuleImm",qos_rule_cmd_write, NULL, "inband delQosRuleImm mac macAddr/all", NULL},	
#endif
	{id_restart_wan,"restartWan",cmd_write, NULL, "inband restartWan", NULL}, 
	{id_get_pppoe_err_code,"getPppoeErrCode",cmd_read, print_get_pppoe_err_code, "inband getPppoeErrCode", NULL}, 	
	{id_wlan_immediately_work,"wlanImmediatelyWork",cmd_write, NULL, "inband wlanImmediatelyWork wlan0/wlan1", NULL}, 	
#endif	
#if defined(CONFIG_APP_ADAPTER_API)
	{id_get_fw_version, "getFwVersion", lanwan_status_cmd_read, print_fw_version, "inband getFwVersion", NULL},
#endif
#if CONFIG_RTK_NAS_FILTER
    {id_enable_nas_filter, "enableNasFilter", nas_filter_enable, print_nas_filter_enable, "inband enableNasFilter 1/0", NULL},
    {id_get_status_nas_filter, "getNasFilterStatus", get_nas_filter_status, print_nas_filter_status, "inband getNasFilterStatus", NULL},
    {id_get_nas_filter_entry, "getNasFilterEntry", get_entry_nas_filter, print_nas_filter_entry, "inband getNasFilterEntry", NULL},
    {id_add_nas_filter_entry, "addNasFilterEntry", add_entry_nas_filter, print_add_nas_filter_entry, "inband addNasFilterEntry mac:(001122334455) [comments]", NULL},
    {id_del_nas_filter_entry, "delNasFilterEntry", del_entry_nas_filter, print_del_nas_filter_entry, "inband delNasFilterEntry mac(e.g:001122334455) comment/all", NULL},
#endif
#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
	{id_set_rgb_blink_start,"setRGBBlinkStart",rgb_led_blink_start, NULL, "inband setRGBBlinkStart blink_mode interval", NULL}, 	
	{id_set_rgb_blink_end,"setRGBBlinkEnd",rgb_led_blink_end, NULL, "inband setRGBBlinkEnd", NULL}, 	
#endif
#if defined(CONFIG_APP_ADAPTER_API)
	{id_get_macfilter_rule,"getMacfilterRule",cmd_read, print_macfilter_rule, "inband getMacfilterRule mac macAddr/all", NULL}, 
	{id_add_macfilter_rule_imm,"addMacfilterRuleImm",cmd_write, NULL, "inband addMacfilterRuleImm mac macAddr ", NULL},	
	{id_del_macfilter_rule_imm,"delMacfilterRuleImm",cmd_write, NULL, "inband delMacfilterRuleImm mac macAddr/all", NULL},	
#endif
	{NULL,  NULL, NULL},
};


unsigned int get_random(int max)
{
        struct timeval tod;

        gettimeofday(&tod , NULL);
        srand(tod.tv_usec);
        return rand()%max;
}

void print_command_list(void)
{
    int i;

    printf("\n==========commands for debugging============\n");
    i = 0;
    while (inband_cmd_table[i].cmd != NULL) {         
            printf("%s\n", inband_cmd_table[i].msg);
        i++;
	}   
}
static void print_scan_result(char *para,char *data,int len)
{
	printf("%s",data);
}
#if defined(CONFIG_APP_ADAPTER_API)
int dist_get_cmd_timeout(unsigned int cmd, int argc, char *argv[])
{
	int timeout = 5000;
	switch (cmd)
	{
		case id_get_storage_info:
			timeout = 10000; 
			break;
		case id_format_partition:
			if (!strncmp(argv[3], "ext", strlen("ext")))
				timeout = 2000000; //fix me? format ext2/3/4 take long time.....
			else
				timeout = 50000;
			break;
		default:
			break;
	}
	return timeout;
}
int disk_cmd_read(int index,int argc, char *argv[])
{
    int len,ret,count;
    char data[1480];	
    char cmd_id,rx_cmd_type;
    char *buf_p;	
   unsigned int tx_seq,rx_seq;	
   unsigned int timeout = 0;
   
   cmd_id = inband_cmd_table[index].cmd_id;
  	memset(data, '\0', sizeof(data));
	if(argc > 3) 
	  	sprintf(data,"%s %s",argv[2],argv[3]); 
	else
	  	sprintf(data,"%s",argv[2]); 
   len = strlen(data);
   ret = host_inband_write(cmd_id,data,len); //send request
    tx_seq = host_inband_seq;
   if(ret < 0)
   	return -1;
   timeout = dist_get_cmd_timeout(cmd_id, argc, argv);
   count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,timeout); //return data length 
   if(count < 0)
	 	return -1;
   if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
   {
	if(inband_cmd_table[index].print_func)
		inband_cmd_table[index].print_func(data,buf_p,count);
   }
   else
	return -1;
   return 0;
}
static void print_storage_info(char *para,char *data,int len)
{
	RTK_DEVICEINFO_T *pinfo;
	unsigned int i = 0, j = 0, par_num = 0;
	int num = len/sizeof(RTK_DEVICEINFO_T);
	pinfo = (RTK_DEVICEINFO_T *)data;
	for (i = 0; i < num; i++)
	{
		if (pinfo[i].name[0])//exist
		{
			printf("\ndevice name: %s partition number: %d \n", pinfo[i].name, pinfo[i].number);
			printf("total size: %lld bytes used size: %lld bytes free size: %lld bytes \n", pinfo[i].total, pinfo[i].used, pinfo[i].free);
			par_num = pinfo[i].number;
			par_num = (par_num >=MAX_LIST_PARTITION_NUMBER)?MAX_LIST_PARTITION_NUMBER:par_num;
			printf("\npartition information:\n");
			for (j = 0; j < par_num; j++)
			{
				printf("partition name: %s partition index: %d filesystem type: %d \n", pinfo[i].partition_info[j].name, pinfo[i].partition_info[j].index, pinfo[i].partition_info[j].systype);
				printf("label: %s uuid: %s \n", pinfo[i].partition_info[j].label, pinfo[i].partition_info[j].uuid);
				printf("total size: %lld bytes used size: %lld bytes free size: %lld bytes \n", pinfo[i].partition_info[j].total, pinfo[i].partition_info[j].used, pinfo[i].partition_info[j].free);
				printf("\n");
			}
			printf("\n");
		}
	}
}
static void print_format_partition(char *para,char *data,int len)
{
	printf("%s\n",data);
}
int wan_cmd_read(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;	
    unsigned int tx_seq,rx_seq;	
    unsigned int timeout = 0;
   	cmd_id = inband_cmd_table[index].cmd_id;
	memset(data, '\0', sizeof(data));
	sprintf(data,"%s",argv[2]); 
	if(argc >3)
		printf("input error,pleast input again\n");
    len = strlen(argv[2]);	//get url string
   
    ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
	
	//printf("%s %d ret=%d \n", __FUNCTION__, __LINE__, ret);
	
   if(ret < 0)
		return -1;
   
   //count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
   timeout = 5000;
   //printf("%s %d timeout=%d \n", __FUNCTION__, __LINE__, timeout);
   count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,timeout); //return data length 
   //printf("%s %d count=%d \n", __FUNCTION__, __LINE__, count);
   if(count < 0)
		return -1;
   //printf("%s %d rx_cmd_type=%d cmd_id=%d tx_seq=%u rx_seq=%u \n", __FUNCTION__, __LINE__, rx_cmd_type, cmd_id, tx_seq, rx_seq);
   if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
   {
		if(inband_cmd_table[index].print_func)
			inband_cmd_table[index].print_func(data,buf_p,count);
   }
   else
	return -1;

   return 0;
	
}

int lanwan_status_cmd_read(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;	
   unsigned int tx_seq,rx_seq;	
   unsigned int timeout = 0;
   
   cmd_id = inband_cmd_table[index].cmd_id;
	memset(data, '\0', sizeof(data));

	#if 0
	if(argc > 3) 
		sprintf(data,"%s %s",argv[2],argv[3]); 
	else
		sprintf(data,"%s",argv[2]); 
	#endif
	
   len = strlen(data);
   
   ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
	
	//printf("%s %d ret=%d \n", __FUNCTION__, __LINE__, ret);
	
   if(ret < 0)
	return -1;
	
   //count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
   timeout = 5000;
   //printf("%s %d timeout=%d \n", __FUNCTION__, __LINE__, timeout);
   count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,timeout); //return data length 
   //printf("%s %d count=%d \n", __FUNCTION__, __LINE__, count);
   if(count < 0)
		return -1;
   //printf("%s %d rx_cmd_type=%d cmd_id=%d tx_seq=%u rx_seq=%u \n", __FUNCTION__, __LINE__, rx_cmd_type, cmd_id, tx_seq, rx_seq);
   if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
   {
		if(inband_cmd_table[index].print_func){
			inband_cmd_table[index].print_func(data,buf_p,count);
		}
   }
   else
	return -1;

   return 0;
	
}

static void print_wan_status(char *para,char *data,int len)
{
	if(len == sizeof(int))
	{
		if(*((unsigned int*)data) == 1)			
			printf("Wan is connected\n");
		else
			printf("Wan isn't connected\n");
	}
	else if(strncmp(data,"error",sizeof("error"))==0)
	{
		printf("Get Wan Status Error\n");
	}
	else	
		printf("%s\n",data);
}
static void print_lan_terminal_info(char *para,char *data,int len)
{
	int i;
	int term_number;// = MAX_TERM_NUMBER; 
	struct rtk_link_type_info *info = (struct rtk_link_type_info *)data;
	term_number = len/(sizeof(struct rtk_link_type_info));
	for(i = 0 ; i < term_number; ++i)
	{
		printf("\n***********************************\n");
		printf("LINK_TYPE(%d),ipAddr(%s),mac(%02x:%02x:%02x:%02x:%02x:%02x),\
			last_rx_bytes(%u),last_tx_bytes(%u),cur_rx_bytes(%u),cur_tx_bytes(%u),\
			port_number(%d),download_speed(%u),upload_speed(%u),all_bytes(%u),brand(%s),link_time(%u)\n",
			info[i].type,inet_ntoa(info[i].ip),
			info[i].mac[0],info[i].mac[1],info[i].mac[2],
			info[i].mac[3],info[i].mac[4],info[i].mac[5],
			info[i].last_rx_bytes,info[i].last_tx_bytes,
			info[i].cur_rx_bytes,info[i].cur_tx_bytes,
			info[i].port_number,info[i].download_speed,info[i].upload_speed,
			info[i].all_bytes,info[i].brand,info[i].link_time);
		printf("\n***********************************\n");		
	}
}
static void print_upload_speed(char *para,char *data,int len)
{
	RTK_UPLOAD_STATICS *info = (RTK_UPLOAD_STATICS*)data;
	printf("avg_upload_speed(%d)max_upload_speed(%d),min_upload_speed(%d),	\
			cur_upload_speed(%d),upload_total(%d)\n",	\
			info->avg_upload_speed,
			info->max_upload_speed,
			info->min_upload_speed,
			info->cur_upload_speed,
			info->upload_total
			);
}
static void print_download_speed(char *para,char *data,int len)
{
	RTK_DOWNLOAD_STATICS *info = (RTK_DOWNLOAD_STATICS*)data;
	printf("avg_download_speed(%d)max_download_speed(%d),min_download_speed(%d),	\
			cur_download_speed(%d),download_total(%d)\n",	\
			info->avg_download_speed,
			info->max_download_speed,
			info->min_download_speed,
			info->cur_download_speed,
			info->download_total
			);
}
static void print_phyPort_status(char *para,char *data,int len)
{
	int i;
	int port_number = MAX_PORT_NUMBER; 
	struct rtk_port_status *info = (struct rtk_port_status *)data;
	char * status_str[]={"LINK_ERROR","LINK_UP","LINK_DOWN"};
	for(i = 0 ; i < port_number; ++i)
	{
		printf("port number(%d),port status(%s)\n",
			info[i].port_number,status_str[(int)(info[i].port_status)]);
	}
}
static void print_lan_dropRate(char *para,char *data,int len)
{
	int rate ;
	rate = *((int*)data);
	printf("lan drop rate(%d%%)\n",rate);
//	printf("%s\n",data);
}

static void print_wan_dropRate(char *para,char *data,int len)
{
	int rate ;
	rate = *((int*)data);
	printf("wan drop rate(%d%%)\n",rate);

//	printf("%s\n",data);
}

static void print_wlan_dropRate(char *para,char *data,int len)
{
	int rate ;
	rate = *((int*)data);
	printf("wlan drop rate(%d%%)\n",rate);

//	printf("%s\n",data);
}

static void print_fw_version(char *para,char *data,int len)
{
	if(strlen(data) > FW_VERSION_MAX_LEN)
		printf("wrong version number, can't be larger than 24 bytes\n");
	else
		printf("%s\n", data);
}
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)

static void print_qos_mnp_rule(char *para,char *data,int len)
{
	
	RTK_QOSMNP_INFO * info;
	info= (RTK_QOSMNP_INFO *)data;

	printf("qos_mnp_rule: enabled(%d) mac(%x:%x:%x:%x:%x:%x) qosTime(%d)\n",info->enabled,
	info->macAddr[0],info->macAddr[1],info->macAddr[2],info->macAddr[3],info->macAddr[4],info->macAddr[5],info->qosTime);
	
	
}

int qos_mnp_cmd_write(int index,int argc, char *argv[])
{
	int len,ret,count;
	unsigned char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;
	unsigned int tx_seq,rx_seq;	 	
	int i=0,datalen=0;
	cmd_id = inband_cmd_table[index].cmd_id;

	if(argc > 3) 
	{
		for(i=2;i<argc;i++)
			datalen+=sprintf(data+datalen,"%s ",argv[i]); 
	}	
	else if(argc == 3) 
		sprintf(data,"%s",argv[2]); 
	else
		data[0] = '\0';
	len = strlen(data);		

	//printf("argc:%d,%s\n",argc,data);
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;	

	if(ret < 0)
		return -1;

	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,2000); //return data length 	
	if(count < 0)
	 	return -1;

	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
		printf("remote write qos mnp ok!\n");
	else
	{
		return -1;
		printf("remote write qos mnp fail!\n");
	}		
	return 0;
}


#endif
#if defined (QOS_BY_BANDWIDTH)

static void print_qos_rule(char *para,char *data,int len)
{
	unsigned int ruleNum=0;
	unsigned int i;
	RTK_IPQOS_T *ruleInfo=NULL;
	ruleNum =data[0];	
	#if 1
	printf("ruleNum:%d,len:%d.[%s]:[%d].\n",ruleNum,len,__FUNCTION__,__LINE__);
	for(i=0;i<len;i++)
	{
		printf("%x ",data[i]);
		if(((i+1)%8)==0)
			printf("\n");
	}
	#endif

	for(i=0;i<ruleNum;i++)
	{
		ruleInfo = (RTK_IPQOS_T *)(&data[i*sizeof(RTK_IPQOS_T)+1]);
		if(ruleInfo){
			printf("[%d] enable[%d] mode[%d] mac[%2x:%2x:%2x:%2x:%2x:%2x] upbw[%dkbps] downbw[%dkbps]\n",i+1,
			ruleInfo->enabled,ruleInfo->mode,ruleInfo->mac[0],ruleInfo->mac[1],
			ruleInfo->mac[2],ruleInfo->mac[3],ruleInfo->mac[4],ruleInfo->mac[5],
			ruleInfo->bandwidth,ruleInfo->bandwidth_downlink);
		}
	}
	
}

int qos_rule_cmd_write(int index,int argc, char *argv[])
{
	int len,ret,count;
	unsigned char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;
	unsigned int tx_seq,rx_seq; 	
	int i=0,datalen=0;
	cmd_id = inband_cmd_table[index].cmd_id;

	if(argc > 3) 
	{
		for(i=2;i<argc;i++)
			datalen+=sprintf(data+datalen,"%s ",argv[i]); 
	}	
	else if(argc == 3) 
		sprintf(data,"%s",argv[2]); 
	else
		data[0] = '\0';
	len = strlen(data); 	

	//printf("argc:%d,%s\n",argc,data);
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;	

	if(ret < 0)
		return -1;

	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,3000); //return data length 	
	if(count < 0)
		return -1;

	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
		printf("remote write qos rule ok!\n");
	else
	{
		return -1;
		printf("remote write qos rule fail!\n");
	}		
	return 0;
}
#endif
static void print_macfilter_rule(char *para,char *data,int len)
{
	unsigned int ruleNum=0;
	unsigned int i;
	RTK_MACFILTER_T *ruleInfo=NULL;
	ruleNum =data[0];	
	#if 1
	printf("---ruleNum:%d,len:%d.[%s]:[%d].\n",ruleNum,len,__FUNCTION__,__LINE__);
	for(i=0;i<len;i++)
	{
		printf("%02x ",data[i]);
		if(((i+1)%8)==0)
			printf("\n");
	}
	#endif

	for(i=0;i<ruleNum;i++)
	{
		ruleInfo = (RTK_MACFILTER_T *)(&data[i*sizeof(RTK_MACFILTER_T)+1]);
		if(ruleInfo){
			printf("[%d] mac[%2x:%2x:%2x:%2x:%2x:%2x] comment:%s\n",i+1,
			ruleInfo->macAddr[0],ruleInfo->macAddr[1],	ruleInfo->macAddr[2],
			ruleInfo->macAddr[3],ruleInfo->macAddr[4],ruleInfo->macAddr[5],
			ruleInfo->comment);
		}
	}
	
}

#if CONFIG_RTK_NAS_FILTER
static void print_nas_filter_enable(char *para,char *data,int len)
{
    // do nothing
}
static void print_nas_filter_status(char *para,char *data,int len)
{
    printf("[%s:%d] data=%s\n", __FUNCTION__, __LINE__, data);
    if(!strcmp(data, "1")){
        printf("Nas filter enabled\n");
    }else{
        printf("Nas filter disabled\n");
    }
}
static void print_nas_filter_entry(char *para,char *data,int len)
{
    int num = len/sizeof(RTK_NASFILTER_T);
    int i=0;
    RTK_NASFILTER_Tp entry;

    if(num==0)
    {
        printf("Nas filter is empty\n");
        return;
    }
    for(i=0; i<num; i++)
    {
        entry = (RTK_NASFILTER_Tp)(data + i*sizeof(RTK_NASFILTER_T));
        printf("Nas filter[%d]:\n", i);
        printf("\tMAC address:%02x:%02x:%02x:%02x:%02x:%02x\n", entry->macAddr[0], entry->macAddr[1],entry->macAddr[2],entry->macAddr[3],entry->macAddr[4],entry->macAddr[5]);
        printf("\tComment:%s\n", entry->comment);
    }
}
static void print_add_nas_filter_entry(char *para,char *data,int len)
{
    printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, data);
}
static void print_del_nas_filter_entry(char *para,char *data,int len)
{
    printf("[%s:%d] %s\n", __FUNCTION__, __LINE__, data);
}
int nas_filter_enable(int index, int argc, char *argv[])
{
    char data[64] = {0};
    int len, ret;
	unsigned int tx_seq,rx_seq, count;
	char cmd_id,rx_cmd_type;
    char *buf_p;

    if(argc==3){
        sprintf(data, "%s", argv[2]);
        len = strlen(data);
    }else
        return -1;

	cmd_id = inband_cmd_table[index].cmd_id;
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;	
	if(ret < 0)
		return -1;

	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,3000); //return data length 	
	if(count < 0)
		return -1;

	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
		printf("set nas filter status ok!\n");
	else
	{
		printf("set nas filter status fail!\n");
		return -1;
	}		
	return 0;
}// end set enable

int get_nas_filter_status(int index, int argc, char *argv[])
{
    char data[64] = {0};
    int len=64, ret;
	unsigned int tx_seq,rx_seq, count;
	char cmd_id,rx_cmd_type;
    char *buf_p;

	cmd_id = inband_cmd_table[index].cmd_id;
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;	
	if(ret < 0)
		return -1;

	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,3000); //return data length 	
	if(count < 0)
		return -1;

	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq)){
        if(inband_cmd_table[index].print_func)
            inband_cmd_table[index].print_func(data,buf_p,count);
        printf("get nas filter status ok!\n");
    }
	else
	{
		printf("get nas filter status fail!\n");
		return -1;
	}		
	return 0;
}// end get status

int get_entry_nas_filter(int index, int argc, char *argv[])
{
    char data[64] = {0};
    int len=64, ret;
	unsigned int tx_seq,rx_seq, count;
	char cmd_id,rx_cmd_type;
    char *buf_p;

	cmd_id = inband_cmd_table[index].cmd_id;
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;	
	if(ret < 0)
		return -1;

    printf("[%s:%d]\n", __FUNCTION__, __LINE__);
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,3000); //return data length 	
	if(count < 0)
		return -1;

	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq)){
        printf("[%s:%d]get nas filter entry ok!\n", __FUNCTION__, __LINE__);
        if(inband_cmd_table[index].print_func)
            inband_cmd_table[index].print_func(data,buf_p,count);
    }
	else
	{
		printf("[%s:%d]get nas filter entry fail!\n", __FUNCTION__, __LINE__);
		return -1;
	}		
	return 0;
}// end get entry

int add_entry_nas_filter(int index, int argc, char *argv[])
{
    char data[64] = {0};
    int len, ret;
	unsigned int tx_seq,rx_seq, count;
	char cmd_id,rx_cmd_type;
    char *buf_p;
    if(argc<3){
        printf("[%s:%d]Wrong arguments. Format: \"inband addNasFilterEntry 001122334455 [comments]\", comments is optional\n", __FUNCTION__, __LINE__);
        return -1;
    }else if(argc==3){
        // no comments
        memcpy(data, argv[2], 12);
        len = 12;
    }else{
        // have comments, 20 is the limitation of comments field of nas filter
        int comm_len = strlen(argv[3])>20?20:strlen(argv[3]);
        memcpy(data, argv[2], 12);
        memcpy(data+12, argv[3], comm_len);
        len = 12+comm_len;
    }
    
	cmd_id = inband_cmd_table[index].cmd_id;
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;	
	if(ret < 0)
		return -1;

    printf("[%s:%d]\n", __FUNCTION__, __LINE__);
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,3000); //return data length 	
	if(count < 0)
		return -1;

	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq)){
        printf("[%s:%d]add nas filter entry ok!\n", __FUNCTION__, __LINE__);
        if(inband_cmd_table[index].print_func)
            inband_cmd_table[index].print_func(data,buf_p,count);
    }
	else
	{
		printf("[%s:%d]add nas filter entry fail!\n", __FUNCTION__, __LINE__);
		return -1;
	}		
	return 0;
}// end add

int del_entry_nas_filter(int index, int argc, char *argv[])
{
    char data[64] = {0};
    int len, ret;
	unsigned int tx_seq,rx_seq, count;
	char cmd_id,rx_cmd_type;
    char *buf_p;
    if(argc<3 || argc>4){
        printf("[%s:%d]Wrong arguments. Format: \"inband delNasFilterEntry mac(e.g:001122334455) comment/all\"\n", __FUNCTION__, __LINE__);
        return -1;
    }else if(argc==3){
        // deleta all
        len = strlen(argv[2]);
        printf("[%s:%d]argc=%d, argv[2]=%s\n", __FUNCTION__, __LINE__, argc, argv[2]);
        if(len!=3){
            printf("[%s:%d]Wrong arguments. Format: \"inband delNasFilterEntry mac(e.g:001122334455) comment/all\"\n", __FUNCTION__, __LINE__);
            return -1;
        }
        sprintf(data, "%s", argv[2]);
    }else if(argc==4){
        // delete one entry
        memcpy(data, argv[2], 12);  // mac addr
        len = 12;
        int comm_len = (strlen(argv[3])>20)?20:strlen(argv[3]);
        memcpy(data + 12, argv[3], comm_len);
        len += comm_len;
    }
    
	cmd_id = inband_cmd_table[index].cmd_id;
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;	
	if(ret < 0)
		return -1;

    printf("[%s:%d]\n", __FUNCTION__, __LINE__);
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,3000); //return data length 	
	if(count < 0)
		return -1;

	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq)){
        printf("[%s:%d]del nas filter entry ok!\n", __FUNCTION__, __LINE__);
        if(inband_cmd_table[index].print_func)
            inband_cmd_table[index].print_func(data,buf_p,count);
    }
	else
	{
		printf("[%s:%d]del nas filter entry fail!\n", __FUNCTION__, __LINE__);
		return -1;
	}		
	return 0;
}// end del
#endif
#endif
static void print_cfgread(char *para,char *data,int len)
{
	int i=0;
	unsigned char *value;

	value = strchr(data,'=')+1;
	data[len] = '\0';

	printf("cmd_cfgread :  %s=%s\n",para,value);
	/*for(;i<len;i++)
		printf("val[%d]=%x\n",i,data[i]);
	*/
	
}

static void print_syscmd(char *para,char *data,int len)
{
        int i=0;
        unsigned char *value;

        data[len] = '\0';
        printf("cmd_cfgread \n");
        printf("%s",data);
}
#if 1
static void print_getassostanum(char *para,char *data,int len)
{
	printf("Associated statsion number = %d \n",(unsigned char)data[0]);	
}
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

static void print_stainfo(char *para,char *data,int len)
{
	WLAN_STA_INFO_Tp pInfo;
	int i=0,rateid=0,sta_num=data[0];
	char mode_buf[20],txrate[20];


	if(sta_num <= 0)
		printf("No Associated  station now!!!!\n ");
	for(i=0;i<sta_num;i++)
	{
		pInfo = (WLAN_STA_INFO_T *)&(data[i*sizeof(WLAN_STA_INFO_T)+1]);
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

static void print_bssinfo(char *para,char *data,int len)
{
	WLAN_BSS_INFO_Tp bssInfo;
	char *pMsg;
	
	bssInfo = (WLAN_BSS_INFO_T *)data;
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
	printf("Rssi : %d \n",bssInfo->rssi);
	
}
static void print_port_status(char *para,char *data,int len)
{
	struct lan_port_status *port_status;
	
	port_status = (struct lan_port_status *)data;
	
	printf("Link = %s\n",lan_link_spec[port_status->link]);	
	printf("Speed = %s\n",lan_speed_spec[port_status->speed]);
	printf("Nway mode = %s\n",enable_spec[port_status->nway]);	
	printf("Duplex = %s\n",enable_spec[port_status->duplex]);
		
}
static void print_port_rate(char *para,char *data,int len)
{
	struct port_rate *portRate;
	int portnum=data[0];
	int portId=0;
	
	for(portId=0;portId<portnum;portId++)
	{
		portRate = (struct port_rate *)(&data[portId*sizeof(struct port_rate)+1]);
		if(portRate){
			printf("port%d:rx rate=%d(%d)  tx rate=%d(%d)\n",portRate->port_id,
			portRate->rx_rate,portRate->rx_percent,portRate->tx_rate,portRate->tx_percent);
		}
	}
}

static void print_port_stats(char *para,char *data,int len)
{
	struct port_statistics *port_stats;
	port_stats = (struct port_statistics *)data;

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

static void print_firm_check_sig_checksum(char *para,char *data,int len)
{
	printf("%s\n",data);
}

static void print_firm_check_flash_data(char *para,char *data,int len)
{
	printf("%s\n",data);
}
static void print_get_pppoe_err_code(char *para,char *data,int len)
{
	printf("recv pppoe error code: %x\n",*((int*)data));
}
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

/* Inband upgrade wait time. winfred_wang */
static int rtk_calac_inband_upgrade_time(int data_len)
{
	#define FIRMWARE_REBOOT_TIME 	60
	#define FIRMWARE_1M_BURN_TIME	32
	#define FIRMWARE_1M_SIZE		(1024*1024)

	float time=0;
	time =  (((float)data_len/FIRMWARE_1M_SIZE))*FIRMWARE_1M_BURN_TIME;
	time += FIRMWARE_REBOOT_TIME;
	return (int)time;
}

static int host_firmware_write(char *data,int len,int wait_burn_flag)
{
	unsigned int tx_seq,rx_seq;	 	
	int ret=0,count;
	char rx_cmd_type;
	char *buf_p;
	int wait_time = 0;


	ret = host_inband_write(id_firm_upgrade,data,len); //send request
	tx_seq = host_inband_seq;	
	if(ret < 0)
		return -1;	

   ret = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,20000); //return data length 	

   if(ret < 0)
   {
	 	ret=-1;
		printf("firmware inband_rcv_data_and_seq timeout!");
		goto out;
   }
   
   if( (rx_cmd_type == id_firm_upgrade) && (tx_seq == rx_seq)){
		wait_time = rtk_calac_inband_upgrade_time(len);
		if(wait_burn_flag){
			printf("host_firmware_write waiting %d s ... \n",wait_time);
			sleep(wait_time);
			printf("host_firmware_write ok! \n");
		}
		else{
			printf("Please don't access to the host within %d s! \n",wait_time);
		}	
   }
   else
   {
		ret=-1;
   		printf("host_firmware_write fail! rx_cmd_type=%x tx_seq=%d,rx_seq=%d\n",rx_cmd_type,tx_seq,rx_seq);
   }

out:
	inband_free_buf(buf_p, count);
	return ret;

}
	
static int rtk_firmware_update(char *upload_data, int upload_len, int wait_burn_flag)
{
	int head_offset=0 ;
	int isIncludeRoot=0;
	int		 len; 
	int          numLeft;
	int          numWrite;
	IMG_HEADER_Tp pHeader;	
	int fh;
	int fwSizeLimit = 0x800000;
	char buffer[100];
	int flag=0;

	while(head_offset <   upload_len) {
	    pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
	    len = pHeader->len;
#ifdef _LITTLE_ENDIAN_
	    len  = DWORD_SWAP(len);
#endif

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
	    else {
	       	strcpy(buffer, "Invalid file format!");
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
				goto ret_upload;
			}
	    }
	    else {
	    	char *ptr = &upload_data[sizeof(IMG_HEADER_T)+head_offset];
	    	if ( !CHECKSUM_OK(ptr, len) ) {
	     		sprintf(buffer, "Image checksum mismatched! len=0x%x", len);
				goto ret_upload;
			}
	    }  
		
		//numWrite = write(fh, &(upload_data[locWrite+head_offset]), numLeft);
		//inband write , and check ack	
		#if 0
		if(host_firmware_write(&(upload_data[head_offset]),numLeft) < 0 )
		{
			sprintf(buffer, "host_firmware_write! numLeft=0x%x", numLeft);
			goto ret_upload;
		}	
		#endif
		head_offset += len + sizeof(IMG_HEADER_T) ;
	}

	if(host_firmware_write(upload_data,upload_len,wait_burn_flag) < 0 )
	{
		sprintf(buffer, "host_firmware_write! numLeft=0x%x", numLeft);
		goto ret_upload;
	}    
  
  	return 0;

  ret_upload:	
  	fprintf(stderr, "%s\n", buffer);	
	return -1;
} //while //sc_yang   

int cmd_firmware_upgrade(int index,int argc, char *argv[])
{
	int len,ret,count;
	//char data[1480];    	    
	char filename[50];
	char *data;
	int fd,rc;
	struct stat ffstat;
	int flen=0; 
	int wait_burn_flag= 0;
   
	if(argc >= 4 && !(strcmp(argv[3],"wait_burn"))){
		wait_burn_flag = 1;
	}
    
	//sprintf(data,"%s",argv[2]); 
	sprintf(filename,"%s",argv[2]); 

	fd = open(filename, O_RDONLY);

	if (fd < 0)	{
		printf("Cannot Open file %s!\n", filename);
		return -1;
	}
	fstat(fd, &ffstat);
	flen = ffstat.st_size;
	printf("flen = %d \n",flen);

	if((data = (char *)malloc(flen)) == NULL)
	{
		printf("data buffer allocation failed!\n");
		return -1;
	}

	// rc = read(fd, data, 1480);
	rc = read(fd, data, flen);
		
	//if (rc != 1480) {
	if (rc != flen) {
		printf("Reading error\n");
		free(data);	//need free before return!!!!!	
		return -1;
	}

	close(fd);

	ret = rtk_firmware_update(data, flen,wait_burn_flag);

	if(ret < 0)
		printf("rtk_firmware_update fail \n");
	else
		printf("rtk_firmware_update ok \n");  	

	free(data);	//need free before return!!!!!

	return ret;  
}


int cmd_firm_check_sig_checksum(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;	
	unsigned int tx_seq,rx_seq;	   
	cmd_id = inband_cmd_table[index].cmd_id;

	len = strlen(data);

	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;

	if(ret < 0)
		return -1;

	//count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,1000); //return data length 
	if(count < 0)
		return -1;

	if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
	{
		if(inband_cmd_table[index].print_func)
			inband_cmd_table[index].print_func(data,buf_p,count);
	}
	else
		return -1;
	return 0;
    
}

int cmd_firm_check_flash_data(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;	
	unsigned int tx_seq,rx_seq;	   
	cmd_id = inband_cmd_table[index].cmd_id;

	len = strlen(data);

	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;

	if(ret < 0)
		return -1;

	//count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,1000); //return data length 
	if(count < 0)
		return -1;

	if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
	{
		if(inband_cmd_table[index].print_func)
			inband_cmd_table[index].print_func(data,buf_p,count);
	}
	else
		return -1;

	return 0;
    
}

int cmd_firm_upgrade_reboot(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;	
	unsigned int tx_seq,rx_seq;	   
	cmd_id = inband_cmd_table[index].cmd_id;

	len = strlen(data);

	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
		
	if(ret < 0)
		return -1;

	return 0;
}

#if 1
int cmd_sendfile(int index,int argc, char *argv[])
{
	int len,ret,count;
	//char data[1480];    	
	char cmd_id,rx_cmd_type;
	char *buf_p;
	unsigned int tx_seq,rx_seq;	 	
	char filename[50];
	char *data;
	int fd,rc;
	struct stat ffstat;
	int flen=0;

	cmd_id = inband_cmd_table[index].cmd_id;

	//sprintf(data,"%s",argv[2]); 
	sprintf(filename,"%s",argv[2]); 

	fd = open(filename, O_RDONLY);

	if (fd < 0)	{
		printf("Cannot Open file %s!\n", filename);
		return -1;
	}
	fstat(fd, &ffstat);
	flen = ffstat.st_size;
	printf("flen = %d \n",flen);

	if((data = (char *)malloc(flen)) == NULL)
	{
		printf("data buffer allocation failed!\n");
		return -1;
	}

	// rc = read(fd, data, 1480);
	rc = read(fd, data, flen);
		
	//if (rc != 1480) {
	if (rc != flen) {
		printf("Reading error\n");
		free(data);	//need free before return!!!!!	
		return -1;
	}

	close(fd);
	//len = 1480;		
	len = flen;		
	tx_seq = host_inband_seq;	
	ret = host_inband_write(cmd_id,data,len); //send request

	free(data);	//need free before return!!!!!

	if(ret < 0)
		return -1;

	//count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
	count = inband_rcv_data_and_seq(&rx_seq,&rx_cmd_type,&buf_p,10000); //return data length 	
	if(count < 0)
	{
	 	ret=-1;
		goto out;
	}	

	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
		printf("remote write ok!\n");
	else
	{
		ret=-1;
		printf("remote write fail!\n");
	}		

	out:
	inband_free_buf(buf_p, count);
		
	return ret;
}
#endif

#endif


static int host_inband_write(char cmd_id,char *data,int data_len)
{
	int ret;

    host_inband_seq = get_random(65536);

	inband_set_cmd_seq(hcd_inband_chan,cmd_id,host_inband_seq);

    ret = inband_write(hcd_inband_chan,host_inband_seq,cmd_id,data,data_len,0); //send request
    return ret;
}

static char *get_intf(char *data,char *intf)
{
	char *ptr=data;
	int len=0, idx=0;

	while (*ptr && *ptr != '\n' ) {
		if (*ptr == '_') {
			if (len <= 1)
				return NULL;
			memcpy(intf, data, len);
			intf[len] = '\0';
			
			return ptr+1;
		}
		len++;
		ptr++;
	}
	return NULL;
}


static char *get_token(char *data, char *token)
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

//////////////////////////////////////////////////////////////////////
static int get_value(char *data, char *value)
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


int generate_wps_conf(unsigned char *config_name)
{
	FILE *in=NULL, *out=NULL;
	int fin=-1,fout=-1;
	struct stat status;
	unsigned char line[400] = {0}, intf[20] = {0}, token[40] = {0}, value[300] = {0}, *ptr, mib[300] = {0}, 
					wsc_pin[9] = {0}, *buf, *bufstart;
	int idx=0, len=0, wepkeyid=0, i=0;
	unsigned char ssid[32] = {0}, key[4][40] = {{0},{0},{0},{0}}, device_name[32] = {0};
	int mode=0,authtype=0,encrypt=0,configbyextreg=1,configured=0,isUpnpEnabled=0,
		config_method=0,wps_disabled=0,connection_type=0,manual_enabled=0,band=0,is_client=0,
		cipher1=0,cipher2=0,wps_enabled=0;

	if( stat(INBAND_WPS_CONFIG,&status) == 0 ){
		sprintf(line,"rm %s",INBAND_WPS_CONFIG);
		system(line);
	}
	in = fopen(config_name, "r");
	if (in < 0) {
		printf("open file [%s] failed!\n", config_name);
		return -1;
	}
#ifdef INBAND_WPS_OVER_HOST
	if (stat("/etc/wscd.conf", &status) < 0) {
		printf("stat() error [%s]!\n", config_name);
		return -1;
	}
#endif	//INBAND_WPS_OVER_HOST
	buf = malloc(status.st_size+2048);
	if (buf == NULL) {
        printf("malloc() error [%d]!\n", (int)status.st_size+2048);
        return -1;
	}

	bufstart = buf;

	while ( fgets(line, 400, in) ) {
		ptr = get_token(line, token);
		if (ptr == NULL)
			continue;
		if (get_value(ptr, value)==0)
			continue;

		if ( strstr(token,"disable_wps") ) {
			if( get_intf(token,intf) == 0 ) {
				return -1;
			}
			wps_enabled = (atoi(value)==1)?0:1;
		} else if ( strcmp(token,"wlan_band") == 0 ) {
			band = (atoi(value) & BAND_11A)+1;
		} else if ( strcmp(token,"sys_wps_pin") == 0 ) {
			strcpy(wsc_pin,value);
		} else if ( strcmp(token,"sys_wps_device_name") == 0 ) {
			strcpy(device_name,value);
		} else if( strstr(token,intf) == 0 ) {
			continue;
		} else if( strstr(token,"_ssid") ) {
			strcpy(ssid,value);
		} else if ( strstr(token,"_mode") ) {
			is_client = atoi(value);
		} else if ( strstr(token,"_network_type") ) {
			connection_type = atoi(value);
		} else if ( strstr(token,"_wep_authtype") ) {
			authtype = atoi(value);
		} else if ( strstr(token,"_encmode") ) {
			encrypt = atoi(value);
		} else if ( strstr(token,"_passphrase") ) {
			strcpy(key[0],value);
		} else if ( strstr(token,"_wpa_cipher") ) {
			cipher1 = atoi(value);
		} else if ( strstr(token,"_wpa2_cipher") ) {
			cipher2 = atoi(value);
		} else if ( strstr(token,"_wep64key1") ) {
			strcpy(key[0],value);
		} else if ( strstr(token,"_wep64key2") ) {
			strcpy(key[1],value);
		} else if ( strstr(token,"_wep64key3") ) {
			strcpy(key[2],value);
		} else if ( strstr(token,"_wep64key4") ) {
			strcpy(key[3],value);
		} else if ( strstr(token,"_wep128key1") ) {
			strcpy(key[0],value);
		} else if ( strstr(token,"_wep128key2") ) {
			strcpy(key[1],value);
		} else if ( strstr(token,"_wep128key3") ) {
			strcpy(key[2],value);
		} else if ( strstr(token,"_wep128key4") ) {
			strcpy(key[3],value);
		} else if ( strstr(token,"_wepdkeyid") ) {
			wepkeyid = atoi(value);
		} else if ( strstr(token,"_wps_configbyextreg") ) {
			configbyextreg = atoi(value);
		} else if ( strstr(token,"_wps_configured") ) {
			configured = atoi(value);
		} else if ( strstr(token,"_wps_upnp_support") ) {
			isUpnpEnabled = atoi(value);
		} else if ( strstr(token,"_wps_config_method") ) {
			config_method = atoi(value);
		} else {
			continue;
		}
	}
	fclose(in);

	if( !wps_enabled )
		return 1;
	
	if (is_client == CLIENT_MODE) {
        if (configbyextreg) {
            if (!configured)
                mode = MODE_CLIENT_UNCONFIG_REGISTRAR;
            else
                mode = MODE_CLIENT_CONFIG;
        }
        else
            mode = MODE_CLIENT_UNCONFIG;
    } else {
        if (!configured)
            mode = MODE_AP_UNCONFIG;
        else
            mode = MODE_AP_PROXY_REGISTRAR;
    }

	switch(encrypt){			
		case ENCRYPT_WEP:
			if (authtype == AUTH_OPEN || authtype == AUTH_BOTH)
                authtype = WSC_AUTH_OPEN;
            else
                authtype = WSC_AUTH_SHARED;
			encrypt = WSC_ENCRYPT_WEP;
			break;
		case ENCRYPT_WPA:
			authtype = WSC_AUTH_WPAPSK;
			if( cipher1 == WPA_CIPHER_TKIP )
				encrypt = WSC_ENCRYPT_TKIP;
			else if( cipher1 == WPA_CIPHER_AES)
				encrypt = WSC_ENCRYPT_AES;
			else
				encrypt = WSC_ENCRYPT_TKIPAES;
			break;
		case ENCRYPT_WPA2:
            authtype = WSC_AUTH_WPA2PSK;
            if (cipher2 == WPA_CIPHER_TKIP)
            	encrypt = WSC_ENCRYPT_TKIP;
            else if (cipher2 == WPA_CIPHER_AES)
            	encrypt = WSC_ENCRYPT_AES;
            else
            	encrypt = WSC_ENCRYPT_TKIPAES;
		default:	//as authtype = WSC_AUTH_OPEN;
			authtype = WSC_AUTH_OPEN;
			encrypt = WSC_ENCRYPT_NONE;
			break;
	}

	if (is_client)
	{
		if (connection_type == 0)
			connection_type = 1;
		else
			connection_type = 2;

	} else {
		connection_type = 1;
	}

	WRITE_WSC_PARAM(buf, line, "mode = %d\n", mode, len);
	//support one interface only
	WRITE_WSC_PARAM(buf, line, "wlan_inter_num = %d\n", 1, len);
	WRITE_WSC_PARAM(buf, line, "upnp = %d\n", isUpnpEnabled ,len);
	WRITE_WSC_PARAM(buf, line, "config_method = %d\n", config_method ,len);
	WRITE_WSC_PARAM(buf, line, "wlan0_wsc_disabled = %d\n", wps_disabled ,len);
	WRITE_WSC_PARAM(buf, line, "auth_type = %d\n", authtype, len);
	WRITE_WSC_PARAM(buf, line, "encrypt_type = %d\n", encrypt, len);
	WRITE_WSC_PARAM(buf, line, "connection_type = %d\n", connection_type, len);
	WRITE_WSC_PARAM(buf, line, "manual_config = %d\n", manual_enabled, len);
	if( encrypt == WSC_ENCRYPT_WEP ) {
		WRITE_WSC_PARAM(buf, line, "wep_transmit_key = %d\n", wepkeyid+1, len);
		for( i=0;i<4;i++ ) {
			if( i == wepkeyid ) {
				WRITE_WSC_PARAM(buf, line, "network_key = %s\n",key[i], len);
			} else {
				sprintf(value,"wep_key%d = %s",i+1,key[i]);
				WRITE_WSC_PARAM(buf, line, "%s\n",value, len);
			}
		}
	} else {
		WRITE_WSC_PARAM(buf, line, "network_key = %s\n",key, len);
	}
	WRITE_WSC_PARAM(buf, line, "ssid = %s\n", ssid, len);
	WRITE_WSC_PARAM(buf, line, "pin_code = %s\n", wsc_pin, len);
	WRITE_WSC_PARAM(buf, line, "rf_band = %d\n", band, len);
	WRITE_WSC_PARAM(buf, line, "device_name = \"%s\"\n", device_name, len);
	WRITE_WSC_PARAM(buf, line, "config_by_ext_reg = %d\n", configbyextreg, len);	
	//printf("!!! %d %d\n",len,(int)(((long)buf)-((long)bufstart)));
#ifdef INBAND_WPS_OVER_HOST
	fin = open("/etc/wscd.conf", O_RDONLY);
	if (fin == -1) {
		printf("open() error [%s]!\n", "/etc/wscd.conf");
		return -1;
	}

	lseek(fin, 0L, SEEK_SET);
	if (stat("/etc/wscd.conf", &status) < 0) {
        printf("stat() error [%s]!\n", "/etc/wscd.conf");
        return -1;
	}
	//printf("read %d bytes from %s\n",status.st_size,"/etc/wscd.conf");
	if (read(fin, buf, status.st_size) != status.st_size) {
		printf("read() error [%s]!\n", in);
		return -1;	
	}
	close(fin);
#endif //#ifdef INBAND_WPS_OVER_HOST
	fout = open(INBAND_WPS_CONFIG, O_RDWR|O_CREAT|O_TRUNC);
	if (fout < 0) {
		printf("open file [%s] to write failed!\n", INBAND_WPS_CONFIG);
		return -1;
	}
	if ( write(fout, bufstart, len+status.st_size) != len+status.st_size ) {
		printf("Write() file error [%s]!\n", INBAND_WPS_CONFIG);
		return -1;
	}
	close(fout);
	free(bufstart);

	return 0;
}

int generate_auth_conf(unsigned char *rsIP, unsigned char *outputFile, int isWds)
{
	int fh, intVal, encrypt, enable1x, wep;
	unsigned char buf1[1024], buf2[1024];
	struct stat status;

	if( stat(outputFile,&status) == 0 ) {
		sprintf(buf1,"rm %s",outputFile);
		system(buf1);
	}

	sprintf(buf1,"echo \"rsIP = %s\" > %s",rsIP,outputFile);
	system(buf1);

	return 0;
}
void init_8021x_mib(struct wlan_8021x_mib *wlan_mib)
{
	memset(wlan_mib,0,sizeof(struct wlan_8021x_mib));
	//add other hardcode mib default value below!!
}


int parse_file_to_8021x_mib(unsigned char *if_name,unsigned char *conf_File,struct wlan_8021x_mib *wlan_mib)
{
	FILE *f;
	char buf[256], *pos ,*para;
	int line = 0;

	f = fopen(conf_File, "r");

	 if (f == NULL)
     {
        printf("Could not open configuration file '%s' "
                                        "for reading.",conf_File);
                return -1;
     }

	while (fgets(buf, sizeof(buf), f))
	{
		line++;
		pos = buf;
        while (*pos != '\0')
        {
          if (*pos == '\n')
          {
             *pos = '\0';
              break;
           }
 	         pos++;
        }

		if (buf[0] == '\0')
	        continue;

		pos = strchr(buf, '=');
        if (pos == NULL)
        {
        	printf("Line %d: invalid line '%s'", line, buf);
            continue;
        }
        *pos = '\0'; 
         pos++;
		 para = buf;
         if (strncmp(buf, if_name,strlen(if_name)) == 0) { //mean this parameter belong to correct wlan interface
				
				para = para + strlen(if_name)+1;
				printf("para = %s !!!\n",para);
				
			    if (strcmp(para, "encmode") == 0) {
					
                     wlan_mib->encrypt = atoi(pos);   
                }
                else if (strcmp(para, "ssid") == 0) {
					strcpy(wlan_mib->ssid,pos);						
                }
                else if (strcmp(para, "802_1x") == 0) {
					wlan_mib->enable1X = atoi(pos);                         
                }
                else if (strcmp(para, "wep") == 0) {
					wlan_mib->wep = atoi(pos);                         
                }
				else if (strcmp(para, "wepkey1") == 0) {
					//wlan_mib->wep128Key1 = atoi(pos);                         
					strcpy(wlan_mib->wep128Key1,pos);  //To Do?
                }
				else if (strcmp(para, "wpa_auth") == 0) {
					wlan_mib->wpaAuth = atoi(pos);                         
                }
				else if (strcmp(para, "wpa_cipher") == 0) {
					wlan_mib->wpaCipher = atoi(pos);                         
                }
				else if (strcmp(para, "wpa2_cipher") == 0) {
					wlan_mib->wpa2Cipher = atoi(pos);                         
                }
				else if (strcmp(para, "wpaPSKFormat") == 0) {
					wlan_mib->wpaPSKFormat = atoi(pos);                         
                }
				else if (strcmp(para, "passphrase") == 0) {
					strcpy(wlan_mib->wpaPSK,pos);  //To Do?                      
                }
				else if (strcmp(para, "gk_rekey") == 0) {
					wlan_mib->wpaGroupRekeyTime = atoi(pos);                         
                }
				else if (strcmp(para, "auth_server_port") == 0) {
					wlan_mib->rsPort = atoi(pos);                         
                }
				else if (strcmp(para, "gk_rekey") == 0) {
					wlan_mib->wpaGroupRekeyTime = atoi(pos);                         
                }
				else if (strcmp(para, "auth_server_port") == 0) {
					wlan_mib->rsPort = atoi(pos);                         
                }
				else if (strcmp(para, "auth_server_ip") == 0) {
					int ip_addr;
					ip_addr = inet_addr(pos);
					wlan_mib->rsIpAddr[0] = (unsigned char)(ip_addr >> 24);                         
					wlan_mib->rsIpAddr[1] = (unsigned char)(ip_addr >> 16);                         
					wlan_mib->rsIpAddr[2] = (unsigned char)(ip_addr >> 8);                         
					wlan_mib->rsIpAddr[3] = (unsigned char)ip_addr;                         					
                }
				else if (strcmp(para, "auth_server_shared_secret") == 0) {
					printf("auth_server_shared_secret=%s \n", pos);
					strcpy(wlan_mib->rsPassword,pos);  //To Do? 
					printf("wlan_mib->rsPassword=%s \n", wlan_mib->rsPassword);
                }
				else
				{
					//printf("it's not mib in 8021x!!!\n");
				}

         }
         
	} //end of while
	
    fclose(f);
	return 0;
}

int gen_wlan_auth_conf(unsigned char *if_name,unsigned char *rsIP,unsigned char *outputFile)
{
	unsigned char buf1[1024];
	struct stat status;
#ifdef MULTI2UNI
	if( stat(outputFile,&status) == 0 )
		sprintf(buf1,"echo \"%s_rsIP = %s\" >> %s",if_name,rsIP,outputFile);
	else
    	sprintf(buf1,"echo \"%s_rsIP = %s\" > %s",if_name,rsIP,outputFile);

	system(buf1);
#else
	if( stat(outputFile,&status) == 0 ) {
		sprintf(buf1,"rm %s",outputFile);
		system(buf1);
    }

	sprintf(buf1,"echo \"%s_rsIP = %s\" > %s",if_name,rsIP,outputFile);
	system(buf1);
#endif	//MULTI2UNI
	return 0;
}

//mark_8021x------------------above


int check_8021x_enabled(unsigned char *config_name)
{
	FILE *in=NULL;

	unsigned char line[400] = {0}, token[40] = {0}, value[300] = {0}, *ptr=NULL, intf[10]={0}, search[40] = {0};
	int radius_enabled = 0, i;
	struct stat status;
	unsigned char wpa_conf_name[9][30] = {{"/var/wpa.conf"},{"/var/wpa-wlan0.conf"},{"/var/wpa-wlan0-va0.conf"},{"/var/wpa-wlan0-va1.conf"},
						{"/var/wpa-wlan0-va2.conf"},{"/var/wpa-wlan0-va3.conf"},{"/var/wpa-wlan0-va4.conf"},
						{"/var/wpa-wlan0-va5.conf"},{"/var/wpa-wlan0-va6.conf"}};

	for( i=0;i<8;i++ ) {
		if( !stat(wpa_conf_name[i],&status) ) {
			sprintf(line,"rm %s",wpa_conf_name[i]);
			system(line);
			printf("%s",line);
		}
	}

	in = fopen(config_name, "r");
	if (in < 0) {
		printf("open file [%s] failed!\n", config_name);
		return -1;
	}

	while ( fgets(line, 400, in) ) {
		ptr = get_token(line, token);
		if (ptr == NULL)
			continue;
		if (get_value(ptr, value)==0)
			continue;

		if ( strstr(token,"802_1x") ) {
			ptr = strchr(token,'_');
			*ptr = '\0';
			strcpy(intf,token);
			//printf("%s enable 8021X\n",intf);
			radius_enabled |= atoi(value);

			if(radius_enabled) {
				sprintf(search,"%s_auth_server_ip",intf);
				while ( fgets(line, 400, in) ) {
					ptr = get_token(line, token);
					if (ptr == NULL)
						continue;
					if (get_value(ptr, value)==0)
						continue;

					if( strcmp(token,search) == 0 ){
						ptr = value;
						break;
					}
				}
#ifdef MULTI2UNI
				sprintf(line,"/var/wpa.conf");
#else
				sprintf(line,"/var/wpa-%s.conf",intf);
#endif	//MULTI2UNI
				gen_wlan_auth_conf(intf,ptr,line);
				radius_enabled = 0;
			}
		} else if (strstr(token,"wpa_auth")) {
			ptr = strchr(token,'_');
			*ptr = '\0';
			strcpy(intf,token);
			if( atoi(value) == 1 ) {
				printf("%s enable radius\n",intf);
				radius_enabled |= atoi(value);
			}

			if(radius_enabled) {
				sprintf(search,"%s_auth_server_ip",intf);
				while ( fgets(line, 400, in) ) {
					ptr = get_token(line, token);
					if (ptr == NULL)
						continue;
					if (get_value(ptr, value)==0)
						continue;

					if( strcmp(token,search) == 0 ){
						ptr = value;
						break;
					}
				}
#ifdef MULTI2UNI
				sprintf(line,"/var/wpa.conf");
#else
				sprintf(line,"/var/wpa-%s.conf",intf);
#endif	//MULTI2UNI
				gen_wlan_auth_conf(intf,ptr,line);
				radius_enabled = 0;
			}
		} else {
			continue;
		}
	}
	fclose(in);
	return 0;
}

int cmd_sendconf(int index,int argc, char *argv[])
{
    int len,ret,count;
    //char data[1480];    	
    char cmd_id,rx_cmd_type;
    char *buf_p;
    unsigned int tx_seq,rx_seq;	 	
    char filename[50];
    char *data;
    int fd,rc;
   struct stat ffstat;
   int flen=0,i;
	unsigned char wpa_conf_name[8][30] = {{"/var/wpa-wlan0.conf"},{"/var/wpa-wlan0-va0.conf"},{"/var/wpa-wlan0-va1.conf"},
						{"/var/wpa-wlan0-va2.conf"},{"/var/wpa-wlan0-va3.conf"},{"/var/wpa-wlan0-va4.conf"},
						{"/var/wpa-wlan0-va5.conf"},{"/var/wpa-wlan0-va6.conf"}};
	unsigned char line[100];
	int radius_enabled = 0;

	for( i=0;i<8;i++ ) {
		if( !stat(wpa_conf_name[i],&ffstat) ) {
			sprintf(line,"rm %s",wpa_conf_name[i]);
			system(line);
		}
	}

   cmd_id = id_set_mib;
    
  //sprintf(data,"%s",argv[2]); 
  sprintf(filename,"%s",argv[2]); 

  fd = open(filename, O_RDONLY);

  if (fd < 0)	{
		printf("Cannot Open file %s!\n", filename);
		return -1;
   }
  fstat(fd, &ffstat);
  flen = ffstat.st_size;
  printf("flen = %d \n",flen);

  if((data = (char *)malloc(flen)) == NULL)
  {
	printf("data buffer allocation failed!\n");
	return -1;
  }

 // rc = read(fd, data, 1480);
  rc = read(fd, data, flen);
   	
  //if (rc != 1480) {
  if (rc != flen) {
  	        printf("Reading error\n");
		 free(data);	//need free before return!!!!!	
  	        return -1;
   }

  close(fd);

  if( generate_wps_conf(filename) > 0 )
  	check_8021x_enabled(filename);

  //generate_auth_conf(data);
  //len = 1480;		
  len = flen;
		
   ret = host_inband_write(cmd_id,data,len); //send request
   tx_seq = host_inband_seq;	

   free(data);	//need free before return!!!!!

   if(ret < 0)
   	return -1;
	
   //count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
   count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,3000); //return data length 	
   if(count < 0)
   {
	 	ret=-1;
		goto out;
   }	
   
   if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
   	printf("remote write ok!\n");
   else
   {
	ret=-1;
   	printf("remote write fail!\n");
   }		

   out:
   inband_free_buf(buf_p, count);
   	
   return ret;
}

static int cmd_trigger_wlan_action(int index,int argc, char *argv[])
{
    int len,ret,count;
    char data[1480];	
    char cmd_id,rx_cmd_type;
    char *buf_p;
    unsigned int tx_seq,rx_seq;	 	

   cmd_id = inband_cmd_table[index].cmd_id;
   
	len = 0;
	if(argc == 3 && (strncmp(argv[2],"wlan",4) == 0))
	{
		len = strlen(argv[2]); 
		memcpy(data,argv[2],len);
		data[len++] = '\0';
	}
	else
		return -1;
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
	if(ret < 0)
		return -1;
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,2000*4);
	if(count < 0)
	 	return -1;
	if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
	{
		printf("%s",buf_p);
	}
	else
		return -1;
	return 0;
}
#ifdef HOST_SEND_CONFIG_FILE
int cmd_send_config_file(int index,int argc, char *argv[])
{
	int ret,count,cmd_id,fd,rc,flen,name_len,flag_len=0;	
	char filename[60];
	char *data;
	char *buf_p;
	char rx_cmd_type;
	unsigned int tx_seq,rx_seq;	
	struct stat ffstat;
	sprintf(filename,"%s",argv[2]);
	fd = open(filename, O_RDONLY);
	if (fd < 0)	{
		printf("Cannot Open file %s!\n", filename);
		return -1;
	}
	fstat(fd, &ffstat);
	flen = ffstat.st_size;
	sprintf(filename,"filename=%s\n",argv[2]);
	name_len = strlen(filename);
  if(argc > 3) 
	{
		if(!strcmp(argv[3],"-d") || !strcmp(argv[3],"-e"))
		{
			flag_len = strlen(argv[3]) + 1;	//-e save and enable config file; -d only save config file
		}
	}
	if((data = (char *)malloc(flen + name_len + flag_len)) == NULL)
	{
		printf("data buffer allocation failed!\n");
		return -1;
	}
	strcpy(data,filename);
	if(flag_len)
		strcpy(data + name_len,argv[3]);
	rc = read(fd, data + name_len + flag_len, flen);
	if (rc != flen) {
		printf("Reading error\n");
		free(data);	//need free before return!!!!!	
		return -1;
	}
	close(fd);
	cmd_id = id_send_config_file;
	ret = host_inband_write(cmd_id,data,flen + name_len + flag_len); //send request
	tx_seq = host_inband_seq;	
	free(data);	//need free before return!!!!!
	if(ret < 0)
		return -1;
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,5000); //return data length	
	if(count < 0)
	{
		ret=-1;
		goto out;
	}	
	if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
		printf("remote write ok!\n");
	else
	{
		ret=-1;
		printf("remote write fail!\n");
	}		
	out:
	inband_free_buf(buf_p, count);
	return ret;
}
int cmd_get_config_file(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;	
	unsigned int tx_seq,rx_seq;	   
	cmd_id = inband_cmd_table[index].cmd_id;
	len = 0;
	if(argc == 3 && (strcmp(argv[2],"all") || strcmp(argv[2],"wlan") || strcmp(argv[2],"hw")))
	{
		len = strlen(argv[2]); 
		memcpy(data,argv[2],len);
		data[len++] = '\0';
	}
	else
		return -1;
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
	if(ret < 0)
		return -1;
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,5000); //return data length 
	if(count < 0)
		return -1;
	if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
	{	
		char filename[50];
		int fd;
		sprintf(filename,"/tmp/config-%s.txt",argv[2]);
		fd = open(filename,O_RDWR|O_CREAT|O_TRUNC);
		write(fd,buf_p,count);
		close(fd);
		if(fd < 0)
			return -1;
		printf("Get config file %s length %d successed!\n",filename,count);
	}
	else
	{
		printf("Get config file fail!\n");
		return -1;
	}
	return 0;
}
#endif
#ifdef INBAND_GET_FILE_SUPPOPRT
int cmd_get_file(int index,int argc, char *argv[])
{
	int len=0,ret=0,count=0;
	char data[1480]={0};	
	char cmd_id=0,rx_cmd_type=0;
	char *buf_p=NULL;	
	unsigned int tx_seq=0,rx_seq=0;    
	cmd_id = inband_cmd_table[index].cmd_id;
	len = 0;
	if(argc == 4)
	{
		len = strlen(argv[2]); 
		memcpy(data,argv[2],len);
		data[len++] = '\0';
	}
	else
		return -1;
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
	if(ret < 0)
		return -1;
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,5000); //return data length 
	if(count < 0)
		return -1;
	if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
	{	
		char filename[64]={0};
		int fd=-1;
		sprintf(filename,"%s",argv[3]);
		fd = open(filename,O_RDWR|O_CREAT|O_TRUNC);
		if(fd < 0)
			return -1;
		write(fd,buf_p,count);
		close(fd);
		printf("Get file %s length %d successed!\n",filename,count);
	}
	else
	{
		printf("Get file fail!\n");
		return -1;
	}
	return 0;
}
#endif
int cmd_sync_to_server(int index,int argc, char *argv[])
{
	int len=0,ret=0,count=0;
	char data[1480]={0};	
	char cmd_id=0,rx_cmd_type=0;
	char *buf_p=NULL;	
	unsigned int tx_seq=0,rx_seq=0;    
	cmd_id = inband_cmd_table[index].cmd_id;
	len = 0;
	if(argc!=4)
	{
		return -1;
	}
  	sprintf(data,"%s %s",argv[2],argv[3]); 
	len=strlen(data);
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
	if(ret < 0)
		return -1;
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,5000); //return data length 
	if(count < 0)
		return -1;
	if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
	{	
		printf("sync to server OK!\n");
		if(inband_cmd_table[index].print_func)
	    	inband_cmd_table[index].print_func(data,buf_p,count);
	}
  else
	{
		printf("sync to server fail!\n");
		return -1;
	}
	return 0;
}
int cmd_write(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;
	unsigned int tx_seq,rx_seq;	 	
	cmd_id = inband_cmd_table[index].cmd_id;
	if(argc > 3) 
	{
		sprintf(data,"%s %s",argv[2],argv[3]); 
	}	
	else if(argc == 3) 
  	sprintf(data,"%s",argv[2]); 
	else
		data[0] = '\0';
   len = strlen(data);		


   ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;	

   if(ret < 0)
   	return -1;
	
   //count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,5000); //return data length 	
   if(count < 0)
	 	return -1;
   
   if( (rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
   	printf("remote write ok!\n");
   else
   {
   	return -1;
   	printf("remote write fail!\n");
   }		
   return 0;
}

int cmd_read(int index,int argc, char *argv[])
{
    int len,ret,count;
    char data[1480];	
    char cmd_id,rx_cmd_type;
    char *buf_p;	
   unsigned int tx_seq,rx_seq;	   
   
   cmd_id = inband_cmd_table[index].cmd_id;
   
  if(argc > 3) 
  	sprintf(data,"%s %s",argv[2],argv[3]); 
	else if(argc == 3) 
  	sprintf(data,"%s",argv[2]); 
	else
		data[0] = '\0';

   len = strlen(data);		

   ret = host_inband_write(cmd_id,data,len); //send request
    tx_seq = host_inband_seq;

   if(ret < 0)
   	return -1;
	
   //count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
   count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,5000); //return data length 

   if(count < 0)
	 	return -1;

   if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
   {
	if(inband_cmd_table[index].print_func)
		inband_cmd_table[index].print_func(data,buf_p,count);
   }
   else
	return -1;

   return 0;
    
}

int cmd_syscmd(int index,int argc, char *argv[])
{
    int len,ret=0,count;
    char data[1480];
    char cmd_id,rx_cmd_type;
    char *buf_p;
   unsigned int tx_seq,rx_seq;

   cmd_id = inband_cmd_table[index].cmd_id;

  if(argc > 3)
        sprintf(data,"%s %s",argv[2],argv[3]);
  else
        sprintf(data,"%s",argv[2]);

   len = strlen(data);


   ret = host_inband_write(cmd_id,data,len); //send request

  tx_seq = host_inband_seq;

   if(ret < 0)
        return -1;

   //count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); //return data length
   count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,5000); //return data length

   if(count < 0)
   {      
      ret=-1;
      goto out;
   }
   //printf("count=%d rx_cmd_type=%x,cmd_id=%x,tx_seq=%d,rx_seq=%d",count,rx_cmd_type,cmd_id,tx_seq,rx_seq);

   if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
   {
        if(inband_cmd_table[index].print_func)
                inband_cmd_table[index].print_func(data,buf_p,count);
   }
   else
     ret=-1;   

   out:
   inband_free_buf(buf_p, count);

   return ret;

}

int gain_rsip(unsigned char *config_name, unsigned char *ip)
{
	FILE *in=NULL;
	unsigned char line[400] = {0}, token[40] = {0}, value[300] = {0}, *ptr;

	in = fopen(config_name, "r");
    if (in < 0) {
            printf("open file [%s] failed!\n", config_name);
            return NULL;
    }

	while ( fgets(line, 400, in) ) {
        ptr = get_token(line, token);
        if (ptr == NULL)
                continue;
        if (get_value(ptr, value)==0)
                continue;

		if ( strstr(token,"rsIP") ) {
			fclose(in);
			memcpy(ip,value,strlen(value));
			return 1;
		}
	}
	fclose(in);
	return 0;
}

int signal_daemon_by_name(unsigned char *name,unsigned int signo)
{
	struct stat status;
	unsigned char line[100];
	int pid;
	FILE *in=NULL;

	sprintf(line,"/var/run/%s.pid",name);
	if( stat(line,&status) == 0 ){
		in = fopen(line,"r");
		if( in )
			fgets(line,sizeof line,in);
		else
			goto out;
		sscanf(line,"%d",&pid);
		kill(pid,signo);
	}

out:
	if(in)
		fclose(in);

	return 0;
}


int cmd_action(int index,int argc, char *argv[])
{
    int len,ret,i;
    unsigned char data[1480],cmd_id,rsip[20];
    unsigned char wpa_conf_name[9][30] = {{"/var/wpa.conf"},{"/var/wpa-wlan0.conf"},{"/var/wpa-wlan0-va0.conf"},{"/var/wpa-wlan0-va1.conf"},{"/var/wpa-wlan0-va2.conf"},
	{"/var/wpa-wlan0-va3.conf"},{"/var/wpa-wlan0-va4.conf"},{"/var/wpa-wlan0-va5.conf"},{"/var/wpa-wlan0-va6.conf"}};
	struct stat status;

	signal_daemon_by_name("wscd-wlan0",SIGTERM);
	signal_daemon_by_name("iweventd",SIGTERM);

	cmd_id = inband_cmd_table[index].cmd_id;
	if(argc > 3) 
  		sprintf(data,"%s %s",argv[2],argv[3]); 
	else
		sprintf(data,"%s",argv[2]); 

	len = strlen(data);		

	ret = host_inband_write(cmd_id,data,len); //send request

	if(ret < 0)
		return -1;

	if( !stat("/var/wps.conf",&status) ) {
#ifdef INBAND_WPS_OVER_HOST
		sleep(2);
		system("wscd -start -c /var/wps.conf -w wlan0 -fi /var/wscd-wlan0.fifo -daemon");
		sleep(1);
		system("iweventd -w &");
#else
		system("iweventd -lw");
#endif
	}

	for( i=0;i<9;i++ ) {
		if( !stat(wpa_conf_name[i],&status) ) {
			memset(data,0,sizeof data);
			memset(rsip,0,sizeof rsip);
			if( gain_rsip(wpa_conf_name[i],rsip) ) {
				sprintf(data,"iweventd -r %s",wpa_conf_name[i]);
				system(data);
			} else {
				printf("Can not find rsIP parameter in config file");
			}
		}
	}

	//we dont check the ack when cmd is trigger action	
	//count = inband_rcv_data(&rx_cmd_type,&buf_p,1000); 
	printf("remote action ok!\n");  
	return 0;
}


int cmd_trigger_wps(int index,int argc, char *argv[])
{
	unsigned char tmpbuf[200],tmp[100];
#ifdef INBAND_HOST
	if( argc == 4 )
		sprintf(tmpbuf,"iweventd wps %s %s",argv[2],argv[3]);
	else
		sprintf(tmpbuf,"iweventd wps %s",argv[2]);

	//printf("%s\n",tmpbuf);

	system(tmpbuf);

	return strlen(tmpbuf);
#else
	if( !strcmp(argv[2],"pin") ){
		sprintf(tmpbuf,"inband syscmd \'iwpriv wlan0 set_mib pin=%s\'",argv[3]);
	}else{
		sprintf(tmpbuf,"inband syscmd \'wscd -sig_pbc\'");
	}
	system(tmpbuf);
	return strlen(tmpbuf);
	//argc = 3;
	//return cmd_syscmd(index,argc,argv);
#endif
}

#if defined(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK)
int rgb_led_blink_start(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;	
	unsigned int tx_seq,rx_seq;	   
	cmd_id = inband_cmd_table[index].cmd_id;

	snprintf(data,1480,"%s %s",argv[2],argv[3]);
	len = strlen(data);
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
	if(ret < 0)
		return -1;
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,1000); //return data length 
	if(count < 0)
		return -1;
	if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
	{
		if(inband_cmd_table[index].print_func)
			inband_cmd_table[index].print_func(data,buf_p,count);
	}
	else
		return -1;
	return 0;
}
int rgb_led_blink_end(int index,int argc, char *argv[])
{
	int len,ret,count;
	char data[1480];	
	char cmd_id,rx_cmd_type;
	char *buf_p;	
	unsigned int tx_seq,rx_seq;	   
	cmd_id = inband_cmd_table[index].cmd_id;
	len = strlen(data);
	ret = host_inband_write(cmd_id,data,len); //send request
	tx_seq = host_inband_seq;
	if(ret < 0)
		return -1;
	count = inband_rcv_data_and_seq(hcd_inband_chan,&rx_seq,&rx_cmd_type,&buf_p,1000); //return data length 
	if(count < 0)
		return -1;
	if((rx_cmd_type == cmd_id) && (tx_seq == rx_seq))
	{
		if(inband_cmd_table[index].print_func)
			inband_cmd_table[index].print_func(data,buf_p,count);
	}
	else
		return -1;
	return 0;
}
#endif

int main(int argc, char *argv[])
{
#ifdef INBANDHOST_PTHREAD_LOCK
	if(pthread_mutex_init_flag == 0){
		pthread_mutex_init_flag = 1;
		pthread_mutex_init(&inband_mutex,NULL);
	}
	pthread_mutex_lock(&inband_mutex);
#endif
	//int fdflags;
	//unsigned int arg;
	int i, ret;
	int chan;

    if (argc < 2) {
        print_command_list();
#ifdef INBANDHOST_PTHREAD_LOCK
		pthread_mutex_unlock(&inband_mutex);
#endif
        return 0;
    }

    //open inband  	
	chan = inband_open(HOST_NETIF,SLAVE_MAC,ETH_P_RTK,0);
	if (chan < 0) {
		printf("open inband failed!\n");
#ifdef INBANDHOST_PTHREAD_LOCK
		pthread_mutex_unlock(&inband_mutex);
#endif
		return -1;
	}
	hcd_inband_chan = chan;	   

    i = 0;
    while (inband_cmd_table[i].cmd != NULL) {
        if (0 == strcmp(argv[1], inband_cmd_table[i].cmd)) 
        {
			if (inband_cmd_table[i].func) {
			    ret = inband_cmd_table[i].func(i,argc, argv);
                printf("return from func with %d\n", ret);
		        if (ret > 0)
		            printf("OK\n");
		        else if (ret < 0)
		            printf("FAIL\n");
		    }
			break;
        }
        i++;
	}
	if(inband_cmd_table[i].cmd == NULL)//wrong cmd
	{
		printf("Input wrong command!\n");	
		print_command_list();
	}
	
	inband_close(hcd_inband_chan);
#ifdef INBANDHOST_PTHREAD_LOCK
	pthread_mutex_unlock(&inband_mutex);
#endif
	
    return 0;
}

