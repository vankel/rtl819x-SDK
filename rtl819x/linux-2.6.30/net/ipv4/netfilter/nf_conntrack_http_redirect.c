#include <linux/init.h>
#include <asm/system.h>
#include <linux/sched.h>
#include <linux/socket.h>
#include <linux/net.h>
#include <linux/un.h>
#include <linux/in.h>
#include <linux/inet.h>
#include <linux/config.h>
#include <linux/ctype.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <net/tcp.h>
#include <net/route.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter/nf_conntrack_tuple_common.h>
#include <linux/netfilter/nf_conntrack_common.h>
//#include <linux/netfilter_ipv4/ip_conntrack_core.h>
#include <linux/skbuff.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/if.h>
#include <linux/spinlock.h>
#include <linux/inetdevice.h>
#ifdef CONFIG_BRIDGE_NETFILTER
#include <linux/netfilter_bridge.h>
#endif
#define HTTP_REDIRECT_ROOT_PROC "http_redirect"
#define HTTP_REDIRECT_ENABLE_PROC "enable"
#define HTTP_REDIRECT_URL_PROC "url"
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
#define HTTP_REDIRECT_MACLIST_PROC "mac_list"
#endif
#if 0
#include "../../bridge/br_private.h"  //for br_get_all_addr
#endif

#ifdef CONFIG_PROC_FS
struct proc_dir_entry *r_root_proc = NULL;
static struct proc_dir_entry *r_enable_proc = NULL;
static struct proc_dir_entry *r_url_proc = NULL;
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
static struct proc_dir_entry *r_maclist_proc = NULL;
#endif
#endif
/*
	notice: if you want change the redirect or Mac information , 
		you must turn off enableForce, after you set, you 
		can tuen on the enableForce, or race may be happen
*/

MODULE_LICENSE("GPL");
MODULE_AUTHOR("huanghongbo <huanghongbo@twsz.com>");
MODULE_DESCRIPTION("foce portal module");
#if 0
#define DEBUGP printk
#define DUMP_CONTENT(dptr, length) \
	{\
	  int i;\
	  DEBUGP("\n*************************DUMP***********************\n");\
	  for (i=0;i<length;i++)\
	    DEBUGP("%c",dptr[i]);\
	  DEBUGP("*************************DUMP OVER******************\n");\
	}
#define DUMP_TUPLE_TCP(tp) \
	DEBUGP("tuple(tcp) %p: %u %u.%u.%u.%u:%u -> %u.%u.%u.%u:%u\n",	\
	       (tp), (tp)->dst.protonum,				\
	       NIPQUAD((tp)->src.ip), ntohs((tp)->src.u.tcp.port),		\
	       NIPQUAD((tp)->dst.ip), ntohs((tp)->dst.u.tcp.port))
#else
#define DEBUGP(format, args...)
#define DUMP_CONTENT(dptr, length) 
#define DUMP_TUPLE_TCP(tp) 
#endif

#define HTTP_PORT 80
/* This is slow, but it's simple. --RR */
static char http_buffer[16384];

u_int32_t devIP = 0;
u_int32_t devMask = 0;

static char LanIP[50] ;
static int redirect_on=1;
unsigned char defaultUrl[128];
#define DEFAULT_URL "www.realsil.com.cn"

#define MAX_RES_STR_LEN 256
#define MAX_HOST_STR_LEN 256

#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
char* pszHttpRedirectHead = 
	"HTTP/1.1 200 OK\r\n"
	"Server: router-gateway\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: %d\r\n"
	"Connection: Close\r\n"
	"\r\n"
	"%s";
char* pszHttpRedirectContent = 
	"<head><head><meta http-equiv=\"Content-Type\" content=\"text/html\">"
	"<meta http-equiv=\"refresh\" content=\"0; url=http://%s\">"
	"<title>Please wait...</title>"
	"</head></html>";
#else
char* pszHttpRedirectHead = 
	"HTTP/1.1 302 Object Moved\r\n"
	"Location: http://%s\r\n"
	"Server: router-gateway\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: %d\r\n"
	"\r\n"
	"%s";
char* pszHttpRedirectContent = 
	"<html><head><title>Object Moved</title></head>"
	"<body><h1>Object Moved</h1>This Object may be found in "
	"<a HREF=\"http://%s\">here</a>.</body><html>";
#endif	
DEFINE_SPINLOCK(guest_dev_list_lock);
#define LOCK_BH(l) spin_lock_bh(l)
#define UNLOCK_BH(l) spin_unlock_bh(l)
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
#define MAX_FIXED_ENTRY_NUM 100
#define MAX_DYNAMIC_ENTRY_NUM 100
struct list_head fixed_mac_head;
struct list_head dynamic_mac_head;
unsigned int fixed_mac_num = 0;
unsigned int dynamic_mac_num = 0;
typedef struct mac_entry{
	unsigned char mac[6];/*mac address*/
	struct list_head list;
}mac_entry;

static int c_is_hex(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||
            ((c >= 'a') && (c <= 'f')));
}

