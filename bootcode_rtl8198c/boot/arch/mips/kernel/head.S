/* $Id: head.S,v 1.1 2009/11/13 13:22:46 jasonwang Exp $
 *
 * arch/mips/kernel/head.S
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994, 1995 Waldorf Electronics
 * Written by Ralf Baechle and Andreas Busse
 * Copyright (C) 1995 - 1999 Ralf Baechle
 * Copyright (C) 1996 Paul M. Antoine
 * Modified for DECStation and hence R3000 support by Paul M. Antoine
 * Further modifications by David S. Miller and Harald Koerfgen
 * Copyright (C) 1999 Silicon Graphics, Inc.
 *
 * Head.S contains the MIPS exception handler and startup code.
 */

#include <linux/config.h>
/*Cyrus Tsai*/
#include <linux/init.h>
/*Cyrus Tsai*/

#include <linux/threads.h>

#include <asm/asm.h>
#include <asm/cacheops.h>
#include <asm/current.h>
#include <asm/offset.h>
#include <asm/processor.h>
#include <asm/regdef.h>
#include <asm/cachectl.h>
#include <asm/mipsregs.h>
#include <asm/lexraregs.h>
#include <asm/stackframe.h>
#include <asm/bootinfo.h>
#include <asm/cpu.h>
#ifdef CONFIG_PM_SUSPEND_WAKEUP
#include <asm/power.h>
#endif


#define REG32_W(addr,v) 		li t6,v;\
					li t7,addr;\
					sw t6, 0(t7);\
					nop;


__INIT

//-------------------------------------------------------------------------
	.text
	NESTED(start, 16, sp)
	.set	noreorder
	/* The following two symbols are used for kernel profiling. */
	EXPORT(stext)
	EXPORT(_stext)



	
	
	/*
	 * Reserved space for exception handlers.
	 * Necessary for machines which link their kernels at KSEG0.
	 * FIXME: Use the initcode feature to get rid of unused handler
	 * variants.
	 */

#if 1
	b check_self_exist
	nop
	nop
	.fill	0x400
check_self_exist:	
#else

#define RVECENT(f,n) \
   b f; nop
