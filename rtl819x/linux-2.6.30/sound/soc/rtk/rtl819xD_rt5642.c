/*
 * rtl819xD_rt5640.c - Realtek machine ASoC driver.
 *
 * Author: Johnny Hsu <johnnyhsu@realtek.com>
 * Copyright 2012 Realtek Semiconductor Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see the file COPYING, or write
 * to the Free Software Foundation, Inc.,
 * 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/tlv.h>
#include <sound/pcm_params.h>

#include "../codecs/rt5640.h"
#include "rtl819x-pcm.h"
#include "rtl8197d-i2s.h"

static int rtl819xd_rt5640_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	unsigned int clk = 0;
	int bfs, rfs, ret = 0;

	
	pr_debug("%s rate %d format %x\n", __func__, params_rate(params),
		params_format(params));

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_U8:
	case SNDRV_PCM_FORMAT_S8:
		bfs = 16;
		break;
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_S16_LE:
	case SNDRV_PCM_FORMAT_S16_BE:
		bfs = 32;
		break;
	default:
		return -EINVAL;
	}

	switch (params_rate(params)) {
	case 16000:
	case 32000:
	case 48000:
	case 8000:
		rfs = 256;
		break;
	default:
		return -EINVAL;
	}
	clk = params_rate(params) * rfs;
	/* set codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);	
	if (ret < 0)
		return ret;
	/* set cpu DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S |
		SND_SOC_DAIFMT_NB_NF | SND_SOC_DAIFMT_CBS_CFS);	
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(codec_dai, 0, clk, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;
	return 0;
}

static struct snd_soc_ops rtl819xd_rt5640_ops = {
	.hw_params = rtl819xd_rt5640_hw_params,
};

#if 1
static int rtl819xd_rt5640_event_int_spk(struct snd_soc_dapm_widget *widget,
			  struct snd_kcontrol *kctl, int event)
{
	struct snd_soc_codec *codec = widget->codec;
	printk("rtl819xd_rt5640_event_int_spk\n");

		
	if (SND_SOC_DAPM_EVENT_ON(event))
	{

		snd_soc_update_bits(codec, RT5640_SPK_VOL,
			RT5640_L_MUTE | RT5640_R_MUTE, 0);
	}
	else
	{
		snd_soc_update_bits(codec, RT5640_SPK_VOL,
			RT5640_L_MUTE | RT5640_R_MUTE,
			RT5640_L_MUTE | RT5640_R_MUTE);

	}
	return 0;
}

static int rtl819xd_rt5640_event_hp(struct snd_soc_dapm_widget *widget,
			  struct snd_kcontrol *kctl, int event)
{
	struct snd_soc_codec *codec = widget->codec;
	printk("rtl819xd_rt5640_event_hp\n");
	
	if (SND_SOC_DAPM_EVENT_ON(event))
	{

		rt5640_pmu_depop(codec);

	}
	else
	{
	
		rt5640_pmd_depop(codec);

	}
	return 0;
}

static const struct snd_soc_dapm_widget rtl819xd_dapm_widgets[] = {
	SND_SOC_DAPM_SPK("Int Spk", rtl819xd_rt5640_event_int_spk),
	SND_SOC_DAPM_HP("Headphone Jack", rtl819xd_rt5640_event_hp),
};
#if 1
static const struct snd_soc_dapm_route rtl819xd_audio_map[] = {
	{"HPOR", NULL, "Headphone Jack"},
	{"HPOL", NULL, "Headphone Jack"},
	{"SPORP", NULL, "Int Spk"},
	{"SPORN", NULL, "Int Spk"},
	{"SPOLP", NULL, "Int Spk"},
	{"SPOLN", NULL, "Int Spk"},
};
#else
static const struct snd_soc_dapm_route rtl819xd_audio_map[] = {
	{"Headphone Jack", NULL, "HPOR"},
	{"Headphone Jack", NULL, "HPOL"},
	{"Int Spk", NULL, "SPORP"},
	{"Int Spk", NULL, "SPORN"},
	{"Int Spk", NULL, "SPOLP"},
	{"Int Spk", NULL, "SPOLN"},
};
#endif

static int rtl819xd_rt5640_rt5640_init(struct snd_soc_codec *codec)
{
	snd_soc_dapm_new_controls(codec, rtl819xd_dapm_widgets, 
		ARRAY_SIZE(rtl819xd_dapm_widgets));
	snd_soc_dapm_add_routes(codec, rtl819xd_audio_map, 
		ARRAY_SIZE(rtl819xd_audio_map));
	
	snd_soc_dapm_enable_pin(codec, "Headphone Jack");
	//snd_soc_dapm_enable_pin(codec, "Int Spk");
	snd_soc_dapm_sync(codec);
	return 0;
}
#endif


static struct snd_soc_dai_link rtl819xd_rt5640_dai = {
	.name = "rtl819xd",
	.stream_name = "RT5640",
	.cpu_dai = &rtl8197d_i2s_dai,
	.codec_dai = &rt5640_dai,
	//.init = rtl819xd_rt5640_rt5640_init,
	.ops = &rtl819xd_rt5640_ops,
};

static struct rt5640_setup_data rtl819xd_rt5640_setup = {
	.i2c_bus = 0,
	.i2c_address = 0x1c,
};

static struct snd_soc_card rtl819xd_rt5640 = {
	.name = "rtl819xd_rt5640",
	.platform = &rtl819x_soc_platform,
	.dai_link = &rtl819xd_rt5640_dai,
	.num_links = 1,
};

static struct snd_soc_device rtl819xd_snd_devdata = {
	.card = &rtl819xd_rt5640,
	.codec_dev = &soc_codec_dev_rt5640,
	.codec_data = &rtl819xd_rt5640_setup,
};

static struct platform_device *rtl819xd_snd_device;

static int __init rtl819xd_rt5640_init(void)
{
	int ret;
	printk(KERN_DEBUG "enter %s\n", __func__);	
	rtl819xd_snd_device = platform_device_alloc("soc-audio", -1);
	if (!rtl819xd_snd_device)
		return -ENOMEM;

	platform_set_drvdata(rtl819xd_snd_device, &rtl819xd_snd_devdata);
	rtl819xd_snd_devdata.dev = &rtl819xd_snd_device->dev;
	ret = platform_device_add(rtl819xd_snd_device);
	if (ret)
		platform_device_put(rtl819xd_snd_device);

	return ret;
}
module_init(rtl819xd_rt5640_init);

static void __exit rtl819xd_rt5640_exit(void)
{
	platform_device_unregister(rtl819xd_snd_device);
}
module_exit(rtl819xd_rt5640_exit);

MODULE_DESCRIPTION("ASoC RT5640 driver");
MODULE_AUTHOR("Johnny Hsu <johnnyhsu@realtek.com>");
MODULE_LICENSE("GPL");
