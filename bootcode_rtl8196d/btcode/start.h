#ifndef __RTL_START_H__
#define __RTL_START_H__


#include "../autoconf.h"



#define	BOOT_ADDR		0x80100000	//compress
//#define	BOOT_ADDR		0x80000000    // no compress


//-------------------------------------------------
// Using register: t6, t7           //wei add this code
// t6=value
// t7=address
#define REG32_R(addr,v)  	or t7,zero,addr;\
						lw v, 0(t7);\						
						nop;

// Using register: t6, t7           value support "constant" and "register" access, so use "or" to instead "li"
#define REG32_W(addr,v) or t6,zero,v;\
						or t7,zero,addr;\
						sw t6, 0(t7);\
						nop;

// Using register: t6, t7           //wei add this code
#define REG32_ANDOR(addr,andV,orV) 	 li t7,addr;\
						               lw t6, 0(t7);\
						               and t6,t6,andV;\
						               or t6,t6,orV;\
						               sw t6, 0(t7);\
						               nop;


// Using register: t6, t7           //wei add this code
#define IF_EQ(a,b,lab)		or t6,zero,a;\
						or t7,zero,b;\
						beq t6,t7,lab;\
						nop;

#define IF_NEQ(a,b,lab)		or t6,zero,a;\
						or t7,zero,b;\
						bne t6,t7,lab;\
						nop;


#define ADD3VAL(r,v1,v2,v3)  add r,v2,v1;\							
							add r,r,v3;
						
						
						


//uart register
#define   UART_BASE         0xB8002000
#define   UART_RBR	(0x00+UART_BASE)
#define   UART_THR	(0x00+UART_BASE)
#define   UART_DLL	(0x00+UART_BASE)
#define	UART_IER	(0x04+UART_BASE)
#define	UART_DLM	(0x04+UART_BASE)
#define	UART_IIR	(0x08+UART_BASE)
#define	UART_FCR	(0x08+UART_BASE)
#define   UART_LCR	(0x0c+UART_BASE)
#define	UART_MCR	(0x10+UART_BASE)
#define	UART_LSR	(0x14+UART_BASE)
#define	UART_MSR	(0x18+UART_BASE)
#define	UART_SCR	(0x1c+UART_BASE)

//---------------------------------------
#define SYS_CLK_RATE	  	(200*1000000) 	
//#define SYS_CLK_RATE  	( 33.8688*1000000)      //33.8688MHz
//#define SYS_CLK_RATE	  	(  40*1000000)      //40Hz
//#define SYS_CLK_RATE	  	(  20*1000000)      //20Hz

#define BAUD_RATE	  	(38400) 

//Using reg: t6,t7
#define UART_WRITE(c)		  1:   REG32_R(UART_LSR,t6);\
								and t6,t6,0x60000000;\								
								beqz		t6,  1b;\
								nop	   ;\								
								REG32_W(UART_THR, c<<24);	
							


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
									nop;\
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

