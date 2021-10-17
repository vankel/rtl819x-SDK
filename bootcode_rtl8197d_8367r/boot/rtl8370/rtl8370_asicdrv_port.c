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
 * $Revision: 23354 $
 * $Date: 2011-09-27 18:29:01 +0800 (?üÊ?‰∫? 27 ‰πùÊ? 2011) $
 *
 * Purpose : RTL8370 switch high-level API for RTL8367B
 * Feature : 
 *
 */

#include <rtl8370_asicdrv_port.h>
#include <rtl8370_asicdrv_phy.h>


/*
@func ret_t | rtl8370_setAsicPortForceLinkExt | Set external interface force linking configuration.
@parm uint32 | id | external interface id (0~1).
@parm rtl8370_port_ability_t* | portability | port ability configuration
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error.
@rvalue RT_ERR_FAILED | Invalid parameter.
@comm
      This API can set external interface force mode properties. 
 */
ret_t rtl8370_setAsicPortForceLinkExt(uint32 id, rtl8370_port_ability_t *portability)
{
    uint32 regData;
    uint16 *accessPtr;
    rtl8370_port_ability_t ability;

    /* Invalid input parameter */
    if(id >=RTL8370_EXTNO)
        return RT_ERR_PORT_ID;
    
    memset(&ability, 0x00, sizeof(rtl8370_port_ability_t));
    memcpy(&ability, portability, sizeof(rtl8370_port_ability_t));    

    accessPtr = (uint16*)&ability;
 
    regData = *accessPtr;

    return rtl8370_setAsicReg(RTL8370_REG_DIGITIAL_INTERFACE0_FORCE + id, regData);
}

/*
@func ret_t | rtl8370_getAsicPortForceLinkExt | Get external interface force linking configuration.
@parm uint32 | id | external interface id (0~1).
@parm rtl8370_port_ability_t* | portability | port ability configuration
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error.
@rvalue RT_ERR_FAILED | Invalid parameter.
@comm
      This API can get external interface force mode properties. 
 */
ret_t rtl8370_getAsicPortForceLinkExt(uint32 id, rtl8370_port_ability_t *portability)
{
    ret_t retVal;
    uint32 regData;
    uint16 *accessPtr;
    rtl8370_port_ability_t ability;

    /* Invalid input parameter */
    if(id >= RTL8370_EXTNO)
        return RT_ERR_PORT_ID;
    
    memset(&ability,0x00,sizeof(rtl8370_port_ability_t));


    accessPtr =  (uint16*)&ability;
 
    retVal = rtl8370_getAsicReg(RTL8370_REG_DIGITIAL_INTERFACE0_FORCE + id, &regData);
    if(retVal !=  RT_ERR_OK)
        return retVal;
    
    *accessPtr = regData;

    memcpy(portability, &ability, sizeof(rtl8370_port_ability_t));        
    
    return RT_ERR_OK;  
}

/*
@func ret_t | rtl8370_setAsicPortExtMode | Set external interface mode configuration.
@parm uint32 | id | external interface id (0~1).
@parm uint32 |mode | external interface mode.
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error.
@rvalue RT_ERR_FAILED | Invalid parameter.
@comm
  This API can set external interface mode properties. 
    DISABLE,
    RGMII,
    MII_MAC,
    MII_PHY, 
    TMII_MAC,
    TMII_PHY, 
    GMII,
    RGMII_33V,    
 */
ret_t rtl8370_setAsicPortExtMode(uint32 id, uint32 mode)
{
    ret_t retVal;
    
    if(id >= RTL8370_EXTNO)
        return RT_ERR_INPUT;	

    if(mode > EXT_RGMII_33V)
        return RT_ERR_INPUT;	

    if( mode == EXT_RGMII_33V || mode == EXT_RGMII )
    {
        if((retVal= rtl8370_setAsicReg(RTL8370_REG_CHIP_DEBUG0,0x0367)) !=  RT_ERR_OK)
            return retVal;
        if((retVal= rtl8370_setAsicReg(RTL8370_REG_CHIP_DEBUG1,0x7777)) !=  RT_ERR_OK)
            return retVal;
    }
    else if((mode == EXT_TMII_MAC)||(mode == EXT_TMII_PHY))
    {
        if((retVal= rtl8370_setAsicRegBit(RTL8370_REG_BYPASS_LINE_RATE,(id+1)%2,1)) !=  RT_ERR_OK)
            return retVal;
    } 
    else if( mode == EXT_GMII )
    {
        if((retVal= rtl8370_setAsicRegBit(RTL8370_REG_CHIP_DEBUG0,RTL8370_CHIP_DEBUG0_DUMMY_0_OFFSET+id,1)) !=  RT_ERR_OK)
            return retVal;
        if((retVal= rtl8370_setAsicRegBit(RTL8370_REG_EXT0_RGMXF+id,6,1)) !=  RT_ERR_OK)
            return retVal;        
    } 
    else 
    {
        if((retVal= rtl8370_setAsicRegBit(RTL8370_REG_BYPASS_LINE_RATE,(id+1)%2,0)) !=  RT_ERR_OK)
            return retVal;
        if((retVal= rtl8370_setAsicRegBit(RTL8370_REG_EXT0_RGMXF+id,6,0)) !=  RT_ERR_OK)
            return retVal;     
    }      

    return rtl8370_setAsicRegBits(RTL8370_REG_DIGITIAL_INTERFACE_SELECT, RTL8370_SELECT_RGMII_0_MASK<<(id*RTL8370_SELECT_RGMII_1_OFFSET), mode);
}

/*
@func ret_t | rtl8370_setAsicLutLearnLimitNo | Set per-Port auto learning limit number
@parm uint32 | port | The port number
@parm uint32 | number | ASIC auto learning entries limit number
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error.
@rvalue RT_ERR_PORT_ID | Invalid port number.
@rvalue RT_ERR_LIMITED_L2ENTRY_NUM | Invalid auto learning limit number
@common
    The API can set per-port ASIC auto learning limit number
*/

#define RTL8370_LUT_LEARNLIMITMAX   0x2040

ret_t rtl8370_setAsicLutLearnLimitNo(uint32 port,uint32 number)
{
    if(port > RTL8370_PORTIDMAX)
        return RT_ERR_PORT_ID;

    if(number > RTL8370_LUT_LEARNLIMITMAX)
        return RT_ERR_LIMITED_L2ENTRY_NUM;

    return rtl8370_setAsicReg(RTL8370_LUT_PORT_LEARN_LIMITNO_REG(port), number);
}

