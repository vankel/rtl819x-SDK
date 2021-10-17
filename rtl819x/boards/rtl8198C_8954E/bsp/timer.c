/*
 * Realtek Semiconductor Corp.
 *
 * bsp/timer.c
 *     bsp timer initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#ifdef CONFIG_USE_UAPI
#include <generated/uapi/linux/version.h>
#else
#include <linux/version.h>
#endif
#include <linux/init.h>
#include <linux/timex.h>

#include <asm/time.h>

#include "bspchip.h"

const unsigned int cpu_clksel_table[]={ 450, 500, 550, 600, 650,700,
										750, 800, 850, 900, 950, 1000,										
										1050, 1100, 1150, 1200 };

#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
#define RANG5  0x1f
#define RANG4  0x0f

#define SYS_BASE 0xb8000000
#define SYS_INT_STATUS (SYS_BASE +0x04)
#define SYS_HW_STRAP   (SYS_BASE +0x08)

#define CK_M2X_FREQ_SEL_OFFSET 10
#define ST_CPU_FREQ_SEL_OFFSET 15

#ifdef CONFIG_CEVT_EXT
void inline bsp_timer_ack(void)
{
	unsigned volatile int eoi;
	eoi = REG32(BSP_TIMER0_EOI);
}

void __init bsp_timer_init(void)
{
	change_c0_cause(CAUSEF_DC, 0);

	/* disable timer */
	REG32(BSP_TIMER0_TCR) = 0x00000000;

	/* initialize timer registers */
	REG32(BSP_TIMER0_TLCR) = BSP_TIMER0_FREQ / HZ;

	/* hook up timer interrupt handler */
	ext_clockevent_init(BSP_TIMER0_IRQ);

	/* enable timer */
	REG32(BSP_TIMER0_TCR) = 0x00000003;       /* 0000-0000-0000-0011 */
}
#endif /* CONFIG_CEVT_EXT */

#ifdef CONFIG_RTL_WTDOG
int bBspWatchdog = 0;

void bsp_enable_watchdog0( void )
{
	bBspWatchdog = 1;
	*(volatile unsigned long *)(0xb800311C)=0x00420000;
}

void bsp_enable_watchdog( void )
{
	bBspWatchdog = 1;
	*(volatile unsigned long *)(0xb800311C)=0x00600000;
}

void bsp_disable_watchdog( void )
{
	*(volatile unsigned long *)(0xb800311C)=0xA5600000;
	bBspWatchdog = 0;
}
#endif // CONFIG_RTL_WTDOG



#ifdef CONFIG_CEVT_R4K
unsigned int __cpuinit get_c0_compare_int(void)
{
	return BSP_COMPARE_IRQ;
}

void __init bsp_timer_init(void)
{
	unsigned int ocp;
	unsigned int cpu_freq_sel;
	
	/* set cp0_compare_irq and cp0_perfcount_irq */
	cp0_compare_irq = BSP_COMPARE_IRQ;
	cp0_perfcount_irq = BSP_PERFCOUNT_IRQ;

	if (cp0_perfcount_irq == cp0_compare_irq)
		cp0_perfcount_irq = -1;

//	mips_hpt_frequency = BSP_CPU0_FREQ / 2;
	cpu_freq_sel=GET_BITVAL(REG32(SYS_HW_STRAP), ST_CPU_FREQ_SEL_OFFSET, RANG4);
	ocp=cpu_clksel_table[cpu_freq_sel] * 1000000;
	mips_hpt_frequency = ocp / 2;

	write_c0_count(0);
	mips_clockevent_init(cp0_compare_irq);
	mips_clocksource_init();

#if CONFIG_RTL_WTDOG	
	REG32(BSP_CDBR)=(BSP_DIVISOR) << BSP_DIVF_OFFSET;
#ifndef CONFIG_RTL_USERSPACE_WTDOG 
	bsp_enable_watchdog();
#else
	bsp_enable_watchdog0();
#endif
#endif

}
#endif /* CONFIG_CEVT_R4K */


#ifdef CONFIG_RTK_VOIP
void timer1_enable(void)
{
	printk( "timer1_enable not implement!!\n" );
}

void timer1_disable(void)
{
	printk( "timer1_disable not implement!!\n" );
}

#endif  // CONFIG_RTK_VOIP
