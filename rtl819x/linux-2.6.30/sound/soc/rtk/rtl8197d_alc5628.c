/*
 * SoC audio for rtl8197d
 *
 * Copyright 2007 KonekTel, a.s.
 * Author: Ivan Kuten
 *         ivan.kuten@promwad.com
 *
 * Heavily based on smdk2443_wm9710.c
 * Copyright 2007 Wolfson Microelectronics PLC.
 * Author: Graeme Gregory
 *         graeme.gregory@wolfsonmicro.com or linux@wolfsonmicro.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/device.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/uda134x.h>

//#include "../codecs/alc5628.h"
#include "../codecs/uda134x.h" /* wait alc5628 codec, jwsyu 20120224 */
#include "rtl819x-pcm.h"
#include "rtl8197d-i2s.h"

static struct snd_soc_card rtl8197d;

static struct snd_soc_dai_link rtl8197d_dai[] = {
{
	.name = "I2S",
	.stream_name = "RTL8197D I2S",
	.cpu_dai = &rtl8197d_i2s_dai,
	//.codec_dai = &ac97_dai,
	.codec_dai = &uda134x_dai, /* wait alc5628 codec, jwsyu 20120224 */
},
};

static struct snd_soc_card rtl8197d = {
	.name = "RTL8197D",
	.platform = &rtl819x_soc_platform,
	.dai_link = rtl8197d_dai,
	.num_links = ARRAY_SIZE(rtl8197d_dai),
};
static struct uda134x_platform_data s3c24xx_uda134x;
#if 0
	.l3 = {
		.setdat = setdat,
		.setclk = setclk,
		.setmode = setmode,
		.data_hold = 1,
		.data_setup = 1,
		.clock_high = 1,
		.mode_hold = 1,
		.mode = 1,
		.mode_setup = 1,
	},
};
#endif

static struct snd_soc_device rtl8197d_snd_i2s_devdata = {
	.card = &rtl8197d,
	//.codec_dev = &soc_codec_dev_uda134x, /* wait alc5628 codec, jwsyu 20120224 */
	.codec_dev = &soc_codec_dev_uda134x,
	.codec_data = &s3c24xx_uda134x,
};

static struct platform_device *rtl8297d_snd_i2s_device;

static int __init rtl8297d_init(void)
{
	int ret;

	rtl8297d_snd_i2s_device = platform_device_alloc("soc-audio", -1);
	if (!rtl8297d_snd_i2s_device)
		return -ENOMEM;

	platform_set_drvdata(rtl8297d_snd_i2s_device,
				&rtl8197d_snd_i2s_devdata);
	rtl8197d_snd_i2s_devdata.dev = &rtl8297d_snd_i2s_device->dev;
	ret = platform_device_add(rtl8297d_snd_i2s_device);

	if (ret)
		platform_device_put(rtl8297d_snd_i2s_device);

	return ret;
}

static void __exit rtl8297d_exit(void)
{
	platform_device_unregister(rtl8297d_snd_i2s_device);
}

module_init(rtl8297d_init);
module_exit(rtl8297d_exit);

/* Module information */
MODULE_AUTHOR("XU JUNWEI, <jwsyu@realtek.com>");
MODULE_DESCRIPTION("ALSA Soc ALC5628 RTL8197D");
MODULE_LICENSE("GP1");
