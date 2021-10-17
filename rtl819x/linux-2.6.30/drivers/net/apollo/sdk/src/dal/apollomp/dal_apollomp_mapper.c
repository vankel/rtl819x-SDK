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
 */

/*
 * Include Files
 */
#include <common/debug/rt_log.h>
#include <dal/dal_mapper.h>
#include <dal/dal_common.h>
#include <dal/apollomp/dal_apollomp_mapper.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <dal/apollomp/dal_apollomp_svlan.h>
#include <dal/apollomp/dal_apollomp_oam.h>
#include <dal/apollomp/dal_apollomp_acl.h>
#include <dal/apollomp/dal_apollomp_stp.h>
#include <dal/apollomp/dal_apollomp_qos.h>
#include <dal/apollomp/dal_apollomp_rate.h>
#include <dal/apollomp/dal_apollomp_sec.h>
#include <dal/apollomp/dal_apollomp_port.h>
#include <dal/apollomp/dal_apollomp_classify.h>
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <dal/apollomp/dal_apollomp_stat.h>
#include <dal/apollomp/dal_apollomp_trunk.h>
#include <dal/apollomp/dal_apollomp_trap.h>
#include <dal/apollomp/dal_apollomp_l2.h>
#include <dal/apollomp/dal_apollomp_vlan.h>
#include <dal/apollomp/dal_apollomp_mirror.h>
#include <dal/apollomp/dal_apollomp_gpon.h>
#include <dal/apollomp/dal_apollomp_rldp.h>
#include <dal/apollomp/dal_apollomp_cpu.h>
#include <dal/apollomp/dal_apollomp_intr.h>
#include <dal/apollomp/dal_apollomp_l34.h>
#include <dal/apollomp/dal_apollomp_dot1x.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
dal_mapper_t dal_apollomp_mapper =
{
    APOLLOMP_CHIP_ID,
    ._init = dal_apollomp_init,

    /* Switch */
    .switch_init = dal_apollomp_switch_init,
    .switch_phyPortId_get = dal_apollomp_switch_phyPortId_get,
    .switch_logicalPort_get = dal_apollomp_switch_logicalPort_get,
    .switch_port2PortMask_set = dal_apollomp_switch_port2PortMask_set,
    .switch_port2PortMask_clear = dal_apollomp_switch_port2PortMask_clear,
    .switch_portIdInMask_check = dal_apollomp_switch_portIdInMask_check,
    .switch_maxPktLenLinkSpeed_get = dal_apollomp_switch_maxPktLenLinkSpeed_get,
    .switch_maxPktLenLinkSpeed_set = dal_apollomp_switch_maxPktLenLinkSpeed_set,
    .switch_mgmtMacAddr_get = dal_apollomp_switch_mgmtMacAddr_get,
    .switch_mgmtMacAddr_set = dal_apollomp_switch_mgmtMacAddr_set,

    /*SVLAN*/
    .svlan_init = dal_apollomp_svlan_init,
    .svlan_create = dal_apollomp_svlan_create,
    .svlan_destroy = dal_apollomp_svlan_destroy,
    .svlan_portSvid_get = dal_apollomp_svlan_portSvid_get,
    .svlan_portSvid_set = dal_apollomp_svlan_portSvid_set,
    .svlan_servicePort_get = dal_apollomp_svlan_servicePort_get,
    .svlan_servicePort_set = dal_apollomp_svlan_servicePort_set,
    .svlan_memberPort_set = dal_apollomp_svlan_memberPort_set,
    .svlan_memberPort_get = dal_apollomp_svlan_memberPort_get,
    .svlan_tpidEntry_get = dal_apollomp_svlan_tpidEntry_get,
    .svlan_tpidEntry_set = dal_apollomp_svlan_tpidEntry_set,
    .svlan_priorityRef_set = dal_apollomp_svlan_priorityRef_set,
    .svlan_priorityRef_get = dal_apollomp_svlan_priorityRef_get,
    .svlan_memberPortEntry_set = dal_apollomp_svlan_memberPortEntry_set,
    .svlan_memberPortEntry_get = dal_apollomp_svlan_memberPortEntry_get,
    .svlan_ipmc2s_add = dal_apollomp_svlan_ipmc2s_add,
    .svlan_ipmc2s_del = dal_apollomp_svlan_ipmc2s_del,
    .svlan_ipmc2s_get = dal_apollomp_svlan_ipmc2s_get,
    .svlan_l2mc2s_add = dal_apollomp_svlan_l2mc2s_add,
    .svlan_l2mc2s_del = dal_apollomp_svlan_l2mc2s_del,
    .svlan_l2mc2s_get = dal_apollomp_svlan_l2mc2s_get,
    .svlan_sp2c_add = dal_apollomp_svlan_sp2c_add,
    .svlan_sp2c_get = dal_apollomp_svlan_sp2c_get,
    .svlan_sp2c_del = dal_apollomp_svlan_sp2c_del,
    .svlan_dmacVidSelState_set = dal_apollomp_svlan_dmacVidSelState_set,
    .svlan_dmacVidSelState_get = dal_apollomp_svlan_dmacVidSelState_get,
    .svlan_unmatchAction_set = dal_apollomp_svlan_unmatchAction_set,
    .svlan_unmatchAction_get = dal_apollomp_svlan_unmatchAction_get,
    .svlan_untagAction_set = dal_apollomp_svlan_untagAction_set,
    .svlan_untagAction_get = dal_apollomp_svlan_untagAction_get,
    .svlan_c2s_add = dal_apollomp_svlan_c2s_add,
    .svlan_c2s_del = dal_apollomp_svlan_c2s_del,
    .svlan_c2s_get = dal_apollomp_svlan_c2s_get,
    .svlan_trapPri_get = dal_apollomp_svlan_trapPri_get,
    .svlan_trapPri_set = dal_apollomp_svlan_trapPri_set,
    .svlan_deiKeepState_get = dal_apollomp_svlan_deiKeepState_get,
    .svlan_deiKeepState_set = dal_apollomp_svlan_deiKeepState_set,
    .svlan_lookupType_get = dal_apollomp_svlan_lookupType_get,
    .svlan_lookupType_set = dal_apollomp_svlan_lookupType_set,
    .svlan_sp2cUnmatchCtagging_get = dal_apollomp_svlan_sp2cUnmatchCtagging_get,
    .svlan_sp2cUnmatchCtagging_set = dal_apollomp_svlan_sp2cUnmatchCtagging_set,

    /* STP */
    .stp_init = dal_apollomp_stp_init,
    .stp_mstpState_get = dal_apollomp_stp_mstpState_get,
    .stp_mstpState_set = dal_apollomp_stp_mstpState_set,

    /*oam*/
    .oam_init = dal_apollomp_oam_init,
    .oam_parserAction_set = dal_apollomp_oam_parserAction_set,
    .oam_parserAction_get = dal_apollomp_oam_parserAction_get,
    .oam_multiplexerAction_set = dal_apollomp_oam_multiplexerAction_set,
    .oam_multiplexerAction_get = dal_apollomp_oam_multiplexerAction_get,


    /*acl*/
    .acl_init = dal_apollomp_acl_init,
    .acl_template_set = dal_apollomp_acl_template_set,
    .acl_template_get = dal_apollomp_acl_template_get,
    .acl_fieldSelect_set = dal_apollomp_acl_fieldSelect_set,
    .acl_fieldSelect_get = dal_apollomp_acl_fieldSelect_get,
    .acl_ruleEntry_get = dal_apollomp_acl_igrRuleEntry_get,
    .acl_igrField_add = dal_apollomp_acl_igrRuleField_add,
    .acl_igrRuleEntry_add = dal_apollomp_acl_igrRuleEntry_add,
    .acl_igrRuleEntry_del = dal_apollomp_acl_igrRuleEntry_del,
    .acl_igrRuleEntry_delAll = dal_apollomp_acl_igrRuleEntry_delAll,
    .acl_igrUnmatchAction_set = dal_apollomp_acl_igrUnmatchAction_set,
    .acl_igrUnmatchAction_get = dal_apollomp_acl_igrUnmatchAction_get,
    .acl_igrState_set = dal_apollomp_acl_igrState_set,
    .acl_igrState_get = dal_apollomp_acl_igrState_get,
    .acl_ipRange_set = dal_apollomp_acl_ipRange_set,
    .acl_ipRange_get = dal_apollomp_acl_ipRange_get,
    .acl_vidRange_set = dal_apollomp_acl_vidRange_set,
    .acl_vidRange_get = dal_apollomp_acl_vidRange_get,
    .acl_portRange_set = dal_apollomp_acl_portRange_set,
    .acl_portRange_get = dal_apollomp_acl_portRange_get,
    .acl_packetLengthRange_set = dal_apollomp_acl_packetLengthRange_set,
    .acl_packetLengthRange_get = dal_apollomp_acl_packetLengthRange_get,
    .acl_igrRuleMode_set = dal_apollomp_acl_igrRuleMode_set,
    .acl_igrRuleMode_get = dal_apollomp_acl_igrRuleMode_get,
    .acl_igrPermitState_set = dal_apollomp_acl_igrPermitState_set,
    .acl_igrPermitState_get = dal_apollomp_acl_igrPermitState_get,


    /* QoS */
    .qos_init = dal_apollomp_qos_init,
    .qos_priSelGroup_get = dal_apollomp_qos_priSelGroup_get,
    .qos_priSelGroup_set = dal_apollomp_qos_priSelGroup_set,
    .qos_portPri_get = dal_apollomp_qos_portPri_get,
    .qos_portPri_set = dal_apollomp_qos_portPri_set,
    .qos_dscpPriRemapGroup_get = dal_apollomp_qos_dscpPriRemapGroup_get,
    .qos_dscpPriRemapGroup_set = dal_apollomp_qos_dscpPriRemapGroup_set,
    .qos_1pPriRemapGroup_get = dal_apollomp_qos_1pPriRemapGroup_get,
    .qos_1pPriRemapGroup_set = dal_apollomp_qos_1pPriRemapGroup_set,
    .qos_priMap_get = dal_apollomp_qos_priMap_get,
    .qos_priMap_set = dal_apollomp_qos_priMap_set,
    .qos_portPriMap_get = dal_apollomp_qos_portPriMap_get,
    .qos_portPriMap_set = dal_apollomp_qos_portPriMap_set,
    .qos_1pRemarkEnable_get = dal_apollomp_qos_1pRemarkEnable_get,
    .qos_1pRemarkEnable_set = dal_apollomp_qos_1pRemarkEnable_set,
    .qos_1pRemarkGroup_get = dal_apollomp_qos_1pRemarkGroup_get,
    .qos_1pRemarkGroup_set = dal_apollomp_qos_1pRemarkGroup_set,
    .qos_dscpRemarkEnable_get = dal_apollomp_qos_dscpRemarkEnable_get,
    .qos_dscpRemarkEnable_set = dal_apollomp_qos_dscpRemarkEnable_set,
    .qos_dscpRemarkGroup_get = dal_apollomp_qos_dscpRemarkGroup_get,
    .qos_dscpRemarkGroup_set = dal_apollomp_qos_dscpRemarkGroup_set,
    .qos_fwd2CpuPriRemap_get = dal_apollomp_qos_fwd2CpuPriRemap_get,
    .qos_fwd2CpuPriRemap_set = dal_apollomp_qos_fwd2CpuPriRemap_set,
    .qos_portDscpRemarkSrcSel_get = dal_apollomp_qos_portDscpRemarkSrcSel_get,
    .qos_portDscpRemarkSrcSel_set = dal_apollomp_qos_portDscpRemarkSrcSel_set,
    .qos_dscp2DscpRemarkGroup_get = dal_apollomp_qos_dscp2DscpRemarkGroup_get,
    .qos_dscp2DscpRemarkGroup_set = dal_apollomp_qos_dscp2DscpRemarkGroup_set,
    .qos_schedulingQueue_get = dal_apollomp_qos_schedulingQueue_get,
    .qos_schedulingQueue_set = dal_apollomp_qos_schedulingQueue_set,

    /*sec*/
    .sec_init = dal_apollomp_sec_init,
    .sec_portAttackPreventState_get = dal_apollomp_sec_portAttackPreventState_get,
    .sec_portAttackPreventState_set = dal_apollomp_sec_portAttackPreventState_set,
    .sec_attackPrevent_get = dal_apollomp_sec_attackPrevent_get,
    .sec_attackPrevent_set = dal_apollomp_sec_attackPrevent_set,
    .sec_attackFloodThresh_get = dal_apollomp_sec_attackFloodThresh_get,
    .sec_attackFloodThresh_set = dal_apollomp_sec_attackFloodThresh_set,


    /*rate*/
    .rate_init = dal_apollomp_rate_init,
    .rate_portIgrBandwidthCtrlRate_get = dal_apollomp_rate_portIgrBandwidthCtrlRate_get,
    .rate_portIgrBandwidthCtrlRate_set = dal_apollomp_rate_portIgrBandwidthCtrlRate_set,
    .rate_portIgrBandwidthCtrlIncludeIfg_get = dal_apollomp_rate_portIgrBandwidthCtrlIncludeIfg_get,
    .rate_portIgrBandwidthCtrlIncludeIfg_set = dal_apollomp_rate_portIgrBandwidthCtrlIncludeIfg_set,
    .rate_portEgrBandwidthCtrlRate_get = dal_apollomp_rate_portEgrBandwidthCtrlRate_get,
    .rate_portEgrBandwidthCtrlRate_set = dal_apollomp_rate_portEgrBandwidthCtrlRate_set,
    .rate_egrBandwidthCtrlIncludeIfg_get = dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_get,
    .rate_egrBandwidthCtrlIncludeIfg_set = dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_set,
    .rate_portEgrBandwidthCtrlIncludeIfg_get = dal_apollomp_rate_portEgrBandwidthCtrlIncludeIfg_get,
    .rate_portEgrBandwidthCtrlIncludeIfg_set = dal_apollomp_rate_portEgrBandwidthCtrlIncludeIfg_set,
    .rate_egrQueueBwCtrlEnable_get = dal_apollomp_rate_egrQueueBwCtrlEnable_get,
    .rate_egrQueueBwCtrlEnable_set = dal_apollomp_rate_egrQueueBwCtrlEnable_set,
    .rate_egrQueueBwCtrlMeterIdx_get = dal_apollomp_rate_egrQueueBwCtrlMeterIdx_get,
    .rate_egrQueueBwCtrlMeterIdx_set = dal_apollomp_rate_egrQueueBwCtrlMeterIdx_set,
    .rate_stormControlMeterIdx_get = dal_apollomp_rate_stormControlMeterIdx_get,
    .rate_stormControlMeterIdx_set = dal_apollomp_rate_stormControlMeterIdx_set,
    .rate_stormControlPortEnable_get = dal_apollomp_rate_stormControlPortEnable_get,
    .rate_stormControlPortEnable_set = dal_apollomp_rate_stormControlPortEnable_set,
    .rate_stormControlEnable_get = dal_apollomp_rate_stormControlEnable_get,
    .rate_stormControlEnable_set = dal_apollomp_rate_stormControlEnable_set,
    .rate_stormBypass_set = dal_apollomp_rate_stormBypass_set,
    .rate_stormBypass_get = dal_apollomp_rate_stormBypass_get,
    .rate_shareMeter_set = dal_apollomp_rate_shareMeter_set,
    .rate_shareMeter_get = dal_apollomp_rate_shareMeter_get,
    .rate_shareMeterBucket_set = dal_apollomp_rate_shareMeterBucket_set,
    .rate_shareMeterBucket_get = dal_apollomp_rate_shareMeterBucket_get,
    .rate_shareMeterExceed_get = dal_apollomp_rate_shareMeterExceed_get,
    .rate_shareMeterExceed_clear = dal_apollomp_rate_shareMeterExceed_clear,

    /* Classification */
    .classify_init                              = dal_apollomp_classify_init,
    .classify_cfgEntry_add                      = dal_apollomp_classify_cfgEntry_add,
    .classify_cfgEntry_get                      = dal_apollomp_classify_cfgEntry_get,
    .classify_cfgEntry_del                      = dal_apollomp_classify_cfgEntry_del,
    .classify_field_add                         = dal_apollomp_classify_field_add,
    .classify_unmatchAction_set                 = dal_apollomp_classify_unmatchAction_set,
    .classify_unmatchAction_get                 = dal_apollomp_classify_unmatchAction_get,
    .classify_portRange_set                     = dal_apollomp_classify_portRange_set,
    .classify_portRange_get                     = dal_apollomp_classify_portRange_get,
    .classify_ipRange_set                       = dal_apollomp_classify_ipRange_set,
    .classify_ipRange_get                       = dal_apollomp_classify_ipRange_get,
    .classify_cf_sel_set                        = dal_apollomp_classify_cf_sel_set,
    .classify_cf_sel_get                        = dal_apollomp_classify_cf_sel_get,
    .classify_cfPri2Dscp_set                    = dal_apollomp_classify_cfPri2Dscp_set,
    .classify_cfPri2Dscp_get                    = dal_apollomp_classify_cfPri2Dscp_get,

    /*PON MAC*/
    .ponmac_init = dal_apollomp_ponmac_init,
    .ponmac_queue_add = dal_apollomp_ponmac_queue_add,
    .ponmac_queue_get = dal_apollomp_ponmac_queue_get,
    .ponmac_queue_del = dal_apollomp_ponmac_queue_del,
    .ponmac_flow2Queue_set = dal_apollomp_ponmac_flow2Queue_set,
    .ponmac_flow2Queue_get = dal_apollomp_ponmac_flow2Queue_get,

    /* statistics */
    .stat_init                                  = dal_apollomp_stat_init,
    .stat_global_reset                          = dal_apollomp_stat_global_reset,
    .stat_port_reset                            = dal_apollomp_stat_port_reset,
    .stat_log_reset                             = dal_apollomp_stat_log_reset,
    .stat_rst_cnt_value_set                     = dal_apollomp_stat_rst_cnt_value_set,
    .stat_rst_cnt_value_get                     = dal_apollomp_stat_rst_cnt_value_get,
    .stat_global_get                            = dal_apollomp_stat_global_get,
    .stat_global_getAll                         = dal_apollomp_stat_global_getAll,
    .stat_port_get                              = dal_apollomp_stat_port_get,
    .stat_port_getAll                           = dal_apollomp_stat_port_getAll,
    .stat_log_get                               = dal_apollomp_stat_log_get,
    .stat_log_ctrl_set                          = dal_apollomp_stat_log_ctrl_set,
    .stat_log_ctrl_get                          = dal_apollomp_stat_log_ctrl_get,
    .stat_mib_cnt_mode_get                      = dal_apollomp_stat_mib_cnt_mode_get,
    .stat_mib_cnt_mode_set                      = dal_apollomp_stat_mib_cnt_mode_set,
    .stat_mib_latch_timer_get                   = dal_apollomp_stat_mib_latch_timer_get,
    .stat_mib_latch_timer_set                   = dal_apollomp_stat_mib_latch_timer_set,
    .stat_mib_sync_mode_get                     = dal_apollomp_stat_mib_sync_mode_get,
    .stat_mib_sync_mode_set                     = dal_apollomp_stat_mib_sync_mode_set,
    .stat_mib_count_tag_length_get              = dal_apollomp_stat_mib_count_tag_length_get,
    .stat_mib_count_tag_length_set              = dal_apollomp_stat_mib_count_tag_length_set,
    .stat_pktInfo_get                           = dal_apollomp_stat_pktInfo_get,

    /* Trunk */
    .trunk_init                                 = dal_apollomp_trunk_init,
    .trunk_distributionAlgorithm_get            = dal_apollomp_trunk_distributionAlgorithm_get,
    .trunk_distributionAlgorithm_set            = dal_apollomp_trunk_distributionAlgorithm_set,
    .trunk_port_get                             = dal_apollomp_trunk_port_get,
    .trunk_port_set                             = dal_apollomp_trunk_port_set,
    .trunk_hashMappingTable_get                 = dal_apollomp_trunk_hashMappingTable_get,
    .trunk_hashMappingTable_set                 = dal_apollomp_trunk_hashMappingTable_set,
    .trunk_mode_get                             = dal_apollomp_trunk_mode_get,
    .trunk_mode_set                             = dal_apollomp_trunk_mode_set,
    .trunk_trafficSeparate_get                  = dal_apollomp_trunk_trafficSeparate_get,
    .trunk_trafficSeparate_set                  = dal_apollomp_trunk_trafficSeparate_set,
    .trunk_portQueueEmpty_get                   = dal_apollomp_trunk_portQueueEmpty_get,
    .trunk_trafficPause_get                     = dal_apollomp_trunk_trafficPause_get,
    .trunk_trafficPause_set                     = dal_apollomp_trunk_trafficPause_set,

    /* Port */
    .port_init                                  = dal_apollomp_port_init,
    .port_link_get                              = dal_apollomp_port_link_get,
    .port_speedDuplex_get                       = dal_apollomp_port_speedDuplex_get,
    .port_flowctrl_get                          = dal_apollomp_port_flowctrl_get,
    .port_phyAutoNegoEnable_get                 = dal_apollomp_port_phyAutoNegoEnable_get,
    .port_phyAutoNegoEnable_set                 = dal_apollomp_port_phyAutoNegoEnable_set,
    .port_phyAutoNegoAbility_get                = dal_apollomp_port_phyAutoNegoAbility_get,
    .port_phyAutoNegoAbility_set                = dal_apollomp_port_phyAutoNegoAbility_set,
    .port_phyForceModeAbility_get               = dal_apollomp_port_phyForceModeAbility_get,
    .port_phyForceModeAbility_set               = dal_apollomp_port_phyForceModeAbility_set,
    .port_phyMasterSlave_get                    = dal_apollomp_port_phyMasterSlave_get,
    .port_phyMasterSlave_set                    = dal_apollomp_port_phyMasterSlave_set,
    .port_phyReg_get                            = dal_apollomp_port_phyReg_get,
    .port_phyReg_set                            = dal_apollomp_port_phyReg_set,
    .port_cpuPortId_get                         = dal_apollomp_port_cpuPortId_get,
    .port_isolation_get                         = (int32 (*)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *))dal_common_unavail,
    .port_isolation_set                         = (int32 (*)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *))dal_common_unavail,
    .port_isolationExt_get                      = (int32 (*)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *))dal_common_unavail,
    .port_isolationExt_set                      = (int32 (*)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *))dal_common_unavail,
    .port_isolationL34_get                      = (int32 (*)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *))dal_common_unavail,
    .port_isolationL34_set                      = (int32 (*)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *))dal_common_unavail,
    .port_isolationExtL34_get                   = (int32 (*)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *))dal_common_unavail,
    .port_isolationExtL34_set                   = (int32 (*)(rtk_port_t, rtk_portmask_t *, rtk_portmask_t *))dal_common_unavail,
    .port_isolationEntry_get                    = dal_apollomp_port_isolationEntry_get,
    .port_isolationEntry_set                    = dal_apollomp_port_isolationEntry_set,
    .port_isolationEntryExt_get                 = dal_apollomp_port_isolationEntryExt_get,
    .port_isolationEntryExt_set                 = dal_apollomp_port_isolationEntryExt_set,
    .port_isolationCtagPktConfig_get            = dal_apollomp_port_isolationCtagPktConfig_get,
    .port_isolationCtagPktConfig_set            = dal_apollomp_port_isolationCtagPktConfig_set,
    .port_isolationL34PktConfig_get             = dal_apollomp_port_isolationL34PktConfig_get,
    .port_isolationL34PktConfig_set             = dal_apollomp_port_isolationL34PktConfig_set,
    .port_isolationIpmcLeaky_get                = dal_apollomp_port_isolationIpmcLeaky_get,
    .port_isolationIpmcLeaky_set                = dal_apollomp_port_isolationIpmcLeaky_set,
    .port_macRemoteLoopbackEnable_get           = dal_apollomp_port_macRemoteLoopbackEnable_get,
    .port_macRemoteLoopbackEnable_set           = dal_apollomp_port_macRemoteLoopbackEnable_set,
    .port_macLocalLoopbackEnable_get            = dal_apollomp_port_macLocalLoopbackEnable_get,
    .port_macLocalLoopbackEnable_set            = dal_apollomp_port_macLocalLoopbackEnable_set,
    .port_adminEnable_get                       = dal_apollomp_port_adminEnable_get,
    .port_adminEnable_set                       = dal_apollomp_port_adminEnable_set,
    .port_specialCongest_get                    = dal_apollomp_port_specialCongest_get,
    .port_specialCongest_set                    = dal_apollomp_port_specialCongest_set,
    .port_specialCongestStatus_get              = dal_apollomp_port_specialCongestStatus_get,
    .port_specialCongestStatus_clear            = dal_apollomp_port_specialCongestStatus_clear,
    .port_greenEnable_get                       = dal_apollomp_port_greenEnable_get,
    .port_greenEnable_set                       = dal_apollomp_port_greenEnable_set,
    .port_phyCrossOverMode_get                  = dal_apollomp_port_phyCrossOverMode_get,
    .port_phyCrossOverMode_set                  = dal_apollomp_port_phyCrossOverMode_set,
    .port_enhancedFid_get                       = dal_apollomp_port_enhancedFid_get,
    .port_enhancedFid_set                       = dal_apollomp_port_enhancedFid_set,
    .port_rtctResult_get                        = dal_apollomp_port_rtctResult_get,
    .port_rtct_start                            = dal_apollomp_port_rtct_start,


    /* Trap */
    .trap_init                                  = dal_apollomp_trap_init,
    .trap_reasonTrapToCpuPriority_get           = dal_apollomp_trap_reasonTrapToCpuPriority_get,
    .trap_reasonTrapToCpuPriority_set           = dal_apollomp_trap_reasonTrapToCpuPriority_set,
    .trap_igmpCtrlPkt2CpuEnable_get             = dal_apollomp_trap_igmpCtrlPkt2CpuEnable_get,
    .trap_igmpCtrlPkt2CpuEnable_set             = dal_apollomp_trap_igmpCtrlPkt2CpuEnable_set,
    .trap_mldCtrlPkt2CpuEnable_get              = dal_apollomp_trap_mldCtrlPkt2CpuEnable_get,
    .trap_mldCtrlPkt2CpuEnable_set              = dal_apollomp_trap_mldCtrlPkt2CpuEnable_set,
    .trap_l2McastPkt2CpuEnable_get              = dal_apollomp_trap_l2McastPkt2CpuEnable_get,
    .trap_l2McastPkt2CpuEnable_set              = dal_apollomp_trap_l2McastPkt2CpuEnable_set,
    .trap_ipMcastPkt2CpuEnable_get              = dal_apollomp_trap_ipMcastPkt2CpuEnable_get,
    .trap_ipMcastPkt2CpuEnable_set              = dal_apollomp_trap_ipMcastPkt2CpuEnable_set,
    .trap_rmaAction_get                         = dal_apollomp_trap_rmaAction_get,
    .trap_rmaAction_set                         = dal_apollomp_trap_rmaAction_set,
    .trap_rmaPri_get                            = dal_apollomp_trap_rmaPri_get,
    .trap_rmaPri_set                            = dal_apollomp_trap_rmaPri_set,
    .trap_rmaVlanCheckEnable_get                = dal_apollomp_trap_rmaVlanCheckEnable_get,
    .trap_rmaVlanCheckEnable_set                = dal_apollomp_trap_rmaVlanCheckEnable_set,
    .trap_rmaPortIsolationEnable_get            = dal_apollomp_trap_rmaPortIsolationEnable_get,
    .trap_rmaPortIsolationEnable_set            = dal_apollomp_trap_rmaPortIsolationEnable_set,
    .trap_rmaStormControlEnable_get             = dal_apollomp_trap_rmaStormControlEnable_get,
    .trap_rmaStormControlEnable_set             = dal_apollomp_trap_rmaStormControlEnable_set,
    .trap_rmaKeepCtagEnable_get                 = dal_apollomp_trap_rmaKeepCtagEnable_get,
    .trap_rmaKeepCtagEnable_set                 = dal_apollomp_trap_rmaKeepCtagEnable_set,
    .trap_oamPduAction_get                      = dal_apollomp_trap_oamPduAction_get,
    .trap_oamPduAction_set                      = dal_apollomp_trap_oamPduAction_set,
    .trap_oamPduPri_get                         = dal_apollomp_trap_oamPduPri_get,
    .trap_oamPduPri_set                         = dal_apollomp_trap_oamPduPri_set,



     /* L2 */
    .l2_init                                    = dal_apollomp_l2_init,
    .l2_flushLinkDownPortAddrEnable_get         = dal_apollomp_l2_flushLinkDownPortAddrEnable_get,
    .l2_flushLinkDownPortAddrEnable_set         = dal_apollomp_l2_flushLinkDownPortAddrEnable_set,
    .l2_ucastAddr_flush                         = dal_apollomp_l2_ucastAddr_flush,
    .l2_limitLearningOverStatus_get             = dal_apollomp_l2_limitLearningOverStatus_get,
    .l2_limitLearningOverStatus_clear           = dal_apollomp_l2_limitLearningOverStatus_clear,
    .l2_learningCnt_get                         = dal_apollomp_l2_learningCnt_get,
    .l2_limitLearningCnt_get                    = dal_apollomp_l2_limitLearningCnt_get,
    .l2_limitLearningCnt_set                    = dal_apollomp_l2_limitLearningCnt_set,
    .l2_limitLearningCntAction_get              = dal_apollomp_l2_limitLearningCntAction_get,
    .l2_limitLearningCntAction_set              = dal_apollomp_l2_limitLearningCntAction_set,
    .l2_portLimitLearningOverStatus_get         = dal_apollomp_l2_portLimitLearningOverStatus_get,
    .l2_portLimitLearningOverStatus_clear       = dal_apollomp_l2_portLimitLearningOverStatus_clear,
    .l2_portLearningCnt_get                     = dal_apollomp_l2_portLearningCnt_get,
    .l2_portLimitLearningCnt_get                = dal_apollomp_l2_portLimitLearningCnt_get,
    .l2_portLimitLearningCnt_set                = dal_apollomp_l2_portLimitLearningCnt_set,
    .l2_portLimitLearningCntAction_get          = dal_apollomp_l2_portLimitLearningCntAction_get,
    .l2_portLimitLearningCntAction_set          = dal_apollomp_l2_portLimitLearningCntAction_set,
    .l2_aging_get                               = dal_apollomp_l2_aging_get,
    .l2_aging_set                               = dal_apollomp_l2_aging_set,
    .l2_portAgingEnable_get                     = dal_apollomp_l2_portAgingEnable_get,
    .l2_portAgingEnable_set                     = dal_apollomp_l2_portAgingEnable_set,
    .l2_lookupMissAction_get                    = dal_apollomp_l2_lookupMissAction_get,
    .l2_lookupMissAction_set                    = dal_apollomp_l2_lookupMissAction_set,
    .l2_portLookupMissAction_get                = dal_apollomp_l2_portLookupMissAction_get,
    .l2_portLookupMissAction_set                = dal_apollomp_l2_portLookupMissAction_set,
    .l2_lookupMissFloodPortMask_get             = dal_apollomp_l2_lookupMissFloodPortMask_get,
    .l2_lookupMissFloodPortMask_set             = dal_apollomp_l2_lookupMissFloodPortMask_set,
    .l2_lookupMissFloodPortMask_add             = dal_apollomp_l2_lookupMissFloodPortMask_add,
    .l2_lookupMissFloodPortMask_del             = dal_apollomp_l2_lookupMissFloodPortMask_del,
    .l2_newMacOp_get                            = dal_apollomp_l2_newMacOp_get,
    .l2_newMacOp_set                            = dal_apollomp_l2_newMacOp_set,
    .l2_nextValidAddr_get                       = dal_apollomp_l2_nextValidAddr_get,
    .l2_nextValidAddrOnPort_get                 = dal_apollomp_l2_nextValidAddrOnPort_get,
    .l2_nextValidMcastAddr_get                  = dal_apollomp_l2_nextValidMcastAddr_get,
    .l2_nextValidIpMcastAddr_get                = dal_apollomp_l2_nextValidIpMcastAddr_get,
    .l2_nextValidEntry_get                      = dal_apollomp_l2_nextValidEntry_get,
    .l2_addr_add                                = dal_apollomp_l2_addr_add,
    .l2_addr_del                                = dal_apollomp_l2_addr_del,
    .l2_addr_get                                = dal_apollomp_l2_addr_get,
    .l2_addr_delAll                             = dal_apollomp_l2_addr_delAll,
    .l2_mcastAddr_add                           = dal_apollomp_l2_mcastAddr_add,
    .l2_mcastAddr_del                           = dal_apollomp_l2_mcastAddr_del,
    .l2_mcastAddr_get                           = dal_apollomp_l2_mcastAddr_get,
    .l2_illegalPortMoveAction_get               = dal_apollomp_l2_illegalPortMoveAction_get,
    .l2_illegalPortMoveAction_set               = dal_apollomp_l2_illegalPortMoveAction_set,
    .l2_ipmcMode_get                            = dal_apollomp_l2_ipmcMode_get,
    .l2_ipmcMode_set                            = dal_apollomp_l2_ipmcMode_set,
    .l2_ipmcGroupLookupMissHash_get             = dal_apollomp_l2_ipmcGroupLookupMissHash_get,
    .l2_ipmcGroupLookupMissHash_set             = dal_apollomp_l2_ipmcGroupLookupMissHash_set,
    .l2_ipmcGroup_add                           = dal_apollomp_l2_ipmcGroup_add,
    .l2_ipmcGroup_del                           = dal_apollomp_l2_ipmcGroup_del,
    .l2_ipmcGroup_get                           = dal_apollomp_l2_ipmcGroup_get,
    .l2_ipMcastAddr_add                         = dal_apollomp_l2_ipMcastAddr_add,
    .l2_ipMcastAddr_del                         = dal_apollomp_l2_ipMcastAddr_del,
    .l2_ipMcastAddr_get                         = dal_apollomp_l2_ipMcastAddr_get,
    .l2_srcPortEgrFilterMask_get                = dal_apollomp_l2_srcPortEgrFilterMask_get,
    .l2_srcPortEgrFilterMask_set                = dal_apollomp_l2_srcPortEgrFilterMask_set,
    .l2_extPortEgrFilterMask_get                = dal_apollomp_l2_extPortEgrFilterMask_get,
    .l2_extPortEgrFilterMask_set                = dal_apollomp_l2_extPortEgrFilterMask_set,

     /* VLAN */
    .vlan_init                                  = dal_apollomp_vlan_init,
    .vlan_create                                = dal_apollomp_vlan_create,
    .vlan_destroy                               = dal_apollomp_vlan_destroy,
    .vlan_destroyAll                            = dal_apollomp_vlan_destroyAll,
    .vlan_fid_get                               = dal_apollomp_vlan_fid_get,
    .vlan_fid_set                               = dal_apollomp_vlan_fid_set,
    .vlan_fidMode_get                           = dal_apollomp_vlan_fidMode_get,
    .vlan_fidMode_set                           = dal_apollomp_vlan_fidMode_set,
    .vlan_port_get                              = dal_apollomp_vlan_port_get,
    .vlan_port_set                              = dal_apollomp_vlan_port_set,
    .vlan_extPort_get                           = dal_apollomp_vlan_extPort_get,
    .vlan_extPort_set                           = dal_apollomp_vlan_extPort_set,
    .vlan_stg_get                               = dal_apollomp_vlan_stg_get,
    .vlan_stg_set                               = dal_apollomp_vlan_stg_set,
    .vlan_priority_get                          = dal_apollomp_vlan_priority_get,
    .vlan_priority_set                          = dal_apollomp_vlan_priority_set,
    .vlan_priorityEnable_get                    = dal_apollomp_vlan_priorityEnable_get,
    .vlan_priorityEnable_set                    = dal_apollomp_vlan_priorityEnable_set,
    .vlan_portAcceptFrameType_get               = dal_apollomp_vlan_portAcceptFrameType_get,
    .vlan_portAcceptFrameType_set               = dal_apollomp_vlan_portAcceptFrameType_set,
    .vlan_vlanFunctionEnable_get                = dal_apollomp_vlan_vlanFunctionEnable_get,
    .vlan_vlanFunctionEnable_set                = dal_apollomp_vlan_vlanFunctionEnable_set,
    .vlan_portIgrFilterEnable_get               = dal_apollomp_vlan_portIgrFilterEnable_get,
    .vlan_portIgrFilterEnable_set               = dal_apollomp_vlan_portIgrFilterEnable_set,
    .vlan_leaky_get                             = dal_apollomp_vlan_leaky_get,
    .vlan_leaky_set                             = dal_apollomp_vlan_leaky_set,
    .vlan_portPvid_get                          = dal_apollomp_vlan_portPvid_get,
    .vlan_portPvid_set                          = dal_apollomp_vlan_portPvid_set,
    .vlan_extPortPvid_get                       = dal_apollomp_vlan_extPortPvid_get,
    .vlan_extPortPvid_set                       = dal_apollomp_vlan_extPortPvid_set,
    .vlan_protoGroup_get                        = dal_apollomp_vlan_protoGroup_get,
    .vlan_protoGroup_set                        = dal_apollomp_vlan_protoGroup_set,
    .vlan_portProtoVlan_get                     = dal_apollomp_vlan_portProtoVlan_get,
    .vlan_portProtoVlan_set                     = dal_apollomp_vlan_portProtoVlan_set,
    .vlan_tagMode_get                           = dal_apollomp_vlan_tagMode_get,
    .vlan_tagMode_set                           = dal_apollomp_vlan_tagMode_set,
    .vlan_portFid_get                           = dal_apollomp_vlan_portFid_get,
    .vlan_portFid_set                           = dal_apollomp_vlan_portFid_set,
    .vlan_portPriority_get                      = dal_apollomp_vlan_portPriority_get,
    .vlan_portPriority_set                      = dal_apollomp_vlan_portPriority_set,
    .vlan_portEgrTagKeepType_get                = dal_apollomp_vlan_portEgrTagKeepType_get,
    .vlan_portEgrTagKeepType_set                = dal_apollomp_vlan_portEgrTagKeepType_set,
    .vlan_transparentEnable_get                 = dal_apollomp_vlan_transparentEnable_get,
    .vlan_transparentEnable_set                 = dal_apollomp_vlan_transparentEnable_set,
    .vlan_cfiKeepEnable_get                     = dal_apollomp_vlan_cfiKeepEnable_get,
    .vlan_cfiKeepEnable_set                     = dal_apollomp_vlan_cfiKeepEnable_set,
    .vlan_reservedVidAction_get                 = dal_apollomp_vlan_reservedVidAction_get,
    .vlan_reservedVidAction_set                 = dal_apollomp_vlan_reservedVidAction_set,

    /* Mirror */
    .mirror_init                                = dal_apollomp_mirror_init,
    .mirror_portBased_set                       = dal_apollomp_mirror_portBased_set,
    .mirror_portBased_get                       = dal_apollomp_mirror_portBased_get,
    .mirror_portIso_set                         = dal_apollomp_mirror_portIso_set,
    .mirror_portIso_get                         = dal_apollomp_mirror_portIso_get,


    /* GPON */
    .gpon_driver_initialize                     = dal_apollomp_gpon_driver_initialize,
    .gpon_driver_deInitialize                   = dal_apollomp_gpon_driver_deInitialize,
    .gpon_device_initialize                     = dal_apollomp_gpon_device_initialize,
    .gpon_device_deInitialize                   = dal_apollomp_gpon_device_deInitialize,
    .gpon_eventHandler_stateChange_reg          = dal_apollomp_gpon_eventHandler_stateChange_reg,
    .gpon_eventHandler_dsFecChange_reg          = dal_apollomp_gpon_eventHandler_dsFecChange_reg,
    .gpon_eventHandler_usFecChange_reg          = dal_apollomp_gpon_eventHandler_usFecChange_reg,
    .gpon_eventHandler_usPloamUrgEmpty_reg      = dal_apollomp_gpon_eventHandler_usPloamUrgEmpty_reg,
    .gpon_eventHandler_usPloamNrmEmpty_reg      = dal_apollomp_gpon_eventHandler_usPloamNrmEmpty_reg,
    .gpon_eventHandler_ploam_reg                = dal_apollomp_gpon_eventHandler_ploam_reg,
    .gpon_eventHandler_omci_reg                 = dal_apollomp_gpon_eventHandler_omci_reg,
    .gpon_callback_queryAesKey_reg              = dal_apollomp_gpon_callback_queryAesKey_reg,
    .gpon_eventHandler_alarm_reg                = dal_apollomp_gpon_eventHandler_alarm_reg,
    .gpon_serialNumber_set                      = dal_apollomp_gpon_serialNumber_set,
    .gpon_serialNumber_get                      = dal_apollomp_gpon_serialNumber_get,
    .gpon_password_set                          = dal_apollomp_gpon_password_set,
    .gpon_password_get                          = dal_apollomp_gpon_password_get,
    .gpon_parameter_set                         = dal_apollomp_gpon_parameter_set,
    .gpon_parameter_get                         = dal_apollomp_gpon_parameter_get,
    .gpon_activate                              = dal_apollomp_gpon_activate,
    .gpon_deActivate                            = dal_apollomp_gpon_deActivate,
    .gpon_ponStatus_get                         = dal_apollomp_gpon_ponStatus_get,
    .gpon_isr_entry                             = dal_apollomp_gpon_isr_entry,
    .gpon_tcont_create                          = dal_apollomp_gpon_tcont_create,
    .gpon_tcont_destroy                         = dal_apollomp_gpon_tcont_destroy,
    .gpon_tcont_get                             = dal_apollomp_gpon_tcont_get,
    .gpon_dsFlow_set                            = dal_apollomp_gpon_dsFlow_set,
    .gpon_dsFlow_get                            = dal_apollomp_gpon_dsFlow_get,
    .gpon_usFlow_set                            = dal_apollomp_gpon_usFlow_set,
    .gpon_usFlow_get                            = dal_apollomp_gpon_usFlow_get,
    .gpon_ploam_send                            = dal_apollomp_gpon_ploam_send,
    .gpon_broadcastPass_set                     = dal_apollomp_gpon_broadcastPass_set,
    .gpon_broadcastPass_get                     = dal_apollomp_gpon_broadcastPass_get,
    .gpon_nonMcastPass_set                      = dal_apollomp_gpon_nonMcastPass_set,
    .gpon_nonMcastPass_get                      = dal_apollomp_gpon_nonMcastPass_get,
    .gpon_multicastAddrCheck_set                = dal_apollomp_gpon_multicastAddrCheck_set,
    .gpon_multicastAddrCheck_get                = dal_apollomp_gpon_multicastAddrCheck_get,
    .gpon_macFilterMode_set                     = dal_apollomp_gpon_macFilterMode_set,
    .gpon_macFilterMode_get                     = dal_apollomp_gpon_macFilterMode_get,
    .gpon_mcForceMode_set                       = dal_apollomp_gpon_mcForceMode_set,
    .gpon_mcForceMode_get                       = dal_apollomp_gpon_mcForceMode_get,
    .gpon_macEntry_add                          = dal_apollomp_gpon_macEntry_add,
    .gpon_macEntry_del                          = dal_apollomp_gpon_macEntry_del,
    .gpon_macEntry_get                          = dal_apollomp_gpon_macEntry_get,
    .gpon_rdi_set                               = dal_apollomp_gpon_rdi_set,
    .gpon_rdi_get                               = dal_apollomp_gpon_rdi_get,
    .gpon_powerLevel_set                        = dal_apollomp_gpon_powerLevel_set,
    .gpon_powerLevel_get                        = dal_apollomp_gpon_powerLevel_get,
    .gpon_alarmStatus_get                       = dal_apollomp_gpon_alarmStatus_get,
    .gpon_globalCounter_get                     = dal_apollomp_gpon_globalCounter_get ,
    .gpon_tcontCounter_get                      = dal_apollomp_gpon_tcontCounter_get ,
    .gpon_flowCounter_get                       = dal_apollomp_gpon_flowCounter_get ,
    .gpon_version_get                           = dal_apollomp_gpon_version_get,
    .gpon_txForceLaser_set                      = dal_apollomp_gpon_txForceLaser_set,
    .gpon_txForceLaser_get                      = dal_apollomp_gpon_txForceLaser_get,
    .gpon_txForceIdle_set                       = dal_apollomp_gpon_txForceIdle_set,
    .gpon_txForceIdle_get                       = dal_apollomp_gpon_txForceIdle_get,
    .gpon_dsFecSts_get                          = dal_apollomp_gpon_dsFecSts_get,
    .gpon_version_show                          = dal_apollomp_gpon_version_show,
    .gpon_devInfo_show                          = dal_apollomp_gpon_devInfo_show,
    .gpon_gtc_show                              = dal_apollomp_gpon_gtc_show,
    .gpon_tcont_show                            = dal_apollomp_gpon_tcont_show,
    .gpon_dsFlow_show                           = dal_apollomp_gpon_dsFlow_show,
    .gpon_usFlow_show                           = dal_apollomp_gpon_usFlow_show,
    .gpon_macTable_show                         = dal_apollomp_gpon_macTable_show,
    .gpon_globalCounter_show                    = dal_apollomp_gpon_globalCounter_show,
    .gpon_tcontCounter_show                     = dal_apollomp_gpon_tcontCounter_show,
    .gpon_flowCounter_show                      = dal_apollomp_gpon_flowCounter_show,
