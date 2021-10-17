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
 *           (1) queue configuration (PIR/CIR/Queue schuedule type)  
 *           (2) flow and queue mapping
 */
 
#ifndef __RTK_PONMAC_H__
#define __RTK_PONMAC_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/ponmac.h>
#include <rtk/qos.h>

/*
 * Symbol Definition
 */

#define GPON_MAC_MODE 1

typedef struct rtk_ponmac_queueCfg_s
{
    uint32 cir;
    uint32 pir;
    rtk_qos_queue_type_t type;
    uint32 weight;
    rtk_enable_t egrssDrop;

} rtk_ponmac_queueCfg_t;    


typedef struct rtk_ponmac_queue_s
{
    uint32 schedulerId;
    uint32 queueId;/*0~31*/
} rtk_ponmac_queue_t;    

/*
 * Macro Declaration
 */


/* Module Name    : PON Mac                                  */
/* Sub-module Name: flow/t-cont/queue mapping */

/* Function Name:
 *      rtk_ponmac_init
 * Description:
 *      Configure PON MAC initial settings
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 */
extern int32
rtk_ponmac_init(void);


/* Function Name:
 *      rtk_ponmac_queue_add
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
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32 
rtk_ponmac_queue_add(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


/* Function Name:
 *      rtk_ponmac_queue_get
 * Description:
 *      get queue setting 
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      pQueueCfg     - queue configuration
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pQueueList point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32 
rtk_ponmac_queue_get(rtk_ponmac_queue_t *pQueue, rtk_ponmac_queueCfg_t *pQueueCfg);


/* Function Name:
 *      rtk_ponmac_queue_del
 * Description:
 *      delete queue from given scheduler id
 * Input:
 *      pQueue         - queue id and scheduler id for ths queue.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32 
rtk_ponmac_queue_del(rtk_ponmac_queue_t *pQueue);



/* Function Name:
 *      rtk_ponmac_flow2Queue_set
 * Description:
 *      mapping flow to given queue
 * Input:
 *      flow          - flow id.
 *      pQueue       - queue id. 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32 
rtk_ponmac_flow2Queue_set(uint32  flow, rtk_ponmac_queue_t *pQueue);


/* Function Name:
 *      rtk_ponmac_flow2Queue_get
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
extern int32 
rtk_ponmac_flow2Queue_get(uint32  flow, rtk_ponmac_queue_t *pQueue);

#endif /* __RTK_PONMAC_H__ */

