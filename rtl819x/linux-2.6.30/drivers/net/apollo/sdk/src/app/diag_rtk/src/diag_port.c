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
#include <diag_util.h>
#include <parser/cparser_priv.h>
#include <diag_str.h>
#include <rtk/port.h>
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>
#if defined(CONFIG_SDK_APOLLO)
#include <dal/apollo/raw/apollo_raw_port.h>
#endif
#if defined(CONFIG_SDK_APOLLOMP)
#include <dal/apollomp/raw/apollomp_raw_port.h>
#endif

/*
 * port init
 */
cparser_result_t
cparser_cmd_port_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_port_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_port_init */

/*
 * port dump isolation
 */
cparser_result_t
cparser_cmd_port_dump_isolation(
    cparser_context_t *context)
{
    int32 ret;
    uint32 vidx;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    portlist.max = HAL_GET_MAX_PORT();
    portlist.min = HAL_GET_MIN_PORT();
    HAL_GET_ALL_PORTMASK(portlist.portmask);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_isolation_get(port, &vidx), ret);
                diag_util_printf("\n Port %d: VIDX %d", port, vidx);

                DIAG_UTIL_ERR_CHK(apollo_raw_port_isolationL34_get(port, &vidx), ret);
                diag_util_printf("\n Port %d: L34 VIDX %d", port, vidx);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    portlist.max = HAL_GET_MAX_EXT_PORT();
    portlist.min = HAL_GET_MIN_EXT_PORT();
    HAL_GET_ALL_EXT_PORTMASK(portlist.portmask);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_extport_isolation_get(port, &vidx), ret);
                diag_util_printf("\n EXT port %d: VIDX %d", port, vidx);

                DIAG_UTIL_ERR_CHK(apollo_raw_extport_isolationL34_get(port, &vidx), ret);
                diag_util_printf("\n EXT port %d: L34 VIDX %d", port, vidx);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
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
}    /* end of cparser_cmd_port_dump_isolation */

/*
 * port get auto-nego port ( <PORT_LIST:ports> | all ) ability
 */
cparser_result_t
cparser_cmd_port_get_auto_nego_port_ports_all_ability(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_port_phy_ability_t ability;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port 1000F 100F 100H 10F 10H FlowControl AsyFC\n");
    diag_util_mprintf("---- ----- ---- ---- --- --- ----------- -----\n");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(HAL_IS_PHY_EXIST(port))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoAbility_get(port, &ability), ret);
            diag_util_mprintf("%-4d %-5s %-4s %-4s %-3s %-3s %-11s %-5s\n",
                                port,
                                diagStr_enDisplay[ability.Full_1000],
                                diagStr_enDisplay[ability.Full_100],
                                diagStr_enDisplay[ability.Half_100],
                                diagStr_enDisplay[ability.Full_10],
                                diagStr_enDisplay[ability.Half_10],
                                diagStr_enDisplay[ability.FC],
                                diagStr_enDisplay[ability.AsyFC]);
        }
        else
        {
            diag_util_mprintf("%-4d  << No PHY embedded >> \n", port);
        }

    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_auto_nego_port_ports_all_ability */

/*
 * port get auto-nego port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_port_get_auto_nego_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(HAL_IS_PHY_EXIST(port))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoEnable_get(port, &state), ret);
            diag_util_printf("\n Port %d Auto-Nego state: %s", port, diagStr_enable[state]);
        }
        else
            diag_util_printf("\n Port %d << No PHY embedded >>", port);
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_auto_nego_port_ports_all_state */

/*
 * port get status port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_status_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_port_linkStatus_t linkStatus;
    rtk_port_speed_t  speed;
    rtk_port_duplex_t duplex;
    uint32 txfc;
    uint32 rxfc;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port Status Speed Duplex TX_FC RX_FC\n");
    diag_util_mprintf("---- ------ ----- ------ ----- -----");

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_link_get(port, &linkStatus), ret);
        DIAG_UTIL_ERR_CHK(rtk_port_speedDuplex_get(port, &speed, &duplex), ret);
        DIAG_UTIL_ERR_CHK(rtk_port_flowctrl_get(port, &txfc, &rxfc), ret);

        diag_util_mprintf("\n%-4d %-6s %-5s %-6s %-5s %-5s",
                          port,
                          diagStr_portLinkStatus[linkStatus],
                          diagStr_portSpeed[speed],
                          diagStr_portDuplex[duplex],
                          diagStr_enDisplay[txfc],
                          diagStr_enDisplay[rxfc]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_status_port_ports_all */

/*
 * port get enhanced-fid port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_enhanced_fid_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_efid_t efid;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_enhancedFid_get(port, &efid), ret);
        diag_util_printf("\n Port %d EFID: %d", port, efid);
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_enhanced_fid_port_ports_all */

/*
 * port set enhanced-fid port ( <PORT_LIST:ports> | all ) enhanced-fid <UINT:efid>
 */
cparser_result_t
cparser_cmd_port_set_enhanced_fid_port_ports_all_enhanced_fid_efid(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *efid_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_enhancedFid_set(port, (rtk_efid_t)*efid_ptr), ret);
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_enhanced_fid_port_ports_all_enhanced_fid_efid */

