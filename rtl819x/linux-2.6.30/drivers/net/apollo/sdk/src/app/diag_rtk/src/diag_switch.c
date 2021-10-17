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
 * $Revision: 16745 $
 * $Date: 2011-04-12 11:46:26 +0800 (Tue, 12 Apr 2011) $
 *
 * Purpose : Definition those command and APIs in the SDK diagnostic shell.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) switch commands.    
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
#include <dal/apollo/raw/apollo_raw_switch.h>
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>


/*
 * switch init
 */
cparser_result_t
cparser_cmd_switch_init(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    
    DIAG_UTIL_PARAM_CHK();
    
    /*init switch module*/
    DIAG_UTIL_ERR_CHK(rtk_switch_init(), ret);

    return CPARSER_OK;
}    /* end of cparser_cmd_epon_init */


/*
 * switch get 48-pass-1 state
 */
cparser_result_t
cparser_cmd_switch_get_48_pass_1_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_read(CFG_BACKPRESSUREr, EN_48_PASS_1f, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_read(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_EN_48_PASS_1f, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    diag_util_mprintf("48 Pass 1 function: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_48_pass_1_state */

/*
 * switch set 48-pass-1 state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_48_pass_1_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_write(CFG_BACKPRESSUREr, EN_48_PASS_1f, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_write(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_EN_48_PASS_1f, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_48_pass_1_state_disable_enable */

/*
 * switch set ipg-compensation state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_ipg_compensation_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_write(SWITCH_CTRLr, SHORT_IPGf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_write(APOLLOMP_SWITCH_CTRLr, APOLLOMP_SHORT_IPGf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_ipg_compensation_state_disable_enable */

/*
 * switch set ipg-compensation ( 65ppm | 90ppm )
 */
cparser_result_t
cparser_cmd_switch_set_ipg_compensation_65ppm_90ppm(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    apollo_raw_ipgCompMode_t mode;
    
    DIAG_UTIL_PARAM_CHK();

    if ('6' == TOKEN_CHAR(3,0))
    {
        mode = RAW_IPGCOMPMODE_65PPM;
    }
    else if ('9' == TOKEN_CHAR(3,0))
    {
        mode = RAW_IPGCOMPMODE_90PPM;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_write(CFG_UNHIOLr, IPG_COMPENSATIONf, &mode)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_write(APOLLOMP_CFG_UNHIOLr, APOLLOMP_IPG_COMPENSATIONf, &mode)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_ipg_compensation_65ppm_90ppm */

/*
 * switch get ipg-compensation state
 */
cparser_result_t
cparser_cmd_switch_get_ipg_compensation_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_read(SWITCH_CTRLr, SHORT_IPGf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_read(APOLLOMP_SWITCH_CTRLr, APOLLOMP_SHORT_IPGf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    diag_util_mprintf("Short IPG function: %s\n",diagStr_enable[enable]);

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_ipg_compensation_state */

/*
 * switch get ipg-compensation
 */
cparser_result_t
cparser_cmd_switch_get_ipg_compensation(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    apollo_raw_ipgCompMode_t mode;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_read(CFG_UNHIOLr, IPG_COMPENSATIONf, &mode)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_read(APOLLOMP_CFG_UNHIOLr, APOLLOMP_IPG_COMPENSATIONf, &mode)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    diag_util_mprintf("IPG compensation: %s\n",diagStr_ipgCompensation[mode]);


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_ipg_compensation */


/*
 * switch get rx-check-crc port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_switch_get_rx_check_crc_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port       Status \n"); 	
    diag_util_mprintf("-----------------------------\n"); 	
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
     
                if ((ret = reg_array_field_read(P_MISCr, port, REG_ARRAY_INDEX_NONE, CRC_SKIPf,&enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                
                if ((ret = reg_array_field_read(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_CRC_SKIPf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
                break;
#endif    
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;        
                break;
        }

        diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[((enable==0)?1:0)]);   
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_rx_check_crc_port_ports_all_state */

/*
 * switch set rx-check-crc port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_rx_check_crc_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e'==TOKEN_CHAR(6,0))
        enable = DISABLED;
    else if('d'==TOKEN_CHAR(6,0))
        enable = ENABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
     
                if ((ret = reg_array_field_write(P_MISCr, port, REG_ARRAY_INDEX_NONE, CRC_SKIPf,&enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                
                if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_CRC_SKIPf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
                break;
#endif    
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;        
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_rx_check_crc_port_ports_all_state_disable_enable */

/*
 * switch set bypass-tx-crc state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_bypass_tx_crc_state_disable_enable(
    cparser_context_t *context)
{
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    if('e'==TOKEN_CHAR(4,0))
        enable = ENABLED;
    else if('d'==TOKEN_CHAR(4,0))
        enable = DISABLED;
  
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_write(CFG_BACKPRESSUREr, EN_BYPASS_ERRORf,&enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_write(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_EN_BYPASS_ERRORf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_bypass_tx_crc_state_disable_enable */

/*
 * switch get bypass-tx-crc state
 */
cparser_result_t
cparser_cmd_switch_get_bypass_tx_crc_state(
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
 
            if ((ret = reg_field_read(CFG_BACKPRESSUREr, EN_BYPASS_ERRORf,&enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_read(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_EN_BYPASS_ERRORf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    diag_util_mprintf("Bypass Tx CRC: %s\n", diagStr_enable[enable]); 

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_bypass_tx_crc_state */



/*
 * switch set mac-address <MACADDR:mac>
 */
cparser_result_t
cparser_cmd_switch_set_mac_address_mac(
    cparser_context_t *context,
    cparser_macaddr_t  *mac_ptr)
{
    rtk_mac_t mac;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();

    osal_memcpy(&mac.octet, mac_ptr->octet, ETHER_ADDR_LEN);    
    DIAG_UTIL_ERR_CHK(rtk_switch_mgmtMacAddr_set(&mac), ret); 		

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_mac_addr_addr */

/*
 * switch get mac-address
 */
cparser_result_t
cparser_cmd_switch_get_mac_address(
    cparser_context_t *context)
{
    rtk_mac_t mac;
    int32 ret = RT_ERR_FAILED;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
   
    DIAG_UTIL_ERR_CHK(rtk_switch_mgmtMacAddr_get(&mac), ret); 		

    diag_util_mprintf("Switch MAC Address: %s\n", diag_util_inet_mactoa(&mac.octet[0]));

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_mac_addr_addr */


/*
 * switch set max-pkt-len ( fe | ge ) port ( <PORT_LIST:ports> | all ) index <UINT:index>
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_fe_ge_port_ports_all_index_index(
    cparser_context_t *context,
    char * *ports_ptr,
    uint32_t  *index_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    apollo_raw_linkSpeed_t speed;
    int32 ret = RT_ERR_FAILED;
    uint32 value,regField;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    if('g'==TOKEN_CHAR(3,0))
     {
        speed = LINKSPEED_GIGA;
#ifdef CONFIG_SDK_APOLLOMP
        regField = APOLLOMP_MAX_LENGTH_GIGAf;
#endif
     }
    else if('f'==TOKEN_CHAR(3,0))
    {
        speed = LINKSPEED_100M;
#ifdef CONFIG_SDK_APOLLOMP
        regField = APOLLOMP_MAX_LENGTH_10_100f;
#endif
    }
	
    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
    #ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
            DIAG_UTIL_ERR_CHK(apollo_raw_switch_maxPktLenSpeed_set( port, speed, *index_ptr), ret); 			
                break;
    #endif
    #ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                value = *index_ptr;
                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &value), ret); 			
                break;
    #endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_max_pkt_len_fe_ge_port_ports_all_index_index */

/*
 * switch get max-pkt-len ( fe | ge ) port ( <PORT_LIST:ports> | all )
 */
cparser_result_t
cparser_cmd_switch_get_max_pkt_len_fe_ge_port_ports_all(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    apollo_raw_linkSpeed_t speed;
    uint32 index;
    int32 ret = RT_ERR_FAILED;
    uint32 regField;
            
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 5), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            diag_util_mprintf("Port       Speed       Config \n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                if('g'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_GIGA;
                }    
                else if('f'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_100M;
                }
                DIAG_UTIL_ERR_CHK(apollo_raw_switch_maxPktLenSpeed_get(port, speed, &index), ret); 			
                diag_util_mprintf("%-10u  %s    %d\n", port, diagStr_portSpeed[speed], index);   
            }
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            diag_util_mprintf("Port       Speed       Config \n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                if('g'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_GIGA;
                    regField = APOLLOMP_MAX_LENGTH_GIGAf;
                }
                else if('f'==TOKEN_CHAR(3,0))
                {
                    speed = LINKSPEED_100M;
                    regField = APOLLOMP_MAX_LENGTH_10_100f;
                }
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_ACCEPT_MAX_LEN_CTRLr, port, REG_ARRAY_INDEX_NONE, regField, &index), ret);
                diag_util_mprintf("%-10u  %s    %d\n", port, diagStr_portSpeed[speed], index);   
            }
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_max_pkt_len_fe_ge_port_ports_all */


/*
 * switch set max-pkt-len index <UINT:index> length <UINT:len>
 */
cparser_result_t
cparser_cmd_switch_set_max_pkt_len_index_index_length_len(
    cparser_context_t *context,
    uint32_t  *index_ptr,
    uint32_t  *len_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 length;
    uint32 index;
    
    DIAG_UTIL_PARAM_CHK();
    length = *len_ptr;
    index = *index_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            switch(index)
            {
                case 0:
                    if ((ret = reg_field_write(MAX_LENGTH_CFG0r, ACCEPT_MAX_LENTH_CFG0f, &length)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
                        return ret;
                    }
                    break;
                case 1:
                    if ((ret = reg_field_write(MAX_LENGTH_CFG1r, ACCEPT_MAX_LENTH_CFG1f, &length)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
                        return ret;
                    }
                    break;
                default:

                    return RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
       case APOLLOMP_CHIP_ID:

            switch(index)
            {
                case 0:
                    if ((ret = reg_field_write(APOLLOMP_MAX_LENGTH_CFG0r, APOLLOMP_ACCEPT_MAX_LENTH_CFG0f, &length)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
                        return ret;
                    }
                    break;
                case 1:
                    if ((ret = reg_field_write(APOLLOMP_MAX_LENGTH_CFG1r, APOLLOMP_ACCEPT_MAX_LENTH_CFG1f, &length)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
                        return ret;
                    }
                    break;
                default:

                    return RT_ERR_CHIP_NOT_SUPPORTED;
            }
			break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_max_pkt_len_index_index_length_len */

/*
 * switch get max-pkt-len index <UINT:index>
 */
cparser_result_t
cparser_cmd_switch_get_max_pkt_len_index_index(
    cparser_context_t *context,
    uint32_t  *index_ptr)
{
    int32 ret = RT_ERR_FAILED;
    uint32 length;
    uint32 index;
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();
    index = *index_ptr;

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:

            switch(index)
            {
                case 0:
                    if ((ret = reg_field_read(MAX_LENGTH_CFG0r, ACCEPT_MAX_LENTH_CFG0f, &length)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
                        return ret;
                    }
                    break;
                case 1:
                    if ((ret = reg_field_read(MAX_LENGTH_CFG1r, ACCEPT_MAX_LENTH_CFG1f, &length)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
                        return ret;
                    }
                    break;
                default:

                    return RT_ERR_CHIP_NOT_SUPPORTED;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:

            switch(index)
            {
                case 0:
                    if ((ret = reg_field_read(APOLLOMP_MAX_LENGTH_CFG0r, APOLLOMP_ACCEPT_MAX_LENTH_CFG0f, &length)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
                        return ret;
                    }
                    break;
                case 1:
                    if ((ret = reg_field_read(APOLLOMP_MAX_LENGTH_CFG1r, APOLLOMP_ACCEPT_MAX_LENTH_CFG1f, &length)) != RT_ERR_OK)
                    {
                        RT_ERR(ret, (MOD_SWITCH | MOD_DAL), "");
                        return ret;
                    }
                    break;
                default:

                    return RT_ERR_CHIP_NOT_SUPPORTED;
            }    
			break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }
    
    diag_util_mprintf("Max-Length Index %u is Length %u bytes.\n", index, length);   

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_max_pkt_len_index_index */

/*
 * switch set limit-pause state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_limit_pause_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();

    if ('d' == TOKEN_CHAR(4,0))
    {
        enable = ENABLED;
    }
    else if ('e' == TOKEN_CHAR(4,0))
    {
        enable = DISABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_write(SWITCH_CTRLr, PAUSE_MAX128f,&enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_write(APOLLOMP_SWITCH_CTRLr, APOLLOMP_PAUSE_MAX128f, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }
	
    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_limit_pause_state_disable_enable */

/*
 * switch get limit-pause state
 */
cparser_result_t
cparser_cmd_switch_get_limit_pause_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_read(SWITCH_CTRLr, PAUSE_MAX128f,&enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_read(APOLLOMP_SWITCH_CTRLr, APOLLOMP_PAUSE_MAX128f, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    diag_util_mprintf("Limit Pause Frame: %s\n",diagStr_enable[((enable==0)?1:0)]);

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_limit_pause_state */

/*
 * switch set small-ipg-tag port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_switch_set_small_ipg_tag_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e'==TOKEN_CHAR(6,0))
        enable = ENABLED;
    else if('d'==TOKEN_CHAR(6,0))
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
    #ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
                DIAG_UTIL_ERR_CHK(apollo_raw_switch_smallTagIpg_set(port, enable), ret); 			
                break;
    #endif
    #ifdef CONFIG_SDK_APOLLOMP
            case APOLLOMP_CHIP_ID:
                DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_SMALL_TAG_IPGf, &enable), ret); 			
                break;
    #endif
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
                return CPARSER_NOT_OK;
                break;
        }
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_small_ipg_tag_port_ports_all_state_enable_disable */

/*
 * switch get small-ipg-tag port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_switch_get_small_ipg_tag_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            diag_util_mprintf("Port       Status \n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(apollo_raw_switch_smallTagIpg_get(port, &enable), ret); 			
                diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
            }	
            break;
#endif

#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            diag_util_mprintf("Port       Status \n"); 	
            diag_util_mprintf("-----------------------------\n"); 	
            DIAG_UTIL_PORTMASK_SCAN(portlist, port)
            {    
                DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_SMALL_TAG_IPGf, &enable), ret); 			
                diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
            }	
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }



    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_small_ipg_tag_port_ports_all_state */


/*
 * switch get back-pressure
 */
cparser_result_t
cparser_cmd_switch_get_back_pressure(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 state;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_read(CFG_BACKPRESSUREr, LONGTXEf,&state)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_read(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_LONGTXEf, &state)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }


    diag_util_mprintf("Back-pressure: %s\n",  diagStr_backPressure[state]);   
	
    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_back_pressure */

/*
 * switch set back-pressure ( jam | defer )
 */
cparser_result_t
cparser_cmd_switch_set_back_pressure_jam_defer(
    cparser_context_t *context)
{
    int32 ret = RT_ERR_FAILED;
    uint32 state;
        
    DIAG_UTIL_PARAM_CHK();

    if('j'==TOKEN_CHAR(3,0))
        state = 0;
    else if('d'==TOKEN_CHAR(3,0))
        state = 1;
   
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
 
            if ((ret = reg_field_write(CFG_BACKPRESSUREr, LONGTXEf,&state)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            
            if ((ret = reg_field_write(APOLLOMP_CFG_BACKPRESSUREr, APOLLOMP_LONGTXEf, &state)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                return ret;
            }
            break;
#endif    
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_back_pressure_jam_defer */

/*
 * switch set small-pkt port ( <PORT_LIST:ports> | all ) state ( enable | disable )
 */
cparser_result_t
cparser_cmd_switch_set_small_pkt_port_ports_all_state_enable_disable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    if('e'==TOKEN_CHAR(6,0))
        enable = ENABLED;
    else if('d'==TOKEN_CHAR(6,0))
        enable = DISABLED;

    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
     
                if ((ret = reg_array_field_write(P_MISCr, port, REG_ARRAY_INDEX_NONE, RX_SPCf,&enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                
                if ((ret = reg_array_field_write(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
                break;
#endif    
            default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;        
                break;
        }

    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_small_pkt_port_ports_all_state_enable_disable */

/*
 * switch get small-pkt port ( <PORT_LIST:ports> | all ) state
 */
cparser_result_t
cparser_cmd_switch_get_small_pkt_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    diag_util_mprintf("Port       Status \n"); 	
    diag_util_mprintf("-----------------------------\n"); 	
    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
    {    
        switch(DIAG_UTIL_CHIP_TYPE)
        {
#ifdef CONFIG_SDK_APOLLO 
            case APOLLO_CHIP_ID:
     
                if ((ret = reg_array_field_read(P_MISCr, port, REG_ARRAY_INDEX_NONE, RX_SPCf,&enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
                break;
#endif    
#ifdef CONFIG_SDK_APOLLOMP 
            case APOLLOMP_CHIP_ID:
                
                if ((ret = reg_array_field_read(APOLLOMP_P_MISCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RX_SPCf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
                break;
#endif    
           default:
                diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
                return CPARSER_NOT_OK;        
                break;
        }
        diag_util_mprintf("%-10u  %s\n", port, diagStr_enable[enable]);   
    }	

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_small_pkt_port_ports_all_state */

/*
 * switch reset ( global | chip ) 
 */
cparser_result_t
cparser_cmd_switch_reset_global_chip(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    apollo_raw_chipReset_t reset=RAW_CHIPRESET_END;
    uint32 field,reg,resetVal;
    DIAG_UTIL_PARAM_CHK();

	
    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            if (!osal_strcmp(TOKEN_STR(2),"global"))
            {
                reset = RAW_SW_GLOBAL_RST;
            }  
            else if (!osal_strcmp(TOKEN_STR(2),"chip"))
            {
                reset = RAW_SW_CHIP_RST;
            }
            else
                return CPARSER_NOT_OK;
        
            DIAG_UTIL_ERR_CHK(apollo_raw_switch_chipReset_set(reset), ret);
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
            resetVal = 1;
            if (!osal_strcmp(TOKEN_STR(2),"global"))
            {
                field = APOLLOMP_SW_RSTf;
                reg   = APOLLOMP_CHIP_RSTr;
            }  
            else if (!osal_strcmp(TOKEN_STR(2),"chip"))
            {
                field = APOLLOMP_CMD_CHIP_RST_PSf;
                reg   = APOLLOMP_SOFTWARE_RSTr;
            }
            else
                return CPARSER_NOT_OK;
            
            DIAG_UTIL_ERR_CHK(reg_field_write(reg, field, &resetVal), ret);
            
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_reset_gphy_global_rsgmii_config_queue_nic_voip_cpu_wdog_pon */

/*
 * switch set output-drop port ( <PORT_LIST:ports> | all ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_output_drop_port_ports_all_state_disable_enable(
    cparser_context_t *context,
    char * *ports_ptr)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;
    diag_portlist_t portlist;
    rtk_port_t port;

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

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP
        case APOLLOMP_CHIP_ID:
		    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
		    {    
				DIAG_UTIL_ERR_CHK(reg_array_field_write(APOLLOMP_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable), ret); 			
		   	}
            break;
#endif
        default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);        
            return CPARSER_NOT_OK;
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_output_drop_port_ports_all_state_disable_enable */

/*
 * switch get output-drop port ( <PORT_LIST:ports> | all ) state 
 */
cparser_result_t
cparser_cmd_switch_get_output_drop_port_ports_all_state(
    cparser_context_t *context,
    char * *ports_ptr)
{
    diag_portlist_t portlist;
    rtk_port_t port;
    rtk_enable_t enable;
    int32 ret = RT_ERR_FAILED;
        
    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    DIAG_UTIL_ERR_CHK(DIAG_UTIL_EXTRACT_PORTLIST(portlist, 4), ret);

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
			diag_util_mprintf("Port Status\n");
		    DIAG_UTIL_PORTMASK_SCAN(portlist, port)
		    {    
				DIAG_UTIL_ERR_CHK(reg_array_field_read(APOLLOMP_OUTPUT_DROP_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &enable), ret); 			
				diag_util_mprintf("%-4d %s\n", port, diagStr_enable[enable]);  
			}
            break;
#endif    
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_output_drop_port_ports_all_state */

/*
 * switch set output-drop ( broadcast | unknown-unicast | multicast ) state ( disable | enable )
 */
cparser_result_t
cparser_cmd_switch_set_output_drop_broadcast_unknown_unicast_multicast_state_disable_enable(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
	
    if ('d' == TOKEN_CHAR(5,0))
    {
        enable = DISABLED;
    }
    else if ('e' == TOKEN_CHAR(5,0))
    {
        enable = ENABLED;
    }

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                if ((ret = reg_field_write(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_BC_SELf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                if ((ret = reg_field_write(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_MC_SELf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                if ((ret = reg_field_write(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_UC_SELf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
            break;
#endif    
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

    return CPARSER_OK;
}    /* end of cparser_cmd_switch_set_output_drop_broadcast_unknown_unicast_multicast_state_disable_enable */

/*
 * switch get output-drop ( broadcast | unknown-unicast | multicast ) state
 */
cparser_result_t
cparser_cmd_switch_get_output_drop_broadcast_unknown_unicast_multicast_state(
    cparser_context_t *context)
{
    int32     ret = RT_ERR_FAILED;
    rtk_enable_t enable;

    DIAG_UTIL_PARAM_CHK();
    DIAG_UTIL_OUTPUT_INIT();

    switch(DIAG_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            if(!osal_strcmp(TOKEN_STR(3),"broadcast"))
            {
                if ((ret = reg_field_read(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_BC_SELf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
            }
            else if(!osal_strcmp(TOKEN_STR(3),"multicast"))
            {
                if ((ret = reg_field_read(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_MC_SELf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
            }
            else if(!osal_strcmp(TOKEN_STR(3),"unknown-unicast"))
            {
                if ((ret = reg_field_read(APOLLOMP_OUTPUT_DROP_CFGr, APOLLOMP_OD_UC_SELf, &enable)) != RT_ERR_OK)
                {
                    RT_ERR(ret, (MOD_DAL|MOD_SWITCH), "");
                    return ret;
                }
            }
            else
                return CPARSER_ERR_INVALID_PARAMS;
        
        	diag_util_mprintf("%s\n", diagStr_enable[enable]);  
            break;
#endif    
       default:
            diag_util_mprintf("%s\n",DIAG_STR_NOTSUPPORT);
            return CPARSER_NOT_OK;        
            break;
    }

	
	
    return CPARSER_OK;
}    /* end of cparser_cmd_switch_get_output_drop_broadcast_unknown_unicast_multicast_state */




