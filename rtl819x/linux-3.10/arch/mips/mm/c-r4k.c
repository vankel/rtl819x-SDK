/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1996 David S. Miller (davem@davemloft.net)
 * Copyright (C) 1997, 1998, 1999, 2000, 2001, 2002 Ralf Baechle (ralf@gnu.org)
 * Copyright (C) 1999, 2000 Silicon Graphics, Inc.
 *
 * Modified for RLX Linux for MIPS
 * Copyright (C) 2008-2011 Tony Wu (tonywu@realtek.com)
 */
#include <linux/hardirq.h>
#include <linux/init.h>
#include <linux/highmem.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/sched.h>
#include <linux/smp.h>
#include <linux/mm.h>
#include <linux/module.h>
#include <linux/bitops.h>

#include <asm/bcache.h>
#include <asm/bootinfo.h>
#include <asm/cache.h>
#include <asm/cacheops.h>
#include <asm/cpu.h>
#include <asm/cpu-features.h>
#include <asm/io.h>
#include <asm/page.h>
#include <asm/pgtable.h>
#include <asm/r4kcache.h>
#include <asm/sections.h>
#include <asm/mmu_context.h>
#include <asm/war.h>
#include <asm/cacheflush.h> /* for run_uncached() */
#include <asm/traps.h>
#include <asm/dma-coherence.h>

/*
 * Special Variant of smp_call_function for use by cache functions:
 *
 *  o No return value
 *  o collapses to normal function call on UP kernels
 *  o collapses to normal function call on systems with a single shared
 *    primary cache.
 *  o doesn't disable interrupts on the local CPU
 */
static inline void r4k_on_each_cpu(void (*func) (void *info), void *info)
{
	preempt_disable();

#if !defined(CONFIG_MIPS_MT_SMP) && !defined(CONFIG_MIPS_MT_SMTC)
	smp_call_function(func, info, 1);
#endif
	func(info);
	preempt_enable();
}

#if defined(CONFIG_MIPS_CMP)
#define cpu_has_safe_index_cacheops 0
#else
#define cpu_has_safe_index_cacheops 1
#endif

/*
 * Must die.
 */
static unsigned long icache_size __read_mostly;
static unsigned long dcache_size __read_mostly;
static unsigned long scache_size __read_mostly;

/*
 * Dummy cache handling routines for machines without boardcaches
 */
static void cache_noop(void) {}

static struct bcache_ops no_sc_ops = {
	.bc_enable = (void *)cache_noop,
	.bc_disable = (void *)cache_noop,
	.bc_wback_inv = (void *)cache_noop,
	.bc_inv = (void *)cache_noop
};

struct bcache_ops *bcops = &no_sc_ops;

#define cpu_is_r4600_v1_x()	((read_c0_prid() & 0xfffffff0) == 0x00002010)
#define cpu_is_r4600_v2_x()	((read_c0_prid() & 0xfffffff0) == 0x00002020)

#define R4600_HIT_CACHEOP_WAR_IMPL					\
do {									\
	if (R4600_V2_HIT_CACHEOP_WAR && cpu_is_r4600_v2_x())		\
		*(volatile unsigned long *)CKSEG1;			\
	if (R4600_V1_HIT_CACHEOP_WAR)					\
		__asm__ __volatile__("nop;nop;nop;nop");		\
} while (0)

static void (*r4k_blast_dcache_page)(unsigned long addr);

static inline void r4k_blast_dcache_page_dc32(unsigned long addr)
{
	R4600_HIT_CACHEOP_WAR_IMPL;
	blast_dcache32_page(addr);
}

static inline void r4k_blast_dcache_page_dc64(unsigned long addr)
{
	blast_dcache64_page(addr);
}

static void __cpuinit r4k_blast_dcache_page_setup(void)
{
	unsigned long  dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		r4k_blast_dcache_page = (void *)cache_noop;
	else if (dc_lsize == 16)
		r4k_blast_dcache_page = blast_dcache16_page;
	else if (dc_lsize == 32)
		r4k_blast_dcache_page = r4k_blast_dcache_page_dc32;
	else if (dc_lsize == 64)
		r4k_blast_dcache_page = r4k_blast_dcache_page_dc64;
}