/*
 * port get force-dmp port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_force_dmp_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    rtk_portmask_t portmask;
    diag_portlist_t portlist;
    uint8 portStr[20];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_forceDmpMask_get(port, &portmask), ret);
                DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &portmask), ret);
                diag_util_printf("\n port %d force DMP: %s", port, portStr);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_port_forceDmpMask_get(port, &portmask), ret);
                DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &portmask), ret);
                diag_util_printf("\n port %d force DMP: %s", port, portStr);
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
}    /* end of cparser_cmd_port_get_force_dmp_port_ports_all */

/*
 * port get force-dmp
 */
cparser_result_t
cparser_cmd_port_get_force_dmp(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_port_forceDmp_get(&enable), ret);
            diag_util_printf("\n Force DMP state: %s", diagStr_enable[enable]);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_port_forceDmp_get(&enable), ret);
            diag_util_printf("\n Force DMP state: %s", diagStr_enable[enable]);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_force_dmp */

/*
 * port set force-dmp port ( <PORT_LIST:ports> | all ) port-mask ( <PORT_LIST:port_mask> | all )
 */
cparser_result_t
cparser_cmd_port_set_force_dmp_port_ports_all_port_mask_port_mask_all(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *port_mask_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    diag_portlist_t dmp_portlist;
    uint8 portStr[20];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(dmp_portlist, 6), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_forceDmpMask_set(port, dmp_portlist.portmask), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_port_forceDmpMask_set(port, dmp_portlist.portmask), ret);
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
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
}    /* end of cparser_cmd_port_set_force_dmp_port_ports_all_port_mask_port_mask_all */

/*
 * port set force-dmp state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_force_dmp_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('e' == TOKEN_CHAR(4, 0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#if defined(CONFIG_SDK_APOLLO)
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_port_forceDmp_set(enable), ret);
            break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_port_forceDmp_set(enable), ret);
            break;
#endif
        default:
            diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;
            break;
    }


    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_force_dmp_state_disable_enable */

/*
 * port get isolation ext <PORT_LIST:ext>
 */
cparser_result_t
cparser_cmd_port_get_isolation_ext_ext(
    cparser_context_t *context,
    char * *ext_ptr)
{
    int32 ret;
    uint32 vidx;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_extport_isolation_get(port, &vidx), ret);
                diag_util_printf("\n EXT port %d: VIDX %d", port, vidx);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
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
}    /* end of cparser_cmd_port_get_isolation_ext_ext */

/*
 * port get isolation ext-l34 <PORT_LIST:ext>
 */
cparser_result_t
cparser_cmd_port_get_isolation_ext_l34_ext(
    cparser_context_t *context,
    char * *ext_ptr)
{
    int32 ret;
    uint32 vidx;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_extport_isolationL34_get(port, &vidx), ret);
                diag_util_printf("\n EXT port(L34) %d: VIDX %d", port, vidx);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
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
}    /* end of cparser_cmd_port_get_isolation_ext_l34_ext */

/*
 * port get isolation port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_isolation_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32 vidx;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_isolation_get(port, &vidx), ret);
                diag_util_printf("\n Port %d: VIDX %d", port, vidx);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
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
}    /* end of cparser_cmd_port_get_isolation_port_ports_all */

/*
 * port get isolation port-l34 ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_isolation_port_l34_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    uint32 vidx;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_isolationL34_get(port, &vidx), ret);
                diag_util_printf("\n Port(L34) %d: VIDX %d", port, vidx);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
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
}    /* end of cparser_cmd_port_get_isolation_port_l34_ports_all */

/*
 * port set isolation ext <PORT_LIST:ext> vlan-index <UINT:vidx>
 */
