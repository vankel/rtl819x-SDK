/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Synthesize TLB refill handlers at runtime.
 *
 * Copyright (C) 2004, 2005, 2006, 2008	 Thiemo Seufer
 * Copyright (C) 2005, 2007, 2008, 2009	 Maciej W. Rozycki
 * Copyright (C) 2006  Ralf Baechle (ralf@linux-mips.org)
 * Copyright (C) 2008, 2009 Cavium Networks, Inc.
 * Copyright (C) 2011  MIPS Technologies, Inc.
 *
 * ... and the days got worse and worse and now you see
 * I've gone completly out of my mind.
 *
 * They're coming to take me a away haha
 * they're coming to take me a away hoho hihi haha
 * to the funny farm where code is beautiful all the time ...
 *
 * (Condolences to Napoleon XIV)
 */

#include <linux/bug.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/smp.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/cache.h>

#include <asm/cacheflush.h>
#include <asm/pgtable.h>
#include <asm/war.h>
#include <asm/uasm.h>
#include <asm/setup.h>

/*
 * TLB load/store/modify handlers.
 *
 * Only the fastpath gets synthesized at runtime, the slowpath for
 * do_page_fault remains normal asm.
 */
extern void tlb_do_page_fault_0(void);
extern void tlb_do_page_fault_1(void);

struct work_registers {
	int r1;
	int r2;
	int r3;
};

struct tlb_reg_save {
	unsigned long a;
	unsigned long b;
} ____cacheline_aligned_in_smp;

static struct tlb_reg_save handler_reg_save[NR_CPUS];

/* Handle labels (which must be positive integers). */
enum label_id {
	label_second_part = 1,
	label_leave,
	label_vmalloc,
	label_vmalloc_done,
	label_tlbw_hazard_0,
	label_split = label_tlbw_hazard_0 + 8,
	label_tlbl_goaround1,
	label_tlbl_goaround2,
	label_nopage_tlbl,
	label_nopage_tlbs,
	label_nopage_tlbm,
	label_smp_pgtable_change,
	label_large_segbits_fault,
#ifdef CONFIG_MIPS_HUGE_TLB_SUPPORT
	label_tlb_huge_update,
#endif
};

UASM_L_LA(_second_part)
UASM_L_LA(_leave)
UASM_L_LA(_vmalloc)
UASM_L_LA(_vmalloc_done)
/* _tlbw_hazard_x is handled differently.  */
UASM_L_LA(_split)
UASM_L_LA(_tlbl_goaround1)
UASM_L_LA(_tlbl_goaround2)
UASM_L_LA(_nopage_tlbl)
UASM_L_LA(_nopage_tlbs)
UASM_L_LA(_nopage_tlbm)
UASM_L_LA(_smp_pgtable_change)
UASM_L_LA(_large_segbits_fault)
#ifdef CONFIG_MIPS_HUGE_TLB_SUPPORT
UASM_L_LA(_tlb_huge_update)
#endif

/*
 * pgtable bits are assigned dynamically depending on processor feature
 * and statically based on kernel configuration.  This spits out the actual
 * values the kernel is using.	Required to make sense from disassembled
 * TLB exception handlers.
 */
