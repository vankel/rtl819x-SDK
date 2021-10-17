/* SPI Flash driver
 *
 * Written by sam (sam@realtek.com)
 * 2010-05-01
 *
 */

#define MTD_SPI_DEBUG 2
#include <linux/autoconf2.h>
#include "spi_common.h"

#ifndef SPI_KERNEL
// ****** spi flash driver in bootcode

#include <asm/rtl8196x.h>
#include <rtl_types.h>
#if (MTD_SPI_DEBUG == 0)
//0
#define NDEBUG(args...) printf(args)
#define KDEBUG(args...) printf(args)
#define LDEBUG(args...) printf(args)
#endif
//1
#if (MTD_SPI_DEBUG == 1)
#define NDEBUG(args...) printf(args)
#define KDEBUG(args...) printf(args)
#define LDEBUG(args...)
#endif
//2
#if (MTD_SPI_DEBUG == 2)
#define NDEBUG(args...) printf(args)
#define KDEBUG(args...)
#define LDEBUG(args...)
#endif
//3
#if (MTD_SPI_DEBUG == 3)
#define NDEBUG(args...)
#define KDEBUG(args...)
#define LDEBUG(args...)
#endif

#else
// ****** spi flash driver in kernel
#include <linux/errno.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#define malloc	vmalloc
#define free	vfree
//0
#if (MTD_SPI_DEBUG == 0)
#define NDEBUG(args...) printk(args)
#define KDEBUG(args...) printk(args)
#define LDEBUG(args...) printk(args)
#endif
//1
#if (MTD_SPI_DEBUG == 1)
#define NDEBUG(args...) printk(args)
#define KDEBUG(args...) printk(args)
#define LDEBUG(args...)
#endif
//2
#if (MTD_SPI_DEBUG == 2)
#define NDEBUG(args...) printk(args)
#define KDEBUG(args...)
#define LDEBUG(args...)
#endif
//3
#if (MTD_SPI_DEBUG == 3)
#define NDEBUG(args...)
#define KDEBUG(args...)
#define LDEBUG(args...)
#endif

#endif


#define SIZEN_01M	0x14
#define SIZEN_02M	0x15
#define SIZEN_04M	0x16
#define SIZEN_08M	0x17
#define SIZEN_16M	0x18
#define SIZEN_32M	0x19
#define SIZEN_64M	0x20
#define SIZEN_CAL	0xff
#define SIZE_256B	0x100
#define SIZE_004K	0x1000
#define SIZE_064K	0x10000


/* SPI Flash Configuration Register(SFCR) (0xb800-1200) */
#define SFCR					0xb8001200			/*SPI Flash Configuration Register*/		
#define SFCR_SPI_CLK_DIV(val)	((val) << 29)
#define SFCR_RBO(val)			((val) << 28)
#define SFCR_WBO(val)			((val) << 27)
//#define SFCR_SPI_TCS(val)		((val) << 23)	//8196B		/*4 bit, 1111 */
#define SFCR_SPI_TCS(val)		((val) << 22)	//8196C Later		/*5 bit, 11111 */

/* SPI Flash Configuration Register(SFCR2) (0xb800-1204) */
#define SFCR2						0xb8001204
#define SFCR2_SFCMD(val)			((val) << 24)			/*8 bit, 1111_1111 */
#define SFCR2_SFSIZE(val)			((val) << 21)			/*3 bit, 111 */
#define SFCR2_RD_OPT(val)			((val) << 20)
#define SFCR2_CMD_IO(val)			((val) << 18)			/*2 bit, 11 */
#define SFCR2_ADDR_IO(val)			((val) << 16)			/*2 bit, 11 */
#define SFCR2_DUMMY_CYCLE(val)		((val) << 13)			/*3 bit, 111 */
#define SFCR2_DATA_IO(val)			((val) << 11)			/*2 bit, 11 */
#define SFCR2_HOLD_TILL_SFDR2(val)	((val) << 10)

/* SPI Flash Control and Status Register(SFCSR)(0xb800-1208) */
#define SFCSR					0xb8001208
#define SFCSR_SPI_CSB0(val)		((val) << 31)
#define SFCSR_SPI_CSB1(val)		((val) << 30)		
#define SFCSR_LEN(val)			((val) << 28)			/*2 bits*/
#define SFCSR_SPI_RDY(val)		((val) << 27)
#define SFCSR_IO_WIDTH(val)		((val) << 25)			/*2 bits*/
#define SFCSR_CHIP_SEL(val)		((val) << 24)
#define SFCSR_CMD_BYTE(val)		((val) << 16)			/*8 bit, 1111_1111 */

#define SFCSR_SPI_CSB(val)		((val) << 30)

/* SPI Flash Data Register(SFDR)(0xb800-120c) */
#define SFDR					0xb800120c

/* SPI Flash Data Register(SFDR2)(0xb8001210) */
#define SFDR2					0xb8001210


#define SPI_BLOCK_SIZE			0x10000				/* 64KB */
#define SPI_SECTOR_SIZE			0x1000				/* 4KB */
#define SPI_PAGE_SIZE			0x100				/* 256B */



#define SPICMD_WREN			(0x06 << 24)	/* 06 xx xx xx xx sets the (WEL) write enable latch bit */
#define SPICMD_WRDI			(0x04 << 24)	/* 04 xx xx xx xx resets the (WEL) write enable latch bit*/
#define SPICMD_RDID			(0x9f << 24)	/* 9f xx xx xx xx outputs JEDEC ID: 1 byte manufacturer ID & 2 byte device ID */
#define SPICMD_RDSR			(0x05 << 24)	/* 05 xx xx xx xx to read out the values of the status register */
#define SPICMD_WRSR			(0x01 << 24)	/* 01 xx xx xx xx to write new values to the status register */
#define SPICMD_READ			(0x03 << 24)	/* 03 a1 a2 a3 xx n bytes read out until CS# goes high */
#define SPICMD_FASTREAD		(0x0b << 24)	/* 0b a1 a2 a3 dd n bytes read out until CS# goes high */
#define SPICMD_2READ		(0xbb << 24)	/* bb 12 3d xx xx n bytes read out by 2 I/O until CS# goes high */
#define SPICMD_4READ		(0xeb << 24)	/* eb 3a 3d xx xx n bytes read out by 4 x I/O until CS# goes high */
#define SPICMD_SE			(0x20 << 24)	/* 20 a1 a2 a3 xx to erase the selected sector */
#define SPICMD_BE			(0xd8 << 24)	/* d8 a1 a2 a3 xx to erase the selected block */
#define SPICMD_CE			(0x60 << 24)	/* 60 xx xx xx xx to erase whole chip (cmd or 0xc7) */
#define SPICMD_PP			(0x02 << 24)	/* 02 a1 a2 a3 xx to program the selected page */
#define SPICMD_4PP			(0x38 << 24)	/* 38 3a 3d xx xx quad input to program the selected page */
#define SPICMD_CP			(0xad << 24)	/* ad a1 a2 a3 xx continously program whole chip, the address is automaticlly increase */
#define SPICMD_DP			(0xb9 << 24)	/* b9 xx xx xx xx enters deep power down mode */
#define SPICMD_RDP			(0xab << 24)	/* ab xx xx xx xx release from deep power down mode */
#define SPICMD_RES			(0xab << 24)	/* ab ?? ?? ?? xx to read out 1 byte device ID */
#define SPICMD_REMS_90		(0x90 << 24)	/* 90 ?? ?? ?? xx output the manufacter ID & device ID */
#define SPICMD_REMS_EF		(0xef << 24)	/* ef ?? ?? ?? xx output the manufacter ID & device ID */
#define SPICMD_REMS_DF		(0xdf << 24)	/* df ?? ?? ?? ?? output the manufacture ID & device ID */
#define SPICMD_ENSO			(0xb1 << 24)	/* b1 xx xx xx xx to enter the 512 bit secured OTP mode */
#define SPICMD_EXSO			(0xc1 << 24)	/* c1 xx xx xx xx to exit the 512 bit secured OTP mode */
#define SPICMD_RDSCUR		(0x2b << 24)	/* 2b xx xx xx xx to read value of secured register */
#define SPICMD_WRSCUR		(0x2f << 24)	/* 2f xx xx xx xx to set the lock down bit as "1" (once lock down, can not be updated) */
#define SPICMD_ESRY			(0x70 << 24)	/* 70 xx xx xx xx to enable SO to output RY/BY# during CP mode */
#define SPICMD_DSRY			(0x80 << 24)	/* 80 xx xx xx xx to disable SO to output RY/BY# during CP mode */

#define SPI_STATUS_REG_SRWD		0x07	/* status register write protect */
#define SPI_STATUS_CP			0x06	/* continously program mode */
#define SPI_STATUS_QE			0x06	/* quad enable */
#define SPI_STATUS_BP3			0x05	/* level of protected block */
#define SPI_STATUS_BP2			0x04	/* level of protected block */
#define SPI_STATUS_BP1			0x03	/* level of protected block */
#define SPI_STATUS_BP0			0x02	/* level of protected block */
#define SPI_STATUS_WEL			0x01	/* write enable latch */
#define SPI_STATUS_WIP			0x00	/* write in process bit */

/****** EON ******/


#define EON_STATUS_SRP		0x07	/* SRP Status Register P// write sector use malloc buffer */
#define EON_STATUS_WPDIS	0x06	/* (WP# disable) 1 = WP# disable 0 = WP# enable */
#define EON_STATUS_BP3		0x05	/* Block Protected bits */
#define EON_STATUS_BP2		0x04	/* Block Protected bits */
#define EON_STATUS_BP1		0x03	/* Block Protected bits */
#define EON_STATUS_BP0		0x02	/* Block Protected bits */
#define EON_STATUS_WEL		0x01	/* (Write Enable Latch) 1 = write enable 0 = not write enable */
#define EON_STATUS_WIP		0x00	/* WIP (Write In Progress bit) 1 = write operation 0 = not in write operation */

#define SPICMD_EON_EQIO		(0x38 << 24)	/* Enable Quad I/O (EQIO) (38h) */
#define SPICMD_EON_RSTQIO	(0xff << 24)	/* Reset Quad I/O (RSTQIO) (FFh) */
#define SPICMD_EON_WREN		(0x06 << 24)	/* Write Enable (WREN) (06h) */
#define SPICMD_EON_WRDI		(0x04 << 24)	/* Write Disable (WRDI) (04h) */
#define SPICMD_EON_RDSR		(0x05 << 24)	/* Read Status Register (RDSR) (05h) */
#define SPICMD_EON_WRSR		(0x01 << 24)	/* Write Status Register (WRSR) (01h) */
#define SPICMD_EON_READ			(0x03 << 24)	/* Read Data Bytes (READ) (03h) */
#define SPICMD_EON_FASTREAD		(0x0b << 24)	/* Read Data Bytes at Higher Speed (FAST_READ) (0Bh) */
#define SPICMD_EON_2READ		(0x3b << 24)	/* Dual Output Fast Read (3Bh) */
#define SPICMD_EON_2FASTREAD	(0xbb << 24)	/* Dual Input / Output FAST_READ (BBh) */
#define SPICMD_EON_4FASTREAD	(0xeb << 24)	/* Quad Input / Output FAST_READ (EBh) */
#define SPICMD_EON_PP			(0x02 << 24)	/* Page Program (PP) (02h) */
#define SPICMD_EON_SE			(0x20 << 24)	/* Sector Erase (SE) (20h) */
#define SPICMD_EON_BE			(0xd8 << 24)	/* Block Erase (BE) (D8h) */
#define SPICMD_EON_CE			(0x60 << 24)	/* Chip Erase (CE) (C7h/60h) */
#define SPICMD_EON_DP			(0xb9 << 24)	/* Deep Power-down (DP) (B9h) */
#define SPICMD_EON_RDI			(0xab << 24)	/* Release from Deep Power-down and Read Device ID (RDI) */
#define SPICMD_EON_DEVID		(0x90 << 24)	/* Read Manufacturer / Device ID (90h) */
#define SPICMD_EON_RDID			(0x9f << 24)	/* Read Identification (RDID) (9Fh) */
#define SPICMD_EON_OTP			(0x3a << 24)	/* Enter OTP Mode (3Ah) */
#define SPICMD_EON_EOTP			(0x20 << 24)	/* Erase OTP Command (20h) */


