
/*==========================================

===========================================*/



#include <linux/interrupt.h>
#include <asm/system.h>
#include "monitor.h"
#include <asm/mipsregs.h>	//wei add
#define TEST_GPIO_FUNCTION 1
#include "test_lib.h"
#include "test_hw_96d.h"


#define REG32(reg) (*(volatile unsigned int *)(reg))
#define REG16(reg) (*(volatile unsigned short *)(reg))
#define REG8(reg) (*(volatile unsigned char *)(reg))


extern int CmdDumpWord( int argc, char* argv[] );
extern int CmdDumpByte( int argc, char* argv[] ); //wei add
extern int CmdWriteWord( int argc, char* argv[] );

unsigned int cpu_clksel_table[]={ 660, 680, 700, 720, 740, 760, 780, 800,
							    620, 640, 580, 660, 540, 560, 500, 520  };
unsigned int m2x_clksel_table[]={ 312, 387, 362, 462, 425, 250, 475, 337 	};

unsigned int cpu_clkdiv_table[]={1, 2, 2, 4};  //be careful here

int TestGPIO(int argc, char* argv[]);
int TestGPIO_TESTIN(int argc, char* argv[]);
int TestGPIO_INT(int argc, char* argv[]);
#define printf dprintf


int TestStrapPin(int argc, char* argv[]); 
int CmdCPUCLK(int argc, char* argv[]);  //wei add
int CmdMEMCLK(int argc, char* argv[]);  //wei add
int CmdLXCLK(int argc, char* argv[]);  //wei add
int CmdSLEEP(int argc, char* argv[]);  //wei add


int CmdTestSRAM(int argc, char* argv[]);  //wei add
int CmdTestNewPrefetch(int argc, char* argv[]);
int CmdTestDramSize(int argc, char* argv[]);
int CmdTestRS232(int argc, char* argv[]);
int TestBISTAll(int argc, char* argv[]); 
int TestDRF(int argc, char* argv[]); 


int at2_errcnt=0;
int at2_mode=0;
int PCIE_Host_RESET(int argc, char* argv[]); 
int PCIE_Host_Init(int argc, char* argv[]); 
int Test_HostPCIE_DataLoopback(int argc, char* argv[]); 
int  PCIE_PowerDown(int argc, char* argv[]); 
int HostPCIe_MDIORead(int argc, char* argv[]); 
int HostPCIe_MDIOWrite(int argc, char* argv[]); 
int HostPCIe_TestINT(int argc, char* argv[]); 



COMMAND_TABLE	TestCmdTable[] =
{
	{ "?"	  ,0, NULL			, "HELP (?)				    : Print this help message"					},
	{ "Q"   ,0, NULL			, "Q: return to previous menu"					}, 	//wei add			
#if 1
	{ "DW"	  ,2, CmdLib_DumpWord		, "DW <Address> <Len>"},
	{ "DWS"	  ,2, CmdLib_DumpWordSwap		, "DWS <Address> <Len>"},	
	{ "DH"	  ,2, CmdLib_DumpHword		, "DH <Address> <Len>"}, //wei add		
	{ "DB"	  ,2, CmdLib_DumpByte		, "DB <Address> <Len>"}, //wei add	
	
	{ "EW",2, CmdLib_WriteWord, "EW <Address> <Value1> <Value2>..."},
	{ "EWS",2, CmdLib_WriteWordSwap, "EW <Address> <Value1> <Value2>..."},	
	{ "EH",2, CmdLib_WriteHword, "EH <Address> <Value1> <Value2>..."},
	{ "EB",2, CmdLib_WriteByte, "EB <Address> <Value1> <Value2>..."},
	

	{ "REGR"     ,1,  CmdLib_RegRead		, "REGR: Reg Read <addr> [mask]"},	
	{ "REGRS"   ,1,  CmdLib_RegReadSwap	, "REGRS: Reg Read <addr> [mask]"},		
	{ "REGW"     ,1, CmdLib_RegWrite		, "REGW: Reg Write <mask> <val> "},	
	{ "REGWS"   ,1, CmdLib_RegWriteSwap	, "REGWS: Reg Write <mask> <val> "},
#endif


	{ "SLEEP"   ,1, CmdSLEEP			, "sleep "},	
	{ "STRAP"   ,1, TestStrapPin			, "STRAP bit value: "},			
	{ "CPUCLK"   ,1, CmdCPUCLK			, "CPUCLK <clk_sel> <div_value> <sync_oc>: 0-f, 0-1, 0-1 "	}, 	//wei add		
	{ "MEMCLK"   ,1, CmdMEMCLK			, "MEMCLK <clk_sel> :0-7 "	}, 	//wei add		
	{ "LXCLK"   ,1, CmdLXCLK			, "LXCLK <clk_sel> :0-1 "	}, 	//wei add	

	{ "SRAM"   ,1, CmdTestSRAM			, "SRAM <map addr> : test sram"	}, 	//wei add	
	{ "PREF"   ,1, CmdTestNewPrefetch			, "PREF <read addr> <read len> : test prefetch"	}, 	//wei add
	
	{ "SIZE"   ,1, CmdTestDramSize			, "SIZE <size> : test mem write/read>"	}, 	//wei add	

	{ "RX232"   ,1, CmdTestRS232			, "RX232"	}, 	//wei add	

#ifdef TEST_GPIO_FUNCTION	
	{ "GPIO"   ,1, TestGPIO			, "GPIO:Test GPIO "},		
	{ "GPIOTI"   ,1, TestGPIO_TESTIN			, "GPIOTI:Test IN <out port bit> <in port bit> "},	
	{ "GPIOINT"   ,1, TestGPIO_INT			, "GPIOINT:Test GPIO INT "},	
#endif	
	{ "BISTALL"   ,1, TestBISTAll			, "BISTALL:Test BIST ALL "},	
	{ "DRF"   ,1, TestDRF			, "DRF:Test DRF BIST ALL "},	

	
	{ "NULL"   ,0, NULL			, "-------Host PCI-E test -----------"},	
	{ "HRST"   ,1, PCIE_Host_RESET			, "HRST: Host Pcie Reset <portnum> <mdio_rst>: "},
	{ "HINIT"   ,1, PCIE_Host_Init			, "HINIT: Host init bar <portnum>"},	
	{ "HLOOP"   ,1, Test_HostPCIE_DataLoopback			, "HLOOP: Test Pci-E data loopback <portnum> <cnt> "},	
	{ "EPDN"   ,1, PCIE_PowerDown			, "EPDN: PCIE Power Down test <portnum><mode> "},			
	{ "EMDIOR"   ,1, HostPCIe_MDIORead			, "EMDIOR: Reg Read <portnum>"},	
	{ "EMDIOW"   ,1, HostPCIe_MDIOWrite			, "EMDIOW <portnum> <reg> <val>:  "},	
	{ "EINT"   ,1, HostPCIe_TestINT			, "EINT <portnum> <loops>:  "},	

};


//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
void 	TestMonitorEntry()
{
	int i;
	//REG32(CLKMANAGE)=ACTIVE_DEFAULT; //enable all interface
	//DBG_PRINT("Enable all interface\r\n");
	#define TEST_PROMPT		"<RealTek/96D>"
	RunMonitor(TEST_PROMPT, TestCmdTable, sizeof(TestCmdTable) / sizeof(COMMAND_TABLE) );
}
//---------------------------------------------------------------------------------------


//-----------------------------------------------------------------
void ShowStrapMsg()
{
	const unsigned char *boot_type_tab[]={ {"SPI"}, {"NOR"}, {"NFBI"}, {"NAND"}, {"ROM1"}, {"ROM2"}, {"ROM3"}, {"ROM4"} };
	const unsigned char *dram_type_tab[]={ {"SDR"} , {"SDR"} , {"DDR2"}, {"DDR1"} };

	#define GET_BITVAL(v,bitpos,pat) ((v& (pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf	

	
	unsigned int v=REG32(SYS_HW_STRAP);

	unsigned int bootsel=GET_BITVAL(v, 0, RANG3);
	unsigned int dramtype=GET_BITVAL(v, 3, RANG2);
/*	
	unsigned int phyidsel=
	unsigned int dis_olt_autotest=		
	unsigned int clklx_from_clkm=
	unsigned int phyif=	
	unsigned int sync_oc=	
*/	
	unsigned int ck_m2x_freq_sel=GET_BITVAL(v, 10, RANG3);
	unsigned int ck_cpu_freq_sel=GET_BITVAL(v, 13, RANG4);
/*	
	unsigned int nrfrst_type=
*/	
	unsigned int ck_cpu_div_sel=GET_BITVAL(v, 18, RANG2);
/*	
	unsigned int ever_reboot_once=
	unsigned int sys_debug_sel=
	unsigned int bus_dbgsel=
*/	
	
	printf("---------------------\n");
	printf("HW_STRAP_VAL= 0x%08x \n", v);
	printf("[02:00] ST_BOOTPINSEL= 0x%x  \n", bootsel);		
	printf("[04:03] ST_DRAMTYPE= 0x%x      	\n", 	dramtype);	
/*	
	printf("[04:03] ST_PHYID= 0x%x \n", 		phyidsel);		
	
	printf("[05:05] ST_OLT_AUTO_TEST= 0x%x \n", dis_olt_autotest);	
	printf("[06:06] ST_DRAMTYPE0= 0x%x      	\n", 	dram_type0);	
	printf("[07:07] ST_CLKLX_FROM_CLKM= 0x%x \n", clklx_from_clkm);	
	printf("[08:08] ST_PHYIF= 0x%x \n", phyif);

	printf("[09:09] ST_SYNC_OCP= 0x%x \n", sync_oc );	
*/	
	printf("[12:10] CK_M2X_FREQ_SEL= 0x%x \n", ck_m2x_freq_sel);	
	printf("[16:13] ST_CPU_FREQ_SEL= 0x%x \n", ck_cpu_freq_sel);
/*	printf("[17:17] ST_NRFRST_TYPE= 0x%x \n", nrfrst_type);	
*/	
	printf("[19:18] ST_CPU_FREQDIV_SEL= 0x%x \n", ck_cpu_div_sel);
/*	printf("[23:23] ST_EVER_REBOOT_ONCE= 0x%x \n", ever_reboot_once);	
	printf("[29:24] ST_SYS_DBG_SEL= 0x%x \n", sys_debug_sel);	
	printf("[31:30] ST_PINBUS_DBG_SEL= 0x%x \n", bus_dbgsel);	
*/	

	printf("\n");
	printf("%s mode, %s Ram,  CPU=%d MHz, Mem2x=%d MHz, \n", 
						boot_type_tab[bootsel],	
						dram_type_tab[dramtype],
						cpu_clksel_table[ck_cpu_freq_sel]/cpu_clkdiv_table[ck_cpu_div_sel] ,
						m2x_clksel_table[ck_m2x_freq_sel]
						);
					
}
//------------------------------------------------------------------------
//---------------------------------------------------------------------------------

int TestStrapPin(int argc, char* argv[])
{

	if(argc < 2) 
	{	ShowStrapMsg();
		dprintf("Usage: strap bit value \n");	
		return 1;
	}
	unsigned int bit = strtoul((const char*)(argv[0]), (char **)NULL, 10);		
	unsigned int val = strtoul((const char*)(argv[1]), (char **)NULL, 16);		


	if(val==0)
	{		
		REG32(SYS_HW_STRAP) &= ~(1<<bit);		
	}
	else if(val==1)
	{			
		REG32(SYS_HW_STRAP) |= (1<<bit);		
	}

	ShowStrapMsg();
	
}
//---------------------------------------------------------------------------------

//=====================================================================

//=====================================================================


static void SPEED_isr(void)
{
	unsigned int isr=REG32(GISR_REG);
	unsigned int cpu_status=REG32(SYS_INT_STATUS);
	
	dprintf("=>CPU Wake-up interrupt happen! GISR=%08x \n", isr);

	if( (isr & (1<<SPEED_IRQ_NO))==0)   //check isr==1
	{	dprintf("Fail, ISR=%x bit %d is not 1\n", isr, SPEED_IRQ_NO);
		while(1) ;
	}

	if((cpu_status & (1<<1))==0)  //check source==1
	{	dprintf("Fail, Source=%x bit %d is not 1 \n", cpu_status, 1);
		while(1) ;
	}
		
	REG32(SYS_INT_STATUS)=(1<<1);  //enable cpu wakeup interrupt mask
//	REG32(GISR_REG)=1<<SPEED_IRQ_NO;	//write to clear, but cannot clear


	REG32(GIMR_REG)= REG32(GIMR_REG) & ~(1<<SPEED_IRQ_NO);	//so, disable interrupt		
}

struct irqaction irq_SPEED = {SPEED_isr, (unsigned long)NULL, (unsigned long)SPEED_IRQ_NO,"SPEED", (void *)NULL, (struct irqaction *)NULL};   

//---------------------------------------------------------------------------
int LetCPUDoSomething()
{
	int i,sum=0;
	unsigned int buf[256];
	unsigned int *p;
	
      for(i=0; i<=100; i++)
	  	sum+=i;
      if(sum != 5050)
      	{
		dprintf("FAIL! ,summation 1 to 100=%d \n",sum);
		while(1) {};
	  }

	p=(unsigned int)buf|0xa0000000;
	for(i=0; i<256; i++)
		p[i]=i;

	for(i=0; i<256;i++)
		if(p[i]!=i)
		{	dprintf("FAIL! idx=%x val=%x\n",i,p[i]);
			while(1) {};
		}

}

