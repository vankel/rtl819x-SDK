



/*
 * Register access macro
 */
#ifndef REG32
#define REG32(reg)		(*(volatile unsigned int   *)(reg))
#endif
#ifndef REG16
#define REG16(reg)		(*(volatile unsigned short *)(reg))
#endif
#ifndef REG08
#define REG08(reg)		(*(volatile unsigned char  *)(reg))
#endif

/*
 * SPRAM
 */
#define BSP_ISPRAM_BASE		0x0
#define BSP_DSPRAM_BASE		0x0

/*
 * GCMP Specific definitions
 */
#define GCMP_BASE_ADDR		0xbfbf8000
#define GCMP_BASE_SIZE		(256 * 1024)

/*
 * GIC Specific definitions
 */
#define GIC_BASE_ADDR		0xbbdc0000
#define GIC_BASE_SIZE		(128 * 1024)

/*
 * CPC Specific defiitions
 */
#define CPC_BASE_ADDR		0xbbde0000
#define CPC_BASE_SIZE		(24 * 1024)



//================================
#define GCR_BASE_ADDR GCMP_BASE_ADDR

#define Virtual2Physical(x)		(((unsigned int)x) & 0x1fffffff)
#define Physical2Virtual(x)		(((unsigned int)x) | 0x80000000)
#define Virtual2NonCache(x)		(((unsigned int)x) | 0x20000000)
#define Physical2NonCache(x)		(((unsigned int)x) | 0xa0000000)
#define UNCACHE_MASK			0x20000000  


#define SPECIAL_EHB() __asm__ volatile(".word 0x000000c0")
#define SPECIAL_ERET() __asm__ volatile(".word 0x42000018")