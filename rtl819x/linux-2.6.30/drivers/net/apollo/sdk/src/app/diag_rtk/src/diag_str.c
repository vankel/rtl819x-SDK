/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * Purpose : Definition those SVLAN command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <diag_str.h>
#include <dal/apollo/raw/apollo_raw_svlan.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*common string*/
const char *diagStr_enable[] = {
    DIAG_STR_DISABLE,
    DIAG_STR_ENABLE
};

/*common string*/
const char *diagStr_valid[] = {
    DIAG_STR_INVALID,
    DIAG_STR_VALID
};

/*svlan string*/
const char *diagStr_svlanAct[] = {
    DIAG_STR_DROP,
    DIAG_STR_TRAP2CPU,
    DIAG_STR_ASSIGN_SVLAN,
    DIAG_STR_ASSIGN_SVLAN_AND_KEEP
};

const char *diagStr_svlanAction[] = {
    DIAG_STR_DROP,
    DIAG_STR_TRAP2CPU,
    DIAG_STR_ASSIGN_SVLAN,
    DIAG_STR_USE_CVID,
    DIAG_STR_ASSIGN_SVLAN_AND_KEEP
};


const char *diagStr_svlanSpriSrc[] = {
    DIAG_STR_INTERNAL_PRI,
    DIAG_STR_1Q_TAG_PRI,
    DIAG_STR_SPRI,
    DIAG_STR_PB_PRI
};

const char *diagStr_svlanFmtStr[] = {
    DIAG_STR_MAC31_0,
    DIAG_STR_IPV4_DIP
};

const char *diagStr_actionStr[] = {
    DIAG_STR_FORWARD,
    DIAG_STR_DROP,
    DIAG_STR_TRAP2CPU,
    DIAG_STR_COPY2CPU,
    DIAG_STR_2GUESTVLAN,
    DIAG_STR_FLOOD_IN_VLAN,
    DIAG_STR_FLOOD_2_ALLPORT,
    DIAG_STR_FLOOD_2_ROUTER_PORT,
    DIAG_STR_FORWARD_EX_CPU,
    DIAG_STR_DROP_EX_RMA,
    DIAG_STR_NOTSUPPORT,

};

const char *diagStr_igmpTypeStr[] = {
    DIAG_STR_IGMPV1,
    DIAG_STR_IGMPV2,
    DIAG_STR_IGMPV3,
    DIAG_STR_MLDV1,
    DIAG_STR_MLDV2,
};

const char *diagStr_aclRangeCheckLenTypeStr[] = {
    DIAG_STR_NOTREVISE,
    DIAG_STR_REVISE,
};

const char *diagStr_aclRangeCheckPortTypeStr[] = {
    DIAG_STR_INVALID,
    DIAG_STR_SPORT,
    DIAG_STR_DPORT,
};

const char *diagStr_aclRangeCheckIpTypeStr[] = {
    DIAG_STR_INVALID,
    DIAG_STR_IPV4_SIP,
    DIAG_STR_IPV4_DIP,
    DIAG_STR_IPV6_SIP,
    DIAG_STR_IPV6_DIP,
};

const char *diagStr_aclRangeCheckVidTypeStr[] = {
    DIAG_STR_INVALID,
    DIAG_STR_CVID,
    DIAG_STR_SVID,
};

const char *diagStr_aclModeStr[] = {
    DIAG_STR_64ENTIRES,
    DIAG_STR_128ENTIRES
};


const char *diagStr_aclActCvlanStr[] = {
    DIAG_STR_INGRESS_VLAN,
    DIAG_STR_EGRESS_VLAN,
    DIAG_STR_SVID,
    DIAG_STR_POLICING,
    DIAG_STR_ACLMIB,
    DIAG_STR_1PREMARK
};

const char *diagStr_aclActSvlanStr[] = {
    DIAG_STR_INGRESS_VLAN,
    DIAG_STR_EGRESS_VLAN,
    DIAG_STR_CVID,
    DIAG_STR_POLICING,
    DIAG_STR_ACLMIB,
    DIAG_STR_1PREMARK,
    DIAG_STR_DSCPREMARK
};

