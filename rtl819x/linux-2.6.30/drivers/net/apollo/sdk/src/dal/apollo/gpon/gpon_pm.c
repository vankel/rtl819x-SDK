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
 *
 *
 * $Revision:  $
 * $Date: 2011-04-19 $
 *
 * Purpose : GPON MAC Driver PM implement
 *
 * Feature : Provide the PM conunters
 *
 */
#include <dal/apollo/gpon/gpon_defs.h>
#include <dal/apollo/raw/apollo_raw_gpon.h>
#include <dal/apollo/gpon/gpon_pm.h>

#define GPON_PM_GETCOUNTER_U16B_FROM_U32B_HIGH(u32bit)  ((uint16)((u32bit>>16)&0xFFFF))
#define GPON_PM_GETCOUNTER_U16B_FROM_U32B_LOW(u32bit)   ((uint16)((u32bit>>0)&0xFFFF))

int32 gpon_pm_onuActivation_get (gpon_dev_obj_t* obj,  rtk_gpon_onu_activation_counter_t* counter )
{
    uint32 tmp;

    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_ACTIVE, &tmp);
    counter->rx_sn_req = (tmp&0xFFFF);
    counter->rx_ranging_req = ((tmp>>16)&0xFFFF);

    return RT_ERR_OK;
}

int32 gpon_pm_dsPhy_get (gpon_dev_obj_t* obj,  rtk_gpon_ds_physical_counter_t* counter )
{
    uint32 tmp;

    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_BIP_BLOCK_ERR, &counter->rx_bip_err_block);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_BIP_BITS_ERR, &counter->rx_bip_err_bit);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BITS, &counter->rx_fec_correct_bit);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_FEC_CORRECTED_BYTES, &counter->rx_fec_correct_byte);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_FEC_CORRETED_CWS, &counter->rx_fec_correct_cw);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_FEC_UNCOR_CWS, &counter->rx_fec_uncor_cw);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_LOM, &tmp);
    counter->rx_lom = (tmp&0xFFFF);
    counter->rx_plen_err = ((tmp>>16)&0xFFFF);

    return RT_ERR_OK;
}

int32 gpon_pm_dsPloam_get (gpon_dev_obj_t* obj,  rtk_gpon_ds_ploam_counter_t* counter )
{
    uint32 tmp;
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;


    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_PLOAM_ACCEPT, &counter->rx_ploam_cnt);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_PLEND_CORRECTIONS, &counter->rx_ploam_correctted);

    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_PLOAM_FAIL, &tmp);
    counter->rx_ploam_err = (tmp&0xFFFF);
    counter->rx_ploam_overflow = ((tmp>>16)&0xFFFF);
    counter->rx_ploam_proc = obj->cnt_cpu_ploam_rx;
    obj->cnt_cpu_ploam_rx = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_dsBwMap_get (gpon_dev_obj_t* obj,  rtk_gpon_ds_bwMap_counter_t* counter)
{
    uint32 tmp;

    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_BWMAP_FAIL, &tmp);
    counter->rx_bwmap_crc_err = (tmp&0xFFFF);
    counter->rx_bwmap_overflow = ((tmp>>16)&0xFFFF);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_BWMAP_INV, &tmp);
    counter->rx_bwmap_inv0 = (tmp&0xFFFF);
    counter->rx_bwmap_inv1 = ((tmp>>16)&0xFFFF);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_BWMAP_ACPT, &counter->rx_bwmap_cnt);

    return RT_ERR_OK;
}

