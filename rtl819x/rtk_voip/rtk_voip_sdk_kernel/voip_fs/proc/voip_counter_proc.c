#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include "rtk_voip.h"
#include "voip_types.h"
#include "voip_init.h"
#include "voip_proc.h"

#define VOIP_COUNTER_DISABLE	0
#define VOIP_COUNTER_ENABLE	1
#define VOIP_COUNTER_RESET	255

#ifdef SUPPORT_VOIP_DBG_COUNTER

int32 gVoipCounterEnable = 0;
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM
static uint32 pcm_rx_count[BUS_PCM_CH_NUM] = {0};
static uint32 pcm_tx_count[BUS_PCM_CH_NUM] = {0};
#endif
#ifdef CONFIG_RTK_VOIP_DRIVERS_IIS
static uint32 iis_rx_count[BUS_IIS_CH_NUM] = {0};
static uint32 iis_tx_count[BUS_IIS_CH_NUM] = {0};
#endif
static uint32 rtp_rx_count[DSP_SS_NUM] = {0};
static uint32 rtp_tx_count[DSP_SS_NUM] = {0};
static uint32 dsp_process_count[DSP_SS_NUM] = {0};
static uint32 tone_gen_count[DSP_SS_NUM] = {0};

void reset_all_count(void)
{
	int i;
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM
	for (i=0; i<BUS_PCM_CH_NUM; i++)
	{
		pcm_rx_count[i] = 0;
		pcm_tx_count[i] = 0;
	}
#endif
#ifdef CONFIG_RTK_VOIP_DRIVERS_IIS
	for (i=0; i<BUS_IIS_CH_NUM; i++) {
		iis_rx_count[i] = 0;
		iis_tx_count[i] = 0;
	}
#endif
	for (i=0; i<DSP_SS_NUM; i++)
	{
		rtp_rx_count[i] = 0;
		rtp_tx_count[i] = 0;
		dsp_process_count[i] = 0;
		tone_gen_count[i] = 0;
	}
}

void PCM_rx_count(uint32 chid)
{
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM
	pcm_rx_count[chid]++;
#endif
}

void PCM_tx_count(uint32 chid)
{
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM
	pcm_tx_count[chid]++;
#endif
}

void IIS_rx_count(uint32 chid)
{
#ifdef CONFIG_RTK_VOIP_DRIVERS_IIS
	iis_rx_count[chid]++;
#endif
}

void IIS_tx_count(uint32 chid)
{
#ifdef CONFIG_RTK_VOIP_DRIVERS_IIS
	iis_tx_count[chid]++;
#endif
}

void RTP_rx_count(uint32 sid)
{
	rtp_rx_count[sid]++;
}

void RTP_tx_count(uint32 sid)
{
	rtp_tx_count[sid]++;
}

void DSP_process_count(uint32 sid)
{
	dsp_process_count[sid]++;
}

