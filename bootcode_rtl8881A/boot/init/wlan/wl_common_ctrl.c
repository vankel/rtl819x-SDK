enum { __FILE_NUM__= 4 };


#include <rtl_types.h>
#include <linux/types.h>
#include <linux/synclink.h>
#include "wl_common_ctrl.h"
#include "wl_type.h"

#include <string.h>
/******************************************/
    /* Macro*/
/******************************************/
#define LPoint 0
#define HPoint 4095

// Without address 4
#define WLAN_HEADER_LENGTH			24
#define WLAN_HEADER_WITH_QOS_SIZE	26
#define WLAN_HEADER_WITH_ENCRY_SIZE 28    


//=========security IV length,ICV length,EIV length, MIC length=================
#define FCSLen 4
#define IVLen  4
#define ICVLen 4
#define EIVLen 4
#define MICLen 8
#define CCMPheaderLen 8
#define WAPIheaderLen 18
#define WAPIMicLen    16


VOID
SetupBcnTxPacket(
    IN  TXBD_ELEMENT*           TxBdEle,
    IN  TX_LINK_LIST_ELE*       LLEle
);


VOID 
TxPktGen(
    IN  u1Byte          *TxPayloadBuff, 
    IN  u4Byte         PayloadLen, 
    IN  u1Byte          *TxPKTBuffer,
    IN  u4Byte         *PKTLength,
    IN  u1Byte          Queue,
    IN  PORT_INFO   *PortInfo, 
    IN  u1Byte          port, 
    IN  u1Byte          dest
);

VOID
TxPayloadGen(
    IN  u1Byte              *databuf, 
    IN  u4Byte             *pktSize,
    IN  u4Byte             *SeqNum, 
    IN  DATA_OPTION     lengthopt , 
    IN  DATA_LENGTH     length,
    IN  DATA_OPTION     payloadopt, 
    IN  DATA_PAYLOAD    payload, 
    IN  DATA_RATE       rate, 
    IN  BOOLEAN            bHT, 
    IN  u1Byte              Queue,
    IN  PORT_INFO       *PortInfo
);

VOID
TxSetPktOption(
    VOID
);

VOID
TxDataPathInit(
    VOID
);

VOID 
ExtractAddr(
    IN  u1Byte *header,
    IN  u1Byte *SA, 
    IN  u1Byte *TA, 
    IN  u1Byte *DA
);

VOID 
MicPrep(
    IN  u1Byte  *MPDU_orig,
    IN  u4Byte *MPDU, 
    IN  u4Byte MPDU_Length,
    IN  u1Byte  *SA,
    IN  u1Byte  *DA,
    IN  u1Byte  *priority
);

VOID 
Michael(
    IN  u4Byte *Key,
    IN  u4Byte *MPDU, 
    IN  u4Byte *micOut,
    IN  u4Byte MPDU_Length
);

BOOLEAN 
WapiIncreasePN(
    IN  u1Byte *PN, 
    IN  u1Byte AddCount
);

VOID
DumpBuff (
    IN  u4Byte Len,
    IN  u1Byte* pBuf
);

BOOLEAN
ReceivePacket(
    IN  u2Byte ReadPoint
);

BOOLEAN
CheckSinglePacket(
    IN  u1Byte  *pDataBuf
);

VOID 
SoftwareCrc32 (
    IN  u1Byte      *in,
    IN  u2Byte     byte_num,
    IN  u1Byte      *out
);

u2Byte
CheckAvailableTXNum (
    IN  u2Byte Rpoint,
    IN  u2Byte Wpoint,
    IN  u4Byte MaxNum
);

BOOLEAN 
CheckMultiSegPacket (
    VOID
);

BOOLEAN 
GetAvaliableRXBuff (
    IN  u2Byte *FreeIndex
);

BOOLEAN 
FreeRXBuff (
    IN  u4Byte Address
);

VOID
SetupTxPacket(
    IN  TXBD_ELEMENT*   TxBdEle,
    IN  TX_LINK_LIST_ELE*       LLEle
);

VOID
SetupTxPacketForExtendTXBD(
    IN  TXBD_ELEMENT*                   TxBdEle,
    IN  EXTEND_TXBD_ELEMENT_MANAGER*    ExTxbdMan,
    IN  TX_LINK_LIST_ELE*               LLEle,
    IN  u4Byte                             MaxTxBdNum
);

BOOLEAN
WLTxCont(
    VOID
);


/******************************************/
	/* Data Structure*/
/******************************************/
typedef	enum	_TypeSubtype
{
	asoc_req	= 0x00,
	asoc_rsp	= 0x10,
	reasoc_req	= 0x20,
	reasoc_rsp	= 0x30,
	probe_req	= 0x40,
	probe_rsp	= 0x50,
	type60		= 0x60,
	type70		= 0x70,
	beacon		= 0x80,
	atim		= 0x90,
	disasoc		= 0xa0,
	auth		= 0xb0,
	deauth		= 0xc0,
	action		= 0xd0,
	action_no_ack = 0xe0,
	typef0		= 0xf0,
	type04		= 0x04,
	type14		= 0x14,
	type24		= 0x24,
	type34		= 0x34,
	type44		= 0x44,
	type54		= 0x54,
	type64		= 0x64,
	control_wrapper	= 0x74,
	blkAckReq	= 0x84,
	blkAck 		= 0x94,
	ps_poll		= 0xa4,//= MkString(S8(0,0,1,0,0,1,0,1));
	rts			= 0xb4,//= MkString(S8(0,0,1,0,1,1,0,1));
	cts			= 0xc4,//= MkString(S8(0,0,1,0,0,0,1,1));
	ack			= 0xd4,//= MkString(S8(0,0,1,0,1,0,1,1));
	cfend		= 0xe4,//= MkString(S8(0,0,1,0,0,1,1,1));
	cfend_cfack	= 0xf4,//= MkString(S8(0,0,1,0,1,1,1,1));
	data		= 0x08,//= MkString(S8(0,0,0,1,0,0,0,0));
	data_cfack	= 0x18,//= MkString(S8(0,0,0,1,1,0,0,0));
	data_cfpoll	= 0x28,//= MkString(S8(0,0,0,1,0,1,0,0));
	data_cfack_cfpoll = 0x38,//= MkString(S8(0,0,0,1,1,1,0,0));
	null_frame	= 0x48,//= MkString(S8(0,0,0,1,0,0,1,0));
	cfack		= 0x58,//= MkString(S8(0,0,0,1,1,0,1,0));
	cfpoll		= 0x68,//= MkString(S8(0,0,0,1,0,1,1,0));
	cfack_cfpoll	= 0x78,//= MkString(S8(0,0,0,1,1,1,1,0));
	qdata		= 0x88,
	qdata_cfack 	= 0x98,
	qdata_cfpoll 	= 0xa8,
	qdata_cfack_cfpoll = 0xb8,
	qnull_frame = 0xc8,
	typed8		= 0xd8,
	qcfpoll 		= 0xe8,
	qcfack_cfpoll	= 0xf8,
	mesh_data	= 0x0c,
    mesh_management	= 0x1c
}TypeSubtype;


/******************************************/
    /* Data Structure*/
/******************************************/
static u4Byte payload_pattern[5] = {0x00, 0x55, 0xAA, 0xFF, 0x3E};
static u1Byte SerialPayload[20000];
static BOOLEAN SerialPayload_Init =FALSE;

static u4Byte PatternLen[3] = {256, 512, 1343};

LB_MODE LbMode = MAC_Delay_LB;

DATA_PATH_VARI      DataPathVari;
MAC_SETTING_GEN     MacSettingGen;
MAC_SETTING_PORT    MacSettingPort;

u4Byte WlTxSegThreshold;
u4Byte WlExTxSegThreshold;
u1Byte  TempCounter;

extern HCI_DMA_MANAGER     *hci_dma_manager;
extern RX_BUF_MAN          RxBufEntry[MAX_BUF_NUM];

u4Byte gTxLen = 256;
u1Byte  gEnableExTXBD;

TX_LINK_LIST_ELE          StackLLEle[5][16];
u32                       TxFailCheckCounter;


//-----------WAPI -------------------------------------------------// add by Guo.Mingzhi
static u1Byte  Init_PN[16]={0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36};
static u1Byte  nInc_PN=1;
static u1Byte  TxLastPN[16]={0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36};
static u1Byte  RxLastPN[16]={0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36,0x5c,0x36};
u1Byte	 txbw, KeyID=0, KeyID2=0;
BOOLEAN WlRxResult;
u1Byte  TxDescBufForFW[64];
TX_LINK_LIST_ELE              BecStackLLEle[8];


u4Byte 
Generate_Packet(
   IN   TX_LINKLIST_ID       Queue,
   IN   TX_LINK_LIST_ELE     *pLLEle
)
{
    Port_id             Port = Port_1;
    u1Byte                  Dest =0;
    PORT_INFO           *PortInfo;
    PACKET_SETTING_UI   *PktSet;
    BOOLEAN                HT = MacSettingGen.source_data.ht_support;
    BOOLEAN                TX_AGG = MacSettingGen.tx_discriptor.tx_agg_support;
	AGG_PARAMETER	    *Agg_Para =&(DataPathVari.Agg_Para[Queue]);
    u4Byte                 SeqNum = (u4Byte)&(DataPathVari.SeqNum[Queue]);
    u4Byte                 nIndex=0;
    u2Byte                 chkArray[16];
    u2Byte                 ChkSum=0x0000;
    u4Byte                 j=0;
#if 0   
    if(bTXSyncRX[QoS_ID] == TRUE)
    {
        UIVariable.ThreadVari.TRX_CS.Unlock();
        WaitForSingleObject(ThreadVari->RXDataProcessed[QoS_ID],INFINITE);          
        UIVariable.ThreadVari.TRX_CS.Lock();
    }
    
    UIVariable.DataPathVari.AggNum[QoS_ID]=0;
    if(TX_AGG)
    {
        /*Get the free Link List element*/
        LLRet = GetFreeTXAGGLinkListEle(&AGG_LLEle, QoS_ID);
        if(LLRet==LINKLIST_UNAVILABLE)
        {
            //MSG_PRINT(CATEGORY_GENERAL, MSG_ERROR,"Free link list unavailable, wait for Free Event");
            AfxMessageBox("AGG Free link list unavailable, wait for Free Event");
        }
        memset(AGG_LLEle->DataBuff, 0, MAXBUFFER_TXAGG);

        /*Reset Agg Parameter*/
        memset(Agg_Para, 0, sizeof(AGG_PARAMETER));
        Agg_Para->aggOrgBuffer=(BYTE*)AGG_LLEle->DataBuff;
        Agg_Para->aggShiftPtr=(BYTE*)AGG_LLEle->DataBuff;
        Agg_Para->totalLength = 0;
        Agg_Para->descCount= 0;
        Agg_Para->totalCount= 0;        
        
    }
#endif
#if 0
    do
    {       
#if 1
        /*leave this loop until get the free tx link list element*/
        do{
            /*Get the free Link List element*/
            LLRet = GetFreeTXLinkListEle(&LLEle, Queue);
            if(LLRet==LINKLIST_UNAVILABLE)
            {
                if(bTXSyncRX[Queue] == FALSE)
                {
                    LLEle = GetTXSendingList(Queue);
                    if(LLEle!=NULL)
                    {
                        MVTXLinkListEle(LLEle, TXList_Sending, TXList_WaitToSend, Queue);
                        LLRet =LINKLIST_SUCCESS;
                    }
                    else
                    {
                        AfxMessageBox("Can not find any element in both FREE and SENDING linklist", MB_ICONERROR | MB_OK);
                    }
                }
                else
                {
                    UIVariable.ThreadVari.TRX_CS.Unlock();
                    AfxMessageBox("Can not find any free TX link list element, wait forever", MB_ICONERROR | MB_OK);
                    WaitForSingleObject(ThreadVari->FreeTXLinkListEleEvent,INFINITE);
                    AfxMessageBox("Get element in free TX link list", MB_ICONERROR | MB_OK);                    
                    UIVariable.ThreadVari.TRX_CS.Lock();
                }
            }
        }while(LLRet==LINKLIST_UNAVILABLE);
#endif

        MSG_PRINT(CATEGORY_GENERAL, MSG_WARNING,"LinkListElePtr = %d ", LLEle);
#endif
        /*Init TX Buffer*/
        memset((u1Byte *)&(pLLEle->TXData), 0, sizeof(pLLEle->TXData));
        //LLEle->Length=0;

        /*Decide the Port and Destination according to the link type*/
        //For multi port
        Port = pLLEle->PortId;//PortGen();
        Dest = pLLEle->PortDestNum;//DestSel(Port);
        //dprintf("Port id: %d, Dest: %d\n",Port , Dest);
        
        /*assign the local pointer to global data structure*/
        PortInfo = &(MacSettingPort.Port[Port][Dest]);
        PktSet=   &(MacSettingGen.source_data.packet[Queue]);
        
        /*Generate Payload according to the UI Setting*/
        TxPayloadGen((u1Byte*)&(pLLEle->TXData.Payload), (u4Byte*)&(pLLEle->TXData.PayloadLen), 
                     (u4Byte*) &SeqNum, PktSet->lengthopt, PktSet->length, PktSet->payloadopt,
                     PktSet->payload, PktSet->rate,HT, (u1Byte)Queue,(PORT_INFO*) PortInfo);

        /*Generate Packet*/
        TxPktGen((u1Byte*)&(pLLEle->TXData.Payload), pLLEle->TXData.PayloadLen, 
                 (u1Byte*)&(pLLEle->TXData.Packet), (u4Byte*)&(pLLEle->TXData.PKTLen), 
                 (u1Byte)Queue, (PORT_INFO*) PortInfo, Port , Dest);

        /*Check Frame*/
//        TxFrameCheck(LLEle->TXData.Packet, LLEle->TXData.PKTLen, PortInfo);


//      UIVariable.DataPathVari.AggNum[QoS_ID]++;
#if 0
        /*Aggregate*/
        if(TX_AGG)
        {
            LLEle->Sent=TRUE;
            txaggstatus[QoS_ID] = Agg_TX_Packet(LLEle->TXData.Packet, LLEle->TXData.PKTLen, AGG_LLEle->DataBuff, &AGG_LLEle->Length, (UCHAR)QoS_ID);
            if(txaggstatus[QoS_ID] ==agg_complete_size)
                MVTXLinkListEle(LLEle, TXList_WaitToSend, TXList_Free, QoS_ID);

        }

    }while(TX_AGG&&( txaggstatus[Queue]==agg_not_complete));
#endif

#if 0    
    if(TX_AGG)
    {   
        ReleaseWaitToSendLinkListEle(QoS_ID);//Put the whole WaitToSendList to SendingList
        
        AGG_LLEle->DataBuff[23] = Agg_Para->totalCount; // offset 0x23h of the 1st Tx descriptor: usb_txagg_num
        //-----clean up the original check sum of the 1st tx descriptor-----//
        AGG_LLEle->DataBuff[28]=0x00;
        AGG_LLEle->DataBuff[29]=0x00;

        //-----recalculate the check sum of the 1st tx descriptor------//
        for(j=0;j<16;j++)
            chkArray[j] = (WORD)AGG_LLEle->DataBuff[2*j]+(AGG_LLEle->DataBuff[2*j+1]<<8);

        for(j=0;j<8;j++)
            ChkSum ^= (chkArray[2*j]^chkArray[2*j+1]);
        
        AGG_LLEle->DataBuff[28] = ChkSum & 0xff;
        AGG_LLEle->DataBuff[29] = (ChkSum>>8) & 0xff;

    }
#endif    
    return TRUE;
}



