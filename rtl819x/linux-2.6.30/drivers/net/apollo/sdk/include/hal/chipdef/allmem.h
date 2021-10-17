/*
 * Copyright (C) 2009 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * $Revision: 8999 $
 * $Date: 2010-04-12 15:42:36 +0800 (?Ÿæ?ä¸€, 12 ?›æ? 2010) $
 *
 * Purpose : chip table and its field definition in the SDK.
 *
 * Feature : chip table and its field definition
 *
 */

#ifndef __HAL_CHIPDEF_ALLMEM_H__
#define __HAL_CHIPDEF_ALLMEM_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Data Type Declaration
 */
typedef struct rtk_tableField_s
{
    unsigned short lsp;             /* LSP of the field */
    unsigned short len;             /* field length */
} rtk_tableField_t;

typedef struct rtk_table_s
{
    unsigned int type;              /* access table type */
    unsigned int size;              /* table size */
    unsigned int datareg_num;       /* total data registers */
    unsigned int field_num;         /* total field numbers */
    rtk_tableField_t *fields;       /* table fields */
} rtk_table_t;

/* indirect control group enum definition */
typedef enum rtk_indirectCtrlGroup_e
{
    INDIRECT_CTRL_GROUP_TABLE, 
    RTK_INDIRECT_CTRL_GROUP_END
} rtk_indirectCtrlGroup_t;




/*
 * Macro Definition
 */

/* Declaration the size of table entry
 * Each structure is a word array that size is the maximum in all supported chips.
 */
#define MEM_ENTRY_DECLARE(name, words)\
    typedef struct {\
        uint32 entry_data[words];\
    } name

/* structure is word array that have the maximum value of all supported chips. */
#ifdef CONFIG_SDK_APOLLO 
    /*L2 table entry*/
    MEM_ENTRY_DECLARE(l2_entry_t, 4);
    /*Vlan table entry*/
    MEM_ENTRY_DECLARE(vlan_entry_t, 2);
    /*L34 table entry*/
    MEM_ENTRY_DECLARE(l34_pppoe_entry_t, 1);
    MEM_ENTRY_DECLARE(l34_arp_entry_t, 1);
    MEM_ENTRY_DECLARE(l34_netif_entry_t, 3);
    MEM_ENTRY_DECLARE(l34_routing_entry_t, 2);
    MEM_ENTRY_DECLARE(l34_nexthop_entry_t, 1);
    MEM_ENTRY_DECLARE(l34_extip_entry_t, 3);
    MEM_ENTRY_DECLARE(l34_napt_inband_entry_t, 3);
    MEM_ENTRY_DECLARE(l34_napt_outband_entry_t, 1);
    MEM_ENTRY_DECLARE(l34_hsa_entry_t, 4);
    MEM_ENTRY_DECLARE(l34_hsb_entry_t, 7);
    MEM_ENTRY_DECLARE(acl_act_entry_t, 2);
    MEM_ENTRY_DECLARE(acl_rule_entry_t, 5);
    MEM_ENTRY_DECLARE(cf_act_entry_t, 2);
    MEM_ENTRY_DECLARE(cf_rule_entry_t, 5);
    MEM_ENTRY_DECLARE(l2_lut_entry_t, 4);
    MEM_ENTRY_DECLARE(hsb_entry_t, 20);
    MEM_ENTRY_DECLARE(hsa_entry_t, 13);
    MEM_ENTRY_DECLARE(hsd_entry_t, 16);
#endif

#ifdef CONFIG_SDK_APOLLOMP 
    /*L2 table entry*/
    MEM_ENTRY_DECLARE(apollomp_l2_entry_t, 3);
    /*LUT entry*/
    MEM_ENTRY_DECLARE(apollomp_l2_lut_entry_t, 3);
    /*Vlan table entry*/
    MEM_ENTRY_DECLARE(apollomp_vlan_entry_t, 2);
    /*ACL table entry*/
    MEM_ENTRY_DECLARE(apollomp_acl_act_entry_t, 2);
    MEM_ENTRY_DECLARE(apollomp_acl_rule_entry_t, 5);
    /*classification table entry*/
    MEM_ENTRY_DECLARE(apollomp_cf_act_entry_t, 2);
    MEM_ENTRY_DECLARE(apollomp_cf_rule_entry_t, 5);
    MEM_ENTRY_DECLARE(apollomp_hsb_entry_t, 20);
    MEM_ENTRY_DECLARE(apollomp_hsa_entry_t, 13);
    MEM_ENTRY_DECLARE(apollomp_hsd_entry_t, 16);

    /*L34 table entry*/
    MEM_ENTRY_DECLARE(apollomp_l34_pppoe_entry_t, 1);
    MEM_ENTRY_DECLARE(apollomp_l34_arp_entry_t, 1);
    MEM_ENTRY_DECLARE(apollomp_l34_netif_entry_t, 3);
    MEM_ENTRY_DECLARE(apollomp_l34_routing_entry_t, 2);
    MEM_ENTRY_DECLARE(apollomp_l34_nexthop_entry_t, 1);
    MEM_ENTRY_DECLARE(apollomp_l34_extip_entry_t, 3);
    MEM_ENTRY_DECLARE(apollomp_l34_napt_inband_entry_t, 3);
    MEM_ENTRY_DECLARE(apollomp_l34_napt_outband_entry_t, 1);
    MEM_ENTRY_DECLARE(apollomp_l34_ipv6_routing_entry_t, 5);
    MEM_ENTRY_DECLARE(apollomp_l34_ipv6_neighbor_entry_t, 3);
    MEM_ENTRY_DECLARE(apollomp_l34_binding_entry_t, 3);
    MEM_ENTRY_DECLARE(apollomp_l34_wan_type_entry_t, 3);

    MEM_ENTRY_DECLARE(apollomp_l34_hsa_entry_t, 4);
    MEM_ENTRY_DECLARE(apollomp_l34_hsb_entry_t, 10);
#endif




#endif  /* __HAL_CHIPDEF_ALLMEM_H__ */
