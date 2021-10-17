
#include "data_8881/data_rtl8051_RAMafw.c"
#include "data_8881/data_rtl8051_ROMafw.c"
#include "data_8881/data_rtl8051_TXBUFafw.c"
#include <rtl_types.h>
#include <linux/types.h>
#include <linux/synclink.h>
#include <linux/byteorder/little_endian.h>
#include "wl_common_ctrl.h"

#if 0
typedef unsigned char			u1Byte,*pu1Byte;
typedef unsigned short			u2Byte,*pu2Byte;
typedef unsigned int			u4Byte,*pu4Byte;
typedef unsigned long long		u1ByteByte,*pu1ByteByte;

typedef signed char				s1Byte,*ps1Byte;
typedef signed short			s2Byte,*ps2Byte;
typedef signed int				s4Byte,*ps4Byte;
typedef signed long long		s8Byte,*ps8Byte;
typedef unsigned long long		ULONG64,*PULONG64;
#endif

#define VAR_MAPPING(dst,src) \
	u1Byte *data_##dst##_start = &data_##src[0]; \
	u1Byte *data_##dst##_end   = &data_##src[sizeof(data_##src)];

VAR_MAPPING(rtl8051_RAMafw, rtl8051_RAMafw);
VAR_MAPPING(rtl8051_ROMafw, rtl8051_ROMafw);
VAR_MAPPING(rtl8051_TXBUFafw, rtl8051_TXBUFafw);


extern u1Byte *data_rtl8051_RAMafw_start,       *data_rtl8051_RAMafw_end;
extern u1Byte *data_rtl8051_ROMafw_start,       *data_rtl8051_ROMafw_end;
extern u1Byte *data_rtl8051_TXBUFafw_start,     *data_rtl8051_TXBUFafw_end;



//3 Mapping General Function
#define HALMalloc(Adapter, Size)        //kmalloc(Size, GFP_ATOMIC)
#define HAL_free(x)                     //kfree(x)
#define PlatformZeroMemory(Ptr, Size)   //memset(Ptr, 0, Size)
#define HAL_delay_ms(t)                 __delay(t*10000)//delay_ms(t)
#define HAL_delay_us(t)                 __delay(t*10)//delay_us(t)
#define HAL_memcpy(dst, src, cnt)       //memcpy(dst, src, cnt)


//3 Mapping Endian Transformer

/*
 *	Call endian free function when
 *		1. Read/write packet content.
 *		2. Before write integer to IO.
 *		3. After read integer from IO.
*/

//#define HAL_cpu_to_le64
//#define HAL_le64_to_cpu
#define HAL_cpu_to_le32     cpu_to_le32
#define HAL_le32_to_cpu     le32_to_cpu
#define HAL_cpu_to_le16     cpu_to_le16
#define HAL_le16_to_cpu     le16_to_cpu
//#define HAL_cpu_to_be64
//#define HAL_be64_to_cpu
#define HAL_cpu_to_be32     cpu_to_be32
#define HAL_be32_to_cpu     be32_to_cpu
#define HAL_cpu_to_be16     cpu_to_be16
#define HAL_be16_to_cpu     be16_to_cpu


#if 0
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
#endif

#ifndef _TRUE
    #define _TRUE               1
#endif
#ifndef _FALSE	
    #define _FALSE              0
#endif


