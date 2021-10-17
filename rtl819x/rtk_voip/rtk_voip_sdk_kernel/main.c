#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#ifdef CONFIG_CPU_HAS_WATCH
#include <linux/ptrace.h>
#endif
#include "rtk_voip.h"
#include "voip_init.h"
#include "voip_proc.h"

#ifdef CONFIG_VOIP_HOST_SMP_LOCK
spinlock_t lock_voip;
int lock_voip_owner = -1;
#ifdef CONFIG_RTK_VOIP_DRIVERS_ATA_SLIC
spinlock_t lock_Reg;
spinlock_t lock_Ram;
int lock_Reg_owner = -1;
int lock_Ram_owner = -1;
#endif
#endif

#if defined (CONFIG_AUDIOCODES_VOIP) && defined (CONFIG_RTK_VOIP_DRIVERS_IP_PHONE) || defined (CONFIG_RTK_VOIP_DECT_UART_SUPPORT)
int rtk_dbg_level = RTK_DBG_ERROR;
//int rtk_dbg_level = RTK_DBG_TRACE;
#else
//int rtk_dbg_level = RTK_DBG_INFO;
int rtk_dbg_level = RTK_DBG_ERROR;
//int rtk_dbg_level = RTK_DBG_TRACE;
#endif

static int __init rtk_voip_init(void)
{
#ifdef CONFIG_VOIP_HOST_SMP_LOCK
	spin_lock_init(&lock_voip);
#ifdef CONFIG_RTK_VOIP_DRIVERS_ATA_SLIC
	spin_lock_init(&lock_Reg);
	spin_lock_init(&lock_Ram);
#endif
	printk(" voip spin lock init done\n");
#endif

	PRINT_MSG("rtk_voip_init\n");
	return 0;
}

static void __exit rtk_voip_exit(void)
{
	PRINT_MSG("rtk_voip_exit\n");
}

#ifdef CONFIG_RTK_VOIP_DRIVERS_11N_MP
int PCM_Check_Any_ChanEnabled(void)
{
	return 0;
}
#endif

voip_initcall(rtk_voip_init);
voip_exitcall(rtk_voip_exit);



#ifdef CONFIG_CPU_HAS_WATCH
static unsigned char test[2];

void memwatch_test_init(unsigned char* ptr)
{
	struct wmpu_addr addr;
	struct wmpu_addr *p = &addr;

#if 0
	p->start = test;
	p->end = test;
#else
	p->start = ptr;
	p->end = ptr;
#endif
	p->attr = WMPU_DW;
	ptrace_wmpu_wp(p);
}

void memwatch_test_run(unsigned char* ptr)
{
#if 0
	test[0] = 0xFF;
#else
	*ptr = 0xFF;
#endif
}
#endif


#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_debug_proc_read( struct seq_file *m, void *data )
{
	extern int g_voip_debug;
	extern int g_force_codec;
	extern int g_force_vad;
	extern int g_force_ptime;
	extern int g_force_PcmMode;
	extern int g_force_g7111mode;
	extern int g_enable_fax_dis;
	extern int g_disable_announce_fax;
	extern int g_disable_report_fax;
	

	if( m->index ) {	/* In our case, we write out all data at once. */
		return 0;
	}

	seq_printf(m, "VoIP Debug Information:\n");
	seq_printf(m, "  - VoIP Debug Message Level: %d\n", rtk_dbg_level);
	
	seq_printf(m, "  - last cmd: %d\n", g_voip_debug);
	seq_printf(m, "  - force codec: %d\n", g_force_codec);
	seq_printf(m, "  - force VAD: %d\n", g_force_vad);
	seq_printf(m, "  - force ptime: %d\n", g_force_ptime);
	seq_printf(m, "  - force pcmmode: %d\n", g_force_PcmMode);
	seq_printf(m, "  - force 711 mode: %d\n", g_force_g7111mode);
	seq_printf(m, "  - enable fax dis: %d\n", g_enable_fax_dis);
	seq_printf(m, "  - disable announce fax: %d\n", g_disable_announce_fax);
	seq_printf(m, "  - disable report fax: %d\n", g_disable_report_fax);
	
	return 0;
}
#else
static int voip_debug_read(char *page, char **start, off_t off,
        int count, int *eof, void *data)
{
	extern int g_voip_debug;
	extern int g_force_codec;
	extern int g_force_vad;
	extern int g_force_ptime;
	extern int g_force_PcmMode;
	extern int g_force_g7111mode;
	extern int g_enable_fax_dis;
	extern int g_disable_announce_fax;
	extern int g_disable_report_fax;
	
	int chid, sid;
	int n;

	if( off ) {	/* In our case, we write out all data at once. */
		*eof = 1;
		return 0;
	}

	n = sprintf(page, "VoIP Debug Information:\n");
	n += sprintf(page+n, "  - VoIP Debug Message Level: %d\n", rtk_dbg_level);
	
	n += sprintf(page+n, "  - last cmd: %d\n", g_voip_debug);
	n += sprintf(page+n, "  - force codec: %d\n", g_force_codec);
	n += sprintf(page+n, "  - force VAD: %d\n", g_force_vad);
	n += sprintf(page+n, "  - force ptime: %d\n", g_force_ptime);
	n += sprintf(page+n, "  - force pcmmode: %d\n", g_force_PcmMode);
	n += sprintf(page+n, "  - force 711 mode: %d\n", g_force_g7111mode);
	n += sprintf(page+n, "  - enable fax dis: %d\n", g_enable_fax_dis);
	n += sprintf(page+n, "  - disable announce fax: %d\n", g_disable_announce_fax);
	n += sprintf(page+n, "  - disable report fax: %d\n", g_disable_report_fax);
	
	*eof = 1;
	return n;
}
#endif

