

#include "../autoconf.h"
#include "start.h"
#include "test_hw_81a.h"

void EnableIP_PADControl(unsigned int dramtype);
void DRAM_ModeRegister(unsigned int dramtype);
void LookUp_MemTimingTable(unsigned int dramtype, unsigned int dramsize, unsigned int m2xclk);
void CLK_M_Adjust(unsigned int dramtype,unsigned int m1xclk);





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


#if DBG
#if 1//DBG
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

#endif


#define Put_UartData uart_outc

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
#endif
#if 0
void printf(const char *fmt, ...)
{	(void)vsprintf(0, fmt, ((const int *)&fmt)+1);	
}
#endif
//--------------------------------------------------
//==============================================================

#if 1
void DDR_cali_API7(void)
{
    register int i,j,k;

    register int L0 = 0, R0 = 33, L1 = 0, R1 = 33;
    const register int  DRAM_ADR = 0xA0100000;
    const register int  DRAM_VAL = 0x5A5AA5A5;
    const register int  DDCR_ADR = 0xB8001050;
    register int  DDCR_VAL = (1<<31);  //digital
    //register int  DDCR_VAL = (0<<31);  //analog	


    REG32(DRAM_ADR) = DRAM_VAL;

    while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);
 


         // Calibrate for DQS0
        for (i = 1; i <= 31; i++)
        {
            CHECK_DCR_READY();
            REG32(DDCR_ADR) = (DDCR_VAL & 0x80000000) | ((i-1) << 25);

            if (L0 == 0)
            {   
            	if ((REG32(DRAM_ADR) & 0x00FF00FF) == 0x005A00A5)        
				{      L0 = i;         
						//printf("\nL0=%d:OK\n",i);
				}
			
            }
            else
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) != 0x005A00A5)          
            		{   R0 = i - 1;  
				//printf("R0=%d:fail\n\n",i);
				break;            
			}
			

		}
        }
        //DDCR_VAL = (DDCR_VAL & 0xC0000000) | (((L0 + R0) >> 1) << 25);
        DDCR_VAL = (DDCR_VAL & 0xC0000000) | ((((L0 + R0) >> 1)) << 25);
        REG32(DDCR_ADR) = DDCR_VAL;

                 // Calibrate for DQS1
        for (i = 1; i <= 31; i++)
        {
            CHECK_DCR_READY();
            REG32(DDCR_ADR) = (DDCR_VAL & 0xFE000000) | ((i-1) << 20);
            if (L1 == 0)
            {    
            	if ((REG32(DRAM_ADR) & 0xFF00FF00) == 0x5A00A500)        
		{   
			L1 = i;    
			//printf("\nL1=%d:OK\n",i);
		}
            }
            else
            {  
            	if ((REG32(DRAM_ADR) & 0xFF00FF00) != 0x5A00A500)        
		{   R1 = i - 1;     
				//printf("R1=%d:fail\n\n",i);
				break;             
		}
            }
        }

        // ASIC
        //DDCR_VAL = (DDCR_VAL & 0xFE000000) | (((L1 + R1) >> 1) << 20);
        DDCR_VAL = (DDCR_VAL & 0xFE000000) | ((((L1 + R1) >> 1)) << 20);
        REG32(DDCR_ADR) = DDCR_VAL;

#if DBG
        printf("\nL0:%d R0:%d C0:%d        ",  L0, R0, (L0 + R0) >> 1);
        printf("\nL1:%d R1:%d C1:%d\n",  L1, R1, (L1 + R1) >> 1);
	 printf("DDCR(0x%x)=0x%x\n", DDCR_REG,REG32(DDCR_REG));
#endif

}
#endif

#if CONFIG_Calibration_DQS_EN_TAP
void DDR_cali_API8(void)
{
    register int i,j,k;

    register int L0 = 0, R0 = 33, L1 = 0, R1 = 33;
    const register int  DRAM_ADR = 0xA0100000;
    const register int  DRAM_VAL = 0x5A5AA5A5;
    const register int  DDCR_ADR = 0xB8001050;
    register int  DDCR_VAL = (1<<31);  //digital
    
    //register int  DDCR_VAL = (0<<31);  //analog	

   int DDCR_VAL_backup =REG32(DDCR_ADR);//backup DQS0/1 delay tap

    REG32(DRAM_ADR) = DRAM_VAL;

    while( (REG32(DDCR_ADR) & 0x40000000) != 0x40000000);    


         // Calibrate for DQS0 EN
        for (i = 1; i <= 31; i++)
        {
            CHECK_DCR_READY();
            REG32(DDCR_ADR) = (DDCR_VAL & 0x80000000) | ((i-1) << 15);

            if (L0 == 0)
            {   
            	if ((REG32(DRAM_ADR) & 0x00FF00FF) == 0x005A00A5)        
				{      L0 = i;         
						//printf("\nL0=%d:OK\n",i);
				}
			
            }
            else
            {   if ((REG32(DRAM_ADR) & 0x00FF00FF) != 0x005A00A5)          
            		{   R0 = i - 1;  
				//printf("R0=%d:fail\n\n",i);
				break;            
			}
			

		}
        }
        DDCR_VAL = (DDCR_VAL & 0xFFF00000) | (((L0 + R0) >> 1) << 15);
        //DDCR_VAL = (DDCR_VAL & 0xC0000000) | ((((L0 + R0) >> 1)-2) << 25);
        REG32(DDCR_ADR) = DDCR_VAL;

                 // Calibrate for DQS1
        for (i = 1; i <= 31; i++)
        {
            CHECK_DCR_READY();
            REG32(DDCR_ADR) = (DDCR_VAL & 0xFE000000) | ((i-1) << 10);
            if (L1 == 0)
            {    
            	if ((REG32(DRAM_ADR) & 0xFF00FF00) == 0x5A00A500)        
		{   
			L1 = i;    
			//printf("\nL1=%d:OK\n",i);
		}
            }
            else
            {  
            	if ((REG32(DRAM_ADR) & 0xFF00FF00) != 0x5A00A500)        
		{   R1 = i - 1;     
				//printf("R1=%d:fail\n\n",i);
				break;             
		}
            }
        }

        // ASIC
        DDCR_VAL = (DDCR_VAL & 0xFFFF8000) | (((L1 + R1) >> 1) << 10);
        //DDCR_VAL = (DDCR_VAL & 0xFE000000) | ((((L1 + R1) >> 1)-2) << 20);
        REG32(DDCR_ADR) = DDCR_VAL;

	 REG32(DDCR_ADR) |=DDCR_VAL_backup;
#if DBG
        printf("\nEN L0 :%d R0:%d C0:%d        ",  L0, R0, (L0 + R0) >> 1);
        printf("\nEN L1 :%d R1:%d C1:%d\n",  L1, R1, (L1 + R1) >> 1);
	 printf("EN DDCR (0x%x)=0x%x\n", DDCR_REG,REG32(DDCR_REG));
		
#endif
	
	 //printf("Final DDCR(0x%x)=0x%x\n", DDCR_REG,REG32(DDCR_REG));

}

