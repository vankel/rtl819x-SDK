
/*==========================================
The following code is maintain by George.
Do not modify.
Tks.
===========================================*/

#include <linux/interrupt.h>
#include <asm/system.h>
#include "monitor.h"
#include <asm/mipsregs.h>	

#include "test_hw_98c.h"
#include <asm/rtl8198c.h>

#define REG32(reg)	(*(volatile unsigned int *)(reg))


const unsigned int cpu_clksel_table[]={ 450, 500, 550, 600, 650,700,
										750, 800, 850, 900, 950, 1000,										
										1050, 1100, 1150, 1200 };

const unsigned int m2x_clksel_table[]={ 250,270,290,310,330,350,370,565,
										410,430,450,470,490,510,530,550,
										390,580,595,610,625,640,655,670,
										685,700,720,740,755,770,785,800 };

unsigned int cpu_clkdiv_table[]={1, 2, 2, 4};  //be careful here


#define printf dprintf


//=================================================================================
void ShowStrapMsg()
{
	const unsigned char *boot_type_tab[]={ {"SPI3B"}, {"SPI4B"}, {"NFBI"}, {"NAND"}, {"ROM01"}, {"ROM02"}, {"ROM03"}, {"auto"} };
	const unsigned char *dram_type_tab[]={  {"DDR2"}, {"DDR3"} };

	#define GET_BITVAL(v,bitpos,pat) ((v& (pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf	
	#define RANG5 0x1f
	
	unsigned int v=REG32(SYS_HW_STRAP);

	unsigned int bootsel=GET_BITVAL(v, 0, RANG3);
	unsigned int dramtype=GET_BITVAL(v, 3, RANG1);
	

	
	unsigned int ck_m2x_freq_sel=GET_BITVAL(v, 10, RANG5);
	unsigned int ck_cpu_freq_sel=GET_BITVAL(v, 15, RANG4);
	unsigned int ck_cpu_div_sel=GET_BITVAL(v, 19, RANG2);



	
	printf("---------------------\n");
	printf("HW_STRAP_VAL= 0x%08x \n", v);
	printf("[02:00] ST_BOOTPINSEL= 0x%x  \n", bootsel);		
	printf("[03:03] ST_DRAMTYPE= 0x%x      	\n", 	dramtype);	

	printf("[04:04] clklx_from_clkm= 0x%x \n",  	GET_BITVAL(v, 4, RANG1)  );	
	printf("[05:05] disable_ext_reset= 0x%x \n",  	GET_BITVAL(v, 5, RANG1)  );	
	printf("[06:06] ext_phy_mode= 0x%x \n",  	GET_BITVAL(v, 6, RANG1)  );		


	printf("[14:10] CK_M2X_FREQ_SEL= 0x%x \n", ck_m2x_freq_sel);	
	printf("[18:15] ST_CPU_FREQ_SEL= 0x%x \n", ck_cpu_freq_sel);
	
	printf("[20:19] ST_CPU_FREQDIV_SEL= 0x%x \n", ck_cpu_div_sel);

	printf("[21:21] clklx_from_halfoc= 0x%x \n",  	GET_BITVAL(v, 21, RANG1)  );	
	
	printf("[22:22] ever_reboot_once= 0x%x \n", 	GET_BITVAL(v, 22, RANG1)  );	
	printf("[23:23] clkoc_from_clkm= 0x%x \n", 	GET_BITVAL(v, 23, RANG1)  );			
	printf("[24:24] sel_40m= 0x%x \n", 	GET_BITVAL(v, 24, RANG1)  );		

	printf("\n");
	printf("%s mode, %s Ram,  CPU=%d MHz, Mem2x=%d MHz, \n", 
						boot_type_tab[bootsel],	
						dram_type_tab[dramtype],
						cpu_clksel_table[ck_cpu_freq_sel]/cpu_clkdiv_table[ck_cpu_div_sel] ,
						m2x_clksel_table[ck_m2x_freq_sel]
						);
					
}

//=================================================================================

int TestStrapPin(int argc, char* argv[])
{

	if(argc < 2) 
	{	ShowStrapMsg();
		dprintf("Usage: strap bit value \n\n");	
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
	else
	{
		dprintf("Unsupport value \n");	
	}
	ShowStrapMsg();
	
}

//=================================================================================

void HS0_Control(unsigned int ocp, unsigned int lx, unsigned int mx, unsigned int sleep)
{

	#define SYS_BASE 0xb8000000
	#define SYS_INT_STATUS (SYS_BASE +0x04)
	#define SYS_HW_STRAP   (SYS_BASE +0x08)
	#define SYS_LX_CTRL   (SYS_BASE +0x14)
	

	//printf("MxSpdupThanLexra\n");

	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG5  0x1f
	#define RANG4  0x0f


	if(lx==0)	lx=200;
	if(mx==0)
	{
		unsigned int m2x_freq_sel=GET_BITVAL(REG32(SYS_HW_STRAP), CK_M2X_FREQ_SEL_OFFSET, RANG5);
		mx=(m2x_clksel_table[m2x_freq_sel])/2;
	}
 	if(ocp==0)
 	{
		unsigned int cpu_freq_sel=GET_BITVAL(REG32(SYS_HW_STRAP), ST_CPU_FREQ_SEL_OFFSET, RANG4);
		ocp=cpu_clksel_table[cpu_freq_sel];
	}
	


	//-------------------------


	
	#define SYS_HS0_CTRL 0xb80000a0
	#define BIT(x)	(1 << x)	
	unsigned int v=0;
	if(lx<mx)		{	v|=BIT(0)| BIT(1) |BIT(2);   	printf("Lx<Mx\n");		}
	if(ocp<mx)		{	v|=BIT(3);					printf("Ocp<Mx\n");	}
	if(ocp<lx)		{	v|=BIT(4);					printf("Ocp<Lx\n");	}

	
	//REG32(SYS_HS0_CTRL) = v;
	REG32(SYS_HS0_CTRL) |= v;
	


	if(sleep)
	{	
		#if 1			
			//printf("llx0\n");
			REG32(SYS_LX_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_LX_CTRL)&(1<<12))==0)  {}; //wait bit to 1, is mean lock ok	

			//printf("llx1\n");
			//REG32(SYS_BIST_CTRL) |= (1<<3) ;	  //lock bus arb4
			//while( (REG32(SYS_BIST_DONE)&(1<<1))==0)  {}; //wait bit to 1, is mean lock ok		

			//printf("llx2\n");
			//REG32(SYS_BIST_CTRL) |= (1<<4) ;	  //lock bus arb6
			//while( (REG32(SYS_BIST_DONE)&(1<<2))==0)  {}; //wait bit to 1, is mean lock ok				
		#endif
		
		//__asm__ volatile("sleep");	 //need 10 usec to guaretee
		__asm__ volatile("nop");


		#if 1
			//printf("ulx0\n");	
			REG32(SYS_LX_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_LX_CTRL)&(1<<12))==(1<<0)) {};  //wait bit to 0  unlock

			//printf("ulx1\n");
			//REG32(SYS_BIST_CTRL) &= ~(1<<3);	//unlock
			//while( (REG32(SYS_BIST_DONE)&(1<<1))==(1<<1)) {};  //wait bit to 0  unlock

			//printf("ulx2\n");
			//REG32(SYS_BIST_CTRL) &= ~(1<<4);	//unlock
			//while( (REG32(SYS_BIST_DONE)&(1<<2))==(1<<2)) {};  //wait bit to 0  unlock				
		#endif
	}
			//printf("done\n");

}
//==============================================================


