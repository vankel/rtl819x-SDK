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
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <hal/mac/mem.h>
#include <hal/mac/reg.h>

/*
 * bandwidth init
 */
cparser_result_t
cparser_cmd_bandwidth_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    
    /*init rate module*/
    DIAG_UTIL_ERR_CHK(rtk_rate_init(), ret);
    
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_init */

/*
 * bandwidth get egress ifg
 */
cparser_result_t
cparser_cmd_bandwidth_get_egress_ifg(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(rtk_rate_egrBandwidthCtrlIncludeIfg_get(&enable), ret);
    
    diag_util_mprintf("Egress Rate counting ifg: %s\n",diagStr_ifgState[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_egress_ifg */

/*
 * bandwidth set egress ifg ( exclude | include )
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_ifg_exclude_include(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    if('i'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_ERR_CHK(rtk_rate_egrBandwidthCtrlIncludeIfg_set(enable), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_ifg_exclude_include */

/*
 * bandwidth get egress ifg port ( <PORT_LIST:ports> | all ) 
 */
cparser_result_t
cparser_cmd_bandwidth_get_egress_ifg_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    { 
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBandwidthCtrlIncludeIfg_get(port,&enable), ret);
        diag_util_mprintf("port:%d Egress Rate counting ifg: %s\n",port,diagStr_ifgState[enable]);

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_egress_port_ports_all_ifg */

/*
 * bandwidth set egress ifg port ( <PORT_LIST:ports> | all ) ( exclude | include )
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_ifg_port_ports_all_exclude_include(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    if('i'==TOKEN_CHAR(6,0))
        enable = ENABLED;
    else
        enable = DISABLED;



    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    { 
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBandwidthCtrlIncludeIfg_set(port,enable), ret);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_ifg_exclude_include */

/*
 * bandwidth get egress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_bandwidth_get_egress_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    diag_mask_t mask;
    uint32    queue;
    uint32    index;
    uint32    rate;
    uint32    ponId;
    
    rtk_switch_phyPortId_get(RTK_PORT_PON,&ponId);
    
    printf("\n RTK_PORT_PON :%d\n",ponId);
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {  
        if(ponId == port)
            continue;
            
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBandwidthCtrlRate_get(port,&rate), ret);   
        diag_util_mprintf("port:%2d  rate:%d\n",port,(rate*8));    


        for(queue=0 ; queue<HAL_MAX_NUM_OF_QUEUE() ; queue++)
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,&index), ret);   
            index = index - ((port%4)*8); 
            diag_util_mprintf("         queue:%2d  apr-index:%2d\n",queue,index);    
        }        
    }    

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_egress_port_ports_all */

/*
 * bandwidth get egress port ( <PORT_LIST:ports> | all ) queue-id ( <MASK_LIST:qid> | all )
 */
cparser_result_t
cparser_cmd_bandwidth_get_egress_port_ports_all_queue_id_qid_all(
    cparser_context_t *context,
    char * *ports_ptr,
    char * *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    diag_mask_t mask;
    uint32 queue;
    uint32    index;
    uint32    ponId;
    
    rtk_switch_phyPortId_get(RTK_PORT_PON,&ponId);
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_MASK8(mask, 6), ret);
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {  
        if(ponId == port)
            continue;
        DIAG_UTIL_MASK_SCAN(mask, queue)
        {
            DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlMeterIdx_get(port,queue,&index), ret);   
            index = index - ((port%4)*8); 
            diag_util_mprintf("port:%2d  queue:%2d  apr-index:%2d\n",port,queue,index);    
        }        
    }    

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_egress_port_ports_all_queue_id_qid_all */

/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> apr-index <UINT:index>
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_apr_index_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32    index;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        index = *index_ptr + ((port%4)*8); 
        DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlMeterIdx_set(port,*qid_ptr,index), ret);   
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_apr_index_index */

/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) queue-id <UINT:qid> share-bandwidth state ( disable | enable )
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_share_bandwidth_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *qid_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(9,0))
        enable = ENABLED;
    else
        enable = DISABLED;
        
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_rate_egrQueueBwCtrlEnable_set(port,*qid_ptr,enable), ret);   
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_queue_id_qid_share_bandwidth_state_disable_enable */

/*
 * bandwidth set egress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_bandwidth_set_egress_port_ports_all_rate_rate(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 rate;
    
    DIAG_UTIL_PARAM_CHK();
    rate = *rate_ptr/8;

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_rate_portEgrBandwidthCtrlRate_set(port,rate), ret);   
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_egress_port_ports_all_rate_rate */