void ShowDDCRDelayMap()
{
    unsigned register DQS_TAP,DQS_EN_TAP;
	#define REG32_ANDOR(x,y,z)  { REG32(x)=( REG32(x) & (y) ) | (z);  }

	
    for (DQS_TAP =0; DQS_TAP<=31; DQS_TAP++)
    {
    
		
        printf("\nDQS_TAP=%02x : ",DQS_TAP );
        for (DQS_EN_TAP =0; DQS_EN_TAP<= 31; DQS_EN_TAP++)//rx loop
        {
            
                    

            REG32_ANDOR(DDCR_REG,    ~( (0x1f<<25) | (0x1f<<20) | (0x1f<<15)  | (0x1f<<10)  ),   (DQS_TAP<<25) | (DQS_TAP<<20) | (DQS_EN_TAP<<15) | (DQS_EN_TAP<<10));
	     CHECK_DCR_READY();  

            REG32(ADDR)=PATT0;      if(REG32(ADDR)!=PATT0)          goto failc0;
            REG32(ADDR)=PATT1;      if(REG32(ADDR)!=PATT1)          goto failc0;
            REG32(ADDR)=PATT2;      if(REG32(ADDR)!=PATT2)          goto failc0;
            REG32(ADDR)=PATT3;      if(REG32(ADDR)!=PATT3)          goto failc0;
            REG32(ADDR)=PATT4;      if(REG32(ADDR)!=PATT4)          goto failc0;

            printf("%02x,",  DQS_EN_TAP);
            continue;

            failc0:
            printf("--," );
            continue;

        }
    }
}


#endif

//==========================================

#define DRAM_SDR 0
#define DRAM_DDR1 1
#define DRAM_DDR2 2

#define SDR_DCR_TAB_COL  2
#define SDR_DCR_TAB_ROW  6


const unsigned int SDR_DCR_TAB[][2]={
						2,    0x14000000,
						8, 	0x14820000,
						16,	0x14920000,
					  	32, 	0x15120000,
					   	64,	0x15220000,
					  	128, 0x15220000,   // < 200M
					       256, 0x15A20000,						
					};

#ifdef CONFIG_MCM_AUTO
#define SDR_DTR_TAB_ROW  3
#define SDR_DTR_TAB_COL  6

const unsigned int SDR_DTR_TAB[][SDR_DTR_TAB_COL]={  
		0,		312,			287,			262,			237,			212,
		//2,		0x22302619,	0x223021d9,	0x22281F91,  0x22201F51,	0x11201ad1,
		8,		0x22302619,	0x223021d9,	0x22281F91,  0x22201F51,	0x11201ad1,
		16,		0x22302619,	0x223021d9,	0x22281F91,  0x22201F51,	0x11201ad1,
		//32,		0x22302611,	0x223021d1,	0x22281F89,	 0x22201F49,	0x11201ac9,
		//64,		0x22302611,	0x223021d1,	0x22281F89,	 0x22201F49,	0x11201ac9,
							};
#define DDR1_DTR_TAB_ROW  2
#define DDR1_DTR_TAB_COL   (9)

const unsigned int DDR1_DTR_TAB[][DDR1_DTR_TAB_COL]={		
      	0,      387,         	362,         	337,        	 312,        	 287,         	262,         	237,         	212,
	//16, 	0x4438369a,  0x4438329a,  0x44302e5a,  0x44302a1a,  0x442829da,  0x33282791,  0x33202351,  0x33201ed1,
	32, 	0x44383692,  0x44383292,  0x44302e52,  0x44302a12,  0x442829d2,  0x33282789,  0x33202349,  0x33201ec9,
	//64, 	0x44383692,  0x44383292,  0x44302e52,  0x44302a12,  0x442829d2,  0x33282789,  0x33202349,  0x33201ec9,
	//128, 0x44385e92,  0x44385692,  0x44305252,  0x44304a12,  0x442845d2,  0x33283f89,  0x33203b49,  0x332032c9,
								};
								
#define DDR2_DTR_TAB_ROW  2 //20140521 : Realtek update 256MB DDR2's DTR parameter
#define DDR2_DTR_TAB_COL   (17)

const unsigned int DDR2_DTR_TAB[][DDR2_DTR_TAB_COL]={	
        0,		775,            725,         675,         625,         575,         525,         475,         425,         387,        	362,        	 337,        	 312,       	  287,      	   262,         237,         212,
	//16,  	0x778876a5,  0x77806ea5,  0x77786665,  0x66705e24,  0x666055e4,  0x55584f9b,  0x5550475b,  0x55483edb,  0x44403a9a,  0x4440369a,  0x4438325a,  0x44382e1a,  0x443029da,  0x33282791,  0x33282351,  0x33201ed1,
	//32,  	0x7788769d,  0x77806e9d,  0x7778665d,  0x66705e1c,  0x666055dc,  0x55584f93,  0x55504753,  0x55483ed3,  0x44403a92,  0x44403692,  0x44383252,  0x44382e12,  0x443029d2,  0x33282789,  0x33282349,  0x33201ec9,
	64,  	0x7788a29d,  0x77809a9d,  0x77788e5d,  0x6670821c,  0x666079dc,  0x55586f93,  0x55506353,  0x55485ad3,  0x44405292,  0x44404e92,  0x44384652,  0x44384212,  0x44303dd2,  0x33283789,  0x33283349,  0x33202ec9,
	//128,    0x7788c69d,  0x7780b69d,  0x7778aa5d,  0x66709e1c,  0x666091dc,  0x55588793,  0x55507b53,  0x55486ad3,  0x44406292,  0x44405a92,  0x44385652,  0x44384e12,  0x443049d2,  0x33284389,  0x33283f49,  0x332036c9,
	//256,    0x778936dd,  0x77811edd,  0x77790e9d,  0x6670f65c,  0x6660e61c,  0x5558cf93,  0x5550bf93,  0x5548a713,  0x44409a92,  0x44408ed2,  0x44388692,  0x44387a52,  0x44307212,  0x332867c9,  0x33285f89,  0x33205309,//20140521 : Realtek update 256MB DDR2's DTR parameter
	//512,  0x7779fa9d,  0x7771da9d,  0x7769ba5d,  0x66619a1c,  0x665975dc,  0x55515793,  0x55493753,  0x554116d3,  0x4438fe92,  0x4438ee92,  0x4430de52,  0x4430ce12,  0x4428bdd2,  0x3328ab89,  0x33209b49,  0x33208ac9,

									};

