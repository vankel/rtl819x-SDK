#ifndef _WL_8881_H_
#define _WL_8881_H_

#include <wl_com/HalComBit.h>
#include <wl_com/HalLBComBit.h>
#include <wl_com/HalComReg.h>
#include <wl_com/HalComTXDesc.h>
#include <wl_com/HalComRXDesc.h>
#include "wl_common_hw.h"


#define MAX_BUF_NUM             64
#define Rx_BUF_REAL_LEN         2048
#define RX_BUF_LEN              2048

#define WT_LITT_EDN             0x02000000
#define RD_LITT_EDN             0x01000000


#define TX_VIQ_DESC_NUM         4
#define TX_VOQ_DESC_NUM         4
#define TX_BKQ_DESC_NUM         4
#define TX_BEQ_DESC_NUM         4
#define TX_BCNQ_DESC_NUM        8
#define TX_MGQ_DESC_NUM         4
#define TX_H0Q_DESC_NUM         4
#define TX_H1Q_DESC_NUM         4
#define TX_H2Q_DESC_NUM         4
#define TX_H3Q_DESC_NUM         4
#define TX_H4Q_DESC_NUM         4
#define TX_H5Q_DESC_NUM         4
#define TX_H6Q_DESC_NUM         4
#define TX_H7Q_DESC_NUM         4
#define RX_Q_DESC_NUM           32


#define SET_VIQ_DES_NUM     (TX_VIQ_DESC_NUM<<16)
#define SET_VOQ_DES_NUM     (TX_VOQ_DESC_NUM)
#define SET_RXQ_DES_NUM     (RX_Q_DESC_NUM<<16)
#define SET_MGQ_DES_NUM     (TX_MGQ_DESC_NUM)
#define SET_BKQ_DES_NUM     (TX_BKQ_DESC_NUM<<16)
#define SET_BEQ_DES_NUM     (TX_BEQ_DESC_NUM)
#define SET_H1Q_DES_NUM     (TX_H1Q_DESC_NUM<<16)
#define SET_H0Q_DES_NUM     (TX_H0Q_DESC_NUM)
#define SET_H3Q_DES_NUM     (TX_H3Q_DESC_NUM<<16)
#define SET_H2Q_DES_NUM     (TX_H2Q_DESC_NUM)
#define SET_H5Q_DES_NUM     (TX_H5Q_DESC_NUM<<16)
#define SET_H4Q_DES_NUM     (TX_H4Q_DESC_NUM)
#define SET_H7Q_DES_NUM     (TX_H7Q_DESC_NUM<<16)
#define SET_H6Q_DES_NUM     (TX_H6Q_DESC_NUM)




#define OPTION0_SEQMENTS        0
#define OPTION1_SEQMENTS        1

#define OPTION0_SEQMENTS_NUM    2
#define OPTION1_SEQMENTS_NUM    4

//0: 2 segment 
//1: 4 segment 
//2: 8 segment 
//#define TX_DESC_MODE            2

#define MAX_TXBD_SEQMENT_NUM    ((TX_DESC_MODE)? (4*TX_DESC_MODE): 2)
#define MAX_EXTEND_TXBD_NUM     8


#define SEQMENT_NUM(Value)      ((Value) ? OPTION1_SEQMENTS_NUM : OPTION0_SEQMENTS_NUM)


