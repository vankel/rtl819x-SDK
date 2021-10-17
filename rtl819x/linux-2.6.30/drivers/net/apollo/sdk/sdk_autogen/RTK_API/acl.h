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
 
#ifndef __RTK_ACL_H__
#define __RTK_ACL_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */
#define RTK_ACL_IGR_FIELD_NUMBER                8


typedef uint32 rtk_filter_state_t;



typedef enum rtk_acl_field_format_e
{
    ACL_FIELD_FORMAT_NORMAL = 0,
    ACL_FIELD_FORMAT_RAW,
    ACL_FIELD_FORMAT_END
}rtk_acl_field_format_t;


/*
 * Data Declaration
 */


typedef enum rtk_acl_igr_rule_mode_e
{
    ACL_IGR_RULE_MODE_0 = 0,
    ACL_IGR_RULE_MODE_1
    ACL_IGR_RULE_MODE_END,
} rtk_acl_igr_rule_mode_t;


typedef enum rtk_field_sel_e
{
    ACL_FORMAT_DEFAULT = 0,
    ACL_FORMAT_RAW,
    ACL_FORMAT_LLC,
    ACL_FORMAT_IPV4,
    ACL_FORMAT_ARP,
    ACL_FORMAT_IPV6,
    ACL_FORMAT_IPPAYLOAD,
    ACL_FORMAT_L4PAYLOAD,
    ACL_FORMAT_END
}rtk_field_sel_t;

typedef enum rtk_acl_field_type_e
{
    ACL_FIELD_DMAC = 0,
    ACL_FIELD_SMAC,
    ACL_FIELD_ETHERTYPE,
    ACL_FIELD_CTAG,
    ACL_FIELD_STAG,

    ACL_FIELD_IPV4_SIP,
    ACL_FIELD_IPV4_DIP,
    ACL_FIELD_IPV6_SIPV6,
    ACL_FIELD_IPV6_DIPV6,
    ACL_FIELD_IPV6_NEXT_HEADER,

    ACL_FIELD_TCP_SPORT,
    ACL_FIELD_TCP_DPORT,
    ACL_FIELD_UDP_SPORT,
    ACL_FIELD_UDP_DPORT,

    ACL_FIELD_IPV4_DIP,


    ACL_FIELD_VID_RANGE,
    ACL_FIELD_IP_RANGE,
    ACL_FIELD_PORT_RANGE,
    ACL_FIELD_PKT_LEN_RANGE,
    ACL_FIELD_USER_DEFINED00,
    ACL_FIELD_USER_DEFINED01,
    ACL_FIELD_USER_DEFINED02,
    ACL_FIELD_USER_DEFINED03,
    ACL_FIELD_USER_DEFINED04,
    ACL_FIELD_USER_DEFINED05,
    ACL_FIELD_USER_DEFINED06,
    ACL_FIELD_USER_DEFINED07,
    ACL_FIELD_USER_DEFINED08,
    ACL_FIELD_USER_DEFINED09,
    ACL_FIELD_USER_DEFINED10,
    ACL_FIELD_USER_DEFINED11,
    ACL_FIELD_USER_DEFINED12,
    ACL_FIELD_USER_DEFINED13,
    ACL_FIELD_USER_DEFINED14,
    ACL_FIELD_USER_DEFINED15,

    ACL_FIELD_PATTERN_MATCH,

    ACL_FIELD_END,
} rtk_acl_field_type_t;


typedef enum rtk_acl_invert_e
{
    ACL_INVERT_DISABLE = 0,
    ACL_INVERT_ENABLE,
    ACL_INVERT_END,
} rtk_acl_invert_t;

typedef enum rtk_filter_unmatch_action_e
{
    FILTER_UNMATCH_DROP = 0,
    FILTER_UNMATCH_PERMIT,
    FILTER_UNMATCH_END,
} rtk_filter_unmatch_action_type_t;

typedef struct rtk_acl_template_s
{
    uint32 index;
    rtk_acl_field_type_t fieldType[RTK_ACL_IGR_FIELD_NUMBER];
} rtk_acl_template_t;



typedef struct rtk_acl_field_entry_s
{
    uint32 index;
    rtk_field_sel_t format;
    uint32 offset;
} rtk_acl_field_entry_t;


