/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 11766 $
 * $Date: 2010-08-05 17:45:25 +0800 (星期四, 05 八月 2010) $
 *
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */

#ifndef __RTK_STAT_H__
#define __RTK_STAT_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */

/* global statistic counter index */
typedef enum rtk_stat_global_type_e
{
    DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX = 0,
    MIB_GLOBAL_CNTR_END
}rtk_stat_global_type_t;

/* port statistic counter index */
typedef enum rtk_stat_port_type_e
{
    IF_IN_OCTETS_INDEX = 0,                         /* RFC 2863 ifEntry */
    IF_IN_UCAST_PKTS_INDEX,                         /* RFC 2863 ifEntry */
    IF_IN_MULTICAST_PKTS_INDEX,                     /* RFC 2863 ifEntry */
    IF_IN_BROADCAST_PKTS_INDEX,                     /* RFC 2863 ifEntry */
    IF_IN_DISCARDS_INDEX,                           /* RFC 2863 ifEntry */
    IF_OUT_OCTETS_INDEX,                            /* RFC 2863 ifEntry */
    IF_OUT_DISCARDS_INDEX,                          /* RFC 2863 ifEntry */
    IF_OUT_UCAST_PKTS_CNT_INDEX,                    /* RFC 2863 IfXEntry */
    IF_OUT_MULTICAST_PKTS_CNT_INDEX,                /* RFC 2863 IfXEntry */
    IF_OUT_BROADCAST_PKTS_CNT_INDEX,                /* RFC 2863 IfXEntry */
    DOT1D_BASE_PORT_DELAY_EXCEEDED_DISCARDS_INDEX,  /* RFC 1493 Dot1dBasePortEntry */
    DOT1D_TP_PORT_IN_DISCARDS_INDEX,                /* RFC 1493 */
    DOT1D_TP_HC_PORT_IN_DISCARDS_INDEX,             /* RFC 2674 Dot1dTpHCPortEntry */
    DOT3_IN_PAUSE_FRAMES_INDEX,                     /* RFC 2665 Dot3PauseEntry */
    DOT3_OUT_PAUSE_FRAMES_INDEX,                    /* RFC 2665 Dot3PauseEntry */
    DOT3_OUT_PAUSE_ON_FRAMES_INDEX,                 /* Proprietary counter */
    DOT3_STATS_ALIGNMENT_ERRORS_INDEX,              /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_FCS_ERRORS_INDEX,                    /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX,       /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX,     /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX,        /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_LATE_COLLISIONS_INDEX,               /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX,          /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_FRAME_TOO_LONGS_INDEX,               /* RFC 2665 Dot3StatsEntry */
    DOT3_STATS_SYMBOL_ERRORS_INDEX,                 /* RFC 2665 Dot3StatsEntry */
    DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX,          /* RFC 2665 Dot3ControlEntry */
    ETHER_STATS_DROP_EVENTS_INDEX,                  /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_OCTETS_INDEX,                       /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_BROADCAST_PKTS_INDEX,               /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_MULTICAST_PKTS_INDEX,               /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_UNDER_SIZE_PKTS_INDEX,              /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_OVERSIZE_PKTS_INDEX,                /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_FRAGMENTS_INDEX,                    /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_JABBERS_INDEX,                      /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_COLLISIONS_INDEX,                   /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_CRC_ALIGN_ERRORS_INDEX,             /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_PKTS_64OCTETS_INDEX,                /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_PKTS_65TO127OCTETS_INDEX,           /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_PKTS_128TO255OCTETS_INDEX,          /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_PKTS_256TO511OCTETS_INDEX,          /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_PKTS_512TO1023OCTETS_INDEX,         /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX,        /* RFC 2819 EtherStatsEntry */
    ETHER_STATS_TX_OCTETS_INDEX,                    /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX,           /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_OVERSIZE_PKTS_INDEX,             /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_PKTS_64OCTETS_INDEX,             /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX,        /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX,       /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX,       /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX,      /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX,     /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX,      /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_BROADCAST_PKTS_INDEX,            /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_MULTICAST_PKTS_INDEX,            /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_FRAGMENTS_INDEX,                 /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_JABBERS_INDEX,                   /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_TX_CRC_ALIGN_ERROR_INDEX,           /* Proprietary counter. RFC 2819 EtherStatsEntry. Count TX packets ONLY */
    ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX,           /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX,      /* Proprietary counter. */
    ETHER_STATS_RX_OVERSIZE_PKTS_INDEX,             /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    ETHER_STATS_RX_PKTS_64OCTETS_INDEX,             /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX,        /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX,       /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX,       /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX,      /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX,     /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX,      /* Proprietary counter. RFC 2819 EtherStatsEntry. Count RX packets ONLY */
    TX_ETHER_STATS_FRAGMENTS_INDEX,                 /* Proprietary counter */
    TX_ETHER_STATS_JABBERS_INDEX,                   /* Proprietary counter */
    TX_ETHER_STATS_CRC_ALIGN_ERRORS_INDEX,          /* Proprietary counter */

    IN_OAM_PDU_PKTS_INDEX,                   		/* Proprietary counter */
    OUT_OAM_PDU_PKTS_INDEX,          				/* Proprietary counter */

    MIB_PORT_CNTR_END
}rtk_stat_port_type_t;

