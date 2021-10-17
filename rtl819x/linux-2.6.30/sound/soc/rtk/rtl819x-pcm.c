/*
 * rtl819x-pcm.c  --  ALSA Soc Audio Layer
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
#include <linux/module.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/dma-mapping.h>

#include <sound/core.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>
#include <sound/soc.h>

#include <asm/dma.h>
//#include <mach/hardware.h>
//#include <mach/dma.h>
//#include <plat/audio.h>

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
#include <linux/swab.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
#include "bspchip.h"
#define IIS_IRQ BSP_I2S_IRQ
  #else
#define IIS_IRQ	26
  #endif

#include "rtl819x-pcm.h"
#include "rtl8197d-i2s.h"
extern int NeedSwap;
#define IIS_PAGE_NUM	4

#define IIS_FIRST_TX	1
#define IIS_FIRST_RX	2

extern int iis_counter;
int iis_first_start;
int iis_channels;

//1920=10ms,
//#define BUFFER_BYTES_MAX (6400*2*4)
//#define BUFFER_BYTES_MAX ((1920*8)/10)*4
#define BUFFER_BYTES_MAX ((1920*40)/10)*4

static const struct snd_pcm_hardware rtl819x_pcm_hardware = {
	.info			= SNDRV_PCM_INFO_INTERLEAVED |
				    SNDRV_PCM_INFO_BLOCK_TRANSFER |
				    SNDRV_PCM_INFO_MMAP |
				    SNDRV_PCM_INFO_MMAP_VALID,
	.formats		= SNDRV_PCM_FMTBIT_S16_BE,
	.rates = (SNDRV_PCM_RATE),
	.rate_min = SNDRV_PCM_RATE_NUM,
	.rate_max = SNDRV_PCM_RATE_NUM,
/*
	.rates = (SNDRV_PCM_RATE_8000 | SNDRV_PCM_RATE_16000 |
		  SNDRV_PCM_RATE_32000 | SNDRV_PCM_RATE_48000),
	.rate_min = 8000,
	.rate_max = 96000,
*/
	.channels_min		= 1,
	.channels_max		= 2,
//	.buffer_bytes_max	= (6400*2*4)*8,//128*1024,
//	.period_bytes_min	= (6400)*8,
//	.period_bytes_max	= (6400*2)*8,
	.buffer_bytes_max	= BUFFER_BYTES_MAX,//128*1024,
	.period_bytes_min	= (BUFFER_BYTES_MAX/16),
	.period_bytes_max	= (BUFFER_BYTES_MAX/2),
	.periods_min		= 2,
	.periods_max		= 4,
	.fifo_size		= 0,
};
#if 0
struct rtl819x_runtime_data {
	spinlock_t lock;
	int state;
	unsigned int dma_loaded;
	unsigned int dma_limit;
	unsigned int dma_period;
	dma_addr_t dma_start;
	dma_addr_t dma_pos;
	dma_addr_t dma_end;
	struct rtl819x_pcm_dma_params *params;
};
#endif
struct rtl819x_runtime_data {
	spinlock_t lock;
	int state;
	unsigned int dma_loaded;
	unsigned int dma_limit;
	unsigned int dma_period;
	dma_addr_t dma_start;
	dma_addr_t dma_pos;
	dma_addr_t dma_end;
	struct rtl819x_pcm_dma_params *params;
};

struct snd_pcm_substream *tx_substream;
struct snd_pcm_substream *rx_substream;

static unsigned long IISChanTxPage[4] = {IIS_TX_P0OK, IIS_TX_P1OK, IIS_TX_P2OK, IIS_TX_P3OK};
static unsigned long IISChanRxPage[4] = {IIS_RX_P0OK, IIS_RX_P1OK, IIS_RX_P2OK, IIS_RX_P3OK};

static int iis_txpage[2];
static int iis_rxpage[2];

static short iis_tx_data[BUFFER_BYTES_MAX/2+32] __attribute__((aligned(4096)));
static short iis_rx_data[BUFFER_BYTES_MAX/2+32] __attribute__((aligned(4096)));