#define XVECENT(f,bev) \
   b f     ;           \
   li k0,bev


	RVECENT(reset,0)	/* U-boot entry point */
	RVECENT(reset,1)	/* software reboot */
	
	#if 0
	.word 0x20110816		//ROM code version tag, 
	.word 0x00000000
	#else
	RVECENT(romReserved,2)
	#endif
	RVECENT(romReserved,3)
	RVECENT(romReserved,4)
	RVECENT(romReserved,5)
	RVECENT(romReserved,6)
	RVECENT(romReserved,7)
	RVECENT(romReserved,8)
	RVECENT(romReserved,9)
	RVECENT(romReserved,10)
	RVECENT(romReserved,11)
	RVECENT(romReserved,12)
	RVECENT(romReserved,13)
	RVECENT(romReserved,14)
	RVECENT(romReserved,15)
	RVECENT(ramExcHandle,0x80)   /* 0x8000-0080  ram exception */
	RVECENT(romReserved,17)
	RVECENT(romReserved,18)
	RVECENT(romReserved,19)
	RVECENT(romReserved,20)
	RVECENT(romReserved,21)
	RVECENT(romReserved,22)
	RVECENT(romReserved,23)
	RVECENT(romReserved,24)
	RVECENT(romReserved,25)
	RVECENT(romReserved,26)
	RVECENT(romReserved,27)
	RVECENT(romReserved,28)
	RVECENT(romReserved,29)
	RVECENT(romReserved,30)
	RVECENT(romReserved,31)
	RVECENT(romReserved,32)
	RVECENT(romReserved,33)
	RVECENT(romReserved,34)
	RVECENT(romReserved,35)
	RVECENT(romReserved,36)
	RVECENT(romReserved,37)
	RVECENT(romReserved,38)
	RVECENT(romReserved,39)
	RVECENT(romReserved,40)
	RVECENT(romReserved,41)
	RVECENT(romReserved,42)
	RVECENT(romReserved,43)
	RVECENT(romReserved,44)
	RVECENT(romReserved,45)
	RVECENT(romReserved,46)
	RVECENT(romReserved,47)
	RVECENT(romReserved,48)
	RVECENT(romReserved,49)
	RVECENT(romReserved,50)
	RVECENT(romReserved,51)
	RVECENT(romReserved,52)
	RVECENT(romReserved,53)
	RVECENT(romReserved,54)
	RVECENT(romReserved,55)
	RVECENT(romReserved,56)
	RVECENT(romReserved,57)
	RVECENT(romReserved,58)
	RVECENT(romReserved,59)
	RVECENT(romReserved,60)
	RVECENT(romReserved,61)
	RVECENT(romReserved,62)
	RVECENT(romReserved,63)
	XVECENT(romExcHandle,0x200)	/* bfc00200: R4000 tlbmiss vector */
	RVECENT(romReserved,65)
	RVECENT(romReserved,66)
	RVECENT(romReserved,67)
	RVECENT(romReserved,68)
	RVECENT(romReserved,69)
	RVECENT(romReserved,70)
	RVECENT(romReserved,71)
	RVECENT(romReserved,72)
	RVECENT(romReserved,73)
	RVECENT(romReserved,74)
	RVECENT(romReserved,75)
	RVECENT(romReserved,76)
	RVECENT(romReserved,77)
	RVECENT(romReserved,78)
	RVECENT(romReserved,79)
	XVECENT(romExcHandle,0x280)	/* bfc00280: R4000 xtlbmiss vector */
	RVECENT(romReserved,81)
	RVECENT(romReserved,82)
	RVECENT(romReserved,83)
	RVECENT(romReserved,84)
	RVECENT(romReserved,85)
	RVECENT(romReserved,86)
	RVECENT(romReserved,87)
	RVECENT(romReserved,88)
	RVECENT(romReserved,89)
	RVECENT(romReserved,90)
	RVECENT(romReserved,91)
	RVECENT(romReserved,92)
	RVECENT(romReserved,93)
	RVECENT(romReserved,94)
	RVECENT(romReserved,95)
	XVECENT(romExcHandle,0x300)	/* bfc00300: R4000 cache vector */
	RVECENT(romReserved,97)
	RVECENT(romReserved,98)
	RVECENT(romReserved,99)
	RVECENT(romReserved,100)
	RVECENT(romReserved,101)
	RVECENT(romReserved,102)
	RVECENT(romReserved,103)
	RVECENT(romReserved,104)
	RVECENT(romReserved,105)
	RVECENT(romReserved,106)
	RVECENT(romReserved,107)
	RVECENT(romReserved,108)
	RVECENT(romReserved,109)
	RVECENT(romReserved,110)
	RVECENT(romReserved,111)
	XVECENT(romExcHandle,0x380)	/* bfc00380: R4000 general vector */
	RVECENT(romReserved,113)
	RVECENT(romReserved,114)
	RVECENT(romReserved,115)
	RVECENT(romReserved,116)
	RVECENT(romReserved,116)
	RVECENT(romReserved,118)
	RVECENT(romReserved,119)
	RVECENT(romReserved,120)
	RVECENT(romReserved,121)
	RVECENT(romReserved,122)
	RVECENT(romReserved,123)
	RVECENT(romReserved,124)
	RVECENT(romReserved,125)
	RVECENT(romReserved,126)
	RVECENT(romReserved,127)

#endif


reset:

	/* Clear watch registers */
	mtc0	zero, CP0_WATCHLO
	mtc0	zero, CP0_WATCHHI

	/* WP(Watch Pending), SW0/1 should be cleared */
	mtc0	zero, CP0_CAUSE
	

	mfc0	t0, CP0_STATUS
	or	t0, ST0_CU0 | 0x1f  
	xor	t0, 0x1f 
	mtc0	t0, CP0_STATUS
	.set	noreorder
	sll	zero, 3				# ehb


	/* Init Timer */
	mtc0	zero, CP0_COUNT
	mtc0	zero, CP0_COMPARE



