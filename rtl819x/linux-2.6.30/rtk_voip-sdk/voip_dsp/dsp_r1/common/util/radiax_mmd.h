#ifndef	_RADIAX_MMD_H_
#define _RADIAX_MMD_H_

#include "mips.h"
#include "radiax.h"
//#include "lexra_asm_tool.h"
//#define LANGUAGE_ASSEMBLY
//#include "regs.h"
//#undef LANGUAGE_ASSEMBLY

#define __asm__ asm
#define ASM __asm__ volatile

#define RADIAX_MMD_MT	0x1		/* MAC 32*32 truncate mode */
#define RADIAX_MMD_MS	0x2		/* MAC 32 bit saturate mode */
#define RADIAX_MMD_MF	0x4		/* MAC fractional mode */

#define radiax_MMD_get_inline() \
({ \
	long __result; \
	ASM ("mfru %0,$24 	\t\n" :"=d" (__result) : /* no inputs */); \
	__result; \
})

#if defined(__m5281)

#define radiax_MMD_set_inline(mmd) \
({ \
	ASM ("mtru %0,$24	\t\n"  : /* no outputs */ : "d" ((long)mmd)); \		
})

#define radiax_MMD_set_backup_inline(mmd) \
({ \
	long __result; \
	ASM ("mfru %0, $24	\t\n"	\
	     "mtru %1, $24	\t\n" : "=d" (__result) : "d" ((long)mmd)); \
	__result; \
})

#define radiax_MMD_MF_enable_inline() \
({ \
	long __result; \
	ASM (".set push"); \
	ASM (".set noat"); \
	ASM ("mfru $1,$24" : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("move %0, $1":"=d" (__result) : /* no inputs */:"$1"); \
	ASM ("ori  $1,$1,0x4"  : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("mtru $1,$24"  : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM (".set pop"); \
	__result; \
})

#define radiax_MMD_MF_disable_inline() \
({ \
	long __result; \
	ASM (".set push"); \
	ASM (".set noat"); \
	ASM ("mfru $1,$24" : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("move %0, $1":"=d" (__result) : /* no inputs */:"$1"); \
	ASM ("andi $1,$1,0x1b"  : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("mtru $1,$24"  : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM (".set pop"); \
	__result; \
})

#else

#define radiax_MMD_set_inline(mmd) \
({ \
	ASM ("mtru %0,$24	\t\n"   \
	     "nop		\t\n"  : /* no outputs */ : "d" ((long)mmd)); \
})

#define radiax_MMD_set_backup_inline(mmd) \
({ \
	long __result; \
	ASM ("mfru %0, $24	\t\n"	\
	     "mtru %1, $24	\t\n"	\
	     "nop		\t\n" : "=d" (__result) : "d" ((long)mmd));  \
	__result; \
})

#define radiax_MMD_MF_enable_inline() \
({ \
	long __result; \
	ASM (".set push"); \
	ASM (".set noat"); \
	ASM ("mfru $1,$24" : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("move %0, $1":"=d" (__result) : /* no inputs */:"$1"); \
	ASM ("ori  $1,$1,0x4"  : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("mtru $1,$24"  : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("nop"); \
	ASM (".set pop"); \
	__result; \
})

#define radiax_MMD_MF_disable_inline() \
({ \
	long __result; \
	ASM (".set push"); \
	ASM (".set noat"); \
	ASM ("mfru $1,$24" : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("move %0, $1":"=d" (__result) : /* no inputs */:"$1"); \
	ASM ("andi $1,$1,0x1b"  : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("mtru $1,$24"  : /* no outputs */ : /* no inputs */ :"$1"); \
	ASM ("nop"); \
	ASM (".set pop"); \
	__result; \
})

#endif

#define radiax_MMD_get		radiax_MMD_get_inline
#define radiax_MMD_set		radiax_MMD_set_inline
#define radiax_MMD_set_backup	radiax_MMD_set_backup_inline
#define radiax_MMD_MF_enable	radiax_MMD_MF_enable_inline
#define radiax_MMD_MF_disable	radiax_MMD_MF_disable_inline
#define radiax_MMD_restore	radiax_MMD_set_inline

#endif /* _RADIAX_MMD_H_ */