//Firmware
#define	RT_FIRMWARE_HDR_SIZE	            32
//#define DOWNLOAD_FIRMWARE_RETRY_TIMES       5
#define DOWNLOAD_FIRMWARE_RETRY_TIMES       100
#define FW_DOWNLOAD_START_ADDRESS           0x1000
#define CHECK_FW_RAMCODE_READY_TIMES        10
#define CHECK_FW_RAMCODE_READY_DELAY_MS     100
#define H2CBUF_OCCUPY_DELAY_CNT             30
#define H2CBUF_OCCUPY_DELAY_US              10  
#define DOWNLOAD_RAM                        0
#define DOWNLOAD_ROM                        1
#define TXDES_LEN                           0x28
#define TXPKT_FW_SIZE                       0x7000
//----------------------------------------------------------------------------
//       8192C MCUFWDL bits						(Offset 0x80-83, 32 bits)
//----------------------------------------------------------------------------
#define		RPWM_SHIFT		24	// Host Request Power State.
#define		RPWM_Mask			0x0FF
#define		CPRST				BIT(23)	// 8051 Reset Status.
#define		ROM_DLEN			BIT(19)	// ROM Download Enable (8051 Core will be reseted) FPGA only.
#define		ROM_PGE_SHIFT		16	// ROM Page (FPGA only).
#define		ROM_PGE_Mask		0x07
#define		MAC1_RFINI_RDY	BIT(10)	// 92D_REG, MAC1 MCU Initial RF ready
#define		MAC1_BBINI_RDY	BIT(9)	// 92D_REG, MAC1 MCU Initial BB ready
#define		MAC1_MACINI_RDY	BIT(8)	// 92D_REG, MAC1 MCU Initial MAC ready
#define		MCU_STATUS		BIT(7)	// 92D_REG, 1: SRAM, 0: ROM
#define		WINTINI_RDY		BIT(6)	// WLAN Interrupt Initial ready.
#define		MAC0_RFINI_RDY	BIT(5)	// MAC0 MCU Initial RF ready.
#define		MAC0_BBINI_RDY	BIT(4)	// MAC0 MCU Initial BB ready.
#define		MAC0_MACINI_RDY	BIT(3)	// MAC0 MCU Initial MAC ready.
#define		FWDL_CHKSUM_RPT	BIT(2)	// FWDL CheckSum report, 1: OK, 0 : Faill.
#define		MCUFWDL_RDY		BIT(1)	// Driver set this bit to notify MCU FW Download OK.
#define		MCUFWDL_EN		BIT(0)	// MCU Firmware download enable. 1:Enable, 0:Disable.
#define     RAM_DL_SEL      BIT(7)


#define MACID_NUM                   128
#define TXRPT_START_ADDR    0x8100
#define TXRPT_SIZE          16
#define PAUSE_TXRPT_TIME    (12)
#define IOPATH_TXRPT_ENTRY      0x10
#define IOPATH_TXRPT_OFFSET     0x04

extern RT_STATUS 
InitFirmware88XX
(
    IN  HAL_PADAPTER    Adapter,
    IN  u1Byte              downloadTarget
);

BOOLEAN
VerifyDownloadStatus(
    IN  HAL_PADAPTER    Adapter,
    IN  pu1Byte         pFWRealStart,
    IN  u4Byte          FWRealLen
);

RT_STATUS
DownloadRAMtoTXPktBuf(
    IN  HAL_PADAPTER    Adapter
);


VOID 
DownloadInit(VOID)
{
    // TODO: Filen, code below is necessary to check doublely
{
         // Disable SIC
         HAL_RTL_W8(REG_GPIO_MUXCFG+1, 0x40);
         HAL_delay_ms(1);
    }

     //Clear 0x80,0x81,0x82[0],0x82[1],0x82[2],0x82[3]
     HAL_RTL_W8(REG_8051FW_CTRL,0x0);
     HAL_RTL_W8(REG_8051FW_CTRL+1,0x0);
     HAL_RTL_W8(REG_8051FW_CTRL+2, HAL_RTL_R8(REG_8051FW_CTRL+2)& 0xfff0);
    
     // Enable MCU
     HAL_RTL_W8(REG_SYS_FUNC_EN+1, HAL_RTL_R8(REG_SYS_FUNC_EN+1)&(~BIT2));
     HAL_delay_ms(1);    
     HAL_RTL_W8(REG_SYS_FUNC_EN+1, HAL_RTL_R8(REG_SYS_FUNC_EN+1) | BIT2);
     RT_DBG_FW_LOG("Enable MCU\n");
     HAL_delay_ms(1);

     // Load SRAM
     HAL_RTL_W8(REG_8051FW_CTRL, HAL_RTL_R8(REG_8051FW_CTRL) | MCUFWDL_EN);
     RT_DBG_FW_LOG("Enable MCUFWDL_EN\n");    
     HAL_delay_ms(1);
    
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
    u1Byte      tmp;
    RT_DBG_FW_LOG("[Write]RAM pFWRealStart =%x,len=%x\n",
               pFWRealStart,FWRealLen);

    
    
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
 
			HAL_RTL_W32(WriteAddr, CPUTOLEX32(Temp));
			WriteAddr += 4;

			if(WriteAddr == 0x2000) {
  			    tmp = HAL_RTL_R8(REG_8051FW_CTRL+2);
                
                //Switch to next page
				tmp += 1;

                //Reset Address
				WriteAddr = 0x1000;
                
				HAL_RTL_W8(REG_8051FW_CTRL+2,tmp);

                // TODO: Filen, it can be remove ??
				HAL_delay_ms(10);
			}
			CurPtr += 4;
		}
	}
}

