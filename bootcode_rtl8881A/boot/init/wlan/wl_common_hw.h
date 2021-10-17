#ifndef _WL_COMMON_HW_H_
#define _WL_COMMON_HW_H_
#include <wl_com/HalComTXDesc.h>
#include "wl_type.h"



#define VoQsel                          TXDESC_QSEL_TID6
#define ViQsel                          TXDESC_QSEL_TID4
#define BeQsel                          TXDESC_QSEL_TID0
#define BkQsel                          TXDESC_QSEL_TID1
#define BcnQsel                         TXDESC_QSEL_BCN
#define MagQsel                         TXDESC_QSEL_MGT
#define HighQsel                        TXDESC_QSEL_HIGH
#define BCN_PAGE_NUM                    0x90

#define HAL_NUM_VWLAN                   0

//3 //==================================
//3  -          RTL8881A IO Base Address          -
#define WLBASE_ADDR                     0xB8640000
#define TXPKT_ADDR                      0xB8650000
#define RXPKT_ADDR                      0xB8660000
//3 //===================================

#define WL_IO_WRITE32(offset, value)        (*((volatile u4Byte*)(WLBASE_ADDR + offset)) = (value))
#define WL_IO_READ32(offset)                (*((volatile u4Byte*)(WLBASE_ADDR + offset)))
#define WL_IO_WRITE16(offset, value)        (*((volatile u2Byte*)(WLBASE_ADDR + offset)) = (value))
#define WL_IO_READ16(offset)                (*((volatile u2Byte*)(WLBASE_ADDR + offset)))
#define WL_IO_WRITE8(offset, value)        (*((volatile u1Byte*)(WLBASE_ADDR + offset)) = (value))
#define WL_IO_READ8(offset)                (*((volatile u1Byte*)(WLBASE_ADDR + offset)))


#define TXBUF_IO_WRITE32(offset, value)        (*((volatile u4Byte*)(TXPKT_ADDR + offset)) = (value))
#define TXBUF_IO_READ32(offset)                (*((volatile u4Byte*)(TXPKT_ADDR + offset)))
#define TXBUF_IO_WRITE16(offset, value)        (*((volatile u2Byte*)(TXPKT_ADDR + offset)) = (value))
#define TXBUF_IO_READ16(offset)                (*((volatile u2Byte*)(TXPKT_ADDR + offset)))
#define TXBUF_IO_WRITE8(offset, value)        (*((volatile u1Byte*)(TXPKT_ADDR + offset)) = (value))
#define TXBUF_IO_READ8(offset)                (*((volatile u1Byte*)(TXPKT_ADDR + offset)))


#define RXBUF_IO_WRITE32(offset, value)        (*((volatile u4Byte*)(RXPKT_ADDR + offset)) = (value))
#define RXBUF_IO_READ32(offset)                (*((volatile u4Byte*)(RXPKT_ADDR + offset)))
#define RXBUF_IO_WRITE16(offset, value)        (*((volatile u2Byte*)(RXPKT_ADDR + offset)) = (value))
#define RXBUF_IO_READ16(offset)                (*((volatile u2Byte*)(RXPKT_ADDR + offset)))
#define RXBUF_IO_WRITE8(offset, value)        (*((volatile u1Byte*)(RXPKT_ADDR + offset)) = (value))
#define RXBUF_IO_READ8(offset)                (*((volatile u1Byte*)(RXPKT_ADDR + offset)))

#define TOPHY_ADDRESS(Value32)  (Value32<<3)>>3


//General IO Read/Write
#define GEN_RTL_W32(offset, value)        (*((volatile u4Byte*)(offset)) = (value))
#define GEN_RTL_R32(offset)                (*((volatile u4Byte*)(offset)))
#define GEN_RTL_W16(offset, value)        (*((volatile u2Byte*)(offset)) = (value))
#define GEN_RTL_R16(offset)                (*((volatile u2Byte*)(offset)))
#define GEN_RTL_W8(offset, value)        (*((volatile u1Byte*)(offset)) = (value))
#define GEN_RTL_R8(offset)                (*((volatile u1Byte*)(offset)))



//3 Mapping IO
//#define HAL_PADAPTER    VOID *//PRTL8192CD_PRIV

#define HAL_RTL_R8(reg)		\
    (WL_IO_READ8(reg))

#define HAL_RTL_R16(reg)	\
    (WL_IO_READ16(reg))

