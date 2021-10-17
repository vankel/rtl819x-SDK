 /*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 */

/*
 * Include Files
 */
#include <dal/apollomp/dal_apollomp.h>

#include <rtk/l34.h>
#include <dal/apollomp/dal_apollomp_l34.h>
#include <dal/apollomp/raw/apollomp_raw.h>
#include <dal/apollomp/raw/apollomp_raw_l34.h>
/*
 * Symbol Definition
 */



/*
 * Data Declaration
 */
static uint32               l34_init = {INIT_NOT_COMPLETED};


/*
 * Macro Definition
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_apollomp_l34_init
 * Description:
 *      Initialize l34 module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize l34 module before calling any l34 APIs.
 */
int32
dal_apollomp_l34_init(void)
{
    l34_init = INIT_COMPLETED;
    return RT_ERR_OK;
} /* end of dal_apollomp_l34_init */





/* Function Name:
 *      dal_apollomp_l34_netifTable_set
 * Description:
 *      Set netif entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_ENTRY_INDEX
 *      RT_ERR_VLAN_VID
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *entry)
{
    int32 ret;
    uint32 tempVal;

    apollomp_l34_netif_entry_t netif_entry;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((RTK_VLAN_ID_MAX < entry->vlan_id), RT_ERR_VLAN_VID);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->enable_rounting != 0 && entry->enable_rounting != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->mtu >= 16384), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->mac_mask != 0 && entry->mac_mask != 4 && entry->mac_mask != 6 && entry->mac_mask != 7), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&netif_entry, 0x0, sizeof(netif_entry));


    tempVal = entry->valid;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_VALIDtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tempVal = entry->mtu;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_MTUtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tempVal = entry->enable_rounting;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_ENRTRtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tempVal = entry->mac_mask;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_MACMASKtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_mac_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_GMACtf, (uint8 *)&entry->gateway_mac, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tempVal = entry->vlan_id;
    if ((ret = table_field_set(APOLLOMP_NETIFt, APOLLOMP_NETIF_VLANIDtf, &tempVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    if ((ret = table_write(APOLLOMP_NETIFt, idx, (uint32 *)&netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    return RT_ERR_OK;
} /* end of dal_apollomp_l34_netifTable_set */



/* Function Name:
 *      dal_apollomp_l34_netifTable_get
 * Description:
 *      Get a arp entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *entry)
{
    int32 ret;
    apollomp_l34_netif_entry_t netif_entry;
    uint32 tmpVal;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&netif_entry, 0x0, sizeof(netif_entry));

    if ((ret = table_read(APOLLOMP_NETIFt, idx, (uint32 *)&netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_MTUtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->mtu = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_ENRTRtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->enable_rounting = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_MACMASKtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->mac_mask = tmpVal;

    if ((ret = table_field_mac_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_GMACtf, (uint8 *)&(entry->gateway_mac), (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_VLANIDtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->vlan_id = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NETIFt, APOLLOMP_NETIF_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &netif_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_netifTable_get */


/* Function Name:
 *      dal_apollomp_l34_arpTable_set
 * Description:
 *      Set ARP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *entry)
{
    int32 ret;
    apollomp_l34_arp_entry_t apollomp_arp_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L2_LEARN_LIMIT_CNT_MAX()<=entry->nhIdx), RT_ERR_ENTRY_INDEX);


    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&apollomp_arp_entry, 0x0, sizeof(apollomp_arp_entry));

    if ((ret = table_field_set(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_NXTHOPIDXtf, (uint32 *)&entry->nhIdx, (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_set(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_VALIDtf, (uint32 *)&entry->valid, (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_ARP_TABLEt, idx, (uint32 *)&apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_arpTable_set */



/* Function Name:
 *      dal_apollomp_l34_arpTable_get
 * Description:
 *      Get a arp entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *entry)
{
    int32 ret;
    apollomp_l34_arp_entry_t apollomp_arp_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&apollomp_arp_entry, 0x0, sizeof(apollomp_arp_entry));

    if ((ret = table_read(APOLLOMP_ARP_TABLEt, idx, (uint32 *)&apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_NXTHOPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;


    return RT_ERR_OK;
} /* end of dal_apollomp_l34_arpTable_get */

/* Function Name:
 *      dal_apollomp_l34_arpTable_del
 * Description:
 *      delete a arp entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_arpTable_del(uint32 idx)
{
    int32 ret;
    apollomp_l34_arp_entry_t apollomp_arp_entry;
    uint32 is_valid=0;
    RT_PARAM_CHK((HAL_L34_ARP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&apollomp_arp_entry, 0x0, sizeof(apollomp_arp_entry));

    if ((ret = table_field_set(APOLLOMP_ARP_TABLEt, APOLLOMP_ARP_TABLE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_ARP_TABLEt, idx, (uint32 *)&apollomp_arp_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_apollomp_l34_arpTable_del */



