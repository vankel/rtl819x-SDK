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

#include <hal/common/halctrl.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>


#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_epon.h>
#endif

/*
 * epon init
 */
cparser_result_t
cparser_cmd_epon_init(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();

    diag_util_printf("feature not implement!!");

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_init */

/*
 * epon get bypass-fec state
 */
cparser_result_t
cparser_cmd_epon_get_bypass_fec_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_bypassFecEnable_get(&enable), ret); 
            diag_util_printf("state:%s",diagStr_enable[enable]);
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_bypass_fec_state */

/*
 * epon set bypass-fec state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_bypass_fec_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_bypassFecEnable_set(enable), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_bypass_fec_state_disable_enable */

/*
 * epon get llid-table <UINT:index>
 */
cparser_result_t
cparser_cmd_epon_get_llid_table_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_llid_table_t   llidEntry;
            
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(*index_ptr,&llidEntry), ret); 
            diag_util_mprintf("idx:%d LLID:%6d valid:%d report_timer:%4d report_timeout:%d\n",
                                *index_ptr,llidEntry.llid,llidEntry.valid,llidEntry.report_timer,llidEntry.is_report_timeout);
            
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_llid_table_index */

/*
 * epon get llid-table
 */
cparser_result_t
cparser_cmd_epon_get_llid_table(
    cparser_context_t *context)
{
    uint32 index;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_llid_table_t   llidEntry;

            for(index = 0; index<8; index++)
            {
                DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(index,&llidEntry), ret); 
                diag_util_mprintf("idx:%d LLID:%6d valid:%d report_timer:%4d report_timeout:%d\n",
                                    index,llidEntry.llid,llidEntry.valid,llidEntry.report_timer,llidEntry.is_report_timeout);
            }                
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_llid_table */

/*
 * epon set llid-table <UINT:index> state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_state_disable_enable(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
        enable = DISABLED;
        
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_llid_table_t   llidEntry;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(*index_ptr,&llidEntry), ret); 
            
            llidEntry.valid = enable;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_set(*index_ptr,&llidEntry), ret); 
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_state_disable_enable */

/*
 * epon set llid-table <UINT:index> llid <UINT:llid>
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_llid_llid(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *llid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_llid_table_t   llidEntry;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(*index_ptr,&llidEntry), ret); 
            
            llidEntry.llid = *llid_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_set(*index_ptr,&llidEntry), ret); 
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_llid_llid */

/*
 * epon set llid-table <UINT:index> report-timer <UINT:timer>
 */
cparser_result_t
cparser_cmd_epon_set_llid_table_index_report_timer_timer(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *timer_ptr)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_llid_table_t   llidEntry;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_get(*index_ptr,&llidEntry), ret); 
            
            llidEntry.report_timer = *timer_ptr;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_llidTable_set(*index_ptr,&llidEntry), ret); 
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_llid_table_index_report_timer_timer */

/*
 * epon get mpcp-gate action
 */
cparser_result_t
cparser_cmd_epon_get_mpcp_gate_action(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpGateHandle_get(&act), ret); 
            
            switch(act)
            {
                case APOLLOMP_EPON_GATE_ASIC_HANDLE:
                    diag_util_mprintf("ASIC Handle\n");
                    break;    
                case APOLLOMP_EPON_GATE_ASIC_HANDLE_AND_TRAP_TO_CPU:
                    diag_util_mprintf("ASIC Handle and trap\n");

                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",act);
                    break;                
            }
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_mpcp_gate_action */

/*
 * epon set mpcp-gate action ( asic-only | trap-and-asic ) 
 */
cparser_result_t
cparser_cmd_epon_set_mpcp_gate_action_asic_only_trap_and_asic(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;

            if('a'==TOKEN_CHAR(4,0))
                act = APOLLOMP_EPON_GATE_ASIC_HANDLE;
            else
                act = APOLLOMP_EPON_GATE_ASIC_HANDLE_AND_TRAP_TO_CPU;

            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpGateHandle_set(act), ret); 
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_mpcp_gate_action_asic_only_trap_and_asic */

/*
 * epon get mpcp-invalid-len action
 */
cparser_result_t
cparser_cmd_epon_get_mpcp_invalid_len_action(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_mpcpHandle_t act;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpInvalidLenHandle_get(&act), ret); 
            
            switch(act)
            {
                case APOLLOMP_EPON_MPCP_DROP:
                    diag_util_mprintf("Drop\n");
                    break;    
                case APOLLOMP_EPON_MPCP_PASS:
                    diag_util_mprintf("Pass\n");
                    break;    
                default:
                    diag_util_mprintf("unknown action:%d\n",act);
                    break;                
            }
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_mpcp_invalid_len_action */

