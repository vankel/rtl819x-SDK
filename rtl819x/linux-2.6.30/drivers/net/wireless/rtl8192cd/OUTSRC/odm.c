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

#include "Mp_Precomp.h"
#include "odm_precomp.h"


const u2Byte dB_Invert_Table[8][12] = {
	{	1,		1,		1,		2,		2,		2,		2,		3,		3,		3,		4,		4},
	{	4,		5,		6,		6,		7,		8,		9,		10,		11,		13,		14,		16},
	{	18,		20,		22,		25,		28,		32,		35,		40,		45,		50,		56,		63},
	{	71,		79,		89,		100,	112,	126,	141,	158,	178,	200,	224,	251},
	{	282,	316,	355,	398,	447,	501,	562,	631,	708,	794,	891,	1000},
	{	1122,	1259,	1413,	1585,	1778,	1995,	2239,	2512,	2818,	3162,	3548,	3981},
	{	4467,	5012,	5623,	6310,	7079,	7943,	8913,	10000,	11220,	12589,	14125,	15849},
	{	17783,	19953,	22387,	25119,	28184,	31623,	35481,	39811,	44668,	50119,	56234,	65535}};

// 20100515 Joseph: Add global variable to keep temporary scan list for antenna switching test.
//u1Byte			tmpNumBssDesc;
//RT_WLAN_BSS	tmpbssDesc[MAX_BSS_DESC];

#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
static u4Byte edca_setting_UL[HT_IOT_PEER_MAX] = 
// UNKNOWN		REALTEK_90	REALTEK_92SE	BROADCOM		RALINK		ATHEROS		CISCO		MERU        MARVELL	92U_AP		SELF_AP(DownLink/Tx)
{ 0x5e4322, 		0xa44f, 		0x5e4322,		0x5ea32b,  		0x5ea422, 	0x5ea322,	0x3ea430,	0x5ea42b, 0x5ea44f,	0x5e4322,	0x5e4322};


static u4Byte edca_setting_DL[HT_IOT_PEER_MAX] = 
// UNKNOWN		REALTEK_90	REALTEK_92SE	BROADCOM		RALINK		ATHEROS		CISCO		MERU,       MARVELL	92U_AP		SELF_AP(UpLink/Rx)
{ 0xa44f, 		0x5ea44f, 	0x5e4322, 		0x5ea42b, 		0xa44f, 		0xa630, 		0x5ea630,	0x5ea42b, 0xa44f,		0xa42b,		0xa42b};

static u4Byte edca_setting_DL_GMode[HT_IOT_PEER_MAX] = 
// UNKNOWN		REALTEK_90	REALTEK_92SE	BROADCOM		RALINK		ATHEROS		CISCO		MERU,       MARVELL	92U_AP		SELF_AP
{ 0x4322, 		0xa44f, 		0x5e4322,		0xa42b, 			0x5e4322, 	0x4322, 		0xa42b,		0x5ea42b, 0xa44f,		0x5e4322,	0x5ea42b};





//============================================================
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)


//avoid to warn in FreeBSD ==> To DO modify
u4Byte EDCAParam[HT_IOT_PEER_MAX][3] =
{          // UL			DL
	{0x5ea42b, 0x5ea42b, 0x5ea42b}, //0:unknown AP
	{0xa44f, 0x5ea44f, 0x5e431c}, // 1:realtek AP
	{0x5ea42b, 0x5ea42b, 0x5ea42b}, // 2:unknown AP => realtek_92SE	
	{0x5ea32b, 0x5ea42b, 0x5e4322}, // 3:broadcom AP
	{0x5ea422, 0x00a44f, 0x00a44f}, // 4:ralink AP
	{0x5ea322, 0x00a630, 0x00a44f}, // 5:atheros AP
	//{0x5ea42b, 0x5ea42b, 0x5ea42b},// 6:cisco AP
	{0x5e4322, 0x5e4322, 0x5e4322},// 6:cisco AP
	//{0x3ea430, 0x00a630, 0x3ea44f}, // 7:cisco AP
	{0x5ea42b,0x5ea42b,0x5ea42b},     //7:meru
	{0x5ea44f, 0x00a44f, 0x5ea42b}, // 8:marvell AP
	{0x5ea44f, 0x5ea44f, 0x5ea44f}, // 9realtek AP
	{0x5ea42b, 0x5ea42b, 0x5ea42b}, // 10:unknown AP=> 92U AP
	{0x5ea42b, 0xa630, 0x5e431c}, // 11:airgocap AP
//	{0x5e4322, 0x00a44f, 0x5ea44f}, // 12:unknown AP
};
//============================================================
// EDCA Paramter for AP/ADSL   by Mingzhi 2011-11-22
//============================================================
#elif (DM_ODM_SUPPORT_TYPE &ODM_ADSL)
enum qos_prio { BK, BE, VI, VO, VI_AG, VO_AG };

static const struct ParaRecord rtl_ap_EDCA[] =
{
//ACM,AIFSN, ECWmin, ECWmax, TXOplimit
     {0,     7,      4,      10,     0},            //BK
     {0,     3,      4,      6,      0},             //BE
     {0,     1,      3,      4,      188},         //VI
     {0,     1,      2,      3,      102},         //VO
     {0,     1,      3,      4,      94},          //VI_AG
     {0,     1,      2,      3,      47},          //VO_AG
};

static const struct ParaRecord rtl_sta_EDCA[] =
{
//ACM,AIFSN, ECWmin, ECWmax, TXOplimit
     {0,     7,      4,      10,     0},
     {0,     3,      4,      10,     0},
     {0,     2,      3,      4,      188},
     {0,     2,      2,      3,      102},
     {0,     2,      3,      4,      94},
     {0,     2,      2,      3,      47},
};
#endif


#if !defined(_OUTSRC_COEXIST)
//============================================================
// Global var
//============================================================


u4Byte OFDMSwingTable_New[OFDM_TABLE_SIZE_92D] = {
	0x0b40002d, // 0,  -15.0dB	
	0x0c000030, // 1,  -14.5dB
	0x0cc00033, // 2,  -14.0dB
	0x0d800036, // 3,  -13.5dB
	0x0e400039, // 4,  -13.0dB    
	0x0f00003c, // 5,  -12.5dB
	0x10000040, // 6,  -12.0dB
	0x11000044, // 7,  -11.5dB
	0x12000048, // 8,  -11.0dB
	0x1300004c, // 9,  -10.5dB
	0x14400051, // 10, -10.0dB
	0x15800056, // 11, -9.5dB
	0x16c0005b, // 12, -9.0dB
	0x18000060, // 13, -8.5dB
	0x19800066, // 14, -8.0dB
	0x1b00006c, // 15, -7.5dB
	0x1c800072, // 16, -7.0dB
	0x1e400079, // 17, -6.5dB
	0x20000080, // 18, -6.0dB
	0x22000088, // 19, -5.5dB
	0x24000090, // 20, -5.0dB
	0x26000098, // 21, -4.5dB
	0x288000a2, // 22, -4.0dB
	0x2ac000ab, // 23, -3.5dB
	0x2d4000b5, // 24, -3.0dB
	0x300000c0, // 25, -2.5dB
	0x32c000cb, // 26, -2.0dB
	0x35c000d7, // 27, -1.5dB
	0x390000e4, // 28, -1.0dB
	0x3c8000f2, // 29, -0.5dB
	0x40000100, // 30, +0dB
	0x43c0010f, // 31, +0.5dB
	0x47c0011f, // 32, +1.0dB
	0x4c000130, // 33, +1.5dB
	0x50800142, // 34, +2.0dB
	0x55400155, // 35, +2.5dB
	0x5a400169, // 36, +3.0dB
	0x5fc0017f, // 37, +3.5dB
	0x65400195, // 38, +4.0dB
	0x6b8001ae, // 39, +4.5dB
	0x71c001c7, // 40, +5.0dB
	0x788001e2, // 41, +5.5dB
	0x7f8001fe  // 42, +6.0dB
};               

u1Byte CCKSwingTable_Ch1_Ch13_New[CCK_TABLE_SIZE][8] = {
	{0x09, 0x08, 0x07, 0x06, 0x04, 0x03, 0x01, 0x01},	//  0, -16.0dB
	{0x09, 0x09, 0x08, 0x06, 0x05, 0x03, 0x01, 0x01},	//  1, -15.5dB
	{0x0a, 0x09, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01},	//  2, -15.0dB
	{0x0a, 0x0a, 0x09, 0x07, 0x05, 0x03, 0x02, 0x01},	//  3, -14.5dB
	{0x0b, 0x0a, 0x09, 0x08, 0x06, 0x04, 0x02, 0x01},	//  4, -14.0dB
	{0x0b, 0x0b, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x01},	//  5, -13.5dB
	{0x0c, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x01},	//  6, -13.0dB
	{0x0d, 0x0c, 0x0b, 0x09, 0x07, 0x04, 0x02, 0x01},	//  7, -12.5dB
	{0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x05, 0x02, 0x01},	//  8, -12.0dB
	{0x0e, 0x0e, 0x0c, 0x0a, 0x08, 0x05, 0x02, 0x01},	//  9, -11.5dB
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01},	// 10, -11.0dB
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},	// 11, -10.5dB
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	// 12, -10.0dB
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	// 13, -9.5dB
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},	// 14, -9.0dB 
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},	// 15, -8.5dB
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},	// 16, -8.0dB 
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},	// 17, -7.5dB
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},	// 18, -7.0dB 
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},	// 19, -6.5dB
    	{0x1c, 0x1a, 0x18, 0x12, 0x0e, 0x08, 0x04, 0x02},	// 20, -6.0dB 
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},	// 21, -5.5dB
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},	// 22, -5.0dB 
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},	// 23, -4.5dB
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},	// 24, -4.0dB 
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},	// 25, -3.5dB
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},	// 26, -3.0dB
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},	// 27, -2.5dB
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},	// 28, -2.0dB 
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},	// 29, -1.5dB
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},	// 30, -1.0dB
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},	// 31, -0.5dB
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04} 	// 32, +0dB
};                                                                  


u1Byte CCKSwingTable_Ch14_New[CCK_TABLE_SIZE][8]= {
	{0x09, 0x08, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00},	//  0, -16.0dB
	{0x09, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	//  1, -15.5dB
	{0x0a, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	//  2, -15.0dB
	{0x0a, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	//  3, -14.5dB
	{0x0b, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	//  4, -14.0dB
	{0x0b, 0x0b, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	//  5, -13.5dB
	{0x0c, 0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	//  6, -13.0dB
	{0x0d, 0x0c, 0x0b, 0x06, 0x00, 0x00, 0x00, 0x00},	//  7, -12.5dB
	{0x0d, 0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	//  8, -12.0dB
	{0x0e, 0x0e, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	//  9, -11.5dB
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00},	// 10, -11.0dB
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},	// 11, -10.5dB
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	// 12, -10.0dB
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	// 13, -9.5dB
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},	// 14, -9.0dB  
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},	// 15, -8.5dB
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},	// 16, -8.0dB  
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},	// 17, -7.5dB
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},	// 18, -7.0dB  
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},	// 19, -6.5dB 
	{0x1c, 0x1a, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},	// 20, -6.0dB  
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},	// 21, -5.5dB
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},	// 22, -5.0dB  
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},	// 23, -4.5dB
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},	// 24, -4.0dB  
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},	// 25, -3.5dB  
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},	// 26, -3.0dB  
	{0x28, 0x28, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00},	// 27, -2.5dB
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},	// 28, -2.0dB  
	{0x2d, 0x2d, 0x17, 0x17, 0x00, 0x00, 0x00, 0x00},	// 29, -1.5dB
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},	// 30, -1.0dB  
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},	// 31, -0.5dB 
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00} 	// 32, +0dB	
};

u4Byte OFDMSwingTable[OFDM_TABLE_SIZE_92D] = {
	0x0b40002d, // 0,  -15.0dB	
	0x0c000030, // 1,  -14.5dB
	0x0cc00033, // 2,  -14.0dB
	0x0d800036, // 3,  -13.5dB
	0x0e400039, // 4,  -13.0dB    
	0x0f00003c, // 5,  -12.5dB
	0x10000040, // 6,  -12.0dB
	0x11000044, // 7,  -11.5dB
	0x12000048, // 8,  -11.0dB
	0x1300004c, // 9,  -10.5dB
	0x14400051, // 10, -10.0dB
	0x15800056, // 11, -9.5dB
	0x16c0005b, // 12, -9.0dB
	0x18000060, // 13, -8.5dB
	0x19800066, // 14, -8.0dB
	0x1b00006c, // 15, -7.5dB
	0x1c800072, // 16, -7.0dB
	0x1e400079, // 17, -6.5dB
	0x20000080, // 18, -6.0dB
	0x22000088, // 19, -5.5dB
	0x24000090, // 20, -5.0dB
	0x26000098, // 21, -4.5dB
	0x288000a2, // 22, -4.0dB
	0x2ac000ab, // 23, -3.5dB
	0x2d4000b5, // 24, -3.0dB
	0x300000c0, // 25, -2.5dB
	0x32c000cb, // 26, -2.0dB
	0x35c000d7, // 27, -1.5dB
	0x390000e4, // 28, -1.0dB
	0x3c8000f2, // 29, -0.5dB
	0x40000100, // 30, +0dB
	0x43c0010f, // 31, +0.5dB
	0x47c0011f, // 32, +1.0dB
	0x4c000130, // 33, +1.5dB
	0x50800142, // 34, +2.0dB
	0x55400155, // 35, +2.5dB
	0x5a400169, // 36, +3.0dB
	0x5fc0017f, // 37, +3.5dB
	0x65400195, // 38, +4.0dB
	0x6b8001ae, // 39, +4.5dB
	0x71c001c7, // 40, +5.0dB
	0x788001e2, // 41, +5.5dB
	0x7f8001fe  // 42, +6.0dB
};               


u1Byte CCKSwingTable_Ch1_Ch13[CCK_TABLE_SIZE][8] = {
	{0x09, 0x08, 0x07, 0x06, 0x04, 0x03, 0x01, 0x01},	//  0, -16.0dB
	{0x09, 0x09, 0x08, 0x06, 0x05, 0x03, 0x01, 0x01},	//  1, -15.5dB
	{0x0a, 0x09, 0x08, 0x07, 0x05, 0x03, 0x02, 0x01},	//  2, -15.0dB
	{0x0a, 0x0a, 0x09, 0x07, 0x05, 0x03, 0x02, 0x01},	//  3, -14.5dB
	{0x0b, 0x0a, 0x09, 0x08, 0x06, 0x04, 0x02, 0x01},	//  4, -14.0dB
	{0x0b, 0x0b, 0x0a, 0x08, 0x06, 0x04, 0x02, 0x01},	//  5, -13.5dB
	{0x0c, 0x0c, 0x0a, 0x09, 0x06, 0x04, 0x02, 0x01},	//  6, -13.0dB
	{0x0d, 0x0c, 0x0b, 0x09, 0x07, 0x04, 0x02, 0x01},	//  7, -12.5dB
	{0x0d, 0x0d, 0x0c, 0x0a, 0x07, 0x05, 0x02, 0x01},	//  8, -12.0dB
	{0x0e, 0x0e, 0x0c, 0x0a, 0x08, 0x05, 0x02, 0x01},	//  9, -11.5dB
	{0x0f, 0x0f, 0x0d, 0x0b, 0x08, 0x05, 0x03, 0x01},	// 10, -11.0dB
	{0x10, 0x10, 0x0e, 0x0b, 0x08, 0x05, 0x03, 0x01},	// 11, -10.5dB
	{0x11, 0x11, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	// 12, -10.0dB
	{0x12, 0x12, 0x0f, 0x0c, 0x09, 0x06, 0x03, 0x01},	// 13, -9.5dB
	{0x13, 0x13, 0x10, 0x0d, 0x0a, 0x06, 0x03, 0x01},	// 14, -9.0dB 
	{0x14, 0x14, 0x11, 0x0e, 0x0b, 0x07, 0x03, 0x02},	// 15, -8.5dB
	{0x16, 0x15, 0x12, 0x0f, 0x0b, 0x07, 0x04, 0x01},	// 16, -8.0dB 
	{0x17, 0x16, 0x13, 0x10, 0x0c, 0x08, 0x04, 0x02},	// 17, -7.5dB
	{0x18, 0x17, 0x15, 0x11, 0x0c, 0x08, 0x04, 0x02},	// 18, -7.0dB 
	{0x1a, 0x19, 0x16, 0x12, 0x0d, 0x09, 0x04, 0x02},	// 19, -6.5dB
    	{0x1c, 0x1a, 0x18, 0x12, 0x0e, 0x08, 0x04, 0x02},	// 20, -6.0dB 
	{0x1d, 0x1c, 0x18, 0x14, 0x0f, 0x0a, 0x05, 0x02},	// 21, -5.5dB
	{0x1f, 0x1e, 0x1a, 0x15, 0x10, 0x0a, 0x05, 0x02},	// 22, -5.0dB 
	{0x20, 0x20, 0x1b, 0x16, 0x11, 0x08, 0x05, 0x02},	// 23, -4.5dB
	{0x22, 0x21, 0x1d, 0x18, 0x11, 0x0b, 0x06, 0x02},	// 24, -4.0dB 
	{0x24, 0x23, 0x1f, 0x19, 0x13, 0x0c, 0x06, 0x03},	// 25, -3.5dB
	{0x26, 0x25, 0x21, 0x1b, 0x14, 0x0d, 0x06, 0x03},	// 26, -3.0dB
	{0x28, 0x28, 0x22, 0x1c, 0x15, 0x0d, 0x07, 0x03},	// 27, -2.5dB
	{0x2b, 0x2a, 0x25, 0x1e, 0x16, 0x0e, 0x07, 0x03},	// 28, -2.0dB 
	{0x2d, 0x2d, 0x27, 0x1f, 0x18, 0x0f, 0x08, 0x03},	// 29, -1.5dB
	{0x30, 0x2f, 0x29, 0x21, 0x19, 0x10, 0x08, 0x03},	// 30, -1.0dB
	{0x33, 0x32, 0x2b, 0x23, 0x1a, 0x11, 0x08, 0x04},	// 31, -0.5dB
	{0x36, 0x35, 0x2e, 0x25, 0x1c, 0x12, 0x09, 0x04} 	// 32, +0dB
};                                                                  


u1Byte CCKSwingTable_Ch14 [CCK_TABLE_SIZE][8]= {
	{0x09, 0x08, 0x07, 0x04, 0x00, 0x00, 0x00, 0x00},	//  0, -16.0dB
	{0x09, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	//  1, -15.5dB
	{0x0a, 0x09, 0x08, 0x05, 0x00, 0x00, 0x00, 0x00},	//  2, -15.0dB
	{0x0a, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	//  3, -14.5dB
	{0x0b, 0x0a, 0x09, 0x05, 0x00, 0x00, 0x00, 0x00},	//  4, -14.0dB
	{0x0b, 0x0b, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	//  5, -13.5dB
	{0x0c, 0x0c, 0x0a, 0x06, 0x00, 0x00, 0x00, 0x00},	//  6, -13.0dB
	{0x0d, 0x0c, 0x0b, 0x06, 0x00, 0x00, 0x00, 0x00},	//  7, -12.5dB
	{0x0d, 0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	//  8, -12.0dB
	{0x0e, 0x0e, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00},	//  9, -11.5dB
	{0x0f, 0x0f, 0x0d, 0x08, 0x00, 0x00, 0x00, 0x00},	// 10, -11.0dB
	{0x10, 0x10, 0x0e, 0x08, 0x00, 0x00, 0x00, 0x00},	// 11, -10.5dB
	{0x11, 0x11, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	// 12, -10.0dB
	{0x12, 0x12, 0x0f, 0x09, 0x00, 0x00, 0x00, 0x00},	// 13, -9.5dB
	{0x13, 0x13, 0x10, 0x0a, 0x00, 0x00, 0x00, 0x00},	// 14, -9.0dB  
	{0x14, 0x14, 0x11, 0x0a, 0x00, 0x00, 0x00, 0x00},	// 15, -8.5dB
	{0x16, 0x15, 0x12, 0x0b, 0x00, 0x00, 0x00, 0x00},	// 16, -8.0dB  
	{0x17, 0x16, 0x13, 0x0b, 0x00, 0x00, 0x00, 0x00},	// 17, -7.5dB
	{0x18, 0x17, 0x15, 0x0c, 0x00, 0x00, 0x00, 0x00},	// 18, -7.0dB  
	{0x1a, 0x19, 0x16, 0x0d, 0x00, 0x00, 0x00, 0x00},	// 19, -6.5dB 
	{0x1c, 0x1a, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},	// 20, -6.0dB  
	{0x1d, 0x1c, 0x18, 0x0e, 0x00, 0x00, 0x00, 0x00},	// 21, -5.5dB
	{0x1f, 0x1e, 0x1a, 0x0f, 0x00, 0x00, 0x00, 0x00},	// 22, -5.0dB  
	{0x20, 0x20, 0x1b, 0x10, 0x00, 0x00, 0x00, 0x00},	// 23, -4.5dB
	{0x22, 0x21, 0x1d, 0x11, 0x00, 0x00, 0x00, 0x00},	// 24, -4.0dB  
	{0x24, 0x23, 0x1f, 0x12, 0x00, 0x00, 0x00, 0x00},	// 25, -3.5dB  
	{0x26, 0x25, 0x21, 0x13, 0x00, 0x00, 0x00, 0x00},	// 26, -3.0dB  
	{0x28, 0x28, 0x24, 0x14, 0x00, 0x00, 0x00, 0x00},	// 27, -2.5dB
	{0x2b, 0x2a, 0x25, 0x15, 0x00, 0x00, 0x00, 0x00},	// 28, -2.0dB  
	{0x2d, 0x2d, 0x17, 0x17, 0x00, 0x00, 0x00, 0x00},	// 29, -1.5dB
	{0x30, 0x2f, 0x29, 0x18, 0x00, 0x00, 0x00, 0x00},	// 30, -1.0dB  
	{0x33, 0x32, 0x2b, 0x19, 0x00, 0x00, 0x00, 0x00},	// 31, -0.5dB 
	{0x36, 0x35, 0x2e, 0x1b, 0x00, 0x00, 0x00, 0x00} 	// 32, +0dB	
};	

#if 0
u4Byte OFDMSwingTable_92E[OFDM_TABLE_SIZE_92E] = {
	/* Index0   6  dB */ 0x7fc001ff,
	/* Index1   5.7dB */ 0x7b4001ed,
	/* Index2   5.4dB */ 0x774001dd,
	/* Index3   5.1dB */ 0x734001cd,
	/* Index4   4.8dB */ 0x6f4001bd,
	/* Index5   4.5dB */ 0x6b8001ae,
	/* Index6   4.2dB */ 0x67c0019f,
	/* Index7   3.9dB */ 0x64400191,
	/* Index8   3.6dB */ 0x60c00183,
	/* Index9   3.3dB */ 0x5d800176,
	/* Index10  3  dB */ 0x5a80016a,
	/* Index11  2.7dB */ 0x5740015d,
	/* Index12  2.4dB */ 0x54400151,
	/* Index13  2.1dB */ 0x51800146,
	/* Index14  1.8dB */ 0x4ec0013b,
	/* Index15  1.5dB */ 0x4c000130,
	/* Index16  1.2dB */ 0x49800126,
	/* Index17  0.9dB */ 0x4700011c,
	/* Index18  0.6dB */ 0x44800112,
	/* Index19  0.3dB */ 0x42000108,
	/* Index20  0  dB */ 0x40000100, // 20 This is OFDM base index
	/* Index21 -0.3dB */ 0x3dc000f7,
	/* Index22 -0.6dB */ 0x3bc000ef,
	/* Index23 -0.9dB */ 0x39c000e7,
	/* Index24 -1.2dB */ 0x37c000df,
	/* Index25 -1.5dB */ 0x35c000d7,
	/* Index26 -1.8dB */ 0x340000d0,
	/* Index27 -2.1dB */ 0x324000c9,
	/* Index28 -2.4dB */ 0x308000c2,
	/* Index29 -2.7dB */ 0x2f0000bc,
	/* Index30 -3  dB */ 0x2d4000b5,
	/* Index31 -3.3dB */ 0x2bc000af,
	/* Index32 -3.6dB */ 0x2a4000a9,
	/* Index33 -3.9dB */ 0x28c000a3,
	/* Index34 -4.2dB */ 0x2780009e,
	/* Index35 -4.5dB */ 0x26000098,
	/* Index36 -4.8dB */ 0x24c00093,
	/* Index37 -5.1dB */ 0x2380008e,
	/* Index38 -5.4dB */ 0x22400089,
	/* Index39 -5.7dB */ 0x21400085,
	/* Index40 -6  dB */ 0x20000080,
	/* Index41 -6.3dB */ 0x1f00007c,
	/* Index42 -6.6dB */ 0x1e000078,
	/* Index43 -6.9dB */ 0x1d000074,
	/* Index44 -7.2dB */ 0x1c000070,
	/* Index45 -7.5dB */ 0x1b00006c,
	/* Index46 -7.8dB */ 0x1a000068,
	/* Index47 -8.1dB */ 0x19400065,
	/* Index48 -8.4dB */ 0x18400061,
	/* Index49 -8.7dB */ 0x1780005e,
	/* Index50 -9  dB */ 0x16c0005b,
	/* Index51 -9.3dB */ 0x16000058,
	/* Index52 -9.6dB */ 0x15400055,
	/* Index53 -9.9dB */ 0x14800052
};
u1Byte CCKSwingTable_Ch1_Ch13_92E[CCK_TABLE_SIZE_92E][8] = {
	/* Index0    0  dB */    {0x36, 0x34 , 0x2E , 0x26 , 0x1C , 0x12 , 0x08 , 0x04},
	/* Index1   -0.3dB */    {0x34, 0x32 , 0x2C , 0x25 , 0x1B , 0x11 , 0x08 , 0x04},
	/* Index2   -0.6dB */    {0x32, 0x30 , 0x2B , 0x23 , 0x1A , 0x11 , 0x07 , 0x04},
	/* Index3   -0.9dB */    {0x31, 0x2F , 0x29 , 0x22 , 0x19 , 0x10 , 0x07 , 0x04},
	/* Index4   -1.2dB */    {0x2F, 0x2D , 0x28 , 0x21 , 0x18 , 0x10 , 0x07 , 0x03},
	/* Index5   -1.5dB */    {0x2D, 0x2C , 0x27 , 0x20 , 0x18 , 0x0F , 0x07 , 0x03},
	/* Index6   -1.8dB */    {0x2C, 0x2A , 0x25 , 0x1F , 0x17 , 0x0F , 0x06 , 0x03},
	/* Index7   -2.1dB */    {0x2A, 0x29 , 0x24 , 0x1E , 0x16 , 0x0E , 0x06 , 0x03},
	/* Index8   -2.4dB */    {0x29, 0x27 , 0x23 , 0x1D , 0x15 , 0x0E , 0x06 , 0x03},
	/* Index9   -2.7dB */    {0x27, 0x26 , 0x22 , 0x1C , 0x14 , 0x0D , 0x06 , 0x03},
	/* Index10  -3  dB */    {0x26, 0x25 , 0x20 , 0x1B , 0x14 , 0x0D , 0x06 , 0x03},
	/* Index11  -3.3dB */    {0x25, 0x23 , 0x1F , 0x1A , 0x13 , 0x0C , 0x05 , 0x03},
	/* Index12  -3.6dB */    {0x24, 0x22 , 0x1E , 0x19 , 0x12 , 0x0C , 0x05 , 0x03},
	/* Index13  -3.9dB */    {0x22, 0x21 , 0x1D , 0x18 , 0x12 , 0x0B , 0x05 , 0x03},
	/* Index14  -4.2dB */    {0x21, 0x20 , 0x1C , 0x17 , 0x11 , 0x0B , 0x05 , 0x02},
	/* Index15  -4.5dB */    {0x20, 0x1F , 0x1B , 0x17 , 0x11 , 0x0B , 0x05 , 0x02},
	/* Index16  -4.8dB */    {0x1F, 0x1E , 0x1A , 0x16 , 0x10 , 0x0A , 0x05 , 0x02},
	/* Index17  -5.1dB */    {0x1E, 0x1D , 0x1A , 0x15 , 0x10 , 0x0A , 0x04 , 0x02},
	/* Index18  -5.4dB */    {0x1D, 0x1C , 0x19 , 0x14 , 0x0F , 0x0A , 0x04 , 0x02},
	/* Index19  -5.7dB */    {0x1C, 0x1B , 0x18 , 0x14 , 0x0E , 0x09 , 0x04 , 0x02},
	/* Index20  -6.0dB */    {0x1B, 0x1A , 0x17 , 0x13 , 0x0E , 0x09 , 0x04 , 0x02}, // 20 This is CCK base index
	/* Index21  -6.3dB */    {0x1A, 0x19 , 0x16 , 0x12 , 0x0E , 0x09 , 0x04 , 0x02},
	/* Index22  -6.6dB */    {0x19, 0x18 , 0x15 , 0x12 , 0x0D , 0x08 , 0x04 , 0x02},
	/* Index23  -6.9dB */    {0x18, 0x17 , 0x15 , 0x11 , 0x0D , 0x08 , 0x04 , 0x02},
	/* Index24  -7.2dB */    {0x18, 0x17 , 0x14 , 0x11 , 0x0C , 0x08 , 0x03 , 0x02},
	/* Index25  -7.5dB */    {0x17, 0x16 , 0x13 , 0x10 , 0x0C , 0x08 , 0x03 , 0x02},
	/* Index26  -7.8dB */    {0x16, 0x15 , 0x13 , 0x0F , 0x0B , 0x07 , 0x03 , 0x02},
	/* Index27  -8.1dB */    {0x15, 0x14 , 0x12 , 0x0F , 0x0B , 0x07 , 0x03 , 0x02},
	/* Index28  -8.4dB */    {0x14, 0x14 , 0x11 , 0x0E , 0x0B , 0x07 , 0x03 , 0x02},
	/* Index29  -8.7dB */    {0x14, 0x13 , 0x11 , 0x0E , 0x0A , 0x07 , 0x03 , 0x01},
	/* Index30  -9.0dB */    {0x13, 0x12 , 0x10 , 0x0D , 0x0A , 0x06 , 0x03 , 0x01}, // 30 This is hp CCK base index
	/* Index31  -9.3dB */    {0x12, 0x12 , 0x0F , 0x0D , 0x0A , 0x06 , 0x03 , 0x01},
	/* Index32  -9.6dB */    {0x12, 0x11 , 0x0F , 0x0D , 0x09 , 0x06 , 0x03 , 0x01},
	/* Index33  -9.9dB */    {0x11, 0x11 , 0x0F , 0x0C , 0x09 , 0x06 , 0x03 , 0x01},
	/* Index34 -10.2dB */    {0x11, 0x11 , 0x0E , 0x0C , 0x09 , 0x06 , 0x02 , 0x01},
	/* Index35 -10.5dB */    {0x10, 0x0F , 0x0E , 0x0B , 0x08 , 0x05 , 0x02 , 0x01},
	/* Index36 -10.8dB */    {0x10, 0x0F , 0x0D , 0x0B , 0x08 , 0x05 , 0x02 , 0x01},
	/* Index37 -11.1dB */    {0x0F, 0x0E , 0x0D , 0x0A , 0x08 , 0x05 , 0x02 , 0x01},
	/* Index38 -11.4dB */    {0x0E, 0x0E , 0x0C , 0x0A , 0x07 , 0x05 , 0x02 , 0x01},
	/* Index39 -11.7dB */    {0x0E, 0x0D , 0x0C , 0x0A , 0x07 , 0x05 , 0x02 , 0x01},
	/* Index40 -12  dB */    {0x0E, 0x0D , 0x0C , 0x0A , 0x07 , 0x05 , 0x02 , 0x01},
	/* Index41 -12.3dB */    {0x0D, 0x0D , 0x0B , 0x09 , 0x07 , 0x04 , 0x02 , 0x01},
	/* Index42 -12.6dB */    {0x0D, 0x0C , 0x0B , 0x09 , 0x07 , 0x04 , 0x02 , 0x01},
	/* Index43 -12.9dB */    {0x0C, 0x0C , 0x0A , 0x09 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index44 -13.2dB */    {0x0C, 0x0B , 0x0A , 0x08 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index45 -13.5dB */    {0x0B, 0x0B , 0x0A , 0x08 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index46 -13.8dB */    {0x0B, 0x0B , 0x09 , 0x08 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index47 -14.1dB */    {0x0B, 0x0A , 0x09 , 0x07 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index48 -14.4dB */    {0x0A, 0x0A , 0x09 , 0x07 , 0x05 , 0x03 , 0x02 , 0x01},
	/* Index49 -14.7dB */    {0x0A, 0x0A , 0x08 , 0x07 , 0x05 , 0x03 , 0x01 , 0x01},
	/* Index50 -15  dB */    {0x0A, 0x09 , 0x08 , 0x07 , 0x05 , 0x03 , 0x01 , 0x01},
	/* Index51 -15.3dB */    {0x09, 0x09 , 0x08 , 0x06 , 0x05 , 0x03 , 0x01 , 0x01},
	/* Index52 -15.6dB */    {0x09, 0x09 , 0x08 , 0x06 , 0x05 , 0x03 , 0x01 , 0x01},
	/* Index53 -15.9dB */    {0x09, 0x08 , 0x07 , 0x06 , 0x04 , 0x03 , 0x01 , 0x01}
};
u1Byte CCKSwingTable_Ch14_92E[CCK_TABLE_SIZE_92E][8] = {
	/* Index0    0  dB */    {0x36, 0x34 , 0x2E , 0x26 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index1   -0.3dB */    {0x34, 0x32 , 0x2C , 0x25 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index2   -0.6dB */    {0x32, 0x30 , 0x2B , 0x23 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index3   -0.9dB */    {0x31, 0x2F , 0x29 , 0x22 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index4   -1.2dB */    {0x2F, 0x2D , 0x28 , 0x21 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index5   -1.5dB */    {0x2D, 0x2C , 0x27 , 0x20 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index6   -1.8dB */    {0x2C, 0x2A , 0x25 , 0x1F , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index7   -2.1dB */    {0x2A, 0x29 , 0x24 , 0x1E , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index8   -2.4dB */    {0x29, 0x27 , 0x23 , 0x1D , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index9   -2.7dB */    {0x27, 0x26 , 0x22 , 0x1C , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index10  -3  dB */    {0x26, 0x25 , 0x20 , 0x1B , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index11  -3.3dB */    {0x25, 0x23 , 0x1F , 0x1A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index12  -3.6dB */    {0x24, 0x22 , 0x1E , 0x19 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index13  -3.9dB */    {0x22, 0x21 , 0x1D , 0x18 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index14  -4.2dB */    {0x21, 0x20 , 0x1C , 0x17 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index15  -4.5dB */    {0x20, 0x1F , 0x1B , 0x17 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index16  -4.8dB */    {0x1F, 0x1E , 0x1A , 0x16 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index17  -5.1dB */    {0x1E, 0x1D , 0x1A , 0x15 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index18  -5.4dB */    {0x1D, 0x1C , 0x19 , 0x14 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index19  -5.7dB */    {0x1C, 0x1B , 0x18 , 0x14 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index20  -6  dB */     {0x1B, 0x1A , 0x17 , 0x13 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index21  -6.3dB */    {0x1A, 0x19 , 0x16 , 0x12 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index22  -6.6dB */    {0x19, 0x18 , 0x15 , 0x12 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index23  -6.9dB */    {0x18, 0x17 , 0x15 , 0x11 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index24  -7.2dB */    {0x18, 0x17 , 0x14 , 0x11 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index25  -7.5dB */    {0x17, 0x16 , 0x13 , 0x10 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index26  -7.8dB */    {0x16, 0x15 , 0x13 , 0x0F , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index27  -8.1dB */    {0x15, 0x14 , 0x12 , 0x0F , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index28  -8.4dB */    {0x14, 0x14 , 0x11 , 0x0E , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index29  -8.7dB */    {0x14, 0x13 , 0x11 , 0x0E , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index30  -9  dB */    {0x13, 0x12 , 0x10 , 0x0D , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index31  -9.3dB */    {0x12, 0x12 , 0x0F , 0x0D , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index32  -9.6dB */    {0x12, 0x11 , 0x0F , 0x0D , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index33  -9.9dB */    {0x11, 0x11 , 0x0F , 0x0C , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index34 -10.2dB */    {0x11, 0x11 , 0x0E , 0x0C , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index35 -10.5dB */    {0x10, 0x0F , 0x0E , 0x0B , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index36 -10.8dB */    {0x10, 0x0F , 0x0D , 0x0B , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index37 -11.1dB */    {0x0F, 0x0E , 0x0D , 0x0A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index38 -11.4dB */    {0x0E, 0x0E , 0x0C , 0x0A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index39 -11.7dB */    {0x0E, 0x0D , 0x0C , 0x0A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index40 -12  dB */    {0x0E, 0x0D , 0x0C , 0x0A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index41 -12.3dB */    {0x0D, 0x0D , 0x0B , 0x09 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index42 -12.6dB */    {0x0D, 0x0C , 0x0B , 0x09 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index43 -12.9dB */    {0x0C, 0x0C , 0x0A , 0x09 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index44 -13.2dB */    {0x0C, 0x0B , 0x0A , 0x08 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index45 -13.5dB */    {0x0B, 0x0B , 0x0A , 0x08 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index46 -13.8dB */    {0x0B, 0x0B , 0x09 , 0x08 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index47 -14.1dB */    {0x0B, 0x0A , 0x09 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index48 -14.4dB */    {0x0A, 0x0A , 0x09 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index49 -14.7dB */    {0x0A, 0x0A , 0x08 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index50 -15  dB */    {0x0A, 0x09 , 0x08 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index51 -15.3dB */    {0x09, 0x09 , 0x08 , 0x06 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index52 -15.6dB */    {0x09, 0x09 , 0x08 , 0x06 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index53 -15.9dB */    {0x09, 0x08 , 0x07 , 0x06 , 0x00 , 0x00 , 0x00 , 0x00}
};
#endif

#ifdef AP_BUILD_WORKAROUND

unsigned int TxPwrTrk_OFDM_SwingTbl[TxPwrTrk_OFDM_SwingTbl_Len] = {
	/*  +6.0dB */ 0x7f8001fe,
	/*  +5.5dB */ 0x788001e2,
	/*  +5.0dB */ 0x71c001c7,
	/*  +4.5dB */ 0x6b8001ae,
	/*  +4.0dB */ 0x65400195,
	/*  +3.5dB */ 0x5fc0017f,
	/*  +3.0dB */ 0x5a400169,
	/*  +2.5dB */ 0x55400155,
	/*  +2.0dB */ 0x50800142,
	/*  +1.5dB */ 0x4c000130,
	/*  +1.0dB */ 0x47c0011f,
	/*  +0.5dB */ 0x43c0010f,
	/*   0.0dB */ 0x40000100,
	/*  -0.5dB */ 0x3c8000f2,
	/*  -1.0dB */ 0x390000e4,
	/*  -1.5dB */ 0x35c000d7,
	/*  -2.0dB */ 0x32c000cb,
	/*  -2.5dB */ 0x300000c0,
	/*  -3.0dB */ 0x2d4000b5,
	/*  -3.5dB */ 0x2ac000ab,
	/*  -4.0dB */ 0x288000a2,
	/*  -4.5dB */ 0x26000098,
	/*  -5.0dB */ 0x24000090,
	/*  -5.5dB */ 0x22000088,
	/*  -6.0dB */ 0x20000080,
	/*  -6.5dB */ 0x1a00006c,
	/*  -7.0dB */ 0x1c800072,
	/*  -7.5dB */ 0x18000060,
	/*  -8.0dB */ 0x19800066,
	/*  -8.5dB */ 0x15800056,
	/*  -9.0dB */ 0x26c0005b,
	/*  -9.5dB */ 0x14400051,
	/* -10.0dB */ 0x24400051,
	/* -10.5dB */ 0x1300004c,
	/* -11.0dB */ 0x12000048,
	/* -11.5dB */ 0x11000044,
	/* -12.0dB */ 0x10000040
};
#endif

#endif


#ifdef CONFIG_WLAN_HAL_8192EE
u4Byte OFDMSwingTable_92E[OFDM_TABLE_SIZE_92E] = {
	/* Index0   6  dB */ 0x7fc001ff,
	/* Index1   5.7dB */ 0x7b4001ed,
	/* Index2   5.4dB */ 0x774001dd,
	/* Index3   5.1dB */ 0x734001cd,
	/* Index4   4.8dB */ 0x6f4001bd,
	/* Index5   4.5dB */ 0x6b8001ae,
	/* Index6   4.2dB */ 0x67c0019f,
	/* Index7   3.9dB */ 0x64400191,
	/* Index8   3.6dB */ 0x60c00183,
	/* Index9   3.3dB */ 0x5d800176,
	/* Index10  3  dB */ 0x5a80016a,
	/* Index11  2.7dB */ 0x5740015d,
	/* Index12  2.4dB */ 0x54400151,
	/* Index13  2.1dB */ 0x51800146,
	/* Index14  1.8dB */ 0x4ec0013b,
	/* Index15  1.5dB */ 0x4c000130,
	/* Index16  1.2dB */ 0x49800126,
	/* Index17  0.9dB */ 0x4700011c,
	/* Index18  0.6dB */ 0x44800112,
	/* Index19  0.3dB */ 0x42000108,
	/* Index20  0  dB */ 0x40000100, // 20 This is OFDM base index
	/* Index21 -0.3dB */ 0x3dc000f7,
	/* Index22 -0.6dB */ 0x3bc000ef,
	/* Index23 -0.9dB */ 0x39c000e7,
	/* Index24 -1.2dB */ 0x37c000df,
	/* Index25 -1.5dB */ 0x35c000d7,
	/* Index26 -1.8dB */ 0x340000d0,
	/* Index27 -2.1dB */ 0x324000c9,
	/* Index28 -2.4dB */ 0x308000c2,
	/* Index29 -2.7dB */ 0x2f0000bc,
	/* Index30 -3  dB */ 0x2d4000b5,
	/* Index31 -3.3dB */ 0x2bc000af,
	/* Index32 -3.6dB */ 0x2a4000a9,
	/* Index33 -3.9dB */ 0x28c000a3,
	/* Index34 -4.2dB */ 0x2780009e,
	/* Index35 -4.5dB */ 0x26000098,
	/* Index36 -4.8dB */ 0x24c00093,
	/* Index37 -5.1dB */ 0x2380008e,
	/* Index38 -5.4dB */ 0x22400089,
	/* Index39 -5.7dB */ 0x21400085,
	/* Index40 -6  dB */ 0x20000080,
	/* Index41 -6.3dB */ 0x1f00007c,
	/* Index42 -6.6dB */ 0x1e000078,
	/* Index43 -6.9dB */ 0x1d000074,
	/* Index44 -7.2dB */ 0x1c000070,
	/* Index45 -7.5dB */ 0x1b00006c,
	/* Index46 -7.8dB */ 0x1a000068,
	/* Index47 -8.1dB */ 0x19400065,
	/* Index48 -8.4dB */ 0x18400061,
	/* Index49 -8.7dB */ 0x1780005e,
	/* Index50 -9  dB */ 0x16c0005b,
	/* Index51 -9.3dB */ 0x16000058,
	/* Index52 -9.6dB */ 0x15400055,
	/* Index53 -9.9dB */ 0x14800052
};
u1Byte CCKSwingTable_Ch1_Ch13_92E[CCK_TABLE_SIZE_92E][8] = {
	/* Index0    0  dB */    {0x36, 0x34 , 0x2E , 0x26 , 0x1C , 0x12 , 0x08 , 0x04},
	/* Index1   -0.3dB */    {0x34, 0x32 , 0x2C , 0x25 , 0x1B , 0x11 , 0x08 , 0x04},
	/* Index2   -0.6dB */    {0x32, 0x30 , 0x2B , 0x23 , 0x1A , 0x11 , 0x07 , 0x04},
	/* Index3   -0.9dB */    {0x31, 0x2F , 0x29 , 0x22 , 0x19 , 0x10 , 0x07 , 0x04},
	/* Index4   -1.2dB */    {0x2F, 0x2D , 0x28 , 0x21 , 0x18 , 0x10 , 0x07 , 0x03},
	/* Index5   -1.5dB */    {0x2D, 0x2C , 0x27 , 0x20 , 0x18 , 0x0F , 0x07 , 0x03},
	/* Index6   -1.8dB */    {0x2C, 0x2A , 0x25 , 0x1F , 0x17 , 0x0F , 0x06 , 0x03},
	/* Index7   -2.1dB */    {0x2A, 0x29 , 0x24 , 0x1E , 0x16 , 0x0E , 0x06 , 0x03},
	/* Index8   -2.4dB */    {0x29, 0x27 , 0x23 , 0x1D , 0x15 , 0x0E , 0x06 , 0x03},
	/* Index9   -2.7dB */    {0x27, 0x26 , 0x22 , 0x1C , 0x14 , 0x0D , 0x06 , 0x03},
	/* Index10  -3  dB */    {0x26, 0x25 , 0x20 , 0x1B , 0x14 , 0x0D , 0x06 , 0x03},
	/* Index11  -3.3dB */    {0x25, 0x23 , 0x1F , 0x1A , 0x13 , 0x0C , 0x05 , 0x03},
	/* Index12  -3.6dB */    {0x24, 0x22 , 0x1E , 0x19 , 0x12 , 0x0C , 0x05 , 0x03},
	/* Index13  -3.9dB */    {0x22, 0x21 , 0x1D , 0x18 , 0x12 , 0x0B , 0x05 , 0x03},
	/* Index14  -4.2dB */    {0x21, 0x20 , 0x1C , 0x17 , 0x11 , 0x0B , 0x05 , 0x02},
	/* Index15  -4.5dB */    {0x20, 0x1F , 0x1B , 0x17 , 0x11 , 0x0B , 0x05 , 0x02},
	/* Index16  -4.8dB */    {0x1F, 0x1E , 0x1A , 0x16 , 0x10 , 0x0A , 0x05 , 0x02},
	/* Index17  -5.1dB */    {0x1E, 0x1D , 0x1A , 0x15 , 0x10 , 0x0A , 0x04 , 0x02},
	/* Index18  -5.4dB */    {0x1D, 0x1C , 0x19 , 0x14 , 0x0F , 0x0A , 0x04 , 0x02},
	/* Index19  -5.7dB */    {0x1C, 0x1B , 0x18 , 0x14 , 0x0E , 0x09 , 0x04 , 0x02},
	/* Index20  -6.0dB */    {0x1B, 0x1A , 0x17 , 0x13 , 0x0E , 0x09 , 0x04 , 0x02}, // 20 This is CCK base index
	/* Index21  -6.3dB */    {0x1A, 0x19 , 0x16 , 0x12 , 0x0E , 0x09 , 0x04 , 0x02},
	/* Index22  -6.6dB */    {0x19, 0x18 , 0x15 , 0x12 , 0x0D , 0x08 , 0x04 , 0x02},
	/* Index23  -6.9dB */    {0x18, 0x17 , 0x15 , 0x11 , 0x0D , 0x08 , 0x04 , 0x02},
	/* Index24  -7.2dB */    {0x18, 0x17 , 0x14 , 0x11 , 0x0C , 0x08 , 0x03 , 0x02},
	/* Index25  -7.5dB */    {0x17, 0x16 , 0x13 , 0x10 , 0x0C , 0x08 , 0x03 , 0x02},
	/* Index26  -7.8dB */    {0x16, 0x15 , 0x13 , 0x0F , 0x0B , 0x07 , 0x03 , 0x02},
	/* Index27  -8.1dB */    {0x15, 0x14 , 0x12 , 0x0F , 0x0B , 0x07 , 0x03 , 0x02},
	/* Index28  -8.4dB */    {0x14, 0x14 , 0x11 , 0x0E , 0x0B , 0x07 , 0x03 , 0x02},
	/* Index29  -8.7dB */    {0x14, 0x13 , 0x11 , 0x0E , 0x0A , 0x07 , 0x03 , 0x01},
	/* Index30  -9.0dB */    {0x13, 0x12 , 0x10 , 0x0D , 0x0A , 0x06 , 0x03 , 0x01}, // 30 This is hp CCK base index
	/* Index31  -9.3dB */    {0x12, 0x12 , 0x0F , 0x0D , 0x0A , 0x06 , 0x03 , 0x01},
	/* Index32  -9.6dB */    {0x12, 0x11 , 0x0F , 0x0D , 0x09 , 0x06 , 0x03 , 0x01},
	/* Index33  -9.9dB */    {0x11, 0x11 , 0x0F , 0x0C , 0x09 , 0x06 , 0x03 , 0x01},
	/* Index34 -10.2dB */    {0x11, 0x11 , 0x0E , 0x0C , 0x09 , 0x06 , 0x02 , 0x01},
	/* Index35 -10.5dB */    {0x10, 0x0F , 0x0E , 0x0B , 0x08 , 0x05 , 0x02 , 0x01},
	/* Index36 -10.8dB */    {0x10, 0x0F , 0x0D , 0x0B , 0x08 , 0x05 , 0x02 , 0x01},
	/* Index37 -11.1dB */    {0x0F, 0x0E , 0x0D , 0x0A , 0x08 , 0x05 , 0x02 , 0x01},
	/* Index38 -11.4dB */    {0x0E, 0x0E , 0x0C , 0x0A , 0x07 , 0x05 , 0x02 , 0x01},
	/* Index39 -11.7dB */    {0x0E, 0x0D , 0x0C , 0x0A , 0x07 , 0x05 , 0x02 , 0x01},
	/* Index40 -12  dB */    {0x0E, 0x0D , 0x0C , 0x0A , 0x07 , 0x05 , 0x02 , 0x01},
	/* Index41 -12.3dB */    {0x0D, 0x0D , 0x0B , 0x09 , 0x07 , 0x04 , 0x02 , 0x01},
	/* Index42 -12.6dB */    {0x0D, 0x0C , 0x0B , 0x09 , 0x07 , 0x04 , 0x02 , 0x01},
	/* Index43 -12.9dB */    {0x0C, 0x0C , 0x0A , 0x09 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index44 -13.2dB */    {0x0C, 0x0B , 0x0A , 0x08 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index45 -13.5dB */    {0x0B, 0x0B , 0x0A , 0x08 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index46 -13.8dB */    {0x0B, 0x0B , 0x09 , 0x08 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index47 -14.1dB */    {0x0B, 0x0A , 0x09 , 0x07 , 0x06 , 0x04 , 0x02 , 0x01},
	/* Index48 -14.4dB */    {0x0A, 0x0A , 0x09 , 0x07 , 0x05 , 0x03 , 0x02 , 0x01},
	/* Index49 -14.7dB */    {0x0A, 0x0A , 0x08 , 0x07 , 0x05 , 0x03 , 0x01 , 0x01},
	/* Index50 -15  dB */    {0x0A, 0x09 , 0x08 , 0x07 , 0x05 , 0x03 , 0x01 , 0x01},
	/* Index51 -15.3dB */    {0x09, 0x09 , 0x08 , 0x06 , 0x05 , 0x03 , 0x01 , 0x01},
	/* Index52 -15.6dB */    {0x09, 0x09 , 0x08 , 0x06 , 0x05 , 0x03 , 0x01 , 0x01},
	/* Index53 -15.9dB */    {0x09, 0x08 , 0x07 , 0x06 , 0x04 , 0x03 , 0x01 , 0x01}
};
u1Byte CCKSwingTable_Ch14_92E[CCK_TABLE_SIZE_92E][8] = {
	/* Index0    0  dB */    {0x36, 0x34 , 0x2E , 0x26 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index1   -0.3dB */    {0x34, 0x32 , 0x2C , 0x25 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index2   -0.6dB */    {0x32, 0x30 , 0x2B , 0x23 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index3   -0.9dB */    {0x31, 0x2F , 0x29 , 0x22 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index4   -1.2dB */    {0x2F, 0x2D , 0x28 , 0x21 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index5   -1.5dB */    {0x2D, 0x2C , 0x27 , 0x20 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index6   -1.8dB */    {0x2C, 0x2A , 0x25 , 0x1F , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index7   -2.1dB */    {0x2A, 0x29 , 0x24 , 0x1E , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index8   -2.4dB */    {0x29, 0x27 , 0x23 , 0x1D , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index9   -2.7dB */    {0x27, 0x26 , 0x22 , 0x1C , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index10  -3  dB */    {0x26, 0x25 , 0x20 , 0x1B , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index11  -3.3dB */    {0x25, 0x23 , 0x1F , 0x1A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index12  -3.6dB */    {0x24, 0x22 , 0x1E , 0x19 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index13  -3.9dB */    {0x22, 0x21 , 0x1D , 0x18 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index14  -4.2dB */    {0x21, 0x20 , 0x1C , 0x17 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index15  -4.5dB */    {0x20, 0x1F , 0x1B , 0x17 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index16  -4.8dB */    {0x1F, 0x1E , 0x1A , 0x16 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index17  -5.1dB */    {0x1E, 0x1D , 0x1A , 0x15 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index18  -5.4dB */    {0x1D, 0x1C , 0x19 , 0x14 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index19  -5.7dB */    {0x1C, 0x1B , 0x18 , 0x14 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index20  -6  dB */     {0x1B, 0x1A , 0x17 , 0x13 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index21  -6.3dB */    {0x1A, 0x19 , 0x16 , 0x12 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index22  -6.6dB */    {0x19, 0x18 , 0x15 , 0x12 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index23  -6.9dB */    {0x18, 0x17 , 0x15 , 0x11 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index24  -7.2dB */    {0x18, 0x17 , 0x14 , 0x11 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index25  -7.5dB */    {0x17, 0x16 , 0x13 , 0x10 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index26  -7.8dB */    {0x16, 0x15 , 0x13 , 0x0F , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index27  -8.1dB */    {0x15, 0x14 , 0x12 , 0x0F , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index28  -8.4dB */    {0x14, 0x14 , 0x11 , 0x0E , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index29  -8.7dB */    {0x14, 0x13 , 0x11 , 0x0E , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index30  -9  dB */    {0x13, 0x12 , 0x10 , 0x0D , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index31  -9.3dB */    {0x12, 0x12 , 0x0F , 0x0D , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index32  -9.6dB */    {0x12, 0x11 , 0x0F , 0x0D , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index33  -9.9dB */    {0x11, 0x11 , 0x0F , 0x0C , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index34 -10.2dB */    {0x11, 0x11 , 0x0E , 0x0C , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index35 -10.5dB */    {0x10, 0x0F , 0x0E , 0x0B , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index36 -10.8dB */    {0x10, 0x0F , 0x0D , 0x0B , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index37 -11.1dB */    {0x0F, 0x0E , 0x0D , 0x0A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index38 -11.4dB */    {0x0E, 0x0E , 0x0C , 0x0A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index39 -11.7dB */    {0x0E, 0x0D , 0x0C , 0x0A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index40 -12  dB */    {0x0E, 0x0D , 0x0C , 0x0A , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index41 -12.3dB */    {0x0D, 0x0D , 0x0B , 0x09 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index42 -12.6dB */    {0x0D, 0x0C , 0x0B , 0x09 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index43 -12.9dB */    {0x0C, 0x0C , 0x0A , 0x09 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index44 -13.2dB */    {0x0C, 0x0B , 0x0A , 0x08 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index45 -13.5dB */    {0x0B, 0x0B , 0x0A , 0x08 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index46 -13.8dB */    {0x0B, 0x0B , 0x09 , 0x08 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index47 -14.1dB */    {0x0B, 0x0A , 0x09 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index48 -14.4dB */    {0x0A, 0x0A , 0x09 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index49 -14.7dB */    {0x0A, 0x0A , 0x08 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index50 -15  dB */    {0x0A, 0x09 , 0x08 , 0x07 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index51 -15.3dB */    {0x09, 0x09 , 0x08 , 0x06 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index52 -15.6dB */    {0x09, 0x09 , 0x08 , 0x06 , 0x00 , 0x00 , 0x00 , 0x00},
	/* Index53 -15.9dB */    {0x09, 0x08 , 0x07 , 0x06 , 0x00 , 0x00 , 0x00 , 0x00}
};
#endif



unsigned int OFDMSwingTable_8812[OFDM_TABLE_SIZE_8812] = {	
	0x3FE, // 0,  (6dB) 
	0x3C4, // 1,  (5.5dB) 
	0x38E, // 2,  (5dB)
	0x35C, // 3,  (4.5dB)
	0x32B, // 4,  (4dB)
	0x2FE, // 5,  (3.5dB)
	0x2D3, // 6,  (3dB)
	0x2AB, // 7,  (2.5dB)
	0x285, // 8,  (2dB)
	0x261, // 9,  (1.5dB
	0x23E, // 10, (1dB)
	0x21E, // 11, (0.5dB)
	0x200, // 12, (0dB)
	0x1E3, // 13, (-0.5dB)
	0x1C8, // 14, (-1dB)
	0x1AF, // 15, (-1.5dB)
	0x197, // 16, (-2dB)
	0x180, // 17, (-2.5dB)
	0x16A, // 18, (-3dB)
	0x156, // 19, (-3.5dB)
	0x143, // 20, (-4dB)
	0x131, // 21, (-4.5dB)
	0x120, // 22, (-5dB)
	0x110, // 23, (-5.5dB)
	0x101, // 24, (-6dB)
	0x0F2, // 25, (-6.5dB)
	0x0E5, // 26, (-7dB)
	0x0D8, // 27, (-7.5dB)
	0x0CC, // 28, (-8dB)
	0x0C0, // 29, (-8.5dB)
	0x0B6, // 30, (-9dB)
	0x0AC, // 31, (-9.5dB)
	0x0A2, // 32, (-10dB)
	0x099, // 33, (-10.5dB)
	0x090, // 34, (-11dB)
	0x088, // 35, (-11.5dB)
	0x081, // 36, (-12dB)	
	0x079, // 37, (-12.5dB)
	0x072, // 38, (-13dB)
	0x06c, // 39, (-13.5dB)
	0x066, // 40, (-14dB)
	0x060, // 41, (-14.5dB)
	0x05B  // 42, (-15dB)
};


//============================================================
// Local Function predefine.
//============================================================

//START------------COMMON INFO RELATED---------------//
VOID
odm_CommonInfoSelfInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_CommonInfoSelfUpdate(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_CmnInfoInit_Debug(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_CmnInfoHook_Debug(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_CmnInfoUpdate_Debug(
	IN		PDM_ODM_T		pDM_Odm
	);
/*
VOID
odm_FindMinimumRSSI(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_IsLinked(
	IN		PDM_ODM_T		pDM_Odm
	);
*/
//END------------COMMON INFO RELATED---------------//

//START-------BB POWER SAVE-----------------------//
VOID 
odm_DynamicBBPowerSavingInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID 
odm_DynamicBBPowerSaving(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_1R_CCA(
	IN		PDM_ODM_T		pDM_Odm
	);
//END---------BB POWER SAVE-----------------------//

//START-----------------PSD-----------------------//
#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN)) 
//============================================================
// Function predefine.
//============================================================
VOID	odm_PathDiversityInit_92C(	IN	PADAPTER	Adapter);
VOID	odm_2TPathDiversityInit_92C(	IN	PADAPTER	Adapter);
VOID	odm_1TPathDiversityInit_92C(	IN	PADAPTER	Adapter);
BOOLEAN	odm_IsConnected_92C(IN	PADAPTER	Adapter);
VOID	odm_PathDiversityAfterLink_92C(	IN	PADAPTER	Adapter);

VOID
odm_CCKTXPathDiversityCallback(
	PRT_TIMER		pTimer
	);

VOID
odm_CCKTXPathDiversityWorkItemCallback(
    IN PVOID            pContext
    );

VOID
odm_PathDivChkAntSwitchCallback(
	PRT_TIMER		pTimer
	);

VOID
odm_PathDivChkAntSwitchWorkitemCallback(
    IN PVOID            pContext
    );

VOID	odm_SetRespPath_92C(		IN	PADAPTER	Adapter, 	IN	u1Byte	DefaultRespPath);
VOID	odm_OFDMTXPathDiversity_92C(	IN	PADAPTER	Adapter);
VOID	odm_CCKTXPathDiversity_92C(	IN	PADAPTER	Adapter);
VOID	odm_ResetPathDiversity_92C(		IN	PADAPTER	Adapter);

//Start-------------------- RX High Power------------------------//
VOID	odm_RXHPInit(	IN		PDM_ODM_T		pDM_Odm);
VOID	odm_RXHP(	IN		PDM_ODM_T		pDM_Odm);
VOID	odm_Write_RXHP(	IN	PDM_ODM_T	pDM_Odm);

VOID	odm_PSD_RXHP(		IN	PDM_ODM_T	pDM_Odm);
VOID	odm_PSD_RXHPCallback(	PRT_TIMER		pTimer);
VOID	odm_PSD_RXHPWorkitemCallback(	IN PVOID            pContext);
//End--------------------- RX High Power -----------------------//

VOID
odm_PathDivInit(	IN	PDM_ODM_T 	pDM_Odm);

VOID
odm_SetRespPath_92C(
	IN	PADAPTER	Adapter,
	IN	u1Byte	DefaultRespPath 
	);

#endif
//END-------------------PSD-----------------------//

VOID
odm_RefreshRateAdaptiveMaskMP(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_RefreshRateAdaptiveMaskCE(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_RefreshRateAdaptiveMaskAPADSL(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID 
odm_DynamicTxPowerInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_DynamicTxPowerRestorePowerIndex(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID 
odm_DynamicTxPowerNIC(
	IN	PDM_ODM_T	pDM_Odm
	);

#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
VOID
odm_DynamicTxPowerSavePowerIndex(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_DynamicTxPowerWritePowerIndex(
	IN	PDM_ODM_T	pDM_Odm, 
	IN 	u1Byte		Value);

VOID 
odm_DynamicTxPower_92C(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID 
odm_DynamicTxPower_92D(
	IN	PDM_ODM_T	pDM_Odm
	);
#endif


VOID
odm_RSSIMonitorInit(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_RSSIMonitorCheckMP(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID 
odm_RSSIMonitorCheckCE(
	IN		PDM_ODM_T		pDM_Odm
	);
VOID 
odm_RSSIMonitorCheckAP(
	IN		PDM_ODM_T		pDM_Odm
	);



VOID
odm_RSSIMonitorCheck(
	IN		PDM_ODM_T		pDM_Odm
	);
VOID 
odm_DynamicTxPower(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID 
odm_DynamicTxPowerAP(
	IN		PDM_ODM_T		pDM_Odm
	);


VOID
odm_SwAntDivInit(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_SwAntDivInit_NIC(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_SwAntDivChkAntSwitch(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			Step
	);

VOID
odm_SwAntDivChkAntSwitchNIC(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte		Step
	);


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
odm_SwAntDivChkAntSwitchCallback(
	PRT_TIMER		pTimer
);
VOID
odm_SwAntDivChkAntSwitchWorkitemCallback(
    IN PVOID            pContext
    );
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext);
#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext);
#endif



VOID
odm_GlobalAdapterCheck(
	IN		VOID
	);

VOID
odm_RefreshRateAdaptiveMask(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
ODM_TXPowerTrackingCheck(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_TXPowerTrackingCheckAP(
	IN		PDM_ODM_T		pDM_Odm
	);

VOID
odm_RateAdaptiveMaskInit(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_TXPowerTrackingThermalMeterInit(
	IN	PDM_ODM_T	pDM_Odm
	);


VOID
odm_TXPowerTrackingInit(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_TXPowerTrackingCheckMP(
	IN	PDM_ODM_T	pDM_Odm
	);


VOID
odm_TXPowerTrackingCheckCE(
	IN	PDM_ODM_T	pDM_Odm
	);

#if(DM_ODM_SUPPORT_TYPE & (ODM_WIN)) 

VOID
ODM_RateAdaptiveStateApInit(
	IN	PADAPTER	Adapter	,
	IN	PRT_WLAN_STA  pEntry
	);

VOID 
odm_TXPowerTrackingCallbackThermalMeter92C(
            IN PADAPTER	Adapter
            );

VOID
odm_TXPowerTrackingCallbackRXGainThermalMeter92D(
	IN PADAPTER 	Adapter
	);

VOID
odm_TXPowerTrackingCallbackThermalMeter92D(
            IN PADAPTER	Adapter
            );

VOID
odm_TXPowerTrackingDirectCall92C(
            IN	PADAPTER		Adapter
            );

VOID
odm_TXPowerTrackingThermalMeterCheck(
	IN	PADAPTER		Adapter
	);

#endif

VOID
odm_EdcaTurboCheck(
	IN		PDM_ODM_T		pDM_Odm
	);
VOID
ODM_EdcaTurboInit(
	IN	PDM_ODM_T		pDM_Odm
);

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
VOID
odm_EdcaTurboCheckMP(
	IN		PDM_ODM_T		pDM_Odm
	);

//check if edca turbo is disabled
BOOLEAN
odm_IsEdcaTurboDisable(
	IN 	PDM_ODM_T 	pDM_Odm
);
//choose edca paramter for special IOT case
VOID 
ODM_EdcaParaSelByIot(
	IN 	PDM_ODM_T 	pDM_Odm,
	OUT	u4Byte		*EDCA_BE_UL,
	OUT u4Byte		*EDCA_BE_DL
	);
//check if it is UL or DL
VOID
odm_EdcaChooseTrafficIdx( 
	IN	PDM_ODM_T		pDM_Odm,
	IN	u8Byte  			cur_tx_bytes,  
	IN	u8Byte  			cur_rx_bytes, 
	IN	BOOLEAN 		bBiasOnRx,
	OUT BOOLEAN 		*pbIsCurRDLState
	);

#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)
VOID
odm_EdcaTurboCheckCE(
	IN		PDM_ODM_T		pDM_Odm
	);
#else
VOID 
odm_IotEngine(
	IN	PDM_ODM_T	pDM_Odm
	);

VOID
odm_EdcaParaInit(
	IN	PDM_ODM_T	pDM_Odm
	);
#endif



#define 	RxDefaultAnt1		0x65a9
#define	RxDefaultAnt2		0x569a

VOID
odm_InitHybridAntDiv(
	IN PDM_ODM_T	pDM_Odm 
	);

BOOLEAN
odm_StaDefAntSel(
	IN PDM_ODM_T	pDM_Odm,
	IN u4Byte		OFDM_Ant1_Cnt,
	IN u4Byte		OFDM_Ant2_Cnt,
	IN u4Byte		CCK_Ant1_Cnt,
	IN u4Byte		CCK_Ant2_Cnt,
	OUT u1Byte		*pDefAnt 
	);

VOID
odm_SetRxIdleAnt(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte	Ant,
	IN   BOOLEAN   bDualPath                     
);



VOID
odm_HwAntDiv(
	IN	PDM_ODM_T	pDM_Odm
);





#if 0
//#if ((DM_ODM_SUPPORT_TYPE==ODM_AP)&&defined(HW_ANT_SWITCH))
VOID
odm_HW_AntennaSwitchInit(
	IN	PDM_ODM_T	pDM_Odm
);

VOID
odm_SetRxIdleAnt(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte		Ant
);

VOID
odm_StaAntSelect(
	IN	PDM_ODM_T	pDM_Odm,
	IN	struct stat_info *pstat
);

VOID
odm_HW_IdleAntennaSelect(
	IN	PDM_ODM_T	pDM_Odm
);

u1Byte
ODM_Diversity_AntennaSelect(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte	*data
);
#endif


//============================================================
//3 Export Interface
//============================================================

//
// 2011/09/21 MH Add to describe different team necessary resource allocate??
//
VOID
ODM_DMInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	//2012.05.03 Luke: For all IC series
	odm_CommonInfoSelfInit(pDM_Odm);
	odm_CmnInfoInit_Debug(pDM_Odm);
	odm_NHMCounterStatisticsInit(pDM_Odm);
	odm_DIGInit(pDM_Odm);
	odm_AdaptivityInit(pDM_Odm);
	odm_RateAdaptiveMaskInit(pDM_Odm);
	ODM_EdcaTurboInit(pDM_Odm);

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		#if (MP_DRIVER != 1)&&(defined(CONFIG_HW_ANTENNA_DIVERSITY))
		ODM_AntDivInit(pDM_Odm);
		#endif
	}
	else if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
		odm_DynamicBBPowerSavingInit(pDM_Odm);
		odm_DynamicTxPowerInit(pDM_Odm);
		odm_TXPowerTrackingInit(pDM_Odm);
		//ODM_EdcaTurboInit(pDM_Odm);
#if (MP_DRIVER != 1) && (defined(CONFIG_HW_ANTENNA_DIVERSITY))		
		if(pDM_Odm->SupportICType==ODM_RTL8723A)
			odm_SwAntDivInit(pDM_Odm);	
		else if(pDM_Odm->SupportICType & (ODM_RTL8192C|ODM_RTL8192D))
		{
			if(pDM_Odm->AntDivType == HW_ANTDIV)
			odm_InitHybridAntDiv(pDM_Odm);
			else
			odm_SwAntDivInit(pDM_Odm);
		}
		else
		ODM_AntDivInit(pDM_Odm);	
#endif

//2010.05.30 LukeLee: For CE platform, files in IC subfolders may not be included to be compiled,
// so compile flags must be left here to prevent from compile errors
#if (RTL8188E_SUPPORT == 1)
		odm_PrimaryCCA_Init(pDM_Odm);    // Gary
		ODM_RAInfo_Init_all(pDM_Odm);
#endif		

//2010.05.30 LukeLee: Following are not incorporated into ODM structure yet.
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
		odm_PSDMonitorInit(pDM_Odm);
	  	odm_RXHPInit(pDM_Odm);	
	  	odm_PathDivInit(pDM_Odm); //92D Path Div Init   //Neil Chen
#endif
	}
}

//
// 2011/09/20 MH This is the entry pointer for all team to execute HW out source DM.
// You can not add any dummy function here, be care, you can only use DM structure
// to perform any new ODM_DM.
//
VOID
ODM_DMWatchdog(
	IN		PDM_ODM_T		pDM_Odm
	)
{	
	odm_CmnInfoHook_Debug(pDM_Odm);
	odm_CmnInfoUpdate_Debug(pDM_Odm);
	odm_CommonInfoSelfUpdate(pDM_Odm);

#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	prtl8192cd_priv priv		= pDM_Odm->priv;
	if( (priv->auto_channel != 0) && (priv->auto_channel != 2) )//if ACS running, do not do FA/CCA counter read
		return;
	else
#endif	
		odm_FalseAlarmCounterStatistics(pDM_Odm);

	
	odm_CheckAdaptivity(pDM_Odm);
	odm_DIG(pDM_Odm);
	{
		pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;
		pDM_Odm->bAdaOn = odm_Adaptivity(pDM_Odm, pDM_DigTable->CurIGValue);
	}
	odm_CCKPacketDetectionThresh(pDM_Odm);
	
	if(*(pDM_Odm->pbPowerSaving)==TRUE)
		return;
	
	odm_RefreshRateAdaptiveMask(pDM_Odm);
	odm_RSSIMonitorCheck(pDM_Odm);
	odm_DynamicBBPowerSaving(pDM_Odm);	
	odm_EdcaTurboCheck(pDM_Odm);
#if (MP_DRIVER != 1)&& (defined(CONFIG_HW_ANTENNA_DIVERSITY))		
// <20120622, Kordan> MP Driver is always controlled by HW.	
        if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		odm_SwAntDivChkAntSwitch(pDM_Odm, SWAW_STEP_PEAK);
	}
	else if(pDM_Odm->SupportICType & (ODM_RTL8192C|ODM_RTL8192D))
	{
		if(pDM_Odm->AntDivType == HW_ANTDIV)
			odm_HwAntDiv(pDM_Odm);
		else
			odm_SwAntDivChkAntSwitch(pDM_Odm, SWAW_STEP_PEAK);
	}
	else	
		ODM_AntDiv(pDM_Odm);

#endif

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		odm_DynamicTxPower(pDM_Odm);	
	}
	else if(pDM_Odm->SupportICType & ODM_IC_11N_SERIES)
	{
		//odm_EdcaTurboCheck(pDM_Odm);
		odm_DynamicTxPower(pDM_Odm);	
		#if( DM_ODM_SUPPORT_TYPE & (ODM_WIN))	
		odm_RXHP(pDM_Odm);
		#endif

//2010.05.30 LukeLee: For CE platform, files in IC subfolders may not be included to be compiled,
// so compile flags must be left here to prevent from compile errors
#if (RTL8192D_SUPPORT == 1)
		ODM_DynamicEarlyMode(pDM_Odm);
#endif
#if (RTL8188E_SUPPORT == 1)
		odm_DynamicPrimaryCCA(pDM_Odm);	
#endif
		
	}
}


//
// Init /.. Fixed HW value. Only init time.
//
VOID
ODM_CmnInfoInit(
	IN		PDM_ODM_T		pDM_Odm,
	IN		ODM_CMNINFO_E	CmnInfo,
	IN		u4Byte			Value	
	)
{
	//ODM_RT_TRACE(pDM_Odm,);

	//
	// This section is used for init value
	//
	switch	(CmnInfo)
	{
		//
		// Fixed ODM value.
		//
		case	ODM_CMNINFO_ABILITY:
			pDM_Odm->SupportAbility = (u4Byte)Value;
			break;		
		case	ODM_CMNINFO_PLATFORM:
			pDM_Odm->SupportPlatform = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_INTERFACE:
			pDM_Odm->SupportInterface = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_MP_TEST_CHIP:
			pDM_Odm->bIsMPChip= (u1Byte)Value;
			break;
            
		case	ODM_CMNINFO_IC_TYPE:
			pDM_Odm->SupportICType = Value;
			break;

		case	ODM_CMNINFO_CUT_VER:
			pDM_Odm->CutVersion = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_FAB_VER:
			pDM_Odm->FabVersion = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_RF_TYPE:
			pDM_Odm->RFType = (u1Byte)Value;
			break;

		case    ODM_CMNINFO_RF_ANTENNA_TYPE:
			pDM_Odm->AntDivType= (u1Byte)Value;
			break;

		case	ODM_CMNINFO_BOARD_TYPE:
			pDM_Odm->BoardType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_PACKAGE_TYPE:
			pDM_Odm->PackageType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_EXT_LNA:
			pDM_Odm->ExtLNA = (u1Byte)Value;
			break;
			
		case	ODM_CMNINFO_5G_EXT_LNA:
			pDM_Odm->ExtLNA5G = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_EXT_PA:
			pDM_Odm->ExtPA = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_5G_EXT_PA:
			pDM_Odm->ExtPA5G = (u1Byte)Value;
			break;
			
		case	ODM_CMNINFO_GPA:
			pDM_Odm->TypeGPA= (ODM_TYPE_GPA_E)Value;
			break;
		case	ODM_CMNINFO_APA:
			pDM_Odm->TypeAPA= (ODM_TYPE_APA_E)Value;
			break;
		case	ODM_CMNINFO_GLNA:
			pDM_Odm->TypeGLNA= (ODM_TYPE_GLNA_E)Value;
			break;
		case	ODM_CMNINFO_ALNA:
			pDM_Odm->TypeALNA= (ODM_TYPE_ALNA_E)Value;
			break;

		case	ODM_CMNINFO_EXT_TRSW:
			pDM_Odm->ExtTRSW = (u1Byte)Value;
			break;
		case 	ODM_CMNINFO_PATCH_ID:
			pDM_Odm->PatchID = (u1Byte)Value;
			break;
		case 	ODM_CMNINFO_BINHCT_TEST:
			pDM_Odm->bInHctTest = (BOOLEAN)Value;
			break;
		case 	ODM_CMNINFO_BWIFI_TEST:
			pDM_Odm->bWIFITest = (BOOLEAN)Value;
			break;	

		case	ODM_CMNINFO_SMART_CONCURRENT:
			pDM_Odm->bDualMacSmartConcurrent = (BOOLEAN )Value;
			break;

		case	ODM_CMNINFO_CONFIG_BB_RF:
			pDM_Odm->ConfigBBRF = (BOOLEAN)Value;
		
		//To remove the compiler warning, must add an empty default statement to handle the other values.	
		default:
			//do nothing
			break;	
		
	}

}


VOID
ODM_CmnInfoHook(
	IN		PDM_ODM_T		pDM_Odm,
	IN		ODM_CMNINFO_E	CmnInfo,
	IN		PVOID			pValue	
	)
{
	//
	// Hook call by reference pointer.
	//
	switch	(CmnInfo)
	{
		//
		// Dynamic call by reference pointer.
		//
		case	ODM_CMNINFO_MAC_PHY_MODE:
			pDM_Odm->pMacPhyMode = (u1Byte *)pValue;
			break;
		
		case	ODM_CMNINFO_TX_UNI:
			pDM_Odm->pNumTxBytesUnicast = (u8Byte *)pValue;
			break;

		case	ODM_CMNINFO_RX_UNI:
			pDM_Odm->pNumRxBytesUnicast = (u8Byte *)pValue;
			break;

		case	ODM_CMNINFO_WM_MODE:
			pDM_Odm->pWirelessMode = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_BAND:
			pDM_Odm->pBandType = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_SEC_CHNL_OFFSET:
			pDM_Odm->pSecChOffset = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_SEC_MODE:
			pDM_Odm->pSecurity = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_BW:
			pDM_Odm->pBandWidth = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_CHNL:
			pDM_Odm->pChannel = (u1Byte *)pValue;
			break;
		
		case	ODM_CMNINFO_DMSP_GET_VALUE:
			pDM_Odm->pbGetValueFromOtherMac = (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_BUDDY_ADAPTOR:
			pDM_Odm->pBuddyAdapter = (PADAPTER *)pValue;
			break;

		case	ODM_CMNINFO_DMSP_IS_MASTER:
			pDM_Odm->pbMasterOfDMSP = (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_SCAN:
			pDM_Odm->pbScanInProcess = (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_POWER_SAVING:
			pDM_Odm->pbPowerSaving = (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_ONE_PATH_CCA:
			pDM_Odm->pOnePathCCA = (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_DRV_STOP:
			pDM_Odm->pbDriverStopped =  (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_PNP_IN:
			pDM_Odm->pbDriverIsGoingToPnpSetPowerSleep =  (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_INIT_ON:
			pDM_Odm->pinit_adpt_in_progress =  (BOOLEAN *)pValue;
			break;

		case	ODM_CMNINFO_ANT_TEST:
			pDM_Odm->pAntennaTest =  (u1Byte *)pValue;
			break;

		case	ODM_CMNINFO_NET_CLOSED:
			pDM_Odm->pbNet_closed = (BOOLEAN *)pValue;
			break;

		//case	ODM_CMNINFO_RTSTA_AID:
		//	pDM_Odm->pAidMap =  (u1Byte *)pValue;
		//	break;

		//case	ODM_CMNINFO_BT_COEXIST:
		//	pDM_Odm->BTCoexist = (BOOLEAN *)pValue;		

		//case	ODM_CMNINFO_STA_STATUS:
			//pDM_Odm->pODM_StaInfo[] = (PSTA_INFO_T)pValue;
			//break;

		//case	ODM_CMNINFO_PHY_STATUS:
		//	pDM_Odm->pPhyInfo = (ODM_PHY_INFO *)pValue;
		//	break;

		//case	ODM_CMNINFO_MAC_STATUS:
		//	pDM_Odm->pMacInfo = (ODM_MAC_INFO *)pValue;
		//	break;
		//To remove the compiler warning, must add an empty default statement to handle the other values.				
		default:
			//do nothing
			break;

	}

}


VOID
ODM_CmnInfoPtrArrayHook(
	IN		PDM_ODM_T		pDM_Odm,
	IN		ODM_CMNINFO_E	CmnInfo,
	IN		u2Byte			Index,
	IN		PVOID			pValue	
	)
{
	//
	// Hook call by reference pointer.
	//
	switch	(CmnInfo)
	{
		//
		// Dynamic call by reference pointer.
		//		
		case	ODM_CMNINFO_STA_STATUS:
			pDM_Odm->pODM_StaInfo[Index] = (PSTA_INFO_T)pValue;
			break;		
		//To remove the compiler warning, must add an empty default statement to handle the other values.				
		default:
			//do nothing
			break;
	}
	
}


//
// Update Band/CHannel/.. The values are dynamic but non-per-packet.
//
VOID
ODM_CmnInfoUpdate(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u4Byte			CmnInfo,
	IN		u8Byte			Value	
	)
{
	//
	// This init variable may be changed in run time.
	//
	switch	(CmnInfo)
	{
		case	ODM_CMNINFO_ABILITY:
			pDM_Odm->SupportAbility = (u4Byte)Value;
			break;

		case	ODM_CMNINFO_RF_TYPE:
			pDM_Odm->RFType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_WIFI_DIRECT:
			pDM_Odm->bWIFI_Direct = (BOOLEAN)Value;
			break;

		case	ODM_CMNINFO_WIFI_DISPLAY:
			pDM_Odm->bWIFI_Display = (BOOLEAN)Value;
			break;

		case	ODM_CMNINFO_LINK:
			pDM_Odm->bLinked = (BOOLEAN)Value;
			break;

		case	ODM_CMNINFO_RSSI_MIN:
			pDM_Odm->RSSI_Min= (u1Byte)Value;
			break;

		case	ODM_CMNINFO_DBG_COMP:
			pDM_Odm->DebugComponents = Value;
			break;

		case	ODM_CMNINFO_DBG_LEVEL:
			pDM_Odm->DebugLevel = (u4Byte)Value;
			break;
		case	ODM_CMNINFO_RA_THRESHOLD_HIGH:
			pDM_Odm->RateAdaptive.HighRSSIThresh = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_RA_THRESHOLD_LOW:
			pDM_Odm->RateAdaptive.LowRSSIThresh = (u1Byte)Value;
			break;
#if defined(BT_30_SUPPORT) && (BT_30_SUPPORT == 1)
		// The following is for BT HS mode and BT coexist mechanism.
		case ODM_CMNINFO_BT_DISABLED:
			pDM_Odm->bBtDisabled = (BOOLEAN)Value;
			break;
			
		case	ODM_CMNINFO_BT_OPERATION:
			pDM_Odm->bBtHsOperation = (BOOLEAN)Value;
			break;

		case ODM_CMNINFO_BT_DIG:
			pDM_Odm->btHsDigVal = (u1Byte)Value;
			break;
			
		case	ODM_CMNINFO_BT_BUSY:
			pDM_Odm->bBtBusy = (BOOLEAN)Value;
			break;	

		case	ODM_CMNINFO_BT_DISABLE_EDCA:
			pDM_Odm->bBtDisableEdcaTurbo = (BOOLEAN)Value;
			break;
#endif

#if(DM_ODM_SUPPORT_TYPE & ODM_AP)		// for repeater mode add by YuChen 2014.06.23
#ifdef UNIVERSAL_REPEATER
		case	ODM_CMNINFO_VXD_LINK:
			pDM_Odm->VXD_bLinked= (BOOLEAN)Value;
			break;
#endif
#endif

/*
		case	ODM_CMNINFO_OP_MODE:
			pDM_Odm->OPMode = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_WM_MODE:
			pDM_Odm->WirelessMode = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_BAND:
			pDM_Odm->BandType = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_SEC_CHNL_OFFSET:
			pDM_Odm->SecChOffset = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_SEC_MODE:
			pDM_Odm->Security = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_BW:
			pDM_Odm->BandWidth = (u1Byte)Value;
			break;

		case	ODM_CMNINFO_CHNL:
			pDM_Odm->Channel = (u1Byte)Value;
			break;			
*/	
	}

	
}

VOID
odm_CommonInfoSelfInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pFAT_T			pDM_FatTable = &pDM_Odm->DM_FatTable;
	pDM_Odm->bCckHighPower = (BOOLEAN) ODM_GetBBReg(pDM_Odm, ODM_REG(CCK_RPT_FORMAT,pDM_Odm), ODM_BIT(CCK_RPT_FORMAT,pDM_Odm));		
	pDM_Odm->RFPathRxEnable = (u1Byte) ODM_GetBBReg(pDM_Odm, ODM_REG(BB_RX_PATH,pDM_Odm), ODM_BIT(BB_RX_PATH,pDM_Odm));
#if (DM_ODM_SUPPORT_TYPE != ODM_CE)	
	pDM_Odm->pbNet_closed = &pDM_Odm->BOOLEAN_temp;
#endif
	ODM_InitDebugSetting(pDM_Odm);
	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		pDM_Odm->AntDivType = SW_ANTDIV;
	}
	else if(pDM_Odm->SupportICType & (ODM_RTL8192C|ODM_RTL8192D))
	{
#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))	
		pDM_Odm->AntDivType = HW_ANTDIV;
		#elif (defined(CONFIG_SW_ANTENNA_DIVERSITY))
		pDM_Odm->AntDivType = CGCS_RX_SW_ANTDIV;
		#endif
	}
	else
	{
		#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
		#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
			ODM_AP_AntDiv_Config(pDM_Odm);
		#endif  
		#endif 
	}
	
}

VOID
odm_CommonInfoSelfUpdate(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	u1Byte	EntryCnt=0;
	u4Byte	i;
	PSTA_INFO_T   	pEntry;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

	PADAPTER	Adapter =  pDM_Odm->Adapter;
	PMGNT_INFO	pMgntInfo = &Adapter->MgntInfo;

	pEntry = pDM_Odm->pODM_StaInfo[0];
	if(pMgntInfo->mAssoc)
	{
		pEntry->bUsed=TRUE;
		for (i=0; i<6; i++)
			pEntry->MacAddr[i] = pMgntInfo->Bssid[i];
	}
	else
	{
		pEntry->bUsed=FALSE;
		for (i=0; i<6; i++)
			pEntry->MacAddr[i] = 0;
	}
#endif


	if(*(pDM_Odm->pBandWidth) == ODM_BW40M)
	{
		if(*(pDM_Odm->pSecChOffset) == 1)
			pDM_Odm->ControlChannel = *(pDM_Odm->pChannel) +2;
		else if(*(pDM_Odm->pSecChOffset) == 2)
			pDM_Odm->ControlChannel = *(pDM_Odm->pChannel) -2;
	}
	else if(*(pDM_Odm->pBandWidth) == ODM_BW80M)
	{
		if(*(pDM_Odm->pSecChOffset) == 1)
			pDM_Odm->ControlChannel = *(pDM_Odm->pChannel) +6;
		else if(*(pDM_Odm->pSecChOffset) == 2)
			pDM_Odm->ControlChannel = *(pDM_Odm->pChannel) -6;
	}
	else
		pDM_Odm->ControlChannel = *(pDM_Odm->pChannel);

	for (i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		pEntry = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pEntry))
			EntryCnt++;
	}
	if(EntryCnt == 1)
		pDM_Odm->bOneEntryOnly = TRUE;
	else
		pDM_Odm->bOneEntryOnly = FALSE;
}

VOID
odm_CmnInfoInit_Debug(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("odm_CmnInfoInit_Debug==>\n"));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("SupportPlatform=%d\n",pDM_Odm->SupportPlatform) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("SupportAbility=0x%x\n",pDM_Odm->SupportAbility) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("SupportInterface=%d\n",pDM_Odm->SupportInterface) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("SupportICType=0x%x\n",pDM_Odm->SupportICType) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("CutVersion=%d\n",pDM_Odm->CutVersion) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("FabVersion=%d\n",pDM_Odm->FabVersion) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("RFType=%d\n",pDM_Odm->RFType) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("BoardType=%d\n",pDM_Odm->BoardType) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("ExtLNA=%d\n",pDM_Odm->ExtLNA) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("ExtPA=%d\n",pDM_Odm->ExtPA) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("ExtTRSW=%d\n",pDM_Odm->ExtTRSW) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("PatchID=%d\n",pDM_Odm->PatchID) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bInHctTest=%d\n",pDM_Odm->bInHctTest) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bWIFITest=%d\n",pDM_Odm->bWIFITest) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bDualMacSmartConcurrent=%d\n",pDM_Odm->bDualMacSmartConcurrent) );

}

VOID
odm_CmnInfoHook_Debug(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("odm_CmnInfoHook_Debug==>\n"));	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pNumTxBytesUnicast=%llu\n",*(pDM_Odm->pNumTxBytesUnicast)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pNumRxBytesUnicast=%llu\n",*(pDM_Odm->pNumRxBytesUnicast)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pWirelessMode=0x%x\n",*(pDM_Odm->pWirelessMode)) );	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pSecChOffset=%d\n",*(pDM_Odm->pSecChOffset)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pSecurity=%d\n",*(pDM_Odm->pSecurity)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pBandWidth=%d\n",*(pDM_Odm->pBandWidth)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pChannel=%d\n",*(pDM_Odm->pChannel)) );

#if (RTL8192D_SUPPORT==1) 
	if(pDM_Odm->pBandType)
	    ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pBandType=%d\n",*(pDM_Odm->pBandType)) );
	if(pDM_Odm->pMacPhyMode)
	    ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pMacPhyMode=%d\n",*(pDM_Odm->pMacPhyMode)) );
	if(pDM_Odm->pBuddyAdapter)
	    ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pbGetValueFromOtherMac=%d\n",*(pDM_Odm->pbGetValueFromOtherMac)) );
	if(pDM_Odm->pBuddyAdapter)
	    ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pBuddyAdapter=%p\n",*(pDM_Odm->pBuddyAdapter)) );
	if(pDM_Odm->pbMasterOfDMSP)
	    ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pbMasterOfDMSP=%d\n",*(pDM_Odm->pbMasterOfDMSP)) );
#endif
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pbScanInProcess=%d\n",*(pDM_Odm->pbScanInProcess)) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pbPowerSaving=%d\n",*(pDM_Odm->pbPowerSaving)) );

	if(pDM_Odm->SupportPlatform & (ODM_AP|ODM_ADSL))
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("pOnePathCCA=%d\n",*(pDM_Odm->pOnePathCCA)) );
}

VOID
odm_CmnInfoUpdate_Debug(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("odm_CmnInfoUpdate_Debug==>\n"));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bWIFI_Direct=%d\n",pDM_Odm->bWIFI_Direct) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bWIFI_Display=%d\n",pDM_Odm->bWIFI_Display) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("bLinked=%d\n",pDM_Odm->bLinked) );
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_COMMON, ODM_DBG_LOUD, ("RSSI_Min=%d\n",pDM_Odm->RSSI_Min) );
}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
ODM_InitAllWorkItems(IN PDM_ODM_T	pDM_Odm )
{
#if USE_WORKITEM
	PADAPTER		pAdapter = pDM_Odm->Adapter;

	ODM_InitializeWorkItem(	pDM_Odm, 
							&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchWorkitem, 
							(RT_WORKITEM_CALL_BACK)odm_SwAntDivChkAntSwitchWorkitemCallback,
							(PVOID)pAdapter,
							"AntennaSwitchWorkitem"
	);

	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->PathDivSwitchWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_PathDivChkAntSwitchWorkitemCallback, 
		(PVOID)pAdapter,
		"SWAS_WorkItem");

	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->CCKPathDiversityWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_CCKTXPathDiversityWorkItemCallback, 
		(PVOID)pAdapter,
		"CCKTXPathDiversityWorkItem");
#if( defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY) ) ||( defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY) )
	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->FastAntTrainingWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_FastAntTrainingWorkItemCallback, 
		(PVOID)pAdapter,
		"FastAntTrainingWorkitem");
#endif
	ODM_InitializeWorkItem(
		pDM_Odm,
		&(pDM_Odm->DM_RXHP_Table.PSDTimeWorkitem), 
		(RT_WORKITEM_CALL_BACK)odm_PSD_RXHPWorkitemCallback, 
		(PVOID)pAdapter,
		"PSDRXHP_WorkItem");  
#endif
}

VOID
ODM_FreeAllWorkItems(IN PDM_ODM_T	pDM_Odm )
{
#if USE_WORKITEM
	ODM_FreeWorkItem(	&(pDM_Odm->DM_SWAT_Table.SwAntennaSwitchWorkitem));

	ODM_FreeWorkItem(&(pDM_Odm->PathDivSwitchWorkitem));      

	ODM_FreeWorkItem(&(pDM_Odm->CCKPathDiversityWorkitem));
	
	ODM_FreeWorkItem(&(pDM_Odm->FastAntTrainingWorkitem));

	ODM_FreeWorkItem((&pDM_Odm->DM_RXHP_Table.PSDTimeWorkitem));
#endif

}
#endif

/*
VOID
odm_FindMinimumRSSI(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	u4Byte	i;
	u1Byte	RSSI_Min = 0xFF;

	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
//		if(pDM_Odm->pODM_StaInfo[i] != NULL)
		if(IS_STA_VALID(pDM_Odm->pODM_StaInfo[i]) )
		{
			if(pDM_Odm->pODM_StaInfo[i]->RSSI_Ave < RSSI_Min)
			{
				RSSI_Min = pDM_Odm->pODM_StaInfo[i]->RSSI_Ave;
			}
		}
	}

	pDM_Odm->RSSI_Min = RSSI_Min;

}

VOID
odm_IsLinked(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	u4Byte i;
	BOOLEAN Linked = FALSE;
	
	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
			if(IS_STA_VALID(pDM_Odm->pODM_StaInfo[i]) )
			{			
				Linked = TRUE;
				break;
			}
		
	}

	pDM_Odm->bLinked = Linked;
}
*/

//3============================================================
//3 BB Power Save
//3============================================================
VOID 
odm_DynamicBBPowerSavingInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pPS_T	pDM_PSTable = &pDM_Odm->DM_PSTable;

	pDM_PSTable->PreCCAState = CCA_MAX;
	pDM_PSTable->CurCCAState = CCA_MAX;
	pDM_PSTable->PreRFState = RF_MAX;
	pDM_PSTable->CurRFState = RF_MAX;
	pDM_PSTable->Rssi_val_min = 0;
	pDM_PSTable->initialize = 0;
}


VOID
odm_DynamicBBPowerSaving(
	IN		PDM_ODM_T		pDM_Odm
	)
{	
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))

	if ((pDM_Odm->SupportICType != ODM_RTL8192C) && (pDM_Odm->SupportICType != ODM_RTL8723A))
		return;
	if(!(pDM_Odm->SupportAbility & ODM_BB_PWR_SAVE))
		return;
	if(!(pDM_Odm->SupportPlatform & (ODM_WIN|ODM_CE)))
		return;
	
	//1 2.Power Saving for 92C
	if((pDM_Odm->SupportICType == ODM_RTL8192C) &&(pDM_Odm->RFType == ODM_2T2R))
	{
		odm_1R_CCA(pDM_Odm);
	}
	
	// 20100628 Joseph: Turn off BB power save for 88CE because it makesthroughput unstable.
	// 20100831 Joseph: Turn ON BB power save again after modifying AGC delay from 900ns ot 600ns.
	//1 3.Power Saving for 88C
	else
	{
		ODM_RF_Saving(pDM_Odm, FALSE);
	}
#endif	// #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	
}

VOID
odm_1R_CCA(
	IN 	PDM_ODM_T	pDM_Odm
	)
{
	pPS_T	pDM_PSTable = &pDM_Odm->DM_PSTable;

	if(pDM_Odm->RSSI_Min!= 0xFF)
	{
		 
		if(pDM_PSTable->PreCCAState == CCA_2R)
		{
			if(pDM_Odm->RSSI_Min >= 35)
				pDM_PSTable->CurCCAState = CCA_1R;
			else
				pDM_PSTable->CurCCAState = CCA_2R;
			
		}
		else{
			if(pDM_Odm->RSSI_Min <= 30)
				pDM_PSTable->CurCCAState = CCA_2R;
			else
				pDM_PSTable->CurCCAState = CCA_1R;
		}
	}
	else{
		pDM_PSTable->CurCCAState=CCA_MAX;
	}
	
	if(pDM_PSTable->PreCCAState != pDM_PSTable->CurCCAState)
	{
		if(pDM_PSTable->CurCCAState == CCA_1R)
		{
			if(  pDM_Odm->RFType ==ODM_2T2R )
			{
				ODM_SetBBReg(pDM_Odm, 0xc04  , bMaskByte0, 0x13);
				//PHY_SetBBReg(pAdapter, 0xe70, bMaskByte3, 0x20);
			}
			else
			{
				ODM_SetBBReg(pDM_Odm, 0xc04  , bMaskByte0, 0x23);
				//PHY_SetBBReg(pAdapter, 0xe70, 0x7fc00000, 0x10c); // Set RegE70[30:22] = 9b'100001100
			}
		}
		else
		{
			ODM_SetBBReg(pDM_Odm, 0xc04  , bMaskByte0, 0x33);
			//PHY_SetBBReg(pAdapter,0xe70, bMaskByte3, 0x63);
		}
		pDM_PSTable->PreCCAState = pDM_PSTable->CurCCAState;
	}
	//ODM_RT_TRACE(pDM_Odm,	COMP_BB_POWERSAVING, DBG_LOUD, ("CCAStage = %s\n",(pDM_PSTable->CurCCAState==0)?"1RCCA":"2RCCA"));
}

void
ODM_RF_Saving(
	IN 	PDM_ODM_T	pDM_Odm,
	IN	u1Byte		bForceInNormal 
	)
{
#if (DM_ODM_SUPPORT_TYPE != ODM_AP)
	pPS_T	pDM_PSTable = &pDM_Odm->DM_PSTable;
	u1Byte	Rssi_Up_bound = 30 ;
	u1Byte	Rssi_Low_bound = 25;
	#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	if(pDM_Odm->PatchID == 40 ) //RT_CID_819x_FUNAI_TV
	{
		Rssi_Up_bound = 50 ;
		Rssi_Low_bound = 45;
	}
	#endif
	if(pDM_PSTable->initialize == 0){
		
		pDM_PSTable->Reg874 = (ODM_GetBBReg(pDM_Odm, 0x874, bMaskDWord)&0x1CC000)>>14;
		pDM_PSTable->RegC70 = (ODM_GetBBReg(pDM_Odm, 0xc70, bMaskDWord)&BIT3)>>3;
		pDM_PSTable->Reg85C = (ODM_GetBBReg(pDM_Odm, 0x85c, bMaskDWord)&0xFF000000)>>24;
		pDM_PSTable->RegA74 = (ODM_GetBBReg(pDM_Odm, 0xa74, bMaskDWord)&0xF000)>>12;
		//Reg818 = PHY_QueryBBReg(pAdapter, 0x818, bMaskDWord);
		pDM_PSTable->initialize = 1;
	}

	if(!bForceInNormal)
	{
		if(pDM_Odm->RSSI_Min != 0xFF)
		{			 
			if(pDM_PSTable->PreRFState == RF_Normal)
			{
				if(pDM_Odm->RSSI_Min >= Rssi_Up_bound)
					pDM_PSTable->CurRFState = RF_Save;
				else
					pDM_PSTable->CurRFState = RF_Normal;
			}
			else{
				if(pDM_Odm->RSSI_Min <= Rssi_Low_bound)
					pDM_PSTable->CurRFState = RF_Normal;
				else
					pDM_PSTable->CurRFState = RF_Save;
			}
		}
		else
			pDM_PSTable->CurRFState=RF_MAX;
	}
	else
	{
		pDM_PSTable->CurRFState = RF_Normal;
	}
	
	if(pDM_PSTable->PreRFState != pDM_PSTable->CurRFState)
	{
		if(pDM_PSTable->CurRFState == RF_Save)
		{
			// <tynli_note> 8723 RSSI report will be wrong. Set 0x874[5]=1 when enter BB power saving mode.
			// Suggested by SD3 Yu-Nan. 2011.01.20.
			if(pDM_Odm->SupportICType == ODM_RTL8723A)
			{
				ODM_SetBBReg(pDM_Odm, 0x874  , BIT5, 0x1); //Reg874[5]=1b'1
			}
			ODM_SetBBReg(pDM_Odm, 0x874  , 0x1C0000, 0x2); //Reg874[20:18]=3'b010
			ODM_SetBBReg(pDM_Odm, 0xc70, BIT3, 0); //RegC70[3]=1'b0
			ODM_SetBBReg(pDM_Odm, 0x85c, 0xFF000000, 0x63); //Reg85C[31:24]=0x63
			ODM_SetBBReg(pDM_Odm, 0x874, 0xC000, 0x2); //Reg874[15:14]=2'b10
			ODM_SetBBReg(pDM_Odm, 0xa74, 0xF000, 0x3); //RegA75[7:4]=0x3
			ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 0x0); //Reg818[28]=1'b0
			ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 0x1); //Reg818[28]=1'b1
			//ODM_RT_TRACE(pDM_Odm,	COMP_BB_POWERSAVING, DBG_LOUD, (" RF_Save"));
		}
		else
		{
			ODM_SetBBReg(pDM_Odm, 0x874  , 0x1CC000, pDM_PSTable->Reg874); 
			ODM_SetBBReg(pDM_Odm, 0xc70, BIT3, pDM_PSTable->RegC70); 
			ODM_SetBBReg(pDM_Odm, 0x85c, 0xFF000000, pDM_PSTable->Reg85C);
			ODM_SetBBReg(pDM_Odm, 0xa74, 0xF000, pDM_PSTable->RegA74); 
			ODM_SetBBReg(pDM_Odm,0x818, BIT28, 0x0);  

			if(pDM_Odm->SupportICType == ODM_RTL8723A)
			{
				ODM_SetBBReg(pDM_Odm,0x874  , BIT5, 0x0); //Reg874[5]=1b'0
			}
			//ODM_RT_TRACE(pDM_Odm,	COMP_BB_POWERSAVING, DBG_LOUD, (" RF_Normal"));
		}
		pDM_PSTable->PreRFState =pDM_PSTable->CurRFState;
	}
#endif	
}


//3============================================================
//3 RATR MASK
//3============================================================
//3============================================================
//3 Rate Adaptive
//3============================================================

VOID
odm_RateAdaptiveMaskInit(
	IN 	PDM_ODM_T	pDM_Odm
	)
{
	PODM_RATE_ADAPTIVE	pOdmRA = &pDM_Odm->RateAdaptive;

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PMGNT_INFO      			pMgntInfo = &pDM_Odm->Adapter->MgntInfo;
	PRATE_ADAPTIVE			pRA = (PRATE_ADAPTIVE)&pMgntInfo->RateAdaptive;

	pRA->RATRState = DM_RATR_STA_INIT;
	if (pMgntInfo->DM_Type == DM_Type_ByDriver)
		pMgntInfo->bUseRAMask = TRUE;
	else
		pMgntInfo->bUseRAMask = FALSE;	

#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	pOdmRA->Type = DM_Type_ByDriver;
	if (pOdmRA->Type == DM_Type_ByDriver)
		pDM_Odm->bUseRAMask = _TRUE;
	else
		pDM_Odm->bUseRAMask = _FALSE;	
	
#endif

	pOdmRA->RATRState = DM_RATR_STA_INIT;
	pOdmRA->HighRSSIThresh = 50;
	pOdmRA->LowRSSIThresh = 20;
	pOdmRA->UltraLowRSSIThresh = 9;
}

#if (DM_ODM_SUPPORT_TYPE & ODM_WIN) 
VOID
ODM_RateAdaptiveStateApInit(
	IN	PADAPTER	Adapter	,
	IN	PRT_WLAN_STA  pEntry
	)
{
	PRATE_ADAPTIVE	pRA = (PRATE_ADAPTIVE)&pEntry->RateAdaptive;
	pRA->RATRState = DM_RATR_STA_INIT;
}
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
u4Byte ODM_Get_Rate_Bitmap(
	IN	PDM_ODM_T	pDM_Odm,	
	IN	u4Byte 		ra_mask,	
	IN	u1Byte 		rssi_level)
{
	u4Byte 	rate_bitmap = 0x0fffffff;
	u1Byte 	WirelessMode =*(pDM_Odm->pWirelessMode);
	

	switch(WirelessMode)
	{
		case ODM_WM_B:
			if(ra_mask & 0x0000000c)		//11M or 5.5M enable				
				rate_bitmap = 0x0000000d;
			else
				rate_bitmap = 0x0000000f;
			break;
			
		case (ODM_WM_A|ODM_WM_G):
			if(rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x00000f00;
			else
				rate_bitmap = 0x00000ff0;
			break;
			
		case (ODM_WM_B|ODM_WM_G):
			if(rssi_level == DM_RATR_STA_HIGH)
				rate_bitmap = 0x00000f00;
			else if(rssi_level == DM_RATR_STA_MIDDLE)
				rate_bitmap = 0x00000ff0;
			else
				rate_bitmap = 0x00000ff5;
			break;		

		case (ODM_WM_B|ODM_WM_G|ODM_WM_N24G)	:
		case (ODM_WM_A|ODM_WM_B|ODM_WM_G|ODM_WM_N24G)	:
			{					
				if (	pDM_Odm->RFType == ODM_1T2R ||pDM_Odm->RFType == ODM_1T1R)
				{
					if(rssi_level == DM_RATR_STA_HIGH)
					{
						rate_bitmap = 0x000f0000;
					}
					else if(rssi_level == DM_RATR_STA_MIDDLE)
					{
						rate_bitmap = 0x000ff000;
					}
					else{
						if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
							rate_bitmap = 0x000ff015;
						else
							rate_bitmap = 0x000ff005;
					}				
				}
				else
				{
					if(rssi_level == DM_RATR_STA_HIGH)
					{		
						rate_bitmap = 0x0f8f0000;
					}
					else if(rssi_level == DM_RATR_STA_MIDDLE)
					{
						rate_bitmap = 0x0f8ff000;
					}
					else
					{
						if (*(pDM_Odm->pBandWidth) == ODM_BW40M)
							rate_bitmap = 0x0f8ff015;
						else
							rate_bitmap = 0x0f8ff005;
					}					
				}
			}
			break;
		default:
		//case WIRELESS_11_24N:
		//case WIRELESS_11_5N:			
			if(pDM_Odm->RFType == RF_1T2R)
				rate_bitmap = 0x000fffff;
			else
				rate_bitmap = 0x0fffffff;
			break;	

	}

	//printk("%s ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x \n",__FUNCTION__,rssi_level,WirelessMode,rate_bitmap);
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, (" ==> rssi_level:0x%02x, WirelessMode:0x%02x, rate_bitmap:0x%08x \n",rssi_level,WirelessMode,rate_bitmap));

	return rate_bitmap;
	
}	
#endif

/*-----------------------------------------------------------------------------
 * Function:	odm_RefreshRateAdaptiveMask()
 *
 * Overview:	Update rate table mask according to rssi
 *
 * Input:		NONE
 *
 * Output:		NONE
 *
 * Return:		NONE
 *
 * Revised History:
 *	When		Who		Remark
 *	05/27/2009	hpfan	Create Version 0.  
 *
 *---------------------------------------------------------------------------*/
VOID
odm_RefreshRateAdaptiveMask(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	if (!(pDM_Odm->SupportAbility & ODM_BB_RA_MASK))
		return;	
	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:
			odm_RefreshRateAdaptiveMaskMP(pDM_Odm);
			break;

		case	ODM_CE:
			odm_RefreshRateAdaptiveMaskCE(pDM_Odm);
			break;

		case	ODM_AP:
		case	ODM_ADSL:
			odm_RefreshRateAdaptiveMaskAPADSL(pDM_Odm);
			break;
	}
	
}

VOID
odm_RefreshRateAdaptiveMaskMP(
	IN		PDM_ODM_T		pDM_Odm	
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER	pAdapter	 =  pDM_Odm->Adapter;
	PADAPTER 				pTargetAdapter = NULL;
	HAL_DATA_TYPE			*pHalData = GET_HAL_DATA(pAdapter);
	PMGNT_INFO				pMgntInfo = GetDefaultMgntInfo(pAdapter);
	//PRATE_ADAPTIVE			pRA = (PRATE_ADAPTIVE)&pMgntInfo->RateAdaptive;
	PODM_RATE_ADAPTIVE		pRA = &pDM_Odm->RateAdaptive;

	if(pAdapter->bDriverStopped)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("<---- odm_RefreshRateAdaptiveMask(): driver is going to unload\n"));
		return;
	}

	if(!pMgntInfo->bUseRAMask)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("<---- odm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n"));
		return;
	}

	// if default port is connected, update RA table for default port (infrastructure mode only)
	if(pAdapter->MgntInfo.mAssoc && (!ACTING_AS_AP(pAdapter)))
	{
		if( ODM_RAStateCheck(pDM_Odm, pHalData->UndecoratedSmoothedPWDB, pMgntInfo->bSetTXPowerTrainingByOid, &pRA->RATRState) )
		{
			ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target AP addr : "), pMgntInfo->Bssid);
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pHalData->UndecoratedSmoothedPWDB, pRA->RATRState));
			pAdapter->HalFunc.UpdateHalRAMaskHandler(
									pAdapter,
									FALSE,
									0,
									NULL,
									NULL,
									pRA->RATRState,
									RAMask_Normal);
		}
	}

	//
	// The following part configure AP/VWifi/IBSS rate adaptive mask.
	//

	if(pMgntInfo->mIbss)
	{
		// Target: AP/IBSS peer.
		pTargetAdapter = GetDefaultAdapter(pAdapter);
	}
	else
	{
		pTargetAdapter = GetFirstAPAdapter(pAdapter);
	}

	// if extension port (softap) is started, updaet RA table for more than one clients associate
	if(pTargetAdapter != NULL)
	{
		int	i;
		PRT_WLAN_STA	pEntry;
		PRATE_ADAPTIVE     pEntryRA;

		for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		{
#if 0	//By YJ,120208		
			if(	pTargetAdapter->MgntInfo.AsocEntry[i].bUsed && pTargetAdapter->MgntInfo.AsocEntry[i].bAssociated)
			{
				pEntry = pTargetAdapter->MgntInfo.AsocEntry+i;
				pEntryRA = &pEntry->RateAdaptive;
				if( ODM_RAStateCheck(pDM_Odm, pEntry->rssi_stat.UndecoratedSmoothedPWDB, pMgntInfo->bSetTXPowerTrainingByOid, &pEntryRA->RATRState) )
				{
					ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target STA addr : "), pEntry->MacAddr);
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pEntry->rssi_stat.UndecoratedSmoothedPWDB, pEntryRA->RATRState));
					pAdapter->HalFunc.UpdateHalRAMaskHandler(
											pTargetAdapter,
											FALSE,
											pEntry->AID+1,
											pEntry->MacAddr,
											pEntry,
											pEntryRA->RATRState,
											RAMask_Normal);
				}
			}
#else
			pEntry = AsocEntry_EnumStation(pTargetAdapter, i);
			if(NULL != pEntry)
			{
				if(pEntry->bAssociated)
				{
					pEntryRA = &pEntry->RateAdaptive;
					if( ODM_RAStateCheck(pDM_Odm, pEntry->rssi_stat.UndecoratedSmoothedPWDB, pMgntInfo->bSetTXPowerTrainingByOid, &pEntryRA->RATRState) )
					{
						ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target STA addr : "), pEntry->MacAddr);
						ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pEntry->rssi_stat.UndecoratedSmoothedPWDB, pEntryRA->RATRState));
						pAdapter->HalFunc.UpdateHalRAMaskHandler(
											pTargetAdapter,
											FALSE,
											pEntry->AID+1,
											pEntry->MacAddr,
											pEntry,
											pEntryRA->RATRState,
											RAMask_Normal);
					}
				}
			}
#endif		
		}
	}

	if(pMgntInfo->bSetTXPowerTrainingByOid)
		pMgntInfo->bSetTXPowerTrainingByOid = FALSE;	
#endif	// #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
}


VOID
odm_RefreshRateAdaptiveMaskCE(
	IN		PDM_ODM_T		pDM_Odm	
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	u4Byte	i;
	PADAPTER	pAdapter	 =  pDM_Odm->Adapter;

	if(pAdapter->bDriverStopped)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_TRACE, ("<---- odm_RefreshRateAdaptiveMask(): driver is going to unload\n"));
		return;
	}

	if(!pDM_Odm->bUseRAMask)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("<---- odm_RefreshRateAdaptiveMask(): driver does not control rate adaptive mask\n"));
		return;
	}

	//printk("==> %s \n",__FUNCTION__);

	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++){
		PSTA_INFO_T pstat = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pstat) ) {
			if( TRUE == ODM_RAStateCheck(pDM_Odm, pstat->rssi_stat.UndecoratedSmoothedPWDB, FALSE , &pstat->rssi_level) )
			{
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level));
				//printk("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi_stat.UndecoratedSmoothedPWDB, pstat->rssi_level);
				pAdapter->HalFunc.UpdateRAMaskHandler(pAdapter,i,pstat->rssi_level);
			}
		
		}
	}			
	
#endif
}

VOID
odm_RefreshRateAdaptiveMaskAPADSL(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	struct rtl8192cd_priv *priv = pDM_Odm->priv;
	struct aid_obj *aidarray;
	u4Byte i;
	PSTA_INFO_T pstat;

	if(priv->up_time % 2)
		return;	

	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++) {
		pstat = pDM_Odm->pODM_StaInfo[i];

		if(IS_STA_VALID(pstat) )
		{			
#if defined(UNIVERSAL_REPEATER) || defined(MBSSID)
			aidarray = container_of(pstat, struct aid_obj, station);
			priv = aidarray->priv;
#endif

			if (!priv->pmib->dot11StationConfigEntry.autoRate) 
				continue;

			if(ODM_RAStateCheck(pDM_Odm, (s4Byte)pstat->rssi, FALSE, &pstat->rssi_level) ) {
				ODM_PRINT_ADDR(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("Target STA addr : "), pstat->hwaddr);
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI:%d, RSSI_LEVEL:%d\n", pstat->rssi, pstat->rssi_level));

#if defined(CONFIG_PCI_HCI)
#ifdef CONFIG_WLAN_HAL
				if (IS_HAL_CHIP(priv)) {
#ifdef WDS
//					if(!(pstat->state & WIFI_WDS))//if WDS donot setting
#endif
						GET_HAL_INTERFACE(priv)->UpdateHalRAMaskHandler(priv, pstat, pstat->rssi_level);
				} else
#endif
#ifdef CONFIG_RTL_8812_SUPPORT
				if(GET_CHIP_VER(priv)== VERSION_8812E) {
					UpdateHalRAMask8812(priv, pstat, 3);
				} else
#endif
#ifdef CONFIG_RTL_88E_SUPPORT
				if (GET_CHIP_VER(priv)==VERSION_8188E) {
#ifdef TXREPORT
					add_RATid(priv, pstat);
#endif
				} else
#endif
				{
#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_92C_SUPPORT)
					add_update_RATid(priv, pstat);
#endif
				}
#elif defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
				update_STA_RATid(priv, pstat);
#endif
			}
		}
	}
#endif
}

// Return Value: BOOLEAN
// - TRUE: RATRState is changed.
BOOLEAN 
ODM_RAStateCheck(
	IN		PDM_ODM_T		pDM_Odm,
	IN		s4Byte			RSSI,
	IN		BOOLEAN			bForceUpdate,
	OUT		pu1Byte			pRATRState
	)
{
	PODM_RATE_ADAPTIVE pRA = &pDM_Odm->RateAdaptive;
	const u1Byte GoUpGap = 5;
	u1Byte HighRSSIThreshForRA = pRA->HighRSSIThresh;
	u1Byte LowRSSIThreshForRA = pRA->LowRSSIThresh;
	u1Byte UltraLowRSSIThreshForRA = pRA->UltraLowRSSIThresh;
	u1Byte RATRState;

	// Threshold Adjustment: 
	// when RSSI state trends to go up one or two levels, make sure RSSI is high enough.
	// Here GoUpGap is added to solve the boundary's level alternation issue.

#if (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	if(pDM_Odm->SupportICType == ODM_RTL8881A)		
		LowRSSIThreshForRA = 30;		// for LDPC / BCC switch
#endif

	switch (*pRATRState)
	{
		case DM_RATR_STA_INIT:
		case DM_RATR_STA_HIGH:
			break;

		case DM_RATR_STA_MIDDLE:
			HighRSSIThreshForRA += GoUpGap;
			break;

		case DM_RATR_STA_LOW:
			HighRSSIThreshForRA += GoUpGap;
			LowRSSIThreshForRA += GoUpGap;
			break;

		case DM_RATR_STA_ULTRA_LOW:
			HighRSSIThreshForRA += GoUpGap;
			LowRSSIThreshForRA += GoUpGap;
			UltraLowRSSIThreshForRA += GoUpGap;
			break;
			
		default: 
			ODM_RT_ASSERT(pDM_Odm, FALSE, ("wrong rssi level setting %d !", *pRATRState) );
			break;
	}

	// Decide RATRState by RSSI.
	if(RSSI > HighRSSIThreshForRA)
		RATRState = DM_RATR_STA_HIGH;
	else if(RSSI > LowRSSIThreshForRA)
		RATRState = DM_RATR_STA_MIDDLE;
	else if(RSSI > UltraLowRSSIThreshForRA)
		RATRState = DM_RATR_STA_LOW;
	else
		RATRState = DM_RATR_STA_ULTRA_LOW;
	//printk("==>%s,RATRState:0x%02x ,RSSI:%d \n",__FUNCTION__,RATRState,RSSI);

	if( *pRATRState!=RATRState || bForceUpdate)
	{
		ODM_RT_TRACE( pDM_Odm, ODM_COMP_RA_MASK, ODM_DBG_LOUD, ("RSSI Level %d -> %d\n", *pRATRState, RATRState) );
		*pRATRState = RATRState;
		return TRUE;
	}

	return FALSE;
}


//============================================================

//3============================================================
//3 Dynamic Tx Power
//3============================================================

VOID 
odm_DynamicTxPowerInit(
	IN		PDM_ODM_T		pDM_Odm	
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

	#if DEV_BUS_TYPE==RT_USB_INTERFACE					
	if(RT_GetInterfaceSelection(Adapter) == INTF_SEL1_USB_High_Power)
	{
		odm_DynamicTxPowerSavePowerIndex(pDM_Odm);
		pMgntInfo->bDynamicTxPowerEnable = TRUE;
	}		
	else	
	#else
	//so 92c pci do not need dynamic tx power? vivi check it later
	if(IS_HARDWARE_TYPE_8192D(Adapter))
		pMgntInfo->bDynamicTxPowerEnable = TRUE;
	else
		pMgntInfo->bDynamicTxPowerEnable = FALSE;
	#endif
	

	pHalData->LastDTPLvl = TxHighPwrLevel_Normal;
	pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	pdmpriv->bDynamicTxPowerEnable = _FALSE;

	#if (RTL8192C_SUPPORT==1) 
	#ifdef CONFIG_USB_HCI

	#ifdef CONFIG_INTEL_PROXIM
	if((pHalData->BoardType == BOARD_USB_High_PA)||(Adapter->proximity.proxim_support==_TRUE))
	#else
	if(pHalData->BoardType == BOARD_USB_High_PA)
	#endif

	{
		//odm_SavePowerIndex(Adapter);
		odm_DynamicTxPowerSavePowerIndex(pDM_Odm);
		pdmpriv->bDynamicTxPowerEnable = _TRUE;
	}		
	else	
	#else
		pdmpriv->bDynamicTxPowerEnable = _FALSE;
	#endif
	#endif
	
	pdmpriv->LastDTPLvl = TxHighPwrLevel_Normal;
	pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;	
	
#endif
	
}

VOID
odm_DynamicTxPowerSavePowerIndex(
	IN		PDM_ODM_T		pDM_Odm	
	)
{	
#if (DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_WIN))
	u1Byte		index;
	u4Byte		Power_Index_REG[6] = {0xc90, 0xc91, 0xc92, 0xc98, 0xc99, 0xc9a};
	
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)	
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);	
	for(index = 0; index< 6; index++)
		pHalData->PowerIndex_backup[index] = PlatformEFIORead1Byte(Adapter, Power_Index_REG[index]);
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)	
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	for(index = 0; index< 6; index++)
		pdmpriv->PowerIndex_backup[index] = rtw_read8(Adapter, Power_Index_REG[index]);
#endif
#endif
}

VOID
odm_DynamicTxPowerRestorePowerIndex(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_WIN))
	u1Byte			index;
	PADAPTER		Adapter = pDM_Odm->Adapter;

	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u4Byte			Power_Index_REG[6] = {0xc90, 0xc91, 0xc92, 0xc98, 0xc99, 0xc9a};
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	for(index = 0; index< 6; index++)
		PlatformEFIOWrite1Byte(Adapter, Power_Index_REG[index], pHalData->PowerIndex_backup[index]);
#elif(DM_ODM_SUPPORT_TYPE == ODM_CE)	
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	for(index = 0; index< 6; index++)
		rtw_write8(Adapter, Power_Index_REG[index], pdmpriv->PowerIndex_backup[index]);
#endif
#endif
}

VOID
odm_DynamicTxPowerWritePowerIndex(
	IN	PDM_ODM_T	pDM_Odm, 
	IN 	u1Byte		Value)
{

	u1Byte			index;
	u4Byte			Power_Index_REG[6] = {0xc90, 0xc91, 0xc92, 0xc98, 0xc99, 0xc9a};
	
	for(index = 0; index< 6; index++)
		//PlatformEFIOWrite1Byte(Adapter, Power_Index_REG[index], Value);
		ODM_Write1Byte(pDM_Odm, Power_Index_REG[index], Value);

}


VOID 
odm_DynamicTxPower(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	// 
	// For AP/ADSL use prtl8192cd_priv
	// For CE/NIC use PADAPTER
	//
	//PADAPTER		pAdapter = pDM_Odm->Adapter;
//	prtl8192cd_priv	priv		= pDM_Odm->priv;

	if (!(pDM_Odm->SupportAbility & ODM_BB_DYNAMIC_TXPWR))
		return;

	// 2012/01/12 MH According to Luke's suggestion, only high power will support the feature.
#if (DM_ODM_SUPPORT_TYPE & (ODM_CE|ODM_WIN))
	if (pDM_Odm->ExtPA == FALSE)
		return;
#endif		

	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:
		case	ODM_CE:
			odm_DynamicTxPowerNIC(pDM_Odm);
			break;	
		case	ODM_AP:
			odm_DynamicTxPowerAP(pDM_Odm);
			break;		

		case	ODM_ADSL:
			//odm_DIGAP(pDM_Odm);
			break;	
	}

	
}


VOID 
odm_DynamicTxPowerNIC(
	IN		PDM_ODM_T		pDM_Odm
	)
{	
	if (!(pDM_Odm->SupportAbility & ODM_BB_DYNAMIC_TXPWR))
		return;
	
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))

	if(pDM_Odm->SupportICType == ODM_RTL8192C)	
	{
		odm_DynamicTxPower_92C(pDM_Odm);
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		odm_DynamicTxPower_92D(pDM_Odm);
	}
	else if (pDM_Odm->SupportICType & ODM_RTL8188E)
	{
		// Add Later.
	}
	else if (pDM_Odm->SupportICType == ODM_RTL8188E)
	{
		// ???
		// This part need to be redefined.
	}
#endif	
}

VOID 
odm_DynamicTxPowerAP(
	IN		PDM_ODM_T		pDM_Odm

	)
{	
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)

//#if ((RTL8192C_SUPPORT==1) || (RTL8192D_SUPPORT==1) || (RTL8188E_SUPPORT==1) || (RTL8812E_SUPPORT==1))


	prtl8192cd_priv	priv		= pDM_Odm->priv;
	s4Byte i;
	s2Byte pwr_thd = TX_POWER_NEAR_FIELD_THRESH_AP;

	if(!priv->pshare->rf_ft_var.tx_pwr_ctrl)
		return;

#if ((RTL8812E_SUPPORT==1) || (RTL8881A_SUPPORT==1))
	if (pDM_Odm->SupportICType & (ODM_RTL8812 | ODM_RTL8881A))
		pwr_thd = TX_POWER_NEAR_FIELD_THRESH_8812;
#endif

#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_92C_SUPPORT)
	if(CHIP_VER_92X_SERIES(priv))
	{
#ifdef HIGH_POWER_EXT_PA
	if(pDM_Odm->ExtPA)
		tx_power_control(priv);
#endif
	}
#endif	
	/*
	 *	Check if station is near by to use lower tx power
	 */

	if ((priv->up_time % 3) == 0 )  {
		int disable_pwr_ctrl = ((pDM_Odm->FalseAlmCnt.Cnt_all > 1000 ) || ((pDM_Odm->FalseAlmCnt.Cnt_all > 300 ) && ((RTL_R8(0xc50) & 0x7f) >= 0x32))) ? 1 : 0;
			
		for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++){
			PSTA_INFO_T pstat = pDM_Odm->pODM_StaInfo[i];
				if(IS_STA_VALID(pstat)) {
					if(disable_pwr_ctrl)
						pstat->hp_level = 0;
					 else if ((pstat->hp_level == 0) && (pstat->rssi > pwr_thd))
							pstat->hp_level = 1;
						else if ((pstat->hp_level == 1) && (pstat->rssi < (pwr_thd-8)))
							pstat->hp_level = 0;
				}
			}

#if defined(CONFIG_WLAN_HAL_8192EE)
		if (GET_CHIP_VER(priv) == VERSION_8192E) {
			if( !disable_pwr_ctrl && (pDM_Odm->RSSI_Min != 0xff) ) {
				if(pDM_Odm->RSSI_Min > pwr_thd)
					RRSR_power_control_11n(priv,  1 );
				else if(pDM_Odm->RSSI_Min < (pwr_thd-8))
					RRSR_power_control_11n(priv,  0 );
			} else {
					RRSR_power_control_11n(priv,  0 );
			}
		}
#endif			
	}
//#endif	

#endif	
}


VOID 
odm_DynamicTxPower_92C(
	IN	PDM_ODM_T	pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER Adapter = pDM_Odm->Adapter;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	s4Byte				UndecoratedSmoothedPWDB;


	// STA not connected and AP not connected
	if((!pMgntInfo->bMediaConnect) &&	
		(pHalData->EntryMinUndecoratedSmoothedPWDB == 0))
	{
		ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("Not connected to any \n"));
		pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;

		//the LastDTPlvl should reset when disconnect, 
		//otherwise the tx power level wouldn't change when disconnect and connect again.
		// Maddest 20091220.
		 pHalData->LastDTPLvl=TxHighPwrLevel_Normal;
		return;
	}

#if (INTEL_PROXIMITY_SUPPORT == 1)
	// Intel set fixed tx power 
	if(pMgntInfo->IntelProximityModeInfo.PowerOutput > 0)
	{
		switch(pMgntInfo->IntelProximityModeInfo.PowerOutput){
			case 1:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_100;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_100\n"));
				break;
			case 2:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_70;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_70\n"));
				break;
			case 3:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_50;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_50\n"));
				break;
			case 4:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_35;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_35\n"));
				break;
			case 5:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_15;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_15\n"));
				break;
			default:
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_100;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_100\n"));
				break;
		}		
	}
	else
#endif		
	{ 
		if(	(pMgntInfo->bDynamicTxPowerEnable != TRUE) ||
			(pHalData->DMFlag & HAL_DM_HIPWR_DISABLE) ||
			pMgntInfo->IOTAction & HT_IOT_ACT_DISABLE_HIGH_POWER)
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		}
		else
		{
			if(pMgntInfo->bMediaConnect)	// Default port
			{
				if(ACTING_AS_AP(Adapter) || ACTING_AS_IBSS(Adapter))
				{
					UndecoratedSmoothedPWDB = pHalData->EntryMinUndecoratedSmoothedPWDB;
					ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Client PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
				}
				else
				{
					UndecoratedSmoothedPWDB = pHalData->UndecoratedSmoothedPWDB;
					ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("STA Default Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
				}
			}
			else // associated entry pwdb
			{	
				UndecoratedSmoothedPWDB = pHalData->EntryMinUndecoratedSmoothedPWDB;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Ext Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
			}
				
			if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)
			{
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x0)\n"));
			}
			else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
				(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) )
			{
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
			}
			else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-5))
			{
				pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
				ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Normal\n"));
			}
		}
	}
	if( pHalData->DynamicTxHighPowerLvl != pHalData->LastDTPLvl )
	{
		ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("PHY_SetTxPowerLevel8192C() Channel = %d \n" , pHalData->CurrentChannel));
		PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
		if(	(pHalData->DynamicTxHighPowerLvl == TxHighPwrLevel_Normal) &&
			(pHalData->LastDTPLvl == TxHighPwrLevel_Level1 || pHalData->LastDTPLvl == TxHighPwrLevel_Level2)) //TxHighPwrLevel_Normal
			odm_DynamicTxPowerRestorePowerIndex(pDM_Odm);
		else if(pHalData->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
			odm_DynamicTxPowerWritePowerIndex(pDM_Odm, 0x14);
		else if(pHalData->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
			odm_DynamicTxPowerWritePowerIndex(pDM_Odm, 0x10);
	}
	pHalData->LastDTPLvl = pHalData->DynamicTxHighPowerLvl;

	
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)

	#if (RTL8192C_SUPPORT==1) 
	PADAPTER Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	struct mlme_priv	*pmlmepriv = &(Adapter->mlmepriv);
	struct mlme_ext_priv	*pmlmeext = &Adapter->mlmeextpriv;
	int	UndecoratedSmoothedPWDB;

	if(!pdmpriv->bDynamicTxPowerEnable)
		return;

#ifdef CONFIG_INTEL_PROXIM
	if(Adapter->proximity.proxim_on== _TRUE){
		struct proximity_priv *prox_priv=Adapter->proximity.proximity_priv;
		// Intel set fixed tx power 
		printk("\n %s  Adapter->proximity.proxim_on=%d prox_priv->proxim_modeinfo->power_output=%d \n",__FUNCTION__,Adapter->proximity.proxim_on,prox_priv->proxim_modeinfo->power_output);
		if(prox_priv!=NULL){
			if(prox_priv->proxim_modeinfo->power_output> 0)	
			{
				switch(prox_priv->proxim_modeinfo->power_output)
				{
					case 1:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_100;
						printk("TxHighPwrLevel_100\n");
						break;
					case 2:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_70;
						printk("TxHighPwrLevel_70\n");
						break;
					case 3:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_50;
						printk("TxHighPwrLevel_50\n");
						break;
					case 4:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_35;
						printk("TxHighPwrLevel_35\n");
						break;
					case 5:
						pdmpriv->DynamicTxHighPowerLvl  = TxHighPwrLevel_15;
						printk("TxHighPwrLevel_15\n");
						break;
					default:
						pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_100;
						printk("TxHighPwrLevel_100\n");
						break;
				}		
			}
		}
	}
	else
#endif	
	{
		// STA not connected and AP not connected
		if((check_fwstate(pmlmepriv, _FW_LINKED) != _TRUE) &&	
			(pdmpriv->EntryMinUndecoratedSmoothedPWDB == 0))
		{
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("Not connected to any \n"));
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;

			//the LastDTPlvl should reset when disconnect, 
			//otherwise the tx power level wouldn't change when disconnect and connect again.
			// Maddest 20091220.
			pdmpriv->LastDTPLvl=TxHighPwrLevel_Normal;
			return;
		}
		
		if(check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE)	// Default port
		{
		#if 0
			//todo: AP Mode
			if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE) ||
			       (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE))
			{
				UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
				//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Client PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
			}
			else
			{
				UndecoratedSmoothedPWDB = pdmpriv->UndecoratedSmoothedPWDB;
				//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("STA Default Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
			}
		#else
		UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;	
		#endif
		}
		else // associated entry pwdb
		{	
			UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Ext Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
		}
			
		if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x0)\n"));
		}
		else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
			(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) )
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
		}
		else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-5))
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Normal\n"));
		}
	}
	if( (pdmpriv->DynamicTxHighPowerLvl != pdmpriv->LastDTPLvl) )
	{
		PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
		if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Normal) // HP1 -> Normal  or HP2 -> Normal
			odm_DynamicTxPowerRestorePowerIndex(pDM_Odm);
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level1)
			odm_DynamicTxPowerWritePowerIndex(pDM_Odm, 0x14);
		else if(pdmpriv->DynamicTxHighPowerLvl == TxHighPwrLevel_Level2)
			odm_DynamicTxPowerWritePowerIndex(pDM_Odm, 0x10);
	}
	pdmpriv->LastDTPLvl = pdmpriv->DynamicTxHighPowerLvl;
	#endif
#endif	// #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

}


VOID 
odm_DynamicTxPower_92D(
	IN	PDM_ODM_T	pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER Adapter = pDM_Odm->Adapter;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	s4Byte				UndecoratedSmoothedPWDB;

	PADAPTER	BuddyAdapter = Adapter->BuddyAdapter;
	BOOLEAN		bGetValueFromBuddyAdapter = dm_DualMacGetParameterFromBuddyAdapter(Adapter);
	u1Byte		HighPowerLvlBackForMac0 = TxHighPwrLevel_Level1;


	// If dynamic high power is disabled.
	if( (pMgntInfo->bDynamicTxPowerEnable != TRUE) ||
		(pHalData->DMFlag & HAL_DM_HIPWR_DISABLE) ||
		pMgntInfo->IOTAction & HT_IOT_ACT_DISABLE_HIGH_POWER)
	{
		pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		return;
	}

	// STA not connected and AP not connected
	if((!pMgntInfo->bMediaConnect) &&	
		(pHalData->EntryMinUndecoratedSmoothedPWDB == 0))
	{
		ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("Not connected to any \n"));
		pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;

		//the LastDTPlvl should reset when disconnect, 
		//otherwise the tx power level wouldn't change when disconnect and connect again.
		// Maddest 20091220.
		 pHalData->LastDTPLvl=TxHighPwrLevel_Normal;
		return;
	}
	
	if(pMgntInfo->bMediaConnect)	// Default port
	{
		if(ACTING_AS_AP(Adapter) || pMgntInfo->mIbss)
		{
			UndecoratedSmoothedPWDB = pHalData->EntryMinUndecoratedSmoothedPWDB;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Client PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
		}
		else
		{
			UndecoratedSmoothedPWDB = pHalData->UndecoratedSmoothedPWDB;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("STA Default Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
		}
	}
	else // associated entry pwdb
	{	
		UndecoratedSmoothedPWDB = pHalData->EntryMinUndecoratedSmoothedPWDB;
		ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Ext Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
	}
	
	if(IS_HARDWARE_TYPE_8192D(Adapter) && GET_HAL_DATA(Adapter)->CurrentBandType == 1){
		if(UndecoratedSmoothedPWDB >= 0x33)
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Level2 (TxPwr=0x0)\n"));
		}
		else if((UndecoratedSmoothedPWDB <0x33) &&
			(UndecoratedSmoothedPWDB >= 0x2b) )
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
		}
		else if(UndecoratedSmoothedPWDB < 0x2b)
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Normal\n"));
		}

	}
	else
	
	{
		if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x0)\n"));
		}
		else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
			(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) )
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
		}
		else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-5))
		{
			pHalData->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Normal\n"));
		}

	}

//sherry  delete flag 20110517
	if(bGetValueFromBuddyAdapter)
	{
		ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() mac 0 for mac 1 \n"));
		if(Adapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP)
		{
			ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() change value \n"));
			HighPowerLvlBackForMac0 = pHalData->DynamicTxHighPowerLvl;
			pHalData->DynamicTxHighPowerLvl = Adapter->DualMacDMSPControl.CurTxHighLvlForAnotherMacOfDMSP;
			PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
			pHalData->DynamicTxHighPowerLvl = HighPowerLvlBackForMac0;
			Adapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP = FALSE;
		}						
	}

	if( (pHalData->DynamicTxHighPowerLvl != pHalData->LastDTPLvl) )
	{
			ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("PHY_SetTxPowerLevel8192S() Channel = %d \n" , pHalData->CurrentChannel));
			if(Adapter->DualMacSmartConcurrent == TRUE)
			{
				if(BuddyAdapter == NULL)
				{
					ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter == NULL case \n"));
					if(!Adapter->bSlaveOfDMSP)
					{
						PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
					}
				}
				else
				{
					if(pHalData->MacPhyMode92D == DUALMAC_SINGLEPHY)
					{
						ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter DMSP \n"));
						if(Adapter->bSlaveOfDMSP)
						{
							ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() bslave case  \n"));
							BuddyAdapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP = TRUE;
							BuddyAdapter->DualMacDMSPControl.CurTxHighLvlForAnotherMacOfDMSP = pHalData->DynamicTxHighPowerLvl;
						}
						else
						{
							ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() master case  \n"));					
							if(!bGetValueFromBuddyAdapter)
							{
								ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() mac 0 for mac 0 \n"));
								PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
							}
						}
					}
					else
					{
						ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter DMDP\n"));
						PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
					}
				}
			}
			else
			{
				PHY_SetTxPowerLevel8192C(Adapter, pHalData->CurrentChannel);
			}

		}
	pHalData->LastDTPLvl = pHalData->DynamicTxHighPowerLvl;
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
#if (RTL8192D_SUPPORT==1) 
	PADAPTER Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct mlme_priv	*pmlmepriv = &(Adapter->mlmepriv);

	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	DM_ODM_T 		*podmpriv = &pHalData->odmpriv;
	int	UndecoratedSmoothedPWDB;
	#if (RTL8192D_EASY_SMART_CONCURRENT == 1)
	PADAPTER	BuddyAdapter = Adapter->BuddyAdapter;
	BOOLEAN		bGetValueFromBuddyAdapter = DualMacGetParameterFromBuddyAdapter(Adapter);
	u8		HighPowerLvlBackForMac0 = TxHighPwrLevel_Level1;
	#endif

	// If dynamic high power is disabled.
	if( (pdmpriv->bDynamicTxPowerEnable != _TRUE) ||
		(!(podmpriv->SupportAbility& ODM_BB_DYNAMIC_TXPWR)) )
	{
		pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		return;
	}

	// STA not connected and AP not connected
	if((check_fwstate(pmlmepriv, _FW_LINKED) != _TRUE) &&	
		(pdmpriv->EntryMinUndecoratedSmoothedPWDB == 0))
	{
		//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("Not connected to any \n"));
		pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
		//the LastDTPlvl should reset when disconnect, 
		//otherwise the tx power level wouldn't change when disconnect and connect again.
		// Maddest 20091220.
		pdmpriv->LastDTPLvl=TxHighPwrLevel_Normal;
		return;
	}
		
	if(check_fwstate(pmlmepriv, _FW_LINKED) == _TRUE)	// Default port
	{
	#if 0
		//todo: AP Mode
		if ((check_fwstate(pmlmepriv, WIFI_ADHOC_MASTER_STATE) == _TRUE) ||
	       (check_fwstate(pmlmepriv, WIFI_ADHOC_STATE) == _TRUE))
		{
			UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Client PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
		}
		else
		{
			UndecoratedSmoothedPWDB = pdmpriv->UndecoratedSmoothedPWDB;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("STA Default Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
		}
	#else
	UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
	#endif
	}
	else // associated entry pwdb
	{	
		UndecoratedSmoothedPWDB = pdmpriv->EntryMinUndecoratedSmoothedPWDB;
		//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("AP Ext Port PWDB = 0x%x \n", UndecoratedSmoothedPWDB));
	}
#if TX_POWER_FOR_5G_BAND == 1
	if(pHalData->CurrentBandType92D == BAND_ON_5G){
		if(UndecoratedSmoothedPWDB >= 0x33)
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Level2 (TxPwr=0x0)\n"));
		}
		else if((UndecoratedSmoothedPWDB <0x33) &&
			(UndecoratedSmoothedPWDB >= 0x2b) )
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
		}
		else if(UndecoratedSmoothedPWDB < 0x2b)
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("5G:TxHighPwrLevel_Normal\n"));
		}
	}
	else
#endif
	{
		if(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL2)
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level2;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x0)\n"));
		}
		else if((UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL2-3)) &&
			(UndecoratedSmoothedPWDB >= TX_POWER_NEAR_FIELD_THRESH_LVL1) )
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Level1;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Level1 (TxPwr=0x10)\n"));
		}
		else if(UndecoratedSmoothedPWDB < (TX_POWER_NEAR_FIELD_THRESH_LVL1-5))
		{
			pdmpriv->DynamicTxHighPowerLvl = TxHighPwrLevel_Normal;
			//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("TxHighPwrLevel_Normal\n"));
		}
	}
#if (RTL8192D_EASY_SMART_CONCURRENT == 1)
	if(bGetValueFromBuddyAdapter)
	{
		//ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() mac 0 for mac 1 \n"));
		if(Adapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP)
		{
			//ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() change value \n"));
			HighPowerLvlBackForMac0 = pHalData->DynamicTxHighPowerLvl;
			pHalData->DynamicTxHighPowerLvl = Adapter->DualMacDMSPControl.CurTxHighLvlForAnotherMacOfDMSP;
			PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
			pHalData->DynamicTxHighPowerLvl = HighPowerLvlBackForMac0;
			Adapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP = _FALSE;
		}						
	}
#endif

	if( (pdmpriv->DynamicTxHighPowerLvl != pdmpriv->LastDTPLvl) )
	{
		//ODM_RT_TRACE(pDM_Odm,COMP_HIPWR, DBG_LOUD, ("PHY_SetTxPowerLevel8192S() Channel = %d \n" , pHalData->CurrentChannel));
#if (RTL8192D_EASY_SMART_CONCURRENT == 1)
		if(BuddyAdapter == NULL)
		{
			//ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter == NULL case \n"));
			if(!Adapter->bSlaveOfDMSP)
			{
				PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
			}
		}
		else
		{
			if(pHalData->MacPhyMode92D == DUALMAC_SINGLEPHY)
			{
				//ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter DMSP \n"));
				if(Adapter->bSlaveOfDMSP)
				{
					//ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() bslave case  \n"));
					BuddyAdapter->DualMacDMSPControl.bChangeTxHighPowerLvlForAnotherMacOfDMSP = _TRUE;
					BuddyAdapter->DualMacDMSPControl.CurTxHighLvlForAnotherMacOfDMSP = pHalData->DynamicTxHighPowerLvl;
				}
				else
				{
					//ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() master case  \n"));					
					if(!bGetValueFromBuddyAdapter)
					{
						//ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() mac 0 for mac 0 \n"));
						PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
					}
				}
			}
			else
			{
				//ODM_RT_TRACE(pDM_Odm,COMP_MLME,DBG_LOUD,("dm_DynamicTxPower() BuddyAdapter DMDP\n"));
				PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
			}
		}
#else
		PHY_SetTxPowerLevel8192D(Adapter, pHalData->CurrentChannel);
#endif
	}
	pdmpriv->LastDTPLvl = pdmpriv->DynamicTxHighPowerLvl;
#endif	
#endif	// #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

}


//3============================================================
//3 RSSI Monitor
//3============================================================

VOID
odm_RSSIMonitorInit(
	IN	PDM_ODM_T	pDM_Odm
	)
{
}

VOID
odm_RSSIMonitorCheck(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	// 
	// For AP/ADSL use prtl8192cd_priv
	// For CE/NIC use PADAPTER
	//
//	PADAPTER		pAdapter = pDM_Odm->Adapter;
//	prtl8192cd_priv	priv		= pDM_Odm->priv;

	if (!(pDM_Odm->SupportAbility & ODM_BB_RSSI_MONITOR))
		return;
	
	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:
			odm_RSSIMonitorCheckMP(pDM_Odm);
			break;

		case	ODM_CE:
			odm_RSSIMonitorCheckCE(pDM_Odm);
			break;

		case	ODM_AP:
			odm_RSSIMonitorCheckAP(pDM_Odm);
			break;		

		case	ODM_ADSL:
			//odm_DIGAP(pDM_Odm);
			break;	
	}
	
}	// odm_RSSIMonitorCheck


VOID
odm_RSSIMonitorCheckMP(
	IN	PDM_ODM_T	pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PRT_WLAN_STA	pEntry;
	u4Byte			i;
	s4Byte			tmpEntryMaxPWDB=0, tmpEntryMinPWDB=0xff;

	RTPRINT(FDM, DM_PWDB, ("pHalData->UndecoratedSmoothedPWDB = 0x%x( %d)\n", 
		pHalData->UndecoratedSmoothedPWDB,
		pHalData->UndecoratedSmoothedPWDB));

	for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
		{
			pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
		}
		else
		{
			pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);
		}

		if(pEntry!=NULL)
		{
			if(pEntry->bAssociated)
			{
				RTPRINT_ADDR(FDM, DM_PWDB, ("pEntry->MacAddr ="), pEntry->MacAddr);
				RTPRINT(FDM, DM_PWDB, ("pEntry->rssi = 0x%x(%d)\n", 
					pEntry->rssi_stat.UndecoratedSmoothedPWDB,
					pEntry->rssi_stat.UndecoratedSmoothedPWDB));
				if(pEntry->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
					tmpEntryMinPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;
				if(pEntry->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
					tmpEntryMaxPWDB = pEntry->rssi_stat.UndecoratedSmoothedPWDB;
			}
		}
		else
		{
			break;
		}
	}

	if(tmpEntryMaxPWDB != 0)	// If associated entry is found
	{
		pHalData->EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;
		RTPRINT(FDM, DM_PWDB, ("EntryMaxPWDB = 0x%x(%d)\n", 
			tmpEntryMaxPWDB, tmpEntryMaxPWDB));
	}
	else
	{
		pHalData->EntryMaxUndecoratedSmoothedPWDB = 0;
	}
	if(tmpEntryMinPWDB != 0xff) // If associated entry is found
	{
		pHalData->EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;
		RTPRINT(FDM, DM_PWDB, ("EntryMinPWDB = 0x%x(%d)\n", 
					tmpEntryMinPWDB, tmpEntryMinPWDB));
	}
	else
	{
		pHalData->EntryMinUndecoratedSmoothedPWDB = 0;
	}

	// Indicate Rx signal strength to FW.
	if(Adapter->MgntInfo.bUseRAMask)
	{
		u1Byte	H2C_Parameter[3] ={0};
	//	DbgPrint("RxSS: %lx =%ld\n", pHalData->UndecoratedSmoothedPWDB, pHalData->UndecoratedSmoothedPWDB);
		H2C_Parameter[2] = (u1Byte)(pHalData->UndecoratedSmoothedPWDB & 0xFF);
		H2C_Parameter[1] = 0x20;   // fw v12 cmdid 5:use max macid ,for nic ,default macid is 0 ,max macid is 1

		ODM_FillH2CCmd(Adapter, ODM_H2C_RSSI_REPORT, 3, H2C_Parameter);
	}
	else
	{
		PlatformEFIOWrite1Byte(Adapter, 0x4fe, (u1Byte)pHalData->UndecoratedSmoothedPWDB);
		//DbgPrint("0x4fe write %x %d\n", pHalData->UndecoratedSmoothedPWDB, pHalData->UndecoratedSmoothedPWDB);
	}
#endif	// #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
}


VOID
odm_RSSIMonitorCheckCE(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	struct dm_priv	*pdmpriv = &pHalData->dmpriv;
	int	i;
	int	tmpEntryMaxPWDB=0, tmpEntryMinPWDB=0xff;
	u8 	sta_cnt=0;
	u32 PWDB_rssi[NUM_STA]={0};//[0~15]:MACID, [16~31]:PWDB_rssi

	if(check_fwstate(&Adapter->mlmepriv, _FW_LINKED) != _TRUE)
		return;
		

	//if(check_fwstate(&Adapter->mlmepriv, WIFI_AP_STATE|WIFI_ADHOC_STATE|WIFI_ADHOC_MASTER_STATE) == _TRUE)
	{
		_irqL irqL;
		_list	*plist, *phead;
		struct sta_info *psta;
		struct sta_priv *pstapriv = &Adapter->stapriv;
		u8 bcast_addr[ETH_ALEN]= {0xff,0xff,0xff,0xff,0xff,0xff};
	
		_enter_critical_bh(&pstapriv->sta_hash_lock, &irqL);

		for(i=0; i< NUM_STA; i++)
		{
			phead = &(pstapriv->sta_hash[i]);
			plist = get_next(phead);
		
			while ((rtw_end_of_queue_search(phead, plist)) == _FALSE)
			{
				psta = LIST_CONTAINOR(plist, struct sta_info, hash_list);

				plist = get_next(plist);

				if(_rtw_memcmp(psta->hwaddr, bcast_addr, ETH_ALEN) || 
					_rtw_memcmp(psta->hwaddr, myid(&Adapter->eeprompriv), ETH_ALEN))
					continue;

				if(psta->state & WIFI_ASOC_STATE)
				{
					
					if(psta->rssi_stat.UndecoratedSmoothedPWDB < tmpEntryMinPWDB)
						tmpEntryMinPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

					if(psta->rssi_stat.UndecoratedSmoothedPWDB > tmpEntryMaxPWDB)
						tmpEntryMaxPWDB = psta->rssi_stat.UndecoratedSmoothedPWDB;

					if(psta->rssi_stat.UndecoratedSmoothedPWDB != (-1)){
						//printk("%s==> mac_id(%d),rssi(%d)\n",__FUNCTION__,psta->mac_id,psta->rssi_stat.UndecoratedSmoothedPWDB);
						#if(RTL8192D_SUPPORT==1)
						PWDB_rssi[sta_cnt++] = (psta->mac_id | (psta->rssi_stat.UndecoratedSmoothedPWDB<<16) | ((Adapter->stapriv.asoc_sta_count+1) << 8));
						#else
						PWDB_rssi[sta_cnt++] = (psta->mac_id | (psta->rssi_stat.UndecoratedSmoothedPWDB<<16) );
						#endif
					}
				}
			
			}

		}
	
		_exit_critical_bh(&pstapriv->sta_hash_lock, &irqL);

		//printk("%s==> sta_cnt(%d)\n",__FUNCTION__,sta_cnt);

		for(i=0; i< sta_cnt; i++)
		{
			if(PWDB_rssi[i] != (0)){
				if(pHalData->fw_ractrl == _TRUE)// Report every sta's RSSI to FW
				{			
					#if(RTL8192D_SUPPORT==1)
					FillH2CCmd92D(Adapter, H2C_RSSI_REPORT, 3, (u8 *)(&PWDB_rssi[i]));					
					#elif((RTL8192C_SUPPORT==1)||(RTL8723A_SUPPORT==1))
					rtl8192c_set_rssi_cmd(Adapter, (u8*)&PWDB_rssi[i]);					
					#endif
				}
				else{
					#if((RTL8188E_SUPPORT==1)&&(RATE_ADAPTIVE_SUPPORT == 1))
					ODM_RA_SetRSSI_8188E(
					&(pHalData->odmpriv), (PWDB_rssi[i]&0xFF), (u8)((PWDB_rssi[i]>>16) & 0xFF));
					#endif
				}
			}
		}		
	}



	if(tmpEntryMaxPWDB != 0)	// If associated entry is found
	{
		pdmpriv->EntryMaxUndecoratedSmoothedPWDB = tmpEntryMaxPWDB;		
	}
	else
	{
		pdmpriv->EntryMaxUndecoratedSmoothedPWDB = 0;
	}

	if(tmpEntryMinPWDB != 0xff) // If associated entry is found
	{
		pdmpriv->EntryMinUndecoratedSmoothedPWDB = tmpEntryMinPWDB;		
	}
	else
	{
		pdmpriv->EntryMinUndecoratedSmoothedPWDB = 0;
	}

#if 0
	if(check_fwstate(&Adapter->mlmepriv, WIFI_STATION_STATE) == _TRUE)
	{
	
		if(pHalData->fw_ractrl == _TRUE)
		{
			u32 param = (u32)(pdmpriv->UndecoratedSmoothedPWDB<<16);
	printk("%s==> rssi(%d)\n",__FUNCTION__,pdmpriv->UndecoratedSmoothedPWDB);	
			param |= 0;//macid=0 for sta mode;
			
			rtl8192c_set_rssi_cmd(Adapter, (u8*)&param);
		}
		else
		{
			rtw_write8(Adapter, 0x4fe, (u8)pdmpriv->UndecoratedSmoothedPWDB);
			//DBG_8192C("0x4fe write %x %d\n", pdmpriv->UndecoratedSmoothedPWDB, pdmpriv->UndecoratedSmoothedPWDB);
		}
	}
#endif		
#endif//if (DM_ODM_SUPPORT_TYPE == ODM_CE)
}
VOID
odm_RSSIMonitorCheckAP(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
#if defined(CONFIG_RTL_92C_SUPPORT) || defined(CONFIG_RTL_92D_SUPPORT) ||defined(CONFIG_RTL_8812_SUPPORT)||defined(CONFIG_WLAN_HAL_8881A)||defined(CONFIG_WLAN_HAL_8192EE)
	{
		prtl8192cd_priv	priv		= pDM_Odm->priv;		
		u4Byte i;
		PSTA_INFO_T pstat;
		static u1Byte H2C_Parameter[5];
		u1Byte	  TxBF_EN = 0;
		pBDC_T	pDM_BdcTable = &pDM_Odm->DM_BdcTable;

		if( priv->up_time % 2 )
			return;

		pDM_BdcTable->num_Txbfee_Client=0;
		pDM_BdcTable->num_Txbfer_Client=0;
		//pDM_BdcTable->num_Client=0;
		
		for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
		{
			pstat = pDM_Odm->pODM_StaInfo[i];
			if(IS_STA_VALID(pstat) )
			{			
#ifdef BEAMFORMING_SUPPORT
				BEAMFORMING_CAP Beamform_cap = Beamforming_GetEntryBeamCapByMacId(priv, pstat->aid);
				if(Beamform_cap == BEAMFORMER_CAP_HT_EXPLICIT || Beamform_cap == BEAMFORMER_CAP_VHT_SU ||
					 Beamform_cap == (BEAMFORMER_CAP_HT_EXPLICIT|BEAMFORMEE_CAP_HT_EXPLICIT) ||
					 Beamform_cap == (BEAMFORMER_CAP_VHT_SU|BEAMFORMEE_CAP_VHT_SU))
				{
					TxBF_EN = (1<< 6);
					pDM_BdcTable->w_BFee_Client[i]=1; //AP act as BFer
					pDM_BdcTable->num_Txbfee_Client++;
				}
				else 
				{
					pDM_BdcTable->w_BFee_Client[i]=0; //AP act as BFer
				}
				
				if((Beamform_cap & BEAMFORMEE_CAP_HT_EXPLICIT) || (Beamform_cap & BEAMFORMEE_CAP_VHT_SU) )
				{
					pDM_BdcTable->w_BFer_Client[i]=1; //AP act as BFee
					pDM_BdcTable->num_Txbfer_Client++;
				}
				else 
				{
					pDM_BdcTable->w_BFer_Client[i]=0; //AP act as BFer
				}

					
				//pDM_BdcTable->num_Client++;
		
				
				
#endif			
                if(pstat->sta_in_firmware == 1) {
#ifdef CONFIG_RTL_8812_SUPPORT
					if(pDM_Odm->SupportICType == ODM_RTL8812) {
						memset(H2C_Parameter,0,5);						
						H2C_Parameter[2] = (u1Byte)(pstat->rssi & 0x7F);
						H2C_Parameter[0] = REMAP_AID(pstat);				
						if ((priv->pmib->dot11nConfigEntry.dot11nSTBC) && (
							(pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_RX_STBC_CAP_)
#ifdef RTK_AC_SUPPORT			
							|| (pstat->vht_cap_buf.vht_cap_info & cpu_to_le32(_VHTCAP_RX_STBC_CAP_))
#endif
							)))	
							H2C_Parameter[3] |= 2;
						H2C_Parameter[3] |= TxBF_EN ; 
						FillH2CCmd8812(pDM_Odm->priv, H2C_8812_RSSI_REPORT, 4, H2C_Parameter);
					}
#endif

#if defined(CONFIG_WLAN_HAL_8881A) || defined(CONFIG_WLAN_HAL_8192EE)
					if(pDM_Odm->SupportICType == ODM_RTL8881A || pDM_Odm->SupportICType == ODM_RTL8192E) {
//						u1Byte	H2C_Parameter[5] ={0};	
						u1Byte	cmdlen = 3;
						memset(H2C_Parameter, 0, 5);
						H2C_Parameter[2] = (u1Byte)(pstat->rssi & 0xFF);
						H2C_Parameter[0] = REMAP_AID(pstat);
						if(pDM_Odm->SupportICType == ODM_RTL8192E) {
							cmdlen = 4;
							if ((priv->pmib->dot11nConfigEntry.dot11nSTBC) && (pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_RX_STBC_CAP_)))	
								H2C_Parameter[3] |= 2;		
							 H2C_Parameter[3] |= TxBF_EN; 

						} 
                        GET_HAL_INTERFACE(pDM_Odm->priv)->FillH2CCmdHandler(pDM_Odm->priv, H2C_88XX_RSSI_REPORT, cmdlen, H2C_Parameter);
					}
#endif

#if defined(CONFIG_RTL_92C_SUPPORT) || defined(CONFIG_RTL_92D_SUPPORT)
					if(pDM_Odm->SupportICType == ODM_RTL8192C || pDM_Odm->SupportICType == ODM_RTL8192D) 
						add_update_rssi(pDM_Odm->priv, pstat);
#endif
				}

			}		
		}
	}
#endif
#endif

}



VOID
ODM_InitAllTimers(
	IN PDM_ODM_T	pDM_Odm 
	)
{	
	ODM_InitializeTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer,
		(RT_TIMER_CALL_BACK)odm_SwAntDivChkAntSwitchCallback, NULL, "SwAntennaSwitchTimer");
#ifdef MP_TEST
	if (pDM_Odm->priv->pshare->rf_ft_var.mp_specific) 
		ODM_InitializeTimer(pDM_Odm, &pDM_Odm->MPT_DIGTimer, 
			(RT_TIMER_CALL_BACK)odm_MPT_DIGCallback, NULL, "MPT_DIGTimer");	
#endif


#if( defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY) ) ||( defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY) )
	ODM_InitializeTimer(pDM_Odm,&pDM_Odm->FastAntTrainingTimer,
		(RT_TIMER_CALL_BACK)odm_FastAntTrainingCallback, NULL, "FastAntTrainingTimer");
#endif


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->PSDTimer, 
		(RT_TIMER_CALL_BACK)dm_PSDMonitorCallback, NULL, "PSDTimer");
	//
	//Path Diversity
	//Neil Chen--2011--06--16--  / 2012/02/23 MH Revise Arch.
	//
	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 
		(RT_TIMER_CALL_BACK)odm_PathDivChkAntSwitchCallback, NULL, "PathDivTimer");

	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->CCKPathDiversityTimer, 
		(RT_TIMER_CALL_BACK)odm_CCKTXPathDiversityCallback, NULL, "CCKPathDiversityTimer");

	ODM_InitializeTimer(pDM_Odm, &pDM_Odm->DM_RXHP_Table.PSDTimer,
		(RT_TIMER_CALL_BACK)odm_PSD_RXHPCallback, NULL, "PSDRXHPTimer");  
#endif	
}

VOID
ODM_CancelAllTimers(
	IN PDM_ODM_T	pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	//
	// 2012/01/12 MH Temp BSOD fix. We need to find NIC allocate mem fail reason in 
	// win7 platform.
	//
	HAL_ADAPTER_STS_CHK(pDM_Odm)
#endif	

	ODM_CancelTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer);
#ifdef MP_TEST
	if (pDM_Odm->priv->pshare->rf_ft_var.mp_specific)
		ODM_CancelTimer(pDM_Odm, &pDM_Odm->MPT_DIGTimer);
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

#if (RTL8188E_SUPPORT == 1)
	ODM_CancelTimer(pDM_Odm,&pDM_Odm->FastAntTrainingTimer);
#endif
	ODM_CancelTimer(pDM_Odm, &pDM_Odm->PSDTimer);	
	//
	//Path Diversity
	//Neil Chen--2011--06--16--  / 2012/02/23 MH Revise Arch.
	//
	ODM_CancelTimer(pDM_Odm, &pDM_Odm->PathDivSwitchTimer);

	ODM_CancelTimer(pDM_Odm, &pDM_Odm->CCKPathDiversityTimer);

	ODM_CancelTimer(pDM_Odm, &pDM_Odm->DM_RXHP_Table.PSDTimer);
#endif	
}


VOID
ODM_ReleaseAllTimers(
	IN PDM_ODM_T	pDM_Odm 
	)
{
	ODM_ReleaseTimer(pDM_Odm,&pDM_Odm->DM_SWAT_Table.SwAntennaSwitchTimer);
#ifdef MP_TEST
	if (pDM_Odm->priv->pshare->rf_ft_var.mp_specific)
		ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->MPT_DIGTimer);
#endif

#if (RTL8188E_SUPPORT == 1)
	ODM_ReleaseTimer(pDM_Odm,&pDM_Odm->FastAntTrainingTimer);
#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->PSDTimer);
	//
	//Path Diversity
	//Neil Chen--2011--06--16--  / 2012/02/23 MH Revise Arch.
	//
	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->PathDivSwitchTimer);

	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->CCKPathDiversityTimer);

	ODM_ReleaseTimer(pDM_Odm, &pDM_Odm->DM_RXHP_Table.PSDTimer); 
#endif	
}


VOID
ODM_InitAllThreads(
	IN PDM_ODM_T	pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	#ifdef TPT_THREAD
	kTPT_task_init(pDM_Odm->priv);
	#endif
#endif
}

VOID
ODM_StopAllThreads(
	IN PDM_ODM_T	pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	#ifdef TPT_THREAD
	kTPT_task_stop(pDM_Odm->priv);
	#endif
#endif	
}


//#endif
//3============================================================
//3 Tx Power Tracking
//3============================================================

VOID
odm_TXPowerTrackingInit(
	IN	PDM_ODM_T	pDM_Odm 
	)
{
	odm_TXPowerTrackingThermalMeterInit(pDM_Odm);
}	


VOID
odm_TXPowerTrackingThermalMeterInit(
	IN	PDM_ODM_T	pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER		Adapter = pDM_Odm->Adapter;
	PMGNT_INFO      	pMgntInfo = &Adapter->MgntInfo;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

	pMgntInfo->bTXPowerTracking = TRUE;
	pHalData->TXPowercount       = 0;
	pHalData->bTXPowerTrackingInit = FALSE;
	#if	MP_DRIVER != 1					//for mp driver, turn off txpwrtracking as default
	pHalData->TxPowerTrackControl = TRUE;		
	#endif//#if	(MP_DRIVER != 1)
	ODM_RT_TRACE(pDM_Odm,COMP_POWER_TRACKING, DBG_LOUD, ("pMgntInfo->bTXPowerTracking = %d\n", pMgntInfo->bTXPowerTracking));
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
	#ifdef CONFIG_RTL8188E
	{
		pDM_Odm->RFCalibrateInfo.bTXPowerTracking = _TRUE;
		pDM_Odm->RFCalibrateInfo.TXPowercount = 0;
		pDM_Odm->RFCalibrateInfo.bTXPowerTrackingInit = _FALSE;
		#if	(MP_DRIVER != 1)		//for mp driver, turn off txpwrtracking as default
		pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = _TRUE;
		#endif//#if	(MP_DRIVER != 1)
		MSG_8192C("pDM_Odm TxPowerTrackControl = %d\n", pDM_Odm->RFCalibrateInfo.TxPowerTrackControl);
	}
	#else
	{
		PADAPTER		Adapter = pDM_Odm->Adapter;
		HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
		struct dm_priv	*pdmpriv = &pHalData->dmpriv;

		//if(IS_HARDWARE_TYPE_8192C(pHalData))
		{
			pdmpriv->bTXPowerTracking = _TRUE;
			pdmpriv->TXPowercount = 0;
			pdmpriv->bTXPowerTrackingInit = _FALSE;
			#if	(MP_DRIVER != 1)		//for mp driver, turn off txpwrtracking as default
			pdmpriv->TxPowerTrackControl = _TRUE;
			#endif//#if	(MP_DRIVER != 1)
		}
		MSG_8192C("pdmpriv->TxPowerTrackControl = %d\n", pdmpriv->TxPowerTrackControl);
	
	}	
	#endif//endif (CONFIG_RTL8188E==1)	
#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
	#ifdef RTL8188E_SUPPORT
	{
		pDM_Odm->RFCalibrateInfo.bTXPowerTracking = _TRUE;
		pDM_Odm->RFCalibrateInfo.TXPowercount = 0;
		pDM_Odm->RFCalibrateInfo.bTXPowerTrackingInit = _FALSE;
		pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = _TRUE;
		pDM_Odm->RFCalibrateInfo.TM_Trigger = 0;
	}
	#endif
#endif

    pDM_Odm->RFCalibrateInfo.TxPowerTrackControl = TRUE;
	pDM_Odm->RFCalibrateInfo.DeltaPowerIndex = 0;
	pDM_Odm->RFCalibrateInfo.DeltaPowerIndexLast = 0;
	pDM_Odm->RFCalibrateInfo.PowerIndexOffset = 0;
	pDM_Odm->RFCalibrateInfo.ThermalValue = 0;
	pDM_Odm->DefaultOfdmIndex = 28;
#if RTL8188E_SUPPORT	
	pDM_Odm->DefaultCckIndex = 20;	// -6 dB	
#elif RTL8192E_SUPPORT
	pDM_Odm->DefaultCckIndex = 8;	// -12 dB
#endif
	pDM_Odm->BbSwingIdxOfdmBase = pDM_Odm->DefaultOfdmIndex;
	pDM_Odm->BbSwingIdxCckBase = pDM_Odm->DefaultCckIndex;
	pDM_Odm->RFCalibrateInfo.CCK_index = pDM_Odm->DefaultCckIndex;
   	pDM_Odm->RFCalibrateInfo.OFDM_index[RF_PATH_A] = pDM_Odm->DefaultOfdmIndex;		
	pDM_Odm->RFCalibrateInfo.OFDM_index[RF_PATH_B] = pDM_Odm->DefaultOfdmIndex;		
	pDM_Odm->BbSwingIdxOfdm[RF_PATH_A] = pDM_Odm->DefaultOfdmIndex;
	pDM_Odm->BbSwingIdxOfdm[RF_PATH_B] = pDM_Odm->DefaultOfdmIndex;
	pDM_Odm->BbSwingIdxCck = pDM_Odm->DefaultCckIndex;
}


VOID
ODM_TXPowerTrackingCheck(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	// 
	// For AP/ADSL use prtl8192cd_priv
	// For CE/NIC use PADAPTER
	//
	PADAPTER		pAdapter = pDM_Odm->Adapter;
	prtl8192cd_priv	priv		= pDM_Odm->priv;

	if (!(pDM_Odm->SupportAbility & ODM_RF_TX_PWR_TRACK))
		return;

	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:
			odm_TXPowerTrackingCheckMP(pDM_Odm);
			break;

		case	ODM_CE:
			odm_TXPowerTrackingCheckCE(pDM_Odm);
			break;

		case	ODM_AP:
			odm_TXPowerTrackingCheckAP(pDM_Odm);		
			break;		

		case	ODM_ADSL:
			//odm_DIGAP(pDM_Odm);
			break;	
	}

}

VOID
odm_TXPowerTrackingCheckCE(
	IN		PDM_ODM_T		pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
	PADAPTER	Adapter = pDM_Odm->Adapter;
	#if( (RTL8192C_SUPPORT==1) ||  (RTL8723A_SUPPORT==1) )
	rtl8192c_odm_CheckTXPowerTracking(Adapter);
	#endif

	#if (RTL8192D_SUPPORT==1) 
	#if (RTL8192D_EASY_SMART_CONCURRENT == 1)
	if(!Adapter->bSlaveOfDMSP)
	#endif
		rtl8192d_odm_CheckTXPowerTracking(Adapter);
	#endif
	#if(RTL8188E_SUPPORT==1)

	//if(!pMgntInfo->bTXPowerTracking /*|| (!pdmpriv->TxPowerTrackControl && pdmpriv->bAPKdone)*/)
	if(!(pDM_Odm->SupportAbility & ODM_RF_TX_PWR_TRACK))
	{
		return;
	}

	if(!pDM_Odm->RFCalibrateInfo.TM_Trigger)		//at least delay 1 sec
	{
		//pHalData->TxPowerCheckCnt++;	//cosa add for debug
		ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_T_METER, bRFRegOffsetMask, 0x60);
		//DBG_8192C("Trigger 92C Thermal Meter!!\n");
		
		pDM_Odm->RFCalibrateInfo.TM_Trigger = 1;
		return;
		
	}
	else
	{
		//DBG_8192C("Schedule TxPowerTracking direct call!!\n");
		odm_TXPowerTrackingCallback_ThermalMeter_8188E(Adapter);
		pDM_Odm->RFCalibrateInfo.TM_Trigger = 0;
	}
	#endif
	
#endif	
}

VOID
odm_TXPowerTrackingCheckMP(
	IN		PDM_ODM_T		pDM_Odm 
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER	Adapter = pDM_Odm->Adapter;

	if (ODM_CheckPowerStatus(Adapter) == FALSE)
		return;

	if(IS_HARDWARE_TYPE_8723A(Adapter))
		return;

	if(!Adapter->bSlaveOfDMSP || Adapter->DualMacSmartConcurrent == FALSE)
		odm_TXPowerTrackingThermalMeterCheck(Adapter);
#endif
	
}


VOID
odm_TXPowerTrackingCheckAP(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	prtl8192cd_priv	priv		= pDM_Odm->priv;

#if ((RTL8188E_SUPPORT==1)||(RTL8192E_SUPPORT==1) ||(RTL8812E_SUPPORT==1)||(RTL8881A_SUPPORT==1))	
	if (pDM_Odm->SupportICType & (ODM_RTL8188E|ODM_RTL8192E|ODM_RTL8812|ODM_RTL8881A))
		ODM_TXPowerTrackingCallback_ThermalMeter(pDM_Odm);
	else
#endif
#if (RTL8192D_SUPPORT==1) 
	if (pDM_Odm->SupportICType==ODM_RTL8192D)
		tx_power_tracking_92D(priv);
	else 
#endif
#if (RTL8192C_SUPPORT==1) 			
	if (pDM_Odm->SupportICType==ODM_RTL8192C)
		tx_power_tracking(priv);
	else
#endif
	{
	}
#endif	

}



//antenna mapping info
// 1: right-side antenna
// 2/0: left-side antenna
//PDM_SWAT_Table->CCK_Ant1_Cnt /OFDM_Ant1_Cnt:  for right-side antenna:   Ant:1    RxDefaultAnt1
//PDM_SWAT_Table->CCK_Ant2_Cnt /OFDM_Ant2_Cnt:  for left-side antenna:     Ant:0    RxDefaultAnt2
// We select left antenna as default antenna in initial process, modify it as needed
//

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

VOID
odm_TXPowerTrackingThermalMeterCheck(
	IN	PADAPTER		Adapter
	)
{
#ifndef AP_BUILD_WORKAROUND
#if (HAL_CODE_BASE==RTL8192_C)
	PMGNT_INFO      		pMgntInfo = &Adapter->MgntInfo;
	//HAL_DATA_TYPE			*pHalData = GET_HAL_DATA(Adapter);
	static u1Byte			TM_Trigger = 0;
	//u1Byte					TxPowerCheckCnt = 5;	//10 sec

	if(!pMgntInfo->bTXPowerTracking /*|| (!pHalData->TxPowerTrackControl && pHalData->bAPKdone)*/)
	{
		return;
	}

	if(!TM_Trigger)		//at least delay 1 sec
	{
		if(IS_HARDWARE_TYPE_8192D(Adapter))
			PHY_SetRFReg(Adapter, RF_PATH_A, RF_T_METER_92D, BIT17 | BIT16, 0x03);
		else if(IS_HARDWARE_TYPE_8188E(Adapter) || IS_HARDWARE_TYPE_8812(Adapter))
			PHY_SetRFReg(Adapter, RF_PATH_A, RF_T_METER_88E, BIT17 | BIT16, 0x03);
		else
			PHY_SetRFReg(Adapter, RF_PATH_A, RF_T_METER, bRFRegOffsetMask, 0x60);
		RT_TRACE(COMP_POWER_TRACKING, DBG_LOUD,("Trigger 92C Thermal Meter!!\n"));
		
		TM_Trigger = 1;
		return;
	}
	else
	{
		RT_TRACE(COMP_POWER_TRACKING, DBG_LOUD,("Schedule TxPowerTracking direct call!!\n"));		
		odm_TXPowerTrackingDirectCall(Adapter); //Using direct call is instead, added by Roger, 2009.06.18.
		TM_Trigger = 0;
	}
#endif
#endif
}

// Only for 8723A SW ANT DIV INIT--2012--07--17
VOID
odm_SwAntDivInit_NIC_8723A(
	IN	PDM_ODM_T		pDM_Odm)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	PADAPTER		Adapter = pDM_Odm->Adapter;
	
	u1Byte 			btAntNum=BT_GetPGAntNum(Adapter);

	if(IS_HARDWARE_TYPE_8723A(Adapter))
	{
		pDM_SWAT_Table->ANTA_ON =TRUE;
		
		// Set default antenna B status by PG
		if(btAntNum == Ant_x2)
			pDM_SWAT_Table->ANTB_ON = TRUE;
		else if(btAntNum ==Ant_x1)
			pDM_SWAT_Table->ANTB_ON = FALSE;
		else
			pDM_SWAT_Table->ANTB_ON = TRUE;
	}	
	
}

#endif //end #ifMP



//3============================================================
//3 SW Antenna Diversity
//3============================================================
#if(defined(CONFIG_SW_ANTENNA_DIVERSITY))
VOID
odm_SwAntDivInit(
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN|ODM_CE))
	odm_SwAntDivInit_NIC(pDM_Odm);
#elif(DM_ODM_SUPPORT_TYPE == ODM_AP)
	dm_SW_AntennaSwitchInit(pDM_Odm->priv);
#endif
}

VOID
odm_SwAntDivInit_NIC(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;


// Init SW ANT DIV mechanism for 8723AE/AU/AS
// Neil Chen--2012--07--17---
// CE/AP/ADSL no using SW ANT DIV for 8723A Series IC
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)

	if(pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		odm_SwAntDivInit_NIC_8723A(pDM_Odm);	
	}	
#endif
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS:Init SW Antenna Switch\n"));
	pDM_SWAT_Table->RSSI_sum_A = 0;
	pDM_SWAT_Table->RSSI_cnt_A = 0;
	pDM_SWAT_Table->RSSI_sum_B = 0;
	pDM_SWAT_Table->RSSI_cnt_B = 0;
	pDM_SWAT_Table->CurAntenna = MAIN_ANT;
	pDM_SWAT_Table->PreAntenna = MAIN_ANT;
	pDM_SWAT_Table->try_flag = 0xff;
	pDM_SWAT_Table->PreRSSI = 0;
	pDM_SWAT_Table->SWAS_NoLink_State = 0;
	pDM_SWAT_Table->bTriggerAntennaSwitch = 0;
	pDM_SWAT_Table->SelectAntennaMap=0xAA;
	pDM_SWAT_Table->lastTxOkCnt = 0;
	pDM_SWAT_Table->lastRxOkCnt = 0;
	pDM_SWAT_Table->TXByteCnt_A = 0;
	pDM_SWAT_Table->TXByteCnt_B = 0;
	pDM_SWAT_Table->RXByteCnt_A = 0;
	pDM_SWAT_Table->RXByteCnt_B = 0;
	pDM_SWAT_Table->TrafficLoad = TRAFFIC_LOW;
	pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ODM_Read4Byte(pDM_Odm, 0x860);
	
}

//
// 20100514 Joseph: 
// Add new function to reset the state of antenna diversity before link.
//
VOID
ODM_SwAntDivResetBeforeLink(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	pDM_SWAT_Table->SWAS_NoLink_State = 0;

}

//
// 20100514 Luke/Joseph:
// Add new function to reset antenna diversity state after link.
//
VOID
ODM_SwAntDivRestAfterLink(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	pDM_SWAT_Table->RSSI_cnt_A = 0;
	pDM_SWAT_Table->RSSI_cnt_B = 0;
	pDM_Odm->RSSI_test = FALSE;
	pDM_SWAT_Table->try_flag = 0xff;
	pDM_SWAT_Table->RSSI_Trying = 0;
	pDM_SWAT_Table->SelectAntennaMap=0xAA;
}

VOID
ODM_SwAntDivChkPerPktRssi(
	IN PDM_ODM_T	pDM_Odm,
	IN u1Byte		StationID,
	IN PODM_PHY_INFO_T pPhyInfo
	)
{	
	SWAT_T		*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	
	if(!(pDM_Odm->SupportAbility & (ODM_BB_ANT_DIV)))
		return;

// temporary Fix 8723A MP SW ANT DIV Bug --NeilChen--2012--07--11
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	if(pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		//if(StationID == pDM_SWAT_Table->RSSI_target)
		//{
		//1 RSSI for SW Antenna Switch
		if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
		{
			pDM_SWAT_Table->RSSI_sum_A += pPhyInfo->RxPWDBAll;
			pDM_SWAT_Table->RSSI_cnt_A++;
		}
		else
		{
			pDM_SWAT_Table->RSSI_sum_B += pPhyInfo->RxPWDBAll;
			pDM_SWAT_Table->RSSI_cnt_B++;

		}
		//}
	}
	else
	{
		if(StationID == pDM_SWAT_Table->RSSI_target)
		{
			//1 RSSI for SW Antenna Switch
			if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
			{
				pDM_SWAT_Table->RSSI_sum_A += pPhyInfo->RxPWDBAll;
				pDM_SWAT_Table->RSSI_cnt_A++;
			}
			else
			{
				pDM_SWAT_Table->RSSI_sum_B += pPhyInfo->RxPWDBAll;
				pDM_SWAT_Table->RSSI_cnt_B++;

			}
		}
	}
#else	
	if(StationID == pDM_SWAT_Table->RSSI_target)
	{
		//1 RSSI for SW Antenna Switch
		if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
		{
			pDM_SWAT_Table->RSSI_sum_A += pPhyInfo->RxPWDBAll;
			pDM_SWAT_Table->RSSI_cnt_A++;
		}
		else
		{
			pDM_SWAT_Table->RSSI_sum_B += pPhyInfo->RxPWDBAll;
			pDM_SWAT_Table->RSSI_cnt_B++;

		}
	}
#endif
}

//
VOID
odm_SwAntDivChkAntSwitch(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			Step
	)
{
	// 
	// For AP/ADSL use prtl8192cd_priv
	// For CE/NIC use PADAPTER
	//
	PADAPTER		pAdapter = pDM_Odm->Adapter;
	prtl8192cd_priv	priv		= pDM_Odm->priv;

	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:	
			odm_SwAntDivChkAntSwitchNIC(pDM_Odm, Step);
			break;
		case	ODM_CE:
			odm_SwAntDivChkAntSwitchNIC(pDM_Odm, Step);
			break;

		case	ODM_AP:
		case	ODM_ADSL:
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP |ODM_ADSL))
			if (priv->pshare->rf_ft_var.antSw_enable && (priv->up_time % 4==1))
				dm_SW_AntennaSwitch(priv, SWAW_STEP_PEAK);
#endif		
			break;			
	}

}

//
// 20100514 Luke/Joseph:
// Add new function for antenna diversity after link.
// This is the main function of antenna diversity after link.
// This function is called in HalDmWatchDog() and ODM_SwAntDivChkAntSwitchCallback().
// HalDmWatchDog() calls this function with SWAW_STEP_PEAK to initialize the antenna test.
// In SWAW_STEP_PEAK, another antenna and a 500ms timer will be set for testing.
// After 500ms, ODM_SwAntDivChkAntSwitchCallback() calls this function to compare the signal just
// listened on the air with the RSSI of original antenna.
// It chooses the antenna with better RSSI.
// There is also a aged policy for error trying. Each error trying will cost more 5 seconds waiting 
// penalty to get next try.


VOID
ODM_SetAntenna(
	IN 	PDM_ODM_T	pDM_Odm,
	IN	u1Byte		Antenna)
{
	ODM_SetBBReg(pDM_Odm, 0x860, BIT8|BIT9, Antenna); 
}

VOID
odm_SwAntDivChkAntSwitchNIC(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte		Step
	)
{
#if (RTL8192C_SUPPORT==1) 
	//PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);

#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PADAPTER		Adapter=pDM_Odm->Adapter;
#endif

	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	s4Byte			curRSSI=100, RSSI_A, RSSI_B;
	u1Byte			nextAntenna=AUX_ANT;
	//static u8Byte		lastTxOkCnt=0, lastRxOkCnt=0;
	u8Byte			curTxOkCnt, curRxOkCnt;
	//static u8Byte		TXByteCnt_A=0, TXByteCnt_B=0, RXByteCnt_A=0, RXByteCnt_B=0;
	u8Byte			CurByteCnt=0, PreByteCnt=0;
	//static u1Byte		TrafficLoad = TRAFFIC_LOW;
	u1Byte			Score_A=0, Score_B=0;       //A: Main; B: AUX
	u1Byte			i;

	if(!(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV))
		return;

	if (pDM_Odm->SupportICType & (ODM_RTL8192D|ODM_RTL8188E))
		return;

	if((pDM_Odm->SupportICType == ODM_RTL8192C) &&(pDM_Odm->RFType == ODM_2T2R))
		return;

	if(pDM_Odm->SupportPlatform & ODM_WIN)
	{
		if(*(pDM_Odm->pAntennaTest))
			return;
	}

	if((pDM_SWAT_Table->ANTA_ON == FALSE) ||(pDM_SWAT_Table->ANTB_ON == FALSE))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,
				("odm_SwAntDivChkAntSwitch(): No AntDiv Mechanism, Antenna A or B is off\n"));
		return;
	}

	// Radio off: Status reset to default and return.
	if(*(pDM_Odm->pbPowerSaving)==TRUE) //pHalData->eRFPowerState==eRfOff
	{
		ODM_SwAntDivRestAfterLink(pDM_Odm);
		return;
	}


	// Handling step mismatch condition.
	// Peak step is not finished at last time. Recover the variable and check again.
	if(	Step != pDM_SWAT_Table->try_flag	)
	{
		ODM_SwAntDivRestAfterLink(pDM_Odm);
	}

#if  (DM_ODM_SUPPORT_TYPE &( ODM_WIN| ODM_CE ))

	if(pDM_SWAT_Table->try_flag == 0xff)
	{
		pDM_SWAT_Table->RSSI_target = 0xff;
		
		#if(DM_ODM_SUPPORT_TYPE & ODM_CE)
		{
			u4Byte			index = 0;
			PSTA_INFO_T		pEntry = NULL;
			
			
			for(index=0; index<ODM_ASSOCIATE_ENTRY_NUM; index++)
			{					
				pEntry =  pDM_Odm->pODM_StaInfo[i];
				if(IS_STA_VALID(pEntry) ) {
					break;
				}
			}
			if(pEntry == NULL)
			{
				ODM_SwAntDivRestAfterLink(pDM_Odm);
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): No Link.\n"));
				return;
			}
			else
			{
				pDM_SWAT_Table->RSSI_target = index;
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): RSSI_target is PEER STA\n"));
			}
                }
		#elif (DM_ODM_SUPPORT_TYPE & ODM_WIN) 
		{
			PADAPTER	pAdapter	 =  pDM_Odm->Adapter;
			PMGNT_INFO	pMgntInfo=&pAdapter->MgntInfo;
			
			// Select RSSI checking target
			if(pMgntInfo->mAssoc && !ACTING_AS_AP(pAdapter))
			{
				// Target: Infrastructure mode AP.
				//pDM_SWAT_Table->RSSI_target = NULL;
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("odm_SwAntDivChkAntSwitch(): RSSI_target is DEF AP!\n"));
			}
			else
			{
				u4Byte			index = 0;
				PSTA_INFO_T		pEntry = NULL;
				PADAPTER		pTargetAdapter = NULL;
			
				if(pMgntInfo->mIbss )
				{
					// Target: AP/IBSS peer.
					pTargetAdapter = pAdapter;
				}
				else
				{
					pTargetAdapter = GetFirstAPAdapter(pAdapter);
				}

				if(pTargetAdapter != NULL)
				{			
					for(index=0; index<ODM_ASSOCIATE_ENTRY_NUM; index++)
					{					
						
						pEntry = AsocEntry_EnumStation(pTargetAdapter, index);
						if(pEntry != NULL)
						{
							if(pEntry->bAssociated)
								break;			
						}
						
					}
					
				}

				if(pEntry == NULL)
				{
					ODM_SwAntDivRestAfterLink(pDM_Odm);
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): No Link.\n"));
					return;
				}
				else
				{
					//pDM_SWAT_Table->RSSI_target = pEntry;
					pDM_SWAT_Table->RSSI_target = index;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): RSSI_target is PEER STA\n"));
				}
			}//end if(pMgntInfo->mAssoc && !ACTING_AS_AP(Adapter))

		}
		#endif

		pDM_SWAT_Table->RSSI_cnt_A = 0;
		pDM_SWAT_Table->RSSI_cnt_B = 0;
		pDM_SWAT_Table->try_flag = 0;
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("odm_SwAntDivChkAntSwitch(): Set try_flag to 0 prepare for peak!\n"));
		return;
	}
	else
	{

// To Fix 8723A SW ANT DIV Bug issue
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
		if (pDM_Odm->SupportICType & ODM_RTL8723A)
		{
			curTxOkCnt = Adapter->TxStats.NumTxBytesUnicast - pDM_SWAT_Table->lastTxOkCnt;
			curRxOkCnt =Adapter->RxStats.NumRxBytesUnicast - pDM_SWAT_Table->lastRxOkCnt;
			pDM_SWAT_Table->lastTxOkCnt = Adapter->TxStats.NumTxBytesUnicast;
			pDM_SWAT_Table->lastRxOkCnt = Adapter->RxStats.NumRxBytesUnicast;
		}
		else
		{
			curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast) - pDM_SWAT_Table->lastTxOkCnt;
			curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast) - pDM_SWAT_Table->lastRxOkCnt;
			pDM_SWAT_Table->lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
			pDM_SWAT_Table->lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);
		}
#else	
		curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast) - pDM_SWAT_Table->lastTxOkCnt;
		curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast) - pDM_SWAT_Table->lastRxOkCnt;
		pDM_SWAT_Table->lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
		pDM_SWAT_Table->lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);
#endif	
		if(pDM_SWAT_Table->try_flag == 1)
		{
			if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
			{
				pDM_SWAT_Table->TXByteCnt_A += curTxOkCnt;
				pDM_SWAT_Table->RXByteCnt_A += curRxOkCnt;
			}
			else
			{
				pDM_SWAT_Table->TXByteCnt_B += curTxOkCnt;
				pDM_SWAT_Table->RXByteCnt_B += curRxOkCnt;
			}
		
			nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
			pDM_SWAT_Table->RSSI_Trying--;
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("RSSI_Trying = %d\n",pDM_SWAT_Table->RSSI_Trying));
			if(pDM_SWAT_Table->RSSI_Trying == 0)
			{
				CurByteCnt = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? (pDM_SWAT_Table->TXByteCnt_A+pDM_SWAT_Table->RXByteCnt_A) : (pDM_SWAT_Table->TXByteCnt_B+pDM_SWAT_Table->RXByteCnt_B);
				PreByteCnt = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? (pDM_SWAT_Table->TXByteCnt_B+pDM_SWAT_Table->RXByteCnt_B) : (pDM_SWAT_Table->TXByteCnt_A+pDM_SWAT_Table->RXByteCnt_A);
				
				if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
					//CurByteCnt = PlatformDivision64(CurByteCnt, 9);
					PreByteCnt = PreByteCnt*9;
				else if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_LOW)
					//CurByteCnt = PlatformDivision64(CurByteCnt, 2);
					PreByteCnt = PreByteCnt*2;

				if(pDM_SWAT_Table->RSSI_cnt_A > 0)
					RSSI_A = pDM_SWAT_Table->RSSI_sum_A/pDM_SWAT_Table->RSSI_cnt_A; 
				else
					RSSI_A = 0;
				if(pDM_SWAT_Table->RSSI_cnt_B > 0)
					RSSI_B = pDM_SWAT_Table->RSSI_sum_B/pDM_SWAT_Table->RSSI_cnt_B; 
				else
					RSSI_B = 0;
				curRSSI = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
				pDM_SWAT_Table->PreRSSI =  (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_B : RSSI_A;
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Luke:PreRSSI = %d, CurRSSI = %d\n",pDM_SWAT_Table->PreRSSI, curRSSI));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: preAntenna= %s, curAntenna= %s \n", 
				(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Luke:RSSI_A= %d, RSSI_cnt_A = %d, RSSI_B= %d, RSSI_cnt_B = %d\n",
					RSSI_A, pDM_SWAT_Table->RSSI_cnt_A, RSSI_B, pDM_SWAT_Table->RSSI_cnt_B));
			}

		}
		else
		{
		
			if(pDM_SWAT_Table->RSSI_cnt_A > 0)
				RSSI_A = pDM_SWAT_Table->RSSI_sum_A/pDM_SWAT_Table->RSSI_cnt_A; 
			else
				RSSI_A = 0;
			if(pDM_SWAT_Table->RSSI_cnt_B > 0)
				RSSI_B = pDM_SWAT_Table->RSSI_sum_B/pDM_SWAT_Table->RSSI_cnt_B; 
			else
				RSSI_B = 0;
			curRSSI = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
			pDM_SWAT_Table->PreRSSI =  (pDM_SWAT_Table->PreAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Ekul:PreRSSI = %d, CurRSSI = %d\n", pDM_SWAT_Table->PreRSSI, curRSSI));
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: preAntenna= %s, curAntenna= %s \n", 
			(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));

			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Ekul:RSSI_A= %d, RSSI_cnt_A = %d, RSSI_B= %d, RSSI_cnt_B = %d\n",
				RSSI_A, pDM_SWAT_Table->RSSI_cnt_A, RSSI_B, pDM_SWAT_Table->RSSI_cnt_B));
			//RT_TRACE(COMP_SWAS, DBG_LOUD, ("Ekul:curTxOkCnt = %d\n", curTxOkCnt));
			//RT_TRACE(COMP_SWAS, DBG_LOUD, ("Ekul:curRxOkCnt = %d\n", curRxOkCnt));
		}

		//1 Trying State
		if((pDM_SWAT_Table->try_flag == 1)&&(pDM_SWAT_Table->RSSI_Trying == 0))
		{

			if(pDM_SWAT_Table->TestMode == TP_MODE)
			{
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: TestMode = TP_MODE"));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("TRY:CurByteCnt = %lld,", CurByteCnt));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("TRY:PreByteCnt = %lld\n",PreByteCnt));		
				if(CurByteCnt < PreByteCnt)
				{
					if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
						pDM_SWAT_Table->SelectAntennaMap=pDM_SWAT_Table->SelectAntennaMap<<1;
					else
						pDM_SWAT_Table->SelectAntennaMap=(pDM_SWAT_Table->SelectAntennaMap<<1)+1;
				}
				else
				{
					if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
						pDM_SWAT_Table->SelectAntennaMap=(pDM_SWAT_Table->SelectAntennaMap<<1)+1;
					else
						pDM_SWAT_Table->SelectAntennaMap=pDM_SWAT_Table->SelectAntennaMap<<1;
				}
				for (i= 0; i<8; i++)
				{
					if(((pDM_SWAT_Table->SelectAntennaMap>>i)&BIT0) == 1)
						Score_A++;
					else
						Score_B++;
				}
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SelectAntennaMap=%x\n ",pDM_SWAT_Table->SelectAntennaMap));
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Score_A=%d, Score_B=%d\n", Score_A, Score_B));
			
				if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
				{
					nextAntenna = (Score_A > Score_B)?MAIN_ANT:AUX_ANT;
				}
				else
				{
					nextAntenna = (Score_B > Score_A)?AUX_ANT:MAIN_ANT;
				}
				//RT_TRACE(COMP_SWAS, DBG_LOUD, ("nextAntenna=%s\n",(nextAntenna==Antenna_A)?"A":"B"));
				//RT_TRACE(COMP_SWAS, DBG_LOUD, ("preAntenna= %s, curAntenna= %s \n", 
				//(DM_SWAT_Table.PreAntenna == Antenna_A?"A":"B"), (DM_SWAT_Table.CurAntenna == Antenna_A?"A":"B")));

				if(nextAntenna != pDM_SWAT_Table->CurAntenna)
				{
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: Switch back to another antenna"));
				}
				else
				{
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: current anntena is good\n"));
				}	
			}

			if(pDM_SWAT_Table->TestMode == RSSI_MODE)
			{	
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: TestMode = RSSI_MODE"));
				pDM_SWAT_Table->SelectAntennaMap=0xAA;
				if(curRSSI < pDM_SWAT_Table->PreRSSI) //Current antenna is worse than previous antenna
				{
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: Switch back to another antenna"));
					nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
				}
				else // current anntena is good
				{
					nextAntenna =pDM_SWAT_Table->CurAntenna;
					ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: current anntena is good\n"));
				}
			}
			pDM_SWAT_Table->try_flag = 0;
			pDM_Odm->RSSI_test = FALSE;
			pDM_SWAT_Table->RSSI_sum_A = 0;
			pDM_SWAT_Table->RSSI_cnt_A = 0;
			pDM_SWAT_Table->RSSI_sum_B = 0;
			pDM_SWAT_Table->RSSI_cnt_B = 0;
			pDM_SWAT_Table->TXByteCnt_A = 0;
			pDM_SWAT_Table->TXByteCnt_B = 0;
			pDM_SWAT_Table->RXByteCnt_A = 0;
			pDM_SWAT_Table->RXByteCnt_B = 0;
			
		}

		//1 Normal State
		else if(pDM_SWAT_Table->try_flag == 0)
		{
			if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
			{
				if ((curTxOkCnt+curRxOkCnt) > 3750000)//if(PlatformDivision64(curTxOkCnt+curRxOkCnt, 2) > 1875000)
					pDM_SWAT_Table->TrafficLoad = TRAFFIC_HIGH;
				else
					pDM_SWAT_Table->TrafficLoad = TRAFFIC_LOW;
			}
			else if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_LOW)
				{
				if ((curTxOkCnt+curRxOkCnt) > 3750000) //if(PlatformDivision64(curTxOkCnt+curRxOkCnt, 2) > 1875000)
					pDM_SWAT_Table->TrafficLoad = TRAFFIC_HIGH;
				else
					pDM_SWAT_Table->TrafficLoad = TRAFFIC_LOW;
			}
			if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
				pDM_SWAT_Table->bTriggerAntennaSwitch = 0;
			//RT_TRACE(COMP_SWAS, DBG_LOUD, ("Normal:TrafficLoad = %llu\n", curTxOkCnt+curRxOkCnt));

			//Prepare To Try Antenna		
					nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
					pDM_SWAT_Table->try_flag = 1;
					pDM_Odm->RSSI_test = TRUE;
			if((curRxOkCnt+curTxOkCnt) > 1000)
			{
				pDM_SWAT_Table->RSSI_Trying = 4;
				pDM_SWAT_Table->TestMode = TP_MODE;
				}
				else
				{
				pDM_SWAT_Table->RSSI_Trying = 2;
				pDM_SWAT_Table->TestMode = RSSI_MODE;

			}
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: Normal State -> Begin Trying!\n"));
			
			
			pDM_SWAT_Table->RSSI_sum_A = 0;
			pDM_SWAT_Table->RSSI_cnt_A = 0;
			pDM_SWAT_Table->RSSI_sum_B = 0;
			pDM_SWAT_Table->RSSI_cnt_B = 0;
		}
	}

	//1 4.Change TRX antenna
	if(nextAntenna != pDM_SWAT_Table->CurAntenna)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("SWAS: Change TX Antenna!\n "));
		//PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, 0x300, nextAntenna);		
		#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)			
		ODM_SetAntenna(pDM_Odm,nextAntenna);		
		#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
		{
			BOOLEAN bEnqueue;			
			bEnqueue = (pDM_Odm->SupportInterface ==  ODM_ITRF_PCIE)?FALSE :TRUE;			
			rtw_antenna_select_cmd(pDM_Odm->Adapter, nextAntenna, bEnqueue);						
		}
		#endif
		
	}

	//1 5.Reset Statistics
	pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
	pDM_SWAT_Table->CurAntenna = nextAntenna;
	pDM_SWAT_Table->PreRSSI = curRSSI;

	//1 6.Set next timer
	{
		PADAPTER		pAdapter = pDM_Odm->Adapter;
		HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	

	if(pDM_SWAT_Table->RSSI_Trying == 0)
		return;

	if(pDM_SWAT_Table->RSSI_Trying%2 == 0)
	{
		if(pDM_SWAT_Table->TestMode == TP_MODE)
		{
			if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
			{
				//PlatformSetTimer( pAdapter, &pHalData->SwAntennaSwitchTimer, 10 ); //ms
				ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 10 ); //ms
				
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("dm_SW_AntennaSwitch(): Test another antenna for 10 ms\n"));
			}
			else if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_LOW)
			{
				//PlatformSetTimer( pAdapter, &pHalData->SwAntennaSwitchTimer, 50 ); //ms
				ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 50 ); //ms
				ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("dm_SW_AntennaSwitch(): Test another antenna for 50 ms\n"));
			}
		}
		else
		{
			//PlatformSetTimer( pAdapter, &pHalData->SwAntennaSwitchTimer, 500 ); //ms
			ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 500 ); //ms
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("dm_SW_AntennaSwitch(): Test another antenna for 500 ms\n"));
		}
	}
	else
	{
		if(pDM_SWAT_Table->TestMode == TP_MODE)
		{
			if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_HIGH)
				//PlatformSetTimer( pAdapter, &pHalData->SwAntennaSwitchTimer, 90 ); //ms
				ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 90 ); //ms
			else if(pDM_SWAT_Table->TrafficLoad == TRAFFIC_LOW)
				//PlatformSetTimer( pAdapter, &pHalData->SwAntennaSwitchTimer, 100 ); //ms
				ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 100 ); //ms
		}
		else
			//PlatformSetTimer( pAdapter, &pHalData->SwAntennaSwitchTimer, 500 ); //ms
			ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 100 ); //ms 
	}
	}
#endif	// #if (DM_ODM_SUPPORT_TYPE  & (ODM_WIN|ODM_CE))
#endif	// #if (RTL8192C_SUPPORT==1) 
}


#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

u1Byte
odm_SwAntDivSelectChkChnl(
	IN	PADAPTER	Adapter
	)
{
#if (RT_MEM_SIZE_LEVEL != RT_MEM_SIZE_MINIMUM)
	u1Byte 	index, target_chnl=0;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T			pDM_Odm = &pHalData->DM_OutSrc;
	u1Byte	chnl_peer_cnt[14] = {0};

	if(Adapter->MgntInfo.tmpNumBssDesc==0)
	{
		return 0;
	}
	else
	{		
		// 20100519 Joseph: Select checking channel from current scan list.
		// We just choose the channel with most APs to be the test scan channel.
		for(index=0; index<Adapter->MgntInfo.tmpNumBssDesc; index++)
		{
			// Add by hpfan: prevent access invalid channel number
			// TODO: Verify channel number by channel plan
			if(Adapter->MgntInfo.tmpbssDesc[index].ChannelNumber == 0 || 
				Adapter->MgntInfo.tmpbssDesc[index].ChannelNumber > 13)
				continue;
			
			chnl_peer_cnt[Adapter->MgntInfo.tmpbssDesc[index].ChannelNumber-1]++;
		}
		for(index=0; index<14; index++)
		{
			if(chnl_peer_cnt[index]>chnl_peer_cnt[target_chnl])
				target_chnl = index;
		}
		target_chnl+=1;
		ODM_RT_TRACE(pDM_Odm,COMP_SWAS, DBG_LOUD, 
			("odm_SwAntDivSelectChkChnl(): Channel %d is select as test channel.\n", target_chnl));

		return target_chnl;
	}
#else
	return	0;
#endif	
}


VOID
odm_SwAntDivConsructChkScanChnl(
	IN	PADAPTER	Adapter,
	IN	u1Byte		ChkChnl
	)
{

	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	PRT_CHANNEL_LIST	pChannelList = GET_RT_CHANNEL_LIST(pMgntInfo);
	u1Byte				index;

	if(ChkChnl==0)
	{
		// 20100519 Joseph: Original antenna scanned nothing. 
		// Test antenna shall scan all channel with half period in this condition.
		RtActChannelList(Adapter, RT_CHNL_LIST_ACTION_CONSTRUCT_SCAN_LIST, NULL, NULL);
		for(index=0; index<pChannelList->ChannelLen; index++)
			pChannelList->ChannelInfo[index].ScanPeriod /= 2;
	}
	else
	{
		// The using of this CustomizedScanRequest is a trick to rescan the two channels 
		//	under the NORMAL scanning process. It will not affect MGNT_INFO.CustomizedScanRequest.
		CUSTOMIZED_SCAN_REQUEST CustomScanReq;

		CustomScanReq.bEnabled = TRUE;
		CustomScanReq.Channels[0] = ChkChnl;
		CustomScanReq.Channels[1] = pMgntInfo->dot11CurrentChannelNumber;
		CustomScanReq.nChannels = 2;
		CustomScanReq.ScanType = SCAN_ACTIVE;
		CustomScanReq.Duration = DEFAULT_ACTIVE_SCAN_PERIOD;

		RtActChannelList(Adapter, RT_CHNL_LIST_ACTION_CONSTRUCT_SCAN_LIST, &CustomScanReq, NULL);
	}

}
#endif //#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

//
// 20100514 Luke/Joseph:
// Callback function for 500ms antenna test trying.
//
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
odm_SwAntDivChkAntSwitchCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	pSWAT_T		pDM_SWAT_Table = &pHalData->DM_OutSrc.DM_SWAT_Table;

	#if DEV_BUS_TYPE==RT_PCI_INTERFACE
	#if USE_WORKITEM
	ODM_ScheduleWorkItem(&pDM_SWAT_Table->SwAntennaSwitchWorkitem);
	#else
	odm_SwAntDivChkAntSwitch(&pHalData->DM_OutSrc, SWAW_STEP_DETERMINE);
	#endif
	#else
	ODM_ScheduleWorkItem(&pDM_SWAT_Table->SwAntennaSwitchWorkitem);
	#endif
	
}
VOID
odm_SwAntDivChkAntSwitchWorkitemCallback(
    IN PVOID            pContext
    )
{

	PADAPTER		pAdapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);

	odm_SwAntDivChkAntSwitch(&pHalData->DM_OutSrc, SWAW_STEP_DETERMINE);

}
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext)
{
	PDM_ODM_T	pDM_Odm= (PDM_ODM_T)FunctionContext;
	PADAPTER	padapter = pDM_Odm->Adapter;
	if(padapter->net_closed == _TRUE)
	    return;
	odm_SwAntDivChkAntSwitch(pDM_Odm, SWAW_STEP_DETERMINE);	
}
#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext)
{
	PDM_ODM_T	pDM_Odm= (PDM_ODM_T)FunctionContext;
	odm_SwAntDivChkAntSwitch(pDM_Odm, SWAW_STEP_DETERMINE);
}
#endif

#else //#if(defined(CONFIG_SW_ANTENNA_DIVERSITY))

VOID odm_SwAntDivInit(	IN		PDM_ODM_T		pDM_Odm	) {}
VOID ODM_SwAntDivChkPerPktRssi(
	IN PDM_ODM_T	pDM_Odm,
	IN u1Byte		StationID,
	IN PODM_PHY_INFO_T pPhyInfo
	) {}
VOID odm_SwAntDivChkAntSwitch(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			Step
	) {}
VOID ODM_SwAntDivResetBeforeLink(	IN		PDM_ODM_T		pDM_Odm	){}
VOID ODM_SwAntDivRestAfterLink(	IN		PDM_ODM_T		pDM_Odm	){}
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
u1Byte odm_SwAntDivSelectChkChnl(	IN	PADAPTER	Adapter	){	return 0;}
VOID
odm_SwAntDivConsructChkScanChnl(
	IN	PADAPTER	Adapter,
	IN	u1Byte		ChkChnl
	){}
#endif
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID odm_SwAntDivChkAntSwitchCallback(	PRT_TIMER		pTimer){}
VOID odm_SwAntDivChkAntSwitchWorkitemCallback(    IN PVOID            pContext    ){}
#elif (DM_ODM_SUPPORT_TYPE == ODM_CE)
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext){}
#elif (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
VOID odm_SwAntDivChkAntSwitchCallback(void *FunctionContext){}
#endif

#endif //#if(defined(CONFIG_SW_ANTENNA_DIVERSITY))

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
#if((defined(CONFIG_SW_ANTENNA_DIVERSITY))||(defined(CONFIG_HW_ANTENNA_DIVERSITY)))
BOOLEAN
ODM_SwAntDivCheckBeforeLink8192C(
	IN		PDM_ODM_T		pDM_Odm
	)
{

#if (RT_MEM_SIZE_LEVEL != RT_MEM_SIZE_MINIMUM)
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE*	pHalData = NULL;
	PMGNT_INFO		pMgntInfo = NULL;
	//pSWAT_T			pDM_SWAT_Table = &Adapter->DM_SWAT_Table;
	pSWAT_T			pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	pFAT_T	pDM_FatTable = &pDM_Odm->DM_FatTable;

	s1Byte			Score = 0;
	PRT_WLAN_BSS	pTmpBssDesc;
	PRT_WLAN_BSS	pTestBssDesc;

	u1Byte			target_chnl = 0;
	u1Byte			index;

return FALSE;
	if (pDM_Odm->Adapter == NULL)  //For BSOD when plug/unplug fast.  //By YJ,120413
	{	// The ODM structure is not initialized.
		return FALSE;
	}
	// 2012/04/26 MH Prevent no-checked IC to execute antenna diversity.
	if(pDM_Odm->SupportICType == ODM_RTL8188E && pDM_Odm->SupportInterface != ODM_ITRF_PCIE)
		return FALSE;
	pHalData = GET_HAL_DATA(Adapter);
	pMgntInfo = &Adapter->MgntInfo;
	
	// Condition that does not need to use antenna diversity.
	if(IS_8723_SERIES(pHalData->VersionID) ||
		IS_92C_SERIAL(pHalData->VersionID) ||
		(pHalData->AntDivCfg==0) || 
		pMgntInfo->AntennaTest ||
		Adapter->bInHctTest)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
				("ODM_SwAntDivCheckBeforeLink8192C(): No AntDiv Mechanism.\n"));
		return FALSE;
	}

	if(IS_8723_SERIES(pHalData->VersionID) || IS_92C_SERIAL(pHalData->VersionID) )
	{
		if((pDM_SWAT_Table->ANTA_ON == FALSE) ||(pDM_SWAT_Table->ANTB_ON == FALSE))
		{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,
					("ODM_SwAntDivCheckBeforeLink8192C(): No AntDiv Mechanism, Antenna A or B is off\n"));
			return FALSE;
		}
	}

	// Since driver is going to set BB register, it shall check if there is another thread controlling BB/RF.
	PlatformAcquireSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	if(pHalData->eRFPowerState!=eRfOn || pMgntInfo->RFChangeInProgress || pMgntInfo->bMediaConnect)
	{
		PlatformReleaseSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
				("ODM_SwAntDivCheckBeforeLink8192C(): RFChangeInProgress(%x), eRFPowerState(%x)\n", 
				pMgntInfo->RFChangeInProgress,
				pHalData->eRFPowerState));
	
		pDM_SWAT_Table->SWAS_NoLink_State = 0;
		
		return FALSE;
	}
	else
	{
		PlatformReleaseSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	}

	//1 Run AntDiv mechanism "Before Link" part.
	if(pDM_SWAT_Table->SWAS_NoLink_State == 0)
	{
		//1 Prepare to do Scan again to check current antenna state.

		// Set check state to next step.
		pDM_SWAT_Table->SWAS_NoLink_State = 1;
	
		// Copy Current Scan list.
		Adapter->MgntInfo.tmpNumBssDesc = pMgntInfo->NumBssDesc;
		PlatformMoveMemory((PVOID)Adapter->MgntInfo.tmpbssDesc, (PVOID)pMgntInfo->bssDesc, sizeof(RT_WLAN_BSS)*MAX_BSS_DESC);
		
		if(pDM_Odm->SupportICType == ODM_RTL8188E)
		{
			if(pDM_FatTable->RxIdleAnt == MAIN_ANT)
				ODM_UpdateRxIdleAnt_88E(pDM_Odm, AUX_ANT);
			else
				ODM_UpdateRxIdleAnt_88E(pDM_Odm, MAIN_ANT);

			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
				("ODM_SwAntDivCheckBeforeLink8192C: Change to %s for testing.\n", ((pDM_FatTable->RxIdleAnt == MAIN_ANT)?"MAIN_ANT":"AUX_ANT")));
		}
		if(pDM_Odm->SupportICType != ODM_RTL8188E)
		{
		// Switch Antenna to another one.
		pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
		pDM_SWAT_Table->CurAntenna = (pDM_SWAT_Table->CurAntenna==MAIN_ANT)?AUX_ANT:MAIN_ANT;
		
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
			("ODM_SwAntDivCheckBeforeLink8192C: Change to Ant(%s) for testing.\n", (pDM_SWAT_Table->CurAntenna==MAIN_ANT)?"MAIN":"AUX"));
		//PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, 0x300, DM_SWAT_Table.CurAntenna);
		pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 & 0xfffffcff) | (pDM_SWAT_Table->CurAntenna<<8));
			ODM_SetBBReg(pDM_Odm,  rFPGA0_XA_RFInterfaceOE, bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg860);
		}
		// Go back to scan function again.
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink8192C: Scan one more time\n"));
		pMgntInfo->ScanStep=0;
		target_chnl = odm_SwAntDivSelectChkChnl(Adapter);
		odm_SwAntDivConsructChkScanChnl(Adapter, target_chnl);
		HTReleaseChnlOpLock(Adapter);
		PlatformSetTimer(Adapter, &pMgntInfo->ScanTimer, 5);

		return TRUE;
	}
	else
	{
		//1 ScanComple() is called after antenna swiched.
		//1 Check scan result and determine which antenna is going
		//1 to be used.

		for(index=0; index<Adapter->MgntInfo.tmpNumBssDesc; index++)
		{
			pTmpBssDesc = &(Adapter->MgntInfo.tmpbssDesc[index]);
			pTestBssDesc = &(pMgntInfo->bssDesc[index]);

			if(PlatformCompareMemory(pTestBssDesc->bdBssIdBuf, pTmpBssDesc->bdBssIdBuf, 6)!=0)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink8192C(): ERROR!! This shall not happen.\n"));
				continue;
			}

			if(pTmpBssDesc->RecvSignalPower > pTestBssDesc->RecvSignalPower)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink8192C: Compare scan entry: Score++\n"));
				RT_PRINT_STR(ODM_COMP_ANT_DIV, ODM_DBG_LOUD, "SSID: ", pTestBssDesc->bdSsIdBuf, pTestBssDesc->bdSsIdLen);
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
			
				Score++;
				PlatformMoveMemory(pTestBssDesc, pTmpBssDesc, sizeof(RT_WLAN_BSS));
			}
			else if(pTmpBssDesc->RecvSignalPower < pTestBssDesc->RecvSignalPower)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SwAntDivCheckBeforeLink8192C: Compare scan entry: Score--\n"));
				RT_PRINT_STR(ODM_COMP_ANT_DIV, ODM_DBG_LOUD, "SSID: ", pTestBssDesc->bdSsIdBuf, pTestBssDesc->bdSsIdLen);
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
				Score--;
			}

		}

		if(pDM_Odm->SupportICType == ODM_RTL8188E)
		{
		if(pMgntInfo->NumBssDesc!=0 && Score<=0)
		{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,
					("ODM_SwAntDivCheckBeforeLink8192C(): Using Ant(%s)\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"MAIN_ANT":"AUX_ANT"));
		}
		else
		{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
					("ODM_SwAntDivCheckBeforeLink8192C(): Remain Ant(%s)\n", (pDM_FatTable->RxIdleAnt==MAIN_ANT)?"AUX_ANT":"MAIN_ANT"));

				if(pDM_FatTable->RxIdleAnt == MAIN_ANT)
					ODM_UpdateRxIdleAnt_88E(pDM_Odm, AUX_ANT);
				else
					ODM_UpdateRxIdleAnt_88E(pDM_Odm, MAIN_ANT);
			}
		}

		if(pDM_Odm->SupportICType != ODM_RTL8188E)
				{
			if(pMgntInfo->NumBssDesc!=0 && Score<=0)
				{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,
					("ODM_SwAntDivCheckBeforeLink8192C(): Using Ant(%s)\n", (pDM_SWAT_Table->CurAntenna==MAIN_ANT)?"MAIN":"AUX"));

				pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
			}
			else
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, 
					("ODM_SwAntDivCheckBeforeLink8192C(): Remain Ant(%s)\n", (pDM_SWAT_Table->CurAntenna==MAIN_ANT)?"AUX":"MAIN"));

				pDM_SWAT_Table->CurAntenna = pDM_SWAT_Table->PreAntenna;

				//PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, 0x300, DM_SWAT_Table.CurAntenna);
			pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 & 0xfffffcff) | (pDM_SWAT_Table->CurAntenna<<8));
			PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg860);
		}
		}
		// Check state reset to default and wait for next time.
		pDM_SWAT_Table->SWAS_NoLink_State = 0;

		return FALSE;
	}
#else
		return	FALSE;
#endif

return FALSE;
}
#else
BOOLEAN
ODM_SwAntDivCheckBeforeLink8192C(
	IN		PDM_ODM_T		pDM_Odm
	)
{

	return FALSE;

}
#endif //#if((defined(CONFIG_SW_ANTENNA_DIVERSITY))||(defined(CONFIG_HW_ANTENNA_DIVERSITY)))
#endif //#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)


//3============================================================
//3 SW Antenna Diversity
//3============================================================

#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))
VOID
odm_InitHybridAntDiv_88C_92D(
	IN PDM_ODM_T	pDM_Odm 
	)
{

#if((DM_ODM_SUPPORT_TYPE==ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))
	struct rtl8192cd_priv *priv=pDM_Odm->priv;
#endif
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u1Byte                  bTxPathSel=0;	        //0:Path-A   1:Path-B
	u1Byte			i;

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_InitHybridAntDiv==============>\n"));

	//whether to do antenna diversity or not
#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	if(priv==NULL)	return;
	if(!priv->pshare->rf_ft_var.antHw_enable)
		return;	
	
	#ifdef SW_ANT_SWITCH
	priv->pshare->rf_ft_var.antSw_enable =0;
	#endif
#endif

	if((pDM_Odm->SupportICType != ODM_RTL8192C) && (pDM_Odm->SupportICType != ODM_RTL8192D))
		return;


	bTxPathSel=(pDM_Odm->RFType==ODM_1T1R)?FALSE:TRUE;

	ODM_SetBBReg(pDM_Odm,ODM_REG_BB_PWR_SAV1_11N, BIT23, 0); //No update ANTSEL during GNT_BT=1
	ODM_SetBBReg(pDM_Odm,ODM_REG_TX_ANT_CTRL_11N, BIT21, 1); //TX atenna selection from tx_info
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTSEL_PIN_11N, BIT23, 1);	//enable LED[1:0] pin as ANTSEL
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTSEL_CTRL_11N, BIT8|BIT9, 0x01);	// 0x01: left antenna, 0x02: right antenna
	// check HW setting: ANTSEL pin connection
	#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	ODM_Write2Byte(pDM_Odm,ODM_REG_RF_PIN_11N, (ODM_Read2Byte(pDM_Odm,0x804)&0xf0ff )| BIT(8) );	// b11-b8=0001,update RFPin setting
	#endif
	
	// only AP support different path selection temperarly
	if(!bTxPathSel){                 //PATH-A
		ODM_SetBBReg(pDM_Odm,ODM_REG_PIN_CTRL_11N, BIT8|BIT9, 0 ); // ANTSEL as HW control
		ODM_SetBBReg(pDM_Odm,ODM_REG_ANTSEL_PATH_11N, BIT13, 1);	 //select TX ANTESEL from path A
	}
	else	{
		ODM_SetBBReg(pDM_Odm,ODM_REG_PIN_CTRL_11N, BIT24|BIT25, 0 ); // ANTSEL as HW control
		ODM_SetBBReg(pDM_Odm,ODM_REG_ANTSEL_PATH_11N, BIT13, 0);		 //select ANTESEL from path B
	}

	//Set OFDM HW RX Antenna Diversity
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTDIV_PARA1_11N, 0x7FF, 0x0c0); //Pwdb threshold=8dB
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTDIV_PARA1_11N, BIT11, 0); //Switch to another antenna by checking pwdb threshold
	ODM_SetBBReg(pDM_Odm,ODM_REG_ANTDIV_PARA3_11N, BIT23, 1);	// Decide final antenna by comparing 2 antennas' pwdb
	
	//Set CCK HW RX Antenna Diversity
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA2_11N, BIT4, 0); //Antenna diversity decision period = 32 sample
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA2_11N, 0xf, 0xf); //Threshold for antenna diversity. Check another antenna power if input power < ANT_lim*4
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA3_11N, BIT13, 1); //polarity ana_A=1 and ana_B=0
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA4_11N, 0x1f, 0x8); //default antenna power = inpwr*(0.5 + r_ant_step/16)


	//Enable HW Antenna Diversity
	if(!bTxPathSel)                 //PATH-A
		ODM_SetBBReg(pDM_Odm,ODM_REG_IGI_A_11N, BIT7,1);	// Enable Hardware antenna switch
	else
		ODM_SetBBReg(pDM_Odm,ODM_REG_IGI_B_11N, BIT7,1);	// Enable Hardware antenna switch
	ODM_SetBBReg(pDM_Odm,ODM_REG_CCK_ANTDIV_PARA1_11N, BIT15, 1);//Enable antenna diversity

	pDM_SWAT_Table->CurAntenna=0;			//choose left antenna as default antenna
	pDM_SWAT_Table->PreAntenna=0;
	for(i=0; i<ASSOCIATE_ENTRY_NUM ; i++)
	{
		pDM_SWAT_Table->CCK_Ant1_Cnt[i] = 0;
		pDM_SWAT_Table->CCK_Ant2_Cnt[i] = 0;
		pDM_SWAT_Table->OFDM_Ant1_Cnt[i] = 0;
		pDM_SWAT_Table->OFDM_Ant2_Cnt[i] = 0;
		pDM_SWAT_Table->RSSI_Ant1_Sum[i] = 0;
		pDM_SWAT_Table->RSSI_Ant2_Sum[i] = 0;
	}
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("<==============odm_InitHybridAntDiv\n"));
}


VOID
odm_InitHybridAntDiv(
	IN PDM_ODM_T	pDM_Odm 
	)
{
	if(!(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("Return: Not Support HW AntDiv\n"));
		return;
	}
	
	if(pDM_Odm->SupportICType & (ODM_RTL8192C | ODM_RTL8192D))
	{
#if ((RTL8192C_SUPPORT == 1)||(RTL8192D_SUPPORT == 1))
		odm_InitHybridAntDiv_88C_92D(pDM_Odm);
#endif
	}
	
}


BOOLEAN
odm_StaDefAntSel(
	IN PDM_ODM_T	pDM_Odm,
	IN u4Byte		OFDM_Ant1_Cnt,
	IN u4Byte		OFDM_Ant2_Cnt,
	IN u4Byte		CCK_Ant1_Cnt,
	IN u4Byte		CCK_Ant2_Cnt,
	OUT u1Byte		*pDefAnt 

	)
{
#if 1
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_StaDefAntSelect==============>\n"));

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("OFDM_Ant1_Cnt:%d, OFDM_Ant2_Cnt:%d\n",OFDM_Ant1_Cnt,OFDM_Ant2_Cnt));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("CCK_Ant1_Cnt:%d, CCK_Ant2_Cnt:%d\n",CCK_Ant1_Cnt,CCK_Ant2_Cnt));

	
	if(((OFDM_Ant1_Cnt+OFDM_Ant2_Cnt)==0)&&((CCK_Ant1_Cnt + CCK_Ant2_Cnt) <10)){
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_StaDefAntSelect Fail: No enough packet info!\n"));
		return	FALSE;
	}

	if(OFDM_Ant1_Cnt || OFDM_Ant2_Cnt )	{
		//if RX OFDM packet number larger than 0
		if(OFDM_Ant1_Cnt > OFDM_Ant2_Cnt)
			(*pDefAnt)=1;
		else
			(*pDefAnt)=0;
	}
	// else if RX CCK packet number larger than 10
	else if((CCK_Ant1_Cnt + CCK_Ant2_Cnt) >=10 )
	{
		if(CCK_Ant1_Cnt > (5*CCK_Ant2_Cnt))
			(*pDefAnt)=1;
		else if(CCK_Ant2_Cnt > (5*CCK_Ant1_Cnt))
			(*pDefAnt)=0;
		else if(CCK_Ant1_Cnt > CCK_Ant2_Cnt)
			(*pDefAnt)=0;
		else
			(*pDefAnt)=1;

	}

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("TxAnt = %s\n",((*pDefAnt)==1)?"Ant1":"Ant2"));
	
#endif
	//u4Byte antsel = ODM_GetBBReg(pDM_Odm, 0xc88, bMaskByte0);
	//(*pDefAnt)= (u1Byte) antsel;
	


	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("<==============odm_StaDefAntSelect\n"));

	return TRUE;

	
}


VOID
odm_SetRxIdleAnt(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte	Ant,
	IN   BOOLEAN   bDualPath                     
)
{
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	//ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_SetRxIdleAnt==============>\n"));

	if(Ant != pDM_SWAT_Table->RxIdleAnt)
	{
	//for path-A
	if(Ant==1) 
			ODM_SetBBReg(pDM_Odm,ODM_REG_RX_DEFUALT_A_11N, 0xFFFF, 0x65a9);   //right-side antenna
	else
			ODM_SetBBReg(pDM_Odm,ODM_REG_RX_DEFUALT_A_11N, 0xFFFF, 0x569a);   //left-side antenna

	//for path-B
	if(bDualPath){
			if(Ant==0) 
				ODM_SetBBReg(pDM_Odm,ODM_REG_RX_DEFUALT_A_11N, 0xFFFF0000, 0x65a9);   //right-side antenna
		else 
				ODM_SetBBReg(pDM_Odm,ODM_REG_RX_DEFUALT_A_11N, 0xFFFF0000, 0x569a);  //left-side antenna
		}
	}
		pDM_SWAT_Table->RxIdleAnt = Ant;
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("RxIdleAnt: %s  Reg858=0x%x\n",(Ant==1)?"Ant1":"Ant2",(Ant==1)?0x65a9:0x569a));

	//ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("<==============odm_SetRxIdleAnt\n"));

	}
		
VOID
ODM_AntselStatistics_88C(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			MacId,
	IN		u4Byte			PWDBAll,
	IN		BOOLEAN			isCCKrate
)
{
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	if(pDM_SWAT_Table->antsel == 1)
	{
		if(isCCKrate)
			pDM_SWAT_Table->CCK_Ant1_Cnt[MacId]++;
		else
		{
			pDM_SWAT_Table->OFDM_Ant1_Cnt[MacId]++;
			pDM_SWAT_Table->RSSI_Ant1_Sum[MacId] += PWDBAll;
		}
	}
	else
	{
		if(isCCKrate)
			pDM_SWAT_Table->CCK_Ant2_Cnt[MacId]++;
		else
		{
			pDM_SWAT_Table->OFDM_Ant2_Cnt[MacId]++;
			pDM_SWAT_Table->RSSI_Ant2_Sum[MacId] += PWDBAll;
		}
	}

}




#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
VOID
ODM_SetTxAntByTxInfo_88C_92D(
	IN		PDM_ODM_T		pDM_Odm,
	IN		pu1Byte			pDesc,
	IN		u1Byte			macId	
)
{
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u1Byte			antsel;

	if(!(pDM_Odm->SupportAbility&ODM_BB_ANT_DIV)) 
		return;

	if(pDM_SWAT_Table->RxIdleAnt == 1)
		antsel=(pDM_SWAT_Table->TxAnt[macId] == 1)?0:1;
	else
		antsel=(pDM_SWAT_Table->TxAnt[macId] == 1)?1:0;
	
	SET_TX_DESC_ANTSEL_A_92C(pDesc, antsel);
	//SET_TX_DESC_ANTSEL_B_92C(pDesc, antsel);
	//ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("SET_TX_DESC_ANTSEL_A_92C=%d\n", pDM_SWAT_Table->TxAnt[macId]));
}
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
VOID
ODM_SetTxAntByTxInfo_88C_92D(
	IN		PDM_ODM_T		pDM_Odm
)
{

}
#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
VOID
ODM_SetTxAntByTxInfo_88C_92D(
	IN		PDM_ODM_T		pDM_Odm
)
{

}
#endif

#if ((RTL8192C_SUPPORT == 1)||(RTL8192D_SUPPORT == 1))
VOID
odm_HwAntDiv_92C_92D(
	IN	PDM_ODM_T	pDM_Odm
)
{
	SWAT_T			*pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u4Byte			RSSI_Min=0xFF, RSSI, RSSI_Ant1, RSSI_Ant2, i;
	u1Byte			RxIdleAnt;
	BOOLEAN		bRet=FALSE;
	PSTA_INFO_T   	pEntry;
	struct stat_info	*pstat;	
	struct list_head *phead, *plist;
	
#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
	struct rtl8192cd_priv *priv=pDM_Odm->priv;
	//if test, return
	if(priv->pshare->rf_ft_var.CurAntenna & 0x80)
		return;	
#endif	

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_HwAntDiv==============>\n"));
	
	if(!(pDM_Odm->SupportAbility&ODM_BB_ANT_DIV))                                    //if don't support antenna diveristy
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_HwAntDiv: Not supported!\n"));
		return;
	}

	if((pDM_Odm->SupportICType != ODM_RTL8192C) && (pDM_Odm->SupportICType != ODM_RTL8192D))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("Return: IC Type is not 92C or 92D\n"));
		return;
	}
	
#if (DM_ODM_SUPPORT_TYPE&(ODM_WIN|ODM_CE))
	if(!pDM_Odm->bLinked)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("Return: bLinked is FALSE\n"));
		return;
	}
#endif

	for (i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		pEntry = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pEntry))
		{

			RSSI_Ant1 = (pDM_SWAT_Table->OFDM_Ant1_Cnt[i] == 0)?0:(pDM_SWAT_Table->RSSI_Ant1_Sum[i]/pDM_SWAT_Table->OFDM_Ant1_Cnt[i]);
			RSSI_Ant2 = (pDM_SWAT_Table->OFDM_Ant2_Cnt[i] == 0)?0:(pDM_SWAT_Table->RSSI_Ant2_Sum[i]/pDM_SWAT_Table->OFDM_Ant2_Cnt[i]);

			ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("RSSI_Ant1=%d,  RSSI_Ant2=%d\n", RSSI_Ant1, RSSI_Ant2));
		
			if(RSSI_Ant1 ||RSSI_Ant2) 
			{
#if (DM_ODM_SUPPORT_TYPE==ODM_AP)		
				if(pDM_Odm->pODM_StaInfo[i]->expire_to)
#endif
				{
					RSSI = (RSSI_Ant1 < RSSI_Ant2) ? RSSI_Ant1 : RSSI_Ant2;
					if((!RSSI) || ( RSSI < RSSI_Min) ) {
						pDM_SWAT_Table->TargetSTA = i;
						RSSI_Min = RSSI;
					}
				}
	}
			///STA: found out default antenna
			bRet=odm_StaDefAntSel(pDM_Odm, 
						 pDM_SWAT_Table->OFDM_Ant1_Cnt[i], 
						 pDM_SWAT_Table->OFDM_Ant2_Cnt[i], 
						 pDM_SWAT_Table->CCK_Ant1_Cnt[i], 
						 pDM_SWAT_Table->CCK_Ant2_Cnt[i], 
						 &pDM_SWAT_Table->TxAnt[i]);

			//if Tx antenna selection: successful
			if(bRet){	
				pDM_SWAT_Table->RSSI_Ant1_Sum[i] = 0;
				pDM_SWAT_Table->RSSI_Ant2_Sum[i] = 0;
				pDM_SWAT_Table->OFDM_Ant1_Cnt[i] = 0;
				pDM_SWAT_Table->OFDM_Ant2_Cnt[i] = 0; 
				pDM_SWAT_Table->CCK_Ant1_Cnt[i] = 0; 
				pDM_SWAT_Table->CCK_Ant2_Cnt[i] = 0; 
			}
		}
	}
	
	//set RX Idle Ant
	RxIdleAnt = pDM_SWAT_Table->TxAnt[pDM_SWAT_Table->TargetSTA];
	odm_SetRxIdleAnt(pDM_Odm, RxIdleAnt, FALSE);

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
#ifdef TX_SHORTCUT
	if (!priv->pmib->dot11OperationEntry.disable_txsc) {
		plist = phead->next;
		while(plist != phead)	{
			pstat = list_entry(plist, struct stat_info, asoc_list);
			if(pstat->expire_to) {
				for (i=0; i<TX_SC_ENTRY_NUM; i++) {
					struct tx_desc *pdesc= &(pstat->tx_sc_ent[i].hwdesc1);	
					pdesc->Dword2 &= set_desc(~ (BIT(24)|BIT(25)));
					if((pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)
						pdesc->Dword2 |= set_desc(BIT(24)|BIT(25));
					pdesc= &(pstat->tx_sc_ent[i].hwdesc2);	
					pdesc->Dword2 &= set_desc(~ (BIT(24)|BIT(25)));
					if((pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)
						pdesc->Dword2 |= set_desc(BIT(24)|BIT(25));					
				}
			}		

			if (plist == plist->next)
				break;
			plist = plist->next;
		};
	}
#endif	
#endif
	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("<==============odm_HwAntDiv\n"));
	
}
#endif

VOID
odm_HwAntDiv(
	IN	PDM_ODM_T	pDM_Odm
)
{	
	if(!(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("Return: Not Support HW AntDiv\n"));
		return;
	}
	
	if(pDM_Odm->SupportICType & (ODM_RTL8192C | ODM_RTL8192D))
	{
#if ((RTL8192C_SUPPORT == 1)||(RTL8192D_SUPPORT == 1))
		odm_HwAntDiv_92C_92D(pDM_Odm);
#endif
	}
}


#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
#if 0
VOID
odm_HwAntDiv(
	IN	PDM_ODM_T	pDM_Odm
)
{
	struct rtl8192cd_priv *priv=pDM_Odm->priv;
	struct stat_info	*pstat, *pstat_min=NULL;
	struct list_head	*phead, *plist;
	int rssi_min= 0xff, i;
	u1Byte	idleAnt=priv->pshare->rf_ft_var.CurAntenna;	
	u1Byte	nextAnt;
	BOOLEAN		bRet=FALSE;
	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("odm_HwAntDiv==============>\n"));

	if((!priv->pshare->rf_ft_var.antHw_enable) ||(!(pDM_Odm->SupportAbility & ODM_BB_ANT_DIV)))
		return;
	
	//if test, return
	if(priv->pshare->rf_ft_var.CurAntenna & 0x80)
		return;
	
	phead = &priv->asoc_list;
	plist = phead->next;
	////=========================
	//find mimum rssi sta
	////=========================
	while(plist != phead)	{
		pstat = list_entry(plist, struct stat_info, asoc_list);
		if((pstat->expire_to) && (pstat->AntRSSI[0] || pstat->AntRSSI[1])) {
			int rssi = (pstat->AntRSSI[0] < pstat->AntRSSI[1]) ? pstat->AntRSSI[0] : pstat->AntRSSI[1];
			if((!pstat_min) || ( rssi < rssi_min) ) {
				pstat_min = pstat;
				rssi_min = rssi;
			}
		}
		///STA: found out default antenna
		bRet=odm_StaDefAntSel(pDM_Odm,
						pstat->hwRxAntSel[1],
						pstat->hwRxAntSel[0],
						pstat->cckPktCount[1],
						pstat->cckPktCount[0],
						&nextAnt
						);
		
		//if default antenna selection: successful
		if(bRet){	
			pstat->CurAntenna = nextAnt;
			//update rssi
			for(i=0; i<2; i++) {
				if(pstat->cckPktCount[i]==0 && pstat->hwRxAntSel[i]==0)
					pstat->AntRSSI[i] = 0;
			}
			if(pstat->AntRSSI[idleAnt]==0)
				pstat->AntRSSI[idleAnt] = pstat->AntRSSI[idleAnt^1];
			// reset variables
			pstat->hwRxAntSel[1] = pstat->hwRxAntSel[0] =0;
			pstat->cckPktCount[1]= pstat->cckPktCount[0] =0;
		}

		if (plist == plist->next)
			break;
		plist = plist->next;
		
	};
	////=========================
	//Choose  RX Idle antenna according to minmum rssi
	////=========================
	if(pstat_min)	{
		if(priv->pshare->rf_ft_var.CurAntenna!=pstat_min->CurAntenna)
			odm_SetRxIdleAnt(pDM_Odm,pstat_min->CurAntenna,TRUE);
		priv->pshare->rf_ft_var.CurAntenna = pstat_min->CurAntenna;
	}


#ifdef TX_SHORTCUT
	if (!priv->pmib->dot11OperationEntry.disable_txsc) {
		plist = phead->next;
		while(plist != phead)	{
			pstat = list_entry(plist, struct stat_info, asoc_list);
			if(pstat->expire_to) {
				for (i=0; i<TX_SC_ENTRY_NUM; i++) {
					struct tx_desc *pdesc= &(pstat->tx_sc_ent[i].hwdesc1);	
					pdesc->Dword2 &= set_desc(~ (BIT(24)|BIT(25)));
					if((pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)
						pdesc->Dword2 |= set_desc(BIT(24)|BIT(25));
					pdesc= &(pstat->tx_sc_ent[i].hwdesc2);	
					pdesc->Dword2 &= set_desc(~ (BIT(24)|BIT(25)));
					if((pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)
						pdesc->Dword2 |= set_desc(BIT(24)|BIT(25));					
				}
			}		

			if (plist == plist->next)
				break;
			plist = plist->next;
		};
	}
#endif	
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV, ODM_DBG_LOUD,"<==============odm_HwAntDiv\n");
}
#endif

u1Byte
ODM_Diversity_AntennaSelect(
	IN	PDM_ODM_T	pDM_Odm,
	IN	u1Byte	*data
)
{
	struct rtl8192cd_priv *priv=pDM_Odm->priv;

	int ant = _atoi(data, 16);

	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("ODM_Diversity_AntennaSelect==============>\n"));

	#ifdef PCIE_POWER_SAVING
	PCIeWakeUp(priv, POWER_DOWN_T0);
	#endif

	if (ant==AUX_ANT || ant==MAIN_ANT) 
	{
		if ( !priv->pshare->rf_ft_var.antSw_select) {
			ODM_Write4Byte(pDM_Odm,0x870, ODM_Read4Byte(pDM_Odm,0x870) | BIT(8)| BIT(9) );  //  ANTSEL A as SW control
			ODM_Write1Byte(pDM_Odm,0xc50, ODM_Read1Byte(pDM_Odm,0xc50) & (~ BIT(7)));	// rx OFDM SW control
			PHY_SetBBReg(priv, 0x860, 0x300, ant);
		} else {
			ODM_Write4Byte(pDM_Odm,0x870, ODM_Read4Byte(pDM_Odm,0x870) | BIT(24)| BIT(25) ); // ANTSEL B as HW control
			PHY_SetBBReg(priv, 0x864, 0x300, ant);
			ODM_Write1Byte(pDM_Odm,0xc58, ODM_Read1Byte(pDM_Odm,0xc58) & (~ BIT(7)));		// rx OFDM SW control
		}

		ODM_Write1Byte(pDM_Odm,0xa01, ODM_Read1Byte(pDM_Odm,0xa01) & (~ BIT(7)));	// rx CCK SW control
		ODM_Write4Byte(pDM_Odm,0x80c, ODM_Read4Byte(pDM_Odm,0x80c) & (~ BIT(21))); // select ant by tx desc
		ODM_Write4Byte(pDM_Odm,0x858, 0x569a569a);

		priv->pshare->rf_ft_var.antHw_enable = 0;
		priv->pshare->rf_ft_var.CurAntenna  = (ant%2);

		#ifdef SW_ANT_SWITCH
		priv->pshare->rf_ft_var.antSw_enable = 0;
		priv->pshare->DM_SWAT_Table.CurAntenna = ant;
		priv->pshare->RSSI_test =0;
		#endif
	}
	else if(ant==0){

		if ( !priv->pshare->rf_ft_var.antSw_select)  {
			ODM_Write4Byte(pDM_Odm,0x870, ODM_Read4Byte(pDM_Odm,0x870) & ~(BIT(8)| BIT(9)) );
			ODM_Write1Byte(pDM_Odm,0xc50, ODM_Read1Byte(pDM_Odm,0xc50) | BIT(7));	// OFDM HW control
		} else {
			ODM_Write4Byte(pDM_Odm,0x870, ODM_Read4Byte(pDM_Odm,0x870) & ~(BIT(24)| BIT(25)) );
			ODM_Write1Byte(pDM_Odm,0xc58, ODM_Read1Byte(pDM_Odm,0xc58) | BIT(7));	// OFDM HW control
		}

		ODM_Write1Byte(pDM_Odm,0xa01, ODM_Read1Byte(pDM_Odm,0xa01) | BIT(7));	// CCK HW control
		ODM_Write4Byte(pDM_Odm,0x80c, ODM_Read4Byte(pDM_Odm,0x80c) | BIT(21) ); // by tx desc
		priv->pshare->rf_ft_var.CurAntenna = 0;
		ODM_Write4Byte(pDM_Odm,0x858, 0x569a569a);
		priv->pshare->rf_ft_var.antHw_enable = 1;
#ifdef SW_ANT_SWITCH
		priv->pshare->rf_ft_var.antSw_enable = 0;
		priv->pshare->RSSI_test =0;
#endif
	}
#ifdef SW_ANT_SWITCH
	else if(ant==3) {
		if(!priv->pshare->rf_ft_var.antSw_enable) {
			
			dm_SW_AntennaSwitchInit(priv);
			ODM_Write4Byte(pDM_Odm,0x858, 0x569a569a);
			priv->pshare->lastTxOkCnt = priv->net_stats.tx_bytes;
			priv->pshare->lastRxOkCnt = priv->net_stats.rx_bytes;
		}
		if ( !priv->pshare->rf_ft_var.antSw_select)
			ODM_Write1Byte(pDM_Odm,0xc50, ODM_Read1Byte(pDM_Odm,0xc50) & (~ BIT(7)));	// rx OFDM SW control
		else
			ODM_Write1Byte(pDM_Odm,0xc58, ODM_Read1Byte(pDM_Odm,0xc58) & (~ BIT(7)));	// rx OFDM SW control

		ODM_Write1Byte(pDM_Odm,0xa01, ODM_Read1Byte(pDM_Odm,0xa01) & (~ BIT(7)));		// rx CCK SW control
		ODM_Write4Byte(pDM_Odm,0x80c, ODM_Read4Byte(pDM_Odm,0x80c) & (~ BIT(21))); 	// select ant by tx desc
		priv->pshare->rf_ft_var.antHw_enable = 0;
		priv->pshare->rf_ft_var.antSw_enable = 1;

	}
#endif
	ODM_RT_TRACE(pDM_Odm, ODM_COMP_ANT_DIV,ODM_DBG_LOUD,("<==============ODM_Diversity_AntennaSelect\n"));

	return 1;
}
#endif

#else //#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))

VOID odm_InitHybridAntDiv(	IN PDM_ODM_T	pDM_Odm 	){}
VOID odm_HwAntDiv(	IN	PDM_ODM_T	pDM_Odm){}
#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
VOID ODM_SetTxAntByTxInfo_88C_92D(
	IN		PDM_ODM_T		pDM_Odm,
	IN		pu1Byte			pDesc,
	IN		u1Byte			macId	
){}
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
VOID ODM_SetTxAntByTxInfo_88C_92D(	IN		PDM_ODM_T		pDM_Odm){ }
#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
VOID ODM_SetTxAntByTxInfo_88C_92D(	IN		PDM_ODM_T		pDM_Odm){ }
#endif

#endif //#if(defined(CONFIG_HW_ANTENNA_DIVERSITY))



//============================================================
//EDCA Turbo
//============================================================
VOID
ODM_EdcaTurboInit(
	IN    PDM_ODM_T		pDM_Odm)
{

#if ((DM_ODM_SUPPORT_TYPE == ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))
	odm_EdcaParaInit(pDM_Odm);
#elif (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PADAPTER	Adapter = NULL;
	HAL_DATA_TYPE	*pHalData = NULL;

	if(pDM_Odm->Adapter==NULL)	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("EdcaTurboInit fail!!!\n"));
		return;
	}

	Adapter=pDM_Odm->Adapter;
	pHalData=GET_HAL_DATA(Adapter);

	pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = FALSE;	
	pDM_Odm->DM_EDCA_Table.bIsCurRDLState = FALSE;
	pHalData->bIsAnyNonBEPkts = FALSE;
	
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	PADAPTER	Adapter = pDM_Odm->Adapter;	
	pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = FALSE;	
	pDM_Odm->DM_EDCA_Table.bIsCurRDLState = FALSE;
	Adapter->recvpriv.bIsAnyNonBEPkts =FALSE;

#endif	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial VO PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_VO_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial VI PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_VI_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial BE PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_BE_PARAM)));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial BK PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_BK_PARAM)));

	
}	// ODM_InitEdcaTurbo

VOID
odm_EdcaTurboCheck(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	// 
	// For AP/ADSL use prtl8192cd_priv
	// For CE/NIC use PADAPTER
	//
//	PADAPTER		pAdapter = pDM_Odm->Adapter;
//	prtl8192cd_priv	priv		= pDM_Odm->priv;

	//
	// 2011/09/29 MH In HW integration first stage, we provide 4 different handle to operate
	// at the same time. In the stage2/3, we need to prive universal interface and merge all
	// HW dynamic mechanism.
	//
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("odm_EdcaTurboCheck========================>\n"));

	if(!(pDM_Odm->SupportAbility& ODM_MAC_EDCA_TURBO ))
		return;

	switch	(pDM_Odm->SupportPlatform)
	{
		case	ODM_WIN:

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
			odm_EdcaTurboCheckMP(pDM_Odm);
#endif
			break;

		case	ODM_CE:
#if(DM_ODM_SUPPORT_TYPE==ODM_CE)
			odm_EdcaTurboCheckCE(pDM_Odm);
#endif
			break;

		case	ODM_AP:
		case	ODM_ADSL:

#if ((DM_ODM_SUPPORT_TYPE == ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))
		odm_IotEngine(pDM_Odm);
#endif
			break;	
	}
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("<========================odm_EdcaTurboCheck\n"));

}	// odm_CheckEdcaTurbo

#if(DM_ODM_SUPPORT_TYPE==ODM_CE)


VOID
odm_EdcaTurboCheckCE(
	IN		PDM_ODM_T		pDM_Odm
	)
{

#if(DM_ODM_SUPPORT_TYPE==ODM_CE)

	PADAPTER		       Adapter = pDM_Odm->Adapter;
	
	u32 	trafficIndex;
	u32	edca_param;
	u64	cur_tx_bytes = 0;
	u64	cur_rx_bytes = 0;
	u8	bbtchange = _FALSE;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);	
	struct xmit_priv		*pxmitpriv = &(Adapter->xmitpriv);
	struct recv_priv		*precvpriv = &(Adapter->recvpriv);
	struct registry_priv	*pregpriv = &Adapter->registrypriv;
	struct mlme_ext_priv	*pmlmeext = &(Adapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);


	if ((pregpriv->wifi_spec == 1) )//|| (pmlmeinfo->HT_enable == 0))
	{
		goto dm_CheckEdcaTurbo_EXIT;
	}

	if (pmlmeinfo->assoc_AP_vendor >=  HT_IOT_PEER_MAX)
	{
		goto dm_CheckEdcaTurbo_EXIT;
	}

#ifdef CONFIG_BT_COEXIST
	if (BT_DisableEDCATurbo(Adapter))
	{
		goto dm_CheckEdcaTurbo_EXIT;
	}
#endif

	// Check if the status needs to be changed.
	if((bbtchange) || (!precvpriv->bIsAnyNonBEPkts) )
	{
		cur_tx_bytes = pxmitpriv->tx_bytes - pxmitpriv->last_tx_bytes;
		cur_rx_bytes = precvpriv->rx_bytes - precvpriv->last_rx_bytes;

		//traffic, TX or RX
		if((pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_RALINK)||(pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_ATHEROS))
		{
			if (cur_tx_bytes > (cur_rx_bytes << 2))
			{ // Uplink TP is present.
				trafficIndex = UP_LINK; 
			}
			else
			{ // Balance TP is present.
				trafficIndex = DOWN_LINK;
			}
		}
		else
		{
			if (cur_rx_bytes > (cur_tx_bytes << 2))
			{ // Downlink TP is present.
				trafficIndex = DOWN_LINK;
			}
			else
			{ // Balance TP is present.
				trafficIndex = UP_LINK;
			}
		}

		if ((pDM_Odm->DM_EDCA_Table.prv_traffic_idx != trafficIndex) || (!pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA))
		{
			
#if 0
			//adjust EDCA parameter for BE queue
			edca_param = EDCAParam[pmlmeinfo->assoc_AP_vendor][trafficIndex];
#else

			if((pmlmeinfo->assoc_AP_vendor == HT_IOT_PEER_CISCO) && (pmlmeext->cur_wireless_mode & WIRELESS_11_24N))
			{
				edca_param = EDCAParam[pmlmeinfo->assoc_AP_vendor][trafficIndex];
			}
			else
			{
				edca_param = EDCAParam[HT_IOT_PEER_UNKNOWN][trafficIndex];
			}
#endif
	
#ifdef CONFIG_PCI_HCI
			if(IS_92C_SERIAL(pHalData->VersionID))
			{
				edca_param = 0x60a42b;
			}
			else
			{
				edca_param = 0x6ea42b;
			}
#endif
			rtw_write32(Adapter, REG_EDCA_BE_PARAM, edca_param);

			pDM_Odm->DM_EDCA_Table.prv_traffic_idx = trafficIndex;
		}
		
		pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = _TRUE;
	}
	else
	{
		//
		// Turn Off EDCA turbo here.
		// Restore original EDCA according to the declaration of AP.
		//
		 if(pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA)
		{
			rtw_write32(Adapter, REG_EDCA_BE_PARAM, pHalData->AcParam_BE);
			pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = _FALSE;
		}
	}

dm_CheckEdcaTurbo_EXIT:
	// Set variables for next time.
	precvpriv->bIsAnyNonBEPkts = _FALSE;
	pxmitpriv->last_tx_bytes = pxmitpriv->tx_bytes;
	precvpriv->last_rx_bytes = precvpriv->rx_bytes;
#endif	
}


#elif(DM_ODM_SUPPORT_TYPE==ODM_WIN)
VOID
odm_EdcaTurboCheckMP(
	IN		PDM_ODM_T		pDM_Odm
	)
{


	PADAPTER		       Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PADAPTER 			pDefaultAdapter = GetDefaultAdapter(Adapter);
	PADAPTER 			pExtAdapter = GetFirstExtAdapter(Adapter);//NULL;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	PSTA_QOS			pStaQos = Adapter->MgntInfo.pStaQos;
	//[Win7 Count Tx/Rx statistic for Extension Port] odm_CheckEdcaTurbo's Adapter is always Default. 2009.08.20, by Bohn
	u8Byte				Ext_curTxOkCnt = 0;
	u8Byte				Ext_curRxOkCnt = 0;	
	static u8Byte 			Ext_lastTxOkCnt = 0;
	static u8Byte 			Ext_lastRxOkCnt = 0;
	//For future Win7  Enable Default Port to modify AMPDU size dynamically, 2009.08.20, Bohn.	
	u1Byte TwoPortStatus = (u1Byte)TWO_PORT_STATUS__WITHOUT_ANY_ASSOCIATE;
	
#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)
	struct dm_priv		*pdmpriv = &pHalData->dmpriv;
	struct xmit_priv		*pxmitpriv = &(Adapter->xmitpriv);
	struct recv_priv		*precvpriv = &(Adapter->recvpriv);
	struct registry_priv	*pregpriv = &Adapter->registrypriv;
	struct mlme_ext_priv	*pmlmeext = &(Adapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	#ifdef CONFIG_BT_COEXIST
	struct btcoexist_priv	*pbtpriv = &(pHalData->bt_coexist);	
	#endif
       u1Byte bbtchange =FALSE;
#endif
	// Keep past Tx/Rx packet count for RT-to-RT EDCA turbo.
	u8Byte				curTxOkCnt = 0;
	u8Byte				curRxOkCnt = 0;	
	u8Byte		       	lastTxOkCnt = 0;
	u8Byte		       	lastRxOkCnt = 0;	
	u4Byte				EDCA_BE_UL = 0x5ea42b;//Parameter suggested by Scott  //edca_setting_UL[pMgntInfo->IOTPeer];
	u4Byte				EDCA_BE_DL = 0x5ea42b;//Parameter suggested by Scott  //edca_setting_DL[pMgntInfo->IOTPeer];
       u4Byte                         EDCA_BE = 0x5ea42b;
	u4Byte                         IOTPeer=0;
	BOOLEAN                      *pbIsCurRDLState=NULL;
	BOOLEAN                      bLastIsCurRDLState=FALSE;
	BOOLEAN				 bBiasOnRx=FALSE;
	BOOLEAN				bEdcaTurboOn=FALSE;
	

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("odm_EdcaTurboCheckMP========================>"));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Orginial BE PARAM: 0x%x\n",ODM_Read4Byte(pDM_Odm,ODM_EDCA_BE_PARAM)));

////===============================
////list paramter for different platform
////===============================
	bLastIsCurRDLState=pDM_Odm->DM_EDCA_Table.bIsCurRDLState;
	pbIsCurRDLState=&(pDM_Odm->DM_EDCA_Table.bIsCurRDLState);	
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)	
       // Caculate TX/RX TP:
	curTxOkCnt = Adapter->TxStats.NumTxBytesUnicast - pMgntInfo->lastTxOkCnt;
	curRxOkCnt = Adapter->RxStats.NumRxBytesUnicast - pMgntInfo->lastRxOkCnt;
	if(pExtAdapter == NULL) 
		pExtAdapter = pDefaultAdapter;

	Ext_curTxOkCnt = pExtAdapter->TxStats.NumTxBytesUnicast - pMgntInfo->Ext_lastTxOkCnt;
	Ext_curRxOkCnt = pExtAdapter->RxStats.NumRxBytesUnicast - pMgntInfo->Ext_lastRxOkCnt;
	GetTwoPortSharedResource(Adapter,TWO_PORT_SHARED_OBJECT__STATUS,NULL,&TwoPortStatus);
	//For future Win7  Enable Default Port to modify AMPDU size dynamically, 2009.08.20, Bohn.
	if(TwoPortStatus == TWO_PORT_STATUS__EXTENSION_ONLY)
	{
		curTxOkCnt = Ext_curTxOkCnt ;
		curRxOkCnt = Ext_curRxOkCnt ;
	}
	//
	IOTPeer=pMgntInfo->IOTPeer;
	bBiasOnRx=(pMgntInfo->IOTAction & HT_IOT_ACT_EDCA_BIAS_ON_RX)?TRUE:FALSE;
	bEdcaTurboOn=((!pHalData->bIsAnyNonBEPkts) && (!pMgntInfo->bDisableFrameBursting))?TRUE:FALSE;
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("bIsAnyNonBEPkts : 0x%lx  bDisableFrameBursting : 0x%lx  \n",pHalData->bIsAnyNonBEPkts,pMgntInfo->bDisableFrameBursting));
	
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	// Caculate TX/RX TP:
	curTxOkCnt = pxmitpriv->tx_bytes - pxmitpriv->last_tx_bytes;
	curRxOkCnt = precvpriv->rx_bytes - precvpriv->last_rx_bytes;
	#ifdef CONFIG_BT_COEXIST
	if(pbtpriv->BT_Coexist)
	{
		if( (pbtpriv->BT_EDCA[UP_LINK]!=0) ||  (pbtpriv->BT_EDCA[DOWN_LINK]!=0))
			bbtchange = TRUE;		
	}
	#endif
	IOTPeer=pmlmeinfo->assoc_AP_vendor;
	bBiasOnRx=((IOTPeer == HT_IOT_PEER_RALINK)||(IOTPeer == HT_IOT_PEER_ATHEROS))?TRUE:FALSE;
	bEdcaTurboOn=(bbtchange || (!precvpriv->bIsAnyNonBEPkts))?TRUE:FALSE;
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("bbtchange : 0x%lx  bIsAnyNonBEPkts : 0x%lx  \n",bbtchange,precvpriv->bIsAnyNonBEPkts));
#endif


////===============================
////check if edca turbo is disabled
////===============================
	if(odm_IsEdcaTurboDisable(pDM_Odm))
		goto dm_CheckEdcaTurbo_EXIT;


////===============================
////remove iot case out
////===============================
	ODM_EdcaParaSelByIot(pDM_Odm, &EDCA_BE_UL, &EDCA_BE_DL);


////===============================
////Check if the status needs to be changed.
////===============================
	if(bEdcaTurboOn)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("bEdcaTurboOn : 0x%x bBiasOnRx : 0x%x\n",bEdcaTurboOn,bBiasOnRx));
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("curTxOkCnt : 0x%lx \n",curTxOkCnt));
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("curRxOkCnt : 0x%lx \n",curRxOkCnt));
		if(bBiasOnRx)
			odm_EdcaChooseTrafficIdx(pDM_Odm,curTxOkCnt, curRxOkCnt,   TRUE,  pbIsCurRDLState);
		else
			odm_EdcaChooseTrafficIdx(pDM_Odm,curTxOkCnt, curRxOkCnt,   FALSE,  pbIsCurRDLState);

//modify by Guo.Mingzhi 2011-12-29
			EDCA_BE=((*pbIsCurRDLState)==TRUE)?EDCA_BE_DL:EDCA_BE_UL;
			ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("EDCA Turbo on: EDCA_BE:0x%lx\n",EDCA_BE));

//             if(((*pbIsCurRDLState)!=bLastIsCurRDLState)||(!pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA))
//		{
//			EDCA_BE=((*pbIsCurRDLState)==TRUE)?EDCA_BE_DL:EDCA_BE_UL;
//			ODM_Write4Byte(pDM_Odm,ODM_EDCA_BE_PARAM,EDCA_BE);
 //             }
		pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = TRUE;
		
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("EDCA_BE_DL : 0x%lx  EDCA_BE_UL : 0x%lx  EDCA_BE : 0x%lx  \n",EDCA_BE_DL,EDCA_BE_UL,EDCA_BE));

	}
	else
	{
		// Turn Off EDCA turbo here.
		// Restore original EDCA according to the declaration of AP.
		 if(pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA)
		{
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
			Adapter->HalFunc.SetHwRegHandler(Adapter, HW_VAR_AC_PARAM, GET_WMM_PARAM_ELE_SINGLE_AC_PARAM(pStaQos->WMMParamEle, AC0_BE) );
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, pHalData->AcParam_BE);
#endif

			pDM_Odm->DM_EDCA_Table.bCurrentTurboEDCA = FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Restore EDCA BE: 0x%lx  \n",pDM_Odm->WMMEDCA_BE));

		}
	}

////===============================
////Set variables for next time.
////===============================
dm_CheckEdcaTurbo_EXIT:
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	pHalData->bIsAnyNonBEPkts = FALSE;
	pMgntInfo->lastTxOkCnt = Adapter->TxStats.NumTxBytesUnicast;
	pMgntInfo->lastRxOkCnt = Adapter->RxStats.NumRxBytesUnicast;
	pMgntInfo->Ext_lastTxOkCnt = pExtAdapter->TxStats.NumTxBytesUnicast;
	pMgntInfo->Ext_lastRxOkCnt = pExtAdapter->RxStats.NumRxBytesUnicast;
#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)
	precvpriv->bIsAnyNonBEPkts = FALSE;
	pxmitpriv->last_tx_bytes = pxmitpriv->tx_bytes;
	precvpriv->last_rx_bytes = precvpriv->rx_bytes;
#endif

}


//check if edca turbo is disabled
BOOLEAN
odm_IsEdcaTurboDisable(
	IN 	PDM_ODM_T 	pDM_Odm
)
{
	PADAPTER		       Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	u4Byte                         IOTPeer=pMgntInfo->IOTPeer;
#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)
	struct registry_priv	*pregpriv = &Adapter->registrypriv;
	struct mlme_ext_priv	*pmlmeext = &(Adapter->mlmeextpriv);
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	u4Byte                         IOTPeer=pmlmeinfo->assoc_AP_vendor;
	u1Byte                         WirelessMode=0xFF;                   //invalid value

	if(pDM_Odm->pWirelessMode!=NULL)
		WirelessMode=*(pDM_Odm->pWirelessMode);

#endif

#if defined(BT_30_SUPPORT) && (BT_30_SUPPORT == 1)	
	if(pDM_Odm->bBtDisableEdcaTurbo)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("EdcaTurboDisable for BT!!\n"));
		return TRUE;
	}
#endif

	if((!(pDM_Odm->SupportAbility& ODM_MAC_EDCA_TURBO ))||
		(pDM_Odm->bWIFITest)||
		(IOTPeer>= HT_IOT_PEER_MAX))
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("EdcaTurboDisable\n"));
		return TRUE;
	}


#if (DM_ODM_SUPPORT_TYPE ==ODM_WIN)
	// 1. We do not turn on EDCA turbo mode for some AP that has IOT issue
	// 2. User may disable EDCA Turbo mode with OID settings.
	if((pMgntInfo->IOTAction & HT_IOT_ACT_DISABLE_EDCA_TURBO) ||pHalData->bForcedDisableTurboEDCA){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("IOTAction:EdcaTurboDisable\n"));
		return	TRUE;
		}
		
#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	//suggested by Jr.Luke: open TXOP for B/G/BG/A mode 2012-0215
	if((WirelessMode==ODM_WM_B)||(WirelessMode==(ODM_WM_B|ODM_WM_G)||(WirelessMode==ODM_WM_G)||(WirelessMode=ODM_WM_A))
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, ODM_Read4Byte(pDM_Odm, ODM_EDCA_BE_PARAM)|0x5E0000);	
	
	if(pDM_Odm->SupportICType==ODM_RTL8192D)		{
		if ((pregpriv->wifi_spec == 1)  || (pmlmeext->cur_wireless_mode == WIRELESS_11B)) {
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("92D:EdcaTurboDisable\n"));
			return TRUE;
		}
	}	
	else
	{
		if((pregpriv->wifi_spec == 1) || (pmlmeinfo->HT_enable == 0)){
			ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD, ("Others:EdcaTurboDisable\n"));
			return TRUE;
		}
	}
#ifdef CONFIG_BT_COEXIST
	if (BT_DisableEDCATurbo(Adapter))
	{
		goto dm_CheckEdcaTurbo_EXIT;
	}
#endif

#endif

	return	FALSE;
	

}

//add iot case here: for MP/CE
VOID 
ODM_EdcaParaSelByIot(
	IN 	PDM_ODM_T 	pDM_Odm,
	OUT	u4Byte		*EDCA_BE_UL,
	OUT u4Byte		*EDCA_BE_DL
	)
{

	PADAPTER		       Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	u4Byte                         IOTPeer=0;
	u4Byte                         ICType=pDM_Odm->SupportICType;
	u1Byte                         WirelessMode=0xFF;                   //invalid value
	u4Byte				RFType=pDM_Odm->RFType;

#if(DM_ODM_SUPPORT_TYPE==ODM_WIN)
	PADAPTER 			pDefaultAdapter = GetDefaultAdapter(Adapter);
	PADAPTER 			pExtAdapter = GetFirstExtAdapter(Adapter);//NULL;
	PMGNT_INFO			pMgntInfo = &Adapter->MgntInfo;
	u1Byte 				TwoPortStatus = (u1Byte)TWO_PORT_STATUS__WITHOUT_ANY_ASSOCIATE;

#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	struct mlme_ext_info	*pmlmeinfo = &(pmlmeext->mlmext_info);
	#ifdef CONFIG_BT_COEXIST
	struct btcoexist_priv	*pbtpriv = &(pHalData->bt_coexist);	
	#endif
       u1Byte bbtchange =FALSE;
#endif

	if(pDM_Odm->pWirelessMode!=NULL)
		WirelessMode=*(pDM_Odm->pWirelessMode);
		
///////////////////////////////////////////////////////////
////list paramter for different platform
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)	
	IOTPeer=pMgntInfo->IOTPeer;
	GetTwoPortSharedResource(Adapter,TWO_PORT_SHARED_OBJECT__STATUS,NULL,&TwoPortStatus);

#elif(DM_ODM_SUPPORT_TYPE==ODM_CE)
	IOTPeer=pmlmeinfo->assoc_AP_vendor;
	#ifdef CONFIG_BT_COEXIST
	if(pbtpriv->BT_Coexist)
	{
		if( (pbtpriv->BT_EDCA[UP_LINK]!=0) ||  (pbtpriv->BT_EDCA[DOWN_LINK]!=0))
			bbtchange = TRUE;		
	}
	#endif

#endif

	if(ICType==ODM_RTL8192D)
	{      
		// Single PHY
		if(pDM_Odm->RFType==ODM_2T2R)
		{
			(*EDCA_BE_UL) = 0x60a42b;    //0x5ea42b;
			(*EDCA_BE_DL) = 0x60a42b;    //0x5ea42b;

		}
		else
		{
			(*EDCA_BE_UL) = 0x6ea42b;
			(*EDCA_BE_DL) = 0x6ea42b;
		}

	}
////============================
/// IOT case for MP
////============================	
#if (DM_ODM_SUPPORT_TYPE==ODM_WIN)
	else
	{

		if(pDM_Odm->SupportInterface==ODM_ITRF_PCIE){
			if((ICType==ODM_RTL8192C)&&(pDM_Odm->RFType==ODM_2T2R))			{
				(*EDCA_BE_UL) = 0x60a42b;
				(*EDCA_BE_DL) = 0x60a42b;
			}
			else
			{
				(*EDCA_BE_UL) = 0x6ea42b;
				(*EDCA_BE_DL) = 0x6ea42b;
			}
		}
	}
 
	if(TwoPortStatus == TWO_PORT_STATUS__EXTENSION_ONLY)
	{
		(*EDCA_BE_UL) = 0x5ea42b;//Parameter suggested by Scott  //edca_setting_UL[ExtAdapter->MgntInfo.IOTPeer];
		(*EDCA_BE_DL) = 0x5ea42b;//Parameter suggested by Scott  //edca_setting_DL[ExtAdapter->MgntInfo.IOTPeer];
	}
     
	#if (INTEL_PROXIMITY_SUPPORT == 1)
	if(pMgntInfo->IntelClassModeInfo.bEnableCA == TRUE)
	{
		(*EDCA_BE_UL) = (*EDCA_BE_DL) = 0xa44f;
	}
	else
	#endif		
	{
		if((!pMgntInfo->bDisableFrameBursting) && 
			(pMgntInfo->IOTAction & (HT_IOT_ACT_FORCED_ENABLE_BE_TXOP|HT_IOT_ACT_AMSDU_ENABLE)))
		{// To check whether we shall force turn on TXOP configuration.
			if(!((*EDCA_BE_UL) & 0xffff0000))
				(*EDCA_BE_UL) |= 0x005e0000; // Force TxOP limit to 0x005e for UL.
			if(!((*EDCA_BE_DL) & 0xffff0000))
				(*EDCA_BE_DL) |= 0x005e0000; // Force TxOP limit to 0x005e for DL.
		}
		
		//92D txop can't be set to 0x3e for cisco1250
		if((ICType!=ODM_RTL8192D) && (IOTPeer== HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
		{
			(*EDCA_BE_DL) = edca_setting_DL[IOTPeer];
			(*EDCA_BE_UL) = edca_setting_UL[IOTPeer];
		}
		//merge from 92s_92c_merge temp brunch v2445    20120215 
		else if((IOTPeer == HT_IOT_PEER_CISCO) &&((WirelessMode==ODM_WM_G)||(WirelessMode==(ODM_WM_B|ODM_WM_G))||(WirelessMode==ODM_WM_A)||(WirelessMode==ODM_WM_B)))
		{
			(*EDCA_BE_DL) = edca_setting_DL_GMode[IOTPeer];
		}
		else if((IOTPeer== HT_IOT_PEER_AIRGO )&& ((WirelessMode==ODM_WM_G)||(WirelessMode==ODM_WM_A)))
		{
			(*EDCA_BE_DL) = 0xa630;
		}

		else if(IOTPeer == HT_IOT_PEER_MARVELL)
		{
			(*EDCA_BE_DL) = edca_setting_DL[IOTPeer];
			(*EDCA_BE_UL) = edca_setting_UL[IOTPeer];
		}
		else if(IOTPeer == HT_IOT_PEER_ATHEROS)
		{
			// Set DL EDCA for Atheros peer to 0x3ea42b. Suggested by SD3 Wilson for ASUS TP issue. 
			(*EDCA_BE_DL) = edca_setting_DL[IOTPeer];
		}
	}
////============================
/// IOT case for CE 
////============================
#elif (DM_ODM_SUPPORT_TYPE==ODM_CE)

	if(RFType==ODM_RTL8192D)
	{
		if((IOTPeer == HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
		{
			(*EDCA_BE_UL) = EDCAParam[IOTPeer][UP_LINK];
			(*EDCA_BE_DL)=EDCAParam[IOTPeer][DOWN_LINK];
		}
		else if((IOTPeer == HT_IOT_PEER_AIRGO) &&
			((WirelessMode==ODM_WM_B)||(WirelessMode==(ODM_WM_B|ODM_WM_G)))) 
			(*EDCA_BE_DL)=0x00a630;
		
		else if((IOTPeer== HT_IOT_PEER_ATHEROS) && 
					(WirelessMode&ODM_WM_N5G) &&
					(Adapter->securitypriv.dot11PrivacyAlgrthm == _AES_ ))
			(*EDCA_BE_DL)=0xa42b;
			
	}
	//92C IOT case:
	else
	{
		#ifdef CONFIG_BT_COEXIST
		if(bbtchange)
		{
			(*EDCA_BE_UL) = pbtpriv->BT_EDCA[UP_LINK];
			(*EDCA_BE_DL) = pbtpriv->BT_EDCA[DOWN_LINK];		
		}
		else
		#endif
		{
			if((IOTPeer == HT_IOT_PEER_CISCO) &&(WirelessMode==ODM_WM_N24G))
			{
				(*EDCA_BE_UL) = EDCAParam[IOTPeer][UP_LINK];
				(*EDCA_BE_DL)=EDCAParam[IOTPeer][DOWN_LINK];
			}
			else
			{
				(*EDCA_BE_UL)=EDCAParam[HT_IOT_PEER_UNKNOWN][UP_LINK];
				(*EDCA_BE_DL)=EDCAParam[HT_IOT_PEER_UNKNOWN][DOWN_LINK];
			}
		}
		if(pDM_Odm->SupportInterface==ODM_ITRF_PCIE){
			if((ICType==ODM_RTL8192C)&&(pDM_Odm->RFType==ODM_2T2R))
			{
				(*EDCA_BE_UL) = 0x60a42b;
				(*EDCA_BE_DL) = 0x60a42b;
			}
			else
			{
				(*EDCA_BE_UL) = 0x6ea42b;
				(*EDCA_BE_DL) = 0x6ea42b;
			}
		}

	}
#endif

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Special: EDCA_BE_UL=0x%lx EDCA_BE_DL =0x%lx",(*EDCA_BE_UL),(*EDCA_BE_DL)));

}


VOID
odm_EdcaChooseTrafficIdx( 
	IN	PDM_ODM_T		pDM_Odm,
	IN	u8Byte  			cur_tx_bytes,  
	IN	u8Byte  			cur_rx_bytes, 
	IN	BOOLEAN 		bBiasOnRx,
	OUT BOOLEAN 		*pbIsCurRDLState
	)
{	
	
	
	if(bBiasOnRx)
	{
	  
		if(cur_tx_bytes>(cur_rx_bytes*4))
		{
			*pbIsCurRDLState=FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Uplink Traffic\n "));

		}
		else
		{
			*pbIsCurRDLState=TRUE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Balance Traffic\n"));

		}
	}
	else
	{
		if(cur_rx_bytes>(cur_tx_bytes*4))
		{
			*pbIsCurRDLState=TRUE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Downlink	Traffic\n"));

		}
		else
		{
			*pbIsCurRDLState=FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_EDCA_TURBO,ODM_DBG_LOUD,("Balance Traffic\n"));
		}
	}

	return ;
}

#endif

#if((DM_ODM_SUPPORT_TYPE==ODM_AP)||(DM_ODM_SUPPORT_TYPE==ODM_ADSL))

void odm_EdcaParaInit(
	IN	PDM_ODM_T	pDM_Odm
	)
{
	prtl8192cd_priv	priv		= pDM_Odm->priv;
	int   mode=priv->pmib->dot11BssType.net_work_type;
	
	static unsigned int slot_time, sifs_time;
	struct ParaRecord EDCA[4];

	 memset(EDCA, 0, 4*sizeof(struct ParaRecord));

	sifs_time = 10;
	slot_time = 20;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G))
		sifs_time = 16;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G| ODM_WM_G|ODM_WM_A))
		slot_time = 9;


#ifdef RTK_AC_SUPPORT //for 11ac logo,  edit aifs time for cca test cases
	if(AC_SIGMA_MODE != AC_SIGMA_NONE)
		sifs_time = 10;	
#endif


#if((defined(RTL_MANUAL_EDCA))&&(DM_ODM_SUPPORT_TYPE==ODM_AP))
	 if( priv->pmib->dot11QosEntry.ManualEDCA ) {
		 if( OPMODE & WIFI_AP_STATE )
			 memcpy(EDCA, priv->pmib->dot11QosEntry.AP_manualEDCA, 4*sizeof(struct ParaRecord));
		 else
			 memcpy(EDCA, priv->pmib->dot11QosEntry.STA_manualEDCA, 4*sizeof(struct ParaRecord));

		#ifdef WIFI_WMM
		if (QOS_ENABLE)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[VI].TXOPlimit<< 16) | (EDCA[VI].ECWmax<< 12) | (EDCA[VI].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
		else
		#endif
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[BE].TXOPlimit<< 16) | (EDCA[BE].ECWmax<< 12) | (EDCA[BE].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));

	}else
	#endif //RTL_MANUAL_EDCA
	{

		 if(OPMODE & WIFI_AP_STATE)
		 {
		 	memcpy(EDCA, rtl_ap_EDCA, 2*sizeof(struct ParaRecord));

			if(mode & (ODM_WM_A|ODM_WM_G|ODM_WM_N24G|ODM_WM_N5G))
				memcpy(&EDCA[VI], &rtl_ap_EDCA[VI_AG], 2*sizeof(struct ParaRecord));
			else
				memcpy(&EDCA[VI], &rtl_ap_EDCA[VI], 2*sizeof(struct ParaRecord));
		 }
		 else
		 {
		 	memcpy(EDCA, rtl_sta_EDCA, 2*sizeof(struct ParaRecord));

			if(mode & (ODM_WM_A|ODM_WM_G|ODM_WM_N24G|ODM_WM_N5G))
				memcpy(&EDCA[VI], &rtl_sta_EDCA[VI_AG], 2*sizeof(struct ParaRecord));
			else
				memcpy(&EDCA[VI], &rtl_sta_EDCA[VI], 2*sizeof(struct ParaRecord));
		 }
		 
	#ifdef WIFI_WMM
		if (QOS_ENABLE)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, (EDCA[VI].TXOPlimit<< 16) | (EDCA[VI].ECWmax<< 12) | (EDCA[VI].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
		else
	#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM,  (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + EDCA[VI].AIFSN* slot_time));
#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM,  (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + 2* slot_time));
#endif
			

	}

	ODM_Write4Byte(pDM_Odm, ODM_EDCA_VO_PARAM, (EDCA[VO].TXOPlimit<< 16) | (EDCA[VO].ECWmax<< 12) | (EDCA[VO].ECWmin<< 8) | (sifs_time + EDCA[VO].AIFSN* slot_time));
	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (EDCA[BE].TXOPlimit<< 16) | (EDCA[BE].ECWmax<< 12) | (EDCA[BE].ECWmin<< 8) | (sifs_time + EDCA[BE].AIFSN* slot_time));
	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM, (EDCA[BK].TXOPlimit<< 16) | (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | (sifs_time + EDCA[BK].AIFSN* slot_time));

#if defined(RTK_AC_SUPPORT) && defined(RTL_MANUAL_EDCA) //for 11ac logo,  make BK worse to seperate with BE.
	if((AC_SIGMA_MODE != AC_SIGMA_NONE) && (priv->pmib->dot11QosEntry.ManualEDCA))
	{
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM, (EDCA[BK].TXOPlimit<< 16) | (EDCA[BK].ECWmax<< 12) | (EDCA[BK].ECWmin<< 8) | 0xa4 );
	}
#endif

//	ODM_Write1Byte(pDM_Odm,ACMHWCTRL, 0x00);

	priv->pshare->iot_mode_enable = 0;
#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	if (priv->pshare->rf_ft_var.wifi_beq_iot)
		priv->pshare->iot_mode_VI_exist = 0;
	
	#ifdef WMM_VIBE_PRI
	priv->pshare->iot_mode_BE_exist = 0;
	#endif

#ifdef WMM_BEBK_PRI
	priv->pshare->iot_mode_BK_exist = 0;
#endif
	
	#ifdef LOW_TP_TXOP
	priv->pshare->BE_cwmax_enhance = 0;
	#endif

#elif (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
      priv->pshare->iot_mode_BE_exist = 0;   
#endif
	priv->pshare->iot_mode_VO_exist = 0;
}

BOOLEAN
ODM_ChooseIotMainSTA(
	IN	PDM_ODM_T		pDM_Odm,
	IN	PSTA_INFO_T		pstat
	)
{
	prtl8192cd_priv	priv = pDM_Odm->priv;
	BOOLEAN		bhighTP_found_pstat=FALSE;
	
	if ((GET_ROOT(priv)->up_time % 2) == 0) {
		unsigned int tx_2s_avg = 0;
		unsigned int rx_2s_avg = 0;
		int i=0, aggReady=0;
		unsigned long total_sum = (priv->pshare->current_tx_bytes+priv->pshare->current_rx_bytes);
		int assoc_num = GET_ROOT(priv)->assoc_num;
#ifdef MBSSID
		if (GET_ROOT(priv)->pmib->miscEntry.vap_enable){
			for (i=0; i<RTL8192CD_NUM_VWLAN; ++i)
				assoc_num += GET_ROOT(priv)->pvap_priv[i]-> assoc_num;
		}
#endif	
#ifdef UNIVERSAL_REPEATER
		if (IS_DRV_OPEN(GET_VXD_PRIV(GET_ROOT(priv))))
			assoc_num += GET_VXD_PRIV(GET_ROOT(priv))-> assoc_num;
#endif
#ifdef WDS
		 if(GET_ROOT(priv)->pmib->dot11WdsInfo.wdsEnabled)
			assoc_num ++;
#endif


		pstat->current_tx_bytes += pstat->tx_byte_cnt;
		pstat->current_rx_bytes += pstat->rx_byte_cnt;

		if (total_sum != 0) {
			if (total_sum <= 1000000) {
			tx_2s_avg = (unsigned int)((pstat->current_tx_bytes*100) / total_sum);
			rx_2s_avg = (unsigned int)((pstat->current_rx_bytes*100) / total_sum);
			} else {
				tx_2s_avg = (unsigned int)(pstat->current_tx_bytes / (total_sum / 100));
				rx_2s_avg = (unsigned int)(pstat->current_rx_bytes / (total_sum / 100));
			}

		}

#if(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (pstat->ht_cap_len) {
			if ((tx_2s_avg + rx_2s_avg) >=25 /*50*/) {

					priv->pshare->highTP_found_pstat = pstat;
					bhighTP_found_pstat=TRUE;
   				}
			}
#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
		for(i=0; i<8; i++)
			aggReady += (pstat->ADDBA_ready[i]);

		if ((pstat->ht_cap_len && (
#ifdef	SUPPORT_TX_AMSDU			
			AMSDU_ENABLE || 
#endif			
			aggReady)) || (pstat->IOTPeer==HT_IOT_PEER_INTEL))
		{
			if ((assoc_num==1) || (tx_2s_avg + rx_2s_avg >= 25)) {
				priv->pshare->highTP_found_pstat = pstat;
			}
			
		#ifdef CLIENT_MODE
			if (OPMODE & WIFI_STATION_STATE) {
				if ((tx_2s_avg + rx_2s_avg) >= 20)
					priv->pshare->highTP_found_pstat = pstat;
		}
		#endif				
	}
#endif
	} 
	else {
		pstat->current_tx_bytes = pstat->tx_byte_cnt;
		pstat->current_rx_bytes = pstat->rx_byte_cnt;
	}

	return bhighTP_found_pstat;
}


#ifdef WIFI_WMM
VOID
ODM_IotEdcaSwitch(
	IN	PDM_ODM_T		pDM_Odm,
	IN	unsigned char		enable
	)
{
	prtl8192cd_priv	priv	= pDM_Odm->priv;
	int   mode=priv->pmib->dot11BssType.net_work_type;
	unsigned int slot_time = 20, sifs_time = 10, BE_TXOP = 47, VI_TXOP = 94;
	unsigned int vi_cw_max = 4, vi_cw_min = 3, vi_aifs;
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	u32 be_edca, vi_edca;
	u16 disable_cfe;
#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
	if (!(!priv->pmib->dot11OperationEntry.wifi_specific ||
		((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific))
	#ifdef CLIENT_MODE
		|| ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific))
	#endif
		))
		return;
#endif

#ifdef RTK_AC_SUPPORT //for 11ac logo, do not dynamic switch edca 
	if(AC_SIGMA_MODE != AC_SIGMA_NONE)
		return;
#endif

	if ((mode & (ODM_WM_N24G|ODM_WM_N5G)) && (priv->pshare->ht_sta_num
	#ifdef WDS
		|| ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11WdsInfo.wdsEnabled && priv->pmib->dot11WdsInfo.wdsNum)
	#endif
		))
		sifs_time = 16;

	if (mode & (ODM_WM_N24G|ODM_WM_N5G|ODM_WM_G|ODM_WM_A)) {
		slot_time = 9;
	} 
	else
	{
		BE_TXOP = 94;
		VI_TXOP = 188;
	}

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	vi_edca = -1;
	disable_cfe = -1;
#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
	if (priv->pshare->iot_mode_VO_exist) {
		// to separate AC_VI and AC_BE to avoid using the same EDCA settings
		if (priv->pshare->iot_mode_BE_exist) {
			vi_cw_max = 5;
			vi_cw_min = 3;
		} else {
			vi_cw_max = 6;
			vi_cw_min = 4;
		}
	}
	vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	vi_edca = ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs;
#else
	ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs);
#endif
	
#elif (DM_ODM_SUPPORT_TYPE==ODM_AP)
	if ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11OperationEntry.wifi_specific) {
		if (priv->pshare->iot_mode_VO_exist) {
	#ifdef WMM_VIBE_PRI
			if (priv->pshare->iot_mode_BE_exist) 
			{
				vi_cw_max = 5;
				vi_cw_min = 3;
				vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);
			}
			else 
	#endif
			{
			vi_cw_max = 6;
			vi_cw_min = 4;
			vi_aifs = 0x2b;
			}
		} 
		else {
			vi_aifs = (sifs_time + ((OPMODE & WIFI_AP_STATE)?1:2) * slot_time);
		}

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
		vi_edca = ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)
			| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs;
#else
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_VI_PARAM, ((VI_TXOP*(1-priv->pshare->iot_mode_VO_exist)) << 16)
			| (vi_cw_max << 12) | (vi_cw_min << 8) | vi_aifs);
#endif

	#ifdef WMM_BEBK_PRI
	#ifdef CONFIG_RTL_88E_SUPPORT
		if ((GET_CHIP_VER(priv) == VERSION_8188E) && priv->pshare->iot_mode_BK_exist) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (10 << 12) | (6 << 8) | 0x4f;
#else
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM, (10 << 12) | (6 << 8) | 0x4f);
#endif
		}
	#endif		
	#endif
#if defined(CONFIG_WLAN_HAL_8881A) 
		if (GET_CHIP_VER(priv) == VERSION_8881A) 
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BK_PARAM,  0xa64f);
#endif		
	}
#endif



#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
 	if (priv->pshare->rf_ft_var.wifi_beq_iot && priv->pshare->iot_mode_VI_exist) {
#if defined(CONFIG_RTL_88E_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)
		if (GET_CHIP_VER(priv) == VERSION_8188E || GET_CHIP_VER(priv) == VERSION_8812E) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (10 << 12) | (6 << 8) | 0x4f;
#else
		  	ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (10 << 12) | (6 << 8) | 0x4f);
#endif
		}
		else
#endif
		{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (10 << 12) | (4 << 8) | 0x4f;
#else
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (10 << 12) | (4 << 8) | 0x4f);
#endif
		}
	} else if(!enable)
#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)      
	if(!enable)                                 //if iot is disable ,maintain original BEQ PARAM
#endif
	{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
		be_edca = (((OPMODE & WIFI_AP_STATE)?6:10) << 12) | (4 << 8)
			| (sifs_time + 3 * slot_time);
		disable_cfe = 1;
#else
		ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (((OPMODE & WIFI_AP_STATE)?6:10) << 12) | (4 << 8)
			| (sifs_time + 3 * slot_time));
#endif
#ifdef CONFIG_PCI_HCI
//		ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) | (DIS_TXOP_CFE));
#endif
	}
	else
	{
		int txop;
		unsigned int cw_max;
#ifdef LOW_TP_TXOP
		unsigned int txop_close;
#endif
		
	#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP))
			cw_max = ((priv->pshare->BE_cwmax_enhance) ? 10 : 6);
			txop_close = ((priv->pshare->rf_ft_var.low_tp_txop && priv->pshare->rf_ft_var.low_tp_txop_close) ? 1 : 0);

			if(priv->pshare->txop_enlarge == 0xe)   //if intel case
				txop = (txop_close ? 0 : (BE_TXOP*2));
			else                                                        //if other case
				txop = (txop_close ? 0: (BE_TXOP*priv->pshare->txop_enlarge));
	#else
			cw_max=6;
			if((priv->pshare->txop_enlarge==0xe)||(priv->pshare->txop_enlarge==0xd))
				txop=BE_TXOP*2;
			else
				txop=BE_TXOP*priv->pshare->txop_enlarge;

	#endif
                           
		if (priv->pshare->ht_sta_num
	#ifdef WDS
			|| ((OPMODE & WIFI_AP_STATE) && (mode & (ODM_WM_N24G|ODM_WM_N5G)) &&
			priv->pmib->dot11WdsInfo.wdsEnabled && priv->pmib->dot11WdsInfo.wdsNum)
	#endif
			) 
			{

			if (priv->pshare->txop_enlarge == 0xe) {
				// is intel client, use a different edca value
				//ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop<< 16) | (cw_max<< 12) | (4 << 8) | 0x1f);
				if (pDM_Odm->RFType==ODM_1T1R) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) | (5 << 12) | (3 << 8) | 0x1f;
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) | (5 << 12) | (3 << 8) | 0x1f);
#endif
				}
				else {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) | (8 << 12) | (5 << 8) | 0x1f;
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) | (8 << 12) | (5 << 8) | 0x1f);
#endif
				}
				
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
				disable_cfe = 0;
#endif
#ifdef CONFIG_PCI_HCI
//				ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) & ~(DIS_TXOP_CFE));
#endif
				priv->pshare->txop_enlarge = 2;
			} 
#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
	#ifndef LOW_TP_TXOP
			 else if (priv->pshare->txop_enlarge == 0xd) {
				// is intel ralink, use a different edca value
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
				be_edca = (txop << 16) | (6 << 12) | (5 << 8) | 0x2b;
#else
				ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) | (6 << 12) | (5 << 8) | 0x2b);
#endif
				priv->pshare->txop_enlarge = 2;
			} 
	#endif
#endif
			else 
			{
//				if (txop == 0) {
//#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
//					disable_cfe = 1;
//#endif
//#ifdef CONFIG_PCI_HCI
//					ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) | (DIS_TXOP_CFE));
//#endif
//				}
					
				if (pDM_Odm->RFType==ODM_2T2R) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time);
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
#endif
				}
				else
				#if(DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP)
				{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) |
						(((priv->pshare->BE_cwmax_enhance) ? 10 : 5) << 12) | (3 << 8) | (sifs_time + 2 * slot_time);
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(((priv->pshare->BE_cwmax_enhance) ? 10 : 5) << 12) | (3 << 8) | (sifs_time + 2 * slot_time));
#endif
				}
				#else
				{
					PSTA_INFO_T		pstat = priv->pshare->highTP_found_pstat;
					if ((GET_CHIP_VER(priv)==VERSION_8881A) && pstat && (pstat->IOTPeer == HT_IOT_PEER_HTC))
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, 0x642b);
					else {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					be_edca = (txop << 16) | (5 << 12) | (3 << 8) | (sifs_time + 2 * slot_time);
#else
					ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (txop << 16) |
						(5 << 12) | (3 << 8) | (sifs_time + 2 * slot_time));
#endif
					}
				}
				#endif
			}
		}
              else 
              {
 #if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined LOW_TP_TXOP))
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			 be_edca = (BE_TXOP << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time);
#else
			 ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (BE_TXOP << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
#endif
 #else
 		#if defined(CONFIG_RTL_8196D) || defined(CONFIG_RTL_8197DL) || defined(CONFIG_RTL_8196E) || (defined(CONFIG_RTL_8197D) && !defined(CONFIG_PORT0_EXT_GIGA))
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (BE_TXOP*2 << 16) | (cw_max << 12) | (5 << 8) | (sifs_time + 3 * slot_time);
#else
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (BE_TXOP*2 << 16) | (cw_max << 12) | (5 << 8) | (sifs_time + 3 * slot_time));
#endif
		#else
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			be_edca = (BE_TXOP*2 << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time);
#else
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM,  (BE_TXOP*2 << 16) | (cw_max << 12) | (4 << 8) | (sifs_time + 3 * slot_time));
#endif
		#endif
/*		
		if (priv->pshare->txop_enlarge == 0xe) {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			disable_cfe = 0;
#endif
	#ifdef CONFIG_PCI_HCI
			ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) & ~(DIS_TXOP_CFE));
	#endif
		} else {
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			disable_cfe = 1;
#endif
	#ifdef CONFIG_PCI_HCI
			ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) | (DIS_TXOP_CFE));
	#endif
		}
*/			
 #endif
              }

	}
	
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	notify_IOT_EDCA_switch(priv, be_edca, vi_edca, disable_cfe);
#endif
}
#endif

VOID 
odm_IotEngine(
	IN	PDM_ODM_T	pDM_Odm
	)
{

	struct rtl8192cd_priv *priv=pDM_Odm->priv;
	PSTA_INFO_T pstat = NULL;
	u4Byte i;
#if defined(WMM_DSCP_C42) && defined(TX_SHORTCUT)
	 u2Byte do_tx_slowpath=0;
#endif	 
#ifdef WIFI_WMM
	unsigned int switch_turbo = 0, avg_tp;
#endif	
	int assoc_num = priv->assoc_num;
////////////////////////////////////////////////////////
//  if EDCA Turbo function is not supported or Manual EDCA Setting
//  then return
////////////////////////////////////////////////////////
	if(!(pDM_Odm->SupportAbility&ODM_MAC_EDCA_TURBO)){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO NOT SUPPORTED\n"));
		return;
	}
	
#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&& defined(RTL_MANUAL_EDCA) && defined(WIFI_WMM))
	if(priv->pmib->dot11QosEntry.ManualEDCA){
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO OFF: MANUAL SETTING\n"));
		return ;
	}
#endif 

#ifdef MBSSID
	if (priv->pmib->miscEntry.vap_enable){
		for (i=0; i<RTL8192CD_NUM_VWLAN; ++i)
			assoc_num += priv->pvap_priv[i]-> assoc_num;
	}
#endif
#ifdef UNIVERSAL_REPEATER
	if (IS_DRV_OPEN(GET_VXD_PRIV(priv)))
		assoc_num += GET_VXD_PRIV(priv)-> assoc_num;
#endif
#ifdef WDS
	if(priv->pmib->dot11WdsInfo.wdsEnabled)
		assoc_num ++;
#endif
	
#if !(DM_ODM_SUPPORT_TYPE &ODM_AP)
 //////////////////////////////////////////////////////
 //find high TP STA every 2s
//////////////////////////////////////////////////////
	if ((GET_ROOT(priv)->up_time % 2) == 0) 
		priv->pshare->highTP_found_pstat==NULL;

#if 0
	phead = &priv->asoc_list;
	plist = phead->next;
	while(plist != phead)	{
		pstat = list_entry(plist, struct stat_info, asoc_list);

		if(ODM_ChooseIotMainSTA(pDM_Odm, pstat));              //find the correct station
			break;
		if (plist == plist->next)                                          //the last plist 
			break;
		plist = plist->next;
	};
#endif

	//find highTP STA
	for(i=0; i<ODM_ASSOCIATE_ENTRY_NUM; i++) {
		pstat = pDM_Odm->pODM_StaInfo[i];
		if(IS_STA_VALID(pstat) && (ODM_ChooseIotMainSTA(pDM_Odm, pstat)))	 //find the correct station
				break;
	}

 //////////////////////////////////////////////////////
 //if highTP STA is not found, then return
 //////////////////////////////////////////////////////
	if(priv->pshare->highTP_found_pstat==NULL)	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_EDCA_TURBO, ODM_DBG_LOUD, ("ODM_MAC_EDCA_TURBO OFF: NO HT STA FOUND\n"));
		return;
	}
#endif

	pstat=priv->pshare->highTP_found_pstat;
//	if(pstat) {
//		if((pstat->tx_avarage + pstat->rx_avarage) < (1<<17))	// 1M bps
//			pstat = NULL;
//	}

#ifdef WIFI_WMM
	if (QOS_ENABLE) {
		if (!priv->pmib->dot11OperationEntry.wifi_specific 
		#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
			||((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific))
		#elif(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
			|| (priv->pmib->dot11OperationEntry.wifi_specific == 2)
		#endif
			) {
			if (priv->pshare->iot_mode_enable &&
				((priv->pshare->phw->VO_pkt_count > 50) ||
				 (priv->pshare->phw->VI_pkt_count > 50) ||
				 (priv->pshare->phw->BK_pkt_count > 50))) {
				priv->pshare->iot_mode_enable = 0;
				switch_turbo++;
#ifdef CONFIG_WLAN_HAL_8881A
				if (GET_CHIP_VER(priv) == VERSION_8881A) {
					RTL_W32(0x460, 0x03086666);
				}
#endif //CONFIG_WLAN_HAL_8881A
			} else if ((!priv->pshare->iot_mode_enable) &&
				((priv->pshare->phw->VO_pkt_count < 50) &&
				 (priv->pshare->phw->VI_pkt_count < 50) &&
				 (priv->pshare->phw->BK_pkt_count < 50))) {
				priv->pshare->iot_mode_enable++;
				switch_turbo++;
//#ifdef CONFIG_WLAN_HAL_8881A
#if 0
				if (GET_CHIP_VER(priv) == VERSION_8881A) {
					if (get_bonding_type_8881A()==BOND_8881AB) {
						RTL_W32(0x460, 0x03086666);
					}
					else {
						RTL_W32(0x460, 0x0320ffff);
					}
				}
#endif //CONFIG_WLAN_HAL_8881A
			}
		}


		#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if ((OPMODE & WIFI_AP_STATE) && priv->pmib->dot11OperationEntry.wifi_specific)
		#elif (DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (priv->pmib->dot11OperationEntry.wifi_specific) 
		#endif
		{
			if (!priv->pshare->iot_mode_VO_exist && (priv->pshare->phw->VO_pkt_count > 50)) {
				priv->pshare->iot_mode_VO_exist++;
				switch_turbo++;
#if defined(WMM_DSCP_C42) && defined(TX_SHORTCUT)
				do_tx_slowpath = 1;
#endif				
			} else if (priv->pshare->iot_mode_VO_exist && (priv->pshare->phw->VO_pkt_count < 50)) {
				priv->pshare->iot_mode_VO_exist = 0;
				switch_turbo++;
#if defined(WMM_DSCP_C42) && defined(TX_SHORTCUT)		
				do_tx_slowpath = 1;
#endif				
			}
#if defined(WMM_DSCP_C42) && defined(TX_SHORTCUT)
			if ((IS_HAL_CHIP(priv)) && do_tx_slowpath &&!priv->pmib->dot11OperationEntry.disable_txsc ) {
				int j=0;
				struct stat_info *psta=NULL;						
				psta = findNextSTA(priv, &j);
				while(psta) {				
					memset(psta->tx_sc_ent, 0, sizeof(psta->tx_sc_ent));
					psta = findNextSTA(priv, &j);
				};	
			}				
#endif
#if((DM_ODM_SUPPORT_TYPE==ODM_ADSL)||((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined WMM_VIBE_PRI)))
			if (priv->pshare->iot_mode_VO_exist) {
				//printk("[%s %d] BE_pkt_count=%d\n", __FUNCTION__, __LINE__, priv->pshare->phw->BE_pkt_count);
				if (!priv->pshare->iot_mode_BE_exist && (priv->pshare->phw->BE_pkt_count > 250)) {
					priv->pshare->iot_mode_BE_exist++;
					switch_turbo++;
				} else if (priv->pshare->iot_mode_BE_exist && (priv->pshare->phw->BE_pkt_count < 250)) {
					priv->pshare->iot_mode_BE_exist = 0;
					switch_turbo++;
				}
			}
#endif

#if((DM_ODM_SUPPORT_TYPE==ODM_ADSL)||((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined WMM_BEBK_PRI)))
			if (priv->pshare->phw->BE_pkt_count) {
				//printk("[%s %d] BK_pkt_count=%d\n", __FUNCTION__, __LINE__, priv->pshare->phw->BK_pkt_count);
				if (!priv->pshare->iot_mode_BK_exist && (priv->pshare->phw->BK_pkt_count > 250)) {
					priv->pshare->iot_mode_BK_exist++;
					switch_turbo++;
				} else if (priv->pshare->iot_mode_BK_exist && (priv->pshare->phw->BK_pkt_count < 250)) {
					priv->pshare->iot_mode_BK_exist = 0;
					switch_turbo++;
				}
			}
#endif

#if (DM_ODM_SUPPORT_TYPE==ODM_AP)
			if (priv->pshare->rf_ft_var.wifi_beq_iot) 
			{
				if (!priv->pshare->iot_mode_VI_exist && (priv->pshare->phw->VI_rx_pkt_count > 50)) {
					priv->pshare->iot_mode_VI_exist++;
					switch_turbo++;
				} else if (priv->pshare->iot_mode_VI_exist && (priv->pshare->phw->VI_rx_pkt_count < 50)) {
					priv->pshare->iot_mode_VI_exist = 0;
					switch_turbo++;
				}
			}
#endif

		}
		else if (!pstat || pstat->rssi < priv->pshare->rf_ft_var.txop_enlarge_lower) {
		   if (priv->pshare->txop_enlarge) {
			   priv->pshare->txop_enlarge = 0;
			   if (priv->pshare->iot_mode_enable)
					switch_turbo++;
				}
         	}

#if(defined(CLIENT_MODE) && (DM_ODM_SUPPORT_TYPE==ODM_AP))
        if ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific))
        {
            if (priv->pshare->iot_mode_enable &&
                (((priv->pshare->phw->VO_pkt_count > 50) ||
                 (priv->pshare->phw->VI_pkt_count > 50) ||
                 (priv->pshare->phw->BK_pkt_count > 50)) ||
                 (pstat && (!pstat->ADDBA_ready[0]) & (!pstat->ADDBA_ready[3]))))
            {
                priv->pshare->iot_mode_enable = 0;
                switch_turbo++;
            }
            else if ((!priv->pshare->iot_mode_enable) &&
                (((priv->pshare->phw->VO_pkt_count < 50) &&
                 (priv->pshare->phw->VI_pkt_count < 50) &&
                 (priv->pshare->phw->BK_pkt_count < 50)) &&
                 (pstat && (pstat->ADDBA_ready[0] | pstat->ADDBA_ready[3]))))
            {
                priv->pshare->iot_mode_enable++;
                switch_turbo++;
            }
        }
#endif

		priv->pshare->phw->VO_pkt_count = 0;
		priv->pshare->phw->VI_pkt_count = 0;
		priv->pshare->phw->BK_pkt_count = 0;

	#if((DM_ODM_SUPPORT_TYPE==ODM_ADSL)||((DM_ODM_SUPPORT_TYPE==ODM_AP)&&(defined WMM_VIBE_PRI)))
		priv->pshare->phw->BE_pkt_count = 0;
	#endif
		
	#if(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if (priv->pshare->rf_ft_var.wifi_beq_iot)
			priv->pshare->phw->VI_rx_pkt_count = 0;
		#endif

	}
#endif

	if ((priv->up_time % 2) == 0) {
		/*
		 * decide EDCA content for different chip vendor
		 */
#ifdef WIFI_WMM
	#if(DM_ODM_SUPPORT_TYPE==ODM_ADSL)
		if (QOS_ENABLE && (!priv->pmib->dot11OperationEntry.wifi_specific || (priv->pmib->dot11OperationEntry.wifi_specific == 2)
	
	#elif(DM_ODM_SUPPORT_TYPE==ODM_AP)
		if (QOS_ENABLE && (!priv->pmib->dot11OperationEntry.wifi_specific || 
			((OPMODE & WIFI_AP_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		#ifdef CLIENT_MODE
            || ((OPMODE & WIFI_STATION_STATE) && (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		#endif
	#endif
		))
	
		{

			if (pstat && pstat->rssi >= priv->pshare->rf_ft_var.txop_enlarge_upper) {
#ifdef LOW_TP_TXOP
				if (pstat->IOTPeer==HT_IOT_PEER_INTEL)
				{
					if (priv->pshare->txop_enlarge != 0xe)
					{
						priv->pshare->txop_enlarge = 0xe;

						if (priv->pshare->iot_mode_enable)
							switch_turbo++;
					}
				} 
				else if (priv->pshare->txop_enlarge != 2) 
				{
					priv->pshare->txop_enlarge = 2;
					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
#else
				if (priv->pshare->txop_enlarge != 2)
				{
					if (pstat->IOTPeer==HT_IOT_PEER_INTEL)
						priv->pshare->txop_enlarge = 0xe;
					else if (pstat->IOTPeer==HT_IOT_PEER_RALINK)
						priv->pshare->txop_enlarge = 0xd;
					else if (pstat->IOTPeer==HT_IOT_PEER_HTC)
						priv->pshare->txop_enlarge = 0;		
					else
						priv->pshare->txop_enlarge = 2;

					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
#endif
			}
			else if ((!pstat || pstat->rssi < priv->pshare->rf_ft_var.txop_enlarge_lower) 
#ifdef SW_TX_QUEUE
			&& (priv->pshare->swq_en == 0)
#endif
			)
			{
				if (priv->pshare->txop_enlarge) {
					priv->pshare->txop_enlarge = 0;
					if (priv->pshare->iot_mode_enable)
						switch_turbo++;
				}
			}

#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&&( defined LOW_TP_TXOP))
			// for Intel IOT, need to enlarge CW MAX from 6 to 10
			if (pstat && pstat->IOTPeer==HT_IOT_PEER_INTEL && (((pstat->tx_avarage+pstat->rx_avarage)>>10) < 
					priv->pshare->rf_ft_var.cwmax_enhance_thd)) 
			{
				if (!priv->pshare->BE_cwmax_enhance && priv->pshare->iot_mode_enable)
				{
					priv->pshare->BE_cwmax_enhance = 1;
					switch_turbo++;
				}
			} else {
				if (priv->pshare->BE_cwmax_enhance) {
					priv->pshare->BE_cwmax_enhance = 0;
					switch_turbo++;
				}
			}
#endif
		}
#endif
		priv->pshare->current_tx_bytes = 0;
		priv->pshare->current_rx_bytes = 0;
	}else {
		if ((GET_CHIP_VER(priv) == VERSION_8881A)||(GET_CHIP_VER(priv) == VERSION_8192E)|| (GET_CHIP_VER(priv) == VERSION_8188E) ){
			unsigned int uldl_tp = (priv->pshare->current_tx_bytes+priv->pshare->current_rx_bytes)>>17;
			if((uldl_tp > 40 || assoc_num>10) && (priv->pshare->agg_to!= 1)) {
				RTL_W8(0x462, 0x08);
				priv->pshare->agg_to = 1;
			} else if((uldl_tp < 35) && (priv->pshare->agg_to !=0)) {
				RTL_W8(0x462, 0x02);
				priv->pshare->agg_to = 0;
			} 
		}
	}	
	
#if((DM_ODM_SUPPORT_TYPE==ODM_AP)&& defined( SW_TX_QUEUE))
	if(AMPDU_ENABLE) {
#ifdef TX_EARLY_MODE
		if (GET_TX_EARLY_MODE) {
			if (!GET_EM_SWQ_ENABLE &&
				((assoc_num > 1) ||
				(pstat && pstat->IOTPeer != HT_IOT_PEER_UNKNOWN))) {
				if ((priv->pshare->em_tx_byte_cnt >> 17) > EM_TP_UP_BOUND) 
					priv->pshare->reach_tx_limit_cnt++;				
				else					
					priv->pshare->reach_tx_limit_cnt = 0;	

				if (priv->pshare->txop_enlarge && priv->pshare->reach_tx_limit_cnt /*>= WAIT_TP_TIME*/) {
					GET_EM_SWQ_ENABLE = 1;			
					priv->pshare->reach_tx_limit_cnt = 0;

					if (pstat->IOTPeer == HT_IOT_PEER_INTEL)
						MAX_EM_QUE_NUM = 12;
					else if (pstat->IOTPeer == HT_IOT_PEER_RALINK)
						MAX_EM_QUE_NUM = 10;
					
					enable_em(priv);			
				}
			}
			else if (GET_EM_SWQ_ENABLE) {
				if ((priv->pshare->em_tx_byte_cnt >> 17) < EM_TP_LOW_BOUND)
					priv->pshare->reach_tx_limit_cnt++;				
				else					
					priv->pshare->reach_tx_limit_cnt = 0;	

				if (!priv->pshare->txop_enlarge || priv->pshare->reach_tx_limit_cnt >= WAIT_TP_TIME) {
					GET_EM_SWQ_ENABLE = 0;
					priv->pshare->reach_tx_limit_cnt = 0;
					disable_em(priv);
				}
			}
		}
#endif

        if((priv->ext_stats.tx_avarage>>17)>TP_HIGH_WATER_MARK) {
            if ((priv->pshare->txop_decision == 0)){
                switch_turbo++;

                if (pstat) {
                    if (pstat->IOTPeer==HT_IOT_PEER_INTEL)
                        priv->pshare->txop_enlarge = 0xe;
                    else if (pstat->IOTPeer==HT_IOT_PEER_RALINK)
                        priv->pshare->txop_enlarge = 0xd;
                    else if (pstat->IOTPeer==HT_IOT_PEER_HTC)
                        priv->pshare->txop_enlarge = 0;		
                    else
                        priv->pshare->txop_enlarge = 2;				

                } else if (priv->pshare->txop_enlarge == 0) {
                    priv->pshare->txop_enlarge = 2;
                }
                priv->pshare->txop_decision = 1;
            }
        }
        else{
            if (priv->pshare->swq_txmac_chg >= priv->pshare->rf_ft_var.swq_en_highthd){
                if ((priv->pshare->txop_decision == 0)){
                    switch_turbo++;
                    if (priv->pshare->txop_enlarge == 0) {
                        if( assoc_num < 10)
                            priv->pshare->txop_enlarge = 2;
                        else
                            priv->pshare->txop_enlarge = 1;
                    }
                    priv->pshare->txop_decision = 1;
                }
                else
                {
                    if ((switch_turbo > 0) && (priv->pshare->txop_enlarge == 0) && (priv->pshare->iot_mode_enable != 0))
                    {
                        if( assoc_num < 10)
                            priv->pshare->txop_enlarge = 2;
                        else
                            priv->pshare->txop_enlarge = 1;

                        switch_turbo--;
                    }
                }
            }
            else if(priv->pshare->swq_txmac_chg <= priv->pshare->rf_ft_var.swq_dis_lowthd){
                priv->pshare->txop_decision = 0;
            }
            else if ((priv->pshare->txop_decision == 1) && (switch_turbo > 0) && (priv->pshare->txop_enlarge == 0) && (priv->pshare->iot_mode_enable != 0)){
                if( assoc_num < 10)
                    priv->pshare->txop_enlarge = 2;
                else
                    priv->pshare->txop_enlarge = 1;
                switch_turbo--;
            }
        } 			

        if(priv->pshare->rf_ft_var.swq_enable == 0) {
            priv->pshare->swq_en = 0;
        }
        else {
            if(priv->pshare->swq_en == 0) {
                if(priv->pshare->swq_use_hw_timer) { /* new SWQ*/
                    priv->pshare->swq_en = 1;
                }
                else { /* Old SWQ, check TP and tx mac change*/
                    if((priv->ext_stats.tx_avarage>>17) > TP_HIGH_WATER_MARK ||
                        priv->pshare->swq_txmac_chg >= priv->pshare->rf_ft_var.swq_en_highthd) {
                        priv->pshare->swq_en = 1;
                    }      

                    #if (DM_ODM_SUPPORT_TYPE==ODM_AP) && defined(TX_EARLY_MODE)		
                    if (GET_EM_SWQ_ENABLE == 1)
                        priv->pshare->swq_en = 1;
                    #endif                       
                }
                                            
                if(priv->pshare->swq_en == 1) {
                    priv->pshare->swqen_keeptime = priv->up_time;
                    init_STA_SWQAggNum(priv);
                }
            }
            else {
                if(priv->pshare->swq_use_hw_timer == 0) {   /*Old SWQ, check TP and tx mac change*/  
                    if((priv->ext_stats.tx_avarage>>17) <= TP_HIGH_WATER_MARK && 
                        priv->pshare->swq_txmac_chg <= priv->pshare->rf_ft_var.swq_dis_lowthd
                        #if (DM_ODM_SUPPORT_TYPE==ODM_AP) && defined(TX_EARLY_MODE)     
                        && (GET_EM_SWQ_ENABLE == 0)
                        #endif 
                    ){
                        priv->pshare->swq_en = 0;
                    }
                }
                
            }

        }
    
    }
    else {
        priv->pshare->swq_en = 0;
    }
#endif

#ifdef WIFI_WMM
#ifdef LOW_TP_TXOP
	if ((!priv->pmib->dot11OperationEntry.wifi_specific || (priv->pmib->dot11OperationEntry.wifi_specific == 2))
		&& QOS_ENABLE) {
		if (switch_turbo || priv->pshare->rf_ft_var.low_tp_txop) {
			unsigned int thd_tp;
			unsigned char under_thd;
			unsigned int curr_tp;

			if (priv->pmib->dot11BssType.net_work_type & (ODM_WM_N24G|ODM_WM_N5G| ODM_WM_G))
			{
				// Determine the upper bound throughput threshold.
				if (priv->pmib->dot11BssType.net_work_type & (ODM_WM_N24G|ODM_WM_N5G)) {
					if (assoc_num && assoc_num != priv->pshare->ht_sta_num)
						thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_g;
					else
						thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_n;
				}
				else
					thd_tp = priv->pshare->rf_ft_var.low_tp_txop_thd_g;

				// Determine to close txop.
#if defined(UNIVERSAL_REPEATER) || defined(MBSSID)
				if(IS_STA_VALID(pstat)) 
				{	
					struct rtl8192cd_priv *tmppriv;
					struct aid_obj *aidarray;	
					aidarray = container_of(pstat, struct aid_obj, station);
					tmppriv = aidarray->priv;

					curr_tp = (unsigned int)(tmppriv->ext_stats.tx_avarage>>17) + (unsigned int)(tmppriv->ext_stats.rx_avarage>>17);
				} 
				else 
#endif
				curr_tp = (unsigned int)(priv->ext_stats.tx_avarage>>17) + (unsigned int)(priv->ext_stats.rx_avarage>>17);
				if (curr_tp <= thd_tp && curr_tp >= priv->pshare->rf_ft_var.low_tp_txop_thd_low)
					under_thd = 1;
				else
					under_thd = 0;
			}
			else
			{
				under_thd = 0;
			}

			if (switch_turbo) 
			{
				priv->pshare->rf_ft_var.low_tp_txop_close = under_thd;
				priv->pshare->rf_ft_var.low_tp_txop_count = 0;
			}
			else if (priv->pshare->iot_mode_enable && (priv->pshare->rf_ft_var.low_tp_txop_close != under_thd)) {
				priv->pshare->rf_ft_var.low_tp_txop_count++;
				if (priv->pshare->rf_ft_var.low_tp_txop_close) {
					priv->pshare->rf_ft_var.low_tp_txop_count = priv->pshare->rf_ft_var.low_tp_txop_delay;
				}
				if (priv->pshare->rf_ft_var.low_tp_txop_count ==priv->pshare->rf_ft_var.low_tp_txop_delay) 

				{					
					priv->pshare->rf_ft_var.low_tp_txop_count = 0;
					priv->pshare->rf_ft_var.low_tp_txop_close = under_thd;
					switch_turbo++;
				}
			} 
			else 
			{
				priv->pshare->rf_ft_var.low_tp_txop_count = 0;
			}
		}
	}
#endif		

#ifdef WMM_DSCP_C42
	if (switch_turbo) {
		if (!priv->pshare->iot_mode_enable && !priv->pshare->aggrmax_change) {
			RTL_W16(0x4ca, 0x0404);
			priv->pshare->aggrmax_change = 1;
		}
		else if (priv->pshare->iot_mode_enable && priv->pshare->aggrmax_change) {
			RTL_W16(0x4ca, priv->pshare->aggrmax_bak);
			priv->pshare->aggrmax_change = 0;
		}
	} 
#endif
#ifdef TX_EARLY_MODE
		unsigned int em_tp = ((priv->ext_stats.tx_avarage>>17) + (priv->ext_stats.rx_avarage>>17));
		if (em_tp > 80)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (0x5e << 16) | (4 << 12) | (3 << 8) | 0x19);
		else //if (em_tp < 75)
			ODM_Write4Byte(pDM_Odm, ODM_EDCA_BE_PARAM, (0x5e << 16) | (6 << 12) | (5 << 8) | 0x2b);
#endif

#if defined(MBSSID)
//	if (priv->pmib->miscEntry.vap_enable && assoc_num>10 && switch_turbo) {
//		ODM_Write2Byte(pDM_Odm, RD_CTRL, ODM_Read2Byte(pDM_Odm, RD_CTRL) | (DIS_TXOP_CFE));	
//	}
#endif
	if (switch_turbo)
		ODM_IotEdcaSwitch( pDM_Odm, priv->pshare->iot_mode_enable );
#endif
}
#endif


#if( DM_ODM_SUPPORT_TYPE == ODM_WIN) 
//
// 2011/07/26 MH Add an API for testing IQK fail case.
//
BOOLEAN
ODM_CheckPowerStatus(
	IN	PADAPTER		Adapter)
{

	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T			pDM_Odm = &pHalData->DM_OutSrc;
	RT_RF_POWER_STATE 	rtState;
	PMGNT_INFO			pMgntInfo	= &(Adapter->MgntInfo);

	// 2011/07/27 MH We are not testing ready~~!! We may fail to get correct value when init sequence.
	if (pMgntInfo->init_adpt_in_progress == TRUE)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ODM_CheckPowerStatus Return TRUE, due to initadapter"));
		return	TRUE;
	}
	
	//
	//	2011/07/19 MH We can not execute tx pwoer tracking/ LLC calibrate or IQK.
	//
	Adapter->HalFunc.GetHwRegHandler(Adapter, HW_VAR_RF_STATE, (pu1Byte)(&rtState));	
	if(Adapter->bDriverStopped || Adapter->bDriverIsGoingToPnpSetPowerSleep || rtState == eRfOff)
	{
		ODM_RT_TRACE(pDM_Odm, ODM_COMP_INIT, ODM_DBG_LOUD, ("ODM_CheckPowerStatus Return FALSE, due to %d/%d/%d\n", 
		Adapter->bDriverStopped, Adapter->bDriverIsGoingToPnpSetPowerSleep, rtState));
		return	FALSE;
	}
	return	TRUE;
}
#else
BOOLEAN
ODM_CheckPowerStatus(
		IN	PADAPTER		Adapter)
{
	/*
	   HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	   PDM_ODM_T			pDM_Odm = &pHalData->DM_OutSrc;
	   RT_RF_POWER_STATE 	rtState;
	   PMGNT_INFO			pMgntInfo	= &(Adapter->MgntInfo);

	// 2011/07/27 MH We are not testing ready~~!! We may fail to get correct value when init sequence.
	if (pMgntInfo->init_adpt_in_progress == TRUE)
	{
	ODM_RT_TRACE(pDM_Odm,COMP_INIT, DBG_LOUD, ("ODM_CheckPowerStatus Return TRUE, due to initadapter"));
	return	TRUE;
	}

	//
	//	2011/07/19 MH We can not execute tx pwoer tracking/ LLC calibrate or IQK.
	//
	Adapter->HalFunc.GetHwRegHandler(Adapter, HW_VAR_RF_STATE, (pu1Byte)(&rtState));	
	if(Adapter->bDriverStopped || Adapter->bDriverIsGoingToPnpSetPowerSleep || rtState == eRfOff)
	{
	ODM_RT_TRACE(pDM_Odm,COMP_INIT, DBG_LOUD, ("ODM_CheckPowerStatus Return FALSE, due to %d/%d/%d\n", 
	Adapter->bDriverStopped, Adapter->bDriverIsGoingToPnpSetPowerSleep, rtState));
	return	FALSE;
	}
	 */
	return	TRUE;
}
#endif

// need to ODM CE Platform
//move to here for ANT detection mechanism using

#if ((DM_ODM_SUPPORT_TYPE == ODM_WIN)||(DM_ODM_SUPPORT_TYPE == ODM_CE))
u4Byte
GetPSDData(
	IN PDM_ODM_T	pDM_Odm,
	unsigned int 	point,
	u1Byte initial_gain_psd)
{
	//unsigned int	val, rfval;
	//int	psd_report;
	u4Byte	psd_report;
	
	//HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	//Debug Message
	//val = PHY_QueryBBReg(Adapter,0x908, bMaskDWord);
	//DbgPrint("Reg908 = 0x%x\n",val);
	//val = PHY_QueryBBReg(Adapter,0xDF4, bMaskDWord);
	//rfval = PHY_QueryRFReg(Adapter, RF_PATH_A, 0x00, bRFRegOffsetMask);
	//DbgPrint("RegDF4 = 0x%x, RFReg00 = 0x%x\n",val, rfval);
	//DbgPrint("PHYTXON = %x, OFDMCCA_PP = %x, CCKCCA_PP = %x, RFReg00 = %x\n",
		//(val&BIT25)>>25, (val&BIT14)>>14, (val&BIT15)>>15, rfval);

	//Set DCO frequency index, offset=(40MHz/SamplePts)*point
	ODM_SetBBReg(pDM_Odm, 0x808, 0x3FF, point);

	//Start PSD calculation, Reg808[22]=0->1
	ODM_SetBBReg(pDM_Odm, 0x808, BIT22, 1);
	//Need to wait for HW PSD report
	ODM_StallExecution(30);
	ODM_SetBBReg(pDM_Odm, 0x808, BIT22, 0);
	//Read PSD report, Reg8B4[15:0]
	psd_report = ODM_GetBBReg(pDM_Odm,0x8B4, bMaskDWord) & 0x0000FFFF;
	
#if 1//(DEV_BUS_TYPE == RT_PCI_INTERFACE) && ( (RT_PLATFORM == PLATFORM_LINUX) || (RT_PLATFORM == PLATFORM_MACOSX))
	psd_report = (u4Byte) (ConvertTo_dB(psd_report))+(u4Byte)(initial_gain_psd-0x1c);
#else
	psd_report = (int) (20*log10((double)psd_report))+(int)(initial_gain_psd-0x1c);
#endif

	return psd_report;
	
}
#endif


//
// LukeLee: 
// PSD function will be moved to FW in future IC, but now is only implemented in MP platform
// So PSD function will not be incorporated to common ODM
//
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

#define	AFH_PSD		1	//0:normal PSD scan, 1: only do 20 pts PSD
#define	MODE_40M		0	//0:20M, 1:40M
#define	PSD_TH2		3  
#define	PSD_CHMIN		20   // Minimum channel number for BT AFH
#define	SIR_STEP_SIZE	3
#define   Smooth_Size_1 	5
#define	Smooth_TH_1	3
#define   Smooth_Size_2 	10
#define	Smooth_TH_2	4
#define   Smooth_Size_3 	20
#define	Smooth_TH_3	4
#define   Smooth_Step_Size 5
#define	Adaptive_SIR	1
//#if(RTL8723_FPGA_VERIFICATION == 1)
//#define	PSD_RESCAN		1
//#else
//#define	PSD_RESCAN		4
//#endif
#define	SCAN_INTERVAL	700 //ms
#define	SYN_Length		5    // for 92D
	
#define	LNA_Low_Gain_1                      0x64
#define	LNA_Low_Gain_2                      0x5A
#define	LNA_Low_Gain_3                      0x58

#define	pw_th_10dB					0x0
#define	pw_th_16dB					0x3

#define	FA_RXHP_TH1                           5000
#define	FA_RXHP_TH2                           1500
#define	FA_RXHP_TH3                             800
#define	FA_RXHP_TH4                             600
#define	FA_RXHP_TH5                             500

#define	Idle_Mode					0
#define	High_TP_Mode				1
#define	Low_TP_Mode				2


VOID
odm_PSDMonitorInit(
	IN PDM_ODM_T	pDM_Odm)
{
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)|(DEV_BUS_TYPE == RT_USB_INTERFACE)
	//HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	//PSD Monitor Setting
	//Which path in ADC/DAC is turnned on for PSD: both I/Q
	ODM_SetBBReg(pDM_Odm, ODM_PSDREG, BIT10|BIT11, 0x3);
	//Ageraged number: 8
	ODM_SetBBReg(pDM_Odm, ODM_PSDREG, BIT12|BIT13, 0x1);
	pDM_Odm->bPSDinProcess = FALSE;
	pDM_Odm->bUserAssignLevel = FALSE;
	pDM_Odm->bPSDactive = FALSE;
	//pDM_Odm->bDMInitialGainEnable=TRUE;		//change the initialization to DIGinit
	//Set Debug Port
	//PHY_SetBBReg(Adapter, 0x908, bMaskDWord, 0x803);
	//PHY_SetBBReg(Adapter, 0xB34, bMaskByte0, 0x00); // pause PSD
	//PHY_SetBBReg(Adapter, 0xB38, bMaskByte0, 10); //rescan
	//PHY_SetBBReg(Adapter, 0xB38, bMaskByte1, 0x32); // PSDDelay
	//PHY_SetBBReg(Adapter, 0xB38, bMaskByte2|bMaskByte3, 100); //interval

	//PlatformSetTimer( Adapter, &pHalData->PSDTriggerTimer, 0); //ms
#endif
}

VOID
PatchDCTone(
	IN	PDM_ODM_T	pDM_Odm,
	pu4Byte		PSD_report,
	u1Byte 		initial_gain_psd
)
{
	//HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	//PADAPTER	pAdapter;
	
	u4Byte	psd_report;

	//2 Switch to CH11 to patch CH9 and CH13 DC tone
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x3FF, 11);
	
	if(pDM_Odm->SupportICType== ODM_RTL8192D)
	{
		if((*(pDM_Odm->pMacPhyMode) == ODM_SMSP)||(*(pDM_Odm->pMacPhyMode) == ODM_DMSP))
		{
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, RF_CHNLBW, 0x3FF, 11);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x25, 0xfffff, 0x643BC);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x26, 0xfffff, 0xFC038);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x27, 0xfffff, 0x77C1A);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x2B, 0xfffff, 0x41289);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x2C, 0xfffff, 0x01840);
		}
		else
		{
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x25, 0xfffff, 0x643BC);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x26, 0xfffff, 0xFC038);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x27, 0xfffff, 0x77C1A);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x2B, 0xfffff, 0x41289);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x2C, 0xfffff, 0x01840);
		}
	}
	
	//Ch9 DC tone patch
	psd_report = GetPSDData(pDM_Odm, 96, initial_gain_psd);
	PSD_report[50] = psd_report;
	//Ch13 DC tone patch
	psd_report = GetPSDData(pDM_Odm, 32, initial_gain_psd);
	PSD_report[70] = psd_report;
	
	//2 Switch to CH3 to patch CH1 and CH5 DC tone
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x3FF, 3);

	
	if(pDM_Odm->SupportICType==ODM_RTL8192D)
	{
		if((*(pDM_Odm->pMacPhyMode) == ODM_SMSP)||(*(pDM_Odm->pMacPhyMode) == ODM_DMSP))
		{
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, RF_CHNLBW, 0x3FF, 3);
			//PHY_SetRFReg(Adapter, RF_PATH_B, 0x25, 0xfffff, 0x643BC);
			//PHY_SetRFReg(Adapter, RF_PATH_B, 0x26, 0xfffff, 0xFC038);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x27, 0xfffff, 0x07C1A);
			//PHY_SetRFReg(Adapter, RF_PATH_B, 0x2B, 0xfffff, 0x61289);
			//PHY_SetRFReg(Adapter, RF_PATH_B, 0x2C, 0xfffff, 0x01C41);
		}
		else
		{
			//PHY_SetRFReg(Adapter, RF_PATH_A, 0x25, 0xfffff, 0x643BC);
			//PHY_SetRFReg(Adapter, RF_PATH_A, 0x26, 0xfffff, 0xFC038);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x27, 0xfffff, 0x07C1A);
			//PHY_SetRFReg(Adapter, RF_PATH_A, 0x2B, 0xfffff, 0x61289);
			//PHY_SetRFReg(Adapter, RF_PATH_A, 0x2C, 0xfffff, 0x01C41);
		}
	}
	
	//Ch1 DC tone patch
	psd_report = GetPSDData(pDM_Odm, 96, initial_gain_psd);
	PSD_report[10] = psd_report;
	//Ch5 DC tone patch
	psd_report = GetPSDData(pDM_Odm, 32, initial_gain_psd);
	PSD_report[30] = psd_report;

}


VOID
GoodChannelDecision(
	PDM_ODM_T	pDM_Odm,
	ps4Byte		PSD_report,
	pu1Byte		PSD_bitmap,
	u1Byte 		RSSI_BT,
	pu1Byte		PSD_bitmap_memory)
{
	pRXHP_T			pRX_HP_Table = &pDM_Odm->DM_RXHP_Table;
	//s4Byte	TH1 =  SSBT-0x15;    // modify TH by Neil Chen
	s4Byte	TH1= RSSI_BT+0x14;
	s4Byte	TH2 = RSSI_BT+85;
	//u2Byte    TH3;
//	s4Byte	RegB34;
	u1Byte	bitmap, Smooth_size[3], Smooth_TH[3];
	//u1Byte	psd_bit;
	u4Byte	i,n,j, byte_idx, bit_idx, good_cnt, good_cnt_smoothing, Smooth_Interval[3];
	int 		start_byte_idx,start_bit_idx,cur_byte_idx, cur_bit_idx,NOW_byte_idx ;
	
//	RegB34 = PHY_QueryBBReg(Adapter,0xB34, bMaskDWord)&0xFF;

	if((pDM_Odm->SupportICType == ODM_RTL8192C)||(pDM_Odm->SupportICType == ODM_RTL8192D))
       {
            TH1 = RSSI_BT + 0x14;  
	}

	Smooth_size[0]=Smooth_Size_1;
	Smooth_size[1]=Smooth_Size_2;
	Smooth_size[2]=Smooth_Size_3;
	Smooth_TH[0]=Smooth_TH_1;
	Smooth_TH[1]=Smooth_TH_2;
	Smooth_TH[2]=Smooth_TH_3;
	Smooth_Interval[0]=16;
	Smooth_Interval[1]=15;
	Smooth_Interval[2]=13;
	good_cnt = 0;
	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		//2 Threshold  

		if(RSSI_BT >=41)
			TH1 = 113;	
		else if(RSSI_BT >=38)   // >= -15dBm
			TH1 = 105;                              //0x69
		else if((RSSI_BT >=33)&(RSSI_BT <38))
			TH1 = 99+(RSSI_BT-33);         //0x63
		else if((RSSI_BT >=26)&(RSSI_BT<33))
			TH1 = 99-(33-RSSI_BT)+2;     //0x5e
	 	else if((RSSI_BT >=24)&(RSSI_BT<26))
			TH1 = 88-((RSSI_BT-24)*3);   //0x58
		else if((RSSI_BT >=18)&(RSSI_BT<24))
			TH1 = 77+((RSSI_BT-18)*2);
		else if((RSSI_BT >=14)&(RSSI_BT<18))
			TH1 = 63+((RSSI_BT-14)*2);
		else if((RSSI_BT >=8)&(RSSI_BT<14))
			TH1 = 58+((RSSI_BT-8)*2);
		else if((RSSI_BT >=3)&(RSSI_BT<8))
			TH1 = 52+(RSSI_BT-3);
		else
			TH1 = 51;
	}

	for (i = 0; i< 10; i++)
		PSD_bitmap[i] = 0;
	

	 // Add By Gary
       for (i=0; i<80; i++)
	   	pRX_HP_Table->PSD_bitmap_RXHP[i] = 0;
	// End



	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
		TH1 =TH1-SIR_STEP_SIZE;
	}
	while (good_cnt < PSD_CHMIN)
	{
		good_cnt = 0;
		if(pDM_Odm->SupportICType==ODM_RTL8723A)
		{
		if(TH1 ==TH2)
			break;
		if((TH1+SIR_STEP_SIZE) < TH2)
			TH1 += SIR_STEP_SIZE;
		else
			TH1 = TH2;
		}
		else
		{
			if(TH1==(RSSI_BT+0x1E))
             		     break;    
   			if((TH1+2) < (RSSI_BT+0x1E))
				TH1+=3;
		     	else
				TH1 = RSSI_BT+0x1E;	
             
		}
		ODM_RT_TRACE(pDM_Odm,COMP_PSD,DBG_LOUD,("PSD: decision threshold is: %d", TH1));
			 
		for (i = 0; i< 80; i++)
		{
			if(PSD_report[i] < TH1)
			{
				byte_idx = i / 8;
				bit_idx = i -8*byte_idx;
				bitmap = PSD_bitmap[byte_idx];
				PSD_bitmap[byte_idx] = bitmap | (u1Byte) (1 << bit_idx);
			}
		}

#if DBG
		ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: before smoothing\n"));
		for(n=0;n<10;n++)
		{
			//DbgPrint("PSD_bitmap[%u]=%x\n", n, PSD_bitmap[n]);
			for (i = 0; i<8; i++)
				ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD_bitmap[%u] =   %d\n", 2402+n*8+i, (PSD_bitmap[n]&BIT(i))>>i));
		}
#endif
	
		//1 Start of smoothing function

		for (j=0;j<3;j++)
		{
			start_byte_idx=0;
			start_bit_idx=0;
			for(n=0; n<Smooth_Interval[j]; n++)
			{
				good_cnt_smoothing = 0;
				cur_bit_idx = start_bit_idx;
				cur_byte_idx = start_byte_idx;
				for ( i=0; i < Smooth_size[j]; i++)
				{
					NOW_byte_idx = cur_byte_idx + (i+cur_bit_idx)/8;
					if ( (PSD_bitmap[NOW_byte_idx]& BIT( (cur_bit_idx + i)%8)) != 0)
						good_cnt_smoothing++;

				}

				if( good_cnt_smoothing < Smooth_TH[j] )
				{
					cur_bit_idx = start_bit_idx;
					cur_byte_idx = start_byte_idx;
					for ( i=0; i< Smooth_size[j] ; i++)
					{	
						NOW_byte_idx = cur_byte_idx + (i+cur_bit_idx)/8;				
						PSD_bitmap[NOW_byte_idx] = PSD_bitmap[NOW_byte_idx] & (~BIT( (cur_bit_idx + i)%8));
					}
				}
				start_bit_idx =  start_bit_idx + Smooth_Step_Size;
				while ( (start_bit_idx)  > 7 )
				{
					start_byte_idx= start_byte_idx+start_bit_idx/8;
					start_bit_idx = start_bit_idx%8;
				}
			}

			ODM_RT_TRACE(	pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: after %u smoothing", j+1));
			for(n=0;n<10;n++)
			{
				for (i = 0; i<8; i++)
				{
					ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD_bitmap[%u] =   %d\n", 2402+n*8+i, (PSD_bitmap[n]&BIT(i))>>i));
					
					if ( ((PSD_bitmap[n]&BIT(i))>>i) ==1)  //----- Add By Gary
					{
	                                   pRX_HP_Table->PSD_bitmap_RXHP[8*n+i] = 1;
					}                                                  // ------end by Gary
				}
			}

		}

	
		good_cnt = 0;
		for ( i = 0; i < 10; i++)
		{
			for (n = 0; n < 8; n++)
				if((PSD_bitmap[i]& BIT(n)) != 0)
					good_cnt++;
		}
		ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: good channel cnt = %u",good_cnt));
	}

	//RT_TRACE(COMP_PSD, DBG_LOUD,("PSD: SSBT=%d, TH2=%d, TH1=%d",SSBT,TH2,TH1));
	for (i = 0; i <10; i++)
		ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: PSD_bitmap[%u]=%x",i,PSD_bitmap[i]));
/*	
	//Update bitmap memory
	for(i = 0; i < 80; i++)
	{
		byte_idx = i / 8;
		bit_idx = i -8*byte_idx;
		psd_bit = (PSD_bitmap[byte_idx] & BIT(bit_idx)) >> bit_idx;
		bitmap = PSD_bitmap_memory[i]; 
		PSD_bitmap_memory[i] = (bitmap << 1) |psd_bit;
	}
*/
}



VOID
odm_PSD_Monitor(
	PDM_ODM_T	pDM_Odm
)
{
	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	//PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

	
       unsigned int 		pts, start_point, stop_point, initial_gain ;
	static u1Byte		PSD_bitmap_memory[80], init_memory = 0;
	static u1Byte 		psd_cnt=0;
	static u4Byte		PSD_report[80], PSD_report_tmp;
	static u8Byte		lastTxOkCnt=0, lastRxOkCnt=0;
	u1Byte 			H2C_PSD_DATA[5]={0,0,0,0,0};
	static u1Byte		H2C_PSD_DATA_last[5] ={0,0,0,0,0};
	u1Byte			idx[20]={96,99,102,106,109,112,115,118,122,125,
					0,3,6,10,13,16,19,22,26,29};
	u1Byte			n, i, channel, BBReset,tone_idx;
	u1Byte			PSD_bitmap[10], SSBT=0,initial_gain_psd=0, RSSI_BT=0, initialGainUpper;
	s4Byte    			PSD_skip_start, PSD_skip_stop;
	u4Byte			CurrentChannel, RXIQI, RxIdleLowPwr, wlan_channel;
	u4Byte			ReScan, Interval, Is40MHz;
	u8Byte			curTxOkCnt, curRxOkCnt;
	int 				cur_byte_idx, cur_bit_idx;
	PADAPTER		Adapter = pDM_Odm->Adapter;
	PMGNT_INFO      	pMgntInfo = &Adapter->MgntInfo;
	
	if(*(pDM_Odm->pbScanInProcess))
	{
		if((pDM_Odm->SupportICType==ODM_RTL8723A)&(pDM_Odm->SupportInterface==ODM_ITRF_PCIE))
		{
			//pHalData->bPSDactive=FALSE;
			//ODM_SetTimer(pDM_Odm,&pDM_SWAT_Table->SwAntennaSwitchTimer, 100 )
			ODM_SetTimer( pDM_Odm, &pDM_Odm->PSDTimer, 900); //ms	
			//psd_cnt=0;
		}
		return;
	}

	if(pDM_Odm->bBtHsOperation)
	{
		ReScan = 1;
		Interval = SCAN_INTERVAL;
	}
	else
	{
	ReScan = PSD_RESCAN;
	Interval = SCAN_INTERVAL;
	}

	//1 Initialization
	if(init_memory == 0)
	{
		ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("Init memory\n"));
		for(i = 0; i < 80; i++)
			PSD_bitmap_memory[i] = 0xFF; // channel is always good
		init_memory = 1;
	}
	if(psd_cnt == 0)
	{
		ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("Enter dm_PSD_Monitor\n"));
		for(i = 0; i < 80; i++)
			PSD_report[i] = 0;
	}
#if 0	//for test only
	DbgPrint("cosa odm_PSD_Monitor call()\n");
	DbgPrint("cosa pHalData->RSSI_BT = %d\n", pHalData->RSSI_BT);
	DbgPrint("cosa pHalData->bUserAssignLevel = %d\n", pHalData->bUserAssignLevel);
#if 0
	psd_cnt++;
	if (psd_cnt < ReScan)
		PlatformSetTimer( Adapter, &pHalData->PSDTimer, Interval); //ms
	else
		psd_cnt = 0;
	return;
#endif
#endif
	//1 Backup Current Settings
	CurrentChannel = ODM_GetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask);
/*
	if(pDM_Odm->SupportICType==ODM_RTL8192D)
	{
		//2 Record Current synthesizer parameters based on current channel
		if((*pDM_Odm->MacPhyMode92D == SINGLEMAC_SINGLEPHY)||(*pDM_Odm->MacPhyMode92D == DUALMAC_SINGLEPHY))
		{
			SYN_RF25 = ODM_GetRFReg(Adapter, RF_PATH_B, 0x25, bMaskDWord);
			SYN_RF26 = ODM_GetRFReg(Adapter, RF_PATH_B, 0x26, bMaskDWord);
			SYN_RF27 = ODM_GetRFReg(Adapter, RF_PATH_B, 0x27, bMaskDWord);
			SYN_RF2B = ODM_GetRFReg(Adapter, RF_PATH_B, 0x2B, bMaskDWord);
			SYN_RF2C = ODM_GetRFReg(Adapter, RF_PATH_B, 0x2C, bMaskDWord);
       	}
		else     // DualMAC_DualPHY 2G
		{
			SYN_RF25 = ODM_GetRFReg(Adapter, RF_PATH_A, 0x25, bMaskDWord);
			SYN_RF26 = ODM_GetRFReg(Adapter, RF_PATH_A, 0x26, bMaskDWord);
			SYN_RF27 = ODM_GetRFReg(Adapter, RF_PATH_A, 0x27, bMaskDWord);
			SYN_RF2B = ODM_GetRFReg(Adapter, RF_PATH_A, 0x2B, bMaskDWord);
			SYN_RF2C = ODM_GetRFReg(Adapter, RF_PATH_A, 0x2C, bMaskDWord);
		}
	}
*/
	//RXIQI = PHY_QueryBBReg(Adapter, 0xC14, bMaskDWord);
	RXIQI = ODM_GetBBReg(pDM_Odm, 0xC14, bMaskDWord);

	//RxIdleLowPwr = (PHY_QueryBBReg(Adapter, 0x818, bMaskDWord)&BIT28)>>28;
	RxIdleLowPwr = (ODM_GetBBReg(pDM_Odm, 0x818, bMaskDWord)&BIT28)>>28;

	//2???
	Is40MHz = pMgntInfo->pHTInfo->bCurBW40MHz;

	ODM_RT_TRACE(pDM_Odm,	COMP_PSD, DBG_LOUD,("PSD Scan Start\n"));
	//1 Turn off CCK
	//PHY_SetBBReg(Adapter, rFPGA0_RFMOD, BIT24, 0);
	ODM_SetBBReg(pDM_Odm, rFPGA0_RFMOD, BIT24, 0);
	//1 Turn off TX
	//Pause TX Queue
	//PlatformEFIOWrite1Byte(Adapter, REG_TXPAUSE, 0xFF);
	ODM_Write1Byte(pDM_Odm,REG_TXPAUSE, 0xFF);
	
	//Force RX to stop TX immediately
	//PHY_SetRFReg(Adapter, RF_PATH_A, RF_AC, bRFRegOffsetMask, 0x32E13);

	ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_AC, bRFRegOffsetMask, 0x32E13);
	//1 Turn off RX
	//Rx AGC off  RegC70[0]=0, RegC7C[20]=0
	//PHY_SetBBReg(Adapter, 0xC70, BIT0, 0);
	//PHY_SetBBReg(Adapter, 0xC7C, BIT20, 0);

	ODM_SetBBReg(pDM_Odm, 0xC70, BIT0, 0);
	ODM_SetBBReg(pDM_Odm, 0xC7C, BIT20, 0);

	
	//Turn off CCA
	//PHY_SetBBReg(Adapter, 0xC14, bMaskDWord, 0x0);
	ODM_SetBBReg(pDM_Odm, 0xC14, bMaskDWord, 0x0);
	
	//BB Reset
	//BBReset = PlatformEFIORead1Byte(Adapter, 0x02);
	BBReset = ODM_Read1Byte(pDM_Odm, 0x02);
	
	//PlatformEFIOWrite1Byte(Adapter, 0x02, BBReset&(~BIT0));
	//PlatformEFIOWrite1Byte(Adapter, 0x02, BBReset|BIT0);

	ODM_Write1Byte(pDM_Odm,  0x02, BBReset&(~BIT0));
	ODM_Write1Byte(pDM_Odm,  0x02, BBReset|BIT0);
	
	//1 Leave RX idle low power
	//PHY_SetBBReg(Adapter, 0x818, BIT28, 0x0);

	ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 0x0);
	//1 Fix initial gain
	//if (IS_HARDWARE_TYPE_8723AE(Adapter))
	//RSSI_BT = pHalData->RSSI_BT;
       //else if((IS_HARDWARE_TYPE_8192C(Adapter))||(IS_HARDWARE_TYPE_8192D(Adapter)))      // Add by Gary
       //    RSSI_BT = RSSI_BT_new;

	//if((pDM_Odm->SupportICType==ODM_RTL8723A)&(pDM_Odm->SupportInterface==ODM_ITRF_PCIE))
	RSSI_BT=pDM_Odm->RSSI_BT;		//need to check C2H to pDM_Odm RSSI BT

	

	   
	ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: RSSI_BT= %d\n", RSSI_BT));
	
	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
	       //Neil add--2011--10--12
		//2 Initial Gain index 
		if(RSSI_BT >=35)   // >= -15dBm
			initial_gain_psd = RSSI_BT*2;
		else if((RSSI_BT >=33)&(RSSI_BT<35))
			initial_gain_psd = RSSI_BT*2+6;
		else if((RSSI_BT >=24)&(RSSI_BT<33))
			initial_gain_psd = 70-(33-RSSI_BT);
	 	else if((RSSI_BT >=19)&(RSSI_BT<24))
			initial_gain_psd = 64-((24-RSSI_BT)*4);
		else if((RSSI_BT >=14)&(RSSI_BT<19))
			initial_gain_psd = 44-((18-RSSI_BT)*2);
		else if((RSSI_BT >=8)&(RSSI_BT<14))
			initial_gain_psd = 35-(14-RSSI_BT);
		else
			initial_gain_psd = 0x1B;
	}
	else
	{
	
		//need to do	
         	initial_gain_psd = pDM_Odm->RSSI_Min;    // PSD report based on RSSI
           	//}  	
	}
	//if(RSSI_BT<0x17)
	//	RSSI_BT +=3;
	//DbgPrint("PSD: RSSI_BT= %d\n", RSSI_BT);
	ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: RSSI_BT= %d\n", RSSI_BT));

	//initialGainUpper = 0x5E;  //Modify by neil chen
	
	if(pDM_Odm->bUserAssignLevel)
	{
		pDM_Odm->bUserAssignLevel = FALSE;
		initialGainUpper = 0x7f;
	}
	else
	{
		initialGainUpper = 0x5E;
	}
	
	/*
	if (initial_gain_psd < 0x1a)
		initial_gain_psd = 0x1a;
	if (initial_gain_psd > initialGainUpper)
		initial_gain_psd = initialGainUpper;
	*/

	//if(pDM_Odm->SupportICType==ODM_RTL8723A)
	SSBT = RSSI_BT  * 2 +0x3E;
	
	
	//if(IS_HARDWARE_TYPE_8723AE(Adapter))
	//	SSBT = RSSI_BT  * 2 +0x3E;
	//else if((IS_HARDWARE_TYPE_8192C(Adapter))||(IS_HARDWARE_TYPE_8192D(Adapter)))   // Add by Gary
	//{
	//	RSSI_BT = initial_gain_psd;
	//	SSBT = RSSI_BT;
	//}
	ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: SSBT= %d\n", SSBT));
	ODM_RT_TRACE(	pDM_Odm,COMP_PSD, DBG_LOUD,("PSD: initial gain= 0x%x\n", initial_gain_psd));
	//DbgPrint("PSD: SSBT= %d", SSBT);
	//need to do
	//pMgntInfo->bDMInitialGainEnable = FALSE;
	pDM_Odm->bDMInitialGainEnable = FALSE;
	initial_gain = ODM_GetBBReg(pDM_Odm, 0xc50, bMaskDWord) & 0x7F;
	ODM_SetBBReg(pDM_Odm, 0xc50, 0x7F, initial_gain_psd);	
	//1 Turn off 3-wire
	ODM_SetBBReg(pDM_Odm, 0x88c, BIT20|BIT21|BIT22|BIT23, 0xF);

	//pts value = 128, 256, 512, 1024
	pts = 128;

	if(pts == 128)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x0);
		start_point = 64;
		stop_point = 192;
	}
	else if(pts == 256)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x1);
		start_point = 128;
		stop_point = 384;
	}
	else if(pts == 512)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x2);
		start_point = 256;
		stop_point = 768;
	}
	else
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x3);
		start_point = 512;
		stop_point = 1536;
	}
	

//3 Skip WLAN channels if WLAN busy

	curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast) - lastTxOkCnt;
	curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast) - lastRxOkCnt;
	lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
	lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);	

	PSD_skip_start=80;
	PSD_skip_stop = 0;
	wlan_channel = CurrentChannel & 0x0f;

	ODM_RT_TRACE(pDM_Odm,COMP_PSD,DBG_LOUD,("PSD: current channel: %x, BW:%d \n", wlan_channel, Is40MHz));
	if(pDM_Odm->SupportICType==ODM_RTL8723A)
	{
#if defined(BT_30_SUPPORT) && (BT_30_SUPPORT == 1)	
		if(pDM_Odm->bBtHsOperation)
		{
			if(pDM_Odm->bLinked)
			{
				if(Is40MHz)
				{
					PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-2;  // Modify by Neil to add 10 chs to mask
					PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+4;
				}
				else
				{
					PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-10;  // Modify by Neil to add 10 chs to mask
					PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+18; 
				}
			}
			else
			{
				// mask for 40MHz
				PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-2;  // Modify by Neil to add 10 chs to mask
				PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+4;
			}
			if(PSD_skip_start < 0)
				PSD_skip_start = 0;
			if(PSD_skip_stop >80)
				PSD_skip_stop = 80;
		}
		else
#endif
		{
			if((curRxOkCnt+curTxOkCnt) > 5)
			{
				if(Is40MHz)
				{
					PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-2;  // Modify by Neil to add 10 chs to mask
					PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+4;
				}
				else
				{
					PSD_skip_start = ((wlan_channel-1)*5 -Is40MHz*10)-10;  // Modify by Neil to add 10 chs to mask
					PSD_skip_stop = (PSD_skip_start + (1+Is40MHz)*20)+18; 
				}
				
				if(PSD_skip_start < 0)
					PSD_skip_start = 0;
				if(PSD_skip_stop >80)
					PSD_skip_stop = 80;
			}
		}
	}
	else
	{
		if((curRxOkCnt+curTxOkCnt) > 1000)
		{
			PSD_skip_start = (wlan_channel-1)*5 -Is40MHz*10;
			PSD_skip_stop = PSD_skip_start + (1+Is40MHz)*20;
		}
	}   

	ODM_RT_TRACE(pDM_Odm,COMP_PSD,DBG_LOUD,("PSD: Skip tone from %d to %d \n", PSD_skip_start, PSD_skip_stop));

 	for (n=0;n<80;n++)
 	{
 		if((n%20)==0)
 		{
			channel = (n/20)*4 + 1;
			/*
			if(pDM_Odm->SupportICType==ODM_RTL8192D)
			{
				switch(channel)
				{
					case 1: 
					case 9:
						group_idx = 0;
						break;
					case 5:
						group_idx = 2;
						break;
					case 13:
				 		group_idx = 1;
						break;
				}
				
				if((pHalData->MacPhyMode92D == SINGLEMAC_SINGLEPHY)||(pHalData->MacPhyMode92D == DUALMAC_SINGLEPHY))   
				{
					for(i = 0; i < SYN_Length; i++)
						ODM_SetRFReg(pDM_Odm, RF_PATH_B, SYN[i], bMaskDWord, SYN_group[group_idx][i]);

					ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
					ODM_SetRFReg(pDM_Odm, RF_PATH_B, RF_CHNLBW, 0x3FF, channel);
				}
				else  // DualMAC_DualPHY 2G
				{
					for(i = 0; i < SYN_Length; i++)
						ODM_SetRFReg(pDM_Odm, RF_PATH_A, SYN[i], bMaskDWord, SYN_group[group_idx][i]);   
					
					ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
				}
			}
			else */
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
		}
		tone_idx = n%20;
		if ((n>=PSD_skip_start) && (n<PSD_skip_stop))
		{	
			PSD_report[n] = SSBT;
			ODM_RT_TRACE(pDM_Odm,COMP_PSD,DBG_LOUD,("PSD:Tone %d skipped \n", n));
		}
		else
		{
			PSD_report_tmp =  GetPSDData(pDM_Odm, idx[tone_idx], initial_gain_psd);

			if ( PSD_report_tmp > PSD_report[n])
				PSD_report[n] = PSD_report_tmp;
				
		}
	}

	PatchDCTone(pDM_Odm, PSD_report, initial_gain_psd);
      
       //----end
	//1 Turn on RX
	//Rx AGC on
	ODM_SetBBReg(pDM_Odm, 0xC70, BIT0, 1);
	ODM_SetBBReg(pDM_Odm, 0xC7C, BIT20, 1);
	//CCK on
	ODM_SetBBReg(pDM_Odm, rFPGA0_RFMOD, BIT24, 1);
	//1 Turn on TX
	//Resume TX Queue
	
	ODM_Write1Byte(pDM_Odm,REG_TXPAUSE, 0x00);
	//Turn on 3-wire
	ODM_SetBBReg(pDM_Odm, 0x88c, BIT20|BIT21|BIT22|BIT23, 0x0);
	//1 Restore Current Settings
	//Resume DIG
	pDM_Odm->bDMInitialGainEnable = TRUE;
	if(!pDM_Odm->bBtHsOperation)
	{
	ODM_SetBBReg(pDM_Odm, 0xc50, 0x7F, initial_gain);
	}
	// restore originl center frequency
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, CurrentChannel);

	/*
	if(pDM_Odm->SupportICType==ODM_RTL8192D)
	{
		if((pHalData->MacPhyMode92D == SINGLEMAC_SINGLEPHY)||(pHalData->MacPhyMode92D == DUALMAC_SINGLEPHY))
		{
			PHY_SetRFReg(Adapter, RF_PATH_B, RF_CHNLBW, bMaskDWord, CurrentChannel);
			PHY_SetRFReg(Adapter, RF_PATH_B, 0x25, bMaskDWord, SYN_RF25);
			PHY_SetRFReg(Adapter, RF_PATH_B, 0x26, bMaskDWord, SYN_RF26);
			PHY_SetRFReg(Adapter, RF_PATH_B, 0x27, bMaskDWord, SYN_RF27);
			PHY_SetRFReg(Adapter, RF_PATH_B, 0x2B, bMaskDWord, SYN_RF2B);
			PHY_SetRFReg(Adapter, RF_PATH_B, 0x2C, bMaskDWord, SYN_RF2C);
		}
		else     // DualMAC_DualPHY
		{
			PHY_SetRFReg(Adapter, RF_PATH_A, 0x25, bMaskDWord, SYN_RF25);
			PHY_SetRFReg(Adapter, RF_PATH_A, 0x26, bMaskDWord, SYN_RF26);
			PHY_SetRFReg(Adapter, RF_PATH_A, 0x27, bMaskDWord, SYN_RF27);
			PHY_SetRFReg(Adapter, RF_PATH_A, 0x2B, bMaskDWord, SYN_RF2B);
			PHY_SetRFReg(Adapter, RF_PATH_A, 0x2C, bMaskDWord, SYN_RF2C);
		}
	}*/
	//Turn on CCA
	ODM_SetBBReg(pDM_Odm, 0xC14, bMaskDWord, RXIQI);
	//Restore RX idle low power
	if(RxIdleLowPwr == TRUE)
		ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 1);
	
	psd_cnt++;
	ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD:psd_cnt = %d \n",psd_cnt));
	if (psd_cnt < ReScan)
		ODM_SetTimer(pDM_Odm, &pDM_Odm->PSDTimer, Interval);		
	else
	{
		psd_cnt = 0;
		for(i=0;i<80;i++)
			//DbgPrint("psd_report[%d]=     %d \n", 2402+i, PSD_report[i]);
			RT_TRACE(	COMP_PSD, DBG_LOUD,("psd_report[%d]=     %d \n", 2402+i, PSD_report[i]));


		GoodChannelDecision(pDM_Odm, PSD_report, PSD_bitmap,RSSI_BT, PSD_bitmap_memory);

		if(pDM_Odm->SupportICType==ODM_RTL8723A)
		{
			cur_byte_idx=0;
			cur_bit_idx=0;

			//2 Restore H2C PSD Data to Last Data
		  	H2C_PSD_DATA_last[0] = H2C_PSD_DATA[0];
			H2C_PSD_DATA_last[1] = H2C_PSD_DATA[1];
			H2C_PSD_DATA_last[2] = H2C_PSD_DATA[2];
			H2C_PSD_DATA_last[3] = H2C_PSD_DATA[3];
			H2C_PSD_DATA_last[4] = H2C_PSD_DATA[4];

	
			//2 Translate 80bit channel map to 40bit channel	
			for ( i=0;i<5;i++)
			{
				for(n=0;n<8;n++)
				{
					cur_byte_idx = i*2 + n/4;
					cur_bit_idx = (n%4)*2;
					if ( ((PSD_bitmap[cur_byte_idx]& BIT(cur_bit_idx)) != 0) && ((PSD_bitmap[cur_byte_idx]& BIT(cur_bit_idx+1)) != 0))
						H2C_PSD_DATA[i] = H2C_PSD_DATA[i] | (u1Byte) (1 << n);
				}
				ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("H2C_PSD_DATA[%d]=0x%x\n" ,i, H2C_PSD_DATA[i]));
			}
	
			//3 To Compare the difference
			for ( i=0;i<5;i++)
			{
				if(H2C_PSD_DATA[i] !=H2C_PSD_DATA_last[i])
				{
					FillH2CCmd(Adapter, H2C_92C_PSD_RESULT, 5, H2C_PSD_DATA);
					ODM_RT_TRACE(pDM_Odm, COMP_PSD, DBG_LOUD,("Need to Update the AFH Map \n"));
					break;
				}
				else
				{
					if(i==5)
						ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("Not need to Update\n"));	
				}
			}
			//pHalData->bPSDactive=FALSE;
			if(pDM_Odm->bBtHsOperation)
			{
				ODM_SetTimer(pDM_Odm, &pDM_Odm->PSDTimer, 10000);
				ODM_RT_TRACE(	pDM_Odm,COMP_PSD, DBG_LOUD,("Leave dm_PSD_Monitor\n"));		
			}
			else
			{
			ODM_SetTimer(pDM_Odm, &pDM_Odm->PSDTimer, 900);
			ODM_RT_TRACE(	pDM_Odm,COMP_PSD, DBG_LOUD,("Leave dm_PSD_Monitor\n"));	
		}
	}
    }
}
/*
//Neil for Get BT RSSI
// Be Triggered by BT C2H CMD
VOID
ODM_PSDGetRSSI(
	IN	u1Byte	RSSI_BT)
{


}

*/

VOID
ODM_PSDMonitor(
	IN	PDM_ODM_T	pDM_Odm
	)
{
	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	
	//if(IS_HARDWARE_TYPE_8723AE(Adapter))
	
	if(pDM_Odm->SupportICType == ODM_RTL8723A)   //may need to add other IC type
	{
		if(pDM_Odm->SupportInterface==ODM_ITRF_PCIE)
		{
#if defined(BT_30_SUPPORT) && (BT_30_SUPPORT == 1)	
			if(pDM_Odm->bBtDisabled) //need to check upper layer connection
			{
				pDM_Odm->bPSDactive=FALSE;
				ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD, ("odm_PSDMonitor, return for BT is disabled!!!\n"));
		   		return; 
			}
#endif
			ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD, ("odm_PSDMonitor\n"));
		//if(pHalData->bPSDactive ==FALSE)
		//{
			pDM_Odm->bPSDinProcess = TRUE;
	 		//pHalData->bPSDactive=TRUE;
	 		pDM_Odm->bPSDactive=TRUE;
			odm_PSD_Monitor(pDM_Odm);
			pDM_Odm->bPSDinProcess = FALSE;
		}	
	}	

}
VOID
odm_PSDMonitorCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
       HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;


#if USE_WORKITEM
	PlatformScheduleWorkItem(&pHalData->PSDMonitorWorkitem);
#else
	ODM_PSDMonitor(pDM_Odm);
#endif
}

VOID
odm_PSDMonitorWorkItemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	Adapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;


	ODM_PSDMonitor(pDM_Odm);
}



 //cosa debug tool need to modify

VOID
ODM_PSDDbgControl(
	IN	PADAPTER	Adapter,
	IN	u4Byte		mode,
	IN	u4Byte		btRssi
	)
{
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

	ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD, (" Monitor mode=%d, btRssi=%d\n", mode, btRssi));
	if(mode)
	{
		pDM_Odm->RSSI_BT = (u1Byte)btRssi;
		pDM_Odm->bUserAssignLevel = TRUE;
		ODM_SetTimer( pDM_Odm, &pDM_Odm->PSDTimer, 0); //ms		
	}
	else
	{
		ODM_CancelTimer(pDM_Odm, &pDM_Odm->PSDTimer);
	}
#endif
}


//#if(DEV_BUS_TYPE == RT_PCI_INTERFACE)|(DEV_BUS_TYPE == RT_USB_INTERFACE)

void	odm_RXHPInit(
	IN		PDM_ODM_T		pDM_Odm)
{
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE)|(DEV_BUS_TYPE == RT_USB_INTERFACE)
	pRXHP_T			pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
   	u1Byte			index;

	pRX_HP_Table->RXHP_enable = TRUE;
	pRX_HP_Table->RXHP_flag = 0;
	pRX_HP_Table->PSD_func_trigger = 0;
	pRX_HP_Table->Pre_IGI = 0x20;
	pRX_HP_Table->Cur_IGI = 0x20;
	pRX_HP_Table->Cur_pw_th = pw_th_10dB;
	pRX_HP_Table->Pre_pw_th = pw_th_10dB;
	for(index=0; index<80; index++)
		pRX_HP_Table->PSD_bitmap_RXHP[index] = 1;

#if(DEV_BUS_TYPE == RT_USB_INTERFACE)
	pRX_HP_Table->TP_Mode = Idle_Mode;
#endif
#endif
}

void odm_RXHP(
	IN		PDM_ODM_T		pDM_Odm)
{
#if( DM_ODM_SUPPORT_TYPE & (ODM_WIN))
#if (DEV_BUS_TYPE == RT_PCI_INTERFACE) | (DEV_BUS_TYPE == RT_USB_INTERFACE)
	PADAPTER	Adapter =  pDM_Odm->Adapter;
	PMGNT_INFO	pMgntInfo = &(Adapter->MgntInfo);
	pDIG_T		pDM_DigTable = &pDM_Odm->DM_DigTable;
	pRXHP_T		pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
       PFALSE_ALARM_STATISTICS		FalseAlmCnt = &(pDM_Odm->FalseAlmCnt);
	
	u1Byte              	i, j, sum;
	u1Byte			Is40MHz;
	s1Byte              	Intf_diff_idx, MIN_Intf_diff_idx = 16;   
       s4Byte              	cur_channel;    
       u1Byte              	ch_map_intf_5M[17] = {0};     
       static u4Byte		FA_TH = 0;	
	static u1Byte      	psd_intf_flag = 0;
	static s4Byte      	curRssi = 0;                
       static s4Byte  		preRssi = 0;                                                                
	static u1Byte		PSDTriggerCnt = 1;
	
	u1Byte			RX_HP_enable = (u1Byte)(ODM_GetBBReg(pDM_Odm, rOFDM0_XAAGCCore2, bMaskDWord)>>31);   // for debug!!

#if(DEV_BUS_TYPE == RT_USB_INTERFACE)	
	static s8Byte  		lastTxOkCnt = 0, lastRxOkCnt = 0;  
       s8Byte			curTxOkCnt, curRxOkCnt;
	s8Byte			curTPOkCnt;
	s8Byte			TP_Acc3, TP_Acc5;
	static s8Byte		TP_Buff[5] = {0};
	static u1Byte		pre_state = 0, pre_state_flag = 0;
	static u1Byte		Intf_HighTP_flag = 0, De_counter = 16; 
	static u1Byte		TP_Degrade_flag = 0;
#endif	   
	static u1Byte		LatchCnt = 0;
	
     	if((pDM_Odm->SupportICType == ODM_RTL8723A)||(pDM_Odm->SupportICType == ODM_RTL8188E))
		return;
	//AGC RX High Power Mode is only applied on 2G band in 92D!!!
	if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		if(*(pDM_Odm->pBandType) != ODM_BAND_2_4G)
			return;
	}

	if(!(pDM_Odm->SupportAbility==ODM_BB_RXHP))
		return;


	//RX HP ON/OFF
	if(RX_HP_enable == 1)
		pRX_HP_Table->RXHP_enable = FALSE;
	else
		pRX_HP_Table->RXHP_enable = TRUE;

	if(pRX_HP_Table->RXHP_enable == FALSE)
	{
		if(pRX_HP_Table->RXHP_flag == 1)
		{
			pRX_HP_Table->RXHP_flag = 0;
			psd_intf_flag = 0;
		}
		return;
	}

#if(DEV_BUS_TYPE == RT_USB_INTERFACE)	
	//2 Record current TP for USB interface
	curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast)-lastTxOkCnt;
	curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast)-lastRxOkCnt;
	lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
	lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);

	curTPOkCnt = curTxOkCnt+curRxOkCnt;
	TP_Buff[0] = curTPOkCnt;    // current TP  
	TP_Acc3 = PlatformDivision64((TP_Buff[1]+TP_Buff[2]+TP_Buff[3]), 3);
	TP_Acc5 = PlatformDivision64((TP_Buff[0]+TP_Buff[1]+TP_Buff[2]+TP_Buff[3]+TP_Buff[4]), 5);
	
	if(TP_Acc5 < 1000)
		pRX_HP_Table->TP_Mode = Idle_Mode;
	else if((1000 < TP_Acc5)&&(TP_Acc5 < 3750000))
		pRX_HP_Table->TP_Mode = Low_TP_Mode;
	else
		pRX_HP_Table->TP_Mode = High_TP_Mode;

	ODM_RT_TRACE(pDM_Odm, 	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RX HP TP Mode = %d\n", pRX_HP_Table->TP_Mode));
	// Since TP result would be sampled every 2 sec, it needs to delay 4sec to wait PSD processing.
	// When LatchCnt = 0, we would Get PSD result.
	if(TP_Degrade_flag == 1)
	{
		LatchCnt--;
		if(LatchCnt == 0)
		{
			TP_Degrade_flag = 0;
		}
	}
	// When PSD function triggered by TP degrade 20%, and Interference Flag = 1
	// Set a De_counter to wait IGI = upper bound. If time is UP, the Interference flag will be pull down.
	if(Intf_HighTP_flag == 1)
	{
		De_counter--;
		if(De_counter == 0)
		{
			Intf_HighTP_flag = 0;
			psd_intf_flag = 0;
		}
	}
#endif

	//2 AGC RX High Power Mode by PSD only applied to STA Mode
	//3 NOT applied 1. Ad Hoc Mode.
	//3 NOT applied 2. AP Mode
	if ((pMgntInfo->mAssoc) && (!pMgntInfo->mIbss) && (!ACTING_AS_AP(Adapter)))
	{    
		Is40MHz = *(pDM_Odm->pBandWidth);
		curRssi = pDM_Odm->RSSI_Min;
		cur_channel = ODM_GetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x0fff) & 0x0f;
		ODM_RT_TRACE(pDM_Odm, 	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP RX HP flag = %d\n", pRX_HP_Table->RXHP_flag));
		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP FA = %d\n", FalseAlmCnt->Cnt_all));
		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP cur RSSI = %d, pre RSSI=%d\n", curRssi, preRssi));
		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP current CH = %d\n", cur_channel));
		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RXHP Is 40MHz = %d\n", Is40MHz));
       	//2 PSD function would be triggered 
       	//3 1. Every 4 sec for PCIE
       	//3 2. Before TP Mode (Idle TP<4kbps) for USB
       	//3 3. After TP Mode (High TP) for USB 
		if((curRssi > 68) && (pRX_HP_Table->RXHP_flag == 0))	// Only RSSI>TH and RX_HP_flag=0 will Do PSD process 
		{
#if (DEV_BUS_TYPE == RT_USB_INTERFACE)
			//2 Before TP Mode ==> PSD would be trigger every 4 sec
			if(pRX_HP_Table->TP_Mode == Idle_Mode)		//2.1 less wlan traffic <4kbps
			{
#endif
				if(PSDTriggerCnt == 1)       
				{    	
					odm_PSD_RXHP(pDM_Odm);
					pRX_HP_Table->PSD_func_trigger = 1;
					PSDTriggerCnt = 0;
				}
				else
				{
             				PSDTriggerCnt++;
				}
#if(DEV_BUS_TYPE == RT_USB_INTERFACE)
			}	
			//2 After TP Mode ==> Check if TP degrade larger than 20% would trigger PSD function
			if(pRX_HP_Table->TP_Mode == High_TP_Mode)
			{
				if((pre_state_flag == 0)&&(LatchCnt == 0)) 
				{
					// TP var < 5%
					if((((curTPOkCnt-TP_Acc3)*20)<(TP_Acc3))&&(((curTPOkCnt-TP_Acc3)*20)>(-TP_Acc3)))
					{
						pre_state++;
						if(pre_state == 3)      // hit pre_state condition => consecutive 3 times
						{
							pre_state_flag = 1;
							pre_state = 0;
						}

					}
					else
					{
						pre_state = 0;
					}
				}
				//3 If pre_state_flag=1 ==> start to monitor TP degrade 20%
				if(pre_state_flag == 1)		
				{
					if(((TP_Acc3-curTPOkCnt)*5)>(TP_Acc3))      // degrade 20%
					{
						odm_PSD_RXHP(pDM_Odm);
						pRX_HP_Table->PSD_func_trigger = 1;
						TP_Degrade_flag = 1;
						LatchCnt = 2;
						pre_state_flag = 0;
					}
					else if(((TP_Buff[2]-curTPOkCnt)*5)>TP_Buff[2])
					{
						odm_PSD_RXHP(pDM_Odm);
						pRX_HP_Table->PSD_func_trigger = 1;
						TP_Degrade_flag = 1;
						LatchCnt = 2;
						pre_state_flag = 0;
					}
					else if(((TP_Buff[3]-curTPOkCnt)*5)>TP_Buff[3])
					{
						odm_PSD_RXHP(pDM_Odm);
						pRX_HP_Table->PSD_func_trigger = 1;
						TP_Degrade_flag = 1;
						LatchCnt = 2;
						pre_state_flag = 0;
					}
				}
			}
#endif
}

#if (DEV_BUS_TYPE == RT_USB_INTERFACE)
		for (i=0;i<4;i++)
		{
			TP_Buff[4-i] = TP_Buff[3-i];
		}
#endif
		//2 Update PSD bitmap according to PSD report 
		if((pRX_HP_Table->PSD_func_trigger == 1)&&(LatchCnt == 0))
    		{	
           		//2 Separate 80M bandwidth into 16 group with smaller 5M BW.
			for (i = 0 ; i < 16 ; i++)
           		{
				sum = 0;
				for(j = 0; j < 5 ; j++)
                			sum += pRX_HP_Table->PSD_bitmap_RXHP[5*i + j];
            
                		if(sum < 5)
                		{
                			ch_map_intf_5M[i] = 1;  // interference flag
                		}
           		}
			//=============just for debug=========================
			//for(i=0;i<16;i++)
				//DbgPrint("RX HP: ch_map_intf_5M[%d] = %d\n", i, ch_map_intf_5M[i]);
			//===============================================
			//2 Mask target channel 5M index
	    		for(i = 0; i < (4+4*Is40MHz) ; i++)
           		{
				ch_map_intf_5M[cur_channel - (1+2*Is40MHz) + i] = 0;  
           		}
				
           		psd_intf_flag = 0;
	    		for(i = 0; i < 16; i++)
           		{
         			if(ch_map_intf_5M[i] == 1)
	              	{
	              		psd_intf_flag = 1;            // interference is detected!!!	
	              		break;
         			}
	    		}
				
#if (DEV_BUS_TYPE == RT_USB_INTERFACE)
			if(pRX_HP_Table->TP_Mode!=Idle_Mode)
			{
				if(psd_intf_flag == 1)     // to avoid psd_intf_flag always 1
				{
					Intf_HighTP_flag = 1;
					De_counter = 32;     // 0x1E -> 0x3E needs 32 times by each IGI step =1
				}
			}
#endif
			ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RX HP psd_intf_flag = %d\n", psd_intf_flag));
			//2 Distance between target channel and interference
           		for(i = 0; i < 16; i++)
          		{
				if(ch_map_intf_5M[i] == 1)
                		{
					Intf_diff_idx = ((cur_channel+Is40MHz-(i+1))>0) ? (s1Byte)(cur_channel-2*Is40MHz-(i-2)) : (s1Byte)((i+1)-(cur_channel+2*Is40MHz));  
                      		if(Intf_diff_idx < MIN_Intf_diff_idx)
						MIN_Intf_diff_idx = Intf_diff_idx;    // the min difference index between interference and target
		  		}
	    		}
	    		ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RX HP MIN_Intf_diff_idx = %d\n", MIN_Intf_diff_idx)); 
			//2 Choose False Alarm Threshold
			switch (MIN_Intf_diff_idx){
      				case 0: 
	   			case 1:
	        		case 2:
	        		case 3:	 	 
                 			FA_TH = FA_RXHP_TH1;  
                     		break;
	        		case 4:				// CH5
	        		case 5:				// CH6
		   			FA_TH = FA_RXHP_TH2;	
               			break;
	        		case 6:				// CH7
	        		case 7:				// CH8
		      			FA_TH = FA_RXHP_TH3;
                    			break; 
               		case 8:				// CH9
	        		case 9:				//CH10
		      			FA_TH = FA_RXHP_TH4;
                    			break; 	
	        		case 10:
	        		case 11:
	        		case 12:
	        		case 13:	 
	        		case 14:
	      			case 15:	 	
		      			FA_TH = FA_RXHP_TH5;
                    			break;  		
       		}	
			ODM_RT_TRACE(pDM_Odm,	ODM_COMP_RXHP, ODM_DBG_LOUD, ("RX HP FA_TH = %d\n", FA_TH));
			pRX_HP_Table->PSD_func_trigger = 0;
		}
		//1 Monitor RSSI variation to choose the suitable IGI or Exit AGC RX High Power Mode
         	if(pRX_HP_Table->RXHP_flag == 1)
         	{
              	if ((curRssi > 80)&&(preRssi < 80))
              	{ 
                   		pRX_HP_Table->Cur_IGI = LNA_Low_Gain_1;
              	}
              	else if ((curRssi < 80)&&(preRssi > 80))
              	{
                   		pRX_HP_Table->Cur_IGI = LNA_Low_Gain_2;
			}
	       	else if ((curRssi > 72)&&(preRssi < 72))
	      		{
                		pRX_HP_Table->Cur_IGI = LNA_Low_Gain_2;
	       	}
              	else if ((curRssi < 72)&&( preRssi > 72))
	     		{
                   		pRX_HP_Table->Cur_IGI = LNA_Low_Gain_3;
	       	}
	       	else if (curRssi < 68)		 //RSSI is NOT large enough!!==> Exit AGC RX High Power Mode
	       	{
                   		pRX_HP_Table->Cur_pw_th = pw_th_10dB;
				pRX_HP_Table->RXHP_flag = 0;    // Back to Normal DIG Mode		  
				psd_intf_flag = 0;
			}
		}
		else    // pRX_HP_Table->RXHP_flag == 0
		{
			//1 Decide whether to enter AGC RX High Power Mode
			if ((curRssi > 70) && (psd_intf_flag == 1) && (FalseAlmCnt->Cnt_all > FA_TH) &&  
				(pDM_DigTable->CurIGValue == pDM_DigTable->rx_gain_range_max))
			{
             			if (curRssi > 80)
             			{
					pRX_HP_Table->Cur_IGI = LNA_Low_Gain_1;
				}
				else if (curRssi > 72) 
              		{
               			pRX_HP_Table->Cur_IGI = LNA_Low_Gain_2;
				}
             			else
            			{
                   			pRX_HP_Table->Cur_IGI = LNA_Low_Gain_3;
				}
           			pRX_HP_Table->Cur_pw_th = pw_th_16dB;		//RegC54[9:8]=2'b11: to enter AGC Flow 3
				pRX_HP_Table->First_time_enter = TRUE;
				pRX_HP_Table->RXHP_flag = 1;    //	RXHP_flag=1: AGC RX High Power Mode, RXHP_flag=0: Normal DIG Mode
			}
		}
		preRssi = curRssi; 
		odm_Write_RXHP(pDM_Odm);	
	}
#endif //#if( DM_ODM_SUPPORT_TYPE & (ODM_WIN))
#endif //#if (DEV_BUS_TYPE == RT_PCI_INTERFACE) | (DEV_BUS_TYPE == RT_USB_INTERFACE)
}

void odm_Write_RXHP(
	IN	PDM_ODM_T	pDM_Odm)
{
	pRXHP_T		pRX_HP_Table = &pDM_Odm->DM_RXHP_Table;
	u4Byte		currentIGI;

	if(pRX_HP_Table->Cur_IGI != pRX_HP_Table->Pre_IGI)
	{
		ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskByte0, pRX_HP_Table->Cur_IGI);
	     	ODM_SetBBReg(pDM_Odm, rOFDM0_XBAGCCore1, bMaskByte0, pRX_HP_Table->Cur_IGI);	
	}
	
	if(pRX_HP_Table->Cur_pw_th != pRX_HP_Table->Pre_pw_th)
{
		ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore2, BIT8|BIT9, pRX_HP_Table->Cur_pw_th);  // RegC54[9:8]=2'b11:  AGC Flow 3
	}

	if(pRX_HP_Table->RXHP_flag == 0)
	{
		pRX_HP_Table->Cur_IGI = 0x20;
	}
	else
	{
		currentIGI = ODM_GetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskByte0);
		if(currentIGI<0x50)
		{
			ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskByte0, pRX_HP_Table->Cur_IGI);
	     		ODM_SetBBReg(pDM_Odm, rOFDM0_XBAGCCore1, bMaskByte0, pRX_HP_Table->Cur_IGI);	
		}
	}
	pRX_HP_Table->Pre_IGI = pRX_HP_Table->Cur_IGI;
	pRX_HP_Table->Pre_pw_th = pRX_HP_Table->Cur_pw_th;

}

VOID
odm_PSD_RXHP(
	IN	PDM_ODM_T	pDM_Odm
)
{
	pRXHP_T			pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
	PADAPTER		Adapter =  pDM_Odm->Adapter;
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	unsigned int 		pts, start_point, stop_point, initial_gain ;
	static u1Byte		PSD_bitmap_memory[80], init_memory = 0;
	static u1Byte 		psd_cnt=0;
	static u4Byte		PSD_report[80], PSD_report_tmp;
	static u8Byte		lastTxOkCnt=0, lastRxOkCnt=0;
	u1Byte			idx[20]={96,99,102,106,109,112,115,118,122,125,
					0,3,6,10,13,16,19,22,26,29};
	u1Byte			n, i, channel, BBReset,tone_idx;
	u1Byte			PSD_bitmap[10], SSBT=0,initial_gain_psd=0, RSSI_BT=0, initialGainUpper;
	s4Byte    			PSD_skip_start, PSD_skip_stop;
	u4Byte			CurrentChannel, RXIQI, RxIdleLowPwr, wlan_channel;
	u4Byte			ReScan, Interval, Is40MHz;
	u8Byte			curTxOkCnt, curRxOkCnt;
	//--------------2G band synthesizer for 92D switch RF channel using----------------- 
	u1Byte			group_idx=0;
	u4Byte			SYN_RF25=0, SYN_RF26=0, SYN_RF27=0, SYN_RF2B=0, SYN_RF2C=0;
	u4Byte			SYN[5] = {0x25, 0x26, 0x27, 0x2B, 0x2C};    // synthesizer RF register for 2G channel
	u4Byte			SYN_group[3][5] = {{0x643BC, 0xFC038, 0x77C1A, 0x41289, 0x01840},     // For CH1,2,4,9,10.11.12   {0x643BC, 0xFC038, 0x77C1A, 0x41289, 0x01840}
									    {0x643BC, 0xFC038, 0x07C1A, 0x41289, 0x01840},     // For CH3,13,14
									    {0x243BC, 0xFC438, 0x07C1A, 0x4128B, 0x0FC41}};   // For Ch5,6,7,8
       //--------------------- Add by Gary for Debug setting ----------------------
       s4Byte                 psd_result = 0;
	u1Byte                 RSSI_BT_new = (u1Byte) ODM_GetBBReg(pDM_Odm, 0xB9C, 0xFF);
       u1Byte                 rssi_ctrl = (u1Byte) ODM_GetBBReg(pDM_Odm, 0xB38, 0xFF);
       //---------------------------------------------------------------------
	
	if(pMgntInfo->bScanInProgress)
	{
		return;
	}

	ReScan = PSD_RESCAN;
	Interval = SCAN_INTERVAL;


	//1 Initialization
	if(init_memory == 0)
	{
		RT_TRACE(	COMP_PSD, DBG_LOUD,("Init memory\n"));
		for(i = 0; i < 80; i++)
			PSD_bitmap_memory[i] = 0xFF; // channel is always good
		init_memory = 1;
	}
	if(psd_cnt == 0)
	{
		RT_TRACE(COMP_PSD, DBG_LOUD,("Enter dm_PSD_Monitor\n"));
		for(i = 0; i < 80; i++)
			PSD_report[i] = 0;
	}

	//1 Backup Current Settings
	CurrentChannel = ODM_GetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask);
	if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		//2 Record Current synthesizer parameters based on current channel
		if((*(pDM_Odm->pMacPhyMode)==ODM_SMSP)||(*(pDM_Odm->pMacPhyMode)==ODM_DMSP))
		{
			SYN_RF25 = ODM_GetRFReg(pDM_Odm, RF_PATH_B, 0x25, bMaskDWord);
			SYN_RF26 = ODM_GetRFReg(pDM_Odm, RF_PATH_B, 0x26, bMaskDWord);
			SYN_RF27 = ODM_GetRFReg(pDM_Odm, RF_PATH_B, 0x27, bMaskDWord);
			SYN_RF2B = ODM_GetRFReg(pDM_Odm, RF_PATH_B, 0x2B, bMaskDWord);
			SYN_RF2C = ODM_GetRFReg(pDM_Odm, RF_PATH_B, 0x2C, bMaskDWord);
       	}
		else     // DualMAC_DualPHY 2G
		{
			SYN_RF25 = ODM_GetRFReg(pDM_Odm, RF_PATH_A, 0x25, bMaskDWord);
			SYN_RF26 = ODM_GetRFReg(pDM_Odm, RF_PATH_A, 0x26, bMaskDWord);
			SYN_RF27 = ODM_GetRFReg(pDM_Odm, RF_PATH_A, 0x27, bMaskDWord);
			SYN_RF2B = ODM_GetRFReg(pDM_Odm, RF_PATH_A, 0x2B, bMaskDWord);
			SYN_RF2C = ODM_GetRFReg(pDM_Odm, RF_PATH_A, 0x2C, bMaskDWord);
		}
	}
	RXIQI = ODM_GetBBReg(pDM_Odm, 0xC14, bMaskDWord);
	RxIdleLowPwr = (ODM_GetBBReg(pDM_Odm, 0x818, bMaskDWord)&BIT28)>>28;
	Is40MHz = *(pDM_Odm->pBandWidth);
	ODM_RT_TRACE(pDM_Odm,	COMP_PSD, DBG_LOUD,("PSD Scan Start\n"));
	//1 Turn off CCK
	ODM_SetBBReg(pDM_Odm, rFPGA0_RFMOD, BIT24, 0);
	//1 Turn off TX
	//Pause TX Queue
	ODM_Write1Byte(pDM_Odm, REG_TXPAUSE, 0xFF);
	//Force RX to stop TX immediately
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_AC, bRFRegOffsetMask, 0x32E13);
	//1 Turn off RX
	//Rx AGC off  RegC70[0]=0, RegC7C[20]=0
	ODM_SetBBReg(pDM_Odm, 0xC70, BIT0, 0);
	ODM_SetBBReg(pDM_Odm, 0xC7C, BIT20, 0);
	//Turn off CCA
	ODM_SetBBReg(pDM_Odm, 0xC14, bMaskDWord, 0x0);
	//BB Reset
	BBReset = ODM_Read1Byte(pDM_Odm, 0x02);
	ODM_Write1Byte(pDM_Odm, 0x02, BBReset&(~BIT0));
	ODM_Write1Byte(pDM_Odm, 0x02, BBReset|BIT0);
	//1 Leave RX idle low power
	ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 0x0);
	//1 Fix initial gain
      	RSSI_BT = RSSI_BT_new;
	RT_TRACE(COMP_PSD, DBG_LOUD,("PSD: RSSI_BT= %d\n", RSSI_BT));
	
	if(rssi_ctrl == 1)        // just for debug!!
		initial_gain_psd = RSSI_BT_new; 
     	else
		initial_gain_psd = pDM_Odm->RSSI_Min;    // PSD report based on RSSI
	
	RT_TRACE(COMP_PSD, DBG_LOUD,("PSD: RSSI_BT= %d\n", RSSI_BT));
	
	initialGainUpper = 0x54;
	
	RSSI_BT = initial_gain_psd;
	//SSBT = RSSI_BT;
	
	//RT_TRACE(	COMP_PSD, DBG_LOUD,("PSD: SSBT= %d\n", SSBT));
	RT_TRACE(	COMP_PSD, DBG_LOUD,("PSD: initial gain= 0x%x\n", initial_gain_psd));
	
	pDM_Odm->bDMInitialGainEnable = FALSE;		
	initial_gain = ODM_GetBBReg(pDM_Odm, 0xc50, bMaskDWord) & 0x7F;
	ODM_SetBBReg(pDM_Odm, 0xc50, 0x7F, initial_gain_psd);	
	//1 Turn off 3-wire
	ODM_SetBBReg(pDM_Odm, 0x88c, BIT20|BIT21|BIT22|BIT23, 0xF);

	//pts value = 128, 256, 512, 1024
	pts = 128;

	if(pts == 128)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x0);
		start_point = 64;
		stop_point = 192;
	}
	else if(pts == 256)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x1);
		start_point = 128;
		stop_point = 384;
	}
	else if(pts == 512)
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x2);
		start_point = 256;
		stop_point = 768;
	}
	else
	{
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x3);
		start_point = 512;
		stop_point = 1536;
	}
	

//3 Skip WLAN channels if WLAN busy
	curTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast) - lastTxOkCnt;
	curRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast) - lastRxOkCnt;
	lastTxOkCnt = *(pDM_Odm->pNumTxBytesUnicast);
	lastRxOkCnt = *(pDM_Odm->pNumRxBytesUnicast);
	
	PSD_skip_start=80;
	PSD_skip_stop = 0;
	wlan_channel = CurrentChannel & 0x0f;

	RT_TRACE(COMP_PSD,DBG_LOUD,("PSD: current channel: %x, BW:%d \n", wlan_channel, Is40MHz));
	
	if((curRxOkCnt+curTxOkCnt) > 1000)
	{
		PSD_skip_start = (wlan_channel-1)*5 -Is40MHz*10;
		PSD_skip_stop = PSD_skip_start + (1+Is40MHz)*20;
	}

	RT_TRACE(COMP_PSD,DBG_LOUD,("PSD: Skip tone from %d to %d \n", PSD_skip_start, PSD_skip_stop));

 	for (n=0;n<80;n++)
 	{
 		if((n%20)==0)
 		{
			channel = (n/20)*4 + 1;
			if(pDM_Odm->SupportICType == ODM_RTL8192D)
			{
				switch(channel)
				{
					case 1: 
					case 9:
						group_idx = 0;
						break;
					case 5:
						group_idx = 2;
						break;
					case 13:
				 		group_idx = 1;
						break;
				}
				if((*(pDM_Odm->pMacPhyMode)==ODM_SMSP)||(*(pDM_Odm->pMacPhyMode)==ODM_DMSP))   
		{
					for(i = 0; i < SYN_Length; i++)
						ODM_SetRFReg(pDM_Odm, RF_PATH_B, SYN[i], bMaskDWord, SYN_group[group_idx][i]);

					ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
					ODM_SetRFReg(pDM_Odm, RF_PATH_B, RF_CHNLBW, 0x3FF, channel);
				}
				else  // DualMAC_DualPHY 2G
			{
					for(i = 0; i < SYN_Length; i++)
						ODM_SetRFReg(pDM_Odm, RF_PATH_A, SYN[i], bMaskDWord, SYN_group[group_idx][i]);   
					
					ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
				}
			}
			else
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, 0x3FF, channel);
			}	
		tone_idx = n%20;
		if ((n>=PSD_skip_start) && (n<PSD_skip_stop))
		{	
			PSD_report[n] = initial_gain_psd;//SSBT;
			ODM_RT_TRACE(pDM_Odm,COMP_PSD,DBG_LOUD,("PSD:Tone %d skipped \n", n));
		}
		else
		{
			PSD_report_tmp =  GetPSDData(pDM_Odm, idx[tone_idx], initial_gain_psd);

			if ( PSD_report_tmp > PSD_report[n])
				PSD_report[n] = PSD_report_tmp;
				
		}
	}

	PatchDCTone(pDM_Odm, PSD_report, initial_gain_psd);
      
       //----end
	//1 Turn on RX
	//Rx AGC on
	ODM_SetBBReg(pDM_Odm, 0xC70, BIT0, 1);
	ODM_SetBBReg(pDM_Odm, 0xC7C, BIT20, 1);
	//CCK on
	ODM_SetBBReg(pDM_Odm, rFPGA0_RFMOD, BIT24, 1);
	//1 Turn on TX
	//Resume TX Queue
	ODM_Write1Byte(pDM_Odm, REG_TXPAUSE, 0x00);
	//Turn on 3-wire
	ODM_SetBBReg(pDM_Odm, 0x88c, BIT20|BIT21|BIT22|BIT23, 0x0);
	//1 Restore Current Settings
	//Resume DIG
	pDM_Odm->bDMInitialGainEnable= TRUE;
	ODM_SetBBReg(pDM_Odm, 0xc50, 0x7F, initial_gain);
	// restore originl center frequency
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask, CurrentChannel);
	if(pDM_Odm->SupportICType == ODM_RTL8192D)
	{
		if((*(pDM_Odm->pMacPhyMode)==ODM_SMSP)||(*(pDM_Odm->pMacPhyMode)==ODM_DMSP))
		{
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, RF_CHNLBW, bMaskDWord, CurrentChannel);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x25, bMaskDWord, SYN_RF25);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x26, bMaskDWord, SYN_RF26);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x27, bMaskDWord, SYN_RF27);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x2B, bMaskDWord, SYN_RF2B);
			ODM_SetRFReg(pDM_Odm, RF_PATH_B, 0x2C, bMaskDWord, SYN_RF2C);
		}
		else     // DualMAC_DualPHY
		{
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x25, bMaskDWord, SYN_RF25);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x26, bMaskDWord, SYN_RF26);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x27, bMaskDWord, SYN_RF27);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x2B, bMaskDWord, SYN_RF2B);
			ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x2C, bMaskDWord, SYN_RF2C);
		}
	}
	//Turn on CCA
	ODM_SetBBReg(pDM_Odm, 0xC14, bMaskDWord, RXIQI);
	//Restore RX idle low power
	if(RxIdleLowPwr == TRUE)
		ODM_SetBBReg(pDM_Odm, 0x818, BIT28, 1);
	
	psd_cnt++;
	//gPrint("psd cnt=%d\n", psd_cnt);
	ODM_RT_TRACE(pDM_Odm,COMP_PSD, DBG_LOUD,("PSD:psd_cnt = %d \n",psd_cnt));
	if (psd_cnt < ReScan)
	{
		ODM_SetTimer(pDM_Odm, &pRX_HP_Table->PSDTimer, Interval);  //ms
	}
	else
			{	
		psd_cnt = 0;
		for(i=0;i<80;i++)
			RT_TRACE(	COMP_PSD, DBG_LOUD,("psd_report[%d]=     %d \n", 2402+i, PSD_report[i]));
			//DbgPrint("psd_report[%d]=     %d \n", 2402+i, PSD_report[i]);

		GoodChannelDecision(pDM_Odm, PSD_report, PSD_bitmap,RSSI_BT, PSD_bitmap_memory);

			}
		}

VOID
odm_PSD_RXHPCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	pRXHP_T			pRX_HP_Table  = &pDM_Odm->DM_RXHP_Table;
	
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
	#if USE_WORKITEM
	ODM_ScheduleWorkItem(&pRX_HP_Table->PSDTimeWorkitem);
	#else
	odm_PSD_RXHP(pDM_Odm);
	#endif
#else
	ODM_ScheduleWorkItem(&pRX_HP_Table->PSDTimeWorkitem);
#endif
	
	}

VOID
odm_PSD_RXHPWorkitemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	pAdapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	
	odm_PSD_RXHP(pDM_Odm);
}

#endif //#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)


//
// 2011/12/02 MH Copy from MP oursrc for temporarily test.
//
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
VOID
odm_OFDMTXPathDiversity_92C(
	IN	PADAPTER	Adapter)
{
//	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	PRT_WLAN_STA	pEntry;
	u1Byte	DefaultRespPath = 0;
	u4Byte	i;
	s4Byte	MinRSSI = 0xFF;
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;
	pDM_PDTable->OFDMTXPath = 0;
	
	//1 Default Port
	if(pMgntInfo->mAssoc)
	{
		RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: Default port RSSI[0]=%d, RSSI[1]=%d\n",
			Adapter->RxStats.RxRSSIPercentage[0], Adapter->RxStats.RxRSSIPercentage[1]));
		if(Adapter->RxStats.RxRSSIPercentage[0] > Adapter->RxStats.RxRSSIPercentage[1])
		{
			pDM_PDTable->OFDMTXPath = pDM_PDTable->OFDMTXPath & (~BIT0);
			MinRSSI =  Adapter->RxStats.RxRSSIPercentage[1];
			DefaultRespPath = 0;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: Default port Select Path-0\n"));
		}
		else
		{
			pDM_PDTable->OFDMTXPath =  pDM_PDTable->OFDMTXPath | BIT0;
			MinRSSI =  Adapter->RxStats.RxRSSIPercentage[0];
			DefaultRespPath = 1;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: Default port Select Path-1\n"));
		}
		//RT_TRACE(	COMP_SWAS, DBG_LOUD, ("pDM_PDTable->OFDMTXPath =0x%x\n",pDM_PDTable->OFDMTXPath));
	}
	//1 Extension Port
	for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
			pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
		else
			pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);

		if(pEntry!=NULL)
		{
			if(pEntry->bAssociated)
			{
				RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: MACID=%d, RSSI_0=%d, RSSI_1=%d\n", 
					pEntry->AID+1, pEntry->rssi_stat.RxRSSIPercentage[0], pEntry->rssi_stat.RxRSSIPercentage[1]));
				
				if(pEntry->rssi_stat.RxRSSIPercentage[0] > pEntry->rssi_stat.RxRSSIPercentage[1])
				{
					pDM_PDTable->OFDMTXPath = pDM_PDTable->OFDMTXPath & ~(BIT(pEntry->AID+1));
					//pHalData->TXPath = pHalData->TXPath & ~(1<<(pEntry->AID+1));
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: MACID=%d Select Path-0\n", pEntry->AID+1));
					if(pEntry->rssi_stat.RxRSSIPercentage[1] < MinRSSI)
					{
						MinRSSI = pEntry->rssi_stat.RxRSSIPercentage[1];
						DefaultRespPath = 0;
					}
				}
				else
				{
					pDM_PDTable->OFDMTXPath = pDM_PDTable->OFDMTXPath | BIT(pEntry->AID+1);
					//pHalData->TXPath = pHalData->TXPath | (1 << (pEntry->AID+1));
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_OFDMTXPathDiversity_92C: MACID=%d Select Path-1\n", pEntry->AID+1));
					if(pEntry->rssi_stat.RxRSSIPercentage[0] < MinRSSI)
					{
						MinRSSI = pEntry->rssi_stat.RxRSSIPercentage[0];
						DefaultRespPath = 1;
					}
				}
			}
		}
		else
		{
			break;
		}
	}

	pDM_PDTable->OFDMDefaultRespPath = DefaultRespPath;
}


BOOLEAN
odm_IsConnected_92C(
	IN	PADAPTER	Adapter
)
{
	PRT_WLAN_STA	pEntry;
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	u4Byte		i;
	BOOLEAN		bConnected=FALSE;
	
	if(pMgntInfo->mAssoc)
	{
		bConnected = TRUE;
	}
	else
	{
		for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		{
			if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
				pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
			else
				pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);

			if(pEntry!=NULL)
			{
				if(pEntry->bAssociated)
				{
					bConnected = TRUE;
					break;
				}
			}
			else
			{
				break;
			}
		}
	}
	return	bConnected;
}


VOID
odm_ResetPathDiversity_92C(
		IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;
	PRT_WLAN_STA	pEntry;
	u4Byte	i;

	pHalData->RSSI_test = FALSE;
	pDM_PDTable->CCK_Pkt_Cnt = 0;
	pDM_PDTable->OFDM_Pkt_Cnt = 0;
	pHalData->CCK_Pkt_Cnt =0;
	pHalData->OFDM_Pkt_Cnt =0;
	
	if(pDM_PDTable->CCKPathDivEnable == TRUE)	
		PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x01); //RX path = PathAB

	for(i=0; i<2; i++)
	{
		pDM_PDTable->RSSI_CCK_Path_cnt[i]=0;
		pDM_PDTable->RSSI_CCK_Path[i] = 0;
	}
	for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
			pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
		else
			pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);

		if(pEntry!=NULL)
		{
			pEntry->rssi_stat.CCK_Pkt_Cnt = 0;
			pEntry->rssi_stat.OFDM_Pkt_Cnt = 0;
			for(i=0; i<2; i++)
			{
				pEntry->rssi_stat.RSSI_CCK_Path_cnt[i] = 0;
				pEntry->rssi_stat.RSSI_CCK_Path[i] = 0;
			}
		}
		else
			break;
	}
}


VOID
odm_CCKTXPathDiversity_92C(
	IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &(Adapter->MgntInfo);
	PRT_WLAN_STA	pEntry;
	s4Byte	MinRSSI = 0xFF;
	u1Byte	DefaultRespPath = 0;
	u4Byte	i;
//	BOOLEAN	bBModePathDiv = FALSE;
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;

	//1 Default Port
	if(pMgntInfo->mAssoc)
	{
		if(pHalData->OFDM_Pkt_Cnt == 0)
		{
			for(i=0; i<2; i++)
			{
				if(pDM_PDTable->RSSI_CCK_Path_cnt[i] > 1) //Because the first packet is discarded
					pDM_PDTable->RSSI_CCK_Path[i] = pDM_PDTable->RSSI_CCK_Path[i] / (pDM_PDTable->RSSI_CCK_Path_cnt[i]-1);
				else
					pDM_PDTable->RSSI_CCK_Path[i] = 0;
			}
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: pDM_PDTable->RSSI_CCK_Path[0]=%d, pDM_PDTable->RSSI_CCK_Path[1]=%d\n",
				pDM_PDTable->RSSI_CCK_Path[0], pDM_PDTable->RSSI_CCK_Path[1]));
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: pDM_PDTable->RSSI_CCK_Path_cnt[0]=%d, pDM_PDTable->RSSI_CCK_Path_cnt[1]=%d\n",
				pDM_PDTable->RSSI_CCK_Path_cnt[0], pDM_PDTable->RSSI_CCK_Path_cnt[1]));
		
			if(pDM_PDTable->RSSI_CCK_Path[0] > pDM_PDTable->RSSI_CCK_Path[1])
			{
				pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath & (~BIT0);
				MinRSSI =  pDM_PDTable->RSSI_CCK_Path[1];
				DefaultRespPath = 0;
				RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Default port Select CCK Path-0\n"));
			}
			else if(pDM_PDTable->RSSI_CCK_Path[0] < pDM_PDTable->RSSI_CCK_Path[1])
			{
				pDM_PDTable->CCKTXPath =  pDM_PDTable->CCKTXPath | BIT0;
				MinRSSI =  pDM_PDTable->RSSI_CCK_Path[0];
				DefaultRespPath = 1;
				RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Default port Select CCK Path-1\n"));
			}
			else
			{
				if((pDM_PDTable->RSSI_CCK_Path[0] != 0) && (pDM_PDTable->RSSI_CCK_Path[0] < MinRSSI))
				{
					pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath & (~BIT0);
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Default port Select CCK Path-0\n"));
					MinRSSI =  pDM_PDTable->RSSI_CCK_Path[1];
					DefaultRespPath = 0;
				}
				else
				{
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Default port unchange CCK Path\n"));
				}
			}
		}
		else //Follow OFDM decision
		{
			pDM_PDTable->CCKTXPath = (pDM_PDTable->CCKTXPath & (~BIT0)) | (pDM_PDTable->OFDMTXPath &BIT0);
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Follow OFDM decision, Default port Select CCK Path-%d\n",
				pDM_PDTable->CCKTXPath &BIT0));
		}
	}
	//1 Extension Port
	for(i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
	{
		if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
			pEntry = AsocEntry_EnumStation(GetFirstExtAdapter(Adapter), i);
		else
			pEntry = AsocEntry_EnumStation(GetDefaultAdapter(Adapter), i);

		if(pEntry!=NULL)
		{
			if(pEntry->bAssociated)
			{
				if(pEntry->rssi_stat.OFDM_Pkt_Cnt == 0)
				{
					for(i=0; i<2; i++)
					{
						if(pEntry->rssi_stat.RSSI_CCK_Path_cnt[i] > 1)
							pEntry->rssi_stat.RSSI_CCK_Path[i] = pEntry->rssi_stat.RSSI_CCK_Path[i] / (pEntry->rssi_stat.RSSI_CCK_Path_cnt[i]-1);
						else
							pEntry->rssi_stat.RSSI_CCK_Path[i] = 0;
					}
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d, RSSI_CCK0=%d, RSSI_CCK1=%d\n", 
						pEntry->AID+1, pEntry->rssi_stat.RSSI_CCK_Path[0], pEntry->rssi_stat.RSSI_CCK_Path[1]));
					
					if(pEntry->rssi_stat.RSSI_CCK_Path[0] >pEntry->rssi_stat.RSSI_CCK_Path[1])
					{
						pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath & ~(BIT(pEntry->AID+1));
						RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d Select CCK Path-0\n", pEntry->AID+1));
						if(pEntry->rssi_stat.RSSI_CCK_Path[1] < MinRSSI)
						{
							MinRSSI = pEntry->rssi_stat.RSSI_CCK_Path[1];
							DefaultRespPath = 0;
						}
					}
					else if(pEntry->rssi_stat.RSSI_CCK_Path[0] <pEntry->rssi_stat.RSSI_CCK_Path[1])
					{
						pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath | BIT(pEntry->AID+1);
						RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d Select CCK Path-1\n", pEntry->AID+1));
						if(pEntry->rssi_stat.RSSI_CCK_Path[0] < MinRSSI)
						{
							MinRSSI = pEntry->rssi_stat.RSSI_CCK_Path[0];
							DefaultRespPath = 1;
						}
					}
					else
					{
						if((pEntry->rssi_stat.RSSI_CCK_Path[0] != 0) && (pEntry->rssi_stat.RSSI_CCK_Path[0] < MinRSSI))
						{
							pDM_PDTable->CCKTXPath = pDM_PDTable->CCKTXPath & ~(BIT(pEntry->AID+1));
							MinRSSI = pEntry->rssi_stat.RSSI_CCK_Path[1];
							DefaultRespPath = 0;
							RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d Select CCK Path-0\n", pEntry->AID+1));
						}
						else
						{
							RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: MACID=%d unchange CCK Path\n", pEntry->AID+1));
						}
					}
				}
				else //Follow OFDM decision
				{
					pDM_PDTable->CCKTXPath = (pDM_PDTable->CCKTXPath & (~(BIT(pEntry->AID+1)))) | (pDM_PDTable->OFDMTXPath & BIT(pEntry->AID+1));
					RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: Follow OFDM decision, MACID=%d Select CCK Path-%d\n",
						pEntry->AID+1, (pDM_PDTable->CCKTXPath & BIT(pEntry->AID+1))>>(pEntry->AID+1)));
				}
			}
		}
		else
		{
			break;
		}
	}

	RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C:MinRSSI=%d\n",MinRSSI));

	if(MinRSSI == 0xFF)
		DefaultRespPath = pDM_PDTable->CCKDefaultRespPath;

	pDM_PDTable->CCKDefaultRespPath = DefaultRespPath;
}



VOID
odm_PathDiversityAfterLink_92C(
	IN	PADAPTER	Adapter
)
{
	HAL_DATA_TYPE		*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	pPD_T		pDM_PDTable = &Adapter->DM_PDTable;
	u1Byte		DefaultRespPath=0;

	if((!IS_92C_SERIAL(pHalData->VersionID)) || (pHalData->PathDivCfg != 1) || (pHalData->eRFPowerState == eRfOff))
	{
		if(pHalData->PathDivCfg == 0)
		{
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("No ODM_TXPathDiversity()\n"));
		}
		else
		{
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("2T ODM_TXPathDiversity()\n"));
		}
		return;
	}
	if(!odm_IsConnected_92C(Adapter))
	{
		RT_TRACE(	COMP_SWAS, DBG_LOUD, ("ODM_TXPathDiversity(): No Connections\n"));
		return;
	}
	
	
	if(pDM_PDTable->TrainingState == 0)
	{
		RT_TRACE(	COMP_SWAS, DBG_LOUD, ("ODM_TXPathDiversity() ==>\n"));
		odm_OFDMTXPathDiversity_92C(Adapter);

		if((pDM_PDTable->CCKPathDivEnable == TRUE) && (pDM_PDTable->OFDM_Pkt_Cnt < 100))
		{
			//RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: TrainingState=0\n"));
			
			if(pDM_PDTable->CCK_Pkt_Cnt > 300)
				pDM_PDTable->Timer = 20;
			else if(pDM_PDTable->CCK_Pkt_Cnt > 100)
				pDM_PDTable->Timer = 60;
			else
				pDM_PDTable->Timer = 250;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: timer=%d\n",pDM_PDTable->Timer));

			PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x00); // RX path = PathA
			pDM_PDTable->TrainingState = 1;
			pHalData->RSSI_test = TRUE;
			ODM_SetTimer( pDM_Odm, &pDM_Odm->CCKPathDiversityTimer, pDM_PDTable->Timer); //ms
		}
		else
		{
			pDM_PDTable->CCKTXPath = pDM_PDTable->OFDMTXPath;
			DefaultRespPath = pDM_PDTable->OFDMDefaultRespPath;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_SetRespPath_92C: Skip odm_CCKTXPathDiversity_92C, DefaultRespPath is OFDM\n"));
			odm_SetRespPath_92C(Adapter, DefaultRespPath);
			odm_ResetPathDiversity_92C(Adapter);
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("ODM_TXPathDiversity() <==\n"));
		}
	}
	else if(pDM_PDTable->TrainingState == 1)
	{
		//RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: TrainingState=1\n"));
		PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x05); // RX path = PathB
		pDM_PDTable->TrainingState = 2;
		ODM_SetTimer( pDM_Odm, &pDM_Odm->CCKPathDiversityTimer, pDM_PDTable->Timer); //ms
	}
	else
	{
		//RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_CCKTXPathDiversity_92C: TrainingState=2\n"));
		pDM_PDTable->TrainingState = 0;	
		odm_CCKTXPathDiversity_92C(Adapter); 
		if(pDM_PDTable->OFDM_Pkt_Cnt != 0)
		{
			DefaultRespPath = pDM_PDTable->OFDMDefaultRespPath;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_SetRespPath_92C: DefaultRespPath is OFDM\n"));
		}
		else
		{
			DefaultRespPath = pDM_PDTable->CCKDefaultRespPath;
			RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_SetRespPath_92C: DefaultRespPath is CCK\n"));
		}
		odm_SetRespPath_92C(Adapter, DefaultRespPath);
		odm_ResetPathDiversity_92C(Adapter);
		RT_TRACE(	COMP_SWAS, DBG_LOUD, ("ODM_TXPathDiversity() <==\n"));
	}

}



VOID
odm_CCKTXPathDiversityCallback(
	PRT_TIMER		pTimer
)
{
#if USE_WORKITEM
       PADAPTER	Adapter = (PADAPTER)pTimer->Adapter;
       HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	   PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
#else
	PADAPTER	Adapter = (PADAPTER)pTimer->Adapter;
#endif

#if DEV_BUS_TYPE==RT_PCI_INTERFACE
#if USE_WORKITEM
	PlatformScheduleWorkItem(&pDM_Odm->CCKPathDiversityWorkitem);
#else
	odm_PathDiversityAfterLink_92C(Adapter);
#endif
#else
	PlatformScheduleWorkItem(&pDM_Odm->CCKPathDiversityWorkitem);
#endif

}


VOID
odm_CCKTXPathDiversityWorkItemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	Adapter = (PADAPTER)pContext;

	odm_CCKTXPathDiversity_92C(Adapter);
}


VOID
ODM_CCKPathDiversityChkPerPktRssi(
	PADAPTER		Adapter,
	BOOLEAN			bIsDefPort,
	BOOLEAN			bMatchBSSID,
	PRT_WLAN_STA	pEntry,
	PRT_RFD			pRfd,
	pu1Byte			pDesc
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	BOOLEAN			bCount = FALSE;
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;
	//BOOLEAN	isCCKrate = RX_HAL_IS_CCK_RATE_92C(pDesc);
#if DEV_BUS_TYPE != RT_SDIO_INTERFACE
	BOOLEAN	isCCKrate = RX_HAL_IS_CCK_RATE(Adapter, pDesc);
#else  //below code would be removed if we have verified SDIO
	BOOLEAN	isCCKrate = IS_HARDWARE_TYPE_8188E(Adapter) ? RX_HAL_IS_CCK_RATE_88E(pDesc) : RX_HAL_IS_CCK_RATE_92C(pDesc);
#endif

	if((pHalData->PathDivCfg != 1) || (pHalData->RSSI_test == FALSE))
		return;
		
	if(pHalData->RSSI_target==NULL && bIsDefPort && bMatchBSSID)
		bCount = TRUE;
	else if(pHalData->RSSI_target!=NULL && pEntry!=NULL && pHalData->RSSI_target==pEntry)
		bCount = TRUE;

	if(bCount && isCCKrate)
	{
		if(pDM_PDTable->TrainingState == 1 )
		{
			if(pEntry)
			{
				if(pEntry->rssi_stat.RSSI_CCK_Path_cnt[0] != 0)
					pEntry->rssi_stat.RSSI_CCK_Path[0] += pRfd->Status.RxPWDBAll;
				pEntry->rssi_stat.RSSI_CCK_Path_cnt[0]++;
			}
			else
			{
				if(pDM_PDTable->RSSI_CCK_Path_cnt[0] != 0)
					pDM_PDTable->RSSI_CCK_Path[0] += pRfd->Status.RxPWDBAll;
				pDM_PDTable->RSSI_CCK_Path_cnt[0]++;
			}
		}
		else if(pDM_PDTable->TrainingState == 2 )
		{
			if(pEntry)
			{
				if(pEntry->rssi_stat.RSSI_CCK_Path_cnt[1] != 0)
					pEntry->rssi_stat.RSSI_CCK_Path[1] += pRfd->Status.RxPWDBAll;
				pEntry->rssi_stat.RSSI_CCK_Path_cnt[1]++;
			}
			else
			{
				if(pDM_PDTable->RSSI_CCK_Path_cnt[1] != 0)
					pDM_PDTable->RSSI_CCK_Path[1] += pRfd->Status.RxPWDBAll;
				pDM_PDTable->RSSI_CCK_Path_cnt[1]++;
			}
		}
	}
}


BOOLEAN
ODM_PathDiversityBeforeLink92C(
	//IN	PADAPTER	Adapter
	IN		PDM_ODM_T		pDM_Odm
	)
{
#if (RT_MEM_SIZE_LEVEL != RT_MEM_SIZE_MINIMUM)
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE*	pHalData = NULL;
	PMGNT_INFO		pMgntInfo = NULL;
	//pSWAT_T		pDM_SWAT_Table = &Adapter->DM_SWAT_Table;
	pPD_T			pDM_PDTable = NULL;

	s1Byte			Score = 0;
	PRT_WLAN_BSS	pTmpBssDesc;
	PRT_WLAN_BSS	pTestBssDesc;

	u1Byte			target_chnl = 0;
	u1Byte			index;

	if (pDM_Odm->Adapter == NULL)  //For BSOD when plug/unplug fast.  //By YJ,120413
	{	// The ODM structure is not initialized.
		return FALSE;
	}
	pHalData = GET_HAL_DATA(Adapter);
	pMgntInfo = &Adapter->MgntInfo;
	pDM_PDTable = &Adapter->DM_PDTable;
	
	// Condition that does not need to use path diversity.
	if((!IS_92C_SERIAL(pHalData->VersionID)) || (pHalData->PathDivCfg!=1) || pMgntInfo->AntennaTest )
	{
		RT_TRACE(COMP_SWAS, DBG_LOUD, 
				("ODM_PathDiversityBeforeLink92C(): No PathDiv Mechanism before link.\n"));
		return FALSE;
	}

	// Since driver is going to set BB register, it shall check if there is another thread controlling BB/RF.
	PlatformAcquireSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	if(pHalData->eRFPowerState!=eRfOn || pMgntInfo->RFChangeInProgress || pMgntInfo->bMediaConnect)
	{
		PlatformReleaseSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	
		RT_TRACE(COMP_SWAS, DBG_LOUD, 
				("ODM_PathDiversityBeforeLink92C(): RFChangeInProgress(%x), eRFPowerState(%x)\n", 
				pMgntInfo->RFChangeInProgress,
				pHalData->eRFPowerState));
	
		//pDM_SWAT_Table->SWAS_NoLink_State = 0;
		pDM_PDTable->PathDiv_NoLink_State = 0;
		
		return FALSE;
	}
	else
	{
		PlatformReleaseSpinLock(Adapter, RT_RF_STATE_SPINLOCK);
	}

	//1 Run AntDiv mechanism "Before Link" part.
	//if(pDM_SWAT_Table->SWAS_NoLink_State == 0)
	if(pDM_PDTable->PathDiv_NoLink_State == 0)
	{
		//1 Prepare to do Scan again to check current antenna state.

		// Set check state to next step.
		//pDM_SWAT_Table->SWAS_NoLink_State = 1;
		pDM_PDTable->PathDiv_NoLink_State = 1;
	
		// Copy Current Scan list.
		Adapter->MgntInfo.tmpNumBssDesc = pMgntInfo->NumBssDesc;
		PlatformMoveMemory((PVOID)Adapter->MgntInfo.tmpbssDesc, (PVOID)pMgntInfo->bssDesc, sizeof(RT_WLAN_BSS)*MAX_BSS_DESC);

		// Switch Antenna to another one.
		if(pDM_PDTable->DefaultRespPath == 0)
		{
			PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x05); // TRX path = PathB
			odm_SetRespPath_92C(Adapter, 1);
			pDM_PDTable->OFDMTXPath = 0xFFFFFFFF;
			pDM_PDTable->CCKTXPath = 0xFFFFFFFF;
		}
		else
		{
			PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x00); // TRX path = PathA
			odm_SetRespPath_92C(Adapter, 0);
			pDM_PDTable->OFDMTXPath = 0x0;
			pDM_PDTable->CCKTXPath = 0x0;
		}
#if 0	

		pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
		pDM_SWAT_Table->CurAntenna = (pDM_SWAT_Table->CurAntenna==Antenna_A)?Antenna_B:Antenna_A;
		
		RT_TRACE(COMP_SWAS, DBG_LOUD, 
			("ODM_SwAntDivCheckBeforeLink8192C: Change to Ant(%s) for testing.\n", (pDM_SWAT_Table->CurAntenna==Antenna_A)?"A":"B"));
		//PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, 0x300, DM_SWAT_Table.CurAntenna);
		pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 & 0xfffffcff) | (pDM_SWAT_Table->CurAntenna<<8));
		PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg860);
#endif

		// Go back to scan function again.
		RT_TRACE(COMP_SWAS, DBG_LOUD, ("ODM_PathDiversityBeforeLink92C: Scan one more time\n"));
		pMgntInfo->ScanStep=0;
		target_chnl = odm_SwAntDivSelectChkChnl(Adapter);
		odm_SwAntDivConsructChkScanChnl(Adapter, target_chnl);
		HTReleaseChnlOpLock(Adapter);
		PlatformSetTimer(Adapter, &pMgntInfo->ScanTimer, 5);

		return TRUE;
	}
	else
	{
		//1 ScanComple() is called after antenna swiched.
		//1 Check scan result and determine which antenna is going
		//1 to be used.

		for(index=0; index<Adapter->MgntInfo.tmpNumBssDesc; index++)
		{
			pTmpBssDesc = &(Adapter->MgntInfo.tmpbssDesc[index]);
			pTestBssDesc = &(pMgntInfo->bssDesc[index]);

			if(PlatformCompareMemory(pTestBssDesc->bdBssIdBuf, pTmpBssDesc->bdBssIdBuf, 6)!=0)
			{
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("ODM_PathDiversityBeforeLink92C(): ERROR!! This shall not happen.\n"));
				continue;
			}

			if(pTmpBssDesc->RecvSignalPower > pTestBssDesc->RecvSignalPower)
			{
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("ODM_PathDiversityBeforeLink92C: Compare scan entry: Score++\n"));
				RT_PRINT_STR(COMP_SWAS, DBG_LOUD, "SSID: ", pTestBssDesc->bdSsIdBuf, pTestBssDesc->bdSsIdLen);
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
			
				Score++;
				PlatformMoveMemory(pTestBssDesc, pTmpBssDesc, sizeof(RT_WLAN_BSS));
			}
			else if(pTmpBssDesc->RecvSignalPower < pTestBssDesc->RecvSignalPower)
			{
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("ODM_PathDiversityBeforeLink92C: Compare scan entry: Score--\n"));
				RT_PRINT_STR(COMP_SWAS, DBG_LOUD, "SSID: ", pTestBssDesc->bdSsIdBuf, pTestBssDesc->bdSsIdLen);
				RT_TRACE(COMP_SWAS, DBG_LOUD, ("Original: %d, Test: %d\n", pTmpBssDesc->RecvSignalPower, pTestBssDesc->RecvSignalPower));
				Score--;
			}

		}

		if(pMgntInfo->NumBssDesc!=0 && Score<=0)
		{
			RT_TRACE(COMP_SWAS, DBG_LOUD,
				("ODM_PathDiversityBeforeLink92C(): DefaultRespPath=%d\n", pDM_PDTable->DefaultRespPath));

			//pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
		}
		else
		{
			RT_TRACE(COMP_SWAS, DBG_LOUD, 
				("ODM_PathDiversityBeforeLink92C(): DefaultRespPath=%d\n", pDM_PDTable->DefaultRespPath));

			if(pDM_PDTable->DefaultRespPath == 0)
			{
				pDM_PDTable->OFDMTXPath = 0xFFFFFFFF;
				pDM_PDTable->CCKTXPath = 0xFFFFFFFF;
				odm_SetRespPath_92C(Adapter, 1);
			}
			else
			{
				pDM_PDTable->OFDMTXPath = 0x0;
				pDM_PDTable->CCKTXPath = 0x0;
				odm_SetRespPath_92C(Adapter, 0);
			}
			PHY_SetBBReg(Adapter, rCCK0_AFESetting  , 0x0F000000, 0x01); // RX path = PathAB

			//pDM_SWAT_Table->CurAntenna = pDM_SWAT_Table->PreAntenna;

			//PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, 0x300, DM_SWAT_Table.CurAntenna);
			//pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 = ((pDM_SWAT_Table->SWAS_NoLink_BK_Reg860 & 0xfffffcff) | (pDM_SWAT_Table->CurAntenna<<8));
			//PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, bMaskDWord, pDM_SWAT_Table->SWAS_NoLink_BK_Reg860);
		}

		// Check state reset to default and wait for next time.
		//pDM_SWAT_Table->SWAS_NoLink_State = 0;
		pDM_PDTable->PathDiv_NoLink_State = 0;

		return FALSE;
	}
#else
		return	FALSE;
#endif
	
}


//Neil Chen---2011--06--22
//----92D Path Diversity----//
//#ifdef PathDiv92D
//==================================
//3 Path Diversity 
//==================================
//
// 20100514 Luke/Joseph:
// Add new function for antenna diversity after link.
// This is the main function of antenna diversity after link.
// This function is called in HalDmWatchDog() and ODM_SwAntDivChkAntSwitchCallback().
// HalDmWatchDog() calls this function with SWAW_STEP_PEAK to initialize the antenna test.
// In SWAW_STEP_PEAK, another antenna and a 500ms timer will be set for testing.
// After 500ms, ODM_SwAntDivChkAntSwitchCallback() calls this function to compare the signal just
// listened on the air with the RSSI of original antenna.
// It chooses the antenna with better RSSI.
// There is also a aged policy for error trying. Each error trying will cost more 5 seconds waiting 
// penalty to get next try.
//
//
// 20100503 Joseph:
// Add new function SwAntDivCheck8192C().
// This is the main function of Antenna diversity function before link.
// Mainly, it just retains last scan result and scan again.
// After that, it compares the scan result to see which one gets better RSSI.
// It selects antenna with better receiving power and returns better scan result.
//


//
// 20100514 Luke/Joseph:
// This function is used to gather the RSSI information for antenna testing.
// It selects the RSSI of the peer STA that we want to know.
//
VOID
ODM_PathDivChkPerPktRssi(
	PADAPTER		Adapter,
	BOOLEAN			bIsDefPort,
	BOOLEAN			bMatchBSSID,
	PRT_WLAN_STA	pEntry,
	PRT_RFD			pRfd
	)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);	
	BOOLEAN			bCount = FALSE;
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	pSWAT_T			pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	if(pHalData->RSSI_target==NULL && bIsDefPort && bMatchBSSID)
		bCount = TRUE;
	else if(pHalData->RSSI_target!=NULL && pEntry!=NULL && pHalData->RSSI_target==pEntry)
		bCount = TRUE;

	if(bCount)
	{
		//1 RSSI for SW Antenna Switch
		if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
		{
			pHalData->RSSI_sum_A += pRfd->Status.RxPWDBAll;
			pHalData->RSSI_cnt_A++;
		}
		else
		{
			pHalData->RSSI_sum_B += pRfd->Status.RxPWDBAll;
			pHalData->RSSI_cnt_B++;

		}
	}
}



//
// 20100514 Luke/Joseph:
// Add new function to reset antenna diversity state after link.
//
VOID
ODM_PathDivRestAfterLink(
	IN	PDM_ODM_T		pDM_Odm
	)
{
	PADAPTER		Adapter=pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	pSWAT_T			pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;

	pHalData->RSSI_cnt_A = 0;
	pHalData->RSSI_cnt_B = 0;
	pHalData->RSSI_test = FALSE;
	pDM_SWAT_Table->try_flag = 0x0;       // NOT 0xff
	pDM_SWAT_Table->RSSI_Trying = 0;
	pDM_SWAT_Table->SelectAntennaMap=0xAA;
	pDM_SWAT_Table->CurAntenna = MAIN_ANT;  
}


//
// 20100514 Luke/Joseph:
// Callback function for 500ms antenna test trying.
//
VOID
odm_PathDivChkAntSwitchCallback(
	PRT_TIMER		pTimer
)
{
	PADAPTER		Adapter = (PADAPTER)pTimer->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

#if DEV_BUS_TYPE==RT_PCI_INTERFACE

#if USE_WORKITEM
	PlatformScheduleWorkItem(&pDM_Odm->PathDivSwitchWorkitem);
#else
	odm_PathDivChkAntSwitch(pDM_Odm);
#endif
#else
	PlatformScheduleWorkItem(&pDM_Odm->PathDivSwitchWorkitem);
#endif

//odm_SwAntDivChkAntSwitch(Adapter, SWAW_STEP_DETERMINE);

}


VOID
odm_PathDivChkAntSwitchWorkitemCallback(
    IN PVOID            pContext
    )
{
	PADAPTER	pAdapter = (PADAPTER)pContext;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(pAdapter);
	PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;

	odm_PathDivChkAntSwitch(pDM_Odm);
}


 //MAC0_ACCESS_PHY1

// 2011-06-22 Neil Chen & Gary Hsin
// Refer to Jr.Luke's SW ANT DIV
// 92D Path Diversity Main function
// refer to 88C software antenna diversity
// 
VOID
odm_PathDivChkAntSwitch(
	PDM_ODM_T		pDM_Odm
	//PADAPTER		Adapter,
	//u1Byte			Step
)
{
	PADAPTER		Adapter = pDM_Odm->Adapter;
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	PMGNT_INFO		pMgntInfo = &Adapter->MgntInfo;


	pSWAT_T			pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	s4Byte			curRSSI=100, RSSI_A, RSSI_B;
	u1Byte			nextAntenna=AUX_ANT;
	static u8Byte		lastTxOkCnt=0, lastRxOkCnt=0;
	u8Byte			curTxOkCnt, curRxOkCnt;
	static u8Byte		TXByteCnt_A=0, TXByteCnt_B=0, RXByteCnt_A=0, RXByteCnt_B=0;
	u8Byte			CurByteCnt=0, PreByteCnt=0;
	static u1Byte		TrafficLoad = TRAFFIC_LOW;
	u1Byte			Score_A=0, Score_B=0;
	u1Byte			i=0x0;
       // Neil Chen
       static u1Byte        pathdiv_para=0x0;     
       static u1Byte        switchfirsttime=0x00;
	// u1Byte                 regB33 = (u1Byte) PHY_QueryBBReg(Adapter, 0xB30,BIT27);
	u1Byte			regB33 = (u1Byte)ODM_GetBBReg(pDM_Odm, PATHDIV_REG, BIT27);


       //u1Byte                 reg637 =0x0;   
       static u1Byte        fw_value=0x0;         
	u1Byte                 n=0;
       static u8Byte		lastTxOkCnt_tmp=0, lastRxOkCnt_tmp=0;
	//u8Byte			curTxOkCnt_tmp, curRxOkCnt_tmp;
       PADAPTER            BuddyAdapter = Adapter->BuddyAdapter;     // another adapter MAC
        // Path Diversity   //Neil Chen--2011--06--22

	//u1Byte                 PathDiv_Trigger = (u1Byte) PHY_QueryBBReg(Adapter, 0xBA0,BIT31);
	u1Byte                 PathDiv_Trigger = (u1Byte) ODM_GetBBReg(pDM_Odm, PATHDIV_TRI,BIT31);
	u1Byte                 PathDiv_Enable = pHalData->bPathDiv_Enable;


	//DbgPrint("Path Div PG Value:%x \n",PathDiv_Enable);	
       if((BuddyAdapter==NULL)||(!PathDiv_Enable)||(PathDiv_Trigger)||(pHalData->CurrentBandType == BAND_ON_2_4G))
       {
           return;
       }
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD,("===================>odm_PathDivChkAntSwitch()\n"));

       // The first time to switch path excluding 2nd, 3rd, ....etc....
	if(switchfirsttime==0)
	{
	    if(regB33==0)
	    {
	       pDM_SWAT_Table->CurAntenna = MAIN_ANT;    // Default MAC0_5G-->Path A (current antenna)     
	    }	    
	}

	// Condition that does not need to use antenna diversity.
	if(pDM_Odm->SupportICType != ODM_RTL8192D)
	{
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_PathDiversityMechanims(): No PathDiv Mechanism.\n"));
		return;
	}

	// Radio off: Status reset to default and return.
	if(pHalData->eRFPowerState==eRfOff)
	{
		//ODM_SwAntDivRestAfterLink(Adapter);
		return;
	}

       /*
	// Handling step mismatch condition.
	// Peak step is not finished at last time. Recover the variable and check again.
	if(	Step != pDM_SWAT_Table->try_flag	)
	{
		ODM_SwAntDivRestAfterLink(Adapter);
	} */
	
	if(pDM_SWAT_Table->try_flag == 0xff)
	{
		// Select RSSI checking target
		if(pMgntInfo->mAssoc && !ACTING_AS_AP(Adapter))
		{
			// Target: Infrastructure mode AP.
			pHalData->RSSI_target = NULL;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_PathDivMechanism(): RSSI_target is DEF AP!\n"));
		}
		else
		{
			u4Byte			index = 0;
			PRT_WLAN_STA	pEntry = NULL;
			PADAPTER		pTargetAdapter = NULL;
		
			if(	pMgntInfo->mIbss || ACTING_AS_AP(Adapter) )
			{
				// Target: AP/IBSS peer.
				pTargetAdapter = Adapter;
			}
			else if(IsAPModeExist(Adapter)  && GetFirstExtAdapter(Adapter) != NULL)
			{
				// Target: VWIFI peer.
				pTargetAdapter = GetFirstExtAdapter(Adapter);
			}

			if(pTargetAdapter != NULL)
			{
				for(index=0; index<ODM_ASSOCIATE_ENTRY_NUM; index++)
				{
					pEntry = AsocEntry_EnumStation(pTargetAdapter, index);
					if(pEntry != NULL)
					{
						if(pEntry->bAssociated)
							break;			
					}
				}
			}

			if(pEntry == NULL)
			{
				ODM_PathDivRestAfterLink(pDM_Odm);
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): No Link.\n"));
				return;
			}
			else
			{
				pHalData->RSSI_target = pEntry;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): RSSI_target is PEER STA\n"));
			}
		}
			
		pHalData->RSSI_cnt_A = 0;
		pHalData->RSSI_cnt_B = 0;
		pDM_SWAT_Table->try_flag = 0;
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("odm_SwAntDivChkAntSwitch(): Set try_flag to 0 prepare for peak!\n"));
		return;
	}
	else
	{
	       // 1st step
		curTxOkCnt = Adapter->TxStats.NumTxBytesUnicast - lastTxOkCnt;
		curRxOkCnt = Adapter->RxStats.NumRxBytesUnicast - lastRxOkCnt;
		lastTxOkCnt = Adapter->TxStats.NumTxBytesUnicast;
		lastRxOkCnt = Adapter->RxStats.NumRxBytesUnicast;
	
		if(pDM_SWAT_Table->try_flag == 1)   // Training State
		{
			if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
			{
				TXByteCnt_A += curTxOkCnt;
				RXByteCnt_A += curRxOkCnt;
			}
			else
			{
				TXByteCnt_B += curTxOkCnt;
				RXByteCnt_B += curRxOkCnt;
			}
		
			nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
			pDM_SWAT_Table->RSSI_Trying--;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: RSSI_Trying = %d\n",pDM_SWAT_Table->RSSI_Trying));
			if(pDM_SWAT_Table->RSSI_Trying == 0)
			{
				CurByteCnt = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? (TXByteCnt_A+RXByteCnt_A) : (TXByteCnt_B+RXByteCnt_B);
				PreByteCnt = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? (TXByteCnt_B+RXByteCnt_B) : (TXByteCnt_A+RXByteCnt_A);
				
				if(TrafficLoad == TRAFFIC_HIGH)
				{
					//CurByteCnt = PlatformDivision64(CurByteCnt, 9);
					PreByteCnt =PreByteCnt*9;
				}
				else if(TrafficLoad == TRAFFIC_LOW)
				{
					//CurByteCnt = PlatformDivision64(CurByteCnt, 2);
					PreByteCnt =PreByteCnt*2;
				}
				if(pHalData->RSSI_cnt_A > 0)
					RSSI_A = pHalData->RSSI_sum_A/pHalData->RSSI_cnt_A; 
				else
					RSSI_A = 0;
				if(pHalData->RSSI_cnt_B > 0)
					RSSI_B = pHalData->RSSI_sum_B/pHalData->RSSI_cnt_B; 
		             else
					RSSI_B = 0;
				curRSSI = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
				pDM_SWAT_Table->PreRSSI =  (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_B : RSSI_A;
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: PreRSSI = %d, CurRSSI = %d\n",pDM_SWAT_Table->PreRSSI, curRSSI));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: preAntenna= %s, curAntenna= %s \n", 
				(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: RSSI_A= %d, RSSI_cnt_A = %d, RSSI_B= %d, RSSI_cnt_B = %d\n",
					RSSI_A, pHalData->RSSI_cnt_A, RSSI_B, pHalData->RSSI_cnt_B));
			}

		}
		else   // try_flag=0
		{
		
			if(pHalData->RSSI_cnt_A > 0)
				RSSI_A = pHalData->RSSI_sum_A/pHalData->RSSI_cnt_A; 
			else
				RSSI_A = 0;
			if(pHalData->RSSI_cnt_B > 0)
				RSSI_B = pHalData->RSSI_sum_B/pHalData->RSSI_cnt_B; 
			else
				RSSI_B = 0;	
			curRSSI = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
			pDM_SWAT_Table->PreRSSI =  (pDM_SWAT_Table->PreAntenna == MAIN_ANT)? RSSI_A : RSSI_B;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: PreRSSI = %d, CurRSSI = %d\n", pDM_SWAT_Table->PreRSSI, curRSSI));
		       ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: preAntenna= %s, curAntenna= %s \n", 
			(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));

			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH DIV=: RSSI_A= %d, RSSI_cnt_A = %d, RSSI_B= %d, RSSI_cnt_B = %d\n",
				RSSI_A, pHalData->RSSI_cnt_A, RSSI_B, pHalData->RSSI_cnt_B));
			//RT_TRACE(COMP_SWAS, DBG_LOUD, ("Ekul:curTxOkCnt = %d\n", curTxOkCnt));
			//RT_TRACE(COMP_SWAS, DBG_LOUD, ("Ekul:curRxOkCnt = %d\n", curRxOkCnt));
		}

		//1 Trying State
		if((pDM_SWAT_Table->try_flag == 1)&&(pDM_SWAT_Table->RSSI_Trying == 0))
		{

			if(pDM_SWAT_Table->TestMode == TP_MODE)
			{
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: TestMode = TP_MODE"));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH= TRY:CurByteCnt = %"i64fmt"d,", CurByteCnt));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH= TRY:PreByteCnt = %"i64fmt"d\n",PreByteCnt));		
				if(CurByteCnt < PreByteCnt)
				{
					if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
						pDM_SWAT_Table->SelectAntennaMap=pDM_SWAT_Table->SelectAntennaMap<<1;
					else
						pDM_SWAT_Table->SelectAntennaMap=(pDM_SWAT_Table->SelectAntennaMap<<1)+1;
				}
				else
				{
					if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
						pDM_SWAT_Table->SelectAntennaMap=(pDM_SWAT_Table->SelectAntennaMap<<1)+1;
					else
						pDM_SWAT_Table->SelectAntennaMap=pDM_SWAT_Table->SelectAntennaMap<<1;
				}
				for (i= 0; i<8; i++)
				{
					if(((pDM_SWAT_Table->SelectAntennaMap>>i)&BIT0) == 1)
						Score_A++;
					else
						Score_B++;
				}
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("SelectAntennaMap=%x\n ",pDM_SWAT_Table->SelectAntennaMap));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Score_A=%d, Score_B=%d\n", Score_A, Score_B));
			
				if(pDM_SWAT_Table->CurAntenna == MAIN_ANT)
				{
					nextAntenna = (Score_A >= Score_B)?MAIN_ANT:AUX_ANT;
				}
				else
				{
					nextAntenna = (Score_B >= Score_A)?AUX_ANT:MAIN_ANT;
				}
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: nextAntenna=%s\n",(nextAntenna==MAIN_ANT)?"MAIN":"AUX"));
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: preAntenna= %s, curAntenna= %s \n", 
				(pDM_SWAT_Table->PreAntenna == MAIN_ANT?"MAIN":"AUX"), (pDM_SWAT_Table->CurAntenna == MAIN_ANT?"MAIN":"AUX")));

				if(nextAntenna != pDM_SWAT_Table->CurAntenna)
				{
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Switch back to another antenna"));
				}
				else
				{
					ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: current anntena is good\n"));
				}	
			}

                    
			if(pDM_SWAT_Table->TestMode == RSSI_MODE)
			{	
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: TestMode = RSSI_MODE"));
				pDM_SWAT_Table->SelectAntennaMap=0xAA;
				if(curRSSI < pDM_SWAT_Table->PreRSSI) //Current antenna is worse than previous antenna
				{
					//RT_TRACE(COMP_SWAS, DBG_LOUD, ("SWAS: Switch back to another antenna"));
					nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)?AUX_ANT : MAIN_ANT;
				}
				else // current anntena is good
				{
					nextAntenna =pDM_SWAT_Table->CurAntenna;
					//RT_TRACE(COMP_SWAS, DBG_LOUD, ("SWAS: current anntena is good\n"));
				}
			}
			
			pDM_SWAT_Table->try_flag = 0;
			pHalData->RSSI_test = FALSE;
			pHalData->RSSI_sum_A = 0;
			pHalData->RSSI_cnt_A = 0;
			pHalData->RSSI_sum_B = 0;
			pHalData->RSSI_cnt_B = 0;
			TXByteCnt_A = 0;
			TXByteCnt_B = 0;
			RXByteCnt_A = 0;
			RXByteCnt_B = 0;
			
		}

		//1 Normal State
		else if(pDM_SWAT_Table->try_flag == 0)
		{
			if(TrafficLoad == TRAFFIC_HIGH)
			{
				if ((curTxOkCnt+curRxOkCnt) > 3750000)//if(PlatformDivision64(curTxOkCnt+curRxOkCnt, 2) > 1875000)
					TrafficLoad = TRAFFIC_HIGH;
				else
					TrafficLoad = TRAFFIC_LOW;
			}
			else if(TrafficLoad == TRAFFIC_LOW)
				{
				if ((curTxOkCnt+curRxOkCnt) > 3750000)//if(PlatformDivision64(curTxOkCnt+curRxOkCnt, 2) > 1875000)
					TrafficLoad = TRAFFIC_HIGH;
				else
					TrafficLoad = TRAFFIC_LOW;
			}
			if(TrafficLoad == TRAFFIC_HIGH)
				pDM_SWAT_Table->bTriggerAntennaSwitch = 0;
			//RT_TRACE(COMP_SWAS, DBG_LOUD, ("Normal:TrafficLoad = %llu\n", curTxOkCnt+curRxOkCnt));

			//Prepare To Try Antenna		
				nextAntenna = (pDM_SWAT_Table->CurAntenna == MAIN_ANT)? AUX_ANT : MAIN_ANT;
				pDM_SWAT_Table->try_flag = 1;
				pHalData->RSSI_test = TRUE;
			if((curRxOkCnt+curTxOkCnt) > 1000)
			{
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
	                    pDM_SWAT_Table->RSSI_Trying = 4;                           
#else
	                    pDM_SWAT_Table->RSSI_Trying = 2;
#endif
				pDM_SWAT_Table->TestMode = TP_MODE;
			}
			else
			{
				pDM_SWAT_Table->RSSI_Trying = 2;
				pDM_SWAT_Table->TestMode = RSSI_MODE;

			}
                          
			//RT_TRACE(COMP_SWAS, DBG_LOUD, ("SWAS: Normal State -> Begin Trying!\n"));			
			pHalData->RSSI_sum_A = 0;
			pHalData->RSSI_cnt_A = 0;
			pHalData->RSSI_sum_B = 0;
			pHalData->RSSI_cnt_B = 0;
		} // end of try_flag=0
	}
	
	//1 4.Change TRX antenna
	if(nextAntenna != pDM_SWAT_Table->CurAntenna)
	{
	
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Change TX Antenna!\n "));
		//PHY_SetBBReg(Adapter, rFPGA0_XA_RFInterfaceOE, 0x300, nextAntenna); for 88C
		if(nextAntenna==MAIN_ANT)
		{
		    ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Next Antenna is RF PATH A\n "));
		    pathdiv_para = 0x02;   //02 to switchback to RF path A
		    fw_value = 0x03;
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
                 odm_PathDiversity_8192D(pDM_Odm, pathdiv_para);
#else
                 ODM_FillH2CCmd(Adapter, ODM_H2C_PathDiv,1,(pu1Byte)(&fw_value));	
#endif
		}	
	       else if(nextAntenna==AUX_ANT)
	       {
	           ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Next Antenna is RF PATH B\n "));
	           if(switchfirsttime==0)  // First Time To Enter Path Diversity
	           {
	               switchfirsttime=0x01;
                      pathdiv_para = 0x00;
			  fw_value=0x00;    // to backup RF Path A Releated Registers		  
					  
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
                     odm_PathDiversity_8192D(pDM_Odm, pathdiv_para);
#else
                     ODM_FillH2CCmd(Adapter, ODM_H2C_PathDiv,1,(pu1Byte)(&fw_value));	
                     //for(u1Byte n=0; n<80,n++)
                     //{
                     //delay_us(500);
			  ODM_delay_ms(500);
                     odm_PathDiversity_8192D(pDM_Odm, pathdiv_para);
			 		 
			 fw_value=0x01;   	// to backup RF Path A Releated Registers		 
                     ODM_FillH2CCmd(Adapter, ODM_H2C_PathDiv,1,(pu1Byte)(&fw_value));	
#endif	
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: FIRST TIME To DO PATH SWITCH!\n "));	
	           }		   
		    else
		    {
		        pathdiv_para = 0x01;
			 fw_value = 0x02;	
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
                     odm_PathDiversity_8192D(pDM_Odm, pathdiv_para);
#else
                     ODM_FillH2CCmd(Adapter, ODM_H2C_PathDiv,1,(pu1Byte)(&fw_value));	
#endif	
		    }		
	       }
           //   odm_PathDiversity_8192D(Adapter, pathdiv_para);
	}

	//1 5.Reset Statistics
	pDM_SWAT_Table->PreAntenna = pDM_SWAT_Table->CurAntenna;
	pDM_SWAT_Table->CurAntenna = nextAntenna;
	pDM_SWAT_Table->PreRSSI = curRSSI;
       //lastTxOkCnt = Adapter->TxStats.NumTxBytesUnicast;
       //lastRxOkCnt = Adapter->RxStats.NumRxBytesUnicast;
	   
	//1 6.Set next timer

	if(pDM_SWAT_Table->RSSI_Trying == 0)
		return;

	if(pDM_SWAT_Table->RSSI_Trying%2 == 0)
	{
		if(pDM_SWAT_Table->TestMode == TP_MODE)
		{
			if(TrafficLoad == TRAFFIC_HIGH)
			{
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 10 ); //ms
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 10 ms\n"));
#else
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 20 ); //ms
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 20 ms\n"));
#endif				
			}
			else if(TrafficLoad == TRAFFIC_LOW)
			{
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 50 ); //ms
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 50 ms\n"));
			}
		}
		else   // TestMode == RSSI_MODE
		{
			ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 500 ); //ms
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 500 ms\n"));
		}
	}
	else
	{
		if(pDM_SWAT_Table->TestMode == TP_MODE)
		{
			if(TrafficLoad == TRAFFIC_HIGH)
				
#if DEV_BUS_TYPE==RT_PCI_INTERFACE
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 90 ); //ms
				//ODM_RT_TRACE(pDM_Odm,ODM_COMP_PATH_DIV, ODM_DBG_LOUD, ("=PATH=: Test another antenna for 90 ms\n"));
#else		
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 180); //ms
#endif				
			else if(TrafficLoad == TRAFFIC_LOW)
				ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 100 ); //ms
		}
		else
			ODM_SetTimer( pDM_Odm, &pDM_Odm->PathDivSwitchTimer, 500 ); //ms
	}
}

//==================================================
//3 PathDiv End
//==================================================

VOID
odm_SetRespPath_92C(
	IN	PADAPTER	Adapter,
	IN	u1Byte	DefaultRespPath
	)
{
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;

	RT_TRACE(	COMP_SWAS, DBG_LOUD, ("odm_SetRespPath_92C: Select Response Path=%d\n",DefaultRespPath));
	if(DefaultRespPath != pDM_PDTable->DefaultRespPath)
	{
		if(DefaultRespPath == 0)
		{
			PlatformEFIOWrite1Byte(Adapter, 0x6D8, (PlatformEFIORead1Byte(Adapter, 0x6D8)&0xc0)|0x15);	
		}
		else
		{
			PlatformEFIOWrite1Byte(Adapter, 0x6D8, (PlatformEFIORead1Byte(Adapter, 0x6D8)&0xc0)|0x2A);
		}	
	}
	pDM_PDTable->DefaultRespPath = DefaultRespPath;
}


VOID
ODM_FillTXPathInTXDESC(
		IN	PADAPTER	Adapter,
		IN	PRT_TCB		pTcb,
		IN	pu1Byte		pDesc
)
{
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	u4Byte	TXPath;
	pPD_T	pDM_PDTable = &Adapter->DM_PDTable;

	//2011.09.05  Add by Luke Lee for path diversity
	if(pHalData->PathDivCfg == 1)
	{	
		TXPath = (pDM_PDTable->OFDMTXPath >> pTcb->macId) & BIT0;
		//RT_TRACE(	COMP_SWAS, DBG_LOUD, ("Fill TXDESC: macID=%d, TXPath=%d\n", pTcb->macId, TXPath));
		//SET_TX_DESC_TX_ANT_CCK(pDesc,TXPath);
		if(TXPath == 0)
		{
			SET_TX_DESC_TX_ANTL_92C(pDesc,1);
			SET_TX_DESC_TX_ANT_HT_92C(pDesc,1);
		}
		else
		{
			SET_TX_DESC_TX_ANTL_92C(pDesc,2);
			SET_TX_DESC_TX_ANT_HT_92C(pDesc,2);
		}
		TXPath = (pDM_PDTable->CCKTXPath >> pTcb->macId) & BIT0;
		if(TXPath == 0)
		{
			SET_TX_DESC_TX_ANT_CCK_92C(pDesc,1);
		}
		else
		{
			SET_TX_DESC_TX_ANT_CCK_92C(pDesc,2);
		}
	}
}

//Only for MP //Neil Chen--2012--0502--
VOID
odm_PathDivInit(
IN	PDM_ODM_T 	pDM_Odm)
{
	pPATHDIV_PARA	pathIQK = &pDM_Odm->pathIQK;

	pathIQK->org_2g_RegC14=0x0;
	pathIQK->org_2g_RegC4C=0x0;
	pathIQK->org_2g_RegC80=0x0;
	pathIQK->org_2g_RegC94=0x0;
	pathIQK->org_2g_RegCA0=0x0;
	pathIQK->org_5g_RegC14=0x0;
	pathIQK->org_5g_RegCA0=0x0;
	pathIQK->org_5g_RegE30=0x0;
	pathIQK->swt_2g_RegC14=0x0;
	pathIQK->swt_2g_RegC4C=0x0;
	pathIQK->swt_2g_RegC80=0x0;
	pathIQK->swt_2g_RegC94=0x0;
	pathIQK->swt_2g_RegCA0=0x0;
	pathIQK->swt_5g_RegC14=0x0;
	pathIQK->swt_5g_RegCA0=0x0;
	pathIQK->swt_5g_RegE30=0x0;

}

#endif	// #if (DM_ODM_SUPPORT_TYPE == ODM_WIN)

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN| ODM_CE))

VOID
odm_PHY_SaveAFERegisters(
	IN	PDM_ODM_T	pDM_Odm,
	IN	pu4Byte		AFEReg,
	IN	pu4Byte		AFEBackup,
	IN	u4Byte		RegisterNum
	)
{
	u4Byte	i;
	
	//RTPRINT(FINIT, INIT_IQK, ("Save ADDA parameters.\n"));
	for( i = 0 ; i < RegisterNum ; i++){
		AFEBackup[i] = ODM_GetBBReg(pDM_Odm, AFEReg[i], bMaskDWord);
	}
}

VOID
odm_PHY_ReloadAFERegisters(
	IN	PDM_ODM_T	pDM_Odm,
	IN	pu4Byte		AFEReg,
	IN	pu4Byte		AFEBackup,
	IN	u4Byte		RegiesterNum
	)
{
	u4Byte	i;

	//RTPRINT(FINIT, INIT_IQK, ("Reload ADDA power saving parameters !\n"));
	for(i = 0 ; i < RegiesterNum; i++)
	{
	
		ODM_SetBBReg(pDM_Odm, AFEReg[i], bMaskDWord, AFEBackup[i]);
	}
}

//
// Description:
//	Set Single/Dual Antenna default setting for products that do not do detection in advance.
//
// Added by Joseph, 2012.03.22
//
VOID
ODM_SingleDualAntennaDefaultSetting(
	IN		PDM_ODM_T		pDM_Odm
	)
{
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	pDM_SWAT_Table->ANTA_ON=TRUE;
	pDM_SWAT_Table->ANTB_ON=TRUE;
}



//2 8723A ANT DETECT
//
// Description:
//	Implement IQK single tone for RF DPK loopback and BB PSD scanning. 
//	This function is cooperated with BB team Neil. 
//
// Added by Roger, 2011.12.15
//
BOOLEAN
ODM_SingleDualAntennaDetection(
	IN		PDM_ODM_T		pDM_Odm,
	IN		u1Byte			mode
	)
{

	//HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(Adapter);
	//PDM_ODM_T		pDM_Odm = &pHalData->DM_OutSrc;
	pSWAT_T		pDM_SWAT_Table = &pDM_Odm->DM_SWAT_Table;
	u4Byte		CurrentChannel,RfLoopReg;
	u1Byte		n;
	u4Byte		Reg88c, Regc08, Reg874, Regc50;
	u1Byte		initial_gain = 0x5a;
	u4Byte		PSD_report_tmp;
	u4Byte		AntA_report = 0x0, AntB_report = 0x0,AntO_report=0x0;
	BOOLEAN		bResult = TRUE;
	u4Byte		AFE_Backup[16];
	u4Byte		AFE_REG_8723A[16] = {
					rRx_Wait_CCA, 	rTx_CCK_RFON, 
					rTx_CCK_BBON, 	rTx_OFDM_RFON,
					rTx_OFDM_BBON, 	rTx_To_Rx,
					rTx_To_Tx, 		rRx_CCK, 
					rRx_OFDM, 		rRx_Wait_RIFS, 
					rRx_TO_Rx,		rStandby,
					rSleep,			rPMPD_ANAEN, 	
					rFPGA0_XCD_SwitchControl, rBlue_Tooth};

	if(!(pDM_Odm->SupportICType & (ODM_RTL8723A|ODM_RTL8192C)))
		return bResult;

	if(!(pDM_Odm->SupportAbility&ODM_BB_ANT_DIV))
		return bResult;

	if(pDM_Odm->SupportICType == ODM_RTL8192C)
	{
		//Which path in ADC/DAC is turnned on for PSD: both I/Q
		ODM_SetBBReg(pDM_Odm, 0x808, BIT10|BIT11, 0x3);
		//Ageraged number: 8
		ODM_SetBBReg(pDM_Odm, 0x808, BIT12|BIT13, 0x1);
		//pts = 128;
		ODM_SetBBReg(pDM_Odm, 0x808, BIT14|BIT15, 0x0);
	}

	//1 Backup Current RF/BB Settings	
	
	CurrentChannel = ODM_GetRFReg(pDM_Odm, RF_PATH_A, ODM_CHANNEL, bRFRegOffsetMask);
	RfLoopReg = ODM_GetRFReg(pDM_Odm, RF_PATH_A, 0x00, bRFRegOffsetMask);
	ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, ODM_DPDT,MAIN_ANT);  // change to Antenna A(Ant_Main)
	// Step 1: USE IQK to transmitter single tone 

	ODM_StallExecution(10);
	
	//Store A Path Register 88c, c08, 874, c50
	Reg88c = ODM_GetBBReg(pDM_Odm, rFPGA0_AnalogParameter4, bMaskDWord);
	Regc08 = ODM_GetBBReg(pDM_Odm, rOFDM0_TRMuxPar, bMaskDWord);
	Reg874 = ODM_GetBBReg(pDM_Odm, rFPGA0_XCD_RFInterfaceSW, bMaskDWord);
	Regc50 = ODM_GetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskDWord);	
	
	// Store AFE Registers
	odm_PHY_SaveAFERegisters(pDM_Odm, AFE_REG_8723A, AFE_Backup, 16);	
	
	//Set PSD 128 pts
	ODM_SetBBReg(pDM_Odm, rFPGA0_PSDFunction, BIT14|BIT15, 0x0);  //128 pts
	
	// To SET CH1 to do
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, ODM_CHANNEL, bRFRegOffsetMask, 0x01);     //Channel 1
	
	// AFE all on step
	ODM_SetBBReg(pDM_Odm, rRx_Wait_CCA, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rTx_CCK_RFON, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rTx_CCK_BBON, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rTx_OFDM_RFON, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rTx_OFDM_BBON, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rTx_To_Rx, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rTx_To_Tx, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rRx_CCK, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rRx_OFDM, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rRx_Wait_RIFS, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rRx_TO_Rx, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rStandby, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rSleep, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rPMPD_ANAEN, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rFPGA0_XCD_SwitchControl, bMaskDWord, 0x6FDB25A4);
	ODM_SetBBReg(pDM_Odm, rBlue_Tooth, bMaskDWord, 0x6FDB25A4);

	// 3 wire Disable
	ODM_SetBBReg(pDM_Odm, rFPGA0_AnalogParameter4, bMaskDWord, 0xCCF000C0);
	
	//BB IQK Setting
	ODM_SetBBReg(pDM_Odm, rOFDM0_TRMuxPar, bMaskDWord, 0x000800E4);
	ODM_SetBBReg(pDM_Odm, rFPGA0_XCD_RFInterfaceSW, bMaskDWord, 0x22208000);

	//IQK setting tone@ 4.34Mhz
	ODM_SetBBReg(pDM_Odm, rTx_IQK_Tone_A, bMaskDWord, 0x10008C1C);
	ODM_SetBBReg(pDM_Odm, rTx_IQK, bMaskDWord, 0x01007c00);	


	//Page B init
	ODM_SetBBReg(pDM_Odm, rConfig_AntA, bMaskDWord, 0x00080000);
	ODM_SetBBReg(pDM_Odm, rConfig_AntA, bMaskDWord, 0x0f600000);
	ODM_SetBBReg(pDM_Odm, rRx_IQK, bMaskDWord, 0x01004800);
	ODM_SetBBReg(pDM_Odm, rRx_IQK_Tone_A, bMaskDWord, 0x10008c1f);
	ODM_SetBBReg(pDM_Odm, rTx_IQK_PI_A, bMaskDWord, 0x82150008);
	ODM_SetBBReg(pDM_Odm, rRx_IQK_PI_A, bMaskDWord, 0x28150008);
	ODM_SetBBReg(pDM_Odm, rIQK_AGC_Rsp, bMaskDWord, 0x001028d0);	

	//RF loop Setting
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x0, 0xFFFFF, 0x50008);	
	
	//IQK Single tone start
	ODM_SetBBReg(pDM_Odm, rFPGA0_IQK, 0xffffff00, 0x808000);
	ODM_SetBBReg(pDM_Odm, rIQK_AGC_Pts, bMaskDWord, 0xf8000000);
	ODM_StallExecution(1000);
	PSD_report_tmp=0x0;

	for (n=0;n<2;n++)
 	{
 		PSD_report_tmp =  GetPSDData(pDM_Odm, 14, initial_gain);	
		if(PSD_report_tmp >AntA_report)
			AntA_report=PSD_report_tmp;
	}

	PSD_report_tmp=0x0;

	ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, 0x300, AUX_ANT);  // change to Antenna B(8723A Ant Aux)
	ODM_StallExecution(10);	

	
	for (n=0;n<2;n++)
 	{
 		PSD_report_tmp =  GetPSDData(pDM_Odm, 14, initial_gain);	
		if(PSD_report_tmp > AntB_report)
			AntB_report=PSD_report_tmp;
	}

	// change to open case
	ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, 0x300, 0);  // change to Ant A and B all open case
	ODM_StallExecution(10);	
	
	for (n=0;n<2;n++)
 	{
 		PSD_report_tmp =  GetPSDData(pDM_Odm, 14, initial_gain);	
		if(PSD_report_tmp > AntO_report)
			AntO_report=PSD_report_tmp;
	}

	//Close IQK Single Tone function
	ODM_SetBBReg(pDM_Odm, rFPGA0_IQK, 0xffffff00, 0);
	PSD_report_tmp = 0x0;

	//1 Return to antanna A (ANT Main)
	ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, 0x300, MAIN_ANT);	
	ODM_SetBBReg(pDM_Odm, rFPGA0_AnalogParameter4, bMaskDWord, Reg88c);
	ODM_SetBBReg(pDM_Odm, rOFDM0_TRMuxPar, bMaskDWord, Regc08);
	ODM_SetBBReg(pDM_Odm, rFPGA0_XCD_RFInterfaceSW, bMaskDWord, Reg874);
	ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, 0x7F, 0x40);
	ODM_SetBBReg(pDM_Odm, rOFDM0_XAAGCCore1, bMaskDWord, Regc50);
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, RF_CHNLBW, bRFRegOffsetMask,CurrentChannel);
	ODM_SetRFReg(pDM_Odm, RF_PATH_A, 0x00, bRFRegOffsetMask,RfLoopReg);

	//Reload AFE Registers
	odm_PHY_ReloadAFERegisters(pDM_Odm, AFE_REG_8723A, AFE_Backup, 16);	

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_A[%d]= %d \n", 2416, AntA_report));	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_B[%d]= %d \n", 2416, AntB_report));	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("psd_report_O[%d]= %d \n", 2416, AntO_report));


	if(pDM_Odm->SupportICType == ODM_RTL8723A)
	{
		//2 Test Ant B based on Ant A is ON
		if(mode==ANTTESTB)
		{
			if(AntA_report >=	100)
			{
				if(AntB_report > (AntA_report+1))
				{
					pDM_SWAT_Table->ANTB_ON=FALSE;
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Single Antenna A\n"));		
				}	
				else
				{
					pDM_SWAT_Table->ANTB_ON=TRUE;
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Dual Antenna is A and B\n"));	
				}	
			}
			else
			{
							ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Need to check again\n"));
				pDM_SWAT_Table->ANTB_ON=FALSE; // Set Antenna B off as default 
				bResult = FALSE;
			}
		}	
		//2 Test Ant A and B based on DPDT Open
		else if(mode==ANTTESTALL)
		{
			if((AntO_report >=100)&(AntO_report <118))
			{
				if(AntA_report > (AntO_report+1))
				{
					pDM_SWAT_Table->ANTA_ON=FALSE;
					//RT_TRACE(COMP_ANTENNA, DBG_LOUD, ("ODM_AntennaDetection(): Antenna A is OFF\n"));	
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Ant A is OFF"));
				}	
				else
				{
					pDM_SWAT_Table->ANTA_ON=TRUE;
					//RT_TRACE(COMP_ANTENNA, DBG_LOUD, ("ODM_AntennaDetection(): Antenna A is ON\n"));
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Ant A is ON"));
				}

				if(AntB_report > (AntO_report+2))
				{
					pDM_SWAT_Table->ANTB_ON=FALSE;
					//RT_TRACE(COMP_ANTENNA, DBG_LOUD, ("ODM_AntennaDetection(): Antenna B is OFF\n"));	
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Ant B is OFF"));
				}	
				else
				{
					pDM_SWAT_Table->ANTB_ON=TRUE;
					//RT_TRACE(COMP_ANTENNA, DBG_LOUD, ("ODM_AntennaDetection(): Antenna B is ON\n"));
						ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD,("Ant B is ON"));
				}
				
				if((pDM_SWAT_Table->ANTA_ON==FALSE)&&(pDM_SWAT_Table->ANTB_ON==TRUE))
				{
					ODM_SetBBReg(pDM_Odm, rFPGA0_XA_RFInterfaceOE, 0x300, AUX_ANT);  //Change to AUX_ANT Due to MAIN_ANT is no connect
				}
				else if((pDM_SWAT_Table->ANTA_ON==FALSE)&&(pDM_SWAT_Table->ANTB_ON==FALSE)) //Error Condition
				{
					bResult=FALSE;   //Need to detect again
				}	
			}
			bResult=FALSE;	//need to detect again
		}
	}
	else if(pDM_Odm->SupportICType == ODM_RTL8192C)
	{
		if(AntA_report >=	100)
		{
			if(AntB_report > (AntA_report+2))
			{
				pDM_SWAT_Table->ANTA_ON=FALSE;
				pDM_SWAT_Table->ANTB_ON=TRUE;
				ODM_SetBBReg(pDM_Odm,  rFPGA0_XA_RFInterfaceOE, 0x300, AUX_ANT);
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Single Antenna B\n"));		
			}	
			else if(AntA_report > (AntB_report+2))
			{
				pDM_SWAT_Table->ANTA_ON=TRUE;
				pDM_SWAT_Table->ANTB_ON=FALSE;
				ODM_SetBBReg(pDM_Odm,  rFPGA0_XA_RFInterfaceOE, 0x300, MAIN_ANT);
				ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Single Antenna A\n"));
			}	
			else
			{
				pDM_SWAT_Table->ANTA_ON=TRUE;
				pDM_SWAT_Table->ANTB_ON=TRUE;
				RT_TRACE(COMP_ANTENNA, DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Dual Antenna \n"));
			}
		}
		else
		{
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_ANT_DIV, ODM_DBG_LOUD, ("ODM_SingleDualAntennaDetection(): Need to check again\n"));
			pDM_SWAT_Table->ANTA_ON=TRUE; // Set Antenna A on as default 
			pDM_SWAT_Table->ANTB_ON=FALSE; // Set Antenna B off as default 
			bResult = FALSE;
		}
	}
	return bResult;

}


#endif   // end odm_CE

#if (DM_ODM_SUPPORT_TYPE & (ODM_AP|ODM_ADSL))
#ifdef MP_TEST
VOID
odm_MPT_Write_DIG(
	IN	PDM_ODM_T		pDM_Odm,
	IN	u1Byte			CurIGValue
)
{
	pDIG_T	pDM_DigTable = &pDM_Odm->DM_DigTable;

	ODM_Write1Byte( pDM_Odm, ODM_REG(IGI_A,pDM_Odm), CurIGValue);

	if(pDM_Odm->RFType > ODM_1T1R)
		ODM_Write1Byte( pDM_Odm, ODM_REG(IGI_B,pDM_Odm), CurIGValue);

	if(pDM_Odm->SupportICType & ODM_IC_11AC_SERIES)
	{
		if(pDM_Odm->RFType > ODM_2T2R)
			ODM_Write1Byte( pDM_Odm, ODM_REG(IGI_C,pDM_Odm), CurIGValue);

		if(pDM_Odm->RFType > ODM_3T3R)
			ODM_Write1Byte( pDM_Odm, ODM_REG(IGI_D,pDM_Odm), CurIGValue);
	}

	pDM_DigTable->CurIGValue = CurIGValue;
	
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("CurIGValue = 0x%x\n", CurIGValue));
	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("pDM_Odm->RFType = 0x%x\n", pDM_Odm->RFType));
}

VOID
ODM_MPT_DIG(
	IN	PDM_ODM_T	pDM_Odm
	)
{
	PADAPTER					pAdapter	= pDM_Odm->Adapter;
	pDIG_T						pDM_DigTable = &pDM_Odm->DM_DigTable;
	PFALSE_ALARM_STATISTICS		pFalseAlmCnt = &pDM_Odm->FalseAlmCnt;
	u1Byte						CurrentIGI = pDM_DigTable->CurIGValue;
	u1Byte						DIG_Upper, DIG_Lower, C50, E50;
	u4Byte						RXOK_cal;

	if (!(pDM_Odm->priv->pshare->rf_ft_var.mp_specific && pDM_Odm->priv->pshare->mp_dig_on))
		return;

	ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("===> ODM_MPT_DIG, pBandType = %d\n", *pDM_Odm->pBandType));

	if (*pDM_Odm->pBandType == 0){
		DIG_Upper = 0x40;
		DIG_Lower = 0x20;
		}
	else{
		DIG_Upper = 0x40;
		DIG_Lower = 0x22;
	}		
	
	if (*pDM_Odm->pBandType == 1) {   // for 5G
		odm_FalseAlarmCounterStatistics( pDM_Odm);

		pDM_Odm->LastNumQryPhyStatusAll = pDM_Odm->NumQryPhyStatusAll;	
		pDM_Odm->NumQryPhyStatusAll = pDM_Odm->PhyDbgInfo.NumQryPhyStatusCCK + pDM_Odm->PhyDbgInfo.NumQryPhyStatusOFDM;	
		RXOK_cal = pDM_Odm->NumQryPhyStatusAll - pDM_Odm->LastNumQryPhyStatusAll;

		if (RXOK_cal == 0)
			pDM_Odm->RxPWDBAve = 0;
		else
			pDM_Odm->RxPWDBAve = pDM_Odm->RxPWDBAve/RXOK_cal;
			
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("RX OK = %d\n", RXOK_cal));
		ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("RSSI = %d\n", pDM_Odm->RxPWDBAve));
			
		if (RXOK_cal >= 70 && pDM_Odm->RxPWDBAve <= 30) 
		{
			if (CurrentIGI > 0x24)
				odm_MPT_Write_DIG(pDM_Odm, 0x24);
		} 
		else
		{
			if(pFalseAlmCnt->Cnt_all > 1000){
				CurrentIGI = CurrentIGI + 8;
			}
			else if(pFalseAlmCnt->Cnt_all > 200){
				CurrentIGI = CurrentIGI + 4;
			}
			else if (pFalseAlmCnt->Cnt_all > 50){
				CurrentIGI = CurrentIGI + 2;
			}
			else if (pFalseAlmCnt->Cnt_all < 2){
				CurrentIGI = CurrentIGI - 2;
			}
			
			if (CurrentIGI < DIG_Lower ){
				CurrentIGI = DIG_Lower;
			}
			else if(CurrentIGI > DIG_Upper){
				CurrentIGI = DIG_Upper;
			}
			
			C50 = ODM_Read1Byte( pDM_Odm, 0xc50);
			E50 = ODM_Read1Byte( pDM_Odm, 0xe50);
	                pDM_Odm->MPDIG_2G = FALSE;
			ODM_RT_TRACE(pDM_Odm,ODM_COMP_DIG, ODM_DBG_LOUD, ("DIG = (%x, %x), Cnt_all = %d, Cnt_Ofdm_fail = %d, Cnt_Cck_fail = %d\n", C50, E50, pFalseAlmCnt->Cnt_all, pFalseAlmCnt->Cnt_Ofdm_fail, pFalseAlmCnt->Cnt_Cck_fail));
		}
	}
	else {
		if (pDM_Odm->MPDIG_2G == FALSE)
			odm_MPT_Write_DIG(pDM_Odm, 0x20);
		pDM_Odm->MPDIG_2G = TRUE;
	}
	
	//pDM_DigTable->CurIGValue = CurrentIGI;
	
	//ODM_Write1Byte( pDM_Odm, rA_IGI_Jaguar, CurrentIGI);
	//ODM_Write1Byte( pDM_Odm, rB_IGI_Jaguar, CurrentIGI);

	odm_MPT_Write_DIG(pDM_Odm, CurrentIGI);
	
	ODM_SetTimer(pDM_Odm, &pDM_Odm->MPT_DIGTimer, 700);	
}	

VOID
odm_MPT_DIGCallback(
	IN	PDM_ODM_T	pDM_Odm
)
{
#if USE_WORKITEM
	PlatformScheduleWorkItem(&pDM_Odm->MPT_DIGWorkitem);
#else
	ODM_MPT_DIG(pDM_Odm);
#endif
}
#endif
#endif


