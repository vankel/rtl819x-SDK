/* jason++ 2005/04/18 */
//#include "gpio.h"
#include "proslic.h"
//#include "spi.h"
#include <linux/config.h>
//#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/string.h>

#define printk prom_printf
#define printf	printk

#ifndef SUCCESS
#define SUCCESS 	0
#endif
#ifndef FAILED
#define FAILED 	-1
#endif

//---chiminer 2005-12-27-----------
//It's for SPI daisy-chain mode.It must be the same with spi.c.
#define chiminer_daisy_chain 1
//The variable is global. It avoids to change prototype of the function.
unsigned char slic_order;
//------------------------------


/*********** Select the Si3210 or Si3215 IndirectReg Array ***************/
#if 0
	#define _Si3210_IndirectReg_
#else
	#define _Si3215_IndirectReg_
#endif
/*****************************************************************/

/*
unsigned char *SPI_CTRL_PORT	= (unsigned char *) PEDATA;
unsigned char *SPI_DIR_PORT	= (unsigned char *) PEDIR;
unsigned char *SPI_CNR_PORT	= (unsigned char *) PECNR;
#define SPICTRL		(*(unsigned char *)SPI_CTRL_PORT)
#define GPIODIR		(*(unsigned char *)SPI_DIR_PORT)
#define GPIOCNR		(*(unsigned char *)SPI_CNR_PORT)
*/

static chipStruct chipData ; /* Represents a proslics state, cached information, and timers */

/****************** COMMON TELEPHONEY TONES*********************************/
tone_struct DialTone = {  /* OSC1= 350 Hz OSC2= 440 Hz .0975 Volts -18 dBm */
	{0x7b30,0x0063,0,0,0,0,0},{0x7870,0x007d,0,0,0,0,0}
};
tone_struct ReorderTone = {	/* OSC1= 480 Hz OSC2 = 620 .0975 Volts -18 dBm */
	{0x7700,0x0089,0,0x09,0x60,0x06,0x40},{0x7120,0x00b2,0,0x09,0x60,0x06,0x40}
};
tone_struct CongestionTone = { /* OSC1= 480 Hz OSC2 = 620 .0975 Volts -18 dBM */
	{0x7700,0x0089,0,0x06,0x40,0x09,0x60},{0x7120,0x00b2,0,0x06,0x40,0x09,0x60}
};
tone_struct RingbackPBX = {	/* OSC1 = 440 Hz OSC2= 480 .0975 Volts -18 dBM */
	{0x7870,0x007d,0,0x1f,0x40,0x5d,0xc0},{0x7700,0x0089,0,0x1f,0x40,0x5d,0xc0}
};
tone_struct RingbackNormal = { /* OSC1 = 440 Hz OSC2 = 480 .0975 Volts -18 dBm */
	{0x7870,0x007d,0,0x3e,0x80,0x7d,0x00},{0x7700,0x0089,0,0x3e,0x80,0x7d,0x00}
};
tone_struct BusySignal = { /* OSC1= 480  OSC2 = 620 .0975 Voltz -18 dBm 8*/
	{0x7700,0x0089,0,0x0f,0xa0,0x0f,0xa0},{0x7120,0x00b2,0,0x0f,0xa0,0x0f,0xa0}
};

tone_struct RingbackJapan = { /* OSC1 = 400 Hz OSC2 = 435 .0975 Volts -18 dBm */
	{0x79c0,0x00e9,0,0x1f,0x40,0x3e,0x80},{0x7940,0x00f2,0,0x1f,0x40,0x3e,0x80}

};


tone_struct BusyJapan = { /* OSC1 = 400 Hz OSC2 = 435 .0975 Volts -18 dBm */
	{0x79c0,0x00e9,0,0x0f,0xa0,0x0f,0xa0},{0,0,0,0,0,0,0}

};

tone_struct JapanDialTone = { /* OSC1 = 400 Hz OSC2 = 435 .0975 Volts -18 dBm */
	{0x79c0,0x00e9,0,0,0,0,0},{0,0,0,0,0,0,0}

};

/*************************0.0 Dbm0 tones*****************************************/
tone_struct dual_tones[]= { // Define(s) Touch-Tones & Call progress tones
	// OSC 1  x     y on_h on_l			OSC 2
	{{0x6d50,0x0643,0,0x03,0x20,0,0}, {0x4a80,0x0b79,0,0x03,0x20,0,0}}, // DTMF 1
	{{0x6d50,0x0643,0,0x03,0x20,0,0}, {0x3fc0,0x0cec,0,0x03,0x20,0,0}}, // DTMF 2
	{{0x6d50,0x0643,0,0x03,0x20,0,0}, {0x3320,0x0ea0,0,0x03,0x20,0,0}}, // DTMF 3
	{{0x6d50,0x0643,0,0x03,0x20,0,0}, {0x2460,0x10ab,0,0x03,0x20,0,0}}, // DTMF A
	{{0x6950,0x06f6,0,0x03,0x20,0,0}, {0x4a80,0x0b79,0,0x03,0x20,0,0}}, // DTMF 4
	{{0x6950,0x06f6,0,0x03,0x20,0,0}, {0x3fc0,0x0cec,0,0x03,0x20,0,0}}, // DTMF 5
	{{0x6950,0x06f6,0,0x03,0x20,0,0}, {0x3320,0x0ea0,0,0x03,0x20,0,0}}, // DTMF 6
	{{0x6950,0x06f6,0,0x03,0x20,0,0}, {0x2460,0x10ab,0,0x03,0x20,0,0}}, // DTMF B
	{{0x6460,0x07c3,0,0x03,0x20,0,0}, {0x4a80,0x0b79,0,0x03,0x20,0,0}}, // DTMF 7
	{{0x6460,0x07c3,0,0x03,0x20,0,0}, {0x3fc0,0x0cec,0,0x03,0x20,0,0}}, // DTMF 8
	{{0x6460,0x07c3,0,0x03,0x20,0,0}, {0x3320,0x0ea0,0,0x03,0x20,0,0}}, // DTMF 9
	{{0x6460,0x07c3,0,0x03,0x20,0,0}, {0x2460,0x10ab,0,0x03,0x20,0,0}}, // DTMF C
	{{0x5ea0,0x08a5,0,0x03,0x20,0,0}, {0x4a80,0x0b79,0,0x03,0x20,0,0}}, // DTMF *
	{{0x5ea0,0x08a5,0,0x03,0x20,0,0}, {0x3fc0,0x0cec,0,0x03,0x20,0,0}}, // DTMF 0
	{{0x5ea0,0x08a5,0,0x03,0x20,0,0}, {0x3320,0x0ea0,0,0x03,0x20,0,0}}, // DTMF #
	{{0x5ea0,0x08a5,0,0x03,0x20,0,0}, {0x2460,0x10ab,0,0x03,0x20,0,0}}  // DTMF D
};

/* Direct Register Initial Value */
#define	INIT_DR0	0X00	//	Serial Interface
#define	INIT_DR1	0X38	//	PCM Mode	pcm linear mode
#define	INIT_DR2	0X00	//	PCM TX Clock Slot Low Byte (1 PCLK cycle/LSB)
#define	INIT_DR3	0x00	//	PCM TX Clock Slot High Byte
#define	INIT_DR4	0x00	//	PCM RX Clock Slot Low Byte (1 PCLK cycle/LSB)
#define	INIT_DR5	0x00	//	PCM RX Clock Slot High Byte
#define	INIT_DR6	0x00	//	DIO Control (external battery operation, Si3211/12)
#define	INIT_DR8	0X00	//	Loopbacks (digital loopback default)
#define	INIT_DR9	0x00	//	Transmit and receive path gain and control
#define	INIT_DR10	0X28	//0x28	Initialization Two-wire impedance (600  and enabled)
#define	INIT_DR11	0x33	//	Transhybrid Balance/Four-wire Return Loss
#define	INIT_DR14	0X10	//	Powerdown Control 1
#define	INIT_DR15	0x00	//	Initialization Powerdown Control 2
#define	INIT_DR18	0xff	//	Normal Oper. Interrupt Register 1 (clear with 0xFF)
#define	INIT_DR19	0xff	//	Normal Oper. Interrupt Register 2 (clear with 0xFF)
#define	INIT_DR20	0xff	//	Normal Oper. Interrupt Register 3 (clear with 0xFF)
#define	INIT_DR21	0xff	//	Interrupt Mask 1
#define	INIT_DR22	0xff	//	Initialization Interrupt Mask 2
#define	INIT_DR23	0xff	//	 Initialization Interrupt Mask 3
#define	INIT_DR32	0x00	//	Oper. Oscillator 1 Control—tone generation
#define	INIT_DR33	0x00	//	Oper. Oscillator 2 Control—tone generation
#define	INIT_DR34	0X18	//	34 0x22 0x00 Initialization Ringing Oscillator Control
#define	INIT_DR35	0x00	//	Oper. Pulse Metering Oscillator Control
#define	INIT_DR36	0x00	//	36 0x24 0x00 Initialization OSC1 Active Low Byte (125 µs/LSB)
#define	INIT_DR37	0x00	//	37 0x25 0x00 Initialization OSC1 Active High Byte (125 µs/LSB)
#define	INIT_DR38	0x00	//	38 0x26 0x00 Initialization OSC1 Inactive Low Byte (125 µs/LSB)
#define	INIT_DR39	0x00	//	39 0x27 0x00 Initialization OSC1 Inactive High Byte (125 µs/LSB)
#define	INIT_DR40	0x00	//	40 0x28 0x00 Initialization OSC2 Active Low Byte (125 µs/LSB)
#define	INIT_DR41	0x00	//	41 0x29 0x00 Initialization OSC2 Active High Byte (125 µs/LSB)
#define	INIT_DR42	0x00	//	42 0x2A 0x00 Initialization OSC2 Inactive Low Byte (125 µs/LSB)
#define	INIT_DR43	0x00	//	43 0x2B 0x00 Initialization OSC2 Inactive High Byte (125 µs/LSB)
#define	INIT_DR44	0x00	//	44 0x2C 0x00 Initialization Pulse Metering Active Low Byte (125 µs/LSB)
#define	INIT_DR45	0x00	//	45 0x2D 0x00 Initialization Pulse Metering Active High Byte (125 µs/LSB)
#define	INIT_DR46	0x00	//	46 0x2E 0x00 Initialization Pulse Metering Inactive Low Byte (125 µs/LSB)
#define	INIT_DR47	0x00	//	47 0x2F 0x00 Initialization Pulse Metering Inactive High Byte (125 µs/LSB)
#define	INIT_DR48	0X80	//	48 0x30 0x00 0x80 Initialization Ringing Osc. Active Timer Low Byte (2 s,125 µs/LSB)
#define	INIT_DR49	0X3E	//	49 0x31 0x00 0x3E Initialization Ringing Osc. Active Timer High Byte (2 s,125 µs/LSB)
#define	INIT_DR50	0X00	//	50 0x32 0x00 0x00 Initialization Ringing Osc. Inactive Timer Low Byte (4 s, 125 µs/LSB)
#define	INIT_DR51	0X7D	//	51 0x33 0x00 0x7D Initialization Ringing Osc. Inactive Timer High Byte (4 s, 125 µs/LSB)
#define	INIT_DR52	0X00	//	52 0x34 0x00 Normal Oper. FSK Data Bit
#define	INIT_DR63	0X54	//	63 0x3F 0x54 Initialization Ringing Mode Loop Closure Debounce Interval
#define	INIT_DR64	0x00	//	64 0x40 0x00 Normal Oper. Mode Byte—primary control
#define	INIT_DR65	0X61	//	65 0x41 0x61 Initialization External Bipolar Transistor Settings
#define	INIT_DR66	0X03	//	66 0x42 0x03 Initialization Battery Control
#define	INIT_DR67	0X1F	//	67 0x43 0x1F Initialization Automatic/Manual Control
#define	INIT_DR69	0X0C	//	69 0x45 0x0A 0x0C Initialization Loop Closure Debounce Interval (1.25 ms/LSB)
#define	INIT_DR70	0X0A	//	70 0x46 0x0A Initialization Ring Trip Debounce Interval (1.25 ms/LSB)
#define	INIT_DR71	0X01	//	71 0x47 0x00 0x01 Initialization Off-Hook Loop Current Limit (20 mA + 3 mA/LSB)
#define	INIT_DR72	0X20	//	72 0x48 0x20 Initialization On-Hook Voltage (open circuit voltage) = 48 V(1.5 V/LSB)
#define	INIT_DR73	0X02	//	73 0x49 0x02 Initialization Common Mode Voltage—VCM = ? V(?.5 V/LSB)
#define	INIT_DR74	0X32	//	74 0x4A 0x32 Initialization VBATH (ringing) = ?5 V (?.5 V/LSB)
#define	INIT_DR75	0X10	//	75 0x4B 0x10 Initialization VBATL (off-hook) = ?4 V (TRACK = 0)(?.5 V/LSB)
#define	INIT_DR92	0x7f	//	92 0x5C  7F Initialization DC–DC Converter PWM Period (61.035 ns/LSB)
#define	INIT_DR93	0x14	//	93 0x5D 0x14 0x19 Initialization DC–DC Converter Min. Off Time (61.035 ns/LSB)
#define	INIT_DR96	0x00	//	96 0x60 0x1F Initialization Calibration Control Register 1(written second and starts calibration)
#define	INIT_DR97	0X1F	//	97 0x61 0x1F Initialization Calibration Control Register 2(written before Register 96)
#define	INIT_DR98	0X10	//	98 0x62 0x10 Informative Calibration result (see data sheet)
#define	INIT_DR99	0X10	//	99 0x63 0x10 Informative Calibration result (see data sheet)
#define	INIT_DR100	0X11	//	100 0x64 0x11 Informative Calibration result (see data sheet)
#define	INIT_DR101	0X11	//	101 0x65 0x11 Informative Calibration result (see data sheet)
#define	INIT_DR102	0x08	//	102 0x66 0x08 Informative Calibration result (see data sheet)
#define	INIT_DR103	0x88	//	103 0x67 0x88 Informative Calibration result (see data sheet)
#define	INIT_DR104	0x00	//	104 0x68 0x00 Informative Calibration result (see data sheet)
#define	INIT_DR105	0x00	//	105 0x69 0x00 Informative Calibration result (see data sheet)
#define	INIT_DR106	0x20	//	106 0x6A 0x20 Informative Calibration result (see data sheet)
#define	INIT_DR107	0x08	//	107 0x6B 0x08 Informative Calibration result (see data sheet)
#define	INIT_DR108	0xEB	//	108 0x63 0x00 0xEB Initialization Feature enhancement register
#define 	INIT_SI3210M_DR92	0x60  //  92 0x60 Initialization DC–DC Converter PWM Period (61.035 ns/LSB)
#define 	INIT_SI3210M_DR93 	0x38  //  92 0x60 Initialization DC–DC Converter PWM Period (61.035 ns/LSB)

