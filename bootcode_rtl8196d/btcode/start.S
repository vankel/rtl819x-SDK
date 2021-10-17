#include <asm/asm.h>
#include <asm/regdef.h>
#include <asm/mipsregs.h>
#include "start.h"
#include "../autoconf.h"




		.text
		.set noreorder
		.globl __start
__start:
		j 	load_boot
		nop              
		nop
		nop

//--------------------------------------------------------------------------------              
 load_boot:   
 		move 	t0, zero
		mtc0 	t0, $12

		nop
//-------------------------------------------------------------------------------- 
#ifdef CONFIG_RTL8196E
	REG32_R(0xb8000000,t6);
	IF_NEQ(t6,0x8196e000, notdiv);
	REG32_ANDOR( 0xb8000008,0xffffffff,(1<<19));          //for 8196E patch
	
notdiv:	
#endif  
		nop
#ifdef CONFIG_RTL8196D
		REG32_ANDOR(0xb8000088, ~(3<<5)&~(0xF<<0),(1<<4));  //for internal SWR issue
		nop
		REG32_ANDOR(0xb8000088, ~(3<<7),0);
		nop
#endif
		nop




//============================================================================

#if 1
lab_uart_show:
		jal uart_show    //show boot msg
		nop	
#endif		

//--------------------------------------------------------------------------------

//--------------------------------------------------------------------------------
#if 0
lab_ddr_cali:

		jal DDR_Auto_Calibration
		nop
#endif
//============================================================================

#if defined(CONFIG_DDR2_SDRAM) || defined(CONFIG_DDR1_SDRAM)
	#if defined(CONFIG_D128_16)
		REG32_W(0xb8001050, 0x50800000); // DDCR
		nop
    
		#ifdef CONFIG_RTL8196E
                REG32_W(0xb8000010, 0x00000b08); // CLKMGR, the value is from start_c.c
		#else                
                REG32_W(0xb8000010, 0x00000308); // CLKMGR, the value is from start_c.c
		#endif 
		   
		#if defined(CONFIG_DDR2_SDRAM)
			REG32_W(0xb8001008, 0x91062920);//DDR2 128MB 193MHZ
		#else //DDR1
			REG32_W(0xb8001008, 0x90E5E920);//DDR1 128MB 193MHZ
		#endif		
		nop
		
		// =======================================================
		// set dtr by hw_strap ck_m2x_freq_sel
		#if defined(CONFIG_RTL8196D) && defined(CONFIG_DDR2_SDRAM)		
		// read hw_strap bit 12-10, t6 = 0xb8000008 & (7<<10)
 		li t7,0xb8000008;
		lw t6, 0(t7);
		and t6,t6,(7<<10);		
		nop;

		// ifeq 193M, do nothing, t6 = (1<<10) ?
		or t7,zero,(1<<10);
		beq t6,t7,ddr193_128;
		nop;
		
		// _else cases, set to 181M dtr parameter
		REG32_W(0xb8001008, 0x9105E920);//DDR2 128MB 181MHZ			
		nop;		
ddr193_128:		
		#endif
		// =======================================================
		
		REG32_W(0xb8001004, 0x54880000);		
		nop
		
		#if defined(CONFIG_DDR2_SDRAM)
			REG32_W(0xb800100c, 0x78000000);//Set EDTCR for 128 DDR2
		#endif
		nop
		
	#elif  defined(CONFIG_D64_16)
	        nop
                REG32_W(0xb8001040, 0x3FFFFF80);      //3.1 MPMR,set default=0x3FFFFF80 , minimun Power saving parameter   ^M
              	nop 
		REG32_W(0xb8001040, 0x7FFFFF80);      //3.2 MPMR,Enable Power Down mode          
		nop
	 	REG32_W(0xb8001050, 0x50800000); // DDCR
		nop
    
		#ifdef CONFIG_RTL8196E
                REG32_W(0xb8000010, 0x00000b08); // CLKMGR, the value is from start_c.c
		#else                
                REG32_W(0xb8000010, 0x00000308); // CLKMGR, the value is from start_c.c
		#endif
		    
		#if defined(CONFIG_DDR2_SDRAM)
			REG32_W(0xb8001008, 0x91052920);//DDR2 64MB 193MHZ
		#else //DDR1
			REG32_W(0xb8001008, 0x90E36920);//DDR1 64MB 193MHZ
		#endif
		nop
		
		// =======================================================
		// set dtr by hw_strap ck_m2x_freq_sel
		#ifdef CONFIG_RTL8196D
		// read hw_strap bit 12-10, t6 = 0xb8000008 & (7<<10)
 		li t7,0xb8000008;
		lw t6, 0(t7);
		and t6,t6,(7<<10);		
		nop;

		// ifeq 193M, do nothing, t6 = (1<<10) ?
		or t7,zero,(1<<10);
		beq t6,t7,ddr193_64;
		nop;
		
		// _else cases, set to 181M dtr parameter
		#if defined(CONFIG_DDR2_SDRAM)
			REG32_W(0xb8001008, 0x9104E920);//DDR2 64MB 181MHZ			
		#else //DDR1
			REG32_W(0xb8001008, 0x90e32920);//DDR1 64MB 181MHZ
		#endif
		nop;		
