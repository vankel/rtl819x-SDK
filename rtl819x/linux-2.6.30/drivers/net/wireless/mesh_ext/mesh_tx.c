/*
 *      Handling routines for Mesh in 802.11 TX
 *
 *      PS: All extern function in ../8190n_headers.h
 */
#define _MESH_TX_C_

#ifdef CONFIG_RTL8192CD
#include "../rtl8192cd/8192cd_cfg.h"
#include "../rtl8192cd/8192cd.h"
#include "../rtl8192cd/8192cd_headers.h"
#else
#include "../rtl8190/8190n_cfg.h"
#include "../rtl8190/8190n.h"
#include "../rtl8190/8190n_headers.h"
#endif
#include "./mesh.h"
#include "./mesh_route.h"

#ifdef CONFIG_RTK_MESH

// 8190n_tx.c
#define RET_AGGRE_ENQUE			1
#define RET_AGGRE_DESC_FULL		2

__inline__ void ini_txinsn(struct tx_insn* txcfg, DRV_PRIV *priv)
{
	txcfg->is_11s = 1;
	txcfg->mesh_header.mesh_flag= 1;
	txcfg->mesh_header.TTL = _MESH_HEADER_TTL_;
	txcfg->mesh_header.segNum = getMeshSeq(priv);		
}


				 
int fire_data_frame(struct sk_buff *skb, struct net_device *dev, struct tx_insn* txinsn) 
{
#ifdef NETDEV_NO_PRIV
	DRV_PRIV priv = ((DRV_PRIV *)netdev_priv(dev))->wlan_priv;
#else
	DRV_PRIV *priv = (DRV_PRIV *)dev->priv;
#endif
	unsigned long		flags;
	txinsn->q_num = BE_QUEUE;        // using low queue for data queue
	skb->cb[1] = 0;
	// txinsn->q_num   = 0; //using low queue for data queue
	txinsn->fr_type = _SKB_FRAME_TYPE_;
	txinsn->pframe  = skb;

#ifdef MESH_AMSDU
	struct stat_info *pstat = get_stainfo(priv, txinsn->nhop_11s);
	if ( pstat && (pstat->aggre_mthd == AGGRE_MTHD_MSDU) && (pstat->amsdu_level > 0)
#ifdef SUPPORT_TX_MCAST2UNI
		&& skb->cb[2] != (char)0xff
#endif
		) {
		int ret = amsdu_check(priv, skb, pstat, txinsn);

		if (ret == RET_AGGRE_ENQUE)
			goto stop_proc;

		if (ret == RET_AGGRE_DESC_FULL)
			goto free_and_stop;
	}
#endif	

	SAVE_INT_AND_CLI(flags);
	txinsn->phdr = (UINT8 *)get_wlanllchdr_from_poll(priv);

	if (txinsn->phdr == NULL) {
		DEBUG_ERR("Can't alloc wlan header!\n");
		goto xmit_11s_skb_fail;
	}

	if (skb->len > priv->pmib->dot11OperationEntry.dot11RTSThreshold)
		txinsn->retry = priv->pmib->dot11OperationEntry.dot11LongRetryLimit;
	else
		txinsn->retry = priv->pmib->dot11OperationEntry.dot11ShortRetryLimit;

	memset((void *)txinsn->phdr, 0, sizeof(struct wlanllc_hdr));

	/* Set Frame Type (Data Frame) */
	SetFrameSubType(txinsn->phdr, WIFI_11S_MESH);

	if (OPMODE & WIFI_AP_STATE) {
		SetFrDs(txinsn->phdr);
		SetToDs(txinsn->phdr);
	}
	else
		DEBUG_WARN("non supported mode yet!\n");

	if (WLAN_TX(priv, txinsn) == CONGESTED)
	{
xmit_11s_skb_fail:
		netif_stop_queue(dev);
		priv->ext_stats.tx_drops++;
		DEBUG_WARN("TX DROP: Congested!\n");
#ifdef _11s_TEST_MODE_
		if(!memcmp("JasonRelay", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 10)
		&& memcmp(skb->data+14+2, "TEST_TRAFFIC", 12)==0  )
			priv->mesh_stats.rx_errors ++;
#endif		
		if (txinsn->phdr)
			release_wlanllchdr_to_poll(priv, txinsn->phdr);
		if (skb)
			rtl_kfree_skb(priv, skb, _SKB_TX_);
		RESTORE_INT(flags);
		return 0;
	}
	RESTORE_INT(flags);
	return 1;

#ifdef MESH_AMSDU	
free_and_stop:		

#ifdef RTL8190_FASTEXTDEV_FUNCALL
		rtl865x_extDev_kfree_skb(skb, FALSE);
#else
		dev_kfree_skb_any(skb);
#endif
stop_proc:
	return 1;
#endif
	
}

int notify_path_found(unsigned char *destaddr, DRV_PRIV *priv) 
{
    struct sk_buff *pskb;
    struct mesh_rreq_retry_entry * retryEntry;
    struct sk_buff * buf [NUM_TXPKT_QUEUE]; // To record the ALL popped-up skbs at one time, because we don't want enable spinlock for dev_queue_xmit
    struct path_sel_entry *pEntry;	
    int i=0;
    struct stat_info* pstat;
    DECLARE_TXINSN(txinsn);
		
		
    //MESH_LOCK(lock_Rreq, flags);
    if(MESH_SPINLOCK(lock_Rreq)) {
        retryEntry= (struct mesh_rreq_retry_entry*) priv->mesh_rreq_retry_queue->search_entry(priv->mesh_rreq_retry_queue,destaddr);
        if(retryEntry == NULL) { // aodv_expire tx it
            //MESH_UNLOCK(lock_Rreq, flags);
            MESH_SPINUNLOCK(lock_Rreq);
                return 0;
        }
            
        pEntry = pathsel_query_table( priv, destaddr );
        if( pEntry == (struct path_sel_entry *)-1) 
        {
            struct proxy_table_entry*	pProxyEntry;
            pProxyEntry = (struct proxy_table_entry*) HASH_SEARCH(priv->proxy_table, destaddr);
            if(pProxyEntry != NULL)
                pEntry = pathsel_query_table( priv, pProxyEntry->owner );
        }
        
        if(pEntry != (struct path_sel_entry *)-1)
        {
            pEntry->start = retryEntry->createTime;
            pEntry->end = jiffies;
            mesh_route_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x found through AODV, next hop %02x:%02x:%02x:%02x:%02x:%02x\n",
            destaddr[0],destaddr[1],destaddr[2],destaddr[3],destaddr[4],destaddr[5],
            pEntry->nexthopMAC[0],pEntry->nexthopMAC[1],pEntry->nexthopMAC[2],pEntry->nexthopMAC[3],pEntry->nexthopMAC[4],pEntry->nexthopMAC[5]);
        }
        

        i = 0;
        do {            
            pskb = (struct sk_buff*)deque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE);
            buf[i++] = pskb;            
        }while(pskb != NULL);
		priv->mesh_rreq_retry_queue->delete_entry(priv->mesh_rreq_retry_queue,retryEntry->MACAddr);
    
        //MESH_UNLOCK(lock_Rreq, flags);
        MESH_SPINUNLOCK(lock_Rreq);


        for(i=0;i<NUM_TXPKT_QUEUE;i++) {
            pskb = buf[i];
            if(pskb == NULL)
                break;

            // the data which transfer in mesh don't need update in proxy table     
            if(dot11s_datapath_decision(pskb, &txinsn, 0)) {
                pstat = get_stainfo(txinsn.priv, txinsn.nhop_11s);
                pskb->dev = priv->mesh_dev;
                __rtl8192cd_start_xmit_out(pskb, pstat, &txinsn);
            }    
        }
    } 
    else {
        panic_printk("%s suffer racing issue\n",__func__);
    }
   
    return 0;
}

