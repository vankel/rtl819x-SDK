/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 - 1999 by Ralf Baechle
 * Modified for R3000 by Paul M. Antoine, 1995, 1996
 * Complete output from die() by Ulf Carlsson, 1998
 * Copyright (C) 1999 Silicon Graphics, Inc.
 */
#include <linux/kconfig.h>

#include <linux/linkage.h>
#include <asm/ptrace.h>
//#include <asm/mipsregs.h>

#ifdef CONFIG_RTL_8198C 
#include <asm/mipsregs.h>
#else
#include <asm/rlxregs.h>
#endif

#include <asm/addrspace.h>
//#include <asm/system.h>

#if defined(CONFIG_RTL_819X) && !defined(CONFIG_RTL_8198C)
/* For Realtek RTL865XC Network platform series */
#define _ICACHE_SIZE		(16 * 1024)		/* 16K bytes */
#define _DCACHE_SIZE		(8 * 1024)		/* 8K bytes */
#define _CACHE_LINE_SIZE	4			/* 4 words */
#endif

#if 1//defined(CONFIG_RTL_8198C)
#define _ICACHE_SIZE		(64 * 1024)		/* 64K bytes */
#define _DCACHE_SIZE		(32 * 1024)		/* 32K bytes */
#define _CACHE_LINE_SIZE	32
#define Hit_Invalidate_I	0x10
#define Hit_Writeback_Inv_D	0x15

#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	push					\n"	\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	pop					\n"	\
	:								\
	: "i" (op), "R" (*(unsigned char *)(addr)))
#endif

#define PROM_DEBUG

#ifdef PROM_DEBUG
extern int prom_printf(char *, ...);
#endif

/*Cyrus Tsai*/
void flush_cache(void);
void flush_icache(unsigned int start, unsigned int end);
void flush_dcache(unsigned int start, unsigned int end);
/*Cyrus Tsai*/

