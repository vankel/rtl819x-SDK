/*
 *	Device handling code
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

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/netpoll.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/list.h>
#include <linux/netfilter_bridge.h>

#include <asm/uaccess.h>
#include "br_private.h"

#if defined (CONFIG_RTL_IGMP_SNOOPING)
/*2008-01-15,for porting igmp snooping to linux kernel 2.6*/
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/igmp.h>
#include <net/checksum.h>
#include <net/rtl/rtl865x_igmpsnooping_glue.h>
#include <net/rtl/rtl865x_igmpsnooping.h>
extern int igmpsnoopenabled;
#if defined (CONFIG_RTL_MLD_SNOOPING)
#include <linux/ipv6.h>
#include <linux/in6.h>
extern int mldSnoopEnabled;
#include <net/rtl/rtl_types.h>
#endif
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
#include <net/rtl/rtl865x_multicast.h>
#include <net/rtl/rtl865x_netif.h>
#include <net/rtl/rtl_nic.h>
#endif
extern unsigned int br0SwFwdPortMask;
extern unsigned int brIgmpModuleIndex;
extern unsigned int nicIgmpModuleIndex;
#endif

#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
extern uint32 rtl_hw_vlan_get_tagged_portmask(void);
#endif

#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
extern unsigned int br1SwFwdPortMask;
extern unsigned int nicIgmpModuleIndex_2;
#endif

#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
#if defined(CONFIG_RTL_IGMP_SNOOPING)
int rtl865x_ipMulticastHardwareAccelerate(struct net_bridge *br, unsigned int brFwdPortMask,
												unsigned int srcPort,unsigned int srcVlanId, 
												unsigned int srcIpAddr, unsigned int destIpAddr);
#if defined(CONFIG_RTL_8198C)
int rtl865x_ipv6MulticastHardwareAccelerate(struct net_bridge *br, unsigned int brFwdPortMask,
												unsigned int srcPort,unsigned int srcVlanId, 
												struct in6_addr srcIpAddr,struct in6_addr destIpAddr);
#endif
#endif
#endif

#if defined (CONFIG_RTL_MLD_SNOOPING)
extern int re865x_getIpv6TransportProtocol(struct ipv6hdr* ipv6h);
#endif

/* net device transmit always called with BH disabled */
netdev_tx_t br_dev_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);
	const unsigned char *dest = skb->data;
	struct net_bridge_fdb_entry *dst;
	struct net_bridge_mdb_entry *mdst;
	struct br_cpu_netstats *brstats = this_cpu_ptr(br->stats);
	u16 vid = 0;
#if defined(CONFIG_RTL_HARDWARE_MULTICAST)&&defined(CONFIG_RTL_8198C)
	int i = 0;
#endif
#if defined (CONFIG_RTL_IGMP_SNOOPING)	
		struct iphdr *iph=NULL;
		unsigned char proto=0;
		unsigned char reserved=0;
#if defined (CONFIG_RTL_MLD_SNOOPING) 	
		struct ipv6hdr *ipv6h=NULL;
#endif
		struct rtl_multicastDataInfo multicastDataInfo;
		struct rtl_multicastFwdInfo multicastFwdInfo;
		int ret=FAILED;
#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
		unsigned int srcPort=skb->srcPort;
		unsigned int srcVlanId=skb->srcVlanId;
#endif/*CONFIG_RTL_HARDWARE_MULTICAST*/
#endif/*CONFIG_RTL_IGMP_SNOOPING*/	

	rcu_read_lock();
#ifdef CONFIG_BRIDGE_NETFILTER
	if (skb->nf_bridge && (skb->nf_bridge->mask & BRNF_BRIDGED_DNAT)) {
		br_nf_pre_routing_finish_bridge_slow(skb);
		rcu_read_unlock();
		return NETDEV_TX_OK;
	}
#endif

	u64_stats_update_begin(&brstats->syncp);
	brstats->tx_packets++;
	brstats->tx_bytes += skb->len;
	u64_stats_update_end(&brstats->syncp);

	if (!br_allowed_ingress(br, br_get_vlan_info(br), skb, &vid))
		goto out;

	BR_INPUT_SKB_CB(skb)->brdev = dev;

	skb_reset_mac_header(skb);
	skb_pull(skb, ETH_HLEN);
