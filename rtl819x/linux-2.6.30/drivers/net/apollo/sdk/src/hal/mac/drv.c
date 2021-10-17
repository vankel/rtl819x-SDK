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
 * Purpose : mac driver service APIs in the SDK.
 *
 * Feature : mac driver service APIs
 *
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <hal/chipdef/allreg.h>
#include <hal/chipdef/allmem.h>
#include <hal/mac/drv.h>
#include <hal/mac/reg.h>
#include <hal/common/halctrl.h>
#include <ioal/io_mii.h>
#include <ioal/mem32.h>

#ifdef CONFIG_SDK_APOLLO
#include <hal/chipdef/apollo/apollo_reg_struct.h>
#include <dal/apollo/raw/apollo_raw_l2.h>
#endif

#ifdef CONFIG_SDK_APOLLOMP
#include <hal/chipdef/apollomp/rtk_apollomp_reg_struct.h>
#include <dal/apollomp/raw/apollomp_raw_l2.h>
#endif

/*
 * Symbol Definition
 */
#define APOLLO_TBL_BUSY_CHECK_NO            (0xFF)

#ifdef CONFIG_SDK_APOLLO
    #define APOLLO_L34_TABLE_WORD  (3)
    #define APOLLO_L2_TABLE_WORD   (5)
    #define APOLLO_LUT_TABLE_WORD  (4)


    #if defined(CONFIG_SDK_ASICDRV_TEST)
        #define SDK_ASICDRV_TEST_VLAN_SIZE           4096
        #define SDK_ASICDRV_TEST_PPPOE_SIZE          8
    	#define SDK_ASICDRV_TEST_ARP_SIZE            512
    	#define SDK_ASICDRV_TEST_NETIF_SIZE          8
    	#define SDK_ASICDRV_TEST_ROUTE_SIZE          8
    	#define SDK_ASICDRV_TEST_NEXTHOP_SIZE        16
    	#define SDK_ASICDRV_TEST_NAPT_OUTBOUND_SIZE  2048
    	#define SDK_ASICDRV_TEST_NAPT_INBOUND_SIZE   1024
    	#define SDK_ASICDRV_TEST_L34_EXTIP_SIZE      16


    	#define SDK_ASICDRV_TEST_ACLACTION_SIZE  128
        #define SDK_ASICDRV_TEST_CFACTION_SIZE   512
        #define SDK_ASICDRV_TEST_CFTBLENTRY_SIZE   128
        #define SDK_ASICDRV_TEST_L2_SIZE         2048


        vlan_entry_t      ApolloVirtualVlanTable[SDK_ASICDRV_TEST_VLAN_SIZE + 1];

        l34_pppoe_entry_t ApolloVirtualPppoeTable[SDK_ASICDRV_TEST_PPPOE_SIZE + 1];
        l34_netif_entry_t ApolloVirtualExtipTable[SDK_ASICDRV_TEST_L34_EXTIP_SIZE + 1];
        l34_arp_entry_t ApolloVirtualArpTable[SDK_ASICDRV_TEST_ARP_SIZE + 1];
        l34_netif_entry_t ApolloVirtualNetifTable[SDK_ASICDRV_TEST_NETIF_SIZE + 1];
        l34_routing_entry_t ApolloVirtualRouteTable[SDK_ASICDRV_TEST_ROUTE_SIZE + 1];
        l34_nexthop_entry_t ApolloVirtualNexthopTable[SDK_ASICDRV_TEST_NEXTHOP_SIZE + 1];
        l34_napt_outband_entry_t ApolloVirtualNaptOutbandTable[SDK_ASICDRV_TEST_NAPT_OUTBOUND_SIZE + 1];
        l34_napt_inband_entry_t ApolloVirtualNaptInbandTable[SDK_ASICDRV_TEST_NAPT_INBOUND_SIZE + 1];


    	acl_act_entry_t ApolloVirtualAclActTable[SDK_ASICDRV_TEST_ACLACTION_SIZE + 1];
        acl_rule_entry_t ApolloVirtualAclDataTable[SDK_ASICDRV_TEST_CFACTION_SIZE + 1];
    	acl_rule_entry_t ApolloVirtualAclMaskTable[SDK_ASICDRV_TEST_ACLACTION_SIZE + 1];

        cf_act_entry_t ApolloVirtualCfActDsTable[SDK_ASICDRV_TEST_CFACTION_SIZE + 1];
        cf_act_entry_t ApolloVirtualCfActUsTable[SDK_ASICDRV_TEST_CFACTION_SIZE + 1];
        cf_rule_entry_t ApolloVirtualCfDataTable[SDK_ASICDRV_TEST_CFTBLENTRY_SIZE + 1];
    	cf_rule_entry_t ApolloVirtualCfMaskTable[SDK_ASICDRV_TEST_CFTBLENTRY_SIZE + 1];
        l2_lut_entry_t ApolloVirtualLutTable[SDK_ASICDRV_TEST_L2_SIZE + 1];
    #endif
#endif /*CONFIG_SDK_APOLLO*/




#ifdef CONFIG_SDK_APOLLOMP
    #define APOLLOMP_L34_TABLE_WORD  (5)
    #define APOLLOMP_L2_TABLE_WORD   (5)
    #define APOLLOMP_LUT_TABLE_WORD  (4)


    #if defined(CONFIG_SDK_ASICDRV_TEST)
        #define SDK_ASICDRV_APOLLOMP_TEST_VLAN_SIZE           (RTK_VLAN_ID_MAX+1)
        #define SDK_ASICDRV_APOLLOMP_TEST_PPPOE_SIZE          APOLLOMP_L34_PPPOE_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_ARP_SIZE            APOLLOMP_L34_ARP_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_NETIF_SIZE          APOLLOMP_L34_NETIF_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_ROUTE_SIZE          APOLLOMP_L34_ROUTING_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_NEXTHOP_SIZE        APOLLOMP_L34_NH_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_NAPT_OUTBOUND_SIZE  APOLLOMP_L34_NAPT_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_NAPT_INBOUND_SIZE   APOLLOMP_L34_NAPTR_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_L34_EXTIP_SIZE      APOLLOMP_L34_EXTIP_TABLE_MAX
        #define SDK_ASICDRV_APOLLOMP_TEST_ACLACTION_SIZE      APOLLOMP_MAX_NUM_OF_ACL_ACTION
        #define SDK_ASICDRV_APOLLOMP_TEST_CFACTION_SIZE       512
        #define SDK_ASICDRV_APOLLOMP_TEST_CFTBLENTRY_SIZE     128
        #define SDK_ASICDRV_APOLLOMP_TEST_L2_SIZE             2048


        apollomp_vlan_entry_t      ApollompVirtualVlanTable[SDK_ASICDRV_APOLLOMP_TEST_VLAN_SIZE + 1];

        apollomp_l34_pppoe_entry_t ApollompVirtualPppoeTable[SDK_ASICDRV_APOLLOMP_TEST_PPPOE_SIZE + 1];
        apollomp_l34_netif_entry_t ApollompVirtualExtipTable[SDK_ASICDRV_APOLLOMP_TEST_L34_EXTIP_SIZE + 1];
        apollomp_l34_arp_entry_t ApollompVirtualArpTable[SDK_ASICDRV_APOLLOMP_TEST_ARP_SIZE + 1];
        apollomp_l34_netif_entry_t ApollompVirtualNetifTable[SDK_ASICDRV_APOLLOMP_TEST_NETIF_SIZE + 1];
        apollomp_l34_routing_entry_t ApollompVirtualRouteTable[SDK_ASICDRV_APOLLOMP_TEST_ROUTE_SIZE + 1];
        apollomp_l34_nexthop_entry_t ApollompVirtualNexthopTable[SDK_ASICDRV_APOLLOMP_TEST_NEXTHOP_SIZE + 1];
        apollomp_l34_napt_outband_entry_t ApollompVirtualNaptOutbandTable[SDK_ASICDRV_APOLLOMP_TEST_NAPT_OUTBOUND_SIZE + 1];
        apollomp_l34_napt_inband_entry_t ApollompVirtualNaptInbandTable[SDK_ASICDRV_APOLLOMP_TEST_NAPT_INBOUND_SIZE + 1];


    	apollomp_acl_act_entry_t ApollompVirtualAclActTable[SDK_ASICDRV_APOLLOMP_TEST_ACLACTION_SIZE + 1];
        apollomp_acl_rule_entry_t ApollompVirtualAclDataTable[SDK_ASICDRV_APOLLOMP_TEST_CFACTION_SIZE + 1];
    	apollomp_acl_rule_entry_t ApollompVirtualAclMaskTable[SDK_ASICDRV_APOLLOMP_TEST_ACLACTION_SIZE + 1];

        apollomp_cf_act_entry_t ApollompVirtualCfActDsTable[SDK_ASICDRV_APOLLOMP_TEST_CFACTION_SIZE + 1];
        apollomp_cf_act_entry_t ApollompVirtualCfActUsTable[SDK_ASICDRV_APOLLOMP_TEST_CFACTION_SIZE + 1];
        apollomp_cf_rule_entry_t ApollompVirtualCfDataTable[SDK_ASICDRV_APOLLOMP_TEST_CFTBLENTRY_SIZE + 1];
    	apollomp_cf_rule_entry_t ApollompVirtualCfMaskTable[SDK_ASICDRV_APOLLOMP_TEST_CFTBLENTRY_SIZE + 1];
        apollomp_l2_lut_entry_t ApollompVirtualLutTable[SDK_ASICDRV_APOLLOMP_TEST_L2_SIZE + 1];
    #endif
