enum { __FILE_NUM__= 1 };


#include <linux/types.h>
#include <linux/synclink.h>
#include "wl_8881.h"
#include "wl_log.h"

#include "OUTSRC/rtl8821a/HalHWImg8821A_TestChip_BB.h"
#include "OUTSRC/rtl8821a/HalHWImg8821A_TestChip_MAC.h"
#include "OUTSRC/rtl8821a/HalHWImg8821A_TestChip_RF.h"


#define                    RX_FIFO_SIZE    16383       //16K
//Tx page setting
#define                    LAST_ENTRY_OF_TX_PKT_BUFFER 0xFF//KaiYuan modified for 88e 22k

extern LB_MODE             LbMode;
extern DATA_PATH_VARI      DataPathVari;
extern MAC_SETTING_GEN     MacSettingGen;
extern MAC_SETTING_PORT    MacSettingPort;

HCI_DMA_MANAGER            *hci_dma_manager;
HCI_DMA_MANAGER            hci_dma_buf;
RX_BUF_MAN                 RxBufEntry[MAX_BUF_NUM];

u4Byte 
InitMACHAL8881A (
    VOID
)
{
    u4Byte   Value32=0x00000000;
    u4Byte   Chose  = 0;
    u2Byte   Value16=0x0000;
    u1Byte   Value8=0x00;

    RTL_DMA_DBG_LOG("InitMAC8195 =======> \n");
    
    
    //-----------------------------------------------------
    /*0x0000h ~ 0x00FFh System Configuration*/
    //-----------------------------------------------------        
    HAL_RTL_W8(REG_CR,Value8);  //diable MAC
    Value8 = (BIT_HCI_TXDMA_EN | BIT_HCI_RXDMA_EN | BIT_TXDMA_EN | 
                BIT_RXDMA_EN| BIT_PROTOCOL_EN | BIT_SCHEDULE_EN | 
                BIT_MACTXEN | BIT_MACRXEN);

    HAL_RTL_W8(REG_CR, Value8);  

    //==== Set LoopBack Mode
    Value32 = HAL_RTL_R32(REG_CR);
        
    Value32 &= ~MASK_LBMODE;
    switch(LbMode)
    {
        case MAC_LB:
            Chose = _LBMODE(0x0b); 
        break;
        case MAC_Delay_LB:
            Chose = _LBMODE(LOOPBACK_MAC_DELAY);
        break;
        case LB_Disable:
            Chose = _LBMODE(LOOPBACK_NORMAL);  
        break;
        default:
            Chose = _LBMODE(LOOPBACK_NORMAL);
        break;
    }
    
    Value32 |= Chose;
    HAL_RTL_W32(REG_CR, Value32);


    //1 Check if the verify item need
    //HAL_RTL_W16(REG_FUNC_ENABLE,HAL_RTL_R16(REG_FUNC_ENABLE)|ENSEC);   //MAC Security Enable    
    
    //Value8 = (lb_mode == LB_Disable) ? TX_PAGE_BOUNDARY_NORMAL_MODE : TX_PAGE_BOUNDARY_LOOPBACK_MODE;
    Value8 = TX_PAGE_BOUNDARY_LOOPBACK_MODE;

    //init tx packet buffer 
    InitLLTTable(Value8, LAST_ENTRY_OF_TX_PKT_BUFFER);

    //Set TX page boundary (part 0: Tx pages; part 1: Rsvd pages); rsvd pages is for beacon or null paket
    //3 Note: When this register is set,  the using point of LLT for hw will be reset to 0. 
    //3 If controller want to aVOID this, it need to set another control bit.
    HAL_RTL_W8(REG_TXPKTBUF_BCNQ_BDNY, Value8);
    HAL_RTL_W8(REG_TXPKTBUF_MGQ_BDNY, Value8);
    HAL_RTL_W8(REG_TDECTRL+1, Value8);

    //4 Initial MBSSID beacon download pages
    HAL_RTL_W8(REG_TDECTRL1+1, BCN_PAGE_NUM);    
    HAL_RTL_W8(REG_TXPKTBUF_BCNQ_BDNY1, BCN_PAGE_NUM);    

    
    HAL_RTL_W16(REG_TRXFF_BNDY+2, RX_FIFO_SIZE);

    
    //Chris: need check reserved page setting
    if( LbMode != LB_Disable) {
        Value8 = _NPQ(NLLB_PAGE_NUM_NPQ);
        Value32 = _HPQ(NLLB_PAGE_NUM_HPQ) | _LPQ(NLLB_PAGE_NUM_LPQ)
                 | _PUBQ(NLLB_PAGE_NUM_PUBQ) | BIT_LD_RQPN;    
    
    }
    else {
            Value8 = _NPQ(NTWOMAC_PAGE_NUM_NPQ);
            Value32 = _HPQ(NTWOMAC_PAGE_NUM_HPQ) | _LPQ(NTWOMAC_PAGE_NUM_LPQ)
                                | _PUBQ(NTWOMAC_PAGE_NUM_PUBQ) | BIT_LD_RQPN;   
    }
    
    HAL_RTL_W8(REG_RQPN_NPQ, Value8);
    HAL_RTL_W32(REG_RQPN, Value32);   


    //3 Note: Set for loopback mode,    
          //3 tx boundary = TX_PAGE_BOUNDARY_LOOPBACK_MODE
          //3 the other is reserved pages for RX back up mode.
    
    if( LbMode == MAC_Delay_LB) {
        Value8 = TX_PAGE_BOUNDARY_LOOPBACK_MODE;
        HAL_RTL_W8(REG_8188E_WMAC_LBK_BUF_HD, Value8);
    }

    // HISR - turn all on
    Value32 = 0xFFFFFFFF;
    HAL_RTL_W32(REG_HISR, Value32);
    // HIMR - turn all on
    HAL_RTL_W32(REG_HIMR, Value32);

    //init_ID
    //dprintf("init ID:\n");
    //IO_Func.WriteRegister(REG_MACID, 6,addr.src_addr);
    //IO_Func.WriteRegister(REG_BSSID, 6,addr.bssid);
    
    // Set MAR (Multicast Address Register)
    HAL_RTL_W32(REG_MAR, 0xFFFFFFFF);
    HAL_RTL_W32((REG_MAR + 4), 0xFFFFFFFF);

    //Set TRX DMA queue attribute: 1) low priority; 2) normal priority; 3) high priority
    //dprintf("init_dma_ctrl:\n");
    Value16 = HAL_RTL_R16(REG_TRX_DMA_CTRL);

    Value16 &=0x7;
    Value16 |=((BIT4|BIT5)|(BIT7&(~BIT6))|(BIT8 &(~BIT9))|(BIT10 &(~BIT11))|(BIT12|BIT13)|(BIT14|BIT15));

    HAL_RTL_W16(REG_TRX_DMA_CTRL, Value16);

    //init_network_type
    //dprintf("init_network_type:\n");
    //Value32 = HAL_RTL_R32(REG_CR);
    //Value32 = (Value32 & ~MASK_NETTYPE) | _NETTYPE(NT_LINK_AP);
    //HAL_RTL_W32(REG_CR, Value32);

    //Chris: Need check hw register
    //set_page_size
    //dprintf("set_page_size:\n");
    Value8 = _PSRX(PBP_128) | _PSTX(PBP_256);
    HAL_RTL_W8(REG_PBP, Value8);//REG_PAGE_SIZE

    //Set Receive Configuration Register
    //dprintf("set_wmac:\n");
    Value32 = BIT_AAP | BIT_APM | BIT_AM |BIT_AB  |BIT_ADD3|BIT_APWRMGT| 
              BIT_APP_ICV | BIT_APP_MIC |BIT_APP_FCS|BIT_ACRC32|BIT_AICV|
              BIT_ADF |BIT_ACF|BIT_AMF|BIT_HTC_LOC_CTRL;
    HAL_RTL_W32(REG_RCR, Value32);

    //init_adaptive_ctrl
    //dprintf("init_adaptive_ctrl:\n");
    // Response Rate Set
    Value32 = HAL_RTL_R32(REG_RRSR);
    Value32 |= BIT_RATE_BITMAP_ALL;
    HAL_RTL_W32(REG_RRSR, Value32);

    // CF-END Threshold
    //  HAL_RTL_W8(REG_CFEND_TH, 0x1);

    // SIFS
    //dprintf("init MAC related register:\n");
    Value16 = _SPEC_SIFS_CCK(0x10) | _SPEC_SIFS_OFDM(0x10);
    HAL_RTL_W16(REG_SPEC_SIFS, Value16);

    // Retry Limit
    //  value16 = _LRL(0x8) | _SRL(0x7);
    Value16 = _LRL(0x0) | _SRL(0x0);
    HAL_RTL_W16(REG_RL, Value16);

    // init_edca
    //dprintf("init_edca:\n");
    //  Value16 = _SIFS_CCK_CTX(0x10) | _SIFS_CCK_TRX(0x10);
    Value16 = _SIFS_CCK_CTX(0x0A) | _SIFS_CCK_TRX(0x0A);
    HAL_RTL_W16(REG_SIFS, Value16);//CCK Setting

    Value16 = _SIFS_OFDM_CTX(0x10) | _SIFS_OFDM_TRX(0x10);
    HAL_RTL_W16(REG_SIFS + 2, Value16);//OFDM Setting

    Value32=HAL_RTL_R32(REG_EDCA_VO_PARAM);   //VO TXOP Disable
    Value32&=0x0000FFFF;
    HAL_RTL_W32(REG_EDCA_VO_PARAM,Value32);

    Value32=HAL_RTL_R32(REG_EDCA_VI_PARAM);  //VI TXOP Disable
    Value32&=0x0000FFFF;
    HAL_RTL_W32(REG_EDCA_VI_PARAM,Value32);

    //dprintf("init_Retry function:\n");
    HAL_RTL_W32(REG_DARFRC, 0x00000000);
    HAL_RTL_W32(REG_DARFRC + 4, 0x06050401);
    HAL_RTL_W32(REG_RARFRC, 0x00000000);
    HAL_RTL_W32(REG_RARFRC+4, 0x06050401);

    // Set Data Auto Rate Fallback Reg.
    u1Byte ByteIndex;
    for(ByteIndex = 0 ; ByteIndex < 4 ; ByteIndex++) {
        HAL_RTL_W32((REG_ARFR0+ByteIndex*4), 0x1f0ffff0);
    }

    // Set ACK timeout
    HAL_RTL_W8(REG_ACKTO, 0x40);   

    //////////////////////////////////////////////////////////////////////////
    HAL_RTL_W8((REG_TCR+1),HAL_RTL_R8(REG_TCR+1)|0x30);
    HAL_RTL_W8(REG_RXTSF_OFFSET_CCK,0x30);  //data path of cck_rx_tsf
    HAL_RTL_W8(REG_RXTSF_OFFSET_OFDM,0x30);  //data path of ofdm_rx_tsf
    HAL_RTL_W8(REG_TCR+2,0x30);
    //HAL_RTL_W8(0x5e4,0x38);
    HAL_RTL_W8(REG_DIS_TXREQ_CLR,0x10);
    //  HAL_RTL_W8(0x5f0,0x38);
    //HAL_RTL_W8(0x5f0,0xb8);                           //modify by Guo.Mingzhi 2010-01-11

    HAL_RTL_W16(REG_SYS_FUNC_EN,HAL_RTL_R16(REG_SYS_FUNC_EN)&(~BIT10)); //reset 8051
    HAL_RTL_W8((REG_STBC_SETTING + 3),HAL_RTL_R8((REG_STBC_SETTING + 3))|BIT7); //enable single pkt ampdu
    HAL_RTL_W8(REG_RX_PKT_LIMIT, 0x18);       //for VHT packet length 11K

    //Release BB 
    HAL_RTL_W8(REG_SYS_FUNC_EN, 0x3);

    HAL_RTL_W8(REG_RF_CTRL, 0x3);


    dprintf("Initial Mac image\n");    
    ODM_ReadAndConfig_TC_8821A_MAC_REG();

    dprintf("Initial PHY image\n");    
    ODM_ReadAndConfig_TC_8821A_PHY_REG();

    dprintf("Initial AGC image\n");    
    ODM_ReadAndConfig_TC_8821A_AGC_TAB();

    dprintf("Initial RF image\n");    
    ODM_ReadAndConfig_TC_8821A_RadioA();

    //
    HAL_RTL_W8(0x80b, 0x3E); 
    

    RTL_DMA_DBG_LOG("<======= InitMAC8195\n");

    return TRUE;    
}

