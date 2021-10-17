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
 *
 * Purpose : Definition those debug command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) debug
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <common/debug/rt_log.h>
#include <common/debug/mem.h>
#include <diag_util.h>
#include <diag_om.h>
#include <parser/cparser_priv.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>
#include <hal/mac/drv.h>
#include <ioal/io_mii.h>
#include <hal/common/halctrl.h>
#include <diag_str.h>
#include <rtk/acl.h>
#include <rtk/debug.h>
#include <netinet/in.h>
#include <hal/chipdef/swdef.h>
#include <apollo_reg_struct.h>
#include <hal/mac/drv.h>
#ifdef CONFIG_SDK_APOLLO
#include <dal/apollo/raw/apollo_raw_hwmisc.h>
#include <dal/apollo/raw/apollo_raw_qos.h>
#include <dal/apollo/raw/apollo_raw_flowctrl.h>
#include <dal/apollo/raw/apollo_raw_vlan.h>
#endif


#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_qos.h>
#endif

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
#include <rtk/rtusr/rtusr_pkt.h>
#endif /* defined(CONFIG_LINUX_USER_SHELL) */

const char *diagStr_hsbLatchModeStr[] = {
    DIAG_STR_All,
    DIAG_STR_NONE,
    DIAG_STR_FIRSTDROP,
    DIAG_STR_FIRSTPASS,
    DIAG_STR_FIRSTTRAP2CPU,
    DIAG_STR_DROP,
    DIAG_STR_TRAP2CPU,
    DIAG_STR_ACL,

};



static int32 _diag_debug_internal_phy_read(
    uint32      phyId,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData)
{
    int32 ret;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            ret = apollo_interPhy_read(phyId, page, phyRegAddr, pData);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            ret = apollomp_interPhy_read(phyId, page, phyRegAddr, pData);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return RT_ERR_FAILED;
            break;
    }

    return ret;
}



static int32 _diag_debug_internal_phy_write(
    uint32      phyId,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data)
{
    int32 ret;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            ret = apollo_interPhy_write(phyId, page, phyRegAddr, data);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            ret = apollomp_interPhy_write(phyId, page, phyRegAddr, data);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return RT_ERR_FAILED;
            break;
    }

    return ret;
}


void _diag_debug_hsb_display(rtk_hsb_t *hsbData)
{
    diag_util_printf("spa: %d pktLen: %d ponIdx: %d\n",
                                hsbData->spa,
                                hsbData->pkt_len,
                                hsbData->pon_idx);

    diag_util_printf("da: %s ",
                        diag_util_inet_mactoa(&hsbData->da.octet[0]));
    diag_util_printf("sa: %s etherType: 0x%4.4x\n",
                        diag_util_inet_mactoa(&hsbData->sa.octet[0]),
                        hsbData->ether_type);

    diag_util_printf("ctag: %d pri: %d cfi: %d vid: %d\n",
                        hsbData->ctag_if,
                        (hsbData->ctag&0xE000)>>13,
                        (hsbData->ctag&0x1000)>>12,
                        hsbData->ctag&0xFFF);

    diag_util_printf("stag: %d pri: %d cfi: %d vid: %d\n",
                        hsbData->stag_if,
                        (hsbData->stag&0xE000)>>13,
                        (hsbData->stag&0x1000)>>12,
                        hsbData->stag&0xFFF);

    diag_util_printf("dip: %s ",
                        diag_util_inet_ntoa(hsbData->dip));
    diag_util_printf("sip: %s iptype: %d tos_dscp: 0x%2.2x\n",
                        diag_util_inet_ntoa(hsbData->sip),
                        hsbData->ip_type,
                        hsbData->tos_dscp);
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("l4ok l3ok gt1 gt5 gre icmp udp tcp\n");
            diag_util_printf("%-4d %-4d %-3d %-3d %-3d %-4d %-3d %-3d\n",
                                hsbData->cks_ok_l4,
                                hsbData->cks_ok_l3,
                                hsbData->ttl_gt1,
                                hsbData->ttl_gt5,
                                hsbData->gre_if,
                                hsbData->icmp_if,
                                hsbData->udp_if,
                                hsbData->tcp_if);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("ip6hnrg l4ok l3ok gt1 gt5 gre icmp udp tcp\n");
            diag_util_printf("%-7d %-4d %-4d %-3d %-3d %-3d %-4d %-3d %-3d\n",
                                hsbData->ip6_nh_rg,
                                hsbData->cks_ok_l4,
                                hsbData->cks_ok_l3,
                                hsbData->ttl_gt1,
                                hsbData->ttl_gt5,
                                hsbData->gre_if,
                                hsbData->icmp_if,
                                hsbData->udp_if,
                                hsbData->tcp_if);
            break;
#endif
    }

    diag_util_printf("ptp oam rlpp rldp llc snap pppoe session\n");
    diag_util_printf("%-3d %-3d %-4d %-4d %-3d %-4d %-5d 0x%4.4x\n",
                        hsbData->ptp_if,
                        hsbData->omapdu,
                        hsbData->rlpp_if,
                        hsbData->rldp_if,
                        hsbData->llc_other,
                        hsbData->snap_if,
                        hsbData->pppoe_if,
                        hsbData->pppoe_session);



    diag_util_printf("userfield valid: 0x%4.4x\n",hsbData->user_valid);
    diag_util_printf("00-07: 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x\n",
                        hsbData->user_field_0,
                        hsbData->user_field_1,
                        hsbData->user_field_2,
                        hsbData->user_field_3,
                        hsbData->user_field_4,
                        hsbData->user_field_5,
                        hsbData->user_field_6,
                        hsbData->user_field_7);
    diag_util_printf("08-15: 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x 0x%4.4x\n",
                        hsbData->user_field_8,
                        hsbData->user_field_9,
                        hsbData->user_field_10,
                        hsbData->user_field_11,
                        hsbData->user_field_12,
                        hsbData->user_field_13,
                        hsbData->user_field_14,
                        hsbData->user_field_15);

    if(hsbData->cputag_if)
    {
        diag_util_printf("aware cpu-tag fields\n");
        diag_util_printf("l3c l4c txpmsk efidEn efid priSel pri keep vsel dislrn\n");
        diag_util_printf("%-3d %-3d 0x%2.2x   %-6d %-4d %-6d %-3d %-4d %-4d %d\n",
                        hsbData->cputag_l3c,
                        hsbData->cputag_l4c,
                        hsbData->cputag_txpmsk,
                        hsbData->cputag_efid_en,
                        hsbData->cputag_efid,
                        hsbData->cputag_prisel,
                        hsbData->cputag_pri,
                        hsbData->cputag_keep,
                        hsbData->cputag_vsel,
                        hsbData->cputag_dislrn);

        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                diag_util_printf("psel extspa pppoe_act pppoe_idx l2br pon_sid dsl_vcmsk\n");
                diag_util_printf("%-4d %-6d %-9d %-9d %-4d %-7d 0x%4.4x\n",
                                hsbData->cputag_psel,
                                hsbData->cputag_extspa,
                                hsbData->cputag_pppoe_act,
                                hsbData->cputag_pppoe_idx,
                                hsbData->cputag_l2br,
                                hsbData->cputag_pon_sid,
                                hsbData->cputag_dsl_vcmsk);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("psel l34keep extspa pppoe_act pppoe_idx l2br pon_sid\n");
                diag_util_printf("%-4d %-7d %-6d %-9d %-9d %-4d %-7d\n",
                                hsbData->cputag_psel,
                                hsbData->cputag_l34keep,
                                hsbData->cputag_extspa,
                                hsbData->cputag_pppoe_act,
                                hsbData->cputag_pppoe_idx,
                                hsbData->cputag_l2br,
                                hsbData->cputag_pon_sid);
                break;
#endif
        }


    }
}

void _diag_debug_hsa_display(rtk_hsa_t *hsaData)
{
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("Port      CPU  5    4    PON  2    1    0\n");
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("Port      CPU  5    PON  3    2    1    0\n");
            break;
#endif
    }



    diag_util_printf("user_pri: %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsam_user_pri>>18)&7,
                                (hsaData->rng_nhsam_user_pri>>15)&7,
                                (hsaData->rng_nhsam_user_pri>>12)&7,
                                (hsaData->rng_nhsam_user_pri>>9)&7,
                                (hsaData->rng_nhsam_user_pri>>6)&7,
                                (hsaData->rng_nhsam_user_pri>>3)&7,
                                hsaData->rng_nhsam_user_pri&7);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("qid:      %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsab_qid>>22)&7,
                                        (hsaData->rng_nhsab_qid>>19)&7,
                                        (hsaData->rng_nhsab_qid>>16)&7,
                                        (hsaData->rng_nhsab_qid>>9)&0x7F,
                                        (hsaData->rng_nhsab_qid>>6)&7,
                                        (hsaData->rng_nhsab_qid>>3)&7,
                                        hsaData->rng_nhsab_qid&7);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("qid:      %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                        (hsaData->rng_nhsab_qid>>22)&7,
                                        (hsaData->rng_nhsab_qid>>19)&7,
                                        (hsaData->rng_nhsab_qid>>12)&0x7F,
                                        (hsaData->rng_nhsab_qid>>9)&7,
                                        (hsaData->rng_nhsab_qid>>6)&7,
                                        (hsaData->rng_nhsab_qid>>3)&7,
                                        hsaData->rng_nhsab_qid&7);
            break;
#endif
    }


    diag_util_printf("dmp:      %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsab_dpm>>12)&3,
                                (hsaData->rng_nhsab_dpm>>10)&3,
                                (hsaData->rng_nhsab_dpm>>8)&3,
                                (hsaData->rng_nhsab_dpm>>6)&3,
                                (hsaData->rng_nhsab_dpm>>4)&3,
                                (hsaData->rng_nhsab_dpm>>2)&3,
                                hsaData->rng_nhsab_dpm&3);

    diag_util_printf("untagset: %-4d %-4d %-4d %-4d %-4d %-4d %-4d\n",
                                (hsaData->rng_nhsac_untagset>>6)&1,
                                (hsaData->rng_nhsac_untagset>>5)&1,
                                (hsaData->rng_nhsac_untagset>>4)&1,
                                (hsaData->rng_nhsac_untagset>>3)&1,
                                (hsaData->rng_nhsac_untagset>>2)&1,
                                (hsaData->rng_nhsac_untagset>>1)&1,
                                hsaData->rng_nhsac_untagset&1);

    diag_util_printf("spa ctag_act tag_if vid cfi pri vidzero\n");
    diag_util_printf("%-3d %-8d %-6d %-3d %-3d %-3d %-7d\n",
                                hsaData->rng_nhsab_spa,
                                hsaData->rng_nhsac_ctag_act,
                                hsaData->rng_nhsac_ctag_if,
                                hsaData->rng_nhsac_vid,
                                hsaData->rng_nhsac_cfi,
                                hsaData->rng_nhsac_pri,
                                hsaData->rng_nhsac_vidzero);


    diag_util_printf("stag_type stag_if sp2s svid svidx dei spri pkt_spri vidsel frctag frctag_if\n");
    diag_util_printf("%-9d %-7d %-4d %-4d %-5d %-3d %-4d %-8d %-6d %-6d %-9d\n",
                                hsaData->rng_nhsas_stag_type,
                                hsaData->rng_nhsas_stag_if,
                                hsaData->rng_nhsas_sp2s,
                                hsaData->rng_nhsas_svid,
                                hsaData->rng_nhsas_svidx,
                                hsaData->rng_nhsas_dei,
                                hsaData->rng_nhsas_spri,
                                hsaData->rng_nhsas_pkt_spri,
                                hsaData->rng_nhsas_vidsel,
                                hsaData->rng_nhsas_frctag,
                                hsaData->rng_nhsas_frctag_if);
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("1p_rem 1p_rem_en dscp_rem dscp_rem_en keep ptp ipv4 ipv6 1042 pppoe\n");
            diag_util_printf("%-6d %-9d %-8d %-11d %-4d %-3d %-4d %-4d %-4d %-5d\n",
                                        hsaData->rng_nhsam_1p_rem,
                                        hsaData->rng_nhsam_1p_rem_en,
                                        hsaData->rng_nhsam_dscp_rem,
                                        hsaData->rng_nhsam_dscp_rem_en,
                                        hsaData->rng_nhsaf_keep,
                                        hsaData->rng_nhsaf_ptp,
                                        hsaData->rng_nhsaf_ipv4,
                                        hsaData->rng_nhsaf_ipv6,
                                        hsaData->rng_nhsaf_rfc1042,
                                        hsaData->rng_nhsaf_pppoe_if);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("1prem 1premen dscprem dscpremen cpukeep keep ptp tcp udp ipv4 ipv6 1042 pppoe\n");
            diag_util_printf("%-5d %-7d %-7d %-9d %-7d %-4d %-3d %-3d %-3d %-4d %-4d %-4d %-5d\n",
                                        hsaData->rng_nhsam_1p_rem,
                                        hsaData->rng_nhsam_1p_rem_en,
                                        hsaData->rng_nhsam_dscp_rem,
                                        hsaData->rng_nhsam_dscp_rem_en,
                                        hsaData->rng_hhsaf_cpukeep,
                                        hsaData->rng_nhsaf_keep,
                                        hsaData->rng_nhsaf_ptp,
                                        hsaData->rng_nhsaf_tcp,
                                        hsaData->rng_nhsaf_udp,
                                        hsaData->rng_nhsaf_ipv4,
                                        hsaData->rng_nhsaf_ipv6,
                                        hsaData->rng_nhsaf_rfc1042,
                                        hsaData->rng_nhsaf_pppoe_if);

            break;
#endif
    }



    diag_util_printf("endsc bgdsc cpupri fwdrsn pon_sid pktlen regen_crc\n");
    diag_util_printf("%-5d %-5d %-6d %-6d %-7d %-6d %-d\n",
                                hsaData->rng_nhsab_endsc,
                                hsaData->rng_nhsab_bgdsc,
                                hsaData->rng_nhsab_cpupri,
                                hsaData->rng_nhsab_fwdrsn,
                                hsaData->rng_nhsab_pon_sid,
                                hsaData->rng_nhsab_pktlen,
                                hsaData->rng_nhsaf_regen_crc);


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            diag_util_printf("vc_spa: %d vc_mask: 0x%4.4x ext_mask: 0x%4.4x\n",
                                        hsaData->rng_nhsab_vc_spa,
                                        hsaData->rng_nhsab_vc_mask,
                                        hsaData->rng_nhsab_ext_mask);

            diag_util_printf("l3: %d org: %d l2trans: %d l34trans: %d src_mode: %d l3chsum: %d l4schsum %d\n",
                                        hsaData->rng_nhsan_l3,
                                        hsaData->rng_nhsan_org,
                                        hsaData->rng_nhsan_l2trans,
                                        hsaData->rng_nhsan_l34trans,
                                        hsaData->rng_nhsan_src_mode,
                                        hsaData->rng_nhsan_l3chsum,
                                        hsaData->rng_nhsan_l4chsum);

            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            diag_util_printf("ext_mask: 0x%4.4x\n",
                                        hsaData->rng_nhsab_ext_mask);

            diag_util_printf("l3: %d org: %d ipmc: %d l2trans: %d l34trans: %d src_mode: %d l3chsum: %d l4schsum %d\n",
                                        hsaData->rng_nhsan_l3,
                                        hsaData->rng_nhsan_org,
                                        hsaData->rng_nhsan_ipmc,
                                        hsaData->rng_nhsan_l2trans,
                                        hsaData->rng_nhsan_l34trans,
                                        hsaData->rng_nhsan_src_mode,
                                        hsaData->rng_nhsan_l3chsum,
                                        hsaData->rng_nhsan_l4chsum);

            break;
