enum { __FILE_NUM__= 3 };

//========= =================================================
//3  -                                                   Include File  
//==========================================================
#include <linux/types.h>
#include <rtl_types.h>
#include "wl_8881.h"
#include "wl_common_hw.h"
//==========================================================


//========= =================================================
//3  -                                                   Static Function Declare   
//==========================================================
VOID  
LLTWrite(
    IN  u4Byte     address,
    IN  u4Byte     data
);

u1Byte 
LLTRead(
    IN  u4Byte     Address
);

VOID
PHY_RF6052SetBandwidth8812(
 	IN	HT_CHANNEL_WIDTH    Bandwidth
);

u1Byte 
phy_GetSecondaryChnl_8812(
    IN  u1Byte      CurrentChannelBW,
    IN  u1Byte      nCur80MhzPrimeSC,
    IN  u1Byte      nCur40MhzPrimeSC
);

VOID
phy_SetRegBW_8812(
    IN  u1Byte      CurrentBW
)	;

BOOLEAN
phy_SwBand8812(
    IN  u4Byte	   CurrentWirelessMode,
    IN  u4Byte     CurrentChannel
);

VOID 
phy_SetTxPowerIndexByRateArray(
    IN  u1Byte              RFPath,
    IN  HT_CHANNEL_WIDTH    BandWidth,	
    IN  u1Byte              Channel,
    IN  pu1Byte             Rates,
    IN  u1Byte              RateArraySize,
    IN	u4Byte 	            powerIndex
);

//==========================================================




VOID  
LLTWrite(
    IN  u4Byte     address,
    IN  u4Byte     data
)
{
    u4Byte Value32;
    u4Byte Count = 0;

    Value32= LLT_INIT_ADDR(address) | LLT_INIT_DATA(data) | LLT_OP(LLT_WRITE_ACCESS);
    HAL_RTL_W32(REG_LLT_INIT, Value32);

    //polling
    do{
        
        Value32 = HAL_RTL_R32(REG_LLT_INIT);
        if (LLT_NO_ACTIVE == LLT_OP_VALUE(Value32)){
            break;
        }
        
        if (Count > LLT_POLLING_THRESHOLD) {
            // Throw exception
            dprintf("init_LLT_table failed\n");
            break;
        }
    }while(Count++);
    
}

u1Byte 
LLTRead(
    IN  u4Byte     Address
)
{
    u4Byte Value32;

    Value32= LLT_INIT_ADDR(Address) | LLT_OP(LLT_READ_ACCESS);
    HAL_RTL_W32(REG_LLT_INIT, Value32);

    Value32 = HAL_RTL_R32(REG_LLT_INIT);
    
    return LLT_INIT_DATA(Value32);
}


VOID 
InitLLTTable(
    IN  u4Byte     TxPktBufBndy, 
    IN  u4Byte     LastEntryTxPktBuf
)
{
    u2Byte PageIndex;

    // Reserve half of pages for local loopback.
//  u4Byte TxPktBufBndy = ;
//  u4Byte LastEntryTxPktBuf= ;


    for (PageIndex = 0; PageIndex < (TxPktBufBndy-1); PageIndex++) {
        LLTWrite(PageIndex , PageIndex + 1);
    }

    // end of list
    LLTWrite(TxPktBufBndy-1, 0x00); 

    // Make the other pages as ring buffer
    // This ring buffer is used as beacon buffer if we config this MAC as two MAC transfer.
    // Otherwise used as local loopback buffer. 
    for (PageIndex = TxPktBufBndy; PageIndex < LastEntryTxPktBuf; PageIndex++) {
        LLTWrite(PageIndex, (PageIndex + 1)); 
    }

    // Let last entry point to the start entry of ring buffer
    LLTWrite(LastEntryTxPktBuf, TxPktBufBndy);    

}


VOID 
Chk_Sum(
    IN  TX_DESC     *TxDesc
)
{
	u2Byte ChkRsult=0x0000;
	u2Byte *pData=NULL;
	u4Byte i=0;
	u2Byte Array[16];
    u4Byte temp = 0;

    TxDesc->Dword7 = SET_DESC(TxDesc->Dword7,
                                  (0x0000),
                                  TX_DW7_SW_TXBUFF_MSK,
                                  TX_DW7_SW_TXBUFF_SH);

	pData=(u2Byte *)(TxDesc);
	memcpy(Array,TxDesc,16);

	for(i=0;i<8;i++)
	{
		ChkRsult^=(pData[2*i]^pData[2*i+1]);
	}

    TxDesc->Dword7 = SET_DESC(TxDesc->Dword7,
                                  (ChkRsult),
                                  TX_DW7_SW_TXBUFF_MSK,
                                  TX_DW7_SW_TXBUFF_SH);

}




/**
* Function:	phy_CalculateBitShift
*
* OverView:	Get shifted position of the BitMask
*
* Input:
*			u4Byte		BitMask,
*
* Output:	none
* Return:		u4Byte		Return the shift bit bit position of the mask
*/
u4Byte 
phy_CalculateBitShift(
    IN  u4Byte     BitMask
)
{
	u4Byte i;

	for (i=0; i<=31; i++) {
		if (((BitMask>>i) & 0x1) == 1)
			break;
	}

	return (i);
}