#if defined(CONFIG_RTL_IGMP_SNOOPING)
		if (dest[0] & 1)
		{
			if(igmpsnoopenabled) 
			{	
				if(MULTICAST_MAC(dest))
				{
					
					iph=(struct iphdr *)skb_network_header(skb);
					proto =  iph->protocol;
				#if 0
					if(( iph->daddr&0xFFFFFF00)==0xE0000000)
					{
							reserved=1;
					}
				#endif
		
				#if defined(CONFIG_USB_UWIFI_HOST)
						if(iph->daddr == 0xEFFFFFFA || iph->daddr == 0xE1010101)
				#else
						if(iph->daddr == 0xEFFFFFFA)
				#endif
						{
							/*for microsoft upnp*/
							reserved=1;
						}
						
						if(((proto ==IPPROTO_UDP) ||(proto ==IPPROTO_TCP))	&& (reserved ==0))
						{
							multicastDataInfo.ipVersion=4;
							multicastDataInfo.sourceIp[0]=	(uint32)(iph->saddr);
							multicastDataInfo.groupAddr[0]=  (uint32)(iph->daddr);
							ret= rtl_getMulticastDataFwdInfo(brIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);
					
							br_multicast_deliver(br, multicastFwdInfo.fwdPortMask, skb, 0);
							if((ret==SUCCESS) && (multicastFwdInfo.cpuFlag==0))
							{
						#if defined (CONFIG_RTL_HARDWARE_MULTICAST)
								if((srcVlanId!=0) && (srcPort!=0xFFFF))
								{
							#if defined(CONFIG_RTK_VLAN_SUPPORT)
									if(rtk_vlan_support_enable == 0)
									{
										rtl865x_ipMulticastHardwareAccelerate(br, multicastFwdInfo.fwdPortMask,srcPort,srcVlanId, multicastDataInfo.sourceIp[0], multicastDataInfo.groupAddr[0]);
									}
							#else
									rtl865x_ipMulticastHardwareAccelerate(br, multicastFwdInfo.fwdPortMask,srcPort,srcVlanId, multicastDataInfo.sourceIp[0], multicastDataInfo.groupAddr[0]);
							#endif
								}
						#endif		
							}
						
						}
						else
						{
							br_flood_deliver(br, skb);
						}
		
						
					}
#if defined(CONFIG_RTL_MLD_SNOOPING)	
					else if(mldSnoopEnabled && IPV6_MULTICAST_MAC(dest))
					{
						ipv6h=(struct ipv6hdr *)skb_network_header(skb);
						proto=re865x_getIpv6TransportProtocol(ipv6h);
						if ((proto ==IPPROTO_UDP) ||(proto ==IPPROTO_TCP))
						{
							multicastDataInfo.ipVersion=6;
							memcpy(&multicastDataInfo.sourceIp, &ipv6h->saddr, sizeof(struct in6_addr));
							memcpy(&multicastDataInfo.groupAddr, &ipv6h->daddr, sizeof(struct in6_addr));
							ret= rtl_getMulticastDataFwdInfo(brIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);
							br_multicast_deliver(br, multicastFwdInfo.fwdPortMask, skb, 0);
							#if defined (CONFIG_RTL_HARDWARE_MULTICAST) && defined(CONFIG_RTL_8198C)
							if((ret==SUCCESS) && (multicastFwdInfo.cpuFlag==0))
							{
								struct in6_addr sip;
								struct in6_addr dip;
								memcpy(&sip,multicastDataInfo.sourceIp,sizeof(struct in6_addr));
								memcpy(&dip,multicastDataInfo.groupAddr,sizeof(struct in6_addr));
								if((srcVlanId!=0) && (srcPort!=0xFFFF))
								{
								#if defined(CONFIG_RTK_VLAN_SUPPORT)
									if(rtk_vlan_support_enable == 0)
									{
										rtl865x_ipv6MulticastHardwareAccelerate(br, multicastFwdInfo.fwdPortMask,srcPort,srcVlanId, sip, dip);
									}
									#else
									rtl865x_ipv6MulticastHardwareAccelerate(br, multicastFwdInfo.fwdPortMask,srcPort,srcVlanId, sip, dip);
								#endif
								}
							}
							#endif		
						}
						else
						{
							br_flood_deliver(br, skb);
						}
					}
#endif/*CONFIG_RTL_MLD_SNOOPING*/		
					else
					{
						br_flood_deliver(br, skb);
					}
				
				}
				else
				{ 
					br_flood_deliver(br, skb);
				}	
			}
		else if ((dst = __br_fdb_get(br, dest,vid)) != NULL)
			br_deliver(dst->dst, skb);
		else
			br_flood_deliver(br, skb);
		