#if 1 //wei add for 1074k core 1
        mfc0    v1, CP0_PRID, 1
        andi    v1, 0x3ff
        beqz    v1, 1f   //ID=0 go out
 //       bnez    v1, 1f        //ID!=0 go out
        nop


		REG32_W(0xb8002000, '2'<<24);	  //Secondary CPU			
		REG32_W(0xb8002000, '2'<<24);	  //Secondary CPU			
		REG32_W(0xb8002000, '2'<<24);	  //Secondary CPU			
loop:
		
        nop
        nop
        b loop
        nop
1:
#endif

	//-----------------------------------------------
#if 0
	#define VIR2PHY(x) (x&0x1fffffff)
	#define SRAM_BASE (0x80000000+(07<<20))  //7M
	#define SRAM_TOP (SRAM_BASE+0x1000)  //4K
	#define REG32_W(addr,v) or t6,zero,v;\
						or t7,zero,addr;\
						sw t6, 0(t7);\
						nop;


	
		//enable unmap
		REG32_W(0xb8001304,0x5);   //unmap seg len=4K
		REG32_W(0xb8001300,VIR2PHY(SRAM_BASE) |1);   //unmap seg addr

		//enable sram
		REG32_W(0xb8004004,0x05);   //sram seg len=4K		
		REG32_W(0xb8004000,VIR2PHY(SRAM_BASE) |1);   //sram seg addr


		li sp,SRAM_TOP-0x4
		nop		
#else
#ifdef CONFIG_NAND_FLASH_BOOTING
		//li sp,0x80700000
		//li sp,0x80500000
		li sp,0xa0500000
		nop
#else
		li sp,0x80700000
		nop
#endif

#endif
	
		
	//-----------------------------------------clear bss
#if 0	
	/* The firmware/bootloader passes argc/argp/envp                   
	 * to us as arguments.  But clear bss first because                
	 * the romvec and other important info is stored there             
	 * by prom_init().                                                 
	 */                                                                
	la	t0, _edata  	//la	t0, _fbss                                                   
	sw	zero, (t0)                                                     
	la	t1, (_end - 4)                                                 
1:                                                                         
	addiu	t0, 4                                                      
	bne	t0, t1, 1b                                                     
	sw	zero, (t0)                                                     
	
#endif     

#if 0
	/* jwsyu init bss section start @20140113 */
	la	t0, __bss_start
	la	t1, (_end -1)
	la	t3, _end
	beq	t0, t3, 2f //check __bss_start == _end, skip init bss section
	move	t2, t0
1:
	sb	zero, 0(t2)
	addi	t2, 1
	bne	t2, t1, 1b
	nop

2:
	/* jwsyu init bss section end @20140113 */
#endif

	//--------------------------------------------
#if 0 //def CONFIG_NAND_FLASH_BOOTING
	//jal flush_cache_r
	nop
#endif
	nop
	nop

	jal	init_arch  	                                          
	nop  
	nop
	
//==========================================================
#if 1
romReserved:
        b       romReserved
        nop

romExcHandle:
        b       romExcHandle
		nop

ramExcHandle:
		b	ramExcHandle
		nop
#endif		

#define REG32_R(addr,v)  	li t7,addr;\
						lw v, 0(t7);\
						nop;

//==============================================================================
#if 1
flush_cache_r:
	#define START_ADDR 0x80000000
	#define _CACHELINE_SIZE 32
	#define _ICACHE_SIZE		(64 * 1024)		
	#define _DCACHE_SIZE		(32 * 1024)	
	#define _SCACHE_SIZE		(512 * 1024)		

	
#define CACHE_OP( code, type )			( ((code) << 2) | (type) )

#define ICACHE_INDEX_INVALIDATE			CACHE_OP(0x0, 0)
#define ICACHE_INDEX_LOAD_TAG			CACHE_OP(0x1, 0)
#define ICACHE_INDEX_STORE_TAG			CACHE_OP(0x2, 0)
#define DCACHE_INDEX_WRITEBACK_INVALIDATE	CACHE_OP(0x0, 1)
#define DCACHE_INDEX_LOAD_TAG			CACHE_OP(0x1, 1)
#define DCACHE_INDEX_STORE_TAG			CACHE_OP(0x2, 1)
#define SCACHE_INDEX_STORE_TAG			CACHE_OP(0x2, 3)

