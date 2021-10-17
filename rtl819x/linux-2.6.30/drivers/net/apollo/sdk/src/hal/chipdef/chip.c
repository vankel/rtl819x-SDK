/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 * Purpose : chip symbol and data type definition in the SDK.
 *
 * Feature : chip symbol and data type definition
 *
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <osal/lib.h>
#include <ioal/mem32.h>
#include <hal/chipdef/chip.h>
#include <hal/common/halctrl.h>


#include <hal/chipdef/apollo/apollo_def.h>
#include <hal/chipdef/apollomp/apollomp_def.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
#if defined(CONFIG_SDK_APOLLO)
static rt_portinfo_t apollo_port_info =
{
    /* Normal APOLLO Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT    /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT     /*P4 */,
        RT_GE_PORT    /*P5 */, RT_CPU_PORT    /*P6 */, RT_PORT_NONE  /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_PORT_NONE  /*P16*/, RT_PORT_NONE /*P17*/, RT_PORT_NONE  /*P18*/, RT_PORT_NONE   /*P19*/,
        RT_PORT_NONE  /*P20*/, RT_PORT_NONE  /*P21*/, RT_PORT_NONE /*P22*/, RT_PORT_NONE  /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
     	16, /*port number*/
     	15,	/*max*/
     	0,  /*min*/
     	{{0xFFFF}}/*port mask*/
     },
     /*extension port member*/
     {
     	6, /*port number*/
     	5,	/*max*/
     	0,  /*min*/
     	{{0x3F}}/*port mask*/
     },
     .ponPort   = 3,
     .rgmiiPort = 2
}; /* end of apollo_port_info */

/*
Apollo port mapping

UTP0               0
UTP1               1
RGMII              2
PON/fiber/UTP4     3
UTP2               4
UTP3               5
CPU port           6

*/



/* Normal apollo Chip Port Information */
static rt_register_capacity_t apollo_capacityInfo =
{
    .max_num_of_mirror                  = APOLLO_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = APOLLO_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = APOLLO_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = APOLLO_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = APOLLO_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = APOLLO_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = APOLLO_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = APOLLO_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = APOLLO_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = APOLLO_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = APOLLO_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = APOLLO_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = APOLLO_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = APOLLO_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = APOLLO_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = APOLLO_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = APOLLO_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = APOLLO_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = APOLLO_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = APOLLO_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = APOLLO_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = APOLLO_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = APOLLO_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = APOLLO_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = APOLLO_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = APOLLO_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = APOLLO_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = APOLLO_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = APOLLO_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = APOLLO_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = APOLLO_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = APOLLO_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = APOLLO_PRI_SEL_GROUP_INDEX_MAX         ,
    .queue_weight_max                   = APOLLO_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = APOLLO_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = APOLLO_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = APOLLO_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = APOLLO_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = APOLLO_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = APOLLO_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = APOLLO_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = APOLLO_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = APOLLO_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = APOLLO_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = APOLLO_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = APOLLO_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = APOLLO_WRED_MPD_MAX                    ,
    .acl_rate_max                       = APOLLO_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = APOLLO_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = APOLLO_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = APOLLO_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = APOLLO_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = APOLLO_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = APOLLO_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = APOLLO_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = APOLLO_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = APOLLO_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = APOLLO_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = APOLLO_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = APOLLO_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = APOLLO_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = APOLLO_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = APOLLO_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = APOLLO_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = APOLLO_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = APOLLO_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = APOLLO_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = APOLLO_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = APOLLO_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = APOLLO_L34_PPPOE_TABLE_MAX             ,
    .gpon_tcont_max                     = APOLLO_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = APOLLO_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = APOLLO_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = APOLLO_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = APOLLO_CLASSIFY_L4PORT_RANG_NUM        ,
    .classify_ip_range_mum              = APOLLO_CLASSIFY_IP_RANG_NUM            ,
    .max_num_of_acl_template            = APOLLO_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = APOLLO_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = APOLLO_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = APOLLO_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = APOLLO_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = APOLLO_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = APOLLO_MAX_NUM_OF_LOG_MIB              ,
    .ext_cpu_port_id                    = APOLLO_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = APOLLO_PONMAC_TCONT_QUEUE_MAX          ,
    .ponmac_pir_cir_rate_max            = APOLLO_PONMAC_PIR_CIR_RATE_MAX         ,
    .max_mib_latch_timer                = APOLLO_MAX_MIB_LATCH_TIMER             ,
};

