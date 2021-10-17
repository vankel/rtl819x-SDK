/*
 * Realtek Semiconductor Corp.
 *
 * bsp/setup.c
 *     bsp interrult initialization and handler code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/console.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/netdevice.h>
#include <linux/rtnetlink.h>

#include <asm/addrspace.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <asm/time.h>
#include <asm/reboot.h>

#include "bspchip.h"

#if defined(CONFIG_RTL_8198C)
#define SHUTDOWN_BR_DEV_IN_IRQ_ENABLED	1
#endif

#if defined(SHUTDOWN_BR_DEV_IN_IRQ_ENABLED)
#include <uapi/linux/if.h>
#endif

extern int bsp_swcore_init(unsigned int version);

#ifndef REG8
#define REG8(reg) 				(*((volatile unsigned char *)(reg)))
#endif

void prom_putchar(char c)
{
#define UART0_BASE		0xB8002000
#define UART0_THR		(UART0_BASE + 0x000)
#define UART0_FCR		(UART0_BASE + 0x008)
#define UART0_LSR       (UART0_BASE + 0x014)
#define TXRST			0x04
#define CHAR_TRIGGER_14	0xC0
#define LSR_THRE		0x20
#define TxCHAR_AVAIL	0x00
#define TxCHAR_EMPTY	0x20

	unsigned int busy_cnt = 0;

	do
	{
		/* Prevent Hanging */
		if (busy_cnt++ >= 30000)
		{
			/* Reset Tx FIFO */
			REG8(UART0_FCR) = TXRST | CHAR_TRIGGER_14;
			return;
		}
	} while ((REG8(UART0_LSR) & LSR_THRE) == TxCHAR_AVAIL);

	/* Send Character */
	REG8(UART0_THR) = c;
	return;
}

static void shutdown_netdev(void)
{
	struct net_device *dev;

	printk("Shutdown network interface\n");
	read_lock(&dev_base_lock);

	for_each_netdev(&init_net, dev)
	{
		if(dev->flags &IFF_UP) 
		{
			rtnl_lock();
			
#if defined(SHUTDOWN_BR_DEV_IN_IRQ_ENABLED)
			if ((dev->priv_flags&IFF_EBRIDGE) && irqs_disabled()) {
				local_irq_enable();
				#if defined(CONFIG_COMPAT_NET_DEV_OPS)
					if(dev->stop)
						dev->stop(dev);
				#else
					if ((dev->netdev_ops)&&(dev->netdev_ops->ndo_stop))
						dev->netdev_ops->ndo_stop(dev);
				#endif
				local_irq_disable();
			} else
#endif
			
#if defined(CONFIG_COMPAT_NET_DEV_OPS)
			if(dev->stop)
				dev->stop(dev);
#else
			if ((dev->netdev_ops)&&(dev->netdev_ops->ndo_stop))
				dev->netdev_ops->ndo_stop(dev);
#endif
			rtnl_unlock();
		}
      	}
#if defined(CONFIG_RTL8192CD)
	{
		extern void force_stop_wlan_hw(void);
		force_stop_wlan_hw();
	}
#endif

	read_unlock(&dev_base_lock);
}

static void bsp_machine_restart(char *command)
{
#define BSP_TC_BASE         0xB8003100
#define BSP_WDTCNR          (BSP_TC_BASE + 0x1C)

	static void (*back_to_prom)(void) = (void (*)(void)) 0xbfc00000;
	
	REG32(BSP_GIMR)=0;
#ifdef CONFIG_SMP
	smp_send_stop();
#endif
	local_irq_disable();
#ifdef CONFIG_NET    
	shutdown_netdev();
#endif    
	REG32(BSP_WDTCNR) = 0; //enable watch dog
#ifdef CONFIG_SPI_3to4BYTES_ADDRESS_SUPPORT
{
	extern unsigned int ComSrlCmd_EX4B(unsigned char ucChip, unsigned int uiLen);
	ComSrlCmd_EX4B(0, 4);
}
#endif
	printk("System halted.\n");
	while(1);
	
	/* Reboot */
	back_to_prom();	
}

static void bsp_machine_halt(void)
{
	printk("System halted.\n");
	while(1);
}

/* callback function */
void __init bsp_setup(void)
{
#if defined(CONFIG_RTL_819X) && defined(CONFIG_RTL_819X_SWCORE)
	int ret= -1;
#endif
	unsigned int version = 0;
	extern void bsp_serial_init(void);
	extern void bsp_smp_init(void);

	/* define io/mem region */
	ioport_resource.start = 0x18000000;
	ioport_resource.end = 0x1fffffff;

	iomem_resource.start = 0x18000000;
	iomem_resource.end = 0x1fffffff;

	/* set reset vectors */
	_machine_restart = bsp_machine_restart;
	_machine_halt = bsp_machine_halt;
	pm_power_off = bsp_machine_halt;

	version = 8;

#ifdef CONFIG_SMP
	bsp_smp_init();
#endif

	bsp_serial_init();

	/* initialize switch core */
#if defined(CONFIG_RTL_819X)
#ifdef CONFIG_RTL_819X_SWCORE
	ret = bsp_swcore_init(version);
	if(ret != 0)
	{
		printk("initialize switch core fail!!!\n");
		bsp_machine_halt();
	}
#endif
#endif


}
