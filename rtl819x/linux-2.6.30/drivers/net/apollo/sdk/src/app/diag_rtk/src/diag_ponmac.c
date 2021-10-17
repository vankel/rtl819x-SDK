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
 * Purpose : Definition those PON MAC command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           POn MAC
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
#include <rtk/ponmac.h>


#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_ponmac.h>
#endif

#ifdef CONFIG_SDK_APOLLO
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <dal/apollo/raw/apollo_raw_ponmac.h>
#endif


static rtk_ponmac_queueCfg_t  globalQueueCfg;
static rtk_ponmac_queue_t     globalQueue;


/*
 * pon init
 */
cparser_result_t
cparser_cmd_pon_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    /*init pon module*/
    DIAG_UTIL_ERR_CHK(rtk_ponmac_init(), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_pon_init */


/*
 * pon clear
 */
cparser_result_t
cparser_cmd_pon_clear(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    /*clear queue config global setting*/
    memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
    memset(&globalQueue,0x0, sizeof(rtk_ponmac_queue_t));

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_clear */


/*
 * pon set drain-out t-cont <UINT:tcont> queue-id <MASK_LIST:qid>
 */
cparser_result_t
cparser_cmd_pon_set_drain_out_t_cont_tcont_queue_id_qid(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    char * *qid_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 queueId;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(mask, 6), ret);
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        queueId = ((*tcont_ptr)/8) * 32 + index;

        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_ponMacQueueDrainOutState_set(queueId), ret);
                break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_ponMacQueueDrainOutState_set(queueId), ret);
                break;
#endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_drain_out_t_cont_tcont_queue_id_qid */

/*
 * pon get drain-out status
 */
