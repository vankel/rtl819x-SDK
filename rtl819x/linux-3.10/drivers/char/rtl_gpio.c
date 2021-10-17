/*
 * FILE NAME rtl_gpio.c
 *
 * BRIEF MODULE DESCRIPTION
 *  GPIO For Flash Reload Default
 *
 *  Author: jimmylin@realtek.com.tw
 *
 *  Copyright (c) 2011 Realtek Semiconductor Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */


#include <generated/autoconf.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/interrupt.h>
#include <asm/errno.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/reboot.h>
#include <linux/kmod.h>
#include <linux/proc_fs.h>
#include  "bspchip.h"

#include <linux/seq_file.h>

//#define CONFIG_USING_JTAG 1
#define AUTO_CONFIG

/*
enabled immediate mode pbc ; must enabled "USE_INTERRUPT_GPIO" and "IMMEDIATE_PBC"
gpio will rx pbc event and trigger wscd by signal method
note:also need enabled IMMEDIATE_PBC at wsc.h (sdk/users/wsc/src/)
*/
//#define USE_INTERRUPT_GPIO	//undefine USE_INTERRUPT_GPIO
//#define IMMEDIATE_PBC 

#ifdef IMMEDIATE_PBC
int	wscd_pid = 0;
struct pid *wscd_pid_Ptr=NULL;
#endif


#if defined(CONFIG_RTL_8198C) 
	////ori////#include "drivers/net/rtl819x/AsicDriver/rtl865xc_asicregs.h"
	#include "../net/rtl819x/AsicDriver/rtl865xc_asicregs.h"

#if 0
/*dummy api for porting*/
void RTL_BSP_GPIO_write(unsigned int gpio_num, unsigned int value)
{
    printk("this is dummy api , when api in bsp ready : use new api\n");
}
#endif
#if  1//defined(CONFIG_RTL_8198CD)

	// GPIO H5
	#define RESET_PIN_IOBASE    PEFGH_CNR 	
	#define RESET_PIN_DIRBASE   PEFGH_DIR 
	#define RESET_PIN_DATABASE  PEFGH_DAT
	#define RESET_PIN_NO 29 /*number of the EFGH*/
	// GPIO H7
	#define RESET_LED_IOBASE    PEFGH_CNR 	
	#define RESET_LED_DIRBASE   PEFGH_DIR	 
	#define RESET_LED_DATABASE  PEFGH_DAT 
	#define RESET_LED_NO 24+7 /*number of the EFGH*/
	// GPIO H7
	#define AUTOCFG_LED_IOBASE      PEFGH_CNR 	
	#define AUTOCFG_LED_DIRBASE     PEFGH_DIR	 
	#define AUTOCFG_LED_DATABASE    PEFGH_DAT 
	#define AUTOCFG_LED_NO 24+7 /*number of the EFGH*/
	// GPIO H6
	#define AUTOCFG_PIN_IOBASE      PEFGH_CNR 	
	#define AUTOCFG_PIN_DIRBASE     PEFGH_DIR	 
	#define AUTOCFG_PIN_DATABASE    PEFGH_DAT 
	#define AUTOCFG_PIN_NO 30 /*number of the EFGH)*/
   
	#define AUTOCFG_PIN_IMR PGH_IMR    
	#define AUTOCFG_BTN_PIN         AUTOCFG_PIN_NO
	#define AUTOCFG_LED_PIN         AUTOCFG_LED_NO
	#define RESET_LED_PIN           RESET_LED_NO
	#define RESET_BTN_PIN           RESET_PIN_NO
		
#ifdef USE_INTERRUPT_GPIO
	#define GPIO_IRQ_ABCD_NUM		26		
	#define GPIO_IRQ_EFGH_NUM		27
	#define GPIO_IRQ_NUM	     	27
#endif
	#define RTL_GPIO_MUX    0xB8000100
    #define RTL_GPIO_MUX2   0xB8000104
    #define RTL_GPIO_MUX3   0xB8000108 
    #define RTL_GPIO_MUX4   0xB800010C
    #define RTL_GPIO_MUX5   0xB8000110
    /*if 8198C demo board need to modify default mux sel value then ebable below*/
    //#define RTL_GPIO_MUX_DATA 0x00340C00//for WIFI ON/OFF and GPIO
	////#define RTL_GPIO_WIFI_ONOFF     19
	#define RTL_GPIO_PCIE_RESET_MUX	(3<<10)
	#define RTL_GPIO_RESET_BTN_MUX	(3<<23)
	#define RTL_GPIO_MUX4_DATA		( RTL_GPIO_PCIE_RESET_MUX | RTL_GPIO_RESET_BTN_MUX)

	 #define RTL_GPIO_SYS_LED_MUX    (3<<14)
	#define RTL_GPIO_WPS_BTN_MUX	(3<<0)
	#define RTL_GPIO_MUX5_DATA		(RTL_GPIO_WPS_BTN_MUX | RTL_GPIO_SYS_LED_MUX)