//------------------------------------------------------------------------------
int SettingCPUClk(int clk_sel, int clk_div, int sync_oc)
{
	int clk_curr, clk_exp;	
	unsigned int old_clk_sel;
	unsigned int mask;
	unsigned int sysreg;



	dprintf("\nInput : CLK_SEL=0x%x, DIV=0x%x, SYNC_OC=0x%x \n", clk_sel, clk_div, sync_oc);
	//dprintf("Want to chage to CPU clock %d\r\n",clk_curr/clk_div);

	
	clk_curr = check_cpu_speed();
	dprintf("Now CPU Speed=%d \n",clk_curr);	
	//----------------------------
	REG32(SYS_INT_STATUS)=(1<<1);  //enable cpu wakeup interrupt mask
	while(REG32(GISR_REG)&(1<<SPEED_IRQ_NO));  //wait speed bit to low.
	//-------------------------------
	

	mask=REG32(GIMR_REG);
	//open speed irq

	
	int irraddr=IRR_REG+SPEED_IRR_NO*4;		
	REG32(irraddr) = (REG32(irraddr) &~(0x0f<<SPEED_IRR_OFFSET)) | (3<<SPEED_IRR_OFFSET);		
	request_IRQ(SPEED_IRQ_NO, &irq_SPEED, NULL); 	

	//be seure open interrupt first.
	REG32(GIMR_REG)=(1<<SPEED_IRQ_NO);  //accept speed interrupt	
	//REG32(GIMR_REG)=(1<<NFBI_IRQ_NO) ;  //only accept NFBI to interrupt	
	//REG32(GIMR_REG)=(1<<NFBI_IRQ_NO) | (1<<SPEED_IRQ_NO);  //accept speed and NFBI to interrupt		

	



	//-------------
	sysreg=REG32(SYS_HW_STRAP);
	//dprintf("Read  SYS_HW_STRAP=%08x\r\n", sysreg);	
	old_clk_sel=(sysreg & ST_CPU_FREQ_SEL) >>ST_CPU_FREQ_SEL_OFFSET;

	sysreg&= ~(ST_FW_CPU_FREQDIV_SEL);
	sysreg&= ~(ST_CK_CPU_FREQDIV_SEL);	
	sysreg&= ~(ST_CPU_FREQ_SEL);
	//sysreg&= ~(ST_SYNC_OCP);

	sysreg|=  (clk_div & 0x03) <<ST_CPU_FREQDIV_SEL_OFFSET ;
	sysreg|=	 (clk_sel&0x0f)<<ST_CPU_FREQ_SEL_OFFSET ;
	//sysreg|=  (sync_oc&01)<<ST_SYNC_OCP_OFFSET;
	
	dprintf("Write SYS_HW_STRAP=%08x \n", sysreg);
	REG32(SYS_HW_STRAP)=sysreg  ;
	//dprintf("Read  SYS_HW_STRAP=%08x \n", REG32(SYS_HW_STRAP));
	
	//--------------
	if(old_clk_sel != clk_sel)
	{


	
		REG32(GISR_REG)=0xffffffff;	
		//dprintf("before sleep, Read  SYS_HW_STRAP=%08x \n", REG32(SYS_HW_STRAP));	
		//dprintf("GISR=%08x \n",REG32(GISR_REG));
		//dprintf("GIMR=%08x \n",REG32(GIMR_REG));	

		#if 1			
			REG32(SYS_BIST_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_BIST_DONE)&(1<<0))==0)  ; //wait bit to 1, is mean lock ok	

			REG32(SYS_BIST_CTRL) |= (1<<3) ;	  //lock bus arb4
			while( (REG32(SYS_BIST_DONE)&(1<<1))==0)  ; //wait bit to 1, is mean lock ok		

			REG32(SYS_BIST_CTRL) |= (1<<4) ;	  //lock bus arb6
			while( (REG32(SYS_BIST_DONE)&(1<<2))==0)  ; //wait bit to 1, is mean lock ok				
		#endif
		
		__asm__ volatile("sleep");	
		__asm__ volatile("nop");


		#if 1
			REG32(SYS_BIST_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<0))==(1<<0)) ;  //wait bit to 0  unlock

			REG32(SYS_BIST_CTRL) &= ~(1<<3);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<1))==(1<<1)) ;  //wait bit to 0  unlock

			REG32(SYS_BIST_CTRL) &= ~(1<<4);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<2))==(1<<2)) ;  //wait bit to 0  unlock				
		#endif

		//dprintf("GISR=%08x\r\n",REG32(GISR_REG));
		//dprintf("GIMR=%08x\r\n",REG32(GIMR_REG));		

		
	
		//dprintf("after  sleep, Read  SYS_HW_STRAP=%08x  \n", REG32(SYS_HW_STRAP));
		int strap_new=REG32(SYS_HW_STRAP) ;
		//int strap_new2=REG32(SYS_HW_STRAP) ;
		//int strap_new3=REG32(SYS_HW_STRAP) ;		
		//if(sysreg != strap_new )
		//	dprintf("FAIL ! strap write value =%x, read value=%x, are not the same ! \n", sysreg, strap_new );

		//dprintf("%x %x %x \n", strap_new, strap_new2,strap_new3);
	}
	//REG32(SYS_HW_STRAP)=sysreg  ;  //because wake up will miss div reg
	

	//-----------------------
		REG32(GIMR_REG)=mask;

	
	//-----------------------
	//test cpu can work
	LetCPUDoSomething();

	//-----------------------
	clk_curr = check_cpu_speed();
	dprintf("After  Changing, CPU Speed=%d \n",clk_curr);	
	//dprintf("GISR=%08x\r\n",REG32(GISR_REG));

	clk_exp=cpu_clksel_table[clk_sel] / cpu_clkdiv_table[clk_div];
	if( (clk_curr >=  clk_exp-1) && (clk_curr <=  clk_exp+1) )  //torrernce 1 MHz
		dprintf("Test PASS!\n");
	else 
	{	dprintf("Test FAIL! Curr_speed=%d but Exp_speed=%d \n", clk_curr, clk_exp);
		//while(1) ;
	}

}
//---------------------------------------------------------------------------
int CmdCPUCLK(int argc, char* argv[])
{

	int clk_sel=0, clk_div=0, sync_oc=0;
	int clk_curr;	


	
	int i;
	if( !argv[0])	//read
	{
		clk_curr = check_cpu_speed();
		dprintf("Now CPU Speed=%d \n",clk_curr);			
		ShowStrapMsg();	
		dprintf("Usage: CPUCLK clk_sel div_value sync_oc: 0-f, 4-0, 0-1  \n");	
		dprintf("Usage: CPUCLK 999 999: test all freq  \n");		


		int i;
		for(i=0; i<16;i++)
		{ dprintf(" %x : %d MHz,  ", i, cpu_clksel_table[i]);
		  if( (i&0x3) ==0x3) dprintf("\n");
		}
		return;	
	}


	
	if(argv[0])	clk_sel = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	if(argv[1])	clk_div = strtoul((const char*)(argv[1]), (char **)NULL, 16);
	if(argv[2])	sync_oc = strtoul((const char*)(argv[2]), (char **)NULL, 16);


	if(clk_sel==0x999)
	{
		if(clk_div==0x999)
		{
			for(clk_sel=0; clk_sel<=0x0f; clk_sel++)
				SettingCPUClk(clk_sel, 4, 0);

			for(clk_sel=0; clk_sel<=0x0f; clk_sel++)
				SettingCPUClk(clk_sel, 2, 0);
			
			for(clk_sel=0; clk_sel<=0x0f; clk_sel++)
				SettingCPUClk(clk_sel, 0, 0);
		}
		else
		{
			for(clk_sel=0; clk_sel<=0x0f; clk_sel++)
				SettingCPUClk(clk_sel, clk_div, sync_oc);
		}

			
	}
	else
		SettingCPUClk(clk_sel, clk_div, sync_oc);

}
extern unsigned long glexra_clock;

//---------------------------------------------------------------------------

int SettingM2xClk(int clk_sel)
{
	int	tmp=REG32(SYS_HW_STRAP) & ~(CK_M2X_FREQ_SEL);
	
		#if 1  //lock bus			
			REG32(SYS_BIST_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_BIST_DONE)&(1<<0))==0)  ; //wait bit to 1, is mean lock ok	

			REG32(SYS_BIST_CTRL) |= (1<<3) ;	  //lock bus arb4
			while( (REG32(SYS_BIST_DONE)&(1<<1))==0)  ; //wait bit to 1, is mean lock ok		

			REG32(SYS_BIST_CTRL) |= (1<<4) ;	  //lock bus arb6
			while( (REG32(SYS_BIST_DONE)&(1<<2))==0)  ; //wait bit to 1, is mean lock ok		

			//add check transaction dram empty .
		#endif

	
	REG32(SYS_HW_STRAP)= tmp | (clk_sel) <<CK_M2X_FREQ_SEL_OFFSET ;
		
		#if 1   //check m2xusable and unlock bus
			while( (REG32(SYS_BIST_DONE)&(1<<18))==0)  ;   //wait to 1, mean m2x is usable
	
			REG32(SYS_BIST_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<0))==(1<<0)) ;  //wait bit to 0  unlock

			REG32(SYS_BIST_CTRL) &= ~(1<<3);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<1))==(1<<1)) ;  //wait bit to 0  unlock

			REG32(SYS_BIST_CTRL) &= ~(1<<4);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<2))==(1<<2)) ;  //wait bit to 0  unlock	
		#endif

	int	clklx_from_clkm=GET_BITVAL(REG32(SYS_HW_STRAP) ,ST_CLKLX_FROM_CLKM_OFFSET, BIT_RANG1);
	if(clklx_from_clkm==1)
	{
		console_init(glexra_clock);
		timer_init(glexra_clock);
	}
		
	dprintf("Change M2x clock freq=%d \n", m2x_clksel_table[clk_sel] );
}
//---------------------------------------------------------------------------
int CmdMEMCLK(int argc, char* argv[])
{

	int clk_sel=0;
	int tmp;
	if( argc < 1 ) 
	{
		dprintf("Usage: MEMCLK <sel>: 0-7  \n");		
		int i;
		for(i=0; i<8;i++)
		dprintf(" %d : %d MHz \n", i, m2x_clksel_table[i]);
		
		int	m2xsel=(REG32(SYS_HW_STRAP) & (CK_M2X_FREQ_SEL))>>10;
		dprintf("status: %d = %d Mhz \n",  m2xsel, m2x_clksel_table[m2xsel] );
		return;	
	}
	

	clk_sel = strtoul((const char*)(argv[0]), (char **)NULL, 16);
		SettingM2xClk(clk_sel);


}
//---------------------------------------------------------------------------

int SettingLxClk(int clklx_from_clkm)
{	
	
	int	tmp=REG32(SYS_HW_STRAP) & ~(ST_CLKLX_FROM_CLKM);
	int strap_newval= tmp | ((clklx_from_clkm&0x01) <<ST_CLKLX_FROM_CLKM_OFFSET) ;

	//--------------------------------------
	//int lexra_newval;

	if(clklx_from_clkm==0)
	{	
		glexra_clock=200*1000*1000;
	}
	else
	{
		//prepare uart 
		unsigned long mem2x_clksel_table[]={ 131250000, 156250000, 300000000, 250000000,  312500000, 337500000, 475000000, 387500000 	};
		int	m2xsel=(REG32(SYS_HW_STRAP) & (CK_M2X_FREQ_SEL))>>CK_M2X_FREQ_SEL_OFFSET;
	       glexra_clock=mem2x_clksel_table[m2xsel] /2;

	}
	//--------------------------------------
	int gimr_tmp=REG32(GIMR_REG);
	REG32(GIMR_REG)=0;
	
		#if 1  //lock bus			
			REG32(SYS_BIST_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_BIST_DONE)&(1<<0))==0)  ; //wait bit to 1, is mean lock ok	

			REG32(SYS_BIST_CTRL) |= (1<<3) ;	  //lock bus arb4
			while( (REG32(SYS_BIST_DONE)&(1<<1))==0)  ; //wait bit to 1, is mean lock ok		

			REG32(SYS_BIST_CTRL) |= (1<<4) ;	  //lock bus arb6
			while( (REG32(SYS_BIST_DONE)&(1<<2))==0)  ; //wait bit to 1, is mean lock ok	

			//add check transaction dram empty .
		#endif
	


	//go
	REG32(SYS_HW_STRAP)=strap_newval;    //change lx clk


		#if 1   //check m2xusable and unlock bus
			//while( (REG32(SYS_BIST_DONE)&(1<<18))==0)  ;   //wait to 1, mean m2x is usable
	
			REG32(SYS_BIST_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<0))==(1<<0)) ;  //wait bit to 0  unlock

			REG32(SYS_BIST_CTRL) &= ~(1<<3);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<1))==(1<<1)) ;  //wait bit to 0  unlock

			REG32(SYS_BIST_CTRL) &= ~(1<<4);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<2))==(1<<2)) ;  //wait bit to 0  unlock	
		#endif

		
	console_init(glexra_clock);
	timer_init(glexra_clock);

		
	REG32(GIMR_REG)=gimr_tmp;
	//--------------------------------------	



	

	dprintf("clklx_from_clkm=%d \n", clklx_from_clkm );
	
}

