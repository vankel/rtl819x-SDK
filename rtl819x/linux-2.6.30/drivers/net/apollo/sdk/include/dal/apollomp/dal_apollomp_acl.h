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
 * $Date: 2010-11-08 17:47:25 +0800 (星期一, 08 十一月 2010) $
 *
 * Purpose : Definition of ACL API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) ACL rule action configure and modification
 *
 */
 
#ifndef __DAL_APOLLOMP_ACL_H__
#define __DAL_APOLLOMP_ACL_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */
#define APOLLOMP_DAL_ACL_NO                      128
#define APOLLOMP_DAL_ACL_MAX                     (APOLLOMP_DAL_ACL_NO-1)
#define APOLLOMP_DAL_ACL_TEMPLATE_FIELD_NO       8

typedef enum apollomp_dal_acl_fieldTypes_e
{
	APOLLOMP_DAL_ACL_UNUSED,
	APOLLOMP_DAL_ACL_DMAC0,
	APOLLOMP_DAL_ACL_DMAC1,
	APOLLOMP_DAL_ACL_DMAC2,
	APOLLOMP_DAL_ACL_SMAC0,
	APOLLOMP_DAL_ACL_SMAC1,
	APOLLOMP_DAL_ACL_SMAC2,
	APOLLOMP_DAL_ACL_ETHERTYPE,
	APOLLOMP_DAL_ACL_STAG,
	APOLLOMP_DAL_ACL_CTAG,
	APOLLOMP_DAL_ACL_GEMIDXLLIDX,
	APOLLOMP_DAL_ACL_IP4SIP0 = 0x10,
	APOLLOMP_DAL_ACL_IP4SIP1,
	APOLLOMP_DAL_ACL_IP4DIP0,
	APOLLOMP_DAL_ACL_IP4DIP1,
	APOLLOMP_DAL_ACL_IP6SIP0WITHIPV4 = 0x20,
	APOLLOMP_DAL_ACL_IP6SIP1WITHIPV4,
	APOLLOMP_DAL_ACL_IP6DIP0WITHIPV4 = 0x28,
	APOLLOMP_DAL_ACL_IP6DIP1WITHIPV4,
	APOLLOMP_DAL_ACL_IP6NH,
	APOLLOMP_DAL_ACL_VIDRANGE = 0x30,
	APOLLOMP_DAL_ACL_IPRANGE,
	APOLLOMP_DAL_ACL_PORTRANGE,
	APOLLOMP_DAL_ACL_PKTLENRANGE,
	APOLLOMP_DAL_ACL_FIELD_VALID,
	APOLLOMP_DAL_ACL_FIELD_SELECT00 = 0x40,
	APOLLOMP_DAL_ACL_FIELD_SELECT01,
	APOLLOMP_DAL_ACL_FIELD_SELECT02,
	APOLLOMP_DAL_ACL_FIELD_SELECT03,
	APOLLOMP_DAL_ACL_FIELD_SELECT04,
	APOLLOMP_DAL_ACL_FIELD_SELECT05,
	APOLLOMP_DAL_ACL_FIELD_SELECT06,
	APOLLOMP_DAL_ACL_FIELD_SELECT07,
	APOLLOMP_DAL_ACL_FIELD_SELECT08,
	APOLLOMP_DAL_ACL_FIELD_SELECT09,
	APOLLOMP_DAL_ACL_FIELD_SELECT10,
	APOLLOMP_DAL_ACL_FIELD_SELECT11,
	APOLLOMP_DAL_ACL_FIELD_SELECT12,
	APOLLOMP_DAL_ACL_FIELD_SELECT13,
	APOLLOMP_DAL_ACL_FIELD_SELECT14,
	APOLLOMP_DAL_ACL_FIELD_SELECT15,
	APOLLOMP_DAL_ACL_TYPE_END
}apollomp_dal_acl_fieldTypes_t;


typedef enum apollomp_dal_acl_vidrange_e
{
    APOLLOMP_DAL_ACL_VIDRANGE_UNUSED = 0,
    APOLLOMP_DAL_ACL_VIDRANGE_CVID,
    APOLLOMP_DAL_ACL_VIDRANGE_SVID,
    APOLLOMP_DAL_ACL_VIDRANGE_END
}apollomp_dal_acl_vidrange_t;