/**
* Function:	PHY_SetBBReg
*
* OverView:	Write "Specific bits" to BB register (page 8~)
*
* Input:
*			PADAPTER		Adapter,
*			u4Byte			RegAddr,		//The target address to be modified
*			u4Byte			BitMask		//The target bit position in the target address
*										//to be modified
*			u4Byte			Data			//The new register value in the target bit position
*										//of the target address
*
* Output:	None
* Return:		None
* Note:		This function is equal to "PutRegSetting" in PHY programming guide
*/
VOID 
PHY_SetBBReg(
    IN  u4Byte     RegAddr, 
    IN  u4Byte     BitMask, 
    IN  u4Byte     Data
)
{
	u4Byte OriginalValue, BitShift, NewValue;


	if (BitMask != bMaskDWord)
	{//if not "double word" write
	 //_TXPWR_REDEFINE ?? if have original value, how to count tx power index from PG file ??
		OriginalValue = HAL_RTL_R32(RegAddr);
		BitShift = phy_CalculateBitShift(BitMask);
		NewValue = ((OriginalValue & (~BitMask)) | (Data << BitShift));
		HAL_RTL_W32(RegAddr, NewValue);
	}
	else
		HAL_RTL_W32(RegAddr, Data);

	return;
}


/**
* Function:	PHY_QueryBBReg
*
* OverView:	Read "sepcific bits" from BB register
*
* Input:
*			PADAPTER		Adapter,
*			u4Byte			RegAddr,		//The target address to be readback
*			u4Byte			BitMask		//The target bit position in the target address
*										//to be readback
* Output:	None
* Return:		u4Byte			Data			//The readback register value
* Note:		This function is equal to "GetRegSetting" in PHY programming guide
*/
u4Byte 
PHY_QueryBBReg(
    IN  u4Byte     RegAddr, 
    IN  u4Byte     BitMask
)
{
  	u4Byte ReturnValue = 0, OriginalValue, BitShift;

	OriginalValue = HAL_RTL_R32(RegAddr);
	BitShift = phy_CalculateBitShift(BitMask);
	ReturnValue = (OriginalValue & BitMask) >> BitShift;

	return (ReturnValue);
}


//for PutRFRegsetting & GetRFRegSetting BitMask
#define		bMask12Bits					0xfff	// RF Reg mask bits
#define		bMask20Bits					0xfffff	// RF Reg mask bits T65 RF


u4Byte 
PHY_QueryRFReg_8812(
    IN  u4Byte     RfPath,
    IN  u4Byte     RegAddr,
    IN  u4Byte     BitMask, 
    IN  u4Byte     dbg_avoid
)
{
	u4Byte Original_Value, Readback_Value, BitShift;
	u4Byte dwTmp;	
				
	RTL_W8(0x8b0, RegAddr);
	
	dwTmp = RTL_R32(0x8b0);
	dwTmp &= ~ BIT(8);
	RTL_W32(0x8b0, dwTmp); 
	//printk("0x8b0 = 0x%x\n", dwTmp);
	dwTmp |= BIT(8);
	RTL_W32(0x8b0, dwTmp); 
	//printk("0x8b0 = 0x%x\n", dwTmp);

	delay_us(10);

	Original_Value = RTL_R32(0xd04);

	Original_Value &= 0xfffff;
	
	BitShift =	phy_CalculateBitShift(BitMask);
	Readback_Value = (Original_Value & BitMask) >> BitShift;
	
	return (Readback_Value);
}

VOID 
phy_RFSerialWrite_8812(
    IN  u4Byte     RfPath,
    IN  u4Byte     Offset, 
    IN  u4Byte     Data
)
{
	u4Byte dwTmp = 0;
	u4Byte value = ((Offset << 20) | Data);

	//printk("_eric_8182 phy_RFSerialWrite_8812 +++ \n");

	if(RfPath == 0) //Path A
	{
		dwTmp = RTL_R32(0xc90);
		dwTmp &= 0xf0000000;
		dwTmp |= value;
		//printk("_eric_8812 0xc90 = 0x%x \n", dwTmp);
		RTL_W32(0xc90, dwTmp);
	}
	else if(RfPath == 1) //Path B
	{
		dwTmp = RTL_R32(0xe90);
		dwTmp &= 0xf0000000;
		dwTmp |= value;
		//printk("_eric_8812 0xe90 = 0x%x \n", dwTmp);
		RTL_W32(0xe90, dwTmp);
	}
}



VOID 
PHY_SetRFReg_8812(
    IN  u4Byte     RfPath,
    IN  u4Byte     RegAddr,
    IN  u4Byte     BitMask, 
    IN  u4Byte     Data
)
{
	u4Byte Original_Value, BitShift, New_Value;
	u4Byte flags;

	if (BitMask != bMask20Bits) {
		Original_Value = PHY_QueryRFReg_8812(RfPath, RegAddr, bMask20Bits, 0);
		BitShift = phy_CalculateBitShift(BitMask);
		New_Value = ((Original_Value & (~BitMask)) | (Data << BitShift));
		phy_RFSerialWrite_8812(RfPath, RegAddr, New_Value);
	} else {
		phy_RFSerialWrite_8812(RfPath, RegAddr, Data);
	}

}



