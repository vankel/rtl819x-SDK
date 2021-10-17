/*
 * Realtek Semiconductor Corp.
 *
 * bsp/irq-gic.c
 *     GIC initialization and handlers
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */

/*
 * Defined in <asm/gic.h>
 *
 * #define GIC_UNUSED             0xdead
 * #define GIC_FLAG_IPI           0x0001
 * #define GIC_FLAG_TRANSPARENT   0x0002
 * #define GIC_FLAG_VPE_SW0       0x0100
 * #define GIC_FLAG_VPE_SW1       0x0200
 * #define GIC_FLAG_VPE_TIMER     0x0400
 * #define GIC_FLAG_VPE_PERFCNT   0x0800
 *
 * struct gic_intr_map {
 *       unsigned int cpunum;
 *       unsigned int pin;
 *       unsigned int polarity;
 *       unsigned int trigtype;
 *       unsigned int flags;
 * };
 */
#if 0 //wei add, for 8198C
/*
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
*/

#endif
#define X GIC_UNUSED
#define IRQ_SPEEDUP 1

static struct gic_intr_map gic_intr_map[GIC_NUM_INTRS] = {
/*	cpunum;	 pin;	polarity;	trigtype;	flags;	*/
	/* IRQ */
	{ X, X,            X,           X,              0 },
	{ X, X,            X,           X,              0 },
	{ X, X,            X,           X,              0 },
	{ X, X,            X,           X,              0 },
	{ X, X,            X,           X,              0 },
	{ X, X,            X,           X,              0 },
	/* VPE local */
#ifdef CONFIG_PERF_EVENTS
//	{ 0, GIC_CPU_INT4, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_VPE_PERFCTR },  //6, performance
	{ 0, GIC_CPU_INT5, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_VPE_PERFCTR },  //7, performance
#else
	{ X, X,            X,           X,              0 },  //6
#endif
	{ 0, GIC_CPU_INT5, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_VPE_TIMER },  //7, timer
	{ X, X,            X,           X,              0 },  //8
	{ X, X,            X,           X,              0 },  //9
	{ X, X,            X,           X,              0 },  //10
	{ X, X,            X,           X,              0 },  //11
	{ X, X,            X,           X,              0 },  //12
	{ X, X,            X,           X,              0 },  //13
	{ X, X,            X,           X,              0 },  //14
	{ X, X,            X,           X,              0 },  //15
	{ X, X,            X,           X,              0 },  //16
	{ X, X,            X,           X,              0 },  //17
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT },  //18, uart 0
#ifdef CONFIG_SERIAL_RTL_UART1	
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT },  //19  uart 1
#else	
	{ X, X,            X,           X,              0 },  //19
#endif
	{ X, X,            X,           X,              0 },  //20
	{ X, X,            X,           X,              0 },  //21
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //22, i2c	
	{ X, X,            X,           X,              0 },  //23
	{ X, X,            X,           X,              0 },  //24
#ifdef IRQ_SPEEDUP	
	{ 0, GIC_CPU_INT3, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //25, switch
#else
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //25, switch
#endif	
	{ X, X,            X,           X,              0 },  //26
	{ X, X,            X,           X,              0 },  //27
	{ X, X,            X,           X,              0 },  //28
	{ X, X,            X,           X,              0 },  //29
	{ X, X,            X,           X,              0 },  //30
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //31, pcie0
	//{ X, X,            X,           X,              0 },  //32
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //32, pcie1
	//{ X, X,            X,           X,              0 },  //33
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //33, usb3
#ifdef IRQ_SPEEDUP	
	{ 0, GIC_CPU_INT4, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //34, usb3
#else
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //34, usb3
#endif	
	{ X, X,            X,           X,              0 },  //35
	{ X, X,            X,           X,              0 },  //36
	{ X, X,            X,           X,              0 },  //37
#ifdef IRQ_SPEEDUP
	{ 0, GIC_CPU_INT1, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //38, sata
#else	
	{ 0, GIC_CPU_INT0, GIC_POL_POS, GIC_TRIG_LEVEL, GIC_FLAG_TRANSPARENT  },  //38, sata
#endif

       { X, X,            X,           X,              0 },  //39

       { X, X,            X,           X,              0 },  //40
       { X, X,            X,           X,              0 },  //41
       { X, X,            X,           X,              0 },  //42
       { X, X,            X,           X,              0 },  //43
       { X, X,            X,           X,              0 },  //44
       { X, X,            X,           X,              0 },  //45
       { X, X,            X,           X,              0 },  //46
       { X, X,            X,           X,              0 },  //47
       { X, X,            X,           X,              0 },  //48
       { X, X,            X,           X,              0 },  //49
       { X, X,            X,           X,              0 },  //50
       { X, X,            X,           X,              0 },  //51
       { X, X,            X,           X,              0 },  //52
       { X, X,            X,           X,              0 },  //53
       { X, X,            X,           X,              0 },  //54
       { X, X,            X,           X,              0 },  //55

       { X, X,            X,           X,              0 },  //56
       { X, X,            X,           X,              0 },  //57
       { X, X,            X,           X,              0 },  //58
       { X, X,            X,           X,              0 },  //59
       { X, X,            X,           X,              0 },  //60
       { X, X,            X,           X,              0 },  //61
       { X, X,            X,           X,              0 },  //62
       { X, X,            X,           X,              0 },  //63

	/* IPI */
};

#undef X

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,7,0)
void gic_enable_interrupt(int irq_vec)
{
	GIC_SET_INTR_MASK(irq_vec);
}

void gic_disable_interrupt(int irq_vec)
{
	GIC_CLR_INTR_MASK(irq_vec);
}

void gic_irq_ack(struct irq_data *d)
{
	int irq = (d->irq - gic_irq_base);

	GIC_CLR_INTR_MASK(irq);

	if (gic_irq_flags[irq] & GIC_TRIG_EDGE)
		GICWRITE(GIC_REG(SHARED, GIC_SH_WEDGE), irq);
}

void gic_finish_irq(struct irq_data *d)
{
	/* Enable interrupts. */
	GIC_SET_INTR_MASK(d->irq - gic_irq_base);
}
#endif

void dump_gic_table(void)
{
	int i;

	printk("irq : cpunum : pin : polarity : trigtype : flags\n");
	for(i=0; i<ARRAY_SIZE(gic_intr_map); i++)
	{	
		printk("[%d] %x : %x : %x : %x : %x\n",i,\
			gic_intr_map[i].cpunum,\
			gic_intr_map[i].pin,\
			gic_intr_map[i].polarity,\
			gic_intr_map[i].trigtype,\
			gic_intr_map[i].flags			);
	}
	
}

#if 1  //wei add, for cascade interrupt.
irqreturn_t bsp_gic_irq_dispatch(int cpl, void *dev_id)
{

	int irq;
	irq=gic_get_int();
	//printk(" GIRQ=%d \n", irq);
	if (irq >= 0)
		do_IRQ(BSP_IRQ_GIC_BASE + irq);
	else
		spurious_interrupt();

	return IRQ_HANDLED;
}

static struct irqaction irq_cascade = {
	.handler = bsp_gic_irq_dispatch,
	.flags      = IRQF_PERCPU,		
	.name = "cascade",
};

void gic_cascade_init(void)
{
	setup_irq(BSP_GIC_CASCADE_IRQ, &irq_cascade);

}

#endif


