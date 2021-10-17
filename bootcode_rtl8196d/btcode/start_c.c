
#include "start.h"
#include "../autoconf.h"



#define REG32(reg)	(*(volatile unsigned int *)(reg))


#define DBG 1
#define DDR_DBG 1


#define CHECK_DCR_READY()   { while(REG32(DCR) & 1)  {}; }   // 1=busy, 0=ready
//---------------------------------------------------
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
#if UART_WAIT_BREAK	
		i++;		
		if (i >=3210)
			break;
#endif	
		if 	(REG32(UART_LSR) & (1<<24) )
			break;		
	}	
	ch=REG32(UART_RBR);
	ch=ch>>24;
	return ch;
}
//-----------------------------------------------------
unsigned int kbhit(unsigned int loops)
{	int i;
	for(i=0; i<loops; i++)
	{
		if(REG32(UART_LSR) & (1<<24) )
			return 1;
	}
	return 0;
}
//-----------------------------------------------------
#define SIMPLE_PRINT 0
#if SIMPLE_PRINT
void uart_print_msg(const char*p)
{
	while( *p )
	{
		uart_outc(*p);
		p++;
	}
}
//-----------------------------------------------------
void uart_print_hex(unsigned char v)
{
       unsigned char ascii_tab[]= "0123456789abcdef";	  
	uart_outc(ascii_tab[ (v>>4)&0x0f] ); 	 
	uart_outc(ascii_tab[ v&0x0f] );
}
//-----------------------------------------------------
void uart_print_dig(unsigned int v)
{
	unsigned int v1,v2;
	v1=(v/1000)*0x10+((v%1000)/100);
	uart_print_hex(v1);	
	
	v=(v%100);
	v2=(v/10)*0x10+(v%10);	
	uart_print_hex(v2);
}
//-----------------------------------------------------
void uart_print_reg(unsigned int v)
{   
	uart_print_hex( (v>>24)&0xff );
	uart_print_hex( (v>>16)&0xff );
	uart_print_hex( (v>>8) &0xff );
	uart_print_hex( (v>>0) &0xff );	
}

#else
//------------------------------------------------------


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
{	(void)vsprintf(0, fmt, ((const int *)&fmt)+1);	
}
#endif
//--------------------------------------------------

//==========================================

const unsigned int SDR_DTR_TAB[][4]={  
		0,		337,			312,			250,
		2,		0x6ce2a5a0,	0x48c26190,	0x48a1f910,
		8,		0x6ce2a5a0,	0x48c26190,	0x48a1f910,
		16,		0x6ce2a5a0,	0x48c26190,	0x48a1f910,
		32,		0x6ce2a520,	0x48c26110,	0x48a1f890,
		64,		0x6ce2a520,	0x48c26110,	0x48a1f890
							};
#define SDR_DTR_TAB_ROW  6
#define SDR_DTR_TAB_COL  4

//--------------------------------------------------


const unsigned int DDR1_DTR_TAB[][9]={
	0,	475,			462,			425,			387,			362,			337,			312,			250,	
	16,	0x912435B0,	0x912435B0,	0x9103ADB0,	0x6CE369A0,	0x6CE329A0,	0x48c2e5a0,	0x48c521a0,	0x24a21990,
	32,	0x91243530,	0x91243530,	0x9103AD30,	0x6CE36920,	0x6CE32920,	0x48c2e520,	0x48c52120,	0x24a21910,
	64,	0x91243530,	0x91243530,	0x9103AD30,	0x6CE36920,	0x6CE32920,	0x48c2e520,	0x48c52120,	0x24a21910,
	128,	0x91273530,	0x9126F530,	0x91066D30,	0x6CE5E920,	0x6CE56920,	0x48c52520,	0x48c4a120,	0x24a3d910
								};
#define DDR1_DTR_TAB_ROW  5
#define DDR1_DTR_TAB_COL   9

//--------------------------------------------------



