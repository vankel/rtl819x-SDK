#ifndef __RTL_START_H__
#define __RTL_START_H__

#include "../autoconf.h"

#ifdef CONFIG_NONO_COMPRESS
        #define	BOOT_ADDR		0xA0000000    // no compress
#else
        #define	BOOT_ADDR		0xa0100000	//compress
#endif


// Using register: t6, t7           //wei add this code
#define IF_EQ(a,b,lab)          or t6,zero,a;\
                                or t7,zero,b;\
                                beq t6,t7,lab;\
                                nop;

#define IF_NEQ(a,b,lab)         or t6,zero,a;\
                                or t7,zero,b;\
                                bne t6,t7,lab;\
                                nop;



#define  ADDR   0xA0380000
	#define  PATT0  0x5a0000a5
	#define  PATT1  0xA5A55A5A
	#define  PATT2  0x005AA500
	#define  PATT3  0xA5A55A5A
	#define  PATT4  0xAAAAAAAA
	#define  PATT5  0xffffffff
	#define  PATT6  0x00000000
	#define  PATT7  0x00000000
	#define  PATT8  0xffffffff
	#define CLK_MANAGER 0xb8000010
	#define DCR 0xb8001004


#define DRAM_DDR2 1
	#define DRAM_DDR3 2

	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf		
	#define RANG5 0x1f	
	

	#define SYS_HW_STRAP   (0xb8000000 +0x08)	


#define SRAM_REG_SET_SIZE	(0x10)
#define SRAM_REG_SET_NUM	(4)
#define SRAM_REG_BASE_ADDR	(0xB8004000)



#define C0SRAMSAR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x0)
#define C0SRAMSSR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x4)
#define C0SRAMSBR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x8)

#define C1SRAMSAR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x40)
#define C1SRAMSSR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x44)
#define C1SRAMSBR_REG_ADDR	(SRAM_REG_BASE_ADDR + 0x48)

#define SRAM_SEG_ENABLE		(0x1)

#define SRAM_SIZE_256B		(0x1)
#define SRAM_SIZE_512B		(0x2)
#define SRAM_SIZE_1KB		(0x3)
#define SRAM_SIZE_2KB		(0x4)
#define SRAM_SIZE_4KB		(0x5)
#define SRAM_SIZE_8KB		(0x6)
#define SRAM_SIZE_16KB		(0x7)
#define SRAM_SIZE_32KB		(0x8)
#define SRAM_SIZE_64KB		(0x9)
#define SRAM_SIZE_128KB		(0xA)
#define SRAM_SIZE_256KB		(0xB)
#define SRAM_SIZE_512KB		(0xC)
#define SRAM_SIZE_1MB		(0xD)



//-------------------------------------------------
// Using register: t6, t7
// t6=value
// t7=address
#define REG32_R(addr,v)  	li t7,addr;\
						lw v, 0(t7);\
						nop;

#ifdef CONFIG_NAND_FLASH_BOOTING
#define REG32_W(addr,v) 		or t6,zero,v;\
					li t7,addr;\
					sw t6, 0(t7);\
					nop;
#else
// Using register: t6, t7           value support "constant" and "register" access, so use "or" to instead "li"
#define REG32_W(addr,v) 		li t6,v;\
					li t7,addr;\
					sw t6, 0(t7);\
					nop;
#endif

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
//#define SYS_CLK_RATE	  	(200000000) 	
//#define SYS_CLK_RATE	  	(  33868800)      //33.8688MHz
//#define SYS_CLK_RATE	  	(  20000000)      //20MHz

#if 0//def CONFIG_FPGA_ROMCODE
//#define SYS_CLK_RATE	  	(  40000000)      //40MHz
#define SYS_CLK_RATE	  	(  25000000)      //25MHz
//#define SYS_CLK_RATE	  	(  33868800)      //25MHz
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
					                                li t7,UART_THR;\
					                                sw t6, 0(t7);\
									j 1b;\
									nop;\
								2:	


#ifdef CONFIG_NAND_FLASH_BOOTING
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
#define SRAM_BASE (0x80000000+(64<<20))  //32M



#define SRAM_TOP (SRAM_BASE+0x1000)  //4K
//#define SRAM_TOP (SRAM_BASE+0x2000)  //8K

#define MFC0_SEL(dst, src, sel)  .word (0x40000000 | ((dst) << 16) | ((src) << 11) | (sel))
#define MTC0_SEL(dst, src, sel)  .word (0x40800000 | ((dst) << 16) | ((src) << 11) | (sel))


//----------------------------------------------------
#endif

#if 1 //RTL8198C

