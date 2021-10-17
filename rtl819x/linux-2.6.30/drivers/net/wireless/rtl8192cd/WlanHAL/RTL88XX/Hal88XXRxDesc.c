/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Hal88XXRxDesc.c
	
Abstract:
	Defined RTL88XX HAL rx desc common function
	    
Major Change History:
	When            Who                         What
	---------- ---------------   -------------------------------
	2012-03-29  Lun-Wu Yeh            Add PrepareRxDesc88XX().	
--*/

#if !defined(__ECOS) && !defined(CPTCFG_CFG80211_MODULE)
#include "HalPrecomp.h"
#else
#include "../HalPrecomp.h"
#endif

#if (HAL_DEV_BUS_TYPE & (HAL_RT_EMBEDDED_INTERFACE | HAL_RT_PCI_INTERFACE))

void DumpRxBDesc88XX(
    IN      HAL_PADAPTER    Adapter,
    IN      u4Byte          q_num 
)
{
	PHCI_RX_DMA_MANAGER_88XX    prx_dma;
	int i=0;
	prx_dma = (PHCI_RX_DMA_MANAGER_88XX)(_GET_HAL_DATA(Adapter)->PRxDMA88XX);

#if RXBD_READY_CHECK_METHOD
	printk(" q_num:%d, hw_idx=%d,host_idx= %d,cur_host_idx:%d, rxtag_seq_num:%d\n", q_num,
		prx_dma->rx_queue[q_num].hw_idx, prx_dma->rx_queue[q_num].host_idx, 
		prx_dma->rx_queue[q_num].cur_host_idx, prx_dma->rx_queue[q_num].rxtag_seq_num);
#else
	printk(" q_num:%d, hw_idx=%d,host_idx= %d,cur_host_idx:%d\n", q_num,
		prx_dma->rx_queue[q_num].hw_idx, prx_dma->rx_queue[q_num].host_idx, 
		prx_dma->rx_queue[q_num].cur_host_idx);
#endif

	printk("total_rxbd_num=%d,avail_rxbd_num= %d,reg_rwptr_idx:%x\n",
		prx_dma->rx_queue[q_num].total_rxbd_num, prx_dma->rx_queue[q_num].avail_rxbd_num, prx_dma->rx_queue[q_num].reg_rwptr_idx);

	printk("RWreg(%x):%08x\n", REG_RXQ_RXBD_IDX, HAL_RTL_R32(REG_RXQ_RXBD_IDX));

#ifdef CONFIG_NET_PCI
	if ((((Adapter->pshare->type >> TYPE_SHIFT) & TYPE_MASK) == TYPE_PCI_BIOS)) {
		u4Byte uiTmp=0;

		for (i=1;i<=q_num;i++){
			uiTmp = _GET_HAL_DATA(Adapter)->ring_dma_addr + prx_dma->rx_queue[i-1].total_rxbd_num * sizeof(RX_BUFFER_DESCRIPTOR);
		}
		printk("pRXBD_head=%08x, %08lx, reg(%x):%08x\n",
		(u4Byte)prx_dma->rx_queue[q_num].pRXBD_head , 
			_GET_HAL_DATA(Adapter)->ring_dma_addr + uiTmp,
		REG_RXQ_RXBD_DESA, HAL_RTL_R32(REG_RXQ_RXBD_DESA));

		for (i=0;i<RX_Q_RXBD_NUM;i++ ){
			printk("pRXBD_head[%d], addr:%08x,%08lx: Dword0: 0x%x, Dword1: 0x%x\n", 
				i, 
				(u4Byte)&prx_dma->rx_queue[q_num].pRXBD_head[i],
				_GET_HAL_DATA(Adapter)->ring_dma_addr + uiTmp + sizeof(RX_BUFFER_DESCRIPTOR)*i,
				(u4Byte)GET_DESC(prx_dma->rx_queue[q_num].pRXBD_head[i].Dword0), 
				(u4Byte)GET_DESC(prx_dma->rx_queue[q_num].pRXBD_head[i].Dword1)     ); 
		}
	} else
#endif
	{
		printk("pRXBD_head=%08x, %08x, reg(%x):%08x\n",
			(u4Byte)prx_dma->rx_queue[q_num].pRXBD_head , 
			HAL_VIRT_TO_BUS1(Adapter, (PVOID)prx_dma->rx_queue[q_num].pRXBD_head,sizeof(RX_BUFFER_DESCRIPTOR) * RX_Q_RXBD_NUM, PCI_DMA_TODEVICE),
			REG_RXQ_RXBD_DESA, HAL_RTL_R32(REG_RXQ_RXBD_DESA));

	    for (i=0;i<RX_Q_RXBD_NUM;i++ ){
			printk("pRXBD_head[%d], addr:%08x,%08x: Dword0: 0x%x, Dword1: 0x%x\n", 
				i, 
				(u4Byte)&prx_dma->rx_queue[q_num].pRXBD_head[i],
				(u4Byte)HAL_VIRT_TO_BUS1(Adapter, (PVOID)&prx_dma->rx_queue[q_num].pRXBD_head[i],sizeof(RX_BUFFER_DESCRIPTOR), PCI_DMA_TODEVICE),
				(u4Byte)GET_DESC(prx_dma->rx_queue[q_num].pRXBD_head[i].Dword0), 
                    (u4Byte)GET_DESC(prx_dma->rx_queue[q_num].pRXBD_head[i].Dword1)     ); 
	    }	
    }
}

typedef void (*INIT_RXBUF_FUNC)(HAL_PADAPTER Adapter, PVOID pSkb, u2Byte rxbd_idx, pu4Byte pBufAddr, pu4Byte pBufLen);