typedef enum rtk_filter_portrange_e
{
    PORTRANGE_UNUSED = 0,
    PORTRANGE_SPORT,
    PORTRANGE_DPORT,
    PORTRANGE_END
}rtk_acl_portrange_type_t;



typedef struct rtk_acl_rangeCheck_l4Port_s
{
    uint16  index;
    uint16  upper_bound;    /* Port range upper bound */
    uint16  lower_bound;    /* Port range lower bound */
    rtk_acl_portrange_type_t     type; 
                            /* 0: unused, 
                             * 1: compare source port,
                             * 2: compare destination port
                             */
} rtk_acl_rangeCheck_l4Port_t;

typedef enum rtk_acl_vidrange_e
{
    VIDRANGE_UNUSED = 0,
    VIDRANGE_CVID,
    VIDRANGE_SVID,
    VIDRANGE_END
}rtk_acl_vidrange_t;

typedef struct rtk_acl_rangeCheck_pktLength_s
{
    uint16  index;
    uint16  upper_bound;    /* packet length range upper bound */
    uint16  lower_bound;    /* packet length range lower bound */
} rtk_acl_rangeCheck_pktLength_t;


typedef struct rtk_acl_rangeCheck_vid_s
{
    uint16  index;
    uint16  upperVid;    /* vid range upper bound */
    uint16  lowerVid;    /* vid range lower bound */
    rtk_acl_vidrange_t     type; 
} rtk_acl_rangeCheck_vid_t;



typedef enum rtk_acl_iprange_e
{
    IPRANGE_UNUSED = 0,
    IPRANGE_IPV4_SIP,
    IPRANGE_IPV4_DIP,
    IPRANGE_IPV6_SIP,
    IPRANGE_IPV6_DIP,
    IPRANGE_END
}rtk_acl_iprange_t;


typedef struct rtk_acl_rangeCheck_ip_s
{
    uint16 index;
    /*for IPv6 the address only specify IPv6[31:0]*/
    ipaddr_t upperIp;    /* IP range upper bound */
    ipaddr_t lowerIp;    /* IP range lower bound */
    
    rtk_acl_iprange_t type; 
} rtk_acl_rangeCheck_ip_t;



typedef struct rtk_acl_value_s
{
    rtk_uint32 value;
    rtk_uint32 mask;
} rtk_acl_value_t;



typedef struct rtk_filter_flag_s
{
    rtk_uint32 value;
    rtk_uint32 mask;
} rtk_filter_flag_t;

typedef enum rtk_acl_care_tag_index_e
{
    ACL_CARE_TAG_PPPOE = 0,
    ACL_CARE_TAG_CTAG,
    ACL_CARE_TAG_STAG,
    ACL_CARE_TAG_IPV4,
    ACL_CARE_TAG_IPV6,
    ACL_CARE_TAG_TCP,
    ACL_CARE_TAG_UDP,
    ACL_CARE_TAG_END,
} rtk_acl_care_tag_index_t;

typedef struct rtk_filter_care_tag_s
{
    rtk_acl_value_t tagType[ACL_CARE_TAG_END];
} rtk_acl_care_tag_t;

/*clvan action*/
typedef enum rtk_acl_igr_cvlan_act_ctrl_e
{
    ACL_IGR_CVLAN_IGR_CVLAN_ACT = 0,
    ACL_IGR_CVLAN_EGR_CVLAN_ACT,
    ACL_IGR_CVLAN_DS_SVID_ACT,
    ACL_IGR_CVLAN_LOG_ACT,
    ACL_IGR_CVLAN_ACT_END,
} rtk_acl_igr_cvlan_act_ctrl_t;

typedef enum rtk_acl_igr_cvlan_act_s
{
    rtk_acl_igr_cvlan_act_ctrl_t act; 
    uint8                        cvidx;
    uint8                        meterMibIdx;
} rtk_acl_igr_cvlan_act_t;