#else
#ifdef CONFIG_SDRAM

#define SDR_DTR_TAB_ROW  6
#define SDR_DTR_TAB_COL  6

const unsigned int SDR_DTR_TAB[][SDR_DTR_TAB_COL]={  
		0,		312,			287,			262,			237,			212,
		2,		0x22302619,	0x223021d9,	0x22281F91,  0x22201F51,	0x11201ad1,
		8,		0x22302619,	0x223021d9,	0x22281F91,  0x22201F51,	0x11201ad1,
		16,		0x22302619,	0x223021d9,	0x22281F91,  0x22201F51,	0x11201ad1,
		32,		0x22302611,	0x223021d1,	0x22281F89,	0x22201F49,	0x11201ac9,
		64,		0x22302611,	0x223021d1,	0x22281F89,	0x22201F49,	0x11201ac9,
							};


#endif
//==========================================

#ifdef CONFIG_DDR1_SDRAM
#define DDR1_DTR_TAB_ROW  5
#define DDR1_DTR_TAB_COL   (9)

const unsigned int DDR1_DTR_TAB[][DDR1_DTR_TAB_COL]={		
      	0,      387,         	362,         	337,        	 312,        	 287,         	262,         	237,         	212,
	16, 	0x4438369a,  0x4438329a,  0x44302e5a,  0x44302a1a,  0x442829da,  0x33282791,  0x33202351,  0x33201ed1,
	32, 	0x44383692,  0x44383292,  0x44302e52,  0x44302a12,  0x442829d2,  0x33282789,  0x33202349,  0x33201ec9,
	64, 	0x44383692,  0x44383292,  0x44302e52,  0x44302a12,  0x442829d2,  0x33282789,  0x33202349,  0x33201ec9,
	128, 0x44385e92,  0x44385692,  0x44305252,  0x44304a12,  0x442845d2,  0x33283f89,  0x33203b49,  0x332032c9,
								};

#endif
//==========================================

#ifdef CONFIG_DDR2_SDRAM

#define DDR2_DTR_TAB_ROW  6 //20140521 : Realtek update 256MB DDR2's DTR parameter
#define DDR2_DTR_TAB_COL   (17)

const unsigned int DDR2_DTR_TAB[][DDR2_DTR_TAB_COL]={	
        0,		775,         725,         	675,         	625,         	575,         	525,         	475,         	425,         	387,        	 362,        	 337,        	 312,       	  287,      	   262,         237,         212,
	16,  	0x778876a5,  0x77806ea5,  0x77786665,  0x66705e24,  0x666055e4,  0x55584f9b,  0x5550475b,  0x55483edb,  0x44403a9a,  0x4440369a,  0x4438325a,  0x44382e1a,  0x443029da,  0x33282791,  0x33282351,  0x33201ed1,
	32,  	0x7788769d,  0x77806e9d,  0x7778665d,  0x66705e1c,  0x666055dc,  0x55584f93,  0x55504753,  0x55483ed3,  0x44403a92,  0x44403692,  0x44383252,  0x44382e12,  0x443029d2,  0x33282789,  0x33282349,  0x33201ec9,
	64,  	0x7788a29d,  0x77809a9d,  0x77788e5d,  0x6670821c,  0x666079dc,  0x55586f93,  0x55506353,  0x55485ad3,  0x44405292,  0x44404e92,  0x44384652,  0x44384212,  0x44303dd2,  0x33283789,  0x33283349,  0x33202ec9,
	128, 0x7788c69d,  0x7780b69d,  0x7778aa5d,  0x66709e1c,  0x666091dc,  0x55588793,  0x55507b53,  0x55486ad3,  0x44406292,  0x44405a92,  0x44385652,  0x44384e12,  0x443049d2,  0x33284389,  0x33283f49,  0x332036c9,
	256,  0x778936dd,  0x77811edd,  0x77790e9d,  0x6670f65c,  0x6660e61c,  0x5558cf93,  0x5550bf93,  0x5548a713,  0x44409a92,  0x44408ed2,  0x44388692,  0x44387a52,  0x44307212,  0x332867c9,  0x33285f89,  0x33205309,//20140521 : Realtek update 256MB DDR2's DTR parameter
	//512,  0x7779fa9d,  0x7771da9d,  0x7769ba5d,  0x66619a1c,  0x665975dc,  0x55515793,  0x55493753,  0x554116d3,  0x4438fe92,  0x4438ee92,  0x4430de52,  0x4430ce12,  0x4428bdd2,  0x3328ab89,  0x33209b49,  0x33208ac9,

									};
#endif
//==========================================
#endif


//--------------------------------------------------
/*
	bootsel   SDR DDR1 DDR2
	 1 0 0     16   16      16
	 1 0 1     32    32      32
	 1 1 0     8      64      64
	 1 1 1     64    128    128	
*/