static void rtl819x_pcm_enqueue(struct snd_pcm_substream *substream);
/* rtl819x i2s controller code */
static void rtl819x_i2s_trx_start(void)
{

	if (iis_channels == 1) {
		rtlRegWrite(IISCR,0x80000000 | (0x14) | 0 | (0<<15) );	// 0->1 enable IIS
		rtlRegWrite(IISCR,0x80000000 | (0x14) | IIS_ENABLE | (0<<15) );	// 0->1 enable IIS
	} else {
		rtlRegWrite(IISCR,0x80000000 | (0x4) | 0 | (0<<15) );	// 0->1 enable IIS
		rtlRegWrite(IISCR,0x80000000 | (0x4) | IIS_ENABLE | (0<<15) );	// 0->1 enable IIS
	}

}
static void rtl819x_i2s_tx_start(void)
{

	//panic_printk("Entered %s\n", __func__);
	if (iis_channels == 1) {
		rtlRegWrite(IISCR,0x80000000 | (0x12) | 0 | (0<<15) );	// 0->1 enable IIS
		rtlRegWrite(IISCR,0x80000000 | (0x12) | IIS_ENABLE | (0<<15) );	// 0->1 enable IIS
	} else {
		rtlRegWrite(IISCR,0x80000000 | (0x2) | 0 | (0<<15) );	// 0->1 enable IIS
		rtlRegWrite(IISCR,0x80000000 | (0x2) | IIS_ENABLE | (0<<15) );	// 0->1 enable IIS
	}


}

static void rtl819x_i2s_rx_start(void)
{

}

static void rtl819x_i2s_trx_stop(void)
{
	int j;
	rtlRegWrite(IISCR, 0x80000000);	// stop IIS
for (j=0;j<5000;j++);
	rtlRegWrite(IISCR, 0x0000);	// stop IIS
for (j=0;j<5000;j++);
	rtlRegWrite(IISCR, 0x80000000);
for (j=0;j<5000;j++);
	/* clear tx rx isr status */
	rtlRegWrite(IIS_TX_ISR, IIS_TX_P0OK | IIS_TX_P1OK | IIS_TX_P2OK | IIS_TX_P3OK | IIS_TX_PAGEUNAVA | IIS_TX_FIFO_EMPTY);
	rtlRegWrite(IIS_RX_ISR, IIS_RX_P0OK | IIS_RX_P1OK | IIS_RX_P2OK | IIS_RX_P3OK | IIS_RX_PAGEUNAVA | IIS_RX_FIFO_FULL);
	iis_txpage[0]=0;
	iis_rxpage[0]=0;
}
static void rtl819x_i2s_tx_stop(void)
{
	int j;
	rtlRegWrite(IISCR, 0x80000000);	// stop IIS
for (j=0;j<5000;j++);
	rtlRegWrite(IISCR, 0x0000);	// stop IIS
for (j=0;j<5000;j++);
	rtlRegWrite(IISCR, 0x80000000);
for (j=0;j<5000;j++);
	/* clear tx rx isr status */
	rtlRegWrite(IIS_TX_ISR, IIS_TX_P0OK | IIS_TX_P1OK | IIS_TX_P2OK | IIS_TX_P3OK | IIS_TX_PAGEUNAVA | IIS_TX_FIFO_EMPTY);
	rtlRegWrite(IIS_RX_ISR, IIS_RX_P0OK | IIS_RX_P1OK | IIS_RX_P2OK | IIS_RX_P3OK | IIS_RX_PAGEUNAVA | IIS_RX_FIFO_FULL);
	iis_txpage[0]=0;
}

static void rtl819x_i2s_rx_stop(void)
{

}