#if 0 //defined(Q_TX_AFTER_ONE_AODV_RETRY)
void toAllPortal(struct sk_buff *pskb, DRV_PRIV *priv)
{	
	struct net_device		*dev = priv->mesh_dev;
	unsigned char 			*eth_src;
	struct proxy_table_entry*	pProxyEntry;
	int k;
			
	// Some potential bug would happen here , due to the reuse of txinsn
	// e.g., txinsn->llc is not initialized in firetx for non-aggregated + non-standard-ethtype frame
	// However, it's not always neccessary, when amsdu is triggered, it would use a different txinsn
	DECLARE_TXINSN(txinsn);

	if((dev == 0) || (!pskb) ){
		if(pskb)
			dev_kfree_skb_any(pskb);
		return;
	}

	ini_txinsn(&txinsn, priv);

	memcpy(txinsn.mesh_header.DestMACAddr, pskb->data, MACADDRLEN);
	memcpy(txinsn.mesh_header.SrcMACAddr, pskb->data+MACADDRLEN, MACADDRLEN);


	// rule of A4/A6:
	// if there is an entry containing eth_src in Proxy Table,  A4=proxy.owner, A6=eth_src
	//    else if there is an entry containing eth_src in Path Sel Table, A4 = A6 = eth_src
	//    else A4 = A6 = eth_src
	// ==> Hence, A6 always be eth_src, and A4 is eth_src except existing a proxy table entry
	
	eth_src = pskb->data+MACADDRLEN;
	pProxyEntry = (struct proxy_table_entry*) HASH_SEARCH(priv->proxy_table, eth_src);
	if(pProxyEntry != NULL) {
		pProxyEntry->aging_time = 0;
		memcpy(eth_src, pProxyEntry->owner, MACADDRLEN);
	}
	
	// NOTE: DO NOT filter packets that SA is myself, because it can't accept root forwarding (e.g., ROOT-SRC(MPP1)-MPP2)
	for (k = 0; k < MAX_MPP_NUM; k++) 
	{
		if (priv->pann_mpp_tb->pann_mpp_pool[k].flag == 1) 
		{
			struct path_sel_entry *pEntry;
			
			pEntry = pathsel_query_table( priv ,priv->pann_mpp_tb->pann_mpp_pool[k].mac ); // chuangch 2007.09.14
			if(pEntry != (struct path_sel_entry *)-1) // has valid route path 
			{ 
				struct sk_buff *pnewskb;
				pnewskb = skb_copy(pskb, GFP_ATOMIC);

				if(!pnewskb) {
					DEBUG_ERR("Can't alloc skb to portal!\n");
					dev_kfree_skb_any(pskb);
					return;
				}

				memcpy(pnewskb->data, priv->pann_mpp_tb->pann_mpp_pool[k].mac, MACADDRLEN);
				txinsn.mesh_header.segNum = getMeshSeq(priv);
				txinsn.is_11s = 1;
//				chkMeshSeq(priv, pnewskb->data+MACADDRLEN,txinsn.mesh_header.segNum);
				memcpy(txinsn.nhop_11s,pEntry->nexthopMAC,MACADDRLEN);
				fire_data_frame(pnewskb, dev, &txinsn);
			} 
			else
				// not have valid route path 
				GEN_PREQ_PACKET(priv->pann_mpp_tb->pann_mpp_pool[k].mac, priv, 0);
		}
	} 

	dev_kfree_skb_any(pskb);

	return;
}
#endif

#ifdef	_11s_TEST_MODE_
unsigned char *get_galileo_from_poll(DRV_PRIV *priv)
{
	return get_buf_from_poll(priv, &(priv->pshare->galileo_list), (unsigned int *)(&priv->pshare->galileo_poll->count));
}

void release_galileo_to_poll(DRV_PRIV *priv, unsigned char *pbuf)
{
	release_buf_to_poll(priv, pbuf, &(priv->pshare->galileo_list), (unsigned int *)(&priv->pshare->galileo_poll->count));
}



void issue_test_traffic(struct sk_buff *skb)
{
	DRV_PRIV *priv = (DRV_PRIV *)skb->dev->priv;
	DECLARE_TXINSN(txinsn);
	struct tx_insn* ptxinsn=&txinsn;
	ini_txinsn(ptxinsn, priv);	
	skb_pull(skb, 14);	
	memcpy(ptxinsn->nhop_11s, skb->data, WLAN_ADDR_LEN);
	memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR,MACADDRLEN);
	memcpy(skb->data+14+26, (void*)(&jiffies), sizeof(long));
	if( skb->len > 80)
		memcpy(skb->data+48, (void*)&priv->pmib->dot1180211sInfo.mesh_reserved1, 30 );
	
	fire_data_frame(skb, skb->dev, ptxinsn);
}


struct stat_info* findNextSta(struct list_head *phead, struct list_head** plist, void* preHop)
{
	struct stat_info	*pstat =NULL;
	while(*plist != phead) 
	{
		pstat = list_entry(*plist, struct stat_info, mesh_mp_ptr);
		*plist = (*plist)->next;
		if( memcmp(pstat->hwaddr, preHop, MACADDRLEN))
			return pstat;		
	}
	return NULL;
}


void signin_txdesc_m2u(DRV_PRIV *priv, struct tx_insn* txcfg, char isTestTraffic)
{
	struct stat_info	*pstat;
	struct list_head	*phead, *plist;
		
	phead= &priv->mesh_mp_hdr;
	plist = phead->next;
	txcfg->is_11s = 17;
	txcfg->need_ack = 1;			
	txcfg->q_num = (txcfg->fr_type == _SKB_FRAME_TYPE_)? VI_QUEUE : MANAGE_QUE_NUM;;
	pstat = findNextSta(phead, &plist, txcfg->prehop_11s);
	
	while( pstat )
	{
		memcpy(GetAddr1Ptr(txcfg->phdr), pstat->hwaddr, MACADDRLEN); 
		txcfg->pstat = pstat;

		if(priv->pmib->dot1180211sInfo.mesh_reserved2&1024)					
			txcfg->lowest_tx_rate = txcfg->tx_rate = get_tx_rate(priv, pstat);
	
		pstat = findNextSta(phead, &plist, txcfg->prehop_11s);
		
		if( pstat == NULL)
			txcfg->is_11s = 1;

		SIGNINTX(priv, txcfg);

		if( pstat != NULL)
		{					
			UINT8 *pwlhdr = (txcfg->fr_type == _SKB_FRAME_TYPE_) ? get_wlanllchdr_from_poll(priv) : get_wlanhdr_from_poll(priv);
			if(!pwlhdr)	break;
			memcpy((void *)pwlhdr, (void *)(txcfg->phdr), txcfg->hdr_len+((txcfg->fr_type == _SKB_FRAME_TYPE_) ? 8 : 0)); 
			txcfg->phdr = pwlhdr;
		}					
		if( isTestTraffic )
			priv->mesh_stats.rx_bytes++;			
	}

	if( txcfg->is_11s == 17)
	{
		txcfg->is_11s =1;

		SIGNINTX(priv, txcfg);

		if( isTestTraffic )
			priv->mesh_stats.rx_bytes++;	
	}	
	
}


// Galileo 2008.07.30
const int MaxInterVal  = 10;

void galileo_timer(unsigned long task_priv)
{
	struct Galileo* gakki = (struct Galileo*)task_priv;
	DRV_PRIV *priv = (DRV_PRIV*)gakki->priv;

	if (gakki)
	{
		if((gakki->txcfg.fr_type == _SKB_FRAME_TYPE_)
			&& (( !memcmp("JasonRelay",  priv->pmib->dot1180211sInfo.mesh_reservedstr1, 10)
			|| !memcmp("JasonSender", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 11)  )))
		{
			unsigned char *frame =  ((struct sk_buff *)(gakki->txcfg.pframe))->data;
			if( memcmp(frame+6, "TEST_TRAFFIC", 12)==0 )
				priv->mesh_stats.rx_bytes++;
		}

		if( --gakki->tx_count )
		{
			unsigned char r =0;
			if( priv->pmib->dot1180211sInfo.mesh_reserved2&64)
			{
				get_random_bytes(&r, 1);
				r %= MaxInterVal;
			}
			SIGNINTX(priv, &gakki->txcfg);
			mod_timer(&gakki->expire_timer, jiffies + r+1);
		}
		else
		{
			del_timer(&gakki->expire_timer);
			gakki->txcfg.is_11s =1;
			SIGNINTX(priv, &gakki->txcfg);
			release_galileo_to_poll(priv, (unsigned char*)gakki);
		}
	}
}



