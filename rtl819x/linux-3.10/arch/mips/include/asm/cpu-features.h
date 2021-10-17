/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2003, 2004 Ralf Baechle
 * Copyright (C) 2004  Maciej W. Rozycki
 *
 * Modified for RLX Linux for MIPS
 * Tony Wu (tonywu@realtek.com)
 */
#ifndef __ASM_CPU_FEATURES_H
#define __ASM_CPU_FEATURES_H

#include <asm/cpu.h>
#include <asm/cpu-info.h>

#include <bspcpu.h>

#ifdef CONFIG_CPU_MIPS4K
#define current_cpu_type()		CPU_4KEC
#endif
#ifdef CONFIG_CPU_MIPS24K
#define current_cpu_type()		CPU_24K
#endif
#ifdef CONFIG_CPU_MIPS34K
#define current_cpu_type()		CPU_34K
#endif
#ifdef CONFIG_CPU_MIPS74K
#define current_cpu_type()		CPU_74K
#endif
#ifdef CONFIG_CPU_MIPS1004K
#define current_cpu_type()		CPU_1004K
#endif
#ifdef CONFIG_CPU_MIPS1074K
#define current_cpu_type()		CPU_74K
#endif

#ifndef current_cpu_type
#define current_cpu_type()		current_cpu_data.cputype
#endif

/*
 * SMP assumption: Options of CPU 0 are a superset of all processors.
 * This is true for all known MIPS systems.
 */
#define cpu_has_tlb			1
#define cpu_has_4kex			1
#define cpu_has_3k_cache		0
#define cpu_has_6k_cache		0
#define cpu_has_8k_cache		0
#define cpu_has_4k_cache		1
#define cpu_has_tx39_cache		0
#define cpu_has_octeon_cache		0

#ifdef CONFIG_CPU_HAS_FPU
#define cpu_has_fpu			1
#define raw_cpu_has_fpu			1
#define cpu_has_32fpr			1
#define cpu_has_nofpuex			0
#else
#define cpu_has_fpu			0
#define raw_cpu_has_fpu			0
#define cpu_has_32fpr			0
#define cpu_has_nofpuex			1
#endif

#define cpu_has_counter			1
#ifdef CONFIG_CPU_HAS_WATCH
#define cpu_has_watch			1
#else
#define cpu_has_watch			0
#endif
#define cpu_has_divec			1
#define cpu_has_vce			0
#define cpu_has_cache_cdex_p		0
#define cpu_has_cache_cdex_s		0
#define cpu_has_prefetch		1
#define cpu_has_mcheck			1
#define cpu_has_ejtag			0
#define cpu_has_llsc			1
#ifndef kernel_uses_llsc
#define kernel_uses_llsc		cpu_has_llsc
#endif
#ifndef cpu_has_mips16
#define cpu_has_mips16			1
#endif
#ifndef cpu_has_mdmx
#define cpu_has_mdmx			0
#endif
#ifndef cpu_has_mips3d
#define cpu_has_mips3d			0
#endif
#ifndef cpu_has_smartmips
#define cpu_has_smartmips		0
#endif
#ifndef cpu_has_rixi
#define cpu_has_rixi			0
#endif
#ifndef cpu_has_mmips
#define cpu_has_mmips			0
#endif
#ifndef cpu_has_vtag_icache
#define cpu_has_vtag_icache		0
#endif
#ifdef CONFIG_CPU_HAS_AR7
#define cpu_has_dc_aliases		0
#define cpu_has_pindexed_dcache		1
#else
#define cpu_has_dc_aliases		(cpu_data[0].dcache.flags & MIPS_CACHE_ALIASES)
#define cpu_has_pindexed_dcache		(cpu_data[0].dcache.flags & MIPS_CACHE_PINDEX)
#endif
#ifndef cpu_has_ic_fills_f_dc
#define cpu_has_ic_fills_f_dc		0
#endif
#ifndef cpu_has_local_ebase
#define cpu_has_local_ebase		0
#endif

/*
 * I-Cache snoops remote store.	 This only matters on SMP.  Some multiprocessors
 * such as the R10000 have I-Caches that snoop local stores; the embedded ones
 * don't.  For maintaining I-cache coherency this means we need to flush the
 * D-cache all the way back to whever the I-cache does refills from, so the
 * I-cache has a chance to see the new data at all.  Then we have to flush the
 * I-cache also.
 * Note we may have been rescheduled and may no longer be running on the CPU
 * that did the store so we can't optimize this into only doing the flush on
 * the local CPU.
 */
#ifndef cpu_icache_snoops_remote_store
#ifdef CONFIG_SMP
#define cpu_icache_snoops_remote_store	(cpu_data[0].icache.flags & MIPS_IC_SNOOPS_REMOTE)
#else
#define cpu_icache_snoops_remote_store	1
#endif
#endif

