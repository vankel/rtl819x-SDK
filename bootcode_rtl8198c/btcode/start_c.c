

#include "../autoconf.h"
#include "start.h"

#include "bspchip.h"
#include "memctl.h"
#define HZ 100

unsigned int  RTK_ZQ_K();

void EnableIP_PADControl(unsigned int dramtype);
void LookUp_MemTimingTable(unsigned int dramtype, unsigned int dramsize, unsigned int m2xclk);

void _DTR_DDR2_MRS_setting(unsigned int *sug_dtr, unsigned int *mr);

unsigned int board_DRAM_freq_mhz(void);
unsigned int _get_DRAM_csnum(void);

void memctlc_ddr2_dll_reset(void);
void memctlc_ddr3_dll_reset(void);

#define REG32(reg)	(*(volatile unsigned int *)(reg))
#define REG(reg)                      (*((volatile unsigned int *)(reg)))
#define DBG 1


//#define _memctl_debug_printf printf
#define _memctl_debug_printf(...)


#define CHECK_DCR_READY()   { while(REG32(DCR) & 1)  {}; }   // 1=busy, 0=ready

#define JUMPADDR 0xa0100000

#define BSP_WDTCNR 0xB800311C


#define WDTCNR 0xB800311C 

#define _WDTCNR_			WDTCNR


#define _WDTKICK_			(1 << 23)

#define _WDTSTOP_			(0xA5f00000)
#define BSP_WDTCNR       BSP_WDTCNR



static __inline__ void watchdog_stop(void)
{
	*((volatile unsigned long *)_WDTCNR_) |= _WDTKICK_;

	*((volatile unsigned long *)_WDTCNR_) = _WDTSTOP_;

}

static __inline__ void watchdog_resume()
{
	*((volatile unsigned long *)_WDTCNR_) |= _WDTKICK_;
}


static __inline__ void watchdog_kick(void)
{
	*((volatile unsigned long *)_WDTCNR_) |= _WDTKICK_;
}
//---------------------------------------------------
#if DBG
void uart_outc(char c)
{
#define UART_WAIT_BREAK 0
#if UART_WAIT_BREAK
  	int i=0;
#endif

    while (1)
	{
#if UART_WAIT_BREAK	
		i++;		
		if (i >=3210)
			break;
#endif	
		if 	(REG32(UART_LSR) & 0x60000000)
			break;	
	}

	//for(i=0; i<0xff00;i++);
 	REG32(UART_THR)= (unsigned int)(c)<<24;  

	if (c == 0x0a)
		REG32(UART_THR)= (unsigned int)(0x0d)<<24;  
}
#endif
//-----------------------------------------------------
inline unsigned char uart_inc(void)
{
#define UART_WAIT_BREAK 0
#if UART_WAIT_BREAK
  	unsigned int i=0;
#endif
	unsigned register  ch;

    while (1)
	{
		if 	(REG32(UART_LSR) & (1<<24) )
			break;	
		
#if UART_WAIT_BREAK	
		i++;		
		if (i >=6540)
			break;
#endif			
	}	
	ch=REG32(UART_RBR);
	ch=ch>>24;
	return ch;
}
//-----------------------------------------------------
unsigned int kbhit(unsigned int loops)
{	unsigned int i=loops;
	while(loops--)
	{
		if(REG32(UART_LSR) & (1<<24) )
			return 1;
	}
	return 0;
}
#if 0
void console_init(void)
{
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


	unsigned long dl;
	unsigned long dll;     
	unsigned long dlm;       
           
  	REG32( UART_LCR)=0x03000000;		//Line Control Register  8,n,1
  		
  	REG32( UART_FCR)=0xc7000000;		//FIFO Ccontrol Register
  	REG32( UART_IER)=0x00000000;
  	dl = (200000000 /16)/38400-1;
  	dll = dl & 0xff;
  	dlm = dl / 0x100;
  	REG32( UART_LCR)=0x83000000;		//Divisor latch access bit=1
  	REG32( UART_DLL)=dll*0x1000000;
   	REG32( UART_DLM)=dlm*0x1000000; 
   	REG32( UART_LCR)=0x83000000& 0x7fffffff;	//Divisor latch access bit=0

}
#endif
#if 0
uart1_init(unsigned int portnum, unsigned long baudrate)
{
	unsigned long lexraclock=40*1000*1000;
	unsigned int UART_BASE;

	if(portnum==0)		 UART_BASE=  0xB8000000;
	else					UART_BASE  =  0xB8000000+0x100;
		
	#define UART_RBR_REG	(0x2000+UART_BASE)
	#define UART_THR_REG	(0x2000+UART_BASE)
	#define UART_DLL_REG	(0x2000+UART_BASE)
	#define	UART_IER_REG	(0x2004+UART_BASE)	
	#define	UART_DLM_REG	(0x2004+UART_BASE)
	#define	UART_IIR_REG	(0x2008+UART_BASE)
	#define	UART_FCR_REG	(0x2008+UART_BASE)
	#define UART_LCR_REG	(0x200c+UART_BASE)
	#define	UART_MCR_REG	(0x2010+UART_BASE)
	#define	UART_LSR_REG	(0x2014+UART_BASE)
	#define	UART_MSR_REG	(0x2018+UART_BASE)
	#define	UART_SCR_REG	(0x201c+UART_BASE)
	
  
        unsigned long dl;
        unsigned long dll;     
        unsigned long dlm;       
        
        
       // *(volatile unsigned long *)(0xb8001004) = 0xffffff00 ;		//Memory Control Register
           
  	REG32(UART_LCR_REG)=0x03000000;		//Line Control Register  8,n,1
  		
  	REG32( UART_FCR_REG)=0xc7000000;		//FIFO Ccontrol Register
  	REG32( UART_IER_REG)=0x00000000;
  	dl = (lexraclock /16)/baudrate-1;
  	*(volatile unsigned long *)(0xa1000000) = dl ; 
  	dll = dl & 0xff;
  	dlm = dl / 0x100;
  	REG32( UART_LCR_REG)=0x83000000;		//Divisor latch access bit=1
  	REG32( UART_DLL_REG)=dll*0x1000000;
   	REG32( UART_DLM_REG)=dlm*0x1000000; 
    	REG32( UART_LCR_REG)=0x83000000& 0x7fffffff;	//Divisor latch access bit=0

	REG32( UART_THR_REG)=('A' <<24);	
   	
}
#endif
#if 0
void Put_UartData(char c)
{
#define IO_BASE         0xB8002000
#define UART_LSR	(0x14+IO_BASE)
#define UART_THR	(0x00+IO_BASE)


#define UART_WAIT_BREAK 1
#if UART_WAIT_BREAK
  	int i=0;
#endif

    while (1)
	{
		if 	(REG32(UART_LSR) & 0x60000000)
			break;		
#if UART_WAIT_BREAK	
		i++;		
		if (i >=3210)
			break;
#endif	

	}

	//for(i=0; i<0xff00;i++);
 	REG32(UART_THR)= (unsigned int)(c)<<24;  

}
#else
#define Put_UartData uart_outc
#endif
inline volatile unsigned int Check_UartRxDataRdy()
{
//	#define IO_BASE         0xB8002000
//	#define	UART_LSR	(0x14+IO_BASE)

	if 	(REG32(UART_LSR) & (1<<24) )
		return 1;
	else
		return 0;
}

inline volatile unsigned int Get_UartData()
{
//	#define IO_BASE         0xB8002100
//	#define UART_RBR	(0x00+IO_BASE)
	
	return REG32(UART_RBR)>>24;
}

inline volatile int Get_UartData_timeout(unsigned char *c, unsigned int  timeout)
{	
	unsigned int t=timeout;

	while(t--)
	{
		if(Check_UartRxDataRdy())
		{	*c=Get_UartData();
			return 1;
		}

	}
	return 0;
}

//-----------------------------------------------------
int vsprintf(char *buf, const char *fmt, const int *dp)
{
	#define putchar uart_outc
	char *p, *s;

	s = buf;
	for ( ; *fmt != '\0'; ++fmt) 
	{
		if (*fmt != '%') 
		{
			buf ? *s++ = *fmt : putchar(*fmt);
			continue;
		}
		if (*++fmt == 's') 
		{
			for (p = (char *)*dp++; *p != '\0'; p++)
				buf ? *s++ = *p : putchar(*p);
		}
		else 
		{	/* Length of item is bounded */
			char tmp[20], *q = tmp;
			int alt = 0;
			int shift = 28;

#if 1   //wei patch for %02x
			if ((*fmt  >= '0') && (*fmt  <= '9'))
			{
				int width;
				unsigned char fch = *fmt;
		                for (width=0; (fch>='0') && (fch<='9'); fch=*++fmt)
		                {    width = width * 10 + fch - '0';
		                }
				  shift=(width-1)*4;
			}
#endif
		
			if (*fmt  == 'x') 
			{
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				const long *lp = (const long *)dp;
				long h = *lp++;
				int ncase = (*fmt & 0x20);
				dp = (const int *)lp;
#if 0				
				if (alt) 
				{
					*q++ = '0';
					*q++ = 'x' | ncase;
				}
#endif				
				for ( ; shift >= 0; shift -= 4)					
					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
				
			}
			else if (*fmt == 'd') 
			{
				int i = *dp++;
				char *r;
				if (i < 0) 
				{
					*q++ = '-';
					i = -i;
				}
				p = q;		/* save beginning of digits */
				do 
				{
					*q++ = '0' + (i % 10);
					i /= 10;
				} while (i);
				/* reverse digits, stop in middle */
				r = q;		/* don't alter q */
				while (--r > p) 
				{
					i = *r;
					*r = *p;
					*p++ = i;
				}
			}			
			else if (*fmt == 'c')
				*q++ = *dp++;
			else
				*q++ = *fmt;
			/* now output the saved string */
			for (p = tmp; p < q; ++p)
				buf ? *s++ = *p : putchar(*p);
		}
	}
	if (buf)
		*s = '\0';
	return (s - buf);
}
//--------------------------------------------------
void printf(const char *fmt, ...)
{	
	(void)vsprintf(0, fmt, ((const int *)&fmt)+1);	
}

//--------------------------------------------------

//==========================================

#if 0
#define SDR_DTR_TAB_ROW  6
#define SDR_DTR_TAB_COL  6

const unsigned int SDR_DTR_TAB[][SDR_DTR_TAB_COL]={  
		0,		312,			287,			262,			237,			212,
		2,		0x22302659,	0x22302219,	0x22281FD1,  0x22201F91,	0x11201B11,
		8,		0x22302659,	0x22302219,	0x22281FD1,  0x22201F91,	0x11201B11,
		16,		0x22302659,	0x22302219,	0x22281FD1,  0x22201F91,	0x11201B11,
		32,		0x22302651,	0x22302211,	0x22281FC9,	0x22201F89,	0x11201B09,
		64,		0x22302651,	0x22302211,	0x22281FC9,	0x22201F89,	0x11201B09,
							};


//--------------------------------------------------
#define DDR1_DTR_TAB_ROW  5
#define DDR1_DTR_TAB_COL   (8+1)

const unsigned int DDR1_DTR_TAB[][DDR1_DTR_TAB_COL]={		
      	0,      387,         	362,         	337,        	 312,        	 287,         	262,         	237,         	212,
	16, 	0x443836da,  0x443832da,  0x44302e9a,  0x44302a5a,  0x44282a1a,  0x332827d1,  0x33202391,  0x33201f11,
	32, 	0x443836d2,  0x443832d2,  0x44302e92,  0x44302a52,  0x44282a12,  0x332827c9,  0x33202389,  0x33201f09,
	64, 	0x443836d2,  0x443832d2,  0x44302e92,  0x44302a52,  0x44282a12,  0x332827c9,  0x33202389,  0x33201f09,
	128, 0x44385ed2,  0x443856d2,  0x44305292,  0x44304a52,  0x44284612,  0x33283fc9,  0x33203b89,  0x33203309,
									};




//--------------------------------------------------

#define DDR2_DTR_TAB_ROW  5
#define DDR2_DTR_TAB_COL   (16+1)

const unsigned int DDR2_DTR_TAB[][DDR2_DTR_TAB_COL]={	
        0,		775,         725,         	675,         	625,         	575,         	525,         	475,         	425,         	387,        	 362,        	 337,        	 312,       	  287,      	   262,         237,         212,
	16,  	0x778876e5,  0x77806ee5,  0x777866a5,  0x66705e64,  0x66605624,  0x55584fdb,  0x5550479b,  0x55483f1b,  0x44403ada,  0x444036da,  0x4438329a,  0x44382e5a,  0x44302a1a,  0x332827d1,  0x33282391,  0x33201f11,
	32,  	0x778876dd,  0x77806edd,  0x7778669d,  0x66705e5c,  0x6660561c,  0x55584fd3,  0x55504793,  0x55483f13,  0x44403ad2,  0x444036d2,  0x44383292,  0x44382e52,  0x44302a12,  0x332827c9,  0x33282389,  0x33201f09,
	64,  	0x7788a2dd,  0x77809add,  0x77788e9d,  0x6670825c,  0x66607a1c,  0x55586fd3,  0x55506393,  0x55485b13,  0x444052d2,  0x44404ed2,  0x44384692,  0x44384252,  0x44303e12,  0x332837c9,  0x33283389,  0x33202f09,
	128, 0x7788c6dd,  0x7780badd,  0x7778ae9d,  0x66709e5c,  0x6660921c,  0x555887d3,  0x55507b93,  0x55486f13,  0x444062d2,  0x44405ed2,  0x44385692,  0x44384e52,  0x44304a12,  0x332843c9,  0x33283f89,  0x33203709,
	//256,  0x778932dd,  0x77811edd,  0x77790a9d,  0x6670f65c,  0x6660e21c,  0x5558cfd3,  0x5550bb93,  0x5548a713,  0x44409ad2,  0x44408ed2,  0x44388692,  0x44387a52,  0x44307212,  0x332867c9,  0x33285f89,  0x33205309,
	//512,  0x7779fadd,  0x7771dadd,  0x7769ba9d,  0x66619a5c,  0x66597a1c,  0x555157d3,  0x55493793,  0x55411713,  0x4438fed2,  0x4438eed2,  0x4430de92,  0x4430ce52,  0x4428be12,  0x3328abc9,  0x33209b89,  0x33208b09,



									};


//====================================================================
const unsigned int SDR_DCR_TAB[][2]={
						2,    0x14000000,
						8, 	0x14820000,
						16,	0x14920000,
					  	32, 	0x15120000,
					   	64,	0x15220000,
					  	128, 0x15220000,   // < 200M
					       256, 0x15A20000,						
					};
#define SDR_DCR_TAB_COL  2
#define SDR_DCR_TAB_ROW  6

#endif

//--------------------------------------------------





#if 0
unsigned char ROM0_DRAM_SIZE_TAB[4][3]={
								16,	16,	16,
								32,	32,	32,
								8,	64,	64,
								2,	128,	128
								};
#endif

//==============================================================
#if 0
unsigned int get_memory_dram_size_parameters(unsigned int *para_array)
{
	DRAM_PARA_INFO setting_info;
	setting_info = (DRAM_PARA_INFO)SETTING_INFO_ADDR;
	para_array[0] = setting_info->dram_size;
	return 1;
}



unsigned int get_memory_dram_buswidth_parameters(unsigned int *para_array)
{
	DRAM_PARA_INFO setting_info;
	setting_info = (DRAM_PARA_INFO)SETTING_INFO_ADDR;
	para_array[0] = setting_info->dram_buswidth;

	return 1;

}



#endif

//==============================================================
#if 0
void DDR_Safe_Setting0(void)
{
#if 0 //for DDR2 FPGA setting
    register int i,j,k;

    register int L0 = 0, R0 = 33, L1 = 0, R1 = 33;
    const register int  DRAM_ADR = 0xA0100000;
    const register int  DRAM_VAL = 0x5A5AA5A5;
    const register int  DDCR_ADR = 0xB8001050;
    register int  DDCR_VAL = (1<<31);  //digital
    //register int  DDCR_VAL = (0<<31);  //analog	


    REG32(DRAM_ADR) = DRAM_VAL;

    while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);
    while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);


         // Calibrate for DQS0
        for (i = 1; i <= 31; i++)
        {
            REG32(DDCR_ADR) = (DDCR_VAL & 0x80000000) | ((i-1) << 25);

            if (L0 == 0)
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) == 0x005A00A5)        {      L0 = i;         }
            }
            else
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) != 0x005A00A5)          {   R0 = i - 1;            break;            }
            }
        }
        DDCR_VAL = (DDCR_VAL & 0xC0000000) | (((L0 + R0) >> 1) << 25);
        REG32(DDCR_ADR) = DDCR_VAL;

                 // Calibrate for DQS1
        for (i = 1; i <= 31; i++)
        {
            REG32(DDCR_ADR) = (DDCR_VAL & 0xFE000000) | ((i-1) << 20);
            if (L1 == 0)
            {    if ((REG32(DRAM_ADR) & 0xFF00FF00) == 0x5A00A500)        {   L1 = i;     }
            }
            else
            {   if ((REG32(DRAM_ADR) & 0xFF00FF00) != 0x5A00A500)        {   R1 = i - 1;             break;             }
            }
        }

        // ASIC
        DDCR_VAL = (DDCR_VAL & 0xFE000000) | (((L1 + R1) >> 1) << 20);
        REG32(DDCR_ADR) = DDCR_VAL;

#if DDR_DBG
        _memctl_debug_printf("L0:%d R0:%d C0:%d\n",  L0, R0, (L0 + R0) >> 1);
        _memctl_debug_printf("L1:%d R1:%d C1:%d\n",  L1, R1, (L1 + R1) >> 1);
#endif

	int i;

	#define MEM_BASE 0xb8001000
	#define MCR  (MEM_BASE+0x00)
	#define DCR  (MEM_BASE+0x04)
	#define DTR0 (MEM_BASE+0x08)
	#define DTR1 (MEM_BASE+0x0c)
	#define DTR2 (MEM_BASE+0x10)
    #define DMCR (MEM_BASE+0x1c)



	
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00110401;


       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};
//======================================================	
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00110404;
	
       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};
//======================================================
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00100762;
	
       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};     
//======================================================
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00120000;

       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};  
//======================================================	
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00100662;
	
       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};    
//======================================================	

    
	//Reset phy fifo pointer 
	REG32(0xb8001500)=0xc0000000;
	REG32(0xb8001500)=0xc0000010;

	_memctl_debug_printf("Write 0xa0400000=0xa5a55a5a\n");
	REG32(0xa0400000)=0xa5a55a5a;
	_memctl_debug_printf("0xa0400000=0x%x \n",REG32(0xa0400000) );	
#else //for DDR3 FPGA setting	 
	
	int i;
		
	REG32(0xb800100c)=0x0a0a051f; //set DTR1
	REG32(0xb8001010)=0x07f15000; //set DTR2


	if(memctlc_is_DDR2())
	{
		_memctl_debug_printf("DDR2 init flow\n");	
		REG32(0xb8001008)=0x48344050; //CL=5 , CWL= 4 //For RTL8198C FPGA setting
		REG32(0xb8001004)=0x11220000; //set DDR2 64MB 		
	}

	if(memctlc_is_DDR3())
	{
		_memctl_debug_printf("DDR3 init flow\n");	
		REG32(0xb8001008)=0x48544050; //CL=5 , CWL= 6 //For RTL8198C FPGA setting	
		REG32(0xb8001004)=0x21220000; //set 128MB
	}

		for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101C)=0x00100A20; //EMR0 , CL=6 ,WR cycles:10
	while((REG32(DMCR)&0x80000000)==0x80000000)
       	{_memctl_debug_printf("DDR DMCR loop\n");};   


		for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101C)=0x00110001; // EMR1 , A0="1"=DLL disable //JSW: For DDR3 under 125MHZ , DLL must turn off
	while((REG32(DMCR)&0x80000000)==0x80000000)
       	{_memctl_debug_printf("DDR DMCR loop\n");};     

		for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101C)=0x00120008; // EMR2 , CWL=6 ,
	while((REG32(DMCR)&0x80000000)==0x80000000)
       	{_memctl_debug_printf("DDR DMCR loop\n");};     

		for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101C)=0x00130000; // EMR3, 
	while((REG32(DMCR)&0x80000000)==0x80000000)
       	{_memctl_debug_printf("DDR DMCR loop\n");};     

		//Reset phy fifo pointer 
	REG32(0xb8001500)=0x80000000;
	REG32(0xb8001500)=0x80000010;

	_memctl_debug_printf("Test:Write 0xa0400000=0xa5a55a5a\n");
	REG32(0xa0400000)=0xa5a55a5a;
	_memctl_debug_printf("0xa0400000=0x%x \n",REG32(0xa0400000) );	
	
	

#endif
	
}

void DDR_Safe_Setting1(void)
{
#if 0 //for DDR2 FPGA setting
    register int i,j,k;

    register int L0 = 0, R0 = 33, L1 = 0, R1 = 33;
    const register int  DRAM_ADR = 0xA0100000;
    const register int  DRAM_VAL = 0x5A5AA5A5;
    const register int  DDCR_ADR = 0xB8001050;
    register int  DDCR_VAL = (1<<31);  //digital
    //register int  DDCR_VAL = (0<<31);  //analog	


    REG32(DRAM_ADR) = DRAM_VAL;

    while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);
    while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);


         // Calibrate for DQS0
        for (i = 1; i <= 31; i++)
        {
            REG32(DDCR_ADR) = (DDCR_VAL & 0x80000000) | ((i-1) << 25);

            if (L0 == 0)
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) == 0x005A00A5)        {      L0 = i;         }
            }
            else
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) != 0x005A00A5)          {   R0 = i - 1;            break;            }
            }
        }
        DDCR_VAL = (DDCR_VAL & 0xC0000000) | (((L0 + R0) >> 1) << 25);
        REG32(DDCR_ADR) = DDCR_VAL;

                 // Calibrate for DQS1
        for (i = 1; i <= 31; i++)
        {
            REG32(DDCR_ADR) = (DDCR_VAL & 0xFE000000) | ((i-1) << 20);
            if (L1 == 0)
            {    if ((REG32(DRAM_ADR) & 0xFF00FF00) == 0x5A00A500)        {   L1 = i;     }
            }
            else
            {   if ((REG32(DRAM_ADR) & 0xFF00FF00) != 0x5A00A500)        {   R1 = i - 1;             break;             }
            }
        }

        // ASIC
        DDCR_VAL = (DDCR_VAL & 0xFE000000) | (((L1 + R1) >> 1) << 20);
        REG32(DDCR_ADR) = DDCR_VAL;

#if DDR_DBG
        _memctl_debug_printf("L0:%d R0:%d C0:%d\n",  L0, R0, (L0 + R0) >> 1);
        _memctl_debug_printf("L1:%d R1:%d C1:%d\n",  L1, R1, (L1 + R1) >> 1);
#endif

	int i;

	



	
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00110401;


       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};
//======================================================	
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00110404;
	
       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};
//======================================================
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00100762;
	
       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};     
//======================================================
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00120000;

       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};  
//======================================================	
	for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101c)=0x00100662;
	
       while((REG32(DMCR)&0x80000000)==0x80000000)
       	{};    
//======================================================	

    
	//Reset phy fifo pointer 
	REG32(0xb8001500)=0xc0000000;
	REG32(0xb8001500)=0xc0000010;

	_memctl_debug_printf("Write 0xa0400000=0xa5a55a5a\n");
	REG32(0xa0400000)=0xa5a55a5a;
	_memctl_debug_printf("0xa0400000=0x%x \n",REG32(0xa0400000) );	
#else //for DDR3 FPGA setting
	 
	
	int i;

	REG32(0xb800100c)=0x0a0a051f; //set DTR1
	REG32(0xb8001010)=0x07f15000; //set DTR2


	if(memctlc_is_DDR2())
	{
		_memctl_debug_printf("DDR2 init flow\n");			
		REG32(0xb8001008)=0x48344050; //CL=5 , CWL= 4 //For RTL8198C ASIC setting
		REG32(0xb8001004)=0x21220000; //set DDR2 64MB 		
	}

	if(memctlc_is_DDR3())
	{
		_memctl_debug_printf("DDR3 init flow\n");	
		//REG32(0xb8001008)=0x48544050; //CL=5 , CWL= 6 //For RTL8198C FPGA setting	
		REG32(0xb8001008)=0x48444050; //CL=5 , CWL= 5 //For RTL8198C ASIC setting	
		REG32(0xb8001004)=0x21220000; //set 128MB , 1Gb start
	}
		
	
#if 0
		for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101C)=0x00100A20; //EMR0 , CL=6 ,WR cycles:10
	while((REG32(DMCR)&0x80000000)==0x80000000)
       	{_memctl_debug_printf("DDR DMCR loop\n");};   


		for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	//REG32(0xb800101C)=0x00110001; // EMR1 , A0="1"=DLL disable //JSW: For DDR3 under 125MHZ , DLL must turn off
	while((REG32(DMCR)&0x80000000)==0x80000000)
       	{_memctl_debug_printf("DDR DMCR loop\n");};     

		for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101C)=0x00120008; // EMR2 , CWL=6 ,
	while((REG32(DMCR)&0x80000000)==0x80000000)
       	{_memctl_debug_printf("DDR DMCR loop\n");};     

		for(i=0; i<32; i++)
	{
		REG32(0xb8001510+i*4)=0x0f000f00;
	}
	REG32(0xb800101C)=0x00130000; // EMR3, 
	while((REG32(DMCR)&0x80000000)==0x80000000)
       	{_memctl_debug_printf("DDR DMCR loop\n");};     

		//Reset phy fifo pointer 
	REG32(0xb8001500)=0x80000000;
	REG32(0xb8001500)=0x80000010;
#endif
	_memctl_debug_printf("Test:Write 0xa0400000=0xa5a55a5a\n");
	REG32(0xa0400000)=0xa5a55a5a;
	_memctl_debug_printf("0xa0400000=0x%x \n",REG32(0xa0400000) );	

	_memctl_debug_printf("Test:Write 0xa0500000=0x12345678\n");
	REG32(0xa0500000)=0x12345678;
	_memctl_debug_printf("0xa0500000=0x%x \n",REG32(0xa0400000) );	
	
	

#endif
	
}


#endif
//==============================================================


void ShowRxDelayMap()
{
    unsigned int tx,rx,DQS_EN_HCLK;
	#define REG32_ANDOR(x,y,z)  { REG32(x)=( REG32(x) & (y) ) | (z);  }


 unsigned int rx_cnt=0,rx_OK_total=0;

 unsigned int tx_bit=31,rx_bit=31,rx_final;//scan bit

 volatile unsigned int i;

 volatile unsigned int delay_tap_DDR,freq_DDR_sel;
  
 unsigned int DQS_EN_HCLK_end=1;

    for (DQS_EN_HCLK =0; DQS_EN_HCLK<=DQS_EN_HCLK_end; DQS_EN_HCLK++)
    {
    	    _memctl_debug_printf("\n\nDQS_EN_HCLK=%02x : ",DQS_EN_HCLK);
	    REG32_ANDOR(DIDER1,    ~( (0x1<<31) | (0x1<<23) ),   (DQS_EN_HCLK<<31) | (DQS_EN_HCLK<<23) );

	 _memctl_debug_printf("\n     Rx=" );
     for (rx =0; rx<=31; rx++)
    	  _memctl_debug_printf("%02x:",rx);
   // for (tx =0; tx<=31; tx++)
    for (tx =0; tx<=tx_bit; tx++)
    {
         _memctl_debug_printf("\nTx=%02x : ",tx );
        for (rx =0; rx<= 31; rx++)
        {
        
        	#if 0
          /* Reset DRAM DLL */
			
				if(memctlc_is_DDR2()){
				//printf("Invoke memctlc_ddr2_dll_reset()\n");
					memctlc_ddr2_dll_reset();
				}else if(memctlc_is_DDR3()){
					//printf("Invoke memctlc_ddr3_dll_reset()\n");
					memctlc_ddr3_dll_reset();
				}else{
					printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
					//while(1);
				}
		#else
		//_memctl_update_phy_param();
		#endif
       
     #if 0
	     REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (rx<<24) | (rx<<16) );
	#else

	 //RX open loop 
	 REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (tx<<24) | (tx<<16) );
	//REG32(DIDER1)=0x8f8f0000;	//DDR3 Chariot 250MHZ OK.

	 REG32_ANDOR(DACCR,    ~( (0x1f<<16) | (0x1f<<8) ),   (rx<<16) | (rx<<8) );
	
	 REG32_ANDOR(CLK_MANAGER,    ~( (0x1f<<5) | (0x1f<<0) ),   (0x0<<5)| (0x0<<0) );

	  
       //Tx command
       //REG32_ANDOR(0xb80000a0,    ~( (0x1f<<5) | (0x1f<<10) ),   (0<<5) |  (0<<10) );


	 //Tx DQS	     
       REG32_ANDOR(DWPR1,    ~( (0x1f<<21) | (0x1f<<16) ),   (0x0<<21) |  (0x0<<16) );


	 //Tx DQM	   
	REG32_ANDOR(0xb8001590,    ~( (0x1f<<24) | (0x1f<<16) ),   (0x0<<24) |  (0x0<<16) );


		
	    #if 1  //Tx DQ
	  
	    for(i=0;i<32;i++)
	    {
 	        //REG32( 0xb8001510 +i*4 ) = 0x00040200; //DDR3 Chariot 250MHZ OK.

		 REG32( 0xb8001510 +i*4 )  = (0x0<<24)|0x00040200;
		//REG32( 0xb8001510 +i*4 )  = 0x00040200;
 		
		
	     } 		
	   #endif
	
	
	#endif


	     volatile unsigned int delay_time;
	     delay_time = 0x500;//about 1us to let memory controller setting take effect
	     while(delay_time--);
		

		 

            REG32(ADDR)=PATT0;      if(REG32(ADDR)!=PATT0)          goto failc0;
            REG32(ADDR)=PATT1;      if(REG32(ADDR)!=PATT1)          goto failc0;
            REG32(ADDR)=PATT2;      if(REG32(ADDR)!=PATT2)          goto failc0;
            REG32(ADDR)=PATT3;      if(REG32(ADDR)!=PATT3)          goto failc0;
            REG32(ADDR)=PATT4;      if(REG32(ADDR)!=PATT4)          goto failc0;
	     REG32(ADDR)=PATT5;      if(REG32(ADDR)!=PATT5)          goto failc0;
	     REG32(ADDR)=PATT6;      if(REG32(ADDR)!=PATT6)          goto failc0;
            REG32(ADDR)=PATT7;      if(REG32(ADDR)!=PATT7)          goto failc0;
	     REG32(ADDR)=PATT8;      if(REG32(ADDR)!=PATT8)          goto failc0;

             _memctl_debug_printf("%02x,",  rx);


	     ////////////////////////////////////Pass
	     rx_OK_total+=rx;
	     rx_cnt+=1;

	     if((tx==tx_bit)&&(rx==rx_bit)&&(DQS_EN_HCLK==DQS_EN_HCLK_end))//Scan over
	     {
			goto DONE;
	      }
		  

	     continue;

	     ////////////////////////////////////Fail
            failc0:
			
	      
	     // rx_cnt+=1;
	     
             _memctl_debug_printf("--," );
	       if((tx==tx_bit)&&(rx==rx_bit)&&(DQS_EN_HCLK==DQS_EN_HCLK_end))//Scan over
	     {
			goto DONE;
	      }

	     
	     
            continue;


	      //////////////////////////////////Done	
	      DONE:
		  	rx_final=( rx_OK_total)/ rx_cnt;	

			rx_final+=3;

			 REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (rx_final<<24) | (rx_final<<16) );
			  _memctl_debug_printf("\n\nMemctrl Open loop's rx_final=%d\n",  rx_final);
			  _memctl_debug_printf("DIDER1(0xb8001050)=0x%x\n", REG32(DIDER1));
			   _memctl_debug_printf("rx_OK_total=%d\n", rx_OK_total);
			     _memctl_debug_printf("rx_cnt=%d\n\n", rx_cnt);

		#if 1

	if(memctlc_is_DDR3())	
		freq_DDR_sel=550;

	if(memctlc_is_DDR2())	
		freq_DDR_sel=510;


	
		
		
	if(board_DRAM_freq_mhz()<freq_DDR_sel)	
	{
	   REG32_ANDOR(CLK_MANAGER,    ~( (0x1f<<5) | (0x1f<<0) ),   (0x0<<5)| (0x0<<0) );

	 REG32(DIDER1)=0x1f1f0000;	//DDR3 Chariot 250MHZ OK.
	  
       //Tx command
       REG32_ANDOR(0xb80000a0,    ~( (0x1f<<5) | (0x1f<<10) ),   (0<<5) |  (0<<10) );

	 //Tx DQS	     
       REG32_ANDOR(DWPR1,    ~( (0x1f<<21) | (0x1f<<16) ),   (0x0<<21) |  (0x0<<16) );

	 //Tx DQM	   
	REG32_ANDOR(0xb8001590,    ~( (0x1f<<24) | (0x1f<<16) ),   (0x0<<24) |  (0x0<<16) );

	   //Tx DQ	   
	    for(i=0;i<32;i++)
	    { 	      
		 REG32( 0xb8001510 +i*4 )  = 0x00040200;		
	     } 	
		
	}
	else //>=550MHZ
	{


	if(memctlc_is_DDR3())	
		 delay_tap_DDR=0x8;

	if(memctlc_is_DDR2())	
		 delay_tap_DDR=0x0;//0x12 fail , 0x8 fail 510MHZ
	
		
	 REG32_ANDOR(CLK_MANAGER,    ~( (0x1f<<5) | (0x1f<<0) ),   (delay_tap_DDR<<5)| (delay_tap_DDR<<0) );
	  
	 //RX open loop 	
	 REG32(DIDER1)=0x1f1f0000;	
	  
       //Tx command
       REG32_ANDOR(0xb80000a0,    ~( (0x1f<<5) | (0x1f<<10) ),   (delay_tap_DDR<<5) |  (delay_tap_DDR<<10) );

	 //Tx DQS	     
       REG32_ANDOR(DWPR1,    ~( (0x1f<<21) | (0x1f<<16) ),   (delay_tap_DDR<<21) |  (delay_tap_DDR<<16) );

	 //Tx DQM	   
	REG32_ANDOR(0xb8001590,    ~( (0x1f<<24) | (0x1f<<16) ),   (delay_tap_DDR<<24) |  (delay_tap_DDR<<16) );

	   //Tx DQ	   
	    for(i=0;i<32;i++)
	    { 	      
		  REG32( 0xb8001510 +i*4 )  = (delay_tap_DDR<<24)|0x00040200;	
	     } 	
		
	}
	  
		#endif
				

        }
    }
   }
}



#if 1//def CONFIG_DRAM_TEST
//==============================================================


