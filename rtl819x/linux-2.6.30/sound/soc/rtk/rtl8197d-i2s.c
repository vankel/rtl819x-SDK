/*
 * rtl8197d-i2s.c  --  ALSA Soc Audio Layer
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  Revision history
 *    24th Feb 2012   Initial version.
 *    4th May 2012    add capture support
 *    6th Nov 2013    add mono channel support
 */
#define DEBUG


#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/jiffies.h>
#include <linux/io.h>
#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/initval.h>
#include <sound/soc.h>
#include <sound/info.h>
#include <linux/config.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/ioctl.h>
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
#include "bspchip.h"
#define IIS_IRQ BSP_I2S_IRQ
  #else
#define IIS_IRQ	26
  #endif


#include "rtl819x-pcm.h"
#include "rtl8197d-i2s.h"

#define I2S_D2A_OUT	1
#define I2S_A2D_IN	2

#define IIS_PAGE_NUM	4
#define IIS_PAGE_SIZE	(80*3)	// 80 * 32bit, 160sample, 20ms

static short iis_tx_buf[IIS_PAGE_NUM*IIS_PAGE_SIZE*2*4+256]__attribute__ ((aligned (32)));
static short iis_rx_buf[IIS_PAGE_NUM*IIS_PAGE_SIZE*2*4+256]__attribute__ ((aligned (32)));


static unsigned long IISChanTxPage[4] = {IIS_TX_P0OK, IIS_TX_P1OK, IIS_TX_P2OK, IIS_TX_P3OK};
static unsigned long IISChanRxPage[4] = {IIS_RX_P0OK, IIS_RX_P1OK, IIS_RX_P2OK, IIS_RX_P3OK};

static int iis_txpage[2];
static int iis_rxpage[2];
static int iis_tr_cnt[2];
int NeedSwap=0;
	/*this iis_counter is used for counting how many pcm streams are opened*/
int iis_counter=0;
/* size :byte unit */
static int iis_set_page_size(unsigned int chid, unsigned int size)
{
	/* Write the reg IIS_SETTING to set pagesize. */
	
	unsigned int n_size;
	unsigned int temp;
	n_size = (size/4 - 1);
	temp = rtlRegRead(IIS_SETTING) & (~0xFFF);
	rtlRegWrite(IIS_SETTING, temp | n_size );	//set pagesize

	//IISDBUG("set channel %d page size = %d\n", chid, size);
	// too many console message will cause R0, T0
	//printk("set channel %d page size = %d\n", chid, size);
	return 0;
}

#ifndef OPTIMIZATION
static unsigned int iis_get_page_size(unsigned int chid)
{
	/* Read the reg IIS_SETTING to get pagesize*/
	unsigned int pagesize, n_size;	/* Actual pagesize which can get from "iis_get_page_size()".
 					It's different from the IISPAGE_SIZE define in header file. */
	
	n_size =  rtlRegRead(IIS_SETTING) & 0xFFF;
	pagesize = 4*(n_size + 1);

	//IISDBUG("get channel %d page size = %d\n", chid, pagesize);

	return pagesize;
}
#endif
#if 1 //For debug 

/* Set Tx, Rx own bit to IIS Controller. */

static void iis_set_tx_own_bit(unsigned int pageindex)
{
	//printk("iis_tx:%d\n", pageindex);
	rtlRegWrite(IIS_TX_P0OWN + 4*pageindex, BIT(31));
	//printk("IIS_TX_P%dOWN= 0x%x\n", pageindex, rtlRegRead(IIS_TX_P0OWN + 4*pageindex));
	//PDBUG("set iis tx own bit %d to HW \n", pageindex );
}

static void iis_set_rx_own_bit(unsigned int pageindex)
{
	//printk("rx:%d\n", pageindex);
	rtlRegWrite(IIS_RX_P0OWN + 4*pageindex, BIT(31));
	//printk("IIS_RX_P%dOWN= 0x%x\n", pageindex, rtlRegRead(IIS_RX_P0OWN + 4*pageindex));
	//PDBUG("set iis rx own bit %d to HW \n", pageindex );
}

