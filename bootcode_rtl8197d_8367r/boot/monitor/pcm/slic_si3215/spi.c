/*
* Copyright c                  Realtek Semiconductor Corporation, 2005
* All rights reserved.
*
* Program : SPI Driver for test_prog_pcm v2 (This verison use readDirectReg() and writeDirectReg() to read/write direct register.)
* Abstract : In test_prog_pcm/src/io/pcm/pcm.c will call the API init_spi() in this SPI driver (test_prog_pcm/src/io/spi/spi.c)
* Author :  thlin (Thlin@realtek.com.tw)
* $Id: spi.c,v 1.3 2005/11/21 09:03:35 chiminer Exp $
*/


#include <linux/config.h>
//#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/string.h>
#include "proslic.h"
#include "../fpga_gpio.h"

//-- thlin + 05-08-01 ---------
#define _thlin_Debug_Level_1_ 0
#define _thlin_Debug_Level_2_ 0
#define _thlin_Debug_ReadWrite_API_ 0
//-----------------------------

//--chiminer 05-11-21----------
//only for GPIOD used.
#define _chiminer_gpiod 0
//-----------------------------

//--chiminer 05-12-26----------
//It's based on original program. I fixed some bug for GPIOD.
//This definiation must inverse with the definiation of _chiminer_gpiod.
#define _chiminer_gpiod_fixed 0
//-----------------------------

//--chiminer 05-12-6-----------//
#define speed_booting_rating 1
int i;
//-----------------------------//

//--chiminer 2005-12-19--------//
#define chiminer_daa_spi_debug 0
//-----------------------------//

//--chiminer 2005-12-27---------
//It's for SPI daisy-chain mode.
#define chiminer_daisy_chain 1
//------------------------------

/*********** Define *************/
typedef unsigned long long	uint64;
typedef long long		int64;
typedef unsigned int		uint32;
typedef int			int32;
typedef unsigned short		uint16;
typedef short			int16;
typedef unsigned char		uint8;
typedef char			int8;

#define printk prom_printf


#ifndef SUCCESS
#define SUCCESS 	0
#endif
#ifndef FAILED
#define FAILED 		-1
#endif

//-- thlin + 05-08-01 ----
#define RTL8186_TESTPROG

#ifdef RTL8186_TESTPROG
#define jiffies get_timer_jiffies()
#endif

//------------------------
//-- thlin + 05-08-01 ----
#define SysClock 660000000   // uint in Hz
#define Maxsped 20000//860000//
//------------------------

struct rtl8954C_spi_dev_s
{
	uint32 gpioSCLK;
	uint32 gpioCS_;
	uint32 gpioSDI;
	uint32 gpioSDO;
	uint32 SClkDelayLoop;
};

typedef struct rtl8954C_spi_dev_s rtl8954C_spi_dev_t;



/******** GPIO define ********/


#define CHANNEL 1 //original for one channel slic
//#define CHANNEL 2 //one for slic and another for daa. chiminer




#if  !_chiminer_gpiod_fixed //chiminer defination for GPIOC used(joshua)

#define SLIC_0 1
//#define SLIC_1 1

#define PIN_RELAY	GPIO_ID(GPIO_PORT_C,5)
#if defined(SLIC_0)
#define PIN_INT1	GPIO_ID(GPIO_PORT_C,4)
#define PIN_CS1		GPIO_ID(GPIO_PORT_C,3)	//output
#endif
#if defined(SLIC_1)
#define PIN_INT1	GPIO_ID(GPIO_PORT_D,3)
#define PIN_CS1		GPIO_ID(GPIO_PORT_D,2)	//output
#endif
#define PIN_RESET1	GPIO_ID(GPIO_PORT_B,0)
#define PIN_CLK		GPIO_ID(GPIO_PORT_C,2)	//output
#define PIN_DI		GPIO_ID(GPIO_PORT_C,1) 	//input

#define PIN_DO		GPIO_ID(GPIO_PORT_B,1)	//output



	/* CH 2 */
	#define PIN_INT2	GPIO_ID(GPIO_PORT_C,7)
	#define PIN_RESET2	GPIO_ID(GPIO_PORT_C,8)
	#define PIN_CS2		GPIO_ID(GPIO_PORT_C,9)