void ShowTxRxDelayMap()
{
    unsigned int tx,rx,DQS_EN_HCLK;
	#define REG32_ANDOR(x,y,z)  { REG32(x)=( REG32(x) & (y) ) | (z);  }


 unsigned int rx_cnt=0,rx_OK_total=0;

 unsigned int tx_bit=31,rx_bit=31,rx_final;//scan bit

  volatile unsigned int i;
  
 unsigned int DQS_EN_HCLK_end=1;

    for (DQS_EN_HCLK =0; DQS_EN_HCLK<=DQS_EN_HCLK_end; DQS_EN_HCLK++)
    {
    	    _memctl_debug_printf("\n\nDQS_EN_HCLK=%02x : ",DQS_EN_HCLK);
	    REG32_ANDOR(DIDER1,    ~( (0x1<<31) | (0x1<<23) ),   (DQS_EN_HCLK<<31) | (DQS_EN_HCLK<<23) );

	_memctl_debug_printf("\n     Rx=" );
     for (rx =0; rx<=31; rx++)
    	 _memctl_debug_printf("%02x:",rx);
   // for (tx =0; tx<=31; tx++)
    for (tx =0; tx<=tx_bit; tx++)
    {
        _memctl_debug_printf("\nTx=%02x : ",tx );
        for (rx =0; rx<= 31; rx++)
        {
        
        	#if 0
          /* Reset DRAM DLL */
			
				if(memctlc_is_DDR2()){
				//_memctl_debug_printf("Invoke memctlc_ddr2_dll_reset()\n");
					memctlc_ddr2_dll_reset();
				}else if(memctlc_is_DDR3()){
					//_memctl_debug_printf("Invoke memctlc_ddr3_dll_reset()\n");
					memctlc_ddr3_dll_reset();
				}else{
					_memctl_debug_printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
					//while(1);
				}
		#else
		_memctl_update_phy_param();
		#endif
       
     #if 0
	     REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (rx<<24) | (rx<<16) );
	#else

	 //RX open loop 
	 REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (tx<<24) | (tx<<16) );
	//REG32(DIDER1)=0x8f8f0000;	//DDR3 Chariot 250MHZ OK.

	  REG32_ANDOR(DACCR,    ~( (0x1f<<16) | (0x1f<<8) ),   (rx<<16) | (rx<<8) );

	
	
	   REG32_ANDOR(CLK_MANAGER,    ~( (0x1f<<5) | (0x1f<<0) ),   (0x10<<5)| (0x10<<0) );


	  
       //Tx command
       //REG32_ANDOR(0xb80000a0,    ~( (0x1f<<5) | (0x1f<<10) ),   (0<<5) |  (0<<10) );


	 //Tx DQS	     
       REG32_ANDOR(DWPR1,    ~( (0x1f<<21) | (0x1f<<16) ),   (0x10<<21) |  (0x10<<16) );


	 //Tx DQM	   
	REG32_ANDOR(0xb8001590,    ~( (0x1f<<24) | (0x1f<<16) ),   (0x10<<24) |  (0x10<<16) );


		
	    #if 1  //Tx DQ
	  
	    for(i=0;i<32;i++)
	    {
 	        //REG32( 0xb8001510 +i*4 ) = 0x00040200; //DDR3 Chariot 250MHZ OK.

		 REG32( 0xb8001510 +i*4 )  = (0x10<<24)|0x00040200;
		//REG32( 0xb8001510 +i*4 )  = 0x00040200;
 		
		
	     } 		
	   #endif
	
	
	#endif


	     volatile unsigned int delay_time;
	     delay_time = 0x500;//about 1us to let memory controller setting take effect
	     while(delay_time--);
		

		 

            REG32(ADDR)=PATT0;      if(REG32(ADDR)!=PATT0)          goto failc0;
            REG32(ADDR)=PATT1;      if(REG32(ADDR)!=PATT1)          goto failc0;
            REG32(ADDR)=PATT2;      if(REG32(ADDR)!=PATT2)          goto failc0;
            REG32(ADDR)=PATT3;      if(REG32(ADDR)!=PATT3)          goto failc0;
            REG32(ADDR)=PATT4;      if(REG32(ADDR)!=PATT4)          goto failc0;
	     REG32(ADDR)=PATT5;      if(REG32(ADDR)!=PATT5)          goto failc0;
	     REG32(ADDR)=PATT6;      if(REG32(ADDR)!=PATT6)          goto failc0;
            REG32(ADDR)=PATT7;      if(REG32(ADDR)!=PATT7)          goto failc0;
	     REG32(ADDR)=PATT8;      if(REG32(ADDR)!=PATT8)          goto failc0;

            _memctl_debug_printf("%02x,",  rx);


	     ////////////////////////////////////Pass
	     rx_OK_total+=rx;
	     rx_cnt+=1;

	     if((tx==tx_bit)&&(rx==rx_bit)&&(DQS_EN_HCLK==DQS_EN_HCLK_end))//Scan over
	     {
			goto DONE;
	      }
		  

	     continue;

	     ////////////////////////////////////Fail
            failc0:
			
	      
	     // rx_cnt+=1;
	     
            _memctl_debug_printf("--," );
	       if((tx==tx_bit)&&(rx==rx_bit)&&(DQS_EN_HCLK==DQS_EN_HCLK_end))//Scan over
	     {
			goto DONE;
	      }

	     
	     
            continue;


	      //////////////////////////////////Done	
	      DONE:
		  	rx_final=( rx_OK_total)/ rx_cnt;	

			rx_final+=3;

			 REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (rx_final<<24) | (rx_final<<16) );
			  _memctl_debug_printf("\n\nMemctrl Open loop's rx_final=%d\n",  rx_final);
			  _memctl_debug_printf("DIDER1(0xb8001050)=0x%x\n", REG32(DIDER1));
			   _memctl_debug_printf("rx_OK_total=%d\n", rx_OK_total);
			     _memctl_debug_printf("rx_cnt=%d\n\n", rx_cnt);

		#if 0 //set to default
			REG32(DIDER1)=0x1f1f0000;	//DDR3 Chariot 250MHZ OK.
		//	REG32(DIDER1)=0x9f9f0000;	//DDR3 Chariot 250MHZ OK.
				//REG32(0xb8001500)=0x80000000;
			

	   REG32_ANDOR(CLK_MANAGER,    ~( (0x1f<<5) | (0x1f<<0) ),   (0<<5)| (0<<0) );

	 //RX open loop 
	 //REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (0x1f<<24) | (0x1f<<16) );
	  
       //Tx command
       REG32_ANDOR(0xb80000a0,    ~( (0x1f<<5) | (0x1f<<10) ),   (0<<5) |  (0<<10) );


	 //Tx DQS	     
       REG32_ANDOR(DWPR1,    ~( (0x1f<<21) | (0x1f<<16) ),   (0<<21) |  (0<<16) );


	 //Tx DQM	   
	REG32_ANDOR(0xb8001590,    ~( (0x1f<<24) | (0x1f<<16) ),   (0<<24) |  (0<<16) );

	  #if 1  //Tx DQ
	   
	    for(i=0;i<32;i++)
	    {
 	        //REG32( 0xb8001510 +i*4 ) = 0x00040200; //DDR3 Chariot 250MHZ OK.

		 REG32( 0xb8001510 +i*4 )  = (0x1f<<24)|0x00040200;
		//REG32( 0xb8001510 +i*4 )  = 0x00040200;
 		
		
	     } 		
	   #endif
		#endif
				return;	

        }
    }
   }
}
//==============================================================

#endif


//==============================================================
unsigned int uart_rx8b_val()  //user input 
{
	unsigned int val=0;
	unsigned char ch;
	int i;

  for(i=28; i>=0; i=i-4)
  {
	ch=uart_inc();
	_memctl_debug_printf("%c",ch);
	ch= (ch<':')? ch-'0'  :   (ch<'Z') ?  ch-'A'+10 : ch-'a'+10;
	val+=ch<<i;
	
  }	
	return val;
}
//==============================================================
unsigned int uart_rx4b_val()    //transfer binary
{
	unsigned int val;
	unsigned char ch;
	
	ch=uart_inc();
	val=ch<<24;
	
	ch=uart_inc();
	val+=ch<<16;

	ch=uart_inc();
	val+=ch<<8;

	ch=uart_inc();
	val+=ch;
	
	return val;
}
//==============================================================


#define putc(x)	Put_UartData(x)
#define getc_timeout  Get_UartData_timeout


int xmodem_receive()
{

//uart1_init(1, 38400);
#define xprintf(x,...) 

	//xmodem state machine
	#define	XMODEM_RX_READY 1
	#define	XMODEM_WAIT_HEADER 2
	#define	XMODEM_RX_PACKET 3
	#define	XMODEM_CANCEL 4
	#define	XMODEM_EOT 5

	//xmodem protocol header
	#define SOH				0x01	/* start of header */
	#define STX 			       0x02	/* start of header */
	#define EOT				0x04	/* end of text */
	#define ACK				0x06	/* acknowledge */
	#define NAK				0x15	/* negative acknowledge */
	#define CAN				0x18	/* cancel */
	#define CRC				0x43	/* crc acknowledge */

	#define RETRY			(20)
	
	volatile unsigned int state = XMODEM_RX_READY;

	unsigned int retry = RETRY;
	unsigned int timeout=50000;
	unsigned int i;
	unsigned char  c;
	
	#define packet_size  128
	unsigned char  block_index = 1;

	unsigned char head1,head2,head_csum;
	unsigned char csum;
	unsigned char *ptr=JUMPADDR;
	//unsigned char *ptr=0x80100000;
	_memctl_debug_printf("Loading...");
#if 0	
	if(!buf )
		return false;
#endif	

	
	/* received buffer size to zero */
	unsigned int size = 0;
	
	while(retry > 0)
	{

		switch(state)
		{	
			case XMODEM_RX_READY:				
				putc(NAK);	
				if(getc_timeout(&c, timeout))
				{
					if(c == SOH)
					{	state = XMODEM_RX_PACKET;
					}			
					else if(c == 3)  //Ctrl+C
					{	putc(CAN);	putc(CAN);
						putc(CAN);	putc(CAN);						
						return 0;
					}
				}	
				break;
				
			case XMODEM_WAIT_HEADER:			
				if(getc_timeout(&c, timeout))
				{	if(c == SOH)
					{	state = XMODEM_RX_PACKET;						
					}				
					else if(c == CAN)
					{	state = XMODEM_CANCEL;					
					}
					else if(c == EOT)
					{	state = XMODEM_EOT;
					}
				}
				else
				{	/* timed out, try again */
					retry--;
				}
				break;
				
			case XMODEM_RX_PACKET:		
				if(getc_timeout(&c, timeout))
				{	xprintf("c1=%x\n", c);
					head1=c;
				}
				else goto failx;
				
				if(getc_timeout(&c, timeout))
				{	xprintf("c2=%x\n", c);
					head2=c;				
				}
				else goto failx;

				csum=0;
				for(i = 0; i < packet_size  ; i++)
				{
					if(getc_timeout(&c, timeout))
					{	xprintf("c3=%x\n", c);
						ptr[i] = c;
						csum+=c;

					}
					else goto failx;
				}

				if(getc_timeout(&c, timeout))
				{	xprintf("c4=%x\n", c);
					head_csum=c;	
				}
				else goto failx;
		
				
				state = XMODEM_WAIT_HEADER;
				
				/* packet was too small, retry */
				if(i < (packet_size))
				{	goto failx;
				}

				/* check validity of packet */
				if( (head1 == (255-head2)) && (head_csum==csum) )
				{	xprintf("hok\n");
					/* is this the packet we were waiting for? */
					if(head1== block_index)
					{	xprintf("aok\n");
						ptr+=packet_size;
						size += packet_size;						
						block_index++;
						retry = RETRY;
						putc(ACK);
						continue;
					}
					else if(head1 == (unsigned char )(block_index-1))
					{
						/* this is a retransmission of the last packet */
						putc(ACK);
						 continue;
					}
				}	
failx:				
				retry--;
				putc(NAK);
				break;
				
			case XMODEM_CANCEL:
				putc(ACK);				
				return 0;
				
			case XMODEM_EOT:
				putc(ACK);
				//mdelay(100);				
				return size;
							
			default:
				break;
		}
	}
	
	/* retry too much, fail */
	putc(CAN);	putc(CAN);
	putc(CAN);	putc(CAN);
	
	
	return 0;
}


//==========================================================

#if 0
unsigned int _is_CKSEL_25MHz(void)
{
	if(REG(SYSREG_PIN_STATUS_REG) & SYSREG_PIN_STATUS_CLSEL_MASK)
		return 0;
	else
		return 1;
}
#endif





#ifdef CONFIG_DRAM_FROCE_DCR_VALUE
#else
//unsigned int ddr1_8bit_size[] =  { };
unsigned int ddr2_8bit_size[] =  { 0x10220000/* 32MB */, 0x10320000/* 64MB */, 0x20320000/* 128MB */, 0x20420000/* 256MB */, 0x20520000/* 512MB */};
//unsigned int ddr3_8bit_size[] =  { };
//unsigned int ddr1_16bit_size[] = { };
unsigned int ddr2_16bit_size[] = { 0x11210000/* 32MB */, 0x11220000/* 64MB */, 0x21220000/* 128MB */, 0x21320000/* 256MB */, 0x21420000/* 512MB */};
//unsigned int ddr3_16bit_size[] = { };
#endif

/* Function Name: 
 * 	_DTR_suggestion
 * Descripton:
 *	Calculate proper DTR setting.
 * Input:
 *	ref_ms
 *	rp_ns
 *	rcd_ns
 *	ras_ns
 *	rfc_ns
 *	wr_ns
 * Output:
 * 	None
 * Return:
 *	Caluculated DTR setting value for the current environment.
 */
void _DTR_suggestion(unsigned int *sug_dtr, unsigned int refi_ns, unsigned int rp_ns,\
			     unsigned int rcd_ns, unsigned int ras_ns, unsigned int rfc_ns, \
			     unsigned int wr_ns , unsigned int rrd_ns, unsigned int fawg_ns,\
			     unsigned int wtr_ns, unsigned int rtp_ns, unsigned int cas_10_ns, \
			     unsigned int mem_freq)
{
	unsigned int rp, rcd, ras, rfc, wr, wtr, rtp, rrd, fawg;
	unsigned int refi_unit, refi, refi_clks, cas;
	unsigned int dtr_value;
	unsigned int clk_10_ns, rowcnt;
	
	dtr_value = 0;

	

	clk_10_ns = (1000 * 10) / mem_freq;

	/*count for CAS*/
	cas = (cas_10_ns*10)/clk_10_ns;//400/40
	if(cas <= 20){/* Cas = 2*/
		cas = 1;
	}
	else if(cas <= 25){ /* Cas = 2.5*/
		cas = 0;
	}else{
		cas = (cas_10_ns/clk_10_ns);

		#if 1
		if((cas_10_ns%clk_10_ns) != 0)
			cas++;
		#else
		//if(((cas_10_ns%clk_10_ns) != 0)&&(mem_freq>=400))//RTL8198C
			//cas++;
		#endif

		cas--;
	}

	#if 1 //JSW add ,test purpose
	if((mem_freq <=400) )
	{
	//if((mem_freq <=450) )
		cas=4; //CL=5 , "0"= 1 unit ,good
		//cas=5; //CL=6 , "0"= 1 unit  , bad
		//cas=6; //CL=7 , "0"= 1 unit  , bad
		//cas=7; //CL=8 , "0"= 1 unit  , bad
	}
	else if((mem_freq <=533) )
	{
		if(memctlc_is_DDR3())
			cas=6;  //CL=7 , "0"= 1 unit 

		if(memctlc_is_DDR2())
			cas=5;  //CL=6 , "0"= 1 unit  , OK
			//cas=6;  //CL=7 , "0"= 1 unit ,fail
		
		//cas=7;  //CL=8 , "0"= 1 unit 	
	}
	else if((mem_freq <=600) )
	{
		cas=7;  //CL=8 , "0"= 1 unit 	
		//cas=8;  //CL=9 , "0"= 1 unit 
		//cas=9;  //CL=10 , "0"= 1 unit 
	}
	else if((mem_freq <=666) )
	{
		
		cas=7;  //CL=8 , "0"= 1 unit 
		//cas=8;  //CL=9 , "0"= 1 unit 
		//cas=9;  //CL=10 , "0"= 1 unit 
	}
	else if((mem_freq <=800) )
	{
		//cas=7;  //CL=8 , "0"= 1 unit 
		//cas=8;  //CL=9 , "0"= 1 unit 
		cas=9;  //CL=10 , "0"= 1 unit 
	}
	#endif


	#if 0
	//_memctl_debug_printf("\ncas_10_ns = %d ,\n", cas_10_ns);
	//_memctl_debug_printf("\ncas = %d ,\n", cas);
	_memctl_debug_printf("\ncas= %d ,\n", cas);//250MHZ : 40
	_memctl_debug_printf("\nclk_10_ns= %d ,\n", clk_10_ns);//250MHZ : 40
	_memctl_debug_printf("\nmem_freq = %d ,\n", mem_freq);//250MHZ : 250
	_memctl_debug_printf("\nfawg_ns = %d ,\n", fawg_ns);// 50
	#endif
	
	

	/*count for FAWG*/
	fawg = (fawg_ns * 10)/clk_10_ns;
	//_memctl_debug_printf("\nfawg 001= %d ,\n", fawg );

	
	if(0 != ((fawg_ns*10)%clk_10_ns))
		fawg++;
		//fawg+=2;
	fawg--; /* FAWG == 0, apply tFAWG with 1 DRAM clock. */

	//_memctl_debug_printf("\nfawg = %d ,\n", fawg );

	/*count for RRD*/
	rrd = (rrd_ns * 10)/clk_10_ns;
	if(0 != ((rrd_ns * 10)%clk_10_ns))
		rrd++;
	rrd--; /* rrd == 0, apply tRRD with 1 DRAM clock. */

	/*count for wtr*/
	wtr = (wtr_ns * 10)/clk_10_ns;
	if(0 != ((wtr_ns*10)%clk_10_ns))
		wtr++;
	wtr--; /* wtr == 0, apply tWTR with 1 DRAM clock. */

	/*count for rtp*/
	rtp = (rtp_ns*10)/clk_10_ns;
	if(0 != ((rtp_ns*10)%clk_10_ns))
		rtp++;
	rtp--; /* wtr == 0, apply tRP with 1 DRAM clock. */


	/*count for rp*/
	rp = (rp_ns*10)/clk_10_ns;
	if(0 != ((rp_ns*10)%clk_10_ns))
		rp++;
	rp--; /* rp == 0, apply tRP with 1 DRAM clock. */

	/*count for rcd*/
	rcd = (rcd_ns*10)/clk_10_ns;
	if(0 != ((rcd_ns*10)%clk_10_ns))
		rcd++;
	
	rcd--; /* rcd == 0, apply tRCD with 1 DRAM clock. */

	/*count for ras*/
	ras = (ras_ns*10)/clk_10_ns;
	if(0 != ((ras_ns*10)%clk_10_ns))
		ras++;
	ras--;

	/*count for rfc*/
	rfc = (rfc_ns*10)/clk_10_ns;
	if(0 != ((rfc_ns*10)%clk_10_ns))
		rfc++;
	rfc--;

	/*count for wr*/
	wr = (wr_ns*10)/clk_10_ns;
	if(0 != ((wr_ns*10)%clk_10_ns))
		wr++;

	if((mem_freq >= 400)&&(memctlc_is_DDR2())){
		wr++;
		//_memctl_debug_printf("\nmem_freq 123= %d ,\n", mem_freq);//250MHZ : 250
	}
	else
	{
		wr--;
	}
	
	if(wr > 7)
	{
		//_memctl_debug_printf("\twr == %d ,> 7 over range\n", wr);
		//wr=7;
	}
	

	/* count for DRAM refresh period.*/
	/* get row count */
	//rowcnt = 2048 << ((REG(DCR) & DCR_ROWCNT_MASK) >> DCR_ROWCNT_FD_S);
	//_memctl_debug_printf("rowcnt(%d)\n", rowcnt);
	//refi_clks = (ref_ms*mem_freq*1000)/rowcnt;
	refi_clks = (refi_ns*mem_freq)/1000;
	for(refi_unit=0; refi_unit <=7; refi_unit++){
		for(refi=0; refi<=15; refi++){
			if(refi_clks < ((32<<refi_unit)*(refi+1))){
			//if(refi_clks < ((32<<refi_unit)*(refi+2))){ //RTL8198C
				if(refi==0){
					refi = 15;
					if(0 < refi_unit)
						refi_unit--;
				}
				else{
					refi--;
				}

				#if 1
				if(mem_freq <=250 )
				{
					refi=14;
				}
				#endif

				
				goto count_dtr;
			}
		}
	}

	if(refi_unit > 7)/* error, not found.*/
		return 0;

count_dtr:
	sug_dtr[0] = ((cas << DTR0_CAS_FD_S)&DTR0_CAS_MASK) | ((wr << DTR0_WR_FD_S)&DTR0_WR_MASK) |\
		     ((rtp << DTR0_RTP_FD_S)&DTR0_RTP_MASK) | ((wtr << DTR0_WTR_FD_S)&DTR0_WTR_MASK) |\
		     ((refi << DTR0_REFI_FD_S)&DTR0_REFI_MASK) | ((refi_unit << DTR0_REFI_UNIT_FD_S)&DTR0_REFI_UNIT_MASK);
	sug_dtr[1] = ((rp << DTR1_RP_FD_S)&DTR1_RP_MASK) | ((rcd << DTR1_RCD_FD_S)&DTR1_RCD_MASK) |\
		     ((rrd << DTR1_RRD_FD_S)&DTR1_RRD_MASK) | ((fawg << DTR1_FAWG_FD_S)&DTR1_FAWG_MASK) ;
	sug_dtr[2] = ((rfc << DTR2_RFC_FD_S)&DTR2_RFC_MASK) | ((ras << DTR2_RAS_FD_S)&DTR2_RAS_MASK);
	//dtr_value = ((rp << DTR_RP_FD_S)&DTR_RP_MASK) | ((rcd << DTR_RCD_FD_S)&DTR_RCD_MASK) | ((ras << DTR_RAS_FD_S)&DTR_RAS_MASK) \
		| ((rfc << DTR_RFC_FD_S)&DTR_RFC_MASK) | ((wr << DTR_WR_FD_S)&DTR_WR_MASK) | ((refi << DTR_REFI_FD_S)&DTR_REFI_MASK) \
		| ((refi_unit << DTR_REFI_UNIT_FD_S)&DTR_REFI_UNIT_MASK);


	

	return;
}


void memctlc_DDR2_config_DTR(void)
{
	volatile unsigned int *dtr0, *dtr1, *dtr2;
	unsigned int sug_dtr[3];
	unsigned int dram_freq_mhz;
	unsigned int std_rfc_ns, cas_10_ns, tcwl, wr, cas, wtr;


	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	

	#if 1
	std_rfc_ns = DDR2_STD_RFC_NS;

	#else
	unsigned int dram_size = memctlc_dram_size()/_get_DRAM_csnum();
	
	//_memctl_debug_printf("\nDDR2 RFC , dramsize=0x%x\n",dram_size);
	
	unsigned int _rfc_ns;
	
	switch (dram_size){
		case 0x2000000:
			_rfc_ns = DDR2_STD_RFC_32MB_NS;
			break;
		case 0x4000000:
			_rfc_ns = DDR2_STD_RFC_64MB_NS;
			break;
		case 0x8000000:
			_rfc_ns = DDR2_STD_RFC_128MB_NS;
			break;
		case 0x10000000:
			_rfc_ns = DDR2_STD_RFC_256MB_NS;
			break;
		case 0x20000000:
			_rfc_ns = DDR2_STD_RFC_512MB_NS;
			break;
		default:
			_rfc_ns = DDR2_STD_RFC_512MB_NS;
			break;
	}
	
	
	std_rfc_ns = _rfc_ns;
	#endif

	

	dram_freq_mhz = board_DRAM_freq_mhz();

	if(dram_freq_mhz >= DDR2_CAS6_MAX_MHZ){
		cas_10_ns = (7 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS4_MAX_MHZ){
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS3_MAX_MHZ){
		cas_10_ns = (4 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS2_MAX_MHZ){
		cas_10_ns = (3 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (2 * 1000 * 10)/dram_freq_mhz;
	}

        _DTR_suggestion(sug_dtr, DDR2_STD_REFI_NS, DDR2_STD_RP_NS, \
                        DDR2_STD_RCD_NS, DDR2_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
                        DDR2_STD_RRD_NS, DDR2_STD_FAWG_NS, DDR2_STD_WTR_NS, DDR2_STD_RTP_NS,\
                        cas_10_ns, dram_freq_mhz);

	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 2){
		cas = 2;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}

	/* DDR2 write cas == read cas - 1*/
	#if 0 //old DDR2-800 setting
	tcwl = cas - 1;
	if(tcwl < 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);
	}else{
		_memctl_debug_printf("\tWorning: wrong tCWL computation\n");
	}
	#else //new DDR2-1066 setting
	tcwl = cas - 1;
	if(tcwl < 8){ //CL=7 is DDR2's max value
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);
	}else{
		printf("\tWarning: wrong tCWL computation\n");
		//sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);
	}
	#endif

	#if 0
	/* DDR2 Write recovery maximum == 6 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (5 << DTR0_WR_FD_S);
	}
	#else
	/* DDR2 Write recovery maximum == 8 for DDR2-1066 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr >= 8){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | ((wr-1) << DTR0_WR_FD_S);
	}
	else{
		_memctl_debug_printf("\ttWR(%d) \n",wr);
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | ((wr-1) << DTR0_WR_FD_S);
	}
	#endif

	/* DDR2 Write to read delay cycle at least 2 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 1){
		wtr = 1;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (wtr << DTR0_WTR_FD_S);
	}


	*dtr0 = sug_dtr[0];
	*dtr1 = sug_dtr[1];
	*dtr2 = sug_dtr[2];

	return;


}


void memctlc_DDR3_config_DTR(void)
{
	volatile unsigned int *dtr0, *dtr1, *dtr2;
	unsigned int sug_dtr[3];
	unsigned int dram_freq_mhz;
	unsigned int std_rfc_ns, cas_10_ns, tcwl;
	unsigned int rrd, wr, cas, wtr, rtp;


	dtr0 = (volatile unsigned int *)DTR0;
	dtr1 = (volatile unsigned int *)DTR1;
	dtr2 = (volatile unsigned int *)DTR2;

	#if 1
	std_rfc_ns = DDR3_STD_RFC_NS;
	
	#else
	unsigned int dram_size = memctlc_dram_size()/_get_DRAM_csnum();

	//_memctl_debug_printf("\nDDR3 RFC , dramsize=0x%x\n",dram_size);
	
	unsigned int _rfc_ns;
	switch (dram_size){
		case 0x4000000:
			_rfc_ns = DDR3_STD_RFC_64MB_NS;
			break;
		case 0x8000000:
			_rfc_ns = DDR3_STD_RFC_128MB_NS;
			break;
		case 0x10000000:
			_rfc_ns = DDR3_STD_RFC_256MB_NS;
			break;
		case 0x20000000:
			_rfc_ns = DDR3_STD_RFC_512MB_NS;
			break;
		default:
			_rfc_ns = DDR3_STD_RFC_1GB_NS;
			break;
	}

	
	
	std_rfc_ns = _rfc_ns;
	#endif


	dram_freq_mhz = board_DRAM_freq_mhz();

	if(dram_freq_mhz >= DDR3_CAS10_MAX_MHZ){
		cas_10_ns = (11 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS9_MAX_MHZ){
		cas_10_ns = (10 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS8_MAX_MHZ){
		cas_10_ns = (9 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS7_MAX_MHZ){
		cas_10_ns = (8 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS6_MAX_MHZ){
		cas_10_ns = (7 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}

        _DTR_suggestion(sug_dtr, DDR3_STD_REFI_NS, DDR3_STD_RP_NS, \
                        DDR3_STD_RCD_NS, DDR3_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
                        DDR3_STD_RRD_NS, DDR3_STD_FAWG_NS, DDR3_STD_WTR_NS, DDR3_STD_RTP_NS,\
                        cas_10_ns, dram_freq_mhz);

	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 4){
		cas = 4;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}

	/* DDR3 write cas */
	if(dram_freq_mhz >= DDR3_CWL8_MAX_MHZ){
		tcwl = 7; //8		
	}
	else if(dram_freq_mhz >= DDR3_CWL7_MAX_MHZ){
		tcwl = 7; //8		
	}
	else if(dram_freq_mhz >= 550){
		tcwl = 6; //7		
	}else if(dram_freq_mhz >= DDR3_CWL6_MAX_MHZ){
		tcwl = 5; //6
		//tcwl = 6;//7
	}else if(dram_freq_mhz >= DDR3_CWL5_MAX_MHZ){
		tcwl = 5;
	}else{
		tcwl = 4;
	}

	sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);

	/* DDR3 Write recovery maximum == 12 , min == 5 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 11){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (11 << DTR0_WR_FD_S);
	}else if(wr < 4){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (4 << DTR0_WR_FD_S);
	}else{}

	/* DDR3 Write to read delay cycle at least 4 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 3){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (3 << DTR0_WTR_FD_S);
	}

	/* DDR3 RTP delay cycle at least 4 clock cycles */
	rtp = ((sug_dtr[0] & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);
	if(rtp < 3){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_RTP_MASK)) | (3 << DTR0_RTP_FD_S);
	}

	/* DDR3 RRD delay cycle at least 4 clock cycles */
	rrd = ((sug_dtr[1] & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);
	if(rrd < 3){
		sug_dtr[1] = (sug_dtr[1] & (~DTR1_RRD_MASK)) | (3 << DTR1_RRD_FD_S);
	}

	*dtr0 = sug_dtr[0];
	*dtr1 = sug_dtr[1];
	*dtr2 = sug_dtr[2];
//check dram OK?
	if((sug_dtr[0]!= *dtr0)||(*dtr1 != sug_dtr[1]) || (*dtr2 != sug_dtr[2]))  //Added by Jason
	{
		REG32(0xb800311c)=0;
	}

	return;

}


void memctlc_config_DTR(void)
{
#if 0//CONFIG_AUTO_DRAM_TIMING_SETTING ,set up by menuconfig
	*dtr0 = CONFIG_DRAM_DTR0;
	*dtr1 = CONFIG_DRAM_DTR1;
	*dtr2 = CONFIG_DRAM_DTR2;
#else
	//if(memctlc_is_DDR()){
		//_memctl_debug_printf("\nmemctlc_check_DTR():DRAM Type: DDR1\n");
	//}else if(memctlc_is_DDR2()){
	if(memctlc_is_DDR2()){
		_memctl_debug_printf("\nmemctlc_check_DTR():DRAM Type: DDR2\n");
		memctlc_DDR2_config_DTR();
		
	}else{
		_memctl_debug_printf("\nmemctlc_check_DTR():DRAM Type: DDR3\n");
		memctlc_DDR3_config_DTR();
	}
	

#endif
	return;
}



/* Function Name: 
 * 	memctlc_check_ZQ
 * Descripton:
 * 	Check ZQ calibration status
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
void memctlc_check_ZQ(void)
{
	volatile unsigned int *zq_cali_reg;
	volatile unsigned int *zq_cali_status_reg;


	zq_cali_reg = (volatile unsigned int *)0xB8001094;
	zq_cali_status_reg = (volatile unsigned int *)0xB8001098;

	_memctl_debug_printf("Checking ZQ calibration Status:\n");
	_memctl_debug_printf("\tZQ Program(0x%x): 0x%x\n", zq_cali_reg, *zq_cali_reg);
	_memctl_debug_printf("\tZQ Status (0x%x): 0x%x\n", zq_cali_status_reg, *zq_cali_status_reg);
	_memctl_debug_printf("\tHS0_control2 (0x%x): 0x%x\n", 0xb80000a4, REG32( 0xb80000a4));
	_memctl_debug_printf("\tHS0_control3 (0x%x): 0x%x\n", 0xb80000a8, REG32( 0xb80000a8));

	if(*zq_cali_status_reg & 0x20000000){
		_memctl_debug_printf("\tZQ calibration Fail!\n");
	}
	else{
		_memctl_debug_printf("\tZQ calibration Pass\n");
	}
}


/* Function Name: 
 * 	_get_DRAM_csnum
 * Descripton:
 *	return DRAN total number of bytes.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	DRAM total byte number.
 */
unsigned int _get_DRAM_csnum(void)
{
    unsigned int dcr;

    dcr = *((unsigned int *)(DCR));

    return (((dcr>>15)&1) + 1);

}


/* Function Name: 
 * 	memctlc_dram_size
 * Descripton:
 *	return DRAN total number of bytes.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	DRAM total byte number.
 */
unsigned int memctlc_dram_size(void)
{
    unsigned int dcr;
    int total_bit = 0;


    dcr = *((unsigned int *)(DCR));
    total_bit = 0;
    total_bit += ((dcr>>24)&0x3); //bus width
   // total_bit += ((dcr>>20)&0x3)+11; //row count
    total_bit += ((dcr>>20)&0xf)+11; //row count
    
   // total_bit += ((dcr>>16)&0x7)+8 ; //col count
    total_bit += ((dcr>>16)&0xf)+8 ; //col count

	
    total_bit += ((dcr>>28)&0x3)+1;  //bank count
    total_bit += (dcr>>15)&1;        //Dram Chip Select

    return ((1<<total_bit));


    //return(1<<total_bit);
}

/* Function Name: 
 * 	memctlc_is_DDR
 * Descripton:
 *	Determine whether the DRAM type is DDR SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	1  -DRAM type is DDR SDRAM
 *	0  -DRAM type isn't DDR SDRAM
 */
