/*
 * rt5633.c  --  RT5633 ALSA SoC audio codec driver
 *
 * Copyright 2011 Realtek Semiconductor Corp.
 * Author: Johnny Hsu <johnnyhsu@realtek.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/spi/spi.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/initval.h>
#include <sound/tlv.h>

#include "rt5633.h"

#define ALC5633_I2C_ADDR	0x38

#define SOC_ENUM_DOUBLE_DECL(name, xreg, xshift_l, xshift_r, xtexts) \
	struct soc_enum name = SOC_ENUM_DOUBLE(xreg, xshift_l, xshift_r, \
						ARRAY_SIZE(xtexts), xtexts)
#define SOC_ENUM_SINGLE_DECL(name, xreg, xshift, xtexts) \
	SOC_ENUM_DOUBLE_DECL(name, xreg, xshift, xshift, xtexts)

static unsigned int BLCK_FREQ=32;	//32fs,bitclk is 32 fs

struct rt5633_priv {
	//bard enum snd_soc_control_type control_type;
	struct snd_soc_codec *codec;
	int master;
	int sysclk;
};

extern unsigned int serial_in_i2c(unsigned int addr, int offset);
extern unsigned int serial_out_i2c(unsigned int addr, int offset, int value);

static const u16 rt5633_reg[RT5633_VENDOR_ID2 + 1] = {
	[RT5633_RESET] = 0x0001,
	[RT5633_SPK_OUT_VOL] = 0xe000,
	[RT5633_HP_OUT_VOL] = 0x8080,
	[RT5633_AUXOUT_VOL] = 0xc080,
	[RT5633_LINE_IN_1_VOL] = 0x0808,
	[RT5633_LINE_IN_2_VOL] = 0x0808,
	[RT5633_MIC_CTRL_1] = 0x0808,
	[RT5633_REC_MIXER_CTRL] = 0x7f7f,
	[RT5633_HPMIXER_CTRL] = 0x3f3f,
	[RT5633_AUXMIXER_CTRL] = 0x3f3f,
	[RT5633_SPKMIXER_CTRL] = 0x00ff,
	[RT5633_SPK_AMP_CTRL] = 0x8000,
	[RT5633_SDP_CTRL] = 0x8000,
	[RT5633_STEREO_AD_DA_CLK_CTRL] = 0x2000,
	[RT5633_GEN_PUR_CTRL_1] = 0x0c00,
	[RT5633_DIG_BEEP_IRQ_CTRL] = 0x0280,
	[RT5633_GEN_PUR_CTRL_2] = 0x40c0,
	[RT5633_DEPOP_CTRL_2] = 0x3000,
	[RT5633_ZC_SM_CTRL_1] = 0x0489,
	[RT5633_ZC_SM_CTRL_2] = 0x5ffe,
	[RT5633_ALC_CTRL_1] = 0x0206,
	[RT5633_ALC_CTRL_3] = 0x2000,
	[RT5633_PSEUDO_SPATL_CTRL] = 0x0553,
	[RT5633_EQ_CTRL_1] = 0x1000,
	[RT5633_VERSION] = 0x0002,
	[RT5633_VENDOR_ID1] = 0x10ec,
	[RT5633_VENDOR_ID2] = 0x6179,
};

struct rt5633_init_reg {
	u8 reg;
	u16 val;
};

static struct rt5633_init_reg init_list[] = {
	{RT5633_SPK_OUT_VOL		, 0x8000},//speaker output volume is 0db by default
	{RT5633_SPK_HP_MIXER_CTRL	, 0x0020},//HP from HP_VOL	
	{RT5633_HP_OUT_VOL 		, 0xc8c8},//HP output volume is -12 db by default
	{RT5633_AUXOUT_VOL		, 0x0010},//Auxout volume is 0db by default
	{RT5633_REC_MIXER_CTRL		, 0x7dff},//ADC Record Mixer Control,MIC1
	{RT5633_MIC_CTRL_2		, 0x5500},//boost 40db
	{RT5633_HPMIXER_CTRL		, 0x3e3e},//"HP Mixer Control"
	//{RT5633_AUXMIXER_CTRL		, 0x3e3e},//"AUX Mixer Control"
	{RT5633_SPKMIXER_CTRL		, 0x08fc},//"SPK Mixer Control"
	{RT5633_ZC_SM_CTRL_1		, 0x0001},	//Disable Zero Cross
	{RT5633_ZC_SM_CTRL_2		, 0x3000},	//Disable Zero cross
	{RT5633_MIC_CTRL_1       		, 0x8808}, //set mic1 to differnetial mode	
	{RT5633_DEPOP_CTRL_2		, 0xB000},
	{RT5633_PRI_REG_ADD		, 0x0056},
	{RT5633_PRI_REG_DATA		, 0x303f},	
	//JD setting	
	//{RT5633_ZC_SM_CTRL_1		, 0x04b0},	
	//{RT5633_ZC_SM_CTRL_2		, 0x3000},
	//{RT5633_JACK_DET_CTRL           , 0x6e00},		
};
#define RT5633_INIT_REG_LEN ARRAY_SIZE(init_list)

static int rt5633_reg_init(struct snd_soc_codec *codec)
{
	int i;
	for (i = 0; i < RT5633_INIT_REG_LEN; i++)
		snd_soc_write(codec, init_list[i].reg, init_list[i].val);	
	return 0;
}

static int rt5633_index_sync(struct snd_soc_codec *codec)
{
	int i;

	for (i = 0; i < RT5633_INIT_REG_LEN; i++)
		if (RT5633_PRI_REG_ADD == init_list[i].reg ||
			RT5633_PRI_REG_DATA == init_list[i].reg)
			snd_soc_write(codec, init_list[i].reg,
					init_list[i].val);

	return 0;
}

/**
 * rt5633_read - Read register.
 * @codec: SoC audio codec device.
 * @reg: The register index.
 *
 * Returns value of register for success or negative error code.
 */
static unsigned int rt5633_read(struct snd_soc_codec *codec, unsigned int reg)
{
	return serial_in_i2c(ALC5633_I2C_ADDR, reg);
}

/**
 * rt5633_cache_write: Set the value of a given register in the cache.
 *
 * @codec: SoC audio codec device.
 * @reg: The register index.
 * @value: The new register value.
 */
static int rt5633_cache_write(struct snd_soc_codec *codec,
			unsigned int reg, unsigned int value)
{
	u16 *cache = codec->reg_cache;
	if (reg > RT5633_VENDOR_ID2)
		return -EINVAL;
	cache[reg] = value;

	return 0;
}

/**
 * rt5633_write - Write register.
 * @codec: SoC audio codec device.
 * @reg: The register index.
 * @value: The new register value.
 *
 * Returns 0 for success or negative error code.
 */
static int rt5633_write(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	u8 data[3];
	int ret;

	data[0] = reg;
	data[1] = (value & 0xff00) >> 8;
	data[2] = value & 0x00ff;

	
	ret = rt5633_cache_write(codec, reg, value);
	if (ret < 0)
		return -EIO;
		
	//pr_info("%s:%d, reg=%X, value=%X\n", __FUNCTION__, __LINE__, reg, value);
	serial_out_i2c(ALC5633_I2C_ADDR, reg, value);

	return 0;
}

/**
 * rt5633_index_write - Write private register.
 * @codec: SoC audio codec device.
 * @reg: Private register index.
 * @value: Private register Data.
 *
 * Modify private register for advanced setting. It can be written through
 * private index (0x6a) and data (0x6c) register.
 *
 * Returns 0 for success or negative error code.
 */