#define 	DISABLE_ALL_DR21 0
#define 	DISABLE_ALL_DR22 0
#define 	DISABLE_ALL_DR23 0
#define	OPEN_DR64		     0
#define	STANDARD_CAL_DR97		0x18	/*	Calibrations without the ADC and DAC offset and without common mode calibration. */
#define	STANDARD_CAL_DR96		0x47	/*	Calibrate common mode and differential DAC mode DAC + ILIM */
#define 	ENB2_DR23  				1<<2	/* enable interrupt for the balance Cal */
#define	BIT_CALCM_DR97			0x01	/*	CALCM Common Mode Balance Calibration. */


/******************************************************/

#ifdef _Si3210_IndirectReg_
#define Si3210_on_off 1
#else
#define Si3210_on_off 0
#endif

#if Si3210_on_off

/* Si3210: The following Array contains: */
indirectRegister  indirectRegisters[] =
{
/* Reg#			Label		Initial Value  */

{	0,	"DTMF_ROW_0_PEAK",	0x55C2	},
{	1,	"DTMF_ROW_1_PEAK",	0x51E6	},
{	2,	"DTMF_ROW2_PEAK",	0x4B85	},
{	3,	"DTMF_ROW3_PEAK",	0x4937	},
{	4,	"DTMF_COL1_PEAK",	0x3333	},
{	5,	"DTMF_FWD_TWIST",	0x0202	},
{	6,	"DTMF_RVS_TWIST",	0x0202	},
{	7,	"DTMF_ROW_RATIO",	0x0198	},
{	8,	"DTMF_COL_RATIO",	0x0198	},
{	9,	"DTMF_ROW_2ND_ARM",	0x0611	},
{	10,	"DTMF_COL_2ND_ARM",	0x0202	},
{	11,	"DTMF_PWR_MIN_",	0x00E5	},
{	12,	"DTMF_OT_LIM_TRES",	0x0A1C	},
{	13,	"OSC1_COEF",		0x7b30	},
{	14,	"OSC1X",		0x0063	},
{	15,	"OSC1Y",		0x0000	},
{	16,	"OSC2_COEF",		0x7870	},
{	17,	"OSC2X",		0x007d	},
{	18,	"OSC2Y",		0x0000	},
{	19,	"RING_V_OFF",		0x0000	},
{	20,	"RING_OSC",		0x7EF0	},
{	21,	"RING_X",		0x0160	},
{	22,	"RING_Y",		0x0000	},
{	23,	"PULSE_ENVEL",		0x2000	},
{	24,	"PULSE_X",		0x2000	},
{	25,	"PULSE_Y",		0x0000	},
#if 1
{	26,	"RECV_DIGITAL_GAIN",	0x4000	},
{	27,	"XMIT_DIGITAL_GAIN",	0x4000	},
#else
{	26,	"RECV_DIGITAL_GAIN",	0x1000	},
{	27,	"XMIT_DIGITAL_GAIN",	0x1000	},
#endif
{	28,	"LOOP_CLOSE_TRES",	0x1000	},
{	29,	"RING_TRIP_TRES",	0x3600	},
{	30,	"COMMON_MIN_TRES",	0x1000	},
#if 1
{	31,	"COMMON_MAX_TRES",	0x0080	},
#else
{	31,	"COMMON_MAX_TRES",	0x0200	},
#endif
{	32,	"PWR_ALARM_Q1Q2",	0x07c0	},
{	33,	"PWR_ALARM_Q3Q4",	0x376f	},
{	34,	"PWR_ALARM_Q5Q6",	0x1B80	},
{	35,	"LOOP_CLSRE_FlTER",	0x8000	},
{	36,	"RING_TRIP_FILTER",	0x0320	},
{	37,	"TERM_LP_POLE_Q1Q2",	0x008c	},
#if 1
{	38,	"TERM_LP_POLE_Q3Q4",	0x008c	},
#else
{	38,	"TERM_LP_POLE_Q3Q4",	0x0100	},
#endif
{	39,	"TERM_LP_POLE_Q5Q6",	0x0010	},
#if 1
{	40,	"CM_BIAS_RINGING",	0x0200	},
#else
{	40,	"CM_BIAS_RINGING",	0x0C00	},
#endif
{	41,	"DCDC_MIN_V",		0x0C00	},
{	43,	"LOOP_CLOSE_TRES Low",	0x1000	},
{	99,	"FSK 0 FREQ PARAM",	0x00DA	},
{	100,	"FSK 0 AMPL PARAM",	0x6B60	},
{	101,	"FSK 1 FREQ PARAM",	0x0074	},
{	102,	"FSK 1 AMPl PARAM",	0x79C0	},
{	103,	"FSK 0to1 SCALER",	0X1120	},
{	104,	"FSK 1to0 SCALER",	0x3BE0	},
{	97,	"RCV_FLTR",		0	},
{0,"",0},
};

#endif

//------------------------------------------

#ifdef _Si3215_IndirectReg_
#define Si3215_on_off 1
#else
#define Si3215_on_off 0
#endif

#if Si3215_on_off
/* Si3215: The following Array contains: */
indirectRegister  indirectRegisters[] =
{
/* Reg#			Label		Initial Value  */

{	0,	"OSC1_COEF",		0x7b30	},
{	1,	"OSC1X",				0x0063	},
{	2,	"OSC1Y",				0x0000	},
{	3,	"OSC2_COEF",		0x7870	},
{	4,	"OSC2X",				0x007d	},
{	5,	"OSC2Y",				0x0000	},
{	6,	"RING_V_OFF",		0x0000	},
{	7,	"RING_OSC",			0x7EF0	},
{	8,	"RING_X",			0x0160	},
{	9,	"RING_Y",			0x0000	},
{	10,	"PULSE_ENVEL",		0x2000	},
{	11,	"PULSE_X",			0x2000	},
{	12,	"PULSE_Y",			0x0000	},
#if 1
{	13,	"RECV_DIGITAL_GAIN",	0x4000	},
{	14,	"XMIT_DIGITAL_GAIN",	0x4000	},
#else
{	13,	"RECV_DIGITAL_GAIN",	0x1000	},
{	14,	"XMIT_DIGITAL_GAIN",	0x1000	},
#endif
{	15,	"LOOP_CLOSE_TRES",	0x1000	},
{	16,	"RING_TRIP_TRES",	0x3600	},
{	17,	"COMMON_MIN_TRES",	0x1000	},
#if 1
{	18,	"COMMON_MAX_TRES",	0x0080	},
#else
{	18,	"COMMON_MAX_TRES",	0x0200	},
#endif
{	19,	"PWR_ALARM_Q1Q2",	0x07c0	},
{	20,	"PWR_ALARM_Q3Q4",	0x376f	},
{	21,	"PWR_ALARM_Q5Q6",	0x1B80	},
{	22,	"LOOP_CLSRE_FlTER",	0x8000	},
{	23,	"RING_TRIP_FILTER",	0x0320	},
{	24,	"TERM_LP_POLE_Q1Q2",0x008c	},
#if 1
{	25,	"TERM_LP_POLE_Q3Q4",0x008c	},
#else
{	25,	"TERM_LP_POLE_Q3Q4",0x0100	},
#endif
{	26,	"TERM_LP_POLE_Q5Q6",0x0010	},
#if 1
{	27,	"CM_BIAS_RINGING",	0x0200	},
#else
{	27,	"CM_BIAS_RINGING",	0x0C00	},
#endif
{	64,	"DCDC_MIN_V",		0x0C00	},
{	66,	"LOOP_CLOSE_TRES Low",0x1000},
{	69,	"FSK 0 FREQ PARAM",	0x00DA	},
{	70,	"FSK 0 AMPL PARAM",	0x6B60	},
{	71,	"FSK 1 FREQ PARAM",	0x0074	},
{	72,	"FSK 1 AMPl PARAM",	0x79C0	},
{	73,	"FSK 0to1 SCALER",	0X1120	},
{	74,	"FSK 1to0 SCALER",	0x3BE0	},
//{	97,	"RCV_FLTR",		0	},
{0,"",0},
};

#endif

//--------------------------------------------

// thlin++ 2005-08-10
#define CHANNEL 2


//-- thlin ++ 2005-08-10 ----
#define RTL8186_TESTPROG

#ifdef RTL8186_TESTPROG
#define jiffies get_timer_jiffies()
//extern unsigned long volatile jiffies;
#endif

#define cyg_current_time	get_timer_jiffies
//------------------------

//extern void show_PCLK_freq(void);
extern void writeDirectReg(unsigned int address, unsigned int data);
extern unsigned char readDirectReg(unsigned int address);
extern void waitForIndirectReg(void);
extern unsigned short readIndirectReg(unsigned char address);
extern void writeIndirectReg(unsigned char address, unsigned short data);
extern void cyg_thread_delay(int delay);
typedef unsigned long cyg_tick_count_t;

//------------------------------------------------------------------------------

/*void waitForIndirectReg(void)
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
}*/

/******************************************************************************/

