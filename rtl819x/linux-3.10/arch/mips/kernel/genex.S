/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 - 2000, 2001, 2003 Ralf Baechle
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 * Copyright (C) 2002, 2007  Maciej W. Rozycki
 * Copyright (C) 2001, 2012 MIPS Technologies, Inc.  All rights reserved.
 */
#include <linux/init.h>

#include <asm/asm.h>
#include <asm/asmmacro.h>
#include <asm/cacheops.h>
#include <asm/irqflags.h>
#include <asm/regdef.h>
#include <asm/fpregdef.h>
#include <asm/mipsregs.h>
#include <asm/stackframe.h>
#include <asm/war.h>
#include <asm/thread_info.h>

#ifdef CONFIG_MIPS_MT_SMTC
#define PANIC_PIC(msg)					\
		.set	push;				\
		.set	nomicromips;			\
		.set	reorder;			\
		PTR_LA	a0,8f;				\
		.set	noat;				\
		PTR_LA	AT, panic;			\
		jr	AT;				\
9:		b	9b;				\
		.set	pop;				\
		TEXT(msg)
#endif

	__INIT

/*
 * General exception vector for all other CPUs.
 *
 * Be careful when changing this, it has to be at most 128 bytes
 * to fit into space reserved for the exception handler.
 */
NESTED(mips_trap_dispatch, 0, sp)
	.set	push
	.set	noat
	mfc0	k1, CP0_CAUSE
	andi	k1, k1, 0x7c
	PTR_L	k0, exception_handlers(k1)
	jr	k0
	.set	pop
	END(mips_trap_dispatch)

	__FINIT

#ifndef CONFIG_CPU_HAS_WAITOFF
	.align	5	/* 32 byte rollback region */
LEAF(__r4k_wait)
	.set	push
	.set	noreorder
	/* start of rollback region */
	LONG_L	t0, TI_FLAGS($28)
	nop
	andi	t0, _TIF_NEED_RESCHED
	bnez	t0, 1f
	 nop
	nop
	nop
#ifdef CONFIG_CPU_MICROMIPS
	nop
	nop
	nop
	nop
#endif
	.set	mips3
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	wait
	nop
	nop
	nop
	nop
	nop
	nop
	nop	
	nop

	/* end of rollback region (the region size must be power of two) */
1:
	jr	ra
	nop
	.set	pop
	END(__r4k_wait)

	.macro	BUILD_ROLLBACK_PROLOGUE handler
	FEXPORT(rollback_\handler)
	.set	push
	.set	noat
	MFC0	k0, CP0_EPC
	PTR_LA	k1, __r4k_wait
	nop
	ori	k0, 0x1f	/* 32 byte rollback region */
	nop
	xori	k0, 0x1f
	bne	k0, k1, 9f
	nop
	MTC0	k0, CP0_EPC
	nop
9:
	.set pop
	.endm
#else
	.macro  BUILD_ROLLBACK_PROLOGUE handler
	.endm
#endif
	.align  5
BUILD_ROLLBACK_PROLOGUE handle_int
NESTED(handle_int, PT_SIZE, sp)
#ifdef CONFIG_TRACE_IRQFLAGS
	/*
	 * Check to see if the interrupted code has just disabled
	 * interrupts and ignore this interrupt for now if so.
	 *
	 * local_irq_disable() disables interrupts and then calls
	 * trace_hardirqs_off() to track the state. If an interrupt is taken
	 * after interrupts are disabled but before the state is updated
	 * it will appear to restore_all that it is incorrectly returning with
	 * interrupts disabled
	 */
	.set	push
	.set	noat
	mfc0	k0, CP0_STATUS
	and	k0, ST0_IE
	bnez	k0, 1f

	eret
1:
	.set pop
#endif
	SAVE_ALL
	CLI
	TRACE_IRQS_OFF

	LONG_L	s0, TI_REGS($28)
	LONG_S	sp, TI_REGS($28)
	PTR_LA	ra, ret_from_irq
	PTR_LA  v0, bsp_irq_dispatch
	jr	v0
#ifdef CONFIG_CPU_MICROMIPS
	nop
#endif
	END(handle_int)

	__INIT

/*
 * Special interrupt vector for MIPS64 ISA & embedded MIPS processors.
 * This is a dedicated interrupt exception vector which reduces the
 * interrupt processing overhead.  The jump instruction will be replaced
 * at the initialization time.
 *
 * Be careful when changing this, it has to be at most 128 bytes
 * to fit into space reserved for the exception handler.
 */
NESTED(mips_trap_divec, 0, sp)
1:	j	1b			/* Dummy, will be replaced */
	END(mips_trap_divec)

	__FINIT