cparser_result_t
cparser_cmd_port_set_isolation_ext_ext_vlan_index_vidx(
    cparser_context_t *context,
    char * *ext_ptr,
    uint32_t  *vidx_ptr)
{
    int32 ret;
    uint32 vidx;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
    #if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_extport_isolation_set(port, *vidx_ptr), ret);
                break;
    #endif
    #if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                break;
    #endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_ext_ext_vlan_index_vidx */

/*
 * port set isolation ext-l34 <PORT_LIST:ext> vlan-index <UINT:vidx>
 */
cparser_result_t
cparser_cmd_port_set_isolation_ext_l34_ext_vlan_index_vidx(
    cparser_context_t *context,
    char * *ext_ptr,
    uint32_t  *vidx_ptr)
{
    int32 ret;
    uint32 vidx;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_extport_isolationL34_set(port, *vidx_ptr), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_ext_l34_ext_vlan_index_vidx */

/*
 * port set isolation port ( <PORT_LIST:ports> | all ) vlan-index <UINT:vidx>
 */
cparser_result_t
cparser_cmd_port_set_isolation_port_ports_all_vlan_index_vidx(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vidx_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_isolation_set(port, *vidx_ptr), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_port_ports_all_vlan_index_vidx */

/*
 * port set isolation port-l34 ( <PORT_LIST:ports> | all ) vlan-index <UINT:vidx>
 */
cparser_result_t
cparser_cmd_port_set_isolation_port_l34_ports_all_vlan_index_vidx(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *vidx_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_isolationL34_set(port, *vidx_ptr), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_port_l34_ports_all_vlan_index_vidx */

/*
 * port set isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 ) egress-port ( <PORT_LIST:egressports> | none )
 */
cparser_result_t
cparser_cmd_port_set_isolation_port_ports_all_mode0_mode1_egress_port_egressports_none(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *egressports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    diag_portlist_t egrportlist;
    rtk_port_isoConfig_t mode;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    uint32 includeCpu;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(egrportlist, 7), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntry_get(mode, port, &portmask, &includeCpu, &extPortmask), ret);

        portmask.bits[0] = egrportlist.portmask.bits[0];

        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntry_set(mode, port, &portmask, includeCpu, &extPortmask), ret);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_port_ports_all_mode0_mode1_egress_port_egressports_none */

/*
 * port set isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 ) egress-ext ( <PORT_LIST:egress_ext> | none )
 */
cparser_result_t
cparser_cmd_port_set_isolation_port_ports_all_mode0_mode1_egress_ext_egress_ext_none(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *egress_ext_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    diag_portlist_t extportlist;
    rtk_port_isoConfig_t mode;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    uint32 includeCpu;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(extportlist, 7), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntry_get(mode, port, &portmask, &includeCpu, &extPortmask), ret);

        extPortmask.bits[0] = extportlist.portmask.bits[0];

        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntry_set(mode, port, &portmask, includeCpu, &extPortmask), ret);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_port_ports_all_mode0_mode1_egress_ext_egress_ext_none */

/*
 * port set isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 ) cpu ( exclude | include )
 */
cparser_result_t
cparser_cmd_port_set_isolation_port_ports_all_mode0_mode1_cpu_exclude_include(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_port_isoConfig_t mode;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    uint32 includeCpu;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntry_get(mode, port, &portmask, &includeCpu, &extPortmask), ret);

        if('e' == TOKEN_CHAR(7, 0))
            includeCpu = 0;
        else if('i' == TOKEN_CHAR(7, 0))
            includeCpu = 1;
        else
            return CPARSER_NOT_OK;

        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntry_set(mode, port, &portmask, includeCpu, &extPortmask), ret);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_port_ports_all_mode0_mode1_cpu_exclude_include */

/*
 * port get isolation port ( <PORT_LIST:ports> | all ) ( mode0 | mode1 )
 */
cparser_result_t
cparser_cmd_port_get_isolation_port_ports_all_mode0_mode1(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_port_isoConfig_t mode;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    uint32 includeCpu;
    uint8 portStr[20];
    uint8 ExtPortStr[20];

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntry_get(mode, port, &portmask, &includeCpu, &extPortmask), ret);

        DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &portmask), ret);
        DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(ExtPortStr, &extPortmask), ret);

        diag_util_printf("\n Port %d:", port);
        diag_util_printf("\n     egress port mask    :%s", portStr);
        diag_util_printf("\n     egress EXT port mask:%s", ExtPortStr);
        diag_util_printf("\n     egress CPU Port     :%s", diagStr_enable[includeCpu]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_isolation_port_ports_all_mode0_mode1 */

/*
 * port set isolation ext <PORT_LIST:ext> ( mode0 | mode1 ) egress-port ( <PORT_LIST:egressports> | none )
 */
cparser_result_t
cparser_cmd_port_set_isolation_ext_ext_mode0_mode1_egress_port_egressports_none(
    cparser_context_t *context,
    char * *ext_ptr,
    char * *egressports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    diag_portlist_t egrportlist;
    rtk_port_isoConfig_t mode;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    uint32 includeCpu;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(egrportlist, 7), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntryExt_get(mode, port, &portmask, &includeCpu, &extPortmask), ret);

        portmask.bits[0] = egrportlist.portmask.bits[0];

        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntryExt_set(mode, port, &portmask, includeCpu, &extPortmask), ret);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_ext_ext_mode0_mode1_egress_port_egressports_none */

/*
 * port set isolation ext <PORT_LIST:ext> ( mode0 | mode1 ) egress-ext ( <PORT_LIST:egress_ext> | none )
 */
cparser_result_t
cparser_cmd_port_set_isolation_ext_ext_mode0_mode1_egress_ext_egress_ext_none(
    cparser_context_t *context,
    char * *ext_ptr,
    char * *egress_ext_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    diag_portlist_t extportlist;
    rtk_port_isoConfig_t mode;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    uint32 includeCpu;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(extportlist, 7), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntryExt_get(mode, port, &portmask, &includeCpu, &extPortmask), ret);

        extPortmask.bits[0] = extportlist.portmask.bits[0];

        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntryExt_set(mode, port, &portmask, includeCpu, &extPortmask), ret);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_ext_ext_mode0_mode1_egress_ext_egress_ext_none */

/*
 * port set isolation ext <PORT_LIST:ext> ( mode0 | mode1 ) cpu ( exclude | include )
 */
cparser_result_t
cparser_cmd_port_set_isolation_ext_ext_mode0_mode1_cpu_exclude_include(
    cparser_context_t *context,
    char * *ext_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_port_isoConfig_t mode;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    uint32 includeCpu;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntryExt_get(mode, port, &portmask, &includeCpu, &extPortmask), ret);

        if('e' == TOKEN_CHAR(7, 0))
            includeCpu = 0;
        else if('i' == TOKEN_CHAR(7, 0))
            includeCpu = 1;
        else
            return CPARSER_NOT_OK;

        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntryExt_set(mode, port, &portmask, includeCpu, &extPortmask), ret);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_ext_ext_mode0_mode1_cpu_exclude_include */