void initializeDirectRegisters(int ch_)
{
	#if chiminer_daisy_chain
		writeDirectReg(0,	0x80	);/*0X80    Serial Interface for daisy-chain mode*/
	#else
		writeDirectReg(0,	INIT_DR0	);/*0X00	Serial Interface */
	#endif
	writeDirectReg(1,	INIT_DR1	);/*0X38	PCM Mode */
	if (slic_order == 1) {//time slot 0.chiminer definiation
		writeDirectReg(2,	0x01	);
		writeDirectReg(3,	INIT_DR3	);/*0x00	PCM TX Clock Slot High Byte */
		writeDirectReg(4,	0x01	);
		writeDirectReg(5,	INIT_DR5	);/*0x00	PCM RX Clock Slot High Byte */
		printk("slic_order = %d\n",slic_order);
	} else {		//time slot 1.chiminer definiation
		writeDirectReg(2,	0x09	);
		writeDirectReg(3,	INIT_DR3	);/*0x00	PCM TX Clock Slot High Byte */
		writeDirectReg(4,	0x09	);
		writeDirectReg(5,	INIT_DR5	);/*0x00	PCM RX Clock Slot High Byte */
		printk("slic_order = %d\n",slic_order);
	}
	writeDirectReg(8,	INIT_DR8	);/*0X00	Loopbacks (digital loopback default) */
	writeDirectReg(9,	INIT_DR9	);/*0x00	Transmit and receive path gain and control */
	writeDirectReg(10,	INIT_DR10	);/*0X28	Initialization Two-wire impedance (600  and enabled) */
	writeDirectReg(11,	INIT_DR11	);/*0x33	Transhybrid Balance/Four-wire Return Loss */
	//writeDirectReg(11,	0x36		);// has near echo from slic
	writeDirectReg(18,	INIT_DR18	);/*0xff	Normal Oper. Interrupt Register 1 (clear with 0xFF) */
	writeDirectReg(19,	INIT_DR19	);/*0xff	Normal Oper. Interrupt Register 2 (clear with 0xFF) */
	writeDirectReg(20,	INIT_DR20	);/*0xff	Normal Oper. Interrupt Register 3 (clear with 0xFF) */
	writeDirectReg(21,	INIT_DR21	);/*0xff	Interrupt Mask 1 */
	writeDirectReg(22,	INIT_DR22	);/*0xff	Initialization Interrupt Mask 2 */
	writeDirectReg(23,	INIT_DR23	);/*0xff	 Initialization Interrupt Mask 3 */
	writeDirectReg(32,	INIT_DR32	);/*0x00	Oper. Oscillator 1 Controltone generation */
	writeDirectReg(33,	INIT_DR33	);/*0x00	Oper. Oscillator 2 Controltone generation */
	writeDirectReg(34,	INIT_DR34	);/*0X18	34 0x22 0x00 Initialization Ringing Oscillator Control */
	writeDirectReg(35,	INIT_DR35	);/*0x00	Oper. Pulse Metering Oscillator Control */
	writeDirectReg(36,	INIT_DR36	);/*0x00	36 0x24 0x00 Initialization OSC1 Active Low Byte (125 µs/LSB) */
	writeDirectReg(37,	INIT_DR37	);/*0x00	37 0x25 0x00 Initialization OSC1 Active High Byte (125 µs/LSB) */
	writeDirectReg(38,	INIT_DR38	);/*0x00	38 0x26 0x00 Initialization OSC1 Inactive Low Byte (125 µs/LSB) */
	writeDirectReg(39,	INIT_DR39	);/*0x00	39 0x27 0x00 Initialization OSC1 Inactive High Byte (125 µs/LSB) */
	writeDirectReg(40,	INIT_DR40	);/*0x00	40 0x28 0x00 Initialization OSC2 Active Low Byte (125 µs/LSB) */
	writeDirectReg(41,	INIT_DR41	);/*0x00	41 0x29 0x00 Initialization OSC2 Active High Byte (125 µs/LSB) */
	writeDirectReg(42,	INIT_DR42	);/*0x00	42 0x2A 0x00 Initialization OSC2 Inactive Low Byte (125 µs/LSB) */
	writeDirectReg(43,	INIT_DR43	);/*0x00	43 0x2B 0x00 Initialization OSC2 Inactive High Byte (125 µs/LSB) */
	writeDirectReg(44,	INIT_DR44	);/*0x00	44 0x2C 0x00 Initialization Pulse Metering Active Low Byte (125 µs/LSB) */
	writeDirectReg(45,	INIT_DR45	);/*0x00	45 0x2D 0x00 Initialization Pulse Metering Active High Byte (125 µs/LSB) */
	writeDirectReg(46,	INIT_DR46	);/*0x00	46 0x2E 0x00 Initialization Pulse Metering Inactive Low Byte (125 µs/LSB) */
	writeDirectReg(47,	INIT_DR47	);/*0x00	47 0x2F 0x00 Initialization Pulse Metering Inactive High Byte (125 µs/LSB) */
	writeDirectReg(48,	INIT_DR48	);/*0X80	48 0x30 0x00 0x80 Initialization Ringing Osc. Active Timer Low Byte (2 s,125 µs/LSB) */
	writeDirectReg(49,	INIT_DR49	);/*0X3E	49 0x31 0x00 0x3E Initialization Ringing Osc. Active Timer High Byte (2 s,125 µs/LSB) */
	writeDirectReg(50,	INIT_DR50	);/*0X00	50 0x32 0x00 0x00 Initialization Ringing Osc. Inactive Timer Low Byte (4 s, 125 µs/LSB) */
	writeDirectReg(51,	INIT_DR51	);/*0X7D	51 0x33 0x00 0x7D Initialization Ringing Osc. Inactive Timer High Byte (4 s, 125 µs/LSB) */
	writeDirectReg(52,	INIT_DR52	);/*0X00	52 0x34 0x00 Normal Oper. FSK Data Bit */
	writeDirectReg(63,	INIT_DR63	);/*0X54	63 0x3F 0x54 Initialization Ringing Mode Loop Closure Debounce Interval */
	writeDirectReg(64,	INIT_DR64	);/*0x00	64 0x40 0x00 Normal Oper. Mode Byte—primary control */
	writeDirectReg(65,	INIT_DR65	);/*0X61	65 0x41 0x61 Initialization External Bipolar Transistor Settings */
	writeDirectReg(66,	INIT_DR66	);/*0X03	66 0x42 0x03 Initialization Battery Control */
	writeDirectReg(67,	INIT_DR67	);/*0X1F	67 0x43 0x1F Initialization Automatic/Manual Control */
	writeDirectReg(69,	INIT_DR69	);/*0X0C	69 0x45 0x0A 0x0C Initialization Loop Closure Debounce Interval (1.25 ms/LSB) */
	writeDirectReg(70,	INIT_DR70	);/*0X0A	70 0x46 0x0A Initialization Ring Trip Debounce Interval (1.25 ms/LSB) */
	writeDirectReg(71,	INIT_DR71	);/*0X01	71 0x47 0x00 0x01 Initialization Off-Hook Loop Current Limit (20 mA + 3 mA/LSB) */
	writeDirectReg(72,	INIT_DR72	);/*0X20	72 0x48 0x20 Initialization On-Hook Voltage (open circuit voltage) = 48 V(1.5 V/LSB) */
	writeDirectReg(73,	INIT_DR73	);/*0X02	73 0x49 0x02 Initialization Common Mode VoltageVCM = 3 V(1.5 V/LSB) */
	writeDirectReg(74,	INIT_DR74	);/*0X32	74 0x4A 0x32 Initialization VBATH (ringing) = 75 V (1.5 V/LSB) */
	writeDirectReg(75,	INIT_DR75	);/*0X10	75 0x4B 0x10 Initialization VBATL (off-hook) = 24 V (TRACK = 0)(1.5 V/LSB) */
	if (chipData.type != 3)
		writeDirectReg(92,	INIT_DR92	);/*0x7f	92 0x5C 0xFF 7F Initialization DCDC Converter PWM Period (61.035 ns/LSB) */
	else
		writeDirectReg(92,	INIT_SI3210M_DR92	);/*0x7f	92 0x5C 0xFF 7F Initialization DCDC Converter PWM Period (61.035 ns/LSB) */

	writeDirectReg(93,	INIT_DR93	);/*0x14	93 0x5D 0x14 0x19 Initialization DCDC Converter Min. Off Time (61.035 ns/LSB) */
	writeDirectReg(96,	INIT_DR96	);/*0x00	96 0x60 0x1F Initialization Calibration Control Register 1(written second and starts calibration) */
	writeDirectReg(97,	INIT_DR97	);/*0X1F	97 0x61 0x1F Initialization Calibration Control Register 2(written before Register 96) */
	writeDirectReg(98,	INIT_DR98	);/*0X10	98 0x62 0x10 Informative Calibration result (see data sheet) */
	writeDirectReg(99,	INIT_DR99	);/*0X10	99 0x63 0x10 Informative Calibration result (see data sheet) */
	writeDirectReg(100,	INIT_DR100	);/*0X11	100 0x64 0x11 Informative Calibration result (see data sheet) */
	writeDirectReg(101,	INIT_DR101	);/*0X11	101 0x65 0x11 Informative Calibration result (see data sheet) */
	writeDirectReg(102,	INIT_DR102	);/*0x08	102 0x66 0x08 Informative Calibration result (see data sheet) */
	writeDirectReg(103,	INIT_DR103	);/*0x88	103 0x67 0x88 Informative Calibration result (see data sheet) */
	writeDirectReg(104,	INIT_DR104	);/*0x00	104 0x68 0x00 Informative Calibration result (see data sheet) */
	writeDirectReg(105,	INIT_DR105	);/*0x00	105 0x69 0x00 Informative Calibration result (see data sheet) */
	writeDirectReg(106,	INIT_DR106	);/*0x20	106 0x6A 0x20 Informative Calibration result (see data sheet) */
	writeDirectReg(107,	INIT_DR107	);/*0x08	107 0x6B 0x08 Informative Calibration result (see data sheet) */
	writeDirectReg(108,	INIT_DR108	);/*0xEB	108 0x63 0x00 0xEB Initialization Feature enhancement register */
}


void initializeIndirectRegisters(void)
{
	unsigned char i=0;

	while (indirectRegisters[i].initial || indirectRegisters[i].address )
	{
		writeIndirectReg(indirectRegisters[i].address, indirectRegisters[i].initial);
		i++;
	}
}

#if 0
void printIndirectRegisters(void)
{
	unsigned char i=0;
	while (indirectRegisters[i].initial || indirectRegisters[i].address )
	{
		printf ("\n");
		printf(" %s = 0x%4.2X  should be 0x%4.2X ",indirectRegisters[i].name,readDirectReg(i),indirectRegisters[i].initial );
		i++;
	}
}
#endif


void verifyIndirectRegisters(void)
{
	int passed = 1;
	unsigned short i,j, initial;
	for (i=0; i<42; i++) {
		j=readIndirectReg((unsigned char) i);
		initial= indirectRegisters[i].initial;
		if ( j != initial )
		{
//			diag_printf("\n %s  iREG %X = %X  should be %X ",indirectRegisters[i].name,i,j,initial );
			passed = 0;
		}
	}

#if 0
	if (passed)
	{
		printf("Initialization of Indirect Registers completed successfully.\n");

	} else
	{
//		diag_printf("\n");
		printf("Initialization of Indirect Registers UNSUCCESSFULLY.\n");
//			key();
//		exit(1);
	}
#endif
}

char * exceptionStrings[] =
{ " ProSLIC not communicating", "Time out durring Power Up", "Time out durring Power Down",
"Power is Leaking; might be a short"," Tip or Ring Ground Short",

"Too Many Q1 Power Alarms" ,"Too Many Q2 Power Alarms" ,"Too Many Q3 Power Alarms"

"Too Many Q4 Power Alarms" ,"Too Many Q5 Power Alarms" ,"Too Many Q6 Power Alarms" };


void exception (enum exceptions e)
/* This is where an embedded system would call its exception handler */
/* This code runs a print out routine */
{
	printk( "\n                 E X C E P T I O N: %s\n",exceptionStrings[e] );
}

void setState(int newState)
{
	chipData.previousState=chipData.state;
	(int)chipData.newState= newState;
	chipData.state=STATEcHANGE;
	switch (newState)
	{
		 case CALLERiD:
			 chipData.eventEnable=0;
			 break;
		 case RINGING:
			 chipData.eventEnable=1;
			 break;
	}
}

int powerUp(void)
{
	unsigned char vBat ;
	int i=0, powerTime=0;

	cyg_tick_count_t initial_time = cyg_current_time();

	if (chipData.type == 3)  /* M version correction */
	{
		writeDirectReg(92,0x60);/* M version */
		writeDirectReg(93,0x38);/* M version */
	}
	else
	{
		/* set the period of the DC-DC converter to 1/64 kHz  START OUT SLOW*/
		writeDirectReg(92, 0x7f);
		writeDirectReg(93, 0x12);
	}

	writeDirectReg(14, 0); 	 /* Engage the DC-DC converter */

	while ((vBat=readDirectReg(82)) < 0xc0)
	{

		if (i++ > 200000)
		{
			printk("vBat=%x", vBat);
			printk("Time for powerup!\n");
			if ((cyg_current_time() - initial_time ) > 500) 	/* 0.5 seconds */
			{
				exception(TIMEoUTpOWERuP);
				return FAILED;
			}
		}

	}
	powerTime= cyg_current_time() - initial_time;
	if ( powerTime > 50)/* 0.5 seconds */
	{
		printk("\nWarning Power Up took %d milliseconds.\t",powerTime*10);
		printk("more than 0.5 seconds\n");
	}

	writeDirectReg(93, 0x80);  /* DC-DC Calibration  */
	printk("Wait for DC-DC Calibration (DR93) to complete\n");

	/* Wait for DC-DC Calibration to complete */
	while(0x80&readDirectReg(93));  // Wait for DC-DC Calibration to complete
	printk("power up complete!\n");
	return SUCCESS;
}

int powerLeakTest(void)
{
	/* Thlin++ 2005-09-07
	   check EC ¹q®e (VBAT OUT 10uF/100V) ¬O§_¥¿±`.
	   ­YEC¤£¥¿±`,«hµLªkÀx¹q. ¦b¥¿±`±¡§Î¤U DC-DC converter Ãö±¼1 sec , VBAT>6V
  	*/
	unsigned long Leaktime;
	//extern unsigned long volatile jiffies;
	unsigned char vBat ;
	writeDirectReg(64,0);
	writeDirectReg(14, 0x10);   //DC-DC Converter Power-off control
	//setState(POWERlEAKtEST);  /* This may be used in a future revision of the code to break up the bring-up */
	//cyg_thread_delay(100);  // one second

	Leaktime = jiffies;
	while((jiffies - Leaktime)<40);/* 100:1 second, 40: 400ms. In our condition: >40, powerleak!!*/
					// check timer accuracy ! thlin++ 2005-09-07

	if( (vBat=readDirectReg(82)) < 0x4 )  // 6 volts
	{
	 	exception(POWERlEAK);
		return FAILED;
	}

	printk("Power not leaking!\n");
	return SUCCESS;
}

unsigned short manualCalibrate(void)
{
	unsigned char x,y,i,progress=0; // progress contains individual bits for the Tip and Ring Calibrations

	//Initialized DR 98 and 99 to get consistant results.
	// 98 and 99 are the results registers and the search should have same intial conditions.
	writeDirectReg(98,0x10); // This is necessary if the calibration occurs other than at reset time
	writeDirectReg(99,0x10);

	for ( i=0x1f; i>0; i--)
	{
		writeDirectReg(98,i);
		cyg_thread_delay(4);
		if((readDirectReg(88)) == 0)
		{
			progress|=1;
			x=i;
			break;
		}
		//else if( readDirectReg( 88 ) > 0 )		// Howard. 2004.10.4
			//continue ;
//		else
			//exception( Howard_test ) ;
	} // for

	for ( i=0x1f; i>0; i--)
	{
		writeDirectReg(99,i);
		cyg_thread_delay(4);
		if((readDirectReg(89)) == 0)
		{
			progress|=2;
			y=i;
			break;
		}
	}//for

	return progress;
}