u4Byte 
InitMACToAPHAL8881A (
    VOID
)
{
    //MIB Information
    u4Byte             Value32;
    u2Byte             dot11BeaconPeriod;
    u4Byte             AckTO;
    u4Byte             vap_enable;
    u4Byte             dot11DTIMPeriod;
    u4Byte             wdsPure;
    u1Byte              meshSilence;
    
    u4Byte             opmode;
    RT_OP_MODE      OP_Mode;

    dot11BeaconPeriod = 100;
    AckTO = 0;
    vap_enable = 0;
    dot11DTIMPeriod = 1;
    wdsPure = 0;
    meshSilence = 0;
    opmode = WIFI_AP_STATE;
    OP_Mode = (u4Byte) RT_OP_MODE_AP;

    if (opmode & WIFI_AP_STATE) {
        dprintf("AP-mode enabled...\n");

#if defined(CONFIG_RTK_MESH)	//Mesh Mode but mesh not enable
        if (wdsPure || meshSilence )
#else
        if (wdsPure)
#endif
        {
            OP_Mode = RT_OP_MODE_NO_LINK;
        }
        else {
            OP_Mode = RT_OP_MODE_AP;
        }            
    }
    else if (opmode & WIFI_STATION_STATE) {
        dprintf("Station-mode enabled...\n");
        OP_Mode = RT_OP_MODE_INFRASTRUCTURE;
    }
    else if (opmode & WIFI_ADHOC_STATE) {
        dprintf("Adhoc-mode enabled...\n");
        OP_Mode = RT_OP_MODE_IBSS;
    }
    else
    {
        dprintf("Operation mode error!\n");
        return 2;
    }


    //4 HW_VAR_MEDIA_STATUS    
    Value32 = HAL_RTL_R32(REG_CR);
    Value32 &= (~BIT_MASK_NETTYPE);
    Value32 &= (~BIT_MASK_LBMODE);    
    Value32 |= BIT_LBMODE(LOOPBACK_NORMAL);
    Value32 |= BIT_NETTYPE(NT_AS_AP);
    HAL_RTL_W32(REG_CR, Value32);

    //4 HW_VAR_BEACON_INTERVAL
    HAL_RTL_W16(REG_MBSSID_BCN_SPACE, dot11BeaconPeriod);

    //4 HW_VAR_MAC_CONFIG    
    HAL_RTL_W8(REG_INIRTS_RATE_SEL, 0x8); // 24M


    if ((AckTO > 0) && (AckTO < 0xff)) {
        HAL_RTL_W8(REG_ACKTO, AckTO);
    }
    else {
        HAL_RTL_W8(REG_ACKTO, 0x40);
    }

    // TODO: check 8881A (11AC), modify value below
    // set user defining ARFR table for 11n 1T
    HAL_RTL_W32(REG_ARFR0, 0xFF015);	// 40M mode
    HAL_RTL_W32(REG_ARFR1, 0xFF005);	// 20M mode    

    /*
        * Disable TXOP CFE
        */
    HAL_RTL_W16(REG_RD_CTRL, HAL_RTL_R16(REG_RD_CTRL) | BIT10);

    /*
        *	RA try rate aggr limit
        */
    HAL_RTL_W8(REG_RA_TRY_RATE_AGG_LMT, 2);

    /*
	 *	Max mpdu number per aggr
	 */
    HAL_RTL_W16(REG_PROT_MODE_CTRL+2, 0x0909);

    // MAC Beacon Tming Related

    if (vap_enable)
    {
        HAL_RTL_W32(REG_TBTT_PROHIBIT, 0x1df04);
    }
    else
    {
        HAL_RTL_W32(REG_TBTT_PROHIBIT, 0x40004);
    }

    HAL_RTL_W8(REG_DRVERLYINT,          10);
    HAL_RTL_W8(REG_BCNDMATIM,           1);
    HAL_RTL_W16(REG_ATIMWND,            0x3C);
    HAL_RTL_W8(REG_DTIM_COUNTER_ROOT,   dot11DTIMPeriod-1);
    HAL_RTL_W32(REG_PKT_LIFETIME_CTRL,  HAL_RTL_R32(REG_PKT_LIFETIME_CTRL) & (~(BIT(19))));

#ifdef CONFIG_HAL_SUPPORT_MBSSID
    if (vap_enable && HAL_NUM_VWLAN == 1 &&
            (HAL_RTL_R16(REG_MBSSID_BCN_SPACE)< 30))
    {
        HAL_RTL_W8(REG_DRVERLYINT, 6);
    }
#endif  //CONFIG_HAL_SUPPORT_MBSSID

    HAL_RTL_W8(REG_PRE_DL_BCN_ITV, HAL_RTL_R8(REG_DRVERLYINT)+1);

    HAL_RTL_W8(REG_BCN_CTRL,            DIS_TSF_UPDATE_N|DIS_SUB_STATE_N);
    HAL_RTL_W8(REG_BCN_MAX_ERR,         0xff);
    HAL_RTL_W16(REG_TSFTR_SYN_OFFSET,   0);
    HAL_RTL_W8(REG_DUAL_TSF_RST,        3);
    if( RT_OP_MODE_INFRASTRUCTURE == OP_Mode )
    {
        HAL_RTL_W8(REG_FUNCTION_ENABLE+2, HAL_RTL_R8(REG_FUNCTION_ENABLE+2)^BIT6);
    }

    HAL_RTL_W8(REG_BCN_CTRL, HAL_RTL_R8(REG_BCN_CTRL) | EN_BCN_FUNCTION | EN_TXBCN_RPT);

    if( RT_OP_MODE_AP == OP_Mode )
    {
        HAL_RTL_W16(REG_BCNTCFG, 0x0001);
    }
    else
    {
        HAL_RTL_W16(REG_BCNTCFG, 0x0204);
    }
    

}