/****** SPANSION ******/
#define SPICMD_SPAN_READ		(0x03 << 24)	/* 3 0 0 (1 to ?? Read Data bytes */
#define SPICMD_SPAN_FASTREAD	(0x0b << 24)	/* 3 0 1 (1 to ?? Read Data bytes at Fast Speed */
#define SPICMD_SPAN_DOR			(0x3b << 24)	/* 3 0 1 (1 to ?? Dual Output Read */
#define SPICMD_SPAN_QOR			(0x6b << 24)	/* 3 0 1 (1 to ?? Quad Ou// write sector use malloc buffer */
#define SPICMD_SPAN_DIOR		(0xbb << 24)	/* 3 1 0 (1 to ?? Dual I/O High Performance Read */
#define SPICMD_SPAN_QIOR		(0xeb << 24)	/* 3 1 2 (1 to ?? Quad I/O High Performance Read */
#define SPICMD_SPAN_RDID		(0x9f << 24)	/* 0 0 0 (1 to 81) Read Identification */
#define SPICMD_SPAN_READ_ID		(0x90 << 24)	/* 3 0 0 (1 to ?? Read Manufacturer and Device Identification */
#define SPICMD_SPAN_WREN		(0x06 << 24)	/* 0 0 0 (0) Write Enable */
#define SPICMD_SPAN_WRDI		(0x04 << 24)	/* 0 0 0 (0) Write Disable */
#define SPICMD_SPAN_P4E			(0x20 << 24)	/* 3 0 0 (0) 4 KB Parameter Sector Erase */
#define SPICMD_SPAN_P8E			(0x40 << 24)	/* 3 0 0 (0) 8 KB (two 4KB) Parameter Sector Erase */
#define SPICMD_SPAN_SE			(0xd8 << 24)	/* 3 0 0 (0) 64KB Sector Erase */
#define SPICMD_SPAN_BE			(0x60 << 24)	/* 0 0 0 (0) Bulk Erase or 0xc7 */
#define SPICMD_SPAN_PP			(0x02 << 24)	/* 3 0 0 (1 to 256) Page Programming */
#define SPICMD_SPAN_QPP			(0x32 << 24)	/* 3 0 0 (1 to 256) Quad Page Programming */
#define SPICMD_SPAN_RDSR		(0x05 << 24)	/* 0 0 0 (1 to ?? Read Status Register */
#define SPICMD_SPAN_WRR			(0x01 << 24)	/* 0 0 0 (1 to 2) Write (Status & Configuration) Register */
#define SPICMD_SPAN_RCR			(0x35 << 24)	/* 0 0 0 (1 to ?? Read Configuration Register (CFG) */
#define SPICMD_SPAN_CLSR		(0x30 << 24)	/* 0 0 0 (1) Reset the Erase and Program Fail Flag (SR5 and SR6) and restore normal operation */
#define SPICMD_SPAN_DP			(0xb9 << 24)	/* 0 0 0 (0) Deep Power-Down */
#define SPICMD_SPAN_RES			(0xab << 24)	/* 0 0 0 (0) or ) Release from Deep Power-Down Mode */
										/* 0 0 3 (1 to ?? Release from Deep Power-Down and Read Electronic Signature */
#define SPICMD_SPAN_OTPP		(0x42 << 24)	/* 3 0 0 (1) Program one byte of data in OTP memory space */
#define SPICMD_SPAN_OTPR		(0x4b << 24)	/* 3 0 1 (1 to ?? Read data in the OTP memory space */
/* 5 TBPROT Configures start of block protection, 1 = Bottom Array (low address), 0 = Top Array (high address) (Default) */
#define SPAN_CONF_TBPROT	0x05
/* 3  BPNV  Configures BP2-0 bits in the Status Register, 1 = Volatile, 0 = Non-volatile (Default) */
#define SPAN_CONF_BPNV		0x03
/* 2 TBPARM Configures Parameter sector location, 1 = Top Array (high address), 0 = Bottom Array (low address) (Default) */
#define SPAN_CONF_TBPARM	0x02
/* 1  QUAD  Puts the device into Quad I/O mode, 1 = Quad I/O, 0 = Dual or Serial I/O (Default) */
#define SPAN_CONF_QUAD		0x01
/* 0 FREEZE Locks BP2-0 bits in the Status Register, 1 = Enabled, 0 = Disabled (Default) */
#define SPAN_CONF_FREEZE	0x00
/* 7 SRWD Status Register Write Disable, 1 = Protects when W#/ACC is low, 0 = No protection, even when W#/ACC is low */
#define SPAN_STATUS_SRWD	0x07
/* 6 P_ERR Programming Error Occurred, 0 = No Error, 1 = Error occurred */
#define SPAN_STATUS_PERR	0x06
/* 5 E_ERR Erase Error Occurred, 0 = No Error, 1 = Error occurred */
#define SPAN_STATUS_EERR	0x05
/* 4 BP2 Block Protect Protects selected Block from Program or Erase */
#define SPAN_STATUS_BP2		0x04
/* 3 BP1 Block Protect Protects selected Block from Program or Erase */
#define SPAN_STATUS_BP1		0x03
/* 2 BP0 Block Protect Protects selected Block from Program or Erase */
#define SPAN_STATUS_BP0		0x02
/* 1 WEL Write Enable Latch, 1 = Device accepts Write Registers, program or erase commands, 0 = Ignores Write Registers, program or erase commands */
#define SPAN_STATUS_WEL		0x01
/* 0 WIP Write in Progress, 1 = Device Busy a Write Registers, program or erase operation is in progress, 0 = Ready. Device is in standby mode and can accept commands. */
#define SPANSON_STATUS_WIP	0x00

/****** WINBOND ******/

#define WB_STATUS_BUSY		0x00	/* ERASE/WRITE IN PROGRESS */
#define WB_STATUS_WEL		0x01	/* WRITE ENABLE LATCH */
#define WB_STATUS_BP0		0x02	/* BLOCK PROTECT BITS 0 (non-volatile) */
#define WB_STATUS_BP1		0x03	/* BLOCK PROTECT BITS 1 (non-volatile) */
#define WB_STATUS_BP2		0x04	/* BLOCK PROTECT BITS 2 (non-volatile) */
#define WB_STATUS_TB		0x05	/* TOP/BOTTOM PROTECT (non-volatile) */
#define WB_STATUS_SEC		0x06	/* SECTOR PROTECT (non-volatile) */
#define WB_STATUS_SRP0		0x07	/* STATUS REGISTER PROTECT 0 (non-volatile) */
#define WB_STATUS_SRP1		0x08	/* STATUS REGISTER PROTECT 1 (non-volatile) */
#define WB_STATUS_QE		0x09	/* QUAD ENABLE (non-volatile) */
#define WB_STATUS_S10		0x0a	/* RESERVED */
#define WB_STATUS_S11		0x0b	/* RESERVED */
#define WB_STATUS_S12		0x0c	/* RESERVED */
#define WB_STATUS_S13		0x0d	/* RESERVED */
#define WB_STATUS_S14		0x0e	/* RESERVED */
#define WB_STATUS_SUS		0x0f	/* SUSPEND STATUS */

/*                                     INSTRUCTION NAME        	BYTE 1		BYTE 2 		BYTE 3 		BYTE 4		BYTE 5		BYTE 6*/
#define SPICMD_WB_WREN		(0x06 << 24)	/* Write Enable				06h */
#define SPICMD_WB_WRDI		(0x04 << 24)	/* Write Disable				04h */
#define SPICMD_WB_RDSR		(0x05 << 24)	/* Read Status Register-1		05h			(S7?“S0) */
#define SPICMD_WB_RDSR2		(0x35 << 24)	/* Read Status Register-2		35h			(S15-S8) */
#define SPICMD_WB_WRSR		(0x01 << 24)	/* Write Status Register		01h			(S7?“S0)		(S15-S8) */
#define SPICMD_WB_PP		(0x02 << 24)	/* Page Program				02h			A23?“A16		A15?“A8		A7?“A0		(D7?“D0) */
#define SPICMD_WB_QPP		(0x32 << 24)	/* Quad Page Program			32h			A23?“A16		A15?“A8		A7?“A0		(D7?“D0, ...) */
#define SPICMD_WB_SE		(0x20 << 24)	/* Sector Erase (4KB)			20h			A23?“A16		A15?“A8		A7?“A0 */
#define SPICMD_WB_BE		(0x52 << 24)	/* Block Erase (32KB)			52h			A23?“A16		A15?“A8		A7?“A0 */
#define SPICMD_WB_BE64		(0xd8 << 24)	/* Block Erase (64KB)			D8h			A23?“A16		A15?“A8		A7?“A0 */
#define SPICMD_WB_CE		(0x60 << 24)	/* Chip Erase					C7h/60h */
#define SPICMD_WB_ES		(0x75 << 24)	/* Erase Suspend				75h */
#define SPICMD_WB_ER		(0x7a << 24)	/* Erase Resume				7Ah */
#define SPICMD_WB_PD		(0xb9 << 24)	/* Power-down					B9h */
#define SPICMD_WB_CRMR		(0xff << 24)	/* Continuous Read Mode Reset	FFh			FFh */
#define SPICMD_WB_READ		(0x03 << 24)	/* Read Data					03h			A23-A16		A15-A8		A7-A0		(D7-D0) */
#define SPICMD_WB_FASTREAD	(0x0b << 24)	/* Fast Read					0Bh			A23-A16		A15-A8		A7-A0		dummy		(D7-D0) */
#define SPICMD_WB_2READ		(0x3b << 24)	/* Fast Read Dual Output	3Bh			A23-A16		A15-A8		A7-A0		dummy		(D7-D0, ...)*/
#define SPICMD_WB_2FASTREAD	(0xbb << 24)	/* Fast Read Dual I/O			BBh			A23-A8		A7-A0		M7-M0		(D7-D0, ...) */
#define SPICMD_WB_4READ		(0x6b << 24)	/* Fast Read Quad Output	6Bh			A23-A16		A15-A8		A7-A0		dummy		(D7-D0, ...) */
#define SPICMD_WB_4FASTREAD	(0xeb << 24)	/* Fast Read Quad I/O		EBh			A23-A0		M7-M0		(x,x,x,x, D7-D0, ...)	(D7-D0, ...) */
#define SPICMD_WB_4WREAD	(0xe7 << 24)	/* Word Read Quad I/O		E7h			A23-A0		M7-M0		(x,x, D7-D0, ...)		(D7-D0, ...) */
#define SPICMD_WB_WREAD		(0xe3 << 24)	/* Octal Word Read				E3h			A23-A0		M7-M0		(D7-D0, ...) */
#define SPICMD_WB_DI		(0xab << 24)	/* Device ID					ABh			dummy		dummy		dummy		(ID7-ID0) */
#define SPICMD_WB_MDI		(0x90 << 24)	/* Manufacturer/Device ID	90h			dummy		dummy		00h			(MF7-MF0)	(ID7-ID0) */
#define SPICMD_WB_2MDI		(0x92 << 24)	/* Manufacturer/Device ID	92h	A23-A8	A7-A0	M[7:0]		(MF[7:0], ID[7:0])		(by Dual I/O) */
#define SPICMD_WB_4MDI		(0x94 << 24) /* Manufacture/Device ID 94h A23-A0 M[7:0] xxxx (MF[7:0] ID[7:0]) (MF[7:0], ID[7:0], ...) (by Quad I/O) */
#define SPICMD_WB_RDID		(0x9f << 24)	/* JEDEC ID	9Fh	(MF7-MF0)	(ID15-ID8)	(ID7-ID0)	(Manufacturer Memory Type Capacity) */
#define SPICMD_WB_RUID		(0x4b << 24)	/* Read Unique ID	4Bh			dummy		dummy		dummy		dummy		(ID63-ID0)	*/