/*
 * Vectored interrupt handler.
 * This prototype is copied to ebase + n*IntCtl.VS and patched
 * to invoke the handler
 */
#ifdef CONFIG_CPU_MIPSR2_IRQ_VI
NESTED(except_vec_vi, 0, sp)
	SAVE_SOME
	SAVE_AT
	.set	push
	.set	noreorder
#ifdef CONFIG_MIPS_MT_SMTC
	/*
	 * To keep from blindly blocking *all* interrupts
	 * during service by SMTC kernel, we also want to
	 * pass the IM value to be cleared.
	 */
FEXPORT(except_vec_vi_mori)
	ori	a0, $0, 0
#endif /* CONFIG_MIPS_MT_SMTC */
	PTR_LA	v1, except_vec_vi_handler
FEXPORT(except_vec_vi_lui)
	lui	v0, 0		/* Patched */
	jr	v1
FEXPORT(except_vec_vi_ori)
	 ori	v0, 0		/* Patched */
	.set	pop
	END(except_vec_vi)
EXPORT(except_vec_vi_end)

/*
 * Common Vectored Interrupt code
 * Complete the register saves and invoke the handler which is passed in $v0
 */
NESTED(except_vec_vi_handler, 0, sp)
	SAVE_TEMP
	SAVE_STATIC
#ifdef CONFIG_MIPS_MT_SMTC
	/*
	 * SMTC has an interesting problem that interrupts are level-triggered,
	 * and the CLI macro will clear EXL, potentially causing a duplicate
	 * interrupt service invocation. So we need to clear the associated
	 * IM bit of Status prior to doing CLI, and restore it after the
	 * service routine has been invoked - we must assume that the
	 * service routine will have cleared the state, and any active
	 * level represents a new or otherwised unserviced event...
	 */
	mfc0	t1, CP0_STATUS
	and	t0, a0, t1
#ifdef CONFIG_MIPS_MT_SMTC_IM_BACKSTOP
	mfc0	t2, CP0_TCCONTEXT
	or	t2, t0, t2
	mtc0	t2, CP0_TCCONTEXT
#endif /* CONFIG_MIPS_MT_SMTC_IM_BACKSTOP */
	xor	t1, t1, t0
	mtc0	t1, CP0_STATUS
	_ehb
#endif /* CONFIG_MIPS_MT_SMTC */
	CLI
#ifdef CONFIG_TRACE_IRQFLAGS
	move	s0, v0
#ifdef CONFIG_MIPS_MT_SMTC
	move	s1, a0
#endif
	TRACE_IRQS_OFF
#ifdef CONFIG_MIPS_MT_SMTC
	move	a0, s1
#endif
	move	v0, s0
#endif

	LONG_L	s0, TI_REGS($28)
	LONG_S	sp, TI_REGS($28)
	PTR_LA	ra, ret_from_irq
	jr	v0
	END(except_vec_vi_handler)
#endif

	__INIT

/*
 * NMI debug exception handler for MIPS reference boards.
 * The NMI debug exception entry point is 0xbfc00000, which
 * normally is in the boot PROM, so the boot PROM must do a
 * unconditional jump to this vector.
 */
NESTED(except_vec_nmi, 0, sp)
	j	nmi_handler
#ifdef CONFIG_CPU_MICROMIPS
	 nop
#endif
	END(except_vec_nmi)

	__FINIT

NESTED(nmi_handler, PT_SIZE, sp)
	.set	push
	.set	noat
	SAVE_ALL
	move	a0, sp
	jal	nmi_exception_handler
	RESTORE_ALL
	.set	mips3
	eret
	.set	pop
	END(nmi_handler)

	.macro	__build_clear_none
	.endm

	.macro	__build_clear_sti
	TRACE_IRQS_ON
	STI
	.endm

	.macro	__build_clear_cli
	CLI
	TRACE_IRQS_OFF
	.endm

#ifdef CONFIG_CPU_HAS_FPU
	.macro	__build_clear_fpe
	.set	push
	/* gas fails to assemble cfc1 for some archs (octeon).*/ \
	.set	mips1
	cfc1	a1, fcr31
	li	a2, ~(0x3f << 12)
	and	a2, a1
	ctc1	a2, fcr31
	.set	pop
	TRACE_IRQS_ON
	STI
	.endm
#endif

	.macro	__build_clear_ade
	MFC0	t0, CP0_BADVADDR
	PTR_S	t0, PT_BVADDR(sp)
	KMODE
	.endm

	.macro	__BUILD_silent exception
	.endm

	/* Gas tries to parse the PRINT argument as a string containing
	   string escapes and emits bogus warnings if it believes to
	   recognize an unknown escape code.  So make the arguments
	   start with an n and gas will believe \n is ok ...  */
	.macro	__BUILD_verbose nexception
	LONG_L	a1, PT_EPC(sp)