/* DAA used. chiminer*/
#define PIN_RESET3_DAA		GPIO_ID(GPIO_PORT_C,10)  //output
#define PIN_CS3_DAA		GPIO_ID(GPIO_PORT_C,11)	//output
#define PIN_CLK_DAA		GPIO_ID(GPIO_PORT_C,12)	//output
#define PIN_DI_DAA		GPIO_ID(GPIO_PORT_C,13) 	//input
#define PIN_DO_DAA		GPIO_ID(GPIO_PORT_C,14)	//output


#endif


#if _chiminer_gpiod_fixed //chiminer defination for GPIOD used

//#define PIN_RELAY	GPIO_ID(GPIO_PORT_D,13)
//#define PIN_INT1	GPIO_ID(GPIO_PORT_D,14)
/*
#define PIN_RESET1	GPIO_ID(GPIO_PORT_D,0)
#define PIN_CS1		GPIO_ID(GPIO_PORT_D,5)	//output
#define PIN_CLK		GPIO_ID(GPIO_PORT_D,6)	//output
#define PIN_DI		GPIO_ID(GPIO_PORT_D,7) 	//input
#define PIN_DO		GPIO_ID(GPIO_PORT_D,12)	//output
*/
/*
#define PIN_RESET1	GPIO_ID(GPIO_PORT_D,8)
#define PIN_CS1		GPIO_ID(GPIO_PORT_D,9)	//output
#define PIN_CLK		GPIO_ID(GPIO_PORT_D,10)	//output
#define PIN_DI		GPIO_ID(GPIO_PORT_D,13) //input
#define PIN_DO		GPIO_ID(GPIO_PORT_D,11)	//output
*/
#define PIN_RESET1	GPIO_ID(GPIO_PORT_D,7)
#define PIN_CS1		GPIO_ID(GPIO_PORT_D,10)	//output
#define PIN_CLK		GPIO_ID(GPIO_PORT_D,8)	//output
#define PIN_DI		GPIO_ID(GPIO_PORT_D,5) //input
#define PIN_DO		GPIO_ID(GPIO_PORT_D,0)	//output
	/* CH 2 */
	/*
	#define PIN_INT2	GPIO_ID(GPIO_PORT_C,7)
	#define PIN_RESET2	GPIO_ID(GPIO_PORT_C,8)
	#define PIN_CS2		GPIO_ID(GPIO_PORT_C,9)
	*/

/* DAA used. chiminer*/
#define PIN_RESET3_DAA		GPIO_ID(GPIO_PORT_D,13)  //output
#define PIN_CS3_DAA		GPIO_ID(GPIO_PORT_D,11)	//output
#define PIN_CLK_DAA		GPIO_ID(GPIO_PORT_D,9)	//output
#define PIN_DI_DAA		GPIO_ID(GPIO_PORT_D,12) 	//input
#define PIN_DO_DAA		GPIO_ID(GPIO_PORT_D,14)	//output
#endif




#define _GPIO_DEBUG_ 0



/************************************* Set GPIO Pin to SPI ***********************************************************/
/*
@func int32 | _rtl8954C_spi_init | Initialize SPI device
@parm rtl8954C_spi_dev_t* | pDev | Structure to store device information
@parm uint32 | gpioSCLK | GPIO ID of SCLK
@parm uint32 | gpioCS_ | GPIO ID of CS_
@parm uint32 | gpioSDI | GPIO ID of SDI
@parm uint32 | gpioSDO | GPIO ID of SDO
@parm uint32 | maxSpeed | how fast SPI driver can generate the SCLK signal (unit: HZ)
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
*/
int32 _rtl8954C_spi_init( rtl8954C_spi_dev_t* pDev, uint32 gpioSCLK, uint32 gpioCS_, uint32 gpioSDO, uint32 gpioSDI, int32 maxSpeed )
{

#if _thlin_Debug_Level_1_
	printk("rtl8954C_spi_init...\n");
#endif

	pDev->gpioSCLK = gpioSCLK;
	pDev->gpioCS_ = gpioCS_;
	pDev->gpioSDI = gpioSDI;
	pDev->gpioSDO = gpioSDO;
	//pDev->SClkDelayLoop = GetSysClockRate() / maxSpeed;	//@@
	pDev->SClkDelayLoop = SysClock / maxSpeed;
	/*rtlglue_printf("GetSysClockRate()=%d\n",GetSysClockRate());*/

	_rtl8954C_initGpioPin( gpioSCLK, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE );
	_rtl8954C_initGpioPin( gpioCS_, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE );
	_rtl8954C_initGpioPin( gpioSDI, GPIO_CONT_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE );
	_rtl8954C_initGpioPin( gpioSDO, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE );

	return SUCCESS;
}


