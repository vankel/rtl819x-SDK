/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: 6401 $
 * $Date: 2009-10-14 16:03:12 +0800 (�P���T, 14 �Q�� 2009) $
 *
 * Purpose : Realtek Switch SDK Rtusr API Module
 *
 * Feature : The file have include the following module and sub-modules
 *           1) GPON
 *
 */

/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>


/*
 * Symbol Definition
 */

typedef struct rtdrv_pon_paraLen_s
{
    rtk_gpon_patameter_type_t   type;
    uint32                      len;
}rtdrv_pon_paraLen_t;

/*
 * Data Declaration
 */

static  rtdrv_pon_paraLen_t pon_len[] =
    {{RTK_GPON_PARA_TYPE_ONU_ACTIVATION, sizeof(rtk_gpon_onu_activation_para_t)},
     {RTK_GPON_PARA_TYPE_LASER,          sizeof(rtk_gpon_laser_para_t)},
     {RTK_GPON_PARA_TYPE_DS_PHY,         sizeof(rtk_gpon_ds_physical_para_t)},
     {RTK_GPON_PARA_TYPE_DS_PLOAM,       sizeof(rtk_gpon_ds_ploam_para_t)},
     {RTK_GPON_PARA_TYPE_DS_BWMAP,       sizeof(rtk_gpon_ds_bwMap_para_t)},
     {RTK_GPON_PARA_TYPE_DS_GEM,         sizeof(rtk_gpon_ds_gem_para_t)},
     {RTK_GPON_PARA_TYPE_DS_ETH,         sizeof(rtk_gpon_ds_eth_para_t)},
     {RTK_GPON_PARA_TYPE_DS_OMCI,        sizeof(rtk_gpon_ds_omci_para_t)},
     {RTK_GPON_PARA_TYPE_US_PHY,         sizeof(rtk_gpon_us_physical_para_t)},
     {RTK_GPON_PARA_TYPE_US_PLOAM,       sizeof(rtk_gpon_us_ploam_para_t)},
     {RTK_GPON_PARA_TYPE_US_DBR,         sizeof(rtk_gpon_us_dbr_para_t)},
    };



/*
 * Function Declaration
 */

