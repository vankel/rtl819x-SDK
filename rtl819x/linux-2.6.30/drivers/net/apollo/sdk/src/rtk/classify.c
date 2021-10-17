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
 * $Revision: 14000 $
 * $Date: 2010-11-08 17:47:25 +0800 (?��?一, 08 ?��???2010) $
 *
 * Purpose : Definition of Classifyication API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) classfication rule add/delete/get
 */



/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/init.h>
#include <rtk/default.h>
#include <dal/dal_mgmt.h>
#include <rtk/classify.h>
/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_classify_init
 * Description:
 *      Initialize classification module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize classification module before calling any classification APIs.
 */
int32
rtk_classify_init(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_init */

/* Function Name:
 *      rtk_classify_cfgEntry_add
 * Description:
 *      Add an classification entry to ASIC
 * Input:
 *      entryIdx       - index of classification entry.
 *      pClassifyCfg     - The classification configuration that this function will add comparison rule
 *      pClassifyAct     - Action(s) of classification configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None.
 */
int32
rtk_classify_cfgEntry_add(rtk_classify_cfg_t *pClassifyCfg)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfgEntry_add( pClassifyCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cfgEntry_add */


/* Function Name:
 *      rtk_classify_cfgEntry_get
 * Description:
 *      Gdd an classification entry from ASIC
 * Input:
 *      None.
 * Output:
 *      pClassifyCfg     - The classification configuration that this function will add comparison rule
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pClassifyCfg point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      None.
 */
int32
rtk_classify_cfgEntry_get(rtk_classify_cfg_t *pClassifyCfg)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfgEntry_get( pClassifyCfg);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cfgEntry_get */


/* Function Name:
 *      rtk_classify_cfgEntry_del
 * Description:
 *      Delete an classification configuration from ASIC
 * Input:
 *      entryIdx    - index of classification entry.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK               - OK
 *      RT_ERR_FAILED           - Failed
 *      RT_ERR_ENTRY_INDEX 		- Invalid classification index .
 * Note:
 *      None.
 */
int32
rtk_classify_cfgEntry_del(uint32 entryIdx)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfgEntry_del( entryIdx);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cfgEntry_del */


/* Function Name:
 *      rtk_classify_field_add
 * Description:
 *      Add comparison field to an classfication configuration
 * Input:
 *      pClassifyEntry     - The classfication configuration that this function will add comparison rule
 *      pClassifyField     - The comparison rule that will be added.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER    	- Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT 			- Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pClassifyField) to an ACL configuration (*pClassifyEntry).
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pAclField will be added to linked
 *      list keeped by structure that pAclEntry points to.
 *      - caller should not free (*pClassifyField) before rtk_classify_cfgEntry_add is called
 */
int32
rtk_classify_field_add(rtk_classify_cfg_t *pClassifyEntry, rtk_classify_field_t *pClassifyField)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_field_add( pClassifyEntry, pClassifyField);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_field_add */


/* Function Name:
 *      rtk_classify_unmatchAction_set
 * Description:
 *      Apply action to packets when no classfication configuration match
 * Input:
 *      None
 * Output:
 *      action - unmatch action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no classfication configruation matches.
 */
int32
rtk_classify_unmatchAction_set(rtk_classify_unmatch_action_t action)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_unmatchAction_set( action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_unmatchAction_set */


/* Function Name:
 *      rtk_classify_unmatchAction_get
 * Description:
 *      Get action to packets when no classfication configuration match
 * Input:
 *      None
 * Output:
 *      pAction - Action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_NULL_POINTER - Pointer pAction point to NULL.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no classfication configruation matches.
 */
int32
rtk_classify_unmatchAction_get(rtk_classify_unmatch_action_t *pAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_unmatchAction_get( pAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_unmatchAction_get */


/* Function Name:
 *      rtk_classify_portRange_set
 * Description:
 *      Set Port Range check
 * Input:
 *      pRangeEntry - L4 Port Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      UpperPort must be larger or equal than lowerPort.
 *      This function is not supported in Test chip.
 */
int32
rtk_classify_portRange_set(rtk_classify_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_portRange_set( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_portRange_set */


/* Function Name:
 *      rtk_classify_portRange_get
 * Description:
 *      Set Port Range check
 * Input:
 *      None
 * Output:
 *      pRangeEntry - L4 Port Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      This function is not supported in Test chip.
 */
int32
rtk_classify_portRange_get(rtk_classify_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_portRange_get( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_portRange_get */


/* Function Name:
 *      rtk_classify_ipRange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      pRangeEntry - IP Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      UpperIp must be larger or equal than lowerIp.
 *      This function is not supported in Test chip.
 */
int32
rtk_classify_ipRange_set(rtk_classify_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_ipRange_set( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_ipRange_set */


/* Function Name:
 *      rtk_classify_ipRange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      None.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      This function is not supported in Test chip.
 */
int32
rtk_classify_ipRange_get(rtk_classify_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_ipRange_get( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_ipRange_get */

/* Function Name:
 *      rtk_classify_cfSel_set
 * Description:
 *      Set CF port selection, only pon port and RGMII port can be set
 * Input:
 *      port    - port id, only pon port and RGMII port can be set.
 *      cfSel   - CF port selection.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      Only accept pon port and RGMII port.
 *      This function is not supported in Test chip.
 */
int32
rtk_classify_cfSel_set(rtk_port_t port, rtk_classify_cf_sel_t cfSel)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cf_sel_set( port, cfSel);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cf_sel_set */

/* Function Name:
 *      rtk_classify_cfSel_get
 * Description:
 *      Get CF port selection, only pon port and RGMII port can be get
 * Input:
 *      port    - port id, only pon port and RGMII port can be get.
 *      pCfSel  - pointer of CF port selection.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      Only accept pon port and RGMII port.
 *      This function is not supported in Test chip.
 */
int32
rtk_classify_cfSel_get(rtk_port_t port, rtk_classify_cf_sel_t *pCfSel)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cf_sel_get( port, pCfSel);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_classify_cf_sel_get */

/* Function Name:
 *      rtk_classify_cfPri2Dscp_set
 * Description:
 *      Set CF priority to DSCP value mapping
 * Input:
 *      pri    - priority value
 *      dscp   - DSCP value.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      This function is not supported in Test chip.
 */
extern int32
rtk_classify_cfPri2Dscp_set(rtk_pri_t pri, rtk_dscp_t dscp)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfPri2Dscp_set( pri, dscp);
    RTK_API_UNLOCK();
    return ret;
}

/* Function Name:
 *      rtk_classify_cfPri2Dscp_get
 * Description:
 *      Get CF priority to DSCP value mapping
 * Input:
 *      pri    - priority value
 * Output:
 *      pDscp  - pointer of DSCP value.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_NULL_POINTER    - Pointer pClassifyCfg point to NULL.
 * Note:
 *      This function is not supported in Test chip.
 */
extern int32
rtk_classify_cfPri2Dscp_get(rtk_pri_t pri, rtk_dscp_t *pDscp)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->classify_cfPri2Dscp_get( pri, pDscp);
    RTK_API_UNLOCK();
    return ret;
}