static void iis_ISR(unsigned int iis_txisr, unsigned int iis_rxisr)
{
	unsigned int bch = 0;
	unsigned int i, j;
	struct rtl819x_runtime_data *prtd;


	for (i=0; i < IIS_PAGE_NUM; i++) // page0/page1/page2/page3
	{

		if ( iis_txisr & IISChanTxPage[iis_txpage[0]] )
		{
			//uint32* txbuf = &pTxBuf[bch][txpage[bch]*(pcm_get_page_size(bch)>>2)];
			//uint32* txbuf = &piis_TxBuf[bch][iis_txpage[bch]*(iis_get_page_size(bch)>>2)];

			//iis_set_tx_own_bit(iis_txpage[0]);
			//iis_txisr &= ~IISChanTxPage[iis_txpage[0]];
			iis_txpage[0] = (iis_txpage[0] +1 ) % IIS_PAGE_NUM;
			if(tx_substream) {
				snd_pcm_period_elapsed(tx_substream);
				//if (prtd->state & ST_RUNNING) {
				if(tx_substream) {
					prtd=tx_substream->runtime->private_data;
					prtd->dma_loaded--;
					rtl819x_pcm_enqueue(tx_substream);
				}
				//}
			}
			//iis_tr_cnt[0]++;
		} // end of tx

		if ( iis_rxisr & IISChanRxPage[iis_rxpage[0]] )
		{
			// iis_set_rx_own_bit ASAP helps a lot!

			//iis_set_rx_own_bit(iis_rxpage[0]);
			//iis_rxisr &= ~IISChanRxPage[iis_rxpage[0]];
			iis_rxpage[0] = (iis_rxpage[0]+1) % IIS_PAGE_NUM;
			if(rx_substream) {
				snd_pcm_period_elapsed(rx_substream);
				//if (prtd->state & ST_RUNNING) {
				if(rx_substream) {
					prtd=rx_substream->runtime->private_data;
					prtd->dma_loaded--;
					rtl819x_pcm_enqueue(rx_substream);
				}
			}
			//iis_tr_cnt[0]--;
		} // end of rx		
	} // end of for i

#if 0
	if ((iis_rxisr != 0) | (iis_txisr != 0))
		printk(" iis_txisr = %X, iis_rxisr = %X ", iis_txisr, iis_rxisr);
#endif	



	return;
}

static irq_handler_t iis_dma_interrupt(int irq, void *dev_instance, struct pt_regs *regs)
{
	unsigned int status_val_tx;
	unsigned int status_val_rx;


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
#if 0
			if (status_val_tx & 0x10)
				printk("TBU ");
			if (status_val_rx & 0x10)
				printk("RBU ");

			//if (status_val_tx & 0x20)
			//	printk("TFU ");
			//if (status_val_rx & 0x20)
			//	printk("RFU ");
			printk("\n");
#endif
		}
	}

    return IRQ_HANDLED;
}


/* rtl819x_pcm_enqueue
 *
 * place a dma buffer onto the queue for the dma system
 * to handle.
*/
void rtl819x_pcm_enqueue(struct snd_pcm_substream *substream)
{
	struct rtl819x_runtime_data *prtd = substream->runtime->private_data;
	dma_addr_t pos = prtd->dma_pos;
	dma_addr_t start = prtd->dma_start;
	int ret;
	int dma_page;
	unsigned int iis_p0own;

	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK)
		iis_p0own=IIS_TX_P0OWN;
	else
		iis_p0own=IIS_RX_P0OWN;

	while (prtd->dma_loaded < prtd->dma_limit) {
		unsigned long len = prtd->dma_period;


		if ((pos + len) > prtd->dma_end) {
			len  = prtd->dma_end - pos;
			pr_info(KERN_INFO "%s: corrected dma len %ld\n",__func__, len);
		}

		dma_page=(pos-start)/prtd->dma_period;
		if ((rtlRegRead(iis_p0own + 4*dma_page))==0) {//page own by cpu
		
			rtlRegWrite(iis_p0own + 4*dma_page, BIT(31));
			prtd->dma_loaded++;
			pos += prtd->dma_period;
			if (pos >= prtd->dma_end)
				pos = prtd->dma_start;
		} else{
			break;
		}
	}

	prtd->dma_pos = pos;
}
static int rtl819x_pcm_hw_params(struct snd_pcm_substream *substream,
	struct snd_pcm_hw_params *params)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct rtl819x_runtime_data *prtd = runtime->private_data;
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct rtl819x_pcm_dma_params *dma = rtd->dai->cpu_dai->dma_data;
	unsigned long totbytes = params_buffer_bytes(params);
	int ret = 0;


	if (prtd->params == NULL) {
		/* prepare DMA */
		prtd->params = dma;

		if (ret < 0) {
			printk(KERN_ERR "failed to get dma channel\n");
			return ret;
		}
	}
	iis_channels = params_channels(params);
	//printk("iis_channels=%d\n"); //debug message
	snd_pcm_set_runtime_buffer(substream, &substream->dma_buffer);

	runtime->dma_bytes = totbytes;

	spin_lock_irq(&prtd->lock);
	prtd->dma_loaded = 0;
	prtd->dma_limit = runtime->hw.periods_min;
	prtd->dma_period = params_period_bytes(params);
	prtd->dma_start = runtime->dma_addr;
	prtd->dma_pos = prtd->dma_start;
	prtd->dma_end = prtd->dma_start + totbytes;
	spin_unlock_irq(&prtd->lock);

	return 0;
}

