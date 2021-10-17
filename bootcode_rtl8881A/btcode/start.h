#ifndef __RTL_START_H__
#define __RTL_START_H__

#include "../autoconf.h"

#define	BOOT_ADDR		0x80100000	//compress
//#define	BOOT_ADDR		0x80000000    // no compress
#define JUMPADDR 0x80100000

/*For start_c.c debug message*/
#define DBG 0 //notice boot.bin can't over 323KB when enable it 

/*For DDR write calibration mechanism*/
//#define CONFIG_DRAM_Write_Calibration 0 //only for QA Board
#define CONFIG_Calibration_DQS_EN_TAP 0 //Calibration_DQS_EN_TAP
#define CONFIG_RX_SLOW_BIT 0 //Enable DRAM RX_Slow 1 cycle function
#define CONFIG_DRAM_MODE_REGISTER 0



#define REG32(reg)	(*(volatile unsigned int *)(reg))

#define SFCR					0xb8001200	


#define CHECK_DCR_READY()   { while(REG32(DCR_REG) & 1)  {}; }   // 1=busy, 0=ready
#define DCR_REG 0xb8001004
#define DTR_REG 0xb8001008
#define DDCR_REG 0xb8001050

#define SYS_HW_STRAP   (0xb8000000 +0x08)
#define  ADDR   0xA0100000
#define  PATT0  0xa5005a00
#define  PATT1  0x5a00a500
#define  PATT2  0xa5a55a5a
#define  PATT3  0x5a5aa5a5
#define  PATT4  0x55aaaa55	
#define CLK_MANAGER 0xb8000010
#define DTR 0xb8001008
#define DCR 0xb8001004
#define HS0_CONTROL 0xb80000a0	
	
#define DLL_REG 0xb8000038
#define PAD_CONTROL 0xb8000048
#define PLL2 0xb8000058
#define PAD_CONTROL2 0xb80000d0
#define PAD_CONTROL3 0xb80000d4
#define PAD_CONTROL4 0xb80000d8

#define DSDR1 0xb80010a0
#define DSDR2 0xb80010a4
#define DSDR3 0xb80010a8
#define DSDR4 0xb80010ac
#define DSDR5 0xb80010b0

#define CSDR1 0xb8001090
#define CSDR2 0xb8001094 
#define CSDR3 0xb8001098

#define DSTR 0xb8001080

#define SWR_LDO1_REG 0xb8000088


	/*EMR1 Setting */			

#define EDTCR_REG 0xb800100c
#define MR_MODE(val)   ((val)<<14) /*2 bit */
#define MR_DATA(val)   ((val)<<0) /*14 bit */
	 
#define DCR_REG 0xb8001004
#define MR_MODE_EN (1<<14)
	 
#define EMR1_REG 0xb8001084 //Record
#define RTT_0ohm (0)
#define RTT_75ohm (1<<2)
#define RTT_150ohm (1<<6)
#define RTT_50ohm ((1<<2)|(1<<6))
#define OCD_DEFAULT ((1<<7)|(1<<8)|(1<<9))
#define MR_DATA_ALL_0xF 0x3FFF
#define DLL_DISABLE (1<<0)
#define ODIT_60percent (1<<1)
#define T_WTR29_27 (7<<27)
#define DDR1_MR_OP_RST_DLL	(0x00000100)

		/* DDR2 Mode register related definition */
#define DDR2_MR_BURST_4 	(0x00000002)
#define DDR2_MR_BURST_8 	(0x00000003)
#define DDR2_MR_BURST_SEQ 	(0x00000000)
#define DDR2_MR_BURST_INTER 	(0x00000008)
#define DDR2_MR_CAS_2 		(0x00000020)
#define DDR2_MR_CAS_3 		(0x00000030)
#define DDR2_MR_CAS_4 		(0x00000040)
#define DDR2_MR_CAS_5 		(0x00000050)
#define DDR2_MR_CAS_6 		(0x00000060)
#define DDR2_MR_TM_NOR 		(0x00000000)
#define DDR2_MR_TM_TEST		(0x00000080)
#define DDR2_MR_DLL_RESET_YES	(0x00000100)
#define DDR2_MR_DLL_RESET_NO	(0x00000000)
#define DDR2_MR_WR_2		(0x00000200)
#define DDR2_MR_WR_3		(0x00000400)
#define DDR2_MR_WR_4		(0x00000600)
#define DDR2_MR_WR_5		(0x00000800)
#define DDR2_MR_WR_6		(0x00000A00)
#define DDR2_MR_WR_7		(0x00000C00)
#define DDR2_MR_PD_FAST		(0x00000000)
#define DDR2_MR_PD_SLOW		(0x00001000)
#define DDR2_EMR1_DLL_EN	(0x00000000)
#define DDR2_EMR1_DLL_DIS	(0x00000001)
#define DDR2_EMR1_DIC_FULL	(0x00000000)
#define DDR2_EMR1_DIC_REDUCE	(0x00000002)
#define DDR2_EMR1_RTT_DIS	(0x00000000)
#define DDR2_EMR1_RTT_75	(0x00000004)
#define DDR2_EMR1_RTT_150	(0x00000040)
#define DDR2_EMR1_RTT_50	(0x00000044)
#define DDR2_EMR1_ADD_0		(0x00000000)
#define DDR2_EMR1_ADD_1		(0x00000008)
#define DDR2_EMR1_ADD_2		(0x00000010)
#define DDR2_EMR1_ADD_3		(0x00000018)
#define DDR2_EMR1_ADD_4		(0x00000020)
#define DDR2_EMR1_ADD_5		(0x00000028)
#define DDR2_EMR1_OCD_EX	(0x00000000)
#define DDR2_EMR1_OCD_D1	(0x00000080)
#define DDR2_EMR1_OCD_D0	(0x00000100)
#define DDR2_EMR1_OCD_AD	(0x00000200)
#define DDR2_EMR1_OCD_DEF	(0x00000380)
#define DDR2_EMR1_QOFF_EN	(0x00000000)
#define DDR2_EMR1_QOFF_DIS	(0x00001000)
#define DDR2_EMR1_NDQS_EN	(0x00000000)
#define DDR2_EMR1_NDQS_DIS	(0x00000400)
#define DDR2_EMR1_RDQS_EN	(0x00000800)
#define DDR2_EMR1_RDQS_DIS	(0x00000000)
#define DDR2_EMR2_HTREF_EN	(0x00000080)
#define DDR2_EMR2_HTREF_DIS	(0x00000000)
#define DDR2_EMR2_DCC_DIS	(0x00000000)
#define DDR2_EMR2_DCC_EN	(0x00000008)
#define DDR2_EMR2_PASELF_FULL	(0x00000000)
		