typedef enum apollomp_dal_acl_iprange_e
{
    APOLLOMP_DAL_ACL_IPRANGE_UNUSED = 0,
    APOLLOMP_DAL_ACL_IPRANGE_IPV4_SIP,
    APOLLOMP_DAL_ACL_IPRANGE_IPV4_DIP,
    APOLLOMP_DAL_ACL_IPRANGE_IPV6_SIP,
    APOLLOMP_DAL_ACL_IPRANGE_IPV6_DIP,
    APOLLOMP_DAL_ACL_IPRANGE_END
}apollomp_dal_acl_iprange_t;

typedef enum apollomp_dal_acl_portrange_e
{
    APOLLOMP_DAL_ACL_PORTRANGE_UNUSED = 0,
    APOLLOMP_DAL_ACL_PORTRANGE_SPORT,
    APOLLOMP_DAL_ACL_PORTRANGE_DPORT,
    APOLLOMP_DAL_ACL_PORTRANGE_END
}apollomp_dal_acl_portrange_type_t;


typedef enum apollomp_dal_field_sel_e
{
    APOLLOMP_DAL_FIELD_FORMAT_DEFAULT = 0,
    APOLLOMP_DAL_FIELD_FORMAT_RAW,
    APOLLOMP_DAL_FIELD_FORMAT_LLC,
    APOLLOMP_DAL_FIELD_FORMAT_IPV4,
    APOLLOMP_DAL_FIELD_FORMAT_ARP,
    APOLLOMP_DAL_FIELD_FORMAT_IPV6,
    APOLLOMP_DAL_FIELD_FORMAT_IPPAYLOAD,
    APOLLOMP_DAL_FIELD_FORMAT_L4PAYLOAD,
    APOLLOMP_DAL_FIELD_FORMAT_END
}apollomp_dal_field_sel_t;



/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
/* Function Name:
 *      dal_apollomp_acl_init
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
extern int32
dal_apollomp_acl_init(void);

/* Function Name:
 *      dal_apollomp_acl_template_set
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
extern int32 
dal_apollomp_acl_template_set(rtk_acl_template_t *aclTemplate);


/* Function Name:
 *      dal_apollomp_acl_template_get
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
extern int32 
dal_apollomp_acl_template_get(rtk_acl_template_t *aclTemplate);

/* Function Name:
 *      dal_apollomp_acl_fieldSelect_set
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
extern int32 
dal_apollomp_acl_fieldSelect_set(rtk_acl_field_entry_t *pFieldEntry);

/* Function Name:
 *      dal_apollomp_acl_fieldSelect_get
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
extern int32 
dal_apollomp_acl_fieldSelect_get(rtk_acl_field_entry_t *pFieldEntry);

/* Function Name:
 *      dal_apollomp_acl_igrRuleAction_get
 * Description:
 *      Get an ACL action from ASIC
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
 *      use this API to get action control and data
 */
extern int32 
dal_apollomp_acl_igrRuleAction_get(rtk_acl_ingress_entry_t *pAclRule);

/* Function Name:
 *      dal_apollomp_acl_igrRuleAction_set
 * Description:
 *      Get an ACL action from ASIC
 * Input:
 *      pAclRule     - The ACL configuration that this function will add comparison rule
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK              					- OK
 *      RT_ERR_FAILED          					- Failed
 *      RT_ERR_NULL_POINTER    					- Pointer pAclRule point to NULL.
 *      RT_ERR_INPUT 							- Invalid input parameters.
 * Note:
 *      use this API to set action control and data
 */
extern int32 
dal_apollomp_acl_igrRuleAction_set(rtk_acl_ingress_entry_t *pAclRule);


/* Function Name:
 *      dal_apollomp_acl_igrRuleEntry_get
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
extern int32 
dal_apollomp_acl_igrRuleEntry_get(rtk_acl_ingress_entry_t *pAclRule);


/* Function Name:
 *      dal_apollomp_acl_igrRuleField_add
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
 *      caller should not free (*pAclField) before dal_apollomp_acl_igrRuleEntry_add is called
 */
extern int32 
dal_apollomp_acl_igrRuleField_add(rtk_acl_ingress_entry_t *pAclRule, rtk_acl_field_t *pAclField);


