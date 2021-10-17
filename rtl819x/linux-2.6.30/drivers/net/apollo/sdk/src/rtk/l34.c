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
#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <dal/dal_mgmt.h>
#include <rtk/l34.h>

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Module Name    : L34     */


/* Function Name:
 *      rtk_l2_init
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
 *      Must initialize l34 module before calling any L34 APIs.
 */
int32
rtk_l34_init(void)
{
    return RT_MAPPER->l34_init();
} /* end of rtk_l34_init */


/* Function Name:
 *      rtk_l34_netifTable_set
 * Description:
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
rtk_l34_netifTable_set(uint32 idx, rtk_l34_netif_entry_t *entry)
{
    return RT_MAPPER->l34_netifTable_set(idx,entry);
} /* end of rtk_l34_netifTable_set */


/* Function Name:
 *      rtk_l34_netifTable_get
 * Description:
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
rtk_l34_netifTable_get(uint32 idx, rtk_l34_netif_entry_t *entry)
{
    return RT_MAPPER->l34_netifTable_get(idx,entry);
} /* end of rtk_l34_netifTable_get */



/* Function Name:
 *      rtk_l34_arpTable_set
 * Description:
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
rtk_l34_arpTable_set(uint32 idx, rtk_l34_arp_entry_t *entry)
{
    return RT_MAPPER->l34_arpTable_set(idx,entry);
} /* end of rtk_l34_arpTable_set */


/* Function Name:
 *      rtk_l34_arpTable_get
 * Description:
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
rtk_l34_arpTable_get(uint32 idx, rtk_l34_arp_entry_t *entry)
{
    return RT_MAPPER->l34_arpTable_get(idx,entry);
} /* end of rtk_l34_arpTable_get */



/* Function Name:
 *      rtk_l34_arpTable_del
 * Description:
 * Input:
 *      None
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
rtk_l34_arpTable_del(uint32 idx)
{
    return RT_MAPPER->l34_arpTable_del(idx);
} /* end of rtk_l34_arpTable_del */



/* Function Name:
 *      rtk_l34_pppoeTable_set
 * Description:
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
rtk_l34_pppoeTable_set(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
    return RT_MAPPER->l34_pppoeTable_set(idx,entry);
} /* end of rtk_l34_pppoeTable_set */


/* Function Name:
 *      rtk_l34_pppoeTable_get
 * Description:
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
rtk_l34_pppoeTable_get(uint32 idx, rtk_l34_pppoe_entry_t *entry)
{
    return RT_MAPPER->l34_pppoeTable_get(idx,entry);
} /* end of rtk_l34_pppoeTable_get */



/* Function Name:
 *      rtk_l34_routingTable_set
 * Description:
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
rtk_l34_routingTable_set(uint32 idx, rtk_l34_routing_entry_t *entry)
{
    return RT_MAPPER->l34_routingTable_set(idx,entry);
} /* end of rtk_l34_routingTable_set */


/* Function Name:
 *      rtk_l34_routingTable_get
 * Description:
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
rtk_l34_routingTable_get(uint32 idx, rtk_l34_routing_entry_t *entry)
{
    return RT_MAPPER->l34_routingTable_get(idx,entry);
} /* end of rtk_l34_routingTable_get */



/* Function Name:
 *      rtk_l34_routingTable_del
 * Description:
 * Input:
 *      None
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
rtk_l34_routingTable_del(uint32 idx)
{
    return RT_MAPPER->l34_routingTable_del(idx);
} /* end of rtk_l34_routingTable_del */



/* Function Name:
 *      rtk_l34_nexthopTable_set
 * Description:
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
rtk_l34_nexthopTable_set(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
    return RT_MAPPER->l34_nexthopTable_set(idx,entry);
} /* end of rtk_l34_nexthopTable_set */


/* Function Name:
 *      rtk_l34_nexthopTable_get
 * Description:
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
rtk_l34_nexthopTable_get(uint32 idx, rtk_l34_nexthop_entry_t *entry)
{
    return RT_MAPPER->l34_nexthopTable_get(idx,entry);
} /* end of rtk_l34_nexthopTable_get */




