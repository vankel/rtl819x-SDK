/*
 *	Realtek RTL8186 PCM Controller Driver Header File
 *
 *	Author : thlin@realtek.com.tw
 *
 *	2005.09.05	
 *
 *	Copyright 2005 Realtek Semiconductor Corp.
 */

#ifndef _PCM_INTERFACE
#define _PCM_INTERFACE

//#include <linux/config.h>
#include <linux/sched.h>
#include "rtk_voip.h"
#include "voip_types.h"

// ========================================================//
/* PCM version definition 
 *   v1.0: old/unknwon version 
 *   v2.0: support 8 channels 
 *   v2.2: (8972B) 
 *   v2.3: add frame cnt (8954C)
 *   v3.0: support 16 channels 
 *   v3.1: wideband ch0~7
 *   v3.3: move COILBE to PCMCR 
 *   v4.0: Add ISI/ZSI loopback mode (Implement 8 channel narrow/wideband only) 
 */
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM8651
#define PCM_VERSION		0x0100		// 1.0 
#elif defined (CONFIG_RTK_VOIP_DRIVERS_PCM8186)
#define PCM_VERSION		0x0100		// 1.0 
#elif defined (CONFIG_RTK_VOIP_DRIVERS_PCM8671) || defined (CONFIG_RTK_VOIP_DRIVERS_PCM8672) || defined (CONFIG_RTK_VOIP_DRIVERS_PCM8676)
#define PCM_VERSION		0x0202		// 2.2 
#elif defined (CONFIG_RTK_VOIP_DRIVERS_PCM865xC)
#define PCM_VERSION		0x0100		// 1.0 
#elif defined (CONFIG_RTK_VOIP_DRIVERS_PCM8972B_FAMILY)
#define PCM_VERSION		0x0202		// 2.2 
#elif defined (CONFIG_RTK_VOIP_DRIVERS_PCM89xxC) || defined (CONFIG_RTK_VOIP_DRIVERS_PCM89xxD)
#define PCM_VERSION		0x0203		// 2.3 
#elif defined (CONFIG_RTK_VOIP_PLATFORM_8686) || defined (CONFIG_RTK_VOIP_DRIVERS_PCM89xxE)
#define PCM_VERSION		0x0400		// 4.0 
#else
#error "Unknown PCM version!!"
#endif

/********************************************************************/
#define MAXCH	4

#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM8651
#define PCM_IRQ	13
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM8186
#define PCM_IRQ	7
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM8671
#define PCM_IRQ	10
#elif defined (CONFIG_RTK_VOIP_DRIVERS_PCM8672) || defined (CONFIG_RTK_VOIP_DRIVERS_PCM8676)
#define PCM_IRQ	19
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM865xC
#define PCM_IRQ	6
#ifdef CONFIG_RTK_VOIP_PULSE_DIAL_GEN_TIMER
#define TIMER1_IRQ 8
#endif
#elif defined(CONFIG_RTK_VOIP_DRIVERS_PCM8972B_FAMILY) || defined(CONFIG_RTK_VOIP_DRIVERS_PCM89xxC) || defined(CONFIG_RTK_VOIP_DRIVERS_PCM89xxD)
#ifdef CONFIG_DEFAULTS_KERNEL_2_6
  #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,30))
#include "bspchip.h"
#define PCM_IRQ	BSP_PCM_IRQ
  #else
  //#error
#define PCM_IRQ	19
  #endif
#ifdef CONFIG_RTK_VOIP_PULSE_DIAL_GEN_TIMER
#define TIMER1_IRQ 	9	/* defined in linux-2.6.x/include/asm/rtl865x/platform.h */
#endif
#else
#define PCM_IRQ	6
#endif
#elif defined( CONFIG_RTK_VOIP_PLATFORM_8686 )
#ifdef CONFIG_RTK_VOIP_IPC_ARCH_DSP_IN_DUAL_LINUX
#define PCM_IRQ 17
#else
#define PCM_IRQ 19	// should match with linux_wrapper 
#endif
#elif defined( CONFIG_RTK_VOIP_DRIVERS_PCM89xxE )
#define PCM_IRQ 19	// should match with linux_wrapper 
#endif