const unsigned int DDR2_DTR_TAB[][9]={	
	0,	475,			462,			425,			387,			362,			337,			312,			250,	
	16,	0x914475B0,	0x914475B0,	0x9123EDB0,	0x6D03A9A0,	0x6D0369A0,	0x48c325a0,	0x48c2e1a0,	0x24a25990,
	32,	0x91447530,	0x91447530,	0x9123ED30,	0x6D03A920,	0x6D036920,	0x48c32520,	0x48c2e120,	0x24a25910,
	64,	0x91463530,	0x91463530,	0x9125AD30,	0x6D052920,	0x6D04E920,	0x48c46520,	0x48c42120,	0x24a35910,
	128,	0x9147B530,	0x91477530,	0x9126ED30,	0x6D062920,	0x6D05E920,	0x48c56520,	0x48c52120,	0x24a41910,
	256,	0x914BB530,	0x914B7530,	0x912A6D30,	0x6D09A920,	0x6D08E920,	0x48c86520,	0x48c7e120,	0x24a61910,
	512,	0x91337530,	0x9132F530,	0x91116D30,	0x6CEFE920,	0x6CEEE920,	0x48ce2520,	0x48cd2120,	0x24aa1910,
									};
#define DDR2_DTR_TAB_ROW  5
#define DDR2_DTR_TAB_COL   9

//====================================================================
const unsigned int SDR_DCR_TAB[][2]={
						2,    0x50000000,
						8, 	0x52080000,
						16,	0x52480000,
					  	32, 	0x54480000,
					   	64,	0x54880000,
					  	128, 0x54880000,   // < 200M
					//    128,0xD4880000 	  //  > 200M, need more
					};
#define SDR_DCR_TAB_COL  2
#define SDR_DCR_TAB_ROW  6
//--------------------------------------------------
/*
	bootsel   SDR DDR1 DDR2
	 1 0 0     16   16      16
	 1 0 1     32    32      32
	 1 1 0     8      64      64
	 1 1 1     64    128    128	
*/

#define DRAM_SDR 0
#define DRAM_DDR1 1
#define DRAM_DDR2 2

unsigned char DRAM_SIZE_TAB[4][3]={
								16,	16,	16,
								32,	32,	32,
								8,	64,	64,
								2,	128,	128
								};

//---------------------------------------------------




//==========================================================================
//Use this c code, be careful.
void LookUp_MemTimingTable(unsigned int dramtype, unsigned int dramsize, unsigned int m2xclk)
{

	unsigned int dcr,dtr,i,ext=0;
	unsigned int row=0,col=0;
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

		#if DBG
			printf("SDR DTR=%x\n",dtr);	
			REG32(0xb8001008)=dtr	;		//ASIC		
		#endif
	}
	else if(dramtype==DRAM_DDR1)  
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

		#if DBG
			printf("DDR1 DTR=%x\n",dtr);	
			REG32(0xb8001008)=dtr	;		//ASIC		
		#endif
		
	}
	else if(dramtype==DRAM_DDR2)  
	{


					
		#if 1
			for(i=1; i<DDR2_DTR_TAB_ROW; i++)
			{		if(DDR2_DTR_TAB[i][0]==dramsize)
						row=i;
			}	
			for(i=1; i<DDR2_DTR_TAB_COL; i++)
			{		if(DDR2_DTR_TAB[0][i]==m2xclk)
						col=i;
			}		
			dtr=DDR2_DTR_TAB[row][col];

			#if DBG
				printf("DDR2 DTR=%x\n",dtr);	
				REG32(0xb8001008)=dtr	;		//ASIC		
			#endif
		#else		


		
		
				#if 0
					#ifdef CONFIG_D16_16 //16MB
						REG32(0xb8001008)=0x6d03a9a0;  //DTR
						REG32(0xb8001004)=0x52480000;  //DCR	
					#endif

					#ifdef CONFIG_D32_16 //32MB						
						REG32(0xb8001008)=0x6D03A920;  //DTR
						REG32(0xb8001004)=0x54480000;  //DCR
					#endif
				
					#ifdef CONFIG_D64_16 //64MB
						REG32(0xb8001008)=0x6D052920;  //DTR
						REG32(0xb8001004)=0x54880000;  //DCR
					#endif

					#ifdef CONFIG_D128_16//128MB
						REG32(0xb8001008)=0x6D062920;  //DTR
						REG32(0xb8001004)=0x54880000;  //DCR
						REG32(0xb800100C)=0x78000000;  //EDTCR
					#endif
					
				#endif
				//DDCR
				REG32(0xb8001050)=0xd0800000;  	

				#if DBG
					printf("\nDDR2\n");
					printf("Manual set DTR(0xb8001008)=%x\n",REG32(0xb8001008));
					printf("Manual set DCR(0xb8001004)=%x\n",REG32(0xb8001004));
					printf("Manual set DDCR(0xb8001050)=%x\n",REG32(0xb8001050));
				#endif	
		#endif
	}	


	REG32(0xb8001004)=REG32(0xb8001004);  //DCR



	

}

