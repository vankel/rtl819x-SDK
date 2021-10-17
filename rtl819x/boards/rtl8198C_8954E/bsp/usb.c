/*
 * Realtek Semiconductor Corp.
 *
 * bsp/usb.c:
 *     bsp USB initialization code
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
#include "usb.h"


/* USB Host Controller */

static struct resource bsp_usb3_resource[] = {
	[0] = {
		.start = BSP_USB_USB3_MAPBASE,
		.end = BSP_USB_USB3_MAPBASE + BSP_USB_USB3_MAPSIZE - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = BSP_PS_USB_IRQ_USB3,
		.end = BSP_PS_USB_IRQ_USB3,
		.flags = IORESOURCE_IRQ,
	}
};

static u64 bsp_usb_dmamask = 0xFFFFFFFFUL;

struct platform_device bsp_usb3_device = {
	.name = "dwc_usb3",
	.id = -1,
	.num_resources = ARRAY_SIZE(bsp_usb3_resource),
	.resource = bsp_usb3_resource,
	.dev = {
		.dma_mask = &bsp_usb_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};

static struct platform_device *bsp_usb_devs[] __initdata = {
	&bsp_usb3_device,
};

static int __init bsp_usb_init(void)
{
	int ret;

	printk("INFO: initializing USB devices ...\n");
#if 0 //wei add for FPGA sata test	
	return -1;
#endif	
	ret = platform_add_devices(bsp_usb_devs, ARRAY_SIZE(bsp_usb_devs));
	if (ret < 0) {
		printk("ERROR: unable to add devices\n");
		return ret;
	}

	return 0;
}
arch_initcall(bsp_usb_init);