void calibrate(void)
{
	//extern unsigned long volatile jiffies;
	unsigned long Caltime;
	unsigned char x,y,i=0,progress=0;
	/* Do Flush durring powerUp and calibrate */
	writeDirectReg(21,DISABLE_ALL_DR21);/*  	Disable all interupts in DR21 */
	writeDirectReg(22,DISABLE_ALL_DR22);/*	Disable all interupts in DR22 */
	writeDirectReg(23,DISABLE_ALL_DR23);/*	Disabel all interupts in DR23 */
	writeDirectReg(64,OPEN_DR64);

	/*(0x18)Calibrations without the ADC and DAC offset and without common mode calibration. */
	writeDirectReg(97,STANDARD_CAL_DR97);
	//writeDirectReg(97,0x1e);
	/* (0x47)Calibrate common mode and differential DAC mode DAC + ILIM */
	writeDirectReg(96,STANDARD_CAL_DR96);

	printk("wait DR 96 calibration (0x47)");
	while (readDirectReg(96) != 0 );
	printk(" ---> OK!\n");
	/*
		Initialized DR 98 and 99 to get consistant results.
 		98 and 99 are the results registers and the search
 		should have same intial conditions.
		The following is the manual gain mismatch calibration
		This is also available as a function
	 */
	Caltime= jiffies;
	while((jiffies-Caltime)<1);/* 0.01 seconds */
#if defined(_Si3210_IndirectReg_)
	writeIndirectReg(88,0);
	writeIndirectReg(89,0);
	writeIndirectReg(90,0);
	writeIndirectReg(91,0);
	writeIndirectReg(92,0);
	writeIndirectReg(93,0);
#else
	writeIndirectReg(75,0);
	writeIndirectReg(76,0);
	writeIndirectReg(77,0);
	writeIndirectReg(78,0);
	writeIndirectReg(79,0);
	writeIndirectReg(80,0);
#endif
	/* This is necessary if the calibration occurs other than at reset time */
	writeDirectReg(98,0x10);
	writeDirectReg(99,0x10);
	for ( i=0x1f; i>0; i--)
	{
		writeDirectReg(98,i);
		Caltime = jiffies;
		while((jiffies-Caltime)<4);/* 0.04 seconds */
		if((readDirectReg(88)) == 0)
		{
			progress|=1;
			x=i;
			break;
		}
	}
	for ( i=0x1f; i>0; i--)
	{
		writeDirectReg(99,i);
		Caltime = jiffies;
		while((jiffies-Caltime)<4);
		if((readDirectReg(89)) == 0)
		{
			progress|=2;
			y=i;
			break;
		}
	}

	/* The preceding is the manual gain mismatch calibration
  	   The following is the longitudinal Balance Cal
  	 */
	goActive();
	if((readDirectReg(68) & 0x3) & 4)
		return ;
	writeDirectReg(64,OPEN_DR64);
#if 1   //there is not this bit of reg23 in 3215.chiminer
	writeDirectReg(23,ENB2_DR23);  /* enable interrupt for the balance Cal */
#endif
	writeDirectReg(97,BIT_CALCM_DR97); /* this is a singular calibration bit for longitudinal calibration */
	writeDirectReg(96,0x40);
	printk("wait DR 96 calibration (0x40)");
#if 1	//speed self-calibration.chiminer
	while(readDirectReg(96) != 0 );
#else
	for (i=0;i<300;i++);
	if (readDirectReg(96) != 0 )
		printk("D-reg 96=%d\n",readDirectReg(96));
	//while(readDirectReg(96) != 0 );
#endif
	printk(" ---> OK!\n");
#if 0   //write some registers according to AN35. chiminer
	writeIndirectReg(75,0);
	writeIndirectReg(76,0);
	writeIndirectReg(77,0);
	writeIndirectReg(78,0);
	writeIndirectReg(79,0);
	writeIndirectReg(80,0);
	writeIndirectReg(81,0);
	writeIndirectReg(82,0);
	writeIndirectReg(84,0);
	writeIndirectReg(208,0);
	writeIndirectReg(209,0);
	writeIndirectReg(210,0);
	writeIndirectReg(211,0);
#endif

   	writeDirectReg(21,INIT_DR21);
    	writeDirectReg(22,INIT_DR22);
    	writeDirectReg(23,INIT_DR23);
	/*The preceding is the longitudinal Balance Cal*/

}

void clearInterrupts(void)
{
	/* Writing ones to the status registers clears out any pending interrupts */
	writeDirectReg(	20	,	INIT_DR20	);
	writeDirectReg(	19	,	INIT_DR19	);
	writeDirectReg(	18	,	INIT_DR18	);
}


void goActive(void)
{
	unsigned long timeEpoch;
	//extern unsigned long volatile jiffies;
	writeDirectReg(64,1);
	/* LOOP STATE REGISTER SET TO ACTIVE */
	/* Active works for on-hook and off-hook see spec. */
	/* The phone hook-switch sets the off-hook and on-hook substate*/
	timeEpoch = jiffies + 10;
	while(jiffies < timeEpoch);
}




//void relay_to_voip(void);
int proslic_initialize(int ch__)
{
	unsigned short temp[5];
	unsigned char i,j;
	int ret = SUCCESS;




	/*  Begin Sanity check  Optional */
	if( (readDirectReg(8) != 2) || (readDirectReg(64) != 0) || (readDirectReg(11) != 0x33) )	/*¢¨ Step 8 ¢©*/
	{
		exception(PROSLICiNSANE);

		return ;
	}
	/* End Sanity check */

	//setState(INITIALIZING); 	// This will have more effect in future release

	initializeIndirectRegisters();															/*¢¨ Step 9 ¢©*/


	/* settings */
	writeDirectReg(8, 0);/* audio path no loop back */
	writeDirectReg(108, 0xEB);/* enhancement enable register */
	if (chipData.type ==0) /* Si3210 only */
	{
		writeDirectReg(67,0x17);/* Automatic control setting, why bit[3] is set to 0? which means no automatic switching to low battery in off-hook state */
		writeDirectReg(66,0x1);/* VBAT tracks VRING */
	}
	if (chipData.version <=2 )/*REVISION B and before*/
		writeDirectReg(73,4);/* set common mode voltage to 6 volts */

	/*  Flush ProSLIC digital filters by setting Coefficients to clear */
	/*  Save OSC control, Atimer, Itimer of OSC1 OSC2 */
#if defined(_Si3210_IndirectReg_)   //There is no register35 to register 39 in Si3215. add chiminer
	temp[0] = readIndirectReg(35);	writeIndirectReg(35, 0x8000);
	temp[1] = readIndirectReg(36);	writeIndirectReg(36, 0x8000);
	temp[2] = readIndirectReg(37);	writeIndirectReg(37, 0x8000);
	temp[3] = readIndirectReg(38);	writeIndirectReg(38, 0x8000);
	temp[4] = readIndirectReg(39);	writeIndirectReg(39, 0x8000);
#else
	temp[0] = readIndirectReg(22);
	temp[1] = readIndirectReg(23);
	temp[2] = readIndirectReg(24);
	temp[3] = readIndirectReg(25);
	temp[4] = readIndirectReg(26);
#endif

	//show_PCLK_freq();


	/* Do Flush durring powerUp and calibrate */
	if (chipData.type == 0 || chipData.type==3) /*  Si3210 */
	{
		ret = powerUp();  /* Turn on the DC-DC converter and verify voltage. */
		if(ret!=SUCCESS)
		{
			printk("PowerUp Failed\n");
			return ret;
		}

		ret = powerLeakTest(); /* Check for power leaks */
		if(ret!=SUCCESS)
		{
			printk("Power leak\n");
			return ret;
		}
		ret = powerUp(); /* Turn on the DC-DC converter again */
		if(ret!=SUCCESS)
		{
			printk("PowerUp Failed\n");
			return ret;
		}
	}

	//show_PCLK_freq();

	initializeDirectRegisters(ch__);
	printk("Start calibrate ...\n");
	calibrate();														//¢¨ Step 14 ~ Step 20 ¢©
	printk("Calibrate OK !\n");

	//show_PCLK_freq();
#if 1   //There is no register35 to register 39 in Si3215. add chiminer
	writeIndirectReg(35, temp[0]);
	writeIndirectReg(36, temp[1]);
	writeIndirectReg(37, temp[2]);
	writeIndirectReg(38, temp[3]);
	writeIndirectReg(39, temp[4]);
#else
	writeIndirectReg(22, temp[0]);
	writeIndirectReg(23, temp[1]);
	writeIndirectReg(24, temp[2]);
	writeIndirectReg(25, temp[3]);
	writeIndirectReg(26, temp[4]);
#endif
	clearInterrupts();
	goActive();
	printk("ProSLIC device initializaed\n");
	return SUCCESS;

}

void si3215_alaw( void )
{
	writeDirectReg(1, 0x20);
}
void si3215_mulaw( void )
{
	writeDirectReg(1, 0x28);
}
void si3215_linear( void )
{
	writeDirectReg(1, 0x38);
}

int proslic_all_initialized=0;

static void proslic_init_main(int ch___)
{
	static unsigned char count=0;
	int freq;
	char* freqs[ ] = {"8192","4028","2048","1024","512","256","1536","768","32768"};
	/* printable list of frequencies the PCM clock of the proslic opperates at */

	#if chiminer_daisy_chain
		if (!count)
			//It can only be written once,if two slic will be initialized.
			writeDirectReg_nodaisy(0,0x80|readDirectReg_nodaisy(0));
		printk("Reg0_internal=%x\n",readDirectReg(0));
		count++;
	#endif


	chipData.version = 0xf & readDirectReg(0);
	chipData.type = (0x30 & readDirectReg(0)) >> 4;
	printk("SLIC  type : %d\t", chipData.type);
	printk("version : %c\n", 'A'-1 + chipData.version);

	/* print out the PCM clock frequecy and the revision */
	freq = readDirectReg(13)>>4;  /* Read the frequency */
	printf("\nPCM clock =  %s KHz   Rev %c\n",  freqs[freq], 'A'-1 + chipData.version);

	proslic_initialize(ch___);  /* initialize the ProSLIC */			/*¢¨ Step 8 ~ Step 21 ¢©*/
	printk("proslic_initialize OK...\n");

	//show_PCLK_freq();

#if 0
	writeDirectReg( 2 , 0x0 ) ;
	writeDirectReg( 3 , 0x0 ) ;
	writeDirectReg( 4 , 0x0 ) ;
	writeDirectReg( 5 , 0x0 ) ;
	writeDirectReg(PCM_MODE, 0x29) ;	 //Enable PCM, set u-law		//¢¨ Step 22 ¢© PCM...

	writeDirectReg( 8 , 0x00 ) ;			// disable loopback
	writeDirectReg( AUDIO_GAIN , 0x00 ) ;//TX RX gain 0x00 = 0 db, 0x0A= 3 db
	writeDirectReg( 10 , 0x28 ) ;
	writeDirectReg(HYBRID, 0x33);					//¢¨ Step 23 ¢© init voice path

	//writeDirectReg( 4 , 0x01) ;		// slic rx delay 2 clock
	//writeDirectReg( 2 , 0x01 ) ;		// slic tx delay 0 clock
#endif

#if 1
	// Ring cadence 1 sec ON , 2 sec FFF
	writeDirectReg( 48 , 0x40 ) ;	// ring on low byte
	writeDirectReg( 49 , 0x1f ) ;	// ring on high byte
	writeDirectReg( 50 , 0x80 ) ;	// ring off low byte
	writeDirectReg( 51 , 0x3e ) ;	// ring off high byte

	writeDirectReg( 34 , 0x18 ) ;	// enable ring active and inactive timer
	writeDirectReg( 71 , 0x01 ) ;	//Loop current limit, 4 = 21+3*4 mA
	writeDirectReg( 69 , 0x0C ) ;	//onhook-offhook debounce time = 159 ms
#endif
	// jason++ 2005/04/26
/*	writeDirectReg( 65 , 0x11 ) ;
	writeDirectReg( 66 , 0x1 ) ;
	writeDirectReg( 69 , 0x0c ) ;
*/
      // thlin 2005-08-15 ¥[¤F¨S¹aÁn

#if 0
	writeDirectReg( 63 , 0x54 ) ;
	writeDirectReg( 67 , 0x1f ) ;
	writeDirectReg( 69 , 0x0c ) ;
	writeDirectReg( 70 , 0x0a ) ;						/*¢¨ Step 25 ¢© optional */
	writeDirectReg( 65 , 0x11 ) ;
	writeDirectReg( 66 , 0x1 ) ;
	writeDirectReg( 71 , 0x1 ) ;
	writeDirectReg( 72 , 0x20 ) ;
	writeDirectReg( 73 , 0x2 ) ;						/*¢¨ Step 26 ¢© optional */
	writeIndirectReg( 35 , 0x8000 ) ;
	writeIndirectReg( 36 , 0x320 ) ;
	writeIndirectReg( 37 , 0x8c ) ;
	writeIndirectReg( 38 , 0x8c ) ;
	writeIndirectReg( 39 , 0x10 ) ;					/*¢¨ Step 27 ¢© write indirect reg */
#endif


	//test = readDirectReg( 1 ) ;
	//OutputMessage("\n PCM = %x\n" , test ) ;
	//writeDirectReg( 1 , 0x3d ) ;	// set PCM bus configuration
	//writeIndirectReg( 32 , 0xff ) ;

	/*	// Howard. 2004.10.1
	com_vol = readDirectReg( 73 ) & 0x3f ;
	diag_printf("\n common vol 1 = %d\n" , com_vol ) ;
	if( com_vol < 48 )
		writeDirectReg( 73 , 0x20 ) ;	// set common mode voltage to 48v
	com_vol = readDirectReg( 73 ) & 0x3f ;
	diag_printf("\n common vol 2 = %dv\n" , com_vol ) ;
	*/


	//writeDirectReg(PCM_MODE, 0x28) ;	 //Enable PCM, set u-law, Tri-state on negative edge of PCLK

	/* print out the PCM clock frequecy and the revision */
	freq = readDirectReg(13)>>4;  /* Read the frequency */
	printf("\nPCM clock =  %s KHz   Rev %c\n",  freqs[freq], 'A'-1 + chipData.version);

	//show_PCLK_freq();

//	writeDirectReg(64, 2);
//	setState( ONHOOK ) ;  // This state is assumed here but it will be tested later.


//	verifyIndirectRegisters() ;

	/******************* Ringing *****************************************/
//	writeDirectReg(64, 0x04); 	// REG 64 , 0x44 => Ringing
//	cyg_thread_delay(20) ;		// Ringing hold for several seconds
//	writeDirectReg(LINE_STATE, 0) ; 	// REG 64 , 0 => Open
/*
	writeDirectReg(64, 0x04); 	// Ringing
	cyg_thread_delay(20) ;
	writeDirectReg(LINE_STATE, 0) ; //  Open

	writeDirectReg(64, 0x04); 	// Ringing
	cyg_thread_delay(20) ;
	writeDirectReg(LINE_STATE, 0) ; //  Open

	writeDirectReg(64, 0x04); 	// Ringing
	cyg_thread_delay(20) ;
	writeDirectReg(LINE_STATE, 0) ; //  Open

	writeDirectReg(64, 0x04); 	// Ringing
	cyg_thread_delay(20) ;
	writeDirectReg(LINE_STATE, 0) ; //  Open

	writeDirectReg(64, 0x04); 	// REG 64 , 0x44 => Ringing
	cyg_thread_delay(20) ;
	writeDirectReg(LINE_STATE, 0) ; 	// REG 64 , 0 => Open
*/	/**********************************************************************/

	//goActive();  /* set register 64 to a value of 1 */


	//writeDirectReg( 1 , 0x1c) ;		// disable PCM

	//test = readDirectReg( 8 ) ;
	//OutputMessage("\n Loop %s\n" , test?"Enabled":"Disabled" ) ;
	/*
	for( test = 0 ; test < 109 ; test++ )
	{
		int	tmp = 0 ;
		tmp = readDirectReg( test ) ;
		if( ( test % 10 ) == 0 )
			OutputMessage("\n") ;
		OutputMessage(" 0x%x" , tmp ) ;
	}
	*/
/* jason++ 2005/04/18 */
#if 0
	// Howard. 2004.10.5
	init_tif_thread() ;
	cyg_thread_resume( tif_thread_handle ) ;    				// Start it
#endif
//WJF 930905 debug remarked of
//#if (0)
	//phoneSystemDemo();  /* pretty much never returns */
//#endif

	proslic_all_initialized=1 ;
	printk("proslic_init_main OK...\n");
	printk("\n");
}