static int mac_string_to_hex(char *string, unsigned char *key, int len)
{
	char tmpBuf[4];
	int idx, ii=0;
	for (idx=0; idx<len; idx+=2) {
		tmpBuf[0] = string[idx];
		tmpBuf[1] = string[idx+1];
		tmpBuf[2] = 0;
		if ( !c_is_hex(tmpBuf[0]) || !c_is_hex(tmpBuf[1]))
			return 0;

		key[ii++] = (unsigned char) simple_strtol(tmpBuf, (char**)NULL, 16);
	}
	return 1;
}
static int is_mac_in_list(unsigned char* mac)
{
	struct mac_entry *tmp_entry;
	struct list_head *pos;
	if(mac == NULL)
	{
		return -1;
	}
	list_for_each(pos,&fixed_mac_head){/*fixed entry*/
		tmp_entry = list_entry(pos,mac_entry,list);
		if(memcmp(mac,tmp_entry->mac,6)==0){
			return 1;
		}
	}
	list_for_each(pos,&dynamic_mac_head){/*dynamic entry*/
		tmp_entry = list_entry(pos,mac_entry,list);
		if(memcmp(mac,tmp_entry->mac,6)==0){
			return 1;
		}
	}
	return 0;
}
static int add_mac_to_list(unsigned char* mac,unsigned int mode)
{
	mac_entry *new_entry,*tmp_entry;
	struct list_head *temp;
	if(mac==NULL)
	{
		return -1;
	}
	if(is_mac_in_list(mac))
	{
		return 0;
	}
	if(mode == 0)/*fixed entry*/
	{
		new_entry = kmalloc(sizeof(mac_entry), GFP_KERNEL);
		if(new_entry == NULL)
		{
			return -1;
		}
		INIT_LIST_HEAD(&new_entry->list);
		memcpy(new_entry->mac,mac,6);
		list_add_tail(&new_entry->list,&fixed_mac_head);
		fixed_mac_num ++;
		
	}
	else if(mode == 1)/*dynamic entry*/
	{
		if(dynamic_mac_num >= MAX_DYNAMIC_ENTRY_NUM)
		{
			temp = dynamic_mac_head.next;
			tmp_entry = list_entry(temp,mac_entry,list);
			list_del(&tmp_entry->list);
			kfree(tmp_entry);
			tmp_entry = NULL;
			dynamic_mac_num --;
		}
		new_entry = kmalloc(sizeof(mac_entry), GFP_KERNEL);
		if(new_entry == NULL)
		{
			return -1;
		}
		INIT_LIST_HEAD(&new_entry->list);
		memcpy(new_entry->mac,mac,6);
		list_add_tail(&new_entry->list,&dynamic_mac_head);
		dynamic_mac_num ++;
	}
	return 0;
}
static int delete_mac_from_list(unsigned char* mac,unsigned int mode)
{
	mac_entry *new_entry,*tmp_entry;
	struct list_head *q,*n;
	if(mac == NULL)
	{
		return -1;
	}
	if(mode == 0)/*fixed entry*/
	{		
		if(!list_empty(&fixed_mac_head)){
			list_for_each_safe(q,n,&fixed_mac_head){
				tmp_entry = list_entry(q,mac_entry,list);
				if(memcmp(tmp_entry->mac,mac,6)==0){
					list_del(&tmp_entry->list);
					kfree(tmp_entry);
					fixed_mac_num --;
				}
			}
		}
	}
	else if(mode == 1)/*dynamic entry*/
	{
		if(!list_empty(&dynamic_mac_head)){
			list_for_each_safe(q,n,&dynamic_mac_head){
				tmp_entry = list_entry(q,mac_entry,list);
				if(memcmp(tmp_entry->mac,mac,6)==0){
					list_del(&tmp_entry->list);
					kfree(tmp_entry);
					dynamic_mac_num --;
				}
			}
		}
	}
	return 0;
}
static int flush_mac_list(unsigned int mode)
{
	mac_entry *new_entry,*tmp_entry;
	struct list_head *q,*n;
	if(mode == 0)/*flush fixed mac entry*/
	{
		if(!list_empty(&fixed_mac_head)){
			list_for_each_safe(q,n,&fixed_mac_head){
				tmp_entry = list_entry(q,mac_entry,list);
					list_del(&tmp_entry->list);
					kfree(tmp_entry);
			}
		}
	}
	else if(mode ==1)/*flush dynamic mac entry*/
	{
		if(!list_empty(&dynamic_mac_head)){
			list_for_each_safe(q,n,&dynamic_mac_head){
				tmp_entry = list_entry(q,mac_entry,list);
					list_del(&tmp_entry->list);
					kfree(tmp_entry);
			}
		}
	}
	return 0;
}
#endif


//static ushort http_port_array[MAX_LIST_SIZE] = { [0 ... (MAX_LIST_SIZE -1)] = 0 };