#ifndef CONFIG_EVA
#define r4k_blast_dcache_user_page  r4k_blast_dcache_page
#else

static void (*r4k_blast_dcache_user_page)(unsigned long addr);

static void __cpuinit r4k_blast_dcache_user_page_setup(void)
{
	unsigned long  dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		r4k_blast_dcache_user_page = (void *)cache_noop;
	else if (dc_lsize == 16)
		r4k_blast_dcache_user_page = blast_dcache16_user_page;
	else if (dc_lsize == 32)
		r4k_blast_dcache_user_page = blast_dcache32_user_page;
	else if (dc_lsize == 64)
		r4k_blast_dcache_user_page = blast_dcache64_user_page;
}
#endif

static void (* r4k_blast_dcache_page_indexed)(unsigned long addr);

static void __cpuinit r4k_blast_dcache_page_indexed_setup(void)
{
	unsigned long dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		r4k_blast_dcache_page_indexed = (void *)cache_noop;
	else if (dc_lsize == 16)
		r4k_blast_dcache_page_indexed = blast_dcache16_page_indexed;
	else if (dc_lsize == 32)
		r4k_blast_dcache_page_indexed = blast_dcache32_page_indexed;
	else if (dc_lsize == 64)
		r4k_blast_dcache_page_indexed = blast_dcache64_page_indexed;
}

void (* r4k_blast_dcache)(void);
EXPORT_SYMBOL(r4k_blast_dcache);

static void __cpuinit r4k_blast_dcache_setup(void)
{
	unsigned long dc_lsize = cpu_dcache_line_size();

	if (dc_lsize == 0)
		r4k_blast_dcache = (void *)cache_noop;
	else if (dc_lsize == 16)
		r4k_blast_dcache = blast_dcache16;
	else if (dc_lsize == 32)
		r4k_blast_dcache = blast_dcache32;
	else if (dc_lsize == 64)
		r4k_blast_dcache = blast_dcache64;
}

/* force code alignment (used for TX49XX_ICACHE_INDEX_INV_WAR) */
#define JUMP_TO_ALIGN(order) \
	__asm__ __volatile__( \
		"b\t1f\n\t" \
		".align\t" #order "\n\t" \
		"1:\n\t" \
		)
#define CACHE32_UNROLL32_ALIGN	JUMP_TO_ALIGN(10) /* 32 * 32 = 1024 */
#define CACHE32_UNROLL32_ALIGN2 JUMP_TO_ALIGN(11)

static void (* r4k_blast_icache_page)(unsigned long addr);

static void __cpuinit r4k_blast_icache_page_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		r4k_blast_icache_page = (void *)cache_noop;
	else if (ic_lsize == 16)
		r4k_blast_icache_page = blast_icache16_page;
	else if (ic_lsize == 32)
		r4k_blast_icache_page = blast_icache32_page;
	else if (ic_lsize == 64)
		r4k_blast_icache_page = blast_icache64_page;
}
#define r4k_blast_icache_user_page  r4k_blast_icache_page

static void (* r4k_blast_icache_page_indexed)(unsigned long addr);

static void __cpuinit r4k_blast_icache_page_indexed_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		r4k_blast_icache_page_indexed = (void *)cache_noop;
	else if (ic_lsize == 16)
		r4k_blast_icache_page_indexed = blast_icache16_page_indexed;
	else if (ic_lsize == 32) {
		r4k_blast_icache_page_indexed = blast_icache32_page_indexed;
	} else if (ic_lsize == 64)
		r4k_blast_icache_page_indexed = blast_icache64_page_indexed;
}

void (* r4k_blast_icache)(void);
EXPORT_SYMBOL(r4k_blast_icache);