#else
	// GPIO H5
	#define RESET_PIN_IOBASE    PEFGH_CNR 	
	#define RESET_PIN_DIRBASE   PEFGH_DIR 
	#define RESET_PIN_DATABASE  PEFGH_DAT
	#define RESET_PIN_NO 29 /*number of the EFGH*/
	// GPIO G0
	#define RESET_LED_IOBASE    PEFGH_CNR 	
	#define RESET_LED_DIRBASE   PEFGH_DIR	 
	#define RESET_LED_DATABASE  PEFGH_DAT 
	#define RESET_LED_NO 7+24 /*number of the EFGH*/
	// GPIO G0
	#define AUTOCFG_LED_IOBASE      PEFGH_CNR 	
	#define AUTOCFG_LED_DIRBASE     PEFGH_DIR	 
	#define AUTOCFG_LED_DATABASE    PEFGH_DAT 
	#define AUTOCFG_LED_NO 7+24 /*number of the EFGH*/
	// GPIO H6
	#define AUTOCFG_PIN_IOBASE      PEFGH_CNR 	
	#define AUTOCFG_PIN_DIRBASE     PEFGH_DIR	 
	#define AUTOCFG_PIN_DATABASE    PEFGH_DAT 
	#define AUTOCFG_PIN_NO 30 /*number of the EFGH)*/
   
	#define AUTOCFG_PIN_IMR PGH_IMR    
	#define AUTOCFG_BTN_PIN         AUTOCFG_PIN_NO
	#define AUTOCFG_LED_PIN         AUTOCFG_LED_NO
	#define RESET_LED_PIN           RESET_LED_NO
	#define RESET_BTN_PIN           RESET_PIN_NO
		
#ifdef USE_INTERRUPT_GPIO
	#define GPIO_IRQ_ABCD_NUM		26		
	#define GPIO_IRQ_EFGH_NUM		27
	#define GPIO_IRQ_NUM	     	27
#endif
	#define RTL_GPIO_MUX    0xB8000100
    #define RTL_GPIO_MUX2   0xB8000104
    #define RTL_GPIO_MUX3   0xB8000108 
    #define RTL_GPIO_MUX4   0xB800010C
    #define RTL_GPIO_MUX5   0xB8000110
    /*if 8198C demo board need to modify default mux sel value then ebable below*/
    //#define RTL_GPIO_MUX_DATA 0x00340C00//for WIFI ON/OFF and GPIO
	////#define RTL_GPIO_WIFI_ONOFF     19
	#define RTL_GPIO_SYS_LED_MUX	(3<<7)
	#define RTL_GPIO_PCIE_RESET_MUX	(3<<10)
	#define RTL_GPIO_RESET_BTN_MUX	(3<<23)
	#define RTL_GPIO_MUX4_DATA		(RTL_GPIO_SYS_LED_MUX | RTL_GPIO_PCIE_RESET_MUX | RTL_GPIO_RESET_BTN_MUX)

	
        #define RTL_GPIO_SYS_LED_MUX    (3<<14)
        #define RTL_GPIO_WPS_BTN_MUX    (3<<0)
        #define RTL_GPIO_MUX5_DATA              (RTL_GPIO_WPS_BTN_MUX | RTL_GPIO_SYS_LED_MUX)



#endif	
	
#endif // CONFIG_RTL_8198C



///////////////////////////////////////////////////////
#define PROBE_TIME	5
#define PROBE_NULL		0
#define PROBE_ACTIVE	1
#define PROBE_RESET		2
#define PROBE_RELOAD	3
#define RTL_R32(addr)		(*(volatile unsigned long *)(addr))
#define RTL_W32(addr, l)	((*(volatile unsigned long*)(addr)) = (l))
#define RTL_R8(addr)		(*(volatile unsigned char*)(addr))
#define RTL_W8(addr, l)		((*(volatile unsigned char*)(addr)) = (l))

//#define  GPIO_DEBUG
#ifdef GPIO_DEBUG
/* note: prints function name for you */
#  define DPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__ , ## args)
#else
#  define DPRINTK(fmt, args...)
#endif

static struct timer_list probe_timer;
static unsigned int    probe_counter;
static unsigned int    probe_state;

static char default_flag='0';
//Brad add for update flash check 20080711
int start_count_time=0;
int Reboot_Wait=0;