/* Function Name:
 *      rtk_l34_extIntIPTable_set
 * Description:
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
rtk_l34_extIntIPTable_set(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
    return RT_MAPPER->l34_extIntIPTable_set(idx,entry);
} /* end of rtk_l34_extIntIPTable_set */


/* Function Name:
 *      rtk_l34_extIntIPTable_get
 * Description:
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
rtk_l34_extIntIPTable_get(uint32 idx, rtk_l34_ext_intip_entry_t *entry)
{
    return RT_MAPPER->l34_extIntIPTable_get(idx,entry);
} /* end of rtk_l34_extIntIPTable_get */



/* Function Name:
 *      rtk_l34_extIntIPTable_del
 * Description:
 * Input:
 *      None
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
rtk_l34_extIntIPTable_del(uint32 idx)
{
    return RT_MAPPER->l34_extIntIPTable_del(idx);
} /* end of rtk_l34_extIntIPTable_del */


/* Function Name:
 *      rtk_l34_naptRemHash_get
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
rtk_l34_naptRemHash_get(uint32 sip, uint32 sport)
{
    return RT_MAPPER->l34_naptRemHash_get(sip,sport);
}/* end of rtk_l34_naptRemHash_get */



/* Function Name:
 *      rtk_l34_naptInboundHashidx_get
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
rtk_l34_naptInboundHashidx_get(uint32 dip, uint16 dport, uint16 isTCP)
{
    return RT_MAPPER->l34_naptInboundHashidx_get(dip, dport, isTCP);

}/* end of rtk_l34_naptInboundHashidx_get */


/* Function Name:
 *      rtk_l34_naptInboundTable_set
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
rtk_l34_naptInboundTable_set(int8 forced, uint32 idx,rtk_l34_naptInbound_entry_t *entry)
{
    return RT_MAPPER->l34_naptInboundTable_set(forced, idx, entry);

}/* end of rtk_l34_naptInboundTable_set */



/* Function Name:
 *      rtk_l34_naptInboundTable_get
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
rtk_l34_naptInboundTable_get(uint32 idx,rtk_l34_naptInbound_entry_t *entry)
{
    return RT_MAPPER->l34_naptInboundTable_get(idx, entry);

}/* end of rtk_l34_naptInboundTable_get */





/* Function Name:
 *      rtk_l34_naptOutboundHashidx_get
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
rtk_l34_naptOutboundHashidx_get(int8 isTCP, uint32 sip, uint16 sport, uint32 dip, uint16 dport)
{
    return RT_MAPPER->l34_naptOutboundHashidx_get(isTCP, sip, sport, dip, dport);
}/* end of rtk_l34_naptOutboundHashidx_get */


/* Function Name:
 *      rtk_l34_naptOutboundTable_set
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
rtk_l34_naptOutboundTable_set(int8 forced, uint32 idx,rtk_l34_naptOutbound_entry_t *entry)
{
    return RT_MAPPER->l34_naptOutboundTable_set(forced, idx, entry);
}/* end of rtk_l34_naptOutboundTable_set */



/* Function Name:
 *      rtk_l34_naptOutboundTable_set
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
rtk_l34_naptOutboundTable_get(uint32 idx,rtk_l34_naptOutbound_entry_t *entry)
{
    return RT_MAPPER->l34_naptOutboundTable_get(idx, entry);
}/* end of rtk_l34_naptOutboundTable_set */




/* Function Name:
 *      rtk_l34_table_reset
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
rtk_l34_table_reset(rtk_l34_table_type_t type)
{
    return RT_MAPPER->l34_table_reset(type);
}/* end of rtk_l34_table_reset */


/* Function Name:
 *      rtk_l34_ipmcTransTable_set
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
rtk_l34_ipmcTransTable_set(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry)
{
    return RT_MAPPER->l34_ipmcTransTable_set(idx, pEntry);
} /* end of rtk_l34_ipmcTransTable_set */


/* Function Name:
 *      rtk_l34_ipmcTransTable_get
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
rtk_l34_ipmcTransTable_get(uint32 idx, rtk_l34_ipmcTrans_entry_t *pEntry)
{
    return RT_MAPPER->l34_ipmcTransTable_get(idx, pEntry);
} /* end of rtk_l34_ipmcTransTable_get */

