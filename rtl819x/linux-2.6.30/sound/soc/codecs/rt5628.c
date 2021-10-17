/*
 * rt5628.c  --  RT5628 ALSA SoC audio codec driver
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

#include "rt5628.h"

#define ALC5628_I2C_ADDR	0x30

struct snd_soc_codec *rt5628_codec;

static const u16 rt5628_reg[RT5628_VENDOR_ID2 + 1] = {
	[RT5628_RESET] = 0x0003,
	[RT5628_SPK_OUT_VOL] = 0x9f9f,
	[RT5628_HP_OUT_VOL] = 0x9f9f,
	[RT5628_LINE_IN_VOL] = 0xc8c8,
	[RT5628_DAC_DIG_VOL] = 0xffff,
	[RT5628_SDV_CTRL_TIME] = 0x0009,
	[RT5628_OUT_MIX_CTRL] = 0x8004,
	[RT5628_SDP_CTRL] = 0x8000,
	[RT5628_DAC_CLK_CTRL] = 0x2000,
	[RT5628_GEN_CTRL] = 0x0100,
	[RT5628_AVC_CTRL] = 0x100b,
};

struct rt5628_priv {
	int master;
	int sysclk;
};

extern unsigned int serial_in_i2c(unsigned int addr, int offset);
extern unsigned int serial_out_i2c(unsigned int addr, int offset, int value);

/**
 * rt5628_read - Read register.
 * @codec: SoC audio codec device.
 * @reg: The register index.
 *
 * Returns value of register for success or negative error code.
 */
static int rt5628_read(struct snd_soc_codec *codec, unsigned int reg)
{
	return serial_in_i2c(ALC5628_I2C_ADDR, reg);
}

/**
 * rt5628_cache_write: Set the value of a given register in the cache.
 *
 * @codec: SoC audio codec device.
 * @reg: The register index.
 * @value: The new register value.
 */
static int rt5628_cache_write(struct snd_soc_codec *codec,
			unsigned int reg, unsigned int value)
{
	u16 *cache = codec->reg_cache;
	if (reg > RT5628_VENDOR_ID2)
		return -EINVAL;
	cache[reg] = value;

	return 0;
}

/**
 * rt5628_write - Write register.
 * @codec: SoC audio codec device.
 * @reg: The register index.
 * @value: The new register value.
 *
 * Returns 0 for success or negative error code.
 */
static int rt5628_write(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	u8 data[3];
	int ret;

	data[0] = reg;
	data[1] = (value & 0xff00) >> 8;
	data[2] = value & 0x00ff;

	
	ret = rt5628_cache_write(codec, reg, value);
	if (ret < 0)
		return -EIO;
	serial_out_i2c(ALC5628_I2C_ADDR, reg, value);

	return 0;
}

static int rt5628_reset(struct snd_soc_codec *codec)
{
	return snd_soc_write(codec, RT5628_RESET, 0);
}

/**
 * rt5628_index_write - Write private register.
 * @codec: SoC audio codec device.
 * @reg: Private register index.
 * @value: Private register Data.
 *
 * Modify private register for advanced setting. It can be written through
 * private index (0x6a) and data (0x6c) register.
 *
 * Returns 0 for success or negative error code.
 */
static int rt5628_index_write(struct snd_soc_codec *codec,
		unsigned int reg, unsigned int value)
{
	int ret;

	ret = snd_soc_write(codec, RT5628_PRIV_INDEX, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private addr: %d\n", ret);
		goto err;
	}
	ret = snd_soc_write(codec, RT5628_PRIV_DATA, value);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private value: %d\n", ret);
		goto err;
	}
	return 0;

err:
	return ret;
}

/**
 * rt5628_index_read - Read private register.
 * @codec: SoC audio codec device.
 * @reg: Private register index.
 *
 * Read advanced setting from private register. It can be read through
 * private index (0x6a) and data (0x6c) register.
 *
 * Returns private register value or negative error code.
 */
static unsigned int rt5628_index_read(
	struct snd_soc_codec *codec, unsigned int reg)
{
	int ret;

	ret = snd_soc_write(codec, RT5628_PRIV_INDEX, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to set private addr: %d\n", ret);
		return ret;
	}
	return snd_soc_read(codec, RT5628_PRIV_DATA);
}

/**
 * rt5628_index_update_bits - update private register bits
 * @codec: audio codec
 * @reg: Private register index.
 * @mask: register mask
 * @value: new value
 *
 * Writes new register value.
 *
 * Returns 1 for change, 0 for no change, or negative error code.
 */