#define CLK_MANAGE				(0xB8000010)
//----------------------------------------------------
#define MEMCTL_DTR_ADDR 	(0xb8001008)
#define MEMCTL_DTR1_ADDR 	(0xb800100c)
#define MEMCTL_DTR2_ADDR 	(0xb8001010)
#define MCR			(0xB8001000)
#define DCR			(0xB8001004)
#define DTR0			(0xB8001008)
#define DTR1			(0xB800100c)
#define DTR2			(0xB8001010)
#define DMCR			(0xB800101C)
#define SOCPNR			(0xB80010FC)
#define DOR				(0xB8001064)
#define DWPR1			(0xB8001800)


#define DACCR			(0xB8001500)
#define DACSPCR			(DACCR+0x4)
#define DACDQF			(DACCR+0x50)
#define DACDQR			(DACCR+0x10)
#define DCDQMR			(DACCR+0x90)

#define DIDER1			(0xB8001050)
#define DIDER2			(0xB8001054)
#define DIDER3  			(0xB8001058)
#define DACDQ0  			(0xB8001510)

//#define DCDR			(0xB8001060)

#define D3ZQCCR (0xB8001080)

#define DACDQR_DQR_PHASE_SHIFT_90_FD_S (24)
#define DACDQR_DQR_PHASE_SHIFT_90_MASK (0x1f << DACDQR_DQR_PHASE_SHIFT_90_FD_S)
#define DACDQR_DQR_MAX_TAP_FD_S (16)
#define DACDQR_DQR_MAX_TAP_MASK (0x1f << DACDQR_DQR_MAX_TAP_FD_S)
#define DACDQR_DQR_MIN_TAP_FD_S (0)
#define DACDQR_DQR_MIN_TAP_MASK (0x1f << DACDQR_DQR_MIN_TAP_FD_S)

#define DCDQMR_DQM0_PHASE_SHIFT_90_FD_S (24)
#define DCDQMR_DQM0_PHASE_SHIFT_90_MASK (0x1f << DCDQMR_DQM0_PHASE_SHIFT_90_FD_S)
#define DCDQMR_DQM1_PHASE_SHIFT_90_FD_S (16)
#define DCDQMR_DQM1_PHASE_SHIFT_90_MASK (0x1f << DCDQMR_DQM1_PHASE_SHIFT_90_FD_S)

/* Field start bit definition */
#define MCR_D_INIT_TRIG_FD_S	(14)
#define DTR0_CAS_FD_S		(28)
#define DTR0_WR_FD_S 		(24)
#define DTR0_CWL_FD_S 		(20)
#define DTR0_RTP_FD_S 		(16)
#define DTR0_WTR_FD_S 		(12)
#define DTR0_REFI_FD_S 		(8)
#define DTR0_REFI_UNIT_FD_S 	(4)
#define DTR1_RP_FD_S 		(24)
#define DTR1_RCD_FD_S 		(16)
#define DTR1_RRD_FD_S 		(8)
#define DTR1_FAWG_FD_S 		(0)
#define DTR2_RFC_FD_S 		(20)
#define DTR2_RAS_FD_S 		(12)

#define MCR_D_INIT_TRIG_MASK	(0x1 << MCR_D_INIT_TRIG_FD_S)
#define DTR0_REFI_MASK 		(0xF << DTR0_REFI_FD_S)
#define DTR0_REFI_UNIT_MASK 	(0xF << DTR0_REFI_UNIT_FD_S)
#define DTR0_CAS_MASK		(0xF << DTR0_CAS_FD_S)
#define DTR0_WR_MASK 		(0xF << DTR0_WR_FD_S)
#define DTR0_CWL_MASK 		(0xF << DTR0_CWL_FD_S)
#define DTR0_RTP_MASK 		(0xF << DTR0_RTP_FD_S)
#define DTR0_WTR_MASK 		(0xF << DTR0_WTR_FD_S)
#define DTR1_RP_MASK 		(0xFF << DTR1_RP_FD_S)
#define DTR1_RCD_MASK 		(0xFF << DTR1_RCD_FD_S)
#define DTR1_RRD_MASK 		(0xFF << DTR1_RRD_FD_S)
#define DTR1_FAWG_MASK 		(0xFF << DTR1_FAWG_FD_S)
#define DTR2_RFC_MASK 		(0xFFF << DTR2_RFC_FD_S)
#define DTR2_RAS_MASK 		(0x3F << DTR2_RAS_FD_S)


#define DCR_BANKCNT_FD_S	(28)
#define DCR_DBUSWID_FD_S	(24)
#define DCR_ROWCNT_FD_S		(20)
#define DCR_COLCNT_FD_S		(16)
#define DCR_DCHIPSEL_FD_S	(15)
#define DCR_BANKCNT_MASK	(0xF << DCR_BANKCNT_FD_S)
#define DCR_DBUSWID_MASK	(0xF << DCR_DBUSWID_FD_S)
#define DCR_ROWCNT_MASK		(0xF << DCR_ROWCNT_FD_S)
#define DCR_COLCNT_MASK		(0xF << DCR_COLCNT_FD_S)
#define DCR_DCHIPSEL_MASK	(1 << DCR_DCHIPSEL_FD_S)