/* Normal APOLLO Chip PER_PORT block information */
static rt_macPpInfo_t apollo_macPpInfo =
{
    0x20000, /* lowerbound_addr */
    0x203FF, /* upperbound_addr */
    0x400,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_APOLLO) */

#if defined(CONFIG_SDK_APOLLOMP)
static rt_portinfo_t apollomp_port_info =
{
    /* Normal APOLLO Chip Port Information */
    {
        RT_GE_PORT    /*P0 */, RT_GE_PORT    /*P1 */, RT_GE_PORT    /*P2 */, RT_GE_PORT    /*P3 */, RT_GE_PORT     /*P4 */,
        RT_GE_PORT    /*P5 */, RT_CPU_PORT    /*P6 */, RT_PORT_NONE  /*P7 */, RT_PORT_NONE  /*P8 */, RT_PORT_NONE   /*P9 */,
        RT_PORT_NONE  /*P10*/, RT_PORT_NONE  /*P11*/, RT_PORT_NONE /*P12*/, RT_PORT_NONE  /*P13*/, RT_PORT_NONE   /*P14*/,
        RT_PORT_NONE  /*P15*/, RT_PORT_NONE  /*P16*/, RT_PORT_NONE /*P17*/, RT_PORT_NONE  /*P18*/, RT_PORT_NONE   /*P19*/,
        RT_PORT_NONE  /*P20*/, RT_PORT_NONE  /*P21*/, RT_PORT_NONE /*P22*/, RT_PORT_NONE  /*P23*/, RT_PORT_NONE   /*P24*/,
        RT_PORT_NONE  /*P25*/, RT_PORT_NONE  /*P26*/, RT_PORT_NONE /*P27*/, RT_PORT_NONE  /*P28*/, RT_PORT_NONE   /*P29*/,
        RT_PORT_NONE  /*P30*/, RT_PORT_NONE  /*P31*/
     },
     /*DSL port member*/
     {
     	16, /*port number*/
     	15,	/*max*/
     	0,  /*min*/
     	{{0xFFFF}}/*port mask*/
     },
     /*extension port member*/
     {
     	6, /*port number*/
     	5,	/*max*/
     	0,  /*min*/
     	{{0x3F}}/*port mask*/
     },
     .ponPort   = 4,
     .rgmiiPort = 5
}; /* end of apollomp_port_info */

/*
Apollo MP port mapping

UTP0               0
UTP1               1
UTP2               2
UTP3               3
PON/fiber/UTP4     4
RGMII              5
CPU port           6

*/