ddr193_64:		
		#endif
		// =======================================================
		
	 	REG32_W(0xb8001004, 0x54880000);
		nop
		nop
		REG32_ANDOR(0xb8000048, ~(3<<22),(1<<23)|(1<<24));
		nop
	#elif  defined(CONFIG_D32_16)
	        nop
                REG32_W(0xb8001040, 0x3FFFFF80);      //3.1 MPMR,set default=0x3FFFFF80 , minimun Power saving parameter   ^M
              	nop 
		REG32_W(0xb8001040, 0x7FFFFF80);      //3.2 MPMR,Enable Power Down mode          
		nop
		REG32_W(0xb8001050, 0x50800000); // DDCR
		nop
		#ifdef CONFIG_RTL8196E
                REG32_W(0xb8000010, 0x00000b08); // CLKMGR, the value is from start_c.c
		#else                
                REG32_W(0xb8000010, 0x00000308); // CLKMGR, the value is from start_c.c
		#endif                
               	
		#if defined(CONFIG_DDR2_SDRAM)
			REG32_W(0xb8001008, 0x9103A920);//DDR2 32MB 193MHZ
		#else //DDR1
			REG32_W(0xb8001008, 0x90E36920);//DDR1 32MB 193MHZ
		#endif

		// =======================================================
		// set dtr by hw_strap ck_m2x_freq_sel
		#ifdef CONFIG_RTL8196D
		// read hw_strap bit 12-10, t6 = 0xb8000008 & (7<<10)
 		li t7,0xb8000008;
		lw t6, 0(t7);
		and t6,t6,(7<<10);		
		nop;

		// ifeq 193M, do nothing, t6 = (1<<10) ?
		or t7,zero,(1<<10);
		beq t6,t7,ddr193_32;
		nop;
		
		// _else cases, set to 181M dtr parameter
		#if defined(CONFIG_DDR2_SDRAM)
			REG32_W(0xb8001008, 0x91036920);//DDR2 32MB 181MHZ			
		#else //DDR1
			REG32_W(0xb8001008, 0x90e32920);//DDR1 32MB 181MHZ
		#endif
		nop;		
ddr193_32:		
		#endif
		// =======================================================
		
                REG32_W(0xb8001004, 0x54480000);
                nop
                nop
		REG32_ANDOR(0xb8000048, ~(3<<22),(1<<23));

	#endif

#elif  defined(CONFIG_SDRAM)

	#if defined(CONFIG_D32_16)
	        nop
                REG32_W(0xb8001040, 0x3FFFFF80);      //3.1 MPMR,set default=0x3FFFFF80 , minimun Power saving parameter   ^M
              	nop 
		REG32_W(0xb8001040, 0x7FFFFF80);      //3.2 MPMR,Enable Power Down mode          
		nop
		
                REG32_W(0xb8000010, 0x00000b08); // CLKMGR, the value is from start_c.c
		nop	
                REG32_W(0xb8001008, 0x48C26110); //SDR 32MB 156MHZ
                nop
                REG32_W(0xb8001004, 0x54480000);
                nop	
	#elif  defined(CONFIG_D64_16)
	        nop
                REG32_W(0xb8001040, 0x3FFFFF80);      //3.1 MPMR,set default=0x3FFFFF80 , minimun Power saving parameter   ^M
              	nop 
		REG32_W(0xb8001040, 0x7FFFFF80);      //3.2 MPMR,Enable Power Down mode          
		nop
		REG32_W(0xb8001050, 0xE3100000); // DDCR
                nop
                REG32_W(0xb8000010, 0x00000b08); // CLKMGR, the value is from start_c.c
                nop
                REG32_W(0xb8001008, 0x48c26110);//SDR 16MB 156MHZ
                nop
                REG32_W(0xb8001004, 0x54880000);
                nop  
	#elif  defined(CONFIG_D16_16)
	        nop
                REG32_W(0xb8001040, 0x3FFFFF80);      //3.1 MPMR,set default=0x3FFFFF80 , minimun Power saving parameter   ^M
              	nop 
		REG32_W(0xb8001040, 0x7FFFFF80);      //3.2 MPMR,Enable Power Down mode          
		nop
		REG32_W(0xb8001050, 0xE3100000); // DDCR
                nop
                REG32_W(0xb8000010, 0x00000b08); // CLKMGR, the value is from start_c.c
                nop
                REG32_W(0xb8001008, 0x48c26190);//SDR 16MB 156MHZ
                nop
                REG32_W(0xb8001004, 0x52480000);
                nop                
	#elif  defined(CONFIG_D8_16)
			        nop
                REG32_W(0xb8001040, 0x3FFFFF80);      //3.1 MPMR,set default=0x3FFFFF80 , minimun Power saving parameter   ^M
              	nop 
		REG32_W(0xb8001040, 0x7FFFFF80);      //3.2 MPMR,Enable Power Down mode          
		nop
		REG32_W(0xb8001050, 0xE3100000); // DDCR
                nop
                REG32_W(0xb8000010, 0x00000b08); // CLKMGR, the value is from start_c.c
                nop
                REG32_W(0xb8001008, 0x48c26190);//SDR 8MB 156MHZ
                nop
                REG32_W(0xb8001004, 0x52080000);
                nop                
	#endif

	#ifdef CONFIG_RTL8196E
		REG32_ANDOR(0xb8000048, ~(1<<19),(0<<19));	// enable ssc function for SDR only
		nop
	#endif                	
