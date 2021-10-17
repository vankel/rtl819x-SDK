/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 */

#ifndef __RTK_L34_H__
#define __RTK_L34_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

typedef enum rtk_l34_table_type_e
{
    L34_ROUTING_TABLE,
    L34_PPPOE_TABLE,
    L34_NEXTHOP_TABLE,
    L34_NETIF_TABLE,
    L34_INTIP_TABLE,
    L34_ARP_TABLE,
    L34_NAPTR_TABLE,
    L34_NAPT_TABLE,
    L34_IPV6_ROUTING_TABLE,
    L34_BINDING_TABLE,
    L34_IPV6_NEIGHBOR_TABLE,
    L34_WAN_TYPE_TABLE,
    L34_TABLE_END
}rtk_l34_table_type_t;






typedef struct rtk_l34_netif_entry_s{

	uint16     mtu;
	uint8      enable_rounting;
	rtk_mac_t  gateway_mac;
	uint8      mac_mask;
	uint16     vlan_id;
	uint8      valid ;
}rtk_l34_netif_entry_t;


typedef struct rtk_l34_pppoe_entry_s{
    uint32          sessionID;
} rtk_l34_pppoe_entry_t;



typedef struct rtk_l34_arp_entry_s {
	uint32  nhIdx;
	uint32  valid;
} rtk_l34_arp_entry_t;



typedef enum rtk_l34_extip_table_type_e
{
    L34_EXTIP_TYPE_NAPT,
    L34_EXTIP_TYPE_NAT,
    L34_EXTIP_TYPE_LP,
    L34_EXTIP_TYPE_END,
}rtk_l34_extip_table_type_t;





typedef struct rtk_l34_ext_intip_entry_s {
	    ipaddr_t 	extIpAddr;
	    ipaddr_t 	intIpAddr;
        uint8       valid;
        rtk_l34_extip_table_type_t   type;
        uint32 		nhIdx; /*index of next hop table*/
        uint8       prival;
        uint8       pri;
} rtk_l34_ext_intip_entry_t;



typedef enum rtk_l34_nexthop_type_e
{
    L34_NH_ETHER  = 0,
    L34_NH_PPPOE  = 1,
    L34_NH_END
}rtk_l34_nexthop_type_t;


typedef struct rtk_l34_nexthop_entry_s {
	rtk_l34_nexthop_type_t  type;
	uint8  ifIdx;
	uint8  pppoeIdx;
    uint16 nhIdx;
} rtk_l34_nexthop_entry_t;



typedef enum rtk_l34_routing_type_e
{
    L34_PROCESS_CPU  = 0,
    L34_PROCESS_DROP = 1,
    L34_PROCESS_ARP =  2,
    L34_PROCESS_NH =   3,
    L34_PROCESS_END
}rtk_l34_routing_type_t;



typedef enum rtk_l34_wanroute_act_e
{
    L34_WAN_ROUTE_FWD         = ACTION_FORWARD,
    L34_WAN_ROUTE_FWD_TO_CPU  = ACTION_TRAP2CPU,
    L34_WAN_ROUTE_DROP        = ACTION_DROP,
    L34_WAN_ROUTE_END         = ACTION_END
} rtk_l34_wanroute_act_t;


typedef struct rtk_l34_routing_entry_s {
	    ipaddr_t ipAddr;
	    uint32   ipMask; /*0: no mask  ... 30:0xFFFFFFFE 31:0xFFFFFFFF maks all*/
	    rtk_l34_routing_type_t process; /*0: CPU, 1:drop, 2:ARP(local route), 4:nexthop(global route)*/
        uint8  valid;
        uint8  internal;
        /* process = ARP */
        uint32 netifIdx;
        uint32 arpStart;
	    uint32 arpEnd;
        /* process = nexthop */
	    uint8 nhStart; /*exact index*/
	    uint8 nhNum;   /*exact number*/
	    uint8 nhNxt;
	    uint8 nhAlgo;
	    uint8 ipDomain;
	    uint32 rt2waninf;
} rtk_l34_routing_entry_t;




typedef enum rtk_l34_naptInboundType_e
{
    L34_NAPTR_ENTRY_INVALID          = 0,
    L34_NAPTR_ENTRY_NO_HASH          = 1,
    L34_NAPTR_ENTRY_IP_PORT_HASH     = 2,
    L34_NAPTR_ENTRY_IP_HASH          = 3,
    L34_NAPTR_ENTRY_END
} rtk_l34_naptInboundType_t;


typedef struct rtk_l34_naptInbound_entry_s {
	uint32  intIp;
    uint16  intPort;
    uint16  remHash;
    uint8   extIpIdx;
    uint8	extPortLSB;
	uint8	isTcp;
    uint8 	valid;
    uint8  	priValid;
    uint8  	priId;
} rtk_l34_naptInbound_entry_t;


typedef struct rtk_l34_naptOutbound_entry_s {
    uint16	    hashIdx;
    uint8	    valid;
    uint8	    priValid;
    uint8	    priValue;
} rtk_l34_naptOutbound_entry_t;