/* HW Init Functions*/
u4Byte 
InitPowerHAL8881A (
    VOID
)
{
    u4Byte    Value32=0x00000000;
    u2Byte    Value16=0x0000;
    u1Byte    Value8=0x00;
    u4Byte    PoweRdyCount =0;

    RTL_DMA_DBG_LOG("InitPower8881 WiFi=======>\n");

    //Enable Lextra CLK 
    dprintf("Enable Lextra Clk\n");
    Value32 = READ_MEM32(0xB8000010);
    Value32 = Value32 | 0x00003800;
    WRITE_MEM32(0xB8000010,Value32);

    //Enable WLMAC Reset 
    dprintf("Enable WLMac Reset\n");
    Value32 = READ_MEM32(0xB80000DC);
    Value32 = Value32 | 0x00000003;
    WRITE_MEM32(0xB80000DC,Value32);
	//delay_ms(1);
//    delay_ms(100);
#if 0
    // Enable AFE PLL
    RTL_DMA_DBG_LOG("Enable AFE PLL\n");

    Value8 = HAL_RTL_R8(REG_AFE_CTRL2);
    Value8 |= BIT_APLL_EN;
    HAL_RTL_W8(REG_AFE_CTRL2, Value8);

    delay_ms(1);


    // Release Isolation 
    RTL_DMA_DBG_LOG("Release Isolation\n");
    Value8 = HAL_RTL_R8(REG_SYS_ISO_CTRL);
    Value8 &= (~(BIT_8723A_ISO_MD2PP | BIT_8723A_ISO_IP2MAC));
    HAL_RTL_W8(REG_SYS_ISO_CTRL, Value8);
    delay_ms(1);


    // Release PLL Clock Gate
    RTL_DMA_DBG_LOG("Release PLL Clock Gate\n");
    Value16 = HAL_RTL_R16(REG_AFE_CTRL2);
    Value16 |= BIT_APLL_320_GATEB;
    HAL_RTL_W16(REG_AFE_CTRL2, Value16);
    delay_ms(1);

    // Enable SEC Clock
    RTL_DMA_DBG_LOG("Enable SEC Clock\n");
    Value16 = HAL_RTL_R16(REG_SYS_CLK_CTRL);
    Value16 |= BIT_SEC_CLK_EN;
    HAL_RTL_W16(REG_SYS_CLK_CTRL, Value16);
    delay_ms(1);


    // Enable WMAC Clock
    RTL_DMA_DBG_LOG("Enable WMAC Clock\n");
    Value16 = HAL_RTL_R16(REG_SYS_CLK_CTRL);
    Value16 |= BIT_MAC_CLK_EN;
    HAL_RTL_W16(REG_SYS_CLK_CTRL, Value16);
    delay_ms(1);

    // Enabl WLMCU Clock
    RTL_DMA_DBG_LOG("Enabl WLMCU Clock\n");
    Value16 = HAL_RTL_R16(REG_SYS_CLK_CTRL);
    Value16 |= BIT_CPU_CLK_EN;
    HAL_RTL_W16(REG_SYS_CLK_CTRL, Value16);
    delay_ms(1);

    
    // Release MAC Power On Reset
    RTL_DMA_DBG_LOG("Enabl WLMCU Clock\n");
    Value16 = HAL_RTL_R16(REG_SYS_FUNC_EN);
    Value16 |= (BIT_FEN_DCORE);
    HAL_RTL_W16(REG_SYS_FUNC_EN, Value16);
    delay_ms(1);
    
    // Enable MAC IOREG Access
    RTL_DMA_DBG_LOG("Enable MAC IOREG Access\n");
    Value16 = HAL_RTL_R16(REG_SYS_FUNC_EN);
    Value16 |= (BIT_FEN_MREGEN);
    HAL_RTL_W16(REG_SYS_FUNC_EN, Value16);
    delay_ms(1);

    //Enable WLMCU (DW8051POR_O)
    RTL_DMA_DBG_LOG("Enable WLMCU\n");
    Value16 = HAL_RTL_R16(REG_SYS_FUNC_EN);
    Value16 |= (BIT_FEN_CPUEN);
    HAL_RTL_W16(REG_SYS_FUNC_EN, Value16);
 #else
     RTL_DMA_DBG_LOG("Enable WLON Power\n");
     
     Value32 = HAL_RTL_R32(REG_SYS_PW_CTRL);
     Value32 |= (1 << 16);
     HAL_RTL_W32(REG_SYS_PW_CTRL, Value32);


     // Enable AFE PLL
    RTL_DMA_DBG_LOG("Enable WL MAC\n");
 
    Value32 = HAL_RTL_R32(REG_SYS_PW_CTRL);
    Value32 |= (1 << 8);
    HAL_RTL_W32(REG_SYS_PW_CTRL, Value32);
//        dprintf("%x\n",(HAL_RTL_R32(REG_SYS_PW_CTRL) & BIT(17)));
#if 1
    while ((HAL_RTL_R32(REG_SYS_PW_CTRL) & BIT(8)))
    {
    	 delay_ms(10);
        //prom_printf("======%x\n",(HAL_RTL_R32(REG_SYS_PW_CTRL) & BIT(8)));
        PoweRdyCount++; //Std value is about 11xxx under CPU=520MHZ.
	 if (PoweRdyCount > (5000)) //JSW:20121122:Necessary
	 {
	 	prom_printf(" Fail,PoweRdyCount = %d\n", PoweRdyCount);
	// if (PoweRdyCount > 50000) //JSW:20121122:Necessary
		return FALSE;
	 }
    }

    
#endif
#if 0
    RTL_DMA_DBG_LOG("Power On RF\n");
  
    Value8 = HAL_RTL_R32(REG_RF_CTRL);
    Value8 |= 1; //Power on RF
    HAL_RTL_W32(REG_RF_CTRL, Value8);
#endif
 #endif
    RTL_DMA_DBG_LOG("<======== InitPower8881 WiFi\n"); 
    return TRUE;    
}