#else/*CONFIG_RTL_IGMP_SNOOPING*/
	if (is_broadcast_ether_addr(dest))
		br_flood_deliver(br, skb);
	else if (is_multicast_ether_addr(dest)) {
		if (unlikely(netpoll_tx_running(dev))) {
			br_flood_deliver(br, skb);
			goto out;
		}
		if (br_multicast_rcv(br, NULL, skb)) {
			kfree_skb(skb);
			goto out;
		}

		mdst = br_mdb_get(br, skb, vid);
		if (mdst || BR_INPUT_SKB_CB_MROUTERS_ONLY(skb))
			br_multicast_deliver(mdst, skb);
		else
			br_flood_deliver(br, skb);
	} else if ((dst = __br_fdb_get(br, dest, vid)) != NULL)
		br_deliver(dst->dst, skb);
	else
		br_flood_deliver(br, skb);
#endif/*CONFIG_RTL_IGMP_SNOOPING*/
out:
	rcu_read_unlock();
	return NETDEV_TX_OK;
}

static int br_dev_init(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	br->stats = alloc_percpu(struct br_cpu_netstats);
	if (!br->stats)
		return -ENOMEM;

	return 0;
}

static int br_dev_open(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	netdev_update_features(dev);
	netif_start_queue(dev);
	br_stp_enable_bridge(br);
	br_multicast_open(br);

	return 0;
}

static void br_dev_set_multicast_list(struct net_device *dev)
{
}

static int br_dev_stop(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	br_stp_disable_bridge(br);
	br_multicast_stop(br);

	netif_stop_queue(dev);

	return 0;
}

static struct rtnl_link_stats64 *br_get_stats64(struct net_device *dev,
						struct rtnl_link_stats64 *stats)
{
	struct net_bridge *br = netdev_priv(dev);
	struct br_cpu_netstats tmp, sum = { 0 };
	unsigned int cpu;

	for_each_possible_cpu(cpu) {
		unsigned int start;
		const struct br_cpu_netstats *bstats
			= per_cpu_ptr(br->stats, cpu);
		do {
			start = u64_stats_fetch_begin_bh(&bstats->syncp);
			memcpy(&tmp, bstats, sizeof(tmp));
		} while (u64_stats_fetch_retry_bh(&bstats->syncp, start));
		sum.tx_bytes   += tmp.tx_bytes;
		sum.tx_packets += tmp.tx_packets;
		sum.rx_bytes   += tmp.rx_bytes;
		sum.rx_packets += tmp.rx_packets;
	}

	stats->tx_bytes   = sum.tx_bytes;
	stats->tx_packets = sum.tx_packets;
	stats->rx_bytes   = sum.rx_bytes;
	stats->rx_packets = sum.rx_packets;

	return stats;
}

static int br_change_mtu(struct net_device *dev, int new_mtu)
{
	struct net_bridge *br = netdev_priv(dev);
	if (new_mtu < 68 || new_mtu > br_min_mtu(br))
		return -EINVAL;

	dev->mtu = new_mtu;

#ifdef CONFIG_BRIDGE_NETFILTER
	/* remember the MTU in the rtable for PMTU */
	dst_metric_set(&br->fake_rtable.dst, RTAX_MTU, new_mtu);
#endif

	return 0;
}