const char *diagStr_aclActPoliceStr[] = {
    DIAG_STR_POLICING,
    DIAG_STR_ACLMIB,
};

const char *diagStr_aclActFwdStr[] = {
    DIAG_STR_COPY,
    DIAG_STR_REDIRECT,
    DIAG_STR_MIRROR,
    DIAG_STR_ACLTRAP,
};

const char *diagStr_aclActPriStr[] = {
    DIAG_STR_ACLPRI,
    DIAG_STR_DSCPREMARK,
    DIAG_STR_1PREMARK,
    DIAG_STR_POLICING,
    DIAG_STR_ACLMIB,
};

const char *diagStr_aclActCfStr[] = {
    DIAG_STR_NONE,
    DIAG_STR_SID,
    DIAG_STR_LLID,
    DIAG_STR_DSLEXT,
};

const char *diagStr_l2IpMcHashOpStr[] = {
    DIAG_STR_DIPONLY,
    DIAG_STR_DIPSIP
};

const char *diagStr_l2IpMcHashMethodStr[] = {
    DIAG_STR_MACFID,
    DIAG_STR_DIPSIP,
    DIAG_STR_DIPVID
};

const char *diagStr_l2LutStaticOrAutoStr[] = {
    DIAG_STR_AUTO,
    DIAG_STR_STATIC,
};

const char *diagStr_l2HashMethodStr[] = {
    DIAG_STR_SVL,
    DIAG_STR_IVL,
};

const char *diagStr_enDisplay[] = {
    DIAG_STR_X,
    DIAG_STR_V
};

const char *diagStr_l34NexthopTypeStr[] = {
    DIAG_STR_ETHERNET,
    DIAG_STR_PPPOE
};


const char *diagStr_vlanTagType[] = {
    DIAG_STR_UNTAG,
    DIAG_STR_TAG
};

const char *diagStr_aclOper[] = {
    DIAG_STR_HIT,
    DIAG_STR_NOT
};


const char *diagStr_trunkMode[] = {
    DIAG_STR_TRUNK_NORMAL_MODE,
    DIAG_STR_TRUNK_DUMB_MODE
};

const char *diagStr_trunkAlgorithm[] = {
    DIAG_STR_TRUNK_HASH_SOURCE_PORT,
    DIAG_STR_TRUNK_HASH_SOURCE_MAC,
    DIAG_STR_TRUNK_HASH_DEST_MAC,
    DIAG_STR_TRUNK_HASH_SOURCE_IP,
    DIAG_STR_TRUNK_HASH_DEST_IP,
    DIAG_STR_TRUNK_HASH_SOURCE_L4PORT,
    DIAG_STR_TRUNK_HASH_DEST_L4PORT
};

const char *diagStr_trunkFloodMode[] = {
    DIAG_STR_TRUNK_FLOOD_NORMAL,
    DIAG_STR_TRUNK_FLOOD_TO_FIRST
};

const char *diagStr_direction[] = {
    DIAG_STR_UPSTREAM,
    DIAG_STR_DOWNSTREAM
};

const char *diagStr_usCStagAction[] = {
    DIAG_STR_NOP,
    DIAG_STR_VS_TPID,
    DIAG_STR_TPID_8100,
    DIAG_STR_DEL,
    DIAG_STR_TRANSPARENT,
};

const char *diagStr_usSvidAction[] = {
    DIAG_STR_ASSIGN,
    DIAG_STR_COPY_1st,
    DIAG_STR_COPY_2nd
};

const char *diagStr_usSpriAction[] = {
    DIAG_STR_ASSIGN,
    DIAG_STR_COPY_1st,
    DIAG_STR_COPY_2nd,
    DIAG_STR_INTER_PRI
};

const char *diagStr_usCtagAction[] = {
    DIAG_STR_NOP,
    DIAG_STR_TAG,
    DIAG_STR_C2S,
    DIAG_STR_DEL,
    DIAG_STR_TRANSPARENT
};

const char *diagStr_usCvidAction[] = {
    DIAG_STR_ASSIGN,
    DIAG_STR_COPY_1st,
    DIAG_STR_COPY_2nd,
    DIAG_STR_INTER_VID
};

