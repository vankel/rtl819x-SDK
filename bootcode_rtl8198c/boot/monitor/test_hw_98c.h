
/* ---------------------------------
	Test 8196C hardware
-----------------------------------*/
#ifndef _HW_TEST_8881A_H
#define _HW_TEST_8881A_H


//System register Table
#define SYS_BASE 0xb8000000
#define SYS_INT_STATUS (SYS_BASE +0x04)
#define SYS_HW_STRAP   (SYS_BASE +0x08)
#define SYS_BOND_OPTION (SYS_BASE+0x0c) //new
#define SYS_CLKMANAGE (SYS_BASE +0x10)
//#define SYS_BIST_CTRL   (SYS_BASE +0x14)
#define SYS_LX_CTRL   (SYS_BASE +0x14

#define SYS_DRF_BIST_CTRL   (SYS_BASE +0x18)
#define SYS_BIST_OUT   (SYS_BASE +0x1c)
#define SYS_BIST_DONE   (SYS_BASE +0x20)
#define SYS_BIST_FAIL   (SYS_BASE +0x24)
#define SYS_DRF_BIST_DONE   (SYS_BASE +0x28)
#define SYS_DRF_BIST_FAIL   (SYS_BASE +0x2c)
#define SYS_PLL_REG   (SYS_BASE +0x30)
#define SYS_USB_SIEPHY   (SYS_BASE +0x34)
#define SYS_DLL_REG   (SYS_BASE +0x38)
#define SYS_DBG_SEL   (SYS_BASE +0x3c)
#define SYS_PIN_MUX_SEL   (SYS_BASE +0x40)
#define SYS_PIN_MUX_SEL2   (SYS_BASE +0x44)
#define SYS_PIN_MUX_SEL3   (SYS_BASE +0x4C) //leroy (+)
#define SYS_PAD_CTRL   (SYS_BASE +0x48)
#define SYS_SIGMA_24M   (SYS_BASE +0x4C)
#define SYS_PCIE_PHY0   (SYS_BASE +0x50)
#define SYS_PCIE_PHY1   (SYS_BASE +0x54)
#define SYS_PCIE_ANA   (SYS_BASE +0x58)
#define SYS_GPHY_CTRL   (SYS_BASE +0x5C)
#define SYS_GPHY_CTRL 2  (SYS_BASE +0x60)
#define SYS_GPHY_STATUS   (SYS_BASE +0x64)
//new add
#define SYS_DUMMY   (SYS_BASE +0x68)
#define SYS_EXTRA   (SYS_BASE +0x6C)
#define SYS_LX0_MST_ADDR   (SYS_BASE +0x70)
#define SYS_LX0_SLV_ADDR   (SYS_BASE +0x74)
#define SYS_LX1_MST_ADDR   (SYS_BASE +0x78)
#define SYS_LX1_SLV_ADDR   (SYS_BASE +0x7C)
#define SYS_LX2_MST_ADDR   (SYS_BASE +0x80)
#define SYS_LX2_SLV_ADDR   (SYS_BASE +0x84)
#define SYS_SWR_LDO1   (SYS_BASE +0x88)
#define SYS_SWR_LDO2   (SYS_BASE +0x8C)
#define SYS_USB_PHY   (SYS_BASE +0x90)
#define SYS_FW_DBG_SEL   (SYS_BASE +0x94)
#define SYS_OTG_CTRL   (SYS_BASE +0x98)
#define SYS_EPHY_CTRL   (SYS_BASE +0x9C)
#define SYS_HS0_CTRL   (SYS_BASE +0xA0)
#define SYS_ROM_BIST_DATAOUT   (SYS_BASE +0xA4)
#define SYS_HS0_BIST_FAIL   (SYS_BASE +0xA8)
#define SYS_HS0_DRF_BIST_FAIL   (SYS_BASE +0xAc)
#define SYS_HS0_DVS_CTRL1   (SYS_BASE +0xB0)
#define SYS_HS0_DVS_CTRL2   (SYS_BASE +0xB4)
#define SYS_HS0_DVS_CTRL3   (SYS_BASE +0xB8)
#define SYS_HS0_DVS_CTRL4   (SYS_BASE +0xBc)
#define SYS_DVS_CTRL   (SYS_BASE +0xC0)



