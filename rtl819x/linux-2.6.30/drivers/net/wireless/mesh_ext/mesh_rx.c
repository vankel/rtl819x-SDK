/*
 *      Handling routines for Mesh in 802.11 RX
 * 
 *      PS: All extern function in ../8190n_headers.h
 */
#define _MESH_RX_C_

#ifdef CONFIG_RTL8192CD
#include "../rtl8192cd/8192cd.h"
#include "../rtl8192cd/8192cd_headers.h"
#else
#include "../rtl8190/8190n.h"
#include "../rtl8190/8190n_headers.h"
#endif
#include "./mesh.h"
#include "./mesh_route.h"

#ifdef CONFIG_RTK_MESH

/*
	Check the "to_fr_ds" field:

						FromDS =1
						ToDS =1
	NOTE: The functuion duplicate from rtl8190_rx_dispatch_wds (8190n_rx.c)
*/
int rx_dispatch_mesh(DRV_PRIV *priv, struct rx_frinfo *pfrinfo)
{
	int reuse = 1;
	unsigned char *pframe = get_pframe(pfrinfo);
	unsigned int frtype = GetFrameType(pframe);
	unsigned short frame_type = GetFrameSubType(pframe);
	
	struct net_device *dev = priv->mesh_dev;
	unsigned char	*da = pfrinfo->da;
	int fragnum;
  
	if (memcmp(GET_MY_HWADDR, GetAddr1Ptr(pframe), MACADDRLEN)
		&& !IS_MCAST(GetAddr1Ptr(pframe))	)
	{
		DEBUG_INFO("Rx a 11s packet but which addr1 is not matched own, drop it!\n");
		goto out;
	}

	if (!netif_running(dev)) {
		DEBUG_ERR("Rx a 11s packet but which interface is not up, drop it!\n");
		goto out;
	}
	
	fragnum = GetFragNum(pframe);
	if (GetMFrag(pframe) || fragnum) {
		DEBUG_ERR("Rx a fragment 11s packet, drop it!\n");
		goto out;
	}

	if ( (frtype != WIFI_DATA_TYPE) && (frtype != WIFI_EXT_TYPE)) {
		DEBUG_ERR("Rx a Mesh packet but which type is not valid, drop it!\n");
		goto out;
	}
	
	// Unsupported (8186): 802.11s action frame, data frame --> type = 11 (i.e., WIFI_EXT_TYPE)
	// Hence, we had to use WIFI_DATA_TYPE in 8186, 8187B and now 8190.
	if (frtype == WIFI_DATA_TYPE) {	
		struct stat_info	*pstat;
        #ifndef SMP_SYNC
		unsigned long		flags;
        #endif               
		SAVE_INT_AND_CLI(flags);
		pstat = get_stainfo(priv, GetAddr2Ptr(pframe));
		
		// action which has lowest state is "Link state announcement"
		if((pstat == NULL) 
				|| !((pstat->mesh_neighbor_TBL.State == MP_SUPERORDINATE_LINK_UP) || (pstat->mesh_neighbor_TBL.State == MP_SUBORDINATE_LINK_UP)
				|| (pstat->mesh_neighbor_TBL.State == MP_SUBORDINATE_LINK_DOWN_E))) // Recived Local Link State Announcement from  DOWN to UP
		{
		
			RESTORE_INT(flags);
			if( !IS_MCAST(da) && frame_type == WIFI_11S_MESH && pstat == NULL)
				issue_deauth_MP(priv, (void *)GetAddr2Ptr(pframe), _RSON_UNSPECIFIED_, 1);
//
			DEBUG_ERR("Rx a Mesh packet but there does not have a corresponding station info\n");
			goto out;
		}
		RESTORE_INT(flags);
		pfrinfo->is_11s = 1;
		
#ifndef MESH_BOOTSEQ_STRESS_TEST	// Stop update expire when stress test.
		// chris: update neighbor timer
		pstat->mesh_neighbor_TBL.expire = jiffies + MESH_EXPIRE_TO;
#endif
	} 
					
	// if( frame_type == WIFI_ACTION ) { // In D1.06 next line replace by this line (And remove SetFrameSubType(pframe, WIFI_ACTION);) by popen
	if( frame_type == WIFI_11S_MESH_ACTION ) 
	{
		int TTL;
		unsigned short seq;
		//struct path_sel_entry *pPathselEntry;
						
		seq = *((unsigned short*)(pframe+32));
		TTL = *GetMeshHeaderTTLWithOutQOS(pframe);
					
		if(TTL == 0)	// filter packets due to duplicate
			goto out;
						
		*GetMeshHeaderTTLWithOutQOS(pframe) = TTL-1;				
		SetFrameSubType(pframe, WIFI_WMM_ACTION); // it's easier for mgt_handler

		// IF Draft modify Action frame to Date frame, Remove line between here to goto out ?
#ifdef RTL8190_DIRECT_RX
		DRV_RX_MGNTFRAME(priv, NULL, pfrinfo);
#else
		list_add_tail(&(pfrinfo->rx_list), &(priv->rx_mgtlist));
#endif
		reuse = 0;
		goto	out;	// Success finish, reuse = 0 (Because not Date frame) (Remove to here)
	}

	else if( frame_type == WIFI_11S_MESH ) 
	{
	
		memcpy(pfrinfo->prehop_11s, GetAddr2Ptr(pframe), MACADDRLEN);
	} 
	else { 
		DEBUG_ERR("Rx a 11s packet but which type is not DATA, drop it!\n");
		goto out;
	} 

#ifdef RTL8190_DIRECT_RX
	DRV_RX_DATA(priv, NULL, pfrinfo);
#else
	list_add_tail(&pfrinfo->rx_list, &priv->rx_datalist);
#endif
	reuse = 0;
out:
	return reuse;
}



