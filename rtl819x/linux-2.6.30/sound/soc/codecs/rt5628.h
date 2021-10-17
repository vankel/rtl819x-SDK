/*
 * rt5628.h  --  RT5628 ALSA SoC audio driver
 *
 * Copyright 2011 Realtek Microelectronics
 * Author: Johnny Hsu <johnnyhsu@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __RT5628_H__
#define __RT5628_H__

#define RT5628_RESET 				0x00
#define RT5628_SPK_OUT_VOL			0x02
#define RT5628_HP_OUT_VOL			0x04
#define RT5628_LINE_IN_VOL			0x0a
#define RT5628_DAC_DIG_VOL			0x0c
#define RT5628_SDV_CTRL_TIME			0X16
#define RT5628_OUT_MIX_CTRL			0x1c
#define RT5628_SDP_CTRL				0x34
#define RT5628_DAC_CLK_CTRL			0x38
#define RT5628_PWR_ADD1			0x3a
#define RT5628_PWR_ADD2			0x3c
#define RT5628_PWR_ADD3			0x3e
#define RT5628_GEN_CTRL				0x40
#define RT5628_GLB_CLK_CTRL			0x42
#define RT5628_PLL_CTRL				0x44
#define RT5628_JD_CTRL				0x5a
#define RT5628_MISC_CTRL1			0x5c
#define RT5628_MISC_CTRL2			0x5e
#define RT5628_AVC_CTRL				0x68
#define RT5628_PRIV_INDEX			0x6a
#define RT5628_PRIV_DATA			0x6c
#define RT5628_VENDOR_ID1	 	    	0x7c
#define RT5628_VENDOR_ID2	 	    	0x7e


/* global definition */
#define RT5628_L_MUTE				(0x1 << 15)
#define RT5628_L_MUTE_SFT			15
#define RT5628_VOL_L_MASK			(0x1f << 8)
#define RT5628_VOL_L_SFT			8
#define RT5628_R_MUTE				(0x1 << 7)
#define RT5628_R_MUTE_SFT			7
#define RT5628_VOL_R_MASK			(0x1f)
#define RT5628_VOL_R_SFT			0

/* LINE Input Volume (0x0a) */
#define RT5628_M_LV_L_HM_L			(0x1 << 15)
#define RT5628_M_LV_L_HM_L_SFT		15
#define RT5628_M_LV_L_SM			(0x1 << 14)
#define RT5628_M_LV_L_SM_SFT			14
#define RT5628_M_LV_R_HM_R			(0x1 << 7)
#define RT5628_M_LV_R_HM_R_SFT		7
#define RT5628_M_LV_R_SM			(0x1 << 6)
#define RT5628_M_LV_R_SM_SFT			6
#define RT5628_M_LINEIN_DIF			(0x1 << 5)
#define RT5628_M_LINEIN_DIF_SFT		5

/* Stereo DAC Digital Volume (0x0c) */
#define RT5628_M_DAC_L_HM_L			(0x1 << 15)
#define RT5628_M_DAC_L_HM_L_SFT		15
#define RT5628_M_DAC_L_SM			(0x1 << 14)
#define RT5628_M_DAC_L_SM_SFT			14
#define RT5628_M_DAC_R_HM_R			(0x1 << 7)
#define RT5628_M_DAC_R_HM_R_SFT		7
#define RT5628_M_DAC_R_SM			(0x1 << 6)
#define RT5628_M_DAC_R_SM_SFT			6

/* Output Mixer Control (0x1c) */
#define RT5628_OM_SPK_MOD_MASK		(0x3 << 14)
#define RT5628_OM_SPK_MOD_SFT			14
#define RT5628_OM_SPK_TYPE_MASK		(0x1 << 13)
#define RT5628_OM_SPK_SRC_MASK		(0x3 << 10)
#define RT5628_OM_SPK_SRC_SFT			10
#define RT5628_OM_HPL_SRC_MASK		(0x1 << 9)
#define RT5628_OM_HPL_SRC_SFT			9
#define RT5628_OM_HPR_SRC_MASK		(0x1 << 8)
#define RT5628_OM_HPR_SRC_SFT			8
#define RT5628_OM_DAC_HP_MASK		(0x1 << 1)
#define RT5628_OM_DAC_HP_SFT			1

/* MAIN Serial Data Port Control(Stereo I2S) (0x34) */
#define RT5628_MAIN_I2S_MODE_SEL		(0x1 << 15)
#define RT5628_MAIN_I2S_BCLK_POLARITY		(0x1 << 7)
#define RT5628_MAIN_I2S_DAC_LR_SWAP		(0x1 << 4)
#define RT5628_MAIN_I2S_DL_MASK		(0x3 << 2)
#define RT5628_MAIN_I2S_DL_16BITS		(0x0 << 2)
#define RT5628_MAIN_I2S_DL_20BITS		(0x1 << 2)
#define RT5628_MAIN_I2S_DL_24BITS		(0x2 << 2)
#define RT5628_MAIN_I2S_DF_MASK		(0x3)
#define RT5628_MAIN_I2S_DF_I2S			(0x0)
#define RT5628_MAIN_I2S_DF_LEFT_JUST		(0x1)
#define RT5628_MAIN_I2S_DF_MODEA		(0x2)
#define RT5628_MAIN_I2S_DF_MODEB		(0x3)