//==========================================================================
//Use this c code, be careful.
void LookUp_MemTimingTable(unsigned int dramtype, unsigned int dramsize, unsigned int m2xclk)
{

	unsigned int dcr,dtr,i,ext=0;
	unsigned int row=0,col=0;
#ifdef CONFIG_MCM_AUTO
	if(dramtype==DRAM_SDR)  
	{	
		for(i=1; i<SDR_DTR_TAB_ROW; i++)
		{		if(SDR_DTR_TAB[i][0]==dramsize)
					row=i;
		}	
		for(i=1; i<SDR_DTR_TAB_COL; i++)
		{		if(SDR_DTR_TAB[0][i]==m2xclk)
					col=i;
		}		
		dtr=SDR_DTR_TAB[row][col];
	}
	if(dramtype==DRAM_DDR1)  
	{
		for(i=1; i<DDR1_DTR_TAB_ROW; i++)
		{		if(DDR1_DTR_TAB[i][0]==dramsize)
					row=i;
		}	
		for(i=1; i<DDR1_DTR_TAB_COL; i++)
		{		if(DDR1_DTR_TAB[0][i]==m2xclk)
					col=i;
		}		
		dtr=DDR1_DTR_TAB[row][col];
	}
	if(dramtype==DRAM_DDR2)  
	{
		for(i=1; i<DDR2_DTR_TAB_ROW; i++)
		{		if(DDR2_DTR_TAB[i][0]==dramsize)
					row=i;
		}	
		for(i=1; i<DDR2_DTR_TAB_COL; i++)
		{		if(DDR2_DTR_TAB[0][i]==m2xclk)
					col=i;
		}		
		dtr=DDR2_DTR_TAB[row][col];
	}
#else
#ifdef CONFIG_SDRAM
	if(dramtype==DRAM_SDR)  
	{	
		for(i=1; i<SDR_DTR_TAB_ROW; i++)
		{		if(SDR_DTR_TAB[i][0]==dramsize)
					row=i;
		}	
		for(i=1; i<SDR_DTR_TAB_COL; i++)
		{		if(SDR_DTR_TAB[0][i]==m2xclk)
					col=i;
		}		
		dtr=SDR_DTR_TAB[row][col];
	}
#endif


#ifdef CONFIG_DDR1_SDRAM
	if(dramtype==DRAM_DDR1)  
	{
		for(i=1; i<DDR1_DTR_TAB_ROW; i++)
		{		if(DDR1_DTR_TAB[i][0]==dramsize)
					row=i;
		}	
		for(i=1; i<DDR1_DTR_TAB_COL; i++)
		{		if(DDR1_DTR_TAB[0][i]==m2xclk)
					col=i;
		}		
		dtr=DDR1_DTR_TAB[row][col];
	}
#endif

#ifdef CONFIG_DDR2_SDRAM

	if(dramtype==DRAM_DDR2)  
	{
		for(i=1; i<DDR2_DTR_TAB_ROW; i++)
		{		if(DDR2_DTR_TAB[i][0]==dramsize)
					row=i;
		}	
		for(i=1; i<DDR2_DTR_TAB_COL; i++)
		{		if(DDR2_DTR_TAB[0][i]==m2xclk)
					col=i;
		}		
		dtr=DDR2_DTR_TAB[row][col];
	}	
#endif


#endif
	REG32(DTR_REG)=dtr;			//ASIC



}
//==============================================================
#if 1
void LookUp_MemSizeTable(unsigned int dramtype, unsigned int dramsize, unsigned int m2xclk)
{

	unsigned int dcr,dtr,i,ext=0;
	unsigned int row=0,col=0;

	//all type.
		for(i=0; i<SDR_DCR_TAB_ROW; i++)
		{		if(SDR_DCR_TAB[i][0]==dramsize)
					col=i;
		}

		dcr=SDR_DCR_TAB[col][1];		

	#ifdef CONFIG_DDR2_SDRAM
	if( (dramtype==DRAM_DDR2)  && (dramsize>=128) )
	{		
		REG32(0xB800100c)|=(1<<30);   //change 4 Bank -> 8 Bank for DDR2 128MB
	}
	#endif

	#ifdef CONFIG_DDR1_SDRAM
	if( (dramtype==DRAM_DDR1)  && (dramsize>=128) )
	{	
		//DDR1 128MB still 4 Bank but different ROW(RA0~RA13)/COL(CA0~CA9) 
		REG32(0xB8001004)=0x15a20000;   
	}	
	#endif
	//--------------------------------------


	REG32(DCR_REG)=dcr;
        CHECK_DCR_READY();
}
#endif


