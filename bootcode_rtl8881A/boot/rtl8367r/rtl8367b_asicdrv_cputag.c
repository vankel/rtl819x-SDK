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
 * $Revision: 14202 $
 * $Date: 2010-11-16 15:13:00 +0800 (星期二, 16 十一月 2010) $
 *
 * Purpose : RTL8367B switch high-level API for RTL8367B
 * Feature : Proprietary CPU-tag related function drivers
 *
 */
#include <rtl8367b_asicdrv_cputag.h>


/* Function Name:
 *      rtl8367b_setAsicCputagEnable
 * Description:
 *      Set cpu tag function enable/disable
 * Input:
 *      enabled - 1: enabled, 0: disabled
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 *      RT_ERR_ENABLE  	- Invalid enable/disable input
 * Note:
 *      If CPU tag function is disabled, CPU tag will not be added to frame
 *    	forwarded to CPU port, and all ports cannot parse CPU tag.
 */
ret_t rtl8367b_setAsicCputagEnable(rtk_uint32 enabled)
{
    if(enabled > 1)
        return RT_ERR_ENABLE;
    
    return rtl8367b_setAsicRegBit(RTL8367B_REG_CPU_CTRL, RTL8367B_CPU_EN_OFFSET, enabled);
}

/* Function Name:
 *      rtl8367b_setAsicCputagPortmask
 * Description:
 *      Set ports that can parse CPU tag
 * Input:
 *      portmask - port mask
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_PORT_MASK  	- Invalid portmask
 * Note:
 *     None
 */
ret_t rtl8367b_setAsicCputagPortmask(rtk_uint32 portmask)
{
    if(portmask > RTL8367B_PORTMASK)
        return RT_ERR_PORT_MASK;

    return rtl8367b_setAsicReg(RTL8367B_CPU_PORT_MASK_REG, portmask);
}

/* Function Name:
 *      rtl8367b_setAsicCputagTrapPort
 * Description:
 *      Set cpu tag trap port
 * Input:
 *      port - port number
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 		- Success
 *      RT_ERR_SMI  	- SMI access error
 *      RT_ERR_PORT_ID  - Invalid port number
 * Note:
 *     API can set destination port of trapping frame
 */
ret_t rtl8367b_setAsicCputagTrapPort(rtk_uint32 port)
{
    if(port >= RTL8367B_PORTNO)
        return RT_ERR_PORT_ID;

    return rtl8367b_setAsicRegBits(RTL8367B_REG_CPU_CTRL, RTL8367B_CPU_TRAP_PORT_MASK, port);    
}

/* Function Name:
 *      rtl8367b_setAsicCputagInsertMode
 * Description:
 *      Set CPU-tag insert mode
 * Input:
 *      mode - 0: insert to all packets; 1: insert to trapped packets; 2: don't insert
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK 			- Success
 *      RT_ERR_SMI  		- SMI access error
 *      RT_ERR_NOT_ALLOWED  - Actions not allowed by the function
 * Note:
 *     None
 */
ret_t rtl8367b_setAsicCputagInsertMode(rtk_uint32 mode)
{
    if(mode >= CPUTAG_INSERT_END)
        return RT_ERR_NOT_ALLOWED;

    return rtl8367b_setAsicRegBits(RTL8367B_REG_CPU_CTRL, RTL8367B_CPU_INSERTMODE_MASK, mode);
}