typedef enum rtk_stat_pktInfo_reasonCode_e
{
    PKTINFO_REASON_NORMAL = 0,

    PKTINFO_REASON_NAT0 = 1,
    PKTINFO_REASON_NAT1,
    PKTINFO_REASON_NAT2,
    PKTINFO_REASON_NAT3,
    PKTINFO_REASON_NAT4,
    PKTINFO_REASON_NAT5,
    PKTINFO_REASON_NAT6,
    PKTINFO_REASON_NAT7,
    PKTINFO_REASON_NAT8,
    PKTINFO_REASON_NAT9,
    PKTINFO_REASON_NAT10,
    PKTINFO_REASON_NAT11,
    PKTINFO_REASON_NAT12,
    PKTINFO_REASON_NAT13,
    PKTINFO_REASON_NAT14,
    PKTINFO_REASON_NAT15,
    PKTINFO_REASON_NAT16,
    PKTINFO_REASON_NAT17,
    PKTINFO_REASON_NAT18,
    PKTINFO_REASON_NAT19,
    PKTINFO_REASON_NAT20,
    PKTINFO_REASON_NAT21,
    PKTINFO_REASON_NAT22,
    PKTINFO_REASON_NAT23,
    PKTINFO_REASON_NAT24,
    PKTINFO_REASON_NAT25,
    PKTINFO_REASON_NAT26,
    PKTINFO_REASON_NAT27,
    PKTINFO_REASON_NAT28,
    PKTINFO_REASON_NAT29,
    PKTINFO_REASON_NAT30,
    PKTINFO_REASON_NAT31,
    PKTINFO_REASON_NAT32,
    PKTINFO_REASON_NAT33,
    PKTINFO_REASON_NAT34,
    PKTINFO_REASON_NAT35,
    PKTINFO_REASON_NAT36,
    PKTINFO_REASON_NAT37,
    PKTINFO_REASON_NAT38,
    PKTINFO_REASON_NAT39,
    PKTINFO_REASON_NAT40,
    PKTINFO_REASON_NAT41,
    PKTINFO_REASON_NAT42,
    PKTINFO_REASON_NAT43,
    PKTINFO_REASON_NAT44,
    PKTINFO_REASON_NAT45,
    PKTINFO_REASON_NAT46,
    PKTINFO_REASON_NAT47,
    PKTINFO_REASON_NAT48,
    PKTINFO_REASON_NAT49,
    PKTINFO_REASON_NAT50,
    PKTINFO_REASON_NAT51,
    PKTINFO_REASON_NAT52,
    PKTINFO_REASON_NAT53,
    PKTINFO_REASON_NAT54,
    PKTINFO_REASON_NAT55,
    PKTINFO_REASON_NAT56,
    PKTINFO_REASON_NAT57,
    PKTINFO_REASON_NAT58,
    PKTINFO_REASON_NAT59,
    PKTINFO_REASON_NAT60,
    PKTINFO_REASON_NAT61,
    PKTINFO_REASON_NAT62,

    PKTINFO_REASON_ACL_RULE0 = 64,
    PKTINFO_REASON_ACL_RULE1,
    PKTINFO_REASON_ACL_RULE2,
    PKTINFO_REASON_ACL_RULE3,
    PKTINFO_REASON_ACL_RULE4,
    PKTINFO_REASON_ACL_RULE5,
    PKTINFO_REASON_ACL_RULE6,
    PKTINFO_REASON_ACL_RULE7,
    PKTINFO_REASON_ACL_RULE8,
    PKTINFO_REASON_ACL_RULE9,
    PKTINFO_REASON_ACL_RULE10,
    PKTINFO_REASON_ACL_RULE11,
    PKTINFO_REASON_ACL_RULE12,
    PKTINFO_REASON_ACL_RULE13,
    PKTINFO_REASON_ACL_RULE14,
    PKTINFO_REASON_ACL_RULE15,
    PKTINFO_REASON_ACL_RULE16,
    PKTINFO_REASON_ACL_RULE17,
    PKTINFO_REASON_ACL_RULE18,
    PKTINFO_REASON_ACL_RULE19,
    PKTINFO_REASON_ACL_RULE20,
    PKTINFO_REASON_ACL_RULE21,
    PKTINFO_REASON_ACL_RULE22,
    PKTINFO_REASON_ACL_RULE23,
    PKTINFO_REASON_ACL_RULE24,
    PKTINFO_REASON_ACL_RULE25,
    PKTINFO_REASON_ACL_RULE26,
    PKTINFO_REASON_ACL_RULE27,
    PKTINFO_REASON_ACL_RULE28,
    PKTINFO_REASON_ACL_RULE29,
    PKTINFO_REASON_ACL_RULE30,
    PKTINFO_REASON_ACL_RULE31,
    PKTINFO_REASON_ACL_RULE32,
    PKTINFO_REASON_ACL_RULE33,
    PKTINFO_REASON_ACL_RULE34,
    PKTINFO_REASON_ACL_RULE35,
    PKTINFO_REASON_ACL_RULE36,
    PKTINFO_REASON_ACL_RULE37,
    PKTINFO_REASON_ACL_RULE38,
    PKTINFO_REASON_ACL_RULE39,
    PKTINFO_REASON_ACL_RULE40,
    PKTINFO_REASON_ACL_RULE41,
    PKTINFO_REASON_ACL_RULE42,
    PKTINFO_REASON_ACL_RULE43,
    PKTINFO_REASON_ACL_RULE44,
    PKTINFO_REASON_ACL_RULE45,
    PKTINFO_REASON_ACL_RULE46,
    PKTINFO_REASON_ACL_RULE47,
    PKTINFO_REASON_ACL_RULE48,
    PKTINFO_REASON_ACL_RULE49,
    PKTINFO_REASON_ACL_RULE50,
    PKTINFO_REASON_ACL_RULE51,
    PKTINFO_REASON_ACL_RULE52,
    PKTINFO_REASON_ACL_RULE53,
    PKTINFO_REASON_ACL_RULE54,
    PKTINFO_REASON_ACL_RULE55,
    PKTINFO_REASON_ACL_RULE56,
    PKTINFO_REASON_ACL_RULE57,
    PKTINFO_REASON_ACL_RULE58,
    PKTINFO_REASON_ACL_RULE59,
    PKTINFO_REASON_ACL_RULE60,
    PKTINFO_REASON_ACL_RULE61,
    PKTINFO_REASON_ACL_RULE62,
    PKTINFO_REASON_ACL_RULE63,

    PKTINFO_REASON_DOS_DAEGSA = 128,
    PKTINFO_REASON_DOS_LAND_ATTACK,
    PKTINFO_REASON_DOS_BLAT_ATTACK,
    PKTINFO_REASON_DOS_SYNFIN_SCAN,
    PKTINFO_REASON_DOS_XMAS_SCAN,
    PKTINFO_REASON_DOS_NULL_SCAN,
    PKTINFO_REASON_DOS_SYN1024,
    PKTINFO_REASON_DOS_TCP_SHORTHDR,
    PKTINFO_REASON_DOS_TCPFRAGERROR,
    PKTINFO_REASON_DOS_ICMPFRAGMENT,
    PKTINFO_REASON_DOS_PINGOFDEATH,
    PKTINFO_REASON_DOS_UDPBOMB,
    PKTINFO_REASON_DOS_SYNWITHDATA,
    PKTINFO_REASON_DOS_SYNFLOOD,
    PKTINFO_REASON_DOS_FINFLOOD,
    PKTINFO_REASON_DOS_ICMPFLOOD,

    PKTINFO_REASON_CVLAN_POLICING = 144,
    PKTINFO_REASON_CVLAN_EGMASK,
    PKTINFO_REASON_CVLAN_IGDROP,
    PKTINFO_REASON_CVLAN_TYPECHECK,

    PKTINFO_REASON_SVLAN_UNTAG = 148,
    PKTINFO_REASON_SVLAN_UNMATCH,
    PKTINFO_REASON_SVLAN_DROP,
    PKTINFO_REASON_SVLAN_EGMASK,

    PKTINFO_REASON_RLPP = 152,

    PKTINFO_REASON_RLDP = 153,

    PKTINFO_REASON_LLDP = 154,

    PKTINFO_REASON_OTHER_RLDP = 155,

    PKTINFO_REASON_FORCE = 156,

    PKTINFO_REASON_PKTLEN = 157,

    PKTINFO_REASON_SPANTREE_TX = 158,
    PKTINFO_REASON_SPANTREE_RX,

    PKTINFO_REASON_RMA_IEEE_00 = 160,
    PKTINFO_REASON_RMA_IEEE_01,
    PKTINFO_REASON_RMA_IEEE_02,
    PKTINFO_REASON_RMA_IEEE_03,
    PKTINFO_REASON_RMA_IEEE_04,
    PKTINFO_REASON_RMA_IEEE_08,
    PKTINFO_REASON_RMA_IEEE_0D,
    PKTINFO_REASON_RMA_IEEE_0E,
    PKTINFO_REASON_RMA_IEEE_10,
    PKTINFO_REASON_RMA_IEEE_11,
    PKTINFO_REASON_RMA_IEEE_12,
    PKTINFO_REASON_RMA_IEEE_13,
    PKTINFO_REASON_RMA_IEEE_18,
    PKTINFO_REASON_RMA_IEEE_1A,
    PKTINFO_REASON_RMA_IEEE_20,
    PKTINFO_REASON_RMA_IEEE_21,
    PKTINFO_REASON_RMA_IEEE_22,
    PKTINFO_REASON_RMA_CISCO_CC,
    PKTINFO_REASON_RMA_CISCO_CD,

    PKTINFO_REASON_L2_LEARNING_LIMIT_PORT = 192,
    PKTINFO_REASON_L2_LEARNING_LIMIT_SYSTEM,

    PKTINFO_REASON_8021X_TRAP_DROP = 194,
    PKTINFO_REASON_8021X_EGRESS_PM,

    PKTINFO_REASON_UNKN_SA = 196,
    PKTINFO_REASON_UNKN_MA_SA,
    PKTINFO_REASON_LINK,
    PKTINFO_REASON_PORT_ISOLATION,

    PKTINFO_REASON_STORM_BCAST = 200,
    PKTINFO_REASON_STORM_KN_MCAST,
    PKTINFO_REASON_STORM_UNKN_UCAST,
    PKTINFO_REASON_STORM_UNKN_MCAST,

    PKTINFO_REASON_UNKN_UC_DA = 204,
    PKTINFO_REASON_UNKN_L2MC_DA,
    PKTINFO_REASON_UNKN_IPV4MC_DA,
    PKTINFO_REASON_UNKN_IPV6MC_DA,

    PKTINFO_REASON_MPCP,
    PKTINFO_REASON_DS_OMCI,
    PKTINFO_REASON_CF,
    PKTINFO_REASON_OAM,
    PKTINFO_REASON_SA_BLOCK,
    PKTINFO_REASON_DA_BLOCK,
    PKTINFO_REASON_FLOOD,
    PKTINFO_REASON_IGMP,
    PKTINFO_REASON_MC_DATA,
    PKTINFO_REASON_L34_MC_DATA,
    PKTINFO_REASON_MIRR_ISO,
    PKTINFO_REASON_EGRESS_DROP,
    PKTINFO_REASON_SRC_BLK,
    PKTINFO_REASON_PTP = 255,

    PKTINFO_PORT_CNTR_END
}rtk_stat_pktInfo_reasonCode_t;

