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
 * Purpose : Mapper Layer is used to seperate different kind of software or hardware platform
 *
 * Feature : Just dispatch information to Multiplex layer
 *
 */
#ifndef __DAL_MAPPER_H__
#define __DAL_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <rtk/switch.h>
#include <rtk/l34.h>
#include <rtk/ponmac.h>
#include <rtk/gpon.h>
#include <rtk/l2.h>
#include <rtk/stp.h>
#include <rtk/classify.h>
#include <rtk/stat.h>
#include <rtk/qos.h>
#include <rtk/svlan.h>
#include <rtk/acl.h>
#include <rtk/trap.h>
#include <rtk/rate.h>
#include <rtk/sec.h>
#include <rtk/led.h>
#include <rtk/dot1x.h>
#include <rtk/oam.h>
#include <rtk/trunk.h>
#include <rtk/intr.h>
#include <rtk/rldp.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

typedef struct dal_mapper_s {
    uint32  family_id;
    int32   (*_init)(void);
    /* switch init*/
    int32   (*switch_init)(void);
    int32   (*switch_phyPortId_get)(rtk_switch_port_name_t, int32 *);
    int32   (*switch_logicalPort_get)(int32, rtk_switch_port_name_t *);
    int32   (*switch_port2PortMask_set)(rtk_portmask_t *, rtk_switch_port_name_t);
    int32   (*switch_port2PortMask_clear)(rtk_portmask_t *, rtk_switch_port_name_t);
    int32   (*switch_portIdInMask_check)(rtk_portmask_t *, rtk_switch_port_name_t);
    int32   (*switch_maxPktLenLinkSpeed_get)(rtk_switch_maxPktLen_linkSpeed_t, uint32 *);
    int32   (*switch_maxPktLenLinkSpeed_set)(rtk_switch_maxPktLen_linkSpeed_t, uint32);
    int32   (*switch_mgmtMacAddr_get)(rtk_mac_t *);
    int32   (*switch_mgmtMacAddr_set)(rtk_mac_t *);

    /* VLAN */
    int32   (*vlan_init)(void);
    int32   (*vlan_create)(rtk_vlan_t);
    int32   (*vlan_destroy)(rtk_vlan_t);
    int32   (*vlan_destroyAll)(uint32);
    int32   (*vlan_fid_get)(rtk_vlan_t, rtk_fid_t *);
    int32   (*vlan_fid_set)(rtk_vlan_t, rtk_fid_t);
    int32   (*vlan_fidMode_get)(rtk_vlan_t, rtk_fidMode_t *);
    int32   (*vlan_fidMode_set)(rtk_vlan_t, rtk_fidMode_t);
    int32   (*vlan_port_get)(rtk_vlan_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*vlan_port_set)(rtk_vlan_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*vlan_extPort_get)(rtk_vlan_t, rtk_portmask_t *);
    int32   (*vlan_extPort_set)(rtk_vlan_t, rtk_portmask_t *);
    int32   (*vlan_stg_get)(rtk_vlan_t, rtk_stg_t *);
    int32   (*vlan_stg_set)(rtk_vlan_t, rtk_stg_t);
    int32   (*vlan_priority_get)(rtk_vlan_t, rtk_pri_t *);
    int32   (*vlan_priority_set)(rtk_vlan_t, rtk_pri_t);
    int32   (*vlan_priorityEnable_get)(rtk_vlan_t, rtk_enable_t *);
    int32   (*vlan_priorityEnable_set)(rtk_vlan_t, rtk_enable_t);
    int32   (*vlan_portAcceptFrameType_get)(rtk_port_t, rtk_vlan_acceptFrameType_t *);
    int32   (*vlan_portAcceptFrameType_set)(rtk_port_t, rtk_vlan_acceptFrameType_t);
    int32   (*vlan_vlanFunctionEnable_get)(rtk_enable_t *);
    int32   (*vlan_vlanFunctionEnable_set)(rtk_enable_t);
    int32   (*vlan_portIgrFilterEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*vlan_portIgrFilterEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*vlan_leaky_get)(rtk_leaky_type_t, rtk_enable_t *);
    int32   (*vlan_leaky_set)(rtk_leaky_type_t, rtk_enable_t);
    int32   (*vlan_portPvid_get)(rtk_port_t, uint32 *);
    int32   (*vlan_portPvid_set)(rtk_port_t, uint32);
    int32   (*vlan_extPortPvid_get)(uint32, uint32 *);
    int32   (*vlan_extPortPvid_set)(uint32, uint32);
    int32   (*vlan_protoGroup_get)(uint32, rtk_vlan_protoGroup_t *);
    int32   (*vlan_protoGroup_set)(uint32, rtk_vlan_protoGroup_t *);
    int32   (*vlan_portProtoVlan_get)(rtk_port_t, uint32, rtk_vlan_protoVlanCfg_t *);
    int32   (*vlan_portProtoVlan_set)(rtk_port_t, uint32, rtk_vlan_protoVlanCfg_t *);
    int32   (*vlan_tagMode_get)(rtk_port_t, rtk_vlan_tagMode_t *);
    int32   (*vlan_tagMode_set)(rtk_port_t, rtk_vlan_tagMode_t);
    int32   (*vlan_portFid_get)(rtk_port_t, rtk_enable_t *, rtk_fid_t *);
    int32   (*vlan_portFid_set)(rtk_port_t, rtk_enable_t, rtk_fid_t);
    int32   (*vlan_portPriority_get)(rtk_port_t, rtk_pri_t *);
    int32   (*vlan_portPriority_set)(rtk_port_t, rtk_pri_t);
    int32   (*vlan_portEgrTagKeepType_get)(rtk_port_t, rtk_portmask_t *, rtk_vlan_tagKeepType_t *);
    int32   (*vlan_portEgrTagKeepType_set)(rtk_port_t, rtk_portmask_t *, rtk_vlan_tagKeepType_t);
    int32   (*vlan_transparentEnable_get)(rtk_enable_t *);
    int32   (*vlan_transparentEnable_set)(rtk_enable_t);
    int32   (*vlan_cfiKeepEnable_get)(rtk_enable_t *);
    int32   (*vlan_cfiKeepEnable_set)(rtk_enable_t);
    int32   (*vlan_reservedVidAction_get)(rtk_vlan_resVidAction_t *, rtk_vlan_resVidAction_t *);
    int32   (*vlan_reservedVidAction_set)(rtk_vlan_resVidAction_t, rtk_vlan_resVidAction_t);

    /* Trap*/
    int32   (*trap_init)(void);
    int32   (*trap_reasonTrapToCpuPriority_get)(rtk_trap_reason_type_t, rtk_pri_t *);
    int32   (*trap_reasonTrapToCpuPriority_set)(rtk_trap_reason_type_t, rtk_pri_t);
    int32   (*trap_igmpCtrlPkt2CpuEnable_get)(rtk_enable_t *);
    int32   (*trap_igmpCtrlPkt2CpuEnable_set)(rtk_enable_t );
    int32   (*trap_mldCtrlPkt2CpuEnable_get)(rtk_enable_t *);
    int32   (*trap_mldCtrlPkt2CpuEnable_set)(rtk_enable_t );
    int32   (*trap_l2McastPkt2CpuEnable_get)(rtk_enable_t *);
    int32   (*trap_l2McastPkt2CpuEnable_set)(rtk_enable_t );
    int32   (*trap_ipMcastPkt2CpuEnable_get)(rtk_enable_t *);
    int32   (*trap_ipMcastPkt2CpuEnable_set)(rtk_enable_t );
    int32   (*trap_rmaAction_get)(rtk_mac_t *, rtk_trap_rma_action_t *);
    int32   (*trap_rmaAction_set)(rtk_mac_t *, rtk_trap_rma_action_t);
    int32   (*trap_rmaPri_get)(rtk_pri_t *);
    int32   (*trap_rmaPri_set)(rtk_pri_t);
    int32   (*trap_rmaVlanCheckEnable_get)(rtk_mac_t *, rtk_enable_t *);
    int32   (*trap_rmaVlanCheckEnable_set)(rtk_mac_t *, rtk_enable_t);
    int32   (*trap_rmaPortIsolationEnable_get)(rtk_mac_t *, rtk_enable_t *);
    int32   (*trap_rmaPortIsolationEnable_set)(rtk_mac_t *, rtk_enable_t);
    int32   (*trap_rmaStormControlEnable_get)(rtk_mac_t *, rtk_enable_t *);
    int32   (*trap_rmaStormControlEnable_set)(rtk_mac_t *, rtk_enable_t);
    int32   (*trap_rmaKeepCtagEnable_get)(rtk_mac_t *, rtk_enable_t *);
    int32   (*trap_rmaKeepCtagEnable_set)(rtk_mac_t *, rtk_enable_t);
    int32   (*trap_oamPduAction_get)(rtk_action_t *);
    int32   (*trap_oamPduAction_set)(rtk_action_t);
    int32   (*trap_oamPduPri_get)(rtk_pri_t *);
    int32   (*trap_oamPduPri_set)(rtk_pri_t);

    /* L2 */
    int32   (*l2_init)(void);
    int32   (*l2_flushLinkDownPortAddrEnable_get)(rtk_enable_t *);
    int32   (*l2_flushLinkDownPortAddrEnable_set)(rtk_enable_t);
    int32   (*l2_ucastAddr_flush)(rtk_l2_flushCfg_t *);
    int32   (*l2_limitLearningOverStatus_get)(uint32 *);
    int32   (*l2_limitLearningOverStatus_clear)(void);
    int32   (*l2_learningCnt_get)(uint32 *);
    int32   (*l2_limitLearningCnt_get)(uint32 *);
    int32   (*l2_limitLearningCnt_set)(uint32 );
    int32   (*l2_limitLearningCntAction_get)(rtk_l2_limitLearnCntAction_t *);
    int32   (*l2_limitLearningCntAction_set)(rtk_l2_limitLearnCntAction_t );
    int32   (*l2_portLimitLearningOverStatus_get)(rtk_port_t, uint32 *);
    int32   (*l2_portLimitLearningOverStatus_clear)(rtk_port_t);
    int32   (*l2_portLearningCnt_get)(rtk_port_t, uint32 *);
    int32   (*l2_portLimitLearningCnt_get)(rtk_port_t, uint32 *);
    int32   (*l2_portLimitLearningCnt_set)(rtk_port_t, uint32);
    int32   (*l2_portLimitLearningCntAction_get)(rtk_port_t, rtk_l2_limitLearnCntAction_t *);
    int32   (*l2_portLimitLearningCntAction_set)(rtk_port_t, rtk_l2_limitLearnCntAction_t);
    int32   (*l2_aging_get)(uint32 *);
    int32   (*l2_aging_set)(uint32);
    int32   (*l2_portAgingEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*l2_portAgingEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*l2_lookupMissAction_get)(rtk_l2_lookupMissType_t, rtk_action_t *);
    int32   (*l2_lookupMissAction_set)(rtk_l2_lookupMissType_t, rtk_action_t);
    int32   (*l2_portLookupMissAction_get)(rtk_port_t , rtk_l2_lookupMissType_t , rtk_action_t *);
    int32   (*l2_portLookupMissAction_set)(rtk_port_t , rtk_l2_lookupMissType_t , rtk_action_t );
    int32   (*l2_lookupMissFloodPortMask_get)(rtk_l2_lookupMissType_t, rtk_portmask_t *);
    int32   (*l2_lookupMissFloodPortMask_set)(rtk_l2_lookupMissType_t, rtk_portmask_t *);
    int32   (*l2_lookupMissFloodPortMask_add)(rtk_l2_lookupMissType_t, rtk_port_t);
    int32   (*l2_lookupMissFloodPortMask_del)(rtk_l2_lookupMissType_t, rtk_port_t);
    int32   (*l2_newMacOp_get)(rtk_port_t, rtk_l2_newMacLrnMode_t *, rtk_action_t *);
    int32   (*l2_newMacOp_set)(rtk_port_t, rtk_l2_newMacLrnMode_t, rtk_action_t);
    int32   (*l2_nextValidAddr_get)(int32 *, rtk_l2_ucastAddr_t *);
    int32   (*l2_nextValidAddrOnPort_get)(rtk_port_t, int32 *, rtk_l2_ucastAddr_t *);
    int32   (*l2_nextValidMcastAddr_get)(int32 *, rtk_l2_mcastAddr_t *);
    int32   (*l2_nextValidIpMcastAddr_get)(int32 *, rtk_l2_ipMcastAddr_t *);
    int32   (*l2_nextValidEntry_get)(int32 *, rtk_l2_addr_table_t *);
    int32   (*l2_addr_add)(rtk_l2_ucastAddr_t *);
    int32   (*l2_addr_del)(rtk_l2_ucastAddr_t *);
    int32   (*l2_addr_get)(rtk_l2_ucastAddr_t *);
    int32   (*l2_addr_delAll)(uint32);
    int32   (*l2_mcastAddr_add)(rtk_l2_mcastAddr_t *);
    int32   (*l2_mcastAddr_del)(rtk_l2_mcastAddr_t *);
    int32   (*l2_mcastAddr_get)(rtk_l2_mcastAddr_t *);
    int32   (*l2_illegalPortMoveAction_get)(rtk_port_t, rtk_action_t *);
    int32   (*l2_illegalPortMoveAction_set)(rtk_port_t, rtk_action_t);
    int32   (*l2_ipmcMode_get)(rtk_l2_ipmcMode_t *);
    int32   (*l2_ipmcMode_set)(rtk_l2_ipmcMode_t);
    int32   (*l2_ipmcGroupLookupMissHash_get)(rtk_l2_ipmcHashOp_t *);
    int32   (*l2_ipmcGroupLookupMissHash_set)(rtk_l2_ipmcHashOp_t);
    int32   (*l2_ipmcGroup_add)(ipaddr_t, rtk_portmask_t *);
    int32   (*l2_ipmcGroup_del)(ipaddr_t);
    int32   (*l2_ipmcGroup_get)(ipaddr_t, rtk_portmask_t *);
    int32   (*l2_ipMcastAddr_add)(rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddr_del)(rtk_l2_ipMcastAddr_t *);
    int32   (*l2_ipMcastAddr_get)(rtk_l2_ipMcastAddr_t *);
    int32   (*l2_srcPortEgrFilterMask_get)(rtk_portmask_t *);
    int32   (*l2_srcPortEgrFilterMask_set)(rtk_portmask_t *);
    int32   (*l2_extPortEgrFilterMask_get)(rtk_portmask_t *);
    int32   (*l2_extPortEgrFilterMask_set)(rtk_portmask_t *);

    /* Port */
    int32   (*port_init)(void);
    int32   (*port_link_get)(rtk_port_t, rtk_port_linkStatus_t *);
    int32   (*port_speedDuplex_get)(rtk_port_t, rtk_port_speed_t *, rtk_port_duplex_t *);
    int32   (*port_flowctrl_get)(rtk_port_t, uint32 *, uint32 *);
    int32   (*port_phyAutoNegoEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_phyAutoNegoEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*port_phyAutoNegoAbility_get)(rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*port_phyAutoNegoAbility_set)(rtk_port_t, rtk_port_phy_ability_t *);
    int32   (*port_phyForceModeAbility_get)(rtk_port_t, rtk_port_speed_t *, rtk_port_duplex_t *, rtk_enable_t *);
    int32   (*port_phyForceModeAbility_set)(rtk_port_t, rtk_port_speed_t, rtk_port_duplex_t, rtk_enable_t);
    int32   (*port_phyReg_get)(rtk_port_t, uint32, rtk_port_phy_reg_t, uint32 *);
    int32   (*port_phyReg_set)(rtk_port_t, uint32, rtk_port_phy_reg_t, uint32);
    int32   (*port_phyMasterSlave_get)(rtk_port_t, rtk_port_masterSlave_t *);
    int32   (*port_phyMasterSlave_set)(rtk_port_t, rtk_port_masterSlave_t);
    int32   (*port_cpuPortId_get)(rtk_port_t *);
    int32   (*port_isolation_get)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*port_isolation_set)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*port_isolationExt_get)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*port_isolationExt_set)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*port_isolationL34_get)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*port_isolationL34_set)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*port_isolationExtL34_get)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*port_isolationExtL34_set)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*port_isolationEntry_get)(rtk_port_isoConfig_t, rtk_port_t, rtk_portmask_t *, uint32 *, rtk_portmask_t *);
    int32   (*port_isolationEntry_set)(rtk_port_isoConfig_t, rtk_port_t, rtk_portmask_t *, uint32, rtk_portmask_t *);
    int32   (*port_isolationEntryExt_get)(rtk_port_isoConfig_t, rtk_port_t, rtk_portmask_t *, uint32 *, rtk_portmask_t *);
    int32   (*port_isolationEntryExt_set)(rtk_port_isoConfig_t, rtk_port_t, rtk_portmask_t *, uint32, rtk_portmask_t *);
    int32   (*port_isolationCtagPktConfig_get)(rtk_port_isoConfig_t *);
    int32   (*port_isolationCtagPktConfig_set)(rtk_port_isoConfig_t);
    int32   (*port_isolationL34PktConfig_get)(rtk_port_isoConfig_t *);
    int32   (*port_isolationL34PktConfig_set)(rtk_port_isoConfig_t);
    int32   (*port_isolationIpmcLeaky_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_isolationIpmcLeaky_set)(rtk_port_t, rtk_enable_t);
    int32   (*port_macRemoteLoopbackEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_macRemoteLoopbackEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*port_macLocalLoopbackEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_macLocalLoopbackEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*port_adminEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_adminEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*port_specialCongest_get)(rtk_port_t, uint32 *);
    int32   (*port_specialCongest_set)(rtk_port_t, uint32);
    int32   (*port_specialCongestStatus_get)(rtk_port_t, uint32 *);
    int32   (*port_specialCongestStatus_clear)(rtk_port_t);
    int32   (*port_greenEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*port_greenEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*port_phyCrossOverMode_get)(rtk_port_t, rtk_port_crossOver_mode_t *);
    int32   (*port_phyCrossOverMode_set)(rtk_port_t, rtk_port_crossOver_mode_t);
    int32   (*port_enhancedFid_get)(rtk_port_t, rtk_efid_t *);
    int32   (*port_enhancedFid_set)(rtk_port_t, rtk_efid_t);
    int32   (*port_rtctResult_get)(rtk_port_t, rtk_rtctResult_t *);
    int32   (*port_rtct_start)(rtk_portmask_t *);

    /* QoS */
    int32   (*qos_init)(void);
    int32   (*qos_priSelGroup_get)(uint32, rtk_qos_priSelWeight_t *);
    int32   (*qos_priSelGroup_set)(uint32, rtk_qos_priSelWeight_t *);
    int32   (*qos_portPri_get)(rtk_port_t, rtk_pri_t *);
    int32   (*qos_portPri_set)(rtk_port_t, rtk_pri_t);
    int32   (*qos_dscpPriRemapGroup_get)(uint32, uint32, rtk_pri_t *, uint32 *);
    int32   (*qos_dscpPriRemapGroup_set)(uint32, uint32, rtk_pri_t, uint32);
    int32   (*qos_1pPriRemapGroup_get)(uint32, rtk_pri_t, rtk_pri_t *, uint32 *);
    int32   (*qos_1pPriRemapGroup_set)(uint32, rtk_pri_t, rtk_pri_t, uint32);
    int32   (*qos_priMap_get)(uint32, rtk_qos_pri2queue_t *);
    int32   (*qos_priMap_set)(uint32, rtk_qos_pri2queue_t *);
    int32   (*qos_portPriMap_get)(rtk_port_t, uint32 *);
    int32   (*qos_portPriMap_set)(rtk_port_t, uint32);
    int32   (*qos_1pRemarkEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*qos_1pRemarkEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*qos_1pRemarkGroup_get)(uint32, rtk_pri_t, uint32, rtk_pri_t *);
    int32   (*qos_1pRemarkGroup_set)(uint32, rtk_pri_t, uint32, rtk_pri_t);
    int32   (*qos_dscpRemarkEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*qos_dscpRemarkEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*qos_dscpRemarkGroup_get)(uint32, rtk_pri_t, uint32, uint32 *);
    int32   (*qos_dscpRemarkGroup_set)(uint32, rtk_pri_t, uint32, uint32);
    int32   (*qos_fwd2CpuPriRemap_get)(rtk_pri_t, rtk_pri_t *);
    int32   (*qos_fwd2CpuPriRemap_set)(rtk_pri_t, rtk_pri_t);
    int32   (*qos_portDscpRemarkSrcSel_get)(rtk_port_t, rtk_qos_dscpRmkSrc_t *);
    int32   (*qos_portDscpRemarkSrcSel_set)(rtk_port_t, rtk_qos_dscpRmkSrc_t);
    int32   (*qos_dscp2DscpRemarkGroup_get)(uint32, uint32, uint32 *);
    int32   (*qos_dscp2DscpRemarkGroup_set)(uint32, uint32, uint32);
    int32   (*qos_schedulingQueue_get)(rtk_port_t, rtk_qos_queue_weights_t *);
    int32   (*qos_schedulingQueue_set)(rtk_port_t, rtk_qos_queue_weights_t *);

    /* STP */
    int32   (*stp_init)(void);
    int32   (*stp_mstpState_get)(uint32, rtk_port_t, rtk_stp_state_t *);
    int32   (*stp_mstpState_set)(uint32, rtk_port_t, rtk_stp_state_t);

    /* L34 */
    int32   (*l34_init)(void);
    int32   (*l34_netifTable_set)(uint32, rtk_l34_netif_entry_t *);
    int32   (*l34_netifTable_get)(uint32, rtk_l34_netif_entry_t *);

    int32   (*l34_arpTable_set)(uint32, rtk_l34_arp_entry_t *);
    int32   (*l34_arpTable_get)(uint32, rtk_l34_arp_entry_t *);
    int32   (*l34_arpTable_del)(uint32);

    int32   (*l34_pppoeTable_set)(uint32, rtk_l34_pppoe_entry_t *);
    int32   (*l34_pppoeTable_get)(uint32, rtk_l34_pppoe_entry_t *);

    int32   (*l34_routingTable_set)(uint32, rtk_l34_routing_entry_t *);
    int32   (*l34_routingTable_get)(uint32, rtk_l34_routing_entry_t *);
    int32   (*l34_routingTable_del)(uint32);

    int32   (*l34_nexthopTable_set)(uint32, rtk_l34_nexthop_entry_t *);
    int32   (*l34_nexthopTable_get)(uint32, rtk_l34_nexthop_entry_t *);

    int32   (*l34_extIntIPTable_set)(uint32, rtk_l34_ext_intip_entry_t *);
    int32   (*l34_extIntIPTable_get)(uint32, rtk_l34_ext_intip_entry_t *);
    int32   (*l34_extIntIPTable_del)(uint32);
    uint32  (*l34_naptRemHash_get)(uint32, uint32);

    int32   (*l34_naptInboundTable_set)(int8 , uint32 ,rtk_l34_naptInbound_entry_t *);
    int32   (*l34_naptInboundTable_get)(uint32, rtk_l34_naptInbound_entry_t *);
    uint32  (*l34_naptInboundHashidx_get)(uint32 , uint16 , uint16 );

    int32   (*l34_naptOutboundTable_set)(int8 , uint32 ,rtk_l34_naptOutbound_entry_t *);
    int32   (*l34_naptOutboundTable_get)(uint32 ,rtk_l34_naptOutbound_entry_t *);
    uint32  (*l34_naptOutboundHashidx_get)(int8 , uint32 , uint16 , uint32 , uint16);

    int32   (*l34_ipmcTransTable_set)(uint32, rtk_l34_ipmcTrans_entry_t *);
    int32   (*l34_ipmcTransTable_get)(uint32, rtk_l34_ipmcTrans_entry_t *);

    int32  (*l34_table_reset)(rtk_l34_table_type_t);

    /*PON MAC*/
    int32   (*ponmac_init)(void);
    int32   (*ponmac_queue_add)(rtk_ponmac_queue_t *, rtk_ponmac_queueCfg_t *);
    int32   (*ponmac_queue_get)(rtk_ponmac_queue_t *, rtk_ponmac_queueCfg_t *);
    int32   (*ponmac_queue_del)(rtk_ponmac_queue_t *);
    int32   (*ponmac_flow2Queue_set)(uint32, rtk_ponmac_queue_t *);
    int32   (*ponmac_flow2Queue_get)(uint32, rtk_ponmac_queue_t *);

    /* GPON */
    int32 (*gpon_driver_initialize)(void);
    int32 (*gpon_driver_deInitialize)(void);
    int32 (*gpon_device_initialize)(void);
    int32 (*gpon_device_deInitialize)(void);
    int32 (*gpon_eventHandler_stateChange_reg)(rtk_gpon_eventHandleFunc_stateChange_t func);
    int32 (*gpon_eventHandler_dsFecChange_reg)(rtk_gpon_eventHandleFunc_fecChange_t func);
    int32 (*gpon_eventHandler_usFecChange_reg)(rtk_gpon_eventHandleFunc_fecChange_t func);
    int32 (*gpon_eventHandler_usPloamUrgEmpty_reg)(rtk_gpon_eventHandleFunc_usPloamEmpty_t func);
    int32 (*gpon_eventHandler_usPloamNrmEmpty_reg)(rtk_gpon_eventHandleFunc_usPloamEmpty_t func);
    int32 (*gpon_eventHandler_ploam_reg)(rtk_gpon_eventHandleFunc_ploam_t func);
    int32 (*gpon_eventHandler_omci_reg)(rtk_gpon_eventHandleFunc_omci_t func);
    int32 (*gpon_callback_queryAesKey_reg)(rtk_gpon_callbackFunc_queryAesKey_t func);
    int32 (*gpon_eventHandler_alarm_reg)(rtk_gpon_alarm_type_t alarmType, rtk_gpon_eventHandleFunc_fault_t func);
    int32 (*gpon_serialNumber_set)(rtk_gpon_serialNumber_t *sn);
    int32 (*gpon_serialNumber_get)(rtk_gpon_serialNumber_t *sn);
    int32 (*gpon_password_set)(rtk_gpon_password_t *pwd);
    int32 (*gpon_password_get)(rtk_gpon_password_t *pwd);
    int32 (*gpon_parameter_set)(rtk_gpon_patameter_type_t type, void *pPara);
    int32 (*gpon_parameter_get)(rtk_gpon_patameter_type_t type, void *pPara);
    int32 (*gpon_activate)(rtk_gpon_initialState_t initState);
    int32 (*gpon_deActivate)(void);
    int32 (*gpon_ponStatus_get)(rtk_gpon_fsm_status_t* status);
    void (*gpon_isr_entry)(void);
    int32 (*gpon_tcont_create)(rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr);
    int32 (*gpon_tcont_destroy)(rtk_gpon_tcont_ind_t* ind);
    int32 (*gpon_tcont_get)(rtk_gpon_tcont_ind_t* ind, rtk_gpon_tcont_attr_t* attr);
    int32 (*gpon_dsFlow_set)(uint32 flowId, rtk_gpon_dsFlow_attr_t* attr);
    int32 (*gpon_dsFlow_get)(uint32 flowId, rtk_gpon_dsFlow_attr_t* attr);
    int32 (*gpon_usFlow_set)(uint32 flowId, rtk_gpon_usFlow_attr_t* attr);
    int32 (*gpon_usFlow_get)(uint32 flowId, rtk_gpon_usFlow_attr_t* attr);
    int32 (*gpon_ploam_send)(int32 urgent, rtk_gpon_ploam_t* ploam);
    int32 (*gpon_broadcastPass_set)(int32 mode);
    int32 (*gpon_broadcastPass_get)(int32* mode);
    int32 (*gpon_nonMcastPass_set)(int32 mode);
    int32 (*gpon_nonMcastPass_get)(int32* mode);