static int rt5633_index_write(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	int ret;

	ret = snd_soc_write(codec, RT5633_PRI_REG_ADD, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private addr: %d\n", ret);
		goto err;
	}
	ret = snd_soc_write(codec, RT5633_PRI_REG_DATA, value);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private value: %d\n", ret);
		goto err;
	}
	return 0;

err:
	return ret;
}

/**
 * rt5633_index_read - Read private register.
 * @codec: SoC audio codec device.
 * @reg: Private register index.
 *
 * Read advanced setting from private register. It can be read through
 * private index (0x6a) and data (0x6c) register.
 *
 * Returns private register value or negative error code.
 */
static unsigned int rt5633_index_read(
	struct snd_soc_codec *codec, unsigned int reg)
{
	int ret;

	ret = snd_soc_write(codec, RT5633_PRI_REG_ADD, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private addr: %d\n", ret);
		return ret;
	}
	return snd_soc_read(codec, RT5633_PRI_REG_DATA);
}

/**
 * rt5633_index_update_bits - update private register bits
 * @codec: audio codec
 * @reg: Private register index.
 * @mask: register mask
 * @value: new value
 *
 * Writes new register value.
 *
 * Returns 1 for change, 0 for no change, or negative error code.
 */
static int rt5633_index_update_bits(struct snd_soc_codec *codec,
	unsigned int reg, unsigned int mask, unsigned int value)
{
	unsigned int old, new;
	int change, ret;

	ret = rt5633_index_read(codec, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to read private reg: %d\n", ret);
		goto err;
	}

	old = ret;
	new = (old & ~mask) | (value & mask);
	change = old != new;
	if (change) {
		ret = rt5633_index_write(codec, reg, new);
		if (ret < 0) {
			dev_err(codec->dev,
				"Failed to write private reg: %d\n", ret);
			goto err;
		}
	}
	return change;

err:
	return ret;
}

static int rt5633_reset(struct snd_soc_codec *codec)
{
	return snd_soc_write(codec, RT5633_RESET, 0);
}

static const DECLARE_TLV_DB_SCALE(out_vol_tlv, -4650, 150, 0);
static const DECLARE_TLV_DB_SCALE(in_vol_tlv, -3450, 150, 0);
/* {0, +20, +24, +30, +35, +40, +44, +50, +52} dB */
static unsigned int mic_bst_tlv[] = {
	TLV_DB_RANGE_HEAD(7),
	0, 0, TLV_DB_SCALE_ITEM(0, 0, 0),
	1, 1, TLV_DB_SCALE_ITEM(2000, 0, 0),
	2, 2, TLV_DB_SCALE_ITEM(2400, 0, 0),
	3, 5, TLV_DB_SCALE_ITEM(3000, 500, 0),
	6, 6, TLV_DB_SCALE_ITEM(4400, 0, 0),
	7, 7, TLV_DB_SCALE_ITEM(5000, 0, 0),
	8, 8, TLV_DB_SCALE_ITEM(5200, 0, 0),
};

static const char *rt5633_auxout_mode[] = {"Differential", "Single ended"};

static const SOC_ENUM_SINGLE_DECL(rt5633_auxout_mode_enum,
	RT5633_AUXOUT_VOL, RT5633_AUXOUT_MODE_SEL_SFT, rt5633_auxout_mode);

static const char *rt5633_input_mode[] = {"Single ended", "Differential"};

static const SOC_ENUM_SINGLE_DECL(rt5633_mic1_mode_enum,
	RT5633_MIC_CTRL_1, RT5633_MIC_1_MODE_SEL_SFT, rt5633_input_mode);

static const SOC_ENUM_SINGLE_DECL(rt5633_mic2_mode_enum,
	RT5633_MIC_CTRL_1, RT5633_MIC_2_MODE_SEL_SFT, rt5633_input_mode);

static const SOC_ENUM_SINGLE_DECL(rt5633_spkout_mode_enum,
	RT5633_SPK_OUT_VOL, RT5633_SPK_CLSAB_M_SFT, rt5633_input_mode);

static const char *rt5633_spon_clsd_ctl_sel[] = {"RN", "RP", "LN", "VMID"};

static const SOC_ENUM_SINGLE_DECL(rt5633_spon_clsd_ctl_enum,
	RT5633_SPK_OUT_VOL, RT5633_SPON_CTL_SFT, rt5633_spon_clsd_ctl_sel);

static const char *rt5633_spon_clsab_ctl_sel[] = {"VMID", "RP", "LN", "RN"};

static const SOC_ENUM_SINGLE_DECL(rt5633_spon_clsab_ctl_enum,
	RT5633_SPK_OUT_VOL, RT5633_SPON_CTL_SFT, rt5633_spon_clsab_ctl_sel);

static const struct snd_kcontrol_new rt5633_snd_controls[] = {
	SOC_ENUM("MIC1 Mode Control", rt5633_mic1_mode_enum),
	SOC_SINGLE_TLV("MIC1 Boost", RT5633_MIC_CTRL_2,
		RT5633_MIC1_BOOST_CTRL_SFT, 8, 0, mic_bst_tlv),
	SOC_ENUM("MIC2 Mode Control", rt5633_mic2_mode_enum),
	SOC_SINGLE_TLV("MIC2 Boost", RT5633_MIC_CTRL_2,
		RT5633_MIC2_BOOST_CTRL_SFT, 8, 0, mic_bst_tlv),
	SOC_ENUM("Classab Mode Control", rt5633_spkout_mode_enum),
	SOC_ENUM("SPO_N Class D Output Control", rt5633_spon_clsd_ctl_enum),
	SOC_ENUM("SPO_N Class AB Output Control", rt5633_spon_clsab_ctl_enum),
	SOC_ENUM("AUXOUT Control", rt5633_auxout_mode_enum),
	SOC_DOUBLE_TLV("Line1 Capture Volume", RT5633_LINE_IN_1_VOL,
		RT5633_L_VOL_SFT, RT5633_R_VOL_SFT, 31, 1, in_vol_tlv),
	SOC_DOUBLE_TLV("Line2 Capture Volume", RT5633_LINE_IN_2_VOL,
		RT5633_L_VOL_SFT, RT5633_R_VOL_SFT, 31, 1, in_vol_tlv),
	SOC_SINGLE_TLV("MIC1 Playback Volume", RT5633_MIC_CTRL_1,
		RT5633_L_VOL_SFT, 31, 1, in_vol_tlv),
	SOC_SINGLE_TLV("MIC2 Playback Volume", RT5633_MIC_CTRL_1,
		RT5633_R_VOL_SFT, 31, 1, in_vol_tlv),
	SOC_SINGLE("AXOL Playback Switch", RT5633_AUXOUT_VOL,
				RT5633_L_MUTE_SFT, 1, 1),
	SOC_SINGLE("AXOR Playback Switch", RT5633_AUXOUT_VOL,
				RT5633_R_MUTE_SFT, 1, 1),
	SOC_DOUBLE("AUX Playback Volume", RT5633_AUXOUT_VOL,
		RT5633_L_VOL_SFT, RT5633_R_VOL_SFT, 31, 1),
	SOC_SINGLE("SPK Playback Switch", RT5633_SPK_OUT_VOL,
				RT5633_L_MUTE_SFT, 1, 1),
	SOC_DOUBLE_TLV("SPK Playback Volume", RT5633_SPK_OUT_VOL,
		RT5633_SPKL_VOL_SFT, RT5633_SPKR_VOL_SFT, 31, 1, out_vol_tlv),
	SOC_SINGLE("HPL Playback Switch", RT5633_HP_OUT_VOL,
				RT5633_L_MUTE_SFT, 1, 1),
	SOC_SINGLE("HPR Playback Switch", RT5633_HP_OUT_VOL,
				RT5633_R_MUTE_SFT, 1, 1),
	SOC_DOUBLE_TLV("HP Playback Volume", RT5633_HP_OUT_VOL,
		RT5633_L_VOL_SFT, RT5633_R_VOL_SFT, 31, 1, out_vol_tlv),
};

