/*
 * init.c: early initialisation code for R39XX Class PDAs
 *
 * Copyright (C) 1999 Harald Koerfgen
 *
 * $Id: init.c,v 1.1 2009/11/13 13:22:47 jasonwang Exp $
 */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/random.h>
#include <asm/bitops.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/system.h>
#include <linux/circ_buf.h>
#include <asm/io.h>



#if defined(RTL8198)
#include <asm/rtl8198.h>
#endif




//#include <linux/init.h>
//#include <linux/config.h>
//#include <linux/kernel.h>
//#include <asm/rtl8181.h>
//#include <asm/io.h>




void serial_outc(char c)
{
	int i;
	i=0;


#if 1
    while (1)
	{
#if 1	
		i++;
		if (i >=6540)
			break;
#endif		
		if 	(rtl_inb(UART_LSR) & 0x60)
			break;	
	}
#endif

	//for(i=0; i<0xff00;i++);
 	rtl_outb(UART_THR, c);  
		  
	if (c == 0x0a)
		rtl_outb(UART_THR, 0x0d);  
	// ----------------------------------------------------
	// above is UART0, and below is SC16IS7x0 
	// ----------------------------------------------------

}


#if defined(CONFIG_TFTP_COMMAND)
extern void autoreboot();
volatile unsigned int tickStart = 0;

#define typecheck(type,x) \
    ({type __dummy; \
        typeof(x) __dummy2; \
        (void)(&__dummy == &__dummy2); \
        1; \
     })

#define time_after(a,b)\
        (typecheck(unsigned long, a) && \
             typecheck(unsigned long, b) && \
             ((long)(b) - (long)(a) < 0))

#define time_before(a,b)time_after(b,a)

#endif
char serial_inc()
{
	int i;
#if defined(CONFIG_TFTP_COMMAND)
	tickStart=get_timer_jiffies();
#endif
    while (1)
	{
		if 	(rtl_inb(UART_LSR) & 0x1)
			break;		
#if defined(CONFIG_TFTP_COMMAND)
        if(time_after((get_timer_jiffies()),(tickStart+18000)))
		//if((get_timer_jiffies() - tickStart) > 18000) //3min=18000  1min=6000
		    autoreboot();
#endif
	}	
	i=rtl_inb(UART_RBR);
	return (i & 0xff);
	// ----------------------------------------------------
	// above is UART0, and below is SC16IS7x0 
	// ----------------------------------------------------
	
}

int isspace(char ch)
{
    return (unsigned int)(ch - 9) < 5u  ||  ch == ' ';	
}


/*
 * Helpful for debugging :-)
 */
 #if 0
int prom_printf(const char * fmt, ...)
{

	static char buf[1024];
	va_list args;
	char c;
	int i = 0;

	/*
	 * Printing messages via serial port
	 */
	va_start(args, fmt);
	vsprintf(buf, fmt, args);
	//do_printf(buf, fmt, args);
	va_end(args);

	for (i = 0; buf[i] != '\0'; i++) {
		c = buf[i];
		if (c == '\n')
			serial_outc('\r');
		serial_outc(c);
	}

	return i;
}
#endif