/*svlan action*/
typedef enum rtk_acl_igr_svlan_act_ctrl_e
{
    ACL_IGR_SVLAN_IGR_SVLAN_ACT = 0,
    ACL_IGR_SVLAN_EGR_SVLAN_ACT,
    ACL_IGR_SVLAN_US_CVID_ACT,
    ACL_IGR_SVLAN_LOG_ACT,
    ACL_IGR_SVLAN_ACT_END,
} rtk_acl_igr_svlan_act_ctrl_t;


typedef enum rtk_acl_igr_svlan_act_s
{
    rtk_acl_igr_svlan_act_ctrl_t act; 
    uint8                        svidx;
    uint8                        meterMibIdx;
} rtk_acl_igr_svlan_act_t;


/*priority action*/
typedef enum rtk_acl_igr_pri_act_ctrl_e
{
    ACL_IGR_PRI_ACL_PRI_ASSIGN_ACT = 0,
    ACL_IGR_PRI_DSCP_REMARK_ACT,
    ACL_IGR_PRI_1P_REMARK_ACT,
    ACL_IGR_PRI_LOG_ACT,
    ACL_IGR_PRI_ACT_END,
} rtk_acl_igr_pri_act_ctrl_t;


typedef enum rtk_acl_igr_pri_act_s
{
    rtk_acl_igr_pri_act_ctrl_t act; 
    uint8                      aclPri;
    uint8                      dscp;
    uint8                      ipPri;
    uint8                      meterMibIdx;
} rtk_acl_igr_pri_act_t;


/*policing action*/
typedef enum rtk_acl_igr_pri_act_s
{
    uint8                      meterMibIdx;
} rtk_acl_igr_log_act_t;



/*forward action*/
typedef enum rtk_acl_igr_forward_act_ctrl_e
{
    ACL_IGR_FORWARD_COPY_ACT = 0,
    ACL_IGR_FORWARD_REDIRECT_ACT,
    ACL_IGR_FORWARD_IGR_MIRROR_ACT,
    ACL_IGR_FORWARD_TRAP_ACT,
    ACL_IGR_FORWARD_ACT_END,
} rtk_acl_igr_forward_act_ctrl_t;


typedef enum rtk_acl_igr_forward_act_s
{
    rtk_acl_igr_forward_act_ctrl_t act; 
    rtk_portmask_t                 portMask;
} rtk_acl_igr_forward_act_t;


/*interrupt, GPIO action*/
typedef enum rtk_acl_igr_gpio_act_ctrl_e
{
    ACL_IGR_GPIO_ENABLE_ACT = 0,
    ACL_IGR_GPIO_DISABLE_ACT,
    ACL_IGR_GPIO_ACT_END,
} rtk_acl_igr_gpio_act_ctrl_t;


typedef enum rtk_acl_igr_extend_act_s
{
    rtk_acl_igr_gpio_act_ctrl_t act; 
    uint8                       gpioPin;
} rtk_acl_igr_extend_act_t;



typedef enum rtk_acl_igr_act_type_e
{
    ACL_IGR_CVLAN_ACT = 0,
    ACL_IGR_SVLAN_ACT,
    ACL_IGR_PRI_ACT,
    ACL_IGR_LOG_ACT,
    ACL_IGR_FORWARD_ACT,
    ACL_IGR_INTR_ACT,
    ACL_IGR_ACT_END,
} rtk_acl_igr_act_type_t;


typedef struct rtk_acl_igr_act_s
{
    rtk_enable_t              enableAct[ACL_IGR_ACT_END];
    rtk_acl_igr_cvlan_act_t   cvlanAct;
    rtk_acl_igr_svlan_act_t   svlanAct;
    rtk_acl_igr_pri_act_t     priAct;
    rtk_acl_igr_log_act_t     logAct;
    rtk_acl_igr_forward_act_t forwardAct;
    rtk_acl_igr_extend_act_t  extendAct; 
} rtk_acl_igr_act_t;



typedef struct rtk_acl_mac_s
{
    rtk_mac_t value;
    rtk_mac_t mask;
} rtk_acl_mac_t;

typedef struct rtk_acl_ip_s
{
    ipaddr_t value;
    ipaddr_t mask;
} rtk_acl_ip_t;


typedef struct rtk_acl_value_s
{
    uint16 value;
    uint16 mask;
} rtk_acl_value_t