#ifdef MESH_AMSDU
static void process_11s_amsdu(struct rtl8190_priv *priv, struct stat_info *pstat, struct rx_frinfo *pfrinfo)
{
    unsigned char	*pframe;
    struct sk_buff	*pskb  = NULL, *pnewskb = NULL;
    unsigned char	*next_head;
    int 			rest, agg_pkt_num=0, i, privacy;
    unsigned int	subfr_len, padding;
    unsigned const char rfc1042_ip_header[8]={0xaa,0xaa,0x03,00,00,00,0x08,0x00};
#ifdef SMP_SYNC
    unsigned long flags;
#endif

    pframe = get_pframe(pfrinfo);
    pskb = get_pskb(pfrinfo);

    rest = pfrinfo->pktlen - pfrinfo->hdr_len;
    next_head = pframe + pfrinfo->hdr_len;

    if (GetPrivacy(pframe)) {
        privacy = get_sta_encrypt_algthm(priv, pstat);
        if ((privacy == _CCMP_PRIVACY_) || (privacy == _TKIP_PRIVACY_)) {
            rest -= 8;
            next_head += 8;
        }
        else {	// WEP
            rest -= 4;
            next_head += 4;
        }
    }
    while (rest > WLAN_ETHHDR_LEN) {
        pnewskb = skb_clone(pskb, GFP_ATOMIC);
        if (pnewskb) {
            pnewskb->data = next_head;		
            subfr_len = (*(next_head + MACADDRLEN*2) << 8) + (*(next_head + MACADDRLEN*2 + 1));
            pnewskb->len = WLAN_ETHHDR_LEN + subfr_len;
            pnewskb->tail = pnewskb->data + pnewskb->len;

            if (!memcmp(rfc1042_ip_header, pnewskb->data+WLAN_ETHHDR_LEN, 8)) {
                for (i=0; i<MACADDRLEN*2; i++)
                    pnewskb->data[19-i] = pnewskb->data[11-i];
                pnewskb->data += 8;
                pnewskb->len -= 8;
            }
            else
                strip_amsdu_llc(priv, pnewskb, pstat);
            agg_pkt_num++;


            {
                struct rx_frinfo newfrinfo;
                struct	MESH_HDR* mhdr = (struct MESH_HDR*) (pnewskb->data+sizeof(struct wlan_ethhdr_t));
                memset(&newfrinfo, 0, sizeof(newfrinfo));
                newfrinfo.is_11s = 8;
                newfrinfo.to_fr_ds = 3;
                newfrinfo.pktlen = pnewskb->len;
                newfrinfo.da = (pnewskb->data);
                newfrinfo.sa = (pnewskb->data+MACADDRLEN);
                newfrinfo.pskb = pnewskb;
                if( 1 >= mhdr->TTL--)
                    rtl_kfree_skb(priv, pnewskb, _SKB_RX_);
                else
                {
                    struct path_sel_entry *pPathselEntry;		
                    SMP_LOCK_MESH_PATH(flags);
                    pPathselEntry = HASH_SEARCH(priv->pathsel_table, newfrinfo.sa);                                         
                    if (pPathselEntry) {
                        if(IS_MCAST(newfrinfo.da) && !chkMeshSeq(pPathselEntry, newfrinfo.sa, mhdr->segNum)) {
                            SMP_UNLOCK_MESH_PATH(flags);
                            rtl_kfree_skb(priv, pnewskb, _SKB_RX_);
                        }
                        else {
                            pPathselEntry->update_time = jiffies; 
                            SMP_UNLOCK_MESH_PATH(flags);

                            memcpy(&(newfrinfo.mesh_header), mhdr, sizeof(struct MESH_HDR));
                            if (process_11s_datafrme(priv, &newfrinfo, pstat) == FAIL) 
                                rtl_kfree_skb(priv, pnewskb, _SKB_RX_);

                        }                    
                    }    
                    else {
                        SMP_UNLOCK_MESH_PATH(flags);
                    }

                }				
            }

            padding = 4 - ((WLAN_ETHHDR_LEN + subfr_len) % 4);
            if (padding == 4)
                padding = 0;
            rest -= (WLAN_ETHHDR_LEN + subfr_len + padding);
            next_head += (WLAN_ETHHDR_LEN + subfr_len + padding);
        }
        else {
            // Can't get new skb header, drop this packet
            break;
        }
    }


    // clear saved shortcut data
#if defined(RX_SHORTCUT) && defined(RX_RL_SHORTCUT)
    if (pstat->rx_payload_offset)
        pstat->rx_payload_offset = 0;
#endif


    rtl_kfree_skb(priv, pskb, _SKB_RX_);
}
#endif


