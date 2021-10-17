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

#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>

/*
 * meter init
 */
cparser_result_t
cparser_cmd_meter_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    
    /*init rate module*/
    DIAG_UTIL_ERR_CHK(rtk_rate_init(), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_init */


/*
 * meter get entry <MASK_LIST:index>
 */
cparser_result_t
cparser_cmd_meter_get_entry_index(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 burstSize;
    uint32 rate;
    rtk_enable_t ifgInclude;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterBucket_get(index, &burstSize), ret);
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);

        diag_util_mprintf("Meter idx = %-2u, meter rate = %-5u, Kbps include IFG = %-8s, burst size = %u\n", 
                            index, rate*8,
                            ifgInclude?DIAG_STR_ENABLE:DIAG_STR_DISABLE,
                            burstSize);        
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index */

/*
 * meter get entry <MASK_LIST:index> burst-size
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_burst_size(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 burstSize;
    
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterBucket_get(index, &burstSize), ret);
        diag_util_mprintf("Meter idx = %u, burst size = %u\n", index, burstSize);
    }

 
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_burst_size */

/*
 * meter set entry <MASK_LIST:index> burst-size <UINT:size>
 */
cparser_result_t
cparser_cmd_meter_set_entry_index_burst_size_size(
    cparser_context_t *context,
    char * *index_ptr,
    uint32_t  *size_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterBucket_set(index, *size_ptr), ret);
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_entry_index_burst_size_size */

/*
 * meter get entry <MASK_LIST:index> ifg
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_ifg(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 rate;
    rtk_enable_t ifgInclude;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);
        diag_util_mprintf("Meter idx = %u, include IFG = %s\n", index, diagStr_ifgState[ifgInclude]);
    }  
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_ifg */

/*
 * meter set entry <MASK_LIST:index> ifg ( exclude | include )
 */
cparser_result_t
cparser_cmd_meter_set_entry_index_ifg_exclude_include(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 rate;
    rtk_enable_t ifgInclude;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);
        
        if ('e' == TOKEN_CHAR(5,0))
            ifgInclude = DISABLED;
        else
            ifgInclude = ENABLED;
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_set(index, rate,ifgInclude), ret);
    }    
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_entry_index_ifg_exclude_include */

/*
 * meter get entry <MASK_LIST:index> meter-exceed
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_meter_exceed(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_enable_t isExceed;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterExceed_get(index, &isExceed), ret);
        diag_util_mprintf("Meter idx = %u, meter exceed = %s\n", index, isExceed?DIAG_STR_YES:DIAG_STR_NO);
    }  

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_meter_exceed */

/*
 * meter reset entry <MASK_LIST:index> meter-exceed
 */
cparser_result_t
cparser_cmd_meter_reset_entry_index_meter_exceed(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    rtk_enable_t isExceed;
    
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeterExceed_get(index, &isExceed), ret);
    } 
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_reset_entry_index_meter_exceed */

/*
 * meter get tick-token
 */
cparser_result_t
cparser_cmd_meter_get_tick_token(
    cparser_context_t *context)
{
    uint32 tickPeriod,tkn;
    uint32 ret; 
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(METER_TB_CTRLr, TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(METER_TB_CTRLr, TKNf, &tkn)), ret);
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TKNf, &tkn)), ret);
            break;
#endif    
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
        
    }
    
    diag_util_mprintf("tick period:%u, token:%u \n",tickPeriod,tkn);   
    
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_tick_token */

/*
 * meter set tick-token tick-period <UINT:period> token <UINT:token>
 */
cparser_result_t
cparser_cmd_meter_set_tick_token_tick_period_period_token_token(
    cparser_context_t *context,
    uint32_t  *period_ptr,
    uint32_t  *token_ptr)
{
    uint32 ret;
    uint32 period;
    uint32 token;
    
    period = *period_ptr;
    token  = *token_ptr;

    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(METER_TB_CTRLr, TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(METER_TB_CTRLr, TKNf, &token)), ret);
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_METER_TB_CTRLr, APOLLOMP_TKNf, &token)), ret);
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
        
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_tick_token_tick_period_period_token_token */

/*
 * meter get pon-tick-token
 */
cparser_result_t
cparser_cmd_meter_get_pon_tick_token(
    cparser_context_t *context)
{
    uint32 tickPeriod,tkn;
    uint32 ret; 
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(PON_TB_CTRLr, TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(PON_TB_CTRLr, TKNf, &tkn)), ret);
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TICK_PERIODf, &tickPeriod)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TKNf, &tkn)), ret);
            break;
        default:
#endif    
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    diag_util_mprintf("tick period:%u, token:%u \n",tickPeriod,tkn);   
 
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_pon_tick_token */

/*
 * meter set pon-tick-token tick-period <UINT:period> token <UINT:token>
 */
cparser_result_t
cparser_cmd_meter_set_pon_tick_token_tick_period_period_token_token(
    cparser_context_t *context,
    uint32_t  *period_ptr,
    uint32_t  *token_ptr)
{
    uint32 ret;
    uint32 period;
    uint32 token;
    
    period = *period_ptr;
    token  = *token_ptr;
    
    DIAG_UTIL_PARAM_CHK();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(PON_TB_CTRLr, TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(PON_TB_CTRLr, TKNf, &token)), ret);
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:

            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TICK_PERIODf, &period)), ret);
            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_PON_TB_CTRLr, APOLLOMP_TKNf, &token)), ret);
            break;
#endif    
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_pon_tick_token_tick_period_period_token_token */

/*
 * meter get entry <MASK_LIST:index> rate
 */
cparser_result_t
cparser_cmd_meter_get_entry_index_rate(
    cparser_context_t *context,
    char * *index_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 rate;
    rtk_enable_t ifgInclude;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);
        diag_util_mprintf("Meter idx = %u, meter rate = %u Kbps\n", index, rate);
    }    
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_get_entry_index_rate */

/*
 * meter set entry <MASK_LIST:index> rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_meter_set_entry_index_rate_rate(
    cparser_context_t *context,
    char * *index_ptr,
    uint32_t  *rate_ptr)
{
    diag_mask_t mask;
    int32 ret = RT_ERR_FAILED;
    uint32 index;
    uint32 rate;
    rtk_enable_t ifgInclude;
/*
    if(*rate_ptr > 1048568 || *rate_ptr%8 != 0 )
    {
        diag_util_printf("The rate range would be in 8~1048568, and must be exactiy divisible by 8!\n");
        return CPARSER_ERR_INVALID_PARAMS;
    }
    rateTemp = (*rate_ptr) >> 3;
*/        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK32(mask, 3), ret);
    
    DIAG_UTIL_MASK_SCAN(mask, index)
    {
        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_get(index, &rate,&ifgInclude), ret);

        DIAG_UTIL_ERR_CHK(rtk_rate_shareMeter_set(index, *rate_ptr,ifgInclude), ret);
    }    
    return CPARSER_OK;
}    /* end of cparser_cmd_meter_set_entry_index_rate_rate */