/* Allow setting mac address to any valid ethernet address. */
static int br_set_mac_address(struct net_device *dev, void *p)
{
	struct net_bridge *br = netdev_priv(dev);
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	spin_lock_bh(&br->lock);
	if (!ether_addr_equal(dev->dev_addr, addr->sa_data)) {
		memcpy(dev->dev_addr, addr->sa_data, ETH_ALEN);
		br_fdb_change_mac_address(br, addr->sa_data);
		br_stp_change_bridge_id(br, addr->sa_data);
	}
	spin_unlock_bh(&br->lock);

	return 0;
}

static void br_getinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strlcpy(info->driver, "bridge", sizeof(info->driver));
	strlcpy(info->version, BR_VERSION, sizeof(info->version));
	strlcpy(info->fw_version, "N/A", sizeof(info->fw_version));
	strlcpy(info->bus_info, "N/A", sizeof(info->bus_info));
}

static netdev_features_t br_fix_features(struct net_device *dev,
	netdev_features_t features)
{
	struct net_bridge *br = netdev_priv(dev);

	return br_features_recompute(br, features);
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void br_poll_controller(struct net_device *br_dev)
{
}

static void br_netpoll_cleanup(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_port *p;

	list_for_each_entry(p, &br->port_list, list)
		br_netpoll_disable(p);
}

static int br_netpoll_setup(struct net_device *dev, struct netpoll_info *ni,
			    gfp_t gfp)
{
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_port *p;
	int err = 0;

	list_for_each_entry(p, &br->port_list, list) {
		if (!p->dev)
			continue;
		err = br_netpoll_enable(p, gfp);
		if (err)
			goto fail;
	}

out:
	return err;

fail:
	br_netpoll_cleanup(dev);
	goto out;
}

int br_netpoll_enable(struct net_bridge_port *p, gfp_t gfp)
{
	struct netpoll *np;
	int err = 0;

	np = kzalloc(sizeof(*p->np), gfp);
	err = -ENOMEM;
	if (!np)
		goto out;

	err = __netpoll_setup(np, p->dev, gfp);
	if (err) {
		kfree(np);
		goto out;
	}

	p->np = np;

out:
	return err;
}

void br_netpoll_disable(struct net_bridge_port *p)
{
	struct netpoll *np = p->np;

	if (!np)
		return;

	p->np = NULL;

	__netpoll_free_async(np);
}

#endif

static int br_add_slave(struct net_device *dev, struct net_device *slave_dev)

{
	struct net_bridge *br = netdev_priv(dev);

	return br_add_if(br, slave_dev);
}

static int br_del_slave(struct net_device *dev, struct net_device *slave_dev)
{
	struct net_bridge *br = netdev_priv(dev);

	return br_del_if(br, slave_dev);
}

static const struct ethtool_ops br_ethtool_ops = {
	.get_drvinfo    = br_getinfo,
	.get_link	= ethtool_op_get_link,
};

static const struct net_device_ops br_netdev_ops = {
	.ndo_open		 = br_dev_open,
	.ndo_stop		 = br_dev_stop,
	.ndo_init		 = br_dev_init,
	.ndo_start_xmit		 = br_dev_xmit,
	.ndo_get_stats64	 = br_get_stats64,
	.ndo_set_mac_address	 = br_set_mac_address,
	.ndo_set_rx_mode	 = br_dev_set_multicast_list,
	.ndo_change_mtu		 = br_change_mtu,
	.ndo_do_ioctl		 = br_dev_ioctl,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_netpoll_setup	 = br_netpoll_setup,
	.ndo_netpoll_cleanup	 = br_netpoll_cleanup,
	.ndo_poll_controller	 = br_poll_controller,
#endif
	.ndo_add_slave		 = br_add_slave,
	.ndo_del_slave		 = br_del_slave,
	.ndo_fix_features        = br_fix_features,
	.ndo_fdb_add		 = br_fdb_add,
	.ndo_fdb_del		 = br_fdb_delete,
	.ndo_fdb_dump		 = br_fdb_dump,
	.ndo_bridge_getlink	 = br_getlink,
	.ndo_bridge_setlink	 = br_setlink,
	.ndo_bridge_dellink	 = br_dellink,
};

static void br_dev_free(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	free_percpu(br->stats);
	free_netdev(dev);
}

static struct device_type br_type = {
	.name	= "bridge",
};

void br_dev_setup(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	eth_hw_addr_random(dev);
	ether_setup(dev);

	dev->netdev_ops = &br_netdev_ops;
	dev->destructor = br_dev_free;
	SET_ETHTOOL_OPS(dev, &br_ethtool_ops);
	SET_NETDEV_DEVTYPE(dev, &br_type);
	dev->tx_queue_len = 0;
	dev->priv_flags = IFF_EBRIDGE;

#if defined(CONFIG_RTL_USB_IP_HOST_SPEEDUP)
	dev->features = NETIF_F_SG | NETIF_F_FRAGLIST | NETIF_F_HIGHDMA |
			NETIF_F_GSO_MASK | NETIF_F_HW_CSUM | NETIF_F_LLTX |
			NETIF_F_NETNS_LOCAL | NETIF_F_HW_VLAN_CTAG_TX |
                        NETIF_F_GSO | NETIF_F_GSO|NETIF_F_GRO;

#else
	dev->features = NETIF_F_SG | NETIF_F_FRAGLIST | NETIF_F_HIGHDMA |
			NETIF_F_GSO_MASK | NETIF_F_HW_CSUM | NETIF_F_LLTX |
			NETIF_F_NETNS_LOCAL | NETIF_F_HW_VLAN_CTAG_TX;
#endif
	dev->hw_features = NETIF_F_SG | NETIF_F_FRAGLIST | NETIF_F_HIGHDMA |
			   NETIF_F_GSO_MASK | NETIF_F_HW_CSUM |
			   NETIF_F_HW_VLAN_CTAG_TX;

	br->dev = dev;
	spin_lock_init(&br->lock);
	INIT_LIST_HEAD(&br->port_list);
	spin_lock_init(&br->hash_lock);

	br->bridge_id.prio[0] = 0x80;
	br->bridge_id.prio[1] = 0x00;

	memcpy(br->group_addr, eth_reserved_addr_base, ETH_ALEN);

	br->stp_enabled = BR_NO_STP;
	br->group_fwd_mask = BR_GROUPFWD_DEFAULT;

	br->designated_root = br->bridge_id;
	br->bridge_max_age = br->max_age = 20 * HZ;
	br->bridge_hello_time = br->hello_time = 2 * HZ;
#if defined(CONFIG_RTL_819X)
	br->bridge_forward_delay = br->forward_delay = 10 * HZ;
#else
	br->bridge_forward_delay = br->forward_delay = 15 * HZ;
#endif
	br->ageing_time = 300 * HZ;

	br_netfilter_rtable_init(br);
#if defined (CONFIG_RTL_IGMP_SNOOPING)
	br->igmpProxy_pid=0;
#endif
	br_stp_timer_init(br);
	br_multicast_init(br);
}

#if defined(CONFIG_RTL_HARDWARE_MULTICAST)
#if defined(CONFIG_RTL_IGMP_SNOOPING)
unsigned int rtl865x_getPhyFwdPortMask(struct net_bridge *br,unsigned int brFwdPortMask)
{	
	unsigned int phyPortMask = 0;
	unsigned int port_bitmask=0;
	struct net_bridge_port *p, *n;
	struct dev_priv *devPriv =NULL;
	if(br==NULL)
	{
		return 0;
	}
	list_for_each_entry_safe(p, n, &br->port_list, list) 
	{
		port_bitmask=1<<p->port_no;
		if(port_bitmask&brFwdPortMask)
		{
			devPriv=(struct dev_priv *)netdev_priv(p->dev);
			if(devPriv){	
				phyPortMask |= devPriv->portmask;
				//printk("[%s:%d]%s,%x,\n",__FUNCTION__,__LINE__,p->dev->name,devPriv->portmask);
			}	
		}
	}
	return phyPortMask;
}
int rtl865x_ipMulticastHardwareAccelerate(struct net_bridge *br, unsigned int brFwdPortMask,
												unsigned int srcPort,unsigned int srcVlanId, 
												unsigned int srcIpAddr, unsigned int destIpAddr)
{
	int ret;
	unsigned int tagged_portmask=0;
	struct rtl_multicastDataInfo multicastDataInfo;
	struct rtl_multicastFwdInfo  multicastFwdInfo;
	
	rtl865x_tblDrv_mCast_t * existMulticastEntry;
	rtl865x_mcast_fwd_descriptor_t  fwdDescriptor;

	#if 0
	printk("%s:%d,srcPort is %d,srcVlanId is %d,srcIpAddr is 0x%x,destIpAddr is 0x%x\n",__FUNCTION__,__LINE__,srcPort,srcVlanId,srcIpAddr,destIpAddr);
	#endif


#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)!=0 &&strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME)!=0 )
	{
		return -1;
	}
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)==0 && (brFwdPortMask & br0SwFwdPortMask))
	{
		return -1;
	}	
	
	if(strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME)==0 && (brFwdPortMask & br1SwFwdPortMask))
	{
		return -1;
	}
