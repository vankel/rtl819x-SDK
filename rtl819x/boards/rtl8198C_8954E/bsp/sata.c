/*
 * Realtek Semiconductor Corp.
 *
 * bsp/usb.c:
 *     bsp SATA initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include "bspchip.h"



/* SATA Host Controller */

static struct resource bsp_sata_resource[] = {
	[0] = {
		.start = BSP_SATA_MAPBASE,
		.end = BSP_SATA_MAPBASE + BSP_SATA_MAPSIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = BSP_PS_SATA_IRQ,
		.end = BSP_PS_SATA_IRQ,
		.flags = IORESOURCE_IRQ,
	}
};

static u64 bsp_sata_dmamask = 0xFFFFFFFFUL;

struct platform_device bsp_sata_device = {
	.name = "ahci",
	.id = -1,
	.num_resources = ARRAY_SIZE(bsp_sata_resource),
	.resource = bsp_sata_resource,
	.dev = {
		.dma_mask = &bsp_sata_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};

static struct platform_device *bsp_sata_devs[] __initdata = {	&bsp_sata_device,  };

static int __init bsp_sata_init(void)
{
	int ret;

	printk("INFO: initializing SATA devices ...\n");

	ret = platform_add_devices(bsp_sata_devs, ARRAY_SIZE(bsp_sata_devs));
	if (ret < 0) {
		printk("ERROR: unable to add devices\n");
		return ret;
	}

	return 0;
}
arch_initcall(bsp_sata_init);