VOID 
TxPktGen(
    IN  u1Byte          *TxPayloadBuff, 
    IN  u4Byte          PayloadLen, 
    IN  u1Byte          *TxPKTBuffer,
    IN  u4Byte          *PKTLength,
    IN  u1Byte          Queue,
    IN  PORT_INFO       *PortInfo, 
    IN  u1Byte          port, 
    IN  u1Byte          dest
)
{

    u1Byte      wlanHead[24];
    u1Byte      wlanHead_encrypt[28];
    u1Byte      ExtendLen;
    TX_DESC curDesc; //added by shhuang
    u1Byte      QC[2];
    u2Byte     SCF[1];
    u4Byte     CalcMic_Tx[2000]={0};
    u1Byte      out_Tx[3000]={0};
    BOOLEAN    bRet=FALSE;
    u4Byte     writeLength=0;
    u4Byte     status=FW_NOERROR;
    u1Byte      Dat[68]={0x24,0x00,0x20,0x8D,0x00,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xA3,0x06,
                     0x00,0x01,0x00,0x00,0x1b,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
                     0x88,0x01,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x02,0x03,0x04,0x05,0x06,0x07,
                     0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,0x30,0x6A,0x56,0x00,0xA3,0x16,0x0A,0x00,0x00,0x03,
                     0x1F,0x11,0x5E,0x37};
    u1Byte      TxExtendDescLen=0x00;
    TX_EXTEND_DESC  curExtendDesc;  
    BOOLEAN    *bLastFrag=NULL;
    BOOLEAN    *bFirstFrag=NULL;
    u1Byte      *nFrag=NULL;
    BOOLEAN    HT = MacSettingGen.source_data.ht_support;
    BOOLEAN    QOS = MacSettingGen.source_data.qos_support;
    BOOLEAN    EARLY_MODE = MacSettingGen.tx_discriptor.early_mode_support;
    BOOLEAN    EARLY_MODE_EXTEND = MacSettingGen.tx_discriptor.early_mode_extend;
    BOOLEAN    MSDU_FRAG_EN = MacSettingGen.tx_discriptor.msdu_frag;
    LINK_TYPE linktype = MacSettingGen.test_option.link_type;
    u4Byte     *SeqNum = &(DataPathVari.SeqNum[Queue]);
    u4Byte     Ampdu_Max_Len=0;
    const u1Byte   BCAddr[6] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
    u1Byte      IsBcn = MacSettingGen.source_data.IsBcn;
    u1Byte      IsDlForFw = MacSettingGen.source_data.IsDlForFw;
    u4Byte     DescLen = 0;

//    dprintf("START: hader len %d\n", *PKTLength);

    memset(wlanHead, 0x0, WLAN_HEADER_LENGTH);
    memset(wlanHead_encrypt, 0x0 , WLAN_HEADER_WITH_ENCRY_SIZE);
    memset(&curDesc, 0x0, sizeof(TX_DESC)); //added by shhuang
    memset(&curExtendDesc,0x00,sizeof(TX_EXTEND_DESC));

    if (IsBcn || IsDlForFw) {
        ExtendLen = 0;
        TxExtendDescLen = sizeof(TX_DESC);
    }
    else {
        ExtendLen=(QOS==TRUE)?2:0; //to be modified
        ExtendLen=(HT==TRUE)?(ExtendLen+4):ExtendLen;

        TxExtendDescLen=(EARLY_MODE==TRUE)?(sizeof(TX_DESC)+8):sizeof(TX_DESC); 
    }

    //2 Generate TX Descriptor and TX Desctriptor extension
    HALWLTxDesxGenHandler(PayloadLen,  Queue, IsBcn, IsDlForFw,
                          PortInfo, &curDesc, &curExtendDesc);

    DescLen = sizeof(TX_DESC);

//    dprintf("TX Desc Len = %d\n",DescLen);
    
    //Copy TX Desc to TX Buffer
    memcpy(TxPKTBuffer, &curDesc, sizeof(TX_DESC)); //TxDesc is first part

    if (IsDlForFw) {
        *PKTLength = TxExtendDescLen+ExtendLen;
        
        return;        
    }

    if (IsBcn) {
        wlanHead[0] = 0x08;

        wlanHead[1] = 0x00;
        

        //from DS
        wlanHead[1] |= BIT1;
                
        memcpy(wlanHead+4,BCAddr, 0x06);
        
        memcpy(wlanHead+10, PortInfo->address_info.src_addr,6);        
        memcpy(wlanHead+16, PortInfo->address_info.bssid, 6);
        memcpy(TxPKTBuffer+TxExtendDescLen, wlanHead, WLAN_HEADER_LENGTH);

        *PKTLength = TxExtendDescLen+ WLAN_HEADER_LENGTH+ExtendLen;
        
        return;
    }

    //For Early Mode, Copy Extend TX Desc to TX Buffer
    if(EARLY_MODE)
    {
        memcpy(TxPKTBuffer+sizeof(TX_DESC),&curExtendDesc,sizeof(TX_EXTEND_DESC));
    }

     switch(Queue)
    {
        case TX_LINKLIST_VO:
            bLastFrag=&(DataPathVari.bLastFrag_VO);   bFirstFrag=&(DataPathVari.bFirstFrag_VO);
            nFrag=&(DataPathVari.nFrag_VO);
            break;
        case TX_LINKLIST_VI:
            bLastFrag=&(DataPathVari.bLastFrag_VI);   bFirstFrag=&(DataPathVari.bFirstFrag_VI);
            nFrag=&(DataPathVari.nFrag_VI);
            break;
            
        case TX_LINKLIST_BE:
            bLastFrag=&(DataPathVari.bLastFrag_BE);   bFirstFrag=&(DataPathVari.bFirstFrag_BE);
            nFrag=&(DataPathVari.nFrag_BE);
            
            break;
        case TX_LINKLIST_BK:
            bLastFrag=&(DataPathVari.bLastFrag_BK);   bFirstFrag=&(DataPathVari.bFirstFrag_BK);
            nFrag=&(DataPathVari.nFrag_BK);  
            break;          
        case TX_LINKLIST_MAG:
            bLastFrag=&(DataPathVari.bLastFrag_MG);   bFirstFrag=&(DataPathVari.bFirstFrag_MG);
            nFrag=&(DataPathVari.nFrag_MG);
            break;
        case TX_LINKLIST_H0:
            bLastFrag=&(DataPathVari.bLastFrag_H0);   bFirstFrag=&(DataPathVari.bFirstFrag_H0);
            nFrag=&(DataPathVari.nFrag_H0);
            break;

        case TX_LINKLIST_H1:
            bLastFrag=&(DataPathVari.bLastFrag_H1);   bFirstFrag=&(DataPathVari.bFirstFrag_H1);
            nFrag=&(DataPathVari.nFrag_H1);
            break;
        case TX_LINKLIST_H2:
            bLastFrag=&(DataPathVari.bLastFrag_H2);   bFirstFrag=&(DataPathVari.bFirstFrag_H2);
            nFrag=&(DataPathVari.nFrag_H2);
            break;
        case TX_LINKLIST_H3:
            bLastFrag=&(DataPathVari.bLastFrag_H3);   bFirstFrag=&(DataPathVari.bFirstFrag_H3);
            nFrag=&(DataPathVari.nFrag_H3);
            break;
        case TX_LINKLIST_H4:
            bLastFrag=&(DataPathVari.bLastFrag_H4);   bFirstFrag=&(DataPathVari.bFirstFrag_H4);
            nFrag=&(DataPathVari.nFrag_H4);
            break;
        case TX_LINKLIST_H5:
            bLastFrag=&(DataPathVari.bLastFrag_H5);   bFirstFrag=&(DataPathVari.bFirstFrag_H5);
            nFrag=&(DataPathVari.nFrag_H5);
            break;
        case TX_LINKLIST_H6:
            bLastFrag=&(DataPathVari.bLastFrag_H6);   bFirstFrag=&(DataPathVari.bFirstFrag_H6);
            nFrag=&(DataPathVari.nFrag_H6);
            break;
        case TX_LINKLIST_H7:
            bLastFrag=&(DataPathVari.bLastFrag_H7);   bFirstFrag=&(DataPathVari.bFirstFrag_H7);
            nFrag=&(DataPathVari.nFrag_H7);
            break;

    }   
 
    //2 Generate WLAN Header
       //if(((Info->SwEn)||(Info->HwEn))&&((mdInfo.TxWEP40)||(mdInfo.TxWEP104)||(mdInfo.TKIP)||(mdInfo.AES)||(mdInfo.WAPI)))
    if(PortInfo->encryption_info.type!=TYPE_NONE)   
    {
        wlanHead_encrypt[0]=0x08;  //set data frame
        if(QOS==TRUE)       
            wlanHead_encrypt[0]=qdata;

        wlanHead_encrypt[1]=0x40; //set protected frame(set WEP bit)

        if(MSDU_FRAG_EN && ((*bLastFrag)==FALSE))
            wlanHead_encrypt[1]|=BIT2;

        if(HT)
            wlanHead_encrypt[1]|=BIT7;

        if((linktype==AP_NONE|| linktype==AP_STA) && (port == 0))   //from DS
            wlanHead_encrypt[1] |= BIT1;
        else if((linktype==STA_NONE|| linktype==STA_STA)  && (port == 0)) //To DS
            wlanHead_encrypt[1] |= BIT0;
        else if((linktype==NONE_STA|| linktype==AP_STA|| linktype==STA_STA)  && (port == 1)) //To DS
            wlanHead_encrypt[1] |= BIT0;

        if(PortInfo->address_info.cast_type==UNICAST)
            memcpy(wlanHead_encrypt+4,&PortInfo->address_info.des_addr[0], 0x06);       
        else
            memcpy(wlanHead_encrypt+4,BCAddr, 0x06);        
        
        memcpy(wlanHead_encrypt+10,&PortInfo->address_info.src_addr[0],0x06);
        memcpy(wlanHead_encrypt+16, &PortInfo->address_info.bssid[0],0x06);

        memcpy(TxPKTBuffer+TxExtendDescLen, wlanHead_encrypt, WLAN_HEADER_WITH_ENCRY_SIZE);//header is third part
        
    }
    else
    {
        wlanHead[0] = 0x08;
        if(QOS==TRUE){      
            wlanHead[0]=qdata;    
        }

        wlanHead[1] = 0x00;

        if(MSDU_FRAG_EN && ((*bLastFrag)==FALSE))
            wlanHead[1]|=BIT2;

        if(HT)         
            wlanHead[1]|=BIT7;

        

        if((linktype==AP_NONE|| linktype==AP_STA) && (port == 0))   //from DS
            wlanHead[1] |= BIT1;
        else if((linktype==STA_NONE|| linktype==STA_STA)  && (port == 0)) //To DS
            wlanHead[1] |= BIT0;
        else if((linktype==NONE_STA|| linktype==AP_STA|| linktype==STA_STA)  && (port == 1)) //To DS
            wlanHead[1] |= BIT0;
        
        
        if(PortInfo->address_info.cast_type==UNICAST)
            memcpy(wlanHead+4,  PortInfo->address_info.des_addr, 6);        
        else
            memcpy(wlanHead+4,BCAddr, 0x06);
        
        memcpy(wlanHead+10, PortInfo->address_info.src_addr,6);        
        memcpy(wlanHead+16, PortInfo->address_info.bssid, 6);
        memcpy(TxPKTBuffer+TxExtendDescLen, wlanHead, WLAN_HEADER_LENGTH);
    }


    switch(Queue)
    {     
        case TX_LINKLIST_VO:  
            QC[0] = 0x06 ; // |TID|EOSP|AckPolicy|
            QC[1] = 0; // Queue size
            SCF[0] = (*SeqNum<<4)+(DataPathVari.nFrag_VO);
        break;
        case TX_LINKLIST_VI:
            QC[0] = 0x04; // |TID|EOSP|AckPolicy|
            QC[1] = 0; // Queue size
            SCF[0] = (*SeqNum<<4)+(DataPathVari.nFrag_VI);

        break;              
        case TX_LINKLIST_BE:
            QC[0] = 0x00 ; // |TID|EOSP|AckPolicy|
            QC[1] = 0; // Queue size
            SCF[0] = (*SeqNum<<4)+(DataPathVari.nFrag_BE);

        break;
        case TX_LINKLIST_BK:
            QC[0] = 0x01; // |TID|EOSP|AckPolicy|
            QC[1] = 0; // Queue size
            SCF[0] = (*SeqNum<<4)+(DataPathVari.nFrag_BK);

        break;
        case TX_LINKLIST_MAG:
            //2 Need check
            QC[0] = 0x00; // |TID|EOSP|AckPolicy|
            QC[1] = 0; // Queue size
            SCF[0] = (*SeqNum<<4)+(DataPathVari.nFrag_BK);

        break;
        case TX_LINKLIST_H0:
        case TX_LINKLIST_H1:
        case TX_LINKLIST_H2:
        case TX_LINKLIST_H3:
        case TX_LINKLIST_H4:
        case TX_LINKLIST_H5:
        case TX_LINKLIST_H6:
        case TX_LINKLIST_H7:
            QC[0] = 0x00; // |TID|EOSP|AckPolicy|
            QC[1] = 0; // Queue size
            SCF[0] = 0;
        break;  

    }
    //add sequence control
    memcpy(TxPKTBuffer+TxExtendDescLen+(WLAN_HEADER_LENGTH-2), SCF, 2);

    if(QOS)
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH, QC, 2);//header is third part

    if(HT)
    {
        u1Byte HTC[4]={0,0,0,0};
        HTC[0]=0x02;

        if(QOS)
            memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+2,HTC,4);
        else
            memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH,HTC,4);
    }

    /*Payload part*/
    if((PortInfo->encryption_info.encryption==HW) && (PortInfo->encryption_info.type==WEP40))
    {
        TxPKTBuffer[TxExtendDescLen+ WLAN_HEADER_LENGTH+3+ExtendLen]=(PortInfo->encryption_info.keyid<< 6) & 0xc0;                    // Set KeyID is key0 (key0~key3)
        *PKTLength = PayloadLen + TxExtendDescLen+ WLAN_HEADER_WITH_ENCRY_SIZE +ExtendLen;
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_WITH_ENCRY_SIZE+ExtendLen, TxPayloadBuff, PayloadLen);       
    }
    else if((PortInfo->encryption_info.encryption==HW) && (PortInfo->encryption_info.type==WEP104))
    {
        TxPKTBuffer[TxExtendDescLen+ WLAN_HEADER_LENGTH+3+ExtendLen]=(PortInfo->encryption_info.keyid << 6) & 0xc0;                    // Set KeyID is key3 (key0~key3)
        *PKTLength = PayloadLen + TxExtendDescLen+ WLAN_HEADER_WITH_ENCRY_SIZE+ExtendLen;
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_WITH_ENCRY_SIZE+ExtendLen, TxPayloadBuff, PayloadLen);       
    }
    else if((PortInfo->encryption_info.encryption==HW) && (PortInfo->encryption_info.type==TKIP))
    {   
        u1Byte key[16]= {0x0, 0x0, 0x0, 0xab,0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab, 0xab};
        
        u4Byte micKey[2]={0x82c094e9, 0x2c82fb63};
        
        u1Byte EncHeader[8] =   {0xa9, 0x29, 0xfe, 0x20, 0x75, 0x3e, 0x1d, 0xcf};
        u1Byte Header[] =
        {0x18, 0x4b, 0xe0, 0x63, 0x40, 0xec, 0x29, 0xfa, 0x75, 0x9b, 0x53, 0xf8, 0x69, 0xfe, 0x27, 0x9a, 0xf0, 0xf9, 0xf8, 0xa6, 0x54, 0x16, 0x80, 0xe3, 0x52, 0xbf, 0xd2, 0x70, 0x3d, 0x24};
        u1Byte TestData[] =    {0x7f, 0x91, 0xf2, 0x47, 0x2d, 0x7a, 0x12, 0x1c, 0x9c, 0xdd, 0x4b, 0x6c, 0x90, 0x80, 0x67, 0x5a, 0x10, 0x20, 0xaa, 0x00};
        u1Byte KeyLen = 16;
        u2Byte dataBodyDwordLen;
        u1Byte SA[6], DA[6], TA[6];
        u1Byte priority[] = {0, 0, 0, 0};
        u4Byte micOut[2];
#if 0
        u4Byte len = curDesc.Dword0.txpktsize;
#else
        u4Byte len;
        len = (curDesc.Dword0 & 0xFFFF);
#endif
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen+IVLen+EIVLen, TxPayloadBuff, PayloadLen);
        dataBodyDwordLen = PayloadLen/4 + 2;
        ExtractAddr(TxPKTBuffer+TxExtendDescLen, SA,TA,DA);
        MicPrep(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+8+ExtendLen,/*CalcMic*/CalcMic_Tx, PayloadLen, SA, DA, priority);
        Michael(micKey, /*CalcMic*/CalcMic_Tx, micOut, dataBodyDwordLen+4);
        
        TxPKTBuffer[TxExtendDescLen+ WLAN_HEADER_LENGTH+3+ExtendLen]=(PortInfo->encryption_info.keyid << 6) & 0xc0 |0x20;                    // Set KeyID is key1 and set ExtIV bit (key0~key3)
        
        // Set KeyID
        memcpy(TxPKTBuffer+TxExtendDescLen + WLAN_HEADER_LENGTH+8+PayloadLen+ExtendLen,micOut,8);      // Set MIC  
        *PKTLength = PayloadLen +TxExtendDescLen + WLAN_HEADER_LENGTH+IVLen+EIVLen+MICLen+ExtendLen; //IV(4)+EIV(4)+MIC(8)+ICV(4)
        
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+4+4+ExtendLen, TxPayloadBuff, PayloadLen); 
      } 
    else if((PortInfo->encryption_info.encryption==HW) && (PortInfo->encryption_info.type==AES))
    {
        TxPKTBuffer[TxExtendDescLen+ WLAN_HEADER_LENGTH+3+ExtendLen]=(PortInfo->encryption_info.keyid << 6) & 0xc0 |0x20;                    // Set KeyID is key2 and set ExtIV bit (key0~key3)
        *PKTLength = PayloadLen + TxExtendDescLen+ WLAN_HEADER_LENGTH+ExtendLen+CCMPheaderLen; //Info->AES header(8)+MIC(8)
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen+CCMPheaderLen, TxPayloadBuff, PayloadLen);      
    }   
    else if((PortInfo->encryption_info.encryption==HW) && (PortInfo->encryption_info.type==WAPI))       //add by Guo.Mingzhi
    {
        WLAN_HEADER_WAPI_EXTENSION HeaderExtension;

        HeaderExtension.KeyIdx=0x00;
        HeaderExtension.Reserved=0x00;

        WapiIncreasePN(TxLastPN,nInc_PN);
        memcpy(&HeaderExtension.PN,TxLastPN,0x10);

        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen,&HeaderExtension,WAPIheaderLen);
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen+WAPIheaderLen,TxPayloadBuff,PayloadLen);

        *PKTLength=PayloadLen+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen+WAPIheaderLen;
    }
    else if((PortInfo->encryption_info.encryption==SW) && 
            ((PortInfo->encryption_info.type==WEP40)||
            (PortInfo->encryption_info.type==WEP104)))
    {
#if 0
        unsigned char key[16]={0x0, 0x0, 0x0, 0xab,0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab, 0xab};
        unsigned char KeyLen;//=Adapter->SecurityMode==1 ? 8:16;   // 1:40 bits,2:104 bits
        unsigned char keyIdx;


        SWCAMSearchHAL(wlanHead_encrypt+4, PortInfo->encryption_info.keyid, key);

        if(PortInfo->encryption_info.type==WEP40)
            KeyLen=8;
        if(PortInfo->encryption_info.type==WEP104)
            KeyLen=16;

        *PKTLength = PayloadLen + TxExtendDescLen+ WLAN_HEADER_WITH_ENCRY_SIZE+ExtendLen+ICVLen;//add 4 cause pkt sent twice?
        EncodeWEP(key,KeyLen,TxPayloadBuff,PayloadLen,out_Tx); //out_Tx(ciphertest)=Payload+ICV
        //Set IV

        if(PortInfo->encryption_info.type==WEP40)
            TxPKTBuffer[TxExtendDescLen+ WLAN_HEADER_LENGTH+3+ExtendLen]=(PortInfo->encryption_info.keyid << 6) & 0xc0;                    // Set KeyID is key0 (key0~key3)
        else if(PortInfo->encryption_info.type==WEP104)
            TxPKTBuffer[TxExtendDescLen+ WLAN_HEADER_LENGTH+3+ExtendLen]=(PortInfo->encryption_info.keyid << 6) & 0xc0;                    // Set KeyID is key3 (key0~key3)
        memcpy(TxPKTBuffer+ TxExtendDescLen+ WLAN_HEADER_LENGTH+ExtendLen+IVLen, out_Tx , PayloadLen+ICVLen);     // Set Data & ICV
#endif
    }
    else if((PortInfo->encryption_info.encryption==SW) && (PortInfo->encryption_info.type==TKIP))
    {
#if 0
        UCHAR key[16]= {0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab, 0xab};
        
        ULONG micKey[2]={0x82c094e9, 0x2c82fb63};
        
        UCHAR EncHeader[8] =   {0xa9, 0x29, 0xfe, 0x20, 0x75, 0x3e, 0x1d, 0xcf};
        UCHAR Header[] =
        {0x18, 0x4b, 0xe0, 0x63, 0x40, 0xec, 0x29, 0xfa, 0x75, 0x9b, 0x53, 0xf8, 0x69, 0xfe, 0x27, 0x9a, 0xf0, 0xf9, 0xf8, 0xa6, 0x54, 0x16, 0x80, 0xe3, 0x52, 0xbf, 0xd2, 0x70, 0x3d, 0x24};
        UCHAR TestData[] =    {0x7f, 0x91, 0xf2, 0x47, 0x2d, 0x7a, 0x12, 0x1c, 0x9c, 0xdd, 0x4b, 0x6c, 0x90, 0x80, 0x67, 0x5a, 0x10, 0x20, 0xaa, 0x00};
        UCHAR KeyLen = 16;
        USHORT dataBodyDwordLen;
        UCHAR SA[6], DA[6], TA[6];
        UCHAR priority[] = {0, 0, 0, 0};
        ULONG micOut[2];
        int len=curDesc.txpktsize;
        unsigned char keyIdx;
        
        //////////////////////////////////////////////////////////////////////////
        SWCAMSearchHAL(wlanHead_encrypt+4, PortInfo->encryption_info.keyid, key);
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen+IVLen+EIVLen, TxPayloadBuff, PayloadLen);
        dataBodyDwordLen = PayloadLen/4 + 2;
        ExtractAddr(TxPKTBuffer+TxExtendDescLen, SA,TA,DA);
        MicPrep(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen+IVLen+EIVLen,CalcMic_Tx, PayloadLen, SA, DA, priority);
        Michael(micKey,CalcMic_Tx, micOut, dataBodyDwordLen+4);

        memcpy(EncHeader, TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen, IVLen+EIVLen);
        EncodeTKIP(micOut, TxPKTBuffer+TxExtendDescLen, PayloadLen, EncHeader, key, out_Tx);
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen, EncHeader, IVLen+EIVLen);
        TxPKTBuffer[TxExtendDescLen+ WLAN_HEADER_LENGTH+3+ExtendLen]=(PortInfo->encryption_info.keyid << 6) & 0xc0 |0x20;                    // Set KeyID is key1 and set ExtIV bit (key0~key3)
        // Set KeyID
        memcpy(TxPKTBuffer+TxExtendDescLen+ WLAN_HEADER_LENGTH+IVLen+EIVLen+ExtendLen,out_Tx,len-WLAN_HEADER_LENGTH-IVLen-EIVLen);    // Set Data, MIC, ICV
        *PKTLength = PayloadLen + TxExtendDescLen+ WLAN_HEADER_LENGTH+IVLen+EIVLen+MICLen+ICVLen+ExtendLen;