u4Byte 
TXDescGenHAL8881A (
    IN  u4Byte PayloadLen, 
    IN  u4Byte Queue, 
    IN  u1Byte  IsBcn,
    IN  u1Byte  IsDlForFw,
    IN  PORT_INFO *PortInfo,
    IN  TX_DESC *curDesc, 
    IN  TX_EXTEND_DESC  *curExtendDesc 
)
{
	u1Byte      ExtendLen;
	u1Byte      TxExtendDescLen=0x00;
	BOOLEAN     *bLastFrag=NULL;
	BOOLEAN     *bFirstFrag=NULL;
	u1Byte      *nFrag=NULL;
	BOOLEAN     HT= MacSettingGen.source_data.ht_support;
	BOOLEAN     QOS=MacSettingGen.source_data.qos_support;
	BOOLEAN     EARLY_MODE= MacSettingGen.tx_discriptor.early_mode_support;
	BOOLEAN 	EARLY_MODE_EXTEND = MacSettingGen.tx_discriptor.early_mode_extend;
	BOOLEAN     MSDU_FRAG_EN=MacSettingGen.tx_discriptor.msdu_frag;
	u4Byte      *SeqNum = &(DataPathVari.SeqNum[Queue]);
	u4Byte	    Ampdu_Max_Len=0;
    u2Byte      TxPktSize, Sectype, Qsel, Seq, DataRate, RtsRate, UseRate, Morefrag;
    u1Byte      DwIndex;
    u4Byte      *pDw;
    u1Byte      Mbssid;
    u1Byte      MacId = 0;
    u1Byte      MoreDataBit = 0;
    u2Byte      GroupBitIEOffset = 0;
    u1Byte      GroupBitIEEn = 0;
    u4Byte      PartialID, GroudID;

    //4 Set for 11AC
    PartialID = 0x11;
    GroudID = 0;

    if (IsBcn || IsDlForFw) {
    	ExtendLen=0;
    	TxExtendDescLen=sizeof(TX_DESC);

        if (IsDlForFw)
        {
            TxExtendDescLen = 0x28;
        }
        
    }
    else {        
    	ExtendLen=(QOS==TRUE)?2:0; //to be modified
    	ExtendLen=(HT==TRUE)?(ExtendLen+4):ExtendLen;

    	TxExtendDescLen=(EARLY_MODE==TRUE)?(sizeof(TX_DESC)+8):sizeof(TX_DESC);	
    }
    
	if((PortInfo->encryption_info.type==WEP40)||(PortInfo->encryption_info.type==WEP104))
	{
		if(PortInfo->encryption_info.encryption==SW)
			TxPktSize = (u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen+IVLen)+ICVLen;
		else
		{
			Sectype = 0x1;
			TxPktSize = (u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen+IVLen);
		}	
	}
	else if(PortInfo->encryption_info.type==TKIP)
	{
		if(PortInfo->encryption_info.encryption==SW)
			TxPktSize = (u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen+IVLen+EIVLen)+MICLen+ICVLen;
		else
		{
			TxPktSize = (u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen+IVLen+EIVLen+MICLen);
			Sectype = 0x1;
		}	
	}
	//Info->AES
    else if(PortInfo->encryption_info.type==AES)
    {
     if(PortInfo->encryption_info.encryption==SW)
    	 TxPktSize = (u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen+CCMPheaderLen)+MICLen;
     else
     {
    	 TxPktSize = (u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen+CCMPheaderLen);
    	 Sectype = 0x3;
    	}
    }
    else if(PortInfo->encryption_info.type==WAPI) 
    {
     Sectype = 0x02;
     if(PortInfo->encryption_info.encryption==SW)
    	 TxPktSize =(u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen+WAPIheaderLen+WAPIMicLen);
     else
     {
    	 TxPktSize =(u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen+WAPIheaderLen);
    	//curDesc->sectype=0x1;
     }	 
    }
    else { //no security
     Sectype = 0x00;
     TxPktSize = (u2Byte)(PayloadLen+WLAN_HEADER_LENGTH+ExtendLen);
    }

    // Handle MBSSID
    if (PortInfo->address_info.bssid[0]) {
        MacId = PortInfo->address_info.bssid[0];
        MoreDataBit = 1;

        if (IsBcn) {
            GroupBitIEOffset = PortInfo->TIMIeOffset;
            GroupBitIEEn = 1;
        }
    }

    if (IsDlForFw) {
        Sectype = 0x00;
        TxPktSize = (u2Byte)(PayloadLen+ExtendLen);    
    }

    if (IsBcn || IsDlForFw) {
        Qsel=BcnQsel; // BCN queue
        Seq=0;
        DataRate = RATE_1;
        RtsRate= 0;         
        UseRate = 0;       
    }
    else {
     
        switch(Queue) {

            case TX_LINKLIST_VO:
                Qsel=VoQsel; // VO queue
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_VO);   
                bFirstFrag=&(DataPathVari.bFirstFrag_VO);
                nFrag=&(DataPathVari.nFrag_VO);
                break;
            case TX_LINKLIST_VI:
                Qsel=ViQsel; // VI queue
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate = (MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_VI);   
                bFirstFrag=&(DataPathVari.bFirstFrag_VI);
                nFrag=&(DataPathVari.nFrag_VI);
                break;
    			
            case TX_LINKLIST_BE:
                Qsel=BeQsel; // BE queue
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_BE);   
                bFirstFrag=&(DataPathVari.bFirstFrag_BE);
                nFrag=&(DataPathVari.nFrag_BE);			
                break;
            case TX_LINKLIST_BK:
                Qsel=BkQsel; // BK queue
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;			
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_BK);   
                bFirstFrag=&(DataPathVari.bFirstFrag_BK);
                nFrag=&(DataPathVari.nFrag_BK);	
                break;					
            case TX_LINKLIST_MAG:
                Qsel=MagQsel; // BK queue
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;			
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_MG);   
                bFirstFrag=&(DataPathVari.bFirstFrag_MG);
                nFrag=&(DataPathVari.nFrag_MG);	
                break;        
            case TX_LINKLIST_H0:
                Qsel=HighQsel; // High queue 0
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;         
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_H0);   
                bFirstFrag=&(DataPathVari.bFirstFrag_H0);
                nFrag=&(DataPathVari.nFrag_H0);
                break;        
            case TX_LINKLIST_H1:
                Qsel=HighQsel; // High queue 1
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;         
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_H1);   
                bFirstFrag=&(DataPathVari.bFirstFrag_H1);
                nFrag=&(DataPathVari.nFrag_H1);
                break;        
            case TX_LINKLIST_H2:
                Qsel=HighQsel; // High queue 2
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;         
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_H2);   
                bFirstFrag=&(DataPathVari.bFirstFrag_H2);
                nFrag=&(DataPathVari.nFrag_H2);
                break;        
            case TX_LINKLIST_H3:
                Qsel=HighQsel; // High queue 3
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;         
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_H3);   
                bFirstFrag=&(DataPathVari.bFirstFrag_H3);
                nFrag=&(DataPathVari.nFrag_H3);
                break;        
            case TX_LINKLIST_H4:
                Qsel=HighQsel; // High queue 4
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;         
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_H4);   
                bFirstFrag=&(DataPathVari.bFirstFrag_H4);
                nFrag=&(DataPathVari.nFrag_H4);
                break;        
            case TX_LINKLIST_H5:
                Qsel=HighQsel; // High queue 5
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;         
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_H5);   
                bFirstFrag=&(DataPathVari.bFirstFrag_H5);
                nFrag=&(DataPathVari.nFrag_H5);
                break;        
            case TX_LINKLIST_H6:
                Qsel=HighQsel; // High queue 6
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;         
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_H6);   
                bFirstFrag=&(DataPathVari.bFirstFrag_H6);
                nFrag=&(DataPathVari.nFrag_H6);
                break;        
            case TX_LINKLIST_H7:
                Qsel=HighQsel; // High queue 7
                Seq=*SeqNum;
                DataRate  = MacSettingGen.source_data.packet[Queue].rate;
                RtsRate=(MacSettingGen.source_data.packet[Queue].rate>3)?0x0b:0x03;         
                UseRate = 1;
                bLastFrag=&(DataPathVari.bLastFrag_H7);   
                bFirstFrag=&(DataPathVari.bFirstFrag_H7);
                nFrag=&(DataPathVari.nFrag_H7);
                break;        
        }	
        
    	if(MSDU_FRAG_EN && ((*bLastFrag)==FALSE))    
    		Morefrag=TRUE;
        else
            Morefrag=FALSE;


    	if(MacSettingGen.tx_discriptor.max_length==len_8k)
    		Ampdu_Max_Len=0x08;
    	else
    		Ampdu_Max_Len=0x16;

    }

	//2 /* Generate Tx Descriptor*/
    //4 Set Dword0
    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  TxPktSize,
                                  TX_DW0_TXPKSIZE_MSK,
                                  TX_DW0_TXPKSIZE_SH);

    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  (u4Byte)(TxExtendDescLen),
                                  TX_DW0_OFFSET_MSK,
                                  TX_DW0_OFFSET_SH);

    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  ((PortInfo->address_info.cast_type==BROADCAST||
                                  PortInfo->address_info.cast_type==MULTICAST)?1:0),
                                  TX_DW0_BMC_MSK,
                                  TX_DW0_BMC_SH);

    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  ((HT==TRUE)?1:0),
                                  TX_DW0_HTC_MSK,
                                  TX_DW0_HTC_SH);

