#ifndef _HALLBCOMBIT_H_
#define _HALLBCOMBIT_H_


#define _LBMODE(x)						(((x) & 0xF) << 24)
#define MASK_LBMODE						0xF000000
#define LOOPBACK_NORMAL					0x0
#define LOOPBACK_IMMEDIATELY			0xB
#define LOOPBACK_MAC_DELAY				0x3
#define LOOPBACK_PHY					0x1
#define LOOPBACK_DMA					0x7

//----------------------------------------------------------------------------
//       8192C MBID_NUM bits					(Offset 0x552, 8 bits)
//----------------------------------------------------------------------------
#define		MBID_BCN_NUM_SHIFT	0	// num of virtual interface num excluding the root.
#define		MBID_BCN_NUM_Mask	0x07


//----------------------------------------------------------------------------
//       8192C MBSSID_BCN_SPACE bits			(Offset 0x554-557, 32 bits)
//----------------------------------------------------------------------------
#define		BCN_SPACE2_SHIFT	16	//
#define		BCN_SPACE2_Mask	0x0FFFF
#define		BCN_SPACE1_SHIFT	0	//
#define		BCN_SPACE1_Mask	0x0FFFF




//=========================================//
//    TXDMA  BIT DEFINITION
//=========================================//
//2 //2 RQPN
#define _HPQ(x)					(x)
#define _LPQ(x)					((x) << 8)
#define _PUBQ(x)				((x) << 16)


//2 PBP - Page Size Register
#define GET_RX_PAGE_SIZE(value)			((value) & 0xF)
#define GET_TX_PAGE_SIZE(value)			(((value) & 0xF0) >> 4)
#define _PSRX_MASK						0xF
#define _PSTX_MASK						0xF0
#define _PSRX(x)						(x)
#define _PSTX(x)						((x) << 4)

//2 SPEC SIFS
#define _SPEC_SIFS_CCK(x)			((x) & 0xFF)
#define _SPEC_SIFS_OFDM(x)			(((x) & 0xFF) << 8)

//2 EDCA_VO_PARAM
#define _AIFS(x)				(x)
#define _ECW_MAX_MIN(x)			((x) << 8)
#define _TXOP_LIMIT(x)			((x) << 16)


#define _BCNIFS(x)				((x) & 0xFF)
#define _BCNECW(x)				(((x) & 0xF))<< 8)


#define _LRL(x)					((x) & 0x3F)
#define _SRL(x)					(((x) & 0x3F) << 8)


//2 SIFS_CCK
#define _SIFS_CCK_CTX(x)		((x) & 0xFF)
#define _SIFS_CCK_TRX(x)		(((x) & 0xFF) << 8);


//2 SIFS_OFDM
#define _SIFS_OFDM_CTX(x)		((x) & 0xFF)
#define _SIFS_OFDM_TRX(x)		(((x) & 0xFF) << 8);


//2 RESP SIF

#define _SIFS_R2T_CCK(x)          ((x) & 0xff)
#define _SIFS_R2T_OFDM(x)          (((x) & 0xff) << 8)

//2 TST SIFS

#define _SIFS_T2T_CCK(x)          ((x) & 0xff)
#define _SIFS_T2T_OFDM(x)          (((x) & 0xff) << 8)


//2 TBTT PROHIBIT
#define _TBTT_PROHIBIT_HOLD(x)	(((x) & 0xFF) << 8)


//2 BCN_CTRL
#define _BCN_TXBCN_RPT			BIT(2)
#define _BCN_FUNC_ENABLE		BIT(3)


//4 8051FWDL
#define FW8051DWL_EN			BIT(0)
#define FW8051DWL_RDY			BIT(1)

// FW download range
#define FW_SIZE					0xc000
#define FW_START_ADDRESS		0x1000
#define FW_END_ADDRESS		0x3FFF
#define MIN_BLOCK_SIZE			8
#define FW_BLOCK_SIZE			64
#define FW_PAGE_SIZE			0x1000
#define FW_BLOCKNUM_ONE_PAGE	(FW_PAGE_SIZE/FW_BLOCK_SIZE)	//be careful, it is neccessary  to be divided with no remainder 
#define FW_WRITE_UNIT 			8
#define FW_DOWNLOAD_BEACON_MODE 1

// Add by Guo.Mingzhi 2009-07-30
#define DUAL_MAC_EN             0x01
#define DUAL_PHY_EN             0x02
#define SUPER_MAC_EN            0x04

#define TXPKTBUF_BLOCK_SIZE		8


//2 8051FWDL
#define FW8051DWL_EN			BIT(0)
#define FW8051DWL_RDY			BIT(1)

#define ROM_DLEN				BIT(3)

//4 8051FWDL
#define FW8051DWL_EN			BIT(0)
#define FW8051DWL_RDY			BIT(1)

// FW download range
#define TESTCHIP_FW_SIZE		0x3000
#define NORMALCHIP_FW_SIZE	0x4000
#define FW_START_ADDRESS		0x1000
#define FW_END_ADDRESS		0x3FFF
#define FW_BLOCK_SIZE			64
#define FW_PAGE_SIZE			0x1000
#define FW_BLOCKNUM_ONE_PAGE	(FW_PAGE_SIZE/FW_BLOCK_SIZE)	//be careful, it is neccessary  to be divided with no remainder 


// Add by Guo.Mingzhi 2009-07-30
#define DUAL_MAC_EN             0x01
#define DUAL_PHY_EN             0x02
#define SUPER_MAC_EN            0x04


//RPWM
#define 	PS_DPS				BIT(0)
#define 	PS_LCLK				(PS_DPS)
#define	PS_RF_OFF			BIT(1)
#define	PS_ALL_ON			BIT(2)
#define	PS_ST_ACTIVE		BIT(3)
#define	PS_LP				BIT(4)	// low performance
#define	PS_ACK				BIT(6)
#define	PS_TOGGLE			BIT(7)
#endif