typedef struct rtk_acl_tag_s
{
    rtk_acl_value_t pri;
    rtk_acl_value_t cfi;
    rtk_acl_value_t vid;
} rtk_acl_tag_t;


typedef struct rtk_filter_tcpFlag_s
{
    rtk_acl_value_t urg;
    rtk_acl_value_t ack;
    rtk_acl_value_t psh;
    rtk_acl_value_t rst;
    rtk_acl_value_t syn;
    rtk_acl_value_t fin;
    rtk_acl_value_t ns;
    rtk_acl_value_t cwr;
    rtk_acl_value_t ece;
} rtk_acl_tcpFlag_t;



typedef struct rtk_filter_ipFlag_s
{
    rtk_acl_value_t xf;
    rtk_acl_value_tmf;
    rtk_acl_value_t df;
} rtk_acl_ipFlag_t;



typedef struct rtk_acl_data_field_s
{
    rtk_acl_value_t data;
    uint16          fieldIdx;
} rtk_acl_data_field_t


typedef struct
{
    rtk_uint32 addr[RTK_IPV6_ADDR_WORD_LENGTH];
} rtk_acl_ip6_addr_t;


typedef struct
{
    rtk_acl_ip6_addr_t value;
    rtk_acl_ip6_addr_t mask;
} rtk_acl_ip6_t;


typedef struct rtk_acl_field_s
{
    rtk_acl_field_type_t fieldType;

    union
    {
        /*for type
         ACL_FIELD_DMAC
         ACL_FIELD_SMAC
        */
        rtk_acl_mac_t     mac;

        /*for type
         ACL_FIELD_CTAG
         ACL_FIELD_STAG,
        */
        rtk_acl_tag_t     l2tag;

        /*for type
         ACL_FIELD_IPV4_SIP
         ACL_FIELD_IPV4_DIP
         ACL_FIELD_IPV6_SIPV6 for IPv6 only specify IPv6[31:0]
         ACL_FIELD_IPV6_DIPV6
        */
        rtk_acl_ip_t      ip;
        
        /*for type
         ACL_FIELD_ETHERTYPE
         ACL_FIELD_IPV6_NEXT_HEADER
         ACL_FIELD_TCP_SPORT
         ACL_FIELD_TCP_DPORT
         ACL_FIELD_UDP_SPORT
         ACL_FIELD_UDP_DPORT
         ACL_FIELD_VID_RANGE
         ACL_FIELD_IP_RANGE
         ACL_FIELD_PORT_RANGE
         ACL_FIELD_PKT_LEN_RANGE
         ACL_FIELD_USER_DEFINED00
         ACL_FIELD_USER_DEFINED01
         ACL_FIELD_USER_DEFINED02
         ACL_FIELD_USER_DEFINED03
         ACL_FIELD_USER_DEFINED04
         ACL_FIELD_USER_DEFINED05
         ACL_FIELD_USER_DEFINED06
         ACL_FIELD_USER_DEFINED07
         ACL_FIELD_USER_DEFINED08
         ACL_FIELD_USER_DEFINED09
         ACL_FIELD_USER_DEFINED10
         ACL_FIELD_USER_DEFINED11
         ACL_FIELD_USER_DEFINED12
         ACL_FIELD_USER_DEFINED13
         ACL_FIELD_USER_DEFINED14
         ACL_FIELD_USER_DEFINED15
        */
        /*for range check please use value field to assign range check care entry*/
        rtk_acl_value_t   value;

        /*for user assign field index*/
        /*for type
         ACL_FIELD_PATTERN_MATCH
        */
        rtk_acl_data_field_t   patten;
	} filter_pattern;

    struct rtk_acl_field_t *next;
}rtk_acl_field_t;



typedef struct rtk_acl_raw_field_s
{
    uint16      dataFieldRaw[RTK_ACL_IGR_FIELD_NUMBER];
    uint16      careFieldRaw[RTK_ACL_IGR_FIELD_NUMBER];
} rtk_acl_raw_field_t;


