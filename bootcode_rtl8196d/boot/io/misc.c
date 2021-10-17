/**************************************************************************
MISC Support Routines
**************************************************************************/

#include "etherboot.h"

/**************************************************************************
STRCASECMP (not entirely correct, but this will do for our purposes)
**************************************************************************/
int strcasecmp(char *a, char *b)
{
	while (*a && *b && (*a & ~0x20) == (*b & ~0x20)) {a++; b++; }
	return((*a & ~0x20) - (*b & ~0x20));
}

/**************************************************************************
PRINTF and friends

	Formats:
		%[#]X	- 4 bytes long (8 hex digits)
		%[#]x	- 2 bytes int (4 hex digits)
			- optional # prefixes 0x
		%b	- 1 byte int (2 hex digits)
		%d	- decimal int
		%c	- char
		%s	- string
		%I	- Internet address in x.x.x.x notation
	Note: width specification not supported
**************************************************************************/


unsigned int rand2(void);
int SprintF(char *buf, const char *fmt, ...);
#define putchar	serial_outc

#if 1
int vsprintf(char *buf, const char *fmt, const int *dp)
{
	char *p, *s;

	s = buf;
	for ( ; *fmt != '\0'; ++fmt) {
		if (*fmt != '%') {
			buf ? *s++ = *fmt : putchar(*fmt);
			continue;
		}
		if (*++fmt == 's') {
			for (p = (char *)*dp++; *p != '\0'; p++)
				buf ? *s++ = *p : putchar(*p);
		}
		else {	/* Length of item is bounded */
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

			/*
			 * Before each format q points to tmp buffer
			 * After each format q points past end of item
			 */
			if ((*fmt == 'x')||(*fmt == 'X')) {
				/* With x86 gcc, sizeof(long) == sizeof(int) */
				const long *lp = (const long *)dp;
				long h = *lp++;
				int ncase = (*fmt & 0x20);
				dp = (const int *)lp;
				if (alt) {
					*q++ = '0';
					*q++ = 'X' | ncase;
				}
				for ( ; shift >= 0; shift -= 4)
					*q++ = "0123456789ABCDEF"[(h >> shift) & 0xF] | ncase;
			}
			else if (*fmt == 'd') {
				int i = *dp++;
				char *r;
				if (i < 0) {
					*q++ = '-';
					i = -i;
				}
				p = q;		/* save beginning of digits */
				do {
					*q++ = '0' + (i % 10);
					i /= 10;
				} while (i);
				/* reverse digits, stop in middle */
				r = q;		/* don't alter q */
				while (--r > p) {
					i = *r;
					*r = *p;
					*p++ = i;
				}
			}
#if 0	
			else if (*fmt == '@') {
				unsigned char *r;
				union {
					long		l;
					unsigned char	c[4];
				} u;
				const long *lp = (const long *)dp;
				u.l = *lp++;
				dp = (const int *)lp;
				for (r = &u.c[0]; r < &u.c[4]; ++r)
					q += SprintF(q, "%d.", *r);
				--q;
			}
#endif
#if 0
			else if (*fmt == '!') {
				char *r;
				p = (char *)*dp++;
				for (r = p + ETH_ALEN; p < r; ++p)
					q += SprintF(q, "%hhX:", *p);
				--q;
			}
#endif			
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

void prom_printf(const char *fmt, ...)
{
	(void)vsprintf(0, fmt, ((const int *)&fmt)+1);	
}

int SprintF(char *buf, const char *fmt, ...)
{
	return vsprintf(buf, fmt, ((const int *)&fmt)+1);
}
#endif

/**************************************************************************
TWIDDLE
**************************************************************************/

static int twiddle_count;

void twiddle(void)
{
	static const char tiddles[]="-\\|/";
	putchar(tiddles[(twiddle_count++)&3]);
	putchar('\b');
}


int getdec(char **ptr)
{
	char *p = *ptr;
	int ret=0;
	if ((*p < '0') || (*p > '9')) return(-1);
	while ((*p >= '0') && (*p <= '9')) {
		ret = ret*10 + (*p - '0');
		p++;
	}
	*ptr = p;
	return(ret);
}
#if 0
void
putchar(int c)
{
   if (c == '\n')
      PutChar('\r');
   PutChar(c);
}
#endif
#define K_RDWR		0x60		/* keyboard data & cmds (read/write) */
#define K_STATUS	0x64		/* keyboard status */
#define K_CMD		0x64		/* keybd ctlr command (write-only) */

#define K_OBUF_FUL	0x01		/* output buffer full */
#define K_IBUF_FUL	0x02		/* input buffer full */

#define KC_CMD_WIN	0xd0		/* read  output port */
#define KC_CMD_WOUT	0xd1		/* write output port */
#define KB_SET_A20	0xdf		/* enable A20,
					   enable output buffer full interrupt
					   enable data line
					   disable clock line */
#define KB_UNSET_A20	0xdd		/* enable A20,
					   enable output buffer full interrupt
					   enable data line
					   disable clock line */

//==========================================================================
/*
	modify from Casey
	
	NOTE:	MUST change _DOPUTC to perporty
*/
#include <stdarg.h>


int dprintf(char *fmt, ...)
{
	(void)vsprintf(0, fmt, ((const int *)&fmt)+1);	
}
//----------------------------------------------------------------
//----------------------------------------------------------------

void ddump(unsigned char * pData, int len)
{
	unsigned char *sbuf = pData;	
	int length=len;

	int i=0,j,offset;
	dprintf(" [Addr]   .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\r\n" );

	while(i< length)
	{		
			
			dprintf("%08X: ", (sbuf+i) );

			if(i+16 < length)
				offset=16;
			else			
				offset=length-i;
			

			for(j=0; j<offset; j++)
				dprintf("%02x ", sbuf[i+j]);	

			for(j=0;j<16-offset;j++)	//a last line
			dprintf("   ");


			dprintf("    ");		//between byte and char
			
			for(j=0;  j<offset; j++)
			{	
				if( ' ' <= sbuf[i+j]  && sbuf[i+j] <= '~')
					dprintf("%c", sbuf[i+j]);
				else
					dprintf(".");
			}
			dprintf("\n\r");
			i+=16;
	}

	//dprintf("\n\r");


	
}

//----------------------------------------------------------------



#if 0
int sscanf( char *src, char *format, ... )
{
	va_list ap;
	//float *f;
	int conv = 0, *i, index;
	char *a, *fp, *sp = src;
	char buf[256] = {'\0'};

	va_start ( ap, format );
	
	for ( fp = format; *fp != '\0'; fp++ ) 
	{
		for ( index = 0; *sp != '\0' && *sp != ' '; index++ )
			buf[index] = *sp++;
		while ( *sp == ' ' ) sp++;
		while ( *fp != '%' ) fp++;
		if ( *fp == '%' ) 
		{
			switch ( *++fp ) 
			{
				case 'i':
					i = va_arg ( ap, int * );
					*i = atoi ( buf );
				break;
				//case 'f':
				//	f = va_arg ( ap, float * );
				//	*f = (float)atof ( buf );
				//break;
				case 's':
					a = va_arg ( ap, char * );
					strncpy ( a, buf, strlen ( buf ) + 1 );
				break;
			}
		conv++;
		}
	}
	va_end ( ap );
	return conv;
}
#endif

//------------------------------------------------------------------
/*
	     srand(0xFF); //srand can gen the same random sequence from given seed
	   buffer[DWtmp]=(BYTE)(random(256));	      

*/     		
//------------------------------------------------------------------		
unsigned int rtl_seed = 0xDeadC0de;
 
void srand( unsigned int seed )
{
 	rtl_seed = seed;
}
 
unsigned int random(unsigned  int range )
{
	 unsigned int hi32, lo32;
	 unsigned r;
	 
	 hi32 = (rtl_seed>>16)*19;
	 lo32 = (rtl_seed&0xffff)*19+37;
	 hi32 = hi32^(hi32<<16);
	 rtl_seed = (hi32^lo32);
	 r=	rtl_seed %range;
	 return ( r );
}
//-----------------------------------------

unsigned int rand2(void)
{
  static unsigned int x = 123456789;
  static unsigned int y = 362436;
  static unsigned int z = 521288629;
  static unsigned int c = 7654321;

  unsigned long long t, a = 698769069;

  x = 69069 * x + 12345;
  y ^= (y << 13);
  y ^= (y >> 17);
  y ^= (y << 5);
  t = a * z + c;
  c = (t >> 32);
  z = t;

  return x + y + z;
}



 



//-----------------------------------------
void delay(unsigned int time_ms)
{
   unsigned int preTime;
   
   preTime = get_timer_jiffies();
   while ( get_timer_jiffies()-preTime <  time_ms/10 );
}
//-----------------------------------------
void delay_sec(unsigned int time_sec)
{
   unsigned int preTime;
   
   preTime = get_timer_jiffies();
   while ( get_timer_jiffies()-preTime <  time_sec*100 );
}
//-----------------------------------------
void delay_ms(unsigned int time_ms)
{
   unsigned int preTime;
   
   preTime = get_timer_jiffies();
   while ( get_timer_jiffies()-preTime <  time_ms/10 );
}