int32 gpon_pm_dsGem_get (gpon_dev_obj_t* obj,  rtk_gpon_ds_gem_counter_t* counter )
{
    uint32 tmp;

    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_GEM_LOS, &counter->rx_gem_los);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_HEC_COR, &counter->rx_hec_correct);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_GEM_IDLE, &counter->rx_gem_idle);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_RX_GEM_NON_IDLE, &counter->rx_gem_non_idle);
    apollo_raw_gpon_dsGem_miscCnt_read(GPON_REG_DSGEMPMMISC_OVER_INTERLEAV, &counter->rx_over_interleaving);
    apollo_raw_gpon_dsGtc_miscCnt_read(GPON_REG_DSGTCPMMISC_GEM_FAIL, &tmp);
    counter->rx_gem_len_mis = (tmp&0xFFFF);
    counter->rx_match_multi_flow = ((tmp>>16)&0xFFFF);

    counter->rx_chunk_overflow = 0;
    counter->rx_chunk_fwd = 0;
    counter->rx_chunk_head = 0;
    counter->rx_chunk_body = 0;
    counter->rx_chunk_last = 0;
    counter->rx_chunk_self = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_dsEth_get (gpon_dev_obj_t* obj,  rtk_gpon_ds_eth_counter_t* counter )
{

    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

     apollo_raw_gpon_dsGem_miscCnt_read(GPON_REG_DSGEMPMMISC_MULTICAST_RX, &counter->rx_eth_multicast);
     apollo_raw_gpon_dsGem_miscCnt_read(GPON_REG_DSGEMPMMISC_UNICAST_RX, &counter->rx_eth_unicast);
     apollo_raw_gpon_dsGem_miscCnt_read(GPON_REG_DSGEMPMMISC_MULTICAST_FWD, &counter->rx_eth_multicast_fwd);
     apollo_raw_gpon_dsGem_miscCnt_read(GPON_REG_DSGEMPMMISC_MULTICAST_LEAK, &counter->rx_eth_multicast_leak);
     apollo_raw_gpon_dsGem_miscCnt_read(GPON_REG_DSGEMPMMISC_ETH_CRC_ERR, &counter->rx_eth_fcs_err);

     return RT_ERR_OK;
}

