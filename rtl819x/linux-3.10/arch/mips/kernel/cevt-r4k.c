/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2007 MIPS Technologies, Inc.
 * Copyright (C) 2007 Ralf Baechle <ralf@linux-mips.org>
 */
#include <linux/clockchips.h>
#include <linux/interrupt.h>
#include <linux/percpu.h>
#include <linux/smp.h>
#include <linux/irq.h>

#include <asm/smtc_ipi.h>
#include <asm/time.h>
#include <asm/cevt-r4k.h>
#include <asm/gic.h>

/*
 * The SMTC Kernel for the 34K, 1004K, et. al. replaces several
 * of these routines with SMTC-specific variants.
 */
#ifdef CONFIG_RTL_WTDOG
int is_fault=0; // kernel fault flag
#endif

#ifndef CONFIG_MIPS_MT_SMTC
static int mips_next_event(unsigned long delta,
			   struct clock_event_device *evt)
{
	unsigned int cnt;
	int res;

	cnt = read_c0_count();
	cnt += delta;
	write_c0_compare(cnt);
	res = ((int)(read_c0_count() - cnt) >= 0) ? -ETIME : 0;
	return res;
}

#endif /* CONFIG_MIPS_MT_SMTC */

void mips_set_clock_mode(enum clock_event_mode mode,
				struct clock_event_device *evt)
{
	/* Nothing to do ...  */
}

DEFINE_PER_CPU(struct clock_event_device, mips_clockevent_device);
int cp0_timer_irq_installed;

#ifndef CONFIG_MIPS_MT_SMTC
irqreturn_t c0_compare_interrupt(int irq, void *dev_id)
{
	struct clock_event_device *cd;
	int cpu = smp_processor_id();
#if defined(CONFIG_RTL_WTDOG)
        if (!is_fault) {
                /**(volatile unsigned long *)(0xB800311c) |=  1 << 23;*/
        } else {
		printk("%s %d\n",__FUNCTION__,__LINE__);
                local_irq_disable(); 
                *(volatile unsigned long *)(0xB800311c)=0; 
                for(;;);
	}
#endif
	handle_perf_irq(1);

	/*
	 * The same applies to performance counter interrupts.	But with the
	 * above we now know that the reason we got here must be a timer
	 * interrupt.  Being the paranoiacs we are we check anyway.
	 */
	if (read_c0_cause() & (1 << CAUSEB_TI)) {
		/* Clear Count/Compare Interrupt */
		write_c0_compare(read_c0_compare());
		cd = &per_cpu(mips_clockevent_device, cpu);
#ifdef CONFIG_CEVT_GIC
		if (!gic_present)
#endif
		cd->event_handler(cd);
	}

#ifdef CONFIG_RTL8686_SHM_NOTIFY
    //printk("%d ", cpu);
    if (cpu == 0)
    {
        extern int aipc_shm_notify(void);
        aipc_shm_notify();
    }
#endif

	return IRQ_HANDLED;
}

#endif /* Not CONFIG_MIPS_MT_SMTC */

struct irqaction c0_compare_irqaction = {
	.handler = c0_compare_interrupt,
	.flags = IRQF_PERCPU | IRQF_TIMER,
	.name = "timer",
};


void mips_event_handler(struct clock_event_device *dev)
{
}

/*
 * FIXME: This doesn't hold for the relocated E9000 compare interrupt.
 */
static int c0_compare_int_pending(void)
{
	return (read_c0_cause() & (1 << CAUSEB_TI));
}

/*
 * Compare interrupt can be routed and latched outside the core,
 * so wait up to worst case number of cycle counter ticks for timer interrupt
 * changes to propagate to the cause register.
 */
#define COMPARE_INT_SEEN_TICKS 50

int c0_compare_int_usable(void)
{
	unsigned int delta;
	unsigned int cnt;

#ifdef CONFIG_KVM_GUEST
    return 1;
#endif

	/*
	 * IP7 already pending?	 Try to clear it by acking the timer.
	 */
	if (c0_compare_int_pending()) {
		cnt = read_c0_count();
		write_c0_compare(cnt);
		back_to_back_c0_hazard();
		while (read_c0_count() < (cnt  + COMPARE_INT_SEEN_TICKS))
			if (!c0_compare_int_pending())
				break;
		if (c0_compare_int_pending())
			return 0;
	}

	for (delta = 0x10; delta <= 0x400000; delta <<= 1) {
		cnt = read_c0_count();
		cnt += delta;
		write_c0_compare(cnt);
		back_to_back_c0_hazard();
		if ((int)(read_c0_count() - cnt) < 0)
		    break;
		/* increase delta if the timer was already expired */
	}

	while ((int)(read_c0_count() - cnt) <= 0)
		;	/* Wait for expiry  */

	while (read_c0_count() < (cnt + COMPARE_INT_SEEN_TICKS))
		if (c0_compare_int_pending())
			break;
	if (!c0_compare_int_pending())
		return 0;
	cnt = read_c0_count();
	write_c0_compare(cnt);
	back_to_back_c0_hazard();
	while (read_c0_count() < (cnt + COMPARE_INT_SEEN_TICKS))
		if (!c0_compare_int_pending())
			break;
	if (c0_compare_int_pending())
		return 0;

	/*
	 * Feels like a real count / compare timer.
	 */
	return 1;
}

#ifndef CONFIG_MIPS_MT_SMTC
int __cpuinit r4k_clockevent_init(int irq)
{
	unsigned int cpu = smp_processor_id();
	struct clock_event_device *cd;

	if (!cpu_has_counter || !mips_hpt_frequency)
		return -ENXIO;

	if (!c0_compare_int_usable())
		return -ENXIO;

	cd = &per_cpu(mips_clockevent_device, cpu);

	cd->name		= "MIPS";
	cd->features		= CLOCK_EVT_FEAT_ONESHOT;

	clockevent_set_clock(cd, mips_hpt_frequency);

	/* Calculate the min / max delta */
	cd->max_delta_ns	= clockevent_delta2ns(0x7fffffff, cd);
	cd->min_delta_ns	= clockevent_delta2ns(0x300, cd);

	cd->rating		= 300;
	cd->irq			= irq;
	cd->cpumask		= cpumask_of(cpu);
	cd->set_next_event	= mips_next_event;
	cd->set_mode		= mips_set_clock_mode;
	cd->event_handler	= mips_event_handler;

#ifdef CONFIG_CEVT_GIC
	if (!gic_present)
#endif
	clockevents_register_device(cd);

	if (cp0_timer_irq_installed)
		return 0;

	cp0_timer_irq_installed = 1;

	setup_irq(irq, &c0_compare_irqaction);

	return 0;
}

#endif /* Not CONFIG_MIPS_MT_SMTC */
