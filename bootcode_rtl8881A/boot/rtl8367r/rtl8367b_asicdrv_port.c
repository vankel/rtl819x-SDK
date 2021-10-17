/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 20646 $
 * $Date: 2011-08-09 14:53:37 +0800 (?Ÿæ?äº? 09 ?«æ? 2011) $
 *
 * Purpose : RTL8367B switch high-level API for RTL8367B
 * Feature : Port security related functions
 *
 */

#include <rtl8367b_asicdrv_port.h>

#include <string.h>

/* Function Name:
 *      rtl8367b_setAsicPortForceLinkExt
 * Description:
 *      Set external interface force linking configuration
 * Input:
 *      id 			- external interface id (0~2)
 *      portAbility - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */ 
ret_t rtl8367b_setAsicPortForceLinkExt(rtk_uint32 id, rtl8367b_port_ability_t *pPortAbility)
{
    rtk_uint32  reg_data;

    /* Invalid input parameter */
    if(id >= RTL8367B_EXTNO)
        return RT_ERR_OUT_OF_RANGE;

    reg_data = (rtk_uint32)(*(rtk_uint16 *)pPortAbility);

	if(0 == id || 1 == id)
	    return rtl8367b_setAsicReg(RTL8367B_REG_DIGITAL_INTERFACE0_FORCE + id, reg_data);
	else
	    return rtl8367b_setAsicReg(RTL8367B_REG_DIGITAL_INTERFACE2_FORCE, reg_data);
}
/* Function Name:
 *      rtl8367b_getAsicPortForceLinkExt
 * Description:
 *      Get external interface force linking configuration
 * Input:
 *      id 			- external interface id (0~1)
 *      pPortAbility - port ability configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
ret_t rtl8367b_getAsicPortForceLinkExt(rtk_uint32 id, rtl8367b_port_ability_t *pPortAbility)
{
    rtk_uint32  reg_data;
    rtk_uint16  ability_data;
    ret_t       retVal;

    /* Invalid input parameter */
    if(id >= RTL8367B_EXTNO)
        return RT_ERR_OUT_OF_RANGE;

	if(0 == id || 1 == id)
    	retVal = rtl8367b_getAsicReg(RTL8367B_REG_DIGITAL_INTERFACE0_FORCE+id, &reg_data);
	else
	    retVal = rtl8367b_getAsicReg(RTL8367B_REG_DIGITAL_INTERFACE2_FORCE, &reg_data);

    if(retVal != RT_ERR_OK)
        return retVal;

    ability_data = (rtk_uint16)reg_data;
    memcpy(pPortAbility, &ability_data, sizeof(rtl8367b_port_ability_t));
    return RT_ERR_OK;
}
/* Function Name:
 *      rtl8367b_setAsicPortExtMode
 * Description:
 *      Set external interface mode configuration
 * Input:
 *      id 		- external interface id (0~2)
 *      mode 	- external interface mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_OUT_OF_RANGE - input parameter out of range
 * Note:
 *      None
 */
ret_t rtl8367b_setAsicPortExtMode(rtk_uint32 id, rtk_uint32 mode)
{
    ret_t   retVal;

    if(id >= RTL8367B_EXTNO)
        return RT_ERR_OUT_OF_RANGE;

    if(mode >= EXT_END)
        return RT_ERR_OUT_OF_RANGE;

    if(mode == EXT_GMII)
    {
        if( (retVal = rtl8367b_setAsicRegBit(RTL8367B_REG_EXT0_RGMXF, RTL8367B_EXT0_RGTX_INV_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367b_setAsicRegBit(RTL8367B_REG_EXT1_RGMXF, RTL8367B_EXT1_RGTX_INV_OFFSET, 1)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367b_setAsicRegBits(RTL8367B_REG_EXT_TXC_DLY, RTL8367B_EXT1_GMII_TX_DELAY_MASK, 5)) != RT_ERR_OK)
            return retVal;

        if( (retVal = rtl8367b_setAsicRegBits(RTL8367B_REG_EXT_TXC_DLY, RTL8367B_EXT0_GMII_TX_DELAY_MASK, 6)) != RT_ERR_OK)
            return retVal;
    }

    if( (mode == EXT_TMII_MAC) || (mode == EXT_TMII_PHY) )
    {
        if( (retVal = rtl8367b_setAsicRegBit(RTL8367B_REG_BYPASS_LINE_RATE, id, 1)) != RT_ERR_OK)
            return retVal;
    }
    else
    {
        if( (retVal = rtl8367b_setAsicRegBit(RTL8367B_REG_BYPASS_LINE_RATE, id, 0)) != RT_ERR_OK)
            return retVal;
    }

	if(0 == id || 1 == id)
   		return rtl8367b_setAsicRegBits(RTL8367B_REG_DIGITAL_INTERFACE_SELECT, RTL8367B_SELECT_GMII_0_MASK << (id * RTL8367B_SELECT_GMII_1_OFFSET), mode);
	else
   		return rtl8367b_setAsicRegBits(RTL8367B_REG_DIGITAL_INTERFACE_SELECT_1, RTL8367B_SELECT_RGMII_2_MASK, mode);
}
 