const char *diagStr_usCpriAction[] = {
    DIAG_STR_ASSIGN,
    DIAG_STR_COPY_1st,
    DIAG_STR_COPY_2nd,
    DIAG_STR_INTER_PRI
};

const char *diagStr_usSidAction[] = {
    DIAG_STR_ASSIGN_SID,
    DIAG_STR_ASSIGN_QID
};

const char *diagStr_dsCStagAction[] = {
    DIAG_STR_NOP,
    DIAG_STR_VS_TPID,
    DIAG_STR_TPID_8100,
    DIAG_STR_DEL,
    DIAG_STR_TRANSPARENT,
    DIAG_STR_SP2C
};

const char *diagStr_dsSvidAction[] = {
    DIAG_STR_NOP,
    DIAG_STR_ASSIGN,
    DIAG_STR_COPY_1st,
    DIAG_STR_COPY_2nd,
    DIAG_STR_LUT_LRN
};

const char *diagStr_dsSpriAction[] = {
    DIAG_STR_NOP,
    DIAG_STR_ASSIGN,
    DIAG_STR_COPY_1st,
    DIAG_STR_COPY_2nd,
    DIAG_STR_INTER_PRI
};

const char *diagStr_dsCtagAction[] = {
    DIAG_STR_NOP,
    DIAG_STR_TAG,
    DIAG_STR_SP2C,
    DIAG_STR_DEL,
    DIAG_STR_TRANSPARENT
};

const char *diagStr_dsCvidAction[] = {
    DIAG_STR_NOP,
    DIAG_STR_ASSIGN,
    DIAG_STR_COPY_1st,
    DIAG_STR_COPY_2nd,
    DIAG_STR_LUT_LRN
};

const char *diagStr_dsCpriAction[] = {
    DIAG_STR_NOP,
    DIAG_STR_ASSIGN,
    DIAG_STR_COPY_1st,
    DIAG_STR_COPY_2nd,
    DIAG_STR_INTER_PRI
};

const char *diagStr_cfpriAction[] = {
    DIAG_STR_SWITCH_CORE,
    DIAG_STR_ASSIGN
};

const char *diagStr_dsUniAction[] = {
    DIAG_STR_FORWARD,
    DIAG_STR_FS_FORWARD
};

const char *diagStr_cfRangeCheckIpTypeStr[] = {
    DIAG_STR_IPV4_SIP,
    DIAG_STR_IPV4_DIP
};

const char *diagStr_cfRangeCheckPortTypeStr[] = {
    DIAG_STR_SPORT,
    DIAG_STR_DPORT,
};

const char *diagStr_frameType[] = {
    DIAG_STR_ETHERNET,
    DIAG_STR_LLC_OTHER,
    DIAG_STR_SNAP
};

const char *diagStr_svlanLookup[] = {
    DIAG_STR_SVLAN_64,
    DIAG_STR_CVLAN_4K
};

const char *diagStr_stormType[] = {
    DIAG_STR_STORM_UNKN_MC,
    DIAG_STR_STORM_UNKN_UC,
    DIAG_STR_STORM_MC,
    DIAG_STR_STORM_BC
};

const char *diagStr_stormAltType[] = {
    DIAG_STR_DEFAULT,
    DIAG_STR_STORM_ALT_ARP,
    DIAG_STR_STORM_ALT_DHCP,
    DIAG_STR_STORM_ALT_IGMPMLD
};

const char *diagStr_authstate[] = {
    DIAG_STR_UNAUTHORIZED,
    DIAG_STR_AUTHORIZED,
};


const char *diagStr_unAuthAct[] = {
    DIAG_STR_DROP,
    DIAG_STR_TRAP2CPU,
    DIAG_STR_GUEST_VLAN
};


const char *diagStr_1xOpDir[] = {
    DIAG_STR_DOT1XOPDIR_BOTH,
    DIAG_STR_DOT1XOPDIR_IN
};

const char *diagStr_ipgCompensation[] = {
    DIAG_STR_90PPM,
    DIAG_STR_65PPM
};


