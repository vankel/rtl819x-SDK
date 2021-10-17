#
# Makefile for the Linux/MIPS kernel.
#

extra-y		:= head.o vmlinux.lds

obj-y		+= cpu-probe.o branch.o entry.o genex.o idle.o irq.o process.o \
		   prom.o ptrace.o reset.o setup.o signal.o syscall.o \
		   time.o topology.o traps.o unaligned.o watch.o vdso.o

ifdef CONFIG_FUNCTION_TRACER
CFLAGS_REMOVE_ftrace.o = -pg
CFLAGS_REMOVE_early_printk.o = -pg
CFLAGS_REMOVE_perf_event.o = -pg
CFLAGS_REMOVE_perf_event_mipsxx.o = -pg
endif

obj-$(CONFIG_CEVT_R4K)		+= cevt-r4k.o
obj-$(CONFIG_CEVT_EXT)		+= cevt-ext.o
obj-$(CONFIG_MIPS_MT_SMTC)	+= cevt-smtc.o
obj-$(CONFIG_CEVT_GIC)		+= cevt-gic.o
obj-$(CONFIG_CSRC_GIC)		+= csrc-gic.o
obj-$(CONFIG_CSRC_R4K)		+= csrc-r4k.o
obj-$(CONFIG_SYNC_R4K)		+= sync-r4k.o

obj-$(CONFIG_STACKTRACE)	+= stacktrace.o
obj-$(CONFIG_MODULES)		+= mips_ksyms.o module.o
obj-$(CONFIG_MODULES_USE_ELF_RELA) += module-rela.o

obj-$(CONFIG_FUNCTION_TRACER)	+= mcount.o ftrace.o

obj-$(CONFIG_CPU_R4K_FPU)	+= r4k_fpu.o r4k_switch.o
obj-$(CONFIG_CPU_R3000)		+= r2300_fpu.o r2300_switch.o

obj-$(CONFIG_SMP)		+= smp.o smp-amon.o
obj-$(CONFIG_SMP_UP)		+= smp-up.o

obj-$(CONFIG_MIPS_MT)		+= mips-mt.o
obj-$(CONFIG_MIPS_MT_FPAFF)	+= mips-mt-fpaff.o
obj-$(CONFIG_MIPS_MT_SMTC)	+= smtc.o smtc-asm.o smtc-proc.o
obj-$(CONFIG_MIPS_MT_SMP)	+= smp-mt.o
obj-$(CONFIG_MIPS_CMP)		+= smp-cmp.o gcmp.o
obj-$(CONFIG_CPU_MIPSR2)	+= spram.o

obj-$(CONFIG_MIPS_VPE_LOADER)	+= vpe.o
obj-$(CONFIG_MIPS_VPE_APSP_API) += rtlx.o

obj-$(CONFIG_IRQ_CPU)		+= irq_cpu.o
obj-$(CONFIG_IRQ_GIC)		+= irq-gic.o

obj-$(CONFIG_KPROBES)		+= kprobes.o
obj-$(CONFIG_32BIT)		+= scall32-o32.o

obj-$(CONFIG_KGDB)		+= kgdb.o
obj-$(CONFIG_PROC_FS)		+= proc.o

obj-$(CONFIG_KEXEC)		+= machine_kexec.o relocate_kernel.o crash.o
obj-$(CONFIG_CRASH_DUMP)	+= crash_dump.o
obj-$(CONFIG_EARLY_PRINTK)	+= early_printk.o
obj-$(CONFIG_SPINLOCK_TEST)	+= spinlock_test.o
obj-$(CONFIG_MIPS_MACHINE)	+= mips_machine.o

obj-$(CONFIG_PERF_EVENTS)	+= perf_event.o
obj-$(CONFIG_HW_PERF_EVENTS)	+= perf_event_mipsxx.o

obj-$(CONFIG_JUMP_LABEL)	+= jump_label.o

#
# DSP ASE supported for MIPS32 or MIPS64 Release 2 cores only. It is not
# safe to unconditionnaly use the assembler -mdsp / -mdspr2 switches
# here because the compiler may use DSP ASE instructions (such as lwx) in
# code paths where we cannot check that the CPU we are running on supports it.
# Proper abstraction using HAVE_AS_DSP and macros is done in
# arch/mips/include/asm/mipsregs.h.
#
ifeq ($(CONFIG_CPU_MIPSR2), y)
CFLAGS_DSP 			= -DHAVE_AS_DSP

CFLAGS_signal.o			= $(CFLAGS_DSP)
CFLAGS_signal32.o		= $(CFLAGS_DSP)
CFLAGS_process.o		= $(CFLAGS_DSP)
CFLAGS_branch.o			= $(CFLAGS_DSP)
CFLAGS_ptrace.o			= $(CFLAGS_DSP)
endif

CPPFLAGS_vmlinux.lds		:= $(KBUILD_CFLAGS)