BOOLEAN
VerifyDownloadStatus(
    IN  HAL_PADAPTER    Adapter,
    IN  pu1Byte         pFWRealStart,
    IN  u4Byte          FWRealLen
)
{
    u4Byte      WriteAddr   = FW_DOWNLOAD_START_ADDRESS;
    u4Byte      CurPtr      = 0;
    u4Byte      binTemp;
    u4Byte      ROMTemp;    

    CurPtr = 0;
         
	while (CurPtr < FWRealLen) {
		if ((CurPtr+4) > FWRealLen) {
			// Reach the end of file.
			while (CurPtr < FWRealLen) {
                if(HAL_RTL_R8(WriteAddr)!=((u1Byte)*(pFWRealStart + CurPtr)))
    			{
        		    RT_DBG_FW_LOG("Verify download fail at [0x%x]",WriteAddr);
                    return _FALSE;
    			}
				WriteAddr++;
				CurPtr++;
			}
		} else {
			// Comapre Download code with original binary

            binTemp = *((pu4Byte)(pFWRealStart + CurPtr));

            //RT_DBG_FW_LOG("[Verify](pFWRealStart + CurPtr) =%x\n",(pFWRealStart + CurPtr));
			binTemp = HAL_cpu_to_le32(binTemp);
            binTemp = CPUTOLEX32(binTemp);
			ROMTemp = HAL_RTL_R32(WriteAddr);
            
			if(binTemp != ROMTemp)
			{
    		    RT_DBG_FW_LOG("Verify download fail at [0x%x] binTemp=%x,ROMTemp=%x",
                    WriteAddr,binTemp,ROMTemp);
           	    RT_DBG_FW_LOG("VerifyDownloadStatuse! 0x80=0x%x\n",HAL_RTL_R32(0x80));
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

                // TODO: Filen, it can be remove ??
				HAL_delay_ms(10);
			}
			CurPtr += 4;
		}
	}
    return _TRUE;
}


