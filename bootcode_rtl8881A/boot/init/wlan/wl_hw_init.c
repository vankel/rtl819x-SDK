enum { __FILE_NUM__= 2 };


#include <linux/types.h>
#include <linux/interrupt.h>
#include <rtl_types.h>
#include "wl_hw_init.h"


u1Byte Global_CPWM=0;
u1Byte Global_RPWM=0;

struct wl_private
{
	u4Byte nr;
};

extern BOOLEAN 
WLIsrReceivePacket(
    VOID
);

extern int 
request_IRQ(
    IN  unsigned long       irq,
    IN  struct irqaction    *action, 
    IN  VOID*               dev_id
);

VOID 
WLInterrupt(
    IN  int             irq, 
    IN  VOID            *dev_id, 
    IN  struct pt_regs  *regs
);


static struct wl_private WL[2];
static u4Byte WLMAC_IRQ=29;
static struct irqaction irq_wlmac29 = {WLInterrupt, 0, 29,"wl", NULL, NULL};  
u4Byte BcnOkCounter = 0;
u4Byte TxbufCheckIndex = 0;
u4Byte TxbufRandomIndex = 0;
u4Byte RxTimeStamp = 0;


VOID 
WLInterrupt(
    IN  int             irq, 
    IN  VOID            *dev_id, 
    IN  struct pt_regs  *regs
)
{
    u4Byte Value32 = 0;
    u4Byte Hisr = 0;
    u2Byte WritePoint, ReadPoint;

    Hisr =  HAL_RTL_R32(REG_HISR0);
    //dprintf("WL ISR Status: 0x%x\n", Hisr);
    HAL_RTL_W32(REG_HISR0,Hisr);

    if (Hisr & BIT(0)) {
        u4Byte Check1Value32,Check2Value32;
            
        Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
        WritePoint = (u2Byte)(Value32 >> 16);
        ReadPoint = (u2Byte)(Value32 & 0xFFFF);
//        dprintf("+==> E RX wp: %d; rp: %d\n", WritePoint, ReadPoint);

        WLIsrReceivePacket();

        RxTimeStamp = HAL_RTL_R32(0x560);

        TxbufCheckIndex++;
#if 0
        switch(TxbufCheckIndex & 0x3) {
            case 0:
                //dprintf("V1\n");
               (*(volatile unsigned int *) (0xB86500E0)) = (0x5A5A5A5A);
               (*(volatile unsigned int *) (0xB86500F0)) = (0x5A5A5A5A);

                Check1Value32 = (*(volatile unsigned int *) (0xB86500E0));
                Check2Value32 = (*(volatile unsigned int *) (0xB86500F0));

                if (Check1Value32 != 0x5A5A5A5A) {
                    dprintf("Check1Value32 IO TXBfu Error 0!!!!\n");                
                }
                if (Check1Value32 != 0x5A5A5A5A) {
                    dprintf("Check2Value32 IO TXBfu Error 0!!!!\n");                                    
                }
                
                break;            
            case 1:
                //dprintf("V2\n");
               (*(volatile unsigned int *) (0xB86500E0)) = (0xFFFFFFFF);
               (*(volatile unsigned int *) (0xB86500F0)) = (0xFFFFFFFF);

                Check1Value32 = (*(volatile unsigned int *) (0xB86500E0));
                Check2Value32 = (*(volatile unsigned int *) (0xB86500F0));                

                if (Check1Value32 != 0xFFFFFFFF) {
                    dprintf("Check1Value32 IO TXBfu Error 1!!!!\n");                
                }
                if (Check1Value32 != 0xFFFFFFFF) {
                    dprintf("Check2Value32 IO TXBfu Error 1!!!!\n");                                    
                }                
                break;
            case 2:
                //dprintf("V3\n");                
                (*(volatile unsigned int *) (0xB86500E0)) = (0);
                (*(volatile unsigned int *) (0xB86500F0)) = (0);
                
                 Check1Value32 = (*(volatile unsigned int *) (0xB86500E0));
                 Check2Value32 = (*(volatile unsigned int *) (0xB86500F0));  


                if (Check1Value32 != 0) {
                    dprintf("Check1Value32 IO TXBfu Error 2!!!!\n");                
                }
                if (Check1Value32 != 0) {
                    dprintf("Check2Value32 IO TXBfu Error 2!!!!\n");                                    
                }                
                
                break;
            case 3:
                //dprintf("V4\n");
                TxbufRandomIndex++;
                (*(volatile unsigned int *) (0xB86500E0)) = (TxbufRandomIndex);

                 Check1Value32 = (*(volatile unsigned int *) (0xB86500E0));
       
                if (Check1Value32 != TxbufRandomIndex) {
                    dprintf("Check1Value32 IO TXBfu Error 3!!!!\n");                
                }
                break;
            default:
                
                break;
        }

#endif
        Value32 = HAL_RTL_R32(REG_RXQ_RXBD_IDX);
        WritePoint = (u2Byte)(Value32 >> 16);
        ReadPoint = (u2Byte)(Value32 & 0xFFFF);
//        dprintf("<==+ A RX wp: %d; rp: %d\n", WritePoint, ReadPoint);
    }

    // blow is for page0 interrupt
    //dprintf("REG[B4]=%x\n",Hisr);

    // check CPWM ISR
    if(Hisr & BIT_CPWM_INT) {
        // write one clear
        HAL_RTL_W32(REG_HISR0, Value32 | BIT_CPWM_INT);
        
        
        Global_CPWM = HAL_RTL_R8(REG_CPWM);

        if((Global_CPWM & 0x7F) != (Global_RPWM & 0x7F)) {
            dprintf("loopback fail RPWM =%x, CPWM =%x\n",Global_RPWM,Global_CPWM);    
        }
        else {
            dprintf("vaule=[%x] CPWM is equal RPWM!\n",Global_RPWM);
        }
        
        Global_RPWM++;
        Global_CPWM++;
        
        if (HAL_RTL_R8(REG_PCIE_HRPWM1_V1) & PS_TOGGLE) {            
            HAL_RTL_W8(REG_PCIE_HRPWM1_V1, Global_RPWM & (~PS_TOGGLE));
        }
        else {            
            HAL_RTL_W8(REG_PCIE_HRPWM1_V1, (Global_RPWM | PS_TOGGLE));
        }

    }


    if (Hisr & BIT(26)) {
        dprintf("Transmit Beacon Error!!!\n");
        Value32 = HAL_RTL_R32(REG_HIMR0);
        Value32 &= (~HIMR_TBDER); 
        dprintf("Disable Beacon Error IMR value: 0x%x\n",Value32);    
        HAL_RTL_W32(REG_HIMR0,Value32);
    }

    if (Hisr & BIT(25)) {
        dprintf("Transmit Beacon ok!!!\n");
        BcnOkCounter++;
        if (BcnOkCounter > 10) {
            Value32 = HAL_RTL_R32(REG_HIMR0);
            Value32 &= (~HIMR_TBDOK); 
            dprintf("Disable Beacon Ok IMR value: 0x%x\n",Value32);    
            HAL_RTL_W32(REG_HIMR0,Value32);
        }
    }

    if (Hisr & BIT(20)) {
        dprintf("Beacon Early ISR\n");
        if (BcnOkCounter > 10) {
            Value32 = HAL_RTL_R32(REG_HIMR0);
            Value32 &= (~HIMR_BcnInt); 
            dprintf("Disable Beacon Early IMR value: 0x%x\n",Value32);    
            HAL_RTL_W32(REG_HIMR0,Value32);        
        }
    }


    if(Hisr & BIT_C2HCMD_INT) {
        dprintf("Get C2H interrupt!!");
         // write one clear
        HAL_RTL_W32(REG_HISR0, Hisr | BIT_C2HCMD_INT);
        dprintf("0x1a0=[%x] \n",HAL_RTL_R32(0x1a0));
        dprintf("0x1a4=[%x] \n",HAL_RTL_R32(0x1a4));    
        HAL_RTL_W8(0x1af,0x0);
    }
    
}