//==============================================================
#if 0
void LookUp_MemSizeTable(unsigned int dramtype, unsigned int dramsize)
{
	unsigned int dcr,dtr,i,ext=0;
	unsigned int row=0,col=0;
	if(dramtype==DRAM_SDR)  
	{			
		for(i=0; i<SDR_DCR_TAB_ROW; i++)
		{		if(SDR_DCR_TAB[i][0]==dramsize)
					col=i;
		}

		dcr=SDR_DCR_TAB[col][1];
	}
	else if(dramtype==DRAM_DDR1)  
	{
		for(i=0; i<SDR_DCR_TAB_ROW; i++)
		{		if(SDR_DCR_TAB[i][0]==dramsize)
					col=i;
		}

		dcr=SDR_DCR_TAB[col][1];
		
	}
	else if(dramtype==DRAM_DDR2)  
	{
		for(i=0; i<SDR_DCR_TAB_ROW; i++)
		{		if(SDR_DCR_TAB[i][0]==dramsize)
					col=i;
		}

		dcr=SDR_DCR_TAB[col][1];
		if(dramsize >=128) ext=1;
	}	
	//--------------------------------------
	if(ext==1)
	{	REG32(0xB800100c)|=(1<<30);
	}
#if DBG	
	printf("DCR=%x\n",dcr);
#endif	
	REG32(0xb8001004)=dcr;

}
#endif
//==============================================================
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

	unsigned int DCR_VALUE = (1<<30) | (width<<28);  //CL3
	
	#define DCR 0xb8001004
	#define EDTCR 0xb800100c
	#define WRITE_DATA 0x12345678

#if DBG
	printf("w%d,", width);
#endif

	//------------------------------------	
	//find bank
	REG32(DCR)=DCR_VALUE | (1<<19);  //[0]:2 bank,  [1]: 4 bank,  set max BANK=1(4 bank)
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
			REG32(DCR)=tmp | (1<<22);   //set COL=1;  addr*=2
			
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
	printf("b%d,", bank);	
#endif
	//--------------------------------------------------------
	//find row
	REG32(DCR)=DCR_VALUE|(3<<25) | (0<<22);      //set max ROW=3(16K), COL=0(256)
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
	printf("r%d,", row);	
#endif	
	//--------------------------------------------------------
	//find col
	REG32(DCR)=DCR_VALUE|(0<<25) | (4<<22);      //ROW=0(2K), set max COL=4(4K)
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
	printf("c%d,", col);	
#endif	
	//--------------------------------------------------------
	//find chip select
	REG32(DCR)= (1<<30) |  //CL3
				(width<<28)  |  
				(1<<27)|         // set max chip count
				((row-11)<<25) | 
				((col-8)<<22) |
				(((bank<2) ? 0 : 1)  <<19);   // [0] : 2bank, [1]: 4bank

	CHECK_DCR_READY();


		unsigned int *t7=0xa0000000 | (1<<(width+bank+row+col) );
		//printf("t7=%x\n", t7 );
		REG32(t7)=WRITE_DATA;
		REG32(0xa0000000)=0;
		REG32(0xa0000000);
		if(WRITE_DATA==REG32(t7)) 
			cs1=1;
	
#if DBG	
	//printf("cs%d,", cs1);	