#define POSTED_CAS(val)    ((val)<<3) /*3 bit */
#define DQS_DISABLE (1<<10) /*0:enable , 1:disable*/


//-------------------------------------------------
// Using register: t6, t7
// t6=value
// t7=address
#define REG32_R(addr,v)  	li t7,addr;\
						lw v, 0(t7);\
						nop;

// Using register: t6, t7           value support "constant" and "register" access, so use "or" to instead "li"
#define REG32_W(addr,v) 		or t6,zero,v;\
					li t7,addr;\
					sw t6, 0(t7);\
					nop;

#define REG32_ANDOR(addr,andV,orV) 	 li t7,addr;\
				         lw t6, 0(t7);\
				         and t6,t6,andV;\
				         or t6,t6,orV;\
				         sw t6, 0(t7);\
				         nop;
									   


//uart register
#define IO_BASE         0xB8000000
#define UART_RBR	(0x2000+IO_BASE)
#define UART_THR	(0x2000+IO_BASE)
#define UART_DLL	(0x2000+IO_BASE)
#define	UART_IER	(0x2004+IO_BASE)
#define	UART_DLM	(0x2004+IO_BASE)
#define	UART_IIR	(0x2008+IO_BASE)
#define	UART_FCR	(0x2008+IO_BASE)
#define UART_LCR	(0x200c+IO_BASE)
#define	UART_MCR	(0x2010+IO_BASE)
#define	UART_LSR	(0x2014+IO_BASE)
#define	UART_MSR	(0x2018+IO_BASE)
#define	UART_SCR	(0x201c+IO_BASE)



//---------------------------------------

#ifdef CONFIG_FPGA_ROMCODE
#define SYS_CLK_RATE	  	(  40000000)      //40MHz
#else
#define SYS_CLK_RATE	  	( 200000000)      //200MHz
#endif


#define BAUD_RATE	  	(38400) 

//Using reg: t6,t7
#define UART_WRITE(c)		  1:   REG32_R(UART_LSR,t6);\
								and t6,t6,0x60000000;\								
								beqz		t6,  1b;\
								nop	   ;\								
								REG32_W(UART_THR, c<<24);	

//----------------------------------------------------


// Using register: t5, t6, t7     t5=msg(idx)
#define UART_PRINT(msg)		la   t5,msg;\
								1:	lbu  t6,0(t5);\		
									beqz		t6,  2f;\
									addu	t5, 1;\	
									              ;\
									sll t6,t6,24;\
									REG32_W(UART_THR, t6);\
									j 1b;\
									nop;\
								2:	

#ifdef CONFIG_NAND_FLASH

 /*For showing NAND Flash booting DMA times*/
#define UART_PRINT_DELAY(msg)		la   s5,msg;\									
								1:	lbu  s6,0(s5);\		
									beqz		s6,  2f;\
									addu	s5, 1;\	
									;\
									sll s6,s6,24;\						
									REG32_W(UART_THR, s6);\
									li s4,800;\
								3:	nop;\
									subu s4,s4,1;\
									bnez s4,3b;\
									nop	;\									
									j 1b;\
									nop;\
								2:	

#else


// Using register: t4, t5, t6, t7     t5=msg(idx), t4=delay loop count
#define UART_PRINT_DELAY(msg)		la   t5,msg;\									
								1:	lbu  t6,0(t5);\		
									beqz		t6,  2f;\
									addu	t5, 1;\	
									;\
									sll t6,t6,24;\						
									REG32_W(UART_THR, t6);\
									li t4,0x100;\
								3:	nop;\
									subu t4,t4,1;\
									bnez t4,3b;\
									nop	;\									
									j 1b;\
									nop;\
								2:	
#endif

//0x00 show ascii '0'
//0x0a show ascii 'a'
//0x1a show ascii 'a', skip 1
#define UART_BIN2HEX(v) 	or t6,zero,v;\
	                                          and t6,t6,0x000f;\
							li t7,'0';\
							add t6,t6,t7;\							
							li t7,'9';\
							bleu t6,t7,1f;\
							nop;\
							li t7,'a'-'9'-1;\
							add t6,t6,t7;\
						1:;\							 
							sll t6,t6,24;\
							REG32_W(UART_THR, t6);






#define VIR2PHY(x) (x&0x1fffffff)

//#define SRAM_BASE (0xbfc00000+0x8000)	//ROM Booting	//24K		
//#define SRAM_BASE (0x80000000)	//SPI NOR
//#define SRAM_BASE (0xbfc00000)	//NFBI NAND							
#define SRAM_BASE (0x80000000+(128<<20))  //32M



#define SRAM_TOP (SRAM_BASE+0x1000)  //4K

//----------------------------------------------------
#endif