#ifdef _BIG_ENDIAN_
typedef struct _DESC_MODE_BIT_MAP_ {
    u4Byte     Rsvd31To21      :11;        //bit[31:21]
    u4Byte     MgqMode         :1;         //bit[20]
    u4Byte     BkqMode         :1;         //bit[19]
    u4Byte     BeqMode         :1;         //bit[18]
    u4Byte     ViqMode         :1;         //bit[17]
    u4Byte     VoqMode         :1;         //bit[16]
    u4Byte     Rsvd15To08      :8;         //bit[15:8]    
    u4Byte     M7qMode         :1;         //bit[7]
    u4Byte     M6qMode         :1;         //bit[6]
    u4Byte     M5qMode         :1;         //bit[5]
    u4Byte     M4qMode         :1;         //bit[4]
    u4Byte     M3qMode         :1;         //bit[3]
    u4Byte     M2qMode         :1;         //bit[2]
    u4Byte     M1qMode         :1;         //bit[1]
    u4Byte     M0qMode         :1;         //bit[0]
}DESC_MODE_BIT_MAP, *PDESC_MODE_BIT_MAP;
#else
typedef struct _DESC_MODE_BIT_MAP_ {
    u4Byte     M0qMode         :1;         //bit[0]
    u4Byte     M1qMode         :1;         //bit[1]
    u4Byte     M2qMode         :1;         //bit[2]
    u4Byte     M3qMode         :1;         //bit[3]
    u4Byte     M4qMode         :1;         //bit[4]
    u4Byte     M5qMode         :1;         //bit[5]
    u4Byte     M6qMode         :1;         //bit[6]
    u4Byte     M7qMode         :1;         //bit[7]
    u4Byte     Rsvd15To08      :8;         //bit[15:8]    
    u4Byte     VoqMode         :1;         //bit[16]
    u4Byte     ViqMode         :1;         //bit[17]
    u4Byte     BeqMode         :1;         //bit[18]
    u4Byte     BkqMode         :1;         //bit[19]
    u4Byte     MgqMode         :1;         //bit[20]
    u4Byte     Rsvd31To21      :11;        //bit[31:21]
}DESC_MODE_BIT_MAP, *PDESC_MODE_BIT_MAP;
#endif

typedef union _DESC_MODE_{
    DESC_MODE_BIT_MAP       Bitmap;
    u4Byte                  Value32;
}DESC_MODE, *PDESC_MODE;


typedef struct _RXBD_ELEMENT_DW_ {
    u4Byte         Dword0;
    u4Byte         PhyAddr;
}RXBD_ELEMENT_DW,*PRXBD_ELEMENT_DW;


typedef struct _TXBD_ELEMENT_ {
    u4Byte         Dword0;
    u4Byte         AddrLow;
}TXBD_ELEMENT,*PTXBD_ELEMENT;

typedef struct _TXBD_ELEMENT_MANAGER_ {
    TXBD_ELEMENT     TXBD_ELE[MAX_TXBD_SEQMENT_NUM];
}TXBD_ELEMENT_MANAGER,*PTXBD_ELEMENT_MANAGER;

typedef struct _BCNTXBD_ELEMENT_MANAGER_ {
    TXBD_ELEMENT     TXBD_ELE[8];
}BCNTXBD_ELEMENT_MANAGER,*PBCNTXBD_ELEMENT_MANAGER;


typedef struct _EXTEND_TXBD_ELEMENT_MANAGER_ {
    TXBD_ELEMENT     TXBD_ELE[MAX_EXTEND_TXBD_NUM];
}EXTEND_TXBD_ELEMENT_MANAGER,*PEXTEND_TXBD_ELEMENT_MANAGER;


