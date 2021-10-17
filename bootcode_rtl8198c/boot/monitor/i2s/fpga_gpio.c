
#include"fpga_gpio.h"

#define FAILED 0
#define SUCCESS 1

#define printfByPolling prom_printf
#define printk printfByPolling

enum GPIO_FUNC
{
	GPIO_FUNC_DEDICATE,

	GPIO_FUNC_DIRECTION,
	GPIO_FUNC_DATA,
	GPIO_FUNC_INTERRUPT_STATUS,
	GPIO_FUNC_INTERRUPT_ENABLE,
	GPIO_FUNC_MAX,
};

//******************************************* GPIO control
static uint32_t regGpioControl[] =
{
	GPABCDCNR, /* Port A */
	GPABCDCNR, /* Port B */
	GPABCDCNR, /* Port C */
	GPABCDCNR, /* Port D */
	GPEFGHCNR, /* Port E */
	GPEFGHCNR, /* Port F */
	GPEFGHCNR, /* Port G */
	GPEFGHCNR, /* Port H */
};

static uint32_t bitStartGpioControl[] =
{
	0,  /* Port A */
	8,  /* Port B */
	16, /* Port C */
	24, /* Port D */
	0,  /* Port E */
	8,  /* Port F */
	16, /* Port G */
	24, /* Port H */
};

#if 0 //8196d
/* 8198 test chip */
#define SHAREPIN_REGISTER 0xB8000040

#define C0C3_SHAREPIN_CFG (SET_BIT(23)|SET_BIT(22)|SET_BIT(20)|SET_BIT(19)|SET_BIT(17)|SET_BIT(16)|SET_BIT(14)|SET_BIT(13))

#define SET_BIT(x) (1<<x)
static uint32_t GpioShare_setting[][2] =
{
	/* {pin_mux_sel      ,    pin_mux_sel2} */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port A0 */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port A1 */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port A2 */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port A3 */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port A4 */
	{(SET_BIT(5)), 0x00000000},	/* Port A5 */
	{(SET_BIT(5)), 0x00000000},	/* Port A6 */
	{(SET_BIT(7)|SET_BIT(6)), 0x00000000},	/* Port A7 */
	{0x00000000, (SET_BIT(2)|SET_BIT(1))},	/* Port B0 */
	{0x00000000, (SET_BIT(5)|SET_BIT(4))},	/* Port B1 */
	{0x00000000, (SET_BIT(8)|SET_BIT(7))},	/* Port B2 */
	{0x00000000, (SET_BIT(11)|SET_BIT(10))},/* Port B3 */
	{0x00000000, 0x00000000},	/* Port B4 */
	{0x00000000, 0x00000000},	/* Port B5 */
	{0x00000000, 0x00000000},	/* Port B6 */
	{0x00000000, 0x00000000},	/* Port B7 */
#if 1 //test chip bug fix
	{0x00000000, C0C3_SHAREPIN_CFG},	/* Port C0 */
	{0x00000000, C0C3_SHAREPIN_CFG},	/* Port C1 */
	{0x00000000, C0C3_SHAREPIN_CFG},	/* Port C2 */
	{0x00000000, C0C3_SHAREPIN_CFG},	/* Port C3 */
	//{0x00000000, (SET_BIT(14)|SET_BIT(13))},	/* Port C1 */
	//{0x00000000, (SET_BIT(14)|SET_BIT(13))},	/* Port C2 */
	//{0x00000000, (SET_BIT(14)|SET_BIT(13))},	/* Port C3 */
#else
	{0x00000000, (SET_BIT(14)|SET_BIT(13))},	/* Port C0 */
	{0x00000000, (SET_BIT(17)|SET_BIT(16))},	/* Port C1 */
	{0x00000000, (SET_BIT(20)|SET_BIT(19))},	/* Port C2 */
	{0x00000000, (SET_BIT(23)|SET_BIT(22))},	/* Port C3 */
#endif
	{0x00000000, (SET_BIT(24))},	/* Port C4 */
	{0x00000000, 0x00000000},	/* Port C5 */
	{0x00000000, 0x00000000},	/* Port C6 */
	{0x00000000, 0x00000000},	/* Port C7 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port D0 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port D1 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port D2 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port D3 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port D4 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port D5 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port D6 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port D7 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E0 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E1 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E2 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port E3 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port E4 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port E5 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port E6 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E7 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F0 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F1 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F2 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F3 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F4 */
	{0x00000000, 0x00000000},	/* Port F5 */
	{0x00000000, 0x00000000},	/* Port F6 */
	{0x00000000, 0x00000000},	/* Port F7 */
	{0x00000000, 0x00000000},	/* Port G0 */
	{0x00000000, 0x00000000},	/* Port G1 */
	{0x00000000, 0x00000000},	/* Port G2 */
	{0x00000000, 0x00000000},	/* Port G3 */
	{0x00000000, 0x00000000},	/* Port G4 */
	{0x00000000, 0x00000000},	/* Port G5 */
	{0x00000000, 0x00000000},	/* Port G6 */
	{0x00000000, 0x00000000},	/* Port G7 */
	{0x00000000, 0x00000000},	/* Port H0 */
	{0x00000000, 0x00000000},	/* Port H1 */
	{0x00000000, 0x00000000},	/* Port H2 */
	{0x00000000, 0x00000000},	/* Port H3 */
	{0x00000000, 0x00000000},	/* Port H4 */
	{0x00000000, 0x00000000},	/* Port H5 */
	{0x00000000, 0x00000000},	/* Port H6 */
	{0x00000000, 0x00000000},	/* Port H7 */
};
#undef SET_BIT
#else
/* 8196d test chip */
#define SHAREPIN_REGISTER 0xB8000040