/*  Interrupt Status Type */
#define TOK2DSP		0
#define ROK2DSP		1
#define TUB2DSP		2
#define RUB2DSP		3

/* Interrupt Mask Type */
#define P0OK		0
#define P1OK		1
#define TBUA		2
#define RBUA		3

/* Define the allocated memory size for PCM tx, rx buffer(2 page) */
//#define PCM_SIZE_N		255					//page size = 320, 20ms
//#define PCMPAGE_SIZE	(4*(PCM_SIZE_N + 1))	//one pages, 4(n + 1)
#define PCMPAGE_SIZE	320					// 20ms page 
#define BUFFER_SIZE		(PCMPAGE_SIZE*2)	//two pages


#define pcm_outb(address, value)	writeb(value, (void*)address)
#define pcm_outw(address, value)	writew(value, (void*)address)
#define pcm_outl(address, value)	writel(value, (void*)address)

#define pcm_inb(address)		readb((void*)address)
#define pcm_inw(address)		readw((void*)address)
#define pcm_inl(address)		readl((void*)address)

#ifndef BIT
#define BIT(x)	(1 << x)
#endif
	
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM8651
#ifndef PCM_BASE
#define PCM_BASE (0xbd017000)
#endif
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM8186
#define PCM_BASE (0xbd280000)
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM8671
#define PCM_BASE (0xb8e08000)
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM865xC
#define PCM_BASE (0xb8008000)
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM8972B_FAMILY || defined(CONFIG_RTK_VOIP_DRIVERS_PCM89xxC)
#define PCM_BASE (0xb8008000)
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM89xxD || defined (CONFIG_RTK_VOIP_DRIVERS_PCM89xxE)
#define PCM_BASE (0xb8008000)
#elif defined CONFIG_RTK_VOIP_DRIVERS_PCM8672 || defined (CONFIG_RTK_VOIP_DRIVERS_PCM8676)
#define PCM_BASE (0xb8008000)
#elif defined( CONFIG_RTK_VOIP_PLATFORM_8686 )
#define PCM_BASE (0xb8008000)
#endif