unsigned int Calc_Dram_Size(unsigned int dramtype)
{
	//printf("Calc_Dram_DCR\n");	


	#define ROW_MIN 11
	#define ROW_MAX 14
	#define COL_MIN 8
	#define COL_MAX 12

	unsigned int cs1=0;
	//power 2
	unsigned int width=1;  //fix 16 bit
	unsigned int bank; 
	unsigned int row; 
	unsigned int col; 	
	unsigned int slow_rx_bit=0; 
	unsigned int CL_value=0; 

	//unsigned int DCR_VALUE = (1<<28) | (width<<26);  //CL3

	unsigned int DCR_VALUE = (width<<26);  //CL3
	

	if (REG32(DCR)&(1<<15)) //JSW patch :for slow_rx setting
	{
		DCR_VALUE|=1<<15;
		slow_rx_bit=1;
	}

	CL_value=REG32(DCR) & 0xf0000000;

	DCR_VALUE|=CL_value;

	
	#define DCR 0xb8001004
	#define EDTCR 0xb800100c
	#define WRITE_DATA 0x12345678

#if DBG
	printf("\n=== DRAM Auto-Size info ===\n");	
	printf("DCR(0x%x)=0x%x\n", DCR,REG32(DCR));	
	printf("width=%d,", width);
	printf("DF DCR=0x%x,",DCR_VALUE);
	printf("DF CAS=0x%x,", CL_value);	
#endif

	//------------------------------------	
	//find bank
	REG32(DCR)=DCR_VALUE | (1<<17);  //[0]:2 bank,  [1]: 4 bank,  set max BANK=1(4 bank)
	CHECK_DCR_READY();
	
	//W1*R11*C8=1M bytes	
	unsigned int *t1=0xa0000000 | (1<<(width+19));
	unsigned int *t3=0xa0000000 | (3<<(width+19));	
	unsigned int *t9=0xa0000000 | (9<<(width+19));

	//if(dramtype==0)  //SDR
	{
		REG32(t3)=0;
		REG32(t1)=0;
	
		REG32(t3)=WRITE_DATA;
	
		if(REG32(t1)==REG32(t3))
		{	//DCR_VALUE|=(0<<19)	;
			bank=1;  // 2 bank
		}
		else
		{	DCR_VALUE|=(1<<19)	;
			bank=2;  // 4 bank
		}
	}
	
	if( (bank==2)  && ((dramtype==1) ||(dramtype==2)) )  //DDR1 DDR2
	//if( (dramtype==1) ||(dramtype==2) )  //DDR1 DDR2	
	//else
	{
			REG32(EDTCR)&=~(3<<30);
			REG32(EDTCR)|= (1<<30);  //two bit, [00]=4 bank  [01]=8 bank
			
			unsigned int tmp=REG32(DCR);
			REG32(DCR)=tmp | (1<<20);   //set COL=1;  addr*=2
			
			REG32(t3)=0;
			REG32(t9)=0;				
			REG32(t1)=0;		
			
			REG32(t9)=WRITE_DATA;
			
			if(REG32(t1)==REG32(t9))
			{	
				bank=2;  // 4 bank, 4M boundary, but double
			}
			else
			{	bank=3;
			}
			REG32(EDTCR)&=~(3<<30);
			REG32(DCR)=tmp;   //set COL=0;

			
	}
#if DBG
	printf("bank=%d,", bank);	
#endif
	//--------------------------------------------------------
	//find row
	REG32(DCR)=DCR_VALUE|(3<<23) | (0<<20);      //set max ROW=3(16K), COL=0(256)
	CHECK_DCR_READY();	
	unsigned int i=0;

	for(row=ROW_MIN; row<=ROW_MAX; row++)
	{
		unsigned int *t7=0xa0000000 | (1<<(width + row + 7));
		REG32(t7)=0;
		REG32(0xa0000000)=WRITE_DATA;
		//printf("  %x=%x\n", t7, REG32(t7));
		if(WRITE_DATA==REG32(t7)) 
			break;	
	}
	row--;
#if DBG
	printf("row=%d,", row);	
#endif	
	//--------------------------------------------------------
	//find col
	REG32(DCR)=DCR_VALUE|(0<<23) | (4<<20);      //ROW=0(2K), set max COL=4(4K)
	CHECK_DCR_READY();

	for(col=COL_MIN; col<=COL_MAX; col++)
	{
		unsigned int *t7=0xa0000000 | (1<<(width + col - 1));
		REG32(t7)=0;
		REG32(0xa0000000)=WRITE_DATA;
		//printf("  %x=%x\n", t7, REG32(t7));
		if(WRITE_DATA==REG32(t7)) 
			break;
	}
	col--;
#if DBG
	printf("col=%d,", col);	
#endif	
	//--------------------------------------------------------
#if 1 //for chip select *2 
#if 0 //for RTL8197D before
	//find chip select
	REG32(DCR)= (1<<30) |  //CL3
				(width<<28)  |  
				(1<<27)|         // set max chip count
				((row-11)<<25) | 
				((col-8)<<22) |
				(((bank<2) ? 0 : 1)  <<19);   // [0] : 2bank, [1]: 4bank
#else //for RTL8881A
	//find chip select	

	REG32(DCR)= (CL_value) |  //CL:JSW add
				(width<<26)  |  
				(1<<25)|         // set max chip count
				((row-11)<<23) | 
				((col-8)<<20) |
				(slow_rx_bit<<15) | //JSW add
				(((bank<2) ? 0 : 1)  <<17);   // [0] : 2bank, [1]: 4bank
#endif

	CHECK_DCR_READY();


		unsigned int *t7=0xa0000000 | (1<<(width+bank+row+col) );
		//printf("t7=%x\n", t7 );
		REG32(t7)=WRITE_DATA;
		REG32(0xa0000000)=0;
		REG32(0xa0000000);
		if(WRITE_DATA==REG32(t7)) 
		{ // DRAM  * 2
			cs1=0;

			REG32(DCR)	&= ~(1<<25); //Set Chip select only cs0 , disable cs1
		}
		else
		{ // DRAM  * 1
			REG32(DCR)	&= ~(1<<25); //Set Chip select only cs0 , disable cs1
		}
	
#if DBG
	printf("cs=%d,", cs1);	
#endif	
#endif
	//--------------------------------------------------------



	//printf("=>line=%d\n", width+bank+row+col);	
	unsigned int size=1<<(width+bank+row+col-20);

#if DBG	
	printf("size=%d MBytes x %d\n", size, cs1+1);
#endif	


	if(bank==3)			
	{	REG32(EDTCR)&=~(3<<30);
		REG32(EDTCR)|= (1<<30);  // 8 bank
	}

#if 0
	REG32(DCR)= (1<<28) |  //CL3
				(width<<26)  |  
				(cs1<<25)|    				
				((row-11)<<23) | 
				((col-8)<<20) |
				(((bank<2) ? 0 : 1)  <<17);   // [0] : 2bank, [1]: 4bank
#endif

#if DBG
	printf("DCR=%x\n",REG32(DCR));
	printf("\n=== end ===\n\n");	
#endif	

	
	return size;	

}




//==============================================================

#ifdef CONFIG_SHOW_TRXDLY_MAP