static void SPEED_isr(void)
{
	unsigned int isr=REG32(GISR_REG);
	unsigned int cpu_status=REG32(SYS_INT_STATUS);
	
	dprintf("=>CPU Wake-up interrupt happen! GISR=%08x \n", isr);

	if( (isr & (1<<27))==0)   //check isr==1
	{	dprintf("Check Fail, GISR=%x bit %d is not 1\n", isr, 27);
		while(1) ;
	}

	if((cpu_status & (1<<1))==0)  //check source==1
	{	dprintf("Fail, Source=%x bit %d is not 1 \n", cpu_status, 1);
		while(1) ;
	}
		
	REG32(SYS_INT_STATUS)=(1<<1);  //enable cpu wakeup interrupt mask
//	REG32(GISR_REG)=1<<SPEED_IRQ_NO;	//write to clear, but cannot clear


//	REG32(GIMR_REG)= REG32(GIMR_REG) & ~(1<<SPEED_IRQ_NO);	//so, disable interrupt		
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
void CPUWAKEUP(void)
{
	request_IRQ(SPEED_IRQ_NO, &irq_SPEED, NULL); 
}


int CmdCore1Wakeup(int argc, char* argv[])
{
		#define POLLING_REG 0xb800006c
  		#define PATT_SLEEP  0x3333		
  		#define PATT_READY  0x5555

	//Let Core 1 wakeup
	#define GIC_WAKEUP_IRQ 43
	REG32(GIC_BASE_ADDR+0x2000+GIC_WAKEUP_IRQ*0x20)=2;  //map2vpe
	REG32(GIC_BASE_ADDR+0x184)=0x800;  //trg edge type 
		
	REG32(GIC_BASE_ADDR+0x280)=0x80000000 | GIC_WAKEUP_IRQ;  //sw int
//	delay_ms(10);
	REG32(GIC_BASE_ADDR+0x280)=0x00000000 | GIC_WAKEUP_IRQ;	
	REG32(GIC_BASE_ADDR+0x184)=0x0;  //trg edge type 
#if 1
	int i=100;
	while(i--)
	{
		if(REG32(POLLING_REG)!=PATT_SLEEP)
		{	
#ifdef _verbose
			printf("Core 1 Wakeup, ret=%x\n", REG32(POLLING_REG));
#endif
			return;
		}
		//delay_ms(10);
	}
#endif

#ifdef _verbose
	printf("Core 1 cannot Wakeup, ret=%x\n", REG32(POLLING_REG));
#endif
}
//---------------------------------------------------------------------------

int SettingCPUClk(int clk_sel, int clk_div)
{
	int clk_curr, clk_exp;	
	unsigned int old_clk_sel;
	unsigned int mask;
	unsigned int sysreg;

	REG32(SYS_INT_STATUS)=(1<<1);  //clear cpu wakeup.


#ifdef _verbose
	dprintf("\nInput : CLK_SEL=0x%x, DIV=0x%x  \n", clk_sel, clk_div );
#endif

#if 1  //check core 1 exist
		#define POLLING_REG 0xb800006c
  		#define PATT_SLEEP  0x3333		
  		#define PATT_READY  0x5555
	if(REG32(POLLING_REG)!=PATT_SLEEP)
	{
#ifdef _verbose
		printf("Core 1 miss, cannot change freq.\n");
#endif
		return 0;
	}


#endif
	
	clk_curr = check_cpu_speed();
#ifdef _verbose
	dprintf("Now CPU Speed=%d \n",clk_curr);	
#endif
	//----------------------------
	REG32(SYS_INT_STATUS)=(1<<1);  //enable cpu wakeup interrupt mask

	request_IRQ(SPEED_IRQ_NO, &irq_SPEED, NULL); 	

	//-------------
	sysreg=REG32(SYS_HW_STRAP);
	//dprintf("Read  SYS_HW_STRAP=%08x\r\n", sysreg);	
	old_clk_sel=(sysreg & ST_CPU_FREQ_SEL) >>ST_CPU_FREQ_SEL_OFFSET;

	sysreg&= ~(ST_FW_CPU_FREQDIV_SEL);
	sysreg&= ~(ST_CK_CPU_FREQDIV_SEL);	
	sysreg&= ~(ST_CPU_FREQ_SEL);

	sysreg|=  (clk_div & 0x03) <<ST_CPU_FREQDIV_SEL_OFFSET ;
	sysreg|=	 (clk_sel&0x0f)<<ST_CPU_FREQ_SEL_OFFSET ;
	//-------------------------

	clk_exp=cpu_clksel_table[clk_sel] / cpu_clkdiv_table[clk_div];

	
#ifdef _verbose
	dprintf("Write SYS_HW_STRAP=%08x \n", sysreg);
#endif	

	REG32(SYS_HW_STRAP)=sysreg  ;
	//dprintf("Read  SYS_HW_STRAP=%08x \n", REG32(SYS_HW_STRAP));
	
	//--------------
	if(old_clk_sel != clk_sel)
	{
#if 1
	HS0_Control(clk_exp, 0, 0, 0);
#endif	
		REG32(GISR_REG)=0xffffffff;	
		//dprintf("before sleep, Read  SYS_HW_STRAP=%08x \n", REG32(SYS_HW_STRAP));	
		//dprintf("GISR=%08x \n",REG32(GISR_REG));
		//dprintf("GIMR=%08x \n",REG32(GIMR_REG));	

		#if 1	
			REG32(SYS_CLKMANAGE) |= (1<<12)|(1<<13)|(1<<19)|(1<<20);  //active lx1 lx2
		
			REG32(SYS_LX_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_LX_CTRL)&(1<<12))==0)  ; //wait bit to 1, is mean lock ok	

			REG32(SYS_LX_CTRL) |= (1<<3) ;	  //lock bus arb4
			while( (REG32(SYS_LX_CTRL)&(1<<13))==0)  ; //wait bit to 1, is mean lock ok		

			REG32(SYS_LX_CTRL) |= (1<<4) ;	  //lock bus arb6
			while( (REG32(SYS_LX_CTRL)&(1<<14))==0)  ; //wait bit to 1, is mean lock ok				
		#endif
		
		__asm__ volatile("nop");
		__asm__ volatile("nop");
		__asm__ volatile("nop");
		__asm__ volatile("nop");
		__asm__ volatile("wait");	
		__asm__ volatile("nop");

		__asm__ volatile("nop");
		__asm__ volatile("nop");
		__asm__ volatile("nop");
		__asm__ volatile("nop");
		__asm__ volatile("nop");

		#if 1
			REG32(SYS_LX_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_LX_CTRL)&(1<<12))==(1<<0)) ;  //wait bit to 0  unlock

			REG32(SYS_LX_CTRL) &= ~(1<<3);	//unlock
			while( (REG32(SYS_LX_CTRL)&(1<<13))==(1<<1)) ;  //wait bit to 0  unlock

			REG32(SYS_LX_CTRL) &= ~(1<<4);	//unlock
			while( (REG32(SYS_LX_CTRL)&(1<<14))==(1<<2)) ;  //wait bit to 0  unlock				
		#endif

		int strap_new=REG32(SYS_HW_STRAP) ;

	}
	
	//-----------------------
	//test cpu can work
	LetCPUDoSomething();

	//-----------------------
	clk_curr = check_cpu_speed();
