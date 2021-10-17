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

#include "../odm_types.h"
#include "../odm_interface.h"
#include "odm_RegConfig8821A.h"


#define		bRFRegOffsetMask						0xfffff


void
odm_ConfigRFReg_8821A(
	IN 	u4Byte 					Addr,
	IN 	u4Byte 					Data,
	IN  ODM_RF_RADIO_PATH_E     RF_PATH,
	IN	u4Byte				    RegAddr
	)
{
    if(Addr == 0xffe)
	{ 					  
		delay_ms(50);
	}
	else if (Addr == 0xfd)
	{
		delay_ms(5);
	}
	else if (Addr == 0xfc)
	{
		delay_ms(1);
	}
	else if (Addr == 0xfb)
	{
//		ODM_delay_us(50);
        delay_ms(1);
	}
	else if (Addr == 0xfa)
	{
//		ODM_delay_us(5);
        delay_ms(1);
	}
	else if (Addr == 0xf9)
	{
//		ODM_delay_us(1);
        delay_ms(1);
	}
	else
	{
		ODM_SetRFReg(RF_PATH, RegAddr, bRFRegOffsetMask, Data);
		// Add 1us delay between BB/RF register setting.
		ODM_delay_us(1);
	}	
}


void 
odm_ConfigRF_RadioA_8821A(
	IN 	u4Byte 					Addr,
	IN 	u4Byte 					Data
	)
{
	u4Byte  content = 0x1000; // RF_Content: radioa_txt
	u4Byte	maskforPhySet= (u4Byte)(content&0xE000);

    odm_ConfigRFReg_8821A(Addr, Data, ODM_RF_PATH_A, Addr|maskforPhySet);

//    dprintf("===> ODM_ConfigRFWithHeaderFile: [RadioA] %08X %08X\n", Addr, Data);
}

void 
odm_ConfigRF_RadioB_8821A(
	IN 	u4Byte 					Addr,
	IN 	u4Byte 					Data
	)
{
	u4Byte  content = 0x1001; // RF_Content: radiob_txt
	u4Byte	maskforPhySet= (u4Byte)(content&0xE000);

    odm_ConfigRFReg_8821A(Addr, Data, ODM_RF_PATH_B, Addr|maskforPhySet);
	
	dprintf("===> ODM_ConfigRFWithHeaderFile: [RadioB] %08X %08X\n", Addr, Data);
    
}

void 
odm_ConfigMAC_8821A(
 	IN 	u4Byte 		Addr,
 	IN 	u1Byte 		Data
 	)
{
	ODM_Write1Byte(Addr, Data);
//    dprintf("===> ODM_ConfigMACWithHeaderFile: [MAC_REG] %08X %08X\n", Addr, Data);
}

void 
odm_ConfigBB_AGC_8821A(
    IN 	u4Byte 		Addr,
    IN 	u4Byte 		Bitmask,
    IN 	u4Byte 		Data
    )
{
	ODM_SetBBReg(Addr, Bitmask, Data);		
	// Add 1us delay between BB/RF register setting.
	ODM_delay_us(1);

//    dprintf("===> ODM_ConfigBBWithHeaderFile: [AGC_TAB] %08X %08X\n", Addr, Data);
}

void
odm_ConfigBB_PHY_REG_PG_8821A(
    IN 	u4Byte 		Addr,
    IN 	u4Byte 		Bitmask,
    IN 	u4Byte 		Data
    )
{    
	if (Addr == 0xfe)
		ODM_delay_ms(50);
	else if (Addr == 0xfd)
		ODM_delay_ms(5);
	else if (Addr == 0xfc)
		ODM_delay_ms(1);
	else if (Addr == 0xfb)
		ODM_delay_us(50);
	else if (Addr == 0xfa)
		ODM_delay_us(5);
	else if (Addr == 0xf9)
		ODM_delay_us(1);

	dprintf("===> @@@@@@@ ODM_ConfigBBWithHeaderFile: [PHY_REG] %08X %08X %08X\n", Addr, Bitmask, Data);

//#if	!(DM_ODM_SUPPORT_TYPE&ODM_AP)
//	storePwrIndexDiffRateOffset(pDM_Odm->Adapter, Addr, Bitmask, Data);
//#endif

}

void 
odm_ConfigBB_PHY_8821A(
    IN 	u4Byte 		Addr,
    IN 	u4Byte 		Bitmask,
    IN 	u4Byte 		Data
    )
{    
	if (Addr == 0xfe)
		ODM_delay_ms(50);
	else if (Addr == 0xfd)
		ODM_delay_ms(5);
	else if (Addr == 0xfc)
		ODM_delay_ms(1);
	else if (Addr == 0xfb)
		ODM_delay_us(50);
	else if (Addr == 0xfa)
		ODM_delay_us(5);
	else if (Addr == 0xf9)
		ODM_delay_us(1);
	else if (Addr == 0xa24) {
        //      pDM_Odm->RFCalibrateInfo.RegA24 = Data;         
	}
	ODM_SetBBReg(Addr, Bitmask, Data);		
	
	// Add 1us delay between BB/RF register setting.
	ODM_delay_us(1);
//    dprintf("===> ODM_ConfigBBWithHeaderFile: [PHY_REG] %08X %08X\n", Addr, Data);
}