#define HAL_RTL_R32(reg)	\
    (WL_IO_READ32(reg))

#define HAL_RTL_W8(reg, val8)	\
    do { \
        (WL_IO_WRITE8(reg, val8)); \
    } while (0)

#define HAL_RTL_W16(reg, val16)	\
    do { \
        WL_IO_WRITE16(reg, val16); \
    } while (0)

#define HAL_RTL_W32(reg, val32)	\
    do { \
        WL_IO_WRITE32(reg, val32) ; \
    } while (0)

#define     RTL_R8(addr)                WL_IO_READ8(addr)
#define     RTL_R16(addr)               WL_IO_READ16(addr)
#define     RTL_R32(addr)               WL_IO_READ32(addr)
#define     RTL_W8(offset, value)       WL_IO_WRITE8(offset, value)
#define     RTL_W16(offset, value)      WL_IO_WRITE16(offset, value)
#define     RTL_W32(offset, value)      WL_IO_WRITE32(offset, value)



//2 LLT_INIT
#define LLT_NO_ACTIVE                  0x0
#define LLT_WRITE_ACCESS               0x1
#define LLT_READ_ACCESS                0x2

#define LLT_INIT_DATA(x)               ((x) & 0xFF)
#define LLT_INIT_ADDR(x)               (((x) & 0xFF) << 8)
#define LLT_OP(x)                      (((x) & 0x3) << 30)
#define LLT_OP_VALUE(x)                (((x) >> 30) & 0x3)

#define LLT_POLLING_THRESHOLD           20

enum{
	NTWOMAC_PAGE_NUM_HPQ			= 0x00,
	NTWOMAC_PAGE_NUM_NPQ			= 0x00,
	NTWOMAC_PAGE_NUM_LPQ			= 0x00,
	NTWOMAC_PAGE_NUM_PUBQ			= 0xF9,		
	TX_PAGE_BOUNDARY_NORMAL_MODE	= 252,//KaiYuan modified for 88e		
};


enum{
	NLLB_PAGE_NUM_HPQ				= 0x10,
	NLLB_PAGE_NUM_NPQ				= 0x10,
	NLLB_PAGE_NUM_LPQ				= 0x10,
	NLLB_PAGE_NUM_PUBQ				= 0x10,	
	TX_PAGE_BOUNDARY_LOOPBACK_MODE	= 0x50,//KaiYuan modified for 88e	
};


#ifdef CONFIG_TRX_DESC_SWAP_SUPPORT
#define     CPUTOLEX32(Value32)       Value32
#define     CPUTOLEX16(Value16)       Value16
#else
#define     CPUTOLEX32(Value32)      (((Value32&0xFF)<<24) | ((Value32&0xFF00)<<8) |\
                                     ((Value32&0xFF0000)>>8) | ((Value32&0xFF000000)>>24))
#define     CPUTOLEX16(Value16)      ((Value16&0xFF00)>>8) | ((Value16&0xFF)<<8)                                    
#endif

#define     CPUTOLEX32_RX(Value32)      (((Value32&0xFF)<<24) | ((Value32&0xFF00)<<8) |\
                                     ((Value32&0xFF0000)>>8) | ((Value32&0xFF000000)>>24))



#define     SET_DESC(Dw, Value32, Mask, Shift)   \
                                     ((Dw & (~(Mask<<Shift)))|((Value32&Mask)<<Shift));

#define     GET_DESC(Dw, Mask, Shift)((CPUTOLEX32_RX(Dw)>>Shift) & Mask)


#define     WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define     READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
#define     WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define     READ_MEM16(addr)         (*(volatile unsigned short *) (addr))
#define     WRITE_MEM8(addr, val)    (*(volatile unsigned char *) (addr)) = (val)
#define     READ_MEM8(addr)          (*(volatile unsigned char *) (addr))


#define delay_us(us) delay_ms(us)


#define RXDESCLEN       24


#define MAXPORTNUM		1
#define MAXDESTNUM		8
#define CAM_DATA_SIZE	256


// Without address 4
#define WLAN_HEADER_LENGTH			24
#define WLAN_HEADER_WITH_QOS_SIZE	26
#define WLAN_HEADER_WITH_ENCRY_SIZE 28    