#else
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)!=0)
	{
		//if(net_ratelimit())printk("[%s:%d]wrong name...\n",__FUNCTION__,__LINE__);
		return -1;
	}

	if(brFwdPortMask & br0SwFwdPortMask)
	{
		//if(net_ratelimit())printk("[%s:%d]brFwdPortMask&br0SwFwdPortMask!=0\n",__FUNCTION__,__LINE__);
		return -1;
	}
#endif
	//printk("%s:%d,destIpAddr is 0x%x, srcIpAddr is 0x%x, srcVlanId is %d, srcPort is %d\n",__FUNCTION__,__LINE__,destIpAddr, srcIpAddr, srcVlanId, srcPort);
	existMulticastEntry=rtl865x_findMCastEntry(destIpAddr, srcIpAddr, (unsigned short)srcVlanId, (unsigned short)srcPort);
	if(existMulticastEntry!=NULL)
	{
		/*it's already in cache */
		//if(net_ratelimit())printk("[%s:%d]already in cache\n",__FUNCTION__,__LINE__);
		return 0;

	}

	if(brFwdPortMask==0)
	{
		//if(net_ratelimit())printk("[%s:%d]block\n",__FUNCTION__,__LINE__);
		rtl865x_blockMulticastFlow(srcVlanId, srcPort, srcIpAddr, destIpAddr);
		return 0;
	}
	
	multicastDataInfo.ipVersion=4;
	multicastDataInfo.sourceIp[0]=  srcIpAddr;
	multicastDataInfo.groupAddr[0]=  destIpAddr;

	/*add hardware multicast entry*/