#endif
    }


    diag_util_printf("pppoe_idx: %d pppoe_act: %d ttl_extmask: 0x%2.2x ttl_pmask: 0x%2.2x\n",
                                hsaData->rng_nhsan_pppoe_idx,
                                hsaData->rng_nhsan_pppoe_act,
                                hsaData->rng_nhsan_ttlm1_extmask,
                                hsaData->rng_nhsan_ttlm1_pmask);

    diag_util_printf("newmac: %s smac_idx: %d ",
                        diag_util_inet_mactoa(&hsaData->rng_nhsan_newmac.octet[0]),
                        hsaData->rng_nhsan_smac_idx);
    diag_util_printf("newip: %s newport: %d\n",
                        diag_util_inet_ntoa(hsaData->rng_nhsan_newip),
                        hsaData->rng_nhsan_newport);
}

void _diag_debug_hsd_display(rtk_hsa_debug_t *hsdData)
{
    diag_util_printf("newmac: %s ",
                        diag_util_inet_mactoa(&hsdData->hsa_debug_newmac.octet[0]));

    diag_util_printf("l34mac: %s ",
                        diag_util_inet_mactoa(&hsdData->hsa_debug_34smac.octet[0]));

    diag_util_printf("newip: %s newprt: %d\n",
                        diag_util_inet_ntoa(hsdData->hsa_debug_newip),hsdData->hsa_debug_newprt);

    diag_util_printf("ep dsl_vc 34pppoe ttlpmsk ttlexmsk l4cksum l3cksum pppoeact\n");
    diag_util_printf("%-2d %-6d %-7d 0x%-5.5x 0x%-6.6x %-7d %-7d %-8d\n",
                        hsdData->hsa_debug_ep,
                        hsdData->hsa_debug_dsl_vc,
                        hsdData->hsa_debug_34pppoe,
                        hsdData->hsa_debug_ttlpmsk,
                        hsdData->hsa_debug_ttlexmsk,
                        hsdData->hsa_debug_l4cksum,
                        hsdData->hsa_debug_l3cksum,
                        hsdData->hsa_debug_pppoeact);

    diag_util_printf("src_mod l34trans l2trans org l3r sv_dei styp pktlen_ori qid\n");
    diag_util_printf("%-7d %-8d %-7d %-3d %-3d %-6d %-4d %-10d %-3d\n",
                        hsdData->hsa_debug_src_mod,
                        hsdData->hsa_debug_l34trans,
                        hsdData->hsa_debug_l2trans,
                        hsdData->hsa_debug_org,
                        hsdData->hsa_debug_l3r,
                        hsdData->hsa_debug_sv_dei,
                        hsdData->hsa_debug_styp,
                        hsdData->hsa_debug_pktlen_ori,
                        hsdData->hsa_debug_qid);
    diag_util_printf("stdsc cpupri spri cori cmdy crms cins cvid cfi regencrc pppoe\n");
    diag_util_printf("%-5d %-6d %-4d %-4d %-4d %-4d %-4d %-4d %-3d %-8d %-5d\n",
                        hsdData->hsa_debug_stdsc,
                        hsdData->hsa_debug_cpupri,
                        hsdData->hsa_debug_spri,
                        hsdData->hsa_debug_cori,
                        hsdData->hsa_debug_cmdy,
                        hsdData->hsa_debug_crms,
                        hsdData->hsa_debug_cins,
                        hsdData->hsa_debug_cvid,
                        hsdData->hsa_debug_cfi,
                        hsdData->hsa_debug_regencrc,
                        hsdData->hsa_debug_pppoe);
    diag_util_printf("rfc1042 ipv6 ipv4 ptp remdscp_pri rem1q_pri remdscp_en rem1q_en\n");
    diag_util_printf("%-7d %-4d %-4d %-3d %-7d %-3d %-9d %-10d %-9d\n",
                        hsdData->hsa_debug_rfc1042,
                        hsdData->hsa_debug_ipv6,
                        hsdData->hsa_debug_ipv4,
                        hsdData->hsa_debug_ptp,
                        hsdData->hsa_debug_remdscp_pri,
                        hsdData->hsa_debug_rem1q_pri,
                        hsdData->hsa_debug_remdscp_en,
                        hsdData->hsa_debug_rem1q_en);
    diag_util_printf("svid instag inctag pktlen spa dpc extmsk vcmsk ponsid trprsn\n");
    diag_util_printf("%-4d %-6d %-6d %-6d %-3d %-3d 0x%-4.4x 0x%-3.3x %-6d %-6d\n",
                        hsdData->hsa_debug_svid,
                        hsdData->hsa_debug_instag,
                        hsdData->hsa_debug_inctag,
                        hsdData->hsa_debug_pktlen,
                        hsdData->hsa_debug_spa,
                        hsdData->hsa_debug_dpc,
                        hsdData->hsa_debug_extmsk,
                        hsdData->hsa_debug_vcmsk,
                        hsdData->hsa_debug_ponsid,
                        hsdData->hsa_debug_trprsn);
}

#define CF_HIT_FIELD_MASK 		0x200
#define CF_HIT_VALUE_MASK		0x1ff
#define FIELD_SIZE 10

cparser_result_t
cparser_cmd_debug_classf_get_hit_entry(
    cparser_context_t *context)
{
	int32 ret,i;
	uint32 value=0,reg=0,field=0;
	uint32 index,arraySize=0;
	int32 portIndex=REG_ARRAY_INDEX_NONE,arrayIndex=REG_ARRAY_INDEX_NONE ;
	
    	DIAG_UTIL_PARAM_CHK();
    	DIAG_UTIL_OUTPUT_INIT();	

	switch(DIAG_UTIL_CHIP_TYPE)
	{
#if defined(CONFIG_SDK_APOLLO)
	case APOLLO_CHIP_ID:
		reg = STAT_PRVTE_DROP_COUNTERr;
		field= PKT_INFOf;
		portIndex = 8;
		arraySize = 1;
	break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
	case APOLLOMP_CHIP_ID:
		/*This register is for CF hit check*/
	   	reg = APOLLOMP_STAT_CF_REASONr;
		field= APOLLOMP_CF_HIT_INFOf;
		arrayIndex = 0;
		arraySize = 2;
	break;
#endif
	default:
	break;
	}


	for(i=0; i<arraySize; i++)
	{
		arrayIndex+=i;
		DIAG_UTIL_ERR_CHK(reg_array_field_read(reg, portIndex,arrayIndex,field, &value), ret);
		diag_util_printf("Field: %d, Value: 0x%x\n",i,value);
		/*check field hit or not*/
		if(value & CF_HIT_FIELD_MASK)
		{
			index = value & CF_HIT_VALUE_MASK;
			diag_util_printf("classification hit entry: %d\n",index);

		}
		else
		{
			diag_util_printf("classification hit entry: none\n");
		}
	}

    	return CPARSER_OK;
}    /* end of cparser_cmd_debug_classf_get_hit_entry */



/*
 * debug get log
 */
cparser_result_t cparser_cmd_debug_get_log(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  data = 0;
    uint64  data64 = 0;
    uint32  i = 0;
    uint32  log_type = LOG_TYPE_DEFAULT;
    int32   ret = RT_ERR_FAILED;

    char *pLevelName[] = {
        "fatal", "major", "minor", "warning", "event", "info",
        "func", "debug", "trace", ""
    };

    char *pModName[] = {
        STR_MOD_GENERAL, STR_MOD_DOT1X, STR_MOD_FILTER, STR_MOD_FLOWCTRL,
        STR_MOD_INIT, STR_MOD_L2, STR_MOD_MIRROR, STR_MOD_NIC, STR_MOD_PORT,
        STR_MOD_QOS, STR_MOD_RATE, STR_MOD_STAT, STR_MOD_STP, STR_MOD_SVLAN,
        STR_MOD_SWITCH, STR_MOD_TRAP, STR_MOD_TRUNK, STR_MOD_VLAN, STR_MOD_ACL,
        STR_MOD_HAL, STR_MOD_DAL, STR_MOD_RTDRV, STR_MOD_RTUSR, STR_MOD_DIAGSHELL,
        STR_MOD_UNITTEST, STR_MOD_OAM, STR_MOD_L3, STR_MOD_RTCORE, STR_MOD_EEE,
        STR_MOD_SEC, STR_MOD_LED, STR_MOD_RSVD_001, STR_MOD_RSVD_002, STR_MOD_RSVD_003,
        STR_MOD_END
    };

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(rt_log_enable_get(&data), ret);
    if (data < RTK_ENABLE_END)
        diag_util_printf("    status      : %s \n", data ? "ENABLE" : "DISABLE");
    else
        diag_util_printf("    status      : ERROR \n");

    DIAG_UTIL_ERR_CHK(rt_log_type_get(&log_type), ret);
    if (log_type < LOG_TYPE_END)
        diag_util_printf("    type        : %s \n", log_type ? "LEVEL-MASK" : "LEVEL");
    else
        diag_util_printf("    type        : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_level_get(&data), ret);
    if (data < LOG_LV_END)
    {
        if (LOG_MSG_OFF == data)
        {
            diag_util_printf("    level       : Message off ");
        }
        else
        {
            diag_util_printf("    level       : %d ", data);
        }
        if (LOG_TYPE_LEVEL == log_type)
            diag_util_printf("(*)");
        diag_util_printf("\n");
    }
    else
        diag_util_printf("    level       : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_mask_get(&data), ret);
    if (data <= LOG_MASK_ALL)
    {
        diag_util_printf("    level-mask  : ");
        if (data)
        {
            for (i = 0; i < LOG_MSG_OFF; i++)
            {
                if ((data >> i) & 0x1)
                    diag_util_printf("%s ", *(pLevelName + i));
            }
        }
        else
            diag_util_printf("ALL_MSG_OFF");

        if (LOG_TYPE_MASK == log_type)
            diag_util_printf("(*)");
        diag_util_printf("\n");
    }
    else
        diag_util_printf("    level-mask  : ERROR \n");

    data = 0;
    DIAG_UTIL_ERR_CHK(rt_log_format_get(&data), ret);
    if (data < LOG_FORMAT_END)
        diag_util_printf("    format      : %s \n", data ? "DETAILED" : "NORMAL");
    else
        diag_util_printf("    format      : ERROR \n");

    data64 = 0;
    DIAG_UTIL_ERR_CHK(rt_log_moduleMask_get(&data64), ret);
    if (data64 <= MOD_ALL)
    {
        diag_util_printf("    module-mask : ");
        if (data64)
        {
            for (i = 0; i < SDK_MOD_END; i++)
            {
                if ((data64 >> i) & 0x1)
                    diag_util_printf("%s ", *(pModName + i));
            }
        }
        else
            diag_util_printf("ALL_MODULE_OFF");
        diag_util_printf("\n\n");
    }
    else
        diag_util_printf("    module-mask : ERROR \n");
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_get_log */

/*
 * debug get memory <UINT:address> { <UINT:words> }
 */
cparser_result_t cparser_cmd_debug_get_memory_address_words(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *words_ptr)
{
    uint32  mem = 0;
    uint32  value = 0;
    uint32  mem_words = 0;
    uint32  index = 0;
    int32   ret = RT_ERR_FAILED;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    mem = *address_ptr;
    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }


    if ('\0' == TOKEN_CHAR(4,0))
    {

        DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &value), ret);
        diag_util_mprintf("Memory 0x%x : 0x%08x\n", mem, value);
    }
    else
    {

        mem_words = *words_ptr;
        for (index = 0; index < mem_words; index++)
        {
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &value), ret);
            if (0 == (index % 4))
            {
                diag_util_mprintf("\n");
                diag_util_printf("0x%08x ", mem);
            }
            diag_util_printf("0x%08x ", value);
            mem = mem + 4;
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
} /* end of cparser_cmd_debug_get_memory_address_words */

/*
 * debug set log state ( disable | enable )
 */
cparser_result_t cparser_cmd_debug_set_log_state_disable_enable(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('e' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_enable_set(ENABLED), ret);
    }
    else if ('d' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_enable_set(DISABLED), ret);
    }
    else {}
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_state_disable_enable */

/*
 * debug set log level <UINT:value>
 */