//static int get_dc_pwr_plugged_state(void);

#ifdef CONFIG_RTL_USERSPACE_WTDOG

#define RTL_WATCHDOG_START	0x01
#define RTL_WATCHDOG_STOP	0x02
#define RTL_WATCHDOG_KICK	0x01

static int watchdog_start_state = 0;
static int watchdog_kick_state = 0;
/* watchdog start */
static int read_watchdog_start_proc(struct seq_file *s, void *v)
{
	int len;
	char flag = '0';

	if(watchdog_start_state == RTL_WATCHDOG_START)
		flag = '1';
	else if(watchdog_start_state == RTL_WATCHDOG_STOP)
		flag = '2';

	seq_printf(s,"%c\n", flag);

	return 0;
}

int watchdog_start_open(struct inode *inode, struct file *file)
{
        return(single_open(file, read_watchdog_start_proc, NULL));
}

static ssize_t watchdog_start_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	char flag[20];

	extern int bBspWatchdog;
	extern void bsp_enable_watchdog(void);
	extern void bsp_disable_watchdog(void);
	
	if (count < 2)
		return -EFAULT;
	if (userbuf && !copy_from_user(&flag, userbuf, 1)) {
		if(flag[0] == '1'){
			watchdog_start_state = RTL_WATCHDOG_START;
			/* enable watchdog */
			bsp_enable_watchdog();
		}else if(flag[0] == '2'){
			watchdog_start_state = RTL_WATCHDOG_STOP;
			/* disable watchdog */
			bsp_disable_watchdog();
		}else
			watchdog_start_state = 0;
		return count;
	}
	
	return -EFAULT;	
}

struct file_operations watchdog_start_proc_fops = {
        .open           = watchdog_start_open,
        .write         = watchdog_start_single_write,
        .read            = seq_read,
        .llseek          = seq_lseek,
        .release        = single_release,
};

/* watchdog kick */
static int read_watchdog_kick_proc(struct seq_file *s, void *v)
{
	int len;
	char flag = '0';

	if(watchdog_kick_state == RTL_WATCHDOG_KICK)
		flag = '1';

	seq_printf(s,"%c\n", flag);

	return 0;
}

int watchdog_kick_open(struct inode *inode, struct file *file)
{
        return(single_open(file, read_watchdog_kick_proc, NULL));
}

static ssize_t watchdog_kick_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	char flag[20];

	if (count < 2)
		return -EFAULT;
	if (userbuf && !copy_from_user(&flag, userbuf, 1)) {
		if(flag[0] == '1'){
			watchdog_kick_state = RTL_WATCHDOG_KICK;
			/* kick watchdog here*/
			*(volatile unsigned long *)(0xB800311c) |=  1 << 23;
		}else {
			watchdog_kick_state = 0;
		}
		return count;
	}
	
	return -EFAULT;	
}

struct file_operations watchdog_kick_proc_fops = {
        .open           = watchdog_kick_open,
        .write         = watchdog_kick_single_write,
        .read            = seq_read,
        .llseek          = seq_lseek,
        .release        = single_release,
};
#endif
#if defined(USE_INTERRUPT_GPIO)
struct gpio_wps_device
{
	unsigned int name;
};
struct gpio_wps_device priv_gpio_wps_device;
#endif

#ifdef USE_INTERRUPT_GPIO
static int wps_button_push = 0;
#endif

int reset_button_pressed(void)
{	
    ////todo : RTL_BSP_Get_RESET_PIN_DATABASE
	if ((RTL_R32(RESET_PIN_DATABASE) & (1 << RESET_BTN_PIN)))	
		return 0;
	else
		return 1;
}


#ifdef AUTO_CONFIG
static unsigned int		AutoCfg_LED_Blink;
static unsigned int		AutoCfg_LED_Toggle;
static unsigned int		AutoCfg_LED_Slow_Blink;
static unsigned int		AutoCfg_LED_Slow_Toggle;