void signin_txdesc_multiTime(DRV_PRIV *priv, struct tx_insn* txcfg)
{
//	short count = (priv->pmib->dot1180211sInfo.mesh_reserved1>>8)& 0xff;
	short count = priv->pmib->dot1180211sInfo.mesh_reserved1;
	struct Galileo* gakki ;
	char totti=0;

	// stats
	if(  (txcfg->fr_type == _SKB_FRAME_TYPE_)
		&& (( !memcmp("JasonRelay",  priv->pmib->dot1180211sInfo.mesh_reservedstr1, 10)
	   	   || !memcmp("JasonSender", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 11)  )))
	{
		unsigned char *frame =	((struct sk_buff *)(txcfg->pframe))->data;
		if( memcmp(frame+6, "TEST_TRAFFIC", 12)==0 )
		{
			priv->mesh_stats.rx_bytes++;
			totti = 1;
		}
	}
		
	if( count >0 /* && (txcfg->fr_type == _SKB_FRAME_TYPE_)*/ )
	{
		if( priv->pmib->dot1180211sInfo.mesh_reserved2&128)
		{
			if( totti )
				priv->mesh_stats.rx_bytes += count;		
			txcfg->is_11s =48;
			while( count--)
				SIGNINTX(priv, txcfg);	
			txcfg->is_11s =1;
			SIGNINTX(priv, txcfg);				
		}
		else		
		{	
			gakki =  (struct Galileo*) get_galileo_from_poll(priv);
			if( gakki )
			{	
				unsigned char r=0;	
				if( priv->pmib->dot1180211sInfo.mesh_reserved2&64)
				{
					get_random_bytes(&r, 1);
					r %= MaxInterVal;
				}				
				txcfg->is_11s =48;
				memcpy(&gakki->txcfg, txcfg, sizeof(struct tx_insn));
				gakki->tx_count = count;
				gakki->expire_timer.data = (unsigned long) gakki;
				gakki->expire_timer.expires = jiffies + r+1;
				mod_timer(&gakki->expire_timer, gakki->expire_timer.expires);
			}
			SIGNINTX(priv, txcfg);	
		}
		
	}else
		SIGNINTX(priv, txcfg);	
}



void signin_txdesc_galileo(DRV_PRIV *priv, struct tx_insn* txcfg)
{
	// 11s multicast 
	if( IS_MCAST(GetAddr1Ptr(txcfg->phdr)) && txcfg->is_11s )
	{
		char isTestTraffic = 0;

// DHCP trace start		
		if( txcfg->fr_type == _SKB_FRAME_TYPE_ && !memcmp("XMT", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 3) )
		{
			unsigned char srcMAC[6]= { 0 }; //,  *frame = ((struct sk_buff *)(txcfg->pframe))->data;				
			mac12_to_6(priv->pmib->dot1180211sInfo.mesh_reservedstr1+3, srcMAC );
		
			if(!memcmp(GetAddr4Ptr(txcfg->phdr), srcMAC, 6)&& 
				( (txcfg->fr_len>331 && txcfg->fr_len<364)
				||(txcfg->fr_len>576 && txcfg->fr_len<591)	)	)
			{

				printk("$(%d,%d)\n", txcfg->fr_len, txcfg->hdr_len);
	//			for(j=0; j<30; j++)
	//				printk("%02X ", frame[j]&0xff);
	//			printk("\n");
			}
		}
// end	
		if(  (txcfg->fr_type == _SKB_FRAME_TYPE_)
			&& (( !memcmp("JasonRelay",  priv->pmib->dot1180211sInfo.mesh_reservedstr1, 10)
			|| !memcmp("JasonSender", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 11)  )))
		{
			unsigned char *frame =	((struct sk_buff *)(txcfg->pframe))->data;
			if( !memcmp(frame+6, "TEST_TRAFFIC", 12) )
			{
				priv->mesh_stats.tx_packets++;
				isTestTraffic = 1;
			}
		}

		// multi-time broadcast
		if( priv->pmib->dot1180211sInfo.mesh_reserved2&32)
			signin_txdesc_multiTime(priv, txcfg);	
		else
		{	
			unsigned char action = *((unsigned char*)txcfg->pframe+6+4+1);
					
			if( (priv->pmib->dot1180211sInfo.mesh_reserved2&2 && (txcfg->fr_type == _SKB_FRAME_TYPE_) )
				||(priv->pmib->dot1180211sInfo.mesh_reserved2&1 && ( txcfg->fr_type == _PRE_ALLOCMEM_ && 
				( action == ACTION_FIELD_PANN || action == ACTION_FIELD_RREQ  || action ==ACTION_FIELD_RANN))))

		// multi-unicast			
				signin_txdesc_m2u(priv, txcfg, isTestTraffic);				
			else	
			{
				SIGNINTX(priv, txcfg);
				if( isTestTraffic )
					priv->mesh_stats.rx_bytes++;
			}			
		}
	}
	else
		SIGNINTX(priv, txcfg);
}

#endif



static int issue_11s_mesh_action(DRV_PRIV * priv, struct sk_buff *skb, struct net_device *dev)
{	
	unsigned char *pframe = skb->data+14, *pbuf = NULL;
//	struct stat_info	*pstat;
	int len;
    DECLARE_TXINSN(txinsn);   
	txinsn.q_num = MANAGE_QUE_NUM;
	txinsn.fr_type = _PRE_ALLOCMEM_;	
	txinsn.is_11s = 1;
	txinsn.fixed_rate = 1;	
    
// chkMeshSeq ??    

	// construct mesh_header of txinsn
	len = (*(GetMeshHeaderFlagWithoutQOS(pframe))& 0x01) ? 16 :4;	//if 6 address is enable, the bit 0 of AE will set to 1,(b7 b6 b5...b0) 
	memcpy(&(txinsn.mesh_header), GetMeshHeaderFlagWithoutQOS(pframe),len);
	
	/* header clean to "0" */
	if( skb->len > (14+WLAN_HDR_A3_LEN)) {

		txinsn.phdr = get_wlanhdr_from_poll(priv);
		pbuf = txinsn.pframe  = get_mgtbuf_from_poll(priv);
		
		if(pbuf == 0 || txinsn.phdr==0)
			goto issue_11s_mesh_actio_FAIL;
	
		// only copy the first 3 address + zero-valued seq 
		memset((void *)(txinsn.phdr), 0, sizeof (struct wlan_hdr));
		memcpy((void *)txinsn.phdr, pframe, WLAN_HDR_A3_LEN);
		
		txinsn.fr_len = skb->len -(14+WLAN_HDR_A3_LEN);
		memcpy(pbuf, pframe + WLAN_HDR_A3_LEN , txinsn.fr_len );
/*
		pstat = get_stainfo(priv, GetAddr1Ptr(pframe)); 
		if (pstat)
		{			
			txinsn.tx_rate = get_tx_rate(priv, pstat);
			txinsn.lowest_tx_rate = get_lowest_tx_rate(priv, pstat, txinsn.tx_rate);
		}else*/
			txinsn.lowest_tx_rate = txinsn.tx_rate = find_rate(priv, NULL, 0, 1);		
	} else
		return 0;

	if (skb->len > priv->pmib->dot11OperationEntry.dot11RTSThreshold)
		txinsn.retry = priv->pmib->dot11OperationEntry.dot11LongRetryLimit;
	else
		txinsn.retry = priv->pmib->dot11OperationEntry.dot11ShortRetryLimit;

	if (WLAN_TX(priv, &txinsn) == CONGESTED) {
		netif_stop_queue(dev);
		priv->ext_stats.tx_drops++;
		DEBUG_WARN("TX DROP: Congested!\n");
issue_11s_mesh_actio_FAIL:
		
		if (txinsn.phdr)
			release_wlanhdr_to_poll(priv, txinsn.phdr);
		
		if (txinsn.pframe)
			release_mgtbuf_to_poll(priv, txinsn.pframe);

		return 0;
	}

#ifdef __KERNEL__
	dev->trans_start = jiffies;
#endif

	return 1;
}

void do_aodv_routing(DRV_PRIV *priv, struct sk_buff *skb, unsigned char *Mesh_dest)
{
    struct mesh_rreq_retry_entry *retryEntry;
    struct sk_buff *poldskb;
    int result=0;

    if(MESH_SPINLOCK(lock_Rreq)) {
        retryEntry= (struct mesh_rreq_retry_entry*) priv->mesh_rreq_retry_queue->search_entry(priv->mesh_rreq_retry_queue,Mesh_dest);
    			
        // with buffer mechanism and AODV timeout flow
        if (retryEntry == NULL) // new AODV path
        {
            GEN_PREQ_PACKET(Mesh_dest, priv, 1);
            mesh_route_debug("Send PREQ to find %02x:%02x:%02x:%02x:%02x:%02x at %lu\n",
            Mesh_dest[0],Mesh_dest[1],Mesh_dest[2],Mesh_dest[3],Mesh_dest[4],Mesh_dest[5],jiffies);
            retryEntry= (struct mesh_rreq_retry_entry*) priv->mesh_rreq_retry_queue->search_entry(priv->mesh_rreq_retry_queue,Mesh_dest);
            if(retryEntry==NULL )
            {
                MESH_SPINUNLOCK(lock_Rreq);
                dev_kfree_skb_any(skb);
                return;
            }
 
            result = enque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail), (unsigned int)retryEntry->ptr->pSkb, NUM_TXPKT_QUEUE,(void*)skb);

            if(result == FALSE)
            {               
                poldskb = (struct sk_buff*)deque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE);
                if(poldskb)
                    dev_kfree_skb_any(poldskb);

                result = enque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE,(void*)skb);
                if(result == FALSE)
                    dev_kfree_skb_any(skb);
            }
				
        }
        else { 
            mesh_route_debug("PREQ to find %02x:%02x:%02x:%02x:%02x:%02x already in retry list,at %lu\n",
            Mesh_dest[0],Mesh_dest[1],Mesh_dest[2],Mesh_dest[3],Mesh_dest[4],Mesh_dest[5],jiffies);      
            result = enque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail), (unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE,(void*)skb);
            if(result == FALSE)
            {
                poldskb = (struct sk_buff*)deque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE);

                if(poldskb)
                    dev_kfree_skb_any(poldskb);

                result = enque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE,(void*)skb);
                if(result == FALSE)
                    dev_kfree_skb_any(skb);
            }
        }
			
        MESH_SPINUNLOCK(lock_Rreq);
    } 
    else {
        panic_printk("%s suffer racing issue\n",__func__);
    }
    return;
}



