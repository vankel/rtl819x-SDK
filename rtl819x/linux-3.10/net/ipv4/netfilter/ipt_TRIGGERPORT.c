/* Trigger Port.If match the port-ranges, then trigger related port-ranges,
 * and alters the destination to a local IP address.
 *
 * (C) 1999-2001 Paul `Rusty' Russell
 * (C) 2002-2006 Netfilter Core Team <coreteam@netfilter.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

//#define DEBUG 1

//#include <linux/autoconf.h>
#include <linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>
#include <net/protocol.h>
#include <net/checksum.h>

//#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv4/ipt_TRIGGERPORT.h>

#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_core.h>
#include <net/netfilter/nf_conntrack_tuple.h>
#include <net/netfilter/nf_conntrack_expect.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <net/netfilter/nf_conntrack_helper.h>
#include <net/netfilter/nf_nat.h>
#define ASSERT_READ_LOCK(x)
#define ASSERT_WRITE_LOCK(x)
#include <uapi/linux/netfilter_ipv4/listhelp.h>
#include <linux/seq_file.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("iptables trigger port module");

#ifdef DEBUG
#undef  pr_debug
#define pr_debug(fmt, args...) printk("[%s:%d] "fmt, __FUNCTION__, __LINE__, ##args)
#endif

/* This rwlock protects the main hash table, protocol/helper/expected
 *    registrations, conntrack timers*/

struct rtl_ipt_trigger_port {
    struct list_head list;		/* Trigger list */
    struct timer_list timeout;	/* Timer for list destroying */
    u_int32_t srcip;		/* Outgoing source address */
    u_int32_t dstip;		/* Outgoing destination address */
    u_int16_t mproto;		/* Trigger protocol */
    u_int16_t rproto;		/* Related protocol */
    struct rtl_ipt_trigger_ports ports;	/* Trigger and related ports */
    u_int8_t reply;			/* Confirm a reply connection */
};

static unsigned int rtl_trigger_port_timeout = 600;	/* 600s */
static DEFINE_RWLOCK(rtl_trigger_lock);
static LIST_HEAD(rtl_trigger_list);
static void rtl_trigger_port_timer_func(unsigned long trig_entry);


static inline int rtl_trigger_port_in_matched(const struct rtl_ipt_trigger_port *trig,
        const u_int16_t dPort, const u_int16_t proto)
{
    pr_debug("trig=%p, proto= %d, dPort=%d.\n", trig, proto, dPort);
    pr_debug("In: match one entry, rProto= %d, rPort[0..1]=%d-%d.\n",
            trig->rproto, trig->ports.rport[0], trig->ports.rport[1]);

    return (((trig->ports.rport[0] <= dPort) && (trig->ports.rport[1] >= dPort))&&
		(trig->rproto ==0 || trig->rproto == proto));
}

static inline int rtl_trigger_port_out_matched(const struct rtl_ipt_trigger_port *trig,
        const u_int16_t dPort, const struct rtl_ipt_trigger_info *info, const u_int16_t proto)
{
    pr_debug("trig=%p, proto=%d, dPort=%d, rProto=%d, rPort[0..1]=%d-%d.\n",
        trig, proto, dPort, info->rproto, info->ports.rport[0], info->ports.rport[1]);
    pr_debug("Out: match one entry, mproto= %d, mport[0..1]=%d-%d.\n",
        trig->mproto, trig->ports.mport[0], trig->ports.mport[1]);

    return ((trig->mproto == 0 || trig->mproto == proto)
	 && (trig->rproto == info->rproto)
        && ((trig->ports.mport[0] <= dPort) && (trig->ports.mport[1] >= dPort))
        && ((trig->ports.rport[0] == info->ports.rport[0]) && (trig->ports.rport[1] == info->ports.rport[1])));
}