static void iis_isr_reset(unsigned int chid)
{
	//printk("1 IIS_TX_ISR= 0x%x. IIS_RX_ISR= 0x%x\n", rtlRegRead(IIS_TX_ISR), rtlRegRead(IIS_RX_ISR));
	rtlRegWrite(IIS_TX_ISR, 0x3f);
	rtlRegWrite(IIS_RX_ISR, 0x3f);
	//printk("2 IIS_TX_ISR= 0x%x. IIS_RX_ISR= 0x%x\n", rtlRegRead(IIS_TX_ISR), rtlRegRead(IIS_RX_ISR));
}

static void iis_imr_enable(unsigned int chid, unsigned char type)
{
	//IISDBUG("enable IIS IMR\n");
#if defined(CONFIG_SND_RTL819XD_SOC_ALC5628)
	switch(type)
	{
		case P0OK_TX:
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)|(IIS_TX_P0OK));
			break;
	
		case P1OK_TX:
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)|(IIS_TX_P1OK));
			break;
	
		case P2OK_TX:
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)|(IIS_TX_P2OK));
			break;
	
		case P3OK_TX:
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)|(IIS_TX_P3OK));
			break;

		case TPUA:	/* tx page unavailable */
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)|(IIS_TX_PAGEUNAVA));
			break;
	
		case TFEM:	/* tx fifo empty */
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)|(IIS_TX_FIFO_EMPTY));
			break;

		case P0OK_RX:
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)|(IIS_RX_P0OK));
			break;
	
		case P1OK_RX:
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)|(IIS_RX_P1OK));
			break;
	
		case P2OK_RX:
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)|(IIS_RX_P2OK));
			break;
	
		case P3OK_RX:
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)|(IIS_RX_P3OK));
			break;

		case RPUA:	/* rx page unavailable */
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)|(IIS_RX_PAGEUNAVA));
			break;
	
		case RFFU:	/* rx fifo full */
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)|(IIS_RX_FIFO_FULL));
			break;

		default:
			printk("enable channel %d IMR type error!\n", chid);
			break;
	}

#endif

	//printk("IIS_IMR %X", rtlRegRead(IIS_RX_IMR));
}


static void iis_imr_disable(unsigned int chid, unsigned char type)
{
#if defined(CONFIG_SND_RTL819XD_SOC_ALC5628)
	switch(type)
	{
		case P0OK_TX:
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)& (~(IIS_TX_P0OK)));
			break;
	
		case P1OK_TX:
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)& (~(IIS_TX_P1OK)));
			break;
	
		case P2OK_TX:
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)& (~(IIS_TX_P2OK)));
			break;
	
		case P3OK_TX:
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)& (~(IIS_TX_P3OK)));
			break;

		case TPUA:	/* tx page unavailable */
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)& (~(IIS_TX_PAGEUNAVA)));
			break;
	
		case TFEM:	/* tx fifo empty */
			rtlRegWrite(IIS_TX_IMR ,rtlRegRead(IIS_TX_IMR)& (~(IIS_TX_FIFO_EMPTY)));
			break;

		case P0OK_RX:
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)& (~(IIS_RX_P0OK)));
			break;
	
		case P1OK_RX:
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)& (~(IIS_RX_P1OK)));
			break;
	
		case P2OK_RX:
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)& (~(IIS_RX_P2OK)));
			break;
	
		case P3OK_RX:
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)& (~(IIS_RX_P3OK)));
			break;

		case RPUA:	/* rx page unavailable */
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)& (~(IIS_RX_PAGEUNAVA)));
			break;
	
		case RFFU:	/* rx fifo full */
			rtlRegWrite(IIS_RX_IMR ,rtlRegRead(IIS_RX_IMR)& (~(IIS_RX_FIFO_FULL)));
			break;

		default:
			printk("disable channel %d IMR type error!\n", chid);
			break;
	}