#define PCMCR		(PCM_BASE + 0x00)	//Interface Control Register
#define PCMCHCNR	(PCM_BASE + 0x04)	//Channel specific Control Register
#define PCMTSR		(PCM_BASE + 0x08)	//Time Slot Assignment Register
#define PCMBSIZE	(PCM_BASE + 0x0C)	//Channels Buffer Size register
#define CH0TXBSA	(PCM_BASE + 0x10)	//Channel 0 TX buffer starting address pointer
#define CH1TXBSA	(PCM_BASE + 0x14)	//Channel 1 TX buffer starting address pointer
#define CH2TXBSA	(PCM_BASE + 0x18)	//Channel 2 TX buffer starting address pointer
#define CH3TXBSA	(PCM_BASE + 0x1C)	//Channel 3 TX buffer starting address pointer
#define CH0RXBSA	(PCM_BASE + 0x20)	//Channel 0 RX buffer starting address pointer
#define CH1RXBSA	(PCM_BASE + 0x24)	//Channel 1 RX buffer starting address pointer
#define CH2RXBSA	(PCM_BASE + 0x28)	//Channel 2 RX buffer starting address pointer
#define CH3RXBSA	(PCM_BASE + 0x2C)	//Channel 3 RX buffer starting address pointer
#define PCMIMR		(PCM_BASE + 0x30)	//Channels Interrupt Mask Register
#define PCMISR		(PCM_BASE + 0x34)	//Channels Interrupt Status Register
#define PCMCHCNR47	(PCM_BASE + 0x38)	//Channel 4-7 specific Control Register
#define PCMTSR47	(PCM_BASE + 0x3c)	//Channel 4-7 Time Slot Assignment Register
#define PCMBSIZE47	(PCM_BASE + 0x40)	//Channel 4-7 Buffer Size register
#define	CH4TXBSA	(PCM_BASE + 0x44)	//Channel 4 TX buffer starting address pointer
#define	CH5TXBSA	(PCM_BASE + 0x48)	//Channel 5 TX buffer starting address pointer
#define	CH6TXBSA	(PCM_BASE + 0x4c)	//Channel 6 TX buffer starting address pointer
#define	CH7TXBSA	(PCM_BASE + 0x50)	//Channel 7 TX buffer starting address pointer
#define	CH4RXBSA	(PCM_BASE + 0x54)	//Channel 4 RX buffer starting address pointer
#define	CH5RXBSA	(PCM_BASE + 0x58)	//Channel 5 RX buffer starting address pointer
#define	CH6RXBSA	(PCM_BASE + 0x5c)	//Channel 6 RX buffer starting address pointer
#define	CH7RXBSA	(PCM_BASE + 0x60)	//Channel 7 RX buffer starting address pointer
#define	PCMIMR47	(PCM_BASE + 0x64)	//Channel 4-7 Interrupt Mask Register
#define	PCMISR47	(PCM_BASE + 0x68)	//Channel 4-7 Interrupt Status Register
#if PCM_VERSION == 0x0400
// 8-11
#define PCMCHCNR811	(PCM_BASE + 0x6c)	//Channel 8-11 specific Control Register
#define PCMTSR811	(PCM_BASE + 0x70)	//Channel 8-11 Time Slot Assignment Register
#define PCMBSIZE811	(PCM_BASE + 0x74)	//Channel 8-11 Buffer Size register
#define	CH8TXBSA	(PCM_BASE + 0x78)	//Channel 8 TX buffer starting address pointer
#define	CH9TXBSA	(PCM_BASE + 0x7c)	//Channel 9 TX buffer starting address pointer
#define	CH10TXBSA	(PCM_BASE + 0x80)	//Channel 10 TX buffer starting address pointer
#define	CH11TXBSA	(PCM_BASE + 0x84)	//Channel 11 TX buffer starting address pointer
#define	CH8RXBSA	(PCM_BASE + 0x88)	//Channel 8 RX buffer starting address pointer
#define	CH9RXBSA	(PCM_BASE + 0x8c)	//Channel 9 RX buffer starting address pointer
#define	CH10RXBSA	(PCM_BASE + 0x90)	//Channel 10 RX buffer starting address pointer
#define	CH11RXBSA	(PCM_BASE + 0x94)	//Channel 11 RX buffer starting address pointer
#define	PCMIMR811	(PCM_BASE + 0x98)	//Channel 8-11 Interrupt Mask Register
#define	PCMISR811	(PCM_BASE + 0x9c)	//Channel 8-11 Interrupt Status Register
// 12-15
#define PCMCHCNR1215	(PCM_BASE + 0xa0)	//Channel 12-15 specific Control Register
#define PCMTSR1215		(PCM_BASE + 0xa4)	//Channel 12-15 Time Slot Assignment Register
#define PCMBSIZE1215	(PCM_BASE + 0xa8)	//Channel 12-15 Buffer Size register
#define	CH12TXBSA	(PCM_BASE + 0xac)	//Channel 12 TX buffer starting address pointer
#define	CH13TXBSA	(PCM_BASE + 0xb0)	//Channel 13 TX buffer starting address pointer
#define	CH14TXBSA	(PCM_BASE + 0xb4)	//Channel 14 TX buffer starting address pointer
#define	CH15TXBSA	(PCM_BASE + 0xb8)	//Channel 15 TX buffer starting address pointer
#define	CH12RXBSA	(PCM_BASE + 0xbc)	//Channel 12 RX buffer starting address pointer
#define	CH13RXBSA	(PCM_BASE + 0xc0)	//Channel 13 RX buffer starting address pointer
#define	CH14RXBSA	(PCM_BASE + 0xc4)	//Channel 14 RX buffer starting address pointer
#define	CH15RXBSA	(PCM_BASE + 0xc8)	//Channel 15 RX buffer starting address pointer
#define	PCMIMR1215	(PCM_BASE + 0xcc)	//Channel 12-15 Interrupt Mask Register
#define	PCMISR1215	(PCM_BASE + 0xd0)	//Channel 12-15 Interrupt Status Register
// others 
#define PCMINTMAP	(PCM_BASE + 0xd4)	//Interrupt mapping 
#define PCMWTSR03	(PCM_BASE + 0xd8)	// Channel 0-3 wideband time slot assignment 
#define PCMWTSR47	(PCM_BASE + 0xdc)	// Channel 4-7 wideband time slot assignment 
#define PCMBUFOWCHK	(PCM_BASE + 0xe0)	// RX buffer data overwrite indicate 
#endif // PCM_VERSION == 0x0400

