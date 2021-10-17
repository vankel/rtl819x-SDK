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
 * $Revision: $
 * $Date: $
 *
 * Purpose : Declaration of RLDP and RLPP API
 *
 * Feature : The file have include the following module and sub-modules
 *           1) RLDP and RLPP configuration and status
 *
 */


/*
 * Include Files
 */
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <rtk/rldp.h> 
#include <dal/dal_mgmt.h> 
#include <common/rt_type.h>

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

/* Module Name : RLDP */

/* Function Name:
 *      rtk_rldp_init
 * Description:
 *      Initialize rldp module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize rldp module before calling any rldp APIs.
 */
int32
rtk_rldp_init(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rldp_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rldp_init */


/* Function Name:
 *      rtk_rldp_config_set
 * Description:
 *      Set RLDP module configuration
 * Input:
 *      pConfig - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_config_set(rtk_rldp_config_t *pConfig)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rldp_config_set( pConfig);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rldp_config_set */


/* Function Name:
 *      rtk_rldp_config_get
 * Description:
 *      Get RLDP module configuration
 * Input:
 *      pConfig - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_config_get(rtk_rldp_config_t *pConfig)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rldp_config_get( pConfig);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rldp_config_get */


/* Function Name:
 *      rtk_rldp_portConfig_set
 * Description:
 *      Set per port RLDP module configuration
 * Input:
 *      port   - port number to be configured
 *      pConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_portConfig_set(rtk_port_t port, rtk_rldp_portConfig_t *pConfig)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rldp_portConfig_set( port, pConfig);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rldp_portConfig_set */


/* Function Name:
 *      rtk_rldp_portConfig_get
 * Description:
 *      Get per port RLDP module configuration
 * Input:
 *      port    - port number to be get
 *      pConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_portConfig_get(rtk_port_t port, rtk_rldp_portConfig_t *pConfig)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rldp_portConfig_get( port, pConfig);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rldp_portConfig_get */


/* Function Name:
 *      rtk_rldp_status_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      pStatus - status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_status_get(rtk_rldp_status_t *pStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rldp_status_get( pStatus);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rldp_status_get */


/* Function Name:
 *      rtk_rldp_portStatus_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      port    - port number to be get
 *      pStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rldp_portStatus_get(rtk_port_t port, rtk_rldp_portStatus_t *pStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rldp_portStatus_get( port, pStatus);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rldp_portStatus_get */


/* Function Name:
 *      rtk_rldp_portStatus_clear
 * Description:
 *      Get RLDP module status
 * Input:
 *      port    - port number to be get
 *      pStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Clear operation effect loop_enter and loop_leave only, other field in
 *      the structure are don't care
 */
int32
rtk_rldp_portStatus_clear(rtk_port_t port, rtk_rldp_portStatus_t *pStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rldp_portStatus_clear( port, pStatus);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rldp_portStatus_clear */


/* Function Name:
 *      rtk_rlpp_init
 * Description:
 *      Initialize rlpp module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize rlpp module before calling any rlpp APIs.
 */
int32
rtk_rlpp_init(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rlpp_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rlpp_init */


/* Function Name:
 *      rtk_rlpp_trap_set
 * Description:
 *      Set RLPP trap to cpu operation, trap or not trap
 * Input:
 *      type - RLPP trap operation type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      Trap the RLPP packet to CPU for software processing
 */
int32
rtk_rlpp_trapType_set(rtk_rlpp_trapType_t type)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rlpp_trapType_set( type);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rlpp_trapType_set */


/* Function Name:
 *      rtk_rlpp_trap_get
 * Description:
 *      Get RLPP trap to cpu operation, trap or not trap
 * Input:
 *      pType - RLPP trap operation type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
rtk_rlpp_trapType_get(rtk_rlpp_trapType_t *pType)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->rlpp_trapType_get( pType);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_rlpp_trapType_get */
