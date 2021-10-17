#ifndef __RTL_WLAN_BITDEF_H__
#define __RTL_WLAN_BITDEF_H__

/*-------------------------Modification Log-----------------------------------
    Base on MAC_Register.doc SVN391
-------------------------Modification Log-----------------------------------*/

/*--------------------------Include File--------------------------------------*/
#include "HalHWCfg.h"
/*--------------------------Include File--------------------------------------*/

//3 ============Programming guide Start=====================
/*
    1. For all bit define, it should be prefixed by "BIT_"
    2. For all bit mask, it should be prefixed by "BIT_MASK_"
    3. For all bit shift, it should be prefixed by "BIT_SHIFT_"
    4. For other case, prefix is not needed

Example:
#define BIT_SHIFT_MAX_TXDMA         16
#define BIT_MASK_MAX_TXDMA          0x7
#define BIT_MAX_TXDMA(x)                (((x) & BIT_MASK_MAX_TXDMA)<<BIT_SHIFT_MAX_TXDMA)

    
*/
//3 ============Programming guide End=====================

// 8051FWDL
#define BIT_FW8051DWL_EN			BIT(0)
#define BIT_FW8051DWL_RDY			BIT(1)
#define BIT_ROM_DLEN				BIT(3)

//=========================================////
//	0x0000h ~ 0x00FFh	System Configuration
//=========================================//
//93C56 (93C56) 
#define BIT_EEDO					BIT(0)
#define BIT_EEDI					BIT(1)
#define BIT_EESK					BIT(2)
#define BIT_EECS					BIT(3)
#define BIT_EERPROMSEL				BIT(4)
#define BIT_AUTOLOAD_SUS			BIT(5)
#define BIT_EEM1_0(x)				(((x) & 0x3) << 6)
#define BIT_VPDIDX(x)				(((x) & 0xFF) << 8)


#if CONFIG_WLANREG_SUPPORT & (~SUPPORT_CHIP_8723A)
    //SYS_ISO_CTRL
    #define BIT_ISO_MD2PP				BIT(0)
    #define BIT_ISO_UA2USB				BIT(1)
    #define BIT_ISO_UD2CORE				BIT(2)
    #define BIT_ISO_PA2PCIE				BIT(3)
    #define BIT_ISO_DIOE				BIT(7)
    #define BIT_ISO_AFEOUTPUTCLOCK		BIT(10)
    #define BIT_UA33V_EN        		BIT(11)
    #define BIT_PC_A15V 				BIT(12)
    #define BIT_PA33V_EN            	BIT(13)
    #define BIT_PWC_EV25V				BIT(14)
    #define BIT_PWC_EBCOEB				BIT(15)
#endif

#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8723A|SUPPORT_CHIP_8881A)//1 Need Check
    //SYS_ISO_CTRL
    #define BIT_8723A_ISO_MD2PP				BIT(0)
    #define BIT_8723A_ISO_UA2UD			    BIT(1)
    #define BIT_8723A_ISO_UD2PP			    BIT(2)
    #define BIT_8723A_ISO_PA2PD			    BIT(3)
    #define BIT_8723A_ISO_PA2PP			    BIT(4)
    #define BIT_8723A_ISO_IP2MAC			BIT(5)
    #define BIT_8723A_ISO_DIOP			    BIT(6)
    #define BIT_8723A_ISO_DIOE			    BIT(7)
    #define BIT_8723A_ISO_EB2PP			    BIT(8)
    #define BIT_8723A_ISO_RFDIO			    BIT(9)
    #define BIT_8723A_UA12V_EN			    BIT(10)
    #define BIT_8723A_UA33V_EN			    BIT(11)
    #define BIT_8723A_PA12V_EN			    BIT(12)
    #define BIT_8723A_PA33V_EN			    BIT(13)
    #define BIT_8723A_PWC_EV25V			    BIT(14)
    #define BIT_8723A_PWC_EV12V			    BIT(15)
#endif


//SYS_FUNC_EN
#define BIT_FEN_BBRSTB				BIT(0)
#define BIT_FEN_BB_GLB_RSTN			BIT(1)
#define BIT_FEN_USBA				BIT(2)
#define BIT_FEN_UPLL				BIT(3)
#define BIT_FEN_USBD				BIT(4)
#define BIT_FEN_DIO_PCIE			BIT(5)
#define BIT_FEN_PCIEA				BIT(6)
#define BIT_FEN_PPLL				BIT(7)
#define BIT_FEN_PCIED				BIT(8)
#define BIT_FEN_DIOE				BIT(9)
#define BIT_FEN_CPUEN				BIT(10)
#define BIT_FEN_DCORE				BIT(11)
#define BIT_FEN_ELDR				BIT(12)
#define BIT_FEN_HWPDN				BIT(14)
#define BIT_FEN_MREGEN				BIT(15)


//SYS_CLKR
#define BIT_CNTD16V_EN				BIT(0)
#define BIT_ANA8M   				BIT(1)
#define BIT_DOMD16V_EN				BIT(2)
#define BIT_WAKEPAD_EN				BIT(3)
#define BIT_MACSLP					BIT(4)
#define BIT_LOADER_CLK_EN			BIT(5)
#define BIT_MAC_CLK_SEL(x) 		    (((x) & 0x3) << 6)
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8723A)
    #define BIT_8723A_OP_SPS_PWM_EN		BIT(6)
#endif
#define BIT_PHY_SSC_RSTB			BIT(9)
#define BIT_SEC_CLK_EN				BIT(10)
#define BIT_MAC_CLK_EN				BIT(11)
#define BIT_SYS_CLK_EN				BIT(12)
#define BIT_RING_CLK_EN				BIT(13)
#define BIT_CPU_CLK_EN				BIT(14)
#define BIT_ANA_CLK_EN				BIT(15)

//RF_CTRL
#define BIT_RF_EN					BIT(0)
#define BIT_RF_RSTB					BIT(1)
#define BIT_RF_SDMRSTB				BIT(2)


//AFE_MISC
#define BIT_AFE_BGMBEN				BIT(0)
#define BIT_AFE_MBEN				BIT(1)
#define BIT_AFEP0PC 				BIT(2)
#define BIT_AFEP1PC 				BIT(3)
#define BIT_LDO15_EN				BIT(6)
#define BIT_MAC_ID_EN				BIT(7)


//AFE_XTAL_CTRL
//KaiYuan: 88E, 8723, and 8812 is different
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8188E)
    #define BIT_8188E_XTAL_EN					BIT(0)
    #define BIT_8188E_XTAL_GMP(x)			    (((x) & 0x1F) << 1)
    #define BIT_8188E_XTAL_GMN(x)			    (((x) & 0x1F) << 6)
    #define BIT_8188E_XTAL_SC_XI(x)			    (((x) & 0x3F) << 11)
    #define BIT_8188E_XTAL_SC_XO(x)			    (((x) & 0x3F) << 17)
    #define BIT_8188E_XQSEL_RF   			    BIT(23)
    #define BIT_8188E_XQSEL        			    BIT(24)
    #define BIT_8188E_XTAL_LDO(x)   		    (((x) & 0x3) << 25)
    #define BIT_8188E_XTAL_GATED_RF1		    BIT(27)
    #define BIT_8188E_XTAL_DRV_RF1(x)  			(((x) & 0x3) << 28)
  
#endif
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8723A)
    #define BIT_8723A_XTAL_EN					BIT(0)
    #define BIT_8723A_XTAL_BSEL				    BIT(1)
    #define BIT_8723A_XTAL_LDO_VCM(x)			(((x) & 0x3) << 2)
    #define BIT_8723A_XTAL_GMP(x)			    (((x) & 0xF) << 4)
    #define BIT_8723A_XTAL_GATE_USB				BIT(8)
    #define BIT_8723A_XTAL_USB_DRV(x)			(((x) & 0x3) << 9)
    #define BIT_8723A_XTAL_GATE_AFE				BIT(11)
    #define BIT_8723A_XTAL_AFE_DRV(x)			(((x) & 0x3) << 12)
    #define BIT_8723A_XTAL_RF_GATE				BIT(14)
    #define BIT_8723A_XTAL_RF_DRV(x)			(((x) & 0x3) << 15)
    #define BIT_8723A_XTAL_GATE_DIG				BIT(17)
    #define BIT_8723A_XTAL_DIG_DRV(x)			(((x) & 0x3) << 18)    
    #define BIT_8723A_XTAL_BT_GATE				BIT(20)
    #define BIT_8723A_XTAL_BT_DRV(x)			(((x) & 0x3) << 21)
    #define BIT_8723A_XTAL_GPIO(x)			    (((x) & 0x7) << 23)        
    #define BIT_8723A_CKDLY_AFE				    BIT(26)
    #define BIT_8723A_CKDLY_USB				    BIT(27)
    #define BIT_8723A_CKDLY_DIG				    BIT(28)
    #define BIT_8723A_CKDLY_BT				    BIT(29)
#endif
//1 Need Check
#if CONFIG_WLANREG_SUPPORT & ~(SUPPORT_CHIP_8723A|SUPPORT_CHIP_8188E)
    #define BIT_XTAL_EN					BIT(0)
    #define BIT_XQSEL_RF				BIT(1)
    #define BIT_DRV_LDO_VCM(x)			(((x) & 0x3) << 2)
//    #define BIT_XTAL_GMP(x)			    (((x) & 0xF) << 4)
//    #define BIT_XTAL_GMN(x)			    (((x) & 0xF) << 8)
    #define BIT_XTAL_GUSB   			BIT(12)
    #define BIT_XTAL_DDRV(x)   		    (((x) & 0x3) << 12)
    #define BIT_XTAL_GAFE   			BIT(15)
    #define BIT_XTAL_ADRV(x)  			(((x) & 0x3) << 15)
    #define BIT_XTAL_GMP   			    BIT(18)
    #define BIT_XTAL_RDRV(x)   		    (((x) & 0x3) << 18)
