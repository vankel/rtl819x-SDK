/* Include Files */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/wireless.h>
#include <sys/sysinfo.h>
#include <time.h> 
#include "./1x_md5c.h"

#define ACK_DEST_PORT	8864
#define SEND_ACK_NUM	20
#define MAX_SC_TIME	30
#define MAX_WPS_TIME	30
#define SC_STATUS_FILE	"/var/sc_status"
#define SC_SECURITY_FILE	"/var/sc_security"
#define BR_IFACE_FILE	"/var/system/br_iface"
#define SC_IP_STATUS_FILE	"/var/sc_ip_status"

#define SC_NONCE_LEN	64
#define SC_DIGEST_LEN	16
#define SC_MAX_NAME_LEN	64
#define SC_ACK_ROUND	5

#define SC_SCAN				0x00
#define SC_SAVE_PROFILE		0x01
#define SC_DEL_PROFILE		0x02
#define SC_RENAME			0x03
#define SC_SUCCESS_ACK		0x04
#define SC_FAIL_ACK			0x10


#define SC_SUCCESS_IP		0x10
#define SC_DHCP_GETTING_IP	0x00
#define SC_DHCP_STATIC_IP	0x01
#define SC_DHCP_GOT_IP		0x02

#define SC_CONTROL_ACK_SAVE		0x01
#define SC_CONTROL_ACK_DEL		0x02
#define SC_CONTROL_ACK_RENAME	0x03

#define SC_RSP_ACK			0x20
#define SC_RSP_SCAN			0x21
#define SC_RSP_SAVE			0x22
#define SC_RSP_DEL			0x23
#define SC_RSP_RENAME		0x24
#define SC_RSP_INVALID		0x3f

#define SC_REINIT_SYSTEM	1
#define SC_REINIT_WLAN		2

//#define WPS_SC_CONCURRENT
#define SIMPLE_CONFIG_PBC_SUPPORT

unsigned char SC_WLAN_IF[32]={0};
int g_sc_support_wps=0;
int g_sc_pbc_duration_time=0;
int g_sc_connect_status=0;
int g_wps_duration_time=0;
int g_sc_link_time=0;
unsigned char config_prefix[16]={0};
unsigned char config_prefix_sync[16]={0};

typedef enum { IP_ADDR, DST_IP_ADDR, SUBNET_MASK, DEFAULT_GATEWAY, HW_ADDR } ADDR_T;

//#if defined(CONFIG_APP_WSC)
enum { WSC_AUTH_OPEN=1, WSC_AUTH_WPAPSK=2, WSC_AUTH_SHARED=4, WSC_AUTH_WPA=8, WSC_AUTH_WPA2=0x10, WSC_AUTH_WPA2PSK=0x20, WSC_AUTH_WPA2PSKMIXED=0x22 };
enum { WSC_ENCRYPT_NONE=1, WSC_ENCRYPT_WEP=2, WSC_ENCRYPT_TKIP=4, WSC_ENCRYPT_AES=8, WSC_ENCRYPT_TKIPAES=12 };
//#endif

#if defined(CONFIG_RTL_SIMPLE_CONFIG_USE_WPS_BUTTON)
#define WPS_BUTTON_HOLD_TIME 3
#endif
typedef struct rtk_sc_ctx{
	unsigned char sc_wlan_ifname[16];
	unsigned char	sc_mib_prefix[16];
	unsigned char	sc_mib_sync_prefix[16];
	unsigned char sc_wlan_status;
	unsigned char sc_pin[65];
	unsigned char sc_default_pin[65];
	unsigned char sc_ssid[64];
	unsigned char sc_passwd[65];
	unsigned char sc_config_file[256];
	unsigned char	sc_device_name[64];
	unsigned short sc_device_type;
	unsigned int 	sc_run_time;
	unsigned int 	sc_linked_time;
	unsigned int 	sc_control_ip;
	unsigned int	sc_send_ack;
	unsigned int 	sc_config_success;
	int 	sc_pbc_duration_time;
	int 	sc_pbc_pressed_time;
	int	sc_wps_support;
	int	sc_wps_duration_time;
	int	sc_max_wps_run_time;
	int	sc_send_ack_num;
	int 	sc_save_profile;
	int 	sc_status;
	int 	sc_sync_profile;
	int	sc_debug;
	int 	sc_ip_status;
}RTK_SC_CTX, *RTK_SC_CTXp; 

RTK_SC_CTX g_sc_ctx;

struct ack_msg{
	unsigned char 	flag;
	unsigned short 	length;
	unsigned char 	smac[6];
	unsigned char 	status;
	unsigned short 	device_type;
	unsigned int		device_ip;
	unsigned char 	device_name[SC_MAX_NAME_LEN];
	
}__attribute__((packed));

struct scan_msg{
	unsigned char 	flag;
	unsigned short 	length;
	unsigned char		sec_level;
	unsigned char 	nonce[SC_NONCE_LEN];
	unsigned char 	digest[SC_DIGEST_LEN];
	unsigned char 	smac[6];
	unsigned short 	device_type;	
}__attribute__((packed));


struct request_msg{
	unsigned char 	flag;
	unsigned short 	length;
	unsigned char		sec_level;
	unsigned char 	nonce[SC_NONCE_LEN];
	unsigned char 	digest1[SC_DIGEST_LEN];
	unsigned char 	digest2[SC_DIGEST_LEN];
	unsigned char		device_name[SC_MAX_NAME_LEN];
}__attribute__((packed));

struct response_msg{
	unsigned char 	flag;
	unsigned short 	length;
	unsigned char 	status;
	unsigned char 	info[32];	
}__attribute__((packed));
