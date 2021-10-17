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
#include "HalHWImg8821A_TestChip_BB.h"
#include "../../wl_common_hw.h"
#include "odm_RegConfig8821A.h"


/******************************************************************************
*                           PHY_REG.TXT
******************************************************************************/

u4Byte Array_TC_8821A_PHY_REG[] = { 
		0x800, 0x0020D410,
		0x804, 0x080112E0,
		0x808, 0x0E028211,
		0x80C, 0x12131111,
		0x810, 0x10101261,
		0x814, 0x020C3D10,
		0x818, 0x03A00385,
		0x820, 0x00000000,
		0x824, 0x00030FE0,
		0x828, 0x00000000,
		0x82C, 0x002083DD,
		0x830, 0x2AAA8E26,
		0x834, 0x0437A705,
		0x838, 0x06389B45,
		0x83C, 0x0000095B,
		0x840, 0xC0000001,
		0x844, 0x40003CDE,
		0x848, 0x62103F8B,
		0x84C, 0x6CFDFFB8,
		0x850, 0x28874706,
		0x854, 0x0001520C,
		0x858, 0x8060E000,
		0x85C, 0x74210168,
		0x860, 0x6929C321,
		0x864, 0x79727432,
		0x868, 0x8CA7A314,
		0x86C, 0x338C2878,
		0x870, 0x03333333,
		0x874, 0x31A12C2E,
		0x878, 0x00000152,
		0x87C, 0x000FC000,
		0x8A0, 0x00000013,
		0x8A4, 0x7F7F7F7F,
		0x8A8, 0xA0000338,
		0x8AC, 0x0FF0FA0A,
		0x8B4, 0x000FC080,
		0x8B8, 0x7C005FFF,
		0x8BC, 0x8CA520A0,
		0x8C0, 0x01F00020,
		0x8C4, 0x00000000,
		0x8C8, 0x00013169,
		0x8CC, 0x08248492,
		0x8D4, 0x940008A0,
		0x8D8, 0x290B5612,
		0x8F8, 0x400002C0,
		0x8FC, 0x00000000,
		0x900, 0x00000700,
		0x90C, 0x00000000,
		0x910, 0x0000FC00,
		0x914, 0x00000404,
		0x918, 0x1C1028C0,
		0x91C, 0x64B11A1C,
		0x920, 0xE0767233,
		0x924, 0x055AA500,
		0x928, 0x00000004,
		0x92C, 0xFFFE0000,
		0x930, 0xFFFFFFFE,
		0x934, 0x001FFFFF,
		0x960, 0x00000000,
		0x964, 0x00000000,
		0x968, 0x00000000,
		0x96C, 0x00000000,
		0x970, 0x801FFFFF,
		0x974, 0x000003FF,
		0x978, 0x00000000,
		0x97C, 0x00000000,
		0x980, 0x00000000,
		0x984, 0x00000000,
		0x988, 0x00000000,
		0x9A4, 0x00000080,
		0x9A8, 0x00000000,
		0x9AC, 0x00000000,
		0x9B0, 0x01081008,
		0x9B4, 0x01081008,
		0x9B8, 0x01081008,
		0x9BC, 0x01081008,
		0x9D0, 0x00000000,
		0x9D4, 0x00000000,
		0x9D8, 0x00000000,
		0x9DC, 0x00000000,
		0x9E4, 0x00000002,
		0x9E8, 0x00000000,
		0xA00, 0x00D047C8,
		0xA04, 0x01FF000C,
		0xA08, 0x8C8A8300,
		0xA0C, 0x2E68000F,
		0xA10, 0x9500BB78,
		0xA14, 0x11144028,
		0xA18, 0x00881117,
		0xA1C, 0x89140F00,
		0xA20, 0x1A1B0000,
		0xA24, 0x090E1317,
		0xA28, 0x00000204,
		0xA2C, 0x00910000,
		0xA70, 0x101FFF00,
		0xA74, 0x00000008,
		0xA78, 0x00000900,
		0xA7C, 0x225B0606,
		0xA80, 0x21805490,
		0xA84, 0x001F0000,
		0xB00, 0x03100000,
		0xB04, 0x0000B000,
		0xB08, 0xAE0201EB,
		0xB0C, 0x01003207,
		0xB10, 0x00009807,
		0xB14, 0x01000000,
		0xB18, 0x00000002,
		0xB1C, 0x00000002,
		0xB20, 0x0000001F,
		0xB24, 0x03020100,
		0xB28, 0x07060504,
		0xB2C, 0x0B0A0908,
		0xB30, 0x0F0E0D0C,
		0xB34, 0x13121110,
		0xB38, 0x17161514,
		0xB3C, 0x0000003A,
		0xB40, 0x00000000,
		0xB44, 0x00000000,
		0xB48, 0x13000032,
		0xB4C, 0x48080000,
		0xB50, 0x00000000,
		0xB54, 0x00000000,
		0xB58, 0x00000000,
		0xB5C, 0x00000000,
		0xC00, 0x00000007,
		0xC04, 0x00042020,
		0xC08, 0x80410231,
		0xC0C, 0x00000000,
		0xC10, 0x00000100,
		0xC14, 0x01000000,
		0xC1C, 0x40000003,
		0xC20, 0x2C2C2C2C,
		0xC24, 0x30303030,
		0xC28, 0x30303030,
		0xC2C, 0x2C2C2C2C,
		0xC30, 0x2C2C2C2C,
		0xC34, 0x2C2C2C2C,
		0xC38, 0x2C2C2C2C,
		0xC3C, 0x2A2A2A2A,
		0xC40, 0x2A2A2A2A,
		0xC44, 0x2A2A2A2A,
		0xC48, 0x2A2A2A2A,
		0xC4C, 0x2A2A2A2A,
		0xC50, 0x00000020,
		0xC54, 0x001C1208,
		0xC58, 0x30000C1C,
		0xC5C, 0x00000058,
		0xC60, 0x34344443,
		0xC64, 0x07003333,
		0xC68, 0x59799979,
		0xC6C, 0x59795979,
		0xC70, 0x99795979,
		0xC74, 0x99795979,
		0xC78, 0x99799979,
		0xC7C, 0x19799979,
		0xC80, 0x19791979,
		0xC84, 0x19791979,
		0xC94, 0x00000000,
		0xC98, 0x00000000,
		0xC9C, 0x00000000,
		0xCA0, 0x00000029,
		0xCA4, 0x08040201,
		0xCA8, 0x80402010,
		0xCB0, 0x77775745,
		0xCB4, 0x10000077,
		0xCB8, 0x00508240,

};