#define TX_BSA(channel)	(CH0TXBSA + 4*(channel))
#define RX_BSA(channel)	(CH0RXBSA + 4*(channel))
#define	CH47TX_BSA(channel)	(CH4TXBSA + 4*((channel&3)))
#define	CH47RX_BSA(channel)	(CH4RXBSA + 4*((channel&3)))


//PCMCR
#if PCM_VERSION == 0x0400
#define ISILBE		BIT(16)
#define ZSILBE		BIT(15)
#define C0ILBE		BIT(14)
#endif
#define PCMLM		BIT(13)	// PCM linear mode enable
#define PCME		BIT(12)
#if PCM_VERSION >= 0x0200 && PCM_VERSION <= 0x0400	// 2.0  ~ 4.0 
#define PCMCLK		BIT(11)
#elif PCM_VERSION == 0x0100	// 1.0 
#define CKDIR		BIT(11)
#endif
#if PCM_VERSION == 0x0100	// 1.0 
#define PXDSE		BIT(10)
#endif
#define FSINV		BIT(9)

#if PCM_VERSION >= 0x0200 && PCM_VERSION <= 0x0400	// 2.0  ~ 4.0 
#define PCM_ENABLE		(PCME | PCMCLK) 
#elif PCM_VERSION == 0x0100	// 1.0 
#define PCM_ENABLE		(PCME | CKDIR)
#endif 

//PCMCHCNR
#define CH3RE		BIT(0)
#define CH3TE		BIT(1)
#define CH3UA		BIT(2)
#define C3CMPE		BIT(3)

#define CH2RE		BIT(8)
#define CH2TE		BIT(9)
#define CH2UA		BIT(10)
#define C2CMPE		BIT(11)

#define CH1RE		BIT(16)
#define CH1TE		BIT(17)
#define CH1UA		BIT(18)
#define C1CMPE		BIT(19)

#define CH0RE		BIT(24)
#define CH0TE		BIT(25)
#define CH0UA		BIT(26)
#define C0CMPE		BIT(27)
#if PCM_VERSION < 0x0400
#define C0ILBE		BIT(28)
#endif

#if PCM_VERSION >= 0x0200 && PCM_VERSION <= 0x0400	// 2.0  ~ 4.0 
#define CH0TXP0IP 	BIT(31)
#define CH0TXP1IP 	BIT(30)
#define CH0RXP0IP 	BIT(29)
#define CH0RXP1P 	BIT(28)
#define CH0TXP0UA 	BIT(27)
#define CH0TXP1UA 	BIT(26)
#define CH0RXP0UA 	BIT(25)
#define CH0RXP1UA 	BIT(24)

#define CH1TXP0IP 	BIT(23)
#define CH1TXP1IP 	BIT(22)
#define CH1RXP0IP 	BIT(21)
#define CH1RXP1P 	BIT(20)
#define CH1TXP0UA 	BIT(19)
#define CH1TXP1UA 	BIT(18)
#define CH1RXP0UA 	BIT(17)
#define CH1RXP1UA 	BIT(16)

#define CH2TXP0IP 	BIT(15)
#define CH2TXP1IP 	BIT(14)
#define CH2RXP0IP 	BIT(13)
#define CH2RXP1P 	BIT(12)
#define CH2TXP0UA 	BIT(11)
#define CH2TXP1UA 	BIT(10)
#define CH2RXP0UA 	BIT(9)
#define CH2RXP1UA 	BIT(8)

#define CH3TXP0IP 	BIT(7)
#define CH3TXP1IP 	BIT(6)
#define CH3RXP0IP 	BIT(5)
#define CH3RXP1P 	BIT(4)
#define CH3TXP0UA 	BIT(3)
#define CH3TXP1UA 	BIT(2)
#define CH3RXP0UA 	BIT(1)
#define CH3RXP1UA 	BIT(0)