static int rt5628_index_update_bits(struct snd_soc_codec *codec,
	unsigned int reg, unsigned int mask, unsigned int value)
{
	unsigned int old, new;
	int change, ret;

	ret = rt5628_index_read(codec, reg);
	if (ret < 0) {
		dev_err(codec->dev, "Failed to read private reg: %d\n", ret);
		goto err;
	}

	old = ret;
	new = (old & ~mask) | (value & mask);
	change = old != new;
	if (change) {
		ret = rt5628_index_write(codec, reg, new);
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


static ssize_t rt5628_codec_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	unsigned int val;
	int cnt = 0, i;

	cnt += sprintf(buf, "RT5628 codec register\n");
	for (i = 0; i <= RT5628_VENDOR_ID2; i++) {
		if (cnt + 23 >= PAGE_SIZE)
			break;
		val = snd_soc_read(rt5628_codec, i);
		if (!val)
			continue;
		cnt += snprintf(buf + cnt, 23,
				"#rng%02x  #rv%04x  #rd0\n", i, val);
	}

	if (cnt >= PAGE_SIZE)
		cnt = PAGE_SIZE - 1;

	return cnt;
}

static ssize_t rt5628_codec_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	unsigned int val = 0, addr = 0;
	int i;

	//pr_debug("register \"%s\" count=%d\n", buf, count);
	for (i = 0; i < count; i++) {	/*address */
		if (*(buf + i) <= '9' && *(buf + i) >= '0')
			addr = (addr << 4) | (*(buf + i) - '0');
		else if (*(buf + i) <= 'f' && *(buf + i) >= 'a')
			addr = (addr << 4) | ((*(buf + i) - 'a') + 0xa);
		else if (*(buf + i) <= 'F' && *(buf + i) >= 'A')
			addr = (addr << 4) | ((*(buf + i) - 'A') + 0xa);
		else
			break;
	}

	for (i = i + 1; i < count; i++) {
		if (*(buf + i) <= '9' && *(buf + i) >= '0')
			val = (val << 4) | (*(buf + i) - '0');
		else if (*(buf + i) <= 'f' && *(buf + i) >= 'a')
			val = (val << 4) | ((*(buf + i) - 'a') + 0xa);
		else if (*(buf + i) <= 'F' && *(buf + i) >= 'A')
			val = (val << 4) | ((*(buf + i) - 'A') + 0xa);
		else
			break;
	}
	//pr_debug("addr=0x%x val=0x%x\n", addr, val);
	if (addr > RT5628_VENDOR_ID2 || val > 0xffff || val < 0){
		return count;
	}
	if (i == count) {
		pr_info("0x%02x = 0x%04x\n", addr,
			 rt5628_read(rt5628_codec, addr));
	} else {
		rt5628_write(rt5628_codec, addr, val);
	}

	return count;
}

static DEVICE_ATTR(codec_reg_w, 0666, rt5628_codec_show, rt5628_codec_store);

struct rt5628_init_reg {
	u8 reg;
	u16 val;
};

static struct rt5628_init_reg init_list[] = {
	{RT5628_DAC_DIG_VOL		,0x1010},
	{RT5628_OUT_MIX_CTRL		,0x0704},
	{RT5628_GEN_CTRL		,0x0b00},
	{RT5628_SDP_CTRL		,0x8000},
	{RT5628_SPK_OUT_VOL		,0x8080},
	{RT5628_HP_OUT_VOL		,0x8080},
};
#define RT5628_INIT_REG_LEN ARRAY_SIZE(init_list)

static int rt5628_reg_init(struct snd_soc_codec *codec)
{
	int i;
	for (i = 0; i < RT5628_INIT_REG_LEN; i++)
		snd_soc_write(codec, init_list[i].reg, init_list[i].val);
	return 0;
}

static const DECLARE_TLV_DB_SCALE(out_vol_tlv, -4650, 150, 0);
static const DECLARE_TLV_DB_SCALE(dac_vol_tlv, -3525, 75, 0);
static const DECLARE_TLV_DB_SCALE(in_vol_tlv, -3450, 150, 0);

static const char *rt5628_input_mode[] = {
	"Single ended", "Differential"};

static const struct soc_enum rt5628_linein_mode_enum =
	SOC_ENUM_SINGLE(RT5628_LINE_IN_VOL, RT5628_M_LINEIN_DIF_SFT,
		ARRAY_SIZE(rt5628_input_mode), rt5628_input_mode);

static const char *rt5628_spk_mod_sel[] = {"LPRN", "LPRP", "LPLN", "MM"};

static const struct soc_enum rt5628_spk_mod_enum =
	SOC_ENUM_SINGLE(RT5628_OUT_MIX_CTRL, RT5628_OM_SPK_MOD_SFT,
		ARRAY_SIZE(rt5628_spk_mod_sel), rt5628_spk_mod_sel);

static const char *rt5628_classd_ratio_sel[] = {"2.25 VDD", "2.00 VDD",
		"1.75 VDD", "1.5 VDD", "1.25 VDD", "1 VDD"};

static const struct soc_enum rt5628_classd_ratio_enum =
	SOC_ENUM_SINGLE(RT5628_GEN_CTRL, RT5628_SPK_RATIO_SFT,
		ARRAY_SIZE(rt5628_classd_ratio_sel), rt5628_classd_ratio_sel);

static const struct snd_kcontrol_new rt5628_snd_controls[] = {
	SOC_ENUM("LINEIN Mode Control",  rt5628_linein_mode_enum),
	SOC_ENUM("Classd AMP Ratio",  rt5628_classd_ratio_enum),
	SOC_ENUM("Left SPK Source",  rt5628_spk_mod_enum),
	SOC_DOUBLE("SPK Playback Switch", RT5628_SPK_OUT_VOL,
		RT5628_L_MUTE_SFT, RT5628_R_MUTE_SFT, 1, 1),
	SOC_DOUBLE_TLV("SPK Playback Volume", RT5628_SPK_OUT_VOL,
			RT5628_VOL_L_SFT, RT5628_VOL_R_SFT,
			31, 1, out_vol_tlv),
	SOC_DOUBLE("HP Playback Switch", RT5628_HP_OUT_VOL,
		RT5628_L_MUTE_SFT, RT5628_R_MUTE_SFT, 1, 1),
	SOC_DOUBLE_TLV("HP Playback Volume", RT5628_HP_OUT_VOL,
			RT5628_VOL_L_SFT, RT5628_VOL_R_SFT,
			31, 1, out_vol_tlv),
};