#endif /* (CONFIG_SND_RTL819XD_SOC_ALC5628) */
}

static void EnaIisIntr(unsigned int chid)
{
	iis_isr_reset(chid);
	iis_imr_enable(chid, P0OK_TX);
	iis_imr_enable(chid, P1OK_TX);
	iis_imr_enable(chid, P2OK_TX);
	iis_imr_enable(chid, P3OK_TX);
	iis_imr_enable(chid, P0OK_RX);
	iis_imr_enable(chid, P1OK_RX);
	iis_imr_enable(chid, P2OK_RX);
	iis_imr_enable(chid, P3OK_RX);
	iis_imr_enable(chid, TPUA);
	iis_imr_enable(chid, RPUA);
	//iis_imr_enable(chid, TFEM);
	//iis_imr_enable(chid, RFFU);
}

static void DisIisIntr(unsigned int chid)
{
	iis_imr_disable(chid, P0OK_TX);
	iis_imr_disable(chid, P1OK_TX);
	iis_imr_disable(chid, P2OK_TX);
	iis_imr_disable(chid, P3OK_TX);
	iis_imr_disable(chid, P0OK_RX);
	iis_imr_disable(chid, P1OK_RX);
	iis_imr_disable(chid, P2OK_RX);
	iis_imr_disable(chid, P3OK_RX);
	iis_imr_disable(chid, TPUA);
	iis_imr_disable(chid, RPUA);
	iis_imr_disable(chid, TFEM);
	iis_imr_disable(chid, RFFU);

	iis_isr_reset(chid);
}

int16_t sinus(int32_t x)
{
	int16_t i;
	int32_t x2;												// Q15
	int32_t q;
	int32_t res=0;
	int16_t coef[5] = { (int16_t)0x3240, (int16_t)0x0054, (int16_t)0xaacc,
					   (int16_t)0x08B7, (int16_t)0x1cce };	// Q12
	if (x > 0x00008000L)
		x2 = x - 0x00008000L;
	else
		x2 = x;

	if (x2 > 0x00004000L)
		x2 = 0x00008000L - x2;
	q = x2;


	for (i=0; i<5; i++)
	{
		res += coef[i]*x2;											// Q27
		x2 *= q;													// Q30
		x2 >>= 15;													// Q15
	}

	res >>= 12;	 /* back to 0x0000-0xFFFF */						// Q15
	if (x > 0x00008000L)
		res = -res;
	if (res > 0 && res > 32767)
		res = 32767;
	else
		if (res < 0 && res < -32768)
			res = -32768;

	return (int16_t)res;
}

int32_t iis_allchannel=2;
int32_t tone_phase=0;

int32_t tone_phase_ad;

int32_t play_channel=0;
int32_t play_channel_now=0;

static int sample_count;
void tone_gens(int32_t sample_num, int16_t *buffPtr)
{
	int32_t i;

	tone_phase_ad = (1014 * 16777) >> 11;		// 65535/8000 in Q11

	if (play_channel>=iis_allchannel) {
		if (iis_allchannel==1)
			play_channel_now=0;
		else
			play_channel_now=play_channel%iis_allchannel;
	} else
		play_channel_now = play_channel;

	for(i=0;i<sample_num;i++)
	{

		if (sample_count==play_channel_now) {
			*buffPtr=sinus(tone_phase)>>2;
			tone_phase += tone_phase_ad;
			if (tone_phase > 65535)
				tone_phase -= 65535;
		} else {
			*buffPtr=sinus(tone_phase)>>2;
		}
		sample_count++;
		if (sample_count==iis_allchannel)
			sample_count=0;
		buffPtr++;
	}
}


