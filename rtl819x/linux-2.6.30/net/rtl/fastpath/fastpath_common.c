#if !defined(CONFIG_RTK_VLAN_SUPPORT)
#include <net/rtl/rtl_types.h>
#endif

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>

#ifdef CONFIG_BRIDGE
#include <bridge/br_private.h>
#endif

#if defined (FAST_PPTP) || defined(FAST_L2TP)
	#include <net/ip.h>
#endif

#if defined(CONFIG_NET_SCHED)
#include <linux/netfilter_ipv4/ip_tables.h>
extern int gQosEnabled;
#endif
#include <net/rtl/features/rtl_features.h>

#include <net/rtl/fastpath/fastpath_core.h>
#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
#include <net/rtl/features/rtl_features.h>
#endif
#include <net/rtl/rtl865x_nat.h>
#include <net/rtl/features/rtl_ps_log.h>

#include <net/rtl/rtl_nic.h>

#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
static int	rtl_fp_gc_rx_threshold;
#endif
static int fast_nat_fw = 1;

#define MAX_EXT_SERVICE_PORTS 30
static int ext_service_ports[MAX_EXT_SERVICE_PORTS] = {0};

#ifdef CONFIG_RTL_PPPOE_DIRECT_REPLY
int magicNum=-1;
void clear_magicNum(struct sk_buff *pskb)
{
	//panic_printk("%s:%d\n",__FUNCTION__,__LINE__);
	magicNum = -1;
}

int is_pppoe_lcp_echo_req(struct sk_buff *skb)
{
	unsigned char *mac_hdr=NULL;
	if(skb->dev==NULL)
	{
		return 0;
	}
	
	if(strncmp(skb->dev->name, "ppp" ,3)==0)
	{
		return 0;
	}
	//printk("%s:%d,%s\n",__FUNCTION__,__LINE__,skb->dev->name);
	mac_hdr=rtl_skb_mac_header(skb);
	if(mac_hdr==NULL)
	{
		return 0;
	}
	
	if( (mac_hdr[12]!=0x88) || (mac_hdr[13]!=0x64) \
		||(mac_hdr[20]!=0xc0) || (mac_hdr[21]!=0x21) \
		|| (mac_hdr[22]!=0x09))
	{		
		return 0;
	}
	return 1;

}


void extract_magicNum(struct sk_buff *skb)
{
	unsigned char *mac_hdr = skb->data;
	int i;
	int payloadLen=0;
	unsigned char type;
	unsigned char len;
	unsigned char *ptr;
	
	/*
	if( (*(unsigned short *)(&mac_hdr[12])==0x8864) \
		&&(*(unsigned short *)(&mac_hdr[20])==0xc021))
	{
		panic_printk("skb->dev is %s\n",skb->dev->name);
		for(i=0;i<32;i++)
		{
				panic_printk("0x%x\t",mac_hdr[i]);
				if(i%8==7)
				{
					panic_printk("\n");
				}
		}

	}	
	*/
	//panic_printk("%s:%d,magicNum is 0x%x\n",__FUNCTION__,__LINE__,magicNum);
	if(skb->dev==NULL)
	{
		return 0;
	}
	
	if(strncmp(skb->dev->name, "ppp" ,3)==0)
	{
		return;
	}
	
	if((mac_hdr[12]!=0x88) || (mac_hdr[13]!=0x64))
	{
		return;
	}
	
	
	if((mac_hdr[20]!=0xc0) || (mac_hdr[21]!=0x21))
	{
		return;
	}

	/*lcp configuration request*/
	if(mac_hdr[22]==0x01)
	{
	
		payloadLen=(mac_hdr[24]<<8)+mac_hdr[25];	
		payloadLen=payloadLen-4;
		ptr=(mac_hdr+26);

		while(payloadLen>0)
		{
			/*parse tlv option*/
			type=*ptr;
			len=*(ptr+1);
			
			//panic_printk("%s:%d,type is %d\n",__FUNCTION__,__LINE__,type);
			if(type==0x05) /*magic number option*/
			{
				memcpy(&magicNum, ptr+2 , 4);
				//panic_printk("%s:%d,magicNum is 0x%x\n",__FUNCTION__,__LINE__,magicNum);
				break;
			}

			if(len>payloadLen)
			{
				break;
			}
			ptr=ptr+len;
			payloadLen=payloadLen-len;
			
		}
	}
	else if(mac_hdr[22]==0x09) /*lcp echo request*/
	{
		ptr=(mac_hdr+26);
		memcpy(&magicNum, ptr , 4);
		//panic_printk("%s:%d,magic number is 0x%x\n",__FUNCTION__,__LINE__,magicNum);
	}
	return;

}

int direct_send_reply(struct sk_buff *skb)
{	

	unsigned char tmp[6];
	
	if(strncmp(skb->dev->name, "ppp" ,3)!=0)
	{
		skb_push(skb,14);
		
		// swap src and dst mac  	
		memcpy(tmp,skb->data,6);
		memcpy(skb->data,(skb->data+6),6);
		memcpy((skb->data+6),tmp,6);		
			
		// build ppp session header
		skb->data[22] = 0x0a;			//reply num
		memcpy((skb->data+26),&magicNum ,4);
		
		skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
		//panic_printk("%s:%d, direct_send_reply  tx\n",__FUNCTION__,__LINE__);		
		return 1;
	}
	
	return 0;
}


#endif

#if defined(IMPROVE_QOS) || defined(CONFIG_RTL_HW_QOS_SUPPORT)
#include <net/arp.h>
#include <net/rtl/rtl865x_netif.h>
//To query hardware address based on IP through arp table of dev
int arp_req_get_ha(__be32 queryIP, struct net_device *dev, unsigned char * resHwAddr)
{
	__be32 ip = queryIP;
	struct neighbour *neigh;
	int err = -ENXIO;

	neigh = neigh_lookup(&arp_tbl, &ip, dev);
	if (neigh) {
		read_lock_bh(&neigh->lock);
		memcpy(resHwAddr, neigh->ha, dev->addr_len);
		read_unlock_bh(&neigh->lock);
		neigh_release(neigh);
		err = 0;
	}
	//else
	//{
	//	resHwAddr=NULL;
	//}

	return err;
}
//EXPORT_SYMBOL(arp_req_get_ha);
#endif


