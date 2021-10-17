#ifndef _WL_COMMAND_CTRL_H_
#define _WL_COMMAND_CTRL_H_

#include "wl_common_hw.h"
#include "wl_hw_init.h"
#include <asm/types.h>

/******************************************/
	/* Macro*/
/******************************************/
#define     LPoint                  0
#define     HPoint                  4095

/*Buffer Size*/
#define     MAX_RX_BUFFERS          16
#define     MAX_TX_BUFFERS          16
/*Sequence Fragment number */
#define     LEN_SF_WINDOW           32

#define     FW_NOERROR				0

#define     MAXTXARRAYELM           1024
#define     MAXRXARRAYELM           1024
#define     MAXBUFFER               4096//8192
#define     MAXBUFFER_AMSDU         4000
#define     MAXBUFFER_AMPDU         1600
#define     MAXBUFFER_NORXAGG       8192
#define     MAXBUFFER_RXAGG         32768
#define     MAXBUFFER_TXAGG         32768
#define     MAXBUFFER_FRAG          1600
#define     MAXBUFFER_RX            8000
#define     AMPDU_MaxLength         4000
#define     LoopBack_MaxLength      1948    //2048-32-24-4-(4+4+8+4)(SW TKIP)
#define     CCKScrollMAXBUFFER      1960
#define     OFDMScrollMAXBUFFER     1960
#define     HTScrollMAXBUFFER       1960
#define     ScrollMAXBUFFER         1960
#define     WlanHeaderLen           24
#define     MaxEtherCmdLen          32800 //  size should larger than MAXBUFFER_RXAGG+ sizeof(InterCmdHdr)
#define     MAX_FIFO_SIZE           8192//1024
#define     LinkListEleSize         1024//512
#define     DisplayBuffSize         128
#define     TXAGGLinkListEleSize    128
#define     MAX_TX_IRP_NUM          1
#define     MAX_RX_IRP_NUM          1

#define     LLT_HALF_PAGE_NUM       0
#define     LISTLEN 16



/*
u4Byte CAMData_Security[CAM_DATA_SIZE]=
{
	//default key
	0x0000FF04,0x00000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF14,0x00000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF08,0x00000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF10,0x00000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,

	//Addr1
	0x0000FF04,0x01000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF15,0x01000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF0A,0x01000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF13,0x01000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	//Addr2
	0x0000FF04,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF15,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF0A,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF13,0x02000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	
	0x0000FF04,0x03000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF15,0x03000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF0A,0x03000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF13,0x03000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,

	0x0000FF04,0x04000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF15,0x04000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF0A,0x04000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF13,0x04000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,

	0x0000FF04,0x05000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF15,0x05000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF0A,0x05000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF13,0x05000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,	

	0x0000FF04,0x06000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF15,0x06000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF0A,0x06000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF13,0x06000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,	

	0x0000FF04,0x07000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF15,0x07000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF0A,0x07000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,
	0x0000FF13,0x07000000,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB,0xABABABAB
};	
*/



/* Structure for the array which is used to keep the TX buffer*/
typedef struct _TxArrayStruct_ {
    u4Byte                 PKTLen;//Packet length
    u4Byte                 PayloadLen; //Payload length
    u1Byte                 Packet[MAXBUFFER];//Packet: Including Payload, Wlan hdr, TX Desc
    u1Byte                 Payload[MAXBUFFER];//Payload
    BOOLEAN                EleInSending;
}TxArrayStruct;

/*TX Link List Element*/
typedef struct _TX_LINK_LIST_ELE_ {
//    TX_LINK_LIST_ELE *prev, *next;
//  HANDLE                  hFile;  ///< The File handle.
//      OVERLAPPED          OverLapped;  ///< The overlapped structure.
    TxArrayStruct          TXData;
    u4Byte                 TxSF;
    BOOLEAN                Sent;
    BOOLEAN                Checked;
    u2Byte                 PortId;
    u2Byte                 PortDestNum;
}TX_LINK_LIST_ELE;



//======================Security===========================
#define  Mk16(hi,lo) ((lo) ^ (((unsigned short)(hi)) << 8))
#define	 Mk32(hi16,lo16) (((unsigned long)(lo16)) ^ (((unsigned long)(hi16)) << 16))

typedef struct _WLAN_HEADER_WAPI_EXTENSION
{
    u1Byte      KeyIdx;
    u1Byte      Reserved;
    u1Byte      PN[16];

} WLAN_HEADER_WAPI_EXTENSION, *PWLAN_HEADER_WAPI_EXTENSION;


typedef enum _RT_STATUS{
	RT_STATUS_SUCCESS,
	RT_STATUS_FAILURE,
	RT_STATUS_PENDING,
	RT_STATUS_RESOURCE,
	RT_STATUS_INVALID_CONTEXT,
	RT_STATUS_INVALID_PARAMETER,
	RT_STATUS_NOT_SUPPORT,
	RT_STATUS_OS_API_FAILED,
}RT_STATUS,*PRT_STATUS;

#define HAL_PADAPTER    VOID *//PRTL8192CD_PRIV

//======================8051===========================
#define downloadRAM             0
#define downloadROM             1
#define downloadTXFWTOTXPKTBUF  2
#define downloadRAMTOTXPKTBUF   3
#define downloadTXPKTPATTERN    4
#define downloadRXPKTPATTERN    5
#define downloadTXPRTPATTERN    6

#define HOST_READ_MODE          0x21
#define HOST_WRITE_MODE         0x09
#define LOCK_REG                0x1e5
#define LOCK_REG2               0x1e6


u4Byte 
Generate_Packet(
   IN   TX_LINKLIST_ID       Queue,
   IN   TX_LINK_LIST_ELE     *pLLEle
);

BOOLEAN
SendPacket(
    IN  u4Byte              LoopMode, 
    IN  TX_LINKLIST_ID      Queue,
    IN  TX_LINK_LIST_ELE*   LLEle
);

BOOLEAN
SendHighQueuePacket(
    IN  TX_LINKLIST_ID      Queue,
    IN  TX_LINK_LIST_ELE*   LLEle
);

BOOLEAN
PollingReceivePacket(
    IN  u4Byte      WaitTime
);

VOID
ReplaceTxPktOption(
    IN  u1Byte      InputRate,
    IN  u1Byte      QueueType
);

BOOLEAN
DmaBeacon(
    IN  u1Byte      BssidNum,
    IN  BOOLEAN     EnSwDownload
);

u4Byte 
DmaPktForFw(
   IN   u4Byte      PayloadLen,
   IN   u1Byte      *Payload
);


BOOLEAN
WLMAC_BIST (
    IN  u4Byte      CheckValue
);

BOOLEAN
WLBB_BIST (
    VOID
);

BOOLEAN
WlCtrlOperater(
    IN  u4Byte  Mode
);


#endif //#ifndef _WL_COMMAND_CTRL_H_
