#include <asm/asm.h>
#include <asm/regdef.h>
	
	.extern __nfjrom_start
	.extern __nfjrom_end 
	.text
	.global NO_FLASH_BOOT
NO_FLASH_BOOT:
	        la      k0, __nfjrom_start
                la              k1, (__nfjrom_end + 4)
                la              t1, 0x81000000
1:
                lw              t0, 0(k0)
                nop
 
                sw              t0, 0(t1)       //*(t1)=*(t0)   t1+=4;  t0+=4;   t1=BOOT_ADDR, t0=__boot_start, run loop
                nop
 
                addu    t1, 4
 
                addu    k0, 4
 
                bne             k1, k0, 1b
                nop
 
//--------------------------------------------------------------------------------
//              Jump to booting
                j              0x81000000
                nop
