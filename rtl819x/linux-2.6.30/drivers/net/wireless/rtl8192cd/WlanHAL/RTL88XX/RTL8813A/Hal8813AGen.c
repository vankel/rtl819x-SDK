/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Hal8813AGen.c
	
Abstract:
	Defined RTL8813A HAL Function
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2012-05-28 Filen            Create.	
--*/

#ifndef __ECOS
#include "HalPrecomp.h"
#else
#include "../../HalPrecomp.h"
#endif

#include "data_AGC_TAB_8813A.c"
#include "data_MAC_REG_8813A.c"
#include "data_PHY_REG_8813A.c"
//#include "data_PHY_REG_1T_8813A.c"
#include "data_PHY_REG_MP_8813A.c"
#include "data_PHY_REG_PG_8813A.c"
#include "data_RadioA_8813A.c"
#include "data_RadioB_8813A.c"
#include "data_RadioC_8813A.c"
#include "data_RadioD_8813A.c"
#include "data_rtl8813Afw.c"
//#include "data_RTL8813AFW_Test_T.c"

// High Power
#if CFG_HAL_HIGH_POWER_EXT_PA
#include "data_AGC_TAB_8813A_hp.c"
#include "data_PHY_REG_8813A_hp.c"
#include "data_RadioA_8813A_hp.c"
#include "data_RadioB_8813A_hp.c"
#endif

// Power Tracking
#include "data_TxPowerTrack_AP_8813A.c"


//3 MACDM
//default
#include "data_MACDM_def_high_8813A.c"
#include "data_MACDM_def_low_8813A.c"
#include "data_MACDM_def_normal_8813A.c"
//general
#include "data_MACDM_gen_high_8813A.c"
#include "data_MACDM_gen_low_8813A.c"
#include "data_MACDM_gen_normal_8813A.c"
//txop
#include "data_MACDM_txop_high_8813A.c"
#include "data_MACDM_txop_low_8813A.c"
#include "data_MACDM_txop_normal_8813A.c"
//criteria
#include "data_MACDM_state_criteria_8813A.c"


#define VAR_MAPPING(dst,src) \
	u1Byte *data_##dst##_start = &data_##src[0]; \
	u1Byte *data_##dst##_end   = &data_##src[sizeof(data_##src)];

VAR_MAPPING(AGC_TAB_8813A, AGC_TAB_8813A);
VAR_MAPPING(MAC_REG_8813A, MAC_REG_8813A);
VAR_MAPPING(PHY_REG_8813A, PHY_REG_8813A);
//VAR_MAPPING(PHY_REG_1T_8813A, PHY_REG_1T_8813A);
VAR_MAPPING(PHY_REG_PG_8813A, PHY_REG_PG_8813A);
VAR_MAPPING(PHY_REG_MP_8813A, PHY_REG_MP_8813A);
VAR_MAPPING(RadioA_8813A, RadioA_8813A);
VAR_MAPPING(RadioB_8813A, RadioB_8813A);
VAR_MAPPING(RadioC_8813A, RadioC_8813A);
VAR_MAPPING(RadioD_8813A, RadioD_8813A);
VAR_MAPPING(rtl8813Afw, rtl8813Afw);

// High Power
#if CFG_HAL_HIGH_POWER_EXT_PA
VAR_MAPPING(AGC_TAB_8813A_hp, AGC_TAB_8813A_hp);
VAR_MAPPING(PHY_REG_8813A_hp, PHY_REG_8813A_hp);
VAR_MAPPING(RadioA_8813A_hp, RadioA_8813A_hp);
VAR_MAPPING(RadioB_8813A_hp, RadioB_8813A_hp);
#endif

// Power Tracking
VAR_MAPPING(TxPowerTrack_AP_8813A, TxPowerTrack_AP_8813A);


//3 MACDM
VAR_MAPPING(MACDM_def_high_8813A, MACDM_def_high_8813A);
VAR_MAPPING(MACDM_def_low_8813A, MACDM_def_low_8813A);
VAR_MAPPING(MACDM_def_normal_8813A, MACDM_def_normal_8813A);

VAR_MAPPING(MACDM_gen_high_8813A, MACDM_gen_high_8813A);
VAR_MAPPING(MACDM_gen_low_8813A, MACDM_gen_low_8813A);
VAR_MAPPING(MACDM_gen_normal_8813A, MACDM_gen_normal_8813A);

VAR_MAPPING(MACDM_txop_high_8813A, MACDM_txop_high_8813A);
VAR_MAPPING(MACDM_txop_low_8813A, MACDM_txop_low_8813A);
VAR_MAPPING(MACDM_txop_normal_8813A, MACDM_txop_normal_8813A);

VAR_MAPPING(MACDM_state_criteria_8813A, MACDM_state_criteria_8813A);