#define SET_BIT(x) (1<<x)
static uint32_t GpioShare_setting[][2] =
{
	/* {pin_mux_sel      ,    pin_mux_sel2} */
	{(SET_BIT(17)|SET_BIT(16)), 0x00000000},	/* Port A0 */
	{(SET_BIT(13)|SET_BIT(12)), 0x00000000},	/* Port A1 */
	{(SET_BIT(2)|SET_BIT(1)), 0x00000000},	/* Port A2 */
	{(SET_BIT(2)|SET_BIT(1)), 0x00000000},	/* Port A3 */
	{(SET_BIT(2)|SET_BIT(1)), 0x00000000},	/* Port A4 */
	{(SET_BIT(2)|SET_BIT(1)), 0x00000000},	/* Port A5 */
	{(SET_BIT(2)|SET_BIT(1)), 0x00000000},	/* Port A6 */
	{(SET_BIT(5)), 0x00000000},	/* Port A7 */
	{(SET_BIT(5)), 0x00000000},	/* Port B0 */
	{(SET_BIT(6)), 0x00000000},	/* Port B1 */
	{0x00000000, (SET_BIT(1)|SET_BIT(0))},	/* Port B2 */
	{0x00000000, (SET_BIT(4)|SET_BIT(3))},/* Port B3 */
	{0x00000000, (SET_BIT(7)|SET_BIT(6))},	/* Port B4 */
	{0x00000000, (SET_BIT(10)|SET_BIT(9))},	/* Port B5 */
	{0x00000000, (SET_BIT(13)|SET_BIT(12))},	/* Port B6 */
	{0x00000000, (SET_BIT(17))},	/* Port B7 */
	{0x00000000, (SET_BIT(20))},	/* Port C0 */
	{(SET_BIT(21)|SET_BIT(20)), 0x00000000},	/* Port C1 */
	{(SET_BIT(23)|SET_BIT(22)), 0x00000000},	/* Port C2 */
	{(SET_BIT(25)|SET_BIT(24)), 0x00000000},	/* Port C3 */
	{(SET_BIT(27)|SET_BIT(26)), 0x00000000},	/* Port C4 */
	{(SET_BIT(29)|SET_BIT(28)), 0x00000000},	/* Port C5 */
	{(SET_BIT(19)|SET_BIT(18)), 0x00000000},	/* Port C6 */
	{0x00000000, 0x00000000},	/* Port C7 */
	{0x00000000, 0x00000000},	/* Port D0 */
	{0x00000000, 0x00000000},	/* Port D1 */
	{0x00000000, 0x00000000},	/* Port D2 */
	{0x00000000, 0x00000000},	/* Port D3 */
	{0x00000000, 0x00000000},	/* Port D4 */
	{0x00000000, 0x00000000},	/* Port D5 */
	{0x00000000, 0x00000000},	/* Port D6 */
	{0x00000000, 0x00000000},	/* Port D7 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E0 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E1 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E2 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E3 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E4 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E5 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E6 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E7 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F0 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F1 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F2 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F3 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F4 */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port F5 */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port F6 */
	{0x00000000, 0x00000000},	/* Port F7 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G0 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G1 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G2 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G3 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G4 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G5 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G6 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G7 */
	{0x00000000, 0x00000000},	/* Port H0 */
	{0x00000000, 0x00000000},	/* Port H1 */
	{0x00000000, 0x00000000},	/* Port H2 */
	{0x00000000, 0x00000000},	/* Port H3 */
	{0x00000000, 0x00000000},	/* Port H4 */
	{0x00000000, 0x00000000},	/* Port H5 */
	{0x00000000, 0x00000000},	/* Port H6 */
	{0x00000000, 0x00000000},	/* Port H7 */
};

