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
 * Purpose : Definition those Trap command and APIs in the SDK diagnostic shell.
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
#include <diag_str.h>
#include <parser/cparser_priv.h>

#include <dal/apollo/raw/apollo_raw_trap.h>

/*
 * trap init
 */
cparser_result_t
cparser_cmd_trap_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_trap_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_init */


/*
 * trap set ( cdp | csstp ) action ( drop | forward | forward-exclude-cpu | trap-to-cpu )
 */
cparser_result_t
cparser_cmd_trap_set_cdp_csstp_action_drop_forward_forward_exclude_cpu_trap_to_cpu(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_action_t action;
    rtk_mac_t rmaMac;
    
    DIAG_UTIL_PARAM_CHK();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;


    if ('d' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcc;
    }
    else if ('s' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcd;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;


    if ('d' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_DROP;
    }
    else if ('t' == TOKEN_CHAR(5, 0))
    {
        action = ACTION_TRAP2CPU;
    }
    else if ('f' == TOKEN_CHAR(5, 0))
    {
         if ('-' == TOKEN_CHAR(5, 7))
            action = ACTION_FORWARD_EXCLUDE_CPU;
         else
            action = ACTION_FORWARD;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_set(&rmaMac, action), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_cdp_csstp_action_drop_forward_forward_exclude_cpu_trap_to_cpu */

/*
 * trap get ( cdp | csstp ) action
 */
cparser_result_t
cparser_cmd_trap_get_cdp_csstp_action(
    cparser_context_t *context)
{
    rtk_action_t action;
    uint32 ret = CPARSER_NOT_OK;
    rtk_mac_t rmaMac;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;


    if ('d' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcc;
    }
    else if ('s' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcd;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    DIAG_UTIL_ERR_CHK(rtk_trap_rmaAction_get(&rmaMac, &action), ret);

    if(rmaMac.octet[5] == 0xcc)
    {
        diag_util_mprintf("CDP ");
    }
    else if(rmaMac.octet[5] == 0xcd)
    {
        diag_util_mprintf("CSSTP ");
    }

    switch(action)
    {
        case ACTION_DROP:
            diag_util_mprintf("%s\n",DIAG_STR_DROP);
            break;
        case ACTION_TRAP2CPU:
            diag_util_mprintf("%s\n",DIAG_STR_TRAP2CPU);
            break;
        case ACTION_FORWARD:
            diag_util_mprintf("%s\n",DIAG_STR_FORWARD);
            break;
    }
    

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_cdp_csstp_action */

/*
 * trap set ( cdp | csstp ) ( vlan-leaky | isolation-leaky | keep-vlan-format | bypass-storm-control )  state ( disable | enable )
 */
cparser_result_t
cparser_cmd_trap_set_cdp_csstp_vlan_leaky_isolation_leaky_keep_vlan_format_bypass_storm_control_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_enable_t enable;
    rtk_mac_t rmaMac;
    
    DIAG_UTIL_PARAM_CHK();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;


    if ('d' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcc;
    }
    else if ('s' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcd;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    
    if ('b' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaStormControlEnable_set(&rmaMac, enable), ret);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaPortIsolationEnable_set(&rmaMac, enable), ret);
    }
    else if ('k' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaKeepCtagEnable_set(&rmaMac, enable), ret);
    }
    else if ('v' == TOKEN_CHAR(3,0))
    {
    	enable = (!enable);
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaVlanCheckEnable_set(&rmaMac, enable), ret);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;
    
    

    return CPARSER_OK;
}    /* end of cparser_cmd_trap_set_cdp_csstp_vlan_leaky_isolation_leaky_keep_vlan_format_bypass_storm_control_state_disable_enable */

/*
 * trap get ( cdp | csstp ) ( vlan-leaky | isolation-leaky | keep-vlan-format | bypass-storm-control )
 */
cparser_result_t
cparser_cmd_trap_get_cdp_csstp_vlan_leaky_isolation_leaky_keep_vlan_format_bypass_storm_control(
    cparser_context_t *context)
{
    int32 ret = CPARSER_NOT_OK;
    rtk_enable_t enable;
    rtk_mac_t rmaMac;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;


    if ('d' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcc;
    }
    else if ('s' == TOKEN_CHAR(2, 1))
    {
        rmaMac.octet[5] = 0xcd;
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if ('b' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaStormControlEnable_get(&rmaMac, &enable), ret);
    }
    else if ('i' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaPortIsolationEnable_get(&rmaMac, &enable), ret);
    }
    else if ('k' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaKeepCtagEnable_get(&rmaMac, &enable), ret);
    }
    else if ('v' == TOKEN_CHAR(3,0))
    {
        DIAG_UTIL_ERR_CHK(rtk_trap_rmaVlanCheckEnable_get(&rmaMac, &enable), ret);

		enable = (!enable);
    }
    else
        return CPARSER_ERR_INVALID_PARAMS;

    if(rmaMac.octet[5] == 0xcc)
    {
        diag_util_mprintf("CDP ");
    }
    else if(rmaMac.octet[5] == 0xcd)
    {
        diag_util_mprintf("CSSTP ");
    }

    diag_util_mprintf("%s : %s \n",  TOKEN_STR(3), diagStr_enable[enable]);
        
    return CPARSER_OK;
}    /* end of cparser_cmd_trap_get_cdp_csstp_vlan_leaky_isolation_leaky_keep_vlan_format_bypass_storm_control */

