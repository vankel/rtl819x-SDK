/*
* Copyright c                  Realtek Semiconductor Corporation, 2012  
* All rights reserved.
* 
* Program : GPIO Header File 
* Abstract : 
* Author :                
* 
*/

 
#ifndef __GPIO_8686_H
#define __GPIO_8686_H
//#include <gpio.h>
/*==================== FOR RTL8686 EVB gpio pin ==================*/


#define PIN_CSEN 27//GPIO_D_3

/* LED */ 

#if defined(CONFIG_RTK_VOIP_PLATFORM_8686)
#define LED_TEL_GREEN	35
#define LED_TEL_RED  	36

#define PIN_VOIP0_LED	LED_TEL_GREEN
#define PIN_VOIP1_LED	LED_TEL_RED
#endif

#if defined(CONFIG_RTK_VOIP_PLATFORM_8686)

/******** GPIO define ********/


/* define GPIO port */
enum GPIO_PORT
{
	GPIO_PORT_A = 0,
	GPIO_PORT_B,
	GPIO_PORT_MAX,
	GPIO_PORT_UNDEF,
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
	GPIO_INT_ENABLE,

};


/* Register access macro (REG*()).*/
#define REG32(reg) 			(*((volatile uint32 *)(reg)))
#define REG16(reg) 			(*((volatile uint16 *)(reg)))
#define REG8(reg) 			(*((volatile uint8 *)(reg)))

/*********************  Function Prototype in gpio.c  ***********************/
//int32 _rtl8686_initGpioPin( uint32 gpioId, enum GPIO_DIRECTION direction, enum GPIO_INTERRUPT_TYPE interruptEnable );
int32 _rtl8686_initGpioPin( uint32 gpioId, unsigned char func );
int32 _rtl8686_getGpioDataBit( uint32 gpioId, uint32* pData );
int32 _rtl8686_setGpioDataBit( uint32 gpioId, uint32 data );

#endif

#endif/*__GPIO__*/
