/*
 * Copyright (C) 2013 Realtek Semiconductor Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __CLKSOURCE_ARM_GLOBAL_TIMER_H
#define __CLKSOURCE_ARM_GLOBAL_TIMER_H

#include <linux/clocksource.h>
#include <linux/types.h>
#include <linux/ioport.h>

#ifndef CONFIG_OF
struct gt_timer_device {
	struct resource res[2];
};

#define DEFINE_GT_TIMER_DEVICE(name,base,irq)		\
struct gt_timer_device name __initdata = {		\
	.res	= {					\
		DEFINE_RES_MEM(base, 0x20),		\
		DEFINE_RES_IRQ(irq),			\
	},						\
};

extern int global_timer_register(struct gt_timer_device *);
#endif

#endif