static void iis_ISR(unsigned int iis_txisr, unsigned int iis_rxisr)
{
	unsigned int bch = 0;
	unsigned int i, j;


	for (i=0; i < IIS_PAGE_NUM; i++) // page0/page1/page2/page3
	{

		if ( iis_txisr & IISChanTxPage[iis_txpage[0]] )
		{
			//uint32* txbuf = &pTxBuf[bch][txpage[bch]*(pcm_get_page_size(bch)>>2)];
			//uint32* txbuf = &piis_TxBuf[bch][iis_txpage[bch]*(iis_get_page_size(bch)>>2)];

			iis_set_tx_own_bit(iis_txpage[0]);
			iis_txisr &= ~IISChanTxPage[iis_txpage[0]];
			iis_txpage[0] = (iis_txpage[0] +1 ) % IIS_PAGE_NUM;

			//iis_tr_cnt[0]++;
		} // end of tx

		if ( iis_rxisr & IISChanRxPage[iis_rxpage[0]] ) {
			
			// iis_set_rx_own_bit ASAP helps a lot!

	
			iis_set_rx_own_bit(iis_rxpage[0]);
			iis_rxisr &= ~IISChanRxPage[iis_rxpage[0]];
			iis_rxpage[0] = (iis_rxpage[0]+1) % IIS_PAGE_NUM;

		} // end of for j
		
	} // end of for i

#if 1 
	if ((iis_rxisr != 0) | (iis_txisr != 0))
		printk(" iis_txisr = %X, iis_rxisr = %X ", iis_txisr, iis_rxisr);
#endif	



	return;
}

static irq_handler_t iis_interrupt(int irq, void *dev_instance, struct pt_regs *regs)
{
	unsigned int status_val_tx;
	unsigned int status_val_rx;

#if 0
	if ((status_val_tx = rtlRegRead(IIS_TX_ISR)) | (status_val_rx = rtlRegRead(IIS_RX_ISR))) {
	
		rtlRegWrite(IIS_TX_ISR, status_val_tx);
		rtlRegWrite(IIS_RX_ISR, status_val_rx);

		memset(&piis_TxBuf[0][0*iis_get_page_size(chid)>>2], 0xaa, iis_get_page_size(chid)*2);	
#if 0
		int k;
		for (k=0; k<160; k++)
		{
			*(((short*)&piis_TxBuf[0][0*iis_get_page_size(chid)>>2])+k) = Sin1KHz[k%8];
		}
#endif
			iis_set_tx_own_bit(0);
			iis_set_tx_own_bit(1);
			iis_set_rx_own_bit(0);
			iis_set_rx_own_bit(1);
		iis_twiddle();
	}
#endif
	//printk("IISa");


#ifdef CHECK_IIS_ISR_AGAIN
	//int iis_isr_cnt = 0;
	while ((status_val_tx = rtlRegRead(IIS_TX_ISR)) | (status_val_rx = rtlRegRead(IIS_RX_ISR)))
#else
	if ((status_val_tx = rtlRegRead(IIS_TX_ISR)) | (status_val_rx = rtlRegRead(IIS_RX_ISR)))
#endif	
	{
		rtlRegWrite(IIS_TX_ISR, status_val_tx);
		rtlRegWrite(IIS_RX_ISR, status_val_rx);

		if ((status_val_tx & 0x0F) | (status_val_rx & 0x0F))	// TOK and ROK only
			iis_ISR(status_val_tx & 0x0F, status_val_rx & 0x0F);

		if ( (status_val_tx & 0x30) || (status_val_rx & 0x30)) // Buffer/Fifo Unavailable only
		{
			if (status_val_tx & 0x10)
				printk("TBU ");
			if (status_val_rx & 0x10)
				printk("RBU ");
			//if (status_val_tx & 0x20)
			//	printk("TFU ");
			//if (status_val_rx & 0x20)
			//	printk("RFU ");
			printk("\n");
		}
	}


    return IRQ_HANDLED;
}