#define ICACHE_ADDR_HIT_INVALIDATE		CACHE_OP(0x4, 0)
#define ICACHE_ADDR_FILL			CACHE_OP(0x5, 0)
#define ICACHE_ADDR_FETCH_LOCK			CACHE_OP(0x7, 0)
#define DCACHE_ADDR_HIT_INVALIDATE		CACHE_OP(0x4, 1)
#define DCACHE_ADDR_HIT_WRITEBACK_INVALIDATE	CACHE_OP(0x5, 1)
#define DCACHE_ADDR_HIT_WRITEBACK		CACHE_OP(0x6, 1)
#define DCACHE_ADDR_FETCH_LOCK			CACHE_OP(0x7, 1)

#define SCACHE_ADDR_HIT_WRITEBACK_INVALIDATE	CACHE_OP(0x5, 3)


	#define MTC0_SEL_OPCODE(src, dst, sel)\
	  	.##word (0x40800000 | ((src)<<16) | ((dst)<<11) | (sel));\
		sll zero,zero,1;\
		sll zero,zero,1;\
		sll zero,zero,1;\
		sll zero,zero,1
		
	//----------------------------------------
//init_L1_dcahe:

	MTC0_SEL_OPCODE( 0, 29, 2 )   //R_C0_DTagHi
	MTC0_SEL_OPCODE( 0, 28, 2 )

0:	
	li	a2, START_ADDR
	li  a1,_CACHELINE_SIZE
	li  a3,START_ADDR+_DCACHE_SIZE-_CACHELINE_SIZE


	/* Loop through all lines, invalidating each of them */
1:	

	cache	DCACHE_INDEX_STORE_TAG, 0(a2)	/* clear tag */


	bne	a2, a3, 1b
	addu	a2, a1


	//--------------------------------------------
//init_L1_icahe:	
	MTC0_SEL_OPCODE( 0, 29, 0 )   //R_C0_ITagHi
	MTC0_SEL_OPCODE( 0, 28, 0 )

0:	
	li	a2, START_ADDR
	li  a1,_CACHELINE_SIZE
	li  a3,START_ADDR+_ICACHE_SIZE-_CACHELINE_SIZE


	/* Loop through all lines, invalidating each of them */
1:	

	cache	ICACHE_INDEX_STORE_TAG, 0(a2)	/* clear tag */


	bne	a2, a3, 1b
	addu	a2, a1
	//--------------------------------------------
init_L2_cache:

#ifdef CONFIG_SPI_FLASH
#define __disable_L2_cache    0
#endif

#ifdef CONFIG_NAND_FLASH_BOOTING
#define __disable_L2_cache    0
#endif

#if __disable_L2_cache
	//disable L2 cache
	li t0,(1<<12)
	MTC0_SEL_OPCODE( 8,16,  2);
#else	
#define RTL98_V0 0x8198C000
	nop
        REG32_R(0xb8000000,t8);
        nop
	nop
//	IF_EQ(t8, RTL98_V0, lab_disSC); //jason
        nop
	nop
	li t0,(0<<12)
	MTC0_SEL_OPCODE( 0, 28, 4 )  //R_C0_SelL23TagLo
	nop
	nop
0:
	li	a2, START_ADDR
	li  a1,_CACHELINE_SIZE
	li  a3,START_ADDR+_SCACHE_SIZE-_CACHELINE_SIZE
	

	/* Loop through all lines, invalidating each of them */
1:	

	cache	SCACHE_INDEX_STORE_TAG, 0(a2)	/* clear tag */

	bne	a2, a3, 1b
	addu	a2, a1
	nop
	nop
	j SC_EXIT
lab_disSC:
	//disable L2 cache
	li t0,(1<<12)
	MTC0_SEL_OPCODE( 8,16,  2);
	nop
	nop
	
#endif
SC_EXIT:
	nop
	
	j ra
	nop
    nop


#endif

//==============================================================================





	END(start)

//-------------------------------------------------------------------------
//==============================================================================




__FINIT
