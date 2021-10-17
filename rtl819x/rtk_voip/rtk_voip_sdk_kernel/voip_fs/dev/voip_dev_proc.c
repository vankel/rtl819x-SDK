#include "voip_init.h"
#include "voip_proc.h"

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_devices_proc_read( struct seq_file *m, void *data )
{
	extern int sprintf_voip_dev_entry( struct seq_file *m, const char *format );
	
	seq_printf( m, "minor nr name\n" );
	
	sprintf_voip_dev_entry( m, "%4d  %2d %s\n" );

	return 0;
}
#else
static int voip_devices_read_proc( char *buf, char **start, off_t off, int count, int *eof, void *data )
{
	extern int sprintf_voip_dev_entry( char *buff, const char *format );
	int len = 0;
	
	len += sprintf( buf + len, "minor nr name\n" );
	
	len += sprintf_voip_dev_entry( buf + len, "%4d  %2d %s\n" );

	*eof = 1;	
	return len;
}
#endif

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_devices_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voip_devices_proc_read, NULL);
}

static const struct file_operations voip_devices_fops = 
{
	.open = voip_devices_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
};
#endif

static int __init voip_devices_proc_init( void )
{
#ifdef CONFIG_LINUX_KERNEL_3_10
	proc_create_data( PROC_VOIP_DIR "/devices", 0, NULL, &voip_devices_fops, NULL );
#else
	create_proc_read_entry( PROC_VOIP_DIR "/devices", 0, NULL, voip_devices_read_proc, NULL );
#endif
	
	return 0;
}

static void __exit voip_devices_proc_exit( void )
{
	remove_voip_proc_entry( PROC_VOIP_DIR "/devices", NULL );
}

voip_initcall_proc( voip_devices_proc_init );
voip_exitcall( voip_devices_proc_exit );