const char *diagStr_AfbMonCount[] = {
    DIAG_STR_AFB_MONCOUNT_8K,
    DIAG_STR_AFB_MONCOUNT_16K,
    DIAG_STR_AFB_MONCOUNT_32K,
    DIAG_STR_AFB_MONCOUNT_64K,
    DIAG_STR_AFB_MONCOUNT_128K,
    DIAG_STR_AFB_MONCOUNT_256K,
    DIAG_STR_AFB_MONCOUNT_512K,
    DIAG_STR_AFB_MONCOUNT_1M
};

const char *diagStr_AfbErrCount[] = {
    DIAG_STR_AFB_ERRCOUNT_1,
    DIAG_STR_AFB_ERRCOUNT_2,
    DIAG_STR_AFB_ERRCOUNT_4,
    DIAG_STR_AFB_ERRCOUNT_8,
    DIAG_STR_AFB_ERRCOUNT_16,
    DIAG_STR_AFB_ERRCOUNT_32,
    DIAG_STR_AFB_ERRCOUNT_64,
    DIAG_STR_AFB_ERRCOUNT_128
};

const char *diagStr_AfbRestorePL[] = {
    DIAG_STR_AFB_NOT_RESTORE_PL,
    DIAG_STR_AFB_RESTORE_PL
};

const char *diagStr_AfbvalidFlow[] = {
    DIAG_STR_AFB_NONE_VALID_FLOW,
    DIAG_STR_AFB_VALID_FLOW
};

const char *diagStr_queueType[] = {
    DIAG_STR_QUEUE_WFQ,
    DIAG_STR_QUEUE_STRICT
};

const char *diagStr_portSpeed[] = {
    DIAG_STR_SPEED_10M,
    DIAG_STR_SPEED_100M,
    DIAG_STR_SPEED_GIGA
};

const char *diagStr_portDuplex[] = {
    DIAG_STR_HALF_DUPLEX,
    DIAG_STR_FULL_DUPLEX
};

const char *diagStr_portLinkStatus[] = {
    DIAG_STR_LINK_DOWN,
    DIAG_STR_LINK_UP
};

const char *diagStr_portNwayFault[] = {
    DIAG_STR_SUCCESS,
    DIAG_STR_FAULT
};

const char *diagStr_flowCtrlType[] = {
    DIAG_STR_FLOWCTRL_EGRESS,
    DIAG_STR_FLOWCTRL_INGRESS
};


const char *diagStr_selectorMode[] = {
    DIAG_STR_DEFAULT,
    DIAG_STR_RAW,
    DIAG_STR_LLC,
    DIAG_STR_IP4HEADER,
    DIAG_STR_ARP,
    DIAG_STR_IP6HEADER,
    DIAG_STR_IPPAYLOAD,
    DIAG_STR_L4PAYLOAD,
};

const char *diagStr_flowCtrlJumboSize[] = {
    DIAG_STR_FLOWCTRL_JUMBO_3K,
    DIAG_STR_FLOWCTRL_JUMBO_4K,
    DIAG_STR_FLOWCTRL_JUMBO_6K,
    DIAG_STR_FLOWCTRL_JUMBO_MAX
};

const char *diagStr_chipReset[] = {
    DIAG_STR_SW_VOIP_RST ,
    DIAG_STR_SW_PCIE_PHY_RST,
    DIAG_STR_SW_PCIE_CTRL_RST,
    DIAG_STR_SW_USB3_PHY_RST,
    DIAG_STR_SW_USB3_CTRL_RST,
    DIAG_STR_SW_USB2_PHY_RST,
    DIAG_STR_SW_USB2_CTRL_RST,
    DIAG_STR_SW_SATA_PHY_RST,
    DIAG_STR_SW_SATA_CTRL_RST,
    DIAG_STR_SW_GPHY_RST,
    DIAG_STR_SW_GLOBAL_RST,
    DIAG_STR_SW_RSG_RST,
    DIAG_STR_SW_CFG_RST,
    DIAG_STR_SW_Q_RST,
    DIAG_STR_SW_NIC_RST,
    DIAG_STR_CPU_MEM_RST,
    DIAG_STR_WDOG_NMI_EN,
    DIAG_STR_PONMAC_RST
};

