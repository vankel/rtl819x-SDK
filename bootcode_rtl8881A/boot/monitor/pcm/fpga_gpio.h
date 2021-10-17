#ifndef _FPGA_GPIO_H
#define _FPGA_GPIO_H

#include <linux/types.h>

#define CONFIG_RTK_VOIP_DRIVERS_CODEC_ALC5621
//#define CONFIG_RTK_VOIP_GPIO_8954C_TESTCHIP
#define CONFIG_RTK_VOIP_GPIO_8196D_TESTCHIP

#if 1	//0: kernel used(udelay), 1: test program used(for(;;)) 
#define __test_program_used__
#else
#define __kernel_used__
#endif

/*********************** I2C data struct ********************************************/
struct rtl8954C_i2c_dev_s
{
	//unsigned int i2c_reset;		//output
	unsigned int sclk;		//output
	unsigned int sdio;		//input or output	
};	

typedef struct rtl8954C_i2c_dev_s rtl8954C_i2c_dev_t;
/*******************************************************************/


#if defined(CONFIG_RTK_VOIP_DRIVERS_CODEC_WM8510)
//----------------------- WM8510 codec ----------------------------------------------
#define I2C_RATING_FACTOR	10	//Adjust I2C read and write rating.
//There is an I2C protocal.
#define _I2C_WM8510_ 	//GPIO pin
//cli() protection for kernel used. Don't define MACRO _WM8510_CLI_PROTECT in test program.
//#define _WM8510_CLI_PROTECT

#elif defined(CONFIG_RTK_VOIP_DRIVERS_CODEC_ALC5621)
//----------------------- ALC5621 codec ----------------------------------------------
#define I2C_RATING_FACTOR	(3)	//Adjust I2C read and write rating.
//There is an I2C protocal.
#define _I2C_ALC5621_ 	//GPIO pin
//cli() protection for kernel used. Don't define MACRO _WM8510_CLI_PROTECT in test program.
//#define _ALC5621_CLI_PROTECT
#endif

/*#define GPIO_BASE (0xb8003500)

#define PABCD_CNR (GPIO_BASE + 0x00)
#define PABCD_DIR (GPIO_BASE + 0x08)
#define PABCD_DAT (GPIO_BASE + 0x0C)
#define PABCD_ISR (GPIO_BASE + 0x10)
#define PAB_IMR   (GPIO_BASE + 0x14)
#define PCD_IMR   (GPIO_BASE + 0x18)*/


/*************** Define RTL8954C GPIO Register Set ************************/
#define GPABCDCNR		0xB8003500
#define GPABCDPTYP		0xB8003504
#define	GPABCDDIR		0xB8003508
#define	GPABCDDATA		0xB800350C
#define	GPABCDISR		0xB8003510
#define	GPABIMR			0xB8003514
#define	GPCDIMR			0xB8003518
#define GPEFGHCNR		0xB800351C
#define GPEFGHPTYP		0xB8003520
#define	GPEFGHDIR		0xB8003524
#define	GPEFGHDATA		0xB8003528
#define	GPEFGHISR		0xB800352C
#define	GPEFIMR			0xB8003530
#define	GPGHIMR			0xB8003534
/**************************************************************************/

/* define GPIO port */
enum GPIO_PORT
{
	GPIO_PORT_A = 0,
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_D,
	GPIO_PORT_E,
	GPIO_PORT_F,
	GPIO_PORT_G,
	GPIO_PORT_H,
	GPIO_PORT_MAX,
};


/* define GPIO control pin */
enum GPIO_CONTROL
{
	GPIO_CONT_GPIO = 0,
	GPIO_CONT_PERI = 0x1,
};




/* define GPIO direction */
enum GPIO_DIRECTION
{
	GPIO_DIR_IN = 0,
	GPIO_DIR_OUT,
};

/* define GPIO Interrupt Type */
enum GPIO_INTERRUPT_TYPE
{
	GPIO_INT_DISABLE = 0,
	GPIO_INT_FALLING_EDGE,
	GPIO_INT_RISING_EDGE,
	GPIO_INT_BOTH_EDGE,
};


/*
 * Every pin of GPIO port can be mapped to a unique ID. All the access to a GPIO pin must use the ID.
 * This macro is used to map the port and pin into the ID.
 */
#define GPIO_ID(port,pin) ((uint32_t)port<<16|(uint32_t)pin)

/* This is reversed macro. */
#define GPIO_PORT(id) (id>>16)
#define GPIO_PIN(id) (id&0xffff)


int32_t _rtl8954C_initGpioPin(uint32_t gpioId, enum GPIO_CONTROL dedicate, 
                                           enum GPIO_DIRECTION, 
                                           enum GPIO_INTERRUPT_TYPE interruptEnable );
int32_t _rtl8954C_getGpioDataBit( uint32_t gpioId, uint32_t* data );
int32_t _rtl8954C_setGpioDataBit( uint32_t gpioId, uint32_t data );



#ifdef _I2C_ALC5621_
#if defined(CONFIG_RTK_VOIP_GPIO_IPP_100)
#define GPIO_I2C "D"
#define SCLK_PIN	GPIO_ID(GPIO_PORT_D,10)	//output
#define SDIO_PIN	GPIO_ID(GPIO_PORT_D,6)	//output or input
#elif defined(CONFIG_RTK_VOIP_GPIO_IPP_101)
#define GPIO_I2C "C"
#define SCLK_PIN	GPIO_ID(GPIO_PORT_C,13)	//output
#define SDIO_PIN	GPIO_ID(GPIO_PORT_C,14)	//output or input
#elif defined(CONFIG_RTK_VOIP_GPIO_8954C_TESTCHIP)
#define GPIO_I2C "B/C"
#define SCLK_PIN	GPIO_ID(GPIO_PORT_B,0)	//output
#define SDIO_PIN	GPIO_ID(GPIO_PORT_C,4)	//output or input
#elif defined(CONFIG_RTK_VOIP_GPIO_8196D_TESTCHIP)
#define GPIO_I2C "B"
#define SCLK_PIN	GPIO_ID(GPIO_PORT_B,5)	//output
#define SDIO_PIN	GPIO_ID(GPIO_PORT_B,6)	//output or input
#endif
#endif


/* Register access macro (REG*()).*/
#define REG32(reg) 			(*((volatile uint32_t *)(reg)))
#define REG16(reg) 			(*((volatile uint16_t *)(reg)))
#define REG8(reg) 			(*((volatile uint8_t *)(reg)))


#endif/*__GPIO__*/