cparser_result_t cparser_cmd_debug_set_log_level_value(cparser_context_t *context,
    uint32_t *value_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  log_level = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    log_level = *value_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_level_set(log_level), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_value */

/*
 * debug set log level-mask <UINT:bitmask>
 */
cparser_result_t cparser_cmd_debug_set_log_level_mask_bitmask(cparser_context_t *context,
    uint32_t *bitmask_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint32  log_level_mask = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    log_level_mask = *bitmask_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_mask_set(log_level_mask), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_mask_bitmask */

/*
 * debug set log level-type ( level | level-mask )
 */
cparser_result_t cparser_cmd_debug_set_log_level_type_level_level_mask(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if (strlen(TOKEN_STR(4)) == strlen("level"))
    {
        DIAG_UTIL_ERR_CHK(rt_log_type_set(LOG_TYPE_LEVEL), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rt_log_type_set(LOG_TYPE_MASK), ret);
    }
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_level_type_level_level_mask */

/*
 * debug set log format ( normal | detail )
 */
cparser_result_t cparser_cmd_debug_set_log_format_normal_detail(cparser_context_t *context)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        DIAG_UTIL_ERR_CHK(rt_log_format_set(LOG_FORMAT_DETAILED), ret);
    }
    else
    {
        DIAG_UTIL_ERR_CHK(rt_log_format_set(LOG_FORMAT_NORMAL), ret);
    }
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_format_normal_detail */

/*
 * debug set log module <UINT64:bitmask>
 */
cparser_result_t cparser_cmd_debug_set_log_module_bitmask(cparser_context_t *context,
    uint64_t *bitmask_ptr)
{
#if defined(CONFIG_SDK_DEBUG)
    uint32  unit = 0;
    uint64  log_module_mask = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    log_module_mask = *bitmask_ptr;
    DIAG_UTIL_ERR_CHK(rt_log_moduleMask_set(log_module_mask), ret);
#endif
    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_log_module_bitmask */

/*
 * debug set memory <UINT:address> <UINT:value>
 */
cparser_result_t cparser_cmd_debug_set_memory_address_value(cparser_context_t *context,
    uint32_t *address_ptr, uint32_t *value_ptr)
{
    uint32  mem = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();

    mem = *address_ptr;
    value = *value_ptr;

    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(ioal_mem32_write(mem, value), ret);

    return CPARSER_OK;
} /* end of cparser_cmd_debug_set_memory_address_value */

/*
 * debug set soc-memory <UINT:address> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_soc_memory_address_value(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *value_ptr)
{
    uint32  mem = 0;
    uint32  value  = 0;
    int32   ret = RT_ERR_FAILED;

    mem = *address_ptr;
    value = *value_ptr;

    if(((mem&0xff000000) != 0xB8000000) && ((mem&0xff000000) != 0xBB000000)) 
    {
        diag_util_printf("\n\rInput error! The address base must be 0xB8000000 or 0xBB000000.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

#if defined(RTL_CYGWIN_EMULATE)
    {
        diag_util_printf("\n\rsoc memory access not support in this mode.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#endif
    
#if 0
    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#endif
    DIAG_UTIL_ERR_CHK(ioal_socMem32_write(mem, value), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_soc_memory_address_value */

/*
 * debug get soc-memory <UINT:address> { <UINT:words> }
 */
cparser_result_t
cparser_cmd_debug_get_soc_memory_address_words(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *words_ptr)
{
    uint32  mem = 0;
    uint32  value = 0;
    uint32  mem_words = 0;
    uint32  index = 0;
    int32   ret = RT_ERR_FAILED;

    /* Don't check the (NULL == words_ptr) due to it is optional token */
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    mem = *address_ptr;
    
    if(((mem&0xff000000) != 0xB8000000) && ((mem&0xff000000) != 0xBB000000)) 
    {
        diag_util_printf("\n\rInput error! The address base must be 0xB8000000 or 0xBB000000.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

#if defined(RTL_CYGWIN_EMULATE)
    {
        diag_util_printf("\n\rsoc memory access not support in this mode.\n\r\n\r");
        return CPARSER_NOT_OK;
    }
#endif

    
    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }


    if ('\0' == TOKEN_CHAR(4,0))
    {

        DIAG_UTIL_ERR_CHK(ioal_socMem32_read(mem, &value), ret);
        diag_util_mprintf("Memory 0x%x : 0x%08x\n", mem, value);
    }
    else
    {

        mem_words = *words_ptr;
        for (index = 0; index < mem_words; index++)
        {
            DIAG_UTIL_ERR_CHK(ioal_socMem32_read(mem, &value), ret);
            if (0 == (index % 4))
            {
                diag_util_mprintf("\n");
                diag_util_printf("0x%08x ", mem);
            }
            diag_util_printf("0x%08x ", value);
            mem = mem + 4;
        }
        diag_util_mprintf("\n");
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_soc_memory_address_words */


typedef union debug_batch_cmd_u
{
    /* A type operation - loop */
    struct
    {
        uint32 count;   /* loop count */
        uint16 pos;     /* Loop command apear pos */
    } loop;
    /* W type operation - write */
    struct
    {
        uint8 reg;      /* target register */
        uint8 msb;      /* msb of operation */
        uint8 lsb;      /* lsb of operation */
        uint16 page;    /* page to do the operation */
        uint16  data;   /* data for operation */
    } write;
    /* R type operation - read */
    struct
    {
        uint8 reg;      /* target register */
        uint8 msb;      /* msb of operation */
        uint8 lsb;      /* lsb of operation */
        uint16 page;    /* page to do the operation */
    } read;
} debug_batch_cmd_t;

typedef struct debug_batch_rec_s
{
    uint8 cmdType;
    debug_batch_cmd_t cmd;
    struct debug_batch_rec_s *pNext;
} debug_batch_rec_t;

#define DEBUG_BATCH_OP_LOOP     0
#define DEBUG_BATCH_OP_WRITE    1
#define DEBUG_BATCH_OP_READ     2
#define DEBUG_BATCH_OP_PHY      3
#define DEBUG_BATCH_OP_LIST     4
#define DEBUG_BATCH_OP_EXECUTE  5

static uint8 _diag_debug_batch_print = 0;
static uint16 phyId = 0;
static debug_batch_rec_t *pListHead = NULL;
static debug_batch_rec_t *pListTail = NULL;

static int32
_diag_debug_batch_parse_cmd(
    uint8 opType,
    uint32_t var1,
    uint32_t var2,
    uint32_t var3,
    uint32_t data)
{
    int32 ret;
    debug_batch_rec_t *pRec = NULL;
    
    if(_diag_debug_batch_print != 0) diag_util_printf("read cmds:\n");
    
    pRec = osal_alloc(sizeof(debug_batch_rec_t));
    if(NULL == pRec)
    {
        diag_util_printf("%s:%d allocate fail\n", __FUNCTION__, __LINE__);
        goto parser_panic;
    }
    memset(pRec, 0, sizeof(debug_batch_rec_t));

    if(NULL == pListHead)
    {
        pListHead = pRec;
        pListTail = pRec;
    }
    else
    {
        pListTail->pNext = pRec;
        pListTail = pRec;
    }

    /* should be the cmd character */
    switch(opType)
    {
    case DEBUG_BATCH_OP_LOOP:
        pRec->cmdType = DEBUG_BATCH_OP_LOOP;
        pRec->cmd.loop.count = var1;

        if(_diag_debug_batch_print != 0) diag_util_printf("A %d\n", var1);
        break;

    case DEBUG_BATCH_OP_WRITE:
        pRec->cmdType = DEBUG_BATCH_OP_WRITE;

        if(var3 > var2)
        {
            diag_util_printf("lsb > msb\n");
            goto parser_panic;
        }
        pRec->cmd.write.reg = var1;
        pRec->cmd.write.msb = var2;
        pRec->cmd.write.lsb = var3;
        pRec->cmd.write.data = data;

        if(var1 <= 7)
            pRec->cmd.write.page = 0x0a40;
        else if(var1 <= 15)
            pRec->cmd.write.page = 0x0a41;
        else if(var1 <= 23)
            pRec->cmd.write.page = 0x0a42;
        else if(var1 <= 31)
            pRec->cmd.write.page = 0x0a43;

        if(_diag_debug_batch_print != 0)
        {
            diag_util_printf("W %2d(0x%04x) %2d %2d 0x%x\n",
            var1, pRec->cmd.write.page, var2, var3, data);
        }
        break;

    case DEBUG_BATCH_OP_READ:
        pRec->cmdType = DEBUG_BATCH_OP_READ;

        if(var3 > var2)
        {
            diag_util_printf("lsb > msb\n");
            goto parser_panic;
        }
        pRec->cmd.read.reg = var1;
        pRec->cmd.read.msb = var2;
        pRec->cmd.read.lsb = var3;

        if(var1 <= 7)
            pRec->cmd.read.page = 0x0a40;
        else if(var1 <= 15)
            pRec->cmd.read.page = 0x0a41;
        else if(var1 <= 23)
            pRec->cmd.read.page = 0x0a42;
        else if(var1 <= 31)
            pRec->cmd.read.page = 0x0a43;

        if(_diag_debug_batch_print != 0)
        {
            diag_util_printf("R %2d(0x%04x) %2d %2d\n",
            var1, pRec->cmd.read.page, var2, var3);
        }
        break;

    default:
        diag_util_printf("parser panic for unknown operation %d\n", opType);
        goto parser_panic;
    }

    return CPARSER_OK;

parser_panic:
    return CPARSER_NOT_OK;
} /* end of _diag_debug_batch_parse_cmd */

static int32
_diag_debug_batch_analysis()
{
    uint16 lastPage = 0x0a42;
    debug_batch_rec_t *pCurr = pListHead;
    
    /* Due to Apollo need additional page switch 
     * This function analysis the list and set page to the command
     */
    if(_diag_debug_batch_print != 0) diag_util_printf("analyzed cmds:\n");
    while(pCurr != NULL)
    {
        switch(pCurr->cmdType)
        {
        case DEBUG_BATCH_OP_LOOP:
            /* Nothing to do with the loop operation */
            if(_diag_debug_batch_print != 0) 
                diag_util_printf("A %d\n", pCurr->cmd.loop.count);
            break;
        case DEBUG_BATCH_OP_WRITE:
            /* Write to page switch register */
            if(31 == pCurr->cmd.write.reg)
            {
                /* Update last switched page */
                lastPage = pCurr->cmd.write.data;
            }
            else if(pCurr->cmd.write.reg >= 16 && pCurr->cmd.write.reg <= 23 )
            {
                /* Update write page */
                pCurr->cmd.write.page = lastPage;
            }
            if(_diag_debug_batch_print != 0)
            {
                diag_util_printf("W %2d(0x%04x) %2d %2d 0x%x\n",
                                 pCurr->cmd.write.reg,
                                 pCurr->cmd.write.page,
                                 pCurr->cmd.write.msb,
                                 pCurr->cmd.write.lsb,
                                 pCurr->cmd.write.data);
            }
            break;
        case DEBUG_BATCH_OP_READ:
            if(pCurr->cmd.read.reg >= 16 && pCurr->cmd.read.reg <= 23 )
            {
                /* Update read page */
                pCurr->cmd.read.page = lastPage;
            }
            if(_diag_debug_batch_print != 0)
            {
                diag_util_printf("R %2d(0x%04x) %2d %2d\n",
                                 pCurr->cmd.read.reg,
                                 pCurr->cmd.read.page,
                                 pCurr->cmd.read.msb,
                                 pCurr->cmd.read.lsb);
            }
            break;
        default:
            diag_util_printf("Unknown operation type %d!\n", pCurr->cmdType);
            return CPARSER_NOT_OK;
        }

        pCurr = pCurr->pNext;
    }

    return CPARSER_OK;
}/* _diag_debug_batch_analysis */

static int32
_diag_debug_batch_assemble_writedata(
    uint16 readData,
    uint16 msb,
    uint16 lsb,
    uint16 writeData,
    uint16 *outData)
{
    uint16 i;
    uint16 probe;
    uint16 temp = readData;

    for(i = lsb, probe = 0x01; i <= msb; i++, probe = probe << 1)
    {
        if((writeData & probe) == probe)
            temp = (temp | (0x01 << i));
        else 
            temp = (temp & (~(0x01 << i)));
    }

    *outData = temp;

    return CPARSER_OK;
}/* _diag_debug_batch_assemble_writedata */

static int32
_diag_debug_batch_assemble_readdata(
    uint16 readData,
    uint16 msb,
    uint16 lsb,
    uint16 *outData)
{
    uint16 temp = readData;
    uint16 dataSize = sizeof(uint16) * 8 - 1;

    temp = temp << (dataSize - msb);
    temp = temp >> ((dataSize - msb) + lsb);
    temp = temp << lsb;

    *outData = temp;

    return CPARSER_OK;
} /* _diag_debug_batch_assemble_readdata */

static int32
_diag_debug_batch_execute()
{
    int32 ret;
    uint32 i = 1;
    uint16 readData;
    uint16 writeData;
    debug_batch_rec_t *pHead = pListHead;
    debug_batch_rec_t *pCurr = pListHead;
    
    if(NULL == pCurr)
        return CPARSER_NOT_OK;

    /* Set loop execution */
    if(_diag_debug_batch_print != 0) diag_util_printf("Execute cmds:\n");
    diag_util_printf("results:\n");
    if(pCurr->cmdType == DEBUG_BATCH_OP_LOOP)
    {
        i = pCurr->cmd.loop.count;
        pCurr = pCurr->pNext;
        pHead = pCurr;
    }

    for(; i > 0; i--)
    {
        pCurr = pHead;
        while(pCurr != NULL)
        {
            switch(pCurr->cmdType)
            {
            case DEBUG_BATCH_OP_WRITE:
                ret = _diag_debug_internal_phy_read(phyId, pCurr->cmd.write.page,
                                           pCurr->cmd.write.reg, &readData);
                if(ret != RT_ERR_OK)
                {
                    diag_util_printf("MDIO read failed!\n");
                    return CPARSER_NOT_OK;
                }
                if(_diag_debug_batch_print != 0)
                {
                    diag_util_printf("W %2d(0x%04x) %2d %2d 0x%04x, 0x%04x --> ",
                                     pCurr->cmd.write.reg,
                                     pCurr->cmd.write.page,
                                     pCurr->cmd.write.msb,
                                     pCurr->cmd.write.lsb,
                                     pCurr->cmd.write.data,
                                     readData);
                }

                _diag_debug_batch_assemble_writedata(readData,
                                                pCurr->cmd.write.msb,
                                                pCurr->cmd.write.lsb,
                                                pCurr->cmd.write.data,
                                                &writeData);
                ret = _diag_debug_internal_phy_write(phyId, pCurr->cmd.write.page,
                                            pCurr->cmd.write.reg, 
                                            writeData);
                if(ret != RT_ERR_OK)
                {
                    diag_util_printf("MDIO write failed!\n");
                    return CPARSER_NOT_OK;
                }
                if(_diag_debug_batch_print != 0)
                    diag_util_printf("0x%04x\n", writeData);
                break;
            case DEBUG_BATCH_OP_READ:
                ret = _diag_debug_internal_phy_read(phyId, pCurr->cmd.read.page,
                                           pCurr->cmd.read.reg, &readData);
                if(ret != RT_ERR_OK)
                {
                    diag_util_printf("MDIO read failed!\n");
                    return CPARSER_NOT_OK;
                }
                if(_diag_debug_batch_print != 0)
                {
                    diag_util_printf("R %2d(0x%04x) %2d %2d = 0x%04x\n",
                                     pCurr->cmd.read.reg,
                                     pCurr->cmd.read.page,
                                     pCurr->cmd.read.msb,
                                     pCurr->cmd.read.lsb,
                                     readData);
                }
                _diag_debug_batch_assemble_readdata(readData,
                                                pCurr->cmd.read.msb,
                                                pCurr->cmd.read.lsb,
                                                &writeData);
                diag_util_printf("0x%04x\n", writeData);
                break;
            case DEBUG_BATCH_OP_LOOP:
            default:
                /* What happens? Skip the command */
                break;
            }

            pCurr = pCurr->pNext;
        }
    }

    return CPARSER_OK;
}/* _diag_debug_batch_execute */

cparser_result_t
_cparser_cmd_debug_batch_loop(
    cparser_context_t *context,
    uint32_t  *loop_ptr)
{
    int32 ret;
    
    ret = _diag_debug_batch_parse_cmd(DEBUG_BATCH_OP_LOOP,
                                      *loop_ptr,
                                      0,
                                      0,
                                      0);
    if(ret != CPARSER_OK)
    {
        diag_util_printf("parse cmd failed\n");
        return CPARSER_NOT_OK;
    }
        
    return CPARSER_OK;
}/* end of _cparser_cmd_debug_batch_loop */

cparser_result_t
_cparser_cmd_debug_batch_write(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr,
    uint32_t  *data_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    ret = _diag_debug_batch_parse_cmd(DEBUG_BATCH_OP_WRITE,
                                      *reg_ptr,
                                      *msb_ptr,
                                      *lsb_ptr,
                                      *data_ptr);
    if(ret != CPARSER_OK)
    {
        diag_util_printf("parse cmd failed\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of _cparser_cmd_debug_batch_write */

cparser_result_t
_cparser_cmd_debug_batch_read(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    ret = _diag_debug_batch_parse_cmd(DEBUG_BATCH_OP_READ,
                                      *reg_ptr,
                                      *msb_ptr,
                                      *lsb_ptr,
                                      0);
    if(ret != CPARSER_OK)
    {
        diag_util_printf("parse cmd failed\n");
        return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of _cparser_cmd_debug_batch_read */

/*
 * debug batch a <UINT:loop>
 */
cparser_result_t
cparser_cmd_debug_batch_a_loop(
    cparser_context_t *context,
    uint32_t  *loop_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    return _cparser_cmd_debug_batch_loop(context, loop_ptr);
}    /* end of cparser_cmd_debug_batch_a_a_loop */

/*
 * debug batch A <UINT:loop>
 */
cparser_result_t
cparser_cmd_debug_batch_A_loop(
    cparser_context_t *context,
    uint32_t  *loop_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    
    return _cparser_cmd_debug_batch_loop(context, loop_ptr);
}    /* end of cparser_cmd_debug_batch_a_a_loop */

/*
 * debug batch w <UINT:reg> <UINT:msb> <UINT:lsb> <HEX:data>
 */
cparser_result_t
cparser_cmd_debug_batch_w_reg_msb_lsb_data(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr,
    uint32_t  *data_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    return _cparser_cmd_debug_batch_write(context, reg_ptr, msb_ptr, lsb_ptr, data_ptr);
}    /* end of cparser_cmd_debug_batch_w_w_reg_msb_lsb_data */

/*
 * debug batch W <UINT:reg> <UINT:msb> <UINT:lsb> <HEX:data>
 */
cparser_result_t
cparser_cmd_debug_batch_W_reg_msb_lsb_data(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr,
    uint32_t  *data_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    return _cparser_cmd_debug_batch_write(context, reg_ptr, msb_ptr, lsb_ptr, data_ptr);
}    /* end of cparser_cmd_debug_batch_w_w_reg_msb_lsb_data */

/*
 * debug batch r <UINT:reg> <UINT:msb> <UINT:lsb>
 */
cparser_result_t
cparser_cmd_debug_batch_r_reg_msb_lsb(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    return _cparser_cmd_debug_batch_read(context, reg_ptr, msb_ptr, lsb_ptr);
}    /* end of cparser_cmd_debug_batch_r_r_reg_msb_lsb */

/*
 * debug batch R <UINT:reg> <UINT:msb> <UINT:lsb>
 */
cparser_result_t
cparser_cmd_debug_batch_R_reg_msb_lsb(
    cparser_context_t *context,
    uint32_t  *reg_ptr,
    uint32_t  *msb_ptr,
    uint32_t  *lsb_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    return _cparser_cmd_debug_batch_read(context, reg_ptr, msb_ptr, lsb_ptr);
}    /* end of cparser_cmd_debug_batch_r_r_reg_msb_lsb */

/*
 * debug batch phy <UINT:phyid>
 */
cparser_result_t
cparser_cmd_debug_batch_phy_phyid(
    cparser_context_t *context,
    uint32_t  *phyid_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    phyId = *phyid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_batch_p_p_phyid */

/*
 * debug batch list
 */
cparser_result_t
cparser_cmd_debug_batch_list(
    cparser_context_t *context)
{
    debug_batch_rec_t *pPtr;
    
    DIAG_UTIL_PARAM_CHK();

    pPtr = pListHead;
    while(pPtr != NULL)
    {
        switch(pPtr->cmdType)
        {
        case DEBUG_BATCH_OP_LOOP:
            diag_util_printf("A %d\n", pPtr->cmd.loop.count);
            break;
        case DEBUG_BATCH_OP_WRITE:
            diag_util_printf("W %2d(0x%04x) %2d %2d 0x%04x\n",
                             pPtr->cmd.write.reg,
                             pPtr->cmd.write.page,
                             pPtr->cmd.write.msb,
                             pPtr->cmd.write.lsb,
                             pPtr->cmd.write.data);
            break;
        case DEBUG_BATCH_OP_READ:
            diag_util_printf("R %2d(0x%04x) %2d %2d\n",
                             pPtr->cmd.read.reg,
                             pPtr->cmd.read.page,
                             pPtr->cmd.read.msb,
                             pPtr->cmd.read.lsb);
            break;
        }

        pPtr = pPtr->pNext;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_batch_l_l */

/*
 * debug batch execute { debug }
 */
cparser_result_t
cparser_cmd_debug_batch_execute_debug(
    cparser_context_t *context)
{
    int32 ret;
    debug_batch_rec_t *pPtr;

    DIAG_UTIL_PARAM_CHK();

    if(4 == TOKEN_NUM() && 
       0 == strcmp(TOKEN_STR(3), "debug"))
    {
        _diag_debug_batch_print = 1;
    }

    ret = _diag_debug_batch_analysis();
    if(ret != CPARSER_OK)
    {
        diag_util_printf("analysis cmd failed\n");
        _diag_debug_batch_print = 0;
        return CPARSER_NOT_OK;
    }

    ret = _diag_debug_batch_execute();
    if(ret != CPARSER_OK)
    {
        diag_util_printf("execute cmd failed\n");
        _diag_debug_batch_print = 0;
        return CPARSER_NOT_OK;
    }

    /* Clean up the mess */
    while(pListHead != NULL)
    {
        pPtr = pListHead;
        pListHead = pListHead->pNext;
        pPtr->pNext = NULL;
        osal_free(pPtr);
    }
    pListHead = NULL;
    pListTail = NULL;
    
    _diag_debug_batch_print = 0;
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_batch_e_e_debug */

/*
 * debug dump ( hsa | hsb | hsd )
 */
cparser_result_t
cparser_cmd_debug_dump_hsa_hsb_hsd(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    int32 tokenIdx;
    rtk_hsb_t hsbData;;
    rtk_hsa_t hsaData;
    rtk_hsa_debug_t hsdData;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_hsbData_get(&hsbData), ret);
                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsd"))
            {
                DIAG_UTIL_ERR_CHK(apollo_raw_hsdData_get(&hsdData), ret);
                _diag_debug_hsd_display(&hsdData);
            }
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(2),"hsb"))
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_hsbData_get(&hsbData), ret);
                _diag_debug_hsb_display(&hsbData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsa"))
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_hsaData_get(&hsaData), ret);
                _diag_debug_hsa_display(&hsaData);

            }
            else if(!osal_strcmp(TOKEN_STR(2),"hsd"))
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_hsdData_get(&hsdData), ret);
                _diag_debug_hsd_display(&hsdData);
            }
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
} /* end of cparser_cmd_debug_dump_hsa_hsb_hsd */

/*
 * debug dump hsd port <UINT:port>
 */
cparser_result_t
cparser_cmd_debug_dump_hsd_port_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
    uint32 readData;
    uint32 writeData;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_PARAM_RANGE_CHK((*port_ptr >= 7), CPARSER_ERR_INVALID_PARAMS);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
		    reg_array_read(RSVD_ALE_HSAr,REG_ARRAY_INDEX_NONE, 0, &readData);

		    writeData = (readData & 0xFFFFFF1F) | (*port_ptr<<5);

		    reg_array_write(RSVD_ALE_HSAr,REG_ARRAY_INDEX_NONE, 0, &writeData);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
			writeData = *port_ptr;
			reg_field_write(APOLLOMP_DBG_EP_CFGr,APOLLOMP_DBG_EPf, &writeData);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_dump_hsd_port_port */

/*
 * debug dump hsd latest
 */
cparser_result_t
cparser_cmd_debug_dump_hsd_latest(
    cparser_context_t *context)
{
    uint32 readData;
    uint32 writeData;

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
		    reg_array_read(RSVD_ALE_HSAr,REG_ARRAY_INDEX_NONE, 0, &readData);

		    writeData = (readData & 0xFFFFFF1F) | (7<<5);

		    reg_array_write(RSVD_ALE_HSAr,REG_ARRAY_INDEX_NONE, 0, &writeData);

            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
			writeData = 7;/*lasted is port 7*/
			reg_field_write(APOLLOMP_DBG_EP_CFGr,APOLLOMP_DBG_EPf, &writeData);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_dump_hsd_latest */


cparser_result_t cparser_cmd_debug_get_table_table_idx_address(cparser_context_t *context,
    uint32_t *table_idx_ptr,
    uint32_t *address_ptr)

{
    uint32      loop;
    int32       ret = RT_ERR_FAILED;
    uint32      value[20];

    DIAG_UTIL_PARAM_CHK();
    if ((ret = table_read(*table_idx_ptr, *address_ptr, value)) != RT_ERR_OK)
    {
        DIAG_ERR_PRINT(ret);
        return CPARSER_NOT_OK;
    }

    diag_util_mprintf("Table %u, address %u\n", *table_idx_ptr, *address_ptr);

    for (loop = 0; loop < 20; loop++)
    {
        diag_util_printf("%x-", value[loop]);
    }

    diag_util_mprintf("\n");

    return CPARSER_OK;
}


/*
 * debug rtk-init
 */
cparser_result_t cparser_cmd_debug_rtk_init(cparser_context_t *context,
    uint32_t *table_idx_ptr,
    uint32_t *address_ptr)

{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(rtk_all_module_init(), ret);

    return CPARSER_OK;
}



  
#ifdef CONFIG_SDK_APOLLOMP
void _apollomp_fpga_debug_phy_write(uint16 phy,uint16 reg,uint16 data)
{
    uint16 inputData;

    inputData = 0xC000 + (phy<<5) + reg;
    io_mii_phy_reg_write(8,1,data);
    io_mii_phy_reg_write(8,0,inputData);

}


static int32 _diag_debug_apollomp_fpga_init(void)
{
#if defined(FPGA_DEFINED)
    int32 ret;
    rtk_port_t port;
    uint32 data;
    
    HAL_SCAN_ALL_PORT(port)
    {
        /*PHY 0~6 RXC delay 6ns*/
        _apollomp_fpga_debug_phy_write(port,31,7);
        _apollomp_fpga_debug_phy_write(port,30,160);
        _apollomp_fpga_debug_phy_write(port,28,0x28);

        /*PHY 0~6 Disable PHY EEE*/
        _apollomp_fpga_debug_phy_write(port,0,0x1940);
        _apollomp_fpga_debug_phy_write(port,31,7);
        _apollomp_fpga_debug_phy_write(port,30,32);
        _apollomp_fpga_debug_phy_write(port,27,0x2f0a);
        _apollomp_fpga_debug_phy_write(port,31,0);
    }
    /*initial SerDes*/
	/*fine tune TX 16bits to 20 bits function block 622M sample 155M data position*/
    _apollomp_fpga_debug_phy_write(0x11,0,0x000a); /*force Tx/RX sample clk timing*/
    _apollomp_fpga_debug_phy_write(0x11,1,0x0100); /*fix Fiber TX parameter*/

	/*setting for jitter transfer---*/
    _apollomp_fpga_debug_phy_write(0x1d,0x1a,0x0000); /*RX_filter setting(7:0)*/
    _apollomp_fpga_debug_phy_write(0x1d,0x02,0x2d16); /*kp1==3,ki=1, TX CLK source =RX cdr,disable CMU_TX*/
    _apollomp_fpga_debug_phy_write(0x1d,0x16,0xa8b2); /*RX_KP1_2=3*/
    _apollomp_fpga_debug_phy_write(0x1d,0x03,0x6041); /*kp2=4*/
    _apollomp_fpga_debug_phy_write(0x1d,0x18,0xdde4); /*RX_KP2_2=4*/

	/*set best CMU-RX PLL parameter4*/
    _apollomp_fpga_debug_phy_write(0x1d,0x06,0xf4f0);
    _apollomp_fpga_debug_phy_write(0x1d,0x05,0x4003);
    _apollomp_fpga_debug_phy_write(0x1d,0x0f,0x4fe6); /*TX/RX Io=CML mode*/
    _apollomp_fpga_debug_phy_write(0x10,0x01,0x000c); /*rxd neg edge launch data*/

    HAL_SCAN_ALL_PORT(port)
    {
        _apollomp_fpga_debug_phy_write(port,4,0x05e1);
        _apollomp_fpga_debug_phy_write(port,0,0x1200);
    }

    /*ABLTY_FORCE_MODE to Enable Port 4,5,6*/
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x000124, 0x00000060), ret);
    /*FORCE_P_ABLTY port 5 ext  1000MF*/
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x00009C, 0x00000076), ret);
    /*FORCE_P_ABLTY port 6 GMAC*/
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x0000A0, 0x00000076), ret);
#if 0
    /*FORCE_P_ABLTY port 4 ext (PON) to link down */
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x000098, 0x00000000), ret);
#endif

    /*set trap-insert-tag disable*/
    data = DISABLED;
    DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_MAC_CPU_TAG_CTRLr, APOLLOMP_TRAP_TAGET_INSERT_ENf, &data), ret);
    
    /*WFQ_IFG = 0 WFQ_BURSTSIZE= 0x3FFF*/
    DIAG_UTIL_ERR_CHK(apollomp_raw_qos_wfqBurstSize_set(0x3FFF), ret);
    DIAG_UTIL_ERR_CHK(apollomp_raw_qos_wfqCalIncIfg_set(APOLLOMP_QOS_IFG_EXCLUDE), ret);
