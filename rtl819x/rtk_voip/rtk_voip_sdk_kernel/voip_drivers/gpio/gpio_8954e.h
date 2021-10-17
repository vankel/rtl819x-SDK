/*
* Copyright c                  Realtek Semiconductor Corporation, 2012  
* All rights reserved.
* 
* Program : GPIO Header File 
* Abstract : 
* Author :                
* 
*/

 
#ifndef __GPIO_8954E_H__
#define __GPIO_8954E_H__
//#include <gpio.h>
/*==================== FOR RTL8954E EVB gpio pin ==================*/

//#define PIN_CSEN 27//GPIO_D_3
#define PIN_RESET1	GPIO_ID(GPIO_PORT_C,0)  //output, for 8198C 256pin QA board

/* LED */ 
#if 0
#define LED_TEL_GREEN	35
#define LED_TEL_RED  	36

#define PIN_VOIP0_LED	LED_TEL_GREEN
#define PIN_VOIP1_LED	LED_TEL_RED
#endif

#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxE

/******** GPIO define ********/

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
	GPIO_PORT_UNDEF,
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
	GPIO_DIR_OUT =1,
};

/* define GPIO Interrupt Type */
enum GPIO_INTERRUPT_TYPE
{
	GPIO_INT_DISABLE = 0,
	GPIO_INT_FALLING_EDGE,
	GPIO_INT_RISING_EDGE,
	GPIO_INT_BOTH_EDGE,
};

/*************** Define RTL8954E Family GPIO Register Set ************************/
#define GPIO_BASE		0xB8003500

#define GPABCDCNR		(GPIO_BASE+0x00)
#define	GPABCDDIR		(GPIO_BASE+0x08)
#define	GPABCDDATA		(GPIO_BASE+0x0C)
#define	GPABCDISR		(GPIO_BASE+0x10)
#define	GPABIMR			(GPIO_BASE+0x14)
#define	GPCDIMR			(GPIO_BASE+0x18)

#define GPEFGHCNR		(GPIO_BASE+0x1C)
#define	GPEFGHDIR		(GPIO_BASE+0x24)
#define	GPEFGHDATA		(GPIO_BASE+0x28)
#define	GPEFGHISR		(GPIO_BASE+0x2C)
#define	GPEFIMR			(GPIO_BASE+0x30)
#define	GPGHIMR			(GPIO_BASE+0x34)

#define GPABCDIMR_CPU0	(GPIO_BASE+0x38)
#define GPABCDIMR_CPU1	(GPIO_BASE+0x3C)
/**************************************************************************/

/* Register access macro (REG*()).*/
#define REG32(reg) 			(*((volatile uint32 *)(reg)))
#define REG16(reg) 			(*((volatile uint16 *)(reg)))
#define REG8(reg) 			(*((volatile uint8 *)(reg)))

/*********************  Function Prototype in gpio.c  ***********************/
int32 _rtl8954E_initGpioPin( uint32 gpioId, enum GPIO_CONTROL dedicate,
                                          enum GPIO_DIRECTION direction,
                                           enum GPIO_INTERRUPT_TYPE interruptEnable );
int32 _rtl8954E_getGpioDataBit( uint32 gpioId, uint32* pData );
int32 _rtl8954E_setGpioDataBit( uint32 gpioId, uint32 data );

#endif

#endif/*__GPIO__*/