#define DDCR_DQS0_TAP_FD_S 	(25)
#define DDCR_DQS1_TAP_FD_S 	(20)
#define DDCR_CAL_MODE_FD_S 	(31)
#define DCDR_PHASE_SHIFT_FD_S 	(17)


/* The values of field in the registers related to the memory controller. */
#define DLL_MODE 		(0)
#define DIGITAL_MODE 		(1)
#define MCR_DRAMTYPE_MASK	(0xF0000000)
#define MCR_DRAMTYPE_DDR 	(0x00000000)
#define MCR_DRAMTYPE_DDR2 	(0x10000000)
#define MCR_DRAMTYPE_DDR3 	(0x20000000)

#define MCR_PREFETCH_ENABLE_INS (0x00800000)
#define MCR_PREFETCH_MODE_IOLD (0x00000000)
#define MCR_PREFETCH_MODE_INEW (0x08000000)
#define MCR_PREFETCH_ENABLE_DATA (0x00400000)
#define MCR_PREFETCH_MODE_DOLD (0x00000000)
#define MCR_PREFETCH_MODE_DNEW (0x04000000)
#define MCR_PREFETCH_INS_SIDE  (0x2)
#define MCR_PREFETCH_DATA_SIDE (0x1)
#define MCR_PREFETCH_OLD_MECH	(0)
#define MCR_PREFETCH_NEW_MECH	(1)

/* Field masks used for extracting the value in the corresponding fields. */
//#define MCR_DRAMTYPE_MASK 	(0x80000000)
#define MCR_PREFETCH_DIS_IMASK (0xFF7FFFFF)
#define MCR_PREFETCH_DIS_DMASK (0xEFBFFFFF)
#define MCR_PREFETCH_MODE_IMASK (0xFF7FFFFF)
#define MCR_PREFETCH_MODE_DMASK (0xFFBFFFFF)
#define DDCR_DQS0_TAP_MASK 	(31 << DDCR_DQS0_TAP_FD_S)
#define DDCR_DQS1_TAP_MASK 	(31 << DDCR_DQS1_TAP_FD_S)
#define DDCR_CAL_MODE_MASK 	(1 << DDCR_CAL_MODE_FD_S)
#define DDCR_CAL_MODE_DLL 	(0 << DDCR_CAL_MODE_FD_S)
#define DDCR_CAL_MODE_DIG 	(1 << DDCR_CAL_MODE_FD_S)
#define DCDR_PHASE_SHIFT_MASK 	(31 << DCDR_PHASE_SHIFT_FD_S)

/* Timing constraints definition used for DRAM diagnosis. */
#define DDR_CAS2_MAX_MHZ	(143)
#define DDR_CAS25_MAX_MHZ	(167)
#define DDR_CAS3_MAX_MHZ	(200)
#define DDR_STD_REF_MS 		(64)
#define DDR_STD_WR_NS 		(15)
#define DDR_STD_RP_NS 		(20)
#define DDR_STD_RCD_NS 		(20)
#define DDR_STD_RAS_NS 		(45)
#define DDR_STD_RFC_NS		(75)
#define DDR_STD_RRD_NS 		(15)
#define DDR_STD_RTP_NS 		(0)
#define DDR_STD_FAWG_NS		(0)
#define DDR_STD_WTR_NS 		(10)


#define DDR2_CAS2_MAX_MHZ	(143)
#define DDR2_CAS3_MAX_MHZ	(200)
#define DDR2_CAS4_MAX_MHZ	(266)
#define DDR2_CAS5_MAX_MHZ	(400)
#define DDR2_CAS6_MAX_MHZ	(533)
#define DDR2_CAS7_MAX_MHZ	(533)
#define DDR2_STD_REF_MS		CONFIG_DRAM_REFI_MS
#define DDR2_STD_RCD_NS 	CONFIG_DRAM_RCD_NS
#define DDR2_STD_RP_NS 		CONFIG_DRAM_RP_NS
#define DDR2_STD_RAS_NS 	CONFIG_DRAM_RAS_NS
#define DDR2_STD_RRD_NS 	CONFIG_DRAM_RRD_NS
#define DDR2_STD_FAWG_NS	CONFIG_DRAM_FAWG_NS
#define DDR2_STD_RTP_NS 	CONFIG_DRAM_RTP_NS
#define DDR2_STD_WTR_NS 	CONFIG_DRAM_WTR_NS
#define DDR2_STD_WR_NS 		CONFIG_DRAM_WR_NS
#define DDR2_STD_RFC_NS         CONFIG_DRAM_RFC_NS
#define DDR2_STD_RFC_32MB_NS	(75)
#define DDR2_STD_RFC_64MB_NS	(105)
#define DDR2_STD_RFC_128MB_NS	(128)
#define DDR2_STD_RFC_256MB_NS	(195)
#define DDR2_STD_RFC_512MB_NS	(328)