static const struct snd_kcontrol_new rt5633_recmixl_mixer_controls[] = {
	SOC_DAPM_SINGLE("HPMIXL Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_L_HPM_SFT, 1, 1),
	SOC_DAPM_SINGLE("AUXMIXL Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_L_AUXM_SFT, 1, 1),
	SOC_DAPM_SINGLE("SPKMIX Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_L_SPKM_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE1L Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_L_LINE1_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE2L Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_L_LINE2_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC1 Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_L_MIC1_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC2 Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_L_MIC2_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5633_recmixr_mixer_controls[] = {
	SOC_DAPM_SINGLE("HPMIXR Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_R_HPM_SFT, 1, 1),
	SOC_DAPM_SINGLE("AUXMIXR Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_R_AUXM_SFT, 1, 1),
	SOC_DAPM_SINGLE("SPKMIX Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_R_SPKM_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE1R Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_R_LINE1_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE2R Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_R_LINE2_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC1 Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_R_MIC1_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC2 Capture Switch", RT5633_REC_MIXER_CTRL,
				RT5633_M_RM_R_MIC2_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5633_hp_mixl_mixer_controls[] = {
	SOC_DAPM_SINGLE("RECMIXL Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_L_RM_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_L_MIC1_SFT, 1, 1),	
	SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_L_MIC2_SFT, 1, 1),	
	SOC_DAPM_SINGLE("LINE1 Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_L_LINE1_SFT, 1, 1),	
	SOC_DAPM_SINGLE("LINE2 Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_L_LINE2_SFT, 1, 1),	
	SOC_DAPM_SINGLE("DAC Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_L_DAC_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5633_hp_mixr_mixer_controls[] = {
	SOC_DAPM_SINGLE("RECMIXR Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_R_RM_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_R_MIC1_SFT, 1, 1),	
	SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_R_MIC2_SFT, 1, 1),	
	SOC_DAPM_SINGLE("LINE1 Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_R_LINE1_SFT, 1, 1),	
	SOC_DAPM_SINGLE("LINE2 Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_R_LINE2_SFT, 1, 1),	
	SOC_DAPM_SINGLE("DAC Playback Switch", RT5633_HPMIXER_CTRL,
				RT5633_M_HPM_R_DAC_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5633_auxmixl_mixer_controls[] = {
	SOC_DAPM_SINGLE("RECMIXL Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_L_RM_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_L_MIC1_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_L_MIC2_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE1 Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_L_LINE1_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE2 Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_L_LINE2_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_L_DAC_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5633_auxmixr_mixer_controls[] = {
	SOC_DAPM_SINGLE("RECMIXR Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_R_RM_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_R_MIC1_SFT, 1, 1),
	SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_R_MIC2_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE1 Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_R_LINE1_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE2 Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_R_LINE2_SFT, 1, 1),
	SOC_DAPM_SINGLE("DAC Playback Switch", RT5633_AUXMIXER_CTRL,
				RT5633_M_AM_R_DAC_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5633_spkmixr_mixer_controls[]  = {
	SOC_DAPM_SINGLE("MIC1 Playback Switch", RT5633_SPKMIXER_CTRL,
				RT5633_M_SM_R_MIC1_SFT, 1, 1),	
	SOC_DAPM_SINGLE("MIC2 Playback Switch", RT5633_SPKMIXER_CTRL,
				RT5633_M_SM_R_MIC2_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE1L Playback Switch", RT5633_SPKMIXER_CTRL,
				RT5633_M_SM_R_L1_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE1R Playback Switch", RT5633_SPKMIXER_CTRL,
				RT5633_M_SM_R_L1_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE2L Playback Switch", RT5633_SPKMIXER_CTRL,
				RT5633_M_SM_R_L2_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("LINE2R Playback Switch", RT5633_SPKMIXER_CTRL,
				RT5633_M_SM_R_L2_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("DACL Playback Switch", RT5633_SPKMIXER_CTRL,
				RT5633_M_SM_R_DACL_SFT, 1, 1),
	SOC_DAPM_SINGLE("DACR Playback Switch", RT5633_SPKMIXER_CTRL,
				RT5633_M_SM_R_DACR_SFT, 1, 1),
};

/* DAC function select MUX */
static const char *rt5633_spo_src_sel[] = {"VMID", "HPMIX", "SPKMIX", "AUXMIX"};

static const struct soc_enum rt5633_spo_src_enum =
	SOC_ENUM_SINGLE(RT5633_SPKMIXER_CTRL, RT5633_SPK_VOL_S_SFT,
		ARRAY_SIZE(rt5633_spo_src_sel), rt5633_spo_src_sel);

static const struct snd_kcontrol_new rt5633_spo_src_mux =
	SOC_DAPM_ENUM("SPO SRC Mux", rt5633_spo_src_enum);

static int rt5633_check_sclk(struct snd_soc_dapm_widget *source,
			 struct snd_soc_dapm_widget *sink)
{
	unsigned int val;

	val = snd_soc_read(source->codec, RT5633_GBL_CLK_CTRL);
	val &= RT5633_SCLK_SRC_MASK;
	return (val == RT5633_SCLK_SRC_PLL);
}

static int rt5633_check_classd(struct snd_soc_dapm_widget *source,
			 struct snd_soc_dapm_widget *sink)
{
	unsigned int val;

	val = snd_soc_read(source->codec, RT5633_SPK_AMP_CTRL);
	val &= RT5633_SPK_AMP_MODE_MASK;

	return (val == RT5633_SPK_AMP_MODE_D);
}

static int rt5633_check_classab(struct snd_soc_dapm_widget *source,
			 struct snd_soc_dapm_widget *sink)
{
	unsigned int val;

	val = snd_soc_read(source->codec, RT5633_SPK_AMP_CTRL);
	val &= RT5633_SPK_AMP_MODE_MASK;

	return (val == RT5633_SPK_AMP_MODE_AB);
}

/* HP power on depop */
static void hp_depop_mode2(struct snd_soc_codec *codec)
{
	snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD3,
		RT5633_P_MAIN_BIAS | RT5633_P_VREF,
		RT5633_P_MAIN_BIAS | RT5633_P_VREF);
	snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD4,
		RT5633_P_HP_L_VOL | RT5633_P_HP_R_VOL,
		RT5633_P_HP_L_VOL | RT5633_P_HP_R_VOL);
	snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD3,
		RT5633_P_HP_AMP, RT5633_P_HP_AMP);
	snd_soc_update_bits(codec, RT5633_DEPOP_CTRL_1,
		RT5633_PW_SOFT_GEN | RT5633_EN_DEPOP_2,
		RT5633_PW_SOFT_GEN | RT5633_EN_DEPOP_2);
	msleep(300);
	snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD3,
		RT5633_P_HP_DIS_DEPOP, RT5633_P_HP_DIS_DEPOP);
	snd_soc_update_bits(codec, RT5633_DEPOP_CTRL_1, RT5633_EN_DEPOP_2, 0);
}

/* HP mute/unmute depop */
static void hp_mute_unmute_depop(struct snd_soc_codec *codec, int Mute)
{
	if (Mute) {
		snd_soc_update_bits(codec, RT5633_DEPOP_CTRL_1,
			RT5633_PW_SOFT_GEN | RT5633_EN_SOFT_FOR_S_M_DEPOP |
			RT5633_EN_HP_R_M_UM_DEPOP | RT5633_EN_HP_L_M_UM_DEPOP,
			RT5633_PW_SOFT_GEN | RT5633_EN_SOFT_FOR_S_M_DEPOP |
			RT5633_EN_HP_R_M_UM_DEPOP | RT5633_EN_HP_L_M_UM_DEPOP);
		snd_soc_update_bits(codec, RT5633_HP_OUT_VOL,
			RT5633_L_MUTE | RT5633_R_MUTE,
			RT5633_L_MUTE | RT5633_R_MUTE);
		mdelay(80);
		snd_soc_update_bits(codec, RT5633_DEPOP_CTRL_1,
			RT5633_PW_SOFT_GEN | RT5633_EN_SOFT_FOR_S_M_DEPOP |
			RT5633_EN_HP_R_M_UM_DEPOP | RT5633_EN_HP_L_M_UM_DEPOP,
			0);
	} else {
		snd_soc_update_bits(codec, RT5633_DEPOP_CTRL_1,
			RT5633_PW_SOFT_GEN | RT5633_EN_SOFT_FOR_S_M_DEPOP |
			RT5633_EN_HP_R_M_UM_DEPOP | RT5633_EN_HP_L_M_UM_DEPOP,
			RT5633_PW_SOFT_GEN | RT5633_EN_SOFT_FOR_S_M_DEPOP |
			RT5633_EN_HP_R_M_UM_DEPOP | RT5633_EN_HP_L_M_UM_DEPOP);
		snd_soc_update_bits(codec, RT5633_HP_OUT_VOL,
			RT5633_L_MUTE | RT5633_R_MUTE, 0);
		mdelay(80);
		snd_soc_update_bits(codec, RT5633_DEPOP_CTRL_1,
			RT5633_PW_SOFT_GEN | RT5633_EN_SOFT_FOR_S_M_DEPOP |
			RT5633_EN_HP_R_M_UM_DEPOP | RT5633_EN_HP_L_M_UM_DEPOP,
			0);
	}
}

static int open_hp_end_widgets(struct snd_soc_codec *codec)
{
	hp_mute_unmute_depop(codec, 0);
	return 0;
}

static int close_hp_end_widgets(struct snd_soc_codec *codec)
{
	hp_mute_unmute_depop(codec, 1);
	snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD3,
		RT5633_P_HP_AMP | RT5633_P_HP_BUF |
		RT5633_P_HP_DIS_DEPOP | RT5633_P_HP_AMP_DRI, 0);
	return 0;
}

static int rt5633_hp_event(struct snd_soc_dapm_widget *w, 
	struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;

	switch (event) {
	case SND_SOC_DAPM_PRE_PMD:
		close_hp_end_widgets(codec);
		break;

	case SND_SOC_DAPM_POST_PMU:
		hp_depop_mode2(codec);
		open_hp_end_widgets(codec);
		break;

	default:
		return -EINVAL;
	}

	return 0;
}

static const struct snd_soc_dapm_widget rt5633_dapm_widgets[] = {
	SND_SOC_DAPM_PGA("PLL", RT5633_PWR_MANAG_ADD2,
			RT5633_P_PLL_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("I2S", RT5633_PWR_MANAG_ADD1,
			RT5633_P_MAIN_I2S_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("DAC Ref", RT5633_PWR_MANAG_ADD1,
			RT5633_P_DAC_REF_BIT, 0, NULL, 0),

	SND_SOC_DAPM_INPUT("MIC1"),
	SND_SOC_DAPM_INPUT("MIC2"),
	SND_SOC_DAPM_INPUT("LINE1L"),
	SND_SOC_DAPM_INPUT("LINE2L"),
	SND_SOC_DAPM_INPUT("LINE1R"),
	SND_SOC_DAPM_INPUT("LINE2R"),

	SND_SOC_DAPM_MICBIAS("Mic Bias1", RT5633_PWR_MANAG_ADD2,
				RT5633_P_MICBIAS1_BIT, 0),
	SND_SOC_DAPM_MICBIAS("Mic Bias2", RT5633_PWR_MANAG_ADD2,
				RT5633_P_MICBIAS2_BIT, 0),

	SND_SOC_DAPM_PGA("Mic1 Boost", RT5633_PWR_MANAG_ADD2,
			RT5633_P_MIC1_BST_BIT, 0, NULL, 0),  
	SND_SOC_DAPM_PGA("Mic2 Boost", RT5633_PWR_MANAG_ADD2,
			RT5633_P_MIC2_BST_BIT, 0, NULL, 0), 

	SND_SOC_DAPM_PGA("LINE1L Inp Vol", RT5633_PWR_MANAG_ADD2,
				RT5633_P_L1_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("LINE1R Inp Vol", RT5633_PWR_MANAG_ADD2,
				RT5633_P_L1_R_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("LINE2L Inp Vol", RT5633_PWR_MANAG_ADD2,
				RT5633_P_L2_L_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("LINE2R Inp Vol", RT5633_PWR_MANAG_ADD2,
				RT5633_P_L2_R_BIT, 0, NULL, 0),

	SND_SOC_DAPM_MIXER("RECMIXL Mixer", RT5633_PWR_MANAG_ADD2,
		RT5633_P_RM_L_BIT, 0, rt5633_recmixl_mixer_controls,
		ARRAY_SIZE(rt5633_recmixl_mixer_controls)),
	SND_SOC_DAPM_MIXER("RECMIXR Mixer", RT5633_PWR_MANAG_ADD2,
		RT5633_P_RM_R_BIT, 0, rt5633_recmixr_mixer_controls,
		ARRAY_SIZE(rt5633_recmixr_mixer_controls)),

	SND_SOC_DAPM_ADC("Left ADC", "HIFI Capture", RT5633_PWR_MANAG_ADD1,
					RT5633_P_ADC_L_CLK_BIT, 0),
	SND_SOC_DAPM_ADC("Right ADC", "HIFI Capture", RT5633_PWR_MANAG_ADD1,
					RT5633_P_ADC_R_CLK_BIT, 0),
	SND_SOC_DAPM_DAC("Left DAC", "HIFI Playback", RT5633_PWR_MANAG_ADD1,
					RT5633_P_DAC_L_CLK_BIT, 0),
	SND_SOC_DAPM_DAC("Right DAC", "HIFI Playback", RT5633_PWR_MANAG_ADD1,
					RT5633_P_DAC_R_CLK_BIT, 0),

	SND_SOC_DAPM_PGA("Left DAC DF2SE", RT5633_PWR_MANAG_ADD1,
			RT5633_P_DAC_L_TO_MIX_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Right DAC DF2SE", RT5633_PWR_MANAG_ADD1,
			RT5633_P_DAC_R_TO_MIX_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Left DAC Path", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Right DAC Path", SND_SOC_NOPM, 0, 0, NULL, 0),

	SND_SOC_DAPM_MIXER("HPMIXL Mixer", RT5633_PWR_MANAG_ADD2,
		RT5633_P_HM_L_BIT, 0, rt5633_hp_mixl_mixer_controls,
		ARRAY_SIZE(rt5633_hp_mixl_mixer_controls)),
	SND_SOC_DAPM_MIXER("HPMIXR Mixer", RT5633_PWR_MANAG_ADD2,
		RT5633_P_HM_R_BIT, 0, rt5633_hp_mixr_mixer_controls,
		ARRAY_SIZE(rt5633_hp_mixr_mixer_controls)),
	SND_SOC_DAPM_MIXER("AUXMIXL Mixer", RT5633_PWR_MANAG_ADD2,
		RT5633_P_AM_L_BIT, 0, rt5633_auxmixl_mixer_controls,
		ARRAY_SIZE(rt5633_auxmixl_mixer_controls)),
	SND_SOC_DAPM_MIXER("AUXMIXR Mixer", RT5633_PWR_MANAG_ADD2,
		RT5633_P_AM_R_BIT, 0, rt5633_auxmixr_mixer_controls,
		ARRAY_SIZE(rt5633_auxmixr_mixer_controls)),
	SND_SOC_DAPM_MIXER("SPXMIX Mixer", RT5633_PWR_MANAG_ADD2,
		RT5633_P_SM_BIT, 0, rt5633_spkmixr_mixer_controls,
		ARRAY_SIZE(rt5633_spkmixr_mixer_controls)),

	SND_SOC_DAPM_PGA("Left SPK Vol", RT5633_PWR_MANAG_ADD4,
			RT5633_P_SPK_L_VOL_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Right SPK Vol", RT5633_PWR_MANAG_ADD4,
			RT5633_P_SPK_R_VOL_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA_E("Left HP Vol", RT5633_PWR_MANAG_ADD4,
			RT5633_P_HP_L_VOL_BIT, 0, NULL, 0, rt5633_hp_event,
			SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
	SND_SOC_DAPM_PGA_E("Right HP Vol", RT5633_PWR_MANAG_ADD4,
			RT5633_P_HP_R_VOL_BIT, 0, NULL, 0, rt5633_hp_event,
			SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
	SND_SOC_DAPM_PGA("Left AUX Out Vol", RT5633_PWR_MANAG_ADD4,
			RT5633_P_AUXOUT_L_VOL_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Right AUX Out Vol", RT5633_PWR_MANAG_ADD4,
			RT5633_P_AUXOUT_R_VOL_BIT, 0, NULL, 0),

	SND_SOC_DAPM_MUX("SPKO Mux", SND_SOC_NOPM, 0, 0, &rt5633_spo_src_mux),

	SND_SOC_DAPM_PGA("Class D Amp", RT5633_PWR_MANAG_ADD1,
				RT5633_P_CLS_D_BIT, 0, NULL, 0),
	SND_SOC_DAPM_PGA("Class AB Amp", RT5633_PWR_MANAG_ADD1,
				RT5633_P_CLS_AB_BIT, 0, NULL, 0),

	SND_SOC_DAPM_OUTPUT("AUXOUTL"),
	SND_SOC_DAPM_OUTPUT("AUXOUTR"),
	SND_SOC_DAPM_OUTPUT("SPOL"),
	SND_SOC_DAPM_OUTPUT("SPOR"),
	SND_SOC_DAPM_OUTPUT("HPOL"),
	SND_SOC_DAPM_OUTPUT("HPOR"),
};

static const struct snd_soc_dapm_route rt5633_dapm_routes[] = {
	{"PLL", NULL, "DAC Ref"},
	{"I2S", NULL, "PLL"},

	{"Mic1 Boost", NULL, "MIC1"},
	{"Mic2 Boost", NULL, "MIC2"},
	
	{"LINE1L Inp Vol", NULL, "LINE1L"},
	{"LINE1R Inp Vol", NULL, "LINE1R"},

	{"LINE2L Inp Vol", NULL, "LINE2L"},
	{"LINE2R Inp Vol", NULL, "LINE2R"},

	{"RECMIXL Mixer", "HPMIXL Capture Switch", "HPMIXL Mixer"},
	{"RECMIXL Mixer", "AUXMIXL Capture Switch", "AUXMIXL Mixer"},
	{"RECMIXL Mixer", "SPKMIX Capture Switch", "SPXMIX Mixer"},
	{"RECMIXL Mixer", "LINE1L Capture Switch", "LINE1L Inp Vol"},
	{"RECMIXL Mixer", "LINE2L Capture Switch", "LINE2L Inp Vol"},
	{"RECMIXL Mixer", "MIC1 Capture Switch", "Mic1 Boost"},
	{"RECMIXL Mixer", "MIC2 Capture Switch", "Mic2 Boost"},

	{"RECMIXR Mixer", "HPMIXR Capture Switch", "HPMIXR Mixer"},
	{"RECMIXR Mixer", "AUXMIXR Capture Switch", "AUXMIXR Mixer"},
	{"RECMIXR Mixer", "SPKMIX Capture Switch", "SPXMIX Mixer"},
	{"RECMIXR Mixer", "LINE1R Capture Switch", "LINE1R Inp Vol"},
	{"RECMIXR Mixer", "LINE2R Capture Switch", "LINE2R Inp Vol"},
	{"RECMIXR Mixer", "MIC1 Capture Switch", "Mic1 Boost"},
	{"RECMIXR Mixer", "MIC2 Capture Switch", "Mic2 Boost"},

	{"DAC Ref", NULL, "RECMIXL Mixer"},
	{"DAC Ref", NULL, "RECMIXR Mixer"},
	{"Left ADC", NULL, "I2S"},
	{"Right ADC", NULL, "I2S"},

	{"Left DAC DF2SE", NULL, "Left DAC"},
	{"DAC Ref", NULL, "Left DAC DF2SE"},
	{"Left DAC Path", NULL, "I2S"},
	{"Right DAC DF2SE", NULL, "Right DAC"},
	{"DAC Ref", NULL, "Right DAC DF2SE"},
	{"Right DAC Path", NULL, "I2S"},

	{"HPMIXL Mixer", "RECMIXL Playback Switch", "RECMIXL Mixer"},
	{"HPMIXL Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"HPMIXL Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"HPMIXL Mixer", "LINE1 Playback Switch", "LINE1L Inp Vol"},
	{"HPMIXL Mixer", "LINE2 Playback Switch", "LINE2L Inp Vol"},
	{"HPMIXL Mixer", "DAC Playback Switch", "Left DAC Path"},

	{"HPMIXR Mixer", "RECMIXR Playback Switch", "RECMIXR Mixer"},
	{"HPMIXR Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"HPMIXR Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"HPMIXR Mixer", "LINE1 Playback Switch", "LINE1R Inp Vol"},
	{"HPMIXR Mixer", "LINE2 Playback Switch", "LINE2R Inp Vol"},
	{"HPMIXR Mixer", "DAC Playback Switch", "Right DAC Path"},

	{"AUXMIXL Mixer", "RECMIXL Playback Switch", "RECMIXL Mixer"},
	{"AUXMIXL Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"AUXMIXL Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"AUXMIXL Mixer", "LINE1 Playback Switch", "LINE1L Inp Vol"},
	{"AUXMIXL Mixer", "LINE2 Playback Switch", "LINE2L Inp Vol"},
	{"AUXMIXL Mixer", "DAC Playback Switch", "Left DAC Path"},

	{"AUXMIXR Mixer", "RECMIXR Playback Switch", "RECMIXR Mixer"},
	{"AUXMIXR Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"AUXMIXR Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"AUXMIXR Mixer", "LINE1 Playback Switch", "LINE1R Inp Vol"},
	{"AUXMIXR Mixer", "LINE2 Playback Switch", "LINE2R Inp Vol"},
	{"AUXMIXR Mixer", "DAC Playback Switch", "Right DAC Path"},

	{"SPXMIX Mixer", "MIC1 Playback Switch", "Mic1 Boost"},
	{"SPXMIX Mixer", "MIC2 Playback Switch", "Mic2 Boost"},
	{"SPXMIX Mixer", "DACL Playback Switch", "Left DAC Path"},
	{"SPXMIX Mixer", "DACR Playback Switch", "Right DAC Path"},
	{"SPXMIX Mixer", "LINE1L Playback Switch", "LINE1L Inp Vol"},
	{"SPXMIX Mixer", "LINE1R Playback Switch", "LINE1R Inp Vol"},
	{"SPXMIX Mixer", "LINE2L Playback Switch", "LINE2L Inp Vol"},
	{"SPXMIX Mixer", "LINE2R Playback Switch", "LINE2R Inp Vol"},

	{"SPKO Mux", "HPMIX", "HPMIXL Mixer"},
	{"SPKO Mux", "SPKMIX", "SPXMIX Mixer"},
	{"SPKO Mux", "AUXMIX", "AUXMIXL Mixer"},

	{"Left SPK Vol",  NULL, "SPKO Mux"},
	{"Right SPK Vol",  NULL, "SPKO Mux"},

	{"Right HP Vol",  NULL, "HPMIXR Mixer"},
	{"Left HP Vol",  NULL, "HPMIXL Mixer"},

	{"Left AUX Out Vol",  NULL, "AUXMIXL Mixer"},
	{"Right AUX Out Vol",  NULL, "AUXMIXR Mixer"},

	{"AUXOUTL", NULL, "Left AUX Out Vol"},
	{"AUXOUTR", NULL, "Right AUX Out Vol"},
	//{"Class AB Amp", NULL, "Left SPK Vol"},
	//{"SPOL", NULL, "Class AB Amp"},
	{"Class D Amp", NULL, "Left SPK Vol"},
	{"SPOL", NULL, "Class D Amp"},
	//{"Class AB Amp", NULL, "Right SPK Vol"},
	//{"SPOR", NULL, "Class AB Amp"},
	{"Class D Amp", NULL, "Right SPK Vol"},
	{"SPOR", NULL, "Class D Amp"},
	{"HPOL", NULL, "Left HP Vol"},
	{"HPOR", NULL, "Right HP Vol"},
};

struct _coeff_div{
	unsigned int mclk;
	unsigned int bclk;
	unsigned int rate;
	unsigned int reg_val;
};

/*PLL divisors*/
struct _pll_div {
	u32 pll_in;
	u32 pll_out;
	u16 regvalue;
};

static const struct _pll_div codec_master_pll_div[] = {
	{  2048000,  8192000,  0x0ea0},
	{  3686400,  8192000,  0x4e27},
	{ 12000000,  8192000,  0x456b},   
	{ 13000000,  8192000,  0x495f},
	{ 13100000,  8192000,  0x0320},
	{  2048000,  11289600,  0xf637},
	{  3686400,  11289600,  0x2f22},
	{ 12000000,  11289600,  0x3e2f},   
	{ 13000000,  11289600,  0x4d5b},
	{ 13100000,  11289600,  0x363b},
	{  2048000,  16384000,  0x1ea0},
	{  3686400,  16384000,  0x9e27},
	{ 12000000,  16384000,  0x452b},   
	{ 13000000,  16384000,  0x542f},
	{ 13100000,  16384000,  0x03a0},
	{  2048000,  16934400,  0xe625},
	{  3686400,  16934400,  0x9126},
	{ 12000000,  16934400,  0x4d2c},
	{ 13000000,  16934400,  0x742f},
	{ 13100000,  16934400,  0x3c27},
	{  2048000,  22579200,  0x2aa0},
	{  3686400,  22579200,  0x2f20},
	{ 12000000,  22579200,  0x7e2f},   
	{ 13000000,  22579200,  0x742f},
	{ 13100000,  22579200,  0x3c27},
	{  2048000,  24576000,  0x2ea0},
	{  3686400,  24576000,  0xee27},
	{ 12000000,  24576000,  0x2915},   
	{ 13000000,  24576000,  0x772e},
	{ 13100000,  24576000,  0x0d20},
	{ 26000000,  24576000,  0x2027},
	{ 26000000,  22579200,  0x392f},
	{ 24576000,  22579200,  0x0921},
	{ 24576000,  24576000,  0x02a0},
};

static const struct _pll_div codec_slave_pll_div[] = {
	{ 256000,  2048000,  0x46f0},
	{ 256000,  4096000,  0x3ea0},
	{ 352800,  5644800,  0x3ea0},
	{ 512000,  8192000,  0x3ea0},
	{ 1024000,  8192000,  0x46f0},
	{ 705600,  11289600,  0x3ea0},
	{ 1024000,  16384000,  0x3ea0},
	{ 1411200,  22579200,  0x3ea0},
	{ 1536000,  24576000,  0x3ea0},
	{ 2048000,  16384000,  0x1ea0},
	{ 2822400,  22579200,  0x1ea0},
	{ 2822400,  45158400,  0x5ec0},
	{ 5644800,  45158400,  0x46f0},
	{ 3072000,  24576000,  0x1ea0},
	{ 3072000,  49152000,  0x5ec0},
	{ 6144000,  49152000,  0x46f0},
	{ 705600,  11289600,  0x3ea0},
	{ 705600,  8467200,  0x3ab0},
	{ 24576000,  24576000,  0x02a0},
	{ 1411200,  11289600,  0x1690},
	{ 2822400,  11289600,  0x0a90},
	{ 1536000,  12288000,  0x1690},
	{ 3072000,  12288000,  0x0a90},
};

struct _coeff_div coeff_div[] = {
	//sysclk is 256fs
	{ 2048000,  8000 * 32,  8000, 0x1000},
	{ 2048000,  8000 * 64,  8000, 0x0000},
	{ 2822400, 11025 * 32, 11025, 0x1000},
	{ 2822400, 11025 * 64, 11025, 0x0000},
	{ 4096000, 16000 * 32, 16000, 0x1000},
	{ 4096000, 16000 * 64, 16000, 0x0000},
	{ 5644800, 22050 * 32, 22050, 0x1000},
	{ 5644800, 22050 * 64, 22050, 0x0000},
	{ 8192000, 32000 * 32, 32000, 0x1000},
	{ 8192000, 32000 * 64, 32000, 0x0000},
	{11289600, 44100 * 32, 44100, 0x1000},
	{11289600, 44100 * 64, 44100, 0x0000},
	{12288000, 48000 * 32, 48000, 0x1000},
	{12288000, 48000 * 64, 48000, 0x0000},
	//sysclk is 512fs
	{ 4096000,  8000 * 32,  8000, 0x3000},
	{ 4096000,  8000 * 64,  8000, 0x2000},
	{ 5644800, 11025 * 32, 11025, 0x3000},
	{ 5644800, 11025 * 64, 11025, 0x2000},
	{ 8192000, 16000 * 32, 16000, 0x3000},
	{ 8192000, 16000 * 64, 16000, 0x2000},
	{11289600, 22050 * 32, 22050, 0x3000},
	{11289600, 22050 * 64, 22050, 0x2000},
	{16384000, 32000 * 32, 32000, 0x3000},
	{16384000, 32000 * 64, 32000, 0x2000},
	{22579200, 44100 * 32, 44100, 0x3000},
	{22579200, 44100 * 64, 44100, 0x2000},
	{24576000, 48000 * 32, 48000, 0x3000},
	{24576000, 48000 * 64, 48000, 0x2000},
	//SYSCLK is 22.5792Mhz or 24.576Mhz(8k to 48k)	
	{24576000, 48000 * 32, 48000, 0x3000},
	{24576000, 48000 * 64, 48000, 0x2000},
	{22579200, 44100 * 32, 44100, 0x3000},
	{22579200, 44100 * 64, 44100, 0x2000},
	{24576000, 32000 * 32, 32000, 0x1080},
	{24576000, 32000 * 64, 32000, 0x0080},
	{22579200, 22050 * 32, 22050, 0x5000},
	{22579200, 22050 * 64, 22050, 0x4000},	
	{24576000, 16000 * 32, 16000, 0x3080},
	{24576000, 16000 * 64, 16000, 0x2080},	
	{22579200, 11025 * 32, 11025, 0x7000},
	{22579200, 11025 * 64, 11025, 0x6000},	
	{24576000, 8000 * 32, 8000, 0x7080},
	{24576000, 8000 * 64, 8000, 0x6080},	
};

static int get_coeff(int mclk, int rate, int timesofbclk)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(coeff_div); i++)
		if ((coeff_div[i].mclk == mclk) 
			&& (coeff_div[i].rate == rate)
			&& ((coeff_div[i].bclk / coeff_div[i].rate) == timesofbclk))
			return i;
	return -1;
}

static int get_coeff_in_slave_mode(int mclk, int rate)
{
	return get_coeff(mclk, rate, BLCK_FREQ);
}

static int get_coeff_in_master_mode(int mclk, int rate)
{
	return get_coeff(mclk, rate ,BLCK_FREQ);	
}

static int rt5633_hifi_pcm_params(struct snd_pcm_substream *substream, 
	struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	struct rt5633_priv *rt5633 = codec->private_data;
	int rate = params_rate(params), coeff = 0;
	unsigned int iface = 0;

	dev_dbg(codec->dev, "enter %s\n", __func__);	
	if (rt5633->master)
		coeff = get_coeff_in_master_mode(rt5633->sysclk, rate);
	else
		coeff = get_coeff_in_slave_mode(rt5633->sysclk, rate);
	if (coeff < 0) {
		dev_err(codec->dev, "get_coeff err!\n");
		return -EINVAL;
	}

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S16_BE:
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		iface |= RT5633_SDP_I2S_DL_20;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		iface |= RT5633_SDP_I2S_DL_24;
		break;
	case SNDRV_PCM_FORMAT_S8:
		iface |= RT5633_SDP_I2S_DL_8;
		break;
	default:
		return -EINVAL;
	}
	
	snd_soc_update_bits(codec, RT5633_SDP_CTRL,
		RT5633_SDP_I2S_DL_MASK, iface);
	snd_soc_write(codec, RT5633_STEREO_AD_DA_CLK_CTRL,
			coeff_div[coeff].reg_val);

	return 0;
}

static int rt5633_hifi_codec_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5633_priv *rt5633 = codec->private_data;
	unsigned int iface = 0;
	
	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		rt5633->master = 1;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		iface |= RT5633_SDP_MODE_SEL_SLAVE;
		rt5633->master = 0;
		break;
	default:
		return -EINVAL;
	}
	
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface |= RT5633_SDP_I2S_DF_RIGHT;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		iface |= RT5633_SDP_I2S_DF_LEFT;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		iface  |= RT5633_SDP_I2S_DF_PCM;
		break;
	default:
		return -EINVAL;
	}
	
	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface |= RT5633_SDP_I2S_BCLK_POL_INV;
		break;
	default:
		return -EINVAL;	
	}
	
	snd_soc_update_bits(codec, RT5633_SDP_CTRL,
		RT5633_SDP_MODE_SEL_MASK | RT5633_SDP_I2S_DF_MASK |
		RT5633_SDP_I2S_BCLK_POL_INV, iface);

	return 0;
}
static int rt5633_hifi_codec_set_dai_sysclk(
	struct snd_soc_dai *dai, int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5633_priv *rt5633 = codec->private_data;

	if (freq == rt5633->sysclk)
		return 0;

	rt5633->sysclk = freq;
	dev_dbg(dai->dev, "Sysclk is %dHz\n", freq);

	return 0;
}

static int rt5633_codec_set_dai_pll(struct snd_soc_dai *dai, int pll_id,
		unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5633_priv *rt5633 = codec->private_data;
	struct _pll_div *pll_tab;
	int i, ret = -EINVAL, tab_num;
	unsigned int val, mask;
	
	
	dev_dbg(codec->dev, "enter %s\n", __func__);	
	if (!freq_in || !freq_out) {
		dev_dbg(codec->dev, "PLL disabled\n");
		snd_soc_update_bits(codec, RT5633_GBL_CLK_CTRL,
			RT5633_SCLK_SRC_MASK, RT5633_SCLK_SRC_MCLK);
		return 0;
	}

	if (rt5633->master) {
		tab_num = ARRAY_SIZE(codec_master_pll_div);
		pll_tab = codec_master_pll_div;
		val = RT5633_SCLK_SRC_PLL | RT5633_PLL_SRC_MCLK;
		mask = RT5633_SCLK_SRC_MASK | RT5633_PLL_SRC_MASK;
	} else {
		tab_num = ARRAY_SIZE(codec_slave_pll_div);
		pll_tab = codec_slave_pll_div;
		val = RT5633_SCLK_SRC_PLL | RT5633_PLL_SRC_BCLK;
		mask = RT5633_SCLK_SRC_MASK | RT5633_PLL_SRC_MASK;
	}

	for (i = 0; i < tab_num; i ++)
		if (freq_in == pll_tab[i].pll_in &&
			freq_out == pll_tab[i].pll_out) {
			snd_soc_write(codec, RT5633_PLL_CTRL,
					pll_tab[i].regvalue);
			msleep(20);
			snd_soc_update_bits(codec,
				RT5633_GBL_CLK_CTRL, mask, val);
			ret = 0;
			break;
		}

	return ret;
}

static void rt5633_sync_cache(struct snd_soc_codec *codec)
{
	const u16 *reg_cache = codec->reg_cache;
	int i;

	/* Sync back cached values if they're different from the
	 * hardware default.
	 */
	for (i = 1; i < codec->reg_cache_size; i++) {
		if (reg_cache[i] == rt5633_reg[i])
			continue;
		snd_soc_write(codec, i, reg_cache[i]);
	}
}

static int rt5633_set_bias_level(struct snd_soc_codec *codec,
			enum snd_soc_bias_level level)
{
	switch (level) {
	case SND_SOC_BIAS_ON:
		snd_soc_update_bits(codec, RT5633_SPK_OUT_VOL,
			RT5633_L_MUTE, 0);
		snd_soc_update_bits(codec, RT5633_HP_OUT_VOL,
			RT5633_L_MUTE | RT5633_R_MUTE, 0);
		break;

	case SND_SOC_BIAS_PREPARE:
		snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD2,
			RT5633_P_MICBIAS1, RT5633_P_MICBIAS1);
		break;

	case SND_SOC_BIAS_STANDBY:
		snd_soc_update_bits(codec, RT5633_SPK_OUT_VOL,
			RT5633_L_MUTE, RT5633_L_MUTE);
		snd_soc_update_bits(codec, RT5633_HP_OUT_VOL,
			RT5633_L_MUTE | RT5633_R_MUTE,
			RT5633_L_MUTE | RT5633_R_MUTE);
		snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD2,
					RT5633_P_MICBIAS1, 0);
		if (SND_SOC_BIAS_OFF == codec->bias_level) {
			snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD3,
				RT5633_P_VREF | RT5633_P_MAIN_BIAS,
				RT5633_P_VREF | RT5633_P_MAIN_BIAS);
			msleep(110);
			snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD3,
					RT5633_P_DIS_FAST_VREF,
					RT5633_P_DIS_FAST_VREF);
			//codec->cache_only = false;
			rt5633_sync_cache(codec);
			rt5633_index_sync(codec);
		}
		break;

	case SND_SOC_BIAS_OFF:
		snd_soc_write(codec, RT5633_PWR_MANAG_ADD1, 0x0000);
		snd_soc_write(codec, RT5633_PWR_MANAG_ADD2, 0x0000);
		snd_soc_write(codec, RT5633_PWR_MANAG_ADD3, 0x0000);
		snd_soc_write(codec, RT5633_PWR_MANAG_ADD4, 0x0000);
		break;

	default:
		break;
	}
	codec->bias_level = level;

	return 0;
}


#define RT5633_STEREO_RATES (SNDRV_PCM_RATE_8000_48000)
#define RT5633_FORMATS (SNDRV_PCM_FMTBIT_S16_BE | SNDRV_PCM_FMTBIT_S16_LE | SNDRV_PCM_FMTBIT_S20_3LE |\
			SNDRV_PCM_FMTBIT_S24_LE | SNDRV_PCM_FMTBIT_S8)

struct snd_soc_dai_ops rt5633_ops = {
	.hw_params = rt5633_hifi_pcm_params,
	.set_fmt = rt5633_hifi_codec_set_dai_fmt,
	.set_sysclk = rt5633_hifi_codec_set_dai_sysclk,
	.set_pll = rt5633_codec_set_dai_pll,
};

struct snd_soc_dai rt5633_dai[] = {
	{
		.name = "rt5633-hifi",
		.id = RT5633_AIF1,
		.playback = {
			.stream_name = "HIFI Playback",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5633_STEREO_RATES,
			.formats = RT5633_FORMATS,	
		}	,
		.capture = {
			.stream_name = "HIFI Capture",
			.channels_min = 1,
			.channels_max = 2,
			.rates = RT5633_STEREO_RATES,	
			.formats = RT5633_FORMATS,	
		},
		.ops =&rt5633_ops,
	},
	{
		.name = "rt5633-voice",
		.id = RT5633_AIF2,
	}
};
EXPORT_SYMBOL_GPL(rt5633_dai);

static int rt5633_init(struct snd_soc_device *socdev)
{
	struct snd_soc_codec *codec = socdev->card->codec;
	int ret = 0;

	codec->name = "RT5633";
	codec->owner = THIS_MODULE;
	codec->read = rt5633_read;
	codec->write = rt5633_write;
	codec->set_bias_level = rt5633_set_bias_level;
	codec->dai = rt5633_dai;
	codec->num_dai = 1;
	codec->reg_cache_size = RT5633_VENDOR_ID2 + 1;
	codec->reg_cache_step = 1;
	codec->reg_cache = kmemdup(rt5633_reg, sizeof(rt5633_reg), GFP_KERNEL);
	if (codec->reg_cache == NULL)
		return -ENOMEM;

	ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
	if (ret < 0 ) {
		dev_err(codec->dev, "failed to create pcms\n");
		goto pcm_err;
	}

	rt5633_reset(codec);
	snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD3,
		RT5633_P_VREF | RT5633_P_MAIN_BIAS,
		RT5633_P_VREF | RT5633_P_MAIN_BIAS);
	msleep(110);
	snd_soc_update_bits(codec, RT5633_PWR_MANAG_ADD3,
			RT5633_P_DIS_FAST_VREF,
			RT5633_P_DIS_FAST_VREF);
	rt5633_reg_init(codec);
	codec->bias_level = SND_SOC_BIAS_STANDBY;

	ret = snd_soc_init_card(socdev);
	if (ret < 0) {
		dev_err(codec->dev, "failed to register card\n");
		goto card_err;
	}

	snd_soc_add_controls(codec, rt5633_snd_controls,
		ARRAY_SIZE(rt5633_snd_controls));
	snd_soc_dapm_new_controls(codec, rt5633_dapm_widgets, 
			ARRAY_SIZE(rt5633_dapm_widgets));
	snd_soc_dapm_add_routes(codec, rt5633_dapm_routes,
		ARRAY_SIZE(rt5633_dapm_routes));
	snd_soc_dapm_new_widgets(codec);

	dev_info(codec->dev, "RT5633 initial ok\n");

	return ret;

card_err:
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);

pcm_err:
	kfree(codec->reg_cache);
	codec->reg_cache = NULL;
	return ret;
}

static int rt5633_probe(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec;
	struct rt5633_priv *rt5633;
	int ret;

	pr_info("enter %s\n", __func__);	

	codec = kzalloc(sizeof(struct snd_soc_codec), GFP_KERNEL);
	if (codec == NULL)
		return -ENOMEM;

	rt5633 = kzalloc(sizeof(struct rt5633_priv), GFP_KERNEL);
	if (rt5633 == NULL) {
		ret = -ENOMEM;
		goto priv_err;	
	}

	codec->dev = &pdev->dev;
	codec->private_data = rt5633;
	socdev->card->codec = codec;
	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);
	rt5633_init(socdev);

	return ret;

priv_err:
	kfree(codec);
	return ret;
}

static int rt5633_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	if (codec->control_data)
		rt5633_set_bias_level(codec, SND_SOC_BIAS_OFF);

	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);
	kfree(codec->private_data);
	kfree(codec);

	return 0;
}


static int rt5633_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	rt5633_set_bias_level(codec, SND_SOC_BIAS_OFF);

	return 0;
}

static int rt5633_resume(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	rt5633_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	return 0;
}


struct snd_soc_codec_device soc_codec_dev_rt5633 = {
	.probe = 	rt5633_probe,
	.remove = rt5633_remove,
	.suspend = rt5633_suspend,
	.resume = rt5633_resume,
};
EXPORT_SYMBOL_GPL(soc_codec_dev_rt5633);

static int __init rt5633_modinit(void)
{
	return snd_soc_register_dais(rt5633_dai, ARRAY_SIZE(rt5633_dai));
}
module_init(rt5633_modinit);

static void __exit rt5633_modexit(void)
{
	snd_soc_unregister_dais(rt5633_dai, ARRAY_SIZE(rt5633_dai));
}
module_exit(rt5633_modexit);

MODULE_DESCRIPTION("ASoC RT5633 driver");
MODULE_AUTHOR("Johnny Hsu <johnnyhsu@realtek.com>");
MODULE_LICENSE("GPL");
