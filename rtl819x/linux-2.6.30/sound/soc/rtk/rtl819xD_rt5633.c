/*
 * rtl819xD_rt5633.c - Realtek machine ASoC driver.
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

#include "../codecs/rt5633.h"
#include "rtl819x-pcm.h"
#include "rtl8197d-i2s.h"

static int rtl819xd_rt5633_hw_params(struct snd_pcm_substream *substream,
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

static struct snd_soc_ops rtl819xd_rt5633_ops = {
	.hw_params = rtl819xd_rt5633_hw_params,
};

static struct snd_soc_dai_link rtl819xd_rt5633_dai = {
	.name = "rtl819xd",
	.stream_name = "RT5633",
	.cpu_dai = &rtl8197d_i2s_dai,
	.codec_dai = &rt5633_dai,
	.ops = &rtl819xd_rt5633_ops,
};

static struct snd_soc_card rtl819xd_rt5633 = {
	.name = "rtl819xd_rt5633",
	.platform = &rtl819x_soc_platform,
	.dai_link = &rtl819xd_rt5633_dai,
	.num_links = 1,
};

static struct snd_soc_device rtl819xd_snd_devdata = {
	.card = &rtl819xd_rt5633,
	.codec_dev = &soc_codec_dev_rt5633,
};

static struct platform_device *rtl819xd_snd_device;
void alc5633_i2c_share_pin( void );
static int __init rtl819xd_rt5633_init(void)
{
	int ret;

	pr_info("%s enter\n", __func__);
	rtl819xd_snd_device = platform_device_alloc("soc-audio", -1);
	if (!rtl819xd_snd_device)
		return -ENOMEM;

	alc5633_i2c_share_pin();

	platform_set_drvdata(rtl819xd_snd_device, &rtl819xd_snd_devdata);
	rtl819xd_snd_devdata.dev = &rtl819xd_snd_device->dev;
	ret = platform_device_add(rtl819xd_snd_device);
	if (ret)
		platform_device_put(rtl819xd_snd_device);

	return ret;
}
module_init(rtl819xd_rt5633_init);

static void __exit rtl819xd_rt5633_exit(void)
{
	pr_info("%s enter\n", __func__);
	platform_device_unregister(rtl819xd_snd_device);
}
module_exit(rtl819xd_rt5633_exit);

MODULE_DESCRIPTION("ASoC RT5633 driver");
MODULE_AUTHOR("Johnny Hsu <johnnyhsu@realtek.com>");
MODULE_LICENSE("GPL");
