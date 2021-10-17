/*the internal api.h for struct defines*/
#ifndef __RTL_API_H__
#define __RTL_API_H__
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <syslog.h>

//#include "../../boa/apmib/apmib.h"
//#include "../common/common_utility.h"
#include "./rtk_firewall_adapter.h"
#include "rtk_disk_adapter.h"

#define RTK_SUCCESS (1)
#define RTK_FAILED    (0)
#define RTK_LEN_32 (32)
#define RTK_LEN_64 (64)
#define LAN_IF "br0"
#define RTK_SCHEDULE_NAME_LEN (20)
#define RTK_COMMENT_LEN (21)
#define RTK_SSID_LEN (32)
#define RTK_VERSION_LEN (32)
#define SYSCMD_LOG_FILE "/tmp/syscmdLog.txt"
#define DUMP_DIAG_LOG_FILE "/tmp/dumpDiagnosticLog.txt"
#define DUMP_DIAG_LOG_FILE_BAK "/tmp/dumpDiagnosticLog_bak.txt"

/*wan part*/
enum PPP_CONNECTION_TYPE {
	RTK_PPP_CONTINUES=0,
	RTK_PPP_MANUAL,
	RTK_PPP_ONDEMAND,
};

/*pptp & l2tp , local ip and server ip should be specificed*/
enum PPP_LOCAL_IP_SELECTION {
	RTK_PPP_LOCAL_IP_STATIC=0,  /*using static ip*/
	RTK_PPP_LOCAL_IP_DHCP,         /*using dhcp*/
};

enum PPP_REMOTE_IP_SELECTION {
	RTK_PPP_REMOTE_IP_STATIC=0,      /*using static ip*/
	RTK_PPP_REMOTE_IP_DOMAIN_NAME, /*using ip get by domain name*/
};

enum DNS_TYPE {
	RTK_DNS_AUTO=0,
	RTK_DNS_MANUAL,
};
	
struct rtk_static_config {    
	unsigned int ip;
	unsigned int mask;
	unsigned int gw;
};

struct rtk_dhcp_config {    
	char host_name[RTK_LEN_32];
};

struct rtk_lan_dhcp_config{
	unsigned int type;
	unsigned int start_ip;
	unsigned int end_ip;
	unsigned int lease_time;
};

struct rtk_ppp_config {    
	char user_name[RTK_LEN_32];
	char passwd[RTK_LEN_32];
	unsigned int connection_type;
	unsigned int idle_time;
};

struct rtk_pppoe_config {
	struct rtk_ppp_config ppp_config;
	unsigned char ac_name[RTK_LEN_32];
};

union server_info {
	unsigned char server_name[RTK_LEN_64];
	unsigned int server_ip;
};

struct rtk_l2tp_config {   
	unsigned int local_ip_selection; //0. static IP; 1. dhcp    
	struct rtk_static_config static_config;
	struct rtk_ppp_config ppp_config;
	union server_info l2tp_server_info;
	unsigned int server_info_flag; //0. by ip; 1. by domain
};

struct rtk_pptp_config {	
	unsigned int local_ip_selection; //0. static IP; 1. dhcp    
	struct rtk_static_config static_config;
	struct rtk_ppp_config ppp_config;
	union server_info pptp_server_info;
	unsigned int server_info_flag;
};

/*lan part*/
enum DHCP_TYPE {
	RTK_DHCP_DISABLE =0,
	RTK_DHCPC,
	RTK_DHCPD,
};


struct rtk_static_lease {    
	unsigned int ip; 
	unsigned char mac[6];
	//unsigned char hostName[32];
};


/* other part */
enum LINK_TYPE {
	RTK_LINK_ERROR =0,
	RTK_ETHERNET,
	RTK_5G,
	RTK_24G
};

struct rtk_link_type_info {
	enum LINK_TYPE  type;		//terminal type(2.4G,5G,ethernet)
	struct in_addr ip;			//terminal ip
	unsigned char  mac[6];		//terminal mac
	unsigned int download_speed;//terminal download speed(bytes/s)
	unsigned int upload_speed;	//terminal upload speed(bytes/s)
	unsigned int last_rx_bytes;
	unsigned int last_tx_bytes;	
	unsigned int cur_rx_bytes;
	unsigned int cur_tx_bytes;  
	unsigned int all_bytes;		//all traffic(bytes)	
	int port_number;			//terminal phy port number
	// modify by chenbo(realtek)
	unsigned char brand[16];
	unsigned int link_time;
};