/*IPv6 routing table*/

typedef enum rtk_l34_ipv6RouteType_e
{
    L34_IPV6_ROUTE_TYPE_TRAP    = 0,
    L34_IPV6_ROUTE_TYPE_DROP    = 1,
    L34_IPV6_ROUTE_TYPE_LOCAL   = 2,
    L34_IPV6_ROUTE_TYPE_GLOBAL  = 3,
    L34_IPV6_ROUTE_TYPE_END
} rtk_l34_ipv6RouteType_t;


typedef struct rtk_ipv6Routing_entry_s {
    uint8	                     valid;
    rtk_l34_ipv6RouteType_t      type;
    uint32	                     nhOrIfidIdx;  /*local: idex to interface table   global: index to ipv6 neighbor table*/
    uint32	                     ipv6PrefixLen;
    rtk_ipv6_addr_t              ipv6Addr;
    uint32                       rt2waninf;
} rtk_ipv6Routing_entry_t;




/*IPv6 Neighbor table*/
typedef struct rtk_ipv6Neighbor_entry_s {
    uint8	    valid;
    uint32      l2Idx;
    uint8	    ipv6RouteIdx;
    uint64	    ipv6Ifid;
} rtk_ipv6Neighbor_entry_t;



typedef enum rtk_l34_bindProto_e
{
    L34_BIND_PROTO_NOT_IPV4_IPV6 = 0,  /*for other ether type*/
    L34_BIND_PROTO_NOT_IPV6      = 1,  /*for IPv4 and other*/
    L34_BIND_PROTO_NOT_IPV4      = 2,  /*for IPv6 and other*/
    L34_BIND_PROTO_ALL           = 3,  /*for IPv4, IPv6 and other*/
    L34_BIND_PROTO_TYPE_END
} rtk_l34_bindProto_t;

/*binding table*/
typedef struct rtk_binding_entry_s {
    uint32	        wanTypeIdx;
    uint32	        vidLan;   /*VID=0: Port based binding  Others: Port-and-VLAN based binding*/
    rtk_portmask_t  portMask;
    rtk_portmask_t  extPortMask;
    rtk_l34_bindProto_t  bindProto;
} rtk_binding_entry_t;




/*WAN type table*/
typedef enum rtk_l34_wanType_e
{
    L34_WAN_TYPE_L2_BRIDGE         = 0,
    L34_WAN_TYPE_L3_ROUTE          = 1,
    L34_WAN_TYPE_L34NAT_ROUTE      = 2,
    L34_WAN_TYPE_L34_CUSTOMIZED    = 3,
    L34_WAN_TYPE_END
} rtk_l34_wanType_t;

typedef struct rtk_wanType_entry_s {
    uint32       	    nhIdx;
    rtk_l34_wanType_t	wanType;
} rtk_wanType_entry_t;

typedef struct rtk_l34_ipmcTrans_entry_s
{
    uint32  netifIdx;
    uint32  sipTransEnable;
    uint32  extipIdx;
    uint32  isPppoeIf;
    uint32  pppoeIdx;
}rtk_l34_ipmcTrans_entry_t;


/*
 * Function Declaration
 */
/*L34 init function*/
extern int32
rtk_l34_init(void);

/*NETIF table access*/
extern int32
rtk_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *entry);

extern int32
rtk_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *entry);

/*ARP table access*/
extern int32
rtk_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *entry);

extern int32
rtk_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *entry);

extern int32
rtk_l34_arpTable_del(uint32 idx);

/*PPPoE tbale access*/
int32
rtk_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *entry);

int32
rtk_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *entry);


/*L3 Routing table access*/
extern int32
rtk_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *entry);

extern int32
rtk_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *entry);

extern int32
rtk_l34_routingTable_del(uint32 idx);

/*NEXT Hop Table access*/
extern int32
rtk_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *entry);

extern int32
rtk_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *entry);

/*External IP Table access*/
extern int32
rtk_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *entry);

extern int32
rtk_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *entry);

extern int32
rtk_l34_extIntIPTable_del(uint32 idx);


/*NAPTR Inbound table access*/
extern uint32
rtk_l34_naptRemHash_get(uint32 sip, uint32 sport);


extern uint32
rtk_l34_naptInboundHashidx_get(uint32 dip, uint16 dport, uint16 isTCP);

extern int32
rtk_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *entry);

extern int32
rtk_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *entry);

/*NAPTR Outbound table access*/
extern uint32
rtk_l34_naptOutboundHashidx_get(int8 isTCP, uint32 sip, uint16 sport, uint32 dip, uint16 dport);

extern int32
rtk_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *entry);

extern int32
rtk_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry);



extern int32
rtk_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry);

extern int32
rtk_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry);

/*Table reset*/
extern int32
rtk_l34_table_reset(rtk_l34_table_type_t type);

#endif /* __RTK_L34_H__ */
