/*
 *  linux/init/main.c
 *
 *  Copyright (C) 1991, 1992  Linus Torvalds
 *
 *  GK 2/5/95  -  Changed to support mounting root fs via NFS
 *  Added initrd & change_root: Werner Almesberger & Hans Lermen, Feb '96
 *  Moan early if gcc is old, avoiding bogus kernels - Paul Gortmaker, May '96
 *  Simplified starting of init:  Michael A. Griffith <grif@acm.org>
 */
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/random.h>
#include <linux/string.h>

#include <asm/bitops.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/system.h>
#include <linux/circ_buf.h>

#include "main.h"

void invalidate_iram()
{
	__asm__ volatile(
		"mtc0	 $0, $20\n\t"
		"nop\n\t"
		"nop\n\t"
		"li 			 $8,0x00000020\n\t"
		"mtc0	 $8, $20\n\t"
		"nop\n\t"
		"nop\n\t"
		);
}

void start_kernel(void)
{
	int ret;

	IMG_HEADER_T header;
	SETTING_HEADER_T setting_header;
//-------------------------------------------------------
	setClkInitConsole();

	initHeap();
	
	initInterrupt();

#ifdef CONFIG_NFBI

#else
	initFlash();
	Init_GPIO();	
#endif
			

	showBoardInfo();

#if defined(CONFIG_RTL8196E_ULINKER_BOOT_LED)
	power_on_led();		// add by jimmy
#endif

#if (defined(CONFIG_SW_8367R) || defined(CONFIG_SW_8367MB)) && defined(CONFIG_LAN_WAN_ISOLATION)
	{
	extern int init_rtl8367r(void);
	init_rtl8367r();
	}
#endif

	return_addr=0;
	ret=check_image	(&header,&setting_header);

#ifdef SUPPORT_TFTP_CLIENT
#include <rtl8196x/asicregs.h>
	extern volatile unsigned int last_sent_time;
	extern unsigned int tftp_from_command;
	extern int retry_cnt;
	retry_cnt = 0;
	tftp_from_command  = 0;
	last_sent_time  = 0;
    //REG32(GIMR_REG)=0x0;
	eth_startup(0); 
	sti();			
	tftpd_entry(1);
#endif

	invalidate_iram();
	doBooting(ret, return_addr, &header);
}

//-------------------------------------------------------
//show board info
void showBoardInfo(void)
{
	volatile int cpu_speed = 0;	

#ifdef CONFIG_RTL8196E
	#define SYS_ECO_NO 0xb8000000
	#define REG32(reg)  (*(volatile unsigned int *)(reg))	
	if(REG32(SYS_ECO_NO)==0x8196e000)
		SettingCPUClk(0, 2, 0);  //CPU Speed to 400MHz
#endif

#if   defined(CONFIG_DDR2_SDRAM)
	prom_printf("DDR2:");
#elif defined(CONFIG_DDR1_SDRAM)
	prom_printf("DDR1:");
#elif defined(CONFIG_SDRAM)
	prom_printf("SDRAM:");
#endif
#if defined(CONFIG_D16_16)
	prom_printf("16MB\n");
#elif defined(CONFIG_D32_16)
	prom_printf("32MB\n");
#elif defined(CONFIG_D64_16)
	prom_printf("64MB\n");
#elif defined(CONFIG_D128_16)
	prom_printf("128MB\n");
#elif defined(CONFIG_D8_16x2)
	prom_printf("8MB\n");
#elif defined(CONFIG_D16_16x2)
	prom_printf("16MB\n");
#elif defined(CONFIG_D32_16x2)
	prom_printf("32MB\n");
#elif defined(CONFIG_D64_16x2)
	prom_printf("64MB\n");
#elif defined(CONFIG_D128_16x2)
	prom_printf("128MB\n");
#endif

	cpu_speed = check_cpu_speed();	
	prom_printf("%s",((*(volatile unsigned int *)(0xb8000008)) & (0x1<<23))?"Reboot Result from Watchdog Timeout!\n":" ");

#ifdef CONFIG_RTL8196E_KLD
	prom_printf("\n---RealTek(RTL8196E-kld)at %s %s [%s](%dMHz)\n",		BOOT_CODE_TIME,B_VERSION, "16bit", cpu_speed);	
#elif defined(CONFIG_RTL8196E)
	prom_printf("\n---RealTek(RTL8196E)at %s %s [%s](%dMHz)\n",		BOOT_CODE_TIME,B_VERSION, "16bit", cpu_speed);	
#else
	//#define REG32(reg)  (*(volatile unsigned int *)(reg))
    //REG32(0xb8001000)=REG32(0xb8001000)|(1<<28);
	prom_printf("\n---RealTek(RTL8196D)at %s %s [%s](%dMHz)\n",		BOOT_CODE_TIME,B_VERSION, "16bit", cpu_speed);	
#endif
}