#if !defined(CONFIG_RTL_MULTI_LAN_DEV)
	#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)==0)
	{
		memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
		strcpy(fwdDescriptor.netifName,"eth*");
		fwdDescriptor.fwdPortMask=0xFFFFFFFF;
		ret= rtl_getMulticastDataFwdInfo(nicIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);
	}
	else if(strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME)==0)
	{
		memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
		strcpy(fwdDescriptor.netifName,"eth2");
		fwdDescriptor.fwdPortMask=0xFFFFFFFF;
		ret= rtl_getMulticastDataFwdInfo(nicIgmpModuleIndex_2, &multicastDataInfo, &multicastFwdInfo);
	}
	#else
	memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
	strcpy(fwdDescriptor.netifName,"eth*");
	fwdDescriptor.fwdPortMask=0xFFFFFFFF;
	
	ret= rtl_getMulticastDataFwdInfo(nicIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);
	#endif
	if(ret!=0)
	{
		if(net_ratelimit())printk("[%s:%d]get nic fwd info failed.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	else
	{
		if(multicastFwdInfo.cpuFlag)
		{
			fwdDescriptor.toCpu=1;
		}
		fwdDescriptor.fwdPortMask=multicastFwdInfo.fwdPortMask & (~(1<<srcPort));
	}
#else/*!CONFIG_RTL_MULTI_LAN_DEV*/
	memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
	strcpy(fwdDescriptor.netifName,RTL_BR_NAME);
	if(multicastFwdInfo.cpuFlag)
	{
		fwdDescriptor.toCpu=1;
	}
	fwdDescriptor.fwdPortMask = rtl865x_getPhyFwdPortMask(br,brFwdPortMask) & (~(1<<srcPort));
	//printk("[%s:%d]fwdDescriptor.fwdPortMask=%x.\n",__FUNCTION__,__LINE__,fwdDescriptor.fwdPortMask);
#endif/*!CONFIG_RTL_MULTI_LAN_DEV*/
#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
	if(rtl_hw_vlan_ignore_tagged_mc == 1)
		tagged_portmask = rtl_hw_vlan_get_tagged_portmask();
#endif
	if((fwdDescriptor.fwdPortMask & tagged_portmask) == 0)
	{

		ret=rtl865x_addMulticastEntry(destIpAddr, srcIpAddr, (unsigned short)srcVlanId, (unsigned short)srcPort,
							&fwdDescriptor, 1, 0, 0, 0);
	}

	return 0;
}

#if defined(CONFIG_RTL_8198C)
int rtl865x_ipv6MulticastHardwareAccelerate(struct net_bridge *br, unsigned int brFwdPortMask,
												unsigned int srcPort,unsigned int srcVlanId, 
												struct in6_addr srcIpAddr,struct in6_addr destIpAddr)
{
#if 0
	if(net_ratelimit())
		printk("[%s:%d]\n",__FUNCTION__,__LINE__);
#endif
	int ret,i;
	unsigned int tagged_portmask=0;
	struct rtl_multicastDataInfo multicastDataInfo;
	struct rtl_multicastFwdInfo  multicastFwdInfo;

	rtl8198c_tblDrv_mCastv6_t * existMulticastEntry;
	rtl8198c_mcast_fwd_descriptor6_t  fwdDescriptor;

	#if 0
	printk("%s:%d,srcPort is %d,srcVlanId is %d,srcIpAddr is 0x%x,destIpAddr is 0x%x\n",__FUNCTION__,__LINE__,srcPort,srcVlanId,srcIpAddr,destIpAddr);
	#endif


#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)!=0 &&strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME)!=0 )
	{
		return -1;
	}
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)==0 && (brFwdPortMask & br0SwFwdPortMask))
	{
		return -1;
	}	
	
	if(strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME)==0 && (brFwdPortMask & br1SwFwdPortMask))
	{
		return -1;
	}
