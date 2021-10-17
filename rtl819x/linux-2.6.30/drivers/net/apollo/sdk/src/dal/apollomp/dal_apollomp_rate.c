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
 * $Revision: 11944 $
 * $Date: 2010-08-12 09:23:54 +0800 (?Ÿæ??? 12 ?«æ? 2010) $
 *
 * Purpose : Definition of Port Bandwidth Control and Storm Control API 
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration of Ingress Port Bandwidth Control [Ingress Rate Limit]
 *           (2) Configuration of Egress  Port Bandwidth Control [Egress  Rate Limit]
 *           (3) Configuration of Storm Control
 *           (3) Configuration of meter
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/rate.h>
#include <dal/apollomp/dal_apollomp_rate.h>
#include <dal/apollomp/raw/apollomp_raw_trap.h>
#include <dal/apollomp/raw/apollomp_raw_qos.h>

/*
 * Symbol Definition
 */

#define DAL_APOLLOMP_STORM_UM_IDX         0
#define DAL_APOLLOMP_STORM_UC_IDX         1
#define DAL_APOLLOMP_STORM_MC_IDX         2
#define DAL_APOLLOMP_STORM_BC_IDX         3
#define DAL_APOLLOMP_STORM_DHCP_IDX       4
#define DAL_APOLLOMP_STORM_ARP_IDX        5
#define DAL_APOLLOMP_STORM_IGMP_MLD_IDX   6
#define DAL_APOLLOMP_STORM_MAX_IDX        7

#define DAL_APOLLOMP_STORM_MAX_ENTRY      4



typedef enum apollomp_storm_alt_type_e
{
    APOLLOMP_STORM_ALT_DEFAULT = 0,
    APOLLOMP_STORM_ALT_ARP,
    APOLLOMP_STORM_ALT_DHCP,
    APOLLOMP_STORM_ALT_IGMP_MLD,
    APOLLOMP_STORM_ALT_TYPE_END
} apollomp_storm_alt_type_t;

typedef struct dal_apollomp_storm_entry_s
{
    uint8   enable;
    uint32  portEnReg;
    uint32  portMeterReg;    
} dal_apollomp_storm_entry_t;


typedef struct dal_apollomp_storm_ctrl_s
{
    dal_apollomp_storm_entry_t entry[DAL_APOLLOMP_STORM_MAX_IDX];
    uint8 phyEntryUsage[DAL_APOLLOMP_STORM_MAX_ENTRY];
} dal_apollomp_storm_ctrl_t;



/*
 * Data Declaration
 */
static uint32    rate_init = {INIT_NOT_COMPLETED}; 

static dal_apollomp_storm_ctrl_t stromCfg;

/*
 * Function Declaration
 */

/*
 * Function Declaration
 */