#endif
    }
    else if((PortInfo->encryption_info.encryption==SW) && (PortInfo->encryption_info.type==AES))
    {
#if 0
        unsigned char key[16] ={0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab};
        unsigned char KeyLen=16;
        int len=curDesc.txpktsize-8;
        UCHAR EncHeader[8] ={0, 0, 0, 0, 0, 0, 0, 0};
        BYTE        reserved[4]={0x0,0x0,0x0,0x0};

        unsigned char keyIdx;
        //////////////////////////////////////////////////////////////////////////
        SWCAMSearchHAL(wlanHead_encrypt+4, PortInfo->encryption_info.keyid, key);
        
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+CCMPheaderLen+ExtendLen, TxPayloadBuff, PayloadLen);
        EncodeCcmp(TxPKTBuffer+TxExtendDescLen, len-WLAN_HEADER_LENGTH-CCMPheaderLen-ExtendLen, EncHeader, key, out_Tx, 0);
        TxPKTBuffer[TxExtendDescLen+ WLAN_HEADER_LENGTH+3+ExtendLen]=(PortInfo->encryption_info.keyid << 6) & 0xc0 |0x20;                    // Set KeyID is key2 and set ExtIV bit (key0~key3)
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+CCMPheaderLen+ExtendLen,out_Tx,len-WLAN_HEADER_LENGTH-ExtendLen);     // Set Data, MIC
        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+4+ExtendLen, reserved, 0x4); //force high 4 bytes of CCMP header to 0x00, added for aVOID Info->AES SW encrypt burst issue(2007.09.14)        
        *PKTLength = PayloadLen + TxExtendDescLen+ WLAN_HEADER_LENGTH+CCMPheaderLen+MICLen+ExtendLen;
#endif
    }   
    else if((PortInfo->encryption_info.encryption==SW) && (PortInfo->encryption_info.type==WAPI))
    {
#if 0
        unsigned char key[16]={0xab, 0xab, 0xab, 0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab, 0xab, 0xab, 0xab,0xab, 0xab, 0xab};
        unsigned char mickey[16]={0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac,0xac};
        unsigned char KeyLen=16;
        unsigned char PDU_Len[2]={0x00,0x00};
        BYTE     FrameCtrl[2];
        BYTE     SeqCtrl[2];

        UCHAR CalcMic_Input1[64]={0};
        UCHAR CalcMic_Input2[MAXBUFFER];
        UCHAR Mic[16]={0x00};
        WLAN_HEADER_WAPI_EXTENSION HeaderExtension;
        unsigned short inputLen1_mic=0,inputLen2_mic=0;
        unsigned char  outputLen_mic=0;
        unsigned short inputLen_enc=0,outputLen_enc=0;
        
        HeaderExtension.KeyIdx=0x00;
        HeaderExtension.Reserved=0x00;
        WapiIncreasePN(TxLastPN, nInc_PN);
        memcpy(&HeaderExtension.PN,TxLastPN,16);

        memset(CalcMic_Input1,0x00,64);

        memcpy(FrameCtrl,TxPKTBuffer+TxExtendDescLen,2);
        FrameCtrl[0]&=(~0x70);
        FrameCtrl[1]=FrameCtrl[1]&(~0x38)|0x40;


            memcpy(CalcMic_Input1,FrameCtrl,2);   //Frame Control;
        memcpy(CalcMic_Input1+2,TxPKTBuffer+TxExtendDescLen+4,12);  //Addr1, Addr2
        memcpy(SeqCtrl,TxPKTBuffer+TxExtendDescLen+22,2);
        SeqCtrl[0]&=0x0f;
        SeqCtrl[1]=0x00;
            memcpy(CalcMic_Input1+14,SeqCtrl,2);   //Sequence Control;      
        memcpy(CalcMic_Input1+16,TxPKTBuffer+TxExtendDescLen+16,6);   //Addr3
        memset(CalcMic_Input1+22,0x00,6);   //Addr4

        if(QOS)
            memcpy(CalcMic_Input1+28,TxPKTBuffer+TxExtendDescLen+24,2);   //QOS
            
        memcpy(CalcMic_Input1+28+ExtendLen,&HeaderExtension,2);  //KeyIndex, Reserverd
        PDU_Len[0]=PayloadLen/256;
        PDU_Len[1]=PayloadLen%256;          
        memcpy(CalcMic_Input1+30+ExtendLen,PDU_Len,2);
        inputLen1_mic=32+ExtendLen;
        
        memset(CalcMic_Input2,0x00,MAXBUFFER);
        memcpy(CalcMic_Input2,TxPayloadBuff,PayloadLen);
        inputLen2_mic=PayloadLen;
        memcpy(&TxPKTBuffer[TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen],&HeaderExtension,WAPIheaderLen);
        WapiSMS4CalculateMic(mickey,TxLastPN, CalcMic_Input1, inputLen1_mic, CalcMic_Input2, inputLen2_mic, Mic, &outputLen_mic);   
        memcpy(TxPayloadBuff+PayloadLen,Mic,outputLen_mic);

        WapiSMS4Encryption(key, TxLastPN, TxPayloadBuff, PayloadLen+outputLen_mic, &TxPKTBuffer[TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen+WAPIheaderLen], &outputLen_enc);           
        *PKTLength=PayloadLen+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen+WAPIheaderLen+WAPIMicLen;
#endif
    }   
    else //No encrypt
    {
        //ASSERT(PayloadLen<(MAXBUFFER-sizeof(TX_DESC)));
//        memcpy(TxPKTBuffer+TxExtendDescLen+WLAN_HEADER_LENGTH+ExtendLen, TxPayloadBuff, PayloadLen); 
//        *PKTLength = PayloadLen + TxExtendDescLen+ WLAN_HEADER_LENGTH+ExtendLen;
       *PKTLength = TxExtendDescLen+ WLAN_HEADER_LENGTH+ExtendLen;

//        dprintf("END: hader len %d\n", *PKTLength);
    }

}


/*-----------------------------------------------------------------------------
 * Function:    TxPayloadGen()
 *
 * Overview:    This function is used to generate payload according to the rate, content, size configuration
 *
 * Input:       NONE
 *
 * Output:      NONE
 *
 * Return:      STATUS_SUCCESS: Operation Success
 *          
 * Note:        NONE
 *---------------------------------------------------------------------------*/
VOID
TxPayloadGen(
    IN  u1Byte              *databuf, 
    IN  u4Byte              *pktSize,
    IN  u4Byte              *SeqNum, 
    IN  DATA_OPTION         lengthopt , 
    IN  DATA_LENGTH         length,
    IN  DATA_OPTION         payloadopt, 
    IN  DATA_PAYLOAD        payload, 
    IN  DATA_RATE           rate, 
    IN  BOOLEAN             bHT, 
    IN  u1Byte              Queue,
    IN  PORT_INFO           *PortInfo
)
{
    //  TODO: Currently, rate option does not implemented
    u4Byte     maxLength;
    u1Byte      TID;
    BOOLEAN    *bLastFrag=NULL;
    BOOLEAN    *bFirstFrag=NULL;
    u4Byte     *pktSize_Amsdu=NULL;
    u1Byte      *nFrag=NULL;
    u1Byte      TxExtendDescLen=32;
    u1Byte      TxWlanHeaderLen=24;
    u4Byte     dwDataLength=0;
    u4Byte     fix_pkt_len = MacSettingGen.source_data.fix_pkt_len;  
    BOOLEAN    AMSDU_EN = MacSettingGen.tx_discriptor.amsdu_support;
    BOOLEAN    AMPDU_EN = MacSettingGen.tx_discriptor.ampdu_support;
    BOOLEAN    MSDU_FRAG_EN = MacSettingGen.tx_discriptor.msdu_frag;
    BOOLEAN    EARLY_MODE = MacSettingGen.tx_discriptor.early_mode_support;
    BOOLEAN    QOS = MacSettingGen.source_data.qos_support;
    BOOLEAN    Magic_Pkt = MacSettingGen.test_option.magic_pkt;
    BOOLEAN    Patten_Match_Pkt = MacSettingGen.test_option.patten_match_pkt;
    u1Byte      IsBcn = MacSettingGen.source_data.IsBcn;
    u4Byte     i;

        
    if(Magic_Pkt)//Gen Magic PKT Payload
    {
        //6*0xFF+16*Mac Address
        memset(databuf, 0xff, 102);//6+16*6=102;
        for(i=0; i<16; i++)
        {
            memcpy(databuf+6+i*6,&PortInfo->address_info.des_addr[0], 0x06);
        }
        *pktSize = 102;     
        return;
    }

    if(IsBcn)//Gen Beacon PKT Payload
    {
        u1Byte TIMIeLen = 0;
        memset(databuf, 0x5A, 16);
        *pktSize = 16;
        databuf += 16;
        dprintf("Bssid %d\n",PortInfo->address_info.bssid[0]);
        //Handle MBSSID for More data bit in TIM IE
        if (PortInfo->address_info.bssid[0]) {
            TIMIeLen = 4;
            TIMIeLen += 2; //Add Element ID and Length 
            databuf[0] = 5; //element id
            databuf[1] = 4; //length
            databuf[2] = 2; //DTIM counter
            databuf[3] = 2; //DTIM period
            databuf[4] = 0x80; //Bitmap control
            databuf[5] = 0; //Partial Virtual Bitmap
            PortInfo->TIMIeOffset = 16;
        }

        *pktSize += TIMIeLen;
        return;
    }


    if(Patten_Match_Pkt)//Gen Patten match PKT Payload
    {
        int PattenMatchLength=64;//Max Patten Match PKT length is 128
        memset(databuf, 0xEA, PattenMatchLength);
        //  TODO: In CRC_CCITT Tool, the payload in Packet.txt should be the same with the payload generated here
        *pktSize = PattenMatchLength;   
        return;
    }


    /*decide the max length*/
    if(bHT)
        maxLength=HTScrollMAXBUFFER;
    else
    {
        if((rate>=0)&&(rate<=3))
            maxLength=CCKScrollMAXBUFFER;
        else if((rate>=4)&&(rate<=11))
            maxLength=OFDMScrollMAXBUFFER;
        else
            maxLength=ScrollMAXBUFFER;
    }
    
     if(LbMode!=LB_Disable)
        maxLength=LoopBack_MaxLength;
     if(AMSDU_EN)
        maxLength=MAXBUFFER_AMSDU;
     if(AMPDU_EN)
        maxLength=MAXBUFFER_AMPDU;

     if(MSDU_FRAG_EN)
     {
        
         switch(Queue) {
             case TX_LINKLIST_VO:    
                 bLastFrag=&(DataPathVari.bLastFrag_VO);   bFirstFrag=&(DataPathVari.bFirstFrag_VO); 
                 pktSize_Amsdu=&(DataPathVari.pktSize_VO);  nFrag=&(DataPathVari.nFrag_VO);     
                 break;
             case TX_LINKLIST_VI:   
                 bLastFrag=&(DataPathVari.bLastFrag_VI);   bFirstFrag=&(DataPathVari.bFirstFrag_VI); 
                 pktSize_Amsdu=&(DataPathVari.pktSize_VI);  nFrag=&(DataPathVari.nFrag_VI);     
                 break;
             case TX_LINKLIST_BE:    
                 bLastFrag=&(DataPathVari.bLastFrag_BE);   bFirstFrag=&(DataPathVari.bFirstFrag_BE); 
                 pktSize_Amsdu=&(DataPathVari.pktSize_BE);  nFrag=&(DataPathVari.nFrag_BE);     
                 break;
             case TX_LINKLIST_BK:
                 bLastFrag=&(DataPathVari.bLastFrag_BK);   bFirstFrag=&(DataPathVari.bFirstFrag_BK); 
                 pktSize_Amsdu=&(DataPathVari.pktSize_BK);  nFrag=&(DataPathVari.nFrag_BK);     
                 break;
             case TX_LINKLIST_MAG:
                 bLastFrag=&(DataPathVari.bLastFrag_MG);   bFirstFrag=&(DataPathVari.bFirstFrag_MG); 
                 pktSize_Amsdu=&(DataPathVari.pktSize_MG);  nFrag=&(DataPathVari.nFrag_MG);     
                 break;         
             case TX_LINKLIST_H0:
                 bLastFrag=&(DataPathVari.bLastFrag_H0);   bFirstFrag=&(DataPathVari.bFirstFrag_H0);
                 pktSize_Amsdu=&(DataPathVari.pktSize_H0);  nFrag=&(DataPathVari.nFrag_H0);     
                 break;         
             case TX_LINKLIST_H1:
                 bLastFrag=&(DataPathVari.bLastFrag_H1);   bFirstFrag=&(DataPathVari.bFirstFrag_H1);
                 pktSize_Amsdu=&(DataPathVari.pktSize_H1);  nFrag=&(DataPathVari.nFrag_H1);     
                 break;         
             case TX_LINKLIST_H2:
                 bLastFrag=&(DataPathVari.bLastFrag_H2);   bFirstFrag=&(DataPathVari.bFirstFrag_H2);
                 pktSize_Amsdu=&(DataPathVari.pktSize_H2);  nFrag=&(DataPathVari.nFrag_H2);     
                 break;         
             case TX_LINKLIST_H3:
                 bLastFrag=&(DataPathVari.bLastFrag_H3);   bFirstFrag=&(DataPathVari.bFirstFrag_H3);
                 pktSize_Amsdu=&(DataPathVari.pktSize_H3);  nFrag=&(DataPathVari.nFrag_H3);     
                 break;         
             case TX_LINKLIST_H4:
                 bLastFrag=&(DataPathVari.bLastFrag_H4);   bFirstFrag=&(DataPathVari.bFirstFrag_H4);
                 pktSize_Amsdu=&(DataPathVari.pktSize_H4);  nFrag=&(DataPathVari.nFrag_H4);     
                 break;         
             case TX_LINKLIST_H5:
                 bLastFrag=&(DataPathVari.bLastFrag_H5);   bFirstFrag=&(DataPathVari.bFirstFrag_H5);
                 pktSize_Amsdu=&(DataPathVari.pktSize_H5);  nFrag=&(DataPathVari.nFrag_H5);     
                 break;         
             case TX_LINKLIST_H6:
                 bLastFrag=&(DataPathVari.bLastFrag_H6);   bFirstFrag=&(DataPathVari.bFirstFrag_H6);
                 pktSize_Amsdu=&(DataPathVari.pktSize_H6);  nFrag=&(DataPathVari.nFrag_H6);     
                 break;         
             case TX_LINKLIST_H7:
                 bLastFrag=&(DataPathVari.bLastFrag_H7);   bFirstFrag=&(DataPathVari.bFirstFrag_H7);
                 pktSize_Amsdu=&(DataPathVari.pktSize_H7);  nFrag=&(DataPathVari.nFrag_H7);
                 break;         

         }

         maxLength=MAXBUFFER_AMSDU;

         if((*bLastFrag)==TRUE)
         {
             if(lengthopt==INCREASING)           
                *pktSize_Amsdu=(u4Byte)((*pktSize_Amsdu)%maxLength+1);
             else if(lengthopt ==RANDOM)         
                *pktSize_Amsdu=0;//1 (u4Byte)(rand()%maxLength + 1);
             else if(lengthopt ==FIXED) 
                {

                    if(length==LENGTH_256)
                        *pktSize_Amsdu=(u4Byte)gTxLen;
                    else if(length==LENGTH_512)
                        *pktSize_Amsdu=(u4Byte)512;
                    else
                        *pktSize_Amsdu=(u4Byte)1343;           

                    //use the user defined packet length
                    if(0!=fix_pkt_len)
                        *pktSize_Amsdu = fix_pkt_len;
                    
                }

             if(*pktSize_Amsdu==0)   
                *pktSize_Amsdu=1;
             
             (*nFrag)=0;
             (*bFirstFrag)=TRUE;
             (*SeqNum)++;
             
             if((*pktSize_Amsdu)>MAXBUFFER_FRAG)
             {
                 (*pktSize)=MAXBUFFER_FRAG;  
                 (*pktSize_Amsdu)-=(*pktSize);
                 (*bLastFrag)=FALSE;
             }
             else
             {
                 (*pktSize)=(*pktSize_Amsdu);
                 (*bLastFrag)=TRUE;
             }
         }
         else
         {
             (*bFirstFrag)=FALSE;
             if((*pktSize_Amsdu)>ScrollMAXBUFFER)
             {
                 (*pktSize)=ScrollMAXBUFFER;
                 (*pktSize_Amsdu)-=(*pktSize);
                 (*bLastFrag)=FALSE;
                 (*nFrag)++;
             }
             else
             {
                 (*pktSize)=(*pktSize_Amsdu);
                 (*nFrag)++;
                 (*bLastFrag)=TRUE;
             }                   
         }  
     }
     else
     {
         if(lengthopt==INCREASING){
             switch(Queue) {
                 case TX_LINKLIST_VO:    
                     pktSize_Amsdu = &(DataPathVari.pktSize_VO);
                     break;
                 case TX_LINKLIST_VI:   
                     pktSize_Amsdu = &(DataPathVari.pktSize_VI);
                     break;
                 case TX_LINKLIST_BE:    
                     pktSize_Amsdu = &(DataPathVari.pktSize_BE);
                     break;
                 case TX_LINKLIST_BK:
                     pktSize_Amsdu = &(DataPathVari.pktSize_BK);
                     break;
                 case TX_LINKLIST_MAG:
                     pktSize_Amsdu = &(DataPathVari.pktSize_MG);
                     break;
                 case TX_LINKLIST_H0:
                     pktSize_Amsdu = &(DataPathVari.pktSize_H0);
                     break;
                 case TX_LINKLIST_H1:
                     pktSize_Amsdu = &(DataPathVari.pktSize_H1);
                     break;
                 case TX_LINKLIST_H2:
                     pktSize_Amsdu = &(DataPathVari.pktSize_H2);
                     break;
                 case TX_LINKLIST_H3:
                     pktSize_Amsdu = &(DataPathVari.pktSize_H3);
                     break;
                 case TX_LINKLIST_H4:
                     pktSize_Amsdu = &(DataPathVari.pktSize_H4);
                     break;
                 case TX_LINKLIST_H5:
                     pktSize_Amsdu = &(DataPathVari.pktSize_H5);
                     break;
                 case TX_LINKLIST_H6:
                     pktSize_Amsdu = &(DataPathVari.pktSize_H6);
                     break;
                 case TX_LINKLIST_H7:
                     pktSize_Amsdu = &(DataPathVari.pktSize_H7);
                     break;
             }

             *pktSize_Amsdu=(u4Byte)(((*pktSize_Amsdu)+1)%maxLength);
             if(*pktSize_Amsdu<8){
                *pktSize_Amsdu=8;
             }
             *pktSize=*pktSize_Amsdu; 
             
         }
         else if(lengthopt ==RANDOM)
             *pktSize=0; //1 ((u4Byte) rand()%maxLength + 1);
         else if(lengthopt ==FIXED) 
         {

            if(length==LENGTH_256)
                *pktSize=(u4Byte)gTxLen;
            else if(length==LENGTH_512)
                *pktSize=(u4Byte)512;
            else
                *pktSize=(u4Byte)1343; 

            //use the user defined packet length
            if(0!=fix_pkt_len)
                *pktSize = fix_pkt_len;
        }        
        //*pktSize=(DWORD)966;//1024
        //*pktSize=(DWORD)454;//512
        //*pktSize=(DWORD)6;//64
         if((*pktSize+58)%512==0) 
            *pktSize=8;
         
        // if(*pktSize==0) *pktSize=1;
         
         (*SeqNum)++;
         
     }

    switch(Queue)
    {
        case TX_LINKLIST_VO:  TID=0x6;      break;
        case TX_LINKLIST_VI:  TID=0x4;      break;
        case TX_LINKLIST_BE:  TID=0x0;      break;
        case TX_LINKLIST_BK:  TID=0x1;      break;
        case TX_LINKLIST_MAG:  TID=0x0;     break;
        case TX_LINKLIST_H0:
        case TX_LINKLIST_H1:
        case TX_LINKLIST_H2:
        case TX_LINKLIST_H3:
        case TX_LINKLIST_H4:
        case TX_LINKLIST_H5:
        case TX_LINKLIST_H6:
        case TX_LINKLIST_H7:
            TID=0x0;     
        break;
    }

    databuf[0]=(*pktSize)/256;    
    databuf[1]=(*pktSize)%256;
    databuf[2]=payloadopt;
    databuf[3]=payload;
    databuf[4]=TID;
    databuf[5]=length;
    databuf[6]=0x99;
    databuf[7]=0x99;
/*
    for (i=0; i<8; i++) {
        dprintf("0x%02x \n",databuf[i]);
    }
    dprintf("\n");
*/
    if(payloadopt ==FIXED)
    {
 //       dprintf("pkt len %d\n",*pktSize);
        memset(&databuf[8],payload_pattern[payload],*pktSize);  
    }
    else if(payloadopt ==RANDOM)
    {
        if((*pktSize)>8)
        {
            for(i=8;i<(*pktSize);i++)
                databuf[i]=0;//1 rand()%256;
        }
    }
    else if(payloadopt ==INCREASING)
    {
        u4Byte Start=*pktSize;
        if(SerialPayload_Init==FALSE)//Generate the increasing buffer if nessisary
        {
            SerialPayload_Init=TRUE;
            for( i=0;i<20000;i++)
            {
                SerialPayload[i]=(u1Byte)i;
            }
        }
        memcpy(&databuf[8],&SerialPayload[Start],(*pktSize-8));
    }

    if((*SeqNum)==HPoint+1)  
        *SeqNum=LPoint; 

    
}