/*
	pfrinfo->is_11s =1  => 802.11 header
	pfrinfo->is_11s =8  => 802.3  header + mesh header
*/
int relay_11s_dataframe(DRV_PRIV *priv, struct sk_buff *skb, int privacy, struct rx_frinfo *pfrinfo)
{	
    struct list_head *phead, *plist;
    struct stat_info *pstat;
    struct sk_buff *pnewskb = NULL, *pnewskb_send = NULL;
    unsigned char prehop[MACADDRLEN];
    unsigned char force_m2u = 0, sta_count = 0;
    struct path_sel_entry* pPathEntry;    
    struct tx_insn tx_insn;
    DRV_PRIV *xmit_priv;
    UINT16 seqNum;  
    int i,j,ret = 0;

    if(1 > pfrinfo->mesh_header.TTL) {
        return 1;
    }

    if (IS_MCAST(pfrinfo->da)) {          
        memcpy(prehop, GetAddr2Ptr(skb->data), MACADDRLEN);
        pnewskb = skb_copy(skb, GFP_ATOMIC);
        if(skb_p80211_to_ether(skb->dev, privacy, pfrinfo) == FAIL){ // for e.g., CISCO CDP which has unsupported LLC's vendor ID
            DEBUG_ERR("RX DROP: skb_p80211_to_ether fail!\n");
            ret = -1;  
            goto relay_end;
        } 
       
        if (pnewskb) {  
            get_pskb(pfrinfo) = pnewskb;
            pnewskb->cb[2] = (char)0xff; // do not aggregation
            pfrinfo->is_11s = 2;/*replace wlan_header with eth_header, but keep the mesh header*/
            if(skb_p80211_to_ether(pnewskb->dev, privacy, pfrinfo) == FAIL){ // for e.g., CISCO CDP which has unsupported LLC's vendor ID                
                goto relay_end;
            }    
        }
        else {           
            goto relay_end;
        }         

               
        if (IP_MCAST_MAC(skb->data)
#ifdef  TX_SUPPORT_IPV6_MCAST2UNI
            || IPV6_MCAST_MAC(skb->data)
#endif
        )
        {
            // all multicast managment packet try do m2u
            if( isSpecialFloodMac(priv,skb) || IS_MDNSV4_MAC(skb->data)||IS_MDNSV6_MAC(skb->data)||IS_IGMP_PROTO(skb->data) || isICMPv6Mng(skb) || IS_ICMPV6_PROTO(skb->data)|| isMDNS(skb->data))
            {
                force_m2u = 1;
            }    
        }
        else {
            force_m2u = 1;
        }     


        sta_count = 0;
        if(force_m2u == 0) {/* if it is not a multicast management or broadcast frame, check if it is a video  frame need to be multicasted*/                        
            for(j = 0 ; j < 2; j++) {
                if(j == 1) {
                    #if !defined(CONFIG_RTL_MESH_CROSSBAND)
                    if(priv->mesh_priv_sc && GET_MIB(priv->mesh_priv_sc)->dot1180211sInfo.mesh_enable) {
                        xmit_priv = priv->mesh_priv_sc;
                    }
                    else 
                    #endif
                        break;
                }
                else {
                    xmit_priv = priv;
                }
                
                phead = &xmit_priv->asoc_list;
                plist = phead->next;
                while (phead && (plist != phead)) {
                    pstat = list_entry(plist, struct stat_info, asoc_list);
                    plist = plist->next;   
                    if(!isMeshPoint(pstat)) {
                        continue;
                    }

                    if(memcmp(pstat->hwaddr, prehop, MACADDRLEN) == 0) {
                        continue;
                    }                   
                    
                    for (i=0; i<MAX_IP_MC_ENTRY; i++) {
                        if (pstat->ipmc[i].used && !memcmp(&pstat->ipmc[i].mcmac[0], skb->data, 6)) {
                            pnewskb_send = skb_copy(pnewskb, GFP_ATOMIC);
                            if(pnewskb_send == NULL) {
                                goto relay_end;
                            }
                            sta_count++;
                            tx_insn.is_11s = RELAY_11S;
                            tx_insn.priv = xmit_priv;  
                            pnewskb_send->dev = xmit_priv->mesh_dev;
                            __rtl8192cd_start_xmit_out(pnewskb_send, pstat, &tx_insn);  
                            break;
                        }
                  }                                        
                }
            }
        }


        if(force_m2u == 1 || (sta_count == 0 && !priv->pshare->rf_ft_var.mc2u_drop_unknown)) 
        {
            /*forward to every mesh nodes*/
            seqNum = getMeshMulticastSeq(priv);
            j = 1 << priv->pathsel_table->table_size_power;
            for (i = 0; i < j; i++) {
                if (priv->pathsel_table->entry_array[i].dirty) 
                { 
                    pPathEntry = ((struct path_sel_entry*)priv->pathsel_table->entry_array[i].data);

                    if(memcmp(pPathEntry->destMAC, pfrinfo->sa, MACADDRLEN) == 0) {
                        continue;
                    }     
                    
                    if(memcmp(pPathEntry->nexthopMAC, prehop, MACADDRLEN) == 0) {
                        continue;
                    }     

                    if(memcmp(pPathEntry->nexthopMAC, pfrinfo->sa, MACADDRLEN) == 0) {
                        continue;
                    }     



                    pstat = get_stainfo(pPathEntry->priv, pPathEntry->nexthopMAC);
                    if(pstat && pstat->mesh_neighbor_TBL.seqNum != seqNum) {
                        pstat->mesh_neighbor_TBL.seqNum = seqNum;

                        pnewskb_send = skb_copy(pnewskb, GFP_ATOMIC);
                        if(pnewskb_send == NULL) {
                            goto relay_end;
                        }                            
                        tx_insn.is_11s = RELAY_11S;
                        tx_insn.priv = pPathEntry->priv;  
                        pnewskb_send->dev = priv->mesh_dev;
                        __rtl8192cd_start_xmit_out(pnewskb_send, pstat, &tx_insn);
                    }                                       
                }
            }
        }
         
relay_end:
         if(pnewskb) {
            dev_kfree_skb_any(pnewskb);
         }
         /*roll back to original value*/
         get_pskb(pfrinfo) = skb;
         pfrinfo->is_11s = 1;
         return ret;
        
  	} 
    else 
  	{

        struct path_sel_entry *pEntry;	
        /*
#ifndef MESH_AMSDU	
        unsigned char *prehopaddr[MACADDRLEN];
        memcpy(prehopaddr, GetAddr2Ptr(skb->data), MACADDRLEN);
        memcpy(GetAddr2Ptr(skb->data),GET_MY_HWADDR,MACADDRLEN);
#endif
        */
        pEntry = pathsel_query_table( priv, pfrinfo->da );		
        if(pEntry == (struct path_sel_entry *)-1) // not have valid route path
        {
            DOT11s_GEN_RERR_PACKET rerr_event;			
            memset((void*)&rerr_event, 0x0, sizeof(DOT11s_GEN_RERR_PACKET));
            rerr_event.EventId = DOT11_EVENT_PATHSEL_GEN_RERR;
            rerr_event.IsMoreEvent = 0;
            memcpy(rerr_event.MyMACAddr,  GET_MY_HWADDR ,MACADDRLEN); 
            memcpy(rerr_event.SorNMACAddr,  pfrinfo->da ,MACADDRLEN);
            memcpy(rerr_event.DataDestMAC,  pfrinfo->sa ,MACADDRLEN);

            // this field will be used by Path Selection daemon for the following case 
            // when MP want to generate RERR to data source but no path to the data source now, it will send the RERR which set Addr2 = Prehop.

            //#ifdef MESH_AMSDU
            memcpy(rerr_event.PrehopMAC, pfrinfo->prehop_11s, MACADDRLEN);
            /*#else
            memcpy(rerr_event.PrehopMAC, prehopaddr, MACADDRLEN);
#endif*/
            rerr_event.TTL = _MESH_HEADER_TTL_;
            rerr_event.Seq_num = getMeshSeq(priv);
            rerr_event.Flag = 2;// flag = 2 means this MP doesn't have the nexthop information for the destination in pathseleciton table
            DOT11_EnQueue2((unsigned long)priv, priv->pathsel_queue, (unsigned char*)&rerr_event, sizeof(DOT11s_GEN_RERR_PACKET));
            notifyPathSelection(priv);
            return 1;

        } // if(pEntry == (struct path_sel_entry *)-1)

#ifdef MESH_AMSDU
        if(pfrinfo->is_11s&1) 
#endif
        {
            pfrinfo->is_11s = 2;
            if(skb_p80211_to_ether(skb->dev, privacy, pfrinfo) == FAIL) // for e.g., CISCO CDP which has unsupported LLC's vendor ID
                return 1;
        }

        pstat = get_stainfo(pEntry->priv, pEntry->nexthopMAC);
        tx_insn.is_11s = RELAY_11S;
        tx_insn.priv = pEntry->priv;  
        skb->dev = priv->mesh_dev;
        __rtl8192cd_start_xmit_out(skb, pstat, &tx_insn);

        return 0;
    }


}

