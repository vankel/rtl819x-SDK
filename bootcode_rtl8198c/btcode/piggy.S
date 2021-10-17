        
#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>        

        .text
        .set noreorder
		.globl __start              
__start:

#if 0
		j 1f
		nop

//offset 0x08		
regdata:  				
		.word 0xb8001008, 0xffff05c0
//0x10		
		.word 0xb8001004, 0x54480000		
		.word 0xb8001050, 0xD2800000
//0x20		
		.word 0xb8000010, 0x017FFD2E		
		.word 0xb8000010, 0x017FFD2E
//0x30		
		.word 0xb8000010, 0x017FFD2E		
		.word 0xb8000010, 0x017FFD2E
//0x40		
		.word 0xb8000010, 0x017FFD2E
		.word 0xb8000010, 0x017FFD2E				
//offset 0x50


1:
#endif

		//setup stack pointer
		la	t0, __bss_end
		nop
		//addi 	t0, t0, 4096
		addi 	t0, t0, 8192//For NAND Flash booting necessary
		nop
		move 	sp, t0
		nop
test:
		jal	boot_entry
		nop