/*
 * port get isolation ext <PORT_LIST:ext> ( mode0 | mode1 )
 */
cparser_result_t
cparser_cmd_port_get_isolation_ext_ext_mode0_mode1(
    cparser_context_t *context,
    char * *ext_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_port_isoConfig_t mode;
    rtk_portmask_t portmask;
    rtk_portmask_t extPortmask;
    uint32 includeCpu;
    uint8 portStr[20];
    uint8 ExtPortStr[20];

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('1' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_1;
    else if('0' == TOKEN_CHAR(5, 4))
        mode = RTK_PORT_ISO_CFG_0;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationEntryExt_get(mode, port, &portmask, &includeCpu, &extPortmask), ret);

        DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(portStr, &portmask), ret);
        DIAG_UTIL_ERR_CHK(diag_util_lPortMask2str(ExtPortStr, &extPortmask), ret);

        diag_util_printf("\n Ext Port %d:", port);
        diag_util_printf("\n     egress port mask    :%s", portStr);
        diag_util_printf("\n     egress EXT port mask:%s", ExtPortStr);
        diag_util_printf("\n     egress CPU Port     :%s", diagStr_enable[includeCpu]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_isolation_ext_ext_mode0_mode1 */

/*
 * port set isolation ctag ( mode0 | mode1 )
 */
cparser_result_t
cparser_cmd_port_set_isolation_ctag_mode0_mode1(
    cparser_context_t *context)
{
    int32 ret;
    rtk_port_isoConfig_t mode;

    DIAG_UTIL_PARAM_CHK();

    if('0' == TOKEN_CHAR(4, 4))
       mode = RTK_PORT_ISO_CFG_0;
    else if('1' == TOKEN_CHAR(4, 4))
       mode = RTK_PORT_ISO_CFG_1;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_port_isolationCtagPktConfig_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_ctag_mode0_mode1 */

/*
 * port get isolation ctag
 */
cparser_result_t
cparser_cmd_port_get_isolation_ctag(
    cparser_context_t *context)
{
    int32 ret;
    rtk_port_isoConfig_t mode;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_port_isolationCtagPktConfig_get(&mode), ret);
    diag_util_printf("\n Ctag mode: Mode%d", mode);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_isolation_ctag */

/*
 * port set isolation l34 ( mode0 | mode1 )
 */
cparser_result_t
cparser_cmd_port_set_isolation_l34_mode0_mode1(
    cparser_context_t *context)
{
    int32 ret;
    rtk_port_isoConfig_t mode;

    DIAG_UTIL_PARAM_CHK();

    if('0' == TOKEN_CHAR(4, 4))
       mode = RTK_PORT_ISO_CFG_0;
    else if('1' == TOKEN_CHAR(4, 4))
       mode = RTK_PORT_ISO_CFG_1;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_ERR_CHK(rtk_port_isolationL34PktConfig_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_l34_mode0_mode1 */

/*
 * port get isolation l34
 */
cparser_result_t
cparser_cmd_port_get_isolation_l34(
    cparser_context_t *context)
{
    int32 ret;
    rtk_port_isoConfig_t mode;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_port_isolationL34PktConfig_get(&mode), ret);
    diag_util_printf("\n L34 mode: Mode%d", mode);

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_isolation_l34 */

/*
 * port get master-slave port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_master_slave_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_port_masterSlave_t masterslave;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyMasterSlave_get(port, &masterslave), ret);
        diag_util_printf("\n Port %d: %s", port, diagStr_masterSlave[masterslave]);
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_master_slave_port_ports_all */

/*
 * port set master-slave port ( <PORT_LIST:ports> | all ) ( auto | master | slave )
 */
cparser_result_t
cparser_cmd_port_set_master_slave_port_ports_all_auto_master_slave(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_port_masterSlave_t masterslave;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('a' == TOKEN_CHAR(5, 0))
        masterslave = PORT_AUTO_MODE;
    else if('m' == TOKEN_CHAR(5, 0))
        masterslave = PORT_MASTER_MODE;
    else if('s' == TOKEN_CHAR(5, 0))
        masterslave = PORT_SLAVE_MODE;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_phyMasterSlave_set(port, masterslave), ret);
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_master_slave_port_ports_all_auto_master_slave */

/*
 * port get phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register>
 */
cparser_result_t
cparser_cmd_port_get_phy_reg_port_ports_all_page_page_register_register(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *page_ptr,
    uint32_t  *register_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
#if defined (FPGA_DEFINED)
        uint16 inputData;
        uint16 outputData;

        /*change page*/
        inputData = 0xC000 + (port<<5) + (31);

        io_mii_phy_reg_write(8,1,*page_ptr);
        io_mii_phy_reg_write(8,0,inputData);

        inputData = 0x8000 + (port<<5) + (*register_ptr);
        io_mii_phy_reg_write(8,0,inputData);
        io_mii_phy_reg_read(8,2,&outputData);
        diag_util_printf("\n Port %d Page %d Register %d: 0x%04X", port, *page_ptr, *register_ptr, outputData);

#else
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_get(port, *page_ptr, *register_ptr, &data), ret);
        diag_util_printf("\n Port %d Page %d Register %d: 0x%04X", port, *page_ptr, *register_ptr, data);
#endif
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_phy_reg_port_ports_all_page_page_register_register */

/*
 * port set phy-reg port ( <PORT_LIST:ports> | all ) page <UINT:page> register <UINT:register> data <UINT:data>
 */
cparser_result_t
cparser_cmd_port_set_phy_reg_port_ports_all_page_page_register_register_data_data(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *page_ptr,
    uint32_t  *register_ptr,
    uint32_t  *data_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {

#if defined (FPGA_DEFINED)
        uint16 inputData;
        /*change page*/
        inputData = 0xC000 + (port<<5) + (31);
        io_mii_phy_reg_write(8,1,*page_ptr);
        io_mii_phy_reg_write(8,0,inputData);

        inputData = 0xC000 + (port<<5) + (*register_ptr);
        io_mii_phy_reg_write(8,1,*data_ptr);
        io_mii_phy_reg_write(8,0,inputData);
#else
        DIAG_UTIL_ERR_CHK(rtk_port_phyReg_set(port, *page_ptr, *register_ptr, *data_ptr), ret);
#endif
    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_phy_reg_port_ports_all_page_page_register_register_data_data */

/*
 * port set auto-nego port ( <PORT_LIST:ports> | all ) ability { 10h } { 10f } { 100h } { 100f } { 1000f } { flow-control } { asy-flow-control }
 */
cparser_result_t
cparser_cmd_port_set_auto_nego_port_ports_all_ability_10h_10f_100h_100f_1000f_flow_control_asy_flow_control(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 token;
    rtk_port_phy_ability_t ability;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    osal_memset(&ability, 0x00, sizeof(rtk_port_phy_ability_t));

    for(token = 6; token < TOKEN_NUM(); token++)
    {
        if(strcmp("10h", TOKEN_STR(token)) == 0)
            ability.Half_10 = ENABLED;
        else if(strcmp("10f", TOKEN_STR(token)) == 0)
            ability.Full_10 = ENABLED;
        else if(strcmp("100h", TOKEN_STR(token)) == 0)
            ability.Half_100 = ENABLED;
        else if(strcmp("100f", TOKEN_STR(token)) == 0)
            ability.Full_100 = ENABLED;
        else if(strcmp("1000f", TOKEN_STR(token)) == 0)
            ability.Full_1000 = ENABLED;
        else if(strcmp("flow-control", TOKEN_STR(token)) == 0)
            ability.FC = ENABLED;
        else if(strcmp("asy-flow-control", TOKEN_STR(token)) == 0)
            ability.AsyFC = ENABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(HAL_IS_PHY_EXIST(port))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoAbility_set(port, &ability), ret);
        }
        else
            diag_util_printf("\n There is no embedded PHY at Port %d", port);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_auto_nego_port_ports_all_ability_10h_10f_100h_100f_1000f_flow_control_asy_flow_control */

/*
 * port set auto-nego port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_auto_nego_port_ports_all_state_disable_enable(
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
    else
        state = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        if(HAL_IS_PHY_EXIST(port))
        {
            DIAG_UTIL_ERR_CHK(rtk_port_phyAutoNegoEnable_set(port, state), ret);
        }
        else
            diag_util_printf("\n There is no embedded PHY at Port %d", port);
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_auto_nego_port_ports_all_state_disable_enable */

/*
 * port get mac-force port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_mac_force_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    uint32 data;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_port_ability_t apollo_raw_portability;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_port_ability_t apollomp_raw_portability;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_printf("\n                                                Link   NWAY    LPI");
    diag_util_printf("\nPort Speed Duplex Fiber1G Link TX_FC RX_FC NWAY MASTER FAULT   100M Giga Force");
    diag_util_printf("\n---- ----- ------ ------- ---- ----- ----- ---- ------ ------- ---- ---- -----");



    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                osal_memset(&apollo_raw_portability, 0x00, sizeof(apollo_raw_port_ability_t));
                DIAG_UTIL_ERR_CHK(apollo_raw_port_ForceAbility_get(port, &apollo_raw_portability), ret);
                DIAG_UTIL_ERR_CHK(reg_field_read(ABLTY_FORCE_MODEr, ABLTY_FORCE_MODEf, &data), ret);

                diag_util_printf("\n%-4d %-5s %-6s %-7s %-4s %-5s %-5s %-4s %-6s %-7s %-4s %-4s %-5s",
                                 port,
                                 diagStr_portSpeed[apollo_raw_portability.speed],
                                 diagStr_portDuplex[apollo_raw_portability.duplex],
                                 diagStr_enDisplay[apollo_raw_portability.linkFib1g],
                                 diagStr_portLinkStatus[apollo_raw_portability.linkStatus],
                                 diagStr_enDisplay[apollo_raw_portability.txFc],
                                 diagStr_enDisplay[apollo_raw_portability.rxFc],
                                 diagStr_enDisplay[apollo_raw_portability.nwayAbility],
                                 diagStr_enDisplay[apollo_raw_portability.masterMod],
                                 diagStr_portNwayFault[apollo_raw_portability.nwayFault],
                                 diagStr_enDisplay[apollo_raw_portability.lpi_100m],
                                 diagStr_enDisplay[apollo_raw_portability.lpi_giga],
                                 diagStr_enDisplay[data & (0x01 << port) ? 1 : 0]);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                osal_memset(&apollomp_raw_portability, 0x00, sizeof(apollomp_raw_port_ability_t));
                DIAG_UTIL_ERR_CHK(apollomp_raw_port_ForceAbility_get(port, &apollomp_raw_portability), ret);
                DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data), ret);

                diag_util_printf("\n%-4d %-5s %-6s %-7s %-4s %-5s %-5s %-4s %-6s %-5s %-4s %-4s %-5s",
                                 port,
                                 diagStr_portSpeed[apollomp_raw_portability.speed],
                                 diagStr_portDuplex[apollomp_raw_portability.duplex],
                                 diagStr_enDisplay[apollomp_raw_portability.linkFib1g],
                                 diagStr_portLinkStatus[apollomp_raw_portability.linkStatus],
                                 diagStr_enDisplay[apollomp_raw_portability.txFc],
                                 diagStr_enDisplay[apollomp_raw_portability.rxFc],
                                 diagStr_enDisplay[apollomp_raw_portability.nwayAbility],
                                 diagStr_enDisplay[apollomp_raw_portability.masterMod],
                                 diagStr_portNwayFault[apollomp_raw_portability.nwayFault],
                                 diagStr_enDisplay[apollomp_raw_portability.lpi_100m],
                                 diagStr_enDisplay[apollomp_raw_portability.lpi_giga],
                                 diagStr_enDisplay[data & (0x01 << port) ? 1 : 0]);
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
}    /* end of cparser_cmd_port_get_mac_force_port_ports_all */

/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) ability ( 10h | 10f | 100h | 100f | 1000f ) flow-control ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_mac_force_port_ports_all_ability_10h_10f_100h_100f_1000f_flow_control_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_port_ability_t portAbilityR;
    apollo_raw_port_ability_t portAbilityW;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_port_ability_t apollomp_portAbilityR;
    apollomp_raw_port_ability_t apollomp_portAbilityW;
#endif

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

#if defined(CONFIG_SDK_APOLLO)
    if(!osal_strcmp(TOKEN_STR(6),"10h"))
    {
        portAbilityW.duplex = PORT_HALF_DUPLEX;
        portAbilityW.speed = PORT_SPEED_10M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"10f"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_10M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"100h"))
    {
        portAbilityW.duplex = PORT_HALF_DUPLEX;
        portAbilityW.speed = PORT_SPEED_100M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"100f"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_100M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"1000f"))
    {
        portAbilityW.duplex = PORT_FULL_DUPLEX;
        portAbilityW.speed = PORT_SPEED_1000M;
    }

    if ('e' == TOKEN_CHAR(8,0))
    {
        portAbilityW.txFc = ENABLED;
        portAbilityW.rxFc = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(8,0))
    {
        portAbilityW.txFc = DISABLED;
        portAbilityW.rxFc = DISABLED;
    }
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    if(!osal_strcmp(TOKEN_STR(6),"10h"))
    {
        apollomp_portAbilityW.duplex = PORT_HALF_DUPLEX;
        apollomp_portAbilityW.speed = PORT_SPEED_10M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"10f"))
    {
        apollomp_portAbilityW.duplex = PORT_FULL_DUPLEX;
        apollomp_portAbilityW.speed = PORT_SPEED_10M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"100h"))
    {
        apollomp_portAbilityW.duplex = PORT_HALF_DUPLEX;
        apollomp_portAbilityW.speed = PORT_SPEED_100M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"100f"))
    {
        apollomp_portAbilityW.duplex = PORT_FULL_DUPLEX;
        apollomp_portAbilityW.speed = PORT_SPEED_100M;
    }
    else if(!osal_strcmp(TOKEN_STR(6),"1000f"))
    {
        apollomp_portAbilityW.duplex = PORT_FULL_DUPLEX;
        apollomp_portAbilityW.speed = PORT_SPEED_1000M;
    }

    if ('e' == TOKEN_CHAR(8,0))
    {
        apollomp_portAbilityW.txFc = ENABLED;
        apollomp_portAbilityW.rxFc = ENABLED;
    }
    else if ('d' == TOKEN_CHAR(8,0))
    {
        apollomp_portAbilityW.txFc = DISABLED;
        apollomp_portAbilityW.rxFc = DISABLED;
    }