#elif PCM_VERSION == 0x0100	// 1.0  
//PCMIMR
#define CH0P0OKIE 	BIT(15)
#define CH0P1OKIE 	BIT(14)
#define CH0TBUAIE 	BIT(13)
#define CH0RBUAIE 	BIT(12)

#define CH1P0OKIE 	BIT(11)
#define CH1P1OKIE 	BIT(10)
#define CH1TBUAIE 	BIT(9)
#define CH1RBUAIE 	BIT(8)

#define CH2P0OKIE 	BIT(7)
#define CH2P1OKIE 	BIT(6)
#define CH2TBUAIE 	BIT(5)
#define CH2RBUAIE 	BIT(4)

#define CH3P0OKIE 	BIT(3)
#define CH3P1OKIE 	BIT(2)
#define CH3TBUAIE 	BIT(1)
#define CH3RBUAIE 	BIT(0)
#endif 

//PCMISR
#define CH3P1RBU	BIT(0)
#define CH3P0RBU	BIT(1)
#define CH3P1TBU	BIT(2)
#define CH3P0TBU	BIT(3)
#define CH3P1ROK	BIT(4)
#define CH3P0ROK	BIT(5)
#define CH3P1TOK	BIT(6)
#define CH3P0TOK	BIT(7)

#define CH2P1RBU	BIT(8)
#define CH2P0RBU	BIT(9)
#define CH2P1TBU	BIT(10)
#define CH2P0TBU	BIT(11)
#define CH2P1ROK	BIT(12)
#define CH2P0ROK	BIT(13)
#define CH2P1TOK	BIT(14)
#define CH2P0TOK	BIT(15)

#define CH1P1RBU	BIT(16)
#define CH1P0RBU	BIT(17)
#define CH1P1TBU	BIT(18)
#define CH1P0TBU	BIT(19)
#define CH1P1ROK	BIT(20)
#define CH1P0ROK	BIT(21)
#define CH1P1TOK	BIT(22)
#define CH1P0TOK	BIT(23)

#define CH0P1RBU	BIT(24)
#define CH0P0RBU	BIT(25)
#define CH0P1TBU	BIT(26)
#define CH0P0TBU	BIT(27)
#define CH0P1ROK	BIT(28)
#define CH0P0ROK	BIT(29)
#define CH0P1TOK	BIT(30)
#define CH0P0TOK	BIT(31)

//PCMISR 4-7
#define CH7P1RBU	BIT(0)
#define CH7P0RBU	BIT(1)
#define CH7P1TBU	BIT(2)
#define CH7P0TBU	BIT(3)
#define CH7P1ROK	BIT(4)
#define CH7P0ROK	BIT(5)
#define CH7P1TOK	BIT(6)
#define CH7P0TOK	BIT(7)

#define CH6P1RBU	BIT(8)
#define CH6P0RBU	BIT(9)
#define CH6P1TBU	BIT(10)
#define CH6P0TBU	BIT(11)
#define CH6P1ROK	BIT(12)
#define CH6P0ROK	BIT(13)
#define CH6P1TOK	BIT(14)
#define CH6P0TOK	BIT(15)

#define CH5P1RBU	BIT(16)
#define CH5P0RBU	BIT(17)
#define CH5P1TBU	BIT(18)
#define CH5P0TBU	BIT(19)
#define CH5P1ROK	BIT(20)
#define CH5P0ROK	BIT(21)
#define CH5P1TOK	BIT(22)
#define CH5P0TOK	BIT(23)

#define CH4P1RBU	BIT(24)
#define CH4P0RBU	BIT(25)
#define CH4P1TBU	BIT(26)
#define CH4P0TBU	BIT(27)
#define CH4P1ROK	BIT(28)
#define CH4P0ROK	BIT(29)
#define CH4P1TOK	BIT(30)
#define CH4P0TOK	BIT(31)

//----------------------------------------------------