#else
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)!=0)
	{
		return -1;
	}

	if(brFwdPortMask & br0SwFwdPortMask)
	{
		return -1;
	}
#endif
	//printk("%s:%d,destIpAddr is 0x%x, srcIpAddr is 0x%x, srcVlanId is %d, srcPort is %d\n",__FUNCTION__,__LINE__,destIpAddr, srcIpAddr, srcVlanId, srcPort);
	inv6_addr_t sip,dip;
	memcpy(&sip,&srcIpAddr,sizeof(struct in6_addr));
	memcpy(&dip,&destIpAddr,sizeof(struct in6_addr));
	#if 0
	for(i=0;i<4;i++)
	{
		sip.v6_addr32[i] = srcIpAddr.s6_addr32[i];
		dip.v6_addr32[i] = destIpAddr.s6_addr32[i];
	}
	#endif
	existMulticastEntry=rtl8198C_findMCastv6Entry(dip,sip, (unsigned short)srcVlanId, (unsigned short)srcPort);
	if(existMulticastEntry!=NULL)
	{
		/*it's already in cache */
#if 0
		if(net_ratelimit())printk("[%s:%d]already in cache\n",__FUNCTION__,__LINE__);
#endif
		return 0;

	}

	if(brFwdPortMask==0)
	{
		//if(net_ratelimit())printk("[%s:%d]block\n",__FUNCTION__,__LINE__);
		rtl8198C_blockMulticastv6Flow(srcVlanId, srcPort, sip,dip);
		return 0;
	}
	
	multicastDataInfo.ipVersion=6;
	memcpy(multicastDataInfo.sourceIp,&srcIpAddr,sizeof(struct in6_addr));
	memcpy(multicastDataInfo.groupAddr,&destIpAddr,sizeof(struct in6_addr));
	#if 0
	for(i=0;i<4;i++)
	{
		multicastDataInfo.sourceIp[i] = srcIpAddr.in6_u.u6_addr32[i];
		multicastDataInfo.groupAddr[i] = destIpAddr.in6_u.u6_addr32[i];
	}
	#endif

	/*add hardware multicast entry*/