void autoconfig_gpio_init(void)
{
#if defined(CONFIG_RTL_8198C)
    ////todo : RTL_BSP_Set_AUTOCFG_PIN  RTL_BSP_Set_AUTOCFG_LED_PIN
    /*need set pin mux sel register for gpio mode? ==> check with hw */
	RTL_W32(AUTOCFG_PIN_IOBASE,(RTL_R32(AUTOCFG_PIN_IOBASE)&(~(1 << AUTOCFG_BTN_PIN))));
    #ifdef AUTOCFG_LED_NO
	RTL_W32(AUTOCFG_LED_IOBASE,(RTL_R32(AUTOCFG_LED_IOBASE)&(~(1 << AUTOCFG_LED_PIN))));
    #endif

	// Set GPIO   pin  as input pin for auto config button
	RTL_W32(AUTOCFG_PIN_DIRBASE, (RTL_R32(AUTOCFG_PIN_DIRBASE) & (~(1 << AUTOCFG_BTN_PIN))));

    #ifdef AUTOCFG_LED_NO
	// Set GPIOA ping 3 as output pin for auto config led
	RTL_W32(AUTOCFG_LED_DIRBASE, (RTL_R32(AUTOCFG_LED_DIRBASE) | (1 << AUTOCFG_LED_PIN)));

	// turn off auto config led in the beginning
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | (1 << AUTOCFG_LED_PIN)));
    #endif	
    //printk("LINE: %x d:%x *  %x****R:%x\n",__LINE__,RTL_R32(0xb8b00728),RTL_R32(PCIE_PIN_MUX),RTL_R32(RESET_PIN_DATABASE));
#endif
}


#if defined(CONFIG_RTL_8198C)

void autoconfig_gpio_off(void)
{
    ////todo : RTL_BSP_Set_AUTOCFG_LED_PIN
	////RTLWIFINIC_GPIO_write(AUTOCFG_LED_PIN, 0);
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | (1 << AUTOCFG_LED_PIN)));    
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_on(void)
{
    ////todo : RTL_BSP_Set_AUTOCFG_LED_PIN
	////RTLWIFINIC_GPIO_write(AUTOCFG_LED_PIN, 1);
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));
	AutoCfg_LED_Blink = 0;
}

void autoconfig_gpio_blink(void)
{
        ////leroy todo : RTL_BSP_Set_AUTOCFG_LED_PIN
	////RTLWIFINIC_GPIO_write(AUTOCFG_LED_PIN, 1);
	RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));	
	AutoCfg_LED_Blink = 1;
	AutoCfg_LED_Toggle = 1;
    /*check if 98C support slow blink?*/
	////AutoCfg_LED_Slow_Blink = 0;
}
#endif

#endif // end of AUTO_CONFIG




static void rtl_gpio_timer(unsigned long data)
{
	unsigned int pressed=1;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27)
	struct pid *pid;
#endif

	if(reset_button_pressed() == 0) //mark_es
	{
		pressed = 0;
		//turn off LED0
        #ifdef RESET_LED_NO
		RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) | ((1 << RESET_LED_PIN))));
		#endif
	}
	else
	{
		DPRINTK("Key pressed %d!\n", probe_counter+1);
	}


	if (RTL_R32(AUTOCFG_PIN_DATABASE) & (1 << AUTOCFG_BTN_PIN))
	{
#ifdef USE_INTERRUPT_GPIO
		wps_button_push = 0;
#endif
	}else{
#ifdef USE_INTERRUPT_GPIO
		wps_button_push++;
#endif
	}

	if (probe_state == PROBE_NULL)
	{
		if (pressed)
		{
			probe_state = PROBE_ACTIVE;
			probe_counter++;
		}
		else
			probe_counter = 0;
	}
	else if (probe_state == PROBE_ACTIVE)
	{
		if (pressed)
		{
			probe_counter++;
			if ((probe_counter >=2 ) && (probe_counter <=PROBE_TIME))
			{
				DPRINTK("2-5 turn on led\n");
				//turn on LED0
                #ifdef RESET_LED_NO
                    RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) & (~(1 << RESET_LED_PIN))));
                #endif
			}
			else if (probe_counter >= PROBE_TIME)
			{
				// sparkling LED0
				DPRINTK(">5 \n");
                #ifdef RESET_LED_NO
				if (probe_counter & 1)
				{ 				
					RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) | ((1 << RESET_LED_PIN))));
				}	
				else
				{
					RTL_W32(RESET_LED_DATABASE, (RTL_R32(RESET_LED_DATABASE) & (~(1 << RESET_LED_PIN))));
				}	
                #endif
			}
		}
		else
		{
			if (probe_counter < 2)
			{
				probe_state = PROBE_NULL;
				probe_counter = 0;
				DPRINTK("<2 \n");
				#if defined(CONFIG_RTL865X_SC)
					ResetToAutoCfgBtn = 1;
				#endif
			}
			else if (probe_counter >= PROBE_TIME)
			{
				//reload default
				default_flag = '1';
				//kernel_thread(reset_flash_default, (void *)1, SIGCHLD);
				return;
			}
			else
			{
				DPRINTK("2-5 reset 1\n");
			#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,27)
				kill_proc(1,SIGTERM,1);
			#else
				pid = get_pid(find_vpid(1));
				kill_pid(pid,SIGTERM,1);
			#endif
				DPRINTK("2-5 reset 2\n");
				//kernel_thread(reset_flash_default, 0, SIGCHLD);
				return;
			}
		}
	}