#ifdef CONFIG_32BIT
	PRINT("Got \nexception at %08lx\012")
#endif
#ifdef CONFIG_64BIT
	PRINT("Got \nexception at %016lx\012")
#endif
	.endm

	.macro	__BUILD_count exception
	LONG_L	t0,exception_count_\exception
	LONG_ADDIU t0, 1
	LONG_S	t0,exception_count_\exception
	.comm	exception_count\exception, 8, 8
	.endm

	.macro	__BUILD_HANDLER exception handler clear verbose ext
	.align	5
	NESTED(handle_\exception, PT_SIZE, sp)
	.set	noat
	SAVE_ALL
	FEXPORT(handle_\exception\ext)
	__BUILD_clear_\clear
	.set	at
	__BUILD_\verbose \exception
	move	a0, sp
	PTR_LA	ra, ret_from_exception
	j	do_\handler
	END(handle_\exception)
	.endm

	.macro	BUILD_HANDLER exception handler clear verbose
	__BUILD_HANDLER \exception \handler \clear \verbose _int
	.endm

	BUILD_HANDLER adel ade ade silent		/* #4  */
	BUILD_HANDLER ades ade ade silent		/* #5  */
	BUILD_HANDLER ibe be cli silent			/* #6  */
	BUILD_HANDLER dbe be cli silent			/* #7  */
	BUILD_HANDLER bp bp sti silent			/* #9  */
	BUILD_HANDLER ri ri sti silent			/* #10 */
	BUILD_HANDLER cpu cpu sti silent		/* #11 */
	BUILD_HANDLER ov ov sti silent			/* #12 */
	BUILD_HANDLER tr tr sti silent			/* #13 */
#ifdef CONFIG_CPU_HAS_FPU
	BUILD_HANDLER fpe fpe fpe silent		/* #15 */
#endif
	BUILD_HANDLER mdmx mdmx sti silent		/* #22 */
#ifdef CONFIG_CPU_HAS_WATCH
	/*
	 * For watch, interrupts will be enabled after the watch
	 * registers are read.
	 */
	BUILD_HANDLER watch watch cli silent		/* #23 */
#endif
	BUILD_HANDLER mcheck mcheck cli verbose		/* #24 */
#ifdef CONFIG_CPU_HAS_MIPSMT
	BUILD_HANDLER mt mt sti silent			/* #25 */
#endif
#ifdef CONFIG_CPU_HAS_DSP
	BUILD_HANDLER dsp dsp sti silent		/* #26 */
#endif
	BUILD_HANDLER reserved reserved sti verbose	/* others */

#ifndef CONFIG_CPU_HAS_RDHWR
	.align	5
	LEAF(handle_ri_rdhwr)
	.set	push
	.set	noat
	.set	noreorder
	/* MIPS32:    0x7c03e83b: rdhwr v1,$29 */
	/* MIPS16m:   0x007d6b3c: rdhwr v1,$29 */
	MFC0	k1, CP0_EPC
#if defined(CONFIG_CPU_MICROMIPS) || defined(CONFIG_CPU_MIPS32_R2) || defined(CONFIG_CPU_MIPS64_R2)
	and     k0, k1, 1
	beqz    k0, 1f
	xor     k1, k0
	lhu     k0, (k1)
	lhu     k1, 2(k1)
	ins     k1, k0, 16, 16
	lui     k0, 0x007d
	b       docheck
	ori     k0, 0x6b3c
1:
	lui     k0, 0x7c03
	lw      k1, (k1)
	ori     k0, 0xe83b
#else
	andi    k0, k1, 1
	bnez    k0, handle_ri
	lui     k0, 0x7c03
	lw      k1, (k1)
	ori     k0, 0xe83b
#endif
	.set    reorder
docheck:
	bne	k0, k1, handle_ri	/* if not ours */

isrdhwr:
	/* The insn is rdhwr.  No need to check CAUSE.BD here. */
	get_saved_sp	/* k1 := current_thread_info */
	.set	noreorder
	MFC0	k0, CP0_EPC
	LONG_ADDIU	k0, 4		/* stall on $k0 */
	MTC0	k0, CP0_EPC
	/* I hope three instructions between MTC0 and ERET are enough... */
	ori	k1, _THREAD_MASK
	xori	k1, _THREAD_MASK
	LONG_L	v1, TI_TP_VALUE(k1)
	.set	mips3
	eret
	.set	mips0
	.set	pop
	END(handle_ri_rdhwr)
#endif