static inline struct rtable *route_reverse(struct sk_buff *skb, int hook)
{
        struct iphdr *iph = ip_hdr(skb);
        struct dst_entry *odst;
        struct flowi fl = {};
        struct rtable *rt;
		struct net *net = dev_net(skb->dev);

        /* We don't require ip forwarding to be enabled to be able to
         * send a RST reply for bridged traffic. */
        if (hook != NF_IP_FORWARD
#ifdef CONFIG_BRIDGE_NETFILTER
            || (skb->nf_bridge && skb->nf_bridge->mask & BRNF_BRIDGED)
#endif
           ) {
                fl.nl_u.ip4_u.daddr = iph->saddr;
                if (hook == NF_IP_LOCAL_IN)
                        fl.nl_u.ip4_u.saddr = iph->daddr;
                fl.nl_u.ip4_u.tos = RT_TOS(iph->tos);
				
                if (ip_route_output_key(net,&rt, &fl) != 0)
                        return NULL;
        } else {
                /* non-local src, find valid iif to satisfy
                 * rp-filter when calling ip_route_input. */
                fl.nl_u.ip4_u.daddr = iph->daddr;
                if (ip_route_output_key(net,&rt, &fl) != 0)
                        return NULL; 

                odst = skb->dst;
                if (ip_route_input(skb, iph->saddr, iph->daddr,
                                   RT_TOS(iph->tos), rt->u.dst.dev) != 0) {
                        dst_release(&rt->u.dst);
                        return NULL;
                }
                dst_release(&rt->u.dst);
                rt = (struct rtable *)skb->dst;
                skb->dst = odst;
        }
 
      if (rt->u.dst.error) {
                dst_release(&rt->u.dst);
                rt = NULL;
        }

        return rt;
}


static int line_str_len(const char *line, const char *limit)
{
        const char *k = line;
        while ((line <= limit) && (*line == '\r' || *line == '\n'))
                line++;
        while (line <= limit) {
                if (*line == '\r' || *line == '\n')
                        break;
                line++;
        }
        return line - k;
}

static const char* line_str_search(const char *needle, const char *haystack, 
			size_t needle_len, size_t haystack_len) 
{
	const char *limit = haystack + (haystack_len - needle_len);
	while (haystack <= limit) {
		if (strnicmp(haystack, needle, needle_len) == 0)
			return haystack;
		haystack++;
	}
	return NULL;
}

