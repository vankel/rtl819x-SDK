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
 * Purpose : Definition those CPU command and APIs in the SDK diagnostic shell.
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
#include <rtk/cpu.h>

/*
 * cpu init
 */
cparser_result_t
cparser_cmd_cpu_init(
    cparser_context_t *context)
{
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(rtk_cpu_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_init */

/*
 * cpu set aware-port ( <PORT_LIST:ports> | all | none )
 */
cparser_result_t
cparser_cmd_cpu_set_aware_port_ports_all_none(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32                       ret;
    diag_portlist_t             portlist;
    rtk_port_t                  port = 0;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 3), ret);

    DIAG_UTIL_ERR_CHK(rtk_cpu_awarePortMask_set(portlist.portmask), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_aware_port_ports_all_none */

/*
 * cpu get aware-port
 */
cparser_result_t
cparser_cmd_cpu_get_aware_port(
    cparser_context_t *context)
{
    int32                       ret;
    rtk_portmask_t lPortMask;
    rtk_port_t port = 0;
    rtk_enable_t enable;
    uint8  port_list[DIAG_UTIL_PORT_MASK_STRING_LEN];

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_cpu_awarePortMask_get(&lPortMask), ret);

    diag_util_lPortMask2str(port_list, &lPortMask);
    diag_util_printf("CPU aware port: %s\n", port_list);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_aware_port */

/*
 * cpu set tag-format ( apollo | normal )
 */
cparser_result_t
cparser_cmd_cpu_set_tag_format_apollo_normal(
    cparser_context_t *context)
{
    rtk_cpu_tag_fmt_t mode;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('a' == TOKEN_CHAR(3,0))
        mode = CPU_TAG_FMT_APOLLO;
    else if('n' == TOKEN_CHAR(3,0))
        mode = CPU_TAG_FMT_NORMAL;

    DIAG_UTIL_ERR_CHK(rtk_cpu_tagFormat_set(mode), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_tag_format_apollo_normal */

/*
 * cpu get tag-format
 */
cparser_result_t
cparser_cmd_cpu_get_tag_format(
    cparser_context_t *context)
{
    int32 ret;
    rtk_cpu_tag_fmt_t mode;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_cpu_tagFormat_get(&mode), ret);

    diag_util_printf("CPU tag format: %s\n\r", diagStr_cpuTagFormat[mode]);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_tag_format */


/*
 * cpu set trap-insert-tag state ( disable | enable )
 */
cparser_result_t
cparser_cmd_cpu_set_trap_insert_tag_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t state;
    int32 ret;

    DIAG_UTIL_PARAM_CHK();

    if('e' == TOKEN_CHAR(4,0))
        state = ENABLED;
    else if('d' == TOKEN_CHAR(4,0))
        state = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_cpu_trapInsertTag_set(state), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_set_trap_insert_tag_enable_disable */

/*
 * cpu get trap-insert-tag state
 */
cparser_result_t
cparser_cmd_cpu_get_trap_insert_tag_state(
    cparser_context_t *context)
{
    int32 ret;
    rtk_enable_t state;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(rtk_cpu_trapInsertTag_get(&state), ret);

    diag_util_printf("CPU trap target insert tag state: %s\n\r", diagStr_enable[state]);

    return CPARSER_OK;
}    /* end of cparser_cmd_cpu_get_trap_insert_tag */

