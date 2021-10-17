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
 * $Revision: 15045 $
 * $Date: 2010-12-27 18:36:22 +0800 (?Ÿæ?ä¸€, 27 ?ä???2010) $
 *
 * Purpose : Definition of Switch Global API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Switch parameter settings
 *           (2) Management address and vlan configuration.
 *
 */

#ifndef __RTK_DEBUG_H__
#define __RTK_DEBUG_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <hal/chipdef/chip.h>

/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */

typedef struct rtk_hsb_s
{
    uint32 spa;
    uint32 user_field_15;
    uint32 user_field_14;
    uint32 user_field_13;
    uint32 user_field_12;
    uint32 user_field_11;
    uint32 user_field_10;
    uint32 user_field_9;
    uint32 user_field_8;
    uint32 user_field_7;
    uint32 user_field_6;
    uint32 user_field_5;
    uint32 user_field_4;
    uint32 user_field_3;
    uint32 user_field_2;
    uint32 user_field_1;
    uint32 user_field_0;
    uint32 user_valid;
    uint32 ip6_nh_rg;
    uint32 cks_ok_l4;
    uint32 cks_ok_l3;
    uint32 ttl_gt1;
    uint32 ttl_gt5;
    uint32 gre_if;
    uint32 icmp_if;
    uint32 udp_if;
    uint32 tcp_if;
    uint32 pppoe_session;
    ipaddr_t dip;
    ipaddr_t sip;
    uint32 tos_dscp;
    uint32 ip_type;
    uint32 ptp_if;
    uint32 omapdu;
    uint32 rlpp_if;
    uint32 rldp_if;
    uint32 llc_other;
    uint32 pppoe_if;
    uint32 snap_if;
    uint32 ether_type;
    uint32 ctag;
    uint32 ctag_if;
    uint32 stag;
    uint32 stag_if;
    uint32 cputag_dsl_vcmsk;
    uint32 cputag_pon_sid;
    uint32 cputag_l2br;
    uint32 cputag_pppoe_idx;
    uint32 cputag_pppoe_act;
    uint32 cputag_extspa;
    uint32 reserved;
    uint32 cputag_l34keep;
    uint32 reserved2;
    uint32 cputag_psel;
    uint32 cputag_dislrn;
    uint32 cputag_vsel;
    uint32 cputag_keep;
    uint32 cputag_pri;
    uint32 cputag_prisel;
    uint32 cputag_efid;
    uint32 cputag_efid_en;
    uint32 cputag_txpmsk;
    uint32 cputag_l4c;
    uint32 cputag_l3c;
    uint32 cputag_if;
    rtk_mac_t sa;
    rtk_mac_t da;
    uint32 pon_idx;
    uint32 pkt_len;
}rtk_hsb_t;