void
ODM_ReadAndConfig_TC_8821A_PHY_REG()
{
	#define READ_NEXT_PAIR(v1, v2, i) do { i += 2; v1 = Array[i]; v2 = Array[i+1]; } while(0)

	u4Byte     i           = 0;
	pu4Byte    ptr_array   = NULL;
 	u4Byte     ArrayLen    = sizeof(Array_TC_8821A_PHY_REG)/sizeof(u4Byte);
	pu4Byte    Array       = Array_TC_8821A_PHY_REG;


	for (i = 0; i < ArrayLen; i += 2 )
	{
	    u4Byte v1 = Array[i];
	    u4Byte v2 = Array[i+1];
	
	    // This (offset, data) pair meets the condition.
  	    odm_ConfigBB_PHY_8821A(v1, bMaskDWord, v2);
 		 	
	}

}

/******************************************************************************
*                           AGC_TAB.TXT
******************************************************************************/

u4Byte Array_TC_8821A_AGC_TAB[] = { 
		0x81C, 0xBF000001,
		0x81C, 0xBF020001,
		0x81C, 0xBF040001,
		0x81C, 0xBF060001,
		0x81C, 0xBE080001,
		0x81C, 0xBD0A0001,
		0x81C, 0xBC0C0001,
		0x81C, 0xBA0E0001,
		0x81C, 0xB9100001,
		0x81C, 0xB8120001,
		0x81C, 0xB7140001,
		0x81C, 0xB6160001,
		0x81C, 0xB5180001,
		0x81C, 0xB41A0001,
		0x81C, 0xB41C0001,
		0x81C, 0xB31E0001,
		0x81C, 0xB2200001,
		0x81C, 0xB1220001,
		0x81C, 0xB0240001,
		0x81C, 0xAF260001,
		0x81C, 0xAE280001,
		0x81C, 0xAD2A0001,
		0x81C, 0xAC2C0001,
		0x81C, 0xAB2E0001,
		0x81C, 0xAA300001,
		0x81C, 0xA9320001,
		0x81C, 0xA8340001,
		0x81C, 0xA7360001,
		0x81C, 0xA6380001,
		0x81C, 0xA53A0001,
		0x81C, 0xA43C0001,
		0x81C, 0x673E0001,
		0x81C, 0x66400001,
		0x81C, 0x65420001,
		0x81C, 0x64440001,
		0x81C, 0x63460001,
		0x81C, 0x62480001,
		0x81C, 0x614A0001,
		0x81C, 0x474C0001,
		0x81C, 0x464E0001,
		0x81C, 0x45500001,
		0x81C, 0x44520001,
		0x81C, 0x43540001,
		0x81C, 0x42560001,
		0x81C, 0x41580001,
		0x81C, 0x285A0001,
		0x81C, 0x275C0001,
		0x81C, 0x265E0001,
		0x81C, 0x25600001,
		0x81C, 0x24620001,
		0x81C, 0x0A640001,
		0x81C, 0x09660001,
		0x81C, 0x08680001,
		0x81C, 0x076A0001,
		0x81C, 0x066C0001,
		0x81C, 0x056E0001,
		0x81C, 0x04700001,
		0x81C, 0x03720001,
		0x81C, 0x02740001,
		0x81C, 0x01760001,
		0x81C, 0x01780001,
		0x81C, 0x017A0001,
		0x81C, 0x017C0001,
		0x81C, 0x017E0001,
		0x81C, 0xFD800001,
		0x81C, 0xFC820001,
		0x81C, 0xFB840001,
		0x81C, 0xFA860001,
		0x81C, 0xF9880001,
		0x81C, 0xF88A0001,
		0x81C, 0xF78C0001,
		0x81C, 0xF68E0001,
		0x81C, 0xF5900001,
		0x81C, 0xF4920001,
		0x81C, 0xF3940001,
		0x81C, 0xF2960001,
		0x81C, 0xF1980001,
		0x81C, 0xF09A0001,
		0x81C, 0xEF9C0001,
		0x81C, 0xEE9E0001,
		0x81C, 0xEDA00001,
		0x81C, 0xECA20001,
		0x81C, 0xEBA40001,
		0x81C, 0xEAA60001,
		0x81C, 0xE9A80001,
		0x81C, 0xE8AA0001,
		0x81C, 0xE7AC0001,
		0x81C, 0xE6AE0001,
		0x81C, 0xC6B00001,
		0x81C, 0xC5B20001,
		0x81C, 0xA5B40001,
		0x81C, 0xA4B60001,
		0x81C, 0xA3B80001,
		0x81C, 0xA2BA0001,
		0x81C, 0xA1BC0001,
		0x81C, 0x65BE0001,
		0x81C, 0x64C00001,
		0x81C, 0x63C20001,
		0x81C, 0x62C40001,
		0x81C, 0x61C60001,
		0x81C, 0x60C80001,
		0x81C, 0x60CA0001,
		0x81C, 0x60CC0001,
		0x81C, 0x60CE0001,
		0x81C, 0x60D00001,
		0x81C, 0x60D20001,
		0x81C, 0x60D40001,
		0x81C, 0x60D60001,
		0x81C, 0x60D80001,
		0x81C, 0x60DA0001,
		0x81C, 0x60DC0001,
		0x81C, 0x60DE0001,
		0x81C, 0x60E00001,
		0x81C, 0x60E20001,
		0x81C, 0x60E40001,
		0x81C, 0x60E60001,
		0x81C, 0x60E80001,
		0x81C, 0x60EA0001,
		0x81C, 0x60EC0001,
		0x81C, 0x60EE0001,
		0x81C, 0x60F00001,
		0x81C, 0x60F20001,
		0x81C, 0x60F40001,
		0x81C, 0x60F60001,
		0x81C, 0x60F80001,
		0x81C, 0x60FA0001,
		0x81C, 0x60FC0001,
		0x81C, 0x60FE0001,

};

void
ODM_ReadAndConfig_TC_8821A_AGC_TAB()
{
	#define READ_NEXT_PAIR(v1, v2, i) do { i += 2; v1 = Array[i]; v2 = Array[i+1]; } while(0)

	u4Byte     i           = 0;
	u4Byte     ArrayLen    = sizeof(Array_TC_8821A_AGC_TAB)/sizeof(u4Byte);
	pu4Byte    Array       = Array_TC_8821A_AGC_TAB;


	for (i = 0; i < ArrayLen; i += 2 )
	{
	    u4Byte v1 = Array[i];
	    u4Byte v2 = Array[i+1];
	
	    // This (offset, data) pair meets the condition.
	    odm_ConfigBB_AGC_8821A(v1, bMaskDWord, v2);
			
	}

}