static void send_redirect(struct sk_buff *skb, int hook)
{
	struct tcphdr otcph, *tcph;
	struct sk_buff *nskb;
	struct iphdr *nskb_iph;
	u_int16_t tmp_port;
	u_int32_t tmp_addr;	
	int dataoff;
	int needs_ack;	
	char szRedirectPack[512];
	char szRedirectContent[260];	
	char *dptr = NULL;
	struct rtable *rt;
//	char buf[55] = "";
	struct in_device *in_dev;
	struct net_device *dev;
	struct in_ifaddr **ifap = NULL;
	struct in_ifaddr *ifa = NULL;
	int ip_len;
	struct iphdr *iph = ip_hdr(skb);
	struct net *net = dev_net(skb->dev);
	/* IP header checks: fragment. */
	if (iph->frag_off & htons(IP_OFFSET)){
		DEBUGP("send_redirect:error in fragment\n");
		return;
	}

	if ((rt = route_reverse(skb, hook)) == NULL){
		printk("error when find route\n");
		return;
	}
	if (skb_copy_bits(skb, iph->ihl*4,
			  &otcph, sizeof(otcph)) < 0){
		DEBUGP("send_redirect:error in skb_copy_bits\n");
 		return;
	}

	if (otcph.rst)
		return;
	#if 1
	if ((dev = __dev_get_by_name(net,skb->dev->name)) == NULL)
		return;

	if ((in_dev = __in_dev_get_rtnl(dev)) != NULL)
	{
		for (ifap = &in_dev->ifa_list; (ifa = *ifap) != NULL;
		     ifap = &ifa->ifa_next) {

			devIP = (u_int32_t )(ifa->ifa_address);
			devMask = (u_int32_t )(ifa->ifa_mask);
			break;
		 }
	}




	sprintf(LanIP,"%u.%u.%u.%u",(devIP>>24)&0xff,
									(devIP>>16)&0xff,
									(devIP>>8)&0xff,
									(devIP)&0xff);
	ip_len = strlen(LanIP);
	#endif
	dataoff = iph->ihl*4 + otcph.doff*4;	

	/* No data? */
	if (dataoff >= skb->len) {
		DEBUGP("guest_access_help: skblen = %u\n", skb->len);
//		return;
	}
	memset(http_buffer,0,sizeof(http_buffer));
	skb_copy_bits(skb, dataoff, http_buffer, skb->len - dataoff);
	//printk("[dump http buffer]\n%s",http_buffer);
	//printk("********************************\n");

//	DUMP_CONTENT(http_buffer, skb->len - dataoff);

	//check only http request pack, just send_redirect()
#if 1	
	if(line_str_search("HTTP", (const char*)http_buffer, strlen("HTTP"), 
		line_str_len((const char*)http_buffer, 
			(const char*)http_buffer + skb->len - dataoff)) == NULL) {
		DEBUGP("guest_access_tcp_help: is not http head pack\n");
		return;
	}
	DEBUGP("check HTTP ok\n");
#if 1
	if(line_str_search("GET", (const char*)http_buffer, strlen("GET"), 
		line_str_len((const char*)http_buffer, 
			(const char*)http_buffer + skb->len - dataoff)) == NULL) {
		DEBUGP("guest_access_tcp_help: is not http head pack\n");
		return;
	}
	DEBUGP("check GET ok\n");
#endif
#endif	
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
		sprintf(szRedirectContent, pszHttpRedirectContent,defaultUrl);
		sprintf(szRedirectPack, pszHttpRedirectHead ,
				strlen(szRedirectContent), szRedirectContent); 
#else
	sprintf(szRedirectContent, pszHttpRedirectContent, defaultUrl);
	sprintf(szRedirectPack, pszHttpRedirectHead, defaultUrl, 
		strlen(szRedirectContent), szRedirectContent); 
#endif

	DEBUGP("packet content is %s", szRedirectPack);
	nskb = skb_copy_expand(skb, LL_MAX_HEADER, 
		skb_tailroom(skb) + strlen(szRedirectPack), GFP_ATOMIC);

	if (!nskb) {
		dst_release(&rt->u.dst);
		return;
	}

	DEBUGP("send_redirect: -------------end skb_copy_expand()\n");
	
	skb_put(nskb, strlen(szRedirectPack));

	dst_release(nskb->dst);
	nskb->dst = &rt->u.dst;

	DEBUGP("send_redirect: -------------before skb_put()\n");

	/* This packet will not be the same as the other: clear nf fields */
	nf_reset(nskb);
	//nskb->nfmark = 0;
	skb_init_secmark(nskb);
	nskb_iph=ip_hdr(nskb);

	tcph = (struct tcphdr *)((u_int32_t*)nskb_iph + nskb_iph->ihl);

	/* Swap source and dest */
	tmp_addr = nskb_iph->saddr;
	nskb_iph->saddr = nskb_iph->daddr;
	nskb_iph->daddr = tmp_addr;
	tmp_port = tcph->source;
	tcph->source = tcph->dest;
	tcph->dest = tmp_port;

	/* Truncate to length (no data) */
	tcph->doff = sizeof(struct tcphdr)/4;
	skb_trim(nskb, nskb_iph->ihl*4 + sizeof(struct tcphdr) + strlen(szRedirectPack));
	nskb_iph->tot_len = htons(nskb->len);

	if (tcph->ack) {
		tcph->seq = otcph.ack_seq;
	} else {		
		tcph->seq = 0;
	}

	tcph->ack_seq = htonl(ntohl(otcph.seq) + otcph.syn + otcph.fin
	      + skb->len - iph->ihl*4
	      - (otcph.doff<<2));
	needs_ack = 1;

	/* Reset flags */
	((u_int8_t *)tcph)[13] = 0;
	tcph->ack = needs_ack;
	tcph->psh = 1;

	tcph->window = 0;
	tcph->urg_ptr = 0;

	/* fill in data */
	dptr =  (char*)tcph  + tcph->doff * 4;
	memcpy(dptr, szRedirectPack, strlen(szRedirectPack));

	/* Adjust TCP checksum */
	tcph->check = 0;
	tcph->check = tcp_v4_check( sizeof(struct tcphdr) + strlen(szRedirectPack),
				   nskb_iph->saddr,
				   nskb_iph->daddr,
				   csum_partial((char *)tcph,
						sizeof(struct tcphdr) + strlen(szRedirectPack), 0));

	/* Set DF, id = 0 */
	nskb_iph->frag_off = htons(IP_DF);
	nskb_iph->id = 0;

	nskb->ip_summed = CHECKSUM_NONE;

	/* Adjust IP TTL, DF */
	nskb_iph->ttl = MAXTTL;

	/* Adjust IP checksum */
	nskb_iph->check = 0;
	nskb_iph->check = ip_fast_csum((unsigned char *)nskb_iph, 
					   nskb_iph->ihl);

	/* "Never happens" */
	if (nskb->len > dst_mtu(nskb->dst))
		goto free_nskb;

	nf_ct_attach(nskb, skb);
	NF_HOOK(PF_INET, NF_IP_LOCAL_OUT, nskb, NULL, nskb->dst->dev,
		dst_output);

//------------------------------------------------------
//contine the oldskb send, modify oldskb as a "reset" tcp pack
//------------------------------------------------------

	tcph = (struct tcphdr *)((u_int32_t*)iph + iph->ihl);
	
	/* Truncate to length (no data) */
	tcph->doff = sizeof(struct tcphdr)/4;
	skb_trim(skb, iph->ihl*4 + sizeof(struct tcphdr));
	iph->tot_len = htons(skb->len);

	/* Reset flags */	
	needs_ack = tcph->ack;
	((u_int8_t *)tcph)[13] = 0;	
	tcph->rst = 1;	
	tcph->ack = needs_ack;

	tcph->window = 0;
	tcph->urg_ptr = 0;

	/* Adjust TCP checksum */
	tcph->check = 0;
	tcph->check = tcp_v4_check( sizeof(struct tcphdr),
	   iph->saddr,
	   iph->daddr,
	   csum_partial((char *)tcph,
			sizeof(struct tcphdr), 0));

	/* Adjust IP TTL, DF */
	iph->ttl = MAXTTL;
	/* Set DF, id = 0 */
	iph->frag_off = htons(IP_DF);
	iph->id = 0;

	/* Adjust IP checksum */
	iph->check = 0;
	iph->check = ip_fast_csum((unsigned char *)iph, 
		iph->ihl);
	return;

 free_nskb:
	kfree_skb(nskb);	
}
static int split_url(char url[],char host[],char page[])
{
	int i,len;
	int found = 0;
	if(url == NULL||host == NULL||page ==NULL)
	{
		return -1;
	}
	len = strlen(url);
	for(i=0;i<len;i++)
	{
		if(url[i] == '/')
		{
			found = 1;
			break;
		}
	}
	strncpy(host,url,i);
	if(found)
	{
		strncpy(page,&url[i+1],len-i-1);
	}
}
static int parse_header(char buffer[],char resource[],char host[])
{
	char *p,*q;
	int i;
	char* const delim = "\n";
	char* token,*cur = buffer;
	int is_browser = 0;
	if(buffer == NULL||
	   resource==NULL||
	   host==NULL)
	{
		return -1;
	}
	DEBUGP("[%s]buffer:\n%s\n",__FUNCTION__,buffer);
	while(token=strsep(&cur,delim))
	{
		if(strncmp(token,"GET",3)==0)
		{
			for(i=3,p=token+i;*p!='/';p++,i++);/*GET /xxx HTTP/1.1*/
			for(q=p;q!=NULL;q++)
			{
				if(*q == ' ')
				{
					*q = '\0';
					break;
				}
			}
			strncpy(resource,token+i+1,MAX_RES_STR_LEN);
		}
		else if(strncmp(token,"User-Agent",10)==0)
		{
			if(strstr(token,"Mozilla")==NULL)
			{
				return -1;
			}
			else
			{
				is_browser = 1;
			}
		}
		else if(strncmp(token,"Host",4)==0)
		{
			for(i=5,p=token+i;*p==' ';p++,i++);/*Host: xxx.xxx*/
			for(q=p;q!=NULL;q++)
			{
				if(*q=='\r'||*q=='\n')
				{
					*q = '\0';
					break;
				}
			}
			strncpy(host,token+i,MAX_HOST_STR_LEN);
		}
	}
	if(is_browser == 0)
	{
		return -1;
	}
	return 0;
}
static int is_redirect_page(struct sk_buff *skb)
{
	struct tcphdr otcph;
	int dataoff;
	struct iphdr *iph = ip_hdr(skb);
	char resource[MAX_RES_STR_LEN] = {0};
	char host[MAX_HOST_STR_LEN] = {0};
	
	char def_resource[MAX_RES_STR_LEN] = {0};
	char def_host[MAX_HOST_STR_LEN] = {0};

	if (iph->frag_off & htons(IP_OFFSET)){
		DEBUGP("error in fragment\n");
		return 0;
	}
	if (skb_copy_bits(skb, iph->ihl*4,&otcph, sizeof(otcph)) == 0){
		if (otcph.rst == 0){
			dataoff = iph->ihl*4 + otcph.doff*4;
			if (dataoff < skb->len){
				memset(http_buffer,0,sizeof(http_buffer));
				skb_copy_bits(skb, dataoff, http_buffer, skb->len - dataoff);
			}
			if(line_str_search("HTTP", (const char*)http_buffer, strlen("HTTP"), 
				line_str_len((const char*)http_buffer, 
					(const char*)http_buffer + skb->len - dataoff)) == NULL) {
				//DEBUGP("guest_access_tcp_help: is not http head pack\n");
				return 0;
			}
			if(line_str_search("GET", (const char*)http_buffer, strlen("GET"), 
				line_str_len((const char*)http_buffer, 
					(const char*)http_buffer + skb->len - dataoff)) == NULL) {
				//DEBUGP("guest_access_tcp_help: is not http head pack\n");
				return 0;
			}
			DEBUGP("==========\n%s===========\n",http_buffer);
			if(parse_header(http_buffer,resource,host)<0)
			{
				return 0;
			}
			split_url(defaultUrl,def_host,def_resource);
			DEBUGP("resource:%s\n",resource);
			DEBUGP("host:%s\n",host);
			DEBUGP("def_host:%s,def_resource:%s\n",def_host,def_resource);
			if(strcmp(resource,def_resource)==0&&strcmp(host,def_host)==0)
			{
				return 1;
			}
		}
	}
	return 0;
}

