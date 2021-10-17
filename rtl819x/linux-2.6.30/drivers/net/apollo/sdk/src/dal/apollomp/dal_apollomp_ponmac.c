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
 * $Revision: 14000 $
 * $Date: 2010-11-08 17:47:25 +0800 (?üÊ?‰∏Ä, 08 ?Å‰???2010) $
 *
 * Purpose : Definition of PON MAC API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) PON mac 
 */

/*
 * Include Files
 */
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_ponmac.h>
#include <dal/apollomp/raw/apollomp_raw_ponmac.h>

/*
 * Symbol Definition
 */



static uint32    ponmac_init = INIT_NOT_COMPLETED; 

/*
 * Macro Declaration
 */


static int32 _ponmac_physicalQueueId_get(uint32 schedulerId, uint32 logicalQId, uint32 *pPhysicalQid)
{
    
#if(GPON_MAC_MODE)    
    *pPhysicalQid = APOLLOMP_TCONT_QUEUE_MAX *(schedulerId/8) + logicalQId;
#else
    *pPhysicalQid = APOLLOMP_TCONT_QUEUE_MAX *(schedulerId/2) + logicalQId;
#endif

    return RT_ERR_OK;        
}


static int32 _ponmac_schedulerQueue_get(uint32 physicalQid, uint32 *pSchedulerId, uint32 *pLogicalQId)
{
    int32   ret;
    uint32  qMask;
    uint32  maxSchedhlerId;
    uint32  schedhlerBase;
    uint32  i;
    
    *pLogicalQId = physicalQid%32;

       
#if(GPON_MAC_MODE)    
    maxSchedhlerId = 8;
    schedhlerBase = (physicalQid/32) * 8;
#else
    maxSchedhlerId = 2;
    schedhlerBase = (physicalQid/32) * 2;
#endif


    for(i = schedhlerBase; i<schedhlerBase+maxSchedhlerId ; i++ )
    {
        if ((ret = apollomp_raw_ponMacScheQmap_get(i, &qMask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        if(qMask & 1<<(*pLogicalQId))
        {
            *pSchedulerId = i;
            return RT_ERR_OK;    
        }
    }
    *pSchedulerId = schedhlerBase;
    return RT_ERR_OK;        
}


/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */


/* Function Name:
 *      dal_apollomp_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_QUEUE_NUM - Invalid queue number
 * Note:
 */
int32
dal_apollomp_ponmac_init(void)
{
    int32   ret;
    uint32 tcont,wData;
    uint32 physicalQid,queueId;
    uint32 flowId;
        
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /*init PON BW_THRESHOLD*/
    wData = 40;
    if ((ret = reg_field_write(APOLLOMP_PON_PORT_CTRLr, APOLLOMP_BW_THRESHOLDf, &wData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }    
   
    for(tcont = 0; tcont <HAL_MAX_NUM_OF_GPON_TCONT(); tcont++ )
    {
        /*disable T-cont schedule */
        if ((ret = apollomp_raw_ponMacTcontEnable_set(tcont, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }        
        /*clear T-cont schedule mask*/
        if ((ret = apollomp_raw_ponMacScheQmap_set(tcont, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }

    
    
    /*set all queue to strict, disable CIR/PIR and disable egress drop*/
    
    for(physicalQid=0 ; physicalQid<HAL_MAX_NUM_OF_PON_QUEUE() ; physicalQid++)
    {
        /*set queue schedule type*/
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_STRICT)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        
        /*set PIR CIR*/            
        if ((ret = apollomp_raw_ponMacCirRate_set(physicalQid, 0x0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    
        if ((ret = apollomp_raw_ponMacPirRate_set(physicalQid, HAL_PONMAC_PIR_CIR_RATE_MAX())) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
        
    
        /*set egress drop*/
        if ((ret = apollomp_raw_ponMac_egrDropState_set(physicalQid, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }    
    }
    /*mapping queue 0~7 to schedule id 0*/
    {
        rtk_ponmac_queueCfg_t   queueCfg;
        rtk_ponmac_queue_t logicalQueue;
        
        memset(&queueCfg, 0, sizeof(rtk_ponmac_queueCfg_t));
        
        logicalQueue.schedulerId = 0 ;
        
        queueCfg.cir       = 0x0; 
        queueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
        queueCfg.type      = STRICT_PRIORITY;
        queueCfg.egrssDrop = DISABLED; 
        ponmac_init = INIT_COMPLETED;
        for(queueId = 0 ; queueId <= RTK_DOT1P_PRIORITY_MAX ; queueId++ )
        {
            logicalQueue.queueId = queueId;
    
            if((ret= dal_apollomp_ponmac_queue_add(&logicalQueue, &queueCfg)) != RT_ERR_OK)
            {
                ponmac_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }
    
    /*mapping all flow to t-cont 0 queue 0*/
    {
        rtk_ponmac_queue_t logicalQueue;
        logicalQueue.schedulerId = 0;
        logicalQueue.queueId     = 0;
        ponmac_init = INIT_COMPLETED;                
        for(flowId=0 ; flowId<HAL_CLASSIFY_SID_NUM() ; flowId++)
        {        
            if((ret= dal_apollomp_ponmac_flow2Queue_set(flowId, &logicalQueue)) != RT_ERR_OK)
            {
                ponmac_init = INIT_NOT_COMPLETED;
                return ret;
            }            
        }
    }
    ponmac_init = INIT_COMPLETED;
       
    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_init */


/* Function Name:
 *      dal_apollomp_ponmac_queue_add
 * Description:
 *      Add queue to given scheduler id and apply queue setting
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 *      pQueueCfg     - queue configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32 
dal_apollomp_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    uint32  qMask;
    uint32  physicalQid;
  
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    RT_PARAM_CHK((pQueueCfg->type >= QUEUE_TYPE_END), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueueCfg->egrssDrop >= RTK_ENABLE_END), RT_ERR_INPUT);
     
    /* add queue to t-cont schedule mask*/
    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if(qMask == 0)
    {/*first queue add to this t-cont enable t-cont schedule*/
        if ((ret = apollomp_raw_ponMacTcontEnable_set(pQueue->schedulerId, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
        
    /*add queue to qMask*/
    qMask = qMask | (1<<pQueue->queueId);
    if ((ret = apollomp_raw_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    /*set Queue id and flow id mapping */
    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);

    /*set queue schedule type*/
    if(pQueueCfg->type == STRICT_PRIORITY)
    {    
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_STRICT)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    else
    {
        if ((ret = apollomp_raw_ponMac_queueType_set(physicalQid, APOLLOMP_QOS_QUEUE_WFQ)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }
    }
    /*set wfq weight*/
    if ((ret = apollomp_raw_ponMac_wfqWeight_set(physicalQid, pQueueCfg->weight)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    /*set PIR CIR*/            
    if ((ret = apollomp_raw_ponMacCirRate_set(physicalQid, pQueueCfg->cir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = apollomp_raw_ponMacPirRate_set(physicalQid, pQueueCfg->pir)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    

    /*set egress drop*/
    if ((ret = apollomp_raw_ponMac_egrDropState_set(physicalQid, pQueueCfg->egrssDrop)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_add */



/* Function Name:
 *      dal_apollomp_ponmac_queue_get
 * Description:
 *      get queue setting 
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList/pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32 
dal_apollomp_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg)
{
    int32   ret;
    rtk_enable_t   enable;
    uint32  qMask;
    uint32  physicalQid;
    apollomp_raw_queue_type_t  queueType;    


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pQueueCfg), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if ((ret = apollomp_raw_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);
    
    if ((ret = apollomp_raw_ponMac_queueType_get(physicalQid, &queueType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    if(APOLLOMP_QOS_QUEUE_STRICT == queueType)
        pQueueCfg->type = STRICT_PRIORITY;    
    else
        pQueueCfg->type = WFQ_WRR_PRIORITY;    
    
    /*set wfq weight*/
    if ((ret = apollomp_raw_ponMac_wfqWeight_get(physicalQid, &(pQueueCfg->weight))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    /*set PIR CIR*/            
    if ((ret = apollomp_raw_ponMacCirRate_get(physicalQid, &(pQueueCfg->cir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    if ((ret = apollomp_raw_ponMacPirRate_get(physicalQid, &(pQueueCfg->pir))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    

    /*set egress drop*/
    if ((ret = apollomp_raw_ponMac_egrDropState_get(physicalQid, &(pQueueCfg->egrssDrop))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
        
    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_get */


/* Function Name:
 *      dal_apollomp_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32 
dal_apollomp_ponmac_queue_del(rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  qMask;
    rtk_enable_t   enable;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    if ((ret = apollomp_raw_ponMacTcontEnable_get(pQueue->schedulerId, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if(DISABLED==enable)
        return RT_ERR_ENTRY_NOTFOUND;

    if ((ret = apollomp_raw_ponMacScheQmap_get(pQueue->schedulerId, &qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if(!(qMask & 1<<(pQueue->queueId)))
        return RT_ERR_ENTRY_NOTFOUND;


    /*remove queue from qMask*/
    qMask = qMask & (~(1<<pQueue->queueId));
    if ((ret = apollomp_raw_ponMacScheQmap_set(pQueue->schedulerId, qMask)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if(0 == qMask)
    {/*no queue map to this t-cont disable schedule for this t-cont*/
        if ((ret = apollomp_raw_ponMacTcontEnable_set(pQueue->schedulerId, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
            return ret;
        }        
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_queue_del */



/* Function Name:
 *      dal_apollomp_ponmac_flow2Queue_set
 * Description:
 *      mapping flow to given queue
 * Input:
 *      flow          - flow id.
 *      pQueue        - queue id. 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32 
dal_apollomp_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;


    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((pQueue->schedulerId >= HAL_MAX_NUM_OF_GPON_TCONT()), RT_ERR_INPUT);
    RT_PARAM_CHK((pQueue->queueId >= APOLLOMP_TCONT_QUEUE_MAX), RT_ERR_INPUT);

    _ponmac_physicalQueueId_get(pQueue->schedulerId, pQueue->queueId, &physicalQid);
 
    if ((ret = apollomp_raw_ponMacSidToQueueMap_set(flow, physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_flow2Queue_set */


/* Function Name:
 *      dal_apollomp_ponmac_flow2Queue_get
 * Description:
 *      get queue id for this flow
 * Input:
 *      flow         - flow id.
 * Output:
 *      pQueue       - queue id. 
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueue point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
int32 
dal_apollomp_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue)
{
    int32   ret;
    uint32  physicalQid;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PONMAC),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(ponmac_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_CLASSIFY_SID_NUM() <= flow), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pQueue), RT_ERR_NULL_POINTER);
 
    if ((ret = apollomp_raw_ponMacSidToQueueMap_get(flow, &physicalQid)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }
    
    if ((ret = _ponmac_schedulerQueue_get(physicalQid, &(pQueue->schedulerId), &(pQueue->queueId))) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PONMAC|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_ponmac_flow2Queue_get */