void proslic_init_all(int ch_____,unsigned char slic_number)
{
	extern unsigned char slic_order;
	int i, ch____;

	slic_order = slic_number;
	ch____ = ch_____;
	set_channel(ch____);

	printk("start proslic_init_main ...\n");
	proslic_init_main(ch____);

}


void genTone(tone_struct tone)
{
	// Uber function to extract values for oscillators from tone_struct
	// place them in the registers, and enable the oscillators.
	unsigned char osc1_ontimer_enable=0, osc1_offtimer_enable=0, osc2_ontimer_enable=0, osc2_offtimer_enable=0;
	int enable_osc2=0;

	//loopBackOff();
	disableOscillators(); // Make sure the oscillators are not already on.

	if (tone.osc1.coeff == 0 || tone.osc1.x == 0) {
		// Error!
		printf("You passed me a invalid struct!\n");
		return;
	}
	// Setup osc 1
	writeIndirectReg( OSC1_COEF, tone.osc1.coeff);
	writeIndirectReg( OSC1X, tone.osc1.x);
	writeIndirectReg( OSC1Y, tone.osc1.y);
	//printf("OUt-> 0x%04x\n",tone.osc1.coeff);
	// Active Timer

	if (tone.osc1.on_hi_byte != 0)
	{
		writeDirectReg( OSC1_ON__LO, tone.osc1.on_low_byte);
		writeDirectReg( OSC1_ON_HI, tone.osc1.on_hi_byte);
		osc1_ontimer_enable = 0x10;
	}

	// Inactive Timer
	if (tone.osc1.off_hi_byte != 0)
	{
		writeDirectReg( OSC1_OFF_LO, tone.osc1.off_low_byte);
		writeDirectReg( OSC1_OFF_HI, tone.osc1.off_hi_byte);
		osc1_offtimer_enable = 0x08;
	}

	if (tone.osc2.coeff != 0)
	{
		// Setup OSC 2
		writeIndirectReg( OSC2_COEF, tone.osc2.coeff);
		writeIndirectReg( OSC2X, tone.osc2.x);
		writeIndirectReg( OSC2Y, tone.osc2.y);

		// Active Timer
		if (tone.osc1.on_hi_byte != 0) {
			writeDirectReg( OSC2_ON__LO, tone.osc2.on_low_byte);
			writeDirectReg( OSC2_ON_HI, tone.osc2.on_hi_byte);
			osc2_ontimer_enable = 0x10;
		}
		// Inactive Timer
		if (tone.osc1.off_hi_byte != 0) {
			writeDirectReg( OSC2_OFF_LO, tone.osc2.off_low_byte);
			writeDirectReg( OSC2_OFF_HI, tone.osc2.off_hi_byte);
			osc2_offtimer_enable = 0x08;
		}
		enable_osc2 = 1;
	}

	writeDirectReg( OSC1, (unsigned char)(0x06 | osc1_ontimer_enable | osc1_offtimer_enable));
	if (enable_osc2)
		writeDirectReg( OSC2, (unsigned char)(0x06 | osc2_ontimer_enable | osc2_offtimer_enable));
	return;
}

/**************************** Tone Definitions *********************************************/

/*

These structures are use to hold the values of the group of registers which control the two oscillators
on the ProSLIC.  These oscillators are implemented with an amplitude parameter called the frequency
parameter called the x parameter and an amplitude parameter called the y parameter.  Each frequency and
amplitude map to an x and y parameter.  This mapping is not linear.  Also included in this structure
is the value of cadence timer periods.  Each field in this structure contains the value of one ProSLIC
register.

typedef struct {
	unsigned short coeff;
	unsigned short x;
	unsigned short y;
	unsigned char on_hi_byte;
	unsigned char on_low_byte;
	unsigned char off_hi_byte;
	unsigned char off_low_byte;
} Oscillator;




typedef struct {
	Oscillator osc1;
	Oscillator osc2;
} tone_struct;

typedef struct {
unsigned short frequency;
unsigned short coeff;
unsigned short x;
unsigned short y;
} coeffData;

*/

/*


Microsoft(R) Windows 98
   (C)Copyright Microsoft Corp 1981-1999.

C:\>tone 400 .0975

INFO: Freq = 400.000000    for n=1 or n=2
 OSCn = 0x79c0, OSCnX = 0x0071, OSCnY = 0x0000

C:\>tone 420 .0975

INFO: Freq = 420.000000    for n=1 or n=2
 OSCn = 0x7920, OSCnX = 0x0077, OSCnY = 0x0000

C:\>tone 415 .0975

INFO: Freq = 415.000000    for n=1 or n=2
 OSCn = 0x7940, OSCnX = 0x0076, OSCnY = 0x0000

C:\>tone 415 .2

INFO: Freq = 415.000000    for n=1 or n=2
 OSCn = 0x7940, OSCnX = 0x00f2, OSCnY = 0x0000

C:\>tone 400 .2

INFO: Freq = 400.000000    for n=1 or n=2
 OSCn = 0x79c0, OSCnX = 0x00e9, OSCnY = 0x0000

C:\>
 */

int Japan=0; // Used as a flag for to give japan tones out.




/*
                 1209 Hz 1336 Hz 1477 Hz 1633 Hz

                          ABC     DEF
   697 Hz          1       2       3       A

                  GHI     JKL     MNO
   770 Hz          4       5       6       B

                  PRS     TUV     WXY
   852 Hz          7       8       9       C

                          oper
   941 Hz          *       0       #       D

0 dbm0 tones

INFO: Freq = 1633.000000    for n=1 or n=2
 OSCn = 0x2460, OSCnX = 0x10ab, OSCnY = 0x0000

INFO: Freq = 697.000000    for n=1 or n=2
 OSCn = 0x6d50, OSCnX = 0x0643, OSCnY = 0x0000

INFO: Freq = 770.000000    for n=1 or n=2
 OSCn = 0x6950, OSCnX = 0x06f6, OSCnY = 0x0000

INFO: Freq = 852.000000    for n=1 or n=2
 OSCn = 0x6460, OSCnX = 0x07c3, OSCnY = 0x0000

INFO: Freq = 941.000000    for n=1 or n=2
 OSCn = 0x5ea0, OSCnX = 0x08a5, OSCnY = 0x0000

INFO: Freq = 1209.000000    for n=1 or n=2
 OSCn = 0x4a80, OSCnX = 0x0b79, OSCnY = 0x0000

INFO: Freq = 1336.000000    for n=1 or n=2
 OSCn = 0x3fc0, OSCnX = 0x0cec, OSCnY = 0x0000

INFO: Freq = 1477.000000    for n=1 or n=2
 OSCn = 0x3320, OSCnX = 0x0ea0, OSCnY = 0x0000

INFO: Freq = 1633.000000    for n=1 or n=2
 OSCn = 0x2460, OSCnX = 0x10ab, OSCnY = 0x0000


*/

/* This is an array of frequencies and their oscillator register values
for a single tones.  Lower in the code is a getOscCoeff (short int freq)*/

unsigned short oscCoeff[] = /*  These are the Primary Tones for Signaling Tone construction */
/* -18 dbM */
{
	350    /* HERTZ */, 0x7b30, 0x0063,
	440    /* HERTZ */ ,0x7870, 0x007d,
	480    /* HERTZ */, 0x7700, 0x0089,
	620    /* HERTZ */, 0x7120, 0x00b2,
	200    /* HERTZ */, 0x7e70, 0x0038,
	300    /* HERTZ */, 0x7c70, 0x0055,
	400    /* HERTZ */, 0x79c0, 0x0071
};

void standardRinging(void)
{
	// Enables ringing mode on ProSlic for standard North American ring
	//	RING_ON__LO	48
	//	RING_ON_HI	49
	//	RING_OFF_LO	50
	//	RING_OFF_HI	51
	// Active Timer
	writeDirectReg( RING_ON__LO, 0x80); // low reg 48
	writeDirectReg( RING_ON_HI, 0x3E); // hi reg 49
	// Inactive Timer
	writeDirectReg( RING_OFF_LO, 0x00); // low reg 50
	writeDirectReg( RING_OFF_HI, 0x7D); // hi reg 51
	// Enable timers for ringing oscillator
	writeDirectReg( 34, 0x18);
}

unsigned long interruptBits(void)
{
//	unsigned int	* ptr ;

#if 0		// Howard. 2004.10.5
	// Determines which interrupt bit is set
	union {
		unsigned char reg_data[3];
		long interrupt_bits;
	} u ;
	u.interrupt_bits=0;

// ONLY CLEAR the ACTIVE INTERRUPT or YOU WILL CREATE CRITICAL SECTION ERROR of LEAVING
// THE TIME OPEN BETWEEN THE Nth BIT and the N+1thbit within the same BYTE.
// eg. if the inactive oscillators are firing at nearly the same time
// you would only see one.

	u.reg_data[0] = readDirectReg( 18);
	writeDirectReg( 18,u.reg_data[0]);
	u.reg_data[1] = readDirectReg( 19);
	writeDirectReg( 19,u.reg_data[1] );
	u.reg_data[2] = readDirectReg( 20);
	writeDirectReg( 20,u.reg_data[2]);
	/*
	if( u.reg_data[1] & 0x2 )		// Howard. 2004.10.5
	{
		ptr = ( unsigned int *)u.reg_data ;
		OutputMessage("\n Loop Closure Interrupt!\n" ) ;
		OutputMessage("\n Loop bits = %x!\n" , u.interrupt_bits ) ;
		OutputMessage("\n ptr bits = %x!\n" , *ptr ) ;
	}
	if( u.reg_data[2] & 0x1 )
	{
		ptr = ( unsigned int *)u.reg_data ;
		OutputMessage("\n DTMF tone Interrupt!\n" ) ;
		OutputMessage("\n DTMF bits = %x!\n" , u.interrupt_bits ) ;
		OutputMessage("\n ptr bits = %x!\n" , *ptr ) ;
	}
	*/
	return u.interrupt_bits ;
#else
	char			reg_data[3] ;
	unsigned int	interrupt_bits = 0 , tmp ;

	reg_data[0] = readDirectReg( 18);
	writeDirectReg( 18,reg_data[0]);
	reg_data[1] = readDirectReg( 19);
	writeDirectReg( 19,reg_data[1] );
	reg_data[2] = readDirectReg( 20);
	writeDirectReg( 20,reg_data[2]);
	//tmp = reg_data[0]
	interrupt_bits = reg_data[0] ;
	tmp = reg_data[1] << 8 ;
	interrupt_bits |= tmp ;
	tmp = reg_data[2] << 16 ;
	interrupt_bits |= tmp ;

	//memcpy( &interrupt_bits , reg_data , 3 ) ;
	/*
	if( reg_data[1] & 0x2 )		// Howard. 2004.10.5
	{
		OutputMessage("\n Loop Closure Interrupt!\n" ) ;
		//OutputMessage("\n Loop bits = %x!\n" , interrupt_bits ) ;
	}
	if( reg_data[2] & 0x1 )
	{
		OutputMessage("\n DTMF tone Interrupt!\n" ) ;
		//OutputMessage("\n DTMF bits = %x!\n" , interrupt_bits ) ;
	}
	*/
	if( reg_data[1] & 0x10 )	// Power Alarm Q3
	{
		tmp = readDirectReg( 86 ) ;
//		OutputMessage("\n Power Alarm Q3 = %d\n" , tmp ) ;
	}
	return interrupt_bits ;
#endif
}

/* jason++ 2005/04/18 */
//unsigned char interruptChipData(void)
void interruptChipData(void)
{
#if 1	// Howard. 2004.10.5
	chipData.interrupt = interruptBits() ; // Store which interrupt occured for which chip.
#else
	int	data ;

	data = Interrupt() ;
	if( data != 0 )
		chipData.interrupt = interruptBits() ; // Store which interrupt occured for which chip.
	return data;
#endif
}

void callerid(void)
{
	writeDirectReg(INTRPT_MASK3, 0);
	writeDirectReg(INTRPT_STATUS3, 0xff);
	writeDirectReg(INTRPT_MASK2, 0);
	writeDirectReg(INTRPT_STATUS2, 0xff);
	writeDirectReg(INTRPT_MASK1, 0);
	writeDirectReg(INTRPT_STATUS1,0xff);

	cyg_thread_delay(25);  // one second

	//disableOscillators();
//??WJF		_asm { cli }
//?? WJF	sendProSLICID();
//??WJF		_asm {sti}

	disableOscillators();

	writeDirectReg(INTRPT_MASK3, 0xff);
	writeDirectReg(INTRPT_STATUS3, 0xff);
	writeDirectReg(INTRPT_MASK2, 0xff);
	writeDirectReg(INTRPT_STATUS2, 0xff);
	writeDirectReg(INTRPT_MASK1, 0xff);
	writeDirectReg(INTRPT_STATUS1,0xff);
}