static void __cpuinit r4k_blast_icache_setup(void)
{
	unsigned long ic_lsize = cpu_icache_line_size();

	if (ic_lsize == 0)
		r4k_blast_icache = (void *)cache_noop;
	else if (ic_lsize == 16)
		r4k_blast_icache = blast_icache16;
	else if (ic_lsize == 32) {
		r4k_blast_icache = blast_icache32;
	} else if (ic_lsize == 64)
		r4k_blast_icache = blast_icache64;
}

static void (* r4k_blast_scache_page)(unsigned long addr);

static void __cpuinit r4k_blast_scache_page_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		r4k_blast_scache_page = (void *)cache_noop;
	else if (sc_lsize == 16)
		r4k_blast_scache_page = blast_scache16_page;
	else if (sc_lsize == 32)
		r4k_blast_scache_page = blast_scache32_page;
	else if (sc_lsize == 64)
		r4k_blast_scache_page = blast_scache64_page;
	else if (sc_lsize == 128)
		r4k_blast_scache_page = blast_scache128_page;
}

static void (* r4k_blast_scache_page_indexed)(unsigned long addr);

static void __cpuinit r4k_blast_scache_page_indexed_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		r4k_blast_scache_page_indexed = (void *)cache_noop;
	else if (sc_lsize == 16)
		r4k_blast_scache_page_indexed = blast_scache16_page_indexed;
	else if (sc_lsize == 32)
		r4k_blast_scache_page_indexed = blast_scache32_page_indexed;
	else if (sc_lsize == 64)
		r4k_blast_scache_page_indexed = blast_scache64_page_indexed;
	else if (sc_lsize == 128)
		r4k_blast_scache_page_indexed = blast_scache128_page_indexed;
}

static void (* r4k_blast_scache)(void);

static void __cpuinit r4k_blast_scache_setup(void)
{
	unsigned long sc_lsize = cpu_scache_line_size();

	if (scache_size == 0)
		r4k_blast_scache = (void *)cache_noop;
	else if (sc_lsize == 16)
		r4k_blast_scache = blast_scache16;
	else if (sc_lsize == 32)
		r4k_blast_scache = blast_scache32;
	else if (sc_lsize == 64)
		r4k_blast_scache = blast_scache64;
	else if (sc_lsize == 128)
		r4k_blast_scache = blast_scache128;
}

static inline void local_r4k___flush_cache_all(void * args)
{
	r4k_blast_dcache();
	r4k_blast_icache();
	r4k_blast_scache();
}

static void r4k___flush_cache_all(void)
{
	r4k_on_each_cpu(local_r4k___flush_cache_all, NULL);
}

static inline int has_valid_asid(const struct mm_struct *mm)
{
#if defined(CONFIG_MIPS_MT_SMP) || defined(CONFIG_MIPS_MT_SMTC)
	int i;

	for_each_online_cpu(i)
		if (cpu_context(i, mm))
			return 1;

	return 0;
#else
	return cpu_context(smp_processor_id(), mm);
#endif
}

static void r4k__flush_cache_vmap(void)
{
	r4k_blast_dcache();
}

static void r4k__flush_cache_vunmap(void)
{
	r4k_blast_dcache();
}

static inline void local_r4k_flush_cache_range(void * args)
{
	struct vm_area_struct *vma = args;
	int exec = vma->vm_flags & VM_EXEC;

	if (!(has_valid_asid(vma->vm_mm)))
		return;

	r4k_blast_dcache();
	if (exec)
	{
		wmb();
		r4k_blast_icache();
	}
}

static void r4k_flush_cache_range(struct vm_area_struct *vma,
	unsigned long start, unsigned long end)
{
	int exec = vma->vm_flags & VM_EXEC;

	if (cpu_has_dc_aliases || (exec && !cpu_has_ic_fills_f_dc))
		r4k_on_each_cpu(local_r4k_flush_cache_range, vma);
}