static int rtl819x_pcm_hw_free(struct snd_pcm_substream *substream)
{
	struct rtl819x_runtime_data *prtd = substream->runtime->private_data;


	/* TODO - do we need to ensure DMA flushed */
	snd_pcm_set_runtime_buffer(substream, NULL);
#if 0
	if (prtd->params) {
		rtl819x_dma_free(prtd->params->channel, prtd->params->client);
		prtd->params = NULL;
	}
#endif
	return 0;
}

static int rtl819x_pcm_prepare(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct rtl819x_runtime_data *prtd = substream->runtime->private_data;
	int period_bytes = frames_to_bytes(runtime, runtime->period_size);
	int ret = 0;
	int iis_page_size;
	int iis_page_number;

	/* channel needs configuring for mem=>device, increment memory addr,
	 * sync to pclk, half-word transfers to the IIS-FIFO. */
	if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
		rtlRegWrite(TX_PAGE_PTR,(unsigned int)(runtime->dma_area) & 0xfffffff);
		iis_page_size=period_bytes;
		iis_page_number=runtime->periods;
		//rtlRegWrite();
		//rtlRegWrite(IIS_SETTING, (iis_page_size - 1) | ((iis_page_number/4-1)<<12) | (1<<14));	//set page size
		rtlRegWrite(IIS_SETTING, (iis_page_size/4 - 1) | ((iis_page_number-1)<<12) | (RTL819XD_SOC_I2S_SR<<14));	//set page size

		rtlRegWrite(IIS_TX_P0OWN,0);
		rtlRegWrite(IIS_TX_P1OWN,0);
		rtlRegWrite(IIS_TX_P2OWN,0);
		rtlRegWrite(IIS_TX_P3OWN,0);
	} else {
		rtlRegWrite(RX_PAGE_PTR,(unsigned int)(runtime->dma_area) & 0xfffffff);
		iis_page_size=period_bytes;
		iis_page_number=runtime->periods;
		rtlRegWrite(IIS_SETTING, (iis_page_size/4 - 1) | ((iis_page_number-1)<<12) | (RTL819XD_SOC_I2S_SR<<14));	//set page size

		rtlRegWrite(IIS_RX_P0OWN,0);
		rtlRegWrite(IIS_RX_P1OWN,0);
		rtlRegWrite(IIS_RX_P2OWN,0);
		rtlRegWrite(IIS_RX_P3OWN,0);
	}


		/* flush the DMA channel */
		prtd->dma_loaded = 0;
		prtd->dma_pos = prtd->dma_start;
	/* enqueue dma buffers */
	//rtl819x_pcm_enqueue(substream);

	return ret;
}

