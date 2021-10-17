/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Hal88XXFirmware.c
	
Abstract:
	Defined RTL88XX Firmware Related Function
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2012-04-11 Filen            Create.	
--*/
#if !defined(__ECOS) && !defined(CPTCFG_CFG80211_MODULE)
#include "HalPrecomp.h"
#else
#include "../HalPrecomp.h"
#endif

#if (IS_RTL8881A_SERIES || IS_RTL8192E_SERIES || IS_RTL8814A_SERIES)
// c2h callback function register here
struct cmdobj	HalC2Hcmds[] = 
{
    {0, NULL},                                                 //0x0 not register yet 
    {0, NULL},                                                 //0x1 not register yet 
    {0, NULL},                                                 //0x2 not register yet 
    {0, NULL},                                                 //0x3 not register yet 
	GEN_FW_CMD_HANDLER(sizeof(APREQTXRPT), APReqTXRpt)	    //0x4 APReqTXRptHandler
};
#endif // #if (IS_RTL8881A_SERIES || IS_RTL8192E_SERIES )


#if (IS_RTL8814A_SERIES)
//
//3 Download Firmware
//

u2Byte checksum(u1Byte *idx, u4Byte len) //Cnt - count of bytes
{

	u4Byte	i;
	u4Byte	val, val_32;
	u1Byte	val_8[4];
 	u1Byte	checksum[4]={0,0,0,0};
	u4Byte	checksum32 = 0;

	for(i = 0; i< len; i+=4, idx+=4)
	{        
			val_8[0] =	*idx;
			val_8[1] =	*(idx+1);
			val_8[2] =	*(idx+2);
			val_8[3] =	*(idx+3);

		checksum[0] ^= val_8[0];
		checksum[1] ^= val_8[1];
		checksum[0] ^= val_8[2];
		checksum[1] ^= val_8[3]; 

	}
	checksum32 = checksum[0] |(checksum[1]<<8);		

	printk("sofetware check sum = %x !!!!!!!!!!!!!!!!!!\n ",checksum32 );
	
	return checksum32;
}



VOID
ReadMIPSFwHdr88XX(
    IN  HAL_PADAPTER    Adapter
)
{
    PRTL88XX_MIPS_FW_HDR pfw_hdr;
    pfw_hdr = (PRTL88XX_MIPS_FW_HDR)_GET_HAL_DATA(Adapter)->PFWHeader;

#ifdef _BIG_ENDIAN_
	Adapter->pshare->fw_signature= le16_to_cpu(pfw_hdr->signature);
	Adapter->pshare->fw_version = le16_to_cpu(pfw_hdr->version);
	//pfw_hdr->SVN_index = le32_to_cpu(pfw_hdr->SVN_index); 
	//pfw_hdr->Year = le16_to_cpu(pfw_hdr->Year); 
#endif

	Adapter->pshare->fw_category	= pfw_hdr->category;
	Adapter->pshare->fw_function	= pfw_hdr->function;
	Adapter->pshare->fw_sub_version = pfw_hdr->Subversion;
	Adapter->pshare->fw_date_month	= pfw_hdr->Month;
	Adapter->pshare->fw_date_day	= pfw_hdr->Date;
	Adapter->pshare->fw_date_hour	= pfw_hdr->Hour;
	Adapter->pshare->fw_date_minute = pfw_hdr->Min;
	
    RT_TRACE_F(COMP_INIT, DBG_TRACE ,("FW version = %x \n", pfw_hdr->version));
    RT_TRACE_F(COMP_INIT, DBG_TRACE ,("FW release at %x/%x \n",     pfw_hdr->Month, pfw_hdr->Date));    
}

VOID
ConfigBeforeDLFW(
    IN  HAL_PADAPTER    Adapter,
    IN  pu4Byte         temBuf    
)
{
    u4Byte tmpVal;
	//Enable SW TX beacon
	temBuf[0] =  HAL_RTL_R32(REG_CR);
    HAL_RTL_W32(REG_CR,temBuf[0] | BIT_ENSWBCN);

	//Disable BIT_EN_BCN_FUNCTION
	temBuf[1] =  HAL_RTL_R32(REG_BCN_CTRL);	
    tmpVal = (temBuf[1] | BIT_DIS_TSF_UDT) & (~BIT_EN_BCN_FUNCTION);
    HAL_RTL_W32(REG_BCN_CTRL,tmpVal);    

    temBuf[2] =  HAL_RTL_R32(REG_FWHW_TXQ_CTRL);  
    HAL_RTL_W32(REG_FWHW_TXQ_CTRL,temBuf[2] & ~BIT22);

    // set beacon header to page 0
    temBuf[3] =  HAL_RTL_R32(REG_FIFOPAGE_CTRL_2);   

    tmpVal = temBuf[3] & 0xF000;
    HAL_RTL_W32(REG_FIFOPAGE_CTRL_2,tmpVal);     

    // Disable MCU Core (CPU RST)
    temBuf[4] =  (u2Byte)HAL_RTL_R16(REG_SYS_FUNC_EN);  
    tmpVal = temBuf[4] & (~BIT_FEN_CPUEN);
    //tmpVal = tmpVal & (~BIT_FEN_BBRSTB);
    //tmpVal = tmpVal & (~BIT_FEN_BB_GLB_RSTn);
    HAL_RTL_W16(REG_SYS_FUNC_EN,tmpVal);    

    SetHwReg88XX(Adapter, HW_VAR_ENABLE_BEACON_DMA, NULL);

#ifdef ENABLE_PCIE_MULTI_TAG
	// Do nothing
#else
	// disable multi-tag
	HAL_RTL_W8(REG_HCI_MIX_CFG,0);
#endif

	tmpVal = HAL_RTL_R32(REG_8051FW_CTRL);
	tmpVal = (tmpVal & (~0x7ff));
	tmpVal = (tmpVal & ~(BIT_FW_DW_RDY|BIT_FW_INIT_RDY));
	tmpVal = (tmpVal | BIT_MCUFWDL_EN);

	// set 3081 CPU clock to 100M
	tmpVal = tmpVal & 0xFFFFCFFF;    
	tmpVal = tmpVal | BIT_CPU_CLK_SEL(2);
	DEBUG_INFO("set 3081 CPU clock to 100M !!! \n");

	HAL_RTL_W32(REG_8051FW_CTRL,tmpVal);
}

VOID
RestoreAfterDLFW(
    IN  HAL_PADAPTER    Adapter,
    IN  pu4Byte         temBuf    
)
{
    u4Byte tmpVal,count = 0;

    SetBeaconDownload88XX(Adapter, HW_VAR_BEACON_DISABLE_DOWNLOAD);

    // Restore beacon header to page 0
    HAL_RTL_W32(REG_FIFOPAGE_CTRL_2,temBuf[3]);    
    
	//Restore SW TX beacon
    HAL_RTL_W32(REG_CR,temBuf[0]);

	//Restore BIT_EN_BCN_FUNCTION
    HAL_RTL_W32(REG_BCN_CTRL,temBuf[1]);    

#if !CFG_HAL_MAC_LOOPBACK
    HAL_RTL_W32(REG_FWHW_TXQ_CTRL,temBuf[2]);
#endif  //#if !CFG_HAL_MAC_LOOPBACK


    // Restore MCU Core (CPU RST)
    HAL_RTL_W16(REG_SYS_FUNC_EN,(u2Byte)temBuf[4]);  

}

RT_STATUS 
InitDDMA88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  u4Byte	source,
    IN  u4Byte	dest,
    IN  u4Byte 	length
)
{
	// TODO: Replace all register define & bit define
	
	u4Byte	ch0ctrl = (DDMA_CHKSUM_EN|DDMA_CH_OWN );
	u4Byte	cnt=5000;
	u1Byte	tmp;
	
	//check if ddma ch0 is idle
	while(HAL_RTL_R32(REG_DDMA_CH0CTRL)&DDMA_CH_OWN){
		delay_ms(1);
		cnt--;
		if(cnt==0){			
            printk("InitDDMA88XX polling fail \n");
			return RT_STATUS_FAILURE;
		}
	}
	
	
	ch0ctrl |= length & DDMA_LEN_MASK;
	
	//check if chksum continuous
    ch0ctrl |= DDMA_CH_CHKSUM_CNT;
		
	HAL_RTL_W32(REG_DDMA_CH0SA, source);
	HAL_RTL_W32(REG_DDMA_CH0DA, dest);
	HAL_RTL_W32(REG_DDMA_CH0CTRL, ch0ctrl);

	return RT_STATUS_SUCCESS;
}

RT_STATUS 
DLImageCheckSum(
    IN  HAL_PADAPTER    Adapter
)
{
    u4Byte	cnt=5000;
	u1Byte	tmp;
	
	//check if ddma ch0 is idle
	while(HAL_RTL_R32(REG_DDMA_CH0CTRL)&DDMA_CH_OWN){
		delay_ms(1);
		cnt--;
		if(cnt==0){			
            printk("CNT error \n");
			return RT_STATUS_FAILURE;
		}
	}

    if(HAL_RTL_R32(REG_DDMA_CH0CTRL)&DDMA_CHKSUM_FAIL)
    {    
        printk("DDMA_CHKSUM_FAIL error = %x\n",HAL_RTL_R16(REG_DDMA_CHKSUM));        
        return RT_STATUS_FAILURE;
    }
    else
    {  
        return RT_STATUS_SUCCESS;
    }              
}

RT_STATUS
CheckMIPSFWImageSize(
    IN  HAL_PADAPTER    Adapter
)
{
    PRTL88XX_MIPS_FW_HDR pfw_hdr;
    u4Byte              imemSZ;
    u2Byte              dmemSZ;
    u4Byte imageSZ;
    

    pfw_hdr = (PRTL88XX_MIPS_FW_HDR)_GET_HAL_DATA(Adapter)->PFWHeader;

    // ReaddDMEM, check length
    dmemSZ = HAL_EF2Byte(pfw_hdr->FW_CFG_SZ)+MIPS_FW_CHKSUM_DUMMY_SZ;

    // ReadIMEM, check length
    imemSZ = HAL_EF4Byte(pfw_hdr->IRAM_SZ)+MIPS_FW_CHKSUM_DUMMY_SZ;

    GET_HAL_INTERFACE(Adapter)->GetHwRegHandler(Adapter, HW_VAR_FWFILE_SIZE, (pu1Byte)&imageSZ);

    if(imageSZ != (sizeof(RTL88XX_MIPS_FW_HDR)+ dmemSZ + imemSZ))
    {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, 
            ("FW size error ! Ima = %d Cal =%d \n",imageSZ,(sizeof(RTL88XX_MIPS_FW_HDR)+ imemSZ + dmemSZ)));
        return RT_STATUS_FAILURE;        
    }
    else
    {
        return RT_STATUS_SUCCESS;
    }
}

#if CFG_FW_VERIFICATION
RT_STATUS
VerifyRsvdPage88XX
(
    IN	HAL_PADAPTER        Adapter,
    IN  pu1Byte             pbuf,
    IN  u4Byte              frlen
)
{
    u4Byte i=0;
    HAL_RTL_W8(REG_PKT_BUFF_ACCESS_CTRL,0x69);


    // compare payload download to TXPKTBUF
    for(i=0;i<frlen;i++)
    {
	    HAL_RTL_W32(REG_PKTBUF_DBG_CTRL,0x780+((i+sizeof(TX_DESC_88XX))>>12));    		
        if(HAL_RTL_R8(0x8000+i+sizeof(TX_DESC_88XX)) != *((pu1Byte)pbuf+i))
        {
            RT_TRACE_F(COMP_INIT, DBG_WARNING, 
            ("Verify DLtxbuf fail at %x A=%x B=%x \n",i,HAL_RTL_R8(0x8000+i+sizeof(TX_DESC_88XX)),*((pu1Byte)pbuf+i)));

            return RT_STATUS_FAILURE;
        }        
    }

    return RT_STATUS_SUCCESS;
}
#endif //#if CFG_FW_VERIFICATION