typedef struct rtk_acl_ingress_entry_s
{
    uint16                   index;
    union{
        rtk_acl_field_t      *pFieldHead;
        rtk_acl_raw_field_t  readField;
	} field;
	
	uint16                   templateIdx;
    rtk_acl_care_tag_t       careTag;
    rtk_portmask_t           activePortMask;
    rtk_acl_igr_act_t        act;
    rtk_acl_invert_t         invert;
	rtk_enable_t             valid;
} rtk_acl_ingress_entry_t;


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
extern int32
rtk_acl_init(void);



/* Module Name    : ACL                                  */
/* Sub-module Name: ACL template configuration*/


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
extern int32 
rtk_acl_template_set(rtk_acl_template_t *aclTemplate);


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
extern int32 
rtk_acl_template_get(rtk_acl_template_t *aclTemplate);



/* Function Name:
 *      rtk_acl_field_sel_set
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
rtk_acl_field_sel_set(rtk_acl_field_entry_t *pFieldEntry);

/* Function Name:
 *      rtk_acl_field_sel_get
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
rtk_acl_field_sel_get(rtk_acl_field_entry_t *pFieldEntry);

/* Module Name    : ACL                                  */
/* Sub-module Name: ACL rule configure and modification */

/* Function Name:
 *      rtk_acl_ruleEntry_get
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
rtk_acl_ruleEntry_get(rtk_acl_ingress_entry_t *pAclRule);


/* Function Name:
 *      rtk_acl_igr_field_add
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
extern int32 
rtk_acl_igr_field_add(rtk_acl_ingress_entry_t *pAclRule, rtk_acl_field_t *pAclField);


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
extern int32 
rtk_acl_igrRuleEntry_add(rtk_acl_ingress_entry_t *pAclRule);

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
extern int32 
rtk_acl_igrRuleEntry_del(uint32 index);

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
extern int32 
rtk_acl_igrRuleEntry_delAll(void);


/* Function Name:
 *      rtk_acl_igrUnmatchAction_set
 * Description:
 *      Apply action to packets when no ACL configuration match
 * Input:
 *      port    - Port id.
 * Output:
 *      pAction - Action.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - Invalid port id.
 *      RT_ERR_INPUT        - Invalid input parameters.
 * Note:
 *      This function gets action of packets when no ACL configruation matches.
 */
extern int32 
rtk_acl_igrUnmatchAction_set(rtk_port_t port, rtk_filter_unmatch_action_type_t *pAction);

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
extern int32 
rtk_acl_igrUnmatchAction_get(rtk_port_t port, rtk_filter_unmatch_action_type_t *pAction);


/* Function Name:
 *      rtk_acl_igrState_set
 * Description:
 *      Set state of ingress ACL.
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
rtk_acl_igrState_set(rtk_port_t port, rtk_filter_state_t *pState);


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
extern int32 
rtk_acl_igrState_get(rtk_port_t port, rtk_filter_state_t *pState);


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
extern int32 
rtk_acl_ipRange_set(rtk_acl_rangeCheck_ip_t *pRangeEntry);


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
extern int32 
rtk_acl_ipRange_get(rtk_acl_rangeCheck_ip_t *pRangeEntry);

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
extern int32 
rtk_acl_vidRange_set(rtk_acl_rangeCheck_vid_t *pRangeEntry);

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
extern int32 
rtk_acl_vidRange_get(rtk_acl_rangeCheck_vid_t *pRangeEntry);


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
extern int32 
rtk_acl_portRange_set(rtk_acl_rangeCheck_l4Port_t *pRangeEntry);


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
extern int32 
rtk_acl_portRange_get(rtk_acl_rangeCheck_l4Port_t *pRangeEntry);


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
extern int32 
rtk_acl_packetLengthRange_set(rtk_acl_rangeCheck_pktLength_t *pRangeEntry);


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
extern int32 
rtk_acl_packetLengthRange_get(rtk_acl_rangeCheck_pktLength_t *pRangeEntry);


/* Function Name:
 *      rtk_acl_igr_rule_mode_set
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
rtk_acl_igr_rule_mode_set(rtk_acl_igr_rule_mode_t mode);



/* Function Name:
 *      rtk_acl_igr_rule_mode_get
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
rtk_acl_igr_rule_mode_get(rtk_acl_igr_rule_mode_t *pMode);

#endif /* __RTK_ACL_H__ */