/****** SST ******/
#define SST_STATUS_BUSY		0x00	/* 0 BUSY 1 = Internal Write operation is in progress 0  R */
#define SST_STATUS_WEL		0x01	/* 1 WEL  1 = Device is memory Write enabled */
#define SST_STATUS_BP0		0x02	/* 2 BP0  Indicate current level of block write protection (See Table 4) 1 R/W */
#define SST_STATUS_BP1		0x03	/* 3 BP1  Indicate current level of block write protection (See Table 4) 1 R/W */
#define SST_STATUS_BP2		0x04	/* 4 BP2  Indicate current level of block write protection (See Table 4) 1 R/W */
#define SST_STATUS_BP3		0x05	/* 5 BP3  Indicate current level of block write protection (See Table 4) 0 R/W */
#define SST_STATUS_AAI		0x06	/* 6 AAI  Auto Address Increment Programming status 0  R 1 = AAI programming mode 0 = Byte-Program mode */
#define SST_STATUS_BPL		0x07	/* 7 BPL  1 = BP3, BP2, BP1, BP0 are read-only bits 0 R/W 0 = BP3, BP2, BP1, BP0 are readable/writable */

#define SPICMD_SST_READ		(0x03 << 24)		/* Read Read Memory 0000 0011b (03H) 3 0 25 MHz */
#define SPICMD_SST_FASTREAD	(0x0b << 24)		/* High-Speed Read Read Memory at higher speed 0000 1011b (0BH) 3 1 80 MHz */
#define SPICMD_SST_SE		(0x20 << 24)		/* KByte Sector-Erase3 Erase 4 KByte of 0010 0000b (20H) 3 0 0 80 MHz */
#define SPICMD_SST_BE32		(0x52 << 24)		/* 32 KByte Block-Erase4 Erase 32KByte block 0101 0010b (52H) 3 0 0 80 MHz */
#define SPICMD_SST_BE64		(0xd8 << 24)		/* 64 KByte Block-Erase5 Erase 64 KByte block 1101 1000b (D8H) 3 0 0 80 MHz */
#define SPICMD_SST_CE		(0x60 << 24)		/* Chip-Erase Erase Full Memory Array 0110 0000b (60H) or 1100 0111b (C7H) 0 0 0 80 MHz */
#define SPICMD_SST_BP		(0x02 << 24)		/* Byte-Program To Program One Data Byte 0000 0010b (02H) 3 0 1 80 MHz */
#define SPICMD_SST_AAI		(0xad << 24)		/* AAI-Word-Program Auto Address Increment 1010 1101b (ADH) 3 0 80 MHz */
#define SPICMD_SST_RDSR		(0x05 << 24)		/* RDSR Read-Status-Register 0000 0101b (05H) 0 0 80 MHz */
#define SPICMD_SST_EWSR		(0X50 << 24)		/* Enable-Write-Status-Register 0101 0000b (50H) 0 0 0 80 MHz */
#define SPICMD_SST_WRSR		(0x01 << 24)		/* Write-Status-Register 0000 0001b (01H) 0 0 1 80 MHz */
#define SPICMD_SST_WREN		(0x06 << 24)		/* Write-Enable 0000 0110b (06H) 0 0 0 80 MHz */
#define SPICMD_SST_WRDI		(0x04 << 24)		/* Write-Disable 0000 0100b (04H) 0 0 0 80 MHz */
#define SPICMD_SST_RDID		(0x90 << 24)		/* Read-ID 1001 0000b (90H) or 1010 1011b (ABH) 3 0 80 MHz */
#define SPICMD_SST_JEDECID	(0x9f << 24)		/* JEDEC-ID JEDEC ID read 1001 1111b (9FH) 0 0 80 MHz */
#define SPICMD_SST_EBSY		(0x70 << 24)		/* Enable SO as an output RY/BY# status during AAI programming 0111 0000b (70H) 0 0 0 80 MHz */
#define SPICMD_SST_DBSY		(0x80 << 24)		/* Disable SO as an output RY/BY# status during AAI programming 1000 0000b (80H) 0 0 0 80 MHz */

/****** GigaDevice ******/
#define GD_STATUS_WIP		0x00	/* 0 WIP 1 = Internal Write operation is in progress 0  R */
#define GD_STATUS_WEL		0x01	/* 1 WEL  1 = Device is memory Write enabled */
#define GD_STATUS_BP0		0x02	/* 2 BP0  Indicate current level of block write protection (See Table 4) 1 R/W */
#define GD_STATUS_BP1		0x03	/* 3 BP1  Indicate current level of block write protection (See Table 4) 1 R/W */
#define GD_STATUS_BP2		0x04	/* 4 BP2  Indicate current level of block write protection (See Table 4) 1 R/W */
#define GD_STATUS_BP3		0x05	/* 5 BP3  Indicate current level of block write protection (See Table 4) 0 R/W */
#define GD_STATUS_BP4		0x06	/* 6 BP4  Indicate current level of block write protection (See Table 4) 0 R/W */
#define GD_STATUS_SRP0		0x07	/* 7 The SRP bits control the m// write sector use malloc buffer */
#define GD_STATUS_SRP1		0x08	/* 8 The SRP bits control the method of write protection */
#define GD_STATUS_QE		0x09	/* 9 When the QE bit is set to 0 (Default) the WP# pin and HOLD# pin are enable */

#define SPICMD_GD_WREN		(0x06 << 24)	/* Write Enable           06H */
#define SPICMD_GD_WRDI		(0x04 << 24)	/* Write Disable          04H */
#define SPICMD_GD_RDSR		(0x05 << 24)	/* Read Status Register   05H     (S7-S0)                                               (continuous) */
#define SPICMD_GD_RDSR1		(0x35 << 24)	/* Read Status Register-1 35H     (S15-S8)                                              (continuous) */
#define SPICMD_GD_WRSR		(0x01 << 24)	/* Write Status Register  01H     (S7-S0)   (S15-S8) */
#define SPICMD_GD_READ		(0x03 << 24)	/* Read Data              03H     A23-A16   A15-A8    A7-A0       (D7-D0)   (Next byte) (continuous) */
#define SPICMD_GD_FASTREAD	(0x0b << 24)	/* Fast Read              0BH     A23-A16   A15-A8    A7-A0       dummy     (D7-D0)     (continuous) */
#define SPICMD_GD_READ2		(0x3b << 24)	/* Dual Output            3BH     A23-A16   A15-A8    A7-A0       dummy     (D7-D0)(1)  (continuous) */
#define SPICMD_GD_READ4		(0x6b << 24)	/* Quad Output Fast Read  6BH     A23-A16   A15-A8    A7-A0       dummy     (D7-D0)(3)  (continuous) */
#define SPICMD_GD_FASTREAD2	(0xbb << 24)	/* Dual I/O Fast Read     BBH     A23-A8(2) A7-A0                 dummy     (D7-D0)     (continuous) */
#define SPICMD_GD_FASTREAD4	(0xeb << 24)    /* Quad I/O Fast Read     EBH     A23-A0(4)                       dummy     (D7-D0)(3)  (continuous) */
#define SPICMD_GD_FASTREADW4 (0xe7 << 24)	/* Quad I/O Word          E7H     A23-A0                          dummy     (D7-D0)(3)  (continuous) */

#define SPICMD_GD_CRMR		(0xff << 24)	/* Continuous Read Reset  FFH */
#define SPICMD_GD_PP		(0x02 << 24)	/* Page Program           02H     A23-A16   A15-A8    A7-A0       D7-D0     Next byte */
#define SPICMD_GD_SE		(0x20 << 24)	/* Sector Erase           20H     A23-A16   A15-A8    A7-A0 */
#define SPICMD_GD_BE32		(0x52 << 24)	/* Block Erase(32K)       52H     A23-A16   A15-A8    A7-A0 */
#define SPICMD_GD_BE64		(0xd8 << 24)	/* Block Erase(64K)       D8H     A23-A16   A15-A8    A7-A0 */
#define SPICMD_GD_BE128		(0xd2 << 24)	/* Block Erase(128K)      D2H     A23-A16   A15-A8    A7-A0 */
#define SPICMD_GD_CE		(0x60 << 24)	/* Chip Erase             C7/60H */
#define SPICMD_GD_PES		(0x75 << 24)	/* Program/Erase Suspend  75H */
#define SPICMD_GD_PER		(0x7a << 24)	/* Program/Erase Resume   7AH */
#define SPICMD_GD_DP		(0xb9 << 24)	/* Deep Power-Down        B9H */
#define SPICMD_GD_RDP		(0xab << 24)	/* Release From Deep      ABH     dummy     dummy     dummy       (ID7-ID0)        (continuous) */
#define SPICMD_GD_REMS		(0x90 << 24)	/*  Manufacturer/Device ID */
#define SPICMD_GD_HPM		(0xa3 << 24)	/*  High Performance Mode A3H dummy   dummy      dummy */
#define SPICMD_GD_RDID		(0x9f << 24)	/* Read Identification   9FH (M7-M0) (ID15-ID8) (ID7-ID0)                   (continuous) */


/****** ATMEL ******/
//                                                                                                Clock         Address    Dummy  Data
//Command                                                                     Opcode           Frequency         Bytes      Bytes Bytes
#define SPICMD_AT_FASTREAD1	(0x1b << 24)	//Read Array							1Bh   0001 1011    Up to 100 MHz          3         2    1+
#define SPICMD_AT_FASTREAD	(0x0b << 24)	//Read Array							0Bh   0000 1011    Up to 85 MHz           3         1    1+
#define SPICMD_AT_READ		(0x03 << 24)	//Read Array							03h   0000 0011    Up to 50 MHz           3         0    1+
#define SPICMD_AT_READ2		(0x3b << 24)	//Dual-Output Read Array				3Bh   0011 1011    Up to 85 MHz           3         1    1+
#define SPICMD_AT_SE		(0x20 << 24)	//Block Erase (4 KBytes)				20h   0010 0000    Up to 100 MHz          3         0     0
#define SPICMD_AT_BE1		(0x52 << 24)	//Block Erase (32 KBytes)				52h   0101 0010    Up to 100 MHz          3         0     0
#define SPICMD_AT_BE		(0xd8 << 24)	//Block Erase (64 KBytes)				D8h   1101 1000    Up to 100 MHz          3         0     0
#define SPICMD_AT_CE		(0x60 << 24)	//Chip Erase							60h   0110 0000    Up to 100 MHz          0         0     0
//#define SPICMD_AT_CE		(0xc7 << 24)	//Chip Erase							C7h   1100 0111    Up to 100 MHz          0         0     0
#define SPICMD_AT_PP		(0x02 << 24)	//Byte/Page Program (1 to 256 Bytes)	02h   0000 0010    Up to 100 MHz          3         0    1+
#define SPICMD_AT_PP2		(0xa2 << 24)	//Dual-Input Byte/Page Program 			A2h   1010 0010    Up to 100 MHz          3         0    1+
#define SPICMD_AT_PES		(0xb0 << 24)	//Program/Erase Suspend					B0h   1011 0000    Up to 100 MHz          0         0     0
#define SPICMD_AT_PER		(0xd0 << 24)	//Program/Erase Resume					D0h   1101 0000    Up to 100 MHz          0         0     0
#define SPICMD_AT_WREN		(0x06 << 24)	//Write Enable							06h   0000 0110    Up to 100 MHz          0         0     0
#define SPICMD_AT_WRDI		(0x04 << 24)	//Write Disable							04h   0000 0100    Up to 100 MHz          0         0     0
#define SPICMD_AT_PS		(0x36 << 24)	//Protect Sector						36h   0011 0110    Up to 100 MHz          3         0     0
#define SPICMD_AT_UPS		(0x39 << 24)	//Unprotect Sector						39h   0011 1001    Up to 100 MHz          3         0     0
#define SPICMD_AT_RSPR		(0x3c << 24)	//Read Sector Protection Registers		3Ch   0011 1100    Up to 100 MHz          3         0    1+
#define SPICMD_AT_SLD		(0x33 << 24)	//Sector Lockdown						33h   0011 0011    Up to 100 MHz          3         0     1
#define SPICMD_AT_FSLS		(0x34 << 24)	//Freeze Sector Lockdown State			34h   0011 0100    Up to 100 MHz          3         0     1
#define SPICMD_AT_RSLR		(0x35 << 24)	//Read Sector Lockdown Registers		35h   0011 0101    Up to 100 MHz          3         0    1+
#define SPICMD_AT_POSR		(0x9b << 24)	//Program OTP Security Register			9Bh   1001 1011    Up to 100 MHz          3         0    1+
#define SPICMD_AT_ROSR		(0x77 << 24)	//Read OTP Security Register			77h   0111 0111    Up to 100 MHz          3         2    1+
#define SPICMD_AT_RSR		(0x05 << 24)	//Read Status Register					05h   0000 0101    Up to 100 MHz          0         0    1+
#define SPICMD_AT_WSR1		(0x01 << 24)	//Write Status Register Byte 1			01h   0000 0001    Up to 100 MHz          0         0     1
#define SPICMD_AT_WSR2		(0x31 << 24)	//Write Status Register Byte 2			31h   0011 0001    Up to 100 MHz          0         0     1
#define SPICMD_AT_RESET		(0xf0 << 24)	//Reset									F0h   1111 0000    Up to 100 MHz          0         0     1
#define SPICMD_AT_RDID		(0x9f << 24)	//Read Manufacturer and Device ID		9Fh   1001 1111    Up to 85 MHz           0         0   1 to 4
#define SPICMD_AT_DP		(0xb9 << 24)	//Deep Power-Down						B9h   1011 1001    Up to 100 MHz          0         0     0
#define SPICMD_AT_RDP		(0xab << 24)	//Resume from Deep Power-Down			ABh   1010 1011    Up to 100 MHz          0         0     0

