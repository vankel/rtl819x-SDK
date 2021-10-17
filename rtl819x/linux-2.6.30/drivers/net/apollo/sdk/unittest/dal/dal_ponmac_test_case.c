/*
 * Copyright(c) Realtek Semiconductor Corporation, 2012
 * All rights reserved.
 *
 * $Revision: 14276 $
 * $Date: 2010-11-18 09:37:36 +0800 (Thu, 18 Nov 2010) $
 *
 * Purpose : Test PON MAC API
 *
 * Feature : PON MAC API test 
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <rtk/ponmac.h>
#include <dal/dal_ponmac_test_case.h>
#include <hal/common/halctrl.h>
/*
 * Function Declaration
 */


/*
 * Function Declaration
 */

int32 dal_ponmac_queue_test(uint32 caseNo)
{  
    int32  ret;
    rtk_ponmac_queueCfg_t   wQueueCfg;
    rtk_ponmac_queueCfg_t   rQueueCfg;
    rtk_ponmac_queue_t queue;
    uint32 schedulerId;
    uint32 queueId;
    uint32 weight;
    uint32 rate;
    rtk_qos_queue_type_t   type;
    rtk_enable_t egrssDrop;
          
    queue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    queue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1 ;
    
    wQueueCfg.cir       = HAL_PONMAC_PIR_CIR_RATE_MAX(); 
    wQueueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
    wQueueCfg.type      = STRICT_PRIORITY;
    wQueueCfg.weight    = 0;
    wQueueCfg.egrssDrop = ENABLED;
    if (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_ponmac_queue_add(&queue, &wQueueCfg) )
        return RT_ERR_OK;
    if (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_ponmac_queue_get(&queue, &rQueueCfg) )
        return RT_ERR_OK;
    if (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_ponmac_queue_del(&queue) )
        return RT_ERR_OK;

    /*error input check*/
    /*1. out of range*/
        /*schedulerId out of range*/
    queue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT();
    queue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1 ;
    if( rtk_ponmac_queue_add(&queue, &wQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_ponmac_queue_add(&queue, &rQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_ponmac_queue_del(&queue) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*queueId out of range*/
    queue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    queue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX();
    if( rtk_ponmac_queue_add(&queue, &wQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_ponmac_queue_add(&queue, &rQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_ponmac_queue_del(&queue) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*cir out of range*/
    queue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    queue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1;
    wQueueCfg.cir       = HAL_PONMAC_PIR_CIR_RATE_MAX()+1; 
    wQueueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
    wQueueCfg.type      = STRICT_PRIORITY;
    wQueueCfg.weight    = 0;
    wQueueCfg.egrssDrop = ENABLED;

    if( rtk_ponmac_queue_add(&queue, &wQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*pir out of range*/
    queue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    queue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1;
    wQueueCfg.cir       = HAL_PONMAC_PIR_CIR_RATE_MAX(); 
    wQueueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX()+1;
    wQueueCfg.type      = STRICT_PRIORITY;
    wQueueCfg.weight    = 0;
    wQueueCfg.egrssDrop = ENABLED;

    if( rtk_ponmac_queue_add(&queue, &wQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*queue type out of range*/
    queue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    queue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1;
    wQueueCfg.cir       = HAL_PONMAC_PIR_CIR_RATE_MAX(); 
    wQueueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
    wQueueCfg.type      = QUEUE_TYPE_END;
    wQueueCfg.weight    = HAL_QUEUE_WEIGHT_MAX();
    wQueueCfg.egrssDrop = ENABLED;

    if( rtk_ponmac_queue_add(&queue, &wQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
        /*queue weight out of range*/
    queue.schedulerId   = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    queue.queueId       = HAL_PONMAC_TCONT_QUEUE_MAX()-1;
    wQueueCfg.cir       = HAL_PONMAC_PIR_CIR_RATE_MAX(); 
    wQueueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
    wQueueCfg.type      = STRICT_PRIORITY;
    wQueueCfg.weight    = HAL_QUEUE_WEIGHT_MAX()+1;
    wQueueCfg.egrssDrop = ENABLED;

    if( rtk_ponmac_queue_add(&queue, &wQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

        /*egress drop out of range*/
    queue.schedulerId   = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    queue.queueId       = HAL_PONMAC_TCONT_QUEUE_MAX()-1;
    wQueueCfg.cir       = HAL_PONMAC_PIR_CIR_RATE_MAX(); 
    wQueueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
    wQueueCfg.type      = STRICT_PRIORITY;
    wQueueCfg.weight    = HAL_QUEUE_WEIGHT_MAX();
    wQueueCfg.egrssDrop = RTK_ENABLE_END;

    if( rtk_ponmac_queue_add(&queue, &wQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*2. null pointer*/
    queue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    queue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1 ;
    
    wQueueCfg.cir       = HAL_PONMAC_PIR_CIR_RATE_MAX(); 
    wQueueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
    wQueueCfg.type      = STRICT_PRIORITY;
    wQueueCfg.weight    = 0;
    wQueueCfg.egrssDrop = ENABLED;
    if( rtk_ponmac_queue_add(NULL, &wQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( rtk_ponmac_queue_get(NULL, &rQueueCfg) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( rtk_ponmac_queue_del(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( rtk_ponmac_queue_add(&queue, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
    if( rtk_ponmac_queue_get(&queue, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /* add/get/del test*/
    for (schedulerId = 0; schedulerId < HAL_MAX_NUM_OF_GPON_TCONT(); schedulerId=schedulerId+15)
    {    
        if(schedulerId >= (HAL_MAX_NUM_OF_GPON_TCONT() -15))
            schedulerId = HAL_MAX_NUM_OF_GPON_TCONT() -1;
    
        for (queueId = 0; queueId < HAL_PONMAC_TCONT_QUEUE_MAX(); queueId=queueId+8)
        {    
            if(queueId >= (HAL_PONMAC_TCONT_QUEUE_MAX() -8))
                queueId = HAL_PONMAC_TCONT_QUEUE_MAX() -1;
            
            for (weight = 0; weight <= HAL_QUEUE_WEIGHT_MAX(); weight=weight+64)
            {            
                if(weight >= (HAL_QUEUE_WEIGHT_MAX() -64))
                    weight = HAL_QUEUE_WEIGHT_MAX() -1;
                    
                for (rate = 0; rate <= HAL_PONMAC_PIR_CIR_RATE_MAX(); rate=rate+4096)
                {            
                    if(rate >= (HAL_QUEUE_WEIGHT_MAX() -4096))
                        rate = HAL_QUEUE_WEIGHT_MAX() -1;
                    for (type = WFQ_WRR_PRIORITY; type < QUEUE_TYPE_END; type++)
                    {              
                        for (egrssDrop = DISABLED; egrssDrop < RTK_ENABLE_END; egrssDrop++)
                        {              
                            queue.schedulerId   = schedulerId;
                            queue.queueId       = queueId;
                            
                            wQueueCfg.cir       = rate; 
                            wQueueCfg.pir       = rate;
                            wQueueCfg.type      = type;
                            wQueueCfg.weight    = weight;
                            wQueueCfg.egrssDrop = egrssDrop;
                            /*add test*/
                            if( rtk_ponmac_queue_add(&queue,&wQueueCfg) != RT_ERR_OK)
                            {
                                osal_printf("\n %s %d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,schedulerId,queueId);
                                return RT_ERR_FAILED;       
                            }                            
                            /*get test*/
                            memset(&rQueueCfg,0x0,sizeof(rtk_ponmac_queueCfg_t));
                            if( rtk_ponmac_queue_get(&queue,&rQueueCfg) != RT_ERR_OK)
                            {
                                osal_printf("\n %s %d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,schedulerId,queueId);
                                return RT_ERR_FAILED;       
                            }                            
                            
                            /*compare*/
                            if(memcmp(&wQueueCfg,&rQueueCfg,sizeof(rtk_ponmac_queueCfg_t))!=0)
                            {
                                osal_printf("\n %s %d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,schedulerId,queueId);
                                return RT_ERR_FAILED;       
                            }
                            /*delete test*/
                            if( rtk_ponmac_queue_del(&queue) != RT_ERR_OK)
                            {
                                osal_printf("\n %s %d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,schedulerId,queueId);
                                return RT_ERR_FAILED;       
                            }                            
                            /*delete test*/
                            /*must return fail*/
                            if( rtk_ponmac_queue_del(&queue) != RT_ERR_ENTRY_NOTFOUND)
                            {
                                osal_printf("\n %s %d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,schedulerId,queueId);
                                return RT_ERR_FAILED;       
                            }                            

                            /*get test*/   
                            /*must return fail*/
                            if( rtk_ponmac_queue_get(&queue,&rQueueCfg) != RT_ERR_ENTRY_NOTFOUND)
                            {
                                osal_printf("\n %s %d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,schedulerId,queueId);
                                return RT_ERR_FAILED;       
                            }                            
                            
                        }            
                    }            
                }
            
            }
        
        }        
    
    }                
    return RT_ERR_OK;        
}


int32 dal_ponmac_flow2Queue_test(uint32 caseNo)
{  
    int32  ret;
    rtk_ponmac_queue_t rQueue;
    rtk_ponmac_queue_t wQueue;
    uint32 flow;
    uint32 schedulerId;
    uint32 queueId;
    rtk_ponmac_queueCfg_t   wQueueCfg;

        
    wQueue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    wQueue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1 ;
    flow = HAL_MAX_NUM_OF_GPON_FLOW()-1;
    if (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_ponmac_flow2Queue_set(flow, &wQueue) )
        return RT_ERR_OK;
    if (RT_ERR_FEATURE_NOT_SUPPORTED == rtk_ponmac_flow2Queue_get(flow, &rQueue) )
        return RT_ERR_OK;    

    /*1. out of range*/
        /*sflow out of range*/
    flow = HAL_MAX_NUM_OF_GPON_FLOW();
    wQueue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    wQueue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1 ;
    if( rtk_ponmac_flow2Queue_set(flow, &wQueue) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_ponmac_flow2Queue_get(flow, &rQueue) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*schedulerId out of range*/
    flow = HAL_MAX_NUM_OF_GPON_FLOW()-1;
    wQueue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT();
    wQueue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX()-1 ;
    if( rtk_ponmac_flow2Queue_set(flow, &wQueue) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        /*queueId out of range*/
    flow = HAL_MAX_NUM_OF_GPON_FLOW()-1;
    wQueue.schedulerId = HAL_MAX_NUM_OF_GPON_TCONT()-1;
    wQueue.queueId = HAL_PONMAC_TCONT_QUEUE_MAX();
    if( rtk_ponmac_flow2Queue_set(flow, &wQueue) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*2. null pointer*/
    flow = HAL_MAX_NUM_OF_GPON_FLOW()-1;
    if( rtk_ponmac_flow2Queue_set(flow, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_ponmac_flow2Queue_get(flow, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    wQueueCfg.cir       = HAL_PONMAC_PIR_CIR_RATE_MAX(); 
    wQueueCfg.pir       = HAL_PONMAC_PIR_CIR_RATE_MAX();
    wQueueCfg.type      = STRICT_PRIORITY;
    wQueueCfg.weight    = 0;
    wQueueCfg.egrssDrop = ENABLED;

    /*set/get  test*/
    for (schedulerId = 0; schedulerId < HAL_MAX_NUM_OF_GPON_TCONT(); schedulerId=schedulerId+15)
    {    
        if(schedulerId >= (HAL_MAX_NUM_OF_GPON_TCONT() -15))
            schedulerId = HAL_MAX_NUM_OF_GPON_TCONT() -1;
    
        for (queueId = 0; queueId < HAL_PONMAC_TCONT_QUEUE_MAX(); queueId=queueId+8)
        {    
            if(queueId >= (HAL_PONMAC_TCONT_QUEUE_MAX() -8))
                queueId = HAL_PONMAC_TCONT_QUEUE_MAX() -1;
            
            for (flow = 0; flow < HAL_MAX_NUM_OF_GPON_FLOW(); flow=flow+32)
            {    
                if(flow >= (HAL_MAX_NUM_OF_GPON_FLOW() -32))
                    flow = HAL_MAX_NUM_OF_GPON_FLOW() -1;
                
                
                wQueue.schedulerId   = schedulerId;
                wQueue.queueId       = queueId;
                /*set test*/
                if( rtk_ponmac_flow2Queue_set(flow,&wQueue) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d flow:%d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,flow,schedulerId,queueId);
                    return RT_ERR_FAILED;       
                }                            
                
                
                /*add queue*/
                if( rtk_ponmac_queue_add(&wQueue, &wQueueCfg) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d flow:%d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,flow,schedulerId,queueId);
                    return RT_ERR_FAILED;       
                } 

                /*get test*/
                memset(&rQueue,0x0,sizeof(rtk_ponmac_queue_t));
                if( rtk_ponmac_flow2Queue_get(flow,&rQueue) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d flow:%d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,flow,schedulerId,queueId);
                    return RT_ERR_FAILED;       
                }                            
                
                /*compare*/
                if(memcmp(&wQueue,&rQueue,sizeof(rtk_ponmac_queue_t))!=0)
                {
                    osal_printf("\n %s %d flow:%d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,flow,schedulerId,queueId);
                    return RT_ERR_FAILED;       
                }
                    
                /*delete queue*/
                if( rtk_ponmac_queue_del(&wQueue) != RT_ERR_OK)
                {
                    osal_printf("\n %s %d flow:%d scheduler:%d queue:%d\n",__FUNCTION__,__LINE__,flow,schedulerId,queueId);
                    return RT_ERR_FAILED;       
                } 
            }            
        }
    }            
         
    return RT_ERR_OK;        
}
