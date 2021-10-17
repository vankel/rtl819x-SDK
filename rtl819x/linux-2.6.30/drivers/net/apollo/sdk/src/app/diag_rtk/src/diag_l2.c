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
 * $Revision: 16745 $
 * $Date: 2011-04-12 11:46:26 +0800 (Tue, 12 Apr 2011) $
 *
 * Purpose : Definition those XXX command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *
 */

/*
 * Include Files
 */
#include <stdio.h>
#include <string.h>
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <hal/common/halctrl.h>
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/l2.h>
#include <rtk/trap.h>
#if defined(CONFIG_SDK_APOLLO)
#include <dal/apollo/raw/apollo_raw_port.h>
#include <dal/apollo/raw/apollo_raw_l2.h>
#endif

#if defined(CONFIG_SDK_APOLLOMP)
#include <dal/apollomp/raw/apollomp_raw_port.h>
#include <dal/apollomp/raw/apollomp_raw_l2.h>
#endif

static uint32 efid = 0;

/* Static function */
static int32
_display_ipMcast_entry(rtk_l2_ipMcastAddr_t *pEntry)
{
    int32 ret;
    uint8 strSip[64];
	uint8 strDip[64];
    uint8 strExt[64];
    rtk_l2_ipmcMode_t   mode;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcMode_get(&mode), ret);

    if(pEntry->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY)
    {
        diag_util_ip2str(strDip,pEntry->dip);
        diag_util_mask2str(strExt, pEntry->ext_portmask.bits[0]);

        diag_util_mprintf("DestinationIP   Member Fwd Pri State  Ext    DipOnly ForceExt L3Route L3Idx\n");
        diag_util_mprintf("--------------- ------ --- --- ------ ------ ------- -------- ------- -----\n");

        diag_util_mprintf("%-15s %-6s %-3s %-3d %-6s %-6s %-7s %-8s %-7s %-5d\n",
                            strDip,
                            diag_util_mask32tostr(pEntry->portmask.bits[0]),
                            diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                            pEntry->priority,
                            diagStr_l2LutStaticOrAutoStr[(pEntry->flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1 : 0],
                            strExt,
                            diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY) ? 1 : 0],
                            diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE) ? 1 : 0],
                            diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN) ? 1 : 0],
                            pEntry->l3_trans_index);
    }
    else if(LOOKUP_ON_DIP_AND_VID == mode)
    {
        diag_util_ip2str(strDip,pEntry->dip);
        diag_util_mask2str(strExt, pEntry->ext_portmask.bits[0]);

        diag_util_mprintf("DestinationIP   Vid  Member Fwd Pri State  Ext    DipOnly \n");
        diag_util_mprintf("--------------- ---- ------ --- --- ------ ------ ------- \n");

        diag_util_mprintf("%-15s %-4d %-6s %-3s %-3d %-6s %-6s %-5s\n",
                            strDip,
                            pEntry->vid,
                            diag_util_mask32tostr(pEntry->portmask.bits[0]),
                            diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                            pEntry->priority,
                            diagStr_l2LutStaticOrAutoStr[(pEntry->flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1 : 0],
                            strExt,
                            diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY) ? 1 : 0]);
    }
    else /* LOOKUP_ON_DIP_AND_SIP */
    {
        diag_util_ip2str(strDip,pEntry->dip);
        diag_util_ip2str(strSip,pEntry->sip);
        diag_util_mask2str(strExt, pEntry->ext_portmask.bits[0]);

        diag_util_mprintf("DestinationIP   SourceIP        Member Fwd Pri State  Ext    DipOnly \n");
        diag_util_mprintf("--------------- --------------- ------ --- --- ------ ------ ------- \n");

        diag_util_mprintf("%-15s %-15s %-6s %-3s %-3d %-6s %-6s %-7s\n",
                            strDip,
                            strSip,
                            diag_util_mask32tostr(pEntry->portmask.bits[0]),
                            diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                            pEntry->priority,
                            diagStr_l2LutStaticOrAutoStr[(pEntry->flags & RTK_L2_IPMCAST_FLAG_STATIC) ? 1 : 0],
                            strExt,
                            diagStr_enDisplay[(pEntry->flags & RTK_L2_IPMCAST_FLAG_DIP_ONLY) ? 1 : 0]);
    }

    return RT_ERR_OK;
}

static int32
_display_l2Mcast_entry(rtk_l2_mcastAddr_t *pMcastAddr)
{
    uint8 strExt[64];

    if(pMcastAddr->flags & RTK_L2_MCAST_FLAG_IVL)
    {
        diag_util_mask2str(strExt, pMcastAddr->ext_portmask.bits[0]);

     	diag_util_mprintf("MACAddress         Member VID  FwdPriEn Pri Ext   \n");
        diag_util_mprintf("------------------ ------ ---- -------- --- ------\n");
		diag_util_mprintf("%-18s %-6s %-4d %-8s %-3d %-6s \n",
			                    diag_util_inet_mactoa(&pMcastAddr->mac.octet[0]),
			                    diag_util_mask32tostr(pMcastAddr->portmask.bits[0]),
                                pMcastAddr->vid,
                                diagStr_enDisplay[(pMcastAddr->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                                pMcastAddr->priority,
                                strExt);
    }
    else
    {
        diag_util_mask2str(strExt, pMcastAddr->ext_portmask.bits[0]);

     	diag_util_mprintf("MACAddress         Member FID  FwdPriEn Pri Ext   \n");
        diag_util_mprintf("------------------ ------ ---- -------- --- ------\n");
		diag_util_mprintf("%-18s %-6s %-4d %-8s %-3d %-6s \n",
			                    diag_util_inet_mactoa(&pMcastAddr->mac.octet[0]),
			                    diag_util_mask32tostr(pMcastAddr->portmask.bits[0]),
                                pMcastAddr->fid,
                                diagStr_enDisplay[(pMcastAddr->flags & RTK_L2_IPMCAST_FLAG_FWD_PRI) ? 1 : 0],
                                pMcastAddr->priority,
                                strExt);
    }

    return RT_ERR_OK;
}

static int32
_display_l2Ucast_entry(rtk_l2_ucastAddr_t *pL2Addr)
{
    diag_util_mprintf("                  Spa/                             Priority         Block\n");
    diag_util_mprintf("MACAddress        Ext Fid Efid Age Vid  State  LRN Sa  Fwd Pri Auth Da  Sa  Arp\n");
    diag_util_mprintf("----------------- --- --- ---- --- ---- ------ --- --- --- --- ---- --- --- ---\n");
    diag_util_mprintf("%-17s %d/%d %-3d %-4d %-3d %-4d %-6s %-3s %-3s %-3s %-3d %-4s %-3s %-3s %-3s\n",
			                    diag_util_inet_mactoa(&pL2Addr->mac.octet[0]),
			                    pL2Addr->port,
                                pL2Addr->ext_port,
			                    pL2Addr->fid,
			                    pL2Addr->efid,
			                    pL2Addr->age,
			                    pL2Addr->vid,
			                    diagStr_l2LutStaticOrAutoStr[(pL2Addr->flags & RTK_L2_UCAST_FLAG_STATIC) ? 1 : 0],
			                    diagStr_l2HashMethodStr[(pL2Addr->flags & RTK_L2_UCAST_FLAG_IVL) ? 1 : 0],
                                diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_LOOKUP_PRI) ? 1 : 0],
                                diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_FWD_PRI) ? 1 : 0],
                                pL2Addr->priority,
                                diagStr_enDisplay[pL2Addr->auth],
                                diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_DA_BLOCK) ? 1 : 0],
                                diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_SA_BLOCK) ? 1 : 0],
                                diagStr_enDisplay[(pL2Addr->flags & RTK_L2_UCAST_FLAG_ARP_USED) ? 1 : 0]);

    return RT_ERR_OK;
}