#endif	
	//--------------------------------------------------------




#if DBG	
	//printf("=>line=%d\n", width+bank+row+col);	
	unsigned int size=1<<(width+bank+row+col-20);
	printf("size=%d MBytes x %d\n", size, cs1+1);		
#endif

	if(bank==3)			
	{	REG32(EDTCR)&=~(3<<30);
		REG32(EDTCR)|= (1<<30);  // 8 bank
	}

	REG32(DCR)= (1<<30) |  //CL3
				(width<<28)  |  
				(cs1<<27)|    				
				((row-11)<<25) | 
				((col-8)<<22) |
				(((bank<2) ? 0 : 1)  <<19);   // [0] : 2bank, [1]: 4bank
				
#if DBG
	printf("DCR=%x\n",REG32(DCR));
#endif	

#if 0
if(dramtype==2) //JSW::DDR1/2
{
					#ifdef CONFIG_D16_16 //16MB					
						size=16;					
					#endif

					#ifdef CONFIG_D32_16 //32MB						
						size=32;
					#endif
				
					#ifdef CONFIG_D64_16 //64MB
						size=64;
					#endif

					#ifdef CONFIG_D128_16//128MB
						size=128;
					#endif
}
#endif

	//test_dram_readwrite(size);
	return size;	

}
//==============================================================
void DDR_cali_API7(void)
{
    register int i,j,k;

    register int L0 = 0, R0 = 33, L1 = 0, R1 = 33;
    const register int  DRAM_ADR = 0xA0100000;
    const register int  DRAM_VAL = 0x5A5AA5A5;
    const register int  DDCR_ADR = 0xB8001050;
    register int  DDCR_VAL = 0x00000000;  
	


    REG32(DRAM_ADR) = DRAM_VAL;

    while( (REG32(0xb8001050) & 0x40000000) != 0x40000000);
    while( (REG32(0xb8001050) & 0x40000000) != 0x40000000);


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
        printf("L0:%d R0:%d C0:%d\n",  L0, R0, (L0 + R0) >> 1);
        printf("L1:%d R1:%d C1:%d\n",  L1, R1, (L1 + R1) >> 1);
#endif

}

//==============================================================
#define RA 32

const unsigned char c0[1][2]={RA/2,RA/2};
const unsigned  char c1[3*3-1][2]={  {(RA/2+RA)/2, RA/4},   {(RA/2+RA)/2, RA/2},   	{(RA/2+RA)/2, (RA/2+RA)/2},
								{RA/2, 	RA/4},   			       				{RA/2, (RA/2+RA)/2},
								{RA/4, 	RA/4},   		{RA/4, RA/2},   			{RA/4, (RA/2+RA)/2},
							};