//    #define BIT_XTAL_GMN   			    BIT(21)
    #define BIT_XTAL_RDRV2(x)   		(((x) & 0x3) << 21)
    #define BIT_XTAL_GDIG   			BIT(24)
    #define BIT_XTAL_DIG_DRV(x)   		(((x) & 0x3) << 24)
    #define BIT_XTAL_GPIO(x)  			(((x) & 0x7) << 26)
#endif

#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8188E)
    #define BIT_8188E_APLL_EN				BIT(0)
    #define BIT_8188E_APREG_EN				BIT(1)
    #define BIT_8188E_APLL_V12ADJ(x)		(((x) & 0x3) << 3)
    #define BIT_8188E_APLL_CP_BIAS(x)		(((x) & 0x7) << 4)
    #define BIT_8188E_APLL_KVCO(x)			(((x) & 0x3) << 7)
    #define BIT_8188E_APLL_WDOGB			BIT(9)
    #define BIT_8188E_APLL_LPFEN  			BIT(10)
    #define BIT_8188E_APLL_C3(x)			(((x) & 0x3) << 11)
    #define BIT_8188E_APLL_CP(x)			(((x) & 0x3) << 13)
    #define BIT_8188E_APLL_CS(x)			(((x) & 0x3) << 15)
    #define BIT_8188E_APLL_R3(x)			(((x) & 0x7) << 17)
    #define BIT_8188E_APLL_RS(x)			(((x) & 0x7) << 20)
    #define BIT_8188E_APLL_320M_SEL			BIT(23)
    #define BIT_8188E_APLL_REF_SEL			BIT(24)
    #define BIT_8188E_APLL_REF_EDGE			BIT(25)
    #define BIT_8188E_APLL_320M_EN			BIT(26)
    #define BIT_8188E_APLL_160M_EN			BIT(27)
    #define BIT_8188E_APLL_SYN_EN			BIT(28)
#endif
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8723A)
    #define BIT_8723A_APLL_EN				BIT(0)
    #define BIT_8723A_APLL_320_EN				BIT(1)
    #define BIT_8723A_APLL_FREE_SEL				BIT(2)
    #define BIT_8723A_APLL_EDGE_SEL				BIT(3)
    #define BIT_8723A_APLL_WDOGB				BIT(4)
    #define BIT_8723A_APLL_LPFEN				BIT(5)
    #define BIT_8723A_APLL_KVCO(x)		        (((x) & 0x3) << 6)
    #define BIT_8723A_APLL_BIAS(x)		        (((x) & 0x7) << 8)
    #define BIT_8723A_APLL_320BIAS(x)		    (((x) & 0x7) << 11)
    #define BIT_8723A_APLL_320_GATEB			BIT(14)
    #define BIT_8723A_APLL_80_GATEB				BIT(15)
    #define BIT_8723A_APLL_80DRV(x)		        (((x) & 0x3) << 16)
    #define BIT_8723A_APLL_88DRV(x)		        (((x) & 0x3) << 18)
    #define BIT_8723A_APLL_PLLDRV(x)		    (((x) & 0x3) << 20)
    #define BIT_8723A_APLL_40DRV(x)		        (((x) & 0x3) << 22)
    #define BIT_8723A_APLL_1MEN				    BIT(24)
    #define BIT_8723A_APLL_SEL_CK80M			BIT(25)
    #define BIT_8723A_AFE_DUMMY(x)		        (((x) & 0x7) << 25)
    #define BIT_8723A_XTAL_GMN(x)		        (((x) & 0x7) << 28)
    
#endif
//1 Chris Add
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8881A|SUPPORT_CHIP_8821A|SUPPORT_CHIP_8723B|SUPPORT_CHIP_8192E)
    #define BIT_APLL_EN                         BIT(0)
    #define BIT_APLL_FREE_SEL                   BIT(2)
    #define BIT_APLL_EDGE_SEL                   BIT(3)
    #define BIT_APLL_WDOGB                      BIT(4)
    #define BIT_APLL_KVCO                       BIT(6)
    #define BIT_APLL_BIAS(x)                    (((x) & 0x7) << 8)
    #define BIT_APLL_320_GATEB                  BIT(14)
    #define BIT_AFE_REG_VO_AD(x)                (((x) & 0x1) << 26)
    #define BIT_XTAL_GMN(x)               (((x) & 0xF) << 28)
#endif

#if CONFIG_WLANREG_SUPPORT & ~(SUPPORT_CHIP_8723A|SUPPORT_CHIP_8188E)
    //AFE_PLL_CTRL
    #define BIT_APLL_EN					BIT(0)
    #define BIT_CKDLY_AFE				BIT(2)
    #define BIT_XQSEL				    BIT(3)
    #define BIT_CKDLY_DIG				BIT(4)
    #define BIT_CKDLY_USB				BIT(5)
    #define BIT_REG_CC(x)			    (((x) & 0x3) << 6)
    #define BIT_REG_VOS				    BIT(8)
    #define BIT_REG_V15(x)			    (((x) & 0xF) << 9)
    #define BIT_REG_VO_AD(x)			(((x) & 0x3) << 13)
    #define BIT_REG_DOGB				BIT(15)
    #define BIT_REG_LPFEN				BIT(16)
    #define BIT_REG_KVCO(x)			    (((x) & 0x3) << 17)
    #define BIT_REG_PLLBIAS(x)			(((x) & 0x7) << 19)
    #define BIT_REG_VREF_SEL			BIT(22)
    #define BIT_REG_EDGE_SEL			BIT(23)
    #define BIT_EN_CK320M				BIT(25)
    #define BIT_EN_CK160M				BIT(26)
    #define BIT_CKMOM				    BIT(28)
    #define BIT_REG_FREF_SEL			BIT(29)
    
#endif


//----------------------------------------------------------------------------
//       REG_HIMR bits				(Offset 0xB0-B3, 32 bits)
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//       REG_HISR bits				(Offset 0xB4-B7, 32 bits)
//----------------------------------------------------------------------------
#define	HIMR_TXCCK				BIT(30)		// TXRPT interrupt when CCX bit of the packet is set	
#define	HIMR_PSTIMEOUT			BIT(29)		// Power Save Time Out Interrupt
#define	HIMR_GTINT4				BIT(28)		// When GTIMER4 expires, this bit is set to 1	
#define	HIMR_GTINT3				BIT(27)		// When GTIMER3 expires, this bit is set to 1	
#define	HIMR_TBDER				BIT(26)		// Transmit Beacon0 Error			
#define	HIMR_TBDOK				BIT(25)		// Transmit Beacon0 OK, ad hoc only
#define	HIMR_TSF_BIT32_TOGGLE	BIT(24)		// TSF Timer BIT32 toggle indication interrupt			
#define	HIMR_BcnInt				BIT(20)		// Beacon DMA Interrupt 0			
#define	HIMR_BDOK				BIT(16)		// Beacon Queue DMA OK0			
#define	HIMR_HSISR_IND_ON_INT	BIT(15)		// HSISR Indicator (HSIMR & HSISR is true, this bit is set to 1)			
#define	HIMR_BCNDMAINT_E		BIT(14)		// Beacon DMA Interrupt Extension for Win7			
#define	HIMR_ATIMEND			BIT(12)		// CTWidnow End or ATIM Window End
#define	HIMR_HISR1_IND_INT		BIT(11)		// HISR1 Indicator (HISR1 & HIMR1 is true, this bit is set to 1)
#define	HIMR_C2HCMD				BIT(10)		// CPU to Host Command INT Status, Write 1 clear	
#define	HIMR_CPWM2				BIT(9)		// CPU power Mode exchange INT Status, Write 1 clear	
#define	HIMR_CPWM				BIT(8)		// CPU power Mode exchange INT Status, Write 1 clear	
#define	HIMR_HIGHDOK			BIT(7)		// High Queue DMA OK	
#define	HIMR_MGNTDOK			BIT(6)		// Management Queue DMA OK	
#define	HIMR_BKDOK				BIT(5)		// AC_BK DMA OK		
#define	HIMR_BEDOK				BIT(4)		// AC_BE DMA OK	
#define	HIMR_VIDOK				BIT(3)		// AC_VI DMA OK		
#define	HIMR_VODOK				BIT(2)		// AC_VO DMA OK	
#define	HIMR_RDU				BIT(1)		// Rx Descriptor Unavailable	
#define	HIMR_ROK				BIT(0)		// Receive DMA OK