/* Function Name:
 *      dal_apollomp_l34_pppoeTable_set
 * Description:
 *      Set ARP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
    int32 ret;
    apollomp_l34_pppoe_entry_t apollomp_pppoe_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->sessionID >= 0x10000), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&apollomp_pppoe_entry, 0x0, sizeof(apollomp_pppoe_entry));

    if ((ret = table_field_set(APOLLOMP_PPPOE_TABLEt, APOLLOMP_PPPOE_TABLE_SESIDtf, (uint32 *)&entry->sessionID, (uint32 *) &apollomp_pppoe_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_PPPOE_TABLEt, idx, (uint32 *)&apollomp_pppoe_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_pppoeTable_set */




/* Function Name:
 *      dal_apollomp_l34_pppoeTable_get
 * Description:
 *      Get a PPPOE entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
    int32 ret;
    apollomp_l34_pppoe_entry_t apollomp_pppoe_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&apollomp_pppoe_entry, 0x0, sizeof(apollomp_pppoe_entry));

    if ((ret = table_read(APOLLOMP_PPPOE_TABLEt, idx, (uint32 *)&apollomp_pppoe_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_PPPOE_TABLEt, APOLLOMP_PPPOE_TABLE_SESIDtf, (uint32 *)&(tmp_val), (uint32 *) &apollomp_pppoe_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->sessionID = tmp_val;


    return RT_ERR_OK;
} /* end of dal_apollomp_l34_pppoeTable_get */