typedef enum rtk_stat_aclCnt_type_e
{
    ACL_CNT_TYPE_PKTCNT = 0,
    ACL_CNT_TYPE_BYTECNT,
    ACL_CNT_TYPE_END
}rtk_stat_aclCnt_type_t;

typedef enum rtk_stat_aclCnt_mode_e
{
    ACL_CNT_MODE_32BITS = 0,
    ACL_CNT_MODE_64BITS,
    ACL_CNT_MODE_END
}rtk_stat_aclCnt_mode_t;

typedef enum rtk_mib_rst_value_e
{
    STAT_MIB_RST_TO_0 = 0,
    STAT_MIB_RST_TO_1,
    STAT_MIB_RST_END
}rtk_mib_rst_value_t;

typedef enum rtk_mib_sync_mode_e
{
    STAT_MIB_SYNC_MODE_STOP_SYNC = 0,
    STAT_MIB_SYNC_MODE_FREE_SYNC,
    STAT_MIB_SYNC_MODE_END
}rtk_mib_sync_mode_t;

typedef enum rtk_mib_count_mode_e
{
    STAT_MIB_COUNT_MODE_FREE = 0,
    STAT_MIB_COUNT_MODE_TIMER,
    STAT_MIB_COUNT_MODE_END
}rtk_mib_count_mode_t;