RT_STATUS
DLtoTXBUFandDDMA88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  pu1Byte         pbuf,    
    IN  u4Byte          len,        
    IN  u4Byte          offset,
    IN  u1Byte          target
)
{
    SigninBeaconTXBD88XX(Adapter,(pu4Byte)pbuf, len);    // fill TXDESC & TXBD

    //RT_PRINT_DATA(COMP_INIT, DBG_LOUD, "DLtoTXBUFandDDMA88XX\n", pbuf, len);

    if(_FALSE == DownloadRsvdPage88XX(Adapter,NULL,0,0)) {
        RT_TRACE_F(COMP_INIT, DBG_WARNING,("Download to TXpktbuf fail ! \n"));
        return RT_STATUS_FAILURE;
    }

#if CFG_FW_VERIFICATION
    if(RT_STATUS_SUCCESS != VerifyRsvdPage88XX(Adapter,pbuf, len)) {
          RT_TRACE_F(COMP_INIT, DBG_WARNING,("VerifyRsvdPage88XX fail ! \n"));        
          return RT_STATUS_FAILURE;
    }
#endif //#if CFG_FW_VERIFICATION    

    if(target == MIPS_DL_DMEM) // DMEM
    {
        if(RT_STATUS_SUCCESS != InitDDMA88XX(Adapter,OCPBASE_TXBUF+sizeof(TX_DESC_88XX),OCPBASE_DMEM+offset,len)) {
            RT_TRACE_F(COMP_INIT, DBG_WARNING,("DDMA to DMEM fail ! \n"));        
            return RT_STATUS_FAILURE;
        }      
    }else {
        if(RT_STATUS_SUCCESS != InitDDMA88XX(Adapter,OCPBASE_TXBUF+sizeof(TX_DESC_88XX),OCPBASE_IMEM+offset,len)) {
            RT_TRACE_F(COMP_INIT, DBG_WARNING,("DDMA to IMEM fail ! \n"));                    
            return RT_STATUS_FAILURE;
        }      
    }

    return RT_STATUS_SUCCESS;
}


RT_STATUS
checkFWSizeAndDLFW88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  pu1Byte         pbuf,
    IN  u4Byte          len,    
    IN  u1Byte          target 
)
{
    u4Byte                  downloadcnt = 0;    
    u4Byte                  offset = 0;          

    while(1){
        if(len > MIPS_MAX_FWBLOCK_DL_SIZE){

            // the offset in DMEM
            offset = downloadcnt*MIPS_MAX_FWBLOCK_DL_SIZE;

            // download len = limitation length
            if(RT_STATUS_SUCCESS != DLtoTXBUFandDDMA88XX(Adapter,pbuf,MIPS_MAX_FWBLOCK_DL_SIZE,offset,target)) {
               return RT_STATUS_FAILURE;
            } 

            // pointer 
            //pbuf += MIPS_MAX_FWBLOCK_DL_SIZE/sizeof(u4Byte);
            pbuf += MIPS_MAX_FWBLOCK_DL_SIZE;
            len = len - MIPS_MAX_FWBLOCK_DL_SIZE;
            downloadcnt++;

            delay_ms(10);
        }
        else{
            offset = downloadcnt*MIPS_MAX_FWBLOCK_DL_SIZE;            

            if(RT_STATUS_SUCCESS != DLtoTXBUFandDDMA88XX(Adapter,pbuf,len,offset,target)) {
                return RT_STATUS_FAILURE;
            }   
            break;
        }
    }

    return RT_STATUS_SUCCESS;
}

RT_STATUS
InitMIPSFirmware88XX(
    IN  HAL_PADAPTER    Adapter
)
{
    PRTL88XX_MIPS_FW_HDR    pfw_hdr;
    pu1Byte                 pFWStart;
    pu1Byte                 imemPtr;    
    pu1Byte                 dmemPtr;        
    pu1Byte                 fwBufPtr;
    u4Byte                  temBuf[8];
    u4Byte                  imemSZ;
    u2Byte                  dmemSZ;
    u4Byte                  offset = 0;
    RT_STATUS status;
    PHAL_DATA_TYPE          pHalData    = _GET_HAL_DATA(Adapter);    

    pHalData->bFWReady = _FALSE;

    GET_HAL_INTERFACE(Adapter)->GetHwRegHandler(Adapter, HW_VAR_FWFILE_START, (pu1Byte)&pFWStart);
    //Register to HAL_DATA
    _GET_HAL_DATA(Adapter)->PFWHeader = (PVOID)pFWStart;

	ReadMIPSFwHdr88XX(Adapter);
	
    pfw_hdr = (PRTL88XX_MIPS_FW_HDR)pFWStart;
 

    if(RT_STATUS_SUCCESS != CheckMIPSFWImageSize(Adapter)) {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("FW size error ! \n"));
        return RT_STATUS_FAILURE;
    }

    // ReaddDMEM, check length
    dmemSZ = HAL_EF2Byte(pfw_hdr->FW_CFG_SZ)+MIPS_FW_CHKSUM_DUMMY_SZ;
   
    // ReadIMEM, check length
    imemSZ = HAL_EF4Byte(pfw_hdr->IRAM_SZ)+MIPS_FW_CHKSUM_DUMMY_SZ;
   
    dmemPtr = pFWStart + MIPS_FW_HEADER_SIZE;
    imemPtr = pFWStart + MIPS_FW_HEADER_SIZE + dmemSZ;

#if defined(CONFIG_NET_PCI) && defined(NOT_RTK_BSP)
    fwBufPtr = HALMalloc(Adapter, GET_MAX(dmemSZ, imemSZ));
    if (NULL == fwBufPtr) {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("Allocate FW buffer error ! \n"));
        return RT_STATUS_FAILURE;
    }
#endif
    status = RT_STATUS_FAILURE;

    ConfigBeforeDLFW(Adapter,(pu4Byte)&temBuf);

    // Download size limitation
    // IMEM: 128K, DMEM: 128K
    // download image -------> txpktbuf --------> IMEM/DMEM
    //                 txDMA               DDMA
    //          tx descriptor:64K          128K
    // Download limitation = 64K - sizeof(TX_DESC) 
    
#if defined(CONFIG_NET_PCI) && defined(NOT_RTK_BSP)
    HAL_memcpy(fwBufPtr, dmemPtr, dmemSZ);
#else
    fwBufPtr = dmemPtr;
#endif
    if(RT_STATUS_SUCCESS != checkFWSizeAndDLFW88XX(Adapter,fwBufPtr,dmemSZ,MIPS_DL_DMEM))
    {
        RestoreAfterDLFW(Adapter,(pu4Byte)&temBuf);    
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("%s %d DL DMEM fail ! \n",__FUNCTION__,__LINE__));
        goto out;
    }

    // verify checkSum
    if(RT_STATUS_SUCCESS != DLImageCheckSum(Adapter)) {
        HAL_RTL_W8(REG_8051FW_CTRL, HAL_RTL_R8(REG_8051FW_CTRL)|DMEM_CHKSUM_FAIL);
        RestoreAfterDLFW(Adapter,(pu4Byte)&temBuf);        
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("%s %d DL DMEM checkSum fail ! \n",__FUNCTION__,__LINE__));        
        goto out;
    }else {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("%s %d DL DMEM checkSum ok ! \n",__FUNCTION__,__LINE__));     
        HAL_RTL_W8(REG_8051FW_CTRL, ((HAL_RTL_R8(REG_8051FW_CTRL)&(~DMEM_CHKSUM_FAIL))|DMEM_DL_RDY));
    }

    // Copy IMEM
#if defined(CONFIG_NET_PCI) && defined(NOT_RTK_BSP)
    HAL_memcpy(fwBufPtr, imemPtr, imemSZ);
#else
    fwBufPtr = imemPtr;
#endif
    if(RT_STATUS_SUCCESS != checkFWSizeAndDLFW88XX(Adapter,fwBufPtr,imemSZ,MIPS_DL_IMEM))	
    {
        RestoreAfterDLFW(Adapter,(pu4Byte)&temBuf);        
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("%s %d DL IMEM fail ! \n",__FUNCTION__,__LINE__));  
        goto out;
    }    

    // verify checkSum
    if(RT_STATUS_SUCCESS != DLImageCheckSum(Adapter))    {
        HAL_RTL_W8(REG_8051FW_CTRL, HAL_RTL_R8(REG_8051FW_CTRL)|IMEM_CHKSUM_FAIL);
        RestoreAfterDLFW(Adapter,(pu4Byte)&temBuf);   
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("%s %d DL IMEM checkSum fail ! \n",__FUNCTION__,__LINE__));          
        goto out;
    }else {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("%s %d DL IMEM checkSum ok ! \n",__FUNCTION__,__LINE__));     		
        HAL_RTL_W8(REG_8051FW_CTRL, ((HAL_RTL_R8(REG_8051FW_CTRL)&(~IMEM_CHKSUM_FAIL))|IMEM_DL_RDY));
    }
    
    RestoreAfterDLFW(Adapter,(pu4Byte)&temBuf);
    HAL_RTL_W8(REG_8051FW_CTRL,HAL_RTL_R8(REG_8051FW_CTRL)&(~BIT_MCUFWDL_EN));
    HAL_RTL_W16(REG_SYS_FUNC_EN,HAL_RTL_R16(REG_SYS_FUNC_EN)|BIT_FEN_CPUEN);
    // wait CPU set ready flag 
    delay_ms(100);    
    if(HAL_RTL_R32(REG_8051FW_CTRL)&BIT_FW_INIT_RDY)
    {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("MIPS CPU Running \n"));        
        pHalData->bFWReady = _TRUE;
		_GET_HAL_DATA(Adapter)->H2CBufPtr88XX = 0;
    }else
    {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("MIPS CPU Not Running \n"));          
        goto out;
    }

    status = RT_STATUS_SUCCESS;

out:

#if defined(CONFIG_NET_PCI) && defined(NOT_RTK_BSP)
    HAL_free(fwBufPtr);
#endif

    return status;
}




#endif //(IS_RTL8814A_SERIES)

#if (IS_RTL8881A_SERIES || IS_RTL8192E_SERIES || IS_RTL8814A_SERIES )
//
//3 Download Firmware
//

static VOID
ReadFwHdr88XX(
    IN  HAL_PADAPTER    Adapter
)
{
#if 0
    PRTL88XX_FW_HDR pfw_hdr;
    pu1Byte     pFWStart;
    
    pfw_hdr = (PRTL88XX_FW_HDR)HALMalloc(Adapter, sizeof(RTL88XX_FW_HDR));
    if (NULL == pfw_hdr) {
        RT_TRACE(COMP_INIT, DBG_WARNING, ("ReadFwHdr88XX\n"));
        return;
    }

    GET_HAL_INTERFACE(Adapter)->GetHwRegHandler(Adapter, HW_VAR_FWFILE_START, (pu1Byte)&pFWStart);
    HAL_memcpy(pfw_hdr, pFWStart, RT_FIRMWARE_HDR_SIZE);
 
    //Register to HAL_DATA
    _GET_HAL_DATA(Adapter)->PFWHeader = pfw_hdr;
    RT_TRACE_F(COMP_INIT, DBG_TRACE ,("FW version = %x \n",pfw_hdr->version));
    RT_TRACE_F(COMP_INIT, DBG_TRACE ,("FW release at %x/%x \n",pfw_hdr->month,pfw_hdr->day));
    HAL_free(pfw_hdr);
#else
    PRTL88XX_FW_HDR pfw_hdr;
    pu1Byte     pFWStart;

    GET_HAL_INTERFACE(Adapter)->GetHwRegHandler(Adapter, HW_VAR_FWFILE_START, (pu1Byte)&pFWStart);
    //Register to HAL_DATA
    _GET_HAL_DATA(Adapter)->PFWHeader = (PVOID)pFWStart;
    pfw_hdr = (PRTL88XX_FW_HDR)pFWStart;
    
    RT_TRACE_F(COMP_INIT, DBG_TRACE ,("FW version = %x \n", pfw_hdr->version));
    RT_TRACE_F(COMP_INIT, DBG_TRACE ,("FW release at %x/%x \n", pfw_hdr->month, pfw_hdr->day));    
#endif
}

static VOID
WriteToFWSRAM88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  pu1Byte         pFWRealStart,
    IN  u4Byte          FWRealLen
)
{
    u4Byte      WriteAddr   = FW_DOWNLOAD_START_ADDRESS;
    u4Byte      CurPtr      = 0;
    u4Byte      Temp;
    
	while (CurPtr < FWRealLen) {
		if ((CurPtr+4) > FWRealLen) {
			// Reach the end of file.
			while (CurPtr < FWRealLen) {
				Temp = *(pFWRealStart + CurPtr);
				HAL_RTL_W8(WriteAddr, (u1Byte)Temp);
				WriteAddr++;
				CurPtr++;
			}
		} else {
			// Write FW content to memory.
			Temp = *((pu4Byte)(pFWRealStart + CurPtr));
			Temp = HAL_cpu_to_le32(Temp);
			HAL_RTL_W32(WriteAddr, Temp);
			WriteAddr += 4;

			if(WriteAddr == 0x2000) {
				u1Byte  tmp = HAL_RTL_R8(REG_8051FW_CTRL+2);
                
                //Switch to next page
				tmp += 1;
                //Reset Address
				WriteAddr = 0x1000;
                
				HAL_RTL_W8(REG_8051FW_CTRL+2, tmp);
			}
			CurPtr += 4;
		}
	}
}

