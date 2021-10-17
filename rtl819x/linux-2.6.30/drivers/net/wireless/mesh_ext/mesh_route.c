/*
 *      Handling routines for Mesh in 802.11 SME (Station Management Entity)
 *
 *      PS: All extern function in ../8190n_headers.h
 */
#define _MESH_ROUTE_C_

#ifdef CONFIG_RTL8192CD
#include "../rtl8192cd/8192cd.h"
#include "../rtl8192cd/8192cd_headers.h"
#else
#include "../rtl8190/8190n.h"
#include "../rtl8190/8190n_headers.h"
#endif
#include "./mesh_route.h"


#ifdef CONFIG_RTK_MESH

unsigned short getMeshSeq(DRV_PRIV *priv)
{
	unsigned long flags;
	SAVE_INT_AND_CLI(flags);
	if(priv->pshare->meshare.seq == 0xffff)
		priv->pshare->meshare.seq = 1;
	else
		priv->pshare->meshare.seq++;
	RESTORE_INT(flags);
	return priv->pshare->meshare.seq;
}


unsigned short getMeshMulticastSeq(DRV_PRIV *priv)
{
	unsigned long flags;
	SAVE_INT_AND_CLI(flags);

	if(priv->seqNum == 0xffff)
		priv->seqNum = 1;
	else
		priv->seqNum++;
  	RESTORE_INT(flags);
	return priv->seqNum;
}


//pepsi
#ifdef PU_STANDARD
UINT8 getPUSeq(DRV_PRIV *priv)
{
	unsigned long flags;
	SAVE_INT_AND_CLI(flags);
	if(priv->pshare->meshare.PUseq == 0xff)
		priv->pshare->meshare.PUseq = 1U;
	else
		priv->pshare->meshare.PUseq++;
	RESTORE_INT(flags);
	return priv->pshare->meshare.PUseq;
}
#endif


// return 0: duplicate
// return 1: ok
unsigned short chkMeshSeq(struct path_sel_entry *pPathselEntry, unsigned char *srcMac, unsigned short seq)
{
    unsigned short idx = seq & (SZ_HASH_IDX2 - 1);

    if(seq == pPathselEntry->RecentSeq[idx]) {
        return 0;

    }

    pPathselEntry->RecentSeq[idx] = seq;
    return 1;
   
}


void insert_PREQ_entry(char *targetMac, DRV_PRIV *priv)
{
	struct mesh_rreq_retry_entry *retryEntry;
	retryEntry= (struct mesh_rreq_retry_entry*) priv->mesh_rreq_retry_queue->search_entry(priv->mesh_rreq_retry_queue, targetMac);
	if(	retryEntry== NULL)
	{
		mesh_route_debug("PREQ to find %02x:%02x:%02x:%02x:%02x:%02x queued at %lu\n",
						targetMac[0],targetMac[1],targetMac[2],targetMac[3],targetMac[4],targetMac[5],jiffies);
		if(priv->RreqEnd == priv->RreqBegin)
		{
			u8 *oldmac = priv->RreqMAC[priv->RreqEnd];
			retryEntry= (struct mesh_rreq_retry_entry*) priv->mesh_rreq_retry_queue->search_entry(priv->mesh_rreq_retry_queue, oldmac);
		
			// retryEntry might be null for the first time
			// It has a potential bug when search_entry retun NULL (why?) but it it indeed a new round
			if(retryEntry) 
			{ 
				if(retryEntry->ptr)
				{
					struct sk_buff *poldskb;
					poldskb = (struct sk_buff*)deque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE);
					while(poldskb)
					{
						dev_kfree_skb_any(poldskb);
						poldskb = (struct sk_buff*)deque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE);
					}
				}
				priv->mesh_rreq_retry_queue->delete_entry(priv->mesh_rreq_retry_queue,retryEntry->MACAddr);
				priv->RreqBegin = (priv->RreqBegin + 1)%AODV_RREQ_TABLE_SIZE;
			}
		}

		memcpy(priv->RreqMAC[(priv->RreqEnd)], targetMac, MACADDRLEN);		
		(priv->RreqEnd) = ((priv->RreqEnd) + 1)%AODV_RREQ_TABLE_SIZE;
		priv->mesh_rreq_retry_queue->insert_entry(priv->mesh_rreq_retry_queue, targetMac, NULL);
		retryEntry= (struct mesh_rreq_retry_entry*) priv->mesh_rreq_retry_queue->search_entry(priv->mesh_rreq_retry_queue, targetMac);						
	}
	if(retryEntry) 
	{
		mesh_route_debug("PREQ to find %02x:%02x:%02x:%02x:%02x:%02x refreshed at %lu\n",
						retryEntry->MACAddr[0],retryEntry->MACAddr[1],retryEntry->MACAddr[2],retryEntry->MACAddr[3],retryEntry->MACAddr[4],retryEntry->MACAddr[5],jiffies);
		retryEntry->TimeStamp=retryEntry->createTime=jiffies;		
		retryEntry->Retries = 0;		// fix: 00000043 2007/11/29
		memcpy(retryEntry->MACAddr, targetMac, MACADDRLEN);						
	}
}