#ifdef RX_RL_SHORTCUT
int mesh_shortcut_update(DRV_PRIV *priv, struct rx_frinfo *pfrinfo, 
             struct stat_info *pstat, int idx, struct path_sel_entry *pEntry, unsigned char *to_mesh, struct MESH_HDR **meshHdrPt)
{
#ifdef SMP_SYNC
    unsigned long flags;
#endif
    unsigned char *pframe = get_pframe(pfrinfo);
    struct path_sel_entry *pPathselEntry;
    struct proxy_table_entry *pProxyEntry;
    struct MESH_HDR * mesh_header;            
    DRV_PRIV * orig_priv = priv;

    *to_mesh = 0;
    if (memcmp(GetAddr4Ptr(pframe), pstat->rx_sc_ent[idx].rx_wlanhdr.addr4, MACADDRLEN)){
        return -1;
    }

    priv = (DRV_PRIV *)priv->mesh_priv_first;
    if (memcmp(GetAddr3Ptr(pframe), GET_MY_HWADDR, MACADDRLEN)) {

        SMP_LOCK_MESH_PATH(flags);
        pPathselEntry = HASH_SEARCH(priv->pathsel_table, GetAddr3Ptr(pframe));
        if (pPathselEntry) {	            
            memcpy(pEntry, pPathselEntry, sizeof(struct path_sel_entry));
            *to_mesh = 1;
        }
        SMP_UNLOCK_MESH_PATH(flags);
        
        if(pPathselEntry == NULL)
            return -1;	
    }

    mesh_header = *meshHdrPt = (struct MESH_HDR *)getMeshHeader(orig_priv, orig_priv->pmib->dot11sKeysTable.dot11Privacy, pframe); 
    if( mesh_header ) 
    {
        if(1 > mesh_header->TTL || mesh_header->mesh_flag != 0x01) 
            return -1;

        if (memcmp(mesh_header->DestMACAddr, pstat->rx_sc_ent[idx].rx_wlanhdr.meshhdr.DestMACAddr, 6) ||
            memcmp(mesh_header->SrcMACAddr, pstat->rx_sc_ent[idx].rx_wlanhdr.meshhdr.SrcMACAddr, 6)) {
            return -1;
        }

    } 
    else {
        return -1;
    }
	
	
    //6 addresss format, update pathsel and proxy timer 
    SMP_LOCK_MESH_PATH(flags);
    pPathselEntry = HASH_SEARCH(priv->pathsel_table, pfrinfo->sa); 
    if (pPathselEntry != NULL)
        pPathselEntry->update_time = jiffies;
    SMP_UNLOCK_MESH_PATH(flags);

    SMP_LOCK_MESH_PROXY(flags);
    pProxyEntry = (struct proxy_table_entry*) HASH_SEARCH(priv->proxy_table, mesh_header->SrcMACAddr);
    if (pProxyEntry != NULL)
        pProxyEntry->aging_time = 0;

    pProxyEntry = (struct proxy_table_entry*) HASH_SEARCH(priv->proxy_table, mesh_header->DestMACAddr); 
    if (pProxyEntry != NULL && pProxyEntry->aging_time >= 0) 
        pProxyEntry->aging_time = 0;
    SMP_UNLOCK_MESH_PROXY(flags);


    return 0;
}

#endif

