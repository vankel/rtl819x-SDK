#ifndef __HAL88XXPHYCFG_H__
#define __HAL88XXPHYCFG_H__

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Hal88XXPhyCfg.h
	
Abstract:
	Defined HAL 88XX PHY BB setting functions
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2012-11-14 Eric              Create.	
--*/



typedef enum _RF88XX_RADIO_PATH_ {
	RF88XX_PATH_A = 0,			//Radio Path A
	RF88XX_PATH_B = 1,			//Radio Path B
	RF88XX_PATH_MAX				//Max RF number 
} RFRF88XX_RADIO_PATH_E, *PRFRF88XX_RADIO_PATH_E;

typedef enum _BAND_TYPE{
	RF88XX_BAND_ON_2_4G = 0,
	RF88XX_BAND_ON_5G,
	RF88XX_BAND_ON_BOTH,
	RF88XX_BANDMAX
}RF88XX_BAND_TYPE,*PRF88XX_BAND_TYPE;

typedef enum _RF88XX_HT_CHANNEL_WIDTH {
	RF88XX_HT_CHANNEL_WIDTH_20		= 0,
	RF88XX_HT_CHANNEL_WIDTH_20_40	= 1,
	RF88XX_HT_CHANNEL_WIDTH_80		= 2,
	RF88XX_HT_CHANNEL_WIDTH_160	    = 3,
	RF88XX_HT_CHANNEL_WIDTH_10		= 4
}RF88XX_HT_CHANNEL_WIDTH,*PRF88XXHT_CHANNEL_WIDTH;

typedef enum _RF88XX_HT_CHANNEL_WIDTH_AC {
	RF88XX_HT_CHANNEL_WIDTH_AC_20	= 0,
	RF88XX_HT_CHANNEL_WIDTH_AC_40	= 1, 
	RF88XX_HT_CHANNEL_WIDTH_AC_80 	= 2,
	RF88XX_HT_CHANNEL_WIDTH_AC_160	= 3,
	RF88XX_HT_CHANNEL_WIDTH_AC_10	= 4,
	RF88XX_HT_CHANNEL_WIDTH_AC_5 	= 5
}RF88XX_HT_CHANNEL_WIDTH_AC,*PRF88XX_HT_CHANNEL_WIDTH_AC;


u4Byte
phy_CalculateBitShift_88XX(
    IN u4Byte BitMask
);    

u4Byte
PHY_QueryBBReg_88XX(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          RegAddr,
    IN  u4Byte          BitMask
);

void 
PHY_SetBBReg_88XX(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          RegAddr,
    IN  u4Byte          BitMask,    
    IN  u4Byte          Data        
);

void 
PHY_SetRFReg_88XX_AC
(
    IN  HAL_PADAPTER                Adapter, 
    IN  u4Byte                      eRFPath,
    IN  u4Byte                      RegAddr,    
    IN  u4Byte                      BitMask,
    IN  u4Byte                      Data
);

u4Byte
PHY_QueryRFReg_88XX_AC(
    IN  HAL_PADAPTER                Adapter, 
    IN  u4Byte                      eRFPath,
    IN  u4Byte                      RegAddr,    
    IN  u4Byte                      BitMask
);

void 
SwBWMode88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          bandwidth,
    IN  s4Byte          offset
);


void 
SetChannelPara88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          channel,
    IN  s4Byte          offset  
);

void
CheckBand88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          RequestChannel
);


void SwitchWirelessBand88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          RequestChannel,
    IN  u1Byte          Band
);


void UpdateBBRFVal88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u1Byte          channel,
    IN  s4Byte          offset      
);


RT_STATUS 
PHYSetCCKTxPower88XX_AC(
        IN  HAL_PADAPTER    Adapter, 
        IN  u1Byte          channel
);

RT_STATUS 
PHYSetOFDMTxPower88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u1Byte          channel
);


void 
CalOFDMTxPower5G_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u1Byte          ch_idx
);

void 
CalOFDMTxPower2G_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u1Byte          ch_idx
);

s1Byte
convert_diff_88XX_AC(
    IN s1Byte value
);

void 
Write_1S_A_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_HT1S_A_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_VHT1S_A_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);


void 
Write_2S_A_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_HT2S_A_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_VHT2S_A_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_OFDM_A_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_OFDM_B_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_1S_B_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_HT1S_B_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_VHT1S_B_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_2S_B_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_HT2S_B_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
Write_VHT2S_B_88XX_AC(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          writeVal
);

void 
use_DefaultOFDMTxPowerPathA88XX_AC(
    IN  HAL_PADAPTER    Adapter
);

void 
use_DefaultOFDMTxPowerPathB88XX_AC(
    IN  HAL_PADAPTER    Adapter
);


RT_STATUS
PHYSetOFDMTxPower88XX_N(
        IN  HAL_PADAPTER    Adapter, 
        IN  u1Byte          channel
);

RT_STATUS
PHYSetCCKTxPower88XX_N(
        IN  HAL_PADAPTER    Adapter, 
        IN  u1Byte          channel
);

void 
SwBWMode88XX_N(
    IN  HAL_PADAPTER    Adapter, 
    IN  u4Byte          bandwidth,
    IN  s4Byte          offset
);

void 
PHY_SetRFReg_88XX_N
(
    IN  HAL_PADAPTER                Adapter, 
    IN  u4Byte                      eRFPath,
    IN  u4Byte                      RegAddr,    
    IN  u4Byte                      BitMask,
    IN  u4Byte                      Data
);

u4Byte
PHY_QueryRFReg_88XX_N
(
    IN  HAL_PADAPTER                Adapter, 
    IN  u4Byte                      eRFPath,
    IN  u4Byte                      RegAddr,    
    IN  u4Byte                      BitMask
);

#if 0
s4Byte 
get_tx_tracking_index_88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  s4Byte  channel,
    IN  s4Byte  i,
    IN  s4Byte  delta,    
    IN  s4Byte  is_decrease,
    IN  s4Byte  is_CCK    
);


void 
set_CCK_swing_index_88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  s2Byte          CCK_index
);

void TXPowerTracking_ThermalMeter_88XX
(
    IN  HAL_PADAPTER    Adapter
);
#endif
RT_STATUS 
AddTxPower88XX_AC(
        IN  HAL_PADAPTER    Adapter, 
        IN  s1Byte          index
);
#if (IS_RTL8192E_SERIES || IS_RTL8881A_SERIES)
BOOLEAN
IsBBRegRange88XX
(
    IN  HAL_PADAPTER                Adapter, 
    IN  u4Byte                      RegAddr
);
#endif //#if (IS_RTL8192E_SERIES || IS_RTL8881A_SERIES)


#if (IS_RTL8813A_SERIES)
BOOLEAN
IsBBRegRange88XX_V1
(
    IN  HAL_PADAPTER                Adapter, 
    IN  u4Byte                      RegAddr
);
#endif //#if (IS_RTL8813A_SERIES)

#endif // #ifndef __HAL88XXPHYCFG_H__