#if 0 /* wellknown addr is removed in GPON_MAC_SWIO_r1.1 */
    int32 (*gpon_wellKnownAddr_set)(int32 mode, uint32 addr);
    int32 (*gpon_wellKnownAddr_get)(int32* mode, uint32* addr);
#endif
    int32 (*gpon_multicastAddrCheck_set)(uint32 ipv4_pattern, uint32 ipv6_pattern);
    int32 (*gpon_multicastAddrCheck_get)(uint32 *ipv4_pattern, uint32 *ipv6_pattern);
    int32 (*gpon_macFilterMode_set)(rtk_gpon_macTable_exclude_mode_t mode);
    int32 (*gpon_macFilterMode_get)(rtk_gpon_macTable_exclude_mode_t* mode);
    int32 (*gpon_mcForceMode_set)(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6);
    int32 (*gpon_mcForceMode_get)(rtk_gpon_mc_force_mode_t *ipv4, rtk_gpon_mc_force_mode_t *ipv6);
    int32 (*gpon_macEntry_add)(rtk_gpon_macTable_entry_t* entry);
    int32 (*gpon_macEntry_del)(rtk_gpon_macTable_entry_t* entry);
    int32 (*gpon_macEntry_get)(uint32 index, rtk_gpon_macTable_entry_t* entry);
    int32 (*gpon_rdi_set)(int32 enable);
    int32 (*gpon_rdi_get)(int32* enable);
    int32 (*gpon_powerLevel_set)(uint8 level);
    int32 (*gpon_powerLevel_get)(uint8* level);
    int32 (*gpon_alarmStatus_get)(rtk_gpon_alarm_type_t alarm, int32 *status);
    int32 (*gpon_globalCounter_get )(rtk_gpon_global_performance_type_t type, rtk_gpon_global_counter_t* para);
    int32 (*gpon_tcontCounter_get )(uint32 tcontId, rtk_gpon_tcont_performance_type_t type, rtk_gpon_tcont_counter_t *pPara);
    int32 (*gpon_flowCounter_get )(uint32 flowId, rtk_gpon_flow_performance_type_t type, rtk_gpon_flow_counter_t *pPara);
    int32 (*gpon_version_get)(rtk_gpon_device_ver_t* hver, rtk_gpon_driver_ver_t* sver);