static VOID
DownloadFWInit88XX(
    IN  HAL_PADAPTER    Adapter
)
{
   //Clear 0x80,0x81,0x82[0],0x82[1],0x82[2],0x82[3]
   HAL_RTL_W8(REG_8051FW_CTRL,0x0);
   HAL_RTL_W8(REG_8051FW_CTRL+1,0x0);

   // Enable MCU
   HAL_RTL_W8(REG_SYS_FUNC_EN+1, HAL_RTL_R8(REG_SYS_FUNC_EN+1) | BIT2);
   HAL_delay_ms(1);

   // Load SRAM
   HAL_RTL_W8(REG_8051FW_CTRL, HAL_RTL_R8(REG_8051FW_CTRL) | BIT_MCUFWDL_EN);
   HAL_delay_ms(1);

   //Clear ROM FPGA Related Parameter
   HAL_RTL_W32(REG_8051FW_CTRL, HAL_RTL_R32(REG_8051FW_CTRL) & 0xfff0ffff);
   delay_ms(1);
}

#if CFG_FW_VERIFICATION
static BOOLEAN
VerifyDownloadStatus88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  pu1Byte         pFWRealStart,
    IN  u4Byte          FWRealLen
)
{
    u4Byte      WriteAddr   = FW_DOWNLOAD_START_ADDRESS;
    u4Byte      CurPtr      = 0;
    u4Byte      binTemp;
    u4Byte      ROMTemp;    
    u1Byte      u1ByteTmp;


    // first clear page number 

    u1ByteTmp =  HAL_RTL_R8(REG_8051FW_CTRL+2);
    u1ByteTmp &= ~BIT0;
    u1ByteTmp &= ~BIT1;
    u1ByteTmp &= ~BIT2;    
    HAL_RTL_W8(REG_8051FW_CTRL+2, u1ByteTmp);
	delay_ms(1);

    // then compare FW image and download content
    CurPtr = 0;
         
	while (CurPtr < FWRealLen) {
		if ((CurPtr+4) > FWRealLen) {
			// Reach the end of file.
			while (CurPtr < FWRealLen) {
                if(HAL_RTL_R8(WriteAddr)!=((u1Byte)*(pFWRealStart + CurPtr)))
    			{
        		    RT_TRACE_F(COMP_INIT, DBG_LOUD,("Verify download fail at [%x] \n",WriteAddr));
                    return _FALSE;
    			}
				WriteAddr++;
				CurPtr++;
			}
		} else {
			// Comapre Download code with original binary

            binTemp = *((pu4Byte)(pFWRealStart + CurPtr));

			binTemp = HAL_cpu_to_le32(binTemp);
			ROMTemp = HAL_RTL_R32(WriteAddr);

			if(binTemp != ROMTemp)
			{
    		   RT_TRACE_F(COMP_INIT, DBG_LOUD,("Verify download fail at [0x%x] binTemp=%x,ROMTemp=%x \n",
                    WriteAddr,binTemp,ROMTemp));
                return _FALSE;
			}
			WriteAddr += 4;

			if(WriteAddr == 0x2000) {
				u1Byte  tmp = HAL_RTL_R8(REG_8051FW_CTRL+2);
                
                //Switch to next page
				tmp += 1;
                //Reset Address
				WriteAddr = 0x1000;
                
				HAL_RTL_W8(REG_8051FW_CTRL+2, tmp);
			}
			CurPtr += 4;
		}
	}
    return _TRUE;
}
#endif  //CFG_FW_VERIFICATION

static BOOLEAN
LoadFirmware88XX(
    IN  HAL_PADAPTER    Adapter
)
{
    pu1Byte     pFWRealStart;
    u4Byte      FWRealLen;
    u1Byte      u1ByteTmp;
    u1Byte      wait_cnt = 0;

    
    RT_TRACE_F(COMP_INIT, DBG_LOUD, ("\n"));

    GET_HAL_INTERFACE(Adapter)->GetHwRegHandler(Adapter, HW_VAR_FWFILE_START, (pu1Byte)&pFWRealStart);
	// get firmware info
#ifdef _BIG_ENDIAN_
    Adapter->pshare->fw_signature	= le16_to_cpu(*(unsigned short *)pFWRealStart);
	Adapter->pshare->fw_version		= le16_to_cpu(*(unsigned short *)(pFWRealStart+4));
#else
    Adapter->pshare->fw_signature	= *(unsigned short *)pFWRealStart;
	Adapter->pshare->fw_version		= *(unsigned short *)(pFWRealStart+4);
#endif
	Adapter->pshare->fw_category	= *(pFWRealStart+2);
	Adapter->pshare->fw_function	= *(pFWRealStart+3);
	Adapter->pshare->fw_sub_version	= *(pFWRealStart+6);
	Adapter->pshare->fw_date_month	= *(pFWRealStart+8);
	Adapter->pshare->fw_date_day	= *(pFWRealStart+9);
	Adapter->pshare->fw_date_hour	= *(pFWRealStart+10);
	Adapter->pshare->fw_date_minute	= *(pFWRealStart+11);
    pFWRealStart += RT_FIRMWARE_HDR_SIZE;

    GET_HAL_INTERFACE(Adapter)->GetHwRegHandler(Adapter, HW_VAR_FWFILE_SIZE, (pu1Byte)&FWRealLen);
    FWRealLen -= RT_FIRMWARE_HDR_SIZE;

    DownloadFWInit88XX(Adapter);

    WriteToFWSRAM88XX(Adapter, pFWRealStart, FWRealLen);

    u1ByteTmp = HAL_RTL_R8(REG_8051FW_CTRL);

    if ( u1ByteTmp & BIT_FWDL_CHK_RPT ) {
        RT_TRACE_F(COMP_INIT, DBG_TRACE , ("CheckSum Pass\n"));
    }
    else {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("CheckSum Failed\n"));
        return _FALSE;        
    }

#if CFG_FW_VERIFICATION    
    if(VerifyDownloadStatus88XX(Adapter,pFWRealStart,FWRealLen)==_TRUE)
    {
        RT_TRACE_F(COMP_INIT, DBG_TRACE,("download verify ok!\n"));
    }
    else
    {
        RT_TRACE_F(COMP_INIT, DBG_WARNING,("download verify fail!\n"));
        return _FALSE;
    }
#endif //#if CFG_FW_VERIFICATION

    // download and verify ok, clear download enable bit, and set MCU DL ready
    u1ByteTmp &= ~BIT_MCUFWDL_EN;
    u1ByteTmp |= BIT_MCUFWDL_RDY;
	HAL_RTL_W8(REG_8051FW_CTRL, u1ByteTmp);
	HAL_delay_ms(1);
    
    // reset MCU, 8051 will jump to RAM code    
	HAL_RTL_W8(REG_8051FW_CTRL+1, 0x00);

    // Disable WLMCU IO interface control
    HAL_RTL_W8(REG_RSV_CTRL+1, HAL_RTL_R8(REG_RSV_CTRL+1)&(~BIT0));
    HAL_RTL_W8(REG_SYS_FUNC_EN+1, HAL_RTL_R8(REG_SYS_FUNC_EN+1)&(~BIT2));
    HAL_delay_ms(1);
    HAL_RTL_W8(REG_RSV_CTRL+1, HAL_RTL_R8(REG_RSV_CTRL+1)| BIT0);  
    HAL_RTL_W8(REG_SYS_FUNC_EN+1, HAL_RTL_R8(REG_SYS_FUNC_EN+1) | BIT2);   


    RT_TRACE_F(COMP_INIT, DBG_WARNING , ("After download RAM reset MCU\n"));

	// Check if firmware RAM Code is ready
    while (!(HAL_RTL_R8(REG_8051FW_CTRL) & BIT_WINTINI_RDY)) {
        if (++wait_cnt > CHECK_FW_RAMCODE_READY_TIMES) {        
            RT_TRACE_F(COMP_INIT, DBG_WARNING, ("RAMCode Failed\n"));
            return _FALSE;
		}
        
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("Firmware is not ready, wait\n"));
        HAL_delay_ms(CHECK_FW_RAMCODE_READY_DELAY_MS);
    }

	_GET_HAL_DATA(Adapter)->H2CBufPtr88XX = 0;

    return _TRUE;
}


RT_STATUS
InitFirmware88XX(
    IN  HAL_PADAPTER    Adapter
)
{
    PHAL_DATA_TYPE      pHalData    = _GET_HAL_DATA(Adapter);    
    u4Byte              dwnRetry    = DOWNLOAD_FIRMWARE_RETRY_TIMES;
    BOOLEAN             bfwStatus   = _FALSE;

    ReadFwHdr88XX(Adapter);

    while(dwnRetry-- && !bfwStatus) {
        bfwStatus = LoadFirmware88XX(Adapter);
    }

    if ( _TRUE == bfwStatus ) {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("LoadFirmware88XX is Successful\n"));
        pHalData->bFWReady = _TRUE;
    }
    else {
        RT_TRACE_F(COMP_INIT, DBG_WARNING, ("LoadFirmware88XX failed\n"));
        pHalData->bFWReady = _FALSE;
        return RT_STATUS_FAILURE;
    }
    
    return RT_STATUS_SUCCESS;
}


static VOID
SetBcnCtrlReg88XX(
	IN HAL_PADAPTER     Adapter,	
	IN	u1Byte		    SetBits,
	IN	u1Byte		    ClearBits
	)
{
	u1Byte tmp = HAL_RTL_R8(REG_BCN_CTRL);

	tmp |=  SetBits;
	tmp &= ~ClearBits;

	HAL_RTL_W8(REG_BCN_CTRL, tmp);
}

static u1Byte
MRateIdxToARFRId88XX(
	IN HAL_PADAPTER     Adapter,
	u1Byte  			RateIdx,    //RATR_TABLE_MODE
	u1Byte	    		RfType
)
{
	u1Byte Ret = 0;
	
	switch(RateIdx){

	case RATR_INX_WIRELESS_NGB:
		if(RfType == MIMO_1T1R)
			Ret = 1;
		else if(RfType == MIMO_2T2R)
			Ret = 0;
		else if(RfType == MIMO_3T3R)// 3T
			Ret = 14;
		break;

	case RATR_INX_WIRELESS_N:
	case RATR_INX_WIRELESS_NG:
		if(RfType == MIMO_1T1R)
			Ret = 5;
		else if(RfType == MIMO_2T2R)
			Ret = 4;
		else if(RfType == MIMO_3T3R)// 3T // becarefull, share with 3T BGN mode, we need to change register setting
			Ret = 14;
		break;

	case RATR_INX_WIRELESS_NB:
		if(RfType == MIMO_1T1R)
			Ret = 3;
		else if(RfType == MIMO_2T2R)
			Ret = 2;
		else if(RfType == MIMO_3T3R)// 3T // becarefull, share with 3T BGN mode, we need to change register setting
			Ret = 14;
		break;

	case RATR_INX_WIRELESS_GB:
		Ret = 6;
		break;

	case RATR_INX_WIRELESS_G:
		Ret = 7;
		break;	

	case RATR_INX_WIRELESS_B:
		Ret = 8;
		break;

	case RATR_INX_WIRELESS_MC:
		if (!(HAL_VAR_NETWORK_TYPE & WIRELESS_11A))
			Ret = 6;
		else
			Ret = 7;
		break;
#if CFG_HAL_RTK_AC_SUPPORT
	case RATR_INX_WIRELESS_AC_N:
		if(RfType == MIMO_1T1R)
			Ret = 10;
		else if(RfType == MIMO_2T2R)
			Ret = 9;
		else
			Ret = 13;
		break;
#endif

	default:
		Ret = 0;
		break;
	}	

	return Ret;
}

static u1Byte
Get_RA_BW88XX(
	BOOLEAN 	bCurTxBW80MHz, 
	BOOLEAN		bCurTxBW40MHz
)
{
	u1Byte	BW = HT_CHANNEL_WIDTH_20;
    
	if(bCurTxBW80MHz)
		BW = HT_CHANNEL_WIDTH_80;
	else if(bCurTxBW40MHz)
		BW = HT_CHANNEL_WIDTH_20_40;
	else
		BW = HT_CHANNEL_WIDTH_20;

	return BW;
}

static u1Byte
Get_VHT_ENI88XX(
	u4Byte			IOTAction,
	u1Byte			WirelessMode,
	u4Byte			ratr_bitmap 
	)
{
	u1Byte Ret = 0;

#ifdef CONFIG_WLAN_HAL_8192EE
//	if (1) return 0;
#endif

	if(WirelessMode < WIRELESS_MODE_N_24G)
		Ret =  0;
	else if(WirelessMode == WIRELESS_MODE_N_24G || WirelessMode == WIRELESS_MODE_N_5G)
	{

//if(IOTAction == HT_IOT_VHT_HT_MIX_MODE)
#if 0
		{
			if(ratr_bitmap & BIT20)	// Mix , 2SS
				Ret = 3;
			else 					// Mix, 1SS
				Ret = 2;
		}
#else
	Ret =  0;
#endif		

	}
	else if(WirelessMode == WIRELESS_MODE_AC_5G)
		Ret = 1;						// VHT

	return (Ret << 4);
}

