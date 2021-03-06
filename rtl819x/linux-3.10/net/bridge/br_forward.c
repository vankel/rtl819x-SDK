/*
 *	Forwarding decision
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/netpoll.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"

#if defined (CONFIG_RTL_IGMP_SNOOPING)
#include <linux/ip.h>
#include <linux/in.h>
#if defined (CONFIG_RTL_MLD_SNOOPING)
#include <linux/ipv6.h>
#include <linux/in6.h>
#endif
#include <linux/igmp.h>
#include <net/checksum.h>
#include <net/rtl/rtl865x_igmpsnooping_glue.h>
#include <net/rtl/rtl865x_igmpsnooping.h>
#include <net/rtl/rtl865x_netif.h>
#include <net/rtl/rtl_nic.h>
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
#include <net/rtl/rtl865x_multicast.h>
#endif

extern int igmpsnoopenabled;
extern unsigned int brIgmpModuleIndex;
extern unsigned int br0SwFwdPortMask;
#if defined (CONFIG_RTL_MLD_SNOOPING)
extern int mldSnoopEnabled;
#endif
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
#include <net/rtl/rtl865x_multicast.h>
#include <net/rtl/rtl865x_netif.h>
#include <net/rtl/rtl_nic.h>
#endif
extern unsigned int br0SwFwdPortMask;
extern unsigned int brIgmpModuleIndex;
extern unsigned int nicIgmpModuleIndex;

#endif/*CONFIG_RTL_IGMP_SNOOPING*/

#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
extern unsigned int brIgmpModuleIndex_2;
extern unsigned int br1SwFwdPortMask;
extern struct net_bridge *bridge1;
#endif
#if	defined(CONFIG_RTL_819X)
#include <net/rtl/features/rtl_ps_hooks.h>
#include <net/rtl/features/rtl_ps_log.h>
#endif

#if defined(CONFIG_BRIDGE_IGMP_SNOOPING)
#if defined(CONFIG_RTL_HARDWARE_MULTICAST)
#include <net/rtl/rtl865x_multicast.h>
#include <net/rtl/rtl_nic.h>
#endif/*CONFIG_RTL_HARDWARE_MULTICAST*/
#endif/*CONFIG_BRIDGE_IGMP_SNOOPING*/

#if defined(CONFIG_RTL_MULTI_LAN_DEV)
extern int rtl_is_lan_dev(struct net_device *dev);
#endif
static int deliver_clone(const struct net_bridge_port *prev,
			 struct sk_buff *skb,
			 void (*__packet_hook)(const struct net_bridge_port *p,
					       struct sk_buff *skb));

/* Don't forward packets to originating port or forwarding diasabled */
static inline int should_deliver(const struct net_bridge_port *p,
				 const struct sk_buff *skb)
{
#if defined(CONFIG_RTK_VLAN_NEW_FEATURE) || defined(CONFIG_RTK_GUEST_ZONE) ||defined(CONFIG_RTL_MULTI_LAN_DEV)
	if (!(((p->flags & BR_HAIRPIN_MODE) || skb->dev != p->dev) &&
		(br_allowed_egress(p->br, nbp_get_vlan_info(p), skb)) &&
		(p->state == BR_STATE_FORWARDING)))
		return 0;
	
#if defined(CONFIG_RTK_VLAN_NEW_FEATURE)
	if (skb->src_info) {
	    struct vlan_info_item *pitem = rtl_get_vlan_info_item_by_dev(p->dev);
		
	    /* index == 1, it means skb is cloned skb in rx_vlan_process */
	    if (skb->src_info->index) {
	        if (pitem && pitem->info.forwarding_rule!=1)
	            return 0;
	    }
		
	    /* vlan_br can't send packet to vlan_nat */
	    if (skb->src_info->forwarding_rule==1) {
	        if (pitem && pitem->info.forwarding_rule==2)
	            return 0;
	    }
		
	    /* vlan_nat can't send packet to vlan_br */
	    if (skb->src_info->forwarding_rule==2) {
	        if (pitem && pitem->info.forwarding_rule==1)
	            return 0;
	    }
	}
#endif
#ifdef CONFIG_RTK_GUEST_ZONE
	if (skb->dev && br_port_get_rcu(skb->dev) && p->br) {
		// skb to/from different zone case, check zone isolation
		if (p->br->is_zone_isolated &&
			(((br_port_get_rcu(skb->dev)->zone_type == ZONE_TYPE_HOST) && (p->zone_type == ZONE_TYPE_GUEST)) ||
			 ((br_port_get_rcu(skb->dev)->zone_type == ZONE_TYPE_GUEST) && (p->zone_type == ZONE_TYPE_HOST)))) {
#ifdef DEBUG_GUEST_ZONE
			GZDEBUG("zone block!\n");
#endif
			return 0;
		}

		// skb to/from guest zone case, check guest isolation
		if (p->br->is_guest_isolated &&
			(br_port_get_rcu(skb->dev)->zone_type == ZONE_TYPE_GUEST) &&
			(p->zone_type == ZONE_TYPE_GUEST)) {
#ifdef DEBUG_GUEST_ZONE
			GZDEBUG("guest block!\n");
#endif
			return 0;
		}

		// skb from guest zone and to gateway zone case, check gateway mac
		if ((br_port_get_rcu(skb->dev)->zone_type == ZONE_TYPE_GUEST) &&
			(p->zone_type == ZONE_TYPE_GATEWAY)) {
			if (p->br->gateway_mac_set &&
				memcmp(eth_hdr(skb)->h_dest, p->br->gateway_mac, 6) &&
				memcmp(eth_hdr(skb)->h_dest, "\xff\xff\xff\xff\xff\xff", 6)) {
#ifdef DEBUG_GUEST_ZONE
				GZDEBUG("un-gateway block!\n");
#endif
				return 0;
			}
		}
	}
#endif // CONFIG_RTK_GUEST_ZONE

	return 1;
#else
	return (((p->flags & BR_HAIRPIN_MODE) || skb->dev != p->dev) &&
		br_allowed_egress(p->br, nbp_get_vlan_info(p), skb) &&
		p->state == BR_STATE_FORWARDING);
#endif
}

static inline unsigned int packet_length(const struct sk_buff *skb)
{
	return skb->len - (skb->protocol == htons(ETH_P_8021Q) ? VLAN_HLEN : 0);
}

int br_dev_queue_push_xmit(struct sk_buff *skb)
{
	/* ip_fragment doesn't copy the MAC header */
	if (nf_bridge_maybe_copy_header(skb) ||
	    (packet_length(skb) > skb->dev->mtu && !skb_is_gso(skb))) {
		kfree_skb(skb);
	} else {
		skb_push(skb, ETH_HLEN);
		br_drop_fake_rtable(skb);
		#if	defined(CONFIG_RTL_819X)
		rtl_br_dev_queue_push_xmit_before_xmit_hooks(skb);
		#endif
		dev_queue_xmit(skb);
	}

	return 0;
}

int br_forward_finish(struct sk_buff *skb)
{
	return NF_HOOK(NFPROTO_BRIDGE, NF_BR_POST_ROUTING, skb, NULL, skb->dev,
		       br_dev_queue_push_xmit);

}

static void __br_deliver(const struct net_bridge_port *to, struct sk_buff *skb)
{
	skb = br_handle_vlan(to->br, nbp_get_vlan_info(to), skb);
	if (!skb)
		return;

	skb->dev = to->dev;

	if (unlikely(netpoll_tx_running(to->br->dev))) {
		if (packet_length(skb) > skb->dev->mtu && !skb_is_gso(skb))
			kfree_skb(skb);
		else {
			skb_push(skb, ETH_HLEN);
			br_netpoll_send_skb(to, skb);
		}
		return;
	}

	NF_HOOK(NFPROTO_BRIDGE, NF_BR_LOCAL_OUT, skb, NULL, skb->dev,
		br_forward_finish);
}