#if 0 /* gemloop is removed in GPON_MAC_SWIO_v1.1 */
    int32 (*gpon_gemLoop_set)(int32 loop);
    int32 (*gpon_gemLoop_get)(int32 *pLoop);
#endif
    int32 (*gpon_txForceLaser_set)(rtk_gpon_laser_status_t status);
    int32 (*gpon_txForceLaser_get)(rtk_gpon_laser_status_t *pStatus);
    int32 (*gpon_txForceIdle_set)(int32 on);
    int32 (*gpon_txForceIdle_get)(int32 *pOn);
#if 0
    int32 (*gpon_txForcePRBS_set)(int32 on);
    int32 (*gpon_txForcePRBS_get)(int32* pOn);
#endif
    int32 (*gpon_dsFecSts_get)(int32* en);
    void (*gpon_version_show)(void);
    void (*gpon_devInfo_show)(void);
    void (*gpon_gtc_show)(void);
    void (*gpon_tcont_show)(uint32 tcont);
    void (*gpon_dsFlow_show)(uint32 flow);
    void (*gpon_usFlow_show)(uint32 flow);
    void (*gpon_macTable_show)(void);
    void (*gpon_globalCounter_show)(rtk_gpon_global_performance_type_t type);
    void (*gpon_tcontCounter_show)(uint32 idx, rtk_gpon_tcont_performance_type_t type);
    void (*gpon_flowCounter_show)(uint32 idx, rtk_gpon_flow_performance_type_t type);