void GEN_PREQ_PACKET(char *targetMac, DRV_PRIV *priv, char insert)
{
	DOT11s_GEN_RREQ_PACKET rreq_event;
	memset((void*)&rreq_event, 0x0, sizeof(DOT11s_GEN_RREQ_PACKET));
	rreq_event.EventId = DOT11_EVENT_PATHSEL_GEN_RREQ;	
	rreq_event.IsMoreEvent = 0;
	memcpy(rreq_event.MyMACAddr,  GET_MY_HWADDR ,MACADDRLEN);
	memcpy(rreq_event.destMACAddr,  targetMac ,MACADDRLEN);
	rreq_event.TTL = _MESH_HEADER_TTL_;
	rreq_event.Seq_num = getMeshSeq(priv);
	DOT11_EnQueue2((unsigned long)priv, priv->pathsel_queue, (unsigned char*)&rreq_event, sizeof(DOT11s_GEN_RREQ_PACKET));
	notifyPathSelection(priv);
		if(insert)
			insert_PREQ_entry(targetMac, priv);
}
		   

//modify by Joule for MESH HEADER
unsigned char* getMeshHeader(DRV_PRIV *priv, int wep_mode, unsigned char* pframe)
{
	INT		payload_offset;
	struct wlan_llc_t      *e_llc;
	struct wlan_snap_t     *e_snap;
	int wlan_pkt_format = WLAN_PKT_FORMAT_OTHERS;

	payload_offset = get_hdrlen(priv, pframe);

	if (GetPrivacy(pframe)) {
		if (((wep_mode == _WEP_40_PRIVACY_) || (wep_mode == _WEP_104_PRIVACY_))) {
			payload_offset += 4;
		}
		else if ((wep_mode == _TKIP_PRIVACY_) || (wep_mode == _CCMP_PRIVACY_)) {
			payload_offset += 8;
		}
		else {
			DEBUG_ERR("unallowed wep_mode privacy=%d\n", wep_mode);
			return NULL;
		}
	}

	e_llc = (struct wlan_llc_t *) (pframe + payload_offset);
	e_snap = (struct wlan_snap_t *) (pframe + payload_offset + sizeof(struct wlan_llc_t));

	if (e_llc->dsap==0xaa && e_llc->ssap==0xaa && e_llc->ctl==0x03) {

		if ( !memcmp(e_snap->oui, oui_rfc1042, WLAN_IEEE_OUI_LEN)) {
			wlan_pkt_format = WLAN_PKT_FORMAT_SNAP_RFC1042;
			if( !memcmp(&e_snap->type, SNAP_ETH_TYPE_IPX, 2) )
				wlan_pkt_format = WLAN_PKT_FORMAT_IPX_TYPE4;
			else if( !memcmp(&e_snap->type, SNAP_ETH_TYPE_APPLETALK_AARP, 2))
				wlan_pkt_format = WLAN_PKT_FORMAT_APPLETALK;
		}
		else if ( !memcmp(e_snap->oui, SNAP_HDR_APPLETALK_DDP, WLAN_IEEE_OUI_LEN) &&
					!memcmp(&e_snap->type, SNAP_ETH_TYPE_APPLETALK_DDP, 2) )
				wlan_pkt_format = WLAN_PKT_FORMAT_APPLETALK;
		else if ( !memcmp( e_snap->oui, oui_8021h, WLAN_IEEE_OUI_LEN))
			wlan_pkt_format = WLAN_PKT_FORMAT_SNAP_TUNNEL;
	}

	if ( (wlan_pkt_format == WLAN_PKT_FORMAT_SNAP_RFC1042)
			|| (wlan_pkt_format == WLAN_PKT_FORMAT_SNAP_TUNNEL) ) {
		payload_offset +=  sizeof(struct wlan_llc_t) + sizeof(struct wlan_snap_t);
	}

	return pframe+payload_offset;
}

