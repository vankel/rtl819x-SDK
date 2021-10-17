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
 * $Date: 2010-11-08 17:47:25 +0800 (?üÊ?‰∏Ä, 08 ?Å‰???2010) $
 *
 * Purpose : Definition of ACL API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) ACL rule action configure and modification
 *
 */
 


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <rtk/acl.h> 
#include <dal/dal_mgmt.h> 

/*
 * Symbol Definition
 */


/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      rtk_acl_init
 * Description:
 *      Initialize ACL module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize ACL module before calling any ACL APIs.
 *      Apollo init acl mode  as ACL_IGR_RULE_MODE_0
 */
int32
rtk_acl_init(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_init */

/* Function Name:
 *      rtk_acl_template_set
 * Description:
 *      Set template of ingress ACL.
 * Input:
 *      template - Ingress ACL template
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function set ACL template.
 */
int32 
rtk_acl_template_set(rtk_acl_template_t *aclTemplate)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_template_set( aclTemplate);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_template_set */


/* Function Name:
 *      rtk_acl_template_get
 * Description:
 *      Get template of ingress ACL.
 * Input:
 *      template - Ingress ACL template
 * Output:
 *      template - Ingress ACL template
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Invalid input parameters.
 * Note:
 *      This function get ACL template.
 */
int32 
rtk_acl_template_get(rtk_acl_template_t *aclTemplate)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_template_get( aclTemplate);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_template_get */

/* Function Name:
 *      rtk_acl_fieldSelect_set
 * Description:
 *      Set user defined field selectors in HSB
 * Input:
 *      pFieldEntry 	- pointer of field selector entry
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      System support 16 user defined field selctors.
 * 		Each selector can be enabled or disable.
 *      User can defined retrieving 16-bits in many predefiend
 * 		standard l2/l3/l4 payload.
 */
int32 
rtk_acl_fieldSelect_set(rtk_acl_field_entry_t *pFieldEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_fieldSelect_set( pFieldEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_fieldSelect_set */

/* Function Name:
 *      rtk_acl_fieldSelect_get
 * Description:
 *      Get user defined field selectors in HSB
 * Input:
 *      None
 * Output:
 *      pFieldEntry 	- pointer of field selector entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 * Note:
 *      None.
 */
int32 
rtk_acl_fieldSelect_get(rtk_acl_field_entry_t *pFieldEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_fieldSelect_get( pFieldEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_fieldSelect_get */

/* Function Name:
 *      rtk_acl_igrRuleEntry_get
 * Description:
 *      Get an ACL entry from ASIC
 * Input:
 *      None.
 * Output:
 *      pAclRule     - The ACL configuration that this function will add comparison rule
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclRule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      use this API to get rule entry the field data will return in raw format
 *      raw data is return in pAclRule->field.readField
 */
int32 
rtk_acl_igrRuleEntry_get(rtk_acl_ingress_entry_t *pAclRule)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_ruleEntry_get( pAclRule);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrRuleEntry_get */

/* Function Name:
 *      rtk_acl_igrField_add
 * Description:
 *      Add comparison rule to an ACL configuration
 * Input:
 *      pAclEntry     - The ACL configuration that this function will add comparison rule
 *      pAclField   - The comparison rule that will be added.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              	- OK
 *      RT_ERR_FAILED          	- Failed
 *      RT_ERR_NULL_POINTER    	- Pointer pFilter_field or pFilter_cfg point to NULL.
 *      RT_ERR_INPUT 			- Invalid input parameters.
 * Note:
 *      This function add a comparison rule (*pAclField) to an ACL configuration (*pAclEntry).
 *      Pointer pFilter_cfg points to an ACL configuration structure, this structure keeps multiple ACL
 *      comparison rules by means of linked list. Pointer pAclField will be added to linked
 *      list keeped by structure that pAclEntry points to.
 *      caller should not free (*pAclField) before rtk_acl_igrRuleEntry_add is called
 */
int32 
rtk_acl_igrField_add(rtk_acl_ingress_entry_t *pAclRule, rtk_acl_field_t *pAclField)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrField_add( pAclRule, pAclField);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrField_add */


/* Function Name:
 *      rtk_acl_igrRuleEntry_add
 * Description:
 *      Add an ACL configuration to ASIC
 * Input:
 *      pAclRule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclrule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 *      RT_ERR_ENTRY_INDEX 						- Invalid entryIdx .
 * Note:
 *      None
 */
int32 
rtk_acl_igrRuleEntry_add(rtk_acl_ingress_entry_t *pAclRule)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrRuleEntry_add( pAclRule);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrRuleEntry_add */

/* Function Name:
 *      rtk_acl_igrRuleEntry_del
 * Description:
 *      Delete an ACL configuration from ASIC
 * Input:
 *      pAclrule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_ENTRY_INDEX 						- Invalid entryIdx .
 * Note:
 *      None
 */
int32 
rtk_acl_igrRuleEntry_del(uint32 index)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrRuleEntry_del( index);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrRuleEntry_del */

/* Function Name:
 *      rtk_acl_igrRuleEntry_delAll
 * Description:
 *      Delete all ACL configuration from ASIC
 * Input:
 *      pAclrule   - ACL ingress filter rule configuration.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 * Note:
 *      None
 */
int32 
rtk_acl_igrRuleEntry_delAll(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrRuleEntry_delAll();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrRuleEntry_delAll */


/* Function Name:
 *      rtk_acl_igrUnmatchAction_set
 * Description:
 *      Apply action to packets when no ACL configuration match
 * Input:
 *      port    - Port id.
 *      action - Action.
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32 
rtk_acl_igrUnmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_type_t action)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrUnmatchAction_set( port, action);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrUnmatchAction_set */

/* Function Name:
 *      rtk_acl_igrUnmatchAction_get
 * Description:
 *      Get action to packets when no ACL configuration match
 * Input:
 *      port    - Port id.
 * Output:
 *      pAction - Action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_SMI          - SMI access error
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32 
rtk_acl_igrUnmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_type_t *pAction)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrUnmatchAction_get( port, pAction);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrUnmatchAction_get */


/* Function Name:
 *      rtk_acl_igrState_set
 * Description:
 *      Set state of ingress ACL.
 * Input:
 *      port    - Port id.
 *      state   - Ingress ACL state.
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32 
rtk_acl_igrState_set(rtk_port_t port, rtk_enable_t state)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrState_set( port, state);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrState_set */


/* Function Name:
 *      rtk_acl_igrState_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32 
rtk_acl_igrState_get(rtk_port_t port, rtk_enable_t *pState)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrState_get( port, pState);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrState_get */


/* Function Name:
 *      rtk_acl_ipRange_set
 * Description:
 *      Set IP Range check
 * Input:
 *      pRangeEntry - IP Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper Ip must be larger or equal than lowerIp.
 */
int32 
rtk_acl_ipRange_set(rtk_acl_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_ipRange_set( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_ipRange_set */


/* Function Name:
 *      rtk_acl_ipRange_get
 * Description:
 *      Set IP Range check
 * Input:
 *      None.
 * Output:
 *      pRangeEntry - IP Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
int32 
rtk_acl_ipRange_get(rtk_acl_rangeCheck_ip_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_ipRange_get( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_ipRange_get */

/* Function Name:
 *      rtk_acl_vidRange_set
 * Description:
 *      Set VID Range check
 * Input:
 *      pRangeEntry - VLAN id Range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper Vid must be larger or equal than lowerVid.
 */
int32 
rtk_acl_vidRange_set(rtk_acl_rangeCheck_vid_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_vidRange_set( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_vidRange_set */

/* Function Name:
 *      rtk_acl_vidRange_get
 * Description:
 *      Get VID Range check
 * Input:
 *      None.
 * Output:
 *      pRangeEntry - VLAN id Range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 * Note:
 *      None.
 */
int32 
rtk_acl_vidRange_get(rtk_acl_rangeCheck_vid_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_vidRange_get( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_vidRange_get */


/* Function Name:
 *      rtk_acl_portRange_set
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
 *      upper Port must be larger or equal than lowerPort.
 */
int32 
rtk_acl_portRange_set(rtk_acl_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_portRange_set( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_portRange_set */


/* Function Name:
 *      rtk_acl_portRange_get
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
 *      None.
 */
int32 
rtk_acl_portRange_get(rtk_acl_rangeCheck_l4Port_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_portRange_get( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_portRange_get */


/* Function Name:
 *      rtk_acl_packetLengthRange_set
 * Description:
 *      Set packet length Range check
 * Input:
 *      pRangeEntry - packet length range entry
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      upper length must be larger or equal than lower length.
 */
int32 
rtk_acl_packetLengthRange_set(rtk_acl_rangeCheck_pktLength_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_packetLengthRange_set( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_packetLengthRange_set */


/* Function Name:
 *      rtk_acl_packetLengthRange_get
 * Description:
 *      Set packet length Range check
 * Input:
 *      None
 * Output:
 *      pRangeEntry - packet length range entry
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_OUT_OF_RANGE    - The parameter is out of range
 *      RT_ERR_INPUT           - Input error
 * Note:
 *      None.
 */
int32 
rtk_acl_packetLengthRange_get(rtk_acl_rangeCheck_pktLength_t *pRangeEntry)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_packetLengthRange_get( pRangeEntry);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_packetLengthRange_get */


/* Function Name:
 *      rtk_acl_igrRuleMode_set
 * Description:
 *      Set ingress ACL rule mode
 * Input:
 *      mode - ingress ACL rule mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Input error
 * Note:
 *          - ACL_IGR_RULE_MODE_0, 64  rules, the size each rule is 16x8 bits 
 *          - ACL_IGR_RULE_MODE_1, 128 rules, 
 *          -               the size each rule is 16x4 bits(entry 0~63)
 *          -               the size each rule is 16x3 bits(entry 64~127) 
 */
int32 
rtk_acl_igrRuleMode_set(rtk_acl_igr_rule_mode_t mode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrRuleMode_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrRuleMode_set */



/* Function Name:
 *      rtk_acl_igrRuleMode_get
 * Description:
 *      Get ingress ACL rule mode
 * Input:
 *      None
 * Output:
 *      pMode - ingress ACL rule mode
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - Failed
 *      RT_ERR_INPUT           - Input error
 * Note:
 *          - ACL_IGR_RULE_MODE_0, 64  rules, the size each rule is 16x8 bits 
 *          - ACL_IGR_RULE_MODE_1, 128 rules, 
 *          -               the size of each rule is 16x4 bits(entry 0~63)
 *          -               the size of each rule is 16x3 bits(entry 64~127) 
 *          Mode chaged all template/rule will be cleared
 */
int32 
rtk_acl_igrRuleMode_get(rtk_acl_igr_rule_mode_t *pMode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrRuleMode_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrRuleMode_get */

/* Function Name:
 *      rtk_acl_igrPermitState_set
 * Description:
 *      Set permit state of ingress ACL.
 * Input:
 *      port    - Port id.
 *      state  - Ingress ACL state.
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function set action of packets when no ACL configruation matches.
 */
int32 
rtk_acl_igrPermitState_set(rtk_port_t port, rtk_enable_t state)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrPermitState_set( port, state);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrPermitState_set */


/* Function Name:
 *      rtk_acl_igrPermitState_get
 * Description:
 *      Get state of ingress ACL.
 * Input:
 *      port    - Port id.
 * Output:
 *      pState  - Ingress ACL state.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
int32 
rtk_acl_igrPermitState_get(rtk_port_t port, rtk_enable_t *pState)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->acl_igrPermitState_get( port, pState);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_acl_igrPermitState_get */