/*
 * bandwidth get ingress bypass-packet state
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_bypass_packet_state(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 enable;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 


        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(IGR_BWCTRL_GLB_CTRLr, BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_read(APOLLOMP_IGR_BWCTRL_GLB_CTRLr, APOLLOMP_BYPASS_ENf, &enable)), ret);
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }
    
    diag_util_mprintf("Ingress Rate byapss:%s\n",diagStr_enable[enable]);
    diag_util_mprintf("byapss packet format:\n");
    diag_util_mprintf("    -DMAC=01-80-C2-00-00-xx\n");
    diag_util_mprintf("    -IGMP/MLD control packet\n");
    diag_util_mprintf("    -8899 frames\n");

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_ingress_bypass_packet_state */

/*
 * bandwidth set ingress bypass-packet state ( disable | enable )
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_bypass_packet_state_disable_enable(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 enable;

    if('e'==TOKEN_CHAR(5,0))
        enable = ENABLED;
    else
        enable = DISABLED;

    DIAG_UTIL_PARAM_CHK();
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(IGR_BWCTRL_GLB_CTRLr, BYPASS_ENf, &enable)), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            DIAG_UTIL_ERR_CHK((reg_field_write(APOLLOMP_IGR_BWCTRL_GLB_CTRLr, APOLLOMP_BYPASS_ENf, &enable)), ret);
            break;
#endif            
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_bypass_packet_state_disable_enable */

/*
 * bandwidth get ingress flow-control high-threshold
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_flow_control_high_threshold(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_ingress_flow_control_high_threshold */

/*
 * bandwidth set ingress flow-control high-threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_flow_control_high_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *threshold_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_flow_control_high_threshold_threshold */

/*
 * bandwidth get ingress flow-control low-threshold
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_flow_control_low_threshold(
    cparser_context_t *context)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_ingress_flow_control_low_threshold */

/*
 * bandwidth set ingress flow-control low-threshold <UINT:threshold>
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_flow_control_low_threshold_threshold(
    cparser_context_t *context,
    uint32_t  *threshold_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_flow_control_low_threshold_threshold */

/*
 * bandwidth get ingress flow-control port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_flow_control_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    diag_util_mprintf("");

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_ingress_flow_control_port_ports_all_state */

/*
 * bandwidth set ingress flow-control port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_flow_control_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    DIAG_UTIL_PARAM_CHK();

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_flow_control_port_ports_all_state_disable_enable */

/*
 * bandwidth get ingress ifg port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_ifg_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlIncludeIfg_get(port,&enable), ret);   
        diag_util_mprintf("port:%2d ifg: %s\n",port,diagStr_ifgState[enable]);
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_ingress_ifg_port_ports_all */

/*
 * bandwidth set ingress ifg port ( <PORT_LIST:ports> | all ) ( exclude | include )
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_ifg_port_ports_all_exclude_include(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(6,0))
        enable = DISABLED;
    else
        enable = ENABLED;
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlIncludeIfg_set(port,enable), ret);   
    }
        
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_ifg_port_ports_all_exclude_include */

/*
 * bandwidth get ingress port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_bandwidth_get_ingress_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 rate,enable;
    
    DIAG_UTIL_PARAM_CHK();
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlRate_get(port,&rate), ret);   
        rate = rate*8;

        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_read(IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, MODEf, &enable)), ret);
                break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_read(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MODEf, &enable)), ret);
                break;
#endif                
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
                break;
        }
        diag_util_mprintf("port:%2d mode:%6s  rate:%d\n",port , diagStr_enable[enable], rate); 
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_get_ingress_port_ports_all */

/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 enable;
    
    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(6,0))
        enable = 0;     
    else
        enable = 1;
             
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_write(IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, MODEf, &enable)), ret);
                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK((reg_array_field_write(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MODEf, &enable)), ret);
                break;
#endif    
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_port_ports_all_state_disable_enable */

/*
 * bandwidth set ingress port ( <PORT_LIST:ports> | all ) rate <UINT:rate>
 */
cparser_result_t
cparser_cmd_bandwidth_set_ingress_port_ports_all_rate_rate(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *rate_ptr)
{
    int32 ret = RT_ERR_FAILED;
    diag_portlist_t portlist;
    rtk_port_t port;
    uint32 rate;
    
    DIAG_UTIL_PARAM_CHK();
    
    rate = *rate_ptr/8;
    
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);    
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        DIAG_UTIL_ERR_CHK(rtk_rate_portIgrBandwidthCtrlRate_set(port,rate), ret);   
    }
    
    return CPARSER_OK;
}    /* end of cparser_cmd_bandwidth_set_ingress_port_ports_all_rate_rate */