void notifyPathSelection(DRV_PRIV *priv)
{ 
        struct task_struct *p;
        
        if(priv->pid_pathsel != 0){
                read_lock(&tasklist_lock); 
                p = find_task_by_vpid(priv->pid_pathsel);
                read_unlock(&tasklist_lock);
                if(p)
                {
                        // printk("send signal from kernel\n");
                        send_sig(SIGUSR1,p,0); 
                }
                else {
                        priv->pid_pathsel = 0;
                }
        }
}


/*
 *	@brief	MESH MP time aging expire
 *
 *	@param	task_priv: priv
 *
 *	@retval	void
 */
 // chuangch 10.19
 #ifdef MESH_ROUTE_MAINTENANCE
 void route_maintenance(DRV_PRIV *priv)
 { 
    const int tbl_sz = 1 << priv->pathsel_table->table_size_power;
    int i;
#ifdef __LINUX_2_6__
    struct timespec now = xtime;
#else
    struct timeval now = xtime;
#endif

    for (i = 0; i < tbl_sz; i++){
        if(priv->pathsel_table->entry_array[i].dirty){

            // Gallardo test 2008.0901			
            struct path_sel_entry *entry = (struct path_sel_entry*)priv->pathsel_table->entry_array[i].data;

            if( entry->flag==0 && (((entry->metric > ((int)(entry->hopcount))<<8 )
                && (now.tv_sec - entry->routeMaintain.tv_sec > HWMP_PREQ_REFRESH_PERIOD ))
                || (now.tv_sec - entry->routeMaintain.tv_sec > HWMP_PREQ_REFRESH_PERIOD2)))
            {				
                entry->routeMaintain = xtime;

                if(MESH_SPINLOCK(lock_Rreq)) {
                    GEN_PREQ_PACKET( entry->destMAC, priv, 1);
                    MESH_SPINUNLOCK(lock_Rreq);
                } else {
                    panic_printk("%s suffer racing issue\n",__func__);
                }
            }
        }
    }
}
 #endif
 /*
 void rreq_retry(struct rtk8185_priv *priv, struct mesh_rreq_retry_entry *retryEntry){ 
 // chuangch
	 GEN_PREQ_PACKET(retryEntry->MACAddr, priv, 0);
	 retryEntry->Retries++;
	 retryEntry->TimeStamp=jiffies;
	 return;
 }*/
 