/* Function Name:
 *      dal_apollomp_l34_routingTable_set
 * Description:
 *      Set ARP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *entry)
{
    int32 ret;
    apollomp_l34_routing_entry_t routing_entry;
    rtk_table_list_t table_type;
    uint32 tmpVal;
    uint32 nh_num_asic_val;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ROUTING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((31 < entry->ipMask), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_PROCESS_END <= entry->process), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->internal != 0 && entry->internal != 1), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= entry->netifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((128 <= entry->arpStart), RT_ERR_INPUT);
    RT_PARAM_CHK((128 <= entry->arpEnd), RT_ERR_INPUT);

    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX() <= entry->nhStart), RT_ERR_INPUT);
    RT_PARAM_CHK((5 <= entry->nhNum), RT_ERR_INPUT);
    RT_PARAM_CHK((3 <= entry->nhAlgo), RT_ERR_INPUT);
    RT_PARAM_CHK((4 <= entry->ipDomain), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));


    /*general part*/
    tmpVal = entry->ipAddr;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_IPtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipMask;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_MASKtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    tmpVal = entry->process;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    tmpVal = entry->valid;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    tmpVal = entry->internal;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_INTtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->rt2waninf;
    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);  
        return RT_ERR_FAILED;
    }

    if(entry->process == L34_PROCESS_CPU || entry->process == L34_PROCESS_DROP)
        table_type = APOLLOMP_L3_ROUTING_DROP_TRAPt;
    else if(entry->process == L34_PROCESS_ARP)
        table_type = APOLLOMP_L3_ROUTING_LOCAL_ROUTEt;
    else if(entry->process == L34_PROCESS_NH)
        table_type = APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt;
    else
        return RT_ERR_FAILED;

    /*process == arp*/
    if(table_type == APOLLOMP_L3_ROUTING_LOCAL_ROUTEt)
    {
        tmpVal = entry->netifIdx;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_DENTIFtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        tmpVal = entry->arpStart;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_ARPSTAtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        tmpVal = entry->arpEnd;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_ARPENDtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
    }
    /*process == nexthop*/
    else if(table_type == APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt)
    {
        tmpVal = entry->ipDomain;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_IPDOMAINtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        tmpVal = entry->nhAlgo;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NHALGOtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        #if 0
        tmpVal = entry->nhNxt;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NHNXTtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        #endif
        tmpVal = entry->nhStart;
        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NH_ADDR_STARTtf, (uint32 *)&tmpVal, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }

        nh_num_asic_val = entry->nhNum;

        if ((ret = table_field_set(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NH_NUMtf, (uint32 *)&nh_num_asic_val, (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }


    }


    if ((ret = table_write(table_type, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_routingTable_set */

/* Function Name:
 *      dal_apollomp_l34_routingTable_get
 * Description:
 *      Get a routing table entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *entry)
{
    int32 ret;
    apollomp_l34_routing_entry_t routing_entry;
    rtk_table_list_t table_type;
    uint32 tmp_val;

    /*input error check*/
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_ROUTING_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));

    if ((ret = table_read(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_PROCESStf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->process = tmp_val;

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_IPtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    entry->ipAddr = tmp_val;

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_MASKtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ipMask = tmp_val;


    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_INTtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->internal = tmp_val;

    if ((ret = table_field_get(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_RT2WANINFtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);  
        return RT_ERR_FAILED;
    }
    entry->rt2waninf = tmp_val;


    /*get entry process type*/
    if(entry->process == L34_PROCESS_CPU || entry->process == L34_PROCESS_DROP)
        table_type = APOLLOMP_L3_ROUTING_DROP_TRAPt;
    else if(entry->process == L34_PROCESS_ARP)
        table_type = APOLLOMP_L3_ROUTING_LOCAL_ROUTEt;
    else if(entry->process == L34_PROCESS_NH)
        table_type = APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt;
    else
        return RT_ERR_FAILED;


    /*process == arp*/
    if(table_type == APOLLOMP_L3_ROUTING_LOCAL_ROUTEt)
    {
        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_DENTIFtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->netifIdx = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_ARPSTAtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->arpStart = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_LOCAL_ROUTE_ARPENDtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->arpEnd = tmp_val;


    }
    /*process == nexthop*/
    else if(table_type == APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt)
    {
        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_IPDOMAINtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->ipDomain = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NHALGOtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->nhAlgo = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NHNXTtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->nhNxt = tmp_val;

        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NH_ADDR_STARTtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->nhStart = tmp_val;



        if ((ret = table_field_get(table_type, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_NH_NUMtf, (uint32 *)&(tmp_val), (uint32 *) &routing_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        entry->nhNum = tmp_val;

    }
    return RT_ERR_OK;
} /* end of dal_apollomp_l34_routingTable_get */




/* Function Name:
 *      dal_apollomp_l34_routingTable_del
 * Description:
 *      delete a routing entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_routingTable_del(uint32 idx)
{
    int32 ret;
    apollomp_l34_routing_entry_t routing_entry;
    uint32 is_valid=0;

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&routing_entry, 0x0, sizeof(routing_entry));

    if ((ret = table_field_set(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, APOLLOMP_L3_ROUTING_GLOBAL_ROUTE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_L3_ROUTING_GLOBAL_ROUTEt, idx, (uint32 *)&routing_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_apollomp_l34_routingTable_del */


/* Function Name:
 *      dal_apollomp_l34_nexthopTable_set
 * Description:
 *      Set nexthop entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
    int32 ret;
    apollomp_l34_nexthop_entry_t nh_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((2048 <= entry->nhIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= entry->ifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= entry->pppoeIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((L34_NH_END <= entry->type), RT_ERR_INPUT);


    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&nh_entry, 0x0, sizeof(nh_entry));


    tmp_val = (uint32)entry->nhIdx;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_NXTHOPIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->pppoeIdx;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_PPPIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->type;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_TYPEtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->ifIdx;
    if ((ret = table_field_set(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_IFIDXtf, &tmp_val, (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_NEXT_HOP_TABLEt, idx, (uint32 *)&nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_apollomp_l34_nexthopTable_set */



/* Function Name:
 *      dal_apollomp_l34_nexthopTable_get
 * Description:
 *      Get a nexthop entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
    int32 ret;
    apollomp_l34_nexthop_entry_t nh_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&nh_entry, 0x0, sizeof(nh_entry));

    if ((ret = table_read(APOLLOMP_NEXT_HOP_TABLEt, idx, (uint32 *)&nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_NXTHOPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_PPPIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->pppoeIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_IFIDXtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ifIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NEXT_HOP_TABLEt, APOLLOMP_NEXT_HOP_TABLE_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &nh_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->type = tmp_val;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_nexthopTable_get */




/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_set
 * Description:
 *      Set external internal IP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
    int32 ret;
    apollomp_l34_extip_entry_t extip_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_NH_ENTRY_MAX()<=entry->nhIdx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->prival != 0 && entry->prival != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->type >= L34_EXTIP_TYPE_END ), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->pri >= RTK_MAX_NUM_OF_PRIORITY ), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    tmp_val = (uint32)entry->pri;
    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_PRIORITYtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->prival;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_PRIVALtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->nhIdx;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_NH_IDXtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmp_val = (uint32)entry->type;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_TYPEtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->valid;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_VALIDtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->extIpAddr;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_EXTIPtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->intIpAddr;
    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_INTIPtf, &tmp_val, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_extIntIPTable_set */



/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_get
 * Description:
 *      Get a external internal IP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None
 */
int32
dal_apollomp_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
    int32 ret;
    apollomp_l34_extip_entry_t extip_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    if ((ret = table_read(APOLLOMP_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_PRIORITYtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->pri = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_PRIVALtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->prival = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_NH_IDXtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->type = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_EXTIPtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->extIpAddr = tmp_val;

    if ((ret = table_field_get(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_INTIPtf, (uint32 *)&(tmp_val), (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->intIpAddr = tmp_val;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_extIntIPTable_get */



/* Function Name:
 *      dal_apollomp_l34_extIntIPTable_del
 * Description:
 *      delete a external internal IP entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_extIntIPTable_del(uint32 idx)
{
    int32 ret;
    apollomp_l34_extip_entry_t extip_entry;
    uint32 is_valid=0;
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&extip_entry, 0x0, sizeof(extip_entry));

    if ((ret = table_field_set(APOLLOMP_EXTERNAL_IP_TABLEt, APOLLOMP_EXTERNAL_IP_TABLE_VALIDtf, (uint32 *)&is_valid, (uint32 *) &extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_EXTERNAL_IP_TABLEt, idx, (uint32 *)&extip_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

}/* end of dal_apollomp_l34_extIntIPTable_del */


/* Function Name:
 *      dal_apollomp_l34_naptRemHash_get
 * Description:
 *      Get a hash index by source IP and source port.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      hash_value
 * Note:
 *      None
 */
uint32
dal_apollomp_l34_naptRemHash_get(uint32 sip, uint32 sport)
{
	uint16 hash_value=0;

	hash_value = ((sip&0xffff) ^ ((sip>>16)&0xffff) ^ (sport));

	return hash_value;
}/* end of dal_apollomp_l34_naptRemHash_get */



/* Function Name:
 *      dal_apollomp_l34_naptInboundHashidx_get
 * Description:
 *      Get Inband NAPT hash index.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      hash idex
 * Note:
 *      None
 */
uint32
dal_apollomp_l34_naptInboundHashidx_get(uint32 dip, uint16 dport, uint16 isTCP)
{
    /* Hashing Algorithm:
        DIP[7:0] ^ DIP[15:8] ^ DIP[23:16] ^ DIP[31:24] ^ DPOR[7:0] ^ DPORT[15:8] ^ ( TCP << 7)
    */
    uint32 eidx=0;

    eidx = ((dip&0xff) ^ ((dip>>8)&0xff) ^ ((dip>>16)&0xff) ^ ((dip>>24)&0xff) ^ (dport&0xff) ^ ((dport>>8)&0xff) ^ (isTCP << 7));

    return eidx;

}


/* Function Name:
 *      dal_apollomp_l34_naptInboundTable_set
 * Description:
 *      Set Inband NAPT table by idx.
 * Input:
 *      None
 * Output:
 *      forced : 0:do not add this entry if collection  1:farce overwrite this entry
 *      idx    : entry index
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *entry)
{
    int32 ret;
    apollomp_l34_napt_inband_entry_t naptInband_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPTR_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((forced != 0 && forced != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->isTcp != 0 && entry->isTcp != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->valid >=4), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priValid != 0 && entry->priValid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priId >= RTK_MAX_NUM_OF_PRIORITY), RT_ERR_PRIORITY);
    RT_PARAM_CHK((entry->extIpIdx >= HAL_L34_EXTIP_ENTRY_MAX()), RT_ERR_INPUT);


    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* check if the index is valid*/
    if(forced == 0)
    {
        osal_memset(&naptInband_entry, 0x0, sizeof(naptInband_entry));
        if ((ret = table_read(APOLLOMP_NAPTR_TABLEt, idx, (uint32 *)&naptInband_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        /*get valid field*/
        if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &naptInband_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if(tmp_val == 1)
        {
            return RT_ERR_FAILED;
        }
    }


    osal_memset(&naptInband_entry, 0x0, sizeof(naptInband_entry));

	tmp_val = entry->priId;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_PRIORITYtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	tmp_val = entry->priValid;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_PRI_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	tmp_val = entry->valid;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	tmp_val = entry->isTcp;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_TCPtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	tmp_val = entry->extPortLSB;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_EXTPRT_LSBtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	tmp_val = entry->extIpIdx;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_EXTIP_IDXtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	tmp_val = entry->remHash;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_REM_HASHtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	tmp_val = entry->intPort;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_INTPORTtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

	tmp_val = entry->intIp;
    if ((ret = table_field_set(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_INTIPtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    if ((ret = table_write(APOLLOMP_NAPTR_TABLEt, idx, (uint32 *)&naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_naptInboundTable_set */






/* Function Name:
 *      dal_apollomp_l34_naptInboundTable_get
 * Description:
 *      Get Inband NAPT table by idx.
 * Input:
 *      None
 * Output:
 *      idx    : entry index
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *entry)
{
    int32 ret;
    apollomp_l34_napt_inband_entry_t naptInband_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPTR_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);


    osal_memset(&naptInband_entry, 0x0, sizeof(naptInband_entry));

    if ((ret = table_read(APOLLOMP_NAPTR_TABLEt, idx, (uint32 *)&naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_PRIORITYtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->priId = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_PRI_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->priValid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_VALIDtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_TCPtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->isTcp = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_EXTPRT_LSBtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->extPortLSB = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_EXTIP_IDXtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->extIpIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_REM_HASHtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->remHash= tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_INTPORTtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->intPort= tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPTR_TABLEt, APOLLOMP_NAPTR_TABLE_INTIPtf, (uint32 *)&tmp_val, (uint32 *) &naptInband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->intIp = tmp_val;

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_naptInboundTable_get */






/* Function Name:
 *      dal_apollomp_l34_naptOutboundHashidx_get
 * Description:
 *      Get Outband NAPT hash index.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      hash idex
 * Note:
 *      None
 */
uint32
dal_apollomp_l34_naptOutboundHashidx_get(int8 isTCP, uint32 sip, uint16 sport, uint32 dip, uint16 dport)
{
    /* Hashing Algorithm:
        X[15:0] = SIP[15:0] ^ SIP[31:16] ^ SPORT[15:0] ^ DIP[15:0] ^ DIP[31:16] ^ DPORT[15:0]
        NAPT 5-tuple hash ID[8:0] = X[8:0] ^ { 2'b0, X[15:9] } ^ ( TCP << 8)
    */
    uint16 eidx_16;
    uint32 eidx_9;

    eidx_16 = (sip&0xff) ^ ((sip&0xff00)>>16) ^ (sport) ^ (dip&0xff) ^ ((dip&0xff00)>>16)^dport;
    eidx_9 = (eidx_16 & 0x1ff) ^ ((eidx_16 & 0xfe)>>9) ^(isTCP<<8) ;

    return eidx_9;


}


/* Function Name:
 *      dal_apollomp_l34_naptOutboundTable_set
 * Description:
 *      Set Outband NAPT table by idx.
 * Input:
 *      None
 * Output:
 *      forced : 0:do not add this entry if collection  1:farce overwrite this entry
 *      idx    : entry index
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *entry)
{
    int32 ret;
    apollomp_l34_napt_inband_entry_t naptOnband_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((forced != 0 && forced != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->valid != 0 && entry->valid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priValid != 0 && entry->priValid != 1), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->priValue >= RTK_MAX_NUM_OF_PRIORITY), RT_ERR_PRIORITY);
    RT_PARAM_CHK((entry->hashIdx >=1024), RT_ERR_INPUT);



    /* check Init status */
    RT_INIT_CHK(l34_init);

    /* check if the index is valid*/
    if(forced == 0)
    {
        osal_memset(&naptOnband_entry, 0x0, sizeof(naptOnband_entry));
        if ((ret = table_read(APOLLOMP_NAPT_TABLEt, idx, (uint32 *)&naptOnband_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        /*get valid field from napt table*/
        if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_VALIDtf, (uint32 *)&(tmp_val), (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return RT_ERR_FAILED;
        }
        if(tmp_val == 1)
        {
            return RT_ERR_FAILED;
        }
    }


    osal_memset(&naptOnband_entry, 0x0, sizeof(naptOnband_entry));

    tmp_val = (uint32)entry->valid;
    if ((ret = table_field_set(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_VALIDtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    tmp_val = (uint32)entry->hashIdx;
    if ((ret = table_field_set(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_HASHIN_IDXtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

#if 0
    tmp_val = (uint32)entry->priValid;
    if ((ret = table_field_set(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_PRI_VALIDtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);  
        return RT_ERR_FAILED;
    }
    
    tmp_val = (uint32)entry->priValue;
    if ((ret = table_field_set(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_PRItf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);  
        return RT_ERR_FAILED;
    }   
#endif
    if ((ret = table_write(APOLLOMP_NAPT_TABLEt, idx, (uint32 *)&naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_naptOutboundTable_set */



/* Function Name:
 *      dal_apollomp_l34_naptOutboundTable_get
 * Description:
 *      Set Outband NAPT table by idx.
 * Input:
 *      None
 * Output:
 *      idx    : entry index
 *      entry  : entry content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry)
{
    int32 ret;
    apollomp_l34_napt_inband_entry_t naptOnband_entry;
    uint32 tmp_val;
    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((HAL_L34_NAPT_ENTRY_MAX()<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&naptOnband_entry, 0x0, sizeof(naptOnband_entry));

    if ((ret = table_read(APOLLOMP_NAPT_TABLEt, idx, (uint32 *)&naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_VALIDtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmp_val;


    if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_HASHIN_IDXtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->hashIdx = tmp_val;
#if 0
    if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_PRI_VALIDtf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);  
        return RT_ERR_FAILED;
    }
    entry->priValid = tmp_val;

    if ((ret = table_field_get(APOLLOMP_NAPT_TABLEt, APOLLOMP_NAPT_TABLE_PRItf, &tmp_val, (uint32 *) &naptOnband_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);  
        return RT_ERR_FAILED;
    }    
    entry->priValue = tmp_val;
#endif
     
    return RT_ERR_OK;
}/* end of dal_apollomp_l34_naptOutboundTable_get */



/* Function Name:
 *      dal_apollomp_l34_table_reset
 * Description:
 *      Reset specific L34 table.
 * Input:
 *      None
 * Output:
 *      type : L34 table type
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_table_reset(rtk_l34_table_type_t type)
{
    int32 ret;
    uint32 reg_field;
    uint32 reset_status,busy;
    /*get reset table type*/
    switch(type)
    {
        case L34_ROUTING_TABLE:
            reg_field = APOLLOMP_RST_L3f;
            break;
        case L34_PPPOE_TABLE:
            reg_field = APOLLOMP_RST_PPf;
            break;
        case L34_NEXTHOP_TABLE:
            reg_field = APOLLOMP_RST_NHf;
            break;
        case L34_NETIF_TABLE:
            reg_field = APOLLOMP_RST_IFf;
            break;
        case L34_INTIP_TABLE:
            reg_field = APOLLOMP_RST_IPf;
            break;
        case L34_ARP_TABLE:
            reg_field = APOLLOMP_RST_ARPf;
            break;
        case L34_NAPTR_TABLE:
            reg_field = APOLLOMP_RST_NAPTRf;
            break;
        case L34_NAPT_TABLE:
            reg_field = APOLLOMP_RST_NAPTf;
            break;

        case L34_IPV6_ROUTING_TABLE:
            reg_field = APOLLOMP_RST_V6RTf;
            break;

        case L34_BINDING_TABLE:
            reg_field = APOLLOMP_RST_BDf;
            break;

        case L34_IPV6_NEIGHBOR_TABLE:
            reg_field = APOLLOMP_RST_NBf;
            break;

        case L34_WAN_TYPE_TABLE:
            reg_field = APOLLOMP_RST_WTf;
            break;

        default:
            return RT_ERR_FAILED;
    }
    /*get register*/
    if ((ret = reg_field_read(APOLLOMP_NAT_TBL_ACCESS_CLRr,reg_field,&reset_status))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    if(reset_status == 0)
    {/*table is not perform reset, we reset it*/
        reset_status = 1;
        if ((ret = reg_field_write(APOLLOMP_NAT_TBL_ACCESS_CLRr,reg_field,&reset_status))!=RT_ERR_OK)
        {
            RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
            return ret;
        }
    }

    /* Wait operation completed */
    do
    {
        if ((ret = reg_field_read(APOLLOMP_NAT_TBL_ACCESS_CLRr, reg_field, &busy)) != RT_ERR_OK)
        {
            return ret;
        }
    } while (busy);

    return RT_ERR_OK;
}/* end of dal_apollomp_l34_table_reset */




/* Function Name:
 *      dal_apollomp_l34_hsb_set
 * Description:
 *      set hsb value.
 * Input:
 *      hsb : hsb content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hsb_set(apollomp_l34_hsb_param_t *hsb)
{
    int32 ret;
    if ((ret = reg_write(APOLLOMP_HSB_DESCr, (uint32 *)hsb->hsbWords))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;

}/* end of dal_apollomp_l34_hsb_set */



/* Function Name:
 *      dal_apollomp_l34_hsb_get
 * Description:
 *      get data from hsb.
 * Input:
 *      None
 * Output:
 *      hsb : hsb content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hsb_get(apollomp_l34_hsb_param_t *hsb)
{
    int32 ret;

    if ((ret = reg_read(APOLLOMP_HSB_DESCr, (uint32 *)hsb->hsbWords))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;


}/* end of dal_apollomp_l34_hsb_get */





/* Function Name:
 *      dal_apollomp_l34_hsa_set
 * Description:
 *      set hsa value.
 * Input:
 *      hsa : hsa content
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hsa_set(apollomp_l34_hsa_param_t *hsa)
{
    int32 ret;
    if ((ret = reg_write(APOLLOMP_HSA_DESCr, (uint32 *)hsa->hsaWords))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;

}/* end of dal_apollomp_l34_hsa_set */



/* Function Name:
 *      dal_apollomp_l34_hsa_get
 * Description:
 *      get data from hsa.
 * Input:
 *      None
 * Output:
 *      hsb : hsa content
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hsa_get(apollomp_l34_hsa_param_t *hsa)
{
    int32 ret;

    if ((ret = reg_read(APOLLOMP_HSA_DESCr, (uint32 *)hsa->hsaWords))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;


}/* end of dal_apollomp_l34_hsa_get */


/* Function Name:
 *      dal_apollomp_l34_hsabCtrMode_set
 * Description:
 *      Set L34 HSAB log mode.
 * Input:
 *      None
 * Output:
 *      mode : L34 HSAB log mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_hsabCtrMode_set(apollomp_l34_hsab_mode_t mode)
{
    int32 ret;
    if ((ret = reg_field_write(APOLLOMP_HSBA_CTRLr,APOLLOMP_TST_LOG_MDf,(uint32 *)&mode))!=RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return ret;
    }
    return RT_ERR_OK;
}/* end of dal_apollomp_l34_hsabCtrMode_set */





/* Function Name:
 *      dal_apollomp_l34_wanTypeTable_set
 * Description:
 *      Set WAN type entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_wanTypeTable_set(uint32 idx, rtk_wanType_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    apollomp_l34_wan_type_entry_t wanType_entry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_WAN_TYPE_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->nhIdx >= HAL_L34_NH_ENTRY_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->wanType >= L34_WAN_TYPE_END), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&wanType_entry, 0x0, sizeof(apollomp_l34_wan_type_entry_t));

    tmpVal = entry->nhIdx;
    if ((ret = table_field_set(APOLLOMP_WAN_TYPE_TABLEt, APOLLOMP_WAN_TYPE_TABLE_NXHOPTBIDXtf, (uint32 *)&tmpVal, (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->wanType;
    if ((ret = table_field_set(APOLLOMP_WAN_TYPE_TABLEt, APOLLOMP_WAN_TYPE_TABLE_WAN_TYPEtf, (uint32 *)&tmpVal, (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_write(APOLLOMP_WAN_TYPE_TABLEt, idx, (uint32 *)&wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_wanTypeTable_set */




/* Function Name:
 *      dal_apollomp_l34_wanTypeTable_get
 * Description:
 *      Get a WAN type entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_wanTypeTable_get(uint32 idx, rtk_wanType_entry_t *entry)
{
    int32 ret;
    apollomp_l34_wan_type_entry_t wanType_entry;
    uint32 tmp_val;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_WAN_TYPE_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&wanType_entry, 0x0, sizeof(apollomp_l34_wan_type_entry_t));

    if ((ret = table_read(APOLLOMP_WAN_TYPE_TABLEt, idx, (uint32 *)&wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_WAN_TYPE_TABLEt, APOLLOMP_WAN_TYPE_TABLE_NXHOPTBIDXtf, (uint32 *)&(tmp_val), (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhIdx = tmp_val;

    if ((ret = table_field_get(APOLLOMP_WAN_TYPE_TABLEt, APOLLOMP_WAN_TYPE_TABLE_WAN_TYPEtf, (uint32 *)&(tmp_val), (uint32 *) &wanType_entry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->wanType = tmp_val;

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_wanTypeTable_get */



/* Function Name:
 *      dal_apollomp_l34_bindingTable_set
 * Description:
 *      Set binding table entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_bindingTable_set(uint32 idx,rtk_binding_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    apollomp_l34_binding_entry_t bindingEntry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_BINDING_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((entry->wanTypeIdx >= APOLLOMP_L34_WAN_TYPE_TABLE_MAX), RT_ERR_INPUT);
    RT_PARAM_CHK((entry->vidLan > RTK_VLAN_ID_MAX), RT_ERR_INPUT);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&bindingEntry, 0x0, sizeof(apollomp_l34_binding_entry_t));

    tmpVal = entry->wanTypeIdx;
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_WAN_TYPE_INDEXtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->vidLan;
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_VID_LANtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->portMask.bits[0];
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_PORT_MASKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->extPortMask.bits[0];
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_EXT_PMSKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->bindProto;
    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_BIND_PTLtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    if ((ret = table_write(APOLLOMP_BINDING_TABLEt, idx, (uint32 *)&bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_bindingTable_set */



/* Function Name:
 *      dal_apollomp_l34_bindingTable_get
 * Description:
 *      Get a binding table entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_bindingTable_get(uint32 idx,rtk_binding_entry_t *entry)
{
    int32 ret;
    apollomp_l34_binding_entry_t bindingEntry;
    uint32 tmpVal;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_WAN_TYPE_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&bindingEntry, 0x0, sizeof(apollomp_l34_binding_entry_t));

    if ((ret = table_read(APOLLOMP_BINDING_TABLEt, idx, (uint32 *)&bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_set(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_WAN_TYPE_INDEXtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->wanTypeIdx = tmpVal;

    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_VID_LANtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->vidLan = tmpVal;

    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_PORT_MASKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->portMask.bits[0] = tmpVal;

    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_EXT_PMSKtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->extPortMask.bits[0] = tmpVal;


    /*binding protocal*/
    if ((ret = table_field_get(APOLLOMP_BINDING_TABLEt, APOLLOMP_BINDING_TABLE_BIND_PTLtf, (uint32 *)&tmpVal, (uint32 *) &bindingEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->bindProto = tmpVal;




    return RT_ERR_OK;
} /* end of dal_apollomp_l34_bindingTable_get */


/* Function Name:
 *      dal_apollomp_l34_ipv6NeighborTable_set
 * Description:
 *      Set IPv6 neighbor table entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_ipv6NeighborTable_set(uint32 idx,rtk_ipv6Neighbor_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    uint64 tmpVal64;
    apollomp_l34_ipv6_neighbor_entry_t rawEntry;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(apollomp_l34_ipv6_neighbor_entry_t));

    tmpVal = entry->valid;
    if ((ret = table_field_set(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->l2Idx;
    if ((ret = table_field_set(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_L2_TABLE_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipv6RouteIdx;
    if ((ret = table_field_set(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_RT_MATCH_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal64 = entry->ipv6Ifid;
    if ((ret = table_field_set(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_IP6IF_IDtf, (uint32 *)&tmpVal64, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    if ((ret = table_write(APOLLOMP_NEIGHBOR_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_l34_ipv6NeighborTable_set */



/* Function Name:
 *      dal_apollomp_l34_ipv6NeighborTable_get
 * Description:
 *      Get a IPv6 neighbor table entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_ipv6NeighborTable_get(uint32 idx,rtk_ipv6Neighbor_entry_t *entry)
{
    int32 ret;
    apollomp_l34_ipv6_neighbor_entry_t rawEntry;
    uint32 tmpVal;
    uint64 tmpVal64;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(apollomp_l34_ipv6_neighbor_entry_t));

    if ((ret = table_read(APOLLOMP_NEIGHBOR_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    if ((ret = table_field_get(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;


    if ((ret = table_field_get(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_L2_TABLE_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->l2Idx = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_RT_MATCH_IDXtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ipv6RouteIdx = tmpVal;

    if ((ret = table_field_get(APOLLOMP_NEIGHBOR_TABLEt, APOLLOMP_NEIGHBOR_TABLE_IP6IF_IDtf, (uint32 *)&tmpVal64, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ipv6Ifid = tmpVal64;


    return RT_ERR_OK;
} /* end of dal_apollomp_l34_ipv6NeighborTable_get */



/* Function Name:
 *      dal_apollomp_l34_ipv6RoutingTable_set
 * Description:
 *      Set a IPv6 routing entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_ipv6RoutingTable_set(uint32 idx, rtk_ipv6Routing_entry_t *entry)
{
    int32 ret;
    uint32 tmpVal;
    apollomp_l34_ipv6_routing_entry_t rawEntry;
    rtk_ipv6_addr_t     ipv6Addr;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(apollomp_l34_ipv6_routing_entry_t));

    tmpVal = entry->valid;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->type;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->nhOrIfidIdx;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_NEXTHOPtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->ipv6PrefixLen;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_IP6_PREFIX_LENtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }

    tmpVal = entry->rt2waninf;
    if ((ret = table_field_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    memcpy(&ipv6Addr,&entry->ipv6Addr,sizeof(rtk_ipv6_addr_t));
    if ((ret = table_field_byte_set(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_IP6_DIPtf, (uint8 *)&ipv6Addr, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);  
        return RT_ERR_FAILED;
    }
 
    if ((ret = table_write(APOLLOMP_IPV6_ROUTING_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_l34_ipv6RoutingTable_set */



/* Function Name:
 *      dal_apollomp_l34_ipv6RoutingTable_get
 * Description:
 *      Get a IPv6 routing entry by idx.
 * Input:
 *      None
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_ipv6RoutingTable_get(uint32 idx,rtk_ipv6Routing_entry_t *entry)
{
    int32 ret;
    apollomp_l34_ipv6_routing_entry_t rawEntry;
    uint32 tmpVal;
    rtk_ipv6_addr_t     ipv6Addr;

    RT_PARAM_CHK((entry==NULL), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_L34_IPV6_NBR_TABLE_MAX<=idx), RT_ERR_ENTRY_INDEX);

    /* check Init status */
    RT_INIT_CHK(l34_init);

    osal_memset(&rawEntry, 0x0, sizeof(apollomp_l34_ipv6_routing_entry_t));

    if ((ret = table_read(APOLLOMP_IPV6_ROUTING_TABLEt, idx, (uint32 *)&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }


    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_VALIDtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->valid = tmpVal;

    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_PROCESStf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->type = tmpVal;

    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_NEXTHOPtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->nhOrIfidIdx = tmpVal;

    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_IP6_PREFIX_LENtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->ipv6PrefixLen = tmpVal;

    if ((ret = table_field_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_RT2WANINFtf, (uint32 *)&tmpVal, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);
        return RT_ERR_FAILED;
    }
    entry->rt2waninf = tmpVal;

    if ((ret = table_field_byte_get(APOLLOMP_IPV6_ROUTING_TABLEt, APOLLOMP_IPV6_ROUTING_TABLE_IP6_DIPtf, (uint8 *)&ipv6Addr, (uint32 *) &rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x",ret);  
        return RT_ERR_FAILED;
    }
    memcpy(&(entry->ipv6Addr),&ipv6Addr,sizeof(rtk_ipv6_addr_t));

    return RT_ERR_OK;
} /* end of dal_apollomp_l34_ipv6RoutingTable_get */

/* Function Name:
 *      dal_apollomp_l34_ipmcTransTable_set
 * Description:
 * Input:
 *      idx     - index
 *      pEntry  - IPMC Translation entry
 * Output:
 *      idx
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry)
{
    int32 ret;
    apollomp_raw_l34_ipmcTransEntry_t   rawEntry;

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_IPMC_TRANS_MAX < idx), RT_ERR_ENTRY_INDEX);
    RT_PARAM_CHK((HAL_L34_NETIF_ENTRY_MAX() <= pEntry->netifIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_PPPOE_ENTRY_MAX() <= pEntry->pppoeIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((HAL_L34_EXTIP_ENTRY_MAX() <= pEntry->extipIdx), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->sipTransEnable), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= pEntry->isPppoeIf), RT_ERR_INPUT);

    osal_memset(&rawEntry, 0x00, sizeof(apollomp_raw_l34_ipmcTransEntry_t));
    rawEntry.index          = idx;
    rawEntry.netifIdx       = pEntry->netifIdx;
    rawEntry.sipTransEnable = pEntry->sipTransEnable;
    rawEntry.extipIdx       = pEntry->extipIdx;
    rawEntry.isPppoeIf      = pEntry->isPppoeIf;
    rawEntry.pppoeIdx       = pEntry->pppoeIdx;

    if ((ret = apollomp_raw_l34_ipmcTransEntry_set(&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;

} /* end of dal_apollomp_l34_ipmcTransTable_set */


/* Function Name:
 *      dal_apollomp_l34_ipmcTransTable_get
 * Description:
 * Input:
 *      idx     - index
 * Output:
 *      pEntry  - IPMC Translation entry
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry)
{
    int32 ret;
    apollomp_raw_l34_ipmcTransEntry_t   rawEntry;

    RT_PARAM_CHK((NULL == pEntry), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((APOLLOMP_IPMC_TRANS_MAX < idx), RT_ERR_ENTRY_INDEX);

    osal_memset(&rawEntry, 0x00, sizeof(apollomp_raw_l34_ipmcTransEntry_t));
    rawEntry.index = idx;
    if ((ret = apollomp_raw_l34_ipmcTransEntry_get(&rawEntry)) != RT_ERR_OK)
    {
        RT_LOG(LOG_DEBUG, (MOD_DAL|MOD_L34), "return failed ret value = %x", ret);
        return RT_ERR_FAILED;
    }

    pEntry->netifIdx        = rawEntry.netifIdx;
    pEntry->sipTransEnable  = rawEntry.sipTransEnable;
    pEntry->extipIdx        = rawEntry.extipIdx;
    pEntry->isPppoeIf       = rawEntry.isPppoeIf;
    pEntry->pppoeIdx        = rawEntry.pppoeIdx;
    return RT_ERR_OK;

} /* end of dal_apollomp_l34_ipmcTransTable_get */