static int32 
_dal_apollomp_rate_stromCtrl_disable_set(void)
{
    int32   ret,i;
    uint32  port;
    uint32  regList[DAL_APOLLOMP_STORM_MAX_ENTRY]={APOLLOMP_STORM_CTRL_UM_CTRLr,
                                                   APOLLOMP_STORM_CTRL_UC_CTRLr,
                                                   APOLLOMP_STORM_CTRL_MC_CTRLr,
                                                   APOLLOMP_STORM_CTRL_BC_CTRLr}; 
    uint32  data;

    data = DISABLED;
    for(i=0 ; i<DAL_APOLLOMP_STORM_MAX_ENTRY ; i++)
    {
        HAL_SCAN_ALL_PORT(port)    
        {
            if ((ret = reg_array_field_write(regList[i], port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;
            }
        }    
    }

    stromCfg.entry[DAL_APOLLOMP_STORM_UC_IDX].enable       = DISABLED;
    stromCfg.entry[DAL_APOLLOMP_STORM_UM_IDX].enable       = DISABLED;
    stromCfg.entry[DAL_APOLLOMP_STORM_BC_IDX].enable       = DISABLED;
    stromCfg.entry[DAL_APOLLOMP_STORM_MC_IDX].enable       = DISABLED;
    stromCfg.entry[DAL_APOLLOMP_STORM_DHCP_IDX].enable     = DISABLED;
    stromCfg.entry[DAL_APOLLOMP_STORM_ARP_IDX].enable      = DISABLED;
    stromCfg.entry[DAL_APOLLOMP_STORM_IGMP_MLD_IDX].enable = DISABLED;
    
    return RT_ERR_OK;
}



static int32 
_dal_apollomp_rate_bandwidthCtrl_disable_set(void)
{
    int32   ret,queue;
    uint32  port;
    
    HAL_SCAN_ALL_PORT(port)    
    {
        if ((ret = dal_apollomp_rate_portIgrBandwidthCtrlRate_set(port,APOLLOMP_RATE_MAX)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;                
        }

        if ((ret = dal_apollomp_rate_portEgrBandwidthCtrlRate_set(port,APOLLOMP_RATE_MAX)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;                
        }
        
        if(3==port)
            continue;
        
        for(queue=0 ; queue<HAL_MAX_NUM_OF_QUEUE() ; queue++)
        {
            if ((ret = dal_apollomp_rate_egrQueueBwCtrlEnable_set(port,queue,DISABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
                return ret;                
            }
        }
    }    

    return RT_ERR_OK;
}



/* Function Name:
 *      dal_apollomp_rate_init
 * Description:
 *      Initial the rate module.
 * Input:
 *      None.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_apollomp_rate_init(void)
{
        int32   ret;
        
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /*diable all storm control*/
    if ((ret = _dal_apollomp_rate_stromCtrl_disable_set()) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    rate_init = INIT_COMPLETED;    
    /*disable bandwidth control*/
    if ((ret = _dal_apollomp_rate_bandwidthCtrl_disable_set()) != RT_ERR_OK)
    {
        rate_init = INIT_NOT_COMPLETED;
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }    
    
    rate_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_init */

/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of ingress port bandwidth control */




/* Function Name:
 *      dal_apollomp_rate_portIgrBandwidthCtrlRate_get
 * Description:
 *      Get the ingress bandwidth control rate.
 * Input:
 *      port  - port id
 * Output:
 *      pRate - ingress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in apollo is 8Kbps.
 */
int32
dal_apollomp_rate_portIgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RATEf, pRate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_portIgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_apollomp_rate_portIgrBandwidthCtrlRate_set
 * Description:
 *      Set the ingress bandwidth control rate.
 * Input:
 *      port - port id
 *      rate - ingress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in apollo is 8Kbps.
 */
int32
dal_apollomp_rate_portIgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_RATE_MAX < rate), RT_ERR_INPUT);

    if ((ret = reg_array_field_write(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_portIgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_apollomp_rate_portIgrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      port  - port id
 * Output:
 *      pIfgInclude - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_rate_portIgrBandwidthCtrlIncludeIfg_get(rtk_port_t port, rtk_enable_t *pIfgInclude)
{

    int32   ret;
    uint32  data;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);
    
    if ((ret = reg_array_field_read(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    
    if(APOLLOMP_QOS_IFG_EXCLUDE == data)
        *pIfgInclude = DISABLED;
    else
        *pIfgInclude = ENABLED;
        
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_portIgrBandwidthCtrlIncludeIfg_get */

/* Function Name:
 *      dal_apollomp_rate_portIgrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of ingress bandwidth control includes IFG or not.
 * Input:
 *      port  - port id
 *      ifgInclude - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Ingress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifgInclude:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_rate_portIgrBandwidthCtrlIncludeIfg_set(rtk_port_t port, rtk_enable_t ifgInclude)
{

    int32   ret;
    uint32  data;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);

    if(ENABLED==ifgInclude)
        data = APOLLOMP_QOS_IFG_INCLUDE;
    else
        data = APOLLOMP_QOS_IFG_EXCLUDE;

    if ((ret = reg_array_field_write(APOLLOMP_IGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_portIgrBandwidthCtrlIncludeIfg_set */



/* Module Name    : Rate                                           */
/* Sub-module Name: Configuration of egress port bandwidth control */

/* Function Name:
 *      dal_apollomp_rate_portEgrBandwidthCtrlRate_get
 * Description:
 *      Get the egress bandwidth control rate.
 * Input:
 *      port  - port id
 * Output:
 *      pRate - egress bandwidth control rate
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_apollomp_rate_portEgrBandwidthCtrlRate_get(rtk_port_t port, uint32 *pRate)
{
    int32   ret;
    uint32  data;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);


    if ((ret = reg_array_field_read(APOLLOMP_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RATEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    *pRate = data;
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_portEgrBandwidthCtrlRate_get */

/* Function Name:
 *      dal_apollomp_rate_portEgrBandwidthCtrlRate_set
 * Description:
 *      Set the egress bandwidth control rate.
 * Input:
 *      port - port id
 *      rate - egress bandwidth control rate
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_RATE    - Invalid input rate
 * Note:
 *      (1) The actual rate is "rate * chip granularity".
 *      (2) The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_apollomp_rate_portEgrBandwidthCtrlRate_set(rtk_port_t port, uint32 rate)
{

    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((APOLLOMP_RATE_MAX < rate), RT_ERR_INPUT);
    
    if ((ret = reg_array_field_write(APOLLOMP_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RATEf, &rate)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_portEgrBandwidthCtrlRate_set */

/* Function Name:
 *      dal_apollomp_rate_portEgrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Per port get the status of egress bandwidth control includes IFG or not.
 * Input:
 *      None
 * Output:
 *      pIfgInclude - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_rate_portEgrBandwidthCtrlIncludeIfg_get(rtk_port_t port,rtk_enable_t *pIfgInclude)
{
    int32   ret;
    uint32  data;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if ((ret = reg_array_field_read(APOLLOMP_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    if(APOLLOMP_QOS_IFG_INCLUDE==data)
        *pIfgInclude = ENABLED;
    else
        *pIfgInclude = DISABLED;
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_portEgrBandwidthCtrlIncludeIfg_get */



/* Function Name:
 *      dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_get
 * Description:
 *      Get the status of egress bandwidth control includes IFG or not.
 * Input:
 *      None
 * Output:
 *      pIfgInclude - include IFG or not
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_get(rtk_enable_t *pIfgInclude)
{
    int32   ret;
    uint32  data;
    rtk_port_t port;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);
    
    port = 0;
    if ((ret = reg_array_field_read(APOLLOMP_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }

    if(APOLLOMP_QOS_IFG_INCLUDE==data)
        *pIfgInclude = ENABLED;
    else
        *pIfgInclude = DISABLED;
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_get */


/* Function Name:
 *      rtk_rate_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Per port set the status of egress bandwidth control includes IFG or not.
 * Input:
 *      port    - port id
 *      ifgInclude - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_rate_portEgrBandwidthCtrlIncludeIfg_set(rtk_port_t port,rtk_enable_t ifgInclude)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END  <= ifgInclude), RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    if(ENABLED==ifgInclude)
        data = APOLLOMP_QOS_IFG_INCLUDE;
    else
        data = APOLLOMP_QOS_IFG_EXCLUDE;

    if ((ret = reg_array_field_write(APOLLOMP_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IFGf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }   

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_set */



/* Function Name:
 *      dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_set
 * Description:
 *      Set the status of egress bandwidth control includes IFG or not.
 * Input:
 *      ifgInclude - include IFG or not
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      (1) Egress bandwidth control includes/excludes the Preamble & IFG (20 Bytes).
 *      (2) The status of ifg_include:
 *          - DISABLED
 *          - ENABLED
 */
int32
dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_set(rtk_enable_t ifgInclude)
{
    int32   ret;
    uint32  data;
    rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END  <= ifgInclude), RT_ERR_INPUT);

    if(ENABLED==ifgInclude)
        data = APOLLOMP_QOS_IFG_INCLUDE;
    else
        data = APOLLOMP_QOS_IFG_EXCLUDE;
    
    HAL_SCAN_ALL_PORT(port)
    {
        if ((ret = reg_array_field_write(APOLLOMP_EGR_BWCTRL_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IFGf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
            return ret;
        }   
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_egrBandwidthCtrlIncludeIfg_set */


/* Function Name:
 *      dal_apollomp_rate_egrQueueBwCtrlEnable_get
 * Description:
 *      Get enable status of egress bandwidth control on specified queue.
 * Input:
 *      port    - port id
 *      queue   - queue id
 * Output:
 *      pEnable - Pointer to enable status of egress queue bandwidth control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_rate_egrQueueBwCtrlEnable_get(
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    *pEnable)
{
    int ret;

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    
    if ((ret = apollomp_raw_qos_aprEnable_get(port, queue, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }    
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_egrQueueBwCtrlEnable_get */

/* Function Name:
 *      dal_apollomp_rate_egrQueueBwCtrlEnable_set
 * Description:
 *      Set enable status of egress bandwidth control on specified queue.
 * Input:
 *      port   - port id
 *      queue  - queue id
 *      enable - enable status of egress queue bandwidth control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_rate_egrQueueBwCtrlEnable_set(
    rtk_port_t      port,
    rtk_qid_t       queue,
    rtk_enable_t    enable)
{
    int ret;
   
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    
    if ((ret = apollomp_raw_qos_aprEnable_set(port, queue, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }    
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_egrQueueBwCtrlEnable_set */

/* Function Name:
 *      dal_apollomp_rate_egrQueueBwCtrlMeterIdx_get
 * Description:
 *      Get rate of egress bandwidth control on specified queue.
 * Input:
 *      port  - port id
 *      queue - queue id
 * Output:
 *      pMeterIndex - meter index
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_QUEUE_ID     - invalid queue id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_apollomp_rate_egrQueueBwCtrlMeterIdx_get(
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      *pMeterIndex)
{
    int ret;
    uint32 offsetIdx;
    
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((NULL == pMeterIndex), RT_ERR_NULL_POINTER);
    
    if ((ret = apollomp_raw_qos_aprMeter_get(port, queue, &offsetIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }

    *pMeterIndex = offsetIdx + ((port%4)*8);

        
    return RT_ERR_OK;    
} /* end of dal_apollomp_rate_egrQueueBwCtrlMeterIdx_get */

/* Function Name:
 *      dal_apollomp_rate_egrQueueBwCtrlMeterIdx_set
 * Description:
 *      Set rate of egress bandwidth control on specified queue.
 * Input:
 *      port  - port id
 *      queue - queue id
 *      meterIndex  - meter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_QUEUE_ID - invalid queue id
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *    The actual rate is "rate * chip granularity".
 *    The unit of granularity in Apollo is 8Kbps.
 */
int32
dal_apollomp_rate_egrQueueBwCtrlMeterIdx_set(
    rtk_port_t  port,
    rtk_qid_t   queue,
    uint32      meterIndex)
{
    int ret;
    uint32 offsetIdx;
    
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_QUEUE() <= queue), RT_ERR_QUEUE_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= meterIndex), RT_ERR_FILTER_METER_ID);

    if (meterIndex < ((port%4)*8) ||  meterIndex > (7 + (port%4)*8))
        return RT_ERR_FILTER_METER_ID;

    offsetIdx = meterIndex - ((port%4)*8);

    if ((ret = apollomp_raw_qos_aprMeter_set(port, queue, offsetIdx)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_QOS|MOD_DAL), "");
        return ret;
    }   
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_egrQueueBwCtrlMeterIdx_set */

/* Module Name    : Rate                           */
/* Sub-module Name: Configuration of storm control */

/* Function Name:
 *      dal_apollomp_rate_stormControlRate_get
 * Description:
 *      Get the storm control meter index.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 * Output:
 *      pIndex     - storm control meter index.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - Invalid port id
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 *    - Before call this API must make sure the global strom gruop for given group is enabled,
 *      otherwise this API will return RT_ERR_ENTRY_NOTFOUND
 */
int32
dal_apollomp_rate_stormControlMeterIdx_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  *pIndex)
{
    int32   ret;
    uint32  groupIdx;
    uint32  data;
    uint32  reg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pIndex), RT_ERR_NULL_POINTER);
    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:    
            groupIdx = DAL_APOLLOMP_STORM_UC_IDX;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:    
            groupIdx = DAL_APOLLOMP_STORM_UM_IDX;
            break;
        case STORM_GROUP_MULTICAST:    
            groupIdx = DAL_APOLLOMP_STORM_MC_IDX;
            break;
        case STORM_GROUP_BROADCAST:    
            groupIdx = DAL_APOLLOMP_STORM_BC_IDX;
            break;
        case STORM_GROUP_DHCP:    
            groupIdx = DAL_APOLLOMP_STORM_DHCP_IDX;
            break;
        case STORM_GROUP_ARP:    
            groupIdx = DAL_APOLLOMP_STORM_ARP_IDX;
            break;
        case STORM_GROUP_IGMP_MLD:    
            groupIdx = DAL_APOLLOMP_STORM_IGMP_MLD_IDX;
            break;
        default:
           return RT_ERR_INPUT;    
    }
    if(!stromCfg.entry[groupIdx].enable)
        return RT_ERR_ENTRY_NOTFOUND;

    reg = stromCfg.entry[groupIdx].portMeterReg;

    if ((ret = reg_array_field_read(reg, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IDXf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    
    *pIndex = data;

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_stormControlMeterIdx_get */

/* Function Name:
 *      dal_apollomp_rate_stormControlMeterIdx_set
 * Description:
 *      Set the storm control meter index.
 * Input:
 *      port       - port id
 *      storm_type - storm group type
 *      index       - storm control meter index.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - Invalid port id
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE    - Invalid input bandwidth
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 *    - Before call this API must make sure the global strom gruop for given group is enabled,
 *      otherwise this API will return RT_ERR_ENTRY_NOTFOUND
 */
int32
dal_apollomp_rate_stormControlMeterIdx_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    uint32                  index)
{
    int32   ret;
    uint32  groupIdx;
    uint32  data;
    uint32  reg;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_NULL_POINTER);

    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:    
            groupIdx = DAL_APOLLOMP_STORM_UC_IDX;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:    
            groupIdx = DAL_APOLLOMP_STORM_UM_IDX;
            break;
        case STORM_GROUP_MULTICAST:    
            groupIdx = DAL_APOLLOMP_STORM_MC_IDX;
            break;
        case STORM_GROUP_BROADCAST:    
            groupIdx = DAL_APOLLOMP_STORM_BC_IDX;
            break;
        case STORM_GROUP_DHCP:    
            groupIdx = DAL_APOLLOMP_STORM_DHCP_IDX;
            break;
        case STORM_GROUP_ARP:    
            groupIdx = DAL_APOLLOMP_STORM_ARP_IDX;
            break;
        case STORM_GROUP_IGMP_MLD:    
            groupIdx = DAL_APOLLOMP_STORM_IGMP_MLD_IDX;
            break;
        default:
           return RT_ERR_INPUT;    
    }
    if(!stromCfg.entry[groupIdx].enable)
        return RT_ERR_ENTRY_NOTFOUND;
        
    reg = stromCfg.entry[groupIdx].portMeterReg;
    
    data = index;
    if ((ret = reg_array_field_write(reg, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IDXf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }        
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_stormControlMeterIdx_set */

/* Function Name:
 *      dal_apollomp_rate_stormControlEnable_get
 * Description:
 *      Get enable status of storm control on specified port.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 * Output:
 *      pEnable    - pointer to enable status of storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 *    - When global strom gruop for given strom type is disabled,
 *      API will return DISABLED  
 */
int32
dal_apollomp_rate_stormControlPortEnable_get(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            *pEnable)
{
    int32   ret;
    uint32  groupIdx;
    uint32  data;
    uint32  reg;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:    
            groupIdx = DAL_APOLLOMP_STORM_UC_IDX;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:    
            groupIdx = DAL_APOLLOMP_STORM_UM_IDX;
            break;
        case STORM_GROUP_MULTICAST:    
            groupIdx = DAL_APOLLOMP_STORM_MC_IDX;
            break;
        case STORM_GROUP_BROADCAST:    
            groupIdx = DAL_APOLLOMP_STORM_BC_IDX;
            break;
        case STORM_GROUP_DHCP:    
            groupIdx = DAL_APOLLOMP_STORM_DHCP_IDX;
            break;
        case STORM_GROUP_ARP:    
            groupIdx = DAL_APOLLOMP_STORM_ARP_IDX;
            break;
        case STORM_GROUP_IGMP_MLD:    
            groupIdx = DAL_APOLLOMP_STORM_IGMP_MLD_IDX;
            break;
        default:
           return RT_ERR_INPUT;    
    }
    if(!stromCfg.entry[groupIdx].enable)
    {
        *pEnable = DISABLED;
        return RT_ERR_OK;
    }
    reg = stromCfg.entry[groupIdx].portEnReg ;

    if ((ret = reg_array_field_read(reg, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }
    
    if(data)
        *pEnable = ENABLED;
    else
        *pEnable = DISABLED;
                
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_stormControlPortEnable_get */

/* Function Name:
 *      dal_apollomp_rate_stormControlPortEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      port       - port id
 *      stormType  - storm group type
 *      enable     - enable status of storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_PORT_ID           - invalid port id
 *      RT_ERR_SFC_UNKNOWN_GROUP - Unknown storm group
 *      RT_ERR_ENTRY_NOTFOUND    - The global strom group is not enable for this group
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 *    - Before call this API must make sure the global strom gruop for given group is enabled,
 *      otherwise this API will return RT_ERR_ENTRY_NOTFOUND  
 */
int32
dal_apollomp_rate_stormControlPortEnable_set(
    rtk_port_t              port,
    rtk_rate_storm_group_t  stormType,
    rtk_enable_t            enable)
{
    int32   ret;
    uint32  groupIdx;
    uint32  data;
    uint32  reg;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
  
    switch(stormType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:    
            groupIdx = DAL_APOLLOMP_STORM_UC_IDX;
            break;
        case STORM_GROUP_UNKNOWN_MULTICAST:    
            groupIdx = DAL_APOLLOMP_STORM_UM_IDX;
            break;
        case STORM_GROUP_MULTICAST:    
            groupIdx = DAL_APOLLOMP_STORM_MC_IDX;
            break;
        case STORM_GROUP_BROADCAST:    
            groupIdx = DAL_APOLLOMP_STORM_BC_IDX;
            break;
        case STORM_GROUP_DHCP:    
            groupIdx = DAL_APOLLOMP_STORM_DHCP_IDX;
            break;
        case STORM_GROUP_ARP:    
            groupIdx = DAL_APOLLOMP_STORM_ARP_IDX;
            break;
        case STORM_GROUP_IGMP_MLD:    
            groupIdx = DAL_APOLLOMP_STORM_IGMP_MLD_IDX;
            break;
        default:
           return RT_ERR_INPUT;    
    }
    if(!stromCfg.entry[groupIdx].enable)
        return RT_ERR_ENTRY_NOTFOUND;

    reg = stromCfg.entry[groupIdx].portEnReg ;
    data = (uint32)enable;

    if ((ret = reg_array_field_write(reg, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_stormControlPortEnable_set */


/* Function Name:
 *      dal_apollomp_rate_stormControlEnable_get
 * Description:
 *      Get enable status of storm control on specified port.
 * Input:
 *      stormType  - storm group type
 * Output:
 *      pEnable    - pointer to enable status of storm control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 */
int32
dal_apollomp_rate_stormControlEnable_get(rtk_rate_storm_group_ctrl_t  *stormCtrl)
{

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == stormCtrl), RT_ERR_NULL_POINTER);

    stormCtrl->unknown_unicast_enable  =stromCfg.entry[DAL_APOLLOMP_STORM_UC_IDX].enable       ;   
    stormCtrl->unknown_multicast_enable=stromCfg.entry[DAL_APOLLOMP_STORM_UM_IDX].enable       ;     
    stormCtrl->broadcast_enable        =stromCfg.entry[DAL_APOLLOMP_STORM_BC_IDX].enable       ; 
    stormCtrl->multicast_enable        =stromCfg.entry[DAL_APOLLOMP_STORM_MC_IDX].enable       ; 
    stormCtrl->dhcp_enable             =stromCfg.entry[DAL_APOLLOMP_STORM_DHCP_IDX].enable     ; 
    stormCtrl->arp_enable              =stromCfg.entry[DAL_APOLLOMP_STORM_ARP_IDX].enable      ; 
    stormCtrl->igmp_mld_enable         =stromCfg.entry[DAL_APOLLOMP_STORM_IGMP_MLD_IDX].enable ; 

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_stormControlEnable_get */


static int32
_dal_apollomp_rate_stormPortEnReg_get(uint32 gruopIdx, uint32 *reg)
{
    switch(gruopIdx)
    {
        case DAL_APOLLOMP_STORM_UM_IDX:
            *reg = APOLLOMP_STORM_CTRL_UM_CTRLr;
            break;

        case DAL_APOLLOMP_STORM_UC_IDX:
            *reg = APOLLOMP_STORM_CTRL_UC_CTRLr;
            break;

        case DAL_APOLLOMP_STORM_MC_IDX:
            *reg = APOLLOMP_STORM_CTRL_MC_CTRLr;
            break;

        case DAL_APOLLOMP_STORM_BC_IDX:
            *reg = APOLLOMP_STORM_CTRL_BC_CTRLr;
            break;
        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;

}

static int32
_dal_apollomp_rate_stormPortMeterReg_get(uint32 gruopIdx, uint32 *reg)
{
    switch(gruopIdx)
    {
        case DAL_APOLLOMP_STORM_UM_IDX:
            *reg = APOLLOMP_STORM_CTRL_UM_METER_IDXr;
            break;

        case DAL_APOLLOMP_STORM_UC_IDX:
            *reg = APOLLOMP_STORM_CTRL_UC_METER_IDXr;
            break;

        case DAL_APOLLOMP_STORM_MC_IDX:
            *reg = APOLLOMP_STORM_CTRL_MC_METER_IDXr;
            break;

        case DAL_APOLLOMP_STORM_BC_IDX:
            *reg = APOLLOMP_STORM_CTRL_BC_METER_IDXr;
            break;

        default:
            return RT_ERR_INPUT;
    }
    return RT_ERR_OK;
}



static int32
_dal_apollomp_rate_stormAltCtrlType_set(uint32 gruopIdx,rtk_rate_storm_group_t altType)
{
    int32  ret;
    uint32 regField;
    uint32 data;
    
    switch(gruopIdx)
    {
        case DAL_APOLLOMP_STORM_UM_IDX:
            regField=APOLLOMP_UNMC_TYPEf;
            break;
        case DAL_APOLLOMP_STORM_UC_IDX:
            regField=APOLLOMP_UNDA_TYPEf;
            break;
        case DAL_APOLLOMP_STORM_MC_IDX:
            regField=APOLLOMP_MC_TYPEf;
            break;
        case DAL_APOLLOMP_STORM_BC_IDX:
            regField=APOLLOMP_BC_TYPEf;
            break;
        default:
            return RT_ERR_INPUT;
    }

    switch(altType)
    {
        case STORM_GROUP_UNKNOWN_UNICAST:
        case STORM_GROUP_UNKNOWN_MULTICAST:
        case STORM_GROUP_MULTICAST:
        case STORM_GROUP_BROADCAST:
            data = APOLLOMP_STORM_ALT_DEFAULT;
            break;
        case STORM_GROUP_DHCP:
            data = APOLLOMP_STORM_ALT_DHCP;
            break;
        case STORM_GROUP_ARP:
            data = APOLLOMP_STORM_ALT_ARP;
            break;
        case STORM_GROUP_IGMP_MLD:
            data = APOLLOMP_STORM_ALT_IGMP_MLD;
            break;
        default:
            return RT_ERR_INPUT;
    }
    if ((ret = reg_field_write(APOLLOMP_STORM_CTRL_ALT_TYPE_SELr, regField, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }

    return RT_ERR_OK;
}


static int32
_dal_apollomp_rate_stormTypeEnable_set(uint32 entryIdx, rtk_rate_storm_group_t altType)
{
    uint32 gruopIdx;
    uint32  isEntryFind;
    int i;
    int32  ret;
                
    if(entryIdx > DAL_APOLLOMP_STORM_BC_IDX)
    {
        isEntryFind = 0;
        for(i=0 ; i<DAL_APOLLOMP_STORM_MAX_ENTRY ; i++)
        {
            if(0 == stromCfg.phyEntryUsage[i])        
            {
                isEntryFind = 1;
                gruopIdx = i;  
                break;
            }
        }
        if(isEntryFind != 1)
            return RT_ERR_INPUT;
    }
    else
    {
        gruopIdx = entryIdx;  
    }
    
    if ((ret = _dal_apollomp_rate_stormPortEnReg_get(gruopIdx, &stromCfg.entry[entryIdx].portEnReg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }       
    if ((ret = _dal_apollomp_rate_stormPortMeterReg_get(gruopIdx, &stromCfg.entry[entryIdx].portMeterReg)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }       
    stromCfg.phyEntryUsage[gruopIdx]=1;

    if ((ret = _dal_apollomp_rate_stormAltCtrlType_set(gruopIdx, altType)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
        return ret;
    }    
    return RT_ERR_OK;    
}    

/* Function Name:
 *      dal_apollomp_rate_stormControlEnable_set
 * Description:
 *      Set enable status of storm control on specified port.
 * Input:
 *      stormType  - storm group type
 *      enable     - enable status of storm control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_INPUT             - invalid input parameter
 * Note:
 *    The storm group types are as following:
 *    - STORM_GROUP_UNKNOWN_UNICAST
 *    - STORM_GROUP_UNKNOWN_MULTICAST
 *    - STORM_GROUP_MULTICAST
 *    - STORM_GROUP_BROADCAST
 *    - STORM_GROUP_DHCP
 *    - STORM_GROUP_ARP
 *    - STORM_GROUP_IGMP_MLD
 *    total 4 storm type can be enabled.
 *      - if total enable group exceed 4 system will return RT_ERR_INPUT
 *      - when global storm type set to disable the per port setting for this 
 *        storm type will also set to disable for all port.    
 */
int32
dal_apollomp_rate_stormControlEnable_set(rtk_rate_storm_group_ctrl_t  *stormCtrl)
{
    int32   ret;
    int32   cnt=0;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);
    
    /* parameter check */
    RT_PARAM_CHK((NULL == stormCtrl), RT_ERR_INPUT);

    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->unknown_unicast_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->unknown_multicast_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->broadcast_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->multicast_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->dhcp_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->arp_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= stormCtrl->igmp_mld_enable), RT_ERR_INPUT);

    
    if(stormCtrl->unknown_unicast_enable)
        cnt++;
    if(stormCtrl->unknown_multicast_enable)
        cnt++;
    if(stormCtrl->broadcast_enable)
        cnt++;
    if(stormCtrl->multicast_enable)
        cnt++;
    if(stormCtrl->dhcp_enable)
        cnt++;
    if(stormCtrl->arp_enable)
        cnt++;
    if(stormCtrl->igmp_mld_enable)
        cnt++;
    RT_PARAM_CHK((DAL_APOLLOMP_STORM_MAX_ENTRY < cnt), RT_ERR_INPUT);
    

    memset(&stromCfg,0x0,sizeof(dal_apollomp_storm_ctrl_t));

    
    /*disable all strom control for all port*/
    
    stromCfg.entry[DAL_APOLLOMP_STORM_UC_IDX].enable       = stormCtrl->unknown_unicast_enable;    
    stromCfg.entry[DAL_APOLLOMP_STORM_UM_IDX].enable       = stormCtrl->unknown_multicast_enable;    
    stromCfg.entry[DAL_APOLLOMP_STORM_BC_IDX].enable       = stormCtrl->broadcast_enable;    
    stromCfg.entry[DAL_APOLLOMP_STORM_MC_IDX].enable       = stormCtrl->multicast_enable;    
    stromCfg.entry[DAL_APOLLOMP_STORM_DHCP_IDX].enable     = stormCtrl->dhcp_enable;    
    stromCfg.entry[DAL_APOLLOMP_STORM_ARP_IDX].enable      = stormCtrl->arp_enable;    
    stromCfg.entry[DAL_APOLLOMP_STORM_IGMP_MLD_IDX].enable = stormCtrl->igmp_mld_enable;    

    /*set storm type*/
    if(stormCtrl->unknown_unicast_enable)
    {
        if ((ret = _dal_apollomp_rate_stormTypeEnable_set(DAL_APOLLOMP_STORM_UC_IDX, STORM_GROUP_UNKNOWN_UNICAST)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    if(stormCtrl->unknown_multicast_enable)
    {
        if ((ret = _dal_apollomp_rate_stormTypeEnable_set(DAL_APOLLOMP_STORM_UM_IDX, STORM_GROUP_UNKNOWN_MULTICAST)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    if(stormCtrl->multicast_enable)
    {
        if ((ret = _dal_apollomp_rate_stormTypeEnable_set(DAL_APOLLOMP_STORM_MC_IDX, STORM_GROUP_MULTICAST)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    if(stormCtrl->broadcast_enable)
    {
        if ((ret = _dal_apollomp_rate_stormTypeEnable_set(DAL_APOLLOMP_STORM_BC_IDX, STORM_GROUP_BROADCAST)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    if(stormCtrl->dhcp_enable)
    {
        if ((ret = _dal_apollomp_rate_stormTypeEnable_set(DAL_APOLLOMP_STORM_DHCP_IDX, STORM_GROUP_DHCP)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    if(stormCtrl->arp_enable)
    {
        if ((ret = _dal_apollomp_rate_stormTypeEnable_set(DAL_APOLLOMP_STORM_ARP_IDX, STORM_GROUP_ARP)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }
    if(stormCtrl->igmp_mld_enable)
    {
        if ((ret = _dal_apollomp_rate_stormTypeEnable_set(DAL_APOLLOMP_STORM_IGMP_MLD_IDX, STORM_GROUP_IGMP_MLD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RATE), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_stormControlEnable_set */




/* Function Name:
 *      dal_apollomp_rate_stormBypass_set
 * Description:
 *      Set bypass storm filter control configuration.
 * Input:
 *      type    - Bypass storm filter control type.
 *      enable  - Bypass status.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 *      RT_ERR_ENABLE 		- Invalid IFG parameter
 * Note:
 *
 *      This API can set per-port bypass stomr filter control frame type including RMA and igmp.
 *      The bypass frame type is as following:
 *      - BYPASS_BRG_GROUP,
 *      - BYPASS_FD_PAUSE,
 *      - BYPASS_SP_MCAST,
 *      - BYPASS_1X_PAE,
 *      - BYPASS_UNDEF_BRG_04,
 *      - BYPASS_UNDEF_BRG_05,
 *      - BYPASS_UNDEF_BRG_06,
 *      - BYPASS_UNDEF_BRG_07,
 *      - BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - BYPASS_UNDEF_BRG_09,
 *      - BYPASS_UNDEF_BRG_0A,
 *      - BYPASS_UNDEF_BRG_0B,
 *      - BYPASS_UNDEF_BRG_0C,
 *      - BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - BYPASS_8021AB,
 *      - BYPASS_UNDEF_BRG_0F,
 *      - BYPASS_BRG_MNGEMENT,
 *      - BYPASS_UNDEFINED_11,
 *      - BYPASS_UNDEFINED_12,
 *      - BYPASS_UNDEFINED_13,
 *      - BYPASS_UNDEFINED_14,
 *      - BYPASS_UNDEFINED_15,
 *      - BYPASS_UNDEFINED_16,
 *      - BYPASS_UNDEFINED_17,
 *      - BYPASS_UNDEFINED_18,
 *      - BYPASS_UNDEFINED_19,
 *      - BYPASS_UNDEFINED_1A,
 *      - BYPASS_UNDEFINED_1B,
 *      - BYPASS_UNDEFINED_1C,
 *      - BYPASS_UNDEFINED_1D,
 *      - BYPASS_UNDEFINED_1E,
 *      - BYPASS_UNDEFINED_1F,
 *      - BYPASS_GMRP,
 *      - BYPASS_GVRP,
 *      - BYPASS_UNDEF_GARP_22,
 *      - BYPASS_UNDEF_GARP_23,
 *      - BYPASS_UNDEF_GARP_24,
 *      - BYPASS_UNDEF_GARP_25,
 *      - BYPASS_UNDEF_GARP_26,
 *      - BYPASS_UNDEF_GARP_27,
 *      - BYPASS_UNDEF_GARP_28,
 *      - BYPASS_UNDEF_GARP_29,
 *      - BYPASS_UNDEF_GARP_2A,
 *      - BYPASS_UNDEF_GARP_2B,
 *      - BYPASS_UNDEF_GARP_2C,
 *      - BYPASS_UNDEF_GARP_2D,
 *      - BYPASS_UNDEF_GARP_2E,
 *      - BYPASS_UNDEF_GARP_2F,
 *      - BYPASS_IGMP.
 */
int32 
dal_apollomp_rate_stormBypass_set(rtk_storm_bypass_t type, rtk_enable_t enable)
{
    int32   ret;
    uint32  rmaTail;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((BYPASS_END <= type), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);
    if(BYPASS_IGMP == type)
    {
        if (( ret = apollomp_raw_trap_igmpBypassStrom_set(enable)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }         
    }    
    else if(BYPASS_CDP == type || BYPASS_CSSTP == type)
    {
        uint32 data;
        if(BYPASS_CDP == type)
            rmaTail = (uint32)0xCC;
        else
            rmaTail = (uint32)0xCD;
        
        if(enable == ENABLED)
            data = 2;
        else
            data = 0;
                    
        if (( ret = apollomp_raw_trap_rmaCiscoAction_set(rmaTail, data)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }
        
    }
    else
    {
        rmaTail = (uint32)type;
        if (( ret = apollomp_raw_trap_rmaFeature_set(rmaTail, APOLLOMP_RMA_FUN_BYPASS_STORM, enable)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }         
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_stormBypass_set */


/* Function Name:
 *      dal_apollomp_rate_stormBypass_get
 * Description:
 *      Get bypass storm filter control configuration.
 * Input:
 *      type - Bypass storm filter control type.
 * Output:
 *      pEnable - Bypass status.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      This API can get per-port bypass stomr filter control frame type including RMA and igmp.
 *      The bypass frame type is as following:
 *      - BYPASS_BRG_GROUP,
 *      - BYPASS_FD_PAUSE,
 *      - BYPASS_SP_MCAST,
 *      - BYPASS_1X_PAE,
 *      - BYPASS_UNDEF_BRG_04,
 *      - BYPASS_UNDEF_BRG_05,
 *      - BYPASS_UNDEF_BRG_06,
 *      - BYPASS_UNDEF_BRG_07,
 *      - BYPASS_PROVIDER_BRIDGE_GROUP_ADDRESS,
 *      - BYPASS_UNDEF_BRG_09,
 *      - BYPASS_UNDEF_BRG_0A,
 *      - BYPASS_UNDEF_BRG_0B,
 *      - BYPASS_UNDEF_BRG_0C,
 *      - BYPASS_PROVIDER_BRIDGE_GVRP_ADDRESS,
 *      - BYPASS_8021AB,
 *      - BYPASS_UNDEF_BRG_0F,
 *      - BYPASS_BRG_MNGEMENT,
 *      - BYPASS_UNDEFINED_11,
 *      - BYPASS_UNDEFINED_12,
 *      - BYPASS_UNDEFINED_13,
 *      - BYPASS_UNDEFINED_14,
 *      - BYPASS_UNDEFINED_15,
 *      - BYPASS_UNDEFINED_16,
 *      - BYPASS_UNDEFINED_17,
 *      - BYPASS_UNDEFINED_18,
 *      - BYPASS_UNDEFINED_19,
 *      - BYPASS_UNDEFINED_1A,
 *      - BYPASS_UNDEFINED_1B,
 *      - BYPASS_UNDEFINED_1C,
 *      - BYPASS_UNDEFINED_1D,
 *      - BYPASS_UNDEFINED_1E,
 *      - BYPASS_UNDEFINED_1F,
 *      - BYPASS_GMRP,
 *      - BYPASS_GVRP,
 *      - BYPASS_UNDEF_GARP_22,
 *      - BYPASS_UNDEF_GARP_23,
 *      - BYPASS_UNDEF_GARP_24,
 *      - BYPASS_UNDEF_GARP_25,
 *      - BYPASS_UNDEF_GARP_26,
 *      - BYPASS_UNDEF_GARP_27,
 *      - BYPASS_UNDEF_GARP_28,
 *      - BYPASS_UNDEF_GARP_29,
 *      - BYPASS_UNDEF_GARP_2A,
 *      - BYPASS_UNDEF_GARP_2B,
 *      - BYPASS_UNDEF_GARP_2C,
 *      - BYPASS_UNDEF_GARP_2D,
 *      - BYPASS_UNDEF_GARP_2E,
 *      - BYPASS_UNDEF_GARP_2F,
 *      - BYPASS_IGMP.
 */
int32
dal_apollomp_rate_stormBypass_get(rtk_storm_bypass_t type, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  rmaTail;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((BYPASS_END <= type), RT_ERR_NULL_POINTER);
    
    if(BYPASS_IGMP == type)
    {
        if (( ret = apollomp_raw_trap_igmpBypassStrom_get(pEnable)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }         
    }    
    else if(BYPASS_CDP == type || BYPASS_CSSTP == type)
    {
        uint32 data;

        if(BYPASS_CDP == type)
            rmaTail = (uint32)0xCC;
        else
            rmaTail = (uint32)0xCD;
        
        if (( ret = apollomp_raw_trap_rmaCiscoAction_get(rmaTail, &data)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }
        switch(data)
        {
            case 0:
                *pEnable = DISABLED;
                break;    
            case 2:   
                *pEnable = ENABLED;
                break; 
            default: 
                    return RT_ERR_FAILED;           
        }         
    }   
    else
    {
        rmaTail = (uint32)type;                               
        if (( ret = apollomp_raw_trap_rmaFeature_get(rmaTail, APOLLOMP_RMA_FUN_BYPASS_STORM, pEnable)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }         
    }
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_stormBypass_get */



/* Module Name    : Rate                                            */
/* Sub-module Name: Configuration of meter */

/* Function Name:
 *      dal_apollomp_rate_shareMeter_set
 * Description:
 *      Set meter configuration
 * Input:
 *      index       - shared meter index
 *      rate        - rate of share meter
 *      ifgInclude  - include IFG or not, ENABLE:include DISABLE:exclude
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 *      RT_ERR_RATE             - Invalid rate
 *      RT_ERR_INPUT            - Invalid input parameters
 * Note:
 *      The API can set shared meter rate and ifg include for each meter.
 *      The rate unit is 1 kbps and the range is from 8k to 1048568k.
 *      The granularity of rate is 8 kbps. The ifg_include parameter is used
 *      for rate calculation with/without inter-frame-gap and preamble.
 */
int32 
dal_apollomp_rate_shareMeter_set(uint32 index, uint32 rate, rtk_enable_t ifgInclude)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);
    
    value = rate>>3;
    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_RATE_MAX < value), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= ifgInclude), RT_ERR_INPUT);
    
    /*set IFG*/
    if(ENABLED==ifgInclude)
        value = 1;
    else
        value = 0;
    if (( ret = reg_array_field_write(APOLLOMP_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IFGf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }    

    /*set rate*/
    value = rate>>3;
    if (( ret = reg_array_field_write(APOLLOMP_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_RATEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
        
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_shareMeter_set */


/* Function Name:
 *      dal_apollomp_rate_shareMeter_get
 * Description:
 *      Get meter configuration
 * Input:
 *      index        - shared meter index
 * Output:
 *      pRate        - pointer of rate of share meter
 *      pIfgInclude  - include IFG or not, ENABLE:include DISABLE:exclude
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter rate and ifg include for each meter.
 *      The rate unit is 1 kbps and the granularity of rate is 8 kbps.
 *      The ifg_include parameter is used for rate calculation with/without inter-frame-gap and preamble
 */
int32 
dal_apollomp_rate_shareMeter_get(uint32 index, uint32 *pRate , rtk_enable_t *pIfgInclude)
{
    int32   ret;
    uint32  value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pRate), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIfgInclude), RT_ERR_NULL_POINTER);

    /*get include IFG status*/
    if (( ret = reg_array_field_read(APOLLOMP_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_IFGf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    if(1 == value)
        *pIfgInclude = ENABLED;    
    else
        *pIfgInclude = DISABLED;    

    /*get rate setting*/
    if (( ret = reg_array_field_read(APOLLOMP_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_RATEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    *pRate = value*8;

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_shareMeter_get */

/* Function Name:
 *      dal_apollomp_rate_shareMeterBucket_set
 * Description:
 *      Set meter Bucket Size
 * Input:
 *      index        - shared meter index
 *      bucketSize   - Bucket Size
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_INPUT            - Error Input
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can set shared meter bucket size.
 */
int32 
dal_apollomp_rate_shareMeterBucket_set(uint32 index, uint32 bucketSize)
{
    uint32  value;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_METER_BUCKETSIZE_MAX < bucketSize), RT_ERR_INPUT);
    
    value = bucketSize;
    if ((ret = reg_array_field_write(APOLLOMP_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_BUCKET_SIZEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }    
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_shareMeterBucket_set */


/* Function Name:
 *      dal_apollomp_rate_shareMeterBucket_get
 * Description:
 *      Get meter Bucket Size
 * Input:
 *      index        - shared meter index
 * Output:
 *      pBucketSize - Bucket Size
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_FILTER_METER_ID  - Invalid meter
 * Note:
 *      The API can get shared meter bucket size.
 */
int32 
dal_apollomp_rate_shareMeterBucket_get(uint32 index, uint32 *pBucketSize)
{
    int32   ret;
    uint32  value;
    
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pBucketSize), RT_ERR_NULL_POINTER);

	if (( ret = reg_array_field_read(APOLLOMP_METER_GLB_CTRLr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_BUCKET_SIZEf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }
    *pBucketSize = value;

    return RT_ERR_OK;
} /* end of dal_apollomp_rate_shareMeterBucket_get */



/* Function Name:
 *      dal_apollomp_rate_shareMeterExceed_get
 * Description:
 *      Get exceed meter status.
 * Input:
 *      index        - shared meter index
 * Output:
 *      pIsExceed  - pointer to exceed status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 *      RT_ERR_NULL_POINTER      - input parameter may be null pointer
 * Note:
 *      - TRUE      - rate is more than configured rate.
 *      - FALSE     - rate is not over then configured rate.
 */
int32
dal_apollomp_rate_shareMeterExceed_get(
    uint32                  index,
    uint32                  *pIsExceed)
{
    int32   ret;
    uint32  value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pIsExceed), RT_ERR_NULL_POINTER);
    
	if (( ret = reg_array_field_read(APOLLOMP_METER_LB_EXCEED_STSr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_LB_EXCEEDf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }    
    *pIsExceed = value;
    
    /*clear meter exceed status*/
    if(value)
    {
        value = 1;
        if ((ret = reg_array_field_write(APOLLOMP_METER_LB_EXCEED_STSr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_LB_EXCEEDf, &value)) != RT_ERR_OK )
        {
            RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
            return ret;
        }        
    }
    
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_shareMeterExceed_get */

/* Function Name:
 *      dal_apollomp_rate_shareMeterExceed_clear
 * Description:
 *      Clear share meter exceed status.
 * Input:
 *      index        - shared meter index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT          - The module is not initial
 * Note:
 */
int32 dal_apollomp_rate_shareMeterExceed_clear(uint32 index)
{
    int32   ret;
    uint32  value;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RATE),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(rate_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_MAX_NUM_OF_METERING() <= index), RT_ERR_INPUT);

    /*clear meter exceed status*/
    value = 1;
    if ((ret = reg_array_field_write(APOLLOMP_METER_LB_EXCEED_STSr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_LB_EXCEEDf, &value)) != RT_ERR_OK )
    {
        RT_ERR(ret, (MOD_RATE|MOD_DAL), "");
        return ret;
    }        
    
    return RT_ERR_OK;
} /* end of dal_apollomp_rate_shareMeterExceed_clear */