#if defined(MESH_TX_SHORTCUT)
int mesh_txsc_decision(struct tx_insn* cfgNew, struct tx_insn* cfgOld)
{
	//cfgOld&1 to confirm no amsdu last time
	if( cfgOld->is_11s &&			
		!memcmp(cfgNew->mesh_header.DestMACAddr, cfgOld->mesh_header.DestMACAddr, MACADDRLEN) &&
		!memcmp(cfgNew->mesh_header.SrcMACAddr, cfgOld->mesh_header.SrcMACAddr, MACADDRLEN))
	{
		return 1;
	}
	else
		return 0;
}
#endif

#if defined(RTL_MESH_TXCACHE)
int expire_mesh_txcache(struct rtl8192cd_priv *priv, unsigned char *da)
{
    int ret = 0;

    if(priv->mesh_txcache.dirty == 1) { /*if the tx cache is valid*/
        if(!memcmp(da, priv->mesh_txcache.da_proxy->owner,MACADDRLEN)) {
            mesh_tx_debug("TX cache of %02X:%02X:%02X:%02X:%02X:%02X expired\n", 
                    priv->mesh_txcache.da_proxy->owner[0], priv->mesh_txcache.da_proxy->owner[1], priv->mesh_txcache.da_proxy->owner[2],
                    priv->mesh_txcache.da_proxy->owner[3], priv->mesh_txcache.da_proxy->owner[4], priv->mesh_txcache.da_proxy->owner[5]);
            priv->mesh_txcache.dirty = 0;
            ret = 1;
        }      
    }

    return ret;
}

int match_tx_cache(struct rtl8192cd_priv *priv,struct sk_buff *skb,struct tx_insn* ptxinsn)
{
    int ret=0;
    if((priv->mesh_txcache.dirty == 1) && !memcmp(priv->mesh_txcache.ether_da,skb->data,MACADDRLEN) && !memcmp(priv->mesh_txcache.ether_sa,skb->data+MACADDRLEN,MACADDRLEN)) {

        memcpy(skb->data,priv->mesh_txcache.da_proxy->owner,MACADDRLEN);
        memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
        memcpy(ptxinsn,&(priv->mesh_txcache.txcfg),sizeof(*ptxinsn));
        priv->mesh_txcache.da_proxy->aging_time = 0;

        ret = 1;

        mesh_txsc_debug("%s %d %02x:%02x:%02x:%02x:%02x:%02x-%02x:%02x:%02x:%02x:%02x:%02x match cache\n",__func__,__LINE__,
                priv->mesh_txcache.ether_da[0],priv->mesh_txcache.ether_da[1],priv->mesh_txcache.ether_da[2],
                priv->mesh_txcache.ether_da[3],priv->mesh_txcache.ether_da[4],priv->mesh_txcache.ether_da[5],
                priv->mesh_txcache.ether_sa[0],priv->mesh_txcache.ether_sa[1],priv->mesh_txcache.ether_sa[2],
                priv->mesh_txcache.ether_sa[3],priv->mesh_txcache.ether_sa[4],priv->mesh_txcache.ether_sa[5]);
    }
    return ret;
}
#endif