//----------------------------------------------------------------------------
//       REG_HIMRE bits			(Offset 0xB8-BB, 32 bits)
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//       REG_HIMSE bits			(Offset 0xBC-BF, 32 bits)
//----------------------------------------------------------------------------
#define	HIMRE_BCNDMAINT7		BIT(27)		// Beacon DMA Interrupt 7
#define	HIMRE_BCNDMAINT6		BIT(26)		// Beacon DMA Interrupt 6
#define	HIMRE_BCNDMAINT5		BIT(25)		// Beacon DMA Interrupt 5
#define	HIMRE_BCNDMAINT4		BIT(24)		// Beacon DMA Interrupt 4
#define	HIMRE_BCNDMAINT3		BIT(23)		// Beacon DMA Interrupt 3
#define	HIMRE_BCNDMAINT2		BIT(22)		// Beacon DMA Interrupt 2
#define	HIMRE_BCNDMAINT1		BIT(21)		// Beacon DMA Interrupt 1
#define	HIMRE_BCNDOK7			BIT(20)		// Beacon Queue DMA OK Interrup 7
#define	HIMRE_BCNDOK6			BIT(19)		// Beacon Queue DMA OK Interrup 6
#define	HIMRE_BCNDOK5			BIT(18)		// Beacon Queue DMA OK Interrup 5
#define	HIMRE_BCNDOK4			BIT(17)		// Beacon Queue DMA OK Interrup 4
#define	HIMRE_BCNDOK3			BIT(16)		// Beacon Queue DMA OK Interrup 3
#define	HIMRE_BCNDOK2			BIT(15)		// Beacon Queue DMA OK Interrup 2
#define	HIMRE_BCNDOK1			BIT(14)		// Beacon Queue DMA OK Interrup 1
#define	HIMRE_ATIMEND_E			BIT(13)		// ATIM Window End Extension for Win7
#define	HIMRE_TXERR				BIT(11)		// Tx Error Flag Interrupt Status, write 1 clear.
#define	HIMRE_RXERR				BIT(10)		// Rx Error Flag INT Status, Write 1 clear
#define	HIMRE_TXFOVW			BIT(9)		// Transmit FIFO Overflow
#define	HIMRE_RXFOVW			BIT(8)		// Receive FIFO Overflow


//=========================================////
//	0x0100h ~ 0x01FFh	MACTOP General Configuration
//=========================================////
//
//Function Enable Registers

#define REG_LBMODE						    (REG_CR + 3)

#define BIT_HCI_TXDMA_EN					BIT(0)
#define BIT_HCI_RXDMA_EN					BIT(1)
#define BIT_TXDMA_EN						BIT(2)
#define BIT_RXDMA_EN						BIT(3)
#define BIT_PROTOCOL_EN						BIT(4)
#define BIT_SCHEDULE_EN						BIT(5)
#define BIT_MACTXEN							BIT(6)
#define BIT_MACRXEN							BIT(7)
#define BIT_ENSWBCN							BIT(8)
#define BIT_MAC_SEC_EN						BIT(9)

//Network type
#define BIT_NETTYPE(x)						(((x) & 0x3) << 16)
#define BIT_MASK_NETTYPE					0x30000
#define BIT_NETTYPE1(x)					    (((x) & 0x3) << 18)
#define BIT_MASK_NETTYPE1					0xC0000
#define NT_NO_LINK						    0x0
#define NT_LINK_AD_HOC					    0x1
#define NT_LINK_AP						    0x2
#define NT_AS_AP						    0x3

#define BIT_LBMODE(x)						(((x) & 0xF) << 24)
#define BIT_MASK_LBMODE						0xF000000
#define LOOPBACK_NORMAL					    0x0
#define LOOPBACK_IMMEDIATELY			    0xB
#define LOOPBACK_MAC_DELAY				    0x3

//PBP - Packet Buffer Page Register 
#define BIT_MASK_PSRX						0xF
#define BIT_MASK_PSTX						0xF0
#define BIT_PSRX(x)						    ((x) & 0xF)
#define BIT_PSTX(x)						    (((x) & 0xF) << 4)

#define PBP_64							0x0
#define PBP_128							0x1
#define PBP_256							0x2
#define PBP_512							0x3
#define PBP_1024						0x4

//TX/RXDMA
#define BIT_RXDMA_ARBBW_EN					BIT(0)
#define BIT_RXSHFT_EN						BIT(1)
#define BIT_RXDMA_AGG_EN					BIT(2)
#define BIT_TXDMA_VOQ_MAP(x) 	            (((x) & 0x3) << 4 )
#define BIT_TXDMA_VIQ_MAP(x) 	            (((x) & 0x3) << 6 )
#define BIT_TXDMA_BEQ_MAP(x) 	            (((x) & 0x3) << 8 )
#define BIT_TXDMA_BKQ_MAP(x) 	            (((x) & 0x3) << 10)		
#define BIT_TXDMA_MGQ_MAP(x) 	            (((x) & 0x3) << 12)
#define BIT_TXDMA_HIQ_MAP(x) 	            (((x) & 0x3) << 14)


#define	LOW_QUEUE		1
#define	NORMAL_QUEUE	2
#define	HIGH_QUEUE	    3


//PKTBUF_DBG_CTRL
//KaiYuan: Different Chip is different
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8188E)
    #define BIT_8188E_PKTBUF_DBG_ADDR(x) 	        (((x) & 0x1FFF) << 0 )
    #define BIT_8188E_RXPKTBUF_DBG					BIT(16)
    #define BIT_8188E_TXPKTBUF_DBG					BIT(23)
#endif
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8723A)
    #define BIT_8723A_PKTBUF_DBG_ADDR(x) 	        (((x) & 0x1FFF) << 0 )
    #define BIT_8723A_TXPKTBUF_DBG					BIT(13)
    #define BIT_8723A_RXPKTBUF_DBG					BIT(14)
#endif
#if CONFIG_WLANREG_SUPPORT & ~(SUPPORT_CHIP_8188E|SUPPORT_CHIP_8723A)
    #define BIT_PKTBUF_DBG_ADDR(x) 	            (((x) & 0x3FFF) << 0 )
    #define BIT_RXPKTBUF_DBG					BIT(16)
    #define BIT_TXRPTBUF_DBG					BIT(20)
    #define BIT_TXPKTBUF_DBG					BIT(23)
    #define BIT_WRITE_BYTE_EN(x) 	            (((x) & 0xFF) << 24 )
#endif


//FFBND
#define BIT_TXPKTBUF_PGBNDY(x)				((x) & 0xFF)
#define BIT_RXFFOVFL_RSV(x)					(((x) & 0xF) << 8)
#define BIT_RXFF0_BND(x)					(((x) & 0xFFFF) << 16)


//LLT_INIT
#define LLT_NO_ACTIVE					    0x0
#define LLT_WRITE_ACCESS				    0x1
#define LLT_READ_ACCESS				        0x2

#define BIT_LLT_INIT_DATA(x)				((x) & 0xFF)
#define BIT_LLT_INIT_ADDR(x)				(((x) & 0xFF) << 8)
#define BIT_LLT_OP(x)						(((x) & 0x3) << 30)
#define BIT_LLT_OP_VALUE(x)				    (((x) >> 30) & 0x3)


//HISR0
#define BIT_RXOK        					BIT(0)
#define BIT_RDU         					BIT(1)
#define BIT_VODOK   						BIT(2)
#define BIT_VIDOK   						BIT(3)
#define BIT_BEDOK   						BIT(4)
#define BIT_BKDOK   						BIT(5)
#define BIT_MGTDOK							BIT(6)
#define BIT_HIGHDOK							BIT(7)
#define BIT_CPWM_INT						BIT(8)
#define BIT_CPWM2_INT						BIT(9)
#define BIT_C2HCMD_INT  					BIT(10)
#define BIT_HISRE_IND_INT  					BIT(11)
#define BIT_CTWEND  						BIT(12)
#define BIT_ATIMEND_E 						BIT(13)
#define BIT_BCNDMAINT_E						BIT(14)
#define BIT_HSISR_IND_ON_INT       			BIT(15)
#define BIT_BCNDOK0	    					BIT(16)
#define BIT_BCNDOK1        					BIT(17)
#define BIT_BCNDOK2        					BIT(18)
#define BIT_BCNDOK3        					BIT(19)
#define BIT_BCNDMAINT0     					BIT(20)
#define BIT_BCNDMAINT1         				BIT(21)
#define BIT_BCNDMAINT2         				BIT(22)
#define BIT_BCNDMAINT3         				BIT(23)
#define BIT_TSF_BIT32_TOGGLE				BIT(24)
#define BIT_TXBCNOK   						BIT(25)
#define BIT_TXBCNERR						BIT(26)
#define BIT_GTINT3							BIT(27)
#define BIT_GTINT4  						BIT(28)
#define BIT_PSTIMEOUT						BIT(29)
#define BIT_TIMEOUT1   		    			BIT(30)
#define BIT_TIMEOUT2   		    			BIT(31)


//=========================================//
//    EDCA  BIT DEFINITION
//=========================================//
////EDCA_VO_PARAM
#define BIT_AIFS(x)				    ((x) & 0xFF)
#define BIT_ECW_MAX_MIN(x)			((x) << 8)
#define BIT_TXOP_LIMIT(x)			((x) << 16)

#define BIT_BCNIFS(x)				((x) & 0xFF)
#define BIT_BCNECW_MIN(x)			(((x) & 0xF))<< 8)
#define BIT_BCNECW_MAX(x)			(((x) & 0xF))<< 12)

#define BIT_LRL(x)					((x) & 0x3F)
#define BIT_SRL(x)					(((x) & 0x3F) << 8)


//SIFS
#define BIT_SIFS_CCK_TRX(x)		    ((x) & 0xFF)
#define BIT_SIFS_OFDM_TRX(x)		(((x) & 0xFF) << 8);
#define BIT_SIFS_CCK_CTX(x)		    (((x) & 0xFF) << 16);
#define BIT_SIFS_OFDM_CTX(x)		(((x) & 0xFF) << 24);



//RESP_SIFS_CCK
#define BIT_SIFS_T2T_CCK(x)            ((x) & 0xff)
#define BIT_SIFS_R2T_CCK(x)            (((x) & 0xff) << 8)

//RESP_SIFS_OFDM
#define BIT_SIFS_T2T_OFDM(x)           ((x) & 0xff)
#define BIT_SIFS_R2T_OFDM(x)           (((x) & 0xff) << 8)