const char *diagStr_backPressure[] = {
    DIAG_STR_BACKPRESSURE_JAM,
    DIAG_STR_BACKPRESSURE_DEFER
};

const char *diagStr_cpuTagFormat[] = {
    DIAG_STR_CPU_APOLLO_TAG_MODE,
    DIAG_STR_CPU_NORMAL_TAG_MODE
};

const char *diagStr_polarity[] = {
    DIAG_STR_POLARITY_HIGH,
    DIAG_STR_POLARITY_LOW
};

/*OAM string*/

const char *diagStr_oamParserAct[] = {
    DIAG_STR_PAR_FORWARD,
    DIAG_STR_PAR_LOOPBACK,
    DIAG_STR_PAR_DISCARD
};

const char *diagStr_oamMuxAct[] = {
    DIAG_STR_MUX_FORWARD,
    DIAG_STR_MUX_DISCARD,
    DIAG_STR_MUX_CPUONLY
};

const char *diagStr_cfUnmatchAct[] = {
    DIAG_STR_ACT_DROP,
    DIAG_STR_ACT_PERMIT_NO_PON,
    DIAG_STR_ACT_PERMIT
};



const char *diagStr_mirrorEgressMode[] = {
    DIAG_STR_MIRROR_ALL_PKT,
    DIAG_STR_MIRROR_MIR_PKT_ONLY
};

const char *diagStr_l2flushMode[] = {
    DIAG_STR_DYNAMIC,
    DIAG_STR_STATIC,
    DIAG_STR_BOTH,
    DIAG_STR_NONE
};

const char *diagStr_logMibMode[] = {
    DIAG_STR_32BITS,
    DIAG_STR_64BITS
};

const char *diagStr_logMibType[] = {
    DIAG_STR_PACKET_COUNT
    DIAG_STR_BYTE_COUNT,
};

const char *diagStr_globalMibCnt[] = {
    DIAG_STR_LEARNED_DISCARDS
};