#ifdef _verbose
	dprintf("After  Changing, CPU Speed=%d \n",clk_curr);	

	if( (clk_curr >=  clk_exp-1) && (clk_curr <=  clk_exp+1) )  //torrernce 1 MHz
		dprintf("Test PASS!\n");
	else 
	{	dprintf("Test FAIL! Curr_speed=%d but Exp_speed=%d \n", clk_curr, clk_exp);
		//while(1) ;
	}
#endif

}
//---------------------------------------------------------------------------
int CmdCPUCLK(int argc, char* argv[])
{

	int clk_sel=0, clk_div=0;
	int clk_curr;	


	
	int i;
	if( argc <1 )	//read
	{
		clk_curr = check_cpu_speed();
		dprintf("Now CPU Speed=%d \n",clk_curr);			
		ShowStrapMsg();	
		dprintf("Usage: CPUCLK clk_sel div_value : 0-f, 0-3  \n");	
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
//	if(argv[2])	sync_oc = strtoul((const char*)(argv[2]), (char **)NULL, 16);


	if(clk_sel==0x999)
	{
		if(clk_div==0x999)
		{
			for(clk_sel=0; clk_sel<=0x0f; clk_sel++)
				SettingCPUClk(clk_sel, 3);

			for(clk_sel=0; clk_sel<=0x0f; clk_sel++)
				SettingCPUClk(clk_sel, 2);
			
			for(clk_sel=0; clk_sel<=0x0f; clk_sel++)
				SettingCPUClk(clk_sel, 0);
		}
		else
		{
			for(clk_sel=0; clk_sel<=0x0f; clk_sel++)
				SettingCPUClk(clk_sel, clk_div);
		}

			
	}
	else
		SettingCPUClk(clk_sel, clk_div);

}
extern unsigned long glexra_clock;

//---------------------------------------------------------------------------

int SettingM2xClk(int clk_sel)
{
	int	tmp=REG32(SYS_HW_STRAP) & ~(CK_M2X_FREQ_SEL);
	
		#if 1  //lock bus			
			REG32(SYS_LX_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_BIST_DONE)&(1<<0))==0)  ; //wait bit to 1, is mean lock ok	

			REG32(SYS_LX_CTRL) |= (1<<3) ;	  //lock bus arb4
			while( (REG32(SYS_BIST_DONE)&(1<<1))==0)  ; //wait bit to 1, is mean lock ok		

			REG32(SYS_LX_CTRL) |= (1<<4) ;	  //lock bus arb6
			while( (REG32(SYS_BIST_DONE)&(1<<2))==0)  ; //wait bit to 1, is mean lock ok		

			//add check transaction dram empty .
		#endif

	
	REG32(SYS_HW_STRAP)= tmp | (clk_sel) <<CK_M2X_FREQ_SEL_OFFSET ;
		
		#if 1   //check m2xusable and unlock bus
			while( (REG32(SYS_BIST_DONE)&(1<<18))==0)  ;   //wait to 1, mean m2x is usable
	
			REG32(SYS_LX_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<0))==(1<<0)) ;  //wait bit to 0  unlock

			REG32(SYS_LX_CTRL) &= ~(1<<3);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<1))==(1<<1)) ;  //wait bit to 0  unlock

			REG32(SYS_LX_CTRL) &= ~(1<<4);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<2))==(1<<2)) ;  //wait bit to 0  unlock	
		#endif