#ifndef cpu_has_mips_1
# define cpu_has_mips_1			0
#endif
#ifndef cpu_has_mips_2
# define cpu_has_mips_2			0
#endif
#ifndef cpu_has_mips_3
# define cpu_has_mips_3			0
#endif
#ifndef cpu_has_mips_4
# define cpu_has_mips_4			0
#endif
#ifndef cpu_has_mips_5
# define cpu_has_mips_5			0
#endif

#ifndef cpu_has_mips32r1
#define cpu_has_mips32r1		0
#endif
#ifndef cpu_has_mips32r2
#define cpu_has_mips32r2		1
#endif
#ifndef cpu_has_mips64r1
#define cpu_has_mips64r1		0
#endif
#ifndef cpu_has_mips64r2
#define cpu_has_mips64r2		0
#endif

/*
 * Shortcuts ...
 */
#define cpu_has_mips32	(cpu_has_mips32r1 | cpu_has_mips32r2)
#define cpu_has_mips64	(cpu_has_mips64r1 | cpu_has_mips64r2)
#define cpu_has_mips_r1 (cpu_has_mips32r1 | cpu_has_mips64r1)
#define cpu_has_mips_r2 (cpu_has_mips32r2 | cpu_has_mips64r2)
#define cpu_has_mips_r	(cpu_has_mips32r1 | cpu_has_mips32r2 | \
			 cpu_has_mips64r1 | cpu_has_mips64r2)

#ifndef cpu_has_mips_r2_exec_hazard
# if defined CONFIG_CPU_MIPS14K || defined CONFIG_CPU_MIPS74K
#  define cpu_has_mips_r2_exec_hazard	0
# else
#  define cpu_has_mips_r2_exec_hazard	cpu_has_mips_r2
# endif
#endif

/*
 * MIPS32, MIPS64, VR5500, IDT32332, IDT32334 and maybe a few other
 * pre-MIPS32/MIPS53 processors have CLO, CLZ.	The IDT RC64574 is 64-bit and
 * has CLO and CLZ but not DCLO nor DCLZ.  For 64-bit kernels
 * cpu_has_clo_clz also indicates the availability of DCLO and DCLZ.
 */
#ifndef cpu_has_clo_clz
#define cpu_has_clo_clz			cpu_has_mips_r
#endif

#ifdef CONFIG_CPU_HAS_DSP
#define cpu_has_dsp			1
#else
#define cpu_has_dsp			0
#endif

#ifndef cpu_has_dsp2
#define cpu_has_dsp2			0
#endif

#ifdef CONFIG_CPU_HAS_MIPSMT
#define cpu_has_mipsmt			1
#else
#define cpu_has_mipsmt			0
#endif

#ifdef CONFIG_CPU_HAS_TLS
#define cpu_has_userlocal		1
#else
#define cpu_has_userlocal		0
#endif

#ifndef cpu_has_64bits
#define cpu_has_64bits			0
#endif
#ifndef cpu_has_64bit_zero_reg
#define cpu_has_64bit_zero_reg		0
#endif
#ifndef cpu_has_64bit_gp_regs
#define cpu_has_64bit_gp_regs		0
#endif
#ifndef cpu_has_64bit_addresses
#define cpu_has_64bit_addresses		0
#endif
#ifndef cpu_vmbits
#define cpu_vmbits			31
#endif

#if defined(CONFIG_CPU_MIPSR2_IRQ_VI) && !defined(cpu_has_vint)
# define cpu_has_vint			1
#elif !defined(cpu_has_vint)
# define cpu_has_vint			0
#endif

#if defined(CONFIG_CPU_MIPSR2_IRQ_EI) && !defined(cpu_has_veic)
# define cpu_has_veic			1
#elif !defined(cpu_has_veic)
# define cpu_has_veic			0
#endif

#define cpu_has_inclusive_pcaches	0

#ifndef cpu_dcache_line_size
#define cpu_dcache_line_size()		cpu_data[0].dcache.linesz
#endif
#ifndef cpu_icache_line_size
#define cpu_icache_line_size()		cpu_data[0].icache.linesz
#endif
#ifndef cpu_scache_line_size
#define cpu_scache_line_size()		cpu_data[0].scache.linesz
#endif

#ifndef cpu_hwrena_impl_bits
#define cpu_hwrena_impl_bits		0
#endif

#ifndef cpu_has_perf_cntr_intr_bit
#define cpu_has_perf_cntr_intr_bit	(cpu_data[0].options & MIPS_CPU_PCI)
#endif

#ifndef cpu_has_vz
#define cpu_has_vz			0
#endif
#ifndef cpu_has_vtag_dcache
#define cpu_has_vtag_dcache     (cpu_data[0].dcache.flags & MIPS_CACHE_VTAG)
#endif
#ifndef cpu_has_ic_aliases
#define cpu_has_ic_aliases      (cpu_data[0].icache.flags & MIPS_CACHE_ALIASES)
#endif
#endif /* __ASM_CPU_FEATURES_H */
