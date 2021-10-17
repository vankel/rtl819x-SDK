/*
 * Processor capabilities determination functions.
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
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/ptrace.h>
#include <linux/smp.h>
#include <linux/stddef.h>
#include <linux/export.h>

#include <asm/bugs.h>
#include <asm/cpu.h>
#include <asm/fpu.h>
#include <asm/mipsregs.h>
#include <asm/watch.h>
#include <asm/elf.h>
#include <asm/spram.h>
#include <asm/uaccess.h>

/*
 * Get the FPU Implementation/Revision.
 */
#ifdef CONFIG_CPU_HAS_FPU
static inline unsigned long cpu_get_fpu_id(void)
{
	unsigned long tmp, fpu_id;

	tmp = read_c0_status();
	__enable_fpu();
	fpu_id = read_32bit_cp1_register(CP1_REVISION);
	write_c0_status(tmp);
	return fpu_id;
}

/*
 * Check the CPU has an FPU the official way.
 */
static inline int __cpu_has_fpu(void)
{
	return ((cpu_get_fpu_id() & 0xff00) != FPIR_IMP_NONE);
}
#endif

static inline void cpu_probe_vmbits(struct cpuinfo_mips *c)
{
#ifdef __NEED_VMBITS_PROBE
	write_c0_entryhi(0x3fffffffffffe000ULL);
	back_to_back_c0_hazard();
	c->vmbits = fls64(read_c0_entryhi() & 0x3fffffffffffe000ULL);
#endif
}

static inline unsigned int decode_config0(struct cpuinfo_mips *c)
{
	unsigned int config0;

	config0 = read_c0_config();

	c->options |= MIPS_CPU_TLB;
	c->isa_level = MIPS_CPU_ISA_M32R2;

	return config0 & MIPS_CONF_M;
}

static inline unsigned int decode_config1(struct cpuinfo_mips *c)
{
	unsigned int config1;

	config1 = read_c0_config1();

	if (config1 & MIPS_CONF1_MD)
		c->ases |= MIPS_ASE_MDMX;
	if (config1 & MIPS_CONF1_WR)
		c->options |= MIPS_CPU_WATCH;
	if (config1 & MIPS_CONF1_CA)
		c->ases |= MIPS_ASE_MIPS16;
	if (config1 & MIPS_CONF1_EP)
		c->options |= MIPS_CPU_EJTAG;
	if (config1 & MIPS_CONF1_FP) {
		c->options |= MIPS_CPU_FPU;
		c->options |= MIPS_CPU_32FPR;
	}
	if (cpu_has_tlb)
		c->tlbsize = ((config1 & MIPS_CONF1_TLBS) >> 25) + 1;

	return config1 & MIPS_CONF_M;
}

static inline unsigned int decode_config2(struct cpuinfo_mips *c)
{
	unsigned int config2;

	config2 = read_c0_config2();

	if (config2 & MIPS_CONF2_SL)
		c->scache.flags &= ~MIPS_CACHE_NOT_PRESENT;

	return config2 & MIPS_CONF_M;
}

static inline unsigned int decode_config3(struct cpuinfo_mips *c)
{
	unsigned int config3;

	config3 = read_c0_config3();

	if (config3 & MIPS_CONF3_SM) {
		c->ases |= MIPS_ASE_SMARTMIPS;
		c->options |= MIPS_CPU_RIXI;
	}
	if (config3 & MIPS_CONF3_RXI)
		c->options |= MIPS_CPU_RIXI;
	if (config3 & MIPS_CONF3_DSP)
		c->ases |= MIPS_ASE_DSP;
	if (config3 & MIPS_CONF3_DSP2P)
		c->ases |= MIPS_ASE_DSP2P;
	if (config3 & MIPS_CONF3_VINT)
		c->options |= MIPS_CPU_VINT;
	if (config3 & MIPS_CONF3_VEIC)
		c->options |= MIPS_CPU_VEIC;
	if (config3 & MIPS_CONF3_MT)
		c->ases |= MIPS_ASE_MIPSMT;
	if (config3 & MIPS_CONF3_ULRI)
		c->options |= MIPS_CPU_ULRI;
	if (config3 & MIPS_CONF3_ISA)
		c->options |= MIPS_CPU_MICROMIPS;
#ifdef CONFIG_CPU_MICROMIPS
	write_c0_config3(read_c0_config3() | MIPS_CONF3_ISA_OE);
#endif
	if (config3 & MIPS_CONF3_VZ)
		c->ases |= MIPS_ASE_VZ;

	return config3 & MIPS_CONF_M;
}

static inline unsigned int decode_config4(struct cpuinfo_mips *c)
{
	unsigned int config4;

	config4 = read_c0_config4();

	if ((config4 & MIPS_CONF4_MMUEXTDEF) == MIPS_CONF4_MMUEXTDEF_MMUSIZEEXT
	    && cpu_has_tlb)
		c->tlbsize += (config4 & MIPS_CONF4_MMUSIZEEXT) * 0x40;

	c->kscratch_mask = (config4 >> 16) & 0xff;

	return config4 & MIPS_CONF_M;
}