struct rtk_terminal_rate_info {
	unsigned char  mac[6];		//terminal mac
	unsigned int download_speed;//terminal download speed(bytes/s)
	unsigned int upload_speed;	//terminal upload speed(bytes/s)
	unsigned int rx_bytes;
	unsigned int tx_bytes;  
	unsigned int all_bytes;		//all traffic(bytes)	
	int port_number;			//terminal phy port number
	unsigned int link_flag;
};

#define MAX_TERM_NUMBER 64

enum LINK_STATUS 
{
	LINK_ERROR,
	LINK_UP,
	LINK_DOWN
};

struct rtk_port_status
{
	enum LINK_STATUS	port_status;
	int 	port_number;
};
#define MAX_PORT_NUMBER 5
struct rtk_dhcp_client_info {
	unsigned char hostname[64];
	unsigned int ip;
	unsigned char mac[6];
	unsigned int expires;
#ifdef GET_DEVICE_BRAND_FROM_MAC
	unsigned char brand[16];
#endif
};
enum CONNECT_TYPE{
	WIRE_CONNECT,
	WIRELESS_CONNECT
};

typedef struct rtk_lan_device_info{
	unsigned char hostname[64];
	unsigned int ip;
	unsigned char mac[6];
	unsigned int expires;
	unsigned char conType;
	unsigned char brand[16];
}RTK_LAN_DEVICE_INFO_T, *RTK_LAN_DEVICE_INFO_Tp;

typedef struct rtk_arp_entry{	
	unsigned int ip;
	unsigned char mac[6];	
}RTK_ARP_ENTRY_T, *RTK_ARP_ENTRY_Tp;
struct rtk_dev_link_time
{
	unsigned char ip[16];
	unsigned char mac[24];
	unsigned int link_time;
	int is_alive;
};

struct rtk_offline_dev_info
{
	//struct in_addr ip;
	unsigned char mac[6];
	//unsigned int link_time;
	unsigned char brand[16];
	int is_static_ip;
	int is_net_limit;
	int is_speed_limit;
	int is_nas_limit;
};

/*wifi part*/

typedef struct rtk_wlan_sta_info {
 unsigned short aid;
 unsigned char  addr[6];
 unsigned long  tx_packets;
 unsigned long  rx_packets;
 unsigned long  expired_time; // 10 msec unit
 unsigned short flag;
 unsigned char  txOperaRates;
 unsigned char  rssi;
 unsigned long  link_time;  // 1 sec unit
 unsigned long  tx_fail;
 unsigned long  tx_bytes;
 unsigned long  rx_bytes;
 unsigned char  network;
 unsigned char  ht_info; // bit0: 0=20M mode, 1=40M mode; bit1: 0=longGI, 1=shortGI
 unsigned char  RxOperaRate;
 unsigned char  resv[5];
} RTK_WLAN_STA_INFO_T, *RTK_WLAN_STA_INFO_Tp;
typedef struct wlan_schedule{
	unsigned char		text[RTK_SCHEDULE_NAME_LEN];
	unsigned short	eco;
	unsigned short	from_time;
	unsigned short	to_time;
	unsigned short	day;
}RTK_SCHEDULE_T, *RTK_SCHEDULE_Tp;
typedef struct wlan_wds{
	unsigned char	macAddr[6];
	unsigned int	fixrate;
	unsigned char	comment[RTK_COMMENT_LEN];
}RTK_WDS_T, *RTK_WDS_Tp;
typedef struct wlan_acl{
	unsigned char	macAddr[6];
	unsigned char	comment[RTK_COMMENT_LEN];
}RTK_ACL_T, *RTK_ACL_Tp;



