#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include "rtk_voip.h"
#include "voip_version.h"
#include "voip_feature.h"
//#include "Slic_api.h"
#include "snd_mux_slic.h"
//#include "Daa_api.h"
#include "snd_mux_daa.h"
#include "con_mux.h"
#include "voip_init.h"
#include "voip_proc.h"
#include "voip_qos.h"

#ifdef IPC_ARCH_DEBUG_HOST
extern unsigned int dsp_ctrl_tx_cnt;
#endif
#ifdef IPC_ARCH_DEBUG_DSP
extern unsigned int dsp_resp_tx_cnt;
extern unsigned int dsp_rtp_tx_cnt;
extern unsigned int dsp_rtcp_tx_cnt;
extern unsigned int dsp_t38_tx_cnt;
#endif
#ifdef CONFIG_RTK_VOIP_QOS
int voip_qos;
#endif


#ifdef CONFIG_LINUX_KERNEL_3_10
int voip_info_proc_read( struct seq_file *m, void *data )
{
#ifndef CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST
	int i;
#endif
#ifdef CONFIG_REALTEK_VOIP
	extern int G711_CNInfoStr( uint32 sid, char *buf );
#endif
	
	if( m->index ) {	/* In our case, we write out all data at once. */
		return 0;
	}

	seq_printf(m, "* VoIP Middleware\t:");
	if (RTK_VOIP_MW_CHK_IS_REALTEK( gVoipFeature ) )
		seq_printf(m, "Realtek\n");
	else if (RTK_VOIP_MW_CHK_IS_AUDIOCODES( gVoipFeature ) )
		seq_printf(m, "AudioCodes\n");
	
	seq_printf(m, "* VoIP Version\t\t:%s (Build Date/Time= %s, %s)\n", VOIP_VERSION, __DATE__, __TIME__);
	seq_printf(m, "* System Version\t:%s\n", SYS_VERSION);
	seq_printf(m, "* On-chip ROM Code\t:");
#ifdef CONFIG_USE_ROMCODE
	seq_printf(m, "Yes\n");
#else
	seq_printf(m, "No\n");
#endif
	seq_printf(m, "* Board CFG Model\t:%s\n", CONFIG_BOARD_CONFIG_MODEL);

	seq_printf(m, "* VoIP Channel Number\t:%d\n", RTK_VOIP_CH_NUM( gVoipFeature ));
	seq_printf(m, "* Phone Number\t\t:%d\n", RTK_VOIP_PHONE_NUM( gVoipFeature ));
	seq_printf(m, "* DECT Number\t\t:%d\n", RTK_VOIP_DECT_NUM( gVoipFeature ));
	seq_printf(m, "* SLIC Number\t\t:%d\n", RTK_VOIP_SLIC_NUM( gVoipFeature ));
	seq_printf(m, "* DAA Number\t\t:%d\n", RTK_VOIP_DAA_NUM( gVoipFeature ));
	seq_printf(m, "* IVR Support\t\t:");
	if (gVoipFeature & IVR_SUPPORT)
		seq_printf(m, "Yes\n");
	else
		seq_printf(m, "No\n");
	
	seq_printf(m, "* T.38 Support\t\t:");
	if (gVoipFeature & CODEC_T38_SUPPORT) {
		seq_printf(m, "Yes\n");
#if !defined(CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST) && defined(CONFIG_RTK_VOIP_T38)
		{
			extern char* t38_version_str;
			seq_printf(m, "\t%s", t38_version_str);
		}
#endif
	} else
		seq_printf(m, "No\n");

	seq_printf(m, "* Codec Support\n");
	seq_printf(m, "  - G.711u/a\t:Yes\n");

#if !defined(CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST)
#ifdef CONFIG_REALTEK_VOIP
	for (i = 0; i < DSP_RTK_SS_NUM; i++)
	{
		n += G711_CNInfoStr( i, buf + n );
	}
#endif
#endif

	
	if (gVoipFeature & CODEC_G729_SUPPORT)
		seq_printf(m, "  - G.729\t:Yes\n");
	
	if (gVoipFeature & CODEC_G723_SUPPORT)
		seq_printf(m, "  - G.723\t:Yes\n");
		
	if (gVoipFeature & CODEC_G726_SUPPORT)
		seq_printf(m, "  - G.726\t:Yes\n");
		
	if (gVoipFeature & CODEC_G722_SUPPORT)
#ifdef CONFIG_RTK_VOIP_G722_ITU_USE
		seq_printf(m, "  - G.722-PLC\t:Yes\n");
#else
		seq_printf(m, "  - G.722\t:Yes\n");
#endif		

	if (gVoipFeature & CODEC_GSMFR_SUPPORT)
		seq_printf(m, "  - GSM-FR\t:Yes\n");
		
	if (gVoipFeature & CODEC_iLBC_SUPPORT)
		seq_printf(m, "  - iLBC\t:Yes\n");

	if (gVoipFeature & CODEC_SPEEX_NB_SUPPORT)
		seq_printf(m, "  - SPEEX-NB\t:Yes\n");
	
	if (gVoipFeature & CODEC_AMR_SUPPORT)
		seq_printf(m, "  - AMR-NB\t:Yes\n");
	
	if (gVoipFeature & CODEC_G7111_SUPPORT)
		seq_printf(m, "  - G.711.1\t:Yes\n");

#ifndef CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST
	seq_printf(m, "* FXS Line Status:\n");
	for(i=0; i < con_ch_num; i++)
	{
		if( get_snd_type_cch( i ) != SND_TYPE_FXS )
			continue;
		
		seq_printf(m, "  - Port%d: ", i);
		switch (FXS_Line_Check(i))
		{
			case 0:
				seq_printf(m, "Phone Dis-connected\n");
				break;
			case 1:
				seq_printf(m, "Phone Connected(On-hook)\n");
				break;
			case 2:
				seq_printf(m, "Phone Connected(Off-hook)\n");
				break;
			case 3:
				seq_printf(m, "Check Timeout(May connect too many phone set)\n");
				break;
			case 4:
				seq_printf(m, "Can't Check Status\n");
				break;
			default:
				seq_printf(m, "Unknow State\n");
				break;
		}
	}
	
#if defined (CONFIG_RTK_VOIP_DRIVERS_FXO) && !defined (CONFIG_RTK_VOIP_DRIVERS_VIRTUAL_DAA)

	seq_printf(m, "* FXO Line Status:\n");
		
	for(i = 0; i < con_ch_num; i++)
	{
		switch( get_snd_type_cch( i ) ) {
		case SND_TYPE_DAA:
		case SND_TYPE_VDAA:
			break;
			
		default:
			continue;
			break;
		}
			
		seq_printf(m, "  - Port%d: ", i);

		switch (DAA_Check_Line_State(i))
		{
			case 0:
				seq_printf(m, "Line Connected\n");
				break;
			case 1:
				seq_printf(m, "Line Dis-connected\n");
				break;
			case 2:
				seq_printf(m, "Line Busy\n");
				break;
			default:
				seq_printf(m, "Unknow State\n");
				break;
		}
	}
#endif
#endif	//CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST

	return 0;
}
#else
int get_voip_info( char *buf, char **start, off_t off, int count, int *eof, void *data )
{
#ifndef CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST
	int i;
#endif
	int n;
#ifdef CONFIG_REALTEK_VOIP
	extern int G711_CNInfoStr( uint32 sid, char *buf );
#endif
	
	if( off ) {	/* In our case, we write out all data at once. */
		*eof = 1;
		return 0;
	}

	n = sprintf(buf, "* VoIP Middleware\t:");
	if (RTK_VOIP_MW_CHK_IS_REALTEK( gVoipFeature ) )
		n += sprintf(buf + n, "Realtek\n");
	else if (RTK_VOIP_MW_CHK_IS_AUDIOCODES( gVoipFeature ) )
		n += sprintf(buf + n, "AudioCodes\n");
	
	n += sprintf(buf + n, "* VoIP Version\t\t:%s (Build Date/Time= %s, %s)\n", VOIP_VERSION, __DATE__, __TIME__);
	n += sprintf(buf + n, "* System Version\t:%s\n", SYS_VERSION);
    n += sprintf(buf + n, "* On-chip ROM Code\t:");
#ifdef CONFIG_USE_ROMCODE
	n += sprintf(buf + n, "Yes\n");
#else
	n += sprintf(buf + n, "No\n");
#endif
	n += sprintf(buf + n, "* Board CFG Model\t:%s\n", CONFIG_BOARD_CONFIG_MODEL);

	n += sprintf(buf + n, "* VoIP Channel Number\t:%d\n", RTK_VOIP_CH_NUM( gVoipFeature ));
	n += sprintf(buf + n, "* Phone Number\t\t:%d\n", RTK_VOIP_PHONE_NUM( gVoipFeature ));
	n += sprintf(buf + n, "* DECT Number\t\t:%d\n", RTK_VOIP_DECT_NUM( gVoipFeature ));
	n += sprintf(buf + n, "* SLIC Number\t\t:%d\n", RTK_VOIP_SLIC_NUM( gVoipFeature ));
	n += sprintf(buf + n, "* DAA Number\t\t:%d\n", RTK_VOIP_DAA_NUM( gVoipFeature ));
#ifdef CONFIG_RTK_VOIP_DRIVERS_IP_PHONE
    #ifdef CONFIG_RTK_VOIP_DRIVERS_CODEC_ALC5621
	n += sprintf(buf + n, "* IP-Phone Codec\t:%s\n", "ALC5621");
    #endif
    #ifdef CONFIG_RTK_VOIP_DRIVERS_CODEC_ALC5633Q
	n += sprintf(buf + n, "* IP-Phone Codec\t:%s\n", "ALC5633Q");
    #endif
#endif
	n += sprintf(buf + n, "* IVR Support\t\t:");
	if (gVoipFeature & IVR_SUPPORT)
		n += sprintf(buf + n, "Yes\n");
	else
		n += sprintf(buf + n, "No\n");
	
	n += sprintf(buf + n, "* T.38 Support\t\t:");
	if (gVoipFeature & CODEC_T38_SUPPORT) {
		n += sprintf(buf + n, "Yes\n");
#if !defined(CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST) && defined(CONFIG_RTK_VOIP_T38)
		{
			extern char* t38_version_str;
			n += sprintf(buf + n, "\t%s", t38_version_str);
		}
#endif
	} else
		n += sprintf(buf + n, "No\n");

	n += sprintf(buf + n, "* Codec Support\n");
	n += sprintf(buf + n, "  - G.711u/a\t:Yes\n");

#if !defined(CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST)
#ifdef CONFIG_REALTEK_VOIP
	for (i = 0; i < DSP_RTK_SS_NUM; i++)
	{
		n += G711_CNInfoStr( i, buf + n );
	}
#endif
#endif

	
	if (gVoipFeature & CODEC_G729_SUPPORT)
		n += sprintf(buf + n, "  - G.729\t:Yes\n");
	
	if (gVoipFeature & CODEC_G723_SUPPORT)
		n += sprintf(buf + n, "  - G.723\t:Yes\n");
		
	if (gVoipFeature & CODEC_G726_SUPPORT)
		n += sprintf(buf + n, "  - G.726\t:Yes\n");
		
	if (gVoipFeature & CODEC_G722_SUPPORT)
#ifdef CONFIG_RTK_VOIP_G722_ITU_USE
		n += sprintf(buf + n, "  - G.722-PLC\t:Yes\n");
#else
		n += sprintf(buf + n, "  - G.722\t:Yes\n");
#endif		

	if (gVoipFeature & CODEC_GSMFR_SUPPORT)
		n += sprintf(buf + n, "  - GSM-FR\t:Yes\n");
		
	if (gVoipFeature & CODEC_iLBC_SUPPORT)
		n += sprintf(buf + n, "  - iLBC\t:Yes\n");

	if (gVoipFeature & CODEC_SPEEX_NB_SUPPORT)
		n += sprintf(buf + n, "  - SPEEX-NB\t:Yes\n");
	
	if (gVoipFeature & CODEC_AMR_SUPPORT)
		n += sprintf(buf + n, "  - AMR-NB\t:Yes\n");
	
	if (gVoipFeature & CODEC_G7111_SUPPORT)
		n += sprintf(buf + n, "  - G.711.1\t:Yes\n");
	
	if (gVoipFeature & CODEC_AMR_WB_SUPPORT)
		n += sprintf(buf + n, "  - AMR-WB\t:Yes\n");

#ifndef CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST
	n += sprintf(buf + n, "* FXS Line Status:\n");
	for(i=0; i < con_ch_num; i++)
	{
		if( get_snd_type_cch( i ) != SND_TYPE_FXS )
			continue;
		
		n += sprintf(buf + n, "  - Port%d: ", i);
		switch (FXS_Line_Check(i))
		{
			case 0:
				n += sprintf(buf + n, "Phone Dis-connected\n");
				break;
			case 1:
				n += sprintf(buf + n, "Phone Connected(On-hook)\n");
				break;
			case 2:
				n += sprintf(buf + n, "Phone Connected(Off-hook)\n");
				break;
			case 3:
				n += sprintf(buf + n, "Check Timeout(May connect too many phone set)\n");
				break;
			case 4:
				n += sprintf(buf + n, "Can't Check Status\n");
				break;
			default:
				n += sprintf(buf + n, "Unknow State\n");
				break;
		}
	}
	
#if defined (CONFIG_RTK_VOIP_DRIVERS_FXO) && !defined (CONFIG_RTK_VOIP_DRIVERS_VIRTUAL_DAA)

	n += sprintf(buf + n, "* FXO Line Status:\n");
		
	for(i = 0; i < con_ch_num; i++)
	{
		switch( get_snd_type_cch( i ) ) {
		case SND_TYPE_DAA:
		case SND_TYPE_VDAA:
			break;
			
		default:
			continue;
			break;
		}
			
		n += sprintf(buf + n, "  - Port%d: ", i);

		switch (DAA_Check_Line_State(i))
		{
			case 0:
				n += sprintf(buf + n, "Line Connected\n");
				break;
			case 1:
				n += sprintf(buf + n, "Line Dis-connected\n");
				break;
			case 2:
				n += sprintf(buf + n, "Line Busy\n");
				break;
			default:
				n += sprintf(buf + n, "Unknow State\n");
				break;
		}
	}
#endif
#endif	//CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST

	*eof = 1;
	return n;
}
#endif

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_info_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voip_info_proc_read, NULL);
}