#endif

#ifdef CONFIG_RTL8196D
                nop
		REG32_ANDOR(0xb8000088, ~(0x6000300F),(0x3008)); // for Freq=2M, ¸ÉÀv­È, 1.0V¤ÉÀ£¤@®æ
                nop
		REG32_ANDOR(0xb800008c, ~(0x1f<<2),(0x1f<<2)); // maximum OCP
                nop
		REG32_ANDOR(0xb8000088, ~(3<<5)&~(0xF<<0),(1<<4));  //for internal SWR issue
        nop
        REG32_ANDOR(0xb8000088, ~(3<<7),0);
        nop

#endif  

#ifdef CONFIG_RTL8196E
                nop
		REG32_ANDOR(0xb8000088, ~(3<<29),(0<<29)); // for Freq=2M
                nop
		REG32_ANDOR(0xb800008c, ~(0x1f<<2),(0x1f<<2)); // maximum OCP
                nop
#endif  

#if defined(CONFIG_RTL8196E_ULINKER_BOOT_LED)
		REG32_ANDOR(0xb8000044, ~(3<<12), (3<<12));
#endif
		
#if !defined(CONFIG_NFBI_UNKNOW_DRAM)	
lab_ddr_cali:

		jal DDR_Auto_Calibration
		nop
#endif			

                             
// flush all cache
		mtc0	zero, $20	//cache control register:  *(20)=0x00
		nop

		li		t0, 0x3
		mtc0	t0, $20		//cache control register:  *(20)=0x03
		nop

		mtc0	zero, $20	//cache control register:  *(20)=0x00



//--------------------------------------------------------------------------------		
// Load to 80100000 for compress
// Load to 80000000 for non-compress

//#define __boot_start 0xbfc00000
//#define __boot_end   0xbfcd0000 
//#define BOOT_ADDR    0x80100000

		la		k0, __boot_start
		la		k1, (__boot_end + 4)
		la		t1, BOOT_ADDR
1:
		lw		t0, 0(k0)
		nop

		sw		t0, 0(t1)	//*(t1)=*(t0)   t1+=4;  t0+=4;   t1=BOOT_ADDR, t0=__boot_start, run loop
		nop

		addu	t1, 4
		
		addu	k0, 4
		
		bne		k1, k0, 1b
		nop


//--------------------------------------------------------------------------------				
//		Jump to booting	
		li		k0, BOOT_ADDR
		jr		k0
		nop 
              //all END         
              
//======================================================================
//  Sub-Function : DDR_Auto_Calibration
//  Description  : Do DDR1 DDR2 calibration.

	EXPORT(DDR_Auto_Calibration)	
DDR_Auto_Calibration:
################DDR DDCR calibration######################

			
		################ DDR DDCR calibration assembly code start ######################
        	   			

			li	t3,0xa0000000    	//(t3)=dram test address		                      
			li	v0,0x5a5aa5a5      	//(v0)=dram test pattern
			li	t2,0xb8001050        //(t2)=DDR calibration register                       

			li	t1,0x80000000 //digital delay line(SW)     //(t1)=DDCR_Pattern