#define DDR3_CAS5_MAX_MHZ	(400)
#define DDR3_CAS6_MAX_MHZ	(400)
#define DDR3_CAS7_MAX_MHZ	(533)
#define DDR3_CAS8_MAX_MHZ	(533)
#define DDR3_CAS9_MAX_MHZ	(666)
#define DDR3_CAS10_MAX_MHZ	(666)
#define DDR3_CAS11_MAX_MHZ	(800)

#define DDR3_CWL5_MAX_MHZ	(400)
#define DDR3_CWL6_MAX_MHZ	(533)
#define DDR3_CWL7_MAX_MHZ	(666)
#define DDR3_CWL8_MAX_MHZ	(800)

#define DDR3_STD_REF_MS		(64)
#define DDR3_STD_RCD_NS 	(15)
#define DDR3_STD_RP_NS 		(15)
#define DDR3_STD_RAS_NS 	(38)
#define DDR3_STD_WTR_CK 	(4)
#define DDR3_STD_WTR_NS 	(8)
#define DDR3_STD_WR_NS 		(15)
#define DDR3_STD_FAWG_NS	(50)
#define DDR3_STD_RTP_NS 	(8)
#define DDR3_STD_RTP_CK 	(4)
#define DDR3_STD_RRD_NS 	(10)
#define DDR3_STD_RRD_CK 	(4)

#define DDR3_STD_RFC_128MB_NS	(110)
#define DDR3_STD_RFC_256MB_NS	(160)
#define DDR3_STD_RFC_512MB_NS	(300)
#define DDR3_STD_RFC_1GB_NS	(350)


#define DMCR_MRS_BUSY		(0x80000000)
#define DMCR_MR_MODE_EN		(0x00100000)
#define DMCR_MRS_MODE_MR	(0x00000000)
#define DMCR_MRS_MODE_EMR1	(0x00010000)
#define DMCR_MRS_MODE_EMR2	(0x00020000)
#define DMCR_MRS_MODE_EMR3	(0x00030000)
#define DMCR_DIS_DRAM_REF_FD_S	(24)
#define DMCR_DIS_DRAM_REF_MASK	(1 << DMCR_DIS_DRAM_REF_FD_S)
#define DMCR_MR_MODE_EN_FD_S	(20)
#define DMCR_MR_MODE_EN_MASK	(1 << DMCR_MR_MODE_EN_FD_S)


/* DDR Mode register related definition */
#define DDR1_MR_BURST_2		(0x00000001)
#define DDR1_MR_BURST_4		(0x00000002)
#define DDR1_MR_BURST_8		(0x00000003)
#define DDR1_MR_BURST_SEQ	(0x00000000)
#define DDR1_MR_BURST_INTER	(0x00000008)
#define DDR1_MR_CAS_2 		(0x00000020)
#define DDR1_MR_CAS_3 		(0x00000030)
#define DDR1_MR_CAS_25 		(0x00000060)
#define DDR1_MR_OP_NOR		(0x00000000)
#define DDR1_MR_OP_RST_DLL	(0x00000100)
#define DDR1_MR_OP_TEST		(0x00000080)

#define DDR1_EMR1_DLL_EN	(0x00000000)
#define DDR1_EMR1_DLL_DIS	(0x00000001)
#define DDR1_EMR1_DRV_NOR	(0x00000000)
#define DDR1_EMR1_DRV_WEAK	(0x00000002)

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