unsigned int memctlc_is_DDR(void)
{
	if(MCR_DRAMTYPE_DDR == (REG(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
}

/* Function Name: 
 * 	memctlc_is_SDR
 * Descripton:
 *	Determine whether the DRAM type is SDR SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	1  -DRAM type is SDR SDRAM
 *	0  -DRAM type isn't SDR SDRAM
 */
unsigned int memctlc_is_DDR3(void)
{
	if(MCR_DRAMTYPE_DDR3 == (REG(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
}

/* Function Name: 
 * 	memctlc_is_DDR2
 * Descripton:
 *	Determine whether the DRAM type is DDR2 SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	1  -DRAM type is DDR2 SDRAM
 *	0  -DRAM type isn't DDR2 SDRAM
 */
unsigned int memctlc_is_DDR2(void)
{
	if(MCR_DRAMTYPE_DDR2 == (REG(MCR) & MCR_DRAMTYPE_MASK))
		return 1;
	else
		return 0;
}

/* Function Name: 
 * 	_DTR_refresh_freq_mhz
 * Descripton:
 *	Find out the min. legal DRAM frequency with the current refresh cycles setting in the refi_ns ns.
 * Input:
 *	ms_period -
 * Output:
 * 	None
 * Return:
 *  	0	- refi_ns is too small or refresh cycles is too large.
 *	Others	- Min. legal DRAM freq. with the current refresh cycles setting.
 */
unsigned int _DTR_refresh_freq_mhz(unsigned int refi_ns)
{
	unsigned int refi, refi_unit,refi_ns_result,dram_clk;
	unsigned int rowcnt;
	unsigned int clk_ns;
	

	refi      = 1 + ((REG(DTR0) & DTR0_REFI_MASK) >> DTR0_REFI_FD_S);
	refi_unit = 32 << (((REG(DTR0) & DTR0_REFI_UNIT_MASK) >> DTR0_REFI_UNIT_FD_S));

//	rowcnt    = 2048 << ((REG(DCR) & DCR_ROWCNT_MASK) >> DCR_ROWCNT_FD_S);
//	clk_ns = (ms_period * 1000000) / (refi*refi_unit*rowcnt);
	clk_ns = refi_ns / (refi*refi_unit);


	#if 1		
		dram_clk=1000*100/board_DRAM_freq_mhz();
		refi_ns_result=(refi*refi_unit*dram_clk)/100;

		if(refi_ns_result>=7800)
		{
		printf("\nWarning: Refresh time Over DRAM Spec 7800ns ! ");
		printf("\nrefi=%d units  ", refi);
		printf("\nrefi_unit=%d DRAM_CLK  ", refi_unit);
		printf("\nrefresh period setting=%d ns  ", refi_ns_result);
		}
		else
		{
			#if 0
			_memctl_debug_printf("\nrefi=%d units  ", refi);
			_memctl_debug_printf("\nrefi_unit=%d DRAM_CLK  ", refi_unit);
			_memctl_debug_printf("\nrefresh period setting=%d ns  ", refi_ns_result);
			#else
			_memctl_debug_printf("\nRefresh time meet DRAM Spec 7800ns! ");
			_memctl_debug_printf("\nrefi=%d units  ", refi);
			_memctl_debug_printf("\nrefi_unit=%d DRAM_CLK  ", refi_unit);
			_memctl_debug_printf("\nrefresh period setting=%d ns  ", refi_ns_result);
			#endif
		}
	#endif
	
	if(clk_ns > 0){
		//return (unsigned int)((refi*refi_unit*rowcnt)/(ms_period * 1000));
		return (unsigned int)((refi*refi_unit*1000)/refi_ns);
	}
	else {
		_memctl_debug_printf("#Warnnig: unstable refresh period setting (%d x %d). ", refi, refi_unit);
		return 0;
	}
}

/* Function Name: 
 * 	_DTR_wtr_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rp) setting.
 * Input:
 *	rp_ns	- tWTR requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(WTR) setting.
 */
unsigned int _DTR_wtr_frq_mhz(unsigned int wtr_ns)
{
	unsigned int wtr;

	/* get tRP value */
	wtr = 1 + ((REG(DTR0) & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);

	return ((1000*wtr)/wtr_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_rtp_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rp) setting.
 * Input:
 *	rp_ns	- tRP requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rp) setting.
 */
unsigned int _DTR_rtp_frq_mhz(unsigned int rtp_ns)
{
	unsigned int rtp;

	/* get tRP value */
	rtp = 1 + ((REG(DTR0) & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);

	return ((1000*rtp)/rtp_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_rp_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rp) setting.
 * Input:
 *	rp_ns	- tRP requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rp) setting.
 */
unsigned int _DTR_rp_frq_mhz(unsigned int rp_ns)
{
	unsigned int rp;

	/* get tRP value */
	rp = 1 + ((REG(DTR1) & DTR1_RP_MASK) >> DTR1_RP_FD_S);
	_memctl_debug_printf("\trp(%d): ",rp );
	return ((1000*rp)/rp_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_rrd_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rcd) setting.
 * Input:
 *	rrd_ns	- tRRD requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rrd) setting.
 */
unsigned int _DTR_rrd_frq_mhz(unsigned int rrd_ns)
{
	unsigned int rrd;

	/* get tRCD value */
	rrd = 1 + ((REG(DTR1) & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);
	_memctl_debug_printf("\trrd(%d): ",rrd);
	return ((1000*rrd)/rrd_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_fawg_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rcd) setting.
 * Input:
 *	fawg_ns	- tFAWG requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(fawg) setting.
 */
unsigned int _DTR_fawg_frq_mhz(unsigned int fawg_ns)
{
	unsigned int fawg;

	/* get tRCD value */
	fawg = 1 + ((REG(DTR1) & DTR1_FAWG_MASK) >> DTR1_FAWG_FD_S);
	_memctl_debug_printf("\tfawg(%d): ",fawg);
	return ((1000*fawg)/fawg_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_rcd_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rcd) setting.
 * Input:
 *	rcd_ns	- tRCD requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rcd) setting.
 */
unsigned int _DTR_rcd_frq_mhz(unsigned int rcd_ns)
{
	unsigned int rcd;

	/* get tRCD value */
	rcd = 1 + ((REG(DTR1) & DTR1_RCD_MASK) >> DTR1_RCD_FD_S);
	_memctl_debug_printf("\trcd(%d): ",rcd );
	return ((1000*rcd)/rcd_ns); /* return number of MHz */
}

/* Function Name: 
 * 	_DTR_ras_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(ras) setting.
 * Input:
 *	ras_ns	- tRAS requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(ras) setting.
 */
unsigned int _DTR_ras_frq_mhz(unsigned int ras_ns)
{
	unsigned int ras;

	/* get tRAS value */
	ras = 1 + ((REG(DTR2) & DTR2_RAS_MASK) >> DTR2_RAS_FD_S);
	_memctl_debug_printf("\tras(%d): ",ras);
	return ((1000*ras)/ras_ns); /* return number of MHz */

}

/* Function Name: 
 * 	_DTR_rfc_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(rfc) setting.
 * Input:
 *	rfc_ns	- tRFC requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(rfc) setting.
 */
unsigned int _DTR_rfc_frq_mhz(unsigned int rfc_ns)
{
	unsigned int rfc;

	/* get tRFC value */
	rfc = 1 + ((REG(DTR2) & DTR2_RFC_MASK) >> DTR2_RFC_FD_S);
	_memctl_debug_printf("\trfc(%d): ",rfc);
	return ((1000*rfc)/rfc_ns); /* return number of MHz */
}


/* Function Name: 
 * 	_DTR_wr_frq_mhz
 * Descripton:
 *	Find out the max. legal DRAM frequency with the current DTR(wr) setting.
 * Input:
 *	wr_ns	- tWR requirement in DRAM Sepc.
 * Output:
 * 	None
 * Return:
 *	Max. legal DRAM freq. with the current DTR(wr) setting.
 * Note:
 *	Require add 2 more DRAM clock cycles in the current design.
 */
unsigned int _DTR_wr_frq_mhz(unsigned int wr_ns)
{
	unsigned int wr;

	/* get tWR value */
	wr = 1 + ((REG(DTR0) & DTR0_WR_MASK) >> DTR0_WR_FD_S);
#if 0
	if(wr <= 2){
		_memctl_debug_printf("\t#Warnning: wr < 2 too small.\n");
		return 0;
	}
	else
#endif
	return ((1000*wr)/wr_ns); /* return number of MHz */
}



#if 0
/* Function Name: 
 * 	_DTR_DDR3_MRS_setting
 * Descripton:
 *	Find out the values of the mode registers according to the DTR0/1/2 setting
 *	for DDR2 SDRAM.
 * Input:
 *	sug_dtr	- The DTR0/1/2 setting.
 * Output:
 *	mr	- The values of the mode registers.
 * Return:
 *	None
 * Note:
 *	None
 */
void _DTR_DDR3_MRS_setting(unsigned int *sug_dtr, unsigned int *mr)
{
	unsigned int cas, wr, cwl;
	/* Default value of Mode registers */
	mr[0] = DMCR_MRS_MODE_MR | DDR3_MR_BURST_8 | DDR3_MR_READ_BURST_NIBBLE | \
		DDR3_MR_TM_NOR | DDR3_MR_DLL_RESET_NO | DDR3_MR_PD_FAST |\
		DMCR_MR_MODE_EN ;

	mr[1] = DDR3_EMR1_DLL_EN | DDR3_EMR1_DIC_RZQ_DIV_6 |\
		DDR3_EMR1_RTT_NOM_DIS | DDR3_EMR1_ADD_0 | DDR3_EMR1_WRITE_LEVEL_DIS | \
		DDR3_EMR1_TDQS_DIS | DDR3_EMR1_QOFF_EN |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR1;
	mr[2] = DDR3_EMR2_PASR_FULL | DDR3_EMR2_ASR_DIS | DDR3_EMR2_SRT_NOR |\
		DDR3_EMR2_RTT_WR_DIS | DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR2;

	mr[3] = DDR3_EMR3_MPR_OP_NOR | DDR3_EMR3_MPR_LOC_PRE_PAT |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR3;

	/* Extract CAS and WR in DTR0 */
	cas = (sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S;
	wr = (sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S;
	cwl = (sug_dtr[0] & DTR0_CWL_MASK) >> DTR0_CWL_FD_S;
	switch (cas){
		case 4:
			mr[0] = mr[0] | DDR3_MR_CAS_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR3_MR_CAS_6;
			break;
		case 6:
			mr[0] = mr[0] | DDR3_MR_CAS_7;
			break;
		case 7:
			mr[0] = mr[0] | DDR3_MR_CAS_8;
			break;
		case 8:
			mr[0] = mr[0] | DDR3_MR_CAS_9;
			break;
		case 9:
			mr[0] = mr[0] | DDR3_MR_CAS_10;
			break;
		case 10:
			mr[0] = mr[0] | DDR3_MR_CAS_11;
			break;
		default:
			/* shall be error */
			mr[0] = mr[0] | DDR3_MR_CAS_6;
			break;
	}

	switch (wr){
		case 4:
			mr[0] = mr[0] | DDR3_MR_WR_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR3_MR_WR_6;
			break;
		case 6:
			mr[0] = mr[0] | DDR3_MR_WR_7;
			break;
		case 7:
			mr[0] = mr[0] | DDR3_MR_WR_8;
			break;
		case 8:
			mr[0] = mr[0] | DDR3_MR_WR_9;
			break;
		case 9:
			mr[0] = mr[0] | DDR3_MR_WR_10;
			break;
		case 11:
			mr[0] = mr[0] | DDR3_MR_WR_12;
			break;
		default:
			/* shall be error */
			mr[0] = mr[0] | DDR3_MR_WR_6;
			break;
	}

	switch (cwl){
		case 4:
			mr[2] = mr[2] | DDR3_EMR2_CWL_5;
			break;
		case 5:
			mr[2] = mr[2] | DDR3_EMR2_CWL_6;
			break;
		case 6:
			mr[2] = mr[2] | DDR3_EMR2_CWL_7;
			break;
		case 7:
			mr[2] = mr[2] | DDR3_EMR2_CWL_8;
			break;
		default:
			/* shall be error */
			mr[2] = mr[2] | DDR3_EMR2_CWL_6;
			break;
	}

	return;
}

#else



/* Function Name: 
 * 	_DTR_DDR3_MRS_setting
 * Descripton:
 *	Find out the values of the mode registers according to the DTR0/1/2 setting
 *	for DDR2 SDRAM.
 * Input:
 *	sug_dtr	- The DTR0/1/2 setting.
 * Output:
 *	mr	- The values of the mode registers.
 * Return:
 *	None
 * Note:
 *	None
 */
void _DTR_DDR3_MRS_setting(unsigned int *sug_dtr, unsigned int *mr)
{
	unsigned int cas, wr, cwl, rtt_wr, rtt_nom, drv_str, dic_value;
	/* Default value of Mode registers */
	mr[0] = DMCR_MRS_MODE_MR | DDR3_MR_BURST_8 | DDR3_MR_READ_BURST_NIBBLE | \
		DDR3_MR_TM_NOR | DDR3_MR_DLL_RESET_NO | DDR3_MR_PD_FAST |\
		DMCR_MR_MODE_EN ;

	#if 1 //350MHZ OK
	rtt_nom=120;//for RTL8198C default
	//ocd_value=40;//for RTL8198C default
	dic_value=34;//for RTL8198C default , DRAM OCD

	rtt_wr =60;
	#else
	rtt_nom=20;//for RTL8198C default
	//ocd_value=40;//for RTL8198C default
	dic_value=34;//for RTL8198C default , DRAM OCD

	rtt_wr =120;
	#endif

	//_memctl_debug_printf("memctlc_ddr3_dll_reset: Set DRAM rtt_value=%d)\n", rtt_nom);
	//_memctl_debug_printf("memctlc_ddr3_dll_reset: Set dic_value(OCD)=%d)\n", dic_value);
	//_memctl_debug_printf("memctlc_ddr3_dll_reset: Set rtt_wr_value=%d)\n", rtt_wr);


	if(1){
		if(rtt_nom != 0){
	   	      switch (rtt_nom){
			case 120: /* div 2 */
				rtt_nom = DDR3_EMR1_RTT_NOM_RZQ_DIV2;
				break;
			case 60: /* div 4*/
				rtt_nom = DDR3_EMR1_RTT_NOM_RZQ_DIV4;
				break;
			case 40: /* div 6 */
				rtt_nom = DDR3_EMR1_RTT_NOM_RZQ_DIV6;
				break;
			case 30: /* div 8 */
				rtt_nom = DDR3_EMR1_RTT_NOM_RZQ_DIV8;
				break;
			case 20: /* div 12 */
				rtt_nom = DDR3_EMR1_RTT_NOM_RZQ_DIV12;
				break;
			default: /* div 2 */
				rtt_nom = DDR3_EMR1_RTT_NOM_RZQ_DIV2;
				break;
			}
		}else{
			rtt_nom = DDR3_EMR1_RTT_NOM_DIS;
		}
	}else{
		rtt_nom = DDR3_EMR1_RTT_NOM_RZQ_DIV2;
	}

	if(1){
		switch (dic_value){
			case 1: /* RZQ/6 */
				dic_value = DDR3_EMR1_DIC_RZQ_DIV6;
				break;
			default: /* RZQ/7 */
				dic_value = DDR3_EMR1_DIC_RZQ_DIV7;
				break;
		}
	}else{
		dic_value = DDR3_EMR1_DIC_RZQ_DIV6;
	}


	mr[1] = DDR3_EMR1_DLL_EN | dic_value |\
		rtt_nom | DDR3_EMR1_ADD_0 | DDR3_EMR1_WRITE_LEVEL_DIS | \
		DDR3_EMR1_TDQS_DIS | DDR3_EMR1_QOFF_EN |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR1;

	if(1){
		if(rtt_wr != 0){
	   	      switch (rtt_wr){
			case 120: /* div 2 */
				rtt_wr = DDR3_EMR2_RTT_WR_RZQ_DIV2;
				break;
			case 60: /* div 4*/
				rtt_wr = DDR3_EMR2_RTT_WR_RZQ_DIV4;
				break;
			default: /* div 2 */
				rtt_wr = DDR3_EMR2_RTT_WR_RZQ_DIV2;
				break;
			}
		}else{
			rtt_wr = DDR3_EMR2_RTT_WR_DIS;
		}
	}else{
		rtt_wr = DDR3_EMR2_RTT_WR_RZQ_DIV2;
	}


	mr[2] = DDR3_EMR2_PASR_FULL | DDR3_EMR2_ASR_DIS | DDR3_EMR2_SRT_NOR |\
		rtt_wr | DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR2;

	mr[3] = DDR3_EMR3_MPR_OP_NOR | DDR3_EMR3_MPR_LOC_PRE_PAT |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR3;

	/* Extract CAS and WR in DTR0 */
	cas = (sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S;
	wr = (sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S;
	cwl = (sug_dtr[0] & DTR0_CWL_MASK) >> DTR0_CWL_FD_S;
	switch (cas){
		case 4:
			mr[0] = mr[0] | DDR3_MR_CAS_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR3_MR_CAS_6;
			break;
		case 6:
			mr[0] = mr[0] | DDR3_MR_CAS_7;
			break;
		case 7:
			mr[0] = mr[0] | DDR3_MR_CAS_8;
			break;
		case 8:
			mr[0] = mr[0] | DDR3_MR_CAS_9;
			break;
		case 9:
			mr[0] = mr[0] | DDR3_MR_CAS_10;
			break;
		case 10:
			mr[0] = mr[0] | DDR3_MR_CAS_11;
			break;
		default:
			/* shall be error */
			mr[0] = mr[0] | DDR3_MR_CAS_6;
			break;
	}

	switch (wr){
		case 4:
			mr[0] = mr[0] | DDR3_MR_WR_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR3_MR_WR_6;
			break;
		case 6:
			mr[0] = mr[0] | DDR3_MR_WR_7;
			break;
		case 7:
			mr[0] = mr[0] | DDR3_MR_WR_8;
			break;
		case 8:
			mr[0] = mr[0] | DDR3_MR_WR_9;
			break;
		case 9:
			mr[0] = mr[0] | DDR3_MR_WR_10;
			break;
		case 11:
			mr[0] = mr[0] | DDR3_MR_WR_12;
			break;
		default:
			/* shall be error */
			mr[0] = mr[0] | DDR3_MR_WR_6;
			break;
	}

	switch (cwl){
		case 4:
			mr[2] = mr[2] | DDR3_EMR2_CWL_5;
			break;
		case 5:
			mr[2] = mr[2] | DDR3_EMR2_CWL_6;
			break;
		case 6:
			mr[2] = mr[2] | DDR3_EMR2_CWL_7;
			break;
		case 7:
			mr[2] = mr[2] | DDR3_EMR2_CWL_8;
			break;
		default:
			/* shall be error */
			mr[2] = mr[2] | DDR3_EMR2_CWL_6;
			break;
	}

	return;
}



#endif




/* Function Name: 
 * 	_DTR_DDR2_MRS_setting
 * Descripton:
 *	Find out the values of the mode registers according to the DTR0/1/2 setting
 *	for DDR2 SDRAM.
 * Input:
 *	sug_dtr	- The DTR0/1/2 setting.
 * Output:
 *	mr	- The values of the mode registers.
 * Return:
 *	None
 * Note:
 *	None
 */
void _DTR_DDR2_MRS_setting(unsigned int *sug_dtr, unsigned int *mr)
{
	unsigned int cas, wr;
	/* Default value of Mode registers */
	mr[0] = DMCR_MRS_MODE_MR | DDR2_MR_BURST_4 | DDR2_MR_BURST_SEQ | \
		DDR2_MR_TM_NOR | DDR2_MR_DLL_RESET_NO | DDR2_MR_PD_FAST |\
		DMCR_MR_MODE_EN ;

	mr[1] = DDR2_EMR1_DLL_EN | DDR2_EMR1_DIC_FULL |\
		DDR2_EMR1_RTT_DIS | DDR2_EMR1_ADD_0 | DDR2_EMR1_OCD_EX | \
		DDR2_EMR1_QOFF_EN | DDR2_EMR1_NDQS_EN | DDR2_EMR1_RDQS_DIS |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR1;
	mr[2] = DDR2_EMR2_HTREF_DIS | DDR2_EMR2_DCC_DIS | DDR2_EMR2_PASELF_FULL |\
		DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR2;

	mr[3] = DMCR_MR_MODE_EN | DMCR_MRS_MODE_EMR3;

	/* Extract CAS and WR in DTR0 */
	cas = (sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S;
	wr = (sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S;
	switch (cas){
		case 1:
			mr[0] = mr[0] | DDR2_MR_CAS_2;
			break;
		case 2:
			mr[0] = mr[0] | DDR2_MR_CAS_3;
			break;
		case 3:
			mr[0] = mr[0] | DDR2_MR_CAS_4;
			break;
		case 4:
			mr[0] = mr[0] | DDR2_MR_CAS_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR2_MR_CAS_6;
			break;
		default:
			mr[0] = mr[0] | DDR2_MR_CAS_6;
			break;
			
	}

	switch (wr){
		case 1:
			mr[0] = mr[0] | DDR2_MR_WR_2;
			break;
		case 2:
			mr[0] = mr[0] | DDR2_MR_WR_3;
			break;
		case 3:
			mr[0] = mr[0] | DDR2_MR_WR_4;
			break;
		case 4:
			mr[0] = mr[0] | DDR2_MR_WR_5;
			break;
		case 5:
			mr[0] = mr[0] | DDR2_MR_WR_6;
			break;
		case 6:
			mr[0] = mr[0] | DDR2_MR_WR_7;
			break;
		default:
			mr[0] = mr[0] | DDR2_MR_WR_7;
			break;
	}

	return;
}


/* Function Name: 
 * 	_DTR_DDR1_MRS_setting
 * Descripton:
 *	Find out the values of the mode registers according to the DTR0/1/2 setting
 *	for DDR1 SDRAM.
 * Input:
 *	sug_dtr	- The DTR0/1/2 setting.
 * Output:
 *	mr	- The values of the mode registers.
 * Return:
 *	None
 * Note:
 *	None
 */
void _DTR_DDR1_MRS_setting(unsigned int *sug_dtr, unsigned int *mr)
{
	unsigned int cas, buswidth;
	/* Default value of Mode registers */
	mr[0] = DMCR_MRS_MODE_MR | DDR1_MR_BURST_SEQ | DDR1_MR_OP_NOR |\
		DMCR_MR_MODE_EN ;

	mr[1] = DMCR_MRS_MODE_EMR1 | DDR1_EMR1_DLL_EN | DDR1_EMR1_DRV_NOR |\
		DMCR_MR_MODE_EN;

	/* Extract CAS and WR in DTR0 */
	cas = (REG(DTR0) & DTR0_CAS_MASK) >> DTR0_CAS_FD_S;
	buswidth = (REG(DCR) & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S;
	switch (cas){
		case 0:
			mr[0] = mr[0] | DDR1_MR_CAS_25;
			break;
		case 1:
			mr[0] = mr[0] | DDR1_MR_CAS_2;
			break;
		case 2:
			mr[0] = mr[0] | DDR1_MR_CAS_3;
			break;
		default:
			mr[0] = mr[0] | DDR1_MR_CAS_3;
			break;
			
	}

	switch (buswidth){
		case 0:
			mr[0] = mr[0] | DDR1_MR_BURST_4;
			break;
		case 1:
			mr[0] = mr[0] | DDR1_MR_BURST_2;
			break;
		default:
			mr[0] = mr[0] | DDR1_MR_BURST_2;
			break;
	}

	return;
}


/* Function Name: 
 * 	_DTR_wr
 * Descripton:
 *	Get WTR setting of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Clock cycles of WR.
 */
unsigned int _DTR_wr(void)
{
	unsigned int wr;

	wr = ((REG(DTR0) & DTR0_WR_MASK) >> DTR0_WR_FD_S);

	return wr+1;
}

/* Function Name: 
 * 	_DTR_wtr
 * Descripton:
 *	Get WTR setting of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Clock cycles of RRD.
 */
unsigned int _DTR_rrd(void)
{
	unsigned int rrd;

	rrd = ((REG(DTR1) & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);

	return rrd+1;
}


/* Function Name: 
 * 	_DTR_rtp
 * Descripton:
 *	Get RTP setting of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Clock cycles of RTP latency
 */
unsigned int _DTR_rtp(void)
{
	unsigned int rtp;

	rtp = ((REG(DTR0) & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);

	return rtp+1;
}

/* Function Name: 
 * 	_DTR_wtr
 * Descripton:
 *	Get WTR setting of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Clock cycles of WTR latency
 */
unsigned int _DTR_wtr(void)
{
	unsigned int wtr;

	wtr = ((REG(DTR0) & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);

	return wtr+1;
}

/* Function Name: 
 * 	_DTR_cwl
 * Descripton:
 *	Get Write CAS Latency of MEMCTL
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Write Cas latency.
 */
unsigned int _DTR_cwl(void)
{
	unsigned int cwl;

	cwl = ((REG(DTR0) & DTR0_CWL_MASK) >> DTR0_CWL_FD_S);

	return cwl+1;
}

/* Function Name: 
 * 	_DTR_two_cas
 * Descripton:
 *	Get 2 times of CAS setting of DCR(cas).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	Cas latency x 2. (Why x2? We have 2.5 cas latency setting.)
 */
unsigned int _DTR_two_cas(void)
{
	unsigned int cas;

	cas = ((REG(DTR0) & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);

	switch (cas) {
		case 0:
			return 5; /* 2.5 x 2 */
		default:
			return (cas+1)*2;
	}
}


/* Function Name: 
 * 	_DCR_get_buswidth
 * Descripton:
 *	Get bus width setting of DCR(dbuswid).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of bit of the bus width setting in DCR(dbuswid)
 */
unsigned int _DCR_get_buswidth(void)
{
	unsigned int buswidth;

	buswidth = ((REG(DCR) & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S);

	switch (buswidth) {
		case 0:
			return (8);
		case 1:
			return (16);
		case 2:
			return (32); 
		default:
			_memctl_debug_printf("#Error: error bus width setting (11)\n");
			return 0;
	}

}


/* Function Name: 
 * 	_DCR_get_chipsel
 * Descripton:
 *	Get chip number setting of DCR(dchipsel).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of chips of the chip number setting in DCR(dchipsel)
 */
unsigned int _DCR_get_chipsel(void)
{
	unsigned int chipsel;

	chipsel = ((REG(DCR) & DCR_DCHIPSEL_MASK) >> DCR_DCHIPSEL_FD_S);

	if(chipsel)
		return 2;
	else
		return 1;
}


/* Function Name: 
 * 	_DCR_get_rowcnt
 * Descripton:
 *	Get row count setting of DCR(rowcnt).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of row count of the row count setting in DCR(rowcnt)
 */
unsigned int _DCR_get_rowcnt(void)
{
	unsigned int rowcnt;

	rowcnt = ((REG(DCR) & DCR_ROWCNT_MASK) >> DCR_ROWCNT_FD_S);
	
	return (2048 << rowcnt);
}


/* Function Name: 
 * 	_DCR_get_colcnt
 * Descripton:
 *	Get column count setting of DCR(colcnt).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of columns of the column count setting in DCR(colcnt)
 */
unsigned int _DCR_get_colcnt(void)
{
	unsigned int colcnt;

	colcnt = ((REG(DCR) & DCR_COLCNT_MASK) >> DCR_COLCNT_FD_S);

	if(4 < colcnt){
		_memctl_debug_printf("#Eror: error colcnt setting. ( > 4)\n");		
		return 0;
	}
	else
		return (256 << colcnt);

}


/* Function Name: 
 * 	_DCR_get_bankcnt
 * Descripton:
 *	Get bank count setting of DCR(bankcnt).
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	the number of banks of the bank count setting in DCR(bankcnt)
 */
unsigned int _DCR_get_bankcnt(void)
{
	unsigned int bankcnt;

	bankcnt = ((REG(DCR) & DCR_BANKCNT_MASK) >> DCR_BANKCNT_FD_S);
	
	switch (bankcnt)
	{
		case 0:
			return 2;
		case 1:
			return 4;
		case 2:
			return 8;
	}
}


/*Board DQS Range (mode, MHz, min tap, max tap)*/
/* Mode: DLL/Digital */
tap_info_t DQS_RANGE[10] =	{
					/*100MHz*/{DLL_MODE, 100, 3, 16} , {DIGITAL_MODE, 100, 1, 23}, 
					/*125MHz*/{DLL_MODE, 125, 3, 13} , {DIGITAL_MODE, 125, 1, 18},
					/*150MHz*/{DLL_MODE, 150, 3, 12} , {DIGITAL_MODE, 150, 1, 13},
					/*175MHz*/{DLL_MODE, 175, 3, 11} , {DIGITAL_MODE, 175, 1, 10},
					/*200MHz*/{DLL_MODE, 200, 4, 10} , {DIGITAL_MODE, 200, 1, 8}
				};

/*28M Demoboard: Write 90 phase range (mode, MHz, min tap, max tap) */
tap_info_t W90_RANGE[10] =	{
					/*100MHz*/{DLL_MODE, 100, 0, 19} , {DIGITAL_MODE, 100, 0, 19},
					/*125MHz*/{DLL_MODE, 125, 0, 16} , {DIGITAL_MODE, 125, 0, 16},
					/*150MHz*/{DLL_MODE, 150, 0, 14} , {DIGITAL_MODE, 150, 0, 14},
					/*175MHz*/{DLL_MODE, 175, 1, 13} , {DIGITAL_MODE, 175, 1, 13},
					/*200MHz*/{DLL_MODE, 200, 2, 12} , {DIGITAL_MODE, 200, 2, 12},
				};

/* Function Name: 
 * 	memctlc_check_DQS_range
 * Descripton:
 *	Check the applied value of DQS delay taps and compared with the experimented data DQS_RANGE.
 *	It shows message related to DQS taps.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
void memctlc_check_DQS_range(void)
{
	unsigned int dqs0_tap, dqs1_tap, cal_mode;
	unsigned int dram_freq_mhz, index;
	
	dqs0_tap = (REG(DDCR) & DDCR_DQS0_TAP_MASK) >> DDCR_DQS0_TAP_FD_S;
	dqs1_tap = (REG(DDCR) & DDCR_DQS1_TAP_MASK) >> DDCR_DQS1_TAP_FD_S;
	cal_mode = (REG(DDCR) & DDCR_CAL_MODE_MASK) >> DDCR_CAL_MODE_FD_S;

	_memctl_debug_printf("DQ/DQS Related Delay:\n");
	if(cal_mode)
		_memctl_debug_printf("\tCAL_MODE: Digtial\n");
	else
		_memctl_debug_printf("\tCAL_MODE: DLL\n");


	_memctl_debug_printf("\tDQS0_tap = %d, DQS1_tap = %d : ", dqs0_tap, dqs1_tap);
	dram_freq_mhz = board_DRAM_freq_mhz();
	for(index = 0; index < (sizeof(DQS_RANGE)/sizeof(tap_info_t)); index++){
		if(DQS_RANGE[index].mode == cal_mode){
			if(DQS_RANGE[index].mhz >= dram_freq_mhz)
				break;
		}
	}

	if(index >= (sizeof(DQS_RANGE)/sizeof(tap_info_t)))
		_memctl_debug_printf("#Error DRAM frequency %dMHz\n", dram_freq_mhz);

	if(dqs0_tap < DQS_RANGE[index].tap_min || dqs0_tap > DQS_RANGE[index].tap_max \
			|| dqs1_tap < DQS_RANGE[index].tap_min || dqs1_tap > DQS_RANGE[index].tap_max)
		_memctl_debug_printf("#Warning: current DQS tap setting may not be stable\n");
	else
		_memctl_debug_printf("ok ");

	_memctl_debug_printf("(Reference DQS taps %d ~ %d on %dMHz)\n", DQS_RANGE[index].tap_min, DQS_RANGE[index].tap_max, DQS_RANGE[index].mhz);

	return;
}


/* Function Name: 
 * 	memctlc_check_90phase_range
 * Descripton:
 *	Check the applied value of phase shift 90 delay taps and compared with the experimented data W90_RANGE.
 *	It shows message related to phase shift 90 taps.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	None
 */
void memctlc_check_90phase_range(void)
{
	unsigned int phase_tap, cal_mode;
	unsigned int dram_freq_mhz, index;
	
	phase_tap = (REG(DCDR) & DCDR_PHASE_SHIFT_MASK) >> DCDR_PHASE_SHIFT_FD_S;
	cal_mode = (REG(DDCR) & DDCR_CAL_MODE_MASK) >> DDCR_CAL_MODE_FD_S;

	_memctl_debug_printf("\tphase_shift_90_tap = %d: ", phase_tap);

	dram_freq_mhz = board_DRAM_freq_mhz();

	for(index = 0; index < (sizeof(W90_RANGE)/sizeof(tap_info_t)); index++){
		if(W90_RANGE[index].mode == cal_mode){
			if(W90_RANGE[index].mhz >= dram_freq_mhz)
				break;
		}
	}

	if(index >= (sizeof(W90_RANGE)/sizeof(tap_info_t)))
		_memctl_debug_printf("#Error DRAM frequency %dMHz\n", dram_freq_mhz);

	if(phase_tap < W90_RANGE[index].tap_min || phase_tap > W90_RANGE[index].tap_max)
		_memctl_debug_printf("#Warnning: Phase shift 90 taps setting may not be stable\n");
	else
		_memctl_debug_printf("ok ");

	_memctl_debug_printf("(Reference phase shift 90 taps %d ~ %d on %dMHz)\n", W90_RANGE[index].tap_min, W90_RANGE[index].tap_max, W90_RANGE[index].mhz);

	return;
}



/* Function Name: 
 * 	memctlc_check_DCR
 * Descripton:
 *	Check the DCR setting of the current environment.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DCR(void)
{

	unsigned int	two_cas, buswidth, chipsel, rowcnt;
	unsigned int	colcnt, bankcnt, dram_total_size;
	//unsigned int	dram_freq_mhz;


	/* Get board DRAM freq. */
	//dram_freq_mhz = board_DRAM_freq_mhz(); 

	/* 
	 * Check DCR
	 */
	/* 1. Bus width     */
	buswidth = _DCR_get_buswidth();
	//printf("\tbus width = %dbit\n", buswidth);
	/* 2. Chip select   */
	chipsel = _DCR_get_chipsel();
	//printf("\tnumber of chips = %d\n", chipsel);
	/* 3. Row count     */
	rowcnt = _DCR_get_rowcnt();
	//printf("\trow count = %d\n", rowcnt);
	/* 4. Column count  */
	colcnt = _DCR_get_colcnt();
	//printf("\tcolumn count = %d\n", colcnt);
	/* 5. Bank count    */
	bankcnt = _DCR_get_bankcnt();
	//printf("\tbank count = %d\n", bankcnt);

	/* 6. Show total DRAM size */
	dram_total_size = rowcnt*colcnt*(buswidth/8)*chipsel*bankcnt;
	printf("\tDRAM Size = %d  MB\n", (dram_total_size/1024/1024));

#if defined(CONFIG_POST_ENABLE)
	REG32(0xb8000f00) = dram_total_size; // write dram total size to a dummy register.
#endif

	return;
}


/* Function Name: 
 * 	memctlc_check_DTR_DDR3
 * Descripton:
 *	Check the DTR setting of the current environment for DDR3 SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DTR_DDR3()
{
	unsigned int	two_cas;
	unsigned int	min_freq_mhz, max_freq_mhz;
	unsigned int	tcwl, wr, std_rtp_ns, std_rrd_ns;
	unsigned int	std_ras_ns, std_wr_ns;
	unsigned int	std_refi_ns, std_wtr_ns, std_fawg_ns;
	unsigned int	std_rp_ns, std_rcd_ns, std_rfc_ns;
	unsigned int	sug_dtr[3], sug_mr[4];
	unsigned int	dram_size;
	unsigned int	cas_10_ns, cas, wtr, rtp, rrd, _rfc_ns;


	unsigned int dram_freq_mhz = board_DRAM_freq_mhz();


	/* 
	 * Check DTR
	 */
	std_wr_ns  = DDR3_STD_WR_NS;
	std_rtp_ns  = DDR3_STD_RTP_NS;
	std_wtr_ns  = DDR3_STD_WTR_NS;
	std_refi_ns = DDR3_STD_REFI_NS; /* 7.8us */
	std_rp_ns  = DDR3_STD_RP_NS;
	std_rcd_ns = DDR3_STD_RCD_NS;
	std_rrd_ns = DDR3_STD_RRD_NS;
	std_fawg_ns = DDR3_STD_FAWG_NS;
	std_ras_ns = DDR3_STD_RAS_NS;
	
	dram_size = memctlc_dram_size()/_get_DRAM_csnum();
	switch (dram_size){
		case 0x4000000:
			_rfc_ns = DDR3_STD_RFC_64MB_NS;
			break;
		case 0x8000000:
			_rfc_ns = DDR3_STD_RFC_128MB_NS;
			break;
		case 0x10000000:
			_rfc_ns = DDR3_STD_RFC_256MB_NS;
			break;
		case 0x20000000:
			_rfc_ns = DDR3_STD_RFC_512MB_NS;
			break;
		default:
			_rfc_ns = DDR3_STD_RFC_1GB_NS;
			break;
	}

	std_rfc_ns = DDR3_STD_RFC_NS;
	//std_rfc_ns = _rfc_ns;

	_memctl_debug_printf("DTR Checking Rules: \n");
	_memctl_debug_printf("\tt_refi = %dns, \n", std_refi_ns);
	_memctl_debug_printf("\tt_wr = %dns\n", std_wr_ns);
	_memctl_debug_printf("\tt_rtp = %dns\n", std_rtp_ns);
	_memctl_debug_printf("\tt_wtr = %dns\n", std_wtr_ns);
	_memctl_debug_printf("\tt_rp = %dns\n", std_rp_ns);
	_memctl_debug_printf("\tt_rcd = %dns\n", std_rcd_ns);
	_memctl_debug_printf("\tt_fawg = %dns\n", std_fawg_ns);	
	_memctl_debug_printf("\tt_rfc = %dns, _rfc_ns(%d)\n", std_rfc_ns, _rfc_ns);
	_memctl_debug_printf("\tt_ras = %dns\n", std_ras_ns);

	_memctl_debug_printf("Checking Current setting: \n");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	_memctl_debug_printf("\n\tcas(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 22)
		_memctl_debug_printf("#Warnning: No CAS > 11 for DDR3 SDRAM\n");
	else if((two_cas == 10) && (DDR3_CAS5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 12) && (DDR3_CAS6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 14) && (DDR3_CAS7_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 16) && (DDR3_CAS8_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 18) && (DDR3_CAS9_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 20) && (DDR3_CAS10_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 22) && (DDR3_CAS11_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else
		_memctl_debug_printf("ok\n");


	/* 2. T_WR  (15ns) */
	wr = _DTR_wr();
	_memctl_debug_printf("\twr(%d): ",wr);
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wr_ns, max_freq_mhz);
	if((wr < 5) | (wr > 12)){
		_memctl_debug_printf("#Warnning: DTR0(wr) setting (%d clks) may vilolates the min./max. requirement(5~12 clks) for current setting\n", wr+1);
	}else{
		_memctl_debug_printf("ok\n");
	}

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	_memctl_debug_printf("\n\tcwl(%d): ", tcwl);
	if(tcwl > 8)
		printf("#Warnning: No CWL > 8 for DDR3 SDRAM\n");
	else if((tcwl == 5) && (DDR3_CWL5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((tcwl == 6) && (DDR3_CWL6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((tcwl == 7) && (DDR3_CWL7_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((tcwl == 8) && (DDR3_CAS8_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CWL might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else
		_memctl_debug_printf("ok\n");


	/* 4. T_RTP  (20ns) */
	rtp = _DTR_rtp();
	_memctl_debug_printf("\trtp(%d): ",rtp);
	
	max_freq_mhz = _DTR_rtp_frq_mhz(std_rtp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rtp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rtp_ns, max_freq_mhz);

	if(rtp < 4){
		printf("#Warnning: DTR1(rtp) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n", rtp);
	}else{
		_memctl_debug_printf("ok\n");
	}


	/* 5. T_WTR  (20ns) */
	wtr = _DTR_wtr();
	_memctl_debug_printf("\twtr(%d): ",wtr);
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wtr_ns, max_freq_mhz);

	if(wtr < 4){
		printf("#Warnning: DTR1(wtr) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n", wtr);
	}else{
		_memctl_debug_printf("ok\n");
	}

	/* 6. Refresh period (7.8us) */
	_memctl_debug_printf("\trefresh cycles: ");
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		_memctl_debug_printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			printf("\n\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dms.\n", std_refi_ns);
		else
			_memctl_debug_printf("ok\n");
	}
	else
		_memctl_debug_printf("DRAM freq. have to > 1000MHz\n");


	/* 7. T_RP  (20ns) */
	//_memctl_debug_printf("\trp(%d): ");
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rp_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");
	/* 8. T_RCD (20ns) */
	//_memctl_debug_printf("\trcd: ");
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rcd_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 9. T_RRD (20ns) */
	rrd = _DTR_rrd();
	//_memctl_debug_printf("\trrd: ");
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rrd_ns, max_freq_mhz);
	
	if(rrd < 4){
		printf("#Warnning: DTR1(rrd) setting (%d clks) may vilolates the min. requirement(4 clks) for current setting\n", rrd);
	}else{
		_memctl_debug_printf("ok\n");
	}
	/* 10. T_FAWG (20ns) */
	//_memctl_debug_printf("\tfawg: ");
	max_freq_mhz = _DTR_fawg_frq_mhz(std_fawg_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(fawg) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_fawg_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 11. T_RFC */
	//_memctl_debug_printf("\trfc: ");
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rfc_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 12. T_RAS (45ns) */
	//_memctl_debug_printf("\tras: ");
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_ras_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	if(dram_freq_mhz >= DDR3_CAS10_MAX_MHZ){
		cas_10_ns = (11 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS9_MAX_MHZ){
		cas_10_ns = (10 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS8_MAX_MHZ){
		cas_10_ns = (9 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS7_MAX_MHZ){
		cas_10_ns = (8 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS6_MAX_MHZ){
		cas_10_ns = (7 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR3_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}

        _DTR_suggestion(sug_dtr, DDR3_STD_REFI_NS, DDR3_STD_RP_NS, \
                        DDR3_STD_RCD_NS, DDR3_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
                        DDR3_STD_RRD_NS, DDR3_STD_FAWG_NS, DDR3_STD_WTR_NS, DDR3_STD_RTP_NS,\
                        cas_10_ns, dram_freq_mhz);

	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 4){
		cas = 4;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}

	/* DDR3 write cas */
	if(dram_freq_mhz >= DDR3_CWL7_MAX_MHZ){
		tcwl = 7;
	}else if(dram_freq_mhz >= DDR3_CWL6_MAX_MHZ){
		tcwl = 6;
	}else if(dram_freq_mhz >= DDR3_CWL5_MAX_MHZ){
		tcwl = 5;
	}else{
		tcwl = 4;
	}

	sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);

	/* DDR3 Write recovery maximum == 12 , min == 5 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 11){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (11 << DTR0_WR_FD_S);
	}else if(wr < 4){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (4 << DTR0_WR_FD_S);
	}else{}

	/* DDR3 Write to read delay cycle at least 4 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 3){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (3 << DTR0_WTR_FD_S);
	}

	/* DDR3 RTP delay cycle at least 4 clock cycles */
	rtp = ((sug_dtr[0] & DTR0_RTP_MASK) >> DTR0_RTP_FD_S);
	if(rtp < 3){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_RTP_MASK)) | (3 << DTR0_RTP_FD_S);
	}

	/* DDR3 RRD delay cycle at least 4 clock cycles */
	rrd = ((sug_dtr[1] & DTR1_RRD_MASK) >> DTR1_RRD_FD_S);
	if(rrd < 3){
		sug_dtr[1] = (sug_dtr[1] & (~DTR1_RRD_MASK)) | (3 << DTR1_RRD_FD_S);
	}

	_DTR_DDR3_MRS_setting(sug_dtr, sug_mr);
	_memctl_debug_printf("\nWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	_memctl_debug_printf("\tApply suggested DTR by\n\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	_memctl_debug_printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DTR2, sug_dtr[2], DMCR, sug_mr[0]);
	_memctl_debug_printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DMCR, sug_mr[1], DMCR, sug_mr[2]);
	

	#if 1//def AUTO_SET_DRAM_PARAMETER
	
	//_memctl_debug_printf("\n=>Auto Set DDR3's DTR by suggested value\n"); 
	//REG32(DTR0) = sug_dtr[0];
	//REG32(DTR1) = sug_dtr[1];
	//REG32(DTR2) = sug_dtr[2]; //only correct RFC

	//_memctl_debug_printf("\n=>Auto Set DRAM Mode Register by suggested value\n"); 
	//REG32(DMCR) = sug_mr[0];
	//REG32(DMCR) = sug_mr[1];
	//REG32(DMCR) = sug_mr[2];
	#endif


}


#if 1
/* Function Name: 
 * 	memctlc_check_DTR_DDR2
 * Descripton:
 *	Check the DTR setting of the current environment for DDR2 SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DTR_DDR2()
{
	unsigned int	two_cas;
	unsigned int	min_freq_mhz, max_freq_mhz;
	unsigned int	tcwl, wr, std_rtp_ns, std_rrd_ns;
	unsigned int	std_ras_ns, std_wr_ns;
	unsigned int	std_refi_ns, std_wtr_ns, std_fawg_ns;
	unsigned int	std_rp_ns, std_rcd_ns, std_rfc_ns;
	unsigned int	sug_dtr[3], sug_mr[4];
	unsigned int	dram_size;
	unsigned int	cas_10_ns, cas, wtr, _rfc_ns;


	unsigned int dram_freq_mhz = board_DRAM_freq_mhz();

	/* 
	 * Check DTR
	 */
	std_wr_ns  = DDR2_STD_WR_NS;
	std_rtp_ns  = DDR2_STD_RTP_NS;
	std_wtr_ns  = DDR2_STD_WTR_NS;
	//std_ref_ms = DDR2_STD_REF_MS; /* 64ms */
	std_refi_ns = DDR2_STD_REFI_NS; /* 7.8us */
	std_rp_ns  = DDR2_STD_RP_NS;
	std_rcd_ns = DDR2_STD_RCD_NS;
	std_rrd_ns = DDR2_STD_RRD_NS;
	std_fawg_ns = DDR2_STD_FAWG_NS;
	std_ras_ns = DDR2_STD_RAS_NS;
	
	dram_size = memctlc_dram_size()/_get_DRAM_csnum();
	switch (dram_size){
		case 0x2000000:
			_rfc_ns = DDR2_STD_RFC_32MB_NS;
			break;
		case 0x4000000:
			_rfc_ns = DDR2_STD_RFC_64MB_NS;
			break;
		case 0x8000000:
			_rfc_ns = DDR2_STD_RFC_128MB_NS;
			break;
		case 0x10000000:
			_rfc_ns = DDR2_STD_RFC_256MB_NS;
			break;
		case 0x20000000:
			_rfc_ns = DDR2_STD_RFC_512MB_NS;
			break;
		default:
			_rfc_ns = DDR2_STD_RFC_512MB_NS;
			break;
	}

	
	std_rfc_ns = DDR2_STD_RFC_NS;
	//std_rfc_ns =_rfc_ns;

	_memctl_debug_printf("\nDTR setting,DRAM size(MB):%d \n",dram_size/0x00100000);

	_memctl_debug_printf("DTR Checking Rules: \n");
	_memctl_debug_printf("\tRefresh cycles = %d ns, \n", std_refi_ns);
	_memctl_debug_printf("\tt_wr = %dns\n", std_wr_ns);
	_memctl_debug_printf("\tt_rtp = %dns\n", std_rtp_ns);
	_memctl_debug_printf("\tt_wtr = %dns\n", std_wtr_ns);
	_memctl_debug_printf("\tt_rp = %dns\n", std_rp_ns);
	_memctl_debug_printf("\tt_rcd = %dns\n", std_rcd_ns);
	_memctl_debug_printf("\tt_rrd = %dns\n", std_rrd_ns);
	_memctl_debug_printf("\tt_fawg = %dns\n", std_fawg_ns);
	_memctl_debug_printf("\tt_ras = %dns\n", std_ras_ns);
	_memctl_debug_printf("\tt_rfc = %dns, _rfc_ns(%d)\n", std_rfc_ns, _rfc_ns);

	_memctl_debug_printf("DDR2 Checking Current setting: \n");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	_memctl_debug_printf("\n\tcas(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 16)
		printf("#Warnning: No CAS > 7 for DDR2 SDRAM\n");
	else if((two_cas == 4) && (DDR2_CAS2_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 6) && (DDR2_CAS3_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 8) && (DDR2_CAS4_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 10) && (DDR2_CAS5_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 12) && (DDR2_CAS6_MAX_MHZ < dram_freq_mhz))
		printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else
		_memctl_debug_printf("ok\n");
	
	/* 2. T_WR  (15ns) */
	_memctl_debug_printf("\twr: ");
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wr_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	_memctl_debug_printf("\tcwl: %d", tcwl);
	if(tcwl != ((two_cas/2) - 1) )
		printf("#Warnning: DTR0(cwl) != cas-1 DRAM clock\n");
	else
		_memctl_debug_printf("ok\n");

	/* 4. T_RTP  (20ns) */
	_memctl_debug_printf("\trtp: ");
	max_freq_mhz = _DTR_rtp_frq_mhz(std_rtp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rtp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rtp_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 5. T_WTR  (20ns) */
	_memctl_debug_printf("\twtr: ");
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wtr_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 6. Refresh period (64ms) */
	_memctl_debug_printf("\trefresh cycles: ");
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		_memctl_debug_printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			printf("\n\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dns.\n", std_refi_ns);
		else
			_memctl_debug_printf("ok\n");
	}
	else
		_memctl_debug_printf("DRAM freq. have to > 1000MHz\n");


	/* 7. T_RP  (20ns) */
	_memctl_debug_printf("\trp: ");
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rp_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");
	/* 8. T_RCD (20ns) */
	_memctl_debug_printf("\trcd: ");
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rcd_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 9. T_RRD (20ns) */
	_memctl_debug_printf("\trrd: ");
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rrd_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 10. T_FAWG (20ns) */
	_memctl_debug_printf("\tfawg: ");
	max_freq_mhz = _DTR_fawg_frq_mhz(std_fawg_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(fawg) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_fawg_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 11. T_RFC */
	_memctl_debug_printf("\trfc: ");
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rfc_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 12. T_RAS (45ns) */
	_memctl_debug_printf("\tras: ");
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_ras_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 13. Suggest DTR Setting for Current Memory freq. */
	if(dram_freq_mhz >= DDR2_CAS6_MAX_MHZ){
		cas_10_ns = (7 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS4_MAX_MHZ){
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS3_MAX_MHZ){
		cas_10_ns = (4 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS2_MAX_MHZ){
		cas_10_ns = (3 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (2 * 1000 * 10)/dram_freq_mhz;
	}
	_DTR_suggestion(sug_dtr, DDR2_STD_REFI_NS, DDR2_STD_RP_NS, \
			DDR2_STD_RCD_NS, DDR2_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
			DDR2_STD_RRD_NS, DDR2_STD_FAWG_NS, DDR2_STD_WTR_NS, DDR2_STD_RTP_NS,\
			cas_10_ns, dram_freq_mhz);

	/* Check for Minimum CAS support */
	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 2){
		cas = 2;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}
	/* DDR2 write cas == read cas - 1*/
	tcwl = cas - 1;
	if(tcwl < 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);
	}else{
		_memctl_debug_printf("\tWarnning: wrong tCWL computation\n");
	}

	/* DDR2 Write recovery maximum == 6 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (5 << DTR0_WR_FD_S);
	}

	/* DDR2 Write to read delay cycle at least 2 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 1){
		wtr = 1;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (wtr << DTR0_WTR_FD_S);
	}

	_DTR_DDR2_MRS_setting(sug_dtr, sug_mr);
	_memctl_debug_printf("\nWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	_memctl_debug_printf("\tApply suggested DTR by\n\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	_memctl_debug_printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DTR2, sug_dtr[2], DMCR, sug_mr[0]);
	_memctl_debug_printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DMCR, sug_mr[1], DMCR, sug_mr[2]);
	
#if 1//def AUTO_SET_DRAM_PARAMETER	
	//_memctl_debug_printf("\n=>Auto Set DDR2's DTR by suggested value\n"); 
	//REG32(DTR0) = sug_dtr[0];
	//REG32(DTR1) = sug_dtr[1];
	//REG32(DTR2) = sug_dtr[2]; //only correct RFC

	//_memctl_debug_printf("\n=>Auto Set DRAM Mode Register by suggested value\n"); 
	//REG32(DMCR) = sug_mr[0];
	//REG32(DMCR) = sug_mr[1];
	//REG32(DMCR) = sug_mr[2];
#endif

	

}

#else



/* Function Name: 
 * 	memctlc_check_DTR_DDR2
 * Descripton:
 *	Check the DTR setting of the current environment for DDR2 SDRAM.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DTR_DDR2(unsigned int dram_freq_mhz)
{
	unsigned int	two_cas;
	unsigned int	min_freq_mhz, max_freq_mhz;
	unsigned int	tcwl, wr, std_rtp_ns, std_rrd_ns;
	unsigned int	std_ras_ns, std_wr_ns;
	unsigned int	std_refi_ns, std_wtr_ns, std_fawg_ns;
	unsigned int	std_rp_ns, std_rcd_ns, std_rfc_ns;
	unsigned int	sug_dtr[3], sug_mr[4];
	unsigned int	dram_size;
	unsigned int	cas_10_ns, cas, wtr, _rfc_ns;


	/* 
	 * Check DTR
	 */
	std_wr_ns  = DDR2_STD_WR_NS;
	std_rtp_ns  = DDR2_STD_RTP_NS;
	std_wtr_ns  = DDR2_STD_WTR_NS;
	std_refi_ns = DDR2_STD_REFI_NS; /* 7.8us */
	std_rp_ns  = DDR2_STD_RP_NS;
	std_rcd_ns = DDR2_STD_RCD_NS;
	std_rrd_ns = DDR2_STD_RRD_NS;
	std_fawg_ns = DDR2_STD_FAWG_NS;
	std_ras_ns = DDR2_STD_RAS_NS;
	
	dram_size = memctlc_dram_size()/_get_DRAM_csnum();
	switch (dram_size){
		case 0x2000000:
			_rfc_ns = DDR2_STD_RFC_32MB_NS;
			break;
		case 0x4000000:
			_rfc_ns = DDR2_STD_RFC_64MB_NS;
			break;
		case 0x8000000:
			_rfc_ns = DDR2_STD_RFC_128MB_NS;
			break;
		case 0x10000000:
			_rfc_ns = DDR2_STD_RFC_256MB_NS;
			break;
		case 0x20000000:
			_rfc_ns = DDR2_STD_RFC_512MB_NS;
			break;
		default:
			_rfc_ns = DDR2_STD_RFC_512MB_NS;
			break;
	}

	std_rfc_ns = DDR2_STD_RFC_NS;


	_memctl_debug_printf("DTR Checking Rules: \n");
	_memctl_debug_printf("\tt_refi = %dns, \n", std_refi_ns);
	_memctl_debug_printf("\tt_wr = %dns\n", std_wr_ns);
	_memctl_debug_printf("\tt_rtp = %dns\n", std_rtp_ns);
	_memctl_debug_printf("\tt_wtr = %dns\n", std_wtr_ns);
	_memctl_debug_printf("\tt_rp = %dns\n", std_rp_ns);
	_memctl_debug_printf("\tt_rcd = %dns\n", std_rcd_ns);
	_memctl_debug_printf("\tt_fawg = %dns\n", std_fawg_ns);
	_memctl_debug_printf("\tt_ras = %dns\n", std_ras_ns);
	_memctl_debug_printf("\tt_rfc = %dns, _rfc_ns(%d)\n", std_rfc_ns, _rfc_ns);

	_memctl_debug_printf("Checking Current setting: \n");
	/* 1. CAS latency   */
	two_cas = _DTR_two_cas();
	_memctl_debug_printf("\tcas(%d.%d): ", (two_cas/2), ((two_cas*10)/2)%10);
	if(two_cas > 12)
		_memctl_debug_printf("#Warnning: No CAS > 6 for DDR2 SDRAM\n");
	else if((two_cas == 4) && (DDR2_CAS2_MAX_MHZ < dram_freq_mhz))
		_memctl_debug_printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 6) && (DDR2_CAS3_MAX_MHZ < dram_freq_mhz))
		_memctl_debug_printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 8) && (DDR2_CAS4_MAX_MHZ < dram_freq_mhz))
		_memctl_debug_printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 10) && (DDR2_CAS5_MAX_MHZ < dram_freq_mhz))
		_memctl_debug_printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else if((two_cas == 12) && (DDR2_CAS6_MAX_MHZ < dram_freq_mhz))
		_memctl_debug_printf("#Warnning: CAS might too small for current DRAM freq.(%dMHz)\n", dram_freq_mhz);
	else
		_memctl_debug_printf("ok\n");
	
	/* 2. T_WR  (15ns) */
	_memctl_debug_printf("\twr: ");
	max_freq_mhz = _DTR_wr_frq_mhz(std_wr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR0(wr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wr_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 3. CAS Write latency */
	tcwl = _DTR_cwl();
	_memctl_debug_printf("\tcwl(%d): ", tcwl);
	if(tcwl != ((two_cas/2) - 1) )
		_memctl_debug_printf("#Warnning: DTR0(cwl) != cas-1 DRAM clock\n");
	else
		_memctl_debug_printf("ok\n");

	/* 4. T_RTP  (20ns) */
	_memctl_debug_printf("\trtp: ");
	max_freq_mhz = _DTR_rtp_frq_mhz(std_rtp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR1(rtp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rtp_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 5. T_WTR  (20ns) */
	_memctl_debug_printf("\twtr: ");
	max_freq_mhz = _DTR_wtr_frq_mhz(std_wtr_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR1(wtr) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_wtr_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 6. Refresh period (64ms) */
	_memctl_debug_printf("\trefresh cycles: ");
	min_freq_mhz  = _DTR_refresh_freq_mhz(std_refi_ns);
	if(0 != min_freq_mhz){
		_memctl_debug_printf("(DRAM freq. have to >= %dMHz) ", min_freq_mhz);
		if(dram_freq_mhz < min_freq_mhz)
			_memctl_debug_printf("\n\t#Warnning: DTR setting may vilolate the requirement of DRAM refresh in %dns.\n", std_refi_ns);
		else
			_memctl_debug_printf("ok\n");
	}
	else
		_memctl_debug_printf("DRAM freq. have to > 1000MHz\n");


	/* 7. T_RP  (20ns) */
	_memctl_debug_printf("\trp: ");
	max_freq_mhz = _DTR_rp_frq_mhz(std_rp_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR1(rp) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rp_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");
	/* 8. T_RCD (20ns) */
	_memctl_debug_printf("\trcd: ");
	max_freq_mhz = _DTR_rcd_frq_mhz(std_rcd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR(rcd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rcd_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 9. T_RRD (20ns) */
	_memctl_debug_printf("\trrd: ");
	max_freq_mhz = _DTR_rrd_frq_mhz(std_rrd_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR(rrd) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rrd_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 10. T_FAWG (20ns) */
	_memctl_debug_printf("\tfawg: ");
	max_freq_mhz = _DTR_fawg_frq_mhz(std_fawg_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR(fawg) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_fawg_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 11. T_RFC */
	_memctl_debug_printf("\trfc: ");
	max_freq_mhz = _DTR_rfc_frq_mhz(std_rfc_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR(rfc) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_rfc_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 12. T_RAS (45ns) */
	_memctl_debug_printf("\tras: ");
	max_freq_mhz = _DTR_ras_frq_mhz(std_ras_ns);
	if(dram_freq_mhz > max_freq_mhz)
		_memctl_debug_printf("#Warnning: DTR(ras) setting may vilolates the requirement (%dns). Max. %dMHz for current setting\n", std_ras_ns, max_freq_mhz);
	else
		_memctl_debug_printf("ok\n");

	/* 13. Suggest DTR Setting for Current Memory freq. */
	if(dram_freq_mhz >= DDR2_CAS5_MAX_MHZ){
		cas_10_ns = (6 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS4_MAX_MHZ){
		cas_10_ns = (5 * 1000 * 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS3_MAX_MHZ){
		cas_10_ns = (4 * 1000* 10)/dram_freq_mhz;
	}else if(dram_freq_mhz >= DDR2_CAS2_MAX_MHZ){
		cas_10_ns = (3 * 1000 * 10)/dram_freq_mhz;
	}else{
		cas_10_ns = (2 * 1000 * 10)/dram_freq_mhz;
	}
	_DTR_suggestion(sug_dtr, DDR2_STD_REFI_NS, DDR2_STD_RP_NS, \
			DDR2_STD_RCD_NS, DDR2_STD_RAS_NS, std_rfc_ns, DDR2_STD_WR_NS,\
			DDR2_STD_RRD_NS, DDR2_STD_FAWG_NS, DDR2_STD_WTR_NS, DDR2_STD_RTP_NS,\
			cas_10_ns, dram_freq_mhz);

	/* Check for Minimum CAS support */
	cas = ((sug_dtr[0] & DTR0_CAS_MASK) >> DTR0_CAS_FD_S);
	if(cas < 2){
		cas = 2;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CAS_MASK)) | (cas << DTR0_CAS_FD_S);
	}
	/* DDR2 write cas == read cas - 1*/
	tcwl = cas - 1;
	if(tcwl < 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_CWL_MASK)) | (tcwl << DTR0_CWL_FD_S);
	}else{
		_memctl_debug_printf("\tWorning: wrong tCWL computation\n");
	}

	/* DDR2 Write recovery maximum == 6 */
	wr = ((sug_dtr[0] & DTR0_WR_MASK) >> DTR0_WR_FD_S);
	if(wr > 7){
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WR_MASK)) | (5 << DTR0_WR_FD_S);
	}

	/* DDR2 Write to read delay cycle at least 2 clock cycles */
	wtr = ((sug_dtr[0] & DTR0_WTR_MASK) >> DTR0_WTR_FD_S);
	if(wtr < 1){
		wtr = 1;
		sug_dtr[0] = (sug_dtr[0] & (~DTR0_WTR_MASK)) | (wtr << DTR0_WTR_FD_S);
	}

	_DTR_DDR2_MRS_setting(sug_dtr, sug_mr);
	_memctl_debug_printf("\tWe suggeset DTR setting for current environment: (0x%08x), (0x%08x), (0x%08x)\n",\
			 sug_dtr[0],  sug_dtr[1],  sug_dtr[2]);
	_memctl_debug_printf("\tApply suggested DTR by\n\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n",\
			 DTR0, sug_dtr[0], DTR1, sug_dtr[1]);
	_memctl_debug_printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DTR2, sug_dtr[2], DMCR, sug_mr[0]);
	_memctl_debug_printf("\t\"mw 0x%08x 0x%08x; mw 0x%08x 0x%08x;\"\n", DMCR, sug_mr[1], DMCR, sug_mr[2]);
	

}


#endif


/* Function Name: 
 * 	memctlc_check_DTR
 * Descripton:
 *	Check the DTR setting of the current environment.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	None
 * Note:
 *	- It shows some messages for the result of the checking.
 */
void memctlc_check_DTR()
{

	
	if(memctlc_is_DDR2()){		
		printf("\nDRAM Type: DDR2\n");
		memctlc_check_DTR_DDR2();		
	}else if(memctlc_is_DDR3()){
		printf("\nDRAM Type: DDR3\n");
		memctlc_check_DTR_DDR3();
		
	}else{
		_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		_memctl_debug_printf("Error Unkown DRAM Type\n");
	}
	
	

	
}




#if 0
void show_DRAM_phy_parameters(void)
{
	unsigned int i;
	volatile unsigned int *ptr;

	_memctl_debug_printf("\tSYSREG_DDRCKODL_REG(0x%08x):0x%08x;\n", \
			SYSREG_DDRCKODL_REG, *((unsigned int *)SYSREG_DDRCKODL_REG) );
	ptr = (unsigned int *)DACCR;
	_memctl_debug_printf("\tDACCR(0x%08x):\n", (unsigned int)ptr);
	for(i = 0; i < 10; i++){
		_memctl_debug_printf("\t\t(0x%08x):0x%08x", (unsigned int)ptr, *ptr );
		ptr++;
		_memctl_debug_printf(", 0x%08x ", *ptr );
		ptr++;
		_memctl_debug_printf(", 0x%08x ", *ptr );
		ptr++;
		_memctl_debug_printf(", 0x%08x\n", *ptr);
		ptr++;
	}
	return;
}
#endif

/* 20131118 : RDC's ZQ setting

DDR2:
    ZPROG[13:0]=1000010,1110111 (ODT=72ohm, OCD=45ohm)
 
DDR3:
    ZPROG[13:0]=0100001,0111010 (ODT=60ohm, OCD=40ohm)
 


*/
#if 0 //Apollo
unsigned int auto_cali_value[] = { 	
					0x00002177, /* OCD 45ohm, ODT 72ohm */ //20131118 from RDC setting for DDR2
					0x000010BA, /* OCD 40ohm, ODT 60ohm */ //20131118 from RDC setting for DDR3
					0x00003a25, /* OCD 75ohm, ODT 75ohm */
					0x000012f2, /* OCD 60ohm, ODT 75ohm */
					0x000012a4, /* OCD 60ohm, ODT 150ohm */
					0x00003D71, /* OCD 50ohm, ODT 50ohm */
					0x00003972, /* OCD 60ohm, ODT 60ohm */
					0x000012d1, /* OCD 150ohm, ODT 150ohm */
					0x0000397e, /* OCD 30ohm, ODT 50ohm */
					0x00003f24  /* OCD 60ohm, ODT 50ohm */
				 };
#else //RTL8198C
unsigned int auto_cali_value_DDR2[] = { 	
					//0x000968D1, /* OCD 192ohm, ODT 40ohm */ //RTL8198C ,DDR3 fail
					//0x000928d1, /* OCD 192ohm, ODT 30ohm *///RTL8198C 	 DDR3 :fail	
					//0x001d12a5, /* OCD 80ohm, ODT 21ohm */	//RTL8198C DDR3 :pass
					//0x001EB8F1, /* OCD 73ohm, ODT 19 ohm */ //RTL8198C  DDR3 :pass
					//0x001F9224,  /* OCD 60ohm, ODT 15ohm */	//RTL8198C DDR3 :pass
					//0x0010BBF7, /* OCD 45ohm, ODT 72ohm */ //20131118 from RDC setting for DDR3 :pass
					//0x00085d3a, /* OCD 40ohm, ODT 60ohm */ //20131118 from RDC setting for DDR3 :pass					
					
					//0x001CBF7E, /* OCD 31ohm, ODT 21ohm */ //RTL8198C DDR3 :pass
					//0x00103F7E, /* OCD 31ohm, ODT 360ohm *///RTL8198C DDR3 :pass ,1ns/1.8V	 ,pass			
					//0x00027F7E, /* OCD 31ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V ,pass //20140212 note: for DDR2 0 ohm , OCD driving is too strong
					//0x0002470E, /* OCD 72ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue, K-fail
					0x00024B16, /* OCD 60ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue , K-fail
					//0x00026F5E,/* OCD 48ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue , K-fail
					//0x00025326,/* OCD 45ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue ,Pass
					//0x0002572E,/* OCD 40ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue ,Pass
					//0x00127F7E, /* OCD 31ohm, ODT 16 ohm *///RTL8198C DDR3 :pass	 , bad then ODT 240
					//0x00085B36, /* OCD 34ohm, ODT 72ohm *///RTL8198C DDR3 :pass	
					
					
					
				 };

unsigned int auto_cali_value_DDR3[] = { 	
					//0x000968D1, /* OCD 192ohm, ODT 40ohm */ //RTL8198C ,DDR3 fail
					//0x000928d1, /* OCD 192ohm, ODT 30ohm *///RTL8198C 	 DDR3 :fail	
					//0x001d12a5, /* OCD 80ohm, ODT 21ohm */	//RTL8198C DDR3 :pass
					//0x001EB8F1, /* OCD 73ohm, ODT 19 ohm */ //RTL8198C  DDR3 :pass
					//0x001F9224,  /* OCD 60ohm, ODT 15ohm */	//RTL8198C DDR3 :pass
					//0x0010BBF7, /* OCD 45ohm, ODT 72ohm */ //20131118 from RDC setting for DDR3 :pass
					//0x00085d3a, /* OCD 40ohm, ODT 60ohm */ //20131118 from RDC setting for DDR3 :pass					
					
					//0x001CBF7E, /* OCD 31ohm, ODT 21ohm */ //RTL8198C DDR3 :pass
					//0x00103F7E, /* OCD 31ohm, ODT 360ohm *///RTL8198C DDR3 :pass ,1ns/1.8V	 ,pass			
					//0x00027F7E, /* OCD 31ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V ,pass //20140212 note: for DDR2 0 ohm , OCD driving is too strong
					//0x0002470E, /* OCD 72ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue, K-fail
					0x00044B16, /* OCD 60ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue , K-fail
					//0x00026F5E,/* OCD 48ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue , K-fail
					//0x00025326,/* OCD 45ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue ,Pass
					//0x0002572E,/* OCD 40ohm, ODT 240ohm *///RTL8198C DDR3 :pass ,1ns/2V , 20131219:For Overshoot issue ,Pass
					//0x00127F7E, /* OCD 31ohm, ODT 16 ohm *///RTL8198C DDR3 :pass	 , bad then ODT 240
					//0x00085B36, /* OCD 34ohm, ODT 72ohm *///RTL8198C DDR3 :pass	
					
					
					
				 };


#endif
/*


/*
 * Check DRAM Configuration setting. 
 */
int chk_dram_cfg(unsigned int  dram_freq_mhz)
{
	int     	rcode = 0;	

	//show_DRAM_phy_parameters();
	/* Get current DRAM freq. */
	
	if(dram_freq_mhz==0){
		dram_freq_mhz = board_DRAM_freq_mhz(); 
		_memctl_debug_printf("\n\t Current DRAM frequency: %dMHz\n", dram_freq_mhz);
	}else{
		_memctl_debug_printf("\t Check for DRAM frequency: %dMHz\n", dram_freq_mhz);
	}

	/* Show register informantion. */
	#if 0
	_memctl_debug_printf("\nDefault Register settings:\n");
	_memctl_debug_printf("\tDRAM frequency: %dMHz\n", dram_freq_mhz);
	_memctl_debug_printf("\tMCR (0x%08x):0x%08x\n", MCR, REG(MCR));
	_memctl_debug_printf("\tDCR (0x%08x):0x%08x\n", DCR, REG(DCR));
	_memctl_debug_printf("\tDTR0(0x%08x):0x%08x\n", DTR0,REG(DTR0));
	_memctl_debug_printf("\tDTR1(0x%08x):0x%08x\n", DTR1,REG(DTR1));
	_memctl_debug_printf("\tDTR2(0x%08x):0x%08x\n", DTR2,REG(DTR2));
	_memctl_debug_printf("\tDDCR(0x%08x):0x%08x\n", DDCR, REG(DDCR));
	_memctl_debug_printf("\tDCDR(0x%08x):0x%08x\n\n", DCDR, REG(DCDR));
	#endif
	
	

	/* 
	 * Check DCR and show its size
	 */
	//memctlc_check_DCR();
	

	/* 
	 * Config DTR
	 */
	 memctlc_config_DTR();


	/* 
	 * Config and set DTR
	 */
	memctlc_check_DTR();


	


	/*
	 * Chech DDCR and DCDR
	 */

	/*
	if(memctlc_is_DDR()){
		// if DDR SDRAM : Check DDR SDRAM DQS delay in DDCR.
		memctlc_check_DQS_range();
		// if DDR SDRAM : Check 90 phase delay in DCDR. 
		memctlc_check_90phase_range();
	}
	*/

		
	return rcode;
}




#if 1
unsigned int cali_pattern[] = { 0x00010000, 0x00000000, 0x0000FFFF, 0xFFFF0000,\
				0x00FF00FF, 0xFF00FF00, 0xF0F0F0F0, 0x0F0F0F0F,\
 				0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A,\
				0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5,\
				0x5555AAAA, 0xAAAA5555, 0x5555AAAA, 0xAAAA5555,\
				0xAAAA5555, 0x5555AAAA, 0xAAAA5555, 0x5555AAAA\
			      };

#else
unsigned int cali_pattern[] = { 0x00000000, 0x0000FFFF, 0xFFFF0000, 0x00FF00FF, \
				0xFF00FF00, 0xF0F0F0F0, 0x0F0F0F0F, 0x5A5AA5A5, \
				0xA5A55A5A, 0x5A5AA5A5, 0xA5A55A5A, 0xA5A55A5A, \
				0x5A5AA5A5, 0xA5A55A5A, 0x5A5AA5A5, 0x5555AAAA, \
				0xAAAA5555, 0x5555AAAA, 0xAAAA5555, 0xAAAA5555, \
				0x5555AAAA, 0xAAAA5555, 0x5555AAAA, 0xFFFFFFFF, \
				0x00000010, 0x00000020, 0x00000040, 0x00000080, \
				0x00000001, 0x00000002, 0x00000004, 0x00000008, \
				0x00001000, 0x00002000, 0x00004000, 0x00008000, \
				0x00000100, 0x00000200, 0x00000400, 0x00000800, \
				0x00010000, 0x00020000, 0x00040000, 0x00080000, \
				0x01000000, 0x02000000, 0x04000000, 0x08000000, \
				0x00100000, 0x00200000, 0x00400000, 0x00800000, \
				~0x10000000, ~0x20000000, ~0x40000000, ~0x80000000, \
				~0x00000010, ~0x00000020, ~0x00000040, ~0x00000080, \
				~0x00000001, ~0x00000002, ~0x00000004, ~0x00000008, \
				~0x00001000, ~0x00002000, ~0x00004000, ~0x00008000, \
				~0x00000100, ~0x00000200, ~0x00000400, ~0x00000800, \
				~0x00010000, ~0x00020000, ~0x00040000, ~0x00080000, \
				~0x01000000, ~0x02000000, ~0x04000000, ~0x08000000, \
				~0x00100000, ~0x00200000, ~0x00400000, ~0x00800000, \
				~0x10000000, ~0x20000000, ~0x40000000, ~0x80000000\
			     };
#endif

void _update_phy_param(void)
{
	//_memctl_debug_printf("Invoke _update_phy_param().\n");
	volatile unsigned int *dmcr;
	volatile unsigned int *dcr;
	volatile unsigned int *dacr;
	volatile unsigned int dacr_tmp1;
	volatile unsigned int dacr_tmp2;

	dmcr = (unsigned int *)DMCR;
	dcr = (unsigned int *)DCR;
	dacr = (unsigned int *)DACCR;
	
	/* Write DMCR register to sync the parameters to phy control. */
	//*dmcr = 0x31;
	*dmcr = 0;
	__asm__ __volatile__("": : :"memory");

	/* reset phy buffer pointer */
	dacr_tmp1 = *dacr;
	dacr_tmp1 = dacr_tmp1 & (0xFFFFFFEF);
	dacr_tmp2 = dacr_tmp1 | (0x10);
	*dacr = dacr_tmp1 ;
	__asm__ __volatile__("": : :"memory");
	*dacr = dacr_tmp2 ;

	/* Waiting for the completion of the update procedure. */
	while((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0);
	while((*dcr & ((unsigned int)0x1)) != 0);
	while((*((volatile unsigned int *)(0xB8001038)) & ((unsigned int)0x40000000)) == 0);//Check no DRAM command is not going

	return;
}


/* Function Name: 
 * 	memctlc_dqs_calibration
 * Descripton:
 *	
 * Input:
 *	dram_type:
 *	ac_mode	 :
 *	dqs_mode :
 *	buswidth :
 *	test_addr:
 *	word_size:
 * Output:
 * 	None
 * Return:
 *	MEMCTL_CALI_PASS:
 *	MEMCTL_CALI_FAIL:
 */

#define MEMCTL_AUTO_CALI_NOT_FOUND (32)
#define MEMCTL_DACSPCR_AC_SILEN_PERIOD_EN (1<<31)
#define MEMCTL_DACSPCR_AC_SILEN_TRIG (1<<20)
#define MEMCTL_DACDQ_DQ_AC_EN	     (1<<31)

int memctlc_hw_auto_calibration( unsigned int buswidth, unsigned int test_addr)
{
	volatile unsigned int *dacqdq;
	volatile unsigned int *dacspcr;
	volatile unsigned int *dacspar;
	volatile unsigned int *daccr;
	volatile unsigned int wait_time;
	unsigned int *sil_pat_addr;
	unsigned int i, ret_value, tap_offset;

	unsigned int delay_tap_reg_bk[32];
	unsigned int delay_tap_max[32];
	unsigned int delay_tap_min[32];
	unsigned int target_delay_tap[32];
	unsigned int delay_tap_cur;

	/* 
	 * 0. Backup delay tap setting 
	 */
	ret_value = MEMCTL_CALI_PASS;
	daccr   = (unsigned int *)DACCR;    /* DDR Auto-Calibration Configuration Register */
	dacqdq  = (unsigned int *)DACDQ0RR; /* DDR Auto-Calibration for DQS0 Rising edge on DQ0 Register */
	dacspcr = (unsigned int *)DACSPCR;  /* Silence Pattern Control Register */
	dacspar = (unsigned int *)DACSPAR;  /* Silence Pattern Address Register */


	for(i = 0; i < 32 ; i++){
		delay_tap_reg_bk[i] = *(dacqdq+i);
		target_delay_tap[i] = delay_tap_reg_bk[i];
		delay_tap_max[i] = MEMCTL_AUTO_CALI_NOT_FOUND;
		delay_tap_min[i] = MEMCTL_AUTO_CALI_NOT_FOUND;
	}
	delay_tap_cur = 0;

#if 1
	_memctl_debug_printf("\nmemory controller silence pattern calibration :\n");
	for(i = 0; i < 32 ; i++){
		//_memctl_debug_printf("delay_tap_max[%d] = %d, delay_tap_min[%d] = %d\n", i, (delay_tap_reg_bk[i] & 0x00FF0000) >> 16, i, (delay_tap_reg_bk[i] & 0x000000FF));
	}
#endif
	/* set to the minimun usable value. */
	for(i = 0; i < 32 ; i++){
		target_delay_tap[i] = (target_delay_tap[i] & 0xFFFF00FF) | ((target_delay_tap[i] & 0x000000FF) << 8);
	}

	/* 
	 * 1. Configure the silence pattern and control register 
	 */
	sil_pat_addr = (unsigned int *)test_addr;
	for(i=0; i < sizeof(cali_pattern)/sizeof(unsigned int);i++){
		*(sil_pat_addr+i) = cali_pattern[i];
	}
	*dacspcr = ((sizeof(cali_pattern)/sizeof(unsigned int)) - 1) & (0xFF);
	*dacspar = test_addr;

	/* 
	 * 2. Searching max delay tap window. 
	 */

	/* 2.1 Minimum tap value for the Maximum delay tap. */
	for(delay_tap_cur=0; delay_tap_cur < 32; delay_tap_cur++){
		/* Initialize the maximum delay tap*/
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) & 0xFF00FFFF | (delay_tap_cur << 16);
		}
		_update_phy_param();

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);
	
		/* Enable the HW auto-calibration update mechanism. */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
		}

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);

		/* Check the maximun delay tap value */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				/* find minmun tap of the maximun delay tap. */
				if( ((*(dacqdq+i) & 0x00FF0000) >> 16) > delay_tap_cur ){
					delay_tap_min[i] = delay_tap_cur;
				}
				/* Disable the HW auto-calibration update mechanism.*/
				*(dacqdq+i) = *(dacqdq+i) & (~MEMCTL_DACDQ_DQ_AC_EN);
			}
		}

	}

	/* Check wheather there is failured DQ. */
	for(i = 0; i < 32 ; i++){
		if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
			_memctl_debug_printf("%s, %d: Fail to calibraton for HW auto-calibration. Register address(0x%08x)\n",\
				 __FUNCTION__, __LINE__, dacqdq+i);
			ret_value = MEMCTL_CALI_FAIL;
		}
		if(ret_value == MEMCTL_CALI_FAIL)
			goto go_out;
	}

	/* Initialize the maximum delay tap*/
	for(i = 0; i < 32 ; i++){
		if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
			*(dacqdq+i) = (delay_tap_reg_bk[i] & 0xFF00FFFF) | delay_tap_min[i];
	}
	_update_phy_param();

	/* 2.2 Maximum tap value for the Maximum delay tap. */
	for(tap_offset=0; tap_offset < 32; tap_offset++){
		/* Initialize the maximum delay tap*/
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				if ((tap_offset+delay_tap_min[i]) < 32)
					*(dacqdq+i) = (delay_tap_reg_bk[i] & 0xFF00FFFF) | ((delay_tap_min[i] + tap_offset) << 16);
				else
					delay_tap_max[i] = 31;
			}
		}
		_update_phy_param();

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);
	
		/* Enable the HW auto-calibration update mechanism. */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
		}

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);

		/* Check the maximun delay tap value */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				/* find maximum tap of the maximun delay tap. */
				if( ((*(dacqdq+i) & 0x00FF0000) >> 16) == (tap_offset+delay_tap_min[i]) ){
					delay_tap_max[i] = tap_offset+delay_tap_min[i]-1;
				}
				/* Disable the HW auto-calibration update mechanism.*/
				*(dacqdq+i) = *(dacqdq+i) & (~MEMCTL_DACDQ_DQ_AC_EN);
			}
		}

	}

	/* Check wheather there is failured DQ. */
	for(i = 0; i < 32 ; i++){
		if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
			_memctl_debug_printf("%s, %d: Fail to calibraton for HW auto-calibration. Register address(0x%08x)\n",\
				 __FUNCTION__, __LINE__, dacqdq+i);
			ret_value = MEMCTL_CALI_FAIL;
		}
		if(ret_value == MEMCTL_CALI_FAIL)
			goto go_out;
	}

#if 0
	_memctl_debug_printf("calibration for maximum delay tap:\n");
	for(i=0;i<32;i++){
		_memctl_debug_printf("delay_tap_max[%d] = %d, delay_tap_min[%d] = %d\n", i, delay_tap_max[i], i, delay_tap_min[i]);
	}
#endif

	/* Commit (max+1) to the register back up array */
	for(i=0;i<32;i++){
		delay_tap_reg_bk[i] = (delay_tap_reg_bk[i] & 0xFF00FFFF) | ((delay_tap_max[i] + 1) << 16);
	}

	for(i=0;i<32;i++){
		target_delay_tap[i] = (target_delay_tap[i] & 0xFF00FFFF) | ((delay_tap_max[i]-2) << 16) ;
	}



	/*
	 * 3. Searching min delay tap window. 
	 */
	/* 3.1 Minimum tap value for the Maximum delay tap. */
	for(delay_tap_cur=1; delay_tap_cur < 32; delay_tap_cur++){ /* start from 1 */
		/* Initialize the maximum delay tap*/
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) & 0xFFFFFF00 | (delay_tap_cur);
		}
		_update_phy_param();

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);
	
		/* Enable the HW auto-calibration update mechanism. */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
		}

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);

		/* Check the maximun delay tap value */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				/* find minmun tap of the maximun delay tap. */
				if( (*(dacqdq+i) & 0x000000FF) < delay_tap_cur ){
					delay_tap_min[i] = delay_tap_cur;
				}
				/* Disable the HW auto-calibration update mechanism.*/
				*(dacqdq+i) = *(dacqdq+i) & (~MEMCTL_DACDQ_DQ_AC_EN);
			}
		}
	}


	/* Check wheather there is failured DQ. */
	for(i = 0; i < 32 ; i++){
		if(delay_tap_min[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
			_memctl_debug_printf("%s, %d: Fail to calibraton for HW auto-calibration. Register address(0x%08x)\n",\
				 __FUNCTION__, __LINE__, dacqdq+i);
			ret_value = MEMCTL_CALI_FAIL;
		}
		if(ret_value == MEMCTL_CALI_FAIL)
			goto go_out;
	}


	/* Initialize the minimum delay tap*/
	for(i = 0; i < 32 ; i++){
		if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
			*(dacqdq+i) = (delay_tap_reg_bk[i] & 0xFFFFFF00) | delay_tap_min[i];
	}
	_update_phy_param();

	/* 3.2 Maximum tap value for the Maximum delay tap. */
	for(tap_offset=0; tap_offset < 32; tap_offset++){
		/* Initialize the maximum delay tap*/
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				if ((tap_offset+delay_tap_min[i]) < 32)
					*(dacqdq+i) = (delay_tap_reg_bk[i] & 0xFFFFFF00) | ((delay_tap_min[i] + tap_offset));
				else
					delay_tap_max[i] = 31;
			}
		}
		_update_phy_param();

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);
	
		/* Enable the HW auto-calibration update mechanism. */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND)
				*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
		}

		/* trigger the silence pattern generation */
		*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_TRIG;
		/* waiting for the completion of the silence pattern generation. */
		while(*dacspcr & MEMCTL_DACSPCR_AC_SILEN_TRIG);

		/* Check the maximun delay tap value */
		for(i = 0; i < 32 ; i++){
			if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
				/* find maximum tap of the minimun delay tap. */
				if( (*(dacqdq+i) & 0x000000FF) == (tap_offset+delay_tap_min[i]) ){
					delay_tap_max[i] = tap_offset+delay_tap_min[i]-1;
				}
				/* Disable the HW auto-calibration update mechanism.*/
				*(dacqdq+i) = *(dacqdq+i) & (~MEMCTL_DACDQ_DQ_AC_EN);
			}
		}

	}

	/* Check wheather there is failured DQ. */
	for(i = 0; i < 32 ; i++){
		if(delay_tap_max[i] == MEMCTL_AUTO_CALI_NOT_FOUND){
			_memctl_debug_printf("%s, %d: Fail to calibraton for HW auto-calibration. Register address(0x%08x)\n",\
				 __FUNCTION__, __LINE__, dacqdq+i);
			ret_value = MEMCTL_CALI_FAIL;
		}
		if(ret_value == MEMCTL_CALI_FAIL)
			goto go_out;
	}

	for(i=0;i<32;i++){
		target_delay_tap[i] = (target_delay_tap[i] & 0xFFFFFF00) | ((delay_tap_min[i])) ;
	}

#if 0
	_memctl_debug_printf("calibration for minimum delay tap:\n");
	for(i=0;i<32;i++){
		_memctl_debug_printf("delay_tap_max[%d] = %d, delay_tap_min[%d] = %d\n", i, delay_tap_max[i], i, delay_tap_min[i]);
	}
#endif


	for(i = 0; i < 32 ; i++){
		*(dacqdq+i) = target_delay_tap[i];
	}
	_update_phy_param();

	/* Enable the HW auto-calibration update mechanism. */
	for(i = 0; i < 32 ; i++){
		*(dacqdq+i) = *(dacqdq+i) | MEMCTL_DACDQ_DQ_AC_EN;
	}

	/* Enable periodic silence pattern generation. */
	//*((volatile unsigned int *)0xB8001008) = 0xfffff7c0;
	//_update_phy_param();

	*dacspcr = (*dacspcr & 0xFFF000FF) | 0x00000F00;
	_update_phy_param();

	*dacspcr = *dacspcr | MEMCTL_DACSPCR_AC_SILEN_PERIOD_EN;

	//*dacspcr = *dacspcr & (~MEMCTL_DACSPCR_AC_SILEN_PERIOD_EN);//disable silence pattern

	_update_phy_param();
	//*((volatile unsigned int *)(0xb8001500)) = 0x80000000;
	//__asm__ __volatile__("": : :"memory");
	//*((volatile unsigned int *)(0xb8001500)) = 0x80000010;
go_out:
	return ret_value;
}





/* Function Name:
 *      sram_mapping
 * Descripton:
 *      Configure the SRAM controller.
 * Input:
 *      segNo       : 0~3, indicate the configured register set.
 *	cpu_addr    : The mapped CPU virtual address.
 *	sram_addr   : The sram internal address to be mapped.
 *	sram_size_no:	SRAM_SIZE_256B          
 *			SRAM_SIZE_512B          
 *			SRAM_SIZE_1KB           
 *			SRAM_SIZE_2KB          
 *			SRAM_SIZE_4KB           
 *			SRAM_SIZE_8KB          
 *			SRAM_SIZE_16KB         
 *			SRAM_SIZE_32KB        
 *			SRAM_SIZE_64KB        
 *			SRAM_SIZE_128KB      
 *			SRAM_SIZE_256KB     
 *			SRAM_SIZE_512KB    
 *			SRAM_SIZE_1MB     
 * Output:
 *      None
 * Return:
 *      >= 0: Success
 *	<  0: Fail
 * Note:
 * 	None 
 */
int sram_mapping(unsigned int segNo, unsigned int cpu_addr, \
		 unsigned int sram_addr, unsigned int sram_size_no)
{
	unsigned int size_in_bytes;
	int          ret_code;

	ret_code = 0;

	/* Parameter checking */
	if(segNo >= SRAM_REG_SET_NUM){
		////////_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		//////_memctl_debug_printf("Wrong SRAM segment number(%d), only support (0 ~ %d)\n", segNo, (SRAM_REG_SET_NUM-1));
		ret_code = -1;
	}
	/* Parameter checking */
	if(sram_size_no > SRAM_SIZE_1MB){
		////////_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		//////_memctl_debug_printf("Wrong sram_size_no: %d\n");
		ret_code = -1;
	}


	/* Alignmemt checking */
	size_in_bytes = 128 << sram_size_no;

	if((sram_addr % size_in_bytes) != 0){
		////////_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		//////_memctl_debug_printf("sram_addr(0x%08x) isn't nature aligned with sram size(0x%08x):\n",\
			sram_addr, size_in_bytes);
		ret_code = -1;
	}
	if((cpu_addr % size_in_bytes) != 0){
		////////_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		//////_memctl_debug_printf("cpu_addr(0x%08x) isn't nature aligned with sram size(0x%08x):\n", \
			cpu_addr, size_in_bytes);
		ret_code = -1;
	}


	/* Overlapping checking */


	//if(ret_code >= 0){
		//__sram_mapping(segNo, cpu_addr, sram_addr, sram_size_no);

	//}
	return ret_code;

}

unsigned int _is_Master_CPU(void)
{
	return 1;	
}

unsigned int soc_vir_to_phy_addr(unsigned int vir_addr)
{
	return (vir_addr&0x1FFFFFFF);
}

/* Real Memory controller setting function. */
void __memctl_unmapping(unsigned int segNo, unsigned int cpu_addr, \
                        unsigned int size_no)
{
	unsigned int reg_base_addr;
	volatile unsigned int *p_addr_reg;
	volatile unsigned int *p_size_reg;
	volatile unsigned int *p_base_reg;

	if(_is_Master_CPU()){
		reg_base_addr = C0UMSAR_REG_ADDR;
	}else{
		reg_base_addr = C1UMSAR_REG_ADDR;
	}

	p_addr_reg = (unsigned int *)(reg_base_addr + (MEMCTL_UNMAP_REG_SET_SIZE*segNo));
	p_size_reg = (unsigned int *)(reg_base_addr + (MEMCTL_UNMAP_REG_SET_SIZE*segNo) + 0x4 );

	*(p_size_reg) = size_no;
	*(p_addr_reg) = soc_vir_to_phy_addr(cpu_addr) | MEMCTL_UNMAP_SEG_ENABLE;
	
	return;
}


/* Function Name:
 *      memctl_unmapping
 * Descripton:
 *      Configure the memory controller ummapping.
 * Input:
 *      segNo       : 0~3, indicate the configured register set.
 *	cpu_addr    : The mapped CPU virtual address.
 *	unmap_size_no:	MEMCTL_UNMAP_SIZE_256B          
 *			MEMCTL_UNMAP_SIZE_512B          
 *			MEMCTL_UNMAP_SIZE_1KB           
 *			MEMCTL_UNMAP_SIZE_2KB          
 *			MEMCTL_UNMAP_SIZE_4KB           
 *			MEMCTL_UNMAP_SIZE_8KB          
 *			MEMCTL_UNMAP_SIZE_16KB         
 *			MEMCTL_UNMAP_SIZE_32KB        
 *			MEMCTL_UNMAP_SIZE_64KB        
 *			MEMCTL_UNMAP_SIZE_128KB      
 *			MEMCTL_UNMAP_SIZE_256KB     
 *			MEMCTL_UNMAP_SIZE_512KB    
 *			MEMCTL_UNMAP_SIZE_1MB     
 * Output:
 *      None
 * Return:
 *      >= 0: Success
 *	<  0: Fail
 * Note:
 * 	None 
 */
int memctl_unmapping(unsigned int segNo, unsigned int cpu_addr, \
		   unsigned int unmap_size_no)
{
	unsigned int size_in_bytes;
	int          ret_code;

	ret_code = 0;

	/* Parameter checking */
	if(segNo >= MEMCTL_UNMAP_REG_SET_NUM){
		_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		_memctl_debug_printf("Wrong memctl unmapping segment number(%d), only support (0 ~ %d)\n", segNo, (MEMCTL_UNMAP_REG_SET_NUM-1));
		ret_code = -1;
	}
	/* Parameter checking */
	if(unmap_size_no > MEMCTL_UNMAP_SIZE_1MB){
		_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		_memctl_debug_printf("Wrong unmap_size_no: %d\n");
		ret_code = -1;
	}


	/* Alignmemt checking */
	size_in_bytes = 128 << unmap_size_no;

	if((cpu_addr % size_in_bytes) != 0){
		_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		_memctl_debug_printf("cpu_addr(0x%08x) isn't nature aligned with unmap size(0x%08x):\n", \
			cpu_addr, size_in_bytes);
		ret_code = -1;
	}


	/* Overlapping checking */


	if(ret_code >= 0){
		__memctl_unmapping(segNo, cpu_addr, unmap_size_no);
	}
	return ret_code;

}




/* Function Name:
 *      sram_unmapping
 * Descripton:
 *      Disable memory mapping of the SRAM controller.
 * Inputt:
 *      segNo       : 0~3 SRAM controller register set, indicate the target register set.
 * Output:
 *      None
 * Return:
 *      >= 0: Success
 *	<  0: Fail
 * Note:
 * 	None 
 */



/* Real SRAM controller setting function. */
void __sram_unmapping(unsigned int segNo)
{
	unsigned int reg_base_addr;
	volatile unsigned int *p_addr_reg;
	volatile unsigned int *p_size_reg;
	volatile unsigned int *p_base_reg;

	if(_is_Master_CPU()){
		reg_base_addr = C0SRAMSAR_REG_ADDR;
	}else{
		reg_base_addr = C1SRAMSAR_REG_ADDR;
	}

	p_addr_reg = (unsigned int *)(reg_base_addr + (SRAM_REG_SET_SIZE*segNo));

	*(p_addr_reg) = *(p_addr_reg) & (~SRAM_SEG_ENABLE);
	
	return;
}


int sram_unmapping(unsigned int segNo)
{
	int ret_code;

	ret_code = 0;

	if(segNo >= SRAM_REG_SET_NUM){
		_memctl_debug_printf("Wrong SRAM segment number(%d), only support (0 ~ %d)\n", segNo, (SRAM_REG_SET_NUM-1));
		return -1;
	}
	
	if(ret_code >= 0)
		__sram_unmapping(segNo);


	return ret_code;

}



/* Real Memory controller setting function. */
void __memctl_unmapping_disable(unsigned int segNo)
{
	unsigned int reg_base_addr;
	volatile unsigned int *p_addr_reg;

	if(_is_Master_CPU()){
		reg_base_addr = C0UMSAR_REG_ADDR;
	}else{
		reg_base_addr = C1UMSAR_REG_ADDR;
	}

	p_addr_reg = (unsigned int *)(reg_base_addr + (MEMCTL_UNMAP_REG_SET_SIZE*segNo));

	*(p_addr_reg) = *(p_addr_reg) & (~MEMCTL_UNMAP_SEG_ENABLE);
	
	return;
}


/* Function Name:
 *      memctl_unmapping_disable
 * Descripton:
 *      Configure the memory controller ummapping.
 * Input:
 *      segNo       : 0~3, indicate the configured register set.
 * Output:
 *      None
 * Return:
 *      >= 0: Success
 *	<  0: Fail
 * Note:
 * 	None 
 */
int memctl_unmapping_disable(unsigned int segNo)
{
	unsigned int size_in_bytes;
	int          ret_code;

	ret_code = 0;

	/* Parameter checking */
	if(segNo >= MEMCTL_UNMAP_REG_SET_NUM){
		_memctl_debug_printf("Function:%s , line %d", __FUNCTION__, __LINE__);
		_memctl_debug_printf("Wrong memctl unmapping segment number(%d), only support (0 ~ %d)\n", segNo, (MEMCTL_UNMAP_REG_SET_NUM-1));
		ret_code = -1;
	}

	/* Overlapping checking */


	if(ret_code >= 0){
		__memctl_unmapping_disable(segNo);
	}
	return ret_code;

}

#if 0
	/* 100, 125, 150, 175, 200, 225, 250, 275, 300, 325, 350, 375, 400 */
unsigned int sys_clk_control_dram25[] = {  0x4, 0x8, 0x0, 0x2, 0x4, 0x6, 0x8, 0x8, 0x0, 0x1, 0x2, 0x3, 0x4};
unsigned int sys_clk_control_dram40[] = {  0x5, 0xa, 0x0, 0x3, 0x5, 0x8, 0xa, 0xa, 0x0, 0x1, 0x3, 0x5, 0x5};
unsigned int sys_clk_mckg_phase90[]   = {  0x2, 0x2, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x2, 0x2, 0x2, 0x2, 0x2};
unsigned int sys_clk_mckg_clk_div[]   = {  0x2, 0x2, 0x1, 0x1, 0x1, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0};

int rtl8686_sys_adj_mem_clk(void)
{
	//_memctl_debug_printf("\n001-Invoke rtl8686_sys_adj_mem_clk()\n");

/* Set 8198B DDR2 LDO output 1V86	
	 */
	#define ANA1_REG 0xb8000218
	// *(volatile unsigned int *)ANA1_REG|= (1<<12);
	////////_memctl_debug_printf("\nboard_mem_diag.c:Set DDR2 LDO output 1V86 , ANA1_REG(0xb8000218)=0x%x\n",*(volatile unsigned int *)ANA1_REG);

	// *(volatile unsigned int *)ANA1_REG|= (5<<10);
	////////_memctl_debug_printf("\nboard_mem_diag.c:Set DDR2 LDO output 1V93 , ANA1_REG(0xb8000218)=0x%x\n",*(volatile unsigned int *)ANA1_REG);

	// *(volatile unsigned int *)ANA1_REG|= (6<<10);
	////////_memctl_debug_printf("\nboard_mem_diag.c:Set DDR2 LDO output 1V99 , ANA1_REG(0xb8000218)=0x%x\n",*(volatile unsigned int *)ANA1_REG);

	 *(volatile unsigned int *)ANA1_REG|= (7<<10);
	//////_memctl_debug_printf("\nboard_mem_diag.c:Set DDR2 LDO output 2V07 , ANA1_REG(0xb8000218)=0x%x\n",*(volatile unsigned int *)ANA1_REG);

#if 1//def CONFIG_SOFTWARE_OVERWRITE_FREQ
	unsigned int cpu_clk, cpu_field;
	unsigned int dsp_clk, dsp_field;
	unsigned int mem_clk, mem_field;
	unsigned int sys_pll_ctl_value;
	char *s;

	cpu_clk = 500;
	dsp_clk = 500;
	mem_clk=100;

#if 0//def CONFIG_SYS_BOOTING_DIALOG
	//////_memctl_debug_printf("\n\nPlease input memory clock(MHz):");
	mem_clk = get_input();
	//////_memctl_debug_printf("\nGet input mem_clk:(%d)\n", mem_clk);
#else
	
#endif
	

	if(_is_CKSEL_25MHz()){
		mem_field = sys_clk_control_dram25[ ((mem_clk-100)/25) ];
		dsp_field = (dsp_clk/25)-20;
		cpu_field = (cpu_clk/25)-20;
	}else{
		mem_field = sys_clk_control_dram40[ ((mem_clk-100)/25) ];
		dsp_field = (dsp_clk/20)-25;
		cpu_field = (cpu_clk/20)-25;
	}

	sys_pll_ctl_value = (cpu_field << 16) | (dsp_field << 8) | mem_field;


	_memctl_debug_printf("\n002-Invoke rtl8686_sys_adj_mem_clk()\n");
	sram_mapping(0, 0xa0000000, 0x30000, SRAM_SIZE_32KB);
	memctl_unmapping(0, 0xa0000000, SRAM_SIZE_32KB);
	sys_adj_mem_clk(sys_pll_ctl_value, sys_clk_mckg_phase90[((mem_clk-100)/25)], sys_clk_mckg_clk_div[((mem_clk-100)/25)]);
	sram_unmapping(0);
	memctl_unmapping_disable(0);
	//_memctl_debug_printf("\n003-Invoke rtl8686_sys_adj_mem_clk()\n");
#endif

	return 0;
}
#endif

#define MEMCTL_DRAM_PARAM_OK	(0)
#define MEMCTL_DRAM_PARAM_ERR	(1)
#define MEMCTL_PHY_CTL_ADDR	(0xB8001500)
#define MEMCTL_DCR_ADDR		(0xB8001004)
#define MEMCTL_DCR_WIDTH_SHIFT	(24)
#define MEMCTL_DCR_ROW_SHIFT	(20)
#define MEMCTL_DCR_COL_SHIFT	(16)
#define MEMCTL_DCR_BK_SHIFT	(28)
#define MEMCTL_DCR_CS_SHIFT	(15)

#define MEMCTL_DRAM_MIN_WIDTH	(8)
#define MEMCTL_DRAM_MAX_WIDTH	(16)
#define MEMCTL_DRAM_MIN_ROWS	(2*1024)
#define MEMCTL_DRAM_MAX_ROWS	(64*1024)
#define MEMCTL_DRAM_MIN_COLS	(256)
#define MEMCTL_DRAM_MAX_COLS	(4*1024)
#define MEMCTL_DRAM_MIN_BKS	(2)
#define MEMCTL_DRAM_MAX_BKS	(8)
#define MEMCTL_DRAM_MIN_CS	(1)
#define MEMCTL_DRAM_MAX_CS	(2)

void memctlc_dram_phy_reset(void)
{
	//return;//RTL8198C
	volatile unsigned int *phy_ctl;

	phy_ctl = (volatile unsigned int *)MEMCTL_PHY_CTL_ADDR;
	*phy_ctl = *phy_ctl & ((unsigned int) 0xFFFFFFEF);
	*phy_ctl = *phy_ctl | ((unsigned int) 0x10);
	//_memctl_debug_printf("memctlc_dram_phy_reset: 0x%x(0x%x)\n", phy_ctl, *phy_ctl);

	return;
}

#define DLL_delay_time 0x10000
void memctlc_ddr2_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];
	unsigned int odt_value;


	odt_value=50;//for RTL8198C ,DQ weak driving needed

	//odt_value=150;//for RTL8198C ,Solved DQ slew falling rate issue

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);

	//_memctl_debug_printf("memctlc_ddr2_dll_reset: Set odt_value RTT=%d ohm\n", odt_value);
	if(1){
		switch (odt_value){
			case 0:
				odt_value = DDR2_EMR1_RTT_DIS;
				break;
			case 50:
				odt_value = DDR2_EMR1_RTT_50;
				break;
			case 75:
				odt_value = DDR2_EMR1_RTT_75;
				break;
			case 150:
				odt_value = DDR2_EMR1_RTT_150;
				break;
			default: /* 50 */
				odt_value = DDR2_EMR1_RTT_50;
				break;
		}
	}else{
		odt_value = DDR2_EMR1_RTT_75;
	}
	_DTR_DDR2_MRS_setting(dtr, mr);

	mr[1] = mr[1] | odt_value;

	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR2_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR2_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);
	
	/* 3. Reset DLL */
	*dmcr = mr[0] | DDR2_MR_DLL_RESET_YES ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 4. Waiting 512 DRAM clock for tDLLk */
	delay_time = DLL_delay_time ;
	//delay_time = 0x2000;
	while(delay_time--);

	/* 5. Set EMR2 */
	*dmcr = mr[2];
	while(*dmcr & DMCR_MRS_BUSY);


	/* 6. reset phy fifo */
	//memctlc_dram_phy_reset();

	

	return;
}