#if defined(CONFIG_SDK_APOLLO)
static void
_diag_lutDisplay(apollo_lut_table_t *diag_lut)
{
	uint8 strBuffer[64];
	uint8 strDsl[64];
	uint8 strExt[64];
	uint8 strSip[64];
	uint8 strDip[64];

    apollo_raw_l2_ipMcHashType_t ipMcHashType;



	if(diag_lut->lookup_busy)
	{
		diag_util_mprintf("\nLUT Access Busy\n");
	}
    else if(!diag_lut->lookup_hit)
    {
        diag_util_mprintf("\nLUT Access Error\n");
    }
    else
    {
        diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",diag_lut->address,(diag_lut->address > (APOLLO_LUT_4WAY_NO-1))?"(64 BCAM)":"(2K LUT)");

        if(diag_lut->l3lookup)
        {
            apollo_raw_l2_ipmcHashType_get(&ipMcHashType);


/*
#ifdef _LITTLE_ENDIAN
		    diag_lut->gip = diag_lut->gip | 0xE0;
#else
*/
            diag_lut->gip = diag_lut->gip | 0xE0000000;
/*#endif*/
            diag_util_ip2str(strSip,diag_lut->sip_vid);
            diag_util_ip2str(strDip,diag_lut->gip);
            diag_util_mask2str(strDsl, diag_lut->dsl_mbr);
            diag_util_mask2str(strExt, diag_lut->ext_mbr);

    		diag_util_mprintf("IP Multicast table:\n");

            if(ipMcHashType == RAW_LUT_IPMCHASH_TYPE_GIPVID)
            {
                diag_util_mprintf("DestinationIP   Vid  Member Fwd Pri State  Ext   Dsl   DipOnly\n");

        		diag_util_mprintf("%-15s %-4d %-6s %-3s %-3d %-6s %-5s %-5s %s\n",
                                    strDip,
                                    diag_lut->sip_vid,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt,
                                    strDsl,
                                    diagStr_enDisplay[diag_lut->gip_only]);
            }
            else
            {
                diag_util_mprintf("DestinationIP   SourceIP        Member Fwd Pri State  Ext   Dsl\n");

        		diag_util_mprintf("%-15s %-15s %-6s %-3s %-3d %-6s %-5s %-5s %s\n",
                                    strDip,
                                    strSip,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt,
                                    strDsl,
                                    diagStr_enDisplay[diag_lut->gip_only]);
            }
        }
        else if(diag_lut->mac.octet[0]&0x01)
        {
            diag_util_mask2str(strDsl, diag_lut->dsl_mbr);
            diag_util_mask2str(strExt, diag_lut->ext_mbr);

            diag_util_mprintf("L2 Multicast table:\n");
			diag_util_mprintf("MACAddress         Member Fid_Vid FwdPriEn Pri State  Hash Ext   Dsl\n");

			diag_util_mprintf("%-18s %-6s %-7d %-8s %-3d %-6s %-4s %-5s %-s\n",
			                    diag_util_inet_mactoa(&diag_lut->mac.octet[0]),
			                    diag_util_mask32tostr(diag_lut->mbr),
			                    diag_lut->cvid_fid,
			                    diagStr_enDisplay[diag_lut->fwdpri_en],
			                    diag_lut->lut_pri,
			                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
			                    diagStr_l2HashMethodStr[diag_lut->ivl_svl],
                                strExt,
                                strDsl);
        }
        else
        {
            diag_util_mprintf("L2 Unicast table:\n");
			diag_util_mprintf("MACAddress        Spa Fid Efid Age Vid  State  Hash\n");
 			diag_util_mprintf("%-17s %-3d %-3d %-4d %-3d %-4d %-6s %s\n",
			                    diag_util_inet_mactoa(&diag_lut->mac.octet[0]),
			                    diag_lut->spa,
			                    diag_lut->fid,
			                    diag_lut->efid,
			                    diag_lut->age,
			                    diag_lut->cvid_fid,
			                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
			                    diagStr_l2HashMethodStr[diag_lut->ivl_svl]);

			diag_util_mprintf("SaPriEn FwdPriEn Pri Auth DaBlock SaBlock Arp ExtDsl\n");
 			diag_util_mprintf("%-7s %-8s %-3d %-4s %-7s %-7s %-3s %d\n",
			                    diagStr_enDisplay[diag_lut->sapri_en],
                                diagStr_enDisplay[diag_lut->fwdpri_en],
                                diag_lut->lut_pri,
                                diagStr_enDisplay[diag_lut->auth],
                                diagStr_enDisplay[diag_lut->da_block],
                                diagStr_enDisplay[diag_lut->sa_block],
                                diagStr_enDisplay[diag_lut->arp_used],
                                diag_lut->ext_dsl_spa);
		}
    }
}
#endif