VOID 
PHY_SetRFReg(
    IN  u4Byte     Path,   
    IN  u4Byte     RegAddr,
    IN  u4Byte     BitMask, 
    IN  u4Byte     Data
)
{
	u4Byte Original_Value, BitShift, New_Value;
	u4Byte flags;

    return PHY_SetRFReg_8812(Path, RegAddr, BitMask, Data);

}


u4Byte 
PHY_QueryRFReg(
    IN  u4Byte     RfPath,
    IN  u4Byte     RegAddr, 
    IN  u4Byte     BitMask, 
    IN  u4Byte     dbg_avoid
)
{
	u4Byte	Original_Value, Readback_Value, BitShift;


    return PHY_QueryRFReg_8812(RfPath, RegAddr, BitMask, dbg_avoid);
}


u4Byte UsingChannel;
u4Byte UsingBand;
u4Byte UsingBW;

/*-----------------------------------------------------------------------------
 * Function:    PHY_RF6052SetBandwidth()
 *
 * Overview:    This function is called by SetBWModeCallback8190Pci() only
 *
 * Input:       PADAPTER				Adapter
 *			WIRELESS_BANDWIDTH_E	Bandwidth	//20M or 40M
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Note:		For RF type 0222D
 *---------------------------------------------------------------------------*/
VOID
PHY_RF6052SetBandwidth8812(
 	IN	HT_CHANNEL_WIDTH    Bandwidth
)	//20M or 40M
{	
 	
	switch(Bandwidth)
	{
		case HT_CHANNEL_WIDTH_20:
			dprintf("PHY_RF6052SetBandwidth8812(), set 20MHz, Channel Value = 0x%x \n",UsingChannel);
			PHY_SetRFReg(0, RF_CHNLBW, BIT11|BIT10, 3);
		break;
			
		case HT_CHANNEL_WIDTH_40:
			dprintf("PHY_RF6052SetBandwidth8812(), set 40MHz, Channel Value = 0x%x \n", UsingChannel);
			PHY_SetRFReg(0, RF_CHNLBW, BIT11|BIT10, 1);	
		break;
		
		case HT_CHANNEL_WIDTH_80:
			dprintf("PHY_RF6052SetBandwidth8812(), set 40MHz, Channel Value = 0x%x \n", UsingChannel);
			PHY_SetRFReg(0, RF_CHNLBW, BIT11|BIT10, 0);	
		break;
			
		default:
			dprintf("PHY_RF6052SetBandwidth8812(): unknown Bandwidth: %#X\n",Bandwidth );
			break;			
	}
}




VOID
phy_SetRegBW_8812(
    IN  u1Byte      CurrentBW
)	
{
	u2Byte	RegRfMod_BW, u2tmp = 0;
	RegRfMod_BW = ODM_Read2Byte(REG_WMAC_TRXPTCL_CTL);

	switch(CurrentBW)
	{
		case HT_CHANNEL_WIDTH_20:
			ODM_Write2Byte(REG_WMAC_TRXPTCL_CTL, (RegRfMod_BW & 0xFE7F)); // BIT 7 = 0, BIT 8 = 0
			break;

		case HT_CHANNEL_WIDTH_40:
			u2tmp = RegRfMod_BW | BIT7;
			ODM_Write2Byte(REG_WMAC_TRXPTCL_CTL, (u2tmp & 0xFEFF)); // BIT 7 = 1, BIT 8 = 0
			break;

		case HT_CHANNEL_WIDTH_80:
			u2tmp = RegRfMod_BW | BIT8;
			ODM_Write2Byte(REG_WMAC_TRXPTCL_CTL, (u2tmp & 0xFF7F)); // BIT 7 = 0, BIT 8 = 1
			break;

		default:
			dprintf("phy_PostSetBWMode8812():	unknown Bandwidth: %#X\n",CurrentBW);
			break;
	}

}



VOID
phy_PostSetBwMode8812(
    IN  u1Byte      CurrentChannelBW,
    IN  u1Byte      nCur80MhzPrimeSC,
    IN  u1Byte      nCur40MhzPrimeSC
)
{
	u1Byte			SubChnlNum = 0;


	//3 Set Reg668 Reg440 BW
	phy_SetRegBW_8812(CurrentChannelBW);

	//3 Set Reg483
	SubChnlNum = phy_GetSecondaryChnl_8812(CurrentChannelBW, nCur80MhzPrimeSC, nCur40MhzPrimeSC);
	HAL_RTL_W8(REG_DATA_SC, SubChnlNum);

#if 0
	if(pHalData->RFChipID == RF_PSEUDO_11N)
	{
		dprintf("phy_PostSetBwMode8812: return for PSEUDO \n");
		return;
	}
#endif

	//3 Set Reg8AC RegA00
	switch(CurrentChannelBW)
	{
		case HT_CHANNEL_WIDTH_20:
			PHY_SetBBReg(rRFMOD_Jaguar, 0x003003C3, 0x00300200); // 0x8ac[21,20,9:6,1,0]=8'b11100000
			break;
			   
		case HT_CHANNEL_WIDTH_40:
			PHY_SetBBReg(rRFMOD_Jaguar, 0x003003C3, 0x00300201); // 0x8ac[21,20,9:6,1,0]=8'b11100000		
			PHY_SetBBReg(rRFMOD_Jaguar, 0x3C, SubChnlNum);
			PHY_SetBBReg(0x838, 0xf0000000, SubChnlNum);

			if(SubChnlNum == VHT_DATA_SC_20_UPPER_OF_80MHZ)
				PHY_SetBBReg(rCCK_System_Jaguar, bCCK_System_Jaguar, 1);
			else
				PHY_SetBBReg(rCCK_System_Jaguar, bCCK_System_Jaguar, 0);
			break;

		case HT_CHANNEL_WIDTH_80:
			PHY_SetBBReg(rRFMOD_Jaguar, 0x003003C3, 0x00300202); // 0x8ac[21,20,9:6,1,0]=8'b11100010
			PHY_SetBBReg(rRFMOD_Jaguar, 0x3C, SubChnlNum);
			PHY_SetBBReg(0x838, 0xf0000000, SubChnlNum);
			break;

		default:
			dprintf("phy_PostSetBWMode8812():	unknown Bandwidth: %#X\n",CurrentChannelBW);
			break;
	}
	dprintf("phy_PostSetBwMode8812(): Reg483: %x\n", HAL_RTL_R8(0x483));
	dprintf("phy_PostSetBwMode8812():	Reg668: %x\n", HAL_RTL_R32(0x668));
	dprintf("phy_PostSetBwMode8812():	Reg8AC: %x\n", PHY_QueryBBReg(rRFMOD_Jaguar, 0xffffffff));

	//3 Set RF related register         
	PHY_RF6052SetBandwidth8812(CurrentChannelBW);

    UsingBW = CurrentChannelBW;
}