/*misc */
typedef struct rtk_user_net_device_stats {
    unsigned long long rx_packets; /* total packets received       */
    unsigned long long tx_packets; /* total packets transmitted    */
    unsigned long long rx_bytes; /* total bytes received         */
    unsigned long long tx_bytes; /* total bytes transmitted      */
    unsigned long rx_errors; /* bad packets received         */
    unsigned long tx_errors; /* packet transmit problems     */
    unsigned long rx_dropped; /* no space in linux buffers    */
    unsigned long tx_dropped; /* no space available in linux  */
    unsigned long rx_multicast; /* multicast packets received   */
    unsigned long tx_multicast; /* multicast packets transmitted   */
    unsigned long rx_unicast; /* unicast packets received   */
    unsigned long tx_unicast; /* unicast packets transmitted   */
    unsigned long rx_broadcast; /* broadcast packets received   */
    unsigned long tx_broadcast; /* broadcast packets transmitted   */
    unsigned long rx_compressed;
    unsigned long tx_compressed;
    unsigned long collisions;

    /* detailed rx_errors: */
    unsigned long rx_length_errors;
    unsigned long rx_over_errors; /* receiver ring buff overflow  */
    unsigned long rx_crc_errors; /* recved pkt with crc error    */
    unsigned long rx_frame_errors; /* recv'd frame alignment error */
    unsigned long rx_fifo_errors; /* recv'r fifo overrun          */
    unsigned long rx_missed_errors; /* receiver missed packet     */
    /* detailed tx_errors */
    unsigned long tx_aborted_errors;
    unsigned long tx_carrier_errors;
    unsigned long tx_fifo_errors;
    unsigned long tx_heartbeat_errors;
    unsigned long tx_window_errors;
}RTK_NET_DEVICE_STATS, *RTK_NET_DEVICE_STATSp; 



typedef struct eth_port_status {
    unsigned char link;
    unsigned char speed;
    unsigned char duplex;
    unsigned char nway;     
}RTK_ETH_PORT_STATUS, *RTK_ETH_PORT_STATUSp;

typedef struct system_info{
    unsigned char platform[32];
    unsigned char version[32];
    unsigned int  uptime;
    
}RTK_SYSTEM_INFO, *RTK_SYSTEM_INFOp;

typedef struct upnp_mapping_info{
	unsigned char 	desc[64];
	unsigned int 	in_ip;
	unsigned short 	iport;
	unsigned short 	eport;
	unsigned char 	proto[4];
	unsigned short 	enabled;
}RTK_UPNP_MAPPING_INFO_T,*RTK_UPNP_MAPPING_INFO_Tp;

typedef struct lan_info{
	unsigned int status;
	/* 0:Getting IP from DHCP server,
	  1:DHCP, 2:Fixed IP*/
	struct in_addr ip;
	struct in_addr mask;
	struct in_addr def_gateway;
	unsigned int dhcp_server; // 0:Disabled, 2:Enabled, others:Auto
	unsigned char mac[6];
}RTK_LAN_INFO, *RTK_LAN_INFOp;

enum WAN_STATUS {
	DISCONNECTED = 0, FIXED_IP_CONNECTED, FIXED_IP_DISCONNECTED,
	GETTING_IP_FROM_DHCP_SERVER, DHCP,
	PPPOE_CONNECTED, PPPOE_DISCONNECTED,
	PPTP_CONNECTED, PPTP_DISCONNECTED,
	L2TP_CONNECTED, L2TP_DISCONNECED,
	USB3G_CONNECTED, USB3G_MODEM_INIT,
	USB3G_DAILING, USB3G_REMOVED, USB3G_DISCONNECTED
};
typedef struct wan_info{
	enum WAN_STATUS status;
	struct in_addr ip;
	struct in_addr mask;
	struct in_addr def_gateway;
	struct in_addr dns1;
	struct in_addr dns2;
	struct in_addr dns3;
	unsigned char mac[6];
}RTK_WAN_INFO, *RTK_WAN_INFOp;

typedef struct wlan_info{
	unsigned int enabled;
	unsigned int mode;
	unsigned int network_type;
	unsigned int band;
	unsigned int channel_num;
	unsigned int channel_width;
	unsigned int encryption;
	unsigned int client_num;
	unsigned int state;
	unsigned char BBSID[6];
	char SSID[SSID_LEN+1];
}RTK_WLAN_INFO, *RTK_WLAN_INFOp;

typedef struct link_status{
	unsigned long long rev_total;
	unsigned long long snd_total;
	unsigned long long drop;
	unsigned int drop_rate;
}RTK_LINK_STATUS, *RTK_LINK_STATUSp;

