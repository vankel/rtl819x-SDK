/*
 * rtl8197d-i2s.h  --  ALSA Soc Audio Layer
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 *
 *  Revision history
 *    24th Feb 2012   Initial version.
 */

#ifndef RTL8197DI2S_H_
#define RTL8197DI2S_H_

#define IIS_BASE (0xb801F000)  //8196d base address

#define IISCR		(IIS_BASE + 0x00)	//IIS Interface Control Register
#define TX_PAGE_PTR	(IIS_BASE + 0x04)	//TX Page Pointer Register
#define RX_PAGE_PTR	(IIS_BASE + 0x08)	//RX Page Pointer Register
#define IIS_SETTING	(IIS_BASE + 0x0C)	//IIS Page size and Sampling rate setting  Register
#define IIS_TX_IMR	(IIS_BASE + 0x10)	//IIS TX Interrupt Mask Register
#define IIS_TX_ISR	(IIS_BASE + 0x14)	//IIS TX Interrupt Status Register
#define IIS_RX_IMR	(IIS_BASE + 0x18)	//IIS RX Interrupt Mask Register
#define IIS_RX_ISR	(IIS_BASE + 0x1C)	//IIS RX Interrupt Status Register
#define IIS_TX_P0OWN	(IIS_BASE + 0x20)	//IIS TX Page 0 Own bit
#define IIS_TX_P1OWN	(IIS_BASE + 0x24)	//IIS TX Page 1 Own bit
#define IIS_TX_P2OWN	(IIS_BASE + 0x28)	//IIS TX Page 2 Own bit
#define IIS_TX_P3OWN	(IIS_BASE + 0x2C)	//IIS TX Page 3 Own bit
#define IIS_RX_P0OWN	(IIS_BASE + 0x30)	//IIS RX Page 0 Own bit
#define IIS_RX_P1OWN	(IIS_BASE + 0x34)	//IIS RX Page 1 Own bit
#define IIS_RX_P2OWN	(IIS_BASE + 0x38)	//IIS RX Page 2 Own bit
#define IIS_RX_P3OWN	(IIS_BASE + 0x3C)	//IIS RX Page 3 Own bit

//IISCR
#define DACLRSWAP	BIT(10)
#define IIS_FORMAT_I2S	(BIT(8)|BIT(9))
#define IIS_LOOP_BACK	BIT(7)
#define IIS_WL_16BIT	BIT(6)
#define IIS_EDGE_N	BIT(5)
#define IIS_MODE_MONO	BIT(4)
#define IIS_TXRXACT	BIT(2)
#define IIS_ENABLE	BIT(0)


//IIS_TX_ISR
#define IIS_TX_P0OK	BIT(0)
#define IIS_TX_P1OK	BIT(1)
#define IIS_TX_P2OK	BIT(2)
#define IIS_TX_P3OK	BIT(3)
#define IIS_TX_PAGEUNAVA	BIT(4)
#define IIS_TX_FIFO_EMPTY	BIT(5)


//IIS_RX_ISR
#define IIS_RX_P0OK	BIT(0)
#define IIS_RX_P1OK	BIT(1)
#define IIS_RX_P2OK	BIT(2)
#define IIS_RX_P3OK	BIT(3)
#define IIS_RX_PAGEUNAVA	BIT(4)
#define IIS_RX_FIFO_FULL	BIT(5)

/* Interrupt Mask Type */
#define P0OK_TX		0
#define P1OK_TX		1
#define P2OK_TX		2
#define P3OK_TX		3
#define TPUA		4
#define TFEM		5
#define P0OK_RX		6
#define P1OK_RX		7
#define P2OK_RX		8
#define P3OK_RX		9
#define RPUA		10
#define RFFU		11


/* clock sources */
#define RTL8197D_CLKSRC_PCLK 0
#define RTL8197D_CLKSRC_MPLL 1

/* Clock dividers */
#define RTL8197D_DIV_MCLK	0
#define RTL8197D_DIV_BCLK	1
#define RTL8197D_DIV_PRESCALER	2

/* prescaler */
#define RTL8197D_PRESCALE(a,b) \
	(((a - 1) << RTL8197D_IISPSR_INTSHIFT) | ((b - 1) << RTL8197D_IISPSR_EXTSHFIT))


extern struct snd_soc_dai rtl8197d_i2s_dai;

void tone_gens(int32_t sample_num, int16_t *buffPtr);

#endif /*RTL8197DI2S_H_*/