#if 0
	int	clklx_from_clkm=GET_BITVAL(REG32(SYS_HW_STRAP) ,ST_CLKLX_FROM_CLKM_OFFSET, BIT_RANG1);
	if(clklx_from_clkm==1)
	{
		console_init(glexra_clock);
		timer_init(glexra_clock);
	}
#endif		
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
	
	int	tmp=REG32(SYS_HW_STRAP) & ~(ST_CLKLX_FROM_HALFOC);
	int strap_newval= tmp | ((clklx_from_clkm&0x01) <<ST_CLKLX_FROM_HALFOC_OFFSET) ;

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
			REG32(SYS_LX_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_BIST_DONE)&(1<<0))==0)  ; //wait bit to 1, is mean lock ok	

			REG32(SYS_LX_CTRL) |= (1<<3) ;	  //lock bus arb4
			while( (REG32(SYS_BIST_DONE)&(1<<1))==0)  ; //wait bit to 1, is mean lock ok		

			REG32(SYS_LX_CTRL) |= (1<<4) ;	  //lock bus arb6
			while( (REG32(SYS_BIST_DONE)&(1<<2))==0)  ; //wait bit to 1, is mean lock ok	

			//add check transaction dram empty .
		#endif
	


	//go
	REG32(SYS_HW_STRAP)=strap_newval;    //change lx clk


		#if 1   //check m2xusable and unlock bus
			//while( (REG32(SYS_BIST_DONE)&(1<<18))==0)  ;   //wait to 1, mean m2x is usable
	
			REG32(SYS_LX_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<0))==(1<<0)) ;  //wait bit to 0  unlock

			REG32(SYS_LX_CTRL) &= ~(1<<3);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<1))==(1<<1)) ;  //wait bit to 0  unlock

			REG32(SYS_LX_CTRL) &= ~(1<<4);	//unlock
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
		int	lxsel=(REG32(SYS_HW_STRAP) & (ST_CLKLX_FROM_HALFOC))>>ST_CLKLX_FROM_HALFOC_OFFSET;
		dprintf("status: %d \n", lxsel );		
		return;	
	}

	
	int clklx_from_clkm = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	SettingLxClk(clklx_from_clkm);


	//--------------------------------------


};  

//return position 
unsigned int ExtractACmdLine(const char *pPattScript,  char *pOneCmdString, int first)
{
	//first=1 reset index, to buffer head
	//first=0 continue read a line

	static unsigned int idx=0;
	unsigned char *p=pPattScript+idx;
	int push=0;
	
	if(first==1)
	{	idx=0;
		return 0;
	}
	
	memset( pOneCmdString, 0, MAX_MONITOR_BUFFER );


	int n=0;
	while( *p )
	{
		if(n==0)
		{
			//skip first return-line
			while( *p && ((*p == 0x0d) ||(*p==0x0a) ||(*p=='\t') ||(*p==' ') ) )
				p++;
		}

		if ((n==0) && (*p =='~') )
			return 0;


		//end
		if(n!=0)
		{
			if( (*p == 0x0d)  || (*p == 0x0a)  || (*p == '#'))
			{	pOneCmdString[n] = 0 ;
				break;
			}
			if(*p==0x09) //TAB->SPACE
				*p=' ';
		}
	
		pOneCmdString[n] = *p ;
		n++;		
		p++;	
		if (n == 80) break;
	}
	idx= (int)p-(int)pPattScript+1;

	//thrim last space
	for(;n>1;n--)
		if( (pOneCmdString[n-1]!=' ')  &&  (pOneCmdString[n-1]!='\t') )
		{	pOneCmdString[n]=0;
			break;
		}

	//dprintf("test=> %s \r\n", pOneCmdString);
	return idx;	

}

//=========================================================