static void rtl8197d_snd_txctrl(int on)
{

}

static void rtl8197d_snd_rxctrl(int on)
{

}
#endif

/*
 * Wait for the LR signal to allow synchronisation to the L/R clock
 * from the codec. May only be needed for slave mode.
 */
static int rtl8197d_snd_lrsync(void)
{
	return 0;
}

/*
 * Check whether CPU is the master or slave
 */
static inline int rtl8197d_snd_is_clkmaster(void)
{
	return 1; // 8197d alway master
}

/*
 * Set RTL8197D I2S DAI format
 */
static int rtl8197d_i2s_set_fmt(struct snd_soc_dai *cpu_dai,
		unsigned int fmt)
{
	u32 iiscr;

	iiscr = rtlRegRead(IISCR);
	switch (fmt & SND_SOC_DAIFMT_FORMAT_MASK) {
	case SND_SOC_DAIFMT_I2S:
		iiscr &= ~IIS_FORMAT_I2S;
		break;
	default:
		return -EINVAL;
	}
	
	//pr_info("hw_params w: IISCR: %x \n", iiscr);
	rtlRegWrite(IISCR, iiscr);
	return 0;
}

static int rtl8197d_i2s_startup(struct snd_pcm_substream *substream,
			     struct snd_soc_dai *dai)
{
	return 0;
}

static int rtl8197d_i2s_hw_params(struct snd_pcm_substream *substream,
				 struct snd_pcm_hw_params *params,
				 struct snd_soc_dai *dai)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	u32 iismod;


	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_S16_BE:
		//panic_printk("\nSet SNDRV_PCM_FORMAT_S16_BE\n");
		break;
	case SNDRV_PCM_FORMAT_S16_LE:
		//panic_printk("\nSet SNDRV_PCM_FORMAT_S16_LE\n");
		break;
	case SNDRV_PCM_FORMAT_S24_BE:
		break;
	}
	return 0;
}

static int rtl8197d_i2s_trigger(struct snd_pcm_substream *substream, int cmd,
			       struct snd_soc_dai *dai)
{
	int ret = 0;
	
	return ret;
}

/*
 * Set S3C24xx Clock source
 */
static int rtl8197d_i2s_set_sysclk(struct snd_soc_dai *cpu_dai,
	int clk_id, unsigned int freq, int dir)
{

	return 0;
}

/*
 * Set S3C24xx Clock dividers
 */
static int rtl8197d_i2s_set_clkdiv(struct snd_soc_dai *cpu_dai,
	int div_id, int div)
{

	return 0;
}

static void rtl8197d_i2s_shutdown(struct snd_pcm_substream *substream,
			       struct snd_soc_dai *dai)
{
}

static int rtl8197d_i2s_probe(struct platform_device *pdev,
			     struct snd_soc_dai *dai)
{
	return 0;
}

#define rtl8197d_i2s_suspend NULL
#define rtl8197d_i2s_resume NULL



static struct snd_soc_dai_ops rtl8197d_i2s_dai_ops = {
	.trigger	= rtl8197d_i2s_trigger,
	.hw_params	= rtl8197d_i2s_hw_params,
	.set_fmt	= rtl8197d_i2s_set_fmt,
	.startup	= rtl8197d_i2s_startup,
	.shutdown	= rtl8197d_i2s_shutdown,
	.set_clkdiv	= rtl8197d_i2s_set_clkdiv,
	.set_sysclk	= rtl8197d_i2s_set_sysclk,
};

struct snd_soc_dai rtl8197d_i2s_dai = {
	.name = "rtl8197d-i2s",
	.id = 0,
	.probe = rtl8197d_i2s_probe,
	.suspend = rtl8197d_i2s_suspend,
	.resume = rtl8197d_i2s_resume,
	.playback = {
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE,
		.formats = SNDRV_PCM_FMTBIT_S16_BE,},
	.capture = {
		.channels_min = 1,
		.channels_max = 2,
		.rates = SNDRV_PCM_RATE,
		.formats = SNDRV_PCM_FMTBIT_S16_LE|SNDRV_PCM_FMTBIT_S16_BE,},
	.ops = &rtl8197d_i2s_dai_ops,
};
EXPORT_SYMBOL_GPL(rtl8197d_i2s_dai);