static inline void local_r4k_flush_cache_mm(void * args)
{
	struct mm_struct *mm = args;

	if (!has_valid_asid(mm))
		return;

	r4k_blast_dcache();
}

static void r4k_flush_cache_mm(struct mm_struct *mm)
{
	if (!cpu_has_dc_aliases)
		return;

	r4k_on_each_cpu(local_r4k_flush_cache_mm, mm);
}

struct flush_cache_page_args {
	struct vm_area_struct *vma;
	unsigned long addr;
	unsigned long pfn;
};

static inline void local_r4k_flush_cache_page(void *args)
{
	struct flush_cache_page_args *fcp_args = args;
	struct vm_area_struct *vma = fcp_args->vma;
	unsigned long addr = fcp_args->addr;
	struct page *page = pfn_to_page(fcp_args->pfn);
	int exec = vma->vm_flags & VM_EXEC;
	struct mm_struct *mm = vma->vm_mm;
	int map_coherent = 0;
	pgd_t *pgdp;
	pud_t *pudp;
	pmd_t *pmdp;
	pte_t *ptep;
	void *vaddr;
	int dontflash = 0;

	/*
	 * If ownes no valid ASID yet, cannot possibly have gotten
	 * this page into the cache.
	 */
	if (!has_valid_asid(mm))
		return;

	addr &= PAGE_MASK;
	pgdp = pgd_offset(mm, addr);
	pudp = pud_offset(pgdp, addr);
	pmdp = pmd_offset(pudp, addr);
	ptep = pte_offset(pmdp, addr);

	/*
	 * If the page isn't marked valid, the page cannot possibly be
	 * in the cache.
	 */
	if (!(pte_present(*ptep)))
		return;

	/*  accelerate it! See below, just skipping kmap_*()/kunmap_*() */
	if ((!exec) && !cpu_has_dc_aliases)
		return;

	if ((mm == current->active_mm) && (pte_val(*ptep) & _PAGE_VALID)) {
		if (cpu_has_dc_aliases || (exec && !cpu_has_ic_fills_f_dc)) {
			r4k_blast_dcache_user_page(addr);
			if (exec && !cpu_has_ic_fills_f_dc)
				wmb();
			if (exec && !cpu_icache_snoops_remote_store)
				r4k_blast_scache_page(addr);
		}
		if (exec)
			r4k_blast_icache_user_page(addr);	
	} else {
		/*
		 * Use kmap_coherent or kmap_atomic to do flushes for
		 * another ASID than the current one.
		 */
		map_coherent = (cpu_has_dc_aliases &&
				page_mapped(page) && !Page_dcache_dirty(page));
		if (map_coherent)
			vaddr = kmap_coherent(page, addr);
		else
			vaddr = kmap_atomic(page);
		addr = (unsigned long)vaddr;
	
		if (cpu_has_dc_aliases || (exec && !cpu_has_ic_fills_f_dc)) {
			r4k_blast_dcache_page(addr);
			if (exec && !cpu_has_ic_fills_f_dc)
				wmb();
			if (exec && !cpu_icache_snoops_remote_store)
				r4k_blast_scache_page(addr);
		}
		if (exec) {
			if (cpu_has_vtag_icache && mm == current->active_mm) {
				int cpu = smp_processor_id();

				if (cpu_context(cpu, mm) != 0)
					drop_mmu_context(mm, cpu);
				dontflash = 1;
			} else
			if (map_coherent  || !cpu_has_ic_aliases )
				r4k_blast_icache_page(addr);
		}

		if (map_coherent)
			kunmap_coherent();
		else
			kunmap_atomic(vaddr);
	
		/*  in case of I-cache aliasing - blast it via coherent page */	
	}
	/*  in case of I-cache aliasing - blast it via coherent page */
	if (exec && cpu_has_ic_aliases && (!dontflash) && !map_coherent) {
		vaddr = kmap_coherent(page, addr);
		r4k_blast_icache_page((unsigned long)vaddr);
		kunmap_coherent();
	}
}