//
//  bufferLen: SKB Buffer Size (Linux, SKB Format)(RXDESC + Payload)
//
RT_STATUS
PrepareRXBD88XX(
    IN      HAL_PADAPTER    Adapter,
    IN      u2Byte          bufferLen,
    IN      PVOID           Callback    // callback function
)
{
    PHCI_RX_DMA_MANAGER_88XX    prx_dma;
    HCI_RX_DMA_QUEUE_88XX       q_num;
    u2Byte                      rxbd_idx;

    pu1Byte                     pdesc_dma_buf, desc_dma_buf_start;
    u4Byte                      value32 = 0;
    PHAL_BUF                    pbuf;

    u4Byte RXBD_NUM[HCI_RX_DMA_QUEUE_MAX_NUM] =
    {
        RX_Q_RXBD_NUM
    };
    
    u4Byte RXBD_Reg[HCI_RX_DMA_QUEUE_MAX_NUM] =
    {
        REG_RXQ_RXBD_DESA
    };

    u4Byte RXBD_RWPtr_Reg[HCI_RX_DMA_QUEUE_MAX_NUM] =
    {
        REG_RXQ_RXBD_IDX
    };

    prx_dma = (PHCI_RX_DMA_MANAGER_88XX)(_GET_HAL_DATA(Adapter)->PRxDMA88XX);
    PlatformZeroMemory(prx_dma, sizeof(HCI_RX_DMA_MANAGER_88XX));

#ifdef CONFIG_NET_PCI
    if ((((Adapter->pshare->type >> TYPE_SHIFT) & TYPE_MASK) == TYPE_PCI_BIOS)) {
        unsigned long tmp_dma_ring_addr =0;
#ifndef TRXBD_CACHABLE_REGION		
        PlatformZeroMemory((void*)_GET_HAL_DATA(Adapter)->alloc_dma_buf, DESC_DMA_PAGE_SIZE_MAX_HAL);
#endif
        pdesc_dma_buf = (pu1Byte)(_GET_HAL_DATA(Adapter)->ring_virt_addr);
        printk("%s(%d):size=%d, ring_dma_addr:%08lx, alloc_dma_buf:%08lx, ring_virt_addr:%08lx\n", __FUNCTION__,__LINE__, DESC_DMA_PAGE_SIZE_MAX_HAL, _GET_HAL_DATA(Adapter)->ring_dma_addr, _GET_HAL_DATA(Adapter)->alloc_dma_buf, (_GET_HAL_DATA(Adapter)->ring_virt_addr));
        prx_dma = (PHCI_RX_DMA_MANAGER_88XX)(_GET_HAL_DATA(Adapter)->PRxDMA88XX);

        //Transfer to Non-cachable address, need this??
#ifdef TRXBD_CACHABLE_REGION
        pdesc_dma_buf =  (pu1Byte)((unsigned long)pdesc_dma_buf | 0x20000000);
        PlatformZeroMemory((void*)_GET_HAL_DATA(Adapter)->alloc_dma_buf, DESC_DMA_PAGE_SIZE_MAX_HAL);        
        printk("%s(%d): pdesc_dma_buf:%08x, size:%x \n", __FUNCTION__,__LINE__, pdesc_dma_buf, sizeof(RX_BUFFER_DESCRIPTOR) * RXBD_NUM[0]);

        dma_cache_inv((unsigned long)bus_to_virt((pdesc_dma_buf&(~0x20000000))-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), _GET_HAL_DATA(Adapter)->alloc_dma_buf);
#endif
        for (q_num = 0; q_num < HCI_RX_DMA_QUEUE_MAX_NUM; q_num++)
        {
            prx_dma->rx_queue[q_num].hw_idx         = 0;
            prx_dma->rx_queue[q_num].host_idx       = 0;
            prx_dma->rx_queue[q_num].cur_host_idx   = 0;
#if RXBD_READY_CHECK_METHOD        
            prx_dma->rx_queue[q_num].rxtag_seq_num  = 0;
#endif
#if CFG_HAL_DELAY_REFILL_RX_BUF
            prx_dma->rx_queue[q_num].rxbd_ok_cnt    = 0;
#endif
            prx_dma->rx_queue[q_num].total_rxbd_num = RXBD_NUM[q_num];
            prx_dma->rx_queue[q_num].avail_rxbd_num = RXBD_NUM[q_num];
            prx_dma->rx_queue[q_num].reg_rwptr_idx  = RXBD_RWPtr_Reg[q_num];

            if ( 0 == q_num ) {
                prx_dma->rx_queue[q_num].pRXBD_head = (PRX_BUFFER_DESCRIPTOR)pdesc_dma_buf;
                tmp_dma_ring_addr = _GET_HAL_DATA(Adapter)->ring_dma_addr;
            }
            else {
                prx_dma->rx_queue[q_num].pRXBD_head = prx_dma->rx_queue[q_num-1].pRXBD_head + \
                                                  sizeof(RX_BUFFER_DESCRIPTOR) * RXBD_NUM[q_num-1];
                tmp_dma_ring_addr = tmp_dma_ring_addr + sizeof(RX_BUFFER_DESCRIPTOR) * RXBD_NUM[q_num-1];
            }

            printk ("QNum: 0x%x, RXBDHead: 0x%x, tmp_dma_ring_addr:0x%08lx\n", 
                   (u4Byte)q_num, (u4Byte)(prx_dma->rx_queue[q_num].pRXBD_head), tmp_dma_ring_addr);

            HAL_RTL_W32(RXBD_Reg[q_num], tmp_dma_ring_addr);
 
            //Init Read/Write Pointer for RX queue
            HAL_RTL_W32(RXBD_RWPtr_Reg[q_num], 0);

            for(rxbd_idx = 0; rxbd_idx < RXBD_NUM[q_num]; rxbd_idx++)
            {
//            struct sk_buff *skb = dev_alloc_skb(RX_BUF_LEN + sizeof(RX_DESC_88XX));
				pbuf = HAL_OS_malloc(Adapter, bufferLen, _SKB_RX_, TRUE);
                if ( NULL == pbuf ) {
                    panic_printk("%s:%d Allocate HAL Memory Failed\n", __FUNCTION__, __LINE__);
                    return RT_STATUS_FAILURE;
                }
                else {
                    UpdateRXBDInfo88XX(Adapter, q_num, rxbd_idx, (pu1Byte)pbuf, Callback, _TRUE);

#if 0
                RT_TRACE_F(COMP_INIT, DBG_TRACE, ("pRXBD_head[%d]: Dword0: 0x%lx, Dword1: 0x%lx\n", \
                                                 rxbd_idx, \
                                                 (u4Byte)GET_DESC(prx_dma->rx_queue[q_num].pRXBD_head[rxbd_idx].Dword0), \
                                                 (u4Byte)GET_DESC(prx_dma->rx_queue[q_num].pRXBD_head[rxbd_idx].Dword1)  \
                                                 ));                
#endif

                }
            }
        }
		DumpRxBDesc88XX(Adapter, 0);
        return RT_STATUS_SUCCESS;
    } 
#endif

    desc_dma_buf_start = _GET_HAL_DATA(Adapter)->desc_dma_buf;
	
	PlatformZeroMemory(desc_dma_buf_start, _GET_HAL_DATA(Adapter)->desc_dma_buf_len);
	HAL_CACHE_SYNC_WBACK(Adapter, (unsigned long)((PVOID)(desc_dma_buf_start)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), _GET_HAL_DATA(Adapter)->desc_dma_buf_len, PCI_DMA_TODEVICE);
	pdesc_dma_buf = (pu1Byte)(((unsigned long)desc_dma_buf_start) + \
		(HAL_PAGE_SIZE - (((unsigned long)desc_dma_buf_start) & (HAL_PAGE_SIZE-1))));

    //Transfer to Non-cachable address
#ifdef TRXBD_CACHABLE_REGION    
  // Do nothing for un-cachable      
#else
    pdesc_dma_buf =  (pu1Byte)HAL_TO_NONCACHE_ADDR((u4Byte)pdesc_dma_buf);
#endif	

    for (q_num = 0; q_num < HCI_RX_DMA_QUEUE_MAX_NUM; q_num++)
    {
        prx_dma->rx_queue[q_num].hw_idx         = 0;
        prx_dma->rx_queue[q_num].host_idx       = 0;
        prx_dma->rx_queue[q_num].cur_host_idx   = 0;
#if RXBD_READY_CHECK_METHOD
        prx_dma->rx_queue[q_num].rxtag_seq_num  = 0;
#endif
#if CFG_HAL_DELAY_REFILL_RX_BUF
        prx_dma->rx_queue[q_num].rxbd_ok_cnt    = 0;
#endif
        prx_dma->rx_queue[q_num].total_rxbd_num = RXBD_NUM[q_num];
        prx_dma->rx_queue[q_num].avail_rxbd_num = RXBD_NUM[q_num];
        prx_dma->rx_queue[q_num].reg_rwptr_idx  = RXBD_RWPtr_Reg[q_num];

        if ( 0 == q_num ) {
            prx_dma->rx_queue[q_num].pRXBD_head = (PRX_BUFFER_DESCRIPTOR)pdesc_dma_buf;
        }
        else {
            prx_dma->rx_queue[q_num].pRXBD_head = prx_dma->rx_queue[q_num-1].pRXBD_head + \
                                                  sizeof(RX_BUFFER_DESCRIPTOR) * RXBD_NUM[q_num-1];
        }

        RT_TRACE_F(COMP_INIT, DBG_TRACE, ("QNum: 0x%lx, RXBDHead: 0x%lx\n", \
                                            (u4Byte)q_num, \
                                            (u4Byte)(prx_dma->rx_queue[q_num].pRXBD_head) \
                                            ));

        HAL_RTL_W32(RXBD_Reg[q_num], \
                        HAL_VIRT_TO_BUS1(Adapter, (PVOID)prx_dma->rx_queue[q_num].pRXBD_head, \
                        sizeof(RX_BUFFER_DESCRIPTOR) * RXBD_NUM[q_num], HAL_PCI_DMA_TODEVICE) + CONFIG_LUNA_SLAVE_PHYMEM_OFFSET_HAL);
 
        //Init Read/Write Pointer for RX queue
        HAL_RTL_W32(RXBD_RWPtr_Reg[q_num], 0);

        for(rxbd_idx = 0; rxbd_idx < RXBD_NUM[q_num]; rxbd_idx++)
        {
            pbuf = HAL_OS_malloc(Adapter, bufferLen, _SKB_RX_, TRUE);
            if ( NULL == pbuf ) {
                panic_printk("%s:%d [%d]Allocate HAL Memory Failed\n", __FUNCTION__, __LINE__, rxbd_idx);
                return RT_STATUS_FAILURE;
            }
            else {
                UpdateRXBDInfo88XX(Adapter, q_num, rxbd_idx, (pu1Byte)pbuf, Callback, _TRUE);

/*                    RT_TRACE_F(COMP_INIT, DBG_TRACE, ("pRXBD_head[%d]: Dword0: 0x%lx, Dword1: 0x%lx\n",
                                                 rxbd_idx,
                                                 (u4Byte)GET_DESC(prx_dma->rx_queue[q_num].pRXBD_head[rxbd_idx].Dword0),
                                                 (u4Byte)GET_DESC(prx_dma->rx_queue[q_num].pRXBD_head[rxbd_idx].Dword1)
                                                 ));  */
            }
        }
    }

    return RT_STATUS_SUCCESS;
}