const char *diagStr_mibName[] = {
    DIAG_STR_IF_IN_OCTETS,
    DIAG_STR_IF_IN_UCAST_PKTS,
    DIAG_STR_F_IN_MULTICAST_PKTS,
    DIAG_STR_IF_IN_BROADCAST_PKTS,
    DIAG_STR_IF_IN_DISCARDS,
    DIAG_STR_IF_OUT_OCTETS,
    DIAG_STR_IF_OUT_DISCARDS,
    DIAG_STR_IF_OUT_UCAST_PKTS_CNT,
    DIAG_STR_IF_OUT_MULTICAST_PKTS_CNT,
    DIAG_STR_IF_OUT_BROADCAST_PKTS_CNT,
    DIAG_STR_DOT1D_PORT_DELAY_EXCEEDED_DISCARDS,
    DIAG_STR_DOT1D_TP_PORT_IN_DISCARDS,
    DIAG_STR_DOT1D_TP_HC_PORT_IN_DISCARDS,
    DIAG_STR_DOT3_IN_PAUSE_FRAMES,
    DIAG_STR_DOT3_OUT_PAUSE_FRAMES,
    DIAG_STR_DOT3_OUT_PAUSE_ON_FRAMES,
    DIAG_STR_DOT3_STATS_ALIGNMENT_ERRORS,
    DIAG_STR_DOT3_STATS_FCS_ERRORS,
    DIAG_STR_DOT3_STATS_SINGLE_COLLISION_FRAMES,
    DIAG_STR_DOT3_STATS_MULTIPLE_COLLISION_FRAMES,
    DIAG_STR_DOT3_STATS_DEFERRED_TRANSMISSIONS,
    DIAG_STR_DOT3_STATS_LATE_COLLISIONS,
    DIAG_STR_DOT3_STATS_EXCESSIVE_COLLISIONS,
    DIAG_STR_DOT3_STATS_FRAME_TOO_LONGS,
    DIAG_STR_DOT3_STATS_SYMBOL_ERRORS,
    DIAG_STR_DOT3_CONTROL_IN_UNKNOWN_OPCODES,
    DIAG_STR_ETHER_STATS_DROP_EVENTS,
    DIAG_STR_ETHER_STATS_OCTETS,
    DIAG_STR_ETHER_STATS_BROADCAST_PKTS,
    DIAG_STR_ETHER_STATS_MULTICAST_PKTS,
    DIAG_STR_ETHER_STATS_UNDER_SIZE_PKTS,
    DIAG_STR_ETHER_STATS_OVERSIZE_PKTS,
    DIAG_STR_ETHER_STATS_FRAGMENTS,
    DIAG_STR_ETHER_STATS_JABBERS,
    DIAG_STR_ETHER_STATS_COLLISIONS,
    DIAG_STR_ETHER_STATS_CRC_ALIGN_ERRORS,
    DIAG_STR_ETHER_STATS_PKTS_64OCTETS,
    DIAG_STR_ETHER_STATS_PKTS_65TO127OCTETS,
    DIAG_STR_ETHER_STATS_PKTS_128TO255OCTETS,
    DIAG_STR_ETHER_STATS_PKTS_256TO511OCTETS,
    DIAG_STR_ETHER_STATS_PKTS_512TO1023OCTETS,
    DIAG_STR_ETHER_STATS_PKTS_1024TO1518OCTETS,
    DIAG_STR_ETHER_STATS_TX_OCTETS,
    DIAG_STR_ETHER_STATS_TX_UNDER_SIZE_PKTS,
    DIAG_STR_ETHER_STATS_TX_OVERSIZE_PKTS,
    DIAG_STR_ETHER_STATS_TX_PKTS_64OCTETS,
    DIAG_STR_ETHER_STATS_TX_PKTS_65TO127OCTETS,
    DIAG_STR_ETHER_STATS_TX_PKTS_128TO255OCTETS,
    DIAG_STR_ETHER_STATS_TX_PKTS_256TO511OCTETS,
    DIAG_STR_ETHER_STATS_TX_PKTS_512TO1023OCTETS,
    DIAG_STR_ETHER_STATS_TX_PKTS_1024TO1518OCTETS,
    DIAG_STR_ETHER_STATS_TX_PKTS_1519TOMAXOCTETS,
    DIAG_STR_ETHER_STATS_TX_BROADCAST_PKTS,
    DIAG_STR_ETHER_STATS_TX_MULTICAST_PKTS,
    DIAG_STR_ETHER_STATS_TX_FRAGMENTS,
    DIAG_STR_ETHER_STATS_TX_JABBERS,
    DIAG_STR_ETHER_STATS_TX_CRC_ALIGN_ERRORS,
    DIAG_STR_ETHER_STATS_RX_UNDER_SIZE_PKTS,
    DIAG_STR_ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS,
    DIAG_STR_ETHER_STATS_RX_OVERSIZE_PKTS,
    DIAG_STR_ETHER_STATS_RX_PKTS_64OCTETS,
    DIAG_STR_ETHER_STATS_RX_PKTS_65TO127OCTETS,
    DIAG_STR_ETHER_STATS_RX_PKTS_128TO255OCTETS,
    DIAG_STR_ETHER_STATS_RX_PKTS_256TO511OCTETS,
    DIAG_STR_ETHER_STATS_RX_PKTS_512TO1023OCTETS,
    DIAG_STR_ETHER_STATS_RX_PKTS_1024TO1518OCTETS,
    DIAG_STR_ETHER_STATS_RX_PKTS_1519TOMAXOCTETS,
    DIAG_STR_IN_OAM_PDU_PKTS,
    DIAG_STR_OUT_OAM_PDU_PKTS
};

const char *diagStr_mibLogTimer[] = {
    DIAG_STR_COUNT_MODE_FREE,
    DIAG_STR_COUNT_MODE_TIMER
};

const char *diagStr_mibGetTagLenDir[] = {
    DIAG_STR_TAG_CNT_DIR_TX,
    DIAG_STR_TAG_CNT_DIR_RX
};

const char *diagStr_mibGetTagLenState[] = {
    DIAG_STR_EXCLUDE,
    DIAG_STR_INCLUDE
};