void aodv_expire(void *task_priv)
{
	DRV_PRIV *priv = (DRV_PRIV *)task_priv;
	struct sk_buff *pskb;
	struct mesh_rreq_retry_entry *retryEntry;
    unsigned long flags;
	int i=0;		 

#if 0 //defined(Q_TX_AFTER_ONE_AODV_RETRY)
    struct sk_buff* buf[AODV_RREQ_TABLE_SIZE]; // this version only send ONE packet for each queue
    int j=AODV_RREQ_TABLE_SIZE;
	while (j>0)
		buf[--j] = NULL;
#endif

	//MESH_LOCK(lock_Rreq, flags);
	if(MESH_SPINLOCK(lock_Rreq)) {
		for(i=(priv->RreqBegin);i!=(priv->RreqEnd);i=((i+1)%AODV_RREQ_TABLE_SIZE)) 
		{
			retryEntry= (struct mesh_rreq_retry_entry*) priv->mesh_rreq_retry_queue->search_entry(priv->mesh_rreq_retry_queue,priv->RreqMAC[i]);
			if(retryEntry==NULL) {
				if( i!=priv->RreqBegin )
					memcpy(priv->RreqMAC[ i ], priv->RreqMAC[priv->RreqBegin], MACADDRLEN);			 
				priv->RreqBegin = (priv->RreqBegin+1)%AODV_RREQ_TABLE_SIZE;
				continue;
			}
	 
			if(time_after(jiffies, (UINT32)(retryEntry->TimeStamp)+ HWMP_NETDIAMETER_TRAVERSAL_TIME)) {
				if(retryEntry->ptr==NULL)	
				{
					mesh_route_debug("PREQ to find %02x:%02x:%02x:%02x:%02x:%02x purged at %lu\n",
						retryEntry->MACAddr[0],retryEntry->MACAddr[1],retryEntry->MACAddr[2],retryEntry->MACAddr[3],retryEntry->MACAddr[4],retryEntry->MACAddr[5],jiffies);

					priv->mesh_rreq_retry_queue->delete_entry(priv->mesh_rreq_retry_queue,retryEntry->MACAddr);
					if( i!=priv->RreqBegin )
						memcpy(priv->RreqMAC[ i ], priv->RreqMAC[priv->RreqBegin], MACADDRLEN);			 
					priv->RreqBegin = (priv->RreqBegin+1)%AODV_RREQ_TABLE_SIZE;
				}
				else if (retryEntry->Retries > HWMP_MAX_PREQ_RETRIES )
				{
					mesh_route_debug("PREQ to find %02x:%02x:%02x:%02x:%02x:%02x exceed retry limit at %lu\n",
						retryEntry->MACAddr[0],retryEntry->MACAddr[1],retryEntry->MACAddr[2],retryEntry->MACAddr[3],retryEntry->MACAddr[4],retryEntry->MACAddr[5],jiffies);
                
					pskb=(struct sk_buff*)deque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE);
					while(pskb)
					{
						dev_kfree_skb_any(pskb);
						pskb=(struct sk_buff*)deque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE);
					}
#if defined(RTK_MESH_REMOVE_PATH_AFTER_AODV_TIMEOUT)
                    SAVE_INT_AND_CLI(flags);
					HASH_DELETE(priv->pathsel_table,retryEntry->MACAddr);
                    RESTORE_INT(flags);
					mesh_route_debug("Path to %02x:%02x:%02x:%02x:%02x:%02x removed at %lu\n",
						retryEntry->MACAddr[0],retryEntry->MACAddr[1],retryEntry->MACAddr[2],retryEntry->MACAddr[3],retryEntry->MACAddr[4],retryEntry->MACAddr[5],jiffies);
#endif
					priv->mesh_rreq_retry_queue->delete_entry(priv->mesh_rreq_retry_queue,retryEntry->MACAddr);
					if( i!=priv->RreqBegin )
						memcpy(priv->RreqMAC[ i ], priv->RreqMAC[priv->RreqBegin], MACADDRLEN);				 
					priv->RreqBegin = (priv->RreqBegin+1)%AODV_RREQ_TABLE_SIZE;
				} else {
					//pskb = (struct sk_buff*)deque(priv,&(retryEntry->ptr->head),&(retryEntry->ptr->tail),(unsigned int)retryEntry->ptr->pSkb,NUM_TXPKT_QUEUE);
					GEN_PREQ_PACKET(retryEntry->MACAddr, priv, 0);
                    // add by chuangch 0928
					retryEntry->TimeStamp=jiffies;
					retryEntry->Retries++;

					mesh_route_debug("PREQ to find %02x:%02x:%02x:%02x:%02x:%02x retried:%d,at %lu\n",
						retryEntry->MACAddr[0],retryEntry->MACAddr[1],retryEntry->MACAddr[2],
						retryEntry->MACAddr[3],retryEntry->MACAddr[4],retryEntry->MACAddr[5],retryEntry->Retries,jiffies);

#if 0 //defined(Q_TX_AFTER_ONE_AODV_RETRY)
					if (pskb) {
						buf[j++] = pskb; // we call toAllPortal later
						if(j==AODV_RREQ_TABLE_SIZE) // no more round
							break;
					}

					if(retryEntry->ptr->head == retryEntry->ptr->tail)
					{
						mesh_route_debug("PREQ to find %02x:%02x:%02x:%02x:%02x:%02x removed,at %lu\n",
							retryEntry->MACAddr[0],retryEntry->MACAddr[1],retryEntry->MACAddr[2],
							retryEntry->MACAddr[3],retryEntry->MACAddr[4],retryEntry->MACAddr[5],jiffies);
                    
						priv->mesh_rreq_retry_queue->delete_entry(priv->mesh_rreq_retry_queue,retryEntry->MACAddr);
							 
						// change with the first entry
						if( i!=priv->RreqBegin )
							memcpy(priv->RreqMAC[i], priv->RreqMAC[priv->RreqBegin], MACADDRLEN);
	 
						priv->RreqBegin = (priv->RreqBegin+1)%AODV_RREQ_TABLE_SIZE;
					}
#endif
				} // (retryEntry->ptr!=NULL) and (not too old)
			} // if(time_after) 
		} // end of for(i=(priv->RreqBegin);i<AODV_RREQ_TABLE_SIZE;i++)

		//MESH_UNLOCK(lock_Rreq, flags);
		MESH_SPINUNLOCK(lock_Rreq);
	} else {
		panic_printk("%s suffer racing issue\n",__func__);
	}
		 
