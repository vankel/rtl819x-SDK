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
 * $Date: 2010-12-27 18:36:22 +0800 (星期一, 27 十二月 2010) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */


/*
 * Include Files
 */
#include <rtk/switch.h>

#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_switch.h>
#include <dal/apollomp/raw/apollomp_raw_switch.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32    switch_init = INIT_NOT_COMPLETED; 
/*
 * Function Declaration
 */

static int32
_dal_apollomp_switch_ponAsicPhyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    switch(portName)
    {
        case RTK_PORT_UTP0:    
            *pPortId = 0;
            break;
        case RTK_PORT_UTP1:    
            *pPortId = 1;
            break;
        case RTK_PORT_UTP2:    
            *pPortId = 2;
            break;
        case RTK_PORT_UTP3:    
            *pPortId = 3;
            break;
        case RTK_PORT_EXT0:    
            *pPortId = 5;
            break;
        case RTK_PORT_PON:    
            *pPortId = 4;
            break;
        case RTK_PORT_CPU:    
            *pPortId = 6;
            break;

        default:
            return RT_ERR_INPUT;        
    }
    return RT_ERR_OK;
}




static int32
_dal_apollomp_switch_ponAsicLogicalPortName_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    switch(portId)
    {
        case 0:    
            *pPortName = RTK_PORT_UTP0;
            break;
        case 1:    
            *pPortName = RTK_PORT_UTP1;
            break;
        case 2:    
            *pPortName = RTK_PORT_UTP2;
            break;
        case 3:    
            *pPortName = RTK_PORT_UTP3;
            break;
        case 5:    
            *pPortName = RTK_PORT_EXT0;
            break;
        case 4:    
            *pPortName = RTK_PORT_PON;
            break;
        case 6:    
            *pPortName = RTK_PORT_CPU;
            break;

        default:
            return RT_ERR_INPUT;        
    }
    return RT_ERR_OK;
}
/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */

/* Function Name:
 *      dal_apollomp_switch_init
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
int32
dal_apollomp_switch_init(void)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    switch_init = INIT_COMPLETED;
    
    /* create semaphore */
    
    return RT_ERR_OK;
} /* end of dal_apollomp_switch_init */

/* Module Name    : Switch     */
/* Sub-module Name: Switch parameter settings */