static int rtl819x_pcm_trigger(struct snd_pcm_substream *substream, int cmd)
{
	struct rtl819x_runtime_data *prtd = substream->runtime->private_data;
	int ret = 0;

	spin_lock(&prtd->lock);

	switch (cmd) {
	case SNDRV_PCM_TRIGGER_START:
		prtd->state |= ST_RUNNING;
		/*
		 * TX and RX are not independent,they are enabled at the
		 * same time, even if only one side is running. So, we
		 * need to configure both of them at the time when the first
		 * stream is opened.
		 *
		 * CPU DAI:slave mode.
		 */
		iis_counter ++;
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			rtl819x_pcm_enqueue(substream);
			tx_substream = substream;
			//rtl819x_i2s_tx_start();
			
		}else {
			rtl819x_pcm_enqueue(substream);
			rx_substream = substream;
			//rtl819x_i2s_rx_start();
		}
		if (iis_counter == 1) {
			if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
				iis_first_start = IIS_FIRST_TX;
				memset( &iis_rx_data[0], 0, BUFFER_BYTES_MAX);
				rtlRegWrite(IIS_RX_P0OWN, 0);
				rtlRegWrite(IIS_RX_P1OWN, 0);
				rtlRegWrite(IIS_RX_P2OWN, 0);
				rtlRegWrite(IIS_RX_P3OWN, 0);
				rtlRegWrite(RX_PAGE_PTR, (unsigned int)&iis_rx_data[0] & 0xfffffff);
			} else {
				iis_first_start = IIS_FIRST_RX;
				memset( &iis_tx_data[0], 0, BUFFER_BYTES_MAX);
				rtlRegWrite(IIS_TX_P0OWN, 0);
				rtlRegWrite(IIS_TX_P1OWN, 0);
				rtlRegWrite(IIS_TX_P2OWN, 0);
				rtlRegWrite(IIS_TX_P3OWN, 0);
				rtlRegWrite(TX_PAGE_PTR, (unsigned int)&iis_tx_data[0] & 0xfffffff);
			}
			rtl819x_i2s_trx_start();

		}
		break;

	case SNDRV_PCM_TRIGGER_STOP:
		iis_counter --;
	//case SNDRV_PCM_TRIGGER_SUSPEND:
	//case SNDRV_PCM_TRIGGER_PAUSE_PUSH:
		if (substream->stream == SNDRV_PCM_STREAM_PLAYBACK) {
			tx_substream = NULL;
			//rtl819x_i2s_tx_stop();
		} else{
			rx_substream = NULL;
			//rtl819x_i2s_rx_stop();
		}
		if (iis_counter == 0) {
			rtl819x_i2s_trx_stop();
		}

		break;

	default:
		ret = -EINVAL;
		break;
	}

	spin_unlock(&prtd->lock);

	return ret;
}

static snd_pcm_uframes_t
rtl819x_pcm_pointer(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct rtl819x_runtime_data *prtd = runtime->private_data;
	unsigned long res;
	dma_addr_t src, dst;



	if (substream->stream == SNDRV_PCM_STREAM_CAPTURE)
		res = iis_rxpage[0]*prtd->dma_period;
	else
		res = iis_txpage[0]*prtd->dma_period;



	/* we seem to be getting the odd error from the pcm library due
	 * to out-of-bounds pointers. this is maybe due to the dma engine
	 * not having loaded the new values for the channel before being
	 * callled... (todo - fix )
	 */

	if (res >= snd_pcm_lib_buffer_bytes(substream)) {
		if (res == snd_pcm_lib_buffer_bytes(substream))
			res = 0;
	}

	return bytes_to_frames(substream->runtime, res);
}

static int rtl819x_pcm_open(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct rtl819x_runtime_data *prtd;


	snd_soc_set_runtime_hwparams(substream, &rtl819x_pcm_hardware);

	prtd = kzalloc(sizeof(struct rtl819x_runtime_data), GFP_KERNEL);
	if (prtd == NULL)
		return -ENOMEM;

	spin_lock_init(&prtd->lock);

	runtime->private_data = prtd;
	return 0;
}

static int rtl819x_pcm_close(struct snd_pcm_substream *substream)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	struct rtl819x_runtime_data *prtd = runtime->private_data;


	if (!prtd){
		pr_info("rtl819x_pcm_close called with prtd == NULL\n");
	}
	else{
		kfree(prtd);
	}
	

	return 0;
}

static int rtl819x_pcm_mmap(struct snd_pcm_substream *substream,
	struct vm_area_struct *vma)
{
	struct snd_pcm_runtime *runtime = substream->runtime;
	unsigned long physical = ( ( unsigned long )runtime->dma_area )&0xfffffff;
	unsigned long prot;
	unsigned long user_size, kern_size;
#if 0
	printk( "runtime->dma_area:%08X\n", runtime->dma_area );
	
	printk( "start:%p,end:%p,physical:%p,prot:%X\n", vma->vm_start, vma->vm_end, physical, vma->vm_page_prot );
#endif
	//set uncache
	prot = pgprot_val(vma->vm_page_prot);
	prot = (prot & ~_CACHE_MASK) | _CACHE_UNCACHED;
	vma->vm_page_prot = __pgprot(prot);
	
	user_size = vma->vm_end - vma->vm_start;

#if 0

vma->vm_pgoff = physical>> PAGE_SHIFT;
#endif
	//vma->vm_start = (unsigned long)runtime->dma_area;
	//vma->vm_end = vma->vm_start + size;
	//vma->vm_flags |=  VM_SHARED;
	remap_pfn_range( vma, vma->vm_start, physical>> PAGE_SHIFT,
	                 user_size,  vma->vm_page_prot );


