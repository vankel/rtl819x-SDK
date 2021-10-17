#ifdef _AIPC_CPU_
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/delay.h>

//#include "gpio.h"
#include "bspchip.h"

#include "./include/aipc_reg.h"
#include "./include/soc_type.h"
#include "./include/aipc_mem.h"
#include "./include/aipc_debug.h"


extern unsigned int SOC_ID, SOC_BOND_ID;
extern unsigned int GPIO_CTRL_0, GPIO_CTRL_1, GPIO_CTRL_2, GPIO_CTRL_4;
unsigned int soc_set_pcm_fs = 0;

#ifdef AIPC_MODULE_VOIP_SET_PCM_FS
// 6266
#define REG_IO_MODE  (0xBB023018)
#define BIT_I2C0  BIT(13)
#define PIN_FS    26
#define PIN_PCM   27

// 6318
#define REG_8685_PIN_STATUS         (0xB8000100)
#define BIT_BONDTYE                 BIT(27)

#define REG_8685_PINMUX_0           (0xB8000108)
#define BIT_UART1_SEL               BIT(11)

#define REG_8685_PINMUX_1			(0xB800010C)
#define BIT_VOIP_SPI_DIR_256_PIN	BIT(16)
#define BIT_VOIP_SPI_DIR_176_PIN	BIT(15)

void
aipc_module_voip_set_pcm_fs(void)
{
#ifdef CONFIG_RTK_VOIP_PLATFORM_8686
	unsigned int rtl8685_bond_type = 0;
	
	if (SOC_ID != 0x0371) {
		// 6318
		if (SOC_ID == 0x0561) {
			rtl8685_bond_type = REG32(REG_8685_PIN_STATUS);
			
			REG32(REG_8685_PINMUX_0) &= ~BIT_UART1_SEL;
			SDEBUG( "REG_8685_PINMUX_0 0x%08x\n" , REG32(REG_8685_PINMUX_0));
			
			if ((rtl8685_bond_type & BIT_BONDTYE) == BIT_BONDTYE) {
				REG32(REG_8685_PINMUX_1) |= BIT_VOIP_SPI_DIR_256_PIN;
			}
			else {
				REG32(REG_8685_PINMUX_1) |= BIT_VOIP_SPI_DIR_176_PIN;
			}
			SDEBUG( "REG_8685_PIN_STATUS 0x%08x\n" , REG32(REG_8685_PIN_STATUS) );
			SDEBUG( "REG_8685_PINMUX_1 0x%08x\n" , REG32(REG_8685_PINMUX_1) );
		}
		// 6266
		else {
			if (SOC_BOND_ID == CHIP_96) {
				// disable I2C0
				SDEBUG( "REG IO_MODE %x\n" , REG32(REG_IO_MODE) );
				REG32(REG_IO_MODE) &= ~(BIT_I2C0);	
				SDEBUG( "REG IO_MODE %x\n" , REG32(REG_IO_MODE) );
			}

			// disalbe GPIO 26 , 27 for FS and PCM respectively
			REG32(GPIO_CTRL_2) &= ~(1<<(PIN_FS  - 1));
			REG32(GPIO_CTRL_2) &= ~(1<<(PIN_PCM - 1));
		}
	}
#endif
}

EXPORT_SYMBOL(aipc_module_voip_set_pcm_fs);
#endif

#ifdef CONFIG_RTL8686_SLIC_RESET
#if (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM < 0) || (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM > 71)
	#error "Wrong GPIO Number Setting"
#endif

#define SWITCH_GPIO_CTRL_0	(0xBB000100)
	#define SWITCH_GPIO_DATA_00_31	(SWITCH_GPIO_CTRL_0+0x0)
	#define SWITCH_GPIO_DATA_32_63	(SWITCH_GPIO_CTRL_0+0x4)
	#define SWITCH_GPIO_DATA_64_71	(SWITCH_GPIO_CTRL_0+0x8)

#define SWITCH_GPIO_CTRL_1	(0xBB00010C)

#define SWITCH_GPIO_CTRL_2	(0xBB000118)
	#define SWITCH_GPIO_ENABLE_00_31	(SWITCH_GPIO_CTRL_2+0x0)
	#define SWITCH_GPIO_ENABLE_32_63	(SWITCH_GPIO_CTRL_2+0x4)
	#define SWITCH_GPIO_ENABLE_64_71	(SWITCH_GPIO_CTRL_2+0x8)

#define SWITCH_GPIO_CTRL_3	(0xBB000124)

#define SWITCH_GPIO_CTRL_4	(0xBB0001D4)
	#define SWITCH_GPIO_DIR_00_31		(SWITCH_GPIO_CTRL_4+0x0)
	#define SWITCH_GPIO_DIR_32_63		(SWITCH_GPIO_CTRL_4+0x4)
	#define SWITCH_GPIO_DIR_64_71		(SWITCH_GPIO_CTRL_4+0x8)

