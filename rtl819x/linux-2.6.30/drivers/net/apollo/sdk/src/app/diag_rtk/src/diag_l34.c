/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 16745 $
 * $Date: 2011-04-12 11:46:26 +0800 (Tue, 12 Apr 2011) $
 *
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>

/*
 * l34 reset table ( l3 | pppoe | nexthop | interface | external-ip | arp | naptr | napt )
 */
cparser_result_t
cparser_cmd_l34_reset_table_l3_pppoe_nexthop_interface_external_ip_arp_naptr_napt(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_table_l3_pppoe_nexthop_interface_external_ip_arp_naptr_napt */

/*
 * l34 get arp
 */
cparser_result_t
cparser_cmd_l34_get_arp(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp */

/*
 * l34 get arp <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_arp_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_index */

/*
 * l34 set arp <UINT:index> next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_arp_index_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_arp_index_next_hop_table_nh_index */

/*
 * l34 del arp <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_arp_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_arp_index */

/*
 * l34 get external-ip 
 */
cparser_result_t
cparser_cmd_l34_get_external_ip(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_external_ip */

/*
 * l34 get external-ip <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_external_ip_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_external_ip_index */

/*
 * l34 set external-ip <UINT:index> type ( nat | napt | lp )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_type_nat_napt_lp(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_type_nat_napt_lp */

/*
 * l34 set external-ip <UINT:index> external-ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_external_ip_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_external_ip_ip */

/*
 * l34 set external-ip <UINT:index> internal-ip <IPV4ADDR:ip>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_internal_ip_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_internal_ip_ip */

/*
 * l34 set external-ip <UINT:index> next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_next_hop_table_nh_index */

/*
 * l34 set external-ip <UINT:index> nat-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_nat_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_nat_priority_state_disable_enable */

/*
 * l34 set external-ip <UINT:index> nat-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_nat_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_nat_priority_priority */

/*
 * l34 set external-ip <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_external_ip_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_external_ip_index_state_disable_enable */

/*
 * l34 del external-ip <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_external_ip_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_external_ip_index */

/*
 * l34 get routing 
 */
cparser_result_t
cparser_cmd_l34_get_routing(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_routing */

/*
 * l34 get routing <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_routing_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_routing_index */

/*
 * l34 set routing <UINT:index> ip <IPV4ADDR:ip> ip-mask <UINT:mask>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_ip_ip_ip_mask_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr,
    uint32_t  *mask_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_ip_ip_ip_mask_mask */

/*
 * l34 set routing <UINT:index> interface-type ( internal | external )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_interface_type_internal_external(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_interface_type_internal_external */

/*
 * l34 set routing <UINT:index> type ( drop | trap )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_drop_trap(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_drop_trap */

/*
 * l34 set routing <UINT:index> type local-route destination-netif <UINT:netif_index>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_local_route_destination_netif_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *netif_index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_local_route_destination_netif_netif_index */

/*
 * l34 set routing <UINT:index> type local-route arp-start-address <UINT:start_addr> arp-end-address <UINT:end_addr>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_local_route_arp_start_address_start_addr_arp_end_address_end_addr(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *start_addr_ptr,
    uint32_t  *end_addr_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_local_route_arp_start_address_start_addr_arp_end_address_end_addr */

/*
 * l34 set routing <UINT:index> type global-route next-hop-table <UINT:nh_index>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_table_nh_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *nh_index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_table_nh_index */

/*
 * l34 set routing <UINT:index> type global-route next-hop-start <UINT:address> next-hop-number <UINT:nh_number>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_start_address_next_hop_number_nh_number(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *address_ptr,
    uint32_t  *nh_number_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_start_address_next_hop_number_nh_number */

/*
 * l34 set routing <UINT:index> type global-route next-hop-algo ( per-packet | per-session | per-source_ip )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_next_hop_algo_per_packet_per_session_per_source_ip(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_next_hop_algo_per_packet_per_session_per_source_ip */

/*
 * l34 set routing <UINT:index> type global-route ip-domain-range <UINT:range>
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_type_global_route_ip_domain_range_range(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *range_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_type_global_route_ip_domain_range_range */

/*
 * l34 set routing <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_routing_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_routing_index_state_disable_enable */

/*
 * l34 del routing <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_routing_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_routing_index */

/*
 * l34 get netif 
 */
cparser_result_t
cparser_cmd_l34_get_netif(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_netif */

/*
 * l34 get netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_netif_index */

/*
 * l34 set netif <UINT:index> gateway-mac <MACADDR:mac> mac-mask ( no-mask | 1bit-mask | 2bit-mask | 3bit-mask )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_gateway_mac_mac_mac_mask_no_mask_1bit_mask_2bit_mask_3bit_mask(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_gateway_mac_mac_mac_mask_no_mask_1bit_mask_2bit_mask_3bit_mask */

/*
 * l34 set netif <UINT:index> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_vid_vid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *vid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_vid_vid */

/*
 * l34 set netif <UINT:index> mtu <UINT:mtu>
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_mtu_mtu(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *mtu_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_mtu_mtu */

/*
 * l34 set netif <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_state_disable_enable */

/*
 * l34 set netif <UINT:index> l3-route state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_netif_index_l3_route_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_netif_index_l3_route_state_enable_disable */

/*
 * l34 del netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_netif_index */

/*
 * l34 get nexthop 
 */
cparser_result_t
cparser_cmd_l34_get_nexthop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_nexthop */

/*
 * l34 get nexthop <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_nexthop_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_nexthop_index */

/*
 * l34 set nexthop <UINT:index> netif <UINT:netif_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_netif_netif_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *netif_index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_netif_netif_index */

/*
 * l34 set nexthop <UINT:index> l2 <UINT:l2_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_l2_l2_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *l2_index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_l2_l2_index */

/*
 * l34 set nexthop <UINT:index> type ( ethernet | pppoe )
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_type_ethernet_pppoe(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_type_ethernet_pppoe */

/*
 * l34 set nexthop <UINT:index> pppoe <UINT:pppoe_index>
 */
cparser_result_t
cparser_cmd_l34_set_nexthop_index_pppoe_pppoe_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *pppoe_index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nexthop_index_pppoe_pppoe_index */

/*
 * l34 get pppoe 
 */
cparser_result_t
cparser_cmd_l34_get_pppoe(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe */

/*
 * l34 get pppoe <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_pppoe_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe_index */

/*
 * l34 set pppoe <UINT:index> session-id <UINT:session_id>
 */
cparser_result_t
cparser_cmd_l34_set_pppoe_index_session_id_session_id(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *session_id_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_pppoe_index_session_id_session_id */

/*
 * l34 get napt 
 */
cparser_result_t
cparser_cmd_l34_get_napt(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_napt */

/*
 * l34 get napt <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_napt_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_napt_index */

/*
 * l34 set napt <UINT:index> hash-index <UINT:hash_index>
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_hash_index_hash_index(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *hash_index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_napt_index_hash_index_hash_index */

/*
 * l34 set napt <UINT:index> napt-priority state ( disable | enable )  
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_napt_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_napt_index_napt_priority_state_disable_enable */

/*
 * l34 set napt <UINT:index> napt-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_napt_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_napt_index_napt_priority_priority */

/*
 * l34 set napt <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l34_set_napt_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_napt_index_state_disable_enable */

/*
 * l34 del napt <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_napt_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_napt_index */

/*
 * l34 get naptr 
 */
cparser_result_t
cparser_cmd_l34_get_naptr(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_naptr */

/*
 * l34 get naptr <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_naptr_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_naptr_index */

/*
 * l34 set naptr <UINT:index> internal-ip <IPV4ADDR:ip> internal-port <UINT:port> 
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_internal_ip_ip_internal_port_port(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *ip_ptr,
    uint32_t  *port_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_internal_ip_ip_internal_port_port */

/*
 * l34 set naptr <UINT:index> protocol ( tcp | udp )
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_protocol_tcp_udp(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_protocol_tcp_udp */

/*
 * l34 set naptr <UINT:index> external-ip <UINT:extip_index> external-port-lsb <UINT:export_lsb>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_external_ip_extip_index_external_port_lsb_export_lsb(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *extip_index_ptr,
    uint32_t  *export_lsb_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_external_ip_extip_index_external_port_lsb_export_lsb */

/*
 * l34 set naptr <UINT:index> naptr-priority state ( disable | enable )  
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_naptr_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_naptr_priority_state_disable_enable */

/*
 * l34 set naptr <UINT:index> naptr-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_naptr_priority_priority(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *priority_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_naptr_priority_priority */

/*
 * l34 set naptr <UINT:index> remote-hash-type ( none | remote_ip | remote_ip_remote_port )
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_remote_hash_type_none_remote_ip_remote_ip_remote_port(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_remote_hash_type_none_remote_ip_remote_ip_remote_port */

/*
 * l34 set naptr <UINT:index> hash-value <UINT:value>
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_hash_value_value(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *value_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_hash_value_value */

/*
 * l34 set naptr <UINT:index> state disable
 */
cparser_result_t
cparser_cmd_l34_set_naptr_index_state_disable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_naptr_index_state_disable */

/*
 * l34 del naptr <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_del_naptr_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_del_naptr_index */

/*
 * l34 set port ( <PORT_LIST:ports> | all ) netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_set_port_ports_all_netif_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_port_ports_all_netif_index */

/*
 * l34 get port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l34_get_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_port_ports_all */

/*
 * l34 set ext ( <PORT_LIST:ext> | all ) netif <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_set_ext_ext_all_netif_index(
    cparser_context_t *context,
    char * *ext_ptr,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_ext_ext_all_netif_index */

/*
 * l34 get ext ( <PORT_LIST:ext> | all )
 */
cparser_result_t
cparser_cmd_l34_get_ext_ext_all(
    cparser_context_t *context,
    char * *ext_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_ext_ext_all */

/*
 * l34 set l4-fragment action ( trap-to-cpu | forward )
 */
cparser_result_t
cparser_cmd_l34_set_l4_fragment_action_trap_to_cpu_forward(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l4_fragment_action_trap_to_cpu_forward */

/*
 * l34 get l4-fragment
 */
cparser_result_t
cparser_cmd_l34_get_l4_fragment(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_fragment */

/*
 * l34 set l3-checksum-error action ( forward | drop )
 */
cparser_result_t
cparser_cmd_l34_set_l3_checksum_error_action_forward_drop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l3_checksum_error_action_forward_drop */

/*
 * l34 get l3-checksum-error
 */
cparser_result_t
cparser_cmd_l34_get_l3_checksum_error(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l3_checksum_error */

/*
 * l34 set l4-checksum-error action ( forward | drop )
 */
cparser_result_t
cparser_cmd_l34_set_l4_checksum_error_action_forward_drop(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_l4_checksum_error_action_forward_drop */

/*
 * l34 get l4-checksum-error
 */
cparser_result_t
cparser_cmd_l34_get_l4_checksum_error(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_checksum_error */

/*
 * l34 set ttl-minus state ( enable | disable )
 */
cparser_result_t
cparser_cmd_l34_set_ttl_minus_state_enable_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_ttl_minus_state_enable_disable */

/*
 * l34 get ttl-minus state
 */
cparser_result_t
cparser_cmd_l34_get_ttl_minus_state(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_ttl_minus_state */

/*
 * l34 set interface-decision-mode ( vlan-based | port-based | mac-based )
 */
cparser_result_t
cparser_cmd_l34_set_interface_decision_mode_vlan_based_port_based_mac_based(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_interface_decision_mode_vlan_based_port_based_mac_based */

/*
 * l34 get interface-decision-mode
 */
cparser_result_t
cparser_cmd_l34_get_interface_decision_mode(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_interface_decision_mode */

/*
 * l34 set nat-attack action ( drop | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l34_set_nat_attack_action_drop_trap_to_cpu(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_nat_attack_action_drop_trap_to_cpu */

/*
 * l34 get nat-attack
 */
cparser_result_t
cparser_cmd_l34_get_nat_attack(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_nat_attack */

/*
 * l34 set wan-route action ( drop | trap-to-cpu | forward )
 */
cparser_result_t
cparser_cmd_l34_set_wan_route_action_drop_trap_to_cpu_forward(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_wan_route_action_drop_trap_to_cpu_forward */

/*
 * l34 get wan-route
 */
cparser_result_t
cparser_cmd_l34_get_wan_route(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_wan_route */

/*
 * l34 set route-mode ( l3 | l3-l4 | disable )
 */
cparser_result_t
cparser_cmd_l34_set_route_mode_l3_l3_l4_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_set_route_mode_l3_l3_l4_disable */

/*
 * l34 get route-mode
 */
cparser_result_t
cparser_cmd_l34_get_route_mode(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_route_mode */

/*
 * l34 get pppoe-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_pppoe_traffic_indicator(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_pppoe_traffic_indicator */

/*
 * l34 get arp-traffic-indicator index <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_arp_traffic_indicator_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_traffic_indicator_index_index */

/*
 * l34 get arp-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_arp_traffic_indicator(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_arp_traffic_indicator */

/*
 * l34 reset arp-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_reset_arp_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_arp_traffic_indicator_table0_table1 */

/*
 * l34 select arp-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_select_arp_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_select_arp_traffic_indicator_table0_table1 */

/*
 * l34 get l4-traffic-indicator index <UINT:index>
 */
cparser_result_t
cparser_cmd_l34_get_l4_traffic_indicator_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_traffic_indicator_index_index */

/*
 * l34 get l4-traffic-indicator
 */
cparser_result_t
cparser_cmd_l34_get_l4_traffic_indicator(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_get_l4_traffic_indicator */

/*
 * l34 reset l4-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_reset_l4_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_reset_l4_traffic_indicator_table0_table1 */

/*
 * l34 select l4-traffic-indicator ( table0 | table1 )
 */
cparser_result_t
cparser_cmd_l34_select_l4_traffic_indicator_table0_table1(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_l34_select_l4_traffic_indicator_table0_table1 */

/*
 * debug l34 set hsb l2-bridge <UINT:l2bridge> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l2_bridge_l2bridge(
    cparser_context_t *context,
    uint32_t  *l2bridge_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l2_bridge_l2bridge */

/*
 * debug l34 set hsb ip-fragments <UINT:is_fragments> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ip_fragments_is_fragments(
    cparser_context_t *context,
    uint32_t  *is_fragments_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ip_fragments_is_fragments */

/*
 * debug l34 set hsb ip-more-fragments <UINT:is_more> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ip_more_fragments_is_more(
    cparser_context_t *context,
    uint32_t  *is_more_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ip_more_fragments_is_more */

/*
 * debug l34 set hsb l4-checksum-ok <UINT:is_ok> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l4_checksum_ok_is_ok(
    cparser_context_t *context,
    uint32_t  *is_ok_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l4_checksum_ok_is_ok */

/*
 * debug l34 set hsb l3-checksum-ok <UINT:is_ok> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_l3_checksum_ok_is_ok(
    cparser_context_t *context,
    uint32_t  *is_ok_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_l3_checksum_ok_is_ok */

/*
 * debug l34 set hsb direct-tx <UINT:is_direct_tx> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_direct_tx_is_direct_tx(
    cparser_context_t *context,
    uint32_t  *is_direct_tx_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_direct_tx_is_direct_tx */

/*
 * debug l34 set hsb udp-no-chksum <UINT:udp_no_chk> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_udp_no_chksum_udp_no_chk(
    cparser_context_t *context,
    uint32_t  *udp_no_chk_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_udp_no_chksum_udp_no_chk */

/*
 * debug l34 set hsb parse-fail <UINT:parse_fail> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_parse_fail_parse_fail(
    cparser_context_t *context,
    uint32_t  *parse_fail_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_parse_fail_parse_fail */

/*
 * debug l34 set hsb pppoe-if <UINT:pppoe_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pppoe_if_pppoe_if(
    cparser_context_t *context,
    uint32_t  *pppoe_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pppoe_if_pppoe_if */

/*
 * debug l34 set hsb svlan-if <UINT:svlan_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_svlan_if_svlan_if(
    cparser_context_t *context,
    uint32_t  *svlan_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_svlan_if_svlan_if */

/*
 * debug l34 set hsb ttls <UINT:ttls> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_ttls_ttls(
    cparser_context_t *context,
    uint32_t  *ttls_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_ttls_ttls */

/*
 * debug l34 set hsb pkt-type <UINT:pkt_type> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pkt_type_pkt_type(
    cparser_context_t *context,
    uint32_t  *pkt_type_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pkt_type_pkt_type */

/*
 * debug l34 set hsb tcp-flag <UINT:tcp_flag> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_tcp_flag_tcp_flag(
    cparser_context_t *context,
    uint32_t  *tcp_flag_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_tcp_flag_tcp_flag */

/*
 * debug l34 set hsb cvlan-if <UINT:cvlan_if> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_cvlan_if_cvlan_if(
    cparser_context_t *context,
    uint32_t  *cvlan_if_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_cvlan_if_cvlan_if */

/*
 * debug l34 set hsb source-port <UINT:spa> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_source_port_spa(
    cparser_context_t *context,
    uint32_t  *spa_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_source_port_spa */

/*
 * debug l34 set hsb cvid <UINT:cvid> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_cvid_cvid(
    cparser_context_t *context,
    uint32_t  *cvid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_cvid_cvid */

/*
 * debug l34 set hsb packet-length <UINT:length> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_packet_length_length(
    cparser_context_t *context,
    uint32_t  *length_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_packet_length_length */

/*
 * debug l34 set hsb dport <UINT:dport> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dport_dport(
    cparser_context_t *context,
    uint32_t  *dport_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dport_dport */

/*
 * debug l34 set hsb pppoe-id <UINT:pppoe> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_pppoe_id_pppoe(
    cparser_context_t *context,
    uint32_t  *pppoe_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_pppoe_id_pppoe */

/*
 * debug l34 set hsb dip <IPV4ADDR:ip> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dip_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dip_ip */

/*
 * debug l34 set hsb sip <IPV4ADDR:ip> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_sip_ip(
    cparser_context_t *context,
    uint32_t  *ip_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_sip_ip */

/*
 * debug l34 set hsb sport <UINT:sport> 
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_sport_sport(
    cparser_context_t *context,
    uint32_t  *sport_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_sport_sport */

/*
 * debug l34 set hsb dmac <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsb_dmac_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsb_dmac_mac */

/*
 * debug l34 get hsb
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsb(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsb */

/*
 * debug l34 get hsa
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsa(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsa */

/*
 * debug l34 set hsba log-mode <UINT:mode>
 */
cparser_result_t
cparser_cmd_debug_l34_set_hsba_log_mode_mode(
    cparser_context_t *context,
    uint32_t  *mode_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_set_hsba_log_mode_mode */

/*
 * debug l34 get hsba log-mode 
 */
cparser_result_t
cparser_cmd_debug_l34_get_hsba_log_mode(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_l34_get_hsba_log_mode */

