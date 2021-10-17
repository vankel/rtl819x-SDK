#ifndef _WL_LOG_H_
#define _WL_LOG_H_
#include "wl_type.h"

u4Byte 
rtl_printf(
    IN  u2Byte FileNum,
    IN  u4Byte Line,
    IN  u1Byte *fmt, ...
);

//#define _WL_DMA_DBG_EN_ 
//#define _WL_TX_DBG_EN_
//#define _WL_RX_DBG_EN_
//#define _WL_CHECK_RX_DBG_EN_
//#define _WL_EX_DBG_EN_
#define _WL_FW_DBG_EN_
//#define _WL_TX_BCN_DBG_EN_
//#define _WL_MP_DBG_EN_

#ifdef _WL_DMA_DBG_EN_
#define RTL_DMA_DBG_LOG(...)    rtl_printf(__FILE_NUM__,__LINE__,__VA_ARGS__)
#else
#define RTL_DMA_DBG_LOG(...)
#endif

#ifdef _WL_EX_DBG_EN_
#define RTL_EX_DBG_LOG(...)    rtl_printf(__FILE_NUM__,__LINE__, __VA_ARGS__)
#else
#define RTL_EX_DBG_LOG(...)
#endif


#ifdef _WL_TX_DBG_EN_
#define RTL_TX_DBG_LOG(...)    rtl_printf(__FILE_NUM__,__LINE__, __VA_ARGS__)
#else
#define RTL_TX_DBG_LOG(...)
#endif

#ifdef _WL_RX_DBG_EN_
#define RTL_RX_DBG_LOG(...)    rtl_printf(__FILE_NUM__,__LINE__, __VA_ARGS__)
#else
#define RTL_RX_DBG_LOG(...)
#endif

#ifdef _WL_TX_BCN_DBG_EN_
#define RTL_TX_BCN_DBG_LOG(...)    rtl_printf(__FILE_NUM__,__LINE__, __VA_ARGS__)
#else
#define RTL_TX_BCN_DBG_LOG(...)
#endif

#ifdef _WL_CHECK_RX_DBG_EN_
#define RTL_RX_CHECK_DBG_LOG(...)    rtl_printf(__FILE_NUM__,__LINE__, __VA_ARGS__)
#else
#define RTL_RX_CHECK_DBG_LOG(...)
#endif

#ifdef _WL_FW_DBG_EN_
#define RT_DBG_LOG(...) do {dprintf(__VA_ARGS__);}while(0)
#define RT_DBG_FW_LOG(...) do {dprintf(__VA_ARGS__);}while(0)
#else
#define RT_DBG_LOG(fmt,...)
#define RT_DBG_FW_LOG(fmt,...)
#endif

#ifdef _WL_MP_DBG_EN_
#define RTL_MP_DBG_LOG(...) do {dprintf(__VA_ARGS__);}while(0)
#else
#define RTL_MP_DBG_LOG(fmt,...)
#endif


#endif
