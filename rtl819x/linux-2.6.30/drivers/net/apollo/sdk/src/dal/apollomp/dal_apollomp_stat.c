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
 * $Revision: 11766 $
 * $Date: 2010-08-05 17:45:25 +0800 (?��??? 05 ?��? 2010) $
 *
 * Purpose : Definition of Statistic API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Statistic Counter Reset
 *           (2) Statistic Counter Get
 *
 */



/*
 * Include Files
 */
#include <rtk/port.h>
#include <rtk/stat.h>
#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_stat.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
static uint32    stat_init = INIT_NOT_COMPLETED;

/*
 * Function Declaration
 */

/* Module Name : STAT */

/* Function Name:
 *      dal_apollomp_stat_init
 * Description:
 *      Initialize stat module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_PORT_CNTR_FAIL   - Could not retrieve/reset Port Counter
 * Note:
 *      Must initialize stat module before calling any stat APIs.
 */
int32
dal_apollomp_stat_init(void)
{
    uint32 index;
    uint32 value;
    rtk_enable_t enable;
	rtk_port_t port;
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /*Check if ASIC is still reseting MIB or not*/
    if ((ret = reg_field_read(APOLLOMP_STAT_RSTr, APOLLOMP_RST_STATf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if (value != 0)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return RT_ERR_BUSYWAIT_TIMEOUT;
    }

	/*Set ACL counter to reset*/
    enable = ENABLED;
    for (index = 0; index < HAL_MAX_NUM_OF_LOG_MIB(); index++)
    {
        if ((ret = reg_array_field_write(APOLLOMP_STAT_ACL_CNT_RSTr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }
    }

	/*Set Port counter to reset*/
	value = 1;
    HAL_SCAN_ALL_PORT(port)
    {
	    if ((ret = reg_array_field_write(APOLLOMP_STAT_PORT_RSTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RST_PORT_MIBf, &value)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
	        return ret;
	    }
	}

	/*Set Global counter to reset*/
	value = 1;
    if ((ret = reg_field_write(APOLLOMP_STAT_RSTr, APOLLOMP_RST_GLOBAL_MIBf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }


    /*Reset command*/
	value = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr, APOLLOMP_RST_CMDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    stat_init = INIT_COMPLETED;

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_init */


/* Function Name:
 *      dal_apollomp_stat_global_reset
 * Description:
 *      Reset the global counters.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED

 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL - Could not retrieve/reset Global Counter
 * Note:
 *      None
 */
int32
dal_apollomp_stat_global_reset(void)
{
    int32 ret;
    uint32 index;
    uint32 value;
    rtk_enable_t enable;
	rtk_port_t port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /*Check if ASIC is still reseting MIB or not*/
    if ((ret = reg_field_read(APOLLOMP_STAT_RSTr, APOLLOMP_RST_STATf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if (value != 0)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return RT_ERR_BUSYWAIT_TIMEOUT;
    }

	/*Set ACL counter to reset*/
    enable = ENABLED;
    for (index = 0; index < HAL_MAX_NUM_OF_LOG_MIB(); index++)
    {
        if ((ret = reg_array_field_write(APOLLOMP_STAT_ACL_CNT_RSTr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }
    }

	/*Set Port counter to reset*/
	value = 1;
    HAL_SCAN_ALL_PORT(port)
    {
	    if ((ret = reg_array_field_write(APOLLOMP_STAT_PORT_RSTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RST_PORT_MIBf, &value)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
	        return ret;
	    }
	}

	/*Set Global counter to reset*/
	value = 1;
    if ((ret = reg_field_write(APOLLOMP_STAT_RSTr, APOLLOMP_RST_GLOBAL_MIBf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }


    /*Reset command*/
	value = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr, APOLLOMP_RST_CMDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_global_reset */


/* Function Name:
 *      dal_apollomp_stat_port_reset
 * Description:
 *      Reset the specified port counters in the specified device.
 * Input:
 *      port - port id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
dal_apollomp_stat_port_reset(rtk_port_t port)
{
    int32 ret;
    uint32 value;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    /*Check if ASIC is still reseting MIB or not*/
    if ((ret = reg_field_read(APOLLOMP_STAT_RSTr, APOLLOMP_RST_STATf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }
    if (value != 0)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return RT_ERR_BUSYWAIT_TIMEOUT;
    }

	/*Set Port counter to reset*/
	value = 1;
    if ((ret = reg_array_field_write(APOLLOMP_STAT_PORT_RSTr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_RST_PORT_MIBf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    /*Reset command*/
	value = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr, APOLLOMP_RST_CMDf, &value)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_port_reset */

/* Function Name:
 *      dal_apollomp_stat_log_reset
 * Description:
 *      Reset the specified ACL logging counters.
 * Input:
 *      index - logging index
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_stat_log_reset(uint32 index)
{
    int32 ret;
    uint32 val;
    rtk_enable_t enable;
    uint32 mode;
    uint32 logIndex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);

    /*Check if ASIC is still reseting MIB or not*/
    if ((ret = reg_field_read(APOLLOMP_STAT_RSTr, APOLLOMP_RST_STATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    if (val != 0)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return RT_ERR_BUSYWAIT_TIMEOUT;
    }

	/*Set ACL counter to reset*/
    enable = ENABLED;
    if((index & 1) == 0)
    {
        logIndex = index / 2;
        if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_CNT_MODEr, REG_ARRAY_INDEX_NONE, logIndex, APOLLOMP_MODEf, &mode)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }

        if(mode == STAT_LOG_MODE_64BITS)
        {
            if ((ret = reg_array_field_write(APOLLOMP_STAT_ACL_CNT_RSTr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }

            if ((ret = reg_array_field_write(APOLLOMP_STAT_ACL_CNT_RSTr, REG_ARRAY_INDEX_NONE, (index + 1), APOLLOMP_ENf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
        }
        else
        {
            if ((ret = reg_array_field_write(APOLLOMP_STAT_ACL_CNT_RSTr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }
        }
    }
    else
    {
        if ((ret = reg_array_field_write(APOLLOMP_STAT_ACL_CNT_RSTr, REG_ARRAY_INDEX_NONE, index, APOLLOMP_ENf, &enable)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }
    }

    /*Reset command*/
	val = 1;
    if ((ret = reg_field_write(APOLLOMP_EPON_STAT_RSTr, APOLLOMP_RST_CMDf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_log_reset */

/* Function Name:
 *      dal_apollomp_stat_rst_cnt_value_set
 * Description:
 *      Set the counter value after reset
 * Input:
 *      None
 * Output:
 *      rstValue  - the counter value after reset
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_stat_rst_cnt_value_set(rtk_mib_rst_value_t rstValue)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((STAT_MIB_RST_END <= rstValue), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_STAT_RSTr, APOLLOMP_RST_MIB_VALf, &rstValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_rst_cnt_value_set */

/* Function Name:
 *      dal_apollomp_stat_rst_cnt_value_get
 * Description:
 *      Get the counter value after reset
 * Input:
 *      None
 * Output:
 *      pRstValue  - pointer buffer of value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_stat_rst_cnt_value_get(rtk_mib_rst_value_t *pRstValue)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRstValue), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_STAT_RSTr, APOLLOMP_RST_MIB_VALf, pRstValue)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_rst_cnt_value_get */

/* Function Name:
 *      dal_apollomp_stat_global_get
 * Description:
 *      Get one specified global counter in the specified device.
 * Input:
 *      cntrIdx - specified global counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID                  - invalid unit id
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
int32
dal_apollomp_stat_global_get(rtk_stat_global_type_t cntrIdx, uint64 *pCntr)
{
    int32 ret;
    uint32 cntr;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((MIB_GLOBAL_CNTR_END <= cntrIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

    switch(cntrIdx)
    {
        case DOT1D_TP_LEARNED_ENTRY_DISCARDS_INDEX:
			if ((ret = reg_read(APOLLOMP_STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDSr, &cntr)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
		        return ret;
		    }
            break;
        default:
            return RT_ERR_INPUT;;
    }

    *pCntr = cntr;

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_global_get */


/* Function Name:
 *      dal_apollomp_stat_global_getAll
 * Description:
 *      Get all global counters in the specified device.
 * Input:
 *      None
 * Output:
 *      pGlobalCntrs - pointer buffer of global counter structure
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 *      RT_ERR_STAT_GLOBAL_CNTR_FAIL    - Could not retrieve/reset Global Counter
 *      RT_ERR_STAT_INVALID_GLOBAL_CNTR - Invalid Global Counter
 * Note:
 *      None
 */
int32
dal_apollomp_stat_global_getAll(rtk_stat_global_cntr_t *pGlobalCntrs)
{
    int32   ret;
    uint32 cntr;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pGlobalCntrs), RT_ERR_NULL_POINTER);

    if ((ret = reg_read(APOLLOMP_STAT_BRIDGE_DOT1DTPLEARNEDENTRYDISCARDSr, &cntr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    pGlobalCntrs->dot1dTpLearnedEntryDiscards = cntr;

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_global_getAll */


/* Function Name:
 *      dal_apollomp_stat_port_get
 * Description:
 *      Get one specified port counter.
 * Input:
 *      port     - port id
 *      cntrIdx - specified port counter index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
dal_apollomp_stat_port_get(rtk_port_t port, rtk_stat_port_type_t cntrIdx, uint64 *pCntr)
{
    int32  ret;
    uint32 cntr_64Flag;
    uint32 cntr;
    uint32 reg;
    uint32 fieldL;
    uint32 fieldH;
    uint64 cntr64;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_INPUT);
    RT_PARAM_CHK((MIB_PORT_CNTR_END <= cntrIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);

	cntr_64Flag = FALSE;
    switch(cntrIdx)
    {
    	case DOT1D_TP_PORT_IN_DISCARDS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_DOT1DTPPORTINDISCARDSf;

            break;
    	case IF_IN_OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldH = APOLLOMP_IFINOCTETS_Hf;
			fieldL = APOLLOMP_IFINOCTETS_Lf;
			cntr_64Flag = TRUE;

            break;
    	case IF_IN_UCAST_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_IFINUCASTPKTSf;

            break;
    	case IF_IN_MULTICAST_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_IFINMULTICASTPKTSf;

            break;
    	case IF_IN_BROADCAST_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_IFINBROADCASTPKTSf;

            break;
    	case IF_OUT_OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_IFOUTOCTETS_Lf;
			fieldH = APOLLOMP_IFOUTOCTETS_Hf;
			cntr_64Flag = TRUE;

            break;

        case IF_OUT_UCAST_PKTS_CNT_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_IFOUTUCASTPKTSf;

            break;
        case IF_OUT_MULTICAST_PKTS_CNT_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_IFOUTMULTICASTPKTSf;

            break;
        case IF_OUT_BROADCAST_PKTS_CNT_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_IFOUTBROADCASTPKTSf;

            break;
        case IF_OUT_DISCARDS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_IFOUTDISCARDSf;

            break;
        case DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_DOT3STATSSINGLECOLLISIONFRAMESf;

            break;
        case DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_DOT3STATSMULTIPLECOLLISIONFRAMESf;

            break;
        case DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_DOT3STATSDEFERREDTRANSMISSIONSf;

            break;
        case DOT3_STATS_LATE_COLLISIONS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_DOT3STATSLATECOLLISIONSf;

            break;
        case DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_DOT3STATSEXCESSIVECOLLISIONSf;

           	break;
        case DOT3_STATS_SYMBOL_ERRORS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_DOT3STATSSYMBOLERRORSf;

           	break;
        case DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_DOT3CONTROLINUNKNOWNOPCODESf;

           	break;
        case DOT3_IN_PAUSE_FRAMES_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_DOT3INPAUSEFRAMESf;

           	break;
        case DOT3_OUT_PAUSE_FRAMES_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_DOT3OUTPAUSEFRAMESf;

           	break;
        case ETHER_STATS_DROP_EVENTS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_ETHERSTATSDROPEVENTSf;

           	break;
        case ETHER_STATS_TX_BROADCAST_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSBROADCASTPKTSf;

           	break;

        case ETHER_STATS_TX_MULTICAST_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSMULTICASTPKTSf;

           	break;
        case ETHER_STATS_CRC_ALIGN_ERRORS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_ETHERSTATSCRCALIGNERRORSf;

           	break;

        case ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSUNDERSIZEPKTSf;

           	break;
        case ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSUNDERSIZEPKTSf;

           	break;
        case ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSUNDERSIZEDROPPKTSf;

           	break;

        case ETHER_STATS_TX_OVERSIZE_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSOVERSIZEPKTSf;

           	break;

        case ETHER_STATS_RX_OVERSIZE_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSOVERSIZEPKTSf;

           	break;
        case ETHER_STATS_FRAGMENTS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_ETHERSTATSFRAGMENTSf;

           	break;
        case ETHER_STATS_JABBERS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_ETHERSTATSJABBERSf;

           	break;
        case ETHER_STATS_COLLISIONS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_ETHERSTATSCOLLISIONSf;

           	break;

        case ETHER_STATS_TX_PKTS_64OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSPKTS64OCTETSf;

           	break;
        case ETHER_STATS_RX_PKTS_64OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSPKTS64OCTETSf;

           	break;
        case ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSPKTS65TO127OCTETSf;

           	break;
        case ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSPKTS65TO127OCTETSf;

           	break;
        case ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSPKTS128TO255OCTETSf;

           	break;
        case ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSPKTS128TO255OCTETSf;

           	break;
        case ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSPKTS256TO511OCTETSf;

           	break;
        case ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSPKTS256TO511OCTETSf;

           	break;
        case ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSPKTS512TO1023OCTETSf;

           	break;
        case ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSPKTS512TO1023OCTETSf;

           	break;
        case ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSPKTS1024TO1518OCTETSf;

           	break;
        case ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSPKTS1024TO1518OCTETSf;

           	break;
        case ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_TX_MIBr;
			fieldL = APOLLOMP_TX_ETHERSTATSPKTS1519TOMAXOCTETSf;

           	break;
        case ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX:
			reg = APOLLOMP_STAT_PORT_RX_MIBr;
			fieldL = APOLLOMP_RX_ETHERSTATSPKTS1519TOMAXOCTETSf;

           	break;
        case IN_OAM_PDU_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_OAM_MIBr;
			fieldL = APOLLOMP_INOAMPDUPKTSf;

           	break;
        case OUT_OAM_PDU_PKTS_INDEX:
			reg = APOLLOMP_STAT_PORT_OAM_MIBr;
			fieldL = APOLLOMP_OUTOAMPDUPKTSf;

           	break;
        default:
            return RT_ERR_INPUT;;
    }


	if(cntr_64Flag)
    {
	    if ((ret = reg_array_field_read(reg, port, REG_ARRAY_INDEX_NONE, fieldH, &cntr)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
	        return ret;
	    }

		cntr64 = cntr;
	}
	else
		cntr64 = 0;


	if ((ret = reg_array_field_read(reg, port, REG_ARRAY_INDEX_NONE, fieldL, &cntr)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

	cntr64 = (cntr64 << 32) | cntr;

	*pCntr = cntr64;

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_port_get */


/* Function Name:
 *      dal_apollomp_stat_port_getAll
 * Description:
 *      Get all counters of one specified port in the specified device.
 * Input:
 *      port        - port id
 * Output:
 *      pPortCntrs - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID             - invalid port id
 *      RT_ERR_NULL_POINTER        - input parameter may be null pointer
 *      RT_ERR_STAT_PORT_CNTR_FAIL - Could not retrieve/reset Port Counter
 * Note:
 *      None
 */
int32
dal_apollomp_stat_port_getAll(rtk_port_t port, rtk_stat_port_cntr_t *pPortCntrs)
{
    uint64 data;
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_GET_PORTNUM() <= port), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPortCntrs), RT_ERR_NULL_POINTER);

    osal_memset(pPortCntrs, 0, sizeof(rtk_stat_port_cntr_t));

    if(dal_apollomp_stat_port_get(port, IF_IN_OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifInOctets = data;
    if(dal_apollomp_stat_port_get(port, IF_IN_UCAST_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifInUcastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, IF_IN_MULTICAST_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifInMulticastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, IF_IN_BROADCAST_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifInBroadcastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, IF_IN_DISCARDS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifInDiscards = (uint32)data;
    if(dal_apollomp_stat_port_get(port, IF_OUT_OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifOutOctets = data;
    if(dal_apollomp_stat_port_get(port, IF_OUT_DISCARDS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifOutDiscards = (uint32)data;
    if(dal_apollomp_stat_port_get(port, IF_OUT_UCAST_PKTS_CNT_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifOutUcastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, IF_OUT_MULTICAST_PKTS_CNT_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifOutMulticastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, IF_OUT_BROADCAST_PKTS_CNT_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->ifOutBrocastPkts = (uint32)data;

    if(dal_apollomp_stat_port_get(port, DOT1D_BASE_PORT_DELAY_EXCEEDED_DISCARDS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot1dBasePortDelayExceededDiscards = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT1D_TP_PORT_IN_DISCARDS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot1dTpPortInDiscards = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT1D_TP_HC_PORT_IN_DISCARDS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot1dTpHcPortInDiscards = (uint32)data;

    if(dal_apollomp_stat_port_get(port, DOT3_IN_PAUSE_FRAMES_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3InPauseFrames = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_OUT_PAUSE_FRAMES_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3OutPauseFrames = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_OUT_PAUSE_ON_FRAMES_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3OutPauseOnFrames = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_ALIGNMENT_ERRORS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsAligmentErrors = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_FCS_ERRORS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsFCSErrors = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_SINGLE_COLLISION_FRAMES_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsSingleCollisionFrames = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_MULTIPLE_COLLISION_FRAMES_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsMultipleCollisionFrames = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_DEFERRED_TRANSMISSIONS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsDeferredTransmissions = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_LATE_COLLISIONS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsLateCollisions = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_EXCESSIVE_COLLISIONS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsExcessiveCollisions = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_FRAME_TOO_LONGS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsFrameTooLongs = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_STATS_SYMBOL_ERRORS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3StatsSymbolErrors = (uint32)data;
    if(dal_apollomp_stat_port_get(port, DOT3_CONTROL_IN_UNKNOWN_OPCODES_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->dot3ControlInUnknownOpcodes = (uint32)data;

    if(dal_apollomp_stat_port_get(port, ETHER_STATS_DROP_EVENTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsDropEvents = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_FRAGMENTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsFragments = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_JABBERS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsJabbers = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_COLLISIONS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsCollisions = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_CRC_ALIGN_ERRORS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsCRCAlignErrors = (uint32)data;

    if(dal_apollomp_stat_port_get(port, ETHER_STATS_OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsOctets = data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_BROADCAST_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsBcastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_MULTICAST_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsMcastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_UNDER_SIZE_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsUndersizePkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_OVERSIZE_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsOversizePkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_PKTS_64OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsPkts64Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_PKTS_65TO127OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsPkts65to127Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_PKTS_128TO255OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsPkts128to255Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_PKTS_256TO511OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsPkts256to511Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_PKTS_512TO1023OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsPkts512to1023Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_PKTS_1024TO1518OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsPkts1024to1518Octets = (uint32)data;

    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxOctets = data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_UNDER_SIZE_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxUndersizePkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_OVERSIZE_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxOversizePkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_PKTS_64OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxPkts64Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_PKTS_65TO127OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxPkts65to127Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_PKTS_128TO255OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxPkts128to255Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_PKTS_256TO511OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxPkts256to511Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_PKTS_512TO1023OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxPkts512to1023Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_PKTS_1024TO1518OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxPkts1024to1518Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_PKTS_1519TOMAXOCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxPkts1519toMaxOctets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_BROADCAST_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxBcastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_MULTICAST_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxMcastPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_FRAGMENTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxFragments = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_JABBERS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxJabbers = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_TX_CRC_ALIGN_ERROR_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsTxCRCAlignErrors = (uint32)data;

    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_UNDER_SIZE_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxUndersizePkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_UNDER_SIZE_DROP_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxUndersizeDropPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_OVERSIZE_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxOversizePkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_PKTS_64OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxPkts64Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_PKTS_65TO127OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxPkts65to127Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_PKTS_128TO255OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxPkts128to255Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_PKTS_256TO511OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxPkts256to511Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_PKTS_512TO1023OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxPkts512to1023Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_PKTS_1024TO1518OCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxPkts1024to1518Octets = (uint32)data;
    if(dal_apollomp_stat_port_get(port, ETHER_STATS_RX_PKTS_1519TOMAXOCTETS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->etherStatsRxPkts1519toMaxOctets = (uint32)data;

    if(dal_apollomp_stat_port_get(port, IN_OAM_PDU_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->inOampduPkts = (uint32)data;
    if(dal_apollomp_stat_port_get(port, OUT_OAM_PDU_PKTS_INDEX, &data) != RT_ERR_OK)
        data = 0;
    pPortCntrs->outOampduPkts = (uint32)data;

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_port_getAll */

/* Function Name:
 *      dal_apollomp_stat_log_get
 * Description:
 *      Get ACL logging counter.
 * Input:
 *      cntrIdx  - logging index
 * Output:
 *      pCntr    - pointer buffer of counter value
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT                    - invalid index
 *      RT_ERR_NULL_POINTER             - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_stat_log_get(uint32 index, uint64 *pCntr)
{
    int32   ret;
    uint32  val;
    uint32  valHigh;
    uint64  mibCounter;
    uint32  logIndex;
    uint32  mode;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);
    RT_PARAM_CHK((NULL == pCntr), RT_ERR_NULL_POINTER);


    if((index & 1) == 0)
    {
        logIndex = index / 2;
        if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_CNT_MODEr, REG_ARRAY_INDEX_NONE, logIndex, APOLLOMP_MODEf, &mode)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }

        if(mode == STAT_LOG_MODE_64BITS)
        {
            if ((ret = reg_array_read(APOLLOMP_STAT_ACL_CNTr, REG_ARRAY_INDEX_NONE, index, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }

            if ((ret = reg_array_read(APOLLOMP_STAT_ACL_CNTr, REG_ARRAY_INDEX_NONE, (index + 1), &valHigh)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }

            mibCounter = valHigh;
            mibCounter = (mibCounter << 32) | val;
        }
        else
        {
            if ((ret = reg_array_read(APOLLOMP_STAT_ACL_CNTr, REG_ARRAY_INDEX_NONE, index, &val)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
                return ret;
            }

            mibCounter = val;
        }
    }
    else
    {
        if ((ret = reg_array_read(APOLLOMP_STAT_ACL_CNTr, REG_ARRAY_INDEX_NONE, index, &val)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }

        mibCounter = val;
    }

    *pCntr = mibCounter;

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_log_get */

/* Function Name:
 *      dal_apollomp_stat_log_ctrl_set
 * Description:
 *      Set the acl log counters mode for 32-bits or 64-bits counter, and
 *      set the acl log counters type for packet or byte counter
 * Input:
 *      index 		- index of ACL log counter
 *      ctrl 		- log counter control setting
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_stat_log_ctrl_set(uint32 index, rtk_stat_log_ctrl_t ctrl)
{
    int32   ret;
    uint32  val;
    uint32  logIndex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);
    RT_PARAM_CHK((STAT_LOG_MODE_END <= ctrl.mode), RT_ERR_INPUT);
    RT_PARAM_CHK((STAT_LOG_TYPE_END <= ctrl.type), RT_ERR_INPUT);
    RT_PARAM_CHK((index&1), RT_ERR_INPUT);

    logIndex = index / 2;
    val = (uint32)ctrl.type;
    if ((ret = reg_array_field_write(APOLLOMP_STAT_ACL_CNT_TYPEr, REG_ARRAY_INDEX_NONE, logIndex, APOLLOMP_TYPEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    val = (uint32)ctrl.mode;
    if ((ret = reg_array_field_write(APOLLOMP_STAT_ACL_CNT_MODEr, REG_ARRAY_INDEX_NONE, logIndex, APOLLOMP_MODEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_log_ctrl_set */

/* Function Name:
 *      dal_apollomp_stat_log_ctrl_get
 * Description:
 *      Get the acl counters mode for 32-bits or 64-bits counter, and
 *      get the acl log counters type for packet or byte counter
 * Input:
 *      index 		- index of ACL log counter
 * Output:
 *      pCtrl 		- log counter control setting
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_stat_log_ctrl_get(uint32 index, rtk_stat_log_ctrl_t *pCtrl)
{
    int32   ret;
    uint32  val;
    uint32  logIndex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((index >= HAL_MAX_NUM_OF_LOG_MIB()), RT_ERR_FILTER_LOG_ID);
    RT_PARAM_CHK((NULL == pCtrl), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((index&1), RT_ERR_INPUT);

    logIndex = index / 2;
    if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_CNT_TYPEr, REG_ARRAY_INDEX_NONE, logIndex, APOLLOMP_TYPEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    pCtrl->type = (rtk_stat_logCnt_type_t)val;

    if ((ret = reg_array_field_read(APOLLOMP_STAT_ACL_CNT_MODEr, REG_ARRAY_INDEX_NONE, logIndex, APOLLOMP_MODEf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    pCtrl->mode = (rtk_stat_logCnt_mode_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_log_ctrl_get */

/* Function Name:
 *      dal_apollomp_stat_mib_cnt_mode_get
 * Description:
 *      Get the MIB data update mode
 * Input:
 *      None
 * Output:
 *      pCnt_mode   - pointer buffer of MIB data update mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_stat_mib_cnt_mode_get(rtk_mib_count_mode_t *pCnt_mode)
{
    int32 ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pCnt_mode), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_STAT_CTRLr, APOLLOMP_CNTING_MODEf, pCnt_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_mib_cnt_mode_get */

/* Function Name:
 *      dal_apollomp_stat_mib_cnt_mode_set
 * Description:
 *      Set MIB data update mode
 * Input:
 *      cnt_mode        - MIB counter update mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_stat_mib_cnt_mode_set(rtk_mib_count_mode_t cnt_mode)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((STAT_MIB_COUNT_MODE_END <= cnt_mode), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_STAT_CTRLr, APOLLOMP_CNTING_MODEf, &cnt_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_mib_cnt_mode_set */

/* Function Name:
 *      dal_apollomp_stat_mib_latch_timer_get
 * Description:
 *      Get the MIB latch timer
 * Input:
 *      None
 * Output:
 *      pTimer   - pointer buffer of MIB latch timer
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_stat_mib_latch_timer_get(uint32 *pTimer)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pTimer), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_STAT_CTRLr, APOLLOMP_LATCH_TIMERf, pTimer)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_mib_latch_timer_get */

/* Function Name:
 *      dal_apollomp_stat_mib_latch_timer_set
 * Description:
 *      Set MIB data update mode
 * Input:
 *      timer        - MIB latch timer
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_stat_mib_latch_timer_set(uint32 timer)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_GET_MIB_LATCH_TIMER_MAX() < timer), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_STAT_CTRLr, APOLLOMP_LATCH_TIMERf, &timer)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_mib_latch_timer_set */

/* Function Name:
 *      dal_apollomp_stat_mib_sync_mode_get
 * Description:
 *      Get the MIB register data update mode
 * Input:
 *      None
 * Output:
 *      pSync_mode   - pointer buffer of MIB register data update mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_stat_mib_sync_mode_get(rtk_mib_sync_mode_t *pSync_mode)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pSync_mode), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_STAT_CTRLr, APOLLOMP_SYNC_MODEf, pSync_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_mib_sync_mode_get */

/* Function Name:
 *      dal_apollomp_stat_mib_sync_mode_set
 * Description:
 *      Set MIB register data update mode
 * Input:
 *      sync_mode        - MIB register data update mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_stat_mib_sync_mode_set(rtk_mib_sync_mode_t sync_mode)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((STAT_MIB_SYNC_MODE_END <= sync_mode), RT_ERR_INPUT);

    if ((ret = reg_field_write(APOLLOMP_STAT_CTRLr, APOLLOMP_SYNC_MODEf, &sync_mode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_mib_sync_mode_set */

/* Function Name:
 *      dal_apollomp_stat_mib_count_tag_length_get
 * Description:
 *      Get counting Tag length state in tx/rx packet
 * Input:
 *      direction - count tx or rx tag length
 * Output:
 *      pState   - pointer buffer of count tx/rx tag length state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_stat_mib_count_tag_length_get(rtk_mib_tag_cnt_dir_t direction, rtk_mib_tag_cnt_state_t *pState)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((STAT_MIB_TAG_CNT_DIR_END <= direction), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pState), RT_ERR_NULL_POINTER);

    if( STAT_MIB_TAG_CNT_DIR_TX == direction)
    {
        if ((ret = reg_field_read(APOLLOMP_STAT_CTRLr, APOLLOMP_TX_CNT_CTAGf, pState)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_read(APOLLOMP_STAT_CTRLr, APOLLOMP_RX_CNT_CTAGf, pState)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_mib_count_tag_length_get */

/* Function Name:
 *      dal_apollomp_stat_mib_count_tag_length_set
 * Description:
 *      Set counting length including Ctag length or excluding Ctag length for tx/rx packet
 * Input:
 *      direction - count tx or rx tag length
 *      enable    - count tag length state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_stat_mib_count_tag_length_set(rtk_mib_tag_cnt_dir_t direction, rtk_mib_tag_cnt_state_t state)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK((STAT_MIB_TAG_CNT_DIR_END <= direction), RT_ERR_INPUT);
    RT_PARAM_CHK((STAT_MIB_TAG_CNT_STATE_END <= state), RT_ERR_INPUT);

    if( STAT_MIB_TAG_CNT_DIR_TX == direction)
    {
        if ((ret = reg_field_write(APOLLOMP_STAT_CTRLr, APOLLOMP_TX_CNT_CTAGf, &state)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }
    }
    else
    {
        if ((ret = reg_field_write(APOLLOMP_STAT_CTRLr, APOLLOMP_RX_CNT_CTAGf, &state)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_mib_count_tag_length_set */

/* Function Name:
 *      dal_apollomp_stat_pktInfo_get
 * Description:
 *      Get the newest packet trap/drop reason
 * Input:
 *      port - port index
 * Output:
 *      pCode   - the newest packet trap/drop reason code
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID
 * Note:
 *      None
 */
int32
dal_apollomp_stat_pktInfo_get(rtk_port_t port, uint32 *pCode)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_STAT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(stat_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pCode), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_STAT_PRIVATE_REASONr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_PKT_INFOf, pCode)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_STAT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_stat_pktInfo_get */