/*
u4Byte 
TxFrameCheck(
    u1Byte *TxPKTBuffer,
    u4Byte PKTLength,
    PORT_INFO *PortInfo)// TODO: TxShareInfo is changed to another type PORT_INFO_UI
{

    TX_DESC curDesc;
    WlanHeaderInfo_11n  WlanHeader;
    UCHAR ExtendLen=0;
    DWORD bRet=0;
    BYTE  TxExtendDescLen;
    BOOL        USB_EARLY_MODE= UiSetting.MacSettingGen.tx_discriptor.early_mode_support;
    BOOL        QOS=UiSetting.MacSettingGen.source_data.qos_support;
    

    TxExtendDescLen=(USB_EARLY_MODE==TRUE)?(sizeof(TX_DESC)+8):sizeof(TX_DESC); 
    ExtendLen=(QOS==TRUE)?2:0;

    ASSERT(TxPKTBuffer!=NULL);//    TODO: Why Assert when not NULL?
    memcpy(&curDesc,TxPKTBuffer,sizeof(TX_DESC));
    memcpy(&WlanHeader,TxPKTBuffer+TxExtendDescLen,WLAN_HEADER_LENGTH+ExtendLen);
    ASSERT(&curDesc!=NULL);
    ASSERT(&WlanHeader!=NULL);

    // Check Descriptor:
    if(curDesc.offset!=TxExtendDescLen)
        bRet=0x01;

    if(PKTLength!=TxExtendDescLen+curDesc.txpktsize)
        bRet=0x02;

    if(PKTLength>MAXBUFFER)
        bRet=0x02;
#if 0   
    if(Info->TX_Enable==0)
            bRet=0x03;
#endif  
    if(memcmp(&WlanHeader.Address1_1,PortInfo->address_info.des_addr,0x06)!=0)
            bRet=0x03;      
#if 0
    if(!MultiDest){
        if(memcmp(&WlanHeader.Address2_1,&Info->MacAddr,0x06))
            bRet=0x04;
    }
#endif  

    if( curDesc.seq>4095)
        bRet=0x05;

    return bRet;
}
*/

VOID
TxSetPktOption(
    VOID
)
{

    u4Byte i;
    
    memset(&MacSettingGen, 0, sizeof(MacSettingGen));
    memset(&MacSettingGen, 0, sizeof(MacSettingGen));
    
    /*===================MAC Setting-General===================*/
    
    /*source data*/
    MacSettingGen.source_data.ht_support=FALSE;
    MacSettingGen.source_data.qos_support=TRUE;
    MacSettingGen.source_data.small_pkt_size=FALSE;
    MacSettingGen.source_data.IsBcn=FALSE;
    MacSettingGen.source_data.IsDlForFw=FALSE;        
    for(i=0;i<5;i++)
    {
        MacSettingGen.source_data.packet[i].traffic_support = TRUE;
        MacSettingGen.source_data.packet[i].lengthopt = FIXED;//INCREASING;
        MacSettingGen.source_data.packet[i].length = LENGTH_256;    
        MacSettingGen.source_data.packet[i].payloadopt = INCREASING;
        MacSettingGen.source_data.packet[i].payload = PAYLOAD_3E;
        MacSettingGen.source_data.packet[i].rateopt = FIXED;
        MacSettingGen.source_data.packet[i].queue = QUEUE_HIGH;
        MacSettingGen.source_data.packet[i].rate = RATE_MCS7;      
    }

    for(i=5;i<13;i++)
    {
        MacSettingGen.source_data.packet[i].traffic_support = TRUE;
        MacSettingGen.source_data.packet[i].lengthopt = FIXED;
        MacSettingGen.source_data.packet[i].length = LENGTH_256;    
        MacSettingGen.source_data.packet[i].payloadopt = FIXED;
        MacSettingGen.source_data.packet[i].payload = PAYLOAD_55;
        MacSettingGen.source_data.packet[i].rateopt = FIXED;
        MacSettingGen.source_data.packet[i].queue = QUEUE_HIGH;
        MacSettingGen.source_data.packet[i].rate = RATE_MCS7;      
    }        

    /*test option*/
    MacSettingGen.test_option.nonstop_tx=TRUE;
    MacSettingGen.test_option.magic_pkt=FALSE;
    MacSettingGen.test_option.patten_match_pkt=FALSE;
    MacSettingGen.test_option.continuous_rx=TRUE;
    MacSettingGen.test_option.lb_mode=MAC_Delay_LB;
    MacSettingGen.test_option.link_type = NONE_LINK_TYPE;
    MacSettingGen.test_option.rf_path= path_A; 
    MacSettingGen.test_option.multi_dest = FALSE;
    MacSettingGen.test_option.rx_crc_chk = FALSE;
    MacSettingGen.test_option.rx_irq_num = num_0;
    MacSettingGen.test_option.rx_seq_chk = TRUE;
    MacSettingGen.test_option.tx_multi_thread = TRUE;
    
    /*tx discriptor*/
    MacSettingGen.tx_discriptor.tx_agg_support = FALSE;
    MacSettingGen.tx_discriptor.rdg_enable= FALSE;
    MacSettingGen.tx_discriptor.amsdu_support= FALSE;
    MacSettingGen.tx_discriptor.ampdu_support= TRUE;
    MacSettingGen.tx_discriptor.msdu_frag= FALSE;
    MacSettingGen.tx_discriptor.early_mode_support= FALSE;
    MacSettingGen.tx_discriptor.ampdu_density=7;
    MacSettingGen.tx_discriptor.rx_agg_mode=RX_AGG_NO_AGG;
    MacSettingGen.tx_discriptor.cts2self_support=FALSE;
    MacSettingGen.tx_discriptor.rts_support= FALSE;
    MacSettingGen.tx_discriptor.data_short= FALSE;
    MacSettingGen.tx_discriptor.data_bw= FALSE;
    MacSettingGen.tx_discriptor.rts_short= FALSE;
    MacSettingGen.tx_discriptor.rts_bw= FALSE;
    MacSettingGen.tx_discriptor.short_gi= FALSE;
    MacSettingGen.tx_discriptor.retry_enable= FALSE;
    MacSettingGen.tx_discriptor.retry_limit=0;
    MacSettingGen.tx_discriptor.agg_num=3;
    MacSettingGen.tx_discriptor.use_max_len = FALSE;
    MacSettingGen.tx_discriptor.max_agg_num=15;
    MacSettingGen.tx_discriptor.max_length= len_8k;
    MacSettingGen.tx_discriptor.tx_desc_check_sum= TRUE;


    /*checksum offload*/
    MacSettingGen.checksum_offload.ip_version=ip_ver_v4;
    MacSettingGen.checksum_offload.protocol=protocol_tcp;
    MacSettingGen.checksum_offload.tx_opt=trx_opt_hw;
    MacSettingGen.checksum_offload.rx_opt=trx_opt_hw;

    /*Sounding PTCL*/
    MacSettingGen.sounding_ptcl.rate=sounding_rate_24;


/*===================MAC Setting-2 ports===================*/
    
    //2Port1 Dest0
    /*in used connection*/
    for (i=0; i<8; i++) {
        MacSettingPort.Port[Port_1][i].use_connect=TRUE ;
        MacSettingPort.Port[Port_1][i].TIMIeOffset = 0;

        /*address info*/
        MacSettingPort.Port[Port_1][i].address_info.cast_type =BROADCAST;
        memset(MacSettingPort.Port[Port_1][i].address_info.bssid, 0, 6);
        MacSettingPort.Port[Port_1][i].address_info.bssid[0]=i;
        memset(MacSettingPort.Port[Port_1][i].address_info.des_addr, 0, 6);
        MacSettingPort.Port[Port_1][i].address_info.des_addr[5]=0x02;
        memset(MacSettingPort.Port[Port_1][i].address_info.src_addr, 0, 6);
        MacSettingPort.Port[Port_1][i].address_info.src_addr[5]=0x02;

        /*security info*/
        MacSettingPort.Port[Port_1][i].encryption_info.type=TYPE_NONE;
        MacSettingPort.Port[Port_1][i].encryption_info.encryption=HW;
        MacSettingPort.Port[Port_1][i].encryption_info.decryption=HW;
        MacSettingPort.Port[Port_1][i].encryption_info.keyid=KEY0;
    }
    WlTxSegThreshold = 4096;
    WlExTxSegThreshold = 32;
    gEnableExTXBD = 1;
    TempCounter = 1;
//        memcpy(MacSettingPort.CAMData, CAMData_Security, CAM_DATA_SIZE*sizeof(u4Byte));

}

VOID
ReplaceTxPktOption(
    IN  u1Byte InputRate,
    IN  u1Byte QueueType
)
{
    u4Byte i;

    for(i=0;i<5;i++)
    {
        MacSettingGen.source_data.packet[i].queue = (DMA_QUEUE) QueueType;
        MacSettingGen.source_data.packet[i].rate = (DATA_RATE) InputRate;      
    }
    
}


VOID
TxDataPathInit(
    VOID
)
{
    DataPathVari.bFirstFrag_BE = FALSE;
    DataPathVari.bFirstFrag_BK = FALSE;
    DataPathVari.bFirstFrag_VI= FALSE;
    DataPathVari.bFirstFrag_VO = FALSE;
    DataPathVari.bFirstFrag_MG = FALSE;
    DataPathVari.bLastFrag_BE = FALSE;
    DataPathVari.bLastFrag_BK = FALSE;
    DataPathVari.bLastFrag_VI = FALSE;
    DataPathVari.bLastFrag_VO = FALSE;
    DataPathVari.bLastFrag_MG = FALSE;
    DataPathVari.nFrag_BE = 0;
    DataPathVari.nFrag_BK = 0;
    DataPathVari.nFrag_VI = 0;
    DataPathVari.nFrag_VO = 0;
    DataPathVari.nFrag_MG = 0;
    DataPathVari.pktSize_BE = 0;
    DataPathVari.pktSize_BK = 0;
    DataPathVari.pktSize_VI = 0;
    DataPathVari.pktSize_VO = 0;
    DataPathVari.pktSize_MG = 0;
    DataPathVari.SeqNum[0] = 0;
    DataPathVari.SeqNum[1] = 0;
    DataPathVari.SeqNum[2] = 0;    
    DataPathVari.SeqNum[3] = 0;
    DataPathVari.SeqNum[4] = 0;

}

VOID 
ExtractAddr(
    IN  u1Byte *header,
    IN  u1Byte *SA, 
    IN  u1Byte *TA, 
    IN  u1Byte *DA
)
{
	int i;
	unsigned char status;
	status = header[1] & 0x03; // Check To DS and From DS
	if (status == 0)
	{
		for (i=0; i<6; i++)
		{
			DA[i] = header[i+4];
			SA[i] = header[i+10];
			TA[i] = SA[i];
		}
	}
	else if (status == 1)
	{
		for (i=0; i<6; i++)
		{
			DA[i] = header[i+16];
			SA[i] = header[i+10];
			TA[i] = SA[i];
		}
	}
	else if (status == 2)
	{
		for (i=0; i<6; i++)
		{
			DA[i] = header[i+4];
			SA[i] = header[i+16];
			TA[i] = header[i+10];
		}
	}
	else
	{
		for (i=0; i<6; i++)
		{
			DA[i] = header[i+16];
			SA[i] = header[i+24];
			TA[i] = header[i+10];
		}
	}
}


VOID 
MicPrep(
    IN  u1Byte  *MPDU_orig,
    IN  u4Byte  *MPDU, 
    IN  u4Byte  MPDU_Length,
    IN  u1Byte  *SA,
    IN  u1Byte  *DA,
    IN  u1Byte  *priority
)
{
//	unsigned char *MPDUpad;
	int NewLen;
	int i, j;
	unsigned char   MPDUpad[3000];


//	MPDUpad = MPDUpad;
	j = 0;
	for (i=0; i<16; i++)
	{
		if (i < 6)
		MPDUpad[i] = DA[j];
		else if ((i >=6) && (i<12))
		MPDUpad[i] = SA[j];
		else
		MPDUpad[i] = priority[j];
		j = (j+1)%6;
	}
	memcpy(MPDUpad+16, MPDU_orig, MPDU_Length);
	/*
	for (i = 0; i < MPDU_Length; i++)
	{
	MPDUpad[i+16] = MPDU_orig[i];
	} */

	NewLen = (MPDU_Length+16)/4 + 2;

	if (MPDU_Length % 4 == 0)
	{
		j = 0;
		for (i=0; i < NewLen-2; i++)
		{
			MPDU[i] = Mk32(Mk16(MPDUpad[j+3], MPDUpad[j+2]), Mk16(MPDUpad[j+1], MPDUpad[j]));
			j +=4;
		}
		MPDU[NewLen-2] = Mk32(Mk16(0x0, 0x0), Mk16(0x0, 0x5a));
		MPDU[NewLen-1] = Mk32(Mk16(0x0, 0x0), Mk16(0x0, 0x0));
	}
	else if (MPDU_Length % 4 == 1)
	{
		j = 0;
		for (i=0; i < NewLen-2; i++)
		{
			MPDU[i] = Mk32(Mk16(MPDUpad[j+3], MPDUpad[j+2]), Mk16(MPDUpad[j+1], MPDUpad[j]));
			j +=4;
		}
		MPDU[NewLen-2] = Mk32(Mk16(0x0, 0x0), Mk16(0x5a, MPDUpad[j]));
		MPDU[NewLen-1] = Mk32(Mk16(0x0, 0x0), Mk16(0x0, 0x0));
	}
	else if (MPDU_Length % 4 == 2)
	{
		j = 0;
		for (i=0; i < NewLen-2; i++)
		{
			MPDU[i] = Mk32(Mk16(MPDUpad[j+3], MPDUpad[j+2]), Mk16(MPDUpad[j+1], MPDUpad[j]));
			j +=4;
		}
		MPDU[NewLen-2] = Mk32(Mk16(0x0, 0x5a), Mk16(MPDUpad[j+1], MPDUpad[j]));
		MPDU[NewLen-1] = Mk32(Mk16(0x0, 0x0), Mk16(0x0, 0x0));
	}
	else if (MPDU_Length % 4 == 3)
	{
		
		j = 0;
		for (i=0; i < NewLen-2; i++)
		{
			MPDU[i] = Mk32(Mk16(MPDUpad[j+3], MPDUpad[j+2]), Mk16(MPDUpad[j+1], MPDUpad[j]));
			j +=4;
		}

		MPDU[NewLen-2] = Mk32(Mk16(0x5a, MPDUpad[j+2]), Mk16(MPDUpad[j+1], MPDUpad[j]));
		MPDU[NewLen-1] = Mk32(Mk16(0x0, 0x0), Mk16(0x0, 0x0));

	}


}


VOID 
Michael(
    IN  u4Byte *Key,
    IN  u4Byte *MPDU, 
    IN  u4Byte *micOut,
    IN  u4Byte MPDU_Length
)
{
	u4Byte i;

	micOut[0] = Key[0];
	micOut[1] = Key[1];

	for (i=0; i < MPDU_Length; i++)
	{
		micOut[0] = micOut[0] ^ MPDU[i];

        //1 Need modify again
		//b(micOut);
	}

}


/* AddCount: 1 or 2. 
 *  If overflow, return 1,
 *  else return 0.
 */
BOOLEAN 
WapiIncreasePN(
    IN  u1Byte *PN, 
    IN  u1Byte AddCount
)
{
    u1Byte  i;

    for (i=0; i<16; i++)
    {
        if (PN[i] + AddCount <= 0xff)
        {
            PN[i] += AddCount;
            return 0;
        }
        else
        {
            PN[i] += AddCount;
            AddCount = 1;
        }
    }

    return 1;
}



BOOLEAN
DmaBeacon(
    IN  u1Byte      BssidNum,
    IN  BOOLEAN     EnSwDownload
)
{

    TX_LINK_LIST_ELE *LLEle; 
    u4Byte     Value32 = 0;
    u2Byte     Value16 = 0;
    u1Byte      BssidIndex = 0;
    u1Byte      Value8;
    
    MacSettingGen.source_data.IsBcn=TRUE;

    for (BssidIndex=0; BssidIndex<BssidNum; BssidIndex++) {
        LLEle = (TX_LINK_LIST_ELE*)(((u4Byte)&BecStackLLEle[BssidIndex])|0xa0000000);

        LLEle->PortId = 0;
        LLEle->PortDestNum = BssidIndex;//root
        SetupBcnTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->BcnqTXBD[BssidIndex]),
                         LLEle);
        if (!BssidIndex) {            
            dprintf("==== Initial Root MBSSID Setup ====\n");
            HALWLInInitMbssid(1,0,0);
        }
        else {
            dprintf("==== Initial VAP%d MBSSID Setup ====\n",BssidIndex);
            HALWLInInitMbssid(0,BssidIndex,0);
        }

    }

    if (EnSwDownload) {
        if (1 == BssidNum) {
            HALWLSwBcnCfg();

            dprintf("Sw Pool Beacon\n");
            //Poll Beacon 
            Value16 = HAL_RTL_R16(REG_RX_RXBD_NUM);

            Value16 |= BIT_BCNQ_FLAG;

            HAL_RTL_W16(REG_RX_RXBD_NUM, Value16);
        }
        else {
            dprintf("There is some error in MBSSID for enable Sw beacon\n");
        }
    }

    dprintf("==== Initial Beacon Config ====\n");
    HALWLApModePwrInit();

    return TRUE;
}



