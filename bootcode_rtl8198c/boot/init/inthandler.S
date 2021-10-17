
#include <asm/asm.h>
#include <asm/mipsregs.h>
#include <asm/regdef.h>
#include <asm/stackframe.h>
#include <asm/rtl8198.h>
#include <asm/rtl8198c.h>


//-------------------------------------------------------------------------
	/* General exception vector. */
		.set	at
		.set	reorder	   
	
	//copy this body to 0x80000180
		NESTED(exception_matrix, 0, sp)
#if 1

		mfc0	k1, CP0_CAUSE	
		nop
		nop
		la	k0, (exception_handlers)
		nop
		nop		
		andi	k1, k1, 0x7c
		addu	k0, k0, k1
		lw	k0, (k0)
		jr	k0
		nop
		nop
#endif

#if 0	
		li k0,0xb8003114
		li k1,(1<<29)|(1<<31)
		sw k1,0(k0)

		li k0,0xb8000000
		lw k1,0(k0)
		nop
		add k1,1
		sw k1,0(k0)
/*
		mfc0 k0, CP0_EPC
		nop
		j k0
		rfe
*/
		
		.word 0x42000018   //eret
		nop
		nop
		nop
#endif
		END(exception_matrix)


//-------------------------------------------------------------------------


#This INT dispatch routine is triggered via Exception->CAUSE:EXC-CODE:INT
	.text
	.set	noreorder
	.set	noat
	.align	5
	NESTED(IRQ_finder, PT_SIZE, sp)
	SAVE_ALL
	CLI
	.set	at
	
		mfc0    t0, CP0_CAUSE       #get pending interrupts
        nop
        nop
        
        mfc0    t2, CP0_STATUS
        nop
        nop
        
        and     t0, t2              #isolate allowed ones
        nop
        and    t2, t0, 0xfc00      #CT: just look at the Hardware INT bit 10-15
        nop
        
        bne     t2, zero, handle_it #it shows there are pending interrupt, jump to Handler.
        nop
        nop
1:        
        li      t0,( UART_THR + 0xb8000000)
        li      t1,'m'		    #it shows m, that is not reasonable to come here.
        nop
        sb      t1, 0(t0)
     	nop
     	nop
	 		            #just hang here.
      	b	1b 
        nop
        nop

handle_it:  
//see GIC
        la      t0, (GIC_BASE_ADDR+0x480)
        la      t1, (GIC_BASE_ADDR+0x400)
	
        lw      a0, 0(t0)  	    
        lw      a1, 0(t1)	    
        nop
        and     a0, a1		   
        

        la      t0, (GIC_BASE_ADDR+0x484)
        la      t1, (GIC_BASE_ADDR+0x404)	
        lw      a2, 0(t0)  	    
        lw      a1, 0(t1)	    
        nop        
        and     a1, a2		    
        
#if 0	//irq happend, will display
	//---------------------------------------------------------- 
        li      t0,( UART_THR + 0xb8000000)
        li      t1,'i'		   
        nop
        sb      t1, 0(t0)
	//----------------------------------------------------------
#endif
        
	jal irq_dispatch        #this will lead to real ISR 
	nop
	nop

#if 0	
	//---------------------------------------------------------- 
        li      t0,( UART_THR + 0xb8000000)
        li      t1,'o'		   
        nop
        sb      t1, 0(t0)
	//----------------------------------------------------------
#endif
        move   a1,sp
	RESTORE_ALL_AND_RET
	nop
	nop
	END(IRQ_finder)

	.set    at