#endif

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_ForceAbility_get(port, &portAbilityR), ret);

                portAbilityR.txFc = portAbilityW.txFc;
                portAbilityR.rxFc = portAbilityW.rxFc;
                portAbilityR.speed = portAbilityW.speed;
                portAbilityR.duplex = portAbilityW.duplex;

                DIAG_UTIL_ERR_CHK(apollo_raw_port_ForceAbility_set(port, &portAbilityR), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_port_ForceAbility_get(port, &apollomp_portAbilityR), ret);

                apollomp_portAbilityR.txFc = apollomp_portAbilityW.txFc;
                apollomp_portAbilityR.rxFc = apollomp_portAbilityW.rxFc;
                apollomp_portAbilityR.speed = apollomp_portAbilityW.speed;
                apollomp_portAbilityR.duplex = apollomp_portAbilityW.duplex;

                DIAG_UTIL_ERR_CHK(apollomp_raw_port_ForceAbility_set(port, &apollomp_portAbilityR), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_mac_force_port_ports_all_ability_10h_10f_100h_100f_1000f_flow_control_disable_enable */

/*
 * port get rtct ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_rtct_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    rtk_port_t port;
    diag_portlist_t portlist;
    rtk_rtctResult_t result;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_rtctResult_get(port, &result), ret);

        if(result.linkType == PORT_SPEED_1000M)
        {
            diag_util_printf("\n Port %d", port);
            diag_util_printf("\n GE result:");

            diag_util_printf("\n channelAShort = %d", result.ge_result.channelAShort);
            diag_util_printf("\n channelBShort = %d", result.ge_result.channelBShort);
            diag_util_printf("\n channelCShort = %d", result.ge_result.channelCShort);
            diag_util_printf("\n channelDShort = %d", result.ge_result.channelDShort);

            diag_util_printf("\n channelAOpen = %d", result.ge_result.channelAOpen);
            diag_util_printf("\n channelBOpen = %d", result.ge_result.channelBOpen);
            diag_util_printf("\n channelCOpen = %d", result.ge_result.channelCOpen);
            diag_util_printf("\n channelDOpen = %d", result.ge_result.channelDOpen);

            diag_util_printf("\n channelAMismatch = %d", result.ge_result.channelAMismatch);
            diag_util_printf("\n channelBMismatch = %d", result.ge_result.channelBMismatch);
            diag_util_printf("\n channelCMismatch = %d", result.ge_result.channelCMismatch);
            diag_util_printf("\n channelDMismatch = %d", result.ge_result.channelDMismatch);

            diag_util_printf("\n channelALinedriver = %d", result.ge_result.channelALinedriver);
            diag_util_printf("\n channelBLinedriver = %d", result.ge_result.channelBLinedriver);
            diag_util_printf("\n channelCLinedriver = %d", result.ge_result.channelCLinedriver);
            diag_util_printf("\n channelDLinedriver = %d", result.ge_result.channelDLinedriver);

            diag_util_printf("\n channelALen = %d", result.ge_result.channelALen);
            diag_util_printf("\n channelBLen = %d", result.ge_result.channelBLen);
            diag_util_printf("\n channelCLen = %d", result.ge_result.channelCLen);
            diag_util_printf("\n channelDLen = %d", result.ge_result.channelDLen);
        }
        else
        {
            diag_util_printf("\n Port %d", port);
            diag_util_printf("\n FE result:");
            diag_util_printf("\n isRxShort = %d", result.fe_result.isRxShort);
            diag_util_printf("\n isTxShort = %d", result.fe_result.isTxShort);
            diag_util_printf("\n isRxOpen = %d", result.fe_result.isRxOpen);
            diag_util_printf("\n isTxOpen = %d", result.fe_result.isTxOpen);
            diag_util_printf("\n isRxMismatch = %d", result.fe_result.isRxMismatch);
            diag_util_printf("\n isTxMismatch = %d", result.fe_result.isTxMismatch);
            diag_util_printf("\n isRxLinedriver = %d", result.fe_result.isRxLinedriver);
            diag_util_printf("\n isTxLinedriver = %d", result.fe_result.isTxLinedriver);
            diag_util_printf("\n rxLen = %d", result.fe_result.rxLen);
            diag_util_printf("\n txLen = %d", result.fe_result.txLen);
        }

    }

    diag_util_printf("\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_rtct_ports_all */

/*
 * port set rtct ( <PORT_LIST:ports> | all ) start
 */
cparser_result_t
cparser_cmd_port_set_rtct_ports_all_start(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret;
    diag_portlist_t portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_ERR_CHK(rtk_port_rtct_start(&(portlist.portmask)), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_rtct_ports_all_start */

/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) ( lpi-100M | lpi-giga ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_mac_force_port_ports_all_lpi_100M_lpi_giga_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_port_ability_t portAbility;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_port_ability_t apollomp_portAbility;
#endif
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d' == TOKEN_CHAR(6,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        enable = ENABLED;
    }

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_ForceAbility_get(port, &portAbility), ret);

                if ('1' == TOKEN_CHAR(5,4))
                {
                    portAbility.lpi_100m = enable;
                }
                else if ('g' == TOKEN_CHAR(5,4))
                {
                    portAbility.lpi_giga = enable;
                }

                DIAG_UTIL_ERR_CHK(apollo_raw_port_ForceAbility_set(port, &portAbility), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_port_ForceAbility_get(port, &apollomp_portAbility), ret);

                if ('1' == TOKEN_CHAR(5,4))
                {
                    apollomp_portAbility.lpi_100m = enable;
                }
                else if ('g' == TOKEN_CHAR(5,4))
                {
                    apollomp_portAbility.lpi_giga = enable;
                }

                DIAG_UTIL_ERR_CHK(apollomp_raw_port_ForceAbility_set(port, &apollomp_portAbility), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_mac_force_port_ports_all_lpi_100m_lpi_giga_state_disable_enable */

/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) link-state ( link-down | link-up )
 */
cparser_result_t
cparser_cmd_port_set_mac_force_port_ports_all_link_state_link_down_link_up(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
#if defined(CONFIG_SDK_APOLLO)
    apollo_raw_port_ability_t portAbilityR;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
    apollomp_raw_port_ability_t apollomp_portAbilityR;
#endif
	rtk_port_linkStatus_t 	linkStatus;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d' == TOKEN_CHAR(6,5))
    {
        linkStatus = PORT_LINKDOWN;
    }
    else if ('u' == TOKEN_CHAR(6,5))
    {
        linkStatus = PORT_LINKUP;
    }
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_port_ForceAbility_get(port, &portAbilityR), ret);

                portAbilityR.linkStatus = linkStatus;

                DIAG_UTIL_ERR_CHK(apollo_raw_port_ForceAbility_set(port, &portAbilityR), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollomp_raw_port_ForceAbility_get(port, &apollomp_portAbilityR), ret);

                apollomp_portAbilityR.linkStatus = linkStatus;

                DIAG_UTIL_ERR_CHK(apollomp_raw_port_ForceAbility_set(port, &apollomp_portAbilityR), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_mac_force_port_ports_all_link_state_link_down_link_up */

/*
 * port set mac-force port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_mac_force_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t state;
    uint32 data;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if ('d' == TOKEN_CHAR(6,0))
    {
        state = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(6,0))
    {
        state = ENABLED;
    }
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_field_read(ABLTY_FORCE_MODEr, ABLTY_FORCE_MODEf, &data), ret);

                if(ENABLED == state)
                    data |= (0x01 << port);
                else
                    data &= ~(0x01 << port);

                DIAG_UTIL_ERR_CHK(reg_field_write(ABLTY_FORCE_MODEr, ABLTY_FORCE_MODEf, &data), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data), ret);

                if(ENABLED == state)
                    data |= (0x01 << port);
                else
                    data &= ~(0x01 << port);

                DIAG_UTIL_ERR_CHK(reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_mac_force_port_ports_all_state_disable_enable */

/*
 * port set isolation leaky ip-mcast port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_port_set_isolation_leaky_ip_mcast_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    if ('d' == TOKEN_CHAR(8,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(8,0))
    {
        enable = ENABLED;
    }


    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationIpmcLeaky_set(port, enable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_isolation_leaky_ip_mcast_port_ports_all_state_enable_disable */

/*
 * port get isolation leaky ip-mcast port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_port_get_isolation_leaky_ip_mcast_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 6), ret);

    diag_util_mprintf("Port State\n");
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_isolationIpmcLeaky_get(port, &enable), ret);

        diag_util_mprintf("%-5d%s\n", port, diagStr_enable[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_isolation_leaky_ip_mcast_port_ports_all_state */

/*
 * port get special-congest ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_port_get_special_congest_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    uint32              sustTimer;
    uint32              cgstTimer;

    DIAG_UTIL_PARAM_CHK();
    RT_PARAM_CHK((NULL == *ports_ptr), CPARSER_ERR_INVALID_PARAMS);
    DIAG_UTIL_OUTPUT_INIT();

    /* show specific port isolation info */
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_specialCongest_get(port, &sustTimer), ret);
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#if defined(CONFIG_SDK_APOLLO)
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(SC_P_CTRL_0r, port, REG_ARRAY_INDEX_NONE, CGST_TMRf, &cgstTimer), ret);
                break;
