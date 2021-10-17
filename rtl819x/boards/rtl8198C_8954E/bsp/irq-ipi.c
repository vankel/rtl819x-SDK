/*
 * Realtek Semiconductor Corp.
 *
 * bsp/irq-ipi.c
 *     IPI initialization and handlers
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */

unsigned int bsp_ipi_map[NR_CPUS];

#ifdef CONFIG_IRQ_GIC
static void bsp_ipi_dispatch(void)
{
	int irq;

	irq = gic_get_int();
	if (irq < 0)
		return;  /* interrupt has already been cleared */

	do_IRQ(BSP_IRQ_GIC_BASE + irq);
}
#endif

/*
 * Handle SMP IPI interrupts
 *
 * Two IPI interrupts, resched and call, are handled here.
 */
static irqreturn_t bsp_ipi_resched(int irq, void *devid)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,0,0)
	scheduler_ipi();
#endif
	return IRQ_HANDLED;
}

static irqreturn_t bsp_ipi_call(int irq, void *devid)
{
	smp_call_function_interrupt();
	return IRQ_HANDLED;
}

static struct irqaction irq_resched = {
	.handler    = bsp_ipi_resched,
	.flags      = IRQF_PERCPU,
	.name       = "IPI resched"
};

static struct irqaction irq_call = {
	.handler    = bsp_ipi_call,
	.flags      = IRQF_PERCPU,
	.name       = "IPI call"
};

/*
 * Initialize IPI interrupts.
 *
 * In MIPS_MT_SMP mode, IPI interrupts are routed via SW0 and SW1.
 * When GIC is present, IPI interrupts are routed via GIC.
 *
 * FIXME: what about MIPS1004K ?
 */
void __init bsp_ipi_init(struct gic_intr_map *intrmap)
{
	int irq;
#ifdef CONFIG_IRQ_GIC
	int cpu;

	/*
	 * setup GIC IPI interrupts
	 */
	gic_setup_ipi(bsp_ipi_map, GIC_CPU_INT1, GIC_CPU_INT2);

	printk("CPU%d: status register was %08x\n", smp_processor_id(), read_c0_status());
	write_c0_status(read_c0_status() | STATUSF_IP3 | STATUSF_IP4);
	printk("CPU%d: status register now %08x\n", smp_processor_id(), read_c0_status());

	for (cpu = 0; cpu < NR_CPUS; cpu++) {
		irq = BSP_IRQ_GIC_BASE + GIC_IPI_RESCHED(cpu);
		setup_irq(irq, &irq_resched);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
		irq_set_handler(irq, handle_percpu_irq);
#else
		set_irq_handler(irq, handle_percpu_irq);
#endif

		irq = BSP_IRQ_GIC_BASE + GIC_IPI_CALL(cpu);
		setup_irq(irq, &irq_call);
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,30)
		irq_set_handler(irq, handle_percpu_irq);
#else
		set_irq_handler(irq, handle_percpu_irq);
#endif
	}
#endif
#ifdef CONFIG_MIPS_MT_SMP
	setup_irq(BSP_IRQ_CPU_BASE, &irq_resched);
	setup_irq(BSP_IRQ_CPU_BASE + 1, &irq_call);
#endif
}