/*
@func int32 | _rtl8954C_spi_rawRead | Read several bits from SPI
@parm rtl8954C_spi_dev_t* | pDev | Structure containing device information
@parm uint32* | pData | Pointer to store data
@parm uint32 | bits | Number bits of data wanted to read
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
*/
int32 _rtl8954C_spi_rawRead( rtl8954C_spi_dev_t* pDev, void* pData, int32 bits )
{
	uint32 delayLoop;
	uint8* pch = pData;

	*pch = 0;



  //original read function without GPIOD used. chiminer
	if ( pData == NULL ) return FAILED;

	_rtl8954C_setGpioDataBit( pDev->gpioCS_, 1 ); /* raise the CS_ */
	_rtl8954C_setGpioDataBit( pDev->gpioSCLK, 1 ); /* raise the SCLK */
	for( delayLoop = pDev->SClkDelayLoop; delayLoop > 0 ; delayLoop-- ); /* delay for a while */

	_rtl8954C_setGpioDataBit( pDev->gpioCS_, 0 ); /* fall down the CS_ */
	for( delayLoop = pDev->SClkDelayLoop; delayLoop > 0 ; delayLoop-- ); /* delay for a while */

	for( bits--; bits >= 0; bits-- )
	{
		uint32 buf;

		_rtl8954C_setGpioDataBit( pDev->gpioSCLK, 0 ); /* fall down the SCLK */
		for( delayLoop = pDev->SClkDelayLoop; delayLoop > 0 ; delayLoop-- ); /* delay for a while */

		pch[bits/8] &= ~((uint8)1<<(bits&0x7));
		_rtl8954C_getGpioDataBit( pDev->gpioSDI, &buf );
		pch[bits/8] |= buf?((uint8)1<<(bits&0x7)):0;

		_rtl8954C_setGpioDataBit( pDev->gpioSCLK, 1 ); /* raising up the SCLK */
		for( delayLoop = pDev->SClkDelayLoop; delayLoop > 0 ; delayLoop-- ); /* delay for a while */
	}

	_rtl8954C_setGpioDataBit( pDev->gpioCS_, 1 ); /* raise the CS_ */


	return SUCCESS;
}


/*
@func int32 | _rtl8954C_spi_rawWrite | Write several bits from SPI
@parm rtl8954C_spi_dev_t* | pDev | Structure containing device information
@parm uint32* | pData | Pointer to data
@parm uint32 | bits | Number bits of data wanting to write
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
*/



int32 _rtl8954C_spi_rawWrite( rtl8954C_spi_dev_t* pDev, void* pData, int32 bits )
{
	uint32 delayLoop;
	uint8* pch = pData;


   //original write function without GPIOD used.
	if ( pData == NULL ) return FAILED;

	_rtl8954C_setGpioDataBit( pDev->gpioCS_, 1 ); /* raise the CS_ */
	_rtl8954C_setGpioDataBit( pDev->gpioSCLK, 1 ); /* raise the SCLK */
	for( delayLoop = pDev->SClkDelayLoop; delayLoop > 0 ; delayLoop-- ); /* delay for a while */

	_rtl8954C_setGpioDataBit( pDev->gpioCS_, 0 ); /* fall down the CS_ */
	for( delayLoop = pDev->SClkDelayLoop; delayLoop > 0 ; delayLoop-- ); /* delay for a while */

	for( bits-- ; bits >= 0; bits-- )
	{
		_rtl8954C_setGpioDataBit( pDev->gpioSDO, (pch[bits/8]&((uint32)1<<(bits&0x7)))?1:0 );
		_rtl8954C_setGpioDataBit( pDev->gpioSCLK, 0 ); /* fall down the SCLK */
		for( delayLoop = pDev->SClkDelayLoop; delayLoop > 0 ; delayLoop-- ); /* delay for a while */
		_rtl8954C_setGpioDataBit( pDev->gpioSCLK, 1 ); /* raising up the SCLK */
		for( delayLoop = pDev->SClkDelayLoop; delayLoop > 0 ; delayLoop-- ); /* delay for a while */
	}

	_rtl8954C_setGpioDataBit( pDev->gpioCS_, 1 ); /* raise the CS_ */



	return SUCCESS;
}