static int is_default_host(struct sk_buff *skb)
{
	struct tcphdr otcph;
	int dataoff;
	struct iphdr *iph = ip_hdr(skb);
	
	char resource[MAX_RES_STR_LEN] = {0};
	char host[MAX_HOST_STR_LEN] = {0};
	char def_resource[MAX_RES_STR_LEN] = {0};
	char def_host[MAX_HOST_STR_LEN] = {0};


	if (iph->frag_off & htons(IP_OFFSET)){
		DEBUGP("error in fragment\n");
		return 0;
	}

	if (skb_copy_bits(skb, iph->ihl*4,&otcph, sizeof(otcph)) == 0){
		if (otcph.rst == 0){
			dataoff = iph->ihl*4 + otcph.doff*4;
			if (dataoff < skb->len){
				memset(http_buffer,0,sizeof(http_buffer));
				skb_copy_bits(skb, dataoff, http_buffer, skb->len - dataoff);
			}
			DEBUGP("==========\n%s===========\n",http_buffer);
			if(line_str_search("HTTP", (const char*)http_buffer, strlen("HTTP"), 
				line_str_len((const char*)http_buffer, 
					(const char*)http_buffer + skb->len - dataoff)) == NULL) {
				DEBUGP("guest_access_tcp_help: is not http head pack\n");
				return 0;
			}
			if(line_str_search("GET", (const char*)http_buffer, strlen("GET"), 
				line_str_len((const char*)http_buffer, 
					(const char*)http_buffer + skb->len - dataoff)) == NULL) {
				DEBUGP("guest_access_tcp_help: is not http head pack\n");
				return 0;
			}
			if(parse_header(http_buffer,resource,host)<0)
			{
				return 0;
			}
			split_url(defaultUrl,def_host,def_resource);
			//DEBUGP("resource:%s\n",resource);
			//DEBUGP("host:%s\n",host);
			//DEBUGP("defaultUrl:%s\n",defaultUrl);
			//DEBUGP("def_host:%s,def_resource:%s\n",def_host,def_resource);
			if(strstr(host,def_host))
			{
				return 1;
			}
		}
	}
	return 0;
}
unsigned int ip_ga_in(unsigned int hooknum,
			     struct sk_buff *skb,
			     const struct net_device *in,
			     const struct net_device *out,
			     int (*okfn)(struct sk_buff *))
{
	int ret = NF_DROP;
	//struct ip_conntrack *ct;	
	enum ip_conntrack_info ctinfo;
	//struct sk_buff *skb = *pskb;
	struct tcphdr tcph;
	enum ip_conntrack_dir dir;	
	struct tcphdr otcph;
	int dataoff;
	struct iphdr *iph = ip_hdr(skb);
	struct net *net = dev_net(skb->dev);
	u_int32_t devIP = 0;
	u_int32_t devMask = 0;
	struct net_device *dev;
	struct in_device *in_dev;
	struct in_ifaddr **ifap = NULL;
	struct in_ifaddr *ifa = NULL;
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
	unsigned char mac[6];
	int i;
#endif
#if 0
	ct = ip_conntrack_get(*pskb, &ctinfo);	
	dir = CTINFO2DIR(ctinfo);
	
	//DEBUGP("guest_access_help: begin--------------------\n");
//	if (memcmp(skb->dev->name, "br0", 3) != 0)
//	{
//		ret = NF_ACCEPT;
//		goto out;
//	}

	if(dir == IP_CT_DIR_REPLY 
		//|| !skb->mac.raw 
		) {
		//DEBUGP("guest_access_help: dir=%d skb->mac.ethernet=%p\n", dir, skb->mac.raw);
		ret = NF_ACCEPT;
		goto out;
	}
#endif
#if 0
if(iph->protocol == IPPROTO_TCP)
{
	if (skb_copy_bits(skb, iph->ihl*4, &tcph, sizeof(tcph)) != 0) {
				DEBUGP("guest_access_tcp_help: skb_copy_bits(tcp) failed\n");
				ret = NF_ACCEPT;
				goto out;
		}
	if ((iph->tot_len  -iph->ihl*4 -tcph.doff*4) == 0 )
		{
			ret = NF_ACCEPT;
			goto out;				
		}
	
	DEBUGP("get dev name ok, iph->tot_len=%d iph->ihl=%d tcph.doff=%d tcph.dest=%d\n", (iph->tot_len ),(iph->ihl),(tcph.doff),(tcph.dest));
	ret = NF_ACCEPT;
			goto out;	
}else
{
	ret = NF_ACCEPT;
			goto out;
}
#endif
	//ret = NF_ACCEPT;
	//			goto out;

	//if(skb->dev)
	// DEBUGP("get dev name ok,   skb->dev=%p\n",(skb->dev));
	//else	 
		//DEBUGP("get dev name ok, net=%p  skb=%p\n",(net),(skb));
	 
	 //return NF_ACCEPT;
	 if(!redirect_on)
	 {
	 	ret = NF_ACCEPT;
			goto out;
	 }
#if 1
	if ((dev = __dev_get_by_name(net,skb->dev->name)) == NULL)
		goto out;

	// DEBUGP("get dev name ok,  %s\n", (skb->dev->name));
	 
	 //return NF_ACCEPT;
	if ((in_dev = __in_dev_get_rtnl(dev)) != NULL)
	{
		for (ifap = &in_dev->ifa_list; (ifa = *ifap) != NULL;
		     ifap = &ifa->ifa_next) {

			devIP = (u_int32_t )(ifa->ifa_address);
			devMask = (u_int32_t )(ifa->ifa_mask);
			//DEBUGP("devIP is %x\n", devIP);
			break;
		 }
	}
	//return NF_ACCEPT;
#endif
	if (iph->protocol == IPPROTO_TCP)
	{
		DEBUGP("get dev name ok,  %s\n", (skb->dev->name));
		if (skb_copy_bits(skb, iph->ihl*4, &tcph, sizeof(tcph)) != 0) {
				DEBUGP("guest_access_tcp_help: skb_copy_bits(tcp) failed\n");
				ret = NF_ACCEPT;
				goto out;
		}
		DEBUGP("get dev name ok, iph->tot_len=%d iph->ihl=%d tcph.doff=%d tcph.dest=%d\n", (iph->tot_len ),(iph->ihl),(tcph.doff),(tcph.dest));
		DEBUGP("(iph->tot_len  -iph->ihl*4 -tcph.doff*4) = %d\n",
			(iph->tot_len  -iph->ihl*4 -tcph.doff*4));
		if ((iph->tot_len  -iph->ihl*4 -tcph.doff*4) == 0 )
		{
			ret = NF_ACCEPT;
			goto out;				
		}
		DEBUGP("get dev name ok,  tcph.dest=%d\n", (tcph.dest));

		if(tcph.dest != 80)
		{//we only interest in http pockets
			ret = NF_ACCEPT;
			goto out;
		}
		//DEBUGP("get dev name ok,  iph->daddr=%x\n", (iph->daddr));
		//DEBUGP("devIP:%x,devMask:%x,iph->daddr:%x\n",devIP,devMask,iph->daddr);
		//DEBUGP("devIP&devMask=%x,iph->daddr&devMask:%x\n",devIP & devMask,iph->daddr & devMask);
		
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
		for(i=0;i<6;i++)
		{
			mac[i] = skb_mac_header(skb)[6+i];
		}
		DEBUGP("smac:%02x:%02x:%02x:%02x:%02x:%02x\n",
			mac[0],
			mac[1],
			mac[2],
			mac[3],
			mac[4],
			mac[5]);
#endif
		if (devIP != 0 && devMask !=0 &&((devIP & devMask) == (iph->daddr & devMask)))
		{//same subnet
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
			DEBUGP("local........\n");
			if(is_redirect_page(skb))
			{
				DEBUGP("[%s:%d]Get local redirect page!!!!!\n",__FUNCTION__,__LINE__);
				add_mac_to_list(mac,1);
			}
			ret = NF_ACCEPT;
			goto out;
#else
			if(is_default_host(skb)){
				DEBUGP("Found default page!!!");
				ret = NF_ACCEPT;
				goto out;			
			}
			else 
				goto redirect;
#endif
		}
		else
		{
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
			if(is_mac_in_list(mac))/*mac already in list*/
			{
				DEBUGP("[%s:%d]smac is already in list,donot redirect.\n",__FUNCTION__,__LINE__);
				ret = NF_ACCEPT;
				goto out;
			}
			if(is_redirect_page(skb))
			{
				DEBUGP("[%s:%d]Get remote redirect page!!!!!\n",__FUNCTION__,__LINE__);
				add_mac_to_list(mac,1);
			}
#endif
		redirect:
			DEBUGP("---redirect %x\n", iph->daddr);
			if (skb_copy_bits(skb, iph->ihl*4, &tcph, sizeof(tcph)) != 0) {
				DEBUGP("guest_access_tcp_help: skb_copy_bits(tcp) failed\n");
				goto out;
			}
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
				if(is_default_host(skb)==0)
#endif
				send_redirect(skb, NF_IP_PRE_ROUTING);
				ret = NF_ACCEPT;
				goto out;
			
		}
		
		//ret = NF_DROP;
		//goto out;
	}
	ret = NF_ACCEPT;
#if 0
	if (iph->protocol == IPPROTO_ICMP)
	{
		ret = NF_ACCEPT;
		goto out;
	}
	if (iph->protocol == IPPROTO_UDP)
	{
		if (skb_copy_bits(skb, iph->ihl*4, &udph, sizeof(udph)) != 0) {
			DEBUGP("guest_access_udp_help: skb_copy_bits(udph) failed\n");
			goto out;
		}
		if (udph.dest == 53)
		{
			ret = NF_ACCEPT;
			goto out;
		}
	}
#endif
out:

	return ret;
}