/*
const unsigned  char c2[3*3-1][2]={ 	{RA/4, 	RA/4},   		{RA/4, RA/2},   			{RA/4, (RA/2+RA)/2},
								{RA/2, 	RA/4},   			       				{RA/2, (RA/2+RA)/2},
								{(RA/2+RA)/2, RA/4},   {(RA/2+RA)/2, RA/2},   	{(RA/2+RA)/2, (RA/2+RA)/2}
							};
*/							
//------------------------------------------------------------------------------------
void ShowTxRxDelayMap()
{
 	unsigned register tx,rx;
	#define REG32_ANDOR(x,y,z)  { REG32(x)=( REG32(x) & (y) ) | (z);  }

	#define  ADDR   0xA0080000
	#define  PATT0  0x00000000
	#define  PATT1  0xffffffff
	#define  PATT2  0x12345678
	#define  PATT3  0x5A5AA5A5
	#define  PATT4  0xAAAAAAAA	
	#define CLK_MANAGER 0xb8000010
	#define DCR 0xb8001004
	

		for (tx =0; tx<=31; tx++)
		{
					printf("Tx=%02x : ",tx );	
		for (rx =0; rx<= 31; rx++)		
		{
			REG32(DCR)=REG32(DCR);
			CHECK_DCR_READY();				
			CHECK_DCR_READY();	
			
			//printf("c0=(%d,%d), p=(%d,%d)\n", c0[i][0],c0[i][1], tx,rx);

			REG32_ANDOR(CLK_MANAGER,	~( (0x1f<<5) | (0x1f<<0) ),   (tx<<5) | (rx<<0) );
			
			REG32(ADDR)=PATT0;		if(REG32(ADDR)!=PATT0)			goto failc0;
			REG32(ADDR)=PATT1;		if(REG32(ADDR)!=PATT1)			goto failc0;
			REG32(ADDR)=PATT2;		if(REG32(ADDR)!=PATT2)			goto failc0;
			REG32(ADDR)=PATT3;		if(REG32(ADDR)!=PATT3)			goto failc0;
			REG32(ADDR)=PATT4;		if(REG32(ADDR)!=PATT4)			goto failc0;

			printf("%02x,",  rx);			
			continue;

failc0:
			printf("--," );			
			continue;				

		
		}
		printf("\n");
		}
	printf("\n");	
}
//------------------------------------------------------------------------------------
void Calc_TRxDly()
{
   	unsigned register tx,rx;
	#define REG32_ANDOR(x,y,z)  { REG32(x)=( REG32(x) & (y) ) | (z);  }

	#define  ADDR   0xA0100000
	#define  PATT0  0x00000000
	#define  PATT1  0xffffffff
	#define  PATT2  0x12345678
	#define  PATT3  0x5A5AA5A5
	#define  PATT4  0xAAAAAAAA	
	#define CLK_MANAGER 0xb8000010
	#define DCR 0xb8001004
	
	
	#define  delta 5
	unsigned register i=0;

	//----------------------------------------------		
	//test	
	//ShowTxRxDelayMap();

	//for(i=0; i<1; i++)
	{
		printf("c0=(%d,%d) ", c0[i][0],c0[i][1]);
		
		for (tx =c0[i][0]-delta; tx <= c0[i][0]+delta; tx++)
		for (rx =c0[i][1]-delta; rx <= c0[i][1]+delta; rx++)		
		{
			REG32(DCR)=REG32(DCR);
			CHECK_DCR_READY();				
			CHECK_DCR_READY();	
			
			//printf("c0=(%d,%d), p=(%d,%d)\n", c0[i][0],c0[i][1], tx,rx);
			printf("(%d,%d) ",  tx,rx);
			REG32_ANDOR(CLK_MANAGER,	~( (0x1f<<5) | (0x1f<<0) ),   (tx<<5) | (rx<<0) );
			
			REG32(ADDR)=PATT0;		if(REG32(ADDR)!=PATT0)			goto failc0;
			REG32(ADDR)=PATT1;		if(REG32(ADDR)!=PATT1)			goto failc0;
			REG32(ADDR)=PATT2;		if(REG32(ADDR)!=PATT2)			goto failc0;
			REG32(ADDR)=PATT3;		if(REG32(ADDR)!=PATT3)			goto failc0;
			REG32(ADDR)=PATT4;		if(REG32(ADDR)!=PATT4)			goto failc0;
		
		}

		printf("c0=(%d,%d) pass\n",c0[i][0],c0[i][1]);	
		REG32_ANDOR(CLK_MANAGER,	~( (0x1f<<5) | (0x1f<<0) ),   (c0[i][0]<<5) | (c0[i][1]<<0) );		
		return ;
	}
		
failc0:
	//----------------------------------------------

	
	for(i=0; i<3*3-1; i++)
	{
		printf("\nc1=(%d,%d) ", c1[i][0],c1[i][1]);
		
		for (tx =c1[i][0]-delta; tx <= c1[i][0]+delta; tx++)		
		for (rx =c1[i][1]-delta; rx <= c1[i][1]+delta; rx++)		
		{
			REG32(DCR)=REG32(DCR);
			CHECK_DCR_READY();	
			CHECK_DCR_READY();	
			
			//printf("c1=(%d,%d), p=(%d,%d)\n", c1[i][0],c1[i][1], tx,rx);
			printf("(%d,%d) ",  tx,rx);			
			REG32_ANDOR(CLK_MANAGER,	~( (0x1f<<5) | (0x1f<<0) ),   (tx<<5) | (rx<<0) );

			
			REG32(ADDR)=PATT0;		if(REG32(ADDR)!=PATT0)			goto next_c1;
			REG32(ADDR)=PATT1;		if(REG32(ADDR)!=PATT1)			goto next_c1;
			REG32(ADDR)=PATT2;		if(REG32(ADDR)!=PATT2)			goto next_c1;
			REG32(ADDR)=PATT3;		if(REG32(ADDR)!=PATT3)			goto next_c1;
			REG32(ADDR)=PATT4;		if(REG32(ADDR)!=PATT4)			goto next_c1;
			
		}
		
		printf("c1=(%d,%d) pass\n",c1[i][0],c1[i][1]);
		REG32_ANDOR(CLK_MANAGER,	~( (0x1f<<5) | (0x1f<<0) ),   (c1[i][0]<<5) | (c1[i][1]<<0) );
		
		return ;

		next_c1:
		continue;

	}
	//----------------------------------------------	


}
//==============================================================
 #define PAD_CONTROL 0xb8000048