HAL_IMEM
RT_STATUS
UpdateRXBDInfo88XX(
    IN      HAL_PADAPTER    Adapter,
    IN      u4Byte          queueIndex,  //HCI_RX_DMA_QUEUE_88XX
    IN      u2Byte          rxbd_idx,
    IN      pu1Byte         pBuf,
    IN      PVOID           Callback,    // callback function    
    IN      BOOLEAN         bInit
)
{
    PHCI_RX_DMA_MANAGER_88XX    prx_dma;
    INIT_RXBUF_FUNC             InitRXDescFunc = (INIT_RXBUF_FUNC)Callback; 
    u4Byte                      bufAddr;
    u4Byte                      bufLen;
	PRX_BUFFER_DESCRIPTOR		pRXBD_head, pRXBD_head1;
    prx_dma = (PHCI_RX_DMA_MANAGER_88XX)(_GET_HAL_DATA(Adapter)->PRxDMA88XX);    

    InitRXDescFunc(Adapter, pBuf, rxbd_idx, &bufAddr, &bufLen);

#if 0
//#ifdef CFG_HAL_DBG

    RT_TRACE_F(COMP_RECV, DBG_TRACE, ("rxbd_idx:0x%lx, bufAddr:0x%lx, phy(bufAddr):0x%lx\n",
        rxbd_idx, bufAddr, HAL_VIRT_TO_BUS1(Adapter, (PVOID)bufAddr, bufLen, HAL_PCI_DMA_TODEVICE)));

#endif 
	
	pRXBD_head = &(prx_dma->rx_queue[queueIndex].pRXBD_head[rxbd_idx]);
#ifdef TRXBD_CACHABLE_REGION
	pRXBD_head = (PRX_BUFFER_DESCRIPTOR)((unsigned long)pRXBD_head | 0x20000000);
#endif
#if 0 //Filen_test
    if ( _TRUE == bInit ) {    
        SET_DESC_FIELD_CLR(prx_dma->rx_queue[queueIndex].pRXBD_head[rxbd_idx].Dword0,
            bufLen,
            RXBD_DW0_RXBUFSIZE_MSK, RXBD_DW0_RXBUFSIZE_SH);
    }
#else
    SET_DESC_FIELD_CLR(pRXBD_head->Dword0,
        bufLen,
        RXBD_DW0_RXBUFSIZE_MSK, RXBD_DW0_RXBUFSIZE_SH);
#endif

    SET_DESC_FIELD_CLR(pRXBD_head->Dword1,
        HAL_VIRT_TO_BUS1(Adapter, (PVOID)bufAddr, bufLen, HAL_PCI_DMA_TODEVICE) + CONFIG_LUNA_SLAVE_PHYMEM_OFFSET_HAL,
        RXBD_DW1_PHYADDR_LOW_MSK, RXBD_DW1_PHYADDR_LOW_SH);

#if RXBD_READY_CHECK_METHOD
    if ( _TRUE == bInit ) {
        SET_DESC_FIELD_CLR(pRXBD_head->Dword0,
                0xFFFF, RXBD_DW0_TOTALRXPKTSIZE_MSK, RXBD_DW0_TOTALRXPKTSIZE_SH);
    }
#else
    SET_DESC_FIELD_CLR(pRXBD_head->Dword0,
                0, RXBD_DW0_TOTALRXPKTSIZE_MSK, RXBD_DW0_TOTALRXPKTSIZE_SH);
#endif  //RXBD_READY_CHECK_METHOD

#ifdef CONFIG_NET_PCI
     if (((Adapter->pshare->type >> TYPE_SHIFT) & TYPE_MASK) == TYPE_PCI_BIOS) {
         HAL_CACHE_SYNC_WBACK(Adapter,
            _GET_HAL_DATA(Adapter)->ring_dma_addr + rxbd_idx * sizeof(RX_BUFFER_DESCRIPTOR),
            sizeof(RX_BUFFER_DESCRIPTOR), HAL_PCI_DMA_TODEVICE);
     } else   
#endif
#ifdef TRXBD_CACHABLE_REGION
    _dma_cache_wback((unsigned long)((PVOID)(pRXBD_head)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(RX_BUFFER_DESCRIPTOR));
#if 0
	// For test _dma_cache_wback fail issue
	if(pRXBD_head->Dword1 != pRXBD_head1->Dword1 || pRXBD_head->Dword0 != pRXBD_head1->Dword0) {
		panic_printk("Dword0:%x,%x, Dword1:%x%x\n",pRXBD_head->Dword0,pRXBD_head1->Dword0,pRXBD_head->Dword1,pRXBD_head1->Dword1);
	}
#endif	
#else
    HAL_CACHE_SYNC_WBACK(Adapter,
        HAL_VIRT_TO_BUS1(Adapter, (PVOID)(pRXBD_head), sizeof(RX_BUFFER_DESCRIPTOR), HAL_PCI_DMA_TODEVICE),
        sizeof(RX_BUFFER_DESCRIPTOR), HAL_PCI_DMA_TODEVICE);
#endif
    if ( 0 == GET_DESC(pRXBD_head->Dword1) ) {
        RT_TRACE(COMP_INIT, DBG_SERIOUS, ("Address(0x%lx) Error\n",  pRXBD_head->Dword1));
        return RT_STATUS_FAILURE;
    }

    return RT_STATUS_SUCCESS;
}


void
DumpRxPktContent88XX(
    IN      HAL_PADAPTER    Adapter,
    IN      u4Byte          queueIndex  //HCI_RX_DMA_QUEUE_88XX
)
{
#if 0
    PHCI_RX_DMA_MANAGER_88XX        prx_dma;
    PHCI_RX_DMA_QUEUE_STRUCT_88XX   cur_q;
    PRX_DESC_88XX                   prx_desc;
    // TODO: no initial value for prx_desc 

    prx_dma         = (PHCI_RX_DMA_MANAGER_88XX)(_GET_HAL_DATA(Adapter)->PRxDMA88XX);
    cur_q           = &(prx_dma->rx_queue[queueIndex]);


    //RXBD
    RT_TRACE_F(COMP_RECV, DBG_TRACE, ("\n\nRXBD[%ld]:\nDword0=0x%lx, Dword1=0x%lx \n", \
                                        (u4Byte)cur_q->cur_host_idx, \
                                        (u4Byte)GET_DESC(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0), \
                                        (u4Byte)GET_DESC(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword1) \
                                        ));

    //RXDESC
    RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "RXDESC:\n", prx_desc, sizeof(RX_DESC_88XX));

    //PHYStatus
    RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "PHYStatus:\n", ((pu1Byte)prx_desc + SIZE_RXDESC_88XX), (GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_DRV_INFO_SIZE_MSK, RX_DW0_DRV_INFO_SIZE_SH)<<3) + GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_SHIFT_MSK, RX_DW0_SHIFT_SH));

    //RXPkt
    RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "Payload:\n", (pu1Byte)prx_desc + SIZE_RXDESC_88XX + (GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_DRV_INFO_SIZE_MSK, RX_DW0_DRV_INFO_SIZE_SH)<<3) + GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_SHIFT_MSK, RX_DW0_SHIFT_SH), \
                                GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_PKT_LEN_MSK, RX_DW0_PKT_LEN_SH));