int32 rtk_gpon_serialNumber_set(rtk_gpon_serialNumber_t *pSN)
{
    SETSOCKOPT(RTDRV_GPON_SN_SET, pSN, rtk_gpon_serialNumber_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_serialNumber_get(rtk_gpon_serialNumber_t *pSN)
{
    GETSOCKOPT(RTDRV_GPON_SN_GET, pSN, rtk_gpon_serialNumber_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_password_set(rtk_gpon_password_t *pPwd)
{
    SETSOCKOPT(RTDRV_GPON_PWD_SET, pPwd, rtk_gpon_password_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_password_get(rtk_gpon_password_t *pPwd)
{
    GETSOCKOPT(RTDRV_GPON_PWD_GET, pPwd, rtk_gpon_password_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_parameter_set(rtk_gpon_patameter_type_t type, void *pPara)
{
    rtdrv_ponPara_t pon;
    int32           para_len=0, i;

    for(i=RTK_GPON_PARA_TYPE_ONU_ACTIVATION; i<RTK_GPON_PARA_TYPE_MAX; i++)
    {
        if(type == i)
            para_len = pon_len[i].len;
    }
    if(0 == para_len)
        return RT_ERR_OUT_OF_RANGE;

    pon.type = type;
    osal_memcpy(&pon.para, pPara, para_len);

    SETSOCKOPT(RTDRV_GPON_PARA_SET, &pon, rtdrv_ponPara_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_parameter_get(rtk_gpon_patameter_type_t type, void *pPara)
{
    rtdrv_ponPara_t pon;
    int32           para_len=0, i;

    for(i=RTK_GPON_PARA_TYPE_ONU_ACTIVATION; i<RTK_GPON_PARA_TYPE_MAX; i++)
    {
        if(type == i)
            para_len = pon_len[i].len;
    }
    if(0 == para_len)
        return RT_ERR_OUT_OF_RANGE;

    pon.type = type;

    GETSOCKOPT(RTDRV_GPON_PARA_GET, &pon, rtdrv_ponPara_t, 1);

    osal_memcpy(pPara, &pon.para, para_len);

    return RT_ERR_OK;
}

int32 rtk_gpon_activate(rtk_gpon_initialState_t initState)
{
    SETSOCKOPT(RTDRV_GPON_ACTIVE, &initState, rtk_gpon_initialState_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_deActivate(void)
{
    int32 dummy;
    SETSOCKOPT(RTDRV_GPON_DEACTIVE, &dummy, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_ponStatus_get(rtk_gpon_fsm_status_t *pStatus)
{
    GETSOCKOPT(RTDRV_GPON_STATE_GET, pStatus, rtk_gpon_fsm_status_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_tcont_create(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr)
{
    rtdrv_tcont_t tcont;

    tcont.ind.alloc_id = pInd->alloc_id;
    tcont.ind.type = pInd->type;

    SETSOCKOPT(RTDRV_GPON_TCONT_ADD, &tcont, rtdrv_tcont_t, 1);
    pAttr->tcont_id = tcont.attr.tcont_id;

    return RT_ERR_OK;
}

int32 rtk_gpon_tcont_destroy(rtk_gpon_tcont_ind_t *pInd)
{
    rtdrv_tcont_t tcont;

    tcont.ind.alloc_id = pInd->alloc_id;
    tcont.ind.type = pInd->type;

    SETSOCKOPT(RTDRV_GPON_TCONT_DEL, &tcont, rtdrv_tcont_t, 1);

    return RT_ERR_OK;
}

int32 rtk_gpon_tcont_get(rtk_gpon_tcont_ind_t *pInd, rtk_gpon_tcont_attr_t *pAttr)
{
    rtdrv_tcont_t tcont;

    tcont.ind.alloc_id = pInd->alloc_id;
    tcont.ind.type = pInd->type;

    GETSOCKOPT(RTDRV_GPON_TCONT_GET, &tcont, rtdrv_tcont_t, 1);
    pAttr->tcont_id = tcont.attr.tcont_id;

    return RT_ERR_OK;
}

int32 rtk_gpon_dsFlow_set(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr)
{
    rtdrv_dsFlow_t flow;

    flow.flowId = flowId;
    osal_memcpy(&flow.attr, pAttr, sizeof(rtk_gpon_dsFlow_attr_t));

    SETSOCKOPT(RTDRV_GPON_DS_FLOW_ADD, &flow, rtdrv_dsFlow_t, 1);

    return RT_ERR_OK;
}

int32 rtk_gpon_dsFlow_get(uint32 flowId, rtk_gpon_dsFlow_attr_t *pAttr)
{
    rtdrv_dsFlow_t flow;

    flow.flowId = flowId;

    GETSOCKOPT(RTDRV_GPON_DS_FLOW_GET, &flow, rtdrv_dsFlow_t, 1);
    osal_memcpy(pAttr, &flow.attr, sizeof(rtk_gpon_dsFlow_attr_t));

    return RT_ERR_OK;
}

int32 rtk_gpon_usFlow_set(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr)
{
    rtdrv_usFlow_t flow;

    flow.flowId = flowId;
    osal_memcpy(&flow.attr, pAttr, sizeof(rtk_gpon_usFlow_attr_t));

    SETSOCKOPT(RTDRV_GPON_US_FLOW_ADD, &flow, rtdrv_usFlow_t, 1);

    return RT_ERR_OK;
}

int32 rtk_gpon_usFlow_get(uint32 flowId, rtk_gpon_usFlow_attr_t *pAttr)
{
    rtdrv_usFlow_t flow;

    flow.flowId = flowId;

    GETSOCKOPT(RTDRV_GPON_US_FLOW_GET, &flow, rtdrv_usFlow_t, 1);
    osal_memcpy(pAttr, &flow.attr, sizeof(rtk_gpon_usFlow_attr_t));

    return RT_ERR_OK;
}

int32 rtk_gpon_broadcastPass_set(int32 mode)
{
    SETSOCKOPT(RTDRV_GPON_BC_PASS_SET, &mode, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_broadcastPass_get(int32 *pMode)
{
    GETSOCKOPT(RTDRV_GPON_BC_PASS_GET, pMode, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_nonMcastPass_set(int32 mode)
{
    SETSOCKOPT(RTDRV_GPON_NON_MC_PASS_SET, &mode, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_nonMcastPass_get(int32 *pMode)
{
    GETSOCKOPT(RTDRV_GPON_NON_MC_PASS_GET, pMode, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_multicastAddrCheck_set(uint32 ipv4_pattern, uint32 ipv6_pattern)
{
    rtdrv_ip_ptn_t ptn;

    ptn.ipv4_pattern = ipv4_pattern;
    ptn.ipv6_pattern = ipv6_pattern;

    SETSOCKOPT(RTDRV_GPON_IP_PTN_SET, &ptn, rtdrv_ip_ptn_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_multicastAddrCheck_get(uint32 *pIpv4_Pattern, uint32 *pIpv6_Pattern)
{
    rtdrv_ip_ptn_t ptn;

    GETSOCKOPT(RTDRV_GPON_IP_PTN_GET, &ptn, rtdrv_ip_ptn_t, 1);
    *pIpv4_Pattern = ptn.ipv4_pattern;
    *pIpv6_Pattern = ptn.ipv6_pattern;

    return RT_ERR_OK;
}

int32 rtk_gpon_macFilterMode_set(rtk_gpon_macTable_exclude_mode_t mode)
{
    SETSOCKOPT(RTDRV_GPON_FLT_MODE_SET, &mode, rtk_gpon_macTable_exclude_mode_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_macFilterMode_get(rtk_gpon_macTable_exclude_mode_t *pMode)
{
    GETSOCKOPT(RTDRV_GPON_FLT_MODE_GET, pMode, rtk_gpon_macTable_exclude_mode_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_mcForceMode_set(rtk_gpon_mc_force_mode_t ipv4, rtk_gpon_mc_force_mode_t ipv6)
{
    rtdrv_fs_mode_t mode;

    mode.ipv4 = ipv4;
    mode.ipv6 = ipv6;

    SETSOCKOPT(RTDRV_GPON_FS_MODE_SET, &mode, rtdrv_fs_mode_t, 1);
    return RT_ERR_OK;
}
int32 rtk_gpon_mcForceMode_get(rtk_gpon_mc_force_mode_t *pIpv4, rtk_gpon_mc_force_mode_t *pIpv6)
{
    rtdrv_fs_mode_t mode;

    GETSOCKOPT(RTDRV_GPON_FS_MODE_GET, &mode, rtdrv_fs_mode_t, 1);
    *pIpv4 = mode.ipv4;
    *pIpv6 = mode.ipv6;

    return RT_ERR_OK;
}

int32 rtk_gpon_macEntry_add(rtk_gpon_macTable_entry_t *pEntry)
{
    SETSOCKOPT(RTDRV_GPON_MAC_ENTRY_ADD, pEntry, rtk_gpon_macTable_entry_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_macEntry_del(rtk_gpon_macTable_entry_t *pEntry)
{
    SETSOCKOPT(RTDRV_GPON_MAC_ENTRY_DEL, pEntry, rtk_gpon_macTable_entry_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_macEntry_get(uint32 index, rtk_gpon_macTable_entry_t *pEntry)
{
    rtdrv_mac_entry_t mac_entry;

    mac_entry.index = index;
    GETSOCKOPT(RTDRV_GPON_MAC_ENTRY_GET, &mac_entry, rtdrv_mac_entry_t, 1);
    osal_memcpy(pEntry, &mac_entry.entry, sizeof(rtk_gpon_macTable_entry_t));

    return RT_ERR_OK;
}

int32 rtk_gpon_rdi_set(int32 enable)
{
    SETSOCKOPT(RTDRV_GPON_RDI_SET, &enable, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_rdi_get(int32 *pEnable)
{
    GETSOCKOPT(RTDRV_GPON_RDI_GET, pEnable, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_powerLevel_set(uint8 level)
{
    int32 data = level;

    SETSOCKOPT(RTDRV_GPON_RDI_SET, &data, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_powerLevel_get(uint8 *pLevel)
{
    int32 data;

    GETSOCKOPT(RTDRV_GPON_PWR_LEVEL_GET, &data, int32, 1);
    *pLevel = (uint8)data;

    return RT_ERR_OK;
}

int32 rtk_gpon_alarmStatus_get(rtk_gpon_alarm_type_t alarm, int32 *status)
{
    rtdrv_ponAlarm_t alarm_data;

    alarm_data.type = alarm;

    GETSOCKOPT(RTDRV_GPON_ALARM_GET, &alarm_data, rtdrv_ponAlarm_t, 1);

    *status = alarm_data.status;

    return RT_ERR_OK;
}

int32 rtk_gpon_txForceLaser_set(rtk_gpon_laser_status_t status)
{
    SETSOCKOPT(RTDRV_GPON_TX_LASER_SET, &status, rtk_gpon_laser_status_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_txForceLaser_get(rtk_gpon_laser_status_t *pStatus)
{
    GETSOCKOPT(RTDRV_GPON_TX_LASER_GET, pStatus, rtk_gpon_laser_status_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_txForceIdle_set(int32 on)
{
    SETSOCKOPT(RTDRV_GPON_FS_IDLE_SET, &on, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_txForceIdle_get(int32 *pOn)
{
    GETSOCKOPT(RTDRV_GPON_FS_IDLE_GET, pOn, int32, 1);
    return RT_ERR_OK;
}

#if 0
int32 rtk_gpon_txForcePRBS_set(int32 on)
{
    SETSOCKOPT(RTDRV_GPON_FS_PRBS_SET, &on, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_txForcePRBS_get(int32* pOn)
{
    GETSOCKOPT(RTDRV_GPON_FS_PRBS_GET, pOn, int32, 1);
    return RT_ERR_OK;
}
#endif

int32 rtk_gpon_dsFecSts_get(int32* en)
{
    GETSOCKOPT(RTDRV_GPON_DS_FEC_GET, en, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_version_show(void)
{
    int32 dummy;
    GETSOCKOPT(RTDRV_GPON_VERSION_SHOW, &dummy, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_devInfo_show(void)
{
    int32 dummy;
    GETSOCKOPT(RTDRV_GPON_DEV_SHOW, &dummy, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_gtc_show(void)
{
    int32 dummy;
    GETSOCKOPT(RTDRV_GPON_GTC_SHOW, &dummy, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_tcont_show(uint32 tcont) /* 0xFFFF means show all tcont */
{
    GETSOCKOPT(RTDRV_GPON_TCONT_SHOW, &tcont, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_dsFlow_show(uint32 flow) /* 0xFFFF means show all ds flow */
{
    GETSOCKOPT(RTDRV_GPON_DS_FLOW_SHOW, &flow, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_usFlow_show(uint32 flow) /* 0xFFFF means show all us flow */
{
    GETSOCKOPT(RTDRV_GPON_US_FLOW_SHOW, &flow, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_macTable_show(void)
{
    int32 dummy;
    GETSOCKOPT(RTDRV_GPON_MAC_TABLE_SHOW, &dummy, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_globalCounter_show(rtk_gpon_global_performance_type_t type)
{
    GETSOCKOPT(RTDRV_GPON_GLB_CNT_SHOW, &type, rtk_gpon_global_performance_type_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_tcontCounter_show(uint32 idx, rtk_gpon_tcont_performance_type_t type)
{
    rtdrv_tcont_cnt_t cnt;

    cnt.tcont = idx;
    cnt.cnt_type = type;

    GETSOCKOPT(RTDRV_GPON_TCONT_CNT_SHOW, &cnt, rtdrv_tcont_cnt_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_flowCounter_show(uint32 idx, rtk_gpon_flow_performance_type_t type)
{
    rtdrv_flow_cnt_t cnt;

    cnt.flow = idx;
    cnt.cnt_type = type;

    GETSOCKOPT(RTDRV_GPON_FLOW_CNT_SHOW, &cnt, rtdrv_flow_cnt_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_test_set(uint32 data)
{
    SETSOCKOPT(RTDRV_GPON_TEST_SET, &data, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_test_get(uint32 *pData)
{
    GETSOCKOPT(RTDRV_GPON_TEST_GET, pData, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_initial(uint32 data)
{
    SETSOCKOPT(RTDRV_GPON_INITIAL, &data, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_deinitial(void)
{
    int32 dummy;

    SETSOCKOPT(RTDRV_GPON_DEINITIAL, &dummy, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_debug_set(int32 enable)
{
    SETSOCKOPT(RTDRV_GPON_DEBUG_SET, &enable, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_unit_test(uint32 id)
{
    SETSOCKOPT(RTDRV_GPON_UNIT_TEST, &id, uint32, 1);
    return RT_ERR_OK;
}

#if defined(OLD_FPGA_DEFINED)
int32 rtk_gpon_pktGen_buf_set(uint32 item, uint8 *buf, uint32 len)
{
    rtdrv_pktGen_t pkt;

    pkt.item = item;
    pkt.tcont = 0;
    pkt.gem = 0;
    pkt.omci = 0;
    pkt.buf_len = len;
    osal_memcpy(pkt.buf, buf, len);

    SETSOCKOPT(RTDRV_GPON_PKTGEN_BUF, &pkt, rtdrv_pktGen_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_pktGen_cfg_set(uint32 item, uint32 tcont, uint32 len, uint32 gem, int32 omci)
{
    rtdrv_pktGen_t pkt;

    pkt.item = item;
    pkt.tcont = tcont;
    pkt.gem = gem;
    pkt.omci = omci;
    pkt.buf_len = len;

    SETSOCKOPT(RTDRV_GPON_PKTGEN_CFG, &pkt, rtdrv_pktGen_t, 1);
    return RT_ERR_OK;
}
#endif

int32 rtk_gpon_omci_tx(rtk_gpon_omci_msg_t *omci)
{
    SETSOCKOPT(RTDRV_GPON_OMCI_TX, omci, rtk_gpon_omci_msg_t, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_autoTcont_set(int32 state)
{
    SETSOCKOPT(RTDRV_GPON_AUTO_TCONT_SET, &state, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_autoTcont_get(int32 *pState)
{
    GETSOCKOPT(RTDRV_GPON_AUTO_TCONT_GET, pState, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_autoBoh_set(int32 state)
{
    SETSOCKOPT(RTDRV_GPON_AUTO_BOH_SET, &state, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_autoBoh_get(int32 *pState)
{
    GETSOCKOPT(RTDRV_GPON_AUTO_BOH_GET, pState, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_eqdOffset_set(int32 offset)
{
    SETSOCKOPT(RTDRV_GPON_EQD_OFFSET_SET, &offset, int32, 1);
    return RT_ERR_OK;
}

int32 rtk_gpon_eqdOffset_get(int32 *pOffset)
{
    GETSOCKOPT(RTDRV_GPON_EQD_OFFSET_GET, pOffset, int32, 1);
    return RT_ERR_OK;
}


