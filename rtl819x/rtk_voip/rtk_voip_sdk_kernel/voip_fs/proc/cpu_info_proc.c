#include "voip_types.h"
#include "voip_init.h"
#include "voip_proc.h"

#define IMPLEMENT_MFC3( reg, x )			\
	if( reg == x ) {						\
		__asm__ __volatile__ 				\
		(	"mfc3	%0, $" #x "\t\n" 		\
			: "=r" (val)	/* output */	\
			: 								\
			: "0"			/* clobbered */	\
		);									\
	} else


static uint32 mfc3_c( int reg )
{
	uint32 val;
	
	//val = 0x002b8000;
	//__asm__ __volatile__ ( "mtc3	%0,$4\t\n" : : "r" (val) );
	//__asm__ __volatile__ ( "mtc3	%0,$5\t\n" : : "r" (val) );
	
	IMPLEMENT_MFC3( reg, 0 )
	IMPLEMENT_MFC3( reg, 1 )
	IMPLEMENT_MFC3( reg, 2 )
	IMPLEMENT_MFC3( reg, 3 )
	IMPLEMENT_MFC3( reg, 4 )
	IMPLEMENT_MFC3( reg, 5 )
	IMPLEMENT_MFC3( reg, 6 )
	IMPLEMENT_MFC3( reg, 7 )
	{
		val = 0xFFFFFFFF;
	}
	
	return val;
}

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_cpu_info_proc_read( struct seq_file *m, void *data )
{
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxC
#define OR_MEM_BITS		0x03FFUL
#else
#define OR_MEM_BITS		0x0000UL
#endif

	static const char * const names[] = {
		"IW base0",
		"IW top 0",
		"IW base1",
		"IW top 1",
		"DW base0",
		"DW top 0",
		"DW base1",
		"DW top 1",
	};
	
	int i;

	if( m->index ) {	/* In our case, we write out all data at once. */
		return 0;
	}
	
	seq_printf( m, "B8000000 = %08lX\n", *( ( unsigned long * )0xb8000000 ) );
	seq_printf( m, "B8000040 = %08lX\n", *( ( unsigned long * )0xb8000040 ) );
	
	for( i = 0; i < 8; i ++ )
		seq_printf( m, "%s = %08lx\n", names[ i ], mfc3_c( i ) | ( ( i & 0x01 ) ? OR_MEM_BITS : 0 ) );
	
	return 0;
}
#else
static int voip_cpu_info_read_proc( char *buf, char **start, off_t off, int count, int *eof, void *data )
{
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxC
#define OR_MEM_BITS		0x03FFUL
#else
#define OR_MEM_BITS		0x0000UL
#endif

	static const char * const names[] = {
		"IW base0",
		"IW top 0",
		"IW base1",
		"IW top 1",
		"DW base0",
		"DW top 0",
		"DW base1",
		"DW top 1",
	};
	
	int n = 0;
	int i;

	if( off ) {	/* In our case, we write out all data at once. */
		*eof = 1;
		return 0;
	}
	
	n += sprintf( buf + n, "B8000000 = %08lX\n", *( ( unsigned long * )0xb8000000 ) );
	n += sprintf( buf + n, "B8000040 = %08lX\n", *( ( unsigned long * )0xb8000040 ) );
	n += sprintf( buf + n, "B8000044 = %08lX\n", *( ( unsigned long * )0xb8000044 ) );
	
	for( i = 0; i < 8; i ++ )
		n += sprintf( buf + n, "%s = %08lx\n", names[ i ], mfc3_c( i ) | ( ( i & 0x01 ) ? OR_MEM_BITS : 0 ) );
	
	*eof = 1;
	return n;
}
#endif

#ifdef CONFIG_LINUX_KERNEL_3_10
static int voip_cpu_info_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, voip_cpu_info_proc_read, NULL);
}

static const struct file_operations voip_cpu_info_fops = 
{
	.open = voip_cpu_info_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
};
#endif

int __init voip_cpu_info_proc_init( void )
{
#ifdef CONFIG_LINUX_KERNEL_3_10
	proc_create_data( PROC_VOIP_DIR "/cpu", 0, NULL, &voip_cpu_info_fops, NULL );
#else
	create_proc_read_entry( PROC_VOIP_DIR "/cpu", 0, NULL, voip_cpu_info_read_proc, NULL );
#endif

	return 0;
}

void __exit voip_cpu_info_proc_exit( void )
{
	remove_voip_proc_entry( PROC_VOIP_DIR "/cpu", NULL );
}

voip_initcall_proc( voip_cpu_info_proc_init );
voip_exitcall( voip_cpu_info_proc_exit );