typedef struct download_statics{
	unsigned long avg_download_speed;
	unsigned long max_download_speed;
	unsigned long min_download_speed;
	unsigned long cur_download_speed;
	unsigned long long download_total;
}RTK_DOWNLOAD_STATICS, *RTK_DOWNLOAD_STATICSp;

typedef struct upload_statics{
	unsigned long avg_upload_speed;
	unsigned long max_upload_speed;
	unsigned long min_upload_speed;
	unsigned long cur_upload_speed;
	unsigned long long upload_total;
}RTK_UPLOAD_STATICS, *RTK_UPLOAD_STATICSp;

typedef struct rtk_time{
	unsigned long day;
	unsigned long hour;
	unsigned long min;
	unsigned long sec;
}RTK_TIME, *RTK_TIMEp;

typedef struct firmware_info{
	char version[16];
	char buildtime[64];
}RTK_FIRMWARE_INFO, *RTK_FIRMWARE_INFOp;
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)

typedef struct rtk_qos_mnp_info
{
	unsigned int enabled;
	unsigned char macAddr[6];
	unsigned int qosTime;
}RTK_QOSMNP_INFO, *RTK_QOSMNP_INFOp;

#endif

/*lan adapter api*/
int rtk_set_lan_ip(struct rtk_static_config *p_static_config);
int rtk_get_lan_ip(struct rtk_static_config *p_static_config);
int rtk_set_lan_dhcp(struct rtk_lan_dhcp_config *p_config);
int rtk_get_lan_dhcp(struct rtk_lan_dhcp_config *p_config);
int rtk_add_lan_static_dhcp(struct rtk_static_lease * s_lease);
int rtk_del_lan_static_dhcp(struct rtk_static_lease * s_lease,unsigned int delall);
int rtk_get_lan_static_dhcp(unsigned int *num, struct rtk_static_lease *pLease, unsigned int empty_entry_num);
int rtk_set_lan_static_dhcp_enable(unsigned int enable);
int rtk_get_lan_static_dhcp_enable(unsigned int* enable);
int rtk_set_lan_clone_mac(unsigned char *mac);
int rtk_get_lan_clone_mac(unsigned char *mac);
int rtk_get_dhcp_client_list(unsigned int *num, struct rtk_dhcp_client_info *pclient);
int rtk_get_device_brand(unsigned char *mac, char *mac_file, char *brand);
int rtk_get_lan_device_info(unsigned int *num, RTK_LAN_DEVICE_INFO_Tp pdevinfo);

/*wan adapter api*/
int rtk_set_wan_type(unsigned int type);
int rtk_set_wan_static(struct rtk_static_config *pstatic_config);
int rtk_set_wan_dhcp(struct rtk_dhcp_config *pdhcp_config);
int rtk_set_wan_pppoe(struct rtk_pppoe_config *pppoe_config);
int rtk_set_wan_l2tp(struct rtk_l2tp_config *pl2tp_config);
int rtk_set_wan_pptp(struct rtk_pptp_config *pptp_config);
int rtk_set_wan_mtu(unsigned int mtu);
int rtk_set_wan_dns(unsigned int dns_type, unsigned int dns1, unsigned int dns2, unsigned dns3);
int rtk_set_wan_clone_mac(unsigned char *mac);
int rtk_get_wan_type(unsigned int *type);
int rtk_get_wan_static(struct rtk_static_config *pstatic_config);
int rtk_get_wan_dhcp(struct rtk_dhcp_config *pdhcp_config);
int rtk_get_wan_pppoe(struct rtk_pppoe_config *ppppoe_config);
int rtk_get_wan_l2tp(struct rtk_l2tp_config *pl2tp_config);
int rtk_get_wan_pptp(struct rtk_pptp_config *ppptp_config);
int rtk_get_wan_mtu(unsigned int *mtu);
int rtk_get_wan_dns(unsigned int *dns_type, unsigned int *dns1, unsigned int *dns2, unsigned *dns3);
int rtk_get_wan_clone_mac(unsigned char *mac);
int rtk_close_wan_connection(void);
int rtk_start_wan_connection(void);
int rtk_restart_wan(void);

