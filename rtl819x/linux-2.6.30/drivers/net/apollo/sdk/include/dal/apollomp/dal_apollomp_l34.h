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
#ifndef __DAL_APOLLOMP_L34_H_
#define __DAL_APOLLOMP_L34_H_
/*
 * Include Files
 */
#include <rtk/l34.h>

/*
 * Symbol Definition
 */
#define APOLLOMP_L34_HSB_WORD 7
#define APOLLOMP_L34_HSA_WORD 4

/*
 * Data Declaration
 */

typedef struct apollomp_l34_hsb_param_s
{
    uint32 hsbWords[APOLLOMP_L34_HSB_WORD];
}apollomp_l34_hsb_param_t;

typedef struct apollomp_l34_hsa_param_s
{
    uint32 hsaWords[APOLLOMP_L34_HSA_WORD];
}apollomp_l34_hsa_param_t;


typedef enum apollomp_l34_hsab_mode_s
{
    APOLLOMP_L34_HSBA_TEST_MODE  = 0,
    APOLLOMP_L34_HSBA_NO_LOG = 1,
    APOLLOMP_L34_HSBA_LOG_ALL = 2,
    APOLLOMP_L34_HSBA_LOG_FIRST_DROP = 3,
    APOLLOMP_L34_HSBA_LOG_FIRS_PASS = 4,
    APOLLOMP_L34_HSBA_LOG_FIRS_TO_CPU = 5
}apollomp_l34_hsab_mode_t;

extern int32
dal_apollomp_l34_init(void);

/*NETIF table access*/
extern int32
dal_apollomp_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *entry);

extern int32
dal_apollomp_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *entry);

/*ARP table access*/
extern int32
dal_apollomp_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *entry);

extern int32
dal_apollomp_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *entry);

extern int32
dal_apollomp_l34_arpTable_del(uint32 idx);

/*PPPoE table access*/
extern int32
dal_apollomp_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *entry);

extern int32
dal_apollomp_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *entry);

/*L3 Routing table access*/
extern int32
dal_apollomp_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *entry);

extern int32
dal_apollomp_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *entry);

extern int32
dal_apollomp_l34_routingTable_del(uint32 idx);

/*NEXT Hop Table access*/
extern int32
dal_apollomp_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *entry);

extern int32
dal_apollomp_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *entry);

/*External IP Table access*/
extern int32
dal_apollomp_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *entry);

extern int32
dal_apollomp_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *entry);

extern int32
dal_apollomp_l34_extIntIPTable_del(uint32 idx);

/*NAPTR Inbound table access*/
extern uint32
dal_apollomp_l34_naptRemHash_get(uint32 sip, uint32 sport);

extern int32
dal_apollomp_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *entry);

extern int32
dal_apollomp_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *entry);

extern int32
dal_apollomp_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry);


extern uint32
dal_apollomp_l34_naptInboundHashidx_get(uint32 dip, uint16 dport, uint16 isTCP);

/*NAPTR Outbound table access*/
extern int32
dal_apollomp_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *entry);

extern int32
dal_apollomp_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry);

extern uint32
dal_apollomp_l34_naptOutboundHashidx_get(int8 isTCP, uint32 sip, uint16 sport, uint32 dip, uint16 dport);


/*Table reset*/
extern int32
dal_apollomp_l34_table_reset(rtk_l34_table_type_t type);


/*HSA/HSB access*/
int32
dal_apollomp_l34_hsb_set(apollomp_l34_hsb_param_t *hsb);

int32
dal_apollomp_l34_hsb_get(apollomp_l34_hsb_param_t *hsb);


int32
dal_apollomp_l34_hsa_set(apollomp_l34_hsa_param_t *hsa);

int32
dal_apollomp_l34_hsa_get(apollomp_l34_hsa_param_t *hsa);

int32
dal_apollomp_l34_hsabCtrMode_set(apollomp_l34_hsab_mode_t mode);



/*wan type table*/
extern int32
dal_apollomp_l34_wanTypeTable_get(uint32 idx,rtk_wanType_entry_t *entry);
extern int32
dal_apollomp_l34_wanTypeTable_set(uint32 idx,rtk_wanType_entry_t *entry);


/*binding table*/
extern int32
dal_apollomp_l34_bindingTable_get(uint32 idx,rtk_binding_entry_t *entry);
extern int32
dal_apollomp_l34_bindingTable_set(uint32 idx,rtk_binding_entry_t *entry);

/*IPv6 routing table*/
extern int32
dal_apollomp_l34_ipv6RoutingTable_get(uint32 idx,rtk_ipv6Routing_entry_t *entry);
extern int32
dal_apollomp_l34_ipv6RoutingTable_set(uint32 idx,rtk_ipv6Routing_entry_t *entry);


/*IPv6 neighbor table*/
extern int32
dal_apollomp_l34_ipv6NeighborTable_get(uint32 idx,rtk_ipv6Neighbor_entry_t *entry);
extern int32
dal_apollomp_l34_ipv6NeighborTable_set(uint32 idx,rtk_ipv6Neighbor_entry_t *entry);


extern int32
dal_apollomp_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry);

extern int32
dal_apollomp_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry);


#endif /*#ifndef __DAL_APOLLOMP_L34_H_*/