static BOOLEAN
LoadFirmware88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  u1Byte              downloadTarget
)
{
    pu1Byte     pFWRealStart;
    u4Byte      FWRealLen;
    u1Byte      u1ByteTmp;
    u1Byte      wait_cnt = 0;

    //4 1. Get fw start, end address
    
    if(downloadTarget== DOWNLOAD_ROM)
    {
        // rom   
        pFWRealStart = data_rtl8051_ROMafw_start;
        FWRealLen = (u4Byte)(data_rtl8051_ROMafw_end - data_rtl8051_ROMafw_start);       
    }
    else
    {
        // ram
    pFWRealStart = data_rtl8051_RAMafw_start;
        pFWRealStart += RT_FIRMWARE_HDR_SIZE;
    FWRealLen = (u4Byte)(data_rtl8051_RAMafw_end - data_rtl8051_RAMafw_start);
        FWRealLen -= RT_FIRMWARE_HDR_SIZE;        
    }

    DownloadInit();

    if(downloadTarget== DOWNLOAD_ROM)
    {
        // rom   
        HAL_RTL_W8(REG_8051FW_CTRL+2, HAL_RTL_R8(REG_8051FW_CTRL+2) | BIT3);
    	delay_ms(1);
    }
    else
    {
        HAL_RTL_W32(REG_8051FW_CTRL, HAL_RTL_R32(REG_8051FW_CTRL) & 0xfff0ffff);
    	delay_ms(1);       
    }

    WriteToFWSRAM88XX(Adapter, pFWRealStart, FWRealLen);


    // clear page number
    u1ByteTmp =  HAL_RTL_R8(REG_8051FW_CTRL+2);

    u1ByteTmp &= ~BIT0;
    u1ByteTmp &= ~BIT1;
    u1ByteTmp &= ~BIT2;    
    HAL_RTL_W8(REG_8051FW_CTRL+2, u1ByteTmp);
	delay_ms(1);


    if(VerifyDownloadStatus(Adapter,pFWRealStart,FWRealLen)==_TRUE)
    {
        RT_DBG_FW_LOG("download verify ok!\n");
    }
    else
    {
        RT_DBG_FW_LOG("download verify fail!\n");
    }
    

    if(downloadTarget == DOWNLOAD_ROM)
    {
        HAL_RTL_W8(REG_8051FW_CTRL+2, (HAL_RTL_R8(REG_8051FW_CTRL+2)& (~BIT3)));
        HAL_RTL_W8(REG_8051FW_CTRL, (HAL_RTL_R8(REG_8051FW_CTRL)& (~BIT0)));        
    }
    else    
    {
    u1ByteTmp = HAL_RTL_R8(REG_8051FW_CTRL);
        
    if ( u1ByteTmp & FWDL_CHKSUM_RPT ) {
             RT_DBG_FW_LOG("CheckSum Pass\n");        
    }
    else {
            RT_DBG_FW_LOG("CheckSum Failed\n");
            return _FALSE;                   
    }
    
    u1ByteTmp &= ~MCUFWDL_EN;
    u1ByteTmp |= MCUFWDL_RDY;
	HAL_RTL_W8(REG_8051FW_CTRL, u1ByteTmp);
    
	HAL_delay_ms(1);
    
	HAL_RTL_W8(REG_8051FW_CTRL+1, 0x00);

        // reset MCU
        HAL_RTL_W8(REG_SYS_FUNC_EN+1, HAL_RTL_R8(REG_SYS_FUNC_EN+1)&(~BIT2));
        HAL_delay_ms(1);    
        HAL_RTL_W8(REG_SYS_FUNC_EN+1, HAL_RTL_R8(REG_SYS_FUNC_EN+1) | BIT2);
        RT_DBG_FW_LOG("After download RAM reset MCU\n");

	// Check if firmware RAM Code is ready
        
        while (!(HAL_RTL_R8(REG_8051FW_CTRL) & RAM_DL_SEL)) {
        if (++wait_cnt > CHECK_FW_RAMCODE_READY_TIMES) {
                RT_DBG_FW_LOG("Check RAM code ready fail\n");
            return _FALSE;
		}
        
            RT_DBG_FW_LOG("Firmware is not ready, wait\n");
        HAL_delay_ms(CHECK_FW_RAMCODE_READY_DELAY_MS);
    }
    }
    return _TRUE;
}


RT_STATUS
InitFirmware88XX(
    IN  HAL_PADAPTER    Adapter,
    IN  u1Byte              downloadTarget
)
{
    u4Byte         dwnRetry    = DOWNLOAD_FIRMWARE_RETRY_TIMES;
    BOOLEAN     bfwStatus   = _FALSE;
     //delay_ms(3000);
    while(dwnRetry-- && !bfwStatus) {
        bfwStatus = LoadFirmware88XX(Adapter,downloadTarget);
    }

    if ( _TRUE == bfwStatus ) {
		
         RT_DBG_FW_LOG("[Congratulation] Download is Successful\n");        
    }
    else {
         RT_DBG_FW_LOG("[Warning]Download is Failed\n");                
        return RT_STATUS_FAILURE;
    }
    
    return RT_STATUS_SUCCESS;
}



RT_STATUS
CompareROM(
    IN  HAL_PADAPTER    Adapter
)
{
    pu1Byte     pFWRealStart;
    u4Byte      FWRealLen;
       
    // rom   
    pFWRealStart = data_rtl8051_ROMafw_start;
    FWRealLen = (u4Byte)(data_rtl8051_ROMafw_end - data_rtl8051_ROMafw_start);       

    DownloadInit();
        
    HAL_RTL_W8(REG_8051FW_CTRL+2, HAL_RTL_R8(REG_8051FW_CTRL+2) | BIT3);
    delay_ms(1);    
    
    if(VerifyDownloadStatus(Adapter,pFWRealStart,FWRealLen)==_TRUE)
    {
        RT_DBG_FW_LOG("Compare with FPGA ROM ok!\n");
    }
    else
    {
        RT_DBG_FW_LOG("Compare with FPGA ROM fail!\n");
    }

    return RT_STATUS_SUCCESS;
}