static BOOLEAN 
Get_RA_ShortGI88XX(	
	IN HAL_PADAPTER         Adapter,
	struct stat_info *		pEntry,
	IN	WIRELESS_MODE		WirelessMode,
	IN	u1Byte				ChnlBW
)
{	
	BOOLEAN	bShortGI;

	BOOLEAN	bShortGI20MHz = FALSE,bShortGI40MHz = FALSE, bShortGI80MHz = FALSE;
	
	if(	WirelessMode == WIRELESS_MODE_N_24G || 
		WirelessMode == WIRELESS_MODE_N_5G || 
		WirelessMode == WIRELESS_MODE_AC_5G )
	{
		if (pEntry->ht_cap_buf.ht_cap_info & HAL_cpu_to_le16(_HTCAP_SHORTGI_40M_)
			&& Adapter->pmib->dot11nConfigEntry.dot11nShortGIfor40M) {
			bShortGI40MHz = TRUE;
		} 
		if (pEntry->ht_cap_buf.ht_cap_info & HAL_cpu_to_le16(_HTCAP_SHORTGI_20M_) &&
			Adapter->pmib->dot11nConfigEntry.dot11nShortGIfor20M) {
			bShortGI20MHz = TRUE;
		}
	}
#if CFG_HAL_RTK_AC_SUPPORT
	if(WirelessMode == WIRELESS_MODE_AC_5G)
	{
		{
			if(( HAL_cpu_to_le32(pEntry->vht_cap_buf.vht_cap_info) & BIT(SHORT_GI80M_E))
				&& Adapter->pmib->dot11nConfigEntry.dot11nShortGIfor40M)
				bShortGI80MHz = TRUE;
		}
	}
#endif

	switch(ChnlBW){
		case HT_CHANNEL_WIDTH_20_40:
			bShortGI = bShortGI40MHz;
			break;
#if CFG_HAL_RTK_AC_SUPPORT            
		case HT_CHANNEL_WIDTH_80:
			bShortGI = bShortGI80MHz;
			break;
#endif
		default:case HT_CHANNEL_WIDTH_20:
			bShortGI = bShortGI20MHz;
			break;
	}		
	return bShortGI;
}

static u8Byte
  RateToBitmap_VHT88XX(
	pu1Byte			pVHTRate,
	u1Byte 		rf_mimo_mode
	
)
{

	u1Byte	i,j , tmpRate,rateMask;
	u8Byte	RateBitmap = 0;

    if(rf_mimo_mode == MIMO_1T1R)
		rateMask = 2;
	else if(rf_mimo_mode == MIMO_2T2R)
        rateMask = 4;
	else
    	rateMask = 6;

	for(i = j= 0; i < rateMask; i+=2, j+=10)
	{
        // now support for 1ss and 2ss
		tmpRate = (pVHTRate[0] >> i) & 3;

		switch(tmpRate){
		case 2:
			RateBitmap = RateBitmap | (0x03ff << j);
			break;
		case 1:
			RateBitmap = RateBitmap | (0x01ff << j);
		break;

		case 0:
			RateBitmap = RateBitmap | (0x00ff << j);
		break;

		default:
			break;
		}
	}

	return RateBitmap;
}

#if 0
u4Byte
Get_VHT_HT_Mix_Ratrbitmap(
	u4Byte					IOTAction,
	WIRELESS_MODE			WirelessMode,
	u4Byte					HT_ratr_bitmap,
	u4Byte					VHT_ratr_bitmap
	)
{
	u4Byte	ratr_bitmap = 0;
	if(WirelessMode == WIRELESS_MODE_N_24G || WirelessMode == WIRELESS_MODE_N_5G)
	{
/*	
		if(IOTAction == HT_IOT_VHT_HT_MIX_MODE)
			ratr_bitmap = HT_ratr_bitmap | BIT28 | BIT29;
		else
			ratr_bitmap =  HT_ratr_bitmap;
*/		
	}
	else
		ratr_bitmap =  VHT_ratr_bitmap;

	return ratr_bitmap;
}
#endif

VOID
UpdateHalRAMask88XX(
	IN HAL_PADAPTER         Adapter,
	HAL_PSTAINFO            pEntry,
	u1Byte                  rssi_level
	)
{
	PRTL8192CD_PRIV		priv = Adapter;
	u1Byte		        WirelessMode    = WIRELESS_MODE_A;
	u1Byte		        BW              = HT_CHANNEL_WIDTH_20;
	u1Byte		        MimoPs          = MIMO_PS_NOLIMIT, ratr_index = 8, H2CCommand[7] ={ 0};
	u4Byte		        ratr_bitmap     = 0, IOTAction = 0;
	BOOLEAN		        bShortGI        = FALSE, bCurTxBW80MHz=FALSE, bCurTxBW40MHz=FALSE;
	struct stat_info    *pstat          = pEntry;
	u1Byte              rf_mimo_mode    = get_rf_mimo_mode(Adapter);
#if CFG_HAL_RTK_AC_SUPPORT
	u4Byte 				VHT_TxMap = Adapter->pmib->dot11acConfigEntry.dot11VHT_TxMap;
#endif
#ifdef MCR_WIRELESS_EXTEND
	u1Byte				print_dbg = Adapter->pshare->rf_ft_var.afb_dbg;
#endif
	if(pEntry == NULL)		
	{
		return;
	}
	
	
		if(pEntry->MIMO_ps & _HT_MIMO_PS_STATIC_)
            MimoPs = MIMO_PS_STATIC;
		else if(pEntry->MIMO_ps & _HT_MIMO_PS_DYNAMIC_)
			MimoPs = MIMO_PS_DYNAMIC;

#if	1
		BW = pEntry->tx_bw;

	if( BW > (Adapter->pshare->CurrentChannelBW))
			BW = Adapter->pshare->CurrentChannelBW;
#endif	
		add_RATid(Adapter, pEntry);		
		rssi_level = pstat->rssi_level;
//		rssi_level = 3;
		ratr_bitmap =  0xfffffff;

#if CFG_HAL_RTK_AC_SUPPORT
		if(pstat->vht_cap_len && (HAL_VAR_NETWORK_TYPE & WIRELESS_11AC) && (!should_restrict_Nrate(Adapter, pstat))) {
			WirelessMode = WIRELESS_MODE_AC_5G;
			if(((HAL_le32_to_cpu(pstat->vht_cap_buf.vht_support_mcs[0])>>2)&3)==3)
				rf_mimo_mode = MIMO_1T1R;
		} 
        else 
#endif
        if ((HAL_VAR_NETWORK_TYPE & WIRELESS_11N) && pstat->ht_cap_len && (!should_restrict_Nrate(Adapter, pstat))) {
			if(HAL_VAR_NETWORK_TYPE & WIRELESS_11A)
				WirelessMode = WIRELESS_MODE_N_5G;
			else
				WirelessMode = WIRELESS_MODE_N_24G;
            
			if((pstat->tx_ra_bitmap & 0xff00000) == 0)
				rf_mimo_mode = MIMO_1T1R;
		}
		else if (((HAL_VAR_NETWORK_TYPE & WIRELESS_11G) && isErpSta(pstat))){
				WirelessMode = WIRELESS_MODE_G;		
		}
        else if ((HAL_VAR_NETWORK_TYPE & WIRELESS_11A) &&
				((HAL_OPMODE & WIFI_AP_STATE) || (Adapter->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G))) {
				WirelessMode = WIRELESS_MODE_A;		
		}
		else if(HAL_VAR_NETWORK_TYPE & WIRELESS_11B){
			WirelessMode = WIRELESS_MODE_B;		
		}

		pstat->WirelessMode = WirelessMode;
#ifdef MCR_WIRELESS_EXTEND
		if (print_dbg)
		GDEBUG("WirelessMode=%d\n", WirelessMode);
#endif		
#if CFG_HAL_RTK_AC_SUPPORT
		if(WirelessMode == WIRELESS_MODE_AC_5G) {
			ratr_bitmap &= 0xfff;
			ratr_bitmap |= RateToBitmap_VHT88XX((pu1Byte)&(pstat->vht_cap_buf.vht_support_mcs[0]), rf_mimo_mode) << 12;
//			ratr_bitmap &= 0x3FCFFFFF;
			if(rf_mimo_mode == MIMO_1T1R)
				ratr_bitmap &= 0x003fffff;
			else if(rf_mimo_mode == MIMO_2T2R)
				ratr_bitmap &= 0x3FCFFFFF;			// Test Chip...	2SS MCS7
			else
				ratr_bitmap &= 0x3FF3FCFFFFF;		// Test Chip...	3SS MCS7

			if(BW==HT_CHANNEL_WIDTH_80)
				bCurTxBW80MHz = TRUE;
		}
#endif

		if (Adapter->pshare->is_40m_bw && (BW == HT_CHANNEL_WIDTH_20_40)
#ifdef WIFI_11N_2040_COEXIST
				&& !(((((GET_MIB(Adapter))->dot11OperationEntry.opmode) & WIFI_AP_STATE)) 
				&& COEXIST_ENABLE
				&& (Adapter->bg_ap_timeout 	|| orForce20_Switch20Map(Adapter)
				))
#endif
	){
			bCurTxBW40MHz = TRUE;
	}

	if(((GET_MIB(Adapter))->dot11OperationEntry.opmode) & WIFI_STATION_STATE) {
		if(((GET_MIB(Adapter))->dot11Bss.t_stamp[1] & 0x6) == 0) {
			bCurTxBW40MHz = bCurTxBW80MHz = FALSE;
		}
	}
	BW = Get_RA_BW88XX(bCurTxBW80MHz, bCurTxBW40MHz);
	
#if CFG_HAL_RTK_AC_SUPPORT	
	if(BW == 0)
	{
		//remove MCS9 for BW=20m
		if (rf_mimo_mode == MIMO_1T1R)
			VHT_TxMap &= ~(BIT(9));
		else if (rf_mimo_mode == MIMO_2T2R)
			VHT_TxMap &= ~(BIT(9)|BIT(19));
	}
#endif

	// assign band mask and rate bitmap
	switch (WirelessMode)
	{
		case WIRELESS_MODE_B:
		{
			ratr_index = RATR_INX_WIRELESS_B;
#ifdef MCR_WIRELESS_EXTEND	
			ratr_bitmap &= 0x0000000f;
			if (print_dbg)
				GDEBUG("ratr_bitmap=0x%x\n", ratr_bitmap);
#else
			if(ratr_bitmap & 0x0000000c)		//11M or 5.5M enable				
				ratr_bitmap &= 0x0000000d;
			else
				ratr_bitmap &= 0x0000000f;
#endif			
		}
		break;

		case WIRELESS_MODE_G:
		{
#ifdef MCR_WIRELESS_EXTEND			
			if (pstat->IOTPeer == HT_IOT_PEER_CMW)
			{
				ratr_index = RATR_INX_WIRELESS_G;
				ratr_bitmap &= 0x00000ff0;
			}
			else
#endif				
			{
			ratr_index = RATR_INX_WIRELESS_GB;
			
			if(rssi_level == 1)
				ratr_bitmap &= 0x00000f00;
			else if(rssi_level == 2)
				ratr_bitmap &= 0x00000fff;
			else if(rssi_level == 3)
				ratr_bitmap &= 0x00000fff;
			else
				ratr_bitmap &= 0x0000000f;
			}
		}
		break;
			
		case WIRELESS_MODE_A:
		{
			ratr_index = RATR_INX_WIRELESS_G;
			ratr_bitmap &= 0x00000ff0;
		}
		break;
			
		case WIRELESS_MODE_N_24G:
		case WIRELESS_MODE_N_5G:
		{
			if(WirelessMode == WIRELESS_MODE_N_24G)
			{
#ifdef MCR_WIRELESS_EXTEND			
				if (pstat->IOTPeer == HT_IOT_PEER_CMW) 					
					ratr_index = RATR_INX_WIRELESS_N;
				else
#endif					
				ratr_index = RATR_INX_WIRELESS_NGB;
			}
			else
				ratr_index = RATR_INX_WIRELESS_NG;

//			if(MimoPs <= MIMO_PS_DYNAMIC)
#if 0
			if(MimoPs < MIMO_PS_DYNAMIC)
			{
				if(rssi_level == 1)
					ratr_bitmap &= 0x00070000;
				else if(rssi_level == 2)
					ratr_bitmap &= 0x0007f000;
				else
					ratr_bitmap &= 0x0007f005;
			}
			else
#endif			
#ifdef MCR_WIRELESS_EXTEND
			if (pstat->IOTPeer == HT_IOT_PEER_CMW)
			{
				if (rf_mimo_mode == MIMO_1T1R)
					ratr_bitmap &= 0x000ff000;
				else if (rf_mimo_mode == MIMO_2T2R)
					ratr_bitmap &= 0x0ffff000;
			}
			else
#endif
			{
				if (rf_mimo_mode == MIMO_1T1R)
				{					
					if (bCurTxBW40MHz)
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x000f0000;
						else if(rssi_level == 2)
							ratr_bitmap &= 0x000ff000;
						else if(rssi_level == 3)
							ratr_bitmap &= 0x000ff015;
						else {
							if(WirelessMode==WIRELESS_MODE_N_24G)
								ratr_bitmap &= 0x0000000f;
						else
							ratr_bitmap &= 0x000ff015;
						}
					}
					else
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x000f0000;
						else if(rssi_level == 2)
							ratr_bitmap &= 0x000ff000;
						else if(rssi_level == 3)
							ratr_bitmap &= 0x000ff005;
						else {
							if(WirelessMode==WIRELESS_MODE_N_24G)
								ratr_bitmap &= 0x0000000f;
						else
							ratr_bitmap &= 0x000ff005;
						}
					}	
				}
				else
				{
					if (bCurTxBW40MHz)
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x0fff0000;
						else if(rssi_level == 2) {
							//ratr_bitmap &= 0x0f8ff000;
							ratr_bitmap &= 0x0ffff01f;
						}
						else if(rssi_level == 3){
							ratr_bitmap &= 0x0ffff01f;
						}
						else {
							if(WirelessMode==WIRELESS_MODE_N_24G)
								ratr_bitmap &= 0x0000000f;
							else 
								ratr_bitmap &= 0x0ffff01f;
						}
					}
					else
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x0fff0000;
						else if(rssi_level == 2) {
							//ratr_bitmap &= 0x0f8ff000;
							ratr_bitmap &= 0x0ffff000;
						}
						else if(rssi_level == 3) {
							//ratr_bitmap &= 0x0f8ff005;
							ratr_bitmap &= 0x0ffff005;
						} else {
							if(WirelessMode==WIRELESS_MODE_N_24G)
								ratr_bitmap &= 0x0000000f;
							else
							ratr_bitmap &= 0x0ffff005;
						}
					}
				}
			}
		}
		break;