//=========security IV length,ICV length,EIV length, MIC length=================
#define FCSLen 4
#define IVLen 4
#define ICVLen 4
#define EIVLen 4
#define MICLen 8
#define CCMPheaderLen 8
#define WAPIheaderLen 18
#define WAPIMicLen    16



//========= ODM Config     =======================================
//for PutRegsetting & GetRegSetting BitMask
#define		bMaskByte0                  0xff	// Reg 0xc50 rOFDM0_XAAGCCore~0xC6f
#define		bMaskByte1                  0xff00
#define		bMaskByte2                  0xff0000
#define		bMaskByte3                  0xff000000
#define		bMaskHWord                  0xffff0000
#define		bMaskLWord                  0x0000ffff
#define		bMaskDWord                  0xffffffff
#define		bMaskH4Bits                 0xf0000000
#define		bMaskH3Bytes                0xffffff00	
#define 	bMaskOFDM_D                 0xffc00000
#define		bMaskCCK                    0x3f3f3f3f


//==========================================================

//========= RF Config     =======================================
#define     RF_CHNLBW_Jaguar                    0x18    // RF channel and BW switch
// BW and sideband setting
#define     bCCK_System_Jaguar                  0x10
#define     HAL_PRIME_CHNL_OFFSET_DONT_CARE     0
#define     HAL_PRIME_CHNL_OFFSET_LOWER         1
#define     HAL_PRIME_CHNL_OFFSET_UPPER         2
// Block & Path enable
#define     bOFDMEN_Jaguar                      0x20000000
#define     bCCKEN_Jaguar                       0x10000000





//==========================================================

/*Define the TX LINKLIST ID*/
typedef enum _TX_LINKLIST_ID{
    TX_LINKLIST_VO,
    TX_LINKLIST_VI,
    TX_LINKLIST_BE,
    TX_LINKLIST_BK,
    TX_LINKLIST_MAG,
    TX_LINKLIST_H0,
    TX_LINKLIST_H1,
    TX_LINKLIST_H2,
    TX_LINKLIST_H3,
    TX_LINKLIST_H4,
    TX_LINKLIST_H5,
    TX_LINKLIST_H6,
    TX_LINKLIST_H7,
    TX_LINKLIST_MAX 
    
} TX_LINKLIST_ID;


typedef enum _Port_id{
    Port_1,
    Port_2
}Port_id;



typedef enum _LB_MODE{
	LB_Disable,
	MAC_LB,
	MAC_Delay_LB,
	BB_LB,
	DMA_LB
}LB_MODE, *PLB_MODE;

/* address setting*/
typedef enum _CAST_TYPE{
    NONE_CAST,
    BROADCAST,
    MULTICAST,
    UNICAST
}CAST_TYPE;


typedef struct _ADDRESS_SETTING{
    CAST_TYPE               cast_type;
    u1Byte                  mac_addr[6];    
    u1Byte                  src_addr[6];
    u1Byte                  des_addr[6];
    u1Byte                  bssid[6];

}ADDRESS_SETTING,*PADDRESS_SETTING;

/* security setting*/

typedef enum _SECURITY_TYPE{
    TYPE_NONE,
    WEP40,
    WEP104,
    TKIP,
    AES,
    WAPI
}SECURITY_TYPE;

typedef enum _EN_DECRYPTION{
    SW,
    HW
}EN_DECRYPTION;

typedef enum _KEY_ID{
    KEY0,
    KEY1,
    KEY2,
    KEY3
}KEY_ID;

typedef struct _SECURITY_SETTING{
    
    SECURITY_TYPE       type;
    EN_DECRYPTION       encryption;
    EN_DECRYPTION       decryption;
    KEY_ID              keyid;
    
}SECURITY_SETTING,*PSECURITY_SETTING;

typedef struct _PORT_INFO{
    
    ADDRESS_SETTING     address_info;
    SECURITY_SETTING    encryption_info;
    BOOLEAN             use_connect;
    u4Byte              TIMIeOffset;
    
}PORT_INFO, *PPORT_INFO;


/*Aggregation Parameter*/
typedef	struct	_AGG_PARAMETER{
	u1Byte      *aggShiftPtr;
	u1Byte	    *aggOrgBuffer;
	u4Byte	    totalLength;
	u4Byte	    descCount;
	u4Byte      totalCount;
}AGG_PARAMETER, *PAGG_PARAMETER;