static void hp_depop_mode2(struct snd_soc_codec *codec)
{
	//printk("enter hp_depop_mode2\n"); //bard 6-19
	snd_soc_update_bits(codec, RT5628_PWR_ADD3,
		RT5628_MB, RT5628_MB);
	snd_soc_update_bits(codec, RT5628_HP_OUT_VOL,
		RT5628_L_MUTE | RT5628_R_MUTE,
		RT5628_L_MUTE | RT5628_R_MUTE); //bard 6-19
	snd_soc_update_bits(codec, RT5628_PWR_ADD1,
		RT5628_SOFTGEN, RT5628_SOFTGEN);
	snd_soc_update_bits(codec, RT5628_PWR_ADD2,
		RT5628_VREF, RT5628_VREF);
	snd_soc_update_bits(codec, RT5628_PWR_ADD3,
		RT5628_HP_L_VOL | RT5628_HP_R_VOL,
		RT5628_HP_L_VOL | RT5628_HP_R_VOL);
	snd_soc_update_bits(codec, RT5628_MISC_CTRL2,
		RT5628_EN_DP2_HP, RT5628_EN_DP2_HP);
	msleep(400); //bard 6-19
}

static int hp_pga_event(struct snd_soc_dapm_widget *w,
		struct snd_kcontrol *kcontrol, int event)
{
	struct snd_soc_codec *codec = w->codec;
	//printk("enter hp_pga_event\n"); //bard 6-19
	switch (event) {
	case SND_SOC_DAPM_PRE_PMD:
		//printk("SND_SOC_DAPM_PRE_PMD\n"); //bard 6-19
		snd_soc_update_bits(codec, RT5628_HP_OUT_VOL,
			RT5628_L_MUTE | RT5628_R_MUTE,
			RT5628_L_MUTE | RT5628_R_MUTE); //bard 6-19
		snd_soc_update_bits(codec, RT5628_PWR_ADD1,
			RT5628_HP_AMP | RT5628_HP_EAMP, 0);
		snd_soc_update_bits(codec, RT5628_PWR_ADD3,
			RT5628_HP_L_VOL | RT5628_HP_R_VOL, 0);
		break;

	case SND_SOC_DAPM_POST_PMU:
		//printk("SND_SOC_DAPM_POST_PMU\n"); //bard 6-19
		hp_depop_mode2(codec);
		snd_soc_update_bits(codec, RT5628_PWR_ADD1, RT5628_HP_AMP |
			RT5628_HP_EAMP, RT5628_HP_AMP | RT5628_HP_EAMP);
		snd_soc_update_bits(codec, RT5628_MISC_CTRL2,
				RT5628_EN_DP2_HP, 0);
		snd_soc_update_bits(codec ,RT5628_HP_OUT_VOL,
			RT5628_L_MUTE | RT5628_R_MUTE, 0); //bard 6-19
		break;

	default:
		break;
	}

	return 0;
}

/* Left HP Mixer */
static const struct snd_kcontrol_new rt5628_hp_l_mix[] = {
	SOC_DAPM_SINGLE("Left Linein Playback Switch", RT5628_LINE_IN_VOL,
				RT5628_M_LV_L_HM_L_SFT, 1, 1),
	SOC_DAPM_SINGLE("Left PCM Playback Switch", RT5628_DAC_DIG_VOL,
				RT5628_M_DAC_L_HM_L_SFT, 1, 1),
};

/* Right HP Mixer */
static const struct snd_kcontrol_new rt5628_hp_r_mix[] = {
	SOC_DAPM_SINGLE("Right Linein Playback Switch", RT5628_LINE_IN_VOL,
				RT5628_M_LV_R_HM_R_SFT, 1, 1),
	SOC_DAPM_SINGLE("Right PCM Playback Switch", RT5628_DAC_DIG_VOL,
				RT5628_M_DAC_R_HM_R_SFT, 1, 1),
};

/*SPK mixer*/
static const struct snd_kcontrol_new rt5628_spk_l_mix[] = {
	SOC_DAPM_SINGLE("Left Linein Playback Switch", RT5628_LINE_IN_VOL,
				RT5628_M_LV_L_SM_SFT, 1, 1),
	SOC_DAPM_SINGLE("Left PCM Playback Switch", RT5628_DAC_DIG_VOL,
				RT5628_M_DAC_L_SM_SFT, 1, 1),
};

static const struct snd_kcontrol_new rt5628_spk_r_mix[] = {
	SOC_DAPM_SINGLE("Right Linein Playback Switch", RT5628_LINE_IN_VOL,
				RT5628_M_LV_R_SM_SFT, 1, 1),
	SOC_DAPM_SINGLE("Right PCM Playback Switch", RT5628_DAC_DIG_VOL,
				RT5628_M_DAC_R_SM_SFT, 1, 1),
};

/* SPK Volume Mux */
static const char *rt5628_spk_src_sel[] = {
		"VMID", "HP Mixer", "Speaker Mixer"};

static const struct soc_enum rt5628_spk_src_enum =
	SOC_ENUM_SINGLE(RT5628_OUT_MIX_CTRL, RT5628_OM_SPK_SRC_SFT,
		ARRAY_SIZE(rt5628_spk_src_sel), rt5628_spk_src_sel);