VOID
PHY_SwitchWirelessBand8812(
    IN  u4Byte     Band
)
{
	// STOP Tx/Rx
	PHY_SetBBReg(rOFDMCCKEN_Jaguar, bOFDMEN_Jaguar|bCCKEN_Jaguar, 0x00);
	
	if(Band == BAND_ON_2_4G)
	{// 2.4G band
		dprintf("==>PHY_UpdateBBRFConfiguration8812() BAND_ON_2_4G settings\n");

		// AGC table select 
		PHY_SetBBReg(rAGC_table_Jaguar, 0x3, 0);

		
		// cck_enable
		PHY_SetBBReg(rOFDMCCKEN_Jaguar, bOFDMEN_Jaguar|bCCKEN_Jaguar, 0x3);


		// CCK_CHECK_en
		ODM_Write1Byte(REG_CCK_CHECK, 0x0);
	}
	else	//5G band
	{
		dprintf("==>PHY_UpdateBBRFConfiguration8812() BAND_ON_5G settings\n");

		// CCK_CHECK_en
		ODM_Write1Byte(REG_CCK_CHECK, 0x80);

		// AGC table select 
		PHY_SetBBReg(rAGC_table_Jaguar, 0x3, 1);

		// cck_enable
		PHY_SetBBReg(rOFDMCCKEN_Jaguar, bOFDMEN_Jaguar|bCCKEN_Jaguar, 0x2);

	}
    UsingBand = Band;
    
	dprintf("<==PHY_SwitchWirelessBand8812():Switch Band OK.\n");
}

//1 5. BandWidth setting API
/*-----------------------------------------------------------------------------
 * Function:    _PHY_SetBWMode8812()
 *
 * Overview:    Timer callback function for SetSetBWMode
 *
 * Input:       	PRT_TIMER		pTimer
 *
 * Output:      NONE
 *
 * Return:      NONE
 *
 * Note:		(1) We do not take j mode into consideration now
 *			(2) Will two workitem of "switch channel" and "switch channel bandwidth" run
 *			     concurrently?
 *---------------------------------------------------------------------------*/