//TBTT PROHIBIT
#define BIT_TBTT_PROHIBIT_SETUP(x)	        ((x) & 0xF)
#define BIT_TBTT_PROHIBIT_HOLD_INFRA(x)     (((x) & 0xF) << 4)
#define BIT_TBTT_PROHIBIT_HOLD_AP(x)	    (((x) & 0xFFF) << 8)

//BCN_CTRL
#define BIT_DIS_ATIM0		    	BIT(0)
#define BIT_DIS_BCNQ_SUB0			BIT(1)
#define BIT_EN_TXBCN0_RPT			BIT(2)
#define BIT_EN_BCN0_FUNCTION		BIT(3)
#define BIT_DIS_TSF_UDT0			BIT(4)
#define BIT_BCN0_AUTO_SYNC			BIT(5)

//=========================================//
//    PROTOCOL  BIT DEFINITION
//=========================================//
//INIRTSMCS_SEL
#define BIT_INIRTSMCS_SEL(x)			((x) & 0x1F)

//MAC_SPEC_SIFS
#define BIT_SPEC_SIFS_CCK(x)			((x) & 0xFF)
#define BIT_SPEC_SIFS_OFDM(x)			(((x) & 0xFF) << 8)


//RRSR
#define BIT_RATE_1M						BIT(0)
#define BIT_RATE_2M						BIT(1)
#define BIT_RATE_5_5M					BIT(2)
#define BIT_RATE_11M					BIT(3)
#define BIT_RATE_6M						BIT(4)
#define BIT_RATE_9M						BIT(5)
#define BIT_RATE_12M					BIT(6)
#define BIT_RATE_18M					BIT(7)
#define BIT_RATE_24M					BIT(8)
#define BIT_RATE_36M					BIT(9)
#define BIT_RATE_48M					BIT(10)
#define BIT_RATE_54M					BIT(11)
#define BIT_RATE_MCS0					BIT(12)
#define BIT_RATE_MCS1					BIT(13)
#define BIT_RATE_MCS2					BIT(14)
#define BIT_RATE_MCS3					BIT(15)
#define BIT_RATE_MCS4					BIT(16)
#define BIT_RATE_MCS5					BIT(17)
#define BIT_RATE_MCS6					BIT(18)
#define BIT_RATE_MCS7					BIT(19)
#define BIT_RATE_BITMAP_ALL				0xFFFFF
#define BIT_RRSC_BITMAP(x)				((x) & 0xFFFFF)
#define BIT_RRSR_BW     				BIT(20)
#define BIT_RRSR_RSC(x)				    (((x) & 0x3) << 21)
#define RRSR_RSC_RESERVED			    0x0
#define RRSR_RSC_UPPER_SUBCHANNEL	    0x1
#define RRSR_RSC_LOWER_SUBCHANNEL	    0x2
#define RRSR_RSC_DUPLICATE_MODE		    0x3

//AGGLEN_LMT
#define BIT_AGGLMT_MCS2(x)			    ((x) & 0xF)
#define BIT_AGGLMT_MCS5(x)			    (((x) & 0xF) << 4)
#define BIT_AGGLMT_MCS7(x)			    (((x) & 0xF) << 8)
#define BIT_AGGLMT_MCS7S(x)			    (((x) & 0xF) << 12)
#define BIT_AGGLMT_MCS10(x)			    (((x) & 0xF) << 16)
#define BIT_AGGLMT_MCS12(x)			    (((x) & 0xF) << 20)
#define BIT_AGGLMT_MCS15(x)			    (((x) & 0xF) << 24)
#define BIT_AGGLMT_MCS15S(x)			(((x) & 0xF) << 28)

//DARFRC
#define BIT_DARF_RC1(x)				((x) & 0x1F)
#define BIT_DARF_RC2(x)				(((x) & 0x1F) << 8)
#define BIT_DARF_RC3(x)				(((x) & 0x1F) << 16)
#define BIT_DARF_RC4(x)				(((x) & 0x1F) << 24)
// NOTE: shift starting from address (DARFRC + 4)
#define BIT_DARF_RC5(x)				((x) & 0x1F)
#define BIT_DARF_RC6(x)				(((x) & 0x1F) << 8)
#define BIT_DARF_RC7(x)				(((x) & 0x1F) << 16)
#define BIT_DARF_RC8(x)				(((x) & 0x1F) << 24)


//RARFRC
#define BIT_RARF_RC1(x)				((x) & 0x1F)
#define BIT_RARF_RC2(x)				(((x) & 0x1F) << 8)
#define BIT_RARF_RC3(x)				(((x) & 0x1F) << 16)
#define BIT_RARF_RC4(x)				(((x) & 0x1F) << 24)
// NOTE: shift starting from address (RARFRC + 4)
#define BIT_RARF_RC5(x)				((x) & 0x1F)
#define BIT_RARF_RC6(x)				(((x) & 0x1F) << 8)
#define BIT_RARF_RC7(x)				(((x) & 0x1F) << 16)
#define BIT_RARF_RC8(x)				(((x) & 0x1F) << 24)




//=========================================//
//    SECURITY  BIT DEFINITION
//=========================================//
////CAMCMD
#define BIT_SECCAM_POLLING		BIT(31)
#define BIT_SECCAN_CLR			BIT(30)
#define BIT_MFBCAM_CLR  		BIT(29)
#define BIT_SECCAM_WE   		BIT(16)
//KaiYuan: 8812 is different with 88E and 8723
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8188E)
    #define BIT_MASK_8188E_SECCAM_ADDR	0xFF
#endif
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8723A)
    #define BIT_MASK_8723A_SECCAM_ADDR	0xFF
#endif
#if CONFIG_WLANREG_SUPPORT & ~(SUPPORT_CHIP_8723A|SUPPORT_CHIP_8188E)
    #define BIT_MASK_SECCAM_ADDR	0x1FF
#endif
#define BIT_SECCAM_ADDR(x)		((x) & BIT_MASK_SECCAM_ADDR)

//SECCFG
#define BIT_TXUHUSEDK			BIT(0)
#define BIT_RXUHUSEDK			BIT(1)
#define BIT_TXENC				BIT(2)
#define BIT_RXDEC				BIT(3)
#define BIT_SKBYA2				BIT(4)
#define BIT_NOSKMC				BIT(5)
#define BIT_TXBCUSEDK			BIT(6)
#define BIT_RXBCUSEDK			BIT(7)
#define BIT_CHK_KEYID			BIT(8)
//#define BIT_RXUSEDK			BIT(11)
//#define BIT_TXENC				BIT(12)
#define BIT_DIS_GCLK_TKIP		BIT(13)
#define BIT_DIS_GCLK_AES		BIT(14)
#define BIT_DIS_GCLK_WAPI		BIT(15)

//=========================================//
//    TXDMA  BIT DEFINITION
//=========================================//
////RQPN
#define BIT_HPQ(x)					((x) & 0xFF)
#define BIT_LPQ(x)					(((x) & 0xFF) << 8)
#define BIT_PUBQ(x)				    (((x) & 0xFF) << 16)

#define BIT_HPQ_PUBLIC_DIS			BIT(24)
#define BIT_LPQ_PUBLIC_DIS			BIT(25)

#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8723A)
    #define BIT_8723A_NPQ_PUBLIC_DIS	BIT(26)
#endif

#define BIT_LD_RQPN					BIT(31)

#define	_NPQ(x)				        ((x) & 0xFF)

//TDECTRL
#define BIT_BLK_DESC_NUM(x)		    (((x) & 0xF) << 4)
#define BIT_BCN_HEAD(x)			    (((x) & 0xFF) << 8)
#define BIT_BCN_VALID(x)			BIT(16)
#define BIT_LLT_FREE_PAGE(x)		(((x) & 0xFF) << 24)
#define BIT_SHIFT_BLK_DESC_NUM		4
#define BIT_MASK_BLK_DESC_NUM		0xF

//=========================================//
//    USB  BIT DEFINITION
//=========================================//
////USB Information (0xFE17)
#define USB_IS_HIGH_SPEED		    0
#define USB_IS_FULL_SPEED		    1
#define BIT_USB_SPEED_MASK			BIT(5)

//Special Option
#define BIT_USB_AGG_EN				BIT(3)

//-----------------------------------------------------
//
//  0x0300h ~ 0x03FFh   PCIe/LBus
//
//-----------------------------------------------------
#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8881A|SUPPORT_CHIP_8192E)

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8192E
//4 REG_PCIE_CTRL1(0x300), 4 Bytes
#define BIT_PCIEIO_PERSTB_SEL       BIT(31)

#define BIT_MASK_PCIE_MAX_RXDMA     0x7
#define BIT_SHIFT_PCIE_MAX_RXDMA    28
#define BIT_PCIE_MAX_RXDMA(x)       (((x) & BIT_MASK_PCIE_MAX_RXDMA)<<BIT_SHIFT_PCIE_MAX_RXDMA)

#define BIT_MULRW                   BIT(27)

#define BIT_MASK_PCIE_MAX_TXDMA     0x7
#define BIT_SHIFT_PCIE_MAX_TXDMA    24
#define BIT_PCIE_MAX_TXDMA(x)       (((x) & BIT_MASK_PCIE_MAX_TXDMA)<<BIT_SHIFT_PCIE_MAX_TXDMA)

#define BIT_EN_CPL_TIMEOUT_PS       BIT(22)
#define BIT_REG_TXDMA_FAIL_PS       BIT(21)
#define BIT_PCIE_RST_TRXDMA_INTF    BIT(20)
#define BIT_EN_HWENTR_L1            BIT(19)
#define BIT_EN_ADV_CLKGATE          BIT(18)
#define BIT_PCIE_EN_SWENT_L23       BIT(17)
#define BIT_PCIE_EN_HWEXT_L1        BIT(16)
#endif

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8881A
//4 REG_LX_CTRL1(0x300)
#define BIT_WT_LIT_EDN              BIT(25)
#define BIT_RD_LITT_EDN             BIT(24)

