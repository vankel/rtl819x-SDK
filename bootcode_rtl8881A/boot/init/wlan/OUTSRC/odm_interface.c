/******************************************************************************
 *
 * Copyright(c) 2007 - 2011 Realtek Corporation. All rights reserved.
 *                                        
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

//============================================================
// include files
//============================================================

#include "odm_types.h"
#include "odm_interface.h"
#include "../wl_common_hw.h"





//
// ODM IO Relative API.
//

u1Byte
ODM_Read1Byte(
 	IN	u4Byte			RegAddr
	)
{
    return HAL_RTL_R8(RegAddr);
}


u2Byte
ODM_Read2Byte(
 	IN	u4Byte			RegAddr
	)
{
    return HAL_RTL_R16(RegAddr);
}


u4Byte
ODM_Read4Byte(
 	IN	u4Byte			RegAddr
	)
{
    return HAL_RTL_R32(RegAddr);
}


VOID
ODM_Write1Byte(
 	IN	u4Byte			RegAddr,
	IN	u1Byte			Data
	)
{
    HAL_RTL_W8(RegAddr, Data);
}


VOID
ODM_Write2Byte(
 	IN	u4Byte			RegAddr,
	IN	u2Byte			Data
	)
{

    HAL_RTL_W16(RegAddr, Data);

}


VOID
ODM_Write4Byte(
 	IN	u4Byte			RegAddr,
	IN	u4Byte			Data
	)
{
    HAL_RTL_W8(RegAddr, Data);

}


VOID
ODM_SetMACReg(	
 	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask,
	IN	u4Byte		Data
	)
{
//    PHY_SetBBReg(RegAddr, BitMask, Data);
    ODM_Write4Byte(RegAddr, Data);
}


u4Byte 
ODM_GetMACReg(	
 	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask
	)
{

//    return PHY_QueryBBReg(RegAddr, BitMask);
    return ODM_Read4Byte(RegAddr);
}


VOID
ODM_SetBBReg(	
 	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask,
	IN	u4Byte		Data
	)
{
#if 1
    PHY_SetBBReg(RegAddr, BitMask, Data);
#else
#endif
}


u4Byte 
ODM_GetBBReg(	
 	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask
	)
{
#if 1
    return PHY_QueryBBReg(RegAddr, BitMask);
#else
    return 0;
#endif
}


VOID
ODM_SetRFReg(	
    IN  u4Byte              RfPath,
 	IN	u4Byte				RegAddr,
	IN	u4Byte				BitMask,
	IN	u4Byte				Data
	)
{
#if 1
    PHY_SetRFReg(RfPath,RegAddr, BitMask, Data);
#else
#endif
}


u4Byte 
ODM_GetRFReg(
    IN  u4Byte              RfPath,
 	IN	u4Byte				RegAddr,
	IN	u4Byte				BitMask
	)
{
#if 1
    return PHY_QueryRFReg(RfPath, RegAddr, BitMask, 1);
#else
    return 0;
#endif
}



VOID
ODM_delay_ms(IN u4Byte	ms)
{

    delay_ms(ms);
}

VOID
ODM_delay_us(IN u4Byte	us)
{
    delay_us(us);
}