u4Byte 
DmaPktForFw(
   IN   u4Byte      PayloadLen,
   IN   u1Byte      *Payload
)
{
    Port_id             Port = Port_1;
    u1Byte                  Dest =0;
    PORT_INFO           *PortInfo;
    PACKET_SETTING_UI   *PktSet;
    TXBD_ELEMENT*       TxBdEle;
    u4Byte                 PKTLen = 0;
    u4Byte                 Value32 = 0;
    u2Byte                 Value16 = 0;
    u1Byte                  *Packet;
    u1Byte                  Index = 0;
    u2Byte                 PsbLen = 0;
    u2Byte                 PageNum = 0;
    u2Byte                 AligementCheck = 0;


    MacSettingGen.source_data.IsDlForFw=TRUE;
   
    HALWLSwBcnCfg();

    /*Decide the Port and Destination according to the link type*/
    //For multi port
    Port= 0;//PortGen();
    Dest =0;//DestSel(Port);
    
    /*assign the local pointer to global data structure*/
    PortInfo = &(MacSettingPort.Port[Port][Dest]);    

    TxBdEle = &hci_dma_manager->BcnqTXBD[0];
    Packet = TxDescBufForFW;

    /*Generate Packet*/
    TxPktGen((u1Byte*) Payload, PayloadLen, 
             (u1Byte*) Packet, (u4Byte*)&(PKTLen),
             0, (PORT_INFO*) PortInfo, Port , Dest);

    RTL_TX_BCN_DBG_LOG("TX Desc and Packet hander: Len %d====>\n",PKTLen);
    //DumpBuff (PKTLen, (u1Byte*)Packet);

    RTL_TX_BCN_DBG_LOG("TX payload: Len %d ====>\n",PayloadLen);
    //DumpBuff (PayloadLen, (u1Byte*)Payload);




    for (Index=0; Index<4; Index++) {
        TxBdEle[Index].Dword0 = 0;        
        TxBdEle[Index].AddrLow = 0;
    }


    Value32 = CPUTOLEX32(TOPHY_ADDRESS((u4Byte)(Packet)));
    TxBdEle[0].AddrLow = Value32;
    
    AligementCheck = ((PKTLen + PayloadLen)&0xFF);
    PageNum = ((PKTLen + PayloadLen) >> 8);
    PageNum = (AligementCheck > 0)? (PageNum+1):PageNum;
    
    Value32 = ((PageNum<<16) | (PKTLen));

    if (MacSettingGen.source_data.IsDlForFw) {
        Value32|=0x80000000;
    }
    Value32 = CPUTOLEX32(Value32);

    TxBdEle[0].Dword0 = Value32;

    //4 Set Payload to TXBD entry 1            
    Value32 = CPUTOLEX32(TOPHY_ADDRESS((u4Byte)(Payload)));
    TxBdEle[1].AddrLow = Value32;

    
    Value32 = CPUTOLEX32(PayloadLen);
    TxBdEle[1].Dword0 = Value32;

    if (Value32 != TxBdEle[1].Dword0) {
        dprintf("ERROR!!!!!!!\n");
    }
    
    volatile u4Byte *gCheck;
    gCheck = &(TxBdEle[3].Dword0);
    Value32 = *gCheck;


    dprintf("Sw Pool Beacon\n");
    //Poll Beacon 
    Value16 = HAL_RTL_R16(REG_RX_RXBD_NUM);

    Value16 |= BIT_BCNQ_FLAG;

    HAL_RTL_W16(REG_RX_RXBD_NUM, Value16);


    MacSettingGen.source_data.IsDlForFw=FALSE;
    return TRUE;
}



BOOLEAN
SendPacket(
    IN  u4Byte              LoopMode, 
    IN  TX_LINKLIST_ID      Queue,
    IN  TX_LINK_LIST_ELE*   LLEle
)
{
    u2Byte     WritePoint = 0;
    u2Byte     ReadPoint = 0;
    u2Byte     PsbLen = 0;
    u2Byte     PageNum = 0;
    u2Byte     AligementCheck = 0;
    u4Byte     dump_index = 0;
    u2Byte     arry_index = 0;
    u4Byte     Value32 = 0;


    MacSettingGen.source_data.IsBcn= FALSE;

    if (LoopMode) {
        HALWLDmaLBCfg();
    }
    else {
        HALWLNormallCfg();
    }


    LLEle->PortId = 0;
    LLEle->PortDestNum = 0;

    Generate_Packet(Queue, (TX_LINK_LIST_ELE*) LLEle);           

    RTL_TX_DBG_LOG("TX Desc and Packet hander: Len %d====>\n",LLEle->TXData.PKTLen);
//    DumpBuff (LLEle->TXData.PKTLen, (u1Byte*)LLEle->TXData.Packet);

    RTL_TX_DBG_LOG("TX payload: Len %d ====>\n",LLEle->TXData.PayloadLen);
//    DumpBuff (LLEle->TXData.PayloadLen, (u1Byte*)LLEle->TXData.Payload);

    switch (Queue)
    {
        case TX_LINKLIST_VO:
            Value32 = HAL_RTL_R32(REG_VOQ_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);           
            RTL_TX_DBG_LOG("++++> VOTX wp: %d; rp: %d\n", WritePoint, ReadPoint);

            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_VOQ_DESC_NUM)) {

                RTL_TX_DBG_LOG("====VO TX====\n");
                if (!gEnableExTXBD) {
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->VoqTXBD[WritePoint]),
                              LLEle);
                }
                else {
                    SetupTxPacketForExtendTXBD((TXBD_ELEMENT*)(&hci_dma_manager->VoqTXBD[WritePoint]),
                        (EXTEND_TXBD_ELEMENT_MANAGER*) (&hci_dma_manager->ExVoqTXBD[WritePoint][0]),
                        LLEle, MAX_TXBD_SEQMENT_NUM);
                }
                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_VOQ_DESC_NUM-1);
                
                //Set write point
                HAL_RTL_W8(REG_VOQ_TXBD_IDX,WritePoint);

                Value32 = HAL_RTL_R32(REG_VOQ_TXBD_IDX);
                ReadPoint = (u2Byte)(Value32 >> 16);
                WritePoint = (u2Byte)(Value32 & 0xFFFF);
                RTL_TX_DBG_LOG("----> VOTX wp: %d; rp: %d\n", WritePoint, ReadPoint);

            }
            else {
//                dprintf("NO VO TX Number\n");
                return FALSE;
            }

            break;
        case TX_LINKLIST_VI:
            Value32 = HAL_RTL_R32(REG_VIQ_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF); 

            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_VIQ_DESC_NUM)) {
                RTL_TX_DBG_LOG("====VI TX====\n");

                if (!gEnableExTXBD) {
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->ViqTXBD[WritePoint]),
                               LLEle);
                }
                else {
                    SetupTxPacketForExtendTXBD((TXBD_ELEMENT*)(&hci_dma_manager->ViqTXBD[WritePoint]),
                        (EXTEND_TXBD_ELEMENT_MANAGER*) (&hci_dma_manager->ExViqTXBD[WritePoint][0]),
                        LLEle, MAX_TXBD_SEQMENT_NUM);
                }

                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_VIQ_DESC_NUM-1);
  
                //Set write point
                HAL_RTL_W8(REG_VIQ_TXBD_IDX,WritePoint);

                Value32 = HAL_RTL_R32(REG_VIQ_TXBD_IDX);
                ReadPoint = (u2Byte)(Value32 >> 16);
                WritePoint = (u2Byte)(Value32 & 0xFFFF);
                RTL_TX_DBG_LOG("----> VITX RX wp: %d; rp: %d\n", WritePoint, ReadPoint);
                
            }
            else {
             //   dprintf("NO VI TX Number\n");
                return FALSE;
            }

            break;
        case TX_LINKLIST_BE:
            Value32 = HAL_RTL_R32(REG_BEQ_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  

            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_BEQ_DESC_NUM)) {
                RTL_TX_DBG_LOG("====BE TX====\n");
                if (!gEnableExTXBD) {
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->BeqTXBD[WritePoint]),
                               LLEle);
                }
                else {
                    SetupTxPacketForExtendTXBD((TXBD_ELEMENT*)(&hci_dma_manager->BeqTXBD[WritePoint]),
                        (EXTEND_TXBD_ELEMENT_MANAGER*) (&hci_dma_manager->ExBeqTXBD[WritePoint][0]),
                        LLEle, MAX_TXBD_SEQMENT_NUM);
                }

                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_BEQ_DESC_NUM-1);

                //Set write point
                HAL_RTL_W8(REG_BEQ_TXBD_IDX,WritePoint);
            }
            else {
            //    dprintf("NO BE TX Number\n");
                return FALSE;
            }

           
            break;
        case TX_LINKLIST_BK:
            Value32 = HAL_RTL_R32(REG_BKQ_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  

            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_BKQ_DESC_NUM)) {
                RTL_TX_DBG_LOG("====BK TX====\n");
                
                if (!gEnableExTXBD) {
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->BkqTXBD[WritePoint]),
                               LLEle);
                }
                else {
                    SetupTxPacketForExtendTXBD((TXBD_ELEMENT*)(&hci_dma_manager->BkqTXBD[WritePoint]),
                        (EXTEND_TXBD_ELEMENT_MANAGER*) (&hci_dma_manager->ExBkqTXBD[WritePoint][0]),
                        LLEle, MAX_TXBD_SEQMENT_NUM);
                }


                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_BKQ_DESC_NUM-1);

                //Set write point
                HAL_RTL_W8(REG_BKQ_TXBD_IDX,WritePoint);
            }
            else {
            //    dprintf("NO BK TX Number\n");
                return FALSE;
            }
            break;
#if 1       
        case TX_LINKLIST_MAG:
            Value32 = HAL_RTL_R32(REG_MGQ_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  

            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_MGQ_DESC_NUM)) {
                RTL_TX_DBG_LOG("====MAG TX====\n");
                
                if (!gEnableExTXBD) {
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->MgqTXBD[WritePoint]),
                               LLEle);
                }
                else {
                    SetupTxPacketForExtendTXBD((TXBD_ELEMENT*)(&hci_dma_manager->MgqTXBD[WritePoint]),
                        (EXTEND_TXBD_ELEMENT_MANAGER*) (&hci_dma_manager->ExMgqTXBD[WritePoint][0]),
                        LLEle, MAX_TXBD_SEQMENT_NUM);
                }


                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_MGQ_DESC_NUM-1);

                //Set write point
                HAL_RTL_W8(REG_MGQ_TXBD_IDX,WritePoint);
            }
            else {
            //    dprintf("NO MG TX Number\n");
                return FALSE;
            }
            break;        
#endif

        default:
            break;
    }

    return TRUE;
}

BOOLEAN
SendHighQueuePacket(
    IN  TX_LINKLIST_ID      Queue,
    IN  TX_LINK_LIST_ELE*   LLEle
)
{
    u2Byte     WritePoint = 0;
    u2Byte     ReadPoint = 0;
    u2Byte     PsbLen = 0;
    u2Byte     PageNum = 0;
    u2Byte     AligementCheck = 0;
    u4Byte     dump_index = 0;
    u2Byte     arry_index = 0;
    u4Byte     Value32 = 0;
    u1Byte      TempQueue;

    TempQueue = Queue;

    TempQueue -= 5;

    MacSettingGen.source_data.IsBcn= FALSE;

    HALWLNormallCfg();

    LLEle->PortId = 0;
    LLEle->PortDestNum = TempQueue;

    dprintf("Port Dest num: %d\n",LLEle->PortDestNum);
    
    Generate_Packet(Queue, (TX_LINK_LIST_ELE*) LLEle);           

    RTL_TX_DBG_LOG("TX Desc and Packet hander: Len %d====>\n",LLEle->TXData.PKTLen);
//    DumpBuff (LLEle->TXData.PKTLen, (u1Byte*)LLEle->TXData.Packet);

    RTL_TX_DBG_LOG("TX payload: Len %d ====>\n",LLEle->TXData.PayloadLen);
//    DumpBuff (LLEle->TXData.PayloadLen, (u1Byte*)LLEle->TXData.Payload);

    switch (Queue)
    {        
        case TX_LINKLIST_H0:
            Value32 = HAL_RTL_R32(REG_HI0Q_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  

            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_H0Q_DESC_NUM)) {
                RTL_TX_DBG_LOG("====High Queue 0 TX====\n");
                
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->H0qTXBD[WritePoint]),
                               LLEle);

                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_H0Q_DESC_NUM-1);

                //Set write point
                HAL_RTL_W8(REG_HI0Q_TXBD_IDX,WritePoint);
            }
            else {
                dprintf("NO High Queue 0 TX Number\n");
                return FALSE;
            }
            break;
        case TX_LINKLIST_H1:
            Value32 = HAL_RTL_R32(REG_HI1Q_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  
        
            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_H1Q_DESC_NUM)) {
                RTL_TX_DBG_LOG("====High Queue 1 TX====\n");
                
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->H1qTXBD[WritePoint]),
                               LLEle);
        
                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_H1Q_DESC_NUM-1);
        
                //Set write point
                HAL_RTL_W8(REG_HI1Q_TXBD_IDX,WritePoint);
            }
            else {
                dprintf("NO High Queue 1 TX Number\n");
                return FALSE;
            }
            break;

        case TX_LINKLIST_H2:
            Value32 = HAL_RTL_R32(REG_HI2Q_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  
        
            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_H2Q_DESC_NUM)) {
                RTL_TX_DBG_LOG("====High Queue 2 TX====\n");
                
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->H2qTXBD[WritePoint]),
                               LLEle);
        
                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_H2Q_DESC_NUM-1);
        
                //Set write point
                HAL_RTL_W8(REG_HI2Q_TXBD_IDX,WritePoint);
            }
            else {
                dprintf("NO High Queue 2 TX Number\n");
                return FALSE;
            }
            break;

        case TX_LINKLIST_H3:
            Value32 = HAL_RTL_R32(REG_HI3Q_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  
        
            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_H3Q_DESC_NUM)) {
                RTL_TX_DBG_LOG("====High Queue 3 TX====\n");
                
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->H3qTXBD[WritePoint]),
                               LLEle);
        
                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_H3Q_DESC_NUM-1);
        
                //Set write point
                HAL_RTL_W8(REG_HI3Q_TXBD_IDX,WritePoint);
            }
            else {
                dprintf("NO High Queue 3 TX Number\n");
                return FALSE;
            }
            break;

        case TX_LINKLIST_H4:
            Value32 = HAL_RTL_R32(REG_HI4Q_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  
        
            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_H4Q_DESC_NUM)) {
                RTL_TX_DBG_LOG("====High Queue 4 TX====\n");
                
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->H4qTXBD[WritePoint]),
                               LLEle);
        
                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_H4Q_DESC_NUM-1);
        
                //Set write point
                HAL_RTL_W8(REG_HI4Q_TXBD_IDX,WritePoint);
            }
            else {
                dprintf("NO High Queue 4 TX Number\n");
                return FALSE;
            }
            break;

        case TX_LINKLIST_H5:
            Value32 = HAL_RTL_R32(REG_HI5Q_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  
        
            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_H5Q_DESC_NUM)) {
                RTL_TX_DBG_LOG("====High Queue 5 TX====\n");
                
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->H5qTXBD[WritePoint]),
                               LLEle);
        
                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_H5Q_DESC_NUM-1);
        
                //Set write point
                HAL_RTL_W8(REG_HI5Q_TXBD_IDX,WritePoint);
            }
            else {
                dprintf("NO High Queue 5 TX Number\n");
                return FALSE;
            }
            break;

        case TX_LINKLIST_H6:
            Value32 = HAL_RTL_R32(REG_HI6Q_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  
        
            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_H6Q_DESC_NUM)) {
                RTL_TX_DBG_LOG("====High Queue 6 TX====\n");
                
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->H6qTXBD[WritePoint]),
                               LLEle);
        
                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_H6Q_DESC_NUM-1);
        
                //Set write point
                HAL_RTL_W8(REG_HI6Q_TXBD_IDX,WritePoint);
            }
            else {
                dprintf("NO High Queue 0 TX Number\n");
                return FALSE;
            }
            break;

        case TX_LINKLIST_H7:
            Value32 = HAL_RTL_R32(REG_HI7Q_TXBD_IDX);
            ReadPoint = (u2Byte)(Value32 >> 16);
            WritePoint = (u2Byte)(Value32 & 0xFFFF);  
        
            if (CheckAvailableTXNum(ReadPoint,WritePoint,TX_H7Q_DESC_NUM)) {
                RTL_TX_DBG_LOG("====High Queue 7 TX====\n");
                
                SetupTxPacket((TXBD_ELEMENT*)(&hci_dma_manager->H7qTXBD[WritePoint]),
                               LLEle);
        
                //Update Write Point
                WritePoint = (WritePoint + 1)&(TX_H7Q_DESC_NUM-1);
        
                //Set write point
                HAL_RTL_W8(REG_HI7Q_TXBD_IDX,WritePoint);
            }
            else {
                dprintf("NO High Queue 7 TX Number\n");
                return FALSE;
            }
            break;
        
            
        default:
            break;
    }

    return TRUE;
}