RT_STATUS
CompareRAM(
    IN  HAL_PADAPTER    Adapter
)
{
    pu1Byte     pFWRealStart;
    u4Byte      FWRealLen;
       
    // rom   
    pFWRealStart = data_rtl8051_RAMafw_start;
    FWRealLen = (u4Byte)(data_rtl8051_RAMafw_end - data_rtl8051_RAMafw_start);       

    DownloadInit();
        
    //HAL_RTL_W8(REG_8051FW_CTRL+2, HAL_RTL_R8(REG_8051FW_CTRL+2) | BIT3);
    delay_ms(1);    
    
    if(VerifyDownloadStatus(Adapter,pFWRealStart,FWRealLen)==_TRUE)
    {
        RT_DBG_FW_LOG("Compare with FPGA ROM ok!\n");
    }
    else
    {
        RT_DBG_FW_LOG("Compare with FPGA ROM fail!\n");
    }

    return RT_STATUS_SUCCESS;
}    

VOID
DMAtoTXPktBuf(
    u1Byte dest
)
{
    
    pu1Byte     pFWRealStart;
    u4Byte      FWRealLen;
    u1Byte      pPaylod[TXPKT_FW_SIZE];
    u2Byte value16 = 0;

    if(dest==0)          
    {
        
        HAL_RTL_W8(REG_RQPN,0x10);
        HAL_RTL_W8(REG_RQPN+1,0x10);
        HAL_RTL_W8(REG_RQPN+2,0x10);    
        HAL_RTL_W8(REG_TDECTRL+1,0x50);    

        pFWRealStart = data_rtl8051_RAMafw_start;
        pFWRealStart += RT_FIRMWARE_HDR_SIZE;
        FWRealLen = (u4Byte)(data_rtl8051_RAMafw_end - data_rtl8051_RAMafw_start);
        FWRealLen -= RT_FIRMWARE_HDR_SIZE; 
        RT_DBG_FW_LOG("RAM code len = %x\n",FWRealLen);      

        
        if(DmaPktForFw(FWRealLen,pFWRealStart))
        {
            RT_DBG_FW_LOG("Download Success\n");
        }

         // write 0x81 bit(12) driver download txpktbuf ready
        HAL_RTL_W8(0x81, 0x1F) ;
    }
    else if(dest==1)
    {
        // get TXBUFAddress
        pFWRealStart = data_rtl8051_TXBUFafw_start;
        pFWRealStart += RT_FIRMWARE_HDR_SIZE;    
        FWRealLen = (u4Byte)(data_rtl8051_TXBUFafw_end - data_rtl8051_TXBUFafw_start);       
        FWRealLen -= RT_FIRMWARE_HDR_SIZE; 
        
        RT_DBG_FW_LOG("TXPKT code len = %x\n",FWRealLen);

         RT_DBG_FW_LOG("Copy payload\n");
        memset(pPaylod,0x0,500);
        memcpy(pPaylod+216,pFWRealStart,FWRealLen);
        FWRealLen += 216;

    	value16 = (HAL_RTL_R8(REG_TDECTRL+1) & 0xff);
        value16 += 1;
    	value16 = (value16*2);
    	HAL_RTL_W16(0x86, value16) ;
        RT_DBG_FW_LOG("value16 = [%x] \n",value16);    
        
        if(DmaPktForFw(FWRealLen,(u1Byte *)&pPaylod))
        {
            RT_DBG_FW_LOG("Download Success\n");
        }
    }
}



VOID TXPKTBUF_WRITE(
    IN  HAL_PADAPTER    Adapter,
    IN  pu1Byte         pFWRealStart,
    IN  u4Byte          FWRealLen,
    IN  u2Byte          WriteAddr
    )
{
    u4Byte CurPtr;
    u1Byte     Temp;
    
        
    while (CurPtr < FWRealLen) 
    {
       Temp = *(pFWRealStart + CurPtr);
       TXBUF_IO_WRITE8(WriteAddr, (u1Byte)Temp);
       WriteAddr++;
       CurPtr++;

       if(WriteAddr == 0xffff)
       {
            RT_DBG_FW_LOG("Write to the TXPKTBUF boundary, Stop write! \n");
       }
    }

}

VOID TXPKTBUF_READ(
    IN  HAL_PADAPTER    Adapter,
    IN  pu1Byte         pFWRealStart,
    IN  u4Byte          FWRealLen,
    IN  u2Byte          WriteAddr
    )
{
    u4Byte CurPtr;
    u1Byte     Temp;
    
    while (CurPtr < FWRealLen) 
    {
       if(TXBUF_IO_READ8(WriteAddr)!= ((u1Byte)*(pFWRealStart + CurPtr)))
       {
          RT_DBG_FW_LOG("Verify download fail at [0x%x]",WriteAddr);
          return;
       }       
           
       WriteAddr++;
       CurPtr++;

       if(WriteAddr == 0xffff)
       {
            RT_DBG_FW_LOG("Read to the TXPKTBUF boundary, Stop Read ! \n");
       }
    }

}