/* Stereo DAC CLK Control (0x38) */
#define RT5628_CLK_PD_MASK			(0x7 << 13)
#define RT5628_CLK_PD_SFT			13
#define RT5628_CLK_PD_1				(0x0 << 13)
#define RT5628_CLK_PD_2				(0x1 << 13)
#define RT5628_CLK_PD_4				(0x2 << 13)
#define RT5628_CLK_PD_8				(0x3 << 13)
#define RT5628_CLK_PD_16			(0x4 << 13)
#define RT5628_CLK_PD_32			(0x5 << 13)
#define RT5628_CLK_BCLK_MASK			(0x1 << 12)
#define RT5628_CLK_BCLK_SFT			12
#define RT5628_CLK_BCLK_32			(0x0 << 12)
#define RT5628_CLK_BCLK_16			(0x1 << 12)
#define RT5628_CLK_FC_MASK			(0x1 << 2)
#define RT5628_CLK_FC_SFT			2
#define RT5628_CLK_FC_256FS			(0x0 << 2)	
#define RT5628_CLK_FC_384FS			(0x1 << 2)	


/* power manage addition 1 (0x3a) */
#define RT5628_EN_MAIN_I2S			(0x1  <<  15)
#define RT5628_EN_MAIN_I2S_BIT			15
#define RT5628_ZCD				(0x1  <<  14)
#define RT5628_ZCD_BIT				14
#define RT5628_SOFTGEN				(0x1  <<  8)
#define RT5628_SOFTGEN_BIT			8
#define RT5628_HP_AMP				(0x1  <<  5)
#define RT5628_HP_AMP_BIT			5
#define RT5628_HP_EAMP				(0x1  <<  4)
#define RT5628_HP_EAMP_BIT			4

/* power manage addition 2 (0x3c) */
#define RT5628_CLSD				(0x1  <<  14)
#define RT5628_CLSD_BIT				14
#define RT5628_VREF				(0x1  <<  13)
#define RT5628_VREF_BIT				13
#define RT5628_PLL				(0x1  <<  12)
#define RT5628_PLL_BIT				12
#define RT5628_DAC_REF				(0x1  <<  10)
#define RT5628_DAC_REF_BIT			10
#define RT5628_DAC_L				(0x1  <<  9)
#define RT5628_DAC_L_BIT			9
#define RT5628_DAC_R				(0x1  <<  8)
#define RT5628_DAC_R_BIT			8
#define RT5628_D2M_L_DIR			(0x1  <<  7)
#define RT5628_D2M_L_DIR_BIT			7
#define RT5628_D2M_R_DIR			(0x1  <<  6)
#define RT5628_D2M_R_DIR_BIT			6
#define RT5628_HPM_L				(0x1  <<  5)
#define RT5628_HPM_L_BIT			5
#define RT5628_HPM_R				(0x1  <<  4)
#define RT5628_HPM_R_BIT			4
#define RT5628_SPM				(0x1  <<  3)
#define RT5628_SPM_BIT				3

/* power manage addition 3 (0x3e) */
#define RT5628_MB				(0x1  <<  15)
#define RT5628_MB_BIT				15
#define RT5628_SPK_L_VOL			(0x1  <<  12)
#define RT5628_SPK_L_VOL_BIT			12
#define RT5628_HP_L_VOL			(0x1  <<  10)
#define RT5628_HP_L_VOL_BIT			10
#define RT5628_HP_R_VOL			(0x1  <<  9)
#define RT5628_HP_R_VOL_BIT			9
#define RT5628_LI_L_VOL				(0x1  <<  7)
#define RT5628_LI_L_VOL_BIT			7
#define RT5628_LI_R_VOL				(0x1  <<  6)
#define RT5628_LI_R_VOL_BIT			6

/* General Purpose Control (0x40) */
#define RT5628_SPK_RATIO_MASK			(0x7 << 9)
#define RT5628_SPK_RATIO_SFT			9

/* Global CLK Control (0x42) */
#define RT5628_SYSCLK_SEL_MASK		(0x1 << 15)
#define RT5628_SYSCLK_SEL_SFT			15
#define RT5628_SYSCLK_SEL_MCLK			(0x0 << 15)
#define RT5628_SYSCLK_SEL_PLL			(0x1 << 15)
#define RT5628_PLL_SEL_MASK			(0x1 << 14)
#define RT5628_PLL_SEL_MCLK			(0x0 << 14)
#define RT5628_PLL_SEL_BCLK			(0x1 << 14)

/* misc2 control (0x5e) */
#define RT5628_EN_VERF_FASTB			(0x1  <<  15)
#define RT5628_EN_THERMAL_SHUTDOWN		(0x1  <<  14)
#define RT5628_EN_DP2_HP			(0x1  <<  9)
#define RT5628_EN_DP1_HP			(0x1  <<  8)
#define RT5628_EN_SMT_HP_L			(0x1  <<  7)
#define RT5628_EN_SMT_HP_R			(0x1  <<  6)
#define RT5628_SMT_TRIG				(0x1  <<  5)
#define RT5628_MUTE_DAC_L			(0x1  <<  3)
#define RT5628_MUTE_DAC_R			(0x1  <<  2)


struct rt5628_setup_data
{
	int i2c_bus;
	int i2c_address;
};

extern struct snd_soc_dai rt5628_dai;
extern struct snd_soc_codec_device soc_codec_dev_rt5628;

#endif /*__RT5628_H__*/