static unsigned int rtl_trigger_port_add(struct rtl_ipt_trigger_port *trig_port)
{
    struct rtl_ipt_trigger_port *new_trigger_port;

    pr_debug("Trigger port add, srcIp= %pI4, dstIp= %pI4, mProto= %d, mPort[0..1]=%d-%d, rProto= %d, rPort[0..1]=%d-%d.\n",
            &(trig_port->srcip), &(trig_port->dstip),
            trig_port->mproto, trig_port->ports.mport[0], trig_port->ports.mport[1],
            trig_port->rproto, trig_port->ports.rport[0], trig_port->ports.rport[1]);

    write_lock_bh(&rtl_trigger_lock);
    new_trigger_port = (struct rtl_ipt_trigger_port *)kmalloc(sizeof(struct rtl_ipt_trigger_port), GFP_ATOMIC);
    if (!new_trigger_port) {
        write_unlock_bh(&rtl_trigger_lock);
        pr_debug("OOM allocating trigger list\n");
        return -ENOMEM;
    }

    memset(new_trigger_port, 0, sizeof(*trig_port));
    INIT_LIST_HEAD(&new_trigger_port->list);
    memcpy(new_trigger_port, trig_port, sizeof(*trig_port));

    /* add to global table of trigger */
    list_prepend(&rtl_trigger_list, &new_trigger_port->list);

    /* add and start timer if required */
    init_timer(&new_trigger_port->timeout);
    new_trigger_port->timeout.function = rtl_trigger_port_timer_func;
    new_trigger_port->timeout.data = (unsigned long)new_trigger_port;
    new_trigger_port->timeout.expires = jiffies + (rtl_trigger_port_timeout * HZ);
    add_timer(&new_trigger_port->timeout);

    write_unlock_bh(&rtl_trigger_lock);

    return 0;
}

static void rtl_trigger_port_del(struct rtl_ipt_trigger_port *trig_port)
{
    NF_CT_ASSERT(trig_port);
    ASSERT_WRITE_LOCK(&rtl_trigger_lock);

    pr_debug("Trigger port delete, srcIp= %pI4, dstIp= %pI4, mProto= %d, mPort[0..1]=%d-%d, rProto= %d, rPort[0..1]=%d-%d.\n",
            &(trig_port->srcip), &(trig_port->dstip),
            trig_port->mproto, trig_port->ports.mport[0], trig_port->ports.mport[1],
            trig_port->rproto, trig_port->ports.rport[0], trig_port->ports.rport[1]);

    /* delete from 'rtl_trigger_list' */
    list_del(&trig_port->list);
    kfree(trig_port);
}


static void rtl_trigger_port_refresh(struct rtl_ipt_trigger_port *trig_port, unsigned long extra_times)
{
    NF_CT_ASSERT(trig_port);
	 pr_debug("Rrigger port refresh, srcIp= %pI4, dstIp= %pI4, mProto= %d, mPort[0..1]=%d-%d, rProto= %d, rPort[0..1]=%d-%d.\n",
            &(trig_port->srcip), &(trig_port->dstip),
            trig_port->mproto, trig_port->ports.mport[0], trig_port->ports.mport[1],
            trig_port->rproto, trig_port->ports.rport[0], trig_port->ports.rport[1]);
    write_lock_bh(&rtl_trigger_lock);
    /* Need del_timer for race avoidance (may already be dying). */
    if (del_timer(&trig_port->timeout)) {
        trig_port->timeout.expires = jiffies + extra_times;
        add_timer(&trig_port->timeout);
    }

    write_unlock_bh(&rtl_trigger_lock);
}

static void rtl_trigger_port_timer_func(unsigned long trig_entry)
{
    struct rtl_ipt_trigger_port *trig_port= (void *) trig_entry;

    write_lock_bh(&rtl_trigger_lock);
    rtl_trigger_port_del(trig_port);
    write_unlock_bh(&rtl_trigger_lock);
    pr_debug("Trigger entry[%p] timed out.\n", trig_port);

}