static uint32_t GpioShare_mask[][2] =
{
	/* {pin_mux_sel      ,    pin_mux_sel2} */
	{(SET_BIT(17)|SET_BIT(16)), 0x00000000},	/* Port A0 */
	{(SET_BIT(13)|SET_BIT(12)), 0x00000000},	/* Port A1 */
	{(SET_BIT(2)|SET_BIT(1)|SET_BIT(0)), 0x00000000},	/* Port A2 */
	{(SET_BIT(2)|SET_BIT(1)|SET_BIT(0)), 0x00000000},	/* Port A3 */
	{(SET_BIT(2)|SET_BIT(1)|SET_BIT(0)), 0x00000000},	/* Port A4 */
	{(SET_BIT(2)|SET_BIT(1)|SET_BIT(0)), 0x00000000},	/* Port A5 */
	{(SET_BIT(2)|SET_BIT(1)|SET_BIT(0)), 0x00000000},	/* Port A6 */
	{(SET_BIT(5)), 0x00000000},	/* Port A7 */
	{(SET_BIT(5)), 0x00000000},	/* Port B0 */
	{(SET_BIT(6)), 0x00000000},	/* Port B1 */
	{0x00000000, (SET_BIT(1)|SET_BIT(0))},	/* Port B2 */
	{0x00000000, (SET_BIT(4)|SET_BIT(3))},/* Port B3 */
	{0x00000000, (SET_BIT(7)|SET_BIT(6))},	/* Port B4 */
	{0x00000000, (SET_BIT(10)|SET_BIT(9))},	/* Port B5 */
	{0x00000000, (SET_BIT(13)|SET_BIT(12))},	/* Port B6 */
	{0x00000000, (SET_BIT(17)|SET_BIT(16)|SET_BIT(15))},	/* Port B7 */
	{0x00000000, (SET_BIT(20)|SET_BIT(19)|SET_BIT(18))},	/* Port C0 */
	{(SET_BIT(21)|SET_BIT(20)), 0x00000000},	/* Port C1 */
	{(SET_BIT(23)|SET_BIT(22)), 0x00000000},	/* Port C2 */
	{(SET_BIT(25)|SET_BIT(24)), 0x00000000},	/* Port C3 */
	{(SET_BIT(27)|SET_BIT(26)), 0x00000000},	/* Port C4 */
	{(SET_BIT(29)|SET_BIT(28)), 0x00000000},	/* Port C5 */
	{(SET_BIT(19)|SET_BIT(18)), 0x00000000},	/* Port C6 */
	{0x00000000, 0x00000000},	/* Port C7 */
	{0x00000000, 0x00000000},	/* Port D0 */
	{0x00000000, 0x00000000},	/* Port D1 */
	{0x00000000, 0x00000000},	/* Port D2 */
	{0x00000000, 0x00000000},	/* Port D3 */
	{0x00000000, 0x00000000},	/* Port D4 */
	{0x00000000, 0x00000000},	/* Port D5 */
	{0x00000000, 0x00000000},	/* Port D6 */
	{0x00000000, 0x00000000},	/* Port D7 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E0 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E1 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E2 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E3 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E4 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E5 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E6 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port E7 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F0 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F1 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F2 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F3 */
	{(SET_BIT(9)|SET_BIT(8)), 0x00000000},	/* Port F4 */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port F5 */
	{(SET_BIT(4)|SET_BIT(3)), 0x00000000},	/* Port F6 */
	{0x00000000, 0x00000000},	/* Port F7 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G0 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G1 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G2 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G3 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G4 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G5 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G6 */
	{(SET_BIT(11)|SET_BIT(10)), 0x00000000},	/* Port G7 */
	{0x00000000, 0x00000000},	/* Port H0 */
	{0x00000000, 0x00000000},	/* Port H1 */
	{0x00000000, 0x00000000},	/* Port H2 */
	{0x00000000, 0x00000000},	/* Port H3 */
	{0x00000000, 0x00000000},	/* Port H4 */
	{0x00000000, 0x00000000},	/* Port H5 */
	{0x00000000, 0x00000000},	/* Port H6 */
	{0x00000000, 0x00000000},	/* Port H7 */
};

#undef SET_BIT
#endif

//******************************************* Direction
static uint32_t regGpioDirection[] =
{
	GPABCDDIR, /* Port A */
	GPABCDDIR, /* Port B */
	GPABCDDIR, /* Port C */
	GPABCDDIR, /* Port D */
	GPEFGHDIR, /* Port E */
	GPEFGHDIR, /* Port F */
	GPEFGHDIR, /* Port G */
	GPEFGHDIR, /* Port H */
};

static uint32_t bitStartGpioDirection[] =
{
	0,  /* Port A */
	8,  /* Port B */
	16, /* Port C */
	24, /* Port D */
	0,  /* Port E */
	8,  /* Port F */
	16, /* Port G */
	24, /* Port H */
};