static struct snd_info_entry *snd_info_rtl8197d_i2s_entry;
extern unsigned int serial_in_i2c(unsigned int addr, int offset);

static void snd_info_rtl8197d_i2s_read(struct snd_info_entry *entry, struct snd_info_buffer *buffer)
{
	int temp;
        snd_iprintf(buffer,"snd_info_rtl8197d_i2s_read \n");
        snd_iprintf(buffer, "i2s sampling rate=%d\n", SNDRV_PCM_RATE_NUM);
        snd_iprintf(buffer, "i2s:00=%x, 10=%x, 40=%x\n", rtlRegRead(0xb8000000), rtlRegRead(0xb8000010), rtlRegRead(0xb8000040));
        snd_iprintf(buffer, "    44=%x, 58=%x, %x\n", rtlRegRead(0xb8000044), rtlRegRead(0xb8000058), rtlRegRead(0xb8000040));
        snd_iprintf(buffer, "    3000=%x, 3004=%x, 3014=%x\n", rtlRegRead(0xb8003000), rtlRegRead(0xb8003004), rtlRegRead(0xb8003014));

        snd_iprintf(buffer, "    IISCR=%x, TX_PAGE_PTR=%x, RX_PAGE_PTR=%x\n", rtlRegRead(IISCR), rtlRegRead(TX_PAGE_PTR), rtlRegRead(RX_PAGE_PTR));
        snd_iprintf(buffer, "    IIS_SETTING=%x, IIS_TX_IMR=%x, IIS_TX_ISR=%x\n", rtlRegRead(IIS_SETTING), rtlRegRead(IIS_TX_IMR), rtlRegRead(IIS_TX_ISR));
        snd_iprintf(buffer, "    IIS_TX_P0OWN=%x, IIS_TX_P1OWN=%x, IIS_TX_P2OWN=%x, IIS_TX_P3OWN=%x\n", rtlRegRead(IIS_TX_P0OWN), rtlRegRead(IIS_TX_P1OWN), rtlRegRead(IIS_TX_P2OWN), rtlRegRead(IIS_TX_P3OWN));
        snd_iprintf(buffer, "    IIS_SETTING=%x, IIS_RX_IMR=%x, IIS_RX_ISR=%x\n", rtlRegRead(IIS_SETTING), rtlRegRead(IIS_RX_IMR), rtlRegRead(IIS_RX_ISR));
        snd_iprintf(buffer, "    IIS_RX_P0OWN=%x, IIS_RX_P1OWN=%x, IIS_RX_P2OWN=%x, IIS_RX_P3OWN=%x\n", rtlRegRead(IIS_RX_P0OWN), rtlRegRead(IIS_RX_P1OWN), rtlRegRead(IIS_RX_P2OWN), rtlRegRead(IIS_RX_P3OWN));
#ifdef CONFIG_SND_RTL819XD_SOC_ALC5642
	temp = serial_in_i2c( 0x38, 0xfe);
	snd_iprintf(buffer, "    reg_fe=%x,\n ", temp);
#endif

#ifdef CONFIG_SND_RTL819XD_SOC_ALC5628
	temp = serial_in_i2c( 0x30, 0x0);
	snd_iprintf(buffer, "    reg_0=%x,\n ", temp);
#endif

#ifdef CONFIG_SND_RTL819XD_SOC_ALC5633Q
	temp = serial_in_i2c( 0x38, 0x0);
	snd_iprintf(buffer, "    reg_0=%x,\n ", temp);
#endif	
}