#endif
}

#define RXBD_RXTAG_POLLING_CNT  100
#define RXBD_RXTAG_MASK         0x1FFF

HAL_IMEM
RT_STATUS
QueryRxDesc88XX (
    IN      HAL_PADAPTER    Adapter,
    IN      u4Byte          queueIndex,  //HCI_RX_DMA_QUEUE_88XX
    IN      pu1Byte         pBufAddr,
    OUT     PVOID           pRxDescStatus
)
{
    BOOLEAN                         bResult = SUCCESS;
    u4Byte                          PollingCnt = 0;
    PHCI_RX_DMA_MANAGER_88XX        prx_dma;
    PHCI_RX_DMA_QUEUE_STRUCT_88XX   cur_q;
    PRX_DESC_STATUS_88XX            prx_desc_status;
    PRX_DESC_88XX                   prx_desc;
#if CFG_HAL_DBG
    u4Byte                          crc32;
    pu1Byte                         pFrame;
    u2Byte                          frameLen;
    BOOLEAN                         bDivisionCRC = _FALSE;
    u2Byte                          cnt = 0;
    u2Byte                          shift = 0;
    u1Byte                          tempByte;
    u4Byte                          rx_crc32;
#endif

    prx_dma         = (PHCI_RX_DMA_MANAGER_88XX)(_GET_HAL_DATA(Adapter)->PRxDMA88XX);
    cur_q           = &(prx_dma->rx_queue[queueIndex]);

    prx_desc_status = (PRX_DESC_STATUS_88XX)pRxDescStatus;

    do {

#if RXBD_READY_CHECK_METHOD
#ifdef TRXBD_CACHABLE_REGION
        _dma_cache_inv((unsigned long)((&(cur_q->pRXBD_head[cur_q->cur_host_idx]))-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), 
            sizeof(RX_BUFFER_DESCRIPTOR));
#endif

        if ( cur_q->rxtag_seq_num != GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0, RXBD_DW0_TOTALRXPKTSIZE_MSK, RXBD_DW0_TOTALRXPKTSIZE_SH) ) {
            RT_TRACE(COMP_RECV, DBG_WARNING, ("Polling failed(cnt: %d), keep trying, DW0(0x%x), RXBDCheckRdySeqNum(0x%x) FS,LS(0x%x,0x%x)\n", 
                                            PollingCnt, 
                                            GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0, RXBD_DW0_TOTALRXPKTSIZE_MSK, RXBD_DW0_TOTALRXPKTSIZE_SH), 
                                            cur_q->rxtag_seq_num,
                                            GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0, RXBD_DW0_FS_MSK, RXBD_DW0_FS_SH),
                                            GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0, RXBD_DW0_LS_MSK, RXBD_DW0_LS_SH)
                                            ));
        }
        else {
            break;
        }