#if defined(FAST_PATH_SPI_ENABLED)
int fast_spi =1;
static struct proc_dir_entry *res_spi=NULL;
static int spi_read_proc(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{
	int len = 0;

	len = sprintf(page, "fast_spi %s\n", fast_spi==1?"Enabled":"Disabled");

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	return len;

}
static int spi_write_proc(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	unsigned char tmpbuf[16];

	memset(tmpbuf, 0, sizeof(tmpbuf));
	if (buffer && !copy_from_user(tmpbuf, buffer, count))
		sscanf(tmpbuf, "%d", &fast_spi);

	return count;
}

#endif


#if defined(CONFIG_PROC_FS)
static struct proc_dir_entry *res1=NULL;
static int read_proc(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{
	int len = 0;

	#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
	len = sprintf(page, "fastpath %s, GC_RX_Count %d, Status: %d\n", fast_nat_fw!=0?"Enabled":"Disabled", rtl_fp_gc_rx_threshold, rtl_newGC_session_status_flags);
	#else
	len = sprintf(page, "fastpath: [%d]\n", fast_nat_fw+10);
	#endif
	len = fastpath_dump_napt_entry_num(page, len);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	return len;

}
static int write_proc(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	unsigned char tmpbuf[16];
	struct net *net;

	if (count < 2)
		return -EFAULT;

	memset(tmpbuf, 0, sizeof(tmpbuf));
	if (buffer && !copy_from_user(tmpbuf, buffer, count))  {
		if (tmpbuf[0] == '2'&&count==2){
			/* first byte == 2, second byte == "enter" */
			for_each_net(net) {
				nf_conntrack_flush(net, 0, 0);		//clean conntrack table
			}
			#if defined(CONFIG_RTL_LAYERED_DRIVER_L4)
			#if defined(CONFIG_RTL_8198) || defined (CONFIG_RTL_8196CT)
			rtl865x_nat_reinit();
			/* the following 2 values MUST set behind reinit nat module	*/
			//rtl_nat_expire_interval_update(RTL865X_PROTOCOL_TCP, tcp_get_timeouts_by_state(TCP_CONNTRACK_ESTABLISHED));
			//rtl_nat_expire_interval_update(RTL865X_PROTOCOL_UDP, nf_ct_udp_timeout>nf_ct_udp_timeout_stream?nf_ct_udp_timeout:nf_ct_udp_timeout_stream);
			#endif
			#endif
		}else{
			sscanf(tmpbuf, "%d", &fast_nat_fw);
			#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
			if (fast_nat_fw>2) {
				rtl_fp_gc_rx_threshold = fast_nat_fw;
			}
			#endif
		}
		return count;
	}

	return -EFAULT;
}

static struct proc_dir_entry *res2=NULL;
static int ext_service_ports_read_proc(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{
	int i;
	int len = 0;

	for (i=0; i<MAX_EXT_SERVICE_PORTS; i++)
	{
		if (ext_service_ports[i])
		{
			len += sprintf(&page[len], "%u ", ext_service_ports[i]);
		}
	}

	return len;
}

static int ext_service_ports_write_proc(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	unsigned char tmpbuf[1024];
	
	memset(tmpbuf, 0, sizeof(tmpbuf));
	if (count > sizeof(tmpbuf))
		return -EFAULT;

	if (buffer && !copy_from_user(tmpbuf, buffer, count))
	{
		int i = 0;
		char *token;
		char *str = (char *) tmpbuf;

		memset(ext_service_ports, 0, sizeof(ext_service_ports));
		while ((token = strsep(&str, " ")) != NULL && 
			i < MAX_EXT_SERVICE_PORTS - 1)
		{
			if (!*token)
				break;

			ext_service_ports[i++] = simple_strtol(token, NULL, 0);
		}	
	}

	return count;
}
#endif

#if	defined(CONFIG_RTL_HW_QOS_SUPPORT)
int32 rtl_qosGetSkbMarkByNaptEntry(rtl865x_napt_entry *naptEntry, rtl865x_qos_mark *qosMark, struct sk_buff *pskb)
{
	struct iphdr *iph;
	struct tcphdr *tcphupuh;  //just keep one , don't care tcp or udp //
	u_int ori_saddr, ori_daddr;
	u_short ori_sport, ori_dport;
	struct net_device	*lanDev, *wanDev;
	struct dst_entry *dst_tmp;
	unsigned char oriSrcMac[6],oriDstMac[6],resMac[14];
	u_short proto;
	unsigned char pppProto[2],ipProto[2];
	__u32 oriSkbMark;
	unsigned long irq_flags;
	uint32 preMark, postMark;

	if(pskb==NULL)
		return FAILED;

	//initial
	pppProto[0]=0x00;
	pppProto[1]=0x21;
	ipProto[0]=0x08;
	ipProto[1]=0x00;

	lanDev=rtl865x_getLanDev();
	wanDev=rtl865x_getWanDev();
	proto = ntohs(pskb->protocol);
	iph = ip_hdr(pskb);
	if(iph==NULL)
	{
		return FAILED;
	}
	tcphupuh = (struct tcphdr*)((__u32 *)iph + iph->ihl);

	if(eth_hdr(pskb)==NULL)
	{
		return FAILED;
	}
	//To bak origal protol mac
	memcpy(oriSrcMac,eth_hdr(pskb)->h_source,ETH_ALEN);
	memcpy(oriDstMac,eth_hdr(pskb)->h_dest,ETH_ALEN);

	//Bak orignal skb mark
	oriSkbMark=pskb->mark;

	//check ip-based qos rule at iptables mangle table
	//To record original info
	ori_saddr=iph->saddr;
	ori_sport=tcphupuh->source;
	ori_daddr=iph->daddr;
	ori_dport=tcphupuh->dest;

	/* for dst mac match, please refer to the xt_mac.c */
	dst_tmp = pskb->dst;
	pskb->dst = NULL;

	//For uplink
#if defined(CONFIG_NET_SCHED)
	preMark = 0;
	postMark= 0;
	{
	//Replace source addr to check uplink mark
	iph->saddr=naptEntry->intIp;
	tcphupuh->source=naptEntry->intPort;
	iph->daddr=naptEntry->remIp;
	tcphupuh->dest=naptEntry->remPort;

	memset(resMac,0,14);
	if((lanDev!=NULL)&&(arp_req_get_ha(naptEntry->intIp,lanDev,resMac)==0))
	{
		//Patch for pppoe wantype: run udp chariot
		//bak skb mac header
		if((memcmp(pskb->data-2, pppProto,2)==0)	//equal 0x0021
			&&(skb_mac_header_was_set(pskb)==1)
			&&(eth_hdr(pskb)->h_proto!=ntohs(0x0800))) //not equal to 0x0800
		{
				skb_set_mac_header(pskb, -22);
		}

		//Replace source mac addr to check uplink mark
		memcpy(eth_hdr(pskb)->h_source,resMac, ETH_ALEN);
		memcpy(eth_hdr(pskb)->h_dest,lanDev->dev_addr, ETH_ALEN);
	}

	pskb->mark=0;//initial
	if(proto == ETH_P_IP){
		(list_empty(&nf_hooks[PF_INET][NF_IP_PRE_ROUTING]))?: \
			ipt_do_table(pskb, NF_IP_PRE_ROUTING, lanDev,wanDev,\
			dev_net(lanDev)->ipv4.iptable_mangle);
	}

	DEBUGP_API("[%s][%d]:[%s][%s][%s][%s][%d]\n", __FUNCTION__, __LINE__,
					lanDev?lanDev->name:"NULL",
					wanDev?wanDev->name:"NULL",
					pskb->inDev?pskb->inDev->name:"NULL",
					pskb->dev?pskb->dev->name:"NULL", pskb->mark);
	preMark = pskb->mark;

	//Replace dest addr to check uplink mark
	iph->saddr=naptEntry->extIp;
	tcphupuh->source=naptEntry->extPort;
	iph->daddr=naptEntry->remIp;
	tcphupuh->dest=naptEntry->remPort;

	memset(resMac,0,14);
	if((wanDev!=NULL)&&(arp_req_get_ha(naptEntry->remIp,wanDev,resMac)==0))
	{
		//Patch for pppoe wantype: run udp chariot
		//bak skb mac header
		if((memcmp(pskb->data-2, pppProto,2)==0)	//equal 0x0021
			&&(skb_mac_header_was_set(pskb)==1)
			&&(eth_hdr(pskb)->h_proto!=ntohs(0x0800))) //not equal to 0x0800
		{
				skb_set_mac_header(pskb, -22);
		}

		//Replace source mac addr to check uplink mark
		memcpy(eth_hdr(pskb)->h_dest,resMac, ETH_ALEN);
		memcpy(eth_hdr(pskb)->h_source,wanDev->dev_addr, ETH_ALEN);
	}

	pskb->mark=0;//initial
	if(proto == ETH_P_IP){
		(list_empty(&nf_hooks[PF_INET][NF_IP_POST_ROUTING]))?: \
			ipt_do_table(pskb, NF_IP_POST_ROUTING, lanDev, wanDev,\
			dev_net(wanDev)->ipv4.iptable_mangle);
	}
	DEBUGP_API("[%s][%d]:[%s][%s][%s][%s][%d]\n", __FUNCTION__, __LINE__,
					lanDev?lanDev->name:"NULL",
					wanDev?wanDev->name:"NULL",
					pskb->inDev?pskb->inDev->name:"NULL",
					pskb->dev?pskb->dev->name:"NULL", pskb->mark);
	postMark= pskb->mark;
	}

	qosMark->uplinkMark=(postMark?postMark:preMark);
#endif

	//for downlink
#if defined(CONFIG_NET_SCHED)
	preMark = 0;
	postMark = 0;
	{
		//Replace source addr to check uplink mark
		iph->saddr=naptEntry->remIp;
		tcphupuh->source=naptEntry->remPort;
		iph->daddr=naptEntry->extIp;
		tcphupuh->dest=naptEntry->extPort;

		memset(resMac,0,14);
		if((wanDev!=NULL)&&(arp_req_get_ha(naptEntry->remIp,wanDev,resMac)==0))
		{
			//Patch for pppoe wantype: run udp chariot
			if((memcmp(pskb->data-2, pppProto,2)==0)	//equal 0x0021
				&&(skb_mac_header_was_set(pskb)==1)
				&&(eth_hdr(pskb)->h_proto!=ntohs(0x0800))) //not equal to 0x0800
			{
				skb_set_mac_header(pskb, -22);
			}

			//Replace source mac addr to check uplink mark
			memcpy(eth_hdr(pskb)->h_source,resMac, ETH_ALEN);
			memcpy(eth_hdr(pskb)->h_dest, wanDev->dev_addr, ETH_ALEN);
		}

		pskb->mark=0;//initial
		if(proto == ETH_P_IP){
			(list_empty(&nf_hooks[PF_INET][NF_IP_PRE_ROUTING]))?: \
				ipt_do_table(pskb, NF_IP_PRE_ROUTING, wanDev,lanDev,\
				dev_net(wanDev)->ipv4.iptable_mangle);
		}
		DEBUGP_API("[%s][%d]:[%s][%s][%s][%s][%d]\n", __FUNCTION__, __LINE__,
						lanDev?lanDev->name:"NULL",
						wanDev?wanDev->name:"NULL",
						pskb->inDev?pskb->inDev->name:"NULL",
						pskb->dev?pskb->dev->name:"NULL", pskb->mark);
		preMark = pskb->mark;

		//Replace dest addr to check uplink mark
		iph->saddr=naptEntry->remIp;
		tcphupuh->source=naptEntry->remPort;
		iph->daddr=naptEntry->intIp;
		tcphupuh->dest=naptEntry->intPort;

		memset(resMac,0,14);
		if ((lanDev!=NULL)&&(arp_req_get_ha(naptEntry->intIp,lanDev,resMac)==0))
		{
			//Patch for pppoe wantype: run udp chariot
			if((memcmp(pskb->data-2, pppProto,2)==0)	//equal 0x0021
				&&(skb_mac_header_was_set(pskb)==1)
				&&(eth_hdr(pskb)->h_proto!=ntohs(0x0800))) //not equal to 0x0800
			{
				skb_set_mac_header(pskb, -22);
			}
			//Replace dest mac addr and  hh data mac to check uplink mark
			memcpy(eth_hdr(pskb)->h_dest,resMac,ETH_ALEN);
			memcpy(eth_hdr(pskb)->h_source, lanDev->dev_addr, ETH_ALEN);
		}
		pskb->mark=0;//initial

		if(proto == ETH_P_IP){
			(list_empty(&nf_hooks[PF_INET][NF_IP_POST_ROUTING]))?: \
				ipt_do_table(pskb, NF_IP_POST_ROUTING, wanDev, lanDev,\
				dev_net(lanDev)->ipv4.iptable_mangle);
		}
		DEBUGP_API("[%s][%d]:[%s][%s][%s][%s][%d]\n", __FUNCTION__, __LINE__,
						lanDev?lanDev->name:"NULL",
						wanDev?wanDev->name:"NULL",
						pskb->inDev?pskb->inDev->name:"NULL",
						pskb->dev?pskb->dev->name:"NULL", pskb->mark);
		postMark= pskb->mark;
	}
	qosMark->downlinkMark=(postMark?postMark:preMark);
#endif

	//Back to original value
	//Back to orignal protol mac
	memcpy(eth_hdr(pskb)->h_source,oriSrcMac, ETH_ALEN);
	memcpy(eth_hdr(pskb)->h_dest,oriDstMac, ETH_ALEN);

	//Back to original skb mark
	pskb->mark=oriSkbMark;

	//back to original info
	iph->saddr=ori_saddr;
	tcphupuh->source=ori_sport;
	iph->daddr=ori_daddr;
	tcphupuh->dest=ori_dport;

	pskb->dst = dst_tmp;

	if(lanDev)
		dev_put(lanDev);

	if(wanDev)
		dev_put(wanDev);

	return SUCCESS;
}
#endif


void fastpath_set_qos_mark(struct sk_buff *skb, unsigned int preRouteMark, unsigned int postRouteMark)
{
	if(skb->mark == 0)
		skb->mark = (postRouteMark?postRouteMark:preRouteMark);
}

#if	defined(FAST_L2TP)
static inline void enter_fast_path_fast_l2tp_pre_process(struct sk_buff *skb)
{
	struct net_device *l2tprx_dev;
	struct in_device *skbIn_dev;
	struct net_device *skbNetDevice;

	if(fast_l2tp_fw){
		l2tprx_dev = skb->dev;
		skbIn_dev = (struct in_device *)skb->dev->ip_ptr;
		if(skbIn_dev == NULL){
			if ((skbNetDevice = __dev_get_by_name(&init_net,l2tprx_dev->name)) != NULL){
				if((skbIn_dev= (struct in_device*)skbNetDevice->ip_ptr) != NULL)
					skb->dev->ip_ptr = (void *)skbIn_dev;
			}
		}
	}
}
#endif

#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
static inline int rtl_fp_gc_status_check_priority(uint32 sIp, uint32 dIp, uint16 sPort, uint16 dPort)
{
	extern unsigned int	_br0_ip;

	#define	RTL_FP_GC_HIGH_PRIORITY_PORT_CHECK(__FP__GC__PORT__)	((sPort!=(__FP__GC__PORT__))&&dPort!=(__FP__GC__PORT__))

	if ( (dIp!=_br0_ip && sIp!=_br0_ip) &&
		(sPort>1024 && dPort>1024) &&
		(sPort!=8080 && dPort!=8080) &&
		(!(IS_CLASSD_ADDR(dIp))) &&
		(!(IS_BROADCAST_ADDR(dIp))) &&
		(!(IS_ALLZERO_ADDR(sIp))))
	{
		int i;

		for (i=0; ext_service_ports[i]; i++)
		{
			if (dPort == ext_service_ports[i])
				return NET_RX_SUCCESS;
		}

		return NET_RX_DROP;
	}
	else
		return NET_RX_SUCCESS;
	#undef	RTL_FP_GC_HIGH_PRIORITY_PORT_CHECK
}

static inline int rtl_fp_gc_status_check(struct iphdr *iph, struct tcphdr *tcph, struct sk_buff *skb)
{
	uint32 sIp, dIp;
	uint16 sPort, dPort;
	int	ret;
	static int	rx_count=0;

	ret = NET_RX_SUCCESS;
	if ((rtl_newGC_session_status_flags!=RTL_FP_SESSION_LEVEL_IDLE)&&time_after_eq(rtl_newGC_session_status_time, jiffies)) {
		sIp = iph->saddr;
		dIp = iph->daddr;
		sPort = tcph->source;
		dPort = tcph->dest;
		if (rtl_fp_gc_status_check_priority(sIp, dIp, sPort, dPort)==NET_RX_DROP) {
			kfree_skb(skb);
			ret = NET_RX_DROP;
		}
	} else {
		if (rtl_newGC_session_status_flags==RTL_FP_SESSION_LEVEL3) {
			if ((rx_count++)>rtl_fp_gc_rx_threshold) {
				rx_count = 0;
				rtl_newGC_session_status_time = jiffies+RTL_FP_SESSION_LEVEL3_INTERVAL;
			}
		} else if (rtl_newGC_session_status_flags==RTL_FP_SESSION_LEVEL1) {
			rx_count += RTL_FP_SESSION_LEVEL1_RX_WEIGHT;
			if ((rx_count)>rtl_fp_gc_rx_threshold) {
				rx_count = 0;
				rtl_newGC_session_status_time=jiffies+RTL_FP_SESSION_LEVEL1_INTERVAL;
			}
		}
	}

	return ret;
}
#endif

int fast_path_pre_process_check(struct iphdr *iph, struct tcphdr *tcphupuh, struct sk_buff *skb)
{
	#if	defined(FAST_L2TP)
	enter_fast_path_fast_l2tp_pre_process(skb);
	#endif

	#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
	if ((iph->protocol==IPPROTO_TCP) &&
		(tcphupuh->syn) && (!(tcphupuh->ack))) {
		return rtl_fp_gc_status_check(iph, tcphupuh, skb);
	}
	#endif

	return NET_RX_PASSBY;
}



static inline int enter_fast_path_fast_l2tp_post_process(struct sk_buff *skb)
{
#if	defined(FAST_L2TP)
	if (fast_l2tp_fw && skb->dev && (!memcmp(skb->dev->name, RTL_FASTPATH_PPP0_DEV_NAME, 4)) )
	{
		if (fast_l2tp_to_wan((void*)skb)) // success
		{
			return NET_RX_DROP;
		}
	}
#endif	
	return NET_RX_SUCCESS;
}


static inline int enter_fast_path_fast_pppoe_post_process(struct sk_buff *skb)
{
	
#if defined(CONFIG_RTL_FAST_PPPOE)
	if (fast_pppoe_xmit(skb)) // success
	{
		return NET_RX_DROP;
	}
#endif
	return NET_RX_SUCCESS;
}


#if defined(FASTPTH_INDEPENDENCE_KERNEL)
struct dst_entry *dst_tmp = NULL;

/*As these API are used in fastpath, so skb will be check as valid, I will not check
skb again*/

unsigned int rtl_get_skb_len(struct sk_buff *skb)
{
	return skb->len;
}

__be16 rtl_get_skb_protocol(struct sk_buff *skb)
{
	return skb->protocol;
}

void rtl_set_skb_protocol(struct sk_buff *skb,__be16 protocol)
{
	skb->protocol=protocol;
}

unsigned char rtl_get_skb_type(struct sk_buff *skb)
{
	return skb->pkt_type;
}


__wsum rtl_get_skb_csum(struct sk_buff *skb)
{
	return skb->csum;
}


unsigned char *rtl_get_skb_data(struct sk_buff* skb)
{
	return skb->data;
}


void rtl_set_skb_data(struct sk_buff *skb, int offset, int action)
{
	if(action == 1)
		skb->data -= offset;
	else if(action == 0)
		skb->data += offset;

	return;
}

unsigned char *rtl_skb_mac_header(struct sk_buff * skb)
{
	return skb_mac_header(skb);
}

void rtl_skb_set_mac_header(struct sk_buff *skb, int offset)
{
	return skb_set_mac_header(skb, offset);
}


int rtl_skb_mac_header_was_set(struct sk_buff *skb)
{
	return skb_mac_header_was_set(skb);
}


void rtl_set_skb_dmac(struct sk_buff *skb, void *device)
{
	struct net_device *dev = (struct net_device *)device;

	memcpy(eth_hdr(skb)->h_dest, dev->dev_addr, ETH_ALEN);

	return;
}

void rtl_set_skb_smac(struct sk_buff *skb, void *device)
{
	struct net_device *dev = (struct net_device *)device;

	memcpy(eth_hdr(skb)->h_source, dev->dev_addr, ETH_ALEN);

	return;
}

unsigned char *rtl_skb_network_header(struct sk_buff * skb)
{
	return skb_network_header(skb);
}


void rtl_skb_set_network_header(struct sk_buff * skb,const int offset)
{
	skb_set_network_header(skb,offset);
}

void rtl_skb_reset_network_header(struct sk_buff *skb)
{
	return skb_reset_network_header(skb);
}

void rtl_set_skb_network_header(struct sk_buff * skb, unsigned char *network_header)
{
	skb->network_header=network_header;
}

unsigned char *rtl_skb_transport_header(struct sk_buff * skb)
{
	return skb_transport_header(skb);
}

void rtl_skb_set_transport_header(struct sk_buff * skb,const int offset)
{
	skb_set_transport_header(skb,offset);
}

void rtl_skb_reset_transport_header(struct sk_buff *skb)
{
	return skb_reset_transport_header(skb);
}

void rtl_set_skb_transport_header(struct sk_buff * skb, unsigned char *transport_header)
{
	skb->transport_header=transport_header;
}


unsigned char *rtl_skb_pull(struct sk_buff *skb, unsigned int len)
{
	return skb_pull(skb,len);
}

unsigned char *rtl_skb_push(struct sk_buff *skb, unsigned int len)
{
	return skb_push(skb,len);
}


int rtl_ppp_proto_check(struct sk_buff *skb, unsigned char* ppp_proto)
{
	if(memcmp(skb->data-2, ppp_proto,2)==0)
		return 1;
	else
		return 0;
}

unsigned int rtl_ipt_do_table(struct sk_buff * skb, unsigned int hook, void *in, void *out)
{
	struct net_device *out_dev = (struct net_device *)out;
	struct net_device *in_dev;

	if(in == NULL)
		in_dev = skb->dev;
	else
		in_dev = (struct net_device *)in;

	return ipt_do_table(skb, hook, in_dev, out_dev, dev_net(skb->dev)->ipv4.iptable_mangle);
}

int rtl_ip_route_input(struct sk_buff  *skb, __be32 daddr, __be32 saddr, u8 tos)
{
	return ip_route_input(skb, daddr, saddr, tos, skb->dev);
}

int rtl_skb_dst_check(struct sk_buff *skb)
{
	int ret = SUCCESS;

	if( !(skb->dst->hh || skb->dst->neighbour)  ||skb->len > dst_mtu(skb->dst))
		ret = FAILED;

	return ret;
}

void rtl_set_skb_ip_summed(struct sk_buff *skb, int value)
{
	skb->ip_summed = value;
	return;
}

void rtl_dst_release(struct sk_buff *skb)
{
	dst_release(skb->dst);
	skb->dst = NULL;

	return;
}


__u32 rtl_get_skb_mark(struct sk_buff *skb)
{
	return skb->mark;
}

void rtl_set_skb_mark(struct sk_buff *skb, unsigned int value)
{
	skb->mark = value;

	return;
}


void rtl_store_skb_dst(struct sk_buff *skb)
{
	dst_tmp = skb->dst;

	skb->dst = NULL;

	return;
}

void rtl_set_skb_dst(struct sk_buff *skb)
{
	skb->dst = dst_tmp;

	return;
}

int rtl_tcp_get_timeouts(void *ptr)
{
	struct nf_conn *ct = (struct nf_conn *)ptr;

	return tcp_get_timeouts_by_state(ct->proto.tcp.state);
}

int rtl_arp_req_get_ha(__be32 queryIP, void *device, unsigned char * resHwAddr)
{
	struct net_device *dev = (struct net_device *)device;

	return arp_req_get_ha(queryIP, dev, resHwAddr);
}



u_int8_t rtl_get_ct_protonum(void *ct_ptr, enum ip_conntrack_dir dir)
{
	struct nf_conn *ct = (struct nf_conn *)ct_ptr;

	return ct->tuplehash[dir].tuple.dst.protonum;
}

unsigned long rtl_get_ct_udp_status(void *ct_ptr)
{
	struct nf_conn *ct = (struct nf_conn *)ct_ptr;

	return ct->status;
}

u_int8_t rtl_get_ct_tcp_state(void *ct_ptr)
{
	struct nf_conn *ct = (struct nf_conn *)ct_ptr;

	return ct->proto.tcp.state;
}

/*flag = 0 for src; flag = 1 for dst*/
__be32 rtl_get_ct_ip_by_dir(void *ct_ptr, enum ip_conntrack_dir dir, int flag)
{
	struct nf_conn *ct = (struct nf_conn *)ct_ptr;

	if(dir == IP_CT_DIR_ORIGINAL)
	{
		if(flag == 0)
			return ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u3.ip;
		else if(flag == 1)
			return ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u3.ip;
	}
	else if(dir == IP_CT_DIR_REPLY)
	{
		if(flag == 0)
			return ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u3.ip;
		else if(flag == 1)
			return ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u3.ip;
	}
}

/*flag = 0 for src; flag = 1 for dst*/
__be16 rtl_get_ct_port_by_dir(void *ct_ptr, enum ip_conntrack_dir dir, int flag)
{
	struct nf_conn *ct = (struct nf_conn *)ct_ptr;

	if(dir == IP_CT_DIR_ORIGINAL)
	{
		if(flag == 0)
			return ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.src.u.all;
		else if(flag == 1)
			return ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple.dst.u.all;

	}
	else if(dir == IP_CT_DIR_REPLY)
	{
		if(flag == 0)
			return ct->tuplehash[IP_CT_DIR_REPLY].tuple.src.u.all;
		else if(flag == 1)
			return ct->tuplehash[IP_CT_DIR_REPLY].tuple.dst.u.all;
	}
}

void rtl_set_ct_timeout_expires(void *ct_ptr, unsigned long value)
{
	struct nf_conn *ct = (struct nf_conn *)ct_ptr;

	ct->timeout.expires = value;

	return;
}

unsigned long rtl_hold_time(void *br_ptr)
{
	struct net_bridge *br = (struct net_bridge *)br_ptr;

	return br->topology_change ? br->forward_delay : br->ageing_time;
}

void rtl_set_fdb_aging(void *fdb_ptr, unsigned long value)
{
	struct net_bridge_fdb_entry *fdb = (struct net_bridge_fdb_entry *)fdb_ptr;

	fdb->ageing_timer = value;

	return;
}

unsigned long rtl_get_fdb_aging(void *fdb_ptr)
{
	struct net_bridge_fdb_entry *fdb = (struct net_bridge_fdb_entry *)fdb_ptr;

	return fdb->ageing_timer;
}

struct ethhdr *rtl_eth_hdr(struct sk_buff *skb)
{
	return eth_hdr(skb);
}


struct iphdr *rtl_ip_hdr(struct sk_buff *skb)
{
	return ip_hdr(skb);
}


struct net_device * rtl_get_dev_by_name(char *name)
{
	return __dev_get_by_name(&init_net, name);
}

struct net_device *rtl_get_skb_dev(struct sk_buff* skb)
{
	return skb->dev;
}


void rtl_set_skb_dev(struct sk_buff *skb, struct net_device *dev)
{
	if(dev == NULL)
		skb->dev = skb->dst->dev;
	else
		skb->dev = dev;

	return;
}

char *rtl_get_skb_dev_name(struct sk_buff *skb)
{
	return skb->dev->name;
}


void rtl_set_skb_inDev(struct sk_buff *skb)
{

	skb->inDev = skb->dev;

	return;
}

unsigned int rtl_get_skb_pppoe_flag(struct sk_buff* skb)
{
	#if defined (CONFIG_RTL_FAST_PPPOE)
	return skb->pppoe_flag;
	#else
	return 0;
	#endif
}

void rtl_set_skb_pppoe_flag(struct sk_buff* skb, unsigned int pppoe_flag)
{
	#if defined (CONFIG_RTL_FAST_PPPOE)
	skb->pppoe_flag=pppoe_flag;
	#endif
	return;
}

struct net_device *rtl_get_skb_rx_dev(struct sk_buff* skb)
{
	#if defined (CONFIG_RTL_FAST_PPPOE)
	return skb->rx_dev;
	#else
	return NULL;
	#endif
}

void rtl_set_skb_rx_dev(struct sk_buff* skb,struct net_device *dev)
{
	#if defined (CONFIG_RTL_FAST_PPPOE)
	return skb->rx_dev=dev;
	#endif

	return;
}


char *rtl_get_ppp_dev_name(struct net_device *ppp_dev)
{
	return ppp_dev->name;
}

void * rtl_get_ppp_dev_priv(struct net_device *ppp_dev)
{
	return ppp_dev->priv;
}

int rtl_call_skb_ndo_start_xmit(struct sk_buff *skb)
{
	return skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
}

void rtl_inc_ppp_stats(struct ppp *ppp, int act, int len)
{

	if(act == 0){	//rx
		//ppp->stats.rx_packets ++;
		//ppp->stats.rx_bytes += len;
		ppp->dev->stats.rx_packets ++;
		ppp->dev->stats.rx_bytes += len;

		
	}else if(act == 1){ //tx
		//ppp->stats.tx_packets ++;
		//ppp->stats.tx_bytes += len;
		ppp->dev->stats.tx_packets ++;
		ppp->dev->stats.tx_bytes += len;

	}

	return;
}


void *rtl_set_skb_tail(struct sk_buff *skb, int offset, int action)
{
	if(action == 1)
		skb->tail -= offset;
	else if(action == 0)
		skb->tail += offset;

	return;
}

struct sk_buff *rtl_ppp_receive_nonmp_frame(struct ppp *ppp, struct sk_buff *skb, int is_fast_fw)
{
	return ppp_receive_nonmp_frame(ppp, skb, is_fast_fw);
}

int rtl_ppp_start_xmit(struct sk_buff *skb, struct net_device *dev)
{
	return ppp_start_xmit(skb, dev);
}

void rtl_set_skb_cb(struct sk_buff *skb, char *value, int len)
{
	memcpy(skb->cb, value, len);

	return;
}

int rtl_ppp_vj_check(struct ppp* ppp)
{
	if(ppp->vj && !((ppp->xstate & SC_COMP_RUN) && ppp->xc_state))
		return 1;
	else
		return 0;
}

void *rtl_get_ppp_xmit_pending(struct ppp* ppp)
{
	return (void*)ppp->xmit_pending;
}

void rtl_set_ppp_xmit_pending(struct ppp* ppp, struct sk_buff* skb)
{
	ppp->xmit_pending = skb;

	return;
}

void rtl_set_skb_nfct(struct sk_buff *skb, void *value)
{
	skb->nfct = value;

	return;
}

struct neighbour *rtl_neigh_lookup(const void *pkey, struct net_device *dev)
{
	return neigh_lookup(&arp_tbl, pkey, dev);
}

struct hh_cache *rtl_get_hh_from_neigh(struct neighbour *neigh)
{
	return neigh->hh;
}

seqlock_t rtl_get_lock_from_hh(struct hh_cache * hh)
{
	return hh->hh_lock;
}

unsigned short rtl_get_len_from_hh(struct hh_cache * hh)
{
	return hh->hh_len;
}

unsigned long *rtl_get_data_from_hh(struct hh_cache * hh)
{
	return hh->hh_data;
}

unsigned int rtl_skb_headroom(struct sk_buff *skb)
{
	return skb_headroom(skb);
}

int rtl_skb_cloned(struct sk_buff *skb)
{
	return skb_cloned(skb);
}

int rtl_skb_shared(const struct sk_buff *skb)
{
	return skb_shared(skb);
}

#if defined(CONFIG_RTL_DSCP_IPTABLE_CHECK) && defined(IMPROVE_QOS)
__u8 rtl_get_skb_orig_dscp(struct sk_buff *skb)
{
	return skb->original_dscp;
}
#endif

void rtl_conntrack_drop_check_hook(struct nf_conn *ct_tmp, uint16 ipprotocol, struct nf_conn *ct)
{	
#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
	if (ct_tmp->drop_flag == -1 && (ipprotocol == IPPROTO_TCP || ipprotocol == IPPROTO_UDP))
	{
		ct_tmp->drop_flag = __conntrack_drop_check(ct);
	}
#endif
}

int  rtl_Add_Pattern_ACL_For_ContentFilter(void)
{
#if defined (CONFIG_RTL_LAYERED_DRIVER_L4)
#ifdef CONFIG_RTL_LAYERED_DRIVER_ACL
        rtl865x_AclRule_t rule;
 #if 0  //### add by sen_liu 2011.5.4 for lan("br0"),don't add pattern acl                     
        memset(&rule,0,sizeof(rtl865x_AclRule_t));                      
        rule.actionType_ = RTL865X_ACL_TOCPU;
        rule.ruleType_ = RTL865X_ACL_IP;
        rule.ipHttpFilter_=rule.ipHttpFilterM_=1;
        rule.pktOpApp_ = RTL865X_ACL_L3_AND_L4;

	#if defined(CONFIG_RTL_NETIF_MAPPING)
	{
		ps_drv_netif_mapping_t *entry;
		struct net_device *dev;
		dev = dev_get_by_name(&init_net,RTL_PS_BR0_DEV_NAME);
		if(dev == NULL)
			return 0;
		
		entry = rtl_get_ps_drv_netif_mapping_by_psdev(dev);
		dev_put(dev);
		if(entry == NULL)
		{
			printk("====%s(%d),ERROR,can't get lan device!\n",__FUNCTION__,__LINE__);
			return 0;
		}
		rtl865x_add_pattern_acl_for_contentFilter(&rule,entry->drvName);
	}
	#else
	rtl865x_add_pattern_acl_for_contentFilter(&rule,"br0");
	#endif
#endif

#ifdef CONFIG_RTL_IPTABLES_RULE_2_ACL
#else
	//Patch: lan pkt rcv to cpu
	memset(&rule,0,sizeof(rtl865x_AclRule_t));                      
        rule.actionType_ = RTL865X_ACL_PERMIT;
        rule.ruleType_ = RTL865X_ACL_MAC;
        rule.pktOpApp_ = RTL865X_ACL_ALL_LAYER;
	#if defined(CONFIG_RTL_NETIF_MAPPING)
	{
		ps_drv_netif_mapping_t *entry;
		void *dev;

		#if 0//### add by sen_liu 2011.5.4 for lan("br0"),don't add pattern acl 
		dev = dev_get_by_name(&init_net,RTL_PS_BR0_DEV_NAME);
		if(dev == NULL)
			return 0;
		
		entry = rtl_get_ps_drv_netif_mapping_by_psdev(dev);
		dev_put(dev);
		if(entry == NULL)
		{
			printk("====%s(%d),ERROR,can't get lan device!\n",__FUNCTION__,__LINE__);
			return 0;
		}
		rtl865x_add_pattern_acl_for_contentFilter(&rule,entry->drvName);
		#endif
		
		//wan
		dev = rtl_get_dev_by_name(RTL_PS_WAN0_DEV_NAME);
		if(dev == NULL)
			return 0;
		entry = rtl_get_ps_drv_netif_mapping_by_psdev(dev);
		dev_put(dev);
		if(entry == NULL)
		{
			printk("====%s(%d),ERROR,can't get wan device!\n",__FUNCTION__,__LINE__);
			return 0;
		}
		rtl865x_add_pattern_acl_for_contentFilter(&rule,entry->drvName);
	}
	#else
	//rtl865x_add_pattern_acl_for_contentFilter(&rule,"br0"); //### add by sen_liu 2011.5.4 for lan("br0"),don't add pattern acl 
	rtl865x_add_pattern_acl_for_contentFilter(&rule,"eth1");
	#endif
	//End patch
#endif
#endif
 #endif
        return 0;
}

int  Add_Pattern_ACL_For_ContentFilter(void)
{
	return rtl_Add_Pattern_ACL_For_ContentFilter();
}

 int  Del_Pattern_ACL_For_ContentFilter(void)
{
#if defined (CONFIG_RTL_LAYERED_DRIVER_L4)
#ifdef CONFIG_RTL_LAYERED_DRIVER_ACL
        rtl865x_AclRule_t rule;
                               
        memset(&rule,0,sizeof(rtl865x_AclRule_t));                      
        rule.actionType_ = RTL865X_ACL_TOCPU;
        rule.ruleType_ = RTL865X_ACL_IP;
        rule.ipHttpFilter_=rule.ipHttpFilterM_=1;
        rule.pktOpApp_ = RTL865X_ACL_L3_AND_L4;
	#if defined(CONFIG_RTL_NETIF_MAPPING)
	{
		ps_drv_netif_mapping_t *entry;
		void *dev;
		dev = rtl_get_dev_by_name(RTL_PS_BR0_DEV_NAME);
		if(dev == NULL)
			return 0;
		
		entry = rtl_get_ps_drv_netif_mapping_by_psdev(dev);
		dev_put(dev);
		if(entry == NULL)
		{
			printk("====%s(%d),ERROR,can't get lan device!\n",__FUNCTION__,__LINE__);
			return 0;
		}
		rtl865x_del_pattern_acl_for_contentFilter(&rule,entry->drvName);
	}
	#else	
	rtl865x_del_pattern_acl_for_contentFilter(&rule,"br0");
	#endif

//2010-12-29
//when wan is connected,if url filter entry is NULL, default acl will be deleted!!
//don't delete the default acl now...
#if 0	
#ifdef CONFIG_RTL_IPTABLES_RULE_2_ACL
#else
	//Patch: lan pkt rcv to cpu
	memset(&rule,0,sizeof(rtl865x_AclRule_t));                      
        rule.actionType_ = RTL865X_ACL_PERMIT;
        rule.ruleType_ = RTL865X_ACL_MAC;
        rule.pktOpApp_ = RTL865X_ACL_ALL_LAYER;
	#if defined(CONFIG_RTL_NETIF_MAPPING)
	{
		ps_drv_netif_mapping_t *entry;
		struct net_device *dev;
		dev = dev_get_by_name(&init_net,RTL_PS_BR0_DEV_NAME);
		if(dev == NULL)
			return 0;
		
		entry = rtl_get_ps_drv_netif_mapping_by_psdev(dev);
		if(entry == NULL)
		{
			printk("====%s(%d),ERROR,can't get lan device!\n",__FUNCTION__,__LINE__);
			return 0;
		}
		rtl865x_del_pattern_acl_for_contentFilter(&rule,entry->drvName);

		dev = dev_get_by_name(&init_net,RTL_PS_WAN0_DEV_NAME);
		if(dev == NULL)
			return 0;
		
		entry = rtl_get_ps_drv_netif_mapping_by_psdev(dev);
		if(entry == NULL)
		{
			printk("====%s(%d),ERROR,can't get wan device!\n",__FUNCTION__,__LINE__);
			return 0;
		}
		rtl865x_del_pattern_acl_for_contentFilter(&rule,entry->drvName);
		
	}
	#else	
	rtl865x_del_pattern_acl_for_contentFilter(&rule,"br0"); 
	rtl865x_del_pattern_acl_for_contentFilter(&rule,"eth1"); 
	#endif
	//End patch
#endif
#endif
#endif
#endif
        return 0;
}

#endif


int fast_path_before_nat_check(struct sk_buff *skb, struct iphdr *iph, uint32 iphProtocol)
{
	#if defined(RTL_FP_CHECK_SPI_ENABLED) || defined(FAST_PATH_SPI_ENABLED)
	int	ret;
	#endif
	#if defined(FAST_PATH_SPI_ENABLED)
	unsigned int dataoff;
	if(fast_spi == 0)
		return NET_RX_PASSBY;
	#endif

	#if defined(FAST_PATH_SPI_ENABLED)
	if (iphProtocol== IPPROTO_TCP){
		dataoff = skb_network_offset(skb) + (iph->ihl<<2);
		ret = rtl_nf_conntrack_in(dev_net(skb->dev), dataoff, NF_INET_PRE_ROUTING, skb);
		switch (ret){
			case	NF_DROP:
				kfree_skb(skb);
				return NET_RX_DROP;
			case	NF_ACCEPT:
				break;
			default:
				kfree_skb(skb);
				return NET_RX_DROP;
			}
	}
	#endif

	return NET_RX_PASSBY;
}

int fast_path_post_process_xmit_check(struct iphdr *iph, struct tcphdr *tcphupuh, struct sk_buff *skb)
{
	#if	defined(FAST_L2TP) || defined (CONFIG_RTL_FAST_PPPOE)
	if (enter_fast_path_fast_l2tp_post_process(skb)==NET_RX_DROP) 
	{
		return NET_RX_DROP;
	}
	else if (enter_fast_path_fast_pppoe_post_process(skb)==NET_RX_DROP)
	{
		return NET_RX_DROP;
	}	
	else
	{
		return NET_RX_SUCCESS;
	}
	#else
	return NET_RX_SUCCESS;
	#endif
}

int fast_path_post_process_return_check(struct iphdr *iph, struct tcphdr *tcphupuh, struct sk_buff *skb)
{
	#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
	if (iph->protocol==IPPROTO_UDP)
		return rtl_fp_gc_status_check(iph, tcphupuh, skb);
	#endif

	return NET_RX_SUCCESS;

}

int32 rtl_fp_dev_queue_xmit_check(struct sk_buff *skb, struct net_device *dev)
{
	#ifdef FAST_L2TP
	if (l2tp_tx_id_hook != NULL)
		l2tp_tx_id_hook((void*)skb);
	#endif

	#ifdef FAST_PPTP
	if (sync_tx_pptp_gre_seqno_hook != NULL)
		sync_tx_pptp_gre_seqno_hook(skb);
	#endif

	return SUCCESS;
}

int32 rtl_fp_dev_hard_start_xmit_check(struct sk_buff *skb, struct net_device *dev, struct netdev_queue *txq)
{
#if	defined(FAST_L2TP)
	#if	defined(CONFIG_NET_SCHED)
	if(!gQosEnabled)
	#endif
	if (l2tp_tx_id_hook != NULL)
		l2tp_tx_id_hook((void*)skb);
#endif

#if defined (CONFIG_RTL_LOCAL_PUBLIC)
	if(	(skb->srcLocalPublicIp!=0) &&
		(rtl865x_curOpMode==GATEWAY_MODE) &&
		#if defined(CONFIG_RTL_PUBLIC_SSID)
		(strncmp(dev->name,RTL_GW_WAN_DEVICE_NAME, 3)==0)
		#else
		(strncmp(dev->name,RTL_PS_WAN0_DEV_NAME, 4)==0)
		#endif
		) {
		rtl865x_getLocalPublicMac(skb->srcLocalPublicIp, skb->data+6);
	}
#endif
	return SUCCESS;
}


int ip_finish_output3(struct sk_buff *skb)
{
	struct dst_entry *dst;
	struct hh_cache *hh;

	dst = skb->dst;
	hh = dst->hh;

#if !defined(IMPROVE_QOS)
#if defined(CONFIG_NET_SCHED)
    if (gQosEnabled) {
		u_short proto = ntohs(skb->protocol);
		if(proto == ETH_P_IP){
			(list_empty(&nf_hooks[PF_INET][NF_IP_POST_ROUTING]))?: \
				ipt_do_table(skb, NF_IP_POST_ROUTING, skb->dev, NULL, \
				dev_net(skb->dev)->ipv4.iptable_mangle);
		}
    }
#endif
#endif


	if (hh) {
// ------------------------------------------------
#if 1
		memcpy(skb->data - 16, hh->hh_data, 16);
#else
		memcpy((unsigned char*)UNCACHE(skb->data - 16), hh->hh_data, 16);
#endif
		skb_push(skb, hh->hh_len);

#ifdef FAST_L2TP
		if (fast_l2tp_fw)
			l2tp_tx_id((void*)skb);
#endif

		if (skb->dev->flags & IFF_UP) {
#if defined(CONFIG_NET_SCHED)
			if (gQosEnabled) {
				// call dev_queue_xmit() instead of hard_start_xmit(), because I want the packets be sent through Traffic Control module
				dev_queue_xmit(skb);
				return 0;
			} else
#endif
			{

		#if defined(CONFIG_BRIDGE)
				/*	In order to improve performance
				*	We'd like to directly xmit and bypass the bridge check
				*/
				if (skb->dev->priv_flags == IFF_EBRIDGE)
				{
					/*	wan->lan	*/
					struct net_bridge *br = netdev_priv(skb->dev);
					const unsigned char *dest = skb->data;
					struct net_bridge_fdb_entry *dst;

					if ((dst = __br_fdb_get(br, dest)) != NULL)
					{
						//skb->dev->stats.tx_packets++;
						//skb->dev->stats.tx_bytes += skb->len;
						skb->dev = dst->dst->dev;
					}
				}
		#endif
				{
					if(!skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev))
						return 0;
				}
			}
		}
//------------------------------- david+2007-05-25

	} else if (dst->neighbour) {
		return dst->neighbour->output(skb);
	}

#if 0
	if (net_ratelimit())
		printk(KERN_DEBUG "ip_finish_output3: No header cache and no neighbour!\n");
#endif
	kfree_skb(skb);
	return -EINVAL;
}

#if defined (CONFIG_RTL_FAST_PPPOE)
int ip_finish_output4(struct sk_buff *skb)
{
#if !defined(IMPROVE_QOS)
#if defined(CONFIG_NET_SCHED)
	if (gQosEnabled) {
		u_short proto = ntohs(skb->protocol);
		if(proto == ETH_P_IP){
			(list_empty(&nf_hooks[PF_INET][NF_IP_POST_ROUTING]))?: \
				ipt_do_table(skb, NF_IP_POST_ROUTING, skb->dev, NULL, \
				dev_net(skb->dev)->ipv4.iptable_mangle);
		}
	}
#endif
#endif

	if (skb->dev->flags & IFF_UP) 
	{
#if defined(CONFIG_NET_SCHED)
		if (gQosEnabled) 
		{
			// call dev_queue_xmit() instead of hard_start_xmit(), because I want the packets be sent through Traffic Control module
			dev_queue_xmit(skb);		
		}
		else            
#endif
		{

#if defined(CONFIG_BRIDGE)
			/*	In order to improve performance
			*	We'd like to directly xmit and bypass the bridge check
			*/
			if (skb->dev->priv_flags == IFF_EBRIDGE)
			{
				/*	wan->lan	*/
				struct net_bridge *br = netdev_priv(skb->dev);
				const unsigned char *dest = skb->data;
				struct net_bridge_fdb_entry *dst;

				if ((dst = __br_fdb_get(br, dest)) != NULL)
				{
					//skb->dev->stats.tx_packets++;
					//skb->dev->stats.tx_bytes += skb->len;
					skb->dev = dst->dst->dev;
				}
				else
				{
					kfree_skb(skb);
					return 0;
				}
			}
#endif
	
			skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
			skb->dev->stats.tx_packets++;
			skb->dev->stats.tx_bytes += skb->len;
			return 0;
		}

	}
	else
	{

		kfree_skb(skb);
	}
	
	return 0;
}
#endif
int FastPath_Enter(struct sk_buff **pskb)
{
	int ret;
	struct sk_buff *skb;

	skb=*pskb;
	//skb->nh.raw = skb->data;
	skb->transport_header=skb->data;
	skb->network_header = skb->data;
	//skb_reset_network_header(skb);
#ifdef CONFIG_RTL_PPPOE_DIRECT_REPLY
		if((magicNum != -1) && is_pppoe_lcp_echo_req(skb))
		{	
			if(direct_send_reply(skb)==1)
				return 1;							
		}
#endif
#if defined(CONFIG_RTL_FAST_PPPOE)
	extern int fast_pppoe_fw;
	check_and_pull_pppoe_hdr(*pskb);

	if(fast_pppoe_fw)
	{
		if((rtl_get_skb_pppoe_flag(*pskb) ==0) && 
			(strncmp(skb->dev->name,"ppp",3)==0))
			return 0;
	}

#endif

//hyking:
//bug fix:when port filter is enable,application will disable fast_nat_fw,at this moment,url filter is abnormal...
#if defined (DOS_FILTER) || defined (URL_FILTER)
		ret = filter_enter((void*)skb);
		if (ret == NF_DROP) {
			LOG_INFO("%s filter pkt, drop it\n", __FUNCTION__);
			kfree_skb(skb);
			return 1;
		}
#if	defined(CONFIG_RTL_FAST_FILTER)
	else if(ret == NF_FASTPATH)
	{
		//continue the fastpath
	}
	else if(ret == NF_OMIT)
	{
		//don't support this in driver now
		ret=0;
		goto out;
	}
	else if(ret == NF_LINUX)
	{
		//don't do rtk fastpath
		ret=0;
		goto out;
	}
#endif
	else if(ret != NF_ACCEPT)
	{
		ret=0;
		goto out;
	}
#endif

	if(!(skb->pkt_type == PACKET_HOST))
	{
		ret=0;
		goto out;
	}

	if (!fast_nat_fw)
	{
		ret=0;
		goto out;
	}

#ifdef FILTER_UPNP_BR
	if (upnp_br_enabled && skb->dev && !memcmp(skb->dev->name, RTL_PS_WAN0_DEV_NAME, 4) && filter_upnp_and_fw(skb)){
		return 1;
	}
#endif

#ifdef FAST_PPTP
	if (fast_pptp_fw) {
		fast_pptp_filter((void*)skb);
		ret = fast_pptp_to_lan((void*)&skb);
		if (ret < 0)	// error, skb has been free
		{
			return 1;
		}
		*pskb=skb;
	}
#endif

#ifdef FAST_L2TP
	if (fast_l2tp_fw)
#ifndef CONFIG_SUPPORT_RUSSIA_FEATURES
		fast_l2tp_rx((void*)skb);
#else
	{
		ret=fast_l2tp_rx((void*)skb);
		if(ret==1)
		{
			return 1;
		}
	}
#endif
#endif

	ret = enter_fast_path((void*)skb);

#if 0
	if(ret != NET_RX_DROP)
	{
		struct tcphdr *tcpudph;
		printk("-------%s(%d),ret(%d), src(0x%x),dst(0x%x), len is %d, version is %d\n",__FUNCTION__,__LINE__,ret,ip_hdr(skb)->saddr,ip_hdr(skb)->daddr, ip_hdr(skb)->ihl, ip_hdr(skb)->version);
		if(ip_hdr(skb)->protocol == IPPROTO_TCP)
		{
			tcpudph = (struct tcphdr*)((__u32 *)skb->data + ip_hdr(skb)->ihl);
			printk("===%s(%d),sport(%d),dport(%d),syn(%d),fin(%d),rst(%d)\n",__FUNCTION__,__LINE__,tcpudph->source,tcpudph->dest,tcpudph->syn,tcpudph->fin,tcpudph->rst);
		}
	}
#endif
#ifdef FAST_PPTP
	if (fast_pptp_fw && ret == 0 && ip_hdr(skb)->protocol == IPPROTO_GRE && skb->len > sizeof(struct iphdr)&& pptp_tcp_finished==1)
		if(Check_GRE_rx_net_device((void*)skb))
		{
			fast_pptp_sync_rx_seq((void*)skb);
		}
#endif


out:
#if defined(CONFIG_RTL_FAST_PPPOE)
	if(ret!=NET_RX_DROP)
	{
		check_and_restore_pppoe_hdr(skb);
	
	}
#endif
	return ret;
}

#ifdef CONFIG_FP_BYPASS_PACKET
static struct proc_dir_entry *proc_packet_bypassed=NULL;
static struct proc_dir_entry *proc_packet_bypassed_tcpport=NULL;
static struct proc_dir_entry *proc_packet_bypassed_udpport=NULL;
extern unsigned int fastpath_packet_bypassed_num;
static unsigned char *packet_bypassed_buffer = NULL;
unsigned int entered_fast_path = 0;

extern unsigned int (*rtl_init_bypass_packet_hook)(struct Path_List_Entry *entry_path);
extern unsigned int (*rtl_check_bypass_packet_hook)(struct Path_List_Entry *entry_path);
extern unsigned int rtl_init_bypass_packet(struct Path_List_Entry *entry_path);
extern unsigned int rtl_check_bypass_packet(struct Path_List_Entry *entry_path);
extern int proc_hwnat_packet_bypassed_tcpport_read_proc(char *page, char **start, off_t off,
		     int count, int *eof, void *data);
extern int proc_hwnat_packet_bypassed_tcpport_write_proc(struct file *file, const char *buffer,
		      unsigned long count, void *data);
extern int proc_hwnat_packet_bypassed_udpport_read_proc(char *page, char **start, off_t off,
		     int count, int *eof, void *data);
extern int proc_hwnat_packet_bypassed_udpport_write_proc(struct file *file, const char *buffer,
		      unsigned long count, void *data);

static int proc_packet_bypassed_read_proc(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{
	int len = 0;

	len = sprintf(page, "%d \n", fastpath_packet_bypassed_num);
	len += sprintf(page+len, "%d \n", entered_fast_path);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	return len;

}
static int proc_packet_bypassed_write_proc(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	if (count < 1)
		return -EFAULT;

	packet_bypassed_buffer = (unsigned char *)kmalloc(count,GFP_ATOMIC);
	if(!packet_bypassed_buffer)
	{
		printk("not enough memory\n");
		return -ENOMEM;
	}
	memset(packet_bypassed_buffer, 0, count);
	if (buffer && !copy_from_user(packet_bypassed_buffer, buffer, count)) {
			sscanf(packet_bypassed_buffer, "%d", &fastpath_packet_bypassed_num);
	}
	if(packet_bypassed_buffer)
		kfree(packet_bypassed_buffer);
	return count;
}
#endif
//======================================
static int __init fastpath_init(void)
{
	int			ret;
	int			buf[64];
	#ifdef CONFIG_FAST_PATH_MODULE
	fast_path_hook=FastPath_Enter;
	FastPath_hook1=rtk_delRoute;
	FastPath_hook2=rtk_modifyRoute;
	FastPath_hook3=rtk_addRoute;
	FastPath_hook4=rtk_delNaptConnection;
	FastPath_hook5=rtk_addArp;
	FastPath_hook6=rtk_addNaptConnection;
	FastPath_hook7=rtk_delArp;
	FastPath_hook8=rtk_modifyArp;
	FastPath_hook9=Get_fast_pptp_fw;
	FastPath_hook10=fast_pptp_to_wan;
	FastPath_hook11=rtk_idleNaptConnection;
	#endif

	#ifdef	DEBUG_PROCFILE
	/* proc file for debug */
	init_fastpath_debug_proc();
	#endif	/* DEBUG_PROCFILE */

#ifdef CONFIG_FP_BYPASS_PACKET
    rtl_init_bypass_packet_hook = rtl_init_bypass_packet;
	rtl_check_bypass_packet_hook = rtl_check_bypass_packet;

	proc_packet_bypassed= create_proc_entry("fastpath_packet_bypassed_num", 0, NULL);
	if (proc_packet_bypassed) {
		proc_packet_bypassed->read_proc = proc_packet_bypassed_read_proc;
		proc_packet_bypassed->write_proc = proc_packet_bypassed_write_proc;
	}

	proc_packet_bypassed_tcpport= create_proc_entry("hwnat_packet_bypassed_tcpport_set", 0, NULL);
	if (proc_packet_bypassed_tcpport) {
		proc_packet_bypassed_tcpport->read_proc = proc_hwnat_packet_bypassed_tcpport_read_proc;
		proc_packet_bypassed_tcpport->write_proc = proc_hwnat_packet_bypassed_tcpport_write_proc;
	}

	proc_packet_bypassed_udpport= create_proc_entry("hwnat_packet_bypassed_udpport_set", 0, NULL);
	if (proc_packet_bypassed_udpport) {
		proc_packet_bypassed_udpport->read_proc = proc_hwnat_packet_bypassed_udpport_read_proc;
		proc_packet_bypassed_udpport->write_proc = proc_hwnat_packet_bypassed_udpport_write_proc;
	}
#else
	extern unsigned int (*rtl_init_bypass_packet_hook)(struct Path_List_Entry *entry_path);
	extern unsigned int (*rtl_check_bypass_packet_hook)(struct Path_List_Entry *entry_path);
	rtl_init_bypass_packet_hook  = NULL;
	rtl_check_bypass_packet_hook = NULL;
#endif

	#ifndef NO_ARP_USED
	/* Arp-Table Init */
	ret=init_table_arp(ARP_TABLE_LIST_MAX,ARP_TABLE_ENTRY_MAX);
	if(ret!=0) {
		DEBUGP_SYS("init_table_arp Failed!\n");
	}
	#endif

	#ifndef DEL_ROUTE_TBL
	/* Route-Table Init */
	ret=init_table_route(ROUTE_TABLE_LIST_MAX, ROUTE_TABLE_ENTRY_MAX);
	if(ret!=0) {
		DEBUGP_SYS("init_table_route Failed!\n");
	}
	#endif

	#ifndef DEL_NAPT_TBL
	/* Napt-Table Init */
	ret=init_table_napt(NAPT_TABLE_LIST_MAX, NAPT_TABLE_ENTRY_MAX);
	if(ret!=0)
	{
		DEBUGP_SYS("init_table_napt Failed!\n");
	}
	#endif

	/* Path-Table Init */
	ret=init_table_path(PATH_TABLE_LIST_MAX, PATH_TABLE_ENTRY_MAX);
	if(ret!=0) {
		DEBUGP_SYS("init_table_path Failed!\n");
	}

	#ifdef CONFIG_UDP_FRAG_CACHE
	if(!udp_fragCache_init(MAX_UDP_FRAG_ENTRY))
		return -1;
	
	if(!negative_fragCache_init())
		return -1;
	#endif

	#ifdef DOS_FILTER
	filter_init();
	#endif

	#ifdef FAST_PPTP
	fast_pptp_init();
	#endif

	#ifdef FAST_L2TP
	fast_l2tp_init();
	#endif
	
	#if defined (CONFIG_RTL_FAST_PPPOE)
	fast_pppoe_init();
	#endif

	#ifdef CONFIG_PROC_FS
	res1=create_proc_entry("fast_nat",0,NULL);
	if (res1) {
	    res1->read_proc=read_proc;
	    res1->write_proc=write_proc;
	}
	res2=create_proc_entry("ext_service_ports",0,NULL);
	if (res2) {
	    res2->read_proc=ext_service_ports_read_proc;
	    res2->write_proc=ext_service_ports_write_proc;
	}
	#endif


	#if defined(FAST_PATH_SPI_ENABLED)
	res_spi = create_proc_entry("fast_spi",0,NULL);
	if(res_spi){
		res_spi->read_proc = spi_read_proc;
		res_spi->write_proc = spi_write_proc;
	}
	#endif

	#if defined(CONFIG_RTL_NF_CONNTRACK_GARBAGE_NEW)
	rtl_fp_gc_rx_threshold = RTL_FP_SESSION_LEVEL3_ALLOW_COUNT;
	#endif
	
	get_fastpath_module_info(buf);
	panic_printk("%s",buf);

	return 0;
}

static void __exit fastpath_exit(void)
{
#ifdef CONFIG_FAST_PATH_MODULE
	fast_path_hook=NULL;
	FastPath_hook1=NULL;
	FastPath_hook2=NULL;
	FastPath_hook3=NULL;
	FastPath_hook4=NULL;
	FastPath_hook5=NULL;
	FastPath_hook6=NULL;
	FastPath_hook7=NULL;
	FastPath_hook8=NULL;
	FastPath_hook9=NULL;
	FastPath_hook10=NULL;
	FastPath_hook11=NULL;
#endif

#ifdef	DEBUG_PROCFILE
	/* proc file for debug */
	remove_fastpath_debug_proc();
#endif	/* DEBUG_PROCFILE */

#ifdef DOS_FILTER
	filter_exit();
#endif

#ifdef FAST_PPTP
	fast_pptp_exit();
#endif

#if defined (CONFIG_RTL_FAST_PPPOE)
	fast_pppoe_exit();
#endif

#ifdef CONFIG_PROC_FS
	if (res1) {
		remove_proc_entry("fast_nat", res1);
		res1 = NULL;
	}
	if (res2) {
		remove_proc_entry("ext_service_ports", res2);
		res2 = NULL;
	}
#endif

#if defined(FAST_PATH_SPI_ENABLED)
		if(res_spi){
			remove_proc_entry("fast_spi", res_spi);
			res_spi = NULL;
		}
#endif



	//printk("%s %s removed!\n", MODULE_NAME, MODULE_VERSION);
}


// direct tx ppp reply ptk to l2tp server
#ifdef CONFIG_SUPPORT_RUSSIA_FEATURES
extern unsigned int l2tp_ppp_imagic;
extern struct l2tp_info l2tpInfo;
#endif

int Direct_Send_Reply(struct sk_buff * skb, int offset)
{
#ifdef CONFIG_SUPPORT_RUSSIA_FEATURES
	int header_len;
	struct iphdr *iph,*iph_new, iph_newone;
	struct Rus_l2tp_ext_hdr *l2tph, l2tphone;
	unsigned char tos;
	unsigned short frag_off;
	struct sk_buff *new_skb;
	// var define ---------
	unsigned char *data;
	data = rtl_get_skb_data(skb);	
	unsigned char req_id;
	unsigned short req_len;

	//if the info is integrity, conitune to contruct the ptk.
	if(!fast_l2tp_fw || l2tpInfo.tid==0 || l2tpInfo.cid==0 || !l2tpInfo.wan_dev)
		return 0;

	if(l2tpInfo.valid != 1 || !l2tp_ppp_imagic)
		return 0;

	//extract necessary info
	{
		req_id = data[offset+1];
		req_len = *((unsigned short*)(&data[offset+2]));
	}

	//contruct the ptk
	skb_push(skb,ETH_HLEN);			//mac header push 
	{
		// build the mac header 
		memcpy(skb->data, l2tpInfo.mac_header, ETH_HLEN);
		
		// build ip header
		iph_new = &iph_newone;
		iph_new->version	=	4;
		iph_new->ihl		=	sizeof(struct iphdr) >> 2;
		//iph_new->frag_off =	frag_off;
		iph_new->frag_off	=	0x4000;
		iph_new->protocol	=	IPPROTO_UDP;

		//need confirm the value --------------------------------------
		iph_new->tos		=	0;
		
		iph_new->daddr	=	l2tpInfo.daddr;
		iph_new->saddr	=	l2tpInfo.saddr;
		
		iph_new->ttl		=	IPDEFTTL;
		skb->ip_summed	=	CHECKSUM_NONE;
		iph_new->tot_len	=	htons(skb->len - ETH_HLEN);
		iph_new->id 	=	0;

		iph_new->check	=	0;
		iph_new->check	=	ip_fast_csum((unsigned char *)iph_new, iph_new->ihl);	
		memcpy(skb->data + ETH_HLEN, &iph_newone, sizeof(iph_newone));

		// build udp header
		l2tph = &l2tphone;
		l2tph->source	=1701;
		l2tph->dest =1701;
		//len ,need to confirm to modify
		if(req_len >8)
			l2tph->len	= 30;	
		else
			l2tph->len	= 26;				
		l2tph->checksum=0;
		
		//build l2tp header
		l2tph->type =0x0002;
		l2tph->tid	=l2tpInfo.tid;
		l2tph->cid	=l2tpInfo.cid;
		
		//build ppp header
		l2tph->addr_control= 0xff03;
		l2tph->protocol =0xc021;
		l2tph->code = 0x0a;
		l2tph->id = req_id;
		l2tph->lcp_length= req_len;
		//message && imagicNumber has the same value
		l2tph->imagicNumber = l2tp_ppp_imagic;
		if(req_len > 8 )
		l2tph->message = l2tp_ppp_imagic;
		
		memcpy(skb->data+ETH_HLEN+sizeof(struct iphdr), &l2tphone, sizeof(struct Rus_l2tp_ext_hdr));

		//reset the skb info
		skb->len = ETH_HLEN+sizeof(struct iphdr)+sizeof(struct Rus_l2tp_ext_hdr);
		
		//set the ethx device to xmit
		skb->dev=l2tpInfo.wan_dev;
		l2tpInfo.last_xmit = jiffies;

		//panic_printk("%s[%d] --\n",__FUNCTION__,__LINE__);
		skb->dev->netdev_ops->ndo_start_xmit(skb,skb->dev);
		//kfree_skb(skb);
		return 1;		
	}
#endif
}

int fast_l2tp_tx_lcp_echo_reply(unsigned char *data, int rightShift, void *skb)
{	
#ifndef CONFIG_SUPPORT_RUSSIA_FEATURES
	return 0;

#else
	//patch for russia ppp disconnect 
	__u16 *ppp_protocol;
	ppp_protocol = &data[36+rightShift];
	
	if(*ppp_protocol == 0xc021)
	{
		if( *((u_int8_t *)(((u_int8_t *)ppp_protocol) + 2)) == 0x09)//request
		{
			//contruct the reply ptk to eth dirver to send		
			Direct_Send_Reply(skb,38+rightShift);
			return 1;
		}
	}
	
	return 0;
#endif
}

module_init(fastpath_init);
module_exit(fastpath_exit);
MODULE_LICENSE("GPL");