#if 0
    /*QoS init*/
    DIAG_UTIL_ERR_CHK(rtk_qos_init(),ret);

    /*init vlan to default setting*/
    DIAG_UTIL_ERR_CHK(rtk_vlan_init(),ret);

    /*init mib counter to default setting*/
    DIAG_UTIL_ERR_CHK(rtk_stat_init(),ret);

    /*init port module to default setting*/
    DIAG_UTIL_ERR_CHK(rtk_port_init(),ret);

#endif



    /*CHIP_INF_SEL */
    DIAG_UTIL_ERR_CHK(ioal_mem32_write(0x000184, 0x00000002), ret);

    /*PON_SCH_QMAP queue 0 map to t-cont 1*/
    DIAG_UTIL_ERR_CHK(apollomp_raw_ponMacScheQmap_set(0,0x000000ff), ret);
    /*PON_TCONT_EN  for T-CONT 1*/
    DIAG_UTIL_ERR_CHK(apollomp_raw_ponMacTcontEnable_set(0, ENABLED), ret);
    /*PON_QID_PIR_RATE for queue 0*/
    DIAG_UTIL_ERR_CHK(apollomp_raw_ponMacPirRate_set(0, 0x1FFFF), ret);

#endif /*FPGA_DEFINED*/
    
    return RT_ERR_OK;
}