//******************************************* Data
static uint32_t regGpioData[] =
{
	GPABCDDATA, /* Port A */
	GPABCDDATA, /* Port B */
	GPABCDDATA, /* Port C */
	GPABCDDATA, /* Port D */
	GPEFGHDATA, /* Port E */
	GPEFGHDATA, /* Port F */
	GPEFGHDATA, /* Port G */
	GPEFGHDATA, /* Port H */
	GPEFGHDATA, /* Port I */
};

static uint32_t bitStartGpioData[] =
{
	0,  /* Port A */
	8,  /* Port B */
	16, /* Port C */
	24, /* Port D */
	0,  /* Port E */
	8,  /* Port F */
	16, /* Port G */
	24, /* Port H */
};

//******************************************* ISR
static uint32_t regGpioInterruptStatus[] =
{
	GPABCDISR, /* Port A */
	GPABCDISR, /* Port B */
	GPABCDISR, /* Port C */
	GPABCDISR, /* Port D */
	GPEFGHISR, /* Port E */
	GPEFGHISR, /* Port F */
	GPEFGHISR, /* Port G */
	GPEFGHISR, /* Port H */
};

static uint32_t bitStartGpioInterruptStatus[] =
{
	0,  /* Port A */
	8,  /* Port B */
	16, /* Port C */
	24, /* Port D */
	0,  /* Port E */
	8,  /* Port F */
	16, /* Port G */
	24, /* Port H */
};

//******************************************* IMR
static uint32_t regGpioInterruptEnable[] =
{
	GPABIMR, /* Port A */
	GPABIMR, /* Port B */
	GPCDIMR, /* Port C */
	GPCDIMR, /* Port D */
	GPEFIMR, /* Port E */
	GPEFIMR, /* Port F */
	GPGHIMR, /* Port G */
	GPGHIMR, /* Port H */
};

static uint32_t bitStartGpioInterruptEnable[] =
{
	0,  /* Port A */
	16, /* Port B */
	0,  /* Port C */
	16, /* Port D */
	0,  /* Port E */
	16, /* Port F */
	0,  /* Port G */
	16, /* Port H */
};

/*
@func int32 | _getGpio | abstract GPIO registers
@parm enum GPIO_FUNC | func | control/data/interrupt register
@parm enum GPIO_PORT | port | GPIO port
@parm uint32_t | pin | pin number
@rvalue uint32_t | value
@comm
This function is for internal use only. You don't need to care what register address of GPIO is.
This function abstracts these information.
*/
static uint32_t _getGpio( enum GPIO_FUNC func, enum GPIO_PORT port, uint32_t pin )
{

#if _GPIO_DEBUG_ >= 4
	printk("[%s():%d] func=%d port=%d pin=%d\n", __FUNCTION__, __LINE__, func, port, pin );
#endif

	switch( func )
	{
		case GPIO_FUNC_DEDICATE:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioControl[port]=0x%08x  bitStartGpioControl[port]=%d\n", __FUNCTION__, __LINE__, regGpioControl[port], bitStartGpioControl[port] );
#endif
			if ( REG32(regGpioControl[port]) & ( (uint32_t)1 << (pin+bitStartGpioControl[port]) ) )
				return 1;
			else
				return 0;
			break;


		case GPIO_FUNC_DIRECTION:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioDirection[port]=0x%08x  bitStartGpioDirection[port]=%d\n", __FUNCTION__, __LINE__, regGpioDirection[port], bitStartGpioDirection[port] );
#endif
			if ( REG32(regGpioDirection[port]) & ( (uint32_t)1 << (pin+bitStartGpioDirection[port]) ) )
				return 1;
			else
				return 0;
			break;

		case GPIO_FUNC_DATA:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioData[port]=0x%08x  bitStartGpioData[port]=%d\n", __FUNCTION__, __LINE__, regGpioData[port], bitStartGpioData[port] );
#endif
			if ( REG32(regGpioData[port]) & ( (uint32_t)1 << (pin+bitStartGpioData[port]) ) )
				return 1;
			else
				return 0;
			break;

		case GPIO_FUNC_INTERRUPT_ENABLE:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioInterruptEnable[port]=0x%08x  bitStartGpioInterruptEnable[port]=%d\n", __FUNCTION__, __LINE__, regGpioInterruptEnable[port], bitStartGpioInterruptEnable[port] );
#endif
			return ( REG32(regGpioInterruptEnable[port]) >> (pin*2+bitStartGpioInterruptEnable[port]) ) & (uint32_t)0x3;
			break;

		case GPIO_FUNC_INTERRUPT_STATUS:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioInterruptStatus[port]=0x%08x  bitStartGpioInterruptEnable[port]=%d\n", __FUNCTION__, __LINE__, regGpioInterruptStatus[port], bitStartGpioInterruptStatus[port] );
#endif
			if ( REG32(regGpioInterruptStatus[port]) & ( (uint32_t)1 << (pin+bitStartGpioInterruptStatus[port]) ) )
				return 1;
			else
				return 0;
			break;

		case GPIO_FUNC_MAX:
			printk("Wrong GPIO function\n");
			break;
	}
	return 0xffffffff;
}