static void __br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
{
	struct net_device *indev;

	if (skb_warn_if_lro(skb)) {
		kfree_skb(skb);
		return;
	}

	skb = br_handle_vlan(to->br, nbp_get_vlan_info(to), skb);
	if (!skb)
		return;

	indev = skb->dev;
	skb->dev = to->dev;
	skb_forward_csum(skb);

	NF_HOOK(NFPROTO_BRIDGE, NF_BR_FORWARD, skb, indev, skb->dev,
		br_forward_finish);
}

/* called with rcu_read_lock */
void br_deliver(const struct net_bridge_port *to, struct sk_buff *skb)
{
	if (to && should_deliver(to, skb)) {
		__br_deliver(to, skb);
		return;
	}

	kfree_skb(skb);
}

/* called with rcu_read_lock */
#if defined(CONFIG_RTL_IGMP_SNOOPING)
void br_forward(const struct net_bridge_port *to, struct sk_buff *skb)
{
	if (should_deliver(to, skb)) {
		__br_forward(to, skb);
		return;
	}

	kfree_skb(skb);
}
#else
void br_forward(const struct net_bridge_port *to, struct sk_buff *skb, struct sk_buff *skb0)
{
	if (should_deliver(to, skb)) {
		if (skb0)
			deliver_clone(to, skb, __br_forward);
		else
			__br_forward(to, skb);
		return;
	}

	if (!skb0)
		kfree_skb(skb);
}
#endif
static int deliver_clone(const struct net_bridge_port *prev,
			 struct sk_buff *skb,
			 void (*__packet_hook)(const struct net_bridge_port *p,
					       struct sk_buff *skb))
{
	struct net_device *dev = BR_INPUT_SKB_CB(skb)->brdev;

	skb = skb_clone(skb, GFP_ATOMIC);
	if (!skb) {
		dev->stats.tx_dropped++;
		return -ENOMEM;
	}

	__packet_hook(prev, skb);
	return 0;
}

static struct net_bridge_port *maybe_deliver(
	struct net_bridge_port *prev, struct net_bridge_port *p,
	struct sk_buff *skb,
	void (*__packet_hook)(const struct net_bridge_port *p,
			      struct sk_buff *skb))
{
	int err;

	if (!should_deliver(p, skb))
		return prev;

	if (!prev)
		goto out;

	err = deliver_clone(prev, skb, __packet_hook);
	if (err)
		return ERR_PTR(err);

out:
	return p;
}

/* called under bridge lock */
#if defined(CONFIG_RTL_IGMP_SNOOPING)
static void br_flood(struct net_bridge *br, struct sk_buff *skb,
	void (*__packet_hook)(const struct net_bridge_port *p,
			      struct sk_buff *skb))
{
	struct net_bridge_port *p;
	struct net_bridge_port *prev;
	const unsigned char *dest = eth_hdr(skb)->h_dest;
	prev = NULL;

	list_for_each_entry_rcu(p, &br->port_list, list) {
#ifndef CONFIG_RTK_INBAND_HOST_HACK
		if (should_deliver(p, skb)) {
#else
		if ((should_deliver(p, skb)) && (inband_deliver_check(p, skb))) {
#endif
			/*patch for wan/lan receive duplicate unknown unicast/broadcast packet when pppoe/ipv6 passthrough enable*/
			/*except the packet dmac=33:33:xx:xx:xx:xx*/
			if((strcmp(skb->dev->name,"peth0")==0)&&(!(dest[0]==0x33&&dest[1]==0x33)))
			{
				 if((strncmp(p->dev->name,"eth",3)==0))
				 {
					continue;
				 }
			}

#ifndef CONFIG_RTL_8198C

			/*patch for lan->wan duplicat packet(dmac=33:33:ff:xx:xx:xx) when pppoe/ipv6 passthrough enable*/
			if((strcmp(skb->dev->name,"eth0")==0)&&(
			
			#if defined(CONFIG_RTK_VLAN_NEW_FEATURE)
				(rtk_vlan_support_enable==0)&&
			#endif
				(dest[0]==0x33)&&(dest[1]==0x33)&&(dest[2]==0xff)))
			{
				 if((strncmp(p->dev->name,"peth0",5)==0))
				 {
					continue;
				 }
			}
#endif			
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
#if defined(CONFIG_RTK_VLAN_SUPPORT)
			if (rtk_vlan_support_enable == 0) 
#endif
			{
				if (is_broadcast_ether_addr(dest) || is_unicast_ether_addr(dest)) {
					if ((rtl_is_lan_dev(skb->dev)==TRUE)&&(rtl_is_lan_dev(p->dev)==TRUE))
						continue;
				}
			}
#endif

			if (prev != NULL) {
				struct sk_buff *skb2;

				if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
					LOG_MEM_ERROR("%s(%d) skb clone failed, drop it\n", __FUNCTION__, __LINE__);
					br->dev->stats.tx_dropped++;
					kfree_skb(skb);
					return;
				}

				__packet_hook(prev, skb2);
			}

			prev = p;
		}
	}

	if (prev != NULL) {
		__packet_hook(prev, skb);
		return;
	}

	kfree_skb(skb);
}
#else
static void br_flood(struct net_bridge *br, struct sk_buff *skb,
		     struct sk_buff *skb0,
		     void (*__packet_hook)(const struct net_bridge_port *p,
					   struct sk_buff *skb))
{
	struct net_bridge_port *p;
	struct net_bridge_port *prev;

	prev = NULL;

	list_for_each_entry_rcu(p, &br->port_list, list) {
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
#if defined(CONFIG_RTK_VLAN_SUPPORT)
			if (rtk_vlan_support_enable == 0) 
#endif
			{
				const unsigned char *dest = eth_hdr(skb)->h_dest;
				if (dest && (is_broadcast_ether_addr(dest) || is_unicast_ether_addr(dest))) {
					if ((rtl_is_lan_dev(skb->dev)==TRUE)&&(rtl_is_lan_dev(p->dev)==TRUE))
						continue;
				}
			}
#endif

		prev = maybe_deliver(prev, p, skb, __packet_hook);
		if (IS_ERR(prev))
			goto out;
	}

	if (!prev)
		goto out;

	if (skb0)
		deliver_clone(prev, skb, __packet_hook);
	else
		__packet_hook(prev, skb);
	return;

out:
	if (!skb0)
		kfree_skb(skb);
}
#endif/*CONFIG_RTL_IGMP_SNOOPING*/

/* called with rcu_read_lock */
void br_flood_deliver(struct net_bridge *br, struct sk_buff *skb)
{
#if defined(CONFIG_RTL_IGMP_SNOOPING)
	br_flood(br, skb, __br_deliver);
#else
	br_flood(br, skb, NULL, __br_deliver);
#endif
}

/* called under bridge lock */
#if defined(CONFIG_RTL_IGMP_SNOOPING)
void br_flood_forward(struct net_bridge *br, struct sk_buff *skb)
{
	br_flood(br, skb, __br_forward);
}
#else
void br_flood_forward(struct net_bridge *br, struct sk_buff *skb,
		      struct sk_buff *skb2)
{
	br_flood(br, skb, skb2, __br_forward);
}
#endif/*CONFIG_RTL_IGMP_SNOOPING*/

#if defined(CONFIG_RTL_HARDWARE_MULTICAST)
extern int rtl865x_blockMulticastFlow(unsigned int srcVlanId, unsigned int srcPort,unsigned int srcIpAddr, unsigned int destIpAddr);
extern int rtl865x_curOpMode;