#ifdef AUTO_CONFIG
	if (AutoCfg_LED_Blink==1)
	{
		if (AutoCfg_LED_Toggle) {
            #ifdef AUTOCFG_LED_NO
			RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) | (1 << AUTOCFG_LED_PIN)));
            #endif
		}
		else {
            #ifdef AUTOCFG_LED_NO	
			 RTL_W32(AUTOCFG_LED_DATABASE, (RTL_R32(AUTOCFG_LED_DATABASE) & (~(1 << AUTOCFG_LED_PIN))));		
            #endif
		}
				
		if(AutoCfg_LED_Slow_Blink)
		{
			if(AutoCfg_LED_Slow_Toggle)
				AutoCfg_LED_Toggle = AutoCfg_LED_Toggle;
			else
				AutoCfg_LED_Toggle = AutoCfg_LED_Toggle? 0 : 1;
			
			AutoCfg_LED_Slow_Toggle = AutoCfg_LED_Slow_Toggle? 0 : 1;
		}
		else
			AutoCfg_LED_Toggle = AutoCfg_LED_Toggle? 0 : 1;
		
	}
#endif////end of #ifdef AUTO_CONFIG

	mod_timer(&probe_timer, jiffies + HZ);
}



#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE

#define SYSTEM_CONTRL_DUMMY_REG 0xb8003504

int is_bank2_root()
{
	//boot code will steal System's dummy register bit0 (set to 1 ---> bank2 booting
	//for 8198 formal chip 
	
	if ((RTL_R32(SYSTEM_CONTRL_DUMMY_REG)) & (0x00000001))  // steal for boot bank idenfy
		return 1;
	
	return 0;
}
static int read_bootbank_proc(struct seq_file *s, void *v)
{
	int len;
	char flag='1';

	if (is_bank2_root())  // steal for boot bank idenfy
		flag='2';


	seq_printf(s,"%c\n", flag);

	return 0;
#if 0		
	len = sprintf(page, "%c\n", flag);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;
	return len;
#endif
}

int bootbank_open(struct inode *inode, struct file *file)
{
        return(single_open(file, read_bootbank_proc, NULL));
}

struct file_operations bootbank_proc_fops = {
        .open           = bootbank_open,
        .read            = seq_read,
        .llseek          = seq_lseek,
        .release        = single_release,
};

#endif


#ifdef AUTO_CONFIG
#if defined(USE_INTERRUPT_GPIO)
static irqreturn_t gpio_interrupt_isr(int irq, void *dev_instance, struct pt_regs *regs)
{
	printk("%s %d\n",__FUNCTION__ , __LINE__);
	unsigned int status;

#if defined(CONFIG_RTL_8198C)
	status = REG32(PEFGH_ISR);
#else

#endif

	if((status & BIT(AUTOCFG_BTN_PIN)) != 0)
	{
		wps_button_push = 1; 		

#if defined(CONFIG_RTL_8198C)
		RTL_W32(PEFGH_ISR, BIT(AUTOCFG_BTN_PIN)); 	
#else

#endif

#ifdef IMMEDIATE_PBC
	if(wscd_pid>0)
	{
		rcu_read_lock();
		wscd_pid_Ptr = get_pid(find_vpid(wscd_pid));
		rcu_read_unlock();	

		if(wscd_pid_Ptr){
			printk("(%s %d);signal wscd ;pid=%d\n",__FUNCTION__ , __LINE__,wscd_pid);			
			kill_pid(wscd_pid_Ptr, SIGUSR2, 1);
			
		}
	}
#endif
	}

	return IRQ_HANDLED;
}
#endif

//static int read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
static int read_proc(struct seq_file *s, void *v)
{
//	int len;
	char flag;

#if  defined(USE_INTERRUPT_GPIO)
// 2009-0414		
	if (wps_button_push) {
		flag = '1';
		//wps_button_push = 0; //mark it for select wlan interface by button pressed time		
	}
	else{
		if (RTL_R32(AUTOCFG_PIN_DATABASE) & (1 << AUTOCFG_BTN_PIN)){
			flag = '0';
		}else{
			//printk("wps button be held \n");
			flag = '1';
		}
	}
// 2009-0414		
#else

	if (RTL_R32(AUTOCFG_PIN_DATABASE) & (1 << AUTOCFG_BTN_PIN))
		flag = '0';
	else 
		flag = '1';
	
#endif // CONFIG_RTL865X_KLD					
//	len = sprintf(page, "%c\n", flag);
	seq_printf(s, "%c\n", flag);

#if 0
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;
	return len;
#else
	return 0;
#endif	
}