#if CFG_HAL_RTK_AC_SUPPORT
		case WIRELESS_MODE_AC_5G:
		{
			ratr_index = RATR_INX_WIRELESS_AC_N;

			if (rf_mimo_mode == MIMO_1T1R) {
				#if 1 // enable VHT-MCS 8~9
				ratr_bitmap &= 0x003ff010;
// 8881A: RSSI < 30, disable MCS8,9
				if(pstat->rssi_level == 3)
					ratr_bitmap &= 0x000ff010;
				
				#else
				ratr_bitmap &= 0x000ff010;	//Disable VHT-MCS 8~9
				#endif
			}
			else
				ratr_bitmap &= 0xfffff010;
			ratr_bitmap &= (VHT_TxMap << 12)|0xff0;
		}
		break;
#endif

		default:
			ratr_index = RATR_INX_WIRELESS_NGB;
			
			if(rf_mimo_mode == MIMO_1T1R)
				ratr_bitmap &= 0x000ff0ff;
			else
				ratr_bitmap &= 0x0f8ff0ff;
			break;
	}
	BW = Get_RA_BW88XX(bCurTxBW80MHz, bCurTxBW40MHz);
	bShortGI = Get_RA_ShortGI88XX(Adapter, pEntry, WirelessMode, BW);

	pstat->ratr_idx = MRateIdxToARFRId88XX(Adapter, ratr_index, rf_mimo_mode) ;
	pstat->tx_bw_fw = BW;

#if defined(AC2G_256QAM) || defined(WLAN_HAL_8814AE)
	 if(is_ac2g(Adapter) && pstat->vht_cap_len ) {
		 printk("AC2G STA Associated !!\n");
		 if (rf_mimo_mode == MIMO_1T1R)
		 {
			 //bShortGI = 1;
			 ratr_bitmap = 0x003ff015;

			 if(BW == 2)
				pstat->ratr_idx = 10;
			 else
				pstat->ratr_idx = 11;
 
			 if(BW == 0)
				VHT_TxMap = 0x1ff;
			 else
				VHT_TxMap = 0x3ff;
 
		 }
		 else if (rf_mimo_mode == MIMO_2T2R)
		 {
			// bShortGI = 0;
			 ratr_bitmap = 0xffcff015;

			 if(BW == 2)
				pstat->ratr_idx = 9;
			 else
				pstat->ratr_idx = 12;

			 if(BW == 0)
				VHT_TxMap = 0x7fdff;
			 else
				VHT_TxMap = 0xfffff;
		 }

		 ratr_bitmap &= ((VHT_TxMap << 12)|0xfff);
		 
		 pstat->WirelessMode = WIRELESS_MODE_AC_24G;
	 }
#endif
#ifdef MCR_WIRELESS_EXTEND
	if (print_dbg)
		GDEBUG(" WirelessMode[%d] rssi[%d] ratr_bitmap= 0x%x   ==> ", WirelessMode, pstat->rssi, ratr_bitmap);
	if (pstat->IOTPeer == HT_IOT_PEER_CMW && (Adapter->pshare->rf_ft_var.afb_ft)) {
		if (WirelessMode == WIRELESS_MODE_B) {
			if (pstat->rssi >= 27)
				ratr_bitmap &= 0x8;
			else if ((pstat->rssi >= 23) && (pstat->rssi <= 26))
				ratr_bitmap &= 0x7;
			else if ((pstat->rssi >= 17) && (pstat->rssi <= 22))
				ratr_bitmap &= 0x2;
			else if (pstat->rssi <= 16)
				ratr_bitmap &= 0x1;
		} 
		else if (WirelessMode == WIRELESS_MODE_G) {
			if (pstat->rssi > 38)
				ratr_bitmap &= 0x800;
			else if ((pstat->rssi >= 34) && (pstat->rssi <= 38))	
				ratr_bitmap &= 0x400;
			else if ((pstat->rssi >= 29) && (pstat->rssi <= 33))
				ratr_bitmap &= 0x200;
			else if ((pstat->rssi >= 26) && (pstat->rssi <= 28))
				ratr_bitmap &= 0x1f0;
			else if ((pstat->rssi >= 23) && (pstat->rssi <= 25))
				ratr_bitmap &= 0x0f0;
			else if ((pstat->rssi >= 19) && (pstat->rssi <= 22))
				ratr_bitmap &= 0x070;
			else if ((pstat->rssi >= 15) && (pstat->rssi <= 18))
				ratr_bitmap &= 0x030;
			else if ((pstat->rssi <= 14))
				ratr_bitmap &= 0x010;
		}
		else if (WirelessMode == WIRELESS_MODE_N_24G) {	
			if (pstat->rssi > 38)
				ratr_bitmap &= 0xffff000;
			else if ((pstat->rssi >= 34) && (pstat->rssi <= 38))	
				ratr_bitmap &= 0x7f7f000;
			else if ((pstat->rssi >= 29) && (pstat->rssi <= 33))
				ratr_bitmap &= 0x3f3f000;
			else if ((pstat->rssi >= 26) && (pstat->rssi <= 28))	
				ratr_bitmap &= 0x1f1f000;
			else if ((pstat->rssi >= 23) && (pstat->rssi <= 25))
				ratr_bitmap &= 0x0f0f000;
			else if ((pstat->rssi >= 19) && (pstat->rssi <= 22))
				ratr_bitmap &= 0x0707000;
			else if ((pstat->rssi >= 15) && (pstat->rssi <= 18))
				ratr_bitmap &= 0x0303000;
			else if ((pstat->rssi <= 14))
				ratr_bitmap &= 0x0101000;
		}
		if (Adapter->pshare->rf_ft_var.afb_force)
			ratr_bitmap = Adapter->pshare->rf_ft_var.afb_force;
		if (print_dbg)
			printk("0x%x\n", ratr_bitmap);	
	}
#endif
	
	H2CCommand[0] = (pstat->aid); //macid
	H2CCommand[1] = (pstat->ratr_idx)| (bShortGI?0x80:0x00) ; //rate_id
	H2CCommand[2] = BW |Get_VHT_ENI88XX(IOTAction, WirelessMode, ratr_bitmap);	

	H2CCommand[2] |= BIT6;			// DisableTXPowerTraining

	H2CCommand[3] = (u1Byte)(ratr_bitmap & 0x000000ff);
	H2CCommand[4] = (u1Byte)((ratr_bitmap & 0x0000ff00) >>8);
	H2CCommand[5] = (u1Byte)((ratr_bitmap & 0x00ff0000) >> 16);
	H2CCommand[6] = (u1Byte)((ratr_bitmap & 0xff000000) >> 24);
	
	FillH2CCmd88XX(Adapter, H2C_88XX_RA_MASK, 7, H2CCommand);

	SetBcnCtrlReg88XX(Adapter, BIT3, 0);
#if 0	
	panic_printk("UpdateHalRAMask88XX(): bitmap = %x ratr_index = %1x, MacID:%x, ShortGI:%x, MimoPs=%d\n", 
		ratr_bitmap, pstat->ratr_idx,  (pstat->aid), bShortGI, MimoPs);
	
	panic_printk("Cmd: %02x, %02x, %02x, %02x, %02x, %02x, %02x  \n",
		H2CCommand[0] ,H2CCommand[1], H2CCommand[2],
		H2CCommand[3] ,H2CCommand[4], H2CCommand[5], H2CCommand[6]		);
#endif	
	
}