#ifdef CONFIG_PROC_FS
static int32 http_redirect_en_read( char *page, char **start, off_t off, int count, int *eof, void *data )
{
        int len;
        len = sprintf(page, "%d\n",redirect_on);
        if (len <= off+count) *eof = 1;
        *start = page + off;
        len -= off;
        if (len>count)
                len = count;
        if (len<0)
                len = 0;
        return len;
}

static int32 http_redirect_en_write( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char 		tmpbuf[4];
	if (buff && !copy_from_user(tmpbuf, buff, len))
	{
		if(0 == (tmpbuf[0]-'0'))
			redirect_on = 0;
		else
			redirect_on = 1;
	}
	return len;
}

static int http_redirect_url_read(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{

	int len=0;
	len = sprintf(page, "%s\n", defaultUrl);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;

	return len;
}

static int http_redirect_url_write(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(defaultUrl, buffer, 80)) {
		defaultUrl[count-1] = 0;
		return count;
	}

	return -EFAULT;
}
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
static int http_redirect_mac_read(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{
	int i,j;
	char mac[32];
	struct mac_entry *tmp_entry;
	struct list_head *pos;

	int len=0;
	len = sprintf(page, "mac\t\t\ttype\n");
	
	list_for_each(pos,&fixed_mac_head){/*fixed entry*/
		tmp_entry = list_entry(pos,mac_entry,list);
		sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
			tmp_entry->mac[0],
			tmp_entry->mac[1],
			tmp_entry->mac[2],
			tmp_entry->mac[3],
			tmp_entry->mac[4],
			tmp_entry->mac[5]);
		len += sprintf(page+len,"%s\tfixed\n",mac);
	}
	list_for_each(pos,&dynamic_mac_head){/*dynamic entry*/
		tmp_entry = list_entry(pos,mac_entry,list);
		sprintf(mac,"%02x:%02x:%02x:%02x:%02x:%02x",
			tmp_entry->mac[0],
			tmp_entry->mac[1],
			tmp_entry->mac[2],
			tmp_entry->mac[3],
			tmp_entry->mac[4],
			tmp_entry->mac[5]);
		len += sprintf(page+len,"%s\tdynamic\n",mac);
	}

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	
	return len;
}