typedef struct _HCI_DMA_MANAGER_ {
    TXBD_ELEMENT_MANAGER ViqTXBD[TX_VIQ_DESC_NUM];
    TXBD_ELEMENT_MANAGER VoqTXBD[TX_VOQ_DESC_NUM];
    TXBD_ELEMENT_MANAGER BeqTXBD[TX_BEQ_DESC_NUM];
    TXBD_ELEMENT_MANAGER BkqTXBD[TX_BKQ_DESC_NUM];
    BCNTXBD_ELEMENT_MANAGER BcnqTXBD[TX_BCNQ_DESC_NUM];    
    TXBD_ELEMENT_MANAGER MgqTXBD[TX_MGQ_DESC_NUM];
    TXBD_ELEMENT_MANAGER H0qTXBD[TX_H0Q_DESC_NUM];
    TXBD_ELEMENT_MANAGER H1qTXBD[TX_H1Q_DESC_NUM];
    TXBD_ELEMENT_MANAGER H2qTXBD[TX_H2Q_DESC_NUM];
    TXBD_ELEMENT_MANAGER H3qTXBD[TX_H3Q_DESC_NUM];
    TXBD_ELEMENT_MANAGER H4qTXBD[TX_H4Q_DESC_NUM];
    TXBD_ELEMENT_MANAGER H5qTXBD[TX_H5Q_DESC_NUM];
    TXBD_ELEMENT_MANAGER H6qTXBD[TX_H6Q_DESC_NUM];    
    TXBD_ELEMENT_MANAGER H7qTXBD[TX_H7Q_DESC_NUM];
    EXTEND_TXBD_ELEMENT_MANAGER ExViqTXBD[TX_VIQ_DESC_NUM][MAX_TXBD_SEQMENT_NUM];
    EXTEND_TXBD_ELEMENT_MANAGER ExVoqTXBD[TX_VOQ_DESC_NUM][MAX_TXBD_SEQMENT_NUM];
    EXTEND_TXBD_ELEMENT_MANAGER ExBeqTXBD[TX_BEQ_DESC_NUM][MAX_TXBD_SEQMENT_NUM];
    EXTEND_TXBD_ELEMENT_MANAGER ExBkqTXBD[TX_BKQ_DESC_NUM][MAX_TXBD_SEQMENT_NUM];
    EXTEND_TXBD_ELEMENT_MANAGER ExMgqTXBD[TX_BKQ_DESC_NUM][MAX_TXBD_SEQMENT_NUM];
    RXBD_ELEMENT_DW         RXBD[RX_Q_DESC_NUM];
    u4Byte                  RxAggBufEntry[RX_Q_DESC_NUM];
    u4Byte                  RxAggLenEntry[RX_Q_DESC_NUM];
    u4Byte                  RxLen;
    u4Byte                  RemainLen;
    u2Byte                  ViqTxWritePoint;
    u2Byte                  ViqTxReadPoint;
    u2Byte                  VoqTxWritePoint;
    u2Byte                  VoqTxReadPoint;
    u2Byte                  BeqTxWritePoint;
    u2Byte                  BeqTxReadPoint;
    u2Byte                  BkqTxWritePoint;
    u2Byte                  BkqTxReadPoint;
    u2Byte                  RxWritePoint;
    u2Byte                  RxReadPoint;
    u2Byte                  RxAggregateNum;
    u2Byte                  RxExpectTag;
    u1Byte                  RxSegFlow;
    u1Byte                  Flagls;
}HCI_DMA_MANAGER, *PHCI_DMA_MANAGER;

typedef struct _RX_BUF_MAN_ {
    u4Byte             IsUsed;
    u1Byte              RxBuf[Rx_BUF_REAL_LEN];
}RX_BUF_MAN;


u4Byte 
InitMACHAL8881A (
    VOID
);

u4Byte 
InitPowerHAL8881A (
    VOID
);

u4Byte 
InitMACToAPHAL8881A (
    VOID
);

u4Byte 
TXDescGenHAL8881A (
    IN  u4Byte PayloadLen, 
    IN  u4Byte Queue, 
    IN  u1Byte  IsBcn,
    IN  u1Byte  IsDlForFw,
    IN  PORT_INFO *PortInfo,
    IN  TX_DESC *curDesc, 
    IN  TX_EXTEND_DESC  *curExtendDesc 
);

VOID 
InitHCIDmaHAL8881A (
    VOID
);

//VOID LinkBDToRing(VOID);
u4Byte 
InitCfgNormallHAL8881A (
    VOID
);

u4Byte 
InitCfgSwBcnHAL8881A (
    VOID
);

u4Byte 
InitCfgDmaLBHAL8881A (
    VOID
);

VOID
InitMBSSID8881A(
    IN  u1Byte  IsRootInterface,
    IN  u1Byte  IsVapInterface,
    IN  u1Byte  vap_init_seq
);

VOID
StopMBSSID8881A(
    IN  u1Byte  IsRootInterface,
    IN  u1Byte  IsVapInterface,
    IN  u1Byte  vap_init_seq
);

#endif //#ifndef _WL_8881_H_