VOID
UpdateHalRAMask8814A(
	IN HAL_PADAPTER         Adapter,
	HAL_PSTAINFO            pEntry,
	u1Byte                  rssi_level
	)
{

	u1Byte		        WirelessMode    = WIRELESS_MODE_A;
	u1Byte		        BW              = HT_CHANNEL_WIDTH_20;
	u1Byte		        MimoPs          = MIMO_PS_NOLIMIT, ratr_index = 8, H2CCommand[7] ={ 0};
	u8Byte		        ratr_bitmap     = 0, IOTAction = 0;
	BOOLEAN		        bShortGI        = FALSE, bCurTxBW80MHz=FALSE, bCurTxBW40MHz=FALSE;
	struct stat_info    *pstat          = pEntry;
	u1Byte              rf_mimo_mode    = get_rf_mimo_mode(Adapter);
	u1Byte		sta_mimo_mode;
#ifdef MCR_WIRELESS_EXTEND	
	u1Byte				print_dbg = Adapter->pshare->rf_ft_var.afb_dbg;
#endif
	//printk("%s %d ratr_bitmap = %llx \n",__func__,__LINE__,ratr_bitmap);            
	//printk("pstat=%x pstat->AID= %x \n",pstat,pstat->aid);

	if(pEntry == NULL)		
	{
		return;
	}
	{
		if(pEntry->MIMO_ps & _HT_MIMO_PS_STATIC_)
            MimoPs = MIMO_PS_STATIC;
		else if(pEntry->MIMO_ps & _HT_MIMO_PS_DYNAMIC_)
			MimoPs = MIMO_PS_DYNAMIC;

#if	1
		BW = pEntry->tx_bw;
		if( BW > Adapter->pshare->CurrentChannelBW)
			BW = Adapter->pshare->CurrentChannelBW;
#endif	
		add_RATid(Adapter, pEntry);		
		rssi_level = pstat->rssi_level;
//		rssi_level = 3;
		ratr_bitmap =  0xffffffffffffffff;

#if CFG_HAL_RTK_AC_SUPPORT
		if(pstat->vht_cap_len && (HAL_VAR_NETWORK_TYPE & WIRELESS_11AC)) {
			WirelessMode = WIRELESS_MODE_AC_5G;
			if(((HAL_le32_to_cpu(pstat->vht_cap_buf.vht_support_mcs[0])>>8)&3)==3) // no support RX 5ss
				sta_mimo_mode = MIMO_4T4R;
			if(((HAL_le32_to_cpu(pstat->vht_cap_buf.vht_support_mcs[0])>>6)&3)==3) // no support RX 4ss
				sta_mimo_mode = MIMO_3T3R;
			if(((HAL_le32_to_cpu(pstat->vht_cap_buf.vht_support_mcs[0])>>4)&3)==3) // no support RX 3ss
				sta_mimo_mode = MIMO_2T2R;
			if(((HAL_le32_to_cpu(pstat->vht_cap_buf.vht_support_mcs[0])>>2)&3)==3) // no support RX 2ss
				sta_mimo_mode = MIMO_1T1R;

			if (get_rf_NTx(sta_mimo_mode) < get_rf_NTx(rf_mimo_mode))
				rf_mimo_mode = sta_mimo_mode;
		
		} 
        else 
#endif
        if ((HAL_VAR_NETWORK_TYPE & WIRELESS_11N) && pstat->ht_cap_len && (!should_restrict_Nrate(Adapter, pstat))) {
			if(HAL_VAR_NETWORK_TYPE & WIRELESS_11A)
				WirelessMode = WIRELESS_MODE_N_5G;
			else
				WirelessMode = WIRELESS_MODE_N_24G;
			
			if (pstat->ht_cap_buf.support_mcs[4] == 0) // no support RX 5ss
				sta_mimo_mode = MIMO_4T4R;
			if (pstat->ht_cap_buf.support_mcs[3] == 0) // no support RX 4ss
				sta_mimo_mode = MIMO_3T3R;
			if (pstat->ht_cap_buf.support_mcs[2] == 0) // no support RX 3ss
				sta_mimo_mode = MIMO_2T2R;
			if (pstat->ht_cap_buf.support_mcs[1] == 0) // no support RX 2ss
				sta_mimo_mode = MIMO_1T1R;

			if (get_rf_NTx(sta_mimo_mode) < get_rf_NTx(rf_mimo_mode))
				rf_mimo_mode = sta_mimo_mode;

			//if((pstat->tx_ra_bitmap & 0xff00000) == 0)
			//	rf_mimo_mode = MIMO_1T1R;
		}
		else if (((HAL_VAR_NETWORK_TYPE & WIRELESS_11G) && isErpSta(pstat))){
				WirelessMode = WIRELESS_MODE_G;		
		}
        else if ((HAL_VAR_NETWORK_TYPE & WIRELESS_11A) &&
				((HAL_OPMODE & WIFI_AP_STATE) || (Adapter->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G))) {
				WirelessMode = WIRELESS_MODE_A;		
		}
		else if(HAL_VAR_NETWORK_TYPE & WIRELESS_11B){
			WirelessMode = WIRELESS_MODE_B;		
		}

		pstat->WirelessMode = WirelessMode;

#if CFG_HAL_RTK_AC_SUPPORT
		if(WirelessMode == WIRELESS_MODE_AC_5G) {
			ratr_bitmap &= 0xfff;
			ratr_bitmap |= RateToBitmap_VHT88XX((pu1Byte)&(pstat->vht_cap_buf.vht_support_mcs[0]), rf_mimo_mode) << 12;

			if(rf_mimo_mode == MIMO_1T1R)
				ratr_bitmap &= 0x003fffff;
            else if(rf_mimo_mode == MIMO_3T3R)
                ratr_bitmap &= 0xFFF3FCFFFFF;			// 3SS MCS9
			else
				ratr_bitmap &= 0xFFCFFFFF;			// 2SS MCS9
				
			if(BW==HT_CHANNEL_WIDTH_80)
				bCurTxBW80MHz = TRUE;

		}
#endif

		if (Adapter->pshare->is_40m_bw && (BW == HT_CHANNEL_WIDTH_20_40)
#ifdef WIFI_11N_2040_COEXIST
				&& !(((((GET_MIB(Adapter))->dot11OperationEntry.opmode) & WIFI_AP_STATE)||
				(((GET_MIB(Adapter))->dot11OperationEntry.opmode) & WIFI_STATION_STATE)) 
				&& Adapter->pmib->dot11nConfigEntry.dot11nCoexist 
				&& (Adapter->bg_ap_timeout || orForce20_Switch20Map(Adapter))
			)
#endif
		)
			bCurTxBW40MHz = TRUE;
	}

	// assign band mask and rate bitmap
	switch (WirelessMode)
	{
		case WIRELESS_MODE_B:
		{
			ratr_index = RATR_INX_WIRELESS_B;
			if(ratr_bitmap & 0x000000000000000c)		//11M or 5.5M enable				
				ratr_bitmap &= 0x000000000000000d;
			else
				ratr_bitmap &= 0x000000000000000f;
		}
		break;

		case WIRELESS_MODE_G:
		{
#ifdef MCR_WIRELESS_EXTEND			
			if (pstat->IOTPeer == HT_IOT_PEER_CMW)
			{
				ratr_index = RATR_INX_WIRELESS_G;
				ratr_bitmap &= 0x00000ff0;
			}
			else
#endif		
			{
			ratr_index = RATR_INX_WIRELESS_GB;
			
			if(rssi_level == 1)
				ratr_bitmap &= 0x0000000000000f00;
			else if(rssi_level == 2)
				ratr_bitmap &= 0x0000000000000ff0;
			else
				ratr_bitmap &= 0x0000000000000ff5;
			}
		}
		break;
			
		case WIRELESS_MODE_A:
		{
			ratr_index = RATR_INX_WIRELESS_G;
			ratr_bitmap &= 0x00000ff0;
		}
		break;
			
		case WIRELESS_MODE_N_24G:
		case WIRELESS_MODE_N_5G:
		{
			if(WirelessMode == WIRELESS_MODE_N_24G)
			{
#if 0//def MCR_WIRELESS_EXTEND			
				if (pstat->IOTPeer == HT_IOT_PEER_CMW) 					
					ratr_index = RATR_INX_WIRELESS_N;
				else
#endif				
				ratr_index = RATR_INX_WIRELESS_NGB;
			}
			else
				ratr_index = RATR_INX_WIRELESS_NG;
#ifdef MCR_WIRELESS_EXTEND			
			if (pstat->IOTPeer == HT_IOT_PEER_CMW)					
				ratr_index = RATR_INX_WIRELESS_N;		
#endif	

//			if(MimoPs <= MIMO_PS_DYNAMIC)
#if 0
			if(MimoPs < MIMO_PS_DYNAMIC)
			{
				if(rssi_level == 1)
					ratr_bitmap &= 0x00070000;
				else if(rssi_level == 2)
					ratr_bitmap &= 0x0007f000;
				else
					ratr_bitmap &= 0x0007f005;
			}
			else
#endif			
#ifdef MCR_WIRELESS_EXTEND
			if (pstat->IOTPeer == HT_IOT_PEER_CMW)
			{
				if (rf_mimo_mode == MIMO_1T1R)
					ratr_bitmap &= 0x000ff000;
				else if (rf_mimo_mode == MIMO_2T2R)
					ratr_bitmap &= 0x0ffff000;
			}
			else
#endif			
			{
				if (rf_mimo_mode == MIMO_1T1R)
				{					
					if (bCurTxBW40MHz)
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x00000000000f0000;
						else if(rssi_level == 2)
							ratr_bitmap &= 0x00000000000ff000;
						else
							ratr_bitmap &= 0x00000000000ff015;
					}
					else
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x00000000000f0000;
						else if(rssi_level == 2)
							ratr_bitmap &= 0x00000000000ff000;
						else
							ratr_bitmap &= 0x00000000000ff005;
					}	
				}
				else if (rf_mimo_mode == MIMO_2T2R)
				{
					if (bCurTxBW40MHz)
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x000000000f8f0000;
						else if(rssi_level == 2) {
							//ratr_bitmap &= 0x0f8ff000;
							ratr_bitmap &= 0x000000000ffff000;
						}
						else {
							//ratr_bitmap &= 0x0f8ff015;
							ratr_bitmap &= 0x000000000ffff015;
						}
					}
					else
					{
						if(rssi_level == 1)
							ratr_bitmap &= 0x000000000f8f0000;
						else if(rssi_level == 2) {
							//ratr_bitmap &= 0x0f8ff000;
							ratr_bitmap &= 0x000000000ffff000;
						}
						else {
							//ratr_bitmap &= 0x0f8ff005;
							ratr_bitmap &= 0x000000000ffff005;
						}
					}
				}
				else
				{
					if(rssi_level == 1){
						ratr_bitmap &= 0x0000000f8f8f0000;
					}
					else if(rssi_level == 2){
						ratr_bitmap &= 0x0000000ffffff000;
					}
					else{
						ratr_bitmap &= 0x0000000ffffff015;
					}
				}
			}
		}
		break;

#if CFG_HAL_RTK_AC_SUPPORT
		case WIRELESS_MODE_AC_24G:
		{
			// need to check !!!
			ratr_index = RATR_INX_WIRELESS_AC_24N;
			if(rssi_level == 1)
				ratr_bitmap &= 0x00000000fc3f0000;
			else if(rssi_level == 2)
				ratr_bitmap &= 0x00000000fffff000;
			else
				ratr_bitmap &= 0x00000000ffffffff;
		}
		break;
		
		case WIRELESS_MODE_AC_5G:
		{
			ratr_index = RATR_INX_WIRELESS_AC_N;

			if (rf_mimo_mode == MIMO_1T1R) {
				ratr_bitmap &= 0x00000000003ff010;
				if(rssi_level == 3)
					ratr_bitmap &= 0x00000000000ff010;
			}
			else if (rf_mimo_mode == MIMO_2T2R) 
				ratr_bitmap &= 0x00000000fffff010;
			else
				ratr_bitmap &= 0x000003fffffff010; // VHT 3SS MCS0~9

			//marked to prevent resize ratr_bitmap from 8-byte to 4-byte
			//ratr_bitmap &= (Adapter->pmib->dot11acConfigEntry.dot11VHT_TxMap << 12)|0xff0;
		}
		break;
#endif

		default:
			ratr_index = RATR_INX_WIRELESS_NGB;
			
			if(rf_mimo_mode == MIMO_1T1R)
				ratr_bitmap &= 0x00000000000ff0ff;
			else
				ratr_bitmap &= 0x000000000f8ff0ff;
			break;
	}

#ifdef MCR_WIRELESS_EXTEND
	if (pstat->IOTPeer == HT_IOT_PEER_CMW && (Adapter->pshare->rf_ft_var.afb_ft)) 
	{
		if (print_dbg)
			GDEBUG(" WirelessMode[%d] rssi[%d] ratr_bitmap= 0x%llx   ==> ", WirelessMode, pstat->rssi, ratr_bitmap);
#if 1
		if (pstat->rssi >= 38)
			ratr_bitmap &= 0x0fffffff;
		else if ((pstat->rssi >= 34) && (pstat->rssi <= 37))
			ratr_bitmap &= 0x07f7f7ff;
		else if ((pstat->rssi >= 30) && (pstat->rssi <= 33))
			ratr_bitmap &= 0x03f3f3ff;
		else if ((pstat->rssi >= 27) && (pstat->rssi <= 29))
			ratr_bitmap &= 0x01f1f1f7;
		else if ((pstat->rssi >= 23) && (pstat->rssi <= 26))
			ratr_bitmap &= 0x00f0f0f7;
		else if ((pstat->rssi >= 19) && (pstat->rssi <= 22))
			ratr_bitmap &= 0x00707073;
		else if ((pstat->rssi >= 15) && (pstat->rssi <= 18))
			ratr_bitmap &= 0x00303033;
		else if ((pstat->rssi <= 14))
			ratr_bitmap &= 0x00101011;
#else			
		if (pstat->rssi >= 38)
			ratr_bitmap &= 0x0fffffff;
		else if ((pstat->rssi >= 35) && (pstat->rssi <= 37))
			ratr_bitmap &= 0x07f7f7ff;
		else if ((pstat->rssi >= 30) && (pstat->rssi <= 34))
			ratr_bitmap &= 0x03f3f3ff;
		else if ((pstat->rssi >= 27) && (pstat->rssi <= 29))
			ratr_bitmap &= 0x01f1f1f7;
		else if ((pstat->rssi >= 23) && (pstat->rssi <= 26))
			ratr_bitmap &= 0x00f0f0f7;
		else if ((pstat->rssi >= 20) && (pstat->rssi <= 22))
			ratr_bitmap &= 0x00707073;
		else if ((pstat->rssi >= 16) && (pstat->rssi <= 19))
			ratr_bitmap &= 0x00303033;
		else if ((pstat->rssi <= 15))
			ratr_bitmap &= 0x00101011;
#endif
		if (Adapter->pshare->rf_ft_var.afb_force) {
			if (WirelessMode == WIRELESS_MODE_A)
				ratr_bitmap = (Adapter->pshare->rf_ft_var.afb_force) << 4;
			else if (WirelessMode == WIRELESS_MODE_N_5G)
				ratr_bitmap = (Adapter->pshare->rf_ft_var.afb_force << 20) | (Adapter->pshare->rf_ft_var.afb_force << 12) ;
		}
		if (print_dbg)
			printk("0x%llx\n", ratr_bitmap);	
	}
#endif
	BW = Get_RA_BW88XX(bCurTxBW80MHz, bCurTxBW40MHz);
	bShortGI = Get_RA_ShortGI88XX(Adapter, pEntry, WirelessMode, BW);

	pstat->ratr_idx = MRateIdxToARFRId88XX(Adapter, ratr_index, rf_mimo_mode);
	pstat->tx_bw_fw = BW;
	
	H2CCommand[0] = (pstat->aid);
	H2CCommand[1] =  (pstat->ratr_idx)| (bShortGI?0x80:0x00) ;	
	H2CCommand[2] = BW |Get_VHT_ENI88XX(IOTAction, WirelessMode, ratr_bitmap);
	

	H2CCommand[2] |= BIT6;			// DisableTXPowerTraining

	H2CCommand[3] = (u1Byte)(ratr_bitmap & 0x000000ff);
	H2CCommand[4] = (u1Byte)((ratr_bitmap & 0x0000ff00) >>8);
	H2CCommand[5] = (u1Byte)((ratr_bitmap & 0x00ff0000) >> 16);
	H2CCommand[6] = (u1Byte)((ratr_bitmap & 0xff000000) >> 24);
	
	FillH2CCmd88XX(Adapter, H2C_88XX_RA_MASK, 7, H2CCommand);
