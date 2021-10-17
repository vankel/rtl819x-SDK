/*
 *  rtl819x-pcm.h --
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  ALSA PCM interface for the Realtek 819x CPU
 */

#ifndef _RTL819X_PCM_H
#define _RTL819X_PCM_H

#define rtlRegRead(addr)        \
        (*(volatile u32 *)(addr))

#define rtlRegWrite(addr, val)  \
        ((*(volatile u32 *)(addr)) = (val))

static inline u32 rtlRegMask(u32 addr, u32 mask, u32 value)
{
	u32 reg;

	reg = rtlRegRead(addr);
	reg &= ~mask;
	reg |= value & mask;
	rtlRegWrite(addr, reg);
	reg = rtlRegRead(addr); /* flush write to the hardware */

	return reg;
}

#ifdef CONFIG_SND_RTL819XD_SOC_I2S_8KHZ
  #define SNDRV_PCM_RATE SNDRV_PCM_RATE_8000
  #define SNDRV_PCM_RATE_NUM 8000
  #define RTL819XD_SOC_I2S_SR 0
#elif defined(CONFIG_SND_RTL819XD_SOC_I2S_16KHZ)
  #define SNDRV_PCM_RATE SNDRV_PCM_RATE_16000
  #define SNDRV_PCM_RATE_NUM 16000
  #define RTL819XD_SOC_I2S_SR 1
#elif defined(CONFIG_SND_RTL819XD_SOC_I2S_24KHZ)
  #define SNDRV_PCM_RATE SNDRV_PCM_RATE_24000
  #define SNDRV_PCM_RATE_NUM 24000
  #define RTL819XD_SOC_I2S_SR 2
#elif defined(CONFIG_SND_RTL819XD_SOC_I2S_32KHZ)
  #define SNDRV_PCM_RATE SNDRV_PCM_RATE_32000
  #define SNDRV_PCM_RATE_NUM 32000
  #define RTL819XD_SOC_I2S_SR 3
#elif defined(CONFIG_SND_RTL819XD_SOC_I2S_48KHZ)
  #define SNDRV_PCM_RATE SNDRV_PCM_RATE_48000
  #define SNDRV_PCM_RATE_NUM 48000
  #define RTL819XD_SOC_I2S_SR 5
#endif

#define ST_RUNNING		(1<<0)
#define ST_OPENED		(1<<1)

struct rtl819x_pcm_dma_params {
	//struct s3c2410_dma_client *client;	/* stream identifier */
	int channel;				/* Channel ID */
	dma_addr_t dma_addr;
	int dma_size;			/* Size of the DMA transfer */
};

#define S3C24XX_DAI_I2S			0

/* platform data */
extern struct snd_soc_platform rtl819x_soc_platform;
//extern struct snd_ac97_bus_ops s3c24xx_ac97_ops;

#endif
