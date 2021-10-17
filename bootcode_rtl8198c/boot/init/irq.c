#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/mipsregs.h>




#if defined(RTL8198)
#include <asm/rtl8198.h>
#endif

#ifdef CONFIG_RTL8198C
#include <asm/rtl8198c.h>
#endif

unsigned long exception_handlers[32];
void set_except_vector(int n, void *addr);
asmlinkage void do_reserved(struct pt_regs *regs);
void __init exception_init(void);


static struct irqaction *irq_action[NR_IRQS] =
{
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};



//#define ALLINTS (IE_IRQ0|IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 | IE_IRQ5)	
#define ALLINTS (IE_IRQ0|IE_IRQ1 | IE_IRQ2 | IE_IRQ3 | IE_IRQ4 )	
//------------------------------------------------------------------------------

inline unsigned int					
clear_cp0_status(unsigned int clear)				
{								
	unsigned int res;					
								
	res = read_32bit_cp0_register(CP0_STATUS);		
	res &= ~clear;						
	write_32bit_cp0_register(CP0_STATUS, res);		
	SPECIAL_EHB();	
}								
//------------------------------------------------------------------------------

inline unsigned int					
change_cp0_status(unsigned int change, unsigned int newvalue)	
{								
	unsigned int res;					
								
	res = read_32bit_cp0_register(CP0_STATUS);		
	res &= ~change;						
	res |= (newvalue & change);					
	write_32bit_cp0_register(CP0_STATUS, res);		
	SPECIAL_EHB();							
	return res;						
}


//------------------------------------------------------------------------------
//============================================================================
GIC_GIMR_enable(int irq)
{
	
	if(irq<32)
	{
		REG32(GIC_BASE_ADDR+0x380)=(1<<irq);
	}
	else
	{		
		REG32(GIC_BASE_ADDR+0x384)=(1<<(irq-32));		
	}

	//map2pin
	REG32(GIC_BASE_ADDR+0x500+irq*4)=0x80000000;	

	//map2vpe
	REG32(GIC_BASE_ADDR+0x2000+irq*0x20)=0x1;		
}
//------------------------------------------------------------------------------

static void  GIMR_enable_irq(unsigned int irq)
{
	REG32(GIMR_REG) |= (1<<irq);
  
}
//------------------------------------------------------------------------------

static void  GIMR_disable_irq(unsigned int irq)
{

	REG32(GIMR_REG) &= ~(1<<irq);	
}

extern  void do_IRQ(int irq);
//------------------------------------------------------------------------------