/*
 * epon set mpcp-invalid-len action ( drop | pass ) 
 */
cparser_result_t
cparser_cmd_epon_set_mpcp_invalid_len_action_drop_pass(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_gatehandle_t act;

            if('d'==TOKEN_CHAR(4,0))
                act = APOLLOMP_EPON_MPCP_DROP;
            else
                act = APOLLOMP_EPON_MPCP_PASS;

            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_mpcpInvalidLenHandle_set(act), ret); 
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_mpcp_invalid_len_action_drop_pass */

/*
 * epon get register mode
 */
cparser_result_t
cparser_cmd_epon_get_register_mode(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_regMode_t mode;
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMode_get(&mode), ret); 
            
            switch(mode)
            {
                case APOLLOMP_EPON_SW_REG:
                    diag_util_mprintf("SW register\n");
                    break;    
                case APOLLOMP_EPON_HW_REG:
                    diag_util_mprintf("HW register\n");
                    break;    
                default:
                    diag_util_mprintf("unknown mode:%d\n",mode);
                    break;                
            }
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_mode */

/*
 * epon set register mode ( asic | sw ) 
 */
cparser_result_t
cparser_cmd_epon_set_register_mode_asic_sw(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            apollomp_raw_epon_regMode_t mode;

            if('a'==TOKEN_CHAR(4,0))
                mode = APOLLOMP_EPON_HW_REG;
            else
                mode = APOLLOMP_EPON_SW_REG;
                
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMode_set(mode), ret); 
            
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_mode_asic_sw */

/*
 * epon get register llid-idx
 */
cparser_result_t
cparser_cmd_epon_get_register_llid_idx(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 llidIdx;
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regLlidIdx_get(&llidIdx), ret); 
            diag_util_mprintf("register llid table index:%d\n",llidIdx);
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_llid_idx */

/*
 * epon set register llid-idx <UINT:index> 
 */
cparser_result_t
cparser_cmd_epon_set_register_llid_idx_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 llidIdx;
    DIAG_UTIL_PARAM_CHK();
      
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
        {
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regLlidIdx_set(*index_ptr), ret); 
            break;
        }
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_llid_idx_index */

/*
 * epon get register state
 */
cparser_result_t
cparser_cmd_epon_get_register_state(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regReguest_get(&enable), ret); 
            diag_util_printf("state:%s",diagStr_enable[enable]);
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_state */

/*
 * epon set register state ( disable | enable )
 */
cparser_result_t
cparser_cmd_epon_set_register_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regReguest_set(enable), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_state_disable_enable */

/*
 * epon get register mac-address
 */
cparser_result_t
cparser_cmd_epon_get_register_mac_address(
    cparser_context_t *context)
{
    rtk_mac_t   regMac;
    int32 ret = RT_ERR_FAILED;
  
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMac_get(&regMac), ret); 
            diag_util_mprintf("dmac data: %s\n",diag_util_inet_mactoa(&regMac.octet[0]));
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_mac_address */

/*
 * epon set register mac-address <MACADDR:mac> 
 */
cparser_result_t
cparser_cmd_epon_set_register_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    rtk_mac_t   regMac;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    osal_memcpy(&regMac.octet, mac_ptr->octet, ETHER_ADDR_LEN);    

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regMac_set(&regMac), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_mac_address_mac */

/*
 * epon get register pendding-grant
 */
cparser_result_t
cparser_cmd_epon_get_register_pendding_grant(
    cparser_context_t *context)
{
    uint32   grantNum;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regPendingGrantNum_get(&grantNum), ret); 
            diag_util_mprintf("grant number:%d\n",grantNum);        
           
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_get_register_pendding_grant */

/*
 * epon set register pendding-grant <UINT:number> 
 */
cparser_result_t
cparser_cmd_epon_set_register_pendding_grant_number(
    cparser_context_t *context,
    uint32_t  *number_ptr)
{
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO
        case APOLLO_CHIP_ID:
            diag_util_printf("feature not support!\n");
            break;
#endif            
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollomp_raw_epon_regPendingGrantNum_set(*number_ptr), ret); 
            break;
#endif            
        default:
            diag_util_printf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;    
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_set_register_pendding_grant_index */