/*
@func int32 | _rtl8954C_spi_exit | Called when a SPI device is released
@parm rtl8954C_spi_dev_t* | pDev | Structure containing device information
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
*/
int32 _rtl8954C_spi_exit( rtl8954C_spi_dev_t* pDev )
{
	return SUCCESS;
}


/************************* SPI API ****************************************/





static rtl8954C_spi_dev_t dev[CHANNEL];
static int cur_channel = 0;//for slic.
static int cur_channel_1 = 1; //for daa. chiminer

typedef unsigned long cyg_tick_count_t;
extern unsigned long volatile jiffies;


void cyg_thread_delay(int delay)
{
	unsigned long t1 = jiffies+delay;

	while(jiffies < t1);

#if 0
	while(jiffies < t1)
	{
		printk("In loop");
	}
#endif

}

//------------------------------------------

void init_channel(int channel, uint32 pin_cs, uint32 pin_reset, uint32 pin_clk, uint32 pin_do, uint32 pin_di)
{
	//thlin modified 2005-08-11

	_rtl8954C_initGpioPin(pin_reset, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
	_rtl8954C_initGpioPin(pin_cs, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
	//_rtl8954C_initGpioPin(pin_clk, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);

	#if speed_booting_rating
		for(i=0;i<50000;i++);
	#else
		cyg_thread_delay(15);
	#endif
	_rtl8954C_setGpioDataBit(pin_cs, 1); 	/* CS_ preset to high state*/
	_rtl8954C_setGpioDataBit(pin_reset, 1);	// reset high
	#if speed_booting_rating
		for(i=0;i<500000;i++);
	#else
		cyg_thread_delay(15);
	#endif
	_rtl8954C_setGpioDataBit(pin_reset, 0);	// set reset low, PCLK and FS id present and stable.
	#if speed_booting_rating
		for(i=0;i<50000;i++);
	#else
		cyg_thread_delay(15);
	#endif
	_rtl8954C_setGpioDataBit(pin_reset, 1);	// release reset
	#if speed_booting_rating
		for(i=0;i<500000;i++);
	#else
		cyg_thread_delay(15);
	#endif				// wait more than 100ms


	_rtl8954C_spi_init(&dev[channel], pin_clk, pin_cs, pin_do, pin_di, Maxsped);

}

//-------------------------------------------

void set_channel(int channel)
{
	cur_channel = channel;

	printk("----------------- Channel %d setting ------------------- \n", channel);
}

//------------------------------------------

void init_spi(void)
{

#if _thlin_Debug_Level_1_
	printk("init_spi =>\n");
#endif

	//_rtl8954C_initGpioPin(PIN_RELAY, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);

#if _thlin_Debug_Level_1_
	printk("PIN_RELAY init OK.\n");
#endif



	init_channel(0, PIN_CS1, PIN_RESET1, PIN_CLK, PIN_DO, PIN_DI);
#if (CHANNEL > 1)
	//for DAA used .chiminer
	init_channel(1, PIN_CS3_DAA, PIN_RESET3_DAA, PIN_CLK_DAA, PIN_DO_DAA, PIN_DI_DAA);
#endif
	//_rtl8954C_setGpioDataBit(PIN_RELAY, 1);
}

/************************* Read & Write Direct Register API ***********************************/
#if chiminer_daisy_chain

static unsigned char read_spi(unsigned int reg)
{
	uint8 buf;
	extern unsigned char slic_order;

	buf = slic_order;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);

	buf = reg | 0x80;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);

	_rtl8954C_spi_rawRead(&dev[cur_channel], &buf, 8);
	return buf;
}

static void write_spi(unsigned int reg, unsigned int data)
{
	uint8 buf;
	extern unsigned char slic_order;

	buf = slic_order;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);

	buf = reg;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);

	buf = data;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);
}


unsigned char readDirectReg(unsigned char address)
{

	return read_spi(address);
}


void writeDirectReg(unsigned char address, unsigned char data)
{

	write_spi(address, data);
}

void waitForIndirectReg(void)
{
	while (readDirectReg(I_STATUS));
}

