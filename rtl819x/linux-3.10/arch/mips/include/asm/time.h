/*
 * Copyright (C) 2001, 2002, MontaVista Software Inc.
 * Author: Jun Sun, jsun@mvista.com or jsun@junsun.net
 * Copyright (c) 2003  Maciej W. Rozycki
 *
 * include/asm-mips/time.h
 *     header file for the new style time.c file and time services.
 *
 * This program is free software; you can redistribute	it and/or modify it
 * under  the terms of	the GNU General	 Public License as published by the
 * Free Software Foundation;  either version 2 of the  License, or (at your
 * option) any later version.
 */
#ifndef _ASM_TIME_H
#define _ASM_TIME_H

#include <linux/rtc.h>
#include <linux/spinlock.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>

extern spinlock_t rtc_lock;

/*
 * RTC ops.  By default, they point to weak no-op RTC functions.
 *	rtc_mips_set_time - reverse the above translation and set time to RTC.
 *	rtc_mips_set_mmss - similar to rtc_set_time, but only min and sec need
 *			to be set.  Used by RTC sync-up.
 */
extern int rtc_mips_set_time(unsigned long);
extern int rtc_mips_set_mmss(unsigned long);

/*
 * board specific routines required by time_init().
 */
extern void bsp_time_init(void);

/*
 * mips_hpt_frequency - must be set if you intend to use an R4k-compatible
 * counter as a timer interrupt source.
 */
extern unsigned int mips_hpt_frequency;

/*
 * The performance counter IRQ on MIPS is a close relative to the timer IRQ
 * so it lives here.
 */
extern int (*perf_irq)(void);

/*
 * Initialize the calling CPU's compare interrupt as clockevent device
 */
#if defined(CONFIG_CEVT_R4K)
extern unsigned int __weak get_c0_compare_int(void);
extern int r4k_clockevent_init(int);
#endif
#ifdef CONFIG_CEVT_EXT
extern int ext_clockevent_init(int);
#endif
#ifdef CONFIG_MIPS_MT_SMTC
extern int smtc_clockevent_init(int);
#endif
#ifdef CONFIG_CEVT_GIC
extern int gic_clockevent_init(void);
#endif

static inline int mips_clockevent_init(int irq)
{
#ifdef CONFIG_MIPS_MT_SMTC
	return smtc_clockevent_init(irq);
#elif defined(CONFIG_CEVT_GIC)
	return (gic_clockevent_init() | r4k_clockevent_init());
#elif defined(CONFIG_CEVT_R4K)
	return r4k_clockevent_init(irq);
#elif defined(CONFIG_CEVT_EXT)
	return ext_clockevent_init(irq);
#else
	return -ENXIO;
#endif
}

/*
 * Initialize the count register as a clocksource
 */
#ifdef CONFIG_CSRC_R4K
extern int r4k_clocksource_init(void);
#endif

static inline int mips_clocksource_init(void)
{
#if defined(CONFIG_CSRC_R4K) && !defined(CONFIG_CSRC_GIC)
	return r4k_clocksource_init();
#else
	return 0;
#endif
}

static inline void clockevent_set_clock(struct clock_event_device *cd,
					unsigned int clock)
{
	clockevents_calc_mult_shift(cd, clock, 4);
}

#endif /* _ASM_TIME_H */