#define BIT_SHIFT_MAX_RXDMA         20
#define BIT_MASK_MAX_RXDMA          0x7
#define BIT_MAX_RXDMA(x)            (((x) & BIT_MASK_MAX_RXDMA)<<BIT_SHIFT_MAX_RXDMA)

#define BIT_SHIFT_MAX_TXDMA         16
#define BIT_MASK_MAX_TXDMA          0x7
#define BIT_MAX_TXDMA(x)            (((x) & BIT_MASK_MAX_TXDMA)<<BIT_SHIFT_MAX_TXDMA)
#endif

#if CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8881A|SUPPORT_CHIP_8192E)
#define BIT_STOP_BCNQ               BIT(14)
#define BIT_STOP_MGQ                BIT(13)
#define BIT_STOP_VOQ                BIT(12)
#define BIT_STOP_VIQ                BIT(11)
#define BIT_STOP_BEQ                BIT(10)
#define BIT_STOP_BKQ                BIT(9)
#define BIT_STOP_RXQ                BIT(8)
#define BIT_STOP_HI7Q               BIT(7)
#define BIT_STOP_HI6Q               BIT(6)
#define BIT_STOP_HI5Q               BIT(5)
#define BIT_STOP_HI4Q               BIT(4)
#define BIT_STOP_HI3Q               BIT(3)
#define BIT_STOP_HI2Q               BIT(2)
#define BIT_STOP_HI1Q               BIT(1)
#define BIT_STOP_HI0Q               BIT(0)
#endif


//4 REG_INT_MIG_CFG(0x0304), 4 Bytes
#define BIT_SHIFT_TXTTIMER_MATCH_NUM                28
#define BIT_MASK_TXTTIMER_MATCH_NUM                 0xF
#define BIT_MAX_TXTTIMER_MATCH_NUM(x)               (((x) & BIT_MASK_TXTTIMER_MATCH_NUM)<<BIT_SHIFT_TXTTIMER_MATCH_NUM)

#define BIT_SHIFT_TXPKT_NUM_MATCH                   24
#define BIT_MASK_TXPKT_NUM_MATCH                    0xF
#define BIT_MAX_TXPKT_NUM_MATCH(x)                  (((x) & BIT_MASK_TXPKT_NUM_MATCH)<<BIT_SHIFT_TXPKT_NUM_MATCH)

#define BIT_SHIFT_RXTTIMER_MATCH_NUM                20
#define BIT_MASK_RXTTIMER_MATCH_NUM                 0xF
#define BIT_MAX_RXTTIMER_MATCH_NUM(x)               (((x) & BIT_MASK_RXTTIMER_MATCH_NUM)<<BIT_SHIFT_RXTTIMER_MATCH_NUM)

#define BIT_SHIFT_RXPKT_NUM_MATCH                   16
#define BIT_MASK_RXPKT_NUM_MATCH                    0xF
#define BIT_MAX_RXPKT_NUM_MATCH(x)                  (((x) & BIT_MASK_RXPKT_NUM_MATCH)<<BIT_SHIFT_RXPKT_NUM_MATCH)

#define BIT_SHIFT_MIGRATE_TIMER                     0
#define BIT_MASK_MIGRATE_TIMER                      0xFFFF
#define BIT_MAX_MIGRATE_TIMER(x)                    (((x) & BIT_MASK_MIGRATE_TIMER)<<BIT_SHIFT_MIGRATE_TIMER)

//4 #define REG_BCNQ_TXBD_DESA          0x0308  // 8 Bytes
//4 #define REG_MGQ_TXBD_DESA           0x0310  // 8 Bytes 
//4 #define REG_VOQ_TXBD_DESA           0x0318  // 8 Bytes
//4 #define REG_VIQ_TXBD_DESA           0x0320  // 8 Bytes
//4 #define REG_BEQ_TXBD_DESA           0x0328  // 8 Bytes
//4 #define REG_BKQ_TXBD_DESA           0x0330  // 8 Bytes
//4 #define REG_RXQ_RXBD_DESA           0x0338  // 8 Bytes
//4 #define REG_HI0Q_TXBD_DESA          0x0340  // 8 Bytes
//4 #define REG_HI1Q_TXBD_DESA          0x0348  // 8 Bytes
//4 #define REG_HI2Q_TXBD_DESA          0x0350  // 8 Bytes
//4 #define REG_HI3Q_TXBD_DESA          0x0358  // 8 Bytes
//4 #define REG_HI4Q_TXBD_DESA          0x0360  // 8 Bytes
//4 #define REG_HI5Q_TXBD_DESA          0x0368  // 8 Bytes
//4 #define REG_HI6Q_TXBD_DESA          0x0370  // 8 Bytes
//4 #define REG_HI7Q_TXBD_DESA          0x0378  // 8 Bytes


//4 #define REG_MGQ_TXBD_NUM            0x0380  // 2 Bytes
#define BIT_SHIFT_MGQ_DESC_MODE                      12
#define BIT_MASK_MGQ_DESC_MODE                       0x3
#define BIT_MAX_MGQ_DESC_MODE(x)                     (((x) & BIT_MASK_MGQ_DESC_MODE)<<BIT_SHIFT_MGQ_DESC_MODE)

#define BIT_SHIFT_MGQ_DESC_NUM                      0
#define BIT_MASK_MGQ_DESC_NUM                       0xFFF
#define BIT_MAX_MGQ_DESC_NUM(x)                     (((x) & BIT_MASK_MGQ_DESC_NUM)<<BIT_SHIFT_MGQ_DESC_NUM)


//4 #define REG_RX_RXBD_NUM             0x0382  // 2 Bytes
#define BIT_SYS_32_64                               BIT(15)

#define BIT_SHIFT_BCNQ_DESC_MODE                    13
#define BIT_MASK_BCNQ_DESC_MODE                     0x3
#define BIT_MAX_BCNQ_DESC_MODE(x)                   (((x) & BIT_MASK_BCNQ_DESC_MODE)<<BIT_SHIFT_BCNQ_DESC_MODE)

#define BIT_BCNQ_FLAG                               BIT(12)

#define BIT_SHIFT_RXQ_DESC_NUM                      0
#define BIT_MASK_RXQ_DESC_NUM                       0xFFF
#define BIT_MAX_RXQ_DESC_NUM(x)                     (((x) & BIT_MASK_RXQ_DESC_NUM)<<BIT_SHIFT_RXQ_DESC_NUM)


//4 #define REG_VOQ_TXBD_NUM            0x0384  // 2 Bytes
#define BIT_VOQ_FLAG                                BIT(14)

#define BIT_SHIFT_VOQ_DESC_MODE                    12
#define BIT_MASK_VOQ_DESC_MODE                     0x3
#define BIT_MAX_VOQ_DESC_MODE(x)                   (((x) & BIT_MASK_VOQ_DESC_MODE)<<BIT_SHIFT_VOQ_DESC_MODE)

#define BIT_SHIFT_VOQ_DESC_NUM                      0
#define BIT_MASK_VOQ_DESC_NUM                       0xFFF
#define BIT_MAX_VOQ_DESC_NUM(x)                     (((x) & BIT_MASK_VOQ_DESC_NUM)<<BIT_SHIFT_VOQ_DESC_NUM)


//4 #define REG_VIQ_TXBD_NUM            0x0386  // 2 Bytes
#define BIT_VIQ_FLAG                                BIT(14)

#define BIT_SHIFT_VIQ_DESC_MODE                    12
#define BIT_MASK_VIQ_DESC_MODE                     0x3
#define BIT_MAX_VIQ_DESC_MODE(x)                   (((x) & BIT_MASK_VIQ_DESC_MODE)<<BIT_SHIFT_VIQ_DESC_MODE)

#define BIT_SHIFT_VIQ_DESC_NUM                      0
#define BIT_MASK_VIQ_DESC_NUM                       0xFFF
#define BIT_MAX_VIQ_DESC_NUM(x)                     (((x) & BIT_MASK_VIQ_DESC_NUM)<<BIT_SHIFT_VIQ_DESC_NUM)


//4 #define REG_BEQ_TXBD_NUM            0x0388  // 2 Bytes
#define BIT_BEQ_FLAG                                BIT(14)

#define BIT_SHIFT_BEQ_DESC_MODE                    12
#define BIT_MASK_BEQ_DESC_MODE                     0x3
#define BIT_MAX_BEQ_DESC_MODE(x)                   (((x) & BIT_MASK_BEQ_DESC_MODE)<<BIT_SHIFT_BEQ_DESC_MODE)

#define BIT_SHIFT_BEQ_DESC_NUM                      0
#define BIT_MASK_BEQ_DESC_NUM                       0xFFF
#define BIT_MAX_BEQ_DESC_NUM(x)                     (((x) & BIT_MASK_BEQ_DESC_NUM)<<BIT_SHIFT_BEQ_DESC_NUM)



//4 #define REG_BKQ_TXBD_NUM            0x038A  // 2 Bytes
#define BIT_BKQ_FLAG                                BIT(14)

#define BIT_SHIFT_BKQ_DESC_MODE                    12
#define BIT_MASK_BKQ_DESC_MODE                     0x3
#define BIT_MAX_BKQ_DESC_MODE(x)                   (((x) & BIT_MASK_BKQ_DESC_MODE)<<BIT_SHIFT_BKQ_DESC_MODE)