static unsigned int rtl_trigger_port_out(struct sk_buff *skb, const struct xt_action_param *par)
{
    struct list_head *cur_entry, *tmp_entry;
    const struct rtl_ipt_trigger_info *info = par->targinfo;
    struct rtl_ipt_trigger_port trig;
    struct rtl_ipt_trigger_port *found = NULL;
    const struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;

    /*Check if the entry is already exist.*/
    list_for_each_safe(cur_entry, tmp_entry, &rtl_trigger_list) {
        struct rtl_ipt_trigger_port *trig_port = (void *)cur_entry;
        if (rtl_trigger_port_out_matched(trig_port, ntohs(tcph->dest), info, iph->protocol)) {
	    	found = trig_port;
		    break;
	    }
    }

    pr_debug("############# trigger port out found ############\n");

    if (found==NULL) {
        /* alloc new trigger port entry*/
        memset(&trig, 0, sizeof(trig));
        trig.mproto = info->mproto;
	 trig.srcip = iph->saddr;
        trig.rproto = info->rproto;
        memcpy(&trig.ports, &info->ports, sizeof(struct rtl_ipt_trigger_ports));

	 pr_debug("new trigger:srcIp=%pI4 dstIp=%pI4 mProto=%d rProto=%d mPort[%d-%d] rPort[%d-%d]\n",
            &(trig.srcip), &(trig.dstip), trig.mproto, trig.rproto,
            trig.ports.mport[0], trig.ports.mport[1],
            trig.ports.rport[0], trig.ports.rport[1]);

        rtl_trigger_port_add(&trig);
    }
    else{
        /* Update timer */
        rtl_trigger_port_refresh(found, rtl_trigger_port_timeout * HZ);

        /* Allow multiple hosts share the same port range. So update the trigger port
		sip as the new one.*/
        if (found->reply)
            found->srcip = iph->saddr;
    }

    return XT_CONTINUE;
}

static unsigned int rtl_trigger_port_in(struct sk_buff *skb, const struct xt_action_param *par)
{
    struct list_head *cur_entry, *tmp_entry;
    struct rtl_ipt_trigger_port *found = NULL;
    const struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = (void *)iph + iph->ihl * 4;

    list_for_each_safe(cur_entry, tmp_entry, &rtl_trigger_list) {
        struct rtl_ipt_trigger_port *trig_port = (void *)cur_entry;
        if (rtl_trigger_port_in_matched(trig_port, ntohs(tcph->dest), iph->protocol)) {
			found = trig_port;
			break;
		}
    }
   
    if (found) {
        pr_debug("#############trigger port in found ############\n");

        /* Update(delay) the destroying timer. */
        rtl_trigger_port_refresh(found, rtl_trigger_port_timeout * HZ);

        /* As the in packets may dropped in the FORWARD chain.*/
        return NF_ACCEPT;
    }

    return XT_CONTINUE;	
}

static unsigned int rtl_trigger_port_dnat(struct sk_buff *skb, const struct xt_action_param *par)
{
    struct nf_conn *ct;
    struct list_head *cur_entry, *tmp_entry;
    const struct iphdr *iph = ip_hdr(skb);
    struct tcphdr *tcph = (void *)iph + iph->ihl * 4;
    struct nf_nat_ipv4_range newrange;
    enum ip_conntrack_info ctinfo;
    struct rtl_ipt_trigger_port *found = NULL;

    NF_CT_ASSERT(par->hooknum == NF_IP_PRE_ROUTING);
    
    list_for_each_safe(cur_entry, tmp_entry, &rtl_trigger_list) {
        struct rtl_ipt_trigger_port *trig_port = (void *)cur_entry;
        if (rtl_trigger_port_in_matched(trig_port, ntohs(tcph->dest), iph->protocol)) {
			found = trig_port;
			break;
		}
    }
    if (!found || !found->srcip)
    {
        return XT_CONTINUE;
    }

    pr_debug("############# trigger port dnat found ############\n");
    ct = nf_ct_get(skb, &ctinfo);
    found->reply = 1;
    NF_CT_ASSERT(ct && (ctinfo == IP_CT_NEW));

    BUG_ON(ct->status & IPS_NAT_DONE_MASK);
    pr_debug("hooknum=%d got ", par->hooknum);
    nf_ct_dump_tuple(&ct->tuplehash[IP_CT_DIR_ORIGINAL].tuple);

    skb->nfctinfo = IP_CT_RELATED;
    newrange = ((struct nf_nat_ipv4_range)
    {
        NF_NAT_RANGE_MAP_IPS,
        found->srcip, found->srcip,
        { 0 }, { 0 }
    });

    return nf_nat_setup_info(ct, &newrange, NF_NAT_MANIP_DST);
}