VOID
SetupTxPacket(
    IN  TXBD_ELEMENT*           TxBdEle,
    IN  TX_LINK_LIST_ELE*       LLEle
)
{
    u2Byte     PsbLen = 0;
    u2Byte     PageNum = 0;
    u2Byte     AligementCheck = 0;
    u2Byte     Index;
    u4Byte     Value32;
    u4Byte     TotalLength;



    RTL_TX_DBG_LOG("TXBD 0 address: 0x%x\n",&TxBdEle[0]);
    RTL_TX_DBG_LOG("TXBD 1 address: 0x%x\n",&TxBdEle[1]);
    RTL_TX_DBG_LOG("TXBD 2 address: 0x%x\n",&TxBdEle[2]);
    RTL_TX_DBG_LOG("TXBD 3 address: 0x%x\n",&TxBdEle[3]);
    //Reset TXBD
    for (Index=0; Index<MAX_TXBD_SEQMENT_NUM; Index++) {
        TxBdEle[Index].Dword0 = 0;        
        TxBdEle[Index].AddrLow = 0;
    }


    RTL_TX_DBG_LOG("Seg0 address: 0x%x\n",(LLEle->TXData.Packet));
    Value32 = CPUTOLEX32(TOPHY_ADDRESS((u4Byte)(LLEle->TXData.Packet)));
    TxBdEle[0].AddrLow = Value32;
    
    AligementCheck = ((LLEle->TXData.PKTLen +LLEle->TXData.PayloadLen)&0xFF);

    if (HAL_RTL_R32(REG_SYS_CFG1) & BIT(23)) {    

        TotalLength= (LLEle->TXData.PKTLen +LLEle->TXData.PayloadLen);//divide 256
    Value32 = ((TotalLength<<16) | (LLEle->TXData.PKTLen));
    
    }
    else {        
    PageNum = ((LLEle->TXData.PKTLen +LLEle->TXData.PayloadLen) >> 8);//divide 256
    PageNum = (AligementCheck > 0)? (PageNum+1):PageNum;
    
    Value32 = ((PageNum<<16) | (LLEle->TXData.PKTLen));
    }

    RTL_TX_DBG_LOG("Seg0 Len: 0x%x\n",Value32);
    if (MacSettingGen.source_data.IsBcn) {
        Value32|=0x80000000;
    }
    Value32 = CPUTOLEX32(Value32);

    TxBdEle[0].Dword0 = Value32;

    //4 Set Payload to TXBD entry 1            
    RTL_TX_DBG_LOG("Seg1 address: 0x%x\n",(LLEle->TXData.Payload));
    Value32 = CPUTOLEX32(TOPHY_ADDRESS((u4Byte)(LLEle->TXData.Payload)));
    TxBdEle[1].AddrLow = Value32;

    RTL_TX_DBG_LOG("Seg1~3 Len: 0x%x\n",LLEle->TXData.PayloadLen);
    
    if (LLEle->TXData.PayloadLen > WlTxSegThreshold) {
    
        Value32 = CPUTOLEX32(WlTxSegThreshold);
        RTL_TX_DBG_LOG("Seg1 Len: 0x%x\n",WlTxSegThreshold);
        TxBdEle[1].Dword0 = Value32;
    
        Value32 = CPUTOLEX32(TOPHY_ADDRESS((u4Byte)(LLEle->TXData.Payload + WlTxSegThreshold)));
        RTL_TX_DBG_LOG("Seg2 address: 0x%x\n",LLEle->TXData.Payload + WlTxSegThreshold);
        TxBdEle[2].AddrLow = Value32;
    
    
        if (LLEle->TXData.PayloadLen > (WlTxSegThreshold*2)) {
    
            Value32 = CPUTOLEX32(WlTxSegThreshold);
            RTL_TX_DBG_LOG("Seg2 Len: 0x%x\n",WlTxSegThreshold);
            TxBdEle[2].Dword0 = Value32;                    
                            
            Value32 = CPUTOLEX32(TOPHY_ADDRESS((u4Byte)(LLEle->TXData.Payload + 
                                                       (WlTxSegThreshold*2))));
            RTL_TX_DBG_LOG("Seg3 address: 0x%x\n",LLEle->TXData.Payload + 
                                              (WlTxSegThreshold*2));
    
            TxBdEle[3].AddrLow = Value32;
                            

#if 1
            if (4 < MAX_TXBD_SEQMENT_NUM) {
                if (LLEle->TXData.PayloadLen > (WlTxSegThreshold*3)) {
                    Value32 = CPUTOLEX32(WlTxSegThreshold);
                    RTL_TX_DBG_LOG("Seg3 Len: 0x%x\n",WlTxSegThreshold);
                    TxBdEle[3].Dword0 = Value32;                    
                                    
                    Value32 = CPUTOLEX32(TOPHY_ADDRESS((u32)(LLEle->TXData.Payload + 
                                                               (WlTxSegThreshold*3))));
                    RTL_TX_DBG_LOG("Seg4 address: 0x%x\n",LLEle->TXData.Payload + 
                                                      (WlTxSegThreshold*3));
                    
                    TxBdEle[4].AddrLow = Value32;
                    
                    if (LLEle->TXData.PayloadLen > (WlTxSegThreshold*4)) {
                        Value32 = CPUTOLEX32(WlTxSegThreshold);
                        RTL_TX_DBG_LOG("Seg4 Len: 0x%x\n",WlTxSegThreshold);
                        TxBdEle[4].Dword0 = Value32;                    
                                        
                        Value32 = CPUTOLEX32(TOPHY_ADDRESS((u32)(LLEle->TXData.Payload + 
                                                                   (WlTxSegThreshold*4))));
                        RTL_TX_DBG_LOG("Seg5 address: 0x%x\n",LLEle->TXData.Payload + 
                                                          (WlTxSegThreshold*4));
                        
                        TxBdEle[5].AddrLow = Value32;                   

                        if (LLEle->TXData.PayloadLen > (WlTxSegThreshold*5)) {
                            Value32 = CPUTOLEX32(WlTxSegThreshold);
                            RTL_TX_DBG_LOG("Seg5 Len: 0x%x\n",WlTxSegThreshold);
                            TxBdEle[5].Dword0 = Value32;                    
                                            
                            Value32 = CPUTOLEX32(TOPHY_ADDRESS((u32)(LLEle->TXData.Payload + 
                                                                       (WlTxSegThreshold*5))));
                            RTL_TX_DBG_LOG("Seg6 address: 0x%x\n",LLEle->TXData.Payload + 
                                                              (WlTxSegThreshold*5));
                            
                            TxBdEle[6].AddrLow = Value32;                   

                            if (LLEle->TXData.PayloadLen > (WlTxSegThreshold*6)) {
                                Value32 = CPUTOLEX32(WlTxSegThreshold);
                                RTL_TX_DBG_LOG("Seg6 Len: 0x%x\n",WlTxSegThreshold);
                                TxBdEle[6].Dword0 = Value32;                    
                                                
                                Value32 = CPUTOLEX32(TOPHY_ADDRESS((u32)(LLEle->TXData.Payload + 
                                                                           (WlTxSegThreshold*6))));
                                RTL_TX_DBG_LOG("Seg7 address: 0x%x\n",LLEle->TXData.Payload + 
                                                                  (WlTxSegThreshold*6));
                                
                                TxBdEle[7].AddrLow = Value32;   

                                Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen - (WlTxSegThreshold*6));
                                RTL_TX_DBG_LOG("Seg7 Len: 0x%x\n",LLEle->TXData.PayloadLen - (WlTxSegThreshold*6));
                                TxBdEle[7].Dword0 = Value32;                        

                                if (Value32 != TxBdEle[7].Dword0) {
                                    RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
                                }                            
                            }
                            else {
                                Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen - (WlTxSegThreshold*5));
                                RTL_TX_DBG_LOG("Seg6 Len: 0x%x\n",LLEle->TXData.PayloadLen - (WlTxSegThreshold*5));
                                TxBdEle[6].Dword0 = Value32;                        
                                
                                if (Value32 != TxBdEle[6].Dword0) {
                                    RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
                            }                          }
                            
                        }
                        else {
                            Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen - (WlTxSegThreshold*4));
                            RTL_TX_DBG_LOG("Seg5 Len: 0x%x\n",LLEle->TXData.PayloadLen - (WlTxSegThreshold*4));
                            TxBdEle[5].Dword0 = Value32;                        
                            
                            if (Value32 != TxBdEle[5].Dword0) {
                                RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
                            }                           
                        }

                        
                    }
                    else {
                        Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen - (WlTxSegThreshold*3));
                        RTL_TX_DBG_LOG("Seg4 Len: 0x%x\n",LLEle->TXData.PayloadLen - (WlTxSegThreshold*3));
                        TxBdEle[4].Dword0 = Value32;                        
                        
                        if (Value32 != TxBdEle[4].Dword0) {
                            RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
                        }     

                    }
                    


                }
                else {
            Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen - (WlTxSegThreshold*2));
                    RTL_TX_DBG_LOG("Seg3 Len: 0x%x\n",LLEle->TXData.PayloadLen - (WlTxSegThreshold*2));
            TxBdEle[3].Dword0 = Value32;                        

            if (Value32 != TxBdEle[3].Dword0) {
                        RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
                    }                
                }
            }
            else {
                Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen - (WlTxSegThreshold*2));
                RTL_TX_DBG_LOG("Seg3 Len: 0x%x\n",LLEle->TXData.PayloadLen - (WlTxSegThreshold*2));
                TxBdEle[3].Dword0 = Value32;                        

                if (Value32 != TxBdEle[3].Dword0) {
                    RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
                }
            }
#else
            Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen - (WlTxSegThreshold*2));
            RTL_TX_DBG_LOG("Seg3 Len: 0x%x\n",LLEle->TXData.PayloadLen - (WlTxSegThreshold*2));
            TxBdEle[3].Dword0 = Value32;                        

            if (Value32 != TxBdEle[3].Dword0) {
                RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
            }
#endif
        }                            
        else {                        
            Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen - WlTxSegThreshold);
            RTL_TX_DBG_LOG("Seg2 Len: 0x%x\n",LLEle->TXData.PayloadLen - WlTxSegThreshold);
            TxBdEle[2].Dword0 = Value32;                    
            if (Value32 != TxBdEle[2].Dword0) {
                RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
            }
        }

    }
    else {
        Value32 = CPUTOLEX32(LLEle->TXData.PayloadLen);
        RTL_TX_DBG_LOG("Seg1 Len: 0x%x\n",LLEle->TXData.PayloadLen);
        TxBdEle[1].Dword0 = Value32;

        if (Value32 != TxBdEle[1].Dword0) {
            RTL_TX_DBG_LOG("ERROR!!!!!!!\n");
        }
    }
    
    volatile u4Byte *gCheck;
    gCheck = &(TxBdEle[3].Dword0);
    Value32 = *gCheck;
    
}


VOID
SetupTxPacketForExtendTXBD(
    IN  TXBD_ELEMENT*                   TxBdEle,
    IN  EXTEND_TXBD_ELEMENT_MANAGER*    ExTxbdMan,
    IN  TX_LINK_LIST_ELE*               LLEle,
    IN  u4Byte                          MaxTxBdNum
)
{
    u2Byte     PsbLen = 0;
    u2Byte     PageNum = 0;
    u2Byte     AligementCheck = 0;
    u2Byte     Index;
    u4Byte     Value32;
    u4Byte     TempAddr;
    u4Byte     TotalLen;
    u4Byte     RemainLen;
    u4Byte     TxbdLen;
    u4Byte     TxbdAddr;
    u1Byte      ExTxbdNum[MAX_TXBD_SEQMENT_NUM];
    u1Byte      ExSegIndex;
    u1Byte      TxBdEntryIndex;
    u1Byte      i;
    u4Byte     TotalLength;

    memset(ExTxbdNum, 0x0, MAX_TXBD_SEQMENT_NUM);
    RTL_EX_DBG_LOG("TXBD 0 address: 0x%x\n",&TxBdEle[0]);
    RTL_EX_DBG_LOG("TXBD 1 address: 0x%x\n",&TxBdEle[1]);
    RTL_EX_DBG_LOG("TXBD 2 address: 0x%x\n",&TxBdEle[2]);
    RTL_EX_DBG_LOG("TXBD 3 address: 0x%x\n",&TxBdEle[3]);
    RTL_EX_DBG_LOG("TXBD 4 address: 0x%x\n",&TxBdEle[4]);
    RTL_EX_DBG_LOG("TXBD 5 address: 0x%x\n",&TxBdEle[5]);
    RTL_EX_DBG_LOG("TXBD 6 address: 0x%x\n",&TxBdEle[6]);
    RTL_EX_DBG_LOG("TXBD 7 address: 0x%x\n",&TxBdEle[7]);

    for (i=0; i<MaxTxBdNum; i++) {    
        for (Index=0; Index<MAX_EXTEND_TXBD_NUM; Index++) {
//            RTL_EX_DBG_LOG("TXBD[%d]; Extend TXBD[%d] Address:0x%x\n",i,Index,&ExTxbdMan[i].TXBD_ELE[Index]);
            ExTxbdMan[i].TXBD_ELE[Index].Dword0 = 0;
            ExTxbdMan[i].TXBD_ELE[Index].AddrLow = 0;
        }
    }
    //Reset TXBD
    for (Index=0; Index<MaxTxBdNum; Index++) {
        TxBdEle[Index].Dword0 = 0;        
        TxBdEle[Index].AddrLow = 0;
    }


    RTL_EX_DBG_LOG("Seg0 address: 0x%x\n",(LLEle->TXData.Packet));
    Value32 = CPUTOLEX32(TOPHY_ADDRESS((u4Byte)(LLEle->TXData.Packet)));
    TxBdEle[0].AddrLow = Value32;
    
    AligementCheck = ((LLEle->TXData.PKTLen +LLEle->TXData.PayloadLen)&0xFF);

    //Test Chip is different with MP chip
    if (HAL_RTL_R32(REG_SYS_CFG1) & BIT(23)) {    

        TotalLength= (LLEle->TXData.PKTLen +LLEle->TXData.PayloadLen);//divide 256
    Value32 = ((TotalLength<<16) | (LLEle->TXData.PKTLen));
    }
    else {
    PageNum = ((LLEle->TXData.PKTLen +LLEle->TXData.PayloadLen) >> 8);//divide 256
    PageNum = (AligementCheck > 0)? (PageNum+1):PageNum;
    
    Value32 = ((PageNum<<16) | (LLEle->TXData.PKTLen));
    }
    
    RTL_EX_DBG_LOG("Seg0 Len: 0x%x\n",Value32);
    Value32 = CPUTOLEX32(Value32);
    TxBdEle[0].Dword0 = Value32;


    TxBdEntryIndex = 1;
    ExSegIndex = 0;

    TotalLen = LLEle->TXData.PayloadLen;
    RemainLen = TotalLen;
    TxbdAddr = (u4Byte)LLEle->TXData.Payload;
    RTL_EX_DBG_LOG("Payload Total Length: %d\n", TotalLen);
    RTL_EX_DBG_LOG("Payload Address: 0x%x\n", TxbdAddr);

    while(RemainLen > 0) {

        if (RemainLen > WlExTxSegThreshold) {
            //4 TXBD Packet Length
            TxbdLen = WlExTxSegThreshold;
            RemainLen -= WlExTxSegThreshold;
            //4 Next TXBD Packet Address
            TempAddr = TxbdAddr - WlExTxSegThreshold;
        }
        else {
            //4 TXBD Packet Length
            TxbdLen = RemainLen;            
            RemainLen -= TxbdLen;            
        }

        RTL_EX_DBG_LOG("Txbd index: %d; Extend index:%d; Addr:0x%x; Len:%d\n",
                        TxBdEntryIndex, ExSegIndex, TxbdAddr, TxbdLen);
        ExTxbdMan[TxBdEntryIndex].TXBD_ELE[ExSegIndex].AddrLow = CPUTOLEX32(TOPHY_ADDRESS(TxbdAddr));
        ExTxbdMan[TxBdEntryIndex].TXBD_ELE[ExSegIndex].Dword0 = CPUTOLEX32(TxbdLen);
        //4 Update the number of the extend TXBD
        ExTxbdNum[TxBdEntryIndex]++;

        ExSegIndex++;

        if (MAX_EXTEND_TXBD_NUM == ExSegIndex) {
            //4 Update TXBD entry 
            TxBdEntryIndex++;
            ExSegIndex = 0;

            if (MaxTxBdNum == TxBdEntryIndex) {
                //4 Check if there are remain packet which didn't set to TXBD
                if (RemainLen) {
                    //4 Update the last TXBD again for the last packet
                    ExTxbdMan[MaxTxBdNum-1].TXBD_ELE[MAX_EXTEND_TXBD_NUM-1].Dword0 = CPUTOLEX32(RemainLen + WlExTxSegThreshold);
                    RTL_EX_DBG_LOG("The remain length: %d\n", RemainLen + WlExTxSegThreshold);
                    RemainLen = 0;
                }
            }
        }

        //4 Update TXBD Packet Address
        TxbdAddr = TempAddr;
    }

    for (Index=1; Index<MaxTxBdNum; Index++) {
        if (ExTxbdNum[Index]) {
            //4 Set Length and Extend bit
            RTL_EX_DBG_LOG("Extend Txbd number: %d\n",ExTxbdNum[Index]);
            TxbdLen = (ExTxbdNum[Index]<<3) | 0x80000000;// |(TempCounter<<16);
            TxbdAddr = (u4Byte)(&ExTxbdMan[Index]);
            RTL_EX_DBG_LOG("TxBD[%d] Recorder Address: 0x%x\n",Index,TxbdAddr);
            RTL_EX_DBG_LOG("TxBD[%d] Transfer Len: 0x%x\n",Index,TxbdLen);
            TxBdEle[Index].AddrLow = CPUTOLEX32(TOPHY_ADDRESS(TxbdAddr));
            TxBdEle[Index].Dword0 = CPUTOLEX32(TxbdLen);

    }
    }

    volatile u4Byte *gCheck;
    gCheck = &(TxBdEle[3].Dword0);
    Value32 = *gCheck;
    
    TempCounter++;
    
}


VOID
SetupBcnTxPacket(
    IN  TXBD_ELEMENT*           TxBdEle,
    IN  TX_LINK_LIST_ELE*       LLEle
)
{
    Generate_Packet(0, (TX_LINK_LIST_ELE*) LLEle);

    RTL_TX_BCN_DBG_LOG("TX Desc and Packet hander: Len %d====>\n",LLEle->TXData.PKTLen);
    DumpBuff (LLEle->TXData.PKTLen, (u1Byte*)LLEle->TXData.Packet);

    RTL_TX_BCN_DBG_LOG("TX payload: Len %d ====>\n",LLEle->TXData.PayloadLen);
    DumpBuff (LLEle->TXData.PayloadLen, (u1Byte*)LLEle->TXData.Payload);

    SetupTxPacket(TxBdEle, LLEle);

    
}


u2Byte
CheckAvailableTXNum (
    IN  u2Byte Rpoint,
    IN  u2Byte Wpoint,
    IN  u4Byte MaxNum
)
{
    u2Byte UsingNum, AvailableNum;

    UsingNum = (Wpoint - Rpoint + MaxNum) & (MaxNum - 1);
    AvailableNum = (MaxNum - 1) - UsingNum;

    return AvailableNum;
}


VOID
DumpBuff (
    IN  u4Byte Len,
    IN  u1Byte* pBuf
)
{
    u4Byte dump_index, arry_index;

    arry_index = 0;
    
    for (dump_index = 0;dump_index<Len; dump_index++) {
        dprintf("0x%02x ",pBuf[dump_index]);
        arry_index++;
        if (arry_index == 4) {
            arry_index = 0;
            dprintf("\n");
        }
    }
    
}



BOOLEAN
PollingReceivePacket(
    IN  u4Byte WaitTime
)
{
    u4Byte Value32 = 0, RepeatCount = 0, MaxwaitTime;
    u2Byte WritePoint, ReadPoint;
    u1Byte SegCheck;
    BOOLEAN Result = TRUE;

    RepeatCount = WaitTime;
    WritePoint = 0;
    ReadPoint = 0;
        
    while(WaitTime) {

        
//        if (HAL_RTL_R32(HCI_DMA_ISR)& 0x1) {
            Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
            WritePoint = (u2Byte)(Value32 >> 16);
            ReadPoint = (u2Byte)(Value32 & 0xFFFF);
    //        dprintf("==>Start RX wp: %d; rp: %d\n", WritePoint, ReadPoint);         

            Value32 = CPUTOLEX32(hci_dma_manager->RXBD[ReadPoint].Dword0);

            SegCheck = (u1Byte) ((Value32 >> 14) & 0x3);

            if (WritePoint != ReadPoint) {
//                dprintf("Receive Packet\n");
                if (!ReceivePacket(ReadPoint)) {
                    dprintf("Receive Packet Error: Stop RX\n");
                    Result = FALSE;
                    WaitTime = 0;
                    dprintf("==>Start RX wp: %d; rp: %d\n", WritePoint, ReadPoint);            }
                else {
                    Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
                    WritePoint = (u2Byte)(Value32 >> 16);
                    ReadPoint = (u2Byte)(Value32 & 0xFFFF);
                    dprintf("Packet OK\n");
                    dprintf("+==> RX wp: %d; rp: %d\n", WritePoint, ReadPoint);
                    if ((SegCheck == 1)||(SegCheck == 3)) {
                        WaitTime = 0;
                    }
                }
                //Clear RX ISR
//                HAL_RTL_W32(HCI_DMA_ISR,(HAL_RTL_R32(HCI_DMA_ISR)&0x1));
            }
            else {
                RepeatCount = RepeatCount - 1;
                __delay(100000);
    //            dprintf ("%d No Recevie Packet!!\n",RepeatCount);
            }

            if (!RepeatCount) {
                Result = FALSE;
                dprintf("repeat error!!\n");
            }

        if (WaitTime >0)
            WaitTime--;
        
    }

    return Result;
}


BOOLEAN
WLIsrReceivePacket(
    VOID
)
{
    u4Byte Value32 = 0, RepeatCount = 0, MaxwaitTime;
    u2Byte WritePoint, ReadPoint,GapNum, LoopIndex, Input;
    BOOLEAN Result = TRUE;
    
    WritePoint = 0;
    ReadPoint = 0;
            
            
    Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
    WritePoint = (u2Byte)(Value32 >> 16);
    ReadPoint = (u2Byte)(Value32 & 0xFFFF);
    RTL_RX_DBG_LOG("==>Start RX wp: %d; rp: %d\n", WritePoint, ReadPoint);         
    GapNum = (WritePoint - ReadPoint + RX_Q_DESC_NUM) & (RX_Q_DESC_NUM - 1);
    
    Value32 = CPUTOLEX32(hci_dma_manager->RXBD[ReadPoint].Dword0);
    
    Input = ReadPoint;
    
    if (WritePoint != ReadPoint) {
        RTL_RX_DBG_LOG("Receive Packet num: %d\n", GapNum);
        for (LoopIndex=0; LoopIndex<GapNum; LoopIndex++) {
            
            if (!ReceivePacket(Input)) {
                dprintf("Receive Packet Error: Stop RX\n");
                Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
                WritePoint = (u2Byte)(Value32 >> 16);
                ReadPoint = (u2Byte)(Value32 & 0xFFFF);
                dprintf("==>RX wp: %d; rp: %d\n", WritePoint, ReadPoint);
                Result = FALSE;
            }
            else {
                Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
                WritePoint = (u2Byte)(Value32 >> 16);
                ReadPoint = (u2Byte)(Value32 & 0xFFFF);
                RTL_RX_DBG_LOG("Packet OK\n");
                RTL_RX_DBG_LOG("+==> RX wp: %d; rp: %d\n", WritePoint, ReadPoint);
                Input = ((Input + 1)&(RX_Q_DESC_NUM-1));
            }
        }
            
    }
    else {
        dprintf ("No Packet in buffer!!\n");
    }

    WlRxResult = Result;
    return Result;

}