//=========================================================
int GPHY_BIST(int argc, char* argv[])
{
	volatile unsigned int phyid=4,rdat=0;
	int i;
	int err=0;

	dprintf("Set P0-P4 force mode...... \n");
	REG32(0xbb804104)= 0x427f0038;
	REG32(0xbb804108)= 0x467f0038;
	REG32(0xbb80410c)= 0x4a7f0038;
	REG32(0xbb804110)= 0x4e7f0038;
	REG32(0xbb804114)= 0x527f0038;
	REG32(0xbb804118)= 0x566f0038;	
	
	//
	rtl8651_setAsicEthernetPHYReg(8, 24, 0x2198 );		
	rtl8651_setAsicEthernetPHYReg(1, 24, 0x2198 );	
	rtl8651_setAsicEthernetPHYReg(2, 24, 0x2198 );	
	rtl8651_setAsicEthernetPHYReg(3, 24, 0x2198 );
	rtl8651_setAsicEthernetPHYReg(4, 24, 0x2198 );	
	
	//
	Set_GPHYWB(0, 0xA40, 0, 0, 0x1140);
	Set_GPHYWB(0, 0xA46, 20, 0, 0x0003);	
//	Set_GPHYWB(0, 0xA4A, 19, 0, 0x001f);	
//	Set_GPHYWB(0, 0xB80, 23, 0, 0x000e);	
	
	//
	rtl8651_setAsicEthernetPHYReg(0, 31,0x0a42 );		
	for(i=0;i<5;i++)
	{
		if(i==0) phyid=8;
		rtl8651_getAsicEthernetPHYReg(phyid, 16, &rdat );
		printf("get data=%x\n", rdat);
		if((rdat&0x7)==	0x3) 	printf("Port %d PCS ready PASS\n",i);
		else 			{ printf("Port %d PCS ready FAIL\n",i); err++; }
	}
	
	//3. m3 bist
	Set_GPHYWB(0, 0xc40, 21, 0, 0xc000);
	Set_GPHYWB(0, 0xA00, 20, 0, 0x0000);	
	rtl8651_setAsicEthernetPHYReg(0, 20, 0x0060 );			
	rtl8651_setAsicEthernetPHYReg(0, 23, 0x0000 );	
	rtl8651_setAsicEthernetPHYReg(0, 23, 0x00a0 );	
	Set_GPHYWB(0, 0xb81, 18, 0, 0x0000);
	rtl8651_setAsicEthernetPHYReg(0, 18, 0x001b );					
	Set_GPHYWB(0, 0xc84, 22, 0, 0x0000);	
	rtl8651_setAsicEthernetPHYReg(0, 22, 0x0005 );	
	
	delay_ms(100);
	printf("\n");
	
	for(i=0;i<5;i++)
	{	
		if(i==0) phyid=8;
		else phyid=i;		
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0a00 );
		rtl8651_getAsicEthernetPHYReg(phyid, 23, &rdat );	
		printf("get data=%x\n", rdat);
		if((rdat&(0xf<<9))== (0x8<<9)) 	printf("Port %d BIST PASS\n",i);
		else 			{ printf("Port %d BIST FAIL\n",i); err++; }
			
			
		//	
		rtl8651_getAsicEthernetPHYReg(phyid, 22, &rdat );	
		printf("get data=%x\n", rdat);
		if(rdat== 0xd279) 	printf("Port %d BIST_ROM [31:16] PASS\n",i);
		else 			{ printf("Port %d BIST_ROM [31:16] FAIL\n",i);	err++; }
			
		//
		rtl8651_getAsicEthernetPHYReg(phyid, 21, &rdat );	
		printf("get data=%x\n", rdat);
		if(rdat== 0xa555) 	printf("Port %d BIST_ROM [15:0] PASS\n",i);
		else 			{ printf("Port %d BIST_ROM [15:0] FAIL\n",i);	err++; }
		
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0b81 );
		rtl8651_getAsicEthernetPHYReg(phyid, 18, &rdat );				
		printf("get data=%x\n", rdat);
		if((rdat&(0xf<<12))== (0x1<<12)) 	printf("Port %d GPHY BIST PASS\n",i);
		else 			{		printf("Port %d GPHY BIST FAIL\n",i);	err++; }
			
			
		rtl8651_getAsicEthernetPHYReg(phyid, 19, &rdat );				
		printf("get data=%x\n", rdat);
		if(rdat== 0x2c34) 	printf("Port %d GPHY BIST_ROM PASS\n",i);
		else 			{ printf("Port %d GPHY BIST_ROM FAIL\n",i);	err++; }
			
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0c84 );
		rtl8651_getAsicEthernetPHYReg(phyid, 23, &rdat );				
		printf("get data=%x\n", rdat);
		if((rdat&(0x7<<0))== (0x4<<0)) 	printf("Port %d GPHY BIST PASS\n",i);
		else 			{	printf("Port %d GPHY BIST FAIL\n",i);	err++; }
					
						
	}	
	
	if(err==0)
		printf("==> GPHY BIST ALL PASS <== \n");
	else
		printf("==> GPHY BIST FAIL count=%d <== \n", err);				
}
//=============================================================================
int GPHY_DRF_BIST(int argc, char* argv[])
{
	volatile unsigned int phyid=4,rdat=0;
	int i;
	int err=0;

//	dprintf("Set P0-P4 force mode...... \n");
	REG32(0xbb804104)= 0x427f0038;
	REG32(0xbb804108)= 0x467f0038;
	REG32(0xbb80410c)= 0x4a7f0038;
	REG32(0xbb804110)= 0x4e7f0038;
	REG32(0xbb804114)= 0x527f0038;
	REG32(0xbb804118)= 0x566f0038;
	
//	void Set_GPHYWB(unsigned int phyid, unsigned int page, unsigned int reg, unsigned int mask, unsigned int val)
//	Set_GPHYWB(8, 24, 22, 0, 0x5bd5);
	
		
	Set_GPHYWB(0, 0xc40, 21, 0, 0xc000);
	Set_GPHYWB(0, 0xa00, 23, 0, 0xc000);

	rtl8651_setAsicEthernetPHYReg(0, 23, 0x0120 );		
	
	//
	Set_GPHYWB(0, 0xb81, 18, 0, 0x0000);
	rtl8651_setAsicEthernetPHYReg(0, 18, 0x0005 );		

	Set_GPHYWB(0, 0xc84, 22, 0, 0x0000);			
	rtl8651_setAsicEthernetPHYReg(0, 22, 0x0007 );		


	delay_ms(1000);

	
	//resume
	Set_GPHYWB(0, 0xa00, 23, 0xffff, 0x0040);
	Set_GPHYWB(0, 0xa00, 23, 0xffbf, 0x0000);
	Set_GPHYWB(0, 0xb81, 18, 0xffff, 0x0400);
	Set_GPHYWB(0, 0xc84, 23, 0xffff, 0x1000);			
	
	
	delay_ms(1000);
	Set_GPHYWB(0, 0xa00, 23, 0xffff, 0x0040);
	Set_GPHYWB(0, 0xa00, 23, 0xffbf, 0x0000);
	Set_GPHYWB(0, 0xb81, 18, 0xffff, 0x0400);
	Set_GPHYWB(0, 0xc84, 23, 0xffff, 0x1000);
	
	delay_ms(1000);
	

	
	for(i=0;i<5;i++)
	{	
		if(i==0) phyid=8;
		else phyid=i;
		
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0a00 );
		rtl8651_getAsicEthernetPHYReg(phyid, 23, &rdat );	
		printf("get data=%x\n", rdat);
		
		if((rdat&(0xf<<9))== (0x4<<9)) 	printf("Port %d BIST PASS\n",i);
		else 			{ printf("Port %d BIST FAIL\n",i);  err++; }
			
			
		//
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0b81 );
		rtl8651_getAsicEthernetPHYReg(phyid, 18, &rdat );	
		printf("get data=%x\n", rdat);
		
		if((rdat&(0xf<<12))== (0x4<<12)) 	printf("Port %d BIST PASS\n",i);
		else 			{ printf("Port %d BIST FAIL\n",i);	err++; }
			
		//
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0c84 );
		rtl8651_getAsicEthernetPHYReg(phyid, 23, &rdat );	
		printf("get data=%x\n", rdat);
		
		if((rdat&(0x7<<0))== (0x4<<0)) 	printf("Port %d BIST PASS\n",i);
		else 			{ printf("Port %d BIST FAIL\n",i);	err++; }
		
	}	
	
	Set_GPHYWB(0, 0xc40, 21, 0, 0x0000);

	
	//=================
	if(err==0)
		printf("==> GPHY BIST ALL PASS <== \n");
	else
		printf("==> GPHY BIST FAIL count=%d <== \n", err);	
			
}
		
		
//=============================================================================
int Cmd_AllBistTest(int argc, char* argv[])
{
	int err=0;

	#define CLK_MANAGER  0xb8000010

	#define BIST_CTRL  0xb8000200

	#define HS0_BIST_CTRL  0xb8000208
	#define HS0_BIST_CTRL2  0xb800020c

	#define BIST_DONE  	0xb8000210

	#define HS0_BIST_DONE  0xb8000218


	#define BIST_FAIL  	0xb8000220

	#define HS0_BIST_FAIL1  0xb8000230
	#define HS0_BIST_FAIL2  0xb8000234
	#define HS0_BIST_FAIL3  0xb8000238
	#define HS0_BIST_FAIL4  0xb800023c
	#define HS0_BIST_FAIL5  0xb8000240
	#define HS0_BIST_FAIL6  0xb8000244

	#define DRF_PAUSE   	0xb8000270
	#define HS0_DRF_PAUSE   0xb8000278
	#define DRF_RESUME 	0xb8000280
	#define HS0_DRF_RESUME  0xb8000288
	#define DRF_DONE    	0xb8000290
	#define HS0_DRF_DONE    0xb8000298

	#define DRF_FAIL    	 0xb80002a0
	#define HS0_DRF_FAIL1    0xb80002b0
	#define HS0_DRF_FAIL2    0xb80002b4
	#define HS0_DRF_FAIL3    0xb80002b8
	#define HS0_DRF_FAIL4    0xb80002bc
	#define HS0_DRF_FAIL5    0xb80002c0
	#define HS0_DRF_FAIL6    0xb80002c4


	printf( "========================\n");
	printf( "Mode 1 BIST : cpu1 mbr \n");
	REG32(HS0_BIST_CTRL) = 0;
		printf( "W:HS0_BIST_CTRL=%08x \n", REG32(HS0_BIST_CTRL) );

#if 0		
	REG32(HS0_BIST_CTRL) |=  (0x7f);
#else
	REG32(HS0_BIST_CTRL) |=  (0x7c);  //skip mbr0, mbr1
#endif
		printf( "W:HS0_BIST_CTRL=%08x \n", REG32(HS0_BIST_CTRL) );
	
	REG32(HS0_BIST_CTRL2) = 0;
		printf( "W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
		
	REG32(HS0_BIST_CTRL2) |=  (0x1e0103ff);
		printf( "W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	

	delay_ms(10);

	printf( "\n");
	printf( "R:HS0_BIST DONE=%08x \n",  REG32(HS0_BIST_DONE) );
	printf( "R:HS0_BIST FAIL1=%08x \n", REG32(HS0_BIST_FAIL1) );
	printf( "R:HS0_BIST FAIL2=%08x \n", REG32(HS0_BIST_FAIL2) );
	printf( "R:HS0_BIST FAIL3=%08x \n", REG32(HS0_BIST_FAIL3) );
	printf( "R:HS0_BIST FAIL4=%08x \n", REG32(HS0_BIST_FAIL4) );
	printf( "R:HS0_BIST FAIL5=%08x \n", REG32(HS0_BIST_FAIL5) );
#if 0	
	if ( (REG32(HS0_BIST_DONE)&0x3ff) != 0x3ff) { printf( " ==>DONE FAIL \n"); }
	if ( REG32(HS0_BIST_FAIL1) != 0) { printf( " ==>FAIL1 FAIL \n"); }	
	if ( REG32(HS0_BIST_FAIL2) != 0) { printf( " ==>FAIL2 FAIL \n"); }	
	if ( REG32(HS0_BIST_FAIL3) != 0) { printf( " ==>FAIL3 FAIL \n"); }	
	if ( REG32(HS0_BIST_FAIL4) != 0) { printf( " ==>FAIL4 FAIL \n"); }	
	if ( REG32(HS0_BIST_FAIL5) != 0) { printf( " ==>FAIL5 FAIL \n"); }	

	if ( ((REG32(HS0_BIST_DONE)&0x3ff)==0x3ff) &&
		  (REG32(HS0_BIST_FAIL1)==0)&&
		  (REG32(HS0_BIST_FAIL2)==0)&&
		  (REG32(HS0_BIST_FAIL3)==0)&&
		  (REG32(HS0_BIST_FAIL4)==0)&&
		  (REG32(HS0_BIST_FAIL5)==0)  )
		  { printf( " ==>BIST PASS \n"); }	 
#endif		
	//============================================
	
	printf( "========================\n");
	printf( "Mode 2 BIST : L2,SRAM,ROM,CPU2 \n");

	printf( "R:HS0_BIST FAIL6=%08x \n", REG32(HS0_BIST_FAIL6));

	if ( (REG32(HS0_BIST_DONE)&(0x79<<10)) != (0x79<<10)) { printf( " ==>DONE FAIL \n"); }	
	if (  REG32(HS0_BIST_FAIL6) != 0) { printf( " ==>FAIL FAIL \n"); }	

	if ((( REG32(HS0_BIST_DONE)&(0x79<<10)) == (0x79<<10)) && 
		 ( REG32(HS0_BIST_FAIL6) == 0) )
	{ printf( " ==>BIST PASS \n"); }
	else
	{ printf( " ==>BIST FAIL \n"); err++; }		
	//============================================
	printf( "========================\n");
	printf( "Mode 3 BIST : NAND,FFT,SATA, PCS ROM, PCS RAM, USB3, OTG, PCIE(ep10),VOIP\n");
	REG32(BIST_CTRL) = 0;
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL));
			
	REG32(BIST_CTRL) |=  (0x01);
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL));

	REG32(BIST_CTRL) |= (0x03ff0001);
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL   )); 

	delay_ms(10);
	printf( "\n");
	printf( "R:BIST DONE=%08x \n", REG32(BIST_DONE));
	printf( "R:BIST FAIL=%08x \n", REG32(BIST_FAIL));


	
	if ( (REG32(BIST_DONE) == 0xffff3fff) && 
		( REG32(BIST_FAIL) == 0x00000000)) { printf( " ==>BIST PASS \n"); }	 
	else 								   
	{ 	printf( " ==>BIST FAIL \n"); err++;
		if ( REG32(BIST_DONE) != 0xffff3fff) { printf( " ==>DONE FAIL \n"); }
		if ( REG32(BIST_FAIL) != 0x00000000) { printf( " ==>FAIL FAIL \n"); }	
	}	
		
	//============================================
	printf( "============================== \n");
	printf( "Mode 4 BIST :switch\n");
	REG32(BIST_CTRL) = 0;
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );	
	REG32(BIST_CTRL) |=  (0x01);
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );
	REG32(BIST_CTRL) |=  (1<<26);
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );
	    
	delay_ms(10);  
	printf( "\n");
	printf( "R:BIST DONE=%08x \n", REG32(BIST_DONE));
	printf( "R:BIST FAIL=%08x \n", REG32(BIST_FAIL));


	
	if ( (REG32(BIST_DONE) == 0xffff4418) && 
		( REG32(BIST_FAIL) == 0x00000000)) { printf( " ==>BIST PASS \n"); }	 
	else								   
	{ 	printf( " ==>BIST FAIL \n"); err++;
		if ( REG32(BIST_DONE) != 0xffff4418) { printf( " ==>DONE FAIL \n"); }
		if ( REG32(BIST_FAIL) != 0x00000000) { printf( " ==>FAIL FAIL \n"); }	
	}	 
	//============================================
	printf( "============================== \n");
	printf( "Mode 5 BIST :switch bist-r \n");
	REG32(BIST_CTRL) = 0;
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );		
	REG32(BIST_CTRL) |=  (0x01);
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );
	REG32(BIST_CTRL) |= (1<<27);
		printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );
	    
	delay_ms(10);   
	printf( "\n");
	printf( "R:BIST DONE=%08x \n", REG32(BIST_DONE) );
	printf( "R:BIST FAIL=%08x \n", REG32(BIST_FAIL) );


	
	if ( (REG32(BIST_DONE) == 0xffff8418) && ( REG32(BIST_FAIL) == 0x00000000)) { printf( " ==>BIST PASS \n"); }	 
	else 							
	{ 	printf( " ==>BIST FAIL \n");  err++;
		if ( REG32(BIST_DONE) != 0xffff8418) { printf( " ==>DONE FAIL \n"); }
		if ( REG32(BIST_FAIL) != 0x00000000) { printf( " ==>FAIL FAIL \n"); }	
	}	 

	//goto end
	//============================================