#if defined(CONFIG_BRIDGE_IGMP_SNOOPING)
extern int32 rtl865x_getMcastFwdInfo(struct net_bridge_mdb_entry *mdst,
									  		struct multicastFwdInfo *mcastFwdInfo);

int rtl865x_ipMulticastHardwareAccelerate(struct net_bridge *br,
													 unsigned int srcPort,
													 unsigned int srcVlanId,
													 unsigned int srcIpAddr,
													 struct net_bridge_mdb_entry *mdst)
{
	int ret;
	unsigned int tagged_portmask=0;
	rtl865x_tblDrv_mCast_t * existMulticastEntry;
	struct multicastFwdInfo mcastFwdInfo;
	rtl865x_mcast_fwd_descriptor_t fwdDesc;

	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)!=0)
	{
		return -1;
	}
	
	ret=rtl865x_getMcastFwdInfo(mdst,&mcastFwdInfo);
	if(ret!=SUCCESS)
	{
		return -1;
	}
	memset(&fwdDesc,0,sizeof(rtl865x_mcast_fwd_descriptor_t));
	strcpy(fwdDesc.netifName,RTL_PS_BR0_DEV_NAME);
	fwdDesc.dip=mdst->addr.u.ip4;
	fwdDesc.fwdPortMask=mcastFwdInfo.fwdPortMask;
	fwdDesc.toCpu=mcastFwdInfo.toCpu;
	
	#ifdef HW_MULTICAST_DBG
	printk("%s:%d,srcPort is %d,srcAddr is 0x%x,srcVlanId is %d,destIpAddr is 0x%x\n",__FUNCTION__,__LINE__,srcPort,srcIpAddr,srcVlanId,mdst->addr.u.ip4);
	#endif

	existMulticastEntry=rtl865x_findMCastEntry(fwdDesc.dip,srcIpAddr, (unsigned short)srcVlanId, (unsigned short)srcPort);
	if(existMulticastEntry!=NULL)
	{
		/*it's already in cache */
		#ifdef HW_MULTICAST_DBG
		printk("%s multicast already in cache\n",__FUNCTION__);
		#endif
		return 0;
	}

	if(mcastFwdInfo.fwdPortMask==0)
	{
		//printk("%s:%d,srcPort is %d,srcAddr is 0x%x,srcVlanId is %d,destIpAddr is 0x%x\n",__FUNCTION__,__LINE__,srcPort,srcIpAddr,srcVlanId,mdst->addr.u.ip4);
		rtl865x_blockMulticastFlow(srcVlanId, srcPort, srcIpAddr, fwdDesc.dip);
		return 0;
	}
	
	/*add hardware multicast entry*/

	fwdDesc.fwdPortMask=mcastFwdInfo.fwdPortMask & (~(1<<srcPort));
	
	//printk("%s:%d,srcPort is %d,srcAddr is 0x%x,srcVlanId is %d,destIpAddr is 0x%x,fwdDesc.fwdPortMask:%x\n",__FUNCTION__,__LINE__,srcPort,srcIpAddr,srcVlanId,mdst->addr.u.ip4,fwdDesc.fwdPortMask);
#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
	if(rtl_hw_vlan_ignore_tagged_mc == 1)
		tagged_portmask = rtl_hw_vlan_get_tagged_portmask();
#endif
	if((fwdDesc.fwdPortMask & tagged_portmask) == 0)
	{
		ret=rtl865x_addMulticastEntry(fwdDesc.dip,srcIpAddr, (unsigned short)srcVlanId, (unsigned short)srcPort,
							&fwdDesc, 1, 0, 0, 0);
	}
	return 0;
}

#if defined(CONFIG_RTL_8198C)
int rtl865x_ipv6MulticastHardwareAccelerate(struct net_bridge *br,
													 unsigned int srcPort,
													 unsigned int srcVlanId,
													 unsigned int srcIpAddr,
													 struct net_bridge_mdb_entry *mdst)
{
	int ret;
	unsigned int tagged_portmask=0;
	rtl8198c_tblDrv_mCastv6_t * existMulticastEntry;
	rtl8198c_mcast_fwd_descriptor6_t fwdDesc;
	struct multicastFwdInfo mcastFwdInfo;
	ret=rtl865x_getMcastFwdInfo(mdst,&mcastFwdInfo);
	
	strcpy(fwdDesc.netifName,RTL_PS_BR0_DEV_NAME);
	fwdDesc.dip=mdst->addr.u.ip6.in6_u;
	fwdDesc.fwdPortMask=mcastFwdInfo.fwdPortMask;
	fwdDesc.toCpu=mcastFwdInfo.toCpu;
	fwdDesc.dip=mdst->addr.u.ip4;

	#ifdef HW_MULTICAST_DBG
	printk("%s:%d,srcPort is %d,srcAddr is 0x%x,srcVlanId is %d,destIpAddr is 0x%x\n",__FUNCTION__,__LINE__,srcPort,srcIpAddr,srcVlanId,mdst->addr.u.ip4);
	#endif

	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)!=0)
	{
		return -1;
	}


	existMulticastEntry=rtl8198C_findMCastv6Entry(fwdDesc.dip,srcIpAddr, (unsigned short)srcVlanId, (unsigned short)srcPort);
	if(existMulticastEntry!=NULL)
	{
		/*it's already in cache */
		#ifdef HW_MULTICAST_DBG
		printk("%s multicast already in cache\n",__FUNCTION__);
		#endif
		return 0;

	}

	if(fwdDesc.fwdPortMask==0)
	{
		//printk("%s:%d,srcPort is %d,srcAddr is 0x%x,srcVlanId is %d,destIpAddr is 0x%x\n",__FUNCTION__,__LINE__,srcPort,srcIpAddr,srcVlanId,mdst->addr.u.ip4);
		rtl8198C_blockMulticastv6Flow(srcVlanId,srcPort,srcIpAddr,fwdDesc.dip);
		return 0;
	}
	
	/*add hardware multicast entry*/

	#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)==0)
	{
		memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
		strcpy(fwdDescriptor.netifName,"eth*");
		ret=rtl865x_getFwdDescriptor(br, &fwdDescriptor,ip)
	}
	else if(strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME)==0)
	{
		memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
		ret=rtl865x_getFwdDescriptor(br,&fwdDescriptor,ip);
	}
	#else
	//ret=rtl865x_getFwdDescriptor(br,&fwdDescriptor,ip);
	#endif
	if(ret!=0)
	{
		return -1;
	}
	else
	{
		fwdDesc.fwdPortMask=fwdDesc.fwdPortMask & (~(1<<srcPort));
	}
	
	//printk("%s:%d,srcPort is %d,srcAddr is 0x%x,srcVlanId is %d,destIpAddr is 0x%x,fwdDesc.fwdPortMask:%x\n",__FUNCTION__,__LINE__,srcPort,srcIpAddr,srcVlanId,mdst->addr.u.ip4,fwdDesc.fwdPortMask);
#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
	if(rtl_hw_vlan_ignore_tagged_mc == 1)
		tagged_portmask = rtl_hw_vlan_get_tagged_portmask();
#endif
	if((fwdDesc.fwdPortMask & tagged_portmask) == 0)
	{
		ret=rtl8198C_addMulticastv6Entry(fwdDesc.dip,srcIpAddr,(unsigned short)srcVlanId,(unsigned short)srcPort,
										 &fwdDesc,1,0,0,0);
	}
	return 0;
}

#endif/*CONFIG_RTL_8198C*/
#endif/*CONFIG_BRIDGE_IGMP_SNOOPING*/
#endif/*CONFIG_RTL_HARDWARE_MULTICAST*/

#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
extern int32 rtl865x_getMcastFwdInfo(struct net_bridge_mdb_entry *mdst,
									  		struct multicastFwdInfo *mcastFwdInfo);

