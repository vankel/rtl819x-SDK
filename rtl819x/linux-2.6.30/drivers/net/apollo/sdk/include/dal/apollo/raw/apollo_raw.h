#ifndef _APOLLO_RAW_H_
#define _APOLLO_RAW_H_

#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_bitop.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <osal/memory.h>
#include <hal/chipdef/allmem.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/apollo/apollo_table_struct.h>
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollo/apollo_def.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/apollo/apollo_def.h>

#define APOLLO_VIDMAX                       0xFFF
#define APOLLO_EVIDMAX                      0x1FFF
#define APOLLO_CVIDXNO                      32
#define APOLLO_CVIDXMAX                     (APOLLO_CVIDXNO-1)

#define APOLLO_PRIMAX                       7
#define APOLLO_DSCPMAX    				    63
#define APOLLO_PRI_WEIGHT_MAX    		    15
#define APOLLO_L4PORT_MAX                   0xFFFF

#define APOLLO_PORTNO                       7
#define APOLLO_PORTIDMAX                    (APOLLO_PORTNO-1)
#define APOLLO_PMSKMAX                      ((1<<(APOLLO_PORTNO))-1)
#define APOLLO_PORTMASK                     0x7F

#define APOLLO_VCPORTNO                     16
#define APOLLO_EXTPORTNO                    5
#define APOLLO_VCPORTNOMAX                  (APOLLO_VCPORTNO-1)
#define APOLLO_EXTPORTNOMAX                 (APOLLO_EXTPORTNO-1)



#define APOLLO_PHYNO                        5
#define APOLLO_PHYIDMAX                     (APOLLO_PHYNO-1)
#define APOLLO_PHYREGMAX                    31
#define APOLLO_PHYDATAMAX                   0xFFFF

#define APOLLO_SVIDXNO                      64
#define APOLLO_SVIDXMAX                     (APOLLO_SVIDXNO-1)
#define APOLLO_MSTIMAX                      15

#define APOLLO_PRI_TO_QUEUE_TBL_SIZE        4
#define APOLLO_QUEUENO                      8
#define APOLLO_QIDMAX                       (APOLLO_QUEUENO-1)

#define APOLLO_PHY_BUSY_CHECK_COUNTER       1000

#define APOLLO_QOS_GRANULARTY_MAX           0x1FFFF
#define APOLLO_QOS_GRANULARTY_LSB_MASK      0xFFFF
#define APOLLO_QOS_GRANULARTY_LSB_OFFSET    0
#define APOLLO_QOS_GRANULARTY_MSB_MASK      0x10000
#define APOLLO_QOS_GRANULARTY_MSB_OFFSET    16

#define APOLLO_QOS_GRANULARTY_UNIT_KBPS     8

#define APOLLO_QOS_RATE_INPUT_MAX           (0x1FFFF * 8)
#define APOLLO_QOS_RATE_INPUT_MIN           8

#define APOLLO_QUEUE_MASK                   0xFF

#define APOLLO_EFIDMAX                      0x7
#define APOLLO_FIDMAX                       0xF

#define APOLLO_GPIOPINNO                    14
#define APOLLO_GPIOPINMAX                   (APOLLO_GPIOPINNO-1)

#define APOLLO_DSLVCNO                    	16
#define APOLLO_DSLVCMAX                   	(APOLLO_DSLVCNO -1)

#define APOLLO_EXTPORTNO                    5
#define APOLLO_EXTPORTMAX               	(APOLLO_EXTPORTNO -1)

#define APOLLO_ETHTYPEMAX                   0xFFFF

/*L34*/
#define APOLLO_PPPOE_ENTRY                  8
#define APOLLO_PPPOE_MAX_SESSION            0xFFFF

#define APOLLO_IPMC_TABLE_ENTRY             64
#define APOLLO_IPMC_TABLE_IDX_MAX           (APOLLO_IPMC_TABLE_ENTRY - 1)

/*EPON*/
#define APOLLO_LLID_MAX                  	8

/*MIB*/
#define APOLLO_ACL_MIB_MAX                  31
#define APOLLO_MIB_LATCH_TIMER_MAX          0xFF

#define APOLLO_LUT_4WAY_NO                  2048
#define APOLLO_LUT_CAM_NO                   64
#define APOLLO_LUT_TBL_NO                   (APOLLO_LUT_4WAY_NO + APOLLO_LUT_CAM_NO) /*2K 4 ways hash entries + 64 CAM entries*/
#define APOLLO_LUT_TBL_MAX                  (APOLLO_LUT_TBL_NO - 1)
#define APOLLO_CF_PATTERN_MAX               511

/*GPON*/
#define APOLLO_SID_MAX                  	127

#define APOLLO_FRAME_LENGTH_MAX   0x3FFF

/*L34*/
#define L34_ARP_TABLE_MAX_IDX       (512)
#define L34_EXTIP_TABLE_MAX_IDX     (16)
#define L34_ROUTING_TABLE_MAX_IDX   (8)
#define L34_NAPT_TABLE_MAX_IDX      (2048)
#define L34_NAPTR_TABLE_MAX_IDX     (1024)
#define L34_NH_TABLE_MAX_IDX        (16)
#define L34_PPPOE_TABLE_MAX_IDX     (8)
#define L34_NETIF_TABLE_MAX_IDX     (8)

#define L34_ROUTING_IP_MAX_MASK     (31)

/*Flow Control*/
#define APOLLO_PON_QUEUE_MAX                  	127
#define APOLLO_PON_QUEUE_NO                       128
#define APOLLO_PON_QUEUE_INDEX_MAX                     7
#define APOLLO_PON_QUEUE_INDEX_NO                       8




typedef uint32  apollo_raw_svidx_t;         /* SVLAN member configuration index*/
typedef uint32  apollo_raw_ethertype_t;     /* Ethertype */


typedef enum apollo_raw_linkSpeed_e
{
    LINKSPEED_10M  = 0,
    LINKSPEED_100M,
    LINKSPEED_GIGA
} apollo_raw_linkSpeed_t;


typedef uint32  apollo_raw_pmsk_t;

#endif /*#ifndef _APOLLO_RAW_H_*/