void Tone_gen_count(uint32 sid)
{
	tone_gen_count[sid]++;
}

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_counter_proc_read( struct seq_file *m, void *data )
{
	int chid, sid;

	if( m->index ) {	/* In our case, we write out all data at once. */
		return 0;
	}

	seq_printf(m, "VoIP counter information:\n");

	if (gVoipCounterEnable == VOIP_COUNTER_DISABLE)
	{
		seq_printf(m, "  *VoIP counter is disabled.\n");
	}
	else if (gVoipCounterEnable == VOIP_COUNTER_ENABLE)
	{
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM
		seq_printf(m, "  *PCM Counter:\n");
		for (chid = 0; chid < BUS_PCM_CH_NUM; chid++)
		{
			seq_printf(m, "    - ch%d rx: %d \n", chid, pcm_rx_count[chid]);
			seq_printf(m, "    - ch%d tx: %d \n", chid, pcm_tx_count[chid]);
		}
#endif
#ifdef CONFIG_RTK_VOIP_DRIVERS_IIS
		seq_printf(m, "  *IIS Counter:\n");
		for (chid = 0; chid < BUS_IIS_CH_NUM; chid++)
		{
			seq_printf(m, "    - ch%d rx: %d \n", chid, iis_rx_count[chid]);
			seq_printf(m, "    - ch%d tx: %d \n", chid, iis_tx_count[chid]);
		}
#endif
		seq_printf(m, "  *RTP Counter:\n");
		for (sid = 0; sid < DSP_SS_NUM; sid++)
		{
			seq_printf(m, "    - sid%d rx: %d \n", sid, rtp_rx_count[sid]);
			seq_printf(m, "    - sid%d tx: %d \n", sid, rtp_tx_count[sid]);
		}
		
		seq_printf(m, "  *DSP Counter:\n");
		for (sid = 0; sid < DSP_SS_NUM; sid++)
		{
			seq_printf(m, "    - sid%d dsp process: %d \n", sid, dsp_process_count[sid]);
		}
		
		for (sid = 0; sid < DSP_SS_NUM; sid++)
		{
			seq_printf(m, "    - sid%d tone gen: %d \n", sid, tone_gen_count[sid]);
		}
	}

	return 0;
}
#else
static int voip_counter_read(char *page, char **start, off_t off,
        int count, int *eof, void *data)
{
	int chid, sid;
	int n;

	if( off ) {	/* In our case, we write out all data at once. */
		*eof = 1;
		return 0;
	}

	n = sprintf(page, "VoIP counter information:\n");

	if (gVoipCounterEnable == VOIP_COUNTER_DISABLE)
	{
		n += sprintf(page+n, "  *VoIP counter is disabled.\n");
	}
	else if (gVoipCounterEnable == VOIP_COUNTER_ENABLE)
	{
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM
		n += sprintf(page+n, "  *PCM Counter:\n");
		for (chid = 0; chid < BUS_PCM_CH_NUM; chid++)
		{
			n += sprintf(page+n, "    - ch%d rx: %d \n", chid, pcm_rx_count[chid]);
			n += sprintf(page+n, "    - ch%d tx: %d \n", chid, pcm_tx_count[chid]);
		}
#endif
#ifdef CONFIG_RTK_VOIP_DRIVERS_IIS
		n += sprintf(page+n, "  *IIS Counter:\n");
		for (chid = 0; chid < BUS_IIS_CH_NUM; chid++)
		{
			n += sprintf(page+n, "    - ch%d rx: %d \n", chid, iis_rx_count[chid]);
			n += sprintf(page+n, "    - ch%d tx: %d \n", chid, iis_tx_count[chid]);
		}
#endif
		n += sprintf(page+n, "  *RTP Counter:\n");
		for (sid = 0; sid < DSP_SS_NUM; sid++)
		{
			n += sprintf(page+n, "    - sid%d rx: %d \n", sid, rtp_rx_count[sid]);
			n += sprintf(page+n, "    - sid%d tx: %d \n", sid, rtp_tx_count[sid]);
		}
		
		n += sprintf(page+n, "  *DSP Counter:\n");
		for (sid = 0; sid < DSP_SS_NUM; sid++)
		{
			n += sprintf(page+n, "    - sid%d dsp process: %d \n", sid, dsp_process_count[sid]);
		}
		
		for (sid = 0; sid < DSP_SS_NUM; sid++)
		{
			n += sprintf(page+n, "    - sid%d tone gen: %d \n", sid, tone_gen_count[sid]);
		}
	}

	*eof = 1;
	return n;
}
#endif

static int voip_counter_write(struct file *file, const char *buffer, 
                               unsigned long count, void *data)
{
	char tmp[128];

	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, 128)) {
		sscanf(tmp, "%d", &gVoipCounterEnable);
	}

	if (gVoipCounterEnable == VOIP_COUNTER_RESET)
	{
		reset_all_count();
		gVoipCounterEnable = VOIP_COUNTER_DISABLE;
	}
	//printk("gVoipCounterEnable = %d\n", gVoipCounterEnable);

	return count;
}
#endif	//SUPPORT_VOIP_DBG_COUNTER

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_counter_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voip_counter_proc_read, NULL);
}

static const struct file_operations voip_counter_fops = 
{
	.open = voip_counter_proc_open,
	.write = voip_counter_write,
	.read = seq_read,
};
#endif

static int __init voip_proc_counter_init(void)
{
#ifdef SUPPORT_VOIP_DBG_COUNTER
	struct proc_dir_entry *voip_counter_proc;

#ifdef CONFIG_LINUX_KERNEL_3_10
	voip_counter_proc = create_voip_proc_rw_entry(PROC_VOIP_DIR "/counter", 0, NULL,
							&voip_counter_fops );
#else
	voip_counter_proc = create_voip_proc_rw_entry(PROC_VOIP_DIR "/counter", 0, NULL,
							voip_counter_read, voip_counter_write );
#endif
	
	if (voip_counter_proc == NULL)
	{
		printk("voip_counter_proc NULL!! \n");
		return -1;
	}
#else
	printk("SUPPORT_VOIP_DBG_COUNTER is not defeind!\n");
#endif
	
	return 0;
}

static void __exit voip_proc_counter_exit( void )
{
#ifdef SUPPORT_VOIP_DBG_COUNTER
	remove_voip_proc_entry( PROC_VOIP_DIR "/counter", NULL );
#else
	printk("SUPPORT_VOIP_DBG_COUNTER is not defeind!\n");
#endif
}

voip_initcall_proc( voip_proc_counter_init );
voip_exitcall( voip_proc_counter_exit );