extern int rtl865x_blockMulticastFlow(unsigned int srcVlanId, unsigned int srcPort,unsigned int srcIpAddr, unsigned int destIpAddr);
extern int rtl865x_curOpMode;

#define MAX_UNKNOWN_MULTICAST_NUM 16
#define MAX_UNKNOWN_MULTICAST_PPS 1500
#define BLOCK_UNKNOWN_MULTICAST 1

struct rtl865x_unKnownMCastRecord
{
	unsigned int groupAddr;
	unsigned long lastJiffies;
	unsigned long pktCnt;
	unsigned int valid;
};
struct rtl865x_unKnownMCastRecord unKnownMCastRecord[MAX_UNKNOWN_MULTICAST_NUM];
#if 0
int rtl865x_checkUnknownMCastLoading(struct rtl_multicastDataInfo *mCastInfo)
{
	int i;
	if(mCastInfo==NULL)
	{
		return 0;
	}
	/*check entry existed or not*/
	for(i=0; i<MAX_UNKNOWN_MULTICAST_NUM; i++)
	{
		if((unKnownMCastRecord[i].valid==1) && (unKnownMCastRecord[i].groupAddr==mCastInfo->groupAddr[0]))
		{
			break;
		}
	}

	/*find an empty one*/
	if(i==MAX_UNKNOWN_MULTICAST_NUM)
	{
		for(i=0; i<MAX_UNKNOWN_MULTICAST_NUM; i++)
		{
			if(unKnownMCastRecord[i].valid!=1)
			{
				break;
			}
		}
	}

	/*find an exipired one */
	if(i==MAX_UNKNOWN_MULTICAST_NUM)
	{
		for(i=0; i<MAX_UNKNOWN_MULTICAST_NUM; i++)
		{
			if(	time_before(unKnownMCastRecord[i].lastJiffies+HZ,jiffies)
				|| time_after(unKnownMCastRecord[i].lastJiffies,jiffies+HZ)	)
			{
		
				break;
			}
		}
	}

	if(i==MAX_UNKNOWN_MULTICAST_NUM)
	{
		return 0;
	}

	unKnownMCastRecord[i].groupAddr=mCastInfo->groupAddr[0];
	unKnownMCastRecord[i].valid=1;
	
	if(time_after(unKnownMCastRecord[i].lastJiffies+HZ,jiffies))
	{
		unKnownMCastRecord[i].pktCnt++;
	}
	else
	{
		unKnownMCastRecord[i].lastJiffies=jiffies;
		unKnownMCastRecord[i].pktCnt=0;
	}

	if(unKnownMCastRecord[i].pktCnt>MAX_UNKNOWN_MULTICAST_PPS)
	{
		return BLOCK_UNKNOWN_MULTICAST;
	}

	return 0;
}

#endif
int rtl865x_ipMulticastFastFwd(struct sk_buff *skb)
{
#if 0
	int ipMulticastFastFwd=1;
	const unsigned char *dest = NULL;
	unsigned char *ptr;
	struct iphdr *iph=NULL;
	unsigned char proto=0;
	unsigned char reserved=0;
	int ret=-1;

	struct net_bridge_port *prev;
	struct net_bridge_port *p, *n;
//	struct rtl_multicastDataInfo multicastDataInfo;
//	struct rtl_multicastFwdInfo multicastFwdInfo;
	struct sk_buff *skb2;

	unsigned short port_bitmask=0;
//	#if defined (CONFIG_RTL_MLD_SNOOPING)
	struct ipv6hdr * ipv6h=NULL;
//	#endif
	unsigned int fwdCnt;
#if	defined (CONFIG_RTL_IGMP_PROXY)
	struct net_device *dev=skb->dev;
#endif
#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	struct net_bridge *bridge = bridge0;
	unsigned int brSwFwdPortMask = br0SwFwdPortMask;
#endif	
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
	unsigned int srcPort=skb->srcPort;
	unsigned int srcVlanId=skb->srcVlanId;
	rtl865x_tblDrv_mCast_t *existMulticastEntry=NULL;
#endif
	/*check fast forward enable or not*/
	if(ipMulticastFastFwd==0)
	{
		return -1;
	}

	/*check dmac is multicast or not*/
	dest=eth_hdr(skb)->h_dest;
	if((dest[0]&0x01)==0)
	{
		return -1;
	}

	//printk("%s:%d,dest is 0x%x-%x-%x-%x-%x-%x\n",__FUNCTION__,__LINE__,dest[0],dest[1],dest[2],dest[3],dest[4],dest[5]);
	//if(igmpsnoopenabled==0)
	//{
	//	return -1;
	//}

	/*check bridge0 exist or not*/
	if((bridge0==NULL) ||(bridge0->dev->flags & IFF_PROMISC))
	{
		return -1;
	}

	if((skb->dev==NULL) ||(strncmp(skb->dev->name,RTL_PS_BR0_DEV_NAME,3)==0))
	{
		return -1;
	}
    #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if((strncmp(skb->dev->name,RTL_PS_BR1_DEV_NAME,3)==0))
	{
		return -1;
	}
	#endif

	/*check igmp snooping enable or not, and check dmac is ipv4 multicast mac or not*/
	if  ((dest[0]==0x01) && (dest[1]==0x00) && (dest[2]==0x5e))
	{
		//printk("%s:%d,skb->dev->name is %s\n",__FUNCTION__,__LINE__,skb->dev->name );
		ptr=(unsigned char *)eth_hdr(skb)+12;
		/*check vlan tag exist or not*/
		if(*(int16 *)(ptr)==(int16)htons(0x8100))
		{
			ptr=ptr+4;
		}

		/*check it's ipv4 packet or not*/
		if(*(int16 *)(ptr)!=(int16)htons(ETH_P_IP))
		{
			return -1;
		}

		iph=(struct iphdr *)(ptr+2);

		if(iph->daddr== 0xEFFFFFFA)
		{
			/*for microsoft upnp*/
			reserved=1;
		}

		/*only speed up udp and tcp*/
		proto =  iph->protocol;
		//printk("%s:%d,proto is %d\n",__FUNCTION__,__LINE__,proto);
		 if(((proto ==IPPROTO_UDP) ||(proto ==IPPROTO_TCP)) && (reserved ==0))
		{

			#if defined (CONFIG_IP_MROUTE)
			/*multicast data comes from wan, need check multicast forwardig cache*/
			if((strncmp(skb->dev->name,RTL_PS_WAN0_DEV_NAME,4)==0) && needCheckMfc )
			{
				#if	defined (CONFIG_RTL_IGMP_PROXY)
				if(rtl865x_checkMfcCache(&init_net,dev,iph->saddr,iph->daddr)!=0)
				#endif	
				{
					if(rtl865x_checkUnknownMCastLoading(&multicastDataInfo)==BLOCK_UNKNOWN_MULTICAST)
					{
#if defined( CONFIG_RTL865X_HARDWARE_MULTICAST) || defined(CONFIG_RTL865X_LANPORT_RESTRICTION)
						if((skb->srcVlanId!=0) && (skb->srcPort!=0xFFFF))
						{
							rtl865x_blockMulticastFlow(srcVlanId, srcPort, iph->saddr,iph->daddr);
						}
						else
#endif
						{
							kfree_skb(skb);
							return 0;
						}
					}
				
					return -1;
				}
			}
			#endif

			multicastDataInfo.ipVersion=4;
			multicastDataInfo.sourceIp[0]=  (unsigned int)(iph->saddr);
			multicastDataInfo.groupAddr[0]=  (unsigned int)(iph->daddr);

            #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT //fix tim
			if(!strcmp(skb->dev->name,RTL_PS_ETH_NAME_ETH2)){
					ret= rtl_getMulticastDataFwdInfo(brIgmpModuleIndex_2, &multicastDataInfo, &multicastFwdInfo);
					bridge = bridge1;
					brSwFwdPortMask = br1SwFwdPortMask;
			}
			else
		    #endif
			ret= rtl_getMulticastDataFwdInfo(brIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);

			//printk("%s:%d,ret is %d\n",__FUNCTION__,__LINE__,ret);
			 if((ret!=0)||multicastFwdInfo.reservedMCast || multicastFwdInfo.unknownMCast)
			{
				if( multicastFwdInfo.unknownMCast && 
					(strncmp(skb->dev->name,RTL_PS_WAN0_DEV_NAME,4)==0) && 		//only block heavyloading multicast data from wan
					(rtl865x_checkUnknownMCastLoading(&multicastDataInfo)==BLOCK_UNKNOWN_MULTICAST))
				{
#if defined( CONFIG_RTL865X_HARDWARE_MULTICAST) || defined(CONFIG_RTL865X_LANPORT_RESTRICTION)
					if((skb->srcVlanId!=0) && (skb->srcPort!=0xFFFF))
					{
						rtl865x_blockMulticastFlow(srcVlanId, srcPort, iph->saddr,iph->daddr);
					}
					else
#endif
					{
						kfree_skb(skb);
						return 0;
					}
				}
				return -1;
			}


			//printk("%s:%d,br0SwFwdPortMask is 0x%x,multicastFwdInfo.fwdPortMask is 0x%x\n",__FUNCTION__,__LINE__,br0SwFwdPortMask,multicastFwdInfo.fwdPortMask);
			//printk("%s:%d,dip: 0x%x,sip: 0x%x,svid:%x,srcPort:%x\n",__FUNCTION__,__LINE__,iph->daddr,iph->saddr,srcVlanId,srcPort);
			#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
			
			existMulticastEntry=rtl865x_findMCastEntry(iph->daddr, iph->saddr, srcVlanId, srcPort);
			if(	(existMulticastEntry==NULL)||
				((existMulticastEntry!=NULL) && (existMulticastEntry->inAsic)))
			
			{
				
				if((skb->srcVlanId!=0) && (skb->srcPort!=0xFFFF))
				{
					/*multicast data comes from ethernet port*/
					#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
					if( (brSwFwdPortMask & multicastFwdInfo.fwdPortMask)==0)
					#else
					if( (br0SwFwdPortMask & multicastFwdInfo.fwdPortMask)==0)
					#endif
					{
						/*hardware forwarding ,let slow path handle packets trapped to cpu*/
						return -1;
					}
				}
			}
			#endif

			skb_push(skb, ETH_HLEN);

			prev = NULL;
			fwdCnt=0;

        #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
			list_for_each_entry_safe(p, n, &bridge->port_list, list) 
		#else
			list_for_each_entry_safe(p, n, &bridge0->port_list, list)
		#endif
			{
				port_bitmask = (1 << p->port_no);
				if ((port_bitmask & multicastFwdInfo.fwdPortMask) && should_deliver(p, skb))
				{
					if (prev != NULL)
					{
						if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL)
						{
							LOG_MEM_ERROR("%s(%d) skb clone failed, drop it\n", __FUNCTION__, __LINE__);
                            #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
							bridge->dev->stats.tx_dropped++;
			    			#else
							bridge0->dev->stats.tx_dropped++;
							#endif
							kfree_skb(skb);
							return 0;
						}
						skb2->dev=prev->dev;
						//printk("%s:%d,prev->dev->name is %s\n",__FUNCTION__,__LINE__,prev->dev->name);
						#if defined(CONFIG_COMPAT_NET_DEV_OPS)
						prev->dev->hard_start_xmit(skb2, prev->dev);
						#else
						prev->dev->netdev_ops->ndo_start_xmit(skb2,prev->dev);
						#endif
						fwdCnt++;
					}

					prev = p;
				}
			}

			if (prev != NULL)
			{
				skb->dev=prev->dev;
				//printk("%s:%d,prev->dev->name is %s\n",__FUNCTION__,__LINE__,prev->dev->name);
		        #if defined(CONFIG_COMPAT_NET_DEV_OPS)
				prev->dev->hard_start_xmit(skb, prev->dev);
				#else
				prev->dev->netdev_ops->ndo_start_xmit(skb,prev->dev);
				#endif
				fwdCnt++;
			}

			if(fwdCnt==0)
			{
				/*avoid memory leak*/
				skb_pull(skb, ETH_HLEN);
				return -1;
			}

			return 0;

		}

	}