#define AT_STATUS2_RES	5	//5:7RES   Reserved for future use  R  0 Reserved for future use.
#define AT_STATUS2_RSTE	4	//RSTE   Reset Enabled:0 Reset command is disabled (default);1 Reset command is enabled.           R/W
#define AT_STATUS2_SLE	3	//SLE   Sector Lockdown Enabled R/W
//0 Sector Lockdown and Freeze Sector Lockdown State commands are disabled (default).
//1 Sector Lockdown and Freeze Sector Lockdown State commands are enabled.
                                      
#define AT_STATUS2_PS	2	//PS   Program Suspend Status:0 No sectors are program suspended (default);1 A sector is program suspended.   R
#define AT_STATUS2_ES	1	//ES   Erase Suspend Status:0 No sectors are erase suspended (default);1 A sector is erase suspended.     R
#define AT_STATUS2_RDY	0	//RDY/BSY Ready/Busy Status:0 Device is ready;1 Device is busy with an internal operation.        R
#define AT_STATUS1_SPRL	7	//SPRL  Sector Protection Registers Locked R/W
//0 Sector Protection Registers are unlocked (default).
//1 Sector Protection Registers are locked.
#define AT_STATUS1_RES	6	//RES   Reserved for future use             R  0  Reserved for future use.
#define AT_STATUS1_EPE	5	//EPE   Erase/Program Error:0 Erase or program operation was successful;1 Erase or program error detected.R
#define AT_STATUS1_WPP	4	//WPP    Write Protect (WP) Pin Status:0 WP is asserted;1 WP is deasserted.       R
#define AT_STATUS1_SWP	2	//3:2  SWP    Software Protection Status          R 
//00 All sectors are software unprotected (all Sector Protection Registers are 0).
//01 Some sectors are software protected. Read individual Sector Protection Registers to determine which sectors are protected.
//10 Reserved for future use.
//11 All sectors are software protected (all Sector Protection Registers are 1 ??default).
#define AT_STATUS1_WEL	1	//WEL   Write Enable Latch Status:0 Device is not write enabled (default);1 Device is write enabled.R
#define AT_STATUS1_RDY	0  	//RDY/BSY Ready/Busy Status:0 Device is ready;1 Device is busy with an internal operation.R

/* Spanson Flash */
#define SPANSION		0x00010000		/*factory id*/
#define SPANSION_F		0x00010200		/*memory_type*/
#define S25FL004A		0x00010212
#define S25FL016A		0x00010214
#define S25FL032A		0x00010215
#define S25FL064A		0x00010216		/*supposed support*/
#define S25FL128P		0x00012018		/*only S25FL128P0XMFI001, Uniform 64KB secotr*/
										/*not support S25FL128P0XMFI011, Uniform 256KB secotr*/
										/*because #define SPI_BLOCK_SIZE 65536 */
#define S25FL032P		0x00010215


/* MICRONIX Flash */
#define MACRONIX		0x00C20000		/*factory_id*/
#define MACRONIX_05		0x00C22000		/*memory_type*/
#define MX25L4005		0x00C22013
#define MX25L1605D		0x00C22015
#define MX25L3205D		0x00C22016		/*supposed support*/
#define MX25L6405D		0x00C22017
#define MX25L12805D		0x00C22018

#define MACRONIX_Q1		0x00C22400		/*memory_type*/
#define MACRONIX_Q2		0x00C25E00		/*memory_type*/
#define MX25L1635D		0x00C22415
#define MX25L3235D		0x00C25E16
#define MX25L6445E		0x00C22017
#define MX25L12845E		0x00C22018

/* SST Flash */
#define SST				0x00BF0000		/*factory_id*/
#define SST_25			0x00BF2500		/*memory_type*/
#define SST_26			0x00BF2600		/*memory_tyep*/
#define SST25VF032B		0x00BF254A		//4MB
#define SST26VF016		0x00BF2601
#define SST26VF032		0x00BF2602

/* WinBond Flash */
#define WINBOND			0X00EF0000		/*factory_id*/
#define WINBOND_Q		0x00EF4000		/*memory_type*/
#define W25Q80			0x00EF4014
#define W25Q16			0x00EF4015
#define W25Q32			0x00EF4016

/* Eon Flash */
#define EON				0x001c0000		/*factory_id*/
#define EON_F			0x001c3100		/*memory_type*/
#define EON_Q			0x001c3000		/*memory_type*/
#define EN25F32			0x001c3116
#define EN25F16			0x001c3115
#define EN25Q32			0x001c3016
#define EN25Q16			0x001c3015

/* GigaDevice Flash */
#define GIGADEVICE		0x00c80000		/*factory_id*/
#define GD_Q			0x00c84000		/*memory_type*/
#define GD25Q8			0x00c84014 /*20120302 Bootcode Verified OK*/
#define GD25Q16			0x00c84015 /*20120305 Bootcode Verified OK*/
#define GD25Q32			0x00c84016  /*20120328 Linux Verified OK*/
#define GD25Q64			0x00c84017  /*20120305 Bootcode Verified OK*/
#define GD25Q128			0x00c84018  /*No sample ,Supposed OK*/

/* Atmel Flash */
#define ATMEL			0x001f0000		/*factory_id*/
#define AT25DF161		0x001f4602

#define SZIE2N_128K	0x11
#define SZIE2N_256K	0x12
#define SZIE2N_512K	0x13
#define SZIE2N_01MB	0x14
#define SZIE2N_02MB	0x15
#define SZIE2N_04MB	0x16
#define SZIE2N_08MB	0x17
#define SZIE2N_16MB	0x18
#define SZIE2N_32MB	0x19
#define SZIE2N_64MB	0x20

#define SPI_REG_READ(reg)		*((volatile unsigned int *)(reg))
#define SPI_REG_LOAD(reg,val)	while((*((volatile unsigned int *)SFCSR) & (SFCSR_SPI_RDY(1))) == 0); *((volatile unsigned int *)(reg)) = (val)

#define IOWIDTH_SINGLE			0x00
#define IOWIDTH_DUAL			0x01
#define IOWIDTH_QUAD			0x02
#define DATA_LENTH1				0x00
#define DATA_LENTH2				0x01
#define DATA_LENTH4				0x02
#define ISFAST_NO				0x00
#define ISFAST_YES				0x01
#define ISFAST_ALL				0x02
#define DUMMYCOUNT_0			0x00
#define DUMMYCOUNT_1			0x01
#define DUMMYCOUNT_2			0x02
#define DUMMYCOUNT_3			0x03
#define DUMMYCOUNT_4			0x04
#define DUMMYCOUNT_5			0x05
#define DUMMYCOUNT_6			0x06
#define DUMMYCOUNT_7			0x07
#define DUMMYCOUNT_8			0x08
#define DUMMYCOUNT_9			0x09

struct spi_flash_type spi_flash_info[2];
//unsigned char ucDispCount = 0;
unsigned char ucSFCR2 = 154;

