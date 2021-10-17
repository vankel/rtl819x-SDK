#ifndef __RTK_H__
#define __RTK_H__

// david ----------------------------------------------------------------
#if defined(CONFIG_NFBI) || defined(CONFIG_NONE_FLASH)
	#define FW_SIGNATURE		((char *)"csys")	// fw signature
	#define FW_SIGNATURE_WITH_ROOT	((char *)"csro")	// fw signature with root fs
	#define ROOT_SIGNATURE          ((char *)"root")
#elif defined(RTL8198)
	#define FW_SIGNATURE			((char *)"cs6c")	// fw signature
	#define FW_SIGNATURE_WITH_ROOT	((char *)"cr6c")	// fw signature with root fs
	#define ROOT_SIGNATURE          ((char *)"r6cr")
#if defined(CONFIG_WEBPAGE_BACKUP)
	#define WEBPAGE_SIGNATURE			((char *)"w6cg")	// webpage signature
#endif
#else
	#define FW_SIGNATURE			((char *)"csys")	// fw signature
	#define FW_SIGNATURE_WITH_ROOT	((char *)"csro")	// fw signature with root fs
	#define ROOT_SIGNATURE          ((char *)"root")
#endif

#if defined(CONFIG_TFTP_COMMAND)
#define COMMAND_SIGNATURE  ((char *)"cmd ")
#endif
#define SQSH_SIGNATURE		((char *)"sqsh")
#define SQSH_SIGNATURE_LE       ((char *)"hsqs")

#if defined(RTL8198)
#define WEB_SIGNATURE		((char *)"w6cp")
#else
#define WEB_SIGNATURE		((char *)"webp")
#endif

#define BOOT_SIGNATURE		((char *)"boot")
#define ALL1_SIGNATURE		((char *)"ALL1")
#define ALL2_SIGNATURE		((char *)"ALL2")

#define HW_SETTING_OFFSET		0x6000

// Cyrus ----------------------------------------------------------------
#define HW_SIGNATURE		((char *)"HS")	// hw signature
#define SW_SIGNATURE_D		((char *)"DS")	// sw_default signature
#define SW_SIGNATURE_C		((char *)"CS")	// sw_current signature
#define SIG_LEN			4

/* Firmware image header */
typedef struct _header_ {
	unsigned char signature[SIG_LEN];
	unsigned long startAddr;
	unsigned long burnAddr;
	unsigned long len;
} IMG_HEADER_T, *IMG_HEADER_Tp;

typedef struct _signature__ {
	unsigned char *signature;
	unsigned char *comment ;
	int sig_len;
	int skip ;
	int maxSize;
	int reboot;
} SIGN_T ;

#define SIZE_OF_SQFS_SUPER_BLOCK 640
#define SIZE_OF_CHECKSUM 2
#define OFFSET_OF_LEN 2


//--------------------------------------------------------------------
//#define CHECK_BURN_SERIAL 1

#if 0 //debug print for CHECK_BURN_SERIAL
  #define BDBG_BSN(format, arg...) 	  \
		  printf(format , ## arg)
#else
  #define BDBG_BSN(format, arg...)
#endif


#if CHECK_BURN_SERIAL
#define SQUASHFS_MAGIC			0x73717368
#define SQUASHFS_MAGIC_SWAP 	0x68737173

struct squashfs_super_block_partial {
	unsigned int		s_magic;
	unsigned int		inodes;
	unsigned int		mkfs_time /* time of filesystem creation */;
	unsigned int		block_size;
#if 0
	unsigned int		fragments;
	unsigned short		compression;
	unsigned short		block_log;
	unsigned short		flags;
	unsigned short		no_ids;
	unsigned short		s_major;
	unsigned short		s_minor;
	long long		root_inode;//squashfs_inode_t	root_inode;
	long long		bytes_used;
	long long		id_table_start;
	long long		xattr_table_start;
	long long		inode_table_start;
	long long		directory_table_start;
	long long		fragment_table_start;
	long long		lookup_table_start;
#endif
};

struct _rootfs_padding {
	unsigned char  zero_pad[2];
	unsigned short chksum;
	unsigned char  signature[SIG_LEN];
	unsigned long  len;
} __attribute__ ((packed));
#endif /* #if CHECK_BURN_SERIAL */
//--------------------------------------------------------------------


#endif