/* Function Name:
 *      dal_apollomp_acl_igrRuleEntry_add
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
extern int32 
dal_apollomp_acl_igrRuleEntry_add(rtk_acl_ingress_entry_t *pAclRule);

/* Function Name:
 *      dal_apollomp_acl_igrRuleEntry_del
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
extern int32 
dal_apollomp_acl_igrRuleEntry_del(uint32 index);

/* Function Name:
 *      dal_apollomp_acl_igrRuleEntry_delAll
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
extern int32 
dal_apollomp_acl_igrRuleEntry_delAll(void);


/* Function Name:
 *      dal_apollomp_acl_igrUnmatchAction_set
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
extern int32 
dal_apollomp_acl_igrUnmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_type_t action);

/* Function Name:
 *      dal_apollomp_acl_igrUnmatchAction_get
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
extern int32 
dal_apollomp_acl_igrUnmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_type_t *pAction);


/* Function Name:
 *      dal_apollomp_acl_igrState_set
 * Description:
 *      Set state of ingress ACL.
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
 *      This function set per-port ACL filtering state.
 */
extern int32 
dal_apollomp_acl_igrState_set(rtk_port_t port, rtk_enable_t state);


/* Function Name:
 *      dal_apollomp_acl_igrState_get
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
 *      This function gets per-port ACL filtering state.
 */
extern int32 
dal_apollomp_acl_igrState_get(rtk_port_t port, rtk_enable_t *pState);


/* Function Name:
 *      dal_apollomp_acl_ipRange_set
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
 *      upper IP must be larger or equal than lower IP.
 */
extern int32 
dal_apollomp_acl_ipRange_set(rtk_acl_rangeCheck_ip_t *pRangeEntry);


/* Function Name:
 *      dal_apollomp_acl_ipRange_get
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
extern int32 
dal_apollomp_acl_ipRange_get(rtk_acl_rangeCheck_ip_t *pRangeEntry);

/* Function Name:
 *      dal_apollomp_acl_vidRange_set
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
extern int32 
dal_apollomp_acl_vidRange_set(rtk_acl_rangeCheck_vid_t *pRangeEntry);

/* Function Name:
 *      dal_apollomp_acl_vidRange_get
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
extern int32 
dal_apollomp_acl_vidRange_get(rtk_acl_rangeCheck_vid_t *pRangeEntry);


/* Function Name:
 *      dal_apollomp_acl_portRange_set
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
extern int32 
dal_apollomp_acl_portRange_set(rtk_acl_rangeCheck_l4Port_t *pRangeEntry);


/* Function Name:
 *      dal_apollomp_acl_portRange_get
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
extern int32 
dal_apollomp_acl_portRange_get(rtk_acl_rangeCheck_l4Port_t *pRangeEntry);


/* Function Name:
 *      dal_apollomp_acl_packetLengthRange_set
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
extern int32 
dal_apollomp_acl_packetLengthRange_set(rtk_acl_rangeCheck_pktLength_t *pRangeEntry);


/* Function Name:
 *      dal_apollomp_acl_packetLengthRange_get
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
extern int32 
dal_apollomp_acl_packetLengthRange_get(rtk_acl_rangeCheck_pktLength_t *pRangeEntry);


/* Function Name:
 *      dal_apollomp_acl_igrRuleMode_set
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
extern int32 
dal_apollomp_acl_igrRuleMode_set(rtk_acl_igr_rule_mode_t mode);



/* Function Name:
 *      dal_apollomp_acl_igrRuleMode_get
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
extern int32 
dal_apollomp_acl_igrRuleMode_get(rtk_acl_igr_rule_mode_t *pMode);

/* Function Name:
 *      dal_apollomp_acl_igrPermitState_set
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
extern int32 
dal_apollomp_acl_igrPermitState_set(rtk_port_t port, rtk_enable_t state);


/* Function Name:
 *      dal_apollomp_acl_igrPermitState_get
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
extern int32 
dal_apollomp_acl_igrPermitState_get(rtk_port_t port, rtk_enable_t *pState);

/* Function Name:
 *      dal_apollomp_acl_log_set
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
extern int32 
dal_apollomp_acl_igrPermitState_set(rtk_port_t port, rtk_enable_t state);

#endif /* __DAL_APOLLOMP_ACL_H__ */