#if 0 //defined (CONFIG_RTL_MLD_SNOOPING)
	/*check igmp snooping enable or not, and check dmac is ipv4 multicast mac or not*/
	if  ((dest[0]==0x33) && (dest[1]==0x33) && (dest[2]!=0xff))
	{
		struct net_bridge_port *p;
		if(mldSnoopEnabled==0)
		{
			return -1;
		}

		/*due to ipv6 passthrough*/
		p= rcu_dereference(skb->dev->br_port);
		if(p==NULL)
		{
			return -1;
		}

		//printk("%s:%d,skb->dev->name is %s\n",__FUNCTION__,__LINE__,skb->dev->name );
		ptr=(unsigned char *)eth_hdr(skb)+12;
		/*check vlan tag exist or not*/
		if(*(int16 *)(ptr)==(int16)htons(0x8100))
		{
			ptr=ptr+4;
		}

		/*check it's ipv6 packet or not*/
		if(*(int16 *)(ptr)!=(int16)htons(ETH_P_IPV6))
		{
			return -1;
		}

		ipv6h=(struct ipv6hdr *)(ptr+2);
		proto =  re865x_getIpv6TransportProtocol(ipv6h);

		//printk("%s:%d,proto is %d\n",__FUNCTION__,__LINE__,proto);
		 if((proto ==IPPROTO_UDP) ||(proto ==IPPROTO_TCP))
		{
			multicastDataInfo.ipVersion=6;
			memcpy(&multicastDataInfo.sourceIp, &ipv6h->saddr, sizeof(struct in6_addr));
			memcpy(&multicastDataInfo.groupAddr, &ipv6h->daddr, sizeof(struct in6_addr));
			/*
			printk("%s:%d,sourceIp is %x-%x-%x-%x\n",__FUNCTION__,__LINE__,
				multicastDataInfo.sourceIp[0],multicastDataInfo.sourceIp[1],multicastDataInfo.sourceIp[2],multicastDataInfo.sourceIp[3]);
			printk("%s:%d,groupAddr is %x-%x-%x-%x\n",__FUNCTION__,__LINE__,
				multicastDataInfo.groupAddr[0],multicastDataInfo.groupAddr[1],multicastDataInfo.groupAddr[2],multicastDataInfo.groupAddr[3]);
			*/
			ret= rtl_getMulticastDataFwdInfo(brIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);
			//printk("%s:%d,ret is %d\n",__FUNCTION__,__LINE__,ret);
			if(ret!=0)
			{
				if(multicastFwdInfo.unknownMCast)
				{
					multicastFwdInfo.fwdPortMask=0xFFFFFFFF;
				}
				else
				{
					return -1;
				}

			}

			//printk("%s:%d,multicastFwdInfo.fwdPortMask is 0x%x\n",__FUNCTION__,__LINE__,multicastFwdInfo.fwdPortMask);

			skb_push(skb, ETH_HLEN);

			prev = NULL;
			fwdCnt=0;
			list_for_each_entry_safe(p, n, &bridge0->port_list, list)
			{
				port_bitmask = (1 << p->port_no);
				if ((port_bitmask & multicastFwdInfo.fwdPortMask) && (skb->dev != p->dev && p->state == BR_STATE_FORWARDING))
				{
					if (prev != NULL)
					{
						if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL)
						{
							kfree_skb(skb);
							return 0;
						}
						skb2->dev=prev->dev;
						//printk("%s:%d,prev->dev->name is %s\n",__FUNCTION__,__LINE__,prev->dev->name);
						#if defined(CONFIG_COMPAT_NET_DEV_OPS)
						prev->dev->hard_start_xmit(skb2, prev->dev);
						#else
						prev->dev->netdev_ops->ndo_start_xmit(skb2,prev->dev);
						#endif
						fwdCnt++;
					}

					prev = p;
				}
			}

			if (prev != NULL)
			{
				skb->dev=prev->dev;
				//printk("%s:%d,prev->dev->name is %s\n",__FUNCTION__,__LINE__,prev->dev->name);
			       #if defined(CONFIG_COMPAT_NET_DEV_OPS)
				prev->dev->hard_start_xmit(skb, prev->dev);
				#else
				prev->dev->netdev_ops->ndo_start_xmit(skb,prev->dev);
				#endif
				fwdCnt++;
			}

			if(fwdCnt==0)
			{
				//printk("%s:%d\n",__FUNCTION__,__LINE__);
				/*avoid memory leak*/
				skb_pull(skb, ETH_HLEN);
				return -1;
			}

			return 0;
		}

	}