int32 gpon_pm_dsOmci_get (gpon_dev_obj_t* obj,  rtk_gpon_ds_omci_counter_t* counter )
{

    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    apollo_raw_gpon_dsGem_miscCnt_read(GPON_REG_DSGEMPMMISC_OMCI_RX, &counter->rx_omci);
    counter->rx_omci_crc_err = 0;
    counter->rx_omci_drop = 0;
    counter->rx_omci_proc = obj->cnt_cpu_omci_rx;
    obj->cnt_cpu_omci_rx = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_usPhy_get (gpon_dev_obj_t* obj,  rtk_gpon_us_physical_counter_t* counter )
{
    uint32 tmp;
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    apollo_raw_gpon_usGtc_miscCnt_read(GPON_REG_USGTCPMMISC_PLOAM_BOH_TX, &tmp);
    obj->cnt_boh_tx += GPON_PM_GETCOUNTER_U16B_FROM_U32B_LOW(tmp);
    obj->cnt_ploamu_tx += GPON_PM_GETCOUNTER_U16B_FROM_U32B_HIGH(tmp);

    counter->tx_boh_cnt = obj->cnt_boh_tx;
    obj->cnt_boh_tx = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_usDbr_get (gpon_dev_obj_t* obj,  rtk_gpon_us_dbr_counter_t* counter )
{
    uint32 tmp;
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;


    apollo_raw_gpon_usGtc_miscCnt_read(GPON_REG_USGTCPMMISC_GEM_DBRU_TX,&tmp);
    obj->cnt_dbru_tx += GPON_PM_GETCOUNTER_U16B_FROM_U32B_LOW(tmp);
    obj->cnt_gem_tx += GPON_PM_GETCOUNTER_U16B_FROM_U32B_HIGH(tmp);

    counter->tx_dbru_cnt = obj->cnt_dbru_tx;
    obj->cnt_dbru_tx = 0;

    counter->tx_dbru_flag_err = 0;
    counter->tx_dbru_seq_err = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_usPloam_get (gpon_dev_obj_t* obj,  rtk_gpon_us_ploam_counter_t* counter )
{
    uint32 tmp;
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    apollo_raw_gpon_usGtc_miscCnt_read(GPON_REG_USGTCPMMISC_PLOAM_BOH_TX,&tmp);
    obj->cnt_boh_tx += GPON_PM_GETCOUNTER_U16B_FROM_U32B_LOW(tmp);
    obj->cnt_ploamu_tx += GPON_PM_GETCOUNTER_U16B_FROM_U32B_HIGH(tmp);

    counter->tx_ploam_cnt = obj->cnt_ploamu_tx;
    obj->cnt_ploamu_tx = 0;

    apollo_raw_gpon_usGtc_miscCnt_read(GPON_REG_USGTCPMMISC_PLOAM_CPU_TX,&tmp);
    counter->tx_ploam_nor = GPON_PM_GETCOUNTER_U16B_FROM_U32B_LOW(tmp);
    counter->tx_ploam_urg = GPON_PM_GETCOUNTER_U16B_FROM_U32B_HIGH(tmp);

    apollo_raw_gpon_usGtc_miscCnt_read(GPON_REG_USGTCPMMISC_PLOAM_AUTO_TX,&tmp);
    counter->tx_ploam_nomsg = GPON_PM_GETCOUNTER_U16B_FROM_U32B_LOW(tmp);
    counter->tx_ploam_sn = GPON_PM_GETCOUNTER_U16B_FROM_U32B_HIGH(tmp);

    counter->tx_ploam_proc = obj->cnt_cpu_ploam_tx;
    obj->cnt_cpu_ploam_tx = 0;
    counter->tx_ploam_urg_proc = obj->cnt_cpu_ploam_tx_urg;
    obj->cnt_cpu_ploam_tx_urg = 0;
    counter->tx_ploam_nor_proc = obj->cnt_cpu_ploam_tx_nor;
    obj->cnt_cpu_ploam_tx_nor = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_usGem_get (gpon_dev_obj_t* obj,  rtk_gpon_us_gem_counter_t* counter )
{
    uint32 tmp;
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    apollo_raw_gpon_usGtc_miscCnt_read(GPON_REG_USGTCPMMISC_GEM_DBRU_TX,&tmp);
    obj->cnt_dbru_tx += GPON_PM_GETCOUNTER_U16B_FROM_U32B_LOW(tmp);
    obj->cnt_gem_tx += GPON_PM_GETCOUNTER_U16B_FROM_U32B_HIGH(tmp);

    counter->tx_gem_cnt = obj->cnt_gem_tx;
    obj->cnt_gem_tx = 0;

    apollo_raw_gpon_usGtc_miscCnt_read(GPON_REG_USGTCPMMISC_GEM_BYTE_TX,&counter->tx_gem_byte);
    counter->tx_gem_idle = 0;
    counter->tx_gem_pkt = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_usEth_get (gpon_dev_obj_t* obj,  rtk_gpon_us_eth_counter_t* counter )
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;


    counter->tx_eth_abort_ebb = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_usOmci_get (gpon_dev_obj_t* obj,  rtk_gpon_us_omci_counter_t* counter )
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    counter->tx_omci_urg = 0;
    counter->tx_omci_nor = 0;
    counter->tx_omci_proc = obj->cnt_cpu_omci_tx;
    obj->cnt_cpu_omci_tx = 0;
    counter->tx_omci_proc_urg = obj->cnt_cpu_omci_tx_urg;
    obj->cnt_cpu_omci_tx_urg = 0;
    counter->tx_omci_proc_nor = obj->cnt_cpu_omci_tx_nor;
    obj->cnt_cpu_omci_tx_nor = 0;

    return RT_ERR_OK;
}

int32 gpon_pm_tcontGem_get (gpon_dev_obj_t* obj, uint32 tcontId, rtk_gpon_tcont_gem_counter_t* counter)
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    if(tcontId);

    counter->gem_pkt = 0;
    return RT_ERR_OK;
}

int32 gpon_pm_tcontEth_get (gpon_dev_obj_t* obj, uint32 tcontId, rtk_gpon_tcont_eth_counter_t* counter )
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    if(tcontId);

    counter->eth_pkt = 0;
    return RT_ERR_OK;
}

int32 gpon_pm_tcontIdle_get (gpon_dev_obj_t* obj, uint32 tcontId, rtk_gpon_tcont_idle_counter_t* counter )
{
    if(NULL == obj)
        return RT_ERR_NULL_POINTER;

    if(RT_ERR_OK != apollo_raw_gpon_gemUs_idleByteCnt_read(tcontId, &(counter->idle_byte)))
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"Read Counter %s (tcont %d) Fail","RX IDLE Byte",tcontId);
        counter->idle_byte = 0;
    }

    return RT_ERR_OK;
}