static struct snd_info_entry *snd_info_rtl8197d_i2s_start_entry;

static void snd_info_rtl8197d_i2s_start_read(struct snd_info_entry *entry, struct snd_info_buffer *buffer)
{
        snd_iprintf(buffer,"snd_info_rtl8197d_i2s_start_read \n");
	rtlRegWrite(IISCR,0x80000000 | (0x2) | IIS_ENABLE | (0<<15) );	// 0->1 enable IIS
}

// create proc info in /proc/asound/rtl8197d_i2s
static int snd_info_rtl8197d_i2s_init(void)
{
        struct snd_info_entry *entry;

        entry = snd_info_create_module_entry(THIS_MODULE, "rtl8197d_i2s", NULL);
        if (entry == NULL)
                return -ENOMEM;
        entry->c.text.read = snd_info_rtl8197d_i2s_read;
        if (snd_info_register(entry) < 0) {
                snd_info_free_entry(entry);
                return -ENOMEM;
        }
        snd_info_rtl8197d_i2s_entry = entry;


        entry = snd_info_create_module_entry(THIS_MODULE, "rtl8197d_i2s_start", NULL);
        if (entry == NULL)
                return -ENOMEM;
        entry->c.text.read = snd_info_rtl8197d_i2s_start_read;
        if (snd_info_register(entry) < 0) {
                snd_info_free_entry(entry);
                return -ENOMEM;
        }
        snd_info_rtl8197d_i2s_start_entry = entry;

        return 0;
}