#if 1//defined(CONFIG_RTL_8198C)
void flush_cache_range(ulong start_addr, ulong size)
{
	unsigned long lsize = _CACHE_LINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (start_addr + size - 1) & ~(lsize - 1);

	/* aend will be miscalculated when size is zero, so we return here */
	if (size == 0)
		return;

	while (1) 
	{
		cache_op(Hit_Writeback_Inv_D, addr);
		cache_op(Hit_Invalidate_I, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
}

#endif

void flush_cache(void)
{
#ifdef 	CONFIG_RTL_EB8186
	flush_dcache(0,0);		
	flush_icache(0,0);	
#endif	
#if defined(CONFIG_RTL_819X) && !defined(CONFIG_RTL_8198C)
	flush_dcache(KSEG0, KSEG0+_DCACHE_SIZE);		
	flush_icache(KSEG0, KSEG0+_ICACHE_SIZE);	
#endif

#if defined(CONFIG_RTL_8198C)
	#define START_ADDR 0x80000000
	flush_cache_range(START_ADDR, _DCACHE_SIZE);
#endif
}

#define __sync()				\
	__asm__ __volatile__(			\
		".set	push\n\t"		\
		".set	noreorder\n\t"		\
		".set	mips2\n\t"		\
		"sync\n\t"			\
		".set	pop"			\
		: /* no output */		\
		: /* no input */		\
		: "memory")

#define Hit_Writeback_Inv_SD	0x17
#define CONFIG_SYS_CACHELINE_SIZE 32
void flush_scache_range(ulong start_addr, ulong size)
{
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = start_addr & ~(lsize - 1);
	unsigned long aend = (start_addr + size - 1) & ~(lsize - 1);

	/* aend will be miscalculated when size is zero, so we return here */
	if (size == 0)
		return;

	while (1) 
	{
		cache_op(Hit_Writeback_Inv_SD, addr);
		if (addr == aend)
			break;
		addr += lsize;
	}
	__sync();
}

void flush_cache_really(void)   
{
    /*this is the right way to flush_cache*/
	#define START_ADDR 0x80000000
	flush_cache_range(START_ADDR, 0x2000000); //flush 32M,bootcode may be used
	flush_scache_range(START_ADDR, 0x2000000);
}


/*Cyrus Tsai*/
void flush_icache(unsigned int start, unsigned int end)
{
#if defined(CONFIG_RTL_819X)
	/*
		Flush data cache at first in write-back platform.

		Ghhuang (2007/3/9):

		RD-Center suggest that we need to flush D-cache entries which
		might match to same address as I-cache ... when we flush
		I-cache.
		( Maybe some data is treated as data/instruction, both. )
	*/
	flush_dcache(start, end);

	/*Invalidate I-Cache*/
	__asm__ volatile(
		"mtc0 $0,$20\n\t"
		"nop\n\t"
		"li $8,2\n\t"
		"mtc0 $8,$20\n\t"
		"nop\n\t"
		"nop\n\t"
		"mtc0 $0,$20\n\t"
		"nop"
		: /* no output */
		: /* no input */
			);

#endif

#ifdef CONFIG_RTL_EB8186
   unsigned long flags;
   volatile unsigned int reg;
    save_flags(flags);cli();
   reg=read_32bit_cp0_register(CP0_XCONTEXT);
   __asm__ volatile("nop");   
    __asm__ volatile("nop");
   write_32bit_cp0_register(CP0_XCONTEXT, (reg &(~0x2))); //write '0' to bit 0,1
   __asm__ volatile("nop");
   __asm__ volatile("nop");    
   write_32bit_cp0_register(CP0_XCONTEXT, (reg | 0x2)); //wirte '1' to bit 0, 1
   __asm__ volatile("nop");  
    __asm__ volatile("nop");
    restore_flags(flags);
#endif
}

void flush_dcache(unsigned int start, unsigned int end)
{
#ifdef CONFIG_RTL_EB8186
   unsigned long flags;
   volatile unsigned int reg;
    save_flags(flags);cli();
    reg=read_32bit_cp0_register(CP0_XCONTEXT);
    __asm__ volatile("nop");
    __asm__ volatile("nop");
    write_32bit_cp0_register(CP0_XCONTEXT, (reg & (~0x1))); //write '0' to bit 0,1
    __asm__ volatile("nop");
    __asm__ volatile("nop");
    write_32bit_cp0_register(CP0_XCONTEXT, (reg | 0x1)); //wirte '1' to bit 0, 1
    __asm__ volatile("nop");
    __asm__ volatile("nop");
    restore_flags(flags);
#endif    
    
#if defined(CONFIG_RTL_819X)
	/* Flush D-Cache using its range */
	unsigned char *p;
	unsigned int size;
	unsigned int flags;
	unsigned int i;
   
	size = end - start;
   
	/* correctness check : flush all if any parameter is illegal */
// david	
//	if (	(size >= dcache_size) ||
	if ((size >= _DCACHE_SIZE) ||	(KSEGX(start) != KSEG0)	)
	{
		/*
		 *	ghhguang
		 *		=> For Realtek Lextra CPU,
		 *		   the cache would NOT be flushed only if the Address to-be-flushed
		 *		   is the EXPLICIT address ( which is really stored in that cache line ).
		 *		   For the aliasd addresses, the cache entry would NOT be flushed even
		 *		   it matchs same cache-index.
		 *
		 *		   => This is different from traditional MIPS-based CPU's configuration.
		 *		      So if we want to flush ALL-cache entries, we would need to use "mtc0"
		 *		      instruction instead of simply modifying the "size" to "dcache_size"
		 *		      and "start" to "KSEG0".
		 *
		 */
		__asm__ volatile(
			"mtc0 $0,$20\n\t"
			"nop\n\t"
			"li $8,512\n\t"
			"mtc0 $8,$20\n\t"
			"nop\n\t"
			"nop\n\t"
			"mtc0 $0,$20\n\t"
			"nop"
			: /* no output */
			: /* no input */
				);
	} 
#if 1	
	else
	{
		/* Start to isolate cache space */
		p = (char *)start;
   
		flags = read_c0_status();
   
		/* isolate cache space */
		write_c0_status( (ST0_ISC | flags) &~ ST0_IEC );
   
		for (i = 0; i < size; i += 0x040)
		{
			asm ( 	
			#ifdef OPEN_RSDK_RTL865x
				".word 0xbc750000\n\t"
				".word 0xbc750010\n\t"
				".word 0xbc750020\n\t"
				".word 0xbc750030\n\t"
			#endif
				"cache 0x15, 0x000(%0)\n\t"
			 	"cache 0x15, 0x010(%0)\n\t"
			 	"cache 0x15, 0x020(%0)\n\t"
			 	"cache 0x15, 0x030(%0)\n\t"
				:		/* No output registers */
				:"r"(p)		/* input : 'p' as %0 */
				);
			p += 0x040;
		}
   
		write_c0_status(flags);
	}    
#endif
#endif    
   
}
/*Cyrus Tsai*/