/* DDR3 Mode register related definition */
#define DDR3_MR_BURST_8 		(0x00000000)
#define DDR3_MR_BURST_BC4OR8 		(0x00000001)
#define DDR3_MR_BURST_BC4 		(0x00000002)
#define DDR3_MR_READ_BURST_NIBBLE 	(0x00000000)
#define DDR3_MR_READ_BURST_INTER 	(0x00000008)
#define DDR3_MR_CAS_5 			(0x00000010)
#define DDR3_MR_CAS_6	 		(0x00000020)
#define DDR3_MR_CAS_7	 		(0x00000030)
#define DDR3_MR_CAS_8	 		(0x00000040)
#define DDR3_MR_CAS_9	 		(0x00000050)
#define DDR3_MR_CAS_10	 		(0x00000060)
#define DDR3_MR_CAS_11	 		(0x00000070)
#define DDR3_MR_TM_NOR	 		(0x00000000)
#define DDR3_MR_TM_TEST			(0x00000080)
#define DDR3_MR_DLL_RESET_YES		(0x00000100)
#define DDR3_MR_DLL_RESET_NO		(0x00000000)
#define DDR3_MR_WR_5			(0x00000200)
#define DDR3_MR_WR_6			(0x00000400)
#define DDR3_MR_WR_7			(0x00000600)
#define DDR3_MR_WR_8			(0x00000800)
#define DDR3_MR_WR_9			(0x00000A00)
#define DDR3_MR_WR_10			(0x00000C00)
#define DDR3_MR_WR_12			(0x00000E00)
#define DDR3_MR_PD_FAST			(0x00001000)
#define DDR3_MR_PD_SLOW			(0x00000000)
#define DDR3_EMR1_DLL_EN		(0x00000000)
#define DDR3_EMR1_DLL_DIS		(0x00000001)
#define DDR3_EMR1_DIC_RZQ_DIV6		(0x00000000)
#define DDR3_EMR1_DIC_RZQ_DIV7		(0x00000002)
#define DDR3_EMR1_RTT_NOM_DIS		(0x00000000)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV4	(0x00000004)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV2	(0x00000040)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV6	(0x00000044)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV12	(0x00000200)
#define DDR3_EMR1_RTT_NOM_RZQ_DIV8	(0x00000204)
#define DDR3_EMR1_ADD_0			(0x00000000)
#define DDR3_EMR1_ADD_CL_RD1		(0x00000008)
#define DDR3_EMR1_ADD_CL_RD2		(0x00000010)
#define DDR3_EMR1_WRITE_LEVEL_DIS	(0x00000000)
#define DDR3_EMR1_WRITE_LEVEL_EN	(0x00000080)
#define DDR3_EMR1_TDQS_DIS		(0x00000000)
#define DDR3_EMR1_TDQS_EN		(0x00000800)
#define DDR3_EMR1_QOFF_DIS		(0x00001000)
#define DDR3_EMR1_QOFF_EN		(0x00000000)
#define DDR3_EMR2_PASR_FULL		(0x00000000)
#define DDR3_EMR2_PASR_HALF_L		(0x00000001)
#define DDR3_EMR2_PASR_QUA		(0x00000002)
#define DDR3_EMR2_PASR_8TH_L		(0x00000003)
#define DDR3_EMR2_PASR_3_QUA		(0x00000004)
#define DDR3_EMR2_PASR_HALF_H		(0x00000005)
#define DDR3_EMR2_PASR_8TH_H		(0x00000007)
#define DDR3_EMR2_CWL_5			(0x00000000)
#define DDR3_EMR2_CWL_6			(0x00000008)
#define DDR3_EMR2_CWL_7			(0x00000010)
#define DDR3_EMR2_CWL_8			(0x00000018)
#define DDR3_EMR2_ASR_EN		(0x00000040)
#define DDR3_EMR2_ASR_DIS		(0x00000000)
#define DDR3_EMR2_SRT_NOR		(0x00000000)
#define DDR3_EMR2_SRT_EXT		(0x00000080)
#define DDR3_EMR2_RTT_WR_DIS		(0x00000000)
#define DDR3_EMR2_RTT_WR_RZQ_DIV4	(0x00000200)
#define DDR3_EMR2_RTT_WR_RZQ_DIV2	(0x00000400)
#define DDR3_EMR3_MPR_LOC_PRE_PAT	(0x00000000)
#define DDR3_EMR3_MPR_OP_NOR		(0x00000000)
#define DDR3_EMR3_MPR_OP_DATA		(0x00000004)


/* DDR Calibration */
#define MEMCTL_DDR_DQS_SE		(1)
#define MEMCTL_DDR_DQS_DIFF		(0)
#define MEMCTL_DDR_DLL_MODE		(0)
#define MEMCTL_DDR_DIG_DELAY_MODE	(1)

#define MEMCTL_DDRTYPE_DDRI	(1)
#define MEMCTL_DDRTYPE_DDRII	(2)
#define MEMCTL_DDRTYPE_DDRIII	(3)

#define MEMCTL_DACCR_AC_MODE_DLL (0<<31)
#define MEMCTL_DACCR_AC_MODE_DIG (1<<31)
#define MEMCTL_DACCR_AC_MODE_MASK (~(1<<31))
#define MEMCTL_DACCR_DQS_MODE_SE (0<<31)
#define MEMCTL_DACCR_DQS_MODE_DIF (1<<31)
#define MEMCTL_DACCR_DQS_MODE_MASK (~(1<<31))
#define MEMCTL_DACCR_DQS0_GRUP_TAP_SHIFT (16)
#define MEMCTL_DACCR_DQS1_GRUP_TAP_SHIFT (8)
#define MEMCTL_DACSPCR_PERIOD_EN_MASK	(~(1 << 31))