typedef struct _DATA_PATH_VARI{

/*Information for the current generate Packet*/
    /*TX Aggregation Parameter*/
    AGG_PARAMETER   Agg_Para[13];//vo, vi, be, bk, Mag, High

    /*Information for the current generate Packet*/
        /*Size*/
    u4Byte   pktSize_VO;
    u4Byte   pktSize_VI;
    u4Byte   pktSize_BE;
    u4Byte   pktSize_BK;
    u4Byte   pktSize_MG;
    u4Byte   pktSize_H0;
    u4Byte   pktSize_H1;
    u4Byte   pktSize_H2;
    u4Byte   pktSize_H3;
    u4Byte   pktSize_H4;
    u4Byte   pktSize_H5;
    u4Byte   pktSize_H6;
    u4Byte   pktSize_H7;


	/*Indicate if the current pkt is the last fragment of the payload*/
	BOOLEAN    bLastFrag_VO;
	BOOLEAN    bLastFrag_VI;
	BOOLEAN    bLastFrag_BE;
	BOOLEAN    bLastFrag_BK;
	BOOLEAN    bLastFrag_MG;
	BOOLEAN    bLastFrag_H0;
	BOOLEAN    bLastFrag_H1;
	BOOLEAN    bLastFrag_H2;
	BOOLEAN    bLastFrag_H3;
	BOOLEAN    bLastFrag_H4;
	BOOLEAN    bLastFrag_H5;
	BOOLEAN    bLastFrag_H6;
	BOOLEAN    bLastFrag_H7;
	/*Indicate if the current pkt is the first fragment of the payload*/
	BOOLEAN    bFirstFrag_VO; 
	BOOLEAN    bFirstFrag_VI;
	BOOLEAN    bFirstFrag_BE;
	BOOLEAN    bFirstFrag_BK;
	BOOLEAN    bFirstFrag_MG;
	BOOLEAN    bFirstFrag_H0;
	BOOLEAN    bFirstFrag_H1;
	BOOLEAN    bFirstFrag_H2;
	BOOLEAN    bFirstFrag_H3;
	BOOLEAN    bFirstFrag_H4;
	BOOLEAN    bFirstFrag_H5;
	BOOLEAN    bFirstFrag_H6;
	BOOLEAN    bFirstFrag_H7;
	/*Indicate if the current pkt is the one fragment of the payload*/
	u1Byte	    nFrag_VO;
	u1Byte	    nFrag_VI;
	u1Byte	    nFrag_BE;
	u1Byte	    nFrag_BK;	
	u1Byte	    nFrag_MG;	
	u1Byte	    nFrag_H0;
	u1Byte	    nFrag_H1;
	u1Byte	    nFrag_H2;
	u1Byte	    nFrag_H3;
	u1Byte	    nFrag_H4;
	u1Byte	    nFrag_H5;
	u1Byte	    nFrag_H6;
	u1Byte	    nFrag_H7;

	/*Sequence number*/
	u4Byte	    SeqNum[13];//vo, vi, be, bk, Mag, High0~High7
}DATA_PATH_VARI, *PDATA_PATH_VARI;

//========== PACKET_SETTING_UI ==================================
typedef enum _DATA_OPTION{
    FIXED,
    RANDOM,
    INCREASING
}DATA_OPTION;

typedef enum _DATA_RATE{
    RATE_1,
    RATE_2,
    RATE_5_5,
    RATE_11,
    RATE_6,
    RATE_9,
    RATE_12,
    RATE_18,
    RATE_24,
    RATE_36,
    RATE_48,
    RATE_54,
    RATE_MCS0,
    RATE_MCS1,
    RATE_MCS2,
    RATE_MCS3,
    RATE_MCS4,
    RATE_MCS5,
    RATE_MCS6,
    RATE_MCS7,
    RATE_MCS8,
    RATE_MCS9,
    RATE_MCS10,
    RATE_MCS11,
    RATE_MCS12,
    RATE_MCS13,
    RATE_MCS14,
    RATE_MCS15

}DATA_RATE;

typedef enum _DATA_LENGTH{
    LENGTH_256,
    LENGTH_512,
    LENGTH_1343
}DATA_LENGTH;

typedef enum _DATA_PAYLOAD{
    PAYLOAD_00,
    PAYLOAD_55,
    PAYLOAD_AA,
    PAYLOAD_FF,
    PAYLOAD_3E
}DATA_PAYLOAD;

