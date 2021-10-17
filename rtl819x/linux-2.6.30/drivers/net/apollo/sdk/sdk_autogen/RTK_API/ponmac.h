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


typedef struct rtk_ponmac_queue_s
{
    uint32 queueId;/*0~31*/
    uint32 cir;
    uint32 pir;
    rtk_qos_queue_type_t type;
    uint32 weight;
} rtk_ponmac_queue_t;    




typedef struct rtk_ponmac_queue_map_s
{
    rtk_ponmac_queue_t queue;
    uint32 flowId;
    uint32 tcontId;
} rtk_ponmac_queue_map_t;




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
 *      rtk_ponmac_tcontQueue_add
 * Description:
 *      Add queue and to T-cont and set queue relative configuration
 * Input:
 *      pQueueMap    - us flow entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pFlowCfg point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None
 */
extern int32 
rtk_ponmac_tcontQueue_add(rtk_ponmac_queue_map_t *pQueueMap);


/* Function Name:
 *      rtk_ponmac_tcontQueue_del
 * Description:
 *      delete a queue from the T-cont
 * Input:
 *      pQueueMap    - Queue and t-cont information.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      When delete a queue from T-cont just input following field:
 *      - pQueueMap->queue.queueId
 *      - pQueueMap->tcontId
 */
extern int32 
rtk_ponmac_tcontQueue_del(rtk_ponmac_queue_map_t *pQueueMap);


#endif /* __RTK_PONMAC_H__ */