#else
        if (0 == GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0, RXBD_DW0_TOTALRXPKTSIZE_MSK, RXBD_DW0_TOTALRXPKTSIZE_SH)) {
            RT_TRACE(COMP_RECV, DBG_WARNING, ("Polling failed(cnt: %d), keep trying, DW0(0x%x)\n", PollingCnt, GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0, RXBD_DW0_TOTALRXPKTSIZE_MSK, RXBD_DW0_TOTALRXPKTSIZE_SH)));
        }
        else {
            break;
        }
#endif   //RXBD_READY_CHECK_METHOD     

        PollingCnt++;
        //HAL_delay_ms(1);
    } while(PollingCnt < RXBD_RXTAG_POLLING_CNT);


    if ( PollingCnt >= RXBD_RXTAG_POLLING_CNT ) {

        RT_TRACE(COMP_RECV, DBG_SERIOUS, ("Polling failed(0x%x)\n", Adapter->pshare->RxTagPollingCount));

		Adapter->pshare->RxTagPollingCount++;
		Adapter->pshare->RxTagMismatchCount++;

#if CFG_HAL_DBG        
        //code below in order to dump packet
        bResult = FAIL;
        prx_desc_status->FS = 1;
        prx_desc_status->LS = 1;
        goto _RXPKT_DUMP;
#else
//        return RT_STATUS_FAILURE;

#if RXBD_READY_CHECK_METHOD
		cur_q->rxtag_seq_num = GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0, RXBD_DW0_TOTALRXPKTSIZE_MSK, RXBD_DW0_TOTALRXPKTSIZE_SH);
