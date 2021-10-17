/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 8997 $
 * $Date: 2010-04-12 15:05:35 +0800 (Mon, 12 Apr 2010) $
 *
 * Purpose : Definition those sdk test command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) sdk test
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
#include <sdk/sdk_test.h>
#include <diag_util.h>
#include <diag_om.h>
#include <parser/cparser_priv.h>
#include <flag/cmd_flag_apollo.h>
/*
 * sdk test group <STRING:item>
 */
cparser_result_t cparser_cmd_sdk_test_group_item(cparser_context_t *context,
    char **item_ptr)
{
    uint32  unit = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_CHIP_ID(unit);

    DIAG_UTIL_ERR_CHK(sdktest_run(unit, *item_ptr), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_sdk_test_group_item */

/*
 * sdk test case_id <UINT:start> { <UINT:end> }
 */
cparser_result_t cparser_cmd_sdk_test_case_id_start_end(cparser_context_t *context,
    uint32_t *start_ptr, uint32_t *end_ptr)
{
    uint32  unit = 0;
    uint32  start = 0, end = 0;
    int32   ret = RT_ERR_FAILED;

    DIAG_UTIL_PARAM_CHK();
    DIAG_OM_GET_CHIP_ID(unit);

    if (5 == TOKEN_NUM())
    {
        start = *start_ptr;
        end = *end_ptr;
    }
    else if (4 == TOKEN_NUM())
    {
        start = *start_ptr;
        end = start;
    }
    else
    {
        diag_util_printf("User config: Error!\n");
        return CPARSER_NOT_OK;
    }

    DIAG_UTIL_ERR_CHK(sdktest_run_id(unit, start, end), ret);
    return CPARSER_OK;
} /* end of cparser_cmd_sdk_test_case_id_start_end */



cparser_result_t cparser_cmd_sample_code_sdk_test_case_test_unit_general(cparser_context_t *context,
    uint32_t *test_unit_ptr, uint32_t *option_ptr)
{

	diag_util_printf("test_unit:%d\n",*test_unit_ptr);

#ifdef APOLLO_FPGA_TEST_CMD
	diag_util_printf("option:%d\n",*option_ptr);
#endif
    return CPARSER_OK;
}    
    


/*
 * sample_cmd  <UINT:test_para_1> 
 */
cparser_result_t cparser_cmd_sample_cmd_test_para_1(cparser_context_t *context,
    uint32_t *test_para_1_ptr)
{
	uint32_t option;
	
	cparser_cmd_sample_code_sdk_test_case_test_unit_general(context,test_para_1_ptr,&option);
    return CPARSER_OK;
} /* end of cparser_cmd_sample_cmd_test_para_1 */


/*
 * sample_cmd  <UINT:test_para_1> { <UINT:test_para_2> }
 */
cparser_result_t cparser_cmd_sample_cmd_test_para_1_test_para_2(cparser_context_t *context,
    uint32_t *test_para_1_ptr,
    uint32_t *test_para_2_ptr)
{
	cparser_cmd_sample_code_sdk_test_case_test_unit_general(context,test_para_1_ptr,test_para_2_ptr);
    return CPARSER_OK;
} /* end of cparser_cmd_sample_cmd_test_para_1_test_para_2 */