void ShowTxRxDelayMap(dramtype,m1xclk,SLOW_RX,CMD_EDGE,CLK_M_N_DLY)
{
 	int register tx,rx;
	#define REG32_ANDOR(x,y,z)  { REG32(x)=( REG32(x) & (y) ) | (z);  }
	
	//REG32(DTR)=0x666079dc;//
	//REG32(DCR)=0x15220000;

	
	//SLOW_RX setting
	REG32(DCR)&=~(1<<15);
	REG32(DCR) = REG32(DCR)|( (SLOW_RX)<<15); 		//Enable SLOW_RX

	
	//CMD_EDGE setting
	REG32(DSTR)&=~(1<<31);
	REG32(DSTR) = REG32(DSTR)|( (CMD_EDGE)<<31); 	

	//CLK_M_N_DLY setting
	REG32(DSTR)&=~(3<<24);
	REG32(DSTR) = REG32(DSTR)|( (CLK_M_N_DLY)<<24); 	
	
	//printf("\nBefore tunning , DCR(0x%x)=0x%x\n", DCR_REG,REG32(DCR_REG));
	//printf("\nDSTR(0x%x)=0x%x\n",DSTR,REG32(DSTR));
	 if((dramtype==DRAM_DDR1)||(dramtype==DRAM_DDR2)) 
	 {	
	 	/**/
		//REG32(0xb8001008)=0x66705e1c; //use safe DTR parameter to calibration
		printf("CLK_M:  ");	
		for (rx =0; rx<= 31; rx++)
		//for (rx =31; rx>= 0; rx--)
			printf("%02x ",rx);	

		printf("\n");	
	 }

	 	int loop;
		for (tx =0; tx<=31; tx++)
		{
					printf("Tx=%02x : ",tx );	
		for (rx =0; rx<= 31; rx++)	
		//for (rx =31; rx>= 0; rx--)
		{
			REG32(DCR)=REG32(DCR);
			CHECK_DCR_READY();		
		
			
			
			//printf("c0=(%d,%d), p=(%d,%d)\n", c0[i][0],c0[i][1], tx,rx);

			REG32_ANDOR(CLK_MANAGER,	~( (0x1f<<5) | (0x1f<<0) ),   (tx<<5) | (rx<<0) );

			 if((dramtype==DRAM_DDR1)||(dramtype==DRAM_DDR2)) 
			 {
				REG32(HS0_CONTROL)=( (rx<<10)|(rx<<5));  
			 }

			#if 1
				DDR_cali_API7();
				#if CONFIG_Calibration_DQS_EN_TAP
					DDR_cali_API8();	
				#endif
			#endif
			
			REG32(ADDR)=PATT0;		if(REG32(ADDR)!=PATT0)			goto failc0;
			REG32(ADDR)=PATT1;		if(REG32(ADDR)!=PATT1)			goto failc0;
			REG32(ADDR)=PATT2;		if(REG32(ADDR)!=PATT2)			goto failc0;
			REG32(ADDR)=PATT3;		if(REG32(ADDR)!=PATT3)			goto failc0;
			REG32(ADDR)=PATT4;		if(REG32(ADDR)!=PATT4)			goto failc0;

			printf("%02x,",  rx);			
			continue;
			//delay_ms(1);
			while(loop--);
failc0:
			printf("--," );			
			continue;				

		
		}
		printf("\n");
		}
	printf("\n");	

		#if DBG
		printf("\n= Mem Phase info =\n");	
		printf("SLOW_RX=%d \n",SLOW_RX);	
		printf("CMD_EDGE=%d \n",CMD_EDGE);	
		printf("CLK_M_N_DLY=%d \n",CLK_M_N_DLY);	
		printf("0.DSTR(0x%x)=0x%x\n",DSTR,REG32(DSTR));
		printf("0.DRAM clock =%d \n",m1xclk );				
		printf("=  end  =\n");	
		#endif
}

#endif
//==============================================================


void EnableIP_PADControl(unsigned int dramtype)
{
	
	
	//for RTL8881A 

	if(dramtype==DRAM_DDR1)
	{
		REG32(DLL_REG )&=0xC7FFFFFF;//Set [29:27]=0
		
		REG32(DLL_REG )|=(7<<27);//Set DDR1 LDO output=2V5
	}
	
	if(dramtype==DRAM_DDR2)
	{

		//Set Vcore from 1V26->1V3
		REG32(SWR_LDO1_REG )&=0xFFFFFFF0;//Set [3:0]=0
		//REG32(SWR_LDO1_REG )|=0x8;//Set [3:0]=8 //1V3
		REG32(SWR_LDO1_REG )|=(0x6|1<<16);//Set [3:0]=9 //1V33 set [17:16} as 11 for 3.3uH
		
		REG32(DLL_REG )&=0xC7FFFFFF;//Set [29:27]=0
		//REG32(DLL_REG )|=(6<<27);//Set DDR2 LDO output=1V88
		REG32(DLL_REG )|=(7<<27);//Set DDR2 LDO output=1V88

		//+rp150 Ohm
		//REG32(PAD_CONTROL3 )=0x3FFFF660;//20121003:2V1 337MHZ ,better

		//REG32(PAD_CONTROL2 )|=(1<<16);//Enable DQ TE
		//REG32(PAD_CONTROL2 )|=(1<<27);//Enable DQ TE
		
		//REG32(PAD_CONTROL3 )=0x333FF660;//20121003:2V1
		REG32(PAD_CONTROL3 )=0x7FFFF660; //Enable DQS TE		
		
		REG32(PAD_CONTROL4 )=0x17FFFEFF;;//Enable DQS TE		
		
#ifdef CONFIG_SW_8367R
		// Enable DQT/DQS, Imax
		REG32(PAD_CONTROL2 )|= ((1<<16) | (1<<27)); //Enable DQ TE
		
		// Enable DQS TE only 
		REG32(PAD_CONTROL3 )=0x7FFFF660; //Enable DQS TE						
		REG32(PAD_CONTROL4 )=0x17FFFEFF;;//Enable DQS TE		
#endif

#if 0
		// Enable DQT/DQS, Imin
		REG32(PAD_CONTROL2 )= 0x5FCBFBDF;
		
		// Enable DQS TE and tuning UDQS/LDQS VIX from 780mv to 810mv 
		REG32(PAD_CONTROL3 )=0x7BFFF660; //Enable DQS TE						
		REG32(PAD_CONTROL4 )=0x17FFBEFF;;//Enable DQS TE		
#endif
	}
}

