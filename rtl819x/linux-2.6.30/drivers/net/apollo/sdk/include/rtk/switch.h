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
 * $Revision: 15045 $
 * $Date: 2010-12-27 18:36:22 +0800 (?��?一, 27 ?��???2010) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

#ifndef __RTK_SWITCH_H__
#define __RTK_SWITCH_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>

/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */

/* information of device */
typedef struct rtk_switch_devInfo_s
{
    uint32  chipId;
    uint32  revision;
    uint32  port_number;
    rt_portType_info_t fe;
    rt_portType_info_t ge;
    rt_portType_info_t ge_combo;
    rt_portType_info_t serdes;
    rt_portType_info_t ether;
    rt_portType_info_t dsl;
    rt_portType_info_t ext;
    rt_portType_info_t all;
    rt_register_capacity_t  capacityInfo;
    int32   cpuPort; /* use (-1) for VALUE_NO_INIT */
} rtk_switch_devInfo_t;



/* information of device */
typedef enum rtk_switch_port_name_e
{
    /*normal UTP port*/
    RTK_PORT_UTP0 = 0,
    RTK_PORT_UTP1,
    RTK_PORT_UTP2,
    RTK_PORT_UTP3,
    RTK_PORT_UTP4,
    RTK_PORT_UTP5,
    RTK_PORT_UTP6,
    RTK_PORT_UTP7,
    RTK_PORT_UTP8,
    RTK_PORT_UTP9,
    RTK_PORT_UTP10,
    RTK_PORT_UTP11 = 63,
    
    /*PON port*/
    RTK_PORT_PON = 128,

    /*Fiber port*/
    RTK_PORT_FIBER = 256,

    /* physical extention port*/
    RTK_PORT_EXT0 = 512,
    RTK_PORT_EXT1,
    RTK_PORT_EXT2,

    /*CPU port*/
    RTK_PORT_CPU = 1024,
    RTK_PORT_NAME_END
} rtk_switch_port_name_t;



/* type of the acceptable packet length */
typedef enum rtk_switch_maxPktLen_e
{
    MAXPKTLEN_1522B = 0,
    MAXPKTLEN_1536B,
    MAXPKTLEN_1552B,
    MAXPKTLEN_9216B,
    MAXPKTLEN_END
} rtk_switch_maxPktLen_t;

/* type of checksum fail */
typedef enum rtk_switch_chksum_fail_e
{
    LAYER2_CHKSUM_FAIL = 0,
    LAYER3_CHKSUM_FAIL,
    LAYER4_CHKSUM_FAIL,
    CHKSUM_FAIL_END
} rtk_switch_chksum_fail_t;

/* type of HW delay */
typedef enum rtk_switch_delayType_e
{
    DELAY_TYPE_INTRA_LINK0_RX = 0,
    DELAY_TYPE_INTRA_LINK0_TX,
    DELAY_TYPE_INTRA_LINK1_RX,
    DELAY_TYPE_INTRA_LINK1_TX,
    DELAY_TYPE_END
} rtk_switch_delayType_t;


/* type of the acceptable packet length in different link speed */ 
typedef enum rtk_switch_maxPktLen_linkSpeed_e {
     MAXPKTLEN_LINK_SPEED_FE = 0,
     MAXPKTLEN_LINK_SPEED_GE,
     MAXPKTLEN_LINK_SPEED_END,
} rtk_switch_maxPktLen_linkSpeed_t;

/*
 * Function Declaration
 */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rtk_switch_init
 * Description:
 *      Initialize switch module of the specified device.
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
extern int32
rtk_switch_init(void);

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      rtk_switch_deviceInfo_get
 * Description:
 *      Get device information of the specific unit
 * Input:
 *      none
 * Output:
 *      pDevInfo - pointer to the device information
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 *      None
 */
extern int32
rtk_switch_deviceInfo_get(rtk_switch_devInfo_t *pDevInfo);


/* Function Name:
 *      rtk_switch_phyPortId_get
 * Description:
 *      Get physical port id from logical port name
 * Input:
 *      portName - logical port name
 * Output:
 *      pPortId  - pointer to the physical port id
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 *      Call RTK API the port ID must get from this API
 */
extern int32
rtk_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId);


/* Function Name:
 *      rtk_switch_logicalPort_get
 * Description:
 *      Get logical port name from physical port id
 * Input:
 *      portId  - physical port id
 * Output:
 *      pPortName - pointer to logical port name
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 */
extern int32
rtk_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName);


/* Function Name:
 *      rtk_switch_port2PortMask_set
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 *      Call RTK API the port mask must set by this API
 */
extern int32
rtk_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);



/* Function Name:
 *      rtk_switch_port2PortMask_set
 * Description:
 *      Set port id to the portlist
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 *      Call RTK API the port mask must set by this API
 */
extern int32
rtk_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);



/* Function Name:
 *      rtk_switch_portIdInMask_check
 * Description:
 *      Check if given port is in port list
 * Input:
 *      pPortMask    - port mask
 *      portName     - logical port name
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK 
 *      RT_ERR_FAILED  
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer 
 * Note:
 */
extern int32
rtk_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName);


/* Function Name:
  *      rtk_switch_maxPktLenLinkSpeed_get
  * Description:
  *      Get the max packet length setting of the specific speed type
  * Input:
  *      speed - speed type
  * Output:
  *      pLen  - pointer to the max packet length
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_NULL_POINTER - input parameter may be null pointer
  *      RT_ERR_INPUT        - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
extern int32
rtk_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen);

/* Function Name:
  *      rtk_switch_maxPktLenLinkSpeed_set
  * Description:
  *      Set the max packet length of the specific speed type
  * Input:
  *      speed - speed type
  *      len   - max packet length
  * Output:
  *      None
  * Return:
  *      RT_ERR_OK
  *      RT_ERR_FAILED
  *      RT_ERR_INPUT   - invalid enum speed type
  * Note:
  *      Max packet length setting speed type
  *      - MAXPKTLEN_LINK_SPEED_FE
  *      - MAXPKTLEN_LINK_SPEED_GE
  */
extern int32
rtk_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len);


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */


/* Function Name:
 *      rtk_switch_mgmtMacAddr_get
 * Description:
 *      Get MAC address of switch.
 * Input:
 *      None
 * Output:
 *      pMac - pointer to MAC address
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_switch_mgmtMacAddr_get(rtk_mac_t *pMac);

/* Function Name:
 *      rtk_switch_mgmtMacAddr_set
 * Description:
 *      Set MAC address of switch.
 * Input:
 *      pMac - MAC address
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_switch_mgmtMacAddr_set(rtk_mac_t *pMac);


#endif /* __RTK_SWITCH_H__ */

