/*
 *  TX handle routines
 *
 *  $Id: 8192cd_tx.c,v 1.39.2.31 2011/01/19 15:20:34 victoryman Exp $
 *
 *  Copyright (c) 2009 Realtek Semiconductor Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#define _8192CD_TX_C_

#ifdef __KERNEL__
#include <linux/if_arp.h>
#include <linux/module.h>
#include <linux/netdevice.h>
#elif defined(__ECOS)
#include <cyg/io/eth/rltk/819x/wrapper/sys_support.h>
#include <cyg/io/eth/rltk/819x/wrapper/skbuff.h>
#include <cyg/io/eth/rltk/819x/wrapper/timer.h>
#include <cyg/io/eth/rltk/819x/wrapper/wrapper.h>
#endif

#ifdef __DRAYTEK_OS__
#include <draytek/wl_dev.h>
#endif

#include "./8192cd_cfg.h"
#include "./8192cd.h"
#include "./8192cd_hw.h"
#include "./8192cd_headers.h"
#include "./8192cd_debug.h"

#if !defined(__KERNEL__) && !defined(__ECOS)
#include "./sys-support.h"
#endif

#ifdef RTL8192CD_VARIABLE_USED_DMEM
#include "./8192cd_dmem.h"
#endif

#if defined(CONFIG_RTL_WAPI_SUPPORT)
#include "wapiCrypto.h"
#endif
#ifdef CONFIG_RTL_VLAN_8021Q
#include <linux/if_vlan.h>
#endif

#ifdef CONFIG_RTL867X_VLAN_MAPPING
#include "../../re_vlan.h"
#endif

#ifdef PERF_DUMP
#include "romeperf.h"
#endif

struct rtl_arphdr
{
	//for corss platform
    __be16          ar_hrd;         /* format of hardware address   */
    __be16          ar_pro;         /* format of protocol address   */
    unsigned char   ar_hln;         /* length of hardware address   */
    unsigned char   ar_pln;         /* length of protocol address   */
    __be16          ar_op;          /* ARP opcode (command)         */
};

#define AMSDU_TX_DESC_TH		2	// A-MSDU tx desc threshold, A-MSDU will be
									// triggered when more than this threshold packet in hw queue

#define RET_AGGRE_BYPASS		0
#define RET_AGGRE_ENQUE			1
#define RET_AGGRE_DESC_FULL		2

#define TX_NORMAL				0
#define TX_NO_MUL2UNI			1
#define TX_AMPDU_BUFFER_SIG		2
#define TX_AMPDU_BUFFER_FIRST	3
#define TX_AMPDU_BUFFER_MID		4
#define TX_AMPDU_BUFFER_LAST	5

#if 0//def CONFIG_RTL_STP
extern unsigned char STPmac[6];
#endif

#ifdef CONFIG_RTL_VLAN_8021Q
extern int linux_vlan_enable;
extern linux_vlan_ctl_t *vlan_ctl_p;
#endif

#ifdef RTL_MANUAL_EDCA
unsigned int PRI_TO_QNUM(struct rtl8192cd_priv *priv, int priority)
{
	if (priv->pmib->dot11QosEntry.ManualEDCA) {
		return priv->pmib->dot11QosEntry.TID_mapping[priority];
	}
	else {
		if ((priority == 0) || (priority == 3)) {
			if (!((OPMODE & WIFI_STATION_STATE) && GET_STA_AC_BE_PARA.ACM))
				return BE_QUEUE;
			else
				return BK_QUEUE;
		} else if ((priority == 7) || (priority == 6)) {
			if (!((OPMODE & WIFI_STATION_STATE) && GET_STA_AC_VO_PARA.ACM)) {
				return VO_QUEUE;
			} else {
				if (!GET_STA_AC_VI_PARA.ACM)
					return VI_QUEUE;
				else if (!GET_STA_AC_BE_PARA.ACM)
					return BE_QUEUE;
				else
					return BK_QUEUE;
			}
		} else if ((priority == 5) || (priority == 4)) {
			if (!((OPMODE & WIFI_STATION_STATE) && GET_STA_AC_VI_PARA.ACM)) {
				return VI_QUEUE;
			} else {
				if (!GET_STA_AC_BE_PARA.ACM)
					return BE_QUEUE;
				else
					return BK_QUEUE;
			}
		} else {
			return BK_QUEUE;
		}
	}
}
#endif


#if !defined(CONFIG_RTL_8676HWNAT) && defined(CONFIG_RTL8672) && !defined(CONFIG_RTL8686) && !defined(CONFIG_RTL8685)
extern int check_IGMP_report(struct sk_buff *skb);
extern int check_wlan_mcast_tx(struct sk_buff *skb);
#endif

#define BG_TABLE_SIZE 21
#define MCS_40M_TABLE_SIZE 18
#define MCS_20M_TABLE_SIZE 22

unsigned short BG_TABLE[2][BG_TABLE_SIZE] = {{73,68,63,57,52,47,42,37,31,30,27,25,23,22,20,18,16,14,11,9,8},
				{108,108,108,108,108,108,108,108,108,108,108,108,72,72,48,48,36,12,11,11,4}};
unsigned short MCS_40M_TABLE[2][MCS_40M_TABLE_SIZE] = {{73,68,63,57,53,45,39,37,31,29,28,26,25,23,21,19,17,12},
					{7,7,7,7,7,7,7,7,5,4,4,4,2,2,2,0,0,0}};
unsigned short MCS_20M_TABLE[2][MCS_20M_TABLE_SIZE]={{73,68,65,60,55,50,45,40,31,32,27,26,24,22,21,19,18,16,14,11,9,8},
					{7,7,7,7,7,7,7,7,7,6,5,5,5,4,3,3,3,2,3,1,0,0}};

static int tkip_mic_padding(struct rtl8192cd_priv *priv,
			unsigned char *da, unsigned char *sa, unsigned char priority,
			unsigned char *llc,struct sk_buff *pskb, struct tx_insn* txcfg);

#ifdef CONFIG_PCI_HCI
static int rtl8192cd_tx_queueDsr(struct rtl8192cd_priv *priv, unsigned int txRingIdx);
static void rtl8192cd_tx_restartQueue(struct rtl8192cd_priv *priv);
#endif

__MIPS16
__IRAM_IN_865X
int __rtl8192cd_start_xmit(struct sk_buff*skb, struct net_device *dev, int tx_fg);

#ifdef TX_SCATTER
extern struct sk_buff *copy_skb(struct sk_buff *skb);
#endif



unsigned int get_tx_rate(struct rtl8192cd_priv *priv, struct stat_info *pstat)
{
    if (pstat->sta_in_firmware == 1) {
#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_92C_SUPPORT)
		if (CHIP_VER_92X_SERIES(priv) && (priv->pmib->dot11RFEntry.txbf == 1))
			check_txrate_by_reg(priv, pstat);
#endif		
		return pstat->current_tx_rate;
	} else {
			// firmware does not keep the aid ...
			//use default rate instead
			// eric_8814 ?? VHT rates ?? 1SS ?? 2SS ?? 3SS ??
			if (pstat->ht_cap_len) {	// is N client
				if (pstat->tx_bw == HT_CHANNEL_WIDTH_20_40) {//(pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SUPPORT_CH_WDTH_))){ //40Mhz
					int i = 0;
					pstat->current_tx_rate = MCS_40M_TABLE[1][MCS_40M_TABLE_SIZE-1] + HT_RATE_ID;
					for (i=1; i < MCS_40M_TABLE_SIZE; i++) {
						if (pstat->rssi > MCS_40M_TABLE[0][i]){
							pstat->current_tx_rate = MCS_40M_TABLE[1][i-1] + HT_RATE_ID;
							break;
						}
					}
					return pstat->current_tx_rate;
				} else { // 20Mhz
					int i = 0;
					pstat->current_tx_rate = MCS_20M_TABLE[1][MCS_20M_TABLE_SIZE-1] + HT_RATE_ID;
					for (i=1; i < MCS_20M_TABLE_SIZE; i++) {
						if (pstat->rssi > MCS_20M_TABLE[0][i]){
							pstat->current_tx_rate = MCS_20M_TABLE[1][i-1] + HT_RATE_ID;
							break;
						}
					}
					return pstat->current_tx_rate;
				}
				return pstat->current_tx_rate;

			} 
#ifdef RTK_AC_SUPPORT
			else if (pstat->vht_cap_len) {	// is N client
				if (pstat->tx_bw == HT_CHANNEL_WIDTH_80) {//(pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SUPPORT_CH_WDTH_))){ //40Mhz
					int i = 0;
					pstat->current_tx_rate = MCS_40M_TABLE[1][MCS_40M_TABLE_SIZE-1] + VHT_RATE_ID;
					for (i=1; i < MCS_40M_TABLE_SIZE; i++) {
						if (pstat->rssi > MCS_40M_TABLE[0][i]){
							pstat->current_tx_rate = MCS_40M_TABLE[1][i-1] + VHT_RATE_ID;
							break;
						}
					}
					return pstat->current_tx_rate;
				} else { // 20Mhz
					int i = 0;
					pstat->current_tx_rate = MCS_20M_TABLE[1][MCS_20M_TABLE_SIZE-1] + VHT_RATE_ID;
					for (i=1; i < MCS_20M_TABLE_SIZE; i++) {
						if (pstat->rssi > MCS_20M_TABLE[0][i]){
							pstat->current_tx_rate = MCS_20M_TABLE[1][i-1] + VHT_RATE_ID;
							break;
						}
					}
					return pstat->current_tx_rate;
				}
				return pstat->current_tx_rate;

			} 
#endif
			else { // is BG client
				int i = 0;
				pstat->current_tx_rate = BG_TABLE[1][BG_TABLE_SIZE-1]; // eric_8814 ?? conflict with HT RATES ??
				for (i = 0; i < BG_TABLE_SIZE; i++) {
					if (pstat->rssi > BG_TABLE[0][i]){
						pstat->current_tx_rate = BG_TABLE[1][i-1];
						break;
				    }
				}
				return pstat->current_tx_rate;
			}

	}

}


unsigned int get_lowest_tx_rate(struct rtl8192cd_priv *priv, struct stat_info *pstat,
				unsigned int tx_rate)
{
	unsigned int lowest_tx_rate;

	if (is_auto_rate(priv , pstat))
	{
			lowest_tx_rate = find_rate(priv, pstat, 0, 0);
	}
	else
		lowest_tx_rate = tx_rate;

	return lowest_tx_rate;
}


__MIPS16
__IRAM_IN_865X
void assign_wlanseq(struct rtl8192cd_hw *phw, unsigned char *pframe, struct stat_info *pstat, struct wifi_mib *pmib
#ifdef CONFIG_RTK_MESH	// For broadcast data frame via mesh (ex:ARP requst)
	, unsigned char is_11s
#endif
	)
{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	u16 seq;
#endif

#ifdef WIFI_WMM
	unsigned char qosControl[2];
	int tid;

	if (is_qos_data(pframe)) {
		memcpy(qosControl, GetQosControl(pframe), 2);
		tid = qosControl[0] & 0x07;

		if (pstat) {
			SetSeqNum(pframe, pstat->AC_seq[tid]);
			pstat->AC_seq[tid] = (pstat->AC_seq[tid] + 1) & 0xfff;
		}
		else {
//			SetSeqNum(pframe, phw->AC_seq[tid]);
//			phw->AC_seq[tid] = (phw->AC_seq[tid] + 1) & 0xfff;
#ifdef CONFIG_RTK_MESH	// For broadcast data frame via mesh (ex:ARP requst)
			if (is_11s)
			{
				SetSeqNum(pframe, phw->seq);
				phw->seq = (phw->seq + 1) & 0xfff;
			}
			else
#endif
				printk("Invalid seq num setting for Multicast or Broadcast pkt!!\n");
		}

		{
			if ((tid == 7) || (tid == 6))
				phw->VO_pkt_count++;
			else if ((tid == 5) || (tid == 4))
				phw->VI_pkt_count++;
			else if ((tid == 2) || (tid == 1))
				phw->BK_pkt_count++;
#ifdef WMM_VIBE_PRI
			else
				phw->BE_pkt_count++;
#endif
		}
	}
	else
#endif
	{
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
		seq = atomic_inc_return(&phw->seq);
		seq = seq & 0x0fff;
		SetSeqNum(pframe, seq);
#elif defined(CONFIG_PCI_HCI)
		SetSeqNum(pframe, phw->seq);
		phw->seq = (phw->seq + 1) & 0xfff;
#endif
	}
}


#ifdef CONFIG_RTK_MESH
static unsigned int get_skb_priority3(struct rtl8192cd_priv *priv, struct sk_buff *skb, int is_11s, struct stat_info *pstat)
#else
__MIPS16 __IRAM_IN_865X unsigned int get_skb_priority(struct rtl8192cd_priv *priv, struct sk_buff *skb, struct stat_info *pstat)
#endif
{
	unsigned int pri=0, parsing=0;
	unsigned char protocol[2];

#ifdef WIFI_WMM
    if (QOS_ENABLE) {    
        parsing = 1;
        if(pstat && !pstat->QosEnabled)
            parsing = 0;     
    }
#endif

	if (parsing) {
#if defined(CONFIG_RTK_VLAN_SUPPORT) ||defined(CONFIG_RTL_VLAN_SUPPORT) 
		if (skb->cb[0])
			pri =  skb->cb[0];
		else
#endif
		{
			protocol[0] = skb->data[12];
			protocol[1] = skb->data[13];

			if ((protocol[0] == 0x08) && (protocol[1] == 0x00))
			{
#ifdef CONFIG_RTK_MESH
				if(is_11s & RELAY_11S)
				{
					pri = (skb->data[31] & 0xe0) >> 5;
				}
				else
#endif
#ifdef HS2_SUPPORT
				if(priv->pmib->hs2Entry.QoSMap_ielen[priv->pmib->hs2Entry.curQoSMap]!=0) {
					pri=getDSCP2UP(priv, (skb->data[15] & 0xFC) >> 2);
				}
				else
#endif
				{
#if defined(CONFIG_SDIO_HCI) && defined(TX_SCATTER)
					pri = (get_skb_data_u8(skb, 15) & 0xe0) >> 5;
#else
					pri = (skb->data[15] & 0xe0) >> 5;
#endif
				}
			}
			else if ((skb->cb[0]>0) && (skb->cb[0]<8))	// Ethernet driver will parse priority and put in cb[0]
				pri = skb->cb[0];
			else
				pri = 0;
		}

#ifdef CLIENT_MODE
		if ((OPMODE & (WIFI_STATION_STATE | WIFI_ASOC_STATE)) == (WIFI_STATION_STATE | WIFI_ASOC_STATE)) {
			if (GET_STA_AC_VO_PARA.ACM) {
				if (!GET_STA_AC_VI_PARA.ACM) 
					pri = 5;
				else if (!GET_STA_AC_BE_PARA.ACM)
					pri = 0;
				else
					pri = 1;
			} else if (GET_STA_AC_VI_PARA.ACM) {
				if (!GET_STA_AC_BE_PARA.ACM)
					pri = 0;
				else
					pri = 1;
			} else if (GET_STA_AC_BE_PARA.ACM) {
				pri = 1;	// DSCP_BK tag = 1;
			}
		}
#endif	
		skb->cb[1] = pri;

		return pri;
	}
	else {
		// default is no priority
		skb->cb[1] = 0;
		return 0;
	}
}


#ifdef CONFIG_RTK_MESH
#define get_skb_priority(priv, skb, pstat)  get_skb_priority3(priv, skb, 0, pstat)
#endif

#ifdef CONFIG_PCI_HCI
static int dz_queue(struct rtl8192cd_priv *priv, struct stat_info *pstat, struct sk_buff *pskb)
{
	unsigned int ret;

	if (pstat)
	{
		if(0 == pstat->expire_to)
			return FALSE;
#if defined(WIFI_WMM) && defined(WMM_APSD)
		if ((QOS_ENABLE) && (APSD_ENABLE) && (pstat->QosEnabled) && (pstat->apsd_bitmap & 0x0f)) {
			int pri = 0;

			pri = get_skb_priority(priv, pskb, pstat);

			if (((pri == 7) || (pri == 6)) && (pstat->apsd_bitmap & 0x01)) {
				ret = enque(priv, &(pstat->VO_dz_queue->head), &(pstat->VO_dz_queue->tail),
					(unsigned long)(pstat->VO_dz_queue->pSkb), NUM_APSD_TXPKT_QUEUE, (void *)pskb);
				if (ret)
					DEBUG_INFO("enque VO pkt\n");
			}
			else if (((pri == 5) || (pri == 4)) && (pstat->apsd_bitmap & 0x02)) {
				ret = enque(priv, &(pstat->VI_dz_queue->head), &(pstat->VI_dz_queue->tail),
					(unsigned long)(pstat->VI_dz_queue->pSkb), NUM_APSD_TXPKT_QUEUE, (void *)pskb);
				if (ret)
					DEBUG_INFO("enque VI pkt\n");
			}
			else if (((pri == 0) || (pri == 3)) && (pstat->apsd_bitmap & 0x08)) {
				ret = enque(priv, &(pstat->BE_dz_queue->head), &(pstat->BE_dz_queue->tail),
					(unsigned long)(pstat->BE_dz_queue->pSkb), NUM_APSD_TXPKT_QUEUE, (void *)pskb);
				if (ret)
					DEBUG_INFO("enque BE pkt\n");
			}
			else if (pstat->apsd_bitmap & 0x04) {
				ret = enque(priv, &(pstat->BK_dz_queue->head), &(pstat->BK_dz_queue->tail),
					(unsigned long)(pstat->BK_dz_queue->pSkb), NUM_APSD_TXPKT_QUEUE, (void *)pskb);
				if (ret)
					DEBUG_INFO("enque BK pkt\n");
			}
			else
				goto legacy_ps;

			if (!pstat->apsd_pkt_buffering)
				pstat->apsd_pkt_buffering = 1;

			if (ret == FALSE) {
				DEBUG_ERR("sleep Q full for priority = %d!\n", pri);
				return CONGESTED;
			}
			return TRUE;
		}
		else
legacy_ps:
#endif
		if (pstat->dz_queue.qlen<NUM_TXPKT_QUEUE){
			skb_queue_tail(&pstat->dz_queue, pskb);
			return TRUE;
		}
	}
	else {	// Multicast or Broadcast
		ret = enque(priv, &(priv->dz_queue.head), &(priv->dz_queue.tail),
			(unsigned long)(priv->dz_queue.pSkb), NUM_TXPKT_QUEUE, (void *)pskb);
		if (ret == TRUE) {
		        if (!priv->pkt_in_dtimQ)
			        priv->pkt_in_dtimQ = 1;
		        return TRUE;
                }
	}

	return FALSE;
}
#endif // CONFIG_PCI_HCI


/*        Function to process different situations in TX flow             */
/* ====================================================================== */
#define TX_PROCEDURE_CTRL_STOP			0
#define TX_PROCEDURE_CTRL_CONTINUE		1
#define TX_PROCEDURE_CTRL_SUCCESS		2

#ifdef WDS
static int rtl8192cd_tx_wdsDevProc(struct rtl8192cd_priv *priv, struct sk_buff *skb, struct net_device **dev_p,
				struct net_device **wdsDev_p, struct tx_insn *txcfg)
{
	struct stat_info *pstat;

	txcfg->wdsIdx = getWdsIdxByDev(priv, *dev_p);
	if (txcfg->wdsIdx < 0) {
		priv->ext_stats.tx_drops++;
		DEBUG_ERR("TX DROP: getWdsIdxByDev() fail!\n");
		goto free_and_stop;
	}

	if (!netif_running(priv->dev)) {
		priv->ext_stats.tx_drops++;
		DEBUG_ERR("TX DROP: Can't send WDS packet due to wlan interface is down!\n");
		goto free_and_stop;
	}
	pstat = get_stainfo(priv, priv->pmib->dot11WdsInfo.entry[txcfg->wdsIdx].macAddr);
	if (NULL == pstat) {
		priv->ext_stats.tx_drops++;
		DEBUG_ERR("TX DROP: %s: get_stainfo() for wds failed [%d]!\n", (char *)__FUNCTION__, txcfg->wdsIdx);
		goto free_and_stop;
	}
	if (pstat->current_tx_rate==0) {
		priv->ext_stats.tx_drops++;
		DEBUG_ERR("TX DROP: Can't send packet due to tx rate is not supported in peer WDS AP!\n");
		goto free_and_stop;
	}
	*wdsDev_p = *dev_p;
	*dev_p = priv->dev;

	/* Reply caller function : Continue process */
	return TX_PROCEDURE_CTRL_CONTINUE;

free_and_stop:		/* Free current packet and stop TX process */

	rtl_kfree_skb(priv,skb,_SKB_TX_);

	/* Reply caller function : STOP process */
	return TX_PROCEDURE_CTRL_STOP;
}
#endif


#ifdef CLIENT_MODE
static int rtl8192cd_tx_clientMode(struct rtl8192cd_priv *priv, struct sk_buff **pskb)
{
	struct sk_buff *skb=*pskb;
	int DontEnterNat25=0;

	{
#ifdef RTK_BR_EXT
		int res, is_vlan_tag=0, i, do_nat25=1;
		unsigned short vlan_hdr=0;
		int lltd_flag=0;

		if (!priv->pmib->wscEntry.wsc_enable)
#ifdef MULTI_MAC_CLONE			
			if (mac_clone_handle_frame(priv, skb)) {
				/*if STAs number exceed max macclone support number then let it run nat25 path*/
                ACTIVE_ID = 0;
				//priv->ext_stats.tx_drops++;
				//DEBUG_ERR("TX DROP: exceed max clone address!\n");
				//goto free_and_stop;
			}
#else
			mac_clone_handle_frame(priv, skb);
		if(priv->pmib->ethBrExtInfo.macclone_enable && priv->macclone_completed){
			if(!memcmp(skb->data+ETH_ALEN, GET_MY_HWADDR, ETH_ALEN))	{
				DontEnterNat25=1;
			}
		}
#endif
		if ((!priv->pmib->ethBrExtInfo.nat25_disable && DontEnterNat25==0) 
#ifdef MULTI_MAC_CLONE
			&& ((ACTIVE_ID == 0) || (ACTIVE_ID > 0 && priv->pshare->mclone_sta[ACTIVE_ID-1].usedStaAddrId != 0xff))//(ACTIVE_ID == 0)//change for wifi-sta
#endif		
			)
		{
			if (*((unsigned short *)(skb->data+MACADDRLEN*2)) == __constant_htons(ETH_P_8021Q)) {
				is_vlan_tag = 1;
				vlan_hdr = *((unsigned short *)(skb->data+MACADDRLEN*2+2));
				for (i=0; i<6; i++)
					*((unsigned short *)(skb->data+MACADDRLEN*2+2-i*2)) = *((unsigned short *)(skb->data+MACADDRLEN*2-2-i*2));
				skb_pull(skb, 4);
			}

            if(priv->nat25_filter) {
                if(nat25_filter(priv, skb)== 1) {
                    priv->ext_stats.tx_drops++;
                    DEBUG_ERR("TX DROP: nat25 filter out!\n");
                    goto free_and_stop;
                }
            }


			if ((*((unsigned short *)(skb->data+MACADDRLEN*2)) == __constant_htons(ETH_P_IP)) && !IS_MCAST(skb->data)) {
				if (memcmp(priv->scdb_mac, skb->data+MACADDRLEN, MACADDRLEN)) {
					if ((priv->scdb_entry = (struct nat25_network_db_entry *)scdb_findEntry(priv,
								skb->data+MACADDRLEN, skb->data+WLAN_ETHHDR_LEN+12)) != NULL) {
						memcpy(priv->scdb_mac, skb->data+MACADDRLEN, MACADDRLEN);
						memcpy(priv->scdb_ip, skb->data+WLAN_ETHHDR_LEN+12, 4);
						priv->scdb_entry->ageing_timer = jiffies;
						do_nat25 = 0;
					}
				}
				else {
					if (priv->scdb_entry) {
						priv->scdb_entry->ageing_timer = jiffies;
						do_nat25 = 0;
					}
					else {
						memset(priv->scdb_mac, 0, MACADDRLEN);
						memset(priv->scdb_ip, 0, 4);
					}
				}
			}

			if (*((unsigned short *)(skb->data+MACADDRLEN*2)) == __constant_htons(0x88d9)) {
				if(skb->data[0] & 0x1)
				{
					do_nat25=0;
					lltd_flag=1;
				}
			}

			if (do_nat25)
			{
				if (nat25_db_handle(priv, skb, NAT25_CHECK) == 0) {
					struct sk_buff *newskb;

					if (is_vlan_tag) {
						skb_push(skb, 4);
						for (i=0; i<6; i++)
							*((unsigned short *)(skb->data+i*2)) = *((unsigned short *)(skb->data+4+i*2));
						*((unsigned short *)(skb->data+MACADDRLEN*2)) = __constant_htons(ETH_P_8021Q);
						*((unsigned short *)(skb->data+MACADDRLEN*2+2)) = vlan_hdr;
					}

					newskb = skb_copy(skb, GFP_ATOMIC);
					if (newskb == NULL) {
						priv->ext_stats.tx_drops++;
						DEBUG_ERR("TX DROP: skb_copy fail!\n");
						goto free_and_stop;
					}
					dev_kfree_skb_any(skb);
					*pskb = skb = newskb;
					if (is_vlan_tag) {
						vlan_hdr = *((unsigned short *)(skb->data+MACADDRLEN*2+2));
						for (i=0; i<6; i++)
							*((unsigned short *)(skb->data+MACADDRLEN*2+2-i*2)) = *((unsigned short *)(skb->data+MACADDRLEN*2-2-i*2));
						skb_pull(skb, 4);
					}
				}

				res = nat25_db_handle(priv, skb, NAT25_INSERT);
				if (res < 0) {
					if (res == -2) {
						priv->ext_stats.tx_drops++;
						DEBUG_ERR("TX DROP: nat25_db_handle fail!\n");
						goto free_and_stop;
					}
					// we just print warning message and let it go
					DEBUG_WARN("nat25_db_handle INSERT fail!\n");
				}
			}
#ifdef MULTI_MAC_CLONE
			if (ACTIVE_ID >= 0)
				mclone_dhcp_caddr(priv, skb);
#endif

			if(lltd_flag != 1)
			{
				memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
			}

			if (is_vlan_tag) {
				skb_push(skb, 4);
				for (i=0; i<6; i++)
					*((unsigned short *)(skb->data+i*2)) = *((unsigned short *)(skb->data+4+i*2));
				*((unsigned short *)(skb->data+MACADDRLEN*2)) = __constant_htons(ETH_P_8021Q);
				*((unsigned short *)(skb->data+MACADDRLEN*2+2)) = vlan_hdr;
			}
		}
		else{

            /*even nat25 is disabled, we still do nat25 for bridge's ip/mac*/
            if (!memcmp(skb->data+MACADDRLEN, priv->br_mac, MACADDRLEN)) {
                unsigned short ethtype = *((unsigned short *)(skb->data+MACADDRLEN*2));
                i = 0;
                if(ethtype == __constant_htons(ETH_P_IP)) {
                    i = 12;
                }
                else if(ethtype == __constant_htons(ETH_P_ARP)) {
                    i= 14;
                }

                if(i) {                           
                    memcpy(priv->br_ip, skb->data+WLAN_ETHHDR_LEN+i, 4);                    
                    res = nat25_db_handle(priv, skb, NAT25_INSERT);
                    if (res < 0) {
                        if (res == -2) {
                            priv->ext_stats.tx_drops++;
                            DEBUG_ERR("TX DROP: nat25_db_handle fail!\n");
                            goto free_and_stop;
                        }
                        // we just print warning message and let it go
                        DEBUG_WARN("nat25_db_handle INSERT fail!\n");
                    }    
                    memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
                }
            }

#ifdef TX_SUPPORT_IPV6_MCAST2UNI            
			if (*((unsigned short *)(skb->data+MACADDRLEN*2)) == __constant_htons(ETH_P_8021Q)) {
				is_vlan_tag = 1;
			}

			if(is_vlan_tag){
				if(ICMPV6_MCAST_MAC(skb->data) && ICMPV6_PROTO1A_VALN(skb->data)){
					memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
				}
			}else
			{
				if(ICMPV6_MCAST_MAC(skb->data) && ICMPV6_PROTO1A(skb->data)){
					memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
				}
			}
#endif            
		}



		// check if SA is equal to our MAC
		if (memcmp(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN)) {
			priv->ext_stats.tx_drops++;
			DEBUG_ERR("TX DROP: untransformed frame SA:%02X%02X%02X%02X%02X%02X!\n",
				skb->data[6],skb->data[7],skb->data[8],skb->data[9],skb->data[10],skb->data[11]);
			goto free_and_stop;
		}
#endif // RTK_BR_EXT
	}

	/* Reply caller function : Continue process */
	return TX_PROCEDURE_CTRL_CONTINUE;

#ifdef RTK_BR_EXT
free_and_stop:		/* Free current packet and stop TX process */

	rtl_kfree_skb(priv, skb, _SKB_TX_);

//stop_proc:
	/* Reply caller function : STOP process */
	return TX_PROCEDURE_CTRL_STOP;
#endif

}
#endif // CLIENT_MODE


#ifdef GBWC
static int rtl8192cd_tx_gbwc(struct rtl8192cd_priv *priv, struct stat_info	*pstat, struct sk_buff *skb)
{
	if (((priv->pmib->gbwcEntry.GBWCMode == GBWC_MODE_LIMIT_MAC_INNER) && (pstat->GBWC_in_group)) ||
		((priv->pmib->gbwcEntry.GBWCMode == GBWC_MODE_LIMIT_MAC_OUTTER) && !(pstat->GBWC_in_group)) ||
		(priv->pmib->gbwcEntry.GBWCMode == GBWC_MODE_LIMIT_IF_TX) ||
		(priv->pmib->gbwcEntry.GBWCMode == GBWC_MODE_LIMIT_IF_TRX)) {
		if ((priv->GBWC_tx_count + skb->len) > ((priv->pmib->gbwcEntry.GBWCThrd_tx * 1024 / 8) / (100 / GBWC_TO))) {
			// over the bandwidth
			if (priv->GBWC_consuming_Q) {
				// in rtl8192cd_GBWC_timer context
				priv->ext_stats.tx_drops++;
				DEBUG_ERR("TX DROP: BWC bandwidth over!\n");
				rtl_kfree_skb(priv, skb, _SKB_TX_);
			}
			else {
				// normal Tx path
				int ret = enque(priv, &(priv->GBWC_tx_queue.head), &(priv->GBWC_tx_queue.tail),
						(unsigned long)(priv->GBWC_tx_queue.pSkb), NUM_TXPKT_QUEUE, (void *)skb);
				if (ret == FALSE) {
					priv->ext_stats.tx_drops++;
					DEBUG_ERR("TX DROP: BWC tx queue full!\n");
					rtl_kfree_skb(priv, skb, _SKB_TX_);
				}
			}
			goto stop_proc;
		}
		else {
			// not over the bandwidth
			if (CIRC_CNT(priv->GBWC_tx_queue.head, priv->GBWC_tx_queue.tail, NUM_TXPKT_QUEUE) &&
					!priv->GBWC_consuming_Q) {
				// there are already packets in queue, put in queue too for order
				int ret = enque(priv, &(priv->GBWC_tx_queue.head), &(priv->GBWC_tx_queue.tail),
						(unsigned long)(priv->GBWC_tx_queue.pSkb), NUM_TXPKT_QUEUE, (void *)skb);
				if (ret == FALSE) {
					priv->ext_stats.tx_drops++;
					DEBUG_ERR("TX DROP: BWC tx queue full!\n");
					rtl_kfree_skb(priv, skb, _SKB_TX_);
				}
				goto stop_proc;
			}
			else {
				// can transmit directly
				priv->GBWC_tx_count += skb->len;
			}
		}
	}

	/* Reply caller function : Continue process */
	return TX_PROCEDURE_CTRL_CONTINUE;

stop_proc:
	/* Reply caller function : STOP process */
	return TX_PROCEDURE_CTRL_STOP;
}
#endif


#ifdef TX_SHORTCUT
static int rtl8192cd_tx_tkip(struct rtl8192cd_priv *priv, struct sk_buff *skb, struct stat_info*pstat, struct tx_insn *txcfg)
{
    struct wlan_ethhdr_t *pethhdr;
    struct llc_snap	*pllc_snap = NULL;
    unsigned char * da;
    pethhdr = (struct wlan_ethhdr_t *)(skb->data - WLAN_ETHHDR_LEN);

    da = pethhdr->daddr;
#ifdef MCAST2UI_REFINE
    memcpy(pethhdr->daddr, &skb->cb[10], 6);
#endif

#ifdef A4_STA
    if(pstat && (pstat->state & WIFI_A4_STA)) {
        da = GetAddr3Ptr(txcfg->phdr);
    }
#endif

    if(txcfg->llc) {
        pllc_snap = (struct llc_snap *)((UINT8 *)(txcfg->phdr) + txcfg->hdr_len + txcfg->iv);
    }
    
#ifdef WIFI_WMM
    if ((tkip_mic_padding(priv, da, pethhdr->saddr, ((QOS_ENABLE) && (pstat) && (pstat->QosEnabled))?skb->cb[1]:0, (UINT8 *)pllc_snap,
            skb, txcfg)) == FALSE)
#else
    if ((tkip_mic_padding(priv, da, pethhdr->saddr, 0, (UINT8 *)pllc_snap,
            skb, txcfg)) == FALSE)
#endif
    {
        priv->ext_stats.tx_drops++;
        DEBUG_ERR("TX DROP: Tkip mic padding fail!\n");
        rtl_kfree_skb(priv, skb, _SKB_TX_);
        release_wlanllchdr_to_poll(priv, txcfg->phdr);
        goto stop_proc;
    }
    skb_put((struct sk_buff *)txcfg->pframe, 8);
    txcfg->fr_len += 8;	// for Michael padding.

    /* Reply caller function : Continue process */
    return TX_PROCEDURE_CTRL_CONTINUE;

stop_proc:
    /* Reply caller function : STOP process */
    return TX_PROCEDURE_CTRL_STOP;
}


#ifdef CONFIG_PCI_HCI
__MIPS16
__IRAM_IN_865X
int get_tx_sc_index(struct rtl8192cd_priv *priv, struct stat_info *pstat, unsigned char *hdr)
{
	struct tx_sc_entry *ptxsc_entry;
	int i;

	ptxsc_entry = pstat->tx_sc_ent;

	for (i=0; i<TX_SC_ENTRY_NUM; i++) {
#ifdef MCAST2UI_REFINE          
		if  ((OPMODE & WIFI_AP_STATE)
#ifdef WDS			
				&& !(pstat->state & WIFI_WDS)
#endif				
			) {		
			if (!memcmp(hdr+6, &ptxsc_entry[i].ethhdr.saddr, sizeof(struct wlan_ethhdr_t)-6)) 
				return i;							
		}
		else				
#endif
		{
			if (!memcmp(hdr, &ptxsc_entry[i].ethhdr, sizeof(struct wlan_ethhdr_t)))
				return i;
		}
	}

	return -1;
}


#ifdef CONFIG_RTL8672
__MIPS16
__IRAM_IN_865X
#endif
int get_tx_sc_free_entry(struct rtl8192cd_priv *priv, struct stat_info *pstat, unsigned char *hdr)
{
	struct tx_sc_entry *ptxsc_entry;
	int i;

	i = get_tx_sc_index(priv, pstat, hdr);
	if (i >= 0)
		return i;
	
	ptxsc_entry = pstat->tx_sc_ent;
	
	for (i=0; i<TX_SC_ENTRY_NUM; i++) {
		if (ptxsc_entry[i].txcfg.fr_len == 0)
			return i;
	}
	
	// no free entry
	i = pstat->tx_sc_replace_idx;
	pstat->tx_sc_replace_idx = (++pstat->tx_sc_replace_idx) % TX_SC_ENTRY_NUM;
	return i;
}
#endif // CONFIG_PCI_HCI
#endif // TX_SHORTCUT


/*
	Always STOP process after calling this Procedure.
*/
#ifdef CONFIG_RTL_KERNEL_MIPS16_WLAN
__NOMIPS16
#endif
static void rtl8192cd_tx_xmitSkbFail(struct rtl8192cd_priv *priv, struct sk_buff *skb, struct net_device *dev,
				struct net_device *wdsDev, struct tx_insn *txcfg)
{
/*
 * Comment-out the following lines to disable flow-control in any case to fix
 * WDS interface may be blocked sometimes during root interface is up/down
 * continously.
 */
#if 0
#ifdef WDS
	if (wdsDev)
		netif_stop_queue(wdsDev);
	else
#endif
	{
#ifdef WIFI_WMM
		if (!QOS_ENABLE)
#endif
			netif_stop_queue(dev);
	}
#endif

	priv->ext_stats.tx_drops++;
	DEBUG_WARN("TX DROP: Congested!\n");
	if (txcfg->phdr)
		release_wlanllchdr_to_poll(priv, txcfg->phdr);
	if (skb)
		rtl_kfree_skb(priv, skb, _SKB_TX_);

	return;
}


int rtl8192cd_tx_slowPath(struct rtl8192cd_priv *priv, struct sk_buff *skb, struct stat_info *pstat,
				struct net_device *dev, struct net_device *wdsDev, struct tx_insn *txcfg)
{

	//SMP_LOCK_XMIT(flags);
#ifdef CONFIG_RTL_WAPI_SUPPORT
	if ((pstat && pstat->wapiInfo
		&& (pstat->wapiInfo->wapiType!=wapiDisable)
		&& skb->protocol != __constant_htons(ETH_P_WAPI)
		&& (!pstat->wapiInfo->wapiUCastTxEnable)))
	{
		rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
		goto stop_proc;
	}
#endif

	if (IEEE8021X_FUN && pstat && (pstat->ieee8021x_ctrlport == DOT11_PortStatus_Unauthorized) &&
#ifdef WDS
		(! (pstat->state & WIFI_WDS)) &&
#endif
#ifdef CONFIG_RTK_MESH
        (txcfg->is_11s == 0) && /* is not mesh packet*/
#endif
		(*((UINT16 *)((UINT8 *)skb->data + ETH_ALEN*2)) != htons(0x888e)) &&
		(!(OPMODE & WIFI_ADHOC_STATE)))
	{
		DEBUG_ERR("TX DROP: control port not authorized!\n");
		rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
		goto stop_proc;
	}

	// Note: Don't change txcfg->q_num for USB. If do it, you may hold two ownerships, but only release one.
#ifdef CONFIG_PCI_HCI
	if (txcfg->aggre_en < FG_AGGRE_MSDU_FIRST) {
		txcfg->q_num = BE_QUEUE;	// using low queue for data queue
		skb->cb[1] = 0;
	}
#endif
	txcfg->fr_type = _SKB_FRAME_TYPE_;
	txcfg->pframe = skb;

	if ((txcfg->aggre_en == FG_AGGRE_MSDU_MIDDLE) || (txcfg->aggre_en == FG_AGGRE_MSDU_LAST))
		txcfg->phdr = NULL;
	else
	{
		txcfg->phdr = (UINT8 *)get_wlanllchdr_from_poll(priv);

		if (txcfg->phdr == NULL) {
			DEBUG_ERR("Can't alloc wlan header!\n");
			rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
			goto stop_proc;
		}

		if (skb->len > priv->pmib->dot11OperationEntry.dot11RTSThreshold)
			txcfg->retry = priv->pmib->dot11OperationEntry.dot11LongRetryLimit;
		else
			txcfg->retry = priv->pmib->dot11OperationEntry.dot11ShortRetryLimit;

		if(*((UINT16 *)((UINT8 *)skb->data + ETH_ALEN*2)) == htons(0x888e))
			txcfg->retry = 0x20;
			
		memset((void *)txcfg->phdr, 0, sizeof(struct wlanllc_hdr));

#ifdef CONFIG_RTK_MESH
		if(txcfg->is_11s)
		{
			SetFrameSubType(txcfg->phdr, WIFI_11S_MESH);
			SetToDs(txcfg->phdr);
		}
		else
#endif
#ifdef WIFI_WMM
		if ((pstat) && (QOS_ENABLE) && (pstat->QosEnabled))
			SetFrameSubType(txcfg->phdr, WIFI_QOS_DATA);
		else
#endif
		SetFrameType(txcfg->phdr, WIFI_DATA);

		if (OPMODE & WIFI_AP_STATE) {
			SetFrDs(txcfg->phdr);
#ifdef WDS
			if (wdsDev)
				SetToDs(txcfg->phdr);
#endif
#ifdef A4_STA
            if (pstat && (pstat->state & WIFI_A4_STA)) {
                SetToDs(txcfg->phdr);   
                txcfg->pstat = pstat;
            }
#endif
		}
#ifdef CLIENT_MODE
		else if (OPMODE & WIFI_STATION_STATE) {
			SetToDs(txcfg->phdr);
#ifdef A4_STA
            if(pstat && (pstat->state & WIFI_A4_STA)) {  
                SetFrDs(txcfg->phdr); 
            }
#endif
        }
		else if (OPMODE & WIFI_ADHOC_STATE)
			/* toDS=0, frDS=0 */;
#endif
		else
			DEBUG_WARN("non supported mode yet!\n");
	}

#ifdef USE_TXQUEUE
	if (GET_ROOT(priv)->pmib->miscEntry.use_txq && priv->pshare->iot_mode_enable)
		rtl8192cd_tx_dsr((unsigned long)priv);
#endif

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	txcfg->next_txpath = TXPATH_FIRETX;
#endif
	
	//SMP_UNLOCK_XMIT(flags);
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	if (__rtl8192cd_firetx(priv, txcfg) == CONGESTED)
#else
	if (rtl8192cd_wlantx(priv, txcfg) == CONGESTED)
#endif
	{
		//SMP_LOCK_XMIT(flags);
		rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
		goto stop_proc;
	}
	//SMP_LOCK_XMIT(flags);
	
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	rtw_handle_xmit_fail(priv, txcfg);
#endif

	/* Reply caller function : process done successfully */
	//SMP_UNLOCK_XMIT(flags);
	return TX_PROCEDURE_CTRL_SUCCESS;

stop_proc:
	
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	rtw_handle_xmit_fail(priv, txcfg);
#endif
	//SMP_UNLOCK_XMIT(flags);
	
	/* Reply caller function : STOP process */
	return TX_PROCEDURE_CTRL_STOP;
}


__IRAM_IN_865X
int rtl8192cd_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	int ret;
#ifdef NETDEV_NO_PRIV
	struct rtl8192cd_priv *priv = ((struct rtl8192cd_priv *)netdev_priv(dev))->wlan_priv;
#else
	struct rtl8192cd_priv *priv = (struct rtl8192cd_priv *)dev->priv;
#endif
#if !defined(CONFIG_USB_HCI) && !defined(CONFIG_SDIO_HCI)
	unsigned long x;
#endif

#if defined(RTK_NL80211) && defined(MBSSID)
	struct net_device *dev_vap = NULL;
	if(ntohs(*((UINT16 *)((UINT8 *)skb->data + ETH_ALEN*2))) == 0x888e)
	{
		NDEBUG3("Tx EAP packets,len[%d],centCh[%d],Ch[%d] \n", 
             skb->len,priv->pshare->working_channel,priv->pshare->working_channel2);
        
		if(IS_ROOT_INTERFACE(priv))
		if(priv->pmib->miscEntry.vap_enable)
		{	
			struct stat_info	*pstat=NULL;
			unsigned char		*da;
			unsigned char		*sa;
			
			da = skb->data;
			sa = skb->data+ETH_ALEN;

			//dump_mac2(da);
			//dump_mac2(sa);
			pstat = get_stainfo(priv, da);

			if(pstat == NULL)
			{
				int i;
				for (i=0; i<RTL8192CD_NUM_VWLAN; i++) {	
					if(IS_DRV_OPEN(priv->pvap_priv[i]))
					{
						pstat = get_stainfo(priv->pvap_priv[i], da);
						if(pstat)
						{
							priv = priv->pvap_priv[i];
							dev_vap = priv->dev;
							memcpy(sa, dev_vap->dev_addr, ETH_ALEN);
							printk("Use [%s] Tx EAP packets \n", dev_vap->name);
							//dump_mac2(sa);
							break;
						}
					}
				}				
			}
		}
	}
#endif

	if (!(priv->drv_state & DRV_STATE_OPEN)){ 
		dev_kfree_skb_any(skb);
  		return 0; 
 	}

	SAVE_INT_AND_CLI(x);
	SMP_LOCK_XMIT(x);

#ifdef MCAST2UI_REFINE
	memcpy(&skb->cb[10], skb->data, 6);
#endif

#if defined(RTK_NL80211) && defined(MBSSID)
	if(dev_vap)
	ret = __rtl8192cd_start_xmit(skb, dev_vap, TX_NORMAL);
	else
#endif
	ret = __rtl8192cd_start_xmit(skb, dev, TX_NORMAL);

	RESTORE_INT(x);
	SMP_UNLOCK_XMIT(x);

	return ret;
}


#ifdef SUPPORT_TX_MCAST2UNI

__IRAM_IN_865X
int rtl8192cd_start_xmit_noM2U(struct sk_buff *skb, struct net_device *dev)
{
	int ret;

#if !defined(CONFIG_USB_HCI) && !defined(CONFIG_SDIO_HCI)
#ifdef NETDEV_NO_PRIV
	struct rtl8192cd_priv *priv = ((struct rtl8192cd_priv *)netdev_priv(dev))->wlan_priv;
#else
	struct rtl8192cd_priv *priv = (struct rtl8192cd_priv *)dev->priv;
#endif
	unsigned long x;

	SAVE_INT_AND_CLI(x);
	SMP_LOCK_XMIT(x);
#endif

	ret = __rtl8192cd_start_xmit(skb, dev, TX_NO_MUL2UNI);

#if !defined(CONFIG_USB_HCI) && !defined(CONFIG_SDIO_HCI)
	RESTORE_INT(x);
	SMP_UNLOCK_XMIT(x);
#endif

	return ret;
}

#endif

#ifdef SUPPORT_TX_AMSDU
static int amsdu_xmit(struct rtl8192cd_priv *priv, struct stat_info *pstat, struct tx_insn *txcfg, int tid,
				int from_isr, struct net_device *wdsDev, struct net_device *dev)
{
	int q_num, max_size, is_first=1, total_len=0, total_num=0;
	struct sk_buff *pskb;
	unsigned long	flags;
	int *tx_head, *tx_tail, space=0;
	struct rtl8192cd_hw	*phw = GET_HW(priv);

	txcfg->pstat = pstat;
	q_num = txcfg->q_num;

	tx_head = get_txhead_addr(phw, q_num);
	tx_tail = get_txtail_addr(phw, q_num);

	max_size = pstat->amsdu_level;

	// start to transmit queued skb
	SAVE_INT_AND_CLI(flags);
	while (skb_queue_len(&pstat->amsdu_tx_que[tid]) > 0) {
		pskb = __skb_dequeue(&pstat->amsdu_tx_que[tid]);
		if (pskb == NULL)
			break;
		total_len += (pskb->len + sizeof(struct llc_snap) + 3);

		if (is_first) {
			if (skb_queue_len(&pstat->amsdu_tx_que[tid]) > 0) {
				space = CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC);
				if (space < 10) {
#ifdef SMP_SYNC
					if (!priv->pshare->has_triggered_tx_tasklet) {
						tasklet_schedule(&priv->pshare->tx_tasklet);
						priv->pshare->has_triggered_tx_tasklet = 1;
					}
#else
					rtl8192cd_tx_dsr((unsigned long)priv);
#endif
				
					space = CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC);
					if (space < 10) {
						// printk("Tx desc not enough for A-MSDU!\n");
						__skb_queue_head(&pstat->amsdu_tx_que[tid], pskb);
						RESTORE_INT(flags);
						return 0;
					}
				}
				txcfg->aggre_en = FG_AGGRE_MSDU_FIRST;
				is_first = 0;
				total_num++;
			}
			else {
				if (!from_isr) {
					__skb_queue_head(&pstat->amsdu_tx_que[tid], pskb);
					RESTORE_INT(flags);
					return 0;
				}
				txcfg->aggre_en = 0;
			}
		}
		else if ((skb_queue_len(&pstat->amsdu_tx_que[tid]) == 0) ||
				((total_len + pstat->amsdu_tx_que[tid].next->len + sizeof(struct llc_snap) + 3) > max_size) ||
				(total_num >= (space - 4)) || // 1 for header, 1 for ICV when sw encrypt, 2 for spare
				(!pstat->is_realtek_sta && (total_num >= (priv->pmib->dot11nConfigEntry.dot11nAMSDUSendNum-1)))) {
			txcfg->aggre_en = FG_AGGRE_MSDU_LAST;
			total_len = 0;
			is_first = 1;
			total_num = 0;
		}
		else {
			txcfg->aggre_en = FG_AGGRE_MSDU_MIDDLE;
			total_num++;
		}

		pstat->amsdu_size[tid] -= (pskb->len + sizeof(struct llc_snap));
#ifdef MESH_AMSDU
		txcfg->llc = 0;
		if( isMeshPoint(pstat))
		{
			txcfg->is_11s = 8;
			dev = priv->mesh_dev;
			memcpy(txcfg->nhop_11s, pstat->hwaddr, MACADDRLEN);
		}
		else
			txcfg->is_11s = 0;

#endif
		//SMP_UNLOCK_XMIT(flags);
		rtl8192cd_tx_slowPath(priv, pskb, pstat, dev, wdsDev, txcfg);
		//SMP_LOCK_XMIT(flags);

	}
	RESTORE_INT(flags);

	return 1;
}


static int amsdu_timer_add(struct rtl8192cd_priv *priv, struct stat_info *pstat, int tid, int from_timeout)
{
	unsigned int now, timeout, new_timer=0;
	int setup_timer;
	int current_idx, next_idx;

	current_idx = priv->pshare->amsdu_timer_head;

	while (CIRC_CNT(current_idx, priv->pshare->amsdu_timer_tail, AMSDU_TIMER_NUM)) {
		if (priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].pstat == NULL) {
			priv->pshare->amsdu_timer_tail = (priv->pshare->amsdu_timer_tail + 1) & (AMSDU_TIMER_NUM - 1);
			new_timer = 1;
		}
		else
			break;
	}

	if (CIRC_CNT(current_idx, priv->pshare->amsdu_timer_tail, AMSDU_TIMER_NUM) == 0) {
		cancel_timer2(priv);
		setup_timer = 1;
	}
	else if (CIRC_SPACE(current_idx, priv->pshare->amsdu_timer_tail, AMSDU_TIMER_NUM) == 0) {
		printk("%s: %s, amsdu timer overflow!\n", priv->dev->name, __FUNCTION__ );
		return -1;
	}
	else {	// some items in timer queue
		setup_timer = 0;
		if (new_timer)
			new_timer = priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].timeout;
	}

	next_idx = (current_idx + 1) & (AMSDU_TIMER_NUM - 1);

	priv->pshare->amsdu_timer[current_idx].priv = priv;
	priv->pshare->amsdu_timer[current_idx].pstat = pstat;
	priv->pshare->amsdu_timer[current_idx].tid = (unsigned char)tid;
	priv->pshare->amsdu_timer_head = next_idx;
	now = RTL_R32(TSFTR);
	timeout = now + priv->pmib->dot11nConfigEntry.dot11nAMSDUSendTimeout;
	priv->pshare->amsdu_timer[current_idx].timeout = timeout;

	if (!from_timeout) {
		if (setup_timer)
			setup_timer2(priv, timeout);
		else if (new_timer) {
			if (TSF_LESS(new_timer, now))
				setup_timer2(priv, timeout);
			else
				setup_timer2(priv, new_timer);
		}
	}

	return current_idx;
}


void amsdu_timeout(struct rtl8192cd_priv *priv, unsigned int current_time)
{
	struct tx_insn tx_insn;
	struct stat_info *pstat;
	struct net_device *wdsDev=NULL;
	struct rtl8192cd_priv *priv_this=NULL;
	int tid=0, head;
	//DECLARE_TXCFG(txcfg, tx_insn);

	head = priv->pshare->amsdu_timer_head;
	while (CIRC_CNT(head, priv->pshare->amsdu_timer_tail, AMSDU_TIMER_NUM))
	{
		DECLARE_TXCFG(txcfg, tx_insn);	// will be reused in this while loop

		pstat = priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].pstat;
		if (pstat) {
			tid = priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].tid;
			priv_this = priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].priv;
			priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].pstat = NULL;
		}

		priv->pshare->amsdu_timer_tail = (priv->pshare->amsdu_timer_tail + 1) & (AMSDU_TIMER_NUM - 1);

		if (pstat) {
#ifdef WDS
			wdsDev = NULL;
			if (pstat->state & WIFI_WDS) {
				wdsDev = getWdsDevByAddr(priv, pstat->hwaddr);
				txcfg->wdsIdx = getWdsIdxByDev(priv, wdsDev);
			}
#endif
#ifdef RTL_MANUAL_EDCA
			txcfg->q_num = PRI_TO_QNUM(priv, tid);
#else
			PRI_TO_QNUM(tid, txcfg->q_num, priv->pmib->dot11OperationEntry.wifi_specific);
#endif

			if (pstat->state & WIFI_SLEEP_STATE)
				pstat->amsdu_timer_id[tid] = amsdu_timer_add(priv_this, pstat, tid, 1) + 1;
			else
			{
				int ret;
				unsigned long flags;

				SMP_LOCK_XMIT(flags);
				ret = amsdu_xmit(priv_this, pstat, txcfg, tid, 1, wdsDev, priv->dev);
				SMP_UNLOCK_XMIT(flags);
				if (ret == 0) // not finish
					pstat->amsdu_timer_id[tid] = amsdu_timer_add(priv_this, pstat, tid, 1) + 1;
			    else
				    pstat->amsdu_timer_id[tid] = 0;
		    }
	    }
	}

	if (CIRC_CNT(priv->pshare->amsdu_timer_head, priv->pshare->amsdu_timer_tail, AMSDU_TIMER_NUM)) {
		setup_timer2(priv, priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].timeout);
		if (TSF_LESS(priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].timeout, current_time))
			printk("Setup timer2 %d too late (now %d)\n", priv->pshare->amsdu_timer[priv->pshare->amsdu_timer_tail].timeout, current_time);
	}
}


 int amsdu_check(struct rtl8192cd_priv *priv, struct sk_buff *skb, struct stat_info *pstat, struct tx_insn *txcfg)
{
	int q_num;
	unsigned int priority;
	unsigned short protocol;
	int *tx_head, *tx_tail, cnt, add_timer=1;
	struct rtl8192cd_hw	*phw;
#ifndef SMP_SYNC
	unsigned long flags;
#endif
	struct net_device *wdsDev=NULL;

	protocol = ntohs(*((UINT16 *)((UINT8 *)skb->data + ETH_ALEN*2)));
#ifdef CONFIG_RTL_WAPI_SUPPORT
	if ((protocol == 0x888e)||(protocol == ETH_P_WAPI))
#else
	if (protocol == 0x888e)
#endif
	{
		return RET_AGGRE_BYPASS;
	}

	if (((protocol + WLAN_ETHHDR_LEN) > WLAN_MAX_ETHFRM_LEN) &&
				 (skb_headroom(skb) < sizeof(struct llc_snap))) {
		return RET_AGGRE_BYPASS;
	}
//----------------------

#ifdef CONFIG_RTK_MESH
	priority = get_skb_priority3(priv, skb, txcfg->is_11s, pstat);
#else
	priority = get_skb_priority(priv, skb, pstat);
#endif
#ifdef RTL_MANUAL_EDCA
	q_num = PRI_TO_QNUM(priv, priority);
#else
	PRI_TO_QNUM(priority, q_num, priv->pmib->dot11OperationEntry.wifi_specific);
#endif

	phw = GET_HW(priv);
	tx_head = get_txhead_addr(phw, q_num);
	tx_tail = get_txtail_addr(phw, q_num);

	cnt = CIRC_CNT_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC);

#if 0
	if (cnt <= AMSDU_TX_DESC_TH)
		return RET_AGGRE_BYPASS;
#endif

	if (cnt == (CURRENT_NUM_TX_DESC - 1))
		return RET_AGGRE_DESC_FULL;

#ifdef MESH_AMSDU
	if(txcfg->is_11s==0 && isMeshPoint(pstat))
	{
		return RET_AGGRE_DESC_FULL;
	}
	if (txcfg->is_11s&1)
	{
		short j, popen =  ((txcfg->mesh_header.mesh_flag &1) ? 16 : 4);
		if (skb_headroom(skb) < popen || skb_cloned(skb)) {
			struct sk_buff *skb2 = dev_alloc_skb(skb->len);
			if (skb2 == NULL) {
				printk("%s: %s, dev_alloc_skb() failed!\n", priv->mesh_dev->name, __FUNCTION__);
				return RET_AGGRE_BYPASS;
			}
         	memcpy(skb_put(skb2, skb->len), skb->data, skb->len);
			dev_kfree_skb_any(skb);
			skb = skb2;
			txcfg->pframe = (void *)skb;
		}
		skb_push(skb, popen);
		for(j=0; j<sizeof(struct wlan_ethhdr_t); j++)
			skb->data[j]= skb->data[j+popen];
		memcpy(skb->data+j, &(txcfg->mesh_header), popen);
	}
#endif // MESH_AMSDU


	SAVE_INT_AND_CLI(flags);
	__skb_queue_tail(&pstat->amsdu_tx_que[priority], skb);
	pstat->amsdu_size[priority] += (skb->len + sizeof(struct llc_snap));

	if ((pstat->amsdu_size[priority] >= pstat->amsdu_level) ||
			(!pstat->is_realtek_sta && (skb_queue_len(&pstat->amsdu_tx_que[priority]) >= priv->pmib->dot11nConfigEntry.dot11nAMSDUSendNum)))
	{
#ifdef WDS
		wdsDev = NULL;
		if (pstat->state & WIFI_WDS) {
			wdsDev = getWdsDevByAddr(priv, pstat->hwaddr);
			txcfg->wdsIdx = getWdsIdxByDev(priv, wdsDev);
		}
#endif
		// delete timer entry
		if (pstat->amsdu_timer_id[priority] > 0) {
			priv->pshare->amsdu_timer[pstat->amsdu_timer_id[priority] - 1].pstat = NULL;
			pstat->amsdu_timer_id[priority] = 0;
		}
		txcfg->q_num = q_num;
		if (amsdu_xmit(priv, pstat, txcfg, priority, 0, wdsDev, priv->dev) == 0) // not finish
			pstat->amsdu_timer_id[priority] = amsdu_timer_add(priv, pstat, priority, 0) + 1;
		else
			add_timer = 0;
	}

	if (add_timer) {
		if (pstat->amsdu_timer_id[priority] == 0)
			pstat->amsdu_timer_id[priority] = amsdu_timer_add(priv, pstat, priority, 0) + 1;
	}

	RESTORE_INT(flags);

	return RET_AGGRE_ENQUE;
}
#endif // SUPPORT_TX_AMSDU

#ifdef SUPPORT_TX_MCAST2UNI
#ifndef CONFIG_RTK_MESH
static 
#endif
int isICMPv6Mng(struct sk_buff *skb)
{
#ifdef __ECOS
	if((skb->data[12] == 0x86) &&(skb->data[13] == 0xdd)&&
#else
	if((skb->protocol == __constant_htons(0x86dd)) &&
#endif
		((skb->data[20] == 0x3a) || (skb->data[54] == 0x3a)) //next header is icmpv6
		//&& skb->data[54] == 0x86 //RA
		/*128,129,133,.....255 SHOULD BE MANAGMENT PACKET
		 REF:http://en.wikipedia.org/wiki/ICMPv6 */
	)
	{
		return 1;		
	}
	else
		return 0;
}

#ifndef CONFIG_RTK_MESH
static 
#endif
inline int isMDNS(unsigned char *data)
{
	if((data[3] == 0x00) && (data[4] == 0x00) && (data[5] == 0xfb) &&
			(((data[12] == 0x08) && (data[13] == 0x00) // IPv4
			&& (data[23] == 0x11) // UDP
			//&& (data[30] == 0xe0) // 224.0.0.251
			&& (data[36] == 0x14) && (data[37] == 0xe9)) // port 5353
			||
			((data[12] == 0x86) && (data[13] == 0xdd) // IPv6
			&& (data[20] == 0x11) // next header is UDP
			&& (data[56] == 0x14) && (data[57] == 0xe9)) // port 5353
			)) {
		return 1;
	}
		
	return 0;
}

#ifdef CONFIG_PCI_HCI
static int rtl8192cd_tx_recycle(struct rtl8192cd_priv *priv, unsigned int txRingIdx, int *recycleCnt_p);
static inline void check_tx_queue(struct rtl8192cd_priv *priv)
{
		int *tx_head, *tx_tail;
		struct rtl8192cd_hw	*phw = GET_HW(priv);

#ifdef  CONFIG_WLAN_HAL
        PHCI_TX_DMA_MANAGER_88XX    ptx_dma;
		if (IS_HAL_CHIP(priv)) {      
	        ptx_dma = (PHCI_TX_DMA_MANAGER_88XX)(_GET_HAL_DATA(priv)->PTxDMA88XX);
		} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif
		{//not HAL
			tx_head = get_txhead_addr(phw, BE_QUEUE);// use BE queue to send multicast
			tx_tail = get_txtail_addr(phw, BE_QUEUE);		
		}
        if (
#ifdef  CONFIG_WLAN_HAL
            (IS_HAL_CHIP(priv)) ? (compareAvailableTXBD(priv, (CURRENT_NUM_TX_DESC/4), BE_QUEUE, 2)) :
#endif		
			(CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC) < (CURRENT_NUM_TX_DESC/4)) 
		)	
		{
			rtl8192cd_tx_queueDsr(priv, BE_QUEUE);
			//int recycleCnt;
			//rtl8192cd_tx_recycle(priv, BE_QUEUE, &recycleCnt);
		}
		
		return;
}
#endif // CONFIG_PCI_HCI


#if defined(CONFIG_RTL865X_ETH_PRIV_SKB) || defined(CONFIG_RTL_ETH_PRIV_SKB)
	extern struct sk_buff *priv_skb_copy(struct sk_buff *skb);
	extern int eth_skb_free_num;
#endif
#ifdef CONFIG_RTL8196B_GW_8M
#define ETH_SKB_FREE_TH 50
#else
#define ETH_SKB_FREE_TH 100
#endif

int isSpecialFloodMac(struct rtl8192cd_priv *priv, struct sk_buff *skb)
{
	int i;
	if(priv->pshare->rf_ft_var.mc2u_flood_ctrl==0)
	{
		return 0;
	}
	
	for(i=0; i< priv->pshare->rf_ft_var.mc2u_flood_mac_num; i++)
	{
		if(memcmp(skb->data, priv->pshare->rf_ft_var.mc2u_flood_mac[i].macAddr,MACADDRLEN)==0)
		{
			return 1;
		}

	}
	return 0;
}

int mlcst2unicst(struct rtl8192cd_priv *priv, struct sk_buff *skb)
{
	struct stat_info *pstat, *sa_stat;
	struct list_head *phead, *plist;
	struct sk_buff *newskb;
	unsigned char origDest[6];
	char skbCloned=0;
	int i= 0;
	int m2uCnt =0;
	int fwdCnt=0;
	struct stat_info *pstat_found = NULL;
#ifdef MCAST2UI_REFINE
	unsigned int privacy;
#endif

#ifdef CONFIG_MAXED_MULTICAST	
	int M2Uanyway=0;
#endif

#ifdef HS2_SUPPORT
	// if AP 
	// 1.support HS2 and 
	// 2.dgaf disable=0 
	// it means to let icmpv6 mgmt multicast to clients
	if ((priv->pmib->hs2Entry.hs2_ielen != 0) && (priv->dgaf_disable == 0)) {
		if (isICMPv6Mng(skb) || IS_ICMPV6_PROTO(skb->data))
			return 0;
    }
#endif
	
	skbCloned = skb_cloned(skb);
	memcpy(origDest, skb->data, 6);

    sa_stat = get_stainfo(priv, skb->data+MACADDRLEN);
#ifdef A4_STA
    if (priv->pshare->rf_ft_var.a4_enable) {
        if(sa_stat == NULL) {
            sa_stat = a4_sta_lookup(priv, skb->data+MACADDRLEN);
        }
    }
#endif  

	// all multicast managment packet try do m2u
	if( isSpecialFloodMac(priv,skb) || IS_MDNSV4_MAC(skb->data)||IS_MDNSV6_MAC(skb->data)||IS_IGMP_PROTO(skb->data) || isICMPv6Mng(skb) || IS_ICMPV6_PROTO(skb->data)|| isMDNS(skb->data))
	{
		/*added by qinjunjie,do multicast to unicast conversion, and send to every associated station */
		phead = &priv->asoc_list;
		plist = phead;
		while ((plist = asoc_list_get_next(priv, plist)) != phead) {
			pstat = list_entry(plist, struct stat_info, asoc_list);

            #ifdef CONFIG_RTK_MESH
            if(isPossibleNeighbor(pstat)) {
                continue;
            }
            #endif

			/* avoid   come from STA1 and send back STA1 */ 
			if (sa_stat == pstat) {
				continue; 
			}
			
            if(pstat->leave || pstat->expire_to==0)
				continue; 			
#ifdef MCAST2UI_REFINE
			privacy = get_sta_encrypt_algthm(priv, pstat);
			if ((privacy == _NO_PRIVACY_ || privacy == _CCMP_PRIVACY_) &&
#ifdef CONFIG_IEEE80211W
				!UseSwCrypto(priv, pstat, (pstat ? FALSE : TRUE),0) && (newskb = skb_clone(skb, GFP_ATOMIC))) {
#else
				!UseSwCrypto(priv, pstat, (pstat ? FALSE : TRUE)) && (newskb = skb_clone(skb, GFP_ATOMIC))) {
#endif
				memcpy(&newskb->cb[10], pstat->hwaddr, 6);
				newskb->cb[2] = (char)0xff;
				__rtl8192cd_start_xmit(newskb, priv->dev, TX_NO_MUL2UNI);
			}
			else
#endif
			{
				if((plist->next == phead) && !skbCloned) {
					asoc_list_unref(priv, pstat);
#ifdef MCAST2UI_REFINE                                                  
					memcpy(&skb->cb[10], pstat->hwaddr, 6);
#else
					memcpy(skb->data, pstat->hwaddr, 6);
#endif
					skb->cb[2] = (char)0xff;			// not do aggregation
#ifdef ENABLE_RTL_SKB_STATS
					rtl_atomic_dec(&priv->rtl_tx_skb_cnt);
#endif
					__rtl8192cd_start_xmit(skb, priv->dev, TX_NO_MUL2UNI);
					return TRUE;
				}
				
#if defined(CONFIG_RTL865X_ETH_PRIV_SKB) || defined(CONFIG_RTL_ETH_PRIV_SKB)
				newskb = priv_skb_copy(skb);
#else
				newskb = skb_copy(skb, GFP_ATOMIC);
#endif
				if (newskb) {
#ifdef MCAST2UI_REFINE                                                  
					memcpy(&newskb->cb[10], pstat->hwaddr, 6);
#else
					memcpy(newskb->data, pstat->hwaddr, 6);
#endif
					newskb->cb[2] = (char)0xff;			// not do aggregation
					__rtl8192cd_start_xmit(newskb, priv->dev, TX_NO_MUL2UNI);
				}
				else {
					asoc_list_unref(priv, pstat);
					
					DEBUG_ERR("%s: muti2unit skb_copy() failed!\n", priv->dev->name);
					priv->stop_tx_mcast2uni = 2;
					priv->ext_stats.tx_drops++;
					DEBUG_ERR("TX DROP: %s: run out ether buffer!\n", __FUNCTION__);
					rtl_kfree_skb(priv, skb, _SKB_TX_);
					return TRUE;
				}

			}
		}
		rtl_kfree_skb(priv, skb, _SKB_TX_);
		return TRUE;
	}

#ifdef TV_MODE
    if((priv->tv_mode_status & BIT0) == 0) { /*TV mode is disabled*/
#ifdef A4_STA
        if(priv->pshare->rf_ft_var.a4_enable == 0)
#endif
            return FALSE;
    }
#endif

//#ifdef VIDEO_STREAMING_REFINE
	// for video streaming refine 
	phead = &priv->asoc_list;
	plist = phead;
	while ((plist = asoc_list_get_next(priv, plist)) != phead) {
		pstat = list_entry(plist, struct stat_info, asoc_list);

        #ifdef CONFIG_RTK_MESH
        if(isPossibleNeighbor(pstat)) {
            continue;
        }
        #endif

		/* avoid   come from STA1 and send back STA1 */ 
		if (sa_stat == pstat){		
			continue; 
		}		
        if(pstat->leave || pstat->expire_to==0)
            continue; 
		for (i=0; i<MAX_IP_MC_ENTRY; i++) {
			if (pstat->ipmc[i].used && !memcmp(&pstat->ipmc[i].mcmac[0], origDest, 6)) {                
				pstat_found = pstat;
				m2uCnt++;
				break;
			}
		}
	}

#ifdef TV_MODE
    if(priv->tv_mode_status & BIT0)/*TV mode is enabled*/    
#endif
    {
        if (m2uCnt == 1 && !skbCloned) {
#ifdef MCAST2UI_REFINE                                                  
            memcpy(&skb->cb[10], pstat_found->hwaddr, 6);
#else
            memcpy(skb->data, pstat_found->hwaddr, 6);
#endif
#ifdef ENABLE_RTL_SKB_STATS
            rtl_atomic_dec(&priv->rtl_tx_skb_cnt);
#endif
            __rtl8192cd_start_xmit(skb, priv->dev, TX_NO_MUL2UNI);		
            return TRUE;
        }
    }

	fwdCnt = m2uCnt;
	
	if(m2uCnt == 0){

#ifdef CONFIG_MAXED_MULTICAST	
		/*case: when STA <=3 do M2U anyway ; 
		if STA number > 3 by orig method(multicast);*/
		if(priv->assoc_num <=3){
			M2Uanyway=1;
			fwdCnt = priv->assoc_num;			
		}else
#endif
		if(!priv->pshare->rf_ft_var.mc2u_drop_unknown) 
		{
			/*case: if M2U can't success then 
			  forward by multicast(orig method),
			  defect: may affect system performance
		    	 advantage:better compatibility*/ 
			return FALSE;		
		}
		else
		{

		/*case: if M2U can't success then drop this packet ;
		    defect:maybe some management packet will lose
		    advantage:better performance*/ 
	   	    DEBUG_WARN("TX DROP: %s %d !\n", __FUNCTION__,__LINE__);
			priv->ext_stats.tx_drops++;
			rtl_kfree_skb(priv, skb, _SKB_TX_);
			return TRUE;
			
		}
	}
//#endif


	
	// Do multicast to unicast conversion
	phead = &priv->asoc_list;
	plist = phead;
	while ((plist = asoc_list_get_next(priv, plist)) != phead) {
		pstat = list_entry(plist, struct stat_info, asoc_list);

        #ifdef CONFIG_RTK_MESH
        if(isPossibleNeighbor(pstat)) {
            continue;
        }
        #endif

		/* avoid   come from STA1 and send back STA1 */ 
		if (sa_stat == pstat) {
			continue;
		}   		
        
        if(pstat->leave || pstat->expire_to==0)
            continue;

#ifdef TV_MODE
        if((priv->tv_mode_status & BIT0) == 0) { /*TV mode is disabled*/
#ifdef A4_STA
            if((pstat->state & WIFI_A4_STA) == 0)
#endif
                continue;
        }
#endif

#ifdef CONFIG_PCI_HCI
		{
			int *tx_head, *tx_tail, q_num;
			struct rtl8192cd_hw	*phw = GET_HW(priv);
			q_num = BE_QUEUE;	// use BE queue to send multicast
#ifdef CONFIG_WLAN_HAL
	        //PHCI_TX_DMA_MANAGER_88XX    ptx_dma;
			if (IS_HAL_CHIP(priv)) {
				//ptx_dma = (PHCI_TX_DMA_MANAGER_88XX)(_GET_HAL_DATA(priv)->PTxDMA88XX);
			} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif
			{//not HAL
				tx_head = get_txhead_addr(phw, q_num);
				tx_tail = get_txtail_addr(phw, q_num);
			}
			
			if (priv->stop_tx_mcast2uni) {
				rtl8192cd_tx_queueDsr(priv, q_num);

				if (priv->stop_tx_mcast2uni  == 1) {
#ifdef CONFIG_WLAN_HAL
                        if(IS_HAL_CHIP(priv)) {
                        	if(compareAvailableTXBD(priv, ((CURRENT_NUM_TX_DESC*1)/4), q_num, 1)) 
								priv->stop_tx_mcast2uni = 0;
                        } else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif
						{//not HAL
							if(CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC) > (CURRENT_NUM_TX_DESC*1)/4)	
								priv->stop_tx_mcast2uni = 0;
						}
				}

#if defined(CONFIG_RTL865X_ETH_PRIV_SKB) || defined(CONFIG_RTL_ETH_PRIV_SKB)
				else if ((priv->stop_tx_mcast2uni == 2) && (eth_skb_free_num > ETH_SKB_FREE_TH))
				{
					priv->stop_tx_mcast2uni = 0;
				}
#endif
				else {
					asoc_list_unref(priv, pstat);
					
					priv->ext_stats.tx_drops++;
					DEBUG_ERR("TX DROP: %s: run out ether buffer!\n", __FUNCTION__);
					rtl_kfree_skb(priv, skb, _SKB_TX_);
					return TRUE;
				}
			} else {
#ifdef CONFIG_WLAN_HAL
                if(IS_HAL_CHIP(priv)) {
					if(compareAvailableTXBD(priv, 20, q_num, 2)) {
						asoc_list_unref(priv, pstat);
						#ifdef __ECOS
						#else
						priv->stop_tx_mcast2uni = 1;
						#endif
						priv->ext_stats.tx_drops++;
						DEBUG_ERR("TX DROP: %s: txdesc full!\n", __FUNCTION__);
						rtl_kfree_skb(priv, skb, _SKB_TX_);
						return TRUE;
					}
				} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif			
				{//not HAL
					if (CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC) < 20) {
						asoc_list_unref(priv, pstat);
						#ifdef __ECOS
						#else
						priv->stop_tx_mcast2uni = 1;
						#endif
						priv->ext_stats.tx_drops++;
						DEBUG_ERR("TX DROP: %s: txdesc full!\n", __FUNCTION__);
						rtl_kfree_skb(priv, skb, _SKB_TX_);
						return TRUE;
					}
				}
			}
		}
#endif // CONFIG_PCI_HCI

#ifdef CONFIG_MAXED_MULTICAST

		if(M2Uanyway){
			if((fwdCnt== 1) && !skb_cloned(skb))
			{
				asoc_list_unref(priv, pstat);
#ifdef MCAST2UI_REFINE                                                  
				memcpy(&skb->cb[10], pstat->hwaddr, 6);
#else
				memcpy(skb->data, pstat->hwaddr, 6);
#endif
				skb->cb[2] = (char)0xff;			// not do aggregation
#ifdef ENABLE_RTL_SKB_STATS
				rtl_atomic_dec(&priv->rtl_tx_skb_cnt);
#endif
				__rtl8192cd_start_xmit(skb, priv->dev, TX_NO_MUL2UNI);
				return TRUE;
			}
			
#if defined(CONFIG_RTL865X_ETH_PRIV_SKB) || defined(CONFIG_RTL_ETH_PRIV_SKB)
			newskb = priv_skb_copy(skb);
#else
			newskb = skb_copy(skb, GFP_ATOMIC);
#endif
			if (newskb) {
#ifdef MCAST2UI_REFINE                                                  
				memcpy(&newskb->cb[10], pstat->hwaddr, 6);
#else
				memcpy(newskb->data, pstat->hwaddr, 6);
#endif
				newskb->cb[2] = (char)0xff;			// not do aggregation
				__rtl8192cd_start_xmit(newskb, priv->dev, TX_NO_MUL2UNI);
			}
			else {
				asoc_list_unref(priv, pstat);
				
				DEBUG_ERR("%s: muti2unit skb_copy() failed!\n", priv->dev->name);
				#ifdef __ECOS
				#else
				priv->stop_tx_mcast2uni = 2;
				#endif
				priv->ext_stats.tx_drops++;
				DEBUG_ERR("TX DROP: %s: run out ether buffer!\n", __FUNCTION__);
				rtl_kfree_skb(priv, skb, _SKB_TX_);
				return TRUE;
			}
			fwdCnt--;
			continue;
		}
#endif
		for (i=0; i<MAX_IP_MC_ENTRY; i++) {
			if (pstat->ipmc[i].used && !memcmp(&pstat->ipmc[i].mcmac[0], origDest, 6)) {


				if((fwdCnt== 1) && !skbCloned)
				{
					asoc_list_unref(priv, pstat);
#ifdef MCAST2UI_REFINE                                                  
					memcpy(&skb->cb[10], pstat->hwaddr, 6);
#else
					memcpy(skb->data, pstat->hwaddr, 6);
#endif
					skb->cb[2] = (char)0xff; 		// not do aggregation
#ifdef ENABLE_RTL_SKB_STATS
					rtl_atomic_dec(&priv->rtl_tx_skb_cnt);
#endif
					__rtl8192cd_start_xmit(skb, priv->dev, TX_NO_MUL2UNI);
					return TRUE;
				}
				else {
#ifdef MCAST2UI_REFINE
					privacy = get_sta_encrypt_algthm(priv, pstat);
					if ((privacy == _NO_PRIVACY_ || (privacy == _CCMP_PRIVACY_ &&
#ifdef CONFIG_IEEE80211W
							!UseSwCrypto(priv, pstat, FALSE,0))) && (newskb = skb_clone(skb, GFP_ATOMIC))) {
#else
							!UseSwCrypto(priv, pstat, FALSE))) && (newskb = skb_clone(skb, GFP_ATOMIC))) {
#endif
						memcpy(&newskb->cb[10], pstat->hwaddr, 6);
						newskb->cb[2] = (char)0xff;
						__rtl8192cd_start_xmit(newskb, priv->dev, TX_NO_MUL2UNI);
					}
					else
#endif
					{	
						#if defined(CONFIG_RTL865X_ETH_PRIV_SKB) || defined(CONFIG_RTL_ETH_PRIV_SKB)

						newskb = priv_skb_copy(skb);
						#else
						newskb = skb_copy(skb, GFP_ATOMIC);
						#endif
						if (newskb) {
#ifdef MCAST2UI_REFINE                                                  
							memcpy(&newskb->cb[10], pstat->hwaddr, 6);
#else
							memcpy(newskb->data, pstat->hwaddr, 6);
#endif
							newskb->cb[2] = (char)0xff;			// not do aggregation
							__rtl8192cd_start_xmit(newskb, priv->dev, TX_NO_MUL2UNI);
						}
						else {
							asoc_list_unref(priv, pstat);
							
							DEBUG_ERR("%s: muti2unit skb_copy() failed!\n", priv->dev->name);
							#ifdef __ECOS
							#else
							priv->stop_tx_mcast2uni = 2;
							#endif
							priv->ext_stats.tx_drops++;
							DEBUG_ERR("TX DROP: %s: run out ether buffer!\n", __FUNCTION__);
							rtl_kfree_skb(priv, skb, _SKB_TX_);
							return TRUE;
						}
					}
					fwdCnt--;
					break;
				}
			}
		}
	}
	
#ifdef TV_MODE
    if((priv->tv_mode_status & BIT0) == 0 && fwdCnt) {
        /*TV mode is disabled and has joint legacy sta*/
        if(!skbCloned) {   
            #ifdef ENABLE_RTL_SKB_STATS
            rtl_atomic_dec(&priv->rtl_tx_skb_cnt);
            #endif            
            __rtl8192cd_start_xmit(skb, priv->dev, TX_NO_MUL2UNI);
        } else 
        {
            #if defined(CONFIG_RTL865X_ETH_PRIV_SKB) || defined(CONFIG_RTL_ETH_PRIV_SKB)
            newskb = priv_skb_copy(skb);
            #else
            newskb = skb_copy(skb, GFP_ATOMIC);
            #endif
            if (newskb) {
                __rtl8192cd_start_xmit(newskb, priv->dev, TX_NO_MUL2UNI);
            }
            else {            
                asoc_list_unref(priv, pstat);                
                DEBUG_ERR("%s: muti2unit skb_copy() failed!\n", priv->dev->name);
                #ifdef __ECOS
                #else
                priv->stop_tx_mcast2uni = 2;
                #endif
                priv->ext_stats.tx_drops++;
                DEBUG_ERR("TX DROP: %s: run out ether buffer!\n", __FUNCTION__);
            }
            rtl_kfree_skb(priv, skb, _SKB_TX_);
        }
        return TRUE;
    }
#endif
	/*
	 *	Device interested in this MC IP cannot be found, drop packet.
	 */
	priv->ext_stats.tx_drops++;
	DEBUG_ERR("TX DROP: %s %d: Device interested in this MC IP cannot be found !\n", __FUNCTION__,__LINE__);
	rtl_kfree_skb(priv, skb, _SKB_TX_);
	return TRUE;
}

#endif // TX_SUPPORT_MCAST2U


#ifdef RESERVE_TXDESC_FOR_EACH_IF
int check_txdesc_dynamic_mechanism(struct rtl8192cd_priv *priv, int q_num, int txdesc_need)
{
	struct rtl8192cd_priv *root_priv = NULL;
	unsigned int lower_limit = priv->pshare->num_txdesc_lower_limit;
	unsigned int avail_cnt = priv->pshare->num_txdesc_cnt;
	unsigned int used = priv->use_txdesc_cnt[q_num];
	unsigned int accu = 0, i;

	if (IS_ROOT_INTERFACE(priv))
		root_priv = priv;
	else
		root_priv = GET_ROOT(priv);
	
	if (IS_ROOT_INTERFACE(priv)) {
		if (IS_DRV_OPEN(priv))
			accu += used;
	} else {
		if (IS_DRV_OPEN(root_priv))
			accu += MAX_NUM(root_priv->use_txdesc_cnt[q_num], lower_limit);
	}
	
#ifdef UNIVERSAL_REPEATER
	if (IS_VXD_INTERFACE(priv)) {
		if (IS_DRV_OPEN(priv))
			accu += used;
	} else {
		if (IS_DRV_OPEN(root_priv->pvxd_priv))
			accu += MAX_NUM(root_priv->pvxd_priv->use_txdesc_cnt[q_num], lower_limit);
	}
#endif

#ifdef MBSSID
	for (i=0; i<RTL8192CD_NUM_VWLAN; i++) {
		if (IS_DRV_OPEN(root_priv->pvap_priv[i])) {
			if (root_priv->pvap_priv[i] == priv)
				accu += used;
	else
				accu += MAX_NUM(root_priv->pvap_priv[i]->use_txdesc_cnt[q_num], lower_limit);
		}
	}
#endif
	
	if (accu + txdesc_need <= avail_cnt)
		return 0;
	
	return -1;
}

#ifdef USE_TXQUEUE
int check_txq_dynamic_mechanism(struct rtl8192cd_priv *priv, int q_num)
{
	struct rtl8192cd_priv *root_priv = NULL;
	unsigned int lower_limit = priv->pshare->num_txq_lower_limit;
	unsigned int avail_cnt = priv->pshare->num_txq_cnt;
	unsigned int used = priv->use_txq_cnt[q_num];
	unsigned int accu = 0, i;
	
	if (IS_ROOT_INTERFACE(priv))
		root_priv = priv;
	else
		root_priv = GET_ROOT(priv);
		
	if (IS_ROOT_INTERFACE(priv))
	{
		if ( IS_DRV_OPEN(priv) )
			accu += used;
	}
	else
	{
		if ( IS_DRV_OPEN(root_priv) )
			accu += MAX_NUM(root_priv->use_txq_cnt[q_num], lower_limit);
	}
		
#ifdef UNIVERSAL_REPEATER
	if (IS_VXD_INTERFACE(priv))
	{
		if ( IS_DRV_OPEN(priv) )
			accu += used;
	}
	else
	{
		if ( IS_DRV_OPEN(root_priv->pvxd_priv) )
			accu += MAX_NUM(root_priv->pvxd_priv->use_txq_cnt[q_num], lower_limit);
	}
#endif
	
#ifdef MBSSID
	for (i=0; i<RTL8192CD_NUM_VWLAN; i++)
	{
		if ( IS_DRV_OPEN(root_priv->pvap_priv[i]) )
		{
			if (root_priv->pvap_priv[i] == priv)
				accu += used;
			else
				accu += MAX_NUM(root_priv->pvap_priv[i]->use_txq_cnt[q_num], lower_limit);
		}
	}
#endif
		
	if (accu < avail_cnt)
		return 0;

	return -1;

}
#endif
#endif


#ifdef BEAMFORMING_SUPPORT
BOOLEAN
IsMgntNDPA(
	pu1Byte		pdu
)
{
	BOOLEAN ret = 0;
	if(IsMgntActionNoAck(pdu) && GET_80211_HDR_ORDER(pdu))
	{
		if(GET_HT_CTRL_NDP_ANNOUNCEMENT(pdu+sMacHdrLng) == 1)
			ret = 1;
	}
	return ret;
}
#endif

void RtsCheck(struct rtl8192cd_priv *priv, struct tx_insn* txcfg,
	BOOLEAN *bRtsEnable, BOOLEAN *bCts2SelfEnable, BOOLEAN *bHwRts, BOOLEAN *bErpProtect, BOOLEAN *bNProtect)
{
	*bRtsEnable = FALSE;
	*bCts2SelfEnable = FALSE;
	*bHwRts = FALSE;
	*bErpProtect = FALSE;
	*bNProtect = FALSE;

	if ((txcfg->rts_thrshld <= get_mpdu_len(txcfg, txcfg->fr_len)) ||
		(txcfg->pstat && txcfg->pstat->is_forced_rts))
		*bRtsEnable = TRUE;
	else {
		if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11N) &&
			is_MCS_rate(txcfg->tx_rate) &&
			(priv->ht_protection /*|| txcfg->pstat->is_rtl8190_sta*/))
		{
			*bNProtect = 1;
			if (priv->pmib->dot11ErpInfo.protection)
				*bErpProtect = 1;
			if (priv->pmib->dot11ErpInfo.ctsToSelf)
				*bCts2SelfEnable = TRUE;
			else					
				*bRtsEnable = TRUE;
				
		}
		else if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11G) &&
			(!is_CCK_rate(txcfg->tx_rate)) && // OFDM mode
			priv->pmib->dot11ErpInfo.protection)
		{
			*bErpProtect = 1;
			if (priv->pmib->dot11ErpInfo.ctsToSelf)
				*bCts2SelfEnable = TRUE;
			else						
				*bRtsEnable = TRUE;
			
		}
		else if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11N) &&
			(txcfg->pstat) && (txcfg->pstat->MIMO_ps & _HT_MIMO_PS_DYNAMIC_))
		{	// when HT MIMO Dynamic power save is set, RTS is needed
			*bRtsEnable = TRUE;
		
		} else {
			/*
			 * Auto rts mode, use rts depends on packet length and packet tx time
			 */
			if (priv->assoc_num >10) {
				*bRtsEnable = TRUE;	
			 } else

			if (is_MCS_rate(txcfg->tx_rate) && (/*(txcfg->pstat->IOTPeer!=HT_IOT_PEER_INTEL) ||*/ !txcfg->pstat->no_rts)) 
			{
				*bNProtect = 1;
				*bHwRts = TRUE;
				if (priv->pmib->dot11ErpInfo.ctsToSelf)
					*bCts2SelfEnable = TRUE;
				else						
					*bRtsEnable = TRUE;					
			}
		}
	}
}

#ifdef RTK_AC_SUPPORT
static void RtsCheckAC(struct rtl8192cd_priv *priv, struct tx_insn* txcfg, BOOLEAN *bRtsEnable, BOOLEAN *bCts2SelfEnable, BOOLEAN *bHwRts)
{
	if((priv->pshare->rf_ft_var.cca_rts) && (txcfg->pstat->vht_cap_len > 0)) {
		*bHwRts = FALSE;
		*bCts2SelfEnable = FALSE;
		*bRtsEnable = TRUE;
	}
}
#endif

#ifdef CONFIG_RTL_8812_SUPPORT
static void rtl8192cd_fill_fwinfo_8812(struct rtl8192cd_priv *priv, struct tx_insn* txcfg, struct tx_desc  *pdesc, unsigned int frag_idx)
{
	char n_txshort = 0, bg_txshort = 0;
	//int erp_protection = 0, n_protection = 0;
//	unsigned char rate;
	unsigned char txRate = 0;
#ifdef DRVMAC_LB
	static unsigned int rate_select = 0;
#endif
	BOOLEAN			bRtsEnable;
	BOOLEAN			bErpProtect;
	BOOLEAN			bNProtect;
	BOOLEAN			bHwRts;
	BOOLEAN			bCts2SelfEnable;
	unsigned char RtsRate;
	unsigned int	reduction_level = 0;

#ifdef MP_TEST
	if (OPMODE & WIFI_MP_STATE) {
		if (is_VHT_rate(txcfg->tx_rate)) {
			txRate = txcfg->tx_rate - VHT_RATE_ID;
			txRate += 44;
		} else if (is_MCS_rate(txcfg->tx_rate)) {	// HT rates
			txRate = txcfg->tx_rate - HT_RATE_ID;
			txRate += 12;
		} else{
			txRate = get_rate_index_from_ieee_value((UINT8)txcfg->tx_rate);
		}

		if (priv->pshare->is_40m_bw == 2) {
			pdesc->Dword5 |= set_desc((0 << TXdesc_92E_DataScSHIFT) | (0 << TXdesc_92E_RtsScSHIFT));
			pdesc->Dword5 |= set_desc(0x2 << TXdesc_92E_DataBwSHIFT);

			if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M)
				n_txshort = 1;
		} else if (priv->pshare->is_40m_bw == 1) {	
			pdesc->Dword5 |= set_desc((0 << TXdesc_92E_DataScSHIFT) | (0 << TXdesc_92E_RtsScSHIFT));
			pdesc->Dword5 |= set_desc(0x1 << TXdesc_92E_DataBwSHIFT);

			if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M)
				n_txshort = 1;
		} else {
			pdesc->Dword5 |= set_desc((0 << TXdesc_92E_DataScSHIFT) | (0 << TXdesc_92E_RtsScSHIFT));
			pdesc->Dword5 |= set_desc(0 << TXdesc_92E_DataBwSHIFT);
			if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M)
				n_txshort = 1;
		}

		if (txcfg->retry) {	
			pdesc->Dword4 |= set_desc(TXdesc_92E_RtyLmtEn);
            		pdesc->Dword4 |= set_desc((txcfg->retry  & TXdesc_92E_DataRtyLmtMask) << TXdesc_92E_DataRtyLmtSHIFT);
		}

		pdesc->Dword4 |= set_desc((txRate & TXdesc_92E_DataRateMask) << TXdesc_92E_DataRateSHIFT);
		
		
		if (priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G)
			pdesc->Dword4 |= set_desc((4 & TX_RtsRateMask_8812) << TX_RtsRateSHIFT_8812);

		return;
	}
#endif


	if (is_MCS_rate(txcfg->tx_rate))	// HT rates
	{
		if( txcfg->tx_rate >= VHT_RATE_ID)
		{
			txRate = (txcfg->tx_rate - VHT_RATE_ID) + 44;
		}
		else
		{
			txRate = (txcfg->tx_rate - HT_RATE_ID) + 12;
		}

		if (priv->pshare->is_40m_bw==2)
		{
//			get_txsc_AC(priv, priv->pmib->dot11RFEntry.dot11channel);
			if(txcfg->pstat && is_VHT_rate(txcfg->tx_rate) && (txcfg->pstat->tx_bw == HT_CHANNEL_WIDTH_80 
				&& (is_auto_rate(priv, txcfg->pstat) || is_fixedVHTTxRate(priv, txcfg->pstat))
			) )
			{
				pdesc->Dword5 |= set_desc((0 << TXdesc_92E_DataScSHIFT) | (0 << TXdesc_92E_RtsScSHIFT));
				pdesc->Dword5 |= set_desc(0x2 << TXdesc_92E_DataBwSHIFT);

				if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M && //todo, add shortGI 80M option
					txcfg->pstat && (txcfg->pstat->vht_cap_buf.vht_cap_info & cpu_to_le32(BIT(5))))
						n_txshort = 1;
			}
			else if(txcfg->pstat && is_MCS_rate(txcfg->tx_rate) && (txcfg->pstat->tx_bw >= HT_CHANNEL_WIDTH_20_40
					&& (is_auto_rate(priv, txcfg->pstat) || is_fixedMCSTxRate(priv, txcfg->pstat))		
				) )
			{
				pdesc->Dword5 |= set_desc((priv->pshare->txsc_40 << TXdesc_92E_DataScSHIFT) | (priv->pshare->txsc_40 << TXdesc_92E_RtsScSHIFT));
				pdesc->Dword5 |= set_desc(0x1 << TXdesc_92E_DataBwSHIFT);

				if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M &&
					txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_40M_)))
						n_txshort = 1;
			}
			else
			{
				pdesc->Dword5 |= set_desc((priv->pshare->txsc_20 << TXdesc_92E_DataScSHIFT) | (priv->pshare->txsc_20 << TXdesc_92E_RtsScSHIFT));
				
				if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M &&
					txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_)))
						n_txshort = 1;
			}
		}
		else
		if (priv->pshare->is_40m_bw) {
			if (txcfg->pstat && (txcfg->pstat->tx_bw == HT_CHANNEL_WIDTH_20_40)
#ifdef WIFI_11N_2040_COEXIST
				&& !(COEXIST_ENABLE && (((OPMODE & WIFI_AP_STATE) && 
				(priv->bg_ap_timeout || orForce20_Switch20Map(priv)
				))
#ifdef CLIENT_MODE
				|| ((OPMODE & WIFI_STATION_STATE) && priv->coexist_connection && 
				(txcfg->pstat->ht_ie_len) && !(txcfg->pstat->ht_ie_buf.info0 & _HTIE_STA_CH_WDTH_))
#endif
				))
#endif

				) {
					
				pdesc->Dword5 |= set_desc((1 << TXdesc_92E_DataBwSHIFT) | (3 << TXdesc_92E_DataScSHIFT));

				{
					if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M &&
						txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_40M_)))
						n_txshort = 1;
				}
			}
			else {
				if (priv->pshare->offset_2nd_chan == HT_2NDCH_OFFSET_BELOW)
					pdesc->Dword5 |= set_desc((2 << TXdesc_92E_DataScSHIFT) | (2 << TXdesc_92E_RtsScSHIFT));
				else
					pdesc->Dword5 |= set_desc((1 << TXdesc_92E_DataScSHIFT) | (1 << TXdesc_92E_RtsScSHIFT));

				{
					if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M &&
						txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_)))
						n_txshort = 1;
				}
			}
		} else {
			{
				if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M &&
					txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_)))
					n_txshort = 1;
			}
		}

		
		if (
			( txcfg->pstat && 
			((txcfg->pstat->aggre_mthd == AGGRE_MTHD_MPDU_AMSDU) || (txcfg->pstat->aggre_mthd == AGGRE_MTHD_MPDU))
			&& txcfg->aggre_en )	|| 
			((txcfg->aggre_en >= FG_AGGRE_MPDU) && (txcfg->aggre_en <= FG_AGGRE_MPDU_BUFFER_LAST)) 
		){
			int TID = ((struct sk_buff *)txcfg->pframe)->cb[1];
			if (txcfg->pstat->ADDBA_ready[TID] && !txcfg->pstat->low_tp_disable_ampdu) {


					pdesc->Dword2 |= set_desc(TXdesc_92E_AggEn);


				/*
				 * assign aggr size
				 */

				// assign aggr density
				if (txcfg->privacy) {
					//8812_11n_iot, set TxAmpduDsty=7 for 20M WPA2
					if ((priv->pmib->dot11RFEntry.phyBandSelect == PHY_BAND_5G) && (!priv->pshare->is_40m_bw))
						pdesc->Dword2 |= set_desc(7 << TX_AmpduDstySHIFT);	// according to DWA-160 A2
					else
						pdesc->Dword2 |= set_desc(5 << TX_AmpduDstySHIFT);	// according to WN111v2
				}
				else {
					pdesc->Dword2 |= set_desc(((txcfg->pstat->ht_cap_buf.ampdu_para & _HTCAP_AMPDU_SPC_MASK_) >> _HTCAP_AMPDU_SPC_SHIFT_) << TX_AmpduDstySHIFT);
				}
				if(	((txcfg->pstat->current_tx_rate >= _MCS0_RATE_) && (txcfg->pstat->current_tx_rate <= _MCS2_RATE_)) 
				||((txcfg->pstat->current_tx_rate >= _MCS8_RATE_) && (txcfg->pstat->current_tx_rate <= _MCS10_RATE_)) 
#ifdef RTK_AC_SUPPORT					
				||((txcfg->pstat->current_tx_rate >= _NSS1_MCS0_RATE_) && (txcfg->pstat->current_tx_rate <= _NSS1_MCS2_RATE_)) 
				||((txcfg->pstat->current_tx_rate >= _NSS2_MCS0_RATE_) && (txcfg->pstat->current_tx_rate <= _NSS2_MCS2_RATE_))
#endif					
				)
					pdesc->Dword3 |= set_desc( (0x02 & TXdesc_92E_MAX_AGG_NUMMask)<< TXdesc_92E_MAX_AGG_NUMSHIFT);		// MAX_AGG_NUM=4
			}
			//set Break
			if((txcfg->q_num >=1 && txcfg->q_num <=4)){
				if((txcfg->pstat != priv->pshare->CurPstat[txcfg->q_num-1])) {

						pdesc->Dword2 |= set_desc(TXdesc_92E_BK);
					priv->pshare->CurPstat[txcfg->q_num-1] = txcfg->pstat;
				}				
			} else {

					pdesc->Dword2 |= set_desc(TXdesc_92E_BK);
			}
		}

		// for STBC
		if (priv->pmib->dot11nConfigEntry.dot11nSTBC &&	txcfg->pstat )	// 2012 10 31  for test
		{
			if((txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_RX_STBC_CAP_)) 
				|| (txcfg->pstat->vht_cap_buf.vht_cap_info & cpu_to_le32(_VHTCAP_RX_STBC_CAP_))){

#ifdef BEAMFORMING_SUPPORT
				u1Byte					Idx = 0;
				PRT_BEAMFORMING_ENTRY	pEntry; 
				pEntry = Beamforming_GetEntryByMacId(priv, txcfg->pstat->aid, &Idx);
				if(pEntry == NULL)
#endif 
				pdesc->Dword5 |= set_desc(1 << TXdesc_92E_DataStbcSHIFT);

			}

		}
	}
	else	// legacy rate
	{
		txRate = get_rate_index_from_ieee_value((UINT8)txcfg->tx_rate);
		if (is_CCK_rate(txcfg->tx_rate) && (txcfg->tx_rate != 2)) {
			if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11G) &&
					(priv->pmib->dot11ErpInfo.longPreambleStaNum > 0))
				; // txfw->txshort = 0
			else {
				if (txcfg->pstat)
					bg_txshort = (priv->pmib->dot11RFEntry.shortpreamble) &&
									(txcfg->pstat->useShortPreamble);
				else
					bg_txshort = priv->pmib->dot11RFEntry.shortpreamble;
			}
		}

		if (priv->pshare->is_40m_bw==2) {
//			get_txsc_AC(priv, priv->pmib->dot11RFEntry.dot11channel);
			pdesc->Dword5 |= set_desc((priv->pshare->txsc_20 << TXdesc_92E_DataScSHIFT) | (priv->pshare->txsc_20  << TXdesc_92E_RtsScSHIFT));
		}
		else
		if (priv->pshare->is_40m_bw) {
			if (priv->pshare->offset_2nd_chan == HT_2NDCH_OFFSET_BELOW)
				pdesc->Dword5 |= set_desc((2 << TXdesc_92E_DataScSHIFT) | (2 << TXdesc_92E_RtsScSHIFT));
			else
				pdesc->Dword5 |= set_desc((1 << TXdesc_92E_DataScSHIFT) | (1 << TXdesc_92E_RtsScSHIFT));	
		}

		if (bg_txshort)
			pdesc->Dword5 |= set_desc(TXdesc_92E_DataShort);
			
	}

	if(AC_SIGMA_MODE != AC_SIGMA_NONE) //for 11ac logo
	if(txcfg->pstat)
	{
		txcfg->pstat->no_rts=0;
	}

	if (txcfg->need_ack) { // unicast
		if (frag_idx == 0)
			RtsCheck(priv, txcfg, &bRtsEnable, &bCts2SelfEnable, &bHwRts, &bErpProtect, &bNProtect);
	}

	RtsRate = find_rts_rate(priv, txcfg->tx_rate, bErpProtect);
	if(bRtsEnable && CheckCts2SelfEnable(RtsRate))
	{
		bRtsEnable = FALSE;
		bCts2SelfEnable = TRUE;
	}

	if(txcfg->pstat) //8812_11n_iot, only vht clnt support cca_rts, //for 11ac logo
		RtsCheckAC(priv, txcfg, &bRtsEnable, &bCts2SelfEnable, &bHwRts);

	if(bRtsEnable)
		pdesc->Dword3 |= set_desc(TX_RtsEn);	
	if(bCts2SelfEnable)
		pdesc->Dword3 |= set_desc(TX_CTS2Self);	
	if(bHwRts)
		pdesc->Dword3 |= set_desc(TX_HwRtsEn);	

	if (bRtsEnable || bCts2SelfEnable ) {

			unsigned int rtsTxRateIdx   = get_rate_index_from_ieee_value(RtsRate);
			if (bErpProtect) {
				unsigned char  rtsShort = 0;
				if (is_CCK_rate(RtsRate) && (RtsRate != 2)) {
					if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11G) &&
							(priv->pmib->dot11ErpInfo.longPreambleStaNum > 0))
						rtsShort = 0; // do nothing
					else {
						if (txcfg->pstat)
							rtsShort = (priv->pmib->dot11RFEntry.shortpreamble) &&
											(txcfg->pstat->useShortPreamble);
						else
							rtsShort = priv->pmib->dot11RFEntry.shortpreamble;
					}
				}
				pdesc->Dword5 |= (rtsShort == 1)? set_desc(TXdesc_92E_RtsShort): 0;
				
			} 
			pdesc->Dword4 |= set_desc((rtsTxRateIdx & TXdesc_92E_RtsRateMask) << TXdesc_92E_RtsRateSHIFT);
			pdesc->Dword4 |= set_desc((0xf & TXdesc_92E_RtsRateFBLmtMask) << TXdesc_92E_RtsRateFBLmtSHIFT);
				
	}

	if(n_txshort == 1 && txcfg->pstat->sta_in_firmware == 1)
		pdesc->Dword5 |= set_desc(TXdesc_92E_DataShort);		
	

#ifdef DRVMAC_LB
	if (priv->pmib->miscEntry.drvmac_lb && (priv->pmib->miscEntry.lb_mlmp == 4)) {
		txRate = rate_select;
		if (rate_select++ > 0x1b)
			rate_select = 0;

		pdesc->Dword3 |= set_desc(TXdesc_92E_DisDataFB|TXdesc_92E_DisRtsFB|TXdesc_92E_UseRate);
	}
#endif


	if((priv->pshare->rf_ft_var.txforce != 0xff)
#ifdef BEAMFORMING_SUPPORT
		&& (!txcfg->ndpa) 
#endif
	) {
				pdesc->Dword3 |= set_desc(TXdesc_92E_DisDataFB|TXdesc_92E_DisRtsFB|TXdesc_92E_UseRate);
				pdesc->Dword4 |= set_desc((priv->pshare->rf_ft_var.txforce & TXdesc_92E_DataRateMask) << TXdesc_92E_DataRateSHIFT);
	}
	else
	pdesc->Dword4 |= set_desc((txRate & TXdesc_92E_DataRateMask) << TXdesc_92E_DataRateSHIFT);


#if 1
	if (priv->pshare->rf_ft_var.rts_init_rate) {
		pdesc->Dword4 &= set_desc(~(TX_RtsRateMask_8812 << TX_RtsRateSHIFT_8812));
		pdesc->Dword4 |= set_desc(((priv->pshare->rf_ft_var.rts_init_rate) & TX_RtsRateMask_8812) << TX_RtsRateSHIFT_8812);
	}		
	if ((priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G) && 
		(TX_RtsRateMask_8812&(get_desc(pdesc->Dword4)>>TX_RtsRateSHIFT_8812)) <4	)
		pdesc->Dword4 |= set_desc((4 & TX_RtsRateMask_8812) << TX_RtsRateSHIFT_8812);
#else		
	if (priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G)
		pdesc->Dword4 |= set_desc((4 & TX_RtsRateMask_8812) << TX_RtsRateSHIFT_8812);

#endif

	if (txcfg->need_ack) {
		// give retry limit to management frame
		if (txcfg->q_num == MANAGE_QUE_NUM) {
			pdesc->Dword4 |= set_desc(TXdesc_92E_RtyLmtEn);
			
			if (GetFrameSubType(txcfg->phdr) == WIFI_PROBERSP) {
				;	// 0 no need to set
			}
#ifdef WDS
			else if ((GetFrameSubType(txcfg->phdr) == WIFI_PROBEREQ) && (txcfg->pstat->state & WIFI_WDS)) {
				pdesc->Dword4 |= set_desc((2 & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
			}
#endif
			else {
				pdesc->Dword4 |= set_desc((6 & TXdesc_92E_DataRtyLmtMask) << TXdesc_92E_DataRtyLmtSHIFT);
			}
		}
#ifdef WDS
		else if (txcfg->wdsIdx >= 0) {
			if (txcfg->pstat->rx_avarage == 0) {
				pdesc->Dword4 |= set_desc(TX_RtyLmtEn);
				pdesc->Dword4 |= set_desc((3 & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
			}
		}
#endif
		else if (txcfg->pstat->rssi < 30) {
			pdesc->Dword4 |= set_desc(TXdesc_92E_RtyLmtEn);
    		pdesc->Dword4 |= set_desc((0x07 & TXdesc_92E_DataRtyLmtMask) << TXdesc_92E_DataRtyLmtSHIFT);
		}
		else if (is_MCS_rate(txcfg->pstat->current_tx_rate) && (txcfg->pstat->IOTPeer==HT_IOT_PEER_INTEL) && (txcfg->pstat->retry_inc)
			&& !(txcfg->pstat->leave) && priv->pshare->intel_rty_lmt) {			
			pdesc->Dword4 |= set_desc(TXdesc_92E_RtyLmtEn);
			pdesc->Dword4 |= set_desc((priv->pshare->intel_rty_lmt & TXdesc_92E_DataRtyLmtMask) << TXdesc_92E_DataRtyLmtSHIFT);
		}

		else if ((txcfg->pstat->IOTPeer==HT_IOT_PEER_BROADCOM) && (txcfg->pstat->retry_inc) && !(txcfg->pstat->leave)) {
			pdesc->Dword4 |= set_desc(TXdesc_92E_RtyLmtEn);
    		pdesc->Dword4 |= set_desc((0x20 & TXdesc_92E_DataRtyLmtMask) << TXdesc_92E_DataRtyLmtSHIFT);
        }

		if (priv->pshare->rf_ft_var.tx_pwr_ctrl && txcfg->pstat && (txcfg->fr_type == _SKB_FRAME_TYPE_)) {
			if (txcfg->pstat->hp_level == 1) {
				reduction_level = 2;
				pdesc->Dword5 |= set_desc((reduction_level & TXdesc_8812_TxPwrOffetMask) << TXdesc_8812_TxPwrOffetSHIFT);
			}
		}	

#ifdef BEAMFORMING_SUPPORT
		if(txcfg->ndpa) {
			unsigned char *pFrame = (unsigned char*)txcfg->phdr;
			if(IsCtrlNDPA(pFrame) || IsMgntNDPA(pFrame)) {
				//SET_TX_DESC_DATA_RETRY_LIMIT_8812(pDesc, 5);
				//SET_TX_DESC_RETRY_LIMIT_ENABLE_8812(pDesc, 1);
				pdesc->Dword4 &= set_desc(~(TXdesc_92E_DataRtyLmtMask << TXdesc_92E_DataRtyLmtSHIFT));									
	            pdesc->Dword4 |= set_desc(TXdesc_92E_RtyLmtEn|((0x05 & TXdesc_92E_DataRtyLmtMask) << TXdesc_92E_DataRtyLmtSHIFT));	

					if(IsMgntNDPA(pFrame))		//0xe0
					{
						pdesc->Dword3 |= set_desc((1 &TXdesc_92E_NDPAMASK)<<TXdesc_92E_NDPASHIFT);
					}	
					else		// 0x54
					{
						if(!IS_TEST_CHIP(priv))
						{
							if ((txcfg->pstat->WirelessMode & WIRELESS_MODE_AC_5G) && (txcfg->pstat->IOTPeer!=HT_IOT_PEER_BROADCOM))
								pdesc->Dword3 |= set_desc((2 &TXdesc_92E_NDPAMASK)<<TXdesc_92E_NDPASHIFT);
							else
								pdesc->Dword3 |= set_desc((1 &TXdesc_92E_NDPAMASK)<<TXdesc_92E_NDPASHIFT);
						}
						else
						{
							pdesc->Dword3 |= set_desc((1 &TXdesc_92E_NDPAMASK)<<TXdesc_92E_NDPASHIFT);				
						}
					}	
	//				panic_printk("LINE:%d, %x\n", __LINE__, get_desc(pdesc->Dword3));
			}
		} else
#endif

		{
			if((priv->pmib->dot11nConfigEntry.dot11nLDPC == 1) && (txcfg->pstat) && (!txcfg->pstat->disable_ldpc) &&
			((txcfg->pstat->ht_cap_len && cpu_to_le16(txcfg->pstat->ht_cap_buf.ht_cap_info) & _HTCAP_SUPPORT_RX_LDPC_) ||
			(txcfg->pstat->vht_cap_len && (cpu_to_le32(txcfg->pstat->vht_cap_buf.vht_cap_info) & BIT(RX_LDPC_E))))
			) {
				pdesc->Dword5 |= set_desc(TXdesc_92E_DataLDPC);	
			}
		}
	}

	if (priv->pmib->dot11RFEntry.txpwr_reduction) {
		if (priv->pmib->dot11RFEntry.txpwr_reduction <= 3) {
			if (reduction_level < priv->pmib->dot11RFEntry.txpwr_reduction) {
				reduction_level = priv->pmib->dot11RFEntry.txpwr_reduction;
				pdesc->Dword5 &= set_desc(~((TXdesc_8812_TxPwrOffetMask) << TXdesc_8812_TxPwrOffetSHIFT));
				pdesc->Dword5 |= set_desc((reduction_level & TXdesc_8812_TxPwrOffetMask) << TXdesc_8812_TxPwrOffetSHIFT);
			}
		}
	}
}

#endif

#ifdef CONFIG_WLAN_HAL
static void 
rtl88XX_fill_fwinfo(
    struct rtl8192cd_priv   *priv, 
    struct tx_insn          *txcfg, 
    unsigned int            frag_idx, 
    PTX_DESC_DATA_88XX      pdesc_data
)
{
    char n_txshort = 0, bg_txshort = 0;
    int erp_protection = 0, n_protection = 0;
    unsigned char rate;
    unsigned char txRate = 0;
#ifdef DRVMAC_LB
    static unsigned int rate_select = 0;
#endif
//	BOOLEAN			bRtsEnable;
	BOOLEAN			bErpProtect;
	BOOLEAN			bNProtect;
//	BOOLEAN			bHwRts;
//	BOOLEAN			bCts2SelfEnable;
	unsigned char RtsRate;

#ifdef MP_TEST
    if (OPMODE & WIFI_MP_STATE) {
    #ifdef RTK_AC_SUPPORT
        if (is_VHT_rate(txcfg->tx_rate)) {
            txRate = (txcfg->tx_rate - VHT_RATE_ID) + 44;
        } 
        else 
    #endif            
        if (is_MCS_rate(txcfg->tx_rate)) {  // HT rates
            txRate = (txcfg->tx_rate - HT_RATE_ID) + 12;
        } 
        else{
            txRate = get_rate_index_from_ieee_value((UINT8)txcfg->tx_rate);
        }

        if (priv->pshare->is_40m_bw == 2) {
            pdesc_data->dataSC  = 0x0;
            pdesc_data->RTSSC   = 0x0;
            pdesc_data->dataBW  = 0x2;

            if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M)
                n_txshort = 1;
        } 
        else if (priv->pshare->is_40m_bw == 1) {
            pdesc_data->dataSC  = 0x0;
            pdesc_data->RTSSC   = 0x0;
            pdesc_data->dataBW  = 0x1;

            if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M)
                n_txshort = 1;
        } 
        else {
            pdesc_data->dataSC  = 0x0;
            pdesc_data->RTSSC   = 0x0;
            pdesc_data->dataBW  = 0x0;
            
            if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M)
                n_txshort = 1;
        }

        if (txcfg->retry) { 
            pdesc_data->rtyLmtEn    = TRUE;
            pdesc_data->dataRtyLmt  = txcfg->retry;
        }

        pdesc_data->dataRate = txRate;
        if (priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G)
            pdesc_data->RTSRate = 4;

        return;
    }
#endif

    if (is_MCS_rate(txcfg->tx_rate))    // HT rates
    {
#ifdef RTK_AC_SUPPORT
        if(is_VHT_rate(txcfg->tx_rate))
        {
            txRate = (txcfg->tx_rate - VHT_RATE_ID) + 44;
        }
        else
#endif
        {
            txRate = (txcfg->tx_rate - HT_RATE_ID) + 12;
        }

#ifdef RTK_AC_SUPPORT
        if (priv->pshare->is_40m_bw==2)
        {
            if(txcfg->pstat && (txcfg->pstat->tx_bw == HT_CHANNEL_WIDTH_80
                && (is_auto_rate(priv, txcfg->pstat) || is_fixedVHTTxRate(priv, txcfg->pstat))
            ) )
            {
                pdesc_data->dataSC = 0;
                pdesc_data->RTSSC = 0;
                pdesc_data->dataBW = 2;                
                if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M && //todo, add shortGI 80M option
                    txcfg->pstat && (txcfg->pstat->vht_cap_buf.vht_cap_info & cpu_to_le32(BIT(5))))
                        n_txshort = 1;
            } // TODO: Pedro, in 8812: is_MCS_rate(txcfg->tx_rate)
            else if(txcfg->pstat && (txcfg->pstat->tx_bw >= HT_CHANNEL_WIDTH_20_40
                    && (is_auto_rate(priv, txcfg->pstat) || is_fixedMCSTxRate(priv, txcfg->pstat))        
                ) )
            {
                pdesc_data->dataSC = priv->pshare->txsc_40;
                pdesc_data->RTSSC  = priv->pshare->txsc_40;
                pdesc_data->dataBW = 1;
                if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M &&
                    txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_40M_)))
                        n_txshort = 1;
            }
            else
            {
                pdesc_data->dataSC = priv->pshare->txsc_20;
                pdesc_data->RTSSC  = priv->pshare->txsc_20;
                if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M &&
                    txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_)))
                        n_txshort = 1;
            }
        }
        else
#endif
        if (priv->pshare->is_40m_bw) {
            if (txcfg->pstat && (txcfg->pstat->tx_bw == HT_CHANNEL_WIDTH_20_40)
#ifdef WIFI_11N_2040_COEXIST
                && !(COEXIST_ENABLE && (((OPMODE & WIFI_AP_STATE) && 
                (priv->bg_ap_timeout || orForce20_Switch20Map(priv)
                ))
#ifdef CLIENT_MODE
                || ((OPMODE & WIFI_STATION_STATE) && priv->coexist_connection && 
                (txcfg->pstat->ht_ie_len) && !(txcfg->pstat->ht_ie_buf.info0 & _HTIE_STA_CH_WDTH_))
#endif
                ))
#endif

                ) {
                pdesc_data->dataBW = 1;
                pdesc_data->dataSC = 3;
                {
                    if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M &&
                        txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_40M_)))
                        n_txshort = 1;
                }
            }
            else {
                if (priv->pshare->offset_2nd_chan == HT_2NDCH_OFFSET_BELOW) {
                    pdesc_data->dataSC = 2;
                    pdesc_data->RTSSC  = 2;
                } else {
                    pdesc_data->dataSC = 1;
                    pdesc_data->RTSSC  = 1;
                }

                {
                    if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M &&
                        txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_)))
                        n_txshort = 1;
                }
            }
        } else {
            {
                if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M &&
                    txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_)))
                    n_txshort = 1;
            }
        }

        
        if (
            ( AMSDU_ENABLE && AMPDU_ENABLE && txcfg->aggre_en ) || 
            ((txcfg->aggre_en >= FG_AGGRE_MPDU) && (txcfg->aggre_en <= FG_AGGRE_MPDU_BUFFER_LAST)) 
        ){
            int TID = ((struct sk_buff *)txcfg->pframe)->cb[1];
            if (txcfg->pstat->ADDBA_ready[TID] && !txcfg->pstat->low_tp_disable_ampdu) {
                pdesc_data->aggEn = TRUE;

                /*
                 * assign aggr size
                 */

#ifdef CONFIG_WLAN_HAL
                // TODO: �o�̬O���DAC chip ???
                // TODO: check �O�_���b��L��m..
#else
                if(GET_CHIP_VER(priv) != VERSION_8812E)
                {
                     if (priv->pshare->rf_ft_var.diffAmpduSz) {
                        pdesc->Dword6 |= set_desc((txcfg->pstat->diffAmpduSz & 0xffff) << TX_MCS1gMaxSHIFT | TX_UseMaxLen);
                        
                        if (GET_CHIP_VER(priv)!=VERSION_8812E)     
                        pdesc->Dword7 |= set_desc(txcfg->pstat->diffAmpduSz & 0xffff0000);
                     }  
                }
#endif // CONFIG_WLAN_HAL

                // assign aggr density
                if (txcfg->privacy) {
#if 1   // TODO: for test two STA TP
                    pdesc_data->ampduDensity = 7;
#else
                    pdesc_data->ampduDensity = 5;
#endif
                }
                else {
#if 0   // TODO: for test two STA TP
                    pdesc_data->ampduDensity = 5;
#else
					pdesc_data->ampduDensity = ((txcfg->pstat->ht_cap_buf.ampdu_para & _HTCAP_AMPDU_SPC_MASK_) >> _HTCAP_AMPDU_SPC_SHIFT_);
#endif
                }
				pdesc_data->maxAggNum = ((1<<(txcfg->pstat->ht_cap_buf.ampdu_para & 0x03))*5)>>1;
				if(	((txcfg->pstat->current_tx_rate >= _MCS0_RATE_) && (txcfg->pstat->current_tx_rate <= _MCS2_RATE_)) 
					||((txcfg->pstat->current_tx_rate >= _MCS8_RATE_) && (txcfg->pstat->current_tx_rate <= _MCS10_RATE_)) 
#ifdef RTK_AC_SUPPORT						
					||((txcfg->pstat->current_tx_rate >= _NSS1_MCS0_RATE_) && (txcfg->pstat->current_tx_rate <= _NSS1_MCS2_RATE_)) 
					||((txcfg->pstat->current_tx_rate >= _NSS2_MCS0_RATE_) && (txcfg->pstat->current_tx_rate <= _NSS2_MCS2_RATE_))
#endif						
				)
					pdesc_data->maxAggNum = 2;		
            }
            //set Break
            if((txcfg->q_num >=1 && txcfg->q_num <=4)){
                if((txcfg->pstat != priv->pshare->CurPstat[txcfg->q_num-1])) {
                    pdesc_data->bk = TRUE;
                    priv->pshare->CurPstat[txcfg->q_num-1] = txcfg->pstat;
                }               
            } else {
                pdesc_data->bk = TRUE;
            }
        }

        // for STBC
        if (priv->pmib->dot11nConfigEntry.dot11nSTBC && (txcfg->pstat) &&
			((txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_RX_STBC_CAP_)) 
#ifdef RTK_AC_SUPPORT			
				|| (txcfg->pstat->vht_cap_buf.vht_cap_info & cpu_to_le32(_VHTCAP_RX_STBC_CAP_))
#endif
		)){
#ifdef BEAMFORMING_SUPPORT
			u1Byte					Idx = 0;
			PRT_BEAMFORMING_ENTRY	pEntry; 
			pEntry = Beamforming_GetEntryByMacId(priv, txcfg->pstat->aid, &Idx);
			if(pEntry == NULL)
#endif
			if((get_rf_mimo_mode(priv) == MIMO_2T2R) || (get_rf_mimo_mode(priv) == MIMO_3T3R))
           		pdesc_data->dataStbc = 1;
        }
		// LDPC
#ifdef BEAMFORMING_SUPPORT
		if(!txcfg->ndpa) 
#endif		
		if((priv->pmib->dot11nConfigEntry.dot11nLDPC) && (txcfg->pstat) && (!txcfg->pstat->disable_ldpc) &&
		((txcfg->pstat->ht_cap_len && cpu_to_le16(txcfg->pstat->ht_cap_buf.ht_cap_info) & _HTCAP_SUPPORT_RX_LDPC_)
#ifdef RTK_AC_SUPPORT		
		||	(txcfg->pstat->vht_cap_len && (cpu_to_le32(txcfg->pstat->vht_cap_buf.vht_cap_info) & BIT(RX_LDPC_E)))
#endif		
		)) {		
				pdesc_data->dataLdpc = 1;
		}
    }
    else    // legacy rate
    {
        txRate = get_rate_index_from_ieee_value((UINT8)txcfg->tx_rate);
        if (is_CCK_rate(txcfg->tx_rate) && (txcfg->tx_rate != 2)) {
            if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11G) &&
                    (priv->pmib->dot11ErpInfo.longPreambleStaNum > 0))
                ; // txfw->txshort = 0
            else {
                if (txcfg->pstat)
                    bg_txshort = (priv->pmib->dot11RFEntry.shortpreamble) &&
                                    (txcfg->pstat->useShortPreamble);
                else
                    bg_txshort = priv->pmib->dot11RFEntry.shortpreamble;
            }
        }

#ifdef RTK_AC_SUPPORT
        if (priv->pshare->is_40m_bw==2) {
            pdesc_data->dataSC = priv->pshare->txsc_20;
            pdesc_data->RTSSC  = priv->pshare->txsc_20;
        }
        else
#endif
        if (priv->pshare->is_40m_bw) {
            if (priv->pshare->offset_2nd_chan == HT_2NDCH_OFFSET_BELOW) {
                pdesc_data->dataSC = 2;
                pdesc_data->RTSSC  = 2;
            } else {
                pdesc_data->dataSC = 1;
                pdesc_data->RTSSC  = 1;
            }
        }

        if (bg_txshort) {
            pdesc_data->dataShort = 1;
        }
            
    }
	
#if defined(CONFIG_WLAN_HAL_8192EE)
	if (GET_CHIP_VER(priv)==VERSION_8192E) {
		if( priv->pmib->dot11RFEntry.bcn2path )
			pdesc_data->TXAnt = 1; // use path A for 1ss CCK rate tx
		else if( (txcfg->pstat) && (txcfg->pstat->IOTPeer==HT_IOT_PEER_BROADCOM)&& (txcfg->pstat->ratr_idx == 1))
			pdesc_data->TXAnt = 1;
		else if( priv->pmib->dot11RFEntry.tx2path )
			pdesc_data->TXAnt = 3;
		else
			pdesc_data->TXAnt = 1;
	}
#endif

    if (txcfg->need_ack) { // unicast
        if (frag_idx == 0)
		RtsCheck(priv, txcfg, &pdesc_data->RTSEn, &pdesc_data->CTS2Self, &pdesc_data->HWRTSEn, &bErpProtect, &bNProtect);
                    }
	RtsRate = find_rts_rate(priv, txcfg->tx_rate, bErpProtect);
	if(pdesc_data->RTSEn && CheckCts2SelfEnable(RtsRate))
	{
		pdesc_data->RTSEn = FALSE;
		pdesc_data->CTS2Self = TRUE;
	}

    if (pdesc_data->CTS2Self || pdesc_data->RTSEn) 
    {
            unsigned int rtsTxRateIdx   = get_rate_index_from_ieee_value(RtsRate);
            if (bErpProtect) {
                unsigned char  rtsShort = 0;
                if (is_CCK_rate(RtsRate) && (RtsRate != 2)) {
                    if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11G) &&
                            (priv->pmib->dot11ErpInfo.longPreambleStaNum > 0))
                        rtsShort = 0; // do nothing
                    else {
                        if (txcfg->pstat)
                            rtsShort = (priv->pmib->dot11RFEntry.shortpreamble) &&
                                            (txcfg->pstat->useShortPreamble);
                        else
                            rtsShort = priv->pmib->dot11RFEntry.shortpreamble;
                    }
                }
                pdesc_data->RTSShort = (rtsShort == 1) ? TRUE : FALSE;                
            } 

            pdesc_data->RTSRate = rtsTxRateIdx;
            pdesc_data->RTSRateFBLmt = 0xf;

    }

    if(n_txshort == 1 && txcfg->pstat->sta_in_firmware == 1)
    {
        pdesc_data->dataShort = 1;
    }
    

#ifdef DRVMAC_LB
    if (priv->pmib->miscEntry.drvmac_lb && (priv->pmib->miscEntry.lb_mlmp == 4)) {
        txRate = rate_select;
        if (rate_select++ > 0x1b)
            rate_select = 0;

        pdesc_data->disDataFB = TRUE;
        pdesc_data->disRTSFB  = TRUE;
        pdesc_data->useRate   = TRUE;
    }
#endif

	if((priv->pshare->rf_ft_var.txforce != 0xff) 
#ifdef BEAMFORMING_SUPPORT
		&& (!txcfg->ndpa) 
#endif
	){
        pdesc_data->useRate  = TRUE;
        pdesc_data->dataRate = priv->pshare->rf_ft_var.txforce;       
	} else {
        pdesc_data->dataRate = txRate;
    }


#if 1
    if (priv->pshare->rf_ft_var.rts_init_rate) {
        pdesc_data->RTSRate = priv->pshare->rf_ft_var.rts_init_rate;
    }      
    if ((priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G) && 
        (pdesc_data->RTSRate)<4) {
        pdesc_data->RTSRate = 4;
    }
#else		
    if (priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G)
        pdesc->Dword4 |= set_desc((4 & TX_RtsRateMask_8812) << TX_RtsRateSHIFT_8812);

#endif

    if (txcfg->need_ack) {
        // give retry limit to management frame
        if (txcfg->q_num == MANAGE_QUE_NUM) {
            pdesc_data->rtyLmtEn = TRUE;
			// TODO: ???? for hw tx sc... how to do
            if (GetFrameSubType(txcfg->phdr) == WIFI_PROBERSP) {
                ;   // 0 no need to set
            }
#ifdef WDS
            else if ((GetFrameSubType(txcfg->phdr) == WIFI_PROBEREQ) && (txcfg->pstat->state & WIFI_WDS)) {
                pdesc_data->dataRtyLmt = 2;
            }
#endif
            else {
                pdesc_data->dataRtyLmt = 6;
            }
        }
#ifdef WDS
        else if (txcfg->wdsIdx >= 0) {
            if (txcfg->pstat->rx_avarage == 0) {
                pdesc_data->rtyLmtEn = TRUE;
                pdesc_data->dataRtyLmt = 3;
            }
        }
#endif
        else if(txcfg->pstat)
        {
			if (txcfg->pstat->rssi < 30) {
	            pdesc_data->rtyLmtEn = TRUE;
	            pdesc_data->dataRtyLmt = 0x07;			
			} else if (is_MCS_rate(txcfg->pstat->current_tx_rate) && (txcfg->pstat->IOTPeer==HT_IOT_PEER_INTEL) && (txcfg->pstat->retry_inc)
				&& !(txcfg->pstat->leave) && priv->pshare->intel_rty_lmt) {        
	            pdesc_data->rtyLmtEn = TRUE;
	            pdesc_data->dataRtyLmt = priv->pshare->intel_rty_lmt;
	        } else if ((txcfg->pstat->IOTPeer==HT_IOT_PEER_BROADCOM) && (txcfg->pstat->retry_inc) && !(txcfg->pstat->leave)) {
	            pdesc_data->rtyLmtEn = TRUE;
	            pdesc_data->dataRtyLmt = 0x20;
			}
		
		}


		// High power
		if (priv->pshare->rf_ft_var.tx_pwr_ctrl && txcfg->pstat && (txcfg->fr_type == _SKB_FRAME_TYPE_)) {
			if (txcfg->pstat->hp_level == 1)
			{
#if defined(CONFIG_WLAN_HAL_8192EE) && defined(HIGH_POWER_EXT_PA)
				if (GET_CHIP_VER(priv)==VERSION_8192E && priv->pshare->rf_ft_var.use_ext_pa)
					pdesc_data->TXPowerOffset = 3; // -11 dB
				else
#endif		
				pdesc_data->TXPowerOffset = 2; // -7 dB
			}
		}

#ifdef BEAMFORMING_SUPPORT
			if(txcfg->ndpa) {
				unsigned char *pFrame = (unsigned char*)txcfg->phdr;
				if(IsCtrlNDPA(pFrame) || IsMgntNDPA(pFrame)) {
					pdesc_data->dataRtyLmt = 5;
					pdesc_data->rtyLmtEn   = TRUE;	
						if(IsMgntNDPA(pFrame))		//0xe0
						{
							pdesc_data->ndpa = 1;
						}	
						else		// 0x54
						{
							if(!IS_TEST_CHIP(priv))
							{
								pdesc_data->ndpa= 2;
							}
							else
							{
								pdesc_data->ndpa = 1;
							}
						}	
    }
}
#endif

    }

	if (priv->pmib->dot11RFEntry.txpwr_reduction) {
		if (priv->pmib->dot11RFEntry.txpwr_reduction <= 3) {
			if (pdesc_data->TXPowerOffset < priv->pmib->dot11RFEntry.txpwr_reduction) {
				pdesc_data->TXPowerOffset = priv->pmib->dot11RFEntry.txpwr_reduction;
			}
		}
	}
}
#endif // CONFIG_WLAN_HAL
#if (CONFIG_WLAN_NOT_HAL_EXIST == 1)
void rtl8192cd_fill_fwinfo(struct rtl8192cd_priv *priv, struct tx_insn* txcfg, struct tx_desc  *pdesc, unsigned int frag_idx)
{
	char n_txshort = 0, bg_txshort = 0;
	//int erp_protection = 0, n_protection = 0;
	unsigned char rate;
	unsigned char txRate = 0;
#ifdef DRVMAC_LB
	static unsigned int rate_select = 0;
#endif
	BOOLEAN			bRtsEnable = FALSE;
	BOOLEAN			bErpProtect = FALSE;
	BOOLEAN			bNProtect = FALSE;
	BOOLEAN			bHwRts = FALSE;
	BOOLEAN			bCts2SelfEnable = FALSE;
	unsigned char RtsRate;

#ifdef MP_TEST
	if (OPMODE & WIFI_MP_STATE) {
		if (is_MCS_rate(txcfg->tx_rate)) {	// HT rates
			txRate = (txcfg->tx_rate - HT_RATE_ID) + 12;
		}
		else{
			txRate = get_rate_index_from_ieee_value((UINT8)txcfg->tx_rate);
		}

		if (priv->pshare->CurrentChannelBW) {
			pdesc->Dword4 |= set_desc(TX_DataBw | (3&TX_DataScMask) << TX_DataScSHIFT);
			if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M)
				n_txshort = 1;
		}
		else {
			if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M)
				n_txshort = 1;
		}

		if (txcfg->retry)
			pdesc->Dword5 |= set_desc((txcfg->retry & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT | TX_RtyLmtEn);

		if(n_txshort == 1)
			pdesc->Dword5 |= set_desc(TX_SGI);

		pdesc->Dword5 |= set_desc((txRate & TX_DataRateMask) << TX_DataRateSHIFT);
		
#ifdef CONFIG_RTL_92D_SUPPORT
		if (priv->pmib->dot11RFEntry.phyBandSelect == PHY_BAND_5G) {
			pdesc->Dword4 |= set_desc((4 & TX_RtsRateMask) << TX_RtsRateSHIFT);
			//if (is_CCK_rate(txRate))
				//pdesc->Dword5 |= set_desc((4 & TX_DataRateMask) << TX_DataRateSHIFT);
		}
#endif

		return;
	}
#endif

	if (priv->pmib->dot11RFEntry.txbf == 1) {
		pdesc->Dword2 &= set_desc(0x03ffffff); // clear related bits
		pdesc->Dword2 |= set_desc(1 << TX_TxAntCckSHIFT); 	// Set Default CCK rate with 1T
		pdesc->Dword2 |= set_desc(1 << TX_TxAntlSHIFT); 	// Set Default Legacy rate with 1T
		pdesc->Dword2 |= set_desc(1 << TX_TxAntHtSHIFT); 	// Set Default Ht rate		
	}
	if(priv->pmib->dot11RFEntry.bcn2path){
		pdesc->Dword2 &= set_desc(0x03ffffff); // clear related bits
		pdesc->Dword2 |= set_desc(1 << TX_TxAntCckSHIFT);	// Set Default CCK rate with 1T
	}

	if (is_MCS_rate(txcfg->tx_rate))	// HT rates
	{
		txRate = (txcfg->tx_rate - HT_RATE_ID) + 12;
		
		if (priv->pmib->dot11RFEntry.txbf == 1) {
			if (txRate <= 0x12) {
				pdesc->Dword2 |= set_desc(3 << TX_TxAntHtSHIFT); // Set Ht rate < MCS6 with 2T
			}
		}

		if (priv->pshare->is_40m_bw) {
			if (txcfg->pstat && (txcfg->pstat->tx_bw == HT_CHANNEL_WIDTH_20_40)
#ifdef WIFI_11N_2040_COEXIST
				&& !(COEXIST_ENABLE && (((OPMODE & WIFI_AP_STATE) && 
				(priv->bg_ap_timeout || orForce20_Switch20Map(priv)
				))
#ifdef CLIENT_MODE
				|| ((OPMODE & WIFI_STATION_STATE) && priv->coexist_connection && 
				(txcfg->pstat->ht_ie_len) && !(txcfg->pstat->ht_ie_buf.info0 & _HTIE_STA_CH_WDTH_))
#endif
				))
#endif

				) {
				pdesc->Dword4 |= set_desc(TX_DataBw | (3 << TX_DataScSHIFT));

#if defined(CONFIG_RTL_88E_SUPPORT) && defined(TXREPORT)
				if ((txcfg->fixed_rate) || (GET_CHIP_VER(priv)!=VERSION_8188E))
#endif
				{
					if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor40M &&
						txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_40M_)))
						n_txshort = 1;
				}
			}
			else {
				if (priv->pshare->offset_2nd_chan == HT_2NDCH_OFFSET_BELOW)
					pdesc->Dword4 |= set_desc((2 << TX_DataScSHIFT) | (2 << TX_RtsScSHIFT));
				else
					pdesc->Dword4 |= set_desc((1 << TX_DataScSHIFT) | (1 << TX_RtsScSHIFT));

#if defined(CONFIG_RTL_88E_SUPPORT) && defined(TXREPORT)
				if ((txcfg->fixed_rate) || (GET_CHIP_VER(priv)!=VERSION_8188E))
#endif
				{
					if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M &&
						txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_)))
						n_txshort = 1;
				}
			}
		} else {
#if defined(CONFIG_RTL_88E_SUPPORT) && defined(TXREPORT)
			if ((txcfg->fixed_rate) || (GET_CHIP_VER(priv)!=VERSION_8188E))
#endif
			{
				if (priv->pmib->dot11nConfigEntry.dot11nShortGIfor20M &&
					txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_SHORTGI_20M_)))
					n_txshort = 1;
			}
		}

#if defined(CONFIG_RTL_88E_SUPPORT) && defined(TXREPORT)
		if ((GET_CHIP_VER(priv)==VERSION_8188E) && !(txcfg->fixed_rate)) {
			if (txcfg->pstat->ht_current_tx_info & TX_USE_SHORT_GI)
				n_txshort = 1;
		}
#endif

		if ((txcfg->aggre_en >= FG_AGGRE_MPDU) && (txcfg->aggre_en <= FG_AGGRE_MPDU_BUFFER_LAST)) {
			int TID = ((struct sk_buff *)txcfg->pframe)->cb[1];
			if (txcfg->pstat->ADDBA_ready[TID] && !txcfg->pstat->low_tp_disable_ampdu) {
#ifdef CONFIG_RTL_88E_SUPPORT
				if (GET_CHIP_VER(priv)==VERSION_8188E)
					pdesc->Dword2 |= set_desc(TXdesc_88E_AggEn);
				else
#endif
					pdesc->Dword1 |= set_desc(TX_AggEn);

				/*
				 * assign aggr size
				 */
				 if (priv->pshare->rf_ft_var.diffAmpduSz) {
					pdesc->Dword6 |= set_desc((txcfg->pstat->diffAmpduSz & 0xffff) << TX_MCS1gMaxSHIFT | TX_UseMaxLen);
					
#ifdef CONFIG_RTL_88E_SUPPORT
					if (GET_CHIP_VER(priv)!=VERSION_8188E)
#endif						
					pdesc->Dword7 |= set_desc(txcfg->pstat->diffAmpduSz & 0xffff0000);
				 } else if(	((txcfg->pstat->current_tx_rate >= _MCS0_RATE_) && (txcfg->pstat->current_tx_rate <= _MCS2_RATE_)) 
						||((txcfg->pstat->current_tx_rate >= _MCS8_RATE_) && (txcfg->pstat->current_tx_rate <= _MCS10_RATE_)) )				 	
				 {
					pdesc->Dword6 |= set_desc((0x4444) << TX_MCS1gMaxSHIFT | TX_UseMaxLen);					
#ifdef CONFIG_RTL_88E_SUPPORT
					if (GET_CHIP_VER(priv)!=VERSION_8188E)
#endif						
					pdesc->Dword7 |= set_desc(0x44440000);									 
				 }
				// assign aggr density
				if (txcfg->privacy) {
#ifdef CONFIG_RTL_92D_SUPPORT
					if ((priv->pmib->dot11RFEntry.phyBandSelect == PHY_BAND_5G) && (!priv->pshare->is_40m_bw))
						pdesc->Dword2 |= set_desc(7 << TX_AmpduDstySHIFT);	// according to DWA-160 A2
					else
#endif
						pdesc->Dword2 |= set_desc(5 << TX_AmpduDstySHIFT);	// according to WN111v2
				}
				else {
					pdesc->Dword2 |= set_desc(((txcfg->pstat->ht_cap_buf.ampdu_para & _HTCAP_AMPDU_SPC_MASK_) >> _HTCAP_AMPDU_SPC_SHIFT_) << TX_AmpduDstySHIFT);
				}
			}
			//set Break
			if((txcfg->q_num >=1 && txcfg->q_num <=4)){
				if((txcfg->pstat != priv->pshare->CurPstat[txcfg->q_num-1])) {
#ifdef CONFIG_RTL_88E_SUPPORT
					if (GET_CHIP_VER(priv)==VERSION_8188E)
						pdesc->Dword2 |= set_desc(TXdesc_88E_BK);
					else
#endif
						pdesc->Dword1 |= set_desc(TX_BK);
					priv->pshare->CurPstat[txcfg->q_num-1] = txcfg->pstat;
				}				
			} else {
#ifdef CONFIG_RTL_88E_SUPPORT
				if (GET_CHIP_VER(priv)==VERSION_8188E)
					pdesc->Dword2 |= set_desc(TXdesc_88E_BK);
				else
#endif
					pdesc->Dword1 |= set_desc(TX_BK);
			}
		}

		// for STBC
#ifdef CONFIG_RTL_92C_SUPPORT
		if (GET_CHIP_VER(priv) != VERSION_8192C)
#endif			
		if (priv->pmib->dot11nConfigEntry.dot11nSTBC &&
			txcfg->pstat && (txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_RX_STBC_CAP_)) &&
			((get_rf_mimo_mode(priv) == MIMO_2T2R) || (get_rf_mimo_mode(priv) == MIMO_3T3R)))
			pdesc->Dword4 |= set_desc(1 << TX_DataStbcSHIFT);
	}
	else	// legacy rate
	{
		txRate = get_rate_index_from_ieee_value((UINT8)txcfg->tx_rate);
		if (is_CCK_rate(txcfg->tx_rate) && (txcfg->tx_rate != 2)) {
			if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11G) &&
					(priv->pmib->dot11ErpInfo.longPreambleStaNum > 0))
				; // txfw->txshort = 0
			else {
				if (txcfg->pstat)
					bg_txshort = (priv->pmib->dot11RFEntry.shortpreamble) &&
									(txcfg->pstat->useShortPreamble);
				else
					bg_txshort = priv->pmib->dot11RFEntry.shortpreamble;
			}
		}
		if (priv->pshare->is_40m_bw) {
			if (priv->pshare->offset_2nd_chan == HT_2NDCH_OFFSET_BELOW)
				pdesc->Dword4 |= set_desc((2 << TX_DataScSHIFT) | (2 << TX_RtsScSHIFT));
			else
				pdesc->Dword4 |= set_desc((1 << TX_DataScSHIFT) | (1 << TX_RtsScSHIFT));	
		}

		if (bg_txshort)
			pdesc->Dword4 |= set_desc(TX_DataShort);
	}

	if (txcfg->need_ack) { // unicast
		if (frag_idx == 0) 
			RtsCheck(priv, txcfg, &bRtsEnable, &bCts2SelfEnable, &bHwRts, &bErpProtect, &bNProtect);
	}

	RtsRate = find_rts_rate(priv, txcfg->tx_rate, bErpProtect);
	if(bRtsEnable && CheckCts2SelfEnable(RtsRate))
	{
		bRtsEnable = FALSE;
		bCts2SelfEnable = TRUE;
	}

//	if(bErpProtect)
//		priv->pshare->phw->bErpProtection = TRUE;

	if(bRtsEnable)
		pdesc->Dword4 |= set_desc(TX_RtsEn);
	if(bCts2SelfEnable)
		pdesc->Dword4 |= set_desc(TX_CTS2Self);
	if(bHwRts)
		pdesc->Dword4 |=set_desc(TX_HwRtsEn);
		
	if (bRtsEnable || bCts2SelfEnable) {
			unsigned int rtsTxRateIdx  = get_rate_index_from_ieee_value(RtsRate);
			if (bErpProtect) {
				unsigned char  rtsShort = 0;
				if (is_CCK_rate(RtsRate) && (RtsRate != 2)) {
					if ((priv->pmib->dot11BssType.net_work_type & WIRELESS_11G) &&
							(priv->pmib->dot11ErpInfo.longPreambleStaNum > 0))
						rtsShort = 0; // do nothing
					else {
						if (txcfg->pstat)
							rtsShort = (priv->pmib->dot11RFEntry.shortpreamble) &&
											(txcfg->pstat->useShortPreamble);
						else
							rtsShort = priv->pmib->dot11RFEntry.shortpreamble;
					}
				}
				pdesc->Dword4 |= (rtsShort == 1)? set_desc(TX_RtsShort): 0;
			} 
			
			pdesc->Dword4 |= set_desc((rtsTxRateIdx & TX_RtsRateMask) << TX_RtsRateSHIFT);
			pdesc->Dword5 |= set_desc((0xf & TX_RtsRateFBLmtMask) << TX_RtsRateFBLmtSHIFT);
			//8192SE Must specified BW mode while sending RTS ...
			if (priv->pshare->is_40m_bw)
				pdesc->Dword4 |= set_desc(TX_RtsBw);

	}

	if(n_txshort == 1 && txcfg->pstat->sta_in_firmware == 1)
		pdesc->Dword5 |= set_desc(TX_SGI);

#ifdef DRVMAC_LB
	if (priv->pmib->miscEntry.drvmac_lb && (priv->pmib->miscEntry.lb_mlmp == 4)) {
		txRate = rate_select;
		if (rate_select++ > 0x1b)
			rate_select = 0;

		pdesc->Dword4 |= set_desc(TX_UseRate);
		pdesc->Dword4 |= set_desc(TX_DisDataFB);
		pdesc->Dword4 |= set_desc(TX_DisRtsFB);// disable RTS fall back
	}
#endif

	if(priv->pshare->rf_ft_var.txforce != 0xff)	{
		pdesc->Dword4 |= set_desc(TX_UseRate);
		pdesc->Dword5 |= set_desc((priv->pshare->rf_ft_var.txforce & TX_DataRateMask) << TX_DataRateSHIFT);
	} else {
		pdesc->Dword5 |= set_desc((txRate & TX_DataRateMask) << TX_DataRateSHIFT);
	}

#ifdef CONFIG_RTL_92D_SUPPORT
	if (priv->pmib->dot11RFEntry.phyBandSelect == PHY_BAND_5G) {
		pdesc->Dword4 |= set_desc((4 & TX_RtsRateMask) << TX_RtsRateSHIFT);
		//if (is_CCK_rate(txRate))
			//pdesc->Dword5 |= set_desc((4 & TX_DataRateMask) << TX_DataRateSHIFT);
	}
#endif

	if (txcfg->need_ack) {
		// give retry limit to management frame
#ifndef DRVMAC_LB		
		if (txcfg->q_num == MANAGE_QUE_NUM) {
			pdesc->Dword5 |= set_desc(TX_RtyLmtEn);
			if (GetFrameSubType(txcfg->phdr) == WIFI_PROBERSP) {
				;	// 0 no need to set
			}
#ifdef WDS
			else if ((GetFrameSubType(txcfg->phdr) == WIFI_PROBEREQ) && (txcfg->pstat->state & WIFI_WDS)) {
				pdesc->Dword5 |= set_desc((2 & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
			}
#endif
			else {
				pdesc->Dword5 |= set_desc((6 & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
			}
		}
#ifdef WDS
		else if (txcfg->wdsIdx >= 0) {
			if (txcfg->pstat->rx_avarage == 0) {
				pdesc->Dword5 |= set_desc(TX_RtyLmtEn);
				pdesc->Dword5 |= set_desc((3 & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
			}
		}
#endif
		else if (txcfg->pstat->rssi < 30) {
			pdesc->Dword5 |= set_desc(TX_RtyLmtEn);
			pdesc->Dword5 |= set_desc((0x07 & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
		}
		else if (is_MCS_rate(txcfg->pstat->current_tx_rate) && (txcfg->pstat->IOTPeer==HT_IOT_PEER_INTEL) && (txcfg->pstat->retry_inc)
			&& !(txcfg->pstat->leave) && priv->pshare->intel_rty_lmt) {
			pdesc->Dword5 |= set_desc(TX_RtyLmtEn);
			pdesc->Dword5 |= set_desc((priv->pshare->intel_rty_lmt & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
		}

		else if ((txcfg->pstat->IOTPeer==HT_IOT_PEER_BROADCOM) && (txcfg->pstat->retry_inc) && !(txcfg->pstat->leave)) {
                pdesc->Dword5 |= set_desc(TX_RtyLmtEn);
                pdesc->Dword5 |= set_desc((0x20 & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
        }
#endif //end DRVMAC_LB

		// High power mechanism
		//if ((GET_CHIP_VER(priv) == VERSION_8192C)||(GET_CHIP_VER(priv) == VERSION_8188C))

		{
			if (priv->pshare->rf_ft_var.tx_pwr_ctrl && txcfg->pstat && (txcfg->fr_type == _SKB_FRAME_TYPE_)) {
				if ((txcfg->pstat->hp_level == 1)
#if defined(CONFIG_RTL_92D_SUPPORT) && defined(CONFIG_RTL_NOISE_CONTROL)
					|| ((priv->pshare->DNC_on) && (priv->pmib->dot11RFEntry.phyBandSelect == PHY_BAND_5G)) 
#endif
					) {
						int pwr = (priv->pshare->rf_ft_var.min_pwr_idex > 16) ? 16: priv->pshare->rf_ft_var.min_pwr_idex;
						pwr &= 0x1e;
						pdesc->Dword6 |= set_desc(((-pwr) & TX_TxAgcAMask) << TX_TxAgcASHIFT);
						pdesc->Dword6 |= set_desc(((-pwr) & TX_TxAgcBMask) << TX_TxAgcBSHIFT);
					}
			}
		}
	}
}
#endif


#ifdef TX_EARLY_MODE
__MIPS16
__IRAM_IN_865X
static void insert_emcontent(struct rtl8192cd_priv *priv, struct tx_insn* txcfg, unsigned char *buf)
{
	struct stat_info *pstat = txcfg->pstat;
	unsigned int dw[2];

	dw[0] = set_desc((pstat->empkt_num & 0xf) |
							(((pstat->empkt_len[0]+pstat->emextra_len) << 4) & 0xfff0) |
							(((pstat->empkt_len[1]+pstat->emextra_len) << 16) & 0xfff0000) |
							(((pstat->empkt_len[2]+pstat->emextra_len) << 28) & 0xf0000000) 	
							);
	dw[1] = set_desc((((pstat->empkt_len[2]+pstat->emextra_len) >> 4) & 0xff) |
							(((pstat->empkt_len[3]+pstat->emextra_len) << 8) & 0xfff00) |							
							(((pstat->empkt_len[4]+pstat->emextra_len) << 20) & 0xfff00000) 
							);
	memcpy(buf, dw, 8);
}
#endif


#ifdef CONFIG_PCI_HCI
#ifdef CONFIG_WLAN_HAL
int rtl88XX_signin_txdesc(struct rtl8192cd_priv *priv, struct tx_insn* txcfg, unsigned char convHdr)
{
	struct tx_desc_info	*pswdescinfo, *pdescinfo;
	unsigned int 		fr_len, tx_len, i, keyid;
	u2Byte              *tx_head;
    u4Byte              q_num;
	unsigned char		*da, *pbuf, *pwlhdr, *pmic, *picv;
	struct rtl8192cd_hw	*phw;
#ifdef TX_SHORTCUT
	int					fit_shortcut=0, idx=0;
#endif


    PHCI_TX_DMA_MANAGER_88XX        ptx_dma;
    u32                             halQNum;
    PHCI_TX_DMA_QUEUE_STRUCT_88XX   cur_q;
    PTX_BUFFER_DESCRIPTOR           cur_txbd;    
    TX_DESC_DATA_88XX               desc_data;

#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
    BOOLEAN isWiFiHdr = TRUE;
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV

	keyid=0;
	pmic=NULL;
	picv=NULL;

	if (txcfg->tx_rate == 0) {
		DEBUG_ERR("tx_rate=0!\n");
		txcfg->tx_rate = find_rate(priv, NULL, 0, 1);
	}

	q_num = txcfg->q_num;

	phw	= GET_HW(priv);
	
    halQNum     = GET_HAL_INTERFACE(priv)->MappingTxQueueHandler(priv, (u32)q_num);
    ptx_dma     = (PHCI_TX_DMA_MANAGER_88XX)(_GET_HAL_DATA(priv)->PTxDMA88XX);
    cur_q       = &(ptx_dma->tx_queue[halQNum]);
    cur_txbd    = cur_q->pTXBD_head + cur_q->host_idx;
    memset(&desc_data, 0, sizeof(TX_DESC_DATA_88XX));

    tx_head     = &(ptx_dma->tx_queue[halQNum].host_idx);

	pswdescinfo = get_txdesc_info(priv->pshare->pdesc_info, q_num);

	tx_len = txcfg->fr_len;

	if (txcfg->fr_type == _SKB_FRAME_TYPE_)
		pbuf = ((struct sk_buff *)txcfg->pframe)->data;
	else
		pbuf = (unsigned char*)txcfg->pframe;

#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
	// this condition is 802.3 header
	if ((GET_CHIP_VER(priv)==VERSION_8813A) && (convHdr==HW_TX_SC_HEADER_CONV))
		isWiFiHdr = FALSE; 
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV


#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
	if (isWiFiHdr)
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
	{
	da = get_da((unsigned char *)txcfg->phdr);
	}

#ifdef CONFIG_IEEE80211W
	unsigned int	isBIP = 0;	
			
	if(txcfg->isPMF && IS_MCAST(da)) 
	{
		isBIP = 1;
		txcfg->iv = 0;
		txcfg->fr_len += 10; // 10: MMIE length
	}
#endif

	// TODO: hw tx shorcut, reuse txdesc only support AES...
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
	if (isWiFiHdr)
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
	// in case of default key, then find the key id
	if (GetPrivacy((txcfg->phdr)))
	{
#ifdef WDS
		if (txcfg->wdsIdx >= 0) {
			if (txcfg->pstat)
			keyid = txcfg->pstat->keyid;
			else
				keyid = 0;
		}
		else
#endif

#ifdef __DRAYTEK_OS__
		if (!IEEE8021X_FUN)
			keyid = priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		else {
			if (IS_MCAST(GetAddr1Ptr ((unsigned char *)txcfg->phdr)) || !txcfg->pstat)
				keyid = priv->pmib->dot11GroupKeysTable.keyid;
			else
				keyid = txcfg->pstat->keyid;
		}
#else

		if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==_WEP_40_PRIVACY_ ||
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==_WEP_104_PRIVACY_) {
			if(IEEE8021X_FUN && txcfg->pstat) {
#ifdef A4_STA
				if (IS_MCAST(da) && !(txcfg->pstat->state & WIFI_A4_STA))
#else
				if(IS_MCAST(da))
#endif					
					keyid = 0;
				else
					keyid = txcfg->pstat->keyid;
			}
			else {
				keyid = priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		    }
		}
#endif
	}


	for (i=0; i < txcfg->frg_num; i++)
	{
        pdescinfo = pswdescinfo + *tx_head;

#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		// TODO: hw tx shortcut no support fragment ? Qos Control bit ??
		if (isWiFiHdr)
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		if (i != 0)
		{
			memset(&desc_data, 0, sizeof(TX_DESC_DATA_88XX));
			
			// we have to allocate wlan_hdr
			pwlhdr = (UINT8 *)get_wlanhdr_from_poll(priv);
			if (pwlhdr == (UINT8 *)NULL)
			{
				DEBUG_ERR("System-bug... should have enough wlan_hdr\n");
				return (txcfg->frg_num - i);
			}
			// other MPDU will share the same seq with the first MPDU
			memcpy((void *)pwlhdr, (void *)(txcfg->phdr), txcfg->hdr_len); // data pkt has 24 bytes wlan_hdr
		}
		else
		{
#ifdef WIFI_WMM
			if (txcfg->pstat && (is_qos_data(txcfg->phdr))) {
				if ((GetFrameSubType(txcfg->phdr) & (WIFI_DATA_TYPE | BIT(6) | BIT(7))) == (WIFI_DATA_TYPE | BIT(7))) {
					unsigned char tempQosControl[2];
					memset(tempQosControl, 0, 2);
					tempQosControl[0] = ((struct sk_buff *)txcfg->pframe)->cb[1];
#ifdef WMM_APSD
					if (
#ifdef CLIENT_MODE
						(OPMODE & WIFI_AP_STATE) &&
#endif
						(APSD_ENABLE) && (txcfg->pstat) && (txcfg->pstat->state & WIFI_SLEEP_STATE) &&
						(!GetMData(txcfg->phdr)) &&
						((((tempQosControl[0] == 7) || (tempQosControl[0] == 6)) && (txcfg->pstat->apsd_bitmap & 0x01)) ||
						 (((tempQosControl[0] == 5) || (tempQosControl[0] == 4)) && (txcfg->pstat->apsd_bitmap & 0x02)) ||
						 (((tempQosControl[0] == 3) || (tempQosControl[0] == 0)) && (txcfg->pstat->apsd_bitmap & 0x08)) ||
						 (((tempQosControl[0] == 2) || (tempQosControl[0] == 1)) && (txcfg->pstat->apsd_bitmap & 0x04))))
						tempQosControl[0] |= BIT(4);
#endif
					if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST)
						tempQosControl[0] |= BIT(7);

					if (priv->pmib->dot11nConfigEntry.dot11nTxNoAck)
						tempQosControl[0] |= BIT(5);

					memcpy((void *)GetQosControl((txcfg->phdr)), tempQosControl, 2);
				}
			}
#endif

#ifdef BEAMFORMING_SUPPORT
			if(!txcfg->ndpa)
#endif	
			assign_wlanseq(GET_HW(priv), txcfg->phdr, txcfg->pstat, GET_MIB(priv)
#ifdef CONFIG_RTK_MESH	// For broadcast data frame via mesh (ex:ARP requst)
				, txcfg->is_11s
#endif
				);
			pwlhdr = txcfg->phdr;
		}
#ifdef BEAMFORMING_SUPPORT
		if(!txcfg->ndpa)
#endif	
        {
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
 	       	if (isWiFiHdr)
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
            {
		SetDuration(pwlhdr, 0);
            }
		}

        // TODO: how to fill some field in rtl88XX_fill_fwinfo
        rtl88XX_fill_fwinfo(priv, txcfg, i, &desc_data);


#ifdef CLIENT_MODE
		if (OPMODE & WIFI_STATION_STATE) {
			if (GetFrameSubType(txcfg->phdr) == WIFI_PSPOLL)
        desc_data.navUseHdr = _TRUE;

			if (priv->ps_state)
				SetPwrMgt(pwlhdr);
			else
				ClearPwrMgt(pwlhdr);
		}
#endif
#ifdef BEAMFORMING_SUPPORT
		if(txcfg->ndpa)
			desc_data.navUseHdr = _TRUE;
#endif

#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		if (isWiFiHdr)
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		if (i != (txcfg->frg_num - 1))
		{
			SetMFrag(pwlhdr);
			if (i == 0) {
				fr_len = (txcfg->frag_thrshld - txcfg->llc);
				tx_len -= (txcfg->frag_thrshld - txcfg->llc);
			}
			else {
				fr_len = txcfg->frag_thrshld;
				tx_len -= txcfg->frag_thrshld;
			}
		}
		else	// last seg, or the only seg (frg_num == 1)
		{
			fr_len = tx_len;
			ClearMFrag(pwlhdr);
		}
        
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
        if (isWiFiHdr)
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
        {
		SetFragNum((pwlhdr), i);
        }

		if (((i == 0) && (txcfg->fr_type == _SKB_FRAME_TYPE_))
#ifdef BEAMFORMING_SUPPORT
			|| txcfg->ndpa
#endif
		) {
			pdescinfo->type = _PRE_ALLOCLLCHDR_;
		}
		else {
			pdescinfo->type = _PRE_ALLOCHDR_;
		}

#ifdef _11s_TEST_MODE_
		mesh_debug_tx9(txcfg, pdescinfo);
#endif

		if(txcfg->fr_type == _SKB_FRAME_TYPE_) {
	       desc_data.tid = ((struct sk_buff *)txcfg->pframe)->cb[1];
#ifdef WMM_DSCP_C42
			{
				unsigned int 	   q_num;
#ifdef RTL_MANUAL_EDCA
				q_num = PRI_TO_QNUM(priv, desc_data.tid);
#else
				PRI_TO_QNUM(desc_data.tid, q_num, priv->pmib->dot11OperationEntry.wifi_specific);
#endif
				if(q_num ^ txcfg->q_num) 
					desc_data.tid = 0x04;
			}
#endif
		}


        if (i != (txcfg->frg_num - 1)) {
            desc_data.frag = _TRUE;
        }

		if (txcfg->pstat) {
			if (txcfg->pstat->aid != MANAGEMENT_AID) {
                desc_data.rateId = txcfg->pstat->ratr_idx;
                desc_data.macId = REMAP_AID(txcfg->pstat);
			}
#ifdef BEAMFORMING_SUPPORT
			if((priv->pmib->dot11RFEntry.txbf == 1) &&
				((txcfg->pstat->ht_cap_len && (txcfg->pstat->ht_cap_buf.txbf_cap)) 
#ifdef RTK_AC_SUPPORT
				||(txcfg->pstat->vht_cap_len && (cpu_to_le32(txcfg->pstat->vht_cap_buf.vht_cap_info) & BIT(SU_BFEE_S)))
#endif
			)){
				desc_data.p_aid = txcfg->pstat->p_aid;
				desc_data.g_id  = txcfg->pstat->g_id;
			}
#endif
		} else {
            desc_data.rateId = ARFR_BMC;
#ifdef BEAMFORMING_SUPPORT
			if (priv->pmib->dot11RFEntry.txbf == 1) 
				desc_data.g_id = 1;
#endif				
        }

        desc_data.dataRateFBLmt = 0x1F;

        if (txcfg->fixed_rate) {
            desc_data.disDataFB = _TRUE;
            desc_data.disRTSFB  = _TRUE;
            desc_data.useRate   = _TRUE;
        }


        if(txcfg->pstat && txcfg->pstat->sta_in_firmware != 1)
            desc_data.useRate = _TRUE;


#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		if (isWiFiHdr)
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		if (txcfg->privacy) {
            desc_data.secType = txcfg->privacy;
#ifdef CONFIG_IEEE80211W			
			if (UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF)) {
#else		
			if (UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE))) {
#endif				
                desc_data.swCrypt = TRUE;
                desc_data.icv = txcfg->icv;
                desc_data.mic = txcfg->mic;
                desc_data.iv  = txcfg->iv;
			} else {
				// hw encrypt
				desc_data.swCrypt = FALSE;
				switch(txcfg->privacy) {
				case _WEP_104_PRIVACY_:
				case _WEP_40_PRIVACY_:
                    desc_data.icv = 0;
                    desc_data.mic = 0;
                    desc_data.iv  = txcfg->iv;
					wep_fill_iv(priv, pwlhdr, txcfg->hdr_len, keyid);
					break;

				case _TKIP_PRIVACY_:
                    desc_data.icv = 0;
                    desc_data.mic = txcfg->mic;
                    desc_data.iv  = txcfg->iv;
					tkip_fill_encheader(priv, pwlhdr, txcfg->hdr_len, keyid);
					break;

				#if defined(CONFIG_RTL_HW_WAPI_SUPPORT)
				case _WAPI_SMS4_:
                    desc_data.icv = 0;
                    desc_data.mic = 0;
                    desc_data.iv  = txcfg->iv;
					break;
				#endif
                
				case _CCMP_PRIVACY_:
					//michal also hardware...
                    desc_data.icv = 0;
                    desc_data.mic = 0;
                    desc_data.iv  = txcfg->iv;
					aes_fill_encheader(priv, pwlhdr, txcfg->hdr_len, keyid);
					break;

				default:
					DEBUG_ERR("Unknow privacy\n");
					break;
				}
			}
		}

#ifdef WLAN_HAL_HW_AES_IV
        if ((txcfg->privacy == _CCMP_PRIVACY_) && 
            (
#ifdef CONFIG_IEEE80211W
            (UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF)) 
#else
            (UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))
#endif
                == FALSE)) {
            desc_data.secType = txcfg->privacy;
            desc_data.swCrypt = FALSE;
            desc_data.icv     = 0;
            desc_data.mic     = 0;
            desc_data.iv      = 0;
            desc_data.hwAESIv = TRUE;

            if (isWiFiHdr == TRUE) {
                desc_data.iv = txcfg->iv;
                // when desc_data.hwAESIv == TRUE, hw auto fill AES iv
                // aes_fill_encheader(priv, pwlhdr, txcfg->hdr_len, keyid);
            }
        }
#endif // WLAN_HAL_HW_AES_IV

		// below is for sw desc info
		// TODO: hw tx shortcut recycle....sw desc.
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		if (isWiFiHdr == FALSE) {
			pdescinfo->pframe = NULL;
		} else
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		{
		pdescinfo->pframe = pwlhdr;
		}

#if defined(WIFI_WMM) && defined(WMM_APSD)
		pdescinfo->priv = priv;
#ifndef TXDESC_INFO
		pdescinfo->pstat = txcfg->pstat;
#endif		
#endif

#ifdef TX_SHORTCUT
		if (!priv->pmib->dot11OperationEntry.disable_txsc && txcfg->pstat &&
				(txcfg->fr_type == _SKB_FRAME_TYPE_) &&
				(txcfg->frg_num == 1) &&
				((txcfg->privacy == 0)
#ifdef CONFIG_RTL_WAPI_SUPPORT
				|| (txcfg->privacy == _WAPI_SMS4_)
#endif
#ifdef CONFIG_IEEE80211W
				|| (txcfg->privacy && !UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))
#else
				|| (txcfg->privacy && !UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))
#endif
				) &&
				(
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
				(isWiFiHdr == FALSE) ? TRUE : 
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
				(!GetMData(txcfg->phdr))
				)
				) {

#ifdef CONFIG_RTK_MESH
			if( txcfg->is_11s) {
				idx = get_tx_sc_free_entry(priv, txcfg->pstat, &priv->ethhdr);
			} else
#endif
			{
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
				if (isWiFiHdr == FALSE) {
					idx = get_tx_sc_free_entry(priv, txcfg->pstat, pbuf);
				} else
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
                {
					idx = get_tx_sc_free_entry(priv, txcfg->pstat, pbuf - sizeof(struct wlan_ethhdr_t));
				}
			}
#ifdef CONFIG_RTK_MESH
			if( txcfg->is_11s) {
				memcpy((void *)&txcfg->pstat->tx_sc_ent[idx].ethhdr, &priv->ethhdr, sizeof(struct wlan_ethhdr_t));
			} else
#endif
			{
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
				if (isWiFiHdr == FALSE) 
				{
					memcpy((void *)&txcfg->pstat->tx_sc_ent[idx].ethhdr, pbuf, sizeof(struct wlan_ethhdr_t));
				} else
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
                {
					memcpy((void *)&txcfg->pstat->tx_sc_ent[idx].ethhdr, pbuf - sizeof(struct wlan_ethhdr_t), sizeof(struct wlan_ethhdr_t));
				}
			}

			txcfg->pstat->protection = priv->pmib->dot11ErpInfo.protection;
			txcfg->pstat->ht_protection = priv->ht_protection;
			txcfg->pstat->tx_sc_ent[idx].sc_keyid = keyid;
			txcfg->pstat->tx_sc_ent[idx].pktpri = ((struct sk_buff *)txcfg->pframe)->cb[1];
			fit_shortcut = 1;

#ifdef CONFIG_WLAN_HAL_8813AE
			if (GET_CHIP_VER(priv)==VERSION_8813A) {
#ifdef WLAN_HAL_HW_TX_SHORTCUT_REUSE_TXDESC
				desc_data.txwifiCp = TRUE;
#endif // WLAN_HAL_HW_TX_SHORTCUT_REUSE_TXDESC
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
				if (convHdr == HW_TX_SC_BACKUP_HEADER) {
					desc_data.macCp = TRUE;		// backup 802.11 header info.
					desc_data.smhEn = FALSE;
				} else if (convHdr == HW_TX_SC_HEADER_CONV) {
					desc_data.macCp = FALSE;
					desc_data.smhEn = TRUE;		// auto conv hdr  (802.3 -> 802.11)	
					// TODO: consider AES for HW_TX_SHORTCUT_HDR_CONV
					// Eth hdr 14 bytes => gen llc 8 bytes
					//                              gen iv 8 bytes
					// skb: ethHdr + data
					// ethHdr + (iv + llc)  + data
					// BD[0]: txDesc, BD[1]: ethHdr, BD[2]: iv+llc, BD[3]: data
				}
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
			}
#endif // CONFIG_WLAN_HAL_8813AE
		}
		else {
			if (txcfg->pstat) {
				for (idx=0; idx<TX_SC_ENTRY_NUM; idx++) {
                    GET_HAL_INTERFACE(priv)->SetShortCutTxBuffSizeHandler(priv, txcfg->pstat->tx_sc_ent[i].hal_hw_desc, 0);
				}
			}
		}
#endif

		// TODO:  Currently, we don't care WAPI
#ifndef TXDESC_INFO			
		if (txcfg->privacy)
		{
			if (txcfg->privacy == _WAPI_SMS4_)
			{
				pdescinfo->pstat = txcfg->pstat;
				pdescinfo->rate = txcfg->tx_rate;
			}
#ifdef CONFIG_IEEE80211W			
			else if (!UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))
#else
			else if (!UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))			
#endif
			{
				pdescinfo->pstat = txcfg->pstat;
				pdescinfo->rate = txcfg->tx_rate;
			}
            // TODO: why swCrypto doesn't set these two swDesc ?
		}
		else
		{
			pdescinfo->pstat = txcfg->pstat;
			pdescinfo->rate = txcfg->tx_rate;
		}
#endif    
	#ifdef CONFIG_RTL_WAPI_SUPPORT
	//panic_printk("%s:%d privacy=%d\n", __FUNCTION__, __LINE__,txcfg->privacy);
	if (txcfg->privacy == _WAPI_SMS4_)
	{
		//panic_printk("%s:%d\n", __FUNCTION__, __LINE__);
		SecSWSMS4Encryption(priv, txcfg);
		desc_data.pMic = ((struct sk_buff *)txcfg->pframe)->data+txcfg->fr_len;
	} 
	#endif    
        /*** start sw encryption ***/      
#ifdef CONFIG_IEEE80211W	
		if (txcfg->privacy && UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE),txcfg->isPMF)) {
#else
		if (txcfg->privacy && UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE))) {
#endif
      
            if (txcfg->privacy == _TKIP_PRIVACY_ ||
                txcfg->privacy == _WEP_40_PRIVACY_ ||
                txcfg->privacy == _WEP_104_PRIVACY_) {
                
                //picvdescinfo = pswdescinfo + *tx_head;
        
                // append ICV first...
                picv = get_icv_from_poll(priv);
                if (picv == NULL) {
                    DEBUG_ERR("System-Buf! can't alloc picv\n");
                    BUG();
                }
                
                pdescinfo->buf_type[1]   = _PRE_ALLOCICVHDR_;
                pdescinfo->buf_pframe[1] = picv;

                desc_data.pIcv = picv;
                
                if (i == 0) {
                    tkip_icv(picv,
                          pwlhdr + txcfg->hdr_len + txcfg->iv, txcfg->llc,
                          pbuf,                                txcfg->fr_len);
                } else {                
                    tkip_icv(picv,
                        NULL, 0,
                        pbuf, txcfg->fr_len);
                }
        
                if ((i == 0) && (txcfg->llc != 0)) {
                    if (txcfg->privacy == _TKIP_PRIVACY_) {
                        tkip_encrypt(priv, pwlhdr, txcfg->hdr_len,
                            pwlhdr + txcfg->hdr_len + 8, sizeof(struct llc_snap),
                            pbuf,                        txcfg->fr_len, 
                            picv,                        txcfg->icv);
                    } else {
                        wep_encrypt(priv, pwlhdr, txcfg->hdr_len,
                            pwlhdr + txcfg->hdr_len + 4, sizeof(struct llc_snap),
                            pbuf,                        txcfg->fr_len, 
                            picv,                        txcfg->icv,
                            txcfg->privacy);
                    }
                } else { // not first segment or no snap header
                    if (txcfg->privacy == _TKIP_PRIVACY_) {
                        tkip_encrypt(priv, pwlhdr, txcfg->hdr_len, 
                            NULL, 0,
                            pbuf, txcfg->fr_len, 
                            picv, txcfg->icv);
                    } else {
                        wep_encrypt(priv, pwlhdr, txcfg->hdr_len, 
                            NULL, 0,
                            pbuf, txcfg->fr_len,
                            picv, txcfg->icv,
                            txcfg->privacy);
                    }
                }
                        
            } else if (txcfg->privacy == _CCMP_PRIVACY_) {
                //pmicdescinfo = pswdescinfo + *tx_head;

                // append MIC first...
                pmic = get_mic_from_poll(priv);
                if (pmic == NULL) {
                    DEBUG_ERR("System-Buf! can't alloc pmic\n");
                    BUG();
                }

                pdescinfo->buf_type[1]   = _PRE_ALLOCMICHDR_;
                pdescinfo->buf_pframe[1] = pmic;

                desc_data.pMic = pmic;
                
                // then encrypt all (including ICV) by AES
                if ((i == 0)&&(txcfg->llc != 0)) { // encrypt 3 segments ==> llc, mpdu, and mic
#ifdef CONFIG_IEEE80211W
					if(isBIP) {
						BIP_encrypt(priv, pwlhdr, 
									pwlhdr + txcfg->hdr_len + 8,
									pbuf, txcfg->fr_len, 
									pmic,txcfg->isPMF);
					} 
					else {
						aesccmp_encrypt(priv, pwlhdr, 
										pwlhdr + txcfg->hdr_len + 8,
										pbuf, txcfg->fr_len, 
										pmic,txcfg->isPMF);
					}
#else
						aesccmp_encrypt(priv, pwlhdr, 
										pwlhdr + txcfg->hdr_len + 8,
										pbuf, txcfg->fr_len, 
										pmic);
#endif
                  
                } else { // encrypt 2 segments ==> mpdu and mic
#ifdef CONFIG_IEEE80211W
					if(isBIP) {
						BIP_encrypt(priv, pwlhdr, 
									NULL,
									pbuf, txcfg->fr_len, 
									pmic,txcfg->isPMF);
					} 
					else {
						aesccmp_encrypt(priv, pwlhdr, 
										NULL,
										pbuf, txcfg->fr_len, 
										pmic,txcfg->isPMF);
					}
#else
					aesccmp_encrypt(priv, pwlhdr, 
									NULL,
									pbuf, txcfg->fr_len, 
									pmic);
#endif                    

               }       
            }
        }
        /*** end sw encryption ***/

        desc_data.pHdr   = pwlhdr;
        desc_data.hdrLen = txcfg->hdr_len;
        desc_data.llcLen = (i==0 ? txcfg->llc : 0);
        desc_data.frLen  = fr_len;
        
        if (fr_len != 0) {
            desc_data.pBuf = pbuf;
        }

#ifdef CONFIG_8813_AP_MAC_VERI
    if (txcfg->fr_len != 0)	//for mgt frame
    {
        if((desc_data.pBuf[0]==0x55) && (desc_data.pBuf[1]==0x55))
        {
            desc_data.dataRtyLmt = 10;
            desc_data.macId = priv->macID_temp;
            desc_data.disDataFB = _FALSE;
            desc_data.disRTSFB  = _FALSE;
            desc_data.useRate= _TRUE;  

            desc_data.dataRate = 0x17; // HT MCS6
            desc_data.RTSRate = 0x8;
            desc_data.rateId = 5;

            if(priv->lowestRate_TXDESCen)
            {
                desc_data.moreData = 1;                
                desc_data.dataRateFBLmt = priv->lowestRate;
                desc_data.RTSRateFBLmt = priv->lowestRate;                
                desc_data.rtyLmtEn = 1;                                         
            }
            else
            {
                desc_data.rtyLmtEn = 0;
            }

            desc_data.RTSEn = TRUE;

#ifdef VERIFY_AP_FAST_EDCA
			desc_data.aggEn = 0;	// no agg.
			desc_data.dataRate = 4;	// OFDM rate
			desc_data.RTSEn = FALSE;
#endif
        }
    }
#endif 

#ifdef TX_SHORTCUT
        if (fit_shortcut) {
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
			if (isWiFiHdr == FALSE) {
				desc_data.pHdr	 = txcfg->phdr;
				desc_data.hdrLen = txcfg->fr_len;
				desc_data.llcLen = 0;
				desc_data.frLen  = 0;
			}
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
            GET_HAL_INTERFACE(priv)->FillShortCutTxHwCtrlHandler(
                    priv, halQNum, (void *)&desc_data, txcfg->pstat->tx_sc_ent[idx].hal_hw_desc, 0x01);
        } else
#endif
        {
            GET_HAL_INTERFACE(priv)->FillTxHwCtrlHandler(priv, halQNum, (void *)&desc_data);
        }

        if (txcfg->fr_len != 0) {
            if (i == 0) {
                pdescinfo->buf_type[0] = txcfg->fr_type;
            } else {
                pdescinfo->buf_type[0] = _RESERVED_FRAME_TYPE_;
            }

            pdescinfo->buf_pframe[0]   = txcfg->pframe;
            pdescinfo->buf_len[0]      = txcfg->fr_len;       
#ifndef TXDESC_INFO
            pdescinfo->buf_paddr[0]    = get_desc(cur_txbd->TXBD_ELE[2].Dword1);//payload
#endif			
        }
#ifndef TXDESC_INFO		
        pdescinfo->paddr = get_desc(cur_txbd->TXBD_ELE[1].Dword1);//header address
#endif
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		// TODO: temp for swdesc...should check it..
		if (isWiFiHdr == FALSE) {
			pdescinfo->type				= _SKB_FRAME_TYPE_;	// 802.3 header + payload
			pdescinfo->pframe 			= txcfg->pframe; // skb

			pdescinfo->buf_type[0]	 	= 0;
			pdescinfo->buf_pframe[0] 	= 0; // no packet payload
		}
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV

#ifdef TX_SHORTCUT
        if (fit_shortcut) {
			//descinfo_copy(&txcfg->pstat->tx_sc_ent[idx].swdesc1, pdescinfo);
    		txcfg->pstat->tx_sc_ent[idx].swdesc1.type = pdescinfo->type;
#ifndef TXDESC_INFO			
	    	txcfg->pstat->tx_sc_ent[idx].swdesc1.len  = pdescinfo->len;
    		txcfg->pstat->tx_sc_ent[idx].swdesc1.rate = pdescinfo->rate;
#endif			
            //txcfg->pstat->tx_sc_ent[idx].swdesc1.buf_type[0] = pdescinfo->buf_type[0]
        }
#endif

		if (txcfg->fr_len == 0)
		{
//            printk("%s(%d): fr_len == 0 !!! \n", __FUNCTION__, __LINE__);
			goto init_deschead;
		}

		pbuf += fr_len;

	}


init_deschead:

    // TODO: ...pfrst_dma_desc 
    GET_HAL_INTERFACE(priv)->SyncSWTXBDHostIdxToHWHandler(priv, halQNum);
	return 0;
}
#endif // CONFIG_WLAN_HAL

#ifdef CONFIG_RTL_8812_SUPPORT
int rtl8192cd_signin_txdesc_8812(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
	struct tx_desc		*phdesc, *pdesc, *pndesc, *picvdesc, *pmicdesc, *pfrstdesc;
	struct tx_desc_info	*pswdescinfo, *pdescinfo, *pndescinfo, *picvdescinfo, *pmicdescinfo;
	unsigned int 		fr_len, tx_len, i, keyid;
	int					*tx_head, q_num;
	unsigned long		tmpphyaddr;
	unsigned char		*da, *pbuf, *pwlhdr, *pmic, *picv;
	struct rtl8192cd_hw	*phw;
	unsigned char		 q_select;
#ifdef TX_SHORTCUT
	int					fit_shortcut=0, idx=0;
#endif
	unsigned long		pfrst_dma_desc=0;
	unsigned long		*dma_txhead;

	unsigned long		flush_addr[20];
	int					flush_len[20];
	int					flush_num=0;

	picvdesc=NULL;
	keyid=0;
	pmic=NULL;
	picv=NULL;
	q_select=0;

	if (txcfg->tx_rate == 0) {
		DEBUG_ERR("tx_rate=0!\n");
		txcfg->tx_rate = find_rate(priv, NULL, 0, 1);
	}

	q_num = txcfg->q_num;

	phw	= GET_HW(priv);

	dma_txhead	= get_txdma_addr(phw, q_num);
	tx_head		= get_txhead_addr(phw, q_num);
	phdesc   	= get_txdesc(phw, q_num);
	pswdescinfo = get_txdesc_info(priv->pshare->pdesc_info, q_num);

	da = get_da((unsigned char *)txcfg->phdr);

#ifdef CONFIG_IEEE80211W
		unsigned int	isBIP = 0;	
		
		if(txcfg->isPMF && IS_MCAST(da)) 
		{
			isBIP = 1;
			txcfg->iv = 0;
			txcfg->fr_len += 10; // 10: MMIE length
			panic_printk("%s(%d),txcfg->fr_len=%d\n", __FUNCTION__, __LINE__, txcfg->fr_len);
		}
#endif

	tx_len = txcfg->fr_len;

	if (txcfg->fr_type == _SKB_FRAME_TYPE_)
		pbuf = ((struct sk_buff *)txcfg->pframe)->data;
	else
		pbuf = (unsigned char*)txcfg->pframe;


	tmpphyaddr = get_physical_addr(priv, pbuf, tx_len, PCI_DMA_TODEVICE);

	// in case of default key, then find the key id
	if (GetPrivacy((txcfg->phdr)))
	{
#ifdef WDS
		if (txcfg->wdsIdx >= 0) {
			if (txcfg->pstat)
				keyid = txcfg->pstat->keyid;
			else
				keyid = 0;
		}
		else
#endif

#ifdef __DRAYTEK_OS__
		if (!IEEE8021X_FUN)
			keyid = priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		else {
			if (IS_MCAST(GetAddr1Ptr ((unsigned char *)txcfg->phdr)) || !txcfg->pstat)
				keyid = priv->pmib->dot11GroupKeysTable.keyid;
			else
				keyid = txcfg->pstat->keyid;
		}
#else

		if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==_WEP_40_PRIVACY_ ||
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==_WEP_104_PRIVACY_) {
			if(IEEE8021X_FUN && txcfg->pstat) {
#ifdef A4_STA
				if (IS_MCAST(da) && !(txcfg->pstat->state & WIFI_A4_STA))
#else
				if(IS_MCAST(da))
#endif					
					keyid = 0;
				else
					keyid = txcfg->pstat->keyid;
			}
			else
				keyid = priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		}
#endif


	}

	for(i=0, pfrstdesc= phdesc + (*tx_head); i < txcfg->frg_num; i++)
	{
		/*------------------------------------------------------------*/
		/*           fill descriptor of header + iv + llc             */
		/*------------------------------------------------------------*/
		pdesc     = phdesc + (*tx_head);
		pdescinfo = pswdescinfo + *tx_head;

		//clear all bits

		memset(pdesc, 0, 40);

		if (i != 0)
		{
			// we have to allocate wlan_hdr
			pwlhdr = (UINT8 *)get_wlanhdr_from_poll(priv);
			if (pwlhdr == (UINT8 *)NULL)
			{
				DEBUG_ERR("System-bug... should have enough wlan_hdr\n");
				return (txcfg->frg_num - i);
			}
			// other MPDU will share the same seq with the first MPDU
			memcpy((void *)pwlhdr, (void *)(txcfg->phdr), txcfg->hdr_len); // data pkt has 24 bytes wlan_hdr
			pdesc->Dword9 |= set_desc((GetSequence(txcfg->phdr) & TXdesc_92E_TX_SeqMask)  << TXdesc_92E_TX_SeqSHIFT);
		}
		else
		{
#ifdef WIFI_WMM
			if (txcfg->pstat && (is_qos_data(txcfg->phdr))) {
				if ((GetFrameSubType(txcfg->phdr) & (WIFI_DATA_TYPE | BIT(6) | BIT(7))) == (WIFI_DATA_TYPE | BIT(7))) {
					unsigned char tempQosControl[2];
					memset(tempQosControl, 0, 2);
					tempQosControl[0] = ((struct sk_buff *)txcfg->pframe)->cb[1];
#ifdef WMM_APSD
					if (
#ifdef CLIENT_MODE
						(OPMODE & WIFI_AP_STATE) &&
#endif
						(APSD_ENABLE) && (txcfg->pstat) && (txcfg->pstat->state & WIFI_SLEEP_STATE) &&
						(!GetMData(txcfg->phdr)) &&
						((((tempQosControl[0] == 7) || (tempQosControl[0] == 6)) && (txcfg->pstat->apsd_bitmap & 0x01)) ||
						 (((tempQosControl[0] == 5) || (tempQosControl[0] == 4)) && (txcfg->pstat->apsd_bitmap & 0x02)) ||
						 (((tempQosControl[0] == 3) || (tempQosControl[0] == 0)) && (txcfg->pstat->apsd_bitmap & 0x08)) ||
						 (((tempQosControl[0] == 2) || (tempQosControl[0] == 1)) && (txcfg->pstat->apsd_bitmap & 0x04))))
						tempQosControl[0] |= BIT(4);
#endif
					if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST)
						tempQosControl[0] |= BIT(7);

					if (priv->pmib->dot11nConfigEntry.dot11nTxNoAck)
						tempQosControl[0] |= BIT(5);

					memcpy((void *)GetQosControl((txcfg->phdr)), tempQosControl, 2);
				}
			}
#endif

#ifdef BEAMFORMING_SUPPORT
			if(!txcfg->ndpa)
#endif	
			assign_wlanseq(GET_HW(priv), txcfg->phdr, txcfg->pstat, GET_MIB(priv)
#ifdef CONFIG_RTK_MESH	// For broadcast data frame via mesh (ex:ARP requst)
				, txcfg->is_11s
#endif
				);

			pdesc->Dword9 |= set_desc((GetSequence(txcfg->phdr) & TXdesc_92E_TX_SeqMask)  << TXdesc_92E_TX_SeqSHIFT);
			pwlhdr = txcfg->phdr;
		}
#ifdef BEAMFORMING_SUPPORT
		if(!txcfg->ndpa)
#endif			
		SetDuration(pwlhdr, 0);

		rtl8192cd_fill_fwinfo_8812(priv, txcfg, pdesc, i);

		if (priv->pmib->dot11nConfigEntry.dot11nTxNoAck){
			pdesc->Dword0 |= set_desc(TX_BMC);
			pdesc->Dword2 &= set_desc(~(TXdesc_92E_AggEn));
		}


		//Disable all RTS/CTS //for 11ac logo
		if(priv->pshare->rf_ft_var.no_rtscts) {
			pdesc->Dword3 &= set_desc(~(TXdesc_92E_HwRtsEn|TXdesc_92E_RtsEn|TXdesc_92E_CTS2Self));
		}
		else
		{
		if(txcfg->pstat) //8812_11n_iot, only vht clnt support cca_rts
		if((priv->pshare->rf_ft_var.cca_rts) && (txcfg->pstat->vht_cap_len > 0))
		pdesc->Dword2 |= set_desc((priv->pshare->rf_ft_var.cca_rts & 0x03) << TX_8812_CcaRtsSHIFT);	//10b:  RTS support dynamic mode CCA secondary
		}




		pdesc->Dword0 |= set_desc(40 << TX_OffsetSHIFT); // tx_desc size
		
		if (IS_MCAST(GetAddr1Ptr((unsigned char *)txcfg->phdr)))
			pdesc->Dword0 |= set_desc(TX_BMC);

#ifdef CLIENT_MODE
		if (OPMODE & WIFI_STATION_STATE) {
			if (GetFrameSubType(txcfg->phdr) == WIFI_PSPOLL)
				pdesc->Dword3 |= set_desc(TXdesc_92E_NAVUSEHDR);


			if (priv->ps_state)
				SetPwrMgt(pwlhdr);
			else
				ClearPwrMgt(pwlhdr);
		}
#endif
#ifdef BEAMFORMING_SUPPORT
		if(txcfg->ndpa)
			pdesc->Dword3 |= set_desc(TXdesc_92E_NAVUSEHDR);
#endif		

		if (i != (txcfg->frg_num - 1))
		{
			SetMFrag(pwlhdr);
			if (i == 0) {
				fr_len = (txcfg->frag_thrshld - txcfg->llc);
				tx_len -= (txcfg->frag_thrshld - txcfg->llc);
			}
			else {
				fr_len = txcfg->frag_thrshld;
				tx_len -= txcfg->frag_thrshld;
			}
		}
		else	// last seg, or the only seg (frg_num == 1)
		{
			fr_len = tx_len;
			ClearMFrag(pwlhdr);
		}
		SetFragNum((pwlhdr), i);

		if ((i == 0) && (txcfg->fr_type == _SKB_FRAME_TYPE_))
		{
			pdesc->Dword7 |= set_desc((txcfg->hdr_len + txcfg->llc) << TX_TxBufSizeSHIFT);
			pdesc->Dword0 |= set_desc((fr_len + (get_desc(pdesc->Dword7) & TX_TxBufSizeMask)) << TX_PktSizeSHIFT);
			pdesc->Dword0 |= set_desc(TX_FirstSeg);
			pdescinfo->type = _PRE_ALLOCLLCHDR_;
		}
		else
		{
			pdesc->Dword7 |= set_desc(txcfg->hdr_len << TX_TxBufSizeSHIFT);
			pdesc->Dword0 |= set_desc((fr_len + (get_desc(pdesc->Dword7) & TX_TxBufSizeMask)) << TX_PktSizeSHIFT);
			pdesc->Dword0 |= set_desc(TX_FirstSeg);

#ifdef BEAMFORMING_SUPPORT
			if(txcfg->ndpa)
				pdescinfo->type = _PRE_ALLOCLLCHDR_;
			else
#endif				
			pdescinfo->type = _PRE_ALLOCHDR_;
		}

#ifdef _11s_TEST_MODE_
		mesh_debug_tx9(txcfg, pdescinfo);
#endif

		switch (q_num) {
		case HIGH_QUEUE:
			q_select = 0x11;// High Queue
			break;
		case MGNT_QUEUE:
			q_select = 0x12;
			break;
#if defined(DRVMAC_LB) && defined(WIFI_WMM)
		case BE_QUEUE:
			q_select = 0;
			break;
#endif
		default:
			// data packet
#ifdef RTL_MANUAL_EDCA
			if (priv->pmib->dot11QosEntry.ManualEDCA) {
				switch (q_num) {
				case VO_QUEUE:
					q_select = 6;
					break;
				case VI_QUEUE:
					q_select = 4;
					break;
				case BE_QUEUE:
					q_select = 0;
					break;
				default:
					q_select = 1;
					break;
				}
			}
			else
#endif
			q_select = ((struct sk_buff *)txcfg->pframe)->cb[1];
				break;
		}
		pdesc->Dword1 |= set_desc((q_select & TX_QSelMask)<< TX_QSelSHIFT);

		if (i != (txcfg->frg_num - 1))
			pdesc->Dword2 |= set_desc(TX_MoreFrag);

// Set RateID
		if (txcfg->pstat) {
			if (txcfg->pstat->aid != MANAGEMENT_AID) {
				u8 ratid;

				ratid = txcfg->pstat->ratr_idx;
								
				pdesc->Dword1 |= set_desc((ratid & TXdesc_92E_RateIDMask) << TX_RateIDSHIFT);
				
	// Set MacID
				pdesc->Dword1 |= set_desc(REMAP_AID(txcfg->pstat) & TXdesc_92E_MacIdMask);

#ifdef BEAMFORMING_SUPPORT
				if((priv->pmib->dot11RFEntry.txbf == 1) &&
					((txcfg->pstat->ht_cap_len && (txcfg->pstat->ht_cap_buf.txbf_cap)) ||
					(txcfg->pstat->vht_cap_len && (cpu_to_le32(txcfg->pstat->vht_cap_buf.vht_cap_info) & BIT(SU_BFEE_S))))
				){
					pdesc->Dword2 |= set_desc((txcfg->pstat->p_aid & TX_8812_PAIDMask) << TX_8812_PAIDSHIFT);
					pdesc->Dword2 |= set_desc((txcfg->pstat->g_id & TX_8812_GIDMask) << TX_8812_GIDSHIFT);
				}
#endif
			}
		} else {
			pdesc->Dword1 |= set_desc((ARFR_BMC &TXdesc_92E_RateIDMask)<<TX_RateIDSHIFT);

#ifdef BEAMFORMING_SUPPORT
				if (priv->pmib->dot11RFEntry.txbf == 1) 
					pdesc->Dword2 |= set_desc((TX_8812_GIDMask) << TX_8812_GIDSHIFT);
#endif				

		}
	
		pdesc->Dword4 |= set_desc((0x1f & TXdesc_92E_DataRateFBLmtMask) << TXdesc_92E_DataRateFBLmtSHIFT);
		
		if (txcfg->fixed_rate) {
			pdesc->Dword3 |= set_desc(TXdesc_92E_DisDataFB|TXdesc_92E_DisRtsFB|TXdesc_92E_UseRate);
		}

		if(txcfg->pstat && txcfg->pstat->sta_in_firmware != 1)
			pdesc->Dword3 |= set_desc(TXdesc_92E_UseRate);

#ifdef CONFIG_IEEE80211W
		if (!txcfg->need_ack && txcfg->privacy && UseSwCrypto(priv, NULL, TRUE, txcfg->isPMF))
#else
		if (!txcfg->need_ack && txcfg->privacy && UseSwCrypto(priv, NULL, TRUE))
#endif // CONFIG_IEEE80211W
			pdesc->Dword1 &= set_desc( ~(TX_SecTypeMask<< TX_SecTypeSHIFT));

		if (txcfg->privacy) {
#ifdef CONFIG_IEEE80211W
			if (UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF)) {
#else
			if (UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE))) {
#endif // CONFIG_IEEE80211W
				pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0)+ txcfg->icv + txcfg->mic + txcfg->iv);
				pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7)+ txcfg->iv);
			} else {
				// hw encrypt
				switch(txcfg->privacy) {
				case _WEP_104_PRIVACY_:
				case _WEP_40_PRIVACY_:
					pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0) + txcfg->iv);
					pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + txcfg->iv);
					wep_fill_iv(priv, pwlhdr, txcfg->hdr_len, keyid);
					pdesc->Dword1 |= set_desc(0x1 << TX_SecTypeSHIFT);					
					break;

				case _TKIP_PRIVACY_:
					pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0) + txcfg->iv + txcfg->mic);
					pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + txcfg->iv);
					tkip_fill_encheader(priv, pwlhdr, txcfg->hdr_len, keyid);
					pdesc->Dword1 |= set_desc(0x1 << TX_SecTypeSHIFT);
					break;
				#if defined(CONFIG_RTL_HW_WAPI_SUPPORT)
				case _WAPI_SMS4_:
					pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0) + txcfg->iv);
					pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + txcfg->iv);
					
					pdesc->Dword1 |= set_desc(0x2 << TX_SecTypeSHIFT);
					break;
				#endif
				case _CCMP_PRIVACY_:
					//michal also hardware...
					pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0) + txcfg->iv);
					pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + txcfg->iv);
					aes_fill_encheader(priv, pwlhdr, txcfg->hdr_len, keyid);
					pdesc->Dword1 |= set_desc(0x3 << TX_SecTypeSHIFT);
					break;

				default:
					DEBUG_ERR("Unknow privacy\n");
					break;
				}
			}
		}

#ifdef TX_EARLY_MODE
		if (GET_TX_EARLY_MODE && pwlhdr && i == 0) {
			pdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & 0xff00ffff) | (0x28 << TX_OffsetSHIFT));
			pdesc->Dword1 = set_desc(get_desc(pdesc->Dword1) | (1 << TX_PktOffsetSHIFT));
			pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + 8);

			memset(pwlhdr-8, '\0', 8);			
			if (txcfg->pstat && txcfg->pstat->empkt_num > 0) 			
				insert_emcontent(priv, txcfg, pwlhdr-8);

			pdesc->Dword10 = set_desc(get_physical_addr(priv, pwlhdr-8,
				get_desc(pdesc->Dword7)&TX_TxBufSizeMask, PCI_DMA_TODEVICE));	
		}
		else
#endif
		{
			pdesc->Dword10 = set_desc(get_physical_addr(priv, pwlhdr,
				(get_desc(pdesc->Dword7)&TX_TxBufSizeMask), PCI_DMA_TODEVICE));

		}

		// below is for sw desc info
#ifndef TXDESC_INFO		
		pdescinfo->paddr  = get_desc(pdesc->Dword10);
#endif		
		pdescinfo->pframe = pwlhdr;
#if defined(WIFI_WMM) && defined(WMM_APSD)
		pdescinfo->priv = priv;
#ifndef TXDESC_INFO		
		pdescinfo->pstat = txcfg->pstat;
#endif		
#endif

#ifdef TX_SHORTCUT
		if (!priv->pmib->dot11OperationEntry.disable_txsc && txcfg->pstat &&
				(txcfg->fr_type == _SKB_FRAME_TYPE_) &&
				(txcfg->frg_num == 1) &&
				((txcfg->privacy == 0)
#ifdef CONFIG_RTL_WAPI_SUPPORT
				|| (txcfg->privacy == _WAPI_SMS4_)
#endif
#ifdef CONFIG_IEEE80211W
				|| (txcfg->privacy && !UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))) &&
#else
				|| (txcfg->privacy && !UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))) &&
#endif // CONFIG_IEEE80211W
				!GetMData(txcfg->phdr) ) {

#ifdef CONFIG_RTK_MESH
			if( txcfg->is_11s) {
				idx = get_tx_sc_free_entry(priv, txcfg->pstat, &priv->ethhdr);
			} else
#endif
				idx = get_tx_sc_free_entry(priv, txcfg->pstat, pbuf - sizeof(struct wlan_ethhdr_t));

#ifdef CONFIG_RTK_MESH
			if( txcfg->is_11s) {
				memcpy((void *)&txcfg->pstat->tx_sc_ent[idx].ethhdr, &priv->ethhdr, sizeof(struct wlan_ethhdr_t));
			} else
#endif
				memcpy((void *)&txcfg->pstat->tx_sc_ent[idx].ethhdr, pbuf - sizeof(struct wlan_ethhdr_t), sizeof(struct wlan_ethhdr_t));

			memcpy(&txcfg->pstat->tx_sc_ent[idx].hwdesc1, pdesc, 40);
			descinfo_copy(&txcfg->pstat->tx_sc_ent[idx].swdesc1, pdescinfo);
			txcfg->pstat->protection = priv->pmib->dot11ErpInfo.protection;
			txcfg->pstat->ht_protection = priv->ht_protection;
			txcfg->pstat->tx_sc_ent[idx].sc_keyid = keyid;
			txcfg->pstat->tx_sc_ent[idx].pktpri = ((struct sk_buff *)txcfg->pframe)->cb[1];
			fit_shortcut = 1;
		}
		else {
			if (txcfg->pstat) {
				for (idx=0; idx<TX_SC_ENTRY_NUM; idx++)
					txcfg->pstat->tx_sc_ent[idx].hwdesc1.Dword7 &= set_desc(~TX_TxBufSizeMask);
			}
		}
#endif

		pfrst_dma_desc = dma_txhead[*tx_head];

		if (i != 0) {
			pdesc->Dword0 |= set_desc(TX_OWN);
#ifndef USE_RTL8186_SDK
			rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif
		}

		flush_addr[flush_num]  = (unsigned long)bus_to_virt(get_desc(pdesc->Dword10));
		flush_len[flush_num++]= (get_desc(pdesc->Dword7) & TX_TxBufSizeMask);

/*
		//printk desc content
		{
			unsigned int *ppdesc = (unsigned int *)pdesc;
			printk("%08x    %08x    %08x \n", get_desc(*(ppdesc)), get_desc(*(ppdesc+1)), get_desc(*(ppdesc+2)));
			printk("%08x    %08x    %08x \n", get_desc(*(ppdesc+3)), get_desc(*(ppdesc+4)), get_desc(*(ppdesc+5)));
			printk("%08x    %08x    %08x \n", get_desc(*(ppdesc+6)), get_desc(*(ppdesc+7)), get_desc(*(ppdesc+8)));
			printk("%08x\n", *(ppdesc+9));
			printk("===================================================\n");
		}
*/

		txdesc_rollover(pdesc, (unsigned int *)tx_head);

		if (txcfg->fr_len == 0)
		{
			if (txcfg->aggre_en != FG_AGGRE_MSDU_FIRST)
				pdesc->Dword0 |= set_desc(TX_LastSeg);
			goto init_deschead;
		}

		/*------------------------------------------------------------*/
		/*              fill descriptor of frame body                 */
		/*------------------------------------------------------------*/
		pndesc     = phdesc + *tx_head;
		pndescinfo = pswdescinfo + *tx_head;
		//clear all bits

		memset(pndesc, 0,40);
		pndesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | (TX_OWN));



#if (defined(CONFIG_HW_ANTENNA_DIVERSITY))
	if(txcfg->pstat)
		ODM_SetTxAntByTxInfo(priv, pdesc, txcfg, txcfg->pstat->aid);  //AC-series Set TX antenna
#endif


		if (txcfg->privacy)
		{
			if (txcfg->privacy == _WAPI_SMS4_)
			{
				if (txcfg->aggre_en != FG_AGGRE_MSDU_FIRST)
					pndesc->Dword0 |= set_desc(TX_LastSeg);
#ifndef TXDESC_INFO						
				pndescinfo->pstat = txcfg->pstat;
				pndescinfo->rate = txcfg->tx_rate;
#endif				
			}
#ifdef CONFIG_IEEE80211W			
			else if (!UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE),txcfg->isPMF))
#else
			else if (!UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))
#endif // CONFIG_IEEE80211W
			{
				if (txcfg->aggre_en != FG_AGGRE_MSDU_FIRST)
					pndesc->Dword0 |= set_desc(TX_LastSeg);
#ifndef TXDESC_INFO						
				pndescinfo->pstat = txcfg->pstat;
				pndescinfo->rate = txcfg->tx_rate;
#endif				
			}
		}
		else
		{
			if (txcfg->aggre_en != FG_AGGRE_MSDU_FIRST)
				pndesc->Dword0 |= set_desc(TX_LastSeg);
#ifndef TXDESC_INFO					
			pndescinfo->pstat = txcfg->pstat;
			pndescinfo->rate = txcfg->tx_rate;
#endif			
		}

#ifdef CONFIG_RTL_WAPI_SUPPORT
#if defined(CONFIG_RTL_HW_WAPI_SUPPORT)
#ifdef CONFIG_IEEE80211W
		if ((txcfg->privacy == _WAPI_SMS4_)&&(UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF)))
#else
		if ((txcfg->privacy == _WAPI_SMS4_)&&(UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE))))
#endif
#else
		if (txcfg->privacy == _WAPI_SMS4_)
#endif
		{
			pndesc->Dword7 |= set_desc( (fr_len+SMS4_MIC_LEN) & TX_TxBufSizeMask);
		}
		else
#endif
		pndesc->Dword7 |= set_desc(fr_len & TX_TxBufSizeMask);

		if (i == 0)
			pndescinfo->type = txcfg->fr_type;
		else
			pndescinfo->type = _RESERVED_FRAME_TYPE_;

#if defined(CONFIG_RTK_MESH) && defined(MESH_USE_METRICOP)
		if( (txcfg->fr_type == _PRE_ALLOCMEM_) && (txcfg->is_11s & 128)) // for 11s link measurement frame
			pndescinfo->type =_RESERVED_FRAME_TYPE_;
#endif

#ifdef _11s_TEST_MODE_
		mesh_debug_tx10(txcfg, pndescinfo);
#endif


		pndesc->Dword10 = set_desc(tmpphyaddr); //TxBufferAddr
#ifndef TXDESC_INFO		
		pndescinfo->paddr = get_desc(pndesc->Dword10);
#endif		
		pndescinfo->pframe = txcfg->pframe;
#ifndef TXDESC_INFO		
		pndescinfo->len = txcfg->fr_len;	// for pci_unmap_single
#endif		
		pndescinfo->priv = priv;

		pbuf += fr_len;
		tmpphyaddr += fr_len;

#ifdef TX_SHORTCUT
		if (fit_shortcut) {
			memcpy(&txcfg->pstat->tx_sc_ent[idx].hwdesc2, pndesc, 40);
			descinfo_copy(&txcfg->pstat->tx_sc_ent[idx].swdesc2, pndescinfo);
		}
#endif

#ifndef USE_RTL8186_SDK
		rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

		flush_addr[flush_num]=(unsigned long)bus_to_virt(get_desc(pndesc->Dword10));
		flush_len[flush_num++] = get_desc(pndesc->Dword7) & TX_TxBufSizeMask;

		txdesc_rollover(pndesc, (unsigned int *)tx_head);

		// retrieve H/W MIC and put in payload
#ifdef CONFIG_RTL_WAPI_SUPPORT
		if (txcfg->privacy == _WAPI_SMS4_)
		{
			SecSWSMS4Encryption(priv, txcfg);
#if 0
			if (txcfg->mic>0)
			{
				pndesc->Dword7 &= set_desc(~TX_TxBufSizeMask);
				pndesc->Dword7 |= set_desc((fr_len+txcfg->mic)& TX_TxBufSizeMask);
				flush_len[flush_num-1]= (get_desc(pndesc->Dword7) & TX_TxBufSizeMask);
			}
			else
			{
				txcfg->mic = SMS4_MIC_LEN;
			}
#endif
		
		}
#endif

#ifndef NOT_RTK_BSP
		if ((txcfg->privacy == _TKIP_PRIVACY_) &&
			(priv->pshare->have_hw_mic) &&
			!(priv->pmib->dot11StationConfigEntry.swTkipMic) &&
			(i == (txcfg->frg_num-1)) )	// last segment
		{
			register unsigned long int l,r;
			unsigned char *mic;
			volatile int i;

			while ((*(volatile unsigned int *)GDMAISR & GDMA_COMPIP) == 0)
				for (i=0; i<10; i++)
					;

			l = *(volatile unsigned int *)GDMAICVL;
			r = *(volatile unsigned int *)GDMAICVR;

			mic = ((struct sk_buff *)txcfg->pframe)->data + txcfg->fr_len - 8;
			mic[0] = (unsigned char)(l & 0xff);
			mic[1] = (unsigned char)((l >> 8) & 0xff);
			mic[2] = (unsigned char)((l >> 16) & 0xff);
			mic[3] = (unsigned char)((l >> 24) & 0xff);
			mic[4] = (unsigned char)(r & 0xff);
			mic[5] = (unsigned char)((r >> 8) & 0xff);
			mic[6] = (unsigned char)((r >> 16) & 0xff);
			mic[7] = (unsigned char)((r >> 24) & 0xff);

#ifdef MICERR_TEST
			if (priv->micerr_flag) {
				mic[7] ^= mic[7];
				priv->micerr_flag = 0;
			}
#endif
		}
#endif // !NOT_RTK_BSP

		/*------------------------------------------------------------*/
		/*                insert sw encrypt here!                     */
		/*------------------------------------------------------------*/
#ifdef CONFIG_IEEE80211W
		if (txcfg->privacy && UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))
#else
		if (txcfg->privacy && UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))
#endif			
		{
			if (txcfg->privacy == _TKIP_PRIVACY_ ||
				txcfg->privacy == _WEP_40_PRIVACY_ ||
				txcfg->privacy == _WEP_104_PRIVACY_)
			{
				picvdesc     = phdesc + *tx_head;
				picvdescinfo = pswdescinfo + *tx_head;
				//clear all bits
				memset(picvdesc, 0,32);

				if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST){
					picvdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | TX_OWN);
				}
				else{
					picvdesc->Dword0   = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | TX_OWN | TX_LastSeg);
				}

				picvdesc->Dword7 |= (set_desc(txcfg->icv & TX_TxBufSizeMask)); //TxBufferSize

				// append ICV first...
				picv = get_icv_from_poll(priv);
				if (picv == NULL)
				{
					DEBUG_ERR("System-Buf! can't alloc picv\n");
					BUG();
				}

				picvdescinfo->type = _PRE_ALLOCICVHDR_;
				picvdescinfo->pframe = picv;
#ifndef TXDESC_INFO					
				picvdescinfo->pstat = txcfg->pstat;
				picvdescinfo->rate = txcfg->tx_rate;
#endif				
				picvdescinfo->priv = priv;
				//TxBufferAddr

				picvdesc->Dword10 = set_desc(get_physical_addr(priv, picv, txcfg->icv, PCI_DMA_TODEVICE));

				if (i == 0)
					tkip_icv(picv,
						pwlhdr + txcfg->hdr_len + txcfg->iv, txcfg->llc,
						pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask));
				else
					tkip_icv(picv,
						NULL, 0,
						pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask));

				if ((i == 0) && (txcfg->llc != 0)) {
					if (txcfg->privacy == _TKIP_PRIVACY_)
						tkip_encrypt(priv, pwlhdr, txcfg->hdr_len,
							pwlhdr + txcfg->hdr_len + 8, sizeof(struct llc_snap),
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), picv, txcfg->icv);
					else
						wep_encrypt(priv, pwlhdr, txcfg->hdr_len,
							pwlhdr + txcfg->hdr_len + 4, sizeof(struct llc_snap),
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), picv, txcfg->icv,
							txcfg->privacy);
				}
				else { // not first segment or no snap header
					if (txcfg->privacy == _TKIP_PRIVACY_)
						tkip_encrypt(priv, pwlhdr, txcfg->hdr_len, NULL, 0,
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), picv, txcfg->icv);
					else
						wep_encrypt(priv, pwlhdr, txcfg->hdr_len, NULL, 0,
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), picv, txcfg->icv,
							txcfg->privacy);
				}
#ifndef USE_RTL8186_SDK
				rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

				flush_addr[flush_num]  = (unsigned long)bus_to_virt(get_desc(picvdesc->Dword10)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET);
				flush_len[flush_num++]=(get_desc(picvdesc->Dword7) & TX_TxBufSizeMask);

				txdesc_rollover(picvdesc, (unsigned int *)tx_head);
			}

			else if (txcfg->privacy == _CCMP_PRIVACY_)
			{
				pmicdesc = phdesc + *tx_head;
				pmicdescinfo = pswdescinfo + *tx_head;

				//clear all bits
				memset(pmicdesc, 0,32);
				
				if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST)
					pmicdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | TX_OWN);
				else
				  pmicdesc->Dword0   = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | TX_OWN | TX_LastSeg);

				// set TxBufferSize
				pmicdesc->Dword7 = set_desc(txcfg->mic & TX_TxBufSizeMask);

				// append MIC first...
				pmic = get_mic_from_poll(priv);
				if (pmic == NULL)
				{
					DEBUG_ERR("System-Buf! can't alloc pmic\n");
					BUG();
				}

				pmicdescinfo->type = _PRE_ALLOCMICHDR_;
				pmicdescinfo->pframe = pmic;
#ifndef TXDESC_INFO					
				pmicdescinfo->pstat = txcfg->pstat;
				pmicdescinfo->rate = txcfg->tx_rate;
#endif				
				pmicdescinfo->priv = priv;
				// set TxBufferAddr
				pmicdesc->Dword10= set_desc(get_physical_addr(priv, pmic, txcfg->mic, PCI_DMA_TODEVICE));

				// then encrypt all (including ICV) by AES
				if ((i == 0)&&(txcfg->llc != 0)) // encrypt 3 segments ==> llc, mpdu, and mic
				{
				
#ifdef CONFIG_IEEE80211W
					if(isBIP) {
						BIP_encrypt(priv, pwlhdr, pwlhdr + txcfg->hdr_len + 8,
									pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic,txcfg->isPMF);
					} 
					else {
						aesccmp_encrypt(priv, pwlhdr, pwlhdr + txcfg->hdr_len + 8,
										pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic,txcfg->isPMF);
					}
#else
					aesccmp_encrypt(priv, pwlhdr, pwlhdr + txcfg->hdr_len + 8,
									pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic);
#endif
				
				}
				else {// encrypt 2 segments ==> mpdu and mic
#ifdef CONFIG_IEEE80211W
					if(isBIP) 
					{
						BIP_encrypt(priv, pwlhdr, NULL,
									pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic,txcfg->isPMF);
					}
					else
					{
						aesccmp_encrypt(priv, pwlhdr, NULL,
										pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic,txcfg->isPMF);
					}
#else // !defined(CONFIG_IEEE80211W)
					aesccmp_encrypt(priv, pwlhdr, NULL,
									pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic);
#endif // CONFIG_IEEE80211W
				}
#ifndef USE_RTL8186_SDK
				rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

				flush_addr[flush_num]=(unsigned long)bus_to_virt(get_desc(pmicdesc->Dword10)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET);
				flush_len[flush_num++]= (get_desc(pmicdesc->Dword7) & TX_TxBufSizeMask);

				txdesc_rollover(pmicdesc, (unsigned int *)tx_head);
			}
		}
	}


init_deschead:
#if 0
	switch (q_select) {
	case 0:
	case 3:
	   if (q_num != BE_QUEUE)
    		printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
	   break;
	case 1:
	case 2:
		if (q_num != BK_QUEUE)
		    printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
	   break;
	case 4:
	case 5:
		if (q_num != VI_QUEUE)
		    printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
		break;
	case 6:
	case 7:
		if (q_num != VO_QUEUE)
			printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
		break;
	case 0x11 :
		 if (q_num != HIGH_QUEUE)
			printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
		break;
	case 0x12 :
		if (q_num != MGNT_QUEUE)
			printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
		break;
	default :
		printk("%s %d warning : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
	break;
	}
#endif

	for (i=0; i<flush_num; i++)
		rtl_cache_sync_wback(priv, flush_addr[i], flush_len[i], PCI_DMA_TODEVICE);

	if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST) {
		priv->amsdu_first_desc = pfrstdesc;
#ifndef USE_RTL8186_SDK
		priv->amsdu_first_dma_desc = pfrst_dma_desc;
#endif
		priv->amsdu_len = get_desc(pfrstdesc->Dword0) & 0xffff; // get pktSize
		return 0;
	}

	pfrstdesc->Dword0 |= set_desc(TX_OWN);
#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(pfrst_dma_desc-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

	if (q_num == HIGH_QUEUE) {
//		priv->pshare->pkt_in_hiQ = 1;
		priv->pkt_in_hiQ = 1;

		return 0;
	} else {
		tx_poll(priv, q_num);
	}

	return 0;
}
#endif // CONFIG_RTL_8812_SUPPORT

// I AM not sure that if our Buffersize and PKTSize is right,
// If there are any problem, fix this first
#if(CONFIG_WLAN_NOT_HAL_EXIST==1)
int rtl8192cd_signin_txdesc(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
	struct tx_desc		*phdesc, *pdesc, *pndesc, *picvdesc, *pmicdesc, *pfrstdesc;
	struct tx_desc_info	*pswdescinfo, *pdescinfo, *pndescinfo, *picvdescinfo, *pmicdescinfo;
	unsigned int 		fr_len, tx_len, i, keyid;
	int					*tx_head, q_num;
	unsigned long		tmpphyaddr;
	unsigned char		*da, *pbuf, *pwlhdr, *pmic, *picv;
	struct rtl8192cd_hw	*phw;
	unsigned char		 q_select;
#ifdef TX_SHORTCUT
	int					fit_shortcut=0, idx=0;
#endif
	unsigned long		pfrst_dma_desc=0;
	unsigned long		*dma_txhead;

	unsigned long		flush_addr[20];
	int					flush_len[20];
	int					flush_num=0;

#ifdef TX_SCATTER
	int				actual_size = 0;
	struct sk_buff	*skb=NULL;
#endif

#ifdef CONFIG_RTL_8812_SUPPORT
	if(GET_CHIP_VER(priv)== VERSION_8812E){
		return rtl8192cd_signin_txdesc_8812(priv, txcfg);
	}
#endif

	picvdesc=NULL;
	keyid=0;
	pmic=NULL;
	picv=NULL;
	q_select=0;

	if (txcfg->tx_rate == 0) {
		DEBUG_ERR("tx_rate=0!\n");
		txcfg->tx_rate = find_rate(priv, NULL, 0, 1);
	}

	q_num = txcfg->q_num;

	phw	= GET_HW(priv);

	dma_txhead	= get_txdma_addr(phw, q_num);
	tx_head		= get_txhead_addr(phw, q_num);
	phdesc   	= get_txdesc(phw, q_num);
	pswdescinfo = get_txdesc_info(priv->pshare->pdesc_info, q_num);

	da = get_da((unsigned char *)txcfg->phdr);
#ifdef CONFIG_IEEE80211W
		unsigned int	isBIP = 0;	
	
		if(txcfg->isPMF && IS_MCAST(da)) 
		{
			isBIP = 1;
			txcfg->iv = 0;
			txcfg->fr_len += 10; // 10: MMIE length
		}
#endif

	tx_len = txcfg->fr_len;

	if (txcfg->fr_type == _SKB_FRAME_TYPE_)
		pbuf = ((struct sk_buff *)txcfg->pframe)->data;
	else
		pbuf = (unsigned char*)txcfg->pframe;


#ifdef TX_SCATTER
	if (txcfg->fr_type == _SKB_FRAME_TYPE_ &&
			((struct sk_buff *)txcfg->pframe)->list_num > 0) {
		skb = (struct sk_buff *)txcfg->pframe;
		actual_size = skb->len;
	} else {
		actual_size = tx_len;
	}

	tmpphyaddr = get_physical_addr(priv, pbuf, actual_size, PCI_DMA_TODEVICE);
#else
	tmpphyaddr = get_physical_addr(priv, pbuf, tx_len, PCI_DMA_TODEVICE);
#endif

	// in case of default key, then find the key id
	if (GetPrivacy((txcfg->phdr)))
	{
#ifdef WDS
		if (txcfg->wdsIdx >= 0) {
			if (txcfg->pstat)
				keyid = txcfg->pstat->keyid;
			else
				keyid = 0;
		}
		else
#endif

#ifdef __DRAYTEK_OS__
		if (!IEEE8021X_FUN)
			keyid = priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		else {
			if (IS_MCAST(GetAddr1Ptr ((unsigned char *)txcfg->phdr)) || !txcfg->pstat)
				keyid = priv->pmib->dot11GroupKeysTable.keyid;
			else
				keyid = txcfg->pstat->keyid;
		}
#else

		if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==_WEP_40_PRIVACY_ ||
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm==_WEP_104_PRIVACY_) {
			if(IEEE8021X_FUN && txcfg->pstat) {
#ifdef A4_STA
				if (IS_MCAST(da) && !(txcfg->pstat->state & WIFI_A4_STA))
#else
				if(IS_MCAST(da))
#endif					
					keyid = 0;
				else
					keyid = txcfg->pstat->keyid;
			}
			else
				keyid = priv->pmib->dot1180211AuthEntry.dot11PrivacyKeyIndex;
		}
#endif


	}

	for(i=0, pfrstdesc= phdesc + (*tx_head); i < txcfg->frg_num; i++)
	{
		/*------------------------------------------------------------*/
		/*           fill descriptor of header + iv + llc             */
		/*------------------------------------------------------------*/
		pdesc     = phdesc + (*tx_head);
		pdescinfo = pswdescinfo + *tx_head;

		//clear all bits
		memset(pdesc, 0, 32);

		if (i != 0)
		{
			// we have to allocate wlan_hdr
			pwlhdr = (UINT8 *)get_wlanhdr_from_poll(priv);
			if (pwlhdr == (UINT8 *)NULL)
			{
				DEBUG_ERR("System-bug... should have enough wlan_hdr\n");
				return (txcfg->frg_num - i);
			}
			// other MPDU will share the same seq with the first MPDU
			memcpy((void *)pwlhdr, (void *)(txcfg->phdr), txcfg->hdr_len); // data pkt has 24 bytes wlan_hdr
			pdesc->Dword3 |= set_desc((GetSequence(txcfg->phdr) & TX_SeqMask)  << TX_SeqSHIFT);
		}
		else
		{
#ifdef WIFI_WMM
			if (txcfg->pstat && (is_qos_data(txcfg->phdr))) {
				if ((GetFrameSubType(txcfg->phdr) & (WIFI_DATA_TYPE | BIT(6) | BIT(7))) == (WIFI_DATA_TYPE | BIT(7))) {
					unsigned char tempQosControl[2];
					memset(tempQosControl, 0, 2);
					tempQosControl[0] = ((struct sk_buff *)txcfg->pframe)->cb[1];
#ifdef WMM_APSD
					if (
#ifdef CLIENT_MODE
						(OPMODE & WIFI_AP_STATE) &&
#endif
						(APSD_ENABLE) && (txcfg->pstat) && (txcfg->pstat->state & WIFI_SLEEP_STATE) &&
						(!GetMData(txcfg->phdr)) &&
						((((tempQosControl[0] == 7) || (tempQosControl[0] == 6)) && (txcfg->pstat->apsd_bitmap & 0x01)) ||
						 (((tempQosControl[0] == 5) || (tempQosControl[0] == 4)) && (txcfg->pstat->apsd_bitmap & 0x02)) ||
						 (((tempQosControl[0] == 3) || (tempQosControl[0] == 0)) && (txcfg->pstat->apsd_bitmap & 0x08)) ||
						 (((tempQosControl[0] == 2) || (tempQosControl[0] == 1)) && (txcfg->pstat->apsd_bitmap & 0x04))))
						tempQosControl[0] |= BIT(4);
#endif
					if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST)
						tempQosControl[0] |= BIT(7);

					if (priv->pmib->dot11nConfigEntry.dot11nTxNoAck)
						tempQosControl[0] |= BIT(5);

					memcpy((void *)GetQosControl((txcfg->phdr)), tempQosControl, 2);
				}
			}
#endif

			assign_wlanseq(GET_HW(priv), txcfg->phdr, txcfg->pstat, GET_MIB(priv)
#ifdef CONFIG_RTK_MESH	// For broadcast data frame via mesh (ex:ARP requst)
				, txcfg->is_11s
#endif
				);
			pdesc->Dword3 |= set_desc((GetSequence(txcfg->phdr) & TX_SeqMask)  << TX_SeqSHIFT);
			pwlhdr = txcfg->phdr;
		}
		SetDuration(pwlhdr, 0);

		rtl8192cd_fill_fwinfo(priv, txcfg, pdesc, i);

#if (defined(CONFIG_HW_ANTENNA_DIVERSITY))
		if(txcfg->pstat)
		{
			ODM_SetTxAntByTxInfo(priv, pdesc, txcfg, txcfg->pstat->aid);   //N-series Set TX antenna	
		}
#endif
#if defined (HW_ANT_SWITCH) && (defined(CONFIG_RTL_92C_SUPPORT) || defined(CONFIG_RTL_92D_SUPPORT))
		pdesc->Dword2 &= set_desc(~ BIT(24));
		pdesc->Dword2 &= set_desc(~ BIT(25));
		if(!(priv->pshare->rf_ft_var.CurAntenna & 0x80) && (txcfg->pstat)) {
			pdesc->Dword2 |= set_desc(((txcfg->pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)<<24);
			pdesc->Dword2 |= set_desc(((txcfg->pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)<<25);
		}
#endif

		pdesc->Dword0 |= set_desc(32 << TX_OffsetSHIFT); // tx_desc size

		if (IS_MCAST(GetAddr1Ptr((unsigned char *)txcfg->phdr)))
			pdesc->Dword0 |= set_desc(TX_BMC);

#ifdef CLIENT_MODE
		if (OPMODE & WIFI_STATION_STATE) {
			if (GetFrameSubType(txcfg->phdr) == WIFI_PSPOLL)
				pdesc->Dword1 |= set_desc(TX_NAVUSEHDR);

			if (priv->ps_state)
				SetPwrMgt(pwlhdr);
			else
				ClearPwrMgt(pwlhdr);
		}
#endif

		if (i != (txcfg->frg_num - 1))
		{
			SetMFrag(pwlhdr);
			if (i == 0) {
				fr_len = (txcfg->frag_thrshld - txcfg->llc);
				tx_len -= (txcfg->frag_thrshld - txcfg->llc);
			}
			else {
				fr_len = txcfg->frag_thrshld;
				tx_len -= txcfg->frag_thrshld;
			}
		}
		else	// last seg, or the only seg (frg_num == 1)
		{
			fr_len = tx_len;
			ClearMFrag(pwlhdr);
		}
		SetFragNum((pwlhdr), i);

		if ((i == 0) && (txcfg->fr_type == _SKB_FRAME_TYPE_))
		{
			pdesc->Dword7 |= set_desc((txcfg->hdr_len + txcfg->llc) << TX_TxBufSizeSHIFT);
			pdesc->Dword0 |= set_desc((fr_len + (get_desc(pdesc->Dword7) & TX_TxBufSizeMask)) << TX_PktSizeSHIFT);
			pdesc->Dword0 |= set_desc(TX_FirstSeg);
			pdescinfo->type = _PRE_ALLOCLLCHDR_;
		}
		else
		{
			pdesc->Dword7 |= set_desc(txcfg->hdr_len << TX_TxBufSizeSHIFT);
			pdesc->Dword0 |= set_desc((fr_len + (get_desc(pdesc->Dword7) & TX_TxBufSizeMask)) << TX_PktSizeSHIFT);
			pdesc->Dword0 |= set_desc(TX_FirstSeg);
			pdescinfo->type = _PRE_ALLOCHDR_;
		}

#ifdef _11s_TEST_MODE_
		mesh_debug_tx9(txcfg, pdescinfo);
#endif

		switch (q_num) {
		case HIGH_QUEUE:
			q_select = 0x11;// High Queue
			break;
		case MGNT_QUEUE:
			q_select = 0x12;
			break;
#if defined(DRVMAC_LB) && defined(WIFI_WMM)
		case BE_QUEUE:
			q_select = 0;
			break;
#endif
		default:
			// data packet
#ifdef RTL_MANUAL_EDCA
			if (priv->pmib->dot11QosEntry.ManualEDCA) {
				switch (q_num) {
				case VO_QUEUE:
					q_select = 6;
					break;
				case VI_QUEUE:
					q_select = 4;
					break;
				case BE_QUEUE:
					q_select = 0;
					break;
				default:
					q_select = 1;
					break;
				}
			}
			else
#endif
			q_select = ((struct sk_buff *)txcfg->pframe)->cb[1];
				break;
		}
		pdesc->Dword1 |= set_desc((q_select & TX_QSelMask)<< TX_QSelSHIFT);

		if (i != (txcfg->frg_num - 1))
			pdesc->Dword2 |= set_desc(TX_MoreFrag);

// Set RateID
		if (txcfg->pstat) {
			if (txcfg->pstat->aid != MANAGEMENT_AID) {
				u8 ratid;

#ifdef CONFIG_RTL_92D_SUPPORT
				if (priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G){
					if (txcfg->pstat->tx_ra_bitmap & 0xffff000) {
						if (priv->pshare->is_40m_bw)
							ratid = ARFR_2T_Band_A_40M;
						else
							ratid = ARFR_2T_Band_A_20M;
					} else {
						ratid = ARFR_G_ONLY;
					}
				} else 
#endif
				{			
					if (txcfg->pstat->tx_ra_bitmap & 0xff00000) {
						if (priv->pshare->is_40m_bw)
							ratid = ARFR_2T_40M;
						else
							ratid = ARFR_2T_20M;
					} else if (txcfg->pstat->tx_ra_bitmap & 0xff000) {
						if (priv->pshare->is_40m_bw)
							ratid = ARFR_1T_40M;
						else
							ratid = ARFR_1T_20M;
					} else if (txcfg->pstat->tx_ra_bitmap & 0xff0) {
						ratid = ARFR_BG_MIX;
					} else {
						ratid = ARFR_B_ONLY;
					}


#ifdef P2P_SUPPORT	/*tx to GC no user B rate*/
					if(txcfg->pstat->is_p2p_client){
						switch(ratid) {
							case  ARFR_BG_MIX :
								ratid = ARFR_G_ONLY;
								break;
							default:
								ratid = ARFR_2T_Band_A_40M;		
						}
					}
#endif				
				}
				pdesc->Dword1 |= set_desc((ratid & TX_RateIDMask) << TX_RateIDSHIFT);
	// Set MacID
#ifdef CONFIG_RTL_88E_SUPPORT
				if (GET_CHIP_VER(priv)==VERSION_8188E)
					pdesc->Dword1 |= set_desc(REMAP_AID(txcfg->pstat) & TXdesc_88E_MacIdMask);
				else
#endif
					pdesc->Dword1 |= set_desc(REMAP_AID(txcfg->pstat) & TX_MacIdMask);
			}
		} else {
	
#ifdef CONFIG_RTL_92D_SUPPORT
		if (priv->pmib->dot11RFEntry.phyBandSelect & PHY_BAND_5G)
			pdesc->Dword1 |= set_desc((ARFR_Band_A_BMC &TX_RateIDMask)<<TX_RateIDSHIFT);
		else
#endif
			pdesc->Dword1 |= set_desc((ARFR_BMC &TX_RateIDMask)<<TX_RateIDSHIFT);
	}

		pdesc->Dword5 |= set_desc((0x1f & TX_DataRateFBLmtMask) << TX_DataRateFBLmtSHIFT);
		if (txcfg->fixed_rate)
			pdesc->Dword4 |= set_desc(TX_DisDataFB|TX_DisRtsFB|TX_UseRate);
#ifdef CONFIG_RTL_88E_SUPPORT
		else if (GET_CHIP_VER(priv)==VERSION_8188E)
			pdesc->Dword4 |= set_desc(TX_UseRate);
#endif

		if(txcfg->pstat &&  txcfg->pstat->sta_in_firmware != 1)
			pdesc->Dword4 |= set_desc(TX_UseRate);

		if (!txcfg->need_ack && txcfg->privacy && 
#ifdef CONFIG_IEEE80211W
			UseSwCrypto(priv, NULL, TRUE, txcfg->isPMF)
#else
			UseSwCrypto(priv, NULL, TRUE)
#endif		
	 	)
			pdesc->Dword1 &= set_desc( ~(TX_SecTypeMask<< TX_SecTypeSHIFT));

		if (txcfg->privacy) {
#ifdef CONFIG_IEEE80211W
			if (UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF
#else
			if (UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)
#endif				
			)){
				pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0)+ txcfg->icv + txcfg->mic + txcfg->iv);
				pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7)+ txcfg->iv);
			} else {
				// hw encrypt
				switch(txcfg->privacy) {
				case _WEP_104_PRIVACY_:
				case _WEP_40_PRIVACY_:
					pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0) + txcfg->iv);
					pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + txcfg->iv);
					wep_fill_iv(priv, pwlhdr, txcfg->hdr_len, keyid);
					pdesc->Dword1 |= set_desc(0x1 << TX_SecTypeSHIFT);
					break;

				case _TKIP_PRIVACY_:
					pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0) + txcfg->iv + txcfg->mic);
					pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + txcfg->iv);
					tkip_fill_encheader(priv, pwlhdr, txcfg->hdr_len, keyid);
					pdesc->Dword1 |= set_desc(0x1 << TX_SecTypeSHIFT);
					break;
				#if defined(CONFIG_RTL_HW_WAPI_SUPPORT)
				case _WAPI_SMS4_:
					pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0) + txcfg->iv);
					pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + txcfg->iv);
					
					pdesc->Dword1 |= set_desc(0x2 << TX_SecTypeSHIFT);
					break;
				#endif
				case _CCMP_PRIVACY_:
					//michal also hardware...
					pdesc->Dword0 = set_desc(get_desc(pdesc->Dword0) + txcfg->iv);
					pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + txcfg->iv);
					aes_fill_encheader(priv, pwlhdr, txcfg->hdr_len, keyid);
					pdesc->Dword1 |= set_desc(0x3 << TX_SecTypeSHIFT);
					break;

				default:
					DEBUG_ERR("Unknow privacy\n");
					break;
				}
			}
		}

#ifdef TX_EARLY_MODE
		if (GET_TX_EARLY_MODE && pwlhdr && i == 0) {
			pdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & 0xff00ffff) | (0x28 << TX_OffsetSHIFT));
			pdesc->Dword1 = set_desc(get_desc(pdesc->Dword1) | (1 << TX_PktOffsetSHIFT));
#ifdef CONFIG_RTL_88E_SUPPORT
			if (GET_CHIP_VER(priv) == VERSION_8188E) {
				pdesc->Dword6 &= set_desc(~ (0xf << TX_MaxAggNumSHIFT));
				pdesc->Dword6 |= set_desc(0xf << TX_MaxAggNumSHIFT);
			}
#endif
			pdesc->Dword7 = set_desc(get_desc(pdesc->Dword7) + 8);

			memset(pwlhdr-8, '\0', 8);			
			if (txcfg->pstat && txcfg->pstat->empkt_num > 0) 			
				insert_emcontent(priv, txcfg, pwlhdr-8);
			pdesc->Dword8 = set_desc(get_physical_addr(priv, pwlhdr-8,
				get_desc(pdesc->Dword7)&TX_TxBufSizeMask, PCI_DMA_TODEVICE));	
		}
		else
#endif
		{
#if defined(TX_EARLY_MODE) && defined(CONFIG_RTL_88E_SUPPORT)
			if (GET_CHIP_VER(priv) == VERSION_8188E)
				pdesc->Dword6 &= set_desc(~ (0xf << TX_MaxAggNumSHIFT));
#endif
			pdesc->Dword8 = set_desc(get_physical_addr(priv, pwlhdr,
				(get_desc(pdesc->Dword7)&TX_TxBufSizeMask), PCI_DMA_TODEVICE));
		}

		// below is for sw desc info
#ifndef TXDESC_INFO
		pdescinfo->paddr  = get_desc(pdesc->Dword8);
#endif
		pdescinfo->pframe = pwlhdr;
#if defined(WIFI_WMM) && defined(WMM_APSD)
		pdescinfo->priv = priv;
#ifndef TXDESC_INFO		
		pdescinfo->pstat = txcfg->pstat;
#endif		
#endif

#ifdef TX_SHORTCUT
		if (!priv->pmib->dot11OperationEntry.disable_txsc && txcfg->pstat &&
				(txcfg->fr_type == _SKB_FRAME_TYPE_) &&
				(txcfg->frg_num == 1) &&
				((txcfg->privacy == 0)
#ifdef CONFIG_RTL_WAPI_SUPPORT
				|| (txcfg->privacy == _WAPI_SMS4_)
#endif
#ifdef CONFIG_IEEE80211W
				|| (txcfg->privacy && !UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE),txcfg->isPMF))) &&
#else
				|| (txcfg->privacy && !UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))) &&
#endif // CONFIG_IEEE80211W
				!GetMData(txcfg->phdr) ) {

#ifdef CONFIG_RTK_MESH
			if( txcfg->is_11s) {
				idx = get_tx_sc_free_entry(priv, txcfg->pstat, &priv->ethhdr);
			} else
#endif
				idx = get_tx_sc_free_entry(priv, txcfg->pstat, pbuf - sizeof(struct wlan_ethhdr_t));

#ifdef CONFIG_RTK_MESH
			if( txcfg->is_11s) {
				memcpy((void *)&txcfg->pstat->tx_sc_ent[idx].ethhdr, &priv->ethhdr, sizeof(struct wlan_ethhdr_t));
			} else
#endif
#ifdef TX_SCATTER
			if (((struct sk_buff *)txcfg->pframe)->list_num > 0)
				memcpy((void *)&txcfg->pstat->tx_sc_ent[idx].ethhdr, ((struct sk_buff *)txcfg->pframe)->list_buf[0].buf, sizeof(struct wlan_ethhdr_t));
			else	
#endif
				memcpy((void *)&txcfg->pstat->tx_sc_ent[idx].ethhdr, pbuf - sizeof(struct wlan_ethhdr_t), sizeof(struct wlan_ethhdr_t));

			desc_copy(&txcfg->pstat->tx_sc_ent[idx].hwdesc1, pdesc);
			descinfo_copy(&txcfg->pstat->tx_sc_ent[idx].swdesc1, pdescinfo);
			txcfg->pstat->protection = priv->pmib->dot11ErpInfo.protection;
			txcfg->pstat->ht_protection = priv->ht_protection;
			txcfg->pstat->tx_sc_ent[idx].sc_keyid = keyid;
			txcfg->pstat->tx_sc_ent[idx].pktpri = ((struct sk_buff *)txcfg->pframe)->cb[1];
			fit_shortcut = 1;
		}
		else {
			if (txcfg->pstat) {
				for (idx=0; idx<TX_SC_ENTRY_NUM; idx++) {
					txcfg->pstat->tx_sc_ent[idx].hwdesc1.Dword7 &= set_desc(~TX_TxBufSizeMask);
#ifdef TX_SCATTER
					txcfg->pstat->tx_sc_ent[idx].has_desc3 = 0;
#endif
				}
			}
		}
#endif

		pfrst_dma_desc = dma_txhead[*tx_head];

		if (i != 0) {
			pdesc->Dword0 |= set_desc(TX_OWN);
#ifndef USE_RTL8186_SDK
			rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif
		}

		flush_addr[flush_num]  = (unsigned long)bus_to_virt(get_desc(pdesc->Dword8)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET);
		flush_len[flush_num++]= (get_desc(pdesc->Dword7) & TX_TxBufSizeMask);

/*
		//printk desc content
		{
			unsigned int *ppdesc = (unsigned int *)pdesc;
			printk("%08x    %08x    %08x \n", get_desc(*(ppdesc)), get_desc(*(ppdesc+1)), get_desc(*(ppdesc+2)));
			printk("%08x    %08x    %08x \n", get_desc(*(ppdesc+3)), get_desc(*(ppdesc+4)), get_desc(*(ppdesc+5)));
			printk("%08x    %08x    %08x \n", get_desc(*(ppdesc+6)), get_desc(*(ppdesc+7)), get_desc(*(ppdesc+8)));
			printk("%08x\n", *(ppdesc+9));
			printk("===================================================\n");
		}
*/

		txdesc_rollover(pdesc, (unsigned int *)tx_head);

		if (txcfg->fr_len == 0)
		{
			if (txcfg->aggre_en != FG_AGGRE_MSDU_FIRST)
				pdesc->Dword0 |= set_desc(TX_LastSeg);
			goto init_deschead;
		}

#ifdef TX_SCATTER
fill_body:
#endif
		/*------------------------------------------------------------*/
		/*              fill descriptor of frame body                 */
		/*------------------------------------------------------------*/
		pndesc     = phdesc + *tx_head;
		pndescinfo = pswdescinfo + *tx_head;
		//clear all bits
		memset(pndesc, 0,32);
		pndesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | (TX_OWN));


#if defined (HW_ANT_SWITCH) && (defined(CONFIG_RTL_92C_SUPPORT) || defined(CONFIG_RTL_92D_SUPPORT))
		pndesc->Dword2 &= set_desc(~ BIT(24));
		pndesc->Dword2 &= set_desc(~ BIT(25));
		if(!(priv->pshare->rf_ft_var.CurAntenna & 0x80) && (txcfg->pstat)) {
			pndesc->Dword2 |= set_desc(((txcfg->pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)<<24);
			pndesc->Dword2 |= set_desc(((txcfg->pstat->CurAntenna^priv->pshare->rf_ft_var.CurAntenna)&1)<<25);
		}
#endif


		if (txcfg->privacy)
		{
			if (txcfg->privacy == _WAPI_SMS4_)
			{
				if (txcfg->aggre_en != FG_AGGRE_MSDU_FIRST)
					pndesc->Dword0 |= set_desc(TX_LastSeg);
#ifndef TXDESC_INFO	
				pndescinfo->pstat = txcfg->pstat;
				pndescinfo->rate = txcfg->tx_rate;
#endif				
			}

#ifdef CONFIG_IEEE80211W
			else if (!UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))
#else
			else if (!UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))
#endif		
			{
				if (txcfg->aggre_en != FG_AGGRE_MSDU_FIRST) {
#ifdef TX_SCATTER
					if (!skb || (skb && ((skb->list_idx+1) == skb->list_num))) 
#endif
						pndesc->Dword0 |= set_desc(TX_LastSeg);
				}
#ifndef TXDESC_INFO					
				pndescinfo->pstat = txcfg->pstat;
				pndescinfo->rate = txcfg->tx_rate;
#endif				
			}
		}
		else
		{
			if (txcfg->aggre_en != FG_AGGRE_MSDU_FIRST) {
#ifdef TX_SCATTER
				if (!skb || (skb && ((skb->list_idx+1) == skb->list_num)))
#endif
					pndesc->Dword0 |= set_desc(TX_LastSeg);
			}
#ifndef TXDESC_INFO				
			pndescinfo->pstat = txcfg->pstat;
			pndescinfo->rate = txcfg->tx_rate;
#endif			
		}
#ifdef TX_SCATTER
		if (skb != NULL)
			fr_len = actual_size;
#endif		

#ifdef CONFIG_RTL_WAPI_SUPPORT
#if defined(CONFIG_RTL_HW_WAPI_SUPPORT)
#ifdef CONFIG_IEEE80211W
		if ((txcfg->privacy == _WAPI_SMS4_)&&(UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF)))
#else
		if ((txcfg->privacy == _WAPI_SMS4_)&&(UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE))))
#endif	// CONFIG_IEEE80211W
#else
		if (txcfg->privacy == _WAPI_SMS4_)
#endif
		{
			pndesc->Dword7 |= set_desc( (fr_len+SMS4_MIC_LEN) & TX_TxBufSizeMask);
		}
		else
#endif
		pndesc->Dword7 |= set_desc(fr_len & TX_TxBufSizeMask);

#ifdef TX_SCATTER
		if (skb && (skb->list_num > 0)) {
			if (get_desc(pndesc->Dword0) & TX_LastSeg)
				pndescinfo->type = txcfg->fr_type;
			else
				pndescinfo->type = _RESERVED_FRAME_TYPE_;
		} else
#endif
		{
			if (i == 0)
				pndescinfo->type = txcfg->fr_type;
			else
				pndescinfo->type = _RESERVED_FRAME_TYPE_;
		}

#if defined(CONFIG_RTK_MESH) && defined(MESH_USE_METRICOP)
		if( (txcfg->fr_type == _PRE_ALLOCMEM_) && (txcfg->is_11s & 128)) // for 11s link measurement frame
			pndescinfo->type =_RESERVED_FRAME_TYPE_;
#endif

#ifdef _11s_TEST_MODE_
		mesh_debug_tx10(txcfg, pndescinfo);
#endif

		pndesc->Dword8 = set_desc(tmpphyaddr); //TxBufferAddr
#ifndef TXDESC_INFO
		pndescinfo->paddr = get_desc(pndesc->Dword8);
#endif		
		pndescinfo->pframe = txcfg->pframe;
#ifndef TXDESC_INFO		
		pndescinfo->len = txcfg->fr_len;	// for pci_unmap_single
#endif		
		pndescinfo->priv = priv;

		pbuf += fr_len;
		tmpphyaddr += fr_len;

#ifdef TX_SHORTCUT
		if (fit_shortcut) {
#ifdef TX_SCATTER
			if (txcfg->pstat->tx_sc_ent[idx].has_desc3) {
				desc_copy(&txcfg->pstat->tx_sc_ent[idx].hwdesc3, pndesc);
				descinfo_copy(&txcfg->pstat->tx_sc_ent[idx].swdesc3, pndescinfo);
			} else
#endif
			{
				desc_copy(&txcfg->pstat->tx_sc_ent[idx].hwdesc2, pndesc);
				descinfo_copy(&txcfg->pstat->tx_sc_ent[idx].swdesc2, pndescinfo);
			}
		}
#endif

#ifndef USE_RTL8186_SDK
		rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

		flush_addr[flush_num]=(unsigned long)bus_to_virt(get_desc(pndesc->Dword8)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET);
		flush_len[flush_num++] = get_desc(pndesc->Dword7) & TX_TxBufSizeMask;

		txdesc_rollover(pndesc, (unsigned int *)tx_head);

		// retrieve H/W MIC and put in payload
#ifdef CONFIG_RTL_WAPI_SUPPORT
		if (txcfg->privacy == _WAPI_SMS4_)
		{
			SecSWSMS4Encryption(priv, txcfg);
#if 0
			if (txcfg->mic>0)
			{
				pndesc->Dword7 &= set_desc(~TX_TxBufSizeMask);
				pndesc->Dword7 |= set_desc((fr_len+txcfg->mic)& TX_TxBufSizeMask);
				flush_len[flush_num-1]= (get_desc(pndesc->Dword7) & TX_TxBufSizeMask);
			}
			else
			{
				txcfg->mic = SMS4_MIC_LEN;
			}
#endif
		
		}
#endif

#ifndef NOT_RTK_BSP
		if ((txcfg->privacy == _TKIP_PRIVACY_) &&
			(priv->pshare->have_hw_mic) &&
			!(priv->pmib->dot11StationConfigEntry.swTkipMic) &&
			(i == (txcfg->frg_num-1)) )	// last segment
		{
			register unsigned long int l,r;
			unsigned char *mic;
			volatile int i;

			while ((*(volatile unsigned int *)GDMAISR & GDMA_COMPIP) == 0)
				for (i=0; i<10; i++)
					;

			l = *(volatile unsigned int *)GDMAICVL;
			r = *(volatile unsigned int *)GDMAICVR;

			mic = ((struct sk_buff *)txcfg->pframe)->data + txcfg->fr_len - 8;
			mic[0] = (unsigned char)(l & 0xff);
			mic[1] = (unsigned char)((l >> 8) & 0xff);
			mic[2] = (unsigned char)((l >> 16) & 0xff);
			mic[3] = (unsigned char)((l >> 24) & 0xff);
			mic[4] = (unsigned char)(r & 0xff);
			mic[5] = (unsigned char)((r >> 8) & 0xff);
			mic[6] = (unsigned char)((r >> 16) & 0xff);
			mic[7] = (unsigned char)((r >> 24) & 0xff);

#ifdef MICERR_TEST
			if (priv->micerr_flag) {
				mic[7] ^= mic[7];
				priv->micerr_flag = 0;
			}
#endif
		}
#endif // !NOT_RTK_BSP

		/*------------------------------------------------------------*/
		/*                insert sw encrypt here!                     */
		/*------------------------------------------------------------*/
		if (txcfg->privacy &&
#ifdef CONFIG_IEEE80211W
			UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))
#else
			UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))
#endif
		{
			if (txcfg->privacy == _TKIP_PRIVACY_ ||
				txcfg->privacy == _WEP_40_PRIVACY_ ||
				txcfg->privacy == _WEP_104_PRIVACY_)
			{
				picvdesc     = phdesc + *tx_head;
				picvdescinfo = pswdescinfo + *tx_head;
				//clear all bits
				memset(picvdesc, 0,32);

				if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST){
					picvdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | TX_OWN);
				}
				else{
					picvdesc->Dword0   = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | TX_OWN | TX_LastSeg);
				}

				picvdesc->Dword7 |= (set_desc(txcfg->icv & TX_TxBufSizeMask)); //TxBufferSize

				// append ICV first...
				picv = get_icv_from_poll(priv);
				if (picv == NULL)
				{
					DEBUG_ERR("System-Buf! can't alloc picv\n");
					BUG();
				}

				picvdescinfo->type = _PRE_ALLOCICVHDR_;
				picvdescinfo->pframe = picv;
#ifndef TXDESC_INFO					
				picvdescinfo->pstat = txcfg->pstat;
				picvdescinfo->rate = txcfg->tx_rate;
#endif				
				picvdescinfo->priv = priv;
				//TxBufferAddr
				picvdesc->Dword8 = set_desc(get_physical_addr(priv, picv, txcfg->icv, PCI_DMA_TODEVICE));

				if (i == 0)
					tkip_icv(picv,
						pwlhdr + txcfg->hdr_len + txcfg->iv, txcfg->llc,
						pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask));
				else
					tkip_icv(picv,
						NULL, 0,
						pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask));

				if ((i == 0) && (txcfg->llc != 0)) {
					if (txcfg->privacy == _TKIP_PRIVACY_)
						tkip_encrypt(priv, pwlhdr, txcfg->hdr_len,
							pwlhdr + txcfg->hdr_len + 8, sizeof(struct llc_snap),
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), picv, txcfg->icv);
					else
						wep_encrypt(priv, pwlhdr, txcfg->hdr_len,
							pwlhdr + txcfg->hdr_len + 4, sizeof(struct llc_snap),
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), picv, txcfg->icv,
							txcfg->privacy);
				}
				else { // not first segment or no snap header
					if (txcfg->privacy == _TKIP_PRIVACY_)
						tkip_encrypt(priv, pwlhdr, txcfg->hdr_len, NULL, 0,
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), picv, txcfg->icv);
					else
						wep_encrypt(priv, pwlhdr, txcfg->hdr_len, NULL, 0,
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), picv, txcfg->icv,
							txcfg->privacy);
				}
#ifndef USE_RTL8186_SDK
				rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

				flush_addr[flush_num]  = (unsigned long)bus_to_virt(get_desc(picvdesc->Dword8)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET);
				flush_len[flush_num++]=(get_desc(picvdesc->Dword7) & TX_TxBufSizeMask);

				txdesc_rollover(picvdesc, (unsigned int *)tx_head);
			}

			else if (txcfg->privacy == _CCMP_PRIVACY_)
			{
				pmicdesc = phdesc + *tx_head;
				pmicdescinfo = pswdescinfo + *tx_head;

				//clear all bits
				memset(pmicdesc, 0,32);
				
				if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST)
					pmicdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | TX_OWN);
				else
				  pmicdesc->Dword0   = set_desc((get_desc(pdesc->Dword0) & (~TX_FirstSeg)) | TX_OWN | TX_LastSeg);

				// set TxBufferSize
				pmicdesc->Dword7 = set_desc(txcfg->mic & TX_TxBufSizeMask);

				// append MIC first...
				pmic = get_mic_from_poll(priv);
				if (pmic == NULL)
				{
					DEBUG_ERR("System-Buf! can't alloc pmic\n");
					BUG();
				}

				pmicdescinfo->type = _PRE_ALLOCMICHDR_;
				pmicdescinfo->pframe = pmic;
#ifndef TXDESC_INFO					
				pmicdescinfo->pstat = txcfg->pstat;
				pmicdescinfo->rate = txcfg->tx_rate;
#endif				
				pmicdescinfo->priv = priv;
				// set TxBufferAddr
				pmicdesc->Dword8= set_desc(get_physical_addr(priv, pmic, txcfg->mic, PCI_DMA_TODEVICE));

				// then encrypt all (including ICV) by AES
				if ((i == 0)&&(txcfg->llc != 0)) // encrypt 3 segments ==> llc, mpdu, and mic
				{
#ifdef CONFIG_IEEE80211W
					if(isBIP) 
					{
						BIP_encrypt(priv, pwlhdr, pwlhdr + txcfg->hdr_len + 8,
									pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic,txcfg->isPMF);
					}
					else {
						aesccmp_encrypt(priv, pwlhdr, pwlhdr + txcfg->hdr_len + 8,
										pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic,txcfg->isPMF);
					}
#else
					aesccmp_encrypt(priv, pwlhdr, pwlhdr + txcfg->hdr_len + 8,
									pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic);
#endif	
				}
				else { // encrypt 2 segments ==> mpdu and mic
#ifdef CONFIG_IEEE80211W
					if(isBIP) 
					{
						BIP_encrypt(priv, pwlhdr, NULL,
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic,txcfg->isPMF);
					}
					else {
						aesccmp_encrypt(priv, pwlhdr, NULL,
							pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic,txcfg->isPMF);
					}
#else // !defined(CONFIG_IEEE80211W)
					aesccmp_encrypt(priv, pwlhdr, NULL,
						pbuf - (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), (get_desc(pndesc->Dword7) & TX_TxBufSizeMask), pmic);
#endif // CONFIG_IEEE80211W
				}
#ifndef USE_RTL8186_SDK
				rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif
				flush_addr[flush_num]=(unsigned long)bus_to_virt(get_desc(pmicdesc->Dword8)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET);
				flush_len[flush_num++]= (get_desc(pmicdesc->Dword7) & TX_TxBufSizeMask);

				txdesc_rollover(pmicdesc, (unsigned int *)tx_head);
			}
		}

#ifdef TX_SCATTER
		if (skb && ++skb->list_idx < skb->list_num) {
			skb_assign_buf(skb, skb->list_buf[skb->list_idx].buf, skb->list_buf[skb->list_idx].len);
			skb->len = skb->list_buf[skb->list_idx].len;
			pbuf = skb->data;
			actual_size = skb->len;
			tmpphyaddr = get_physical_addr(priv, pbuf, actual_size, PCI_DMA_TODEVICE);
#ifdef TX_SHORTCUT
			if (txcfg->pstat) {
				if (txcfg->pstat->tx_sc_ent[idx].has_desc3) {
					fit_shortcut = 0;
					txcfg->pstat->tx_sc_ent[idx].has_desc3 = 0;
					for (idx=0; idx<TX_SC_ENTRY_NUM; idx++)
						txcfg->pstat->tx_sc_ent[idx].hwdesc1.Dword7 &= set_desc(~TX_TxBufSizeMask);
				} else {
					txcfg->pstat->tx_sc_ent[idx].has_desc3 = 1;
				}
			}
#endif
			goto fill_body;
		}
#endif
	}


init_deschead:
#if 0
	switch (q_select) {
	case 0:
	case 3:
	   if (q_num != BE_QUEUE)
    		printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
	   break;
	case 1:
	case 2:
		if (q_num != BK_QUEUE)
		    printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
	   break;
	case 4:
	case 5:
		if (q_num != VI_QUEUE)
		    printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
		break;
	case 6:
	case 7:
		if (q_num != VO_QUEUE)
			printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
		break;
	case 0x11 :
		 if (q_num != HIGH_QUEUE)
			printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
		break;
	case 0x12 :
		if (q_num != MGNT_QUEUE)
			printk("%s %d error : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
		break;
	default :
		printk("%s %d warning : q_select[%d], q_num[%d]\n", __FUNCTION__, __LINE__, q_select, q_num);
	break;
	}
#endif

	for (i=0; i<flush_num; i++)
		rtl_cache_sync_wback(priv, flush_addr[i], flush_len[i], PCI_DMA_TODEVICE);

	if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST) {
		priv->amsdu_first_desc = pfrstdesc;
#ifndef USE_RTL8186_SDK
		priv->amsdu_first_dma_desc = pfrst_dma_desc;
#endif
		priv->amsdu_len = get_desc(pfrstdesc->Dword0) & 0xffff; // get pktSize
		return 0;
	}

	pfrstdesc->Dword0 |= set_desc(TX_OWN);
#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(pfrst_dma_desc-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

	if (q_num == HIGH_QUEUE) {
//		priv->pshare->pkt_in_hiQ = 1;
		priv->pkt_in_hiQ = 1;

		return 0;
	} else {
		tx_poll(priv, q_num);
	}

	return 0;
}
#else
int rtl8192cd_signin_txdesc(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
	return 0;
}
#endif//CONFIG_WLAN_NOT_HAL_EXIST
#endif // CONFIG_PCI_HCI


#ifdef SUPPORT_TX_AMSDU
int rtl8192cd_signin_txdesc_amsdu(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
	struct tx_desc *phdesc, *pdesc, *pfrstdesc;
	struct tx_desc_info *pswdescinfo, *pdescinfo;
	unsigned int  tx_len;
	int *tx_head, q_num;
	unsigned long	tmpphyaddr;
	unsigned char *pbuf;
	struct rtl8192cd_hw *phw;
	unsigned long *dma_txhead;

	q_num = txcfg->q_num;
	phw	= GET_HW(priv);

	dma_txhead	= get_txdma_addr(phw, q_num);
	tx_head		= get_txhead_addr(phw, q_num);
	phdesc   	= get_txdesc(phw, q_num);
	pswdescinfo = get_txdesc_info(priv->pshare->pdesc_info, q_num);

	pbuf = ((struct sk_buff *)txcfg->pframe)->data;
	tx_len = ((struct sk_buff *)txcfg->pframe)->len;
	tmpphyaddr = get_physical_addr(priv, pbuf, tx_len, PCI_DMA_TODEVICE);
#ifdef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, (unsigned long)pbuf, tx_len, PCI_DMA_TODEVICE);
#endif

	pdesc     = phdesc + (*tx_head);
	pdescinfo = pswdescinfo + *tx_head;

	//clear all bits
#ifdef CONFIG_RTL_8812_SUPPORT
	if(GET_CHIP_VER(priv)== VERSION_8812E)
	{
		memset(pdesc, 0, 40);					//clear all bits
		pdesc->Dword10 = set_desc(tmpphyaddr); // TXBufferAddr
		pdesc->Dword7 |= (set_desc(tx_len & TX_TxBufSizeMask));
		pdesc->Dword0 |= set_desc(40 << TX_OffsetSHIFT); // tx_desc size
	}
	else
#endif
	{
		memset(pdesc, 0, 32);
		pdesc->Dword8 = set_desc(tmpphyaddr); // TXBufferAddr
		pdesc->Dword7 |= (set_desc(tx_len & TX_TxBufSizeMask));
		pdesc->Dword0 |= set_desc(32 << TX_OffsetSHIFT); // tx_desc size
	}

	if (txcfg->aggre_en == FG_AGGRE_MSDU_LAST){
		pdesc->Dword0 = set_desc(TX_OWN | TX_LastSeg);
	}
	else
		pdesc->Dword0 = set_desc(TX_OWN);

#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

	pdescinfo->type = _SKB_FRAME_TYPE_;

//#ifdef CONFIG_RTL_8812_SUPPORT
//	if(GET_CHIP_VER(priv)== VERSION_8812E)
//	pdescinfo->paddr = get_desc(pdesc->Dword10); // TXBufferAddr
//	else
//#endif
//	pdescinfo->paddr = get_desc(pdesc->Dword8); // TXBufferAddr

	pdescinfo->pframe = txcfg->pframe;
//	pdescinfo->len = txcfg->fr_len;
	pdescinfo->priv = priv;

	txdesc_rollover(pdesc, (unsigned int *)tx_head);

	priv->amsdu_len += tx_len;

	if (txcfg->aggre_en == FG_AGGRE_MSDU_LAST) {
		pfrstdesc = priv->amsdu_first_desc;
		pfrstdesc->Dword0 = set_desc((get_desc(pfrstdesc->Dword0) &0xff0000) | priv->amsdu_len | TX_FirstSeg | TX_OWN);
#ifdef CONFIG_RTL_8812_SUPPORT
		if(GET_CHIP_VER(priv)== VERSION_8812E)
			pfrstdesc->Dword2 |= set_desc(TXdesc_92E_BK);
#endif

#ifndef USE_RTL8186_SDK
		rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(priv->amsdu_first_dma_desc-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif
		tx_poll(priv, q_num);
	}

	return 0;
}
#endif // SUPPORT_TX_AMSDU


#if 0

#ifdef CONFIG_PCI_HCI
//-----------------------------------------------------------------------------
// Procedure:    Fill Tx Command Packet Descriptor
//
// Description:   This routine fill command packet descriptor. We assum that command packet
//				require only one descriptor.
//
// Arguments:   This function is only for Firmware downloading in current stage
//
// Returns:
//-----------------------------------------------------------------------------
int rtl8192cd_SetupOneCmdPacket(struct rtl8192cd_priv *priv, unsigned char *dat_content, unsigned short txLength, unsigned char LastPkt)
/*
	IN	PADAPTER		Adapter,
	IN	PRT_TCB			pTcb,
	IN	u1Byte			QueueIndex,
	IN	u2Byte			index,
	IN	BOOLEAN			bFirstSeg,
	IN	BOOLEAN			bLastSeg,
	IN	pu1Byte			VirtualAddress,
	IN	u4Byte			PhyAddressLow,
	IN	u4Byte			BufferLen,
	IN	BOOLEAN     		bSetOwnBit,
	IN	BOOLEAN			bLastInitPacket,
	IN    u4Byte			DescPacketType,
	IN	u4Byte			PktLen
	)
*/
{

	unsigned char	ih=0;
	unsigned char	DescNum;
	unsigned short	DebugTimerCount;

	struct tx_desc	*pdesc;
	struct tx_desc	*phdesc;
	volatile unsigned int *ppdesc  ; //= (unsigned int *)pdesc;
	int	*tx_head, *tx_tail;
	struct rtl8192cd_hw	*phw = GET_HW(priv);

	tx_head	= (int *)&phw->txcmdhead;
	tx_tail = (int *)&phw->txcmdtail;
	phdesc = phw->txcmd_desc;

	DebugTimerCount = 0; // initialize debug counter to exit loop
	DescNum = 1;

//TODO: Fill the dma check here

//	printk("data lens: %d\n", txLength );

	for (ih=0; ih<DescNum; ih++) {
		pdesc      = (phdesc + (*tx_head));
		ppdesc = (unsigned int *)pdesc;
		// Clear all status
		memset(pdesc, 0, 36);
//		rtl_cache_sync_wback(priv, phw->txcmd_desc_dma_addr[*tx_head], 32, PCI_DMA_TODEVICE);
		// For firmware downlaod we only need to set LINIP
		if (LastPkt)
			pdesc->Dword0 |= set_desc(TX_LINIP);

		// From Scott --> 92SE must set as 1 for firmware download HW DMA error
		pdesc->Dword0 |= set_desc(TX_FirstSeg);;//bFirstSeg;
		pdesc->Dword0 |= set_desc(TX_LastSeg);;//bLastSeg;

		// 92SE need not to set TX packet size when firmware download
		pdesc->Dword7 |=  (set_desc((unsigned short)(txLength) << TX_TxBufSizeSHIFT));

		memcpy(priv->pshare->txcmd_buf, dat_content, txLength);

		rtl_cache_sync_wback(priv, (unsigned long)priv->pshare->txcmd_buf, txLength, PCI_DMA_TODEVICE);

#ifdef CONFIG_RTL_8812_SUPPORT
		if(GET_CHIP_VER(priv)== VERSION_8812E)
			pdesc->Dword10 =  set_desc(priv->pshare->cmdbuf_phyaddr);
		else
#endif
		pdesc->Dword8 =  set_desc(priv->pshare->cmdbuf_phyaddr);


//		pdesc->Dword0	|= set_desc((unsigned short)(txLength) << TX_PktIDSHIFT);
		pdesc->Dword0	|= set_desc((unsigned short)(txLength) << TX_PktSizeSHIFT);
		//if (bSetOwnBit)
		{
			pdesc->Dword0 |= set_desc(TX_OWN);
//			*(ppdesc) |= set_desc(BIT(31));
		}

#ifndef USE_RTL8186_SDK
		rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(phw->txcmd_desc_dma_addr[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif
		*tx_head = (*tx_head + 1) & (NUM_CMD_DESC - 1);
	}

	return TRUE;
}
#endif // CONFIG_PCI_HCI
#endif

#ifdef CONFIG_RTK_MESH
#ifndef __LINUX_2_6__
__MIPS16
#endif
int reuse_meshhdr(struct rtl8192cd_priv *priv, struct tx_insn *txcfg)
{
	const short meshhdrlen= (txcfg->mesh_header.mesh_flag & 0x01) ? 16 : 4;
	struct sk_buff *pskb = (struct sk_buff *)txcfg->pframe;
	if (skb_cloned(pskb))
	{
		struct sk_buff	*newskb = skb_copy(pskb, GFP_ATOMIC);
		rtl_kfree_skb(priv, pskb, _SKB_TX_);
		if (newskb == NULL) {
			priv->ext_stats.tx_drops++;
			release_wlanllchdr_to_poll(priv, txcfg->phdr);
			DEBUG_ERR("TX DROP: Can't copy the skb!\n");
			return 0;
		}
		txcfg->pframe = pskb = newskb;
#ifdef ENABLE_RTL_SKB_STATS
		rtl_atomic_inc(&priv->rtl_tx_skb_cnt);
#endif
	}
	memcpy(skb_push(pskb,meshhdrlen), &(txcfg->mesh_header), meshhdrlen);
	txcfg->fr_len += meshhdrlen;
	return 1;
}
#endif //CONFIG_RTK_MESH


#if defined(WIFI_WMM) && defined(DRVMAC_LB)
void SendLbQosNullData(struct rtl8192cd_priv *priv)
{
	struct wifi_mib *pmib;
	unsigned char *hwaddr;
	unsigned char tempQosControl[2];
	DECLARE_TXINSN(txinsn);


	pmib = GET_MIB(priv);
	hwaddr = pmib->dot11OperationEntry.hwaddr;

	if(!pmib->miscEntry.drvmac_lb) {
		printk("LB mode disabled, cannot SendLbQosNullData!!!\n");
		return;
	}

	if(!memcmp(pmib->miscEntry.lb_da, "\x0\x0\x0\x0\x0\x0", 6)) {
		printk("LB addr is NULL, cannot SendLbQosNullData!!!\n");
		return;
	}

	txinsn.retry = priv->pmib->dot11OperationEntry.dot11ShortRetryLimit;
	txinsn.q_num = BE_QUEUE;
//	txinsn.q_num = MANAGE_QUE_NUM;
	txinsn.tx_rate = find_rate(priv, NULL, 0, 1);
#ifndef TX_LOWESTRATE	
	txinsn.lowest_tx_rate = txinsn.tx_rate;
#endif	
	txinsn.fixed_rate = 1;
	txinsn.phdr = get_wlanhdr_from_poll(priv);
	txinsn.pframe = NULL;

	if (txinsn.phdr == NULL)
		goto send_qos_null_fail;

	memset((void *)(txinsn.phdr), 0, sizeof (struct	wlan_hdr));

	SetFrameSubType(txinsn.phdr, BIT(7) | WIFI_DATA_NULL);
	SetFrDs(txinsn.phdr);
	memcpy((void *)GetAddr1Ptr((txinsn.phdr)), pmib->miscEntry.lb_da, MACADDRLEN);
	memcpy((void *)GetAddr2Ptr((txinsn.phdr)), hwaddr, MACADDRLEN);
	memcpy((void *)GetAddr3Ptr((txinsn.phdr)), hwaddr, MACADDRLEN);
	txinsn.hdr_len = WLAN_HDR_A3_QOS_LEN;

	memset(tempQosControl, 0, 2);
//	tempQosControl[0] = 0x07;		//set priority to VO
//	tempQosControl[0] |= BIT(4);	//set EOSP
	memcpy((void *)GetQosControl((txinsn.phdr)), tempQosControl, 2);

	if ((rtl8192cd_firetx(priv, &txinsn)) == SUCCESS)
		return;

send_qos_null_fail:

	if (txinsn.phdr)
		release_wlanhdr_to_poll(priv, txinsn.phdr);
}


void SendLbQosData(struct rtl8192cd_priv *priv)
{
	struct wifi_mib *pmib;
	unsigned char *hwaddr;
	unsigned char tempQosControl[2];
	unsigned char	*pbuf;
	static unsigned int pkt_length = 1;
	DECLARE_TXINSN(txinsn);

	pmib = GET_MIB(priv);
	hwaddr = pmib->dot11OperationEntry.hwaddr;

	if(!pmib->miscEntry.drvmac_lb) {
		printk("LB mode disabled, cannot SendLbQosData!!!\n");
		return;
	}

	if(!memcmp(pmib->miscEntry.lb_da, "\x0\x0\x0\x0\x0\x0", 6)) {
		printk("LB addr is NULL, cannot SendLbQosData!!!\n");
		return;
	}

	txinsn.retry = priv->pmib->dot11OperationEntry.dot11ShortRetryLimit;
	txinsn.q_num = BE_QUEUE;
	txinsn.fr_type = _PRE_ALLOCMEM_;
	txinsn.tx_rate = find_rate(priv, NULL, 0, 1);
#ifndef TX_LOWESTRATE
	txinsn.lowest_tx_rate = txinsn.tx_rate;
#endif	
	txinsn.fixed_rate = 1;
//	txinsn.pframe = NULL;

	pbuf = txinsn.pframe = get_mgtbuf_from_poll(priv);
	if (pbuf == NULL)
		goto send_qos_fail;

	txinsn.phdr = get_wlanhdr_from_poll(priv);
	if (txinsn.phdr == NULL)
		goto send_qos_fail;

	memset((void *)(txinsn.phdr), 0, sizeof (struct	wlan_hdr));
	memset((void *)pbuf, 0, PRE_ALLOCATED_BUFSIZE);

	SetFrameSubType(txinsn.phdr, WIFI_QOS_DATA);
	SetFrDs(txinsn.phdr);
	memcpy((void *)GetAddr1Ptr((txinsn.phdr)), pmib->miscEntry.lb_da, MACADDRLEN);
	memcpy((void *)GetAddr2Ptr((txinsn.phdr)), hwaddr, MACADDRLEN);
	memcpy((void *)GetAddr3Ptr((txinsn.phdr)), hwaddr, MACADDRLEN);
	txinsn.hdr_len = WLAN_HDR_A3_QOS_LEN;

	memset(tempQosControl, 0, 2);
	memcpy((void *)GetQosControl((txinsn.phdr)), tempQosControl, 2);

//	printk("--0x%02x%02x%02x%02x 0x%02x%02x%02x%02x--\n", *pbuf, *(pbuf+1), *(pbuf+2), *(pbuf+3), *(pbuf+4), *(pbuf+5), *(pbuf+6), *(pbuf+7));

	if (pmib->miscEntry.lb_mlmp == 1) {
		// all zero in payload
		memset((void *)pbuf, 0x00, pkt_length);
		pbuf += pkt_length;
		txinsn.fr_len += pkt_length;
	}
	else if (pmib->miscEntry.lb_mlmp == 2) {
		// all 0xff in payload
		memset((void *)pbuf, 0xff, pkt_length);
//		printk("~~0x%02x%02x%02x%02x 0x%02x%02x%02x%02x~~\n", *pbuf, *(pbuf+1), *(pbuf+2), *(pbuf+3), *(pbuf+4), *(pbuf+5), *(pbuf+6), *(pbuf+7));
		pbuf += pkt_length;
		txinsn.fr_len += pkt_length;
	}
	else if ((pmib->miscEntry.lb_mlmp == 3) || (pmib->miscEntry.lb_mlmp == 4)) {
		// all different value in payload, 0x00~0xff
		unsigned int i = 0;
//		printk("~~");
		for (i = 0; i <pkt_length; i++) {
			memset((void *)pbuf, i%0x100, 1);
//			printk("%02x", *pbuf);
			pbuf += 1;
			txinsn.fr_len += 1;
		}
//		printk("~~\n");
	}
	else {
		printk("wrong LB muli-length-multi-packet setting!!\n");
		goto send_qos_fail;
	}

//	if (pkt_length++ >= 600)
	if (pkt_length++ >= 2048)
		pkt_length = 1;

	if ((rtl8192cd_firetx(priv, &txinsn)) == SUCCESS)
		return;

send_qos_fail:

	if (txinsn.phdr)
		release_wlanhdr_to_poll(priv, txinsn.phdr);
	if (txinsn.pframe)
		release_mgtbuf_to_poll(priv, txinsn.pframe);
}
#endif

#ifdef TX_EARLY_MODE
__MIPS16
__IRAM_IN_865X
static void get_tx_early_info(struct rtl8192cd_priv *priv, struct stat_info  *pstat,  struct sk_buff_head *pqueue)
{	
	struct sk_buff *next_skb;

	pstat->empkt_num = 0;
	memset(pstat->empkt_len, '\0', sizeof(pstat->empkt_len));
	
#ifdef CONFIG_RTL_88E_SUPPORT
	if (GET_CHIP_VER(priv) == VERSION_8188E) {
		int tmplen = 0, extra_len = 0;
		
		if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _CCMP_PRIVACY_) {
			extra_len = 16;
		} else if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _TKIP_PRIVACY_) {
			extra_len = 20;
		} else if (priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _WEP_40_PRIVACY_ ||
			priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm == _WEP_104_PRIVACY_) {
			extra_len = 8;
		}
		
		skb_queue_walk(pqueue, next_skb) {
			if ((pstat->empkt_num %2) == 0) {
				pstat->empkt_len[pstat->empkt_num/2] = next_skb->len+WLAN_HDR_A3_LEN+WLAN_CRC_LEN+extra_len;
			} else {
				tmplen = pstat->empkt_len[(pstat->empkt_num-1)/2];
				tmplen += ((tmplen%4)?(4-tmplen%4):0)+4;
				tmplen += next_skb->len+WLAN_HDR_A3_LEN+WLAN_CRC_LEN+extra_len;	
				pstat->empkt_len[(pstat->empkt_num-1)/2] = tmplen;
			}
		
			pstat->empkt_num++;
			if (skb_queue_is_last(pqueue, next_skb))
				break;

			if (pstat->empkt_num >= 10)
				break;
		}	
	} else 
#endif	
	skb_queue_walk(pqueue, next_skb) {
		pstat->empkt_len[pstat->empkt_num++] = next_skb->len;

		if (skb_queue_is_last(pqueue, next_skb))
			break;

		if (pstat->empkt_num >= 5)
			break;
	}	
}
#endif

#ifdef SW_TX_QUEUE
__MIPS16
__IRAM_IN_865X
#if defined(CONFIG_RTK_MESH)
int __rtl8192cd_start_xmit_out(struct sk_buff *skb, struct stat_info *pstat, struct tx_insn *ptxcfg);
#else
int __rtl8192cd_start_xmit_out(struct sk_buff *skb, struct stat_info	*pstat);
#endif



/* when using hw timer,  the parameter "add_timer_timeout" is TSF time
     when using sw timer, the parameter "add_timer_timeout" is jiffies time
*/
__inline__ static void rtl8192cd_swq_settimer(struct rtl8192cd_priv *priv, UINT32 timer_timeout)
{
    UINT32 set_timeout;    
    UINT32 current_time;
    unsigned char set_timer = 0;

    if(timer_timeout == 0) {
        if(priv->pshare->swq_current_timeout) {
            priv->pshare->swq_current_timeout = 0;
            if(priv->pshare->swq_use_hw_timer) {                          
                RTL_W32(TC4_CTRL, 0);           
            }
            else {      
                if (timer_pending(&priv->pshare->swq_sw_timer))
			        del_timer_sync(&priv->pshare->swq_sw_timer);
            }            
        }
    }
    else {
        if(priv->pshare->swq_use_hw_timer) {  
            current_time = RTL_R32(TSFTR1);   
            set_timeout = current_time + SWQ_HWTIMER_MINIMUN;
            if(timer_timeout - set_timeout > SWQ_HWTIMER_MAXIMUN) {                
                timer_timeout = set_timeout;
                set_timeout = SWQ_HWTIMER_MINIMUN;                
            }            
            else {
                set_timeout = timer_timeout - current_time;
            }

            if(priv->pshare->swq_current_timeout) {
                if(timer_timeout + SWQ_HWTIMER_TOLERANCE - priv->pshare->swq_current_timeout > SWQ_HWTIMER_MAXIMUN) {                
                    set_timer = 1;
                }
            }
            else {
                set_timer = 1;
            }

            if(set_timer) {                         
                priv->pshare->swq_current_timeout = (timer_timeout == 0?1:timer_timeout);            
                set_timeout = RTL_MICROSECONDS_TO_GTIMERCOUNTER(set_timeout);                
                set_timeout = BIT26 | BIT24 | (set_timeout & 0x00FFFFFF);  //TC40INT_EN | TC4EN | TC4Data
                RTL_W32(TC4_CTRL, set_timeout);           
            }            
        }
        else {
            if(priv->pshare->swq_current_timeout) {                          
                if(timer_timeout < priv->pshare->swq_current_timeout) {                
                    set_timer = 1;
                }                
            }
            else {            
                set_timer = 1;
            }

            if(set_timer) {                                
                priv->pshare->swq_current_timeout = timer_timeout;            
                mod_timer(&priv->pshare->swq_sw_timer, (unsigned long)timer_timeout);            
            }            
        }
    }
}


/* when using hw timer,  the parameter "add_timer_timeout" is TSF time
     when using sw timer, the parameter "add_timer_timeout" is jiffies time
*/
__inline__ static int rtl8192cd_swq_addtimer(struct rtl8192cd_priv *priv, struct stat_info* pstat, unsigned char qnum, UINT32 add_timer_timeout)
{
    unsigned short timer_index = (priv->pshare->swq_timer_head + 1) % SWQ_TIMER_NUM; 
    if(priv->pshare->swq_timer_tail == timer_index) { /* queue is full */           
        /* find an empty slot */
        for(timer_index = 0 ; timer_index < SWQ_TIMER_NUM; timer_index++) {
            if(timer_index == priv->pshare->swq_timer_tail) {
                continue;
            }
            if(priv->pshare->swq_timer[timer_index].pstat == NULL) {
                 break;
            }
        }

        if(timer_index == SWQ_TIMER_NUM) {
            printk("%s: %s, swq timer overflow!\n", priv->dev->name, __FUNCTION__);
            return -1;
        }
    }

    priv->pshare->swq_timer_head = timer_index;                 
    priv->pshare->swq_timer[timer_index].priv = priv;
    priv->pshare->swq_timer[timer_index].pstat = pstat;
    priv->pshare->swq_timer[timer_index].qnum = qnum;
    priv->pshare->swq_timer[timer_index].timeout = add_timer_timeout;
    pstat->swq.swq_timer_id[qnum] = timer_index + 1;

    return 0;
}


void rtl8192cd_swq_deltimer(struct rtl8192cd_priv *priv, struct stat_info* pstat, unsigned char qnum)
{
    unsigned char set_timer = 0;
    unsigned short temp_tail;
    unsigned short timer_index = pstat->swq.swq_timer_id[qnum]-1;
    UINT32 set_timer_timeout;
    priv->pshare->swq_timer[timer_index].pstat = NULL;
    pstat->swq.swq_timer_id[qnum] = 0; 

    /*dequeu dummy slot */    
     while(priv->pshare->swq_timer_head != priv->pshare->swq_timer_tail) /*check if queue is empty*/
    {
        temp_tail = (priv->pshare->swq_timer_tail + 1) % SWQ_TIMER_NUM;
        if(priv->pshare->swq_timer[temp_tail].pstat != NULL) {
            break;
        }            
        priv->pshare->swq_timer_tail = temp_tail;
    }
  
    if (priv->pshare->swq_timer_head != priv->pshare->swq_timer_tail) // if queue is not empty
    {
        temp_tail = priv->pshare->swq_timer_tail;
        do {            
            temp_tail = (temp_tail + 1) % SWQ_TIMER_NUM;
            if(priv->pshare->swq_timer[temp_tail].pstat != NULL) {
                if(priv->pshare->swq_use_hw_timer) { 
                    if(priv->pshare->swq_timer[temp_tail].timeout - (priv->pshare->swq_current_timeout + SWQ_HWTIMER_MINIMUN) > SWQ_HWTIMER_MAXIMUN) {
                        set_timer = 0;
                        break;
                    }
                    else {
                        if(set_timer == 0) {
                            set_timer_timeout = priv->pshare->swq_timer[temp_tail].timeout;
                        }
                        else {
                            if(priv->pshare->swq_timer[temp_tail].timeout - set_timer_timeout > SWQ_HWTIMER_MAXIMUN) {
                                set_timer_timeout = priv->pshare->swq_timer[temp_tail].timeout;
                            }                     
                        }
                        
                        set_timer = 1;
                    }
                }
                else {
                    if(priv->pshare->swq_timer[temp_tail].timeout <= priv->pshare->swq_current_timeout) {
                        set_timer = 0;
                        break;
                    }
                    else {
                        if(set_timer == 0) {
                            set_timer_timeout = priv->pshare->swq_timer[temp_tail].timeout;
                        }
                        else {
                            if(priv->pshare->swq_timer[temp_tail].timeout < set_timer_timeout) {
                                set_timer_timeout = priv->pshare->swq_timer[temp_tail].timeout;
                            }
                        }                        
                        set_timer = 1;
                    }

                }                
            }
        }while(temp_tail != priv->pshare->swq_timer_head);

        if(set_timer) {
            priv->pshare->swq_current_timeout = 0;
            rtl8192cd_swq_settimer(priv, set_timer_timeout);   /*need to re-setup timer*/              
        }
    }
    else {
        rtl8192cd_swq_settimer(priv, 0);   /*disable timer*/
    }

}

__inline__ static int rtl8192cd_swq_bdfull(struct rtl8192cd_priv *priv, struct stat_info *pstat,  unsigned char qnum)
{
    int *tx_head=NULL, *tx_tail=NULL;

#ifdef CONFIG_WLAN_HAL	 	 
    if(IS_HAL_CHIP(priv))    {
        if(compareAvailableTXBD(priv, 4, qnum, 2))
            return 1;
    } else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif
    {
        tx_head = get_txhead_addr(GET_HW(priv), qnum);
        tx_tail = get_txtail_addr(GET_HW(priv), qnum);
        if((4) > CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC))
            return 1;
    }

    return 0;
}


/*return: 1: not finished,  0: the queue is empty now*/
__MIPS16
__IRAM_IN_865X
__inline__ static int rtl8192cd_swq_dequeue(struct rtl8192cd_priv *priv, struct stat_info *pstat,  unsigned char qnum)
{
    int count = 0;
    
    while(1)
    {
        struct sk_buff *tmpskb;
        tmpskb = skb_dequeue(&pstat->swq.swq_queue[qnum]);
        if (tmpskb == NULL)
            break;

#ifdef TX_EARLY_MODE
        if (GET_TX_EARLY_MODE && GET_EM_SWQ_ENABLE) 
            get_tx_early_info(priv, pstat, &pstat->swq.swq_queue[qnum]);
#endif

#if defined(CONFIG_RTK_MESH)
        __rtl8192cd_start_xmit_out(tmpskb, pstat, NULL);
#else
        __rtl8192cd_start_xmit_out(tmpskb, pstat);
#endif
        count++;
    }

    if(skb_queue_len(&pstat->swq.swq_queue[qnum]) == 0) {        
        pstat->swq.swq_empty[qnum] = 0;        
    }
        
    return count;
}

__inline__ static int rtl8192cd_swq_enqueue(struct rtl8192cd_priv *priv, struct sk_buff *skb, struct stat_info	*pstat)
{
    int q_len, pri, q_num;
    unsigned char need_deque = 0;
    UINT32 tri_time;
    
    pri = get_skb_priority(priv, skb, pstat);
#if defined(RTL_MANUAL_EDCA)
    q_num = PRI_TO_QNUM(priv, pri);
#else
    PRI_TO_QNUM(pri, q_num, priv->pmib->dot11OperationEntry.wifi_specific);
#endif

    if(priv->pshare->swq_use_hw_timer) {
        /*check if we need to enable/disable the swq for the specific q_num and pstat*/
        if(pstat->swq.swq_en[q_num]) {
            if(pstat->tx_avarage < 1000 || 20000000 < pstat->tx_avarage) { /* 8K < TP  or 160M < TP*/
                pstat->swq.swq_en[q_num] = 0; /*disable swq*/  
                if (pstat->swq.swq_timer_id[q_num])
                    rtl8192cd_swq_deltimer(priv, pstat, q_num);
                if(pstat->swq.swq_empty[q_num])
                    rtl8192cd_swq_dequeue(priv, pstat, q_num);                          
            }
        }
        else {
            if(1250 < pstat->tx_avarage && pstat->tx_avarage < 19375000) { /* 10K < TP < 155M*/
                pstat->swq.swq_en[q_num] = 1; /*enable swq*/ 
                pstat->swq.swq_prev_timeout[q_num] = 0;
                pstat->swq.swq_timeout_change[q_num] = 0;
                pstat->swq.swq_keeptime[q_num] = 0;         
           }
        }

        if(pstat->swq.swq_en[q_num] == 0) {
            #if defined(CONFIG_RTK_MESH)
            __rtl8192cd_start_xmit_out(skb, pstat, NULL);
            #else
            __rtl8192cd_start_xmit_out(skb, pstat);
            #endif
            return 0;
        }
    }

    
    pstat->swq.q_used[q_num] = 1;    
    if (!pstat->ADDBA_ready[pri])
    {
        need_deque = 1;
    }
    
    if(need_deque == 0) {    
        q_len = skb_queue_len(&pstat->swq.swq_queue[q_num]);
        #ifdef TX_EARLY_MODE
        if (q_len + 1 >= (GET_EM_SWQ_ENABLE ? MAX_EM_QUE_NUM : pstat->swq.q_aggnum[q_num])) {
        #else
        if (q_len + 1 >= pstat->swq.q_aggnum[q_num]) {
        #endif		
            need_deque = 1;
        } 
    }

    if(need_deque) {
        if (pstat->swq.swq_timer_id[q_num])
            rtl8192cd_swq_deltimer(priv, pstat, q_num);
        if(rtl8192cd_swq_bdfull(priv, pstat, q_num)) {           
            skb_queue_tail(&pstat->swq.swq_queue[q_num], skb);
            pstat->swq.swq_empty[q_num] = 1;                                       
        }
        else {
            if(pstat->swq.swq_empty[q_num])
                rtl8192cd_swq_dequeue(priv, pstat, q_num);

            #if defined(CONFIG_RTK_MESH)
            __rtl8192cd_start_xmit_out(skb, pstat, NULL);
            #else
            __rtl8192cd_start_xmit_out(skb, pstat);        
            #endif
        }
    }
    else {
        skb_queue_tail(&pstat->swq.swq_queue[q_num], skb);
        pstat->swq.swq_empty[q_num] = 1;  
    }

    if(pstat->swq.swq_empty[q_num]) { /*queue not empty, add timer for it*/        
        if (pstat->swq.swq_timer_id[q_num] == 0)
        {           
            if(priv->pshare->swq_use_hw_timer) {
                if (pstat->tx_avarage > 13750000)           // 110M~
                    tri_time = 1;                   
                else if (pstat->tx_avarage > 1875000)       //15M~110M
                    tri_time = 6;                                   
                else if (pstat->tx_avarage > 500000)        //4M~15M
                    tri_time = 10;
                else if (pstat->tx_avarage > 250000)        //2M~4M
                    tri_time = 20;
                else if (pstat->tx_avarage > 100000)        //800K~2M
                    tri_time = 25;
                else if (pstat->tx_avarage > 25000)	        //200K~800K
                    tri_time = 30;                
                else
                    tri_time = 1;
                       
                if(pstat->swq.swq_prev_timeout[q_num] != tri_time) {
                    pstat->swq.swq_prev_timeout[q_num] = tri_time;
                    pstat->swq.swq_timeout_change[q_num] = 1;                                 
                }  
                tri_time = RTL_R32(TSFTR1) + tri_time*1000;
            }
            else {
                if (pstat->tx_avarage > 1875000) 					//15M~
                    tri_time = 1;
                else if (pstat->tx_avarage > 500000) 					//4M~15M
                    tri_time = 30;
                else if (pstat->tx_avarage > 250000) //2M~4M
                    tri_time = 90;
                else if (pstat->tx_avarage > 100000)					 //800K~2M
                    tri_time = 120;              
                else
                    tri_time = 10;    
                
                tri_time = jiffies + RTL_MILISECONDS_TO_JIFFIES(tri_time);
            }

            rtl8192cd_swq_addtimer(priv, pstat, q_num, tri_time);           
            rtl8192cd_swq_settimer(priv, tri_time);   /*need to re-setup timer*/                        
        }
    }

    return 0;
 }

void rtl8192cd_swq_timeout(unsigned long task_priv)
{
    struct rtl8192cd_priv        *priv = (struct rtl8192cd_priv *)task_priv;
    struct sw_tx_queue_timer* swq_timer;
    int head;
    unsigned char add_timer;
    unsigned char need_dequeue;  
    unsigned char set_timer = 0;
    
    UINT32 add_timer_timeout;
    UINT32 set_timer_timeout;
    struct stat_info * pstat;
    struct rtl8192cd_priv *priv_this=NULL;
    unsigned char qnum;    
    UINT32 current_time;
    int bdfull;
    unsigned long x = 0;
    SMP_LOCK_XMIT(x);
    SAVE_INT_AND_CLI(x);
    
    priv->pshare->swq_current_timeout = 0;
    if(priv->pshare->swq_use_hw_timer) {     
        current_time = RTL_R32(TSFTR1);
    }
    else {
        current_time = jiffies;
    }
  

    head = priv->pshare->swq_timer_head;
    while (head != priv->pshare->swq_timer_tail) //check if queue is empty
    {
        priv->pshare->swq_timer_tail = (priv->pshare->swq_timer_tail + 1) % SWQ_TIMER_NUM;    
        swq_timer = &priv->pshare->swq_timer[priv->pshare->swq_timer_tail];
        if (swq_timer->pstat) {           
            pstat = swq_timer->pstat;            
            priv_this = swq_timer->priv;
            qnum = swq_timer->qnum;
            add_timer_timeout = swq_timer->timeout;

            swq_timer->pstat = NULL;
            pstat->swq.swq_timer_id[qnum] = 0;


            if (!(priv_this->drv_state & DRV_STATE_OPEN)) {
                continue;
            }   
           
            need_dequeue = 0;
            if(priv->pshare->swq_use_hw_timer) {     
                if(add_timer_timeout -  (current_time + SWQ_HWTIMER_TOLERANCE) > SWQ_HWTIMER_MAXIMUN) {
                    need_dequeue = 1;
                }
            }
            else {
                if(add_timer_timeout <= current_time) {
                    need_dequeue = 1;
                }
            }
            
            add_timer = 0;
            if(need_dequeue) {  
                bdfull = rtl8192cd_swq_bdfull(priv, pstat, qnum);
                if(bdfull && skb_queue_len(&pstat->swq.swq_queue[qnum]) < pstat->swq.q_aggnum[qnum]*2 ) {
                    add_timer = 1;                             
                    if(priv->pshare->swq_use_hw_timer) {        
                        add_timer_timeout = 1;   
                        add_timer_timeout = add_timer_timeout*1000; // to us
                    }
                    else {
                        add_timer_timeout = 30;   
                        add_timer_timeout = RTL_MILISECONDS_TO_JIFFIES(add_timer_timeout); 
                    }
                    add_timer_timeout += current_time; 

                }
                else {
                    if(priv->pshare->swq_use_hw_timer) {
                        rtl8192cd_swq_dequeue(priv_this, pstat, qnum);
                        pstat->swq.q_TOCount[qnum]++;                        
                    }
                    else {
                        pstat->swq.q_TOCount[qnum] += rtl8192cd_swq_dequeue(priv_this, pstat, qnum); 
                    }
                }
                
                if(!bdfull) {
                    adjust_swq_setting(priv, pstat, qnum, CHECK_DEC_AGGN);
                }
            }
            else {                        
                add_timer = 1;
            }


            if(add_timer) { 
                rtl8192cd_swq_addtimer(priv_this, pstat, qnum, add_timer_timeout);  
                if(set_timer == 0) {
                    set_timer_timeout = add_timer_timeout;
                }
                else {
                    if(priv->pshare->swq_use_hw_timer) { 
                        if(add_timer_timeout - set_timer_timeout > SWQ_HWTIMER_MAXIMUN) {
                            set_timer_timeout = add_timer_timeout;
                        }
                    }
                    else {
                        if(add_timer_timeout < set_timer_timeout) {
                            set_timer_timeout = add_timer_timeout;
                        }                    
                    }
                }                
                
                set_timer = 1;
            }
        }


    }

    if(set_timer) {
        rtl8192cd_swq_settimer(priv, set_timer_timeout);   /*need to re-setup timer*/
    }    

    RESTORE_INT(x);		
    SMP_UNLOCK_XMIT(x);    
}    
#endif

#ifndef SW_TX_QUEUE
__MIPS16
__IRAM_IN_865X
int __rtl8192cd_start_xmit_out(struct sk_buff *skb, struct stat_info *pstat);
#endif

__MIPS16
__IRAM_IN_865X
int __rtl8192cd_start_xmit(struct sk_buff *skb, struct net_device *dev, int tx_flag)
{
	struct rtl8192cd_priv *priv;
	struct stat_info	*pstat=NULL;
	unsigned char		*da;
#ifdef __KERNEL__
#if defined(HS2_SUPPORT) || !defined(CONFIG_PCI_HCI)
	struct sk_buff *newskb = NULL;
#endif
#endif
#ifdef TX_SCATTER
	struct sk_buff *newskb = NULL;
#endif
	struct net_device *wdsDev = NULL;
#ifdef SW_TX_QUEUE
	int swq_out = 0;
#endif
#if defined(CONFIG_RTK_VLAN_SUPPORT)
	struct vlan_info *vlan=NULL;
#endif
	struct tx_insn tx_insn;
#ifdef CONFIG_RTL_EAP_RELAY
	int real_len;
#endif
#ifdef CONFIG_RTL_VLAN_8021Q
    UINT16 vid;
    struct sk_buff *newskb;
#endif
    int temp;
#ifdef SMP_SYNC
	unsigned long flags;
#endif


	DECLARE_TXCFG(txcfg, tx_insn);

#ifdef NETDEV_NO_PRIV
	priv = ((struct rtl8192cd_priv *)netdev_priv(dev))->wlan_priv;
#else
	priv = (struct rtl8192cd_priv *)dev->priv;
#endif

#ifdef ENABLE_RTL_SKB_STATS
	rtl_atomic_inc(&priv->rtl_tx_skb_cnt);
#endif

	if (skb->len < 15)
    {
        _DEBUG_ERR("TX DROP: SKB len small:%d\n", skb->len);
        goto free_and_stop;
    }

    #ifdef CONFIG_RTL_NETSNIPER_SUPPORT
    rtl_netsniper_check(skb, dev);
    #endif

#if defined(CONFIG_RTL_CUSTOM_PASSTHRU)
	if (dev==wlan_device[passThruWanIdx].priv->pWlanDev ||skb->dev==wlan_device[passThruWanIdx].priv->pWlanDev)
	{
#ifdef __ECOS
		if (SUCCESS==rtl_isWlanPassthruFrame(skb->data))
#else
		if (SUCCESS==rtl_isPassthruFrame(skb->data))
#endif
		{
#ifdef UNIVERSAL_REPEATER			
			if(passThruStatusWlan & 0x8) //WISP Mode Enable. default is vxd
			{
			    // TODO: need to add 8881A?, check it
            #if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)||defined (CONFIG_RTL_8881A)
				  unsigned int wispWlanIndex=(passThruStatusWlan&WISP_WLAN_IDX_MASK)>>WISP_WLAN_IDX_RIGHT_SHIFT;
				  if ((wlan_device[wispWlanIndex].priv->drv_state & DRV_STATE_OPEN )&&
				  	 ((GET_MIB(GET_VXD_PRIV((wlan_device[wispWlanIndex].priv)))->dot11OperationEntry.opmode) & WIFI_STATION_STATE))
				  {
					#if 0//def NETDEV_NO_PRIV
				    	//dev=skb->dev=((struct rtl8192cd_priv *)(((struct rtl8192cd_priv *)netdev_priv(wlan_device[wispWlanIndex].priv->pWlanDev))->wlan_priv))->pvxd_priv->dev;
						dev=skb->dev=(((struct rtl8192cd_priv *)netdev_priv(wlan_device[wispWlanIndex].priv->dev))->wlan_priv)->pvxd_priv->dev;
					#else	
						dev=skb->dev=GET_VXD_PRIV(wlan_device[wispWlanIndex].priv)->dev;
					#endif
				  } else {
					goto free_and_stop;
				  }
				#else		
					#if 0//def NETDEV_NO_PRIV
					//dev=skb->dev=((struct rtl8192cd_priv *)(((struct rtl8192cd_priv *)netdev_priv(wlan_device[passThruWanIdx].priv->pWlanDev))->wlan_priv))->pvxd_priv->dev;
					dev=skb->dev=(((struct rtl8192cd_priv *)netdev_priv(wlan_device[wispWlanIndex].priv->dev))->wlan_priv)->pvxd_priv->dev;
					#else	
					dev=skb->dev= GET_VXD_PRIV(wlan_device[passThruWanIdx].priv)->dev;
					#endif
				#endif
			} else
#endif
			{
			    // TODO: need to add 8881A?, check it
				#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_8812_SUPPORT)||defined (CONFIG_RTL_8881A)
				  unsigned int wispWlanIndex=(passThruStatusWlan&WISP_WLAN_IDX_MASK)>>WISP_WLAN_IDX_RIGHT_SHIFT;
				  if ((wlan_device[wispWlanIndex].priv->drv_state & DRV_STATE_OPEN )&&
				  	 (((GET_MIB(wlan_device[wispWlanIndex].priv))->dot11OperationEntry.opmode) & WIFI_STATION_STATE))
				  {
					#ifdef NETDEV_NO_PRIV
				        dev=skb->dev=((struct rtl8192cd_priv *)netdev_priv(wlan_device[wispWlanIndex].priv->pWlanDev))->wlan_priv->dev;
					#else	
						dev=skb->dev=wlan_device[wispWlanIndex].priv->dev;
					#endif
				  } else {
					goto free_and_stop;
				  }
				#else		
					#ifdef NETDEV_NO_PRIV
					dev=skb->dev=((struct rtl8192cd_priv *)netdev_priv(wlan_device[passThruWanIdx].priv->pWlanDev))->wlan_priv->dev;				
					#else	
					dev=skb->dev=((struct rtl8192cd_priv *)(wlan_device[passThruWanIdx].priv->pWlanDev->priv))->dev;
					#endif
				#endif
			}

		SMP_UNLOCK_XMIT(flags);
			
		rtl8192cd_start_xmit(skb, dev);
		
		SMP_LOCK_XMIT(flags);
		return 0;	

		} else {
			goto free_and_stop;
		}
	}
#endif	/* defined(CONFIG_RTL_CUSTOM_PASSTHRU) */

#if 0//def CONFIG_RTL_STP
	//port5: virtual device for wlan0 stp BPDU process
	if(memcmp((void *)(dev->name), "port5", 5)==0)
	{
		if ((skb->data[0]&0x01) && !memcmp(&(skb->data[0]), STPmac, 5) && !(skb->data[5] & 0xF0))
		{
			//To virtual device port5, tx bpdu.
		}
		else
		{
			//To virtual device port5, drop tx pkt becasue not bpdu!
			goto free_and_stop;
		}
	}
#endif

#ifdef CONFIG_RTL_EAP_RELAY
//mark_test , remove EAP padding by ethernet
	if (*((UINT16 *)((UINT8 *)skb->data + ETH_ALEN*2)) == __constant_htons(0x888e))
	{			
		real_len = ETH_ALEN*2+2+4+ntohs(*((UINT16 *)((UINT8 *)skb->data + ETH_ALEN*2+2+2)));
		skb_trim(skb,real_len);
		//printk("2 wlan tx EAP ,skb->len=%d,skb->data=%d \n",skb->len,skb->data_len);		
	}
#endif

//#ifdef SW_TX_QUEUE
        skb->dev = dev;
//#endif

#ifdef WDS
	if (!dev->base_addr && skb_cloned(skb)
		&& (priv->pmib->dot11WdsInfo.wdsPrivacy == _TKIP_PRIVACY_)
		) {
		struct sk_buff *mcast_skb = NULL;
		mcast_skb = skb_copy(skb, GFP_ATOMIC);
		if (mcast_skb == NULL) {
			priv->ext_stats.tx_drops++;
			DEBUG_ERR("TX DROP: Can't copy the skb!\n");
			goto free_and_stop;
		}
		dev_kfree_skb_any(skb);
		skb = mcast_skb;
	}
#endif

#ifdef	IGMP_FILTER_CMO
	if (priv->pshare->rf_ft_var.igmp_deny)
	{
		if ((OPMODE & WIFI_AP_STATE) &&	IS_MCAST(skb->data))
		{
			if (IP_MCAST_MAC(skb->data)
				#ifdef	TX_SUPPORT_IPV6_MCAST2UNI
				|| ICMPV6_MCAST_MAC(skb->data)
				#endif
				)
			{
				if(!IS_IGMP_PROTO(skb->data)){
					priv->ext_stats.tx_drops++;										
					DEBUG_ERR("TX DROP: Multicast packet filtered\n");
					goto free_and_stop;
				}
			}
		}
	}
#endif

#ifdef HS2_SUPPORT
	//priv->proxy_arp = 1;
	#if 0 // for debug
	if((ICMPV6_MCAST_SOLI_MAC(skb->data)) 
        && (skb->data[12] == 0x86 && skb->data[13] == 0xdd)
        && (skb->data[14+40]==135))
    {
        if(priv->proxy_arp == 0){
            HS2DEBUG("[%s] proxy_arp=0\n",priv->dev->name);
        }
    }   
	#endif
	if (priv->proxy_arp)	{

		unsigned short protocol;

		if(isDHCPpkt(skb))	{
			staip_snooping_bydhcp(skb, priv);
		}

		if (IS_MCAST(skb->data))	{
			protocol = *((unsigned short *)(skb->data + 2 * ETH_ALEN));
	
			//proxy arp	
			if (protocol == __constant_htons(ETH_P_ARP))	{
				if (proxy_arp_handle(priv, skb))	{
					//reply ok and return
			        	goto stop_proc;
				}
				//drop packets
		        	goto free_and_stop;
			}
			//icmpv6 
			if (ICMPV6_MCAST_SOLI_MAC(skb->data))	{
			//if (ICMPV6_MCAST_MAC(skb->data))
				if (proxy_icmpv6_ndisc(priv,skb))	{
					goto stop_proc;
				}
				//drop packets
				goto free_and_stop;
			} 
			//drop unsolicited neighbor advertisement when proxy arp=1
			if (ICMPV6_MCAST_MAC(skb->data))	{
				if (check_nei_advt(priv,skb))	{
					 //drop packets
			             goto free_and_stop;
				}
			}
		}
	} 
	
	if(priv->dgaf_disable)
	{
		//dhcp m2u check
		if (IS_MCAST(skb->data))
		{
			if (isDHCPpkt(skb))
			{
				struct list_head *phead, *plist;
                HS2_DEBUG_INFO("DHCP multicast to unicast\n");
				phead = &priv->asoc_list;
				plist = phead->next;
				while (phead && (plist != phead)) 
				{
					pstat = list_entry(plist, struct stat_info, asoc_list);
					plist = plist->next;

					/* avoid   come from STA1 and send back STA1 */
			        if (!memcmp(pstat->hwaddr, &skb->data[6], 6))
					{
						continue;
					}
#if defined(CONFIG_RTL865X_ETH_PRIV_SKB) || defined(CONFIG_RTL_ETH_PRIV_SKB)
					newskb = priv_skb_copy(skb);
#else
					newskb = skb_copy(skb, GFP_ATOMIC);
                    #endif
	                if (newskb) 
					{
						memcpy(newskb->data, pstat->hwaddr, 6);
				        newskb->cb[2] = (char)0xff;         // not do aggregation
				        memcpy(newskb->cb+10,newskb->data,6);
                        #if 0   // for debug
						printk("send m2u, da=");						
						MAC_PRINT(pstat->hwaddr);
						printk("\n");
                        #endif
						__rtl8192cd_start_xmit(newskb, priv->dev, TX_NO_MUL2UNI);
					}
				}
			}
	        goto free_and_stop;
		}
	}
#endif

#ifdef SUPPORT_TX_MCAST2UNI
#ifdef WDS			// when interface is WDS don't enter multi2uni path
	if (dev->base_addr && !priv->pmib->dot11WdsInfo.wdsPure)
#endif
	if (!priv->pshare->rf_ft_var.mc2u_disable) {
		if ((OPMODE & WIFI_AP_STATE) &&	IS_MCAST(skb->data))
		{
			if ((IP_MCAST_MAC(skb->data)
				#ifdef	TX_SUPPORT_IPV6_MCAST2UNI
				|| IPV6_MCAST_MAC(skb->data)
				#endif
				) && (tx_flag != TX_NO_MUL2UNI))
			{
				if (mlcst2unicst(priv, skb)){
					return 0;
				}
			}
		}

		skb->cb[2] = 0;	// allow aggregation
	}
#endif

#ifdef SUPPORT_TX_MCAST2UNI
	skb->cb[16] = tx_flag;
#endif

#if !defined(CONFIG_RTL_8676HWNAT) && defined(CONFIG_RTL8672) && !defined(CONFIG_RTL8686) && !defined(CONFIG_RTL8685)

	//IGMP snooping
	if (check_wlan_mcast_tx(skb)==1) {
		goto free_and_stop;
	}
#endif

#ifdef DFS
	if (!priv->pmib->dot11DFSEntry.disable_DFS &&
		GET_ROOT(priv)->pmib->dot11DFSEntry.disable_tx 
		&& (priv->site_survey)
		&& (priv->site_survey->hidden_ap_found != HIDE_AP_FOUND_DO_ACTIVE_SSAN)) {
		priv->ext_stats.tx_drops++;
		DEBUG_ERR("TX DROP: DFS probation period\n");
		goto free_and_stop;
	}
#endif

	if (!IS_DRV_OPEN(priv))
		goto free_and_stop;

#ifdef MP_TEST
	if (OPMODE & WIFI_MP_STATE) {
		goto free_and_stop;
	}
#endif

#ifdef MULTI_MAC_CLONE
	if (priv->pmib->ethBrExtInfo.macclone_enable) {
		int id;
		if (!(skb->data[ETH_ALEN] & 1) && MCLONE_NUM > 0) {
			id = mclone_find_address(priv, skb->data+ETH_ALEN, skb, MAC_CLONE_SA_FIND);
			if (id > 0)
				ACTIVE_ID = id;
			else				
				ACTIVE_ID = 0;
		}else
			ACTIVE_ID = 0;
	}	
#endif

#ifdef CONFIG_RTL867X_VLAN_MAPPING
	if (re_vlan_loaded()) {
		struct ethhdr *eth = (struct ethhdr *)skb->data;
		unsigned short vid = 0;

		if (eth->h_proto != ETH_P_PAE) {
			if (re_vlan_skb_xmit(skb, &vid)) {
				priv->ext_stats.tx_drops++;
				return 0;
			}
			if (vid && re_vlan_addtag(skb, vid)) {
				priv->ext_stats.tx_drops++;
				return 0;
			}
		}
	}
#endif

#ifdef CONFIG_RTK_VLAN_SUPPORT
		vlan = &priv->pmib->vlan;

	if (vlan->global_vlan &&
			*((UINT16 *)((UINT8 *)skb->data + ETH_ALEN*2)) != __constant_htons(0x888e)) {
		int get_pri = 0;
#ifdef WIFI_WMM
		if (QOS_ENABLE) {
#ifdef CLIENT_MODE
			if ((OPMODE & (WIFI_STATION_STATE | WIFI_ASOC_STATE)) == (WIFI_STATION_STATE | WIFI_ASOC_STATE))
				pstat = get_stainfo(priv, BSSID);
			else
#endif
			{
#ifdef MCAST2UI_REFINE
                                if (pstat == NULL && !IS_MCAST(&skb->cb[10]))
                                        pstat = get_stainfo(priv, &skb->cb[10]);
#else
				if (pstat == NULL && !IS_MCAST(skb->data))
					pstat = get_stainfo(priv, skb->data);
#endif
			}

			if (pstat && pstat->QosEnabled)
				get_pri = 1;
		}
#endif

		if (!get_pri)
			skb->cb[0] = '\0';

		if (tx_vlan_process(dev, &priv->pmib->vlan, skb, get_pri)) {
			priv->ext_stats.tx_drops++;
			DEBUG_ERR("TX DROP: by vlan!\n");
			goto free_and_stop;
		}
	}
	else
		skb->cb[0] = '\0';
#endif

#if defined(CONFIG_RTL_819X_ECOS)&&defined(CONFIG_RTL_VLAN_SUPPORT)&&defined(CONFIG_RTL_819X_SWCORE)
	if (rtl_vlan_support_enable && ntohs(*((UINT16 *)((UINT8 *)skb->data + ETH_ALEN*2))) != 0x888e) {
		int get_pri = 0;
		if (QOS_ENABLE) {
#ifdef CLIENT_MODE
			if ((OPMODE & (WIFI_STATION_STATE | WIFI_ASOC_STATE)) == (WIFI_STATION_STATE | WIFI_ASOC_STATE))
				pstat = get_stainfo(priv, BSSID);
			else
#endif
			{
				if (pstat == NULL && !IS_MCAST(skb->data))
					pstat = get_stainfo(priv, skb->data);
			}

			if (pstat && pstat->QosEnabled)
				get_pri = 1;
		}

		if (!get_pri)
			skb->cb[0] = '\0';

		if (rtl_vlanEgressProcess(skb, priv->dev->name, get_pri) < 0) {
			priv->ext_stats.tx_drops++;
			DEBUG_ERR("TX DROP: by vlan!\n");
			goto free_and_stop;
		}
	}
	else
		skb->cb[0] = '\0';
#endif

#ifdef CONFIG_RTL_VLAN_8021Q	
	/*remove vlan tag if it is not need to tagged out*/
	if(linux_vlan_enable){		
		if(*((UINT16*)(skb->data+(ETH_ALEN<<1))) == __constant_htons(ETH_P_8021Q)){
			vid = *((UINT16*)(skb->data+(ETH_ALEN<<1)+2));
			vid = vid&0xfff;			
			if(!(vlan_ctl_p->group[vid].tagMemberMask&(1<<dev->vlan_member_map))){
				if (skb_cloned(skb)){
					newskb = skb_copy(skb, GFP_ATOMIC);
					if (newskb == NULL) {
 					 	goto free_and_stop;
					}
					dev_kfree_skb_any(skb);
					skb = newskb;
				}
				memmove(skb->data+VLAN_HLEN, skb->data, ETH_ALEN<<1);
				skb_pull(skb,VLAN_HLEN);
			}
		}
	}
#endif

#ifdef WDS
	if (dev->base_addr) {
		// normal packets
		if (priv->pmib->dot11WdsInfo.wdsPure) {
			priv->ext_stats.tx_drops++;
			DEBUG_ERR("TX DROP: Sent normal pkt in Pure WDS mode!\n");
			goto free_and_stop;
		}
	}
	else {
		// WDS process
		if (rtl8192cd_tx_wdsDevProc(priv, skb, &dev, &wdsDev, txcfg) == TX_PROCEDURE_CTRL_STOP) {
			goto stop_proc;
		}
	}
#endif // WDS

	if (priv->pmib->miscEntry.func_off || priv->pmib->miscEntry.raku_only)
		goto free_and_stop;

	// drop packets if link status is null
#ifdef WDS
	if (!wdsDev)
#endif
	{
        temp = get_assoc_sta_num(priv, 1);        
		if (temp == 0) {
			priv->ext_stats.tx_drops++;
			DEBUG_WARN("TX DROP: Non asoc tx request!\n");
			goto free_and_stop;
		}
	}

#ifdef CLIENT_MODE
	// nat2.5 translation, mac clone, dhcp broadcast flag add.
	if ((OPMODE & WIFI_STATION_STATE) || (OPMODE & WIFI_ADHOC_STATE)) {

#ifdef A4_STA
        pstat = get_stainfo(priv, BSSID);
        if(pstat && !(pstat->state & WIFI_A4_STA))
#endif
        {            
#ifdef RTK_BR_EXT
			if (!priv->pmib->ethBrExtInfo.nat25_disable &&
					!(skb->data[0] & 1) &&
#ifdef MULTI_MAC_CLONE
					(ACTIVE_ID == 0) &&
#endif		
#ifdef __KERNEL__
					GET_BR_PORT(priv->dev) &&
#endif
					memcmp(skb->data+MACADDRLEN, priv->br_mac, MACADDRLEN) &&
//					*((unsigned short *)(skb->data+MACADDRLEN*2)) != __constant_htons(ETH_P_8021Q) &&
					*((unsigned short *)(skb->data+MACADDRLEN*2)) == __constant_htons(ETH_P_IP) &&
					!memcmp(priv->scdb_mac, skb->data+MACADDRLEN, MACADDRLEN) && priv->scdb_entry) {
                if(priv->nat25_filter) {
                    if(nat25_filter(priv, skb)== 1) {
                        priv->ext_stats.tx_drops++;
                        DEBUG_ERR("TX DROP: nat25 filter out!\n");
                        goto free_and_stop;
                    }
                }

				memcpy(skb->data+MACADDRLEN, GET_MY_HWADDR, MACADDRLEN);
				priv->scdb_entry->ageing_timer = jiffies;
			}
			else
#endif		
			{
				SMP_UNLOCK_XMIT(flags);
				if (rtl8192cd_tx_clientMode(priv, &skb) == TX_PROCEDURE_CTRL_STOP) {
					SMP_LOCK_XMIT(flags);
					goto stop_proc;
				}
				SMP_LOCK_XMIT(flags);
			}
    		
#ifdef RTK_BR_EXT //8812_client
#ifdef MULTI_MAC_CLONE
		if (ACTIVE_ID == 0)//not a mac clone sta
#endif	
			dhcp_flag_bcast(priv, skb);
#endif
        }

	}
#endif // CLIENT_MODE

#ifdef MBSSID
	if (GET_ROOT(priv)->pmib->miscEntry.vap_enable)
	{
		if ((OPMODE & WIFI_AP_STATE) && !wdsDev) {
			if ((*(unsigned int *)&(skb->cb[8]) == 0x86518190) &&						// come from wlan interface
				(*(unsigned int *)&(skb->cb[12]) != priv->pmib->miscEntry.groupID))		// check group ID
			{
				priv->ext_stats.tx_drops++;
				DEBUG_ERR("TX DROP: not the same group!\n");
				goto free_and_stop;
			}
		}
	}
#endif

#ifdef USE_TXQUEUE
	if (GET_ROOT(priv)->pmib->miscEntry.use_txq && priv->pshare->iot_mode_enable) {
		int pri = 0, qnum = BE_QUEUE, qlen = 0, qidx = 0;
		pri = get_skb_priority(priv, skb, NULL);
#ifdef RTL_MANUAL_EDCA
        q_num = PRI_TO_QNUM(priv, pri);
#else
		PRI_TO_QNUM(pri, qnum, priv->pmib->dot11OperationEntry.wifi_specific);
#endif
		
		if (!priv->pshare->txq_isr)
		{
			if (txq_len(&priv->pshare->txq_list[qnum]) > 0)
			{
#ifdef RESERVE_TXDESC_FOR_EACH_IF
				if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc) {
					if (!check_txq_dynamic_mechanism(priv, qnum)) {
						append_skb_to_txq_tail(&priv->pshare->txq_list[qnum], priv, skb, dev, &priv->pshare->txq_pool);
						priv->use_txq_cnt[qnum]++;
						priv->pshare->txq_check = 1;
						goto stop_proc;
					} else {
						DEBUG_ERR("TX DROP: exceed the tx queue!\n");
						priv->ext_stats.tx_drops++;
						goto free_and_stop;
					}
				}
				else
#endif
				{
					for (qidx=0; qidx<7; qidx++)
						qlen += txq_len(&priv->pshare->txq_list[qidx]);
					if (qlen < TXQUEUE_SIZE) {
						append_skb_to_txq_tail(&priv->pshare->txq_list[qnum], priv, skb, dev, &priv->pshare->txq_pool);
						priv->pshare->txq_check = 1;
						goto stop_proc;
					} else {
						DEBUG_ERR("TX DROP: exceed the tx queue!\n");
						priv->ext_stats.tx_drops++;
						goto free_and_stop;
					}
				}
			}
		}
	}
#endif

#ifdef WDS
	if (wdsDev)
		da = priv->pmib->dot11WdsInfo.entry[txcfg->wdsIdx].macAddr;
	else
#endif
	{

#ifdef MCAST2UI_REFINE
        da = &skb->cb[10];
#else
		da = skb->data;
#endif
	}

#ifdef CLIENT_MODE
	if ((OPMODE & (WIFI_STATION_STATE | WIFI_ASOC_STATE)) == (WIFI_STATION_STATE | WIFI_ASOC_STATE))
		pstat = get_stainfo(priv, BSSID);
	else
#endif
	{
		pstat = get_stainfo(priv, da);
#ifdef A4_STA
        if (tx_flag == TX_NORMAL && pstat == NULL && 
            priv->pshare->rf_ft_var.a4_enable && (OPMODE & WIFI_AP_STATE)) {
            if(IS_MCAST(da)) {
                if(a4_tx_mcast_to_unicast(priv, skb) == 1) {
                    goto free_and_stop;
                }
            }
            else {
                pstat = a4_sta_lookup(priv, da);	
                if(pstat == NULL && priv->pshare->rf_ft_var.a4_enable == 2) {
                    /* Unknown unicast, transmit to every enhanced A4 sta*/
                    a4_tx_unknown_unicast(priv, skb);                
                    goto free_and_stop;
                }
            }
        }
#endif		

#if defined(TV_MODE) && defined(SUPPORT_TX_MCAST2UNI) /*drop (joint multicast && (ipv4 || ipv6)) packet*/
        if(priv->pmib->miscEntry.forward_streaming == 0 && (priv->tv_mode_status & BIT0) == 0 && IS_MCAST(da)) {
            unsigned short ethtype = *((unsigned short *)(skb->data+MACADDRLEN*2));
            if(ethtype == __constant_htons(ETH_P_IP) || ethtype == __constant_htons(ETH_P_IPV6)) {
                if(tv_mode_igmp_group_check(priv, skb))
                    goto free_and_stop;
            }
        }
#endif


#ifdef SW_TX_QUEUE
		if (pstat)
			swq_out = priv->pshare->swq_en | pstat->swq.swq_empty[BK_QUEUE] | 
			                         pstat->swq.swq_empty[BE_QUEUE] | 
			                         pstat->swq.swq_empty[VO_QUEUE] | 
			                         pstat->swq.swq_empty[VI_QUEUE];
#endif
	}
#ifdef DETECT_STA_EXISTANCE
	if(pstat && pstat->leave)	{
		priv->ext_stats.tx_drops++;
		DEBUG_WARN("TX DROP: sta may leave! %02x%02x%02x%02x%02x%02x\n", pstat->hwaddr[0],pstat->hwaddr[1],pstat->hwaddr[2],pstat->hwaddr[3],pstat->hwaddr[4],pstat->hwaddr[5]);
		goto free_and_stop;
	}
#endif

#ifdef TX_SCATTER
#ifdef CONFIG_IEEE80211W
		if (skb->list_num > 0 && (UseSwCrypto(priv, pstat, (pstat ? FALSE : TRUE), 0) ||
#else
		if (skb->list_num > 0 && (UseSwCrypto(priv, pstat, (pstat ? FALSE : TRUE)) ||
#endif
		(pstat && (get_sta_encrypt_algthm(priv, pstat) == _TKIP_PRIVACY_)))) {
		newskb = copy_skb(skb);
		if (newskb == NULL) {
			priv->ext_stats.tx_drops++;
			DEBUG_ERR("TX DROP: Can't copy the skb for list buffer!\n");
			goto free_and_stop;
		}
		dev_kfree_skb_any(skb);
		skb = newskb;
	}
#endif

#ifdef CONFIG_PCI_HCI
#ifdef SW_TX_QUEUE
	if (swq_out == 0
#ifdef GBWC
		|| priv->GBWC_consuming_Q
#endif		
		) {
#ifdef CONFIG_RTK_MESH
     		return __rtl8192cd_start_xmit_out(skb, pstat, NULL);
#else
			return __rtl8192cd_start_xmit_out(skb, pstat);
#endif
    } else if (priv->pshare->swq_en) {
        return rtl8192cd_swq_enqueue(priv, skb, pstat);
    } else {
        for(temp = BK_QUEUE; temp <= VO_QUEUE; temp++)
        {
            if(pstat->swq.swq_empty[temp]) {                
                if (pstat->swq.swq_timer_id[temp])
                    rtl8192cd_swq_deltimer(priv, pstat, temp);                                
                rtl8192cd_swq_dequeue(priv, pstat, temp);
            }
        }

#if defined(CONFIG_RTK_MESH)
		__rtl8192cd_start_xmit_out(skb, pstat, NULL);
#else
	    __rtl8192cd_start_xmit_out(skb, pstat);
#endif
	}
	goto stop_proc;
#else
#ifdef CONFIG_RTK_MESH
    return __rtl8192cd_start_xmit_out(skb, pstat, NULL);
#else
	return __rtl8192cd_start_xmit_out(skb, pstat);
#endif	
#endif	

free_and_stop:          /* Free current packet and stop TX process */
	
	rtl_kfree_skb(priv, skb, _SKB_TX_);

stop_proc:                      /* Stop process and assume the TX-ed packet is already "processed" (freed or TXed) in previous code. */

    return 0;
}

__MIPS16
__IRAM_IN_865X
#ifdef CONFIG_RTK_MESH
int __rtl8192cd_start_xmit_out(struct sk_buff *skb, struct stat_info *pstat, struct tx_insn *ptxcfg) //struct net_device *dev)
#else
int __rtl8192cd_start_xmit_out(struct sk_buff *skb, struct stat_info *pstat) //struct net_device *dev)
#endif
{
    struct rtl8192cd_priv *priv;
    //struct stat_info        *pstat=NULL;
    struct net_device *dev = skb->dev;
    struct sk_buff *newskb = NULL;
    struct net_device *wdsDev = NULL;
#if defined(CONFIG_RTL8672) || defined(TX_SHORTCUT)
    int k;
#endif
#ifdef TXSC_HDR
	int txsc_lv2 = 0;
#endif	
#if defined(TX_SHORTCUT)
	struct tx_sc_entry *ptxsc_entry = NULL;
#endif
    struct tx_insn tx_insn;
    DECLARE_TXCFG(txcfg, tx_insn);

#ifdef NETDEV_NO_PRIV
	priv = ((struct rtl8192cd_priv *)netdev_priv(dev))->wlan_priv;
#else
	priv = (struct rtl8192cd_priv *)dev->priv;
#endif
        
#if defined(CONFIG_RTK_MESH)
	if(dev == priv->mesh_dev) {
		if(ptxcfg->is_11s == 1) {
			memcpy(txcfg, ptxcfg, sizeof(*ptxcfg));
            priv = txcfg->priv;
        }    
		else { /* if ptxcfg->is_11s is RELAY_11S */
            txcfg->is_11s = RELAY_11S;                            
            memcpy(txcfg->nhop_11s, pstat->hwaddr, MACADDRLEN); 
            memcpy(&txcfg->mesh_header, skb->data + WLAN_ETHHDR_LEN, sizeof(struct lls_mesh_header));       
            priv = ptxcfg->priv;
       }    
    }    
#endif

#ifdef CONFIG_RTL8672
#ifdef SUPPORT_TX_MCAST2UNI
	if (skb->cb[16] == TX_NO_MUL2UNI)
		txcfg->isMC2UC = 1;
	else
		txcfg->isMC2UC = 0;
#endif
#endif

#ifdef WDS
	if (dev->base_addr) {
		// normal packets
		if (priv->pmib->dot11WdsInfo.wdsPure) {
			priv->ext_stats.tx_drops++;
			DEBUG_ERR("TX DROP: Sent normal pkt in Pure WDS mode!\n");
			goto free_and_stop;
		}
	}
	else {
		// WDS process
		if (rtl8192cd_tx_wdsDevProc(priv, skb, &dev, &wdsDev, txcfg) == TX_PROCEDURE_CTRL_STOP) {
			goto stop_proc;
		}
	}
#endif // WDS
#endif // CONFIG_PCI_HCI

/*
	if (UseSwCrypto(priv, pstat, (pstat ? FALSE : TRUE)) ||					// sw enc will modify content
		(pstat && (get_sta_encrypt_algthm(priv, pstat) == _TKIP_PRIVACY_)))	// need append MIC
*/
	{
		if (skb_cloned(skb)
#ifdef MCAST2UI_REFINE
                        && !memcmp(skb->data, &skb->cb[10], 6)
#endif
			)
		{
			newskb = skb_copy(skb, GFP_ATOMIC);
			if (newskb == NULL) {
				priv->ext_stats.tx_drops++;
				DEBUG_ERR("TX DROP: Can't copy the skb!\n");
				goto free_and_stop;
			}
			dev_kfree_skb_any(skb);
			skb = newskb;
		}
	}

#ifdef SUPPORT_SNMP_MIB
	if (IS_MCAST(skb->data))
		SNMP_MIB_INC(dot11MulticastTransmittedFrameCount, 1);
#endif

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	txcfg->fr_type = _SKB_FRAME_TYPE_;
	txcfg->pframe = skb;
	txcfg->pstat = pstat;
	
	if (update_txinsn_stage1(priv, txcfg) == FALSE) {
		priv->ext_stats.tx_drops++;
		goto free_and_stop;
	}
	
	txcfg->next_txpath = TXPATH_HARD_START_XMIT;
#endif // CONFIG_USB_HCI || CONFIG_SDIO_HCI

	if ((OPMODE & WIFI_AP_STATE)
#ifdef WDS
			&& (!wdsDev)
#endif
#ifdef CONFIG_RTK_MESH
			&& (dev != priv->mesh_dev)
#endif
		) {
#ifdef MCAST2UI_REFINE
                if ((pstat && ((pstat->state & (WIFI_SLEEP_STATE | WIFI_ASOC_STATE)) ==
                                (WIFI_SLEEP_STATE | WIFI_ASOC_STATE))) ||
                        (((IS_MCAST(&skb->cb[10]) && (priv->sleep_list.next != &priv->sleep_list)) ||
                        ((priv->pshare->rf_ft_var.bcast_to_dzq) && ((unsigned char )(skb->cb[10]) == 0xff))) &&
                                (!priv->release_mcast)))
#else
		if ((pstat && ((pstat->state & (WIFI_SLEEP_STATE | WIFI_ASOC_STATE)) ==
				(WIFI_SLEEP_STATE | WIFI_ASOC_STATE))) ||
			(((IS_MCAST(skb->data) && (priv->sleep_list.next != &priv->sleep_list)) ||
			(priv->pshare->rf_ft_var.bcast_to_dzq && (*(skb->data) == 0xff))) &&
				(!priv->release_mcast)))
#endif
		{

#ifdef _11s_TEST_MODE_
			mesh_debug_tx3(dev, priv, skb);
#endif

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			if (dz_queue(priv, pstat, txcfg) == TRUE)
#elif defined(CONFIG_PCI_HCI)
			if (dz_queue(priv, pstat, skb) == TRUE)
#endif
			{
				DEBUG_INFO("queue up skb due to sleep mode\n");
				goto stop_proc;
			}
			else {
				if (pstat) {
					DEBUG_WARN("ucst sleep queue full!!\n");
				}
				else {
					DEBUG_WARN("mcst sleep queue full!!\n");
				}
				goto free_and_stop;
			}
		}
	}

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	if (rtw_is_tx_queue_empty(priv, txcfg) == FALSE) {
		if (rtw_xmit_enqueue(priv, txcfg) == FALSE) {
			goto free_and_stop;
		}
		goto stop_proc;
	}
	
	if (__rtl8192cd_usb_start_xmit(priv, txcfg) == 0)
		goto stop_proc;

free_and_stop:		/* Free current packet and stop TX process */

	rtl_kfree_skb(priv, skb, _SKB_TX_);

stop_proc:			/* Stop process and assume the TX-ed packet is already "processed" (freed or TXed) in previous code. */

	return 0;
}

int __rtl8192cd_usb_start_xmit(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
	struct net_device *dev;
	struct net_device *wdsDev = NULL;
	struct stat_info *pstat;
	struct sk_buff *skb;
#ifdef TX_SHORTCUT
	int k;
	struct tx_sc_entry *ptxsc_entry = NULL;
	unsigned char pktpri;
#endif
//	int priority;
#if defined(CONFIG_SDIO_HCI) && defined(TX_SCATTER)
	struct wlan_ethhdr_t	*pethhdr=NULL;
#endif
	
	skb = (struct sk_buff *)txcfg->pframe;

	if (!IS_DRV_OPEN(priv))
		goto free_and_stop;

	if (priv->pmib->miscEntry.func_off || priv->pmib->miscEntry.raku_only)
		goto free_and_stop;
	
	switch( rtw_xmit_decision(priv, txcfg) ) {
	case XMIT_DECISION_ENQUEUE:
		goto stop_proc;
	case XMIT_DECISION_STOP:
		priv->ext_stats.tx_drops++;
		goto free_and_stop;
	case XMIT_DECISION_CONTINUE:
		break;
	}
	
#ifdef WDS
	if (txcfg->wdsIdx >= 0) {
		wdsDev = priv->wds_dev[txcfg->wdsIdx];
	}
#endif
	
	dev = priv->dev;
	pstat = txcfg->pstat;
	
#endif // CONFIG_USB_HCI || CONFIG_SDIO_HCI

#ifdef GBWC
	if (priv->pmib->gbwcEntry.GBWCMode && pstat) {
		if (rtl8192cd_tx_gbwc(priv, pstat, skb) == TX_PROCEDURE_CTRL_STOP) {
			goto stop_proc;
		}
	}
#endif

#ifdef SUPPORT_TX_AMSDU
	if (pstat && (pstat->aggre_mthd & AGGRE_MTHD_MSDU) && (pstat->amsdu_level > 0)
#ifdef SUPPORT_TX_MCAST2UNI
		&& (priv->pshare->rf_ft_var.mc2u_disable || (skb->cb[2] != (char)0xff))
#endif
		) {
		int ret = amsdu_check(priv, skb, pstat, txcfg);

		if (ret == RET_AGGRE_ENQUE)
			goto stop_proc;

		if (ret == RET_AGGRE_DESC_FULL)
			goto free_and_stop;
	}
#endif
#ifdef MESH_AMSDU
	if(dev == priv->mesh_dev)
		goto just_skip;
#endif

#ifdef SW_TX_QUEUE

#ifdef CONFIG_RTK_MESH
    if(dev != priv->mesh_dev) /*mesh do not support sw tx queue yet, skip this*/
#endif        
    {

    	if ((priv->assoc_num > 1) && pstat)
        {
#ifdef MCAST2UI_REFINE
            if (memcmp(&skb->cb[10], priv->pshare->record_mac, 6))
#else
            if (memcmp(skb->data, priv->pshare->record_mac, 6))
#endif
        	{
            	priv->pshare->swq_txmac_chg++;
#ifdef MCAST2UI_REFINE
                memcpy(priv->pshare->record_mac, &skb->cb[10], 6);
#else
                memcpy(priv->pshare->record_mac, skb->data, 6);
#endif
           	}
            else
            {
            	int pri, q_num;

#ifdef CONFIG_RTK_MESH
                pri = get_skb_priority3(priv, skb, txcfg->is_11s, pstat);
#else
                pri = get_skb_priority(priv, skb, pstat);
#endif

#ifdef RTL_MANUAL_EDCA
                q_num = PRI_TO_QNUM(priv, pri);
#else
                PRI_TO_QNUM(pri, q_num, priv->pmib->dot11OperationEntry.wifi_specific);
#endif
                if (priv->pshare->record_qnum != q_num)
                {
                	priv->pshare->swq_txmac_chg++;
                    priv->pshare->record_qnum = q_num;
                }
            }
        }

#ifdef SW_TX_QUEUE_SMALL_PACKET_CHECK
        // add check for small udp packet(88B) test with veriwave tool
        if ((priv->pshare->swq_en == 0) && (priv->assoc_num > 1) && (AMPDU_ENABLE))
        {
            int thd_value;

            if ((priv->swq_boost_delay > 0) && (priv->swq_boost_delay < 10))
                thd_value = priv->pshare->rf_ft_var.swq_en_highthd / 10;
            else
                thd_value = priv->pshare->rf_ft_var.swq_en_highthd;

            if (priv->pshare->swq_txmac_chg >= thd_value);
            {
                //printk("%s %d : thd_value %d\n", __FUNCTION__, __LINE__, thd_value);

                if (priv->pshare->txop_enlarge == 0)
                    priv->pshare->txop_enlarge = 2;

                priv->pshare->swq_en = 1;
                priv->pshare->swqen_keeptime = priv->up_time;

                struct stat_info	*pstat_swq;
                struct list_head	*phead, *plist;
                struct stat_info	*pstat_highest = NULL;

                phead = &priv->asoc_list;
                plist = phead->next;

                while (plist != phead)
                {
                    pstat_swq = list_entry(plist, struct stat_info, asoc_list);

                    int i;
                    for (i = BK_QUEUE; i <= VO_QUEUE; i++) 
                    {				
                        pstat_swq->swq.q_aggnum[i] = priv->pshare->rf_ft_var.swq_aggnum; // MAX_BACKOFF_CNT;
                    }

                    if (plist == plist->next)
                        break;

                    plist = plist->next;
                }
            }
        }
#endif

    }
#endif //SW_TX_QUEUE

#ifdef TX_SHORTCUT
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
    if(pstat)
    	pktpri = get_skb_priority(priv, skb, pstat);
	
	if (!priv->pmib->dot11OperationEntry.disable_txsc && pstat
			&& ((k = get_tx_sc_index(priv, pstat, skb->data, pktpri)) >= 0))
		ptxsc_entry = &pstat->tx_sc_ent[pktpri][k];
#else
	if (!priv->pmib->dot11OperationEntry.disable_txsc && pstat
			&& ((k = get_tx_sc_index(priv, pstat, skb->data)) >= 0))
		ptxsc_entry = &pstat->tx_sc_ent[k];
#endif
	
	if ((NULL != ptxsc_entry) &&
		((ptxsc_entry->txcfg.privacy == 0) ||
#ifdef CONFIG_RTL_WAPI_SUPPORT
		  (ptxsc_entry->txcfg.privacy == _WAPI_SMS4_) ||
#endif
#ifdef CONFIG_IEEE80211W
		!UseSwCrypto(priv, pstat->tx_sc_ent[k].txcfg.pstat, (pstat->tx_sc_ent[k].txcfg.pstat ? FALSE : TRUE), 0) ) &&
#else
		!UseSwCrypto(priv, pstat->tx_sc_ent[k].txcfg.pstat, (pstat->tx_sc_ent[k].txcfg.pstat ? FALSE : TRUE)) ) &&
#endif			
		(ptxsc_entry->txcfg.fr_len > 0) &&
#ifdef CONFIG_PCI_HCI
#ifdef CONFIG_RTK_MESH
		(get_skb_priority3(priv, skb, txcfg->is_11s, pstat) == ptxsc_entry->pktpri) &&
#else		
		(get_skb_priority(priv, skb, pstat) == ptxsc_entry->pktpri) &&
#endif
#endif
		(FRAGTHRSLD > 1500))
	{
#ifdef CONFIG_PCI_HCI
			int						*tx_head=NULL, *tx_tail=NULL, q_num;
			struct rtl8192cd_hw		*phw = GET_HW(priv);
#ifdef TX_SCATTER
			int reuse_txdesc = 0;
#endif
#ifdef CONFIG_WLAN_HAL
			PHCI_TX_DMA_MANAGER_88XX    ptx_dma;
#endif

			q_num = ptxsc_entry->txcfg.q_num;
#ifdef CONFIG_WLAN_HAL
			if (IS_HAL_CHIP(priv)) {
            	ptx_dma = (PHCI_TX_DMA_MANAGER_88XX)(_GET_HAL_DATA(priv)->PTxDMA88XX);
			} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif // CONFIG_WLAN_HAL
			{//not HAL
			    tx_head = get_txhead_addr(phw, q_num);
	 			tx_tail = get_txtail_addr(phw, q_num);
			}
			// check if we need active tx tasklet
//#ifdef __KERNEL__

            if(
#ifdef CONFIG_WLAN_HAL
            IS_HAL_CHIP(priv) ? (compareAvailableTXBD(priv, (CURRENT_NUM_TX_DESC/2), q_num, 2)) :
#endif // CONFIG_WLAN_HAL
            (CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC) < CURRENT_NUM_TX_DESC/2)

            )
            {
				if (!priv->pshare->has_triggered_tx_tasklet) {
#ifdef __KERNEL__
					tasklet_schedule(&priv->pshare->tx_tasklet);
#endif
					priv->pshare->has_triggered_tx_tasklet = 1;
				}
			}
//#endif
			// Check if we need to reclaim TX-ring before processing TX

            if(
#ifdef CONFIG_WLAN_HAL
            IS_HAL_CHIP(priv) ? (compareAvailableTXBD(priv, 32, q_num, 2)) : 
#endif // CONFIG_WLAN_HAL
            (CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC) < 10)

            )
            {
				rtl8192cd_tx_queueDsr(priv, q_num);
			}

#ifdef RESERVE_TXDESC_FOR_EACH_IF
			if ( GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc ) {
				if( check_txdesc_dynamic_mechanism(priv, q_num, 2) ) {
#ifdef USE_TXQUEUE
					if (GET_ROOT(priv)->pmib->miscEntry.use_txq && priv->pshare->iot_mode_enable) {
						if (priv->pshare->txq_isr) {
							append_skb_to_txq_head(&priv->pshare->txq_list[q_num], priv, skb, dev, &priv->pshare->txq_pool);
							priv->pshare->txq_stop = 1;
						} else {
							append_skb_to_txq_tail(&priv->pshare->txq_list[q_num], priv, skb, dev, &priv->pshare->txq_pool);
						}
						priv->use_txq_cnt[q_num]++;
						priv->pshare->txq_check = 1;
					}
					else
#endif
					{
						DEBUG_WARN("%d hw Queue desc exceed available count: used:%d\n", q_num, priv->use_txdesc_cnt[q_num]);
						rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
					}
					goto stop_proc;
				}
			} else
#endif
			{
#ifdef TX_EARLY_MODE
            if(
#ifdef CONFIG_WLAN_HAL
            IS_HAL_CHIP(priv) ? 
                ( compareAvailableTXBD(priv, ((GET_TX_EARLY_MODE && GET_EM_SWQ_ENABLE) ? 14 : 4), q_num, 2)) :
#endif // CONFIG_WLAN_HAL
                (((GET_TX_EARLY_MODE && GET_EM_SWQ_ENABLE) ? 14 : 4) > CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC))

            )
#else
			if(
#ifdef CONFIG_WLAN_HAL
            IS_HAL_CHIP(priv) ? (compareAvailableTXBD(priv, 4, q_num, 2)) : 
#endif // CONFIG_WLAN_HAL			
#ifdef TX_SCATTER
			((ptxsc_entry->has_desc3 && 6 > CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC)) ||
					(!ptxsc_entry->has_desc3 && 4 > CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC)))
#else
			((4) > CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC)) //per mpdu, we need 2 desc...
#endif
			)
#endif
			{
				// 2 is for spare...
#ifdef USE_TXQUEUE
				if (GET_ROOT(priv)->pmib->miscEntry.use_txq && priv->pshare->iot_mode_enable) {
					if (priv->pshare->txq_isr) {
						append_skb_to_txq_head(&priv->pshare->txq_list[q_num], priv, skb, dev, &priv->pshare->txq_pool);
						priv->pshare->txq_stop = 1;
					} else {
						append_skb_to_txq_tail(&priv->pshare->txq_list[q_num], priv, skb, dev, &priv->pshare->txq_pool);
					}
					priv->pshare->txq_check = 1;
				}
				else
#endif
				{
#ifdef CONFIG_WLAN_HAL
					if(IS_HAL_CHIP(priv)) {
						DEBUG_ERR("%s:%d: tx drop: %d hw Queue desc not available! head=%d, tail=%d request %d\n", __FUNCTION__, __LINE__,
							q_num, ptx_dma->tx_queue[q_num].host_idx,  ptx_dma->tx_queue[q_num].hw_idx,2);
					}
					else
#endif
						DEBUG_ERR("%d hw Queue desc not available! head=%d, tail=%d request %d\n",q_num,*tx_head,*tx_tail,2);


					rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
				}
				goto stop_proc;
			}
			}
#endif // CONFIG_PCI_HCI

#if defined(MESH_TX_SHORTCUT)
			if(txcfg->is_11s) {
                if(!mesh_txsc_decision(txcfg, &ptxsc_entry->txcfg)) { /*compare addr 5 and 6*/
                    goto just_skip;
                }

                if(memcmp(skb->data, GetAddr3Ptr(&ptxsc_entry->wlanhdr), MACADDRLEN)) {/*compare addr 3*/
                    goto just_skip;
                }

                ptxsc_entry->txcfg.mesh_header.segNum = txcfg->mesh_header.segNum;
                ptxsc_entry->txcfg.mesh_header.TTL = txcfg->mesh_header.TTL;
                ptxsc_entry->txcfg.is_11s = txcfg->is_11s;
			}
#endif

#ifdef A4_STA
            if(pstat->state & WIFI_A4_STA) {
                if(memcmp(skb->data, ptxsc_entry->ethhdr.daddr, MACADDRLEN)) {/*compare addr 3*/
                    goto just_skip;
                }
            }
#endif

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			memcpy(txcfg, &ptxsc_entry->txcfg, FIELD_OFFSET(struct tx_insn, pxmitframe));
#elif defined(CONFIG_PCI_HCI)
#if !defined(TXSC_CFG)
		memcpy(txcfg, &ptxsc_entry->txcfg, sizeof(struct tx_insn));
#else
		txcfg = &(ptxsc_entry->txcfg);
		txcfg->one_txdesc = 0;
#endif
#endif

#ifdef CONFIG_RTL8672
#ifdef SUPPORT_TX_MCAST2UNI
			if (skb->cb[16] == TX_NO_MUL2UNI)
				txcfg->isMC2UC = 1;
			else
				txcfg->isMC2UC = 0;
#endif
#endif

#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
			if (GET_CHIP_VER(priv)==VERSION_8813A) {
				txcfg->phdr   	= skb->data; 	// 802.3 hdr
				//txcfg->hdr_len	= WLAN_ETHHDR_LEN;
				txcfg->pframe 	= skb;  		// skb->data:  802.3 hdr + payload
				txcfg->fr_len 	= skb->len; 	// len(802.3 hdr + payload)
				// TODO: currently , we don't care encryption.... it's only support AES in HW
			} else
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV

			{
#ifndef TXSC_HDR
			txcfg->phdr = (UINT8 *)get_wlanllchdr_from_poll(priv);
			if (txcfg->phdr == NULL) {
				DEBUG_ERR("Can't alloc wlan header!\n");
				rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
				goto stop_proc;
			}
			memcpy(txcfg->phdr, (const void *)&ptxsc_entry->wlanhdr, sizeof(struct wlanllc_hdr));
#endif
			txcfg->pframe = skb;
#ifdef WDS
			/*Correct the aggre_mthd*/
			if(pstat && (pstat->state & WIFI_WDS))
				if(txcfg->aggre_en != pstat->aggre_mthd) {
					/*Invalid the tx_sc entry*/
					ptxsc_entry->txcfg.fr_len=0;
					txcfg->aggre_en = pstat->aggre_mthd;
				}	
#endif

#if defined(CONFIG_PCI_HCI) && defined(TX_SCATTER)
			if (skb->list_num > 0)
				txcfg->fr_len = skb->total_len - WLAN_ETHHDR_LEN;
			else
#endif
				txcfg->fr_len = skb->len - WLAN_ETHHDR_LEN;
#if defined(CONFIG_SDIO_HCI) && defined(TX_SCATTER)
			// in case 1st buffer len is 14, we get ether header pointer first and then ajust the skb
			pethhdr = (struct wlan_ethhdr_t *)(skb->data);
#endif
			skb_pull(skb, WLAN_ETHHDR_LEN);

#ifdef TXSC_HDR
#ifdef CONFIG_PCI_HCI
#ifdef TX_SCATTER
			if (skb->list_num == 0) {
				reuse_txdesc = 1;
			} else {
				if (ptxsc_entry->has_desc3) {
					if (skb->list_num == 3 && (skb->list_buf[0].len == WLAN_ETHHDR_LEN) &&
						(skb->list_buf[2].len  == (get_desc(ptxsc_entry->hwdesc3.Dword7)&TX_TxBufSizeMask))){
						reuse_txdesc = 1;
					}
				} else {
					if ((skb->list_num == 1 && skb->list_buf[0].len > WLAN_ETHHDR_LEN) ||
						(skb->list_num == 2 && skb->list_buf[0].len == WLAN_ETHHDR_LEN))
						reuse_txdesc = 1;
				}
				if (skb->len == 0 && skb->list_num > 1) {
					skb->list_idx++;
					skb_assign_buf(skb, skb->list_buf[skb->list_idx].buf, skb->list_buf[skb->list_idx].len);
					skb->len = skb->list_buf[skb->list_idx].len;
				}
			}
#endif
#endif
        if(
#ifdef WLAN_HAL_HW_TX_SHORTCUT_DISABLE_REUSE_TXDESC_FOR_DEBUG
                    // TODO: tempory False for disable reuse txdesc, only hdr conversion
                    FALSE &&
#endif
#if defined(CONFIG_PCI_HCI)
                   (
#ifdef CONFIG_WLAN_HAL
                    IS_HAL_CHIP(priv) ? 
#ifdef WLAN_HAL_HW_TX_SHORTCUT_REUSE_TXDESC
                    ((GET_CHIP_VER(priv)==VERSION_8813A) ? 
                        TRUE : 
                        ((GET_HAL_INTERFACE(priv)->GetShortCutTxBuffSizeHandler(priv, ptxsc_entry->hal_hw_desc)) > 0) ) 
#else
                    ((GET_HAL_INTERFACE(priv)->GetShortCutTxBuffSizeHandler(priv, ptxsc_entry->hal_hw_desc)) > 0)
#endif
                    :
#endif // CONFIG_WLAN_HAL
                    ((get_desc(ptxsc_entry->hwdesc1.Dword7)&TX_TxBufSizeMask) > 0) 
                    )
                && (
#ifdef CONFIG_RTL_WAPI_SUPPORT
                    // Note: for sw wapi, txcfg->mic=16; for hw wapi, txcfg->mic=0.
                     (txcfg->privacy==_WAPI_SMS4_) ? ((skb->len+txcfg->mic)==(get_desc(ptxsc_entry->hwdesc2.Dword7)&TX_TxBufSizeMask)) :
#endif
                    (
#ifdef CONFIG_WLAN_HAL
                    IS_HAL_CHIP(priv) ?
#ifdef WLAN_HAL_HW_TX_SHORTCUT_REUSE_TXDESC
                    ((GET_CHIP_VER(priv)==VERSION_8813A) ?
                        TRUE:
                        (skb->len == (GET_HAL_INTERFACE(priv)->GetShortCutTxBuffSizeHandler(priv, ptxsc_entry->hal_hw_desc))) )
#else
                    (skb->len ==  (GET_HAL_INTERFACE(priv)->GetShortCutTxBuffSizeHandler(priv, ptxsc_entry->hal_hw_desc))) 
#endif
                    :
#endif // CONFIG_WLAN_HAL
#ifdef TXSC_SKBLEN
                        ((GET_CHIP_VER(priv)== VERSION_8812E) ? 1 : (skb->len == (get_desc(ptxsc_entry->hwdesc2.Dword7)&TX_TxBufSizeMask)))
#else
                        (skb->len ==  (get_desc(ptxsc_entry->hwdesc2.Dword7)&TX_TxBufSizeMask))
#endif				
                    )
                ) &&
#elif defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
                    ((get_desc(ptxsc_entry->hwdesc1.Dword7)&TX_TxBufSizeMask) > 0) &&
                    (skb->len == (get_desc(ptxsc_entry->hwdesc1.Dword7)&TX_TxBufSizeMask)) &&
#endif
#ifdef TXSC_HDR
                        ((GET_CHIP_VER(priv)== VERSION_8812E) ? 1 : (txcfg->tx_rate == ptxsc_entry->txcfg.tx_rate)) &&
#else
                        (txcfg->tx_rate == ptxsc_entry->txcfg.tx_rate) &&
#endif				
                    (pstat->protection == priv->pmib->dot11ErpInfo.protection) &&
                    (pstat->ht_protection == priv->ht_protection)
#if defined(WIFI_WMM) && defined(WMM_APSD)
                    && (!(
#ifdef CLIENT_MODE
                    (OPMODE & WIFI_AP_STATE) &&
#endif
                    (APSD_ENABLE) && (pstat->state & WIFI_SLEEP_STATE)))
#endif
#ifdef CONFIG_PCI_HCI
#ifdef TX_SCATTER
                    && reuse_txdesc
#endif
#endif
                    )
					 txsc_lv2 =1;

#ifdef CONFIG_WLAN_HAL                
	        if (IS_HAL_CHIP(priv)) {
					txcfg->phdr = (UINT8 *)get_wlanllchdr_from_poll(priv);
					if (txcfg->phdr == NULL) {
					    DEBUG_ERR("Can't alloc wlan header!\n");
						rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
					    goto stop_proc;
					}
					memcpy(txcfg->phdr, (const void *)&ptxsc_entry->wlanhdr, sizeof(struct wlanllc_hdr));
	                  
	            // for TXBD mechanism, sending a packet always need one txdesc. So, txcfg->one_txdesc is useless.
	        } else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif // CONFIG_WLAN_HAL
	        {//not HAL
	            if (txsc_lv2 && (skb_headroom(skb) >= (txcfg->hdr_len + txcfg->llc + txcfg->iv
#ifdef TX_EARLY_MODE
	                + (GET_TX_EARLY_MODE ? 8 : 0)
#endif
	                )) &&
	                (txcfg->privacy != _TKIP_PRIVACY_) &&
#if defined(CONFIG_RTL_WAPI_SUPPORT)
	                (txcfg->privacy != _WAPI_SMS4_) &&
#endif
	                ((((unsigned int)skb->data) % 2) == 0)
	                )
	            {
	                txcfg->phdr = skb->data - (txcfg->hdr_len + txcfg->llc + txcfg->iv);
	                memcpy(txcfg->phdr, (const void *)&ptxsc_entry->wlanhdr, (txcfg->hdr_len + txcfg->llc + txcfg->iv));
	                txcfg->one_txdesc = 1;
	            } else {
					txcfg->phdr = (UINT8 *)get_wlanllchdr_from_poll(priv);
					if (txcfg->phdr == NULL) {
					    DEBUG_ERR("Can't alloc wlan header!\n");
						rtl8192cd_tx_xmitSkbFail(priv, skb, dev, wdsDev, txcfg);
					    goto stop_proc;
					}
					memcpy(txcfg->phdr, (const void *)&ptxsc_entry->wlanhdr, sizeof(struct wlanllc_hdr));
	            }
	        }
#endif
		}

#ifdef CONFIG_RTL_WAPI_SUPPORT
#ifdef CONFIG_RTL_HW_WAPI_SUPPORT
#ifdef CONFIG_IEEE80211W
			if((txcfg->privacy==_WAPI_SMS4_)&&(txcfg->llc>0)&&(UseSwCrypto(priv, pstat, (pstat ? FALSE : TRUE), 0)))
#else
			if((txcfg->privacy==_WAPI_SMS4_)&&(txcfg->llc>0)&&(UseSwCrypto(priv, pstat, (pstat ? FALSE : TRUE))))
#endif				
#else
			if((txcfg->privacy==_WAPI_SMS4_)&&(txcfg->llc>0))
#endif
			{				
				//To restore not-encrypted llc in wlan hdr
				//because llc in wlan hdr has been sms4encrypted to deliver at SecSWSMS4Encryption()
				eth_2_llc(&ptxsc_entry->ethhdr, (struct llc_snap *)(txcfg->phdr+txcfg->hdr_len + txcfg->iv));
			}
#endif

			if (txcfg->privacy == _TKIP_PRIVACY_) {
				if (rtl8192cd_tx_tkip(priv, skb, pstat, txcfg) == TX_PROCEDURE_CTRL_STOP) {
					goto stop_proc;
				}
			}

#ifdef MESH_TX_SHORTCUT
			if ( (txcfg->is_11s&1) && (GetFrameSubType(txcfg->phdr) == WIFI_11S_MESH))
                    if( !reuse_meshhdr(priv, txcfg) ) {
					        goto stop_proc;
					}
#endif
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			if (txcfg->is_pspoll && (tx_servq_len(&pstat->tx_queue[BE_QUEUE]) > 0)) {
				SetMData(txcfg->phdr);
			}
#endif

			txcfg->tx_rate = get_tx_rate(priv, pstat);
#ifndef TX_LOWESTRATE
			txcfg->lowest_tx_rate = get_lowest_tx_rate(priv, pstat, txcfg->tx_rate);
#endif			
			// log tx statistics...

#ifdef CONFIG_RTL8672
			tx_sum_up(priv, pstat, txcfg->fr_len+txcfg->hdr_len+txcfg->iv+txcfg->llc+txcfg->mic+txcfg->icv, txcfg);
#else
			tx_sum_up(priv, pstat, txcfg->fr_len+txcfg->hdr_len+txcfg->iv+txcfg->llc+txcfg->mic+txcfg->icv);
#endif
			SNMP_MIB_INC(dot11TransmittedFragmentCount, 1);
			pstat->tx_sc_pkts_lv1++;

#ifdef PCIE_POWER_SAVING
			PCIeWakeUp(priv, (POWER_DOWN_T0));
#endif

			// for SW LED
			priv->pshare->LED_tx_cnt++;
			if (
#ifdef RTK_AC_SUPPORT //for 11ac logo //FOR_VHT5G_PF
			( txcfg->pstat && 
			((txcfg->pstat->aggre_mthd == AGGRE_MTHD_MPDU_AMSDU) || (txcfg->pstat->aggre_mthd == AGGRE_MTHD_MPDU))
			&& txcfg->aggre_en ) ||
#endif
			((txcfg->aggre_en >= FG_AGGRE_MPDU) && (txcfg->aggre_en <= FG_AGGRE_MPDU_BUFFER_LAST))
			) {
				if (!pstat->ADDBA_ready[(int)skb->cb[1]]) {
					if ((pstat->ADDBA_req_num[(int)skb->cb[1]] < 5) && !pstat->ADDBA_sent[(int)skb->cb[1]]) {
						pstat->ADDBA_req_num[(int)skb->cb[1]]++;
						SMP_UNLOCK_XMIT(flags);
						issue_ADDBAreq(priv, pstat, (int)skb->cb[1]);
						SMP_LOCK_XMIT(flags);
						pstat->ADDBA_sent[(int)skb->cb[1]]++;
					}
				}
			}
#ifndef TXSC_HDR 
#ifdef CONFIG_PCI_HCI
#ifdef TX_SCATTER
			if (skb->list_num == 0) {
				reuse_txdesc = 1;
			} else {
				if (ptxsc_entry->has_desc3) {
					if (skb->list_num == 3 && (skb->list_buf[0].len == WLAN_ETHHDR_LEN) &&
						(skb->list_buf[2].len  == (get_desc(ptxsc_entry->hwdesc3.Dword7)&TX_TxBufSizeMask))){
						reuse_txdesc = 1;
					}
				} else {
					if ((skb->list_num == 1 && skb->list_buf[0].len > WLAN_ETHHDR_LEN) ||
						(skb->list_num == 2 && skb->list_buf[0].len == WLAN_ETHHDR_LEN))
						reuse_txdesc = 1;
				}
				if (skb->len == 0 && skb->list_num > 1) {
					skb->list_idx++;
					skb_assign_buf(skb, skb->list_buf[skb->list_idx].buf, skb->list_buf[skb->list_idx].len);
					skb->len = skb->list_buf[skb->list_idx].len;
				}
			}
#endif	//TX_SCATTER
#endif	//CONFIG_PCI_HCI
#endif

#ifdef TXSC_HDR
			if (txsc_lv2) {
#else

			// check if we could re-use tx descriptor
			if (
#ifdef WLAN_HAL_HW_TX_SHORTCUT_DISABLE_REUSE_TXDESC_FOR_DEBUG
				// TODO: tempory False for disable reuse txdesc, only hdr conversion
				FALSE &&
#endif
#if defined(CONFIG_PCI_HCI)
               (
#ifdef CONFIG_WLAN_HAL
                IS_HAL_CHIP(priv) ? 
#ifdef WLAN_HAL_HW_TX_SHORTCUT_REUSE_TXDESC
                ((GET_CHIP_VER(priv)==VERSION_8813A) ? 
                	TRUE : 
                	((GET_HAL_INTERFACE(priv)->GetShortCutTxBuffSizeHandler(priv, ptxsc_entry->hal_hw_desc)) > 0) ) 
#else
				((GET_HAL_INTERFACE(priv)->GetShortCutTxBuffSizeHandler(priv, ptxsc_entry->hal_hw_desc)) > 0)
#endif
                :
#endif // CONFIG_WLAN_HAL
                ((get_desc(ptxsc_entry->hwdesc1.Dword7)&TX_TxBufSizeMask) > 0) 

                )
            && (
#ifdef CONFIG_RTL_WAPI_SUPPORT
				// Note: for sw wapi, txcfg->mic=16; for hw wapi, txcfg->mic=0.
				 (txcfg->privacy==_WAPI_SMS4_) ? ((skb->len+txcfg->mic)==(get_desc(ptxsc_entry->hwdesc2.Dword7)&TX_TxBufSizeMask)) :
#endif
                (
#ifdef CONFIG_WLAN_HAL
                IS_HAL_CHIP(priv) ?
#ifdef WLAN_HAL_HW_TX_SHORTCUT_REUSE_TXDESC
				((GET_CHIP_VER(priv)==VERSION_8813A) ?
					TRUE:
					(skb->len == (GET_HAL_INTERFACE(priv)->GetShortCutTxBuffSizeHandler(priv, ptxsc_entry->hal_hw_desc))) )
#else
				(skb->len == (GET_HAL_INTERFACE(priv)->GetShortCutTxBuffSizeHandler(priv, ptxsc_entry->hal_hw_desc))) 
#endif
                :
#endif // CONFIG_WLAN_HAL
#ifdef TXSC_SKBLEN
                        ((GET_CHIP_VER(priv)== VERSION_8812E) ? 1 : (skb->len == (get_desc(ptxsc_entry->hwdesc2.Dword7)&TX_TxBufSizeMask)))
#else
                (skb->len == (get_desc(ptxsc_entry->hwdesc2.Dword7)&TX_TxBufSizeMask))
#endif
                )
            ) &&
#elif defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
				((get_desc(ptxsc_entry->hwdesc1.Dword7)&TX_TxBufSizeMask) > 0) &&
				(skb->len == (get_desc(ptxsc_entry->hwdesc1.Dword7)&TX_TxBufSizeMask)) &&
#endif
#if 1
                        ((GET_CHIP_VER(priv)== VERSION_8812E) ? 1 : (txcfg->tx_rate == ptxsc_entry->txcfg.tx_rate)) &&
#else
				(txcfg->tx_rate == ptxsc_entry->txcfg.tx_rate) &&
#endif	

				(pstat->protection == priv->pmib->dot11ErpInfo.protection) &&
				(pstat->ht_protection == priv->ht_protection)
#if defined(WIFI_WMM) && defined(WMM_APSD)
				&& (!(
#ifdef CLIENT_MODE
				(OPMODE & WIFI_AP_STATE) &&
#endif
				(APSD_ENABLE) && (pstat->state & WIFI_SLEEP_STATE)))
#endif
#ifdef CONFIG_PCI_HCI
#ifdef TX_SCATTER
				&& reuse_txdesc
#endif
#endif
				) {
#endif		

				pstat->tx_sc_pkts_lv2++;

				if (txcfg->privacy) {
					switch (txcfg->privacy) {
					case _WEP_104_PRIVACY_:
					case _WEP_40_PRIVACY_:
						wep_fill_iv(priv, txcfg->phdr, txcfg->hdr_len, ptxsc_entry->sc_keyid);
						break;

					case _TKIP_PRIVACY_:
						tkip_fill_encheader(priv, txcfg->phdr, txcfg->hdr_len, ptxsc_entry->sc_keyid);
						break;

					case _CCMP_PRIVACY_:
						aes_fill_encheader(priv, txcfg->phdr, txcfg->hdr_len, ptxsc_entry->sc_keyid);
						break;
					}
				}

#ifndef TXSC_HDR
#ifdef CONFIG_PCI_HCI
#ifdef CONFIG_WLAN_HAL                
				if (IS_HAL_CHIP(priv)) {
					// for TXBD mechanism, sending a packet always need one txdesc. So, txcfg->one_txdesc is useless.
				} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif // CONFIG_WLAN_HAL
				{//not HAL
					if ((skb_headroom(skb) >= (txcfg->hdr_len + txcfg->llc + txcfg->iv
#ifdef TX_EARLY_MODE
						+ (GET_TX_EARLY_MODE ? 8 : 0)
#endif
						)) &&
						!skb_cloned(skb) &&
						(txcfg->privacy != _TKIP_PRIVACY_) &&
#if defined(CONFIG_RTL_WAPI_SUPPORT)
						(txcfg->privacy != _WAPI_SMS4_) &&
#endif
						((((unsigned int)skb->data) % 2) == 0)
						)
					{
						memcpy((skb->data - (txcfg->hdr_len + txcfg->llc + txcfg->iv)), txcfg->phdr, (txcfg->hdr_len + txcfg->llc + txcfg->iv));
						release_wlanllchdr_to_poll(priv, txcfg->phdr);
						txcfg->phdr = skb->data - (txcfg->hdr_len + txcfg->llc + txcfg->iv);
						txcfg->one_txdesc = 1;
					}
				}
#endif // CONFIG_PCI_HCI

#endif
#ifdef RESERVE_TXDESC_FOR_EACH_IF
				if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc) {
					int ret=0;
					priv->use_txdesc_cnt[q_num] += (txcfg->one_txdesc)? 1 : 2;

                    
#ifdef CONFIG_WLAN_HAL                        
                    if(IS_HAL_CHIP(priv))
                    	ret = rtl88XX_signin_txdesc_shortcut(priv, txcfg, k);
		    else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif // CONFIG_WLAN_HAL
                    	ret = rtl8192cd_signin_txdesc_shortcut(priv, txcfg, k);

                    if(ret)
						priv->use_txdesc_cnt[q_num] -= (txcfg->one_txdesc)? 1 : 2;
				} else
#endif
				{
#if defined(CONFIG_PCI_HCI)
#ifdef CONFIG_WLAN_HAL
					if (IS_HAL_CHIP(priv)) {
						rtl88XX_signin_txdesc_shortcut(priv, txcfg, k);
					} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif // CONFIG_WLAN_HAL
					{//not HAL
						rtl8192cd_signin_txdesc_shortcut(priv, txcfg, k);
					}
#elif defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
					rtl8192cd_signin_txdesc_shortcut(priv, txcfg, ptxsc_entry);
#endif
				}
#if defined(SHORTCUT_STATISTIC) //defined(__ECOS) && defined(_DEBUG_RTL8192CD_)
				priv->ext_stats.tx_cnt_sc2++;
#endif
				goto stop_proc;
			}

#ifdef CONFIG_RTK_MESH
            if(txcfg->is_11s) {
                memcpy(&priv->ethhdr, &(ptxsc_entry->ethhdr), sizeof(struct wlan_ethhdr_t));
            }
#endif

#ifdef RESERVE_TXDESC_FOR_EACH_IF
			if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc) {
				int desc_num=2;	
				if ( txcfg->privacy
#if defined(CONFIG_RTL_WAPI_SUPPORT)
						&& (_WAPI_SMS4_ != txcfg->privacy)
#endif
#ifdef CONFIG_IEEE80211W
						&& UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE),0) ){
#else
						&& UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)) ){
#endif						
					desc_num=3;
				}
				priv->use_txdesc_cnt[q_num] += desc_num;
				if ( rtl8192cd_signin_txdesc(priv, txcfg)) {
					priv->use_txdesc_cnt[q_num] -= desc_num;
				}
			} else
#endif
			{
#if defined(CONFIG_PCI_HCI)
#ifdef  CONFIG_WLAN_HAL
				if (IS_HAL_CHIP(priv)) {
					rtl88XX_signin_txdesc(priv, txcfg, HW_TX_SC_HEADER_CONV);
				} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif
				{//not HAL
					rtl8192cd_signin_txdesc(priv, txcfg);
				}
#elif defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
#if defined(__ECOS) && defined(TX_SCATTER)
				if (skb->key)
					rtl8192cd_signin_txdesc(priv, txcfg, pethhdr); //TBD, ethhdr might be overwrite by WAPI
				else
					rtl8192cd_signin_txdesc(priv, txcfg, NULL);
#else
				rtl8192cd_signin_txdesc(priv, txcfg, NULL);
#endif
#endif
			}
#if defined(SHORTCUT_STATISTIC) //defined(__ECOS) && defined(_DEBUG_RTL8192CD_)
			priv->ext_stats.tx_cnt_sc1++; 
#endif
			ptxsc_entry->txcfg.tx_rate = txcfg->tx_rate;
			goto stop_proc;
	}
	if (!priv->pmib->dot11OperationEntry.disable_txsc && pstat)
		pstat->tx_sc_pkts_slow++;
#endif // TX_SHORTCUT
#if defined(CONFIG_RTK_MESH) || defined(A4_STA)
just_skip:
#endif

#if defined(SHORTCUT_STATISTIC) //defined(__ECOS) && defined(_DEBUG_RTL8192CD_)
	priv->ext_stats.tx_cnt_nosc++;
#endif

	/* ==================== Slow path of packet TX process ==================== */
	SMP_UNLOCK_XMIT(flags);
	if (rtl8192cd_tx_slowPath(priv, skb, pstat, dev, wdsDev, txcfg) == TX_PROCEDURE_CTRL_STOP) {
		SMP_LOCK_XMIT(flags);
		goto stop_proc;
	}
	SMP_LOCK_XMIT(flags);

#ifdef __KERNEL__
	dev->trans_start = jiffies;
#endif

	goto stop_proc;

free_and_stop:		/* Free current packet and stop TX process */

	rtl_kfree_skb(priv, skb, _SKB_TX_);

stop_proc:			/* Stop process and assume the TX-ed packet is already "processed" (freed or TXed) in previous code. */
	
#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	rtw_handle_xmit_fail(priv, txcfg);
#endif
	
	return 0;
}


#ifndef CONFIG_RTK_MESH
int	rtl8192cd_wlantx(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
	return (rtl8192cd_firetx(priv, txcfg));
}
#endif

#ifdef TX_SHORTCUT

#ifdef CONFIG_PCI_HCI
#ifdef CONFIG_WLAN_HAL
__MIPS16
__IRAM_IN_865X
int rtl88XX_signin_txdesc_shortcut(struct rtl8192cd_priv *priv, struct tx_insn *txcfg, int idx)
{
    struct tx_desc_info *pswdescinfo, *pdescinfo;
    struct rtl8192cd_hw *phw;
    int q_num;
    u2Byte *tx_head;
    struct stat_info *pstat;
    struct sk_buff *pskb;

    PHCI_TX_DMA_MANAGER_88XX        ptx_dma;
    u32                             halQNum;
    PHCI_TX_DMA_QUEUE_STRUCT_88XX   cur_q;
    PTX_BUFFER_DESCRIPTOR           cur_txbd;
    TX_DESC_DATA_88XX               desc_data;

    pstat = txcfg->pstat;
    pskb = (struct sk_buff *)txcfg->pframe;

    phw = GET_HW(priv);
    q_num = txcfg->q_num;

    halQNum     = GET_HAL_INTERFACE(priv)->MappingTxQueueHandler(priv, (u32)q_num);
    ptx_dma     = (PHCI_TX_DMA_MANAGER_88XX)(_GET_HAL_DATA(priv)->PTxDMA88XX);
    cur_q       = &(ptx_dma->tx_queue[halQNum]);
    cur_txbd    = cur_q->pTXBD_head + cur_q->host_idx;
    memset(&desc_data, 0, sizeof(TX_DESC_DATA_88XX));

    tx_head     = &(ptx_dma->tx_queue[halQNum].host_idx);

    pswdescinfo = get_txdesc_info(priv->pshare->pdesc_info, q_num);    

    pdescinfo = pswdescinfo + *tx_head;

	// TODO: if enable HW_SEQ , it should be disabled here...
    assign_wlanseq(GET_HW(priv), txcfg->phdr, pstat, GET_MIB(priv)
#ifdef CONFIG_RTK_MESH	// For broadcast data frame via mesh (ex:ARP requst)
    , txcfg->is_11s
#endif
        );

    //set Break
    if((txcfg->q_num >=1 && txcfg->q_num <=4)) {
        if((pstat != priv->pshare->CurPstat[txcfg->q_num-1])) {
            desc_data.bk = _TRUE;
            priv->pshare->CurPstat[txcfg->q_num-1] = pstat;
        } else {
            desc_data.bk = _FALSE;
        }
    } else {
        desc_data.bk = _TRUE;
    }

	if ((pstat->IOTPeer==HT_IOT_PEER_INTEL) && (pstat->retry_inc)) {
        if (is_MCS_rate(pstat->current_tx_rate) && !(pstat->leave)
            && priv->pshare->intel_rty_lmt) {
            desc_data.rtyLmtEn   = _TRUE;
            desc_data.dataRtyLmt = priv->pshare->intel_rty_lmt; 
        } else {
            desc_data.rtyLmtEn   = _FALSE;
            desc_data.dataRtyLmt = 1;
        }
    }


    desc_data.iv = txcfg->iv;
    desc_data.secType = txcfg->privacy;

	if((priv->pshare->rf_ft_var.txforce != 0xff)
#ifdef BEAMFORMING_SUPPORT
		&& (!txcfg->ndpa) 
#endif
	){
        desc_data.dataRate  = priv->pshare->rf_ft_var.txforce;
        desc_data.disDataFB = _TRUE;
        desc_data.disRTSFB  = _TRUE;
        desc_data.useRate   = _TRUE;
	}

#ifdef BEAMFORMING_SUPPORT
        if (priv->pmib->dot11nConfigEntry.dot11nSTBC && (txcfg->pstat) &&
			((txcfg->pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_RX_STBC_CAP_)) 
#ifdef RTK_AC_SUPPORT			
				|| (txcfg->pstat->vht_cap_buf.vht_cap_info & cpu_to_le32(_VHTCAP_RX_STBC_CAP_))
#endif
		)){
			u1Byte					Idx = 0;
			PRT_BEAMFORMING_ENTRY	pEntry; 
			pEntry = Beamforming_GetEntryByMacId(priv, txcfg->pstat->aid, &Idx);
			if(((get_rf_mimo_mode(priv) == MIMO_2T2R) || (get_rf_mimo_mode(priv) == MIMO_3T3R)) && (pEntry == NULL))
    	        desc_data.dataStbc = 1;
        }
#endif
#if 0
//#ifdef TX_EARLY_MODE
    if (GET_TX_EARLY_MODE) 
        pdesc->Dword8 = set_desc(get_physical_addr(priv, txcfg->phdr-8,
            (get_desc(pdesc->Dword7)& TX_TxBufSizeMask), PCI_DMA_TODEVICE));
    else
#endif

#if 0
        pdescinfo->pframe = txcfg->phdr;
        //pdescinfo->buf_type[0] = txcfg->fr_type;
        //pdescinfo->buf_pframe[0] = txcfg->pframe;
#if defined(WIFI_WMM) && defined(WMM_APSD)
        pdescinfo->priv = priv;
#ifndef TXDESC_INFO	
        pdescinfo->pstat = pstat;
#endif
#endif
#endif // 0

#ifdef CLIENT_MODE
    if (OPMODE & WIFI_STATION_STATE) {
        if (GetFrameSubType(txcfg->phdr) == WIFI_PSPOLL) {
            desc_data.navUseHdr = _TRUE;
        }

        if (priv->ps_state)
            SetPwrMgt(txcfg->phdr);
        else
            ClearPwrMgt(txcfg->phdr);
    }
#endif

//    if (txcfg->one_txdesc)
//        goto one_txdesc;

    // TODO: have_hw_mic
//one_txdesc:

    //descinfo_copy(pdescinfo, &pstat->tx_sc_ent[idx].swdesc1);
    pdescinfo->type = pstat->tx_sc_ent[idx].swdesc1.type;
#ifndef TXDESC_INFO	
    pdescinfo->len = pstat->tx_sc_ent[idx].swdesc1.len;
    pdescinfo->rate = pstat->tx_sc_ent[idx].swdesc1.rate;  
#endif	

    //??? pdescinfo->buf_type[0] = pstat->tx_sc_ent[idx].swdesc1.buf_type[0];
    pdescinfo->buf_type[0] = txcfg->fr_type;
    
    pdescinfo->pframe = txcfg->phdr;
    //pdescinfo->buf_pframe[0] = txcfg->pframe;
#if defined(WIFI_WMM) && defined(WMM_APSD)
    pdescinfo->priv = priv;
#ifndef TXDESC_INFO		
    pdescinfo->pstat = pstat;
#endif	
#endif
    
    desc_data.pHdr   = txcfg->phdr;    
    desc_data.hdrLen = txcfg->hdr_len;
    desc_data.llcLen = txcfg->llc;
    desc_data.frLen  = txcfg->fr_len;   // pskb->len ??
    //if (pskb->len != txcfg->fr_len) {
    //    printk("%s(%d): pskb->len:%d, txcfg->fr_len:%d \n", __FUNCTION__, __LINE__, pskb->len, txcfg->fr_len);
    //}
    desc_data.pBuf   = pskb->data;

#ifdef CONFIG_WLAN_HAL_8813AE
	if (GET_CHIP_VER(priv)==VERSION_8813A) {
#ifdef WLAN_HAL_HW_TX_SHORTCUT_REUSE_TXDESC
		desc_data.stwEn = TRUE;
		if (txcfg->pstat->aid != MANAGEMENT_AID) {
			desc_data.macId = REMAP_AID(txcfg->pstat);
    }
#endif // WLAN_HAL_HW_TX_SHORTCUT_REUSE_TXDESC
#ifdef WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
		desc_data.smhEn = TRUE;

		desc_data.pHdr	   = txcfg->phdr; // 802.3 hdr len + payload len;
		desc_data.hdrLen   = txcfg->fr_len; // 802.3 hdr + payload

		desc_data.llcLen   = 0;

		desc_data.frLen    = 0;
		desc_data.pBuf	   = 0; // useless....because frLen is zero
		// TODO: recycle problem for sw desc..
#endif // WLAN_HAL_HW_TX_SHORTCUT_HDR_CONV
	}
#endif // CONFIG_WLAN_HAL_8813AE

    GET_HAL_INTERFACE(priv)->FillShortCutTxHwCtrlHandler(
        priv, halQNum, (void *)&desc_data, pstat->tx_sc_ent[idx].hal_hw_desc, 0x02);

	// TODO: check the sw desc here for HW_TX_SHORTCUT
#ifndef TXDESC_INFO	
    pdescinfo->paddr         = get_desc(cur_txbd->TXBD_ELE[1].Dword1);//header address
#endif	
    pdescinfo->buf_paddr[0]  = get_desc(cur_txbd->TXBD_ELE[2].Dword1);//payload address
    pdescinfo->buf_pframe[0] = pskb;
    pdescinfo->buf_len[0]    = txcfg->fr_len; // pskb->len;

    GET_HAL_INTERFACE(priv)->SyncSWTXBDHostIdxToHWHandler(priv, halQNum);

    return 0;
}
#endif // CONFIG_WLAN_HAL

#ifdef CONFIG_RTL_8812_SUPPORT
__MIPS16
__IRAM_IN_865X
int rtl8192cd_signin_txdesc_shortcut_8812(struct rtl8192cd_priv *priv, struct tx_insn *txcfg, int idx)
{
	struct tx_desc *phdesc, *pdesc, *pfrstdesc;
	struct tx_desc_info *pswdescinfo, *pdescinfo;
	struct rtl8192cd_hw	*phw;
	int *tx_head, q_num;
	struct stat_info *pstat;
	struct sk_buff *pskb;
	unsigned long pfrst_dma_desc;
	unsigned long *dma_txhead;
/*
#if defined(CONFIG_RTL_WAPI_SUPPORT)
	uint8				*wapiMic2;
	struct tx_desc		*pmicdesc;
	struct tx_desc_info	*pmicdescinfo;
#endif
*/
	pstat = txcfg->pstat;
	pskb = (struct sk_buff *)txcfg->pframe;
	pfrst_dma_desc=0;

	phw	= GET_HW(priv);
	q_num = txcfg->q_num;

	dma_txhead	= get_txdma_addr(phw, q_num);
	tx_head		= get_txhead_addr(phw, q_num);
	phdesc		= get_txdesc(phw, q_num);
	pswdescinfo	= get_txdesc_info(priv->pshare->pdesc_info, q_num);

	/*------------------------------------------------------------*/
	/*           fill descriptor of header + iv + llc             */
	/*------------------------------------------------------------*/
	pfrstdesc = pdesc = phdesc + *tx_head;
	pdescinfo = pswdescinfo + *tx_head;

    memcpy(pdesc, &pstat->tx_sc_ent[idx].hwdesc1, 40);

	assign_wlanseq(GET_HW(priv), txcfg->phdr, pstat, GET_MIB(priv)
#ifdef CONFIG_RTK_MESH	// For broadcast data frame via mesh (ex:ARP requst)
	, txcfg->is_11s
#endif
		);

	pdesc->Dword9 = 0;
	pdesc->Dword9 |= set_desc((GetSequence(txcfg->phdr) & TXdesc_92E_TX_SeqMask)  << TXdesc_92E_TX_SeqSHIFT);	

//	if (txcfg->pstat)
//		pdesc->Dword1 |= set_desc(txcfg->pstat->aid & TX_MACIDMask);

#if 1
//#ifndef TXSC_HDR
	//set Break
	if((txcfg->q_num >=1 && txcfg->q_num <=4)){
		if((pstat != priv->pshare->CurPstat[txcfg->q_num-1])) {

				pdesc->Dword2 |= set_desc(TXdesc_92E_BK);

			priv->pshare->CurPstat[txcfg->q_num-1] = pstat;
		} else
			pdesc->Dword2 &= set_desc(~TXdesc_92E_BK);

	} else {
			pdesc->Dword2 |= set_desc(TXdesc_92E_BK);
	}
#endif

	if((priv->pshare->rf_ft_var.txforce != 0xff)
#ifdef BEAMFORMING_SUPPORT
		&& (!txcfg->ndpa) 
#endif
	){
		pdesc->Dword4 &= set_desc(~(TXdesc_92E_DataRateMask << TXdesc_92E_DataRateSHIFT));
		pdesc->Dword3 |= set_desc(TXdesc_92E_DisDataFB|TXdesc_92E_DisRtsFB|TXdesc_92E_UseRate);
		pdesc->Dword4 |= set_desc((priv->pshare->rf_ft_var.txforce & TXdesc_92E_DataRateMask) << TXdesc_92E_DataRateSHIFT);
	}


	if (txcfg->one_txdesc) {
#ifdef TX_EARLY_MODE
		if (GET_TX_EARLY_MODE) {
			pdesc->Dword0 = set_desc(((get_desc(pdesc->Dword0) & 0xff00ffff) |(0x28 << TX_OffsetSHIFT)) |
									TX_LastSeg | 	(txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len));
			pdesc->Dword1 = set_desc(get_desc(pdesc->Dword1) | (1 << TX_PktOffsetSHIFT) );
			pdesc->Dword7 = set_desc((get_desc(pdesc->Dword7) & 0xffff0000) |
						(txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len+8));
			memset(txcfg->phdr-8, '\0', 8);			
			if (pstat->empkt_num > 0) 				
				insert_emcontent(priv, txcfg, txcfg->phdr-8);
			pdesc->Dword10 = set_desc(get_physical_addr(priv, txcfg->phdr-8,
				(get_desc(pdesc->Dword7)&TX_TxBufSizeMask), PCI_DMA_TODEVICE));
					
		}
		else
#endif
		{	
			pdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & 0xffff0000) |
				TX_LastSeg | (txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len));
			pdesc->Dword7 = set_desc((get_desc(pdesc->Dword7) & 0xffff0000) |
				(txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len));
		}
	}

#ifdef TXSC_SKBLEN
    else {
		pdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & 0xffff0000) |
						 (txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len));
		pdesc->Dword7 = set_desc((get_desc(pdesc->Dword7) & 0xffff0000) |
						  (txcfg->hdr_len + txcfg->llc + txcfg->iv ));
	}
#endif

#ifdef BEAMFORMING_SUPPORT
	if ((priv->pmib->dot11RFEntry.txbf == 1) && (priv->pmib->dot11nConfigEntry.dot11nSTBC))	{
		if ((pstat->ht_cap_buf.ht_cap_info & cpu_to_le16(_HTCAP_RX_STBC_CAP_)) 
#ifdef RTK_AC_SUPPORT			
				|| (txcfg->pstat->vht_cap_buf.vht_cap_info & cpu_to_le32(_VHTCAP_RX_STBC_CAP_))
#endif
		) {		
			u1Byte					Idx = 0;
			PRT_BEAMFORMING_ENTRY	pEntry; 
			pEntry = Beamforming_GetEntryByMacId(priv, pstat->aid, &Idx);
			if(pEntry)
				pdesc->Dword5 &= set_desc(~ (BIT(TXdesc_92E_DataStbcSHIFT)));
		}
	}
#endif
#ifdef TX_EARLY_MODE
	if (GET_TX_EARLY_MODE) 
		pdesc->Dword10 = set_desc(get_physical_addr(priv, txcfg->phdr-8,
			(get_desc(pdesc->Dword7)& TX_TxBufSizeMask), PCI_DMA_TODEVICE));
	else
#endif
	pdesc->Dword10 = set_desc(get_physical_addr(priv, txcfg->phdr,
		(get_desc(pdesc->Dword7)& TX_TxBufSizeMask), PCI_DMA_TODEVICE));

	descinfo_copy(pdescinfo, &pstat->tx_sc_ent[idx].swdesc1);
#ifndef TXDESC_INFO
	pdescinfo->paddr  = get_desc(pdesc->Dword10); // buffer addr
#endif	
	if (txcfg->one_txdesc) {
		pdescinfo->type = _SKB_FRAME_TYPE_;
		pdescinfo->pframe = pskb;
		pdescinfo->priv = priv;
#if defined(WIFI_WMM) && defined(WMM_APSD)
#ifndef TXDESC_INFO
		pdescinfo->pstat = pstat;
#endif		
#endif
	}
	else {
		pdescinfo->pframe = txcfg->phdr;
#if defined(WIFI_WMM) && defined(WMM_APSD)
		pdescinfo->priv = priv;
#ifndef TXDESC_INFO		
		pdescinfo->pstat = pstat;
#endif		
#endif
	}

#ifdef CLIENT_MODE
	if (OPMODE & WIFI_STATION_STATE) {
		if (GetFrameSubType(pdescinfo->pframe) == WIFI_PSPOLL)
			pdesc->Dword1 |= set_desc(TX_NAVUSEHDR);

		if (priv->ps_state)
			SetPwrMgt(pdescinfo->pframe);
		else
			ClearPwrMgt(pdescinfo->pframe);
	}
#endif

	pfrst_dma_desc = dma_txhead[*tx_head];
/*
#ifdef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, get_desc(pdesc->Dword8), (get_desc(pdesc->Dword7)&TX_TxBufferSizeMask), PCI_DMA_TODEVICE);
#endif
*/
	txdesc_rollover(pdesc, (unsigned int *)tx_head);

	if (txcfg->one_txdesc)
		goto one_txdesc;

	/*------------------------------------------------------------*/
	/*              fill descriptor of frame body                 */
	/*------------------------------------------------------------*/
	pdesc = phdesc + *tx_head;
	pdescinfo = pswdescinfo + *tx_head;
    memcpy(pdesc, &pstat->tx_sc_ent[idx].hwdesc2, 40);

#ifdef TXSC_SKBLEN
	pdesc->Dword7 = set_desc((get_desc(pdesc->Dword7) & 0xffff0000) |
				  ( txcfg->fr_len));
#endif

	pdesc->Dword10 = set_desc(get_physical_addr(priv, pskb->data,
		(get_desc(pdesc->Dword7)&0x0fff), PCI_DMA_TODEVICE));

	descinfo_copy(pdescinfo, &pstat->tx_sc_ent[idx].swdesc2);
#ifndef TXDESC_INFO
	pdescinfo->paddr  = get_desc(pdesc->Dword10);
#endif	
	pdescinfo->pframe = pskb;
	pdescinfo->priv = priv;
/*
#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, dma_txhead[*tx_head], sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#else
	rtl_cache_sync_wback(priv, get_desc(pdesc->Dword8), (get_desc(pdesc->Dword7)&TX_TxBufferSizeMask), PCI_DMA_TODEVICE);
#endif
*/
	txdesc_rollover(pdesc, (unsigned int *)tx_head);

#if defined(CONFIG_RTL_WAPI_SUPPORT)
	if (txcfg->privacy == _WAPI_SMS4_)
	{
		SecSWSMS4Encryption(priv, txcfg);
	}
#endif

#ifndef NOT_RTK_BSP
	if ((txcfg->privacy == _TKIP_PRIVACY_) &&
		(priv->pshare->have_hw_mic) &&
		!(priv->pmib->dot11StationConfigEntry.swTkipMic))
	{
		register unsigned long int l,r;
		unsigned char *mic;
		int delay = 18;

		while ((*(volatile unsigned int *)GDMAISR & GDMA_COMPIP) == 0) {
			delay_us(delay);
			delay = delay / 2;
		}

		l = *(volatile unsigned int *)GDMAICVL;
		r = *(volatile unsigned int *)GDMAICVR;

		mic = ((struct sk_buff *)txcfg->pframe)->data + txcfg->fr_len - 8;
		mic[0] = (unsigned char)(l & 0xff);
		mic[1] = (unsigned char)((l >> 8) & 0xff);
		mic[2] = (unsigned char)((l >> 16) & 0xff);
		mic[3] = (unsigned char)((l >> 24) & 0xff);
		mic[4] = (unsigned char)(r & 0xff);
		mic[5] = (unsigned char)((r >> 8) & 0xff);
		mic[6] = (unsigned char)((r >> 16) & 0xff);
		mic[7] = (unsigned char)((r >> 24) & 0xff);

#ifdef MICERR_TEST
		if (priv->micerr_flag) {
			mic[7] ^= mic[7];
			priv->micerr_flag = 0;
		}
#endif
	}
#endif // NOT_RTK_BSP

#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(get_desc(pdesc->Dword10)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), (get_desc(pdesc->Dword7)&TX_TxBufSizeMask), PCI_DMA_TODEVICE);

one_txdesc:

	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(get_desc(pfrstdesc->Dword10)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), (get_desc(pfrstdesc->Dword7)&TX_TxBufSizeMask), PCI_DMA_TODEVICE);

#ifdef SUPPORT_SNMP_MIB
	if (txcfg->rts_thrshld <= get_mpdu_len(txcfg, txcfg->fr_len))
		SNMP_MIB_INC(dot11RTSSuccessCount, 1);
#endif

	pfrstdesc->Dword0 |= set_desc(TX_OWN);

#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(pfrst_dma_desc-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

	if (q_num == HIGH_QUEUE) {
		DEBUG_WARN("signin shortcut for DTIM pkt?\n");
		return 0;
	} else {
		tx_poll(priv, q_num);
	}

	return 0;
}
#endif

#if(CONFIG_WLAN_NOT_HAL_EXIST==1)
__MIPS16
__IRAM_IN_865X
int rtl8192cd_signin_txdesc_shortcut(struct rtl8192cd_priv *priv, struct tx_insn *txcfg, int idx)
{
	struct tx_desc *phdesc, *pdesc, *pfrstdesc;
	struct tx_desc_info *pswdescinfo, *pdescinfo;
	struct rtl8192cd_hw	*phw;
	int *tx_head, q_num;
	struct stat_info *pstat;
	struct sk_buff *pskb;
	unsigned long pfrst_dma_desc;
	unsigned long *dma_txhead;
/*
#if defined(CONFIG_RTL_WAPI_SUPPORT)
	uint8				*wapiMic2;
	struct tx_desc		*pmicdesc;
	struct tx_desc_info	*pmicdescinfo;
#endif
*/
#ifdef TX_SCATTER
	int	go_desc3=0;
#endif
#ifdef CONFIG_RTL_8812_SUPPORT
	if(GET_CHIP_VER(priv)== VERSION_8812E)
		return rtl8192cd_signin_txdesc_shortcut_8812(priv, txcfg, idx);
#endif

	pstat = txcfg->pstat;
	pskb = (struct sk_buff *)txcfg->pframe;
	pfrst_dma_desc=0;

	phw	= GET_HW(priv);
	q_num = txcfg->q_num;

	dma_txhead	= get_txdma_addr(phw, q_num);
	tx_head		= get_txhead_addr(phw, q_num);
	phdesc		= get_txdesc(phw, q_num);
	pswdescinfo	= get_txdesc_info(priv->pshare->pdesc_info, q_num);

	/*------------------------------------------------------------*/
	/*           fill descriptor of header + iv + llc             */
	/*------------------------------------------------------------*/
	pfrstdesc = pdesc = phdesc + *tx_head;
	pdescinfo = pswdescinfo + *tx_head;
   	
	desc_copy(pdesc, &pstat->tx_sc_ent[idx].hwdesc1);	
	assign_wlanseq(GET_HW(priv), txcfg->phdr, pstat, GET_MIB(priv)
#ifdef CONFIG_RTK_MESH	// For broadcast data frame via mesh (ex:ARP requst)
	, txcfg->is_11s
#endif
		);

			
	pdesc->Dword3 = 0;
	pdesc->Dword3 = set_desc((GetSequence(txcfg->phdr) & TX_SeqMask) << TX_SeqSHIFT);

#if defined(CONFIG_RTL_92D_SUPPORT) || defined(CONFIG_RTL_92C_SUPPORT)
	if (CHIP_VER_92X_SERIES(priv))
#endif
	if (priv->pmib->dot11RFEntry.txbf == 1) {
		pdesc->Dword2 &= set_desc(0x03ffffff); // clear related bits
		pdesc->Dword2 |= set_desc(1 << TX_TxAntCckSHIFT);	// Set Default CCK rate with 1T
		pdesc->Dword2 |= set_desc(1 << TX_TxAntlSHIFT); 	// Set Default Legacy rate with 1T
		pdesc->Dword2 |= set_desc(1 << TX_TxAntHtSHIFT);	// Set Default Ht rate

		if (is_MCS_rate(txcfg->tx_rate)) {
			if ((txcfg->tx_rate - HT_RATE_ID) <= 6){
					pdesc->Dword2 |= set_desc(3 << TX_TxAntHtSHIFT); // Set Ht rate < MCS6 with 2T
			}
		}
	}
	if(priv->pmib->dot11RFEntry.bcn2path){
		pdesc->Dword2 &= set_desc(0x03ffffff); // clear related bits
		pdesc->Dword2 |= set_desc(1 << TX_TxAntCckSHIFT);	// Set Default CCK rate with 1T
	}

//	if (txcfg->pstat)
//		pdesc->Dword1 |= set_desc(txcfg->pstat->aid & TX_MACIDMask);

	//set Break
	if((txcfg->q_num >=1 && txcfg->q_num <=4)){
		if((pstat != priv->pshare->CurPstat[txcfg->q_num-1])) {
#ifdef CONFIG_RTL_88E_SUPPORT
			if (GET_CHIP_VER(priv)==VERSION_8188E)
				pdesc->Dword2 |= set_desc(TXdesc_88E_BK);
			else
#endif
				pdesc->Dword1 |= set_desc(TX_BK);
			priv->pshare->CurPstat[txcfg->q_num-1] = pstat;
		} else
#ifdef CONFIG_RTL_88E_SUPPORT
		if (GET_CHIP_VER(priv)==VERSION_8188E) {
			pdesc->Dword2 &= set_desc(~TXdesc_88E_BK);
		} else
#endif
		{
			pdesc->Dword1 &= set_desc(~TX_BK); // clear it
		}
	} else {
#ifdef CONFIG_RTL_88E_SUPPORT
		if (GET_CHIP_VER(priv)==VERSION_8188E)
			pdesc->Dword2 |= set_desc(TXdesc_88E_BK);
		else
#endif
			pdesc->Dword1 |= set_desc(TX_BK);
	}


	if ((pstat->IOTPeer==HT_IOT_PEER_INTEL) && (pstat->retry_inc)) {
		if (is_MCS_rate(pstat->current_tx_rate) && !(pstat->leave)
			&& priv->pshare->intel_rty_lmt) {
			pdesc->Dword5 |= set_desc(TX_RtyLmtEn);
			
			pdesc->Dword5 &= set_desc(~(TX_DataRtyLmtMask << TX_DataRtyLmtSHIFT));
			pdesc->Dword5 |= set_desc((priv->pshare->intel_rty_lmt & TX_DataRtyLmtMask) << TX_DataRtyLmtSHIFT);
		} else {
			pdesc->Dword5 &= set_desc(~TX_RtyLmtEn);
			pdesc->Dword5 &= set_desc(~(TX_DataRtyLmtMask << TX_DataRtyLmtSHIFT));
		}
	}

#if defined(CONFIG_RTL_88E_SUPPORT) && defined(TXREPORT)
	if ((GET_CHIP_VER(priv)==VERSION_8188E) && (!txcfg->fixed_rate)) {
		if (pstat->ht_current_tx_info & TX_USE_SHORT_GI)
			pdesc->Dword5 |= set_desc(TX_SGI);
		else
			pdesc->Dword5 &= set_desc(~TX_SGI);
	}
#endif

	if (txcfg->one_txdesc) {
#ifdef TX_EARLY_MODE
		if (GET_TX_EARLY_MODE) {
			pdesc->Dword0 = set_desc(((get_desc(pdesc->Dword0) & 0xff00ffff) |(0x28 << TX_OffsetSHIFT)) |
									TX_LastSeg | 	(txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len));
			pdesc->Dword1 = set_desc(get_desc(pdesc->Dword1) | (1 << TX_PktOffsetSHIFT) );
			pdesc->Dword7 = set_desc((get_desc(pdesc->Dword7) & 0xffff0000) |
						(txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len+8));
#ifdef CONFIG_RTL_88E_SUPPORT
			if (GET_CHIP_VER(priv) == VERSION_8188E) {
				pdesc->Dword6 &= set_desc(~ (0xf << TX_MaxAggNumSHIFT));
				pdesc->Dword6 |= set_desc(0xf << TX_MaxAggNumSHIFT);
			}
#endif
			memset(txcfg->phdr-8, '\0', 8);			
			if (pstat->empkt_num > 0) 				
				insert_emcontent(priv, txcfg, txcfg->phdr-8);
			pdesc->Dword8 = set_desc(get_physical_addr(priv, txcfg->phdr-8,
				(get_desc(pdesc->Dword7)&TX_TxBufSizeMask), PCI_DMA_TODEVICE));
					
		}
		else
#endif
		{	
#if defined(TX_EARLY_MODE) && defined(CONFIG_RTL_88E_SUPPORT)
			if (GET_CHIP_VER(priv) == VERSION_8188E)
				pdesc->Dword6 &= set_desc(~ (0x0f << TX_MaxAggNumSHIFT));
#endif
			pdesc->Dword0 = set_desc((get_desc(pdesc->Dword0) & 0xffff0000) |
				TX_LastSeg | (txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len));
			pdesc->Dword7 = set_desc((get_desc(pdesc->Dword7) & 0xffff0000) |
				(txcfg->hdr_len + txcfg->llc + txcfg->iv + txcfg->fr_len));
		}
	}

	if(priv->pshare->rf_ft_var.txforce != 0xff)	{
		pdesc->Dword5 &= set_desc(~(TX_DataRateMask << TX_DataRateSHIFT));
		pdesc->Dword4 |= set_desc(TX_UseRate);
		pdesc->Dword5 |= set_desc((priv->pshare->rf_ft_var.txforce & TX_DataRateMask) << TX_DataRateSHIFT);
	}

#ifdef TX_EARLY_MODE
	if (GET_TX_EARLY_MODE) 
		pdesc->Dword8 = set_desc(get_physical_addr(priv, txcfg->phdr-8,
			(get_desc(pdesc->Dword7)& TX_TxBufSizeMask), PCI_DMA_TODEVICE));
	else
#endif
	pdesc->Dword8 = set_desc(get_physical_addr(priv, txcfg->phdr,
		(get_desc(pdesc->Dword7)& TX_TxBufSizeMask), PCI_DMA_TODEVICE));

	descinfo_copy(pdescinfo, &pstat->tx_sc_ent[idx].swdesc1);
#ifndef TXDESC_INFO
	pdescinfo->paddr  = get_desc(pdesc->Dword8); // buffer addr
#endif	
	if (txcfg->one_txdesc) {
		pdescinfo->type = _SKB_FRAME_TYPE_;
		pdescinfo->pframe = pskb;
		pdescinfo->priv = priv;
#if defined(WIFI_WMM) && defined(WMM_APSD)
#ifndef TXDESC_INFO
		pdescinfo->pstat = pstat;
#endif		
#endif
	}
	else {
		pdescinfo->pframe = txcfg->phdr;
#if defined(WIFI_WMM) && defined(WMM_APSD)
		pdescinfo->priv = priv;
#ifndef TXDESC_INFO		
		pdescinfo->pstat = pstat;
#endif		
#endif
	}

#ifdef CLIENT_MODE
	if (OPMODE & WIFI_STATION_STATE) {
		if (GetFrameSubType(pdescinfo->pframe) == WIFI_PSPOLL)
			pdesc->Dword1 |= set_desc(TX_NAVUSEHDR);

		if (priv->ps_state)
			SetPwrMgt(pdescinfo->pframe);
		else
			ClearPwrMgt(pdescinfo->pframe);
	}
#endif

	pfrst_dma_desc = dma_txhead[*tx_head];
/*
#ifdef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, get_desc(pdesc->Dword8), (get_desc(pdesc->Dword7)&TX_TxBufferSizeMask), PCI_DMA_TODEVICE);
#endif
*/
	txdesc_rollover(pdesc, (unsigned int *)tx_head);

	if (txcfg->one_txdesc)
		goto one_txdesc;

	/*------------------------------------------------------------*/
	/*              fill descriptor of frame body                 */
	/*------------------------------------------------------------*/
#ifdef TX_SCATTER
next_desc:
#endif	
	pdesc = phdesc + *tx_head;
	pdescinfo = pswdescinfo + *tx_head;
#ifdef TX_SCATTER
	if (go_desc3)
		desc_copy(pdesc, &pstat->tx_sc_ent[idx].hwdesc3);
	else
#endif
		desc_copy(pdesc, &pstat->tx_sc_ent[idx].hwdesc2);

#ifdef TX_SCATTER
	if (pskb->list_num > 1) {
		if (go_desc3)
			pdesc->Dword7 = set_desc((get_desc(pdesc->Dword7) & 0xffff0000) |
				pskb->list_buf[2].len);
		else
			pdesc->Dword7 = set_desc((get_desc(pdesc->Dword7) & 0xffff0000) |
				pskb->list_buf[1].len);
	}
#endif

	pdesc->Dword8 = set_desc(get_physical_addr(priv, pskb->data,
		(get_desc(pdesc->Dword7)&0x0fff), PCI_DMA_TODEVICE));

#ifdef TX_SCATTER
	if (go_desc3) {
		descinfo_copy(pdescinfo, &pstat->tx_sc_ent[idx].swdesc3);
		pdescinfo->type = _RESERVED_FRAME_TYPE_;
	} else
#endif
	{
		descinfo_copy(pdescinfo, &pstat->tx_sc_ent[idx].swdesc2);
	}
#ifndef TXDESC_INFO
	pdescinfo->paddr  = get_desc(pdesc->Dword8);
#endif	
	pdescinfo->pframe = pskb;
	pdescinfo->priv = priv;
/*
#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, dma_txhead[*tx_head], sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#else
	rtl_cache_sync_wback(priv, get_desc(pdesc->Dword8), (get_desc(pdesc->Dword7)&TX_TxBufferSizeMask), PCI_DMA_TODEVICE);
#endif
*/
	txdesc_rollover(pdesc, (unsigned int *)tx_head);

#if defined(CONFIG_RTL_WAPI_SUPPORT)
	if (txcfg->privacy == _WAPI_SMS4_)
	{
		SecSWSMS4Encryption(priv, txcfg);
	}
#endif

#ifndef NOT_RTK_BSP
	if ((txcfg->privacy == _TKIP_PRIVACY_) &&
		(priv->pshare->have_hw_mic) &&
		!(priv->pmib->dot11StationConfigEntry.swTkipMic))
	{
		register unsigned long int l,r;
		unsigned char *mic;
		int delay = 18;

		while ((*(volatile unsigned int *)GDMAISR & GDMA_COMPIP) == 0) {
			delay_us(delay);
			delay = delay / 2;
		}

		l = *(volatile unsigned int *)GDMAICVL;
		r = *(volatile unsigned int *)GDMAICVR;

		mic = ((struct sk_buff *)txcfg->pframe)->data + txcfg->fr_len - 8;
		mic[0] = (unsigned char)(l & 0xff);
		mic[1] = (unsigned char)((l >> 8) & 0xff);
		mic[2] = (unsigned char)((l >> 16) & 0xff);
		mic[3] = (unsigned char)((l >> 24) & 0xff);
		mic[4] = (unsigned char)(r & 0xff);
		mic[5] = (unsigned char)((r >> 8) & 0xff);
		mic[6] = (unsigned char)((r >> 16) & 0xff);
		mic[7] = (unsigned char)((r >> 24) & 0xff);

#ifdef MICERR_TEST
		if (priv->micerr_flag) {
			mic[7] ^= mic[7];
			priv->micerr_flag = 0;
		}
#endif
	}
#endif // NOT_RTK_BSP

#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(dma_txhead[*tx_head]-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(get_desc(pdesc->Dword8)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), (get_desc(pdesc->Dword7)&TX_TxBufSizeMask), PCI_DMA_TODEVICE);
#ifdef TX_SCATTER
	if (pstat->tx_sc_ent[idx].has_desc3 && go_desc3 == 0) {
		go_desc3 = 1;
		goto next_desc;
	}
#endif
one_txdesc:

	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(get_desc(pfrstdesc->Dword8)-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), (get_desc(pfrstdesc->Dword7)&TX_TxBufSizeMask), PCI_DMA_TODEVICE);

#ifdef SUPPORT_SNMP_MIB
	if (txcfg->rts_thrshld <= get_mpdu_len(txcfg, txcfg->fr_len))
		SNMP_MIB_INC(dot11RTSSuccessCount, 1);
#endif

	pfrstdesc->Dword0 |= set_desc(TX_OWN);

#ifndef USE_RTL8186_SDK
	rtl_cache_sync_wback(priv, (unsigned long)bus_to_virt(pfrst_dma_desc-CONFIG_LUNA_SLAVE_PHYMEM_OFFSET), sizeof(struct tx_desc), PCI_DMA_TODEVICE);
#endif

	if (q_num == HIGH_QUEUE) {
		DEBUG_WARN("signin shortcut for DTIM pkt?\n");
		return 0;
	} else {
		tx_poll(priv, q_num);
	}

	return 0;
}
#else
__MIPS16
__IRAM_IN_865X
int rtl8192cd_signin_txdesc_shortcut(struct rtl8192cd_priv *priv, struct tx_insn *txcfg, int idx)
{
	return 0;
}
#endif//CONFIG_WLAN_NOT_HAL_EXIST
#endif // CONFIG_PCI_HCI
#endif // TX_SHORTCUT

#ifdef CONFIG_PCI_HCI
/* This sub-routine is gonna to check how many tx desc we need */
static int check_txdesc(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
	struct sk_buff 	*pskb=NULL;
	unsigned short  protocol;
	unsigned char   *da=NULL;
	struct stat_info	*pstat=NULL;
	int priority=0;
	unsigned int is_dhcp = 0;

	if (txcfg->aggre_en == FG_AGGRE_MSDU_MIDDLE || txcfg->aggre_en == FG_AGGRE_MSDU_LAST)
		return TRUE;

	txcfg->privacy = txcfg->iv = txcfg->icv = txcfg->mic = 0;
	txcfg->frg_num = 0;
	txcfg->need_ack = 1;

	if (txcfg->fr_type == _SKB_FRAME_TYPE_)
	{
		pskb = ((struct sk_buff *)txcfg->pframe);
#ifdef TX_SCATTER
		if (pskb->list_num > 0)
			txcfg->fr_len = pskb->total_len - WLAN_ETHHDR_LEN;
		else
#endif
			txcfg->fr_len = pskb->len - WLAN_ETHHDR_LEN;

#ifdef MP_TEST
		if (OPMODE & WIFI_MP_STATE) {
			txcfg->hdr_len = WLAN_HDR_A3_LEN;
			txcfg->frg_num = 1;
			if (IS_MCAST(pskb->data))
				txcfg->need_ack = 0;
			return TRUE;
		}
#endif

#ifdef WDS
		if (txcfg->wdsIdx >= 0) {
			txcfg->hdr_len = WLAN_HDR_A4_LEN;
			pstat = get_stainfo(priv, priv->pmib->dot11WdsInfo.entry[txcfg->wdsIdx].macAddr);
			if (pstat == NULL) {
				DEBUG_ERR("TX DROP: %s: get_stainfo() for wds failed [%d]!\n", (char *)__FUNCTION__, txcfg->wdsIdx);
				return FALSE;
			}

			txcfg->privacy = priv->pmib->dot11WdsInfo.wdsPrivacy;
			switch (txcfg->privacy) {
				case _WEP_40_PRIVACY_:
				case _WEP_104_PRIVACY_:
					txcfg->iv = 4;
					txcfg->icv = 4;
					break;
				case _TKIP_PRIVACY_:
					txcfg->iv = 8;
					txcfg->icv = 4;
					txcfg->mic = 0;
					txcfg->fr_len += 8; // for Michael padding
					break;
				case _CCMP_PRIVACY_:
					txcfg->iv = 8;
					txcfg->icv = 0;
					txcfg->mic = 8;
					break;
			}
			txcfg->frg_num = 1;
			if (txcfg->aggre_en < FG_AGGRE_MSDU_FIRST) {
				priority = get_skb_priority(priv, (struct sk_buff *)txcfg->pframe, pstat);
#ifdef RTL_MANUAL_EDCA
				txcfg->q_num = PRI_TO_QNUM(priv, priority);
#else
				PRI_TO_QNUM(priority, txcfg->q_num, priv->pmib->dot11OperationEntry.wifi_specific);
#endif
			}

			txcfg->tx_rate = get_tx_rate(priv, pstat);
#ifndef TX_LOWESTRATE
			txcfg->lowest_tx_rate = get_lowest_tx_rate(priv, pstat, txcfg->tx_rate);
#endif
			if (priv->pmib->dot11WdsInfo.entry[pstat->wds_idx].txRate)
				txcfg->fixed_rate = 1;
			txcfg->need_ack = 1;
			txcfg->pstat = pstat;
#ifdef WIFI_WMM
			if ((pstat) && (QOS_ENABLE) && (pstat->QosEnabled) && (is_qos_data(txcfg->phdr)))
				txcfg->hdr_len = WLAN_HDR_A4_QOS_LEN;
#endif

			if (txcfg->aggre_en == 0) {
				if ((pstat->aggre_mthd == AGGRE_MTHD_MPDU) && is_MCS_rate(txcfg->tx_rate))
					txcfg->aggre_en = FG_AGGRE_MPDU;
			}

			return TRUE;
		}
#endif

#ifdef WIFI_WMM
		if (OPMODE & WIFI_AP_STATE) {
#ifdef MCAST2UI_REFINE
			pstat = get_stainfo(priv, &pskb->cb[10]);
#else
			pstat = get_stainfo(priv, pskb->data);
#endif
#ifdef A4_STA
            if (pstat == NULL) {
                pstat = txcfg->pstat;
            }
#endif
		}
		else if (OPMODE & WIFI_STATION_STATE)
			pstat = get_stainfo(priv, BSSID);
		else if (OPMODE & WIFI_ADHOC_STATE)
				pstat = get_stainfo(priv, pskb->data);

		if ((pstat) && (QOS_ENABLE) && (pstat->QosEnabled) && (is_qos_data(txcfg->phdr))) {
			txcfg->hdr_len = WLAN_HDR_A3_QOS_LEN;
		}
		else
#endif
		{
			txcfg->hdr_len = WLAN_HDR_A3_LEN;
		}

#ifdef CONFIG_RTK_MESH
		if(txcfg->is_11s)
		{
			txcfg->hdr_len = WLAN_HDR_A4_QOS_LEN ;
			da = txcfg->nhop_11s;
		}
		else
#endif

#ifdef MCAST2UI_REFINE
		da = &pskb->cb[10];
#else
		da = pskb->data;
#endif

#ifdef A4_STA
        if(priv->pshare->rf_ft_var.a4_enable && get_tofr_ds(txcfg->phdr) == 3) {
            if(pstat)
                da = pstat->hwaddr;
            txcfg->hdr_len += WLAN_ADDR_LEN;
        }
#endif

		//check if da is associated, if not, just drop and return false
		if (!IS_MCAST(da)
#ifdef CLIENT_MODE
			|| (OPMODE & WIFI_STATION_STATE)
#endif
			)
		{
#ifdef CLIENT_MODE
			if (OPMODE & WIFI_STATION_STATE)
				pstat = get_stainfo(priv, BSSID);
			else
#endif
			{
				pstat = get_stainfo(priv, da);
			}

			if ((pstat == NULL) || (!(pstat->state & WIFI_ASOC_STATE)))
			{
				DEBUG_ERR("TX DROP: Non asoc tx request!\n");
				return FALSE;
			}

			protocol = ntohs(*((UINT16 *)((UINT8 *)pskb->data + ETH_ALEN*2)));

			if ((((protocol == 0x888E) && ((GET_UNICAST_ENCRYP_KEYLEN == 0)
#ifdef WIFI_SIMPLE_CONFIG
				|| (pstat->state & WIFI_WPS_JOIN)
#endif
				))
#ifdef CONFIG_RTL_WAPI_SUPPORT
				 || (protocol == ETH_P_WAPI)
#endif

#ifdef BEAMFORMING_SUPPORT
				|| (txcfg->ndpa) 
#endif
				))
				txcfg->privacy = 0;
			else
				txcfg->privacy = get_privacy(priv, pstat, &txcfg->iv, &txcfg->icv, &txcfg->mic);

			if ((OPMODE & WIFI_AP_STATE) && !IS_MCAST(da) && pskb && (isDHCPpkt(pskb) == TRUE))
				is_dhcp++;

			if ((protocol == 0x888E)
#ifdef CONFIG_RTL_WAPI_SUPPORT
				||(protocol == ETH_P_WAPI)
#endif
			|| is_dhcp) {
				// use basic rate to send EAP packet for sure
				txcfg->tx_rate = find_rate(priv, NULL, 0, 1);
#ifndef TX_LOWESTRATE
				txcfg->lowest_tx_rate = txcfg->tx_rate;
#endif				
				txcfg->fixed_rate = 1;
			} else {
				txcfg->tx_rate = get_tx_rate(priv, pstat);
#ifndef TX_LOWESTRATE				
				txcfg->lowest_tx_rate = get_lowest_tx_rate(priv, pstat, txcfg->tx_rate);
#endif				
				if (!is_auto_rate(priv, pstat) &&
					!(should_restrict_Nrate(priv, pstat) && is_fixedMCSTxRate(priv, pstat)))
                    {            
#ifdef RTK_AC_SUPPORT
		        		if(! is_fixedVHTTxRate(priv, pstat) || (pstat->vht_cap_len))
#endif
                        {
		    	    		txcfg->fixed_rate = 1;
                        }
                   }
			}

			if (txcfg->aggre_en < FG_AGGRE_MSDU_FIRST) {
#ifdef CONFIG_RTK_MESH
				priority = get_skb_priority3(priv, (struct sk_buff *)txcfg->pframe, txcfg->is_11s, pstat);
#else
				priority = get_skb_priority(priv, (struct sk_buff *)txcfg->pframe, pstat);
#endif
#ifdef RTL_MANUAL_EDCA
				txcfg->q_num = PRI_TO_QNUM(priv, priority);
#else
				PRI_TO_QNUM(priority, txcfg->q_num, priv->pmib->dot11OperationEntry.wifi_specific);
#endif
			}

#ifdef CONFIG_RTL_WAPI_SUPPORT
			if (protocol==ETH_P_WAPI)
			{
				txcfg->q_num = MGNT_QUEUE;
			}
#endif

			if (txcfg->aggre_en == 0
#ifdef SUPPORT_TX_MCAST2UNI
					&& (priv->pshare->rf_ft_var.mc2u_disable || (pskb->cb[2] != (char)0xff))
#endif
				) {
				if ((pstat->aggre_mthd == AGGRE_MTHD_MPDU) &&
				/*	is_MCS_rate(txcfg->tx_rate) &&*/ (protocol != 0x888E)
#ifdef CONFIG_RTL_WAPI_SUPPORT
					&& (protocol != ETH_P_WAPI)
#endif
					&& !is_dhcp)
					txcfg->aggre_en = FG_AGGRE_MPDU;
			}

#ifdef WMM_DSCP_C42
			if((IS_HAL_CHIP(priv)) && (txcfg->q_num==BE_QUEUE) && priv->pshare->iot_mode_VO_exist) {
				txcfg->q_num++;
			}
#endif
			if(
#ifdef RTK_AC_SUPPORT //FOR_VHT5G_PF
			( txcfg->pstat && 
			((txcfg->pstat->aggre_mthd == AGGRE_MTHD_MPDU_AMSDU) || (txcfg->pstat->aggre_mthd == AGGRE_MTHD_MPDU))
			&& txcfg->aggre_en ) ||
#endif
			 (txcfg->aggre_en >= FG_AGGRE_MPDU && txcfg->aggre_en <= FG_AGGRE_MPDU_BUFFER_LAST) 
			)
			{
				//panic_printk("%s %d pstat->ADDBA_ready[priority]=%d, priority=%d\n",__func__,__LINE__,pstat->ADDBA_ready[priority],priority);
				if (!pstat->ADDBA_ready[priority]) {
					if ((pstat->ADDBA_req_num[priority] < 5) && !pstat->ADDBA_sent[priority]) {
						pstat->ADDBA_req_num[priority]++;
						issue_ADDBAreq(priv, pstat, priority);
						pstat->ADDBA_sent[priority]++;
					}
				}
			}
		}
		else
		{
			// if group key not set yet, don't let unencrypted multicast go to air
			if (priv->pmib->dot11GroupKeysTable.dot11Privacy) {
				if (GET_GROUP_ENCRYP_KEYLEN == 0) {
					DEBUG_ERR("TX DROP: group key not set yet!\n");
					return FALSE;
				}
			}

			txcfg->privacy = get_mcast_privacy(priv, &txcfg->iv, &txcfg->icv, &txcfg->mic);
#if defined(CONFIG_WLAN_HAL)
			if (IS_HAL_CHIP(priv)) {
				if (OPMODE & WIFI_AP_STATE) {
#ifdef MBSSID
					if (IS_ROOT_INTERFACE(priv))
						txcfg->q_num = HIGH_QUEUE;
					else if (priv->vap_init_seq == 1)
						txcfg->q_num = HIGH_QUEUE1;
					else if (priv->vap_init_seq == 2)
						txcfg->q_num = HIGH_QUEUE2;
					else if (priv->vap_init_seq == 3)
						txcfg->q_num = HIGH_QUEUE3;
					else if (priv->vap_init_seq == 4)
						txcfg->q_num = HIGH_QUEUE4;
					else if (priv->vap_init_seq == 5)
						txcfg->q_num = HIGH_QUEUE5;
					else if (priv->vap_init_seq == 6)
						txcfg->q_num = HIGH_QUEUE6;
					else if (priv->vap_init_seq == 7)
						txcfg->q_num = HIGH_QUEUE7;
#else
					txcfg->q_num = HIGH_QUEUE;
#endif
					SetMData(txcfg->phdr);
				} else {
					// to do: sta mode?
					txcfg->q_num = BE_QUEUE;
					pskb->cb[1] = 0;
				}
			} else if (CONFIG_WLAN_NOT_HAL_EXIST)
#endif
			{//not HAL
				if ((OPMODE & WIFI_AP_STATE) && priv->pkt_in_dtimQ) {
					txcfg->q_num = MCAST_QNUM;
					SetMData(txcfg->phdr);
				}
				else {
					txcfg->q_num = BE_QUEUE;
					pskb->cb[1] = 0;
				}
			}

			if ((*da) == 0xff)	// broadcast
				txcfg->tx_rate = find_rate(priv, NULL, 0, 1);
			else {				// multicast
				if (priv->pmib->dot11StationConfigEntry.lowestMlcstRate)
					txcfg->tx_rate = get_rate_from_bit_value(priv->pmib->dot11StationConfigEntry.lowestMlcstRate);
				else
					txcfg->tx_rate = find_rate(priv, NULL, 1, 1);
			}

#ifdef _11s_TEST_MODE_
			mesh_debug_tx4(priv, txcfg);
#endif

#ifndef TX_LOWESTRATE
			txcfg->lowest_tx_rate = txcfg->tx_rate;
#endif			
			txcfg->fixed_rate = 1;
		}
	}
#ifdef _11s_TEST_MODE_	/*---11s mgt frame---*/
	else if (txcfg->is_11s)
		mesh_debug_tx6(priv, txcfg);
#endif

	if (!da)
	{
		// This is non data frame, no need to frag.
#ifdef CONFIG_RTK_MESH
		if(txcfg->is_11s)
			da = GetAddr1Ptr(txcfg->phdr);
		else
#endif
			da = get_da((unsigned char *) (txcfg->phdr));

		txcfg->frg_num = 1;

		if (IS_MCAST(da))
			txcfg->need_ack = 0;
		else
			txcfg->need_ack = 1;

		if (GetPrivacy(txcfg->phdr))
		{		
#ifdef CONFIG_IEEE80211W 
			if(txcfg->isPMF) {
				txcfg->privacy = _CCMP_PRIVACY_;

					txcfg->iv = 8;
					txcfg->icv = 0;
					txcfg->mic = 8;
			} else {			
#endif
			// only auth with legacy wep...
			txcfg->iv = 4;
			txcfg->icv = 4;
			txcfg->privacy = priv->pmib->dot1180211AuthEntry.dot11PrivacyAlgrthm;
#ifdef CONFIG_IEEE80211W 
			}
#endif
		}

#ifdef DRVMAC_LB
		if (GetFrameType(txcfg->phdr) == WIFI_MGT_TYPE)
#endif
		if (txcfg->fr_len != 0)	//for mgt frame
			txcfg->hdr_len += WLAN_HDR_A3_LEN;
	}

#ifdef CLIENT_MODE
	if ((OPMODE & WIFI_AP_STATE) || (OPMODE & WIFI_ADHOC_STATE))
#else
	if (OPMODE & WIFI_AP_STATE)
#endif
	{
		if (IS_MCAST(da))
		{
			txcfg->frg_num = 1;
			txcfg->need_ack = 0;
			txcfg->rts_thrshld = 10000;
		}
		else if(!(txcfg->phdr && (GetFrameType(txcfg->phdr) == WIFI_MGT_TYPE) && (GetFrameSubType((unsigned char *) (txcfg->phdr))>>4 == 5)))	//  exclude probe rsp
		{
			pstat = get_stainfo(priv, da);
			txcfg->pstat = pstat;
		}
	}
#ifdef CLIENT_MODE
	else if (OPMODE & WIFI_STATION_STATE)
	{
		if ((txcfg->fr_type == _SKB_FRAME_TYPE_) ||							// skb frame
				!memcmp(GetAddr1Ptr(txcfg->phdr), BSSID, MACADDRLEN)) {		// mgt frame to AP
			pstat = get_stainfo(priv, BSSID);
			txcfg->pstat = pstat;
		}
	}
#endif

#ifdef BEAMFORMING_SUPPORT
	if((priv->pmib->dot11RFEntry.txbf == 1) && (pstat) &&
		((pstat->ht_cap_len && (pstat->ht_cap_buf.txbf_cap)) 
#ifdef RTK_AC_SUPPORT		
		||(pstat->vht_cap_len && (cpu_to_le32(pstat->vht_cap_buf.vht_cap_info) & BIT(SU_BFEE_S)))
#endif		
		))
		Beamforming_GidPAid(priv, pstat);
#endif
	if (txcfg->privacy == _TKIP_PRIVACY_)
		txcfg->fr_len += 8;	// for Michael padding.

	txcfg->frag_thrshld -= (txcfg->mic + txcfg->iv + txcfg->icv + txcfg->hdr_len);

	if (txcfg->frg_num == 0)
	{
		if (txcfg->aggre_en > 0)
			txcfg->frg_num = 1;
		else {
			// how many mpdu we need...
			int llc;

			if ((ntohs(*((UINT16 *)((UINT8 *)pskb->data + ETH_ALEN*2))) + WLAN_ETHHDR_LEN) > WLAN_MAX_ETHFRM_LEN)
				llc = sizeof(struct llc_snap);
			else
				llc = 0;

			txcfg->frg_num = (txcfg->fr_len + llc) / txcfg->frag_thrshld;
			if (((txcfg->fr_len + llc) % txcfg->frag_thrshld) != 0)
				txcfg->frg_num += 1;
		}
	}

#ifdef TX_SCATTER
	if (pskb && pskb->list_num > 0 && txcfg->frg_num > 1) {
		struct sk_buff *newskb = copy_skb(pskb);
		if (newskb == NULL) {
			DEBUG_ERR("TX DROP: Can't copy the skb for list buffer in frag!\n");
			return FALSE;
		}
		dev_kfree_skb_any(pskb);
		txcfg->pframe = (void *)newskb;
	}
#endif

	return TRUE;
}
#endif // CONFIG_PCI_HCI

/*-------------------------------------------------------------------------------
tx flow:
	Please refer to design spec for detail flow

rtl8192cd_firetx: check if hw desc is available for tx
				hdr_len, iv,icv, tx_rate info are available

signin_txdesc: fillin the desc and txpoll is necessary
--------------------------------------------------------------------------------*/
int __rtl8192cd_firetx(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
	int					*tx_head=NULL, *tx_tail=NULL, q_num;
	unsigned int		val32=0, is_dhcp=0;
	struct sk_buff		*pskb=NULL;
	struct llc_snap		*pllc_snap;
	struct wlan_ethhdr_t	*pethhdr=NULL;
#ifdef SUPPORT_TX_AMSDU
	struct wlan_ethhdr_t	*pmsdu_hdr;
	struct wlan_ethhdr_t  pethhdr_data;
#endif
	struct rtl8192cd_hw	*phw = GET_HW(priv);
#ifdef CONFIG_WLAN_HAL
#ifndef TRXBD_CACHABLE_REGION
//	PHCI_TX_DMA_MANAGER_88XX	ptx_dma;
#endif	
#endif

#ifdef RESERVE_TXDESC_FOR_EACH_IF
	unsigned int		txdesc_need = 1, ret_txdesc = 0;
#endif
	unsigned long		x;
    unsigned char *da;
#if defined(CONFIG_RTL_WAPI_SUPPORT)
	struct wlan_ethhdr_t	ethhdr;
	pethhdr = &ethhdr;
#endif

#ifdef CONFIG_PCI_HCI
	/*---frag_thrshld setting---plus tune---0115*/
#ifdef	WDS
	if (txcfg->wdsIdx >= 0){
		txcfg->frag_thrshld = 2346; // if wds, disable fragment
	}else
#endif
#ifdef CONFIG_RTK_MESH
	if(txcfg->is_11s){
		txcfg->frag_thrshld = 2346; // if Mesh case, disable fragment
	}else
#endif
	{
		txcfg->frag_thrshld = FRAGTHRSLD - _CRCLNG_;
	}
	/*---frag_thrshld setting---end*/

	txcfg->rts_thrshld  = RTSTHRSLD;
	txcfg->frg_num = 0;

#ifdef DFS
	if (!priv->pmib->dot11DFSEntry.disable_DFS &&
		GET_ROOT(priv)->pmib->dot11DFSEntry.disable_tx
		&& (priv->site_survey)
		&& (priv->site_survey->hidden_ap_found != HIDE_AP_FOUND_DO_ACTIVE_SSAN)) {
		priv->ext_stats.tx_drops++;
		DEBUG_ERR("TX DROP: DFS probation period\n");

		if (txcfg->fr_type == _SKB_FRAME_TYPE_) {
			rtl_kfree_skb(priv, (struct sk_buff *)txcfg->pframe, _SKB_TX_);
			release_wlanllchdr_to_poll(priv, txcfg->phdr);
		} else if (txcfg->fr_type == _PRE_ALLOCMEM_) {
			release_mgtbuf_to_poll(priv, txcfg->pframe);
			release_wlanhdr_to_poll(priv, txcfg->phdr);
		}

		return SUCCESS;
	}
#endif

	if ((check_txdesc(priv, txcfg)) == FALSE) // this will only happen in errorous forwarding
	{
		priv->ext_stats.tx_drops++;
        DEBUG_ERR("TX DROP: check_txdesc Fail \n");
		// Don't need to print "TX DROP", already print in check_txdesc()
		if (txcfg->fr_type == _SKB_FRAME_TYPE_) {
			rtl_kfree_skb(priv, (struct sk_buff *)txcfg->pframe, _SKB_TX_);
			release_wlanllchdr_to_poll(priv, txcfg->phdr);
		}
		else if (txcfg->fr_type == _PRE_ALLOCMEM_) {
			release_mgtbuf_to_poll(priv, txcfg->pframe);
			release_wlanhdr_to_poll(priv, txcfg->phdr);
		}
		return SUCCESS;
	}

	if (txcfg->aggre_en > 0)
		txcfg->frag_thrshld = 2346;

	if (txcfg->aggre_en < FG_AGGRE_MSDU_FIRST) {
		// now we are going to calculate how many hw desc we should have before tx...
		// wlan_hdr(including iv and llc) will occupy one desc, payload will occupy one, and
		// icv/mic will occupy the third desc if swcrypto is utilized.
		q_num = txcfg->q_num;
#ifdef CONFIG_WLAN_HAL
		if (IS_HAL_CHIP(priv)) {
	        val32 = txcfg->frg_num;
		} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif
		{//not HAL
			{
				tx_head = get_txhead_addr(phw, q_num);
				tx_tail = get_txtail_addr(phw, q_num);
			}
		

			if (txcfg->privacy)
			{
				if (
	#if defined(CONFIG_RTL_WAPI_SUPPORT)
						(_WAPI_SMS4_ != txcfg->privacy) &&
	#endif
#ifdef CONFIG_IEEE80211W
			UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))
#else
			UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))
#endif	

					val32 = txcfg->frg_num * 3;  //extra one is for ICV padding.
				else
					val32 = txcfg->frg_num * 2;
			}
			else {
				val32 = txcfg->frg_num * 2;
			}

#ifdef TX_SCATTER
			if (txcfg->fr_type == _SKB_FRAME_TYPE_ &&
					((struct sk_buff *)txcfg->pframe)->key > 0 &&  
						((struct sk_buff *)txcfg->pframe)->list_num > 1)
				val32 += ((struct sk_buff *)txcfg->pframe)->list_num -1;
#endif
		}

#ifdef RESERVE_TXDESC_FOR_EACH_IF
		if(GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc) {
			if (check_txdesc_dynamic_mechanism(priv, q_num, val32)) {
#ifdef USE_TXQUEUE
				if (GET_ROOT(priv)->pmib->miscEntry.use_txq && priv->pshare->iot_mode_enable) {
					//SMP_LOCK_XMIT(x);
					if (priv->pshare->txq_isr) {
						append_skb_to_txq_head(&priv->pshare->txq_list[q_num], priv, txcfg->pframe, priv->dev, &priv->pshare->txq_pool);
						priv->pshare->txq_stop = 1;
					} else {
						append_skb_to_txq_tail(&priv->pshare->txq_list[q_num], priv, txcfg->pframe, priv->dev, &priv->pshare->txq_pool);
					}
					if (txcfg->phdr)
						release_wlanllchdr_to_poll(priv, txcfg->phdr);
					priv->use_txq_cnt[q_num]++;
					priv->pshare->txq_check = 1;
					//SMP_UNLOCK_XMIT(x);
					return SUCCESS;					
				}
				else
#endif
				{
					DEBUG_WARN("%d hw Queue desc exceed available count: used:%d\n", q_num,priv->use_txdesc_cnt[q_num]);
					return CONGESTED;
				}
			}
			txdesc_need = val32;
		} else
#endif
		{
#ifdef CONFIG_WLAN_HAL
#ifndef TRXBD_CACHABLE_REGION
//      	if (IS_HAL_CHIP(priv)) {
//	        ptx_dma = (PHCI_TX_DMA_MANAGER_88XX)(_GET_HAL_DATA(priv)->PTxDMA88XX);
//		}
#endif		
#endif		
		//SMP_LOCK_XMIT(x);
		if (
#ifdef CONFIG_WLAN_HAL
		IS_HAL_CHIP(priv)? (compareAvailableTXBD(priv, val32+1, q_num, 2))  :
#endif
		((val32 + 2) > CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC)) //per mpdu, we need 2 desc...
		){
			 // 2 is for spare...
#ifdef USE_TXQUEUE
			if (GET_ROOT(priv)->pmib->miscEntry.use_txq && priv->pshare->iot_mode_enable) {
				if (priv->pshare->txq_isr) {
					append_skb_to_txq_head(&priv->pshare->txq_list[q_num], priv, txcfg->pframe, priv->dev, &priv->pshare->txq_pool);
					priv->pshare->txq_stop = 1;
				} else {
					append_skb_to_txq_tail(&priv->pshare->txq_list[q_num], priv, txcfg->pframe, priv->dev, &priv->pshare->txq_pool);
				}
				if (txcfg->phdr)
					release_wlanllchdr_to_poll(priv, txcfg->phdr);
				priv->pshare->txq_check = 1;
				//SMP_UNLOCK_XMIT(x);
				return SUCCESS;
			}
			else
#endif
			{
				// Marked by SC, may cause crash.
				//DEBUG_ERR("%d hw Queue desc not available! head=%d, tail=%d request %d\n",q_num,*tx_head,*tx_tail,val32);
				//SMP_UNLOCK_XMIT(x);
				return CONGESTED;
			}
		}
		//SMP_UNLOCK_XMIT(x);
	}
	} else {
		  q_num = txcfg->q_num;
	}

	// then we have to check if wlan-hdr is available for usage...
	// actually, the checking can be void

	/* ----------
				Actually, I believe the check below is redundant.
				Since at this moment, desc is available, hdr/icv/
				should be enough.
													--------------*/
	val32 = txcfg->frg_num;

	if (val32 >= priv->pshare->pwlan_hdr_poll->count)
	{
		DEBUG_ERR("%d hw Queue tx without enough wlan_hdr\n", q_num);
		return CONGESTED;
	}
#endif // CONFIG_PCI_HCI

#if defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
	switch( rtw_xmit_decision(priv, txcfg) ) {
	case XMIT_DECISION_ENQUEUE:
		return SUCCESS;
	case XMIT_DECISION_STOP:
		return CONGESTED;
	case XMIT_DECISION_CONTINUE:
		break;
	}
	
	if (update_txinsn_stage2(priv, txcfg) == FALSE) {
		return CONGESTED;
	}
	
	q_num = txcfg->q_num;
#endif // CONFIG_USB_HCI || CONFIG_SDIO_HCI

	// then we have to check if wlan_snapllc_hdrQ is enough for use
	// for each msdu, we need wlan_snapllc_hdrQ for maximum

	if (txcfg->fr_type == _SKB_FRAME_TYPE_)
	{
		pskb = (struct sk_buff *)txcfg->pframe;

#ifdef CONFIG_PCI_HCI
		if ((OPMODE & WIFI_AP_STATE) && pskb && (isDHCPpkt(pskb) == TRUE))
			is_dhcp++;
#endif

#ifdef SUPPORT_TX_AMSDU
		// for AMSDU
		if (txcfg->aggre_en >= FG_AGGRE_MSDU_FIRST) {
			unsigned short usLen;
			int msdu_len;

			memcpy(&pethhdr_data, pskb->data, sizeof(struct wlan_ethhdr_t));
			pethhdr = &pethhdr_data;
			msdu_len = pskb->len - WLAN_ETHHDR_LEN;
			if ((ntohs(pethhdr->type) + WLAN_ETHHDR_LEN) > WLAN_MAX_ETHFRM_LEN) {
				if (skb_headroom(pskb) < sizeof(struct llc_snap)) {
					struct sk_buff *skb2 = dev_alloc_skb(pskb->len);
					if (skb2 == NULL) {
						printk("%s: %s, dev_alloc_skb() failed!\n", priv->dev->name, __FUNCTION__);
						rtl_kfree_skb(priv, pskb, _SKB_TX_);
						if(txcfg->aggre_en == FG_AGGRE_MSDU_FIRST)
							release_wlanllchdr_to_poll(priv, txcfg->phdr);
						return 0;
					}
					memcpy(skb_put(skb2, pskb->len), pskb->data, pskb->len);
					dev_kfree_skb_any(pskb);
					pskb = skb2;
					txcfg->pframe = (void *)pskb;
				}
				skb_push(pskb, sizeof(struct llc_snap));
			}
			pmsdu_hdr = (struct wlan_ethhdr_t *)pskb->data;

			memcpy(pmsdu_hdr, pethhdr, 12);
			if ((ntohs(pethhdr->type) + WLAN_ETHHDR_LEN) > WLAN_MAX_ETHFRM_LEN) {
				usLen = (unsigned short)(msdu_len + sizeof(struct llc_snap));
				pmsdu_hdr->type = htons(usLen);
				eth_2_llc(pethhdr, (struct llc_snap *)(((unsigned long)pmsdu_hdr)+sizeof(struct wlan_ethhdr_t)));
			}
			else {
				usLen = (unsigned short)msdu_len;
				pmsdu_hdr->type = htons(usLen);
			}

			if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST)
				eth2_2_wlanhdr(priv, pethhdr, txcfg);

			txcfg->llc = 0;
			pllc_snap = NULL;

			if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST || txcfg->aggre_en == FG_AGGRE_MSDU_MIDDLE) {
				if ((usLen+14) % 4)
					skb_put(pskb, 4-((usLen+14)%4));
			}
			txcfg->fr_len = pskb->len;
		}
		else 
#endif /* SUPPORT_TX_AMSDU */			
		{	// not A-MSDU
#if defined(CONFIG_SDIO_HCI) && defined(TX_SCATTER)
			// in case 1st buffer len is 14, we get ether header pointer first and then ajust the skb
			pethhdr = (struct wlan_ethhdr_t *)(pskb->data);
			// now, we should adjust the skb ...
			skb_pull(pskb, WLAN_ETHHDR_LEN);
#else
			// now, we should adjust the skb ...
			skb_pull(pskb, WLAN_ETHHDR_LEN);
#ifdef CONFIG_RTK_MESH
            pethhdr = &priv->ethhdr;
			memcpy(pethhdr, pskb->data - sizeof(struct wlan_ethhdr_t), sizeof(struct wlan_ethhdr_t));
#else
			pethhdr = (struct wlan_ethhdr_t *)(pskb->data - sizeof(struct wlan_ethhdr_t));
#endif
#endif
			pllc_snap = (struct llc_snap *)((UINT8 *)(txcfg->phdr) + txcfg->hdr_len + txcfg->iv);

			if ((ntohs(pethhdr->type) + WLAN_ETHHDR_LEN) > WLAN_MAX_ETHFRM_LEN) {
				eth_2_llc(pethhdr, pllc_snap);
				txcfg->llc = sizeof(struct llc_snap);
			}
			else
			{
				pllc_snap = NULL;
			}

			eth2_2_wlanhdr(priv, pethhdr, txcfg);

#ifdef CONFIG_PCI_HCI
#ifdef TX_SCATTER
			if (pskb->len == 0 && pskb->list_num > 1) {
				pskb->list_idx++;
				skb_assign_buf(pskb, pskb->list_buf[pskb->list_idx].buf, pskb->list_buf[pskb->list_idx].len);
				pskb->len = pskb->list_buf[pskb->list_idx].len;
			}
#endif
#endif

#ifdef CONFIG_RTK_MESH
			if(txcfg->is_11s&1 && GetFrameSubType(txcfg->phdr) == WIFI_11S_MESH)
			{
				const short meshhdrlen= (txcfg->mesh_header.mesh_flag & 0x01) ? 16 : 4;
				if (skb_cloned(pskb))
				{
					struct sk_buff	*newskb = skb_copy(pskb, GFP_ATOMIC);
					rtl_kfree_skb(priv, pskb, _SKB_TX_);
					if (newskb == NULL) {
						priv->ext_stats.tx_drops++;
						release_wlanllchdr_to_poll(priv, txcfg->phdr);
						DEBUG_ERR("TX DROP: Can't copy the skb!\n");
						return SUCCESS;
					}
					txcfg->pframe = pskb = newskb;
#ifdef ENABLE_RTL_SKB_STATS
					rtl_atomic_inc(&priv->rtl_tx_skb_cnt);
#endif
				}
				memcpy(skb_push(pskb,meshhdrlen), &(txcfg->mesh_header), meshhdrlen);
				txcfg->fr_len += meshhdrlen;
			}
#endif // CONFIG_RTK_MESH
		}

		// TODO: modify as when skb is not bigger enough, take ICV from local pool
#ifdef CONFIG_RTL_WAPI_SUPPORT
		if (txcfg->privacy == _WAPI_SMS4_)
		{
			if ((pskb->tail + SMS4_MIC_LEN) > pskb->end)
			{
				struct sk_buff *pnewskb;
				
				pnewskb = skb_copy_expand(pskb, skb_headroom(pskb), SMS4_MIC_LEN, GFP_ATOMIC);
				if (NULL == pnewskb) {
					priv->ext_stats.tx_drops++;
					DEBUG_ERR("TX DROP: an over size skb!\n");
					rtl_kfree_skb(priv, (struct sk_buff *)txcfg->pframe, _SKB_TX_);
					release_wlanllchdr_to_poll(priv, txcfg->phdr);
					return SUCCESS;
				}
				
				dev_kfree_skb_any(pskb);
				pskb = pnewskb;
				txcfg->pframe = pskb;
			}
			memcpy(&ethhdr, pethhdr, sizeof(struct wlan_ethhdr_t));
#ifdef MCAST2UI_REFINE
                        memcpy(&ethhdr.daddr, &pskb->cb[10], 6);
#endif
			pethhdr = &ethhdr;
		} else
#endif

		if (txcfg->privacy == _TKIP_PRIVACY_)
		{
			if ((pskb->tail + 8) > pskb->end)
			{
#if defined(CONFIG_PCI_HCI)
				priv->ext_stats.tx_drops++;
				DEBUG_ERR("TX DROP: an over size skb!\n");
				rtl_kfree_skb(priv, (struct sk_buff *)txcfg->pframe, _SKB_TX_);
				release_wlanllchdr_to_poll(priv, txcfg->phdr);
				return SUCCESS;
				
#elif defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
				struct sk_buff *pnewskb;
				
				pnewskb = skb_copy_expand(pskb, skb_headroom(pskb), 8, GFP_ATOMIC);
				if (NULL == pnewskb) {
					priv->ext_stats.tx_drops++;
					DEBUG_ERR("TX DROP: an over size skb!\n");
					rtl_kfree_skb(priv, (struct sk_buff *)txcfg->pframe, _SKB_TX_);
					release_wlanllchdr_to_poll(priv, txcfg->phdr);
					return SUCCESS;
				}
				
				dev_kfree_skb_any(pskb);
				pskb = pnewskb;
				txcfg->pframe = pskb;
#endif
			}

            da = pethhdr->daddr;
#ifdef MCAST2UI_REFINE
            memcpy(pethhdr->daddr, &pskb->cb[10], 6);
#endif

#ifdef A4_STA
            if(txcfg->pstat && (txcfg->pstat->state & WIFI_A4_STA)) {
                da = GetAddr3Ptr(txcfg->phdr);
            }
#endif

#ifdef WIFI_WMM
			if ((tkip_mic_padding(priv, da, pethhdr->saddr, ((QOS_ENABLE) && (txcfg->pstat) && (txcfg->pstat->QosEnabled))?pskb->cb[1]:0, (UINT8 *)pllc_snap,
					pskb, txcfg)) == FALSE)
#else
			if ((tkip_mic_padding(priv, da, pethhdr->saddr, 0, (UINT8 *)pllc_snap,
					pskb, txcfg)) == FALSE)
#endif
			{
				priv->ext_stats.tx_drops++;
				DEBUG_ERR("TX DROP: Tkip mic padding fail!\n");
				rtl_kfree_skb(priv, pskb, _SKB_TX_);
				release_wlanllchdr_to_poll(priv, txcfg->phdr);
				return SUCCESS;
			}
			if ((txcfg->aggre_en < FG_AGGRE_MSDU_FIRST) || (txcfg->aggre_en == FG_AGGRE_MSDU_LAST))
				skb_put((struct sk_buff *)txcfg->pframe, 8);
		}
	}

	if (txcfg->privacy && txcfg->aggre_en <= FG_AGGRE_MSDU_FIRST)
		SetPrivacy(txcfg->phdr);

	// log tx statistics...
#ifdef CONFIG_RTL8672
	tx_sum_up(priv, txcfg->pstat, txcfg->fr_len+txcfg->hdr_len+txcfg->iv+txcfg->llc+txcfg->mic+txcfg->icv, txcfg);
#else
	tx_sum_up(priv, txcfg->pstat, txcfg->fr_len+txcfg->hdr_len+txcfg->iv+txcfg->llc+txcfg->mic+txcfg->icv);
#endif
	SNMP_MIB_INC(dot11TransmittedFragmentCount, 1);

	// for SW LED
	if (txcfg->aggre_en > FG_AGGRE_MSDU_FIRST || GetFrameType(txcfg->phdr) == WIFI_DATA_TYPE) {
		priv->pshare->LED_tx_cnt++;
	} else {
		if (priv->pshare->LED_cnt_mgn_pkt)
			priv->pshare->LED_tx_cnt++;
	}

#ifdef PCIE_POWER_SAVING
	PCIeWakeUp(priv, (POWER_DOWN_T0));
#endif

	SAVE_INT_AND_CLI(x);
	//SMP_LOCK_XMIT(x);
	
#ifdef SUPPORT_TX_AMSDU
	if (txcfg->aggre_en == FG_AGGRE_MSDU_MIDDLE || txcfg->aggre_en == FG_AGGRE_MSDU_LAST) {
#ifdef RESERVE_TXDESC_FOR_EACH_IF
		if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc) {
			priv->use_txdesc_cnt[q_num] += 1;
			if (rtl8192cd_signin_txdesc_amsdu(priv, txcfg))
				priv->use_txdesc_cnt[q_num] -= 1;
		} else
#endif
		{
		rtl8192cd_signin_txdesc_amsdu(priv, txcfg);
		}
	} else
#endif
	{
#ifdef _11s_TEST_MODE_
		signin_txdesc_galileo(priv, txcfg);
#else
#ifdef RESERVE_TXDESC_FOR_EACH_IF
		if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc) {
			priv->use_txdesc_cnt[q_num] += txdesc_need;
			if (
#ifdef CONFIG_WLAN_HAL			
			IS_HAL_CHIP(priv) ? ((ret_txdesc = rtl88XX_signin_txdesc(priv, txcfg)) != 0 ):
#endif			
			((ret_txdesc = rtl8192cd_signin_txdesc(priv, txcfg)) != 0 ) 
			){
				if (
#if defined(CONFIG_RTL_WAPI_SUPPORT)
						(_WAPI_SMS4_ != txcfg->privacy) &&
#endif
#ifdef CONFIG_IEEE80211W
				UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))
#else
				UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))
											
#endif	
					priv->use_txdesc_cnt[q_num] -= 3 * ret_txdesc;
				else
					priv->use_txdesc_cnt[q_num] -= 2 * ret_txdesc;
			}
		} else
#endif
		{
#if defined(CONFIG_PCI_HCI)
#ifdef CONFIG_WLAN_HAL
			if (IS_HAL_CHIP(priv)) {
				// TODO: these if conditions are all the same as the following code, it should be optimzed.....
				if((!priv->pmib->dot11OperationEntry.disable_txsc) &&
					(txcfg->fr_type == _SKB_FRAME_TYPE_) &&
					(txcfg->pstat) &&
					(txcfg->frg_num == 1) &&
					((txcfg->privacy == 0)	|| (txcfg->privacy && 			
#ifdef CONFIG_IEEE80211W
					!UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF)))
#else
					!UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE))))
#endif
					&&(cpu_to_be16(pethhdr->type) != 0x888e) &&
					!is_dhcp && 
					!GetMData(txcfg->phdr) &&
					#ifdef MCAST2UI_REFINE
	                pskb && !IS_MCAST(&pskb->cb[10]) &&
					#else
					!IS_MCAST((unsigned char *)pethhdr) &&
					#endif
					(txcfg->aggre_en < FG_AGGRE_MSDU_FIRST)
				) {				
					rtl88XX_signin_txdesc(priv, txcfg, HW_TX_SC_BACKUP_HEADER);
				} else {
					rtl88XX_signin_txdesc(priv, txcfg, HW_TX_SC_NORMAL);
				}
				
			} else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif		
			{//not HAL
				rtl8192cd_signin_txdesc(priv, txcfg);
			}
#elif defined(CONFIG_USB_HCI) || defined(CONFIG_SDIO_HCI)
			rtl8192cd_signin_txdesc(priv, txcfg, pethhdr);
#endif
		}
#endif
	}

	//SMP_UNLOCK_XMIT(x);
	RESTORE_INT(x);

#ifdef CONFIG_PCI_HCI
#ifdef TX_SHORTCUT
	if ((!priv->pmib->dot11OperationEntry.disable_txsc) &&
		(txcfg->fr_type == _SKB_FRAME_TYPE_) &&
		(txcfg->pstat) &&
		(txcfg->frg_num == 1) &&
		((txcfg->privacy == 0)
#ifdef CONFIG_RTL_WAPI_SUPPORT
		|| (txcfg->privacy == _WAPI_SMS4_)
#endif
#ifdef CONFIG_IEEE80211W
		|| (txcfg->privacy && !UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE), txcfg->isPMF))) &&
#else
		|| (txcfg->privacy && !UseSwCrypto(priv, txcfg->pstat, (txcfg->pstat ? FALSE : TRUE)))) &&
#endif
		/*(!IEEE8021X_FUN ||
			(IEEE8021X_FUN && (txcfg->pstat->ieee8021x_ctrlport == 1) &&
			(pethhdr->type != htons(0x888e)))) && */
		(pethhdr->type != htons(0x888e)) &&
		!is_dhcp && 
#ifdef CONFIG_RTL_WAPI_SUPPORT
		(pethhdr->type != htons(ETH_P_WAPI)) &&
#endif
		!GetMData(txcfg->phdr) &&
#ifdef A4_STA		
        ((txcfg->pstat && txcfg->pstat->state & WIFI_A4_STA) 		
        #ifdef MCAST2UI_REFINE
            ||(pskb && !IS_MCAST(&pskb->cb[10])))&& 
        #else
            ||!IS_MCAST((unsigned char *)pethhdr)) &&
        #endif	
#else
#ifdef MCAST2UI_REFINE
        (
#ifdef WDS
	(txcfg->pstat->state & WIFI_WDS)? (!IS_BCAST2((unsigned char *)pethhdr)) : 
#endif
		(pskb && !IS_MCAST(&pskb->cb[10])))&&                
#else
        (
#ifdef WDS
        (txcfg->pstat->state & WIFI_WDS)? (!IS_BCAST2((unsigned char *)pethhdr)) : 
#endif  
        (!IS_MCAST((unsigned char *)pethhdr))) &&
#endif
#endif
		(txcfg->aggre_en < FG_AGGRE_MSDU_FIRST)
		)
	{
		int i = get_tx_sc_index(priv, txcfg->pstat, (unsigned char *)pethhdr);
		memcpy(&txcfg->pstat->tx_sc_ent[i].txcfg, txcfg, sizeof(struct tx_insn));
		memcpy((void *)&txcfg->pstat->tx_sc_ent[i].wlanhdr, txcfg->phdr, sizeof(struct wlanllc_hdr));
	}
	else {
		if (txcfg->pstat && pethhdr) {
			int i = get_tx_sc_index(priv, txcfg->pstat, (unsigned char *)pethhdr);
			if (i >= 0) {
				txcfg->pstat->tx_sc_ent[i].txcfg.fr_len = 0;
#ifdef TX_SCATTER
				txcfg->pstat->tx_sc_ent[i].has_desc3 = 0;
#endif
			}
		}
	}
#endif
#endif // CONFIG_PCI_HCI

	return SUCCESS;
}

#ifdef CONFIG_PCI_HCI
int rtl8192cd_firetx(struct rtl8192cd_priv *priv, struct tx_insn* txcfg)
{
#ifdef RX_TASKLET
//#ifndef SMP_SYNC
	unsigned long x;
	int locked=0;	
//#endif
	int ret;

	SAVE_INT_AND_CLI(x);
#ifdef SMP_SYNC		
	SMP_TRY_LOCK_XMIT(x,locked);
#endif
	ret = __rtl8192cd_firetx(priv, txcfg);

#ifdef SMP_SYNC				
	if(locked)
    SMP_UNLOCK_XMIT(flags);
#endif
	RESTORE_INT(x);
	return ret;
#else
	return (__rtl8192cd_firetx(priv, txcfg));
#endif
}
#endif // CONFIG_PCI_HCI


#ifdef CONFIG_PCI_HCI
#ifdef CONFIG_WLAN_HAL
static int 
rtl88XX_tx_recycle(
    struct rtl8192cd_priv   *priv, 
    unsigned int            txRingIdx, 
    int                     *recycleCnt_p
)
{
	int	                        cnt = 0;
	struct tx_desc_info         *pdescinfoH, *pdescinfo;
    // TODO: int or u2Byte ?
	//volatile int	            head, tail;
    volatile u2Byte             head, tail;
	int				            needRestartQueue = 0;
	int				            recycleCnt = 0;
    unsigned int                halQnum = GET_HAL_INTERFACE(priv)->MappingTxQueueHandler(priv, txRingIdx);
    PHCI_TX_DMA_MANAGER_88XX    ptx_dma;

    ptx_dma = (PHCI_TX_DMA_MANAGER_88XX)(_GET_HAL_DATA(priv)->PTxDMA88XX);
    head = GET_HAL_INTERFACE(priv)->GetTxQueueHWIdxHandler(priv, txRingIdx);
    tail = ptx_dma->tx_queue[halQnum].hw_idx;

	pdescinfoH	= get_txdesc_info(priv->pshare->pdesc_info, txRingIdx);

	while (CIRC_CNT_RTK(head, tail, ptx_dma->tx_queue[halQnum].total_txbd_num))
	{
		pdescinfo = pdescinfoH + (tail);

#if defined(CONFIG_NET_PCI) && !defined(USE_RTL8186_SDK)
		if (IS_PCIBIOS_TYPE && (pdescinfo->buf_len[0]!=0 && pdescinfo->buf_paddr[0]!=0))
			//use the paddr and flen of pdesc field for icv, mic case which doesn't fill the pdescinfo
			pci_unmap_single(priv->pshare->pdev,
							 pdescinfo->buf_paddr[0],//payload
							 (pdescinfo->buf_len[0])&0xffff,
							 PCI_DMA_TODEVICE);
#endif

		if (pdescinfo->type == _SKB_FRAME_TYPE_)
		{
#ifdef MP_TEST 
			if (OPMODE & WIFI_MP_CTX_BACKGROUND) {
                #if 0
                printk("skb_tail: 0x%x, skb_head: 0x%x\n", 
                                priv->pshare->skb_tail,
                                priv->pshare->skb_head
                                );
                #endif
                
				struct sk_buff *skb = (struct sk_buff *)(pdescinfo->pframe);
				skb->data = skb->head;
				skb->tail = skb->data;
				skb->len = 0;
				priv->pshare->skb_tail = (priv->pshare->skb_tail + 1) & (NUM_MP_SKB - 1);
			}
			else
#endif
			{
#ifdef __LINUX_2_6__
				rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->pframe), _SKB_TX_IRQ_);
#else
// for debug ------------
//				rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->pframe), _SKB_TX_IRQ_);
				if (pdescinfo->pframe) {
    				if (((struct sk_buff *)pdescinfo->pframe)->list) {
    					DEBUG_ERR("Free tx skb error, skip it!\n");
    					priv->ext_stats.freeskb_err++;
    				}
    				else {
    					rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->pframe), _SKB_TX_IRQ_);
    				}
				}
#endif
				needRestartQueue = 1;
			}
		}
		else if (pdescinfo->type == _PRE_ALLOCMEM_)
		{
			release_mgtbuf_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _PRE_ALLOCHDR_)
		{
			release_wlanhdr_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _PRE_ALLOCLLCHDR_)
		{
			release_wlanllchdr_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _PRE_ALLOCICVHDR_)
		{
			release_icv_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _PRE_ALLOCMICHDR_)
		{
			release_mic_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _RESERVED_FRAME_TYPE_)
		{
			// the chained skb, no need to release memory
		}
		else
		{
			DEBUG_ERR("Unknown tx frame type %d\n", pdescinfo->type);
		}

        //Reset to default value
        pdescinfo->type     = _RESERVED_FRAME_TYPE_;
		// for skb buffer free
		pdescinfo->pframe   = NULL;

		recycleCnt++;

		for (cnt =0; cnt<TXBD_ELE_NUM-2; cnt++) {
			//if (txRingIdx == 2)
			//panic_printk("buf type[%d]=%d\n", cnt, pdescinfo->buf_type[cnt]);

			if (pdescinfo->buf_type[cnt] == _SKB_FRAME_TYPE_)
			{
#ifdef MP_TEST
				if (OPMODE & WIFI_MP_CTX_BACKGROUND) {
					struct sk_buff *skb = (struct sk_buff *)(pdescinfo->buf_pframe[cnt]);
					skb->data = skb->head;
					skb->tail = skb->data;
					skb->len = 0;
					priv->pshare->skb_tail = (priv->pshare->skb_tail + 1) & (NUM_MP_SKB - 1);
				}
				else
#endif
				{
#ifdef __LINUX_2_6__
					rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->buf_pframe[cnt]), _SKB_TX_IRQ_);
#else
// for debug ------------
//					rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->pframe), _SKB_TX_IRQ_);
					if (pdescinfo->buf_pframe[cnt]) {
					if (((struct sk_buff *)pdescinfo->buf_pframe[cnt])->list) {
						DEBUG_ERR("Free tx skb error, skip it!\n");
						priv->ext_stats.freeskb_err++;
					}
					else
						rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->buf_pframe[cnt]), _SKB_TX_IRQ_);
					}
#endif
					needRestartQueue = 1;
				}
			}
			else if (pdescinfo->buf_type[cnt] == _PRE_ALLOCMEM_)
			{
				release_mgtbuf_to_poll(priv, (UINT8 *)(pdescinfo->buf_pframe[cnt]));
			}
			else if (pdescinfo->buf_type[cnt] == _PRE_ALLOCHDR_)
			{
				release_wlanhdr_to_poll(priv, (UINT8 *)(pdescinfo->buf_pframe[cnt]));
			}
			else if (pdescinfo->buf_type[cnt] == _PRE_ALLOCLLCHDR_)
			{
				release_wlanllchdr_to_poll(priv, (UINT8 *)(pdescinfo->buf_pframe[cnt]));
			}
			else if (pdescinfo->buf_type[cnt] == _PRE_ALLOCICVHDR_)
			{
				release_icv_to_poll(priv, (UINT8 *)(pdescinfo->buf_pframe[cnt]));
			}
			else if (pdescinfo->buf_type[cnt] == _PRE_ALLOCMICHDR_)
			{
				release_mic_to_poll(priv, (UINT8 *)(pdescinfo->buf_pframe[cnt]));
			}
			else if (pdescinfo->buf_type[cnt] == _RESERVED_FRAME_TYPE_)
			{
				// the chained skb, no need to release memory
			}
			else
			{
				DEBUG_ERR("Unknown tx frame type %d:%d\n", pdescinfo->buf_type[cnt]);
			}

            //Reset to default value
            pdescinfo->buf_type[cnt]    = _RESERVED_FRAME_TYPE_;
			// for skb buffer free
            pdescinfo->buf_pframe[cnt]  = NULL;

			recycleCnt ++;
		}

    	tail = (tail + 1) % (ptx_dma->tx_queue[halQnum].total_txbd_num);
        ptx_dma->tx_queue[halQnum].avail_txbd_num++;

#ifdef RESERVE_TXDESC_FOR_EACH_IF
        // TODO:
		if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc)
			pdescinfo->priv->use_txdesc_cnt[txRingIdx]--;
#endif

		if (head == tail)
			head = GET_HAL_INTERFACE(priv)->GetTxQueueHWIdxHandler(priv, txRingIdx);
	}


    ptx_dma->tx_queue[halQnum].hw_idx = tail;

	if (recycleCnt_p)
		*recycleCnt_p = recycleCnt;

	return needRestartQueue;
}
#endif

/*
	Procedure to re-cycle TXed packet in Queue index "txRingIdx"

	=> Return value means if system need restart-TX-queue or not.

		1: Need Re-start Queue
		0: Don't Need Re-start Queue
*/

static int rtl8192cd_tx_recycle(struct rtl8192cd_priv *priv, unsigned int txRingIdx, int *recycleCnt_p)
{
	struct tx_desc	*pdescH, *pdesc;
	struct tx_desc_info *pdescinfoH, *pdescinfo;
	volatile int	head, tail;
	struct rtl8192cd_hw	*phw=GET_HW(priv);
	int				needRestartQueue=0;
	int				recycleCnt=0;
#ifdef CONFIG_WLAN_HAL
    if (IS_HAL_CHIP(priv)) {
        return rtl88XX_tx_recycle(priv, txRingIdx, recycleCnt_p);
    } else if (CONFIG_WLAN_NOT_HAL_EXIST)
#endif //CONFIG_WLAN_HAL
	{//not HAL
	head		= get_txhead(phw, txRingIdx);
	tail		= get_txtail(phw, txRingIdx);
	pdescH		= get_txdesc(phw, txRingIdx);
	pdescinfoH	= get_txdesc_info(priv->pshare->pdesc_info, txRingIdx);

	while (CIRC_CNT_RTK(head, tail, CURRENT_NUM_TX_DESC))
	{
		pdesc = pdescH + (tail);
		pdescinfo = pdescinfoH + (tail);

#ifdef __MIPSEB__
		pdesc = (struct tx_desc *)KSEG1ADDR(pdesc);
#endif

		if (!pdesc || (get_desc(pdesc->Dword0) & TX_OWN))
			break;
		if (pdescinfo->pframe == NULL) {
			DEBUG_ERR("Free Null Buf: head=%d tail=%d recycleCnt=%d RingIdx=%d!\n", head, tail, recycleCnt, txRingIdx);
		}

#ifdef CONFIG_PCI_HCI
#if defined(CONFIG_NET_PCI) && !defined(USE_RTL8186_SDK)
		if (IS_PCIBIOS_TYPE)
			//use the paddr and flen of pdesc field for icv, mic case which doesn't fill the pdescinfo
#ifdef CONFIG_RTL_8812_SUPPORT
			if(GET_CHIP_VER(priv)== VERSION_8812E)	
			pci_unmap_single(priv->pshare->pdev,
							 get_desc(pdesc->Dword10),
							 (get_desc(pdesc->Dword7)&0xffff),
							 PCI_DMA_TODEVICE);
			else
#endif
			pci_unmap_single(priv->pshare->pdev,
							 get_desc(pdesc->Dword8),
							 (get_desc(pdesc->Dword7)&0xffff),
							 PCI_DMA_TODEVICE);

#endif
#endif // CONFIG_PCI_HCI

		if (pdescinfo->type == _SKB_FRAME_TYPE_)
		{
#ifdef MP_TEST
			if (OPMODE & WIFI_MP_CTX_BACKGROUND) {
				struct sk_buff *skb = (struct sk_buff *)(pdescinfo->pframe);
				skb->data = skb->head;
				skb->tail = skb->data;
				skb->len = 0;
				priv->pshare->skb_tail = (priv->pshare->skb_tail + 1) & (NUM_MP_SKB - 1);
			}
			else
#endif
			{
#ifdef __LINUX_2_6__
				rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->pframe), _SKB_TX_IRQ_);
#else
// for debug ------------
//				rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->pframe), _SKB_TX_IRQ_);
				if (pdescinfo->pframe) {
				if (((struct sk_buff *)pdescinfo->pframe)->list) {
					DEBUG_ERR("Free tx skb error, skip it!\n");
					priv->ext_stats.freeskb_err++;
				}
				else
					rtl_kfree_skb(pdescinfo->priv, (struct sk_buff *)(pdescinfo->pframe), _SKB_TX_IRQ_);
				}
#endif
				needRestartQueue = 1;
			}
		}
		else if (pdescinfo->type == _PRE_ALLOCMEM_)
		{
			release_mgtbuf_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _PRE_ALLOCHDR_)
		{
			release_wlanhdr_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _PRE_ALLOCLLCHDR_)
		{
			release_wlanllchdr_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _PRE_ALLOCICVHDR_)
		{
			release_icv_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _PRE_ALLOCMICHDR_)
		{
			release_mic_to_poll(priv, (UINT8 *)(pdescinfo->pframe));
		}
		else if (pdescinfo->type == _RESERVED_FRAME_TYPE_)
		{
			// the chained skb, no need to release memory
		}
		else
		{
			DEBUG_ERR("Unknown tx frame type %d\n", pdescinfo->type);
		}

		// for skb buffer free
		pdescinfo->type = _RESERVED_FRAME_TYPE_;
		pdescinfo->pframe = NULL;

		recycleCnt ++;

		tail = (tail + 1) % CURRENT_NUM_TX_DESC;

#ifdef RESERVE_TXDESC_FOR_EACH_IF
		if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc)
			pdescinfo->priv->use_txdesc_cnt[txRingIdx]--;
#endif
	}

	*get_txtail_addr(phw, txRingIdx) = tail;

	if (recycleCnt_p)
		*recycleCnt_p = recycleCnt;

	return needRestartQueue;
	} //not HAL
#ifdef CONFIG_WLAN_HAL
	else {
		panic_printk("Shouldn't come here %s()\n", __FUNCTION__);
		return 0;
	}
#endif
}


#ifdef  CONFIG_WLAN_HAL
static void rtl88XX_tx_dsr(unsigned long task_priv)
{
	struct rtl8192cd_priv	*priv = (struct rtl8192cd_priv *)task_priv;
	unsigned int	        j=0;
	unsigned int	        restart_queue=0;
	struct rtl8192cd_hw	    *phw=GET_HW(priv);
	int                     needRestartQueue;
    int                     Queue_max = HIGH_QUEUE;
	unsigned long           flags;

	if (!phw)
		return;

#ifdef PCIE_POWER_SAVING
	if ((priv->pwr_state == L2) || (priv->pwr_state == L1)) {
		priv->pshare->has_triggered_tx_tasklet = 0;
		return;
	}
#endif

#ifdef MBSSID
    if (GET_ROOT(priv)->pmib->miscEntry.vap_enable) 
        Queue_max = HIGH_QUEUE7;      
#endif


	SAVE_INT_AND_CLI(flags);
	SMP_LOCK_XMIT(flags);

	for(j=0; j<=Queue_max; j++)
	{
		needRestartQueue = rtl8192cd_tx_recycle(priv, j, NULL);
		/* If anyone of queue report the TX queue need to be restart : we would set "restart_queue" to process ALL queues */
		if (needRestartQueue == 1)
			restart_queue = 1;
	}

	if (restart_queue)
		rtl8192cd_tx_restartQueue(priv);

#ifdef MP_TEST
#if 1//def CONFIG_RTL8672
	if ((OPMODE & (WIFI_MP_STATE|WIFI_MP_CTX_BACKGROUND|WIFI_MP_CTX_BACKGROUND_STOPPING))
			==(WIFI_MP_STATE|WIFI_MP_CTX_BACKGROUND))
#else //CONFIG_RTL8672
	if ((OPMODE & (WIFI_MP_STATE|WIFI_MP_CTX_BACKGROUND))==(WIFI_MP_STATE|WIFI_MP_CTX_BACKGROUND)) 
#endif //CONFIG_RTL8672
	{
#ifdef CONFIG_WLAN_HAL
		PHCI_TX_DMA_MANAGER_88XX    ptx_dma;
#endif

		RESTORE_INT(flags);

#ifdef CONFIG_WLAN_HAL
        ptx_dma = (PHCI_TX_DMA_MANAGER_88XX)(_GET_HAL_DATA(priv)->PTxDMA88XX);

        if (compareAvailableTXBD(priv, (CURRENT_NUM_TX_DESC/2), BE_QUEUE, 1)) {
			SMP_UNLOCK_XMIT(flags);
            mp_ctx(priv, (unsigned char *)"tx-isr");
			SMP_LOCK_XMIT(flags);
        }
#else
		int *tx_head, *tx_tail;

		tx_head = get_txhead_addr(phw, BE_QUEUE);
		tx_tail = get_txtail_addr(phw, BE_QUEUE);
		if (CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC) > (CURRENT_NUM_TX_DESC/2))
		{
			SMP_UNLOCK_XMIT(flags);
			mp_ctx(priv, (unsigned char *)"tx-isr");
			SMP_LOCK_XMIT(flags);
		}
#endif //CONFIG_WLAN_HAL

		SAVE_INT_AND_CLI(flags);
	}
#endif

	refill_skb_queue(priv);

#ifdef USE_TXQUEUE
    // TODO: HAL
	if (GET_ROOT(priv)->pmib->miscEntry.use_txq && !priv->pshare->txq_isr && priv->pshare->txq_check)
	{
		int q_num, send_cnt = 0;
		priv->pshare->txq_isr = 1;
		
		for (q_num=6; q_num>=0; q_num--)
		{
			priv->pshare->txq_stop = 0;
			while ( txq_len(&priv->pshare->txq_list[q_num]) > 0 )
			{
				struct sk_buff *tmp_skb = NULL;
				struct net_device *dev = NULL;
				remove_skb_from_txq(&priv->pshare->txq_list[q_num], &tmp_skb, &dev, &priv->pshare->txq_pool);
				if (tmp_skb && dev) {
#ifdef RESERVE_TXDESC_FOR_EACH_IF
					if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc)
						((struct rtl8192cd_priv*)(dev->priv))->use_txq_cnt[q_num]--;
#endif
#ifdef ENABLE_RTL_SKB_STATS
					rtl_atomic_dec(&priv->rtl_tx_skb_cnt);
#endif
					__rtl8192cd_start_xmit(tmp_skb, dev, TX_NORMAL);
					send_cnt++;
					if (priv->pshare->txq_stop) break;
				}
			}
		}
		
		priv->pshare->txq_isr = 0;

		if (send_cnt == 0)
			priv->pshare->txq_check = 0;
	}
#endif

	priv->pshare->has_triggered_tx_tasklet = 0;

	RESTORE_INT(flags);
	SMP_UNLOCK_XMIT(flags);
}
#endif  //CONFIG_WLAN_HAL


/*-----------------------------------------------------------------------------
Purpose of tx_dsr:

	For ALL TX queues
		1. Free Allocated Buf
		2. Update tx_tail
		3. Update tx related counters
		4. Restart tx queue if needed
------------------------------------------------------------------------------*/
void rtl8192cd_tx_dsr(unsigned long task_priv)
{
	struct rtl8192cd_priv	*priv = (struct rtl8192cd_priv *)task_priv;
	unsigned int	j=0;
	unsigned int	restart_queue=0;
	struct rtl8192cd_hw	*phw=GET_HW(priv);
	int needRestartQueue;
	unsigned long flags;
#ifdef CONFIG_WLAN_HAL
    if(IS_HAL_CHIP(priv)){
        rtl88XX_tx_dsr((unsigned long)priv);
        return;
    }else if(CONFIG_WLAN_NOT_HAL_EXIST)
#endif //CONFIG_WLAN_HAL
	{//not HAL
	if (!phw)
		return;

#ifdef PCIE_POWER_SAVING
	if ((priv->pwr_state == L2) || (priv->pwr_state == L1)) {
		priv->pshare->has_triggered_tx_tasklet = 0;
		return;
	}
#endif

	SAVE_INT_AND_CLI(flags);
	SMP_LOCK_XMIT(flags);
	for(j=0; j<=HIGH_QUEUE; j++)
	{
		needRestartQueue = rtl8192cd_tx_recycle(priv, j, NULL);
		/* If anyone of queue report the TX queue need to be restart : we would set "restart_queue" to process ALL queues */
		if (needRestartQueue == 1)
			restart_queue = 1;
	}

	if (restart_queue)
		rtl8192cd_tx_restartQueue(priv);

#ifdef MP_TEST
#if 1//def CONFIG_RTL8672
	if ((OPMODE & (WIFI_MP_STATE|WIFI_MP_CTX_BACKGROUND|WIFI_MP_CTX_BACKGROUND_STOPPING))
			==(WIFI_MP_STATE|WIFI_MP_CTX_BACKGROUND))
#else //CONFIG_RTL8672
	if ((OPMODE & (WIFI_MP_STATE|WIFI_MP_CTX_BACKGROUND))==(WIFI_MP_STATE|WIFI_MP_CTX_BACKGROUND)) 
#endif //CONFIG_RTL8672
	{
		int *tx_head, *tx_tail;
		RESTORE_INT(flags);
		tx_head = get_txhead_addr(phw, BE_QUEUE);
		tx_tail = get_txtail_addr(phw, BE_QUEUE);
		if (CIRC_SPACE_RTK(*tx_head, *tx_tail, CURRENT_NUM_TX_DESC) > (CURRENT_NUM_TX_DESC/2))
		{
			SMP_UNLOCK_XMIT(flags);
			mp_ctx(priv, (unsigned char *)"tx-isr");
		    SMP_LOCK_XMIT(flags);
	    }
		SAVE_INT_AND_CLI(flags);
	}
#endif

	refill_skb_queue(priv);

#ifdef USE_TXQUEUE
	if (GET_ROOT(priv)->pmib->miscEntry.use_txq && !priv->pshare->txq_isr && priv->pshare->txq_check)
	{
		int q_num, send_cnt = 0, tx_flag = TX_NORMAL;
		priv->pshare->txq_isr = 1;
		
		for (q_num=6; q_num>=0; q_num--)
		{
			priv->pshare->txq_stop = 0;
			while ( txq_len(&priv->pshare->txq_list[q_num]) > 0 )
			{
				struct sk_buff *tmp_skb = NULL;
				struct net_device *dev = NULL;
				remove_skb_from_txq(&priv->pshare->txq_list[q_num], &tmp_skb, &dev, &priv->pshare->txq_pool);
				if (tmp_skb && dev) {
#ifdef RESERVE_TXDESC_FOR_EACH_IF
					if (GET_ROOT(priv)->pmib->miscEntry.rsv_txdesc)
						((struct rtl8192cd_priv*)(dev->priv))->use_txq_cnt[q_num]--;
#endif
#ifdef SUPPORT_TX_MCAST2UNI
					tx_flag = tmp_skb->cb[16];
#endif
#ifdef ENABLE_RTL_SKB_STATS
					rtl_atomic_dec(&priv->rtl_tx_skb_cnt);
#endif
					__rtl8192cd_start_xmit(tmp_skb, dev, tx_flag);
					send_cnt++;
					if (priv->pshare->txq_stop) break;
				}
			}
		}
		
		priv->pshare->txq_isr = 0;

		if (send_cnt == 0)
			priv->pshare->txq_check = 0;
	}
#endif

	priv->pshare->has_triggered_tx_tasklet = 0;

	RESTORE_INT(flags);
	SMP_UNLOCK_XMIT(flags);
}
}


/*
	Try to do TX-DSR for only ONE TX-queue ( rtl8192cd_tx_dsr would check for ALL TX queue )
*/
int rtl8192cd_tx_queueDsr(struct rtl8192cd_priv *priv, unsigned int txRingIdx)
{
	int recycleCnt;
#ifndef SMP_SYNC
	unsigned long flags;
#endif
	SAVE_INT_AND_CLI(flags);

	if (rtl8192cd_tx_recycle(priv, txRingIdx, &recycleCnt) == 1)
		rtl8192cd_tx_restartQueue(priv);

	RESTORE_INT(flags);
	return recycleCnt;
}


/*
	Procedure to restart TX Queue
*/
void rtl8192cd_tx_restartQueue(struct rtl8192cd_priv *priv)
{
#ifdef __KERNEL__
	priv = GET_ROOT(priv);

	if (IS_DRV_OPEN(priv) && netif_queue_stopped(priv->dev)) {
		DEBUG_INFO("wake-up Root queue\n");
		netif_wake_queue(priv->dev);
	}

#ifdef UNIVERSAL_REPEATER
	if (IS_DRV_OPEN(GET_VXD_PRIV(priv)) && netif_queue_stopped(GET_VXD_PRIV(priv)->dev)) {
		DEBUG_INFO("wake-up VXD queue\n");
		netif_wake_queue(GET_VXD_PRIV(priv)->dev);
	}
#endif

#ifdef MBSSID
	if (priv->pmib->miscEntry.vap_enable) {
		int bssidIdx;
		for (bssidIdx=0; bssidIdx<RTL8192CD_NUM_VWLAN; bssidIdx++) {
			if (IS_DRV_OPEN(priv->pvap_priv[bssidIdx]) && netif_queue_stopped(priv->pvap_priv[bssidIdx]->dev)) {
				DEBUG_INFO("wake-up Vap%d queue\n", bssidIdx);
				netif_wake_queue(priv->pvap_priv[bssidIdx]->dev);
			}
		}
	}
#endif

#ifdef CONFIG_RTK_MESH
    if(priv->mesh_dev) {
		if (netif_running(priv->mesh_dev) &&
				netif_queue_stopped(priv->mesh_dev) )
		{
			netif_wake_queue(priv->mesh_dev);
		}
    }
#endif
#ifdef WDS
	if (priv->pmib->dot11WdsInfo.wdsEnabled) {
		int i;
		for (i=0; i<priv->pmib->dot11WdsInfo.wdsNum; i++) {
			if (netif_running(priv->wds_dev[i]) &&
				netif_queue_stopped(priv->wds_dev[i])) {
				DEBUG_INFO("wake-up wds[%d] queue\n", i);
				netif_wake_queue(priv->wds_dev[i]);
			}
		}
	}
#endif
#endif
}
#endif // CONFIG_PCI_HCI


static int tkip_mic_padding(struct rtl8192cd_priv *priv,
				unsigned char *da, unsigned char *sa, unsigned char priority,
				unsigned char *llc, struct sk_buff *pskb, struct tx_insn* txcfg)
{
	// now check what's the mic key we should apply...

	unsigned char	*mickey = NULL;
	unsigned int	keylen = 0;
	struct stat_info	*pstat = NULL;
	unsigned char	*hdr, hdr_buf[16];
	unsigned int	num_blocks;
	unsigned char	tkipmic[8];
	unsigned char	*pbuf=pskb->data;
	unsigned int	len=pskb->len;
	unsigned int	llc_len = 0;

	// check if the mic/tkip key is valid at this moment.

	if ((pskb->tail + 8) > (pskb->end))
	{
		DEBUG_ERR("pskb have no extra room for TKIP Michael padding\n");
		return FALSE;
	}

#ifdef WDS
	if (txcfg->wdsIdx >= 0) {
		pstat = get_stainfo(priv, priv->pmib->dot11WdsInfo.entry[txcfg->wdsIdx].macAddr);
		if (pstat) {
			keylen = GET_UNICAST_MIC_KEYLEN;
			mickey = GET_UNICAST_TKIP_MIC1_KEY;
		}
	}
	else
#endif
	if (OPMODE & WIFI_AP_STATE)
	{

#ifdef CONFIG_RTK_MESH

		if(txcfg->is_11s)

			da = txcfg->nhop_11s;

#endif
		if (IS_MCAST(da))
		{
			keylen = GET_GROUP_MIC_KEYLEN;
			mickey = GET_GROUP_TKIP_MIC1_KEY;
#ifdef A4_STA			
			if (txcfg->pstat && (txcfg->pstat->state & WIFI_A4_STA)) {
				pstat = txcfg->pstat;
				keylen = GET_UNICAST_MIC_KEYLEN;
				mickey = GET_UNICAST_TKIP_MIC1_KEY;
			}
#endif			
		}
		else
		{
			pstat = get_stainfo (priv, da);
#ifdef A4_STA			
			if (priv->pshare->rf_ft_var.a4_enable && (pstat == NULL)) 
				pstat = a4_sta_lookup(priv, da);
#endif		
			if (pstat == NULL) {
				DEBUG_ERR("tx mic pstat == NULL\n");
				return FALSE;
			}

			keylen = GET_UNICAST_MIC_KEYLEN;
			mickey = GET_UNICAST_TKIP_MIC1_KEY;
		}
	}
#ifdef CLIENT_MODE
	else if (OPMODE & WIFI_STATION_STATE)
	{
		pstat = get_stainfo (priv, BSSID);
		if (pstat == NULL) {
			DEBUG_ERR("tx mic pstat == NULL\n");
			return FALSE;
		}

		keylen = GET_UNICAST_MIC_KEYLEN;
		mickey = GET_UNICAST_TKIP_MIC2_KEY;
	}
	else if (OPMODE & WIFI_ADHOC_STATE)
	{
		keylen = GET_GROUP_MIC_KEYLEN;
		mickey = GET_GROUP_TKIP_MIC1_KEY;
	}
#endif

	if ((txcfg->aggre_en == FG_AGGRE_MSDU_MIDDLE) || (txcfg->aggre_en == FG_AGGRE_MSDU_LAST))
		mickey = pstat->tmp_mic_key;

	if (keylen == 0)
	{
		DEBUG_ERR("no mic padding for TKIP due to keylen=0\n");
		return FALSE;
	}

	if (txcfg->aggre_en <= FG_AGGRE_MSDU_FIRST) {
		hdr = hdr_buf;
		memcpy((void *)hdr, (void *)da, WLAN_ADDR_LEN);
		memcpy((void *)(hdr + WLAN_ADDR_LEN), (void *)sa, WLAN_ADDR_LEN);
		hdr[12] = priority;
		hdr[13] = hdr[14] = hdr[15] = 0;
	}
	else
		hdr = NULL;

	pbuf[len] = 0x5a;   /* Insert padding */
	pbuf[len+1] = 0x00;
	pbuf[len+2] = 0x00;
	pbuf[len+3] = 0x00;
	pbuf[len+4] = 0x00;
	pbuf[len+5] = 0x00;
	pbuf[len+6] = 0x00;
	pbuf[len+7] = 0x00;

	if (llc)
		llc_len = 8;
	num_blocks = (16 + llc_len + len + 5) / 4;
	if ((16 + llc_len + len + 5) & (4-1))
		num_blocks++;

	if (txcfg->aggre_en >= FG_AGGRE_MSDU_FIRST) {
		if (txcfg->aggre_en == FG_AGGRE_MSDU_FIRST) {
			num_blocks = (16 + len) / 4;
			if ((16 + len) & (4-1))
				num_blocks++;
		}
		else if (txcfg->aggre_en == FG_AGGRE_MSDU_MIDDLE) {
			num_blocks = len / 4;
			if (len & (4-1))
				num_blocks++;
		}
		else if (txcfg->aggre_en == FG_AGGRE_MSDU_LAST) {
			num_blocks = (len + 5) / 4;
			if ((len + 5) & (4-1))
				num_blocks++;
		}
	}

	michael(priv, mickey, hdr, llc, pbuf, (num_blocks << 2), tkipmic, 1);

	//tkip mic is MSDU-based, before filled-in descriptor, already finished.
	if (!(priv->pshare->have_hw_mic) ||
		(priv->pmib->dot11StationConfigEntry.swTkipMic))
	{
#ifdef MICERR_TEST
		if (priv->micerr_flag) {
			tkipmic[7] ^= tkipmic[7];
			priv->micerr_flag = 0;
		}
#endif
		if ((txcfg->aggre_en == FG_AGGRE_MSDU_FIRST) || (txcfg->aggre_en == FG_AGGRE_MSDU_MIDDLE)) {
			memcpy((void *)pstat->tmp_mic_key, (void *)tkipmic, 8);
		}
		else
			memcpy((void *)(pbuf + len), (void *)tkipmic, 8);
	}

	return TRUE;
}


void wep_fill_iv(struct rtl8192cd_priv *priv,
				unsigned char *pwlhdr, unsigned int hdrlen, unsigned long keyid)
{
	unsigned char *iv = pwlhdr + hdrlen;
	union PN48 *ptsc48 = NULL;
	union PN48 auth_pn48;
	unsigned char *da;
	struct stat_info *pstat = NULL;

	memset(&auth_pn48, 0, sizeof(union PN48));
	da = get_da(pwlhdr);

#if defined(WDS) || defined(A4_STA)
	if (get_tofr_ds(pwlhdr) == 3)
		da = GetAddr1Ptr(pwlhdr);
#endif

	if (OPMODE & WIFI_AP_STATE)
	{
		if (IS_MCAST(da))
		{
			ptsc48 = GET_GROUP_ENCRYP_PN;
		}
		else
		{
			pstat = get_stainfo(priv, da);
			ptsc48 = GET_UNICAST_ENCRYP_PN;
		}
	}
#ifdef CLIENT_MODE
	else if (OPMODE & WIFI_STATION_STATE)
	{
		pstat = get_stainfo(priv, BSSID);
		if (pstat != NULL)
			ptsc48 = GET_UNICAST_ENCRYP_PN;
		else
			ptsc48 = &auth_pn48;
	}
	else if (OPMODE & WIFI_ADHOC_STATE)
		ptsc48 = GET_GROUP_ENCRYP_PN;
#endif

	if (ptsc48 == NULL)
	{
		DEBUG_ERR("no TSC for WEP due to ptsc48=NULL\n");
		return;
	}

	iv[0] = ptsc48->_byte_.TSC0;
	iv[1] = ptsc48->_byte_.TSC1;
	iv[2] = ptsc48->_byte_.TSC2;
	iv[3] = 0x0 | (keyid << 6);

	if (ptsc48->val48 == 0xffffffffffffULL)
		ptsc48->val48 = 0;
	else
		ptsc48->val48++;
}


void tkip_fill_encheader(struct rtl8192cd_priv *priv,
				unsigned char *pwlhdr, unsigned int hdrlen, unsigned long keyid_out)
{
	unsigned char *iv = pwlhdr + hdrlen;
	union PN48 *ptsc48 = NULL;
	unsigned int keyid = 0;
	unsigned char *da;
	struct stat_info *pstat = NULL;

	da = get_da(pwlhdr);

	if (OPMODE & WIFI_AP_STATE)
	{
#if defined(WDS) || defined(CONFIG_RTK_MESH) || defined(A4_STA)
		unsigned int to_fr_ds = (GetToDs(pwlhdr) << 1) | GetFrDs(pwlhdr);
		if (to_fr_ds == 3)
			da = GetAddr1Ptr(pwlhdr);
#endif

		if (IS_MCAST(da))
		{
			ptsc48 = GET_GROUP_ENCRYP_PN;
			keyid = priv->pmib->dot11GroupKeysTable.keyid;
		}
		else
		{
			pstat = get_stainfo(priv, da);
			ptsc48 = GET_UNICAST_ENCRYP_PN;
			keyid = 0;
		}
	}
#ifdef CLIENT_MODE
	else if (OPMODE & WIFI_STATION_STATE)
	{
		pstat = get_stainfo(priv, BSSID);
		ptsc48 = GET_UNICAST_ENCRYP_PN;
		keyid = 0;
	}
	else if (OPMODE & WIFI_ADHOC_STATE)
	{
		ptsc48 = GET_GROUP_ENCRYP_PN;
		keyid = 0;
	}
#endif

#ifdef __DRAYTEK_OS__
	keyid = keyid_out;
#endif

	if (ptsc48 == NULL)
	{
		DEBUG_ERR("no TSC for TKIP due to ptsc48=NULL\n");
		return;
	}

	iv[0] = ptsc48->_byte_.TSC1;
	iv[1] = (iv[0] | 0x20) & 0x7f;
	iv[2] = ptsc48->_byte_.TSC0;
	iv[3] = 0x20 | (keyid << 6);
	iv[4] = ptsc48->_byte_.TSC2;
	iv[5] = ptsc48->_byte_.TSC3;
	iv[6] = ptsc48->_byte_.TSC4;
	iv[7] = ptsc48->_byte_.TSC5;

	if (ptsc48->val48 == 0xffffffffffffULL)
		ptsc48->val48 = 0;
	else
		ptsc48->val48++;
}


__MIPS16
#ifndef WIFI_MIN_IMEM_USAGE
__IRAM_IN_865X
#endif
void aes_fill_encheader(struct rtl8192cd_priv *priv,
				unsigned char *pwlhdr, unsigned int hdrlen, unsigned long keyid)
{
	unsigned char *da;
	struct stat_info *pstat = NULL;
	union PN48 *pn48 = NULL;

#ifdef __DRAYTEK_OS__
	int	keyid_input = keyid;
#endif

	da = get_da(pwlhdr);

	if (OPMODE & WIFI_AP_STATE)
	{
#if defined(WDS) || defined(CONFIG_RTK_MESH) || defined(A4_STA)
		unsigned int to_fr_ds = (GetToDs(pwlhdr) << 1) | GetFrDs(pwlhdr);
		if (to_fr_ds == 3)
			da = GetAddr1Ptr(pwlhdr);
#endif


		if (IS_MCAST(da))
		{
			pn48 = GET_GROUP_ENCRYP_PN;
			keyid = priv->pmib->dot11GroupKeysTable.keyid;
		}
		else
		{
			pstat = get_stainfo(priv, da);
			if(NULL == pstat) {
				DEBUG_ERR("no such station\n");
				DEBUG_ERR("da:%02x-%02x-%02x-%02x-%02x-%02x\n",
					da[0],da[1],da[2],da[3],da[4],da[5]);
				return;
			}
			pn48 = GET_UNICAST_ENCRYP_PN;
			keyid = 0;
		}
	}
#ifdef CLIENT_MODE
	else if (OPMODE & WIFI_STATION_STATE)
	{
		pstat = get_stainfo(priv, BSSID);
		if(NULL == pstat) {
			DEBUG_ERR("no such station\n");
			return;
		}
		pn48 = GET_UNICAST_ENCRYP_PN;
		keyid = 0;
	}
	else if (OPMODE & WIFI_ADHOC_STATE)
	{
		pn48 = GET_GROUP_ENCRYP_PN;
		keyid = 0;
	}
#endif

	if (pn48 == NULL)
	{
		DEBUG_ERR("no TSC for AES due to pn48=NULL\n");
		return;
	}

#ifdef __DRAYTEK_OS__
	keyid = keyid_input;
#endif

	pwlhdr[hdrlen]   = pn48->_byte_.TSC0;
	pwlhdr[hdrlen+1] = pn48->_byte_.TSC1;
	pwlhdr[hdrlen+2] =  0x00;
	pwlhdr[hdrlen+3] = (0x20 | (keyid << 6));
	pwlhdr[hdrlen+4] = pn48->_byte_.TSC2;
	pwlhdr[hdrlen+5] = pn48->_byte_.TSC3;
	pwlhdr[hdrlen+6] = pn48->_byte_.TSC4;
	pwlhdr[hdrlen+7] = pn48->_byte_.TSC5;

   	if (pn48->val48 == 0xffffffffffffULL)
		pn48->val48 = 0;
	else
		pn48->val48++;
}

