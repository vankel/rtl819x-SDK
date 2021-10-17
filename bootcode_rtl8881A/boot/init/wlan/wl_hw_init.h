#ifndef _WL_HW_INIT_H_
#define _WL_HW_INIT_H_

#include "wl_8881.h"
#include <asm/types.h>
#include "wl_log.h"


typedef u4Byte (*HAL_WL_MAC_INIT) (VOID);
typedef u4Byte (*HAL_WL_PWR_INIT) (VOID);
typedef u4Byte (*HAL_WL_AP_Mode_PWR_INIT) (VOID);
typedef VOID (*HAL_WL_HCIDMA_INIT) (VOID);

typedef u4Byte (*HAL_WL_TX_DESC_GEN_HANDLER)(
                                                u4Byte          PayloadLen,
                                                u4Byte          Queue,
                                                u1Byte          IsBcn,
                                                u1Byte          IsDlForFw,
                                                PORT_INFO       *PortInfo,
                                                TX_DESC         *curDesc,
                                                TX_EXTEND_DESC  *curExtendDesc );
typedef u4Byte (*HAL_WL_SW_BCN_CFG) (VOID);
typedef u4Byte (*HAL_WL_NORMALL_CFG) (VOID);
typedef u4Byte (*HAL_WL_DMA_LB_CFG) (VOID);
typedef VOID (*HAL_WL_INIT_MBSSID) (u1Byte IsRootInterface,u1Byte IsVapInterface,u1Byte vap_init_seq);
typedef VOID (*HAL_WL_STOP_MBSSID) (u1Byte IsRootInterface,u1Byte IsVapInterface,u1Byte vap_init_seq);


HAL_WL_MAC_INIT                 HALWLMacInit;
HAL_WL_PWR_INIT                 HALWLPwrInit;
HAL_WL_AP_Mode_PWR_INIT         HALWLApModePwrInit;
HAL_WL_HCIDMA_INIT              HALWLHCIDmaInit;
HAL_WL_TX_DESC_GEN_HANDLER      HALWLTxDesxGenHandler;
HAL_WL_NORMALL_CFG              HALWLNormallCfg;
HAL_WL_SW_BCN_CFG               HALWLSwBcnCfg;
HAL_WL_DMA_LB_CFG               HALWLDmaLBCfg;
HAL_WL_INIT_MBSSID              HALWLInInitMbssid;
HAL_WL_STOP_MBSSID              HALWLInStopMbssid;


VOID 
WLHWRinit(
    VOID
);

VOID 
WLIsrinit(
    VOID
);


#endif //#ifndef _WL_HW_H_