#if CONFIG_DRAM_MODE_REGISTER
void DRAM_ModeRegister(unsigned int dramtype)
{
	//unsigned int RTT_value=RTT_50ohm;
	//unsigned int RTT_value=RTT_0ohm;
	unsigned int MR_Data;

	/*for RTL8881A DDR2 only*/
	if(dramtype==DRAM_DDR2)
	{				
			/* 1. Disable DLL */
		MR_Data= MR_MODE(1)|DLL_DISABLE ; //EMR1
		REG32(EDTCR_REG)&=0xffff0000;//clear last MR_Data
		REG32(EDTCR_REG)|=MR_Data;
		CHECK_DCR_READY();	
		REG32(DCR_REG) |=  MR_MODE_EN;	
		CHECK_DCR_READY();	
		REG32(DCR_REG) &=  ~MR_MODE_EN;	
		
		
			/* 2. Enable DLL */
		MR_Data= MR_MODE(1) ; //EMR1
		REG32(EDTCR_REG)&=0xffff0000;//clear last MR_Data
		REG32(EDTCR_REG)|=MR_Data;
		CHECK_DCR_READY();	
		REG32(DCR_REG) |=  MR_MODE_EN;	
		CHECK_DCR_READY();	
		REG32(DCR_REG) &=  ~MR_MODE_EN;		


		/* 3. Reset DLL */
		//MR_Data= MR_MODE(0) |DDR2_MR_BURST_4|DDR2_MR_CAS_6|DDR2_MR_WR_6| DDR1_MR_OP_RST_DLL ; //MR
		MR_Data= MR_MODE(0) | DDR1_MR_OP_RST_DLL ; //MR
		REG32(EDTCR_REG)&=0xffff0000;//clear last MR_Data
		REG32(EDTCR_REG)|=MR_Data;
		CHECK_DCR_READY();	
		REG32(DCR_REG) |=  MR_MODE_EN;	
		CHECK_DCR_READY();	
		REG32(DCR_REG) &=  ~MR_MODE_EN;			
	
		
		/* 4. Waiting 200 clock cycles */
		unsigned int delay_time = 200;
		while(delay_time--);


		/* 5. Normal mode, avoid to reset DLL when updating phy params */
			/*DQS Single END*/

		//unsigned int RTT_value=RTT_0ohm;//Tx range:0x14~0x1f [312MHZ] , Tx range:0x14~0x1f [337MHZ] 
		//unsigned int RTT_value=RTT_50ohm;//Tx range:0x14~0x1f [312MHZ] , Tx range:0x14~0x1f [337MHZ] 
		unsigned int RTT_value=RTT_75ohm;//Tx range:0x14~0x1f [312MHZ] ,  Tx range:0x14~0x1f [337MHZ] 
		//unsigned int RTT_value=RTT_150ohm;//Tx range:0x14~0x1f [312MHZ] , Tx range:0x14~0x1f [337MHZ] 


		#define DQS_SINGLE_ENDED 0

		/*DQS Single-end for DRAM*/
		#if DQS_SINGLE_ENDED
			MR_Data= MR_MODE(1) |RTT_value|OCD_DEFAULT|DQS_DISABLE; 
		#else		
			MR_Data= MR_MODE(1) |RTT_value|OCD_DEFAULT; 
			//MR_Data= MR_MODE(1) |RTT_value|OCD_DEFAULT |DLL_DISABLE; 
			//MR_Data= MR_MODE(1) |RTT_value|OCD_DEFAULT |ODIT_60percent; 
		#endif
		
		REG32(EDTCR_REG)&=0xffff0000;//clear last MR_Data
		REG32(EDTCR_REG)|=MR_Data;
		CHECK_DCR_READY();	
		REG32(DCR_REG) |=  MR_MODE_EN;	
		//REG32(DCR_REG) |=  (MR_MODE_EN|(1<<28)) ;
		
		CHECK_DCR_READY();	
		REG32(DCR_REG) &=  ~MR_MODE_EN;	
		
		//printf("\nMR_Data=0x%x\r\n",MR_Data);

		#if 0
		/*DQS Single-end for DRAM*/
		#if DQS_SINGLE_ENDED
			printf("\=>Set DQS_SINGLE_ENDED\r\n");
			REG32(PAD_CONTROL4) |=(1<<28);//pad SE
			printf("\n=>PAD_CONTROL4(0x%x)=0x%x>\r\n",PAD_CONTROL4,REG32(PAD_CONTROL4));
			printf("\n=>EMR1_REG(0x%x)=0x%x\r\n",EMR1_REG,REG32(EMR1_REG));
		#endif
		
		
		
		/*Check RTT command*/	
		if(REG32(EMR1_REG)&RTT_value)
		{
			printf("\n=>EMR1_test <RTT Pass >\r\n");
		}
		else
		{
			if((REG32(EMR1_REG)&RTT_value)==0)
				printf("\n=>EMR1_test <RTT 0ohm Pass >\r\n");
			else
				printf("\n=>EMR1_test <RTT Fail >\r\n");
		}		
		#endif
		
		
		
	}
}


#endif


//==============================================================


#define putc(x)	Put_UartData(x)
#define getc_timeout  Get_UartData_timeout


//==============================================================
	const unsigned char *boot_type_tab[]={ "SPI", "ROM1", "NFBI", "NAND", "ROM01", "ROM02", "ROM03", "ROM04" };
	const unsigned char *dram_type_tab[]={ "SDR" ,"DDR1", "DDR2" };
	const unsigned int m2x_clksel_table[]={ 212,	237,	 262, 287, 312,  337,  362,  387,  425,  475, 525, 575, 625,  675,  725,  775, 	};

	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf		
	
	


//------------------------------------------------------------------
/*Important DRAM parameter*/
#define CLK_M_90_default 25 //For write DQ[15:0] delay
#define CLK_M_default 28 	     //For command/address delay


#define RXCLK_DLY_default 8  //For SDR Rx delay
#define TXCLK_DLY_default 24	     //For DDR Tx delay , when 312MHZ ,tQDSS <=800ps



/* Ideal from TFTP*/
/*CLK_M=28 ,TXCLK_DLY=24 that 362MHZ can TFTP
    20121019:
    1.From timing observation: TXCLK_DLY=CLK_M_default+(>=3)
    	 //362MHZ :   TXCLK_DLY(30) =CLK_M_default(27)+(>=3)

    2. <=262MHZ :   TXCLK_DLY(26) =CLK_M_default(28)
*/
/* tDQSS issue 
tDQSS MIN = กV0.25 กั tCK
tDQSS MAX = +0.25 กั tCK

ex:262MHZ =3800 ps/4 = 950 ps
*/