#endif /*CONFIG_SDK_APOLLOMP*/

/*
 * debug fpga-init
 */
cparser_result_t cparser_cmd_debug_fpga_init(cparser_context_t *context,
    uint32_t *table_idx_ptr,
    uint32_t *address_ptr)

{
    int32 ret = RT_ERR_FAILED;
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("please use old version diagshell for test chip fpga verify!\n");
            break;
#endif
            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(_diag_debug_apollomp_fpga_init(), ret);
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    return CPARSER_OK;
}


/*
 * debug set phy <UINT:phy_id> <UINT:reg_address> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_phy_phy_id_reg_address_value(
    cparser_context_t *context,
    uint32_t  *phy_id_ptr,
    uint32_t  *reg_address_ptr,
    uint32_t  *value_ptr)
{
    DIAG_UTIL_PARAM_CHK();

#if defined (FPGA_DEFINED)
    io_mii_phy_reg_write((uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr,(uint16_t)*value_ptr);
#else
    diag_util_mprintf("command not support in this mode!\n");
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_phy_phy_id_reg_address_value */

/*
 * debug get phy <UINT:phy_id> <UINT:reg_address>
 */
cparser_result_t
cparser_cmd_debug_get_phy_phy_id_reg_address(
    cparser_context_t *context,
    uint32_t  *phy_id_ptr,
    uint32_t  *reg_address_ptr)
{
    uint16 data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();


#if defined (FPGA_DEFINED)
    io_mii_phy_reg_read((uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr,&data);
    diag_util_mprintf("phy: %d reg: %d data: 0x%x\n", (uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr, data);

#else
    diag_util_mprintf("command not support in this mode!\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_phy_phy_id_reg_address_address */

#define PATCH_W_OFFSET 0xC000
#define PATCH_R_OFFSET 0x8000
/*
 * debug set ind_phy <UINT:phy_id> <UINT:reg_address> <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_set_ind_phy_phy_id_reg_address_value(
    cparser_context_t *context,
    uint32_t  *phy_id_ptr,
    uint32_t  *reg_address_ptr,
    uint32_t  *value_ptr)
{
    uint16 data;

    DIAG_UTIL_PARAM_CHK();

    data = *value_ptr;
    io_mii_phy_reg_write(8, 1, data);

    data = (PATCH_W_OFFSET | ((*phy_id_ptr & 0x1F) << 5) |(*reg_address_ptr & 0x1F));
    io_mii_phy_reg_write(8, 0, data);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_ind_phy_phy_id_reg_address_value */

/*
 * debug get ind_phy <UINT:phy_id> <UINT:reg_address>
 */
cparser_result_t
cparser_cmd_debug_get_ind_phy_phy_id_reg_address(
    cparser_context_t *context,
    uint32_t  *phy_id_ptr,
    uint32_t  *reg_address_ptr)
{
    uint16 data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    data = (PATCH_R_OFFSET | ((*phy_id_ptr & 0x1F) << 5) |(*reg_address_ptr & 0x1F));
    io_mii_phy_reg_write(8, 0, data);

    io_mii_phy_reg_read(8, 2, &data);

    diag_util_printf("phy: %d reg: %d data: 0x%x\n", (uint8_t)*phy_id_ptr,(uint8_t)*reg_address_ptr, data);


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_ind_phy_phy_id_reg_address */

/*
 * debug set register dump ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_set_register_dump_enable_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    if(TOKEN_STR(4)[0] == 'e')
        ioal_mem32_debugMode_set(ENABLED);
    else
        ioal_mem32_debugMode_set(DISABLED);
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_register_dump_enable_disable */

/*
 * debug get version { detail }
 */
cparser_result_t
cparser_cmd_debug_get_version_detail(
    cparser_context_t *context)
{
    uint16 date, time, revision;
    int32 detail, i, total_entry, def_idx;
    uint32 rtl_version, sw_version;
    uint32 reg_addr;

#ifdef CONFIG_SDK_APOLLOMP
    uint32 rgf_ver_reg[]={
            APOLLOMP_RGF_VER_GLB_CTRLr,
            /*RGF_VER_CHIP_INFOr,*/
            APOLLOMP_RGF_VER_ALE_GLBr,
            APOLLOMP_RGF_VER_ALE_ACLr,
            APOLLOMP_RGF_VER_ALE_CVLANr,
            APOLLOMP_RGF_VER_ALE_DPMr,
            APOLLOMP_RGF_VER_ALE_L2r,
            APOLLOMP_RGF_VER_ALE_MLTVLANr,
            APOLLOMP_RGF_VER_ALE_SVLANr,
            APOLLOMP_RGF_VER_ALE_EEE_LLDPr,
            APOLLOMP_RGF_VER_ALE_RLDPr,
            APOLLOMP_RGF_VER_ALE_EAV_AFBKr,
            APOLLOMP_RGF_VER_INTRr,
            /*RGF_VER_LEDr,*/
            APOLLOMP_RGF_VER_PER_PORT_MACr,
            /*RGF_VER_SDSREGr,*/
            APOLLOMP_RGF_VER_SWCOREr,
            /*RGF_VER_EPON_CTRLr,*/
            APOLLOMP_RGF_VER_ALE_RMA_ATTACKr,
            APOLLOMP_RGF_VER_BIST_CTRLr,
            APOLLOMP_RGF_VER_EGR_OUTQr,
            APOLLOMP_RGF_VER_EGR_SCHr,
            APOLLOMP_RGF_VER_ALE_HSAr,
            APOLLOMP_RGF_VER_ALE_METERr,
            /*RGF_VER_MAC_PONr,*/
            APOLLOMP_RGF_VER_MIB_CTRLr,
            APOLLOMP_RGF_VER_ALE_PISOr
            };

    int8 *rgf_ver_str[]={
            "RGF_VER_GLB_CTRL",
            /*"RGF_VER_CHIP_INFO",*/
            "RGF_VER_ALE_GLB",
            "RGF_VER_ALE_ACL",
            "RGF_VER_ALE_CVLAN",
            "RGF_VER_ALE_DPM",
            "RGF_VER_ALE_L2",
            "RGF_VER_ALE_MLTVLAN",
            "RGF_VER_ALE_SVLAN",
            "RGF_VER_ALE_EEE_LLDP",
            "RGF_VER_ALE_RLDP",
            "RGF_VER_ALE_EAV_AFBK",
            "RGF_VER_INTR",
            /*"RGF_VER_LED",*/
            "RGF_VER_PER_PORT_MAC",
            /*"RGF_VER_SDSREG",*/
            "RGF_VER_SWCORE",
            /*"RGF_VER_EPON_CTRL",*/
            "RGF_VER_ALE_RMA_ATTACK",
            "RGF_VER_BIST_CTRL",
            "RGF_VER_EGR_OUTQ",
            "RGF_VER_EGR_SCH",
            "RGF_VER_ALE_HSA",
            "RGF_VER_ALE_METER",
            /*"RGF_VER_MAC_PON",*/
            "RGF_VER_MIB_CTRL",
            "RGF_VER_ALE_PISO"
            };
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if(TOKEN_NUM() == 4)
        detail = 1;
    else
        detail = 0;
#ifdef RTL_CYGWIN_EMULATE
    diag_util_mprintf("Diagshell Build Time: %s %s (for RAM emulation)\n", __DATE__,__TIME__);
#else
    #if defined(FPGA_DEFINED)
        diag_util_mprintf("Diagshell Build Time: %s %s (for FPGA board)\n", __DATE__,__TIME__);
    #else
        diag_util_mprintf("Diagshell Build Time: %s %s (for chip)\n", __DATE__,__TIME__);
    #endif
#endif

#if defined (FPGA_DEFINED)
    io_mii_phy_reg_read(9, 0x0, &date);
    io_mii_phy_reg_read(9, 0x1, &time);
    io_mii_phy_reg_read(9, 0x2, &revision);
    diag_util_mprintf("FPGA data: 0x%04x, time 0x%04x, revision 0x%04x\n", date, time, revision);

    reg_field_read(rgf_ver_reg[0], APOLLOMP_REGFILE_VERf, &rtl_version);
    diag_util_printf("%-22s: 0x%08x\n",rgf_ver_str[0], rtl_version);


    total_entry = sizeof(rgf_ver_reg)/sizeof(uint32);
    for(i = 0; i < total_entry; i++)
    {
        reg_field_read(rgf_ver_reg[i], APOLLOMP_REGFILE_VERf, &rtl_version);

        reg_addr = HAL_GET_REG_ADDR(rgf_ver_reg[i]);
        sw_version = 0;
        for(def_idx = 0; def_idx < APOLLOMP_REG_MAP_ENTRY_NUM; def_idx++)
        {
            if(apollomp_reg_map_def[def_idx].reg_addr == reg_addr)
                sw_version = apollomp_reg_map_def[def_idx].reset_val;
        }

        if((rtl_version != sw_version))
            diag_util_printf("Mismatch, %-22s: rtl_version=0x%08x, sw_version=0x%08x\n",rgf_ver_str[i],rtl_version,sw_version);

        if(detail)
            diag_util_printf("%-22s, version=0x%08x\n",rgf_ver_str[i],rtl_version);
    }
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_version_detail */

/* * debug packet rx dump { <UINT:byte> } */
cparser_result_t
cparser_cmd_debug_packet_rx_dump_byte(
    cparser_context_t *context,
    uint32_t  *byte_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int i, j;
    int len;
    uint32 enable;
    uint16 max_len;
    uint16 rx_len;
    uint8 buf[2048];
    struct pkt_dbg_rx_info rxInfo;

    DIAG_UTIL_PARAM_CHK();

    if(5 == TOKEN_NUM())
        max_len = (*byte_ptr > 2048) ? 2048 : *byte_ptr;
    else
        max_len = 128;

    rtk_pkt_rxDump_get(&buf[0], max_len, &rx_len, &rxInfo, &enable);

    diag_util_printf("Packet dump: %s\n", enable == 0 ? "disabled" : "enabled");
    diag_util_printf("Actual packet length 0x%x\n", rx_len);
    diag_util_printf("Maximum dump length 0x%x\n", max_len);
    len = (max_len > rx_len) ? rx_len : max_len;

    if(0 == len)
        return CPARSER_OK;

    diag_util_printf("Descriptor:\n");
    diag_util_printf("%8s %8s %8s %8s\n", "opts1", "addr", "opts2", "opts3");
    diag_util_printf("0x%08x 0x%08x 0x%08x 0x%08x\n\n", rxInfo.opts1.dw, rxInfo.addr, rxInfo.opts2.dw, rxInfo.opts3.dw);
    diag_util_printf("own eor fs ls crc l3csf l4csf rcdf frag ppptag rwt pkttype rout orifmt pctrl len\n");
    diag_util_printf("%3d %3d %2d %2d %3d %5d %5d %4d %4d %6d %3d %7d %4d %6d %5d %3d\n",
        rxInfo.opts1.bit.own,
        rxInfo.opts1.bit.eor,
        rxInfo.opts1.bit.fs,
        rxInfo.opts1.bit.ls,
        rxInfo.opts1.bit.crcerr,
        rxInfo.opts1.bit.ipv4csf,
        rxInfo.opts1.bit.l4csf,
        rxInfo.opts1.bit.rcdf,
        rxInfo.opts1.bit.ipfrag,
        rxInfo.opts1.bit.pppoetag,
        rxInfo.opts1.bit.rwt,
        rxInfo.opts1.bit.pkttype,
        rxInfo.opts1.bit.l3routing,
        rxInfo.opts1.bit.origformat,
        rxInfo.opts1.bit.pctrl,
        rxInfo.opts1.bit.data_length);
    
    diag_util_printf("cputag ptp_in_cpu_tag_exist svlan_tag_exist pon_stream_id ctagva cvaln_tag\n");
    diag_util_printf("%6d %20d %15d %13d %6d %9d\n",
        rxInfo.opts2.bit.cputag,
        rxInfo.opts2.bit.ptp_in_cpu_tag_exist,
        rxInfo.opts2.bit.svlan_tag_exist,
        rxInfo.opts2.bit.pon_stream_id,
        rxInfo.opts2.bit.ctagva,
        rxInfo.opts2.bit.cvaln_tag);

    diag_util_printf("src_port_num dst_port_mask reason internal_pri ext_port_ttl\n");
    diag_util_printf("%12d %13d %6d %12d %12d\n\n",
        rxInfo.opts3.bit.src_port_num,
        rxInfo.opts3.bit.dst_port_mask,
        rxInfo.opts3.bit.reason,
        rxInfo.opts3.bit.internal_priority,
        rxInfo.opts3.bit.ext_port_ttl_1);

    diag_util_printf("     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
    
    for(i = 0, j = 0; i < len; i++) {
        if(0 == (i % 0x10)) {
            diag_util_printf("\n%04x ", j);
            j++;
        }
        diag_util_printf("%02x ", buf[i]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif
    
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_dump_byte */

/*
 * debug packet rx clear
 */
cparser_result_t
cparser_cmd_debug_packet_rx_clear(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    rtk_pkt_rxDump_clear();
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_clear */

/* * debug packet rx { enable | disable } */
cparser_result_t
cparser_cmd_debug_packet_rx_enable_disable(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    if('e' == TOKEN_CHAR(3, 0))
        rtk_pkt_rxDumpEnable_set(1);
    else if('d' == TOKEN_CHAR(3, 0))
        rtk_pkt_rxDumpEnable_set(0);
    else
        return CPARSER_NOT_OK;
#else
    diag_util_printf("command not support in this mode\n");
#endif
    
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_rx_enable_disable */

/*
 * debug packet tx set pkt <UINT:pos> <STRING:data>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_pkt_pos_data(
    cparser_context_t *context,
    uint32_t  *pos_ptr,
    char * *data_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 i;
    char tmp[3];
    char *pPtr;
    uint8 *pData = NULL;
    uint16 input_length;
    uint16 payload_length;
    
    DIAG_UTIL_PARAM_CHK();

    /* Syntax check of the input string */
    input_length = strlen(*data_ptr);
    if(input_length & 0x01)
    {
        diag_util_printf("payload syntax error\n");
        return CPARSER_NOT_OK;
    }
    for(i = 0, pPtr = *data_ptr; i < input_length; i++)
    {
        if((pPtr[i] >= 'a') && (pPtr[i] <= 'f'))
            continue;
        else if((pPtr[i] >= 'A') && (pPtr[i] <= 'F'))
            continue;
        else if((pPtr[i] >= '0') && (pPtr[i] <= '9'))
            continue;

        diag_util_printf("payload syntax error at %d\n", i);
        return CPARSER_NOT_OK;
    }

    /* Parse the string */
    payload_length = input_length >> 1;
    pData = osal_alloc(payload_length * sizeof(uint8));
    if(NULL == pData)
    {
        diag_util_printf("allocate failed\n", i);
        return CPARSER_NOT_OK;
    }
    
    tmp[2] = '\0';
    for(i = 0, pPtr = *data_ptr; i < payload_length; i++, pPtr += 2)
    {
        tmp[0] = pPtr[0];
        tmp[1] = pPtr[1];
        pData[i] = strtoul(&tmp[0], NULL, 16);
    }

    rtk_pkt_txPkt_set(*pos_ptr, pData, payload_length);

    osal_free(pData);
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_pkt_pos_data */

/*
 * debug packet tx set addr <MACADDR:da> <MACADDR:sa>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_addr_da_sa(
    cparser_context_t *context,
    cparser_macaddr_t  *da_ptr,
    cparser_macaddr_t  *sa_ptr)
{
    DIAG_UTIL_PARAM_CHK();
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    rtk_pkt_txAddr_set((rtk_mac_t *)da_ptr, (rtk_mac_t *)sa_ptr);
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_addr_da_sa */

/*
 * debug packet tx set l2payload <UINT:pos> <STRING:payload>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_l2payload_pos_payload(
    cparser_context_t *context,
    uint32_t  *pos_ptr,
    char * *payload_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 i;
    char tmp[3];
    char *pPtr;
    uint8 *pPayload = NULL;
    uint16 input_length;
    uint16 payload_length;
    
    DIAG_UTIL_PARAM_CHK();

    /* Syntax check of the input string */
    input_length = strlen(*payload_ptr);
    if(input_length & 0x01)
    {
        diag_util_printf("payload syntax error\n");
        return CPARSER_NOT_OK;
    }
    for(i = 0, pPtr = *payload_ptr; i < input_length; i++)
    {
        if((pPtr[i] >= 'a') && (pPtr[i] <= 'f'))
            continue;
        else if((pPtr[i] >= 'A') && (pPtr[i] <= 'F'))
            continue;
        else if((pPtr[i] >= '0') && (pPtr[i] <= '9'))
            continue;

        diag_util_printf("payload syntax error at %d\n", i);
        return CPARSER_NOT_OK;
    }

    /* Parse the string */
    payload_length = input_length >> 1;
    pPayload = osal_alloc(payload_length * sizeof(uint8));
    if(NULL == pPayload)
    {
        diag_util_printf("allocate failed\n", i);
        return CPARSER_NOT_OK;
    }
    
    tmp[2] = '\0';
    for(i = 0, pPtr = *payload_ptr; i < payload_length; i++, pPtr += 2)
    {
        tmp[0] = pPtr[0];
        tmp[1] = pPtr[1];
        pPayload[i] = strtoul(&tmp[0], NULL, 16);
    }

    rtk_pkt_txPkt_set(*pos_ptr + PKT_DEBUG_PKT_MACADDR_OFFSET, pPayload, payload_length);

    osal_free(pPayload);
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_payload_payload */

/*
 * debug packet tx set padding <UINT:start> <UINT:end> <UINT:pkt_len>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_padding_start_end_pkt_len(
    cparser_context_t *context,
    uint32_t  *start_ptr,
    uint32_t  *end_ptr,
    uint32_t  *pkt_len_ptr)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    rtk_pkt_txPadding_set(*start_ptr, *end_ptr, *pkt_len_ptr);
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_padding_data_pkt_len */

/*
 * debug packet tx get preview { <UINT:length> }
 */
cparser_result_t
cparser_cmd_debug_packet_tx_get_preview_length(
    cparser_context_t *context,
    uint32_t  *length_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 i,j;
    int32 ret;
    uint8 *pkt;
    uint16 pkt_length;
    uint16 dump_length;
//    rtk_mac_t src;
//    rtk_mac_t dst;
    
    DIAG_UTIL_PARAM_CHK();

    if(TOKEN_NUM() >= 6)
        dump_length = ((*length_ptr) > PKT_DEBUG_PKT_LENGTH_MAX) ? PKT_DEBUG_PKT_LENGTH_MAX : (*length_ptr);
    else
        dump_length = 128;

    pkt = osal_alloc(dump_length * sizeof(uint8));
    if(NULL == pkt)
    {
        diag_util_printf("%s:%d Allocate buffer failed!\n", __FUNCTION__, __LINE__);
        return CPARSER_NOT_OK;
    }        

#if 0
    ret = rtk_pkt_txAddr_get(&dst, &src);
    if(ret != 0)
    {
        diag_util_printf("%s:%d Get address!\n", __FUNCTION__, __LINE__);
        osal_free(pkt);
        return CPARSER_NOT_OK;
    }
#endif
        
    ret = rtk_pkt_txBuffer_get(pkt, dump_length, &pkt_length);
    if(ret != 0)
    {
        diag_util_printf("%s:%d Get packet failed!\n", __FUNCTION__, __LINE__);
        osal_free(pkt);
        return CPARSER_NOT_OK;
    }

#if 0
    diag_util_printf("Address: \n DA: %s\n", diag_util_inet_mactoa(&dst.octet[0]));
    diag_util_printf(" SA: %s\n\n", diag_util_inet_mactoa(&src.octet[0]));
#endif

    diag_util_printf("packet length: %d\n\n", pkt_length);
    if(pkt_length != 0)
    {
        diag_util_printf("     00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n");
    
        for(i = 0, j = 0; i < pkt_length; i++) {
            if(0 == (i % 0x10)) {
                diag_util_printf("\n%04x ", j);
                j++;
            }
            diag_util_printf("%02x ", pkt[i]);
        }
    }
    diag_util_printf("\n");

    osal_free(pkt);
#else
    DIAG_UTIL_PARAM_CHK();
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_get_preview_length */

/*
 * debug packet tx clear
 */
cparser_result_t
cparser_cmd_debug_packet_tx_clear(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    rtk_pkt_txBuffer_clear();
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_clear */

/*
 * debug packet tx send
 */
cparser_result_t
cparser_cmd_debug_packet_tx_send(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    rtk_pkt_tx_send();
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_send */

/*
 * debug packet tx set cputag l3cs ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_l3cs_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word1.bit.l3cs = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word1.bit.l3cs = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_l3cs_enable_disable */

/*
 * debug packet tx set cputag l4cs ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_l4cs_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word1.bit.l4cs = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word1.bit.l4cs = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_l4cs_enable_disable */

/*
 * debug packet tx set cputag keep ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_keep_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.keep = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.keep = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_keep_enable_disable */

/*
 * debug packet tx set cputag learning ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_learning_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.dislrn = 0;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.dislrn = 1;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_learning_enable_disable */

/*
 * debug packet tx set cputag l2br ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_l2br_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word3.bit.l2br = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word3.bit.l2br = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_l2br_enable_disable */

/*
 * debug packet tx set cputag l34keep ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_l34keep_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.l34keep = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.l34keep = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_l34keep_enable_disable */

/*
 * debug packet tx set cputag efid ( enable | disable ) <UINT:efid>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_efid_enable_disable_efid(
    cparser_context_t *context,
    uint32_t  *efid_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    if(*efid_ptr > 0b111)
    {
        diag_util_printf("enhance fid out of range %d\n", *efid_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.efid = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.efid = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    cpuTag.word2.bit.efid_value = *efid_ptr & 0b111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_efid_enable_disable_efid */

/*
 * debug packet tx set cputag prisel ( enable | disable ) <UINT:priority>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_prisel_enable_disable_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    if(*priority_ptr > 0b111)
    {
        diag_util_printf("priority out of range %d\n", *priority_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.prisel = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.prisel = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    cpuTag.word2.bit.priority = *priority_ptr & 0b111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_prisel_enable_disable_priority */

/*
 * debug packet tx set cputag vsel ( enable | disable )
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_vsel_enable_disable(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.vsel = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.vsel = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_vsel_enable_disable */

/*
 * debug packet tx set cputag txmask_vidx <UINT:value>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_txmask_vidx_value(
    cparser_context_t *context,
    uint32_t *value_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    if(*value_ptr > 0b111111)
    {
        diag_util_printf("txmask_vidx out of range %d\n", *value_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
    cpuTag.word1.bit.txmask_vidx = *value_ptr & 0b111111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_txmask_vidx */

/*
 * debug packet tx set cputag psel ( enable | disable ) <UINT:streamid>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_psel_enable_disable_streamid(
    cparser_context_t *context,
    uint32_t  *streamid_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    if(*streamid_ptr > 0b1111111)
    {
        diag_util_printf("stream id out of range %d\n", *streamid_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('e' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.psel = 1;
    }
    else if('d' == TOKEN_CHAR(6,0))
    {
        cpuTag.word2.bit.psel = 0;
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    cpuTag.word3.bit.pon_streamid = *streamid_ptr & 0b1111111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_psel_enable_disable_streamid */

/*
 * debug packet tx set cputag pppoeact ( intact | addhdr | removehdr | remarking ) <UINT:index>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_pppoeact_intact_addhdr_removehdr_remarking_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    if(*index_ptr > 0b111)
    {
        diag_util_printf("stream id out of range %d\n", *index_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    if('i' == TOKEN_CHAR(6,0))
    {
        cpuTag.word3.bit.pppoeact = 0b00;
    }
    else if('a' == TOKEN_CHAR(6,0))
    {
        cpuTag.word3.bit.pppoeact = 0b01;
    }
    else if(strlen(TOKEN_STR(6)) >= 4)
    {
        if('o' == TOKEN_CHAR(6,3))
        {
            cpuTag.word3.bit.pppoeact = 0b10;
        }
        else if('a' == TOKEN_CHAR(6,3))
        {
            cpuTag.word3.bit.pppoeact = 0b11;
        }
        else
        {
            diag_util_printf("invalid option %s\n", TOKEN_STR(6));
            return CPARSER_NOT_OK;
        }
    }
    else
    {
        diag_util_printf("invalid option %s\n", TOKEN_STR(6));
        return CPARSER_NOT_OK;
    }
    cpuTag.word3.bit.pppoeidx = *index_ptr & 0b111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_pppoeact_intact_addhdr_removehdr_remarking_index */

/*
 * debug packet tx set cputag extspa <UINT:port>
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_extspa_port(
    cparser_context_t *context,
    uint32_t  *port_ptr)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    if(*port_ptr > 0b111)
    {
        diag_util_printf("port out of range %d\n", *port_ptr);
        return CPARSER_NOT_OK;
    }

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    cpuTag.word3.bit.extspa = *port_ptr & 0b111;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;}    /* end of cparser_cmd_debug_packet_tx_set_cputag_extspa_port */

/*
 * debug packet tx set cputag clear
 */
cparser_result_t
cparser_cmd_debug_packet_tx_set_cputag_clear(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;

    DIAG_UTIL_PARAM_CHK();

    cpuTag.word1.w = 0;
    cpuTag.word2.w = 0;
    cpuTag.word3.w = 0;
    cpuTag.rsvd = 0;

    ret = rtk_pkt_txCpuTag_set(cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag set failed %d\n", ret);
        return CPARSER_NOT_OK;
    }
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_set_cputag_clear */

/*
 * debug packet tx get cputag
 */
cparser_result_t
cparser_cmd_debug_packet_tx_get_cputag(
    cparser_context_t *context)
{
#if defined(CONFIG_LINUX_USER_SHELL) && defined(CONFIG_SOC_DEPEND_FEATURE)
    int32 ret;
    struct pkt_dbg_cputag cpuTag;
    
    DIAG_UTIL_PARAM_CHK();

    memset(&cpuTag, 0, sizeof(struct pkt_dbg_cputag));
    ret = rtk_pkt_txCpuTag_get(&cpuTag);
    if(ret != RT_ERR_OK)
    {
        diag_util_printf("%s:%d cpu tag get failed %d\n", ret);
        return CPARSER_NOT_OK;
    }

    diag_util_printf("l3cs l4cs txmask_vidx\n");
    diag_util_printf("%4u %4u %11u\n",
                     cpuTag.word1.bit.l3cs,
                     cpuTag.word1.bit.l4cs,
                     cpuTag.word1.bit.txmask_vidx);
    diag_util_printf("efid enhace_fid prisel priority keep vsel dislrn psel l34keep\n");
    diag_util_printf("%4u %10u %6u %8u %4u %4u %6u %4u %7u\n",
                     cpuTag.word2.bit.efid,
                     cpuTag.word2.bit.efid_value,
                     cpuTag.word2.bit.prisel,
                     cpuTag.word2.bit.priority,
                     cpuTag.word2.bit.keep,
                     cpuTag.word2.bit.vsel,
                     cpuTag.word2.bit.dislrn,
                     cpuTag.word2.bit.psel,
                     cpuTag.word2.bit.l34keep);
    diag_util_printf("extspa pppoeact pppoeidx l2br ponstream_id\n");
    diag_util_printf("%6u %8u %8u %4u %12u\n",
                     cpuTag.word3.bit.extspa,
                     cpuTag.word3.bit.pppoeact,
                     cpuTag.word3.bit.pppoeidx,
                     cpuTag.word3.bit.l2br,
                     cpuTag.word3.bit.pon_streamid);
#else
    diag_util_printf("command not support in this mode\n");
#endif

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_packet_tx_get_cputag */

/*
 * debug register-rw-test register <UINT:address> data <UINT:value> count <UINT:count>
 */
cparser_result_t
cparser_cmd_debug_register_rw_test_register_address_data_value_count_count(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *value_ptr,
    uint32_t  *count_ptr)
{
    uint32  mem = 0;
    uint32  value  = 0;
    uint32  rValue  = 0;
    int32   ret = RT_ERR_FAILED;
    int32   i;
    int32   isSocMem = 0;
    

    DIAG_UTIL_PARAM_CHK();

    mem = *address_ptr;
    value = *value_ptr;

    if(mem >= 0x01000000)
        isSocMem = 1;/*soc memory*/    
    else
        isSocMem = 0;    

    if (0 != (mem % 4))
    {
        diag_util_printf("\n\rWarning! The address must be a multiple of 4.\n\r\n\r");
        return CPARSER_NOT_OK;
    }

    for(i=0 ; i< *count_ptr ; i++)
    {
        if(isSocMem)
        {
            DIAG_UTIL_ERR_CHK(ioal_socMem32_write(mem, 0x0), ret);
            DIAG_UTIL_ERR_CHK(ioal_socMem32_read(mem, &rValue), ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(ioal_mem32_write(mem, 0x0), ret);
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &rValue), ret);
        }
        if(rValue != 0x0)
        {
            diag_util_printf("\n\rError! data not match write:0x%8.8x read:0x%8.8x\n\r\n\r",0x0,rValue);
            return CPARSER_NOT_OK;
        }

        if(isSocMem)
        {
            DIAG_UTIL_ERR_CHK(ioal_socMem32_write(mem, value), ret);
            DIAG_UTIL_ERR_CHK(ioal_socMem32_read(mem, &rValue), ret);
        }
        else
        {
            DIAG_UTIL_ERR_CHK(ioal_mem32_write(mem, value), ret);
            DIAG_UTIL_ERR_CHK(ioal_mem32_read(mem, &rValue), ret);
        }
        
        if(rValue != value)
        {
            diag_util_printf("\n\rError! data not match write:0x%8.8x read:0x%8.8x\n\r\n\r",value,rValue);
            return CPARSER_NOT_OK;
        }


    }

    diag_util_printf("\n\rTest pass.\n\r\n\r");
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_register_rw_test_register_address_data_value_count_count */




#if defined(CMD_DEBUG_SET_PRINT_PORT_MUX_MUX)
/*
 * debug set print-port-mux <UINT:mux>
 */
cparser_result_t
cparser_cmd_debug_set_print_port_mux_mux(
    cparser_context_t *context,
    uint32_t  *mux_ptr)
{
    DIAG_UTIL_PARAM_CHK();


#if defined(CYGWIN_MDIO_IO)
    MDC_PIN_DEF(3,0,4,1,*mux_ptr);
#endif
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_print_port_mux_mux */
#endif


/*
 * debug set ( dbgo_wrap_gphy | dbgo_sys_gen | dbgo_ctrlckt | dbgo_misc | dbg_rrcp_o | dbg_rldp_o )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_wrap_gphy_dbgo_sys_gen_dbgo_ctrlckt_dbgo_misc_dbg_rrcp_o_dbg_rldp_o(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbgo_wrap_gphy"))
    {
        value = 0x0000;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_sys_gen"))
    {
        value = 0x0100;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_ctrlckt"))
    {
        value = 0x0300;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_misc"))
    {
        value = 0x0500;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_rrcp_o"))
    {
        value = 0x0500;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_rldp_o"))
    {
        value = 0x0501;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_wrap_gphy_dbgo_sys_gen_dbgo_ctrlckt_dbgo_misc_dbg_rrcp_o_dbg_rldp_o */

/*
 * debug set ( dbg_led_o | dbg_rtct_o | dbg_mib_o | dbg_phy_sts_o | dbg_intrpt_o | dbg_afbk_o | dbg_diag_o | dbgo_hwpkt | dbgo_efuse )
 */
cparser_result_t
cparser_cmd_debug_set_dbg_led_o_dbg_rtct_o_dbg_mib_o_dbg_phy_sts_o_dbg_intrpt_o_dbg_afbk_o_dbg_diag_o_dbgo_hwpkt_dbgo_efuse(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbg_led_o"))
    {
        value = 0x0502;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_rtct_o"))
    {
        value = 0x0503;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_mib_o"))
    {
        value = 0x0504;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_phy_sts_o"))
    {
        value = 0x0505;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_intrpt_o"))
    {
        value = 0x0506;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_afbk_o"))
    {
        value = 0x0507;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbg_diag_o"))
    {
        value = 0x0508;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_hwpkt"))
    {
        value = 0x0509;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_efuse"))
    {
        value = 0x050a;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbg_led_o_dbg_rtct_o_dbg_mib_o_dbg_phy_sts_o_dbg_intrpt_o_dbg_afbk_o_dbg_diag_o_dbgo_hwpkt_dbgo_efuse */

/*
 * debug set ( dbgo_wrap_sds | dbgo_chip_clk_gen_0 | dbgo_chip_clk_gen_1 | dbgo_chip_clk_gen_2 | dbgo_chip_rst_gen )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_wrap_sds_dbgo_chip_clk_gen_0_dbgo_chip_clk_gen_1_dbgo_chip_clk_gen_2_dbgo_chip_rst_gen(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbgo_wrap_sds"))
    {
        value = 0x0800;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_clk_gen_0"))
    {
        value = 0x0900;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_clk_gen_1"))
    {
        value = 0x0a00;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_clk_gen_2"))
    {
        value = 0x0b00;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_rst_gen"))
    {
        value = 0x0c00;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_wrap_sds_dbgo_chip_clk_gen_0_dbgo_chip_clk_gen_1_dbgo_chip_clk_gen_2_dbgo_chip_rst_gen */

/*
 * debug set ( dbgo_chip_misc | dbgo_cfgif | dbgo_soc | dbgo_ssc | dbgo_pll_root | dbgo_ponctrl | dbgo_clkctrl )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_chip_misc_dbgo_cfgif_dbgo_soc_dbgo_ssc_dbgo_pll_root_dbgo_ponctrl_dbgo_clkctrl(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbgo_chip_misc"))
    {
        value = 0x0d00;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_cfgif"))
    {
        value = 0x0e00;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_soc"))
    {
        value = 0x1000;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_ssc"))
    {
        value = 0x1100;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_pll_root"))
    {
        value = 0x1200;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_ponctrl"))
    {
        value = 0x1300;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_clkctrl"))
    {
        value = 0x1400;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_chip_misc_dbgo_cfgif_dbgo_soc_dbgo_ssc_dbgo_pll_root_dbgo_ponctrl_dbgo_clkctrl */

/*
 * debug set ( dbgo_rstgen | dbgo_clkgen | dbgo_sw_lxslv | PLLTOUT )
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_rstgen_dbgo_clkgen_dbgo_sw_lxslv_PLLTOUT(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    if(!osal_strcmp(TOKEN_STR(2),"dbgo_rstgen"))
    {
        value = 0x1500;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_clkgen"))
    {
        value = 0x1600;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_sw_lxslv"))
    {
        value = 0x1700;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"PLLTOUT"))
    {
        value = 0x1800;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_rstgen_dbgo_clkgen_dbgo_sw_lxslv_plltout */

/*
 * debug set dbgo_regctrl
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_regctrl(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0200;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_regctrl */

/*
 * debug set dbgo_regctrl ( dbgo_swarb | dbgo_gphyarb | dbgo_smimst | dbgo_iicmst | dbgo_iicmst_1 ) bits-3-0 <UINT:bits_3_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_regctrl_dbgo_swarb_dbgo_gphyarb_dbgo_smimst_dbgo_iicmst_dbgo_iicmst_1_bits_3_0_bits_3_0(
    cparser_context_t *context,
    uint32_t  *bits_3_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0200;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    value = *bits_3_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"dbgo_swarb"))
    {
        value = 0x0050 | value;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbgo_gphyarb"))
    {
        value = 0x0060 | value;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_smimst"))
    {
        value = 0x0070 | value;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_iicmst"))
    {
        value = 0x00a0 | value;
    }
    else if(!osal_strcmp(TOKEN_STR(2),"dbgo_iicmst_1"))
    {
        value = 0x00b0 | value;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_regctrl_dbgo_swarb_dbgo_gphyarb_dbgo_smimst_dbgo_iicmst_dbgo_iicmst_1_bits_3_0_bits_3_0 */

/*
 * debug set dbgo_fctrl
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_fctrl(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0400;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_fctrl */

/*
 * debug set dbgo_fctrl ( dbgo_ingress | dbgo_egress ) bits-3-0 <UINT:bits_3_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_fctrl_dbgo_ingress_dbgo_egress_bits_3_0_bits_3_0(
    cparser_context_t *context,
    uint32_t  *bits_3_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0400;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    value = *bits_3_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"dbgo_ingress"))
    {
        value = 0x0000 | value;
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbgo_egress"))
    {
        value = 0x0010 | value;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_fctrl_dbgo_ingress_dbgo_egress_bits_3_0_bits_3_0 */

/*
 * debug set dbgo_swcore_cen
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_swcore_cen(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0600;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_swcore_cen */

/*
 * debug set dbgo_swcore_cen ( dbgo_sel_sch | dbgo_sel_outq | dbgo_sel_mtr | dbgo_sel_hsactrl | dbgo_sel_inq | dbgo_sel_out_drp ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_swcore_cen_dbgo_sel_sch_dbgo_sel_outq_dbgo_sel_mtr_dbgo_sel_hsactrl_dbgo_sel_inq_dbgo_sel_out_drp_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0600;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_sch"))
    {
        value = 0x0100 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_outq"))
    {
        value = 0x0200 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_mtr"))
    {
        value = 0x0300 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_hsactrl"))
    {
        value = 0x0400 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_inq"))
    {
        value = 0x0500 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_out_drp"))
    {
        value = 0x0600 | (value&0xFF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_swcore_cen_dbgo_sel_sch_dbgo_sel_outq_dbgo_sel_mtr_dbgo_sel_hsactrl_dbgo_sel_inq_dbgo_sel_out_drp_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_swcore_cen ( dbgo_sel_sch_pon | dbg_dpm_o | dbg_l2_o | dbg_acl_o | dbg_misc_o ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_swcore_cen_dbgo_sel_sch_pon_dbg_dpm_o_dbg_l2_o_dbg_acl_o_dbg_misc_o_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0600;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"dbgo_sel_sch_pon"))
    {
        value = 0x0700 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbg_dpm_o"))
    {
        value = 0x0000 | (value&0x3F);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbg_l2_o"))
    {
        value = 0x0080 | (value&0xF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbg_acl_o"))
    {
        value = 0x0090 | (value&0xF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"dbg_misc_o"))
    {
        value = 0x00a0 | (value&0xF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_swcore_cen_dbgo_sel_sch_pon_dbg_dpm_o_dbg_l2_o_dbg_acl_o_dbg_misc_o_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac */

/*
 * debug set dbgo_mac ( p0_dbgo_tx | p0_dbgo_rx | p0_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p0_dbgo_tx_p0_dbgo_rx_p0_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_tx"))
    {
        value = 0x0000 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_rx"))
    {
        value = 0x0100 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p0_dbgo_eee"))
    {
        value = 0x0200 | (value&0xFF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p0_dbgo_tx_p0_dbgo_rx_p0_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p1_dbgo_tx | p1_dbgo_rx | p1_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p1_dbgo_tx_p1_dbgo_rx_p1_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_tx"))
    {
        value = 0x0400 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_rx"))
    {
        value = 0x0500 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p1_dbgo_eee"))
    {
        value = 0x0600 | (value&0xFF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p1_dbgo_tx_p1_dbgo_rx_p1_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p2_dbgo_tx | p2_dbgo_rx | p2_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p2_dbgo_tx_p2_dbgo_rx_p2_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_tx"))
    {
        value = 0x0800 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_rx"))
    {
        value = 0x0900 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p2_dbgo_eee"))
    {
        value = 0x0a00 | (value&0xFF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p2_dbgo_tx_p2_dbgo_rx_p2_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p3_dbgo_tx | p3_dbgo_rx | p3_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p3_dbgo_tx_p3_dbgo_rx_p3_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_tx"))
    {
        value = 0x0c00 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_rx"))
    {
        value = 0x0d00 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p3_dbgo_eee"))
    {
        value = 0x0e00 | (value&0xFF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p3_dbgo_tx_p3_dbgo_rx_p3_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p4_dbgo_tx | p4_dbgo_rx | p4_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p4_dbgo_tx_p4_dbgo_rx_p4_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_tx"))
    {
        value = 0x1000 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_rx"))
    {
        value = 0x1100 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p4_dbgo_eee"))
    {
        value = 0x1200 | (value&0xFF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p4_dbgo_tx_p4_dbgo_rx_p4_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p5_dbgo_tx | p5_dbgo_rx | p5_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p5_dbgo_tx_p5_dbgo_rx_p5_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_tx"))
    {
        value = 0x1400 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_rx"))
    {
        value = 0x1500 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p5_dbgo_eee"))
    {
        value = 0x1600 | (value&0xFF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
    DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p5_dbgo_tx_p5_dbgo_rx_p5_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug set dbgo_mac ( p6_dbgo_tx | p6_dbgo_rx | p6_dbgo_eee ) bits-7-0 <UINT:bits_7_0>
 */
cparser_result_t
cparser_cmd_debug_set_dbgo_mac_p6_dbgo_tx_p6_dbgo_rx_p6_dbgo_eee_bits_7_0_bits_7_0(
    cparser_context_t *context,
    uint32_t  *bits_7_0_ptr)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();

    value = 0x0700;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(DBG_BLK_SELr, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_write(APOLLOMP_DBG_BLK_SELr, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    value = *bits_7_0_ptr;
    if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_tx"))
    {
        value = 0x1800 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_rx"))
    {
        value = 0x1900 | (value&0xFF);
    }
    else if(!osal_strcmp(TOKEN_STR(3),"p6_dbgo_eee"))
    {
        value = 0x1a00 | (value&0xFF);
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(DEBUG_SELr,DBGO_SELf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_DEBUG_SELr,APOLLOMP_DBGO_SELf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_dbgo_mac_p6_dbgo_tx_p6_dbgo_rx_p6_dbgo_eee_bits_7_0_bits_7_0 */

/*
 * debug get hsb latch-mode
 */
cparser_result_t
cparser_cmd_debug_get_hsb_latch_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32  value  = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(HSB_CTRLr,LATCH_MODEf, &value), ret);
            break;

#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_HSB_CTRLr,APOLLOMP_LATCH_MODEf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_mprintf("hsb latch mode: %s\n", diagStr_hsbLatchModeStr[value]);

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_hsb_latch_mode */

/*
 * debug set hsb latch-mode ( all | none | first-drop | first-pass | first-trap | drop | trap | acl )
 */
cparser_result_t
cparser_cmd_debug_set_hsb_latch_mode_all_none_first_drop_first_pass_first_trap_drop_trap_acl(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;
    DIAG_UTIL_PARAM_CHK();


    if(!osal_strcmp(TOKEN_STR(4),"all"))
    {
        value = 0x0;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"none"))
    {
        value = 0x1;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-drop"))
    {
        value = 0x2;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-pass"))
    {
        value = 0x3;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"first-trap"))
    {
        value = 0x4;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"drop"))
    {
        value = 0x5;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"trap"))
    {
        value = 0x6;
    }
    else if(!osal_strcmp(TOKEN_STR(4),"acl"))
    {
        value = 0x7;
    }


    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(HSB_CTRLr,LATCH_MODEf, &value), ret);
            break;
#endif

#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_HSB_CTRLr,APOLLOMP_LATCH_MODEf, &value), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_debug_set_hsb_latch_mode_all_none_first_drop_first_pass_first_trap_drop_trap_acl */


/*
 * debug get bond-chip-mode
 */
cparser_result_t
cparser_cmd_debug_get_bond_chip_mode(
    cparser_context_t *context)
{
    int32   ret = RT_ERR_FAILED;
    uint32  value  = 0;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            value = 0xb;
            DIAG_UTIL_ERR_CHK(reg_field_write(BOND_INFOr,BOND_INFO_ENf, &value), ret);
            DIAG_UTIL_ERR_CHK(reg_field_read(BOND_INFOr,BOND_CHIP_MODEf, &value), ret);

            value = value>>2;
        
            switch(value)
            {
                case 0x4:
                    diag_util_mprintf("Apollo (TQFP176EP)\n");

                    break;
                case 0x1:
                    diag_util_mprintf("Apollo-Pro (LQFP216EP)\n");
                    break;
                case 0x6:
                    diag_util_mprintf("RTL8198B (LQFP256EP)\n");
                    break;
                case 0x7:
                    diag_util_mprintf("LQFP256EP Debug\n");
                    break;
                default:
                    diag_util_mprintf("unknown chip mode\n");
                    break;
            }

            value = 0x0;
            DIAG_UTIL_ERR_CHK(reg_field_write(BOND_INFOr,BOND_INFO_ENf, &value), ret);
            break;

#endif

#ifdef CONFIG_SDK_APOLLO
        case APOLLOMP_CHIP_ID:
            diag_util_mprintf("function not ready\n");
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_bond_chip_mode */

/*
 * debug test <UINT:index>
 */
cparser_result_t
cparser_cmd_debug_test_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32   ret = RT_ERR_FAILED;

    rtk_acl_ingress_entry_t  aclRule;
    rtk_acl_field_t			aclField;
    rtk_acl_field_t			aclField2;
    rtk_acl_template_t      aclTemplate;
    rtk_acl_field_entry_t   fieldSel;
    rtk_acl_rangeCheck_ip_t ipRange;

    rtk_acl_field_t			rawField08;
    rtk_acl_field_t			rawField09;
    rtk_acl_field_t			rawField10;
    rtk_acl_field_t			rawField11;
    rtk_acl_field_t			rawField12;
    rtk_acl_field_t			rawField13;
    rtk_enable_t state;
    rtk_port_t port;


    switch(*index_ptr)
    {   
        /*add acl rule just after rtk_acl_init*/
        case 1:
            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
    
            aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
            aclField.fieldUnion.pattern.fieldIdx = 0x7;
            /*bit 0 TCP*/
            /*bit 1 UPD*/
            /*bit 2 ARP*/
            /* :       */
            /*bit 12 dhcp*/
            /*bit 13 dhcpv6*/
            aclField.fieldUnion.pattern.data.value = (0x1 << 2);/*care ARP*/
            aclField.fieldUnion.pattern.data.mask = (0x1 << 2);

            if((ret = rtk_acl_igrField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }
            aclRule.valid = ENABLED;
            aclRule.index = 0;/*rule index 0 */
            aclRule.templateIdx = 0x3;/*template index 3*/
            /*can be ingored*/
            /*
            aclRule.careTag.tags[ACL_CARE_TAG_IPV4].value = TRUE;
            aclRule.careTag.tags[ACL_CARE_TAG_IPV4].mask = TRUE;
            */
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_TRAP_ACT;

            /*enable for all ports actived*/
            aclRule.activePorts.bits[0] = 0x7F;


            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;
        /*change acl template 2 to MAC_DA+MAC_SA+CTAG+STAG*/    
        case 2:
            osal_memset(&aclTemplate, 0, sizeof(rtk_acl_template_t));

            aclTemplate.index = 2;
            aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
            aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
            aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
            aclTemplate.fieldType[3] = ACL_FIELD_SMAC0;
            aclTemplate.fieldType[4] = ACL_FIELD_SMAC1;
            aclTemplate.fieldType[5] = ACL_FIELD_SMAC2;
            aclTemplate.fieldType[6] = ACL_FIELD_CTAG;
            aclTemplate.fieldType[7] = ACL_FIELD_STAG;

            if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
            {
                return ret;
            }
            break;

        /*add acl rule just after rtk_acl_init*/
        case 3:
            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
    
            aclField.fieldType = ACL_FIELD_STAG;
            aclField.fieldUnion.l2tag.vid.value = 100;
            aclField.fieldUnion.l2tag.vid.mask = 0xFFF;
            aclField.next = NULL;

            if((ret = rtk_acl_igrField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }

            aclField2.fieldType = ACL_FIELD_ETHERTYPE;
            aclField2.fieldUnion.data.value = 0x0806;
            aclField2.fieldUnion.data.mask = 0xFFFF;
            aclField2.next = NULL;

            if((ret = rtk_acl_igrField_add(&aclRule, &aclField2))!= RT_ERR_OK)
            {
                return ret;
            }

            
            aclRule.valid = ENABLED;
            aclRule.index = 1;/*rule index 1*/
            aclRule.templateIdx = 0x0;/*template index 0*/
            /*care Ctag only*/
            aclRule.careTag.tags[ACL_CARE_TAG_CTAG].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_CTAG].mask = 1;
            
        
            aclRule.act.enableAct[ACL_IGR_PRI_ACT] = ENABLED;
            aclRule.act.priAct.act = ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT;
            aclRule.act.priAct.aclPri = 7;
            /*enable for port 2 actived*/
            aclRule.activePorts.bits[0] = 0x4;


            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }
            
            break;

        case 4:
            
            fieldSel.format = ACL_FORMAT_IPPAYLOAD;
            fieldSel.index = 13;
            fieldSel.offset = 0x0;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }            
            
            aclTemplate.index = 3;
            aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
            aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
            aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
            aclTemplate.fieldType[3] = ACL_FIELD_SMAC0;
            aclTemplate.fieldType[4] = ACL_FIELD_SMAC1;
            aclTemplate.fieldType[5] = ACL_FIELD_SMAC2;
            aclTemplate.fieldType[6] = ACL_FIELD_CTAG;
            aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED13;

            if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
            {
                return ret;
            }
            
            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            aclField.fieldType = ACL_FIELD_USER_DEFINED13;
            aclField.fieldUnion.data.value = 0x1234;/*l4 port*/
            aclField.fieldUnion.data.mask = 0xFFFF;

            if((ret = rtk_acl_igrField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }


            aclRule.valid = ENABLED;
            aclRule.index = 2;
            aclRule.templateIdx = 0x3;

            /*care tcp only*/
            aclRule.careTag.tags[ACL_CARE_TAG_TCP].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_TCP].mask = 1;

            /*drop packets*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
            RTK_PORTMASK_RESET(aclRule.act.forwardAct.portMask);

            HAL_GET_ALL_PORTMASK(aclRule.activePorts);

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;

        case 5:
            
            fieldSel.format = ACL_FORMAT_IPPAYLOAD;
            fieldSel.index = 13;
            fieldSel.offset = 0x0;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            }            
            
            aclTemplate.index = 3;
            aclTemplate.fieldType[0] = ACL_FIELD_DMAC0;
            aclTemplate.fieldType[1] = ACL_FIELD_DMAC1;
            aclTemplate.fieldType[2] = ACL_FIELD_DMAC2;
            aclTemplate.fieldType[3] = ACL_FIELD_SMAC0;
            aclTemplate.fieldType[4] = ACL_FIELD_SMAC1;
            aclTemplate.fieldType[5] = ACL_FIELD_SMAC2;
            aclTemplate.fieldType[6] = ACL_FIELD_CTAG;
            aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED13;

            if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
            {
                return ret;
            }
            
            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            
            aclField.fieldType = ACL_FIELD_PATTERN_MATCH;
            aclField.fieldUnion.pattern.data.value = 0x1234;
            aclField.fieldUnion.pattern.data.mask = 0xFFFF;
            aclField.fieldUnion.pattern.fieldIdx = 7;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }


            aclRule.valid = ENABLED;
            aclRule.index = 2;
            aclRule.templateIdx = 0x3;

            /*care tcp only*/
            aclRule.careTag.tags[ACL_CARE_TAG_TCP].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_TCP].mask = 1;

            /*drop packets*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
            RTK_PORTMASK_RESET(aclRule.act.forwardAct.portMask);

            HAL_GET_ALL_PORTMASK(aclRule.activePorts);

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;
            
        case 6:
            
            ipRange.index = 2;
            ipRange.type = IPRANGE_IPV4_SIP;
            ipRange.lowerIp = 0x01020304;
            ipRange.upperIp = 0x05060708;

            if((ret = rtk_acl_ipRange_set(&ipRange))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
            
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            
            aclField.fieldType = ACL_FIELD_IP_RANGE;
            aclField.fieldUnion.data.value = 0x0004;
            aclField.fieldUnion.data.mask = 0x0004;

            
            if((ret = rtk_acl_igrField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }

            aclRule.valid = ENABLED;
            aclRule.index = 3;
            aclRule.templateIdx = 0x1;

            /*drop packets*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
            RTK_PORTMASK_RESET(aclRule.act.forwardAct.portMask);

            HAL_GET_ALL_PORTMASK(aclRule.activePorts);

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }

            break;

        case 7:

            if ((ret = rtk_acl_init()) != RT_ERR_OK)
            {
                return ret;
            }
            
            HAL_SCAN_ALL_PORT(port)
            {
                state = ENABLED;
                if ((ret = rtk_acl_igrState_set(port, state)) != RT_ERR_OK)
                {
                    return ret;
                }
            }

            aclTemplate.index = 3;
            aclTemplate.fieldType[0] = ACL_FIELD_IPV6_SIP0;
            aclTemplate.fieldType[1] = ACL_FIELD_IPV6_SIP1;
            aclTemplate.fieldType[2] = ACL_FIELD_USER_DEFINED08;
            aclTemplate.fieldType[3] = ACL_FIELD_USER_DEFINED09;
            aclTemplate.fieldType[4] = ACL_FIELD_USER_DEFINED10;
            aclTemplate.fieldType[5] = ACL_FIELD_USER_DEFINED11;
            aclTemplate.fieldType[6] = ACL_FIELD_USER_DEFINED12;
            aclTemplate.fieldType[7] = ACL_FIELD_USER_DEFINED13;

            if((ret = rtk_acl_template_set(&aclTemplate))!= RT_ERR_OK)
            {
                return ret;
            }
            
            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 8;
            fieldSel.offset = 18;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            } 

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 9;
            fieldSel.offset = 16;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            } 

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 10;
            fieldSel.offset = 14;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            } 

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 11;
            fieldSel.offset = 12;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            } 

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 12;
            fieldSel.offset = 10;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            } 

            fieldSel.format = ACL_FORMAT_IPV6;
            fieldSel.index = 13;
            fieldSel.offset = 8;

            if((ret = rtk_acl_fieldSelect_set(&fieldSel))!= RT_ERR_OK)
            {
                return ret;
            } 


            osal_memset(&aclRule, 0, sizeof(rtk_acl_ingress_entry_t));
       
            osal_memset(&aclField, 0, sizeof(rtk_acl_field_t));
            aclField.fieldType = ACL_FIELD_IPV6_SIP0;
            aclField.fieldUnion.ip6.value.ipv6_addr[14]= 0x00;
            aclField.fieldUnion.ip6.value.ipv6_addr[15]= 0x00;
            aclField.fieldUnion.ip6.mask.ipv6_addr[14]= 0xFF;
            aclField.fieldUnion.ip6.mask.ipv6_addr[15]= 0xFF;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &aclField))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&aclField2, 0, sizeof(rtk_acl_field_t));
            aclField2.fieldType = ACL_FIELD_IPV6_SIP1;
            aclField2.fieldUnion.ip6.value.ipv6_addr[12]= 0x11;
            aclField2.fieldUnion.ip6.value.ipv6_addr[13]= 0x11;
            aclField2.fieldUnion.ip6.mask.ipv6_addr[12]= 0xFF;
            aclField2.fieldUnion.ip6.mask.ipv6_addr[13]= 0xFF;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &aclField2))!= RT_ERR_OK)
            {
                return ret;
            }

            osal_memset(&rawField08, 0, sizeof(rtk_acl_field_t));
            rawField08.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField08.fieldUnion.pattern.data.value = 0x2222;
            rawField08.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField08.fieldUnion.pattern.fieldIdx = 2;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &rawField08))!= RT_ERR_OK)
            {
                return ret;
            }
            
            osal_memset(&rawField09, 0, sizeof(rtk_acl_field_t));
            rawField09.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField09.fieldUnion.pattern.data.value = 0x3333;
            rawField09.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField09.fieldUnion.pattern.fieldIdx = 3;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &rawField09))!= RT_ERR_OK)
            {
                return ret;
            }
            
            osal_memset(&rawField10, 0, sizeof(rtk_acl_field_t));
            rawField10.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField10.fieldUnion.pattern.data.value = 0x4444;
            rawField10.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField10.fieldUnion.pattern.fieldIdx = 4;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &rawField10))!= RT_ERR_OK)
            {
                return ret;
            }
            
            osal_memset(&rawField11, 0, sizeof(rtk_acl_field_t));
            rawField11.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField11.fieldUnion.pattern.data.value = 0x5555;
            rawField11.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField11.fieldUnion.pattern.fieldIdx = 5;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &rawField11))!= RT_ERR_OK)
            {
                return ret;
            }
            
            osal_memset(&rawField12, 0, sizeof(rtk_acl_field_t));
            rawField12.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField12.fieldUnion.pattern.data.value = 0x6666;
            rawField12.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField12.fieldUnion.pattern.fieldIdx = 6;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &rawField12))!= RT_ERR_OK)
            {
                return ret;
            }
            
            osal_memset(&rawField13, 0, sizeof(rtk_acl_field_t));
            rawField13.fieldType = ACL_FIELD_PATTERN_MATCH;
            rawField13.fieldUnion.pattern.data.value = 0x7777;
            rawField13.fieldUnion.pattern.data.mask = 0xFFFF;
            rawField13.fieldUnion.pattern.fieldIdx = 7;
            
            if((ret = rtk_acl_igrField_add(&aclRule, &rawField13))!= RT_ERR_OK)
            {
                return ret;
            }

            aclRule.valid = ENABLED;
            aclRule.index = 4;
            aclRule.templateIdx = 0x3;

            /*care IPv6 only*/
            aclRule.careTag.tags[ACL_CARE_TAG_IPV6].value = 1;
            aclRule.careTag.tags[ACL_CARE_TAG_IPV6].mask = 1;

            /*drop packets*/
            aclRule.act.enableAct[ACL_IGR_FORWARD_ACT] = ENABLED;
            aclRule.act.forwardAct.act = ACL_IGR_FORWARD_REDIRECT_ACT;
            RTK_PORTMASK_RESET(aclRule.act.forwardAct.portMask);

            HAL_GET_ALL_PORTMASK(aclRule.activePorts);

            if((ret = rtk_acl_igrRuleEntry_add(&aclRule))!= RT_ERR_OK)
            {
                return ret;
            }
            
            break;
    }

    
    return CPARSER_OK;
}    /* end of cparser_cmd_debug_get_bond_chip_mode */


