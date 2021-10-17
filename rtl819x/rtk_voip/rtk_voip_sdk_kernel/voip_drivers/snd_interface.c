#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>	/* jiffies is defined */
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/string.h>

#ifdef CONFIG_RTL8686
#define ZSI_ISI_PIN_SEL			( ( volatile unsigned int * )0xbb023018 )
#endif

#define ZSI_ISI_PIN_EN			( ( volatile unsigned int * )0xbb000174 )
#define ZSI_ISI_PIN_MODE		( ( volatile unsigned int * )0xB8000600 )

#if 0
#ifdef CONFIG_RTL8685
#define 
#endif
#endif

static void __init slicInterface(){
	unsigned int regValue = 0;
	printk("[%s] slic isi/zsi enable\n", __FUNCTION__);
#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_ZSI 
#ifdef CONFIG_RTL8686
	regValue = *ZSI_ISI_PIN_SEL;
	regValue = regValue & 0xfff9ffff; 
	regValue |= (1<<16);
	*ZSI_ISI_PIN_SEL = regValue;
#endif
	
	regValue = *ZSI_ISI_PIN_EN;
	regValue |= (1<<2);
	*ZSI_ISI_PIN_EN = regValue;

	regValue = *ZSI_ISI_PIN_MODE;//writel(0xB8000600 , readl(0xB8000600) | (1<<26) | (1<<25) );	
#ifdef CONFIG_RTL8686
	regValue |= (1<<26);
#endif
#ifdef CONFIG_RTL8685
	regValue &= 0xf7ffffff;
	regValue |= (1<<27);
#endif
	*ZSI_ISI_PIN_MODE = regValue;
#if 0
	regValue = pcm_inl(0xb8008000);
	regValue |= ZSILBE;
	pcm_outl(0xb8008000, regValue);
#endif
#endif


#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_ISI 
#ifdef CONFIG_RTL8686
	regValue = *ZSI_ISI_PIN_SEL;
	regValue = regValue & 0xfff9ffff; 
	regValue |= (1<<15);
	*ZSI_ISI_PIN_SEL = regValue;
#endif
	regValue = *ZSI_ISI_PIN_EN;
	regValue |= (1<<2);
	*ZSI_ISI_PIN_EN = regValue;
	
	regValue = *ZSI_ISI_PIN_MODE;//writel(0xB8000600 , readl(0xB8000600) | (1<<26) | (1<<25) );	
#ifdef CONFIG_RTL8686
	regValue |= (1<<26);
	regValue |= (1<<25);
#endif
#ifdef CONFIG_RTL8685
	regValue &= 0xf3ffffff;
	regValue |= (1<<26);
	regValue |= (1<<27);

#endif
	*ZSI_ISI_PIN_MODE = regValue;

#if 0
	regValue = pcm_inl(0xbb023018);
	regValue = regValue & 0xfff9ffff; 
	regValue |= (1<<15);
	pcm_outl(0xbb023018, regValue);
	
	regValue = pcm_inl(0xbb000174);
	regValue |= (1<<2);
	pcm_outl(0xbb000174 , regValue);

	pcm_outl(0xB8000600 , pcm_inl(0xB8000600) | (1<<26) | (1<<25) );	

	regValue = pcm_inl(0xb8008000);
	regValue |= ISILBE;
	pcm_outl(0xb8008000, regValue);
	printk("0xb8008000 = [%04x]\n", pcm_inl(0xb8008000));
#endif
#endif
}

module_init(slicInterface);
