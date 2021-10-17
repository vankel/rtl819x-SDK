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
#include <rtl8196x/asicregs.h>
#include "main.h"
#if defined(CONFIG_POST_ENABLE)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
int POSTRW_API(void)
{
  	unsigned int Test_Start_offset=0x0;  
#if defined(CONFIG_D8_16)  	
unsigned int Test_Size=0x00800000;  //8MB	
#else
unsigned int Test_Size=0x01000000;  //16MB	
#endif
	  unsigned short test_pattern_mode=0;//"0"=0x5a,"1"=0xa5,
	  unsigned int test_cnt=0;    
	  unsigned int test_result=1;  //"1":Pass,"0":fail

	  //unsigned int DRAM_ADR = 0xA0000000; //uncache address
    unsigned int DRAM_ADR = 0x80000000; //cache address
    unsigned int DRAM_Start_Test_ADR,DRAM_End_Test_ADR; 	  
    unsigned int DRAM_pattern1 = 0xA5A5A5A5;
	  unsigned int DRAM_pattern0 = 0x5A5A5A5A;
	  unsigned int DRAM_pattern;

	  DRAM_Start_Test_ADR= DRAM_ADR + Test_Start_offset; 
	  DRAM_End_Test_ADR=  DRAM_Start_Test_ADR+Test_Size; 
	
	for(test_pattern_mode=1 ;test_pattern_mode<=2;test_pattern_mode++)
	{
		
		if(test_pattern_mode%2==0)
		{
			DRAM_pattern=DRAM_pattern0;
		}
		else
		{
			DRAM_pattern=DRAM_pattern1;
		}
		//prom_printf("\nPOST(%d),Pattern:0x%x=> ",test_pattern_mode,DRAM_pattern);
		

		/* Set Data Loop*/
		/* 	Test from 1~16MB ,except 7~8MB*/
		for (test_cnt= 0; test_cnt < Test_Size;test_cnt+=0x00100000 )
		{	 	
		     if ((test_cnt==0x0 )||(test_cnt==0x00400000 ))//skip DRAM size from 0~1MB and 4~5MB
		     {				     
					  continue;
		     }				  
			 memset((unsigned int *) (DRAM_Start_Test_ADR+ test_cnt),DRAM_pattern,(unsigned int)0x00100000 );
		}	 

		/*Verify Data Loop*/
		 for(test_cnt=0;test_cnt<Test_Size;test_cnt+=4)
		 {
		 	 if(((test_cnt >= 0x0 ) && (test_cnt <=0x00100000))||((test_cnt >= 0x00400000 ) && (test_cnt <=0x00500000)))
			 	 continue;

			 if (READ_MEM32(DRAM_Start_Test_ADR+test_cnt) != DRAM_pattern)//Compare FAIL
		 	  {											
						//prom_printf("\nDRAM POST Fail at addr:0x%x!!!\n\n",(DRAM_Start_Test_ADR+test_cnt) );
						test_result=0;
						return 0;					
		 	  }
		 }//end of test_cnt

		   if (test_result)
			  	prom_printf("OK\n");
			  else
			  	prom_printf("Fail\n");
		 
	}//end of test_pattern_mode
	 
	  prom_printf("\n\n");
	   return 1;
  }//end of POSTRW_API
#endif
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
#if defined(CONFIG_POST_ENABLE)
	int post_test_result=1;
#endif
	Init_GPIO();
        #if defined(CONFIG_POST_ENABLE)
	if(POSTRW_API()==0)
		post_test_result=0;
        #endif	
#endif
			

	showBoardInfo();
#if defined(CONFIG_POST_ENABLE)
	if(!post_test_result)
        {
          return;

        }
#else
#endif

#if defined(CONFIG_RTL8196E_ULINKER_BOOT_LED)
	power_on_led();		// add by jimmy
#endif

#ifdef CONFIG_RTL8196E
	if ((REG32(BOND_OPTION) & BOND_ID_MASK) == BOND_8196ES) {
		rtl8196e_gpio_init();
	}
#endif

	return_addr=0;
	ret=check_image	(&header,&setting_header);

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
	cpu_speed = check_cpu_speed();	
	prom_printf("%s",((*(volatile unsigned int *)(0xb8000008)) & (0x1<<23))?"Reboot Result from Watchdog Timeout!\n":" ");

#ifdef CONFIG_RTL8196E_KLD
	prom_printf("\n---RealTek(RTL8196E-kld)at %s %s [%s](%dMHz)\n",		BOOT_CODE_TIME,B_VERSION, "16bit", cpu_speed);	
#elif defined(CONFIG_RTL8196E)
	prom_printf("\n---RealTek(RTL8196E)at %s %s [%s](%dMHz)\n",		BOOT_CODE_TIME,B_VERSION, "16bit", cpu_speed);	
#else
	prom_printf("\n---RealTek(RTL8196D)at %s %s [%s](%dMHz)\n",		BOOT_CODE_TIME,B_VERSION, "16bit", cpu_speed);	
#endif
}