void irq_dispatch(int irq_nr, int irq_nr2)
{
	int i,irq=0;
	//prom_printf("irq.c : irq_nr=%x  irq_nr2=%x  \n",irq_nr, irq_nr2);
	
	//Low 32bit
    for (i=0; i<=31; i++)
    {
        if (irq_nr & 0x01)
		{
			//prom_printf("do irq=%x\n",irq);
			do_IRQ(irq);

		}  
        irq++;
        irq_nr = irq_nr >> 1;
    }

	//High 32bit
    for (i=0; i<=31; i++)
    {
        if (irq_nr2 & 0x01)
		{
			if(irq==43)
   			{
    				if(REG32(0xbbdc2560)==2)   //for core 1, not process.
     				return;
   			}		
	
					//prom_printf("do irq=%x\n",irq);
			do_IRQ(irq);

		}  
        irq++;
        irq_nr2 = irq_nr2 >> 1;
    }
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


void  init_IRQ(void)
{
	extern asmlinkage void IRQ_finder(void);

	unsigned int i;

	/* Disable all hardware interrupts */
	change_cp0_status(ST0_IM, 0x00);
	

	/* Set up the external interrupt exception vector */
	/* First exception is Interrupt*/
	set_except_vector(0, IRQ_finder);

	/* Enable all interrupts */
	change_cp0_status(ST0_IM, ALLINTS);
}


// below is adopted from kernel/irq.c

//------------------------------------------------------------------------------

int setup_IRQ(int irq, struct irqaction *new)
{
    int shared = 0;
    struct irqaction *old, **p;
    unsigned long flags;

    p = irq_action + irq;
//    prom_printf("IRQ action=%x,%x\n",irq_action,irq);
    save_and_cli(flags);
    *p = new;
    
    restore_flags(flags);
    
    return 0;
}
//------------------------------------------------------------------------------
#if 0 //wei add, for 8198C
assign cpu_int_ip[00]=1'b0; //flsh_int_pls;
assign cpu_int_ip[01]=1'b0; //int_ps_otgctrl;
assign cpu_int_ip[02]=oc1_timeout_intps;
assign cpu_int_ip[03]=oc2_timeout_intps;
assign cpu_int_ip[04]=int_ps_lx0_bframe;
assign cpu_int_ip[05]=1'b0; //int_ps_lx1_bframe;
assign cpu_int_ip[06]=int_ps_lx2_bframe;
assign cpu_int_ip[07]=int_ps_lx0_btrdy;
assign cpu_int_ip[08]=1'b0; //int_ps_lx1_btrdy;
assign cpu_int_ip[09]=int_ps_lx2_btrdy;
assign cpu_int_ip[10]=int_ps_lx0_slv_btrdy;
assign cpu_int_ip[11]=1'b0; //int_ps_lx1_slv_btrdy;
assign cpu_int_ip[12]=int_ps_lx2_slv_btrdy;
assign cpu_int_ip[13]=1'b0; //~wdog_rst_n_p2;
assign cpu_int_ip[14]=timer0_ip;
assign cpu_int_ip[15]=timer1_ip;
assign cpu_int_ip[16]=1'b0; //timer2_ip;
assign cpu_int_ip[17]=1'b0; //timer3_ip;
assign cpu_int_ip[18]=uart0_ip;
assign cpu_int_ip[19]=uart1_ip;
assign cpu_int_ip[20]=1'b0; //uart2_ip;
assign cpu_int_ip[21]=1'b0; //uart3_ip;
assign cpu_int_ip[22]=1'b0; //i2c_ip;
assign cpu_int_ip[23]=1'b0; //efuse_ctrl_ip;
assign cpu_int_ip[24]=1'b0; //int_voipacc;
assign cpu_int_ip[25]=1'b0; //swlbcint;
assign cpu_int_ip[26]=1'b0; //gpio0_ip;
assign cpu_int_ip[27]=1'b0; //gpio1_ip;
assign cpu_int_ip[28]=1'b0; //int_nfbi;
assign cpu_int_ip[29]=1'b0; //int_pcm;
assign cpu_int_ip[30]=1'b0; //int_ipsec;
assign cpu_int_ip[31]=1'b0; //int_pcie0;
assign cpu_int_ip[32]=1'b0; //int_pcie1;
assign cpu_int_ip[33]=1'b0; //int_usbotg;
assign cpu_int_ip[34]=int_usb3;
assign cpu_int_ip[35]=1'b0; //int_usbwake1;
assign cpu_int_ip[36]=1'b0; //int_iis;
assign cpu_int_ip[37]=1'b0; //int_usbwake0;
assign cpu_int_ip[38]=1'b0; //int_ahsata;
assign cpu_int_ip[39]=1'b0; //int_fftacc;
assign cpu_int_ip[40]=1'b0; //int_xsi;
assign cpu_int_ip[41]=1'b0; //spi_int;
assign cpu_int_ip[42]=si_timer_int;
assign cpu_int_ip[43]=int_cpuwake;
assign cpu_int_ip[44]=int_cpu1tr2;
assign cpu_int_ip[45]=int_cpu2tr1;
assign cpu_int_ip[46]=oc1_cpu_ila_int;
assign cpu_int_ip[47]=oc2_cpu_ila_int;
assign cpu_int_ip[48]=1'b0; //{|int_gphy[4:0]};
assign cpu_int_ip[49]=1'd0;

#endif


int request_IRQ(unsigned long irq, struct irqaction *action, void* dev_id)
{

    int retval;
      
 //   prom_printf("IRQ No=%x,%x\n",irq,NR_IRQS);
    if (irq >= NR_IRQS)
		return -EINVAL;

	action->dev_id = dev_id;
	
    retval = setup_IRQ(irq, action);
 //   prom_printf("devid & retval =%x,%x\n",dev_id,retval);
 	
	//GIMR_enable_irq(irq);
	GIC_GIMR_enable(irq);
	
    if (retval)

	    return retval;
}

//------------------------------------------------------------------------------

int	free_IRQ(unsigned long irq)
{
	GIMR_disable_irq(irq);	
	
}

//------------------------------------------------------------------------------

void do_IRQ(int irqnr)
{
    struct irqaction *action;
	unsigned long i;

	//printf("Got irq %d\n", irqnr);
	
    action = *(irqnr + irq_action);
        	
	if (action) 
    {
    	//printf("Do ISR=%x\n", action->handler);
	    action->handler(irqnr, action->dev_id, NULL);
    }
	else
	{    
		prom_printf("Fail, you got irq=%X, but not have ISR\n", irqnr);
		for(;;);
	}			
}	


//------------------------------------------------------------------------------
void set_except_vector(int n, void *addr)
{
	unsigned handler = (unsigned long) addr;
	exception_handlers[n] = handler;
}
//------------------------------------------------------------------------------

void do_reserved(struct pt_regs *regs)
{
	/*fatal hard/software error*/
	int i;
	prom_printf("Undefined Exception happen.");	
	for(;;);
	/*Just hang here.*/
}

unsigned long ebase_reg = 0;
//------------------------------------------------------------------------------
void  init_exception(void)
{
	extern char exception_matrix;

	unsigned long i;
	clear_cp0_status(ST0_BEV);

	for (i = 0; i <= 31; i++)
		set_except_vector(i, do_reserved);

#ifdef CONFIG_NAND_FLASH_BOOTING
	ebase_reg = 0xa0000000;
	write_c0_ebase(ebase_reg);
    memcpy((void *)(KSEG1 + 0x180), &exception_matrix, 0x80);
#else
    memcpy((void *)(KSEG0 + 0x180), &exception_matrix, 0x80);
#endif
    flush_cache();
}
//------------------------------------------------------------------------------

//============================================================================

// init interrupt 
void initInterrupt(void)
{
	//printf("=>init interrupt...\n");
	REG32(GIMR_REG)=0x00;/*mask all interrupt*/
	
		REG32(0xb8003114)=0;  //disable timer interrupt
		REG32(0xb8000010)&=~(1<<11);
		
		REG32(0xbbdc0300)=0xFFFFFFFF;
		REG32(0xbbdc0304)=0xFFFFFFFF;
	
   /*setup the BEV0,and IRQ */
	init_exception();/*Copy handler to 0x80000080*/
	init_IRQ();      /*Allocate IRQfinder to Exception 0*/
	sti();
}