/*
@func int32 | _setGpio | abstract GPIO registers
@parm enum GPIO_FUNC | func | control/data/interrupt register
@parm enum GPIO_PORT | port | GPIO port
@parm uint32_t | pin | pin number
@parm uint32_t | data | value
@rvalue NONE
@comm
This function is for internal use only. You don't need to care what register address of GPIO is.
This function abstracts these information.
*/
static void _setGpio( enum GPIO_FUNC func, enum GPIO_PORT port, uint32_t pin, uint32_t data )
{
	unsigned int temp;
#if _GPIO_DEBUG_ >= 4
	printk("[%s():%d] func=%d port=%d pin=%d data=%d\n", __FUNCTION__, __LINE__, func, port, pin, data );
#endif

	switch( func )
	{
		case GPIO_FUNC_DEDICATE:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioControl[port]=0x%08x  bitStartGpioControl[port]=%d\n", __FUNCTION__, __LINE__, regGpioControl[port], bitStartGpioControl[port] );
#endif
			if ( data )
				REG32(regGpioControl[port]) |= (uint32_t)1 << (pin+bitStartGpioControl[port]);
			else {
				REG32(regGpioControl[port]) &= ~((uint32_t)1 << (pin+bitStartGpioControl[port]));
				//printk("oldsharepin_register=%x,%x\n", REG32(SHAREPIN_REGISTER), REG32(SHAREPIN_REGISTER+4));
#if 0
// 8198 gpio pin mux register
				REG32(SHAREPIN_REGISTER) |= GpioShare_setting[(port<<3)|pin][0];
				REG32(SHAREPIN_REGISTER+4) |= GpioShare_setting[(port<<3)|pin][1];
#else
// 8196d gpio pin mux register
				temp = REG32(SHAREPIN_REGISTER) & (~GpioShare_mask[(port<<3)|pin][0]);
				REG32(SHAREPIN_REGISTER) = temp | GpioShare_setting[(port<<3)|pin][0];
				temp = REG32(SHAREPIN_REGISTER+4) & (~GpioShare_mask[(port<<3)|pin][1]);
				REG32(SHAREPIN_REGISTER+4) = temp | GpioShare_setting[(port<<3)|pin][1];

#endif
				//printk("newsharepin_register=%x,%x\n", REG32(SHAREPIN_REGISTER), REG32(SHAREPIN_REGISTER+4));
				//printk("[%d,%d]GPIOSET(%x)", port, pin,(port<<3)|pin);
			}
			break;


		case GPIO_FUNC_DIRECTION:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioDirection[port]=0x%08x  bitStartGpioDirection[port]=%d\n", __FUNCTION__, __LINE__, regGpioDirection[port], bitStartGpioDirection[port] );
#endif
			if ( data )
				REG32(regGpioDirection[port]) |= (uint32_t)1 << (pin+bitStartGpioDirection[port]);
			else
				REG32(regGpioDirection[port]) &= ~((uint32_t)1 << (pin+bitStartGpioDirection[port]));
			break;

		case GPIO_FUNC_DATA:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioData[port]=0x%08x  bitStartGpioData[port]=%d\n", __FUNCTION__, __LINE__, regGpioData[port], bitStartGpioData[port] );
#endif
			if ( data )
				REG32(regGpioData[port]) |= (uint32_t)1 << (pin+bitStartGpioData[port]);
			else
				REG32(regGpioData[port]) &= ~((uint32_t)1 << (pin+bitStartGpioData[port]));
#if 1
	asm volatile (" sync \n\t");
#else

#endif

			break;

		case GPIO_FUNC_INTERRUPT_ENABLE:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioInterruptEnable[port]=0x%08x  bitStartGpioInterruptEnable[port]=%d\n", __FUNCTION__, __LINE__, regGpioInterruptEnable[port], bitStartGpioInterruptEnable[port] );
#endif
			REG32(regGpioInterruptEnable[port]) &= ~((uint32_t)0x3 << (pin*2+bitStartGpioInterruptEnable[port]));
			REG32(regGpioInterruptEnable[port]) |= (uint32_t)data << (pin*2+bitStartGpioInterruptEnable[port]);
			break;

		case GPIO_FUNC_INTERRUPT_STATUS:
#if _GPIO_DEBUG_ >= 5
			printk("[%s():%d] regGpioInterruptStatus[port]=0x%08x  bitStartGpioInterruptStatus[port]=%d\n", __FUNCTION__, __LINE__, regGpioInterruptStatus[port], bitStartGpioInterruptStatus[port] );
#endif
			if ( data )
				REG32(regGpioInterruptStatus[port]) |= (uint32_t)1 << (pin+bitStartGpioInterruptStatus[port]);
			else
				REG32(regGpioInterruptStatus[port]) &= ~((uint32_t)1 << (pin+bitStartGpioInterruptStatus[port]));
			break;

		case GPIO_FUNC_MAX:
			printk("Wrong GPIO function\n");
			break;
	}
}