/* firewall adapter api */
int rtk_set_port_forward_enable(unsigned int enabled);
int rtk_add_port_forward_entry(RTK_PORTFW_T *pPortFW);
int rtk_del_port_forward_entry(RTK_PORTFW_T *pPortFW, unsigned int delall);
int rtk_get_port_forward_enable(unsigned int *enabled);
int rtk_get_port_forward_entry(unsigned int *num, RTK_PORTFW_T *pPortFW, unsigned int empty_entry_num);

int rtk_set_port_filter_enable(unsigned int enabled);
int rtk_add_port_filter_entry(RTK_PORTFILTER_T *pPortFilter);
int rtk_del_port_filter_entry(RTK_PORTFILTER_T *pPortFilter, unsigned int delall);
int rtk_get_port_filter_enable(unsigned int *enabled);
int rtk_get_port_filter_entry(unsigned int *num, RTK_PORTFILTER_T *pPortFilter, unsigned int empty_entry_num);

int rtk_set_ip_filter_enable(unsigned int enabled);
int rtk_add_ip_filter_entry(RTK_IPFILTER_T *pIpFilter);
int rtk_del_ip_filter_entry(RTK_IPFILTER_T *pIpFilter, unsigned int delall);
int rtk_get_ip_filter_enable(unsigned int *enabled);
int rtk_get_ip_filter_entry(unsigned int *num, RTK_IPFILTER_T *pIpFilter, unsigned int empty_entry_num);

int rtk_set_mac_filter_enable(unsigned int enabled);
int rtk_add_mac_filter_entry(RTK_MACFILTER_T *pMacFilter);
int rtk_del_mac_filter_entry(RTK_MACFILTER_T *pMacFilter, unsigned int delall);
int rtk_get_mac_filter_enable(unsigned int *enabled);
int rtk_get_mac_filter_entry(unsigned int *num, RTK_MACFILTER_T *pMacFilter, unsigned int empty_entry_num);


int rtk_set_nas_filter_enable(unsigned int enabled);
int rtk_get_nas_filter_enable(unsigned int *enabled);
int rtk_add_nas_filter_entry(RTK_NASFILTER_T *pNasFilter);
int rtk_del_nas_filter_entry(RTK_NASFILTER_T *pNasFilter, unsigned int delall);
int rtk_get_nas_filter_entry(unsigned int *num, RTK_NASFILTER_T *pNasFilter, unsigned int empty_entry_num);

int rtk_set_url_filter_enable(unsigned int enabled);
int rtk_set_url_filter_mode(int url_filter_mode);
int rtk_add_url_filter_entry(RTK_URLFILTER_T *pUrlFilter);
int rtk_del_url_filter_entry(RTK_URLFILTER_T *pUrlFilter,unsigned int delall);
int rtk_get_url_filter_enable(unsigned int *enabled);
int rtk_get_url_filter_mode(int *url_filter_mode);
int rtk_get_url_filter_entry(unsigned int *num, RTK_URLFILTER_T *pUrlFilter, unsigned int empty_entry_num);

int rtk_set_dmz(unsigned int enabled, unsigned int dmz_ip);
int rtk_get_dmz(unsigned int *enabled, unsigned int *dmz_ip);

#if defined(VLAN_CONFIG_SUPPORTED)
int rtk_set_vlan_enable(unsigned int enabled);
int rtk_modify_vlan_config(RTK_VLAN_CONFIG_T *pVlanConfig);
int rtk_get_vlan_enable(unsigned int *enabled);
int rtk_get_vlan_config(RTK_VLAN_CONFIG_T *pVlanConfig);
#endif
#if defined(QOS_BY_BANDWIDTH)
int rtk_set_qos_enable(unsigned int enabled);
int rtk_set_qos_speed(unsigned int uplink_speed, unsigned downlink_speed);
int rtk_modify_qos_rule_entry(RTK_IPQOS_T *pQos);
int rtk_add_qos_rule_entry(RTK_IPQOS_T *pQos);
int rtk_del_qos_rule_entry(RTK_IPQOS_T *pQos, unsigned int delall);
int rtk_get_qos_enable(unsigned int *enabled);
int rtk_get_qos_speed(unsigned int *uplink_speed, unsigned int *downlink_speed);
int get_qos_rule_entry(unsigned int *num, RTK_IPQOS_T *pQos, unsigned int empty_entry_num);
int rtk_get_qos_rule_entry_by_index(RTK_IPQOS_T *pQos, unsigned int index);
int rtk_get_qos_rule_entry_index(RTK_IPQOS_T *pQos);