static const struct file_operations voip_info_fops = 
{
	.open = voip_info_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
};
#endif

int __init voip_info_init( void )
{
#ifdef CONFIG_LINUX_KERNEL_3_10
	proc_create_data( PROC_VOIP_DIR "/info",0, NULL, &voip_info_fops, NULL );
#else
	create_proc_read_entry( PROC_VOIP_DIR "/info",0, NULL, get_voip_info, NULL );
#endif
	return  0;
}

void __exit voip_info_exit( void )
{
	remove_voip_proc_entry( PROC_VOIP_DIR "/info", NULL );
}

voip_initcall_proc(voip_info_init);
voip_exitcall(voip_info_exit);

int __init voip_proc_structure_init( void )
{
	int i;
	char buf[ 256 ];
	
	proc_mkdir( PROC_VOIP_DIR, NULL );	// 'voip'

#ifdef CONFIG_RTK_VOIP_DRIVERS_ATA_DECT	
	proc_mkdir( PROC_VOIP_DIR "/" PROC_VOIP_DECT_DIR, NULL );
#endif 
	
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM
	proc_mkdir( PROC_VOIP_DIR "/" PROC_VOIP_PCM_DIR, NULL );
#endif
	
	proc_mkdir( PROC_VOIP_DIR "/" PROC_VOIP_CH_MAPS_DIR, NULL );
	
	for( i = 0; i < PROC_VOIP_CH_NUM; i ++ ) {	// 'voip/ch%02u'
		sprintf( buf, PROC_VOIP_DIR "/" PROC_VOIP_CH_FORMAT, i );
		proc_mkdir( buf, NULL );
	}

	for( i = 0; i < PROC_VOIP_SS_NUM; i ++ ) {	// 'voip/ss%02u'
		sprintf( buf, PROC_VOIP_DIR "/" PROC_VOIP_SS_FORMAT, i );
		proc_mkdir( buf, NULL );
	}
	
	return 0;
}