#if defined(OLD_FPGA_DEFINED)
    int32 (*gpon_pktGen_cfg_set)(uint32 item, uint32 tcont, uint32 len, uint32 gem, int32 omci);
    int32 (*gpon_pktGen_buf_set)(uint32 item, uint8 *buf, uint32 len);
#endif
    int32 (*gpon_omci_tx)(rtk_gpon_omci_msg_t* omci);
    int32 (*gpon_omci_rx)(rtk_gpon_omci_msg_t* omci);
    int32 (*gpon_autoTcont_set)(int32 state);
    int32 (*gpon_autoTcont_get)(int32 *pState);
    int32 (*gpon_autoBoh_set)(int32 state);
    int32 (*gpon_autoBoh_get)(int32 *pState);
    int32 (*gpon_eqdOffset_set)(int32 offset);
    int32 (*gpon_eqdOffset_get)(int32 *pOffset);

    /* Classification */
    int32   (*classify_init)(void);
    int32   (*classify_cfgEntry_add)(rtk_classify_cfg_t *);
    int32   (*classify_cfgEntry_get)(rtk_classify_cfg_t *);
    int32   (*classify_cfgEntry_del)(uint32);
    int32   (*classify_field_add)(rtk_classify_cfg_t *, rtk_classify_field_t *);
    int32   (*classify_unmatchAction_set)(rtk_classify_unmatch_action_t);
    int32   (*classify_unmatchAction_get)(rtk_classify_unmatch_action_t *);
    int32   (*classify_portRange_set)(rtk_classify_rangeCheck_l4Port_t *);
    int32   (*classify_portRange_get)(rtk_classify_rangeCheck_l4Port_t *);
    int32   (*classify_ipRange_set)(rtk_classify_rangeCheck_ip_t *);
    int32   (*classify_ipRange_get)(rtk_classify_rangeCheck_ip_t *);
    int32   (*classify_cf_sel_set)(rtk_port_t, rtk_classify_cf_sel_t);
    int32   (*classify_cf_sel_get)(rtk_port_t, rtk_classify_cf_sel_t *);
    int32   (*classify_cfPri2Dscp_set)(rtk_pri_t pri, rtk_dscp_t dscp);
    int32   (*classify_cfPri2Dscp_get)(rtk_pri_t pri, rtk_dscp_t *pDscp);


    /* statistics */
    int32   (*stat_init)(void);
    int32   (*stat_global_reset)(void);
    int32   (*stat_port_reset)(rtk_port_t);
    int32   (*stat_log_reset)(uint32);
    int32   (*stat_rst_cnt_value_set)(rtk_mib_rst_value_t);
    int32   (*stat_rst_cnt_value_get)(rtk_mib_rst_value_t *);
    int32   (*stat_global_get)(rtk_stat_global_type_t, uint64 *);
    int32   (*stat_global_getAll)(rtk_stat_global_cntr_t *);
    int32   (*stat_port_get)(rtk_port_t, rtk_stat_port_type_t, uint64 *);
    int32   (*stat_port_getAll)(rtk_port_t, rtk_stat_port_cntr_t *);
    int32   (*stat_log_get)(uint32, uint64 *);
    int32   (*stat_log_ctrl_set)(uint32, rtk_stat_log_ctrl_t);
    int32   (*stat_log_ctrl_get)(uint32, rtk_stat_log_ctrl_t *);
    int32   (*stat_mib_cnt_mode_get)(rtk_mib_count_mode_t *);
    int32   (*stat_mib_cnt_mode_set)(rtk_mib_count_mode_t);
    int32   (*stat_mib_latch_timer_get)(uint32 *);
    int32   (*stat_mib_latch_timer_set)(uint32);
    int32   (*stat_mib_sync_mode_get)(rtk_mib_sync_mode_t *);
    int32   (*stat_mib_sync_mode_set)(rtk_mib_sync_mode_t);
    int32   (*stat_mib_count_tag_length_get)(rtk_mib_tag_cnt_dir_t, rtk_mib_tag_cnt_state_t *);
    int32   (*stat_mib_count_tag_length_set)(rtk_mib_tag_cnt_dir_t, rtk_mib_tag_cnt_state_t);
    int32   (*stat_pktInfo_get)(rtk_port_t, uint32 *);

    /* svlan */
    int32   (*svlan_init)(void);
    int32   (*svlan_create)(rtk_vlan_t);
    int32   (*svlan_destroy)(rtk_vlan_t);
    int32   (*svlan_portSvid_get)(rtk_port_t, rtk_vlan_t *);
    int32   (*svlan_portSvid_set)(rtk_port_t, rtk_vlan_t);
    int32   (*svlan_servicePort_get)(rtk_port_t, rtk_enable_t *);
    int32   (*svlan_servicePort_set)(rtk_port_t, rtk_enable_t);
    int32   (*svlan_memberPort_set)(rtk_vlan_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*svlan_memberPort_get)(rtk_vlan_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*svlan_tpidEntry_get)(uint32, uint32 *);
    int32   (*svlan_tpidEntry_set)(uint32, uint32);
    int32   (*svlan_priorityRef_set)(rtk_svlan_pri_ref_t);
    int32   (*svlan_priorityRef_get)(rtk_svlan_pri_ref_t *);
    int32   (*svlan_memberPortEntry_set)(rtk_svlan_memberCfg_t *);
    int32   (*svlan_memberPortEntry_get)(rtk_svlan_memberCfg_t *);
    int32   (*svlan_ipmc2s_add)(ipaddr_t, ipaddr_t, rtk_vlan_t);
    int32   (*svlan_ipmc2s_del)(ipaddr_t, ipaddr_t);
    int32   (*svlan_ipmc2s_get)(ipaddr_t, ipaddr_t, rtk_vlan_t *);
    int32   (*svlan_l2mc2s_add)(rtk_mac_t, rtk_mac_t, rtk_vlan_t);
    int32   (*svlan_l2mc2s_del)(rtk_mac_t, rtk_mac_t);
    int32   (*svlan_l2mc2s_get)(rtk_mac_t, rtk_mac_t, rtk_vlan_t *);
    int32   (*svlan_sp2c_add)(rtk_vlan_t, rtk_port_t, rtk_vlan_t);
    int32   (*svlan_sp2c_get)(rtk_vlan_t, rtk_port_t, rtk_vlan_t *);
    int32   (*svlan_sp2c_del)(rtk_vlan_t, rtk_port_t);
    int32   (*svlan_dmacVidSelState_set)(rtk_port_t, rtk_enable_t);
    int32   (*svlan_dmacVidSelState_get)(rtk_port_t, rtk_enable_t *);
    int32   (*svlan_unmatchAction_set)(rtk_svlan_action_t, rtk_vlan_t);
    int32   (*svlan_unmatchAction_get)(rtk_svlan_action_t *, rtk_vlan_t *);
    int32   (*svlan_untagAction_set)(rtk_svlan_action_t, rtk_vlan_t);
    int32   (*svlan_untagAction_get)(rtk_svlan_action_t *, rtk_vlan_t *);
    int32   (*svlan_c2s_add)(rtk_vlan_t, rtk_port_t, rtk_vlan_t);
    int32   (*svlan_c2s_del)(rtk_vlan_t, rtk_port_t, rtk_vlan_t);
    int32   (*svlan_c2s_get)(rtk_vlan_t, rtk_port_t, rtk_vlan_t *);
    int32   (*svlan_trapPri_get)(rtk_pri_t *);
    int32   (*svlan_trapPri_set)(rtk_pri_t);
    int32   (*svlan_deiKeepState_get)(rtk_enable_t *);
    int32   (*svlan_deiKeepState_set)(rtk_enable_t);
    int32   (*svlan_lookupType_get)(rtk_svlan_lookupType_t *);
    int32   (*svlan_lookupType_set)(rtk_svlan_lookupType_t);
    int32   (*svlan_sp2cUnmatchCtagging_get)(rtk_enable_t *);
    int32   (*svlan_sp2cUnmatchCtagging_set)(rtk_enable_t);

    /*acl*/
    int32   (*acl_init)(void);
    int32   (*acl_template_set)(rtk_acl_template_t *);
    int32   (*acl_template_get)(rtk_acl_template_t *);
    int32   (*acl_fieldSelect_set)(rtk_acl_field_entry_t *);
    int32   (*acl_fieldSelect_get)(rtk_acl_field_entry_t *);
    int32   (*acl_ruleEntry_get)(rtk_acl_ingress_entry_t *);
    int32   (*acl_igrField_add)(rtk_acl_ingress_entry_t *, rtk_acl_field_t *);
    int32   (*acl_igrRuleEntry_add)(rtk_acl_ingress_entry_t *);
    int32   (*acl_igrRuleEntry_del)(uint32);
    int32   (*acl_igrRuleEntry_delAll)(void);
    int32   (*acl_igrUnmatchAction_set)(rtk_port_t, rtk_filter_unmatch_action_type_t );
    int32   (*acl_igrUnmatchAction_get)(rtk_port_t, rtk_filter_unmatch_action_type_t *);
    int32   (*acl_igrState_set)(rtk_port_t, rtk_enable_t);
    int32   (*acl_igrState_get)(rtk_port_t, rtk_enable_t *);
    int32   (*acl_ipRange_set)(rtk_acl_rangeCheck_ip_t *);
    int32   (*acl_ipRange_get)(rtk_acl_rangeCheck_ip_t *);
    int32   (*acl_vidRange_set)(rtk_acl_rangeCheck_vid_t *);
    int32   (*acl_vidRange_get)(rtk_acl_rangeCheck_vid_t *);
    int32   (*acl_portRange_set)(rtk_acl_rangeCheck_l4Port_t *);
    int32   (*acl_portRange_get)(rtk_acl_rangeCheck_l4Port_t *);
    int32   (*acl_packetLengthRange_set)(rtk_acl_rangeCheck_pktLength_t *);
    int32   (*acl_packetLengthRange_get)(rtk_acl_rangeCheck_pktLength_t *);
    int32   (*acl_igrRuleMode_set)(rtk_acl_igr_rule_mode_t );
    int32   (*acl_igrRuleMode_get)(rtk_acl_igr_rule_mode_t *);
    int32   (*acl_igrPermitState_set)(rtk_port_t, rtk_enable_t);
    int32   (*acl_igrPermitState_get)(rtk_port_t, rtk_enable_t *);

    /*sec*/
    int32   (*sec_init)(void);
    int32   (*sec_portAttackPreventState_get)(rtk_port_t, rtk_enable_t *);
    int32   (*sec_portAttackPreventState_set)(rtk_port_t, rtk_enable_t);
    int32   (*sec_attackPrevent_get)(rtk_sec_attackType_t, rtk_action_t *);
    int32   (*sec_attackPrevent_set)(rtk_sec_attackType_t, rtk_action_t);
    int32   (*sec_attackFloodThresh_get)(rtk_sec_attackFloodType_t, uint32 *);
    int32   (*sec_attackFloodThresh_set)(rtk_sec_attackFloodType_t, uint32);

    /*rate*/
    int32   (*rate_init)(void);
    int32   (*rate_portIgrBandwidthCtrlRate_get)(rtk_port_t, uint32 *);
    int32   (*rate_portIgrBandwidthCtrlRate_set)(rtk_port_t, uint32);
    int32   (*rate_portIgrBandwidthCtrlIncludeIfg_get)(rtk_port_t, rtk_enable_t *);
    int32   (*rate_portIgrBandwidthCtrlIncludeIfg_set)(rtk_port_t, rtk_enable_t);
    int32   (*rate_portEgrBandwidthCtrlRate_get)(rtk_port_t, uint32 *);
    int32   (*rate_portEgrBandwidthCtrlRate_set)(rtk_port_t, uint32);
    int32   (*rate_egrBandwidthCtrlIncludeIfg_get)(rtk_enable_t *);
    int32   (*rate_egrBandwidthCtrlIncludeIfg_set)(rtk_enable_t);
    int32   (*rate_portEgrBandwidthCtrlIncludeIfg_get)(rtk_port_t, rtk_enable_t *);
    int32   (*rate_portEgrBandwidthCtrlIncludeIfg_set)(rtk_port_t, rtk_enable_t);
    int32   (*rate_egrQueueBwCtrlEnable_get)(rtk_port_t, rtk_qid_t, rtk_enable_t *);
    int32   (*rate_egrQueueBwCtrlEnable_set)(rtk_port_t, rtk_qid_t, rtk_enable_t);
    int32   (*rate_egrQueueBwCtrlMeterIdx_get)(rtk_port_t, rtk_qid_t, uint32 *);
    int32   (*rate_egrQueueBwCtrlMeterIdx_set)(rtk_port_t, rtk_qid_t, uint32);
    int32   (*rate_stormControlMeterIdx_get)(rtk_port_t, rtk_rate_storm_group_t, uint32 *);
    int32   (*rate_stormControlMeterIdx_set)(rtk_port_t, rtk_rate_storm_group_t, uint32);
    int32   (*rate_stormControlPortEnable_get)(rtk_port_t, rtk_rate_storm_group_t, rtk_enable_t *);
    int32   (*rate_stormControlPortEnable_set)(rtk_port_t, rtk_rate_storm_group_t, rtk_enable_t);
    int32   (*rate_stormControlEnable_get)(rtk_rate_storm_group_ctrl_t *);
    int32   (*rate_stormControlEnable_set)(rtk_rate_storm_group_ctrl_t *);
    int32   (*rate_stormBypass_set)(rtk_storm_bypass_t, rtk_enable_t);
    int32   (*rate_stormBypass_get)(rtk_storm_bypass_t, rtk_enable_t *);
    int32   (*rate_shareMeter_set)(uint32, uint32, rtk_enable_t);
    int32   (*rate_shareMeter_get)(uint32, uint32 *, rtk_enable_t *);
    int32   (*rate_shareMeterBucket_set)(uint32, uint32);
    int32   (*rate_shareMeterBucket_get)(uint32, uint32 *);
    int32   (*rate_shareMeterExceed_get)(uint32, uint32 *);
    int32   (*rate_shareMeterExceed_clear)(uint32);

    /* Mirror */
    int32   (*mirror_init)(void);
    int32   (*mirror_portBased_set)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*mirror_portBased_get)(rtk_port_t *, rtk_portmask_t *, rtk_portmask_t *);
    int32   (*mirror_portIso_set)(rtk_enable_t);
    int32   (*mirror_portIso_get)(rtk_enable_t *);

    /* Trunk */
    int32   (*trunk_init)(void);
    int32   (*trunk_distributionAlgorithm_get)(uint32, uint32 *);
    int32   (*trunk_distributionAlgorithm_set)(uint32, uint32);
    int32   (*trunk_port_get)(uint32, rtk_portmask_t *);
    int32   (*trunk_port_set)(uint32, rtk_portmask_t *);
    int32   (*trunk_hashMappingTable_get)(uint32, rtk_trunk_hashVal2Port_t *);
    int32   (*trunk_hashMappingTable_set)(uint32, rtk_trunk_hashVal2Port_t *);
    int32   (*trunk_mode_get)(rtk_trunk_mode_t *);
    int32   (*trunk_mode_set)(rtk_trunk_mode_t);
    int32   (*trunk_trafficSeparate_get)(uint32, rtk_trunk_separateType_t *);
    int32   (*trunk_trafficSeparate_set)(uint32, rtk_trunk_separateType_t);
    int32   (*trunk_portQueueEmpty_get)(rtk_portmask_t *);
    int32   (*trunk_trafficPause_get)(uint32, rtk_enable_t *);
    int32   (*trunk_trafficPause_set)(uint32, rtk_enable_t);

    /*LED*/
    int32   (*led_init)(void);
    int32   (*led_operation_get)(rtk_led_operation_t *);
    int32   (*led_operation_set)(rtk_led_operation_t);
    int32   (*led_serialMode_get)(rtk_led_active_t *);
    int32   (*led_serialMode_set)(rtk_led_active_t);
    int32   (*led_blinkRate_get)(rtk_led_blinkGroup_t, rtk_led_blink_rate_t *);
    int32   (*led_blinkRate_set)(rtk_led_blinkGroup_t, rtk_led_blink_rate_t);
    int32   (*led_config_set)(uint32, rtk_led_type_t, rtk_led_config_t *);
    int32   (*led_config_get)(uint32, rtk_led_type_t *, rtk_led_config_t *);
    int32   (*led_modeForce_get)(uint32, rtk_led_force_mode_t *);
    int32   (*led_modeForce_set)(uint32, rtk_led_force_mode_t);

    /*dot1x*/
    int32   (*dot1x_init)(void);
    int32   (*dot1x_unauthPacketOper_get)(rtk_port_t, rtk_action_t *);
    int32   (*dot1x_unauthPacketOper_set)(rtk_port_t, rtk_action_t);
    int32   (*dot1x_portBasedEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*dot1x_portBasedEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*dot1x_portBasedAuthStatus_get)(rtk_port_t, rtk_dot1x_auth_status_t *);
    int32   (*dot1x_portBasedAuthStatus_set)(rtk_port_t, rtk_dot1x_auth_status_t);
    int32   (*dot1x_portBasedDirection_get)(rtk_port_t, rtk_dot1x_direction_t *);
    int32   (*dot1x_portBasedDirection_set)(rtk_port_t, rtk_dot1x_direction_t);
    int32   (*dot1x_macBasedEnable_get)(rtk_port_t, rtk_enable_t *);
    int32   (*dot1x_macBasedEnable_set)(rtk_port_t, rtk_enable_t);
    int32   (*dot1x_macBasedDirection_get)(rtk_dot1x_direction_t *);
    int32   (*dot1x_macBasedDirection_set)(rtk_dot1x_direction_t);
    int32   (*dot1x_guestVlan_get)(rtk_vlan_t *);
    int32   (*dot1x_guestVlan_set)(rtk_vlan_t);
    int32   (*dot1x_guestVlanBehavior_get)(rtk_dot1x_guestVlanBehavior_t *);
    int32   (*dot1x_guestVlanBehavior_set)(rtk_dot1x_guestVlanBehavior_t);
    int32   (*dot1x_trapPri_get)(rtk_pri_t *);
    int32   (*dot1x_trapPri_set)(rtk_pri_t);

    /*oma*/
    int32   (*oam_init)(void);
    int32   (*oam_parserAction_set)(rtk_port_t, rtk_oam_parser_act_t);
    int32   (*oam_parserAction_get)(rtk_port_t, rtk_oam_parser_act_t *);
    int32   (*oam_multiplexerAction_set)(rtk_port_t, rtk_oam_multiplexer_act_t);
    int32   (*oam_multiplexerAction_get)(rtk_port_t, rtk_oam_multiplexer_act_t *);

	/*interrupt*/
	int32   (*intr_init)(void);
	int32   (*intr_polarity_set)(rtk_intr_polarity_t);
	int32   (*intr_polarity_get)(rtk_intr_polarity_t *);
	int32   (*intr_imr_set)(rtk_intr_type_t, rtk_enable_t);
	int32   (*intr_imr_get)(rtk_intr_type_t, rtk_enable_t *);
	int32   (*intr_ims_get)(rtk_intr_type_t, rtk_enable_t *);
	int32   (*intr_ims_clear)(rtk_intr_type_t);
	int32   (*intr_speedChangeStatus_get)(rtk_portmask_t *);
	int32   (*intr_speedChangeStatus_clear)(void);
	int32   (*intr_linkupStatus_get)(rtk_portmask_t *);
	int32   (*intr_linkupStatus_clear)(void);
	int32   (*intr_linkdownStatus_get)(rtk_portmask_t *);
	int32   (*intr_linkdownStatus_clear)(void);
	int32   (*intr_gphyStatus_get)(rtk_portmask_t *);
	int32   (*intr_gphyStatus_clear)(void);
	int32   (*intr_imr_restore)(uint32 );

    /* RLDP and RLPP */
    int32   (*rldp_init)(void);
    int32   (*rldp_config_set)(rtk_rldp_config_t *);
    int32   (*rldp_config_get)(rtk_rldp_config_t *);
    int32   (*rldp_portConfig_set)(rtk_port_t, rtk_rldp_portConfig_t *);
    int32   (*rldp_portConfig_get)(rtk_port_t, rtk_rldp_portConfig_t *);
    int32   (*rldp_status_get)(rtk_rldp_status_t *);
    int32   (*rldp_portStatus_get)(rtk_port_t, rtk_rldp_portStatus_t *);
    int32   (*rldp_portStatus_clear)(rtk_port_t, rtk_rldp_portStatus_t *);
    int32   (*rlpp_init)(void);
    int32   (*rlpp_trapType_set)(rtk_rlpp_trapType_t);
    int32   (*rlpp_trapType_get)(rtk_rlpp_trapType_t *);

    /*cpu*/
    int32   (*cpu_init)(void);
    int32   (*cpu_awarePortMask_set)(rtk_portmask_t);
    int32   (*cpu_awarePortMask_get)(rtk_portmask_t *);
    int32   (*cpu_tagFormat_set)(rtk_cpu_tag_fmt_t);
    int32   (*cpu_tagFormat_get)(rtk_cpu_tag_fmt_t *);
    int32   (*cpu_trapInsertTag_set)(rtk_enable_t);
    int32   (*cpu_trapInsertTag_get)(rtk_enable_t *);
} dal_mapper_t;




/*NAPTR Inbound table access*/


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Module Name : */


#endif /* __DAL_MAPPER_H __ */