#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
int rtk_get_qos_rule_monopoly(unsigned int* enabled, unsigned char * macAddr, unsigned int* qosTime);
int rtk_set_qos_rule_monopoly(unsigned int enabled, unsigned char * macAddr, unsigned int qosTime);
int rtk_set_qos_rule_monopoly_immediately(unsigned int enabled, unsigned char * macAddr, unsigned int qosTime);
int rtk_get_qos_rule_monopoly_info(RTK_QOSMNP_INFOp info);
int rtk_get_qos_rule_entry_num(unsigned int *num);

#endif
#endif

/* disk related */
int rtk_disk_format(unsigned char *partition_name, unsigned char *systype);
int rtk_get_storage_info(unsigned int *num, RTK_DEVICEINFO_T *pinfo, unsigned int empty_entry_num);

/* wifi adapter api */
int rtk_wlan_disable(unsigned char *ifname);
int rtk_wlan_enable(unsigned char *ifname);
int rtk_wlan_down_interface(unsigned char *ifname);
int rtk_wlan_up_interface(unsigned char *ifname);
int rtk_wlan_immediately_work(unsigned char *ifname);
int rtk_wlan_set_mode(unsigned char *ifname, unsigned int mode);
int  rtk_wlan_set_ssid(unsigned char *ifname, unsigned char *ssid);
int rtk_wlan_set_passwd(unsigned char *ifname, unsigned char *passwd);
int rtk_wlan_set_band(unsigned char *ifname, unsigned int band);
int rtk_wlan_set_band_immediately(unsigned char *ifname, unsigned int band);
int rtk_wlan_set_network_type(unsigned char *ifname, unsigned int type);
int rtk_wlan_set_channel_bandwidth(unsigned char *ifname, unsigned int bandwidth);
int rtk_wlan_set_channel_bandwidth_immediately(unsigned char *ifname, unsigned int bandwidth);
int rtk_wlan_set_2ndchoff(unsigned char *ifname, unsigned int offset);
int rtk_wlan_set_channel(unsigned char *ifname, unsigned int channel);
int rtk_wlan_set_channel_immediately(unsigned char *ifname, unsigned int channel);
int rtk_wlan_set_hidden_ssid(unsigned char *ifname, unsigned int hidden);
int rtk_wlan_set_fixrate(unsigned char *ifname, unsigned int rate);
int rtk_wlan_set_txrestrict(unsigned char *ifname, unsigned int bandwith);
int rtk_wlan_set_rxrestrict(unsigned char *ifname, unsigned int bandwith);
int rtk_wlan_set_enable_repeater(unsigned char *ifname);
int rtk_wlan_set_disable_repeater(unsigned char *ifname);
int rtk_wlan_set_repeater_ssid(unsigned char *ifname,unsigned char *ssid);
int rtk_wlan_set_fragthres(unsigned char *ifname, unsigned int fragthres);
int rtk_wlan_set_rtsthres(unsigned char *ifname, unsigned int rtsthres);
int rtk_wlan_set_beacon_interval(unsigned char *ifname, unsigned int interval);
int rtk_wlan_set_preamble(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_protection(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_aggregation(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_shortgi(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_isolation(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_stbc(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_ldpc(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_coexist(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_txbeamforming(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_mc2u(unsigned char *ifname, unsigned int enable);
int rtk_wlan_set_rfpower(unsigned char *ifname, unsigned int level);
int rtk_wlan_set_rfpower_immediately(unsigned char *ifname, unsigned int level);
int rtk_wlan_set_encryption(unsigned char *ifname,unsigned int encrypt);
int rtk_wlan_set_security_wep(unsigned char *ifname, unsigned int auth_type, unsigned int key_len, unsigned int key_type, unsigned int default_key_idx, unsigned char *key);
int rtk_wlan_set_security_wpa(unsigned char *ifname, unsigned int auth_type, unsigned int cipher_suite, unsigned int key_type, unsigned char *key);
int rtk_wlan_set_security_wpa2(unsigned char *ifname, unsigned int auth_type, unsigned int cipher_suite, unsigned int key_type, unsigned char *key);
int rtk_wlan_set_security_wpamix(unsigned char *ifname, unsigned int auth_type, unsigned int wpa_cipher_suite, unsigned int wpa2_cipher_suite, unsigned int key_type, unsigned char *key);
int rtk_wlan_set_wds(unsigned char *ifname, unsigned int enable);
int rtk_wlan_add_wds_entry(unsigned char *ifname, unsigned char *mac);
int rtk_wlan_del_wds_entry(unsigned char *ifname,unsigned char *mac, unsigned int delall);
int rtk_wlan_set_acl_mode(unsigned char *ifname, unsigned int acl_mode);
int rtk_wlan_add_acl_entry(unsigned char *ifname, unsigned char *mac);
int rtk_wlan_del_acl_entry(unsigned char *ifname, unsigned char *mac, unsigned int delall);
int rtk_wlan_set_schdule(unsigned char *ifname, unsigned int enable);
int rtk_wlan_add_schedule_entry(unsigned char *ifname, RTK_SCHEDULE_T *entry);
int rtk_wlan_del_schedule_entry(unsigned char *ifname, RTK_SCHEDULE_T *entry, unsigned int delall);
int rtk_wlan_get_enable(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_mode(unsigned char *ifname, unsigned int *mode);
int rtk_wlan_get_ssid(unsigned char *ifname, unsigned char *ssid);
int rtk_wlan_get_passwd(unsigned char *ifname, unsigned char *passwd);
int rtk_wlan_get_band(unsigned int *ifname, unsigned int *band);
int rtk_wlan_get_network_type(unsigned char  *ifname, unsigned int *type);
int rtk_wlan_get_channel_bandwidth(unsigned char *ifname, unsigned int *bandwidth);
int rtk_wlan_get_2ndchoff(unsigned char *ifname, unsigned int *offset);
int rtk_wlan_get_channel(unsigned char *ifname, unsigned int *channel);
int rtk_wlan_get_hidden_ssid(unsigned char *ifname, unsigned int* hidden);
int rtk_wlan_get_fixrate(unsigned char *ifname, unsigned int* rate);
int rtk_wlan_get_txrestrict(unsigned char *ifname, unsigned int *bandwith);
int rtk_wlan_get_rxrestrict(unsigned char *ifname, unsigned int *bandwith);
int rtk_wlan_get_enable_repeater(unsigned char *ifname,unsigned int *enable);
int rtk_wlan_get_repeater_ssid(unsigned char *ifname,unsigned char *ssid);
int rtk_wlan_get_fragthres(unsigned char *ifname, unsigned int *fragthres);
int rtk_wlan_get_rtsthres(unsigned char *ifname, unsigned int *rtsthres);
int rtk_wlan_get_beacon_interval(unsigned char *ifname, unsigned int *interval);
int rtk_wlan_get_preamble(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_protection(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_aggregation(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_shortgi(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_isolation(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_stbc(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_ldpc(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_coexist(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_txbeamforming(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_mc2u(unsigned char *ifname, unsigned int *enable);
int rtk_wlan_get_rfpower(unsigned char *ifname, unsigned int *level);
int rtk_wlan_get_encryption(unsigned char *ifname,unsigned int *encrypt);
int rtk_wlan_get_security_wep(unsigned char *ifname, unsigned int *auth_type, unsigned int *key_len, unsigned int *key_type, unsigned int *default_key_idx, unsigned char *key);
int rtk_wlan_get_security_wpa(unsigned char *ifname, unsigned int *auth_type, unsigned int *cipher_suite, unsigned int *key_type, unsigned char *key);
int rtk_wlan_get_security_wpa2(unsigned char *ifname, unsigned int *auth_type, unsigned int *cipher_suite, unsigned int *key_type, unsigned char *key);
int rtk_wlan_get_security_wpamix(unsigned char *ifname, unsigned int *auth_type, unsigned int *wpa_cipher_suite, unsigned int *wpa2_cipher_suite, unsigned int *key_type, unsigned char *key);
int rtk_wlan_get_wds(unsigned char *ifname, unsigned int* enable);
int rtk_wlan_get_wds_entry_num(unsigned char *ifname,unsigned int* n);
int rtk_wlan_get_wds_entry(unsigned char *ifname,unsigned int* n, unsigned char *mac, unsigned int empty_num);
int rtk_wlan_get_acl_mode(unsigned char *ifname, unsigned int *acl_mode);
int rtk_wlan_get_acl_entry(unsigned char *ifname,unsigned int *mac_num,unsigned char* mac, unsigned int empty_num);
int rtk_wlan_get_schdule(unsigned char *ifname, unsigned int* enable);
int rtk_wlan_get_schdule_entry_num(unsigned char *ifname,unsigned int* n);
int rtk_wlan_get_schedule_entry(unsigned char *ifname,unsigned int *n,RTK_SCHEDULE_T *entry, unsigned int empty_num);
int rtk_get_wlan_sta( unsigned char *ifname,  RTK_WLAN_STA_INFO_Tp pInfo);
int rtk_get_wlan_info(RTK_WLAN_INFOp info, char *iface);

/*misc api*/
int rtk_set_upnp_enable(int enable);
int rtk_get_upnp_enable(int *enable);
int rtk_get_upnp_list(RTK_UPNP_MAPPING_INFO_T *info, unsigned int* n,unsigned int max_num);
int rtk_get_mib_value(int id, void *value);
int rtk_set_mib_value(int id, void *value);
/*other adapter api*/
int rtk_get_internet_status(int *connected);
int rtk_get_phy_port_status(struct rtk_port_status *info);
int rtk_get_link_status(RTK_NET_DEVICE_STATSp link_info, char *iface);
int rtk_get_download_statics(RTK_DOWNLOAD_STATICSp download);
int rtk_get_upload_statics(RTK_UPLOAD_STATICSp upload);
int rtk_get_router_info(char**version,char**cpu);
int rtk_get_uptime(RTK_TIMEp uptime);
int rtk_get_firmware_info(RTK_FIRMWARE_INFOp info);
int rtk_get_lan_info(RTK_LAN_INFOp info);
int rtk_get_wan_info(RTK_WAN_INFOp info);
int rtk_reboot();
int rtk_restore_default();

int rtk_api_init();
int rtk_update();
int rtk_sys_reinit();
int rtk_set_user_passwd(unsigned char *user, unsigned char *passwd);
int rtk_get_user_passwd(unsigned char *user, unsigned char *passwd);
int rtk_get_default_mac_address(unsigned char* mac, char* iface);
int rtk_get_lan_drop_rate(unsigned int *drop_rate);
int rtk_get_wan_drop_rate(unsigned int *drop_rate);
int rtk_get_wlan_drop_rate(unsigned int *drop_rate,unsigned char* wlan_if);
int rtk_get_wan_status_by_url(unsigned int* connected,char* url);
int rtk_get_pppoe_err_code(int *err_code);

int rtk_get_terminal_info(unsigned int *real_num,struct rtk_link_type_info *info, unsigned int empty_entry_num);
int rtk_get_ports_status(unsigned int *real_num,struct rtk_port_status *info, unsigned int empty_entry_num);
#ifdef CONFIG_RTL_DNS_TRAP
int rtk_enable_domain_access(unsigned int enable);
int rtk_set_domain_access_url(char* url);
#ifdef CONFIG_RTL_HTTP_REDIRECT
int rtk_enable_welcome_page(unsigned int enable);
int rtk_set_welcome_page(char* page_name);
#endif
#endif
int rtk_start_pppoe(void);

int rtk_nas_login(char * username,char * password);
int rtk_get_nas_info();
int rtk_nas_logout();
//int rtk_get_cgi_disk_formatable(int *formatable);
//int rtk_cgi_disk_format();
int rtk_check_upgradable(int *upgradable);
int rtk_upgrade();
int rtk_check_upgrade_result(int *upgrad_result);
int rtk_send_le_id_pass(char * username,char * password);
int rtk_logout_le_id_pass(char * username);
int rtk_upload_log();
int rtk_query_software_version(char *version);
int rtk_is_wizard_done(int *is_wizard_done);
// rtk_common_cgi_api(char * httpMethod,char *url,char*httpData,int parmNum,char *nameArray[],char*outPutArray[]);


void rtk_set_terminal_ip(char *ip_addr);

void rtk_command_syslog(char * sysCmd);
void rtk_dump_log_command(char * command);
void rtk_sys_diagnostic();
#endif