BOOLEAN
ReceivePacket(
    IN  u2Byte ReadPoint
)
{
    u4Byte Value32, Address, *pBuf, *pDword;
    u2Byte RxBuffSize, TotalPktSize, RxAggNum, LoopIndex, 
        MaxFreeNum, RxIndex, RxTag, ConfirmIndex;
    u1Byte SegCheck, IsFinish;
    BOOLEAN Result;

    IsFinish = 0;
    Result = TRUE;
    ConfirmIndex = 0;

    
#if 1
    //4 Check Rx Tag
    while (!IsFinish) {
        Value32 = CPUTOLEX32(hci_dma_manager->RXBD[ReadPoint].Dword0);

        RxTag = (u2Byte) ((Value32 >> 16) & 0xFFFF);

        ConfirmIndex++;

        if (RxTag == hci_dma_manager->RxExpectTag) {
            hci_dma_manager->RxExpectTag++;
            hci_dma_manager->RxExpectTag &= 0x1FFF;
            IsFinish = 1;
            RTL_RX_DBG_LOG("RX ready; Rx Tag: %d; Read Point: %d\n", RxTag, ReadPoint);
        }

        if (ConfirmIndex > 10000) {
            IsFinish = 1;
            Result = FALSE;
        }
    }

    if (!Result) {
        dprintf("RX DMA Error: Can't Read Rx data ; Rx Tag: %d; Sw Rx Tag: %d; Read point: %d\n",
                   RxTag, hci_dma_manager->RxExpectTag,ReadPoint );
        return FALSE;
    }
#else
    Value32 = CPUTOLEX32(hci_dma_manager->RXBD[ReadPoint].Dword0);
#endif

    SegCheck = (u1Byte) ((Value32 >> 14) & 0x3);

    RTL_RX_DBG_LOG("FS and LS Status: %d\n",SegCheck);

    Address = (CPUTOLEX32(hci_dma_manager->RXBD[ReadPoint].PhyAddr) | 0x80000000);

    RTL_RX_DBG_LOG("RX Buf address: 0x%x\n", Address);
    
    pDword = (u4Byte*) Address;

    //4 For singal packet/first segment, Check CRC32 is correct
    if (SegCheck & 0x2) {
        if (GET_DESC((u4Byte)(*pDword),RX_DW0_CRC32_MSK,RX_DW0_CRC32_SH)) {
            dprintf("ERROR: CRC Error!!!\n");
            return FALSE;
        }

#if CONFIG_WLMAC_8051_SUPPORT
        //4 Check C2H Packet

    
    if (GET_DESC((u4Byte)(*(pDword+2)),0x1,RX_DW2_RSVD28_30_SH)) {
        dprintf("====================\n");        
        dprintf("Receive C2H packet\n");
        dprintf("C2H content = [%x]\n",(*(pDword+6)));
        dprintf("C2H content = [%x]\n",(*(pDword+7)));
        dprintf("C2H content = [%x]\n",(*(pDword+8)));        
        dprintf("====================\n");                
            //4 Compare C2H content
            
        
            //4 Update RXBD Index
            Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
            ReadPoint = (u2Byte)(Value32 & 0xFFFF);
            Value32 = ((Value32 & (~0xFFFF)) | ((ReadPoint + 1)&(RX_Q_DESC_NUM-1)));
            HAL_RTL_W32(REG_RXQ_RXBD_IDX,Value32);        
            return TRUE;
        }
#endif //#if CONFIG_WLMAC_8051_SUPPORT

    }    
    
    //SegCheck: 
    //        0     : Segemet packet
    //        1     : Last segement packet
    //        2     : Fisrt segement packet
    //        3     : Single packet
    if ((SegCheck == 0) || (SegCheck == 2)) {
        //4 Set Seg packet flag
        hci_dma_manager->RxSegFlow = 1;
        
        //4 Mantain Rx Segement Buffer address
        RxAggNum = hci_dma_manager->RxAggregateNum;

        hci_dma_manager->RxAggBufEntry[RxAggNum] = Address;

        hci_dma_manager->RxAggregateNum = hci_dma_manager->RxAggregateNum + 1;
        
        //4 Allocate new rx buffer
#if 0
        if (!GetAvaliableRXBuff(&RxIndex)) {
            dprintf("Can't Find avaliable rx buffer!!!");
            return FALSE;
        }
        dprintf("rx index %d\n",RxIndex);

        Value32 = (((u4Byte)&(RxBufEntry[RxIndex].RxBuf))|0xa0000000);        

        //4 Initial to RXBD
        dprintf("New Rx buffer address: 0x%x\n",Value32);
        Value32 = CPUTOLEX32(TOPHY_ADDRESS(Value32));
        hci_dma_manager->RXBD[ReadPoint].PhyAddr = Value32;
#endif        
    }
    else {

        //Handle Single Packet or Last seqement packet

#if 1

        //4 Update Last segement packet to record array            
        //4 Mantain Rx Segement Buffer address
        RxAggNum = hci_dma_manager->RxAggregateNum;
        
        hci_dma_manager->RxAggBufEntry[RxAggNum] = Address;
        
        hci_dma_manager->RxAggregateNum = hci_dma_manager->RxAggregateNum + 1;
        
        //4 Check multi-segement packet CRC32
        if (!CheckMultiSegPacket()) {
            hci_dma_manager->RxSegFlow = 0;
            hci_dma_manager->RxAggregateNum = 0;            
            return FALSE;
        }
//        Result = CheckMultiSegPacket();

#if 0
        if (!GetAvaliableRXBuff(&RxIndex)) {
            dprintf("Can't Find avaliable rx buffer!!!");
            return FALSE;
        }
        dprintf("rx index %d\n",RxIndex);
        
        Value32 = (((u4Byte)&(RxBufEntry[RxIndex].RxBuf))|0xa0000000);        

        //4 Initial to RXBD
        dprintf("New Rx buffer address: 0x%x\n",Value32);
        Value32 = CPUTOLEX32(TOPHY_ADDRESS(Value32));
        hci_dma_manager->RXBD[ReadPoint].PhyAddr = Value32;
#endif 


#if 0
        //4 Free Rx buffer 
        if (hci_dma_manager->RxSegFlow) {
            dprintf("Free RX Buff\n");
            MaxFreeNum = hci_dma_manager->RxAggregateNum-1; //Can't free the last buffer
            for (LoopIndex=0; LoopIndex<MaxFreeNum; LoopIndex++) {
//                osk_free(hci_dma_manager->RxAggBufEntry[LoopIndex]);
                if(!FreeRXBuff(hci_dma_manager->RxAggBufEntry[LoopIndex])) {
                    dprintf("Free RX Buffer Fail!!");
                    return FALSE;
                }
            }
        }
#endif        
#else
        if (hci_dma_manager->RxSegFlow) {
            //Update Last segement packet to record array            
            //4 Mantain Rx Segement Buffer address
            RxAggNum = hci_dma_manager->RxAggregateNum;

            hci_dma_manager->RxAggBufEntry[RxAggNum] = Address;

            hci_dma_manager->RxAggregateNum = hci_dma_manager->RxAggregateNum + 1;

            //4 Check multi-segement packet CRC32
            Result = CheckMultiSegPacket();
        }
        else {
            //4 Check single packet CRC32 without segement
            Result = CheckSinglePacket((u1Byte*) Address);            
        }
#endif
        //4 Clear Seg packet flag and aggregate number

        u1Byte i;

        for (i=0; i<hci_dma_manager->RxAggregateNum; i++) {                
            hci_dma_manager->RxAggBufEntry[i] = 0;
        }
        hci_dma_manager->RxSegFlow = 0;
        hci_dma_manager->RxAggregateNum = 0;

    }
#if 0
    Value32 = 0;
    Value32 = CPUTOLEX32(Value32 | RX_BUF_LEN);        
    hci_dma_manager->RXBD[ReadPoint].Dword0 = Value32;
    
    pBuf = (u4Byte*) Address;
#endif
//    PktLen = (u2Byte)(CPUTOLEX32((u4Byte)*pBuf) & 0x3FFF);
#if 0
    //4 Update RXBD Index
    Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
    
    Value32 = ((Value32 & (~0xFFFF)) | ((ReadPoint + 1)&(RX_Q_DESC_NUM-1)));
    
    HAL_RTL_W32(REG_RXQ_RXBD_IDX,Value32); 
#endif

//    if (Result)
        return TRUE;
//    else
//        return FALSE;
}


BOOLEAN
CheckSinglePacket(
    IN  u1Byte  *pDataBuf
)
{
    DATA_OPTION     LengthOpt;
    DATA_LENGTH     Length;
    DATA_OPTION     PayloadOpt;
    DATA_PAYLOAD    Payload;
    DATA_RATE       Rate;
    u4Byte PktSize, MacheaderLen, FirstPayload;
    u4Byte Value32, *pDword, OutCrc32;
    u2Byte PktLen;
    u1Byte *pOutCrc32, ByteIndex;

    pOutCrc32 = (u1Byte*)&OutCrc32;

    pDataBuf = (u1Byte*)((u4Byte)pDataBuf | 0xA0000000);
    pDword = (u4Byte *) pDataBuf;

#if 0    
    if ( GET_DESC((u4Byte)(*(pDataBuf+4)),RX_DW2_FCS_OK_MSK,RX_DW2_FCS_OK_SH)) {
        dprintf("FCS Error!!!\n");
        return FALSE;
    }
#endif

    PktLen = GET_DESC((u4Byte)(*pDword),RX_DW0_PKT_LEN_MSK,RX_DW0_PKT_LEN_SH);
    
    MacheaderLen = (GET_DESC((u4Byte)(*pDword),RX_DW0_QOS_MSK,RX_DW0_QOS_SH))?
                    (WLAN_HEADER_LENGTH + 2): WLAN_HEADER_LENGTH;

    dprintf("PktLen = %d\n",PktLen);
    dprintf("MacheaderLen = %d\n",MacheaderLen);

    FirstPayload = MacheaderLen + SIZE_RX_DESC_LEN;

    Length = pDataBuf[FirstPayload + 5];
//    dprintf("%d; 0x%x\n",pDataBuf[FirstPayload], &pDataBuf[FirstPayload]);
//    dprintf("%d\n",PatternLen[Length]);


    PktSize = pDataBuf[FirstPayload]*256 + 
              pDataBuf[FirstPayload + 1];

    PayloadOpt = pDataBuf[FirstPayload + 2];
    Payload = pDataBuf[FirstPayload + 3];

    __delay(1);
//    dprintf("Payload Length: %d\n",PktSize);
//    dprintf("Payload Option: %d\n",PayloadOpt);
//    dprintf("Payload Pattern: 0x%x\n",payload_pattern[Payload]);
//    dprintf("FirstPayload: %d\n",FirstPayload);

//    DumpBuff((PktSize+MacheaderLen),(u1Byte*)(pDataBuf+SIZE_RX_DESC_LEN));
/*
    if (FIXED == PayloadOpt) {
        if (payload_pattern[Payload] != pDataBuf[FirstPayload + PktSize - 1]) {
            dprintf("Address: 0x%x\n",&pDataBuf[FirstPayload + PktSize - 1]);
            dprintf("Error: Pattern Check Value 0x%02x != 0x%02x\n",
                payload_pattern[Payload] ,pDataBuf[FirstPayload + PktSize]);
        }
        else {
        }
    }
*/



    //Genert Check CRC Value
    SoftwareCrc32((u1Byte*)(pDataBuf+SIZE_RX_DESC_LEN),PktSize+MacheaderLen,(u1Byte*)pOutCrc32);

    for (ByteIndex = 0;ByteIndex<4; ByteIndex++) {

        if (pDataBuf[FirstPayload + PktSize + ByteIndex] != pOutCrc32[3-ByteIndex]) {
            dprintf("ERROR: [%d] Caculate CRC= 0x%02x; RXCRC= 0x%02x\n",
                    ByteIndex, pOutCrc32[3-ByteIndex],
                    pDataBuf[FirstPayload + PktSize + ByteIndex]);
            
            return FALSE;
        }
        
//        dprintf("[%d] Caculate CRC= 0x%02x; RXCRC= 0x%02x\n",
//                ByteIndex,pOutCrc32[3-ByteIndex],
//                pDataBuf[FirstPayload + PktSize + ByteIndex]);
    }

    
    return TRUE;    

}


VOID 
SoftwareCrc32 (
    IN  u1Byte      *in,
    IN  u2Byte      byte_num,
    IN  u1Byte      *out
)
{
    u4Byte a,b;
    u1Byte mask,smask;
    u4Byte CRCMask = 0x00000001, POLY = 0xEDB88320;
    u4Byte CRC_32 = 0xffffffff;
    u4Byte i,j;

    smask=0x01;
    for(i=0;i<byte_num;i++)
    {
        mask=smask;
        for(j=0;j<8;j++)
        {
            a=((CRC_32&CRCMask)!=(unsigned long)0);
            b=((in[i]&mask)!=(unsigned long)0);

            CRC_32 >>= 1;
            mask<<=1;

            if(a^b)
            CRC_32 ^= POLY;
        }
    }
    *((unsigned long *)out)=CRC_32^(unsigned long)0xffffffff;

}

BOOLEAN 
CheckMultiSegPacket (
    VOID
)
{
    u4Byte a,b;
    u1Byte mask,smask, *in, *pOutCrc32, ByteIndex;
    u4Byte CRCMask = 0x00000001, POLY = 0xEDB88320;
    u4Byte CRC_32 = 0xffffffff;
    u4Byte i,j, Address, *pDword;
    u2Byte LoopIndex,MaxAggNum;
    u4Byte MacheaderLen,OutCrc32;
    u2Byte PktLen, byte_num;
    u4Byte FirstPartLen,SecPartLen;
    u1Byte  RxPktCrc[4];
    u2Byte RemainIndex, Anotherindex;
    u4Byte Value32;
    u2Byte ReadPoint;

    pOutCrc32 = (u1Byte*)&OutCrc32;

    smask=0x01;
    MaxAggNum = hci_dma_manager->RxAggregateNum;

    RTL_RX_CHECK_DBG_LOG("Segement Max Number %d\n",MaxAggNum);

    for (LoopIndex = 0; LoopIndex < MaxAggNum; LoopIndex++) {

        Address = (hci_dma_manager->RxAggBufEntry[LoopIndex] | 0xa0000000);    
        
        RTL_RX_CHECK_DBG_LOG("Segement %d: RX buffer 0x%x\n",LoopIndex, Address);

        //4 Handle the first segement or single packet
        if (!LoopIndex) {
        
            pDword = (u4Byte *)Address;
            in = (u1Byte *) (Address + SIZE_RX_DESC_LEN);


            PktLen = GET_DESC((u4Byte)(*pDword),RX_DW0_PKT_LEN_MSK,RX_DW0_PKT_LEN_SH);

            RTL_RX_CHECK_DBG_LOG("Total PktLen = %d\n",PktLen); 
                
            MacheaderLen = (GET_DESC((u4Byte)(*pDword),RX_DW0_QOS_MSK,RX_DW0_QOS_SH))?
                            (WLAN_HEADER_LENGTH + 2): WLAN_HEADER_LENGTH;

            hci_dma_manager->RxLen = PktLen;
            hci_dma_manager->RemainLen = PktLen;

            if (PktLen > (RX_BUF_LEN - SIZE_RX_DESC_LEN)) {
                //The number of RX Segment  more than 1
                SecPartLen = PktLen - (RX_BUF_LEN - SIZE_RX_DESC_LEN);

                hci_dma_manager->RemainLen = hci_dma_manager->RemainLen - 
                                             (RX_BUF_LEN - SIZE_RX_DESC_LEN);
                if (SecPartLen > 4) {
                    FirstPartLen = 0;
                    byte_num = RX_BUF_LEN - SIZE_RX_DESC_LEN;                    
                }
                else {
                    //Handle CRC32 bytes
                    FirstPartLen = 4 - SecPartLen;
                    byte_num = RX_BUF_LEN - SIZE_RX_DESC_LEN - FirstPartLen;
                    for (ByteIndex = 0; ByteIndex< FirstPartLen; ByteIndex++) {
                        RxPktCrc[ByteIndex] = in[byte_num + ByteIndex];
                        RTL_RX_CHECK_DBG_LOG("1)Rx Pkt CRC[%d] = 0x%2x\n",ByteIndex, RxPktCrc[ByteIndex]);
                    }

                }

                RTL_RX_CHECK_DBG_LOG("1)First Part len: %d; Second Part Len: %d\n",FirstPartLen, SecPartLen);
            }
            else {
                byte_num = PktLen - 4;
                //Record Rx packet CRC32 bytes
                for (ByteIndex = 0;ByteIndex<4; ByteIndex++) {
                        RxPktCrc[ByteIndex] = in[byte_num + ByteIndex];
                }
            }
            //RTL_RX_CHECK_DBG_LOG("MacheaderLen = %d\n",MacheaderLen);
        }
        else {
            in = (u1Byte *) (Address);

            if (hci_dma_manager->RemainLen > RX_BUF_LEN) {
                //The number of RX Segment  more than 1
                SecPartLen = hci_dma_manager->RemainLen - RX_BUF_LEN;

                hci_dma_manager->RemainLen -= RX_BUF_LEN;
                
                if (SecPartLen > 4) {
                    FirstPartLen = 0;
                    byte_num = RX_BUF_LEN;
                }
                else {
                    //Handle CRC32 bytes
                    FirstPartLen = 4 - SecPartLen;
                    byte_num = RX_BUF_LEN - FirstPartLen;
                    for (ByteIndex = 0; ByteIndex< FirstPartLen; ByteIndex++) {
                        RxPktCrc[ByteIndex] = in[byte_num + ByteIndex];
                        RTL_RX_CHECK_DBG_LOG("1)Rx Pkt CRC[%d] = 0x%2x\n",ByteIndex, RxPktCrc[ByteIndex]);
                    }

                }
            }
            else {
                if (hci_dma_manager->RemainLen < 5) {
                    SecPartLen = hci_dma_manager->RemainLen;
                    FirstPartLen = 4 - SecPartLen;
                    for (ByteIndex = FirstPartLen;ByteIndex<4; ByteIndex++) {
                            RxPktCrc[ByteIndex] = in[ByteIndex - FirstPartLen];
                            RTL_RX_CHECK_DBG_LOG("2)Rx Pkt CRC[%d] = 0x%2x\n",ByteIndex, RxPktCrc[ByteIndex]);
                    }
                    byte_num = 0;
                    RTL_RX_CHECK_DBG_LOG("2)First Part len: %d; Second Part Len: %d\n",FirstPartLen, SecPartLen);
                }
                else {                
                    byte_num = hci_dma_manager->RemainLen - 4;
                    for (ByteIndex = 0;ByteIndex<4; ByteIndex++) {
                        RxPktCrc[ByteIndex] = in[byte_num + ByteIndex];
                    }
                }
            }

        }

        RTL_RX_CHECK_DBG_LOG("PktLen = %d\n",PktLen);        



//        DumpBuff(PktLen,(u1Byte*)(in));              

        __delay(1);
    
        hci_dma_manager->RxAggLenEntry[LoopIndex] = byte_num;

        for(i=0;i<byte_num;i++)
        {
            mask=smask;
            for(j=0;j<8;j++)
            {
                a=((CRC_32&CRCMask)!=(unsigned long)0);
                b=((in[i]&mask)!=(unsigned long)0);
        
                CRC_32 >>= 1;
                mask<<=1;
        
                if(a^b)
                CRC_32 ^= POLY;
            }
        }

    }

#if 1
    //4 Update RXBD Index
    Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
    ReadPoint = (u2Byte)(Value32 & 0xFFFF);
    Value32 = ((Value32 & (~0xFFFF)) | ((ReadPoint + MaxAggNum)&(RX_Q_DESC_NUM-1)));
    HAL_RTL_W32(REG_RXQ_RXBD_IDX,Value32); 
#endif


    *((unsigned long *)pOutCrc32)=CRC_32^(unsigned long)0xffffffff;

    for (ByteIndex = 0;ByteIndex<4; ByteIndex++) {

        if (RxPktCrc[ByteIndex] != pOutCrc32[3-ByteIndex]) {
            dprintf("ERROR: [%d] Caculate CRC= 0x%02x; RXCRC= 0x%02x\n",
                    ByteIndex, pOutCrc32[3-ByteIndex],
                    RxPktCrc[ByteIndex]);

            dprintf("Rx Len: %d; Agg Number: %d \n", hci_dma_manager->RxLen, hci_dma_manager->RxAggregateNum);

            for (i=0; i<MaxAggNum; i++) {                
                dprintf("Segement %d: ;RX buffer 0x%x ; Len: %d\n",i, 
                         hci_dma_manager->RxAggBufEntry[i],hci_dma_manager->RxAggLenEntry[LoopIndex]);
            }
            return FALSE;
        }
        RTL_RX_CHECK_DBG_LOG("[%d] Caculate CRC= 0x%02x; RXCRC= 0x%02x\n",
                ByteIndex,pOutCrc32[3-ByteIndex],
                RxPktCrc[ByteIndex]);
    }

    return TRUE;

}