typedef enum rtk_mib_tag_cnt_dir_e
{
    STAT_MIB_TAG_CNT_DIR_TX = 0,
    STAT_MIB_TAG_CNT_DIR_RX,
    STAT_MIB_TAG_CNT_DIR_END
}rtk_mib_tag_cnt_dir_t;

typedef enum rtk_mib_tag_cnt_state_e
{
    STAT_MIB_TAG_CNT_STATE_EXCLUDE = 0,
    STAT_MIB_TAG_CNT_STATE_INCLUDE,
    STAT_MIB_TAG_CNT_STATE_END
}rtk_mib_tag_cnt_state_t;


/*
 * Data Declaration
 */

/* global statistic counter structure */
typedef struct rtk_stat_global_cntr_s
{
    rtk_uint64 dot1dTpLearnedEntryDiscards;
}rtk_stat_global_cntr_t;

/* port statistic counter structure */
typedef struct rtk_stat_port_cntr_s
{
    rtk_uint64 ifInOctets;
    rtk_uint32 dot3StatsFCSErrors;
    rtk_uint32 dot3StatsSymbolErrors;
    rtk_uint32 dot3InPauseFrames;
    rtk_uint32 dot3ControlInUnknownOpcodes;
    rtk_uint32 etherStatsFragments;
    rtk_uint32 etherStatsJabbers;
    rtk_uint32 ifInUcastPkts;
    rtk_uint32 etherStatsDropEvents;
    rtk_uint64 etherStatsOctets;
    rtk_uint32 etherStatsUndersizePkts;
    rtk_uint32 etherStatsOversizePkts;
    rtk_uint32 etherStatsPkts64Octets;
    rtk_uint32 etherStatsPkts65to127Octets;
    rtk_uint32 etherStatsPkts128to255Octets;
    rtk_uint32 etherStatsPkts256to511Octets;
    rtk_uint32 etherStatsPkts512to1023Octets;
    rtk_uint32 etherStatsPkts1024toMaxOctets;
    rtk_uint32 etherStatsMcastPkts;
    rtk_uint32 etherStatsBcastPkts;
    rtk_uint64 ifOutOctets;
    rtk_uint32 dot3StatsSingleCollisionFrames;
    rtk_uint32 dot3StatsMultipleCollisionFrames;
    rtk_uint32 dot3StatsDeferredTransmissions;
    rtk_uint32 dot3StatsLateCollisions;
    rtk_uint32 etherStatsCollisions;
    rtk_uint32 dot3StatsExcessiveCollisions;
    rtk_uint32 dot3OutPauseFrames;
    rtk_uint32 dot1dBasePortDelayExceededDiscards;
    rtk_uint32 dot1dTpPortInDiscards;
    rtk_uint32 ifOutUcastPkts;
    rtk_uint32 ifOutMulticastPkts;
    rtk_uint32 ifOutBrocastPkts;
    rtk_uint32 outOampduPkts;
    rtk_uint32 inOampduPkts;
    rtk_uint32 pktgenPkts;
    rtk_uint32 inMldChecksumError;
    rtk_uint32 inIgmpChecksumError;
    rtk_uint32 inMldSpecificQuery;
    rtk_uint32 inMldGeneralQuery;
    rtk_uint32 inIgmpSpecificQuery;
    rtk_uint32 inIgmpGeneralQuery;
    rtk_uint32 inMldLeaves;
    rtk_uint32 inIgmpLeaves;
    rtk_uint32 inIgmpJoinsSuccess;
    rtk_uint32 inIgmpJoinsFail;
    rtk_uint32 inMldJoinsSuccess;
    rtk_uint32 inMldJoinsFail;
    rtk_uint32 inReportSuppressionDrop;
    rtk_uint32 inLeaveSuppressionDrop;
    rtk_uint32 outIgmpReports;
    rtk_uint32 outIgmpLeaves;
    rtk_uint32 outIgmpGeneralQuery;
    rtk_uint32 outIgmpSpecificQuery;
    rtk_uint32 outMldReports;
    rtk_uint32 outMldLeaves;
    rtk_uint32 outMldGeneralQuery;
    rtk_uint32 outMldSpecificQuery;
    rtk_uint32 inKnownMulticastPkts;
    rtk_uint32 ifInMulticastPkts;
    rtk_uint32 ifInBroadcastPkts;
}rtk_stat_port_cntr_t;