void _memctl_delay_clkm_cycles(unsigned int delay_cycles)
{
	volatile unsigned int *mcr, read_tmp;

	mcr = (unsigned int *)MCR;

	while(delay_cycles--){
		read_tmp = *mcr;
	}

	return;
}

//------------------------------------------------------------------

void memctlc_DDR3_ZQ_long_calibration(void)
{
	volatile unsigned int *ddr3_zqccr;
	ddr3_zqccr = (volatile unsigned int *)D3ZQCCR;

	*ddr3_zqccr |= (1<<31);
	_memctl_delay_clkm_cycles(DLL_delay_time);
	while(*ddr3_zqccr & (1<<31));

	return;

}

#if 0


void memctlc_ddr3_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];
	unsigned int rtt_nom_div_value, rtt_wr_div_value, dic_div_value;

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);

	_DTR_DDR3_MRS_setting(dtr, mr);

	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR3_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR3_EMR1_DLL_DIS);
	while(*dmcr & DMCR_MRS_BUSY);
	
	/* 3. Reset DLL */
	*dmcr = mr[0] | DDR3_MR_DLL_RESET_YES ;
	while(*dmcr & DMCR_MRS_BUSY);

	/* 4. Waiting 200 clock cycles */
	delay_time = 0x800;
	while(delay_time--);

	/* 5. Set EMR2 */
	*dmcr = mr[2];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 6. Set EMR3 */
	*dmcr = mr[3];
	while(*dmcr & DMCR_MRS_BUSY);

	/* 7. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
}


#else


void memctlc_ddr3_dll_reset(void)
{
	volatile unsigned int *dmcr, *dtr0;
	volatile unsigned int delay_time;
	unsigned int dtr[3], mr[4];
	unsigned int rtt_nom_div_value, rtt_wr_div_value, dic_div_value;

	dmcr = (volatile unsigned int *)DMCR;
	dtr0 = (volatile unsigned int *)DTR0;
	
	dtr[0]= *dtr0;
	dtr[1]= *(dtr0 + 1);
	dtr[2]= *(dtr0 + 2);
	


	#if 1

	REG32(0xb800101c)=(1<<24);//disable DRAM refresh 
	while(*dmcr & DMCR_MRS_BUSY);
	/* Disable DRAM refresh operation */
	//REG32(0xb800101c)= (REG32(0xb800101c) | DMCR_DIS_DRAM_REF_MASK) & (~DMCR_MR_MODE_EN_MASK);
	//_memctl_debug_printf("\n\n001-Disable DRAM refresh activity \n");
	delay_time = DLL_delay_time; //about 1us
	while(delay_time--);

	//_memctl_debug_printf("\n\n002-Enable DRAM power down mode  ->CKE low \n");
	REG32(0xb8001040)|=(1<<28);//Enable DRAM self refresh mode  ->CKE low
	//_memctl_debug_printf("\n005-MPMR normal mode ->CKE low,0xb8001040=(0x%x)\n",REG32(0xb8001040));
	//delay_time = 0x10000;//about 1000us
	//	while(delay_time--);

	while(*dmcr & DMCR_MRS_BUSY);	
	/*Toggle DDR3 reset pin*/
	//_memctl_debug_printf("\n\n003-Toggle DDR3 reset pin low\n");
	REG32(0xb8001800)&=0xBFFFFFFF; // reset low
	while(*dmcr & DMCR_MRS_BUSY);
	//delay_time = 0x8000;//about 500us
	delay_time = DLL_delay_time;//about 1000us
		while(delay_time--);

	/*Toggle DDR3 reset pin*/
	REG32(0xb8001800)|=(1<<30); // DDR3 reset high
	while(*dmcr & DMCR_MRS_BUSY);
	//_memctl_debug_printf("004-Toggle DDR3 reset pin high\n\n");