//------------------------------------------------------------------
void start_c()
{
	//printf("start_c()\n");
	//printf("\n#SFCR(0x%x) =0x%x\n",SFCR,REG32(SFCR));
	//-----------------------------------------------------------------	
		
	//printf("Strap=%x\n",v);

	//-----------
	//REG32(0xb8000058)=0x2015E; //For KsChung ,Set RLL pin to output m2x freq
	
	//printf("\n#(0xb8000058) =0x%x\n",,REG32(SFCR));
	unsigned int v=REG32(SYS_HW_STRAP);
	unsigned char boot_sel=GET_BITVAL(v, 0, RANG3);
	unsigned char dramtype_sel=GET_BITVAL(v, 3, RANG2);
	unsigned char m2x_freq_sel=GET_BITVAL(v, 11, RANG4);  //bit [14:10], but bit 10 is not strap pin	
	unsigned int m1xclk=m2x_clksel_table[m2x_freq_sel]/2;
     
	
	//unsigned char ever_reboot_once_freq=0;
	unsigned int SYS_HW_STRAP_value;

	unsigned char rom_size_sel;
	unsigned int dramsize;

	//-----------
	unsigned char dram_type_remap[]={	DRAM_SDR,   // 00 -> SDR
										DRAM_SDR,   // 01 -> SDR
										DRAM_DDR2,   // 10 ->DDR2
										DRAM_DDR1 // 11-> DDR1
									};

	unsigned char dramtype=dram_type_remap[dramtype_sel];
		
	unsigned int m2xclk=m2x_clksel_table[m2x_freq_sel];
	

	  //------------------------------------------
        /*Show board info*/
	//printf("Mode=%s\n",boot_type_tab[boot_sel]);
	//printf("RAM=%s\n", dram_type_tab[dramtype]);
	//printf("M1XCLK =%d\n",m1xclk);

	 //------------------------------------------
        /*Set-1.Set DRAM driving , LDO , pad phase delay*/     
	EnableIP_PADControl(dramtype);	
	


	#if 1
		/*Set-8.For SDR/DDR1/DDR2 Auto CAS Latency setting by current Freq*/				
		{			
			REG32(DCR)=REG32(DCR)& ~(0xf<<28);
			CHECK_DCR_READY();
			
			if(m1xclk<=200)			REG32(DCR) = REG32(DCR)| (1)<<28; 	//0001:  CL3			
			else if(m1xclk<=312)	REG32(DCR) = REG32(DCR)| (3)<<28; 		//0011:  CL4
			else if(m1xclk<=337)	REG32(DCR) = REG32(DCR)| (4)<<28; 		//0100:  CL5
			else if(m1xclk<=400)	REG32(DCR) = REG32(DCR)| (5)<<28; 		//0101:  CL6
			else REG32(DCR) = REG32(DCR)| (6)<<28; //0110:  CL7					//(m2xclk>400)		
		}		
		
	#endif	

	 //------------------------------------------
	
	/*DRAM phase fine-tune*/
	#if 0	
		

	unsigned int SLOW_RX=0,CMD_EDGE=0,CLK_M_N_DLY=0;

	

	#if 1
		unsigned int LDQS_O_delay=0,UDQS_O_delay=0;
		unsigned int LDQM_O_delay=0,UDQM_O_delay=0;
		
		REG32(DSDR1) = (LDQS_O_delay<<0) |(UDQS_O_delay<<4)|(LDQM_O_delay<<8)|(UDQM_O_delay<<12); 	
	
	#endif
	
	#if 1
	
	
	#define LDQ_O_delay 7
	#define DQ6_O_delay 0 	

	#define UDQ_O_delay 7
	#define DQ8_O_delay 0 		
				
		REG32(DSDR2) = (DQ8_O_delay<<0) |(UDQ_O_delay<<4)|(UDQ_O_delay<<8)|(UDQ_O_delay<<12)\
						|(UDQ_O_delay<<16)|(UDQ_O_delay<<20)|(UDQ_O_delay<<24)|(UDQ_O_delay<<28); 

		REG32(DSDR3) = (LDQ_O_delay<<0) |(LDQ_O_delay<<4)|(LDQ_O_delay<<8)|(LDQ_O_delay<<12)\
						|(LDQ_O_delay<<16)|(LDQ_O_delay<<20)|(DQ6_O_delay<<24)|(LDQ_O_delay<<28);	
					
	#endif	

				/*Modify DDR2 DRAM's RTT*/
	#if CONFIG_DRAM_MODE_REGISTER
		DRAM_ModeRegister(dramtype);
	#endif
	
	
	#endif	

	
	    /*Set-3.Adjust IC Internal DDR clock phase and TRx delay*/	
		CLK_M_Adjust(dramtype,m1xclk);	

		if((dramtype==DRAM_DDR1)||(dramtype==DRAM_DDR2)) //JSW:For 8881A DDR2 temporary
		 { 	
			DDR_cali_API7();
			#if CONFIG_Calibration_DQS_EN_TAP
				DDR_cali_API8();	
			#endif
		}
	
 	 //------------------------------------------
	
		/*Set-.Auto calculate DRAM size*/
		dramsize=Calc_Dram_Size(dramtype);
	
	 //------------------------------------------
		/*Set-.Auto set DRAM DTR by size and Freq*/			
		LookUp_MemTimingTable(dramtype, dramsize, m2xclk);
		
		 REG32(DCR)=REG32(DCR);     

}


//==============================================================


void CLK_M_Adjust(unsigned int dramtype,unsigned int m1xclk)
{
	

	/*For tDQSS over Spec :
		If RX path turnaround delay is too large, memory controller can return 
		read data with increased latency by 1 memory clock cycle.

		REG32(DCR)|=~(1<<15); //disable slow_rx
	*/

	/*Reset to default value*/
	REG32(DCR)&=~(1<<15); //disable slow_rx
	REG32(DSTR)=0x83000000; 
	
	 if((dramtype==DRAM_SDR)||(dramtype==DRAM_DDR1)) //JSW:For 8881A SDR temporary	
	 {
	 	/*Verified OK*/	
		//REG32(HS0_CONTROL)|=( (CLK_M_90_default<<10)|(CLK_M_default<<5)); 	
		//REG32(CLK_MANAGER)=( (TXCLK_DLY_default<<5)|(RXCLK_DLY_default<<0));	 
		REG32(HS0_CONTROL)=( (25<<10)|(28<<5));  
		REG32(CLK_MANAGER)=( (20<<5)|(RXCLK_DLY_default<<0));	
 
	}	
	 else if((dramtype==DRAM_DDR2)) //JSW:For 8881A DDR2 temporary
	 { 	
			/*Modify DDR2 DRAM's RTT*/
	#if CONFIG_DRAM_MODE_REGISTER
		DRAM_ModeRegister(dramtype);
	#endif
	
		
		#if 1
			
			if(m1xclk<=400)//for DDR2
			{							
				//REG32(HS0_CONTROL)=( (CLK_M_90_default<<10)|(CLK_M_default<<5));  
				REG32(HS0_CONTROL)=( (25<<10)|(28<<5));  
				REG32(CLK_MANAGER)=( (24<<5)|(RXCLK_DLY_default<<0));	
			}		
		
						
		
		#endif
		CHECK_DCR_READY();		
					
	 }
}