void __exit voip_proc_structure_exit( void )
{
	int i;
	char buf[ 256 ];
	
	proc_mkdir( PROC_VOIP_DIR, NULL );	// 'voip'
	
	for( i = 0; i < PROC_VOIP_CH_NUM; i ++ ) {	// 'voip/ch%02u'
		sprintf( buf, PROC_VOIP_DIR "/" PROC_VOIP_CH_FORMAT, i );
		remove_voip_proc_entry( buf, NULL );
	}

	for( i = 0; i < PROC_VOIP_SS_NUM; i ++ ) {	// 'voip/ss%02u'
		sprintf( buf, PROC_VOIP_DIR "/" PROC_VOIP_SS_FORMAT, i );
		remove_voip_proc_entry( buf, NULL );
	}
}

voip_initcall( voip_proc_structure_init );
voip_exitcall( voip_proc_structure_exit );

#ifdef CONFIG_RTK_VOIP_QOS
#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_qos_proc_read( struct seq_file *m, void *data )
{
	if(!voip_qos)
	       seq_printf(m,"qos is disabled\n");
	else
	{
		//seq_printf(m,"voip_qos: %d \n",voip_qos);
		if(voip_qos & VOIP_QOS_INTERRUPT_RX_TASKLET)
		{
			seq_printf(m,"INTERRUPT_RX_TASKLET is enabled \n");
		}
		if(voip_qos & VOIP_QOS_RX_HW_HIGH_QUEUE)
                {
                        seq_printf(m,"RX_HW_HIGH_QUEUE is enabled \n");
			
                }
		if(voip_qos & VOIP_QOS_RX_SW_HIGH_QUEUE)
                {
                        seq_printf(m,"RX_SW_HIGH_QUEUE is enabled \n");
                }
		if(voip_qos & VOIP_QOS_TX_HW_HIGH_QUEUE)
                {
                        seq_printf(m,"TX_HW_HIGH_QUEUE is enabled \n");
                }
		if(voip_qos & VOIP_QOS_TX_SW_HIGH_QUEUE)
                {
                        seq_printf(m,"TX_SW_HIGH_QUEUE is enabled \n");
                }
		if(voip_qos & VOIP_QOS_TX_DISABLE_FC)
                {
                        seq_printf(m,"TX_DISABLE_FC is enabled \n");
                }
		if(voip_qos & VOIP_QOS_LOCAL_SESSION_RESERVE)
                {
                        seq_printf(m,"LOCAL_SESSION_RESERVE is enabled \n");
                }
		if(voip_qos & VOIP_QOS_DROP_BIG_TRAFFIC)
                {
                        seq_printf(m,"DROP_BIG_TRAFFIC is enabled \n");
                }

	}

    return 0;

}
#else
static int voip_qos_read(char *page, char **start, off_t off,
        int count, int *eof, void *data)
{
	int len;
	
	char tmp[300]={0};
	int index = 0;
	if(!voip_qos)
	       len = sprintf(page,"qos is disabled\n");
	else
	{
		//index += sprintf(tmp+index,"voip_qos: %d \n",voip_qos);
		if(voip_qos & VOIP_QOS_INTERRUPT_RX_TASKLET)
		{
			index += sprintf(tmp+index,"INTERRUPT_RX_TASKLET is enabled \n");
		}
		if(voip_qos & VOIP_QOS_RX_HW_HIGH_QUEUE)
                {
                        index += sprintf(tmp+index,"RX_HW_HIGH_QUEUE is enabled \n");
			
                }
		if(voip_qos & VOIP_QOS_RX_SW_HIGH_QUEUE)
                {
                        index += sprintf(tmp+index,"RX_SW_HIGH_QUEUE is enabled \n");
                }
		if(voip_qos & VOIP_QOS_TX_HW_HIGH_QUEUE)
                {
                        index += sprintf(tmp+index,"TX_HW_HIGH_QUEUE is enabled \n");
                }
		if(voip_qos & VOIP_QOS_TX_SW_HIGH_QUEUE)
                {
                        index += sprintf(tmp+index,"TX_SW_HIGH_QUEUE is enabled \n");
                }
		if(voip_qos & VOIP_QOS_TX_DISABLE_FC)
                {
                        index += sprintf(tmp+index,"TX_DISABLE_FC is enabled \n");
                }
		if(voip_qos & VOIP_QOS_LOCAL_SESSION_RESERVE)
                {
                        index += sprintf(tmp+index,"LOCAL_SESSION_RESERVE is enabled \n");
                }
		if(voip_qos & VOIP_QOS_DROP_BIG_TRAFFIC)
                {
                        index += sprintf(tmp+index,"DROP_BIG_TRAFFIC is enabled \n");
                }
		len =  sprintf(page,tmp);
	}
    if (len <= off+count)
        *eof = 1;
    *start = page + off;
    len -= off;

    if (len > count)
        len = count;
    if (len < 0)
        len = 0;
    return len;

}
#endif