//			lui		t1,0x0     //analog delay line(HW)    

			li	a2,0             //L0
			li	t6,33        	//R0
			
			li	t5,0                //L1
			li	t7,33              //R1
			
			sw	v0,0(t3)        // DRAM_ADDR=0x5a5aa5a5                             

			li	v1,1           	        //v1=1,Calibration for DQS0 ,v1 will run 1~32 times   //(v1)=DQS0
			move	a0,t1              //(a0)=DDCR_Pattern        
			
			li	t4,0x00ff00ff          // (t4)=mask
		   		                
			li	t0,0x005a00a5            //(t0)=expect valeue		              
		       	             
			move	a3,t4                         
			move	a1,t0                             
			addiu	v0,v1,-1                    

	lab_loop_DQS0:
			sll	v0,v1,25     // tmp=(DQS0<< DQS0_offset)                   
			or	v0,a0,v0                 //tmp=DDCR_Pattern | tmp
			
			bnez	a2,lab_notzero      //if(L0==0), continue.
			sw	v0,0(t2)               //DDCR=tmp     

	lab_zero: //wei add, just for easy read		
			lw	v0,0(t3)         //check read, (v0)= val                      
			nop                                        
			and	v0,v0,t4                       // val= val &mask        
			bne	v0,t0,lab_next_DQS0    // compare val and exp_value
			nop           
			
			j lab_next_DQS0             
			move	a2,v1    //save to L0

	lab_notzero:                           
			lw	v0,0(t3)                                
			nop                                        
			and	v0,v0,a3               // val= val &mask         
			bne	v0,a1,lab_end_DQS0     
			nop     

	lab_next_DQS0:                                   
			addiu	v1,v1,1          //V1++  , DQS0++                  
			sltiu	v0,v1,33                    
			bnez	 v0,lab_loop_DQS0      

       lab_end_DQS0:   
			  
			addiu	t8,v1,-1 	//saved R0's value to t8

			li	v0,0xc0000000 //digital (SW)    
//			li	v0,0x40000000 //analog (HW)    

			and	t1,t1,v0                               
			//addu	v0,a2,t6   
			
			add	v0,t8,a2   	//(L0+R0)			
			srl	v0,v0,0x1      //c0=(L0+R0)/2



DDCR_SHIFT_EXIT:
			move t9,v0	  //command t9 if no set DQS1	
			                              
			sll	v0,v0,25       //DQS0 Offset
			sll 	t9,t9,20  //command t9 if no set DQS1	//DQS1 Offset
			or	t1,t1,v0
			or    t1,t1,t9		  //command t9(and DDR_Calibration_end) if no set DQS1	
			
			sw	t1,0(t2)      //set DDCR DQS0/DQS1   //DDCR=(0xc0000000) | (C0<<25) | (C0<<20)
			nop
			nop

			 j DDR_Calibration_end //end of set DDCR if don't try DQS1 
			nop



	############### DDR DDCR calibration assembly code end  #####################	
			
	
	DDR_Calibration_end:  	

		jr ra
		nop




//==========================================================================	
//  Sub-Function : uart_show
//  Description  : Show "booting" word

	EXPORT(uart_show)
uart_show:	

	//uart_init:		
  	REG32_W( UART_LCR,0x03000000);
  		
  	REG32_W( UART_FCR,0xc7000000);
  	REG32_W( UART_IER,0x00000000);
  	dl = (SYS_CLK_RATE /16)/BAUD_RATE-1;

  	dll = dl & 0xff;
  	dlm = dl / 0x100;
  	REG32_W( UART_LCR,0x83000000);
  	REG32_W( UART_DLL,dll*0x1000000);
   	REG32_W( UART_DLM,dlm*0x1000000); 
	REG32_W( UART_LCR,0x83000000& 0x7fffffff);

	//uart show
	UART_PRINT(boot_msg);
	j ra 
 	nop

boot_msg:	.ascii "\r\nBooting...\r\n\0"
rxdly_msg:	.ascii "T\\R| 0 1 2 3 4 5 6 7 8 9 a b c d e f 0 1 2 3 4 5 6 7 8 9 a b c d e f\r\n\0"
seprate_msg:	.ascii "---+-----------------------------------------------------------------\r\n\0"	

 
txdly_msg:	.ascii "abcdefghijk\0"
ascii_tab:         .ascii "0123456789abcdef\0"

	.align 4

//==========================================================================	



//boot_msg:	.ascii "\r\nBooting...\r\n\0"	

DTR_msg:	.ascii "\r\nDTR Done.\r\n\0"	

DCR_msg:	.ascii "\r\nDCR Done.\r\n\0"	

DDCR_msg:	.ascii "\r\nDDCR Done.\r\n\0"	