// Gakki
/*
	pfrinfo->is_11s =1  => 802.11 header
	pfrinfo->is_11s =8  => 802.3  header + mesh header
*/
int process_11s_datafrme(DRV_PRIV *priv, struct rx_frinfo *pfrinfo, struct stat_info *pstat)
{
#ifdef SMP_SYNC
    unsigned long flags;
#endif
    unsigned char 	*pframe;
    unsigned int  	privacy, res;
    struct sk_buff	 *pskb  = NULL;
    struct stat_info *dapstat = NULL;
    unsigned char *addr5, *addr6;
    int ret;
    DRV_PRIV * orig_priv = priv;
            
    pframe = get_pframe(pfrinfo);
    pskb = get_pskb(pfrinfo);  

    priv = (DRV_PRIV *)priv->mesh_priv_first;

    // Process A-MSDU
    if (pfrinfo->is_11s&1)	{
        if((*GetQosControl(pframe))& BIT(7)) {
#ifdef MESH_AMSDU
            process_11s_amsdu(orig_priv, pstat, pfrinfo);
            return SUCCESS;
#else
            return FAIL;
#endif
        } else 	{
            struct path_sel_entry *pPathselEntry;
            struct MESH_HDR *meshHdrPtr; 			
            meshHdrPtr = (struct MESH_HDR *)getMeshHeader(orig_priv, orig_priv->pmib->dot11sKeysTable.dot11Privacy, pframe); 	
            if( meshHdrPtr ) 
            {
                /*strictly check mesh header to  filter packets due to duplicate, 
                                or due to 92C,92D, 88C HW bug which may recieved garbage packets when using CCMP encryption*/
                if(meshHdrPtr->TTL < 1 || _MESH_HEADER_TTL_ < meshHdrPtr->TTL || meshHdrPtr->mesh_flag != 0x01 || meshHdrPtr->SrcMACAddr[0] & 0x01)
                    return FAIL;
                meshHdrPtr->TTL--;
            }		
            else {
                return FAIL;
            }

            SMP_LOCK_MESH_PATH(flags);

            pPathselEntry = HASH_SEARCH(priv->pathsel_table, pfrinfo->sa);
            if(pPathselEntry) {
            
                // Usually, IS_MCAST(A3) = IS_MCAST(A1) for a multicast frame
                // However, when using unicast to simulate multicast, A1 is unicast but A3 is not 
                // On the other hand, if a maliciuos host issues multicast for A1 but unicast for A3, a flooding would happen if not chcking A1
                if(IS_MCAST(pfrinfo->da) ){ // filter packets due to duplicate                                       
                    if(!chkMeshSeq(pPathselEntry, pfrinfo->sa, meshHdrPtr->segNum)) {  
                        SMP_UNLOCK_MESH_PATH(flags);
                        return FAIL;
                    }            
                }     
                pPathselEntry->update_time = jiffies;
            }
            else {/*drop loop multicast packet*/
                SMP_UNLOCK_MESH_PATH(flags);
                return FAIL;
            }
            SMP_UNLOCK_MESH_PATH(flags);
		
            memcpy(&(pfrinfo->mesh_header), meshHdrPtr, sizeof(struct lls_mesh_header) );
		
        }
    }
    else {
        return FAIL;
    }

    
    /*update proxy table*/
    addr5 = pfrinfo->mesh_header.DestMACAddr;
    addr6 = pfrinfo->mesh_header.SrcMACAddr;

    //if frame is 6 addresss format, update proxy table 
    if(memcmp(pfrinfo->sa, addr6, MACADDRLEN)) { // addr4 <> addr 6, save it to proxy table
        mesh_proxy_update(priv, pfrinfo->sa, addr6);
    }

    privacy = get_sta_encrypt_algthm(orig_priv, pstat);
    if(IS_MCAST(pfrinfo->da))
    {  
        if (!orig_priv->pmib->dot11OperationEntry.block_relay)
        {
            ret = relay_11s_dataframe(priv,pskb, /*pskb->dev,*/ privacy, pfrinfo);
            if(ret < 0) {
                return FAIL;
            }
            else if(ret == 1) {
                goto receive_packet; 
            }
            else {
                goto to_bridge;
            }            
        }
        goto receive_packet;       
    } 
    else			
    {

        if( memcmp(pfrinfo->da, GET_MY_HWADDR, MACADDRLEN)== 0)
        {     
            // filter packets that SA is myself 
            if ( memcmp(pfrinfo->sa, GET_MY_HWADDR, MACADDRLEN) == 0)
                return FAIL;

#ifdef PU_STANDARD_RX
            if( Is_6AddrFormat && memcmp(GET_MY_HWADDR, addr5, MACADDRLEN) )
            {
                struct proxyupdate_table_entry Entry;
                struct proxy_table_entry *pEntry=NULL;
                struct path_sel_entry *pdstEntry=NULL;

                //check if addr5 is my associating STA
                if(get_stainfo(priv, addr5)!=NULL)
                    goto receive_packet;

                // check if addr5 is any other AP's STA
                SMP_LOCK_MESH_PROXY(flags);
                pEntry = HASH_SEARCH(priv->proxy_table, addr5);
                if( pEntry )
                {
                    pEntry->aging_time = 0;
                    // case: my bridged PC
                    //     da=me, STA = NULL, proxy != NULL 
                    if(memcmp(GET_MY_HWADDR, pEntry->owner, MACADDRLEN)==0) {
                        SMP_UNLOCK_MESH_PROXY(flags);
                        goto receive_packet;
                    }

                    // case: I know the proxied node resided in (but not behind me)
                    //     da=me, STA =NULL, proxy->owner=other node -> proxy update:add
                    else 
                    {
                        Entry.PUflag = PU_add;
                        memcpy((void *)Entry.proxymac, pEntry->owner, MACADDRLEN);
                    }
                }
                else					
                {
                    // case: unknown entry
                    //  da=me, STA=NULL, proxy = NULL
                    //  it might be an un-recorded bridged PC happens for pEntry = NULL
                    Entry.PUflag = PU_delete;
                    memcpy((void *)Entry.proxymac, GET_MY_HWADDR, MACADDRLEN);
                }                    
                SMP_UNLOCK_MESH_PROXY(flags);

                SMP_LOCK_MESH_PATH(flags);
                pdstEntry = HASH_SEARCH(priv->pathsel_table, pfrinfo->sa);
                if( pdstEntry){
                    Entry.isMultihop = pdstEntry->hopcount ;
                    memcpy(Entry.nexthopmac ,pdstEntry->nexthopMAC ,MACADDRLEN);
                    SMP_UNLOCK_MESH_PATH(flags);
                }
                else{
                    SMP_UNLOCK_MESH_PATH(flags);
                    memset((void *)Entry.nexthopmac ,0xff ,MACADDRLEN);
                    Entry.isMultihop =0;
                }
                memcpy(Entry.destproxymac ,pfrinfo->sa ,MACADDRLEN);
                memcpy((void *)Entry.proxiedmac, addr5 ,MACADDRLEN);								
                Entry.retry = 1U;
                Entry.PUSN = getPUSeq(priv);
                Entry.STAcount = 0x0001;
                Entry.update_time = jiffies;
                issue_proxyupdate_MP(priv,&Entry);
            }

#endif // PU_STANDARD_RX

            goto receive_packet;
							
        } // if( !memcmp(pfrinfo->da, GET_MY_HWADDR, MACADDRLEN) )
        else if (orig_priv->pmib->dot11OperationEntry.block_relay == 0)
        { 
            // dest is not me & relay is allow
            dapstat = get_stainfo(priv, pfrinfo->da);
            if(dapstat != NULL && isSTA(dapstat))
            {
                // filter packets that SA is myself 
                if ( memcmp(pfrinfo->sa, GET_MY_HWADDR, MACADDRLEN) == 0)
                    return FAIL;
                else
                {
receive_packet:
                    res = skb_p80211_to_ether(pskb->dev, privacy, pfrinfo);
                    if (res == FAIL) {
                        orig_priv->ext_stats.rx_data_drops++;
                        DEBUG_ERR("RX DROP: skb_p80211_to_ether fail!\n");
                        return FAIL;
                    }

to_bridge:
                    pskb->dev = priv->mesh_dev;
                    if ((orig_priv->pmib->dot11BssType.net_work_type & WIRELESS_11N) &&
                        orig_priv->pmib->reorderCtrlEntry.ReorderCtrlEnable) {
                        *(unsigned int *)&(pfrinfo->pskb->cb[4]) = 0;
                        if (reorder_ctrl_check(orig_priv, pstat, pfrinfo)) {
                            rtl_netif_rx(orig_priv, pskb, pstat);
                        }
                    }
                    else
                        rtl_netif_rx(orig_priv, pskb, pstat);

                    return SUCCESS;					
                }
            } 

            if (relay_11s_dataframe(priv, pskb, privacy, pfrinfo))
                rtl_kfree_skb(orig_priv, pskb, _SKB_RX_);	
        } 
		return SUCCESS;
    }   
}

#endif	// CONFIG_RTK_MESH