//#define BSIZE(channel, size)	((size << 24) >> (8*channel))	
#define BSIZE(channel, size)	( (size & 0xFF) << ((3-channel)*8))

#define P1RBU(channel)		(CH0P1RBU >> (8*(channel&3)))
#define P0RBU(channel)		(CH0P0RBU >> (8*(channel&3)))
#define P1TBU(channel)		(CH0P1TBU >> (8*(channel&3)))
#define P0TBU(channel)		(CH0P0TBU >> (8*(channel&3)))
#define P1ROK(channel)		(CH0P1ROK >> (8*(channel&3)))
#define P0ROK(channel)		(CH0P0ROK >> (8*(channel&3)))
#define P1TOK(channel)		(CH0P1TOK >> (8*(channel&3)))
#define P0TOK(channel)		(CH0P0TOK >> (8*(channel&3)))

#define ISR_MASK(channel)	(0xFF000000 >> (8*(channel&3)))
#define TOK_MASK(channel)	(0xC0000000 >> (8*(channel&3)))
#define ROK_MASK(channel)	(0x30000000 >> (8*(channel&3)))
#define TBU_MASK(channel)	( 0xC000000 >> (8*(channel&3)))
#define RBU_MASK(channel)	( 0x3000000 >> (8*(channel&3)))

#define CHxRE(channel)		(CH0RE   >> (8*channel))
#define CHxTE(channel)		(CH0TE   >> (8*channel))
#define CHxUA(channel)		(CH0UA   >> (8*channel))
#define CxCMPE(channel)		(C0CMPE  >> (8*channel))

#define CHxP0OKIE(channel)	(CH0P0OKIE >> (4*channel))
#define CHxP1OKIE(channel)	(CH0P1OKIE >> (4*channel))
#define CHxTBUAIE(channel)	(CH0TBUAIE >> (4*channel))
#define CHxRBUAIE(channel)	(CH0RBUAIE >> (4*channel))

#define NAME_SIZE	10
//#define BOOL	unsigned char

struct pcm_priv{
#if 0
	unsigned int chidannel;
	unsigned int txpindex;		//tx page index.
	unsigned int rxpindex;		//rx page index
	unsigned int rx_pagepointer;	//used for read. use this to move in the circular buffer page
	unsigned int tx_pagepointer;	//used for write.
#endif
	char name[NAME_SIZE];
	unsigned char* tx_buffer;	//buffer address of non-cached address
	unsigned char* rx_buffer;	//buffer address of non-cached address
	unsigned char* tx_allocated;	//buffer for tx - virtual address
	unsigned char* rx_allocated;	//buffer for rx - virtual address
#if 0
	unsigned char* rx_circular;	//rx circular buffer.
	unsigned char* tx_circular;	//tx circular buffer.

	unsigned int rx_W;		//rx write index
	unsigned int rx_R;		//rx read index
	unsigned int tx_W;		//tx write index
	unsigned int tx_R;		//rx read index
	BOOL rx_overflow;		//rx circular buffer overflow.
        unsigned int chidCNRValue;        //a-law, i-law or linear mode. tx or rx enable.

	unsigned int mode;		//operation mode - linear, a-law or u-law
	
	wait_queue_head_t	wait_qr;
	wait_queue_head_t	wait_qw;

#ifdef USE_TASKLET
	struct tasklet_struct	rx_tasklet;
	struct tasklet_struct	tx_tasklet;
#endif

	//single-open
	spinlock_t	open_lock;
	unsigned int 	open_count;	//open count

#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM8186
	struct fasync_struct *pcm_fasync;    // asynch notification
#endif
#endif

	// cache. provide quick access 
	uint32 page_size;
};

typedef struct pcm_priv pcm_priv_t;


//---------------------------------------------------------------------------------------------
//	Debug
//---------------------------------------------------------------------------------------------

//#define PCM_DEBUG

#undef PDBUG

#ifdef PCM_DEBUG
	#define PDBUG(fmt, args...) printk("-%s:" fmt, __FUNCTION__, ## args)
#else
	#define PDBUG(fmt, args...)
#endif


#define PERR(fmt, args...)	printk(KERN_ERR "PCM - " fmt, ## args)

#endif