char defered_isr(void)
{
	// Interrupt service  routine
	char	digit = 0 , i = 0 ;
	unsigned long shiftMask = 1 ;//, original_vec;
#define	SLIC_DEBUG	1
/* jason++ 2005/04/16 */
#if 0 //SLIC_DEBUG
	enum              // Declare enum type Days
	{
		OSC1_T1=0,
		OSC1_T2=1,
        OSC2_T1=2,
        OSC2_T2=3,
		RING_T1=4,
		RING_T2=5,
		PULSE_T1=6,
        PULSE_T2=7,
		RING_TRIP=8,
		LOOP__STAT=9,
        PQ1=10,
        PQ2=11,
		PQ3=12,
		PQ4=13,
		PQ5=14,
        PQ6=15,
		DTMF=16, // DTMF detected
		INDIRECT=17, // Indirect Reg Access ready
		CAL_CM_BAL=18, // Common Mode Calibration Error
	} interruptCause;                // Variable today has type Days

	static char * icause[]=
	{
		"Osc1 Inactive",
		"Osc1 Active",
        "Osc2 Inactive",
        "Osc2 Active",
		"Ring Inactive",
		"Ring Active" ,
		"Pulse Metering Inactive",
        "Pulse Metering Active",
		"Ring Trip",
		"Loop Status Change",
        "                           Pwr Alarm Q1",
        "                           Pwr Alarm Q2",
		"                           Pwr Alarm Q3",
		"                           Pwr Alarm Q4",
		"                           Pwr Alarm Q5",
        "                           Pwr Alarm Q6",
		"DTMF Decode", // DTMF detected
		"Indirect Access Complete", // Indirect Reg Access ready
		"Common mode balance fault",
	};
#endif
	//chipData.eventNumber++;
	//original_vec=chipData.interrupt;

	//for( interruptCause = OSC1_T1 ; interruptCause <= CAL_CM_BAL ; ( ( int )interruptCause ) ++ )
	for( i = 0 ; i <= 18 ; i++ )
	{
		if( shiftMask & chipData.interrupt )
		{
			chipData.interrupt &= ~shiftMask ;   // clear interrupt cause
		#if SLIC_DEBUG
//			OutputMessage((( interruptCause >=10) && (interruptCause<=11))?"\n %s":"\n(%s)  ", icause[interruptCause]) ;
		#endif
			//switch( interruptCause )
			switch( i )
			{
				case 16 ://DTMF:
					digit = readDirectReg( 24 ) & 0x0f ;
					if( ( digit >= DTMF_DIGIT1 ) && ( digit <= DTMF_DIGITHASH ) )
					{
						//OutputMessage("\n DTMF Digit = %d\n" , digit ) ;
						return digit ;
					}
					else
						return ( -1 ) ;
					break ;

            	case 8 ://RING_TRIP:
				case 9 ://LOOP__STAT:
					//OutputMessage("\n Loop Transition!\n" ) ;
					//genTone( DialTone ) ;
					if( readDirectReg( 68 ) & 0x01 )
						return FXS_OFFHOOK ;
					else
						return FXS_ONHOOK ;
					//return 0 ;
					break ;
		/*
				case PQ3:
					writeDirectReg( 64 , 0x01 ) ;
					break ;

				case OSC1_T1:
					break;
            	case OSC1_T2:
            		break;
				case OSC2_T1:
					break;
				case OSC2_T2:
            		break;
				case RING_T1:
					chipData.ringCount++;
    	        	if (chipData.state==FIRSTrING)
    	        	{
						chipData.ringCount=1;

						if (chipData.version >2)
							setState(CALLERiD);
    	        		else
							setState(RINGING);
			    	}

	        		break;
				case RING_T2:
   			    	if (chipData.state==PROGRAMMEDrING)
	        		{
						nextCadence();
					}
		    		break;
				case PULSE_T1:
					break;
            	case PULSE_T2:
            		break;

            	case RING_TRIP:
					#if 0	Howard. 2004.10.5
					if (chipData.version <=2 )  // if REVISION B  set to active.
					{
		            	goActive(); // Rev B fix not needed
					}
					#endif
					break ;	// Howard. 2004.10.7
					// no break ? 930925 WJF
				case LOOP__STAT:
            		// mark off by Howard. 2004.10.5
            		//if ((readDirectReg(80) < 2) || readDirectReg(81) < 2)
						//exception(TIPoRrINGgROUNDsHORT); // Check for grounded Tip or Ring Leads
					OutputMessage("\n Loop Transition!\n" ) ;
					return FXS_HOOK ;
					//return ( ( readDirectReg( 68 ) & 0x01 ) ? FXS_OFFHOOK : FXS_ONHOOK ) ;
					setState( LOOPtRANSITION ) ;
					break ;

				case PQ1:
				case PQ2:
				case PQ3:
				case PQ4:
				case PQ5:
				case PQ6:
					{
						static unsigned long lastEventNumber =1;
						if (lastEventNumber != chipData.eventNumber)  //  We allow only one alarm per alarm event
						{
							int i = interruptCause - PQ1;
							lastEventNumber = chipData.eventNumber;
							powerLeakTest();
							powerUp();
							OutputMessage( "  %d time",chipData.qLog[i]);
							if (chipData.qLog[i]++>2)
								exception((enum exceptions)(POWERaLARMQ1+i));
							if(chipData.qLog[i] >1)
								OutputMessage( "s");
							writeDirectReg(19,0xFC); //Clear Alarm bits
							goActive();
							setState(ONHOOK);
						}
					}
					break;

				case DTMF:
					digit = readDirectReg( 24 ) & 0x0f ;
					if( ( digit >= DTMF_DIGIT1 ) && ( digit <= DTMF_DIGITHASH ) )
						return digit ;
					else
						return 0 ;
					setState( DTMFtRANISTION ) ;
					break ;

				case INDIRECT:
					break ;
		*/
				default :
					break ;
			} //switch
		} //if
		shiftMask <<= 1 ;
	} //for
	return 0;		/* jason++ 2005/04/18 */
} // function interruptCause

coeffData getOscCoeff (short int freq)
{
	// Grabs osc coefficents from osc array, select by using .coeff
	int i;
	coeffData tempVar;

	i = 0;
	while (oscCoeff[i])
	{
		if (freq == oscCoeff[i])
		{
			tempVar.frequency = oscCoeff[i];
			tempVar.coeff = oscCoeff[i+1];
			tempVar.x = oscCoeff[i+2];
			tempVar.y = 0x0000;
			return tempVar;
		}
		else
		{
			i++;
		}
	}
	// Guess the freq they want isn't in the table so return nothing
	tempVar.frequency = 0x0;
	tempVar.coeff = 0x0;
	tempVar.x = 0x0;
	tempVar.y = 0x0;
	return tempVar;
}

void converterOn(void)
{
	writeDirectReg(14,	0);
}

void disableOscillators()
{
	// Turns of OSC1 and OSC2
	unsigned char i;
	//printf("Disabling Oscillators!!!\n");
	for ( i=32; i<=45; i++)
		if (i !=34)  // Don't write to the ringing oscillator control
			writeDirectReg(i,0);
}

void activateRinging(void)
{
//	diag_printf("\nactivateRing");
	writeDirectReg( LINE_STATE, RING_LINE); // REG 64,4
}

void cadenceRingPhone(ringStruct ringRegs)
{
	// Enables ringing mode on ProSlic for standard North American ring
	//	RING_ON__LO	48
	//	RING_ON_HI	49
	//	RING_OFF_HI	51
	//	RING_OFF_LO	50

//	diag_printf ("\n on= %d ms off =%d ms", ringRegs.onTime/8, ringRegs.offTime/8);
	writeDirectReg( RING_ON__LO,  ringRegs.on.onLowByte); // lo reg 48
	writeDirectReg( RING_ON_HI,   ringRegs.on.onHiByte); // hi reg 49
	// Inactive Timer
	writeDirectReg( RING_OFF_LO, ringRegs.off.offLowByte); // low reg 50
	writeDirectReg( RING_OFF_HI, ringRegs.off.offHiByte); // hi reg 51
	// Enable timers for ringing oscillator
	writeDirectReg( 34, 0x18);
}

#if 0
void phoneSystemDemo(void)
{
	int demo=1;

	chipData.interrupt = 0;
	chipData.digit_count = 0;
	chipData.eventEnable =1;

	/* Basic Touch Tone Phone system demo */
	while(demo )
	{
		if (chipData.eventEnable)
		{
			interruptChipData();
			if (chipData.interrupt)
				defered_isr();   // isr = interrrupt service routine
		}
		stateMachine();
	}
}

void stateMachine(void)
{
//	diag_printf("\nstateMachine");
	//if( ( chipData.previousState != 1 ) || ( chipData.state != 25 ) )	// not On-hook state
		//OutputMessage("\n previous state = %d , now state = %d\n" , chipData.previousState , chipData.state ) ;	// Howard. 2004.10.5
	switch( chipData.state )
	{
	/*  Below are only the meta/transient states (transitions).
	The actual precursor states are changed durring the event/interrupt handler.
	I know this is unconventional but it does shrink down the code considerably.
	Latter when I fold in the wait states and the delays I will put the stable states back here.
		*/
	case STATEcHANGE:
		 chipData.state=chipData.newState ;
		  break ;

	case CALLBACK :
		disableOscillators() ;
		standardRinging() ;
		activateRinging() ;
		chipData.state = FIRSTrING ;
		break ;

	case CALLBACKpROGRAMMED:
		disableOscillators() ;
	 	activateRinging() ;
		chipData.state = PROGRAMMEDrING ;
		break ;

	case MAKEoFFHOOK: // Make dialtone
		if( Japan == 1 )
			genTone( JapanDialTone ) ;
		else
			genTone( DialTone ) ;
		printMenu() ;
		setState( DIALtONE ) ;
		chipData.digit_count = 0 ;
		break ;

	case LOOPtRANSITION:
			//if (chipData.version <= 2)
				//writeDirectReg(69,10);   // Loop Debounce Register  = initial value
		switch( chipData.previousState )
		{
			case RINGING:
			case FIRSTrING:
				setState( OFFHOOK ) ;
		 		break ;

			case DEFEREDcALLBACKpROGRAMMED:
				setState( CALLBACKpROGRAMMED ) ;
				break ;

			case PRENEON:
				while( readDirectReg(82) < (60/1.5 - 3) ) ;
				writeDirectReg(72,0x3F) ;//  High Neon
				setState(NEON);
				break ;

			default:
				loopAction() ;
		}
		break ;

	case DTMFtRANISTION:
		switch( chipData.previousState )
		{
			case OFFHOOK:
			case BUSY:
			case RINGbACK:
			break ;

			case DIALtONE:
				disableOscillators() ;
				dtmfAction() ;
				break ;

			case DIGITDECODING:
				dtmfAction() ;
				break ;
		}
		break ;

	case CALLERiD:
		callerid() ;
		setState( RINGING ) ;
		break ;

	case RINGING:
		if( chipData.ringCount > 6 )
		{
			stopRinging() ;
			chipData.ringCount = 0 ;
			setState(ONHOOK) ;
		}
		break ;

	case MAKEbUSY:
		genTone( BusySignal) ;
		setState(BUSY) ;
		chipData.digit_count = 0 ;
		break ;

	case MAKErINGbACK:
		genTone( RingbackNormal ) ;
		setState( RINGbACK ) ;
		chipData.digit_count = 0 ;
		break ;

	case MAKEbUSYjAPAN:
		genTone( BusyJapan) ;
		setState(RINGbACKjAPAN) ;
		chipData.digit_count = 0 ;
		break ;

    case MAKErINGbACKjAPAN:
		genTone( RingbackJapan) ;
		setState(RINGbACKjAPAN) ;
		chipData.digit_count = 0 ;
		break ;

	case MAKErEORDER:
		genTone( ReorderTone) ;
		setState(REORDER) ;
		chipData.digit_count = 0 ;
		break ;

	case MAKEcONGESTION:
		genTone( CongestionTone) ;
		setState(CONGESTION) ;
		chipData.digit_count = 0 ;
		break ;
	}
}

void loopAction(void)
{
	// Howard. 2004.10.5
	//printf( readDirectReg(68)&4?"On hook":"Off hook" ) ;
//	OutputMessage( readDirectReg(68)&1?"loop action - Off hook":"loop action - On hook" ) ;

	if( ( chipData.state != MAKEoFFHOOK ) && ( readDirectReg(68) & 0x3 ) )
	{
		/*	Howard. 2004.10.5
		if (chipData.version <=2 )  // if REVISION B  set to active.
		{
			goActive();  // Rev B fix not needed
		}
		*/
		chipData.Off_Hook_time = clock() ;
		setState( MAKEoFFHOOK ) ;
		return ;
	}

	if( ( chipData.state != ONHOOK ) && ( readDirectReg(68) & 0x3 ) == 0 )
	{
		setState( ONHOOK ) ;
		chipData.On_Hook_time = clock() ;
		if( ( chipData.On_Hook_time - chipData.Off_Hook_time  ) < 2000 )
		{
			setState( CALLBACK ) ;
			return ;
		}
		setState( ONHOOK ) ;
		chipData.On_Hook_time = clock() ;
		disableOscillators() ;
		return ;
	}
}

void dtmfAction()
{
	void (*funct)();
	unsigned char digit;
	char asciiChar;
	setState(DIGITDECODING);
	digit = readDirectReg(24) & 0x0f;
//	diag_printf("\nDTMF Action");

	//asciiChar= '0' + digit;
	switch (digit)
	{
		case 0xA :
			asciiChar = '0';
			break;
		case 0xB:
			asciiChar = '*';
			break;
		case 0xC:
			asciiChar = '#';
			break;

		default:
			asciiChar= '0' + digit;
			break;
	}
	if (chipData.digit_count < 20)
	{
		chipData.DTMF_digits[chipData.digit_count] = asciiChar;
		chipData.digit_count++;
		chipData.DTMF_digits[chipData.digit_count]= 0;

//		diag_printf("Value= 0x%01x  String collected \"%s\" ", digit, &chipData.DTMF_digits );
		funct =		findNumber();
		if (funct) funct();
	}
}


void busy()
{
//	diag_printf("\n 3210 match");

	setState(MAKEbUSY);
}

void congestion()
{
//	diag_printf("\n 5555 match");
	setState(MAKEcONGESTION);
}
void ringBack()
{
//	diag_printf("\n 3211 match");
	setState(MAKErINGbACK);
}

