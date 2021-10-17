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
 * $Revision: 15909 $
 * $Date: 2011-02-23 11:09:25 +0800 (?Ÿæ?ä¸? 23 äºŒæ? 2011) $
 *
 * Purpose : Definition of L2 API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Mac address flush
 *           (2) Address learning limit
 *           (3) Parameter for L2 lookup and learning engine
 *           (4) Unicast address
 *           (5) L2 multicast
 *           (6) IP multicast
 *           (7) Multicast forwarding table
 *           (8) CPU mac
 *           (9) Port move
 *           (10) Parameter for lookup miss
 *           (11) Parameter for MISC
 *
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/l2.h>
#include <dal/dal_mgmt.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */

/* Module Name    : L2     */
/* Sub-module Name: Global */

/* Function Name:
 *      rtk_l2_init
 * Description:
 *      Initialize l2 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l2 module before calling any l2 APIs.
 */
int32
rtk_l2_init(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_init( );
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_init */

/* Module Name    : L2                */
/* Sub-module Name: Mac address flush */

/* Function Name:
 *      rtk_l2_flushLinkDownPortAddrEnable_get
 * Description:
 *      Get HW flush linkdown port mac configuration.
 * Input:
 *      None
 * Output:
 *      pEnable - pointer buffer of state of HW clear linkdown port mac
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
rtk_l2_flushLinkDownPortAddrEnable_get(rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_flushLinkDownPortAddrEnable_get( pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_flushLinkDownPortAddrEnable_get */


/* Function Name:
 *      rtk_l2_flushLinkDownPortAddrEnable_set
 * Description:
 *      Set HW flush linkdown port mac configuration.
 * Input:
 *      enable - configure value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) Make sure chip have supported the function before using the API.
 *      (2) The API is apply to whole system.
 *      (3) The status of flush linkdown port address is as following:
 *          - DISABLED
 *          - ENABLED
 */
int32
rtk_l2_flushLinkDownPortAddrEnable_set(rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_flushLinkDownPortAddrEnable_set( enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_flushLinkDownPortAddrEnable_set */



/* Function Name:
 *      rtk_l2_ucastAddr_flush
 * Description:
 *      Flush unicast address
 * Input:
 *      pConfig - flush config
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_l2_ucastAddr_flush(rtk_l2_flushCfg_t *pConfig)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ucastAddr_flush( pConfig);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ucastAddr_flush */

/* Module Name    : L2                     */
/* Sub-module Name: Address learning limit */

/* Function Name:
 *      rtk_l2_limitLearningOverStatus_get
 * Description:
 *      Get the system learning over status
 * Input:
 *      None.
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
rtk_l2_limitLearningOverStatus_get(uint32 *pStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_limitLearningOverStatus_get(pStatus);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l2_limitLearningOverStatus_clear
 * Description:
 *      Clear the system learning over status
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
int32
rtk_l2_limitLearningOverStatus_clear(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_limitLearningOverStatus_clear();
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l2_learningCnt_get
 * Description:
 *      Get the total mac learning counts of the whole specified device.
 * Input:
 *      None
 * Output:
 *      pMac_cnt - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 */
int32
rtk_l2_learningCnt_get(uint32 *pMac_cnt)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_learningCnt_get(pMac_cnt);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_learningCnt_get */

/* Function Name:
 *      rtk_l2_limitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the specified device.
 * Input:
 *      None
 * Output:
 *      pMac_cnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the mac_cnt to 0 mean disable learning in the system.
 */
int32
rtk_l2_limitLearningCnt_get(uint32 *pMac_cnt)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_limitLearningCnt_get(pMac_cnt);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_limitLearningCnt_get */

/* Function Name:
 *      rtk_l2_limitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the specified device.
 * Input:
 *      mac_cnt - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID             - invalid unit id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the mac_cnt to 0 mean disable learning in the system.
 */
int32
rtk_l2_limitLearningCnt_set(uint32 mac_cnt)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_limitLearningCnt_set(mac_cnt);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_limitLearningCnt_set */

/* Function Name:
 *      rtk_l2_limitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the specified device.
 * Input:
 *      None
 * Output:
 *      pAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8398, 8389B
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
rtk_l2_limitLearningCntAction_get(rtk_l2_limitLearnCntAction_t *pAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_limitLearningCntAction_get(pAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_limitLearningCntAction_get */

/* Function Name:
 *      rtk_l2_limitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the specified device.
 * Input:
 *      action - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Applicable:
 *      8398, 8389B
 * Note:
 *      (1) The action symbol as following
 *          - LIMIT_LEARN_CNT_ACTION_DROP
 *          - LIMIT_LEARN_CNT_ACTION_FORWARD
 *          - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *          - LIMIT_LEARN_CNT_ACTION_COPY_TO_CPU
 */
int32
rtk_l2_limitLearningCntAction_set(rtk_l2_limitLearnCntAction_t action)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_limitLearningCntAction_set(action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_limitLearningCntAction_set */

/* Function Name:
 *      rtk_l2_portLimitLearningOverStatus_get
 * Description:
 *      Get the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      pStatus     - 1: learning over, 0: not learning over
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *
 */
int32
rtk_l2_portLimitLearningOverStatus_get(rtk_port_t port, uint32 *pStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLimitLearningOverStatus_get( port, pStatus);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l2_portLimitLearningOverStatus_clear
 * Description:
 *      Clear the port learning over status
 * Input:
 *      port        - Port ID
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *
 */
int32
rtk_l2_portLimitLearningOverStatus_clear(rtk_port_t port)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLimitLearningOverStatus_clear( port);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l2_portLearningCnt_get
 * Description:
 *      Get the mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt  - pointer buffer of mac learning counts of the port
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The mac learning counts only calculate dynamic mac numbers.
 *      (2) RTL8329/RTL8389 chip have support the counter per port, but don't provide
 *          API to get right now.
 */
int32
rtk_l2_portLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLearningCnt_get( port, pMacCnt);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_portLearningCnt_get */



/* Function Name:
 *      rtk_l2_portLimitLearningCnt_get
 * Description:
 *      Get the maximum mac learning counts of the port.
 * Input:
 *      port     - port id
 * Output:
 *      pMacCnt - pointer buffer of maximum mac learning counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the mac_cnt to 0 mean disable learning in the port.
 */
int32
rtk_l2_portLimitLearningCnt_get(rtk_port_t port, uint32 *pMacCnt)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLimitLearningCnt_get( port, pMacCnt);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_portLimitLearningCnt_get */


/* Function Name:
 *      rtk_l2_portLimitLearningCnt_set
 * Description:
 *      Set the maximum mac learning counts of the port.
 * Input:
 *      port    - port id
 *      macCnt  - maximum mac learning counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_LIMITED_L2ENTRY_NUM - invalid limited L2 entry number
 * Note:
 *      (1) The maximum mac learning counts only limit for dynamic learning mac
 *          address, not apply to static mac address.
 *      (2) Set the mac_cnt to 0 mean disable learning in the port.
 */
int32
rtk_l2_portLimitLearningCnt_set(rtk_port_t port, uint32 macCnt)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLimitLearningCnt_set( port, macCnt);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_portLimitLearningCnt_set */


/* Function Name:
 *      rtk_l2_portLimitLearningCntAction_get
 * Description:
 *      Get the action when over learning maximum mac counts of the port.
 * Input:
 *      port    - port id
 * Output:
 *      pAction - pointer buffer of action when over learning maximum mac counts
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
rtk_l2_portLimitLearningCntAction_get(rtk_port_t port, rtk_l2_limitLearnCntAction_t *pAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLimitLearningCntAction_get( port, pAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_portLimitLearningCntAction_get */


/* Function Name:
 *      rtk_l2_portLimitLearningCntAction_set
 * Description:
 *      Set the action when over learning maximum mac counts of the port.
 * Input:
 *      port   - port id
 *      action - action when over learning maximum mac counts
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      The action symbol as following
 *      - LIMIT_LEARN_CNT_ACTION_DROP
 *      - LIMIT_LEARN_CNT_ACTION_FORWARD
 *      - LIMIT_LEARN_CNT_ACTION_TO_CPU
 *      - LIMIT_LEARN_CNT_ACTION_COPY_CPU
 */
int32
rtk_l2_portLimitLearningCntAction_set(rtk_port_t port, rtk_l2_limitLearnCntAction_t action)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLimitLearningCntAction_set( port, action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_portLimitLearningCntAction_set */



/* Module Name    : L2                                          */
/* Sub-module Name: Parameter for L2 lookup and learning engine */

/* Function Name:
 *      rtk_l2_aging_get
 * Description:
 *      Get the dynamic address aging time.
 * Input:
 *      None
 * Output:
 *      pAgingTime - pointer buffer of aging time
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Get aging_time as 0 mean disable aging mechanism. (0.1sec)
 */
int32
rtk_l2_aging_get(uint32 *pAgingTime)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_aging_get( pAgingTime);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_aging_get */


/* Function Name:
 *      rtk_l2_aging_set
 * Description:
 *      Set the dynamic address aging time.
 * Input:
 *      agingTime - aging time
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      (1) RTL8329/RTL8389 aging time is not configurable.
 *      (2) apply aging_time as 0 mean disable aging mechanism.
 */
int32
rtk_l2_aging_set(uint32 agingTime)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_aging_set( agingTime);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_aging_set */


/* Function Name:
 *      rtk_l2_portAgingEnable_get
 * Description:
 *      Get the dynamic address aging out setting of the specified port.
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to enable status of Age
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_l2_portAgingEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portAgingEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_portAgingEnable_get */

/* Function Name:
 *      rtk_l2_portAgingEnable_set
 * Description:
 *      Set the dynamic address aging out configuration of the specified port
 * Input:
 *      port    - port id
 *      enable  - enable status of Age
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_l2_portAgingEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portAgingEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_portAgingEnable_set */

/* Module Name    : L2      */
/* Sub-module Name: Parameter for lookup miss */
/* Function Name:
 *      rtk_l2_lookupMissAction_get
 * Description:
 *      Get forwarding action when destination address lookup miss.
 * Input:
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
rtk_l2_lookupMissAction_get(rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_lookupMissAction_get( type, pAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_lookupMissAction_get */

/* Function Name:
 *      rtk_l2_lookupMissAction_set
 * Description:
 *      Set forwarding action when destination address lookup miss.
 * Input:
 *      type   - type of lookup miss
 *      action - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_INPUT      - invalid type of lookup miss
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
rtk_l2_lookupMissAction_set(rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_lookupMissAction_set( type, action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_lookupMissAction_set */

/* Function Name:
 *      rtk_l2_portLookupMissAction_get
 * Description:
 *      Get forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 * Output:
 *      pAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_NULL_POINTER     - input parameter may be null pointer
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
rtk_l2_portLookupMissAction_get(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t *pAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLookupMissAction_get( port, type, pAction);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l2_portLookupMissAction_set
 * Description:
 *      Set forwarding action of specified port when destination address lookup miss.
 * Input:
 *      port    - port id
 *      type    - type of lookup miss
 *      action  - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT         - The module is not initial
 *      RT_ERR_INPUT            - invalid type of lookup miss
 *      RT_ERR_PORT_ID          - invalid port id
 *      RT_ERR_FWD_ACTION       - invalid forwarding action
 * Note:
 *      Type of lookup missis as following:
 *      - DLF_TYPE_IPMC
 *      - DLF_TYPE_UCAST
 *      - DLF_TYPE_MCAST
 *      - DLF_TYPE_IP6MC
 *
 *      Forwarding action is as following:
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_FORWARD
 *      - ACTION_DROP_EXCLUDE_RMA (Only for DLF_TYPE_MCAST)
 */
int32
rtk_l2_portLookupMissAction_set(rtk_port_t port, rtk_l2_lookupMissType_t type, rtk_action_t action)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_portLookupMissAction_set( port, type, action);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_get
 * Description:
 *      Get flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type   - type of lookup miss
 * Output:
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
rtk_l2_lookupMissFloodPortMask_get(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_lookupMissFloodPortMask_get( type, pFlood_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_lookupMissFloodPortMask_get */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_set
 * Description:
 *      Set flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type            - type of lookup miss
 *      pFlood_portmask - flooding port mask configuration when unicast/multicast lookup missed.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC, DLF_TYPE_IP6MC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
rtk_l2_lookupMissFloodPortMask_set(rtk_l2_lookupMissType_t type, rtk_portmask_t *pFlood_portmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_lookupMissFloodPortMask_set( type, pFlood_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_lookupMissFloodPortMask_set */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_add
 * Description:
 *      Add one port member to flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration.
 */
int32
rtk_l2_lookupMissFloodPortMask_add(rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_lookupMissFloodPortMask_add( type, flood_port);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_lookupMissFloodPortMask_add */

/* Function Name:
 *      rtk_l2_lookupMissFloodPortMask_del
 * Description:
 *      Del one port member in flooding port mask when unicast or multicast address lookup missed in L2 table.
 * Input:
 *      type        - type of lookup miss
 *      flood_port  - port id that is going to be added in flooding port mask.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      DLF_TYPE_IPMC & DLF_TYPE_MCAST shares the same configuration..
 */
int32
rtk_l2_lookupMissFloodPortMask_del(rtk_l2_lookupMissType_t type, rtk_port_t flood_port)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_lookupMissFloodPortMask_del( type, flood_port);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_lookupMissFloodPortMask_del */

/* Module Name    : L2      */
/* Sub-module Name: Unicast */
/* Function Name:
 *      rtk_l2_newMacOp_get
 * Description:
 *      Get learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pLrnMode   - pointer to learning mode
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
rtk_l2_newMacOp_get(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  *pLrnMode,
    rtk_action_t            *pFwdAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_newMacOp_get( port, pLrnMode, pFwdAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_newMacOp_get */

/* Function Name:
 *      rtk_l2_newMacOp_set
 * Description:
 *      Set learning mode and forwarding action of new learned address on specified port.
 * Input:
 *      port      - port id
 *      lrnMode   - learning mode
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_INPUT      - invalid input parameter
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 *
 *      Learning mode is as following
 *      - HARDWARE_LEARNING
 *      - SOFTWARE_LEARNING
 *      - NOT_LEARNING
 */
int32
rtk_l2_newMacOp_set(
    rtk_port_t              port,
    rtk_l2_newMacLrnMode_t  lrnMode,
    rtk_action_t            fwdAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_newMacOp_set( port, lrnMode, fwdAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_newMacOp_set */


/* Module Name    : L2              */
/* Sub-module Name: Get next entry */

/* Function Name:
 *      rtk_l2_nextValidAddr_get
 * Description:
 *      Get next valid L2 unicast address entry from the specified device.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 *      include_static - the get type, include static mac or not.
 * Output:
 *      pL2Data       - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
rtk_l2_nextValidAddr_get(
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2Data)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidAddr_get( pScanIdx, pL2Data);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_nextValidAddr_get */


/* Function Name:
 *      rtk_l2_nextValidAddrOnPort_get
 * Description:
 *      Get next valid L2 unicast address entry from specify port.
 * Input:
 *      pScanIdx      - currently scan index of l2 table to get next.
 * Output:
 *      pL2Data       - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 multicast and ip multicast entry and
 *          reply next valid L2 unicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScanIdx is the input and also is the output argument.
 */
int32
rtk_l2_nextValidAddrOnPort_get(
    rtk_port_t          port,
    int32               *pScanIdx,
    rtk_l2_ucastAddr_t  *pL2Data)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidAddrOnPort_get( port, pScanIdx, pL2Data);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_nextValidAddrOnPort_get */


/* Function Name:
 *      rtk_l2_nextValidMcastAddr_get
 * Description:
 *      Get next valid L2 multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pL2Data  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and ip multicast entry and
 *          reply next valid L2 multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
int32
rtk_l2_nextValidMcastAddr_get(
    int32               *pScanIdx,
    rtk_l2_mcastAddr_t  *pL2Data)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidMcastAddr_get( pScanIdx, pL2Data);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_nextValidMcastAddr_get */

/* Function Name:
 *      rtk_l2_nextValidIpMcastAddr_get
 * Description:
 *      Get next valid L2 ip multicast address entry from the specified device.
 * Input:
 *      pScanIdx - currently scan index of l2 table to get next.
 * Output:
 *      pL2Data  - structure of l2 address data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      (1) The function will skip valid l2 unicast and multicast entry and
 *          reply next valid L2 ip multicast address is based on index order of l2 table.
 *      (2) Please input 0 for get the first entry of l2 table.
 *      (3) The pScan_idx is the input and also is the output argument.
 */
int32
rtk_l2_nextValidIpMcastAddr_get(
    int32                   *pScanIdx,
    rtk_l2_ipMcastAddr_t    *pL2Data)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidIpMcastAddr_get( pScanIdx, pL2Data);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_nextValidIpMcastAddr_get */



/* Function Name:
 *      rtk_l2_nextValidEntry_get
 * Description:
 *      Get LUT next valid entry.
 * Input:
 *      pScanIdx - Index field in the structure.
 * Output:
 *      pL2Entry - entry content
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_L2_EMPTY_ENTRY   - Empty LUT entry.
 *      RT_ERR_INPUT            - Invalid input parameters.
 * Note:
 *      This API is used to get next valid LUT entry.
 */
int32
rtk_l2_nextValidEntry_get(
        int32                   *pScanIdx,
        rtk_l2_addr_table_t     *pL2Entry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_nextValidEntry_get( pScanIdx, pL2Entry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_nextValidEntry_get */



/* Module Name    : L2              */
/* Sub-module Name: Unicast address */

/* Function Name:
 *      rtk_l2_addr_add
 * Description:
 *      Add L2 entry to ASIC.
 * Input:
 *      pL2_addr - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 entry before add it.
 */
int32
rtk_l2_addr_add(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_addr_add( pL2Addr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_addr_add */

/* Function Name:
 *      rtk_l2_addr_del
 * Description:
 *      Delete a L2 unicast address entry.
 * Input:
 *      pL2Addr  - L2 entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_VLAN_VID          - invalid vid
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the mac has existed in the LUT, it will be deleted. Otherwise, it will return RT_ERR_L2_ENTRY_NOTFOUND.
 */
int32
rtk_l2_addr_del(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_addr_del( pL2Addr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_addr_del */


/* Function Name:
 *      rtk_l2_addr_get
 * Description:
 *      Get L2 entry based on specified vid and MAC address
 * Input:
 *      None
 * Output:
 *      pL2Addr - pointer to L2 entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_VLAN_VID          - invalid vlan id
 *      RT_ERR_MAC               - invalid mac address
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 *      RT_ERR_L2_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      If the unicast mac address existed in LUT, it will return the port and fid where
 *      the mac is learned. Otherwise, it will return a RT_ERR_L2_ENTRY_NOTFOUND error.
 */
int32
rtk_l2_addr_get(rtk_l2_ucastAddr_t *pL2Addr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_addr_get( pL2Addr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_addr_get */


/* Function Name:
 *      rtk_l2_addr_delAll
 * Description:
 *      Delete all L2 unicast address entry.
 * Input:
 *      includeStatic - include static mac or not?
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
rtk_l2_addr_delAll(uint32 includeStatic)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_addr_delAll( includeStatic);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_addr_delAll */


/* Module Name    : L2           */
/* Sub-module Name: l2 multicast */

/* Function Name:
 *      rtk_l2_mcastAddr_add
 * Description:
 *      Add L2 multicast entry to ASIC.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      Need to initialize L2 multicast entry before add it.
 */
int32
rtk_l2_mcastAddr_add(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_mcastAddr_add( pMcastAddr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_mcastAddr_add */

/* Function Name:
 *      rtk_l2_mcastAddr_del
 * Description:
 *      Delete a L2 multicast address entry.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      None
 */
int32
rtk_l2_mcastAddr_del(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_mcastAddr_del( pMcastAddr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_mcastAddr_del */

/* Function Name:
 *      rtk_l2_mcastAddr_get
 * Description:
 *      Update content of L2 multicast entry.
 * Input:
 *      pMcastAddr - L2 multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_VLAN_VID     - invalid vlan id
 *      RT_ERR_MAC          - invalid mac address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_INPUT        - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_l2_mcastAddr_get(rtk_l2_mcastAddr_t *pMcastAddr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_mcastAddr_get( pMcastAddr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_mcastAddr_get */

/* Module Name    : L2        */
/* Sub-module Name: Port move */
/* Function Name:
 *      rtk_l2_illegalPortMoveAction_get
 * Description:
 *      Get forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port       - port id
 * Output:
 *      pFwdAction - pointer to forwarding action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
rtk_l2_illegalPortMoveAction_get(
    rtk_port_t          port,
    rtk_action_t        *pFwdAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_illegalPortMoveAction_get( port, pFwdAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_illegalPortMoveAction_get */

/* Function Name:
 *      rtk_l2_illegalPortMoveAction_set
 * Description:
 *      Set forwarding action when illegal port moving happen on specified port.
 * Input:
 *      port      - port id
 *      fwdAction - forwarding action
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_DROP
 *      - ACTION_TRAP2CPU
 *      - ACTION_COPY2CPU
 */
int32
rtk_l2_illegalPortMoveAction_set(
    rtk_port_t          port,
    rtk_action_t        fwdAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_illegalPortMoveAction_set( port, fwdAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_illegalPortMoveAction_set */


/* Module Name    : L2           */
/* Sub-module Name: IP multicast */


/* Function Name:
 *      rtk_l2_ipmcMode_get
 * Description:
 *      Get lookup mode of layer2 ip multicast switching.
 * Input:
 *      None
 * Output:
 *      pMode - pointer to lookup mode of layer2 ip multicast switching
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DPI_AND_VID
 */
int32
rtk_l2_ipmcMode_get(rtk_l2_ipmcMode_t *pMode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipmcMode_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipmcMode_get */

/* Function Name:
 *      rtk_l2_ipmcMode_set
 * Description:
 *      Set lookup mode of layer2 ip multicast switching.
 * Input:
 *      mode - lookup mode of layer2 ip multicast switching
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      Lookup mode of layer2 ip multicast switching is as following
 *      - LOOKUP_ON_DIP_AND_SIP
 *      - LOOKUP_ON_MAC_AND_VID_FID
 *      - LOOKUP_ON_DPI_AND_VID
 */
int32
rtk_l2_ipmcMode_set(rtk_l2_ipmcMode_t mode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipmcMode_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipmcMode_set */

/* Function Name:
 *      rtk_l2_ipmcGroupLookupMissHash_get
 * Description:
 *      Get Hash operation of IPv4 multicast packet which is not in IPMC Group Table.
 * Input:
 *      None
 * Output:
 *      pMode - pointer of Hash operation of IPv4 multicast packet which is not in IPMC Group Table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
rtk_l2_ipmcGroupLookupMissHash_get(rtk_l2_ipmcHashOp_t *pMode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipmcGroupLookupMissHash_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipmcGroupLookupMissHash_get */

/* Function Name:
 *      rtk_l2_ipmcGroupLookupMissHash_set
 * Description:
 *      Set Hash operation of IPv4 multicast packet which is not in IPMC Group Table.
 * Input:
 *      mode - Hash operation of IPv4 multicast packet which is not in IPMC Group Table
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
rtk_l2_ipmcGroupLookupMissHash_set(rtk_l2_ipmcHashOp_t mode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipmcGroupLookupMissHash_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipmcGroupLookupMissHash_set */

/* Function Name:
 *      rtk_l2_ipmcGroup_add
 * Description:
 *      Add an entry to IPMC Group Table.
 * Input:
 *      gip         - Group IP
 *      pPortmask   - Group member port mask
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 *      RT_ERR_ENTRY_FULL   - entry is full
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroup_add(ipaddr_t gip, rtk_portmask_t *pPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipmcGroup_add( gip, pPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipmcGroup_add */

/* Function Name:
 *      rtk_l2_ipmcGroup_del
 * Description:
 *      Delete an entry from IPMC Group Table.
 * Input:
 *      gip         - Group IP
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroup_del(ipaddr_t gip)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipmcGroup_del( gip);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipmcGroup_del */

/* Function Name:
 *      rtk_l2_ipmcGroup_get
 * Description:
 *      Get an entry from IPMC Group Table.
 * Input:
 *      gip         - Group IP
 * Output:
 *      pPortmask   - Group member port mask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT       - The module is not initial
 *      RT_ERR_NULL_POINTER   - input parameter may be null pointer
 *      RT_ERR_ENTRY_NOTFOUND - specified entry not found
 * Note:
 *      None
 */
int32
rtk_l2_ipmcGroup_get(ipaddr_t gip, rtk_portmask_t *pPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipmcGroup_get( gip, pPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipmcGroup_get */

/* Function Name:
 *      rtk_l2_ipMcastAddr_add
 * Description:
 *      Add IP multicast entry to ASIC.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT      - The module is not initial
 *      RT_ERR_IPV4_ADDRESS  - Invalid IPv4 address
 *      RT_ERR_VLAN_VID      - invalid vlan id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_INPUT         - invalid input parameter
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
int32
rtk_l2_ipMcastAddr_add(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipMcastAddr_add( pIpmcastAddr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipMcastAddr_add */

/* Function Name:
 *      rtk_l2_ipMcastAddr_del
 * Description:
 *      Delete a L2 ip multicast address entry from the specified device.
 * Input:
 *      pIpmcastAddr  - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_L2_HASH_KEY    - invalid L2 Hash key
 *      RT_ERR_L2_EMPTY_ENTRY - the entry is empty(invalid)
 * Note:
 *      (1) In vlan unaware mode (SVL), the vid will be ignore, suggest to
 *          input vid=0 in vlan unaware mode.
 *      (2) In vlan aware mode (IVL), the vid will be care.
 */
int32
rtk_l2_ipMcastAddr_del(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipMcastAddr_del( pIpmcastAddr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipMcastAddr_del */

/* Function Name:
 *      rtk_l2_ipMcastAddr_get
 * Description:
 *      Get IP multicast entry on specified dip and sip.
 * Input:
 *      pIpmcastAddr - IP multicast entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_IPV4_ADDRESS - Invalid IPv4 address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Need to initialize IP multicast entry before add it.
 */
int32
rtk_l2_ipMcastAddr_get(rtk_l2_ipMcastAddr_t *pIpmcastAddr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_ipMcastAddr_get( pIpmcastAddr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_ipMcastAddr_get */


/* Module Name    : L2                 */
/* Sub-module Name: Parameter for MISC */


/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_get
 * Description:
 *      Get source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
int32
rtk_l2_srcPortEgrFilterMask_get(rtk_portmask_t *pFilter_portmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_srcPortEgrFilterMask_get( pFilter_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_srcPortEgrFilterMask_get */

/* Function Name:
 *      rtk_l2_srcPortEgrFilterMask_set
 * Description:
 *      Set source port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pFilter_portmask - source port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
rtk_l2_srcPortEgrFilterMask_set(rtk_portmask_t *pFilter_portmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_srcPortEgrFilterMask_set( pFilter_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_srcPortEgrFilterMask_set */

/* Function Name:
 *      rtk_l2_extPortEgrFilterMask_get
 * Description:
 *      Get extension port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      None
 * Output:
 *      pExt_portmask - extension port egress filtering configuration when packet egress.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected.
 *      Get permittion status for frames if its source port is equal to destination port.
 */
int32
rtk_l2_extPortEgrFilterMask_get(rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_extPortEgrFilterMask_get( pExt_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_extPortEgrFilterMask_get */

/* Function Name:
 *      rtk_l2_extPortEgrFilterMask_set
 * Description:
 *      Set extension port egress filter mask to determine if mac need to do source filtering for an specific port
 *      when packet egress.
 * Input:
 *      pExt_portmask - extension port egress filtering configuration when packet egress.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      May be used when wirless device connected
 */
int32
rtk_l2_extPortEgrFilterMask_set(rtk_portmask_t *pExt_portmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->l2_extPortEgrFilterMask_set( pExt_portmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_l2_extPortEgrFilterMask_set */