#if 0// defined(Q_TX_AFTER_ONE_AODV_RETRY)		 
	 for(j=0;j<AODV_RREQ_TABLE_SIZE;j++)
	 {
		 pskb = buf[j];
			 
		 if(pskb ==NULL)
			 goto j_no_more;
	 
		 //sending the data to all portals
		 toAllPortal(pskb,priv);
	 }
j_no_more:
#endif

	 return;
	 
 }
 

void init_mpp_pool(struct mpp_tb* pTB)
{
	int i;
	for (i = 0; i < MAX_MPP_NUM; i++) {
		pTB->pann_mpp_pool[i].flag = 0;
	}
	pTB->pool_count = 0;
}

// following functions modified by chuangch 2007.09.14 for pathsel_talbe being supported for hash function
struct path_sel_entry *pathsel_query_table(DRV_PRIV *priv ,unsigned char destaddr[MACADDRLEN] )
{
	struct path_sel_entry *pEntry = (struct path_sel_entry *)priv->pathsel_table->search_entry(priv->pathsel_table, destaddr);

	if(pEntry)
	{
		return pEntry;
	}
	else
	{
		return (struct path_sel_entry *)-1;
	}
}
//int pathsel_modify_table_entry( DRV_PRIV *priv, unsigned char destaddr[MACADDRLEN], struct path_sel_entry *pEntry)
int pathsel_modify_table_entry(DRV_PRIV *priv, struct path_sel_entry *pEntry)
{
    struct path_sel_entry *entry;
    entry = (struct path_sel_entry *)priv->pathsel_table->search_entry(priv->pathsel_table, pEntry->destMAC);
    if(entry && entry->flag ==0) {
        /*prevent update path relative to any invalid neighbor*/
        if(get_stainfo(priv, pEntry->nexthopMAC)) {
            pEntry->priv = priv;                
        }
        #ifndef CONFIG_RTL_MESH_CROSSBAND
        else if(priv->mesh_priv_sc && get_stainfo(priv->mesh_priv_sc, pEntry->nexthopMAC)) {
            pEntry->priv = priv->mesh_priv_sc;
        }
        #endif
        else {
            return -1;
        }

        memcpy(entry, pEntry, (int)&((struct path_sel_entry*)0)->start);
        entry->update_time = xtime;
        entry->priv = pEntry->priv;
#if defined(RTL_MESH_TXCACHE)
        expire_mesh_txcache(priv, pEntry->destMAC);
#endif


        mesh_route_debug("Update Path of %02x:%02x:%02x:%02x:%02x:%02x-%02x:%02x:%02x:%02x:%02x:%02x\n\tMetric from %d to %d\n\tDSN from %u tp %u\n",
                pEntry->nexthopMAC[0],pEntry->nexthopMAC[1],pEntry->nexthopMAC[2],
                pEntry->nexthopMAC[3],pEntry->nexthopMAC[4],pEntry->nexthopMAC[5],
                pEntry->destMAC[0],pEntry->destMAC[1],pEntry->destMAC[2],
                pEntry->destMAC[3],pEntry->destMAC[4],pEntry->destMAC[5],entry->metric,pEntry->metric,entry->dsn,pEntry->dsn);

        return 0;
    }

    return -1;
}
 int pathsel_table_entry_insert_tail( DRV_PRIV *priv, struct path_sel_entry *pEntry)
{
    return priv->pathsel_table->insert_entry( priv->pathsel_table, pEntry->destMAC, pEntry);
}
 int pathsel_table_entry_insert_head(struct path_sel_entry *pEntry)
{
    return -1;
}