static int voip_debug_write(struct file *file, const char *buffer, 
                               unsigned long count, void *data)
{
	extern void do_voip_debug_translation( void );
	extern int g_voip_debug;
	
	char tmp[128];
	int dbg_val;

	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, 128)) {
		if( sscanf(tmp, "%d", &dbg_val) != 1 )
			return -EFAULT;

		printk("dbg_val = %d\n", dbg_val);

		// debug level 
		if (dbg_val <= RTK_DBG_MAX)
			rtk_dbg_level = dbg_val;
		
		// voip debug 
		g_voip_debug = dbg_val;
		
		do_voip_debug_translation();
		
#ifdef CONFIG_CPU_HAS_WATCH
		if (dbg_val == 80)
			memwatch_test_init();
		else if (dbg_val == 81)
			memwatch_test_run();
#endif
	}

	return count;
}

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_debug5_proc_read( struct seq_file *m, void *data )
{
	extern int g_voip_debug5;
#ifndef CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST
#ifdef CONFIG_RTK_VOIP_T38
	extern int g_disable_1st_no_sig_packet;
#endif
#endif

    if( m->index ) {	/* In our case, we write out all data at once. */
		return 0;
	}

	seq_printf(m, "VoIP Debug5 Information:\n");
	seq_printf(m, "  - last cmd: %d\n", g_voip_debug5);
#ifndef CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST
#ifdef CONFIG_RTK_VOIP_T38
	seq_printf(m, "  - disable firt t.38 no-signal packet : %d\n", g_disable_1st_no_sig_packet);
#endif
#endif
	return 0;
}
#else
static int voip_debug5_read(char *page, char **start, off_t off,
        int count, int *eof, void *data)
{
	extern int g_voip_debug5;
#ifndef CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST
#ifdef CONFIG_RTK_VOIP_T38
	extern int g_disable_1st_no_sig_packet;
#endif
#endif
	int n;

	if( off ) {	/* In our case, we write out all data at once. */
		*eof = 1;
		return 0;
	}

	n = sprintf(page, "VoIP Debug5 Information:\n");
	n += sprintf(page+n, "  - last cmd: %d\n", g_voip_debug5);
#ifndef CONFIG_RTK_VOIP_IPC_ARCH_IS_HOST
#ifdef CONFIG_RTK_VOIP_T38
	n += sprintf(page+n, "  - disable firt t.38 no-signal packet : %d\n", g_disable_1st_no_sig_packet);
#endif
#endif
	*eof = 1;
	return n;
}
#endif

static int voip_debug5_write(struct file *file, const char *buffer, 
                               unsigned long count, void *data)
{
	extern int g_voip_debug5;
	
	char tmp[128];
	int dbg_val;

	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, 128)) {
		if( sscanf(tmp, "%d", &dbg_val) != 1 )
			return -EFAULT;

		printk("dbg_val = %d\n", dbg_val);

		// voip debug 5
		g_voip_debug5 = dbg_val;
	}

	return count;
}

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_debug5_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voip_debug5_proc_read, NULL);
}

static const struct file_operations voip_debug5_fops = 
{
	.open = voip_debug5_proc_open,
	.write = voip_debug5_write,
	.read = seq_read,
};
#endif

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_debug_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voip_debug_proc_read, NULL);
}

static const struct file_operations voip_debug_fops = 
{
	.open = voip_debug_proc_open,
	.write = voip_debug_write,
	.read = seq_read,
};
#endif

static int __init voip_proc_debug_init(void)
{
	struct proc_dir_entry *voip_debug_proc;
	struct proc_dir_entry *voip_debug5_proc;

#ifdef CONFIG_LINUX_KERNEL_3_10
	voip_debug_proc = create_voip_proc_rw_entry(PROC_VOIP_DIR "/debug", 0, NULL,
						&voip_debug_fops);
#else
	voip_debug_proc = create_voip_proc_rw_entry(PROC_VOIP_DIR "/debug", 0, NULL,
						voip_debug_read, voip_debug_write);
#endif
	
	if (voip_debug_proc == NULL)
	{
		printk("voip_debug_proc NULL!! \n");
		return -1;
	}
	
#ifdef CONFIG_LINUX_KERNEL_3_10
	voip_debug5_proc = create_voip_proc_rw_entry(PROC_VOIP_DIR "/debug5", 0, NULL,
						&voip_debug5_fops);
#else
	voip_debug5_proc = create_voip_proc_rw_entry(PROC_VOIP_DIR "/debug5", 0, NULL,
						voip_debug5_read, voip_debug5_write);
#endif
	
	if (voip_debug5_proc == NULL)
	{
		printk("voip_debug5_proc NULL!! \n");
		return -1;
	}
	
	return 0;
}

static void __exit voip_proc_debug_exit( void )
{
	remove_voip_proc_entry( PROC_VOIP_DIR "/debug", NULL );
	remove_voip_proc_entry( PROC_VOIP_DIR "/debug5", NULL );
}

voip_initcall_proc( voip_proc_debug_init );
voip_exitcall( voip_proc_debug_exit );

#ifndef CONFIG_PRINTK
// printk wrapper
int __wrap_printk(const char *fmt, ...)
{
#if defined(CONFIG_PANIC_PRINTK) || defined(CONFIG_PRINTK_FUNC)
    va_list args;
    int r;

    va_start(args, fmt);
#ifdef CONFIG_PANIC_PRINTK
    r = vprintk(fmt, args);
#else
	r = scrlog_vprintk(fmt, args);
#endif
    va_end(args);

    return r;
#else
	return 0;
#endif
}
#endif