static unsigned int rtl_trigger_port_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
    const struct iphdr *iph = ip_hdr(skb);
    const struct rtl_ipt_trigger_info *info = par->targinfo;
    struct tcphdr *tcph = (void *)iph + iph->ihl*4;	/* Might be TCP, UDP */

	pr_debug("type=%s, protocol=%s, dPort=%d\n",
        (info->type == IPT_TRIGGER_OUT) ? "out"
            :(info->type == IPT_TRIGGER_IN) ? "in" : "dnat",
        (iph->protocol == IPPROTO_TCP) ? "tcp" : "udp",
        ntohs(tcph->dest));

    /* The trigger port only supports TCP and UDP. */
    if ((iph->protocol != IPPROTO_UDP) && (iph->protocol != IPPROTO_TCP))
        return XT_CONTINUE;

    if ((info->type == IPT_TRIGGER_IN) || (info->type == IPT_TRIGGER_DNAT))
    {
        if (info->ports.rport[0])
        {
            if ((ntohs(tcph->dest) < info->ports.rport[0] || ntohs(tcph->dest) > info->ports.rport[1])||
		   (info->rproto != 0 && info->rproto != iph->protocol))
                return XT_CONTINUE;
        }

       
        if (info->type == IPT_TRIGGER_DNAT) 
            return rtl_trigger_port_dnat(skb, par);
        else  if (info->type == IPT_TRIGGER_IN)
            return rtl_trigger_port_in(skb, par);
    }
    else if (info->type == IPT_TRIGGER_OUT)
    {
        if ((ntohs(tcph->dest) < info->ports.mport[0] || ntohs(tcph->dest) > info->ports.mport[1])||
		(info->mproto != 0 && info->mproto != iph->protocol))
            return XT_CONTINUE;

        return rtl_trigger_port_out(skb, par);
    }
    

    return XT_CONTINUE;
}

static bool rtl_trigger_port_tg_check(const struct xt_tgchk_param *par)
{
    const struct rtl_ipt_trigger_info *info = par->targinfo;

    if (info==NULL) {
        pr_debug("par->targinfo is NULL!\n");
        return -EINVAL;
    }

    if (par->hook_mask & ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD))) {
        pr_debug("Trigger port not support hooks %x, %x.\n", par->hook_mask, ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD)));
        return -EINVAL;
    }
    if ((strncmp(par->table, "mangle", 6) == 0)) {
        pr_debug("Trigger port not support table `%s'.\n", par->table);
        return -EINVAL;
    }

    if (info->rproto) {
        if (info->rproto != IPPROTO_UDP && info->rproto != IPPROTO_TCP) {
            pr_debug("Trigger port not support rproto %d.\n", info->rproto);
            return -EINVAL;
        }
    }

    if (info->mproto) {
        if (info->mproto != IPPROTO_UDP && info->mproto != IPPROTO_TCP) {
            pr_debug("Trigger port not support mproto %d.\n", info->mproto);
            return -EINVAL;
        }
    }
	
    if ((info->type==IPT_TRIGGER_DNAT) ||(info->type ==IPT_TRIGGER_IN))
    {
        if (info->mproto || info->ports.mport[0]) {
            pr_debug("When trigger-type is 'in' or 'dnat', mproto and mport is not needed\n");
            return -EINVAL;
        }
    } else if(info->type == IPT_TRIGGER_OUT) {
        if (!info->ports.mport[0] || !info->ports.rport[0]) {
            pr_debug("Help 'iptables -j TRIGGER -h'.\n");
            return -EINVAL;
        }
    }

    return 0;
}

/* drop reference count of cluster config when rule is deleted */
static void rtl_trigger_port_tg_destory(const struct xt_tgdtor_param *par)
{
}

