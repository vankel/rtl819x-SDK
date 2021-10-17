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


#ifndef	__ODM_INTERFACE_H__
#define __ODM_INTERFACE_H__

#include "odm_types.h"

//
// =========== Constant/Structure/Enum/... Define
//



//
// =========== Macro Define
//

#define _reg_all(_name)			ODM_##_name
#define _reg_ic(_name, _ic)		ODM_##_name##_ic
#define _bit_all(_name)			BIT_##_name
#define _bit_ic(_name, _ic)		BIT_##_name##_ic

// _cat: implemented by Token-Pasting Operator.
#if 0
#define _cat(_name, _ic_type, _func)								\
	( 															\
		_func##_all(_name)										\
	)
#endif

/*===================================

#define ODM_REG_DIG_11N		0xC50
#define ODM_REG_DIG_11AC	0xDDD

ODM_REG(DIG,_pDM_Odm)
=====================================*/

#define _reg_11N(_name)			ODM_REG_##_name##_11N 
#define _reg_11AC(_name)		ODM_REG_##_name##_11AC
#define _bit_11N(_name)			ODM_BIT_##_name##_11N 
#define _bit_11AC(_name)		ODM_BIT_##_name##_11AC

#if 1 //TODO: enable it if we need to support run-time to differentiate between 92C_SERIES and JAGUAR_SERIES.
#define _cat(_name, _ic_type, _func)									\
	( 															\
		((_ic_type) & ODM_IC_11N_SERIES)? _func##_11N(_name):		\
		_func##_11AC(_name)									\
	)
#endif
#if 0 // only sample code
#define _cat(_name, _ic_type, _func)									\
	( 															\
		((_ic_type) & ODM_RTL8192C)? _func##_ic(_name, _8192C):		\
		((_ic_type) & ODM_RTL8192D)? _func##_ic(_name, _8192D):		\
		((_ic_type) & ODM_RTL8192S)? _func##_ic(_name, _8192S):		\
		((_ic_type) & ODM_RTL8723A)? _func##_ic(_name, _8723A):		\
		((_ic_type) & ODM_RTL8188E)? _func##_ic(_name, _8188E):		\
		_func##_ic(_name, _8195)									\
	)
#endif

// _name: name of register or bit.
// Example: "ODM_REG(R_A_AGC_CORE1, pDM_Odm)" 
//        gets "ODM_R_A_AGC_CORE1" or "ODM_R_A_AGC_CORE1_8192C", depends on SupportICType.
#define ODM_REG(_name, _pDM_Odm)	_cat(_name, _pDM_Odm->SupportICType, _reg)
#define ODM_BIT(_name, _pDM_Odm)	_cat(_name, _pDM_Odm->SupportICType, _bit)



//
// =========== Extern Variable ??? It should be forbidden.
//


//
// =========== EXtern Function Prototype
//


u1Byte
ODM_Read1Byte(
 	IN	u4Byte			RegAddr
	);

u2Byte
ODM_Read2Byte(
 	IN	u4Byte			RegAddr
	);

u4Byte
ODM_Read4Byte(
 	IN	u4Byte			RegAddr
	);

VOID
ODM_Write1Byte(
 	IN	u4Byte			RegAddr,
	IN	u1Byte			Data
	);

VOID
ODM_Write2Byte(
 	IN	u4Byte			RegAddr,
	IN	u2Byte			Data
	);

VOID
ODM_Write4Byte(
 	IN	u4Byte			RegAddr,
	IN	u4Byte			Data
	);

VOID
ODM_SetMACReg(	
 	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask,
	IN	u4Byte		Data
	);

u4Byte 
ODM_GetMACReg(	
 	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask
	);

VOID
ODM_SetBBReg(	
 	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask,
	IN	u4Byte		Data
	);

u4Byte 
ODM_GetBBReg(	
 	IN	u4Byte		RegAddr,
	IN	u4Byte		BitMask
	);

VOID
ODM_SetRFReg(	
    IN  u4Byte              RfPath,
 	IN	u4Byte				RegAddr,
	IN	u4Byte				BitMask,
	IN	u4Byte				Data
	);

u4Byte 
ODM_GetRFReg(	
 	IN	u4Byte				RfPath,
 	IN	u4Byte				RegAddr,
	IN	u4Byte				BitMask
	);

VOID
ODM_delay_ms(IN u4Byte	ms);



VOID
ODM_delay_us(IN u4Byte	us);



#endif	// __ODM_INTERFACE_H__