VOID DirectIOToTXPKTBUF(u1Byte dest)
{
    pu1Byte     pFWRealStart;
    u4Byte      FWRealLen,CurPtr;
    TX_DESC     curDesc;
    u1Byte      pPaylod[TXPKT_FW_SIZE];
    int offset;
	int total_page, page;          
	u1Byte tmp[1] = {0x20};
    u1Byte bcn_page;
    u2Byte txbufAddr = 0;
    u2Byte value86 = 0;
    u2Byte value16 = 0;
    u2Byte pktLen=0;


    if(dest==0)
    {
        // get TXBUFAddress
        pFWRealStart = data_rtl8051_TXBUFafw_start;
        pFWRealStart += RT_FIRMWARE_HDR_SIZE;    
        FWRealLen = (u4Byte)(data_rtl8051_TXBUFafw_end - data_rtl8051_TXBUFafw_start );       
        FWRealLen -= RT_FIRMWARE_HDR_SIZE; 
        RT_DBG_FW_LOG("FWRealLen = %x\n",FWRealLen);

        bcn_page = HAL_RTL_R8(REG_TDECTRL+1);
        txbufAddr = bcn_page << 8;

        value86 = (HAL_RTL_R16(0x86) & 0x8000);
        RT_DBG_FW_LOG("value86 = [%x] \n",value86);    
    	value16 = (HAL_RTL_R8(REG_TDECTRL+1) & 0xff);
    	value16 = (value16*2);
    	value16 |= value86;
    	HAL_RTL_W16(0x86, value16) ;
        RT_DBG_FW_LOG("value16 = [%x] \n",value16);    
    }

    if(dest==1)
    {
        DownloadInit();

        pFWRealStart = data_rtl8051_RAMafw_start;
        pFWRealStart += RT_FIRMWARE_HDR_SIZE;    
        FWRealLen = (u4Byte)( data_rtl8051_RAMafw_end - data_rtl8051_RAMafw_start);       
        FWRealLen -= RT_FIRMWARE_HDR_SIZE; 
        RT_DBG_FW_LOG("FWRealLen = %x\n",FWRealLen);

        HAL_RTL_W8(REG_RQPN,0x10);
        HAL_RTL_W8(REG_RQPN+1,0x10);
        HAL_RTL_W8(REG_RQPN+2,0x10);    
        HAL_RTL_W8(REG_TDECTRL+1,0x50);    

        bcn_page = HAL_RTL_R8(REG_TDECTRL+1);
        txbufAddr = bcn_page << 8;

        // write pkt length in TXDESC
        TXBUF_IO_WRITE8(txbufAddr, (u2Byte)(FWRealLen));    
        TXBUF_IO_WRITE8(txbufAddr+1, (u2Byte)((FWRealLen>>8)&0x7f));
        RT_DBG_FW_LOG("pkt[0] = %x\n",TXBUF_IO_READ8(txbufAddr));
        RT_DBG_FW_LOG("pkt[1] = %x\n",TXBUF_IO_READ8(txbufAddr+1));
        
        txbufAddr += 40;
        RT_DBG_FW_LOG("bcn_page = %x\n",bcn_page);
        RT_DBG_FW_LOG("txbufAddr = %x\n",txbufAddr);

    }

    RT_DBG_FW_LOG("TXPKTBUF WRITE! \n");
                    
    TXPKTBUF_WRITE(NULL,pFWRealStart,FWRealLen,txbufAddr);

    RT_DBG_FW_LOG("TXPKTBUF WRITE END! \n");

    RT_DBG_FW_LOG("TXPKTBUF READ! \n");
    TXPKTBUF_READ(NULL,pFWRealStart,FWRealLen,txbufAddr);
    RT_DBG_FW_LOG("TXPKTBUF READ END! \n");

    if(dest==1)
    {
        // write 0x81 bit(12) driver download txpktbuf ready
        HAL_RTL_W8(0x81, 0x1F) ;
    }
}