void ringBackJapan()
{
//	diag_printf("\n 4444 match");
	setState(MAKErINGbACKjAPAN);
}

void busyJapan()
{
//	diag_printf("\n 4445 match");
	setState(MAKEbUSYjAPAN);
}

void emergency()
{
//	diag_printf("\n 911 match");
}

void reOrder()
{
//	diag_printf("\n no match");
	setState(MAKErEORDER);
}

void quickNeon(void)
{
//	diag_printf("\n Neon");
	setState( PRENEON);
}

void callbackProgrammed(int type)
{
//	diag_printf("\n Ringing Type %i",type);
	setState(DEFEREDcALLBACKpROGRAMMED);
	chipData.ringCadenceCordinates.nextCadenceEntryIndex=0;
	(int)chipData.ringCadenceCordinates.ringType = type;

}

void cb0() { callbackProgrammed (0); }
void cb1() { callbackProgrammed (1); }
void cb2() { callbackProgrammed (2); }
void cb3() { callbackProgrammed (3); }
void cb4() { callbackProgrammed (4); }
void cb5() { callbackProgrammed (5); }


typedef struct  {
	char * phoneNumber ;
	char * functionName;
	void (*action)();
}tNUMBER;

tNUMBER phoneNumbers[] = {

	{ "3210","Busy Tone", busy},
//	{ "***", "Quit Program", exit},
	{ "3211", "Ring Back Tone",ringBack},
	{ "333", "Neon",quickNeon},
	{ "4444", "Ring Back Japan",ringBackJapan},
	{ "4445", "Ring Back Japan",busyJapan},
	{ "50#", "Type 0 Ringing",cb0},
	{ "51#", "Type 1 Ringing",cb1},
	{ "52#", "Type 2 Ringing",cb2},
	{ "53#", "Type 3 Ringing",cb3},
	{ "54#", "Type 4 Ringing",cb4},
	{ "55#", "Type 5 Ringing",cb5},
	{ "555", "Congestion Tone",congestion},
	{"911", "Busy Tone",busy},
	{"", " Any Other Number=>Reorder Tone", reOrder},
};

void printMenu(void)
{
	int	i=0;
//	diag_printf("\n\n\t \t \t P H O N E  D I R E C T O R Y\n ");
	while (*phoneNumbers[i].phoneNumber)
	{
//		diag_printf("\n \t %s \t %s",phoneNumbers[i].phoneNumber,phoneNumbers[i].functionName);
		i++;
	}
//	diag_printf("\n\n\t \t \t P H O N E  D I R E C T O R Y\n ");
}

void  (*findNumber())()
{
	static  int currentNumber;
	int  tableR; // Row
	int  tableC; // Column

	/*
	To find the number a while loop is start at the last place the program left off
	in the phone number table and continues until either there is a match with fewer digits
	than a table entry, or there is a match with an entire entry, or there is no match  and the
	end of the table is reached.
	*/

	tableR=currentNumber;
	tableC=chipData.digit_count-1;

	while (phoneNumbers[tableR].phoneNumber[tableC] !=0)
	{
		if (phoneNumbers[tableR].phoneNumber[tableC] == chipData.DTMF_digits[tableC])
		{
			tableC++;

			if (tableC == chipData.digit_count)
			{
				if (phoneNumbers[tableR].phoneNumber[tableC]== 0)
				{
					currentNumber=0;
					tableC= 0;
					chipData.digit_count=0;

					return  phoneNumbers[tableR].action;

				}
				else
					return NULL ;
			}

		}
		else
		{
			tableR=++currentNumber;
			tableC= 0;
		}
	}

	currentNumber=0;
	chipData.digit_count=0;
	return &reOrder ;
}

void nextCadence(void)
{

	int x = chipData.ringCadenceCordinates.nextCadenceEntryIndex;
	int y =chipData.ringCadenceCordinates.ringType;
	/* Start current coordinate in the table */

	cadenceRingPhone(ringCadenceTables[y][x].ringRegs);

	/* change coordinate */
	chipData.ringCadenceCordinates.nextCadenceEntryIndex= ringCadenceTables[y][x].next;
}

void stopRinging(void)
{
	if (chipData.version <=2 )  // if REVISION B
	 	writeDirectReg(69,10);   // Loop Debounce Register  = initial value

	goActive();
}
/*
char get_data_from_fxs()
{
	interruptChipData() ;
	if( chipData.interrupt )
		return defered_isr() ;   // isr = interrrupt service routine
	return 0 ;
}
*/
#endif

/* jason++ 2005/04/18 */
#if 0
extern CKeypadDrv *g_keypaddrv ;
extern CUI *g_cui ;
extern int execute_hot_key_function( KEY_TYPE nKey) ;
extern void hot_key_check(KEY_TYPE nKey) ;
extern int hot_function_control;
extern void upgrade_mac_address(KEY_TYPE mKey);
#endif
void relay_to_pstn(void);
int offhook_cnt=0, onhook_cnt=0;
int switch_back_to_voip_when_onhook=0, switch_back_to_pstn_when_onhook=0;

/* jason++ 2005/04/18 */
#if 0
int voip_call_onhook_indication(void);

static char	config_mode = 0 ;
static char	previous_digit = 0 ;

char map_key_for_gateway_config( char digit )
{
	KEY_TYPE nKey ;
	//static FXS_CONFIG_STATE_T	input ;
	//if( config_mode == 0 )		// Not in config mode
	{
	if( !proslic_all_initialized )
		return digit;

//	send_data_in_ppp_format(1, (char *)&digit, 4);	//WJF 930107 debug
		switch( digit )
		{
			case DTMF_DIGIT0 :		// #0 -> Cancel
				if( previous_digit == DTMF_DIGITHASH )
				{
					if( config_mode )
					{
						//previous_digit = 0 ;
//940121						g_keypaddrv->TransferHandler( KEY_CANCEL );
						nKey = KEY_CANCEL ;
						//return 16 ;				// Cancel
						//return 0 ;
					}
					else
					{
//940121						g_keypaddrv->TransferHandler( KEY_DIGIT0 ) ;
						nKey = KEY_DIGIT0 ;
					}
					previous_digit = 0 ;
//	send_data_in_ppp_format(1, "DBG#1", 5);	//WJF 930107 debug
				}
				else
				{
//940121					g_keypaddrv->TransferHandler( KEY_DIGIT0 ) ;
					nKey = KEY_DIGIT0 ;
					previous_digit = digit ;
				}
				break ;

			case DTMF_DIGIT1 :
//	send_data_in_ppp_format(1, "debug1", 6);	//WJF 930107 debug
//940121				g_keypaddrv->TransferHandler( KEY_DIGIT1 ) ;
				nKey = KEY_DIGIT1 ;
				previous_digit = digit ;	//WJF 940328 added
				break ;

			case DTMF_DIGIT2 :
//940121				g_keypaddrv->TransferHandler( KEY_DIGIT2 ) ;
				nKey = KEY_DIGIT2 ;
				previous_digit = digit ;	//WJF 940328 added
				break ;

			case DTMF_DIGIT3 :
//940121				g_keypaddrv->TransferHandler( KEY_DIGIT3 ) ;
				nKey = KEY_DIGIT3 ;
				previous_digit = digit ;	//WJF 940328 added
				break ;

			case DTMF_DIGIT4 :
				if( previous_digit == DTMF_DIGITHASH )
				{
					if( config_mode )
					{
						//previous_digit = 0 ;
//940121						g_keypaddrv->TransferHandler( KEY_SOFTKEY2 );
						nKey = KEY_SOFTKEY2 ;
						//return 0 ;
					}
					else
					{
//940121						g_keypaddrv->TransferHandler( KEY_DIGIT4 ) ;
						nKey = KEY_DIGIT4 ;
					}
					previous_digit = 0 ;
//	send_data_in_ppp_format(1, "DBG#2", 5);	//WJF 930107 debug
				}
				else
				{
//940121					g_keypaddrv->TransferHandler( KEY_DIGIT4 ) ;
					nKey = KEY_DIGIT4 ;
					previous_digit = digit ;
				}
				break ;

			case DTMF_DIGIT5 :
//940121				g_keypaddrv->TransferHandler( KEY_DIGIT5 ) ;
				nKey = KEY_DIGIT5 ;
				previous_digit = digit ;	//WJF 940328 added
				break ;

			case DTMF_DIGIT6 :
//940121				g_keypaddrv->TransferHandler( KEY_DIGIT6 ) ;
				nKey = KEY_DIGIT6 ;
				previous_digit = digit ;	//WJF 940328 added
				break ;

			case DTMF_DIGIT7 :
				if( previous_digit == DTMF_DIGITHASH )
				{
					if( config_mode )
					{
						//previous_digit = 0 ;
//940121						g_keypaddrv->TransferHandler( KEY_LEFT );
						nKey = KEY_LEFT ;
						//return 14 ;				// '<-'
						//return 0 ;
					}
					else
					{
//940121						g_keypaddrv->TransferHandler( KEY_DIGIT7 ) ;
						nKey = KEY_DIGIT7 ;
					}
					previous_digit = 0 ;
//	send_data_in_ppp_format(1, "DBG#3", 5);	//WJF 930107 debug
				}
				else
				{
					previous_digit = digit ;
//940121					g_keypaddrv->TransferHandler( KEY_DIGIT7 ) ;
					nKey = KEY_DIGIT7 ;
				}
				break ;

			case DTMF_DIGIT8 :
				if( previous_digit == DTMF_DIGITHASH )
				{
					config_mode = 1 ;
					previous_digit = 0 ;
//	send_data_in_ppp_format(1, "DBG#4", 5);	//WJF 930107 debug
//940121					g_keypaddrv->TransferHandler( KEY_OK );
					nKey = KEY_OK ;
					//return 17 ;					// Menu/OK
					//return 0 ;
//	send_data_in_ppp_format(1, "debug1", 6);	//WJF 930107 debug
				}
				else
				{
					previous_digit = digit ;
//940121					g_keypaddrv->TransferHandler( KEY_DIGIT8 ) ;
					nKey = KEY_DIGIT8 ;
				}
				break ;

			case DTMF_DIGIT9 :
				if( previous_digit == DTMF_DIGITHASH )
				{
					if( config_mode )
					{
						//previous_digit = 0 ;
//940121						g_keypaddrv->TransferHandler( KEY_RIGHT );
						nKey = KEY_RIGHT ;
						//return 15 ;				// '->'
						//return 0 ;
					}
					else
					{
//940121						g_keypaddrv->TransferHandler( KEY_DIGIT9 ) ;
						nKey = KEY_DIGIT9 ;
					}
					previous_digit = 0 ;
//	send_data_in_ppp_format(1, "DBG#5", 5);	//WJF 930107 debug
				}
				else
				{
					previous_digit = digit ;
//940121					g_keypaddrv->TransferHandler( KEY_DIGIT9 ) ;
					nKey = KEY_DIGIT9 ;
				}
				break ;

			case DTMF_DIGITSTART :	// /*/
				if( config_mode == 0 )		// Not in config mode
				{
					if( previous_digit == DTMF_DIGITSTART )
					{
						previous_digit = 0 ;
//	send_data_in_ppp_format(1, "DBG#6", 5);	//WJF 930107 debug
						//return 0 ;
					}
					else if( previous_digit == 0 )	// first digit
					{
//		send_data_in_ppp_format(1, "relay#3", 8);	//WJF 930107 debug
						relay_to_pstn();
						switch_back_to_voip_when_onhook=1 ;
					}
//940121					else
//940121						g_keypaddrv->TransferHandler( KEY_DIGITSTAR ) ;
				}
				else
				{
					previous_digit = digit ;
//940121					g_keypaddrv->TransferHandler( KEY_DIGITSTAR ) ;
				}
				nKey = KEY_DIGITSTAR ;
				//return 0 ;
				break ;

			case DTMF_DIGITHASH :	// '#'
				//OutputMessage("\n config mode = %d\n" , config_mode ) ;
				if( config_mode == 0 )		// Not in config mode
				{
					if( previous_digit == DTMF_DIGITHASH )
					{
						// switch the relay to VoIP mode
						previous_digit = 0 ;
//	send_data_in_ppp_format(1, "DBG#7", 5);	//WJF 930107 debug
						//return 0 ;
					}
					else if( previous_digit == 0 )	// first digit
					{
						previous_digit = DTMF_DIGITHASH ;
						//g_keypaddrv->TransferHandler( KEY_SPEAKER ) ;
						g_cui->onhook_setting() ;
						//return 13 ;				// speaker
						//return 0 ;
					}
					else
						g_keypaddrv->TransferHandler( KEY_DIGITHASH ) ;
				}
				else
				{
					previous_digit = digit ;
					return digit;	//WJF 940121 added for hot function
				}
				//return 0 ;

				nKey = KEY_DIGITHASH ;
				break ;

			case FXS_ONHOOK :
//	send_data_in_ppp_format(1, "OnHook", 6);	//WJF 930107 debug
				cyg_thread_delay(10);		//wait a moment
				if( (readDirectReg( 68 ) & 0x01) == 0 )	//WJF 940127 debug
				{
//	send_data_in_ppp_format(1, "OnHook1", 7);	//WJF 930107 debug
					cyg_thread_delay(10);		//wait a moment
					if( (readDirectReg( 68 ) & 0x01) == 0 )	//WJF 940127 debug
					{

//	send_data_in_ppp_format(1, "OnHook2", 7);	//WJF 930107 debug

						g_cui->onhook_setting() ;
						config_mode = 0 ;
						nKey=KEY_CANCEL ;
						voip_call_onhook_indication();
						//g_keypaddrv->TransferHandler( KEY_SPEAKER ) ;
						break ;
					}
				}
			case FXS_OFFHOOK :
//	send_data_in_ppp_format(1, "OffHook", 6);	//WJF 930107 debug
				g_cui->offhook_setting() ;
				previous_digit = 0 ;
//	send_data_in_ppp_format(1, "DBG#8", 5);	//WJF 930107 debug
				break ;

			default :
				previous_digit = digit ;
				break ;
		}
	}

	if( (digit>=DTMF_DIGIT1) && (digit<=FXS_ONHOOK) )
	{
//	send_data_in_ppp_format(1, "debug2", 6);	//WJF 930107 debug
		if( hot_function_control == 0 )
		{
			hot_key_check(nKey) ;

			if( hot_function_control )
				config_mode = 1 ;
		}

//	send_data_in_ppp_format(1, "debug3", 6);	//WJF 930107 debug
		if( hot_function_control )
			execute_hot_key_function( nKey ) ;
		else
		{
//	send_data_in_ppp_format(1, "debug3", 6);	//WJF 930107 debug
//	send_data_in_ppp_format(1, (char *)&nKey, 4);	//WJF 930107 debug
			g_keypaddrv->TransferHandler( nKey ) ;
//	send_data_in_ppp_format(1, "after", 5);	//WJF 930107 debug
		}
	}

	return digit ;
}
#endif