int CmdLXCLK(int argc, char* argv[])
{
	if( argc < 1 ) 
	{
		dprintf("Usage: LXCLK <sel>: 0-1  \r\n");		
		int	lxsel=(REG32(SYS_HW_STRAP) & (ST_CLKLX_FROM_CLKM))>>ST_CLKLX_FROM_CLKM_OFFSET;
		dprintf("status: %d \n", lxsel );		
		return;	
	}

	
	int clklx_from_clkm = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	SettingLxClk(clklx_from_clkm);


	//--------------------------------------


};  
//---------------------------------------------------------------------------
int CmdSLEEP(int argc, char* argv[])
{
	unsigned int mask;

	dprintf("Now CPU Speed=%d\r\n",check_cpu_speed() );
	mask=REG32(GIMR_REG);
	
	
	dprintf("GISR=%08x\r\n",REG32(GISR_REG));			
	
//	cli();
//	REG32(GIMR_REG)=0;
	__asm__ volatile("sleep");	
/*
__asm__ __volatile__ (
		".set\tnoreorder\n"	
		".text \n"
		".word 0x42000038,0,0,0,0 \n"
		".set\treorder"		
		);
*/
	
	__asm__ volatile("nop");	
	dprintf("GISR=%08x\r\n",REG32(GISR_REG));	
	REG32(GIMR_REG)=mask;


	dprintf("Now CPU Speed=%d\r\n",check_cpu_speed() );	
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------

//------------------------------------------------------------------------
void Enable_SRAM(unsigned int paddr)
{
	//unmap
	REG32(0xb8001304)=0x5;   //unmap seg len=4K
	REG32(0xb8001300)=(unsigned int)paddr |1;   //unmap seg addr

	//enable sram
	REG32(0xb8004004)=0x05;   //sram seg len=4K		
	REG32(0xb8004000)=(unsigned int)paddr |1;   //sram seg addr
}

void Disable_SRAM()
{
	//unmap
	REG32(0xb8001300)=0;   //unmap seg addr
	//enable sram
	REG32(0xb8004000)=0;   //sram seg addr
}


int CmdTestSRAM(int argc, char* argv[])
{
	#define VIR2PHY(x) ((unsigned int)x&0x1fffffff)
	#define VIR2NOC(x) ((unsigned int)x|0xa0000000)
	#define REG32(reg)	(*(volatile unsigned int *)(reg))

	unsigned char *addr;
	unsigned int len=(1<<12);  //SRAM size=4K
	unsigned int i;
	
	if( argc < 1 ) 
	{
		dprintf("Usage: sram <map addr> \n");		
		return;	
	}
	addr = strtoul((const char*)(argv[0]), (char **)NULL, 16);	

	unsigned char *paddr=VIR2PHY(addr);
	unsigned char  *n8addr=VIR2NOC(addr);
	unsigned short *n16addr=VIR2NOC(addr);		
	unsigned int     *n32addr=VIR2NOC(addr);	
	
	//===============================================
	//stage 1: r/w
	memset(n8addr, 0xff, len);
#if 1
	Enable_SRAM(paddr);
#endif
	//-----------------------------------------------------
	//test fix pattern
	unsigned int patt_array[]={ 0x00000000, 0x55555555, 0xaaaaaaaa, 0xffffffff, 0x1234567, 0xa5a55a5a  };
	unsigned int patt_idx=0; 

	#define ACC8 0
	#define ACC16 1
	#define ACC32 2
	#define ACCEND 3
	unsigned int accmode;
	
	for(accmode=ACC8; accmode<ACCEND; accmode++)  // 8 bit, 16bit, 32bit
	{
	
	for(patt_idx=0; patt_idx< sizeof(patt_array)/sizeof(patt_array[0]) ; patt_idx++)
	{
		if(accmode==ACC8)  //pattern fix value, 8 bit access
		{			
			unsigned char patt=patt_array[patt_idx];
			printf("Test Patt %02x, byte w/r \n", patt);	
			memset(n8addr, patt, len);	
			for(i=0; i<len; i++)
			{
				if(REG8(n8addr+i) != patt)
					printf("%x err, exp=%x, read=%x\n", n8addr+i, patt, REG8(n8addr+i));
			}
		}
		else if(accmode==ACC16)			//pattern fix value, 16 bit access
		{
			unsigned short patt=patt_array[patt_idx];
			printf("Test Patt %04x, short w/r \n", patt);	

			for(i=0; i<len; i+=2)			
				REG16(n8addr+i)= patt;	
			
			for(i=0; i<len; i+=2)
			{
				if(REG16(n8addr+i) != patt)
					printf("%x err, exp=%x, read=%x\n", n8addr+i, patt, REG16(n8addr+i));
			}
		}
		else if(accmode==ACC32)   //pattern fix value, 32 bit access
		{			
			unsigned int patt=patt_array[patt_idx];
			printf("Test Patt %08x, int w/r \n", patt);	

			for(i=0; i<len; i+=4)			
				REG32(n8addr+i)= patt;	

			for(i=0; i<len; i+=4)
			{
				if(REG32(n8addr+i) != patt)
					printf("%x err, exp=%x, read=%x\n", n8addr+i, patt, REG32(n8addr+i));
			}
		}		
	}
	}
	//-----------------------------------------------------	
	//test inc patt
	for(accmode=ACC8; accmode<ACCEND; accmode++)  // 8 bit, 16bit, 32bit
	{
		if(accmode==ACC8)  //pattern fix value, 8 bit access
		{			
			printf("Test inc Patt, byte w/r \n");	
			for(i=0; i<len; i++)		
				REG8(n8addr+i)=i;
			
			for(i=0; i<len; i++)
			{
				if(REG8(n8addr+i) != (i&0xff) )
					printf("%x err, exp=%x, read=%x\n", n8addr+i, (i&0xff), REG8(n8addr+i));
			}
		}
		else if(accmode==ACC16)			//pattern fix value, 16 bit access
		{
			printf("Test inc Patt, short w/r \n");	

			for(i=0; i<len/2; i++)			
				REG16(n16addr+i)= i;	
			
			for(i=0; i<len/2; i++)
			{
				if(REG16(n16addr+i) != (i&0xffff) )
					printf("%x err, exp=%x, read=%x\n", n16addr+i, (i&0xffff), REG16(n16addr+i));
			}
		}
		else if(accmode==ACC32)   //pattern fix value, 32 bit access
		{			
			printf("Test inc Patt, int w/r \n");	

			for(i=0; i<len/4; i++)			
				REG32(n32addr+i)= i;	

			for(i=0; i<len/4; i++)
			{
				if(REG32(n32addr+i) != i)
					printf("%x err, exp=%x, read=%x\n", n32addr+i, i, REG32(n32addr+i));
			}
		}		
	}
	//-----------------------------------------------------	
	//pattern rand, byte access
	printf("Test Pattern rand, byte w/r \n");
	
	srand(0x55aa);
	for(i=0; i<len; i++)		
	{	unsigned int data;
		data=random(0xff);
		n8addr[i]=data;
		if(n8addr[i] !=data)
			printf("%x err, exp=%x, read=%x\n", n8addr+i, data, n8addr[i]);
	}
	//-----------------------------------------------------
	
	//pattern rand, 4 byte access
	printf("Test Pattern rand, 4 bytes w/r \n");
	
	srand(0xaa55);
	for(i=0; i<len; i+=4)		
	{	unsigned int data;
		data=rand2();
		n32addr[i]=data;
		if(n32addr[i] !=data)
			printf("%x err, exp=%x, read=%x\n", n8addr+i, data, n8addr[i]);
	}

	//-----------------------------------------------------
	memset(n8addr,0xaa,len);
	Disable_SRAM();


	//===========================================
	//stage 2: test on/off
	printf("Test Pattern 0x55 in DRAM, 0xaa in SRAM \n");
	
	memset(n8addr,0x55,len);
	
#if 1
	printf("Enable SRAM, do compare\n");
	Enable_SRAM(paddr);
#endif

	
	//compare the original sdram
	for(i=0; i<len; i++)
	{
		if(n8addr[i] != 0xaa)
			printf("%x err, exp=%x, read=%x\n", n8addr+i, 0xaa, n8addr[i]);
	}
	
	printf("Disable SRAM, do compare\n");
	Disable_SRAM();
	
	for(i=0; i<len; i++)
	{
		if(n8addr[i] != 0x55)
			printf("%x err, exp=%x, read=%x\n", n8addr+i, 0x55, n8addr[i]);
	}

	//-----------------------------------------------------

	
	printf("No error is pass\n");
}

//------------------------------------------------------------------------
int CmdTestNewPrefetch(int argc, char* argv[])
{

	if( argc < 1 ) 
	{
		dprintf("Usage: PREF <addr> <loop read count> \n");		
		return;	
	}
	unsigned int addr = strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	unsigned int cnt = strtoul((const char*)(argv[1]), (char **)NULL, 16);	



	
	unsigned int time_start,time_end,time_delta;
	unsigned int i,tmp,lop,mode;
	unsigned int t0=0, t1=0, t2=0;
	
	unsigned int len=(1<<20);  // 1M

  for(mode=0; mode<3; mode++)
  {
	#define MCR 0xb8001000
//	if(mode==0)
	{
		REG32(MCR) &= ~(1<<28);  //disable
		REG32(MCR) &= ~(1<<29);  //disable

	}	
	if(mode==1)
	{
		REG32(MCR) &= ~(1<<26);  //old
		REG32(MCR) &= ~(1<<27);  //old
		
		REG32(MCR) |= (1<<28);  //en
		REG32(MCR) |= (1<<29);  //en


	}	
	else if(mode==2)
	{
		REG32(MCR) |= (1<<26);  //new
		REG32(MCR) |= (1<<27);  //new
		
		REG32(MCR) |= (1<<28);  //en
		REG32(MCR) |= (1<<29);  //en		

	}		
	//st
	time_start=get_timer_jiffies();


	//testing...
	for(lop=0;lop<cnt;lop++)
	for(i=0;i<len; i+=4)
	{	tmp=REG32(addr+i);
	}

	//et
	time_end=get_timer_jiffies();	


	time_delta=time_end-time_start;
	
	//printf("t1=%d ms\n", time_start*10);  //return in ms
	//printf("t2=%d ms\n", time_end*10);  
	printf("mode=%d, diff=%d ms\n", mode, (time_end-time_start)*10);  

	//save
	if(mode==0)  t0=time_delta;
	else if(mode==1) t1=time_delta;
	else if(mode==2) t2=time_delta;

    }	

	if( (t0 > t1) && (t1 >  t2))   //t2 is more fast
		printf("=> Test Result: PASS\n");  
	else
		printf("=> Test Result: FAIL\n");	
  
}

//===========================================================================
int CmdTestDramSize(int argc, char* argv[])
{
	if( argc < 1 ) 
	{
		dprintf("Usage: size <size> \n");		
		return;	
	}
	#define VIR2CAC(x) ((unsigned int)x|0x80000000)
	#define VIR2NOC(x) ((unsigned int)x|0xa0000000)
	
	unsigned int size = strtoul((const char*)(argv[0]), (char **)NULL, 16);	

	unsigned int *st=VIR2CAC(8<<20);  //start from 8M
	unsigned int *ed=VIR2CAC(size<<20);  //start from 8M	
	unsigned int *p;
	
	for(p=st; p<ed; p++)
		REG32(p)=p;
	
	for(p=st; p<ed; p++)
	{	if(REG32(p)!=p)
			printf("Addr=%x error\n", p);
			break;
	}
	

}

//===========================================================================
int CmdTestRS232(int argc, char* argv[])
{
	#define GetChar()	 (serial_inc()&0xff)
	int i;

	for(i=0; i<0x40; i++)
	{
		dprintf("%02x ", GetChar());
	}
}
//===========================================================================

#ifdef TEST_GPIO_FUNCTION
#define GPIO_PA_OFFSET 0
#define GPIO_PB_OFFSET 8
#define GPIO_PC_OFFSET 16
#define GPIO_PD_OFFSET 24
#define GPIO_PE_OFFSET 0
#define GPIO_PF_OFFSET 8
#define GPIO_PG_OFFSET 16
#define GPIO_PH_OFFSET 24

#define GPIO_PA_MASK 0x000000ff
#define GPIO_PB_MASK 0x0000ff00
#define GPIO_PC_MASK 0x00ff0000
#define GPIO_PD_MASK 0xff000000
#define GPIO_PE_MASK 0x000000ff
#define GPIO_PF_MASK 0x0000ff00
#define GPIO_PG_MASK 0x00ff0000
#define GPIO_PH_MASK 0xff000000




#define GPIO_PORT_MAXNUM 8
unsigned char GPIO_PORT[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h' };
unsigned int GPIO_OFFSET[]={GPIO_PA_OFFSET,  GPIO_PB_OFFSET, GPIO_PC_OFFSET, GPIO_PD_OFFSET,
							GPIO_PE_OFFSET, GPIO_PF_OFFSET, GPIO_PG_OFFSET, GPIO_PH_OFFSET   };
unsigned int GPIO_MASK[]={ GPIO_PA_MASK, GPIO_PB_MASK, GPIO_PC_MASK, GPIO_PD_MASK,
							GPIO_PE_MASK, GPIO_PF_MASK, GPIO_PG_MASK, GPIO_PH_MASK	};
unsigned int GPIO_CNR[]={ PABCDCNR_REG, PABCDCNR_REG, PABCDCNR_REG, PABCDCNR_REG,
						PEFGHCNR_REG, PEFGHCNR_REG, PEFGHCNR_REG, PEFGHCNR_REG	};
unsigned int GPIO_DIR[]={PABCDDIR_REG, PABCDDIR_REG, PABCDDIR_REG, PABCDDIR_REG,
						PEFGHDIR_REG, PEFGHDIR_REG, PEFGHDIR_REG, PEFGHDIR_REG		};
unsigned int GPIO_DATA[]={ PABCDDAT_REG, PABCDDAT_REG, PABCDDAT_REG, PABCDDAT_REG,
							PEFGHDAT_REG, PEFGHDAT_REG, PEFGHDAT_REG, PEFGHDAT_REG};

unsigned int GPIO_IMR[]={ PABIMR_REG, PABIMR_REG, PCDIMR_REG, PCDIMR_REG, 
						PEFIMR_REG, PEFIMR_REG, PGHIMR_REG, PGHIMR_REG };

unsigned int GPIO_IMR_OFFSET[]={ 0, 16, 0, 16, 
								0, 16, 0, 16 };

unsigned int GPIO_ISR[]={ PABCDISR_REG, PABCDISR_REG, PABCDISR_REG, PABCDISR_REG, 
						PEFGHISR_REG, PEFGHISR_REG, PEFGHISR_REG, PEFGHISR_REG };

unsigned int GPIO_ISR_OFFSET[]={ 0, 8, 16, 24, 
								0, 8, 16, 24 };
//------------------------------------------------------------------------
unsigned int GPIO_GetPortNum_byPortName(unsigned char portname)
{
	unsigned int port=0;
	for(port=0; port<GPIO_PORT_MAXNUM; port++)
	{	if(GPIO_PORT[port]==portname)
			break;
	}
	if(port==GPIO_PORT_MAXNUM)
	{	dprintf("Port not support! \n"); 				
	}
	return port;
}
//------------------------------------------------------------------------
void ChangePinMux_toGPIOMode(unsigned int portnum, unsigned int bit)
{
	#define SPRS SYS_PIN_MUX_SEL // Share Pin Register
	#define SPRS2 SYS_PIN_MUX_SEL2 
	
	switch(GPIO_PORT[portnum])
	{
		case 'a': 
				if( bit==0)  		REG32(SPRS) |=  (0x3<<16);   
				else if(bit==1) 	REG32(SPRS) |=  (0x3<<12);   
				else if( (bit>=2) && (bit<=6) ) 	REG32(SPRS) |=   (0x6<<0);   
				//a7-b0 uart
				else if(bit ==7)	REG32(SPRS) |=  (0x1 <<5) ;
				break;
		case 'b':
				
				if(bit==0)		REG32(SPRS) |=  (0x1<<5);
				else if(bit==1) 	REG32(SPRS) |=  (0x1<<6);   
				else if(bit ==2)	REG32(SPRS2) |=  (0x3 <<0) ;
				else if(bit ==3)	REG32(SPRS2) |=  (0x3 <<3) ;	
				else if(bit ==4)	REG32(SPRS2) |=  (0x3 <<6) ;	
				else if(bit ==5)	REG32(SPRS2) |=  (0x3 <<9) ;	
				else if(bit ==6)	REG32(SPRS2) |=  (0x3 <<12) ;	
				else if(bit ==7)	REG32(SPRS2) |=  (0x4 <<15) ;				
				break;
		case 'c':
				
				if(bit==0)		REG32(SPRS2) |=  (0x4<<18);
				else if(bit ==1)	REG32(SPRS) |=  (0x3 <<20) ;	
				else if(bit ==2)	REG32(SPRS) |=  (0x3 <<22) ;	
				else if(bit ==3)	REG32(SPRS) |=  (0x3 <<24) ;	
				else if(bit ==4)	REG32(SPRS) |=  (0x3 <<26) ;	
				else if(bit ==5)	REG32(SPRS) |=  (0x3 <<28) ;		
				else if(bit ==6)	REG32(SPRS) |=  (0x3 <<18) ;					
				//8196d no have c7				
				break;

				
			
		case 'd': 	
/*			
				if(bit==0)	 	REG32(SPRS) |=  (0x3<<8);   
				else if(bit==1)	REG32(SPRS) |=  (0x3<<8);   
				else if(bit==6)	REG32(SPRS) |=  (0x3<<8);   				
				else if(bit ==7)	REG32(SPRS) |=  (0x3<<8);
				
				else if(bit ==2)	REG32(SPRS) |=  (0x3<<10);
				else if(bit ==3)	REG32(SPRS) |=  (0x3<<10);				
				else if(bit ==4)	REG32(SPRS) |=  (0x3<<10);
				else if(bit ==5)	REG32(SPRS) |=  (0x3<<10);				
*/			
				break;

		case 'e': 
				if( (bit>=0) && (bit<=7) ) 	 	REG32(SPRS) |=  (0x3<<8);   
				break;

			
		case 'f':  		
				if( (bit>=0) && (bit<=4) ) 	 	REG32(SPRS) |=  (0x3<<8);   
				else if( (bit>=5) && (bit<=6) ) 	REG32(SPRS) |=  (0x3<<3);    				
				//8196d no have f7				
				break;

		case 'g':  		
				if( (bit>=0) && (bit<=7) ) 	 	REG32(SPRS) |=  (0x3<<10);   						
				break;

		case 'h':  	
/*			
				if( (bit>=0) && (bit<=3) ) 	 	REG32(SPRS) |=  (0x7<<0);   
*/				
				break;


		default: 		break;

	}

}
//------------------------------------------------------------------------
void ChangePinMux_toDefault()
{	
	REG32(SYS_PIN_MUX_SEL) =0x0;
	REG32(SYS_PIN_MUX_SEL2) =0x2492;
	
}
//------------------------------------------------------------------------

void SetGPIOHighLow(unsigned char portname, unsigned int bit, unsigned int val)
{
	unsigned int port=GPIO_GetPortNum_byPortName(portname);
	if(port==GPIO_PORT_MAXNUM) return ;
	
	REG32(GPIO_CNR[port]) = REG32(GPIO_CNR[port])  & ~(1<<bit<<GPIO_OFFSET[port]) ; //0 is set gpio,				
	REG32(GPIO_DIR[port] ) |=  (1<<bit<<GPIO_OFFSET[port]);  //0 input, 1 output, set   bit 7-0 output
	
	int t=REG32(GPIO_DATA[port]) & ~( (1<<bit)<<GPIO_OFFSET[port] );	//clear to 0
	REG32(GPIO_DATA[port]) = t |( (val<<bit)<<GPIO_OFFSET[port] );
	
}
//------------------------------------------------------------------------
int GetGPIOHighLow(unsigned char portname, unsigned int bit)
{
	unsigned int port=GPIO_GetPortNum_byPortName(portname);
	if(port==GPIO_PORT_MAXNUM) return ;
	
	REG32(GPIO_CNR[port]) = REG32(GPIO_CNR[port])  & ~(1<<bit<<GPIO_OFFSET[port]) ; //0 is set gpio,					
	REG32(GPIO_DIR[port] ) &= ~ (1<<bit<<GPIO_OFFSET[port]);  //0 input, 1 output, set   bit 7-0 input

				
	int val=(REG32(GPIO_DATA[port]) & GPIO_MASK[port] )>>GPIO_OFFSET[port] ;
	val=(val & (1<<bit)) >> bit;
	return val;

}
//------------------------------------------------------------------------
int TestGPIO(int argc, char* argv[])
{
	//PB[3:0] output high
	//REG32(PABCDCNR_REG) = REG32(PABCDCNR_REG)& (~(0x0f<<8) ); //0 is set gpio, set PB bit 3-0 gpio
	//REG32(PABCDDIR_REG) = REG32(PABCDDIR_REG) |  (0x0f<<8);  //0 input, 1 output, set  PB bit 3-0 output
	//REG32(PABCDDAT_REG) = REG32(PABCDDAT_REG) |  (0x0f<<8) ;  //PB Data output high

	if(argc < 2) 
	{
		dprintf("Usage: GPIO port[a-h]bit[0-7] mode[o/i] val[0-1] \n");	
		dprintf("EX: GPIO a0 o 1  \n");
		dprintf("EX: GPIO a0 i   \n");		
		return 1;
	}
	int t,i,j;
	unsigned int port,bit,val=0;
	unsigned char mode;

	//parser port
	unsigned char portname=*argv[0];
	port=GPIO_GetPortNum_byPortName(portname);
	if(port==GPIO_PORT_MAXNUM) return 0;

	//parser bit	
	bit=*(argv[0]+1) - '0';	
	
	//parser mode
	mode=*argv[1]; 

	//parser val
	if(argc==3)
		val = strtoul((const char*)(argv[2]), (char **)NULL, 16);		

		
	dprintf("Command: Port=%c, bit=%d, mode=%c, val=%d \n", GPIO_PORT[port], bit, mode, val);
		
	ChangePinMux_toDefault();
	ChangePinMux_toGPIOMode(port, bit);

	if(mode=='o')
	{	SetGPIOHighLow(portname, bit, val );
		//int val2=GetGPIOHighLow(portname, bit);
		int val2=(REG32(GPIO_DATA[port]) & GPIO_MASK[port] )>>GPIO_OFFSET[port] ;
		val2=(val2 & (1<<bit)) >> bit;		
		dprintf("read back is %x\n", val2);
		if(val!=val2)
			dprintf("=> *** Fail !!! output=%x, but read back is=%x \n", val,val2);	
	}

	if(mode=='i')
	{
		val=GetGPIOHighLow(portname, bit);
		dprintf("input bit val=%x \n", val);
	}	
	if(mode=='l')  //loop
				{
		REG32(GPIO_CNR[port]) = REG32(GPIO_CNR[port])  & ~(1<<bit<<GPIO_OFFSET[port]) ; //0 is set gpio,	
					REG32(GPIO_DIR[port] ) |=  (1<<bit<<GPIO_OFFSET[port] );  //set output
					for(i=0;i<0xfffff0;i++)
					{
							REG32( GPIO_DATA[port] ) &= ~( (1<<bit)<<GPIO_OFFSET[port] );  //out 0
								//for(j=0;j<0xffffff;j++) ;
							REG32(GPIO_DATA[port]  ) |=   ( (1<<bit)<<GPIO_OFFSET[port] );  //out 1
								//for(j=0;j<0xffffff;j++) ;
					}
				}
				
/*
	//PE[6:0] PD[7:0] output high
	REG32(PABCDCNR_REG) = REG32(PABCDCNR_REG)& (~(0xff<<GPIO_PD_OFFSET) ); //0 is set gpio, set PD bit 7-0 gpio
	REG32(PABCDDIR_REG) = REG32(PABCDDIR_REG) |  (0xff<<GPIO_PD_OFFSET);  //0 input, 1 output, set  PD bit 7-0 output
	//REG32(PABCDDAT_REG) = REG32(PABCDDAT_REG) |  (0xff<<GPIO_PD_OFFSET) ;  //PD Data output high
	

	//PA[7:0]
	REG32(PABCDCNR_REG) &=  (~(0xff<<GPIO_PA_OFFSET) ); //0 is set gpio, set PA bit 7-0 gpio
	REG32(PABCDDIR_REG)  |=     (0xff<<GPIO_PA_OFFSET);  //0 input, 1 output, set  PA bit 7-0 output
*/
/*

	int t;
	t=REG32(PABCDDAT_REG) & ~( (1<<bit)<<GPIO_PD_OFFSET);	//clear to 0
	REG32(PABCDDAT_REG) = t | ( (val<<bit)<<GPIO_PD_OFFSET);	
	t=REG32(PEFGHDAT_REG) & ~( (1<<bit)<<GPIO_PE_OFFSET);	//clear to 0
	REG32(PEFGHDAT_REG) = t |( (val<<bit)<<GPIO_PE_OFFSET);
*/
	dprintf("------------------- \n");
	for(i=0;i<8;i++)
	{dprintf("Port: %c DAT=%02x \n", GPIO_PORT[i], (REG32(GPIO_DATA[i]) & GPIO_MASK[i] )>>GPIO_OFFSET[i] );
        }



}
//--------------------------------------------------------------------------------------------------
int TestGPIO_TESTIN(int argc, char* argv[])
{
	//PB[3:0] output high
	//REG32(PABCDCNR_REG) = REG32(PABCDCNR_REG)& (~(0x0f<<8) ); //0 is set gpio, set PB bit 3-0 gpio
	//REG32(PABCDDIR_REG) = REG32(PABCDDIR_REG) |  (0x0f<<8);  //0 input, 1 output, set  PB bit 3-0 output
	//REG32(PABCDDAT_REG) = REG32(PABCDDAT_REG) |  (0x0f<<8) ;  //PB Data output high

	if(argc < 2) 
	{
		dprintf("Usage: GPIOTI out_port[a-h]bit[0-7]  in_port[a-h]bit[0-7]\n");	
		dprintf("EX: GPIOTI a0 b1  \n");		
		return 1;
	}
	int t,i,j;
	unsigned int port,bit,val=0;
	unsigned char mode;

	//parser outpot port
	unsigned char portname=*argv[0];
	port=GPIO_GetPortNum_byPortName(portname);
	if(port==GPIO_PORT_MAXNUM) return 0;

	//parser bit	
	bit=*(argv[0]+1) - '0';	

	
	//parser input port
	unsigned char portname2=*argv[1];
	unsigned int port2=GPIO_GetPortNum_byPortName(portname2);
	if(port2==GPIO_PORT_MAXNUM) return 0;

	//parser bit	
	unsigned int bit2=*(argv[1]+1) - '0';	
	
		
	dprintf("Command: outPort=%c, bit=%d, inPort=%c, bit=%d \n", GPIO_PORT[port], bit, GPIO_PORT[port2], bit2);
		
	ChangePinMux_toDefault();
	ChangePinMux_toGPIOMode(port, bit);
	ChangePinMux_toGPIOMode(port2, bit2);	


	//test
	unsigned char pattern[]={0,1,1,0,1,0,0,1};
	for(i=0;i<7;i++)
	{
	int val=pattern[i];
	SetGPIOHighLow(portname, bit, val );
	int val2=GetGPIOHighLow(portname2, bit2);
	
	dprintf("output=%x, input=%x ...", val,val2);	
	
		if(val!=val2)
		//	dprintf("=> *** Fail !!! output=%x, input=%x \n", val,val2);	
			dprintf(" => FAIL\n");
		else
			dprintf("PASS\n");
	

	}

}
//------------------------------------------------------------------------
int gGPIOint=0;

unsigned int testportnum=0;
unsigned int testportbit=0;

static void gpio_interrupt(void)
{
	gGPIOint++;
	int port;
	unsigned int isr;
	//dprintf("\n=>Got GPIO INT!, ");
	
	for(port=0; port<8; port++)
	{	
		isr=  (REG32(GPIO_ISR[port]) & (0x000000ff << GPIO_ISR_OFFSET[port] )   )>>  GPIO_ISR_OFFSET[port];		

		//check 
		if(port==testportnum)
		{
			if( (isr & (1<<testportbit)) ==0)
			{
				dprintf("*** FAIL ! isr=%x\n", isr);
			}
			else
			{	//dprintf("PASS, isr=%x\n",isr);
			}
		}

		
		//dprintf("  GPIO P%c ISR=%02x \n", 'A'+port,  isr);
		REG32(GPIO_ISR[port]) = (0xff <<GPIO_ISR_OFFSET[port] );   //write 1 to clear

		//check
		if( (REG32(GPIO_ISR[port]) & (0xff<<GPIO_ISR_OFFSET[port] )) !=0 )
		{
			dprintf("*** FAIL! cannot clear, isr\n" );
		}
			
	}

	//dprintf("\n");

}
struct irqaction irq_GPIO = {gpio_interrupt, NULL, 8,"gpio", NULL, NULL};  

int TestGPIO_INT(int argc, char* argv[])
{
#if 0
	if(argc < 2) 
	{
		dprintf("Usage: GPIOINT port[a-h]bit[0-7] mode[0: diable, 1:falling trg, 2:  rasing trg, 3:both] \n");	
		dprintf("EX: GPIOINT a0 0  \n");
		dprintf("EX: GPIOINT a0 1   \n");		
		return 1;
	}
	int t,i,j;
	unsigned int port,bit,val=0;
	unsigned int mode;

	//parser port
	unsigned char portname=*argv[0];
	port=GPIO_GetPortNum_byPortName(portname);
	if(port==GPIO_PORT_MAXNUM) return 0;
	
	//parser bit	
	bit=*(argv[0]+1) - '0';	
	
	if(argc>=2)
		mode = strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	

	REG32(GPIO_IMR[port] ) = (mode&3) << ((2*bit) + GPIO_IMR_OFFSET[port]);


  	int irraddr=IRR_REG+GPIO_IRR_NO*4;		
	REG32(irraddr) = (REG32(irraddr) &~(0x0f<<GPIO_IRR_OFFSET)) | (3<<GPIO_IRR_OFFSET);		
	request_IRQ(GPIO_IRQ_NO, &irq_GPIO, NULL); 
#endif
		
#if 1
	if(argc < 2) 
	{
		dprintf("Usage: GPIOINT outport inport \n");	
		dprintf("EX: GPIOINT b0 a0  \n");		
		return 1;
	}

	int t,i,j;
	unsigned int oport,obit,iport,ibit;
	unsigned int mode;

	//parser out port
	unsigned char oportname=*argv[0];
	oport=GPIO_GetPortNum_byPortName(oportname);
	if(oport==GPIO_PORT_MAXNUM) return 0;
	
	//parser bit	
	obit=*(argv[0]+1) - '0';	


	//parser in port
	unsigned char iportname=*argv[1];
	iport=GPIO_GetPortNum_byPortName(iportname);
	if(iport==GPIO_PORT_MAXNUM) return 0;
	
	//parser bit	
	ibit=*(argv[1]+1) - '0';	


	dprintf("Out %c%d, In %c%d \n", 'a'+oport, obit, 'a'+iport, ibit);

	//
	testportnum=iport;
	testportbit=ibit;
	
	//pin mux
	ChangePinMux_toDefault();
	ChangePinMux_toGPIOMode(iport, ibit);
	ChangePinMux_toGPIOMode(oport, obit);	

	//enable irq
	if( (iportname>='a') && (iportname <='d') ) //abcd
	{
  		int irraddr=IRR_REG+GPIO_IRR_NO*4;		
		REG32(irraddr) = (REG32(irraddr) &~(0x0f<<GPIO_IRR_OFFSET)) | (3<<GPIO_IRR_OFFSET);		
		request_IRQ(GPIO_IRQ_NO, &irq_GPIO, NULL); 
	}
	else   //efgh
	{
  		int irraddr=IRR_REG+GPIO2_IRR_NO*4;		
		REG32(irraddr) = (REG32(irraddr) &~(0x0f<<GPIO2_IRR_OFFSET)) | (3<<GPIO2_IRR_OFFSET);		
		request_IRQ(GPIO2_IRQ_NO, &irq_GPIO, NULL); 
	}

	
	//test
	int m;
	int errcnt=0;
	for(m=0; m<4;m++)
	{
		dprintf("----------------------------------\n");
		
		REG32(GPIO_IMR[iport] ) = (m&3) << ((2*ibit) + GPIO_IMR_OFFSET[iport]);

		if(m==0)  //disable, out:[0-1-0-1], no interrupt
		{     dprintf("Test GPIO Mode =%d, disabled, out: [0-1-0-1] \n", m);
			gGPIOint=0;
			
			SetGPIOHighLow(oportname, obit, 0);	delay_ms(200);
				if(GetGPIOHighLow('a'+iport, ibit)==0) 	dprintf("PASS \n");
				else									{ dprintf("FAIL \n"); errcnt++;}
			SetGPIOHighLow(oportname, obit, 1); delay_ms(200);
				if(GetGPIOHighLow('a'+iport, ibit)==1)    dprintf("PASS \n");
				else									{ dprintf("FAIL \n"); errcnt++;}		
			SetGPIOHighLow(oportname, obit, 0); delay_ms(200);
				if(GetGPIOHighLow('a'+iport, ibit)==0) 	dprintf("PASS \n");
				else									{ dprintf("FAIL \n"); errcnt++;}	
			SetGPIOHighLow(oportname, obit, 1);	delay_ms(200);
				if(GetGPIOHighLow('a'+iport, ibit)==1)    dprintf("PASS \n");
				else									{ dprintf("FAIL \n"); errcnt++;}
				
			if(gGPIOint==0)	dprintf("PASS, No got INT \n");
			else				{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint); errcnt++; }
				
		}
		if(m==1)   //falling trg
		{	
			dprintf("Test GPIO Mode =%d, falling trg, out: [1-0-1-0] \n", m);
			SetGPIOHighLow(oportname, obit,1); delay_ms(200);
				gGPIOint=0;				
			SetGPIOHighLow(oportname, obit,0); delay_ms(200);
				if(gGPIOint==1)		dprintf("PASS, got INT \n");
				else					{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;				
			SetGPIOHighLow(oportname, obit,1); delay_ms(200);
				if(gGPIOint==0)		dprintf("PASS, No INT \n");
				else 				{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;							
			SetGPIOHighLow(oportname, obit,0);	delay_ms(200);
				if(gGPIOint==1)		dprintf("PASS, got INT \n");
				else 				{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;					
		}	
		if(m==2)   //rasing trg
		{	dprintf("Test GPIO Mode =%d, rasing trg, out: [0-1-0-1] \n", m);	
			SetGPIOHighLow(oportname, obit,0);delay_ms(200);
				gGPIOint=0;				
			SetGPIOHighLow(oportname, obit,1); delay_ms(200);
				if(gGPIOint==1)		dprintf("PASS, got INT \n");
				else					{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;				
			SetGPIOHighLow(oportname, obit,0); delay_ms(200);
				if(gGPIOint==0)		dprintf("PASS, No INT \n");
				else 				{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;							
			SetGPIOHighLow(oportname, obit,1);	delay_ms(200);
				if(gGPIOint==1)		dprintf("PASS, got INT \n");
				else 				{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;					
		}	
		if(m==3)   //both trg
		{	dprintf("Test GPIO Mode =%d, both trg, out: [0-1-0-1] \n", m);	
			SetGPIOHighLow(oportname, obit,0);delay_ms(200);
				gGPIOint=0;				
			SetGPIOHighLow(oportname, obit,1);delay_ms(200);
				if(gGPIOint==1)		dprintf("PASS, got INT \n");
				else					{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;				
			SetGPIOHighLow(oportname, obit,0);delay_ms(200);
				if(gGPIOint==1)		dprintf("PASS, got INT \n");
				else 				{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;							
			SetGPIOHighLow(oportname, obit,1);	delay_ms(200);
				if(gGPIOint==1)		dprintf("PASS, got INT \n");
				else 				{ dprintf("FAIL gGPIOInt=%x\n", gGPIOint);	errcnt++; }
				gGPIOint=0;					
		}	

	}

	//turn off interrupt
	m=0;
	REG32(GPIO_IMR[iport] ) = (m&3) << ((2*ibit) + GPIO_IMR_OFFSET[iport]);


	//report
	if(errcnt==0)
		dprintf("==> total test is PASS\n");
	else
		dprintf("==> total test is FAIL, fail count is %d\n", errcnt);
	
#endif	

}
#endif
//===========================================================================

//---------------------------------------------------------------------------
int TestBISTAll(int argc, char* argv[])
{

#define mdelay delay_ms
#define BISTCTRL_REG   0xb8000014
#define BISTDONE_REG   0xb8000020
#define BISTFAIL_REG   0xb8000024

#define CLK_MANAGER   0xb8000010
#define PCIE0_BIST_REG   0xb8b01010
#define PCIE1_BIST_REG   0xb8b21010


//BIST CONTROL REG
#define BISTCTRL_BIST_MODE      5 
#define BISTCTRL_OTG_RSTN 11
#define BISTCTRL_CPU_BIST0_RSTN 12
#define BISTCTRL_CPU_BIST1_RSTN 13
#define BISTCTRL_CPU_BISTR_RSTN 14
#define BISTCTRL_VOIP_RSTN 15
#define BISTCTRL_PCIE0_RSTN 16
#define BISTCTRL_PCIE1_RSTN 17
#define BISTCTRL_USB_RSTN 19
#define BISTCTRL_SWCORE_BIST_RSTN 20
#define BISTCTRL_SWCORE_BISR_RSTN 21
#define BISTCTRL_NAND_RSTN 24
#define BISTCTRL_PCS_RSTN 25
#define BISTCTRL_SRAM_RSTN 26
#define BISTCTRL_ROM_RSTN 27

#define BISTCTRL_ENABLE(pos) { REG32(BISTCTRL_REG) |= (1<<pos); }
#define BISTCTRL_DISABLE(pos) { REG32(BISTCTRL_REG) &= ~(1<<pos); }

//BIST DONE
#define BISTDONE_CPU_BIST0_OFF	  (  3)
#define BISTDONE_CPU_BIST1_OFF	  (  4)
#define BISTDONE_CPU_BISTR_OFF	  (  7)
#define BISTDONE_VOIPACC_OFF	  (  9)
#define BISTDONE_PCIE0_OFF   	  (  10)
#define BISTDONE_PCIE1_OFF   	  (  11)
#define BISTDONE_USB_OFF   		  (  13)
#define BISTDONE_SWCORE_BIST_OFF   	  (  14)
#define BISTDONE_SWCORE_BISR_OFF   	  (  16)
#define BISTDONE_ROM_OFF   		  (  20)
#define BISTDONE_OTG_OFF   		  (  21)
#define BISTDONE_PCS_ROM_OFF   	  (  22)
#define BISTDONE_PCS_RAM_OFF   	  (  23)
#define BISTDONE_SRAM_OFF  		  (  24)
#define BISTDONE_NAND_OFF  		  (  25)

#define BISTDONE_GETBIT(pos)  ( (REG32(BISTDONE_REG)&(1<<pos) )>>pos )

//BIST FAIL REG------------------------
#define BISTFAIL_REG   0xb8000024

#define BISTFAIL_SRAM_OFF 		  (  22)
#define BISTFAIL_OTG_OFF 		  (  23)
#define BISTFAIL_VOIPACC_OFF 	  (  24)
#define BISTFAIL_PCIE0_OFF   	  (  26)
#define BISTFAIL_PCIE1_OFF   	  (  27)
#define BISTFAIL_USB_OFF   		  (  29)
#define BISTFAIL_SWCORE_BIST_OFF	  (  30)
#define BISTFAIL_SWCORE_BISR_OFF	  (  31)

#define BISTFAIL_SRAM_PAT 	  (1)
#define BISTFAIL_OTG_PAT	  (1 )
#define BISTFAIL_VOIPACC_PAT 	  (0x3)
#define BISTFAIL_PCIE0_PAT   	  (1 )
#define BISTFAIL_PCIE1_PAT   	  (1 )
#define BISTFAIL_USB_PAT   	  (1 )
#define BISTFAIL_SWCORE_BIST_PAT  (1 )
#define BISTFAIL_SWCORE_BISR_PAT  (1)

#define BISTFAIL_GETBIT(pos) ((REG32(BISTFAIL_REG)&(1<<pos))>>pos)
#define GETVAL(reg,pos,pat) ((REG32(reg)&(pat<<pos))>>pos)

//---------------------------special
#define BISTFAIL_NAND_REG    0xb8000020
#define BISTFAIL_NAND_OFF    (26)
#define BISTFAIL_NAND_PAT    (1)

#define BISTFAIL_PCS_REG    0xb8000020
#define BISTFAIL_PCS_RAM_OFF    ( 27)

#define BISTFAIL_ROM_REG   0xb80000a4
#define BISTFAIL_ROM_OFF   (  0)
#define BISTFAIL_ROM_PAT   (0xffffffff)
//-----------------------------
#define BISTFAIL_CPU_REG   0xb80000a8
#define BISTFAIL_CPU_BIST0_OFF   (  0)
#define BISTFAIL_CPU_BIST1_OFF   (  13)
#define BISTFAIL_CPU_BISTR_OFF   (  23)

#define BISTFAIL_CPU_BIST0_PAT   (0x1ff  )
#define BISTFAIL_CPU_BIST1_PAT   (0x3ff )
#define BISTFAIL_CPU_BISTR_PAT   (0x3 )
//-----------------------------



#define PCIE0_BIST_REG   0xb8b01010
#define PCIE1_BIST_REG   0xb8b21010
//-----------------------------

//BISTCTRL, DONE, OFF, FAIL, OFF, PAT
unsigned int bist[][6]={\
	BISTCTRL_VOIP_RSTN,				BISTDONE_REG,	BISTDONE_VOIPACC_OFF,			BISTFAIL_REG,	BISTFAIL_VOIPACC_OFF,	BISTFAIL_VOIPACC_PAT,
	BISTCTRL_PCIE0_RSTN,			BISTDONE_REG,	BISTDONE_PCIE0_OFF,				BISTFAIL_REG,	BISTFAIL_PCIE0_OFF,			1, 
	BISTCTRL_PCIE1_RSTN,			BISTDONE_REG,	BISTDONE_PCIE1_OFF,				BISTFAIL_REG,	BISTFAIL_PCIE1_OFF,			1, 								
	BISTCTRL_USB_RSTN,				BISTDONE_REG,	BISTDONE_USB_OFF,				BISTFAIL_REG,	BISTFAIL_USB_OFF,			1, 
	BISTCTRL_SWCORE_BIST_RSTN,		BISTDONE_REG,	BISTDONE_SWCORE_BIST_OFF,		BISTFAIL_REG,	BISTFAIL_SWCORE_BIST_OFF,	1, 
	BISTCTRL_SWCORE_BISR_RSTN,		BISTDONE_REG,	BISTDONE_SWCORE_BISR_OFF,		BISTFAIL_REG,	BISTFAIL_SWCORE_BISR_OFF,	1, 
	
	BISTCTRL_ROM_RSTN,				BISTDONE_REG,	BISTDONE_ROM_OFF,				BISTFAIL_ROM_REG,	BISTFAIL_ROM_OFF,			BISTFAIL_ROM_PAT, 
	BISTCTRL_NAND_RSTN,			BISTDONE_REG,	BISTDONE_NAND_OFF,				BISTFAIL_NAND_REG,	BISTFAIL_NAND_OFF,		1, 	
	BISTCTRL_PCS_RSTN,				BISTDONE_REG,	BISTDONE_PCS_RAM_OFF,			BISTFAIL_PCS_REG,	BISTFAIL_PCS_RAM_OFF,		1, 
};


	//Turn On IP
	REG32(CLK_MANAGER)|= (0x01FFFC00);    //all IP


//---------------------------

	printf( "Begin BIST \n");
	REG32(BISTCTRL_REG) = 0;

		
	BISTCTRL_ENABLE(BISTCTRL_BIST_MODE);

	printf( "--------------------- \n");
	printf( "R:BIST DONE=%08x \n", REG32(BISTDONE_REG));
	printf( "R:BIST FAIL=%08x \n", REG32(BISTFAIL_REG));
	printf( "R:PCIE BIST=%08x \n", REG32(PCIE0_BIST_REG));
	printf( "--------------------- \n");

#if 0
	printf( "-Mode1--------------------------------- \n");
	REG32(BISTCTRL_REG) |=  (BISTCTRL_CPU_BIST0_RSTN) ;
		
		mdelay(100);
			
		printf( "R:BISTDONE_REG=%08x, CPU BIST0 done bit(bit 3)=%x \n", REG32(BISTDONE_REG), (REG32(BISTDONE_REG) & BISTDONE_CPU_BIST0)>>3);
		printf( "R:BISTFAIL_REG=%08x, CPU BIST0 fail bit(bit 9:0)=%x ", REG32(BISTFAIL_REG), (REG32(BISTFAIL_REG) & BISTFAIL_CPU_BIST0)>>0);
		if ( (REG32(BISTDONE_REG) & BISTDONE_CPU_BIST0) &&((REG32(BISTFAIL_REG) & BISTFAIL_CPU_BIST0) == 0)) { printf( " ==>PASS \n"); }	 { printf( " ==>FAIL \n"); }	

	REG32(BISTCTRL_REG)  &=  ~(BISTCTRL_CPU_BIST0_RSTN) ;
		

	printf( "-Mode2---------------------------------- \n");
	REG32(BISTCTRL_REG) |= BISTCTRL_CPU_BIST1_RSTN;
			
		mdelay(100);

			
		printf( "R:BISTDONE_REG=%08x, CPU BIST1 done bit(bit 4)=%x \n",   REG32(BISTDONE_REG), (REG32(BISTDONE_REG) & BISTDONE_CPU_BIST1)>>4);
		printf( "R:BISTFAIL_REG=%08x, CPU BIST1 fail bit(bit 19:10)=%x ", REG32(BISTFAIL_REG), (REG32(BISTFAIL_REG) & BISTFAIL_CPU_BIST1)>>10);
		if ( (REG32(BISTDONE_REG) & BISTDONE_CPU_BIST1)  && ((REG32(BISTFAIL_REG) & BISTFAIL_CPU_BIST1) == 0)) { printf( " ==>PASS \n"); }	 { printf( " ==>FAIL \n"); }	

	REG32(BISTCTRL_REG) &=  ~BISTCTRL_CPU_BIST1_RSTN; 
		

	printf( "--------------------------------------- \n");
	REG32(BISTCTRL_REG) |= BISTCTRL_CPU_BISTR_RSTN; 
		
		mdelay(100);
			
		printf( "R:BISTDONE_REG=%08x, CPU BISTR done bit(bit 7)=%x \n", REG32(BISTDONE_REG), (REG32(BISTDONE_REG) & BISTDONE_CPU_BISR_DONE)>>7);
		printf( "R:BISTDONE_REG=%08x, CPU BISTR repr bit(bit 5)=%x \n", REG32(BISTDONE_REG), (REG32(BISTDONE_REG) & BISTDONE_CPU_BISR_REPAIRED)>>5);
		printf( "R:BISTFAIL_REG=%08x, CPU BISTR fail bit(bit 21:20)=%x ", REG32(BISTFAIL_REG), (REG32(BISTFAIL_REG) & BISTFAIL_CPU_BISTR )>>20);
		if ( (REG32(BISTDONE_REG) & BISTDONE_CPU_BISR_DONE)  &&((REG32(BISTFAIL_REG) & BISTFAIL_CPU_BISTR) == 0)) { printf( " ==>PASS \n"); }	 { printf( " ==>FAIL \n"); }	

	//REG32(BISTCTRL_REG = (REG32(BISTCTRL_REG) & ~(BISTCTRL_CPU_BISTR_RSTN) //CPU BISTR RESET cannot write 0
		
#endif
	int ip=0;
	for(ip=0; ip < 6; ip++)
	{
		BISTCTRL_ENABLE(bist[ip][0]);			
		mdelay(100);
			
		printf( " done bit=%x ",   	(REG32(bist[ip][1]) & (1<<bist[ip][2])) >>bist[ip][2]  );
		printf( " fail bit=%x ",   	(REG32(bist[ip][3]) & (bist[ip][5] << bist[ip][4])) >>bist[ip][4]  );		
		if( 		(REG32(bist[ip][1]) & (1<<bist[ip][2]))   && 
			     (REG32(bist[ip][3]) & (bist[ip][5] << bist[ip][4]) == 0)
		   ) 
		{ 	printf( " ==>PASS \n"); 
		}	
		else		
		{	 printf( " ==>FAIL \n"); 
		}	
		
		BISTCTRL_DISABLE(bist[ip][0]);


	}



#if 0
	printf( "---------------------------------- \n"	);
		BISTCTRL_ENABLE(BISTCTRL_VOIP_RSTN);			
		mdelay(100);
			
		printf( "VOIP done bit=%x ",   	BISTDONE_GETBIT(BISTDONE_VOIPACC_OFF));
		printf( "VOIP fail bit=%x ",   	BISTFAIL_GETBIT(BISTFAIL_VOIPACC_OFF));		
		if ( 		BISTDONE_GETBIT(BISTDONE_VOIPACC_OFF) && 
			      (BISTFAIL_GETBIT( BISTFAIL_VOIPACC_OFF) == 0)
		   ) 
				{ printf( " ==>PASS \n"); }	
		else		{ printf( " ==>FAIL \n"); }	
		
		BISTCTRL_DISABLE(BISTCTRL_VOIP_RSTN);
	printf( "------------------------ \n");
	
		BISTCTRL_ENABLE( BISTCTRL_PCIE0_RSTN) ;
			
		mdelay(100);
			
		printf( "PCIE0 done bit=%x \n",	BISTDONE_GETBIT(BISTDONE_PCIE0_OFF) );
		printf( "PCIE0 fail bit=%x ",   	BISTFAIL_GETBIT(BISTFAIL_PCIE0_OFF));
		if ( BISTDONE_GETBIT(BISTDONE_PCIE0_OFF)  && (BISTDONE_GETBIT(BISTFAIL_PCIE0_OFF) == 0))
			{ printf( " ==>PASS \n"); }	 
		else{ printf( " ==>FAIL \n"); }	
		printf( "  PCIE0 BIST=%08x \n", REG32(PCIE0_BIST_REG));	
		BISTCTRL_DISABLE(BISTCTRL_PCIE0_RSTN) ;
	printf( "--------------------------------------- \n");
		BISTCTRL_ENABLE( BISTCTRL_PCIE1_RSTN); 
			
		mdelay(100);
		
		printf( "PCIE1 done bit=%x \n",	BISTDONE_GETBIT(BISTDONE_PCIE1_OFF) );
		printf( "PCIE1 fail bit=%x ",   	BISTFAIL_GETBIT(BISTFAIL_PCIE1_OFF));
		if ( BISTDONE_GETBIT(BISTDONE_PCIE1_OFF)  && (BISTDONE_GETBIT(BISTFAIL_PCIE1_OFF) == 0))
			{ printf( " ==>PASS \n"); }	 
		else{ printf( " ==>FAIL \n"); }	
		printf( "  PCIE1 BIST=%08x \n", REG32(PCIE1_BIST_REG	));
				
		BISTCTRL_DISABLE(BISTCTRL_PCIE1_RSTN); 
	printf( "--------------------------------------- \n");
		BISTCTRL_ENABLE( BISTCTRL_USB_RSTN); 
			
		mdelay(100);
		
		printf( "USB done bit(=%x \n", 	BISTDONE_GETBIT(BISTDONE_USB_OFF) );
		printf( "USB fail bit=%x ",    	BISTFAIL_GETBIT(BISTFAIL_USB_OFF) );
		if ( BISTDONE_GETBIT(BISTDONE_USB_OFF)  && (BISTFAIL_GETBIT(BISTFAIL_USB_OFF) == 0)) 
			{ printf( " ==>PASS \n"); }	 
		else { printf( " ==>FAIL \n"); }			
		BISTCTRL_DISABLE(BISTCTRL_USB_RSTN);
	printf( "---------------------------------- \n");			
		BISTCTRL_ENABLE( BISTCTRL_SWCORE_BIST_RSTN);
			
		mdelay(100);
		
		printf( "SW BIST done bit=%x \n", BISTDONE_GETBIT(BISTDONE_SWCORE_BIST_OFF) );
		printf( "SW BIST fail bit=%x ",    BISTFAIL_GETBIT(BISTFAIL_SWCORE_BIST_OFF) );
		if ( BISTDONE_GETBIT(BISTDONE_SWCORE_BIST_OFF)    && (BISTFAIL_GETBIT(BISTFAIL_SWCORE_BIST_OFF) == 0 ) )
			{ printf( " ==>PASS \n"); }
		else{ printf( " ==>FAIL \n"); }			
		BISTCTRL_DISABLE(BISTCTRL_SWCORE_BIST_RSTN);
	printf( "--------------------------------------- \n");
		BISTCTRL_ENABLE( BISTCTRL_SWCORE_BISR_RSTN) ;
			
		mdelay(100);
		
		printf( "SW BISR done bit=%x \n",  BISTDONE_GETBIT( BISTDONE_SWCORE_BISR_OFF) );
		printf( "SW BISR fail bit=%x \n",    BISTFAIL_GETBIT( BISTFAIL_SWCORE_BISR_OFF) );
		REG32(BISTCTRL_REG) &=  ~(BISTCTRL_SWCORE_BISR_RSTN) ;
			
		
		REG32( 0xbb80420c) = 0x00000013	;
		BISTCTRL_ENABLE( BISTCTRL_SWCORE_BISR_RSTN) ;
			
		printf( "SW BISR done bit=%x \n",  BISTDONE_GETBIT( BISTDONE_SWCORE_BISR_OFF) );
		printf( "SW BISR fail bit=%x \n",    BISTFAIL_GETBIT( BISTFAIL_SWCORE_BISR_OFF) );

		mdelay(100);
		
		if ( BISTDONE_GETBIT( BISTDONE_SWCORE_BISR_OFF)  && (BISTFAIL_GETBIT( BISTFAIL_SWCORE_BISR_OFF)== 0)) 
			{ printf( " ==>PASS \n"); }
		else{ printf( " ==>FAIL \n"); }			
		BISTCTRL_DISABLE(BISTCTRL_SWCORE_BISR_RSTN) ;
	printf( "--------------------------------------- \n");
#endif
/*
	

	printf( "-all-------------------- \n");
	ew b8000014=0x00004220
	dw b8000020
	dw b8000024

	ew b8000014=0x000FC220

	dw b8000020
	dw b8000024

*/
		
	//===============================================================
	//
	//printf( "\n"
	//	printf( "R:PCIE EMA1=%08x \n", @0xb8b01018 
	//ew b8b01018=0x09249249
	//	printf( "R:PCIE EMA1=%08x \n", @0b8b01018

	//	printf( "R:PCIE EMA2=%08x \n", @0xb8b0101C 
	//ew b8b0101c=0x00009249
	//	printf( "R:PCIE EMA2=%08x \n", @0xb8b0101C 
		
	//===============================================================	
	

}
//=======================================================================
int TestDRF(int argc, char* argv[])
{
	#define DRFCTRL_REG 0xb8000018
	#define DRFCTRL_DRF_MODE	 15
	#define DRFCTRL_MODE1_RST	 16
	#define DRFCTRL_MODE2_RST	 17
	#define DRFCTRL_MODE3_RST	 18
	#define DRFCTRL_MODE4_RST	 19
	#define DRFCTRL_MODE5_RST	 20
	#define DRFCTRL_MODE6_RST	 21
		
	#define DRFCTRL_MODE1_RESUME	 23
	#define DRFCTRL_MODE2_RESUME	 17
	#define DRFCTRL_MODE3_RESUME	 18
	#define DRFCTRL_MODE4_RESUME	 19
	#define DRFCTRL_MODE5_RESUME	 20
	#define DRFCTRL_MODE6_RESUME	 21

	#define SET_DRFCTRL(pos) (REG32(DRFCTRL_REG) |= (1<<pos))
	#define CLEAR_DRFCTRL(pos) (REG32(DRFCTRL_REG) &= ~(1<<pos))
	

	#define DRFDONE_REG 0xb8000028
	#define DRFDONE_PAUSE 1
	#define DRFDONE_ALLIP ((1<<1) | (1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6) | (1<<7) |  \
							(1<<8) | (1<<9) | (1<<11) | (1<<12) | (1<<13) | (1<<14) | (1<<15) )

	#define GET_DRFDONE(pos) ((REG32(DRFDONE_REG) &(1<<pos))>>pos)


	#define DRFFAIL_REG 0xb800002c
	#define DRFFAIL_OTHER_REG 0xb8000028
	#define DRFFAIL_OTHER_MASK ((1<<16)|(1<<17)|(1<<18))
	#define DRFFAIL_CPU_REG 0xb80000ac
	
							
	SET_DRFCTRL(DRFCTRL_DRF_MODE);

	SET_DRFCTRL(DRFCTRL_MODE1_RST);
	SET_DRFCTRL(DRFCTRL_MODE2_RST);	
	SET_DRFCTRL(DRFCTRL_MODE3_RST);
	SET_DRFCTRL(DRFCTRL_MODE4_RST);	
	SET_DRFCTRL(DRFCTRL_MODE5_RST);

	//wait 50us
	delay_ms(10);

	int i;
for(i=0; i<2; i++)
{

	//chk pause
	if(GET_DRFDONE(DRFDONE_PAUSE)==1)
		printf("Pause PASS\n");
	else printf("Pause FAIL\n");

	//start
	SET_DRFCTRL(	DRFCTRL_MODE1_RESUME);
	CLEAR_DRFCTRL(	DRFCTRL_MODE1_RESUME);

	SET_DRFCTRL(	DRFCTRL_MODE2_RESUME);
	CLEAR_DRFCTRL(	DRFCTRL_MODE2_RESUME);
	
	SET_DRFCTRL(	DRFCTRL_MODE3_RESUME);
	CLEAR_DRFCTRL(	DRFCTRL_MODE3_RESUME);

	SET_DRFCTRL(	DRFCTRL_MODE4_RESUME);
	CLEAR_DRFCTRL(	DRFCTRL_MODE4_RESUME);

	SET_DRFCTRL(	DRFCTRL_MODE5_RESUME);
	CLEAR_DRFCTRL(	DRFCTRL_MODE5_RESUME);

	//chk pause
	if(GET_DRFDONE(DRFDONE_PAUSE)==0)
		printf("Pause PASS\n");
	else printf("Pause FAIL\n");


	//wait 83us, 42us
	delay_ms(10);
	
}

	if(REG32(DRFDONE_REG)& DRFDONE_ALLIP==DRFDONE_ALLIP)
		printf("Done PASS\n");
	else printf("Done FAIL = %x\n", REG32(DRFDONE_REG) );		



	printf("IP FAIL value =%x\n", REG32(DRFFAIL_REG) );
	printf("Other FAIL value=%x\n", REG32(DRFFAIL_OTHER_REG) & DRFFAIL_OTHER_MASK);
	printf("CPU FAIL value=%x\n", REG32(DRFFAIL_CPU_REG));

	if( (	(REG32(DRFFAIL_OTHER_REG) & DRFFAIL_OTHER_MASK)==0) &&
             ((REG32(DRFFAIL_OTHER_REG) & DRFFAIL_OTHER_MASK) ==0)	&&
       	(REG32(DRFFAIL_CPU_REG)==0))
  		printf("=> DRF All PASS\n");
	else printf("=> DRF FAIL\n");	     	
	
}

//===========================================================

//------------------------------------------------------------------------
void PCIE_Device_PERST(void)
{
	 #define CLK_MANAGE 	0xb8000010
        // 6. PCIE Device Reset       
     REG32(CLK_MANAGE) &= ~(1<<26);    //perst=0 off.    
        __delay(100000);   //PCIE standadrd: poweron: 100us, after poweron: 100ms
        __delay(100000);   
        __delay(100000);   		
    REG32(CLK_MANAGE) |=  (1<<26);   //PERST=1

}
//------------------------------------------------------------------------

void PCIE_MDIO_Reset(unsigned int portnum)
{
        #define SYS_PCIE_PHY0   (0xb8000000 +0x50)
        #define SYS_PCIE_PHY1   (0xb8000000 +0x54)	
	 
	unsigned int sys_pcie_phy;

	if(portnum==0)	sys_pcie_phy=SYS_PCIE_PHY0;
	else if(portnum==1)	sys_pcie_phy=SYS_PCIE_PHY1;
	else return;
		
       // 3.MDIO Reset
 	   REG32(sys_pcie_phy) = (1<<3) |(0<<1) | (0<<0);     //mdio reset=0,     	    //0x088
 	   REG32(sys_pcie_phy) = (1<<3) |(0<<1) | (1<<0);     //mdio reset=1,           //0x09
 	   REG32(sys_pcie_phy) = (1<<3) |(1<<1) | (1<<0);     //bit1 load_done=1       //0x0a

}
//------------------------------------------------------------------------
void PCIE_PHY_Reset(unsigned int portnum)
{
	 #define PCIE_PHY0 	0xb8b01008
	 #define PCIE_PHY1 	0xb8b21008
	 
	unsigned int pcie_phy;

	if(portnum==0)	pcie_phy=PCIE_PHY0;
	else if(portnum==1)	pcie_phy=PCIE_PHY1;
	else return;

        //4. PCIE PHY Reset       
	REG32(pcie_phy) = 0x01;	//bit7:PHY reset=0   bit0: Enable LTSSM=1
	REG32(pcie_phy) = 0x81;   //bit7: PHY reset=1   bit0: Enable LTSSM=1
	
}
//------------------------------------------------------------------------
int PCIE_Check_Link(unsigned int portnum)
{
	unsigned int dbgaddr;
	unsigned int cfgaddr;
	
	if(portnum==0)	dbgaddr=0xb8b00728;
	else if(portnum==1)	dbgaddr=0xb8b20728;
	else if(portnum==2)	dbgaddr=0xb8b40728;	
	else return;	

  //wait for LinkUP
      delay_ms(10);
	volatile int i=5;
	while(--i)
	{
		 delay_ms(10);	  
	      if( (REG32(dbgaddr)&0x1f)==0x11)
		  	break;

	}
	if(i==0)
	{	if(at2_mode==0)  //not auto test, show message
		dprintf("PCIE_P%d ->  Cannot LinkUP\n",portnum);
		return 0;
	}
	else  //already  linkup
	{
		if(portnum==0)	      REG32(PCIE0_RC_CFG_BASE + 0x04)= 0x00100007;
		else if(portnum==1) REG32(PCIE1_RC_CFG_BASE + 0x04)= 0x00100007;

		  
		if(portnum==0) cfgaddr=0xb8b10000;
		else if(portnum==1) cfgaddr=0xb8b30000;

		if(at2_mode==0)
		dprintf("Find Port=%x Device:Vender ID=%x\n", portnum, REG32(cfgaddr) );
	}
	return 1;
}

//------------------------------------------------------------------------
unsigned int HostPCIe_SetPhyMdioRead(unsigned int portnum, unsigned int regaddr)
{
	unsigned int mdioaddr;

	if(portnum==0)		mdioaddr=PCIE0_MDIO;	
	else if(portnum==1)	mdioaddr=PCIE1_MDIO;
	else if(portnum==2)	mdioaddr=0xb8b410c0;	  //EP MDIO
	else return 0;
	
	REG32(mdioaddr)= ((regaddr&0x1f)<<PCIE_MDIO_REG_OFFSET)  | (0<<PCIE_MDIO_RDWR_OFFSET); 
	//delay 
	volatile int i;
	for(i=0;i<5555;i++)  ;

	int val;
	val=REG32(mdioaddr)&  (0xffff <<PCIE_MDIO_DATA_OFFSET) ;
	return ((val>>PCIE_MDIO_DATA_OFFSET)&0xffff);
	
}


void HostPCIe_SetPhyMdioWrite(unsigned int portnum, unsigned int regaddr, unsigned short val)
{
	unsigned int mdioaddr;

	if(portnum==0)		mdioaddr=PCIE0_MDIO;	
	else if(portnum==1)	mdioaddr=PCIE1_MDIO;
	else if(portnum==2)	mdioaddr=0xb8b410c0;	  //EP MDIO	
	else return 0;
	
	REG32(mdioaddr)= ( (regaddr&0x1f)<<PCIE_MDIO_REG_OFFSET) | ((val&0xffff)<<PCIE_MDIO_DATA_OFFSET)  | (1<<PCIE_MDIO_RDWR_OFFSET) ; 
	//delay 
	volatile int i;
	for(i=0;i<5555;i++)  ;
}

//------------------------------------------------------------------------
//#ifdef CONFIG_EPHY_40MHZ
//#define PHY_EAT_40MHZ 1
//#else  //25MHz
//#define PHY_EAT_40MHZ 0
//#endif

void PCIE_reset_procedure(int portnum, int Use_External_PCIE_CLK, int mdio_reset)
{
 //	dprintf("port=%x, mdio_rst=%x \n", portnum, mdio_reset);

 #if 1
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	unsigned int v=REG32(SYS_HW_STRAP);
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	unsigned char sel40m=GET_BITVAL(v, 25, RANG1);
#endif


 	//first, Turn On PCIE IP
	 #define CLK_MANAGE 	0xb8000010
	REG32(CLK_MANAGE) |= (1<<12) | (1<<13) | (1<<18);
	 
	if(portnum==0)		    REG32(CLK_MANAGE) |=  (1<<14);        //enable active_pcie0
	else if(portnum==1)	    REG32(CLK_MANAGE) |=  (1<<16);        //enable active_pcie1	
	else return;
		    

			
      // __delay(1000*1000*1);
       delay_ms(10);

        //add compatible, slove sata pcie card.
	if(portnum==0)	  REG32(0xb8b0100c)=(1<<3);  //set target Device Num=1;
	if(portnum==1)	  REG32(0xb8b2100c)=(2<<3);  //set target Device Num=1;

 	//REG32(SYS_PCIE_ANA)=0x3f39;   //PCIE_ANA  bokai tell me to do this.

	if(mdio_reset)
	{
		if(at2_mode==0)  //no auto test, show message
			dprintf("Do MDIO_RESET\n");
		
		 delay_ms(10);
       	// 3.MDIO Reset
		PCIE_MDIO_Reset(portnum);
	}  
	
	  delay_ms(10);
 	PCIE_PHY_Reset(portnum);	
	
       // __delay(1000*1000);
        delay_ms(10);
       
 
	  //----------------------------------------
	  if(mdio_reset)
	  	{

//------------------------------------------------------

		HostPCIe_SetPhyMdioWrite(portnum, 0, 0xD087);  //bokai tell, and fix

		HostPCIe_SetPhyMdioWrite(portnum, 1, 0x0003);
		HostPCIe_SetPhyMdioWrite(portnum, 2, 0x4d18);
//#if  PHY_EAT_40MHZ
		if(sel40m==1)
		HostPCIe_SetPhyMdioWrite(portnum, 5, 0x0BCB);   //40M
//#endif

//#if  PHY_EAT_40MHZ
		if(sel40m==1)
		HostPCIe_SetPhyMdioWrite(portnum, 6, 0xF148);  //40M
//#else
		else
		HostPCIe_SetPhyMdioWrite(portnum, 6, 0xf848);  //25M
//#endif

		HostPCIe_SetPhyMdioWrite(portnum, 7, 0x31ff);
//		HostPCIe_SetPhyMdioWrite(portnum, 8, 0x18d5);  //peisi tune
		HostPCIe_SetPhyMdioWrite(portnum, 8, 0x18d6);  //slove high temp	


		HostPCIe_SetPhyMdioWrite(portnum, 0x09, 0x539c); 	
		HostPCIe_SetPhyMdioWrite(portnum, 0x0a, 0x20eb); 	
//		HostPCIe_SetPhyMdioWrite(portnum, 0x0d, 0x1764); 	     //tx amp=111, over eye mask
		HostPCIe_SetPhyMdioWrite(portnum, 0x0d, 0x1464);     //tx amp=100


		HostPCIe_SetPhyMdioWrite(portnum, 0x0b, 0x0511);   //for sloving low performance


		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0a00);				
		HostPCIe_SetPhyMdioWrite(portnum, 0x19, 0xFCE0); 

		HostPCIe_SetPhyMdioWrite(portnum, 0x1a, 0x7e4f);   //formal chip, reg 0x1a.4=0
		HostPCIe_SetPhyMdioWrite(portnum, 0x1b, 0xFC01);   //formal chip	 reg 0x1b.0=1
		
		HostPCIe_SetPhyMdioWrite(portnum, 0x1e, 0xC280);	


		if(Use_External_PCIE_CLK==1)
		{
		//external clock
		//reg6=fc48
		//reg 1a=bit 4=1   7e40
		//reg 1b=bit 0=0
		HostPCIe_SetPhyMdioWrite(portnum, 0x06, 0xfc48);	
		HostPCIe_SetPhyMdioWrite(portnum, 0x1a, 0x7e50);	
		HostPCIe_SetPhyMdioWrite(portnum, 0x1b, 0xfc00);			

		}

	  	}
 
	//---------------------------------------
         // 6. PCIE Device Reset
       delay_ms(10);
	PCIE_PHY_Reset(portnum);
       REG32(CLK_MANAGE) &= ~(1<<26);    //perst=0 off.
     	delay_ms(300);
       REG32(CLK_MANAGE) |=  (1<<26);   //PERST=1 
	//---------------------------------------	  
	PCIE_Check_Link(portnum);

}


//------------------------------------------------------------------------
int Test_HostPCIE_DataLoopback(int argc, char* argv[])
{
//	extern void example();

//    PCIE_reset_procedure(0,0,1);
	unsigned int portnum=0;
	unsigned int test_packet_num=1;
	
	if(argc >= 1) 
	{	portnum= strtoul((const char*)(argv[0]), (char **)NULL, 16);
	}

	if(argc >= 2) 
	{	test_packet_num= strtoul((const char*)(argv[1]), (char **)NULL, 16);
	}

    unsigned int cnt;

    unsigned int PCIE_Test_cnt;
	unsigned int pcie_err=0;
	
	if(test_packet_num==0)
	{
		while(1)
       	 if(example(portnum, at2_mode)==0)   //0: mean fail
       	 {
				printf("FAIL, and hang!\n");
				while(1)  {};
		 }
	}
	
    for (PCIE_Test_cnt=1; PCIE_Test_cnt<=test_packet_num; PCIE_Test_cnt++)
    {
#if DBG
        prom_printf("\n==================(Start)======================\n");
        prom_printf("\nPCIE_Test_cnt:%d\n",PCIE_Test_cnt);
#endif        

        if(example(portnum, at2_mode)==0)   //0: mean fail

	{		at2_errcnt++;
		pcie_err++;
        }


#if 0  //for bokai
	if 	(rtl_inb(UART_LSR) & 0x1)
	{	
		int ch=rtl_inb(UART_RBR)&0xff;
		if(ch=='q')
			return;
	}
#endif
		
    }
	printf("====> Total test cnt=%d, test fail=%d\n", test_packet_num,  pcie_err);
	
}; 
//------------------------------------------------------------------------
int PCIE_Host_RESET(int argc, char* argv[])
{
	int  portnum= 0;  //0: one port, 1: two port
	int Use_External_PCIE_CLK=0;
	int mdio_reset=0;
	if(argc<2)
	{//dump all	
	       dprintf("\n"); 		   
		dprintf("hrst <portnum> <mdio_reset>\n");
		return;
	}
	
	if(argc >= 1) 
	{	portnum= strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	}
	if(argc >= 2) 
	{	mdio_reset= strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	}

	if(portnum==2)
	{
		PCIE_reset_procedure(0, Use_External_PCIE_CLK, mdio_reset);
		PCIE_reset_procedure(1, Use_External_PCIE_CLK, mdio_reset);
		//PCIE_reset_procedure(portnum, 1, mdio_reset);		
	}		
	else
		PCIE_reset_procedure(portnum, Use_External_PCIE_CLK, mdio_reset);

}; 


//--------------------------------------------------------------------------
int PCIE_Host_Init(int argc, char* argv[])
{
	int portnum=0;
	if(argc >= 1) 
	{	portnum= strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	}



#define MAX_READ_REQSIZE_128B    0x00
#define MAX_READ_REQSIZE_256B    0x10
#define MAX_READ_REQSIZE_512B    0x20
#define MAX_READ_REQSIZE_1KB     0x30
#define MAX_READ_REQSIZE_2KB     0x40
#define MAX_READ_REQSIZE_4KB     0x50


#define MAX_PAYLOAD_SIZE_128B    0x00
#define MAX_PAYLOAD_SIZE_256B    0x20
#define MAX_PAYLOAD_SIZE_512B    0x40
#define MAX_PAYLOAD_SIZE_1KB     0x60
#define MAX_PAYLOAD_SIZE_2KB     0x80
#define MAX_PAYLOAD_SIZE_4KB     0xA0



	int rc_cfg, cfgaddr;
	int iomapaddr;
	int memmapaddr;

	if(portnum==0)
	{	rc_cfg=PCIE0_RC_CFG_BASE;
		cfgaddr=PCIE0_EP_CFG_BASE;
		iomapaddr=PCIE0_MAP_IO_BASE;
		memmapaddr=PCIE0_MAP_MEM_BASE;
	}
	else if(portnum==1)
	{	rc_cfg=PCIE1_RC_CFG_BASE;
		cfgaddr=PCIE1_EP_CFG_BASE;
		iomapaddr=PCIE1_MAP_IO_BASE;
		memmapaddr=PCIE1_MAP_MEM_BASE;	
	}
	else 
	{	return 0;
	}
	
	int t=REG32(rc_cfg);
	unsigned int vid=t&0x0000ffff;   //0x819810EC
	unsigned int pid=(t&0xffff0000) >>16;
	
	if( (vid!= 0x10ec) || (pid!=0x8196))
	{	DBG_PRINT("VID=%x, PID=%x \n", vid, pid );
		DBG_PRINT(" !!!  Read VID PID fail !!! \n");
		//at_errcnt++;
		return;
	}

	//STATUS
	//bit 4: capabilties List

	//CMD
	//bit 2: Enable Bys master, 
	//bit 1: enable memmap, 
	//bit 0: enable iomap

	REG32(rc_cfg + 0x04)= 0x00100007;   

	//Device Control Register 
	//bit [7-5]  payload size
	REG32(rc_cfg + 0x78)= (REG32(rc_cfg + 0x78 ) & (~0xE0)) | MAX_PAYLOAD_SIZE_128B;  // Set MAX_PAYLOAD_SIZE to 128B,default
	  
      REG32(cfgaddr + 0x04)= 0x00100007;    //0x00180007

	//bit 0: 0:memory, 1 io indicate
      REG32(cfgaddr + 0x10)= (iomapaddr | 0x00000001) & 0x1FFFFFFF;  // Set BAR0

	//bit 3: prefetch
	//bit [2:1] 00:32bit, 01:reserved, 10:64bit 11:reserved
      REG32(cfgaddr + 0x18)= (memmapaddr | 0x00000004) & 0x1FFFFFFF;  // Set BAR1  


	//offset 0x78 [7:5]
      REG32(cfgaddr + 0x78) = (REG32(cfgaddr + 0x78) & (~0xE0)) | (MAX_PAYLOAD_SIZE_128B);  // Set MAX_PAYLOAD_SIZE to 128B

	//offset 0x79: [6:4] 
      REG32(cfgaddr + 0x78) = (REG32(cfgaddr + 0x78) & (~0x7000)) | (MAX_READ_REQSIZE_256B<<8);  // Set MAX_REQ_SIZE to 256B,default

	  
	//check
//      if(REG32(cfgaddr + 0x10) != ((iomapaddr | 0x00000001) & 0x1FFFFFFF))
      {	//at_errcnt++;
      		DBG_PRINT("Read Bar0=%x \n", REG32(cfgaddr + 0x10)); //for test
      	}
	  

//	if(REG32(cfgaddr + 0x18)!=((memmapaddr| 0x00000004) & 0x1FFFFFFF))
	{	//at_errcnt++;
      		DBG_PRINT("Read Bar1=%x \n", REG32(cfgaddr + 0x18));      //for test
	}
	DBG_PRINT("Set BAR finish \n");


	//io and mem limit, setting to no litmit
	REG32(rc_cfg+ 0x1c) = (2<<4) | (0<<12);   //  [7:4]=base  [15:12]=limit
	REG32(rc_cfg+ 0x20) = (2<<4) | (0<<20);   //  [15:4]=base  [31:20]=limit	
	REG32(rc_cfg+ 0x24) = (2<<4) | (0<<20);   //  [15:4]=base  [31:20]=limit		

}


//----------------------------------------------------------------------------
int HostPCIe_MDIORead(int argc, char* argv[])
{

	unsigned int i,val,j;
/*
	if(argc<1)
	{//dump all	
	       dprintf("\n"); 		   
		dprintf("emdior <portnum> \n");
		return;
	}
*/	
	unsigned int portnum=0;
	
	if(argc>=1)
		portnum	= strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	
	for(i=0; i<=0x1f; i++)
	{/*
		REG32(PCIE0_MDIO)= (i<<PCIE_MDIO_REG_OFFSET) | (0<<PCIE_MDIO_RDWR_OFFSET) ;   
		for(j=0;j<0x5555;j++) ;
		val=REG32(PCIE0_MDIO);
		val=( val& PCIE_MDIO_DATA_MASK ) >> PCIE_MDIO_DATA_OFFSET;		
	*/
		val=HostPCIe_SetPhyMdioRead(portnum, i);
		dprintf("MDIO Reg %x=%x \n", i,val);

	}



}; 
//----------------------------------------------------------------------------
int HostPCIe_MDIOWrite(int argc, char* argv[])
{

	if(argc<3)
	{	 
		dprintf("mdiow <portnum> <addr> <val> \n");		
		dprintf("ex: mdiow 0 00  ffff \n");			
		return;	
	}

	unsigned int portnum = strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	unsigned int addr = strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	unsigned int val = strtoul((const char*)(argv[2]), (char **)NULL, 16);	
/*
	REG32(PCIE0_MDIO)= (addr<<PCIE_MDIO_REG_OFFSET) | (val<<PCIE_MDIO_DATA_OFFSET)  | (1<<PCIE_MDIO_RDWR_OFFSET) ;   ;   
*/
	HostPCIe_SetPhyMdioWrite(portnum, addr, val);

}; 
//---------------------------------------------------------------------------

int  PCIE_PowerDown(int argc, char* argv[])
{
	#define PCIE0_RC_CFG_BASE (0xb8b00000)
	#define PCIE0_EP_CFG_BASE (0xb8b10000)
	#define PCIE1_RC_CFG_BASE (0xb8b20000)
	#define PCIE1_EP_CFG_BASE (0xb8b30000)
	#define DEVICE_OFFSET (0x10000)

	 #define PCIE_PHY0 	0xb8b01008
	 
	 int portnum=0;
	 int mode=0;
	int baseaddr=0xb8b00000;
	 
	if( argc < 2 ) 
	{
		dprintf("epdn <portnum> <mode>.\n");	
		dprintf("epdn 0: D0 ->L0 \n");			
		dprintf("epdn 3: D3hot ->L1 \n");
		dprintf("epdn 4: board cast PME_TurnOff \n");	
		
		dprintf("epdn 7: enable aspm and L0s entry \n");	
		dprintf("epdn 8: enable aspm and L1 entry \n");	
		dprintf("epdn 9: diable  aspm \n");	

		dprintf("epdn 010: measure L0->L1->L0 \n");		
		dprintf("epdn 020: measure L0->L2->L0 \n");			
		dprintf("P0 Link status=%x \n", REG32(0xb8b00728)&0x1f );		
		dprintf("P1 Link status=%x \n", REG32(0xb8b20728)&0x1f );		
		return 0;
	}
	if(argc>=1)	portnum = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	if(argc>=2)   mode = strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	int tmp;

	if(portnum==0) baseaddr=PCIE0_RC_CFG_BASE;
	else if(portnum==1) baseaddr=PCIE1_RC_CFG_BASE;
	else { dprintf("Only support P0 and P1 \n"); return 0; }


	if(mode==0)
	{

		#if 0 //saving more power, leave L1 write
		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0f0f);
		#endif	
		
		tmp = REG32(baseaddr+DEVICE_OFFSET+0x44) &( ~(3));  //D0
		REG32(baseaddr+DEVICE_OFFSET+0x44) = tmp|  (0);  //D0	
		dprintf("D0 \n");
	
	}

	
	if(mode==3)
	{

		#if 1 //saving more power
		REG32(baseaddr+DEVICE_OFFSET+0x80)|= (0x100);  //enable clock PM
		#endif
		
		tmp = REG32(baseaddr+DEVICE_OFFSET+0x44) &( ~(3));  //D0
		REG32(baseaddr+DEVICE_OFFSET+0x44) = tmp|  (3);  //D3	
		//HostPCIe_SetPhyMdioWrite(0xd, 0x15a6);
		dprintf("D3 hot \n");		

		#if 0 //saving more power		
		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0708);
		#endif
	}

	if(mode==4)
	{	
		#if 0 //saving more power   leave L1 write
		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0f0f);
		#endif	
	
		REG32(baseaddr+0x1008) |= (0x200);  		
		dprintf("Host boardcase PME_TurnOff \n");		
	}
	if(mode==7)
	{
		REG32(baseaddr+DEVICE_OFFSET+0x070c) &= ~  ((0x7 <<27)|(0x7<<24));
		REG32(baseaddr+DEVICE_OFFSET+0x070c) |=  ((3)<<27) | ((1)<<24);   //L1=3us, L0s=1us

		REG32(baseaddr+0x80) &= ~(0x3);
		REG32(baseaddr+DEVICE_OFFSET+0x80) &= ~(0x3);		

		REG32(baseaddr+0x80) |= 1;   //L0s
		REG32(baseaddr+DEVICE_OFFSET+0x80) |= 1;				

	}



	if(mode==8)
	{
		REG32(baseaddr+DEVICE_OFFSET+0x070c) &= ~  ((0x7 <<27)|(0x7<<24));
		REG32(baseaddr+DEVICE_OFFSET+0x070c) |=  ((1)<<27) | ((3)<<24);   //L1=1us, L0s=3us

		REG32(baseaddr+0x80) &= ~(0x3);
		REG32(baseaddr+DEVICE_OFFSET+0x80) &= ~(0x3);		

		REG32(baseaddr+0x80) |= 3;   //L1
		REG32(baseaddr+DEVICE_OFFSET+0x80) |= 3;	//L1			

	}

	if(mode==9)
	{
		REG32(baseaddr+0x80) &= ~(0x3);
		REG32(baseaddr+DEVICE_OFFSET+0x80) &= ~(0x3);
	}
		
	
#if 0
	if(powerdown==1)
	{
//		REG32(0xb8b00044) &= ~(3);	//D0	
		HostPCIe_SetPhyMdioWrite(0xd, 0x15b6);

    REG32(PCIE_PHY0) = 0x1;	//bit7 PHY reset=0   bit0 Enable LTSSM=1
    REG32(PCIE_PHY0) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1


//	__delay(9000000);  //OK
	__delay(9000000);	
//	HostPCIe_SetPhyMdioWrite(0xd, 0x15b6);

	REG32(PCIE_PHY0) = 0x1;	//bit7 PHY reset=0   bit0 Enable LTSSM=1
        REG32(PCIE_PHY0) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1

//    REG32(PCIE_PHY0) = 0x1;	//bit7 PHY reset=0   bit0 Enable LTSSM=1
//    REG32(PCIE_PHY0) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1
	
	
	//	REG32(0xb8b10044) &= ~(3);	//D0		
		dprintf("333 \n");	
		dprintf("status=%x \n", REG32(0xb8b00728) );		
	}
#endif

       //-------------------------------------------------------------	
	if(mode==0x010)
	{
             //L0->L1->L0
		tmp = REG32(0xb8b10044) &( ~(3));  //D0
	/*	
		REG32(0xb8b10044) = tmp|  (0);  //D0	


		while(1)
		{	if((REG32(0xb8b00728)&0x1f)==0x11)   //wait to L0
			break;
		}	
	
       */
		REG32(0xb8b10044) = tmp|  (3);  //D3	
		/*
		while(1)
		{	if((REG32(0xb8b00728)&0x1f)==0x14)   //wait to L1
			break;
		}	
		*/
		delay_ms(100);
		//REG32(0xb8b10000);
		REG32(0xb8b10044) = tmp|  (0);  //D0, wakeup
		
		while(1)
		{	if((REG32(0xb8b00728)&0x1f)==0x11)   //wait to L0
			break;
		}	
		
		//delay_ms(100);
		dprintf("DID/VID=%x\n", REG32(0xb8b10000));
	}
	//-------------------------------------------------------------	
	if(mode==0x020)
	{
             //L0->L2->L0
		tmp = REG32(0xb8b10044) &( ~(3));  //D0

		REG32(0xb8b10044) = tmp|  (3);  //D3	
		delay_ms(100);

		REG32(0xb8b01008) |= (0x200);  
        __delay(100000);  		
		//dprintf("Host boardcase PME_TurnOff \n");	
		//delay_ms(100);

	//wakeup
     REG32(SYS_CLKMANAGE) &= ~(1<<12);    //perst=0 off.
            //dprintf("CLK_MANAGE=%x \n",  REG32(CLK_MANAGE));
        __delay(100000);   
        __delay(100000);   
        __delay(100000);   
		
    REG32(SYS_CLKMANAGE) |=  (1<<12);   //PERST=1
    //prom_printf("\nCLK_MANAGE(0x%x)=0x%x\n\n",CLK_MANAGE,READ_MEM32(CLK_MANAGE));

	
        //4. PCIE PHY Reset       
    REG32(PCIE_PHY0) = 0x01;	//bit7 PHY reset=0   bit0 Enable LTSSM=1
    REG32(PCIE_PHY0) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1

#if 0  //wait for LinkUP
	int i=1000;
	while(--i)
	{
	      if( (REG32(0xb8b00728)&0x1f)==0x11)
		  	break;
      		__delay(100000);		  

	}
	if(i==0)
	{	dprintf("i=%x  Cannot LinkUP \n",i);
	}
#else

	while(1)
	{
		if( (REG32(0xb8b00728)&0x1f)==0x11)
		  	break;
	}
#endif



		dprintf("DID/VID=%x\n", REG32(0xb8b10000));
	}
		





		dprintf("Port%d Link status=%x \n", portnum, REG32(baseaddr+0x728)&0x1f );			
		
};
//---------------------------------------------------------------------------
int  HostPCIe_TestINT(int argc, char* argv[])
{
	if( argc < 2 ) 
	{
		dprintf("eint <portnum> <loops>.\n");	
	
		return 0;
	}
	unsigned int portnum=0, loops=10;
	if(argc>=1)	portnum = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	if(argc>=2)   loops = strtoul((const char*)(argv[1]), (char **)NULL, 16);


	int rc_cfg, cfgaddr;
	int iomapaddr;
	int memmapaddr;
	
	if(portnum==0)
	{	rc_cfg=PCIE0_RC_CFG_BASE;
		cfgaddr=PCIE0_EP_CFG_BASE;
		iomapaddr=PCIE0_MAP_IO_BASE;
		memmapaddr=PCIE0_MAP_MEM_BASE;
	}
	else if(portnum==1)
	{	rc_cfg=PCIE1_RC_CFG_BASE;
		cfgaddr=PCIE1_EP_CFG_BASE;
		iomapaddr=PCIE1_MAP_IO_BASE;
		memmapaddr=PCIE1_MAP_MEM_BASE;	
	}

	REG32(memmapaddr + 0x3c) |= (1<<8);  //swint mask

	#define SWINT (1<<24)
	#define PCIE0INT (1<<21)
	#define PCIE1INT (1<<22)	

	unsigned int  PCIEISR;
	if(portnum==0)	PCIEISR=PCIE0INT;
	else 			PCIEISR=PCIE1INT;

	
	#define GISR (0xb8003004)
	
	int i;
	for(i=0; i<loops; i++)
	{

		REG32(memmapaddr + 0x38) |= (1<<0);  //kick swint
		
		while(1) 
		{
			if( REG32(memmapaddr + 0x3c) & SWINT)   //check interrupt status swINT=1
				break;
		}

		dprintf("Got   SWINT %x=%x \n",memmapaddr + 0x3c, REG32(memmapaddr + 0x3c) );

		
		while(1) 
		{
			if( REG32(GISR) & PCIEISR )  //check pcie port 0
				break;
		}
		dprintf("Got   GISR=%x \n", REG32(0xb8003004) );


		//-----------------
		REG32(memmapaddr + 0x3c) |= SWINT;  //write to clear



		while(1) 
		{
			if( (REG32(memmapaddr + 0x3c) & SWINT) ==0)  //check interrupt status swINT=1
				break;
		}

		dprintf("Clear SWINT %x=%x \n",memmapaddr + 0x3c, REG32(memmapaddr + 0x3c) );

		
		while(1) 
		{
			if( (REG32(GISR) & PCIEISR )==0)  //check pcie port 0
				break;
		}
		dprintf("Clear GISR=%x \n", REG32(0xb8003004) );
	}
	

	



}