void EnableIP_PADControl(unsigned int dramtype)
{
	if(dramtype==DRAM_DDR2)
	{
		 //REG32(0xb8000010)=(0xe<<5)|(0x08);
		 REG32(PAD_CONTROL)|=(2<<22);
	#if DBG
		printf("Add clock driving for DDR2,PAD_CONTROL(%x)=%x\n",PAD_CONTROL ,REG32(PAD_CONTROL));
	#endif

	}


}
//==============================================================


#if 0
void console_init()
{
	#define UART_BASE         0xB8002000   //uart 0
	//#define UART_BASE         0xB8002100  //uart 1	
	
	#define UART_RBR_REG	(0x00+UART_BASE)
	#define UART_THR_REG	(0x00+UART_BASE)
	#define UART_DLL_REG	(0x00+UART_BASE)
	#define	UART_IER_REG	(0x04+UART_BASE)	
	#define	UART_DLM_REG	(0x04+UART_BASE)
	#define	UART_IIR_REG	(0x08+UART_BASE)
	#define	UART_FCR_REG	(0x08+UART_BASE)
	#define UART_LCR_REG	(0x0c+UART_BASE)
	#define	UART_MCR_REG	(0x10+UART_BASE)
	#define	UART_LSR_REG	(0x14+UART_BASE)
	#define	UART_MSR_REG	(0x18+UART_BASE)
	#define	UART_SCR_REG	(0x1c+UART_BASE)
	
        #define BAUD_RATE	  	(38400)  	   
           
  	REG32(UART_LCR_REG)=0x03000000;		//Line Control Register  8,n,1
  		
  	REG32( UART_FCR_REG)=0xc7000000;		//FIFO Ccontrol Register
  	REG32( UART_IER_REG)=0x00000000;
/*	
  	dl = (lexea_clock /16)/BAUD_RATE-1;
  	dll = dl & 0xff;
  	dlm = dl / 0x100;
*/	
	#define lexea_clock SYS_CLK_RATE
	#define dl    ( (lexea_clock /16)/BAUD_RATE-1)
	#define dll  (dl&0xff)
	#define dlm (dl/0x100)
	
  	REG32( UART_LCR_REG)=0x83000000;		//Divisor latch access bit=1
  	REG32( UART_DLL_REG)=dll*0x1000000;
   	REG32( UART_DLM_REG)=dlm*0x1000000; 
    	REG32( UART_LCR_REG)=0x83000000& 0x7fffffff;	//Divisor latch access bit=0

 	printf("\nUART init\n");
}
#endif
//==============================================================
unsigned int uart_rx4b_val()
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