#ifdef CONFIG_RTL_KERNEL_MIPS16_CHAR
__NOMIPS16
#endif 
static int write_proc(struct file *file, const char *buffer, unsigned long count, void *data)
{
	char flag[20];
//Brad add for update flash check 20080711

	char start_count[10], wait[10];

	if (count < 2)
		return -EFAULT;

	DPRINTK("file: %08x, buffer: %s, count: %lu, data: %08x\n",
		(unsigned int)file, buffer, count, (unsigned int)data);

	if (buffer && !copy_from_user(&flag, buffer, 1)) {
		if (flag[0] == 'E') {
			autoconfig_gpio_init();
		}
		else if (flag[0] == '0')
			autoconfig_gpio_off();
		else if (flag[0] == '1')
			autoconfig_gpio_on();
		else if (flag[0] == '2')
			autoconfig_gpio_blink();
//Brad add for update flash check 20080711
		else if (flag[0] == '4'){
			start_count_time= 1;
			sscanf(buffer, "%s %s", start_count, wait);
			Reboot_Wait = (simple_strtol(wait,NULL,0))*100;
		}

		else
			{}

		return count;
	}
	else
		return -EFAULT;
}
#ifdef IMMEDIATE_PBC
static unsigned long atoi_dec(char *s)
{
	unsigned long k = 0;

	k = 0;
	while (*s != '\0' && *s >= '0' && *s <= '9') {
		k = 10 * k + (*s - '0');
		s++;
	}
	return k;
}
static int read_gpio_wscd_pid(char *page, char **start, off_t off,
				int count, int *eof, void *data)
{
	int len;
	char flag;

	DPRINTK("wscd_pid=%d\n",wscd_pid);	
	
	len = sprintf(page, "%d\n", wscd_pid);
	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len > count) len = count;
	if (len < 0) len = 0;
	return len;
}
static int write_gpio_wscd_pid(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char flag[20];
	char start_count[10], wait[10];
	if (count < 2)
		return -EFAULT;

	DPRINTK("file: %08x, buffer: %s, count: %lu, data: %08x\n",
		(unsigned int)file, buffer, count, (unsigned int)data);

	if (buffer && !copy_from_user(&flag, buffer, 1)) {

		wscd_pid = atoi_dec(buffer);
		DPRINTK("wscd_pid=%d\n",wscd_pid);	
		return count;
	}
	else{
		return -EFAULT;
	}
}
#endif
#endif // AUTO_CONFIG

//static int default_read_proc(char *page, char **start, off_t off, int count, int *eof, void *data)
static int default_read_proc(struct seq_file *s, void *v)
{
#if 0
	int len;

	len = sprintf(page, "%c\n", default_flag);
	if (len <= off+count) *eof = 1;
	  *start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	  return len;
#else
	seq_printf(s, "%c\n", default_flag);
	return 0;
#endif
}

#ifdef CONFIG_RTL_KERNEL_MIPS16_CHAR
__NOMIPS16
#endif 
static int default_write_proc(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	if (count < 2)
		return -EFAULT;
	if (buffer && !copy_from_user(&default_flag, buffer, 1)) {
		return count;
	}
	return -EFAULT;
}

#ifdef CONFIG_PROC_FS

extern struct proc_dir_entry proc_root;

int gpio_single_open(struct inode *inode, struct file *file)
{
        return(single_open(file, read_proc, NULL));
}

static ssize_t gpio_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return write_proc(file, userbuf,count, off);
}


struct file_operations gpio_proc_fops = {
        .open           = gpio_single_open,
	 .write		= gpio_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};

int load_default_single_open(struct inode *inode, struct file *file)
{
        return(single_open(file, default_read_proc, NULL));
}

static ssize_t load_default_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return default_write_proc(file, userbuf,count, off);
}

struct file_operations load_default_proc_fops = {
        .open           = load_default_single_open,
	 .write           = load_default_single_write,
        .read            = seq_read,
        .llseek          = seq_lseek,
        .release        = single_release,
};





#endif

#if 0 // example
struct proc_dir_entry *hostap_proc;

static int __init hostap_init(void)
{
	if (init_net.proc_net != NULL) {
		hostap_proc = proc_mkdir("rtl865x", &proc_root);
		if (!hostap_proc)
			printk(KERN_WARNING "Failed to mkdir "
			       "/proc/rtl865x\n");
	} else
		hostap_proc = NULL;

	return 0;
}

static void __exit hostap_exit(void)
{
	if (hostap_proc != NULL) {
		hostap_proc = NULL;
		remove_proc_entry("rtl865x", &proc_root);
	}
}
#endif