unsigned short readIndirectReg(unsigned char address)
{
	waitForIndirectReg();

	writeDirectReg(IAA,address);
	waitForIndirectReg();
	return ( readDirectReg(IDA_LO) | (readDirectReg (IDA_HI))<<8);
}


void writeIndirectReg(unsigned char address, unsigned short data)
{
	waitForIndirectReg();
	writeDirectReg(IDA_LO,(unsigned char)(data & 0xFF));
	writeDirectReg(IDA_HI,(unsigned char)((data & 0xFF00)>>8));
	writeDirectReg(IAA,address);
}

static unsigned char read_spi_nodaisy(unsigned int reg)
{
	uint8 buf;

	buf = reg | 0x80;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);

	_rtl8954C_spi_rawRead(&dev[cur_channel], &buf, 8);
	return buf;
}

unsigned char readDirectReg_nodaisy(unsigned char address)
{

	return read_spi_nodaisy(address);
}

static void write_spi_nodaisy(unsigned int reg, unsigned int data)
{
	uint8 buf;

	buf = reg;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);

	buf = data;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);
}

void writeDirectReg_nodaisy(unsigned char address, unsigned char data)
{

	write_spi_nodaisy(address, data);
}


#else

static unsigned char read_spi(unsigned int reg)
{
	uint8 buf;

	buf = reg | 0x80;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);

	_rtl8954C_spi_rawRead(&dev[cur_channel], &buf, 8);
	return buf;
}

static void write_spi(unsigned int reg, unsigned int data)
{
	uint8 buf;

	buf = reg;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);

	buf = data;
	_rtl8954C_spi_rawWrite(&dev[cur_channel], &buf, 8);
}


unsigned char readDirectReg(unsigned char address)
{
#if _thlin_Debug_ReadWrite_API_
	printk("read DirectReg %d...\n", address);
#endif
	return read_spi(address);
}


void writeDirectReg(unsigned char address, unsigned char data)
{
#if _thlin_Debug_ReadWrite_API_
	printk("write 0x%x to DirectReg %d...\n", data, address);
#endif
	write_spi(address, data);
}

void waitForIndirectReg(void)
{
	while (readDirectReg(I_STATUS));
}

unsigned short readIndirectReg(unsigned char address)
{
	waitForIndirectReg();

	writeDirectReg(IAA,address);
	waitForIndirectReg();
	return ( readDirectReg(IDA_LO) | (readDirectReg (IDA_HI))<<8);
}


void writeIndirectReg(unsigned char address, unsigned short data)
{
	waitForIndirectReg();
	writeDirectReg(IDA_LO,(unsigned char)(data & 0xFF));
	writeDirectReg(IDA_HI,(unsigned char)((data & 0xFF00)>>8));
	writeDirectReg(IAA,address);
}


#endif

/*********************************************************/
//The following function is used to DAA(Si3050 Si3018/19).
//Because of differential SPI interface, people must call
//those function when they want to initialize DAA.
//chiminer 12_19_2005
/*********************************************************/

static unsigned char read_spi_daa(unsigned int reg)
{
	uint8 buf;

	buf = 0x60;
	_rtl8954C_spi_rawWrite(&dev[cur_channel_1], &buf, 8);

	buf = reg;
	_rtl8954C_spi_rawWrite(&dev[cur_channel_1], &buf, 8);

	_rtl8954C_spi_rawRead(&dev[cur_channel_1], &buf, 8);
	return buf;
}

static void write_spi_daa(unsigned int reg, unsigned int data)
{
	uint8 buf;

	buf = 0x20;
	_rtl8954C_spi_rawWrite(&dev[cur_channel_1], &buf, 8);

	buf = reg;
	_rtl8954C_spi_rawWrite(&dev[cur_channel_1], &buf, 8);

	buf = data;
	_rtl8954C_spi_rawWrite(&dev[cur_channel_1], &buf, 8);
}

unsigned char readDAAReg(unsigned char address)
{
#if chiminer_daa_spi_debug
	printk("read DirectReg %d...\n", address);
#endif
	return read_spi_daa(address);
}


void writeDAAReg(unsigned char address, unsigned char data)
{
#if chiminer_daa_spi_debug
	printk("write 0x%x to DirectReg %d...\n", data, address);
#endif
	write_spi_daa(address, data);
}






/*****************************************************************************/