static int voip_qos_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char tmp[128];

	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, 128)) {
		sscanf(tmp,"%d",&voip_qos);
	}
	return count;
}

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_qos_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voip_qos_proc_read, NULL);
}

static const struct file_operations voip_qos_fops =
{
    .open = voip_qos_proc_open,
    .write = voip_qos_write,
    .read = seq_read,
};
#endif

static int __init voip_proc_qos_init (void)
{
	struct proc_dir_entry *voip_qos_proc;
	
	voip_qos |= VOIP_QOS_INTERRUPT_RX_TASKLET;
	voip_qos |= VOIP_QOS_RX_HW_HIGH_QUEUE;
	voip_qos |= VOIP_QOS_RX_SW_HIGH_QUEUE;
	voip_qos |= VOIP_QOS_TX_HW_HIGH_QUEUE;
	voip_qos |= VOIP_QOS_TX_SW_HIGH_QUEUE;
	voip_qos |= VOIP_QOS_TX_DISABLE_FC;
	voip_qos |= VOIP_QOS_LOCAL_SESSION_RESERVE;

	//voip_qos |= VOIP_QOS_DROP_BIG_TRAFFIC;

#ifdef CONFIG_LINUX_KERNEL_3_10
	voip_qos_proc = create_voip_proc_rw_entry(PROC_VOIP_DIR "/qos",0,NULL,
						&voip_qos_fops);
#else	
	voip_qos_proc = create_voip_proc_rw_entry(PROC_VOIP_DIR "/qos",0,NULL,
						voip_qos_read, voip_qos_write);
#endif
	if (voip_qos_proc == NULL){
		printk("voip_qos_proc NULL!! \n");
		return -1;
	}
	
	return 0;
}