#if 0	//skip mbr drf
	printf( "============================== \n");
	printf( "Mode 6 DRF_BIST TEST : mbr \n");
	printf( "1.bist_r \n");
	REG32(HS0_BIST_CTRL2) = 0;
		printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	REG32(HS0_BIST_CTRL2) |=  (0x00060000);
		printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	delay_ms(10);
	printf( "\n");
	printf( " R:HS0_BIST_DONE=%08x \n", REG32(HS0_BIST_DONE));
	if ( REG32(HS0_BIST_DONE) == 0xffff1800) { printf( " ==>DONE PASS \n"); }
	if ( REG32(HS0_BIST_DONE) != 0xffff1800) { printf( " ==>DONE FAIL \n"); }	

	printf( "2.second run \n");

		printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	REG32(HS0_BIST_CTRL2) = 0x00180000;
		printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	REG32(HS0_BIST_CTRL2) = 0x001e0000;
		printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );    
	    
	delay_ms(10);
	printf( "\n");
	printf( " R:HS0_BIST_DONE=%08x \n", REG32(HS0_BIST_DONE));
	if ( REG32(HS0_BIST_DONE) == 0xffff1800) { printf( " ==>DONE PASS \n"); }
	if ( REG32(HS0_BIST_DONE) != 0xffff1800) { printf( " ==>DONE FAIL \n"); }	

	printf( " R:HS0_BIST_FAIL6=%08x \n", REG32(HS0_BIST_FAIL6));
	//printf( "R:HS0_BIST_FAIL6[12:5]=%08x \n", REG32(HS0_BIST_FAIL6
	if ( (REG32(HS0_BIST_DONE) == 0xffff1800) && ( REG32(HS0_BIST_FAIL6) == 0)) 
		{ printf( " ==>BIST PASS \n"); }	 
	//-------------
	printf( "3.remap\n");
	REG32(HS0_BIST_CTRL2) = 0x00600000;
		printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	REG32(HS0_BIST_CTRL2) = 0x00ff0000;
		printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) ); 
	REG32(HS0_BIST_CTRL2) = 0x0e6703ff;
		printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) ); 
	    
	printf( "4.pause\n");    
	printf( " R:HS0_DRF_PAUSE=%08x \n", REG32(HS0_DRF_PAUSE ));   
	if ( REG32(HS0_DRF_PAUSE) == 0xffffffff) { printf( " ==>PAUSE PASS \n"); }
	if ( REG32(HS0_DRF_PAUSE) != 0xffffffff) { printf( " ==>PAUSE FAIL \n"); }

	printf( "5.resume\n");   
	REG32(HS0_DRF_RESUME) = 0xffffffff;
		printf( " W:HS0_DRF_RESUME=%08x \n", REG32(HS0_DRF_RESUME) );   

	printf( "6.two pause\n");      
	printf( "R:HS0_DRF_PAUSE=%08x \n", REG32(HS0_DRF_PAUSE ));     
	if ( REG32(HS0_DRF_PAUSE) == 0xffffffff) { printf( " ==>PAUSE PASS \n"); }
	if ( REG32(HS0_DRF_PAUSE) != 0xffffffff) { printf( " ==>PAUSE FAIL \n"); }
	    
	printf( "7.two pause\n");      
	printf( " R:HS0_DRF_DONE=%08x \n", REG32(HS0_DRF_DONE ) );
	printf( " R:HS0_DRF_FAIL1=%08x \n", REG32(HS0_DRF_FAIL1) );
	printf( " R:HS0_DRF_FAIL2=%08x \n", REG32(HS0_DRF_FAIL2) );
	printf( " R:HS0_DRF_FAIL3=%08x \n", REG32(HS0_DRF_FAIL3) );
	printf( " R:HS0_DRF_FAIL4=%08x \n", REG32(HS0_DRF_FAIL4) );
	printf( " R:HS0_DRF_FAIL5=%08x \n", REG32(HS0_DRF_FAIL5) );
	printf( " R:HS0_DRF_FAIL6=%08x \n", REG32(HS0_DRF_FAIL6) );

	if ( REG32(HS0_DRF_DONE) == 0xffffffff) { printf( " ==>DRF DONE PASS \n"); }
	if ( REG32(HS0_DRF_DONE) != 0xffffffff) { printf( " ==>DRF DONE FAIL \n"); }
	    
	if ( REG32(HS0_DRF_FAIL1) != 0) { printf( " ==>FAIL1 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL2) != 0) { printf( " ==>FAIL2 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL3) != 0) { printf( " ==>FAIL3 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL4) != 0) { printf( " ==>FAIL4 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL5) != 0) { printf( " ==>FAIL5 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL6) != 0) { printf( " ==>FAIL6 FAIL \n"); }

	if ( (REG32(HS0_DRF_DONE)==0xffffffff) &&
		(REG32(HS0_DRF_FAIL1)==0)&&
		(REG32(HS0_DRF_FAIL2)==0)&&
		(REG32(HS0_DRF_FAIL3)==0)&&
		(REG32(HS0_DRF_FAIL4)==0)&&
		(REG32(HS0_DRF_FAIL5)==0)&&
		(REG32(HS0_DRF_FAIL6)==0)  )
		{ printf( " ==>DRF PASS \n"); }	 
	printf( "============================== \n");	    
#endif
		
		
	printf( "============================== \n");
	printf( "Mode x DRF_BIST TEST : IP \n");
	REG32( 0xb800020c)=0x001e0000;
	REG32( 0xb8000200)=0x00000002;
//	REG32( 0xb8000208)=0x007f007f;   //skip mbr
	//
	REG32( 0xb8000200)=0x0fff0002;


	printf( "4.pause\n");    
	printf( " R:DRF_PAUSE=%08x \n", REG32(DRF_PAUSE    ));
//	if ( REG32(DRF_PAUSE) == 0xffffffff) { printf( " ==>PAUSE PASS \n"); }
//	if ( REG32(DRF_PAUSE) != 0xffffffff) { printf( " ==>PAUSE FAIL \n"); }
#if 0	//skip mbr     
	printf( " R:HS0_DRF_PAUSE=%08x \n", REG32(HS0_DRF_PAUSE ));
	if ( REG32(HS0_DRF_PAUSE) == 0xffffffff) { printf( " ==>PAUSE PASS \n"); }
	if ( REG32(HS0_DRF_PAUSE) != 0xffffffff) { printf( " ==>PAUSE FAIL \n"); }
#endif


	printf( "5.resume\n");   
	REG32(DRF_RESUME) = 0xffffffff;
		printf( " W:DRF_RESUME=%08x \n", REG32(DRF_RESUME) );    

	printf( "6.two pause\n");      
	printf( " R:DRF_PAUSE=%08x \n", REG32(DRF_PAUSE) );    
//	if ( REG32(DRF_PAUSE) == 0xffffffff) { printf( " ==>PAUSE PASS \n"); }
//	if ( REG32(DRF_PAUSE) != 0xffffffff) { printf( " ==>PAUSE FAIL \n"); }
	
#if 0 //skip mbr	 
	printf( " R:HS0_DRF_PAUSE=%08x \n", REG32(HS0_DRF_PAUSE ));
	if ( REG32(HS0_DRF_PAUSE) == 0xffffffff) { printf( " ==>PAUSE PASS \n"); }
	if ( REG32(HS0_DRF_PAUSE) != 0xffffffff) { printf( " ==>PAUSE FAIL \n"); }
#endif	    
	printf( "7.two resume\n");    
	REG32(DRF_RESUME) = 0xffffffff;
		printf( " W:DRF_RESUME=%08x \n", REG32(DRF_RESUME ) ); 
	  
	printf( " R:DRF_DONE=%08x \n", REG32(DRF_DONE) );
	printf( " R:DRF_FAIL=%08x \n", REG32(DRF_FAIL) );



	if ( (REG32(DRF_DONE)==0xffffffff) &&( REG32(DRF_FAIL)==0) ) 
		{ printf( " ==>DRF PASS \n"); }	 
	else	
	{ 
		printf( " ==>DRF FAIL \n");  err++;
		if ( REG32(DRF_DONE) != 0xffffffff) { printf( " ==>DRF DONE FAIL \n"); }	    
		if ( REG32(DRF_FAIL) != 0) { printf( " ==>FAIL FAIL \n"); }		
	}	


	if(err==0)
		printf("==> IP BIST ALL PASS <== \n");
	else
		printf("==> IP BIST FAIL count=%d <== \n", err);		

}