/* Function Name:
 *      dal_apollomp_switch_phyPortId_get
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
int32
dal_apollomp_switch_phyPortId_get(rtk_switch_port_name_t portName, int32 *pPortId)
{
    dal_apollomp_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortId), RT_ERR_NULL_POINTER);

    /*get chip info to check port name mapping*/
    asicType = APOLLOMP_ASIC_PON;
    
    switch(asicType)
    {
        case APOLLOMP_ASIC_PON:
        default:
            return _dal_apollomp_switch_ponAsicPhyPortId_get(portName,pPortId); 
            break;    
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_phyPortId_get */



/* Function Name:
 *      dal_apollomp_switch_logicalPort_get
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
int32
dal_apollomp_switch_logicalPort_get(int32 portId, rtk_switch_port_name_t *pPortName)
{
    dal_apollomp_switch_asic_type_t asicType;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortName), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(portId), RT_ERR_PORT_ID);

    /*get chip info to check port name mapping*/
    asicType = APOLLOMP_ASIC_PON;
    
    switch(asicType)
    {
        case APOLLOMP_ASIC_PON:
        default:
            return _dal_apollomp_switch_ponAsicLogicalPortName_get(portId,pPortName); 
            break;    
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_logicalPort_get */



/* Function Name:
 *      dal_apollomp_switch_port2PortMask_set
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
int32
dal_apollomp_switch_port2PortMask_set(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32 portId;
    int32   ret;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_apollomp_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;
    
    RTK_PORTMASK_PORT_SET(*pPortMask,portId);    
        
    return RT_ERR_OK;
} /* end of dal_apollomp_switch_port2PortMask_set */



/* Function Name:
 *      dal_apollomp_switch_port2PortMask_clear
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
int32
dal_apollomp_switch_port2PortMask_clear(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_apollomp_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;
    
    RTK_PORTMASK_PORT_CLEAR(*pPortMask, portId);

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_port2PortMask_clear */



/* Function Name:
 *      dal_apollomp_switch_portIdInMask_check
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
int32
dal_apollomp_switch_portIdInMask_check(rtk_portmask_t *pPortMask, rtk_switch_port_name_t portName)
{
    int32   ret;
    int32 portId;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortMask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_PORT_NAME_END <= portName), RT_ERR_INPUT);

    if((ret=dal_apollomp_switch_phyPortId_get(portName,&portId))!=RT_ERR_OK)
        return ret;

    if(RTK_PORTMASK_IS_PORT_SET(*pPortMask,portId))
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;
        
} /* end of dal_apollomp_switch_portIdInMask_check */


/* Function Name:
  *      dal_apollomp_switch_maxPktLenLinkSpeed_get
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
int32
dal_apollomp_switch_maxPktLenLinkSpeed_get(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 *pLen)
{
    int32   ret;
	uint32 regAddr;
    uint32 fieldIdx;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pLen), RT_ERR_NULL_POINTER);
    
    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
	    regAddr =  APOLLOMP_MAX_LENGTH_CFG0r;
        fieldIdx = APOLLOMP_ACCEPT_MAX_LENTH_CFG0f;  
    }
    else
    {
	    regAddr =  APOLLOMP_MAX_LENGTH_CFG1r;
        fieldIdx = APOLLOMP_ACCEPT_MAX_LENTH_CFG1f;  
    }    

    if ((ret = reg_field_read(regAddr, fieldIdx, pLen)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
    

    return RT_ERR_OK;
} /* end of dal_apollomp_switch_maxPktLenLinkSpeed_get */

/* Function Name:
  *      dal_apollomp_switch_maxPktLenLinkSpeed_set
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
int32
dal_apollomp_switch_maxPktLenLinkSpeed_set(rtk_switch_maxPktLen_linkSpeed_t speed, uint32 len)
{
    rtk_port_t  port, max_port;
    int32   ret;
	uint32 regAddr;
    uint32 fieldIdx;
    uint32 portField;
    uint32 index;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((MAXPKTLEN_LINK_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_PACEKT_LENGTH_MAX < len), RT_ERR_INPUT);

    if(speed == MAXPKTLEN_LINK_SPEED_FE)
    {
	    regAddr =  APOLLOMP_MAX_LENGTH_CFG0r;
        fieldIdx = APOLLOMP_ACCEPT_MAX_LENTH_CFG0f;  
        portField = APOLLOMP_MAX_LENGTH_10_100f;
        index = 0;
    }
    else
    {
	    regAddr   = APOLLOMP_MAX_LENGTH_CFG1r;
        fieldIdx  = APOLLOMP_ACCEPT_MAX_LENTH_CFG1f;  
        portField = APOLLOMP_MAX_LENGTH_GIGAf;
        index = 1;
    }    

    if ((ret = reg_field_write(regAddr, fieldIdx, &len)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
        return ret;
    }
 
    /*set all port index to currect length index*/
    max_port = HAL_GET_MAX_PORT();
    for (port = 0; port < max_port; port++)
    { 
        if (!HAL_IS_PORT_EXIST(port))
        {
            continue;
        }

        if ((ret = reg_array_field_write(APOLLOMP_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, portField, &index)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
            return ret;
        }
    }
    return RT_ERR_OK;    
} /* end of dal_apollomp_switch_maxPktLenLinkSpeed_set */


/* Module Name    : Switch     */
/* Sub-module Name: Management address and vlan configuration */


/* Function Name:
 *      dal_apollomp_switch_mgmtMacAddr_get
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
int32
dal_apollomp_switch_mgmtMacAddr_get(rtk_mac_t *pMac)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);
    
    if ((ret = apollomp_raw_switch_macAddr_get(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    
    return RT_ERR_OK;
} /* end of dal_apollomp_switch_mgmtMacAddr_get */

/* Function Name:
 *      dal_apollomp_switch_mgmtMacAddr_set
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
int32
dal_apollomp_switch_mgmtMacAddr_set(rtk_mac_t *pMac)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_SWITCH),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(switch_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pMac), RT_ERR_NULL_POINTER);

    if ((ret = apollomp_raw_switch_macAddr_set(pMac)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_SWITCH|MOD_DAL), "");
        return ret;
    }
    
    return RT_ERR_OK;
} /* end of dal_apollomp_switch_mgmtMacAddr_set */