void
aipc_module_voip_slic_reset_0371(void)
{
	int count = 0;
	unsigned int gpio_data = 0, gpio_dir = 0 , gpio_enable = 0;

	SDEBUG( "\n" );

	if ((CONFIG_RTL8686_SLIC_RESET_GPIO_NUM >= 0) && (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM <= 31)){
		count = CONFIG_RTL8686_SLIC_RESET_GPIO_NUM-1;

		gpio_data   = SWITCH_GPIO_DATA_00_31 ;
		gpio_dir    = SWITCH_GPIO_DIR_00_31 ;
		gpio_enable = SWITCH_GPIO_ENABLE_00_31 ;
	}
	else if ((CONFIG_RTL8686_SLIC_RESET_GPIO_NUM >= 32) && (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM <= 63)){
		count = CONFIG_RTL8686_SLIC_RESET_GPIO_NUM % 32;

		gpio_data   = SWITCH_GPIO_DATA_32_63 ;
		gpio_dir    = SWITCH_GPIO_DIR_32_63 ;
		gpio_enable = SWITCH_GPIO_ENABLE_32_63 ;
	}
	else if ((CONFIG_RTL8686_SLIC_RESET_GPIO_NUM >= 64) && (CONFIG_RTL8686_SLIC_RESET_GPIO_NUM <= 71)){
		count = CONFIG_RTL8686_SLIC_RESET_GPIO_NUM % 32;
	
		gpio_data   = SWITCH_GPIO_DATA_64_71 ;
		gpio_dir    = SWITCH_GPIO_DIR_64_71 ;
		gpio_enable = SWITCH_GPIO_ENABLE_64_71 ;
	}
	else{
		SDEBUG("Wrong GPIO Number Setting\n");
	}

	SDEBUG( "Pull low GPIO %d in switch. count = %d\n" , CONFIG_RTL8686_SLIC_RESET_GPIO_NUM , count );
    SDEBUG( "data %x = 0x%x dir %x = 0x%x enable %x = 0x%x\n" , 
    		gpio_data    ,  REG32(gpio_data) , 
			gpio_dir     ,  REG32(gpio_dir) , 
    		gpio_enable  ,  REG32(gpio_enable));

    REG32(gpio_data)	&= ~(1<<(count));  //data
    REG32(gpio_dir)		|=  (1<<(count));  //dir
    REG32(gpio_enable)	|=  (1<<(count));  //enable

	mdelay(RESET_SLIC_DELAY_TIME);

    REG32(gpio_data) 	|=  (1<<(count));
    
	SDEBUG( "Pull high GPIO %d in switch\n" , CONFIG_RTL8686_SLIC_RESET_GPIO_NUM );
    SDEBUG( "data %x = 0x%x dir %x = 0x%x enable %x = 0x%x\n" , 
    		gpio_data    ,  REG32(gpio_data) , 
			gpio_dir     ,  REG32(gpio_dir) , 
    		gpio_enable  ,  REG32(gpio_enable));
}


void
aipc_module_voip_slic_reset_others(void)
{
	int slic_reset = CONFIG_RTL8686_SLIC_RESET_GPIO_NUM;
	int value = 0;

	SDEBUG( "\n" );
    SDEBUG( "Pull low GPIO %d in switch\n" , CONFIG_RTL8686_SLIC_RESET_GPIO_NUM );
	
	gpioClear( slic_reset );
	gpioConfig( slic_reset, GPIO_FUNC_OUTPUT );
	value = gpioRead( slic_reset );
    SDEBUG( "GPIO value = %d\n" , value );
	
	mdelay(RESET_SLIC_DELAY_TIME);
    
    SDEBUG( "Pull high GPIO %d in switch\n" , CONFIG_RTL8686_SLIC_RESET_GPIO_NUM );
	gpioSet( slic_reset );
	
	value = gpioRead( slic_reset );
    SDEBUG( "GPIO value = %d\n" , value );
}

void
aipc_module_voip_slic_reset(void)
{
	if(SOC_ID==0x0371){
		aipc_module_voip_slic_reset_0371();
	} else {
		aipc_module_voip_slic_reset_others();
	}
}
EXPORT_SYMBOL(aipc_module_voip_slic_reset);
#endif


#ifdef  AIPC_MODULE_DISALBE_WDOG
#define WDOG_ENABLE 0xb8003224
void
aipc_module_disable_wdog(void)
{
#ifdef CONFIG_RTK_VOIP_PLATFORM_8686
	REG32(WDOG_ENABLE) &= ~WATCHDOG_ENABLE;
//	SDEBUG("WDOG Setting 0x%08x\n" , REG32(WDOG_ENABLE));
#else
	printk("%s: this api is not implement!\n", __FUNCTION__);
#endif
}
#endif

#ifdef AIPC_MODULE_VOIP_IP_ENABLE
void
aipc_module_voip_ip_enable(void)
{
#ifdef CONFIG_RTK_VOIP_PLATFORM_8686
	REG32(R_AIPC_IP_ENABLE_CTRL) |= BIT_IP_ENABLE_VOIPFFT | \
									BIT_IP_ENABLE_VOIPACC | \
									BIT_IP_ENABLE_GDMA1 | \
									BIT_IP_ENABLE_GDMA0 | \
									BIT_IP_ENABLE_PCM | \
									BIT_IP_ENABLE_GMAC | \
									BIT_IP_ENABLE_PREI_VOIP;

	mdelay(100);
#endif
}
#endif

#if 0
#define PCM_CTR         0xb8008000
#define BIT_PRECISE     BIT(10)

static void
aipc_module_set_pcm_precise(int enable)
{
	if (enable)
		REG32(PCM_CTR) |=  BIT_PRECISE;
	else
		REG32(PCM_CTR) &= ~BIT_PRECISE;
}
#endif

#endif

