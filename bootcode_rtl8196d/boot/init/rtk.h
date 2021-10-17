#ifndef __RTK_H__
#define __RTK_H__

// david ----------------------------------------------------------------
#if defined(CONFIG_RTL8196E_KLD)
	#define FW_SIGNATURE			((char *)"klln")	// fw signature
	#define FW_SIGNATURE_WITH_ROOT	((char *)"k6ln")	// fw signature with root fs
	#define ROOT_SIGNATURE          ((char *)"k6rt")
#elif defined(CONFIG_NFBI) || defined(CONFIG_NONE_FLASH)
	#define FW_SIGNATURE		((char *)"csys")	// fw signature
	#define FW_SIGNATURE_WITH_ROOT	((char *)"csro")	// fw signature with root fs
	#define ROOT_SIGNATURE          ((char *)"root")
#elif defined(CONFIG_OSK)
	#define FW_SIGNATURE		((char *)"OSKR")	// fw signature
#elif defined(RTL8198)
	#define FW_SIGNATURE			((char *)"cs6c")	// fw signature
	#define FW_SIGNATURE_WITH_ROOT	((char *)"cr6c")	// fw signature with root fs
#if defined(CONFIG_WEBPAGE_CHECK)
	#define WEBPAGE_SIGNATURE			((char *)"w6cg")	// webpage signature
#endif
	#define ROOT_SIGNATURE          ((char *)"r6cr")
#else
	#define FW_SIGNATURE			((char *)"csys")	// fw signature
	#define FW_SIGNATURE_WITH_ROOT	((char *)"csro")	// fw signature with root fs
	#define ROOT_SIGNATURE          ((char *)"root")
#endif

#define SQSH_SIGNATURE		((char *)"sqsh")
#define SQSH_SIGNATURE_LE       ((char *)"hsqs")

#if defined(RTL8198)
#define WEB_SIGNATURE		((char *)"w6cp")
#define WEB_JFFS2_SIGNATURE    ((char *)"jw6c")
#else
#define WEB_SIGNATURE		((char *)"webp")
#define WEB_JFFS2_SIGNATURE    ((char *)"jweb")
#endif

//BUILDIN_SECURITY_FILE
#define CWMP_SIGNATURE ((char *)"cwmp")
#define KSAP_SIGNATURE	((char *)"ksap")
//BUILDIN_SECURITY_FILE

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


#define READ_LINUX_ONCE 1


#endif