int32 gpon_pm_dsFlowGem_get (gpon_dev_obj_t* obj, uint32 flowId, rtk_gpon_dsFlow_gem_counter_t* counter )
{
    if(obj);

    if (RT_ERR_OK != apollo_raw_gpon_dsPort_pktCnt_read(flowId, &(counter->gem_block)))
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"Read Counter %s (flow %d) Fail","RX GEM BLOCK",flowId);
        counter->gem_block = 0;
    }
    if (RT_ERR_OK != apollo_raw_gpon_dsPort_byteCnt_read(flowId, &(counter->gem_byte)))
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"Read Counter %s (flow %d) Fail","RX GEM BYTE",flowId);
        counter->gem_byte = 0;
    }

    return RT_ERR_OK;
}

int32 gpon_pm_dsFlowEth_get (gpon_dev_obj_t* obj, uint32 flowId, rtk_gpon_dsFlow_eth_counter_t* counter )
{
    if(obj);

    if(RT_ERR_OK != apollo_raw_gpon_dsPort_ethRxCnt_read(flowId, &(counter->eth_pkt_rx)))
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"Read Counter %s (flow %d) Fail","RX ETH PKT",flowId);
        counter->eth_pkt_rx = 0;
    }
    if(RT_ERR_OK != apollo_raw_gpon_dsPort_ethFwdCnt_read(flowId, &(counter->eth_pkt_fwd)))
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"Read Counter %s (flow %d) Fail","FWD ETH PKT",flowId);
        counter->eth_pkt_fwd = 0;
    }

    return RT_ERR_OK;
}

int32 gpon_pm_usFlowGem_get (gpon_dev_obj_t* obj, uint32 flowId, rtk_gpon_usFlow_gem_counter_t* counter )
{
    if(obj);

    if (RT_ERR_OK != apollo_raw_gpon_gemUs_dataByteCnt_read(flowId, &(counter->gem_byte)))
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"Read Counter %s (flow %d) Fail","US GEM BYTE",flowId);
        counter->gem_byte = 0;
    }

    if (RT_ERR_OK != apollo_raw_gpon_gemUs_gemCnt_read(flowId, &(counter->gem_block)))
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"Read Counter %s (flow %d) Fail","US GEM BLOCK",flowId);
        counter->gem_block = 0;
    }

    return RT_ERR_OK;
}

int32 gpon_pm_usFlowEth_get (gpon_dev_obj_t* obj, uint32 flowId, rtk_gpon_usFlow_eth_counter_t* counter )
{
    if(obj);
    
    if (RT_ERR_OK != apollo_raw_gpon_gemUs_ethCnt_read(flowId, &(counter->eth_cnt)))
    {
        GPON_OS_Log(GPON_LOG_LEVEL_ERROR,"Read Counter %s (flow %d) Fail","US ETH PKT",flowId);
        counter->eth_cnt = 0;
    }

    return RT_ERR_OK;
}