typedef enum _DMA_QUEUE{
    QUEUE_LOW =1,
    QUEUE_NORMAL,
    QUEUE_HIGH
}DMA_QUEUE;

//==========================================================



//========== SOURCE_DATA_SETTING_UI==============================
typedef struct _PACKET_SETTING_UI{
    BOOLEAN                traffic_support;//to show if this traffic category is supported
    DATA_OPTION         rateopt;    //High Throughput
    DATA_RATE           rate;   //High Throughput
    DATA_OPTION         lengthopt;
    DATA_LENGTH         length;
    DATA_OPTION         payloadopt;
    DATA_PAYLOAD        payload;
    DMA_QUEUE           queue;
}PACKET_SETTING_UI, *PPACKET_SETTING_UI;

//==========================================================


//========== TEST_OPTION_UI=====================================
typedef enum _irq_num{
	num_0,
	num_1,
	num_2,
	num_3,
	num_4,
	num_5,
	num_6,
	num_7,
	num_8,
	num_9,
	num_10,
	num_11,
	num_12,
	num_13,
	num_14,
	num_15
}irq_num;

typedef enum _LINK_TYPE{   //KaiYuan: should be moved to upper layer
    NONE_LINK_TYPE,
    STA_NONE,
    NONE_STA,
    AP_NONE,
    Ad_hoc_NONE,
    AP_STA,
    STA_STA,
    TWOMAC_AP_STA,//MultiDestination
    TWOMAC_STA_STA  
    
}LINK_TYPE;

typedef enum _RF_PATH{
	path_A,
	path_B
}RF_PATH;


//==========================================================


//========== TX_DISCRIPTOR_UI=====================================
/* RxAggMode*/
typedef enum _RX_AGG_MODE{
	RX_AGG_NO_AGG, 			//un-aggregated packets. Do nothing
	RX_AGG_DMA_ONLY, 	//HW RX DMA aggregate
	RX_AGG_DMA_USB, 		//HW RX DMA aggregate with USB aggregate
	RX_AGG_USB_ONLY		//Usb aggregate only
}RX_AGG_MODE;

typedef enum _MAX_LEN_TX_DISC{
	len_8k,
	len_16k
}MAX_LEN_TX_DISC;


//==========================================================


//========== CHECKSUM_OFFLOAD_UI================================
/* IP Version*/
typedef enum _ip_ver{
	ip_ver_none,
	ip_ver_v4,
	ip_ver_v6
}ip_ver;

/* Protocol Option*/
typedef enum _protocol_opt{
	protocol_tcp,
	protocol_udp,
	protocol_other
}protocol_opt;

/* Check Sum Offload TRX Option*/
typedef enum _checksum_trx_opt{
	trx_opt_sw,
	trx_opt_hw
}checksum_trx_opt;

//==========================================================


//========== SOUNDING_PTCL_UI===================================
/* Roll Selection */
typedef enum _roll_selection{
	roll_Beamformer,
	roll_Beamformee
}roll_selection;

/* Mode Selection */
typedef enum _mode_selection{
	mode_VHT,
	mode_HT
}mode_selection;

/* Sounding PTCL BandWidth*/
typedef enum _sounding_ptcl_bw{
	bw_20M,
	bw_40M,
	bw_80M
}sounding_ptcl_bw;

/* Sounding PTCL rate*/
typedef enum _sounding_ptcl_rate{
	sounding_rate_6,
	sounding_rate_9,
	sounding_rate_12,
	sounding_rate_18,
	sounding_rate_24,
	sounding_rate_36,
	sounding_rate_48,
	sounding_rate_54
}sounding_ptcl_rate;

//==========================================================


//========== MAC_SETTING_GEN===================================

typedef struct _SOURCE_DATA_SETTING_UI{
	BOOLEAN       				ht_support;	//High Throughput
	BOOLEAN    	   			qos_support;
	BOOLEAN       				small_pkt_size;
    BOOLEAN                    IsBcn;
    BOOLEAN                    IsDlForFw;
	u4Byte     				fix_pkt_len;	
	PACKET_SETTING_UI		packet[13]; //vo, vi, be, bk, mag
	
}SOURCE_DATA_SETTING_UI;