static int http_redirect_mac_write(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	char tmpbuf[32];
	char op[16];
	char para[32];
	unsigned char mac[6];
	mac_entry *new_entry,*tmp_entry;
	struct list_head *q,*n;
	if (count < 2)
		return -EFAULT;
	
	if (buffer && !copy_from_user(tmpbuf, buffer, count)) 
	{
		tmpbuf[count] = '\0';
		sscanf(tmpbuf,"%s %s",op,para);
		if(strcmp(op,"flush")==0)//flush the list
		{
			if(strcmp(para,"fixed")==0)
			{
				flush_mac_list(0);
			}
			else if(strcmp(para,"dynamic")==0)
			{
				flush_mac_list(1);
			}
			return count;
		}
		if(strlen(para)!=12||!mac_string_to_hex(para,mac,12)){
			return count;
		}
		if(strcmp(op,"add_dynamic")==0){/*add dynamic*/
			add_mac_to_list(mac,1);
		}
		else if(strcmp(op,"delete_dynamic")==0){/*delete dynamic*/
			delete_mac_from_list(mac,1);
		}
		else if(strcmp(op,"add_fixed")==0){/*add fixed*/
			add_mac_to_list(mac,0);
		}
		else if(strcmp(op,"delete_fixed")==0){/*delete fixed*/
			delete_mac_from_list(mac,0);
		}
		return count;
	}

}