static void __cpuinit decode_configs(struct cpuinfo_mips *c)
{
	int ok;

	/* MIPS32 or MIPS64 compliant CPU.  */
	c->options = MIPS_CPU_4KEX | MIPS_CPU_4K_CACHE | MIPS_CPU_COUNTER |
		     MIPS_CPU_DIVEC | MIPS_CPU_LLSC | MIPS_CPU_MCHECK;

	c->scache.flags = MIPS_CACHE_NOT_PRESENT;

	ok = decode_config0(c);			/* Read Config registers.  */
	BUG_ON(!ok);				/* Arch spec violation!	 */
	if (ok)
		ok = decode_config1(c);
	if (ok)
		ok = decode_config2(c);
	if (ok)
		ok = decode_config3(c);
	if (ok)
		ok = decode_config4(c);

#ifdef CONFIG_CPU_HAS_WATCH
	mips_probe_watch_registers(c);
#endif

	if (cpu_has_mips_r2)
		c->core = read_c0_ebase() & 0x3ff;
}

#define R4K_OPTS (MIPS_CPU_TLB | MIPS_CPU_4KEX | MIPS_CPU_4K_CACHE \
		| MIPS_CPU_COUNTER)

static inline void cpu_probe_mips(struct cpuinfo_mips *c, unsigned int cpu)
{
	decode_configs(c);
	switch (c->processor_id & 0xff00) {
	case PRID_IMP_4KEC:
	case PRID_IMP_4KECR2:
		c->cputype = CPU_4KEC;
		__cpu_name[cpu] = "MIPS 4KEc";
		break;
	case PRID_IMP_24K:
		c->cputype = CPU_24K;
		__cpu_name[cpu] = "MIPS 24Kc";
		break;
	case PRID_IMP_24KE:
		c->cputype = CPU_24K;
		__cpu_name[cpu] = "MIPS 24KEc";
		break;
	case PRID_IMP_34K:
		c->cputype = CPU_34K;
		__cpu_name[cpu] = "MIPS 34Kc";
		break;
	case PRID_IMP_74K:
		c->cputype = CPU_74K;
		__cpu_name[cpu] = "MIPS 74Kc";
		break;
	case PRID_IMP_M14KC:
		c->cputype = CPU_M14KC;
		__cpu_name[cpu] = "MIPS M14Kc";
		break;
	case PRID_IMP_M14KEC:
		c->cputype = CPU_M14KEC;
		__cpu_name[cpu] = "MIPS M14KEc";
		break;
	case PRID_IMP_1004K:
		c->cputype = CPU_1004K;
		__cpu_name[cpu] = "MIPS 1004Kc";
		break;
	case PRID_IMP_1074K:
		c->cputype = CPU_74K;
		__cpu_name[cpu] = "MIPS 1074Kc";
		break;
	}

#ifdef CONFIG_CPU_HAS_SPRAM
	spram_config();
#endif
}

#ifdef CONFIG_64BIT
/* For use by uaccess.h */
u64 __ua_limit;
EXPORT_SYMBOL(__ua_limit);
#endif

const char *__cpu_name[NR_CPUS];
const char *__elf_platform;

__cpuinit void cpu_probe(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int cpu = smp_processor_id();

	c->processor_id = PRID_IMP_UNKNOWN;
	c->fpu_id	= FPIR_IMP_NONE;
	c->cputype	= CPU_UNKNOWN;

	c->processor_id = read_c0_prid();
	cpu_probe_mips(c, cpu);

	BUG_ON(!__cpu_name[cpu]);
	BUG_ON(c->cputype == CPU_UNKNOWN);

	/*
	 * Platform code can force the cpu type to optimize code
	 * generation. In that case be sure the cpu type is correctly
	 * manually setup otherwise it could trigger some nasty bugs.
	 */
	BUG_ON(current_cpu_type() != c->cputype);

#ifndef CONFIG_CPU_HAS_DSP
	c->ases &= ~MIPS_ASE_DSP;
#endif

#ifdef CONFIG_CPU_HAS_FPU
	if (c->options & MIPS_CPU_FPU) {
		c->fpu_id = cpu_get_fpu_id();

		if (c->fpu_id & MIPS_FPIR_3D)
			c->ases |= MIPS_ASE_MIPS3D;
	}
#else
	c->options &= ~MIPS_CPU_FPU;
#endif

	if (cpu_has_mips_r2) {
		c->srsets = ((read_c0_srsctl() >> 26) & 0x0f) + 1;
		/* R2 has Performance Counter Interrupt indicator */
		c->options |= MIPS_CPU_PCI;
	}
	else
		c->srsets = 1;

	cpu_probe_vmbits(c);

#ifdef CONFIG_64BIT
	if (cpu == 0)
		__ua_limit = ~((1ull << cpu_vmbits) - 1);
#endif
}

__cpuinit void cpu_report(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;

	printk(KERN_INFO "CPU revision is: %08x (%s)\n",
	       c->processor_id, cpu_name_string());
	if (c->options & MIPS_CPU_FPU)
		printk(KERN_INFO "FPU revision is: %08x\n", c->fpu_id);
}
