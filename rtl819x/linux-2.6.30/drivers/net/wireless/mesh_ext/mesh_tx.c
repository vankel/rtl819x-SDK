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

__inline__ void ini_txinsn(struct tx_insn* txcfg, DRV_PRIV *priv)
{
	txcfg->is_11s = 1;
	txcfg->mesh_header.mesh_flag= 1;
	txcfg->mesh_header.TTL = _MESH_HEADER_TTL_;
	txcfg->mesh_header.segNum = getMeshSeq(priv);		
}

int notify_path_found(unsigned char *destaddr, DRV_PRIV *priv) 
{
    unsigned long flags = 0;
    struct sk_buff *pskb;
    struct mesh_rreq_retry_entry * retryEntry;
    struct sk_buff * buf [NUM_TXPKT_QUEUE]; // To record the ALL popped-up skbs at one time, because we don't want enable spinlock for dev_queue_xmit
    int i=0;

    SAVE_INT_AND_CLI(flags);
    SMP_LOCK_MESH_PREQ(flags);

    retryEntry= (struct mesh_rreq_retry_entry*) HASH_SEARCH(priv->mesh_rreq_retry_queue,destaddr);
    if(retryEntry == NULL) { // aodv_expire tx it
        RESTORE_INT(flags);
        SMP_UNLOCK_MESH_PREQ(flags); 
        return 0;
    }

    i = 0;
    do {            
        pskb = (struct sk_buff*)deque(priv,&(retryEntry->pktqueue.head),&(retryEntry->pktqueue.tail),(unsigned int)retryEntry->pktqueue.pSkb,NUM_TXPKT_QUEUE);
        buf[i++] = pskb;            
    }while(pskb != NULL);

    HASH_DELETE(priv->mesh_rreq_retry_queue,retryEntry->MACAddr);
    
    RESTORE_INT(flags);
    SMP_UNLOCK_MESH_PREQ(flags); 


    for(i=0;i<NUM_TXPKT_QUEUE;i++) {
        pskb = buf[i];
        if(pskb == NULL)
            break;

        if(mesh_start_xmit(pskb,  priv->mesh_dev))
            rtl_kfree_skb(priv, pskb, _SKB_TX_);           
    }
    return 0;
}

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
    unsigned long flags = 0;
    SAVE_INT_AND_CLI(flags);
    SMP_LOCK_MESH_PREQ(flags);


    retryEntry = (struct mesh_rreq_retry_entry*) HASH_SEARCH(priv->mesh_rreq_retry_queue,Mesh_dest);
    if (retryEntry == NULL) // new AODV path
    {     
        RESTORE_INT(flags);
        SMP_UNLOCK_MESH_PREQ(flags);
        if(0 != insert_PREQ_entry(Mesh_dest, priv)) {
            dev_kfree_skb_any(skb);
            return;            
        }
        GEN_PREQ_PACKET(Mesh_dest, priv); 



        SAVE_INT_AND_CLI(flags);
        SMP_LOCK_MESH_PREQ(flags);
        retryEntry = (struct mesh_rreq_retry_entry*) HASH_SEARCH(priv->mesh_rreq_retry_queue,Mesh_dest);
    }

    
    /* queue packets*/
    if (retryEntry) {
        if(FALSE == enque(priv,&(retryEntry->pktqueue.head),&(retryEntry->pktqueue.tail), (unsigned long)retryEntry->pktqueue.pSkb, NUM_TXPKT_QUEUE,(void*)skb))
        {             
            dev_kfree_skb_any(skb);
        
            poldskb = (struct sk_buff*)deque(priv,&(retryEntry->pktqueue.head),&(retryEntry->pktqueue.tail),(unsigned int)retryEntry->pktqueue.pSkb,NUM_TXPKT_QUEUE);
            if(poldskb)
                dev_kfree_skb_any(poldskb);

            if(FALSE == enque(priv,&(retryEntry->pktqueue.head),&(retryEntry->pktqueue.tail),(unsigned long)retryEntry->pktqueue.pSkb,NUM_TXPKT_QUEUE,(void*)skb))
                dev_kfree_skb_any(skb);
        }

    }
    else
        dev_kfree_skb_any(skb);


    RESTORE_INT(flags);
    SMP_UNLOCK_MESH_PREQ(flags);    
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
    unsigned char prehop[MACADDRLEN], nexthop[MACADDRLEN];
    unsigned char force_m2u = 0, sta_count = 0;
    struct path_sel_entry* pPathEntry;    
    struct tx_insn tx_insn;
    DRV_PRIV *xmit_priv = NULL;
    UINT16 seqNum;  
    int i,j,ret = 0;

    unsigned long flags = 0;

    if(1 > pfrinfo->mesh_header.TTL) {        
        ret = 1;
        goto ret;
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
                    if(priv->mesh_priv_sc && IS_DRV_OPEN(priv->mesh_priv_sc) && GET_MIB(priv->mesh_priv_sc)->dot1180211sInfo.mesh_enable) {
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


                            priv = xmit_priv;
                            SAVE_INT_AND_CLI(flags);
                            SMP_LOCK_XMIT(flags);    
                            __rtl8192cd_start_xmit_out(pnewskb_send, pstat, &tx_insn);  
                            RESTORE_INT(flags);
                            SMP_UNLOCK_XMIT(flags);
                            priv = priv->mesh_priv_first;
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

                xmit_priv = NULL;
                SMP_LOCK_MESH_PATH(flags);                
                if (priv->pathsel_table->entry_array[i].dirty) 
                { 
                    pPathEntry = ((struct path_sel_entry*)priv->pathsel_table->entry_array[i].data);

                    if(memcmp(pPathEntry->destMAC, pfrinfo->sa, MACADDRLEN) &&
                       memcmp(pPathEntry->nexthopMAC, prehop, MACADDRLEN) &&
                       memcmp(pPathEntry->nexthopMAC, pfrinfo->sa, MACADDRLEN)) {
                        xmit_priv = pPathEntry->priv;
                        memcpy(nexthop, pPathEntry->nexthopMAC, MACADDRLEN);  
                    }                                                                                                                                    
                }
                SMP_UNLOCK_MESH_PATH(flags);

                if(xmit_priv) {
                    pstat = get_stainfo(xmit_priv, nexthop);
                    if(pstat && pstat->mesh_neighbor_TBL.seqNum != seqNum) {
                        pstat->mesh_neighbor_TBL.seqNum = seqNum;

                        pnewskb_send = skb_copy(pnewskb, GFP_ATOMIC);
                        if(pnewskb_send == NULL) {
                            goto relay_end;
                        }         
                        tx_insn.is_11s = RELAY_11S;
                        tx_insn.priv = xmit_priv;  
                        pnewskb_send->dev = priv->mesh_dev;


                        priv = xmit_priv;
                        SAVE_INT_AND_CLI(flags);
                        SMP_LOCK_XMIT(flags);
                        __rtl8192cd_start_xmit_out(pnewskb_send, pstat, &tx_insn);
                        RESTORE_INT(flags);
                        SMP_UNLOCK_XMIT(flags);
                        priv = priv->mesh_priv_first;
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
         goto ret;
        
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
        
        SMP_LOCK_MESH_PATH(flags);
        pEntry = HASH_SEARCH(priv->pathsel_table, pfrinfo->da );		
        if(pEntry) {
            xmit_priv = pEntry->priv;
            memcpy(nexthop, pEntry->nexthopMAC, MACADDRLEN);
        }
        SMP_UNLOCK_MESH_PATH(flags);
        
        if(pEntry == NULL) // not have valid route path
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
            ret = 1;

        } // if(pEntry == (struct path_sel_entry *)-1)
        else {
#ifdef MESH_AMSDU
            if(pfrinfo->is_11s&1) 
#endif
            {
                pfrinfo->is_11s = 2;
                if(skb_p80211_to_ether(skb->dev, privacy, pfrinfo) == FAIL) { // for e.g., CISCO CDP which has unsupported LLC's vendor ID
                    ret = 1;
                    goto ret;
                }
            }

            pstat = get_stainfo(xmit_priv, nexthop);
            tx_insn.is_11s = RELAY_11S;
            tx_insn.priv = xmit_priv;  
            skb->dev = priv->mesh_dev;

            priv = xmit_priv;
            SAVE_INT_AND_CLI(flags);
            SMP_LOCK_XMIT(flags);
            __rtl8192cd_start_xmit_out(skb, pstat, &tx_insn);
            RESTORE_INT(flags);
            SMP_UNLOCK_XMIT(flags);
            priv = priv->mesh_priv_first;         
            
            ret = 0;
        }
    }


ret:


    return ret;

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
    #ifdef SMP_SYNC
    unsigned long flags = 0;
    #endif

    SMP_LOCK_MESH_PROXY(flags);
    if(priv->mesh_txcache.dirty == 1) { /*if the tx cache is valid*/
        if(!memcmp(da, priv->mesh_txcache.da_proxy->owner,MACADDRLEN)) {
            mesh_tx_debug("TX cache of %02X:%02X:%02X:%02X:%02X:%02X expired\n", 
                    priv->mesh_txcache.da_proxy->owner[0], priv->mesh_txcache.da_proxy->owner[1], priv->mesh_txcache.da_proxy->owner[2],
                    priv->mesh_txcache.da_proxy->owner[3], priv->mesh_txcache.da_proxy->owner[4], priv->mesh_txcache.da_proxy->owner[5]);
            priv->mesh_txcache.dirty = 0;
            ret = 1;
        }      
    }
    SMP_UNLOCK_MESH_PROXY(flags);

    return ret;
}

int match_tx_cache(struct rtl8192cd_priv *priv,struct sk_buff *skb,struct tx_insn* ptxinsn)
{
    int ret=0;
    #ifdef SMP_SYNC
    unsigned long flags = 0;
    #endif

    SMP_LOCK_MESH_PROXY(flags);    
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
    SMP_UNLOCK_MESH_PROXY(flags);
    return ret;
}
#endif


int dot11s_datapath_decision(struct sk_buff *skb, /*struct net_device *dev,*/ struct tx_insn* ptxinsn, int isUpdateProxyTable)
{

#ifdef SMP_SYNC
    unsigned long proxy_flags = 0;
    unsigned long path_flags = 0;
#endif
  

#ifdef NETDEV_NO_PRIV
    DRV_PRIV *priv = ((DRV_PRIV *)netdev_priv(skb->dev))->wlan_priv;
#else
    DRV_PRIV *priv = (DRV_PRIV *)skb->dev->priv;
#endif

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
    unsigned char nextHopMac[MACADDRLEN];
    
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
        if(memcmp(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN)) { // the entry briged by me
            mesh_proxy_insert(priv, skb->data+MACADDRLEN);
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
                    if(priv->mesh_priv_sc && IS_DRV_OPEN(priv->mesh_priv_sc) && GET_MIB(priv->mesh_priv_sc)->dot1180211sInfo.mesh_enable) {
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

                                priv = xmit_priv;
                                SMP_LOCK_XMIT(x);
                                __rtl8192cd_start_xmit_out(newskb, pstat, ptxinsn);
                                SMP_UNLOCK_XMIT(x);
                                priv = priv->mesh_priv_first;
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
                xmit_priv = NULL;
                SMP_LOCK_MESH_PATH(path_flags);
                if (priv->pathsel_table->entry_array[i].dirty) 
        		{ 
        			pPathEntry = ((struct path_sel_entry*)priv->pathsel_table->entry_array[i].data);  
                    memcpy(nextHopMac, pPathEntry->nexthopMAC, MACADDRLEN);
                    xmit_priv = pPathEntry->priv;                    
        		}
                SMP_UNLOCK_MESH_PATH(path_flags);
 
                if(xmit_priv) {
                    pstat = get_stainfo(xmit_priv, nextHopMac);                                                 
                    if(pstat && pstat->mesh_neighbor_TBL.seqNum != seqNum) {
                        pstat->mesh_neighbor_TBL.seqNum = seqNum;
                        memcpy(ptxinsn->nhop_11s, pstat->hwaddr, MACADDRLEN);   
                        newskb = skb_copy(skb, GFP_ATOMIC);
                        if (newskb) {     
                            memcpy(newskb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
                            newskb->cb[2] = (char)0xff;         // not do aggregation
                            ptxinsn->priv = xmit_priv;
                            newskb->dev = priv->mesh_dev;

                            priv = xmit_priv;
                            SMP_LOCK_XMIT(x);
                            __rtl8192cd_start_xmit_out(newskb, pstat, ptxinsn);
                            SMP_UNLOCK_XMIT(x);
                            priv = priv->mesh_priv_first;
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

        SMP_LOCK_MESH_PROXY(proxy_flags);
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
                SMP_UNLOCK_MESH_PROXY(proxy_flags);
                mesh_proxy_debug("[%s %d]Delete Proxy entry of %02x:%02x:%02x:%02x:%02x:%02x\n",
                            __func__,__LINE__,ptxinsn->mesh_header.DestMACAddr[0],ptxinsn->mesh_header.DestMACAddr[1],ptxinsn->mesh_header.DestMACAddr[2],ptxinsn->mesh_header.DestMACAddr[3],ptxinsn->mesh_header.DestMACAddr[4],ptxinsn->mesh_header.DestMACAddr[5]);
                dev_kfree_skb_any(skb);
                return 0;
            }
            // The code is important for uni-directional traffic (how often?) to maintain a proxy entry.
            // However, its side effect is to forcedly occupy a proxy entry during the duration of the traffic.
            // pProxyEntry->update_time = jiffies;
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
        
        
        SMP_LOCK_MESH_PATH(path_flags);
        pEntry = HASH_SEARCH(priv->pathsel_table, skb->data );
        if(pEntry ) {// has valid route path 
            mesh_tx_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x exist, next-hop %02x:%02x:%02x:%02x:%02x:%02x\n",
                    *(unsigned char *)skb->data,*(unsigned char *)(skb->data+1),*(unsigned char *)(skb->data+2),
                    *(unsigned char *)(skb->data+3),*(unsigned char *)(skb->data+4),*(unsigned char *)(skb->data+5),
                    pEntry->nexthopMAC[0],pEntry->nexthopMAC[1],pEntry->nexthopMAC[2],pEntry->nexthopMAC[3],pEntry->nexthopMAC[4],pEntry->nexthopMAC[5]);
            memcpy(ptxinsn->nhop_11s, pEntry->nexthopMAC, MACADDRLEN);
            ptxinsn->priv = pEntry->priv;
        }
        SMP_UNLOCK_MESH_PATH(path_flags);
        
        if(pEntry) {  // has valid route path      
#if defined(RTL_MESH_TXCACHE)
            if(priv->mesh_txcache.da_proxy) {
                memcpy(&(priv->mesh_txcache.txcfg),ptxinsn,sizeof(*ptxinsn));
                priv->mesh_txcache.dirty = 1; /*fire tx cache*/
            }
#endif
            SMP_UNLOCK_MESH_PROXY(proxy_flags);
        } 
        else {// not have valid route path 
            unsigned char Mesh_dest[MACADDRLEN];
            
            SMP_UNLOCK_MESH_PROXY(proxy_flags);
            memcpy(Mesh_dest, skb->data, MACADDRLEN);
            memcpy(skb->data, ptxinsn->mesh_header.DestMACAddr, MACADDRLEN);
            memcpy(skb->data+MACADDRLEN, ptxinsn->mesh_header.SrcMACAddr,MACADDRLEN);

            mesh_tx_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x not exist, do AODV\n",
                                *(unsigned char *)skb->data,*(unsigned char *)(skb->data+1),*(unsigned char *)(skb->data+2),
                                *(unsigned char *)(skb->data+3),*(unsigned char *)(skb->data+4),*(unsigned char *)(skb->data+5));
            do_aodv_routing(priv, skb, Mesh_dest);

            return 0;

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

    int ret = 0;
    unsigned char zero14[14] = {0};

#ifdef NETDEV_NO_PRIV
    struct rtl8192cd_priv *priv = ((struct rtl8192cd_priv *)netdev_priv(dev))->wlan_priv;
#else
    struct rtl8192cd_priv *priv = (struct rtl8192cd_priv *)dev->priv; 
#endif

    struct rtl8192cd_priv * orig_priv = priv;
    struct tx_insn tx_insn;
    struct path_sel_entry * pEntry;
    struct rtl8192cd_priv * xmit_priv = NULL;
#ifdef SMP_SYNC
    unsigned long flags = 0;
#endif
#ifndef SMP_SYNC
    unsigned long x;
#endif

    SAVE_INT_AND_CLI(x);
	
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
            SMP_LOCK_MESH_PATH(flags);
            pEntry = HASH_SEARCH(priv->pathsel_table, GetAddr3Ptr(skb->data+14) );
            if(pEntry) {// has valid route path 
                if (!IS_DRV_OPEN(pEntry->priv)){
                    SMP_UNLOCK_MESH_PATH(flags);
                    dev_kfree_skb_any(skb);                    
                    goto end;
                }            
                xmit_priv = pEntry->priv;
                memcpy(GetAddr1Ptr(skb->data+14), pEntry->nexthopMAC, MACADDRLEN);
                memcpy(GetAddr2Ptr(skb->data+14), GET_MIB(pEntry->priv)->dot11OperationEntry.hwaddr, MACADDRLEN);               
            }   
            SMP_UNLOCK_MESH_PATH(flags);

            if(pEntry)
                issue_11s_mesh_action(xmit_priv, skb, dev);
        }
        dev_kfree_skb_any(skb);
    }
    else {


        #ifdef CONFIG_RTL_MESH_CROSSBAND
        /*if this packet is from a 5G sta, transmit use 2g mesh, otherwise, use 5g mesh*/
        pstat = get_stainfo(priv, skb->data + MACADDRLEN);
        if(pstat) {
            if(IS_DRV_OPEN(priv->mesh_priv_sc) && GET_MIB(priv->mesh_priv_sc)->dot1180211sInfo.mesh_enable) {
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
        if(priv->mesh_priv_sc && IS_DRV_OPEN(priv->mesh_priv_sc)) { 
            pstat = get_stainfo(priv->mesh_priv_sc, skb->data);
            if(pstat!=0 && isSTA(pstat))
            {
                dev_kfree_skb_any(skb);
                goto end;
            }
        }

        
        {
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
            priv = txcfg->priv;
            SMP_LOCK_XMIT(x);
            ret = __rtl8192cd_start_xmit_out(skb, pstat, txcfg);
            SMP_UNLOCK_XMIT(x);
        }
    }

end:    
    priv = orig_priv;
    RESTORE_INT(x);

    return ret;
}

#endif //  CONFIG_RTK_MESH