#endif
#endif  //#if  CFG_HAL_DBG        

    }
    else {
		Adapter->pshare->RxTagPollingCount=0;
    }
#if RXBD_READY_CHECK_METHOD
			cur_q->rxtag_seq_num++;
			cur_q->rxtag_seq_num &= RXBD_RXTAG_MASK;
#endif  //#if RXBD_READY_CHECK_METHOD


    #if 0
    //Cache flush for current RXDESC, becuase we don't flush rxdesc in some cases. ex. memory allocate fail then reuse
    // TODO: move to other better place
    HAL_CACHE_SYNC_WBACK(Adapter,
        HAL_VIRT_TO_BUS1(Adapter, (PVOID)prx_desc, sizeof(RX_DESC_88XX), HAL_PCI_DMA_TODEVICE),
        sizeof(RX_DESC_88XX), HAL_PCI_DMA_TODEVICE);    
    #endif

    #if 0
    HAL_CACHE_SYNC_WBACK(Adapter,
        HAL_VIRT_TO_BUS1(Adapter, (PVOID)(prx_desc), 2048, HAL_PCI_DMA_TODEVICE),
        2048, HAL_PCI_DMA_TODEVICE);
    #endif
   

    // get rxbd    
    prx_desc_status->FS = GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0,
                                            RXBD_DW0_FS_MSK, RXBD_DW0_FS_SH);
    prx_desc_status->LS = GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0,
                                            RXBD_DW0_LS_MSK, RXBD_DW0_LS_SH);
    prx_desc_status->RXBuffSize = GET_DESC_FIELD(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0,
                                            RXBD_DW0_RXBUFSIZE_MSK, RXBD_DW0_RXBUFSIZE_SH);

    if ( prx_desc_status->FS==0x01 ) {
        prx_desc                        = (PRX_DESC_88XX)pBufAddr;
        // get rx desc
        prx_desc_status->PKT_LEN        = GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_PKT_LEN_MSK, RX_DW0_PKT_LEN_SH);
        prx_desc_status->CRC32          = GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_CRC32_MSK, RX_DW0_CRC32_SH);
        prx_desc_status->ICVERR         = GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_ICVERR_MSK, RX_DW0_ICVERR_SH);
        prx_desc_status->DRV_INFO_SIZE  = GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_DRV_INFO_SIZE_MSK, RX_DW0_DRV_INFO_SIZE_SH)<<3;
        prx_desc_status->SHIFT          = GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_SHIFT_MSK, RX_DW0_SHIFT_SH);
        prx_desc_status->PHYST          = GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_PHYST_MSK, RX_DW0_PHYST_SH);
        prx_desc_status->SWDEC          = GET_DESC_FIELD(prx_desc->Dword0, RX_DW0_SWDEC_MSK, RX_DW0_SWDEC_SH);   
        prx_desc_status->PAGGR          = GET_DESC_FIELD(prx_desc->Dword1, RX_DW1_PAGGR_MSK, RX_DW1_PAGGR_SH);
        prx_desc_status->C2HPkt         = GET_DESC_FIELD(prx_desc->Dword2, RX_DW2_C2HPKT_MSK, RX_DW2_C2HPKT_SH);
        prx_desc_status->RX_RATE        = GET_DESC_FIELD(prx_desc->Dword3, RX_DW3_RX_RATE_MSK, RX_DW3_RX_RATE_SH);

#if (IS_RTL8192E_SERIES || IS_RTL8881A_SERIES)
    if (IS_HARDWARE_TYPE_8192E(Adapter) || IS_HARDWARE_TYPE_8881A(Adapter)) {        
        prx_desc_status->OFDM_SGI       = GET_DESC_FIELD(prx_desc->Dword4, RX_DW4_OFDM_SGI_MSK, RX_DW4_OFDM_SGI_SH);
    }
#endif //#if (IS_RTL8192E_SERIES || IS_RTL8881A_SERIES)

        prx_desc_status->BW             = GET_DESC_FIELD(prx_desc->Dword4, RX_DW4_BW_MSK, RX_DW4_BW_SH);
#if CFG_HAL_HW_FILL_MACID
        if (IS_HARDWARE_TYPE_8813A(Adapter)) {        
            prx_desc_status->rxMACID        = GET_DESC_FIELD(prx_desc->Dword4, RX_DW4_MACID_MSK, RX_DW4_MACID_SH);     
        }
#endif //#if IS_RTL8813A_SERIES