////////////////////error///////////////////////////////////////
	delay_time = DLL_delay_time;//about 1000us
	while(delay_time--);

	
	REG32(0xb8001040)&=~(3<<28);//DDR3 MPMR normal mode ->CKE high
	while(*dmcr & DMCR_MRS_BUSY);
	//_memctl_debug_printf("004-Toggle DDR3 reset pi

	//_memctl_debug_printf("\n005-MPMR normal mode ->CKE high,0xb8001040=(0x%x)\n",REG32(0xb8001040));	
	delay_time = DLL_delay_time;//about 1000us
	while(delay_time--);


	
	#else
		/*Toggle DDR3 reset pin*/
	_memctl_debug_printf("\n\nToggle DDR3 reset pin low\n");
	REG32(0xb8001800)&=0xBFFFFFFF; // reset low
	//delay_time = 0x20000;//about 2ms:2000us
	delay_time = 0x10000;//about 1ms:1000us
		while(delay_time--);

	/*Toggle DDR3 reset pin*/
	REG32(0xb8001800)|=(1<<30); // reset high
		_memctl_debug_printf("Toggle DDR3 reset pin high\n\n");
	
		
	#endif

	_DTR_DDR3_MRS_setting(dtr, mr);

	
	
	/* 1. Disable DLL */
	*dmcr = mr[1] | DDR3_EMR1_DLL_DIS;
	while(*dmcr & DMCR_MRS_BUSY);
	


	/* 5. Set EMR2 */
	*dmcr = mr[2];
	//_memctl_debug_printf("\nMR[2]=0x%x\n",   mr[2]  );	
	while(*dmcr & DMCR_MRS_BUSY);

	/* 6. Set EMR3 */
	*dmcr = mr[3];
	//_memctl_debug_printf("\nMR[3]=0x%x\n",   mr[3]  );	
	while(*dmcr & DMCR_MRS_BUSY);

		/* 2. Enable DLL */
	*dmcr = mr[1] & (~DDR3_EMR1_DLL_DIS);

	//_memctl_debug_printf("\nMR[1]=0x%x\n", mr[1] & (~DDR3_EMR1_DLL_DIS));	
	//_memctl_debug_printf("\nEMR1=0x%x\n", REG32(DMCR));	
	while(*dmcr & DMCR_MRS_BUSY);

		/* 3. Reset DLL */
	*dmcr = mr[0] | DDR3_MR_DLL_RESET_YES ;
	//_memctl_debug_printf("\nMR[0]=0x%x\n",  mr[0] | DDR3_MR_DLL_RESET_YES  );	
	while(*dmcr & DMCR_MRS_BUSY);

		/* 4. Waiting 200 clock cycles for tDLLk time */
	//delay_time = 0x800;
	delay_time = DLL_delay_time;
	while(delay_time--);

	memctlc_DDR3_ZQ_long_calibration();


	delay_time = DLL_delay_time;
	while(delay_time--);

	
	/* DDR3 Enable DRAM refresh operation */		
	//REG32(0xb800101c) = REG32(0xb800101c) &  (~DMCR_DIS_DRAM_REF_MASK) ;
	REG32(0xb800101c)&=~(1<<24);//enable DRAM refresh	
	delay_time = DLL_delay_time;
	while(delay_time--);
	while(*dmcr & DMCR_MRS_BUSY);

	/* 7. reset phy fifo */
	memctlc_dram_phy_reset();

	return;
}


#endif

/* Function Name: 
 * 	memctlc_ZQ_calibration
 * Descripton:
 *	Do chip side ZQ configuration
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	.
 */