int dot11s_datapath_decision(struct sk_buff *skb, /*struct net_device *dev,*/ struct tx_insn* ptxinsn, int isUpdateProxyTable)
{
    DRV_PRIV *priv = (DRV_PRIV *)skb->dev->priv;
    struct proxy_table_entry*   pProxyEntry;
    struct path_sel_entry* pPathEntry;
    struct list_head *phead, *plist;
    struct stat_info *pstat;
    struct sk_buff *newskb;
    unsigned char force_m2u = 0;
    unsigned char sta_count = 0;
    int i, j;
    UINT16 seqNum;
    DRV_PRIV * xmit_priv;
    
    if(ptxinsn)
        ini_txinsn(ptxinsn, priv);
    else {
        dev_kfree_skb_any(skb);
        return 0;
    }    


#if defined(RTL_MESH_TXCACHE)
    if(!IS_MCAST(skb->data)){            
        if(match_tx_cache(priv,skb,ptxinsn)) {
                goto dot11s_datapath_decision_end;
        }     
        mesh_tx_debug("%s %d %02x:%02x:%02x:%02x:%02x:%02x-%02x:%02x:%02x:%02x:%02x:%02x not match to mesh_txcache\n"
            ,__func__,__LINE__,*(unsigned char *)skb->data,*(unsigned char *)(skb->data+1),*(unsigned char *)(skb->data+2)
            ,*(unsigned char *)(skb->data+3),*(unsigned char *)(skb->data+4),*(unsigned char *)(skb->data+5)
            ,*(unsigned char *)(skb->data+6),*(unsigned char *)(skb->data+7),*(unsigned char *)(skb->data+8)
            ,*(unsigned char *)(skb->data+9),*(unsigned char *)(skb->data+10),*(unsigned char *)(skb->data+11));
    }
#endif

    if(isUpdateProxyTable == 1) {
        //update proxy table from packets 1.Unicast from bridge 2.Broadcast ARP
        unsigned char *Eth_src = skb->data+MACADDRLEN;           
        if(memcmp(Eth_src, GET_MY_HWADDR, MACADDRLEN)) { // the entry briged by me
            struct proxy_table_entry	Entry, *pEntry;
            pEntry = (struct proxy_table_entry *)HASH_SEARCH(priv->proxy_table,Eth_src);
            if(pEntry == NULL) {
                memcpy(Entry.sta, Eth_src, MACADDRLEN);
                memcpy(Entry.owner, GET_MY_HWADDR, MACADDRLEN);
                Entry.aging_time = -1; /*never timeout*/
                HASH_INSERT(priv->proxy_table, Entry.sta, &Entry);
                mesh_proxy_debug("[Locally forwardding]Insert Proxy table: %02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x\n",
                                    Entry.owner[0],Entry.owner[1],Entry.owner[2],Entry.owner[3],Entry.owner[4],Entry.owner[5],
                                    Entry.sta[0],Entry.sta[1],Entry.sta[2],Entry.sta[3],Entry.sta[4],Entry.sta[5]);
#if defined(CONFIG_RTL_MESH_CROSSBAND) 
                if(sync_proxy_info(priv->mesh_priv_sc,Eth_src, 1)) //insert proxy info
                    mesh_proxy_debug("Sync proxy information to %s failed\n",priv->mesh_priv_sc->dev->name);
#endif


#if defined(PU_STANDARD)  
                issue_proxyupdate_ADD(priv, Entry.sta);
#endif
            }
            else if(memcmp(pEntry->owner,GET_MY_HWADDR,MACADDRLEN)) {
                mesh_proxy_debug("[Locally forwardding]Update owner from %02x:%02x:%02x:%02x:%02x:%02x to %02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x",
                                    pEntry->owner[0],pEntry->owner[1],pEntry->owner[2],pEntry->owner[3],pEntry->owner[4],pEntry->owner[5],
                                    GET_MY_HWADDR[0],GET_MY_HWADDR[1],GET_MY_HWADDR[2],GET_MY_HWADDR[3],GET_MY_HWADDR[4],GET_MY_HWADDR[5],
                                    pEntry->sta[0],pEntry->sta[1],pEntry->sta[2],pEntry->sta[3],pEntry->sta[4],pEntry->sta[5]);                      
                memcpy(pEntry->owner,GET_MY_HWADDR,MACADDRLEN);
                pEntry->aging_time = -1; /*never timeout*/

#if defined(CONFIG_RTL_MESH_CROSSBAND) 
                if(sync_proxy_info(priv->mesh_priv_sc,Eth_src, 1)) //insert proxy info
                    mesh_proxy_debug("Sync proxy information to %s failed\n",priv->mesh_priv_sc->dev->name);
#endif

#if defined(PU_STANDARD)  
                issue_proxyupdate_ADD(priv, Eth_src);
#endif
            }
		}
	}

    if (IS_MCAST(skb->data))
    {        

        // Note that Addr4 of an 11s broadcast frame is the original packet issuer (i.e., skb->data+MACADDRLEN)
        // When rx receives an 11s broadcast frame, it also check mssh seq by using Addr4 as the search key
        // Transfer multicast to unicast and then send to each mesh neighbor

        if (IP_MCAST_MAC(skb->data)
    #ifdef  TX_SUPPORT_IPV6_MCAST2UNI
            || IPV6_MCAST_MAC(skb->data)
    #endif
            )
        {
            // all multicast managment packet try do m2u
            if( isSpecialFloodMac(priv,skb) || IS_MDNSV4_MAC(skb->data)||IS_MDNSV6_MAC(skb->data)||IS_IGMP_PROTO(skb->data) || isICMPv6Mng(skb) || IS_ICMPV6_PROTO(skb->data)|| isMDNS(skb->data))
            {
                force_m2u = 1;
            }    
        }
        else {
            force_m2u = 1;
        }

        memcpy(ptxinsn->mesh_header.DestMACAddr, skb->data, MACADDRLEN);
        memcpy(ptxinsn->mesh_header.SrcMACAddr,  skb->data+MACADDRLEN, MACADDRLEN);        
        sta_count = 0;

        if(force_m2u == 0) {/* if it is not a multicast management or broadcast frame, check if it is a video  frame need to be multicasted*/                        
            for(j = 0 ; j < 2; j++) {
                if(j == 1) {
                    #if !defined(CONFIG_RTL_MESH_CROSSBAND)
                    if(priv->mesh_priv_sc && GET_MIB(priv->mesh_priv_sc)->dot1180211sInfo.mesh_enable) {
                        xmit_priv = priv->mesh_priv_sc;
                    }
                    else 
                    #endif
                        break;
                }
                else {
                    xmit_priv = priv;
                }
                
                phead = &xmit_priv->asoc_list;
                plist = phead->next;
                while (phead && (plist != phead)) {
                    pstat = list_entry(plist, struct stat_info, asoc_list);
                    plist = plist->next;                
                    if(!isMeshPoint(pstat)) {
                        continue;
                    }
                    
                    
                    for (i=0; i<MAX_IP_MC_ENTRY; i++) {
                        if (pstat->ipmc[i].used && !memcmp(&pstat->ipmc[i].mcmac[0], skb->data, 6)) {
                            memcpy(ptxinsn->nhop_11s, pstat->hwaddr, MACADDRLEN);   
                            newskb = skb_copy(skb, GFP_ATOMIC);
                            if (newskb) {     
                                sta_count++;
                                memcpy(newskb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
                                newskb->cb[2] = (char)0xff;         // not do aggregation
                                ptxinsn->priv = xmit_priv;
                                newskb->dev = priv->mesh_dev;
                                __rtl8192cd_start_xmit_out(newskb, pstat, ptxinsn);
                            }
                            else {                
                                dev_kfree_skb_any(skb);
                                return 0;
                            }
                            break;
                        }
                  }                                        
                }
            }
        }


        if(force_m2u == 1 || (sta_count == 0 && !priv->pshare->rf_ft_var.mc2u_drop_unknown)) 
        {
            /*forward to every mesh nodes*/
            seqNum = getMeshMulticastSeq(priv);            
            j = 1 << priv->pathsel_table->table_size_power;
        	for (i = 0; i < j; i++) {
                
                if (priv->pathsel_table->entry_array[i].dirty) 
        		{ 
        			pPathEntry = ((struct path_sel_entry*)priv->pathsel_table->entry_array[i].data);        			
                    pstat = get_stainfo(pPathEntry->priv, pPathEntry->nexthopMAC);
                    if(pstat && pstat->mesh_neighbor_TBL.seqNum != seqNum) {
                        pstat->mesh_neighbor_TBL.seqNum = seqNum;
                        memcpy(ptxinsn->nhop_11s, pstat->hwaddr, MACADDRLEN);   
                        newskb = skb_copy(skb, GFP_ATOMIC);
                        if (newskb) {     
                            memcpy(newskb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
                            newskb->cb[2] = (char)0xff;         // not do aggregation
                            ptxinsn->priv = pPathEntry->priv;
                            newskb->dev = priv->mesh_dev;
                            __rtl8192cd_start_xmit_out(newskb, pstat, ptxinsn);
                        }
                        else {                
                            dev_kfree_skb_any(skb);
                            return 0;
                        }
                    }                       			
        		}
            }
        }

        dev_kfree_skb_any(skb);
        return 0;
    }
    else // unicast
    {
        struct path_sel_entry *pEntry;
        
        memcpy(ptxinsn->mesh_header.DestMACAddr, skb->data, MACADDRLEN);
        memcpy(ptxinsn->mesh_header.SrcMACAddr,  skb->data+MACADDRLEN, MACADDRLEN);
        memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
		
#if defined(RTL_MESH_TXCACHE)
        priv->mesh_txcache.dirty = 0; //clear tx cache first
        memcpy(priv->mesh_txcache.ether_sa,ptxinsn->mesh_header.SrcMACAddr,MACADDRLEN);
#endif


        // search proxy table for dest addr
        pProxyEntry = (struct proxy_table_entry*) HASH_SEARCH(priv->proxy_table, ptxinsn->mesh_header.DestMACAddr);
        mesh_tx_debug("Search Dest:%02x%02x%02x%02x%02x%02x from proxy table\n",
                ptxinsn->mesh_header.DestMACAddr[0],ptxinsn->mesh_header.DestMACAddr[1],ptxinsn->mesh_header.DestMACAddr[2],
                ptxinsn->mesh_header.DestMACAddr[3],ptxinsn->mesh_header.DestMACAddr[4],ptxinsn->mesh_header.DestMACAddr[5]);       
        if(pProxyEntry != NULL) // src isn't me or dest can find in proxy table
        {
            // e.g., bridge table had expired (would it happen?)
            if(memcmp(pProxyEntry->owner, GET_MY_HWADDR, MACADDRLEN) == 0) {                    
                HASH_DELETE(priv->proxy_table, ptxinsn->mesh_header.DestMACAddr);
                mesh_proxy_debug("[%s %d]Delete Proxy entry of %02x:%02x:%02x:%02x:%02x:%02x\n",
                            __func__,__LINE__,ptxinsn->mesh_header.DestMACAddr[0],ptxinsn->mesh_header.DestMACAddr[1],ptxinsn->mesh_header.DestMACAddr[2],ptxinsn->mesh_header.DestMACAddr[3],ptxinsn->mesh_header.DestMACAddr[4],ptxinsn->mesh_header.DestMACAddr[5]);
                dev_kfree_skb_any(skb);
                return 0;
            }
            // The code is important for uni-directional traffic (how often?) to maintain a proxy entry.
            // However, its side effect is to forcedly occupy a proxy entry during the duration of the traffic.
            // pProxyEntry->update_time = xtime;
            pProxyEntry->aging_time = 0;
            
            memcpy(skb->data, pProxyEntry->owner, MACADDRLEN);             
            mesh_tx_debug("found, owner is %02x%02x%02x%02x%02x%02x\n",
                            pProxyEntry->owner[0],pProxyEntry->owner[1],pProxyEntry->owner[2],
                            pProxyEntry->owner[3],pProxyEntry->owner[4],pProxyEntry->owner[5]);

#if defined(RTL_MESH_TXCACHE)
            memcpy(priv->mesh_txcache.ether_da,ptxinsn->mesh_header.DestMACAddr,MACADDRLEN);
            priv->mesh_txcache.da_proxy = pProxyEntry;
#endif
        }
        else {
            priv->mesh_txcache.da_proxy = NULL;
        }

        
		
        pEntry = pathsel_query_table( priv, skb->data );
        if(pEntry != (struct path_sel_entry *)-1) {// has valid route path 
            mesh_tx_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x exist, next-hop %02x:%02x:%02x:%02x:%02x:%02x\n",
                    *(unsigned char *)skb->data,*(unsigned char *)(skb->data+1),*(unsigned char *)(skb->data+2),
                    *(unsigned char *)(skb->data+3),*(unsigned char *)(skb->data+4),*(unsigned char *)(skb->data+5),
                    pEntry->nexthopMAC[0],pEntry->nexthopMAC[1],pEntry->nexthopMAC[2],pEntry->nexthopMAC[3],pEntry->nexthopMAC[4],pEntry->nexthopMAC[5]);
            memcpy(ptxinsn->nhop_11s, pEntry->nexthopMAC, MACADDRLEN);
            ptxinsn->priv = pEntry->priv;
#if defined(RTL_MESH_TXCACHE)
            if(priv->mesh_txcache.da_proxy) {
                memcpy(&(priv->mesh_txcache.txcfg),ptxinsn,sizeof(*ptxinsn));
                priv->mesh_txcache.dirty = 1; /*fire tx cache*/
            }
#endif
        } 
        else {// not have valid route path 
            //static unsigned char zeroAddr[MACADDRLEN] = { 0 };      // fix: 0000072 2008/02/01
            unsigned char Mesh_dest[MACADDRLEN];
            memcpy(Mesh_dest, skb->data, MACADDRLEN);
            memcpy(skb->data, ptxinsn->mesh_header.DestMACAddr, MACADDRLEN);
            memcpy(skb->data+MACADDRLEN, ptxinsn->mesh_header.SrcMACAddr,MACADDRLEN);


#if 0            
            if(memcmp(priv->root_mac, zeroAddr, MACADDRLEN) == 0) // doesn't has root info, run AODV routing protocol
            {
                mesh_tx_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x not exist, find through AODV\n",
                    Mesh_dest[0],Mesh_dest[1],Mesh_dest[2],Mesh_dest[3],Mesh_dest[4],Mesh_dest[5]);
                do_aodv_routing(priv,skb, Mesh_dest);
                return 0;
            }
            else if(memcmp(priv->root_mac, GET_MY_HWADDR, MACADDRLEN) == 0) // i am root, but no path, fire aodv
            {
                mesh_tx_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x not exist, find through AODV becoz I'm root\n",
                        *(unsigned char *)skb->data,*(unsigned char *)(skb->data+1),*(unsigned char *)(skb->data+2),
                        *(unsigned char *)(skb->data+3),*(unsigned char *)(skb->data+4),*(unsigned char *)(skb->data+5));
                do_aodv_routing(priv, skb, Mesh_dest);
                return 0;
            } 
            else  // send to root
#endif                
            {
#if defined(RTK_MESH_REDIRECT_TO_ROOT)
                pEntry = pathsel_query_table( priv, priv->root_mac );
                if(pEntry != (struct path_sel_entry *)-1) { // has valid route path 
                    memcpy(ptxinsn->nhop_11s, pEntry->nexthopMAC, MACADDRLEN);
                    memcpy(skb->data, priv->root_mac, MACADDRLEN);
                    memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
                    ptxinsn->mesh_header.mesh_flag = 0x01;

                    mesh_tx_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x not exist, redirect to Root, next hop:%02x:%02x:%02x:%02x:%02x:%02x\n",
                                *(unsigned char *)skb->data,*(unsigned char *)(skb->data+1),*(unsigned char *)(skb->data+2),
                                *(unsigned char *)(skb->data+3),*(unsigned char *)(skb->data+4),*(unsigned char *)(skb->data+5),
                    pEntry->nexthopMAC[0],pEntry->nexthopMAC[1],pEntry->nexthopMAC[2],pEntry->nexthopMAC[3],pEntry->nexthopMAC[4],pEntry->nexthopMAC[5]);
                }
#endif	//RTK_MESH_REDIRECT_TO_ROOT
                mesh_tx_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x not exist, do AODV\n",
                                    *(unsigned char *)skb->data,*(unsigned char *)(skb->data+1),*(unsigned char *)(skb->data+2),
                                    *(unsigned char *)(skb->data+3),*(unsigned char *)(skb->data+4),*(unsigned char *)(skb->data+5));
                do_aodv_routing(priv, skb, Mesh_dest);

                return 0;
            }
        } // end of else (not have valid route path)
    } // end unicast