#if 0 //CFG_HAL_DBG
        RT_TRACE_F(COMP_RECV, DBG_TRACE, ("pBufAddr: 0x%lx\n", (u4Byte)pBufAddr));

        RT_TRACE_F(COMP_RECV, DBG_TRACE, ("RXBuffSize: 0x%lx, fs,ls:(0x%lx,0x%lx), PKT_LEN:0x%lx, DRV_INFO:0x%lx, SHIFT:0x%lx\n", \
                            prx_desc_status->RXBuffSize, prx_desc_status->FS, prx_desc_status->LS, \
                            prx_desc_status->PKT_LEN, prx_desc_status->DRV_INFO_SIZE, prx_desc_status->SHIFT));
#endif        
    }

_RXPKT_DUMP:

#if 0 // CFG_HAL_DBG
    //RXBD
    RT_TRACE_F(COMP_RECV, DBG_TRACE, ("RXBD[%ld]:\nDword0=0x%lx, Dword1=0x%lx \n",
                                        (u4Byte)cur_q->cur_host_idx, 
                                        (u4Byte)GET_DESC(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword0), 
                                        (u4Byte)GET_DESC(cur_q->pRXBD_head[cur_q->cur_host_idx].Dword1)
										));

    if ( prx_desc_status->FS==0x01 && prx_desc_status->LS==0x01 ) {
        //RXDESC
        RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "RXDESC:(FS=1 & LS=1)\n", prx_desc, sizeof(RX_DESC_88XX));

        //PHYStatus
//        RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "PHYStatus:\n", ((pu1Byte)prx_desc + SIZE_RXDESC_88XX), prx_desc_status->DRV_INFO_SIZE + prx_desc_status->SHIFT);

        //RXPkt
        pFrame = (pu1Byte)prx_desc + SIZE_RXDESC_88XX + prx_desc_status->DRV_INFO_SIZE + prx_desc_status->SHIFT;
        RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "Payload:\n", pFrame, prx_desc_status->PKT_LEN);

#if 0
        crc32 = 0;
        SoftwareCRC32(pFrame, prx_desc_status->PKT_LEN - _CRCLNG_, &crc32);

        if (HAL_memcmp(pFrame + prx_desc_status->PKT_LEN - _CRCLNG_, (PVOID)&crc32, _CRCLNG_)) {
            RT_TRACE_F(COMP_RECV, DBG_WARNING, ("SW CRC32 error. 0x%lx\n", crc32));
        } else {
            RT_TRACE_F(COMP_RECV, DBG_TRACE, ("SW CRC32 ok. 0x%lx\n", crc32) );
        }