//Lextra: Modify to rsvd
#if 0
    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  (0x01),
                                  TX_DW0_RSVD_26_MSK,
                                  TX_DW0_RSVD_26_SH);

    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  (0x01),
                                  TX_DW0_RSVD_27_MSK,
                                  TX_DW0_RSVD_27_SH);
#endif

    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  (0x00),
                                  TX_DW0_LINIP_MSK,
                                  TX_DW0_LINIP_SH);

    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  (0x00),
                                  TX_DW0_NOACM_MSK,
                                  TX_DW0_NOACM_SH);

    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  (0x00),
                                  TX_DW0_GF_MSK,
                                  TX_DW0_GF_SH);
    //4 Don't Need: 8881A, 8192E
    curDesc->Dword0 = SET_DESC(curDesc->Dword0,
                                  (0x01),
                                  TX_DW0_RSVD_31_MSK,
                                  TX_DW0_RSVD_31_SH);

    //4 Set Dword1
    curDesc->Dword1 = SET_DESC(curDesc->Dword1,
                                  Sectype,
                                  TX_DW1_SECTYPE_MSK,
                                  TX_DW1_SECTYPE_SH);

    curDesc->Dword1 = SET_DESC(curDesc->Dword1,
                                  ((EARLY_MODE==TRUE)?8:0),
                                  TX_DW1_PKT_OFFSET_MSK,
                                  TX_DW1_PKT_OFFSET_SH);

    curDesc->Dword1 = SET_DESC(curDesc->Dword1,
                                  Qsel,
                                  TX_DW1_QSEL_MSK,
                                  TX_DW1_QSEL_SH);

    curDesc->Dword1 = SET_DESC(curDesc->Dword1,
                                  MoreDataBit,
                                  TX_DW1_MOREDATA_MSK,
                                  TX_DW1_MOREDATA_SH);

    //KaiYuan added for 88e force key function
    curDesc->Dword1 = SET_DESC(curDesc->Dword1,
                                  MacId,
                                  TX_DW1_MACID_MSK,
                                  TX_DW1_MACID_SH);

    //KaiYuan added for 88e force key function
    curDesc->Dword1 = SET_DESC(curDesc->Dword1,
                                  (MacSettingPort.ForceKey_En),
                                  TX_DW1_EN_DESC_ID_MSK,
                                  TX_DW1_EN_DESC_ID_SH);


    //4 Set Dword2
    curDesc->Dword2 = SET_DESC(curDesc->Dword2,
                                  (Morefrag),
                                  TX_DW2_MOREFRAG_MSK,
                                  TX_DW2_MOREFRAG_SH);
    
    curDesc->Dword2 = SET_DESC(curDesc->Dword2,
                                  (MacSettingGen.tx_discriptor.ampdu_support),
                                  TX_DW2_AGG_EN_MSK,
                                  TX_DW2_AGG_EN_SH);

    curDesc->Dword2 = SET_DESC(curDesc->Dword2,
                                  (MacSettingGen.tx_discriptor.ampdu_density),
                                  TX_DW2_AMPDU_DENSITY_MSK,
                                  TX_DW2_AMPDU_DENSITY_SH);

    curDesc->Dword2 = SET_DESC(curDesc->Dword2,
                                  (PartialID),
                                  TX_DW2_P_AID_MSK,
                                  TX_DW2_P_AID_SH);


    curDesc->Dword2 = SET_DESC(curDesc->Dword2,
                                  (GroudID),
                                  TX_DW2_G_ID_MSK,
                                  TX_DW2_G_ID_SH);


    //4 Set Dword3
    curDesc->Dword3 = SET_DESC(curDesc->Dword3,
                                  (0x01),
                                  TX_DW3_DISRTSFB_MSK,
                                  TX_DW3_DISRTSFB_SH);                                

    curDesc->Dword3 = SET_DESC(curDesc->Dword3,
                                  (0x01),
                                  TX_DW3_DISDATAFB_MSK,
                                  TX_DW3_DISDATAFB_SH);

    curDesc->Dword3 = SET_DESC(curDesc->Dword3,
                                  (MacSettingGen.tx_discriptor.rts_support),
                                  TX_DW3_RTSEN_MSK,
                                  TX_DW3_RTSEN_SH);

    curDesc->Dword3 = SET_DESC(curDesc->Dword3,
                                  (MacSettingGen.tx_discriptor.cts2self_support),
                                  TX_DW3_CTS2SELF_MSK,
                                  TX_DW3_CTS2SELF_SH);

    curDesc->Dword3 = SET_DESC(curDesc->Dword3,
                                  (MacSettingGen.tx_discriptor.max_agg_num),
                                  TX_DW3_MAX_AGG_NUM_MSK,
                                  TX_DW3_MAX_AGG_NUM_SH);

    curDesc->Dword3 = SET_DESC(curDesc->Dword3,
                                  ((MacSettingGen.tx_discriptor.use_max_len)?1:0),
                                  TX_DW3_USE_MAX_LEN_MSK,
                                  TX_DW3_USE_MAX_LEN_SH);

    curDesc->Dword3 = SET_DESC(curDesc->Dword3,
                                  UseRate,
                                  TX_DW3_USERATE_MSK,
                                  TX_DW3_USERATE_SH);

    //4 Set Dword4
    curDesc->Dword4 = SET_DESC(curDesc->Dword4,
                                  (MacSettingGen.tx_discriptor.retry_enable),
                                  TX_DW4_RTY_LMT_EN_MSK,
                                  TX_DW4_RTY_LMT_EN_SH);

    curDesc->Dword4 = SET_DESC(curDesc->Dword4,
                                  (MacSettingGen.tx_discriptor.retry_limit),
                                  TX_DW4_DATA_RT_LMT_MSK,
                                  TX_DW4_DATA_RT_LMT_SH);

    curDesc->Dword4 = SET_DESC(curDesc->Dword4,
                                  DataRate,
                                  TX_DW4_DATARATE_MSK,
                                  TX_DW4_DATARATE_SH);

    curDesc->Dword4 = SET_DESC(curDesc->Dword4,
                                  RtsRate,
                                  TX_DW4_RTSRATE_MSK,
                                  TX_DW4_RTSRATE_SH);

    //4 Set Dword5
    curDesc->Dword5 = SET_DESC(curDesc->Dword5,
                                  (MacSettingGen.tx_discriptor.data_short),
                                  TX_DW5_DATA_SHORT_MSK,
                                  TX_DW5_DATA_SHORT_SH);

    curDesc->Dword5 = SET_DESC(curDesc->Dword5,
                                  (MacSettingGen.tx_discriptor.ampdu_support),
                                  TX_DW5_RTS_SHORT_MSK,
                                  TX_DW5_RTS_SHORT_SH);

    //4 Set Dword8
    curDesc->Dword8 = SET_DESC(curDesc->Dword8,
                                  (MacSettingGen.tx_discriptor.rts_short),
                                  TX_DW8_EN_HWSEQ_MSK,
                                  TX_DW8_EN_HWSEQ_SH);


    //4 Set Dword9
    curDesc->Dword9 = SET_DESC(curDesc->Dword9,
                                  GroupBitIEOffset,
                                  TX_DW9_GROUPBIT_IE_OFFSET_MSK,
                                  TX_DW9_GROUPBIT_IE_OFFSET_SH);

    curDesc->Dword9 = SET_DESC(curDesc->Dword9,
                                  GroupBitIEEn,
                                  TX_DW9_GROUPBIT_IE_ENABLE_MSK,
                                  TX_DW9_GROUPBIT_IE_ENABLE_SH);

    curDesc->Dword9 = SET_DESC(curDesc->Dword9,
                                  Seq,
                                  TX_DW9_SEQ_MSK,
                                  TX_DW9_SEQ_SH);

	if(MacSettingGen.tx_discriptor.tx_desc_check_sum)
		Chk_Sum(curDesc);