VOID 
WLIsrinit(
    VOID
)
{    
        /* Set interrupt routing register */
        u4Byte Value32;

        //Set IRR1_REG
        (*(volatile u4Byte *)(0xb8003014)) |= (3<<20); 
        request_IRQ(WLMAC_IRQ, &irq_wlmac29,&(WL[0]));
    
        Value32 = HAL_RTL_R32(REG_HIMR0);
//        Value32 |= HIMR_ROK | HIMR_BcnInt |HIMR_TBDOK | HIMR_TBDER | HIMR_CPWM | HIMR_C2HCMD; 
        Value32 |= HIMR_ROK |HIMR_TBDOK | HIMR_TBDER | HIMR_CPWM | HIMR_C2HCMD; 
//          Value32 |= HIMR_TBDOK | HIMR_TBDER | HIMR_CPWM | HIMR_C2HCMD; 

        dprintf("Set WL DMA IMR value: 0x%x\n",Value32);
    
        HAL_RTL_W32(REG_HIMR0,Value32);


        Value32 = HAL_RTL_R32(0x550);
        Value32 |= 0x8;
        HAL_RTL_W32(0x550,Value32);

        

        
}


VOID 
WLHWRinit(
    VOID
)
{
    HALWLMacInit = InitMACHAL8881A;
    HALWLPwrInit = InitPowerHAL8881A;
    HALWLApModePwrInit = InitMACToAPHAL8881A;
    HALWLHCIDmaInit = InitHCIDmaHAL8881A;
    HALWLTxDesxGenHandler = TXDescGenHAL8881A;
    HALWLNormallCfg = InitCfgNormallHAL8881A;
    HALWLSwBcnCfg = InitCfgSwBcnHAL8881A;
    HALWLDmaLBCfg = InitCfgDmaLBHAL8881A;
    HALWLInInitMbssid = InitMBSSID8881A;
    HALWLInStopMbssid = StopMBSSID8881A;

}