int remove_path_entry(DRV_PRIV *priv,unsigned char *invalid_addr)
{
    struct proxy_table_entry *pEntry = 0;   
    int i;
        
    //clear conresponding path table entry  
    priv->pathsel_table->delete_entry( priv->pathsel_table, invalid_addr );
#if defined(RTL_MESH_TXCACHE)
    expire_mesh_txcache(priv, invalid_addr);
#endif
    //clear conresponding proxy entry               
    for(i=0;i<(1 << priv->proxy_table->table_size_power);i++)
    {
        if(priv->proxy_table->entry_array[i].dirty) {
            pEntry = (struct proxy_table_entry *)(priv->proxy_table->entry_array[i].data);
            if(memcmp(invalid_addr,pEntry->owner,MACADDRLEN)==0){
                priv->proxy_table->entry_array[i].dirty = 0;     
                mesh_proxy_debug("[%s %d]Delete Proxy entry of %02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x\n",           
                            __func__,__LINE__,pEntry->owner[0],pEntry->owner[1],pEntry->owner[2],pEntry->owner[3],pEntry->owner[4],pEntry->owner[5],
                            pEntry->sta[0],pEntry->sta[1],pEntry->sta[2],pEntry->sta[3],pEntry->sta[4],pEntry->sta[5]);

            }    
        }
    }
                        
    return 0;
}

#if defined(CONFIG_RTL_MESH_CROSSBAND)
/*
    action: 0 : refresh aging time only
                1: create proxy info if not exist and refresh aging time
                2: delete proxy info

*/
int sync_proxy_info(DRV_PRIV *priv,unsigned char *sta, unsigned char action)
{
    struct proxy_table_entry Entry, *pEntry;

    if(GET_MIB(priv)->dot1180211sInfo.mesh_enable == 0) {
        return -1;
    }
    
    pEntry = HASH_SEARCH(priv->proxy_table, sta);

    if(action == 2) { //delete        
        if(pEntry) {           
            mesh_proxy_debug("[Sync]Remove Proxy table: %02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x\n",
                            pEntry->owner[0],pEntry->owner[1],pEntry->owner[2],pEntry->owner[3],pEntry->owner[4],pEntry->owner[5],
                            pEntry->sta[0],pEntry->sta[1],pEntry->sta[2],pEntry->sta[3],pEntry->sta[4],pEntry->sta[5]);
            HASH_DELETE(priv->proxy_table, sta);

            #if defined(RTL_MESH_TXCACHE)
    		priv->mesh_txcache.dirty = 0;
            #endif
        }

    }
    else if(action == 0) { //refresh aging time only
        if(pEntry) {
            pEntry->aging_time = 0;
            mesh_proxy_debug("[Sync]Refresh Proxy table: %02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x\n",
                            pEntry->owner[0],pEntry->owner[1],pEntry->owner[2],pEntry->owner[3],pEntry->owner[4],pEntry->owner[5],
                            pEntry->sta[0],pEntry->sta[1],pEntry->sta[2],pEntry->sta[3],pEntry->sta[4],pEntry->sta[5]);

        }
    }
    else { // create proxy information if not exist
        if(pEntry) {

            pEntry->aging_time = -1;  /*never timeout*/         

            if(memcmp(pEntry->owner, GET_MY_HWADDR, MACADDRLEN)) {
                mesh_proxy_debug("[Sync]Update Proxy table from %02x:%02x:%02x:%02x:%02x:%02x to %02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x\n",
                                pEntry->owner[0],pEntry->owner[1],pEntry->owner[2],pEntry->owner[3],pEntry->owner[4],pEntry->owner[5],
                                priv->pmib->dot11OperationEntry.hwaddr[0], priv->pmib->dot11OperationEntry.hwaddr[1],
                                priv->pmib->dot11OperationEntry.hwaddr[2], priv->pmib->dot11OperationEntry.hwaddr[3],
                                priv->pmib->dot11OperationEntry.hwaddr[4], priv->pmib->dot11OperationEntry.hwaddr[5],                                
                                pEntry->sta[0],pEntry->sta[1],pEntry->sta[2],pEntry->sta[3],pEntry->sta[4],pEntry->sta[5]);
                memcpy(pEntry->owner, GET_MY_HWADDR, MACADDRLEN);

                #if defined(RTL_MESH_TXCACHE)
        		priv->mesh_txcache.dirty = 0;
                #endif
            }
            else {
                mesh_proxy_debug("[Sync]Refresh Proxy table: %02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x\n",
                                pEntry->owner[0],pEntry->owner[1],pEntry->owner[2],pEntry->owner[3],pEntry->owner[4],pEntry->owner[5],
                                pEntry->sta[0],pEntry->sta[1],pEntry->sta[2],pEntry->sta[3],pEntry->sta[4],pEntry->sta[5]); 

            }
                                       
        }
        else {
            memcpy(Entry.sta, sta, MACADDRLEN);
	        memcpy(Entry.owner, GET_MY_HWADDR, MACADDRLEN);
		    Entry.aging_time = -1; /*never timeout*/
		    HASH_INSERT(priv->proxy_table, Entry.sta, &Entry);
            mesh_proxy_debug("[Sync]Insert Proxy table: %02x:%02x:%02x:%02x:%02x:%02x/%02x:%02x:%02x:%02x:%02x:%02x\n",
                            Entry.owner[0],Entry.owner[1],Entry.owner[2],Entry.owner[3],Entry.owner[4],Entry.owner[5],
                            Entry.sta[0],Entry.sta[1],Entry.sta[2],Entry.sta[3],Entry.sta[4],Entry.sta[5]);
        }        
    }

	return 0;
}
#endif