#if defined(CONFIG_SDK_APOLLOMP)
static void
_diag_apollomp_lutDisplay(apollomp_lut_table_t *diag_lut)
{
	uint8 strBuffer[64];
	uint8 strDsl[64];
	uint8 strExt[64];
	uint8 strSip[64];
	uint8 strDip[64];

    apollomp_raw_l2_ipMcHashType_t ipMcHashType;



	if(diag_lut->lookup_busy)
	{
		diag_util_mprintf("\nLUT Access Busy\n");
	}
    else if(!diag_lut->lookup_hit)
    {
        diag_util_mprintf("\nLUT Access Error\n");
    }
    else
    {
        diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",diag_lut->address,(diag_lut->address > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");

        if(diag_lut->l3lookup)
        {
            apollomp_raw_l2_ipmcHashType_get(&ipMcHashType);


/*
#ifdef _LITTLE_ENDIAN
		    diag_lut->gip = diag_lut->gip | 0xE0;
#else
*/
            diag_lut->gip = diag_lut->gip | 0xE0000000;
/*#endif*/
            diag_util_ip2str(strSip,diag_lut->sip_vid);
            diag_util_ip2str(strDip,diag_lut->gip);
            diag_util_mask2str(strDsl, diag_lut->dsl_mbr);
            diag_util_mask2str(strExt, diag_lut->ext_mbr);

    		diag_util_mprintf("IP Multicast table:\n");

            if(ipMcHashType == APOLLOMP_RAW_LUT_IPMCHASH_TYPE_GIPVID)
            {
                diag_util_mprintf("DestinationIP   Vid  Member Fwd Pri State  Ext   Dsl   DipOnly\n");

        		diag_util_mprintf("%-15s %-4d %-6s %-3s %-3d %-6s %-5s %-5s %s\n",
                                    strDip,
                                    diag_lut->sip_vid,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt,
                                    strDsl,
                                    diagStr_enDisplay[diag_lut->gip_only]);
            }
            else
            {
                diag_util_mprintf("DestinationIP   SourceIP        Member Fwd Pri State  Ext   Dsl\n");

        		diag_util_mprintf("%-15s %-15s %-6s %-3s %-3d %-6s %-5s %-5s %s\n",
                                    strDip,
                                    strSip,
                                    diag_util_mask32tostr(diag_lut->mbr),
                                    diagStr_enDisplay[diag_lut->fwdpri_en],
                                    diag_lut->lut_pri,
                                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
                                    strExt,
                                    strDsl,
                                    diagStr_enDisplay[diag_lut->gip_only]);
            }
        }
        else if(diag_lut->mac.octet[0]&0x01)
        {
            diag_util_mask2str(strDsl, diag_lut->dsl_mbr);
            diag_util_mask2str(strExt, diag_lut->ext_mbr);

            diag_util_mprintf("L2 Multicast table:\n");
			diag_util_mprintf("MACAddress         Member Fid_Vid FwdPriEn Pri State  Hash Ext   Dsl\n");

			diag_util_mprintf("%-18s %-6s %-7d %-8s %-3d %-6s %-4s %-5s %-s\n",
			                    diag_util_inet_mactoa(&diag_lut->mac.octet[0]),
			                    diag_util_mask32tostr(diag_lut->mbr),
			                    diag_lut->cvid_fid,
			                    diagStr_enDisplay[diag_lut->fwdpri_en],
			                    diag_lut->lut_pri,
			                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
			                    diagStr_l2HashMethodStr[diag_lut->ivl_svl],
                                strExt,
                                strDsl);
        }
        else
        {
            diag_util_mprintf("L2 Unicast table:\n");
			diag_util_mprintf("MACAddress        Spa Fid Efid Age Vid  State  Hash\n");
 			diag_util_mprintf("%-17s %-3d %-3d %-4d %-3d %-4d %-6s %s\n",
			                    diag_util_inet_mactoa(&diag_lut->mac.octet[0]),
			                    diag_lut->spa,
			                    diag_lut->fid,
			                    diag_lut->efid,
			                    diag_lut->age,
			                    diag_lut->cvid_fid,
			                    diagStr_l2LutStaticOrAutoStr[diag_lut->nosalearn],
			                    diagStr_l2HashMethodStr[diag_lut->ivl_svl]);

			diag_util_mprintf("SaPriEn FwdPriEn Pri Auth DaBlock SaBlock Arp ExtDsl\n");
 			diag_util_mprintf("%-7s %-8s %-3d %-4s %-7s %-7s %-3s %d\n",
			                    diagStr_enDisplay[diag_lut->sapri_en],
                                diagStr_enDisplay[diag_lut->fwdpri_en],
                                diag_lut->lut_pri,
                                diagStr_enDisplay[diag_lut->auth],
                                diagStr_enDisplay[diag_lut->da_block],
                                diagStr_enDisplay[diag_lut->sa_block],
                                diagStr_enDisplay[diag_lut->arp_used],
                                diag_lut->ext_dsl_spa);
		}
    }
}
#endif

/*
 * l2-table init
 */
cparser_result_t
cparser_cmd_l2_table_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_init */

/*
 * l2-table del all { include-static }
 */