struct spi_flash_known spi_flash_registed[] = {
/****************************************** Micronix Flash ******************************************/
//#define MX25L1605D		0x00C22015
{0x00C22015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L1605D", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define MX25L3205D		0x00C22016 50MHZ
//#define MX25L3206E		0x00C22016 86MHZ
{0x00C22016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L3205D", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define MX25L6405D		0x00C22017
{0x00C22017, 0x00, SIZEN_08M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L6405D", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define MX25L12805D		0x00C22018
{0x00C22018, 0x00, SIZEN_16M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L12805D", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define MX25L1635D		0x00C22415
{0x00C22415, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L1635D", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_q1, mxic_spi_setQEBit, mxic_cmd_write_q1
#endif
},
//#define MX25L3235D		0x00C25E16
{0x00C25E16, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "MX25L3235D", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},

///#define Micro		0x0020ba17
{0x0020ba17, 0x00, SIZEN_08M, SIZE_064K, SIZE_004K, SIZE_256B, "MCba17", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, mxic_cmd_read_q1, mxic_spi_setQEBit, mxic_cmd_write_q1
#endif
},
//#define MX25L6445E		0x00C22017
//#define MX25L12845E		0x00C22018
//#define MX25L4005			0x00C22013

/****************************************** Spanson Flash ******************************************/
//#define S25FL016A		0x00010214
{0x00010214, 0x00, SIZEN_02M, SIZE_064K, SIZE_064K, SIZE_256B, "S25FL016A", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_BE, span_cmd_read_s1, ComSrlCmd_NoneQeBit, span_cmd_write_s1
#endif
},
//#define S25FL032A		0x00010215
//#define S25FL032P		0x00010215
{0x00010215, 0x00, SIZEN_04M, SIZE_064K, SIZE_064K, SIZE_256B, "S25FL032A", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_BE, span_cmd_read_q0, span_spi_setQEBit, span_cmd_write_q0
#endif
},
//#define S25FL004A		0x00010212
//#define S25FL064A		0x00010216
{0x00010216, 0x00, SIZEN_08M, SIZE_064K, SIZE_064K, SIZE_256B, "S25FL064P", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_BE, span_cmd_read_q0, span_spi_setQEBit, span_cmd_write_q0
#endif
},
//#define S25FL128P		0x00012018

/****************************************** Eon Flash ******************************************/
//#define EN25F16			0x001c3115
{0x001c3115, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "EN25F16", 50
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, eon_cmd_read_s1, ComSrlCmd_NoneQeBit, eon_cmd_write_s1
#endif
},
//#define EN25F32			0x001c3116
{0x001c3116, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "EN25F32", 104
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, eon_cmd_read_s1, ComSrlCmd_NoneQeBit, eon_cmd_write_s1
#endif
},
//#define EN25Q16			0x001c3015
{0x001c3015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "EN25Q16", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, eon_cmd_read_q1, ComSrlCmd_NoneQeBit, eon_cmd_write_s1
#endif
},
//#define EN25Q32			0x001c3016
{0x001c3016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "EN25Q32", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, eon_cmd_read_q1, ComSrlCmd_NoneQeBit, eon_cmd_write_s1
#endif
},

#ifndef CONFIG_SPI_STD_MODE
/****************************************** SST Flash ******************************************/
//#define SST25VF032B		0x00BF254A
{0x00BF254A, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "SST25VF032B", 40
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, sst_cmd_read_s1, ComSrlCmd_NoneQeBit, sst_cmd_write_s1
#endif
},
//#define SST26VF032		0x00BF2602
//#define SST26VF016		0x00BF2601
#endif

/****************************************** GigaDevice Flash ******************************************/
//#define GD25Q8			0x00c84014
/*20120302 Note:GD25Q8 set to 120MHZ will cause unstable*/
{0x00c84014, 0x00, SIZEN_01M, SIZE_064K, SIZE_004K, SIZE_256B, "GD25Q8", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, gd_cmd_read_q0, gd_spi_setQEBit, gd_cmd_write_s1
#endif
},
//#define GD25Q16			0x00c84015
{0x00c84015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "GD25Q16", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, gd_cmd_read_q0, gd_spi_setQEBit, gd_cmd_write_s1
#endif
},
//#define GD25Q32			0x00c84016
{0x00c84016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "GD25Q32", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, gd_cmd_read_q0, gd_spi_setQEBit, gd_cmd_write_s1
#endif
},
//#define GD25Q64			0x00c84017
{0x00c84017, 0x00, SIZEN_08M, SIZE_064K, SIZE_004K, SIZE_256B, "GD25Q64", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, gd_cmd_read_q0, gd_spi_setQEBit, gd_cmd_write_s1
#endif
},
//#define GD25Q128			0x00c84018
{0x00c84018, 0x00, SIZEN_16M, SIZE_064K, SIZE_004K, SIZE_256B, "GD25Q128", 84
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, gd_cmd_read_q0, gd_spi_setQEBit, gd_cmd_write_s1
#endif
},

/****************************************** WinBond Flash ******************************************/
//#define W25Q16			0x00EF4015
{0x00EF4015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q16", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
//#define W25Q32			0x00EF4016
{0x00EF4016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q32", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
//#define W25Q64			0x00EF4017
{0x00EF4017, 0x00, SIZEN_08M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q64", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
//#define W25Q128			0x00EF4018
{0x00EF4018, 0x00, SIZEN_16M, SIZE_064K, SIZE_004K, SIZE_256B, "W25Q128", 80
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, wb_cmd_read_q0, wb_spi_setQEBit, wb_cmd_write_q0
#endif
},
//#define W25X16			0x00EF3015
{0x00EF3015, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "W25X16", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, at_cmd_read_d0, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define W25X32			0x00EF3016
{0x00EF3016, 0x00, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "W25X32", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, at_cmd_read_d0, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
//#define W25X64			0x00EF3017
{0x00EF3016, 0x00, SIZEN_08M, SIZE_064K, SIZE_004K, SIZE_256B, "W25X64", 75
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, at_cmd_read_d0, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1
#endif
},
/****************************************** ATMEL Flash ******************************************/
//#define AT25DF161		0x001f4602
{0x001f4602, 0x00, SIZEN_02M, SIZE_064K, SIZE_004K, SIZE_256B, "AT25DF161", 85
#ifndef CONFIG_SPI_STD_MODE
, ComSrlCmd_SE, at_cmd_read_d0, ComSrlCmd_NoneQeBit, at_cmd_write_d0
#endif
}
};
// spi flash probe
void spi_regist(unsigned char ucChip)
{
	unsigned int ui, i, uiCount;
	unsigned char pucBuffer[4];

	ui = ComSrlCmd_RDID(ucChip, 4);
	ui = ComSrlCmd_RDID(ucChip, 4);
	ui = ui >> 8;

	uiCount = sizeof(spi_flash_registed) / sizeof(struct spi_flash_known);

	for (i = 0; i < uiCount; i++)
	{
		if((spi_flash_registed[i].uiChipId == ui) && (spi_flash_registed[i].uiDistinguish == 0x00))
		{
			break;
		}
	}
	if(i == uiCount)
	{
		// default setting
		setFSCR(ucChip, 40, 1, 1, 31);
		set_flash_info(ucChip, ui, SIZEN_04M, SIZE_064K, SIZE_004K, SIZE_256B, "UNKNOWN", ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1);
	}
	else
	{
		// have registed
		setFSCR(ucChip, spi_flash_registed[i].uiClk, 1, 1, 31);
#ifndef CONFIG_SPI_STD_MODE
		set_flash_info(ucChip, ui, spi_flash_registed[i].uiCapacityId, spi_flash_registed[i].uiBlockSize, spi_flash_registed[i].uiSectorSize, spi_flash_registed[i].uiPageSize, spi_flash_registed[i].pcChipName, spi_flash_registed[i].pfErase, spi_flash_registed[i].pfRead, spi_flash_registed[i].pfQeBit, spi_flash_registed[i].pfPageWrite);
#else
		if((ui & 0x00ffff00) == SPANSION_F)
		{
			set_flash_info(ucChip, ui, spi_flash_registed[i].uiCapacityId, spi_flash_registed[i].uiBlockSize, spi_flash_registed[i].uiSectorSize, spi_flash_registed[i].uiPageSize, spi_flash_registed[i].pcChipName, ComSrlCmd_BE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1);
		}
		else
		{
			set_flash_info(ucChip, ui, spi_flash_registed[i].uiCapacityId, spi_flash_registed[i].uiBlockSize, spi_flash_registed[i].uiSectorSize, spi_flash_registed[i].uiPageSize, spi_flash_registed[i].pcChipName, ComSrlCmd_SE, mxic_cmd_read_s1, ComSrlCmd_NoneQeBit, mxic_cmd_write_s1);
		}
#endif
	}
	spi_flash_info[ucChip].pfQeBit(ucChip);
	prnFlashInfo(ucChip, spi_flash_info[ucChip]);
	ui = spi_flash_info[ucChip].pfRead(ucChip, 0x00, 4, pucBuffer);
	LDEBUG("spi_regist: ucChip=%x; i=%x; uiCount=%x\n", ucChip, i, uiCount);
}
// set spi_flash_info struction content
void set_flash_info(unsigned char ucChip, unsigned int chip_id, unsigned int device_cap, unsigned int block_size, unsigned int sector_size, unsigned int page_size, char* chip_name, FUNC_ERASE pfErase, FUNC_READ pfRead, FUNC_SETQEBIT pfQeBit, FUNC_PAGEWRITE pfPageWrite)
{
	unsigned int ui = 1 << device_cap;
	spi_flash_info[ucChip].chip_id = chip_id;
	spi_flash_info[ucChip].mfr_id = (chip_id >> 16) & 0xff;
	spi_flash_info[ucChip].dev_id = (chip_id >> 8) & 0xff;
	spi_flash_info[ucChip].capacity_id = (chip_id) & 0xff;
	spi_flash_info[ucChip].size_shift = calShift(spi_flash_info[ucChip].capacity_id, device_cap);
	spi_flash_info[ucChip].device_size = device_cap;			// 2 ^ N (bytes)
	spi_flash_info[ucChip].chip_size =  ui;
	spi_flash_info[ucChip].block_size = block_size;
	spi_flash_info[ucChip].block_cnt = ui / block_size;
	spi_flash_info[ucChip].sector_size = sector_size;
	spi_flash_info[ucChip].sector_cnt = ui / sector_size;
	spi_flash_info[ucChip].page_size = page_size;
	spi_flash_info[ucChip].page_cnt = sector_size / page_size;
	spi_flash_info[ucChip].chip_name = chip_name;
	spi_flash_info[ucChip].pfErase = pfErase;
	spi_flash_info[ucChip].pfWrite = ComSrlCmd_ComWriteData;
	spi_flash_info[ucChip].pfRead = pfRead;
	spi_flash_info[ucChip].pfQeBit = pfQeBit;
	spi_flash_info[ucChip].pfPageWrite = pfPageWrite;
	//SPI_REG_LOAD(SFCR2, 0x0bb08000);
	LDEBUG("set_flash_info: ucChip=%x; chip_id=%x; device_cap=%x; block_size=%x; sector_size=%x; page_size=%x; chip_name=%s\n", ucChip, chip_id, device_cap, block_size, sector_size, page_size, chip_name);
}

/****************************** Common function ******************************/
// get Dram Frequence
unsigned int CheckDramFreq(void)                       //JSW:For 8196C
{
	unsigned short usFreqBit;
#ifdef CONFIG_RTL8198
	//unsigned short usFreqVal[] = {65, 181, 150, 125, 156, 168, 237, 193};
	unsigned short usFreqVal[] = {156, 193, 181, 231, 212, 125, 237, 168}; //8196D
#else
	unsigned short usFreqVal[] = {65, 78, 125, 150, 156, 168, 193, 193}; //8196E
#endif
	usFreqBit = (0x00001C00 & (*(unsigned int*)0xb8000008)) >> 10 ;
	//NDEBUG("usFreqBit:%d\n", usFreqBit);
	//LDEBUG("CheckDramFreq:usFreqVal=%dMHZ; usFreqBit=%x; B8000008=%x;\n", usFreqVal[usFreqBit], usFreqBit, (*(unsigned int*)0xb8000008));
	//NDEBUG("SDRAM CLOCK:%dMHZ\n", usFreqVal[usFreqBit]);
	return usFreqVal[usFreqBit];
}
// Set FSCR register
void setFSCR(unsigned char ucChip, unsigned int uiClkMhz, unsigned int uiRBO, unsigned int uiWBO, unsigned int uiTCS)
{
	unsigned int ui, uiClk;
	uiClk = CheckDramFreq();
	ui = uiClk / uiClkMhz;
	if((uiClk % uiClkMhz) > 0)
	{
		ui = ui + 1;
	}
	if((ui % 2) > 0)
	{
		ui = ui + 1;
	}
	spi_flash_info[ucChip].chip_clk = uiClk / ui;
	SPI_REG_LOAD(SFCR, SFCR_SPI_CLK_DIV((ui-2)/2) | SFCR_RBO(uiRBO) | SFCR_WBO(uiWBO) | SFCR_SPI_TCS(uiTCS));
	LDEBUG("setFSCR:uiClkMhz=%d, uiRBO=%d, uiWBO=%d, uiTCS=%d, resMhz=%d, vale=%8x\n", uiClkMhz, uiRBO, uiWBO, uiTCS, spi_flash_info[ucChip].chip_clk, SPI_REG_READ(SFCR));
}
// Calculate write address group
void calAddr(unsigned int uiStart, unsigned int uiLenth, unsigned int uiSectorSize, unsigned int* uiStartAddr, unsigned int*  uiStartLen, unsigned int* uiSectorAddr, unsigned int* uiSectorCount, unsigned int* uiEndAddr, unsigned int* uiEndLen)
{
	unsigned int ui;
	// only one sector
	if ((uiStart + uiLenth) < ((uiStart / uiSectorSize + 1) * uiSectorSize))
	{	// start	
		*uiStartAddr = uiStart;
		*uiStartLen = uiLenth;
		//middle
		*uiSectorAddr = 0x00;
		*uiSectorCount = 0x00;
		// end
		*uiEndAddr = 0x00;
		*uiEndLen = 0x00;
	}
	//more then one sector
	else
	{
		// start
		*uiStartAddr = uiStart;
		*uiStartLen = uiSectorSize - (uiStart % uiSectorSize);
		if(*uiStartLen == uiSectorSize)
		{
			*uiStartLen = 0x00;
		}
		// middle
		ui = uiLenth - *uiStartLen;
		*uiSectorAddr = *uiStartAddr + *uiStartLen;
		*uiSectorCount = ui / uiSectorSize;
		//end
		*uiEndAddr = *uiSectorAddr + (*uiSectorCount * uiSectorSize);
		*uiEndLen = ui % uiSectorSize;
	}
	LDEBUG("calAddr:uiStart=%x; uiSectorSize=%x; uiLenth=%x;-> uiStartAddr=%x; uiStartLen=%x; uiSectorAddr=%x; uiSectorCount=%x; uiEndAddr=%x; uiEndLen=%x;\n",uiStart, uiSectorSize, uiLenth, *uiStartAddr, *uiStartLen, *uiSectorAddr, *uiSectorCount, *uiEndAddr, *uiEndLen);	
}
// Calculate chip capacity shift bit 
unsigned char calShift(unsigned char ucCapacityId, unsigned char ucChipSize)
{
	unsigned int ui;
	if(ucChipSize > ucCapacityId)
	{
		ui = ucChipSize - ucCapacityId;
	}
	else
	{
		ui = ucChipSize + 0x100 -ucCapacityId;
	}
	LDEBUG("calShift: ucCapacityId=%x; ucChipSize=%x; ucReturnVal=%x\n", ucCapacityId, ucChipSize, ui);
	return (unsigned char)ui;	
}
// Print spi_flash_type
void prnFlashInfo(unsigned char ucChip, struct spi_flash_type sftInfo)
{
#ifndef CONFIG_SPI_STD_MODE
	NDEBUG("\n********************************************************************************\n");
	NDEBUG("*\n");
	NDEBUG("* chip__no chip__id mfr___id dev___id cap___id size_sft dev_size chipSize\n");
	NDEBUG("* %7xh %7xh %7xh %7xh %7xh %7xh %7xh %7xh\n", ucChip, sftInfo.chip_id, sftInfo.mfr_id, sftInfo.dev_id, sftInfo.capacity_id, sftInfo.size_shift, sftInfo.device_size, sftInfo.chip_size);
	NDEBUG("* blk_size blk__cnt sec_size sec__cnt pageSize page_cnt chip_clk chipName\n");
	NDEBUG("* %7xh %7xh %7xh %7xh %7xh %7xh %7xh %s\n", sftInfo.block_size, sftInfo.block_cnt, sftInfo.sector_size, sftInfo.sector_cnt, sftInfo.page_size, sftInfo.page_cnt, sftInfo.chip_clk, sftInfo.chip_name);
	NDEBUG("* \n");
	NDEBUG("********************************************************************************\n");
#else
	NDEBUG("\n@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
	NDEBUG("@\n");
	NDEBUG("@ chip__no chip__id mfr___id dev___id cap___id size_sft dev_size chipSize\n");
	NDEBUG("@ %7xh %7xh %7xh %7xh %7xh %7xh %7xh %7xh\n", ucChip, sftInfo.chip_id, sftInfo.mfr_id, sftInfo.dev_id, sftInfo.capacity_id, sftInfo.size_shift, sftInfo.device_size, sftInfo.chip_size);
	NDEBUG("@ blk_size blk__cnt sec_size sec__cnt pageSize page_cnt chip_clk chipName\n");
	NDEBUG("@ %7xh %7xh %7xh %7xh %7xh %7xh %7xh %s\n", sftInfo.block_size, sftInfo.block_cnt, sftInfo.sector_size, sftInfo.sector_cnt, sftInfo.page_size, sftInfo.page_cnt, sftInfo.chip_clk, sftInfo.chip_name);
	NDEBUG("@ \n");
	NDEBUG("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@\n");
#endif
}
// print when writing
/*
void prnDispAddr(unsigned int uiAddr)
{
	if((ucDispCount % 0x0a) == 0)
	{
		NDEBUG("\n%8x", uiAddr);
	}
	else
	{
		NDEBUG("%8x", uiAddr);
	}
	ucDispCount++;
}
*/
// Check WIP bit
unsigned int spiFlashReady(unsigned char ucChip)
{
	unsigned int uiCount, ui;	
	uiCount = 0;
	while (1)
	{
		uiCount++;
		ui = SeqCmd_Read(ucChip, IOWIDTH_SINGLE, SPICMD_RDSR, 1);
		if ((ui & (1 << SPI_STATUS_WIP)) == 0)
		{
			break;
		}
	}
	KDEBUG("spiFlashReady: uiCount=%x\n", uiCount);	
	return uiCount;
}
//toggle CS
void rstSPIFlash(unsigned char ucChip)
{
	SFCSR_CS_L(ucChip, 0, IOWIDTH_SINGLE);
	SFCSR_CS_H(ucChip, 0, IOWIDTH_SINGLE);
	SFCSR_CS_L(ucChip, 0, IOWIDTH_SINGLE);
	SFCSR_CS_H(ucChip, 0, IOWIDTH_SINGLE);
	LDEBUG("rstFPIFlash: ucChip=%x;\n", ucChip);	
}

/****************************** Layer 1 ******************************/
// set cs high
void SFCSR_CS_L(unsigned char ucChip, unsigned char ucLen, unsigned char ucIOWidth)
{
	LDEBUG("SFCSR_CS_L: ucChip=%x; uiLen=%x; ucIOWidth=%x;\n", ucChip, ucLen, ucIOWidth);
	while((*((volatile unsigned int *)SFCSR) & (SFCSR_SPI_RDY(1))) == 0);
	*((volatile unsigned int *)(SFCSR)) = SFCSR_SPI_CSB(1 + (ucChip)) | SFCSR_LEN(ucLen) | SFCSR_SPI_RDY(1) |  SFCSR_IO_WIDTH(ucIOWidth) | SFCSR_CHIP_SEL(0) | SFCSR_CMD_BYTE(5);
	//20101215 *((volatile unsigned int *)(SFCSR)) = SFCSR_SPI_CSB(1 + (ucChip)) | SFCSR_LEN(ucLen) | SFCSR_SPI_RDY(1) |  SFCSR_IO_WIDTH(ucIOWidth);	
}
// set cs low
void SFCSR_CS_H(unsigned char ucChip, unsigned char ucLen, unsigned char ucIOWidth)
{
	LDEBUG("SFCSR_CS_H: ucChip=%x; uiLen=%x; ucIOWidth=%x;\n", ucChip, ucLen, ucIOWidth);
	if(ucLen == 0) ucLen = 1;
	while((*((volatile unsigned int *)SFCSR) & (SFCSR_SPI_RDY(1))) == 0);
	*((volatile unsigned int *)(SFCSR)) = SFCSR_SPI_CSB(3) | SFCSR_LEN(ucLen) | SFCSR_SPI_RDY(1) |  SFCSR_IO_WIDTH(ucIOWidth) | SFCSR_CHIP_SEL(0) | SFCSR_CMD_BYTE(5);
	//20101215 *((volatile unsigned int *)(SFCSR)) = SFCSR_SPI_CSB(3) | SFCSR_LEN(ucLen) | SFCSR_SPI_RDY(1) |  SFCSR_IO_WIDTH(ucIOWidth);	
}


// Read Identification (RDID) Sequence (Command 9F)
unsigned int ComSrlCmd_RDID(unsigned char ucChip, unsigned int uiLen)
{
	unsigned int ui;
	SPI_REG_LOAD(SFCR, (SFCR_SPI_CLK_DIV(7) | SFCR_RBO(1) | SFCR_WBO(1) | SFCR_SPI_TCS(31)));		//SFCR default setting
	rstSPIFlash(ucChip);
	SFCSR_CS_L(ucChip, 0, IOWIDTH_SINGLE);
	SPI_REG_LOAD(SFDR, SPICMD_RDID);
	SFCSR_CS_L(ucChip, (uiLen - 1), IOWIDTH_SINGLE);
	ui = SPI_REG_READ(SFDR);
	SFCSR_CS_H(ucChip, 0, IOWIDTH_SINGLE);
	LDEBUG("ComSrlCmd_RDID: ucChip=%x; uiLen=%x; returnValue=%x; SPICMD_RDID=%x;\n", ucChip, uiLen, ui, SPICMD_RDID);
	return ui;
}
// One byte Command
void SeqCmd_Order(unsigned char ucChip,  unsigned char ucIOWidth, unsigned int uiCmd)
{
	LDEBUG("SeqCmd_Type1: ucChip=%x; ucIOWidth=%x; SPICMD=%x;\n", ucChip, ucIOWidth, uiCmd);
	SFCSR_CS_L(ucChip, ucIOWidth, IOWIDTH_SINGLE);
	SPI_REG_LOAD(SFDR, uiCmd);
	SFCSR_CS_H(ucChip, ucIOWidth, IOWIDTH_SINGLE);
}
// One byte Command Write
void SeqCmd_Write(unsigned char ucChip,  unsigned char ucIOWidth, unsigned int uiCmd, unsigned int uiValue, unsigned char ucValueLen)
{
	SFCSR_CS_L(ucChip, DATA_LENTH1, ucIOWidth);
	SPI_REG_LOAD(SFDR, uiCmd);
	SFCSR_CS_L(ucChip, ucValueLen - 1, ucIOWidth);
	SPI_REG_LOAD(SFDR, (uiValue << ((4 - ucValueLen) * 8)));
	SFCSR_CS_H(ucChip, DATA_LENTH1, IOWIDTH_SINGLE);
	LDEBUG("SeqCmd_Write: ucChip=%x; ucIOWidth=%x; uiCmd=%x; uiValue=%x; ucValueLen=%x;\n", ucChip, ucIOWidth, uiCmd, uiValue, ucValueLen);
}
// One byte Command Read
unsigned int SeqCmd_Read(unsigned char ucChip,  unsigned char ucIOWidth, unsigned int uiCmd, unsigned char ucRDLen)
{
	unsigned int ui;	
	SFCSR_CS_L(ucChip, DATA_LENTH1, ucIOWidth);
	SPI_REG_LOAD(SFDR, uiCmd);
	SFCSR_CS_L(ucChip, ucRDLen-1, ucIOWidth);
	ui = SPI_REG_READ(SFDR);
	SFCSR_CS_H(ucChip, DATA_LENTH1, ucIOWidth);
	ui = ui >> ((4 - ucRDLen) * 8);	
	LDEBUG("SeqCmd_Read: ucChip=%x; ucIOWidth=%x; uiCmd=%x; ucRDLen=%x; RetVal=%x\n", ucChip, ucIOWidth, uiCmd, ucRDLen, ui);
	return ui;
}

/****************************** Layer 2 ******************************/
// Sector Erase (SE) Sequence (Command 20)
unsigned int ComSrlCmd_SE(unsigned char ucChip, unsigned int uiAddr)
{
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WREN);
	SeqCmd_Write(ucChip,  IOWIDTH_SINGLE, SPICMD_SE, uiAddr, 3);
	KDEBUG("ComSrlCmd_SE: ucChip=%x; uiSector=%x; uiSectorSize=%x; SPICMD_SE=%x\n", ucChip, uiAddr, spi_flash_info[ucChip].sector_size, SPICMD_SE);	
	return spiFlashReady(ucChip);
}
// Block Erase (BE) Sequence (Command D8)
unsigned int ComSrlCmd_BE(unsigned char ucChip, unsigned int uiAddr)
{
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WREN);
	SeqCmd_Write(ucChip,  IOWIDTH_SINGLE, SPICMD_BE, uiAddr, 3);	
	KDEBUG("ComSrlCmd_BE: ucChip=%x; uiBlock=%x; uiBlockSize=%x; SPICMD_BE=%x\n", ucChip, uiAddr, spi_flash_info[ucChip].block_size, SPICMD_BE);
	return spiFlashReady(ucChip);
}
// Chip Erase (CE) Sequence (Command 60 or C7)
unsigned int ComSrlCmd_CE(unsigned char ucChip)
{
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WREN);
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_CE);	
	KDEBUG("ComSrlCmd_CE: ucChip=%x; SPICMD_CE=%x\n", ucChip, SPICMD_CE);
	return spiFlashReady(ucChip);
}
// without QE bit
unsigned int ComSrlCmd_NoneQeBit(unsigned char ucChip)
{
	KDEBUG("ComSrlCmd_NoneQeBit: ucChip=%x;\n", ucChip);
	return 0;
}
// ucIsFast: = 0 cmd, address, dummy single IO ; =1 cmd single IO, address and dummy multi IO; =2 cmd, address and dummy multi IO;
void ComSrlCmd_InputCommand(unsigned char ucChip, unsigned int uiAddr, unsigned int uiCmd, unsigned char ucIsFast, unsigned char ucIOWidth, unsigned char ucDummyCount)
{
	int i;
	LDEBUG("ComSrlCmd_InputCommand: ucChip=%x; uiAddr=%x; uiCmd=%x; uiIsfast=%x; ucIOWidth=%x; ucDummyCount=%x\n", ucChip, uiAddr, uiCmd, ucIsFast, ucIOWidth, ucDummyCount);

	// input command
	if(ucIsFast == ISFAST_ALL)
	{
		SFCSR_CS_L(ucChip, 0, ucIOWidth);
	}
	else
	{
		SFCSR_CS_L(ucChip, 0, IOWIDTH_SINGLE);
	}
	SPI_REG_LOAD(SFDR, uiCmd);				// Read Command

	// input 3 bytes address
	if(ucIsFast == ISFAST_NO)
	{
		SFCSR_CS_L(ucChip, 0, IOWIDTH_SINGLE);
	}
	else
	{
		SFCSR_CS_L(ucChip, 0, ucIOWidth);
	}
	SPI_REG_LOAD(SFDR,(uiAddr << 8));
	SPI_REG_LOAD(SFDR,(uiAddr << 16));
	SPI_REG_LOAD(SFDR,(uiAddr << 24));

	//input dummy cycle
	for (i = 0; i < ucDummyCount; i++)
	{
		SPI_REG_LOAD(SFDR, 0);
	}
	
	SFCSR_CS_L(ucChip, 3, ucIOWidth);
}
// Set SFCR2 for memery map read
unsigned int SetSFCR2(unsigned int uiCmd, unsigned char ucIsFast, unsigned char ucIOWidth, unsigned char ucDummyCount)
{
	unsigned int ui, uiDy;
	ucSFCR2 = 0;
	ui = SFCR2_SFCMD(uiCmd) | SFCR2_SFSIZE(spi_flash_info[0].device_size - 17) | SFCR2_RD_OPT(0) | SFCR2_HOLD_TILL_SFDR2(0);
	switch (ucIsFast)
	{
		case ISFAST_NO:
		{
			ui = ui | SFCR2_CMD_IO(IOWIDTH_SINGLE) | SFCR2_ADDR_IO(IOWIDTH_SINGLE) | SFCR2_DATA_IO(ucIOWidth);
			uiDy = 1;
			break;
		}
		case ISFAST_YES:
		{
			ui = ui | SFCR2_CMD_IO(IOWIDTH_SINGLE) | SFCR2_ADDR_IO(ucIOWidth) | SFCR2_DATA_IO(ucIOWidth);
			uiDy = ucIOWidth * 2;
			break;
		}
		case ISFAST_ALL:
		{
			ui = ui | SFCR2_CMD_IO(ucIOWidth) | SFCR2_ADDR_IO(ucIOWidth) | SFCR2_DATA_IO(ucIOWidth);
			uiDy = ucIOWidth * 2;
			break;
		}
		default:
		{
			ui = ui | SFCR2_CMD_IO(IOWIDTH_SINGLE) | SFCR2_ADDR_IO(IOWIDTH_SINGLE) | SFCR2_DATA_IO(ucIOWidth);
			uiDy = 1;
			break;
		}
	}
	if (uiDy == 0)
	{
		uiDy = 1;
	}
	ui = ui | SFCR2_DUMMY_CYCLE((ucDummyCount * 4 / uiDy));		// ucDummyCount is Byte Count ucDummyCount*8 / (uiDy*2)
	SPI_REG_LOAD(SFCR2, ui);
	LDEBUG("SetSFCR2: uiCmd=%x; ucIsFast=%; ucIOWidth=%x; ucDummyCount=%x; ucSFCR2=%x; SFCR2=%x\n;", uiCmd, ucIsFast, ucIOWidth, ucDummyCount, ucSFCR2, ui);
	return ui;	
}
// read template
unsigned int ComSrlCmd_ComRead(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer,unsigned int uiCmd, unsigned char ucIsFast, unsigned char ucIOWidth, unsigned char ucDummyCount)
{

	unsigned int ui, uiCount, i;
	unsigned char* puc = pucBuffer;
	LDEBUG("ComSrlCmd_ComRead: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; uiCmd=%x; uiIsfast=%x; ucIOWidth=%x; ucDummyCount=%x\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, uiCmd, ucIsFast, ucIOWidth, ucDummyCount);
	ComSrlCmd_InputCommand(ucChip, uiAddr, uiCmd, ucIsFast, ucIOWidth, ucDummyCount);
	if(ucSFCR2 != 0)	// set SFCR2
	{
		ui = SetSFCR2((uiCmd >> 24), ucIsFast, ucIOWidth, ucDummyCount);
	}

	uiCount = uiLen / 4;							
	for( i = 0; i< uiCount; i++)					// Read 4 bytes every time.
	{
		ui = SPI_REG_READ(SFDR);
		memcpy(puc, &ui, 4);
		puc += 4;
	}

	i = uiLen % 4;
	if(i > 0)
	{
		ui = SPI_REG_READ(SFDR);					// another bytes.
		memcpy(puc, &ui, i);
		puc += i;
	}
	SFCSR_CS_H(ucChip, 0, IOWIDTH_SINGLE);
	return uiLen;
	
}
// write template
unsigned int ComSrlCmd_ComWrite(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer, unsigned int uiCmd, unsigned char ucIsFast, unsigned char ucIOWidth, unsigned char ucDummyCount)
{
	unsigned int ui, uiCount, i;
	unsigned char* puc = pucBuffer;
	LDEBUG("ComSrlCmd_ComWrite: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; uiCmd=%x; uiIsfast=%x; ucIOWidth=%x; ucDummyCount=%x\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, uiCmd, ucIsFast, ucIOWidth, ucDummyCount);
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WREN);

	ComSrlCmd_InputCommand(ucChip, uiAddr, uiCmd, ucIsFast, ucIOWidth, ucDummyCount);

	uiCount = uiLen / 4;
	for (i = 0; i <  uiCount; i++)
	{
		memcpy(&ui, puc, 4);
		puc += 4;
		SPI_REG_LOAD(SFDR, ui);
	}

	i = uiLen % 4;
	if(i > 0)
	{
		memcpy(&ui, puc, i);
		puc += i;
		SFCSR_CS_L(ucChip, i-1, ucIOWidth);
		SPI_REG_LOAD(SFDR, ui);
	}
	SFCSR_CS_H(ucChip, 0, IOWIDTH_SINGLE);
	ui = spiFlashReady(ucChip);
	return uiLen;
}
// write a whole sector once
unsigned int ComSrlCmd_ComWriteSector(unsigned char ucChip, unsigned int uiAddr, unsigned char* pucBuffer)
{
	unsigned int i, ui;
	unsigned char* puc = pucBuffer;
	LDEBUG("ComSrlCmd_ComWriteSector: ucChip=%x; uiAddr=%x; pucBuffer=%x; returnValue=%x;\n", ucChip, uiAddr, (unsigned int)pucBuffer, spi_flash_info[ucChip].sector_size);
	//prnDispAddr(uiAddr);

	
	#if defined( CONFIG_RTL8196D_FT1_TEST) ||defined( CONFIG_RTL8196D_FT2_TEST)
		prom_printf("#");
	#else
		NDEBUG(".");
	#endif
	ui = spi_flash_info[ucChip].pfErase(ucChip, uiAddr);
	for (i = 0; i < spi_flash_info[ucChip].page_cnt; i++)
	{
		ui = spi_flash_info[ucChip].pfPageWrite(ucChip, uiAddr, spi_flash_info[ucChip].page_size, puc);
		uiAddr += spi_flash_info[ucChip].page_size;
		puc += spi_flash_info[ucChip].page_size;
	}
	return spi_flash_info[ucChip].sector_size;
}

// write sector use malloc buffer
unsigned int ComSrlCmd_BufWriteSector(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	unsigned char pucSector[spi_flash_info[ucChip].sector_size];
	unsigned int ui, uiStartAddr, uiOffset;
	LDEBUG("ComSrlCmd_BufWriteSector:ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x;\n", ucChip, uiAddr, uiLen, pucBuffer);
	uiOffset = uiAddr % spi_flash_info[ucChip].sector_size;
	uiStartAddr = uiAddr - uiOffset;
	// get
	ui = spi_flash_info[ucChip].pfRead(ucChip, uiStartAddr, spi_flash_info[ucChip].sector_size, pucSector);
	// modify
	memcpy(pucSector + uiOffset, pucBuffer, uiLen);
	//write back
	ui = ComSrlCmd_ComWriteSector(ucChip, uiStartAddr, pucSector);
	return ui;
}

// write data, any address any lenth
unsigned int ComSrlCmd_ComWriteData(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	unsigned int uiStartAddr, uiStartLen, uiSectorAddr, uiSectorCount, uiEndAddr, uiEndLen, i;
	unsigned char* puc = pucBuffer;
	LDEBUG("ComSrlCmd_ComWriteData:ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer);
	calAddr(uiAddr, uiLen, spi_flash_info[ucChip].sector_size, &uiStartAddr, &uiStartLen, &uiSectorAddr, &uiSectorCount, &uiEndAddr, &uiEndLen);
	if((uiSectorCount == 0x00) && (uiEndLen == 0x00))	// all data in the same sector
	{
		ComSrlCmd_BufWriteSector(ucChip, uiStartAddr, uiStartLen, puc);
	}
	else
	{
		if(uiStartLen > 0)
		{
			ComSrlCmd_BufWriteSector(ucChip, uiStartAddr, uiStartLen, puc);
			puc += uiStartLen;
		}
		for(i = 0; i < uiSectorCount; i++)
		{
			ComSrlCmd_ComWriteSector(ucChip, uiSectorAddr, puc);
			puc += spi_flash_info[ucChip].sector_size;
			uiSectorAddr += spi_flash_info[ucChip].sector_size;
		}
		if(uiEndLen > 0)
		{
			ComSrlCmd_BufWriteSector(ucChip, uiEndAddr, uiEndLen, puc);
		}
	}
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WRDI);
	return uiLen;
}

/****************************** Macronix ******************************/
// Set quad enable bit
#ifndef CONFIG_SPI_STD_MODE
unsigned int mxic_spi_setQEBit(unsigned char ucChip)
{
	unsigned int ui;
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WREN);
	//ui = (0 << SPI_STATUS_REG_SRWD) | (1 << SPI_STATUS_QE) | (0 << SPI_STATUS_BP3) | (0 << SPI_STATUS_BP2) | (0 << SPI_STATUS_BP1) | (0 << SPI_STATUS_BP0) | (0 << SPI_STATUS_WEL) | (0 << SPI_STATUS_WIP);
	ui = 1 << SPI_STATUS_QE;
	SeqCmd_Write(ucChip, IOWIDTH_SINGLE, SPICMD_WRSR, ui, 1);
	KDEBUG("MxicSetQEBit: ucChip=%d; statusRegister=%x; returnValue=%x\n", ucChip, SeqCmd_Read(ucChip, IOWIDTH_SINGLE, SPICMD_RDSR, 1), ui);
	return ui;
}
#endif
// MX25L1605 MX25L3205 Read at High Speed (FAST_READ) Sequence (Command 0B)
unsigned int mxic_cmd_read_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("mxic_cmd_read_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_FASTREAD=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_FASTREAD);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_FASTREAD, ISFAST_YES, IOWIDTH_SINGLE, DUMMYCOUNT_1);
}
// MX25L1605 MX25L3205 Read at Dual IO Mode Sequence (Command BB)
#ifndef CONFIG_SPI_STD_MODE
unsigned int mxic_cmd_read_d1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("mxic_cmd_read_d1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_2READ=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_2READ);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_2READ, ISFAST_YES, IOWIDTH_DUAL, DUMMYCOUNT_1);
}
#endif
// MX25L1635 MX25L3235 4 x I/O Read Mode Sequence (Command EB)
#ifndef CONFIG_SPI_STD_MODE
unsigned int mxic_cmd_read_q1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("mxic_cmd_read_q1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_4READ=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_4READ);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_4READ, ISFAST_YES, IOWIDTH_QUAD, DUMMYCOUNT_3);
}
#endif
// Page Program (PP) Sequence (Command 02)
unsigned int mxic_cmd_write_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("mxic_cmd_write_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_PP=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_PP);
	return ComSrlCmd_ComWrite(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_PP, ISFAST_NO, IOWIDTH_SINGLE, DUMMYCOUNT_0);
}
// 4 x I/O Page Program (4PP) Sequence (Command 38)
#ifndef CONFIG_SPI_STD_MODE
unsigned int mxic_cmd_write_q1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("mxic_cmd_write_q1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_4PP=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_4PP);
	return ComSrlCmd_ComWrite(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_4PP, ISFAST_YES, IOWIDTH_QUAD, DUMMYCOUNT_0);
}
#endif

/****************************** SST ******************************/
// Layer1 SST Byte-Program
#ifndef CONFIG_SPI_STD_MODE
void SstComSrlCmd_BP(unsigned char ucChip, unsigned int uiAddr, unsigned char ucValue)
{
	unsigned int ui;
	ui = SPICMD_SST_BP | (uiAddr & 0x00ffffff);
	SFCSR_CS_L(ucChip, 3, 0);
	SPI_REG_LOAD(SFDR, ui);
	SFCSR_CS_L(ucChip, 0, IOWIDTH_SINGLE);
	SPI_REG_LOAD(SFDR, (ucValue<< 24));
	SFCSR_CS_H(ucChip, 0, IOWIDTH_SINGLE);
	LDEBUG("SstComSrlCmd_BP: ucChip=%x; uiAddr=%x; ucValue=%x; SPICMD_SST_BP=%x;\n", ucChip, uiAddr, ucValue, SPICMD_SST_BP);
}
#endif
// Read at High Speed (FAST_READ) Sequence (Command 0B)
#ifndef CONFIG_SPI_STD_MODE
unsigned int sst_cmd_read_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("sst_cmd_read_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_FASTREAD=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_FASTREAD);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_FASTREAD, ISFAST_YES, IOWIDTH_SINGLE, DUMMYCOUNT_1);
}
#endif
// Layer2 Sector Write Use BP Mode
#ifndef CONFIG_SPI_STD_MODE
unsigned int sst_cmd_write_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	unsigned int i, ui;
	unsigned char* puc = pucBuffer;
	KDEBUG("sst_cmd_write_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; returnValue=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, uiLen);
	for (i = 0; i < uiLen; i++)
	{
		SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WREN);
		SstComSrlCmd_BP(ucChip, uiAddr, *puc);
		ui = spiFlashReady(ucChip);
		puc += 1;
		uiAddr = uiAddr + 1;
	}
	return uiLen;
}
#endif

/****************************** Spansion ******************************/

// Layer2 Spansion Set QE bit
#ifndef CONFIG_SPI_STD_MODE
unsigned int span_spi_setQEBit(unsigned char ucChip)
{
	unsigned int ui;
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WREN);
	// Status
	//ui = (0 << SPAN_STATUS_SRWD) | (0 << SPAN_STATUS_PERR) | (0 << SPAN_STATUS_EERR) | (0 << SPAN_STATUS_BP2) | (0 << SPAN_STATUS_BP1) | (0 << SPAN_STATUS_BP0) | (0 << SPAN_STATUS_WEL) | (0 << SPANSON_STATUS_WIP);
	// Configure
	//ui = (ui << 8) | (0 << SPAN_CONF_TBPROT) | (0 << SPAN_CONF_BPNV) |(0 << SPAN_CONF_TBPARM) | (1 << SPAN_CONF_QUAD ) | (0 << SPAN_CONF_FREEZE);
	ui = 1 << SPAN_CONF_QUAD;
	SeqCmd_Write(ucChip, IOWIDTH_SINGLE, SPICMD_SPAN_WRR, ui, 2);
	KDEBUG("SpanSetQEBit: ucChip=%d; statusRegister=%x; returnValue=%x\n", ucChip, SeqCmd_Read(ucChip, IOWIDTH_SINGLE, SPICMD_SPAN_RCR, 2), ui);
	return spiFlashReady(ucChip);
}
#endif
// S25FL016A Layer1 Spansion FASTREAD Read Mode (Single IO)
#ifndef CONFIG_SPI_STD_MODE
unsigned int span_cmd_read_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("span_cmd_read_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_SPAN_FASTREAD=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_SPAN_FASTREAD);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer,SPICMD_SPAN_FASTREAD, ISFAST_YES, IOWIDTH_SINGLE, DUMMYCOUNT_1);
}
#endif
// S25FL032A Layer1 Spansion Quad Output Read Mode (QOR) 
#ifndef CONFIG_SPI_STD_MODE
unsigned int span_cmd_read_q0(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("span_cmd_read_q0: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_SPAN_QOR=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_SPAN_QOR);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer,SPICMD_SPAN_QOR, ISFAST_NO, IOWIDTH_QUAD, DUMMYCOUNT_1);
}
#endif
// Layer1 Spansion Single IO Program (PP)
#ifndef CONFIG_SPI_STD_MODE
unsigned int span_cmd_write_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("span_cmd_write_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_SPAN_PP=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_SPAN_PP);
	return ComSrlCmd_ComWrite(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_SPAN_PP, ISFAST_YES, IOWIDTH_SINGLE, DUMMYCOUNT_0);
}
#endif
// Layer1 Spansion QUAD Page Program (QPP)
#ifndef CONFIG_SPI_STD_MODE
unsigned int span_cmd_write_q0(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("span_cmd_write_q0: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_SPAN_QPP=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_SPAN_QPP);
	return ComSrlCmd_ComWrite(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_SPAN_QPP, ISFAST_NO, IOWIDTH_QUAD, DUMMYCOUNT_0);
}
#endif

/****************************** Winbond ******************************/
// Layer3 Winbond Set QE Bit
#ifndef CONFIG_SPI_STD_MODE
unsigned int wb_spi_setQEBit(unsigned char ucChip)
{
	//unsigned int ui, uiA, uiB;
	unsigned int ui;
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WB_WREN);
	//uiA = (0 << WB_STATUS_BUSY) | (0 << WB_STATUS_WEL) | (0 << WB_STATUS_BP0) | (0 << WB_STATUS_BP1) | (0 << WB_STATUS_BP2) | (0 << WB_STATUS_TB) | (0 << WB_STATUS_SEC) | (0 << WB_STATUS_SRP0);
 	//uiB = (0 << WB_STATUS_SRP1) | (1 << WB_STATUS_QE) | (0 << WB_STATUS_S10) | (0 << WB_STATUS_S11) | (0 << WB_STATUS_S12) | (0 << WB_STATUS_S13) | (0 << WB_STATUS_S14) | (0 << WB_STATUS_SUS);
	//ui = (uiA << 8) | (uiB >> 8);
	ui = (1 << WB_STATUS_QE) >> 8;
	SeqCmd_Write(ucChip, IOWIDTH_SINGLE, SPICMD_WB_WRSR, ui, 2);
	KDEBUG("WBSetQEBit: ucChip=%d; statusRegister=%x; returnValue=%x\n", ucChip, SeqCmd_Read(ucChip, IOWIDTH_SINGLE, SPICMD_WB_RDSR, 1), ui);
	return spiFlashReady(ucChip);
}
#endif

// W25Q80 W25Q16 W25Q32 4 x I/O Read Mode Sequence (Command EB)
#ifndef CONFIG_SPI_STD_MODE
unsigned int wb_cmd_read_q0(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("wb_cmd_read_q1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_WB_4READ=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_WB_4READ);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_WB_4READ, ISFAST_NO, IOWIDTH_QUAD, DUMMYCOUNT_1);
}
#endif
// 4 x I/O Page Program (4PP) Sequence (Command 38)
#ifndef CONFIG_SPI_STD_MODE
unsigned int wb_cmd_write_q0(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("wb_cmd_write_q1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_WB_QPP=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_WB_QPP);
	return ComSrlCmd_ComWrite(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_WB_QPP, ISFAST_NO, IOWIDTH_QUAD, DUMMYCOUNT_0);
}
#endif
/****************************** Eon ******************************/

// Eon read Single IO
#ifndef CONFIG_SPI_STD_MODE
unsigned int eon_cmd_read_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("eon_cmd_read_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_EON_FASTREAD=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_EON_FASTREAD);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_EON_FASTREAD, ISFAST_YES, IOWIDTH_SINGLE, DUMMYCOUNT_1);
}
#endif
// Eon Read Quad IO
#ifndef CONFIG_SPI_STD_MODE
unsigned int eon_cmd_read_q1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("eon_cmd_read_q1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_EON_4FASTREAD=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_EON_4FASTREAD);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_EON_4FASTREAD, ISFAST_YES, IOWIDTH_QUAD, DUMMYCOUNT_3);
}
#endif
// Page Program (PP) Sequence (Command 02)
#ifndef CONFIG_SPI_STD_MODE
unsigned int eon_cmd_write_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("eon_cmd_write_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_EON_PP=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_EON_PP);
	return ComSrlCmd_ComWrite(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_EON_PP, ISFAST_NO, IOWIDTH_SINGLE, DUMMYCOUNT_0);
}
#endif
/*
// Layer1 Eon Page Program (PP) (02h) under QIO Mode
#ifndef CONFIG_SPI_STD_MODE
unsigned int eon_cmd_write_q2(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	unsigned int ui, uiCount, i;
	unsigned char* puc = pucBuffer;
	LDEBUG("EonComSrlCmd_4PP: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_EON_PP=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_EON_PP);
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_EON_EQIO);
	SeqCmd_Order(ucChip,  IOWIDTH_QUAD, SPICMD_WREN);

	SFCSR_CS_L(ucChip, 0, IOWIDTH_QUAD);
	SPI_REG_LOAD(SFDR, SPICMD_EON_PP);			// Command
	SFCSR_CS_L(ucChip, 2, IOWIDTH_QUAD);
	SPI_REG_LOAD(SFDR, (uiAddr << 8));					// Address
	SFCSR_CS_L(ucChip, 3, IOWIDTH_QUAD);

	uiCount = uiLen / 4;
	for (i = 0; i <  uiCount; i++)
	{
		memcpy(&ui, puc, 4);
		puc += 4;
		SPI_REG_LOAD(SFDR, ui);
	}

	i = uiLen % 4;
	if(i > 0)
	{
		SFCSR_CS_L(ucChip, (i - 1), 2);
		memcpy(&ui, puc, i);
		puc += i;
		SPI_REG_LOAD(SFDR, ui);
	}
	SFCSR_CS_H(ucChip, 0, 2);
	SeqCmd_Order(ucChip,  IOWIDTH_QUAD, SPICMD_EON_RSTQIO);
	ui = spiFlashReady(ucChip);
	return uiLen;
}
#endif
*/
/****************************** Giga Device ******************************/
// Set quad enable bit
#ifndef CONFIG_SPI_STD_MODE
unsigned int gd_spi_setQEBit(unsigned char ucChip)
{
	unsigned int ui;
	SeqCmd_Order(ucChip,  IOWIDTH_SINGLE, SPICMD_WREN);
	//ui = (0 << GD_STATUS_WIP) | (0 << GD_STATUS_WEL) | (0 << GD_STATUS_BP0) | (0 << GD_STATUS_BP1) | (0 << GD_STATUS_BP2) | (0 << GD_STATUS_BP3) | ( 0 << GD_STATUS_BP4) | (0 << GD_STATUS_SRP0);
	//ui = (ui << 8) | (0 << (GD_STATUS_SRP1 - 8)) | 
	ui = 1 << (GD_STATUS_QE - 8);
	SeqCmd_Write(ucChip, IOWIDTH_SINGLE, SPICMD_GD_WRSR, ui, 2);	// set Giga Devcie QE bit
	KDEBUG("gd_spi_setQEBit: ucChip=%d; statusRegister=%x; returnValue=%x\n", ucChip, SeqCmd_Read(ucChip, IOWIDTH_SINGLE, SPICMD_RDSR, 2));
	return ui;
}
#endif
// GD25Q16 Read at Fast read Quad Sequence (Command EB)
#ifndef CONFIG_SPI_STD_MODE
unsigned int gd_cmd_read_q0(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("gd_cmd_read_q0: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_GD_READ4=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_GD_READ4);
	SeqCmd_Write(ucChip,  IOWIDTH_SINGLE, SPICMD_GD_HPM, 0x00, 3);	// command adn 3 dummy
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_GD_READ4, ISFAST_NO, IOWIDTH_QUAD, DUMMYCOUNT_1);
}
#endif
// Page Program (PP) Sequence (Command 02)
#ifndef CONFIG_SPI_STD_MODE
unsigned int gd_cmd_write_s1(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("gd_cmd_write_s1: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_GD_PP=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_PP);
	return ComSrlCmd_ComWrite(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_GD_PP, ISFAST_NO, IOWIDTH_SINGLE, DUMMYCOUNT_0);
}
#endif

/****************************** ATMEL ******************************/
// AT25DF161 Dual-Output Read Array(Command 3B)
#ifndef CONFIG_SPI_STD_MODE
unsigned int at_cmd_read_d0(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("at_cmd_read_d0: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_AT_READ2=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_AT_READ2);
	return ComSrlCmd_ComRead(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_AT_READ2, ISFAST_NO, IOWIDTH_DUAL, DUMMYCOUNT_1);
}
#endif
// AT25DF161 Dual-Input Byte/Page Program(Command A2)
#ifndef CONFIG_SPI_STD_MODE
unsigned int at_cmd_write_d0(unsigned char ucChip, unsigned int uiAddr, unsigned int uiLen, unsigned char* pucBuffer)
{
	KDEBUG("at_cmd_write_s0: ucChip=%x; uiAddr=%x; uiLen=%x; pucBuffer=%x; SPICMD_AT_PP2=%x;\n", ucChip, uiAddr, uiLen, (unsigned int)pucBuffer, SPICMD_AT_PP2);
	return ComSrlCmd_ComWrite(ucChip, uiAddr, uiLen, pucBuffer, SPICMD_AT_PP2, ISFAST_NO, IOWIDTH_DUAL, DUMMYCOUNT_0);
}
#endif



