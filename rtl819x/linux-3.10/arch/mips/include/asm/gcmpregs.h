/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2000, 07 MIPS Technologies, Inc.
 *
 * Multiprocessor Subsystem Register Definitions
 *
 */
#ifndef _ASM_GCMPREGS_H
#define _ASM_GCMPREGS_H

#ifndef __ASSEMBLY__
extern unsigned long gcmp_base; /* defined in arch/mips/kernel/gcmp.c */
#endif

/* Offsets to major blocks within GCMP from GCMP base */
#define GCMP_GCB_OFS		0x0000 /* Global Control Block */
#define GCMP_CLCB_OFS		0x2000 /* Core Local Control Block */
#define GCMP_COCB_OFS		0x4000 /* Core Other Control Block */
#define GCMP_GDB_OFS		0x8000 /* Global Debug Block */

/* Offsets to individual GCMP registers from GCMP base */
#define GCMPOFS(block, tag, reg)	\
	(GCMP_##block##_OFS + GCMP_##tag##_##reg##_OFS)
#define GCMPOFSn(block, tag, reg, n) \
	(GCMP_##block##_OFS + GCMP_##tag##_##reg##_OFS(n))

#define GCMPGCBOFS(reg)		GCMPOFS(GCB, GCB, reg)
#define GCMPGCBOFSn(reg, n)	GCMPOFSn(GCB, GCB, reg, n)
#define GCMPCLCBOFS(reg)	GCMPOFS(CLCB, CCB, reg)
#define GCMPCOCBOFS(reg)	GCMPOFS(COCB, CCB, reg)
#define GCMPGDBOFS(reg)		GCMPOFS(GDB, GDB, reg)

/* GCMP register access */
#define GCMPGCB(reg)			REGP(gcmp_base, GCMPGCBOFS(reg))
#define GCMPGCBn(reg, n)		REGP(gcmp_base, GCMPGCBOFSn(reg, n))
#define GCMPCLCB(reg)			REGP(gcmp_base, GCMPCLCBOFS(reg))
#define GCMPCOCB(reg)			REGP(gcmp_base, GCMPCOCBOFS(reg))
#define GCMPGDB(reg)			REGP(gcmp_base, GCMPGDBOFS(reg))

/* Mask generation */
#define GCMPMSK(block, reg, bits)	(MSK(bits)<<GCMP_##block##_##reg##_SHF)
#define GCMPGCBMSK(reg, bits)		GCMPMSK(GCB, reg, bits)
#define GCMPCCBMSK(reg, bits)		GCMPMSK(CCB, reg, bits)
#define GCMPGDBMSK(reg, bits)		GCMPMSK(GDB, reg, bits)

/* GCB registers */
#define GCMP_GCB_GC_OFS			0x0000	/* Global Config Register */
#define	 GCMP_GCB_GC_NUMIOCU_SHF	8
#define	 GCMP_GCB_GC_NUMIOCU_MSK	GCMPGCBMSK(GC_NUMIOCU, 4)
#define	 GCMP_GCB_GC_NUMCORES_SHF	0
#define	 GCMP_GCB_GC_NUMCORES_MSK	GCMPGCBMSK(GC_NUMCORES, 8)
#define GCMP_GCB_GCMPB_OFS		0x0008		/* Global GCMP Base */
#define	 GCMP_GCB_GCMPB_GCMPBASE_SHF	15
#define	 GCMP_GCB_GCMPB_GCMPBASE_MSK	GCMPGCBMSK(GCMPB_GCMPBASE, 17)
#define	 GCMP_GCB_GCMPB_CMDEFTGT_SHF	0
#define	 GCMP_GCB_GCMPB_CMDEFTGT_MSK	GCMPGCBMSK(GCMPB_CMDEFTGT, 2)
#define	 GCMP_GCB_GCMPB_CMDEFTGT_DISABLED	0
#define	 GCMP_GCB_GCMPB_CMDEFTGT_MEM		1
#define	 GCMP_GCB_GCMPB_CMDEFTGT_IOCU1		2
#define	 GCMP_GCB_GCMPB_CMDEFTGT_IOCU2		3
#define GCMP_GCB_CCMC_OFS		0x0010	/* Global CM Control */
#define GCMP_GCB_GCSRAP_OFS		0x0020	/* Global CSR Access Privilege */
#define	 GCMP_GCB_GCSRAP_CMACCESS_SHF	0
#define	 GCMP_GCB_GCSRAP_CMACCESS_MSK	GCMPGCBMSK(GCSRAP_CMACCESS, 8)
#define GCMP_GCB_GCMPREV_OFS		0x0030	/* GCMP Revision Register */
#define GCMP_GCB_GCMEM_OFS		0x0040	/* Global CM Error Mask */
#define GCMP_GCB_GCMEC_OFS		0x0048	/* Global CM Error Cause */
#define	 GCMP_GCB_GMEC_ERROR_TYPE_SHF	27
#define	 GCMP_GCB_GMEC_ERROR_TYPE_MSK	GCMPGCBMSK(GMEC_ERROR_TYPE, 5)
#define	 GCMP_GCB_GMEC_ERROR_INFO_SHF	0
#define	 GCMP_GCB_GMEC_ERROR_INFO_MSK	GCMPGCBMSK(GMEC_ERROR_INFO, 27)
#define GCMP_GCB_GCMEA_OFS		0x0050	/* Global CM Error Address */
#define GCMP_GCB_GCMEO_OFS		0x0058	/* Global CM Error Multiple */
#define	 GCMP_GCB_GMEO_ERROR_2ND_SHF	0
#define	 GCMP_GCB_GMEO_ERROR_2ND_MSK	GCMPGCBMSK(GMEO_ERROR_2ND, 5)
#define GCMP_GCB_GICBA_OFS		0x0080	/* Global Interrupt Controller Base Address */
#define  GCMP_GCB_GICBA_BASE_SHF	17
#define  GCMP_GCB_GICBA_BASE_MSK	GCMPGCBMSK(GICBA_BASE, 15)
#define  GCMP_GCB_GICBA_EN_SHF		0
#define  GCMP_GCB_GICBA_EN_MSK		GCMPGCBMSK(GICBA_EN, 1)
#define GCMP_GCB_CPCBA_OFS		0x0088  /* CPC Base Address */
#define  GCMP_GCB_CPCBA_BASE_SHF	17
#define  GCMP_GCB_CPCBA_BASE_MSK	GCMPGCBMSK(CPCBA_BASE, 15)
#define  GCMP_GCB_CPCBA_EN_SHF		0
#define  GCMP_GCB_CPCBA_EN_MSK		GCMPGCBMSK(CPCBA_EN, 1)
#define GCMP_GCB_GICSR_OFS		0x00D0  /* GIC Status */
#define  GCMP_GCB_GICSR_EX_SHF		0
#define  GCMP_GCB_GICSR_EX_MSK		GCMPGCBMSK(GICSR_EX, 1)
#define GCMP_GCB_CR_OFS			0x00E0  /* Cache Revision */
#define  GCMP_GCB_CR_MAJOR_SHF		8
#define  GCMP_GCB_CR_MAJOR_MSK		GCMPGCBMSK(CR_MAJOR, 8)
#define  GCMP_GCB_CR_MINOR_SHF		0
#define  GCMP_GCB_CR_MINOR_MSK		GCMPGCBMSK(CR_MINOR, 8)
#define GCMP_GCB_CPCSR_OFS		0x00F0  /* CPC Status */
#define  GCMP_GCB_CPCSR_EX_SHF		0
#define  GCMP_GCB_CPCSR_EX_MSK		GCMPGCBMSK(CPCSR_EX, 1)
#define GCMP_GCB_IOCUR_OFS		0x0200  /* IOCU Revision */
#define  GCMP_GCB_IOCUR_MAJOR_SHF	8
#define  GCMP_GCB_IOCUR_MAJOR_MSK	GCMPGCBMSK(IOCUR_MAJOR, 8)
#define  GCMP_GCB_IOCUR_MINOR_SHF	0
#define  GCMP_GCB_IOCUR_MINOR_MSK	GCMPGCBMSK(IOCUR_MINOR, 8)

/* GCB Regions */
#define GCMP_GCB_CMxBASE_OFS(n)		(0x0090+16*(n))		/* Global Region[0-3] Base Address */
#define	 GCMP_GCB_CMxBASE_BASE_SHF	16
#define	 GCMP_GCB_CMxBASE_BASE_MSK	GCMPGCBMSK(CMxBASE_BASE, 16)
#define GCMP_GCB_CMxMASK_OFS(n)		(0x0098+16*(n))		/* Global Region[0-3] Address Mask */
#define	 GCMP_GCB_CMxMASK_MASK_SHF	16
#define	 GCMP_GCB_CMxMASK_MASK_MSK	GCMPGCBMSK(CMxMASK_MASK, 16)
#define	 GCMP_GCB_CMxMASK_CMREGTGT_SHF	0
#define	 GCMP_GCB_CMxMASK_CMREGTGT_MSK	GCMPGCBMSK(CMxMASK_CMREGTGT, 2)
#define	 GCMP_GCB_CMxMASK_CMREGTGT_MEM	 0
#define	 GCMP_GCB_CMxMASK_CMREGTGT_MEM1	 1
#define	 GCMP_GCB_CMxMASK_CMREGTGT_IOCU1 2
#define	 GCMP_GCB_CMxMASK_CMREGTGT_IOCU2 3


/* Core local/Core other control block registers */
#define GCMP_CCB_RESETR_OFS		0x0000			/* Reset Release */
#define	 GCMP_CCB_RESETR_INRESET_SHF	0
#define	 GCMP_CCB_RESETR_INRESET_MSK	GCMPCCBMSK(RESETR_INRESET, 16)
#define GCMP_CCB_COHCTL_OFS		0x0008			/* Coherence Control */
#define	 GCMP_CCB_COHCTL_DOMAIN_SHF	0
#define	 GCMP_CCB_COHCTL_DOMAIN_MSK	GCMPCCBMSK(COHCTL_DOMAIN, 8)
#define GCMP_CCB_CFG_OFS		0x0010			/* Config */
#define	 GCMP_CCB_CFG_IOCUTYPE_SHF	10
#define	 GCMP_CCB_CFG_IOCUTYPE_MSK	GCMPCCBMSK(CFG_IOCUTYPE, 2)
#define	  GCMP_CCB_CFG_IOCUTYPE_CPU	0
#define	  GCMP_CCB_CFG_IOCUTYPE_NCIOCU	1
#define	  GCMP_CCB_CFG_IOCUTYPE_CIOCU	2
#define	 GCMP_CCB_CFG_NUMVPE_SHF	0
#define	 GCMP_CCB_CFG_NUMVPE_MSK	GCMPCCBMSK(CFG_NUMVPE, 10)
#define GCMP_CCB_OTHER_OFS		0x0018		/* Other Address */
#define	 GCMP_CCB_OTHER_CORENUM_SHF	16
#define	 GCMP_CCB_OTHER_CORENUM_MSK	GCMPCCBMSK(OTHER_CORENUM, 16)
#define GCMP_CCB_RESETBASE_OFS		0x0020		/* Reset Exception Base */
#define	 GCMP_CCB_RESETBASE_BEV_SHF	12
#define	 GCMP_CCB_RESETBASE_BEV_MSK	GCMPCCBMSK(RESETBASE_BEV, 20)
#define GCMP_CCB_ID_OFS			0x0028		/* Identification */
#define GCMP_CCB_DINTGROUP_OFS		0x0030		/* DINT Group Participate */
#define GCMP_CCB_DBGGROUP_OFS		0x0100		/* DebugBreak Group */

/*
 * CPC register definitions
 */

#define CPCBASE				0x9bde0000

/* Offsets to major blocks within CPC from CPC base */
#define CPC_GCB_OFS			0x0000 /* Global Control Block */
#define CPC_CLCB_OFS			0x2000 /* Core Local Control Block */
#define CPC_COCB_OFS			0x4000 /* Core Other Control Block */

/* Offsets to individual CPC registers from CPC base */
#define CPCOFS(block,tag,reg) \
	(CPC_##block##_OFS + CPC_##tag##_##reg##_OFS)

#define CPCGCBOFS(reg)			CPCOFS(GCB,GCB,reg)
#define CPCCLCBOFS(reg)			CPCOFS(CLCB,CCB,reg)
#define CPCCOCBOFS(reg)			CPCOFS(COCB,CCB,reg)

/* CPC register access */
#define CPCGCB(reg)			REGP(CPCBASE,CPCGCBOFS(reg))
#define CPCCLCB(reg)			REGP(CPCBASE,CPCCLCBOFS(reg))
#define CPCCOCB(reg)			REGP(CPCBASE,CPCCOCBOFS(reg))

/* GCB registers */
#define CPC_GCB_ACCESS_OFS		0x0000	/* CPC Global Access Privilege */
#define CPC_GCB_SEQDEL_OFS		0x0008	/* CPC Global Sequence Delay Counter */
#define CPC_GCB_RAIL_OFS		0x0010	/* CPC Global Rail Delay Counter */
#define CPC_GCB_RESETLEN_OFS		0x0018	/* CPC Global Reset Width Counter */

/* CPC Core local/Core other control block registers */
#define CPC_CCB_CMD_OFS			0x0000	/* CPC CCB Command */
#define CPC_CCB_STAT_OFS		0x0008	/* CPC CCB Status */
#define CPC_CCB_OTHER_OFS		0x0010	/* CPC CCB Other */

#ifndef __ASSEMBLY__
extern int __init gcmp_probe(unsigned long, unsigned long);
extern int __init gcmp_niocu(void);
extern void __init gcmp_setregion(int, unsigned long, unsigned long, int);
#endif
#endif /* _ASM_GCMPREGS_H */