#endif        
    }
    else if ( prx_desc_status->FS==0x01 && prx_desc_status->LS==0x0 ) {
        //RXDESC
        RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "RXDESC:(FS=1 & LS=0)\n", prx_desc, sizeof(RX_DESC_88XX));

        //PHYStatus
        RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "PHYStatus:\n", ((pu1Byte)prx_desc + SIZE_RXDESC_88XX), prx_desc_status->DRV_INFO_SIZE + prx_desc_status->SHIFT);        

        //Partial RXPkt
        pFrame = (pu1Byte)prx_desc + SIZE_RXDESC_88XX + prx_desc_status->DRV_INFO_SIZE + prx_desc_status->SHIFT;
        frameLen = prx_desc_status->RXBuffSize - SIZE_RXDESC_88XX - prx_desc_status->DRV_INFO_SIZE - prx_desc_status->SHIFT;
        RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "Payload:\n", pFrame, frameLen);

        prx_desc_status->pktNum = 0;
        prx_desc_status->pktBufAddr[prx_desc_status->pktNum] = pFrame;
        prx_desc_status->pktBufLen[prx_desc_status->pktNum]  = frameLen;
        RT_TRACE_F(COMP_RECV, DBG_TRACE, ("pktBufLen[0x%lx]:0x%lx,%d\n", \
                      prx_desc_status->pktNum, prx_desc_status->pktBufLen[prx_desc_status->pktNum], prx_desc_status->pktBufLen[prx_desc_status->pktNum]) );
        prx_desc_status->pktNum++;
        prx_desc_status->remaining_pkt_len = SIZE_RXDESC_88XX + prx_desc_status->DRV_INFO_SIZE + prx_desc_status->SHIFT + \
                                                prx_desc_status->PKT_LEN - prx_desc_status->RXBuffSize;
    }
    else if ( prx_desc_status->FS==0x0 && prx_desc_status->LS==0x0 ) {
        // No RXDESC
        //Partial RXPkt
        RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "Payload:(FS=0 & LS=0)\n", (pu1Byte)pBufAddr, prx_desc_status->RXBuffSize);

        prx_desc_status->pktBufAddr[prx_desc_status->pktNum] = pBufAddr;
        prx_desc_status->pktBufLen[prx_desc_status->pktNum]  = prx_desc_status->RXBuffSize;
        RT_TRACE_F(COMP_RECV, DBG_TRACE, ("pktBufLen[0x%lx]:0x%lx,%d\n", \
                      prx_desc_status->pktNum, prx_desc_status->pktBufLen[prx_desc_status->pktNum], prx_desc_status->pktBufLen[prx_desc_status->pktNum]) );
        prx_desc_status->pktNum++;

        prx_desc_status->remaining_pkt_len -= prx_desc_status->RXBuffSize;
    }
    else if ( prx_desc_status->FS==0x0 && prx_desc_status->LS==0x1 ) {
        // No RXDESC
        //Partial RXPkt

        RT_PRINT_DATA(COMP_RECV, DBG_TRACE, "Payload:(FS=0 & LS=1)\n", (pu1Byte)pBufAddr, prx_desc_status->remaining_pkt_len);

        prx_desc_status->pktBufAddr[prx_desc_status->pktNum] = pBufAddr;
        prx_desc_status->pktBufLen[prx_desc_status->pktNum]  = prx_desc_status->remaining_pkt_len;
        RT_TRACE_F(COMP_RECV, DBG_TRACE, ("pktBufLen[0x%lx]:0x%lx,%d\n", \
                    prx_desc_status->pktNum, prx_desc_status->pktBufLen[prx_desc_status->pktNum], prx_desc_status->pktBufLen[prx_desc_status->pktNum]) );

        prx_desc_status->pktNum++;
        crc32 = 0;

        // for special case: CRC in last one and two packets...
        if (prx_desc_status->pktBufLen[prx_desc_status->pktNum-1] < _CRCLNG_) {            
            prx_desc_status->pktBufLen[prx_desc_status->pktNum-2] = prx_desc_status->pktBufLen[prx_desc_status->pktNum-2] + prx_desc_status->pktBufLen[prx_desc_status->pktNum-1] - _CRCLNG_;
            prx_desc_status->pktBufLen[prx_desc_status->pktNum-1] = 0;
            bDivisionCRC = _TRUE;
            printk("special case: CRC in last one and two packets.\n");
        } else {
            prx_desc_status->pktBufLen[prx_desc_status->pktNum-1] -= _CRCLNG_;
        }
       
        SoftwareCRC32_RXBuffGather(&(prx_desc_status->pktBufAddr[0]), &(prx_desc_status->pktBufLen[0]), prx_desc_status->pktNum, &crc32);

        if (bDivisionCRC == _TRUE) {
            while(cnt < (_CRCLNG_ - prx_desc_status->remaining_pkt_len)) {
                tempByte = *((pu1Byte)prx_desc_status->pktBufAddr[prx_desc_status->pktNum-2] + \
                            prx_desc_status->pktBufLen[prx_desc_status->pktNum-2] + cnt);
                HAL_memcpy((pu1Byte)&rx_crc32+shift, (pu1Byte)&tempByte, sizeof(tempByte));
                cnt++;
                shift++;
            }

            cnt = 0;
            while(cnt < prx_desc_status->remaining_pkt_len) {
                tempByte = *((pu1Byte)prx_desc_status->pktBufAddr[prx_desc_status->pktNum-1] + cnt);
                HAL_memcpy((pu1Byte)&rx_crc32+shift, (pu1Byte)&tempByte, sizeof(tempByte));
                cnt++;              
                shift++;
            }

            if ( HAL_memcmp( (PVOID)&rx_crc32, (PVOID)&crc32, _CRCLNG_) ) {
                RT_TRACE_F(COMP_RECV, DBG_WARNING, ("SW CRC32 error in division case. 0x%lx\n", crc32));
            } else {
                RT_TRACE_F(COMP_RECV, DBG_TRACE, ("SW CRC32 ok in division case. 0x%lx\n", crc32) );
            }            
        } else {
            if (HAL_memcmp(pBufAddr + prx_desc_status->remaining_pkt_len - _CRCLNG_, (PVOID)&crc32, _CRCLNG_)) {
                RT_TRACE_F(COMP_RECV, DBG_WARNING, ("SW CRC32 error. 0x%lx\n", crc32));
            } else {
                RT_TRACE_F(COMP_RECV, DBG_TRACE, ("SW CRC32 ok. 0x%lx\n", crc32) );
            }      
        }
    }
#endif

    if ( SUCCESS == bResult ) {
        return RT_STATUS_SUCCESS;
    }
    else {
        return RT_STATUS_FAILURE;
    }
}

HAL_IMEM
u2Byte	
UpdateRXBDHWIdx88XX(
    IN  HAL_PADAPTER    Adapter,
	IN  u4Byte		    queueIndex  //HCI_RX_DMA_QUEUE_88XX
)
{    
    PHCI_RX_DMA_MANAGER_88XX        prx_dma;
    PHCI_RX_DMA_QUEUE_STRUCT_88XX   cur_q;

    prx_dma = (PHCI_RX_DMA_MANAGER_88XX)(_GET_HAL_DATA(Adapter)->PRxDMA88XX);
    cur_q   = &(prx_dma->rx_queue[queueIndex]);

    cur_q->hw_idx = HAL_RTL_R16(cur_q->reg_rwptr_idx + 2) & 0xFFF;
    
    return cur_q->hw_idx;
}

HAL_IMEM
VOID
UpdateRXBDHostIdx88XX (
    IN      HAL_PADAPTER    Adapter,
    IN      u4Byte          queueIndex,  //HCI_RX_DMA_QUEUE_88XX
    IN      u4Byte          count
)
{
    PHCI_RX_DMA_MANAGER_88XX        prx_dma;
    PHCI_RX_DMA_QUEUE_STRUCT_88XX   cur_q;

    prx_dma = (PHCI_RX_DMA_MANAGER_88XX)(_GET_HAL_DATA(Adapter)->PRxDMA88XX);
    cur_q   = &(prx_dma->rx_queue[queueIndex]);

    if ( 0 != count ) {
        cur_q->host_idx = (cur_q->host_idx + count) % cur_q->total_rxbd_num;    
        HAL_RTL_W16(cur_q->reg_rwptr_idx, (cur_q->host_idx & 0xFFF));

//        RT_TRACE_F(COMP_RECV, DBG_TRACE, ("host_idx:0x%lx\n", cur_q->host_idx));
    }
    else {
//        RT_TRACE_F(COMP_RECV, DBG_LOUD, ("count = 0\n"));
    }
}

#endif // (HAL_DEV_BUS_TYPE & (HAL_RT_EMBEDDED_INTERFACE | HAL_RT_PCI_INTERFACE))