#define BIT_SHIFT_BKQ_DESC_NUM                      0
#define BIT_MASK_BKQ_DESC_NUM                       0xFFF
#define BIT_MAX_BKQ_DESC_NUM(x)                     (((x) & BIT_MASK_BKQ_DESC_NUM)<<BIT_SHIFT_BKQ_DESC_NUM)


//4 #define REG_HI0Q_TXBD_NUM            0x038C  // 2 Bytes
#define BIT_HI0Q_FLAG                                BIT(14)

#define BIT_SHIFT_HI0Q_DESC_MODE                    12
#define BIT_MASK_HI0Q_DESC_MODE                     0x3
#define BIT_MAX_HI0Q_DESC_MODE(x)                   (((x) & BIT_MASK_HI0Q_DESC_MODE)<<BIT_SHIFT_HI0Q_DESC_MODE)

#define BIT_SHIFT_HI0Q_DESC_NUM                      0
#define BIT_MASK_HI0Q_DESC_NUM                       0xFFF
#define BIT_MAX_HI0Q_DESC_NUM(x)                     (((x) & BIT_MASK_HI0Q_DESC_NUM)<<BIT_SHIFT_HI0Q_DESC_NUM)


//4 #define REG_HI1Q_TXBD_NUM            0x038E  // 2 Bytes
#define BIT_HI1Q_FLAG                                BIT(14)

#define BIT_SHIFT_HI1Q_DESC_MODE                    12
#define BIT_MASK_HI1Q_DESC_MODE                     0x3
#define BIT_MAX_HI1Q_DESC_MODE(x)                   (((x) & BIT_MASK_HI1Q_DESC_MODE)<<BIT_SHIFT_HI1Q_DESC_MODE)

#define BIT_SHIFT_HI1Q_DESC_NUM                      0
#define BIT_MASK_HI1Q_DESC_NUM                       0xFFF
#define BIT_MAX_HI1Q_DESC_NUM(x)                     (((x) & BIT_MASK_HI1Q_DESC_NUM)<<BIT_SHIFT_HI1Q_DESC_NUM)


//4 #define REG_HI2Q_TXBD_NUM            0x0390  // 2 Bytes
#define BIT_HI2Q_FLAG                                BIT(14)

#define BIT_SHIFT_HI2Q_DESC_MODE                    12
#define BIT_MASK_HI2Q_DESC_MODE                     0x3
#define BIT_MAX_HI2Q_DESC_MODE(x)                   (((x) & BIT_MASK_HI2Q_DESC_MODE)<<BIT_SHIFT_HI2Q_DESC_MODE)


#define BIT_SHIFT_HI2Q_DESC_NUM                      0
#define BIT_MASK_HI2Q_DESC_NUM                       0xFFF
#define BIT_MAX_HI2Q_DESC_NUM(x)                     (((x) & BIT_MASK_HI2Q_DESC_NUM)<<BIT_SHIFT_HI2Q_DESC_NUM)


//4 #define REG_HI3Q_TXBD_NUM            0x0392  // 2 Bytes
#define BIT_HI3Q_FLAG                                BIT(14)

#define BIT_SHIFT_HI3Q_DESC_MODE                    12
#define BIT_MASK_HI3Q_DESC_MODE                     0x3
#define BIT_MAX_HI3Q_DESC_MODE(x)                   (((x) & BIT_MASK_HI3Q_DESC_MODE)<<BIT_SHIFT_HI3Q_DESC_MODE)

#define BIT_SHIFT_HI3Q_DESC_NUM                      0
#define BIT_MASK_HI3Q_DESC_NUM                       0xFFF
#define BIT_MAX_HI3Q_DESC_NUM(x)                     (((x) & BIT_MASK_HI3Q_DESC_NUM)<<BIT_SHIFT_HI3Q_DESC_NUM)


//4 #define REG_HI4Q_TXBD_NUM            0x0394  // 2 Bytes
#define BIT_HI4Q_FLAG                                BIT(14)

#define BIT_SHIFT_HI4Q_DESC_MODE                    12
#define BIT_MASK_HI4Q_DESC_MODE                     0x3
#define BIT_MAX_HI4Q_DESC_MODE(x)                   (((x) & BIT_MASK_HI4Q_DESC_MODE)<<BIT_SHIFT_HI4Q_DESC_MODE)

#define BIT_SHIFT_HI4Q_DESC_NUM                      0
#define BIT_MASK_HI4Q_DESC_NUM                       0xFFF
#define BIT_MAX_HI4Q_DESC_NUM(x)                     (((x) & BIT_MASK_HI4Q_DESC_NUM)<<BIT_SHIFT_HI4Q_DESC_NUM)


//4 #define REG_HI5Q_TXBD_NUM            0x0396  // 2 Bytes
#define BIT_HI5Q_FLAG                                BIT(14)

#define BIT_SHIFT_HI5Q_DESC_MODE                    12
#define BIT_MASK_HI5Q_DESC_MODE                     0x3
#define BIT_MAX_HI5Q_DESC_MODE(x)                   (((x) & BIT_MASK_HI5Q_DESC_MODE)<<BIT_SHIFT_HI5Q_DESC_MODE)

#define BIT_SHIFT_HI5Q_DESC_NUM                      0
#define BIT_MASK_HI5Q_DESC_NUM                       0xFFF
#define BIT_MAX_HI5Q_DESC_NUM(x)                     (((x) & BIT_MASK_HI5Q_DESC_NUM)<<BIT_SHIFT_HI5Q_DESC_NUM)


//4 #define REG_HI6Q_TXBD_NUM            0x0398  // 2 Bytes
#define BIT_HI6Q_FLAG                                BIT(14)

#define BIT_SHIFT_HI6Q_DESC_MODE                    12
#define BIT_MASK_HI6Q_DESC_MODE                     0x3
#define BIT_MAX_HI6Q_DESC_MODE(x)                   (((x) & BIT_MASK_HI6Q_DESC_MODE)<<BIT_SHIFT_HI6Q_DESC_MODE)

#define BIT_SHIFT_HI6Q_DESC_NUM                      0
#define BIT_MASK_HI6Q_DESC_NUM                       0xFFF
#define BIT_MAX_HI6Q_DESC_NUM(x)                     (((x) & BIT_MASK_HI6Q_DESC_NUM)<<BIT_SHIFT_HI6Q_DESC_NUM)


//4 #define REG_HI7Q_TXBD_NUM            0x039A  // 2 Bytes
#define BIT_HI7Q_FLAG                                BIT(14)

#define BIT_SHIFT_HI7Q_DESC_MODE                    12
#define BIT_MASK_HI7Q_DESC_MODE                     0x3
#define BIT_MAX_HI7Q_DESC_MODE(x)                   (((x) & BIT_MASK_HI7Q_DESC_MODE)<<BIT_SHIFT_HI7Q_DESC_MODE)

#define BIT_SHIFT_HI7Q_DESC_NUM                      0
#define BIT_MASK_HI7Q_DESC_NUM                       0xFFF
#define BIT_MAX_HI7Q_DESC_NUM(x)                     (((x) & BIT_MASK_HI7Q_DESC_NUM)<<BIT_SHIFT_HI7Q_DESC_NUM)


//4 #define REG_TSFTIMER_HCI            0x039C  // 4 Bytes
#define BIT_SHIFT_TSFT2_HCI                           16
#define BIT_MASK_TSFT2_HCI                            0xFFFF
#define BIT_MAX_TSFT2_HCI(x)                         (((x) & BIT_MASK_TSFT2_HCI)<<BIT_SHIFT_TSFT2_HCI)

#define BIT_SHIFT_TSFT1_HCI                           0
#define BIT_MASK_TSFT1_HCI                            0xFFFF
#define BIT_MAX_TSFT1_HCI(x)                         (((x) & BIT_MASK_TSFT1_HCI)<<BIT_SHIFT_TSFT1_HCI)


//4 #define REG_BD_RWPTR_CLR            0x039C  // 4 Bytes
#define BIT_CLR_HI7Q_HW_IDX                             BIT(29)
#define BIT_CLR_HI6Q_HW_IDX                             BIT(28)
#define BIT_CLR_HI5Q_HW_IDX                             BIT(27)
#define BIT_CLR_HI4Q_HW_IDX                             BIT(26)
#define BIT_CLR_HI3Q_HW_IDX                             BIT(25)
#define BIT_CLR_HI2Q_HW_IDX                             BIT(24)
#define BIT_CLR_HI1Q_HW_IDX                             BIT(23)
#define BIT_CLR_HI0Q_HW_IDX                             BIT(22)
#define BIT_CLR_BKQ_HW_IDX                              BIT(21)
#define BIT_CLR_BEQ_HW_IDX                              BIT(20)
#define BIT_CLR_VIQ_HW_IDX                              BIT(19)
#define BIT_CLR_VOQ_HW_IDX                              BIT(18)
#define BIT_CLR_MGTQ_HW_IDX                             BIT(17)
#define BIT_CLR_RXQ_HW_IDX                              BIT(16)

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8881A
#define BIT_SRST_TX                                     BIT(15)
#define BIT_SRST_RX                                     BIT(14)
#endif  //CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8881A

#define BIT_CLR_HI7Q_HOST_IDX                           BIT(13)
#define BIT_CLR_HI6Q_HOST_IDX                           BIT(12)
#define BIT_CLR_HI5Q_HOST_IDX                           BIT(11)
#define BIT_CLR_HI4Q_HOST_IDX                           BIT(10)
#define BIT_CLR_HI3Q_HOST_IDX                           BIT(9)
#define BIT_CLR_HI2Q_HOST_IDX                           BIT(8)
#define BIT_CLR_HI1Q_HOST_IDX                           BIT(7)
#define BIT_CLR_HI0Q_HOST_IDX                           BIT(6)
#define BIT_CLR_BKQ_HOST_IDX                            BIT(5)
#define BIT_CLR_BEQ_HOST_IDX                            BIT(4)
#define BIT_CLR_VIQ_HOST_IDX                            BIT(3)
#define BIT_CLR_VOQ_HOST_IDX                            BIT(2)
#define BIT_CLR_MGTQ_HOST_IDX                           BIT(1)
#define BIT_CLR_RXQ_HOST_IDX                            BIT(0)