typedef struct _TEST_OPTION_UI{
	LB_MODE                 lb_mode;
	irq_num                 rx_irq_num;
	LINK_TYPE               link_type;
	RF_PATH                 rf_path;
	BOOLEAN                 nonstop_tx;
	BOOLEAN                 continuous_rx;
	BOOLEAN                 rx_seq_chk;
	BOOLEAN                 rx_crc_chk;
	BOOLEAN                 tx_multi_thread;
	BOOLEAN                 multi_dest;
	BOOLEAN                 magic_pkt;
	BOOLEAN                 patten_match_pkt;	
}TEST_OPTION_UI, *PTEST_OPTION_UI;

typedef struct _TX_DISCRIPTOR_UI{
	BOOLEAN       		tx_agg_support; // 4 5
	BOOLEAN       		rdg_enable; //4 7
	BOOLEAN       		ampdu_support; 	
	BOOLEAN       		amsdu_support; // 4 13
	BOOLEAN       		early_mode_support;//4 30??
	u4Byte              ampdu_density;//8 22
	RX_AGG_MODE         rx_agg_mode; //	
	BOOLEAN             cts2self_support; //16 11
	BOOLEAN       		rts_support;	//16 12
	BOOLEAN       		data_short; //16 24
	BOOLEAN       		data_bw; //16 25
	BOOLEAN       		rts_short; //16 26
	BOOLEAN       		rts_bw; //16  27
	BOOLEAN     		short_gi; //20 6
	BOOLEAN       		retry_enable; //20 17
	u4Byte		        retry_limit; //20 23
	u4Byte         	    agg_num; // 20 31
	BOOLEAN       		use_max_len; //24 10
	u4Byte		        max_agg_num; //24 15
	MAX_LEN_TX_DISC     max_length; //28 31
	BOOLEAN       		tx_desc_check_sum;	
	BOOLEAN 		    msdu_frag;
	BOOLEAN       		tri_frame;
	BOOLEAN       		trying;
	BOOLEAN       		ccx;
	BOOLEAN       		tag1;
	BOOLEAN       		early_mode_extend;	
	u4Byte		        swoffset30;
	u4Byte		        swoffset31;
}TX_DISCRIPTOR_UI, *PTX_DISCRIPTOR_UI;


typedef struct _CHECKSUM_OFFLOAD_UI{
	BOOLEAN				checksum_offload_enable;
	ip_ver			    ip_version;
	protocol_opt		protocol;
	checksum_trx_opt	tx_opt;
	checksum_trx_opt	rx_opt;

}CHECKSUM_OFFLOAD_UI, *PCHECKSUM_OFFLOAD_UI;


typedef struct _SOUNDING_PTCL_UI{
	BOOLEAN				sounding_ptcl_enable;
	roll_selection		roll;
	mode_selection	    mode;
	sounding_ptcl_bw	bw;
	sounding_ptcl_rate	rate;
}SOUNDING_PTCL_UI, *PSOUNDING_PTCL_UI;
//==========================================================


typedef struct _MAC_SETTING_GEN{
	SOURCE_DATA_SETTING_UI      source_data;
	TEST_OPTION_UI              test_option;
	TX_DISCRIPTOR_UI            tx_discriptor;
	CHECKSUM_OFFLOAD_UI         checksum_offload;
	SOUNDING_PTCL_UI            sounding_ptcl;
}MAC_SETTING_GEN,*PMAC_SETTING_GEN;


typedef struct _MAC_SETTING_PORT{
	PORT_INFO 	Port[MAXPORTNUM][MAXDESTNUM];//Port1 and Port2
	BOOLEAN     ForceKey_En;
	u4Byte 		ForceKey_ID;
	u4Byte 		CAMData[CAM_DATA_SIZE];
	
}MAC_SETTING_PORT, *PMAC_SETTING_PORT;


//==============TX RX Desc =====================================
typedef struct _TX_DESC{
    u4Byte     Dword0;
    u4Byte     Dword1;
    u4Byte     Dword2;
    u4Byte     Dword3;
    u4Byte     Dword4;
    u4Byte     Dword5;
    u4Byte     Dword6;
    u4Byte     Dword7;
    u4Byte     Dword8;
    u4Byte     Dword9;
} TX_DESC, *PTX_DESC;

#define SIZE_TX_DESC_LEN    (sizeof(TX_DESC))

typedef struct _RX_DESC{
    u4Byte     Dword0;
    u4Byte     Dword1;
    u4Byte     Dword2;
    u4Byte     Dword3;
    u4Byte     Dword4;
    u4Byte     Dword5;
} RX_DESC, *PRX_DESC;