//1 Check early mode control
/*
	if(USB_EARLY_MODE)
	{
		memset(curExtendDesc,0x00,sizeof(TX_EXTEND_DESC));

		if(!USB_EARLY_MODE_EXTEND)	
		{
			if(MacSettingGen.tx_discriptor.agg_num==0) 
				MacSettingGen.tx_discriptor.agg_num=0;//1 rand()%5+1;

			curExtendDesc->Pkt_num=MacSettingGen.tx_discriptor.agg_num;

			if(MacSettingGen.tx_discriptor.agg_num>=1) {
                curExtendDesc->Len0=curDesc->Dword0.txpktsize;
			}
            
			if(MacSettingGen.tx_discriptor.agg_num>=2) {
                curExtendDesc->Len1=curDesc->Dword0.txpktsize;
			}
            
			if(MacSettingGen.tx_discriptor.agg_num>=3){
				curExtendDesc->Len2_p1=curDesc->Dword0.txpktsize;
				curExtendDesc->Len2_p2=curDesc->Dword0.txpktsize>>4;
			}
            
			if(MacSettingGen.tx_discriptor.agg_num>=4) {
                curExtendDesc->Len3=curDesc->Dword0.txpktsize;
			}
            
			if(MacSettingGen.tx_discriptor.agg_num>=5) {
                curExtendDesc->Len4=curDesc->Dword0.txpktsize;
			}
		}
		else
		{
			if(MacSettingGen.tx_discriptor.agg_num==0) 
				MacSettingGen.tx_discriptor.agg_num=0;//1 rand()%10+1;

			curExtendDesc->Pkt_num=MacSettingGen.tx_discriptor.agg_num;		   

			if(MacSettingGen.tx_discriptor.agg_num>=1) {
                curExtendDesc->Len0=curDesc->Dword0.txpktsize;
			}
            
			if(MacSettingGen.tx_discriptor.agg_num>=2) {
                curExtendDesc->Len0+=curDesc->Dword0.txpktsize;
			}

			if(MacSettingGen.tx_discriptor.agg_num>=3) {
                curExtendDesc->Len1=curDesc->Dword0.txpktsize;
			}
            
			if(MacSettingGen.tx_discriptor.agg_num>=4) {
                curExtendDesc->Len1+=curDesc->Dword0.txpktsize;
			}

			if(MacSettingGen.tx_discriptor.agg_num==5) {
				curExtendDesc->Len2_p1=curDesc->Dword0.txpktsize;
				curExtendDesc->Len2_p2=(curDesc->Dword0.txpktsize)>>4;
			}
			else if(MacSettingGen.tx_discriptor.agg_num>=6) {
				curExtendDesc->Len2_p1=curDesc->Dword0.txpktsize*2;
				curExtendDesc->Len2_p2=(curDesc->Dword0.txpktsize*2)>>4;
			}

			if(MacSettingGen.tx_discriptor.agg_num>=7) {
                curExtendDesc->Len3=curDesc->Dword0.txpktsize;
			}
            
			if(MacSettingGen.tx_discriptor.agg_num>=8) {
                curExtendDesc->Len3+=curDesc->Dword0.txpktsize;
			}
			if(MacSettingGen.tx_discriptor.agg_num>=9) {
                curExtendDesc->Len4=curDesc->Dword0.txpktsize;
			}
            
			if(MacSettingGen.tx_discriptor.agg_num>=10) {
                curExtendDesc->Len4+=curDesc->Dword0.txpktsize;        
			}

		}
	}	
*/

    pDw = &curDesc->Dword0;
    for (DwIndex=0;DwIndex<10;DwIndex++) {
        *pDw = CPUTOLEX32((u4Byte)*pDw);
        pDw++;
    }

	return TRUE;
}


VOID 
InitHCIDmaHAL8881A (
    VOID
)
{   
    u4Byte Value32 = 0;
    u2Byte Value16 = 0;
    u1Byte RxEntryIndex = 0;
#if 0
    u4Byte heap_addr=((u4Byte)RxBuf&(~7))+8 ;
    u4Byte heap_end=heap_addr+sizeof(RxBuf)-8;
    dprintf("Heap Top Address: 0x%x\n",heap_addr);
    dprintf("Heap Bottom Address: 0x%x\n",heap_end);
    i_alloc((VOID *)heap_addr, heap_end);
#endif

//    Value32 = (u4Byte)&hci_dma_buf;
    hci_dma_manager = (HCI_DMA_MANAGER *)(((u4Byte)&hci_dma_buf)|0xa0000000);

//    Value32 = (u4Byte)RxBuf;

    
    //Initial HCI DMA Manager
    memset(hci_dma_manager, 0x0, sizeof(HCI_DMA_MANAGER));

    //LinkBDToRing();

    //Disable Stop RX DMA
    Value32 = HAL_RTL_R32(REG_LX_CTRL1);
    Value32 = Value32 & (~(0x100));
    HAL_RTL_W32(REG_LX_CTRL1,Value32);

#ifdef CONFIG_TRX_DESC_SWAP_SUPPORT
    //Enable TX/RX Desc Hw Swap function
    Value32 = WL_IO_READ32(REG_LX_CTRL1);
    Value32 = Value32 | (0xc000000);
    WL_IO_WRITE32(REG_LX_CTRL1,Value32);
#endif

    //Clear ISR
    HAL_RTL_W32(REG_LX_DMA_ISR,0xFFFFFF);

    dprintf("TXBD Segment Number: %d\n",MAX_TXBD_SEQMENT_NUM);

    //3 ===Set Desciptor Number and Desciptor Mode===
    Value16 = BIT_MAX_MGQ_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_MGQ_DESC_NUM(TX_MGQ_DESC_NUM);

    HAL_RTL_W16(REG_MGQ_TXBD_NUM, Value16);


    Value16 = BIT_MAX_BCNQ_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_RXQ_DESC_NUM(RX_Q_DESC_NUM);

    HAL_RTL_W16(REG_RX_RXBD_NUM, Value16);
    

    Value16 = BIT_MAX_VOQ_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_VOQ_DESC_NUM(TX_VOQ_DESC_NUM);

    HAL_RTL_W16(REG_VOQ_TXBD_NUM, Value16);


    Value16 = BIT_MAX_VIQ_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_VIQ_DESC_NUM(TX_VIQ_DESC_NUM);

    HAL_RTL_W16(REG_VIQ_TXBD_NUM, Value16);


    Value16 = BIT_MAX_BEQ_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_BEQ_DESC_NUM(TX_BEQ_DESC_NUM);

    HAL_RTL_W16(REG_BEQ_TXBD_NUM, Value16);


    Value16 = BIT_MAX_BKQ_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_BKQ_DESC_NUM(TX_BKQ_DESC_NUM);

    HAL_RTL_W16(REG_BKQ_TXBD_NUM, Value16);


    Value16 = BIT_MAX_HI0Q_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_HI0Q_DESC_NUM(TX_H0Q_DESC_NUM);

    HAL_RTL_W16(REG_HI0Q_TXBD_NUM, Value16);


    Value16 = BIT_MAX_HI1Q_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_HI1Q_DESC_NUM(TX_H1Q_DESC_NUM);

    HAL_RTL_W16(REG_HI1Q_TXBD_NUM, Value16);


    Value16 = BIT_MAX_HI2Q_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_HI2Q_DESC_NUM(TX_H2Q_DESC_NUM);

    HAL_RTL_W16(REG_HI2Q_TXBD_NUM, Value16);


    Value16 = BIT_MAX_HI3Q_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_HI3Q_DESC_NUM(TX_H3Q_DESC_NUM);

    HAL_RTL_W16(REG_HI3Q_TXBD_NUM, Value16);


    Value16 = BIT_MAX_HI4Q_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_HI4Q_DESC_NUM(TX_H4Q_DESC_NUM);

    HAL_RTL_W16(REG_HI4Q_TXBD_NUM, Value16);


    Value16 = BIT_MAX_HI5Q_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_HI5Q_DESC_NUM(TX_H5Q_DESC_NUM);

    HAL_RTL_W16(REG_HI5Q_TXBD_NUM, Value16);


    Value16 = BIT_MAX_HI6Q_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_HI6Q_DESC_NUM(TX_H6Q_DESC_NUM);

    HAL_RTL_W16(REG_HI6Q_TXBD_NUM, Value16);


    Value16 = BIT_MAX_HI7Q_DESC_MODE(TX_DESC_MODE) |
              BIT_MAX_HI7Q_DESC_NUM(TX_H7Q_DESC_NUM);

    HAL_RTL_W16(REG_HI7Q_TXBD_NUM, Value16);

    //3 ===Set Desciptor Base Address===
    HAL_RTL_W32(REG_VIQ_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->ViqTXBD));

    HAL_RTL_W32(REG_VOQ_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->VoqTXBD));
    
    HAL_RTL_W32(REG_BEQ_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->BeqTXBD));

    HAL_RTL_W32(REG_BKQ_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->BkqTXBD));
    
    HAL_RTL_W32(REG_BCNQ_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->BcnqTXBD));

    HAL_RTL_W32(REG_MGQ_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->MgqTXBD));

    HAL_RTL_W32(REG_HI0Q_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->H0qTXBD));

    HAL_RTL_W32(REG_HI1Q_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->H1qTXBD));

    HAL_RTL_W32(REG_HI2Q_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->H2qTXBD));

    HAL_RTL_W32(REG_HI3Q_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->H3qTXBD));

    HAL_RTL_W32(REG_HI4Q_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->H4qTXBD));

    HAL_RTL_W32(REG_HI5Q_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->H5qTXBD));

    HAL_RTL_W32(REG_HI6Q_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->H6qTXBD));

    HAL_RTL_W32(REG_HI7Q_TXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->H7qTXBD));

    HAL_RTL_W32(REG_RXQ_RXBD_DESA,TOPHY_ADDRESS((u4Byte) hci_dma_manager->RXBD));

    //3 ===Initial RXDB RX Buffer Address===


    for (RxEntryIndex=0; RxEntryIndex<RX_Q_DESC_NUM; RxEntryIndex++) {
//        Value32 = (((u4Byte) &RxBuf[RxEntryIndex])|0xa0000000);
//        Value32 = (((u4Byte) osk_malloc(1024))|0xa0000000);
            RxBufEntry[RxEntryIndex].IsUsed = 1;
            Value32 = (((u4Byte) &(RxBufEntry[RxEntryIndex].RxBuf))|0xa0000000);
    
        RTL_DMA_DBG_LOG("Rx buffer address: 0x%x\n",Value32);
        Value32 = CPUTOLEX32(TOPHY_ADDRESS(Value32));
        hci_dma_manager->RXBD[RxEntryIndex].PhyAddr = Value32;
        Value32 = 0xFFFF0000;
//        Value32 = 0;
//        Value32 = CPUTOLEX32(Value32 | RX_BUF_LEN);        
        Value32 |= RX_BUF_LEN;
//        dprintf("Dw0: 0x%x\n",Value32);
        Value32 = CPUTOLEX32(Value32);        

        hci_dma_manager->RXBD[RxEntryIndex].Dword0 = Value32;
        
    }

    //Initial Expect Rx Tag
    hci_dma_manager->RxExpectTag = 0;
    
}
/*
VOID 
LinkBDToRing(
    VOID
)
{
   u4Byte  LastSeqmentNum = SEQMENT_NUM(OPTION0_SEQMENTS - 1);


   hci_dma_manager.ViqTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.ViqTXBD[0];
   hci_dma_manager.VoqTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.VoqTXBD[0];
   hci_dma_manager.BeqTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.BeqTXBD[0];
   hci_dma_manager.BkqTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.BkqTXBD[0];
   hci_dma_manager.MgqTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.MgqTXBD[0];
   hci_dma_manager.H0qTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.H0qTXBD[0];
   hci_dma_manager.H1qTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.H1qTXBD[0];
   hci_dma_manager.H2qTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.H2qTXBD[0];
   hci_dma_manager.H3qTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.H3qTXBD[0];
   hci_dma_manager.H4qTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.H4qTXBD[0];
   hci_dma_manager.H5qTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.H5qTXBD[0];
   hci_dma_manager.H6qTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.H6qTXBD[0];
   hci_dma_manager.H7qTXBD[LastSeqmentNum].AddrLow = &hci_dma_manager.H7qTXBD[0];


   
}
*/

