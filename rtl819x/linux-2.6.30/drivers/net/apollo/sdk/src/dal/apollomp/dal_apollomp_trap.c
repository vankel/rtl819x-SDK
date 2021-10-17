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
 * $Revision: 16212 $
 * $Date: 2011-03-10 10:39:02 +0800 (?��??? 10 三�? 2011) $
 *
 * Purpose : Definition of TRAP API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration for traping packet to CPU
 *           (2) RMA
 *           (3) User defined RMA
 *           (4) System-wise management frame
 *           (5) System-wise user defined management frame
 *           (6) Per port user defined management frame
 *           (7) Packet with special flag or option
 *           (8) CFM and OAM packet
 *
 */



/*
 * Include Files
 */
#include <rtk/trap.h>

#include <dal/apollomp/dal_apollomp.h>
#include <dal/apollomp/dal_apollomp_trap.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
static uint32    apollomp_trap_init = INIT_NOT_COMPLETED;

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_apollomp_trap_init
 * Description:
 *      Initial the trap module of the specified device..
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32
dal_apollomp_trap_init(void)
{
    int32 ret;
    rtk_mac_t rmaMac;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    apollomp_trap_init = INIT_COMPLETED;

    if((ret = dal_apollomp_trap_reasonTrapToCpuPriority_set(TRAP_REASON_RMA, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trap_reasonTrapToCpuPriority_set(TRAP_REASON_MULTICASTDLF, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trap_reasonTrapToCpuPriority_set(TRAP_REASON_1XUNAUTH, 0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trap_igmpCtrlPkt2CpuEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trap_mldCtrlPkt2CpuEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trap_ipMcastPkt2CpuEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trap_l2McastPkt2CpuEnable_set(DISABLED)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trap_rmaPri_set(0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x80;
    rmaMac.octet[2] = 0xC2;
    rmaMac.octet[3] = 0x00;
    rmaMac.octet[4] = 0x00;
    for(rmaMac.octet[5] = 0; rmaMac.octet[5] <= 0x2F; rmaMac.octet[5]++)
    {
        if((ret = dal_apollomp_trap_rmaAction_set(&rmaMac, ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_trap_rmaVlanCheckEnable_set(&rmaMac, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_trap_rmaPortIsolationEnable_set(&rmaMac, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_trap_rmaStormControlEnable_set(&rmaMac, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_trap_rmaKeepCtagEnable_set(&rmaMac, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    rmaMac.octet[0] = 0x01;
    rmaMac.octet[1] = 0x00;
    rmaMac.octet[2] = 0x0C;
    rmaMac.octet[3] = 0xCC;
    rmaMac.octet[4] = 0xCC;
    for(rmaMac.octet[5] = 0xCC; rmaMac.octet[5] <= 0xCD; rmaMac.octet[5]++)
    {
        if((ret = dal_apollomp_trap_rmaAction_set(&rmaMac, ACTION_FORWARD)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_trap_rmaVlanCheckEnable_set(&rmaMac, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_trap_rmaPortIsolationEnable_set(&rmaMac, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_trap_rmaStormControlEnable_set(&rmaMac, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_trap_rmaKeepCtagEnable_set(&rmaMac, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
            apollomp_trap_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if((ret = dal_apollomp_trap_oamPduAction_set(ACTION_FORWARD)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_trap_oamPduPri_set(0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        apollomp_trap_init = INIT_NOT_COMPLETED;
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_init */

/* Module Name    : Trap                                    */
/* Sub-module Name: Configuration for traping packet to CPU */

/* Function Name:
 *      dal_apollomp_trap_reasonTrapToCpuPriority_get
 * Description:
 *      Get priority value of a packet that trapped to CPU port according to specific reason.
 * Input:
 *      type      - reason that trap to CPU port.
 * Output:
 *      pPriority - configured internal priority for such reason.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_IPV4IGMP
 *      - TRAP_REASON_IPV6MLD
 *      - TRAP_REASON_1XEAPOL
 *      - TRAP_REASON_VLANERR
 *      - TRAP_REASON_SLPCHANGE
 *      - TRAP_REASON_MULTICASTDLF
 *      - TRAP_REASON_CFI
 *      - TRAP_REASON_1XUNAUTH
 */
int32
dal_apollomp_trap_reasonTrapToCpuPriority_get(rtk_trap_reason_type_t type, rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  pri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((TRAP_REASON_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    switch (type)
    {
        case TRAP_REASON_RMA:
		    if ((ret = reg_field_read(APOLLOMP_RMA_CFGr, APOLLOMP_RMA_TRAP_PRIORITYf, &pri)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
		        return ret;
		    }
			break;

        case TRAP_REASON_MULTICASTDLF:
		    if ((ret = reg_field_read(APOLLOMP_UNKN_MC_PRIr, APOLLOMP_UNKN_MC_PRIf, &pri)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		        return ret;
		    }
            break;

        case TRAP_REASON_1XUNAUTH:
		    if ((ret = reg_field_read(APOLLOMP_DOT1X_CFG_0r, APOLLOMP_DOT1X_PRIORTYf, &pri)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
		        return ret;
		    }
            break;
        case TRAP_REASON_IPV4IGMP:
        case TRAP_REASON_IPV6MLD:
            if ((ret = reg_field_read(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_TRAP_PRIORITYf, &pri)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
		        return ret;
		    }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    *pPriority = (rtk_pri_t)pri;
    return RT_ERR_OK;
} /* end of dal_apollomp_trap_reasonTrapToCpuPriority_get */

/* Function Name:
 *      dal_apollomp_trap_reasonTrapToCpuPriority_set
 * Description:
 *      Set priority value of a packet that trapped to CPU port according to specific reason.
 * Input:
 *      type     - reason that trap to CPU port.
 *      priority - internal priority that is going to be set for specific trap reason.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_IPV4IGMP
 *      - TRAP_REASON_IPV6MLD
 *      - TRAP_REASON_1XEAPOL
 *      - TRAP_REASON_VLANERR
 *      - TRAP_REASON_SLPCHANGE
 *      - TRAP_REASON_MULTICASTDLF
 *      - TRAP_REASON_CFI
 *      - TRAP_REASON_1XUNAUTH
 */
int32
dal_apollomp_trap_reasonTrapToCpuPriority_set(rtk_trap_reason_type_t type, rtk_pri_t priority)
{
    int32   ret;
    uint32  pri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((TRAP_REASON_END <= type), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_QOS_INT_PRIORITY);

    pri = (uint32)priority;
    switch (type)
    {
        case TRAP_REASON_RMA:
		    if ((ret = reg_field_write(APOLLOMP_RMA_CFGr, APOLLOMP_RMA_TRAP_PRIORITYf, &pri)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
		        return ret;
		    }
			break;

        case TRAP_REASON_MULTICASTDLF:
		    if ((ret = reg_field_write(APOLLOMP_UNKN_MC_PRIr, APOLLOMP_UNKN_MC_PRIf, &pri)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
		        return ret;
		    }
            break;

        case TRAP_REASON_1XUNAUTH:
		    if ((ret = reg_field_write(APOLLOMP_DOT1X_CFG_0r, APOLLOMP_DOT1X_PRIORTYf, &pri)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
		        return ret;
		    }
            break;
        case TRAP_REASON_IPV4IGMP:
        case TRAP_REASON_IPV6MLD:
            if ((ret = reg_field_write(APOLLOMP_IGMP_GLB_CTRLr, APOLLOMP_TRAP_PRIORITYf, &pri)) != RT_ERR_OK)
		    {
		        RT_ERR(ret, (MOD_DOT1X|MOD_DAL), "");
		        return ret;
		    }
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_trap_reasonTrapToCpuPriority_set */

/* Function Name:
 *      dal_apollomp_trap_igmpCtrlPkt2CpuEnable_get
 * Description:
 *      Get the configuration about whether IGMP control packets need be trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of IGMP control packet trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of IGMP control packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_trap_igmpCtrlPkt2CpuEnable_get(rtk_enable_t *pEnable)
{
    int32  ret;
    uint32 act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_IGMP_P_CTRLr, 0, REG_ARRAY_INDEX_NONE, APOLLOMP_IGMPV1_OPf, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pEnable = ((APOLLOMP_IGMP_ACT_TRAP == act) ? ENABLED : DISABLED);

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_igmpCtrlPkt2CpuEnable_get */

/* Function Name:
 *      dal_apollomp_trap_igmpCtrlPkt2CpuEnable_set
 * Description:
 *      Set the configuration about whether IGMP control packets need be trapped to CPU.
 * Input:
 *      unit   - unit id
 *      enable - status of IGMP control packet trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of IGMP control packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_trap_igmpCtrlPkt2CpuEnable_set(rtk_enable_t enable)
{
    int32  ret;
    uint32 act;
    uint32 port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    act = ((ENABLED == enable) ? APOLLOMP_IGMP_ACT_TRAP : APOLLOMP_IGMP_ACT_FORWARD);
    HAL_SCAN_ALL_PORT(port)
    {
	    if ((ret = reg_array_field_write(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IGMPV1_OPf, &act)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IGMPV2_OPf, &act)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_IGMPV3_OPf, &act)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
	        return ret;
	    }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_igmpCtrlPkt2CpuEnable_set */

/* Function Name:
 *      dal_apollomp_trap_mldCtrlPkt2CpuEnable_get
 * Description:
 *      Get the configuration about whether MLD control packets need be trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of MLD control packet trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of MLD control packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_trap_mldCtrlPkt2CpuEnable_get(rtk_enable_t *pEnable)
{
    int32  ret;
    rtk_action_t act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_IGMP_P_CTRLr, 0, REG_ARRAY_INDEX_NONE, APOLLOMP_MLDV1_OPf, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pEnable = ((APOLLOMP_IGMP_ACT_TRAP == act) ? ENABLED : DISABLED);

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_mldCtrlPkt2CpuEnable_get */

/* Function Name:
 *      dal_apollomp_trap_mldCtrlPkt2CpuEnable_set
 * Description:
 *      Set the configuration about whether MLD control packets need be trapped to CPU.
 * Input:
 *      enable - status of MLD control packet trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of MLD control packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_trap_mldCtrlPkt2CpuEnable_set(rtk_enable_t enable)
{
    int32  ret;
    uint32 act;
    uint32 port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    act = ((ENABLED == enable) ? APOLLOMP_IGMP_ACT_TRAP : APOLLOMP_IGMP_ACT_FORWARD);
    HAL_SCAN_ALL_PORT(port)
    {
	    if ((ret = reg_array_field_write(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MLDV1_OPf, &act)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
	        return ret;
	    }

	    if ((ret = reg_array_field_write(APOLLOMP_IGMP_P_CTRLr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_MLDV2_OPf, &act)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
	        return ret;
	    }
	}

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_mldCtrlPkt2CpuEnable_set */

/* Function Name:
 *      dal_apollomp_trap_ipMcastPkt2CpuEnable_get
 * Description:
 *      Get the configuration about whether IP multicast packet lookup miss need be trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of IP multicast packet trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of IP multicast packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_trap_ipMcastPkt2CpuEnable_get(rtk_enable_t *pEnable)
{

    int32  ret;
    uint32 act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_UNKN_IP4_MCr, 0, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    *pEnable = ((APOLLOMP_L2_COMMON_ACT_TRAP == act) ? ENABLED : DISABLED);

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_ipMcastPkt2CpuEnable_get */

/* Function Name:
 *      dal_apollomp_trap_ipMcastPkt2CpuEnable_set
 * Description:
 *      Set the configuration about whether IP multicast packet lookup miss need be trapped to CPU.
 * Input:
 *      enable - status of IP multicast packet trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of IP multicast packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_trap_ipMcastPkt2CpuEnable_set(rtk_enable_t enable)
{
    int32  ret;
    uint32 act;
    uint32 port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    act = ((ENABLED == enable) ? APOLLOMP_L2_COMMON_ACT_TRAP : APOLLOMP_L2_COMMON_ACT_FORWARD);

	HAL_SCAN_ALL_PORT(port)
    {
	    if ((ret = reg_array_field_write(APOLLOMP_UNKN_IP4_MCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &act)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
	        return ret;
	    }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_ipMcastPkt2CpuEnable_set */

/* Function Name:
 *      dal_apollomp_trap_l2McastPkt2CpuEnable_get
 * Description:
 *      Get the configuration about whether L2 multicast packets lookup miss need be trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of L2 multicast packet trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of L2 multicast packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_trap_l2McastPkt2CpuEnable_get(rtk_enable_t *pEnable)
{
    int32  ret;
    rtk_action_t act;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_UNKN_L2_MCr, 0, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return ret;
    }

    *pEnable = ((APOLLOMP_L2_COMMON_ACT_TRAP == act) ? ENABLED : DISABLED);

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_l2McastPkt2CpuEnable_get */

/* Function Name:
 *      dal_apollomp_trap_l2McastPkt2CpuEnable_set
 * Description:
 *      Set the configuration about whether L2 multicast packets lookup miss need be trapped to CPU.
 * Input:
 *      enable - status of L2 multicast packet trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of L2 multicast packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
int32
dal_apollomp_trap_l2McastPkt2CpuEnable_set(rtk_enable_t enable)
{
    int32 ret;
    uint32 act;
    uint32 port;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    act = ((ENABLED == enable) ? APOLLOMP_L2_COMMON_ACT_TRAP : APOLLOMP_L2_COMMON_ACT_FORWARD);

    HAL_SCAN_ALL_PORT(port)
    {
	    if ((ret = reg_array_field_write(APOLLOMP_UNKN_L2_MCr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ACTf, &act)) != RT_ERR_OK)
	    {
	        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
	        return ret;
	    }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_l2McastPkt2CpuEnable_set */

/* Module Name    : Trap     */
/* Sub-module Name: RMA      */

/* Function Name:
 *      dal_apollomp_trap_rmaAction_get
 * Description:
 *      Get action of reserved multicast address(RMA) frame.
 * Input:
 *      pRmaFrame  - Reserved multicast address.
 * Output:
 *      pRmaAction - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaAction_get(rtk_mac_t *pRmaFrame, rtk_trap_rma_action_t *pRmaAction)
{
    int32   ret;
    uint32  act;
    uint32  regAddr;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRmaAction), RT_ERR_NULL_POINTER);


    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        ( (0xCC == pRmaFrame->octet[5]) || (0xCD == pRmaFrame->octet[5]) ) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
        (0x80 == pRmaFrame->octet[1]) &&
        (0xC2 == pRmaFrame->octet[2]) &&
        (0x00 == pRmaFrame->octet[3]) &&
        (0x00 == pRmaFrame->octet[4]) &&
        (0x2F >= pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((ret = reg_field_read(regAddr, APOLLOMP_OPERATIONf, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    switch (act)
    {
        case APOLLOMP_RMA_ACT_FORWARD:
            *pRmaAction = RMA_ACTION_FORWARD;
            break;
        case APOLLOMP_RMA_ACT_DROP:
            *pRmaAction = RMA_ACTION_DROP;
            break;
        case APOLLOMP_RMA_ACT_TRAP:
            *pRmaAction = RMA_ACTION_TRAP2CPU;
            break;
        default:
            return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaAction_get */

/* Function Name:
 *      dal_apollomp_trap_rmaAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      rmaAction - RMA action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT      - Invalid input parameter
 *      RT_ERR_RMA_ACTION - Invalid RMA action
 * Note:
 *      The supported Reserved Multicast Address frame:
 *      Assignment                                                                  Address
 *      RMA_BRG_GROUP (Bridge Group Address)                                        01-80-C2-00-00-00
 *      RMA_FD_PAUSE (IEEE Std 802.3, 1988 Edition, Full Duplex PAUSE operation)    01-80-C2-00-00-01
 *      RMA_SP_MCAST (IEEE Std 802.3ad Slow Protocols-Multicast address)            01-80-C2-00-00-02
 *      RMA_1X_PAE (IEEE Std 802.1X PAE address)                                    01-80-C2-00-00-03
 *      RMA_RESERVED04 (Reserved)                                                   01-80-C2-00-00-04
 *      RMA_MEDIA_ACCESS_USE (Media Access Method Specific Use)                     01-80-C2-00-00-05
 *      RMA_RESERVED06 (Reserved)                                                   01-80-C2-00-00-06
 *      RMA_RESERVED07 (Reserved)                                                   01-80-C2-00-00-07
 *      RMA_PVD_BRG_GROUP (Provider Bridge Group Address)                           01-80-C2-00-00-08
 *      RMA_RESERVED09 (Reserved)                                                   01-80-C2-00-00-09
 *      RMA_RESERVED0A (Reserved)                                                   01-80-C2-00-00-0A
 *      RMA_RESERVED0B (Reserved)                                                   01-80-C2-00-00-0B
 *      RMA_RESERVED0C (Reserved)                                                   01-80-C2-00-00-0C
 *      RMA_MVRP (Provider Bridge MVRP Address)                                     01-80-C2-00-00-0D
 *      RMA_1ab_LL_DISCOVERY (802.1ab Link Layer Discover Protocol Address)         01-80-C2-00-00-0E
 *      RMA_RESERVED0F (Reserved)                                                   01-80-C2-00-00-0F
 *      RMA_BRG_MNGEMENT (All LANs Bridge Management Group Address)                 01-80-C2-00-00-10
 *      RMA_LOAD_SERV_GENERIC_ADDR (Load Server Generic Address)                    01-80-C2-00-00-11
 *      RMA_LOAD_DEV_GENERIC_ADDR (Loadable Device Generic Address)                 01-80-C2-00-00-12
 *      RMA_RESERVED13 (Reserved)                                                   01-80-C2-00-00-13
 *      RMA_RESERVED14 (Reserved)                                                   01-80-C2-00-00-14
 *      RMA_RESERVED15 (Reserved)                                                   01-80-C2-00-00-15
 *      RMA_RESERVED16 (Reserved)                                                   01-80-C2-00-00-16
 *      RMA_RESERVED17 (Reserved)                                                   01-80-C2-00-00-17
 *      RMA_MANAGER_STA_GENERIC_ADDR (Generic Address for All Manager Stations)     01-80-C2-00-00-18
 *      RMA_RESERVED19 (Reserved)                                                   01-80-C2-00-00-19
 *      RMA_AGENT_STA_GENERIC_ADDR (Generic Address for All Agent Stations)         01-80-C2-00-00-1A
 *      RMA_RESERVED1B (Reserved)                                                   01-80-C2-00-00-1B
 *      RMA_RESERVED1C (Reserved)                                                   01-80-C2-00-00-1C
 *      RMA_RESERVED1D (Reserved)                                                   01-80-C2-00-00-1D
 *      RMA_RESERVED1E (Reserved)                                                   01-80-C2-00-00-1E
 *      RMA_RESERVED1F (Reserved)                                                   01-80-C2-00-00-1F
 *      RMA_GMRP (GMRP Address)                                                     01-80-C2-00-00-20
 *      RMA_GVRP (GVRP address)                                                     01-80-C2-00-00-21
 *      RMA_UNDEF_GARP22~2F (Undefined GARP address)                                01-80-C2-00-00-22
 *                                                                                ~ 01-80-C2-00-00-2F
 *      CDP                                                                         01-00-0C-CC-CC-CC
 *      CDP                                                                         01-00-0C-CC-CC-CD
 *
 *      The supported Reserved Multicast Address action:
 *      - RMA_ACTION_FORWARD
 *      - RMA_ACTION_DROP
 *      - RMA_ACTION_TRAP2CPU
 */
int32
dal_apollomp_trap_rmaAction_set(rtk_mac_t *pRmaFrame, rtk_trap_rma_action_t rmaAction)
{
    int32   ret;
    uint32  act;
    uint32  regAddr;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RMA_ACTION_END <= rmaAction), RT_ERR_INPUT);

    switch (rmaAction)
    {
        case RMA_ACTION_FORWARD:
            act = APOLLOMP_RMA_ACT_FORWARD;
            break;
        case RMA_ACTION_DROP:
            act = APOLLOMP_RMA_ACT_DROP;
            break;
        case RMA_ACTION_TRAP2CPU:
            act = APOLLOMP_RMA_ACT_TRAP;
            break;
        default:
            return RT_ERR_FAILED;
    }

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        ( (0xCC == pRmaFrame->octet[5]) || (0xCD == pRmaFrame->octet[5]) ) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
        (0x80 == pRmaFrame->octet[1]) &&
        (0xC2 == pRmaFrame->octet[2]) &&
        (0x00 == pRmaFrame->octet[3]) &&
        (0x00 == pRmaFrame->octet[4]) &&
        (0x2F >= pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else
        return RT_ERR_CHIP_NOT_SUPPORTED;

    if ((ret = reg_field_write(regAddr, APOLLOMP_OPERATIONf, &act)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaAction_set */

/* Function Name:
 *      dal_apollomp_trap_rmaPri_get
 * Description:
 *      Get priority of packets trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pPriority  - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaPri_get(rtk_pri_t *pPriority)
{
    int32   ret;
    uint32  pri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    if ((ret = reg_field_read(APOLLOMP_RMA_CFGr, APOLLOMP_RMA_TRAP_PRIORITYf, &pri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pPriority = (rtk_pri_t)pri;

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaPri_get */

/* Function Name:
 *      dal_apollomp_trap_rmaPri_set
 * Description:
 *      Set priority of packets trapped to CPU.
 * Input:
 *      priority   - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaPri_set(rtk_pri_t priority)
{
    int32   ret;
    uint32  pri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_QOS_INT_PRIORITY);

    pri = (uint32)priority;

    if ((ret = reg_field_write(APOLLOMP_RMA_CFGr, APOLLOMP_RMA_TRAP_PRIORITYf, &pri)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaPri_set */

/* Function Name:
 *      dal_apollomp_trap_rmaVlanCheckEnable_get
 * Description:
 *      Get enable status of vlan checking on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaVlanCheckEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }

    if ((ret = reg_field_read(regAddr, APOLLOMP_VLAN_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)!val;

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaVlanCheckEnable_get */

/* Function Name:
 *      dal_apollomp_trap_rmaVlanCheckEnable_set
 * Description:
 *      Set enable status of vlan checking on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaVlanCheckEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)!enable;

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }

    if ((ret = reg_field_write(regAddr, APOLLOMP_VLAN_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaVlanCheckEnable_set */

/* Function Name:
 *      dal_apollomp_trap_rmaPortIsolationEnable_get
 * Description:
 *      Get enable status of port isolation on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaPortIsolationEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }

    if ((ret = reg_field_read(regAddr, APOLLOMP_PORTISO_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)!val;

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaPortIsolationEnable_get */

/* Function Name:
 *      dal_apollomp_trap_rmaPortIsolationEnable_set
 * Description:
 *      Set enable status of port isolation on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaPortIsolationEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)!enable;

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }

    if ((ret = reg_field_write(regAddr, APOLLOMP_PORTISO_LEAKYf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaPortIsolationEnable_set */

/* Function Name:
 *      dal_apollomp_trap_rmaStormControlEnable_get
 * Description:
 *      Get enable status of storm control on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaStormControlEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }

    if ((ret = reg_field_read(regAddr, APOLLOMP_DISCARD_STORM_FILTERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)!val;

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaStormControlEnable_get */

/* Function Name:
 *      dal_apollomp_trap_rmaStormControlEnable_set
 * Description:
 *      Set enable status of storm control on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaStormControlEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)!enable;

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }

    if ((ret = reg_field_write(regAddr, APOLLOMP_DISCARD_STORM_FILTERf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaStormControlEnable_set */

/* Function Name:
 *      dal_apollomp_trap_rmaKeepCtagEnable_get
 * Description:
 *      Get enable status of keeping ctag format on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaKeepCtagEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }

    if ((ret = reg_field_read(regAddr, APOLLOMP_KEEP_FORMATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;


    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaKeepCtagEnable_get */

/* Function Name:
 *      dal_apollomp_trap_rmaKeepCtagEnable_set
 * Description:
 *      Set enable status of keeping ctag format on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_trap_rmaKeepCtagEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable)
{
    int32   ret;
    uint32  regAddr;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pRmaFrame), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;

    if( (0x01 == pRmaFrame->octet[0]) &&
        (0x00 == pRmaFrame->octet[1]) &&
        (0x0C == pRmaFrame->octet[2]) &&
        (0xCC == pRmaFrame->octet[3]) &&
        (0xCC == pRmaFrame->octet[4]) &&
        (0xCC == pRmaFrame->octet[5] || 0xCD == pRmaFrame->octet[5]) )
    {
    	switch(pRmaFrame->octet[5])
    	{
    		case 0xCC:
    			regAddr = APOLLOMP_RMA_CTRL_CDPr;
    			break;
    		case 0xCD:
    			regAddr = APOLLOMP_RMA_CTRL_SSTPr;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }
    else if((0x01 == pRmaFrame->octet[0]) &&
            (0x80 == pRmaFrame->octet[1]) &&
            (0xC2 == pRmaFrame->octet[2]) &&
            (0x00 == pRmaFrame->octet[3]) &&
            (0x00 == pRmaFrame->octet[4]) &&
            (0x2F >= pRmaFrame->octet[5]) )
    {

    	switch(pRmaFrame->octet[5])
    	{
    		case 0x00:
    			regAddr = APOLLOMP_RMA_CTRL00r;
    			break;
    		case 0x01:
    			regAddr = APOLLOMP_RMA_CTRL01r;
    			break;
    		case 0x02:
    			regAddr = APOLLOMP_RMA_CTRL02r;
    			break;
    		case 0x03:
    			regAddr = APOLLOMP_RMA_CTRL03r;
    			break;
    		case 0x04:
    		case 0x05:
    		case 0x06:
    		case 0x07:
    		case 0x09:
    		case 0x0A:
    		case 0x0B:
    		case 0x0C:
    		case 0x0F:
    			regAddr = APOLLOMP_RMA_CTRL04r;
    			break;
    		case 0x08:
    			regAddr = APOLLOMP_RMA_CTRL08r;
    			break;
    		case 0x0D:
    			regAddr = APOLLOMP_RMA_CTRL0Dr;
    			break;
    		case 0x0E:
    			regAddr = APOLLOMP_RMA_CTRL0Er;
    			break;
    		case 0x10:
    			regAddr = APOLLOMP_RMA_CTRL10r;
    			break;
    		case 0x11:
    			regAddr = APOLLOMP_RMA_CTRL11r;
    			break;
    		case 0x12:
    			regAddr = APOLLOMP_RMA_CTRL12r;
    			break;
    		case 0x13:
    		case 0x14:
    		case 0x15:
    		case 0x16:
    		case 0x17:
    		case 0x19:
    		case 0x1B:
    		case 0x1C:
    		case 0x1D:
    		case 0x1E:
    		case 0x1F:
    			regAddr = APOLLOMP_RMA_CTRL13r;
    			break;
    		case 0x18:
    			regAddr = APOLLOMP_RMA_CTRL18r;
    			break;
    		case 0x1A:
    			regAddr = APOLLOMP_RMA_CTRL1Ar;
    			break;
    		case 0x20:
    			regAddr = APOLLOMP_RMA_CTRL20r;
    			break;
    		case 0x21:
    			regAddr = APOLLOMP_RMA_CTRL21r;
    			break;
    		case 0x22:
            case 0x23:
            case 0x24:
            case 0x25:
            case 0x26:
            case 0x27:
            case 0x28:
            case 0x29:
            case 0x2a:
            case 0x2b:
            case 0x2c:
            case 0x2d:
            case 0x2e:
            case 0x2f:
    			regAddr = APOLLOMP_RMA_CTRL22r;
    			break;
    		default:
                return RT_ERR_CHIP_NOT_SUPPORTED;
    	}
    }

    if ((ret = reg_field_write(regAddr, APOLLOMP_KEEP_FORMATf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_TRAP), "");
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_trap_rmaKeepCtagEnable_set */




/* Module Name    : Trap       */
/* Sub-module Name: OAM packet */

/* Function Name:
 *      dal_apollomp_trap_oamPduAction_get
 * Description:
 *      Get forwarding action of trapped oam PDU on specified port.
 * Input:
 *      None.
 * Output:
 *      pAction - pointer to forwarding action of trapped oam PDU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
dal_apollomp_trap_oamPduAction_get(rtk_action_t *pAction)
{
    int32  ret;
    uint32 state;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pAction), RT_ERR_NULL_POINTER);

    RT_ERR_CHK(reg_field_read(APOLLOMP_OAM_CTRL_1r, APOLLOMP_OAM_ENABLEf, &state), ret);

    *pAction = ((ENABLED == state) ? ACTION_TRAP2CPU : ACTION_FORWARD);
    return RT_ERR_OK;
} /* end of dal_apollomp_trap_oamPduAction_get */

/* Function Name:
 *      dal_apollomp_trap_oamPduAction_set
 * Description:
 *      Set forwarding action of trapped oam PDU on specified port.
 * Input:
 *      action - forwarding action of trapped oam PDU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
int32
dal_apollomp_trap_oamPduAction_set(rtk_action_t action)
{
    int32  ret;
    uint32 state;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    switch(action)
    {
        case ACTION_FORWARD:
            state = DISABLED;
            break;
        case ACTION_TRAP2CPU:
            state = ENABLED;
            break;
        default:
            return RT_ERR_CHIP_NOT_SUPPORTED;
            break;
    }

    RT_ERR_CHK(reg_field_write(APOLLOMP_OAM_CTRL_1r, APOLLOMP_OAM_ENABLEf, &state), ret);
    return RT_ERR_OK;
} /* end of dal_apollomp_trap_oamPduAction_set */

/* Function Name:
 *      dal_apollomp_trap_oamPduPri_get
 * Description:
 *      Get priority of trapped OAM PDU.
 * Input:
 *      None.
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_trap_oamPduPri_get(rtk_pri_t *pPriority)
{
    int32  ret;
    uint32 pri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPriority), RT_ERR_NULL_POINTER);

    RT_ERR_CHK(reg_field_read(APOLLOMP_OAM_CTRL_0r, APOLLOMP_OAM_PRIOIRTYf, &pri), ret);
    *pPriority = (rtk_pri_t)pri;

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_oamPduPri_get */

/* Function Name:
 *      dal_apollomp_trap_oamPduPri_set
 * Description:
 *      Set priority of trapped OAM PDU.
 * Input:
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Note:
 *      None
 */
int32
dal_apollomp_trap_oamPduPri_set(rtk_pri_t priority)
{

    int32  ret;
    uint32 pri;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_TRAP),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(apollomp_trap_init);

    /* parameter check */
    RT_PARAM_CHK((HAL_INTERNAL_PRIORITY_MAX() < priority), RT_ERR_QOS_INT_PRIORITY);

    pri = (uint32)priority;
    RT_ERR_CHK(reg_field_write(APOLLOMP_OAM_CTRL_0r, APOLLOMP_OAM_PRIOIRTYf, &pri), ret);

    return RT_ERR_OK;
} /* end of dal_apollomp_trap_oamPduPri_set */