#define SIZE_RX_DESC_LEN   (sizeof(RX_DESC))

typedef struct _TX_EXTEND_DESC{
    u4Byte       Dword0;
    u4Byte       Dword1;    
   
}TX_EXTEND_DESC, *PTX_EXTEND_DESC;

typedef enum _RT_OP_MODE{
	RT_OP_MODE_AP,
	RT_OP_MODE_INFRASTRUCTURE,
	RT_OP_MODE_IBSS,
	RT_OP_MODE_NO_LINK,
}RT_OP_MODE, *PRT_OP_MODE;

enum wifi_state {
	WIFI_NULL_STATE		=	0x00000000,
	WIFI_ASOC_STATE		=	0x00000001,
	WIFI_REASOC_STATE	=	0x00000002,
	WIFI_SLEEP_STATE	=	0x00000004,
	WIFI_STATION_STATE	=	0x00000008,
	WIFI_AP_STATE		=	0x00000010,
	WIFI_ADHOC_STATE	=	0x00000020,
	WIFI_AUTH_NULL		=	0x00000100,
	WIFI_AUTH_STATE1	= 	0x00000200,
	WIFI_AUTH_SUCCESS	=	0x00000400,
	WIFI_SITE_MONITOR	=	0x00000800,
};

#if 0
typedef enum _rf_path{
	path_A,
	path_B
}rf_path;
#endif

//==========================================================

//========= RF Config     =======================================


/*--------------------------Define -------------------------------------------*/
/* BIT 7 HT Rate*/
// TxHT = 0
#define	MGN_1M				0x02
#define	MGN_2M				0x04
#define	MGN_5_5M			0x0b
#define	MGN_11M				0x16

#define	MGN_6M				0x0c
#define	MGN_9M				0x12
#define	MGN_12M				0x18
#define	MGN_18M				0x24
#define	MGN_24M				0x30
#define	MGN_36M				0x48
#define	MGN_48M				0x60
#define	MGN_54M				0x6c

// TxHT = 1
#define	MGN_MCS0			0x80
#define	MGN_MCS1			0x81
#define	MGN_MCS2			0x82
#define	MGN_MCS3			0x83
#define	MGN_MCS4			0x84
#define	MGN_MCS5			0x85
#define	MGN_MCS6			0x86
#define	MGN_MCS7			0x87
#define	MGN_MCS8			0x88
#define	MGN_MCS9			0x89
#define	MGN_MCS10			0x8a
#define	MGN_MCS11			0x8b
#define	MGN_MCS12			0x8c
#define	MGN_MCS13			0x8d
#define	MGN_MCS14			0x8e
#define	MGN_MCS15			0x8f
#define	MGN_VHT1SS_MCS0		0x90
#define	MGN_VHT1SS_MCS1		0x91
#define	MGN_VHT1SS_MCS2		0x92
#define	MGN_VHT1SS_MCS3		0x93
#define	MGN_VHT1SS_MCS4		0x94
#define	MGN_VHT1SS_MCS5		0x95
#define	MGN_VHT1SS_MCS6		0x96
#define	MGN_VHT1SS_MCS7		0x97
#define	MGN_VHT1SS_MCS8		0x98
#define	MGN_VHT1SS_MCS9		0x99
#define	MGN_VHT2SS_MCS0		0x9a
#define	MGN_VHT2SS_MCS1		0x9b
#define	MGN_VHT2SS_MCS2		0x9c
#define	MGN_VHT2SS_MCS3		0x9d
#define	MGN_VHT2SS_MCS4		0x9e
#define	MGN_VHT2SS_MCS5		0x9f
#define	MGN_VHT2SS_MCS6		0xa0
#define	MGN_VHT2SS_MCS7		0xa1
#define	MGN_VHT2SS_MCS8		0xa2
#define	MGN_VHT2SS_MCS9		0xa3

#define	MGN_MCS0_SG			0xc0
#define	MGN_MCS1_SG			0xc1
#define	MGN_MCS2_SG			0xc2
#define	MGN_MCS3_SG			0xc3
#define	MGN_MCS4_SG			0xc4
#define	MGN_MCS5_SG			0xc5
#define	MGN_MCS6_SG			0xc6
#define	MGN_MCS7_SG			0xc7
#define	MGN_MCS8_SG			0xc8
#define	MGN_MCS9_SG			0xc9
#define	MGN_MCS10_SG		0xca
#define	MGN_MCS11_SG		0xcb
#define	MGN_MCS12_SG		0xcc
#define	MGN_MCS13_SG		0xcd
#define	MGN_MCS14_SG		0xce
#define	MGN_MCS15_SG		0xcf