//4 #define REG_VOQ_TXBD_IDX            0x03A0  // 4 Bytes
//4 #define REG_VIQ_TXBD_IDX            0x03A4  // 4 Bytes
//4 #define REG_BEQ_TXBD_IDX            0x03A8  // 4 Bytes
//4 #define REG_BKQ_TXBD_IDX            0x03AC  // 4 Bytes
//4 #define REG_MGQ_TXBD_IDX            0x03B0  // 4 Bytes
//4 #define REG_RXQ_RXBD_IDX            0x03B4  // 4 Bytes
//4 #define REG_HI0Q_TXBD_IDX           0x03B8  // 4 Bytes
//4 #define REG_HI1Q_TXBD_IDX           0x03BC  // 4 Bytes
//4 #define REG_HI2Q_TXBD_IDX           0x03C0  // 4 Bytes
//4 #define REG_HI3Q_TXBD_IDX           0x03C4  // 4 Bytes
//4 #define REG_HI4Q_TXBD_IDX           0x03C8  // 4 Bytes
//4 #define REG_HI5Q_TXBD_IDX           0x03CC  // 4 Bytes
//4 #define REG_HI6Q_TXBD_IDX           0x03D0  // 4 Bytes
//4 #define REG_HI7Q_TXBD_IDX           0x03D4  // 4 Bytes

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8192E
//4 #define REG_DBG_SEL_V1              0x03D8  // 1 Bytes
#endif

//4 #define REG_PCIE_HRPWM1_V1          0x03D9  // 1 Bytes
//4 #define REG_PCIE_HCPWM1_V1          0x03DA  // 1 Bytes

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8192E
//4 #define REG_PICE_CTRL2              0x03DB  // 1 Bytes
#define BIT_DIS_TXDMA_PRE                           BIT(7)
#define BIT_DIS_RXDMA_PRE                           BIT(6)

#define BIT_SHIFT_HPS_CLKR_PCIE                     4
#define BIT_MASK_HPS_CLKR_PCIE                      0x3
#define BIT_HPS_CLKR_PCIE(x)                        (((x) & BIT_MASK_HPS_CLKR_PCIE)<<BIT_SHIFT_HPS_CLKR_PCIE)

#define BIT_PCIE_INT                                BIT(3)
#define BIT_TXFLAG_EXIT_L1_EN                       BIT(2)
#define BIT_EN_RXDMA_ALIGN                          BIT(1)
#define BIT_EN_TXDMA_ALIGN                          BIT(0)
#endif

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8881A
//4 #define REG_LX_CTRL2                0x03DB  // 1 Bytes
#define BIT_SHIFT_HPS_CLKR                          4
#define BIT_MASK_HPS_CLKR                           0x3
#define BIT_HPS_CLKR(x)                             (((x) & BIT_MASK_HPS_CLKR)<<BIT_SHIFT_HPS_CLKR)
#define BIT_LX_INT                                  BIT(3)
#endif

//4 #define REG_PCIE_HRPWM2_V1          0x03DC  // 2 Bytes
//4 #define REG_PCIE_HCPWM2_V1          0x03DE  // 2 Bytes
//4 #define REG_PCIE_H2C_MSG_V1         0x03E0  // 4 Bytes
//4 #define REG_PCIE_C2H_MSG_V1         0x03E4  // 4 Bytes

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8192E
//4 #define REG_DBI_WDATA_V1            0x03E8  // 4 Bytes
//4 #define REG_DBI_RDATA_V1            0x03EC  // 4 Bytes
//4 #define REG_DBI_FLAG_V1             0x03F0  // 4 Bytes
#define BIT_DBI_RFLAG                               BIT(17)
#define BIT_DBI_WFLAG                               BIT(16)

#define BIT_SHIFT_DBI_WREN                          12
#define BIT_MASK_DBI_WREN                           0xF
#define BIT_DBI_WREN(x)                             (((x) & BIT_MASK_DBI_WREN)<<BIT_SHIFT_DBI_WREN)

#define BIT_SHIFT_DBI_ADDR                          0
#define BIT_MASK_DBI_ADDR                           0xFFF
#define BIT_DBI_ADDR(x)                             (((x) & BIT_MASK_DBI_ADDR)<<BIT_SHIFT_DBI_ADDR)
#endif 

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8881A
//4 #define REG_LX_DMA_ISR              0x03E8  // 4 Bytes
#define BIT_BCN7DOK         BIT(23)
#define BIT_BCN6DOK         BIT(22)
#define BIT_BCN5DOK         BIT(21)
#define BIT_BCN4DOK         BIT(20)
#define BIT_BCN3DOK         BIT(19)
#define BIT_BCN2DOK         BIT(18)
#define BIT_BCN1DOK         BIT(17)
#define BIT_BCN0DOK         BIT(16)

#define BIT_M7DOK           BIT(15)
#define BIT_M6DOK           BIT(14)
#define BIT_M5DOK           BIT(13)
#define BIT_M4DOK           BIT(12)
#define BIT_M3DOK           BIT(11)
#define BIT_M2DOK           BIT(10)
#define BIT_M1DOK           BIT(9)
#define BIT_M0DOK           BIT(8)

#define BIT_MGTQDOK         BIT(6)
#define BIT_BKQDOK          BIT(5)
#define BIT_BEQDOK          BIT(4)
#define BIT_VIQDOK          BIT(3)
#define BIT_VOQDOK          BIT(2)
#define BIT_RDU             BIT(1)
#define BIT_RXDOK           BIT(0)

//4 #define REG_LX_DMA_IMR              0x03EC  // 4 Bytes
#define BIT_BCN7DOKM        BIT(23)
#define BIT_BCN6DOKM        BIT(22)
#define BIT_BCN5DOKM        BIT(21)
#define BIT_BCN4DOKM        BIT(20)
#define BIT_BCN3DOKM        BIT(19)
#define BIT_BCN2DOKM        BIT(18)
#define BIT_BCN1DOKM        BIT(17)
#define BIT_BCN0DOKM        BIT(16)

#define BIT_M7DOKM          BIT(15)
#define BIT_M6DOKM          BIT(14)
#define BIT_M5DOKM          BIT(13)
#define BIT_M4DOKM          BIT(12)
#define BIT_M3DOKM          BIT(11)
#define BIT_M2DOKM          BIT(10)
#define BIT_M1DOKM          BIT(9)
#define BIT_M0DOKM          BIT(8)

#define BIT_MGTQDOKM        BIT(6)
#define BIT_BKQDOKM         BIT(5)
#define BIT_BEQDOKM         BIT(4)
#define BIT_VIQDOKM         BIT(3)
#define BIT_VOQDOKM         BIT(2)
#define BIT_RDUM            BIT(1)
#define BIT_RXDOKM          BIT(0)

//4 #define REG_LX_DMA_DBG              0x03F0  // 4 Bytes
#define BIT_RX_OVER_RD_ERR              BIT(20)
#define BIT_RXDMA_STUCK                 BIT(19)

#define BIT_SHIFT_RX_STATE              16
#define BIT_MASK_RX_STATE               0x7
#define BIT_RX_STATE(x)                 (((x) & BIT_MASK_RX_STATE)<<BIT_SHIFT_RX_STATE)

#define BIT_TDE_NO_IDLE                 BIT(15)
#define BIT_TXDMA_STUCK                 BIT(14)
#define BIT_TDE_FULL_ERR                BIT(13)
#define BIT_HD_SIZE_ERR                 BIT(12)

#define BIT_SHIFT_TX_STATE              8
#define BIT_MASK_TX_STATE               0xF
#define BIT_TX_STATE(x)                 (((x) & BIT_MASK_TX_STATE)<<BIT_SHIFT_TX_STATE)

#define BIT_MST_BUSY                    BIT(3)
#define BIT_SLV_BUSY                    BIT(2)
#define BIT_RXDES_UNAVAIL               BIT(1)
#define BIT_EN_DBG_STUCK                BIT(0)
#endif

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8192E
//4 #define REG_MDIO_V1                 0x03F4  // 4 Bytes
#endif


//4 #define REG_PCIE_MIX_CFG            0x03F8  // 4 Bytes
//4 #define REG_BUS_MIX_CFG             0x03F8  // 4 Bytes
#define BIT_SHIFT_WATCH_DOG_RECORD              10
#define BIT_MASK_WATCH_DOG_RECORD               0x3FFF
#define BIT_WATCH_DOG_RECORD(x)                 (((x) & BIT_MASK_WATCH_DOG_RECORD)<<BIT_SHIFT_WATCH_DOG_RECORD)

#define BIT_R_IO_TIMEOUT_FLAG                   BIT(9)
#define BIT_EN_WATCH_DOG                        BIT(8)

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8192E
//4 //4 #define REG_PCIE_MIX_CFG            0x03F8  // 4 Bytes
#define BIT_ECRC_EN                             BIT(7)
#define BIT_MDIO_RFLAG                          BIT(6)
#define BIT_MDIO_WFLAG                          BIT(5)

#define BIT_SHIFT_MDIO_ADDRESS                  0
#define BIT_MASK_MDIO_ADDRESS                   0x1F
#define BIT_MDIO_ADDRESS(x)                     (((x) & BIT_MASK_MDIO_ADDRESS)<<BIT_SHIFT_MDIO_ADDRESS)
#endif