#if 0
    printk("H2C H2C_88XX_RA_MASK 0x40\n");
    printk("%s %d ratr_bitmap = %llx \n",__func__,__LINE__,ratr_bitmap);
    printk("pstat->ratr_idx = %x \n",pstat->ratr_idx);
    printk("pstat->aid = %x \n",pstat->aid);
    printk("bShortGI = %x \n",bShortGI);
    printk("MimoPs = %x \n",MimoPs);    
	printk("UpdateHalRAMask88XX(): bitmap = %x ratr_index = %x, pstat->aid = %x, ShortGI:%x, MimoPs=%d\n", 
			ratr_bitmap, pstat->ratr_idx,  (pstat->aid), bShortGI, MimoPs);

	panic_printk("Cmd: %02x, %02x, %02x, %02x, %02x, %02x, %02x  \n",
					H2CCommand[0] ,H2CCommand[1], H2CCommand[2],
					H2CCommand[3] ,H2CCommand[4], H2CCommand[5], H2CCommand[6]      );
#endif
	H2CCommand[0] = (pstat->aid);
	H2CCommand[3] = (u1Byte)((ratr_bitmap>>32) & 0x000000ff);
	H2CCommand[4] = (u1Byte)(((ratr_bitmap>>32) & 0x0000ff00) >>8);
	#if (IS_RTL8814A_SERIES)
	FillH2CCmd88XX(Adapter, H2C_88XX_RA_MASK_3SS, 5, H2CCommand);
	#endif

	//SetBcnCtrlReg88XX(Adapter, BIT3, 0);
#if 0
	printk("H2C H2C_88XX_RA_MASK_3SS 0x46\n");
	printk("%s %d ratr_bitmap = %llx \n",__func__,__LINE__,ratr_bitmap);
	printk("pstat->ratr_idx = %x \n",pstat->ratr_idx);
	printk("pstat->aid = %x \n",pstat->aid);
	printk("bShortGI = %x \n",bShortGI);
	printk("MimoPs = %x \n",MimoPs);

	//printk("UpdateHalRAMask88XX(): bitmap = %x ratr_index = %x, pstat->aid:%x, ShortGI:%x, MimoPs=%d\n", 
	//	ratr_bitmap, pstat->ratr_idx,  (pstat->aid), bShortGI, MimoPs);
	
	panic_printk("Cmd: %02x, %02x, %02x, %02x, %02x, %02x, %02x  \n",
		H2CCommand[0] ,H2CCommand[1], H2CCommand[2],
		H2CCommand[3] ,H2CCommand[4], H2CCommand[5], H2CCommand[6]		);
#endif	
}

void
UpdateHalMSRRPT88XX(
	IN HAL_PADAPTER     Adapter,
	HAL_PSTAINFO        pEntry,
	u1Byte              opmode
	)
{
	u1Byte		H2CCommand[3] ={0};
    update_remapAid(Adapter,pEntry);
	H2CCommand[0] = opmode & 0x01;
	H2CCommand[1] = REMAP_AID(pEntry) & 0xff;
	H2CCommand[2] = 0;
	FillH2CCmd88XX(Adapter, H2C_88XX_MSRRPT, 3, H2CCommand);
	
//	panic_printk("UpdateHalMSRRPT88XX Cmd: %02x, %02x, %02x  \n",
//		H2CCommand[0] ,H2CCommand[1], H2CCommand[2]);
}


static VOID 
SetBCNRsvdPage88XX
(    
	IN HAL_PADAPTER     Adapter,
    u1Byte              numOfAP,
	pu1Byte             loc_bcn
)
{
    u1Byte      H2CCommand[8] ={0};
    u1Byte      i;

    for(i=0;i<numOfAP;i++)
    {
        H2CCommand[i] = *(loc_bcn+i);
    }   

   	FillH2CCmd88XX(Adapter, H2C_88XX_BCN_RSVDPAGE, 7, H2CCommand);
}

static VOID 
SetProbeRsvdPage88XX
(    
	IN HAL_PADAPTER     Adapter,
    u1Byte              numOfAP,
	pu1Byte             loc_probe
)
{
    u1Byte      H2CCommand[8] ={0};
    u1Byte      i;

    for(i=0;i<numOfAP;i++)
    {
        H2CCommand[i] = *(loc_probe+i);
    }   

   	FillH2CCmd88XX(Adapter, H2C_88XX_PROBE_RSVDPAGE, 7, H2CCommand);
}


static VOID 
SetAPOffloadEnable88XX
(    
	IN HAL_PADAPTER     Adapter,
    u1Byte              bEn, 
    u1Byte              numOfAP,
	u1Byte              bHidden,	
	u1Byte              bDenyAny
)
{

    
    u1Byte      H2CCommand[8] ={0};
    u1Byte      i;

    H2CCommand[0] = bEn;

    if(bHidden)
    {
        for(i=0;i<numOfAP;i++)
        {
            H2CCommand[1] |= BIT(i);           
        }   
    }

    if(bDenyAny)
    {
        for(i=0;i<numOfAP;i++)
        {
            H2CCommand[2] |= BIT(i);            
        }   
    }
    
	FillH2CCmd88XX(Adapter, H2C_88XX_AP_OFFLOAD, 3, H2CCommand);
}


void
SetAPOffload88XX(
	IN HAL_PADAPTER     Adapter,
	u1Byte              bEn,
	u1Byte              numOfAP,
	u1Byte              bHidden,	
	u1Byte              bDenyAny,
	pu1Byte             loc_bcn,
	pu1Byte             loc_probe
	)
{
    if(bEn)
    {
        SetBCNRsvdPage88XX(Adapter,numOfAP,loc_bcn);
        SetProbeRsvdPage88XX(Adapter,numOfAP,loc_probe);      
        SetAPOffloadEnable88XX(Adapter,bEn,numOfAP,bHidden,bDenyAny);
    }
    else
    {
        SetAPOffloadEnable88XX(Adapter,bEn,numOfAP,bHidden,bDenyAny);
    }
}









// This function is call before download Rsvd page
// Function input 1. current len
// return 1. the len after add dmmuy byte, 2. the page location

u4Byte
GetRsvdPageLoc88XX
( 
	IN  IN HAL_PADAPTER     Adapter,
    IN  u4Byte              frlen,
    OUT pu1Byte             loc_page
)
{
#if (IS_RTL8192E_SERIES || IS_RTL8881A_SERIES)
	if ( IS_HARDWARE_TYPE_8192E(Adapter) || IS_HARDWARE_TYPE_8881A(Adapter)) {
    if(frlen%PBP_PSTX_SIZE)
	{
		frlen = (frlen+PBP_PSTX_SIZE-(frlen%PBP_PSTX_SIZE)) ;
	}

    *(loc_page) = (u1Byte)(frlen /PBP_PSTX_SIZE);
	}
#endif // (IS_RTL8192E_SERIES || IS_RTL8881A_SERIES)

#if (IS_RTL8814A_SERIES)
	if ( IS_HARDWARE_TYPE_8814A(Adapter) ) {
	    if(frlen%PBP_PSTX_SIZE_V1)
	    {
	        frlen = (frlen+PBP_PSTX_SIZE_V1-(frlen%PBP_PSTX_SIZE_V1)) ;
	    }

	    *(loc_page) = (u1Byte)(frlen /PBP_PSTX_SIZE_V1);
	}
#endif // IS_RTL8814A_SERIES
    return frlen;
}

VOID
SetRsvdPage88XX
( 
	IN  IN HAL_PADAPTER     Adapter,
    IN  pu1Byte             prsp,
    IN  pu4Byte             beaconbuf,    
    IN  u4Byte              pktLen,  
    IN  u4Byte              bigPktLen  
)
{
   // TX_DESC_88XX tx_desc;

#if IS_EXIST_PCI || IS_EXIST_EMBEDDED
    FillBeaconDesc88XX(Adapter, prsp-SIZE_TXDESC_88XX, (void*)prsp, pktLen, 1);

    SigninBeaconTXBD88XX(Adapter, beaconbuf, bigPktLen);
#endif	
}


#endif //(IS_RTL8881A_SERIES || IS_RTL8192E_SERIES)

#if IS_RTL88XX_GENERATION

//
//3 H2C Command
//
#if 0
BOOLEAN
IsH2CBufOccupy88XX(
    IN  HAL_PADAPTER    Adapter
)
{
    PHAL_DATA_TYPE              pHalData = _GET_HAL_DATA(Adapter);
   
    if ( HAL_RTL_R8(REG_HMETFR) & BIT(pHalData->H2CBufPtr88XX) ) {
        return _TRUE;
    }
    else {
        RT_TRACE(COMP_DBG, DBG_WARNING, ("H2CBufOccupy(%d) !!\n", 
                                            pHalData->H2CBufPtr88XX) );
        return _FALSE;
    }
}


BOOLEAN
SigninH2C88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  PH2C_CONTENT    pH2CContent
)
{
    PHAL_DATA_TYPE      pHalData = _GET_HAL_DATA(Adapter);
    u4Byte              DelayCnt = H2CBUF_OCCUPY_DELAY_CNT;
    
    //Check if h2c cmd signin buffer is occupied
    while( _TRUE == IsH2CBufOccupy88XX(Adapter) ) {
       HAL_delay_us(H2CBUF_OCCUPY_DELAY_US);
       DelayCnt--;

       if ( 0 == DelayCnt ) {
            RT_TRACE(COMP_DBG, DBG_WARNING, ("H2CBufOccupy retry timeout\n") );
            return _FALSE;
       }
       else {
            //Continue to check H2C Buf
       }
       
    }

    //signin reg in order to fit hw requirement
    if ( pH2CContent->content & BIT7 ) {
        HAL_RTL_W16(REG_HMEBOX_E0_E1 + (pHalData->H2CBufPtr88XX*2), pH2CContent->ext_content);
    }

    HAL_RTL_W16(REG_HMEBOX0 + (pHalData->H2CBufPtr88XX*4), pH2CContent->content);

	//printk("(smcc) sign in h2c %x\n", HMEBOX_0+(priv->pshare->fw_q_fifo_count*4));
    RT_TRACE(COMP_DBG, DBG_LOUD, ("sign in h2c(%d) 0x%x\n", 
                                        pHalData->H2CBufPtr88XX, 
                                        REG_HMEBOX0 + (pHalData->H2CBufPtr88XX*4)) );

    //rollover ring buffer count
    if (++pHalData->H2CBufPtr88XX > 3) {
        pHalData->H2CBufPtr88XX = 0;
    }

    return _TRUE;
}

#else
BOOLEAN
CheckFwReadLastH2C88XX(
	IN  HAL_PADAPTER    Adapter,
	IN  u1Byte          BoxNum
)
{
	u1Byte      valHMETFR;
	BOOLEAN     Result = FALSE;
	
	valHMETFR = HAL_RTL_R8(REG_HMETFR);

	if(((valHMETFR>>BoxNum)&BIT0) == 0)
		Result = TRUE;
		
	return Result;
}