VOID TXPKTBUF_DownLoad_pattern(VOID)
{
    u1Byte pattern[256];
    u2Byte i;
    u2Byte txbuf_addr;
    static u16  count=0;

    RT_DBG_FW_LOG("Fill pattern! \n");
    // gen download pattern
    for(i=0;i<256;i++)
    {
        pattern[i]=(i+count);
       // RT_DBG_FW_LOG("pattern[%x] = %x\n",pattern[i],i);
    }

    RT_DBG_FW_LOG("Start Write pattern \n");

    // write to txpktbuf 
    for(txbuf_addr=0x0;txbuf_addr<0x1fff;txbuf_addr++)
    {
        if(txbuf_addr==0x0)
        {
            RT_DBG_FW_LOG("Host Write txbuf addr =[%x] pattern = [%x] \n",txbuf_addr, pattern[(u1Byte)txbuf_addr]);        
        }
        TXBUF_IO_WRITE8(txbuf_addr, pattern[(u1Byte)txbuf_addr]);
    }
   count++;
   
   RT_DBG_FW_LOG("Download TX pattern end \n");

}

u1Byte TXPKTBUF_verify_pattern(u1Byte bReset)
{
    u1Byte pattern[256];
    u2Byte i;
    u2Byte txbuf_addr;
    static u2Byte  count=0;

    RT_DBG_FW_LOG("Fill pattern! \n");
    // gen download pattern
    if(!bReset)
   {
       
       for(i=0;i<256;i++)
       {
            pattern[i]=(i+count);
            // RT_DBG_FW_LOG("pattern[%x] = %x\n",pattern[i],i);
       }
   }
    else
    {
        for(i=0;i<256;i++)
        {
            pattern[i]=i;
           // RT_DBG_FW_LOG("pattern[%x] = %x\n",pattern[i],i);
        }
    }
    
    RT_DBG_FW_LOG("Start Verify pattern \n");

    // write to txpktbuf 
    for(txbuf_addr=0x0;txbuf_addr<0x1fff;txbuf_addr++)
    {
        if(TXBUF_IO_READ8(txbuf_addr)!=pattern[(u1Byte)txbuf_addr])
        {
            RT_DBG_FW_LOG("txbuf error =[%x] pattern = [%x] \n",txbuf_addr, pattern[(u1Byte)txbuf_addr]);        
            return 0;
        }

        if(txbuf_addr==0x0)
        {
            RT_DBG_FW_LOG("TXBUF_IO_READ8(txbuf_addr) =[%x] pattern[(u1Byte)txbuf_addr] = [%x] \n",TXBUF_IO_READ8(txbuf_addr),pattern[(u1Byte)txbuf_addr]);

        }
    }

   count++;

   RT_DBG_FW_LOG("Verify TX pattern end \n");
   return 1;
}

void reset_all_pattern()
{
    u32 addr;
    for(addr=0x0;addr<0x1fff;addr++)
    {
       TXBUF_IO_WRITE8(addr, 0);           
    }

    for(addr=0x0;addr<0x4000;addr++)
    {
       RXBUF_IO_WRITE8(addr, 0); 
    }

    WL_IO_WRITE8(0x8204,0x0);
    WL_IO_WRITE32(0x800,0x0);
}

u1Byte RXPKTBUF_Verify_pattern(u1Byte bReset)
{
    u1Byte pattern[256];
    u2Byte i;
    u2Byte rxbuf_addr;
    static u2Byte  count=0;

    RT_DBG_FW_LOG("Fill pattern! \n");
    // gen download pattern
   if(!bReset)
   {
       
       for(i=0;i<256;i++)
       {
            pattern[i]=(i+count);
            // RT_DBG_FW_LOG("pattern[%x] = %x\n",pattern[i],i);
       }
   }
   else
   {
        for(i=0;i<256;i++)
        {
            pattern[i]=i;
           // RT_DBG_FW_LOG("pattern[%x] = %x\n",pattern[i],i);
        }
   }
   
    RT_DBG_FW_LOG("Start Verify pattern \n");

    // write to txpktbuf 
    for(rxbuf_addr=0x0;rxbuf_addr<0x4000;rxbuf_addr++)
    {
        if(RXBUF_IO_READ8(rxbuf_addr)!=pattern[(u1Byte)rxbuf_addr])
        {
            RT_DBG_FW_LOG("rxbuf error =[%x] pattern = [%x] \n",rxbuf_addr, pattern[(u1Byte)rxbuf_addr]);        
            return 0;
        }

        
        if(rxbuf_addr==0x0)
          {
              RT_DBG_FW_LOG("RXBUF_IO_READ8(rxbuf_addr) =[%x] pattern[(u1Byte)rxbuf_addr] = [%x] \n",RXBUF_IO_READ8(rxbuf_addr),pattern[(u1Byte)rxbuf_addr]);
        
          }
    }

   
  count++;

   RT_DBG_FW_LOG("Verify RX pattern end \n");
   return 1;
}