//hw strap register
#define ST_BOOTSEL (7<<0)
#define ST_DRAMTYPE (2<<3)
#define ST_EN_EXT_RSTN (1<<5)
#define ST_OLT_MODE (2<<6)   //8198 formal chip
#define ST_PHYID (0x3<<8) //2'b11 
#define CK_M2X_FREQ_SEL (0x1f <<10)
#define ST_CPU_FREQ_SEL (0xf<<15)
#define ST_FW_CPU_FREQDIV_SEL (0x1<<19) //new
#define ST_CK_CPU_FREQDIV_SEL (0x1<<20) //new
#define ST_CLKLX_FROM_HALFOC (1<<21)
#define ST_EVER_REBOOT_ONCE (1<<22)
#define ST_CLKOC_FROM_CLKM (1<<23)
#define ST_SEL_40M (1<<24)
#define ST_TEST_MODE (1<<25)

#define CK_M2X_FREQ_SEL_OFFSET 10
#define ST_CPU_FREQ_SEL_OFFSET 15
#define ST_CPU_FREQDIV_SEL_OFFSET 19
#define ST_CLKLX_FROM_HALFOC_OFFSET 21

// clock manager register
#define ACTIVE_GDMA (1<<10)
#define ACTIVE_SWCORE (1<<11)
#define ACTIVE_LX1 (1<<12)
#define ACTIVE_ARB6 (1<<13)
#define ACTIVE_PCIE0 (1<<14)
#define ACTIVE_VOIP (1<<15)
#define ACTIVE_PCIE1 (1<<16)
#define ACTIVE_IPSEC (1<<17)
#define ACTIVE_STI_LX1 (1<<18)
#define ACTIVE_LX2 (1<<19)
#define ACTIVE_ARB4 (1<<20)
#define ACTIVE_USB (1<<21)
#define ACTIVE_IIS (1<<22)
#define ACTIVE_PCM (1<<23)
#define ACTIVE_STI_LX2 (1<<24)
#define ACTIVE_EXT_CLK24M (1<<25)
#define ACTIVE_PCIE_RST (1<<26)

#define ACTIVE_DEFAULT  (ACTIVE_LX1|ACTIVE_ARB6 |ACTIVE_STI_LX1 |ACTIVE_ARB4 |ACTIVE_STI_LX2)  //(0x00FFFFD6)//(0x00FFFFD8)

//===========================================================================
#define GICR_BASE                           0xB8003000
#define GIMR_REG                                (0x000 + GICR_BASE)       /* Global interrupt mask */
#define GISR_REG                                (0x004 + GICR_BASE)       /* Global interrupt status */
#define IRR_REG                                 (0x008 + GICR_BASE)       /* Interrupt routing */
#define IRR1_REG                                (0x00C + GICR_BASE)       /* Interrupt routing */
#define IRR2_REG                                (0x010 + GICR_BASE)       /* Interrupt routing */
#define IRR3_REG                                (0x014 + GICR_BASE)       /* Interrupt routing */

//============================================================================


#define SPEED_IRQ_NO 43  //GIMR 27 GIC 43
#define SPEED_IRR_NO (SPEED_IRQ_NO/8)   //IRR3
#define SPEED_IRR_OFFSET ((SPEED_IRQ_NO-SPEED_IRR_NO*8)*4)   //12


//#define SPEED_IRQ_NO 43
//============================================================================
#define GPIO_IRQ_NO 16  //PA0
#define GPIO_IRR_NO (SPEED_IRQ_NO/8)   //IRR2
#define GPIO_IRR_OFFSET ((SPEED_IRQ_NO-SPEED_IRR_NO*8)*4)   // 0

