/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 - 2003, 06, 07 by Ralf Baechle (ralf@linux-mips.org)
 * Copyright (C) 2007 MIPS Technologies, Inc.
 */
#include <linux/fs.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/syscalls.h>
#include <linux/mm.h>

#include <asm/cacheflush.h>
#include <asm/processor.h>
#include <asm/cpu.h>
#include <asm/cpu-features.h>
#include <linux/highmem.h>
/* Cache operations. */
void (*flush_cache_all)(void);
void (*__flush_cache_all)(void);
void (*flush_cache_mm)(struct mm_struct *mm);
void (*flush_cache_range)(struct vm_area_struct *vma, unsigned long start,
	unsigned long end);
void (*flush_cache_page)(struct vm_area_struct *vma, unsigned long page,
	unsigned long pfn);
void (*flush_icache_range)(unsigned long start, unsigned long end);
void (*local_flush_icache_range)(unsigned long start, unsigned long end);

void (*__flush_cache_vmap)(void);
void (*__flush_cache_vunmap)(void);

void (*__flush_kernel_vmap_range)(unsigned long vaddr, int size);
void (*__invalidate_kernel_vmap_range)(unsigned long vaddr, int size);

void (*mips_flush_data_cache_range)(struct vm_area_struct *vma,
      unsigned long vaddr, struct page *page, unsigned long addr,
      unsigned long size);

EXPORT_SYMBOL_GPL(__flush_kernel_vmap_range);

/* MIPS specific cache operations */
void (*flush_cache_sigtramp)(unsigned long addr);
void (*local_flush_data_cache_page)(void * addr);
void (*flush_data_cache_page)(unsigned long addr);
void (*flush_icache_all)(void);

EXPORT_SYMBOL_GPL(local_flush_data_cache_page);
EXPORT_SYMBOL(flush_data_cache_page);
EXPORT_SYMBOL(flush_icache_all);

#ifdef CONFIG_DMA_NONCOHERENT

/* DMA cache operations. */
void (*_dma_cache_wback_inv)(unsigned long start, unsigned long size);
void (*_dma_cache_wback)(unsigned long start, unsigned long size);
void (*_dma_cache_inv)(unsigned long start, unsigned long size);

EXPORT_SYMBOL(_dma_cache_wback_inv);

#endif /* CONFIG_DMA_NONCOHERENT */

/*
 * We could optimize the case where the cache argument is not BCACHE but
 * that seems very atypical use ...
 */
SYSCALL_DEFINE3(cacheflush, unsigned long, addr, unsigned long, bytes,
	unsigned int, cache)
{
	if (bytes == 0)
		return 0;
	if (!access_ok(VERIFY_WRITE, (void __user *) addr, bytes))
		return -EFAULT;

	flush_icache_range(addr, addr + bytes);

	return 0;
}

void __flush_dcache_page(struct page *page)
{
//	struct address_space *mapping = page_mapping(page);
//	unsigned long addr;
	void *addr;
//	if (PageHighMem(page))
//		return;
//	if (mapping && !mapping_mapped(mapping)) {
	if (page_mapping(page) && !page_mapped(page)) {
		SetPageDcacheDirty(page);
		return;
	}

	/*
	 * We could delay the flush for the !page_mapping case too.  But that
	 * case is for exec env/arg pages and those are %99 certainly going to
	 * get faulted into the tlb (and thus flushed) anyways.
	 */
	//addr = (unsigned long) page_address(page);
	//flush_data_cache_page(addr);
	if (PageHighMem(page)) {
		addr = kmap_atomic(page);
		flush_data_cache_page((unsigned long)addr);
		kunmap_atomic(addr);
	} else {
		addr = (void *) page_address(page);
		flush_data_cache_page((unsigned long)addr);
	}
	ClearPageDcacheDirty(page);
}

EXPORT_SYMBOL(__flush_dcache_page);