#define _MEMCTL_CALI_FAIL	      (0xffff)
#define _MEMCTL_CALI_PASS	      (0)
#define _MEMCTL_CALI_STATUS_LOOKING_A (1)
#define _MEMCTL_CALI_STATUS_LOOKING_B (2)
#define _MEMCTL_CALI_STATUS_LOOKING_C (3)
#define _MEMCTL_CALI_SATAUS_DONE      (4)
#define _MEMCTL_CALI_SATAUS_FAIL      (5)
#define _MEMCTL_CALI_SATAUS_OVERFLOW  (6)
#define _MEMCTL_CALI_SATAUS_OK	      (7)
#define _MEMCTL_CALI_PHASE_A_OVERFLOW (8)
#define _MEMCTL_CALI_PHASE_B_OVERFLOW (9)
#define _MEMCTL_CALI_PHASE_C_OVERFLOW (10)
#define _MEMCTL_CALI_PHASE_A_SATAUS_OK   (11)
#define _MEMCTL_CALI_STATUS_UNKNOWN   (12)
#define MEMCTL_CALI_FAIL	      (-1)
#define MEMCTL_CALI_PASS	      (0)

#define MIN_READ_DELAY_WINDOW  (1)
#define MIN_WRITE_DELAY_WINDOW (1)
#define MIN_WRITE_MASK_DELAY_WINDOW (3)


/* STATUS defintiion */
#define CALI_FAIL_DCDR_VALUE    (0xFFFE0000)
#define CALI_FAIL_DDCR_VALUE    (0xFFFFFC00)

/* Memory Unmapping */
#define MEMCTL_UNMAP_REG_SET_SIZE	(0x10)
#define MEMCTL_UNMAP_REG_SET_NUM	(4)
#define MEMCTL_UNMAP_REG_BASE_ADDR	(0xB8001300)

#define C0UMSAR_REG_ADDR	(MEMCTL_UNMAP_REG_BASE_ADDR + 0x0)
#define C0UMSSR_REG_ADDR	(MEMCTL_UNMAP_REG_BASE_ADDR + 0x4)

#define C1UMSAR_REG_ADDR	(MEMCTL_UNMAP_REG_BASE_ADDR + 0x40)
#define C1UMSSR_REG_ADDR	(MEMCTL_UNMAP_REG_BASE_ADDR + 0x44)

#define MEMCTL_UNMAP_SEG_ENABLE		(0x1)

#define MEMCTL_UNMAP_SIZE_256B		(0x1)
#define MEMCTL_UNMAP_SIZE_512B		(0x2)
#define MEMCTL_UNMAP_SIZE_1KB		(0x3)
#define MEMCTL_UNMAP_SIZE_2KB		(0x4)
#define MEMCTL_UNMAP_SIZE_4KB		(0x5)
#define MEMCTL_UNMAP_SIZE_8KB		(0x6)
#define MEMCTL_UNMAP_SIZE_16KB		(0x7)
#define MEMCTL_UNMAP_SIZE_32KB		(0x8)
#define MEMCTL_UNMAP_SIZE_64KB		(0x9)
#define MEMCTL_UNMAP_SIZE_128KB		(0xA)
#define MEMCTL_UNMAP_SIZE_256KB		(0xB)
#define MEMCTL_UNMAP_SIZE_512KB		(0xC)
#define MEMCTL_UNMAP_SIZE_1MB		(0xD)


/*
 * Board
 */
#undef  CONFIG_RTL8676
#define CONFIG_RTL8686 1
#define CONFIG_UART0 1
#undef  CONFIG_UART1

/*
 * Frequency configuration
 */
#define CONFIG_SOFTWARE_OVERWRITE_FREQ 1
#undef  CONFIG_SOFTWARE_FREQUENCY_ENV_DECIDE
#define CONFIG_CPUCLK_MHZ (500)
#define CONFIG_DSPCLK_MHZ (500)
#define CONFIG_MEMCLK_MHZ (100)

/*
 * Memory configuration
 */
#undef  CONFIG_DRAM_FROCE_DCR_VALUE
#undef  CONFIG_DRAM_BUS_WIDTH_8BIT
#define CONFIG_DRAM_BUS_WIDTH_16BIT 1
#define CONFIG_DRAM_CHIP_NUM_ONE 1
#undef  CONFIG_DRAM_CHIP_NUM_TWO
#undef  CONFIG_ONE_DRAM_CHIP_SIZE_32MB
#define CONFIG_ONE_DRAM_CHIP_SIZE_64MB 1
#undef  CONFIG_ONE_DRAM_CHIP_SIZE_128MB
#undef  CONFIG_ONE_DRAM_CHIP_SIZE_256MB
#undef  CONFIG_ONE_DRAM_CHIP_SIZE_512MB
#define CONFIG_ONE_DRAM_CHIP_SIZE 0x4000000
#define CONFIG_ZQ_AUTO_CALI 1
#define CONFIG_DIGITAL_DELAY_LINE 1
#define CONFIG_AUTO_DRAM_TIMING_SETTING 1
#define CONFIG_DRAM_CAS_CLK (6)
#define CONFIG_DRAM_WR_NS (15)
#define CONFIG_DRAM_CWL_CLK (5)
#define CONFIG_DRAM_RTP_NS (8)
#define CONFIG_DRAM_WTR_NS (9)
#define CONFIG_DRAM_REFI_MS (64)
#define CONFIG_DRAM_RP_NS (15)
#define CONFIG_DRAM_RCD_NS (15)
#define CONFIG_DRAM_RRD_NS (10)
#define CONFIG_DRAM_FAWG_NS (50)
#define CONFIG_DRAM_RFC_NS (328)
#define CONFIG_DRAM_RAS_NS (45)
#define CONFIG_FLASH_SPI 1