static const struct snd_kcontrol_new rt5628_spk_src_mux =
	SOC_DAPM_ENUM("SPK Out Mux", rt5628_spk_src_enum);

/* HP Volume Mux */
static const char *rt5628_hp_src_sel[] = {"VMID", "HP Mixer"};

static const struct soc_enum rt5628_hp_l_src_enum =
	SOC_ENUM_SINGLE(RT5628_OUT_MIX_CTRL, RT5628_OM_HPL_SRC_SFT,
		ARRAY_SIZE(rt5628_hp_src_sel), rt5628_hp_src_sel);

static const struct snd_kcontrol_new rt5628_hpl_src_mux =
	SOC_DAPM_ENUM("HPL Out Mux", rt5628_hp_l_src_enum);

static const struct soc_enum rt5628_hp_r_src_enum =
	SOC_ENUM_SINGLE(RT5628_OUT_MIX_CTRL, RT5628_OM_HPR_SRC_SFT,
		ARRAY_SIZE(rt5628_hp_src_sel), rt5628_hp_src_sel);

static const struct snd_kcontrol_new rt5628_hpr_src_mux =
	SOC_DAPM_ENUM("HPR Out Mux", rt5628_hp_r_src_enum);

/*HP Mux Out*/
static const char *rt5628_hp_direct_sel[] = {"Normal", "Direct Out"};

static const struct soc_enum rt5628_hp_direct_enum =
	SOC_ENUM_SINGLE(RT5628_OUT_MIX_CTRL, RT5628_OM_DAC_HP_SFT,
		ARRAY_SIZE(rt5628_hp_direct_sel), rt5628_hp_direct_sel);

static const struct snd_kcontrol_new rt5628_hp_direct_mux =
	SOC_DAPM_ENUM("HP Out Mux", rt5628_hp_direct_enum);