/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      rtk_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_PORT_CNTR_FAIL   - Could not retrieve/reset Port Counter
 * Note:
 *      Must initialize stat module before calling any stat APIs.
 */
extern int32
rtk_stat_init(void);


/* Function Name:
 *      rtk_stat_global_reset
 * Description:
 *      Reset the global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED

 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 * Note:
 *      None
 */
extern int32
rtk_stat_global_reset(void);


/* Function Name:
 *      rtk_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
rtk_stat_port_reset(rtk_port_t port);

/* Function Name:
 *      rtk_stat_port_reset
 * Description:
 *      Reset the specified ACL logging counters.
 * Input:
 *      index - ACL logging index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_acl_reset(uint32 index);

/* Function Name:
 *      rtk_stat_rst_cnt_value_set
 * Description:
 *      Set the counter value after reset
 * Input:
 *      None
 * Output:
 *      rstValue  - the counter value after reset
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_rst_cnt_value_set(rtk_mib_rst_value_t rstValue);

/* Function Name:
 *      rtk_stat_rst_cnt_value_get
 * Description:
 *      Get the counter value after reset
 * Input:
 *      None
 * Output:
 *      pRstValue  - pointer buffer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
rtk_stat_rst_cnt_value_get(rtk_mib_rst_value_t *pRstValue);

/* Function Name:
 *      rtk_stat_global_get
 * Description:
 *      Get one specified global counter in the specified device.
 * Input:
 *      cntrIdx - specified global counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
extern int32
rtk_stat_global_get(rtk_stat_global_type_t cntrIdx, uint64 *pCntr);


/* Function Name:
 *      rtk_stat_global_getAll
 * Description:
 *      Get all global counters in the specified device.
 * Input:
 *      None
 * Output:
 *      pGlobalCntrs - pointer buffer of global counter structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
extern int32
rtk_stat_global_getAll(rtk_stat_global_cntr_t *pGlobalCntrs);


/* Function Name:
 *      rtk_stat_port_get
 * Description:
 *      Get one specified port counter.
 * Input:
 *      port     - port id
 *      cntrIdx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
rtk_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr);


/* Function Name:
 *      rtk_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
extern int32
rtk_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs);

/* Function Name:
 *      rtk_stat_acl_get
 * Description:
 *      Get ACL logging counter.
 * Input:
 *      cntrIdx  - logging index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - invalid index
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
rtk_stat_acl_get(uint32 index, uint64 *pCntr);

/* Function Name:
 *      rtk_stat_acl_mode_set
 * Description:
 *      Set the acl counters mode for 32-bits or 64-bits counter
 * Input:
 *      index 		- index of ACL counter
 *      mode 		- counter mode of ACL counter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_acl_mode_set(uint32 index, rtk_stat_aclCnt_mode_t mode);

/* Function Name:
 *      rtk_stat_acl_mode_get
 * Description:
 *      Get the acl counters mode for 32-bits or 64-bits counter
 * Input:
 *      index 		- index of ACL counter
 * Output:
 *      pMode 		- counter mode of ACL counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_acl_mode_get(uint32 index, rtk_stat_aclCnt_mode_t* pMode);

/* Function Name:
 *      rtk_stat_acl_type_set
 * Description:
 *      Set the acl counters type for packet or byte counter
 * Input:
 *      index 		- index of ACL counter
 *      type		- counter type of ACL counter
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_acl_type_set(uint32 index, rtk_stat_aclCnt_type_t type);

/* Function Name:
 *      rtk_stat_acl_type_get
 * Description:
 *      Get the acl counters type for packet or byte counter
 * Input:
 *      index 		- index of ACL counter
 * Output:
 *      pType 		- counter type of ACL counter
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_acl_type_get(uint32 index, rtk_stat_aclCnt_type_t* pType);

/* Function Name:
 *      rtk_stat_mib_cnt_mode_get
 * Description:
 *      Get the MIB data update mode
 * Input:
 *      None
 * Output:
 *      pCnt_mode   - pointer buffer of MIB data update mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
rtk_stat_mib_cnt_mode_get(rtk_mib_count_mode_t *pCnt_mode);

/* Function Name:
 *      rtk_stat_mib_cnt_mode_set
 * Description:
 *      Set MIB data update mode
 * Input:
 *      cnt_mode        - MIB counter update mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_mib_cnt_mode_set(rtk_mib_count_mode_t cnt_mode);

/* Function Name:
 *      rtk_stat_mib_latch_timer_get
 * Description:
 *      Get the MIB latch timer
 * Input:
 *      None
 * Output:
 *      pTimer   - pointer buffer of MIB latch timer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
rtk_stat_mib_latch_timer_get(uint32 *pTimer);

/* Function Name:
 *      rtk_stat_mib_latch_timer_set
 * Description:
 *      Set MIB data update mode
 * Input:
 *      timer        - MIB latch timer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_mib_latch_timer_set(uint32 timer);

/* Function Name:
 *      rtk_stat_mib_sync_mode_get
 * Description:
 *      Get the MIB register data update mode
 * Input:
 *      None
 * Output:
 *      pSync_mode   - pointer buffer of MIB register data update mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
rtk_stat_mib_sync_mode_get(rtk_mib_sync_mode_t *pSync_mode);

/* Function Name:
 *      rtk_stat_mib_sync_mode_set
 * Description:
 *      Set MIB register data update mode
 * Input:
 *      sync_mode        - MIB register data update mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_mib_sync_mode_set(rtk_mib_sync_mode_t sync_mode);

/* Function Name:
 *      rtk_stat_mib_count_tag_length_get
 * Description:
 *      Get counting Tag length state in tx/rx packet
 * Input:
 *      direction - count tx or rx tag length
 * Output:
 *      pState   - pointer buffer of count tx/rx tag length state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
extern int32
rtk_stat_mib_count_tag_length_get(rtk_mib_tag_cnt_dir_t direction, rtk_mib_tag_cnt_state_t *pState)'

/* Function Name:
 *      rtk_stat_mib_count_tag_length_set
 * Description:
 *      Set counting length including Ctag length or excluding Ctag length for tx/rx packet
 * Input:
 *      direction - count tx or rx tag length
 *      enable    - count tag length state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
extern int32
rtk_stat_mib_count_tag_length_set(rtk_mib_tag_cnt_dir_t direction, rtk_mib_tag_cnt_state_t state);

/* Function Name:
 *      rtk_stat_pktInfo_get
 * Description:
 *      Get the newest packet trap/drop reason
 * Input:
 *      port - port index
 * Output:
 *      pCode   - the newest packet trap/drop reason code
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
extern int32
rtk_stat_pktInfo_get(rtk_port_t port, uint32 *pCode);

#endif /* __RTK_STAT_H__ */