#if defined(RTL_MESH_TXCACHE)
dot11s_datapath_decision_end:
#endif
//txsc_path:
    return 1;
}

__IRAM_IN_865X
int mesh_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
    struct stat_info	*pstat=NULL;
	unsigned long x;
	int ret = 0;
    unsigned char zero14[14] = {0};
    struct rtl8192cd_priv *priv = (struct rtl8192cd_priv *)dev->priv;   
    struct tx_insn tx_insn;
    struct path_sel_entry * pEntry;
    
	SAVE_INT_AND_CLI(x);
	SMP_LOCK_XMIT(x);


    if (skb->len < 15)
    {
        _DEBUG_ERR("TX DROP: SKB len small:%d\n", skb->len);
        dev_kfree_skb_any(skb);
        goto end;
    }
   
    skb->cb[2] = 0; // allow aggregation
    skb->cb[0] = '\0';



    // 11s action, send by pathsel daemon
    // the first 14 bytes is zero: 802.3: 6 bytes (src) + 6 bytes (dst) + 2 bytes (protocol)
    if(memcmp(skb->data, zero14, sizeof(zero14))==0) 
    {
        #ifdef CONFIG_RTL_MESH_CROSSBAND
        if(memcmp(GetAddr2Ptr(skb->data+14), GET_MY_HWADDR, MACADDRLEN)) {
            priv = priv->mesh_priv_sc;
        }
        #endif
    
        if(IS_MCAST(GetAddr1Ptr(skb->data+14))) { 
            if (!IS_DRV_OPEN(priv)){
                dev_kfree_skb_any(skb);
                goto end;
            }
            issue_11s_mesh_action(priv, skb, dev);    
            
            #if !defined(CONFIG_RTL_MESH_CROSSBAND)
            if(priv->mesh_priv_sc && GET_MIB(priv->mesh_priv_sc)->dot1180211sInfo.mesh_enable ) { /*send to other band's mesh if exists*/
                if (!IS_DRV_OPEN(priv->mesh_priv_sc)){
                    dev_kfree_skb_any(skb);
                    goto end;
                }
                memcpy(GetAddr2Ptr(skb->data+14), GET_MIB(priv->mesh_priv_sc)->dot11OperationEntry.hwaddr, MACADDRLEN);
                issue_11s_mesh_action(priv->mesh_priv_sc, skb, dev);           
            }    
            #endif            
        }
        else { 
            pEntry = pathsel_query_table( priv, GetAddr3Ptr(skb->data+14) );
            if(pEntry != (struct path_sel_entry *)-1) {// has valid route path 
                if (!IS_DRV_OPEN(pEntry->priv)){
                    dev_kfree_skb_any(skb);
                    goto end;
                }            
                memcpy(GetAddr1Ptr(skb->data+14), pEntry->nexthopMAC, MACADDRLEN);
                memcpy(GetAddr2Ptr(skb->data+14), GET_MIB(pEntry->priv)->dot11OperationEntry.hwaddr, MACADDRLEN);
                issue_11s_mesh_action(pEntry->priv, skb, dev);
            }            
        }
        dev_kfree_skb_any(skb);
    }
    else {


        #ifdef CONFIG_RTL_MESH_CROSSBAND
        /*if this packet is from a 5G sta, transmit use 2g mesh, otherwise, use 5g mesh*/
        pstat = get_stainfo(priv, skb->data + MACADDRLEN);
        if(pstat) {
            if(GET_MIB(priv->mesh_priv_sc)->dot1180211sInfo.mesh_enable) {
                priv = priv->mesh_priv_sc;
            }
        }
        #endif
        

        // drop any packet which has dest to STA but go throu MSH        
        pstat = get_stainfo(priv, skb->data);
        if(pstat!=0 && isSTA(pstat))
        {
            dev_kfree_skb_any(skb);
            goto end;
        }

        /*check another band, too*/
        if(priv->mesh_priv_sc) { 
            pstat = get_stainfo(priv->mesh_priv_sc, skb->data);
            if(pstat!=0 && isSTA(pstat))
            {
                dev_kfree_skb_any(skb);
                goto end;
            }
        }
        
        DECLARE_TXCFG(txcfg, tx_insn);
        skb->dev = priv->dev;
        if(!dot11s_datapath_decision(skb, txcfg, 1)) {//the dest form bridge need be update to proxy table
            goto end;
        }            

        if (!IS_DRV_OPEN(txcfg->priv)){
            dev_kfree_skb_any(skb);
            goto end;
        } 
        pstat = get_stainfo(txcfg->priv, txcfg->nhop_11s);

        #ifdef DETECT_STA_EXISTANCE
        if(pstat && pstat->leave)	{
            txcfg->priv->ext_stats.tx_drops++;
            DEBUG_WARN("TX DROP: sta may leave! %02x%02x%02x%02x%02x%02x\n", pstat->hwaddr[0],pstat->hwaddr[1],pstat->hwaddr[2],pstat->hwaddr[3],pstat->hwaddr[4],pstat->hwaddr[5]);
            dev_kfree_skb_any(skb);
            goto end;
        }
        #endif

        skb->dev = priv->mesh_dev;
	    ret = __rtl8192cd_start_xmit_out(skb, pstat, txcfg);
    }
    