/*
 * Command Configuration
 */
#define CONFIG_CMD_DRAM_TEST 1
#define CONFIG_CMD_DRAM_DIG 1
#define CONFIG_CMD_GDMA_TEST 1
#define CONFIG_CMD_SRAM_TEST 1
#define CONFIG_CMD_IDMEM_TEST 1
#define CONFIG_CMD_CONCURENT_TEST 1
#define CONFIG_CMD_FOREVER_TEST 1
#define CONFIG_CMD_BOOT_BIN 1
#define CONFIG_CMD_BOOT_DSP 1
#undef  CONFIG_CMD_NEXT_FREQ
#undef  CONFIG_CMD_DRAM_AC_TEST
#define CONFIG_SYS_BOOTING_DIALOG 1
#undef  CONFIG_CMD_MEASURE_CPU_CLK
#define CFG_DCACHE_SIZE (32768)
#define CFG_ICACHE_SIZE (32768)
#define CFG_CACHELINE_SIZE (32)
#define CONFIG_SRAM_START 0x02000000
#define CFG_SRAM_SIZE 0x00008000
#define CFG_SRAM_SIZE_SET 0x00000008
#define CONFIG_CONS_INDEX (1)
#define CFG_NS16550 1
#define CFG_NS16550_SERIAL 1
#define CFG_NS16550_REG_SIZE (-4)
#define CFG_NS16550_COM1 0xB8002000
#undef  CONFIG_SERIAL_SOFTWARE_FIFO
#define CFG_SDRAM_BASE (0x80000000)
#define CONFIG_SYS_TEMP_SRAM_ADDR (0x90000000)
#define CONFIG_SYS_TEMP_SRAM_SIZE (0x8000)
#define CONFIG_ENV_OFFSET 0x80000
#define CONFIG_LXBUS_MHZ (200)

/* Function Name: 
 * 	memctlc_dqs_calibration_expansion
 * Descripton:
 *	Expand the over all delay tap spectrum for the input DQ.
 * Input:
 *	dram_type: MEMCTL_DRAM_TYPE_DDRI, MEMCTL_DRAM_TYPE_DDRII, MEMCTL_DRAM_TYPE_DDRIII
 * Output:
 * 	None
 * Return:
 *	None.
 */