#if defined(OLD_FPGA_DEFINED)
    .gpon_pktGen_cfg_set                        = (int32 (*)(uint32, uint32, uint32, uint32, int32))dal_common_unavail,
    .gpon_pktGen_buf_set                        = (int32 (*)(uint32, uint8 *, uint32))dal_common_unavail,
#endif
    .gpon_omci_tx                               = dal_apollomp_gpon_omci_tx,
    .gpon_omci_rx                               = dal_apollomp_gpon_omci_rx,
    .gpon_autoTcont_set                         = dal_apollomp_gpon_auto_tcont_set,
    .gpon_autoTcont_get                         = dal_apollomp_gpon_auto_tcont_get,
    .gpon_autoBoh_set                           = dal_apollomp_gpon_auto_boh_set,
    .gpon_autoBoh_get                           = dal_apollomp_gpon_auto_boh_get,
    .gpon_eqdOffset_set                         = dal_apollomp_gpon_eqd_offset_set,
    .gpon_eqdOffset_get                         = dal_apollomp_gpon_eqd_offset_get,


     /*interrupt*/
    .intr_init = dal_apollomp_intr_init,
    .intr_polarity_set = dal_apollomp_intr_polarity_set,
    .intr_polarity_get = dal_apollomp_intr_polarity_get,
    .intr_imr_set = dal_apollomp_intr_imr_set,
    .intr_imr_get = dal_apollomp_intr_imr_get,
    .intr_ims_get = dal_apollomp_intr_ims_get,
    .intr_ims_clear = dal_apollomp_intr_ims_clear,
    .intr_speedChangeStatus_get = dal_apollomp_intr_speedChangeStatus_get,
    .intr_speedChangeStatus_clear = dal_apollomp_intr_speedChangeStatus_clear,
    .intr_linkupStatus_get = dal_apollomp_intr_linkupStatus_get,
    .intr_linkupStatus_clear = dal_apollomp_intr_linkupStatus_clear,
    .intr_linkdownStatus_get = dal_apollomp_intr_linkdownStatus_get,
    .intr_linkdownStatus_clear = dal_apollomp_intr_linkdownStatus_clear,
    .intr_gphyStatus_get = dal_apollomp_intr_gphyStatus_get,
    .intr_gphyStatus_clear = dal_apollomp_intr_gphyStatus_clear,
    .intr_imr_restore = dal_apollomp_intr_imr_restore,

    /* RLDP and RLPP */
    .rldp_init = dal_apollomp_rldp_init,
    .rldp_config_set = dal_apollomp_rldp_config_set,
    .rldp_config_get = dal_apollomp_rldp_config_get,
    .rldp_portConfig_set = dal_apollomp_rldp_portConfig_set,
    .rldp_portConfig_get = dal_apollomp_rldp_portConfig_get,
    .rldp_status_get = dal_apollomp_rldp_status_get,
    .rldp_portStatus_get = dal_apollomp_rldp_portStatus_get,
    .rldp_portStatus_clear = dal_apollomp_rldp_portStatus_clear,
    .rlpp_init = dal_apollomp_rlpp_init,
    .rlpp_trapType_set = dal_apollomp_rlpp_trapType_set,
    .rlpp_trapType_get = dal_apollomp_rlpp_trapType_get,

    /*cpu*/
    .cpu_init = dal_apollomp_cpu_init,
    .cpu_awarePortMask_set = dal_apollomp_cpu_awarePortMask_set,
    .cpu_awarePortMask_get = dal_apollomp_cpu_awarePortMask_get,
    .cpu_tagFormat_set = dal_apollomp_cpu_tagFormat_set,
    .cpu_tagFormat_get = dal_apollomp_cpu_tagFormat_get,
    .cpu_trapInsertTag_set = dal_apollomp_cpu_trapInsertTag_set,
    .cpu_trapInsertTag_get = dal_apollomp_cpu_trapInsertTag_get,

    /*dot1x*/
    .dot1x_init = dal_apollomp_dot1x_init,
    .dot1x_unauthPacketOper_get = dal_apollomp_dot1x_unauthPacketOper_get,
    .dot1x_unauthPacketOper_set = dal_apollomp_dot1x_unauthPacketOper_set,
    .dot1x_portBasedEnable_get = dal_apollomp_dot1x_portBasedEnable_get,
    .dot1x_portBasedEnable_set = dal_apollomp_dot1x_portBasedEnable_set,
    .dot1x_portBasedAuthStatus_get = dal_apollomp_dot1x_portBasedAuthStatus_get,
    .dot1x_portBasedAuthStatus_set = dal_apollomp_dot1x_portBasedAuthStatus_set,
    .dot1x_portBasedDirection_get = dal_apollomp_dot1x_portBasedDirection_get,
    .dot1x_portBasedDirection_set = dal_apollomp_dot1x_portBasedDirection_set,
    .dot1x_macBasedEnable_get = dal_apollomp_dot1x_macBasedEnable_get,
    .dot1x_macBasedEnable_set = dal_apollomp_dot1x_macBasedEnable_set,
    .dot1x_macBasedDirection_get = dal_apollomp_dot1x_macBasedDirection_get,
    .dot1x_macBasedDirection_set = dal_apollomp_dot1x_macBasedDirection_set,
    .dot1x_guestVlan_get = dal_apollomp_dot1x_guestVlan_get,
    .dot1x_guestVlan_set = dal_apollomp_dot1x_guestVlan_set,
    .dot1x_guestVlanBehavior_get = dal_apollomp_dot1x_guestVlanBehavior_get,
    .dot1x_guestVlanBehavior_set = dal_apollomp_dot1x_guestVlanBehavior_set,
    .dot1x_trapPri_get = dal_apollomp_dot1x_trapPri_get,
    .dot1x_trapPri_set = dal_apollomp_dot1x_trapPri_set,

    /* L34 Function */
    .l34_init = dal_apollomp_l34_init,
    .l34_netifTable_set = dal_apollomp_l34_netifTable_set,
    .l34_netifTable_get = dal_apollomp_l34_netifTable_get,
    .l34_arpTable_set = dal_apollomp_l34_arpTable_set,
    .l34_arpTable_get = dal_apollomp_l34_arpTable_get,
    .l34_arpTable_del = dal_apollomp_l34_arpTable_del,
    .l34_pppoeTable_set = dal_apollomp_l34_pppoeTable_set,
    .l34_pppoeTable_get = dal_apollomp_l34_pppoeTable_get,

    .l34_routingTable_set = dal_apollomp_l34_routingTable_set,
    .l34_routingTable_get = dal_apollomp_l34_routingTable_get,
    .l34_routingTable_del = dal_apollomp_l34_routingTable_del,
    .l34_nexthopTable_set = dal_apollomp_l34_nexthopTable_set,
    .l34_nexthopTable_get = dal_apollomp_l34_nexthopTable_get,
    .l34_extIntIPTable_set = dal_apollomp_l34_extIntIPTable_set,
    .l34_extIntIPTable_get = dal_apollomp_l34_extIntIPTable_get,
    .l34_extIntIPTable_del = dal_apollomp_l34_extIntIPTable_del,
    .l34_naptRemHash_get   = dal_apollomp_l34_naptRemHash_get,
    .l34_naptInboundTable_set   = dal_apollomp_l34_naptInboundTable_set,
    .l34_naptInboundTable_get   = dal_apollomp_l34_naptInboundTable_get,
    .l34_naptInboundHashidx_get   = dal_apollomp_l34_naptInboundHashidx_get,
    .l34_naptOutboundTable_set   = dal_apollomp_l34_naptOutboundTable_set,
    .l34_naptOutboundTable_get   = dal_apollomp_l34_naptOutboundTable_get,
    .l34_naptOutboundHashidx_get   = dal_apollomp_l34_naptOutboundHashidx_get,
    .l34_ipmcTransTable_set     = dal_apollomp_l34_ipmcTransTable_set,
    .l34_ipmcTransTable_get     = dal_apollomp_l34_ipmcTransTable_get,
    .l34_table_reset               = dal_apollomp_l34_table_reset,

};

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */


/* Module Name    :  */

/* Function Name:
 *      dal_apollomp_init
 * Description:
 *      Initilize DAL of smart switch
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - initialize success
 *      RT_ERR_FAILED - initialize fail
 * Note:
 *      RTK must call this function before do other kind of action.
 */
int32 dal_apollomp_init(void)
{
    return RT_ERR_OK;
} /* end of dal_apollomp_init */