/* Normal apollo MP Chip Port Information */
static rt_register_capacity_t apollomp_capacityInfo =
{
    .max_num_of_mirror                  = APOLLOMP_MAX_NUM_OF_MIRROR               ,
    .max_num_of_trunk                   = APOLLOMP_MAX_NUM_OF_TRUNK                ,
    .max_num_of_trunkMember             = APOLLOMP_MAX_NUM_OF_TRUNKMEMBER          ,
    .max_num_of_dumb_trunkMember        = APOLLOMP_MAX_NUM_OF_DUMB_TRUNKMEMBER     ,
    .max_num_of_trunkHashVal            = APOLLOMP_MAX_NUM_OF_TRUNKHASHVAL         ,
    .max_num_of_msti                    = APOLLOMP_MAX_NUM_OF_MSTI                 ,
    .max_num_of_metering                = APOLLOMP_MAX_NUM_OF_METERING             ,
    .max_num_of_field_selector          = APOLLOMP_MAX_NUM_OF_FIELD_SELECTOR       ,
    .max_num_of_range_check_srcPort     = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_SRCPORT  ,
    .max_num_of_range_check_ip          = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_IP       ,
    .max_num_of_range_check_vid         = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_VID      ,
    .max_num_of_range_check_pktLen      = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_PKTLEN   ,
    .max_num_of_range_check_l4Port      = APOLLOMP_MAX_NUM_OF_RANGE_CHECK_L4PORT   ,
    .max_num_of_pattern_match_data      = APOLLOMP_MAX_NUM_OF_PATTERN_MATCH_DATA   ,
    .pattern_match_port_max             = APOLLOMP_PATTERN_MATCH_PORT_MAX          ,
    .pattern_match_port_min             = APOLLOMP_PATTERN_MATCH_PORT_MIN          ,
    .max_num_of_l2_hashdepth            = APOLLOMP_MAX_NUM_OF_L2_HASHDEPTH         ,
    .max_num_of_queue                   = APOLLOMP_MAX_NUM_OF_QUEUE                ,
    .min_num_of_queue                   = APOLLOMP_MIN_NUM_OF_QUEUE                ,
    .max_num_of_pon_queue               = APOLLOMP_MAX_NUM_OF_PON_QUEUE            ,
    .min_num_of_pon_queue               = APOLLOMP_MIN_NUM_OF_PON_QUEUE            ,
    .max_num_of_cvlan_tpid              = APOLLOMP_MAX_NUM_OF_CVLAN_TPID           ,
    .max_num_of_svlan_tpid              = APOLLOMP_MAX_NUM_OF_SVLAN_TPID           ,
    .tpid_entry_idx_max                 = APOLLOMP_TPID_ENTRY_IDX_MAX              ,
    .tpid_entry_mask_max                = APOLLOMP_TPID_ENTRY_MASK_MAX             ,
    .protocol_vlan_idx_max              = APOLLOMP_PROTOCOL_VLAN_IDX_MAX           ,
    .max_num_vlan_mbrcfg                = APOLLOMP_MAX_NUM_VLAN_MBRCFG             ,
    .vlan_fid_max                       = APOLLOMP_VLAN_FID_MAX                    ,
    .flowctrl_thresh_max                = APOLLOMP_FLOWCTRL_THRESH_MAX             ,
    .flowctrl_pauseOn_page_packet_max   = APOLLOMP_FLOWCTRL_PAUSEON_PAGE_PACKET_MAX,
    .pri_of_selection_max               = APOLLOMP_PRI_OF_SELECTION_MAX            ,
    .pri_of_selection_min               = APOLLOMP_PRI_OF_SELECTION_MIN            ,
    .pri_sel_group_index_max            = APOLLOMP_PRI_SEL_GROUP_INDEX_MAX         ,
    .queue_weight_max                   = APOLLOMP_QUEUE_WEIGHT_MAX                ,
    .qid0_weight_max                    = APOLLOMP_QID0_WEIGHT_MAX                 ,
    .rate_of_bandwidth_max              = APOLLOMP_RATE_OF_BANDWIDTH_MAX           ,
    .thresh_of_igr_bw_flowctrl_max      = APOLLOMP_THRESH_OF_IGR_BW_FLOWCTRL_MAX   ,
    .max_num_of_fastPath_of_rate        = APOLLOMP_MAX_NUM_OF_FASTPATH_OF_RATE     ,
    .rate_of_storm_control_max          = APOLLOMP_RATE_OF_STORM_CONTROL_MAX       ,
    .burst_rate_of_storm_control_max    = APOLLOMP_BURST_RATE_OF_STORM_CONTROL_MAX ,
    .internal_priority_max              = APOLLOMP_INTERNAL_PRIORITY_MAX           ,
    .drop_precedence_max                = APOLLOMP_DROP_PRECEDENCE_MAX             ,
    .priority_remap_group_idx_max       = APOLLOMP_PRIORITY_REMAP_GROUP_IDX_MAX    ,
    .priority_remark_group_idx_max      = APOLLOMP_PRIORITY_REMARK_GROUP_IDX_MAX   ,
    .priority_to_queue_group_idx_max    = APOLLOMP_PRIORITY_TO_QUEUE_GROUP_IDX_MAX ,
    .wred_weight_max                    = APOLLOMP_WRED_WEIGHT_MAX                 ,
    .wred_mpd_max                       = APOLLOMP_WRED_MPD_MAX                    ,
    .acl_rate_max                       = APOLLOMP_ACL_RATE_MAX                    ,
    .l2_learn_limit_cnt_max             = APOLLOMP_L2_LEARN_LIMIT_CNT_MAX          ,
    .l2_learn_lut_4way_no               = APOLLOMP_L2_LEARN_4WAY_NO                ,
    .l2_aging_time_max                  = APOLLOMP_L2_AGING_TIME_MAX               ,
    .l2_entry_aging_max                 = APOLLOMP_L2_ENTRY_AGING_MAX              ,
    .eee_queue_thresh_max               = APOLLOMP_EEE_QUEUE_THRESH_MAX            ,
    .sec_minIpv6FragLen_max             = APOLLOMP_SEC_MINIPV6FRAGLEN_MAX          ,
    .sec_maxPingLen_max                 = APOLLOMP_SEC_MAXPINGLEN_MAX              ,
    .sec_smurfNetmaskLen_max            = APOLLOMP_SEC_SMURFNETMASKLEN_MAX         ,
    .sflow_rate_max                     = APOLLOMP_SFLOW_RATE_MAX                  ,
    .max_num_of_mcast_fwd               = APOLLOMP_MAX_NUM_OF_MCAST_FWD            ,
    .miim_page_id_min                   = APOLLOMP_MIIM_PAGE_ID_MIN                ,
    .miim_page_id_max                   = APOLLOMP_MIIM_PAGE_ID_MAX                ,
    .miim_reg_id_max                    = APOLLOMP_MIIM_REG_ID_MAX                 ,
    .miim_data_max                      = APOLLOMP_MIIM_DATA_MAX                   ,
    .l34_netif_table_max                = APOLLOMP_L34_NETIF_TABLE_MAX             ,
    .l34_arp_table_max                  = APOLLOMP_L34_ARP_TABLE_MAX               ,
    .l34_extip_table_max                = APOLLOMP_L34_EXTIP_TABLE_MAX             ,
    .l34_routing_table_max              = APOLLOMP_L34_ROUTING_TABLE_MAX           ,
    .l34_napt_table_max                 = APOLLOMP_L34_NAPT_TABLE_MAX              ,
    .l34_naptr_table_max                = APOLLOMP_L34_NAPTR_TABLE_MAX             ,
    .l34_nh_table_max                   = APOLLOMP_L34_NH_TABLE_MAX                ,
    .l34_pppoe_table_max                = APOLLOMP_L34_PPPOE_TABLE_MAX             ,
    .gpon_tcont_max                     = APOLLOMP_GPON_TCONT_MAX                  ,
    .gpon_flow_max                      = APOLLOMP_GPON_FLOW_MAX                   ,
    .classify_entry_max                 = APOLLOMP_CLASSIFY_ENTRY_MAX              ,
    .classify_sid_max                   = APOLLOMP_CLASSIFY_SID_MAX                ,
    .classify_l4port_range_mum          = APOLLOMP_CLASSIFY_L4PORT_RANGE_NUM       ,
    .classify_ip_range_mum              = APOLLOMP_CLASSIFY_IP_RANGE_NUM            ,
    .max_num_of_acl_template            = APOLLOMP_MAX_NUM_OF_ACL_TEMPLATE         ,
    .max_num_of_acl_rule_field          = APOLLOMP_MAX_NUM_OF_ACL_RULE_FIELD       ,
    .max_num_of_acl_action              = APOLLOMP_MAX_NUM_OF_ACL_ACTION           ,
    .max_num_of_acl_rule_entry          = APOLLOMP_MAX_NUM_OF_ACL_RULE_ENTRY       ,
    .max_special_congest_second         = APOLLOMP_MAX_SPECIAL_CONGEST_SEC         ,
    .max_enhanced_fid                   = APOLLOMP_MAX_ENHANCED_FID                ,
    .max_num_of_log_mib                 = APOLLOMP_MAX_NUM_OF_LOG_MIB              ,
    .ext_cpu_port_id                    = APOLLOMP_EXT_CPU_PORT_ID                 ,
    .ponmac_tcont_queue_max             = APOLLO_PONMAC_TCONT_QUEUE_MAX            ,
    .ponmac_pir_cir_rate_max            = APOLLO_PONMAC_PIR_CIR_RATE_MAX           ,
    .max_mib_latch_timer                = APOLLOMP_MAX_MIB_LATCH_TIMER             ,
};