#define GPIO2_IRQ_NO 17  //Port EFGH
#define GPIO2_IRR_NO (GPIO2_IRQ_NO/8)   //IRR2
#define GPIO2_IRR_OFFSET ((GPIO2_IRQ_NO-GPIO2_IRR_NO*8)*4)   // 4
//============================================================================

	#define GET_BITVAL(v,bitpos,pat) ((v& (pat<<bitpos))>>bitpos)
	#define BIT_RANG1 1
	#define BIT_RANG2 3
	#define BIT_RANG3  7
	#define BIT_RANG4 0xf	
//============================================================================

#define TIMER1_IRQ_NO 9  //Timer1
#define TIMER1_IRR_NO (TIMER1_IRQ_NO/8)   //IRR1
#define TIMER1_IRR_OFFSET ((TIMER1_IRQ_NO-TIMER1_IRR_NO*8)*4)   // offset bit 4
//============================================================================



#define I2CBASE 0xb8000600
#define I2C_CMD 		(I2CBASE+0x00)
#define I2C_STATUS	 (I2CBASE+0x04)
#define I2C_REGADDR (I2CBASE+0x08)
#define I2C_CONFIG 	(I2CBASE+0x0c)

#define I2C_DATAW0 (I2CBASE+0x10)
#define I2C_DATAW1 (I2CBASE+0x14)
#define I2C_DATAW2 (I2CBASE+0x18)
#define I2C_DATAW3 (I2CBASE+0x1c)
#define I2C_DATAW4 (I2CBASE+0x20)
#define I2C_DATAW5 (I2CBASE+0x24)
#define I2C_DATAW6 (I2CBASE+0x28)
#define I2C_DATAW7 (I2CBASE+0x2c)


#define I2C_DATAR0 (I2CBASE+0x30)
#define I2C_DATAR1 (I2CBASE+0x34)
#define I2C_DATAR2 (I2CBASE+0x38)
#define I2C_DATAR3 (I2CBASE+0x3c)
#define I2C_DATAR4 (I2CBASE+0x40)
#define I2C_DATAR5 (I2CBASE+0x44)
#define I2C_DATAR6 (I2CBASE+0x48)
#define I2C_DATAR7 (I2CBASE+0x4c)

#define I2C_DATAR8 (I2CBASE+0x50)
#define I2C_DATAR9 (I2CBASE+0x54)
#define I2C_DATAR10 (I2CBASE+0x58)
#define I2C_DATAR11 (I2CBASE+0x5c)
#define I2C_DATAR12 (I2CBASE+0x60)
#define I2C_DATAR13 (I2CBASE+0x64)
#define I2C_DATAR14 (I2CBASE+0x68)
#define I2C_DATAR15 (I2CBASE+0x6c)


//0x00
#define I2C_CMD_SLAVE_ADDR_OFFSET (0)
#define I2C_CMD_AUTO_POLLING_OFFSET (7)
#define I2C_CMD_READ_LENGTH_OFFSET (16)
#define I2C_CMD_WRITE_LENGTH_OFFSET (23)
#define I2C_CMD_REG_ADDR_OFFSET (29)

//0x04
#define I2C_STATUS_INTSTS 			(1<<0)
#define I2C_STATUS_INTMASK 		(1<<1)
#define I2C_STATUS_POLLING_FAIL 	(1<<2)
#define I2C_STATUS_WDATA_FAIL 	(1<<3)
#define I2C_STATUS_WREG_FAIL 		(1<<4)
#define I2C_STATUS_WSLVADDR_FAIL 	(1<<5)
#define I2C_STATUS_READY 			(1<<6)

//0x0c
#define I2C_SCONFIG_CLKSEL_OFFSET (0)
#define I2C_SCONFIG_POLLINGTIMER_OFFSET (16)
#endif