	return 0;
}

static struct snd_pcm_ops rtl819x_pcm_ops = {
	.open		= rtl819x_pcm_open,
	.close		= rtl819x_pcm_close,
	.ioctl		= snd_pcm_lib_ioctl,
	.hw_params	= rtl819x_pcm_hw_params,
	.hw_free	= rtl819x_pcm_hw_free,
	.prepare	= rtl819x_pcm_prepare,
	.trigger	= rtl819x_pcm_trigger,
	.pointer	= rtl819x_pcm_pointer,
	.mmap		= rtl819x_pcm_mmap,
};

static int rtl819x_pcm_preallocate_dma_buffer(struct snd_pcm *pcm, int stream)
{
	struct snd_pcm_substream *substream = pcm->streams[stream].substream;
	struct snd_dma_buffer *buf = &substream->dma_buffer;
	size_t size = rtl819x_pcm_hardware.buffer_bytes_max;


	buf->dev.type = SNDRV_DMA_TYPE_DEV;
	buf->dev.dev = pcm->card->dev;
	buf->private_data = NULL;
	//buf->area = dma_alloc_coherent(pcm->card->dev, size,
	//				   &buf->addr, GFP_KERNEL);
	if (stream==SNDRV_PCM_STREAM_PLAYBACK) {
		buf->area = (unsigned char *) (((unsigned int)&iis_tx_data[0]) | 0xa0000000);
	} else {
		buf->area = (unsigned char *) (((unsigned int)&iis_rx_data[0]) | 0xa0000000);
	}
	tone_gens( size/2,buf->area);
	//printk("area:%x addr: %x\n", buf->area, buf->addr);
	if (!buf->area)
		return -ENOMEM;
	buf->bytes = size;
	return 0;
}

static void rtl819x_pcm_free_dma_buffers(struct snd_pcm *pcm)
{
	struct snd_pcm_substream *substream;
	struct snd_dma_buffer *buf;
	int stream;


	for (stream = 0; stream < 2; stream++) {
		substream = pcm->streams[stream].substream;
		if (!substream)
			continue;

		buf = &substream->dma_buffer;
		if (!buf->area)
			continue;

		//dma_free_coherent(pcm->card->dev, buf->bytes,
		//		      buf->area, buf->addr);
		buf->area = NULL;
	}
}

static u64 rtl819x_pcm_dmamask = DMA_BIT_MASK(32);

static int rtl819x_pcm_new(struct snd_card *card,
	struct snd_soc_dai *dai, struct snd_pcm *pcm)
{
	int ret = 0;


	if (!card->dev->dma_mask)
		card->dev->dma_mask = &rtl819x_pcm_dmamask;
	if (!card->dev->coherent_dma_mask)
		card->dev->coherent_dma_mask = 0xffffffff;

	if (dai->playback.channels_min) {
		ret = rtl819x_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_PLAYBACK);
		if (ret)
			goto out;
	}

	if (dai->capture.channels_min) {
		ret = rtl819x_pcm_preallocate_dma_buffer(pcm,
			SNDRV_PCM_STREAM_CAPTURE);
		if (ret)
			goto out;
	}
 out:
	return ret;
}

struct snd_soc_platform rtl819x_soc_platform = {
	.name		= "rtl819x-audio",
	.pcm_ops 	= &rtl819x_pcm_ops,
	.pcm_new	= rtl819x_pcm_new,
	.pcm_free	= rtl819x_pcm_free_dma_buffers,
};
EXPORT_SYMBOL_GPL(rtl819x_soc_platform);

static int __init rtl819x_soc_platform_init(void)
{
	request_irq(IIS_IRQ, iis_dma_interrupt, IRQF_DISABLED, "iis_dma", NULL);// NULL OK
	return snd_soc_register_platform(&rtl819x_soc_platform);
}
module_init(rtl819x_soc_platform_init);

static void __exit rtl819x_soc_platform_exit(void)
{
	snd_soc_unregister_platform(&rtl819x_soc_platform);
}
module_exit(rtl819x_soc_platform_exit);

MODULE_AUTHOR("XU JUNWEI, <jwsyu@realtek.com>");
MODULE_DESCRIPTION("Realtek I2S DMA module");
MODULE_LICENSE("GP1");