#endif
#if defined(CONFIG_SDK_APOLLOMP)
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_SC_P_CTRL_0r, port, REG_ARRAY_INDEX_NONE, APOLLOMP_CGST_TMRf, &cgstTimer), ret);
                break;
#endif
            default:
                diag_util_printf("%s\n", DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;
                break;
        }

        diag_util_mprintf("Port %2u, sustain timer = %u sec, congest timer = %u sec\n", port, sustTimer, cgstTimer);
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_special_congest_ports_all */

/*
 * port set special-congest ( <PORT_LIST:ports> | all ) sustain-timer <UINT:second>
 */
cparser_result_t
cparser_cmd_port_set_special_congest_ports_all_sustain_timer_second(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *second_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t    portlist;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_specialCongest_set(port, *second_ptr), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_special_congest_ports_all_sustain_timer_second */

/*
 * port get special-congest ( <PORT_LIST:ports> | all ) indicator
 */
cparser_result_t
cparser_cmd_port_get_special_congest_ports_all_indicator(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32               ret = RT_ERR_FAILED;
    rtk_port_t          port = 0;
    diag_portlist_t     portlist;
    uint32              status;;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    /* show specific port isolation info */
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_specialCongestStatus_get(port, &status), ret);
        diag_util_mprintf("Port %2u : ever occur = %u\n", port, status);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_special_congest_ports_all_indicator */

/*
 * port clear special-congest ( <PORT_LIST:ports> | all ) indicator
 */
cparser_result_t
cparser_cmd_port_clear_special_congest_ports_all_indicator(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_specialCongestStatus_clear(port), ret);
    }

    diag_util_printf("\n");
    return CPARSER_OK;
}    /* end of cparser_cmd_port_clear_special_congest_ports_all_indicator */

/*
 * port set port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_port_set_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    if('e' == TOKEN_CHAR(5, 0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(5, 0))
        state = DISABLED;
    else
        return CPARSER_NOT_OK;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_adminEnable_set(port, state), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_port_set_port_ports_all_state_disable_enable */

/*
 * port get port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_port_get_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32           ret = RT_ERR_FAILED;
    rtk_port_t      port = 0;
    diag_portlist_t  portlist;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {
        DIAG_UTIL_ERR_CHK(rtk_port_adminEnable_get(port, &state), ret);
        diag_util_mprintf("\n Port %d Admin state: %s", port, diagStr_enable[state]);
    }

    diag_util_mprintf("\n");


    return CPARSER_OK;
}    /* end of cparser_cmd_port_get_port_ports_all_state */

