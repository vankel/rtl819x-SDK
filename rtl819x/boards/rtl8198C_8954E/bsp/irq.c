/*
 * Realtek Semiconductor Corp.
 *
 * bsp/irq.c
 *     bsp interrupt initialization and handler code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#ifdef CONFIG_USE_UAPI
#include <generated/uapi/linux/version.h>
#else
#include <linux/version.h>
#endif
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/sched.h>

#include <asm/irq.h>
#include <asm/gic.h>
#include <asm/irq_cpu.h>

#include <asm/gcmpregs.h>
#include <asm/mipsregs.h>

#include "bspchip.h"

static inline int clz(unsigned long x)
{
	__asm__ __volatile__(
	"       clz     %0, %1                                  \n"
	: "=r" (x)
	: "r" (x));

	return x;
}

/*
 * FFS
 *
 * Given pending, use ffs to find first leading non-zero. Then,
 * Use offset to shift bit range. For example, use CAUSEB_IP as offset
 * to look for bit starting at 12 in status register, so that ffs is
 * rounded between 0~7
 */
static inline int irq_ffs(unsigned int pending, unsigned int offset)
{
	return -clz(pending) + 31 - offset;
}

#ifdef CONFIG_IRQ_ICTL
#include "irq-ictl.c"
#endif

#ifdef CONFIG_IRQ_GIC
#include "irq-gic.c"
//#define IRQ_GIC_CASECADE	6
#endif

#ifdef CONFIG_SMP
#include "irq-ipi.c"
#endif

/*
 * IRQs on the Sheipa3 look basically (barring software IRQs which we
 * don't use at all and all external interrupt sources are combined together
 * on hardware interrupt 0 (MIPS IRQ 2)) like:
 *
 *      MIPS IRQ        GIC
 *      --------        ------           ------
 *             0        UNUSED           SWINT0
 *             1        UNUSED           SWINT1
 *             2        irq_n            UART0
 *             3        ipi_resched      GIC IPI
 *             4        ipi_call         GIC IPI
 *             5        UNUSED           UNUSED
 *             6        r4k_perfcnt      GIC VPE local
 *             7        r4k_timer        GIC VPE local
 *
 * We handle the IRQ according to _our_ priority which is:
 *
 * Highest ----     r4k timer
 * Lowest  ----     ictl
 *
 * then we just return, if multiple IRQs are pending then we will just take
 * another exception, big deal.
 */
 
 
asmlinkage void bsp_irq_dispatch(void)
{
#define IRQ_SPEEDUP 1
#ifdef IRQ_SPEEDUP
	
	unsigned int pending = read_c0_cause() & read_c0_status() & ST0_IM;
	int irq;

	irq = irq_ffs(pending, CAUSEB_IP);
	

		
	if(irq==5)
	{	//printk("SW\n");
		//do_IRQ(BSP_SWITCH_IRQ);  
		struct irq_desc *desc = irq_to_desc(BSP_SWITCH_IRQ);
		generic_handle_irq_desc(BSP_SWITCH_IRQ, desc);		
	}

	else if(irq==6)
	{
		//do_IRQ(BSP_PS_USB_IRQ_USB3); 	
		struct irq_desc *desc = irq_to_desc(BSP_PS_USB_IRQ_USB3);
		generic_handle_irq_desc(BSP_PS_USB_IRQ_USB3, desc);			
	}
	
/*	
	else if(irq==6)
	{	//printk("SATA\n");
		//do_IRQ(BSP_PS_SATA_IRQ);
		struct irq_desc *desc = irq_to_desc(BSP_PS_SATA_IRQ);
		generic_handle_irq_desc(BSP_PS_SATA_IRQ, desc);
		//extern irqreturn_t ahci_interrupt(int irq, void *dev_instance);
		//ahci_interrupt(BSP_PS_SATA_IRQ, desc);
	
			 
	}
*/	
	else if ((irq==3)||(irq==4))
		bsp_ipi_dispatch();
				
	else if (irq >= 0)
	{	
		do_IRQ(irq);
	}
	else
		spurious_interrupt();

	
#else
	unsigned int pending = read_c0_cause() & read_c0_status() & ST0_IM;
	int irq;

	irq = irq_ffs(pending, CAUSEB_IP);
		
#if defined(CONFIG_SMP) && defined(CONFIG_IRQ_GIC)
	if (((1 << irq) & bsp_ipi_map[smp_processor_id()]))
		bsp_ipi_dispatch();
	else
#endif
	if (irq >= 0)
	{	
		do_IRQ(irq);
	}
	else
		spurious_interrupt();
		
#endif		
}

void __init bsp_irq_init(void)
{
	/* initialize IRQ action handlers */
	mips_cpu_irq_init(BSP_IRQ_CPU_BASE);
#ifdef CONFIG_IRQ_GIC
	gic_init(GIC_BASE_ADDR, GIC_BASE_SIZE,
		 gic_intr_map, BSP_IRQ_GIC_BASE);
#endif
#ifdef CONFIG_IRQ_ICTL
	bsp_ictl_irq_init(BSP_IRQ_ICTL_BASE);
#endif
#ifdef CONFIG_SMP
	bsp_ipi_init(gic_intr_map);
#endif
}
