#ifndef __COMMON_UTILITY_H__
#define __COMMON_UTILITY_H__

#define IFACE_FLAG_T 0x01
#define IP_ADDR_T 0x02
#define NET_MASK_T 0x04
#define HW_ADDR_T 0x08

#define SSID_LEN 32

typedef enum { IP_ADDR, DST_IP_ADDR, SUBNET_MASK, DEFAULT_GATEWAY, HW_ADDR } ADDR_T;

typedef enum _rtk_wlan_mac_state {
    RTK_STATE_DISABLED=0, RTK_STATE_IDLE, RTK_STATE_SCANNING, RTK_STATE_STARTED, RTK_STATE_CONNECTED, RTK_STATE_WAITFORKEY
} rtk_wlan_mac_state;

typedef struct bss_info{
    unsigned char state;
    unsigned char channel;
    unsigned char txRate;
    unsigned char bssid[6];
    unsigned char rssi, sq;	// RSSI  and signal strength
    unsigned char ssid[SSID_LEN+1];
} RTK_BSS_INFO, *RTK_BSS_INFOp;

int rtk_getInAddr( char *interface, int type, void *pAddr );
int isConnectPPP();
int getWanLink(char *interface);
int getDefaultRoute(char *interface, struct in_addr *route);
int getInterfaces(char* lanIface,char* wanIface);
int getWlBssInfo(char *interface, RTK_BSS_INFOp pInfo);
int isDhcpClientExist(char *name);
int getWlStaNum( char *interface, int *num );
int check_wlan_downup(char wlanIndex);
int SetWlan_idx(char * wlan_iface_name);
#ifdef CONFIG_SMART_REPEATER
int getWispRptIface(char**pIface,int wlanId);
#endif

#endif