/*
@func int32_t | _rtl8954C_initGpioPin | Initiate a specifed GPIO port.
@parm uint32_t | gpioId | The GPIO port that will be configured
@parm enum GPIO_CONTROL | dedicate | Dedicated peripheral type
@parm enum GPIO_DIRECTION | direction | Data direction, in or out
@parm enum GPIO_INTERRUPT_TYPE | interruptEnable | Interrupt mode
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
This function is used to initialize GPIO port.
*/
int32_t _rtl8954C_initGpioPin( uint32_t gpioId, enum GPIO_CONTROL dedicate,
                                           enum GPIO_DIRECTION direction,
                                           enum GPIO_INTERRUPT_TYPE interruptEnable )
{
	uint32_t port = GPIO_PORT( gpioId );
	uint32_t pin = GPIO_PIN( gpioId );

	if ( port >= GPIO_PORT_MAX ) return FAILED;
	if ( pin >= 8 ) return FAILED;

	switch ( dedicate )
	{
		case GPIO_CONT_GPIO:
			_setGpio( GPIO_FUNC_DEDICATE, port, pin, 0 );
			break;
		case GPIO_CONT_PERI:
			_setGpio( GPIO_FUNC_DEDICATE, port, pin, 1 );
			break;
	}

	_setGpio( GPIO_FUNC_DIRECTION, port, pin, direction );

	_setGpio( GPIO_FUNC_INTERRUPT_ENABLE, port, pin, interruptEnable );

	return SUCCESS;
}

/*
@func int32_t | _rtl8954C_getGpioDataBit | Get the bit value of a specified GPIO ID.
@parm uint32_t | gpioId | GPIO ID
@parm uint32_t* | data | Pointer to store return value
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
*/
int32_t _rtl8954C_getGpioDataBit( uint32_t gpioId, uint32_t* pData )
{
	uint32_t port = GPIO_PORT( gpioId );
	uint32_t pin = GPIO_PIN( gpioId );

	if ( port >= GPIO_PORT_MAX ) return FAILED;
	if ( pin >= 8 ) return FAILED;
	if ( pData == NULL ) return FAILED;

	*pData = _getGpio( GPIO_FUNC_DATA, port, pin );
#if _GPIO_DEBUG_ >= 3
	printk("[%s():%d] (port=%d,pin=%d)=%d\n", __FUNCTION__, __LINE__, port, pin, *pData );
#endif

	return SUCCESS;
}

/*
@func int32_t | _rtl8954C_setGpioDataBit | Set the bit value of a specified GPIO ID.
@parm uint32_t | gpioId | GPIO ID
@parm uint32_t | data | Data to write
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
*/
int32_t _rtl8954C_setGpioDataBit( uint32_t gpioId, uint32_t data )
{
	uint32_t port = GPIO_PORT( gpioId );
	uint32_t pin = GPIO_PIN( gpioId );

	if ( port >= GPIO_PORT_MAX ) return FAILED;
	if ( pin >= 8 ) return FAILED;

#if _GPIO_DEBUG_ >= 3
	printk("[%s():%d] (port=%d,pin=%d)=%d\n", __FUNCTION__, __LINE__, port, pin, data );
#endif
	_setGpio( GPIO_FUNC_DATA, port, pin, data );

	return SUCCESS;
}


/************************* I2C read/write function ************************/
#if defined(CONFIG_RTK_VOIP_DRIVERS_CODEC_WM8510)

#elif defined(CONFIG_RTK_VOIP_DRIVERS_CODEC_ALC5621)
static void serial_write(rtl8954C_i2c_dev_t* pI2C_Dev, unsigned char *data)
{
	int i;
	char j;
	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	for (j=7;j>=0;j--) {
		_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
		_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, ((*data)>>j)&0x00000001);//write data,from MSB to LSB
		//delay 2 us.
		#ifdef __kernel_used__
		udelay(2*I2C_RATING_FACTOR);
		#endif
		#ifdef __test_program_used__
		for (i=0;i<200*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
		#endif
		_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
		//delay 1 us.
		#ifdef __kernel_used__
		udelay(1*I2C_RATING_FACTOR);
		#endif
		#ifdef __test_program_used__
		for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
		#endif
	}
	return;
}

