/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * $Revision: $
 * $Date: $
 *
 * Purpose : Realtek Switch SDK Rtusr API Module
 *
 * Feature : The file include the debug tools
 *           1) packet send/receive
 *
 */

/*
 * Include Files
 */
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <osal/lib.h>
#include <rtk/rtusr/include/rtusr_util.h>
#include <rtdrv/rtdrv_netfilter.h>

/*
 * Symbol Definition
 */

/*
 * Macro Declaration
 */

/*
 * Data Declaration
 */
static struct pkt_dbg_tx_info txInfo;
static uint16 pkt_length = 0;
static unsigned char pkt_payload[PKT_DEBUG_PKT_LENGTH_MAX] = { 0 };

/*
 * Function Declaration
 */
int32 rtk_pkt_rxDump_get(uint8 *pPayload, uint16 max_length, uint16 *pPkt_length, struct pkt_dbg_rx_info *pInfo, uint16 *pEnable)
{
    rtdrv_pktdbg_t pkt_info;
    
    pkt_info.length = max_length;
    GETSOCKOPT(RTDRV_PKT_RXDUMP_GET, &pkt_info, sizeof(rtdrv_pktdbg_t), 1);

    memcpy(pPayload, &pkt_info.buf[0], max_length);
    *pInfo = pkt_info.rx_info;
    *pPkt_length = pkt_info.rx_length;
    *pEnable = pkt_info.enable;
    
    return RT_ERR_OK;
}

