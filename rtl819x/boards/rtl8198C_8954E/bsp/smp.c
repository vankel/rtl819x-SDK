/*
 * Realtek Semiconductor Corp.
 *
 * bsp/smp.c
 *     bsp SMP initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#ifdef CONFIG_USE_UAPI
#include <generated/uapi/linux/version.h>
#else
#include <linux/version.h>
#endif
#include <linux/smp.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

#include <asm/gcmpregs.h>
#include <asm/smp-ops.h>

#include "bspchip.h"

#ifdef CONFIG_SMP
/*
 * Called in kernel/smp-*.c to do secondary CPU initialization.
 *
 * All platform-specific initialization should be implemented in
 * this function.
 *
 * Known SMP callers are:
 *     kernel/smp-mt.c
 *     kernel/smp-cmp.c
 */
void __cpuinit bsp_smp_init_secondary(void)
{
#ifdef CONFIG_MIPS_MT_SMP
	change_c0_status(ST0_IM, STATUSF_IP0 | STATUSF_IP1 |
				 STATUSF_IP2 | STATUSF_IP7);
#endif
#ifdef CONFIG_MIPS_CMP
	change_c0_status(ST0_IM, STATUSF_IP3 | STATUSF_IP4 |
				 STATUSF_IP6 | STATUSF_IP7);
#endif
	change_c0_status(ST0_IM, 0xff00);
}

/*
 * Called in bsp/setup.c to initialize SMP operations
 *
 * Depends on SMP type, bsp_smp_init calls corresponding
 * SMP operation initializer in arch/mips/kernel
 *
 * Known SMP types are:
 *     CONFIG_MIPS_CMP
 *     CONFIG_MIPS_MT_SMP
 *     CONFIG_MIPS_MT_SMTC
 */
void __init bsp_smp_init(void)
{
#ifdef CONFIG_MIPS_CMP
        if (gcmp_probe(GCMP_BASE_ADDR, GCMP_BASE_SIZE))
                register_cmp_smp_ops();
#endif
#ifdef CONFIG_MIPS_MT_SMP
        register_vsmp_smp_ops();
#endif
#ifdef CONFIG_MIPS_MT_SMTC
        register_smp_ops(&bsp_smtc_smp_ops);
#endif
}
#endif