end:    
	RESTORE_INT(x);
	SMP_UNLOCK_XMIT(x);
	return ret;
}



#ifdef  _11s_TEST_MODE_
int mesh_debug_tx1(struct net_device *dev, DRV_PRIV *priv, struct sk_buff *skb)
{
	if(!memcmp("XMT", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 3) )
	{
		unsigned char srcMAC[6]= { 0 };
		mac12_to_6(priv->pmib->dot1180211sInfo.mesh_reservedstr1+3, srcMAC ); 

		if(!memcmp(skb->data+6, srcMAC, 6))
		{
			LOG_MESH_MSG("xmit:%02X %02X %02X %02X %02X %02X [ %02X %02X %02X %02X %02X %02X, %s,%d\n",
				skb->data[0]&0xff, skb->data[1]&0xff, skb->data[2]&0xff, skb->data[3]&0xff, skb->data[4]&0xff, skb->data[5]&0xff,
				skb->data[6]&0xff, skb->data[7]&0xff, skb->data[8]&0xff, skb->data[9]&0xff, skb->data[10]&0xff, skb->data[11]&0xff,
				dev->name, skb->len);

			if((skb->len>341 && skb->len<374)||(skb->len>586 && skb->len<601))
				printk("xmit:%02X %02X %02X %02X %02X %02X [ %02X %02X %02X %02X %02X %02X, %s,%d\n",
					skb->data[0]&0xff, skb->data[1]&0xff, skb->data[2]&0xff, skb->data[3]&0xff, skb->data[4]&0xff, skb->data[5]&0xff,
					skb->data[6]&0xff, skb->data[7]&0xff, skb->data[8]&0xff, skb->data[9]&0xff, skb->data[10]&0xff, skb->data[11]&0xff,
					dev->name, skb->len);
/*
			if(memcmp(skb->data+14, "\x00\x01\x08\x00\x06\x04\x00", 7)==0)
				printk("[%s]",skb->data[21]==2 ? "ARP RSP" : "arp req" ); 
			for(j=0; j<20; j++)
				printk("%02X ", skb->data[j+12]&0xff);
			printk("\n");*/
		}
	}

	if(!memcmp("JasonRelay", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 10)
		&& memcmp(skb->data+14+2, "TEST_TRAFFIC", 12)==0 )
	{
		dev_kfree_skb_any(skb);
		return -1;
	}
	return 0;
}

int mesh_debug_tx2( DRV_PRIV *priv, struct sk_buff *skb)
{
	if(memcmp(skb->data, "**************", 14)==0  )
	{
		if(!memcmp("JasonSender", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 11))
			issue_test_traffic(skb);
		else
			dev_kfree_skb_any(skb);
		return -1;
	}
	return 0;
}

int mesh_debug_tx3(struct net_device *dev, DRV_PRIV *priv, struct sk_buff *skb)
{
/*
	struct stat_info *pstat;
	struct list_head *phead = &priv->sleep_list;
	struct list_head *plist = phead->next;

	while(plist != phead)
	{
		pstat = list_entry(plist, struct stat_info, sleep_list);
		plist = plist->next;
		printMac(pstat->hwaddr);
		printk("... sleeping popen\n");
	}
	printHex(skb->data, 20);
	printk("\n...%d,%s\n", skb->len, dev->name);
*/
	return 0;
}

// multicast data frame use fixedTxRate in advanced setting page
int mesh_debug_tx4(DRV_PRIV *priv, struct tx_insn* txcfg)
{
	if(priv->pmib->dot11StationConfigEntry.autoRate==0 && priv->pmib->dot1180211sInfo.mesh_reserved2&4)
		txcfg->tx_rate = get_rate_from_bit_value(priv->pmib->dot11StationConfigEntry.fixedTxRate);
	return 0;
}


int mesh_debug_tx5(DRV_PRIV *priv, struct tx_insn* txcfg)
{
	if( priv->pmib->dot1180211sInfo.mesh_reserved2&16)
	{
		txcfg->tx_rate = (priv->pmib->dot1180211sInfo.mesh_reserved4&128) ? priv->pmib->dot1180211sInfo.mesh_reserved4 : (priv->pmib->dot1180211sInfo.mesh_reserved4 << 1);
		txcfg->fixed_rate = 1;
	}
	else if(priv->pmib->dot11StationConfigEntry.autoRate==0 && priv->pmib->dot1180211sInfo.mesh_reserved2&4)
	{
		txcfg->tx_rate = get_rate_from_bit_value(priv->pmib->dot11StationConfigEntry.fixedTxRate);
		txcfg->fixed_rate = 1;
	}
	return 0;
}

int mesh_debug_tx6(DRV_PRIV *priv, struct tx_insn* txcfg)
{
	if (priv->pmib->dot1180211sInfo.mesh_reserved2&8)
	{
		txcfg->lowest_tx_rate = txcfg->tx_rate = (priv->pmib->dot1180211sInfo.mesh_reserved4&128) ? priv->pmib->dot1180211sInfo.mesh_reserved4 : (priv->pmib->dot1180211sInfo.mesh_reserved4 << 1);
		txcfg->fixed_rate = 1;
	}
	else
	{
		if (IS_MCAST(GetAddr1Ptr(txcfg->phdr)))
		{
			if(priv->pmib->dot1180211sInfo.mesh_reserved2&256)
			{
				txcfg->lowest_tx_rate = txcfg->tx_rate = (priv->pmib->dot1180211sInfo.mesh_reserved4&128) ? priv->pmib->dot1180211sInfo.mesh_reserved4 : (priv->pmib->dot1180211sInfo.mesh_reserved4 << 1);
				txcfg->fixed_rate = 1;
			}
		}
		else
		{
			if(priv->pmib->dot1180211sInfo.mesh_reserved2&512)
			{
				txcfg->lowest_tx_rate = txcfg->tx_rate = (priv->pmib->dot1180211sInfo.mesh_reserved3&128) ? priv->pmib->dot1180211sInfo.mesh_reserved3 : (priv->pmib->dot1180211sInfo.mesh_reserved3 << 1);
				txcfg->fixed_rate = 1;
			}
		}
	}
	return 0;
}

int mesh_debug_tx7(DRV_PRIV *priv,  struct tx_desc *pdesc)
{
/* in 8190se, pdesc has no member named flen
	if( (!memcmp("JasonSender", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 11) ||
			!memcmp("JasonRelay", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 10) )
			&& (priv->pmib->dot1180211sInfo.mesh_reserved3 == (pdesc->flen & 0xfff) - _CRCLNG_ ) )
		priv->mesh_stats.tx_errors++;
*/
	return 0;
}
int mesh_debug_tx8(DRV_PRIV *priv,  struct tx_desc *pdesc)
{
/* in 8190se, pdesc has no member named flen
	if( (!memcmp("JasonSender", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 11) ||
			!memcmp("JasonRelay", priv->pmib->dot1180211sInfo.mesh_reservedstr1, 10) )
			&& (priv->pmib->dot1180211sInfo.mesh_reserved3  == (pdesc->flen & 0xfff) - _CRCLNG_ ) )
		priv->mesh_stats.tx_bytes += retry;
*/
	return 0;
}
int mesh_debug_tx9(struct tx_insn* txcfg, struct tx_desc_info *pdescinfo)
{
	if( txcfg->is_11s & 32)
		pdescinfo->type =_RESERVED_FRAME_TYPE_;
	return 0;
}
int mesh_debug_tx10(struct tx_insn* txcfg, struct tx_desc_info *pndescinfo)
{
	if( txcfg->is_11s & 16)
		pndescinfo->type =_RESERVED_FRAME_TYPE_;
	return 0;
}

#endif // _11s_TEST_MODE_

#endif //  CONFIG_RTK_MESH