int32 rtk_pkt_rxDump_clear(void)
{
    int32 dummy;
    SETSOCKOPT(RTDRV_PKT_RXDUMP_CLEAR, &dummy, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_pkt_rxDumpEnable_set(uint32 enable)
{
    SETSOCKOPT(RTDRV_PKT_RXDUMP_ENABLE, &enable, uint32, 1);
    return RT_ERR_OK;
}

int32 rtk_pkt_txAddr_set(rtk_mac_t *pDst, rtk_mac_t *pSrc)
{
    if((NULL == pDst) || (NULL == pSrc))
    {
        return RT_ERR_NULL_POINTER;
    }

    /* Set destination mac address */
    pkt_payload[0] = pDst->octet[0];
    pkt_payload[1] = pDst->octet[1];
    pkt_payload[2] = pDst->octet[2];
    pkt_payload[3] = pDst->octet[3];
    pkt_payload[4] = pDst->octet[4];
    pkt_payload[5] = pDst->octet[5];

    /* Set source mac address */
    pkt_payload[6] = pSrc->octet[0];
    pkt_payload[7] = pSrc->octet[1];
    pkt_payload[8] = pSrc->octet[2];
    pkt_payload[9] = pSrc->octet[3];
    pkt_payload[10] = pSrc->octet[4];
    pkt_payload[11] = pSrc->octet[5];

    if(pkt_length < PKT_DEBUG_PKT_MACADDR_OFFSET)
        pkt_length = PKT_DEBUG_PKT_MACADDR_OFFSET;

    return RT_ERR_OK;
}

int32 rtk_pkt_txAddr_get(rtk_mac_t *pDst, rtk_mac_t *pSrc)
{
    if((NULL == pDst) || (NULL == pSrc))
    {
        return RT_ERR_NULL_POINTER;
    }

    /* Set destination mac address */
    pDst->octet[0] = pkt_payload[0];
    pDst->octet[1] = pkt_payload[1];
    pDst->octet[2] = pkt_payload[2];
    pDst->octet[3] = pkt_payload[3];
    pDst->octet[4] = pkt_payload[4];
    pDst->octet[5] = pkt_payload[5];

    /* Set source mac address */
    pSrc->octet[0] = pkt_payload[6]; 
    pSrc->octet[1] = pkt_payload[7]; 
    pSrc->octet[2] = pkt_payload[8]; 
    pSrc->octet[3] = pkt_payload[9]; 
    pSrc->octet[4] = pkt_payload[10];
    pSrc->octet[5] = pkt_payload[11];
    
    return RT_ERR_OK;
}

int32 rtk_pkt_txPkt_set(uint16 pos, uint8 *pData, uint16 length)
{
    uint16 write_length;
    
    if(NULL == pData)
        return RT_ERR_NULL_POINTER;

    if(pos + length > PKT_DEBUG_PKT_LENGTH_MAX)
        write_length = PKT_DEBUG_PKT_LENGTH_MAX - pos;
    else
        write_length = length;

    osal_memcpy(&pkt_payload[pos], pData, write_length);

    if(write_length + pos > pkt_length)
    {
        pkt_length = write_length + pos;
    }

    return RT_ERR_OK;
}

int32 rtk_pkt_txPadding_set(uint16 start, uint16 end, uint16 length)
{
    int i;
    uint16 total_length;
    uint16 pos;

    if(length >= PKT_DEBUG_PKT_LENGTH_MAX)
        total_length = PKT_DEBUG_PKT_LENGTH_MAX;
    else
        total_length = length;

    if(start > end)
        return RT_ERR_INPUT;

    pos = end + 1;
    for(i = start; pos < total_length; i++, pos++)
    {
        if(i > end)
            i = start;
        
        pkt_payload[pos] = pkt_payload[i];
    }
    pkt_length = total_length;

    return RT_ERR_OK;
}

int32 rtk_pkt_txBuffer_get(uint8 *pPayload, uint16 max_length, uint16 *pPkt_length)
{
    if((NULL == pPayload) || (NULL == pPkt_length))
        return RT_ERR_NULL_POINTER;

    *pPkt_length = pkt_length;
    osal_memcpy(pPayload, &pkt_payload[0], (pkt_length <= max_length) ? pkt_length : max_length);
    return RT_ERR_OK;
}

int32 rtk_pkt_txBuffer_clear(void)
{
    pkt_length = 0;    
    return RT_ERR_OK;
}

int32 rtk_pkt_tx_send(void)
{
    rtdrv_pktdbg_t pkt_info;

    pkt_info.length = pkt_length;
    pkt_info.tx_info = txInfo;
    osal_memcpy(&pkt_info.buf[0], &pkt_payload[0], (pkt_info.length <= PKT_DEBUG_PKT_LENGTH_MAX) ? pkt_info.length : PKT_DEBUG_PKT_LENGTH_MAX);
    SETSOCKOPT(RTDRV_PKT_SEND, &pkt_info, sizeof(rtdrv_pktdbg_t), 1);
    return RT_ERR_OK;
}

int32 rtk_pkt_txCpuTag_set(struct pkt_dbg_cputag cputag)
{
    txInfo.opts1.bit.keep               = cputag.word2.bit.keep;
    txInfo.opts1.bit.blu                = cputag.word3.bit.l2br;
    txInfo.opts1.bit.vsel               = cputag.word2.bit.vsel;
    txInfo.opts1.bit.dislrn             = cputag.word2.bit.dislrn;
    txInfo.opts1.bit.cputag_ipcs        = cputag.word1.bit.l3cs;
    txInfo.opts1.bit.cputag_l4cs        = cputag.word1.bit.l4cs;
    txInfo.opts1.bit.cputag_psel        = cputag.word2.bit.psel;
    txInfo.opts2.bit.aspri              = cputag.word2.bit.prisel;
    txInfo.opts2.bit.cputag_pri         = cputag.word2.bit.priority;
    txInfo.opts2.bit.tx_pppoe_action    = cputag.word3.bit.pppoeact;
    txInfo.opts2.bit.tx_pppoe_idx       = cputag.word3.bit.pppoeidx;
    txInfo.opts2.bit.efid               = cputag.word2.bit.efid;
    txInfo.opts2.bit.enhance_fid        = cputag.word2.bit.efid_value;
    txInfo.opts3.bit.extspa             = cputag.word3.bit.extspa;
    txInfo.opts3.bit.tx_portmask        = cputag.word1.bit.txmask_vidx;
    txInfo.opts3.bit.tx_dst_stream_id   = cputag.word3.bit.pon_streamid;
    txInfo.opts3.bit.l34keep            = cputag.word2.bit.l34keep;

    return RT_ERR_OK;
}

int32 rtk_pkt_txCpuTag_get(struct pkt_dbg_cputag *pCputag)
{
    if(NULL == pCputag)
        return RT_ERR_NULL_POINTER;

    pCputag->word1.bit.l3cs         = txInfo.opts1.bit.cputag_ipcs;
    pCputag->word1.bit.l4cs         = txInfo.opts1.bit.cputag_l4cs;
    pCputag->word1.bit.txmask_vidx  = txInfo.opts3.bit.tx_portmask;
    pCputag->word2.bit.efid         = txInfo.opts2.bit.efid;
    pCputag->word2.bit.efid_value   = txInfo.opts2.bit.enhance_fid;
    pCputag->word2.bit.prisel       = txInfo.opts2.bit.aspri;
    pCputag->word2.bit.priority     = txInfo.opts2.bit.cputag_pri;
    pCputag->word2.bit.keep         = txInfo.opts1.bit.keep;
    pCputag->word2.bit.vsel         = txInfo.opts1.bit.vsel;
    pCputag->word2.bit.dislrn       = txInfo.opts1.bit.dislrn;
    pCputag->word2.bit.psel         = txInfo.opts1.bit.cputag_psel;
    pCputag->word2.bit.l34keep      = txInfo.opts3.bit.l34keep;
    pCputag->word3.bit.extspa       = txInfo.opts3.bit.extspa;
    pCputag->word3.bit.pppoeact     = txInfo.opts2.bit.tx_pppoe_action;
    pCputag->word3.bit.pppoeidx     = txInfo.opts2.bit.tx_pppoe_idx;
    pCputag->word3.bit.l2br         = txInfo.opts1.bit.blu;
    pCputag->word3.bit.pon_streamid = txInfo.opts3.bit.tx_dst_stream_id;
    
    return RT_ERR_OK;
}