/* Normal APOLLO MP Chip PER_PORT block information */
static rt_macPpInfo_t apollomp_macPpInfo =
{
    0x20000, /* lowerbound_addr */
    0x203FF, /* upperbound_addr */
    0x400,  /* interval */
};
#endif /* End of #if defined(CONFIG_SDK_APOLLO) */

/* Supported mac chip lists */
static rt_device_t supported_devices[] =
{
#if defined(CONFIG_SDK_APOLLO)
    /* RT_DEVICE_APOLLO */
    {
            APOLLO_CHIP_ID,
            CHIP_REV_ID_0,
            APOLLO_CHIP_ID,
            CHIP_REV_ID_0,
            CHIP_AFLAG_LEXRA,
            &apollo_port_info,
            &apollo_capacityInfo,
            &apollo_macPpInfo
    },
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    /* RT_DEVICE_APOLLOMP */
    {
            APOLLOMP_CHIP_ID,
            CHIP_REV_ID_A,
            APOLLOMP_CHIP_ID,
            CHIP_REV_ID_A,
            CHIP_AFLAG_LEXRA,
            &apollomp_port_info,
            &apollomp_capacityInfo,
            &apollomp_macPpInfo
    },
#endif

};


/*
 * Macro Definition
 */



/* Function Name:
 *      hal_isPpBlock_check
 * Description:
 *      Check the register is PER_PORT block or not?
 * Input:
 *      addr       - register address
 * Output:
 *      pIsPpBlock - pointer buffer of chip is PER_PORT block?
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - failed
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
hal_isPpBlock_check(uint32 addr, uint32 *pIsPpBlock)
{
    /* parameter check */
    RT_PARAM_CHK((NULL == pIsPpBlock), RT_ERR_NULL_POINTER);