static void r4k_flush_cache_page(struct vm_area_struct *vma,
	unsigned long addr, unsigned long pfn)
{
	struct flush_cache_page_args args;

	args.vma = vma;
	args.addr = addr;
	args.pfn = pfn;

	r4k_on_each_cpu(local_r4k_flush_cache_page, &args);
	if (cpu_has_dc_aliases)
		ClearPageDcacheDirty(pfn_to_page(pfn));
}

static inline void local_r4k_flush_data_cache_page(void * addr)
{
	r4k_blast_dcache_page((unsigned long) addr);
}

struct mips_flush_data_cache_range_args {
	struct vm_area_struct *vma;
	unsigned long vaddr;
	unsigned long start;
	unsigned long len;
};

static inline void local_r4k_mips_flush_data_cache_range(void *args)
{
	struct mips_flush_data_cache_range_args *f_args = args;
	unsigned long vaddr = f_args->vaddr;
	unsigned long start = f_args->start;
	unsigned long len = f_args->len;
	struct vm_area_struct * vma = f_args->vma;

	blast_dcache_range(start, start + len);

	if ((vma->vm_flags & VM_EXEC) && !cpu_has_ic_fills_f_dc) {
			wmb();

		/* vma is given for exec check only, mmap is current,
		   so - no non-current vma page flush, just user or kernel */
		protected_blast_icache_range(vaddr, vaddr + len);
	}
}

/* flush dirty kernel data and a corresponding user instructions (if needed).
   used in copy_to_user_page() */
static void r4k_mips_flush_data_cache_range(struct vm_area_struct *vma,
	unsigned long vaddr, struct page *page, unsigned long start,
	unsigned long len)
{
	struct mips_flush_data_cache_range_args args;

	args.vma = vma;
	args.vaddr = vaddr;
	args.start = start;
	args.len = len;

	r4k_on_each_cpu(local_r4k_mips_flush_data_cache_range, (void *)&args);
}

static void r4k_flush_data_cache_page(unsigned long addr)
{
	if (in_atomic())
		local_r4k_flush_data_cache_page((void *)addr);
	else
		r4k_on_each_cpu(local_r4k_flush_data_cache_page, (void *) addr);
}

struct flush_icache_range_args {
	unsigned long start;
	unsigned long end;
};

static inline void local_r4k_flush_icache_range(unsigned long start, unsigned long end)
{
	if (!cpu_has_ic_fills_f_dc) {
		if (end - start >= dcache_size) {
			r4k_blast_dcache();
		} else {
			R4600_HIT_CACHEOP_WAR_IMPL;
			protected_blast_dcache_range(start, end);
		}
	}
	wmb();
	if (end - start > icache_size)
		r4k_blast_icache();
	else
		protected_blast_icache_range(start, end);
}

static inline void local_r4k_flush_icache_range_ipi(void *args)
{
	struct flush_icache_range_args *fir_args = args;
	unsigned long start = fir_args->start;
	unsigned long end = fir_args->end;

	local_r4k_flush_icache_range(start, end);
}

static void r4k_flush_icache_range(unsigned long start, unsigned long end)
{
	struct flush_icache_range_args args;

	args.start = start;
	args.end = end;

	r4k_on_each_cpu(local_r4k_flush_icache_range_ipi, &args);
	instruction_hazard();
}

#ifdef CONFIG_DMA_NONCOHERENT

static void r4k_dma_cache_wback_inv(unsigned long addr, unsigned long size)
{
	/* Catch bad driver code */
	BUG_ON(size == 0);
	preempt_disable();
	if (cpu_has_inclusive_pcaches) {
		if (size >= scache_size)
			r4k_blast_scache();
		else
			blast_scache_range(addr, addr + size);
		preempt_enable();
		__sync();
		return;
	}

	/*
	 * Either no secondary cache or the available caches don't have the
	 * subset property so we have to flush the primary caches
	 * explicitly
	 */
	if (cpu_has_safe_index_cacheops && size >= dcache_size) {
		r4k_blast_dcache();
	} else {
		R4600_HIT_CACHEOP_WAR_IMPL;
		blast_dcache_range(addr, addr + size);
	}
 	preempt_enable();
	bc_wback_inv(addr, size);
	__sync();
}