#if CONFIG_WLANREG_SUPPORT & SUPPORT_CHIP_8881A
//4 #define REG_BUS_MIX_CFG             0x03F8  // 4 Bytes
#endif

#endif // endif CONFIG_WLANREG_SUPPORT & (SUPPORT_CHIP_8881A|SUPPORT_CHIP_8192E)


//-----------------------------------------------------
//
//  0x0400h ~ 0x047Fh   Protocol Configuration
//
//-------------------------------------------------------
#define BIT_RATE_BITMAP_ALL				0xFFFFF

//----------------------------------------------------------------------------
//       8192C BCN_CTRL bits					(Offset 0x550, 8 bits)
//----------------------------------------------------------------------------

#define 	DIS_SUB_STATE		BIT(4)
#define 	DIS_SUB_STATE_N		BIT(1)
#define 	DIS_TSF_UPDATE		BIT(5)
#define 	DIS_TSF_UPDATE_N	BIT(4)
#define 	DIS_ATIM			BIT(0)

#define		BCN0_AUTO_SYNC	BIT(5)	// When this bit is set, TSFTR will update the timestamp in Beacon matched BSSID.
#define		DIS_TSF_UPT		BIT(4)	// 92D_REG, When this bit is set, tsf will not update
#define		EN_BCN_FUNCTION	BIT(3)	// bit=1, TSF and other beacon related functions are then enabled.
#define		EN_TXBCN_RPT		BIT(2)	//
#define		EN_MBSSID			BIT(1)	//
#define		PBCNQSEL			BIT(0)	//



//=========================================//
// 0x0600h ~ 0x07FFh	WMAC Configuration
//=========================================//
//APSD_CTRL
#define BIT_APSDOFF					BIT(6)
#define BIT_APSDOFF_STATUS			BIT(7)


//BWOPMODE
#define BIT_BW_20MHZ				BIT(2)

//TCR
#define BIT_TSFRST					BIT(0)
#define BIT_DIS_GCLK				BIT(1)
#define BIT_PAD_SEL					BIT(2)
#define BIT_HIQ_HW_MD				BIT(4)
#define BIT_HW_DTIM					BIT(5)
#define BIT_PWR_ST					BIT(6)
#define BIT_PWRBIT_OW_EN			BIT(7)
#define BIT_CRC					    BIT(8)
#define BIT_CFEND_FORMAT			BIT(9)
#define BIT_ICV						BIT(10)
#define BIT_WMAC_TCR_TXSK_PERPKT	BIT(11)
#define BIT_WMAC_TCR_ERRSTEN		BIT(12)
//1 Need Check
//#define BIT_WMAC_TCR_ERRSTEN	    BIT(13)
//#define BIT_WMAC_TCR_ERRSTEN		BIT(14)
//#define BIT_WMAC_TCR_ERRSTEN		BIT(15)
#define BIT_TSFT_CMP(x)		        (((x) & 0xFF) << 16)

//RCR
#define BIT_AAP					        BIT(0)
#define BIT_APM				            BIT(1)
#define BIT_AM					        BIT(2)
#define BIT_AB					        BIT(3)
#define BIT_ADD3				        BIT(4)
#define BIT_APWRMGT					    BIT(5)
#define BIT_CBSSID_DATA					BIT(6)
#define BIT_CBSSID_BCN			        BIT(7)
#define BIT_ACRC32					    BIT(8)
#define BIT_AICV			            BIT(9)
#define BIT_DISDECMYPKT					BIT(10)
#define BIT_ADF				            BIT(11)
#define BIT_ACF					        BIT(12)
#define BIT_AMF					        BIT(13)
#define BIT_HTC_LOC_CTRL				BIT(14)
#define BIT_RXSK_PERPKT					BIT(15)
#define BIT_UC_DATA_EN					BIT(16)
#define BIT_BM_DATA_EN			        BIT(17)
#define BIT_TIM_PARSER_EN				BIT(18)
#define BIT_PKTCTL_DLEN					BIT(20)
#define BIT_DISCHKPPDLLEN				BIT(21)
#define BIT_MFBEN					    BIT(22)
#define BIT_LSIGEN					    BIT(23)
#define BIT_ENMBID				        BIT(24)
#define BIT_TCPOFLD_EN					BIT(25)
#define BIT_APP_BASSN			        BIT(27)
#define BIT_APP_PHYSTS					BIT(28)
#define BIT_APP_ICV			            BIT(29)
#define BIT_APP_MIC					    BIT(30)
#define BIT_APP_FCS				        BIT(31)

//NAV_CTRL
#define BIT_RTSRST(x)				    ((x) & 0xFF)
#define BIT_RXMYRTS_NAV(x)			    (((x) & 0xF) << 8)
#define BIT_NAV_UPPER(x)			    (((x) & 0xFF) << 16)


//AMPDU_MIN_SPACE
#define BIT_MIN_SPACE(x)			    ((x) & 0x7)
#define BIT_SHORT_GI_PADDING(x)	        (((x) & 0x1F) << 3)


//RXERR_RPT
#define RXERR_TYPE_OFDM_PPDU		    0
#define RXERR_TYPE_OFDM_FALSE_ALARM	    1 
#define	RXERR_TYPE_OFDM_MPDU_OK		    2
#define RXERR_TYPE_OFDM_MPDU_FAIL	    3
#define RXERR_TYPE_CCK_PPDU			    4
#define RXERR_TYPE_CCK_FALSE_ALARM	    5
#define RXERR_TYPE_CCK_MPDU_OK		    6
#define RXERR_TYPE_CCK_MPDU_FAIL	    7
#define RXERR_TYPE_HT_PPDU			    8
#define RXERR_TYPE_HT_FALSE_ALARM	    9
#define RXERR_TYPE_HT_MPDU_TOTAL	    10
#define RXERR_TYPE_HT_MPDU_OK		    11
#define RXERR_TYPE_HT_MPDU_FAIL		    12
#define RXERR_TYPE_RX_FULL_DROP		    15

#define BIT_MASK_RXERR_COUNTER  		0xFFFFF
#define BIT_RXERR_RPT_RST				BIT(27)
#define BIT_RXERR_RPT_SEL(x)		    (((x) & 0xF) << 28)

//----------------------------------------------------------------------------
//       8192C RCR bits							(Offset 0x608-60B, 32 bits)
//----------------------------------------------------------------------------
#define		RCR_APP_FCS		BIT(31)	// wmac RX will append FCS after payload.
#define		RCR_APP_MIC		BIT(30)	// bit=1, MACRX will retain the MIC at the bottom of the packet.
#define		RCR_APP_ICV		BIT(29)	// bit=1, MACRX will retain the ICV at the bottom of the packet.
#define		RCR_APP_PHYSTS	BIT(28)	// Append RXFF0 PHY Status Enable.
#define		RCR_APP_BASSN		BIT(27)	// Append SSN of previous TXBA Enable.
#define		RCR_MBID_EN		BIT(24)	// Enable Multiple BssId.
#define		RCR_LSIGEN			BIT(23)	// Enable LSIG TXOP Protection function.
#define		RCR_MFBEN			BIT(22)	// Enable immediate MCS Feedback function.
#define		RCR_BM_DATA_EN	BIT(17)	// BM_DATA_EN.
#define		RCR_UC_DATA_EN	BIT(16)	// Unicast data packet interrupt enable.
#define		RCR_HTC_LOC_CTRL	BIT(14)	// 1: HTC -> MFC, 0: MFC-> HTC.
#define		RCR_AMF			BIT(13)	// Accept Management Frame.
#define		RCR_ACF			BIT(12)	// Accept Control Frame.
#define		RCR_ADF			BIT(11)	// Accept Data Frame.
#define		RCR_AICV			BIT(9)	// Accept Integrity Check Value Error packets.
#define		RCR_ACRC32			BIT(8)	// Accept CRC32 Error packets.

#define		RCR_CBSSID_ADHOC		(BIT(6)|BIT(7))	// Check BSSID.
#define		RCR_CBSSID			BIT(6)	// Check BSSID.
#define		RCR_APWRMGT		BIT(5)	// Accept Power Management Packet.
#define		RCR_ADD3			BIT(4)	// Accept Address 3 Match Packets.
#define		RCR_AB				BIT(3)	// Accept Broadcast packets.
#define		RCR_AM				BIT(2)	// Accept Multicast packets.
#define		RCR_APM			BIT(1)	// Accept Physical Match packets.
#define		RCR_AAP			BIT(0)	// Accept Destination Address packets.


// 		MACID Setting Register. (Offset 0x610 - 0x62Fh)
//----------------------------------------------------------------------------
//       8192C MBIDCAMCFG bits					(Offset 0x628-62F, 64 bits)
//----------------------------------------------------------------------------
#define		MBIDCAM_POLL		BIT(31)	// Pooling bit.
#define		MBIDWRITE_EN		BIT(30)	// Write Enable.
#define		MBIDCAM_ADDR_SHIFT	24	// CAM Address.
#define		MBIDCAM_ADDR_Mask	0x01F
#define		MBIDCAM_VALID		BIT(23)	// CAM Valid bit.

//----------------------------------------------------------------------------
//       8881A TXAGC					(Offset 0xc20-e4c)
//----------------------------------------------------------------------------
#define		bTxAGC_byte0_Jaguar	0xff
#define		bTxAGC_byte1_Jaguar	0xff00
#define		bTxAGC_byte2_Jaguar	0xff0000
#define		bTxAGC_byte3_Jaguar	0xff000000



#endif//__RTL_WLAN_BITDEF_H__
