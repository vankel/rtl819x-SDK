


#ifndef __R39XX_H__
#define __R39XX_H__

#include <asm/addrspace.h>

#if 0	//wei add
#define Virtual2Physical(x)		(((int)x) & 0x1fffffff)
#define Physical2Virtual(x)		(((int)x) | 0x80000000)
#define Virtual2NonCache(x)		(((int)x) | 0x20000000)
#define Physical2NonCache(x)		(((int)x) | 0xa0000000)

#define RTL8181_REG_BASE (0xBD010000)

#define rtl_inb(offset) (*(volatile unsigned char *)(RTL8181_REG_BASE+offset))
#define rtl_inw(offset) (*(volatile unsigned short *)(RTL8181_REG_BASE+offset))
#define rtl_inl(offset) (*(volatile unsigned long *)(RTL8181_REG_BASE+offset))

#define rtl_outb(offset,val)	(*(volatile unsigned char *)(RTL8181_REG_BASE+offset) = val)
#define rtl_outw(offset,val)	(*(volatile unsigned short *)(RTL8181_REG_BASE+ offset) = val)
#define rtl_outl(offset,val)	(*(volatile unsigned long *)(RTL8181_REG_BASE+offset) = val)


#ifdef BIT
#error	"BIT define occurred earlier elsewhere!\n"
#endif

#define BIT(x)	( 1 << (x))


// For Uart1 Controller
#define UART_RBR	0xC3
#define UART_THR	0xC3
#define UART_DLL	0xC3
#define UART_IER	0xC7
#define UART_DLM	0xC7
#define UART_IIR	0xCB
#define UART_FCR	0xCB
#define UART_LCR	0xCF
#define UART_MCR	0xD3
#define UART_LSR	0xD7
#define UART_MSR	0xDB
#define UART_SCR	0xDF

// For Uart1 Flags
#define UART_RXFULL	 (1 << 0)
#define UART_TXEMPTY	((1 << 6) | (1 << 5))
#define UART_RXFULL_MASK	(1 << 0)
#define UART_TXEMPTY_MASK	( 1 << 1)


// For Interrupt Controller
#define GIMR0		0x00
#define GISR		0x04
#define IRR0		0x08


// For General Purpose Timer/Counter

#define TC0DATA		0x60
#define TC1DATA		0x64
#define TC2DATA		0x68
#define TC3DATA		0x6C
#define TC0CNT		0x70
#define TC1CNT		0x74
#define TC2CNT		0x78
#define TC3CNT		0x7C
#define TCCNR		0x50
#define TCIR		0x54
#define BTDATA		0x58
#define WDTCNR		0x5C




// For NIC

#define NIC_CNR1	0x000
#define NIC_ID		0x004
#define NIC_MAR		0x00C
#define NIC_TSAD	0x014
#define NIC_RSAD	0x018
#define NIC_IMTR	0x01C
#define NIC_IMR		0x020
#define NIC_ISR		0x024
#define NIC_TMF0	0x028
#define NIC_TMF1	0x02C
#define NIC_TMF2	0x030
#define NIC_TMF3	0x034
#define NIC_MII		0x038
#define NIC_CNR2	0x03C
#define NIC_MPC		0x080
#define NIC_TXCOL	0x084
#define NIC_RXERR	0x088
#define NIC_BIST	0x08C
#endif


#define WLAN_MAC0		(0x3f0000 +0x00)
#define WLAN_MAC1		(0x3f0000 +0x04)

#define WLAN_MAR0		(0x3f0000 +0x08)
#define WLAN_MAR1		(0x3f0000 +0x09)
#define WLAN_MAR2		(0x3f0000 +0x0A)
#define WLAN_MAR3		(0x3f0000 +0x0B)
#define WLAN_MAR4		(0x3f0000 +0x0C)
#define WLAN_MAR5		(0x3f0000 +0x0D)
#define WLAN_MAR6		(0x3f0000 +0x0E)
#define WLAN_MAR7		(0x3f0000 +0x0F)

#define	WLAN_TLPDS		(0x3f0000 +0x20)
#define WLAN_TNPDS		(0x3f0000 +0x24)
#define	WLAN_THPDS		(0x3f0000 +0x28)
#define WLAN_BRSR		(0x3f0000 +0x2C)
#define WLAN_BSSID		(0x3f0000 +0x2E)
#define WLAN_BSSID2		(0x3f0000 +0x30)
#define WLAN_CR			(0x3f0000 +0x37)
#define	WLAN_IMR		(0x3f0000 +0x3C)
#define	WLAN_ISR		(0x3f0000 +0x3E)
#define WLAN_TCR		(0x3f0000 +0x40)
#define	WLAN_RCR		(0x3f0000 +0x44)
#define	WLAN_TBDR		(0x3f0000 +0x4C)
#define	WLAN_9346CR		(0x3f0000 +0x50)
#define WLAN_CONFIG0	(0x3f0000 +0x51)
#define WLAN_CONFIG1	(0x3f0000 +0x52)
#define WLAN_CONFIG2	(0x3f0000 +0x53)
#define WLAN_PHYPara	(0x3f0000 +0x54)
#define WLAN_MSR		(0x3f0000 +0x58)
#define WLAN_CONFIG3	(0x3f0000 +0x59)
#define WLAN_CONFIG4	(0x3f0000 +0x5A)
#define WLAN_TESTMODE	(0x3f0000 +0x5B)
#define WLAN_SCR		(0x3f0000 +0x5F)
#define WLAN_DTIMPRD	(0x3f0000 +0x6F)	
#define WLAN_BCNITV		(0x3f0000 +0x70)
#define WLAN_ATIMWND	(0x3f0000 +0x72)
#define WLAN_BINTRITV	(0x3f0000 +0x74)
#define WLAN_ATIMTRITV	(0x3f0000 +0x76)
#define WLAN_PHYDelay	(0x3f0000 +0x78)
#define WLAN_BBValue	(0x3f0000 +0x7C)
#define WLAN_RFValue	(0x3f0000 +0x80)
#define WLAN_DK0		(0x3f0000 +0x90)
#define WLAN_DK1		(0x3f0000 +0xA0)
#define WLAN_DK2		(0x3f0000 +0xB0)
#define WLAN_DK3		(0x3f0000 +0xC0)


#define WLAN_CONFIG5	(0x3f0000 +0xD8)
#define WLAN_TPPOLL		(0x3f0000 +0xD9)
#define WLAN_CWR		(0x3f0000 +0xDC)
#define WLAN_RETRYCTR	(0x3f0000 +0xDE)
#define WLAN_RDSAR		(0x3f0000 +0xE4)


#define	WLAN_BIST		(0x3f0000 +0x8C)

#define	WLAN_KeyAdd		(0x3f0000 +0x00)
#define	WLAN_KeyVal		(0x3f0000 +0x06)
#define	WLAN_KeyConf	(0x3f0000 +0x16)

#define WLAN_TRIG		(0x3f0000 +0x00)

#endif