static void r4k_dma_cache_inv(unsigned long addr, unsigned long size)
{
	/* Catch bad driver code */
	BUG_ON(size == 0);
	preempt_disable();
	if (cpu_has_inclusive_pcaches) {
		if (size >= scache_size)
		{
			r4k_blast_scache();
		}
		else {
			/*
			 * There is no clearly documented alignment requirement
			 * for the cache instruction on MIPS processors and
			 * some processors, among them the RM5200 and RM7000
			 * QED processors will throw an address error for cache
			 * hit ops with insufficient alignment.	 Solved by
			 * aligning the address to cache line size.
			 */
			blast_inv_scache_range(addr, addr + size);
		}
		preempt_enable();
		__sync();

		return;
	}

	if (cpu_has_safe_index_cacheops && size >= dcache_size) {
		r4k_blast_dcache();
	} else {
		R4600_HIT_CACHEOP_WAR_IMPL;
		blast_inv_dcache_range(addr, addr + size);
	}
	preempt_enable();
	bc_inv(addr, size);
	__sync();
}
#endif /* CONFIG_DMA_NONCOHERENT */

/*
 * While we're protected against bad userland addresses we don't care
 * very much about what happens in that case.  Usually a segmentation
 * fault will dump the process later on anyway ...
 */
static void local_r4k_flush_cache_sigtramp(void * arg)
{
	unsigned long ic_lsize = cpu_icache_line_size();
	unsigned long dc_lsize = cpu_dcache_line_size();
	unsigned long sc_lsize = cpu_scache_line_size();
	unsigned long addr = (unsigned long) arg;

	R4600_HIT_CACHEOP_WAR_IMPL;
	if (dc_lsize)
		protected_writeback_dcache_line(addr & ~(dc_lsize - 1));
	if (!cpu_icache_snoops_remote_store && scache_size)
		protected_writeback_scache_line(addr & ~(sc_lsize - 1));
	if (ic_lsize)
		protected_flush_icache_line(addr & ~(ic_lsize - 1));
	if (MIPS4K_ICACHE_REFILL_WAR) {
		__asm__ __volatile__ (
			".set push\n\t"
			".set noat\n\t"
			".set mips3\n\t"
#ifdef CONFIG_32BIT
			"la	$at,1f\n\t"
#endif
#ifdef CONFIG_64BIT
			"dla	$at,1f\n\t"
#endif
			"cache	%0,($at)\n\t"
			"nop; nop; nop\n"
			"1:\n\t"
			".set pop"
			:
			: "i" (Hit_Invalidate_I));
	}
	if (MIPS_CACHE_SYNC_WAR)
		__asm__ __volatile__ ("sync");
}

static void r4k_flush_cache_sigtramp(unsigned long addr)
{
	r4k_on_each_cpu(local_r4k_flush_cache_sigtramp, (void *) addr);
}

static void r4k_flush_icache_all(void)
{
	if (cpu_has_vtag_icache)
		r4k_blast_icache();
}

struct flush_kernel_vmap_range_args {
	unsigned long	vaddr;
	int		size;
};

static inline void local_r4k_flush_kernel_vmap_range(void *args)
{
	struct flush_kernel_vmap_range_args *vmra = args;
	unsigned long vaddr = vmra->vaddr;
	int size = vmra->size;

	/*
	 * Aliases only affect the primary caches so don't bother with
	 * S-caches or T-caches.
	 */
	if (cpu_has_safe_index_cacheops && size >= dcache_size)
		r4k_blast_dcache();
	else {
		R4600_HIT_CACHEOP_WAR_IMPL;
		blast_dcache_range(vaddr, vaddr + size);
	}
}

