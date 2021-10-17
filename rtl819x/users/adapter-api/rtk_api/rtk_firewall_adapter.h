/*the internal api.h for struct defines*/
#ifndef __RTK_FIREWALL_ADAPTER_H__
#define __RTK_FIREWALL_ADAPTER_H__
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/sockios.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/stat.h>
//#include <net/if.h>
#include <linux/sockios.h>
#include <arpa/inet.h>

//#include "rtk_api.h"
#include "../common/common_utility.h"


#define RTK_FIREWALL_ADAPTER_DEBUG

#define RTK_BRIDGE_INTERFACE_NAME "br0"
#define RTK_MAX_PORT_NUMBER 65535

#define RTK_FW_COMMENT_LEN  21
#define RTK_FW_IFNAMSIZE  16 
#define RTK_FW_MAX_QOS_NAME_LEN 15
#define RTK_FW_MAC_ADDR_LEN 6

typedef struct port_forward {
	unsigned char ipAddr[4];
	unsigned short fromPort;
	unsigned short toPort;
	unsigned char protoType; /* PROTO_BOTH=3, PROTO_TCP=1, PROTO_UDP=2 */
	unsigned short svrport;
	unsigned char svrName[RTK_FW_COMMENT_LEN];
	unsigned int InstanceNum;
	unsigned int WANIfIndex;
	unsigned char comment[RTK_FW_COMMENT_LEN];
}__attribute__ ((packed)) RTK_PORTFW_T, *RTK_PORTFW_Tp;

typedef struct port_filter {
	unsigned short fromPort;
	unsigned short toPort;
	unsigned char protoType; /* PROTO_BOTH=3, PROTO_TCP=1, PROTO_UDP=2 */
	unsigned char comment[RTK_FW_COMMENT_LEN];
	unsigned char ipVer;
}__attribute__ ((packed)) RTK_PORTFILTER_T, *RTK_PORTFILTER_Tp;

typedef struct ip_filter {
	unsigned char ipAddr[4];
	unsigned char protoType; /* PROTO_BOTH=3, PROTO_TCP=1, PROTO_UDP=2 */
	unsigned char comment[RTK_FW_COMMENT_LEN];
	#ifdef CONFIG_IPV6
	unsigned char ip6Addr[48];
	unsigned char ipVer;
	#endif
}__attribute__ ((packed)) RTK_IPFILTER_T, *RTK_IPFILTER_Tp;

typedef struct mac_filter {
	unsigned char macAddr[6];
	unsigned char comment[RTK_FW_COMMENT_LEN];
}__attribute__ ((packed)) RTK_MACFILTER_T, *RTK_MACFILTER_Tp;

typedef struct nas_filter {
	unsigned char macAddr[6];
	unsigned char comment[RTK_FW_COMMENT_LEN];
}__attribute__ ((packed)) RTK_NASFILTER_T, *RTK_NASFILTER_Tp;

typedef struct url_filter {	
	unsigned char urlAddr[31];
	unsigned char ruleMode;
}__attribute__ ((packed)) RTK_URLFILTER_T, *RTK_URLFILTER_Tp;

#ifdef VLAN_CONFIG_SUPPORTED
typedef struct vlan_config_entry {
	unsigned char enabled;
	unsigned char netIface[RTK_FW_IFNAMSIZE];
	unsigned char tagged;
	unsigned char priority;
	unsigned char cfi;
	unsigned short vlanId;
#if defined(CONFIG_RTK_VLAN_NEW_FEATURE) ||defined(CONFIG_RTL_HW_VLAN_SUPPORT)
	unsigned char forwarding_rule;
#endif
}__attribute__ ((packed)) RTK_VLAN_CONFIG_T, *RTK_VLAN_CONFIG_Tp;
#endif

#ifdef QOS_BY_BANDWIDTH
typedef struct qos_tbl_entry {
	unsigned char entry_name[RTK_FW_MAX_QOS_NAME_LEN+1];
	unsigned char enabled;
	unsigned char mac[RTK_FW_MAC_ADDR_LEN];
	unsigned char mode;
	unsigned char local_ip_start[4];
	unsigned char local_ip_end[4];
	unsigned long bandwidth;
	unsigned long bandwidth_downlink;
	unsigned char l7_protocol[64+1];
	unsigned char ip6_src[40];
}__attribute__ ((packed)) RTK_IPQOS_T, *RTK_IPQOS_Tp;
#endif


#endif
