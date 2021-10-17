/*
 * Realtek Semiconductor Corp.
 *
 * asm/time.h
 *
 * Copied and modified from arch/mips/include/asm.h
 * Copyright 2012  Viller Hsiao (villerhsiao@realtek.com)
 * Copyright 2012  Tony Wu (tonywu@realtek.com)
 */
#ifndef _ASM_TIME_H
#define _ASM_TIME_H

#include <linux/rtc.h>
#include <linux/spinlock.h>
#include <linux/clockchips.h>
#include <linux/clocksource.h>

/*
 * Initialize the calling CPU's compare interrupt as clockevent device
 */
#ifdef CONFIG_CEVT_EXT
extern int ext_clockevent_init(int);
#endif

static inline void clockevent_set_clock(struct clock_event_device *cd,
					unsigned int clock)
{
    clockevents_calc_mult_shift(cd, clock, 4);
}

#endif /* _ASM_TIME_H */