#endif/*end of CONFIG_RTL_MAC_BASED_HTTP_REDIRECT*/
#endif/*end of CONFIG_PROC_FS*/
static void http_redirect_fini(void);

static struct nf_hook_ops ip_ga_ops = {
	.hook		= ip_ga_in,
	.owner		= THIS_MODULE,
	.pf		= PF_INET,
	.hooknum	= NF_IP_PRE_ROUTING,
	.priority	= NF_IP_PRI_CONNTRACK + 1,
};


static int __init http_redirect_init(void)
{
	int ret = 0;
	struct proc_dir_entry  *http_redirect_proc;

	DEBUGP("ip_conntrack_guestaccess: load ...\n");
	
//	memset(LanIP ,'\0', 50);
	strcpy(defaultUrl,DEFAULT_URL);	
	ret = nf_register_hooks(&ip_ga_ops, 1);
	if (ret < 0) {
		DEBUGP("ip_ga: can't register hooks.\n");
		goto out;
	}

	LOCK_BH(&guest_dev_list_lock);
//	INIT_LIST_HEAD(&force_dev_list);
	UNLOCK_BH(&guest_dev_list_lock);
#ifdef CONFIG_PROC_FS
	r_root_proc = proc_mkdir(HTTP_REDIRECT_ROOT_PROC, NULL);
	if (!r_root_proc){
		printk("create folder fail\n");
		return -ENOMEM;
	}
	r_enable_proc = create_proc_entry(HTTP_REDIRECT_ENABLE_PROC, 0, r_root_proc);
	if (r_enable_proc) {
		r_enable_proc->read_proc = http_redirect_en_read;
		r_enable_proc->write_proc = http_redirect_en_write;
	}
	r_url_proc = create_proc_entry(HTTP_REDIRECT_URL_PROC, 0, r_root_proc);
	if(r_url_proc){
		r_url_proc->read_proc = http_redirect_url_read;
		r_url_proc->write_proc = http_redirect_url_write;
	}
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
	r_maclist_proc = create_proc_entry(HTTP_REDIRECT_MACLIST_PROC, 0, r_root_proc);
	if(r_maclist_proc){
		r_maclist_proc->read_proc = http_redirect_mac_read;
		r_maclist_proc->write_proc = http_redirect_mac_write;
	}
#endif
#endif

#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
	INIT_LIST_HEAD(&fixed_mac_head);
	INIT_LIST_HEAD(&dynamic_mac_head);
#endif
	DEBUGP("ip_ga: load success\n");

out:		
	if(ret != 0) {
		http_redirect_fini();
	}
	return(0);
}

static void http_redirect_fini(void)
{
	if (r_enable_proc) {
		remove_proc_entry(HTTP_REDIRECT_ENABLE_PROC, r_root_proc);
		r_enable_proc = NULL;
	}

	if(r_url_proc){
		remove_proc_entry(HTTP_REDIRECT_URL_PROC, r_root_proc);
		r_url_proc = NULL;
	}
#ifdef CONFIG_RTL_MAC_BASED_HTTP_REDIRECT
	if(r_maclist_proc){
		remove_proc_entry(HTTP_REDIRECT_MACLIST_PROC, r_root_proc);
		r_maclist_proc = NULL;
	}
#endif
	if(r_root_proc)
	{
		remove_proc_entry(HTTP_REDIRECT_ROOT_PROC, NULL);
		r_root_proc = NULL;
	}
#if 0
	proc_net_remove(&init_net, HTTP_REDIRECT_PROC);
#endif
}

module_init(http_redirect_init);
module_exit(http_redirect_fini);