#endif /*CONFIG_SDK_APOLLOMP*/



/*
 * Macro Declaration
 */


/*
 * Function Declaration
 */
#ifdef CONFIG_SDK_APOLLO
    int32 apollo_l34_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 apollo_l34_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 apollo_l2_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 apollo_l2_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 apollo_lut_table_write(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 apollo_lut_table_read(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 lutStToTblData(apollo_lut_table_t *pL2Table, uint32 *pTblData);
    int32 tblDataToLutSt(apollo_lut_table_t *pL2Table, uint32 *pTblData);

    #if defined(CONFIG_SDK_ASICDRV_TEST)
    int32 _apollo_drv_virtualTable_read(
        uint32 table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    int32 _apollo_drv_virtualTable_write(
        uint32  table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    #endif /*defined(CONFIG_SDK_ASICDRV_TEST)*/

#endif /*#ifdef CONFIG_SDK_APOLLO*/


#ifdef CONFIG_SDK_APOLLOMP
    int32 apollomp_l34_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 apollomp_l34_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 apollomp_l2_table_write(uint32 table, rtk_table_t *pTable, uint32  addr,uint32 *pData);
    int32 apollomp_l2_table_read(uint32 table, rtk_table_t *pTable, uint32  addr, uint32 *pData);
    int32 apollomp_lut_table_write(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 apollomp_lut_table_read(rtk_table_t *pTable, uint32 addr, uint32 *pData);
    int32 apollomp_lutStToTblData(apollomp_lut_table_t *pL2Table, uint32 *pTblData);
    int32 apollomp_tblDataToLutSt(apollomp_lut_table_t *pL2Table, uint32 *pTblData);

    #if defined(CONFIG_SDK_ASICDRV_TEST)
    int32 _apollomp_drv_virtualTable_read(
        uint32 table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    int32 _apollomp_drv_virtualTable_write(
        uint32  table,
        rtk_table_t *pTable,
        uint32  addr,
        uint32  *pData);

    #endif /*defined(CONFIG_SDK_ASICDRV_TEST)*/

#endif /*#ifdef CONFIG_SDK_APOLLOMP*/



/* Function Name:
 *      table_find
 * Description:
 *      Find this kind of table structure in this specified chip.
 * Input:
 *      table - table index
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of table structure that found
 * Note:
 *      None
 */
rtk_table_t *
table_find (uint32 table)
{

    return &(hal_ctrl.pChip_driver->pTable_list[table]);
} /* end of table_find */


#if defined(CONFIG_SDK_ASICDRV_TEST)
#ifdef CONFIG_SDK_APOLLO
int32 _apollo_drv_virtualTable_read(
    uint32 table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32 *ptr;
	switch (table)
    {
        case VLANt:
            ptr = (uint32 *)&ApolloVirtualVlanTable[addr];
            break;

		case ACL_ACTIONt:
			ptr = (uint32 *)&ApolloVirtualAclActTable[addr];
            break;

        case ACL_DATAt:
        case ACL_DATA2t:
            ptr = (uint32 *)&ApolloVirtualAclDataTable[addr];
		    break;

		case ACL_MASKt:
        case ACL_MASK2t:
            ptr = (uint32 *)&ApolloVirtualAclMaskTable[addr];
			break;

        case CF_MASKt:
            ptr = (uint32 *)&ApolloVirtualCfMaskTable[addr];
            break;

        case CF_RULEt:
            ptr = (uint32 *)&ApolloVirtualCfDataTable[addr];
            break;

        case CF_ACTION_DSt:
            ptr = (uint32 *)&ApolloVirtualCfActDsTable[addr];
            break;

        case CF_ACTION_USt:
            ptr = (uint32 *)&ApolloVirtualCfActUsTable[addr];
            break;

        case L2_MC_DSLt:
        case L2_UCt:
        case L3_MC_DSLt:
        case L3_MC_ROUTEt:
            ptr = (uint32 *)&ApolloVirtualLutTable[addr];
            break;
        case EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualExtipTable[addr];
			break;

        case PPPOE_TABLEt:
            ptr = (uint32 *)&ApolloVirtualPppoeTable[addr];
			break;

		case ARP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualArpTable[addr];
			break;
		case L3_ROUTING_DROP_TRAPt:
		case L3_ROUTING_GLOBAL_ROUTEt:
		case L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&ApolloVirtualRouteTable[addr];
			break;


		case NAPT_TABLEt:
            ptr =(uint32 *) &ApolloVirtualNaptOutbandTable[addr];
			break;

		case NAPTR_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNaptInbandTable[addr];
			break;

		case NETIFt:
            ptr = (uint32 *)&ApolloVirtualNetifTable[addr];
			break;

		case NEXT_HOP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNexthopTable[addr];
			break;

        default:
            return RT_ERR_INPUT;
    }

    /*copy table array to pData*/
    osal_memcpy(pData,ptr,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;

}

int32 _apollo_drv_virtualTable_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32  *ptr;
    switch(table)
    {
        case VLANt:
            ptr = (uint32 *)&ApolloVirtualVlanTable[addr];
            break;
		case ACL_ACTIONt:
			ptr = (uint32 *)&ApolloVirtualAclActTable[addr];
            break;
		case ACL_DATAt:
        case ACL_DATA2t:
            ptr = (uint32 *)&ApolloVirtualAclDataTable[addr];
		    break;
		case ACL_MASKt:
        case ACL_MASK2t:
            ptr = (uint32 *)&ApolloVirtualAclMaskTable[addr];
			break;
        case CF_MASKt:
            ptr = (uint32 *)&ApolloVirtualCfMaskTable[addr];
            break;
        case CF_RULEt:
            ptr = (uint32 *)&ApolloVirtualCfDataTable[addr];
            break;
        case CF_ACTION_DSt:
            ptr = (uint32 *)&ApolloVirtualCfActDsTable[addr];
            break;
        case CF_ACTION_USt:
            ptr = (uint32 *)&ApolloVirtualCfActUsTable[addr];
            break;
        case PPPOE_TABLEt:
            ptr = (uint32 *)&ApolloVirtualPppoeTable[addr];
            break;
		case EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualExtipTable[addr];
			break;
		case ARP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualArpTable[addr];
			break;
		case L3_ROUTING_DROP_TRAPt:
		case L3_ROUTING_GLOBAL_ROUTEt:
		case L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&ApolloVirtualRouteTable[addr];
			break;
		case NAPT_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNaptOutbandTable[addr];
			break;
		case NAPTR_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNaptInbandTable[addr];
			break;
		case NETIFt:
            ptr = (uint32 *)&ApolloVirtualNetifTable[addr];
			break;
		case NEXT_HOP_TABLEt:
            ptr = (uint32 *)&ApolloVirtualNexthopTable[addr];
			break;
		case L2_MC_DSLt:
        case L2_UCt:
        case L3_MC_DSLt:
        case L3_MC_ROUTEt:
            ptr = (uint32 *)&ApolloVirtualLutTable[addr];
		    break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy pData to table array*/
    osal_memcpy(ptr,pData,pTable->datareg_num*sizeof(uint32));
    return RT_ERR_OK;
}

#endif /*CONFIG_SDK_APOLLO*/

#ifdef CONFIG_SDK_APOLLOMP
int32 _apollomp_drv_virtualTable_read(
    uint32 table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32 *ptr;
	switch (table)
    {
        case APOLLOMP_VLANt:
            ptr = (uint32 *)&ApollompVirtualVlanTable[addr];
            break;
		case APOLLOMP_ACL_ACTION_TABLEt:
			ptr = (uint32 *)&ApollompVirtualAclActTable[addr];
            break;
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
            ptr = (uint32 *)&ApollompVirtualAclDataTable[addr];
		    break;
		case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
            ptr = (uint32 *)&ApollompVirtualAclMaskTable[addr];
			break;
        case APOLLOMP_CF_MASKt:
            ptr = (uint32 *)&ApollompVirtualCfMaskTable[addr];
            break;
        case APOLLOMP_CF_RULEt:
            ptr = (uint32 *)&ApollompVirtualCfDataTable[addr];
            break;
        case APOLLOMP_CF_ACTION_DSt:
            ptr = (uint32 *)&ApollompVirtualCfActDsTable[addr];
            break;
        case APOLLOMP_CF_ACTION_USt:
            ptr = (uint32 *)&ApollompVirtualCfActUsTable[addr];
            break;
        case APOLLOMP_L2_MC_DSLt:
        case APOLLOMP_L2_UCt:
        case APOLLOMP_L3_MC_DSLt:
        case APOLLOMP_L3_MC_ROUTEt:
            ptr = (uint32 *)&ApollompVirtualLutTable[addr];
            break;
        case APOLLOMP_EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualExtipTable[addr];
			break;
        case APOLLOMP_PPPOE_TABLEt:
            ptr = (uint32 *)&ApollompVirtualPppoeTable[addr];
			break;
		case APOLLOMP_ARP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualArpTable[addr];
			break;
		case APOLLOMP_L3_ROUTING_DROP_TRAPt:
		case APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt:
		case APOLLOMP_L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&ApollompVirtualRouteTable[addr];
			break;
		case APOLLOMP_NAPT_TABLEt:
            ptr =(uint32 *) &ApollompVirtualNaptOutbandTable[addr];
			break;
		case APOLLOMP_NAPTR_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNaptInbandTable[addr];
			break;
		case APOLLOMP_NETIFt:
            ptr = (uint32 *)&ApollompVirtualNetifTable[addr];
			break;
		case APOLLOMP_NEXT_HOP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNexthopTable[addr];
			break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy table array to pData*/
    osal_memcpy(pData,ptr,pTable->datareg_num*sizeof(uint32));

    return RT_ERR_OK;
}

int32 _apollomp_drv_virtualTable_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    uint32  *ptr;
    switch(table)
    {
        case APOLLOMP_VLANt:
            ptr = (uint32 *)&ApollompVirtualVlanTable[addr];
            break;
		case APOLLOMP_ACL_ACTION_TABLEt:
			ptr = (uint32 *)&ApollompVirtualAclActTable[addr];
            break;
		case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
            ptr = (uint32 *)&ApollompVirtualAclDataTable[addr];
		    break;
		case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
            ptr = (uint32 *)&ApollompVirtualAclMaskTable[addr];
			break;
        case APOLLOMP_CF_MASKt:
            ptr = (uint32 *)&ApollompVirtualCfMaskTable[addr];
            break;
        case APOLLOMP_CF_RULEt:
            ptr = (uint32 *)&ApollompVirtualCfDataTable[addr];
            break;
        case APOLLOMP_CF_ACTION_DSt:
            ptr = (uint32 *)&ApollompVirtualCfActDsTable[addr];
            break;
        case APOLLOMP_CF_ACTION_USt:
            ptr = (uint32 *)&ApollompVirtualCfActUsTable[addr];
            break;
        case APOLLOMP_PPPOE_TABLEt:
            ptr = (uint32 *)&ApollompVirtualPppoeTable[addr];
            break;
		case APOLLOMP_EXTERNAL_IP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualExtipTable[addr];
			break;
		case APOLLOMP_ARP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualArpTable[addr];
			break;
		case APOLLOMP_L3_ROUTING_DROP_TRAPt:
		case APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt:
		case APOLLOMP_L3_ROUTING_LOCAL_ROUTEt:
            ptr = (uint32 *)&ApollompVirtualRouteTable[addr];
			break;
		case APOLLOMP_NAPT_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNaptOutbandTable[addr];
			break;
		case APOLLOMP_NAPTR_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNaptInbandTable[addr];
			break;
		case APOLLOMP_NETIFt:
            ptr = (uint32 *)&ApollompVirtualNetifTable[addr];
			break;
		case APOLLOMP_NEXT_HOP_TABLEt:
            ptr = (uint32 *)&ApollompVirtualNexthopTable[addr];
			break;
		case APOLLOMP_L2_MC_DSLt:
        case APOLLOMP_L2_UCt:
        case APOLLOMP_L3_MC_DSLt:
        case APOLLOMP_L3_MC_ROUTEt:
            ptr = (uint32 *)&ApollompVirtualLutTable[addr];
		    break;
        default:
            return RT_ERR_INPUT;
    }

    /*copy pData to table array*/
    osal_memcpy(ptr,pData,pTable->datareg_num*sizeof(uint32));
    return RT_ERR_OK;
}

#endif/*CONFIG_SDK_APOLLOMP*/

#endif /*CONFIG_SDK_ASICDRV_TEST*/


#ifdef CONFIG_SDK_APOLLO

static int32 _appolo_l2TableBusy_check(uint32 busyCounter)
{
    uint32      busy;
    int32   ret;
    /*check if table access status*/
    while(busyCounter)
    {
        if ((ret = reg_field_read(TBL_ACCESS_STSr, BUSY_FLAGf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      lutStToTblData
 * Description:
 *      Transfer apollo_lut_table_t structure to table data
 * Input:
 *      pL2Table 	-  table entry structure for filtering database
 * Output:
 *      pTblData      - data for table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 lutStToTblData(apollo_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pTblData == NULL, RT_ERR_NULL_POINTER);


    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_set(L2_UCt, L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI*/
    if ((ret = table_field_set(L2_UCt, L2_UC_FWDPRItf, &pL2Table->lut_pri, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI_EN*/
    if ((ret = table_field_set(L2_UCt, L2_UC_FWDPRI_ENtf, &pL2Table->fwdpri_en, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_set(L2_UCt, L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_set(L2_UCt, L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*--- L2 ---*/
    if (pL2Table->table_type == RAW_LUT_ENTRY_TYPE_L2UC || pL2Table->table_type == RAW_LUT_ENTRY_TYPE_L2MC_DSL)
    {
        /*MAC*/
        if ((ret = table_field_mac_set(L2_UCt, L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*CVID / CVID_FID*/
        if ((ret = table_field_set(L2_UCt, L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_set(L2_UCt, L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

    }

    /*--- L3 ---*/
	if (pL2Table->table_type == RAW_LUT_ENTRY_TYPE_L3MC_DSL || pL2Table->table_type == RAW_LUT_ENTRY_TYPE_L3MC_ROUTE)
    {

        /*GIP*/
        if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*GIP_ONLY*/
        if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_GIP_ONLYtf, &pL2Table->gip_only, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if (pL2Table->table_type != RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_set(L3_MC_DSLt, L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_set(L3_MC_DSLt, L3_MC_DSL_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*DSL_MBR*/
        if ((ret = table_field_set(L3_MC_DSLt, L3_MC_DSL_DSL_MBRtf, &pL2Table->dsl_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }


    }
    switch ( pL2Table->table_type )
    {
        case RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_set(L2_UCt, L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EFID*/
            if ((ret = table_field_set(L2_UCt, L2_UC_EFIDtf, &pL2Table->efid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SAPRI_EN*/
            if ((ret = table_field_set(L2_UCt, L2_UC_SAPRI_ENtf, &pL2Table->sapri_en, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_set(L2_UCt, L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_set(L2_UCt, L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_set(L2_UCt, L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_set(L2_UCt, L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_set(L2_UCt, L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_DSL_SPA*/
            if ((ret = table_field_set(L2_UCt, L2_UC_EXT_DSL_SPAtf, &pL2Table->ext_dsl_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_set(L2_UCt, L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            break;

        case RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_set(L3_MC_DSLt, L3_MC_DSL_SIP_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            break;

        case RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*L3 Translation Index*/
            if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*EXT_FR*/
            if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_EXT_FRtf, &pL2Table->ext_fr, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*WAN_SA*/
            if ((ret = table_field_set(L3_MC_ROUTEt, L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;

    }

    return RT_ERR_OK;
}

/* Function Name:
 *      tblDataToLutSt
 * Description:
 *      Get filtering database entry
 * Input:
 *      pTblData      - data for table
 * Output:
 *      pL2Table 	-  table entry structure for filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 tblDataToLutSt(apollo_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pTblData, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pL2Table->method > RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_get(L2_UCt, L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI*/
    if ((ret = table_field_get(L2_UCt, L2_UC_FWDPRItf, &pL2Table->lut_pri, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI_EN*/
    if ((ret = table_field_get(L2_UCt, L2_UC_FWDPRI_ENtf, &pL2Table->fwdpri_en, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_get(L2_UCt, L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_get(L2_UCt, L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    if (pL2Table->l3lookup==0) /*L2*/
    {

        /*MAC*/
        if ((ret = table_field_mac_get(L2_UCt, L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*CVID / CVID_FID*/
        if ((ret = table_field_get(L2_UCt, L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_get(L2_UCt, L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(pL2Table->mac.octet[0]&0x01)
            pL2Table->table_type = RAW_LUT_ENTRY_TYPE_L2MC_DSL;
        else
            pL2Table->table_type = RAW_LUT_ENTRY_TYPE_L2UC;


    }
    else
    {/*L3*/

        /*GIP*/
        if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*GIP_ONLY*/
        if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_GIP_ONLYtf, &pL2Table->gip_only, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(pL2Table->gip_only)
            pL2Table->table_type = RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
        else
            pL2Table->table_type = RAW_LUT_ENTRY_TYPE_L3MC_DSL;

    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if(pL2Table->table_type != RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_get(L3_MC_DSLt, L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_get(L3_MC_DSLt, L3_MC_DSL_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*DSL_MBR*/
        if ((ret = table_field_get(L3_MC_DSLt, L3_MC_DSL_DSL_MBRtf, &pL2Table->dsl_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }


    }
    switch(pL2Table->table_type)
    {
        case RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_get(L2_UCt, L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EFID*/
            if ((ret = table_field_get(L2_UCt, L2_UC_EFIDtf, &pL2Table->efid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SAPRI_EN*/
            if ((ret = table_field_get(L2_UCt, L2_UC_SAPRI_ENtf, &pL2Table->sapri_en, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_get(L2_UCt, L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_get(L2_UCt, L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_get(L2_UCt, L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_get(L2_UCt, L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_get(L2_UCt, L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_DSL_SPA*/
            if ((ret = table_field_get(L2_UCt, L2_UC_EXT_DSL_SPAtf, &pL2Table->ext_dsl_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_get(L2_UCt, L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            break;


        case RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_get(L3_MC_DSLt, L3_MC_DSL_SIP_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*WAN_SA*/
            if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*L3 Translation Index*/
            if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_FR*/
            if ((ret = table_field_get(L3_MC_ROUTEt, L3_MC_ROUTE_EXT_FRtf, &pL2Table->ext_fr, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

	return RT_ERR_OK;
}


/* Function Name:
 *      apollo_lut_table_read
 * Description:
 *      Read one lut specified table entry by table index or methods.
 * Input:
 *      pTable - the table description
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_lut_table_read(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    apollo_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      cam_or_l2;
    uint32      address;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    uint32      tableData[APOLLO_LUT_TABLE_WORD];
    pLutSt = (apollo_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    osal_memset(tableData,0,sizeof(tableData));
    if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    lutStToTblData((apollo_lut_table_t *)pData, tableData);
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    switch (pLutSt->method)
    {
        case RAW_LUT_READ_METHOD_MAC:
            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                l2_table_data[pTable->datareg_num-i-1] = tableData[i];
            }
            if ((ret = reg_write(TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;

        case RAW_LUT_READ_METHOD_ADDRESS:
	    case RAW_LUT_READ_METHOD_NEXT_ADDRESS:
        case RAW_LUT_READ_METHOD_NEXT_L2UC:
        case RAW_LUT_READ_METHOD_NEXT_L2MC:
        case RAW_LUT_READ_METHOD_NEXT_L3MC:
        case RAW_LUT_READ_METHOD_NEXT_L2L3MC:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        case RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
             /*set spa*/
            field_data = pLutSt->spa;
            if ((ret = reg_field_set(TBL_ACCESS_CTRLr, SPAf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set access methold */
    field_data = pLutSt->method;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data = 0;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if ((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        tableData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


    tblDataToLutSt((apollo_lut_table_t *)pData, tableData);
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(TBL_ACCESS_STSr, HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(TBL_ACCESS_STSr, TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(TBL_ACCESS_STSr, ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
		pLutSt = (apollo_lut_table_t *)pData;
		_apollo_drv_virtualTable_read(L2_UCt,pTable,addr,tableData);
		tblDataToLutSt((apollo_lut_table_t *)pData, tableData);
		pLutSt->lookup_hit = 1;
		pLutSt->lookup_busy = 0;
#endif

    return RT_ERR_OK;
}/* end of apollo_lut_table_read */


/* Function Name:
 *      apollo_l2_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l2_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    int32       i;
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    /*for ACL and CF data and mask use the same table,but different index*/
#if 0
    if (table == ACL_MASKt || table == ACL_MASK2t || table == CF_MASKt)
        addr+= pTable->size;
#endif
    if (table == ACL_DATAt || table == ACL_DATA2t || table == CF_RULEt)
        addr+= pTable->size;



    if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data =0;

    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data =pTable->type;

    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        pData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }



#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollo_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of apollo_l2_table_read */



/* Function Name:
 *      apollo_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    RT_DBG(LOG_DEBUG, (MOD_HAL), "apollo_table_read table=%d, addr=0x%x", table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case ARP_TABLEt:
        case EXTERNAL_IP_TABLEt:
        case L3_ROUTING_DROP_TRAPt:
        case L3_ROUTING_GLOBAL_ROUTEt:
        case L3_ROUTING_LOCAL_ROUTEt:
        case NAPT_TABLEt:
        case NAPTR_TABLEt:
        case NETIFt:
        case NEXT_HOP_TABLEt:
        case PPPOE_TABLEt:
            return apollo_l34_table_read(table, pTable, addr, pData);
            break;

        case L2_MC_DSLt:
        case L2_UCt:
        case L3_MC_DSLt:
        case L3_MC_ROUTEt:
            return apollo_lut_table_read(pTable ,addr ,pData);
            break;

        case ACL_ACTIONt:
        case ACL_DATAt:
        case ACL_DATA2t:
        case ACL_MASKt:
        case ACL_MASK2t:
        case CF_ACTION_DSt:
        case CF_ACTION_USt:
        case CF_MASKt:
        case CF_RULEt:
        case VLANt:
            return apollo_l2_table_read(table, pTable ,addr ,pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
} /* end of apollo_table_read */



/* Function Name:
 *      apollo_l34_table_write
 * Description:
 *      Write one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l34_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[APOLLO_L34_TABLE_WORD];

    if(table);

    /* initialize variable */
    reg_data = 0;
    busy = 0;
    osal_memset(l34_table_data, 0, sizeof(l34_table_data));


     for (i = 0 ; i < APOLLO_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        l34_table_data[APOLLO_L34_TABLE_WORD - i -1] = pData[pTable->datareg_num-i-1];
    }

    /* Write pre-configure table data to indirect data register */
    if ((ret = reg_write(NAT_TBL_ACCESS_WRDATAr, l34_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Table access operation
     */
    field_data =1;

    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, WR_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(NAT_TBL_ACCESS_CTRLr, WR_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);
#if defined(CONFIG_SDK_ASICDRV_TEST)

    _apollo_drv_virtualTable_write(table,pTable,addr,pData);

#endif  /*defined(CONFIG_SDK_ASICDRV_TEST)*/

    return RT_ERR_OK;
}/* end of apollo_l34_table_write */
/* Function Name:
 *      apollo_l34_table_read
 * Description:
 *      Read one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l34_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[APOLLO_L34_TABLE_WORD];
    /* initialize variable */
    reg_data = 0;
    busy = 0;

    if(table);

    osal_memset(l34_table_data, 0, sizeof(l34_table_data));

    /* Table access operation
     */
    field_data = 1;

    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, RD_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(NAT_TBL_ACCESS_CTRLr, ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */

    if ((ret = reg_write(NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(NAT_TBL_ACCESS_CTRLr, RD_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read table data from indirect data register */
    if ((ret = reg_read(NAT_TBL_ACCESS_RDDATAr, l34_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < APOLLO_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        pData[pTable->datareg_num - i - 1]= l34_table_data[APOLLO_L34_TABLE_WORD - i - 1];
    }

#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollo_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of apollo_l34_table_read */



/* Function Name:
 *      apollo_lut_table_write
 * Description:
 *      Write one LUT specified table entry by table index or methods.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_lut_table_write(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    apollo_lut_table_t *pLutSt = NULL;
	int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      tableData[APOLLO_L2_TABLE_WORD];
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    uint32      cam_or_l2;
    uint32      address;
    uint32      i;

    pLutSt = (apollo_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(tableData, 0, sizeof(tableData));
    osal_memset(l2_table_data, 0, sizeof(l2_table_data));
    if ((ret = _appolo_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
         return ret;

    /* transfer data to register data*/

    lutStToTblData((apollo_lut_table_t *)pData, tableData);

    /*write data to TBL_ACCESS_WR_DATA*/
    /* Write table data to indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = tableData[i];
    }


    if ((ret = reg_write(TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold*/
    field_data = pLutSt->method;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;

    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolo_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(TBL_ACCESS_STSr, HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(TBL_ACCESS_STSr, TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(TBL_ACCESS_STSr, ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (apollo_lut_table_t *)pData;
    lutStToTblData((apollo_lut_table_t *)pData, tableData);
    _apollo_drv_virtualTable_write(L2_UCt,pTable,addr,tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */
    return RT_ERR_OK;
}/* end of apollo_lut_table_write */


/* Function Name:
 *      apollo_l2_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l2_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
#if 0
    if (table == ACL_MASKt || table == ACL_MASK2t || table == CF_MASKt)
        addr+= pTable->size;
#endif
    if (table == ACL_DATAt || table == ACL_DATA2t || table == CF_RULEt)
        addr+= pTable->size;


    if ((ret = _appolo_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = pData[i];
    }
    if ((ret = reg_write(TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

    _apollo_drv_virtualTable_write(table,pTable,addr,pData);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */


    return RT_ERR_OK;
}/* end of apollo_l2_table_write */


/* Function Name:
 *      apollo_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
int32
apollo_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case ARP_TABLEt:
        case EXTERNAL_IP_TABLEt:
        case L3_ROUTING_DROP_TRAPt:
        case L3_ROUTING_GLOBAL_ROUTEt:
        case L3_ROUTING_LOCAL_ROUTEt:
        case NAPT_TABLEt:
        case NAPTR_TABLEt:
        case NETIFt:
        case NEXT_HOP_TABLEt:
        case PPPOE_TABLEt:
            return apollo_l34_table_write(table, pTable, addr, pData);
            break;

        case L2_MC_DSLt:
        case L2_UCt:
        case L3_MC_DSLt:
        case L3_MC_ROUTEt:
            return apollo_lut_table_write(pTable, addr, pData);

            break;

        case ACL_ACTIONt:
        case ACL_DATAt:
        case ACL_DATA2t:
        case ACL_MASKt:
        case ACL_MASK2t:
        case CF_ACTION_DSt:
        case CF_ACTION_USt:
        case CF_MASKt:
        case CF_RULEt:
        case VLANt:
            return apollo_l2_table_write(table, pTable, addr, pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;

} /* end of apollo_table_write */


/* Function Name:
 *      apollo_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
apollo_init(void)
{
#if 0 /* use command to do patch, temply. */
    /* Serdes patch */
    apollo_serdes_patch();

    /* GPHY initial */
    ioal_mem32_write(0x148, 0x8);
    ioal_mem32_write(0x94, 0x14);
#endif
    return RT_ERR_OK;
} /* end of apollo_init */




/* Function Name:
 *      apollo_l2_table_clear
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollo_l2_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx)
{
    rtk_table_t *pTable = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      regData,addr;
    uint32      fieldData;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      l2_table_data[APOLLO_L2_TABLE_WORD];
    uint32      startAddr,endAddr;

    switch(table)
    {
        case ACL_ACTIONt:
        case ACL_DATAt:
        case ACL_DATA2t:
        case ACL_MASKt:
        case ACL_MASK2t:
        case CF_ACTION_DSt:
        case CF_ACTION_USt:
        case CF_MASKt:
        case CF_RULEt:
        case VLANt:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((startIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((endIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);


    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == ACL_DATAt || table == ACL_DATA2t || table == CF_RULEt)
    {

        startAddr = startIdx+pTable->size;
        endAddr = endIdx+pTable->size;
    }
    else
    {
        startAddr = startIdx;
        endAddr = endIdx;
    }
    if ((ret = _appolo_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    memset(l2_table_data,0x0,sizeof(l2_table_data));

    if ((ret = reg_write(TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set access methold -- 0b1 with specify lut address*/
    fieldData =1;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ACCESS_METHODf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set table type*/
    fieldData =pTable->type;
    if ((ret = reg_field_set(TBL_ACCESS_CTRLr, TBL_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    for(addr = startAddr; addr<=endAddr; addr++)
    {
        /*set address*/
        if ((ret = reg_field_set(TBL_ACCESS_CTRLr, ADDRf, &addr, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set command type -- 0b1 write*/
        fieldData =1;
        if ((ret = reg_field_set(TBL_ACCESS_CTRLr, CMD_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Write indirect control register to start the write operation */
        if ((ret = reg_write(TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*check if table access status*/
        if((ret = _appolo_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
            return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

        _apollo_drv_virtualTable_write(table,pTable,fieldData,l2_table_data);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */

    }


    return RT_ERR_OK;
}/* end of apollo_l2_table_write */



/* Function Name:
 *      apollo_interPhy_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollo_interPhy_read(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;


    regData = 0;

    /*set phy id and reg address*/
    /*phy id bit 20~16*/
    /*bit 15~0 : ($Page_Addr << 4) + (($Reg_Addr % 8) << 1)]*/
    fieldData = (phyID<<16) | (page<<4) |((phyRegAddr % 8)<<1);

    if ((ret = reg_field_set(GPHY_IND_CMDr, ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(GPHY_IND_CMDr, WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(GPHY_IND_CMDr, CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_read(GPHY_IND_RDr,&regData)) != RT_ERR_OK)
        {
            return ret;
        }
        if ((ret = reg_field_get(GPHY_IND_RDr, BUSYf, &busy, &regData)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_get(GPHY_IND_RDr, RD_DATf ,&fieldData,&regData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (uint16)fieldData;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollo_interPhy_write
 * Description:
 *      Set PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollo_interPhy_write(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;

    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(GPHY_IND_WDr, WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set phy id and reg address*/
    /*phy id bit 20~16*/
    /*bit 15~0 : ($Page_Addr << 4) + (($Reg_Addr % 8) << 1)]*/
    fieldData = (phyID<<16) | (page<<4) |((phyRegAddr % 8)<<1);
    if ((ret = reg_field_set(GPHY_IND_CMDr, ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(GPHY_IND_CMDr, WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(GPHY_IND_CMDr, CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(GPHY_IND_RDr, BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    return RT_ERR_OK;
}


/* Function Name:
 *      apollo_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
apollo_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    uint16 data;
    int32 ret;
    uint32 phyid;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    switch(port)
    {
        case 0:
            phyid = 0;
            break;
        case 1:
            phyid = 1;
            break;
        case 4:
            phyid = 2;
            break;
        case 5:
            phyid = 3;
            break;
        default:
            return RT_ERR_PORT_ID;
            break;
    }

    if(0 == page)
    {
        if(phyReg <= 7)
            real_page = HAL_MIIM_PAGE_ID_MIN();
        else if(phyReg <= 15)
            real_page = HAL_MIIM_PAGE_ID_MIN() + 1;
        else
            real_page = page;
    }
    else
    {
        real_page = page;
    }

    if ((ret = apollo_interPhy_read(phyid, real_page, phyReg, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = data;

    return RT_ERR_OK;
} /* end of apollo_miim_read */


/* Function Name:
 *      apollo_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
apollo_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{
    uint32 phyid;
    int32 ret;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page >= HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    switch(port)
    {
        case 0:
            phyid = 0;
            break;
        case 1:
            phyid = 1;
            break;
        case 4:
            phyid = 2;
            break;
        case 5:
            phyid = 3;
            break;
        default:
            return RT_ERR_PORT_ID;
            break;
    }

    if(0 == page)
    {
        if(phyReg <= 7)
            real_page = HAL_MIIM_PAGE_ID_MIN();
        else if(phyReg <= 15)
            real_page = HAL_MIIM_PAGE_ID_MIN() + 1;
        else
            real_page = page;
    }
    else
    {
        real_page = page;
    }

    if ((ret = apollo_interPhy_write(phyid, real_page, phyReg, (uint16)data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollo_miim_write */



#if 0
#define PATCH_W_OFFSET 0xC000
#define PATCH_R_OFFSET 0x8000

static int32 patch_chk_busy(void)
{
    uint16 value;

    io_mii_phy_reg_read(8, 0, &value);
    while((value&0x8000))
        io_mii_phy_reg_read(8, 0, &value);

    return 0;
}

static void patch_write(uint8 phy_id, uint8 reg, uint16 value)
{
    uint16 data;

    patch_chk_busy();

    io_mii_phy_reg_write(8, 1, value);

    data = (PATCH_W_OFFSET | ((phy_id & 0x1F) << 5) |(reg & 0x1F));
    io_mii_phy_reg_write(8, 0, data);
}

void apollo_serdes_patch(void)
{
#if 0
    osal_printf("apollo_serdes_patch\n\r");
#endif
    /*
    #====================================================================================
    #initial SerDes
    #source RLE0439_TX-RX-2d488G.tcl
    #----------------------------------------------#
    #fine tune TX 16bits to 20 bits function block 622M sample 155M data position
    con_PHYReg w 0x11 0x00 0xA; #force sample clk timing"
    con_PHYReg w 0x11 0x01 0x0100;
    */
    patch_write(0x11, 0x0, 0xA);
    patch_write(0x11, 0x1, 0x0100);

    /*
    #---------------------------------------------------------------------------------
    #setting for jitter transfer---
    con_PHYReg w 0x1d 0x1a 0x0000;   #RX_filter setting(7:0)
    con_PHYReg w 0x1d 0x2 0x2d16;  #kp1==3,ki=1, TX CLK source =RX cdr,disable CMU_TX
    con_PHYReg w 0x1d 0x16 0xa8b2;  #RX_KP1_2=3
    #con_PHYReg w 0x1d 0x16 0xa801;  #Tx clock from CMU.
    con_PHYReg w 0x1d 0x3 0x6041;  #kp2=4
    con_PHYReg w 0x1d 0x18 0xdde4;   #RX_KP2_2=4
    */
    patch_write(0x1d, 0x1a, 0x0000);
    patch_write(0x1d, 0x2, 0x2d16);
    patch_write(0x1d, 0x16, 0xa8b2);
    patch_write(0x1d, 0x3, 0x6041);
    patch_write(0x1d, 0x18, 0xdde4);

    /*
    #----------------------------------------------
    # set best CMU-RX PLL parameter4
    con_PHYReg w 0x1d 0x06 0xf4f0;
    #con_PHYReg w 0x1d 0x07 0x01f7;
    con_PHYReg w 0x1d 0x05 0x4003;
    con_PHYReg w 0x1d 0x0f 0x4fe6;  #TX/RX Io=CML mode
    #con_PHYReg w 0x1d 0x0f 0x4f66;  #TX IO= LVPECL /RX Io=CML mode
    #----------------------------------------------
    #con_PHYReg w 0x1d 0x0 0x5122; # set Fiber 1000 serdes TX internal looback to serdes  RX  (bit8=1)
    #con_PHYReg w 0x1d 0x0 0x5922; # set Fiber 100 serdes TX internal looback to serdes  RX  (bit8=1)

    con_PHYReg w 0x10 1 0xc; #rxd neg edge launch data
    */
    patch_write(0x1d, 0x6, 0xf4f0);
    patch_write(0x1d, 0x5, 0x4003);
    patch_write(0x1d, 0xf, 0x4fe6);
    patch_write(0x10, 0x1, 0xc);
}
#endif


#endif/*CONFIG_SDK_APOLLO*/



#ifdef CONFIG_SDK_APOLLOMP

/*********************************************************/
/*                  APOLLO MP                            */
/*********************************************************/

/* Function Name:
 *      apollomp_init
 * Description:
 *      Initialize the specified settings of the chip.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK     - OK
 *      RT_ERR_FAILED - invalid parameter
 * Note:
 *      None
 */
int32
apollomp_init(void)
{
    return RT_ERR_OK;
} /* end of apollomp_init */



static int32 _appolomp_l2TableBusy_check(uint32 busyCounter)
{
    uint32      busy;
    int32   ret;
    /*check if table access status*/
    while(busyCounter)
    {
        if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_BUSY_FLAGf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
        if(!busy)
            break;

        busyCounter --;
        if(busyCounter == 0)
            return RT_ERR_BUSYWAIT_TIMEOUT;
    }
    return RT_ERR_OK;
}

/* Function Name:
 *      apollomp_lutStToTblData
 * Description:
 *      Transfer apollo_lut_table_t structure to table data
 * Input:
 *      pL2Table 	-  table entry structure for filtering database
 * Output:
 *      pTblData      - data for table
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_lutStToTblData(apollomp_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(pL2Table == NULL, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pTblData == NULL, RT_ERR_NULL_POINTER);


    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FWDPRItf, &pL2Table->lut_pri, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI_EN*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FWDPRI_ENtf, &pL2Table->fwdpri_en, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }

    /*--- L2 ---*/
    if (pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC || pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL)
    {
        /*MAC*/
        if ((ret = table_field_mac_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        /*CVID / CVID_FID*/
        if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

    }

    /*--- L3 ---*/
	if (pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL || pL2Table->table_type == APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE)
    {

        /*GIP*/
        if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*GIP_ONLY*/
        if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_GIP_ONLYtf, &pL2Table->gip_only, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if (pL2Table->table_type != APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_set(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_set(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }

    switch ( pL2Table->table_type )
    {
        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EFID*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_EFIDtf, &pL2Table->efid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SAPRI_EN*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SAPRI_ENtf, &pL2Table->sapri_en, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_DSL_SPA*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_EXT_DSL_SPAtf, &pL2Table->ext_dsl_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_set(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_set(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_SIP_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*L3 Translation Index*/
            if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*EXT_FR*/
            if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_EXT_FRtf, &pL2Table->ext_fr, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }

            /*WAN_SA*/
            if ((ret = table_field_set(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;

    }

    return RT_ERR_OK;
} /*apollomp_lutStToTblData*/

/* Function Name:
 *      apollomp_tblDataToLutSt
 * Description:
 *      Get filtering database entry
 * Input:
 *      pTblData      - data for table
 * Output:
 *      pL2Table 	-  table entry structure for filtering database
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32 apollomp_tblDataToLutSt(apollomp_lut_table_t *pL2Table, uint32 *pTblData)
{
    int32 ret = RT_ERR_FAILED;

    RT_PARAM_CHK(NULL == pL2Table, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(NULL == pTblData, RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(pL2Table->method > APOLLOMP_RAW_LUT_READ_METHOD_END, RT_ERR_INPUT);

    /*--- Common part registers configuration ---*/

    /*L3LOOKUP*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_L3LOOKUPtf, &pL2Table->l3lookup, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FWDPRItf, &pL2Table->lut_pri, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*FWDPRI_EN*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FWDPRI_ENtf, &pL2Table->fwdpri_en, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*NOT_SALEARN*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_NOT_SALEARNtf, &pL2Table->nosalearn, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }
    /*VALID*/
    if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_VALIDtf, &pL2Table->valid, pTblData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_L2|MOD_DAL), "");
        return RT_ERR_FAILED;
    }


    if (pL2Table->l3lookup==0) /*L2*/
    {

        /*MAC*/
        if ((ret = table_field_mac_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_MACtf, (uint8 *)&pL2Table->mac, pTblData)) != RT_ERR_OK)
        {
             RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*CVID / CVID_FID*/
        if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_CVIDtf, &pL2Table->cvid_fid, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*IVL_SVL*/
        if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_IVL_SVLtf, &pL2Table->ivl_svl, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        if(pL2Table->mac.octet[0]&0x01)
            pL2Table->table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL;
        else
            pL2Table->table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC;


    }
    else
    {/*L3*/

        /*GIP*/
        if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_GIPtf, &pL2Table->gip, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*GIP_ONLY*/
        if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_GIP_ONLYtf, &pL2Table->gip_only, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }

        if(pL2Table->gip_only)
            pL2Table->table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE;
        else
            pL2Table->table_type = APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL;

    }


    /*(L2 MC DSL)(L3 MC DSL)(L3 MC ROUTE)*/
    if(pL2Table->table_type != APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC)
    {

        /*MBR*/
        if ((ret = table_field_get(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_MBRtf, &pL2Table->mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
        /*EXT_MBR*/
        if ((ret = table_field_get(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_EXT_MBRtf, &pL2Table->ext_mbr, pTblData)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_L2|MOD_DAL), "");
            return RT_ERR_FAILED;
        }
    }
    switch(pL2Table->table_type)
    {
        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L2UC: /*L2 UC*/
            /*FID*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_FIDtf, &pL2Table->fid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EFID*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_EFIDtf, &pL2Table->efid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SAPRI_EN*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SAPRI_ENtf, &pL2Table->sapri_en, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SPA*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SPAtf, &pL2Table->spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AGE*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_AGEtf, &pL2Table->age, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*AUTH*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_AUTHtf, &pL2Table->auth, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*SA_BLOCK*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_SA_BLKtf, &pL2Table->sa_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*DA_BLOCK*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_DA_BLKtf, &pL2Table->da_block, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_DSL_SPA*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_EXT_DSL_SPAtf, &pL2Table->ext_dsl_spa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*ARP_USED*/
            if ((ret = table_field_get(APOLLOMP_L2_UCt, APOLLOMP_L2_UC_ARP_USAGEtf, &pL2Table->arp_used, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L2MC_DSL:
            break;


        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_DSL:
            /* ---L3 MC DSL---*/
            /*SIP_VID*/
            if ((ret = table_field_get(APOLLOMP_L3_MC_DSLt, APOLLOMP_L3_MC_DSL_SIP_VIDtf, &pL2Table->sip_vid, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;

        case APOLLOMP_RAW_LUT_ENTRY_TYPE_L3MC_ROUTE:
            /* ---L3 MC ROUTE---*/
            /*WAN_SA*/
            if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_WAN_SAtf, &pL2Table->wan_sa, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*L3 Translation Index*/
            if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_L3_IDXtf, &pL2Table->l3_idx, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            /*EXT_FR*/
            if ((ret = table_field_get(APOLLOMP_L3_MC_ROUTEt, APOLLOMP_L3_MC_ROUTE_EXT_FRtf, &pL2Table->ext_fr, pTblData)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_L2|MOD_DAL), "");
                return RT_ERR_FAILED;
            }
            break;
        default:
            return RT_ERR_FAILED;
    }

	return RT_ERR_OK;
}/*apollomp_tblDataToLutSt*/


/* Function Name:
 *      apollomp_lut_table_read
 * Description:
 *      Read one lut specified table entry by table index or methods.
 * Input:
 *      pTable - the table description
 *      addr    - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_lut_table_read(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    apollomp_lut_table_t *pLutSt = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      cam_or_l2;
    uint32      address;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;

    uint32      tableData[APOLLOMP_LUT_TABLE_WORD];
    pLutSt = (apollomp_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    osal_memset(tableData,0,sizeof(tableData));
    if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    apollomp_lutStToTblData((apollomp_lut_table_t *)pData, tableData);
    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    switch (pLutSt->method)
    {
        case APOLLOMP_RAW_LUT_READ_METHOD_MAC:
            for (i = 0 ; i < pTable->datareg_num ; i++)
            {
                l2_table_data[pTable->datareg_num-i-1] = tableData[i];
            }
            if ((ret = reg_write(APOLLOMP_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;

        case APOLLOMP_RAW_LUT_READ_METHOD_ADDRESS:
	    case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_ADDRESS:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2UC:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2MC:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L3MC:
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2L3MC:
            /*set address*/
            field_data = addr;
            if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        case APOLLOMP_RAW_LUT_READ_METHOD_NEXT_L2UCSPA:
             /*set spa*/
            field_data = pLutSt->spa;
            if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_SPAf, &field_data, &reg_data)) != RT_ERR_OK)
            {
                return ret;
            }
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /*set access methold */
    field_data = pLutSt->method;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data = 0;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data = pTable->type;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if ((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        tableData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }


    apollomp_tblDataToLutSt((apollomp_lut_table_t *)pData, tableData);
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;
#if defined(CONFIG_SDK_ASICDRV_TEST)
		pLutSt = (apollomp_lut_table_t *)pData;
		_apollomp_drv_virtualTable_read(APOLLOMP_L2_UCt,pTable,addr,tableData);
		apollomp_tblDataToLutSt((apollomp_lut_table_t *)pData, tableData);
		pLutSt->lookup_hit = 1;
		pLutSt->lookup_busy = 0;
#endif

    return RT_ERR_OK;
}/* end of apollomp_lut_table_read */




/* Function Name:
 *      apollomp_lut_table_write
 * Description:
 *      Write one LUT specified table entry by table index or methods.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_lut_table_write(
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{
    apollomp_lut_table_t *pLutSt = NULL;
	int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      tableData[APOLLOMP_L2_TABLE_WORD];
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      cam_or_l2;
    uint32      address;
    uint32      i;

    pLutSt = (apollomp_lut_table_t *)pData;
    /*busyCounter = pLutSt->wait_time;*/
    pLutSt->lookup_hit = 0;
    pLutSt->lookup_busy = 1;
    osal_memset(tableData, 0, sizeof(tableData));
    osal_memset(l2_table_data, 0, sizeof(l2_table_data));
    if ((ret = _appolomp_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
         return ret;

    /* transfer data to register data*/

    apollomp_lutStToTblData((apollomp_lut_table_t *)pData, tableData);

    /*write data to TBL_ACCESS_WR_DATA*/
    /* Write table data to indirect data register */

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = tableData[i];
    }


    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold*/
    field_data = pLutSt->method;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;

    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }
    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolomp_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;
    pLutSt->lookup_busy = 0;
    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_HIT_STATUSf, &pLutSt->lookup_hit)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_TYPEf, &cam_or_l2)) != RT_ERR_OK)
    {
        return ret;
    }

    if ((ret = reg_field_read(APOLLOMP_TBL_ACCESS_STSr, APOLLOMP_ADDRf, &address)) != RT_ERR_OK)
    {
        return ret;
    }

    pLutSt->address = (cam_or_l2 << 11) | address;

#if defined(CONFIG_SDK_ASICDRV_TEST)
    pLutSt = (apollomp_lut_table_t *)pData;
    apollomp_lutStToTblData((apollomp_lut_table_t *)pData, tableData);
    _apollomp_drv_virtualTable_write(APOLLOMP_L2_UCt,pTable,addr,tableData);
    pLutSt->lookup_hit = 1;
    pLutSt->lookup_busy = 0;
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */
    return RT_ERR_OK;
}/* end of apollomp_lut_table_write */




/* Function Name:
 *      apollomp_l34_table_write
 * Description:
 *      Write one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l34_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[APOLLOMP_L34_TABLE_WORD];

    if(table);

    /* initialize variable */
    reg_data = 0;
    busy = 0;
    osal_memset(l34_table_data, 0, sizeof(l34_table_data));


     for (i = 0 ; i < APOLLOMP_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        l34_table_data[i] = pData[pTable->datareg_num-i-1];
    }

    /* Write pre-configure table data to indirect data register */
    if ((ret = reg_write(APOLLOMP_NAT_TBL_ACCESS_WRDATAr, l34_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Table access operation
     */
    field_data =1;

    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_WR_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(APOLLOMP_NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_WR_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);
#if defined(CONFIG_SDK_ASICDRV_TEST)

    _apollomp_drv_virtualTable_write(table,pTable,addr,pData);

#endif  /*defined(CONFIG_SDK_ASICDRV_TEST)*/

    return RT_ERR_OK;
}/* end of apollomp_l34_table_write */


/* Function Name:
 *      apollomp_l34_table_read
 * Description:
 *      Read one L34 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l34_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    uint32      reg_data,field_data;
    uint32      busy;
    uint32      i;
    int32       ret = RT_ERR_FAILED;
    uint32      l34_table_data[APOLLOMP_L34_TABLE_WORD];
    /* initialize variable */
    reg_data = 0;
    busy = 0;

    if(table);

    osal_memset(l34_table_data, 0, sizeof(l34_table_data));

    /* Table access operation
     */
    field_data = 1;

    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_RD_EXEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* access table type */
    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_TBL_IDXf, (uint32 *)&(pTable->type), &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Select access address of the table */
    if ((ret = reg_field_set(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_ETRY_IDXf, &addr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */

    if ((ret = reg_write(APOLLOMP_NAT_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_NAT_TBL_ACCESS_CTRLr, APOLLOMP_RD_EXEf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read table data from indirect data register */
    if ((ret = reg_read(APOLLOMP_NAT_TBL_ACCESS_RDDATAr, l34_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < APOLLOMP_L34_TABLE_WORD ; i++)
    {
        if(i >= pTable->datareg_num)
            break;
        pData[pTable->datareg_num - i - 1]= l34_table_data[i];
    }

#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollomp_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of apollomp_l34_table_read */



/* Function Name:
 *      apollomp_l2_table_write
 * Description:
 *      Write one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l2_table_write(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      i;
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == APOLLOMP_ACL_DATAt || table == APOLLOMP_ACL_DATA2t || table == APOLLOMP_CF_RULEt)
        addr+= pTable->size;


    if ((ret = _appolomp_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        l2_table_data[pTable->datareg_num-i-1] = pData[i];
    }
    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b1 write*/
    field_data =1;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type*/
    field_data =pTable->type;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the write operation */
    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

    _apollomp_drv_virtualTable_write(table,pTable,addr,pData);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */


    return RT_ERR_OK;
}/* end of apollomp_l2_table_write */

/* Function Name:
 *      apollomp_l2_table_read
 * Description:
 *      Read one L2 specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l2_table_read(
    uint32  table,
    rtk_table_t *pTable,
    uint32  addr,
    uint32  *pData)
{

    int32       ret = RT_ERR_FAILED;
    int32       i;
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      reg_data;
    uint32      field_data;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    osal_memset(l2_table_data,0,sizeof(l2_table_data));
    /*for ACL and CF data and mask use the same table,but different index*/
    if (table == APOLLOMP_ACL_DATAt || table == APOLLOMP_ACL_DATA2t || table == APOLLOMP_CF_RULEt)
        addr+= pTable->size;

    if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set address*/
    field_data = addr;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }


    /*set access methold -- 0b1 with specify lut address*/
    field_data =1;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set command type -- 0b0 read*/
    field_data =0;

    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*set table type */
    field_data =pTable->type;

    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &field_data, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Write indirect control register to start the read operation */
    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &reg_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /*check if table access status*/
    if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
        return ret;

    /*read data from TBL_ACCESS_RD_DATA*/
    /* Read table data from indirect data register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_RD_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    for (i = 0 ; i < pTable->datareg_num ; i++)
    {
        pData[pTable->datareg_num - i - 1]= l2_table_data[i];
    }



#if defined(CONFIG_SDK_ASICDRV_TEST)
    _apollomp_drv_virtualTable_read(table,pTable,addr,pData);
#endif

    return RT_ERR_OK;
}/* end of apollomp_l2_table_read */





/* Function Name:
 *      apollomp_l2_table_clear
 * Description:
 *      clear L2 specified table entry by table index range.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_l2_table_clear(
    uint32  table,
    uint32  startIdx,
    uint32  endIdx)
{
    rtk_table_t *pTable = NULL;
    int32       ret = RT_ERR_FAILED;
    uint32      regData,addr;
    uint32      fieldData;
    uint32      busyCounter = APOLLO_TBL_BUSY_CHECK_NO;
    uint32      l2_table_data[APOLLOMP_L2_TABLE_WORD];
    uint32      startAddr,endAddr;

    switch(table)
    {
        case APOLLOMP_ACL_ACTION_TABLEt:
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
        case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
        case APOLLOMP_CF_ACTION_DSt:
        case APOLLOMP_CF_ACTION_USt:
        case APOLLOMP_CF_MASKt:
        case APOLLOMP_CF_RULEt:
        case APOLLOMP_VLANt:
            break;
        default:
            return RT_ERR_INPUT;
            break;
    }

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((startIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((endIdx >= pTable->size), RT_ERR_OUT_OF_RANGE);


    /*for ACL and CF data and mask use the same table,but different index*/
    /*ACL MASK*/
    if (table == APOLLOMP_ACL_DATAt || table == APOLLOMP_ACL_DATA2t || table == APOLLOMP_CF_RULEt)
    {

        startAddr = startIdx+pTable->size;
        endAddr = endIdx+pTable->size;
    }
    else
    {
        startAddr = startIdx;
        endAddr = endIdx;
    }
    if ((ret = _appolomp_l2TableBusy_check(busyCounter)) != RT_ERR_OK)
        return ret;


    /*write data to TBL_ACCESS_WR_DATA*/
    /*Write table data to indirect data register */
    memset(l2_table_data,0x0,sizeof(l2_table_data));

    if ((ret = reg_write(APOLLOMP_TBL_ACCESS_WR_DATAr, l2_table_data)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Read TBL_ACCESS_CTRL register */
    if ((ret = reg_read(APOLLOMP_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set access methold -- 0b1 with specify lut address*/
    fieldData =1;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ACCESS_METHODf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }



    /*set table type*/
    fieldData =pTable->type;
    if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_TBL_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    for(addr = startAddr; addr<=endAddr; addr++)
    {
        /*set address*/
        if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_ADDRf, &addr, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*set command type -- 0b1 write*/
        fieldData =1;
        if ((ret = reg_field_set(APOLLOMP_TBL_ACCESS_CTRLr, APOLLOMP_CMD_TYPEf, &fieldData, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /* Write indirect control register to start the write operation */
        if ((ret = reg_write(APOLLOMP_TBL_ACCESS_CTRLr, &regData)) != RT_ERR_OK)
        {
            return ret;
        }

        /*check if table access status*/
        if((ret = _appolomp_l2TableBusy_check(busyCounter))!=RT_ERR_OK)
            return ret;

#if defined(CONFIG_SDK_ASICDRV_TEST)

        _apollomp_drv_virtualTable_write(table,pTable,fieldData,l2_table_data);
#endif/* defined(CONFIG_SDK_ASICDRV_TEST) */

    }


    return RT_ERR_OK;
}/* end of apollomp_l2_table_clear */





/* Function Name:
 *      apollomp_table_write
 * Description:
 *      Write one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 *      pData - pointer buffer of table entry data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 *      1. The addr argument of RTL8389 PIE table is not continuous bits from
 *         LSB bits, we do one compiler option patch for this.
 *      2. If you don't use the RTL8389 chip, please turn off the "RTL8389"
 *         definition symbol, then performance will be improved.
 */
int32
apollomp_table_write(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);

    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);
    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case APOLLOMP_ARP_TABLEt:
        case APOLLOMP_EXTERNAL_IP_TABLEt:
        case APOLLOMP_L3_ROUTING_DROP_TRAPt:
        case APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt:
        case APOLLOMP_L3_ROUTING_LOCAL_ROUTEt:
        case APOLLOMP_NAPT_TABLEt:
        case APOLLOMP_NAPTR_TABLEt:
        case APOLLOMP_NETIFt:
        case APOLLOMP_NEXT_HOP_TABLEt:
        case APOLLOMP_PPPOE_TABLEt:
        case APOLLOMP_WAN_TYPE_TABLEt:
        case APOLLOMP_NEIGHBOR_TABLEt:
        case APOLLOMP_IPV6_ROUTING_TABLEt:
        case APOLLOMP_BINDING_TABLEt:
            return apollomp_l34_table_write(table, pTable, addr, pData);
            break;

        case APOLLOMP_L2_MC_DSLt:
        case APOLLOMP_L2_UCt:
        case APOLLOMP_L3_MC_DSLt:
        case APOLLOMP_L3_MC_ROUTEt:
            return apollomp_lut_table_write(pTable, addr, pData);

            break;

        case APOLLOMP_ACL_ACTION_TABLEt:
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
        case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
        case APOLLOMP_CF_ACTION_DSt:
        case APOLLOMP_CF_ACTION_USt:
        case APOLLOMP_CF_MASKt:
        case APOLLOMP_CF_RULEt:
        case APOLLOMP_VLANt:
            return apollomp_l2_table_write(table, pTable, addr, pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}



/* Function Name:
 *      apollomp_table_read
 * Description:
 *      Read one specified table entry by table index.
 * Input:
 *      table - table index
 *      addr  - entry address of the table
 * Output:
 *      pData - pointer buffer of table entry data
 * Return:
 *      RT_ERR_OK                 - OK
 *      RT_ERR_FAILED             - Failed
 *      RT_ERR_OUT_OF_RANGE       - input parameter out of range
 *      RT_ERR_CHIP_NOT_SUPPORTED - functions not supported by this chip model
 *      RT_ERR_INPUT              - invalid input parameter
 * Note:
 */
int32
apollomp_table_read(
    uint32  table,
    uint32  addr,
    uint32  *pData)
{
    rtk_table_t *pTable = NULL;
    RT_DBG(LOG_DEBUG, (MOD_HAL), "apollomp_table_read table=%d, addr=0x%x", table, addr);

    /* parameter check */
    RT_PARAM_CHK((table >= HAL_GET_MAX_TABLE_IDX()), RT_ERR_OUT_OF_RANGE);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    pTable = table_find(table);
    /* NULL means the table is not supported in this chip unit */
    RT_PARAM_CHK((NULL == pTable), RT_ERR_CHIP_NOT_SUPPORTED);
    RT_PARAM_CHK((addr >= pTable->size), RT_ERR_OUT_OF_RANGE);

    switch(table)
    {
        case APOLLOMP_ARP_TABLEt:
        case APOLLOMP_EXTERNAL_IP_TABLEt:
        case APOLLOMP_L3_ROUTING_DROP_TRAPt:
        case APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt:
        case APOLLOMP_L3_ROUTING_LOCAL_ROUTEt:
        case APOLLOMP_NAPT_TABLEt:
        case APOLLOMP_NAPTR_TABLEt:
        case APOLLOMP_NETIFt:
        case APOLLOMP_NEXT_HOP_TABLEt:
        case APOLLOMP_PPPOE_TABLEt:
        case APOLLOMP_WAN_TYPE_TABLEt:
        case APOLLOMP_NEIGHBOR_TABLEt:
        case APOLLOMP_IPV6_ROUTING_TABLEt:
        case APOLLOMP_BINDING_TABLEt:
            return apollomp_l34_table_read(table, pTable, addr, pData);
            break;

        case APOLLOMP_L2_MC_DSLt:
        case APOLLOMP_L2_UCt:
        case APOLLOMP_L3_MC_DSLt:
        case APOLLOMP_L3_MC_ROUTEt:
            return apollomp_lut_table_read(pTable ,addr ,pData);
            break;

        case APOLLOMP_ACL_ACTION_TABLEt:
        case APOLLOMP_ACL_DATAt:
        case APOLLOMP_ACL_DATA2t:
        case APOLLOMP_ACL_MASKt:
        case APOLLOMP_ACL_MASK2t:
        case APOLLOMP_CF_ACTION_DSt:
        case APOLLOMP_CF_ACTION_USt:
        case APOLLOMP_CF_MASKt:
        case APOLLOMP_CF_RULEt:
        case APOLLOMP_CF_MASK_L34t:
        case APOLLOMP_CF_RULE_L34t:
        case APOLLOMP_VLANt:
            return apollomp_l2_table_read(table, pTable ,addr ,pData);
            break;

        default:
            return RT_ERR_INPUT;
            break;
    }

    return RT_ERR_INPUT;
}




/* Function Name:
 *      apollomp_interPhy_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollomp_interPhy_read(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      *pData)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;


    regData = 0;

    /*set phy id and reg address*/
    /*phy id bit 20~16*/
    /*bit 15~0 : ($Page_Addr << 4) + (($Reg_Addr % 8) << 1)]*/
    fieldData = (phyID<<16) | (page<<4) |((phyRegAddr % 8)<<1);

    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to read*/
    fieldData = 0;
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(APOLLOMP_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_GPHY_IND_RDr, APOLLOMP_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    /* Read data register */
    if ((ret = reg_field_read(APOLLOMP_GPHY_IND_RDr, APOLLOMP_RD_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = (uint16)fieldData;

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_interPhy_write
 * Description:
 *      Set PHY registers from apollo family chips.
 * Input:
 *      phyID      - PHY id
 *      page       - page number
 *      phyRegAddr - PHY register
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT        - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
*/
int32
apollomp_interPhy_write(
    uint32      phyID,
    uint32      page,
    uint32      phyRegAddr,
    uint16      data)
{
    uint32 regData,fieldData;
    int32 ret;
    uint32      busy;

    /*write data to write buffer*/
    fieldData = data;
    if ((ret = reg_field_write(APOLLOMP_GPHY_IND_WDr, APOLLOMP_WR_DATf ,&fieldData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set phy id and reg address*/
    /*phy id bit 20~16*/
    /*bit 15~0 : ($Page_Addr << 4) + (($Reg_Addr % 8) << 1)]*/
    fieldData = (phyID<<16) | (page<<4) |((phyRegAddr % 8)<<1);
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_ADRf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD to write*/
    fieldData = 1;
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_WRENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*set CMD enable*/
    fieldData = 1;
    if ((ret = reg_field_set(APOLLOMP_GPHY_IND_CMDr, APOLLOMP_CMD_ENf, &fieldData, &regData)) != RT_ERR_OK)
    {
        return ret;
    }
    /*write to register*/
    if ((ret = reg_write(APOLLOMP_GPHY_IND_CMDr, &regData)) != RT_ERR_OK)
    {
        return ret;
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_GPHY_IND_RDr, APOLLOMP_BUSYf, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    return RT_ERR_OK;
}



/* Function Name:
 *      apollomp_miim_read
 * Description:
 *      Get PHY registers from apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 * Output:
 *      pData   - pointer buffer of read data
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 *      RT_ERR_NULL_POINTER - input parameter is null pointer
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
apollomp_miim_read(
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      *pData)
{
    uint16 data;
    int32 ret;
    uint32 phyid;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    switch(port)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            phyid = port;
            break;
        default:
            return RT_ERR_PORT_ID;
            break;
    }

    if(0 == page)
    {
        if(phyReg <= 7)
            real_page = HAL_MIIM_PAGE_ID_MIN();
        else if(phyReg <= 15)
            real_page = HAL_MIIM_PAGE_ID_MIN() + 1;
        else
            real_page = page;
    }
    else
    {
        real_page = page;
    }

    if ((ret = apollomp_interPhy_read(phyid, real_page, phyReg, &data)) != RT_ERR_OK)
    {
        return ret;
    }

    *pData = data;

    return RT_ERR_OK;
} /* end of apollomp_miim_read */


/* Function Name:
 *      apollomp_miim_write
 * Description:
 *      Set PHY registers in apollo family chips.
 * Input:
 *      port    - port id
 *      page    - PHY page
 *      phy_reg - PHY register
 *      data    - Read data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid PHY page id
 *      RT_ERR_PHY_REG_ID   - invalid PHY reg id
 * Note:
 *      1. port valid range is 0 ~ 5
 *      2. page valid range is 0 ~ 31
 *      3. phy_reg valid range is 0 ~ 31
 */
int32
apollomp_miim_write (
    rtk_port_t  port,
    uint32      page,
    uint32      phyReg,
    uint32      data)
{
    uint32 phyid;
    int32 ret;
    uint32 real_page;

    RT_PARAM_CHK((!HAL_IS_ETHER_PORT(port)), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page >= HAL_MIIM_PAGE_ID_MAX()), RT_ERR_PHY_PAGE_ID);
    RT_PARAM_CHK((phyReg >= PHY_REG_MAX), RT_ERR_PHY_REG_ID);

    switch(port)
    {
        case 0:
        case 1:
        case 2:
        case 3:
            phyid = port;
            break;
        default:
            return RT_ERR_PORT_ID;
            break;
    }

    if(0 == page)
    {
        if(phyReg <= 7)
            real_page = HAL_MIIM_PAGE_ID_MIN();
        else if(phyReg <= 15)
            real_page = HAL_MIIM_PAGE_ID_MIN() + 1;
        else
            real_page = page;
    }
    else
    {
        real_page = page;
    }

    if ((ret = apollomp_interPhy_write(phyid, real_page, phyReg, (uint16)data)) != RT_ERR_OK)
    {
        return ret;
    }

    return RT_ERR_OK;
} /* end of apollomp_miim_write */

#endif/*CONFIG_SDK_APOLLOMP*/