static int rtl_trigger_port_write( struct file *filp, const char __user *buf,unsigned long len, void *data )
{
	struct list_head *cur_entry, *tmp_entry;
	int ret;
	char buf_tmp[256];
	int val = 0;
       char value[20] = {0};
	
	if(len > 0xff)
	{
		printk("The len is too big!\n");
		return len;
	}

	copy_from_user(buf_tmp, buf, len);
	buf_tmp[len] = '\0';

	ret = sscanf(buf_tmp, "%s %d", value, (int*)&val);
       if(val < 0 ||ret != 2)
	{
		printk("Cmd sample: echo expire 10 > /proc/rtl_triggerPort \n");
		return len;
	}
	  
	if (strncmp(value, "flush", 5) == 0)
       {
	        pr_debug("Flush trigger port list.\n");

	        /* Empty the 'rtl_trigger_list' */
	        list_for_each_safe(cur_entry, tmp_entry, &rtl_trigger_list) {
	            struct rtl_ipt_trigger_port *trig_port = (void *)cur_entry;
	            del_timer(&trig_port->timeout);
	            rtl_trigger_port_del(trig_port);
	        }
    	}
	else if (strncmp(value, "expire", 6) == 0)
	{
	        if(val < 1 || val > 9999)
	    	{
	    		printk("The expire value is too big, it must be 1-9999 minutes\n");
	    		return len;
	    	}
	    	rtl_trigger_port_timeout = val * 60;
	}
    	else
    	{
        	printk("Invalid cmd.\n");
   	}

	return len;
}


static int rtl_trigger_port_read(struct seq_file *s, void *v)
{
    struct list_head *cur_entry, *tmp_entry;
    int i = 0;

    seq_printf(s, "Trigger port expire value: %d\n", rtl_trigger_port_timeout/60);
    seq_printf(s, "Trigger port list:\n");

    /* Read the 'rtl_trigger_list' */
    list_for_each_safe(cur_entry, tmp_entry, &rtl_trigger_list) {
        struct rtl_ipt_trigger_port *trig_port = (void *)cur_entry;
        seq_printf(s, "[%d] srcIp= %pI4, dstIp= %pI4, mProto= %d, mPort[0..1]=%d-%d, rProto= %d, rPort[0..1]=%d-%d.\n",
                    ++i, &(trig_port->srcip), &(trig_port->dstip),
                    trig_port->mproto, trig_port->ports.mport[0], trig_port->ports.mport[1],
                    trig_port->rproto, trig_port->ports.rport[0], trig_port->ports.rport[1]);
    }

    return 0;
}

int rtl_trigger_port_single_open(struct inode *inode, struct file *file)
{
        return(single_open(file, rtl_trigger_port_read, NULL));
}

static ssize_t rtl_trigger_port_single_write(struct file * file, const char __user * userbuf,
		     size_t count, loff_t * off)
{
	return rtl_trigger_port_write(file, userbuf,count, off);
}


struct file_operations rtl_trigger_port_proc_fops = {
        .open           = rtl_trigger_port_single_open,
	 .write		= rtl_trigger_port_single_write,
        .read           = seq_read,
        .llseek         = seq_lseek,
        .release        = single_release,
};

extern struct proc_dir_entry proc_root;
static struct proc_dir_entry *rtl_trigger_port_entry = NULL;

static struct xt_target rtl_trigger_port_tg_reg __read_mostly = {
	.name		= "TRIGGER",
	.family		= NFPROTO_IPV4,
	.target		= rtl_trigger_port_tg,
	.targetsize	= sizeof(struct rtl_ipt_trigger_info),
	.checkentry	= rtl_trigger_port_tg_check,
	.destroy	= rtl_trigger_port_tg_destory,
	.me		= THIS_MODULE,
};

static int __init rtl_trigger_port_tg_init(void)
{
	rtl_trigger_port_entry = proc_create_data("rtl_triggerPort", 0, &proc_root,
			 &rtl_trigger_port_proc_fops, NULL);

    return xt_register_target(&rtl_trigger_port_tg_reg);
}

static void __exit rtl_trigger_port_tg_exit(void)
{
    if (rtl_trigger_port_entry) {
        remove_proc_entry("rtl_triggerPort", &proc_root);
	rtl_trigger_port_entry = NULL;
    }

    xt_unregister_target(&rtl_trigger_port_tg_reg);
}

module_init(rtl_trigger_port_tg_init);
module_exit(rtl_trigger_port_tg_exit);