#endif
#endif
	return -1;
}


/* called with rcu_read_lock */
static void br_multicast_flood(struct net_bridge_mdb_entry *mdst,
			       struct sk_buff *skb, struct sk_buff *skb0,
			       void (*__packet_hook)(
					const struct net_bridge_port *p,
					struct sk_buff *skb))
{
	struct net_device *dev = BR_INPUT_SKB_CB(skb)->brdev;
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_port *prev = NULL;
	struct net_bridge_port_group *p;
	struct hlist_node *rp;

	rp = rcu_dereference(hlist_first_rcu(&br->router_list));
	p = mdst ? rcu_dereference(mdst->ports) : NULL;
	while (p || rp) {
		struct net_bridge_port *port, *lport, *rport;

		lport = p ? p->port : NULL;
		rport = rp ? hlist_entry(rp, struct net_bridge_port, rlist) :
			     NULL;

		port = (unsigned long)lport > (unsigned long)rport ?
		       lport : rport;

		prev = maybe_deliver(prev, port, skb, __packet_hook);
		if (IS_ERR(prev))
			goto out;

		if ((unsigned long)lport >= (unsigned long)port)
			p = rcu_dereference(p->next);
		if ((unsigned long)rport >= (unsigned long)port)
			rp = rcu_dereference(hlist_next_rcu(rp));
	}

	if (!prev)
		goto out;

	if (skb0)
		deliver_clone(prev, skb, __packet_hook);
	else
		__packet_hook(prev, skb);
	return;

out:
	if (!skb0)
		kfree_skb(skb);
}

/* called with rcu_read_lock */
void br_multicast_deliver(struct net_bridge_mdb_entry *mdst,
			  struct sk_buff *skb)
{
	br_multicast_flood(mdst, skb, NULL, __br_deliver);
}

/* called with rcu_read_lock */
void br_multicast_forward(struct net_bridge_mdb_entry *mdst,
			  struct sk_buff *skb, struct sk_buff *skb2)
{
	br_multicast_flood(mdst, skb, skb2, __br_forward);
}
#endif/*CONFIG_BRIDGE_IGMP_SNOOPING*/

#if defined(CONFIG_RTL_IGMP_SNOOPING)
int bitmask_to_id(unsigned char val)
{
	int i;
	for (i=0; i<8; i++) {
		if (val & (1 <<i))
			break;
	}

	if(i>=8)
	{
		i=7;
	}
	return (i);
}

static void br_multicast(struct net_bridge *br, unsigned int fwdPortMask, struct sk_buff *skb, int clone,
		  void (*__packet_hook)(const struct net_bridge_port *p, struct sk_buff *skb))
{
//	char i;
	struct net_bridge_port *prev;
	struct net_bridge_port *p, *n;
	unsigned short port_bitmask=0;
        if (clone) {
                struct sk_buff *skb2;

                if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
			LOG_MEM_ERROR("%s(%d) skb clone failed, drop it\n", __FUNCTION__, __LINE__);
                        br->dev->stats.tx_dropped++;
                        return;
                }

                skb = skb2;
        }

	prev = NULL;

	list_for_each_entry_safe(p, n, &br->port_list, list) {
		port_bitmask = (1 << p->port_no);
                if ((port_bitmask & fwdPortMask) && should_deliver(p, skb)) {
                        if (prev != NULL) {
                                struct sk_buff *skb2;

                                if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL) {
					LOG_MEM_ERROR("%s(%d) skb clone failed, drop it\n", __FUNCTION__, __LINE__);
                                        br->dev->stats.tx_dropped++;
                                        kfree_skb(skb);
                                        return;
                                }

                                __packet_hook(prev, skb2);
                        }

                        prev = p;
                }
	}

        if (prev != NULL) {
                __packet_hook(prev, skb);
                return;
        }

	kfree_skb(skb);
}

void br_multicast_deliver(struct net_bridge *br, unsigned int fwdPortMask, struct sk_buff *skb, int clone)
{
	br_multicast(br, fwdPortMask, skb, clone, __br_deliver);
}
void br_multicast_forward(struct net_bridge *br, unsigned int fwdPortMask, struct sk_buff *skb, int clone)
{
	br_multicast(br, fwdPortMask, skb, clone, __br_forward);
}

extern struct net_bridge *bridge0;
extern int ipMulticastFastFwd;
extern int needCheckMfc;
#if defined (CONFIG_IP_MROUTE)
#if defined (CONFIG_RTL_IGMP_PROXY)
extern int rtl865x_checkMfcCache(struct net *net,struct net_device *dev,__be32 origin,__be32 mcastgrp);
#endif
#endif

#if defined(CONFIG_RTL_MLD_SNOOPING)
extern int re865x_getIpv6TransportProtocol(struct ipv6hdr* ipv6h);
#endif

extern int rtl865x_blockMulticastFlow(unsigned int srcVlanId, unsigned int srcPort,unsigned int srcIpAddr, unsigned int destIpAddr);
extern int rtl865x_curOpMode;

#define MAX_UNKNOWN_MULTICAST_NUM 16
//#define MAX_UNKNOWN_MULTICAST_PPS 1500
#define BLOCK_UNKNOWN_MULTICAST 1
unsigned int maxUnknownMcastPPS=1500;
unsigned int chkUnknownMcastEnable=1;

struct rtl865x_unKnownMCastRecord
{
	unsigned int groupAddr;
	unsigned long lastJiffies;
	unsigned long pktCnt;
	unsigned int valid;
};
struct rtl865x_unKnownMCastRecord unKnownMCastRecord[MAX_UNKNOWN_MULTICAST_NUM];