void __flush_icache_page(struct vm_area_struct *vma, struct page *page)
{
       unsigned long addr;

       if (PageHighMem(page))
               return;

       addr = (unsigned long) page_address(page);
       flush_data_cache_page(addr);
}
EXPORT_SYMBOL_GPL(__flush_icache_page);



void __flush_anon_page(struct page *page, unsigned long vmaddr)
{
	unsigned long addr = (unsigned long) page_address(page);
#if 0
	if (pages_do_alias(addr, vmaddr)) {
		if (page_mapped(page) && !Page_dcache_dirty(page)) {
			void *kaddr;

			kaddr = kmap_coherent(page, vmaddr);
			flush_data_cache_page((unsigned long)kaddr);
			kunmap_coherent();
		} else
			flush_data_cache_page(addr);
	}
#endif
{

	if (pages_do_alias((unsigned long)addr, vmaddr & PAGE_MASK)) {
			if (page_mapped(page) && !Page_dcache_dirty(page)) {
				void *kaddr;

				kaddr = kmap_coherent(page, vmaddr);
				flush_data_cache_page((unsigned long)kaddr);
				kunmap_coherent();
			} else {
				void *kaddr;

				kaddr = kmap_atomic(page);
				flush_data_cache_page((unsigned long)kaddr);
				kunmap_atomic(kaddr);
				ClearPageDcacheDirty(page);
			}
		}
}
}

EXPORT_SYMBOL(__flush_anon_page);

void __update_cache(struct vm_area_struct *vma, unsigned long address,
	pte_t pte)
{
	struct page *page;
	unsigned long pfn;//, addr;
//	int exec = (vma->vm_flags & VM_EXEC) && !cpu_has_ic_fills_f_dc;

	pfn = pte_pfn(pte);
	if (unlikely(!pfn_valid(pfn))){
				wmb();
		return;
	}
	page = pfn_to_page(pfn);
	if (page_mapping(page) && Page_dcache_dirty(page)) {
#if 0
		addr = (unsigned long) page_address(page);
		if (exec || pages_do_alias(addr, address & PAGE_MASK))
			flush_data_cache_page(addr);
		ClearPageDcacheDirty(page);
#endif
         unsigned long page_addr = (unsigned long) page_address(page);
        void *kaddr = NULL;
        if (!cpu_has_ic_fills_f_dc ||
	pages_do_alias(page_addr, address & PAGE_MASK)) {
	flush_data_cache_page(page_addr);
	ClearPageDcacheDirty(page);
	}
	if (kaddr)
	kunmap_atomic((void *)kaddr);
	}
	wmb();
	
}

unsigned long _page_cachable_default;
EXPORT_SYMBOL(_page_cachable_default);

static inline void setup_protection_map(void)
{
	protection_map[0] = PAGE_NONE;
	protection_map[1] = PAGE_READONLY;
	protection_map[2] = PAGE_COPY;
	protection_map[3] = PAGE_COPY;
	protection_map[4] = PAGE_READONLY;
	protection_map[5] = PAGE_READONLY;
	protection_map[6] = PAGE_COPY;
	protection_map[7] = PAGE_COPY;
	protection_map[8] = PAGE_NONE;
	protection_map[9] = PAGE_READONLY;
	protection_map[10] = PAGE_SHARED;
	protection_map[11] = PAGE_SHARED;
	protection_map[12] = PAGE_READONLY;
	protection_map[13] = PAGE_READONLY;
	protection_map[14] = PAGE_SHARED;
	protection_map[15] = PAGE_SHARED;
}

void __cpuinit cpu_cache_init(void)
{
	extern void __weak r4k_cache_init(void);

	r4k_cache_init();
	setup_protection_map();
}

int __weak __uncached_access(struct file *file, unsigned long addr)
{
	if (file->f_flags & O_DSYNC)
		return 1;

	return addr >= __pa(high_memory);
}