u1Byte 
phy_GetSecondaryChnl_8812(
    IN  u1Byte      CurrentChannelBW,
    IN  u1Byte      nCur80MhzPrimeSC,
    IN  u1Byte      nCur40MhzPrimeSC
)
{
	u1Byte		SCSettingOf40 = 0, SCSettingOf20 = 0;

	dprintf("SCMapping: VHT Case: pHalData->CurrentChannelBW %d, pHalData->nCur80MhzPrimeSC %d, pHalData->nCur40MhzPrimeSC %d \n",
             CurrentChannelBW,nCur80MhzPrimeSC,nCur40MhzPrimeSC);
	if(CurrentChannelBW== HT_CHANNEL_WIDTH_80)
	{
		if(nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
			SCSettingOf40 = VHT_DATA_SC_40_LOWER_OF_80MHZ;
		else if(nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
			SCSettingOf40 = VHT_DATA_SC_40_UPPER_OF_80MHZ;
		else
			dprintf("SCMapping: Not Correct Primary40MHz Setting \n");
		
		if((nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) && (nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER))
			SCSettingOf20 = VHT_DATA_SC_20_LOWEST_OF_80MHZ;
		else if((nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER) && (nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER))
			SCSettingOf20 = VHT_DATA_SC_20_LOWER_OF_80MHZ;
		else if((nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) && (nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER))
			SCSettingOf20 = VHT_DATA_SC_20_UPPER_OF_80MHZ;
		else if((nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER) && (nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER))
			SCSettingOf20 = VHT_DATA_SC_20_UPPERST_OF_80MHZ;
		else
			dprintf("SCMapping: Not Correct Primary40MHz Setting \n");
	}
	else if(CurrentChannelBW == HT_CHANNEL_WIDTH_40)
	{
		dprintf("SCMapping: VHT Case: pHalData->CurrentChannelBW %d, pHalData->nCur40MhzPrimeSC %d \n",CurrentChannelBW,nCur40MhzPrimeSC);

		if(nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
			SCSettingOf20 = VHT_DATA_SC_20_UPPER_OF_80MHZ;
		else if(nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
			SCSettingOf20 = VHT_DATA_SC_20_LOWER_OF_80MHZ;
		else
			dprintf("SCMapping: Not Correct Primary40MHz Setting \n");
	}

	dprintf("SCMapping: SC Value %x \n", ( (SCSettingOf40 << 4) | SCSettingOf20));
	return  ( (SCSettingOf40 << 4) | SCSettingOf20);
}




//1 6  Channel setting API

VOID
phy_SwChnl8812(
    IN  u4Byte     CurrentWirelessMode,
    IN  u4Byte     CurrentChannel
)
{
	u1Byte			eRFPath = 0;
	u1Byte 			channelToSW = CurrentChannel;
    u4Byte         NumTotalRFPath = 1;  //1 path 1

	if(phy_SwBand8812(CurrentWirelessMode,channelToSW) == FALSE)
	{
		dprintf("error Chnl %d", channelToSW);
	}	

    UsingChannel = CurrentChannel;
	  
	// fc_area		
	if (36 <= channelToSW && channelToSW <= 48) 
		PHY_SetBBReg(rFc_area_Jaguar, 0x1ffe0000, 0x494); 
	else if (50 <= channelToSW && channelToSW <= 64) 
		PHY_SetBBReg(rFc_area_Jaguar, 0x1ffe0000, 0x453);  
	else if (100 <= channelToSW && channelToSW <= 116) 
		PHY_SetBBReg(rFc_area_Jaguar, 0x1ffe0000, 0x452);  
	else if (118 <= channelToSW && channelToSW <= 165) 
		PHY_SetBBReg(rFc_area_Jaguar, 0x1ffe0000, 0x412);  
	else
		PHY_SetBBReg(rFc_area_Jaguar, 0x1ffe0000, 0x96a);
	    	
	for(eRFPath = 0; eRFPath < NumTotalRFPath; eRFPath++)
	{
		// [2.4G] LC Tank

		// RF_MOD_AG
		if (36 <= channelToSW && channelToSW <= 64) 
			PHY_SetRFReg(eRFPath, RF_CHNLBW_Jaguar, BIT18|BIT17|BIT16|BIT9|BIT8, 0x101); //5'b00101); 
		else if (100 <= channelToSW && channelToSW <= 140) 
			PHY_SetRFReg(eRFPath, RF_CHNLBW_Jaguar, BIT18|BIT17|BIT16|BIT9|BIT8, 0x301); //5'b01101); 
		else if (140 < channelToSW) 
			PHY_SetRFReg(eRFPath, RF_CHNLBW_Jaguar, BIT18|BIT17|BIT16|BIT9|BIT8, 0x501); //5'b10101); 
		else	
			PHY_SetRFReg(eRFPath, RF_CHNLBW_Jaguar, BIT18|BIT17|BIT16|BIT9|BIT8, 0x000); //5'b00000); 

		PHY_SetRFReg(eRFPath, RF_CHNLBW_Jaguar, bMaskByte0, channelToSW);
	}
}



//1 7. Band setting API

BOOLEAN
phy_SwBand8812(
    IN  u4Byte	   CurrentWirelessMode,
    IN  u4Byte     CurrentChannel
)
{
	u1Byte			    u1Btmp; 
	BOOLEAN			ret_value = TRUE;
	u1Byte			    Band = BAND_ON_5G;
	WIRELESS_MODE	wirelessmode;
	
	u1Btmp = HAL_RTL_R8(REG_CCK_CHECK);

	if(u1Btmp & BIT7)
		wirelessmode = (WIRELESS_MODE)(WIRELESS_MODE_AC_5G|WIRELESS_MODE_N_5G|WIRELESS_MODE_A);
	else
		wirelessmode = (WIRELESS_MODE)(WIRELESS_MODE_N_24G|WIRELESS_MODE_G|WIRELESS_MODE_B);

	//--------------------------------------------
	switch(CurrentWirelessMode)
	{
		case WIRELESS_MODE_A:
		case WIRELESS_MODE_N_5G:
		case WIRELESS_MODE_AC_5G:
			if(!(wirelessmode & CurrentWirelessMode))
			{
				Band = BAND_ON_5G;
                PHY_SwitchWirelessBand8812(Band);
			}	
			
			if(CurrentChannel <=14)
				ret_value= FALSE;
			break;
			
		case WIRELESS_MODE_B:
		case WIRELESS_MODE_G:
		case WIRELESS_MODE_N_24G:
			if(!(wirelessmode & CurrentWirelessMode))
			{
				Band = BAND_ON_2_4G;
                PHY_SwitchWirelessBand8812(Band);
			}
			if(CurrentChannel>14)
				ret_value= FALSE;
			break;

		default:
			ret_value = FALSE;
			break;
	}
    UsingBand = Band;

	return ret_value;
}


//1 5. Tx  Power setting API


VOID
PHY_SetTxPowerIndex_8812A(
    IN  u4Byte      PowerIndex,
    IN  u1Byte      RFPath,	
    IN  u1Byte      Rate
)
{
    if (RFPath == RF_PATH_A)
    {
        switch (Rate)
        {
            case MGN_1M:    PHY_SetBBReg(rTxAGC_A_CCK11_CCK1_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_2M:    PHY_SetBBReg(rTxAGC_A_CCK11_CCK1_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_5_5M:  PHY_SetBBReg(rTxAGC_A_CCK11_CCK1_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_11M:   PHY_SetBBReg(rTxAGC_A_CCK11_CCK1_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_6M:    PHY_SetBBReg(rTxAGC_A_Ofdm18_Ofdm6_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_9M:    PHY_SetBBReg(rTxAGC_A_Ofdm18_Ofdm6_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_12M:   PHY_SetBBReg(rTxAGC_A_Ofdm18_Ofdm6_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_18M:   PHY_SetBBReg(rTxAGC_A_Ofdm18_Ofdm6_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_24M:   PHY_SetBBReg(rTxAGC_A_Ofdm54_Ofdm24_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_36M:   PHY_SetBBReg(rTxAGC_A_Ofdm54_Ofdm24_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_48M:   PHY_SetBBReg(rTxAGC_A_Ofdm54_Ofdm24_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_54M:   PHY_SetBBReg(rTxAGC_A_Ofdm54_Ofdm24_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_MCS0:  PHY_SetBBReg(rTxAGC_A_MCS3_MCS0_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_MCS1:  PHY_SetBBReg(rTxAGC_A_MCS3_MCS0_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_MCS2:  PHY_SetBBReg(rTxAGC_A_MCS3_MCS0_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_MCS3:  PHY_SetBBReg(rTxAGC_A_MCS3_MCS0_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_MCS4:  PHY_SetBBReg(rTxAGC_A_MCS7_MCS4_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_MCS5:  PHY_SetBBReg(rTxAGC_A_MCS7_MCS4_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_MCS6:  PHY_SetBBReg(rTxAGC_A_MCS7_MCS4_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_MCS7:  PHY_SetBBReg(rTxAGC_A_MCS7_MCS4_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_MCS8:  PHY_SetBBReg(rTxAGC_A_MCS11_MCS8_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_MCS9:  PHY_SetBBReg(rTxAGC_A_MCS11_MCS8_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_MCS10: PHY_SetBBReg(rTxAGC_A_MCS11_MCS8_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_MCS11: PHY_SetBBReg(rTxAGC_A_MCS11_MCS8_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_MCS12: PHY_SetBBReg(rTxAGC_A_MCS15_MCS12_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_MCS13: PHY_SetBBReg(rTxAGC_A_MCS15_MCS12_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_MCS14: PHY_SetBBReg(rTxAGC_A_MCS15_MCS12_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_MCS15: PHY_SetBBReg(rTxAGC_A_MCS15_MCS12_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_VHT1SS_MCS0: PHY_SetBBReg(rTxAGC_A_Nss1Index3_Nss1Index0_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT1SS_MCS1: PHY_SetBBReg(rTxAGC_A_Nss1Index3_Nss1Index0_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT1SS_MCS2: PHY_SetBBReg(rTxAGC_A_Nss1Index3_Nss1Index0_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT1SS_MCS3: PHY_SetBBReg(rTxAGC_A_Nss1Index3_Nss1Index0_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_VHT1SS_MCS4: PHY_SetBBReg(rTxAGC_A_Nss1Index7_Nss1Index4_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT1SS_MCS5: PHY_SetBBReg(rTxAGC_A_Nss1Index7_Nss1Index4_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT1SS_MCS6: PHY_SetBBReg(rTxAGC_A_Nss1Index7_Nss1Index4_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT1SS_MCS7: PHY_SetBBReg(rTxAGC_A_Nss1Index7_Nss1Index4_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_VHT1SS_MCS8: PHY_SetBBReg(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT1SS_MCS9: PHY_SetBBReg(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT2SS_MCS0: PHY_SetBBReg(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT2SS_MCS1: PHY_SetBBReg(rTxAGC_A_Nss2Index1_Nss1Index8_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_VHT2SS_MCS2: PHY_SetBBReg(rTxAGC_A_Nss2Index5_Nss2Index2_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT2SS_MCS3: PHY_SetBBReg(rTxAGC_A_Nss2Index5_Nss2Index2_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT2SS_MCS4: PHY_SetBBReg(rTxAGC_A_Nss2Index5_Nss2Index2_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT2SS_MCS5: PHY_SetBBReg(rTxAGC_A_Nss2Index5_Nss2Index2_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_VHT2SS_MCS6: PHY_SetBBReg(rTxAGC_A_Nss2Index9_Nss2Index6_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT2SS_MCS7: PHY_SetBBReg(rTxAGC_A_Nss2Index9_Nss2Index6_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT2SS_MCS8: PHY_SetBBReg(rTxAGC_A_Nss2Index9_Nss2Index6_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT2SS_MCS9: PHY_SetBBReg(rTxAGC_A_Nss2Index9_Nss2Index6_JAguar, bMaskByte3, PowerIndex); break;

            default:
                 dprintf("Invalid Rate!!\n");
                 break;				
        }
    }
    else if (RFPath == RF_PATH_B)
    {
        switch (Rate)
        {
            case MGN_1M:    PHY_SetBBReg(rTxAGC_B_CCK11_CCK1_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_2M:    PHY_SetBBReg(rTxAGC_B_CCK11_CCK1_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_5_5M:  PHY_SetBBReg(rTxAGC_B_CCK11_CCK1_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_11M:   PHY_SetBBReg(rTxAGC_B_CCK11_CCK1_JAguar, bMaskByte3, PowerIndex); break;
                                                         
            case MGN_6M:    PHY_SetBBReg(rTxAGC_B_Ofdm18_Ofdm6_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_9M:    PHY_SetBBReg(rTxAGC_B_Ofdm18_Ofdm6_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_12M:   PHY_SetBBReg(rTxAGC_B_Ofdm18_Ofdm6_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_18M:   PHY_SetBBReg(rTxAGC_B_Ofdm18_Ofdm6_JAguar, bMaskByte3, PowerIndex); break;
                                                         
            case MGN_24M:   PHY_SetBBReg(rTxAGC_B_Ofdm54_Ofdm24_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_36M:   PHY_SetBBReg(rTxAGC_B_Ofdm54_Ofdm24_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_48M:   PHY_SetBBReg(rTxAGC_B_Ofdm54_Ofdm24_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_54M:   PHY_SetBBReg(rTxAGC_B_Ofdm54_Ofdm24_JAguar, bMaskByte3, PowerIndex); break;
                                                         
            case MGN_MCS0:  PHY_SetBBReg(rTxAGC_B_MCS3_MCS0_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_MCS1:  PHY_SetBBReg(rTxAGC_B_MCS3_MCS0_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_MCS2:  PHY_SetBBReg(rTxAGC_B_MCS3_MCS0_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_MCS3:  PHY_SetBBReg(rTxAGC_B_MCS3_MCS0_JAguar, bMaskByte3, PowerIndex); break;
                                                         
            case MGN_MCS4:  PHY_SetBBReg(rTxAGC_B_MCS7_MCS4_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_MCS5:  PHY_SetBBReg(rTxAGC_B_MCS7_MCS4_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_MCS6:  PHY_SetBBReg(rTxAGC_B_MCS7_MCS4_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_MCS7:  PHY_SetBBReg(rTxAGC_B_MCS7_MCS4_JAguar, bMaskByte3, PowerIndex); break;
                                                         
            case MGN_MCS8:  PHY_SetBBReg(rTxAGC_B_MCS11_MCS8_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_MCS9:  PHY_SetBBReg(rTxAGC_B_MCS11_MCS8_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_MCS10: PHY_SetBBReg(rTxAGC_B_MCS11_MCS8_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_MCS11: PHY_SetBBReg(rTxAGC_B_MCS11_MCS8_JAguar, bMaskByte3, PowerIndex); break;
                                                         
            case MGN_MCS12: PHY_SetBBReg(rTxAGC_B_MCS15_MCS12_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_MCS13: PHY_SetBBReg(rTxAGC_B_MCS15_MCS12_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_MCS14: PHY_SetBBReg(rTxAGC_B_MCS15_MCS12_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_MCS15: PHY_SetBBReg(rTxAGC_B_MCS15_MCS12_JAguar, bMaskByte3, PowerIndex); break;

            case MGN_VHT1SS_MCS0: PHY_SetBBReg(rTxAGC_B_Nss1Index3_Nss1Index0_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT1SS_MCS1: PHY_SetBBReg(rTxAGC_B_Nss1Index3_Nss1Index0_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT1SS_MCS2: PHY_SetBBReg(rTxAGC_B_Nss1Index3_Nss1Index0_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT1SS_MCS3: PHY_SetBBReg(rTxAGC_B_Nss1Index3_Nss1Index0_JAguar, bMaskByte3, PowerIndex); break;
                                                               
            case MGN_VHT1SS_MCS4: PHY_SetBBReg(rTxAGC_B_Nss1Index7_Nss1Index4_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT1SS_MCS5: PHY_SetBBReg(rTxAGC_B_Nss1Index7_Nss1Index4_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT1SS_MCS6: PHY_SetBBReg(rTxAGC_B_Nss1Index7_Nss1Index4_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT1SS_MCS7: PHY_SetBBReg(rTxAGC_B_Nss1Index7_Nss1Index4_JAguar, bMaskByte3, PowerIndex); break;
                                                               
            case MGN_VHT1SS_MCS8: PHY_SetBBReg(rTxAGC_B_Nss2Index1_Nss1Index8_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT1SS_MCS9: PHY_SetBBReg(rTxAGC_B_Nss2Index1_Nss1Index8_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT2SS_MCS0: PHY_SetBBReg(rTxAGC_B_Nss2Index1_Nss1Index8_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT2SS_MCS1: PHY_SetBBReg(rTxAGC_B_Nss2Index1_Nss1Index8_JAguar, bMaskByte3, PowerIndex); break;
                                                               
            case MGN_VHT2SS_MCS2: PHY_SetBBReg(rTxAGC_B_Nss2Index5_Nss2Index2_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT2SS_MCS3: PHY_SetBBReg(rTxAGC_B_Nss2Index5_Nss2Index2_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT2SS_MCS4: PHY_SetBBReg(rTxAGC_B_Nss2Index5_Nss2Index2_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT2SS_MCS5: PHY_SetBBReg(rTxAGC_B_Nss2Index5_Nss2Index2_JAguar, bMaskByte3, PowerIndex); break;
                                                               
            case MGN_VHT2SS_MCS6: PHY_SetBBReg(rTxAGC_B_Nss2Index9_Nss2Index6_JAguar, bMaskByte0, PowerIndex); break;
            case MGN_VHT2SS_MCS7: PHY_SetBBReg(rTxAGC_B_Nss2Index9_Nss2Index6_JAguar, bMaskByte1, PowerIndex); break;
            case MGN_VHT2SS_MCS8: PHY_SetBBReg(rTxAGC_B_Nss2Index9_Nss2Index6_JAguar, bMaskByte2, PowerIndex); break;
            case MGN_VHT2SS_MCS9: PHY_SetBBReg(rTxAGC_B_Nss2Index9_Nss2Index6_JAguar, bMaskByte3, PowerIndex); break;

            default:
                 dprintf("Invalid Rate!!\n");
                 break;			
        }
    }
    else
    {
		dprintf("Invalid RFPath!!\n");
    }
    
}

VOID 
phy_SetTxPowerIndexByRateArray(
    IN  u1Byte              RFPath,
    IN  HT_CHANNEL_WIDTH    BandWidth,	
    IN  u1Byte              Channel,
    IN  pu1Byte             Rates,
    IN  u1Byte              RateArraySize,
    IN	u4Byte 	            powerIndex
)
{
    int i = 0;

    for (i = 0; i < RateArraySize; ++i) {
        PHY_SetTxPowerIndex_8812A(powerIndex, RFPath, Rates[i]);
    }

}


VOID
PHY_SetTxPowerLevel8812(
    IN  u1Byte      channel,
    IN	u4Byte 	    powerIndex
)
{

    u1Byte          path = 0;
    u1Byte          cckRates[] = {MGN_1M, MGN_2M, MGN_5_5M, MGN_11M};
    u1Byte          ofdmRates[] = {MGN_6M, MGN_9M, MGN_12M, MGN_18M, MGN_24M, MGN_36M, MGN_48M, MGN_54M};
    u1Byte          htRates1T[]  = 
                                {MGN_MCS0, MGN_MCS1, MGN_MCS2, MGN_MCS3, MGN_MCS4, MGN_MCS5, MGN_MCS6, MGN_MCS7};
    u1Byte          htRates2T[]  = 
                                {MGN_MCS8,  MGN_MCS9, MGN_MCS10, MGN_MCS11, MGN_MCS12, MGN_MCS13, MGN_MCS14, MGN_MCS15};
    u1Byte          vhtRates1T[]  = 
                                {MGN_VHT1SS_MCS0, MGN_VHT1SS_MCS1, MGN_VHT1SS_MCS2, MGN_VHT1SS_MCS3, MGN_VHT1SS_MCS4, 
                                MGN_VHT1SS_MCS5, MGN_VHT1SS_MCS6, MGN_VHT1SS_MCS7, MGN_VHT1SS_MCS8, MGN_VHT1SS_MCS9};
    u1Byte          vhtRates2T[]  = 
                                {MGN_VHT2SS_MCS0, MGN_VHT2SS_MCS1, MGN_VHT2SS_MCS2, MGN_VHT2SS_MCS3, MGN_VHT2SS_MCS4, 
                                MGN_VHT2SS_MCS5, MGN_VHT2SS_MCS6, MGN_VHT2SS_MCS7, MGN_VHT2SS_MCS8, MGN_VHT2SS_MCS9};
    
    u1Byte          NumTotalRFPath;

    NumTotalRFPath = 1;//Only path A


    for(path = RF_PATH_A; path < NumTotalRFPath; path++)
    {
        if(UsingBand == BAND_ON_5G)
        {
            phy_SetTxPowerIndexByRateArray(path, UsingBW, channel, vhtRates1T,
                                           sizeof(vhtRates1T)/sizeof(u1Byte),
                                           powerIndex);
        	
        	if(NumTotalRFPath >= 2)
        	    phy_SetTxPowerIndexByRateArray(path, UsingBW, channel, vhtRates2T,
        	                                   sizeof(vhtRates2T)/sizeof(u1Byte),
        	                                   powerIndex);
        }
        else if(UsingBand == BAND_ON_2_4G)
            phy_SetTxPowerIndexByRateArray(path, UsingBW, channel, cckRates, 
                                           sizeof(cckRates)/sizeof(u1Byte),
                                           powerIndex);
        
        phy_SetTxPowerIndexByRateArray(path, UsingBW, channel, ofdmRates,
                                       sizeof(ofdmRates)/sizeof(u1Byte), 
                                       powerIndex);
        
        phy_SetTxPowerIndexByRateArray(path, UsingBW, channel, htRates1T, 
                                       sizeof(htRates1T)/sizeof(u1Byte), 
                                       powerIndex);

        if(NumTotalRFPath >= 2)
            phy_SetTxPowerIndexByRateArray(path, UsingBW, channel, htRates2T, 
                                           sizeof(htRates2T)/sizeof(u1Byte), 
                                           powerIndex);
    }
}