u4Byte 
InitCfgNormallHAL8881A (
    VOID
)
{
    u4Byte   Value32 = 0;

    //Disable loopback mode
    Value32 = HAL_RTL_R32(REG_CR);
    Value32 &= (~MASK_LBMODE);
    HAL_RTL_W32(REG_CR, Value32);
    return;
}



u4Byte 
InitCfgSwBcnHAL8881A (
    VOID
)
{
    u4Byte   Value32 = 0;

    //Disable loopback mode
    Value32 = HAL_RTL_R32(REG_CR);
    Value32 &= (~MASK_LBMODE);
    Value32 |= 0x100;
    HAL_RTL_W32(REG_CR, Value32);

#if 0
    Value32 = HAL_RTL_R32(REG_TDECTRL);
    Value32 &= (~0xFF00);
    Value32 |= (BCN_PAGE_NUM<<8);
    HAL_RTL_W32(REG_TDECTRL, Value32);
#endif
    return;
}


u4Byte 
InitCfgDmaLBHAL8881A (
    VOID
)
{
    u4Byte   Value32 = 0;
    u4Byte   Chose  = 0;
    u1Byte   Value8 = 0;

    //==== Set LoopBack Mode
    Value32 = HAL_RTL_R32(REG_CR);
        
    Value32 &= ~MASK_LBMODE;
    switch(LbMode)
    {
        case MAC_LB:
            Chose = _LBMODE(0x0b); 
        break;
        case MAC_Delay_LB:
            Chose = _LBMODE(LOOPBACK_MAC_DELAY);
        break;
        case LB_Disable:
            Chose = _LBMODE(LOOPBACK_NORMAL);  
        break;
        default:
            Chose = _LBMODE(LOOPBACK_NORMAL);
        break;
    }
    //4 Set Loopback mode    
    Value32 |= Chose;

    //4 Disable Sw Beacon
    Value32 &= (~0x100);
    HAL_RTL_W32(REG_CR, Value32);

    if (MacSettingGen.tx_discriptor.ampdu_support) {
        Value32 = HAL_RTL_R32(REG_FAST_EDCA_CTRL);
        Value32 &= (~0xFF0000);
        Value32 |= (0xFF<<16);        
        HAL_RTL_W32(REG_FAST_EDCA_CTRL, Value32);
    }
    else {
        Value32 = HAL_RTL_R32(REG_FAST_EDCA_CTRL);
        Value32 &= (~0xFF0000);
        Value32 |= (0x08<<16);        
        HAL_RTL_W32(REG_FAST_EDCA_CTRL, Value32);
    }
}


u1Byte gMacAddr[6] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6};

