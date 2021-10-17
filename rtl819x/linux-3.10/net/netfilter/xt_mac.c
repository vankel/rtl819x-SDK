/* Kernel module to match MAC address parameters. */

/* (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2004 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/etherdevice.h>

#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv6.h>
#include <linux/netfilter/xt_mac.h>
#include <linux/netfilter/x_tables.h>

#if defined(CONFIG_RTL_819X)
#include <net/dst.h>
#include <net/route.h>
#include <net/arp.h>
#if defined(CONFIG_RTL_IPTABLES_RULE_2_ACL)
#include <net/rtl/rtl_types.h>
#include <net/rtl/rtl865x_netif.h>
#endif
#endif

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Netfilter Core Team <coreteam@netfilter.org>");
MODULE_DESCRIPTION("Xtables: MAC address match");
MODULE_ALIAS("ipt_mac");
MODULE_ALIAS("ip6t_mac");

#if defined(CONFIG_RTL_819X)
#if defined(CONFIG_RTL_IPTABLES_RULE_2_ACL)
static int mac_match2acl(const char *tablename,
			  const void *ip,
			  const struct xt_match *match,
			  void *matchinfo,
			  void *acl_rule,
			  unsigned int *invflags)
{
	const struct xt_mac_info *info = matchinfo;
	rtl865x_AclRule_t *rule = (rtl865x_AclRule_t *)acl_rule;
	if(matchinfo == NULL || rule == NULL)
		return 1;

	rule->ruleType_ = RTL865X_ACL_MAC;

	//To initial first
	memset(rule->srcMac_.octet, 0, ETH_ALEN);
	memset(rule->srcMacMask_.octet, 0, ETH_ALEN);
	memset(rule->dstMac_.octet, 0, ETH_ALEN);
	memset(rule->dstMacMask_.octet, 0, ETH_ALEN);

	if (info->flags & MAC_SRC) {
		memcpy(rule->srcMac_.octet, info->srcaddr.macaddr, ETH_ALEN);
		memset(rule->srcMacMask_.octet, 0xff, ETH_ALEN);
	}

	if (info->flags & MAC_DST) {
		memcpy(rule->dstMac_.octet, info->dstaddr.macaddr, ETH_ALEN);
		memset(rule->dstMacMask_.octet, 0xff, ETH_ALEN);
	}
	
	rule->typeLen_ = rule->typeLenMask_ = 0;
	
	return 0;
}
#endif

static int compare_with_header_cache_dest_mac(const struct sk_buff *skb, unsigned char *macaddr)
{
	struct dst_entry *dst = skb_dst(skb);
	struct rtable *rt = (struct rtable *)dst;
	int ret = 0;
	struct neighbour *neigh = NULL;
	struct hh_cache *hh = NULL;
	unsigned int nexthop;

	if(!compare_ether_addr(eth_hdr(skb)->h_dest, macaddr))
	{
		ret=1;
	}
	else
	{
	if (rt==NULL || ip_hdr(skb)==NULL)
		return 0;
	
	nexthop = (__force u32) rt_nexthop(rt, ip_hdr(skb)->daddr);
	neigh = __ipv4_neigh_lookup_noref(dst->dev, nexthop);

	if (neigh != NULL)
		hh = &(neigh->hh);
	
	if (neigh&&hh&&(neigh->nud_state&NUD_CONNECTED)&&(hh->hh_len))
	{
	    	//if (hh && (hh->hh_type==ETH_P_IP || hh->hh_type==ETH_P_IPV6))
		{
	    		read_lock_bh(&hh->hh_lock);
	      		memcpy(skb->data - 16, hh->hh_data, 16);
	      		if (memcmp((((u8*)hh->hh_data) + 2), macaddr, ETH_ALEN) == 0)
	      		    ret = 1;
	    		read_unlock_bh(&hh->hh_lock);
	    	}
	}
	}
	
	return ret;
}

static bool mac_mt(const struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_mac_info *info = par->matchinfo;   

	#if defined(CONFIG_RTL_MAC_FILTER_CARE_INPORT)
	if((info->flags&INPORT_FLAG) && !((1<<(skb->srcPhyPort)) & info->inPortMask)){
		return 0;
	}
	#endif
	
	if (info->flags & MAC_SRC) {
	     /* Is mac pointer valid? */
	    if ((skb_mac_header(skb) >= skb->head
		    && (skb_mac_header(skb) + ETH_HLEN) <= skb->data
		    /* If so, compare... */
		    && ((!compare_ether_addr(eth_hdr(skb)->h_source, info->srcaddr.macaddr))
			^ !!(info->flags & MAC_SRC_INV)))==0)
	    	{
			return 0;
	    	}
	}

	if (info->flags & MAC_DST) {		
	     /* Is mac pointer valid? */
	    if( (skb_mac_header(skb) >= skb->head
		    && (skb_mac_header(skb) + ETH_HLEN) <= skb->data
		    /* If so, compare... */
		    && (compare_with_header_cache_dest_mac(skb, (unsigned char*)(info->dstaddr.macaddr)) ^ !!(info->flags & MAC_DST_INV)))==0)
	    	{
			return 0;
	    	}
	}

	return 1;

}
#else
static bool mac_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_mac_info *info = par->matchinfo;
	bool ret;

	#if defined(CONFIG_RTL_MAC_FILTER_CARE_INPORT)
	//if((info->flags&INPORT_FLAG) && !((1<<(skb->srcPhyPort)) & info->inPortMask)){
	if(!((1<<(skb->srcPhyPort)) & info->inPortMask)){
		return 0;
	}
	#endif

	if (skb->dev == NULL || skb->dev->type != ARPHRD_ETHER)
		return false;
	if (skb_mac_header(skb) < skb->head)
		return false;
	if (skb_mac_header(skb) + ETH_HLEN > skb->data)
		return false;
	ret  = ether_addr_equal(eth_hdr(skb)->h_source, info->srcaddr);
	ret ^= info->invert;
	return ret;
}
#endif

static struct xt_match mac_mt_reg __read_mostly = {
	.name      = "mac",
	.revision  = 0,
	.family    = NFPROTO_UNSPEC,
	.match     = mac_mt,
	.matchsize = sizeof(struct xt_mac_info),
#ifndef CONFIG_RTL_819X	
	.hooks     = (1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_LOCAL_IN) |
	             (1 << NF_INET_FORWARD),
#endif     
	.me        = THIS_MODULE,
#if defined(CONFIG_RTL_IPTABLES_RULE_2_ACL)
	.match2acl	= mac_match2acl,
#endif
};

static int __init mac_mt_init(void)
{
	return xt_register_match(&mac_mt_reg);
}

static void __exit mac_mt_exit(void)
{
	xt_unregister_match(&mac_mt_reg);
}

module_init(mac_mt_init);
module_exit(mac_mt_exit);