static void r4k_flush_kernel_vmap_range(unsigned long vaddr, int size)
{
	struct flush_kernel_vmap_range_args args;

	args.vaddr = (unsigned long) vaddr;
	args.size = size;

	r4k_on_each_cpu(local_r4k_flush_kernel_vmap_range, &args);
}

static char *way_string[] __cpuinitdata = { NULL, "direct mapped", "2-way",
	"3-way", "4-way", "5-way", "6-way", "7-way", "8-way"
};

static void __cpuinit probe_pcache(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;
	unsigned int config = read_c0_config();
	unsigned long config1;
	unsigned int lsize;

	/*
	 * So we seem to be a MIPS32 or MIPS64 CPU
	 * So let's probe the I-cache ...
	 */
	config1 = read_c0_config1();

	if ((lsize = ((config1 >> 19) & 7)))
		c->icache.linesz = 2 << lsize;
	else
		c->icache.linesz = lsize;
	c->icache.sets = 32 << (((config1 >> 22) + 1) & 7);
	c->icache.ways = 1 + ((config1 >> 16) & 7);

	icache_size = c->icache.sets *
	              c->icache.ways *
	              c->icache.linesz;
	c->icache.waybit = __ffs(icache_size/c->icache.ways);

	if (config & 0x8)		/* VI bit */
		c->icache.flags |= MIPS_CACHE_VTAG;

	/*
	 * Now probe the MIPS32 / MIPS64 data cache.
	 */
	c->dcache.flags = 0;

	if ((lsize = ((config1 >> 10) & 7)))
		c->dcache.linesz = 2 << lsize;
	else
		c->dcache.linesz= lsize;
	c->dcache.sets = 32 << (((config1 >> 13) + 1) & 7);
	c->dcache.ways = 1 + ((config1 >> 7) & 7);

	dcache_size = c->dcache.sets *
	              c->dcache.ways *
	              c->dcache.linesz;
	c->dcache.waybit = __ffs(dcache_size/c->dcache.ways);

	c->options |= (MIPS_CPU_PREFETCH);

	/* compute a couple of other cache variables */
	c->icache.waysize = icache_size / c->icache.ways;
	c->dcache.waysize = dcache_size / c->dcache.ways;

	c->icache.sets = c->icache.linesz ?
		icache_size / (c->icache.linesz * c->icache.ways) : 0;
	c->dcache.sets = c->dcache.linesz ?
		dcache_size / (c->dcache.linesz * c->dcache.ways) : 0;

	if ((read_c0_config7() & (1 << 16))) {
		/* effectively physically indexed dcache,
		   thus no virtual aliases. */
		c->dcache.flags |= (MIPS_CACHE_PINDEX );
	} else {
		//if (ic->dcache.waysize > PAGE_SIZE)
		if(1)	
			c->dcache.flags |= (MIPS_CACHE_ALIASES );
	}

	printk("Primary instruction cache %ldkB, %s, %s, linesize %d bytes.\n",
	       icache_size >> 10,
	       c->icache.flags & MIPS_CACHE_VTAG ? "VIVT" : "VIPT",
	       way_string[c->icache.ways], c->icache.linesz);

	printk("Primary data cache %ldkB, %s, %s, %s, linesize %d bytes\n",
	       dcache_size >> 10, way_string[c->dcache.ways],
	       (c->dcache.flags & MIPS_CACHE_PINDEX) ? "PIPT" : "VIPT",
	       (c->dcache.flags & MIPS_CACHE_ALIASES) ?
			"cache aliases" : "no aliases",
	       c->dcache.linesz);
}

extern int mips_sc_init(void);

static void __cpuinit setup_scache(void)
{
	struct cpuinfo_mips *c = &current_cpu_data;

#ifdef CONFIG_MIPS_CPU_SCACHE
	if (mips_sc_init ()) {
		scache_size = c->scache.ways * c->scache.sets * c->scache.linesz;
		printk("MIPS secondary cache %ldkB, %s, linesize %d bytes.\n",
		       scache_size >> 10,
		       way_string[c->scache.ways], c->scache.linesz);
	}
#else
	if (!(c->scache.flags & MIPS_CACHE_NOT_PRESENT))
		panic("Dunno how to handle MIPS32 / MIPS64 second level cache");
#endif
	return;
}