int memctlc_ZQ_calibration(unsigned int auto_cali_value)
{
#if 1//def CONFIG_ZQ_AUTO_CALI
	volatile unsigned int *dmcr, *daccr, *zq_cali_reg;
	volatile unsigned int *zq_cali_status_reg;
	volatile unsigned int *sys_dram_clk_drv_reg, *socpnr;
	unsigned int polling_limit;



	dmcr = (volatile unsigned int *)DMCR;
	//socpnr = (volatile unsigned int *)SOCPNR;//Apollo's chip ID
	daccr = (volatile unsigned int *)DACCR;
	zq_cali_reg = (volatile unsigned int *)0xB8001094;//DDZQPCR
	zq_cali_status_reg = (volatile unsigned int *)0xB8001098;
	//sys_dram_clk_drv_reg = (volatile unsigned int *)0xB800012c;//RTL8198C N/A

	/* Disable DRAM refresh operation */
	*dmcr = ((*dmcr | DMCR_DIS_DRAM_REF_MASK) & (~DMCR_MR_MODE_EN_MASK));

	/* Enable ZQ calibration model */
	#if 0 //JSW
	if( *socpnr == 0x83890000 )
		*daccr = *daccr | 0x40000000; //single ended
	#else
		//default is differential ended
	#endif

	REG32(0xb8001090)&=~(1<<25);//20140205:Fix tDQSH timing fail issue 

	REG32(0xb80000a0)|=(1<<24);//enable ZQ function for RTL8198C

	REG32(0xb8001090)|=(1<<31);//Force ZQ

	/* Trigger the calibration */
	//*zq_cali_reg = auto_cali_value | 0x40000000;//Apollo
	*zq_cali_reg = auto_cali_value | 0x80000000;//RTL8198C

	/* Polling to ready */
	polling_limit = 0x1000000;
	while(*zq_cali_reg & 0x80000000){
		polling_limit--;
		if(polling_limit == 0){
			_memctl_debug_printf("%s, %d: Error, ZQ auto-calibration ready polling timeout!\n", __FUNCTION__, __LINE__);
			break;
		}
	}
	
	/* Enable DRAM refresh operation */
	*dmcr = *dmcr &  (~DMCR_DIS_DRAM_REF_MASK) ;
	
	/* Setting the fine tuned DRAM Clock driving */
	//*sys_dram_clk_drv_reg = 0xe300db;

	if(*zq_cali_status_reg & 0x20000000)
		return 1; /* Error, calibration fail. */
	else
		return 0; /* Pass, calibration pass.*/

#else
	unsigned int zq_force_value0, zq_force_value1, zq_force_value2;
	volatile unsigned int *zq_pad_ctl_reg;
	//zq_force_value0 = 0x200267; /* 176 demo board*/
	//zq_force_value1 = 0x10001;
	//zq_force_value2 = 0x50000009;
	//zq_force_value0 = 0x002be738; /* 176 demo board*/
	//zq_force_value1 = 0x017d017d;
	//zq_force_value2 = 0x58785809;
	//zq_force_value0 = 0x0022b267; /*OCD 60, ODT 75*/
	zq_force_value0 = 0x0022b49f; /*OCD 60, ODT 50*/
	zq_force_value1 = 0x00570057;
	zq_force_value2 = 0x58282809;

	zq_pad_ctl_reg = (volatile unsigned int *)0xB8000118;

	*zq_pad_ctl_reg     = zq_force_value0;
	*(zq_pad_ctl_reg+1) = zq_force_value0;
	*(zq_pad_ctl_reg+2) = zq_force_value0;
	*(zq_pad_ctl_reg+3) = zq_force_value0;
	*(zq_pad_ctl_reg+6) = zq_force_value0;
	*(zq_pad_ctl_reg+7) = zq_force_value0;
	*(zq_pad_ctl_reg+8) = zq_force_value0;
	*(zq_pad_ctl_reg+9) = zq_force_value0;


	*(zq_pad_ctl_reg+4) = zq_force_value1;
	*(zq_pad_ctl_reg+5) = zq_force_value1;
	*(zq_pad_ctl_reg+10)= zq_force_value1;

	*(zq_pad_ctl_reg+11)= zq_force_value2;


	return 0;
#endif

}








void memctlc_set_dqm_delay(void)
{
	volatile unsigned int *dacdqr;
	volatile unsigned int *dcdqmr;
	volatile unsigned int *dcr;
	unsigned int dqm_delay;
	unsigned int dqm_delay_max;
	unsigned int dqm_delay_min;
	unsigned int bk_dcr;
	volatile unsigned char *src_addr;
	unsigned int src_data_len;
	unsigned char c_data;

	
	dcr    = (volatile unsigned int *)DCR;
	dacdqr = (volatile unsigned int *)DACDQR;
	dcdqmr = (volatile unsigned int *)DCDQMR;
	bk_dcr = *dcr;
	
	/* Configure to 8bit */
	*dcr = *dcr & (~DCR_DBUSWID_MASK); 
	dqm_delay_min = 33;
	dqm_delay_max = 32;
	for(dqm_delay=0;dqm_delay<32;dqm_delay++){
		*dcdqmr = (dqm_delay << DCDQMR_DQM0_PHASE_SHIFT_90_FD_S);
		src_addr = (volatile unsigned char *)0xa0000000;
		src_data_len = 0x100;
		c_data = 0;
		while(src_data_len){
			*src_addr++ = c_data++;
			src_data_len--;
		}
		
		src_addr = (volatile unsigned char *)0xa0000000;
		src_data_len = 0x100;
		c_data = 0;
		while(src_data_len){
			if(*src_addr != c_data){
				break;
			}
			src_addr++;
			c_data++;
			src_data_len--;
		}
		if(src_data_len == 0){
			if(dqm_delay_min==33){
				dqm_delay_min = dqm_delay;
			}
		}else{
			if(dqm_delay_min!=33){
				dqm_delay_max = dqm_delay-1;
				break;
			}
		}
	}
	dqm_delay = (dqm_delay_max + dqm_delay_min)/2;
	*dcdqmr = (dqm_delay << DCDQMR_DQM0_PHASE_SHIFT_90_FD_S) | (dqm_delay << DCDQMR_DQM1_PHASE_SHIFT_90_FD_S);
	/* Restore DCR */
	*dcr = bk_dcr;
	return;

}







#if 0

/*
 * setting clock reverse indication.
 * Can't run in DRAM.
 */
void memctlc_clk_rev_check(void)
{

	unsigned int *clk_rev_ctl_reg;
	unsigned int clk_rev;
	unsigned int cpu_clk;
	unsigned int mem_clk;
	unsigned int lx_clk;

	clk_rev = 0;
	clk_rev_ctl_reg = (unsigned int *)SYSREG_CMUCTLR_REG;	

	cpu_clk = 500;//board_CPU_freq_mhz();
	mem_clk = 100;//board_DRAM_freq_mhz();
	lx_clk = 200;//board_LX_freq_mhz();

	if(cpu_clk < mem_clk)
		clk_rev = (clk_rev | SYSREG_OCP0_SMALLER_MASK | SYSREG_OCP1_SMALLER_MASK);

	if(lx_clk < mem_clk){
		clk_rev = (clk_rev | SYSREG_LX0_SMALLER_MASK | SYSREG_LX1_SMALLER_MASK | SYSREG_LX2_SMALLER_MASK);
		//clk_rev = (clk_rev | LX1_SMALLER_MEM ); /* 20110830: We only can change LX1 freq. */
	}
	*clk_rev_ctl_reg = *clk_rev_ctl_reg | clk_rev;	

	return;
}


int memctlc_init_dram(void)
{
	unsigned int dram_type, i;
	unsigned int target_clkm_delay, clkm_delay;
	unsigned int max_min_window, cur_min_window;
	unsigned int ddrkodl_init_value, target_ddrkodl_value, zq_cali_value, is_zq_fail;
	unsigned int built_addr;
	unsigned int mem_clk_mhz;
	volatile unsigned int delay_loop;
	volatile unsigned int *sysreg_dram_clk_en_reg;
	volatile unsigned int *ddrkodl_reg;


	/* Runtime deterministic DRAM intialization.
	 * Enter DRAM initialization if it is enabled.
	 * Determine whether we run in DRAM.
	 */
	built_addr = (unsigned int) memctlc_init_dram;
	built_addr = built_addr & 0xF0000000;
	if((built_addr == 0xA0000000) || (built_addr == 0x80000000)){
		goto skip_dram_init;
	}

	mem_clk_mhz = 100;//board_DRAM_freq_mhz();

	/* Delay a little bit for waiting for system to enter stable state.*/
	delay_loop = 0x1000;
	while(delay_loop--);
	
	/* Enable DRAM clock */
	sysreg_dram_clk_en_reg = (volatile unsigned int *)SYSREG_DRAM_CLK_EN_REG;
	while(*sysreg_dram_clk_en_reg != SYSREG_DRAM_CLK_EN_MASK ){
		*sysreg_dram_clk_en_reg = SYSREG_DRAM_CLK_EN_MASK;
	}


	/* Delay a little bit for waiting for more stable of the DRAM clock.*/
	delay_loop = 0x1000;
	while(delay_loop--);




	


	/* Configure ZQ */
	is_zq_fail = 1;
	if(0){ //get the value
		is_zq_fail = memctlc_ZQ_calibration(zq_cali_value);
	}
	if(is_zq_fail){//user-defined value fail, try other predefine value
		for(i=0; i< (sizeof(auto_cali_value)/sizeof(unsigned int));i++){
			if(0 == memctlc_ZQ_calibration(auto_cali_value[i])){
				/* We found one .*/
				_memctl_debug_printf("ZQ calibration Pass\n");
				break;
			}
		}
		if(i >= (sizeof(auto_cali_value)/sizeof(unsigned int)) ){
			_memctl_debug_printf("ZQ calibration failed\n");
		}
	}

	ddrkodl_reg = (volatile unsigned int *)SYSREG_DDRCKODL_REG;


	if(mem_clk_mhz <= 250){
		ddrkodl_init_value = 0;
		target_clkm_delay = 0;
		target_ddrkodl_value =  ddrkodl_init_value | (target_clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S);
		*ddrkodl_reg = target_ddrkodl_value;
	}else if(mem_clk_mhz < 350){
		ddrkodl_init_value = 0xf0000;
		target_clkm_delay = 0;
		target_ddrkodl_value =  ddrkodl_init_value | (target_clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S);
		*ddrkodl_reg = target_ddrkodl_value;
	}else{
		ddrkodl_init_value = 0xf0000;
		if(1){ //get the value
			//we got a predefined value
			//*ddrkodl_reg = (*ddrkodl_reg & 0xFF ) | (target_ddrkodl_value & 0xFFFF00);
			*ddrkodl_reg = target_ddrkodl_value;
			/* Try the value. Search again when fail to try. */
		}else{ //no predefined value
#if 1
			max_min_window = 0;
			target_clkm_delay = 0xffffffff;
			for(clkm_delay = 0;clkm_delay < 32; clkm_delay+=2){
	
				//*ddrkodl_reg = (*ddrkodl_reg & 0xFF ) | \
						(target_ddrkodl_value & 0xFFFF00);
				*ddrkodl_reg = (*ddrkodl_reg & 0xFF ) | \
				((ddrkodl_init_value | (clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S)) & 0xFFFF00) ;
		
				/* Reset DRAM DLL */
				if(memctlc_is_DDR()){
					memctlc_ddr1_dll_reset();
				}else if(memctlc_is_DDR2()){
				_memctl_debug_printf("Invoke memctlc_ddr2_dll_reset()\n");
					memctlc_ddr2_dll_reset();
				}else if(memctlc_is_DDR3()){
					memctlc_ddr3_dll_reset();
				}else{
					_memctl_debug_printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
					while(1);
				}

				/* DRAM calibration, memctls_init return 1 when failed. */

				_memctl_debug_printf("ZQ calibration failed\n");
#if 0				
				if(memctls_init()){
					cur_min_window = 0;
					if( target_clkm_delay != 0xffffffff ){
					/* Failed areadly, no more search. */
					break;
				}
				}else{
					cur_min_window = memctlc_extract_min_dqs_window();
#if 0
				/* cur_min_window == 0 dose fail too.*/
				if(target_clkm_delay != 0xffffffff ){
					if(cur_min_window==0)
						break;
				}
#endif
				//memctlc_set_dqm_delay();
				_memctl_debug_printf("clkm_delay(%d):cur_min_window=%d\n", clkm_delay, cur_min_window);
			}

#endif
				
			if(cur_min_window >= max_min_window){
				if(cur_min_window < 22){
					max_min_window = cur_min_window;
					target_clkm_delay = clkm_delay;
				}
			}
		}

		if(target_clkm_delay == 0xffffffff )
			_memctl_debug_printf(" Error: Calibraton failed !\n");

		target_clkm_delay = target_clkm_delay / 2;

		target_ddrkodl_value = ddrkodl_init_value | (target_clkm_delay << SYSREG_DDRCKODL_DDRCLM_TAP_FD_S);
		*ddrkodl_reg = target_ddrkodl_value;
#endif
		}
	}

	/* Reset DRAM DLL */
	if(memctlc_is_DDR()){
		memctlc_ddr1_dll_reset();
	}else if(memctlc_is_DDR2()){
		memctlc_ddr2_dll_reset();
	}else if(memctlc_is_DDR3()){
		memctlc_ddr3_dll_reset();
	}else{
		_memctl_debug_printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
		while(1);
	}

	//_memctl_debug_printf("Invoke memctls_init()\n");
	//memctls_init();
	_memctl_debug_printf("Call memctlc_set_dqm_delay() \n");
	memctlc_set_dqm_delay();

skip_dram_init:
	/* Clock reverse configuration */
	memctlc_clk_rev_check();

	/* Check DRAM Status */
	//board_DRAM_check();

	return 0;
}

#endif

void _set_dmcr(unsigned int dmcr_value)
{
	/* Error cheching here ?*/
	REG(DMCR) = dmcr_value;
	while(REG(DMCR) & 0x80000000);

	return;
}



void memctlc_set_DRAM_buswidth(unsigned int buswidth)
{
	volatile unsigned int *dcr;
	unsigned int t_cas, dmcr_value;
	/* get DCR value */
	dcr = (unsigned int *)DCR;


	switch (buswidth){
		case 8:
			if(memctlc_is_DDR()){
				t_cas = ((REG(DTR0) & (~(DTR0_CAS_MASK))) >> DTR0_CAS_FD_S);
				/*0:2.5, 1:2, 2:3*/
				if(t_cas == 0)
					dmcr_value = 0x00100062;
				else if(t_cas == 1)
					dmcr_value = 0x00100022;
				else if(t_cas == 2)
					dmcr_value = 0x00100032;
				else{
					_memctl_debug_printf("%s, %d: Error t_cas value(%d)\n", __FUNCTION__, __LINE__, t_cas);
					return;
				}
				_set_dmcr(dmcr_value);
			}
			*dcr = (*dcr & (~((unsigned int)DCR_DBUSWID_MASK)));
			break;

		case 16:
			if(memctlc_is_DDR()){
				t_cas = ((REG(DTR0) & (~(DTR0_CAS_MASK))) >> DTR0_CAS_FD_S);
				/*0:2.5, 1:2, 2:3*/
				if(t_cas == 0)
					dmcr_value = 0x00100062;
				else if(t_cas == 1)
					dmcr_value = 0x00100022;
				else if(t_cas == 2)
					dmcr_value = 0x00100032;
				else{
					_memctl_debug_printf("%s, %d: Error t_cas value(%d)\n", __FUNCTION__, __LINE__, t_cas);
					return;
				}
				_set_dmcr(dmcr_value);
			}
			*dcr = (*dcr & (~((unsigned int)DCR_DBUSWID_MASK))) | (unsigned int)(1<<DCR_DBUSWID_FD_S);
			break;

		default:
			_memctl_debug_printf("%s, %d: Error buswidth value(%d)\n", __FUNCTION__, __LINE__, buswidth);
			break;
	}

	return;
}
void memctlc_set_DRAM_colnum(unsigned int col_num)
{
	volatile unsigned int *dcr;

	/* get DCR value */
	dcr = (unsigned int *)DCR;


	switch (col_num){
		case 256:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK)));
			break;

		case 512:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(1<<DCR_COLCNT_FD_S);
			break;

		case 1024:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(2<<DCR_COLCNT_FD_S);
			break;

		case 2048:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(3<<DCR_COLCNT_FD_S);
			break;

		case 4096:
			*dcr = (*dcr & (~((unsigned int)DCR_COLCNT_MASK))) | (unsigned int)(4<<DCR_COLCNT_FD_S);
			break;

		default:
			_memctl_debug_printf("%s, %d: Error column number value(%d)\n", __FUNCTION__, __LINE__, col_num);
			break;
	}

	return;


}
void memctlc_set_DRAM_rownum(unsigned int row_num)
{
	volatile unsigned int *dcr;

	/* get DCR value */
	dcr = (unsigned int *)DCR;


	switch (row_num){
		case 2048:
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK)));
			break;

		case 4096:
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(1<<DCR_ROWCNT_FD_S);
			break;

		case 8192:
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(2<<DCR_ROWCNT_FD_S);
			break;

		case 16384:
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(3<<DCR_ROWCNT_FD_S);
			break;

		case (32*1024):
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(4<<DCR_ROWCNT_FD_S);
			break;

		case (64*1024):
			*dcr = (*dcr & (~((unsigned int)DCR_ROWCNT_MASK))) | (unsigned int)(5<<DCR_ROWCNT_FD_S);
			break;

		default:
			_memctl_debug_printf("%s, %d: Error row number value(%d)\n", __FUNCTION__, __LINE__, row_num);
			break;
	}

	return;


}
void memctlc_set_DRAM_banknum(unsigned int bank_num)
{
	volatile unsigned int *dcr, *dmcr;

	/* get DCR value */
	dcr = (unsigned int *)DCR;
	dmcr = (unsigned int *)DMCR;


	if(memctlc_is_DDR()){
		switch (bank_num){
			case 2:
				*dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK)));
				break;

			case 4:
				*dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(1<<DCR_BANKCNT_FD_S);
				break;

			default:
				_memctl_debug_printf("%s, %d: Error DDR1 bank number value(%d)\n", __FUNCTION__, __LINE__, bank_num);
				break;
		}
	}else{
		switch (bank_num){
			case 4:
				*dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(1<<DCR_BANKCNT_FD_S);
				break;

			case 8:
				*dcr = (*dcr & (~((unsigned int)DCR_BANKCNT_MASK))) | (unsigned int)(2<<DCR_BANKCNT_FD_S);
				break;

			default:
				_memctl_debug_printf("%s, %d: Error DDR2/3 bank number value(%d)\n", __FUNCTION__, __LINE__, bank_num);
				break;
		}
	}

	return;

}
void memctlc_set_DRAM_chipnum(unsigned int chip_num)
{
	volatile unsigned int *dcr;

	/* get DCR value */
	dcr = (unsigned int *)DCR;


	switch (chip_num){
		case 1:
			*dcr = (*dcr & (~((unsigned int)DCR_DCHIPSEL_MASK)));
			break;

		case 2:
			*dcr = (*dcr & (~((unsigned int)DCR_DCHIPSEL_MASK))) | (unsigned int)(1<<DCR_DCHIPSEL_FD_S);
			break;

		default:
			_memctl_debug_printf("%s, %d: Error chip number value(%d)\n", __FUNCTION__, __LINE__, chip_num);
			break;
	}

	return;

}


int memctl_dram_para_set(unsigned int width, unsigned int row, unsigned int column, \
			 unsigned int bank,  unsigned int cs)
{
	//return;//JSW
	unsigned int dcr_value;
	volatile unsigned int *p_dcr;


	_memctl_debug_printf("%s, %d: width(%d), row(%d), column(%d), bank(%d), cs(%d)\n",\
				 __FUNCTION__, __LINE__, width, row, column, bank, cs);
	memctlc_set_DRAM_buswidth(width);
	memctlc_set_DRAM_colnum(column);
	memctlc_set_DRAM_rownum(row);
	memctlc_set_DRAM_banknum(bank);
	memctlc_set_DRAM_chipnum(cs);

	/* Reset PHY FIFO pointer */
	memctlc_dram_phy_reset();

	return MEMCTL_DRAM_PARAM_OK;
}



/* Function Name: 
 * 	memctlc_dram_size_detect
 * Descripton:
 *	Detect the size of current DRAM Chip
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	The number of bytes of current DRAM chips
 * Note:
 *	None
 */
unsigned int memctlc_dram_size_detect(void)
{
	unsigned int i;
	unsigned int width, row, col, bk, cs, max_bk;
	volatile unsigned int *p_dcr;
	volatile unsigned int *test_addr0, *test_addr1;
	volatile unsigned int test_v0, test_v1, tmp;
	p_dcr	    = (volatile unsigned int *)DCR;

	/* Intialize DRAM parameters */
	width = MEMCTL_DRAM_MIN_WIDTH;
	row   = MEMCTL_DRAM_MIN_ROWS ;
	col   = MEMCTL_DRAM_MIN_COLS ;
	cs    = MEMCTL_DRAM_MIN_CS;

	
	/* Configure to the maximun bank number */
	if(memctlc_is_DDR()){
		bk = MEMCTL_DRAM_DDR_MIN_BANK_NUM;
	}else if(memctlc_is_DDR2()){
		bk = MEMCTL_DRAM_DDR2_MIN_BANK_NUM;
	}else{
		bk = MEMCTL_DRAM_DDR3_MIN_BANK_NUM;
	}	

	
	/* 0. Buswidth detection */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0x12345678;
	test_v1 = 0x00000000;
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(MEMCTL_DRAM_MAX_WIDTH, row, col, bk , cs)){
			memctlc_dram_phy_reset();
			*test_addr0 = test_v1;
			*test_addr0 = test_v0;
			//_memctl_debug_printf("test_addr(0x%08x)!= test_v0(0x%08x)\n", *test_addr0, test_v0);
			if( test_v0 !=  *test_addr0 ){
				width = MEMCTL_DRAM_MIN_WIDTH;
			}else{
				width = MEMCTL_DRAM_MAX_WIDTH;
			}
	}


	/* 1. Chip detection */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0xCACA0000;
	test_v1 = 0xACAC0000;
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk , MEMCTL_DRAM_MAX_CS)){
		while(cs < MEMCTL_DRAM_MAX_CS){
			memctlc_dram_phy_reset();
			test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row*bk*cs);
			*test_addr0 = 0x0;
			*test_addr1 = 0x0;
			*test_addr0 = test_v0;
			*test_addr1 = test_v1;
			if( test_v0 ==  *test_addr0 ){
				if( test_v1 ==  *test_addr1 ){
					cs = cs << 1;
					test_v0++;
					test_v1++;
					continue;
				}
			}
			break;
		}
	}

	/* 2. Bank detction */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0x33330000;
	test_v1 = 0xCCCC0000;
	if(memctlc_is_DDR()){
		max_bk = MEMCTL_DRAM_DDR_MAX_BANK_NUM;
	}else if(memctlc_is_DDR2()){
		max_bk = MEMCTL_DRAM_DDR2_MAX_BANK_NUM;
	}else{
		max_bk = MEMCTL_DRAM_DDR3_MAX_BANK_NUM;
	}

	//_memctl_debug_printf("%s,%d: width(%d), row(%d), col(%d), max_bk(%d), cs(%d)\n", __FUNCTION__, __LINE__, width, row, col, max_bk, cs);
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, max_bk, cs)){
		while(bk < max_bk){
			memctlc_dram_phy_reset();
			test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row*bk);
			_memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
			_memctl_debug_printf("BK:(0x%08p)\n", test_addr1);
			*test_addr0 = 0x0;
			*test_addr1 = 0x0;
			*test_addr0 = test_v0;
			*test_addr1 = test_v1;
			tmp = *test_addr0;
			tmp = *test_addr1;
			//_memctl_debug_printf("test_addr0(0x%p):0x%x\n", test_addr0, *test_addr0);
			//_memctl_debug_printf("test_addr1(0x%p):0x%x\n", test_addr1, *test_addr1);
			if( test_v0 ==  *test_addr0 ){
				if( test_v1 ==  *test_addr1 ){
					bk = bk << 1;
					test_v0++;
					test_v1++;
					continue;
				}
			}
			break;
		}
	}

	/* 3. Row detction */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0xCAFE0000;
	test_v1 = 0xDEAD0000;
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, MEMCTL_DRAM_MAX_ROWS, col, bk, cs)){
		while(row < MEMCTL_DRAM_MAX_ROWS){
			memctlc_dram_phy_reset();
			test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col*row);
			_memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
			_memctl_debug_printf("row:(0x%08p)\n", test_addr1);
			*test_addr0 = test_v0;
			*test_addr1 = test_v1;
			tmp = *test_addr0;
			tmp = *test_addr1;
			_memctl_debug_printf("test_addr0(0x%p):0x%x\n", test_addr0, *test_addr0);
			_memctl_debug_printf("test_addr1(0x%p):0x%x\n", test_addr1, *test_addr1);
			_memctl_debug_printf("row = %d\n", row);
			if( test_v0 ==  *test_addr0 ){
				if( test_v1 ==  *test_addr1 ){
					row = row << 1;
					test_v0++;
					test_v1++;
					continue;
				}
			}
			break;
		}
	}

	/* 4. Column detection */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_v0 = 0x5A5A0000;
	test_v1 = 0xA5A50000;
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, MEMCTL_DRAM_MAX_COLS, bk, cs)){
		while(col < MEMCTL_DRAM_MAX_COLS){
			memctlc_dram_phy_reset();
			test_addr1 = (volatile unsigned int *)(0xA0000000 + (width/8)*col);
			*test_addr0 = 0x0;
			*test_addr1 = 0x0;
			_memctl_debug_printf("DCR(0x%08x):", *((volatile unsigned int *)DCR) );
			_memctl_debug_printf("col:(0x%08p)\n", test_addr1);
			*test_addr0 = test_v0;
			*test_addr1 = test_v1;
			tmp = *test_addr0;
			tmp = *test_addr1;
			_memctl_debug_printf("test_addr0(0x%p):0x%x\n", test_addr0, *test_addr0);
			_memctl_debug_printf("test_addr1(0x%p):0x%x\n", test_addr1, *test_addr1);
			if( test_v0 ==  *test_addr0 ){
				if( test_v1 ==  *test_addr1 ){
					col = col << 1;
					test_v0++;
					test_v1++;
					continue;
				}
			}
			break;
		}
	}

	/* 5. Width detction */
	test_addr0 = (volatile unsigned int *)(0xA0000000);
	test_addr1 = (volatile unsigned int *)(0xA0000000);
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk, cs)){
		while(width < MEMCTL_DRAM_MAX_WIDTH){
		memctlc_dram_phy_reset();
		*test_addr0 = 0x3333CCCC;
		__asm__ __volatile__("": : :"memory");
		if( 0x3333CCCC !=  *test_addr0 ){
			width = width >> 1;
			continue;
		}
		__asm__ __volatile__("": : :"memory");
		*test_addr1 = 0x12345678;
		__asm__ __volatile__("": : :"memory");
		if( 0x12345678 !=  *test_addr1 ){
			width = width >> 1;
			continue;
		}
		break;
		}
	}



	memctlc_dram_phy_reset();
	_memctl_debug_printf("\nAuto DRAM size detect: DCR(0x%xp): 0x%08x\n", p_dcr, *p_dcr);
	if(MEMCTL_DRAM_PARAM_OK ==  memctl_dram_para_set(width, row, col, bk, cs)){
		_memctl_debug_printf("Width	  : %d\n", width);
		_memctl_debug_printf("Row  	  : %d\n", row);
		_memctl_debug_printf("Column    : %d\n", col);
		_memctl_debug_printf("Bank 	  : %d\n", bk);
		_memctl_debug_printf("Chip 	  : %d\n", cs);
		_memctl_debug_printf("total size(Byte): 0x%x\n", (unsigned int)((width/8)*row*col*bk*cs));
		_memctl_debug_printf("total size(MB): %d \n",( (unsigned int)((width/8)*row*col*bk*cs))/0x00100000);
		 #if 0//def CONFIG_RTL8198C_FT1_TEST//write DRAM size to dummy register for FT1/2
			REG32(0xb8000f00)=( (unsigned int)((width/8)*row*col*bk*cs))/0x00100000;
		#endif		
		goto test_pass;
	}else{
		_memctl_debug_printf("Error! memctl_dram_para_set failed, function: %s, line:%d", __FUNCTION__, __LINE__);
		goto test_fail;
	}

test_pass:
	return (unsigned int)((width/8)*row*col*bk*cs);
test_fail:
	return 0;
}



#if 0
void DDR_cali_API7(void)//for internal DQS_EN_TAP window duration extension
{
    register int i,j,k;

    register int L0 = 0, R0 = 33, L1 = 0, R1 = 33;
    const register int  DRAM_ADR = 0xA0100000;
    const register int  DRAM_VAL = 0x5A5AA5A5;
    const register int  DACCR_ADR = 0xB8001500;
    register int  DDCR_VAL = (1<<31);  //digital
    //register int  DDCR_VAL = (0<<31);  //analog	


    REG32(DRAM_ADR) = DRAM_VAL;

    //while( (REG32(DACCR_ADR) & 0x40000000) != 0x40000000);
    //while( (REG32(DACCR_ADR) & 0x40000000) != 0x40000000);


         // Calibrate for DQS0
        for (i = 1; i <= 31; i++)
        {
            REG32(DACCR_ADR) = (DDCR_VAL & 0x80000000) | ((i-1) << 16);

            if (L0 == 0)
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) == 0x005A00A5)        {      L0 = i;         }
            }
            else
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) != 0x005A00A5)          {   R0 = i - 1;            break;            }
            }
        }
        DDCR_VAL = (DDCR_VAL & 0xC0000000) | (((L0 + R0) >> 1) << 16);
        REG32(DACCR_ADR ) = DDCR_VAL;

                 // Calibrate for DQS1
        for (i = 1; i <= 31; i++)
        {
            REG32(DACCR_ADR) = (DDCR_VAL & 0xFE000000) | ((i-1) << 8);
            if (L1 == 0)
            {    if ((REG32(DRAM_ADR) & 0xFF00FF00) == 0x5A00A500)        {   L1 = i;     }
            }
            else
            {   if ((REG32(DRAM_ADR) & 0xFF00FF00) != 0x5A00A500)        {   R1 = i - 1;             break;             }
            }
        }

        // ASIC
        DDCR_VAL = (DDCR_VAL & 0xFE000000) | (((L1 + R1) >> 1) << 8);
        REG32(DACCR_ADR) = DDCR_VAL;


        _memctl_debug_printf("DQS0 L0:%d R0:%d C0:%d\n",  L0, R0, (L0 + R0) >> 1);
        _memctl_debug_printf("DQS1 L1:%d R1:%d C1:%d\n",  L1, R1, (L1 + R1) >> 1);


}


#endif

#if 1
void DDR_cali_API7(void)
{
    register int i,j,k;

    register int L0 = 0, R0 = 33, L1 = 0, R1 = 33;
    const register int  DRAM_ADR = 0xA0100000;
    const register int  DRAM_VAL = 0x5A5AA5A5;
    const register int  DDCR_ADR = 0xB8001500;
    register int  DDCR_VAL = (1<<31);  //digital
    //register int  DDCR_VAL = (0<<31);  //analog	


    REG32(DRAM_ADR) = DRAM_VAL;

    //while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);
    //while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);


         // Calibrate for DQS0
        for (i = 0; i <= 31; i++)
        {
        	_memctl_update_phy_param();

            REG32(DDCR_ADR) = (DDCR_VAL & 0x80000000) | ((i-1) << 16);

            if (L0 == 0)
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) == 0x005A00A5)        {      L0 = i;         }
		 

            }
            else
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) != 0x005A00A5)          {   
			
			R0 = i - 1;            break;            }
            }
        }
        DDCR_VAL = (DDCR_VAL & 0x80000000) | (((L0 + R0) >> 1) << 16);
        REG32(DDCR_ADR) = DDCR_VAL;

                 // Calibrate for DQS1
        for (i = 0; i <= 31; i++)
        {
        	_memctl_update_phy_param();

            REG32(DDCR_ADR) = (DDCR_VAL & 0xFFFF0000) | ((i-1) << 8);
            if (L1 == 0)
            {    if ((REG32(DRAM_ADR) & 0xFF00FF00) == 0x5A00A500)        {   L1 = i;     }
		
            }
            else
            {   if ((REG32(DRAM_ADR) & 0xFF00FF00) != 0x5A00A500)        {   
				
			R1 = i - 1;             break;             }
            }
        }

        // ASIC
        //DDCR_VAL = (DDCR_VAL & 0xFE000000) | (((L1 + R1) >> 1) << 8);
      

#if 1//DDR_DBG
        _memctl_debug_printf("\nLDQS0:%d R0:%d C0:%d\n",  L0, R0, (L0 + R0) >> 1);
       _memctl_debug_printf("LDQS1:%d R1:%d C1:%d\n",  L1, R1, (L1 + R1) >> 1);
	   REG32(0xb8001500) &=0xFFE0E0FF;
	   REG32(0xb8001500) |=  ((L0 + R0)/8)<<16;
	   REG32(0xb8001500) |= ((L1 + R1)/8)<<8;

	   // REG32(0xb8001500) |=  (((L0 + R0)/2)-3)<<16;
	  // REG32(0xb8001500) |= (((L1 + R1)/2)-3)<<8;
	_memctl_debug_printf("DACCR(0xb8001500)=0x%x\n",REG32(0xb8001500) );
#endif

}


void DDR_cali_API5(void)
{
    register int i,j,k;

    register int L0 = 0, R0 = 33, L1 = 0, R1 = 33;
    const register int  DRAM_ADR = 0xA0100000;
    const register int  DRAM_VAL = 0x5A5AA5A5;
    const register int  DDCR_ADR = 0xB8001500;
    register int  DDCR_VAL = (1<<31)|(1<<23);  //delay 1/2 memory clock
    //register int  DDCR_VAL = (0<<31);  //	


     REG32(DRAM_ADR) = DRAM_VAL;

    //while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);
    //while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);


         // Calibrate for DQS0
        for (i = 0; i <= 31; i++)
        {
            REG32(DDCR_ADR) = (DDCR_VAL & 0x80000000) | ((i-1) << 16);

            if (L0 == 0)
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) == 0x005A00A5)        {      L0 = i;         }
		 

            }
            else
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) != 0x005A00A5)          {   
			
			R0 = i - 1;            break;            }
            }
        }
        DDCR_VAL = (DDCR_VAL & 0x80000000) | (((L0 + R0) >> 1) << 16);
        REG32(DDCR_ADR) = DDCR_VAL;

                 // Calibrate for DQS1
        for (i = 0; i <= 31; i++)
        {
            REG32(DDCR_ADR) = (DDCR_VAL & 0xFFFF0000) | ((i-1) << 8);
            if (L1 == 0)
            {    if ((REG32(DRAM_ADR) & 0xFF00FF00) == 0x5A00A500)        {   L1 = i;     }
		
            }
            else
            {   if ((REG32(DRAM_ADR) & 0xFF00FF00) != 0x5A00A500)        {   
				
			R1 = i - 1;             break;             }
            }
        }

        // ASIC
        //DDCR_VAL = (DDCR_VAL & 0xFE000000) | (((L1 + R1) >> 1) << 8);
      

#if 1//DDR_DBG
        _memctl_debug_printf("LDQS0:%d R0:%d C0:%d\n",  L0, R0, (L0 + R0) >> 1);
        _memctl_debug_printf("LDQS1:%d R1:%d C1:%d\n",  L1, R1, (L1 + R1) >> 1);
	   REG32(0xb8001500) &=0xFFE0E0FF;
	   REG32(0xb8001500) |=  ((L0 + R0)/4)<<16;
	   REG32(0xb8001500) |= ((L1 + R1)/4)<<8;
	_memctl_debug_printf("DACCR(0xb8001500)=0x%x\n",REG32(0xb8001500) );
#endif

}


#endif
//===========================================================