BOOLEAN 
GetAvaliableRXBuff (
    IN  u2Byte *FreeIndex
)
{
    u2Byte LoopIndex;

    for(LoopIndex=0; LoopIndex<MAX_BUF_NUM; LoopIndex++) {
        if (!RxBufEntry[LoopIndex].IsUsed) {
            *FreeIndex = LoopIndex;
            RxBufEntry[LoopIndex].IsUsed = 1;
            return TRUE;
        }
    }

    return FALSE;
}

BOOLEAN 
FreeRXBuff (
    IN  u4Byte Address
)
{
    u2Byte LoopIndex;

    for(LoopIndex=0; LoopIndex<MAX_BUF_NUM; LoopIndex++) {
        if (((u4Byte)&(RxBufEntry[LoopIndex].RxBuf)) == Address) {
            RxBufEntry[LoopIndex].IsUsed = 0;            
            return TRUE;
        }
    }
    dprintf("Can't Find free address: 0x%x\n",Address);
    return FALSE;
}



BOOLEAN
WLMAC_BIST (
    IN  u4Byte CheckValue
)
{
    u4Byte Value32;
    u4Byte TimeOutCounter, LoopCheckIndex;
    
    HAL_RTL_W8(REG_BIST_CTRL, 0x07); // 1
    HAL_RTL_W8(REG_BIST_CTRL, 0x05); // 2
    HAL_RTL_W8(REG_BIST_CTRL, 0x07); // 3

    
    LoopCheckIndex = 1;
    TimeOutCounter = 0;

    // 4
    while (LoopCheckIndex) {
        Value32 = HAL_RTL_R32(REG_BIST_RPT);
        if (Value32 & BIT(10)) {
            LoopCheckIndex = 0;
        }
        TimeOutCounter++;
        if (TimeOutCounter > 100000) {
            dprintf("Check 0xd4[10] (1) Fail 0x%x\n", Value32);
            return FALSE;
        }
        delay_ms(1);
    }

    dprintf("loop number (1) %d\n",TimeOutCounter);

    // 5
    Value32 = HAL_RTL_R32(REG_BIST_RPT);
    if (Value32 & BIT(9)) {
        dprintf("MAC Bist Fail!!\n");
        return FALSE;
    }

    // 6
    HAL_RTL_W8(REG_BIST_CTRL, 0x17);


    LoopCheckIndex = 1;
    TimeOutCounter = 0;

    // 7
    while (LoopCheckIndex) {
        Value32 = HAL_RTL_R32(REG_BIST_RPT);
        if (Value32 & BIT(10)) {
            LoopCheckIndex = 0;
        }
        TimeOutCounter++;
        if (TimeOutCounter > 100000) {
            dprintf("Check 0xd4[10] (2) Fail\n");
            return FALSE;
        }
        delay_ms(1);
    }    
    dprintf("loop number (2) %d\n",TimeOutCounter);


    // 8
    HAL_RTL_W8(REG_BIST_CTRL, 0x17);

   	//delay_ms(1000);//JSW:original
   delay_ms(10);

    LoopCheckIndex = 1;
    TimeOutCounter = 0;

    // 9
    while (LoopCheckIndex) {
        Value32 = HAL_RTL_R32(REG_BIST_RPT);
        if (Value32 & BIT(8)) {
            LoopCheckIndex = 0;
        }
        TimeOutCounter++;
        if (TimeOutCounter > 100000) {
            dprintf("Check 0xd4[8] Fail\n");
            return FALSE;
        }
        delay_ms(1);
    }    
    dprintf("loop number (3) %d\n",TimeOutCounter);


    // 10
    Value32 = HAL_RTL_R32(REG_BIST_RPT);
    if (Value32 & BIT(9)) {
        dprintf("MAC Bist Fail!!\n");
        return FALSE;
    }


    // 11
    Value32 = HAL_RTL_R32(REG_MBIST_FAIL);
    if (Value32 != CheckValue) { //0xd212
        dprintf("THE Final Check Fail\n");
        return FALSE;
    }
    return TRUE;
    
}

BOOLEAN
WLBB_BIST (
    VOID
)
{
    u4Byte Value32;
    u4Byte TimeOutCounter, LoopCheckIndex;


    HAL_RTL_W32(0x8b4, 0x40);
    HAL_RTL_W32(0x924, 0x8000);
    HAL_RTL_W32(0x9a4, 0x80000);
    HAL_RTL_W32(0x808, 0x30000000);
    HAL_RTL_W32(0xb00, 0x3000100);
    HAL_RTL_W32(0xa00, 0x80904708);
    HAL_RTL_W32(0x8f8, 0x180000);
    HAL_RTL_W32(0x8b4, 0);

    delay_ms(10);
        
    HAL_RTL_W32(0x8f8, 0x380000);
    HAL_RTL_W32(0x8f8, 0x180000);




    LoopCheckIndex = 1;
    TimeOutCounter = 0;

    while (LoopCheckIndex) {
        Value32 = HAL_RTL_R32(0xfec);
        if (Value32 == 0x40000000) {
            LoopCheckIndex = 0;
        }
        TimeOutCounter++;
        if (TimeOutCounter > 100000) {            
            dprintf("Check 0xfec == 0x40000000 Fail\n");
            return FALSE;
        }
        delay_ms(10);
    }   
    dprintf("loop number (1) %d\n",TimeOutCounter);


    HAL_RTL_W32(0x8f8, 0x380000);
    HAL_RTL_W32(0x8f8, 0x180000);

    delay_ms(10);

    while (LoopCheckIndex) {
        Value32 = HAL_RTL_R32(0xfec);
        if (Value32 == 0x80000000) {
            LoopCheckIndex = 0;
        }
        TimeOutCounter++;
        if (TimeOutCounter > 100000) {            
            dprintf("Check 0xfec == 0x80000000 Fail\n");
            return FALSE;
        }
        delay_ms(10);
    } 

    dprintf("loop number (2) %d\n",TimeOutCounter);


    while (LoopCheckIndex) {
        Value32 = HAL_RTL_R32(0xa5c);
        if (Value32 == 0x700000) {
            LoopCheckIndex = 0;
        }
        TimeOutCounter++;
        if (TimeOutCounter > 100000) {            
            dprintf("Check 0xa5c == 0x700000 Fail\n");
            return FALSE;
        }
        delay_ms(10);
    } 
    dprintf("loop number (3) %d\n",TimeOutCounter);

    return TRUE;
}


BOOLEAN
WLTxCont(
    VOID
)
{
    u32 LoopbackNum = 0;
    u32 LoopbackIndex = 0;
    TX_LINKLIST_ID Queue;
    TX_LINK_LIST_ELE *LLEle; 
    u32 AcIDInput;
    u32 WaitTime;
    u32 type = 0, LoopMax, InputACIndex, check, ThreType, TxExtendEn;
    u32 TxLen,LoopMode, TxFailCheckflag, RxTimeArry[16];
    u8 RateIndex = 0, QueueTypeIndex = 3, temp;
    u32 VoBufIndex, ViBufIndex, BeBufIndex, BkBufIndex, MagBufIndex;

    TxFailCheckCounter = 0;
    InputACIndex = 0;
    WlRxResult = TRUE;
    LoopMode = 1;
    VoBufIndex = 0;
    ViBufIndex = 0;
    BeBufIndex = 0;
    BkBufIndex = 0;


//    if (argc<6) {
//        dprintf("Wrong argument number!\r\n");
//        return;
//    }

    AcIDInput = 0;//strtoul((const char*)(argv[0]), (char **)NULL, 16);
    LoopbackNum = 10000;//strtoul((const char*)(argv[1]), (char **)NULL, 16);
    //LoopbackNum = 5000;//strtoul((const char*)(argv[1]), (char **)NULL, 16); //JSW:RTL8881A Wireless MAC test count
    type = 8;//strtoul((const char*)(argv[2]), (char **)NULL, 16);
    ThreType = 8;//strtoul((const char*)(argv[3]), (char **)NULL, 16);
    TxLen = 0;//strtoul((const char*)(argv[4]), (char **)NULL, 16);
    TxExtendEn = 1;//strtoul((const char*)(argv[5]), (char **)NULL, 16);


    dprintf("Tx Payload Len: %d/0x%x\n",TxLen,TxLen);

    if (AcIDInput < 5) {
        Queue = (TX_LINKLIST_ID) AcIDInput;
    }
    else {
        dprintf("No Support AC Type!!!!!!!\n");
        return;
    }

    if (!LoopbackNum) {
        dprintf("0 times Loopback number!!!!!!!\n");
        return;        
    }


    if (TxExtendEn) {
        gEnableExTXBD = 1;
    }
    else
        gEnableExTXBD = 0;

//    if (type > 0) {
//        LoopMax = 0x1000000*LoopbackNum;
//    }
//    else {

        LoopMax = LoopbackNum;
        InputACIndex = Queue;
//    }

    switch (ThreType) {
        case 0:
            WlTxSegThreshold = 4096;
            WlExTxSegThreshold = 4096;
            break;
        case 1:
            WlTxSegThreshold = 1024;
            WlExTxSegThreshold = 1024;
            break;
        case 2:
            WlTxSegThreshold = 700;
            WlExTxSegThreshold = 700;
            break;
        case 3:
            WlTxSegThreshold = 512;
            WlExTxSegThreshold = 512;
            break;
        case 4:
            WlTxSegThreshold = 256;
            WlExTxSegThreshold = 256;
            break;
        case 5:
            WlTxSegThreshold = 160;
            WlExTxSegThreshold = 160;
            break;
        case 6:
            WlTxSegThreshold = 100;
            WlExTxSegThreshold = 100;
            break;
        case 7:
            WlTxSegThreshold = 40;
            WlExTxSegThreshold = 40;
            break;
        case 8:
            WlTxSegThreshold = 32;            
            WlExTxSegThreshold = 32;
            break;

        default:
            break;
    }
    dprintf("Segment Threshold Value: %d\n",WlTxSegThreshold);

    if (TxLen) {
        gTxLen = TxLen;
    }

    if (8 == type) {
        //Download 8051 firmware
        
        dprintf("JSW:Down Load RAM code\n");            
        if(InitFirmware88XX(NULL,downloadRAM)) {
           dprintf("Down Load RAM code Fail!!!!!!!\n");
           goto  ResultError;
        }
        
        //Enable 8051 fw for access
        WRITE_MEM8(0xb8640089,0x10);
    }


    for (LoopbackIndex=0; LoopbackIndex< LoopMax; LoopbackIndex++) {
#if 0
        temp = LoopbackIndex & 0x7FF;
        LLEle = (TX_LINK_LIST_ELE*)(((u32)&StackLLEle[temp])|0xa0000000);
#endif
        switch(type) {
            case 0:
                dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);                
                break;
            case 1:
                dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);                                
                break;
            case 2:
                if(!(LoopbackIndex & 0xFF)) {
                    dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);
                }
                break;
            case 3:
                InputACIndex = (LoopbackIndex & 0x3);
                dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex); 

                break;
            case 4:
                InputACIndex = (LoopbackIndex & 0x3);
                if(!(LoopbackIndex & 0xFF)) {
                    dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);
                }
                break;
            case 5:
#if 0
                InputACIndex = (LoopbackIndex & 0x3);

#else
                if (!(LoopbackIndex & 0xF)) {
                    InputACIndex++;
                    if(InputACIndex > 4) {
                       InputACIndex = 0; 
                    }
                }
#endif  

                if(!(LoopbackIndex & 0xFF)) {
                    dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);
                }

                if (!(LoopbackIndex & 0xFF)) {
                    RateIndex++;
                    if (RateIndex > 26) 
                        RateIndex = 0;

                    dprintf("Rate Index: %d\n",RateIndex);
                    ReplaceTxPktOption(RateIndex,3);
                }

                break;
            case 6:
                InputACIndex = (LoopbackIndex & 0x3);
                if(!(LoopbackIndex & 0xFF)) {
                    dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);
                }
                
                if (!(LoopbackIndex & 0xFF)) {
                    QueueTypeIndex++;
                    if (QueueTypeIndex > 3) 
                        QueueTypeIndex = 1;
                
                    dprintf("Queue Type Index: %d\n",QueueTypeIndex);
                    ReplaceTxPktOption(19,QueueTypeIndex);
                }
                break;
            case 7:
                if (!(LoopbackIndex & 0xFF)) {
                    InputACIndex++;
                    if(InputACIndex > 4) {
                       InputACIndex = 0; 
                    }
                }
                
                if(!(LoopbackIndex & 0x6F)) {
                    dprintf("=============>\n");
                    dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);                    
                    dprintf("Rate Index: %d\n",RateIndex);
                    dprintf("Queue Type Index: %d\n",QueueTypeIndex);
                    dprintf("<=============\n");
                }
                   
                if (!(LoopbackIndex & 0xFF)) {
                    RateIndex++;
                    if (RateIndex > 26) 
                        RateIndex = 0;

                    //dprintf("Rate Index: %d\n",RateIndex);

                    QueueTypeIndex++;
                    if (QueueTypeIndex > 3) 
                        QueueTypeIndex = 1;
                
                    //dprintf("Queue Type Index: %d\n",QueueTypeIndex);
                    ReplaceTxPktOption(RateIndex,QueueTypeIndex);
                }                

                if(!(LoopbackIndex & 0xFFF)) {
                    dprintf("=============>\n");
                    dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);                    
                    dprintf("Rate Index: %d\n",RateIndex);
                    dprintf("Queue Type Index: %d\n",QueueTypeIndex);
                    dprintf("<=============\n");
                }
                
                break;
            case 8:
                if (!(LoopbackIndex & 0xFF)) {
                    InputACIndex++;
                    if(InputACIndex > 4) {
                       InputACIndex = 0; 
                    }
                }
                
                if(!(LoopbackIndex & 0x7FF)) {
                    u8 FwCheckValue = 0;
                    FwCheckValue = READ_MEM8(0xb86401b9);
                    dprintf("=============>\n");
                    dprintf("Loopback number: 0x%x; AC id:%d\n", LoopbackIndex, InputACIndex);                    
                    dprintf("Rate Index: %d\n",RateIndex);
                    dprintf("Queue Type Index: %d\n",QueueTypeIndex);
                    dprintf("8051 Access Value: %d\n", FwCheckValue);
                    dprintf("<=============\n");
                }
                
                if (!(LoopbackIndex & 0xFFF)) {
                    RateIndex++;
                    if (RateIndex > 26) 
                        RateIndex = 0;
            
                    dprintf("Rate Index: %d\n",RateIndex);
            
                    QueueTypeIndex++;
                    if (QueueTypeIndex > 3) 
                        QueueTypeIndex = 1;
                
                    dprintf("Queue Type Index: %d\n",QueueTypeIndex);
                    ReplaceTxPktOption(RateIndex,QueueTypeIndex);
                }                
                break;

            default:
                break;
        }

        if (!WlRxResult) {
            dprintf("Loop Num: %d; Rx error!!!!!!!", LoopbackIndex);
	     goto ResultError;
            break;
        }


        switch (InputACIndex) {
            case 0://VO
                VoBufIndex++;
                VoBufIndex = VoBufIndex & 0xf;
                temp = (u8) VoBufIndex;
                break;
            case 1://VI
                ViBufIndex++;
                ViBufIndex = ViBufIndex & 0xf;            
                temp = (u8) ViBufIndex;
                break;
            case 2://BE
                BeBufIndex++;
                BeBufIndex = BeBufIndex & 0xf;            
                temp = (u8) BeBufIndex;
                break;
            case 3://BK
                BkBufIndex++;
                BkBufIndex = BkBufIndex & 0xf;            
                temp = (u8) BkBufIndex;
                break;
            case 4:
                MagBufIndex++;
                MagBufIndex = MagBufIndex & 0xf;            
                temp = (u8) MagBufIndex;
                break;
            default:
                dprintf("AC Index Error!!!!");
		  goto ResultError;
                break;
        }


        LLEle = (TX_LINK_LIST_ELE*)(((u32)&StackLLEle[InputACIndex][temp])|0xa0000000);

//        if (!SendPacket(LoopMode, InputACIndex, (TX_LINK_LIST_ELE*)LLEle)) {
#if 0
       if (!SendPacket(LoopMode, InputACIndex, (TX_LINK_LIST_ELE*)LLEle)) {
           dprintf("Loop Num: %d;AC %d, TX ERROR!!!!!\n", LoopbackIndex, InputACIndex);
            break;
       }
#else
            TxFailCheckCounter++;
            TxFailCheckflag = 1;
            while (TxFailCheckflag) {

                 if (!SendPacket(LoopMode, InputACIndex, (TX_LINK_LIST_ELE*)LLEle)) {
                    TxFailCheckCounter++;                    

                    if (TxFailCheckCounter > 1000) {
                        dprintf("Loop Num: %d;AC %d, TX ERROR!!!!!\n", LoopbackIndex, InputACIndex);
                        TxFailCheckflag = 0;
			   goto ResultError;
                    }
                 }
                 else {
                    TxFailCheckCounter = 0;
                    TxFailCheckflag = 0;
                 }
            }
            
            if (TxFailCheckCounter > 1000) {
                break;
            }
#endif            

        if (0xFF == READ_MEM8(0xb86401b8)) {
            dprintf("Loop Num: %d; 8051 access error!!!!!!!", LoopbackIndex);
 	     goto ResultError;			
            break;
        }

    }
    goto ResultSuccess;


ResultError:
    dprintf("=======    Fail   =======\n");
    return FALSE;


ResultSuccess:
    dprintf("========  Susccess Finish  ==========\n");
    return TRUE;

}




#define WL_INIT_MODE    1
#define WL_TEST_MODE    2


BOOLEAN
WlCtrlOperater(
    IN  u4Byte  Mode
)
{
    BOOLEAN Result = TRUE;

    
    switch(Mode) {
        case WL_INIT_MODE:
#ifdef CONFIG_WLMAC_SUPPORT
                WLHWRinit();
            
                if (!HALWLPwrInit())
		       return FALSE;
            
#if defined(CONFIG_WLMAC_LOOPBACK_SUPPORT) || defined(CONFIG_WLMAC_8051_SUPPORT)
            
                WLIsrinit();
            
            //    Mpinit();
            
                TxDataPathInit();
            
                TxSetPktOption();
            
                HALWLMacInit();
                
                HALWLHCIDmaInit();
#endif //CONFIG_WLMAC_LOOPBACK_SUPPORT
            
#endif //CONFIG_WLMAC_SUPPORT

            break;
        case WL_TEST_MODE:
            Result = WLTxCont();
            break;
        default:
            Result = FALSE;
            break;            
    }

    return Result;
}