typedef struct rtk_hsa_s
{
    uint32 rng_nhsab_endsc;
    uint32 rng_nhsab_bgdsc;
    uint32 rng_nhsab_qid;
    uint32 rng_nhsab_reserve;
    uint32 rng_nhsab_cpupri;
    uint32 rng_nhsab_fwdrsn;
    uint32 rng_nhsab_pon_sid;
    uint32 rng_nhsab_vc_spa;
    uint32 rng_nhsab_vc_mask;
    uint32 rng_nhsab_ext_mask;
    uint32 rng_nhsab_dpm;
    uint32 rng_nhsab_spa;
    uint32 rng_nhsab_pktlen;
    uint32 rng_nhsac_untagset;
    uint32 rng_nhsac_ctag_act;
    uint32 rng_nhsac_vidzero;
    uint32 rng_nhsac_pritag_if;
    uint32 rng_nhsac_ctag_if;
    uint32 rng_nhsac_vid;
    uint32 rng_nhsac_cfi;
    uint32 rng_nhsac_pri;
    uint32 rng_nhsas_stag_if;
    uint32 rng_nhsas_stag_type;
    uint32 rng_nhsas_sp2s;
    uint32 rng_nhsas_dei;
    uint32 rng_nhsas_vidsel;
    uint32 rng_nhsas_frctag;
    uint32 rng_nhsas_frctag_if;
    uint32 rng_nhsas_svid;
    uint32 rng_nhsas_svidx;
    uint32 rng_nhsas_pkt_spri;
    uint32 rng_nhsas_spri;
    uint32 rng_nhsam_user_pri;
    uint32 rng_nhsam_1p_rem;
    uint32 rng_nhsam_1p_rem_en;
    uint32 rng_nhsam_dscp_rem;
    uint32 rng_nhsam_dscp_rem_en;
    uint32 rng_nhsaf_regen_crc;
    uint32 rng_hhsaf_cpukeep;
    uint32 rng_nhsaf_keep;
    uint32 rng_nhsaf_ptp;
    uint32 rng_nhsaf_tcp;
    uint32 rng_nhsaf_udp;
    uint32 rng_nhsaf_ipv4;
    uint32 rng_nhsaf_ipv6;
    uint32 rng_nhsaf_rfc1042;
    uint32 rng_nhsaf_pppoe_if;
    uint8 rng_nhsap_ptp_resv[9];
    uint32 rng_nhsap_ptp_act;
    uint8 rng_nhsap_ptp_sec[6];
    uint32 rng_nhsap_ptp_nsec;

    uint32 rng_nhsan_l3;
    uint32 rng_nhsan_org;
    uint32 rng_nhsan_ipmc;
    uint32 rng_nhsan_l2trans;
    uint32 rng_nhsan_l34trans;
    uint32 rng_nhsan_src_mode;
    uint32 rng_nhsan_pppoe_idx;
    uint32 rng_nhsan_pppoe_act;
    uint32 rng_nhsan_smac_idx;
    uint32 rng_nhsan_l3chsum;
    uint32 rng_nhsan_l4chsum;
    ipaddr_t rng_nhsan_newip;
    uint32 rng_nhsan_newport;
    rtk_mac_t rng_nhsan_newmac;
    uint32 rng_nhsan_ttlm1_extmask;
    uint32 rng_nhsan_ttlm1_pmask;
    
}rtk_hsa_t;


typedef struct rtk_hsa_debug_s
{
    uint32 hsa_debug_ep;
    uint32 hsa_debug_dsl_vc;
    uint32 hsa_debug_34pppoe;
    rtk_mac_t hsa_debug_34smac;
    uint32 hsa_debug_ttlpmsk;
    uint32 hsa_debug_ttlexmsk;
    rtk_mac_t hsa_debug_newmac;
    uint32 hsa_debug_newprt;
    uint32 hsa_debug_newip;
    uint32 hsa_debug_l4cksum;
    uint32 hsa_debug_l3cksum;
    uint32 hsa_debug_pppoeact;
    uint32 hsa_debug_src_mod;
    uint32 hsa_debug_l34trans;
    uint32 hsa_debug_l2trans;
    uint32 hsa_debug_org;
    uint32 hsa_debug_l3r;
    uint32 hsa_debug_sv_dei;
    uint32 hsa_debug_styp;
    uint32 hsa_debug_pktlen_ori;
    uint32 hsa_debug_qid;
    uint32 hsa_debug_stdsc;
    uint32 hsa_debug_cpupri;
    uint32 hsa_debug_spri;
    uint32 hsa_debug_cori;
    uint32 hsa_debug_cmdy;
    uint32 hsa_debug_crms;
    uint32 hsa_debug_cins;
    uint32 hsa_debug_cvid;
    uint32 hsa_debug_cfi;
    uint32 hsa_debug_ptpnsec;
    uint8  hsa_debug_ptpsec[6];
    uint32 hsa_debug_ptpact;
    uint32 hsa_debug_regencrc;
    uint32 hsa_debug_pppoe;
    uint32 hsa_debug_rfc1042;
    uint32 hsa_debug_ipv6;
    uint32 hsa_debug_ipv4;
    uint32 hsa_debug_ptp;
    uint32 hsa_debug_remdscp_pri;
    uint32 hsa_debug_rem1q_pri;
    uint32 hsa_debug_remdscp_en;
    uint32 hsa_debug_rem1q_en;
    uint32 hsa_debug_svid;
    uint32 hsa_debug_instag;
    uint32 hsa_debug_inctag;
    uint32 hsa_debug_pktlen;
    uint32 hsa_debug_spa;
    uint32 hsa_debug_dpc;
    uint32 hsa_debug_extmsk;
    uint32 hsa_debug_vcmsk;
    uint32 hsa_debug_ponsid;
    uint32 hsa_debug_trprsn;
    
}rtk_hsa_debug_t;



#endif /* __RTK_DEBUG_H__ */

