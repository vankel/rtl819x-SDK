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
 * Purpose : For linux kernel mode
 *           MAC debug APIs in the SDK
 *
 * Feature : MAC debug APIs
 *
 */

/*
 * Include Files
 */
#include <linux/kernel.h>
#include <common/error.h>
#include <common/util.h>
#include <rtk/rtusr/rtusr_pkt.h>

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
 *      mac_debug_rxPkt_enable
 * Description:
 *      Due to the rx packet debug might impact the performance, this API is to
 *      enable/disable the rx packet debug function
 * Input:
 *      enable - enable or disable the rx packet storage
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      None
 */
int32 mac_debug_rxPkt_enable(uint32 enable)
{
#if defined(CONFIG_SOC_DEPEND_FEATURE)
    int re8686_rx_pktDump_set(unsigned int enable);
    
    re8686_rx_pktDump_set(enable);
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      mac_debug_rxPkt_get
 * Description:
 *      Get the last received packet
 * Input:
 *      pPayload   - entire packet to be sent
 *      buf_length - specified the maximum length to be dumped
 *      pkt_length - get the actul length of packet
 *      pInfo      - rx descriptor of packet
 *      pEnable    - get the dump copy state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Dump command always success
 */
int32 mac_debug_rxPkt_get(uint8 *pPayload, uint16 buf_length, uint16 *pPkt_len, struct pkt_dbg_rx_info *pInfo, unsigned int *pEnable)
{
#if defined(CONFIG_SOC_DEPEND_FEATURE)
    int re8686_rx_pktDump_get(unsigned char *pPayload, unsigned short buf_length, unsigned short *pPkt_len, void *pInfo, unsigned int *pEnable);
    
    re8686_rx_pktDump_get(pPayload, buf_length, pPkt_len, pInfo, pEnable);
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      mac_debug_rxPkt_clear
 * Description:
 *      Clear the last received packet storage buffer
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Clear command always success
 */
int32 mac_debug_rxPkt_clear(void)
{
#if defined(CONFIG_SOC_DEPEND_FEATURE)
    int re8686_rx_pktDump_clear(void);
    
    re8686_rx_pktDump_clear();
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      mac_debug_txPkt_send
 * Description:
 *      Send the packet in the buffer
 * Input:
 *      pPayload - entire packet to be sent
 *      length   - packet length
 *      pInfo    - tx descriptor of packet
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 * Note:
 *      Dump command always success
 */
int32 mac_debug_txPkt_send(uint8 *pPayload, uint16 length, struct pkt_dbg_tx_info *pInfo)
{
#if defined(CONFIG_SOC_DEPEND_FEATURE)
    int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo);
    
    re8686_tx_with_Info(pPayload, length, pInfo);
#endif

    return RT_ERR_OK;
}