VOID
InitMBSSID8881A(
    IN  u1Byte  IsRootInterface,
    IN  u1Byte  IsVapInterface,
    IN  u1Byte  vap_init_seq
)
{
    u4Byte         i, j;
    u4Byte         camData[2];
    u1Byte         *macAddr = gMacAddr;//HAL_VAR_MY_HWADDR;    
    u2Byte         dot11BeaconPeriod = 100;
    u1Byte         dot11DTIMPeriod = 1;
//    u1Byte          vap_init_seq;

	if (IsRootInterface)
	{
		camData[0] = MBIDCAM_POLL | MBIDWRITE_EN | MBIDCAM_VALID | (macAddr[5] << 8) | macAddr[4];
		camData[1] = (macAddr[3] << 24) | (macAddr[2] << 16) | (macAddr[1] << 8) | macAddr[0];

#if 0
		for (j=1; j>=0; j--) {
			HAL_RTL_W32((REG_MBIDCAMCFG+4)-4*j, camData[j]);
		}
#else
        HAL_RTL_W32((REG_MBIDCAMCFG), camData[1]);
        HAL_RTL_W32((REG_MBIDCAMCFG+4), camData[0]);
#endif
		// clear the rest area of CAM
		camData[1] = 0;
		for (i=1; i<8; i++) {
			camData[0] = MBIDCAM_POLL | MBIDWRITE_EN | (i&MBIDCAM_ADDR_Mask)<<MBIDCAM_ADDR_SHIFT;
#if 0
            for (j=1; j>=0; j--) {
                HAL_RTL_W32((REG_MBIDCAMCFG+4)-4*j, camData[j]);
            }
#else
            HAL_RTL_W32((REG_MBIDCAMCFG), camData[1]);
            HAL_RTL_W32((REG_MBIDCAMCFG+4), camData[0]);
#endif
		}

		// set MBIDCTRL & MBID_BCN_SPACE by cmd
		HAL_RTL_W32(REG_MBSSID_BCN_SPACE,
			(dot11BeaconPeriod & BCN_SPACE2_Mask)<<BCN_SPACE2_SHIFT
			|(dot11BeaconPeriod & BCN_SPACE1_Mask)<<BCN_SPACE1_SHIFT);

		HAL_RTL_W8(REG_BCN_CTRL, 0);
		HAL_RTL_W8(REG_DUAL_TSF_RST, 1);

        HAL_RTL_W8(REG_BCN_CTRL, EN_BCN_FUNCTION | DIS_SUB_STATE_N | DIS_TSF_UPDATE_N|EN_TXBCN_RPT);

		HAL_RTL_W32(REG_RCR, HAL_RTL_R32(REG_RCR) | RCR_MBID_EN);	// MBSSID enable
		
	}
	else if (IsVapInterface)
	{
		vap_init_seq = HAL_RTL_R8(REG_MBID_NUM) & MBID_BCN_NUM_Mask;
		vap_init_seq++;

        dprintf("init swq=%d\n", vap_init_seq);

		switch (vap_init_seq)
		{
			case 1:
				HAL_RTL_W8(REG_ATIMWND1, 0x3C);		
				HAL_RTL_W8(REG_DTIM_COUNTER_VAP1, dot11DTIMPeriod-1);
				break;
			case 2:
				HAL_RTL_W8(REG_ATIMWND2, 0x3C);
				HAL_RTL_W8(REG_DTIM_COUNTER_VAP2, dot11DTIMPeriod);
				break;
			case 3:
				HAL_RTL_W8(REG_ATIMWND3, 0x3C);
				HAL_RTL_W8(REG_DTIM_COUNTER_VAP3, dot11DTIMPeriod);
				break;
			case 4:
				HAL_RTL_W8(REG_ATIMWND4, 0x3C);
				HAL_RTL_W8(REG_DTIM_COUNTER_VAP4, dot11DTIMPeriod);
				break;
			case 5:
				HAL_RTL_W8(REG_ATIMWND5, 0x3C);
				HAL_RTL_W8(REG_DTIM_COUNTER_VAP5, dot11DTIMPeriod);
				break;
			case 6:
				HAL_RTL_W8(REG_ATIMWND6, 0x3C);
				HAL_RTL_W8(REG_DTIM_COUNTER_VAP6, dot11DTIMPeriod);
				break;
			case 7:
				HAL_RTL_W8(REG_ATIMWND7, 0x3C);
				HAL_RTL_W8(REG_DTIM_COUNTER_VAP7, dot11DTIMPeriod);
				break;
            default:
                dprintf("Invalid init swq=%d\n", dot11DTIMPeriod);
                break;
		}
		
		camData[0] = MBIDCAM_POLL | MBIDWRITE_EN | MBIDCAM_VALID |
				(vap_init_seq & MBIDCAM_ADDR_Mask)<<MBIDCAM_ADDR_SHIFT |
				(macAddr[5] << 8) | macAddr[4];
		camData[1] = (macAddr[3] << 24) | (macAddr[2] << 16) | (macAddr[1] << 8) | macAddr[0];
#if 0
        for (j=1; j>=0; j--) {
            HAL_RTL_W32((REG_MBIDCAMCFG+4)-4*j, camData[j]);
        }
#else
        HAL_RTL_W32((REG_MBIDCAMCFG), camData[1]);
        HAL_RTL_W32((REG_MBIDCAMCFG+4), camData[0]);
#endif

        HAL_RTL_W32(REG_MBSSID_BCN_SPACE,
			((dot11BeaconPeriod-
			((dot11BeaconPeriod/(vap_init_seq+1))*vap_init_seq))
			& BCN_SPACE2_Mask)<<BCN_SPACE2_SHIFT
			|((dot11BeaconPeriod/(vap_init_seq+1)) & BCN_SPACE1_Mask)
			<<BCN_SPACE1_SHIFT);

		HAL_RTL_W8(REG_BCN_CTRL, 0);
		HAL_RTL_W8(REG_DUAL_TSF_RST, 1);

        HAL_RTL_W8(REG_BCN_CTRL, EN_BCN_FUNCTION | DIS_SUB_STATE_N | DIS_TSF_UPDATE_N|EN_TXBCN_RPT);

        HAL_RTL_W8(REG_MBID_NUM, (HAL_RTL_R8(REG_MBID_NUM) & ~MBID_BCN_NUM_Mask) | (vap_init_seq & MBID_BCN_NUM_Mask));
        
		HAL_RTL_W32(REG_RCR, HAL_RTL_R32(REG_RCR) & ~RCR_MBID_EN);
		HAL_RTL_W32(REG_RCR, HAL_RTL_R32(REG_RCR) | RCR_MBID_EN);	// MBSSID enable
	}
}


VOID
StopMBSSID8881A(
    IN  u1Byte  IsRootInterface,
    IN  u1Byte  IsVapInterface,
    IN  u1Byte  vap_init_seq
)
{
    u4Byte          i, j;
    u4Byte          camData[2];
    u2Byte          dot11BeaconPeriod=100;
    u1Byte          dot11DTIMPeriod=1;


    
    camData[1] = 0;

    if (IsRootInterface)
    {
        // clear the rest area of CAM
        for (i=0; i<8; i++) {
            camData[0] = MBIDCAM_POLL | MBIDWRITE_EN | (i&MBIDCAM_ADDR_Mask)<<MBIDCAM_ADDR_SHIFT;
            for (j=1; j>=0; j--) {
                HAL_RTL_W32((REG_MBIDCAMCFG+4)-4*j, camData[j]);
            }
        }

        HAL_RTL_W32(REG_RCR, HAL_RTL_R32(REG_RCR) & ~RCR_MBID_EN);  // MBSSID disable
        HAL_RTL_W32(REG_MBSSID_BCN_SPACE,
            (dot11BeaconPeriod & BCN_SPACE1_Mask)<<BCN_SPACE1_SHIFT);

        HAL_RTL_W8(REG_BCN_CTRL, 0);
        HAL_RTL_W8(0x553, 1);
        HAL_RTL_W8(REG_BCN_CTRL, EN_BCN_FUNCTION | DIS_SUB_STATE_N | DIS_TSF_UPDATE_N| EN_TXBCN_RPT);

    }
    else if (IsVapInterface && (vap_init_seq >= 0))//3 Need confirm vap_init_seq type
    {
        camData[0] = MBIDCAM_POLL | MBIDWRITE_EN |
                        (vap_init_seq & MBIDCAM_ADDR_Mask)<<MBIDCAM_ADDR_SHIFT;
        for (j=1; j>=0; j--) {
            HAL_RTL_W32((REG_MBIDCAMCFG+4)-4*j, camData[j]);
        }

        if (HAL_RTL_R8(REG_MBID_NUM) & MBID_BCN_NUM_Mask) {
            HAL_RTL_W8(REG_MBID_NUM, ((HAL_RTL_R8(REG_MBID_NUM) & MBID_BCN_NUM_Mask)-1) & MBID_BCN_NUM_Mask);
            
            HAL_RTL_W32(REG_MBSSID_BCN_SPACE,
            ((dot11BeaconPeriod-
            ((dot11BeaconPeriod/((HAL_RTL_R8(REG_MBID_NUM) & MBID_BCN_NUM_Mask)+1))*(HAL_RTL_R8(REG_MBID_NUM)&MBID_BCN_NUM_Mask)))
            & BCN_SPACE2_Mask)<<BCN_SPACE2_SHIFT
            |((dot11BeaconPeriod/((HAL_RTL_R8(REG_MBID_NUM) & MBID_BCN_NUM_Mask)+1)) & BCN_SPACE1_Mask)
            <<BCN_SPACE1_SHIFT);

            HAL_RTL_W8(REG_BCN_CTRL, 0);
            HAL_RTL_W8(0x553, 1);
            HAL_RTL_W8(REG_BCN_CTRL, EN_BCN_FUNCTION | DIS_SUB_STATE_N | DIS_TSF_UPDATE_N| EN_TXBCN_RPT);
        }
        
        HAL_RTL_W32(REG_RCR, HAL_RTL_R32(REG_RCR) & ~RCR_MBID_EN);
        HAL_RTL_W32(REG_RCR, HAL_RTL_R32(REG_RCR) | RCR_MBID_EN);
        vap_init_seq = -1;
    }
}