cparser_result_t
cparser_cmd_l2_table_del_all_include_static(
    cparser_context_t *context)
{
    int32 ret;
    uint32 includeStatic;

    DIAG_UTIL_PARAM_CHK();

    if(4 == TOKEN_NUM())
    {
        includeStatic = 1;
    }
    else
    {
        includeStatic = 0;
    }
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_delAll(includeStatic), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_all_include_static */

/*
 * l2-table del ip-mcast dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_dip_dip(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_dip_dip */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> port none
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_none(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    RTK_PORTMASK_RESET(ipmcAddr.portmask);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_none */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_ports_all(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_ports_all */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all ) l3-interface <UINT:index>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_ports_all_l3_interface_index(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    ipmcAddr.l3_trans_index = *index_ptr;
    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_port_ports_all_l3_interface_index */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_priority_priority(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    ipmcAddr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_priority_priority */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    if('e' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_priority_state_disable_enable */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *dip_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_ext_ext_all_none */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> l3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_l3routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    if('e' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else if('d' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_L3MC_ROUTE_EN;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_l3routing_state_disable_enable */

/*
 * l2-table add ip-mcast dip <IPV4ADDR:dip> forcedl3routing state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_dip_dip_forcedl3routing_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    }

    if('e' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE;
    else if('d' == TOKEN_CHAR(7, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_FORCE_EXT_ROUTE;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_dip_dip_forcedl3routing_state_disable_enable */

/*
 * l2-table get ip-mcast dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_dip_dip(
    cparser_context_t *context,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_DIP_ONLY;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);


    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
	_display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_dip_dip */

/*
 * l2-table del ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_sip_sip_dip_dip(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_sip_sip_dip_dip */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_port_ports_all_none */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_priority_priority(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    ipmcAddr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_priority_priority */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    if('e' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_priority_state_disable_enable */

/*
 * l2-table add ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_sip_sip_dip_dip_ext_ext_all_none */

/*
 * l2-table get ip-mcast sip <IPV4ADDR:sip> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_sip_sip_dip_dip(
    cparser_context_t *context,
    uint32_t  *sip_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.sip = (ipaddr_t)(*sip_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
	_display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_sip_sip_dip_dip */

/*
 * l2-table del ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_del_ip_mcast_vid_vid_dip_dip(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_del(&ipmcAddr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_ip_mcast_vid_vid_dip_dip */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_port_ports_all_none */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_priority_priority(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    ipmcAddr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_priority_priority */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    if('e' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        ipmcAddr.flags &= ~RTK_L2_IPMCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_priority_state_disable_enable */

/*
 * l2-table add ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    ret = rtk_l2_ipMcastAddr_get(&ipmcAddr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
        ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
        ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
        ipmcAddr.vid = (uint32)(*vid_ptr);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(ipmcAddr.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_add(&ipmcAddr), ret);

    diag_util_printf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_ip_mcast_vid_vid_dip_dip_ext_ext_all_none */

/*
 * l2-table get ip-mcast vid <UINT:vid> dip <IPV4ADDR:dip>
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_vid_vid_dip_dip(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    uint32_t  *dip_ptr)
{
    int32 ret;
    rtk_l2_ipMcastAddr_t ipmcAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&ipmcAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));
    ipmcAddr.flags |= RTK_L2_IPMCAST_FLAG_STATIC;
    ipmcAddr.dip = (ipaddr_t)(*dip_ptr);
    ipmcAddr.vid = (uint32)(*vid_ptr);
    DIAG_UTIL_ERR_CHK(rtk_l2_ipMcastAddr_get(&ipmcAddr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",ipmcAddr.index,(ipmcAddr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_ipMcast_entry(&ipmcAddr);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_vid_vid_dip_dip */

/*
 * l2-table del mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_mcast_filter_id_fid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_del(&l2Mcast), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_mcast_filter_id_fid_mac_address_mac */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_port_ports_all_none */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Mcast.flags &= ~RTK_L2_MCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_priority_state_disable_enable */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_priority_priority(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Mcast.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_priority_priority */

/*
 * l2-table add mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.fid = (rtk_fid_t)*fid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_filter_id_fid_mac_address_mac_ext_ext_all_none */

/*
 * l2-table get mac-mcast filter-id <UINT:fid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_mcast_filter_id_fid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.fid = (rtk_fid_t)*fid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_get(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Mcast_entry(&l2Mcast);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_mcast_filter_id_fid_mac_address_mac */

/*
 * l2-table del mac-mcast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_mcast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_del(&l2Mcast), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_mcast_vid_vid_mac_address_mac */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_port_ports_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_port_ports_all_none */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Mcast.flags &= ~RTK_L2_MCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_priority_state_disable_enable */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_priority_priority(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Mcast.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_priority_priority */

/*
 * l2-table add mac-mcast vid <UINT:vid> mac-address <MACADDR:mac> ext ( <PORT_LIST:ext> | all | none )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_ext_ext_all_none(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    char * *ext_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 8), ret);

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_mcastAddr_get(&l2Mcast);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
        l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
        l2Mcast.vid = (rtk_fid_t)*vid_ptr;
        osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    RTK_PORTMASK_FROM_UINT_PORTMASK((&(l2Mcast.ext_portmask)), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_add(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_mcast_vid_vid_mac_address_mac_ext_ext_all_none */

/*
 * l2-table get mac-mcast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_mcast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_mcastAddr_t l2Mcast;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Mcast, 0x00, sizeof(rtk_l2_mcastAddr_t));
    l2Mcast.flags |= RTK_L2_MCAST_FLAG_IVL;
    l2Mcast.vid = (rtk_fid_t)*vid_ptr;
    osal_memcpy(&l2Mcast.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_mcastAddr_get(&l2Mcast), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Mcast.index,(l2Mcast.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Mcast_entry(&l2Mcast);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_mcast_vid_vid_mac_address_mac */

/*
 * l2-table del mac-ucast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_ucast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_del(&l2Addr), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_ucast_vid_vid_mac_address_mac */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_spn_port(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.port = *port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_spn_port */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> filter-id <UINT:fid>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_filter_id_fid(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.fid = *fid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_filter_id_fid */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> age <UINT:age>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_age_age(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *age_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.age = *age_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_age_age */

/*
 * l2-table set mac-ucast enhanced-filter-id <UINT:efid>
 */
cparser_result_t
cparser_cmd_l2_table_set_mac_ucast_enhanced_filter_id_efid(
    cparser_context_t *context,
    uint32 *efid_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    efid = *efid_ptr;

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_mac_ucast_enhanced_filter_id_efid */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_priority_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> sa-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_sa_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_LOOKUP_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_LOOKUP_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_sa_priority_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_priority_priority(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_priority_priority */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> arp-usage state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_arp_usage_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_ARP_USED;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_ARP_USED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_arp_usage_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> auth state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_auth_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.auth = ENABLED;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.auth = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_auth_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> da-block state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_da_block_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_DA_BLOCK;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_da_block_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> sa-block state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_sa_block_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_SA_BLOCK;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_sa_block_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> static state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_static_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_STATIC;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_STATIC;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_static_state_disable_enable */

/*
 * l2-table add mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> ext-spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_ext_spn_port(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
        l2Addr.vid = *vid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.ext_port = *port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_vid_vid_mac_address_mac_ext_spn_port */

/*
 * l2-table del mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_del_mac_ucast_filter_id_fid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_del(&l2Addr), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_del_mac_ucast_filter_id_fid_mac_address_mac */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_spn_port(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.port = *port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_spn_port */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_vid_vid(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.vid = *vid_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_vid_vid */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> age <UINT:age>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_age_age(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *age_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.age = *age_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_age_age */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_FWD_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_FWD_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_priority_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> sa-priority state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_sa_priority_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_LOOKUP_PRI;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_LOOKUP_PRI;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_sa_priority_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_priority_priority(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *priority_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.priority = *priority_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_priority_priority */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> arp-usage state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_arp_usage_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_ARP_USED;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_ARP_USED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_arp_usage_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> auth state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_auth_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.auth = ENABLED;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.auth = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_auth_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> da-block state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_da_block_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_DA_BLOCK;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_DA_BLOCK;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_da_block_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> sa-block state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_sa_block_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_SA_BLOCK;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_SA_BLOCK;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_sa_block_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> static state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_static_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    if('e' == TOKEN_CHAR(9, 0))
        l2Addr.flags |= RTK_L2_UCAST_FLAG_STATIC;
    else if('d' == TOKEN_CHAR(9, 0))
        l2Addr.flags &= ~RTK_L2_UCAST_FLAG_STATIC;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_static_state_disable_enable */

/*
 * l2-table add mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> ext-spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_ext_spn_port(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    ret = rtk_l2_addr_get(&l2Addr);

    if(RT_ERR_OK != ret)
    {
        osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
        l2Addr.fid = *fid_ptr;
        l2Addr.efid = efid;
        osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);
    }

    l2Addr.ext_port = *port_ptr;
    DIAG_UTIL_ERR_CHK(rtk_l2_addr_add(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_add_mac_ucast_filter_id_fid_mac_address_mac_ext_spn_port */

/*
 * l2-table get mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac> enhanced-filter-id <UINT:efid>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_ucast_filter_id_fid_mac_address_mac_enhanced_filter_id_efid(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = *efid_ptr;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Ucast_entry(&l2Addr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_ucast_filter_id_fid_mac_address_mac_enhanced_filter_id_efid */

/*
 * l2-table get mac-ucast filter-id <UINT:fid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_ucast_filter_id_fid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *fid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.fid = *fid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Ucast_entry(&l2Addr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_ucast_filter_id_fid_mac_address_mac */

/*
 * l2-table get mac-ucast vid <UINT:vid> mac-address <MACADDR:mac> enhanced-filter-id <UINT:efid>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_ucast_vid_vid_mac_address_mac_enhanced_filter_id_efid(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = *efid_ptr;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Ucast_entry(&l2Addr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_ucast_vid_vid_mac_address_mac_enhanced_filter_id_efid */

/*
 * l2-table get mac-ucast vid <UINT:vid> mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_l2_table_get_mac_ucast_vid_vid_mac_address_mac(
    cparser_context_t *context,
    uint32_t  *vid_ptr,
    cparser_macaddr_t  *mac_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_l2_ucastAddr_t l2Addr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    osal_memset(&l2Addr, 0x00, sizeof(rtk_l2_ucastAddr_t));
    l2Addr.flags |= RTK_L2_UCAST_FLAG_IVL;
    l2Addr.vid = *vid_ptr;
    l2Addr.efid = efid;
    osal_memcpy(&l2Addr.mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);

    DIAG_UTIL_ERR_CHK(rtk_l2_addr_get(&l2Addr), ret);

    diag_util_mprintf("\nLUT address: 0x%4.4x %s\n",l2Addr.index,(l2Addr.index > (HAL_L2_LEARN_4WAY_NO()-1))?"(64 BCAM)":"(2K LUT)");
    _display_l2Ucast_entry(&l2Addr);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_mac_ucast_vid_vid_mac_address_mac */

/*
 * l2-table get aging-out port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_l2_table_get_aging_out_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portAgingEnable_get(port, &state), ret);
        diag_util_printf("\n Port %d Age state: %s", port, diagStr_enable[state]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_aging_out_port_ports_all_state */

/*
 * l2-table set aging-out port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_aging_out_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portAgingEnable_set(port, state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_aging_out_port_ports_all_state_disable_enable */

/*
 * l2-table get aging-time
 */
cparser_result_t
cparser_cmd_l2_table_get_aging_time(
    cparser_context_t *context)
{
    int32 ret;
    uint32 time;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_aging_get(&time), ret);
    diag_util_printf("\n Age Time: %d", time);

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_aging_time */

/*
 * l2-table set aging-time <UINT:time>
 */
cparser_result_t
cparser_cmd_l2_table_set_aging_time_time(
    cparser_context_t *context,
    uint32_t  *time_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_aging_set(*time_ptr), ret);
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_aging_time_time */

/*
 * l2-table get cam state
 */
cparser_result_t
cparser_cmd_l2_table_get_cam_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_camEnable_get(&state), ret);
            diag_util_printf("\n Cam state: %s", diagStr_enable[state]);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_camEnable_get(&state), ret);
            diag_util_printf("\n Cam state: %s", diagStr_enable[state]);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_cam_state */

/*
 * l2-table set cam state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_cam_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(4, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_camEnable_set(state), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_camEnable_set(state), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_cam_state_disable_enable */

/*
 * l2-table get limit-learning action
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_action(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_limitLearnCntAction_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCntAction_get(&action), ret);

    diag_util_printf("\n System learning Over Action: %s", diagStr_actionStr[action]);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_action */

/*
 * l2-table set limit-learning action ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_action_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    if('c' == TOKEN_CHAR(4, 0))
        action = ACTION_COPY2CPU;
    else if('d' == TOKEN_CHAR(4, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(4, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(4, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCntAction_set((rtk_l2_limitLearnCntAction_t)action), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_action_copy_to_cpu_drop_forward_trap_to_cpu */

/*
 * l2-table get limit-learning count
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_count(
    cparser_context_t *context)
{
    int32 ret;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCnt_get(&cnt), ret);

    diag_util_printf("\n System Learning Limit: %d\n", cnt);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_count */

/*
 * l2-table set limit-learning count <UINT:count>
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_count_count(
    cparser_context_t *context,
    uint32_t  *count_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCnt_set(*count_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_count_count */

/*
 * l2-table set limit-learning count unlimited
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_count_unlimited(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_l2_limitLearningCnt_set(HAL_L2_LEARN_LIMIT_CNT_MAX()), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_count_unlimited */



/*
 * l2-table get limit-learning port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_limitLearnCntAction_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCntAction_get(port, &action), ret);
        diag_util_printf("\n Port %d learning limit over action: %s", port, diagStr_actionStr[action]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_port_ports_all */

/*
 * l2-table set limit-learning port ( <PORT_LIST:ports> | all ) ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_port_ports_all_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_limitLearnCntAction_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('c' == TOKEN_CHAR(5, 0))
        action = LIMIT_LEARN_CNT_ACTION_COPY_CPU;
    else if('d' == TOKEN_CHAR(5, 0))
        action = LIMIT_LEARN_CNT_ACTION_DROP;
    else if('f' == TOKEN_CHAR(5, 0))
        action = LIMIT_LEARN_CNT_ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(5, 0))
        action = LIMIT_LEARN_CNT_ACTION_TO_CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCntAction_set(port, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_port_ports_all_copy_to_cpu_drop_forward_trap_to_cpu */

/*
 * l2-table get limit-learning port ( <PORT_LIST:ports> | all ) count
 */
cparser_result_t
cparser_cmd_l2_table_get_limit_learning_port_ports_all_count(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_get(port, &cnt), ret);
        diag_util_printf("\n Port %d learning limit: %d", port, cnt);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_limit_learning_port_ports_all_count */

/*
 * l2-table set limit-learning port ( <PORT_LIST:ports> | all ) count <UINT:count>
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_port_ports_all_count_count(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *count_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_set(port, *count_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_port_ports_all_count_count */

/*
 * l2-table set limit-learning port ( <PORT_LIST:ports> | all ) count unlimited
 */
cparser_result_t
cparser_cmd_l2_table_set_limit_learning_port_ports_all_count_unlimited(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLimitLearningCnt_set(port, HAL_L2_LEARN_LIMIT_CNT_MAX()), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_limit_learning_port_ports_all_count_unlimited */

/*
 * l2-table get learning-count
 */
cparser_result_t
cparser_cmd_l2_table_get_learning_count(
    cparser_context_t *context)
{
    int32 ret;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_learningCnt_get(&cnt), ret);

    diag_util_printf("\n System Learning Counter: %d\n", cnt);


    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_learning_count */

/*
 * l2-table get learning-count port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l2_table_get_learning_count_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 cnt;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLearningCnt_get(port, &cnt), ret);
        diag_util_printf("\n Port %d learning counter: %d", port, cnt);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_learning_count_port_ports_all */



/*
 * l2-table get link-down-flush state
 */
cparser_result_t
cparser_cmd_l2_table_get_link_down_flush_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_flushLinkDownPortAddrEnable_get(&state), ret);
    diag_util_printf("\n Link down flush state: %s", diagStr_enable[state]);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_link_down_flush_state */

/*
 * l2-table set link-down-flush state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_link_down_flush_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(4, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_flushLinkDownPortAddrEnable_set(state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_link_down_flush_state_disable_enable */

/*
 * l2-table get lookup-miss multicast trap-priority
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_multicast_trap_priority(
    cparser_context_t *context)
{
    int32 ret;
    rtk_pri_t priority;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_trap_reasonTrapToCpuPriority_get(TRAP_REASON_MULTICASTDLF, &priority), ret);
    diag_util_printf("\n Lookup-miss multicast trap-priority: %d", priority);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_multicast_trap_priority */

/*
 * l2-table set lookup-miss multicast trap-priority <UINT:priority>
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_multicast_trap_priority_priority(
    cparser_context_t *context,
    uint32_t  *priority_ptr)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trap_reasonTrapToCpuPriority_set(TRAP_REASON_MULTICASTDLF, (rtk_pri_t)*priority_ptr), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_multicast_trap_priority_priority */

/*
 * l2-table get lookup-miss ( broadcast | unicast | multicast ) flood-ports
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_broadcast_unicast_multicast_flood_ports(
    cparser_context_t *context)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_l2_lookupMissType_t type;
    rtk_portmask_t portmask;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('b' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_BCAST;
    else if('u' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_UCAST;
    else if('m' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_MCAST;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_get(type, &portmask), ret);
    diag_util_printf("\n %s Lookup miss flood portmask: %s", diagStr_lookupmissType[type], diag_util_mask32tostr(portmask.bits[0]));
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_broadcast_unicast_multicast_flood_ports */

/*
 * l2-table set lookup-miss ( broadcast | unicast | multicast ) flood-ports ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_broadcast_unicast_multicast_flood_ports_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;
    rtk_l2_lookupMissType_t type;
    rtk_portmask_t portmask;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    if('b' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_BCAST;
    else if('u' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_UCAST;
    else if('m' == TOKEN_CHAR(3, 0))
        type = DLF_TYPE_MCAST;
    else
        return CPARSER_NOT_OK;

    RTK_PORTMASK_FROM_UINT_PORTMASK((&portmask), &(portlist.portmask.bits[0]));
    DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_set(type, &portmask), ret);

    if(DLF_TYPE_MCAST == type)
        DIAG_UTIL_ERR_CHK(rtk_l2_lookupMissFloodPortMask_set(DLF_TYPE_IPMC, &portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_broadcast_unicast_multicast_flood_ports_ports_all */

/*
 * l2-table get lookup-miss port ( <PORT_LIST:ports> | all ) ( multicast | ip-mcast | ip6-mcast | unicast ) action
 */
cparser_result_t
cparser_cmd_l2_table_get_lookup_miss_port_ports_all_multicast_ip_mcast_ip6_mcast_unicast_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_lookupMissType_t type;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('l' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_MCAST;
    else if('-' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_IPMC;
    else if('6' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_IP6MC;
    else if('i' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_UCAST;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_get(port, type, &action), ret);
        if(DLF_TYPE_MCAST == type)
            diag_util_printf("\n Port %d lookup-miss multicast Action: %s", port, diagStr_actionStr[action]);
        else if(DLF_TYPE_IPMC == type)
            diag_util_printf("\n Port %d lookup-miss IPv4 multicast Action: %s", port, diagStr_actionStr[action]);
        else if(DLF_TYPE_IP6MC == type)
            diag_util_printf("\n Port %d lookup-miss IPv6 multicast Action: %s", port, diagStr_actionStr[action]);
        else if(DLF_TYPE_UCAST == type)
            diag_util_printf("\n Port %d lookup-miss Unicast Action: %s", port, diagStr_actionStr[action]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_lookup_miss_port_ports_all_multicast_ip_mcast_ip6_mcast_unicast_action */

/*
 * l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) ( ip-mcast | ip6-mcast ) action ( drop | flood-in-vlan | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_port_ports_all_ip_mcast_ip6_mcast_action_drop_flood_in_vlan_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_lookupMissType_t type;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('-' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_IPMC;
    else if('6' == TOKEN_CHAR(5, 2))
        type = DLF_TYPE_IP6MC;
    else
        return CPARSER_NOT_OK;

    if('d' == TOKEN_CHAR(7, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(7, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(7, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_set(port, type, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_port_ports_all_ip_mcast_ip6_mcast_action_drop_flood_in_vlan_trap_to_cpu */

/*
 * l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) multicast action ( drop | drop-exclude-rma | flood-in-vlan | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_port_ports_all_multicast_action_drop_drop_exclude_rma_flood_in_vlan_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('-' == TOKEN_CHAR(7, 4))
        action = ACTION_DROP_EXCLUDE_RMA;
    else if('d' == TOKEN_CHAR(7, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(7, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(7, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_set(port, DLF_TYPE_MCAST, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_port_ports_all_multicast_action_drop_drop_exclude_rma_flood_in_vlan_trap_to_cpu */

/*
 * l2-table set lookup-miss port ( <PORT_LIST:ports> | all ) unicast action ( drop | flood-in-vlan | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_lookup_miss_port_ports_all_unicast_action_drop_flood_in_vlan_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('d' == TOKEN_CHAR(7, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(7, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(7, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_l2_portLookupMissAction_set(port, DLF_TYPE_UCAST, action), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_lookup_miss_port_ports_all_unicast_action_drop_flood_in_vlan_trap_to_cpu */

/*
 * l2-table set ip-mcast-mode ( dip-and-sip | dip-and-vid | vid-and-mac )
 */
cparser_result_t
cparser_cmd_l2_table_set_ip_mcast_mode_dip_and_sip_dip_and_vid_vid_and_mac(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcMode_t mode;

    DIAG_UTIL_PARAM_CHK();

    if('s' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_SIP;
    else if('v' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_DIP_AND_VID;
    else if('m' == TOKEN_CHAR(3, 8))
        mode = LOOKUP_ON_MAC_AND_VID_FID;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcMode_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_ip_mcast_mode_dip_and_sip_dip_and_vid_vid_and_mac */

/*
 * l2-table get ip-mcast-mode
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_mode(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_ipmcMode_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_ipmcMode_get(&mode), ret);

    diag_util_printf("\n IPMC mode : %s", diagStr_l2IpMcHashMethodStr[mode]);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_mode */

/*
 * l2-table get ( port-move | unknown-sa ) port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_l2_table_get_port_move_unknown_sa_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_l2_newMacLrnMode_t mode;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if('p' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_illegalPortMoveAction_get(port, &action), ret);
            diag_util_printf("\n Port %d Port move Action: %s", port, diagStr_actionStr[action]);
        }
        else if('u' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_newMacOp_get(port, &mode, &action), ret);
            diag_util_printf("\n Port %d unknown SA Action: %s", port, diagStr_actionStr[action]);
        }
        else
            return CPARSER_NOT_OK;
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_port_move_unknown_sa_port_ports_all_action */

/*
 * l2-table set ( port-move | unknown-sa ) port ( <PORT_LIST:ports> | all ) action ( copy-to-cpu | drop | forward | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_l2_table_set_port_move_unknown_sa_port_ports_all_action_copy_to_cpu_drop_forward_trap_to_cpu(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('c' == TOKEN_CHAR(6, 0))
        action = ACTION_COPY2CPU;
    else if('d' == TOKEN_CHAR(6, 0))
        action = ACTION_DROP;
    else if('f' == TOKEN_CHAR(6, 0))
        action = ACTION_FORWARD;
    else if('t' == TOKEN_CHAR(6, 0))
        action = ACTION_TRAP2CPU;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if('p' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_illegalPortMoveAction_set(port, action), ret);
        }
        else if('u' == TOKEN_CHAR(2, 0))
        {
            DIAG_UTIL_ERR_CHK(rtk_l2_newMacOp_set(port, HARDWARE_LEARNING, action), ret);
        }
        else
            return CPARSER_NOT_OK;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_port_move_unknown_sa_port_ports_all_action_copy_to_cpu_drop_forward_trap_to_cpu */

/*
 * l2-table set flush mac-ucast
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast(
    cparser_context_t *context)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_flush_ctrl_t apollo_cfg;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_flush_ctrl_t apollomp_cfg;
#endif

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_get(&apollo_cfg), ret);

            apollo_cfg.flushType = RAW_FLUSH_TYPE_DYNAMIC;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_set(&apollo_cfg), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_get(&apollomp_cfg), ret);

            apollomp_cfg.flushType = APOLLOMP_RAW_FLUSH_TYPE_DYNAMIC;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_set(&apollomp_cfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast */

/*
 * l2-table set flush mac-ucast include-static
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_include_static(
    cparser_context_t *context)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_flush_ctrl_t apollo_cfg;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_flush_ctrl_t apollomp_cfg;
#endif

    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_get(&apollo_cfg), ret);

            apollo_cfg.flushType = RAW_FLUSH_TYPE_BOTH;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_flushCtrl_set(&apollo_cfg), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_get(&apollomp_cfg), ret);

            apollomp_cfg.flushType = APOLLOMP_RAW_FLUSH_TYPE_BOTH;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_flushCtrl_set(&apollomp_cfg), ret);

            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_include_static */

/*
 * l2-table set flush mac-ucast static-only
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_static_only(
    cparser_context_t *context)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;

    DIAG_UTIL_PARAM_CHK();

    osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));

    cfg.flushAddrOnAllPorts = 1;
    cfg.flushDynamicAddr = 0;
    cfg.flushStaticAddr = 1;
    DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_static_only */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByFid = 1;
        cfg.fid = *fid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 0;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid> include-static
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid_include_static(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByFid = 1;
        cfg.fid = *fid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid_include_static */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) filter-id <UINT:fid> static-only
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid_static_only(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *fid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByFid = 1;
        cfg.fid = *fid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 0;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_filter_id_fid_static_only */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid>
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByVid = 1;
        cfg.vid = *vid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 0;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid> include-static
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid_include_static(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByVid = 1;
        cfg.vid = *vid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid_include_static */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) vid <UINT:vid> static-only
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid_static_only(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vid_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByVid = 1;
        cfg.vid = *vid_ptr;
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 0;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_vid_vid_static_only */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 0;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) include-static
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_include_static(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 1;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_include_static */

/*
 * l2-table set flush mac-ucast port ( <PORT_LIST:ports> | all ) static-only
 */
cparser_result_t
cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_static_only(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_l2_flushCfg_t cfg;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        osal_memset(&cfg, 0x00, sizeof(rtk_l2_flushCfg_t));
        cfg.flushByPort = 1;
        cfg.port = port;
        cfg.flushDynamicAddr = 0;
        cfg.flushStaticAddr = 1;
        DIAG_UTIL_ERR_CHK(rtk_l2_ucastAddr_flush(&cfg), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_flush_mac_ucast_port_ports_all_static_only */

/*
 * l2-table set ip-mcast-data port ( <PORT_LIST:ports> | all ) action ( forward | drop )
 */
cparser_result_t
cparser_cmd_l2_table_set_ip_mcast_data_port_ports_all_action_forward_drop(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('f' == TOKEN_CHAR(6, 0))
        action = ACTION_FORWARD;
    else if('d' == TOKEN_CHAR(6, 0))
        action = ACTION_DROP;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_l2_ipmcAction_set(port, action), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_l2_ipmcAction_set(port, action), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_ip_mcast_data_port_ports_all_action_forward_drop */

/*
 * l2-table get ip-mcast-data port ( <PORT_LIST:ports> | all ) action
 */
cparser_result_t
cparser_cmd_l2_table_get_ip_mcast_data_port_ports_all_action(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_action_t action;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_l2_ipmcAction_get(port, &action), ret);
                diag_util_printf("\n Port %d IPMC action: %s", port, diagStr_actionStr[action]);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_l2_ipmcAction_get(port, &action), ret);
                diag_util_printf("\n Port %d IPMC action: %s", port, diagStr_actionStr[action]);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_ip_mcast_data_port_ports_all_action */

/*
 * l2-table get entry address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_entry_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_lut_table_t diag_lut;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_lut_table_t apollomp_diag_lut;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            osal_memset(&diag_lut, 0x0, sizeof(apollo_lut_table_t));

            diag_lut.method = RAW_LUT_READ_METHOD_ADDRESS;
            diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_lookUpTb_get(&diag_lut), ret);

            _diag_lutDisplay(&diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            osal_memset(&apollomp_diag_lut, 0x0, sizeof(apollomp_lut_table_t));

            apollomp_diag_lut.method = APOLLOMP_RAW_LUT_READ_METHOD_ADDRESS;
            apollomp_diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_lookUpTb_get(&apollomp_diag_lut), ret);

            _diag_apollomp_lutDisplay(&apollomp_diag_lut);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_entry_address_address */

/*
 * l2-table get next-entry address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_lut_table_t diag_lut;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_lut_table_t apollomp_diag_lut;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            osal_memset(&diag_lut, 0x0, sizeof(apollo_lut_table_t));

            diag_lut.method = RAW_LUT_READ_METHOD_NEXT_ADDRESS;
            diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_lookUpTb_get(&diag_lut), ret);

            _diag_lutDisplay(&diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            osal_memset(&apollomp_diag_lut, 0x0, sizeof(apollomp_lut_table_t));

            apollomp_diag_lut.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_ADDRESS;
            apollomp_diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_lookUpTb_get(&apollomp_diag_lut), ret);

            _diag_apollomp_lutDisplay(&apollomp_diag_lut);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_address_address */

/*
 * l2-table get next-entry mac-ucast address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_mac_ucast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
    int32 address;
    rtk_l2_ucastAddr_t ucastAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&ucastAddr, 0x00, sizeof(rtk_l2_ucastAddr_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_nextValidAddr_get(&address, &ucastAddr), ret);
    _display_l2Ucast_entry(&ucastAddr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_mac_ucast_address_address */

/*
 * l2-table get next-entry mac-ucast address <UINT:address> spn <UINT:port>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_mac_ucast_address_address_spn_port(
    cparser_context_t *context,
    uint32_t  *address_ptr,
    uint32_t  *port_ptr)
{
    int32 ret;
    int32 address;
    rtk_l2_ucastAddr_t ucastAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&ucastAddr, 0x00, sizeof(rtk_l2_ucastAddr_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_nextValidAddrOnPort_get(*port_ptr, &address, &ucastAddr), ret);
    _display_l2Ucast_entry(&ucastAddr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_mac_ucast_address_address_spn_port */

/*
 * l2-table get next-entry l2-mcast address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_l2_mcast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
    int32 address;
    rtk_l2_mcastAddr_t mcastAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&mcastAddr, 0x00, sizeof(rtk_l2_mcastAddr_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_nextValidMcastAddr_get(&address, &mcastAddr), ret);
    _display_l2Mcast_entry(&mcastAddr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_l2_mcast_address_address */

/*
 * l2-table get next-entry ip-mcast address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_ip_mcast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
    int32 address;
    rtk_l2_ipMcastAddr_t ipMcastAddr;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    address = *address_ptr;
    osal_memset(&ipMcastAddr, 0x00, sizeof(rtk_l2_ipMcastAddr_t));

    DIAG_UTIL_ERR_CHK(rtk_l2_nextValidIpMcastAddr_get(&address, &ipMcastAddr), ret);
    _display_ipMcast_entry(&ipMcastAddr);
    diag_util_printf("\n Address = %d", address);
    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_ip_mcast_address_address */

/*
 * l2-table get next-entry l2-ip-mcast address <UINT:address>
 */
cparser_result_t
cparser_cmd_l2_table_get_next_entry_l2_ip_mcast_address_address(
    cparser_context_t *context,
    uint32_t  *address_ptr)
{
    int32 ret;
#if defined(CONFIG_SDK_APOLLO)
    apollo_lut_table_t diag_lut;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_lut_table_t apollomp_diag_lut;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            osal_memset(&diag_lut, 0x0, sizeof(apollo_lut_table_t));

            diag_lut.method = RAW_LUT_READ_METHOD_NEXT_L2L3MC;
            diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollo_raw_l2_lookUpTb_get(&diag_lut), ret);

            _diag_lutDisplay(&diag_lut);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            osal_memset(&apollomp_diag_lut, 0x0, sizeof(apollomp_lut_table_t));

            apollomp_diag_lut.method = APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2L3MC;
            apollomp_diag_lut.address = *address_ptr;

            DIAG_UTIL_ERR_CHK(apollomp_raw_l2_lookUpTb_get(&apollomp_diag_lut), ret);

            _diag_apollomp_lutDisplay(&apollomp_diag_lut);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_next_entry_l2_ip_mcast_address_address */

/*
 * l2-table get src-port-egress-filter port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_l2_table_get_src_port_egress_filter_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_portmask_t portmask;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_get(&portmask), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(RTK_PORTMASK_IS_PORT_SET(portmask, port))
        {
            diag_util_printf("\n SRC Port %d egress filter state: %s", port, diagStr_enable[ENABLED]);
        }
        else
        {
            diag_util_printf("\n SRC Port %d egress filter state: %s", port, diagStr_enable[DISABLED]);
        }
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_get_src_port_egress_filter_port_ports_all_state */

/*
 * l2-table set src-port-egress-filter port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_l2_table_set_src_port_egress_filter_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_portmask_t portmask;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e' == TOKEN_CHAR(6, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(6, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_get(&portmask), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(ENABLED == state)
        {
            RTK_PORTMASK_PORT_SET(portmask, port);
        }
        else
        {
            RTK_PORTMASK_PORT_CLEAR(portmask, port);
        }
    }

    DIAG_UTIL_ERR_CHK(rtk_l2_srcPortEgrFilterMask_set(&portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_l2_table_set_src_port_egress_filter_port_ports_all_state_disable_enable */