#if 1
    if (addr < HAL_GET_MACPP_MIN_ADDR() || addr > HAL_GET_MACPP_MAX_ADDR())
        *pIsPpBlock = FALSE;
    else
        *pIsPpBlock = TRUE;
#else
    *pIsPpBlock = FALSE;
#endif


    return RT_ERR_OK;
} /* end of hal_isPpBlock_check */



/* Function Name:
 *      hal_find_device
 * Description:
 *      Find the mac chip from SDK supported mac device lists.
 * Input:
 *      chip_id     - chip id
 *      chip_rev_id - chip revision id
 * Output:
 *      None
 * Return:
 *      NULL        - Not found
 *      Otherwise   - Pointer of mac chip structure that found
 * Note:
 *      The function have take care the forward compatible in revision.
 *      Return one recently revision if no extra match revision.
 */
rt_device_t *
hal_find_device(uint32 chip_id, uint32 chip_rev_id)
{
    uint32  dev_idx;
    uint32  most_rev_id = 0;
    rt_device_t *pMatchDevice = NULL;


    RT_PARAM_CHK((chip_rev_id > CHIP_REV_ID_MAX), NULL);

    /* find out appropriate supported revision from supported_devices lists
     */
    for (dev_idx = 0; dev_idx < RT_DEVICE_END; dev_idx++)
    {
        if (supported_devices[dev_idx].chip_id == chip_id)
        {
            if (supported_devices[dev_idx].chip_rev_id == chip_rev_id)
            {
                /* Match and return this MAC device */
                return (&supported_devices[dev_idx]);
            }
            else if ((supported_devices[dev_idx].chip_rev_id < chip_rev_id) &&
                     (supported_devices[dev_idx].chip_rev_id >= most_rev_id))
            {
                /* Match better candidate of MAC device */
                most_rev_id = supported_devices[dev_idx].chip_rev_id;
                pMatchDevice = &supported_devices[dev_idx];
            }
        }
    }

    return (pMatchDevice);
} /* end of hal_find_device */
