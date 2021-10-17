/*
 * MIPS idle loop and WAIT instruction support.
 *
 * Copyright (C) xxxx  the Anonymous
 * Copyright (C) 1994 - 2006 Ralf Baechle
 * Copyright (C) 2003, 2004  Maciej W. Rozycki
 * Copyright (C) 2001, 2004, 2011, 2012	 MIPS Technologies, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */
#include <linux/export.h>
#include <linux/init.h>
#include <linux/irqflags.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <asm/cpu.h>
#include <asm/cpu-info.h>
#include <asm/idle.h>
#include <asm/mipsregs.h>

/*
 * Not all of the MIPS CPUs have the "wait" instruction available. Moreover,
 * the implementation of the "wait" feature differs between CPU families. This
 * points to the function that implements CPU specific wait.
 * The wait instruction stops the pipeline and reduces the power consumption of
 * the CPU very much.
 */
void (*cpu_wait)(void);
EXPORT_SYMBOL(cpu_wait);

#ifndef CONFIG_CPU_HAS_WAITOFF
void r4k_wait(void)
{
	local_irq_enable();
	__r4k_wait();
}
#endif

/*
 * This variant is preferable as it allows testing need_resched and going to
 * sleep depending on the outcome atomically.  Unfortunately the "It is
 * implementation-dependent whether the pipeline restarts when a non-enabled
 * interrupt is requested" restriction in the MIPS32/MIPS64 architecture makes
 * using this version a gamble.
 */
#ifdef CONFIG_CPU_HAS_WAITOFF
void r4k_wait_irqoff(void)
{
	if (!need_resched())
		__asm__(
		"	.set	push		\n"
		"	.set	mips3		\n"
		"	wait			\n"
		"	.set	pop		\n");
	local_irq_enable();
	__asm__(
	"	.globl __pastwait	\n"
	"__pastwait:			\n");
}
#endif

void __init check_wait(void)
{
#ifdef CONFIG_CPU_HAS_WAITOFF
	cpu_wait = r4k_wait_irqoff;
#else
	cpu_wait = r4k_wait;
#endif
}

static void smtc_idle_hook(void)
{
#ifdef CONFIG_MIPS_MT_SMTC
	void smtc_idle_loop_hook(void);

	smtc_idle_loop_hook();
#endif
}

void arch_cpu_idle(void)
{
	smtc_idle_hook();
	cpu_wait();
}