int rtl865x_checkUnknownMCastLoading(struct rtl_multicastDataInfo *mCastInfo)
{
	int i;
	
	if (chkUnknownMcastEnable==0)
		return 0;
	
	if(mCastInfo==NULL)
	{
		return 0;
	}
	
	if(rtl_check_ReservedMCastAddr(mCastInfo->groupAddr[0])==SUCCESS)
	{
		return 0;
	}
	/*check entry existed or not*/
	for(i=0; i<MAX_UNKNOWN_MULTICAST_NUM; i++)
	{
		if((unKnownMCastRecord[i].valid==1) && (unKnownMCastRecord[i].groupAddr==mCastInfo->groupAddr[0]))
		{
			break;
		}
	}

	/*find an empty one*/
	if(i==MAX_UNKNOWN_MULTICAST_NUM)
	{
		for(i=0; i<MAX_UNKNOWN_MULTICAST_NUM; i++)
		{
			if(unKnownMCastRecord[i].valid!=1)
			{
				break;
			}
		}
	}

	/*find an exipired one */
	if(i==MAX_UNKNOWN_MULTICAST_NUM)
	{
		for(i=0; i<MAX_UNKNOWN_MULTICAST_NUM; i++)
		{
			if(	time_before(unKnownMCastRecord[i].lastJiffies+HZ,jiffies)
				|| time_after(unKnownMCastRecord[i].lastJiffies,jiffies+HZ)	)
			{
		
				break;
			}
		}
	}

	if(i==MAX_UNKNOWN_MULTICAST_NUM)
	{
		return 0;
	}

	unKnownMCastRecord[i].groupAddr=mCastInfo->groupAddr[0];
	unKnownMCastRecord[i].valid=1;
	
	if(time_after(unKnownMCastRecord[i].lastJiffies+HZ,jiffies))
	{
		unKnownMCastRecord[i].pktCnt++;
	}
	else
	{
		unKnownMCastRecord[i].lastJiffies=jiffies;
		unKnownMCastRecord[i].pktCnt=0;
	}

	if(unKnownMCastRecord[i].pktCnt>maxUnknownMcastPPS)
	{
		return BLOCK_UNKNOWN_MULTICAST;
	}

	return 0;
}
int rtl865x_ipMulticastFastFwd(struct sk_buff *skb)
{
	const unsigned char *dest = NULL;
	unsigned char *ptr;
	struct iphdr *iph=NULL;
	unsigned char proto=0;
	unsigned char reserved=0;
	int ret=-1;

	struct net_bridge_port *prev;
	struct net_bridge_port *p, *n;
	struct rtl_multicastDataInfo multicastDataInfo;
	struct rtl_multicastFwdInfo multicastFwdInfo;
	struct sk_buff *skb2;

	unsigned short port_bitmask=0;
	#if defined (CONFIG_RTL_MLD_SNOOPING)
	struct ipv6hdr * ipv6h=NULL;
	#endif
	unsigned int fwdCnt;
#if	defined (CONFIG_RTL_IGMP_PROXY)
	struct net_device *dev=skb->dev;
#endif
#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	struct net_bridge *bridge = bridge0;
	unsigned int brSwFwdPortMask = br0SwFwdPortMask;
#endif	
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
	unsigned int srcPort=skb->srcPort;
	unsigned int srcVlanId=skb->srcVlanId;
	rtl865x_tblDrv_mCast_t *existMulticastEntry=NULL;
#endif
	/*check fast forward enable or not*/
	if(ipMulticastFastFwd==0)
	{
		return -1;
	}

	/*check dmac is multicast or not*/
	dest=eth_hdr(skb)->h_dest;
	if((dest[0]&0x01)==0)
	{
		return -1;
	}

	//printk("%s:%d,dest is 0x%x-%x-%x-%x-%x-%x\n",__FUNCTION__,__LINE__,dest[0],dest[1],dest[2],dest[3],dest[4],dest[5]);
	if(igmpsnoopenabled==0)
	{
		return -1;
	}

	/*check bridge0 exist or not*/
	if((bridge0==NULL) ||(bridge0->dev->flags & IFF_PROMISC))
	{
		return -1;
	}

	if((skb->dev==NULL) ||(strncmp(skb->dev->name,RTL_PS_BR0_DEV_NAME,3)==0))
	{
		return -1;
	}
    #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if((strncmp(skb->dev->name,RTL_PS_BR1_DEV_NAME,3)==0))
	{
		return -1;
	}
	#endif

	/*check igmp snooping enable or not, and check dmac is ipv4 multicast mac or not*/
	if  ((dest[0]==0x01) && (dest[1]==0x00) && (dest[2]==0x5e))
	{
		//printk("%s:%d,skb->dev->name is %s\n",__FUNCTION__,__LINE__,skb->dev->name );
		ptr=(unsigned char *)eth_hdr(skb)+12;
		/*check vlan tag exist or not*/
		if(*(int16 *)(ptr)==(int16)htons(0x8100))
		{
			ptr=ptr+4;
		}

		/*check it's ipv4 packet or not*/
		if(*(int16 *)(ptr)!=(int16)htons(ETH_P_IP))
		{
			return -1;
		}

		iph=(struct iphdr *)(ptr+2);

		if((iph->daddr== 0xEFFFFFFA)||
		(rtl_check_ReservedMCastAddr(iph->daddr)==SUCCESS))
		{
			/*for microsoft upnp*/
			reserved=1;
		}

		/*only speed up udp and tcp*/
		proto =  iph->protocol;
		//printk("%s:%d,proto is %d\n",__FUNCTION__,__LINE__,proto);
		 if(((proto ==IPPROTO_UDP) ||(proto ==IPPROTO_TCP)) && (reserved ==0))
		{

			#if defined (CONFIG_IP_MROUTE)
			/*multicast data comes from wan, need check multicast forwardig cache*/
			if((strncmp(skb->dev->name,RTL_PS_WAN0_DEV_NAME,4)==0) && needCheckMfc )
			{
				#if	defined (CONFIG_RTL_IGMP_PROXY)
				if(rtl865x_checkMfcCache(&init_net,dev,iph->saddr,iph->daddr)!=0)
				#endif	
				{
					if(rtl865x_checkUnknownMCastLoading(&multicastDataInfo)==BLOCK_UNKNOWN_MULTICAST)
					{
#if defined( CONFIG_RTL865X_HARDWARE_MULTICAST) || defined(CONFIG_RTL865X_LANPORT_RESTRICTION)
						if((skb->srcVlanId!=0) && (skb->srcPort!=0xFFFF))
						{
							rtl865x_blockMulticastFlow(srcVlanId, srcPort, iph->saddr,iph->daddr);
						}
						else
#endif
						{
							kfree_skb(skb);
							return 0;
						}
					}
				
					return -1;
				}
			}
			#endif

			multicastDataInfo.ipVersion=4;
			multicastDataInfo.sourceIp[0]=  (unsigned int)(iph->saddr);
			multicastDataInfo.groupAddr[0]=  (unsigned int)(iph->daddr);

            #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT //fix tim
			if(!strcmp(skb->dev->name,RTL_PS_ETH_NAME_ETH2)){
					ret= rtl_getMulticastDataFwdInfo(brIgmpModuleIndex_2, &multicastDataInfo, &multicastFwdInfo);
					bridge = bridge1;
					brSwFwdPortMask = br1SwFwdPortMask;
			}
			else
		    #endif
			ret= rtl_getMulticastDataFwdInfo(brIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);

			//printk("%s:%d,ret is %d\n",__FUNCTION__,__LINE__,ret);
			 if((ret!=0)||multicastFwdInfo.reservedMCast || multicastFwdInfo.unknownMCast)
			{
				if( multicastFwdInfo.unknownMCast && 
					(strncmp(skb->dev->name,RTL_PS_WAN0_DEV_NAME,4)==0) && 		//only block heavyloading multicast data from wan
					(rtl865x_checkUnknownMCastLoading(&multicastDataInfo)==BLOCK_UNKNOWN_MULTICAST))
				{
#if defined( CONFIG_RTL865X_HARDWARE_MULTICAST) || defined(CONFIG_RTL865X_LANPORT_RESTRICTION)
					if((skb->srcVlanId!=0) && (skb->srcPort!=0xFFFF))
					{
						rtl865x_blockMulticastFlow(srcVlanId, srcPort, iph->saddr,iph->daddr);
					}
					else
#endif
					{
						kfree_skb(skb);
						return 0;
					}
				}
				return -1;
			}


			//printk("%s:%d,br0SwFwdPortMask is 0x%x,multicastFwdInfo.fwdPortMask is 0x%x\n",__FUNCTION__,__LINE__,br0SwFwdPortMask,multicastFwdInfo.fwdPortMask);
			//printk("%s:%d,dip: 0x%x,sip: 0x%x,svid:%x,srcPort:%x\n",__FUNCTION__,__LINE__,iph->daddr,iph->saddr,srcVlanId,srcPort);
			#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
			
			existMulticastEntry=rtl865x_findMCastEntry(iph->daddr, iph->saddr, srcVlanId, srcPort);
			if(	(existMulticastEntry==NULL)||
				((existMulticastEntry!=NULL) && (existMulticastEntry->inAsic)))
			
			{
				
				if((skb->srcVlanId!=0) && (skb->srcPort!=0xFFFF))
				{
					/*multicast data comes from ethernet port*/
					#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
					if( (brSwFwdPortMask & multicastFwdInfo.fwdPortMask)==0)
					#else
					if( (br0SwFwdPortMask & multicastFwdInfo.fwdPortMask)==0)
					#endif
					{
						/*hardware forwarding ,let slow path handle packets trapped to cpu*/
						return -1;
					}
				}
			}
			#endif

			skb_push(skb, ETH_HLEN);

			prev = NULL;
			fwdCnt=0;

        #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
			list_for_each_entry_safe(p, n, &bridge->port_list, list) 
		#else
			list_for_each_entry_safe(p, n, &bridge0->port_list, list)
		#endif
			{
				port_bitmask = (1 << p->port_no);
				if ((port_bitmask & multicastFwdInfo.fwdPortMask) && should_deliver(p, skb))
				{
					if (prev != NULL)
					{
						if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL)
						{
							LOG_MEM_ERROR("%s(%d) skb clone failed, drop it\n", __FUNCTION__, __LINE__);
                            #ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
							bridge->dev->stats.tx_dropped++;
			    			#else
							bridge0->dev->stats.tx_dropped++;
							#endif
							kfree_skb(skb);
							return 0;
						}
						skb2->dev=prev->dev;
						//printk("%s:%d,prev->dev->name is %s\n",__FUNCTION__,__LINE__,prev->dev->name);
						#if defined(CONFIG_COMPAT_NET_DEV_OPS)
						prev->dev->hard_start_xmit(skb2, prev->dev);
						#else
						prev->dev->netdev_ops->ndo_start_xmit(skb2,prev->dev);
						#endif
						fwdCnt++;
					}

					prev = p;
				}
			}

			if (prev != NULL)
			{
				skb->dev=prev->dev;
				//printk("%s:%d,prev->dev->name is %s\n",__FUNCTION__,__LINE__,prev->dev->name);
		        #if defined(CONFIG_COMPAT_NET_DEV_OPS)
				prev->dev->hard_start_xmit(skb, prev->dev);
				#else
				prev->dev->netdev_ops->ndo_start_xmit(skb,prev->dev);
				#endif
				fwdCnt++;
			}

			if(fwdCnt==0)
			{
				/*avoid memory leak*/
				skb_pull(skb, ETH_HLEN);
				return -1;
			}

			return 0;

		}

	}