void RescueMode(void)
{
	#define JUMPADDR 0x80100000
	
	unsigned char ch;
	unsigned char *p=JUMPADDR;
	unsigned int len=0,csum=0;
	unsigned int i;
	unsigned int addr,val;
	void (*jumpF)(void);	

#if 1//DBG	
	printf("Rescue:\n");
#endif

	//header, 1 line, offset [00-15] 
	while(uart_inc()!='b')
	{	
	}

	for(i=0; i<3; i++)
	{	uart_inc();
	}
	
	uart_rx4b_val();  //dummy
	uart_rx4b_val();	 //dummy
	len=uart_rx4b_val();
	
#if DBG		
	printf("Len=%d\n",len);
#endif

	//jump code, 0.5 line,  offset [00-07]
	for(i=2; i>0; i--)
	{	uart_rx4b_val();
	}


	//mem patch, 4.5 lines,  9 record, 
	for(i=9; i>0; i--)
	{	addr=uart_rx4b_val();
		val  =uart_rx4b_val();
		REG32(addr)=val;
#if 0//DBG		
	printf("A=%x, D=%x \n",addr, val);
#endif	


	}
	
	//fill 5 line to memory
	for(i=5*16; i >0;  i--)	
	{			
		*p=0;
		p++;
	}

	//content
	for(i=len-5*16; i>0 ; i--)
	{
		ch=uart_inc();		
		*p=ch;
		p++;
	}
	printf("Jmp");
	jumpF = (void *)(JUMPADDR);
	jumpF();		

	printf("Hang");
	while(1)  ;
}

//==============================================================	
	const unsigned char *boot_type_tab[]={ "SPI", "NOR", "NFBI", "NAND", "ROM1", "ROM2", "ROM3", "ROM4" };
	const unsigned char *dram_type_tab[]={ "SDR" , "SDR" , "DDR2", "DDR1" };
	const unsigned int m2x_clksel_table[]={ 312, 387, 362, 462, 425, 250, 475, 337 	};
//------------------------------------------------------------------
void start_c()
{
#if DBG	
	printf("c start\n");
#endif

	if(kbhit(0x2000))
	{
		if(uart_inc()=='r')
		RescueMode();
		
	}
	//-----------------------------------------------------------------
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	unsigned int v=REG32(SYS_HW_STRAP);
#if DBG	
	printf("Strap=%x\n",v);
#endif
	//-----------
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf
	unsigned char boot_sel=GET_BITVAL(v, 0, RANG3);
	unsigned char dramtype_sel=GET_BITVAL(v, 3, RANG2);
	unsigned char m2x_freq_sel=GET_BITVAL(v, 10, RANG3);


	//-----------
	unsigned char dram_type_remap[]={	DRAM_SDR,   // 00 -> SDR
						DRAM_SDR,   // 01 -> SDR
						DRAM_DDR2,   // 10 ->DDR2
						DRAM_DDR1 // 11-> DDR1
					};

	unsigned char dramtype=dram_type_remap[dramtype_sel];

	unsigned int m2xclk=m2x_clksel_table[m2x_freq_sel];

#if DBG
	printf("Mode=%s\n",boot_type_tab[boot_sel]);
	printf("RAM=%s\n", dram_type_tab[dramtype_sel]);
	printf("CLK=%d\n",m2x_clksel_table[m2x_freq_sel]);
#endif	
        //--------------
	EnableIP_PADControl(dramtype);	
        //--------------
        if((dramtype==DRAM_DDR1)||(dramtype==DRAM_DDR2))
	{  //REG32(0xb8000010)=(0xe<<5)|(0x08);
			REG32(0xb8000010)=(24<<5)|(24);
		DDR_cali_API7();

#if DBG
	   printf("DDCR=%x\n", REG32(0xb8001050));
#endif
        }
        //----------------

	Calc_TRxDly();

#if DBG
	printf("CLKMGR=%x\n", REG32(0xb8000010));	
#endif	
	//----------------
#if 0  
       //strap pin tell size	 for rom booting		
	unsigned int dramsize=DRAM_SIZE_TAB[(boot_sel&3)][dramtype];
#else
	unsigned int dramsize=Calc_Dram_Size(dramtype);
	//unsigned int dramsize=64;
	
#endif

	//printf("SIZE=%d\n",dramsize);

	LookUp_MemTimingTable(dramtype, dramsize, m2xclk);


}