VOID RXPKTBUF_DownLoad_pattern(VOID)
{
    u1Byte pattern[256];
    u2Byte i;
    u2Byte rxbuf_addr;
    static u16  count=0;

    RT_DBG_FW_LOG("Fill pattern! \n");
    // gen download pattern
    for(i=0;i<256;i++)
    {
        pattern[i]=(i+count);
       // RT_DBG_FW_LOG("pattern[%x] = %x\n",pattern[i],i);
    }


    RT_DBG_FW_LOG("Start Write pattern \n");

    // write to txpktbuf 
    for(rxbuf_addr=0x0;rxbuf_addr<0x4000;rxbuf_addr++)
    {
        if(rxbuf_addr==0x0)
        {
            RT_DBG_FW_LOG("Host Write rxbuf addr =[%x] pattern = [%x] \n",rxbuf_addr, pattern[(u1Byte)rxbuf_addr]);        
        }
     //   RT_DBG_FW_LOG("txbuf addr =[%x] pattern = [%x] \n",txbuf_addr, pattern[(u1Byte)txbuf_addr]);        
        RXBUF_IO_WRITE8(rxbuf_addr, pattern[(u1Byte)rxbuf_addr]);
    }
    count++;
   
    RT_DBG_FW_LOG("Download pattern end \n");

}




VOID TXRPTBUF_DownLoad_pattern(VOID)
{
    u1Byte  TXRPT[MACID_NUM][TXRPT_SIZE];
    static u8  count=0;    

    HAL_RTL_W8(TXRPT_START_ADDR+IOPATH_TXRPT_ENTRY*TXRPT_SIZE+IOPATH_TXRPT_OFFSET,count);
    RT_DBG_FW_LOG("Download to %x vaule =%x\n",TXRPT_START_ADDR+IOPATH_TXRPT_ENTRY*TXRPT_SIZE+IOPATH_TXRPT_OFFSET,count);
    count++;
}

u1Byte TXRPTBUF_Verify_pattern(u1Byte bReset)
{
    u1Byte  TXRPT[MACID_NUM][TXRPT_SIZE];
    static u8  count=0;    

    if(HAL_RTL_R8(TXRPT_START_ADDR+IOPATH_TXRPT_ENTRY*TXRPT_SIZE+IOPATH_TXRPT_OFFSET)!= (u8)count)
    {
        RT_DBG_FW_LOG("txrpt error to %x \n",HAL_RTL_R8(TXRPT_START_ADDR+IOPATH_TXRPT_ENTRY*TXRPT_SIZE+IOPATH_TXRPT_OFFSET));
        return 0;
    }
    else
    {
        RT_DBG_FW_LOG("TXRTP Verify OK, addr[%x]=%x \n",TXRPT_START_ADDR+IOPATH_TXRPT_ENTRY*TXRPT_SIZE+IOPATH_TXRPT_OFFSET,HAL_RTL_R8(TXRPT_START_ADDR+IOPATH_TXRPT_ENTRY*TXRPT_SIZE+IOPATH_TXRPT_OFFSET));

        if(!bReset)
        {
            count++;
        }
        return 1;
    }

}

VOID genH2Ccmd(u1Byte index)
{
    u1Byte ext_cmd[4] = {0xdd,0xcc,0xbb,0xaa};
    u1Byte cmd[4] = {0x33,0x22,0x11,0xe0};


    RT_DBG_FW_LOG("generate HC2_C2H loopback cmd\n");
    RT_DBG_FW_LOG("index =%x ext = %x cmd =%x\n",index,*((u4Byte*)(ext_cmd)),*((u4Byte*)(cmd)));    
    HAL_RTL_W32(REG_HMEBOX_4+index*4,*((u4Byte*)(ext_cmd)));   
    HAL_RTL_W32(REG_HMEBOX_0+index*4,*((u4Byte*)(cmd)));

}