int set_metric_manually(DRV_PRIV *priv,unsigned char *str)
{
   	struct list_head *phead, *plist;
	struct stat_info *pstat;
    unsigned char mac_address[MACADDRLEN],i=0,*ptr,ret=-1;
    unsigned int metric=0;

	ptr = str;
    for(;i<MACADDRLEN;i++) {
		if(str[i*2] < ':' && str[i*2] > '/') {
			mac_address[i] = (str[i*2]-'0')<<4;
		} else if (str[i*2] > '`' && str[i*2] < 'g') {
            mac_address[i] = (0xa+(str[i*2]-'a'))<<4;
		} else {
            panic_printk("Invalid format!\n");
			goto set_metric_manually_errorout;
		}

        if(str[i*2+1] < ':' && str[i*2+1] > '/') {
			mac_address[i] |= (str[i*2+1]-'0');
		} else if (str[i*2+1] > '`' && str[i*2+1] < 'g') {
            mac_address[i] |= 0xa+(str[i*2+1]-'a');
		} else {
            panic_printk("Invalid format!\n");
			goto set_metric_manually_errorout;
		}
	}

	if(strstr(str,"-"))
        ptr = strstr(str,"-")+1;
    else {
        panic_printk("Invalid format!\n");
		goto set_metric_manually_errorout;
	}
	sscanf(ptr,"%d",&metric);

    panic_printk("%s %d %02x:%02x:%02x:%02x:%02x:%02x %d\n",__func__,__LINE__,
        mac_address[0],mac_address[1],mac_address[2],mac_address[3],mac_address[4],mac_address[5],metric);
	
	if (1 == GET_MIB(priv)->dot1180211sInfo.mesh_enable) {
		phead = &priv->mesh_mp_hdr;
		if (!netif_running(priv->dev) || list_empty(phead))
			goto set_metric_manually_errorout;

		plist = phead->next;
		while (plist != phead) {
            pstat = list_entry(plist, struct stat_info, mesh_mp_ptr);

			if(!memcmp(pstat->hwaddr,mac_address,MACADDRLEN)) {
				pstat->mesh_neighbor_TBL.manual_metric = metric;
                plist = phead;
                ret = 0;
                break;
			} else
				plist = plist->next;
		}
	} else {
		panic_printk("  Mesh mode DISABLE !!\n");
	}

set_metric_manually_errorout:
	return ret;
}

#endif	// CONFIG_RTK_MESH