#define	RF_PATH_A		0		//Radio Path A
#define	RF_PATH_B		1		//Radio Path B
#define	RF_PATH_C		2		//Radio Path C
#define	RF_PATH_D		3		//Radio Path D



typedef enum _WIRELESS_MODE {
	WIRELESS_MODE_UNKNOWN = 0x00,
	WIRELESS_MODE_A = 0x01,
	WIRELESS_MODE_B = 0x02,
	WIRELESS_MODE_G = 0x04,
	WIRELESS_MODE_AUTO = 0x08,
	WIRELESS_MODE_N_24G = 0x10,
	WIRELESS_MODE_N_5G = 0x20,
	WIRELESS_MODE_AC_5G = 0x40
} WIRELESS_MODE;

typedef enum _BAND_TYPE{
	BAND_ON_2_4G = 0,
	BAND_ON_5G,
	BAND_ON_BOTH,
	BANDMAX
}BAND_TYPE,*PBAND_TYPE;


//
// Represent Channel Width in HT Capabilities
//
typedef enum _HT_CHANNEL_WIDTH{
	HT_CHANNEL_WIDTH_20 = 0,
	HT_CHANNEL_WIDTH_40 = 1,
	HT_CHANNEL_WIDTH_80 = 2,
	HT_CHANNEL_WIDTH_MAX = 3,
}HT_CHANNEL_WIDTH, *PHT_CHANNEL_WIDTH;



typedef enum _VHT_DATA_SC{
	VHT_DATA_SC_DONOT_CARE = 0,
	VHT_DATA_SC_20_UPPER_OF_80MHZ = 1,
	VHT_DATA_SC_20_LOWER_OF_80MHZ = 2,
	VHT_DATA_SC_20_UPPERST_OF_80MHZ = 3,
	VHT_DATA_SC_20_LOWEST_OF_80MHZ = 4,
	VHT_DATA_SC_20_RECV1 = 5,
	VHT_DATA_SC_20_RECV2 = 6,
	VHT_DATA_SC_20_RECV3 = 7,
	VHT_DATA_SC_20_RECV4 = 8,
	VHT_DATA_SC_40_UPPER_OF_80MHZ = 9,
	VHT_DATA_SC_40_LOWER_OF_80MHZ = 10,
}VHT_DATA_SC, *PVHT_DATA_SC_E;



//==========================================================


VOID 
InitLLTTable(
    IN  u4Byte     TxPktBufBndy, 
    IN  u4Byte     LastEntryTxPktBuf
);

VOID 
PHY_SetBBReg(
    IN  u4Byte     RegAddr, 
    IN  u4Byte     BitMask, 
    IN  u4Byte     Data
);

u4Byte 
PHY_QueryBBReg(
    IN  u4Byte     RegAddr, 
    IN  u4Byte     BitMask
);


VOID 
PHY_SetRFReg(
    IN  u4Byte     Path,   
    IN  u4Byte     RegAddr,
    IN  u4Byte     BitMask, 
    IN  u4Byte     Data
);

u4Byte 
PHY_QueryRFReg(
    IN  u4Byte     RfPath,
    IN  u4Byte     RegAddr, 
    IN  u4Byte     BitMask, 
    IN  u4Byte     dbg_avoid
);

VOID
phy_PostSetBwMode8812(
    IN  u1Byte      CurrentChannelBW,
    IN  u1Byte      nCur80MhzPrimeSC,
    IN  u1Byte      nCur40MhzPrimeSC
);

VOID
PHY_SwitchWirelessBand8812(
    IN  u4Byte     Band
);

VOID
phy_SwChnl8812(
    IN  u4Byte     CurrentWirelessMode,
    IN  u4Byte     CurrentChannel
);

VOID
PHY_SetTxPowerIndex_8812A(
    IN  u4Byte      PowerIndex,
    IN  u1Byte      RFPath,	
    IN  u1Byte      Rate
);


VOID
PHY_SetTxPowerLevel8812(
    IN  u1Byte      channel,
    IN	u4Byte 	    powerIndex
);



#endif //#ifndef _WL_COMMON_HW_H_