void RescueMode(void)
{
	
	unsigned char ch;
	unsigned char *p=JUMPADDR;
	unsigned int len=0,csum=0;
	unsigned int i;
	unsigned int addr,val;
	
	void (*jumpF)(void);	



	while(1)
	{
		REG32(0xb800311c)=0xa5000000;
		REG32(0xb8003504)=0x333;	
		printf("\nRescue: ");
		ch=uart_inc();
		
	
		//-------------------------		
			
	
		//-------------------------
		if(ch=='b')
		{	printf("booting\n");
			break;
		}
#if 0
		if(ch=='0')
		{	
			_memctl_debug_printf("\nDDR_Safe_Setting0()..\n");
			DDR_Safe_Setting0(); //Step1: Set CAS/CWL //For FPGA
		}
		
	
#endif

#if 1//def CONFIG_DRAM_TEST
		if(ch=='1')
		{	
			_memctl_debug_printf("Show clock TRx delay map:");
			ShowTxRxDelayMap();//Step2: Set TRx delay

		}

		if(ch=='2')
		{	
			DDR_cali_API5();
		}
#endif
		
		#if 1
		if(ch=='3')
		{	
			DDR_cali_API7();//Step3: for calibrate DQS_TAP of DACCR(0xb8001500)
		}	
		#endif

		
		if(ch=='4')
		{				
			memctlc_dram_size_detect(); //Step4: Size detect , set DMCR to activate DRAM parameter

		}	

		if(ch=='5')
		{		
			#if 1 //Check DCR and config DTR
 				chk_dram_cfg(0); //Step5: auto set DTR (check system register's freq setting)
 			#endif
		}	

		if(ch=='6')
		{		
			_memctl_debug_printf("\nAuto set DQ[15:0] calibration loop for read\n"); 
			memctlc_hw_auto_calibration(16,0xa1000000); //Step6
			
		}	


		if(ch=='7')
		{		
			 _memctl_debug_printf("\nManual set DQ calibration loop\n"); 
			 for(i=0;i<32;i++)
 			REG32( 0xb8001510 +i*4 ) = 0x00040200;
 			REG32( 0xb800101c ) = 0x00110401;
 
 			while( 0x80000000  == (REG32(0xb800101c)&0x80000000) ) {}
 

			 for(i=0;i<32;i++)
			 REG32( 0xb8001510 +i*4 ) = 0x00040200;
 			REG32( 0xb800101c ) = 0x00110404;
 
 			while( 0x80000000 == (REG32(0xb800101c)&0x80000000) ){}
 
			 for(i=0;i<32;i++)
 			REG32( 0xb8001510 +i*4 ) = 0x00040200;
 			REG32( 0xb800101c ) = 0x00100762;
 
 			while( 0x80000000 == (REG32(0xb800101c)&0x80000000) ) {}
 
 			for(i=0;i<32;i++)
 			REG32( 0xb8001510 +i*4 ) = 0x00040200;
 			REG32( 0xb800101c ) = 0x00120000;
 
 			while( 0x80000000  == (REG32(0xb800101c)&0x80000000) ){}
 
 			for(i=0;i<32;i++)
 			REG32( 0xb8001510 +i*4 ) = 0x00040200;
 			REG32( 0xb800101c ) = 0x00100662;
 

 			while( 0x80000000  == (REG32(0xb800101c)&0x80000000) ){ }
 
 			/* Reset phy fifo pointer */
 			REG32( 0xb8001500 ) = 0xc0000000;
 			REG32( 0xb8001500 ) = 0xc0000010;

			
		}	


		if(ch=='c')
		{
		#if 1
		/* 
		 * Check DTR
		 */	
			memctlc_check_DTR();
		#endif
		
		}	

	
		
		if(ch=='d')
		{
			_memctl_debug_printf("dw ");		

			addr=uart_rx8b_val();
			_memctl_debug_printf(" %x",REG32(addr));
		
		}	
		
		if(ch=='e')
		{
			_memctl_debug_printf("ew ");		

			addr=uart_rx8b_val();
			_memctl_debug_printf(" ");
			val  =uart_rx8b_val();
			REG32(addr)=val;
		}

		if(ch=='j')
		{				
			_memctl_debug_printf("Jmp:");
		#if 1			
			addr=uart_rx8b_val();
			jumpF = (void *)(addr);
		#else		
			jumpF = (void *)(JUMPADDR);			
		#endif		
			jumpF();	
			
			_memctl_debug_printf("Hang");
			while(1)  ;		
		}



	

#if 1//def CONFIG_DRAM_TEST	


			if(ch=='l')
		{			
			 RTK_ZQ_K();
			//_memctl_debug_printf("\nDQPR1(0xb8001800)=0xC2100000,CMD/DQS delay\n");	
			REG32(SYS_HW_STRAP)=0x59003E60;//Set memory clock=550MHZ	
			//REG32(0xb8001800)=0xC2100000;
			_memctl_debug_printf("\n2.Set DRAM_CMD_Phase falling of clk\n");	
			REG32(DWPR1)=0xc0000000;

			//Step3 , DCR
			_memctl_debug_printf("\n3.Set DCR(128MB)\n"); 
			REG32(DCR)=0x21220000;//32MB

			chk_dram_cfg(0); 

			REG32(DIDER1)=0x1f1f0000;	


		 #if 0
	     REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (rx<<24) | (rx<<16) );
	#else

	 //RX open loop 
	 //REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (tx<<24) | (tx<<16) );
	//REG32(DIDER1)=0x8f8f0000;	//DDR3 Chariot 250MHZ OK.

	  //REG32_ANDOR(DACCR,    ~( (0x1f<<16) | (0x1f<<8) ),   (rx<<16) | (rx<<8) );

	
	
	   REG32_ANDOR(CLK_MANAGER,    ~( (0x1f<<5) | (0x1f<<0) ),   (0x10<<5)| (0x10 <<0) );


	  
       //Tx command
       //REG32_ANDOR(0xb80000a0,    ~( (0x1f<<5) | (0x1f<<10) ),   (0<<5) |  (0<<10) );


	 //Tx DQS	     
       REG32_ANDOR(DWPR1,    ~( (0x1f<<21) | (0x1f<<16) ),   (0x10<<21) |  (0x10<<16) );


	 //Tx DQM	   
	REG32_ANDOR(0xb8001590,    ~( (0x1f<<24) | (0x1f<<16) ),   (0x10<<24) |  (0x10<<16) );


		
	    #if 1  //Tx DQ
	  
	    for(i=0;i<32;i++)
	    {
 	        //REG32( 0xb8001510 +i*4 ) = 0x00040200; //DDR3 Chariot 250MHZ OK.

		 REG32( 0xb8001510 +i*4 )  = (0x10<<24)|0x00040200;
		//REG32( 0xb8001510 +i*4 )  = 0x00040200;
 		
		
	     } 		
	   #endif
	
	
	#endif


			 
			
		
		}
		

		if(ch=='m')
		{		
			REG32(SYS_HW_STRAP)=0x59003E60;//Set memory clock=550MHZ	
			/* Configure ZQ */
			 RTK_ZQ_K();
			_memctl_debug_printf("\nYifan Test  flow DDR3-Enable EN_HCLK DQS delay\n");	


			#if 1
			//Step1
			//_memctl_debug_printf("20131119:Set MemClk as 390MHZ as default\n");
	
			//REG32(SYS_HW_STRAP)=0x59004260; //Set memory clock=390MHZ	
			
			#endif
			
			#if 1
			//Step1
			_memctl_debug_printf("\n0.Set clk Tx delay=24\n");	
			REG32(CLK_MANAGE)|=0xAC0;
			//_memctl_debug_printf("CLK_MANAGE(0x%x)=0x%x\n",CLK_MANAGE,REG32(CLK_MANAGE));	

			_memctl_debug_printf("\n1.Set DRAM clk phase inverter \n");	
			REG32(DOR)=0x100000ff;
			//_memctl_debug_printf("DOP(0x%x)=0x%x\n",DOR,REG32(DOR));	


			//_memctl_debug_printf("\nDQPR1(0xb8001800)=0xC2100000,CMD/DQS delay\n");	
			//REG32(0xb8001800)=0xC2100000;
			_memctl_debug_printf("\n2.Set DRAM_CMD_Phase falling of clk\n");	
			REG32(DWPR1)=0xC0000000;
			//_memctl_debug_printf("DWPR1(0x%x)=0x%x\n",DWPR1,REG32(DWPR1));	

			#else
			RTK_ZQ_K();
			_memctl_debug_printf("\n2.Set DRAM_CMD_Phase falling of clk\n");	
			//REG32(DWPR1)=0xC0000000;
			REG32(DWPR1)=0x40000000;
			_memctl_debug_printf("DWPR1(0x%x)=0x%x\n",DWPR1,REG32(DWPR1));	

			_memctl_debug_printf("\n1.Set DRAM clk phase inverter \n");	
			REG32(DOR)=0x100000ff;
			_memctl_debug_printf("DOR(0x%x)=0x%x\n",DOR,REG32(DOR));	

			_memctl_debug_printf("\n3.PI phase\n");	
			REG32(0xb80000dc)=0x24900400;
			_memctl_debug_printf("0xb80000dc(0x%x)=0x%x\n",0xb80000dc,REG32(0xb80000dc));	

			_memctl_debug_printf("\n4.DQ phase inv\n");	
			REG32(0xb8001804)|=(5<<29);
			_memctl_debug_printf("0xb8001804(0x%x)=0x%x\n",0xb8001804,REG32(0xb8001804));		

			

			_memctl_debug_printf("\n5.PLL div2\n");	
			REG32(0xb80000d8)|=(1<<21);
			_memctl_debug_printf("0xb80000d8(0x%x)=0x%x\n",0xb80000d8,REG32(0xb80000d8));

			_memctl_debug_printf("\n6.PLL multi 2\n");	
			REG32(0xb80000d8)&=~(1<<21);
			_memctl_debug_printf("0xb80000d8(0x%x)=0x%x\n",0xb80000d8,REG32(0xb80000d8));


			
			#endif
			//Step2
			_memctl_debug_printf("\n3.Set Safe DTR\n"); 
			//memctlc_config_DTR();
			/*
				 ddr2_16bit_size[] = { 
				 0x11210000 * 32MB *, 
				 0x11220000 * 64MB *, 
				 0x21220000 * 128MB *, 
				 0x21320000/ 256MB /, 
				 0x21420000/ 512MB /};
			*/
			#if 0
			//DDR3 290MHZ
			REG32(DTR0)=0x54433630; //set DTR0,CL=6 , CWL= 5 //For RTL8198C DDR3 ASIC setting
			//REG32(DTR1)=0x0404030e; //set DTR1
			//REG32(DTR2)=0x0600d000; //set DTR2
			//REG32(0xb8001004)=0x21220000; //set DDR2 128MB 		
			
			//Step3 , DCR
			_memctl_debug_printf("\n3.Set DDR3 DCR(128MB)\n"); 
			REG32(DCR)=0x21220000;//128MB
			#else
			//REG32(DTR0)=0x37277240; //set DTR0,CL=4 , CWL= 3 //For RTL8198C ASIC setting
			//REG32(DTR1)=0x10101010; //set DTR1
			//REG32(DTR2)=0x0800B000; //set DTR2
			//REG32(0xb8001004)=0x21220000; //set DDR2 128MB 		
			
			//Step3 , DCR
			_memctl_debug_printf("\n3.Set DCR(128MB)\n"); 
			REG32(DCR)=0x21220000;//32MB

			chk_dram_cfg(0); 
			#endif

			
				_update_phy_param();
			if(board_DRAM_freq_mhz()>=290)
			{
				//_memctl_debug_printf("\n6.Manual set DIDER1 ,Enable EN_HCLK DQS delay\n"); 
				//REG32(DIDER1)=0x80800000;	
				//REG32(DIDER1)=0x04040000;
				//REG32(DIDER2)=0x80000000;	
				REG32(DIDER1)=0x1f1f0000;	
				_memctl_debug_printf("DIDER1(0x%x)=0x%x\n",DIDER1,REG32(DIDER1));	
			}
			else
			{
					//Step4 , DIDER1
				_memctl_debug_printf("\n4.Manual set DIDER1\n"); 
				REG32(DIDER1)=0x04040000;
			}


				//Step5 , DACQ0~15
			_memctl_debug_printf("\n5.Manual set DACQ0~15\n"); 
			int i;
			 for(i=0;i<32;i++)
 			REG32( 0xb8001510 +i*4 ) = 0x00040200;
 			//REG32( 0xb8001510 +i*4 ) = 0x10040200;

			_update_phy_param();	
 			/* Reset phy fifo pointer */
 			REG32( 0xb8001500 ) = 0x80000000;
 			REG32( 0xb8001500 ) = 0x80000010;


			//_update_phy_param();	
			//memctlc_dram_size_detect(); //Step4: Size detect , set DMCR to activate DRAM parameter
			_update_phy_param();	
			//chk_dram_cfg(0); //Step5: auto set DTR (check system register's freq setting)
			//_update_phy_param();	

			
			#if 0
			//Step6 , Set MCR bit14 for init EMR , only useful for DDR2
			REG32( MCR ) |= (1<<14);	 
			#else
			/* Reset DRAM DLL */
			//	if(memctlc_is_DDR()){
					//_memctl_debug_printf("Invoke memctlc_ddr1_dll_reset()\n");
					//memctlc_ddr1_dll_reset();
				//}else if(memctlc_is_DDR2()){
				if(memctlc_is_DDR2()){
				_memctl_debug_printf("Invoke memctlc_ddr2_dll_reset()\n");
					memctlc_ddr2_dll_reset();
				}else if(memctlc_is_DDR3()){
					_memctl_debug_printf("Invoke memctlc_ddr3_dll_reset()\n");
					memctlc_ddr3_dll_reset();
				}else{
					_memctl_debug_printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
					//while(1);
				}
			#endif			
			
			
			_update_phy_param();
			if(board_DRAM_freq_mhz()>=290)
			{
				//_memctl_debug_printf("\n6.Manual set DIDER1 ,Enable EN_HCLK DQS delay\n"); 
				//REG32(DIDER1)=0x80800000;	
				//REG32(DIDER1)=0x04040000;
				//REG32(DIDER2)=0x80000000;	
				REG32(DIDER1)=0x1f1f0000;	
				_memctl_debug_printf("DIDER1(0x%x)=0x%x\n",DIDER1,REG32(DIDER1));	
			}
			else
			{
					//Step4 , DIDER1
				_memctl_debug_printf("\n4.Manual set DIDER1\n"); 
				REG32(DIDER1)=0x04040000;
			}
			//memctlc_hw_auto_calibration(16,0xa0800000); //Step6
			_update_phy_param();	
			
		}

		

		
		if(ch=='n')
		{				
			_memctl_debug_printf("\nYifan Test  flow DDR3-Enable EN_HCLK DQS delay\n");	
			/* Configure ZQ */
			//RTK_ZQ_K();
			
			#if 0
			//Step1
			_memctl_debug_printf("\n0.Set clk Tx delay=24\n");	
			REG32(CLK_MANAGE)|=0xAC0;
			//_memctl_debug_printf("CLK_MANAGE(0x%x)=0x%x\n",CLK_MANAGE,REG32(CLK_MANAGE));	

			_memctl_debug_printf("\n1.Set DRAM clk phase inverter \n");	
			REG32(DOR)=0x100000ff;
			//_memctl_debug_printf("DOP(0x%x)=0x%x\n",DOR,REG32(DOR));	


			//_memctl_debug_printf("\nDQPR1(0xb8001800)=0xC2100000,CMD/DQS delay\n");	
			//REG32(0xb8001800)=0xC2100000;
			_memctl_debug_printf("\n2.Set DRAM_CMD_Phase falling of clk\n");	
			REG32(DWPR1)=0x40000000;

			_memctl_debug_printf("\n3.PI phase\n");	
			REG32(0xb80000dc)=0x24900400;
			_memctl_debug_printf("0xb80000dc(0x%x)=0x%x\n",0xb80000dc,REG32(0xb80000dc));	

			_memctl_debug_printf("\n4.DQ phase inv\n");	
			REG32(0xb8001804)|=(5<<29);
			_memctl_debug_printf("0xb8001804(0x%x)=0x%x\n",0xb8001804,REG32(0xb8001804));		

			

			_memctl_debug_printf("\n5.PLL div2\n");	
			REG32(0xb80000d8)|=(1<<21);
			_memctl_debug_printf("0xb80000d8(0x%x)=0x%x\n",0xb80000d8,REG32(0xb80000d8));

			_memctl_debug_printf("\n6.PLL multi 2\n");	
			REG32(0xb80000d8)&=~(1<<21);
			_memctl_debug_printf("0xb80000d8(0x%x)=0x%x\n",0xb80000d8,REG32(0xb80000d8));
			//_memctl_debug_printf("DWPR1(0x%x)=0x%x\n",DWPR1,REG32(DWPR1));	
			#else
			//RTK_ZQ_K();
			_memctl_debug_printf("\n2.Set DRAM_CMD_Phase falling of clk\n");	
			//REG32(DWPR1)=0xC0000000;
			REG32(DWPR1)=0x40000000;
			_memctl_debug_printf("DWPR1(0x%x)=0x%x\n",DWPR1,REG32(DWPR1));	

			_memctl_debug_printf("\n1.Set DRAM clk phase inverter \n");	
			REG32(DOR)=0x100000ff;
			_memctl_debug_printf("DOR(0x%x)=0x%x\n",DOR,REG32(DOR));	

			_memctl_debug_printf("\n3.PI phase\n");	
			REG32(0xb80000dc)=0x24900400;
			_memctl_debug_printf("0xb80000dc(0x%x)=0x%x\n",0xb80000dc,REG32(0xb80000dc));	

			_memctl_debug_printf("\n4.DQ phase inv\n");	
			REG32(0xb8001804)|=(5<<29);
			_memctl_debug_printf("0xb8001804(0x%x)=0x%x\n",0xb8001804,REG32(0xb8001804));		

			

			_memctl_debug_printf("\n5.PLL div2\n");	
			REG32(0xb80000d8)|=(1<<21);
			_memctl_debug_printf("0xb80000d8(0x%x)=0x%x\n",0xb80000d8,REG32(0xb80000d8));

			_memctl_debug_printf("\n6.PLL multi 2\n");	
			REG32(0xb80000d8)&=~(1<<21);
			_memctl_debug_printf("0xb80000d8(0x%x)=0x%x\n",0xb80000d8,REG32(0xb80000d8));


			
			#endif
			//Step2
			_memctl_debug_printf("\n3.Set Safe DTR\n"); 
			//memctlc_config_DTR();
			/*
				 ddr2_16bit_size[] = { 
				 0x11210000 * 32MB *, 
				 0x11220000 * 64MB *, 
				 0x21220000 * 128MB *, 
				 0x21320000/ 256MB /, 
				 0x21420000/ 512MB /};
			*/
			#if 0
			//DDR3 290MHZ
			REG32(DTR0)=0x54433630; //set DTR0,CL=6 , CWL= 5 //For RTL8198C DDR3 ASIC setting
			//REG32(DTR1)=0x0404030e; //set DTR1
			//REG32(DTR2)=0x0600d000; //set DTR2
			//REG32(0xb8001004)=0x21220000; //set DDR2 128MB 		
			
			//Step3 , DCR
			_memctl_debug_printf("\n3.Set DDR3 DCR(128MB)\n"); 
			REG32(DCR)=0x21220000;//128MB
			#else
			
			
			//Step3 , DCR
			_memctl_debug_printf("\n3.Set DCR(128MB)\n"); 
			REG32(DCR)=0x21220000;//

			chk_dram_cfg(0); 
			#endif

			
				_update_phy_param();
			if(board_DRAM_freq_mhz()>=250)
			{
				//_memctl_debug_printf("\n6.Manual set DIDER1 ,Enable EN_HCLK DQS delay\n"); 
				//REG32(DIDER1)=0x80800000;	
				//REG32(DIDER1)=0x04040000;
				//REG32(DIDER2)=0x80000000;	
				REG32(DIDER1)=0x1f1f0000;	
				_memctl_debug_printf("DIDER1(0x%x)=0x%x\n",DIDER1,REG32(DIDER1));	
			}
			else
			{
					//Step4 , DIDER1
				_memctl_debug_printf("\n4.Manual set DIDER1\n"); 
				REG32(DIDER1)=0x04040000;
			}


				//Step5 , DACQ0~15
			_memctl_debug_printf("\n5.Manual set DACQ0~15\n"); 
			int i;
			 for(i=0;i<32;i++)
 			REG32( 0xb8001510 +i*4 ) = 0x00040200;
 			//REG32( 0xb8001510 +i*4 ) = 0x10040200;

			_update_phy_param();	
 			/* Reset phy fifo pointer */
 			REG32( 0xb8001500 ) = 0x80000000;
 			REG32( 0xb8001500 ) = 0x80000010;

			
			_update_phy_param();	
			
			

			
			#if 0
			//Step6 , Set MCR bit14 for init EMR , only useful for DDR2
			REG32( MCR ) |= (1<<14);	 
			#else
			/* Reset DRAM DLL */
			//	if(memctlc_is_DDR()){
					//_memctl_debug_printf("Invoke memctlc_ddr1_dll_reset()\n");
					//memctlc_ddr1_dll_reset();
				//}else if(memctlc_is_DDR2()){
				if(memctlc_is_DDR2()){
				_memctl_debug_printf("Invoke memctlc_ddr2_dll_reset()\n");
					memctlc_ddr2_dll_reset();
				}else if(memctlc_is_DDR3()){
					_memctl_debug_printf("Invoke memctlc_ddr3_dll_reset()\n");
					memctlc_ddr3_dll_reset();
				}else{
					_memctl_debug_printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
					//while(1);
				}
			#endif			
			
			
			_update_phy_param();
			if(board_DRAM_freq_mhz()>=290)
			{
				//_memctl_debug_printf("\n6.Manual set DIDER1 ,Enable EN_HCLK DQS delay\n"); 
				//REG32(DIDER1)=0x80800000;	
				//REG32(DIDER1)=0x04040000;
				//REG32(DIDER2)=0x80000000;	
				REG32(DIDER1)=0x1f1f0000;	
				_memctl_debug_printf("DIDER1(0x%x)=0x%x\n",DIDER1,REG32(DIDER1));	
			}
			else
			{
					//Step4 , DIDER1
				_memctl_debug_printf("\n4.Manual set DIDER1\n"); 
				REG32(DIDER1)=0x04040000;
			}
			//memctlc_hw_auto_calibration(16,0xa0800000); //Step6
			_update_phy_param();	
			
		}

		

		if(ch=='o')
		{				
			_memctl_debug_printf("\nDram_test_onflash():\n");
			Dram_test_onflash();			
		}

	

		if(ch=='q')
		{				
		/* Reset DRAM DLL */
			//	if(memctlc_is_DDR()){
					//_memctl_debug_printf("Invoke memctlc_ddr1_dll_reset()\n");
					//memctlc_ddr1_dll_reset();
				//}else if(memctlc_is_DDR2()){
				if(memctlc_is_DDR2()){
				_memctl_debug_printf("Invoke memctlc_ddr2_dll_reset()\n");
					memctlc_ddr2_dll_reset();
				}else if(memctlc_is_DDR3()){
					_memctl_debug_printf("Invoke memctlc_ddr3_dll_reset()\n");
					memctlc_ddr3_dll_reset();
				}else{
					_memctl_debug_printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
					//while(1);
				}
			
		}
			

#endif		
		if(ch=='s')
		{
			
			_memctl_debug_printf("0xa0400000=0x%x \n",REG32(0xa0400000) );
			
			_memctl_debug_printf("0xa0500000=0x%x \n",REG32(0xa0500000) );	

			_memctl_debug_printf("0xa0600000=0x%x \n",REG32(0xa0600000) );	
		}	
		

		if(ch=='t')
		{
			_memctl_debug_printf("Test:Write 0xa0400000=0xa5a55a5a\n");
			REG32(0xa0400000)=0xa5a55a5a;
			//_memctl_debug_printf("0xa0400000=0x%x \n",REG32(0xa0400000) );	

			_memctl_debug_printf("Test:Write 0xa0400000=0x12345678\n");
			REG32(0xa0400000)=0x12345678;
			//_memctl_debug_printf("0xa0400000=0x%x \n",REG32(0xa0400000) );	

			_memctl_debug_printf("Test:Write 0xa0500000=0xa5005a00\n");
			REG32(0xa0500000)=0xa5005a00;
			//_memctl_debug_printf("0xa0500000=0x%x \n",REG32(0xa0500000) );	

			_memctl_debug_printf("Test:Write 0xa0600000=0x98765432\n");
			REG32(0xa0600000)=0x98765432;
			//_memctl_debug_printf("0xa0600000=0x%x \n",REG32(0xa0600000) );	
		}	
		
		if(ch=='u')
		{	_memctl_debug_printf("up ");	
			p=JUMPADDR;
			
			
			_memctl_debug_printf("len=");		
			len=uart_rx8b_val();

			_memctl_debug_printf(" load...");		

			while(len--)
			{
				*p=uart_inc();						
				p++;
			}

		}	

		if(ch=='v')
		{	
			_memctl_debug_printf("\nZQ test\n");	
			RTK_ZQ_K();
			
		}	


		

		if(ch=='x')
		{		
				len=xmodem_receive();
				_memctl_debug_printf("len=%d\n",len);
				ch='j';
		}	

		

		if(ch=='z')
		{	
			//250MHZ
			printf("\nDDR2/3 init flow 250MHZ\n"); 
			REG32(SYS_HW_STRAP)=0x59000260; //Set memory clock=250MHZ	

			_memctl_update_phy_param();

			/*Realtek RTL8198C ZQ pad calibration*/
			RTK_ZQ_K();

			/*For DDR3 must , for DDR2 safe*/
			memctlc_DBFM_enalbe();

			memctlc_8198C_PI_init();

			_memctl_update_phy_param();


				memctlc_config_DTR();

				if(memctlc_is_DDR2()){
					REG32(DCR)=0x11210000;//32MB				
			
					memctlc_ddr2_dll_reset();

					
				}else if(memctlc_is_DDR3()){		
					REG32(DCR)=0x21220000;//128MB			
					
					memctlc_ddr3_dll_reset();					
					
				}else{
					_memctl_debug_printf("%s, %d: Error, Unknown DRAM type! \n", __FUNCTION__, __LINE__);	
					//while(1);
				}
				_memctl_update_phy_param();

				memctlc_check_DTR();	

				_memctl_update_phy_param();
			
			 	ShowRxDelayMap(); 				
			
				 DDR_cali_API7();

				 _memctl_debug_printf("DRAM size detect\n");
				memctlc_dram_size_detect(); //Step4: Size detect , set DMCR to activate DRAM parameter

		}
	
		
		

		if(ch=='?')			
		{	
#if 1		
			_memctl_debug_printf("cmd:ed ulxj bsc \n");
#else			
			_memctl_debug_printf("\ne:ew\n");
			_memctl_debug_printf("d:dw\n");
			_memctl_debug_printf("u:uoload\n");
			//_memctl_debug_printf("j:jmp\n");		
			_memctl_debug_printf("b:booting\n");
			_memctl_debug_printf("c:\n");			
#endif
		}
	}
}




//==============================================================
	
	
	const unsigned char *boot_type_tab[]={ "SPI0", "SPI1", "NFBI", "NAND", "ROM01", "ROM02", "ROM03", "EnAutoOLTtest" };
	const unsigned char *dram_type_tab[]={ "DDR2", "DDR3" };
	//const unsigned int m2x_clksel_table[]={ 250,	290,	330, 370, 410,  450,  490,  530,  390,  595, 625, 655, 685,  720,  755,  785};//[bit14:11]
	const unsigned int m2x_clksel_table[]={ 250,	270,290,310,330,350,370,565, 410,430,450,470,490,510 ,530,550 , 390, 580, 595,610, 625, 640,655,670, \
	685,700, 720, 740 ,755, 770 ,785,800};//[bit14:10]
	
	
	

//------------------------------------------------------------------
		


/* Function Name: 
 * 	board_DRAM_freq_mhz
 * Descripton:
 *	Get the current DRAM frequency in MHz.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *  	DRAM Frequncy in MHz.
 */
unsigned int board_DRAM_freq_mhz(void)
{
#if 0//def CONFIG_SOFTWARE_OVERWRITE_FREQ
	volatile unsigned int *sysclk_contr_reg;
	volatile unsigned int *sysclk_mckg_div_reg;
	unsigned int mem_clk, clk_divison;

	sysclk_contr_reg = (volatile unsigned int *)SYSREG_SYSCLK_CONTROL_REG;
	sysclk_mckg_div_reg = (volatile unsigned int *)SYSREG_MCKG_FREQ_DIV_REG;
	clk_divison = *sysclk_mckg_div_reg;
	mem_clk = (*sysclk_contr_reg & SYSREG_SYSCLK_CONTROL_SDPLL_MASK) \
		>> SYSREG_SYSCLK_CONTROL_SDPLL_FD_S;

	if( _is_CKSEL_25MHz() ){
		mem_clk = (mem_clk + 12) * 25;
	}else{
		mem_clk = (mem_clk + 15) * 20;
	}

	while(clk_divison){
		mem_clk = mem_clk / 2;
		clk_divison--;
	}

	return mem_clk;
#else		

	
	unsigned int v=REG32(SYS_HW_STRAP);
	
	_memctl_debug_printf("Strap=%x\n",v);

	//-----------	

	//unsigned char m2x_freq_sel=GET_BITVAL(v, 11, RANG4);  //bit [14:10], but bit 10 is not strap pin
	unsigned char m2x_freq_sel=GET_BITVAL(v, 10, RANG5);  //bit [14:10], but bit 10 is not strap pin

	//-----------
	
	
		
	unsigned int m2xclk=m2x_clksel_table[m2x_freq_sel];
	unsigned int m1xclk=m2x_clksel_table[m2x_freq_sel];

	
	_memctl_debug_printf("\nboard_DRAM_freq_mhz(): Strap DRAM freq=%d\n",m1xclk);

	#if 0
	if(REG32(0xb80000d8)=0x562B5DC0) //	clk/2
	{
		return m1xclk/2;//clock div 2
	}
	#else
		return m1xclk;
	#endif
	
#endif
}

#if 1//Dramtest on flash

#if 1
unsigned int rand2(void)
{
    static unsigned int x = 123456789;
    static unsigned int y = 362436;
    static unsigned int z = 521288629;
    static unsigned int c = 7654321;

    unsigned long long t, a= 698769069;

    x = 69069 * x + 12345;
    y ^= (y << 13); y ^= (y >> 17); y ^= (y << 5);
    t = a * z + c; c = (t >> 32); z = t;

    return x + y + z;
}
#endif
#define MAX_SAMPLE  0x8000
//#define START_ADDR  0x100000               //1MB
#define START_ADDR  0x00700000              //7MB, 0~7MB can't be tested
//#define END_ADDR      0x800000		//8MB
//#define END_ADDR      0x1000000         //16MB
//#define END_ADDR      0x2000000        //32MB
//#define END_ADDR      0x4000000       //64MB
//#define END_ADDR      0x8000000         //128MB      
#define MPMR_REG 0xB8001040
#define BURST_COUNTS  256
void Dram_test_onflash()
{
    unsigned int i, j,k,k2=0;
    unsigned int cache_type=0;
    unsigned int access_type=0;
    unsigned int Data_pattern=0;
     unsigned int random_test=1;
    unsigned int addr;
    unsigned int burst=0;
    unsigned long int wdata;
    unsigned int samples,test_range;

    unsigned int enable_delay,delay_time,PM_MODE;//JSW:For DRAM Power Management
       
    unsigned int wdata_array[BURST_COUNTS];         //JSW:It must equal to Burst size
    
  

    /*JSW: Auto set DRAM test range*/
   
    unsigned int END_ADDR,DCR_VALUE;

#if 0 //For RTL8881A only
    DCR_VALUE=REG32(DCR_REG)&0x01F00000;//Just check bit[24:20]
    
	
    if (DCR_VALUE==0x00800000)
	END_ADDR=0x800000; //Test 8MB
    else if (DCR_VALUE==0x00900000) //16MBx1
       END_ADDR=0x1000000; //Test 16MB
    else if (DCR_VALUE==0x01100000) // 32MBx1
       END_ADDR=0x2000000; //Test 32MB
    else if (DCR_VALUE==0x01200000) //64MBx1
       END_ADDR=0x4000000;

   
	
    if((DCR_VALUE==0x01200000) && (REG32(EDTCR_REG)&(1<<30)))  // For DDR2 128MBx1
       END_ADDR=0x8000000;

	
	
   // if((DCR_VALUE==0x03A20000))  // For DDR1 128MBx1   (DCR=0x15A20000) 
   if((DCR_VALUE==  0x01A00000)) 
       END_ADDR=0x08000000;

    if(REG32(DCR_REG)&0x02000000) //DRAM chip select==1
	END_ADDR=END_ADDR*2;	
	
#else
	END_ADDR=0x01000000; //Test 16MB  
#endif

    unsigned int keep_W_R_mode;
   
    	 keep_W_R_mode= 0;
        enable_delay= 0;
	 PM_MODE= 0;
	 cache_type=1;
	 access_type=2;
	 Data_pattern=1;//sequential
	 random_test=1;
 		
		_memctl_debug_printf("ex:dramtest <1-R/W> <2-enable_random_delay> <3-PowerManagementMode><4-cache_type><5-bit_type><6-Data_Pattern><7-Random_mode>\r\n");
	       _memctl_debug_printf("<1-R/W>:<0>=R+W, <1>=R,<2>=W\r\n");
		_memctl_debug_printf("<2-enable_random_delay>: <0>=Disable, <1>=Enable\r\n");
		_memctl_debug_printf("<3-PowerManagementMode> : <0>=Normal, <1>=PowerDown, <2>=Self Refresh\r\n");
		_memctl_debug_printf("   		 <3>:Reserved,<4>:CPUSleep + Self Refresh in IMEM   \r\n"); 
		_memctl_debug_printf("<4-cache_type>:<0>=cached, <1>=un-cached \r\n"); 
		_memctl_debug_printf("<5-Access_type>:<0>=8bit, <1>=16bit , <2>=32bit \r\n");
		_memctl_debug_printf("<6-Data_pattern>:<0>=random, <1>=sequential , <2>=0x5a5a5a5a \r\n");
		_memctl_debug_printf("<7-Enable random_mode>:<0>=disable, <1>=enable  \r\n");
		
		

	
	 

    while(1)
    {

      

#if 1                                     //RTL8196_208PIN_SUPPORT_DDR
        _memctl_debug_printf("\n================================\n");
        k2++;
        _memctl_debug_printf("\nBegin DRAM Test : %d\n",k2);
        _memctl_debug_printf("Dram Test parameter:\n" );
	 _memctl_debug_printf("0.CLK_MANAGE(0xb8000010)=%x\n",READ_MEM32(CLK_MANAGE) );
	 //_memctl_debug_printf("0.PAD_CONTROL(0xb8000048)=%x\n",READ_MEM32(PAD_CONTROL_REG) );
	 //_memctl_debug_printf("0.PAD_CONTROL(0xb8000048)=%x\n",READ_MEM32(0xb8000048) );
        _memctl_debug_printf("1.DIDER(0xb8001050)=%x\n",READ_MEM32(0xb8001050) );
        _memctl_debug_printf("2.DTR(0xb8001008)=%x\n",READ_MEM32(DTR) );
        _memctl_debug_printf("3.DCR(0xb8001004)=%x\n",READ_MEM32(DCR) );
        //_memctl_debug_printf("4.HS0_CONTROL(0x%x)=0x%x\n", HS0_CONTROL,REG32(HS0_CONTROL));
        _memctl_debug_printf("5.Burst times=%d \n",burst);
        _memctl_debug_printf("6.cache_type(0:cached)(1:Un-cached)=%d \n",cache_type);
        _memctl_debug_printf("7.Access_type(0:8bit)(1:16bit)(2:32bit)=%d \n",access_type);
	 _memctl_debug_printf("8.Tested size=0x%x \n",END_ADDR);        
        _memctl_debug_printf("9.Tested addr =0x%x \n",addr);
	
#endif

     

        for (samples = 0; samples < MAX_SAMPLE; samples++)
        {
            if(random_test==1)
            {
            	       cache_type = rand2() % ((unsigned int) 2);
		       access_type = rand2()  % ((unsigned int) 3);            	      
            }	    
           

           // burst = rand2() % (unsigned int) BURST_COUNTS;	
            burst =BURST_COUNTS;	

     

            //addr = 0x80000000 + START_ADDR + (rand2() % (unsigned int) (END_ADDR - START_ADDR));
            addr = 0xa0000000 + START_ADDR + (rand2() % (unsigned int) (END_ADDR - START_ADDR));


   	   
            addr = cache_type ? (addr | 0x20000000) : addr;
            wdata = rand2();

	  
            if (access_type == 0)  //8 bit
            {
                wdata = wdata & 0xFF;
            }
            else if (access_type == 1) //16 bit
            {
                addr = (addr) & 0xFFFFFFFE;
                wdata = wdata & 0xFFFF;
            }
            else //32 bit
            {
                addr = (addr) & 0xFFFFFFFC;
            }

        /* Check if Exceed Limit */
            if ( ((addr + (burst << access_type)) & 0x1FFFFFFF) > END_ADDR)
            {
                burst = (END_ADDR - ((addr) & 0x1FFFFFFF)) >> access_type;
		  _memctl_debug_printf("11.Exceed Limit,burst=%d \n", burst);
            }

#if 1
            if (samples % 100 == 0)
            {
                _memctl_debug_printf("\nSamples: %d", samples);
		  
		
		 
		   #if 1 //JSW @20091106 :For DRAM Test + Power Management 
		 	if(enable_delay)
		 	{
			     delay_time=rand2() % ((unsigned int) 1000*1000);
			     _memctl_debug_printf(" Delay_time=%d\n",delay_time);
			     for(k=0;k<=delay_time;k++); //delay_loop	

			    // CmdCPUSleepIMEM();
		 	}

			#if 1
		 	if(PM_MODE)
			{
				
			  //set bit[31:30]=0 for default "Normal Mode" and prevent unstable state transition
			  //  REG32(MPMR_REG)= 0x3FFFFFFF ;
			  REG32(MPMR_REG)= 0x040FFFFF ;

			    switch(PM_MODE)
			    {
			        case 0:
			            _memctl_debug_printf("\nDRAM : Normal mode\n");
				     //return 0;
			            break;

			        case 1:
			            _memctl_debug_printf("\nDRAM :Auto Power Down mode\n");
			            REG32(MPMR_REG)= READ_MEM32(MPMR_REG)|(0x1 <<30) ;

				    
			            break;

			        case 2:
			            _memctl_debug_printf("\nDRAM : Set to Self Refresh mode\n");				    
			            REG32(MPMR_REG)|= (0x2 <<30) ;	
				     
				    
			            break;

			        case 3:
			            _memctl_debug_printf("\nReserved!\n");			            
			            REG32(MPMR_REG)= 0x3FFFFFFF ;
				     //return 0;
			            break;

				case 4:
			            _memctl_debug_printf("\nCPUSleep + Self Refresh in IMEM!\n");
				    // CmdCPUSleepIMEM();
			            //return 0;
			            break;

			        default :
			            _memctl_debug_printf("\nError Input,should be 0~4\n");
			            break;
			     }   //end of switch(PM_MODE)
		 	}//end of if(PM_MODE)	
		 	#endif
		 #endif
		 
            }//end of switch(PM_MODE)
#endif
     

        /* Prepare Write Data */
            for (i = 0; i < burst ; i++)
            {            
            		
            		if(Data_pattern==0)
            		{
                  		 wdata = (unsigned int)(rand2());/* Prepare random data */
            		}
			else if (Data_pattern==1)
				 wdata =( (i<<0)| (i<<8) |(i<<16) |(i<<23));  /* Prepare Sequential Data */   
			else if (Data_pattern==2)
				wdata=0x5a5aa5a5;//fixed data
			else
			{
				 _memctl_debug_printf("\nError Data_pattern Input,return \n");
				 return;
			}
				
			

                if (access_type == 0)               //8 bit
                    wdata = wdata & 0xFF;
                else if (access_type == 1)          //16bit
                    wdata = wdata & 0xFFFF;

                wdata_array[i] = wdata;
     
            }            

	    
            for (i = 0, j = addr; i < burst ; i++)
            {
                if (access_type == 0)
                    *(volatile unsigned char *) (j) = wdata_array[i];//8bit
                else if (access_type == 1)
                    *(volatile unsigned short *) (j) = wdata_array[i];//16bit
                else
                    *(volatile unsigned int *) (j) = wdata_array[i];//32bit

                j = j + (1 << access_type);
        //keep reading
                if (keep_W_R_mode==1)
                {

		
				
                	 for (i = 0; i < burst ; i++)	
                	 {
                	 	if(Data_pattern==0)
            			{
            				WRITE_MEM32(0xa0700000+(i*4), rand2());                  			
            			}
				else if (Data_pattern==1)
					 /* Prepare Sequential Data */   
					WRITE_MEM32(0xa0700000+(i*4), ( (i<<0)| (i<<8) |(i<<16) |(i<<23)));					  
				else if (Data_pattern==2)
					WRITE_MEM32(0xa0700000+(i*4), wdata); 			

			 }
                		//WRITE_MEM32(0xa0700000+(i*4), 0x5aa5a55a);
					 
                	_memctl_debug_printf("\nkeep reading\n");

			keep_reading:
			 for (i = 0; i < burst ; i++)	
			 {
			 		_memctl_debug_printf("\naddr(0x%x),value=0x%x\n",0xa0700000+(i*4),REG32(0xa0700000+(i*4)));
					
			 }

					 goto keep_reading;
                }
            }

	  //keep writing
            if (keep_W_R_mode==2)
            {
            	  //_memctl_debug_printf("\nkeep writing,writing addr(0xa0800000)=0xa5a55a5a\n");
            	  _memctl_debug_printf("\nkeep writing...\n");	
	
				
		 for (i = 0; i < burst ; i++)
		 {
			  //wdata = rand2();
			  //wdata=0xffffffff;
			  wdata =( (i<<0)| (i<<8) |(i<<16) |(i<<23));  /* Prepare Sequential Data */  

			  if (access_type == 0)               //8 bit
	                    wdata = wdata & 0xFF;
	                else if (access_type == 1)          //16bit
	                    wdata = wdata & 0xFFFF;

               	 wdata_array[i] = wdata;				 
		 	  	
		 } 

		  keep_writing:	 
		  for (i = 0, j = addr; i < burst ; i++)
		 {
		  if (access_type == 0)
                    *(volatile unsigned char *)  (j) = wdata_array[i];//8bit
                else if (access_type == 1)
                    *(volatile unsigned short *)  (j)= wdata_array[i];//16bit
                else
                    *(volatile unsigned int *)  (j) = wdata_array[i];//32bit

		    j = j + (1 << access_type);
		 } 
              
       
                goto keep_writing;
            }

        /* Read Verify */
            for (i = 0, j = addr; i < burst ; i++)
            {
                unsigned rdata;

                if (access_type == 0)
                {
                    rdata = *(volatile unsigned char *) (j);
                }
                else if (access_type == 1)
                {
                    rdata = *(volatile unsigned short *) (j);
                }
                else
                {
                    rdata = *(volatile unsigned int *) (j);
                }
        //_memctl_debug_printf("\n==========In Read Verify========= \n");
        // _memctl_debug_printf("\nrdata: %d\n", rdata);
        //_memctl_debug_printf("\nwdata_array[i]: %d\n",wdata_array[i]);
        // _memctl_debug_printf("\n==========End Read Verify========= \n");

                if (rdata != wdata_array[i])
                {
                    _memctl_debug_printf("\nWrite Data Array: 0x%x", wdata_array[i]);

                    if (cache_type)
                        _memctl_debug_printf("\n==> Uncached Access Address: 0x%x, Type: %d bit, Burst: %d",
                            addr, (access_type == 0) ? 8 : (access_type == 1) ? 16 : 32, burst);
                    else
                        _memctl_debug_printf("\n==>   Cached Access Address: 0x%x, Type: %d bit, Burst: %d",
                            addr, (access_type == 0) ? 8 : (access_type == 1) ? 16 : 32, burst);

                    _memctl_debug_printf("\n====> Verify Error! Addr: 0x%x = 0x%x, expected to be 0x%x\n", j, rdata, wdata_array[i]);

        //HaltLoop:
        //goto HaltLoop;
                    return 0;

                }

                j = j + (1 << access_type);

            }                                       //end of reading

        }

    }                                               //end while(1)
}