static int __cpuinitdata cca = -1;

static int __init cca_setup(char *str)
{
	get_option(&str, &cca);

	return 0;
}

early_param("cca", cca_setup);

static void __cpuinit coherency_setup(void)
{
#if defined CONFIG_MIPS_CMP
	cca = CONF_CM_CACHABLE_COW;
#elif defined CONFIG_CPU_HAS_WBC
	cca = CONF_CM_CACHABLE_NONCOHERENT;
#else
	if (cca < 0 || cca > 7)
		cca = read_c0_config() & CONF_CM_CMASK;
#endif
	_page_cachable_default = cca << _CACHE_SHIFT;

	pr_debug("Using cache attribute %d\n", cca);
	change_c0_config(CONF_CM_CMASK, cca);
}

static void __cpuinit r4k_cache_error_setup(void)
{
	extern char __weak except_vec2_generic;
	set_uncached_handler(0x100, &except_vec2_generic, 0x80);
}

void __cpuinit r4k_cache_init(void)
{
	extern void build_clear_page(void);
	extern void build_copy_page(void);
	struct cpuinfo_mips *c = &current_cpu_data;

	probe_pcache();
	setup_scache();

	r4k_blast_dcache_page_setup();
	r4k_blast_dcache_page_indexed_setup();
	r4k_blast_dcache_setup();
	r4k_blast_icache_page_setup();
	r4k_blast_icache_page_indexed_setup();
	r4k_blast_icache_setup();
	r4k_blast_scache_page_setup();
	r4k_blast_scache_page_indexed_setup();
	r4k_blast_scache_setup();

	/*
	 * Some MIPS32 and MIPS64 processors have physically indexed caches.
	 * This code supports virtually indexed processors and will be
	 * unnecessarily inefficient on physically indexed processors.
	 */
	if (c->dcache.linesz)
		shm_align_mask = max_t( unsigned long,
					c->dcache.sets * c->dcache.linesz - 1,
					PAGE_SIZE - 1);
	else
		shm_align_mask = PAGE_SIZE-1;

	__flush_cache_vmap	= r4k__flush_cache_vmap;
	__flush_cache_vunmap	= r4k__flush_cache_vunmap;

	flush_cache_all		= cache_noop;
	__flush_cache_all	= r4k___flush_cache_all;
	flush_cache_mm		= r4k_flush_cache_mm;
	flush_cache_page	= r4k_flush_cache_page;
	flush_cache_range	= r4k_flush_cache_range;

	__flush_kernel_vmap_range = r4k_flush_kernel_vmap_range;

	flush_cache_sigtramp	= r4k_flush_cache_sigtramp;
	flush_icache_all	= r4k_flush_icache_all;
	local_flush_data_cache_page	= local_r4k_flush_data_cache_page;
	flush_data_cache_page	= r4k_flush_data_cache_page;
	flush_icache_range	= r4k_flush_icache_range;
	local_flush_icache_range	= local_r4k_flush_icache_range;
	mips_flush_data_cache_range = r4k_mips_flush_data_cache_range;
#ifdef CONFIG_DMA_NONCOHERENT
	_dma_cache_wback_inv	= r4k_dma_cache_wback_inv;
	_dma_cache_wback	= r4k_dma_cache_wback_inv;
	_dma_cache_inv		= r4k_dma_cache_inv;
#endif

	build_clear_page();
	build_copy_page();

	/*
	 * We want to run CMP kernels on core with and without coherent
	 * caches. Therefore, do not use CONFIG_MIPS_CMP to decide whether
	 * or not to flush caches.
	 */
	local_r4k___flush_cache_all(NULL);

	coherency_setup();
	board_cache_error_setup = r4k_cache_error_setup;
}