#define MEMCTL_BASE 		(0xB8001000)
#define DDR_PHY_CONTR_BASE	(MEMCTL_BASE + (0x500))
#define DACCR			(DDR_PHY_CONTR_BASE + (0x0))
#define DACSPCR			(DDR_PHY_CONTR_BASE + (0x4))
#define DACSPAR			(DDR_PHY_CONTR_BASE + (0x8))
#define DACSPSR			(DDR_PHY_CONTR_BASE + (0xC))
#define DACDQ0RR		(DDR_PHY_CONTR_BASE + (0x10))
#define DACDQ1RR		(DDR_PHY_CONTR_BASE + (0x14))
#define DACDQ2RR		(DDR_PHY_CONTR_BASE + (0x18))
#define DACDQ3RR		(DDR_PHY_CONTR_BASE + (0x1C))
#define DACDQ4RR		(DDR_PHY_CONTR_BASE + (0x20))
#define DACDQ5RR		(DDR_PHY_CONTR_BASE + (0x24))
#define DACDQ6RR		(DDR_PHY_CONTR_BASE + (0x28))
#define DACDQ7RR		(DDR_PHY_CONTR_BASE + (0x2C))
#define DACDQ8RR		(DDR_PHY_CONTR_BASE + (0x30))
#define DACDQ9RR		(DDR_PHY_CONTR_BASE + (0x34))
#define DACDQ10RR		(DDR_PHY_CONTR_BASE + (0x38))
#define DACDQ11RR		(DDR_PHY_CONTR_BASE + (0x3C))
#define DACDQ12RR		(DDR_PHY_CONTR_BASE + (0x40))
#define DACDQ13RR		(DDR_PHY_CONTR_BASE + (0x44))
#define DACDQ14RR		(DDR_PHY_CONTR_BASE + (0x48))
#define DACDQ15RR		(DDR_PHY_CONTR_BASE + (0x4C))
#define DACDQ0FR		(DDR_PHY_CONTR_BASE + (0x50))
#define DACDQ1FR		(DDR_PHY_CONTR_BASE + (0x54))
#define DACDQ2FR		(DDR_PHY_CONTR_BASE + (0x58))
#define DACDQ3FR		(DDR_PHY_CONTR_BASE + (0x5C))
#define DACDQ4FR		(DDR_PHY_CONTR_BASE + (0x60))
#define DACDQ5FR		(DDR_PHY_CONTR_BASE + (0x64))
#define DACDQ6FR		(DDR_PHY_CONTR_BASE + (0x68))
#define DACDQ7FR		(DDR_PHY_CONTR_BASE + (0x6C))
#define DACDQ8FR		(DDR_PHY_CONTR_BASE + (0x70))
#define DACDQ9FR		(DDR_PHY_CONTR_BASE + (0x74))
#define DACDQ10FR		(DDR_PHY_CONTR_BASE + (0x78))
#define DACDQ11FR		(DDR_PHY_CONTR_BASE + (0x7C))
#define DACDQ12FR		(DDR_PHY_CONTR_BASE + (0x80))
#define DACDQ13FR		(DDR_PHY_CONTR_BASE + (0x84))
#define DACDQ14FR		(DDR_PHY_CONTR_BASE + (0x88))
#define DACDQ15FR		(DDR_PHY_CONTR_BASE + (0x8C))
#define DCDQMR			(DDR_PHY_CONTR_BASE + (0x90))
//#define DMCR			(MEMCTL_BASE + (0x1C))
#define DMCR_MRS_BUSY		(0x80000000)

#define MAX_NUM_OF_DQ		(16)
#define DRAM_TEST_ADDR		(0x80000000)
#define TEST_BYTE_SIZE		(0x8000)
#endif


/*
 * Memory configuration
 */
#define CONFIG_DRAM_PREFERED_ZQ_PROGRAM 0x000010ef
#define CONFIG_DRAM_PREFERED_DDRKODL 0x090f08
#undef  CONFIG_DRAM_ODT_VALUE_0
#undef  CONFIG_DRAM_ODT_VALUE_50
#define CONFIG_DRAM_ODT_VALUE_75 1
#undef  CONFIG_DRAM_ODT_VALUE_150
#define CONFIG_PREFERED_DRAM_ODT_VALUE (75)
#undef  CONFIG_DRAM_REDUCE_DRIV_STRENGTH
#define CONFIG_PREFERED_DRAM_DRIV_STRENGTH (1)
#undef  CONFIG_DRAM_AUTO_SIZE_DETECTION
#undef  CONFIG_DRAM_FROCE_DCR_VALUE
#undef  CONFIG_DRAM_BUS_WIDTH_8BIT
#define CONFIG_DRAM_BUS_WIDTH_16BIT 1
#define CONFIG_DRAM_CHIP_NUM_ONE 1
#undef  CONFIG_DRAM_CHIP_NUM_TWO
#undef  CONFIG_ONE_DRAM_CHIP_SIZE_32MB
#define CONFIG_ONE_DRAM_CHIP_SIZE_64MB 1
#undef  CONFIG_ONE_DRAM_CHIP_SIZE_128MB
#undef  CONFIG_ONE_DRAM_CHIP_SIZE_256MB
#undef  CONFIG_ONE_DRAM_CHIP_SIZE_512MB
#define CONFIG_ONE_DRAM_CHIP_SIZE 0x4000000
#undef  CONFIG_ANALOG_DLL_DELAY_LINE
#define CONFIG_ZQ_AUTO_CALI 1
#define CONFIG_AUTO_DRAM_TIMING_SETTING 1
#define CONFIG_DRAM_WR_NS (15)
#define CONFIG_DRAM_RTP_NS (8)
#define CONFIG_DRAM_WTR_NS (8)
#define CONFIG_DRAM_REFI_NS (7800)
#define CONFIG_DRAM_RP_NS (15)
#define CONFIG_DRAM_RCD_NS (15)
#define CONFIG_DRAM_RRD_NS (10)
#define CONFIG_DRAM_FAWG_NS (50)
#define CONFIG_DRAM_RFC_NS (328)
#define CONFIG_DRAM_RAS_NS (45)
#define CONFIG_DRAM_REFI_MS (64)
#define CONFIG_FLASH_SPI 1
#define CONFIG_FLASH_SPI_MAXCLK (20)
#define CONFIG_FLASH_SPI_OP_SIO 1
#undef  CONFIG_FLASH_SPI_OP_DIO
#undef  CONFIG_FLASH_SPI_OP_QIO







