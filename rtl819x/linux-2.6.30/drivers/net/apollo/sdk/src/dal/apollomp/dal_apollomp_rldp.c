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
 * $Revision: $
 * $Date: $
 *
 * Purpose : Declaration of RLDP and RLPP API
 *
 * Feature : The file have include the following module and sub-modules
 *           1) RLDP and RLPP configuration and status
 *
 */


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/rldp.h>
#include <dal/apollomp/dal_apollomp_rldp.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32    apollomp_rldp_init = {INIT_NOT_COMPLETED}; 
static uint32    apollomp_rlpp_init = {INIT_NOT_COMPLETED}; 


/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : RLDP */

/* Function Name:
 *      dal_apollomp_rldp_init
 * Description:
 *      Initialize rldp module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize rldp module before calling any rldp APIs.
 */
int32
dal_apollomp_rldp_init(void)
{
    int32   ret;
    rtk_port_t port;
    rtk_rldp_config_t config;
    rtk_rldp_portConfig_t portConfig;
    rtk_rldp_portStatus_t portStatus;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    apollomp_rldp_init = INIT_COMPLETED;

    /* Default system settings */
    config.rldp_enable = DISABLED;
    config.fc_mode = RTK_RLDP_FLOWCTRL_DROP;
    config.magic.octet[0] = 0;;
    config.magic.octet[1] = 0;;
    config.magic.octet[2] = 0;;
    config.magic.octet[3] = 0;;
    config.magic.octet[4] = 0;;
    config.magic.octet[5] = 0;;
    config.compare_type = RTK_RLDP_CMPTYPE_MAGIC;
    config.interval_check = 0;
    config.num_check = 0;
    config.interval_loop = 0;
    config.num_loop = 0;
    if ((ret = dal_apollomp_rldp_config_set(&config)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RLDP|MOD_DAL), "");
        apollomp_rldp_init = INIT_NOT_COMPLETED;
        return ret;
    }

    /* Default port settings */
    portConfig.tx_enable = DISABLED;
    HAL_SCAN_ALL_PORT(port)
    {
        if(!HAL_IS_CPU_PORT(port))
        {
            if ((ret = dal_apollomp_rldp_portConfig_set(port, &portConfig)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_RLDP|MOD_DAL), "");
                apollomp_rldp_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    /* Reset port status - write 1 to clear */
    portStatus.loop_enter = RTK_RLDP_LOOPSTS_LOOPING;
    portStatus.loop_leave = RTK_RLDP_LOOPSTS_LOOPING;
    HAL_SCAN_ALL_PORT(port)
    {
        if(!HAL_IS_CPU_PORT(port))
        {
            if ((ret = dal_apollomp_rldp_portStatus_clear(port, &portStatus)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_RLDP|MOD_DAL), "");
                apollomp_rldp_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rldp_init */


/* Function Name:
 *      dal_apollomp_rldp_config_set
 * Description:
 *      Set RLDP module configuration
 * Input:
 *      pConfig - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_rldp_config_set(rtk_rldp_config_t *pConfig)
{
    int32   ret;
    uint32 tmp[2];
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_rldp_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= pConfig->rldp_enable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_RLDP_FLOWCTRL_END <= pConfig->fc_mode), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_RLDP_CMPTYPE_END <= pConfig->compare_type), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_RLDP_INTERVAL_MAX < pConfig->interval_check), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_RLDP_NUM_MAX < pConfig->num_check), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_RLDP_INTERVAL_MAX < pConfig->interval_loop), RT_ERR_INPUT);
    RT_PARAM_CHK((APOLLOMP_RLDP_NUM_MAX < pConfig->num_loop), RT_ERR_INPUT);

    val = pConfig->rldp_enable;
    if ((ret = reg_field_write(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->fc_mode;
    if ((ret = reg_field_write(APOLLOMP_RLDP_CTRL_0r, APOLLOMP_ACT_RUNOUTDSCf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    tmp[0] = (pConfig->magic.octet[2]<<24) | (pConfig->magic.octet[3]<<16) | (pConfig->magic.octet[4]<<8) | (pConfig->magic.octet[5]); 
    tmp[1] = (pConfig->magic.octet[0]<<8) | (pConfig->magic.octet[1]);

    if ((ret = reg_field_write(APOLLOMP_RLDP_MAGIC_NUMr, APOLLOMP_NUM_31_0f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_write(APOLLOMP_RLDP_MAGIC_NUMr, APOLLOMP_NUM_47_32f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }	

    val = pConfig->compare_type;
    if ((ret = reg_field_write(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_CMP_TYPEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->interval_check;
    if ((ret = reg_field_write(APOLLOMP_RLDP_CHK_STS_CTRLr, APOLLOMP_PERIODf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->num_check;
    if ((ret = reg_field_write(APOLLOMP_RLDP_CHK_STS_CTRLr, APOLLOMP_CNTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->interval_loop;
    if ((ret = reg_field_write(APOLLOMP_RLDP_LP_STS_CTRLr, APOLLOMP_PERIODf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    val = pConfig->num_loop;
    if ((ret = reg_field_write(APOLLOMP_RLDP_LP_STS_CTRLr, APOLLOMP_CNTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rldp_config_set */


/* Function Name:
 *      dal_apollomp_rldp_config_get
 * Description:
 *      Get RLDP module configuration
 * Input:
 *      pConfig - configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_rldp_config_get(rtk_rldp_config_t *pConfig)
{
    int32 ret;
    uint32 val;
    uint32 tmp[2];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_rldp_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->rldp_enable = val;
    
    if ((ret = reg_field_read(APOLLOMP_RLDP_CTRL_0r, APOLLOMP_ACT_RUNOUTDSCf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->fc_mode = val;

    if ((ret = reg_field_read(APOLLOMP_RLDP_MAGIC_NUMr, APOLLOMP_NUM_31_0f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_RLDP_MAGIC_NUMr, APOLLOMP_NUM_47_32f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }	

    pConfig->magic.octet[0] = (tmp[1]&0x0000FF00) >>8;
    pConfig->magic.octet[1] = tmp[1]&0x000000FF;	
    pConfig->magic.octet[2] = (tmp[0]&0xFF000000) >>24;
    pConfig->magic.octet[3] = (tmp[0]&0x00FF0000) >>16;
    pConfig->magic.octet[4] = (tmp[0]&0x0000FF00) >>8;
    pConfig->magic.octet[5] = tmp[0]&0x000000FF;

    if ((ret = reg_field_read(APOLLOMP_RLDP_CTRL_1r, APOLLOMP_CMP_TYPEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->compare_type = val;

    if ((ret = reg_field_read(APOLLOMP_RLDP_CHK_STS_CTRLr, APOLLOMP_PERIODf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->interval_check = val;

    if ((ret = reg_field_read(APOLLOMP_RLDP_CHK_STS_CTRLr, APOLLOMP_CNTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->num_check = val;

    if ((ret = reg_field_read(APOLLOMP_RLDP_LP_STS_CTRLr, APOLLOMP_PERIODf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->interval_loop = val;

    if ((ret = reg_field_read(APOLLOMP_RLDP_LP_STS_CTRLr, APOLLOMP_CNTf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->num_loop = val;

    return RT_ERR_OK;
} /* end of dal_apollomp_rldp_config_get */


/* Function Name:
 *      dal_apollomp_rldp_portConfig_set
 * Description:
 *      Set per port RLDP module configuration
 * Input:
 *      port   - port number to be configured
 *      pConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_rldp_portConfig_set(rtk_port_t port, rtk_rldp_portConfig_t *pConfig)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_rldp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK(HAL_IS_CPU_PORT(port), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= pConfig->tx_enable), RT_ERR_INPUT);

    val = (uint32)pConfig->tx_enable;
    if ((ret = reg_array_field_write(APOLLOMP_RLDP_PORT_TX_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rldp_portConfig_set */


/* Function Name:
 *      dal_apollomp_rldp_portConfig_get
 * Description:
 *      Get per port RLDP module configuration
 * Input:
 *      port    - port number to be get
 *      pConfig - per port configuration structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_rldp_portConfig_get(rtk_port_t port, rtk_rldp_portConfig_t *pConfig)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_rldp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK(HAL_IS_CPU_PORT(port), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pConfig), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_RLDP_PORT_TX_ENr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pConfig->tx_enable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_rldp_portConfig_get */


/* Function Name:
 *      dal_apollomp_rldp_status_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      pStatus - status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_rldp_status_get(rtk_rldp_status_t *pStatus)
{
    int32   ret;
    uint32 tmp[2];

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_rldp_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_RLDP_RNDM_NUMr, APOLLOMP_NUM_31_0f, &tmp[0])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_RLDP_RNDM_NUMr, APOLLOMP_NUM_47_32f, &tmp[1])) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_RLDP | MOD_DAL), "");
        return ret;
    }	

    pStatus->id.octet[0] = (tmp[1]&0x0000FF00) >>8;
    pStatus->id.octet[1] = tmp[1]&0x000000FF;	
    pStatus->id.octet[2] = (tmp[0]&0xFF000000) >>24;
    pStatus->id.octet[3] = (tmp[0]&0x00FF0000) >>16;
    pStatus->id.octet[4] = (tmp[0]&0x0000FF00) >>8;
    pStatus->id.octet[5] = tmp[0]&0x000000FF;

    return RT_ERR_OK;
} /* end of dal_apollomp_rldp_status_get */


/* Function Name:
 *      dal_apollomp_rldp_portStatus_get
 * Description:
 *      Get RLDP module status
 * Input:
 *      port    - port number to be get
 *      pStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_rldp_portStatus_get(rtk_port_t port, rtk_rldp_portStatus_t *pStatus)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_rldp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK(HAL_IS_CPU_PORT(port), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_RLDP_PORT_LP_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_STSf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pStatus->loop_status = val;

    if ((ret = reg_array_field_read(APOLLOMP_RLDP_PORT_LP_ENTER_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_STSf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pStatus->loop_enter = val;

    if ((ret = reg_array_field_read(APOLLOMP_RLDP_PORT_LP_LEAVE_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_STSf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pStatus->loop_leave = val;

    if ((ret = reg_array_field_read(APOLLOMP_RLDP_PORT_LP_PNUMr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_NUMf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    pStatus->looped_port = val;

    return RT_ERR_OK;
} /* end of dal_apollomp_rldp_portStatus_get */


/* Function Name:
 *      dal_apollomp_rldp_portStatus_clear
 * Description:
 *      Get RLDP module status
 * Input:
 *      port    - port number to be get
 *      pStatus - per port status structure of RLDP
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      Clear operation effect loop_enter and loop_leave only, other field in
 *      the structure are don't care
 */
int32
dal_apollomp_rldp_portStatus_clear(rtk_port_t port, rtk_rldp_portStatus_t *pStatus)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_rldp_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK(HAL_IS_CPU_PORT(port), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_RLDP_LOOPSTS_END <= pStatus->loop_enter), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_RLDP_LOOPSTS_END <= pStatus->loop_leave), RT_ERR_INPUT);

    if(RTK_RLDP_LOOPSTS_LOOPING == pStatus->loop_enter)
    {
        val = pStatus->loop_enter;
        if ((ret = reg_array_field_write(APOLLOMP_RLDP_PORT_LP_ENTER_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_STSf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
            return ret;
        }
    }   

    if(RTK_RLDP_LOOPSTS_LOOPING == pStatus->loop_leave)
    {
        val = pStatus->loop_leave;
        if ((ret = reg_array_field_write(APOLLOMP_RLDP_PORT_LP_LEAVE_STSr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_STSf, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rldp_portStatus_clear */


/* Function Name:
 *      dal_apollomp_rlpp_init
 * Description:
 *      Initialize rlpp module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize rlpp module before calling any rlpp APIs.
 */
int32
dal_apollomp_rlpp_init(void)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    apollomp_rlpp_init = INIT_COMPLETED;

    if ((ret = dal_apollomp_rlpp_trapType_set(RTK_RLPP_TRAPTYPE_NONE)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        apollomp_rlpp_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rlpp_init */


/* Function Name:
 *      dal_apollomp_rlpp_trap_set
 * Description:
 *      Set RLPP trap to cpu operation, trap or not trap
 * Input:
 *      type - RLPP trap operation type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      Trap the RLPP packet to CPU for software processing
 */
int32
dal_apollomp_rlpp_trapType_set(rtk_rlpp_trapType_t type)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    //RT_INIT_CHK(apollomp_rlpp_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_RLPP_TRAPTYPE_END <= type), RT_ERR_INPUT);

    val = type;
    if ((ret = reg_field_write(APOLLOMP_RLPP_CTRLr, APOLLOMP_TRAP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_rlpp_trapType_set */


/* Function Name:
 *      dal_apollomp_rlpp_trap_get
 * Description:
 *      Get RLPP trap to cpu operation, trap or not trap
 * Input:
 *      pType - RLPP trap operation type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_rlpp_trapType_get(rtk_rlpp_trapType_t *pType)
{
    int32 ret;
    uint32 val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_RLDP),"%s",__FUNCTION__);

    /* check Init status */
    //RT_INIT_CHK(apollomp_rlpp_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pType), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_RLPP_CTRLr, APOLLOMP_TRAP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_RLDP), "");
        return ret;
    }
    *pType = val;

    return RT_ERR_OK;
} /* end of dal_apollomp_rlpp_trapType_get */