static int write_watchdog_reboot(struct file *file, const char *buffer,
				unsigned long count, void *data)
{
	char tmp[16];

	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(tmp, buffer, 8)) {	
		if (tmp[0] == '1') {
			local_irq_disable();	
			printk("reboot...\n");
			*(volatile unsigned long *)(0xB800311c)=0; /*this is to enable 865xc watch dog reset*/
			for(;;);
		}

		return count;
	}
	return -EFAULT;
}
int watchdog_reboot_open(struct inode *inode, struct file *file)
{
        return(single_open(file, read_proc, NULL));
}

struct file_operations watchdog_reboot_proc_fops = {
        .open           = watchdog_reboot_open,
	 .write           = write_watchdog_reboot,
        .read            = seq_read,
        .llseek          = seq_lseek,
        .release        = single_release,
};

int __init rtl_gpio_init(void)
{
//	struct proc_dir_entry *res=NULL;

	printk("Realtek GPIO Driver for Flash Reload Default\n");

#if  defined(CONFIG_RTL_8198C)
    ////todo : board related config.
    ////todo : logical function : set pin mux & jtag enable or not.

	#ifdef CONFIG_USING_JTAG
		////leroy todo////RTL_W32(RTL_GPIO_MUX, (RTL_R32(RTL_GPIO_MUX) | RTL_GPIO_MUX_GPIOA0_1));
	#else
		////leroy todo////RTL_W32(RTL_GPIO_MUX, (RTL_R32(RTL_GPIO_MUX) | RTL_GPIO_MUX_POCKETAP_DATA));
		RTL_W32(RTL_GPIO_MUX4, (RTL_R32(RTL_GPIO_MUX4) | (RTL_GPIO_MUX4_DATA))); 
		RTL_W32(RTL_GPIO_MUX5, (RTL_R32(RTL_GPIO_MUX5) | (RTL_GPIO_MUX5_DATA))); 
	#endif
#endif // #if defined(CONFIG_RTL_8198C)
       
    ////todo : logical function (+): reset pin & reset led : RTL_BSP_Set_Reset_PIN
    ////: RTL_BSP_Set_Reset_LEN
	RTL_W32(RESET_PIN_IOBASE, (RTL_R32(RESET_PIN_IOBASE) & (~(1 << RESET_BTN_PIN))));
	RTL_W32(RESET_PIN_DIRBASE, (RTL_R32(RESET_PIN_DIRBASE) & (~(1 << RESET_BTN_PIN))));

    #ifdef RESET_LED_NO
	// Set GPIOA ping 2 as output pin for reset led
    RTL_W32(RESET_LED_IOBASE, (RTL_R32(RESET_LED_IOBASE) | (((1 << RESET_LED_PIN)))));
	RTL_W32(RESET_LED_DIRBASE, (RTL_R32(RESET_LED_DIRBASE) | ((1 << RESET_LED_PIN))));
    #endif
    //// todo : logical function (-): reset pin & reset led
    
    #ifdef AUTO_CONFIG
#if 0	
	res = create_proc_entry("gpio", 0, NULL);
	if (res) {
		res->read_proc = read_proc;
		res->write_proc = write_proc;
	}
	else {
		printk("Realtek GPIO Driver, create proc failed!\n");
	}
#else

	proc_create_data("gpio", 0, &proc_root,
			 &gpio_proc_fops, NULL);

#endif


    #ifdef	USE_INTERRUPT_GPIO
    #ifdef  IMMEDIATE_PBC
	res = create_proc_entry("gpio_wscd_pid", 0, NULL);
	if (res)
	{
		res->read_proc = read_gpio_wscd_pid;
		res->write_proc = write_gpio_wscd_pid;
		DPRINTK("create gpio_wscd_pid OK!!!\n\n");
	}
	else{
		printk("create gpio_wscd_pid failed!\n\n");
	}
    #endif	
    #endif


		
    #if defined(USE_INTERRUPT_GPIO)
	RTL_R32(AUTOCFG_PIN_IMR) |= (0x01 << (AUTOCFG_BTN_PIN-16)*2); // enable interrupt in falling-edge	
	if (request_irq(GPIO_IRQ_NUM, gpio_interrupt_isr, IRQF_SHARED, "rtl_gpio", (void *)&priv_gpio_wps_device)) {
		printk("gpio request_irq(%d) error!\n", GPIO_IRQ_NUM);		
   	}
    #endif
		
    #endif ////end of AUTO_CONFIG


#if 0
	res = create_proc_entry("load_default", 0, NULL);
	if (res) {
		res->read_proc = default_read_proc;
		res->write_proc = default_write_proc;
	}
#else	

	proc_create_data("load_default", 0, &proc_root,
			 &load_default_proc_fops, NULL);
#endif

	proc_create_data("watchdog_reboot", 0, &proc_root,
			&watchdog_reboot_proc_fops, NULL);


#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE	
	proc_create_data("bootbank", 0, &proc_root,
			&bootbank_proc_fops, NULL);
#endif

	
			
	init_timer(&probe_timer);
	probe_counter = 0;
	probe_state = PROBE_NULL;
	probe_timer.expires = jiffies + HZ;
	probe_timer.data = (unsigned long)NULL;
	probe_timer.function = &rtl_gpio_timer;
	mod_timer(&probe_timer, jiffies + HZ);
	autoconfig_gpio_init(); //always init wps gpio

#if defined(CONFIG_RTL_8198C) && defined(CONFIG_SERIAL_RTL_UART1)
	{
		int portnum=0x1;
		//System register Table
#define SYS_BASE 0xb8000000
#define SYS_INT_STATUS (SYS_BASE +0x04)
#define SYS_HW_STRAP   (SYS_BASE +0x08)
#define SYS_BOND_OPTION (SYS_BASE+0x0c) //new
#define SYS_CLKMANAGE (SYS_BASE +0x10)
		 
#define SYS_LX_CTRL   (SYS_BASE +0x14)
#define SYS_CLKMANAGE2   (SYS_BASE +0x18)
		 
#define SYS_PIN_MUX_SEL1 (SYS_BASE +0x100)
#define SYS_PIN_MUX_SEL2 (SYS_BASE +0x104)
#define SYS_PIN_MUX_SEL3 (SYS_BASE +0x108)
#define SYS_PIN_MUX_SEL4 (SYS_BASE +0x10c)
#define SYS_PIN_MUX_SEL5 (SYS_BASE +0x120)
		 
		 
		 
		/*
		UART pin:  VCC, GND,CTS, RTS,TX,RX
		 
		256 have JTAG, UART0 delicate pin.
		 
		UART0
		RX	:P5 RxD7:
		TX	:P5 TxD4
		 
		UART1
		JTAG CLK J19.6:  RXD : P5 GTXC: T2
		JTAG TMS J19.8:  CTS : P5 TXD7
		JTAG TDI J19.10: RTS : P5 TXD6
		JTAG TDO 19.2  : TXD : P5 TXD5
		 
		
		UART2
		U0RX: RXD: T0
		U0TX: TXD
		U0CTS:CTS(I)
		U0RTS:RTS(O)
		 
		*/
		 
		 
		 
		 if(portnum==1)
		 {
		  //UART1 using P5 pin.
		  REG32(SYS_PIN_MUX_SEL4)&=~((0xf<<3)|(7<<7)|(7<<10)|(0xf<<13));
		  REG32(SYS_PIN_MUX_SEL4)|=(0x7<<3)|(7<<7)|(7<<10)|(0x8<<13);
		  printk("value 0x%x\n",(0x7<<3)|(7<<7)|(7<<10)|(0x8<<13));
		  printk("value reg 0x%x\n",REG32(SYS_PIN_MUX_SEL4));
		 } 
		 
		 if(portnum==0x11)
		 {
		  //UART1 using JTAG pin, JTAG RESET not belong UART1
		  printk("Uart1 using jtag pin\n");
		  REG32(SYS_PIN_MUX_SEL2)&=~((0xf)|(0xf<<8)|(0xf<<12)|(0xf<<16));
		  REG32(SYS_PIN_MUX_SEL2)|=(2)|(2<<8)|(2<<12)|(2<<16);	  
		  printk("value 0x%x\n",(2)|(2<<8)|(2<<12)|(2<<16));;
		  printk("value reg 0x%x\n",REG32(SYS_PIN_MUX_SEL4));
		 }
		 
		 if(portnum==2)
		 {
		  //UART2
		  REG32(SYS_PIN_MUX_SEL2)&=~(0x1<<22)|(3<<23)|(3<<25);	
		 }	 
	}
#endif

#ifdef  CONFIG_RTL_USERSPACE_WTDOG
	proc_create_data("watchdog_start", 0, &proc_root,
			&watchdog_start_proc_fops, NULL);
			
	proc_create_data("watchdog_kick", 0, &proc_root,
			&watchdog_kick_proc_fops, NULL);
#endif
	return 0;
}


static void __exit rtl_gpio_exit(void)
{
	printk("Unload Realtek GPIO Driver \n");
	del_timer_sync(&probe_timer);
}

module_exit(rtl_gpio_exit);
module_init(rtl_gpio_init);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("GPIO driver for Reload default");