static void __exit voip_proc_qos_exit( void )
{
	remove_voip_proc_entry( PROC_VOIP_DIR "/qos", NULL );
}

voip_initcall_proc( voip_proc_qos_init );
voip_exitcall( voip_proc_qos_exit );

#endif


#ifdef CONFIG_LINUX_KERNEL_3_10
int voip_version_proc_read( struct seq_file *m, void *data )
{
	if( m->index ) { /* In our case, we write out all data at once. */
		return 0;
	}
	seq_printf(m, "%s\n", VOIP_VERSION);
	return 0;

}
#else
int get_voip_version( char *buf, char **start, off_t off, int count, int *eof, void *data )
{
	int n;
	if( off ) { /* In our case, we write out all data at once. */
		*eof = 1;
		return 0;
	}
	n = sprintf(buf, "%s\n", VOIP_VERSION);
	*eof = 1;
	return n;

}
#endif

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_version_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voip_version_proc_read, NULL);
}

static const struct file_operations voip_version_fops = 
{
	.open = voip_version_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
};
#endif

int __init voip_version_init( void )
{
#ifdef CONFIG_LINUX_KERNEL_3_10
	proc_create_data( PROC_VOIP_DIR "/version",0, NULL, &voip_version_fops, NULL );
#else
	create_proc_read_entry( PROC_VOIP_DIR "/version",0, NULL, get_voip_version, NULL );
#endif
	return  0;
}

void __exit voip_version_exit( void )
{
	remove_voip_proc_entry( PROC_VOIP_DIR "/version", NULL );
}

voip_initcall_proc(voip_version_init);
voip_exitcall(voip_version_exit);

