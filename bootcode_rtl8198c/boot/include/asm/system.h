/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 - 1999 by Ralf Baechle
 * Copyright (C) 1996 by Paul M. Antoine
 * Copyright (C) 1994 - 1999 by Ralf Baechle
 */
#ifndef _ASM_SYSTEM_H
#define _ASM_SYSTEM_H


extern __inline__ void
__sti(void)
{
	__asm__ __volatile__(
		".set\tpush\n\t"
		".set\treorder\n\t"
		".set\tnoat\n\t"
		"mfc0\t$1,$12\n\t"
		"ori\t$1,0x1\n\t"	
		"mtc0\t$1,$12\n\t"
		"sll	$0, 3\n\t"
		".set\tpop\n\t"
		: /* no outputs */
		: /* no inputs */
		: "$1", "memory");
}




/*
 * For cli() we have to insert nops to make shure that the new value
 * has actually arrived in the status register before the end of this
 * macro.
 * R4000/R4400 need three nops, the R4600 two nops and the R10000 needs
 * no nops at all.
 */
extern __inline__ void
__cli(void)
{
	__asm__ __volatile__(
		".set    push\n    "
		".set    reorder\n    "
		".set    noat\n    "
		"mfc0    $1,$12\n    "
		"ori    $1,1\n    "
		"xori    $1,1\n    "
		".set    noreorder\n    "
		"mtc0    $1,$12\n    "
		"sll	$0, 3\n    "		
		"nop\n    "
		"nop\n    "
		"nop\n    "
		".set    pop\n    "
		: /* no outputs */
		: /* no inputs */
		: "$1", "memory");
}

#define __save_flags(x)							\
__asm__ __volatile__(							\
	".set    push\n    "						\
	".set    reorder\n    "						\
	"mfc0    %0,$12\n    "						\
	".set    pop\n    "							\
	: "=r" (x))

#define __save_and_cli(x)						\
__asm__ __volatile__(							\
	".set    push\n    "						\
	".set    reorder\n    "						\
	".set    noat\n    "						\
	"mfc0    %0,$12\n    "						\
	"ori    $1,%0,1\n    "						\
	"xori    $1,1\n    "						\
	".set    noreorder\n    "						\
	"mtc0    $1,$12\n    "						\
	"sll	$0, 3\n\t"				\		
	"nop\n    "							\
	"nop\n    "							\
	"nop\n    "							\
	".set    pop\n    "							\
	: "=r" (x)							\
	: /* no inputs */						\
	: "$1", "memory")

#define __restore_flags(flags)						\
do {									\
	unsigned long __tmp1;						\
									\
	__asm__ __volatile__(						\
		".set    noreorder            # __restore_flags\n    "		\
		".set    noat\n    "					\
		"mfc0    $1, $12\n    "					\
		"andi    %0, 1\n    "					\
		"ori    $1, 1\n    "					\
		"xori    $1, 1\n    "					\
		"or    %0, $1\n    "					\
		"mtc0    %0, $12\n    "					\
		"sll	$0, 3\n\t"				\		
		"nop\n    "						\
		"nop\n    "						\
		"nop\n    "						\
		".set    at\n    "						\
		".set    reorder"						\
		: "=r" (__tmp1)						\
		: "0" (flags)						\
		: "$1", "memory");					\
} while(0)

/*
 * Non-SMP versions ...
 */
#define sti() __sti()
#define cli() __cli()
#define save_flags(x) __save_flags(x)
#define save_and_cli(x) __save_and_cli(x)
#define restore_flags(x) __restore_flags(x)


#endif /* _ASM_SYSTEM_H */