static unsigned char ACK(rtl8954C_i2c_dev_t* pI2C_Dev)
{
	int i;
	unsigned int buf;
#if 0
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 0); /* fall down sdio*/
	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);//change sdio to input
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
#endif
	//_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 0); /* fall down sdio*/

	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);//change sdio to input
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif

	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif

	_rtl8954C_getGpioDataBit(pI2C_Dev->sdio,&buf);
	if (buf != 0)
		printk("NO ACK\n");
	//_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	#if 0
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	#endif
	return buf;
}
static void ACK_w(rtl8954C_i2c_dev_t* pI2C_Dev, unsigned char data)
{
	int i;

#if 0
	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, data); /* sdio send 0 for ACK, 1 for NACK*/
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
#endif
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	//for (i=0;i<500*I2C_RATING_FACTOR;i++);

	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, data); /* sdio send 0 for ACK, 1 for NACK*/

	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif

	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif

	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);//change sdio to input
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	#if 0
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	#endif
	return;
}
#endif

static void serial_read(rtl8954C_i2c_dev_t* pI2C_Dev, unsigned short int *data)
{
	int i;
	char j;
	unsigned int buf;

	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_IN, GPIO_INT_DISABLE);//change sdio to input
	for (j=7;j>=0;j--) {
		_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
		//delay 2 us.
		#ifdef __kernel_used__
		udelay(2*I2C_RATING_FACTOR);
		#endif
		#ifdef __test_program_used__
		for (i=0;i<200*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
		#endif
		_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
		_rtl8954C_getGpioDataBit( pI2C_Dev->sdio, &buf);//read data,from MSB to LSB
		*data |= (buf<<j);
		//delay 1 us.
		#ifdef __kernel_used__
		udelay(1*I2C_RATING_FACTOR);
		#endif
		#ifdef __test_program_used__
		for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
		#endif
	}
	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	return;
}



#if defined(CONFIG_RTK_VOIP_DRIVERS_CODEC_WM8510)


#elif defined(CONFIG_RTK_VOIP_DRIVERS_CODEC_ALC5621)

/*
@func int | _rtl8954C_i2c_rawWrite_alc5621 | Write several bits to device
@parm rtl8954C_i2c_dev_t* | pI2C_Dev | Structure containing device information
@parm unsigned char* | pDEV_ID | i2c id address
@parm unsigned char* | pReg | i2c register address
@parm unsigned short int* | pData | i2c data
@comm
*/
static void _rtl8954C_i2c_rawWrite_alc5621( rtl8954C_i2c_dev_t* pI2C_Dev, unsigned char *pDEV_ID, unsigned char *pReg, unsigned short int *pData)
{
	int i;
	//char j;
	//unsigned int buf;
	unsigned char dev_id, reg, data_hibyte, data_lowbyte;

	if ((pData == NULL) || (pDEV_ID == NULL) || (pReg == NULL)) {
		printk("Wrong I2C write function\n");
		return;
	}

start_condition:

	dev_id = (*pDEV_ID<<1) & 0xfe; //shift DEV_ID 1-bit left and unset in writting operation
	reg = *pReg;
	data_hibyte =(unsigned char) (*pData>>8);
	data_lowbyte =(unsigned char) (*pData & 0xff);
	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 1); /* raise sdio*/
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 0); /* fall down sdio*//*start condition*/
	//delay 2 us.
	#ifdef __kernel_used__
	udelay(2*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<200*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif


	serial_write(pI2C_Dev,&dev_id);//write pDEV_ID,from MSB to LSB
	if (ACK(pI2C_Dev) != 0)
		goto start_condition;

	serial_write(pI2C_Dev,&reg);//write pReg,from MSB to LSB
	if (ACK(pI2C_Dev) != 0)
		goto start_condition;

	serial_write(pI2C_Dev,&data_hibyte);//write pData(hibtye),from MSB to LSB (bit15 - bit 8)
	if (ACK(pI2C_Dev) != 0)
		goto start_condition;

	serial_write(pI2C_Dev,&data_lowbyte);//write pData(lowbtye),from MSB to LSB (bit7 - bit 0)
	if (ACK(pI2C_Dev) != 0)
		goto start_condition;

	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 1); /* raise sdio*//*stop condition*/

	return;
}


