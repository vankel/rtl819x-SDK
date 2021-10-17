#ifndef _APOLLO_RAW_HWMISC_H_
#define _APOLLO_RAW_HWMISC_H_

/****************************************************************/
/* Header File inclusion                                        */
/****************************************************************/
#include <dal/apollo/raw/apollo_raw.h>
#include <rtk/debug.h>

/****************************************************************/
/* Constant Definition                                          */
/****************************************************************/
#define APOLLO_VIR_MAC_DUMY_CYCLE  30

#define APOLLO_VIR_MAC_TX_MAX_CNT  2000


/****************************************************************/
/* Type Definition                                              */
/****************************************************************/
extern int32 apollo_raw_virtualMacMode_Set(rtk_port_t port, rtk_enable_t enable);
extern int32 apollo_raw_virtualMac_Input(rtk_port_t fromPort, uint8 *pPkt, uint32 len);
extern int32 apollo_raw_virtualMac_Output(rtk_port_t toPort, uint8 *pPkt, uint32 *plen);
extern int32 apollo_raw_hsbData_get(rtk_hsb_t *hsbData);
extern int32 apollo_raw_hsaData_get(rtk_hsa_t *hsaData);
extern int32 apollo_raw_hsdData_get(rtk_hsa_debug_t *hsdData);

#endif /*#ifndef _APOLLO_RAW_HWMISC_H_*/