cparser_result_t
cparser_cmd_pon_get_drain_out_status(
    cparser_context_t *context)
{
    uint32 state;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_ponMacQueueDrainOutState_get((apollo_raw_ponmac_draintOutState_t *)&state), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_ponMacQueueDrainOutState_get((apollomp_raw_ponmac_draintOutState_t *)&state), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    diag_util_mprintf("drant out state:%d\n",state);

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_get_drain_out_status */

/*
 * pon get stream <MASK_LIST:sid>
 */
cparser_result_t
cparser_cmd_pon_get_stream_sid(
    cparser_context_t *context,
    char * *sid_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 sid;
    rtk_ponmac_queue_t queue;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(mask, 3), ret);
    DIAG_UTIL_MASK_SCAN(mask, sid)
    {
        ret = rtk_ponmac_flow2Queue_get(sid ,&queue);
        if(ret != RT_ERR_OK)
            continue;

        diag_util_mprintf("sid:%3d scheduler id:%3d queue id:%3d\n",sid,queue.schedulerId, queue.queueId);

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_get_stream_sid */

/*
 * pon set stream <MASK_LIST:sid> t-cont <UINT:tcont> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_pon_set_stream_sid_t_cont_tcont_queue_id_qid(
    cparser_context_t *context,
    char * *sid_ptr,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr)
{
    diag_mask_t mask;
    uint32 sid;
    rtk_ponmac_queue_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    queue.schedulerId=*tcont_ptr;
    queue.queueId=*qid_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(mask, 3), ret);
    DIAG_UTIL_MASK_SCAN(mask, sid)
    {
        DIAG_UTIL_ERR_CHK(rtk_ponmac_flow2Queue_set(sid,&queue), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_stream_sid_t_cont_tcont_queue_id_qid */

/*
 * pon set stream <MASK_LIST:sid> llid <UINT:llid> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_pon_set_stream_sid_llid_llid_queue_id_qid(
    cparser_context_t *context,
    char * *sid_ptr,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr)
{
    diag_mask_t mask;
    uint32 sid;
    rtk_ponmac_queue_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    queue.schedulerId=*llid_ptr;
    queue.queueId=*qid_ptr;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK128(mask, 3), ret);
    DIAG_UTIL_MASK_SCAN(mask, sid)
    {
        DIAG_UTIL_ERR_CHK(rtk_ponmac_flow2Queue_set(sid,&queue), ret);
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_stream_sid_llid_llid_queue_id_qid */

/*
 * pon get t-cont <UINT:tcont> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_pon_get_t_cont_tcont_queue_id_qid(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr)
{
    rtk_ponmac_queue_t queue;
    int32 ret = RT_ERR_FAILED;
    rtk_ponmac_queueCfg_t  queueCfg;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    queue.schedulerId=*tcont_ptr;
    queue.queueId=*qid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ponmac_queue_get(&queue,&queueCfg), ret);

    memcpy(&globalQueue,&queue, sizeof(rtk_ponmac_queue_t));
    memcpy(&globalQueueCfg,&queueCfg, sizeof(rtk_ponmac_queueCfg_t));


    diag_util_mprintf("CIR:%d\n",queueCfg.cir);
    diag_util_mprintf("PIR:%d\n",queueCfg.pir);
    diag_util_mprintf("queue Type:%s\n",diagStr_queueType[queueCfg.type]);
    diag_util_mprintf("WFQ weight:%d\n",queueCfg.weight);
    diag_util_mprintf("Egress Drop:%s\n",diagStr_enable[queueCfg.egrssDrop]);

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_get_t_cont_tcont_queue_id_qid */

/*
 * pon add t-cont <UINT:tcont> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_pon_add_t_cont_tcont_queue_id_qid(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    
   
    if(globalQueue.schedulerId != *tcont_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_ponmac_queue_add(&globalQueue,&globalQueueCfg), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_pon_add_t_cont_tcont_queue_id_qid */

/*
 * pon del t-cont <UINT:tcont> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_pon_del_t_cont_tcont_queue_id_qid(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr)
{
    rtk_ponmac_queue_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    queue.schedulerId=*tcont_ptr;
    queue.queueId=*qid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ponmac_queue_del(&queue), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_del_t_cont_tcont_queue_id_qid */

/*
 * pon set t-cont <UINT:tcont> queue-id <UINT:qid> pir rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_pir_rate_rate(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *tcont_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *tcont_ptr;
        globalQueue.queueId = *qid_ptr;
    }

    globalQueueCfg.pir = *rate_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_pir_rate_rate */

/*
 * pon set t-cont <UINT:tcont> queue-id <UINT:qid> cir rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_cir_rate_rate(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *tcont_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *tcont_ptr;
        globalQueue.queueId = *qid_ptr;
    }

    globalQueueCfg.cir = *rate_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_cir_rate_rate */

/*
 * pon set t-cont <UINT:tcont> queue-id <UINT:qid> scheduling type ( strict | wfq )
 */
cparser_result_t
cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_scheduling_type_strict_wfq(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *tcont_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *tcont_ptr;
        globalQueue.queueId = *qid_ptr;
    }

    if('s'==TOKEN_CHAR(8,0))
        globalQueueCfg.type = STRICT_PRIORITY;
    else
        globalQueueCfg.type = WFQ_WRR_PRIORITY;

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_scheduling_type_strict_wfq */

/*
 * pon set t-cont <UINT:tcont> queue-id <UINT:qid> scheduling weight <UINT:weight>
 */
cparser_result_t
cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_scheduling_weight_weight(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *tcont_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *tcont_ptr;
        globalQueue.queueId = *qid_ptr;
    }
    globalQueueCfg.weight = *weight_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_scheduling_weight_weight */

/*
 * pon set t-cont <UINT:tcont> queue-id <UINT:qid> egress-drop state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_egress_drop_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *tcont_ptr,
    uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *tcont_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *tcont_ptr;
        globalQueue.queueId = *qid_ptr;
    }

    if('e'==TOKEN_CHAR(8,0))
        globalQueueCfg.egrssDrop = ENABLED;
    else
        globalQueueCfg.egrssDrop = DISABLED;

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_t_cont_tcont_queue_id_qid_egress_drop_state_enable_disable */

/*
 * pon get llid <UINT:llid> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_pon_get_llid_llid_queue_id_qid(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr)
{
    rtk_ponmac_queue_t queue;
    int32 ret = RT_ERR_FAILED;
    rtk_ponmac_queueCfg_t  queueCfg;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    queue.schedulerId=*llid_ptr;
    queue.queueId=*qid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ponmac_queue_get(&queue,&queueCfg), ret);

    memcpy(&globalQueue,&queue, sizeof(rtk_ponmac_queue_t));
    memcpy(&globalQueueCfg,&queueCfg, sizeof(rtk_ponmac_queueCfg_t));


    diag_util_mprintf("CIR:%d\n",queueCfg.cir);
    diag_util_mprintf("PIR:%d\n",queueCfg.pir);
    diag_util_mprintf("queue Type:%s\n",diagStr_queueType[queueCfg.type]);
    diag_util_mprintf("WFQ weight:%d\n",queueCfg.weight);
    diag_util_mprintf("Egress Drop:%s\n",diagStr_enable[queueCfg.egrssDrop]);

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_get_llid_llid_queue_id_qid */

/*
 * pon add llid <UINT:llid> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_pon_add_llid_llid_queue_id_qid(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *llid_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(rtk_ponmac_queue_add(&globalQueue,&globalQueueCfg), ret);


    return CPARSER_OK;
}    /* end of cparser_cmd_pon_add_llid_llid_queue_id_qid */

/*
 * pon del llid <UINT:llid> queue-id <UINT:qid>
 */
cparser_result_t
cparser_cmd_pon_del_llid_llid_queue_id_qid(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr)
{
    rtk_ponmac_queue_t queue;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    queue.schedulerId=*llid_ptr;
    queue.queueId=*qid_ptr;

    DIAG_UTIL_ERR_CHK(rtk_ponmac_queue_del(&queue), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_del_llid_llid_queue_id_qid */

/*
 * pon set llid <UINT:llid> queue-id <UINT:qid> pir rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_pon_set_llid_llid_queue_id_qid_pir_rate_rate(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *llid_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *llid_ptr;
        globalQueue.queueId = *qid_ptr;
    }

    globalQueueCfg.pir = *rate_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_llid_llid_queue_id_qid_pir_rate_rate */

/*
 * pon set llid <UINT:llid> queue-id <UINT:qid> cir rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_pon_set_llid_llid_queue_id_qid_cir_rate_rate(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *llid_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *llid_ptr;
        globalQueue.queueId = *qid_ptr;
    }

    globalQueueCfg.cir = *rate_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_llid_llid_queue_id_qid_cir_rate_rate */

/*
 * pon set llid <UINT:llid> queue-id <UINT:qid> scheduling type ( strict | wfq )
 */
cparser_result_t
cparser_cmd_pon_set_llid_llid_queue_id_qid_scheduling_type_strict_wfq(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *llid_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *llid_ptr;
        globalQueue.queueId = *qid_ptr;
    }

    if('s'==TOKEN_CHAR(8,0))
        globalQueueCfg.type = STRICT_PRIORITY;
    else
        globalQueueCfg.type = WFQ_WRR_PRIORITY;

    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_llid_llid_queue_id_qid_scheduling_type_strict_wfq */

/*
 * pon set llid <UINT:llid> queue-id <UINT:qid> scheduling weight <UINT:weight>
 */
cparser_result_t
cparser_cmd_pon_set_llid_llid_queue_id_qid_scheduling_weight_weight(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *weight_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *llid_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *llid_ptr;
        globalQueue.queueId = *qid_ptr;
    }
    globalQueueCfg.weight = *weight_ptr;
    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_llid_llid_queue_id_qid_scheduling_weight_weight */

/*
 * pon set llid <UINT:llid> queue-id <UINT:qid> egress-drop state ( enable | disable )
 */
cparser_result_t
cparser_cmd_pon_set_llid_llid_queue_id_qid_egress_drop_state_enable_disable(
    cparser_context_t *context,
    uint32_t  *llid_ptr,
    uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if(globalQueue.schedulerId != *llid_ptr ||  globalQueue.queueId != *qid_ptr)
    {
        memset(&globalQueueCfg,0x0, sizeof(rtk_ponmac_queueCfg_t));
        globalQueue.schedulerId = *llid_ptr;
        globalQueue.queueId = *qid_ptr;
    }

    if('e'==TOKEN_CHAR(8,0))
        globalQueueCfg.egrssDrop = ENABLED;
    else
        globalQueueCfg.egrssDrop = DISABLED;
    return CPARSER_OK;
}    /* end of cparser_cmd_pon_set_llid_llid_queue_id_qid_egress_drop_state_enable_disable */