static int __init rtl8197d_i2s_init(void)
{
	int i, j;


	rtlRegMask(0xb8003014, 0x00000F00, 0x00000200);//route iis interrupt
	rtlRegMask(0xb8000010, 0x03DCB000, 0x01DCB000);//enable iis controller clock
	rtlRegMask(0xb8000058, 0x00000001, 0x00000001);//enable 24p576mHz clock

	/* Configure the I2S pins in correct mode */
	// set the jtag as iis-audio
#if defined(CONFIG_RTL_8881A)
	// set the led-sig0 as iis-sd1_out
	rtlRegMask(0xb8000044, 0x0000FFF9, 0x000000D9);
#else
	rtlRegMask(0xb8000040, 0x00000007, 0x00000003);//change pin mux to iis-voice pin
	// set the led-sig0 as iis-sd1_out
	rtlRegMask(0xb8000044, 0x00000003, 0x00000001);//change pin mux to iis-voice pin
#endif	
#if defined(CONFIG_RTL_8881A)
		printk("PABCD_CNR:\t%08X\nPABCD_DIR:\t%08X\nPABCD_DAT:\t%08X\n"
				"PIN_MUX_SEL:\t%08X\nPIN_MUX_SEL_2:\t%08X\nPIN_MUX_SEL_3:\t%08X\n",
					rtlRegRead(0xB8003500), rtlRegRead(0xB8003508),
					rtlRegRead(0xB800350C), rtlRegRead(0xB8000040),
					rtlRegRead(0xB8000044),rtlRegRead(0xB800004C));
#else
		printk("PABCD_CNR:\t%08X\nPABCD_DIR:\t%08X\nPABCD_DAT:\t%08X\n"
				"PIN_MUX_SEL:\t%08X\nPIN_MUX_SEL_2:\t%08X\n",
					rtlRegRead(0xB8003500), rtlRegRead(0xB8003508),
					rtlRegRead(0xB800350C), rtlRegRead(0xB8000040),
					rtlRegRead(0xB8000044));
#endif	

#if defined(CONFIG_RTL_8881A)
	rtlRegWrite(0xB800004C, ((rtlRegRead(0xB800004C) & ~0xFFFFFF) | 0x33333));
	printk("PABCD_CNR:\t%08X\nPABCD_DIR:\t%08X\nPABCD_DAT:\t%08X\n"
					"PIN_MUX_SEL:\t%08X\nPIN_MUX_SEL_2:\t%08X\nPIN_MUX_SEL_3:\t%08X\n",
						rtlRegRead(0xB8003500), rtlRegRead(0xB8003508),
						rtlRegRead(0xB800350C), rtlRegRead(0xB8000040),
						rtlRegRead(0xB8000044),rtlRegRead(0xB800004C));
#else
	rtlRegWrite(0xB8000044, ((rtlRegRead(0xB8000044) & ~0x3FFF9) | 0x100D9));
	printk("PABCD_CNR:\t%08X\nPABCD_DIR:\t%08X\nPABCD_DAT:\t%08X\n"
					"PIN_MUX_SEL:\t%08X\nPIN_MUX_SEL_2:\t%08X\n",
						rtlRegRead(0xB8003500), rtlRegRead(0xB8003508),
						rtlRegRead(0xB800350C), rtlRegRead(0xB8000040),
						rtlRegRead(0xB8000044));
#endif
	
	
	
	

	rtlRegWrite(IISCR, 0x80000000);	// stop IIS
for (j=0;j<5000;j++);
	rtlRegWrite(IISCR, 0x0000);	// stop IIS
for (j=0;j<5000;j++);
	rtlRegWrite(IISCR, 0x80000000);
for (j=0;j<5000;j++);
	/* clear tx rx isr status */
#if 0 //For debug 
	tone_gens((PAGE_SIZE*IIS_PAGE_NUM*2), ((unsigned long)iis_tx_buf)|0x20000000);
	
		printk("iis tx page: \n");
		for (i=0;i<(5*IIS_PAGE_NUM)*2+8;i++) {
			if ((i%8) == 7)
				printk(" %x\n",iis_tx_buf[i]);
			else
				printk(" %x ",iis_tx_buf[i]);
		}
#endif		
	rtlRegWrite(IIS_TX_ISR, IIS_TX_P0OK | IIS_TX_P1OK | IIS_TX_P2OK | IIS_TX_P3OK | IIS_TX_PAGEUNAVA | IIS_TX_FIFO_EMPTY);
	rtlRegWrite(IIS_RX_ISR, IIS_RX_P0OK | IIS_RX_P1OK | IIS_RX_P2OK | IIS_RX_P3OK | IIS_RX_PAGEUNAVA | IIS_RX_FIFO_FULL);

	// allocate buffer address
	rtlRegWrite(TX_PAGE_PTR,(unsigned int)iis_tx_buf & 0xfffffff);

	rtlRegWrite(IIS_TX_P0OWN,BIT(31));
	rtlRegWrite(IIS_TX_P1OWN,BIT(31));
	rtlRegWrite(IIS_TX_P2OWN,BIT(31));
	rtlRegWrite(IIS_TX_P3OWN,BIT(31));

	//printf("enable IIS  interrupt\n");
	rtlRegWrite(IIS_TX_IMR, 0x0f);
	rtlRegWrite(IIS_RX_IMR, 0x0f);
	rtlRegWrite(IIS_SETTING, (PAGE_SIZE - 1) | ((IIS_PAGE_NUM-1)<<12) | (1<<14));	//set page size


	snd_info_rtl8197d_i2s_init();



	return snd_soc_register_dai(&rtl8197d_i2s_dai);
}
module_init(rtl8197d_i2s_init);

static void __exit rtl8197d_i2s_exit(void)
{
	//pr_info("Entered %s\n", __func__);
	snd_info_free_entry(snd_info_rtl8197d_i2s_entry);
	snd_info_free_entry(snd_info_rtl8197d_i2s_start_entry);
	snd_soc_unregister_dai(&rtl8197d_i2s_dai);
}
module_exit(rtl8197d_i2s_exit);

/* Module information */
MODULE_AUTHOR("XU JUNWEI, <jwsyu@realtek.com>");
MODULE_DESCRIPTION("Realtek I2S DMA module");
MODULE_LICENSE("GP1");

