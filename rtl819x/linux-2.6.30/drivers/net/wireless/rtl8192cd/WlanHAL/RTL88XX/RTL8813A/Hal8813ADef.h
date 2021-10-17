#ifndef __HAL8813A_DEF_H__
#define __HAL8813A_DEF_H__

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Hal8813ADef.h
	
Abstract:
	Defined HAL 8813A data structure & Define
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2013-05-28 Filen            Create.	
--*/

extern u1Byte *data_AGC_TAB_8813A_start,    *data_AGC_TAB_8813A_end;
extern u1Byte *data_MAC_REG_8813A_start,    *data_MAC_REG_8813A_end;
extern u1Byte *data_PHY_REG_8813A_start,    *data_PHY_REG_8813A_end;
//extern u1Byte *data_PHY_REG_1T_8813A_start, *data_PHY_REG_1T_8813A_end;
extern u1Byte *data_PHY_REG_MP_8813A_start, *data_PHY_REG_MP_8813A_end;
extern u1Byte *data_PHY_REG_PG_8813A_start, *data_PHY_REG_PG_8813A_end;
extern u1Byte *data_RadioA_8813A_start,     *data_RadioA_8813A_end;
extern u1Byte *data_RadioB_8813A_start,     *data_RadioB_8813A_end;
extern u1Byte *data_RadioC_8813A_start,     *data_RadioC_8813A_end;
extern u1Byte *data_RadioD_8813A_start,     *data_RadioD_8813A_end;


//High Power
#if CFG_HAL_HIGH_POWER_EXT_PA
extern u1Byte *data_AGC_TAB_8813A_hp_start,    *data_AGC_TAB_8813A_hp_end;
extern u1Byte *data_PHY_REG_8813A_hp_start,    *data_PHY_REG_8813A_hp_end;
extern u1Byte *data_RadioA_8813A_hp_start,     *data_RadioA_8813A_hp_end;
extern u1Byte *data_RadioB_8813A_hp_start,     *data_RadioB_8813A_hp_end;
#endif

// FW
extern u1Byte *data_rtl8813Afw_start,         *data_rtl8813Afw_end;
extern u1Byte *data_rtl8813AfwMP_start,       *data_rtl8813AfwMP_end;

// Power Tracking
extern u1Byte *data_TxPowerTrack_AP_8813A_start,    *data_TxPowerTrack_AP_8813A_end;


//3 MACDM
//default
extern u1Byte *data_MACDM_def_high_8813A_start, *data_MACDM_def_high_8813A_end;
extern u1Byte *data_MACDM_def_low_8813A_start, *data_MACDM_def_low_8813A_end;
extern u1Byte *data_MACDM_def_normal_8813A_start, *data_MACDM_def_normal_8813A_end;

//general
extern u1Byte *data_MACDM_gen_high_8813A_start, *data_MACDM_gen_high_8813A_end;
extern u1Byte *data_MACDM_gen_low_8813A_start, *data_MACDM_gen_low_8813A_end;
extern u1Byte *data_MACDM_gen_normal_8813A_start, *data_MACDM_gen_normal_8813A_end;

//txop
extern u1Byte *data_MACDM_txop_high_8813A_start, *data_MACDM_txop_high_8813A_end;
extern u1Byte *data_MACDM_txop_low_8813A_start, *data_MACDM_txop_low_8813A_end;
extern u1Byte *data_MACDM_txop_normal_8813A_start, *data_MACDM_txop_normal_8813A_end;

//criteria
extern u1Byte *data_MACDM_state_criteria_8813A_start, *data_MACDM_state_criteria_8813A_end;





#endif  //__HAL8813A_DEF_H__