static const struct snd_soc_dapm_widget rt5628_dapm_widgets[] = {
	SND_SOC_DAPM_VMID("VMID"),

	SND_SOC_DAPM_INPUT("Left Line In"),
	SND_SOC_DAPM_INPUT("Right Line In"),

	SND_SOC_DAPM_MIXER_NAMED_CTL("Left Linein Volume", RT5628_PWR_ADD3,
			RT5628_LI_L_VOL_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("Right Linein Volume", RT5628_PWR_ADD3,
			RT5628_LI_R_VOL_BIT, 0, NULL, 0),

	SND_SOC_DAPM_DAC("Left DAC", "Left DAC HIFI Playback",
		RT5628_PWR_ADD2, RT5628_DAC_L_BIT, 0),
	SND_SOC_DAPM_DAC("Right DAC", "Right DAC HIFI Playback",
		RT5628_PWR_ADD2, RT5628_DAC_R_BIT, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("PLL", RT5628_PWR_ADD2,
			RT5628_PLL_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("I2S", RT5628_PWR_ADD1,
			RT5628_EN_MAIN_I2S_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("DAC REF", RT5628_PWR_ADD2,
			RT5628_DAC_REF_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("Left DAC Path", RT5628_PWR_ADD2,
			RT5628_D2M_L_DIR_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("Right DAC Path", RT5628_PWR_ADD2,
			RT5628_D2M_R_DIR_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("DAC", SND_SOC_NOPM, 0, 0, NULL, 0),

	SND_SOC_DAPM_MIXER("Left HP Mixer", RT5628_PWR_ADD2, RT5628_HPM_L_BIT,
			0, rt5628_hp_l_mix, ARRAY_SIZE(rt5628_hp_l_mix)),
	SND_SOC_DAPM_MIXER("Right HP Mixer", RT5628_PWR_ADD2, RT5628_HPM_R_BIT,
			0, rt5628_hp_r_mix, ARRAY_SIZE(rt5628_hp_r_mix)),
	SND_SOC_DAPM_MIXER("HP Mixer", SND_SOC_NOPM, 0, 0, NULL, 0),
	SND_SOC_DAPM_MIXER("Left SPK Mixer", SND_SOC_NOPM, 0, 0, 
			rt5628_spk_l_mix, ARRAY_SIZE(rt5628_spk_l_mix)),
	SND_SOC_DAPM_MIXER("Right SPK Mixer", SND_SOC_NOPM, 0, 0, 
			rt5628_spk_r_mix, ARRAY_SIZE(rt5628_spk_r_mix)),
	SND_SOC_DAPM_MIXER("SPK Mixer", RT5628_PWR_ADD2,
			RT5628_SPM_BIT, 0, NULL, 0),

	SND_SOC_DAPM_MUX("SPK Out Mux", SND_SOC_NOPM, 0, 0,
				&rt5628_spk_src_mux),
	SND_SOC_DAPM_MUX("HPL Out Mux", SND_SOC_NOPM, 0, 0,
				&rt5628_hpl_src_mux),
	SND_SOC_DAPM_MUX("HPR Out Mux", SND_SOC_NOPM, 0, 0,
				&rt5628_hpr_src_mux),

	SND_SOC_DAPM_MIXER_NAMED_CTL("SPK Volume", RT5628_PWR_ADD3,
			RT5628_SPK_L_VOL_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("Left HP Volume", RT5628_PWR_ADD3,
			RT5628_HP_L_VOL_BIT, 0, NULL, 0),
	SND_SOC_DAPM_MIXER_NAMED_CTL("Right HP Volume", RT5628_PWR_ADD3,
			RT5628_HP_R_VOL_BIT , 0, NULL, 0),

	SND_SOC_DAPM_PGA_E("HP Depop", SND_SOC_NOPM, 0, 0, NULL, 0,
		hp_pga_event, SND_SOC_DAPM_PRE_PMD | SND_SOC_DAPM_POST_PMU),
	SND_SOC_DAPM_MUX("HP Out Mux", SND_SOC_NOPM, 0, 0,
				&rt5628_hp_direct_mux),

	SND_SOC_DAPM_MIXER_NAMED_CTL("ClassD", RT5628_PWR_ADD2,
			RT5628_CLSD_BIT , 0, NULL, 0),
	SND_SOC_DAPM_OUTPUT("HP"),
	SND_SOC_DAPM_OUTPUT("SPK"),
};

static const struct snd_soc_dapm_route rt5628_dapm_routes[] = {
	{"Left Linein Volume", NULL, "Left Line In"},
	{"Right Linein Volume", NULL, "Right Line In"},

	{"PLL", NULL, "Left DAC"},
	{"I2S", NULL, "PLL"},
	{"DAC REF", NULL, "I2S"},
	{"Left DAC Path", NULL, "DAC REF"},
	{"PLL", NULL, "Right DAC"},
	{"I2S", NULL, "PLL"},
	{"DAC REF", NULL, "I2S"},
	{"Right DAC Path", NULL, "DAC REF"},

	{"Left HP Mixer", "Left Linein Playback Switch", "Left Linein Volume"},
	{"Left HP Mixer", "Left PCM Playback Switch", "Left DAC Path"},

	{"Right HP Mixer", "Right Linein Playback Switch", "Right Linein Volume"},
	{"Right HP Mixer", "Right PCM Playback Switch", "Right DAC Path"},

	{"Left SPK Mixer", "Left Linein Playback Switch", "Left Linein Volume"},
	{"Left SPK Mixer", "Left PCM Playback Switch", "Left DAC Path"},

	{"Right SPK Mixer", "Right Linein Playback Switch", "Right Linein Volume"},
	{"Right SPK Mixer", "Right PCM Playback Switch", "Right DAC Path"},

	{"HP Mixer", NULL, "Left HP Mixer"},
	{"HP Mixer", NULL, "Right HP Mixer"},
	{"SPK Mixer", NULL, "Left SPK Mixer"},
	{"SPK Mixer", NULL, "Right SPK Mixer"},

	{"SPK Out Mux", "VMID", "VMID"},
	{"SPK Out Mux", "HP Mixer", "HP Mixer"},
	{"SPK Out Mux", "Speaker Mixer", "SPK Mixer"},

	{"HPL Out Mux", "VMID", "VMID"},
	{"HPL Out Mux", "HP Mixer", "Left HP Mixer"},
	{"HPR Out Mux", "VMID", "VMID"},
	{"HPR Out Mux", "HP Mixer", "Right HP Mixer"},

	{"SPK Volume", NULL, "SPK Out Mux"},
	{"Left HP Volume", NULL, "HPL Out Mux"},
	{"Right HP Volume", NULL, "HPR Out Mux"},

	{"HP Depop", NULL, "Left HP Volume"},
	{"HP Depop", NULL, "Right HP Volume"},
	{"DAC", NULL, "Left DAC Path"},
	{"DAC", NULL, "Right DAC Path"},

	{"HP Out Mux", "Normal", "HP Depop"},
	{"HP Out Mux", "Direct Out", "DAC"},

	{"ClassD", NULL, "SPK Volume"},
	{"SPK", NULL, "ClassD"},
	{"HP", NULL, "HP Out Mux"},
};

static void rt5628_add_widgets(struct snd_soc_codec *codec)
{
	snd_soc_dapm_new_controls(codec, rt5628_dapm_widgets, 
			ARRAY_SIZE(rt5628_dapm_widgets));
	snd_soc_dapm_add_routes(codec, rt5628_dapm_routes,
		ARRAY_SIZE(rt5628_dapm_routes));
	snd_soc_dapm_new_widgets(codec);
}


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
	{ 3124000,  3072000,  0x392d},			
	{ 4165000,  4233600,  0x3b2d},			
	{ 6240000,  6144000,  0x3d2e},			
	{ 8330000,  8467200,  0x3b2d},			
	{ 12490000,  12288000,  0x746d},			
	{ 16667000,  16934400,  0x3b3a},			
	{  2048000,  22579200,  0x2aa0},
	{  3686400,  22579200,  0x2f20},	
	{ 12000000,  22579200,  0x7e2f},   
	{ 13000000,  22579200,  0x742f},
	{ 13100000,  22579200,  0x3c27},		
	{ 19200000,  22579200,  0x4e2f},		
	{  2048000,  24576000,  0x2ea0},
	{  3686400,  24576000,  0xee27},	
	{ 12000000,  24576000,  0x2915},   
	{ 13000000,  24576000,  0x772e},
	{ 13100000,  24576000,  0x0d20},
	{ 19200000,  24576000,  0x552f}
};

static const struct _pll_div codec_slave_pll_div[] = {
	{  1024000,  16384000,  0x3ea0},	
	{  1411200,  22579200,  0x3ea0},
	{  1536000,  24576000,  0x3ea0},	
	{  2048000,  16384000,  0x1ea0},	
	{  2822400,  22579200,  0x1ea0},
	{  3072000,  24576000,  0x1ea0},
	{  705600,  22579200,  0x7ea0},
	{  705600,  11289600,  0x3ea0},
	{  705600,  8467200,  0x3ab0},
	{  256000,  24576000,  0x7ea0}
};

struct _coeff_div{
	u32 mclk;
	u32 bclk;
	u32 rate;
	u32 reg_val;
};

static int rt5628_set_dai_fmt(struct snd_soc_dai *dai, unsigned int fmt)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5628_priv *rt5628 = codec->private_data;
	u16 iface = 0;

	switch (fmt & SND_SOC_DAIFMT_MASTER_MASK) {
	case SND_SOC_DAIFMT_CBM_CFM:
		iface = 0x0000;
		rt5628->master = 1;
		break;
	case SND_SOC_DAIFMT_CBS_CFS:
		rt5628->master = 0;
		iface = 0x8000;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		iface |= 0x0000;
		break;
	case SND_SOC_DAIFMT_LEFT_J:
		iface |= 0x0001;
		break;
	case SND_SOC_DAIFMT_DSP_A:
		iface |= 0x0002;
		break;
	case SND_SOC_DAIFMT_DSP_B:
		iface |= 0x0003;
		break;
	default:
		return -EINVAL;
	}

	switch (fmt & SND_SOC_DAIFMT_INV_MASK) {
	case SND_SOC_DAIFMT_NB_NF:
		iface |= 0x0000;
		break;
	case SND_SOC_DAIFMT_IB_NF:
		iface |= 0x0080;
		break;
	default:
		return -EINVAL;		
	}

	snd_soc_write(codec, RT5628_SDP_CTRL, iface);	
	return 0;
}


#define RT_IISMOD_256FS	  0
#define RT_IISMOD_384FS	  1

struct clk_info_s {
	u8 i2s_pre_div;       //000b - 101b
	u8 iismod;	//0b:256fs; 1b:384fs
};

static int get_clock_info(int sysclk, int rate, struct clk_info_s *clk_info_p)
{
	int mult_rate, exp, exp_two;

	if(sysclk <= 0 || rate <= 0 || sysclk % rate || clk_info_p == NULL)
		return -EINVAL;

	mult_rate = sysclk / rate;
	//calculate for 256fs
	if(0 == (mult_rate % 256)) {
		exp_two = mult_rate / 256;
		exp = 0;
		while(!(exp_two % 2)) {
			exp_two /= 2;
			exp++;
		}
		if(exp_two < 2) {//sysclk = 256fs * 2^exp
			clk_info_p->i2s_pre_div = exp;
			clk_info_p->iismod = RT_IISMOD_256FS;
			return 0;
		}
	}
	//calculate for 384fs
	if(0 == (mult_rate % 384)) {
		exp_two = mult_rate / 384;
		exp = 0;
		while(!(exp_two % 2)) {
			exp_two /= 2;
			exp++;
		}
		if(exp_two < 2) {//sysclk = 384fs * 2^exp
			clk_info_p->i2s_pre_div = exp;
			clk_info_p->iismod = RT_IISMOD_384FS;
			return 0;
		}
	}

	return -EINVAL;
}

static int rt5628_hifi_pcm_params(struct snd_pcm_substream *substream, 
	struct snd_pcm_hw_params *params, struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_device *socdev = rtd->socdev;
	struct snd_soc_codec *codec = socdev->card->codec;
	struct rt5628_priv *rt5628 = codec->private_data;
	struct clk_info_s clk_info;
	unsigned short iface = 0, clk_ctrl;
	int rate = params_rate(params), ret = 0;

	if(0 != (ret = get_clock_info(rt5628->sysclk, rate, &clk_info))) {
		dev_err(codec->dev, "clock err\n");
		return -EINVAL;
	}

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_BE:
	case SNDRV_PCM_FORMAT_S16_LE:
		break;
	case SNDRV_PCM_FORMAT_S20_3LE:
		iface |= RT5628_MAIN_I2S_DL_20BITS;
		break;
	case SNDRV_PCM_FORMAT_S24_LE:
		iface |= RT5628_MAIN_I2S_DL_24BITS;
		break;
	default:
		return -EINVAL;
	}

	snd_soc_update_bits(codec, RT5628_SDP_CTRL,
		RT5628_MAIN_I2S_DL_MASK, iface);
	if(ret == 0) {
		clk_ctrl = (clk_info.i2s_pre_div << RT5628_CLK_PD_SFT) |
			(clk_info.iismod << RT5628_CLK_FC_SFT);
		snd_soc_update_bits(codec, RT5628_DAC_CLK_CTRL,
			RT5628_CLK_PD_MASK | RT5628_CLK_FC_MASK, clk_ctrl);
	}

	return 0;
}

static int rt5628_hifi_codec_set_dai_sysclk(struct snd_soc_dai *dai, int clk_id, unsigned int freq, int dir)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5628_priv *rt5628 = codec->private_data;

	if ((freq >= (256 * 8000)) && (freq <= (512 * 48000))) {
		//dev_err(codec->dev, "sysclk freq %u for audio i2s\n", freq);
		rt5628->sysclk = freq;
		return 0;
	}

	return -EINVAL;
}

static int rt5628_codec_set_dai_pll(struct snd_soc_dai *dai, 
		int pll_id, unsigned int freq_in, unsigned int freq_out)
{
	struct snd_soc_codec *codec = dai->codec;
	struct rt5628_priv *rt5628 = codec->private_data;
	int i, ret = -EINVAL;
	
	if (!freq_in || !freq_out) {
		dev_dbg(codec->dev, "PLL disabled\n");
		snd_soc_update_bits(codec, RT5628_GLB_CLK_CTRL,
			RT5628_SYSCLK_SEL_MASK,
			RT5628_SYSCLK_SEL_MCLK);
		return 0;
	}
		
	if (rt5628->master) {
		for (i = 0; i < ARRAY_SIZE(codec_master_pll_div); i ++) {
			if ((freq_in == codec_master_pll_div[i].pll_in) &&
				(freq_out == codec_master_pll_div[i].pll_out)) {
				snd_soc_write(codec, RT5628_PLL_CTRL,
					codec_master_pll_div[i].regvalue);
				mdelay(20);
				snd_soc_update_bits(codec, RT5628_GLB_CLK_CTRL,
					RT5628_SYSCLK_SEL_MASK |
					RT5628_PLL_SEL_MASK,
					RT5628_SYSCLK_SEL_PLL |
					RT5628_PLL_SEL_MCLK);
				ret = 0;
			}
		}
	} else {
		for (i = 0; i < ARRAY_SIZE(codec_slave_pll_div); i ++) {
			if ((freq_in == codec_slave_pll_div[i].pll_in) &&
				(freq_out == codec_slave_pll_div[i].pll_out))  {
				snd_soc_write(codec, RT5628_PLL_CTRL,
					codec_slave_pll_div[i].regvalue);
				mdelay(20);
				snd_soc_update_bits(codec, RT5628_GLB_CLK_CTRL,
					RT5628_SYSCLK_SEL_MASK |
					RT5628_PLL_SEL_MASK,
					RT5628_SYSCLK_SEL_PLL |
					RT5628_PLL_SEL_BCLK);
				ret = 0;
			}
		}
	}

	return ret;
}


#define RT5628_RATES SNDRV_PCM_RATE_8000_48000
#define RT5628_FORMAT (SNDRV_PCM_FMTBIT_S16_LE |\
			SNDRV_PCM_FMTBIT_S16_BE |\
			SNDRV_PCM_FMTBIT_S20_3LE |\
			SNDRV_PCM_FMTBIT_S24_LE)

struct snd_soc_dai_ops rt5628_hifi_ops = {
	.hw_params = rt5628_hifi_pcm_params,
	.set_pll = rt5628_codec_set_dai_pll,
	.set_fmt = rt5628_set_dai_fmt,
	.set_sysclk = rt5628_hifi_codec_set_dai_sysclk,
};

struct snd_soc_dai rt5628_dai = {
	.name = "RT5628",
	.id = 1,
	.playback = {
		.stream_name = "HIFI Playback",
		.channels_min = 1,
		.channels_max = 2,
		.rates = RT5628_RATES,
		.formats = RT5628_FORMAT
	},
	.ops = &rt5628_hifi_ops,
};
EXPORT_SYMBOL_GPL(rt5628_dai);

static void rt5628_sync_cache(struct snd_soc_codec *codec)
{
	const u16 *reg_cache = codec->reg_cache;
	int i;

	/* Sync back cached values if they're different from the
	 * hardware default.
	 */
	for (i = 1; i < codec->reg_cache_size; i++) {
		if (reg_cache[i] == rt5628_reg[i])
			continue;
		snd_soc_write(codec, i, reg_cache[i]);
	}
}

static inline void rt5628_dump_regs(struct snd_soc_codec *codec)
{
	u16 i, regVal;

	pr_info("\n[ RT5628 Register ]\n");
	for (i = 0; i <= 0x7E; i ++) {
		regVal = snd_soc_read(codec, i);
		if (regVal) pr_info("    0x%02x: 0x%04x\n", i, regVal);
	}
}

static int rt5628_set_bias_level(struct snd_soc_codec *codec, enum snd_soc_bias_level level)
{
	switch (level) {
	case SND_SOC_BIAS_ON:
		/*dev_info(codec->dev, "\n[ SND_SOC_BIAS_ON ]");
		rt5628_dump_regs(codec);*/
//		printk("rt5628_set_bias_level case SND_SOC_BIAS_ON:\n"); //bard 6-19
		snd_soc_update_bits(codec, RT5628_HP_OUT_VOL,
			RT5628_L_MUTE | RT5628_R_MUTE, 0);
		snd_soc_update_bits(codec, RT5628_SPK_OUT_VOL,
			RT5628_L_MUTE | RT5628_R_MUTE, 0);
		break;

	case SND_SOC_BIAS_PREPARE:
		//dev_info(codec->dev, "\n[ SND_SOC_BIAS_PREPARE ]");
		break;

	case SND_SOC_BIAS_STANDBY:		
		snd_soc_write(codec, RT5628_PWR_ADD1, 0x0000);
		snd_soc_write(codec, RT5628_PWR_ADD2, RT5628_VREF); //bard 6-19
		snd_soc_write(codec, RT5628_PWR_ADD3, RT5628_MB);
		if (codec->bias_level == SND_SOC_BIAS_OFF) {
			snd_soc_write(codec, RT5628_PWR_ADD2, RT5628_VREF); //bard 6-19
			snd_soc_write(codec, RT5628_PWR_ADD3, RT5628_MB);
			//hp_depop_mode2(codec); //bard 6-19 remove
			rt5628_sync_cache(codec);
		}
		break;

	case SND_SOC_BIAS_OFF:
		snd_soc_write(codec, RT5628_PWR_ADD1, 0x0000);
		snd_soc_write(codec, RT5628_PWR_ADD2, 0x0000);
		snd_soc_write(codec, RT5628_PWR_ADD3, 0x0000);
		break;

	default:
		break;
	}
	codec->bias_level = level;

	return 0;
}

static int rt5628_init(struct snd_soc_device *socdev)
{
	struct snd_soc_codec *codec = socdev->card->codec;
	int ret = 0;

	codec->name = "RT5628";
	codec->owner = THIS_MODULE;
	codec->read = rt5628_read;
	codec->write = rt5628_write;
	codec->set_bias_level = rt5628_set_bias_level;
	codec->dai= &rt5628_dai;
	codec->num_dai = 1;
	codec->reg_cache_size = ARRAY_SIZE(rt5628_reg);
	codec->reg_cache_step = 1;
	codec->reg_cache = kmemdup(rt5628_reg, sizeof(rt5628_reg), GFP_KERNEL);
	if (codec->reg_cache == NULL)
		return -ENOMEM;

	ret = snd_soc_new_pcms(socdev, SNDRV_DEFAULT_IDX1, SNDRV_DEFAULT_STR1);
	if (ret < 0 ) {
		dev_err(codec->dev, "failed to create pcms\n");
		goto pcm_err;
	}

	rt5628_reset(codec);
	snd_soc_write(codec, RT5628_PWR_ADD2, RT5628_VREF); //bard 6-19
	snd_soc_write(codec, RT5628_PWR_ADD3, RT5628_MB);
	//hp_depop_mode2(codec); //bard 6-19 remove
	rt5628_reg_init(codec);
	codec->bias_level = SND_SOC_BIAS_STANDBY;

	ret = snd_soc_init_card(socdev);
	if (ret < 0) {
		dev_err(codec->dev, "failed to register card\n");
		goto card_err;
	}

	snd_soc_add_controls(codec, rt5628_snd_controls,
		ARRAY_SIZE(rt5628_snd_controls));
	rt5628_add_widgets(codec);

	dev_info(codec->dev, "RT5628 initial ok\n");

	return ret;

card_err:
	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);

pcm_err:
	kfree(codec->reg_cache);
	codec->reg_cache = NULL;
	return ret;
}
static int rt5628_probe(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct rt5628_setup_data *setup = socdev->codec_data;
	struct snd_soc_codec *codec;
	struct rt5628_priv *rt5628;
	int ret;

	pr_info("enter %s\n", __func__);	


	codec = kzalloc(sizeof(struct snd_soc_codec), GFP_KERNEL);
	if (codec == NULL)
		return -ENOMEM;

	rt5628 = kzalloc(sizeof(struct rt5628_priv), GFP_KERNEL);
	if (rt5628 == NULL) {
		ret = -ENOMEM;
		goto priv_err;	
	}

	codec->dev = &pdev->dev;
	codec->private_data = rt5628;
	socdev->card->codec = codec;
	mutex_init(&codec->mutex);
	INIT_LIST_HEAD(&codec->dapm_widgets);
	INIT_LIST_HEAD(&codec->dapm_paths);
	rt5628_init(socdev);

	rt5628_codec = codec;
	ret = device_create_file(codec->dev, &dev_attr_codec_reg_w);
	if (ret != 0) {
		dev_err(codec->dev,
			"Failed to create codex_reg sysfs files: %d\n", ret);
		return ret;
	}

	return ret;

priv_err:
	kfree(codec);
	return ret;
}

static int rt5628_remove(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	if (codec->control_data)
		rt5628_set_bias_level(codec, SND_SOC_BIAS_OFF);

	snd_soc_free_pcms(socdev);
	snd_soc_dapm_free(socdev);
	kfree(codec->private_data);
	kfree(codec);
	device_remove_file(codec->dev, &dev_attr_codec_reg_w);
	
	return 0;
}


static int rt5628_suspend(struct platform_device *pdev, pm_message_t state)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	rt5628_set_bias_level(codec, SND_SOC_BIAS_OFF);

	return 0;
}

static int rt5628_resume(struct platform_device *pdev)
{
	struct snd_soc_device *socdev = platform_get_drvdata(pdev);
	struct snd_soc_codec *codec = socdev->card->codec;

	rt5628_set_bias_level(codec, SND_SOC_BIAS_STANDBY);

	return 0;
}


struct snd_soc_codec_device soc_codec_dev_rt5628 = {
	.probe = 	rt5628_probe,
	.remove = rt5628_remove,
	.suspend = rt5628_suspend,
	.resume = rt5628_resume,
};
EXPORT_SYMBOL_GPL(soc_codec_dev_rt5628);

static int __init rt5628_modinit(void)
{
	return snd_soc_register_dai(&rt5628_dai);
}
module_init(rt5628_modinit);

static void __exit rt5628_modexit(void)
{
	snd_soc_unregister_dai(&rt5628_dai);
}
module_exit(rt5628_modexit);

MODULE_DESCRIPTION("ASoC RT5628 driver");
MODULE_AUTHOR("Johnny Hsu <johnnyhsu@realtek.com>");
MODULE_LICENSE("GPL");