static void output_pgtable_bits_defines(void)
{
#define pr_define(fmt, ...)					\
	pr_debug("#define " fmt, ##__VA_ARGS__)

	pr_debug("#include <asm/asm.h>\n");
	pr_debug("#include <asm/regdef.h>\n");
	pr_debug("\n");

	pr_define("_PAGE_PRESENT_SHIFT %d\n", _PAGE_PRESENT_SHIFT);
	pr_define("_PAGE_READ_SHIFT %d\n", _PAGE_READ_SHIFT);
	pr_define("_PAGE_WRITE_SHIFT %d\n", _PAGE_WRITE_SHIFT);
	pr_define("_PAGE_ACCESSED_SHIFT %d\n", _PAGE_ACCESSED_SHIFT);
	pr_define("_PAGE_MODIFIED_SHIFT %d\n", _PAGE_MODIFIED_SHIFT);
#ifdef CONFIG_MIPS_HUGE_TLB_SUPPORT
	pr_define("_PAGE_HUGE_SHIFT %d\n", _PAGE_HUGE_SHIFT);
	pr_define("_PAGE_SPLITTING_SHIFT %d\n", _PAGE_SPLITTING_SHIFT);
#endif
	if (cpu_has_rixi) {
#ifdef _PAGE_NO_EXEC_SHIFT
		pr_define("_PAGE_NO_EXEC_SHIFT %d\n", _PAGE_NO_EXEC_SHIFT);
#endif
#ifdef _PAGE_NO_READ_SHIFT
		pr_define("_PAGE_NO_READ_SHIFT %d\n", _PAGE_NO_READ_SHIFT);
#endif
	}
	pr_define("_PAGE_GLOBAL_SHIFT %d\n", _PAGE_GLOBAL_SHIFT);
	pr_define("_PAGE_VALID_SHIFT %d\n", _PAGE_VALID_SHIFT);
	pr_define("_PAGE_DIRTY_SHIFT %d\n", _PAGE_DIRTY_SHIFT);
	pr_define("_PFN_SHIFT %d\n", _PFN_SHIFT);
	pr_debug("\n");
}

static inline void dump_handler(const char *symbol, const u32 *handler, int count)
{
	int i;

	pr_debug("LEAF(%s)\n", symbol);

	pr_debug("\t.set push\n");
	pr_debug("\t.set noreorder\n");

	for (i = 0; i < count; i++)
		pr_debug("\t.word\t0x%08x\t\t# %p\n", handler[i], &handler[i]);

	pr_debug("\t.set\tpop\n");

	pr_debug("\tEND(%s)\n", symbol);
}

/* The only general purpose registers allowed in TLB handlers. */
#define K0		26
#define K1		27

/* Some CP0 registers */
#define C0_INDEX	0, 0
#define C0_ENTRYLO0	2, 0
#define C0_TCBIND	2, 2
#define C0_ENTRYLO1	3, 0
#define C0_CONTEXT	4, 0
#define C0_PAGEMASK	5, 0
#define C0_BADVADDR	8, 0
#define C0_ENTRYHI	10, 0
#define C0_EPC		14, 0
#define C0_XCONTEXT	20, 0

#ifdef CONFIG_64BIT
# define GET_CONTEXT(buf, reg) UASM_i_MFC0(buf, reg, C0_XCONTEXT)
#else
# define GET_CONTEXT(buf, reg) UASM_i_MFC0(buf, reg, C0_CONTEXT)
#endif

/* The worst case length of the handler is around 18 instructions for
 * R3000-style TLBs and up to 63 instructions for R4000-style TLBs.
 * Maximum space available is 32 instructions for R3000 and 64
 * instructions for R4000.
 *
 * We deliberately chose a buffer size of 128, so we won't scribble
 * over anything important on overflow before we panic.
 */
static u32 tlb_handler[128] __cpuinitdata;

/* simply assume worst case size for labels and relocs */
static struct uasm_label labels[128] __cpuinitdata;
static struct uasm_reloc relocs[128] __cpuinitdata;


static struct work_registers __cpuinit build_get_work_registers(u32 **p)
{
	struct work_registers r;

	int smp_processor_id_reg;
	int smp_processor_id_sel;
	int smp_processor_id_shift;

	if (num_possible_cpus() > 1) {
#ifdef CONFIG_MIPS_PGD_C0_CONTEXT
		smp_processor_id_shift = 51;
		smp_processor_id_reg = 20; /* XContext */
		smp_processor_id_sel = 0;
#else
# ifdef CONFIG_32BIT
		smp_processor_id_shift = 25;
		smp_processor_id_reg = 4; /* Context */
		smp_processor_id_sel = 0;
# endif
# ifdef CONFIG_64BIT
		smp_processor_id_shift = 26;
		smp_processor_id_reg = 4; /* Context */
		smp_processor_id_sel = 0;
# endif
#endif
		/* Get smp_processor_id */
		UASM_i_MFC0(p, K0, smp_processor_id_reg, smp_processor_id_sel);
		UASM_i_SRL_SAFE(p, K0, K0, smp_processor_id_shift);

		/* handler_reg_save index in K0 */
		UASM_i_SLL(p, K0, K0, ilog2(sizeof(struct tlb_reg_save)));

		UASM_i_LA(p, K1, (long)&handler_reg_save);
		UASM_i_ADDU(p, K0, K0, K1);
	} else {
		UASM_i_LA(p, K0, (long)&handler_reg_save);
	}
	/* K0 now points to save area, save $1 and $2  */
	UASM_i_SW(p, 1, offsetof(struct tlb_reg_save, a), K0);
	UASM_i_SW(p, 2, offsetof(struct tlb_reg_save, b), K0);

	r.r1 = K1;
	r.r2 = 1;
	r.r3 = 2;
	return r;
}

static void __cpuinit build_restore_work_registers(u32 **p)
{
	/* K0 already points to save area, restore $1 and $2  */
	UASM_i_LW(p, 1, offsetof(struct tlb_reg_save, a), K0);
	UASM_i_LW(p, 2, offsetof(struct tlb_reg_save, b), K0);
}

#ifndef CONFIG_MIPS_PGD_C0_CONTEXT

/*
 * CONFIG_MIPS_PGD_C0_CONTEXT implies 64 bit and lack of pgd_current,
 * we cannot do r3000 under these circumstances.
 *
 * Declare pgd_current here instead of including mmu_context.h to avoid type
 * conflicts for tlbmiss_handler_setup_pgd
 */
extern unsigned long pgd_current[];

#endif /* CONFIG_MIPS_PGD_C0_CONTEXT */

/*
 * The R4000 TLB handler is much more complicated. We have two
 * consecutive handler areas with 32 instructions space each.
 * Since they aren't used at the same time, we can overflow in the
 * other one.To keep things simple, we first assume linear space,
 * then we relocate it to the final handler layout as needed.
 */
static u32 final_handler[64] __cpuinitdata;

/*
 * Hazards
 *
 * From the IDT errata for the QED RM5230 (Nevada), processor revision 1.0:
 * 2. A timing hazard exists for the TLBP instruction.
 *
 *	stalling_instruction
 *	TLBP
 *
 * The JTLB is being read for the TLBP throughout the stall generated by the
 * previous instruction. This is not really correct as the stalling instruction
 * can modify the address used to access the JTLB.  The failure symptom is that
 * the TLBP instruction will use an address created for the stalling instruction
 * and not the address held in C0_ENHI and thus report the wrong results.
 *
 * The software work-around is to not allow the instruction preceding the TLBP
 * to stall - make it an NOP or some other instruction guaranteed not to stall.
 *
 * Errata 2 will not be fixed.	This errata is also on the R5000.
 *
 * As if we MIPS hackers wouldn't know how to nop pipelines happy ...
 */
static void __cpuinit __maybe_unused build_tlb_probe_entry(u32 **p)
{
	uasm_i_tlbp(p);
}

/*
 * Write random or indexed TLB entry, and care about the hazards from
 * the preceding mtc0 and for the following eret.
 */
enum tlb_write_entry { tlb_random, tlb_indexed };

static void __cpuinit build_tlb_write_entry(u32 **p, struct uasm_label **l,
					 struct uasm_reloc **r,
					 enum tlb_write_entry wmode)
{
	void(*tlbw)(u32 **) = NULL;

	switch (wmode) {
	case tlb_random: tlbw = uasm_i_tlbwr; break;
	case tlb_indexed: tlbw = uasm_i_tlbwi; break;
	}

	if (cpu_has_mips_r2) {
		/*
		 * The architecture spec says an ehb is required here,
		 * but a number of cores do not have the hazard and
		 * using an ehb causes an expensive pipeline stall.
		 */
		switch (current_cpu_type()) {
		case CPU_M14KC:
		case CPU_74K:
			break;

		default:
			uasm_i_ehb(p);
			break;
		}
		tlbw(p);
		return;
	}
}

static __cpuinit __maybe_unused void build_convert_pte_to_entrylo(u32 **p,
								  unsigned int reg)
{
	if (cpu_has_rixi) {
		UASM_i_ROTR(p, reg, reg, ilog2(_PAGE_GLOBAL));
	} else {
		UASM_i_SRL(p, reg, reg, ilog2(_PAGE_GLOBAL));
	}
}

/*
 * TMP and PTR are scratch.
 * TMP will be clobbered, PTR will hold the pgd entry.
 */
static void __cpuinit __maybe_unused
build_get_pgde32(u32 **p, unsigned int tmp, unsigned int ptr)
{
	long pgdc = (long)pgd_current;

	/* 32 bit SMP has smp_processor_id() stored in CONTEXT. */
#ifdef CONFIG_SMP
#ifdef	CONFIG_MIPS_MT_SMTC
	/*
	 * SMTC uses TCBind value as "CPU" index
	 */
	uasm_i_mfc0(p, ptr, C0_TCBIND);
	UASM_i_LA_mostly(p, tmp, pgdc);
	uasm_i_srl(p, ptr, ptr, 19);
#else
	/*
	 * smp_processor_id() << 3 is stored in CONTEXT.
	 */
	uasm_i_mfc0(p, ptr, C0_CONTEXT);
	UASM_i_LA_mostly(p, tmp, pgdc);
	uasm_i_srl(p, ptr, ptr, 23);
#endif
	uasm_i_addu(p, ptr, tmp, ptr);
#else
	UASM_i_LA_mostly(p, ptr, pgdc);
#endif
	uasm_i_mfc0(p, tmp, C0_BADVADDR); /* get faulting address */
	uasm_i_lw(p, ptr, uasm_rel_lo(pgdc), ptr);
	uasm_i_srl(p, tmp, tmp, PGDIR_SHIFT); /* get pgd only bits */
	uasm_i_sll(p, tmp, tmp, PGD_T_LOG2);
	uasm_i_addu(p, ptr, ptr, tmp); /* add in pgd offset */
}

static void __cpuinit build_adjust_context(u32 **p, unsigned int ctx)
{
	unsigned int shift = 4 - (PTE_T_LOG2 + 1) + PAGE_SHIFT - 12;
	unsigned int mask = (PTRS_PER_PTE / 2 - 1) << (PTE_T_LOG2 + 1);

	if (shift)
		UASM_i_SRL(p, ctx, ctx, shift);
	uasm_i_andi(p, ctx, ctx, mask);
}

static void __cpuinit build_get_ptep(u32 **p, unsigned int tmp, unsigned int ptr)
{
	/*
	 * Bug workaround for the Nevada. It seems as if under certain
	 * circumstances the move from cp0_context might produce a
	 * bogus result when the mfc0 instruction and its consumer are
	 * in a different cacheline or a load instruction, probably any
	 * memory reference, is between them.
	 */
	GET_CONTEXT(p, tmp); /* get context reg */
	UASM_i_LW(p, ptr, 0, ptr);

	build_adjust_context(p, tmp);
	UASM_i_ADDU(p, ptr, ptr, tmp); /* add in offset */
}

static void __cpuinit build_update_entries(u32 **p, unsigned int tmp,
					unsigned int ptep)
{
	UASM_i_LW(p, tmp, 0, ptep); /* get even pte */
	UASM_i_LW(p, ptep, sizeof(pte_t), ptep); /* get odd pte */
	if (cpu_has_rixi) {
		UASM_i_SRL(p, tmp, tmp, ilog2(_PAGE_NO_EXEC));
		UASM_i_SRL(p, ptep, ptep, ilog2(_PAGE_NO_EXEC));
		UASM_i_ROTR(p, tmp, tmp, ilog2(_PAGE_GLOBAL) - ilog2(_PAGE_NO_EXEC));
		UASM_i_MTC0(p, tmp, C0_ENTRYLO0); /* load it */
		UASM_i_ROTR(p, ptep, ptep, ilog2(_PAGE_GLOBAL));
	} else {
		UASM_i_SRL(p, tmp, tmp, ilog2(_PAGE_GLOBAL)); /* convert to entrylo0 */
		UASM_i_MTC0(p, tmp, C0_ENTRYLO0); /* load it */
		UASM_i_SRL(p, ptep, ptep, ilog2(_PAGE_GLOBAL)); /* convert to entrylo1 */
	}
	UASM_i_MTC0(p, ptep, C0_ENTRYLO1); /* load it */
}

static void __cpuinit build_r4000_tlb_refill_handler(void)
{
	u32 *p = tlb_handler;
	struct uasm_label *l = labels;
	struct uasm_reloc *r = relocs;
	u32 *f;
	unsigned int final_len;

	memset(tlb_handler, 0, sizeof(tlb_handler));
	memset(labels, 0, sizeof(labels));
	memset(relocs, 0, sizeof(relocs));
	memset(final_handler, 0, sizeof(final_handler));

	/*
	 * create the plain linear handler
	 */
	build_get_pgde32(&p, K0, K1); /* get pgd in K1 */

	build_get_ptep(&p, K0, K1);
	build_update_entries(&p, K0, K1);
	build_tlb_write_entry(&p, &l, &r, tlb_random);
	uasm_l_leave(&l, p);
	uasm_i_eret(&p); /* return from trap */

	/*
	 * Overflow check: For the 64bit handler, we need at least one
	 * free instruction slot for the wrap-around branch. In worst
	 * case, if the intended insertion point is a delay slot, we
	 * need three, with the second nop'ed and the third being
	 * unused.
	 */
	if ((p - tlb_handler) > 64)
		panic("TLB refill handler space exceeded");

	/*
	 * Now fold the handler in the TLB refill handler space.
	 */
	f = final_handler;
	/* Simplest case, just copy the handler. */
	uasm_copy_handler(relocs, labels, tlb_handler, p, f);
	final_len = p - tlb_handler;

	uasm_resolve_relocs(relocs, labels);
	pr_debug("Wrote TLB refill handler (%u instructions).\n",
		 final_len);

	memcpy((void *)ebase, final_handler, 0x100);

	dump_handler("r4000_tlb_refill", (u32 *)ebase, 64);
}

/*
 * 128 instructions for the fastpath handler is generous and should
 * never be exceeded.
 */
#define FASTPATH_SIZE 128

u32 handle_tlbl[FASTPATH_SIZE] __cacheline_aligned;
u32 handle_tlbs[FASTPATH_SIZE] __cacheline_aligned;
u32 handle_tlbm[FASTPATH_SIZE] __cacheline_aligned;
#ifdef CONFIG_MIPS_PGD_C0_CONTEXT
u32 tlbmiss_handler_setup_pgd_array[16] __cacheline_aligned;

static void __cpuinit build_r4000_setup_pgd(void)
{
	const int a0 = 4;
	const int a1 = 5;
	u32 *p = tlbmiss_handler_setup_pgd_array;
	struct uasm_label *l = labels;
	struct uasm_reloc *r = relocs;

	memset(tlbmiss_handler_setup_pgd_array, 0, sizeof(tlbmiss_handler_setup_pgd_array));
	memset(labels, 0, sizeof(labels));
	memset(relocs, 0, sizeof(relocs));

	pgd_reg = allocate_kscratch();

	if (pgd_reg == -1) {
		/* PGD << 11 in c0_Context */
		/*
		 * If it is a ckseg0 address, convert to a physical
		 * address.  Shifting right by 29 and adding 4 will
		 * result in zero for these addresses.
		 *
		 */
		UASM_i_SRA(&p, a1, a0, 29);
		UASM_i_ADDIU(&p, a1, a1, 4);
		uasm_il_bnez(&p, &r, a1, label_tlbl_goaround1);
		uasm_i_nop(&p);
		uasm_i_dinsm(&p, a0, 0, 29, 64 - 29);
		uasm_l_tlbl_goaround1(&l, p);
		UASM_i_SLL(&p, a0, a0, 11);
		uasm_i_jr(&p, 31);
		UASM_i_MTC0(&p, a0, C0_CONTEXT);
	} else {
		/* PGD in c0_KScratch */
		uasm_i_jr(&p, 31);
		UASM_i_MTC0(&p, a0, 31, pgd_reg);
	}
	if (p - tlbmiss_handler_setup_pgd_array > ARRAY_SIZE(tlbmiss_handler_setup_pgd_array))
		panic("tlbmiss_handler_setup_pgd_array space exceeded");
	uasm_resolve_relocs(relocs, labels);
	pr_debug("Wrote tlbmiss_handler_setup_pgd_array (%u instructions).\n",
		 (unsigned int)(p - tlbmiss_handler_setup_pgd_array));

	dump_handler("tlbmiss_handler",
		     tlbmiss_handler_setup_pgd_array,
		     ARRAY_SIZE(tlbmiss_handler_setup_pgd_array));
}
#endif

static void __cpuinit
iPTE_LW(u32 **p, unsigned int pte, unsigned int ptr)
{
#ifdef CONFIG_SMP
	UASM_i_LL(p, pte, 0, ptr);
#else
	UASM_i_LW(p, pte, 0, ptr);
#endif
}

static void __cpuinit
iPTE_SW(u32 **p, struct uasm_reloc **r, unsigned int pte, unsigned int ptr,
	unsigned int mode)
{
	uasm_i_ori(p, pte, pte, mode);
#ifdef CONFIG_SMP
	UASM_i_SC(p, pte, 0, ptr);

	uasm_il_beqz(p, r, pte, label_smp_pgtable_change);
	uasm_i_nop(p);
#else
	UASM_i_SW(p, pte, 0, ptr);
#endif
}

/*
 * Check if PTE is present, if not then jump to LABEL. PTR points to
 * the page table where this PTE is located, PTE will be re-loaded
 * with it's original value.
 */
static void __cpuinit
build_pte_present(u32 **p, struct uasm_reloc **r,
		  int pte, int ptr, int scratch, enum label_id lid)
{
	int t = scratch >= 0 ? scratch : pte;

	if (cpu_has_rixi) {
		uasm_i_andi(p, t, pte, _PAGE_PRESENT);
		uasm_il_beqz(p, r, t, lid);
		if (pte == t)
			/* You lose the SMP race :-(*/
			iPTE_LW(p, pte, ptr);
	} else {
		uasm_i_andi(p, t, pte, _PAGE_PRESENT | _PAGE_READ);
		uasm_i_xori(p, t, t, _PAGE_PRESENT | _PAGE_READ);
		uasm_il_bnez(p, r, t, lid);
		if (pte == t)
			/* You lose the SMP race :-(*/
			iPTE_LW(p, pte, ptr);
	}
}

/* Make PTE valid, store result in PTR. */
static void __cpuinit
build_make_valid(u32 **p, struct uasm_reloc **r, unsigned int pte,
		 unsigned int ptr)
{
	unsigned int mode = _PAGE_VALID | _PAGE_ACCESSED;

	iPTE_SW(p, r, pte, ptr, mode);
}

/*
 * Check if PTE can be written to, if not branch to LABEL. Regardless
 * restore PTE with value from PTR when done.
 */
static void __cpuinit
build_pte_writable(u32 **p, struct uasm_reloc **r,
		   unsigned int pte, unsigned int ptr, int scratch,
		   enum label_id lid)
{
	int t = scratch >= 0 ? scratch : pte;

	uasm_i_andi(p, t, pte, _PAGE_PRESENT | _PAGE_WRITE);
	uasm_i_xori(p, t, t, _PAGE_PRESENT | _PAGE_WRITE);
	uasm_il_bnez(p, r, t, lid);
	if (pte == t)
		/* You lose the SMP race :-(*/
		iPTE_LW(p, pte, ptr);
	else
		uasm_i_nop(p);
}

/* Make PTE writable, update software status bits as well, then store
 * at PTR.
 */
static void __cpuinit
build_make_write(u32 **p, struct uasm_reloc **r, unsigned int pte,
		 unsigned int ptr)
{
	unsigned int mode = (_PAGE_ACCESSED | _PAGE_MODIFIED | _PAGE_VALID
			     | _PAGE_DIRTY);

	iPTE_SW(p, r, pte, ptr, mode);
}

/*
 * Check if PTE can be modified, if not branch to LABEL. Regardless
 * restore PTE with value from PTR when done.
 */
static void __cpuinit
build_pte_modifiable(u32 **p, struct uasm_reloc **r,
		     unsigned int pte, unsigned int ptr, int scratch,
		     enum label_id lid)
{
	int t = scratch >= 0 ? scratch : pte;
	uasm_i_andi(p, t, pte, _PAGE_WRITE);
	uasm_il_beqz(p, r, t, lid);
	if (pte == t)
		/* You lose the SMP race :-(*/
		iPTE_LW(p, pte, ptr);
}

/*
 * R4000 style TLB load/store/modify handlers.
 */
static struct work_registers __cpuinit
build_r4000_tlbchange_handler_head(u32 **p, struct uasm_label **l,
				   struct uasm_reloc **r)
{
	struct work_registers wr = build_get_work_registers(p);

	build_get_pgde32(p, wr.r1, wr.r2); /* get pgd in ptr */

	UASM_i_MFC0(p, wr.r1, C0_BADVADDR);
	UASM_i_LW(p, wr.r2, 0, wr.r2);
	UASM_i_SRL(p, wr.r1, wr.r1, PAGE_SHIFT + PTE_ORDER - PTE_T_LOG2);
	uasm_i_andi(p, wr.r1, wr.r1, (PTRS_PER_PTE - 1) << PTE_T_LOG2);
	UASM_i_ADDU(p, wr.r2, wr.r2, wr.r1);

#ifdef CONFIG_SMP
	uasm_l_smp_pgtable_change(l, *p);
#endif
	iPTE_LW(p, wr.r1, wr.r2); /* get even pte */
	build_tlb_probe_entry(p);
	return wr;
}

static void __cpuinit
build_r4000_tlbchange_handler_tail(u32 **p, struct uasm_label **l,
				   struct uasm_reloc **r, unsigned int tmp,
				   unsigned int ptr)
{
	uasm_i_ori(p, ptr, ptr, sizeof(pte_t));
	uasm_i_xori(p, ptr, ptr, sizeof(pte_t));
	build_update_entries(p, tmp, ptr);
	build_tlb_write_entry(p, l, r, tlb_indexed);
	uasm_l_leave(l, *p);
	build_restore_work_registers(p);
	uasm_i_eret(p); /* return from trap */
}

static void __cpuinit build_r4000_tlb_load_handler(void)
{
	u32 *p = handle_tlbl;
	struct uasm_label *l = labels;
	struct uasm_reloc *r = relocs;
	struct work_registers wr;

	memset(handle_tlbl, 0, sizeof(handle_tlbl));
	memset(labels, 0, sizeof(labels));
	memset(relocs, 0, sizeof(relocs));

	wr = build_r4000_tlbchange_handler_head(&p, &l, &r);
	build_pte_present(&p, &r, wr.r1, wr.r2, wr.r3, label_nopage_tlbl);

	if (cpu_has_rixi) {
		/*
		 * If the page is not _PAGE_VALID, RI or XI could not
		 * have triggered it.  Skip the expensive test..
		 */
		uasm_i_andi(&p, wr.r3, wr.r1, _PAGE_VALID);
		uasm_il_beqz(&p, &r, wr.r3, label_tlbl_goaround1);
		uasm_i_nop(&p);

		uasm_i_tlbr(&p);

		switch (current_cpu_type()) {
		default:
			if (cpu_has_mips_r2) {
				uasm_i_ehb(&p);

		case CPU_CAVIUM_OCTEON:
		case CPU_CAVIUM_OCTEON_PLUS:
		case CPU_CAVIUM_OCTEON2:
				break;
			}
		}

		/* Examine  entrylo 0 or 1 based on ptr. */
		uasm_i_andi(&p, wr.r3, wr.r2, sizeof(pte_t));
		uasm_i_beqz(&p, wr.r3, 8);
		/* load it in the delay slot*/
		UASM_i_MFC0(&p, wr.r3, C0_ENTRYLO0);
		/* load it if ptr is odd */
		UASM_i_MFC0(&p, wr.r3, C0_ENTRYLO1);
		/*
		 * If the entryLo (now in wr.r3) is valid (bit 1), RI or
		 * XI must have triggered it.
		 */
		uasm_i_andi(&p, wr.r3, wr.r3, 2);
		uasm_il_bnez(&p, &r, wr.r3, label_nopage_tlbl);
		uasm_i_nop(&p);
		uasm_l_tlbl_goaround1(&l, p);
	}
	build_make_valid(&p, &r, wr.r1, wr.r2);
	build_r4000_tlbchange_handler_tail(&p, &l, &r, wr.r1, wr.r2);

#ifdef CONFIG_MIPS_HUGE_TLB_SUPPORT
	/*
	 * This is the entry point when build_r4000_tlbchange_handler_head
	 * spots a huge page.
	 */
	uasm_l_tlb_huge_update(&l, p);
	iPTE_LW(&p, wr.r1, wr.r2);
	build_pte_present(&p, &r, wr.r1, wr.r2, wr.r3, label_nopage_tlbl);
	build_tlb_probe_entry(&p);

	if (cpu_has_rixi) {
		/*
		 * If the page is not _PAGE_VALID, RI or XI could not
		 * have triggered it.  Skip the expensive test..
		 */
		if (use_bbit_insns()) {
			uasm_il_bbit0(&p, &r, wr.r1, ilog2(_PAGE_VALID),
				      label_tlbl_goaround2);
		} else {
			uasm_i_andi(&p, wr.r3, wr.r1, _PAGE_VALID);
			uasm_il_beqz(&p, &r, wr.r3, label_tlbl_goaround2);
		}
		uasm_i_nop(&p);

		uasm_i_tlbr(&p);

		switch (current_cpu_type()) {
		default:
			if (cpu_has_mips_r2) {
				uasm_i_ehb(&p);

		case CPU_CAVIUM_OCTEON:
		case CPU_CAVIUM_OCTEON_PLUS:
		case CPU_CAVIUM_OCTEON2:
				break;
			}
		}

		/* Examine  entrylo 0 or 1 based on ptr. */
		if (use_bbit_insns()) {
			uasm_i_bbit0(&p, wr.r2, ilog2(sizeof(pte_t)), 8);
		} else {
			uasm_i_andi(&p, wr.r3, wr.r2, sizeof(pte_t));
			uasm_i_beqz(&p, wr.r3, 8);
		}
		/* load it in the delay slot*/
		UASM_i_MFC0(&p, wr.r3, C0_ENTRYLO0);
		/* load it if ptr is odd */
		UASM_i_MFC0(&p, wr.r3, C0_ENTRYLO1);
		/*
		 * If the entryLo (now in wr.r3) is valid (bit 1), RI or
		 * XI must have triggered it.
		 */
		if (use_bbit_insns()) {
			uasm_il_bbit0(&p, &r, wr.r3, 1, label_tlbl_goaround2);
		} else {
			uasm_i_andi(&p, wr.r3, wr.r3, 2);
			uasm_il_beqz(&p, &r, wr.r3, label_tlbl_goaround2);
		}
		if (PM_DEFAULT_MASK == 0)
			uasm_i_nop(&p);
		/*
		 * We clobbered C0_PAGEMASK, restore it.  On the other branch
		 * it is restored in build_huge_tlb_write_entry.
		 */
		build_restore_pagemask(&p, &r, wr.r3, label_nopage_tlbl, 0);

		uasm_l_tlbl_goaround2(&l, p);
	}
	uasm_i_ori(&p, wr.r1, wr.r1, (_PAGE_ACCESSED | _PAGE_VALID));
	build_huge_handler_tail(&p, &r, &l, wr.r1, wr.r2);
#endif

	uasm_l_nopage_tlbl(&l, p);
	build_restore_work_registers(&p);
#ifdef CONFIG_CPU_MICROMIPS
	if ((unsigned long)tlb_do_page_fault_0 & 1) {
		uasm_i_lui(&p, K0, uasm_rel_hi((long)tlb_do_page_fault_0));
		uasm_i_addiu(&p, K0, K0, uasm_rel_lo((long)tlb_do_page_fault_0));
		uasm_i_jr(&p, K0);
	} else
#endif
	uasm_i_j(&p, (unsigned long)tlb_do_page_fault_0 & 0x0fffffff);
	uasm_i_nop(&p);

	if ((p - handle_tlbl) > FASTPATH_SIZE)
		panic("TLB load handler fastpath space exceeded");

	uasm_resolve_relocs(relocs, labels);
	pr_debug("Wrote TLB load handler fastpath (%u instructions).\n",
		 (unsigned int)(p - handle_tlbl));

	dump_handler("r4000_tlb_load", handle_tlbl, ARRAY_SIZE(handle_tlbl));
}

static void __cpuinit build_r4000_tlb_store_handler(void)
{
	u32 *p = handle_tlbs;
	struct uasm_label *l = labels;
	struct uasm_reloc *r = relocs;
	struct work_registers wr;

	memset(handle_tlbs, 0, sizeof(handle_tlbs));
	memset(labels, 0, sizeof(labels));
	memset(relocs, 0, sizeof(relocs));

	wr = build_r4000_tlbchange_handler_head(&p, &l, &r);
	build_pte_writable(&p, &r, wr.r1, wr.r2, wr.r3, label_nopage_tlbs);
	build_make_write(&p, &r, wr.r1, wr.r2);
	build_r4000_tlbchange_handler_tail(&p, &l, &r, wr.r1, wr.r2);

	uasm_l_nopage_tlbs(&l, p);
	build_restore_work_registers(&p);
#ifdef CONFIG_CPU_MICROMIPS
	if ((unsigned long)tlb_do_page_fault_1 & 1) {
		uasm_i_lui(&p, K0, uasm_rel_hi((long)tlb_do_page_fault_1));
		uasm_i_addiu(&p, K0, K0, uasm_rel_lo((long)tlb_do_page_fault_1));
		uasm_i_jr(&p, K0);
	} else
#endif
	uasm_i_j(&p, (unsigned long)tlb_do_page_fault_1 & 0x0fffffff);
	uasm_i_nop(&p);

	if ((p - handle_tlbs) > FASTPATH_SIZE)
		panic("TLB store handler fastpath space exceeded");

	uasm_resolve_relocs(relocs, labels);
	pr_debug("Wrote TLB store handler fastpath (%u instructions).\n",
		 (unsigned int)(p - handle_tlbs));

	dump_handler("r4000_tlb_store", handle_tlbs, ARRAY_SIZE(handle_tlbs));
}

static void __cpuinit build_r4000_tlb_modify_handler(void)
{
	u32 *p = handle_tlbm;
	struct uasm_label *l = labels;
	struct uasm_reloc *r = relocs;
	struct work_registers wr;

	memset(handle_tlbm, 0, sizeof(handle_tlbm));
	memset(labels, 0, sizeof(labels));
	memset(relocs, 0, sizeof(relocs));

	wr = build_r4000_tlbchange_handler_head(&p, &l, &r);
	build_pte_modifiable(&p, &r, wr.r1, wr.r2, wr.r3, label_nopage_tlbm);
	/* Present and writable bits set, set accessed and dirty bits. */
	build_make_write(&p, &r, wr.r1, wr.r2);
	build_r4000_tlbchange_handler_tail(&p, &l, &r, wr.r1, wr.r2);

	uasm_l_nopage_tlbm(&l, p);
	build_restore_work_registers(&p);
#ifdef CONFIG_CPU_MICROMIPS
	if ((unsigned long)tlb_do_page_fault_1 & 1) {
		uasm_i_lui(&p, K0, uasm_rel_hi((long)tlb_do_page_fault_1));
		uasm_i_addiu(&p, K0, K0, uasm_rel_lo((long)tlb_do_page_fault_1));
		uasm_i_jr(&p, K0);
	} else
#endif
	uasm_i_j(&p, (unsigned long)tlb_do_page_fault_1 & 0x0fffffff);
	uasm_i_nop(&p);

	if ((p - handle_tlbm) > FASTPATH_SIZE)
		panic("TLB modify handler fastpath space exceeded");

	uasm_resolve_relocs(relocs, labels);
	pr_debug("Wrote TLB modify handler fastpath (%u instructions).\n",
		 (unsigned int)(p - handle_tlbm));

	dump_handler("r4000_tlb_modify", handle_tlbm, ARRAY_SIZE(handle_tlbm));
}

static void __cpuinit flush_tlb_handlers(void)
{
	local_flush_icache_range((unsigned long)handle_tlbl,
			   (unsigned long)handle_tlbl + sizeof(handle_tlbl));
	local_flush_icache_range((unsigned long)handle_tlbs,
			   (unsigned long)handle_tlbs + sizeof(handle_tlbs));
	local_flush_icache_range((unsigned long)handle_tlbm,
			   (unsigned long)handle_tlbm + sizeof(handle_tlbm));
#ifdef CONFIG_MIPS_PGD_C0_CONTEXT
	local_flush_icache_range((unsigned long)tlbmiss_handler_setup_pgd_array,
			   (unsigned long)tlbmiss_handler_setup_pgd_array + sizeof(tlbmiss_handler_setup_pgd_array));
#endif
}

void __cpuinit build_tlb_refill_handler(void)
{
	/*
	 * The refill handler is generated per-CPU, multi-node systems
	 * may have local storage for it. The other handlers are only
	 * needed once.
	 */
	static int run_once = 0;

	output_pgtable_bits_defines();

	if (!run_once) {
#ifdef CONFIG_MIPS_PGD_C0_CONTEXT
		build_r4000_setup_pgd();
#endif
		build_r4000_tlb_load_handler();
		build_r4000_tlb_store_handler();
		build_r4000_tlb_modify_handler();
		if (!cpu_has_local_ebase)
			build_r4000_tlb_refill_handler();
		flush_tlb_handlers();
		run_once++;
	}
	if (cpu_has_local_ebase)
		build_r4000_tlb_refill_handler();
}