/*
@func int | _rtl8954C_i2c_rawRead | Read several bits from device
@parm rtl8954C_i2c_dev_t* | pI2C_Dev | Structure containing device information
@parm unsigned char* | pDEV_ID | i2c id address
@parm unsigned char* | pReg | i2c register address
@parm unsigned short int* | pData | i2c data
@comm
*/
static void _rtl8954C_i2c_rawRead_alc5621( rtl8954C_i2c_dev_t* pI2C_Dev, unsigned char *pDEV_ID, unsigned char *pReg, unsigned short int *pData)
{
	int i;
	unsigned short int buf;
	unsigned char dev_id, reg;
	if ((pData == NULL) || (pDEV_ID == NULL) || (pReg == NULL)) {
		printk("Wrong I2C Read function\n");
		return;
	}

start_condition_read:


	//dev_id = (*pDEV_ID<<1) | 0x01;	//shift DEV_ID 1-bit left and set bit0 in reading operation
	dev_id = (*pDEV_ID<<1) & 0xfe; //shift DEV_ID 1-bit left and unset in writting operation
	reg = *pReg;
	*pData = 0;
	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 1); /* raise sdio*/
	for (i=0;i<100;i++) asm volatile (" nop \n\t");
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 0); /* fall down sdio*//*start condition*/
	//delay 2 us.
	#ifdef __kernel_used__
	udelay(2*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<200*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif

	serial_write(pI2C_Dev,&dev_id);//write pDEV_ID,from MSB to LSB
	if (ACK(pI2C_Dev) != 0)
		goto start_condition_read;

	serial_write(pI2C_Dev,&reg);//write pReg,from MSB to LSB
	if (ACK(pI2C_Dev) != 0)
		goto start_condition_read;

	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif

	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 1); /* raise sdio*/
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 0); /* fall down sdio*//*start condition*/
	//delay 2 us.
	#ifdef __kernel_used__
	udelay(2*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<200*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif

	dev_id = (*pDEV_ID<<1) | 0x01;	//shift DEV_ID 1-bit left and set bit0 in reading operation

	serial_write(pI2C_Dev,&dev_id);//write pDEV_ID,from MSB to LSB
	if (ACK(pI2C_Dev) != 0)
		goto start_condition_read;

	buf=0;		//init buf data to 0
	serial_read(pI2C_Dev,&buf);//read high byte data from device
	ACK_w(pI2C_Dev, 0);	//write ACK

	serial_read(pI2C_Dev,pData);//read low byte data from device
	ACK_w(pI2C_Dev, 1);	//write NACK

	*pData = *pData | (buf <<8);

	_rtl8954C_initGpioPin(pI2C_Dev->sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 0); /* fall down sdio*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	_rtl8954C_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	//delay 1 us.
	#ifdef __kernel_used__
	udelay(1*I2C_RATING_FACTOR);
	#endif
	#ifdef __test_program_used__
	for (i=0;i<100*I2C_RATING_FACTOR;i++) asm volatile (" nop ");
	#endif
	_rtl8954C_setGpioDataBit( pI2C_Dev->sdio, 1); /* raise sdio*//*stop condition*/

	return;

}

#endif

/************************* I2C data struct ****************************************/
static rtl8954C_i2c_dev_t i2c_dev;

//-------------------  I2C API ------------------------------//

void init_i2c_gpio(void)
{
	printk("( GPIO %s )  ", GPIO_I2C );

	i2c_dev.sclk = SCLK_PIN;
	i2c_dev.sdio = SDIO_PIN;

	_rtl8954C_initGpioPin(i2c_dev.sclk, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
	_rtl8954C_initGpioPin(i2c_dev.sdio, GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
	printk("For I2C port init=> OK !\n");
	return;
}


#if defined(CONFIG_RTK_VOIP_DRIVERS_CODEC_WM8510)

#elif defined(CONFIG_RTK_VOIP_DRIVERS_CODEC_ALC5621)
unsigned short int read_ALC5621(unsigned char Reg)
{
	unsigned short int buf;
	const unsigned char dev_id = 0x1A;
#ifdef _ALC5621_CLI_PROTECT
	unsigned long flags;
	save_flags(flags); cli();
#endif

	_rtl8954C_i2c_rawRead_alc5621(&i2c_dev,&dev_id,&Reg,&buf);
#ifdef _ALC5621_CLI_PROTECT
	restore_flags(flags);
#endif
	return buf;
}

void write_ALC5621(unsigned char Reg, unsigned short int data)
{
	const unsigned char dev_id = 0x1A;
#ifdef _ALC5621_CLI_PROTECT
	unsigned long flags;
	save_flags(flags); cli();
#endif

	_rtl8954C_i2c_rawWrite_alc5621(&i2c_dev,&dev_id,&Reg,&data);
#ifdef _ALC5621_CLI_PROTECT
	restore_flags(flags);
#endif
	return;
}

unsigned short int read_ALC5621_hidden(unsigned char index)
{

	write_ALC5621(0x6a, index);

	return read_ALC5621(0x6c);
}

void write_ALC5621_hidden(unsigned char index, unsigned short int data)
{
	write_ALC5621(0x6a, index);

	write_ALC5621(0x6c, data);
}

#endif