const char *diagStr_mibRstValue[] = {
    DIAG_STR_RST_TO_0,
    DIAG_STR_RST_TO_1
};

const char *diagStr_mibSyncMode[] = {
    DIAG_STR_FREE_SYNC,
    DIAG_STR_STOP_SYNC
};

const char *diagStr_secGetTypeName[] = {
	DIAG_STR_DAEQSA_DENY,
	DIAG_STR_LAND_DENY,
	DIAG_STR_BLAT_DENY,
	DIAG_STR_XMA_DENY,
	DIAG_STR_NULLSCAN_DENY,
	DIAG_STR_SYN_SPORTL1024_DENY,
	DIAG_STR_TCPHDR_MIN_CHECK,
	DIAG_STR_TCP_FRAG_OFF_MIN_CHECK,
	DIAG_STR_ICMP_FRAG_PKTS_DENY,
	DIAG_STR_POD_DENY,
	DIAG_STR_UDPDOMB_DENY,
	DIAG_STR_SYNFIN_DENY,
    DIAG_STR_SYNWITHDATA_DENY,
    DIAG_STR_SYNFLOOD_DENY,
    DIAG_STR_FINFLOOD_DENY,
    DIAG_STR_ICMPFLOOD_DENY
};


typedef enum rtk_sec_attackType_e
{

    SYNFLOOD_DENY,
    FINFLOOD_DENY,
    ICMPFLOOD_DENY,    
    ATTACK_TYPE_END
} rtk_sec_attackType_t;




const char *diagStr_secThresholdName[] = {
    DIAG_STR_SYNFLOOD_DENY_THRESHOLD,
    DIAG_STR_FINFLOOD_DENY_THRESHOLD,
    DIAG_STR_ICMPFLOOD_DENY_THRESHOLD
};

const char *diagStr_masterSlave[] = {
    DIAG_STR_AUTO,
    DIAG_STR_SLAVE,
    DIAG_STR_MASTER,
};


const char *diagStr_lookupmissType[] = {
    DIAG_LOOKUP_MISS_IPMC,
    DIAG_LOOKUP_MISS_UCAST,
    DIAG_LOOKUP_MISS_BCAST,
    DIAG_LOOKUP_MISS_MCAST,
    DIAG_LOOKUP_MISS_IP6MC
};

const char *diagStr_gponFsmStatus[] = {
    DIAG_STR_GPON_FSM_UNKNOW,
    DIAG_STR_GPON_FSM_O1,
    DIAG_STR_GPON_FSM_O2,
    DIAG_STR_GPON_FSM_O3,
    DIAG_STR_GPON_FSM_O4,
    DIAG_STR_GPON_FSM_O5,
    DIAG_STR_GPON_FSM_O6,
    DIAG_STR_GPON_FSM_O7
};

const char *diagStr_gponFlowType[] = {
    DIAG_STR_GPON_FLOW_OMCI,
    DIAG_STR_GPON_FLOW_ETH,
    DIAG_STR_GPON_FLOW_TDM
};

const char *diagStr_gponMcForwardMode[] = {
    DIAG_STR_GPON_FORCE_NOP,
    DIAG_STR_GPON_FORCE_PASS,
    DIAG_STR_GPON_FORCE_DROP
};

const char *diagStr_gponMcMode[] = {
    DIAG_STR_GPON_MC_INCLUDE,
    DIAG_STR_GPON_MC_EXCLUDE
};

const char *diagStr_gponAlarmType[] = {
    DIAG_STR_GPON_ALARM_NONE,
    DIAG_STR_GPON_ALARM_LOS,
    DIAG_STR_GPON_ALARM_LOF,
    DIAG_STR_GPON_ALARM_LOM
};

const char *diagStr_gponLaserMode[] = {
    DIAG_STR_GPON_LASER_NORMAL,
    DIAG_STR_GPON_LASER_FS_ON,
    DIAG_STR_GPON_LASER_FS_OFF
};

const char *diagStr_ifgState[] = {
    DIAG_STR_EXCLUDE,
    DIAG_STR_INCLUDE
};