RT_STATUS
FillH2CCmd88XX(
	IN  HAL_PADAPTER    Adapter,
	IN	u1Byte 		    ElementID,
	IN	u4Byte 		    CmdLen,
	IN	pu1Byte		    pCmdBuffer
)
{
    PHAL_DATA_TYPE      pHalData = _GET_HAL_DATA(Adapter);
	u1Byte	            BoxNum;
	u2Byte	            BOXReg=0, BOXExtReg=0;
	u2Byte	            BOXRegLast=0, BOXExtRegLast=0;
	BOOLEAN             bFwReadClear=FALSE;
	u1Byte	            BufIndex=0;
	u2Byte	            WaitH2cLimmit=0;
	u1Byte	            BoxContent[4], BoxExtContent[4];
	u1Byte	            idx=0;
#ifdef BT_COEXIST	
	int 				i=0;
#endif

#ifdef BT_COEXIST	
#ifdef BT_COEXIST_DEBUG
			if(ElementID >= 0x60 && ElementID <= 0x66){
				printk("[%s] ID=%x, data:",__FUNCTION__,ElementID);
				for(i=0;i<CmdLen;i++)
					printk("%x ", *(pCmdBuffer+i)); 	
				printk("\n");
			}
#endif		
#endif	
    if(!pHalData->bFWReady) {
        RT_TRACE(COMP_DBG, DBG_WARNING, ("H2C bFWReady=False !!\n") );
        return RT_STATUS_FAILURE;
    }

	// 1. Find the last BOX number which has been writen.
	BoxNum = pHalData->H2CBufPtr88XX;	//pHalData->LastHMEBoxNum;
	switch(BoxNum)
	{
		case 0:
			BOXReg = REG_HMEBOX0;
			BOXExtReg = REG_HMEBOX_E0;
			BOXRegLast = REG_HMEBOX3;
			BOXExtRegLast = REG_HMEBOX_E3;
			break;
		case 1:
			BOXReg = REG_HMEBOX1;
			BOXExtReg = REG_HMEBOX_E1;
			BOXRegLast = REG_HMEBOX0;
			BOXExtRegLast = REG_HMEBOX_E0;
			break;
		case 2:
			BOXReg = REG_HMEBOX2;
			BOXExtReg = REG_HMEBOX_E2;
			BOXRegLast = REG_HMEBOX1;
			BOXExtRegLast = REG_HMEBOX_E1;
			break;
		case 3:
			BOXReg = REG_HMEBOX3;
			BOXExtReg = REG_HMEBOX_E3;
			BOXRegLast = REG_HMEBOX2;
			BOXExtRegLast = REG_HMEBOX_E2;
			break;
		default:
			break;
	}

	// 2. Check if the box content is empty.
	while(!bFwReadClear)
	{
		bFwReadClear = CheckFwReadLastH2C88XX(Adapter, BoxNum);
		if(WaitH2cLimmit == 600) { //do the first stage, clear last cmd
			HAL_RTL_W32(BOXRegLast, 0x00020101);
			//printk("H2C cmd-TO, first stage!! REG_HMETFR:0x%x, BoxNum:%d\n", HAL_RTL_R8(REG_HMETFR), BoxNum);
		} else if(WaitH2cLimmit == 1000) { //do the second stage, clear all cmd
			HAL_RTL_W32(REG_HMEBOX0, 0x00020101);
			HAL_RTL_W32(REG_HMEBOX1, 0x00020101);
			HAL_RTL_W32(REG_HMEBOX2, 0x00020101);
			HAL_RTL_W32(REG_HMEBOX3, 0x00020101);
			//printk("H2C cmd-TO, second stage!! REG_HMETFR:0x%x, BoxNum:%d\n", HAL_RTL_R8(REG_HMETFR), BoxNum);
		} else if(WaitH2cLimmit >= 1200) {
			//printk("H2C cmd-TO, final stage!! REG_HMETFR:0x%x, BoxNum:%d\n", HAL_RTL_R8(REG_HMETFR), BoxNum);
			Adapter->pshare->h2c_box_full++;
			return RT_STATUS_FAILURE;
		}
		else if(!bFwReadClear)
		{	
			HAL_delay_us(10); //us
		}
		WaitH2cLimmit++;
	}

	// 4. Fill the H2C cmd into box 	
	HAL_memset(BoxContent, 0, sizeof(BoxContent));
	HAL_memset(BoxExtContent, 0, sizeof(BoxExtContent));
	
	BoxContent[0] = ElementID; // Fill element ID
//	RTPRINT(FFW, FW_MSG_H2C_CONTENT, ("[FW], Write ElementID BOXReg(%4x) = %2x \n", BOXReg, ElementID));

	switch(CmdLen)
	{
	case 1:
	case 2:
	case 3:
	{
		//BoxContent[0] &= ~(BIT7);
		HAL_memcpy((pu1Byte)(BoxContent)+1, pCmdBuffer+BufIndex, CmdLen);
		//For Endian Free.
		for(idx= 0; idx < 4; idx++)
		{
			HAL_RTL_W8(BOXReg+idx, BoxContent[idx]);
		}
		break;
	}
	case 4: 
	case 5:
	case 6:
	case 7:
	{
		//BoxContent[0] |= (BIT7);
		HAL_memcpy((pu1Byte)(BoxExtContent), pCmdBuffer+BufIndex+3, (CmdLen-3));
		HAL_memcpy((pu1Byte)(BoxContent)+1, pCmdBuffer+BufIndex, 3);
		//For Endian Free.
		for(idx = 0 ; idx < 4 ; idx ++)
		{
			HAL_RTL_W8(BOXExtReg+idx, BoxExtContent[idx]);
		}		
		for(idx = 0 ; idx < 4 ; idx ++)
		{
			HAL_RTL_W8(BOXReg+idx, BoxContent[idx]);
		}
		break;
	}
	
	default:
//RTPRINT(FFW, FW_MSG_H2C_STATE, ("[FW], Invalid command len=%d!!!\n", CmdLen));
		return RT_STATUS_FAILURE;
	}

	if (++pHalData->H2CBufPtr88XX > 3)
		pHalData->H2CBufPtr88XX = 0;

//RTPRINT(FFW, FW_MSG_H2C_CONTENT, ("[FW], pHalData->LastHMEBoxNum = %d\n", pHalData->LastHMEBoxNum));
	return RT_STATUS_SUCCESS;
}
#endif



void C2HHandler88XX(
    IN HAL_PADAPTER     Adapter
)
{

#if 0 // 8881A & 8192E receive C2H by rx packets
	u4Byte      C2H_ID;
    u1Byte      C2HCONTENT[C2H_CONTENT_LEN];
	u4Byte      idx;
    VOID        (*c2hcallback)(IN HAL_PADAPTER Adapter,u1Byte *pbuf);

#ifdef CONFIG_WLAN_HAL_8192EE
	if (GET_CHIP_VER(Adapter) == VERSION_8192E) 
		return;
#endif
	C2H_ID = HAL_RTL_R8(REG_C2HEVT);

	HAL_memset(C2HCONTENT, 0, sizeof(C2HCONTENT));

    //For Endian Free.
    for(idx= 0; idx < C2H_CONTENT_LEN; idx++)
    {
        // content start at Reg[0x1a2]
        C2HCONTENT[idx] = HAL_RTL_R8(idx+(REG_C2HEVT+2));
    }


	if(C2H_ID >=  sizeof(HalC2Hcmds)/sizeof(struct cmdobj)) {
		panic_printk("Get Error C2H ID = %x \n",C2H_ID);
	} else {
	    c2hcallback = HalC2Hcmds[C2H_ID].c2hfuns;
	    if(c2hcallback)	{
	        c2hcallback(Adapter,C2HCONTENT);       
	    } else {
	        RT_TRACE_F(COMP_IO, DBG_WARNING ,("Get Error C2H ID = %x \n",C2H_ID)); 
	    }
	}
	HAL_RTL_W8(REG_C2HEVT + 0xf, 0);
#ifdef TXREPORT	
	if( C2H_ID == 4)
		requestTxReport88XX(Adapter);
#endif	

#endif
}

#if 1

VOID
C2HPacket88XX
(
	IN  HAL_PADAPTER    Adapter,
	IN  pu1Byte			pBuf,
	IN	u1Byte			length
)
{
	u1Byte	c2hCmdId=0, c2hCmdSeq=0, c2hCmdLen=0;
	pu1Byte tmpBuf=NULL;
	c2hCmdId = *(pBuf);
	c2hCmdSeq = *(pBuf+1);

	/*
	if(c2hCmdId==C2H_88XX_EXTEND_IND)
	{
		c2hCmdLen = length;
		tmpBuf = pBuf;
		C2HExtEventHandler88XX(Adapter, c2hCmdId, c2hCmdLen, tmpBuf);
	}
	else
	*/
	{
		c2hCmdLen = length -2;
		tmpBuf = pBuf+2;
		C2HEventHandler88XX(Adapter, c2hCmdId, c2hCmdLen, tmpBuf);
	}
}


VOID
C2HEventHandler88XX
(
    IN HAL_PADAPTER     Adapter,
    IN u1Byte			c2hCmdId, 
    IN u1Byte			c2hCmdLen,
    IN pu1Byte 			tmpBuf    			
)
{
    static u1Byte CmdBuffer = 0x1;
    u1Byte i;
	u1Byte	Extend_c2hSubID = 0;
		
    switch(c2hCmdId)
    {
	case C2H_88XX_DBG:
		phydm_fw_trace_handler(&(Adapter->pshare->_dmODM), tmpBuf, c2hCmdLen);
		break;
    
    case C2H_88XX_LB:
        // get c2h loopback
        printk("c2h content c2hCmdId = 0x%x c2hCmdLen= 0x%x \n", c2hCmdId, c2hCmdLen);

        for(i=0;i<c2hCmdLen;i++)
        {
            printk("%x ",*(tmpBuf+i));    
        }

        printk("\n");

        CmdBuffer++;

        delay_ms(100);
        // issue h2c 0xE0 back
        printk("GEN h2c cmd CmdBuffer = %x \n",CmdBuffer);        
        FillH2CCmd88XX(Adapter,0xE0,1,&CmdBuffer);        
      //  FillH2CCmd88XX(Adapter,0xE0,1,&CmdBuffer);                
    break;
    
    case C2H_88XX_TX_RATE:
#ifdef TXREPORT	
        APReqTXRptHandler(Adapter,tmpBuf);
        requestTxReport88XX(Adapter);
#endif			
    break;

case C2H_88XX_TXBF:
#ifdef BEAMFORMING_SUPPORT
	C2HTxBeamformingHandler88XX(Adapter, tmpBuf, c2hCmdLen);
#endif
    break;

    case C2H_88XX_RA_PARA_RPT:
	ODM_C2HRaParaReportHandler(&(Adapter->pshare->_dmODM), tmpBuf, c2hCmdLen);
		break;
		
	case C2H_88XX_RA_DYNAMIC_TX_PATH_RPT:
		phydm_c2h_dtp_handler(&(Adapter->pshare->_dmODM), tmpBuf, c2hCmdLen);
	//DbgPrint("[C2H] C2H_8192E_RA_PARA_RPT \n");
	break;

	case C2H_88XX_EXTEND_IND:	
		Extend_c2hSubID= tmpBuf[0];
		if(Extend_c2hSubID == EXTEND_C2H_88XX_DBG_PRINT)
		{
			phydm_fw_trace_handler_8051(&(Adapter->pshare->_dmODM), tmpBuf, c2hCmdLen);
		}
		break;

    default:
    break;
    }
}
#endif
VOID
C2HExtEventHandler88XX
(
    IN HAL_PADAPTER     Adapter,
    IN u1Byte			c2hCmdId, 
    IN u1Byte			c2hCmdLen,
    IN pu1Byte 			tmpBuf    			
)
{
		
	//printk("c2h content c2hCmdId = 0x%x  \n",C2H_88XX_EXTEND_IND);
	//printk("total length=%d\n",c2hCmdLen);
	//printk("pkt inside=%s\n",tmpBuf);

	u1Byte c2hissued_len = 0;
	
	u1Byte cmdseq = 0;
	u1Byte cmdsubID = 0;
	u1Byte cmdsubLen = 0;
	u1Byte cmdsubSeq = 0;
	u1Byte cmdsubcontent[LENGTH_C2HEXT_CONTENT];
	u2Byte count_i=0;
	pu1Byte print_tmp=tmpBuf;
	
	while(c2hissued_len < c2hCmdLen-1)
	{
		HAL_memset(cmdsubcontent, 0, sizeof(cmdsubcontent));
		cmdseq = *(tmpBuf+1);
		cmdsubID = *(tmpBuf+2);
		cmdsubLen = *(tmpBuf+3);
		cmdsubSeq = *(tmpBuf+4);
		c2hissued_len = c2hissued_len+cmdsubLen+4-1; //hdr len = 4
		
		HAL_memcpy(cmdsubcontent,(tmpBuf+5),(cmdsubLen-1));
		cmdsubcontent[cmdsubLen-2]='\0';
		switch(cmdsubID)
		{
			case 0:
			printk("[RTKFW, seq=%d] %s",cmdsubSeq,cmdsubcontent);
		}
		tmpBuf = tmpBuf+cmdsubLen+4-1;
	}
	
}

#ifdef BEAMFORMING_SUPPORT
VOID
C2HTxBeamformingHandler88XX(
	struct rtl8192cd_priv *priv,
	pu1Byte			CmdBuf,
	u1Byte			CmdLen
)
{
#ifdef CONFIG_WLAN_HAL_8814AE
	C2HTxBeamformingHandler_8814A(priv, CmdBuf, CmdLen);
#endif

}
#endif

BOOLEAN
DownloadRsvdPage88XX
( 
	IN  HAL_PADAPTER    Adapter,
    IN  pu4Byte         beaconbuf,    
    IN  u4Byte          beaconPktLen,
    IN  u1Byte          bReDownload
)
{
    u1Byte      wait_cnt = 0;

    SetBeaconDownload88XX(Adapter, HW_VAR_BEACON_ENABLE_DOWNLOAD);
    HAL_RTL_W8(REG_RX_RXBD_NUM+1, HAL_RTL_R8(REG_RX_RXBD_NUM+1) | BIT(4));

    while((HAL_RTL_R8(REG_RX_RXBD_NUM+1) & BIT(4))== 0x10)
    {
        if (++wait_cnt > 100) { 
            RT_TRACE_F(COMP_INIT, DBG_SERIOUS, ("Download Img fail\n"));
            return _FALSE;
        }
       delay_us(10);
    }

    if(bReDownload) {
        // download small beacon
        SigninBeaconTXBD88XX(Adapter, beaconbuf, beaconPktLen);
        
        HAL_RTL_W8(REG_RX_RXBD_NUM+1, BIT(4));
    }

    return _TRUE;
}
#endif //if IS_RTL88XX_GENERATION