#endif


unsigned int  RTK_ZQ_K()
{
	/* ZQ calibration*/
			
			int i;
				
			/* Configure ZQ */		
		
			//for(i=0; i< (sizeof(auto_cali_value)/sizeof(unsigned int));i++)
			unsigned int auto_cali_value=0;
			 if(memctlc_is_DDR3())
			 {
				auto_cali_value=auto_cali_value_DDR3[0];			
			 }
			else if(memctlc_is_DDR2())
			{
				auto_cali_value=auto_cali_value_DDR2[0];			
			}
		
			{
				if(0 == memctlc_ZQ_calibration(auto_cali_value))
				{
				/* We found one .*/
				_memctl_debug_printf("\n\nZQ[%d] calibration Pass\n",i);
					memctlc_check_ZQ();
					//break;
				}
				else
				{
					_memctl_debug_printf("\n\nZQ[%d] calibration fail\n",i);
					memctlc_check_ZQ();
				}
			}			
			

}

void _memctl_update_phy_param(void)
{
	volatile unsigned int *dmcr;
	volatile unsigned int *dcr;
	volatile unsigned int *dacr;
	volatile unsigned int dacr_tmp1, dacr_tmp2;
	volatile unsigned int dmcr_tmp;

	dmcr = (unsigned int *)DMCR;
	dcr = (unsigned int *)DCR;
	dacr = (unsigned int *)DACCR;
	
	/* Write DMCR register to sync the parameters to phy control. */
	dmcr_tmp = *dmcr;
	*dmcr = dmcr_tmp;
	_memctl_delay_clkm_cycles(10);	
	/* Waiting for the completion of the update procedure. */
	while((*dmcr & ((unsigned int)DMCR_MRS_BUSY)) != 0);

	__asm__ __volatile__("": : :"memory");

	/* reset phy buffer pointer */
	dacr_tmp1 = *dacr;
	dacr_tmp1 = dacr_tmp1 & (0xFFFFFFEF);
	dacr_tmp2 = dacr_tmp1 | (0x10);
	*dacr = dacr_tmp1 ;

	_memctl_delay_clkm_cycles(10);	
	__asm__ __volatile__("": : :"memory");
	*dacr = dacr_tmp2 ;

	return;
}



void _memctl_disable_hw_auto_cali(void)
{
	unsigned int i;
	volatile unsigned int *dacspcr, *dacdqr;

	dacspcr = (volatile unsigned int *)DACSPCR;
	dacdqr = (volatile unsigned int *)DACDQR;
	
	*dacspcr &= (~(1<<31));

	for(i=0;i<32;i++){
		*dacdqr &= (~(1<<31));
		dacdqr++;
	}

	_memctl_update_phy_param();

	return;
}


#if 0
unsigned int DDR_short_ZQ()
{
		_memctl_debug_printf("\nEnable short ZQ for DDR3\n");
			volatile unsigned int *d3zqccr;
			d3zqccr = (volatile unsigned int *)D3ZQCCR;

			/* 4. Enable short ZQ */
			if(memctlc_is_DDR3()){
				*d3zqccr |= (1<<30);
			}else{
				*d3zqccr &= (~(1<<30));
			}		

}
#endif

void memctlc_DBFM_enalbe(void)
{
	volatile unsigned int *mcr;

	mcr = (volatile unsigned int *)MCR;

	*mcr = *mcr |0x1e0;

	return;

}

void memctlc_show_reg_info(void)
{
#if 1
				/* Show register informantion. */	
	//_memctl_debug_printf("\nMem Final setting info:\n");
	printf("\tDRAM frequency: %dMHz\n", board_DRAM_freq_mhz());
	  memctlc_check_DCR();
	_memctl_debug_printf("\tMCR (0x%08x):0x%08x\n", MCR, REG(MCR));
	_memctl_debug_printf("\tDCR (0x%08x):0x%08x\n", DCR, REG(DCR));
	_memctl_debug_printf("\tDTR0(0x%08x):0x%08x\n", DTR0,REG(DTR0));
	_memctl_debug_printf("\tDTR1(0x%08x):0x%08x\n", DTR1,REG(DTR1));
	_memctl_debug_printf("\tDTR2(0x%08x):0x%08x\n", DTR2,REG(DTR2));
	_memctl_debug_printf("\tDIDER1(0x%08x):0x%08x\n", DIDER1,REG(DIDER1));
	_memctl_debug_printf("\tDIDER2(0x%08x):0x%08x\n", DIDER2,REG(DIDER2));
	_memctl_debug_printf("\tDIDER3(0x%08x):0x%08x\n", DIDER3,REG(DIDER3));
	_memctl_debug_printf("\tDACDQ0(0x%08x):0x%08x\n", DACDQ0,REG(DACDQ0));
#endif
}

void memctlc_8198C_PI_init(void)
{
//Normal booting flow
			//Step1
			
		
			int i,PI_freq_DDR;

			if(memctlc_is_DDR3())
				PI_freq_DDR=550;

			if(memctlc_is_DDR2())
				PI_freq_DDR=510;

			

			if(board_DRAM_freq_mhz()<PI_freq_DDR)	//can go Chariot at 250MHZ		
			{			

			
				_memctl_debug_printf("\n2.Set DRAM_CMD_Phase rising of clk\n");	
				REG32(DWPR1)=0x40000000;
				_memctl_debug_printf("DWPR1(0x%x)=0x%x\n",DWPR1,REG32(DWPR1));

				
			_memctl_debug_printf("\n1.Set DRAM clk phase inverter \n");	
			REG32(DOR)=0x100000ff;
			_memctl_debug_printf("DOR(0x%x)=0x%x\n",DOR,REG32(DOR));	

			_memctl_debug_printf("\n3.PI phase\n");	
			//REG32(0xb80000dc)=0x2CB00400;  //suggestion by PM
			//REG32(0xb80000dc)=0x08200400;//make clock can trigger CAS's low , 330MHZ
			

			 if( (memctlc_is_DDR2()))      //RTL8198CD+DDR2
                        {
                                REG32_ANDOR(0xb80000dc, ~((0x3f<<24)|(0x3f<<18)), ((0x2c<<24)|(0x2c<<18)))//set PI = 12
                        }
                        else
                        {
                                REG32_ANDOR(0xb80000dc, ~((0x3f<<24)|(0x3f<<18)), ((0x2c<<24)|(0x2C<<18)))//set PI = 12
                        }



			_memctl_debug_printf("0xb80000dc(0x%x)=0x%x\n",0xb80000dc,REG32(0xb80000dc));	
			_memctl_debug_printf("0xb80000dc(0x%x)=0x%x\n",0xb80000dc,REG32(0xb80000dc));	

			_memctl_debug_printf("\n4.DQ phase inv\n");	
			//REG32(0xb8001804)|=(5<<29);

			REG32(0xb8001804)|=(5<<29);
			//REG32(0xb8001804)=0xbf0f0000;
			//REG32(0xb8001804)=0xbf030000;
			_memctl_debug_printf("0xb8001804(0x%x)=0x%x\n",0xb8001804,REG32(0xb8001804));		

			

			_memctl_debug_printf("\n5.PLL div2\n");	
			REG32(0xb80000d8)|=(1<<21);
			_memctl_debug_printf("0xb80000d8(0x%x)=0x%x\n",0xb80000d8,REG32(0xb80000d8));

			_memctl_debug_printf("\n6.PLL multi 2\n");	
			REG32(0xb80000d8)&=~(1<<21);
			_memctl_debug_printf("0xb80000d8(0x%x)=0x%x\n",0xb80000d8,REG32(0xb80000d8));

				//Step5 , DACQ0~15
			_memctl_debug_printf("\n5.Manual set DACQ0~15\n"); 			
			 for(i=0;i<32;i++)
			 {			 	
 				REG32( 0xb8001510 +i*4 ) = 0x00040200; //DDR3 Chariot 600 MHZ OK.
 				// REG32( 0xb8001510 +i*4 )  = (0x1f<<24)|0x00040200;
 				
			
			 } 		
			

			
			}
			else   	//
			{
				#if 0 //For DDR3 660MHZ 
			
				#else
				
			_memctl_debug_printf("\n2.Set DRAM_CMD_Phase falling of clk\n");	
			REG32(DWPR1)=0xc0000000;


			_memctl_debug_printf("\n0.Set clk Tx delay=8\n");	
			REG32(CLK_MANAGE)|=8<<5;


			 for(i=0;i<32;i++)
			 {			 	
 				//REG32( 0xb8001510 +i*4 ) = 0x00040200; //DDR3 Chariot 600 MHZ OK.
 				 REG32( 0xb8001510 +i*4 )  = (0x8<<24)|0x00040200; 				
			
			 } 		
			

			//Step3 , DCR
			//_memctl_debug_printf("\n3.Set DCR(128MB)\n"); 
			//REG32(DCR)=0x21220000;//128MB

			

				  
	  			#endif
			}	
			
			
			REG32(DIDER1)=0x1f1f0000;	
				
}
#define RTL_8198C 0
#define RTL_DEFAULT 1
#define RTL_DEFAULT1 0x81
#define RTL_8198CD 0x2
#define RTL8954E 0x3
#define RTL_8198CS 0x80
#define RTL8954ES 0x83
#define BSP_SYS_CLK_RATE	  	(200 * 1000 * 1000)     //HS1 clock : 200 MHz
#define BSP_TC_BASE         0xB8003100
#define BSP_TC0DATA         (BSP_TC_BASE + 0x00)
#define BSP_TC1DATA         (BSP_TC_BASE + 0x04)
   #define BSP_TCD_OFFSET      4
#define BSP_TC0CNT          (BSP_TC_BASE + 0x08)
#define BSP_TC1CNT          (BSP_TC_BASE + 0x0C)
#define BSP_TCCNR           (BSP_TC_BASE + 0x10)
   #define BSP_TC0EN           (1 << 31)
   #define BSP_TC0MODE_TIMER   (1 << 30)
   #define BSP_TC1EN           (1 << 29)
   #define BSP_TC1MODE_TIMER   (1 << 28)
#define BSP_TCIR            (BSP_TC_BASE + 0x14)
   #define BSP_TC0IE           (1 << 31)
   #define BSP_TC1IE           (1 << 30)
   #define BSP_TC0IP           (1 << 29)
   #define BSP_TC1IP           (1 << 28)
#define BSP_CDBR            (BSP_TC_BASE + 0x18)
   #define BSP_DIVF_OFFSET     16
#define BSP_WDTCNR          (BSP_TC_BASE + 0x1C)

#define BSP_DIVISOR         8000

//------------------------------------------------------------------
void start_c()
{	
#if CONFIG_ESD_SUPPORT
	REG32(BSP_CDBR)=(BSP_DIVISOR) << BSP_DIVF_OFFSET;
        REG32(BSP_TC0DATA) = (((BSP_SYS_CLK_RATE/BSP_DIVISOR)/HZ)) << 4;
        REG32(0xb800311c)=0x00600000;
	/*For DDR3 must , for DDR2 safe*/
#endif
	memctlc_DBFM_enalbe();
	
	/*Realtek RTL8198C ZQ pad calibration*/
	RTK_ZQ_K();
	
	//-----------------------------------------------------------------	
	//REG32(0xb8001200)=0xffc00000; //Set SPI clock div 16
	unsigned int v=REG32(SYS_HW_STRAP);
	unsigned char boot_sel=GET_BITVAL(v, 0, RANG3);
	unsigned char dramtype_sel=GET_BITVAL(v, 3, RANG1);
	//unsigned char m2x_freq_sel=GET_BITVAL(v, 11, RANG4);  //bit [14:10], but bit 10 is not strap pin
	unsigned char m2x_freq_sel=GET_BITVAL(v, 10, RANG5);  //bit [14:10], but bit 10 is not strap pin

	//Make sure strap-pin not get error
	_memctl_debug_printf("\nDefault Strap =%x\n",v);	
	_memctl_debug_printf("Boot Mode=%s\n",boot_type_tab[boot_sel]);
	_memctl_debug_printf("DRAM Type=%s\n", dram_type_tab[dramtype_sel]);	
	_memctl_debug_printf("CLK(MHZ)=%d\n",m2x_clksel_table[m2x_freq_sel]);
	_memctl_debug_printf("boot_sel =%d\n",boot_sel);
	_memctl_debug_printf("dramtype_sel=%d\n",dramtype_sel);
	_memctl_debug_printf("m2x_freq_sel=%d\n",m2x_freq_sel);


	//-----------
	//Rescur for debug
	#if 1
	if(kbhit(0x2000))
	{
		if(uart_inc()=='r')
		{	
			RescueMode();			
			
			return ;		
		}
		
	}
	#endif	
	
	#if 1
	_memctl_debug_printf("\n8198C Cstart\n");


	//if(memctlc_is_DDR3()){	
	if(1){	//for both DDR2 and DDR3
	#if 1
	
		if(memctlc_is_DDR2()){
			//REG32(SYS_HW_STRAP)=0x59004260; //Set memory clock=390MHZ	
			       unsigned int tmp_v=0;
                                 tmp_v=REG32(SYS_HW_STRAP);
                                REG32(SYS_HW_STRAP) = ( tmp_v &~(0x1f<<10))|(0x10<<10);
		}
		else {
			// todo: 8198C DDR3 670MHz; 8198CS DDR3 740MHz
			//		 8954E DDR3 670MHz; 8954ES DDR3 740MHz
			unsigned int tmp_v=0;
			tmp_v=REG32(SYS_HW_STRAP);
			if ((REG32(0xb800000c)==RTL_8198C)||(REG32(0xb800000c)==RTL8954E||(REG32(0xb800000c)==RTL8954E)||(REG32(0xb800000c)==RTL8954ES)))
			{
			//	REG32(SYS_HW_STRAP)=0x59006E60;//Set memory clock=740HZ
				REG32(SYS_HW_STRAP) = ( tmp_v &~(0x1f<<10))|(0x11<<10);
				if ((REG32(0xb800000c)==RTL8954E)||(REG32(0xb800000c)==RTL8954ES))
					REG32(0xb80000a0) |= 0x10; // for the case of 5181 500MHz < mem 580MHz
			}
			else if (REG32(0xb800000c)==RTL_8198CD)
			{
			//	REG32(SYS_HW_STRAP)=0x59005A60;//Set memory clock=655H
				 REG32(SYS_HW_STRAP) = ( tmp_v &~(0x1f<<10))|(0xE<<10);
			}
			else
			{
				if((REG32(0xb800000c)==RTL_DEFAULT1))
				//REG32(SYS_HW_STRAP)=0x59006E60;//Set memory clock=740HZ
				 REG32(SYS_HW_STRAP) = ( tmp_v &~(0x1f<<10))|(0x11<<10);
				else
				//REG32(SYS_HW_STRAP)=0x59004660;//Set memory clock=580MHz
				 REG32(SYS_HW_STRAP) = ( tmp_v &~(0x1f<<10))|(0x11<<10);
			}
		}
	//REG32(SYS_HW_STRAP)=0x59000260; //Set memory clock=250MHZ	

	//REG32(SYS_HW_STRAP)=0x59000A60; //Set memory clock=290MHZ	

	//REG32(SYS_HW_STRAP)=0x59001260; //Set memory clock=330MHZ	//Pass 12hrs
	
	//REG32(SYS_HW_STRAP)=0x59001660; //Set memory clock=350MHZ	
	
	//REG32(SYS_HW_STRAP)=0x59001A60; //Set memory clock=370MHZ	

	//REG32(SYS_HW_STRAP)=0x59004260; //Set memory clock=390MHZ	

	//REG32(SYS_HW_STRAP)=0x59002A60; //Set memory clock=450MHZ		
	
	//REG32(SYS_HW_STRAP)=0x59003260;//Set memory clock=490MHZ	

	//REG32(SYS_HW_STRAP)=0x59003660;//Set memory clock=510MHZ	

	//REG32(SYS_HW_STRAP)=0x59003A60;//Set memory clock=530MHZ	

	//REG32(SYS_HW_STRAP)=0x59003E60;//Set memory clock=550MHZ	

	//REG32(SYS_HW_STRAP)=0x59004660;//Set memory clock=580MHZ	 , Linux bring-up OK, 

	//REG32(SYS_HW_STRAP)=0x59004A60;//Set memory clock=595MHZ,Linux bring-up OK on QA Board and demo board
	
	//REG32(SYS_HW_STRAP)=0x59004E60;//Set memory clock=610MHZ,Linux bring-up OK on QA Board , demo board fail

	//REG32(SYS_HW_STRAP)=0x59005260;//Set memory clock=625MHZ		,Linux bring-up OK on QA Board , demo board fail

	//REG32(SYS_HW_STRAP)=0x59005660;//Set memory clock=640HZ	 ,Linux bring-up OK on QA Board , demo board fail

	//REG32(SYS_HW_STRAP)=0x59005A60;//Set memory clock=655HZ	 ,Linux bring-up fail on QA Board fail

	//REG32(SYS_HW_STRAP)=0x59005E60;//Set memory clock=670HZ

	//REG32(SYS_HW_STRAP)=0x59006260;//Set memory clock=685HZ

	//REG32(SYS_HW_STRAP)=0x59006660;//Set memory clock=700HZ

	//REG32(SYS_HW_STRAP)=0x59006A60;//Set memory clock=720HZ

	//REG32(SYS_HW_STRAP)=0x59006E60;//Set memory clock=740HZ

	//REG32(SYS_HW_STRAP)=0x59007260;//Set memory clock=755HZ

	//REG32(SYS_HW_STRAP)=0x59007E60;//Set memory clock=800HZ

	//thlin 20140117
#ifdef CONFIG_RLX5181_TEST //enable when DDR3 clock >= 490MHz, when 5181 cpu clock = 500MHz
	REG32(SYS_HW_STRAP);
	
	//printf("=======> REG32(SYS_HW_STRAP) = 0x%x\n", REG32(SYS_HW_STRAP));
	if ( (REG32(SYS_HW_STRAP)&0x7C00) >= 0x01100)	
	{
		memctlc_dram_phy_reset();
		REG32(0xb80000a0) |= 0x10;	//CLK_OC2_SLOWER	(OCP2  < DRAM)
	}	
#endif

	#else
	//_memctl_debug_printf("20131119:Set MemClk as 250MHZ as default\n");	
	//REG32(SYS_HW_STRAP)=0x59000260; //Set memory clock=250MHZ		
	//REG32(0xb80000d8)=0x562B5DC0; //	clk/2	
	//REG32(0xb80000a0)&=0xFFFFFFF8; //	bit{2:0] set to zero when mem > lx clock

	
	#endif
	}

	
	
	

	boot_sel=GET_BITVAL(v, 0, RANG3);
	dramtype_sel=GET_BITVAL(v, 3, RANG1);
	//m2x_freq_sel=GET_BITVAL(v, 11, RANG4);  //bit [14:10], but bit 10 is not strap pin
	m2x_freq_sel=GET_BITVAL(v, 10, RANG5);  //bit [14:10], but bit 10 is not strap pin	
	_memctl_debug_printf("Boot Mode=%s\n",boot_type_tab[boot_sel]);
	_memctl_debug_printf("DRAM Type=%s\n", dram_type_tab[dramtype_sel]);	
	_memctl_debug_printf("CLK(MHZ)=%d\n",m2x_clksel_table[m2x_freq_sel]);
	_memctl_debug_printf("boot_sel =%d\n",boot_sel);
	_memctl_debug_printf("dramtype_sel=%d\n",dramtype_sel);
	_memctl_debug_printf("m2x_freq_sel=%d\n",m2x_freq_sel);
	_memctl_debug_printf("\nModified Strap =%x\n",REG32(SYS_HW_STRAP));		

	
	#endif
	
			

			
			if(memctlc_is_DDR3()){				

			_memctl_debug_printf("\nDDR3 init flow\n"); 
		

			memctlc_8198C_PI_init();

			_memctl_update_phy_param();


			#if 0
			
			memctlc_hw_auto_calibration(16,0xa4000000); //Step6
			printf("memctlc_hw_auto_calibration\n");

			DDR_short_ZQ();
			
			_memctl_disable_hw_auto_cali();
			printf("memctlc_hw_auto_calibration\n");
			
			#endif			
			
						
			/*
				 ddr2_16bit_size[] = { 
				 0x11210000 * 32MB *, 
				 0x11220000 * 64MB *, 
				 0x21220000 * 128MB *, 
				 0x21320000/ 256MB /, 
				 0x21420000/ 512MB /};
			*/

			

			#if 1
			_memctl_debug_printf("\nSet DCR\n"); 
			REG32(DCR)=0x21220000;//128MB
			
			#endif			
						
			_memctl_update_phy_param();

			
			while(1){			
			

			_memctl_debug_printf("Config DTR for DDR3()\n");		
			memctlc_config_DTR();		
			_memctl_update_phy_param();
			
				
			_memctl_debug_printf("Invoke memctlc_ddr3_dll_reset()\n");
			memctlc_ddr3_dll_reset();
			_memctl_update_phy_param();				

			

			#if 0 //Enable read cycle -1
			if(board_DRAM_freq_mhz()>=390)
			{
				REG32(0xb8001004)|=(1<<14);
			}
			#endif
						

			#if 1
			/*When DDR up to 550MHZ , RTL8198C's read cycle=CL+1*/

			if(board_DRAM_freq_mhz()>=550)
			{
			unsigned int CASminusOne=REG32(0xb8001008)>>28;
			_memctl_debug_printf("\nDefault DTR0(0x%x)=0x%x\n",DTR0 ,REG32(DTR0) );	
			
			REG32(0xb8001008)&=0x0fffffff;//set CAS=0

			//CASminusOne-=1;
			CASminusOne+=1;

			REG32(0xb8001008)|=CASminusOne<<28;			

			//_memctl_debug_printf("\nAfter CAS-1,DTR0(0x%x)=0x%x\n",DTR0 ,REG32(DTR0) );	
			_memctl_debug_printf("\nAfter CAS+1,DTR0(0x%x)=0x%x\n",DTR0 ,REG32(DTR0) );				
			}
			#else
			//Check DTR 		
			memctlc_check_DTR_DDR3();			
			_memctl_update_phy_param();
			#endif
			memctlc_check_DTR();	
			_memctl_update_phy_param();


			#if 1 //K read and show read margin		
			
			
			 ShowRxDelayMap(); 				
			
			 DDR_cali_API7();

			_memctl_debug_printf("DRAM size detect\n");
			memctlc_dram_size_detect(); //Step4: Size detect , set DMCR to activate DRAM parameter
			
			#else		//K write	
			 //DDR_cali_API7();
			REG32(0xb8001050)=0x0f0f0000; //Open loop
			REG32(0xb8001500)=0x80080800; //RX DQS

			//ShowTxDelayMap();
			 REG32_ANDOR(CLK_MANAGER,    ~( (0x1f<<5) | (0x1f<<0) ),   (tx<<5)  );
	    		 //REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (rx<<24) | (rx<<16) );

	    		 REG32_ANDOR(DWPR1,    ~( (0x1f<<21) | (0x1f<<16) ),   (dq<<21) |  (dq<<16) );

			for(i=0;i<32;i++)
	    		{
	    			REG32( 0xb8001510 +i*4 )  = (tx_final<<24)|0x00040200; 			
		       } 	
			
			#endif		

			
					

			#if 1		
			/*Good for Write , but seems not to be needed because DRAM has pulled DQ to 1/2 VDD already)*/ 
			REG32(0xb800107c)|=0x80000000;//Eanble RTL8198C ODT

			/*Good for read*/
			REG32(0xb800107c)|=0x40000000;/*Eanble RTL8198C TE*/
			#endif	
				
			_memctl_debug_printf("Test:Write 0xa0400000=0xa5a55a5a\n");
			REG32(0xa0400000)=0xa5a55a5a;
			_memctl_debug_printf("0xa0400000=0x%x\n",REG32(0xa0400000) );	

			_memctl_debug_printf("Test:Write 0xa0500000=0xa5005a00\n");
			REG32(0xa0500000)=0xa5005a00;
			_memctl_debug_printf("0xa0500000=0x%x \n",REG32(0xa0500000) );	

			_memctl_debug_printf("Test:Write 0xa0600000=0x98765432\n");
			REG32(0xa0600000)=0x98765432;
			_memctl_debug_printf("0xa0600000=0x%x \n",REG32(0xa0600000) );

			_memctl_debug_printf("Test:Write 0xa0700000=0x5a5aa5a5\n");
			REG32(0xa0700000)=0x5a5aa5a5;
			_memctl_debug_printf("0xa0600000=0x%x \n",REG32(0xa0700000) );	

			if((REG32(0xa0400000)==0xa5a55a5a)&&(REG32(0xa0500000)==0xa5005a00)&&(REG32(0xa0600000)==0x98765432))
			{
				_memctl_debug_printf("\nDDR3 Init and R/W Pass\n");	
				_memctl_debug_printf("\nDDR3 init flow done\n"); 	
				memctlc_show_reg_info();
				break;
			}
			

			}//end of while 1 loop
			

				//return;
			}
		
						
		
			

			if(memctlc_is_DDR2()){
			
			_memctl_debug_printf("\nDDR2 init flow\n"); 				
				

			memctlc_8198C_PI_init();
			
			
			//Step2
			_memctl_debug_printf("\n3.Set Safe DTR\n"); 
			//memctlc_config_DTR();
			/*
				 ddr2_16bit_size[] = { 
				 0x11210000 * 32MB *, 
				 0x11220000 * 64MB *, 
				 0x21220000 * 128MB *, 
				 0x21320000/ 256MB /, 
				 0x21420000/ 512MB /};
			*/

			_memctl_update_phy_param();
			#if 1
			//REG32(DTR0)=0x37277240; //set DTR0,CL=4 , CWL= 3 //For RTL8198C ASIC setting
			//REG32(DTR1)=0x10101010; //set DTR1
			//REG32(DTR2)=0x0800B000; //set DTR2
			//REG32(0xb8001004)=0x21220000; //set DDR2 128MB 	
			
			//Step3 , DCR
			_memctl_debug_printf("\n3.Set DCR\n"); 
			REG32(DCR)=0x11210000;//32MB
			
			memctlc_config_DTR();
			_memctl_update_phy_param();
		
			#endif			
 
 			
			_memctl_debug_printf("Invoke memctlc_ddr2_dll_reset()\n");
			memctlc_ddr2_dll_reset();
			_memctl_update_phy_param();		
		

			#if 1
			/*When DDR up to 530MHZ , RTL8198C's read cycle=CL+1*/

			//if(board_DRAM_freq_mhz()>=530)
			if(board_DRAM_freq_mhz()>=510)
			{
			unsigned int CASminusOne=REG32(0xb8001008)>>28;
			_memctl_debug_printf("\nDefault DTR0(0x%x)=0x%x\n",DTR0 ,REG32(DTR0) );	
			
			REG32(0xb8001008)&=0x0fffffff;//set CAS=0

			//CASminusOne-=1;
			CASminusOne+=1;

			REG32(0xb8001008)|=CASminusOne<<28;			

			//_memctl_debug_printf("\nAfter CAS-1,DTR0(0x%x)=0x%x\n",DTR0 ,REG32(DTR0) );	
			_memctl_debug_printf("\nAfter CAS+1,DTR0(0x%x)=0x%x\n",DTR0 ,REG32(DTR0) );				
			}
			#else
			//Check DTR 		
			//memctlc_check_DTR_DDR3();			
			//_memctl_update_phy_param();
			#endif
			//chk_dram_cfg(0); //Step5: auto set DTR (check system register's freq setting)
			memctlc_check_DTR();
			
			_memctl_update_phy_param();
					
		

			#if 1 //K read
			 ShowRxDelayMap(); 
			_memctl_update_phy_param();
			
			 DDR_cali_API7();
			 _memctl_update_phy_param();

			 _memctl_debug_printf("DRAM size detect\n");
			memctlc_dram_size_detect(); //Step4: Size detect , set DMCR to activate DRAM parameter
			#else		//K write	
			 //DDR_cali_API7();
			REG32(0xb8001050)=0x0f0f0000; //Open loop
			REG32(0xb8001500)=0x80080800; //RX DQS

			//ShowTxDelayMap();
			 REG32_ANDOR(CLK_MANAGER,    ~( (0x1f<<5) | (0x1f<<0) ),   (tx<<5)  );
	    		 //REG32_ANDOR(DIDER1,    ~( (0x1f<<24) | (0x1f<<16) ),   (rx<<24) | (rx<<16) );

	    		 REG32_ANDOR(DWPR1,    ~( (0x1f<<21) | (0x1f<<16) ),   (dq<<21) |  (dq<<16) );

			for(i=0;i<32;i++)
	    		{
	    			REG32( 0xb8001510 +i*4 )  = (tx_final<<24)|0x00040200;
 			
		       } 	
			
			#endif


			_memctl_debug_printf("\nDDR2 init flow done\n"); 
			memctlc_show_reg_info();

	
			}

				/////////////////////////////////////////////////////////
 #if 0//def CONFIG_RTL8198C_FT1_TEST

#define ECO_NO_REG 0xb8000000
	
	#define HW_STRAP_REG 0xb8000008
	
	#define Bond_Option_REG 0xb800000c
 unsigned int chip_id_code=REG32(ECO_NO_REG);
 unsigned int Bond_Option_value=REG32(Bond_Option_REG);	
  printf("\n{SV001}");    
 printf("\n{0x%x}",chip_id_code);          
 printf("\n{0x%x}",Bond_Option_value);   


 
   printf("\n{DRAM=%d MHZ}\n",board_DRAM_freq_mhz());
	

//	prom_printf("Mode=%s\n",boot_type_tab[boot_sel]);
   printf("{%s=%d MB}\n", dram_type_tab[dramtype_sel],REG32(0xb8000f00));


 
#endif 
 

}
//==============================================================