#if 0 //defined (CONFIG_RTL_MLD_SNOOPING)
	/*check igmp snooping enable or not, and check dmac is ipv4 multicast mac or not*/
	if  ((dest[0]==0x33) && (dest[1]==0x33) && (dest[2]!=0xff))
	{
		struct net_bridge_port *p;
		if(mldSnoopEnabled==0)
		{
			return -1;
		}

		/*due to ipv6 passthrough*/
		p= rcu_dereference(skb->dev->br_port);
		if(p==NULL)
		{
			return -1;
		}

		//printk("%s:%d,skb->dev->name is %s\n",__FUNCTION__,__LINE__,skb->dev->name );
		ptr=(unsigned char *)eth_hdr(skb)+12;
		/*check vlan tag exist or not*/
		if(*(int16 *)(ptr)==(int16)htons(0x8100))
		{
			ptr=ptr+4;
		}

		/*check it's ipv6 packet or not*/
		if(*(int16 *)(ptr)!=(int16)htons(ETH_P_IPV6))
		{
			return -1;
		}

		ipv6h=(struct ipv6hdr *)(ptr+2);
		proto =  re865x_getIpv6TransportProtocol(ipv6h);

		//printk("%s:%d,proto is %d\n",__FUNCTION__,__LINE__,proto);
		 if((proto ==IPPROTO_UDP) ||(proto ==IPPROTO_TCP))
		{
			multicastDataInfo.ipVersion=6;
			memcpy(&multicastDataInfo.sourceIp, &ipv6h->saddr, sizeof(struct in6_addr));
			memcpy(&multicastDataInfo.groupAddr, &ipv6h->daddr, sizeof(struct in6_addr));
			/*
			printk("%s:%d,sourceIp is %x-%x-%x-%x\n",__FUNCTION__,__LINE__,
				multicastDataInfo.sourceIp[0],multicastDataInfo.sourceIp[1],multicastDataInfo.sourceIp[2],multicastDataInfo.sourceIp[3]);
			printk("%s:%d,groupAddr is %x-%x-%x-%x\n",__FUNCTION__,__LINE__,
				multicastDataInfo.groupAddr[0],multicastDataInfo.groupAddr[1],multicastDataInfo.groupAddr[2],multicastDataInfo.groupAddr[3]);
			*/
			ret= rtl_getMulticastDataFwdInfo(brIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);
			//printk("%s:%d,ret is %d\n",__FUNCTION__,__LINE__,ret);
			if(ret!=0)
			{
				if(multicastFwdInfo.unknownMCast)
				{
					multicastFwdInfo.fwdPortMask=0xFFFFFFFF;
				}
				else
				{
					return -1;
				}

			}

			//printk("%s:%d,multicastFwdInfo.fwdPortMask is 0x%x\n",__FUNCTION__,__LINE__,multicastFwdInfo.fwdPortMask);

			skb_push(skb, ETH_HLEN);

			prev = NULL;
			fwdCnt=0;
			list_for_each_entry_safe(p, n, &bridge0->port_list, list)
			{
				port_bitmask = (1 << p->port_no);
				if ((port_bitmask & multicastFwdInfo.fwdPortMask) && (skb->dev != p->dev && p->state == BR_STATE_FORWARDING))
				{
					if (prev != NULL)
					{
						if ((skb2 = skb_clone(skb, GFP_ATOMIC)) == NULL)
						{
							kfree_skb(skb);
							return 0;
						}
						skb2->dev=prev->dev;
						//printk("%s:%d,prev->dev->name is %s\n",__FUNCTION__,__LINE__,prev->dev->name);
						#if defined(CONFIG_COMPAT_NET_DEV_OPS)
						prev->dev->hard_start_xmit(skb2, prev->dev);
						#else
						prev->dev->netdev_ops->ndo_start_xmit(skb2,prev->dev);
						#endif
						fwdCnt++;
					}

					prev = p;
				}
			}

			if (prev != NULL)
			{
				skb->dev=prev->dev;
				//printk("%s:%d,prev->dev->name is %s\n",__FUNCTION__,__LINE__,prev->dev->name);
			       #if defined(CONFIG_COMPAT_NET_DEV_OPS)
				prev->dev->hard_start_xmit(skb, prev->dev);
				#else
				prev->dev->netdev_ops->ndo_start_xmit(skb,prev->dev);
				#endif
				fwdCnt++;
			}

			if(fwdCnt==0)
			{
				//printk("%s:%d\n",__FUNCTION__,__LINE__);
				/*avoid memory leak*/
				skb_pull(skb, ETH_HLEN);
				return -1;
			}

			return 0;
		}

	}
#endif/*if 0*/

	return -1;
}

//#endif

#endif/*CONFIG_RTL_IGMP_SNOOPING*/