#if !defined(CONFIG_RTL_MULTI_LAN_DEV)
	#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if(strcmp(br->dev->name,RTL_PS_BR0_DEV_NAME)==0)
	{
		memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
		strcpy(fwdDescriptor.netifName,"eth*");
		fwdDescriptor.fwdPortMask=0xFFFFFFFF;
		ret= rtl_getMulticastDataFwdInfo(nicIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);
	}
	else if(strcmp(br->dev->name,RTL_PS_BR1_DEV_NAME)==0)
	{
		memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
		strcpy(fwdDescriptor.netifName,"eth2");
		fwdDescriptor.fwdPortMask=0xFFFFFFFF;
		ret= rtl_getMulticastDataFwdInfo(nicIgmpModuleIndex_2, &multicastDataInfo, &multicastFwdInfo);
	}
	#else
	memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
	strcpy(fwdDescriptor.netifName,"eth*");
	fwdDescriptor.fwdPortMask=0xFFFFFFFF;
	
	ret= rtl_getMulticastDataFwdInfo(nicIgmpModuleIndex, &multicastDataInfo, &multicastFwdInfo);
	#endif
	if(ret!=0)
	{
		if(net_ratelimit())printk("[%s:%d]get nic fwd info failed.\n",__FUNCTION__,__LINE__);
		return -1;
	}
	else
	{
		if(multicastFwdInfo.cpuFlag)
		{
			fwdDescriptor.toCpu=1;
		}
		fwdDescriptor.fwdPortMask=multicastFwdInfo.fwdPortMask & (~(1<<srcPort));
	}
#else/*!CONFIG_RTL_MULTI_LAN_DEV*/
	memset(&fwdDescriptor, 0, sizeof(rtl865x_mcast_fwd_descriptor_t ));
	strcpy(fwdDescriptor.netifName,RTL_BR_NAME);
	if(multicastFwdInfo.cpuFlag)
	{
		fwdDescriptor.toCpu=1;
	}
	fwdDescriptor.fwdPortMask = rtl865x_getPhyFwdPortMask(br,brFwdPortMask) & (~(1<<srcPort));
	//printk("[%s:%d]fwdDescriptor.fwdPortMask=%d.\n",__FUNCTION__,__LINE__,fwdDescriptor.fwdPortMask);
#endif/*!CONFIG_RTL_MULTI_LAN_DEV*/
#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
	if(rtl_hw_vlan_ignore_tagged_mc == 1)
		tagged_portmask = rtl_hw_vlan_get_tagged_portmask();
#endif
	if((fwdDescriptor.fwdPortMask & tagged_portmask) == 0)
	{

		ret=rtl8198C_addMulticastv6Entry(dip,sip, (unsigned short)srcVlanId, (unsigned short)srcPort,
							&fwdDescriptor, 1, 0, 0, 0);
	}

	return 0;
}

#endif
#endif/*CONFIG_RTL_IGMP_SNOOPING*/
#endif/*CONFIG_RTL_HARDWARE_MULTICAST*/

#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
int rtl865x_same_root(struct net_device *dev1,struct net_device *dev2){

	struct net_bridge_port *p = br_port_get_rcu(dev1);
	struct net_bridge_port *p2 = br_port_get_rcu(dev2);
	return !strncmp(p->br->dev->name,p2->br->dev->name,3);
}
#endif