//generate ring cadence to FXS port from SLIC
void Ring_On_FXS(void)
{
//	printf("\nRing On");
	writeDirectReg( 64 , 0x4 ) ;	// ringing
}

void Ring_Off_FXS(void)
{
//	printf("\nRing Off");
	writeDirectReg( 64 , 0x1 ) ;
}

#define PSTN_STATUS_ONHOOK	1
#define PSTN_STATUS_RINGING	2
#define PSTN_STATUS_OFFHOOK	3

#define PSTN_EVENT_ONHOOK	1
#define PSTN_EVENT_RINGING	2
#define PSTN_EVENT_OFFHOOK	3

extern unsigned char *	GW_LED_PORT ;
#define	GW_LED_PORT2 0xbfd00002
#define	GW_LED_PORT3 0xbfd00003
#define	GW_LED_PORT4 0xbfd00004

unsigned char gw_led_data=3;
int pstn_status=PSTN_STATUS_ONHOOK ;
int relay_status=1;		//0:pstn, 1:voip
void relay_to_pstn()
{
	relay_status=0 ;
/* jason++ 2005/04/18 */
#if 0
	GPIOB_DATA &= 0x7f ;	// relay off to pstn
#endif
//	execute_modem_command( "ATS0=0" ) ;	//disable auto-answer
//	execute_modem_command( "AT:U65,6000" ) ;	//place modem IC to power down mode
	//Hardware reset
/*
#ifdef __MODEM_INSIDE
	cyg_thread_delay(50);		//wait a moment
	execute_modem_command( "ATZ" ) ;
#endif
*/
//	*(unsigned char *)(GW_LED_PORT2) |= 0x01 ;		// set D0 to HIGH : turn off the led
	gw_led_data |= 1 ;
	*(unsigned char *)(GW_LED_PORT2) = gw_led_data ;		// set D0 to HIGH : turn off the led
// jason++ 2005/04/16 */
#if 0
	g_lcddrv->ShowMessage("Welcome", 0) ;
#endif
}

/* jason++ 2005/04/18 */
#if 0
void relay_to_voip()
{
/* jason++ 2005/04/18 */
#if 0
	GPIOB_DATA |= 0x80 ;	// relay on to VoIP
#endif
	relay_status=1 ;
//	*(unsigned char *)(GW_LED_PORT2) &= 0xfe ;		// set D0 to HIGH : turn off the led
	gw_led_data &= 0xfe ;
	*(unsigned char *)(GW_LED_PORT2) = gw_led_data ;		// set D0 to HIGH : turn off the led
/* jason++ 2005/04/18 */
#if 0
	g_lcddrv->ShowMessage("Welcome", 0) ;
#endif
}

//key 0x44
void pstn_relay_toggle(void)
{
//	send_data_in_ppp_format(1, (char *)&chipData.interrupt, 4);	//WJF 930107 debug
	if( relay_status )
		relay_to_pstn() ;
	else
		relay_to_voip() ;
}

extern void modem_dial_up_now(void);
void modem_dial_up_and_register(void) ;
externC void modem_hang_up_now(void);
externC void execute_modem_hang_up_now(void) ;

extern int modem_connected_mode, modem_dialing ;	//set to 1 when the dial to ISP connected( rx CONNECT XXXXXX )
int modem_toggle_on=0 ;
//key 0x45 modem dial-up, hang-up toggle
void modem_dialup_toggle()
{
//	if( modem_toggle_on )
	if( modem_connected_mode || modem_dialing )
	{
		modem_toggle_on = 0 ;
//		*(unsigned char *)(GW_LED_PORT2) |= 0x02 ;		// set D1 to HIGH : turn off the led
		gw_led_data |= 2 ;
		*(unsigned char *)(GW_LED_PORT2) = gw_led_data ;		// set D0 to HIGH : turn off the led
//		modem_hang_up_now();
//		send_data_in_ppp_format(1, "Hangup#101", 10);	//WJF 930107 debug
		execute_modem_hang_up_now() ;
	}
	else
	{
		relay_to_voip() ;
		modem_toggle_on = 1 ;
//		*(unsigned char *)(GW_LED_PORT2) &= 0xfd ;		// set D1 to HIGH : turn off the led
		gw_led_data &= 0xfd ;
		*(unsigned char *)(GW_LED_PORT2) = gw_led_data ;		// set D0 to HIGH : turn off the led
//		modem_dial_up_now();
		modem_reset();
		modem_dial_up_and_register() ;
	}
}

//this function will be called every second
void modem_led_flash()
{
	if( modem_connected_mode )
	{
		if( (gw_led_data&0x02) != 0 )	//modem led off
		{
			gw_led_data &= 0xfd ;
			*(unsigned char *)(GW_LED_PORT2) = gw_led_data ;		// set D0 to HIGH : turn off the led
		}
	}
	else if( modem_dialing )
	{
		if( (gw_led_data&0x02) != 0 )	//modem led off
			gw_led_data &= 0xfd ;
		else
			gw_led_data |= 2 ;

		*(unsigned char *)(GW_LED_PORT2) = gw_led_data ;		// set D0 to HIGH : turn off the led
	}
	else
	{
		if( (gw_led_data&0x02) == 0 )	//modem led on
		{
			gw_led_data |= 2 ;
			*(unsigned char *)(GW_LED_PORT2) = gw_led_data ;		// set D0 to HIGH : turn off the led
		}
	}
}

//called from keypadhandler() in keypaddrv.cpp
externC bool check_if_modem_on();
extern	unsigned int my_current_ip_address ;
extern void lcd_display_string2( char * in_string , int cursor_line ) ;
void gateway_keypad_check(KEY_TYPE nkey)
{
	unsigned char * ip_ptr=(unsigned char *)&my_current_ip_address ;
	char tmp_string[32] ;

	if( !proslic_all_initialized )
		return ;

//	send_data_in_ppp_format(1, (char *)&nkey, 4);	//WJF 930107 debug
	if( nkey == KEY_DEBUG1 )
		pstn_relay_toggle() ;
	else if( nkey == KEY_DEBUG2 )
	{
//		send_data_in_ppp_format(1, "toggle", 8);	//WJF 930107 debug
#ifdef __MODEM_INSIDE
		if( check_if_modem_on() )
			modem_dialup_toggle() ;
		else
		{
			sprintf(tmp_string , "IP=%d.%d.%d.%d" , ip_ptr[0], ip_ptr[1], ip_ptr[2], ip_ptr[3] ) ;
			lcd_display_string2(tmp_string , 2) ;
		}
#else
		sprintf(tmp_string , "IP=%d.%d.%d.%d" , ip_ptr[0], ip_ptr[1], ip_ptr[2], ip_ptr[3] ) ;
		lcd_display_string2(tmp_string , 2) ;
#endif
	}
}

SYSTEMSTATE get_system_state()
{
	return g_theApp.GetSysState() ;
}

void pstn_event_action(int event)
{
	static int ringing_cnt=0;

	if( !proslic_all_initialized )
		return ;

/* jason++ 2005/04/18 */
#if 0
	//WJF 940328 added, do not react to pstn event when modem dialing
	if( modem_connected_mode || modem_dialing )
		return ;
#endif

	switch(event)
	{
		case PSTN_EVENT_RINGING:
//		send_data_in_ppp_format(1, "pstn ringing", 8);	//WJF 930107 debug
			ringing_cnt += 3 ;
			if( ringing_cnt<6 )
				break ;

			offhook_cnt=0;
			onhook_cnt=0;
			if( relay_status == 1 )		//when on VoIP
			{
/* jason++ 2005/04/18 */
#if 0
				if( get_system_state() == SYS_STATE_IDLE )		//and VoIP idle
				{
//		send_data_in_ppp_format(1, "relay#1", 8);	//WJF 930107 debug
					relay_to_pstn();
					switch_back_to_voip_when_onhook=1 ;
					pstn_status = PSTN_STATUS_RINGING ;
				}
#endif
			}
			break ;
		case PSTN_EVENT_ONHOOK:
//		send_data_in_ppp_format(1, "pstn on-hook", 8);	//WJF 930107 debug
			if( ringing_cnt > 0 )
				--ringing_cnt ;

			offhook_cnt=0 ;
			if( (++onhook_cnt) < 3 )	//debounce
				break ;
			if( relay_status == 0 )		//when on PSTN
			{
				if( (pstn_status == PSTN_STATUS_OFFHOOK) || (onhook_cnt>40) )
				{
					if( switch_back_to_voip_when_onhook )
					{
//		send_data_in_ppp_format(1, "Relay#8", 8);	//WJF 930107 debug
						relay_to_voip();
						switch_back_to_voip_when_onhook=0;
					}
					pstn_status = PSTN_STATUS_ONHOOK ;
				}
			}
			else
				onhook_cnt = 0 ;

			break ;
		case PSTN_EVENT_OFFHOOK:
//		send_data_in_ppp_format(1, "pstn off-hook", 8);	//WJF 930107 debug

			onhook_cnt=0;
			if( (++offhook_cnt) < 3 )	//debounce
				break ;
			if( relay_status == 0 )		//when on PSTN
			{
				pstn_status = PSTN_STATUS_OFFHOOK ;
			}
			break ;
	}
}

void pstn_event_detection(void)
{
	unsigned char tmp = GPIOB_DATA ;
//	return ;
#if 1
	if( (tmp&0x10) == 0 )	//check PSTN Off-hook
		pstn_event_action(PSTN_EVENT_OFFHOOK) ;
	else
		pstn_event_action(PSTN_EVENT_ONHOOK) ;

	if( (tmp&0x20) == 0 )	//check PSTN Ringing
		pstn_event_action(PSTN_EVENT_RINGING) ;
#else
//	relay_to_pstn();
/*	if( GPIOB_DATA & 0x20 )	// check user off-hook(PSTN) or not
		printf("\nOn-hook") ;
	else
		printf("\nOff-hook") ;
*/
//	tmp &= 0x10 ;
	if( (tmp&0x10) == 0 )	//check PSTN Off-hook
		send_data_in_ppp_format(1, "off-hook", 8);	//WJF 930107 debug

	if( (tmp&0x20) == 0 )	//check PSTN Ringing
		send_data_in_ppp_format(1, "ring", 4);	//WJF 930107 debug
//		printf("\nRing") ;
#endif
}

//WJF 940126, return 0:reject the incoming call
int voip_incoming_call_indication(void)
{
	if( relay_status == 0 )		//check if relay to PSTN
	{
		if( pstn_status == PSTN_STATUS_ONHOOK )	//relay to VoIP when onhook
		{
			relay_to_voip() ;
			switch_back_to_pstn_when_onhook=1;
		}
		else
		{
			return 0 ;
//			g_theApp.SetSysState(SYS_STATE_CALL) ;
//			g_theApp.m_theUiEvent.UiSendTspRequest(pMsg, nAddrId, M_APP_SETUP_REJ, "User disconnects the active call!\n");
		}
	}
	return 1 ;
}

//int voip_call_onhook_indication(void)
void voip_call_onhook_indication(void)
{
	if(	switch_back_to_pstn_when_onhook )
	{
		switch_back_to_pstn_when_onhook=0;
//		send_data_in_ppp_format(1, "relay#2", 8);	//WJF 930107 debug
		relay_to_pstn() ;
	}
}

void tif_thread(void)
{
	char	c ;
	chipData.interrupt = 0;
	chipData.digit_count = 0;
	chipData.eventEnable =1;

	while( 1 )
	{
		pstn_event_detection() ;
		//WJF 940103 modified from 9 to 5, 9 will cause some problem
		cyg_thread_delay( 9 ) ;			// check Telephony Interface ( FXS ) for every 90 ms
		//if( chipData.eventEnable )
		{
			interruptChipData() ;
			if( chipData.interrupt )
			{
//				unsigned int tmp_v = chipData.interrupt ;
				c = defered_isr() ;   // isr = interrrupt service routine
//				send_data_in_ppp_format(1, (char *)&(tmp_v), 4);	//WJF 930107 debug
/* jason++ 2005/04/18 */
#if 0
				map_key_for_gateway_config( c ) ;
#endif
			}
		}
		//stateMachine() ;
	}
}

externC void init_tif_thread()
{
	if( tif_thread_handle )
		return ;

	cyg_flag_init( &tif_thread_flag ) ;

	cyg_thread_create(
		CYGPKG_NET_THREAD_PRIORITY ,							// Priority
		(cyg_thread_entry_t * )tif_thread ,						// entry
		0 ,														// entry parameter
		"Telephony Interface" ,									// Name
		&tif_thread_stack[0] ,									// Stack
		TIF_THREAD_STACK_SIZE ,									// Size
		&tif_thread_handle ,									// Handle
		&tif_thread_data										// Thread data structure
		) ;
}
#endif

/* call in each 90 ms by the timer routine */
char slic_routine(void)
{
	chipData.interrupt 		= 0;
	chipData.digit_count 	= 0;
//	chipData.eventEnable 	= 1;
//	if (chipData.eventEnable)
	{
		interruptChipData();
		if (chipData.interrupt)
			return defered_isr();
	}
	// stateMachine();
	return 0;							// no event
}
