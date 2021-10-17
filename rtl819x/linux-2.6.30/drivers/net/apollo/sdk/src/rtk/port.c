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
 * $Revision: 16303 $
 * $Date: 2011-03-16 15:22:24 +0800 (?Ÿæ?ä¸? 16 ä¸‰æ? 2011) $
 *
 * Purpose : Definition of Port API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Parameter settings for the port-based view
 *           (2) RTCT
 */




/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <rtk/port.h>
#include <dal/dal_mgmt.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
rtk_port_init(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_init */

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

/* Function Name:
 *      rtk_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
int32
rtk_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_link_get( port, pStatus);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_link_get */

/* Function Name:
 *      rtk_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to the port speed
 *      pDuplex - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID       - invalid unit id
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      (1) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M
 *
 *      (2) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
int32
rtk_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_speedDuplex_get( port, pSpeed, pDuplex);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_speedDuplex_get */

/* Function Name:
 *      rtk_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      port      - port id
 * Output:
 *      pTxStatus - pointer to the negotiation result of the Tx flow control
 *      pRxStatus - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      None
 */
int32
rtk_port_flowctrl_get(
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_flowctrl_get( port, pTxStatus, pRxStatus);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_flowctrl_get */

/* Function Name:
 *      rtk_port_phyAutoNegoEnable_get
 * Description:
 *      Get PHY ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to PHY auto negotiation status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyAutoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyAutoNegoEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyAutoNegoEnable_get */

/* Function Name:
 *      rtk_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port
 * Input:
 *      port   - port id
 *      enable - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - input parameter out of range
 * Note:
 *      - ENABLED : switch to PHY auto negotiation mode
 *      - DISABLED: switch to PHY force mode
 *      - Once the abilities of both auto-nego and force mode are set,
 *        you can freely swtich the mode without calling ability setting API again
 */
int32
rtk_port_phyAutoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyAutoNegoEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyAutoNegoEnable_set */

/* Function Name:
 *      rtk_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY auto negotiation ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pAbility - pointer to the PHY ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyAutoNegoAbility_get(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyAutoNegoAbility_get( port, pAbility);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyAutoNegoAbility_get */

/* Function Name:
 *      rtk_port_phyAutoNegoAbility_set
 * Description:
 *      Set PHY auto negotiation ability of the specific port
 * Input:
 *      port     - port id
 *      pAbility - pointer to the PHY ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 */
int32
rtk_port_phyAutoNegoAbility_set(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyAutoNegoAbility_set( port, pAbility);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyAutoNegoAbility_set */

/* Function Name:
 *      rtk_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability status of the specific port
 * Input:
 *      port         - port id
 * Output:
 *      pSpeed       - pointer to the port speed
 *      pDuplex      - pointer to the port duplex
 *      pFlowControl - pointer to the flow control enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyForceModeAbility_get(
    rtk_port_t          port,
    rtk_port_speed_t    *pSpeed,
    rtk_port_duplex_t   *pDuplex,
    rtk_enable_t        *pFlowControl)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyForceModeAbility_get( port, pSpeed, pDuplex, pFlowControl);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyForceModeAbility_get */

/* Function Name:
 *      rtk_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode
 * Input:
 *      port        - port id
 *      speed       - port speed
 *      duplex      - port duplex mode
 *      flowControl - enable flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_PHY_SPEED  - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX - invalid PHY duplex setting
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      (1) You can set these abilities no matter which mode PHY currently stays on
 *
 *      (2) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *
 *      (3) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
int32
rtk_port_phyForceModeAbility_set(
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyForceModeAbility_set( port, speed, duplex, flowControl);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyForceModeAbility_set */

/* Function Name:
 *      rtk_port_phyReg_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 * Output:
 *      pMasterSlave        - pointer to the PHY master slave configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyReg_get(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyReg_get( port, page, reg, pData);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyReg_get */

/* Function Name:
 *      rtk_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port
 * Input:
 *      port - port id
 *      page - page id
 *      reg  - reg id
 *      data - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Note:
 *      None
 */
int32
rtk_port_phyReg_set(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyReg_set( port, page, reg, data);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyReg_set */

/* Function Name:
 *      rtk_port_phyMasterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 * Output:
 *      pMasterSlave        - pointer to the PHY master slave configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_phyMasterSlave_get(
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlave)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyMasterSlave_get( port, pMasterSlave);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyMasterSlave_get */

/* Function Name:
 *      rtk_port_phyMasterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - RT_ERR_INPUT
 * Note:
 *      None
 */
int32
rtk_port_phyMasterSlave_set(
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyMasterSlave_set( port, masterSlave);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyMasterSlave_set */

/* Function Name:
 *      rtk_port_cpuPortId_get
 * Description:
 *      Get CPU port id of the specific unit
 * Input:
 *      None
 * Output:
 *      pPort - pointer to CPU port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_cpuPortId_get(rtk_port_t *pPort)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_cpuPortId_get( pPort);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_cpuPortId_get */

/* Function Name:
 *      rtk_port_isolation_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolation_get(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolation_get( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolation_get */

/* Function Name:
 *      rtk_port_isolation_set
 * Description:
 *      Set the portmask of the port isolation
 * Input:
 *      port      - port id
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolation_set(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolation_set( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolation_set */

/* Function Name:
 *      rtk_port_isolationExt_get
 * Description:
 *      Get the portmask of 2nd port isolation configuration
 * Input:
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationExt_get(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationExt_get( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationExt_get */

/* Function Name:
 *      rtk_port_isolationExt_set
 * Description:
 *      Set the portmask of the 2nd port isolation configuration
 * Input:
 *      port      - port id
 *      portmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationExt_set(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationExt_set( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationExt_set */

/* Function Name:
 *      rtk_port_isolationL34_get
 * Description:
 *      Get the portmask of the port isolation
 * Input:
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationL34_get(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationL34_get( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationL34_get */

/* Function Name:
 *      rtk_port_isolationL34_set
 * Description:
 *      Set the portmask of the port isolation
 * Input:
 *      port      - port id
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationL34_set(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationL34_set( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationL34_set */

/* Function Name:
 *      rtk_port_isolationExtL34_get
 * Description:
 *      Get the portmask of 2nd port isolation configuration
 * Input:
 *      port      - port id
 * Output:
 *      pPortmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationExtL34_get(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationExtL34_get( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationExtL34_get */

/* Function Name:
 *      rtk_port_isolationExtL34_set
 * Description:
 *      Set the portmask of the 2nd port isolation configuration
 * Input:
 *      port      - port id
 *      portmask - pointer to the portmask
 *      pExtPortmask - pointer to extension portmask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID   - invalid port id
 *      RT_ERR_PORT_MASK - invalid port mask
 * Note:
 *      (1) Default value of each port is 1
 *      (2) Enable port isolation in the certain ports if relative portmask bits are set to 1
 */
int32
rtk_port_isolationExtL34_set(rtk_port_t port, rtk_portmask_t *pPortmask, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationExtL34_set( port, pPortmask, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationExtL34_set */

/* Function Name:
 *      rtk_port_isolationEntry_get
 * Description:
 *      Get Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 * Output:
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pIncludeCpu     - Include CPU or not
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationEntry_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, uint32 *pIncludeCpu, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationEntry_get( mode, port, pPortmask, pIncludeCpu, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationEntry_get */

/* Function Name:
 *      rtk_port_isolationEntry_set
 * Description:
 *      Set Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      includeCpu      - Include CPU or not
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationEntry_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, uint32 includeCpu, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationEntry_set( mode, port, pPortmask, includeCpu, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationEntry_set */

/* Function Name:
 *      rtk_port_isolationEntryExt_get
 * Description:
 *      Get Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 * Output:
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pIncludeCpu     - Include CPU or not
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationEntryExt_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, uint32 *pIncludeCpu, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationEntryExt_get( mode, port, pPortmask, pIncludeCpu, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationEntryExt_get */

/* Function Name:
 *      rtk_port_isolationEntryExt_set
 * Description:
 *      Set Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      includeCpu      - Include CPU or not
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationEntryExt_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, uint32 includeCpu, rtk_portmask_t *pExtPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationEntryExt_set(mode, port, pPortmask, includeCpu, pExtPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationEntryExt_set */

/* Function Name:
 *      rtk_port_isolationCtagPktConfig_get
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      None
 * Output:
 *      pMode   - Isolation configuration selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationCtagPktConfig_get(rtk_port_isoConfig_t *pMode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationCtagPktConfig_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationCtagPktConfig_get */

/* Function Name:
 *      rtk_port_isolationCtagPktConfig_set
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      mode    - Isolation configuration selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_port_isolationCtagPktConfig_set(rtk_port_isoConfig_t mode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationCtagPktConfig_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationCtagPktConfig_set */

/* Function Name:
 *      rtk_port_isolationL34PktConfig_get
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      None
 * Output:
 *      pMode   - Isolation configuration selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
rtk_port_isolationL34PktConfig_get(rtk_port_isoConfig_t *pMode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationL34PktConfig_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationL34PktConfig_get */

/* Function Name:
 *      rtk_port_isolationL34PktConfig_set
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      mode    - Isolation configuration selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
rtk_port_isolationL34PktConfig_set(rtk_port_isoConfig_t mode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationL34PktConfig_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationL34PktConfig_set */

/* Function Name:
 *      rtk_port_isolationIpmcLeaky_get
 * Description:
 *      Get the ip multicast leaky state of the port isolation
 * Input:
 *      port      - port id
 * Output:
 *      pEnable   - status of port isolation leaky for ip multicast packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_isolationIpmcLeaky_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationIpmcLeaky_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationIpmcLeaky_get */

/* Function Name:
 *      rtk_port_isolationIpmcLeaky_set
 * Description:
 *      Set the ip multicast leaky state of the port isolation
 * Input:
 *      port      - port id
 *      enable    - status of port isolation leaky for ip multicast packets
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
rtk_port_isolationIpmcLeaky_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_isolationIpmcLeaky_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_isolationIpmcLeaky_get */

/* Function Name:
 *      rtk_port_macRemoteLoopbackEnable_get
 * Description:
 *      Get the mac remote loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of mac remote loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac remote loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Remote loopback is used to loopback packet RX to switch core back to the outer interface.
 */
int32
rtk_port_macRemoteLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_macRemoteLoopbackEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_macRemoteLoopbackEnable_get */

/* Function Name:
 *      rtk_port_macRemoteLoopbackEnable_set
 * Description:
 *      Set the mac remote loopback enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of mac remote loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac remote loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Remote loopback is used to loopback packet RX to switch core back to the outer interface.
 */
int32
rtk_port_macRemoteLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_macRemoteLoopbackEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_macRemoteLoopbackEnable_set */

/* Function Name:
 *      rtk_port_macLocalLoopbackEnable_get
 * Description:
 *      Get the mac local loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of mac local loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac local loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Local loopback is used to loopback packet TX from switch core back to switch core.
 */
int32
rtk_port_macLocalLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_macLocalLoopbackEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_macLocalLoopbackEnable_get */

/* Function Name:
 *      rtk_port_macLocalLoopbackEnable_set
 * Description:
 *      Set the mac local loopback enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of mac local loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac local loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Local loopback is used to loopback packet TX from switch core back to switch core.
 */
int32
rtk_port_macLocalLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_macLocalLoopbackEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_macLocalLoopbackEnable_set */

/* Function Name:
 *      rtk_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port admin status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_adminEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_adminEnable_get */

/* Function Name:
 *      rtk_port_adminEnable_set
 * Description:
 *      Set port admin status of the specific port
 * Input:
 *      port    - port id
 *      enable  - port admin status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
rtk_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_adminEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_adminEnable_set */

/* Function Name:
 *      rtk_port_specialCongest_get
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSecond - congest timer (seconds)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_specialCongest_get(rtk_port_t port, uint32 *pSecond)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_specialCongest_get( port, pSecond);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_specialCongest_get */

/* Function Name:
 *      rtk_port_specialCongest_set
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      port   - port id
 *      second - congest timer (seconds)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_port_specialCongest_set(rtk_port_t port, uint32 second)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_specialCongest_set( port, second);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_specialCongest_set */

/* Function Name:
 *      rtk_port_specialCongestStatus_get
 * Description:
 *      Get the congest status of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      pStatus - Congest status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_port_specialCongestStatus_get(rtk_port_t port, uint32 *pStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_specialCongestStatus_get( port, pStatus);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_port_specialCongestStatus_get
 * Description:
 *      Get the congest status of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
rtk_port_specialCongestStatus_clear(rtk_port_t port)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_specialCongestStatus_clear( port);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_port_greenEnable_get
 * Description:
 *      Get the statue of green feature of the specific port in the specific unit
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
rtk_port_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_greenEnable_get( port, pEnable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_greenEnable_get */

/* Function Name:
 *      rtk_port_greenEnable_set
 * Description:
 *      Set the statue of green feature of the specific port in the specific unit
 * Input:
 *      port   - port id
 *      enable - status of green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
rtk_port_greenEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_greenEnable_set( port, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_greenEnable_set */


/* Function Name:
 *      rtk_port_phyCrossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
rtk_port_phyCrossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyCrossOverMode_get( port, pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyCrossOverMode_get */

/* Function Name:
 *      rtk_port_phyCrossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
rtk_port_phyCrossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_phyCrossOverMode_set( port, mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_phyCrossOverMode_set */

/* Function Name:
 *      rtk_port_enhancedFid_get
 * Description:
 *      Get port EFID
 * Input:
 *      port      - port id
 * Output:
 *      pEfid     - EFID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
rtk_port_enhancedFid_get(rtk_port_t port, rtk_efid_t *pEfid)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_enhancedFid_get( port, pEfid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_enhancedFid_get */

/* Function Name:
 *      rtk_port_enhancedFid_set
 * Description:
 *      Set port EFID
 * Input:
 *      port      - port id
 *      efid      - EFID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
rtk_port_enhancedFid_set(rtk_port_t port, rtk_efid_t efid)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_enhancedFid_set( port, efid);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_enhancedFid_set */

/* Function Name:
 *      rtk_port_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      port      - port id
 * Output:
 *      pResult   - Test Result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *      If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
int32
rtk_port_rtctResult_get(rtk_port_t port, rtk_rtctResult_t *pResult)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_rtctResult_get( port, pResult);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_port_rtctResult_get */

/* Function Name:
 *      rtk_port_rtct_start
 * Description:
 *      Start RTCT for ports.
 *      When enable RTCT, the port won't transmit and receive normal traffic.
 * Input:
 *      pPortmask   - the ports for RTCT test
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
rtk_port_rtct_start(rtk_portmask_t *pPortmask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->port_rtct_start( pPortmask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_port_rtct_start */

