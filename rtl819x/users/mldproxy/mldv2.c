#include "mclab.h"
#include "timeout.h"
#include "mldproxy.h"
#include <fcntl.h>
#include <signal.h>
#include "mld.h"

#ifdef CONFIG_MLDV2_SUPPORT
#define IPV6_READY_LOGOV2 1
#define IN6_IS_ADDR_MULTICAST(a) (((__u8 *) (a))[0] == 0xff)

#define	IN6_IS_ADDR_RESVER_0(a) \
	(IN6_IS_ADDR_MULTICAST(a)					      \
	 && (((( __u8 *) (a))[1] & 0xf) == 0x0))
#define IS_IPV6_RESVER_0_ADDRESS(ipv6addr)	((ipv6addr[1] & 0xf0)==0x00)




#if 0
#define MLDV2LOG	printf
#else
#define MLDV2LOG(...)	while(0){}
#endif

#define MLDV2_MAX_SRCNUM	64
extern int ctlbuflen ;
extern struct in6_addr IS_MLD_ALL_HOSTS_ADDRESS;
extern struct in6_addr IS_MLD_ALL_ROUTER_ADDRESS;

//__u32 gsrctmp[MLDV2_MAX_SRCNUM];
struct in6_addr gsrctmp[MLDV2_MAX_SRCNUM];

//int mldv2_query( struct mcft_entry *entry, int srcnum, __u32 *srcfilter );
int mldv2_query( struct mcft_entry *entry, int srcnum, struct in6_addr *srcfilter );

#if defined IPV6_READY_LOGOV2
//#error1
struct src_entry *add_to_if_srclist(struct ifrec_entry *iflist, struct in6_addr src)
{
	struct src_entry *p;
	
	if(iflist==NULL) 
		return NULL;

	MLDV2LOG( "%s> group=%s", __FUNCTION__, inet_ntoa( mcp->grp_addr ) );
	MLDV2LOG( ", src=%s\n", inet_ntoa( src ) );
	
	p = iflist->srclist;
	while (p) {
		if (!memcmp(&p->srcaddr, &src,sizeof(struct in6_addr))){
			printf("src exited already![%s]:[%d].\n",__FUNCTION__,__LINE__);
			return p;
		}
		p = p->next;
	}
	//new src
	p = malloc(sizeof(struct src_entry));
	if (!p) {
		return NULL;
	}
	memset( p, 0, sizeof(struct src_entry) );
	memcpy(&p->srcaddr,&src,sizeof(struct in6_addr));
	p->timer.lefttime = 0;
	p->timer.retry_left = 0;
	p->next = iflist->srclist;
	iflist->srclist = p;
	return p;
}

int del_from_if_srclist(struct ifrec_entry *iflist, struct in6_addr src)
{
	struct src_entry **q, *p;

	if(iflist==NULL) return  -1;
	
	MLDV2LOG( "%s> group=%s", __FUNCTION__, inet_ntoa( mcp->grp_addr ) );
	MLDV2LOG( ", src=%s\n", inet_ntoa( src ) );
	
	q = &iflist->srclist;
	p = *q;
	while (p) {
		//if(p->srcaddr == src) {
		if(!memcmp(&p->srcaddr,&src,sizeof(struct in6_addr))){
			*q = p->next;
			free(p);
			return 0;
		}
		q = &p->next;
		p = p->next;
	}
	
	return 0;
}

//#else
struct src_entry *add_to_grp_srclist(struct mcft_entry *mcp, struct in6_addr src)
{
	struct src_entry *p;
	
	if(mcp==NULL) 
		return NULL;

	MLDV2LOG( "%s> group=%s", __FUNCTION__, inet_ntoa( mcp->grp_addr ) );
	MLDV2LOG( ", src=%s\n", inet_ntoa( src ) );
	
	p = mcp->srclist;
	while (p) {
		if (!memcmp(&p->srcaddr, &src,sizeof(struct in6_addr))){
			printf("src exited already![%s]:[%d].\n",__FUNCTION__,__LINE__);
			return p;
		}
		p = p->next;
	}
	
	p = malloc(sizeof(struct src_entry));
	if (!p) {
		return NULL;
	}
	memset( p, 0, sizeof(struct src_entry) );
	//p->srcaddr = src;
	memcpy(&p->srcaddr,&src,sizeof(struct in6_addr));
	p->timer.lefttime = 0;
	p->timer.retry_left = 0;
	//p->mrt_state = 0;
	p->next = mcp->srclist;
	mcp->srclist = p;
	return p;
}

int del_from_grp_srclist(struct mcft_entry *mcp, struct in6_addr src)
{
	struct src_entry **q, *p;

	if(mcp==NULL) return  -1;
	
	MLDV2LOG( "%s> group=%s", __FUNCTION__, inet_ntoa( mcp->grp_addr ) );
	MLDV2LOG( ", src=%s\n", inet_ntoa( src ) );
	
	q = &mcp->srclist;
	p = *q;
	while (p) {
		//if(p->srcaddr == src) {
		if(!memcmp(&p->srcaddr,&src,sizeof(struct in6_addr))){
			*q = p->next;
			free(p);
			return 0;
		}
		q = &p->next;
		p = p->next;
	}
	
	return 0;
}
#endif
struct src_entry * get_specific_src(struct mcft_entry *mcp, struct in6_addr src)
{
	struct src_entry **q, *p;
	
	if(mcp==NULL) return NULL;
	
	MLDV2LOG( "%s> group=%s", __FUNCTION__, inet_ntoa( mcp->grp_addr ) );
	MLDV2LOG( ", src=%s\n", inet_ntoa( src ) );
	
	q = &mcp->srclist;
	p = *q;
	while (p) {
		//if(p->srcaddr == src)
		if(!memcmp(&p->srcaddr,&src,sizeof(struct in6_addr)))
		{
			return p;
		}
		q = &p->next;
		p = p->next;
	}
	
	return NULL;
}

int get_srclist_num( struct mcft_entry *mcp )
{
	int ret=0;
	struct src_entry *p;

	if(mcp==NULL) return ret;
	
	p = mcp->srclist;
	while(p)
	{
		ret++;
		p=p->next;
	}

	MLDV2LOG( "%s> group:%s has %d source(s)\n", __FUNCTION__, inet_ntoa( mcp->grp_addr ), ret );

	return ret;
}

#ifdef CONFIG_MLDPROXY_MULTIWAN
int mld_add_group( struct in6_addr group ,int ifindex)
{
	struct ipv6_mreq mreq;
	struct IfDesc *up_dp ;
	int ret;
    int idx ;
		
	MLDV2LOG( "%s> join the group=%s\n", __FUNCTION__, inet_ntoa( group ) );

	/* join multicast group */
	//mreq.imr_multiaddr.s_addr = group;
	memcpy(&(mreq.ipv6mr_multiaddr),&(group),sizeof(struct in6_addr));
#if defined IPV6_READY_LOGOV2
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	for(idx=0;idx<mld_up_if_num;idx++){
		up_dp = getIfByName(mld_up_if_name[idx]);
		if ((int)up_dp->pif_idx!=ifindex){
			mreq.ipv6mr_ifindex =(int)(up_dp->pif_idx);
			ret = setsockopt(up_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
			if(ret) {
				printf("setsockopt IPV6_ADD_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
				return ret;
			}
		}
	}
	if ((int)down_dp->pif_idx!=ifindex){
		mreq.ipv6mr_ifindex =(int)(down_dp->pif_idx);
		ret = setsockopt(down_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt IPV6_ADD_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			return ret;
		}
	}
	return 1;	
#else	
	for(idx=0;idx<mld_up_if_num;idx++){
	   up_dp = getIfByName(mld_up_if_name[idx]);	
	   //mreq.imr_interface.s_addr = up_dp->InAdr.s_addr;
	   mreq.ipv6mr_ifindex =(int)(up_dp->pif_idx);
	   ret = setsockopt(up_dp->sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
	   if(ret)
		printf("setsockopt IP_ADD_MEMBERSHIP %s error!\n", inet_ntoa(mreq.ipv6mr_multiaddr));
	}	
	return ret;	
#endif
}

int mld_del_group( struct in6_addr group )
{
	struct ipv6_mreq mreq;
	struct IfDesc *up_dp ;
	int idx ;
        int ret ;
	MLDV2LOG( "%s> leave the group=%s\n", __FUNCTION__, inet_ntoa( group ) );

	/* drop multicast group */
	//mreq.imr_multiaddr.s_addr = group;
	memcpy(&(mreq.ipv6mr_multiaddr),&(group),sizeof(struct in6_addr));
	for(idx=0;idx<mld_up_if_num;idx++){
		
		up_dp = getIfByName(mld_up_if_name[idx]);	
		//mreq.imr_interface.s_addr = up_dp->InAdr.s_addr;
		mreq.ipv6mr_ifindex =up_dp->pif_idx;
		ret = setsockopt(up_dp->sock, IPPROTO_IPV6, IPV6_DROP_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret)
			printf("setsockopt IP_DROP_MEMBERSHIP %s error!\n", inet_ntoa(mreq.ipv6mr_multiaddr));
	}	

	return ret;
}
#else
//add membership
int mld_add_group( struct in6_addr group, int ifindex )
{
	struct ipv6_mreq mreq;
	
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	//struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	int ret;
	if (up_dp==NULL)
		printf("get if failed![%s][%d].\n",__FUNCTION__,__LINE__);
	MLDV2LOG( "%s> join the group=%s\n", __FUNCTION__, inet_ntoa( group ) );

	/* join multicast group */
	/*mreq.imr_multiaddr.s_addr = group;
	//mreq.imr_interface.s_addr = up_dp->InAdr.s_addr;
	ret = setsockopt(up_dp->sock, IPPROTO_IPV6, IP_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
	if(ret)
		printf("setsockopt IP_ADD_MEMBERSHIP %s error!\n", inet_ntoa(mreq.imr_multiaddr));
	*/
	memcpy(&(mreq.ipv6mr_multiaddr),&(group),sizeof(struct in6_addr));
#if defined IPV6_READY_LOGOV2
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	if ((int)up_dp->pif_idx!=ifindex){
		printf("ifindex:%d,up_dp->pif_idx:%d,[%s]:[%d].\n",ifindex,up_dp->pif_idx,__FUNCTION__,__LINE__);
		mreq.ipv6mr_ifindex =(int)(up_dp->pif_idx);
		ret = setsockopt(up_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt IPV6_ADD_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			return ret;
		}
	}
	if ((int)down_dp->pif_idx!=ifindex){
		printf("ifindex:%d,down_dp->pif_idx:%d,[%s]:[%d].\n",ifindex,down_dp->pif_idx,__FUNCTION__,__LINE__);
		mreq.ipv6mr_ifindex =(int)(down_dp->pif_idx);
		ret = setsockopt(down_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt IPV6_ADD_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			return ret;
		}
	}
	return 1;
#else
	mreq.ipv6mr_ifindex =(int)(up_dp->pif_idx);
	ret = setsockopt(up_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
	if(ret) {
		printf("setsockopt IPV6_ADD_MEMBERSHIP error%s !\n", inet_ntoa(mreq.ipv6mr_multiaddr.s6_addr));
		return ret;
	}

	syslog(LOG_INFO, "mldproxy: Add membership %s", inet_ntoa(mreq.ipv6mr_multiaddr.s6_addr));
	return 1;	
#endif
}

int mld_del_group( struct in6_addr group,int ifindex )
{
	struct ipv6_mreq mreq;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	int ret;
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	if (up_dp==NULL)
		printf("get if(%s)failed![%s][%d].\n",mld_up_if_name,__FUNCTION__,__LINE__);
	MLDV2LOG( "%s> leave the group=%s\n", __FUNCTION__, inet_ntoa( group ) );
	
	/* drop multicast group */
	/*
	mreq.imr_multiaddr.s_addr = group;
	mreq.imr_interface.s_addr = up_dp->InAdr.s_addr;
	ret = setsockopt(up_dp->sock, IPPROTO_IPV6, IP_DROP_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
	//if(ret)
	//	printf("setsockopt IP_DROP_MEMBERSHIP %s error!\n", inet_ntoa(mreq.imr_multiaddr));
	*/
	memcpy(&(mreq.ipv6mr_multiaddr),&(group),sizeof(struct in6_addr));
	//mreq.imr_interface.s_addr = up_dp->InAdr.s_addr;
	mreq.ipv6mr_ifindex =up_dp->pif_idx;
	ret = setsockopt(up_dp->sock, IPPROTO_IPV6 , IPV6_DROP_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
	if(ret)
		printf("setsockopt IPV6_DROP_MEMBERSHIP %s error!\n", inet_ntoa(mreq.ipv6mr_multiaddr));
	syslog(LOG_INFO, "mldproxy: Drop membership %s", inet_ntoa(mreq.ipv6mr_multiaddr));
	return ret;
}
#endif

#ifdef CONFIG_MLDPROXY_MULTIWAN
int mld_add_mr( struct in6_addr group, struct in6_addr src, int enable )
{
	struct MRouteDesc	mrd;
	int idx ;
	/* add multicast routing entry */
	/*
	mrd.OriginAdr.s_addr = src;
	mrd.SubsAdr.s_addr = 0;
	mrd.McAdr.s_addr = group;
	*/
	memcpy(&(mrd.OriginAdr),&(src),sizeof(struct in6_addr));
	memset(&(mrd.SubsAdr),0,sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	mrd.TtlVc[mld_down_if_idx] = enable;	

	MLDV2LOG( "%s> group:%s", __FUNCTION__, inet_ntoa(mrd.McAdr) );
	MLDV2LOG( ", src:%s, enable:%d\n", inet_ntoa(mrd.OriginAdr), enable );
	for(idx=0;idx<mld_up_if_num;idx++)
	{    
		mrd.InVif = mld_up_if_idx[idx];
		addMRoute(&mrd);
	}
	return (1);
}

int mld_del_mr( struct in6_addr group, struct in6_addr src )
{
	struct MRouteDesc	mrd;
	int ret=0;
    int idx;
		
	/* delete multicast routing entry */
	/*mrd.OriginAdr.s_addr = src;
	mrd.McAdr.s_addr = group;
	*/
	memcpy(&(mrd.OriginAdr),&src,sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&group,sizeof(struct in6_addr));

	MLDV2LOG( "%s> group:%s", __FUNCTION__, inet_ntoa(mrd.McAdr) );
	MLDV2LOG( ", src:%s\n", inet_ntoa(mrd.OriginAdr) );
	for(idx=0;idx<mld_up_if_num;idx++){
       mrd.InVif = mld_up_if_idx[idx];
	   delMRoute(&mrd);
	}
	return ret;
}
//update mfc
int mld_update_mr(struct in6_addr group, struct in6_addr src, int enable ,int ifindex)
{
	struct MRouteDesc	mrd;
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	/* add multicast routing entry */
	memcpy(&(mrd.OriginAdr),&(src),sizeof(struct in6_addr));
	memset(&(mrd.SubsAdr),0,sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	
#if defined IPV6_READY_LOGOV2
	//int pifindex;
	struct mcft_entry *mymcp=get_mcft(group);
	//struct mbr_entry	*gcp=mymcp->grp_mbr;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp;//= getIfByPix(infindex);
	uint8 cin_ix;//=(uint8)cin_dp->vif_idx;
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	printf( "srcaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	src.s6_addr16[ 0],src.s6_addr16[ 1],src.s6_addr16[ 2],src.s6_addr16[ 3],
	src.s6_addr16[ 4],src.s6_addr16[ 5],src.s6_addr16[ 6],src.s6_addr16[ 7]);
	
	printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	group.s6_addr16[ 0],group.s6_addr16[ 1],group.s6_addr16[ 2],group.s6_addr16[ 3],
	group.s6_addr16[ 4],group.s6_addr16[ 5],group.s6_addr16[ 6],group.s6_addr16[ 7]);
	
		
	{
		printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		int pifindex;
		//struct mcft_entry *mymcp=get_mcft(group);
		struct ifrec_entry *if_rec=mymcp->iflist;
		//not the first time receive report,and add all interface who received report togeter 
		while(if_rec){
			pifindex=if_rec->ifindex;
			cin_dp= getIfByPix(pifindex);
			cin_ix=(uint8)cin_dp->vif_idx;	
			if(if_rec->ifindex!=ifindex){
				mrd.TtlVc[cin_ix] = 1; //data pkt out interface==interface receive report
			}
			else{
				printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				mrd.TtlVc[cin_ix] = enable; //update mfc
			}
			if_rec=if_rec->next;
			
		}
	}
	if (enable==0)//means the interface leave
	{
		//up leave group ,then up in
		if((int)(up_dp->pif_idx)==ifindex){
			mrd.InVif = mld_up_if_idx;
			printf("up_dp->pif_idx:%d,ifindex:%d,[%s]:[%d].\n",up_dp->pif_idx,ifindex,__FUNCTION__,__LINE__);
			addMRoute(&mrd);
			
		}
		//down leave group and  down in
		if((int)(down_dp->pif_idx)==ifindex){
			mrd.InVif = mld_down_if_idx;
			printf("down_dp->pif_idx:%d,ifindex:%d,[%s]:[%d].\n",down_dp->pif_idx,ifindex,__FUNCTION__,__LINE__);
			addMRoute(&mrd);
		}
	}
	else{
		printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	}

#else
	mrd.InVif = mld_up_if_idx;
	//memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	mrd.TtlVc[mld_down_if_idx] = enable;	
	addMRoute(&mrd);
#endif

	return 1;
}

#else
int mld_add_mr(struct in6_addr group, struct in6_addr src, int enable ,int ifindex)
/*{
	struct MRouteDesc	mrd;

	//add multicast routing entry 
	mrd.OriginAdr.s_addr = src;
	mrd.SubsAdr.s_addr = 0;
	mrd.McAdr.s_addr = group;

	mrd.InVif = igmp_up_if_idx;
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	mrd.TtlVc[igmp_down_if_idx] = enable;	

	MLDV2LOG( "%s> group:%s", __FUNCTION__, inet_ntoa(mrd.McAdr) );
	MLDV2LOG( ", src:%s, enable:%d\n", inet_ntoa(mrd.OriginAdr), enable );

	return (addMRoute(&mrd));
}*/
{
	struct MRouteDesc	mrd;
	/* add multicast routing entry */
	memcpy(&(mrd.OriginAdr),&(src),sizeof(struct in6_addr));
	memset(&(mrd.SubsAdr),0,sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	
#if defined IPV6_READY_LOGOV2
	//int pifindex;
	struct mcft_entry *mymcp=get_mcft(group);
	//struct mbr_entry 	*gcp=mymcp->grp_mbr;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp;//= getIfByPix(infindex);
	uint8 cin_ix;//=(uint8)cin_dp->vif_idx;
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	printf( "srcaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	src.s6_addr16[ 0],src.s6_addr16[ 1],src.s6_addr16[ 2],src.s6_addr16[ 3],
	src.s6_addr16[ 4],src.s6_addr16[ 5],src.s6_addr16[ 6],src.s6_addr16[ 7]);
	
	printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	group.s6_addr16[ 0],group.s6_addr16[ 1],group.s6_addr16[ 2],group.s6_addr16[ 3],
	group.s6_addr16[ 4],group.s6_addr16[ 5],group.s6_addr16[ 6],group.s6_addr16[ 7]);
	if (mymcp->if_count==1)
	{
		//the first time receive report,and if_count=1
		printf("if_count=1,[%s]:[%d].\n",__FUNCTION__,__LINE__);
		cin_dp= getIfByPix(ifindex);
		cin_ix=(uint8)cin_dp->vif_idx;
		mrd.TtlVc[cin_ix] = enable;
		printf("ifindex=%d,cin_ix=%d\n",ifindex,cin_ix);
	}
	else{
		printf("if_count!=1,[%s]:[%d].\n",__FUNCTION__,__LINE__);
		int pifindex;
		//struct mcft_entry *mymcp=get_mcft(group);
		struct ifrec_entry *if_rec=mymcp->iflist;
		//not the first time receive report,and add all interface who received report togeter 
		while(if_rec){
			pifindex=if_rec->ifindex;
			if(if_rec->ifindex!=ifindex){
				//if_rec->mrt_state =0;
			}
			cin_dp= getIfByPix(pifindex);
			cin_ix=(uint8)cin_dp->vif_idx;
			mrd.TtlVc[cin_ix] = enable;	//data pkt out interface==interface receive report
			if_rec=if_rec->next;
			
		}
	}
	//up don't receive report,then up in
	if((int)(up_dp->pif_idx)!=ifindex){
		mrd.InVif = mld_up_if_idx;
		printf("up_dp->pif_idx:%d,ifindex:%d,[%s]:[%d].\n",up_dp->pif_idx,ifindex,__FUNCTION__,__LINE__);
		addMRoute(&mrd);
		
	}
	//down don't receive report and  down in
	if((int)(down_dp->pif_idx)!=ifindex){
		mrd.InVif = mld_down_if_idx;
		printf("down_dp->pif_idx:%d,ifindex:%d,[%s]:[%d].\n",down_dp->pif_idx,ifindex,__FUNCTION__,__LINE__);
		addMRoute(&mrd);
	}

#else
	mrd.InVif = mld_up_if_idx;
	//memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	mrd.TtlVc[mld_down_if_idx] = enable;	
	addMRoute(&mrd);
#endif

	return 1;
}
//Question: Ttl Vc ÖÐ´«ÈëÒ»¸öenableµÄ±äÁ¿

int mld_del_mr( struct in6_addr group, struct in6_addr src,int ifindex )
/*
{
	struct MRouteDesc	mrd;
	int ret=0;

	//delete multicast routing entry 
	mrd.OriginAdr.s_addr = src;
	mrd.McAdr.s_addr = group;
	mrd.InVif = igmp_up_if_idx;
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));

	MLDV2LOG( "%s> group:%s", __FUNCTION__, inet_ntoa(mrd.McAdr) );
	MLDV2LOG( ", src:%s\n", inet_ntoa(mrd.OriginAdr) );

	delMRoute(&mrd);	
	return ret;
}
*/
{
	struct MRouteDesc	mrd;
	/* delete multicast routing entry */
	memcpy(&(mrd.OriginAdr),&src,sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&group,sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
#if defined IPV6_READY_LOGOV2
	
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp= getIfByPix(ifindex);
	uint8 cin_ix=(uint8)cin_dp->vif_idx;
	if(ifindex==-1){
		mrd.InVif=-1;
		delMRoute(&mrd);
	}
	else{	
		//up don't receive report,then del mfc in up
		if((int)(up_dp->pif_idx)!=ifindex){
			mrd.InVif = mld_up_if_idx;
			
			//mrd.TtlVc[cin_ix] = 1;	//data pkt out interface==interface receive interface
			delMRoute(&mrd);
			
		}
		//down don't receive report and del mfc in down
		if((int)(down_dp->pif_idx)!=ifindex){
			mrd.InVif = mld_down_if_idx;
			//mrd.TtlVc[cin_ix] = 1;	//data pkt out interface==interface receive interface
			delMRoute(&mrd);
		}
	}
#else
	mrd.InVif = mld_up_if_idx;
	
	delMRoute(&mrd);
#endif
	return 1;
}

//update mfc
int mld_update_mr(struct in6_addr group, struct in6_addr src, int enable ,int ifindex)
{
	struct MRouteDesc	mrd;
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	/* add multicast routing entry */
	memcpy(&(mrd.OriginAdr),&(src),sizeof(struct in6_addr));
	memset(&(mrd.SubsAdr),0,sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	
#if defined IPV6_READY_LOGOV2
	//int pifindex;
	struct mcft_entry *mymcp=get_mcft(group);
	//struct mbr_entry	*gcp=mymcp->grp_mbr;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp;//= getIfByPix(infindex);
	uint8 cin_ix;//=(uint8)cin_dp->vif_idx;
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	printf( "srcaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	src.s6_addr16[ 0],src.s6_addr16[ 1],src.s6_addr16[ 2],src.s6_addr16[ 3],
	src.s6_addr16[ 4],src.s6_addr16[ 5],src.s6_addr16[ 6],src.s6_addr16[ 7]);
	
	printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	group.s6_addr16[ 0],group.s6_addr16[ 1],group.s6_addr16[ 2],group.s6_addr16[ 3],
	group.s6_addr16[ 4],group.s6_addr16[ 5],group.s6_addr16[ 6],group.s6_addr16[ 7]);
	
		
	{
		printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		int pifindex;
		//struct mcft_entry *mymcp=get_mcft(group);
		struct ifrec_entry *if_rec=mymcp->iflist;
		//not the first time receive report,and add all interface who received report togeter 
		while(if_rec){
			pifindex=if_rec->ifindex;
			cin_dp= getIfByPix(pifindex);
			cin_ix=(uint8)cin_dp->vif_idx;	
			if(if_rec->ifindex!=ifindex){
				printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				mrd.TtlVc[cin_ix] = 1; //data pkt out interface==interface receive report
			}
			else{
				printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				mrd.TtlVc[cin_ix] = enable; //update mfc
			}
			if_rec=if_rec->next;
			
		}
	}
	if (enable==0)//means the interface leave
	{
		//up leave group ,then up in
		if((int)(up_dp->pif_idx)==ifindex){
			mrd.InVif = mld_up_if_idx;
			printf("up_dp->pif_idx:%d,ifindex:%d,[%s]:[%d].\n",up_dp->pif_idx,ifindex,__FUNCTION__,__LINE__);
			addMRoute(&mrd);
			
		}
		//down leave group and  down in
		if((int)(down_dp->pif_idx)==ifindex){
			mrd.InVif = mld_down_if_idx;
			printf("down_dp->pif_idx:%d,ifindex:%d,[%s]:[%d].\n",down_dp->pif_idx,ifindex,__FUNCTION__,__LINE__);
			addMRoute(&mrd);
		}
	}
	else{
		printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	}

#else
	mrd.InVif = mld_up_if_idx;
	//memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	mrd.TtlVc[mld_down_if_idx] = enable;	
	addMRoute(&mrd);
#endif

	return 1;
}

#endif

#ifdef CONFIG_MLDPROXY_MULTIWAN
int mld_set_srcfilter( struct mcft_entry *p )
{

#if 0
	struct ip_msfilter *imsfp;
	int	size,i;
	struct IfDesc *up_dp ;
	__u32 group;
	struct src_entry *s;
	int idx;
	if(p==NULL)	
		return -1;

	  
    //        up_dp = getIfByName(mld_up_if_name[idx]);
			
	   /*use the "send_buf buffer*/
	   imsfp = (struct ip_msfilter *)send_buf;
	   imsfp->imsf_multiaddr=p->grp_addr;
//	   imsfp->imsf_interface=up_dp->InAdr.s_addr;
	   imsfp->imsf_fmode=p->filter_mode;
	   imsfp->imsf_numsrc=0;
	   MLDV2LOG( "%s> maddr:%s", __FUNCTION__, inet_ntoa(imsfp->imsf_multiaddr) );
	   MLDV2LOG( ", if:%s, fmode:%d\n", inet_ntoa(imsfp->imsf_interface), imsfp->imsf_fmode  );

	   i=0;
	   s=p->srclist;
	     while(s)
	   {
		MLDV2LOG( "%s>try to match=> fmode:%d, timer:%d, slist:%s\n", __FUNCTION__, p->filter_mode, s->timer.lefttime, inet_ntoa(s->srcaddr) );
		if( ((p->filter_mode==MCAST_INCLUDE) && (s->timer.lefttime>0)) ||
		    ((p->filter_mode==MCAST_EXCLUDE) && (s->timer.lefttime==0)) )
		{
			imsfp->imsf_slist[i] = s->srcaddr;
			MLDV2LOG( "%s> slist:%s\n", __FUNCTION__, inet_ntoa(imsfp->imsf_slist[i]) );
			i++;
		}
		s=s->next;
	   }
	   imsfp->imsf_numsrc=i;
	   size = IP_MSFILTER_SIZE( i );
	   MLDV2LOG( "%s> numsrc:%d, size:%d\n", __FUNCTION__, imsfp->imsf_numsrc, size );

	   
	    for(idx=0;idx<mld_up_if_num;idx++)
          {
                  up_dp = getIfByName(mld_up_if_name[idx]);
                  imsfp->imsf_interface=up_dp->InAdr.s_addr;
		 if (setsockopt(up_dp->sock, IPPROTO_IPV6v6, IP_MSFILTER, imsfp,size) < 0 )
	        {
		perror("setsockopt IP_MSFILTER"); 
        	return -1;
	       }		  
	  }  
	return 0;
	#endif
}
#else
//need to modify
#if defined IPV6_READY_LOGOV2
int mld_set_if_srcfilter( struct ifrec_entry *p ,struct in6_addr grp_addr,int ifindex)
{

	//struct ip_msfilter *imsfp;
	int size,i;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct in6_addr group;
	struct src_entry *s;
	struct IfDesc *dp_tmp;
	int num_src;
	struct group_source_req gsr;
	gsr.gsr_group.ss_family=AF_INET6;
	gsr.gsr_source.ss_family=AF_INET6;
	memcpy(&group,&grp_addr,sizeof(struct in6_addr));
	memcpy(&gsr.gsr_group,&group,sizeof(struct in6_addr));

	if(p==NULL){	
		printf("ifrec_entry is null![%s]:[%d].\n",__FUNCTION__,__LINE__);
		return -1;
	}	
	//need to modify
	/*use the "send_buf buffer*/
	
	i=0;
	s=p->srclist;
	if (s==NULL){
		printf("srclist is null,no need to set srcfilter![%s][%d].\n",__FUNCTION__,__LINE__);
		return 0;
	}
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	//determine If to setsockopt 
	if (up_dp->pif_idx!=(__u16)ifindex){
		dp_tmp=up_dp;
		printf("dp_tmp->pif_idx=%d,ifindex=%d,[%s]:[%d].\n",dp_tmp->pif_idx,ifindex,__FUNCTION__,__LINE__);
	}
	if (down_dp->pif_idx!=(__u16)ifindex){
		dp_tmp=down_dp;
		printf("dp_tmp->pif_idx=%d,ifindex=%d,[%s]:[%d].\n",dp_tmp->pif_idx,ifindex,__FUNCTION__,__LINE__);
	}
	while(s)
	{
		//MLDV2LOG( "%s>try to match=> fmode:%d, timer:%d, slist:%s\n", __FUNCTION__, p->filter_mode, s->timer.lefttime, inet_ntoa(s->srcaddr) );
		if( ((p->filter_mode==MCAST_INCLUDE) && (s->timer.lefttime>0)) ||
			((p->filter_mode==MCAST_EXCLUDE) && (s->timer.lefttime==0)) )
		{
			//imsfp->imsf_slist[i] = s->srcaddr;
			memcpy(&gsr.gsr_source,&s->srcaddr,sizeof(struct in6_addr));
			//MLDV2LOG( "%s> slist:%s\n", __FUNCTION__, inet_ntoa(imsfp->imsf_slist[i]) );
			printf("setsockopt MCAST_JOIN_SOURCE_GROUP !![%s]:[%d]\n",__FUNCTION__,__LINE__);
			if (setsockopt(dp_tmp->sock, IPPROTO_IPV6, MCAST_JOIN_SOURCE_GROUP, (char *)&gsr,sizeof(gsr)) < 0 )
			{
				printf("setsockopt MCAST_JOIN_SOURCE_GROUP error!!\n");
				log( LOG_WARNING, errno, "MCAST_JOIN_SOURCE_GROUP" );
			//perror("setsockopt IP_MSFILTER"); 
			//return -1;
			}
			i++;
		}
		else if( ((p->filter_mode==MCAST_EXCLUDE) && (s->timer.lefttime>0)) ||
			((p->filter_mode==MCAST_INCLUDE) && (s->timer.lefttime==0)) )
		{
			//imsfp->imsf_slist[i] = s->srcaddr;
			memcpy(&gsr.gsr_source,&s->srcaddr,sizeof(struct in6_addr));
			//MLDV2LOG( "%s> slist:%s\n", __FUNCTION__, inet_ntoa(imsfp->imsf_slist[i]) );
			printf("setsockopt MCAST_LEAVE_SOURCE_GROUP!![%s]:[%d]\n",__FUNCTION__,__LINE__);
			if (setsockopt(dp_tmp->sock, IPPROTO_IPV6, MCAST_LEAVE_SOURCE_GROUP, (char *)&gsr,sizeof(gsr)) < 0 )
			{
				printf("setsockopt MCAST_LEAVE_SOURCE_GROUP error!!\n");
				log( LOG_WARNING, errno, "MCAST_LEAVE_SOURCE_GROUP" );
			//perror("setsockopt IP_MSFILTER"); 
			//return -1;
			}
			i++;
		}
		s=s->next;
	}
	num_src=i;
	printf( "numsrc:%d,[%s]:[%d]\n",num_src, __FUNCTION__,__LINE__);

	return 0;

}

#else
int mld_set_srcfilter( struct mcft_entry *p ,int ifindex)

{

	//struct ip_msfilter *imsfp;
	int	size,i;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct in6_addr group;
	struct src_entry *s;
	struct IfDesc *dp_tmp;
	int num_src;
	struct group_source_req gsr;
	gsr.gsr_group.ss_family=AF_INET6;
	gsr.gsr_source.ss_family=AF_INET6;
	memcpy(&gsr.gsr_group,&p->grp_addr,sizeof(struct in6_addr));

	if(p==NULL){	
		printf("mcft_entry is null![%s]:[%d].\n",__FUNCTION__,__LINE__);
		return -1;
	}	
	//need to modify
	/*use the "send_buf buffer*/
	
	i=0;
	s=p->srclist;
	
	while(s)
	{
		//MLDV2LOG( "%s>try to match=> fmode:%d, timer:%d, slist:%s\n", __FUNCTION__, p->filter_mode, s->timer.lefttime, inet_ntoa(s->srcaddr) );
		if( ((p->filter_mode==MCAST_INCLUDE) && (s->timer.lefttime>0)) ||
		    ((p->filter_mode==MCAST_EXCLUDE) && (s->timer.lefttime==0)) )
		{
			//imsfp->imsf_slist[i] = s->srcaddr;
			memcpy(&gsr.gsr_source,&s->srcaddr,sizeof(struct in6_addr));
			//MLDV2LOG( "%s> slist:%s\n", __FUNCTION__, inet_ntoa(imsfp->imsf_slist[i]) );
			printf("setsockopt MCAST_JOIN_SOURCE_GROUP !![%s]:[%d]\n",__FUNCTION__,__LINE__);
			if (setsockopt(up_dp->sock, IPPROTO_IPV6, MCAST_JOIN_SOURCE_GROUP, (char *)&gsr,sizeof(gsr)) < 0 )
			{
				printf("setsockopt MCAST_JOIN_SOURCE_GROUP error!!\n");
				log( LOG_WARNING, errno, "MCAST_JOIN_SOURCE_GROUP" );
			//perror("setsockopt IP_MSFILTER"); 
        	//return -1;
			}
			i++;
		}
		else if( ((p->filter_mode==MCAST_EXCLUDE) && (s->timer.lefttime>0)) ||
		    ((p->filter_mode==MCAST_INCLUDE) && (s->timer.lefttime==0)) )
		{
			//imsfp->imsf_slist[i] = s->srcaddr;
			memcpy(&gsr.gsr_source,&s->srcaddr,sizeof(struct in6_addr));
			//MLDV2LOG( "%s> slist:%s\n", __FUNCTION__, inet_ntoa(imsfp->imsf_slist[i]) );
			printf("setsockopt MCAST_LEAVE_SOURCE_GROUP !![%s]:[%d]\n",__FUNCTION__,__LINE__);
			if (setsockopt(up_dp->sock, IPPROTO_IPV6, MCAST_LEAVE_SOURCE_GROUP, (char *)&gsr,sizeof(gsr)) < 0 )
			{
				printf("setsockopt MCAST_LEAVE_SOURCE_GROUP error!!\n");
				log( LOG_WARNING, errno, "MCAST_LEAVE_SOURCE_GROUP" );
			//perror("setsockopt IP_MSFILTER"); 
        	//return -1;
			}
			i++;
		}
		s=s->next;
	}
	num_src=i;
	printf( "numsrc:%d,[%s]:[%d]\n",num_src, __FUNCTION__,__LINE__);

	return 0;


}
#endif

#endif
int mld_set_grp_srcfilter( struct mcft_entry *p ,int ifindex)

{

	//struct ip_msfilter *imsfp;
	struct ifrec_entry *if_rec;
	int	size,i;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct in6_addr group;
	struct src_entry *s;
	struct IfDesc *dp_tmp;
	int num_src;
	struct group_source_req gsr;
	gsr.gsr_group.ss_family=AF_INET6;
	gsr.gsr_source.ss_family=AF_INET6;
	memcpy(&gsr.gsr_group,&p->grp_addr,sizeof(struct in6_addr));
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	if(p==NULL){	
		printf("mcft_entry is null![%s]:[%d].\n",__FUNCTION__,__LINE__);
		return -1;
	}	
	//need to modify
	/*use the "send_buf buffer*/
	
	i=0;
	//if_rec=p->iflist;
	i=0;
	s=p->srclist;
	if (s==NULL){
		printf("srclist is null,no need to set srcfilter![%s][%d].\n",__FUNCTION__,__LINE__);
		return 0;
	}
	
	while(s)
	{
		//MLDV2LOG( "%s>try to match=> fmode:%d, timer:%d, slist:%s\n", __FUNCTION__, p->filter_mode, s->timer.lefttime, inet_ntoa(s->srcaddr) );
		if( ((p->filter_mode==MCAST_INCLUDE) && (s->timer.lefttime>0)) ||
		    ((p->filter_mode==MCAST_EXCLUDE) && (s->timer.lefttime==0)) )
		{
			//imsfp->imsf_slist[i] = s->srcaddr;
			memcpy(&gsr.gsr_source,&s->srcaddr,sizeof(struct in6_addr));
			//MLDV2LOG( "%s> slist:%s\n", __FUNCTION__, inet_ntoa(imsfp->imsf_slist[i]) );
			printf("setsockopt MCAST_JOIN_SOURCE_GROUP !![%s]:[%d]\n",__FUNCTION__,__LINE__);
			if (setsockopt(up_dp->sock, IPPROTO_IPV6, MCAST_JOIN_SOURCE_GROUP, (char *)&gsr,sizeof(gsr)) < 0 )
			{
				printf("setsockopt MCAST_JOIN_SOURCE_GROUP error!!\n");
				log( LOG_WARNING, errno, "MCAST_JOIN_SOURCE_GROUP" );
			//perror("setsockopt IP_MSFILTER"); 
        	//return -1;
			}
			i++;
		}
		else if( ((p->filter_mode==MCAST_EXCLUDE) && (s->timer.lefttime>0)) ||
		    ((p->filter_mode==MCAST_INCLUDE) && (s->timer.lefttime==0)) )
		{
			//imsfp->imsf_slist[i] = s->srcaddr;
			memcpy(&gsr.gsr_source,&s->srcaddr,sizeof(struct in6_addr));
			//MLDV2LOG( "%s> slist:%s\n", __FUNCTION__, inet_ntoa(imsfp->imsf_slist[i]) );
			printf("setsockopt MCAST_LEAVE_SOURCE_GROUP !![%s]:[%d]\n",__FUNCTION__,__LINE__);
			if (setsockopt(up_dp->sock, IPPROTO_IPV6, MCAST_LEAVE_SOURCE_GROUP, (char *)&gsr,sizeof(gsr)) < 0 )
			{
				printf("setsockopt MCAST_LEAVE_SOURCE_GROUP error!!\n");
				log( LOG_WARNING, errno, "MCAST_LEAVE_SOURCE_GROUP" );
			//perror("setsockopt IP_MSFILTER"); 
        	//return -1;
			}
			i++;
		}
		s=s->next;
	}
	num_src=i;
	printf( "numsrc:%d,[%s]:[%d]\n",num_src, __FUNCTION__,__LINE__);

	return 0;


}

int check_src_set( struct in6_addr src, struct src_entry *srclist )
{
	struct src_entry *p;
	for( p=srclist; p!=NULL; p=p->next )
	{
		if (!memcmp( &src , &p->srcaddr,sizeof( struct in6_addr)) )
			return 1;
	}
	return 0;
}
//need to modify
int check_src( struct in6_addr src, struct in6_addr *sources, int numsrc )
{
	int i;
	for (i=0;i< numsrc; i++)
	{
		if( !memcmp(&src , &sources[i],sizeof(struct in6_addr) ))
			return 1;
	}
	return 0;
}
#if defined IPV6_READY_LOGOV2
struct ifrec_entry * check_if(struct ifrec_entry *iflist, int ifindex)
{
		
	while(iflist){
		if(ifindex==iflist->ifindex){
			printf("Find if![%s]:[%d].\n",__FUNCTION__,__LINE__);
			return iflist;
			//iflist=iflist->next;
		}	
		iflist=iflist->next;
	}
	return NULL;
}


#endif
void handle_mldv2_grp_isex(struct mcft_entry *mymcp ,int ifindex)
{
	struct mcft_entry mcp;
	struct ifrec_entry *myif_rec;
	myif_rec=get_ifrec(mymcp,ifindex);
	int grp_fmode;
	grp_fmode=mymcp->filter_mode;
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	switch(grp_fmode)
	{
		case MCAST_INCLUDE:
		{
			int i;
			struct src_entry *s, *old_set,*new_set;
			new_set = myif_rec->srclist;
			// Mason Yu Test
			//printf("handle_igmpv3_isex: MCAST_INCLUDE\n");
			//IN(A), IS_EX(B) => EX(A*B, B-A)
			old_set = mymcp->srclist;
			//for(i=0;i<srcnum;i++)
			while(new_set)
			{
				s = add_to_grp_srclist( mymcp, new_set->srcaddr);
				if(s)
				{	// (B-A)=0
					if( check_src_set( s->srcaddr, old_set )==0 )
					{
						s->timer.lefttime = 0;
						s->timer.retry_left = 0;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 0 ,ifindex);
					}
				}
				new_set=new_set->next;
			}
			
			s = old_set;
			while(s)
			{
				struct src_entry *s_next=s->next;
				
				//Delete (A-B)
				if( check_src_set( s->srcaddr, new_set)==0 )
				{
					mld_del_mr( mymcp->grp_addr, s->srcaddr,ifindex );
					del_from_grp_srclist( mymcp, s->srcaddr );
				}					
				s = s_next;
			}
			printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
			//Group Timer=GMI
			mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
			mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
			
			//set the new state
			mymcp->filter_mode = MCAST_EXCLUDE;
			mld_set_grp_srcfilter( mymcp,ifindex );
		}
			break;
		case MCAST_EXCLUDE:
		{
			int i;
			struct src_entry *s, *old_set,*new_set;
			new_set = myif_rec->srclist;
			// Mason Yu Test
			//printf("handle_igmpv3_isex: MCAST_EXCLUDE\n");

			//EX(X,Y), IS_EX(A) => EX(A-Y, Y*A)
			old_set = mymcp->srclist;
			//for(i=0;i<srcnum;i++)
			while(new_set)
			{
				s = add_to_grp_srclist( mymcp, new_set->srcaddr);
				if(s)
				{	// (A-X-Y)=Group Timer
					if( check_src_set( s->srcaddr, old_set )==0 )
					{
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1,ifindex );
					}
				}
				new_set=new_set->next;
			}
			
			s = old_set;
			while(s)
			{
				struct src_entry *s_next=s->next;
				
				//Delete (X-A), Delete(Y-A)
				if( check_src_set( s->srcaddr,new_set)==0 )
				{
					mld_del_mr( mymcp->grp_addr, s->srcaddr,ifindex );
					del_from_grp_srclist( mymcp, s->srcaddr );
				}
				s = s_next;
			}
							
			//Group Timer=GMI
			mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
			mymcp->timer.retry_left = MEMBER_QUERY_COUNT;

			//set the new state
			mymcp->filter_mode = MCAST_EXCLUDE;
			mld_set_grp_srcfilter( mymcp ,ifindex);
		}
			break;
		default:
			break;
		}	
		
}
void handle_mldv2_grp_isin(struct mcft_entry *mymcp ,int ifindex)
{
	struct mcft_entry mcp;
	struct ifrec_entry *myif_rec;
	myif_rec=get_ifrec(mymcp,ifindex);
	int grp_fmode;
	grp_fmode=mymcp->filter_mode;

	switch(grp_fmode)

	{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set, *new_set;
				new_set = myif_rec->srclist;
				//printf("handle_igmpv3_isin: MCAST_INCLUDE\n");				
				//IN(A), IN(B) => IN(A+B)
				old_set = mymcp->srclist;
				//for(i=0;i<srcnum;i++)
				while(new_set)
				{
					s = add_to_grp_srclist( mymcp, new_set->srcaddr);
					if(s)
					{	// (B)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						if( check_src_set( s->srcaddr, old_set )==0 )
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1 ,ifindex);
					}
					new_set = new_set->next;
				}

				//set the new state
				mymcp->filter_mode = MCAST_INCLUDE;
				mld_set_grp_srcfilter( mymcp, ifindex );
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set, *new_set;
				new_set = myif_rec->srclist;
				//printf("handle_igmpv3_isin: MCAST_EXCLUDE\n");
				//EX(X,Y), IS_IN(A) => EX(X+A, Y-A)
				old_set = mymcp->srclist;
				//for(i=0;i<srcnum;i++)
				while(new_set)
				{
					s = add_to_grp_srclist( mymcp, new_set->srcaddr);
					if(s)
					{	// (A)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex);
					}
					new_set=new_set->next;
				}
				
				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_grp_srcfilter( mymcp , ifindex);
			}
			break;
		default:
			break;
	}

}

void handle_mldv2_grp_toex(struct mcft_entry *mymcp ,int ifindex)
{
	struct mcft_entry mcp;
	struct ifrec_entry *myif_rec;
	myif_rec=get_ifrec(mymcp,ifindex);
	int grp_fmode;
	grp_fmode=mymcp->filter_mode;
	if(mymcp)
		return;
	{
		switch( mymcp->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set, *new_set;
				new_set=myif_rec->srclist;
				
				//IN(A), TO_EX(B) => EX(A*B, B-A)
				old_set = mymcp->srclist;
				//for(i=0;i<srcnum;i++)
				while(new_set)
				{
					s = add_to_grp_srclist( mymcp, new_set->srcaddr);
					if(s)
					{	// (B-A)=0
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							s->timer.lefttime = 0;
							s->timer.retry_left = 0;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 0 ,ifindex);
						}
					}
					new_set= new_set->next;
				}
				
				s = old_set;
				while(s)
				{
					struct src_entry *s_next=s->next;
					
					//Delete (A-B)
					if( check_src_set( s->srcaddr, new_set )==0 )
					{
						mld_del_mr( mymcp->grp_addr, s->srcaddr,ifindex );
						del_from_grp_srclist( mymcp, s->srcaddr );
					}					
					s = s_next;
				}

				//send Q(G,A*B)
				i=0;
				s = mymcp->srclist;
				while(s)
				{
					if( s->timer.lefttime > 0 )
					{
						gsrctmp[i]=s->srcaddr;
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}
					s=s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );
				
				
				//Group Timer=GMI
				mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
				
				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_grp_srcfilter( mymcp, ifindex );
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set,*new_set;
				new_set= myif_rec->srclist;
				//printf("handle_igmpv3_toex: MCAST_EXCLUDE\n");

				//EX(X,Y), TO_EX(A) => EX(A-Y, Y*A)
				old_set = mymcp->srclist;
				//for(i=0;i<srcnum;i++)
				while(new_set)
				{
					s = add_to_grp_srclist( mymcp, new_set->srcaddr);
					if(s)
					{	// (A-X-Y)=Group Timer
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							s->timer.lefttime = mymcp->timer.lefttime;
							s->timer.retry_left = MEMBER_QUERY_COUNT;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
						}
					}
					new_set=new_set->next;
				}
				
				s = old_set;
				while(s)
				{
					struct src_entry *s_next=s->next;
					
					//Delete (X-A), Delete(Y-A)
					if( check_src_set( s->srcaddr, new_set )==0 )
					{
						mld_del_mr( mymcp->grp_addr, s->srcaddr,ifindex );
						del_from_grp_srclist( mymcp, s->srcaddr );
					}
					s = s_next;
				}

				//send Q(G,A-Y)
				i=0;
				s = mymcp->srclist;
				while(s)
				{
					if( s->timer.lefttime > 0 )
					{
						gsrctmp[i]=s->srcaddr;
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}
					s=s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );
				
				//Group Timer=GMI
				mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;

				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_grp_srcfilter( mymcp,ifindex );
			}
			break;
		default:
			break;
		}
		}

}
void handle_mldv2_grp_toin(struct mcft_entry *mymcp ,int ifindex)
{
	struct mcft_entry *mcp;
	struct ifrec_entry *myif;
	if (mymcp==NULL)
		return; 
	myif=get_ifrec(mymcp,ifindex);
	if(myif)
	{
		switch( mymcp->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set,*new_set;
				new_set= myif->srclist;
				// Mason Yu Test
				//printf("handle_igmpv3_toin: MCAST_INCLUDE\n");

				//IN(A), TO_IN(B) => IN(A+B)
				old_set = mymcp->srclist;
				//for(i=0;i<srcnum;i++)
				while(new_set)
				{
					s = add_to_grp_srclist( mymcp, new_set->srcaddr);
					if(s)
					{	// (B)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						if( check_src_set( s->srcaddr, old_set )==0 )
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1,ifindex );
					}
					new_set= new_set->next;
				}
				
				//send Q(G,A-B)
				i=0;
				s = old_set;
				while(s)
				{
					if( check_src_set( s->srcaddr,new_set )==0 )
					{
						gsrctmp[i]=s->srcaddr;
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}					
					s = s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );
				
				//set the new state
				mymcp->filter_mode = MCAST_INCLUDE;
				mld_set_grp_srcfilter( mymcp,ifindex );				
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set, *new_set;
				new_set= myif->srclist;
				old_set = mymcp->srclist;
				// Mason Yu Test
				//printf("handle_igmpv3_toin: MCAST_EXCLUDE and srcnum=%d\n", srcnum);
				printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				if ( new_set == NULL ) {
				int count;
				count = del_if(mymcp, ifindex);
				if (count == 0) {// no member, drop it!
					del_mr(mymcp->grp_addr,ifindex);				
					del_mcft(mymcp->grp_addr,ifindex);
				}
				}


				//EX(X,Y), TO_IN(A) => EX(X+A, Y-A)
				old_set = mymcp->srclist;
				//for(i=0;i<srcnum;i++)
				while(new_set)
				{
					s = add_to_grp_srclist( mymcp, new_set->srcaddr);
					if(s)
					{	// (A)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1 ,ifindex);
					}
					new_set=new_set->next;
				}	

				//send Q(G,X-A)
				i=0;
				s = old_set;
				while(s)
				{
					if( s->timer.lefttime>0 )
					{
						if( check_src_set( s->srcaddr, new_set )==0 )
						{
							gsrctmp[i]=s->srcaddr;
							i++;
							if(i==MLDV2_MAX_SRCNUM) break;
						}
					}					
					s = s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );

				//send Q(G)
				if( mymcp->mld_ver==MLD_VER_2)
					mldv2_query( mymcp, 0, NULL );
				else
					mld_query(IS_MLD_ALL_HOSTS_ADDRESS, mymcp->grp_addr, LAST_MEMBER_QUERY_INTERVAL);

				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_grp_srcfilter( mymcp,ifindex );
			}
			break;
		default:
			break;
		}
		}

}

void 	handle_mldv2_grp_update(struct mcft_entry *mymcp ,int ifindex, int if_mode)
{
	int grp_mode;
	grp_mode=mymcp->filter_mode;
	switch(if_mode)
	{
		case MCAST_EXCLUDE:
			handle_mldv2_grp_isex(mymcp,ifindex);
		case MCAST_INCLUDE:
			handle_mldv2_grp_isin(mymcp,ifindex);
	}
}
void handle_mldv2_isex( struct in6_addr group, struct in6_addr src, int srcnum,struct in6_addr *grec_src,int ifindex )
{
	struct mcft_entry *mymcp;
	struct ifrec_entry *myiflist;

	// Mason Yu Test
	printf("handle_mldv2_isex\n");
	//ÔÝÊ±È¥µô¼ì²é
	#if 0	
	if(!IN_MULTICAST(group))
		return;
	/* check if it's protocol reserved group */
	if((group&0xFFFFFF00)==0xE0000000)
		printf("It's protocol | reserved group!\n");
		return;
	#endif
#if 0
	if(!chk_mcft(group)) 
	{
		//this group is a new group
		printf("NEW GROUP![%s]:[%d].\n",__FUNCTION__,__LINE__);
		//if ()
		mymcp = add_mcft(group, src, ifindex);
		if(!mymcp){ 
			printf("MYMCP IS NULL!![%s]:[%d].\n",__FUNCTION__,__LINE__);	
			return -1;
		}
#if 0//defined IPV6_READY_LOGOV2
		//check_if(mymcp,ifindex);
		//struct ifrec_entry *myiflist;
		myiflist=mymcp->iflist;
		while(myiflist){
			if(ifindex==myiflist->ifindex){
				myiflist->mld_ver = MLD_VER_1;
		
				//set the new state
				myiflist->filter_mode = MCAST_EXCLUDE;
				printf("[%s]:[%d]\n",__FUNCTION__,__LINE__);
				break;
			}
			myiflist=myiflist->next;
			
		}
		mymcp->mld_ver = MLD_VER_1;
		mld_add_group( *group,ifindex );

		//Group Timer=GMI
		mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
		mymcp->timer.retry_left = MEMBER_QUERY_COUNT;

		myiflist->mld_ver = MLD_VER_1;
		
		//set the new state
		myiflist->filter_mode = MCAST_EXCLUDE;
		mld_set_srcfilter( myiflist,ifindex );
#endif
		//printf("this group is not handled by this proxy[%s]:[%d].\n",__FUNCTION__,__LINE__);
		//return;
	}
	else{
		//group exited already
		printf("group exited already![%s]:[%d].\n",__FUNCTION__,__LINE__);
		mymcp = get_mcft(group);
		if (mymcp)
		{
#ifdef KEEP_GROUP_MEMBER
			add_user(mymcp, src, ifindex);
#endif
#if 0//defined IPV6_READY_LOGOV2
			int ret=0;
			ret=add_if(mymcp,ifindex);
			if(ret==2){
				printf("new interface added![%s]:[%d].\n",__FUNCTION__,__LINE__);
				//new interface added, mfc needed to update}
				}
#endif
		}
	}
#endif
	//printf("srcnum=%d,filter_mode=%d,ifindex=%d.[%s]:[%d].\n",srcnum,mymcp->filter_mode,ifindex,__FUNCTION__,__LINE__);	
	mymcp = get_mcft(group);
	if(mymcp)
	{
		printf("mymcp->if_count:%d.[%s]:[%d].\n",mymcp->if_count,__FUNCTION__,__LINE__);
#if defined IPV6_READY_LOGOV2
			
		//check_if(mymcp,ifindex);
		//struct if_rec_entry *myiflist;
		myiflist=get_ifrec(mymcp,ifindex);
		
		if(myiflist)
		{
			printf("find if record![%s]:[%d].\n",__FUNCTION__,__LINE__);
			
#ifdef KEEP_GROUP_MEMBER
			add_user(myiflist, src);
			printf("USER COUNT:%d.[%s]:[%d].\n",myiflist->user_count,__FUNCTION__,__LINE__);
#endif
			
		}
		else{
			printf("No if record!Add a new one![%s]:[%d].\n",__FUNCTION__,__LINE__);
			add_if(mymcp,ifindex,src);
		}
		printf("myiflist->filter_mode:%d,myiflist->ifindex:%d,ifindex:%d\n",myiflist->filter_mode,myiflist->ifindex,ifindex);
	//switch(mymcp->filter_mode){}
		switch( myiflist->filter_mode )
		{
		/*1*/
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				printf("MCAST_INCLUDE![%s]:[%d].\n",__FUNCTION__,__LINE__);
				// Mason Yu Test
				//printf("handle_mldv2_isex: MCAST_INCLUDE\n");
				//IN(A), IS_EX(B) => EX(A*B, B-A)
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( myiflist, grec_src[i] );
					if(s)
					{	// set (B-A)=0
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							printf("src[%s]:[%d].\n",__FUNCTION__,__LINE__);
							s->timer.lefttime = 0;
							s->timer.retry_left = 0;
							//block the part (B-A)
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 0 ,ifindex);
						}
						
					}
				}
				
				s = old_set;
				while(s)
				{
					struct src_entry *s_next=s->next;
					
					//Delete (A-B) forwarding
					if( check_src( s->srcaddr, grec_src, srcnum )==0 )
					{
						mld_del_mr( mymcp->grp_addr, s->srcaddr,ifindex);
						del_from_if_srclist( myiflist, s->srcaddr );
					}					
					s = s_next;
				}
				//update interface record
				myiflist->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
				
				//Group Timer=GMI
				mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
				
				//set the new state
				myiflist->filter_mode = MCAST_EXCLUDE;
			
				mld_set_if_srcfilter( myiflist,mymcp->grp_addr, ifindex );
				//Ç°Ãæ¶ÔinterfaceµÄ´¦ÀíÍêÖ®ºó²ÅÄÜ´¦ÀíÕû¸ögroup
				//mld_set_grp_srcfilter(mymcp,ifindex);
				handle_mldv2_grp_isex(mymcp,ifindex);
				
				
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				// Mason Yu Test
				printf("MCAST_EXCLUDE.[%s]:[%d]\n",__FUNCTION__,__LINE__);
#ifdef KEEP_GROUP_MEMBER
				add_user(myiflist, src);
				printf("USER COUNT:%d.[%s]:[%d].\n",myiflist->user_count,__FUNCTION__,__LINE__);
#endif

				//EX(X,Y), IS_EX(A) => EX(A-Y, Y*A)
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( myiflist, grec_src[i] );
					if(s)
					{	// (A-X-Y)=Group Timer
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							s->timer.lefttime = MEMBER_QUERY_INTERVAL;
							s->timer.retry_left = MEMBER_QUERY_COUNT;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1,ifindex );
						}
					}
				}
				
				s = old_set;
				while(s)
				{
					struct src_entry *s_next=s->next;
					printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
					//Delete (X-A), Delete(Y-A)
					if( check_src( s->srcaddr, grec_src, srcnum )==0 )
					{
						mld_del_mr( mymcp->grp_addr, s->srcaddr,ifindex );
						del_from_if_srclist( myiflist, s->srcaddr );
					}
					s = s_next;
				}
								
				//Group Timer=GMI
				mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;

				//set the new state
				myiflist->filter_mode = MCAST_EXCLUDE;
				mld_set_if_srcfilter( myiflist,mymcp->grp_addr,ifindex );

				// set new state of the group
				handle_mldv2_grp_isex(mymcp,ifindex);
			}
			break;
		default:
			break;
		}
		//handle_mldv2_grp_isex(mymcp,ifindex);

#else

		switch( mymcp->filter_mode )
		{
		/*1*/
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				printf("MCAST_INCLUDE![%s]:[%d].\n",__FUNCTION__,__LINE__);
				// Mason Yu Test
				//printf("handle_mldv2_isex: MCAST_INCLUDE\n");
				//IN(A), IS_EX(B) => EX(A*B, B-A)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (B-A)=0
						if( check_src_set( s->srcaddr, old_set )==0 )//Q:src exited in old_set ,so add mr,ÄÇ²»Ó¦¸ÃÊÇ1Âðå?
						{
							printf("src[%s]:[%d].\n",__FUNCTION__,__LINE__);
							s->timer.lefttime = 0;
							s->timer.retry_left = 0;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 0 ,ifindex);
						}
					}
				}
				
				s = old_set;
				while(s)
				{
					struct src_entry *s_next=s->next;
					
					//Delete (A-B)
					if( check_src( s->srcaddr, grec_src, srcnum )==0 )
					{
						mld_del_mr( mymcp->grp_addr, s->srcaddr,ifindex);
						del_from_if_srclist( mymcp, s->srcaddr );
					}					
					s = s_next;
				}

				//Group Timer=GMI
				mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
				
				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_srcfilter( mymcp, ifindex );
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				// Mason Yu Test
				printf("handle_mldv2_isex: MCAST_EXCLUDE.[%s]:[%d]\n",__FUNCTION__,__LINE__);

				//EX(X,Y), IS_EX(A) => EX(A-Y, Y*A)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
					s = add_to_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (A-X-Y)=Group Timer
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							s->timer.lefttime = MEMBER_QUERY_INTERVAL;
							s->timer.retry_left = MEMBER_QUERY_COUNT;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1,ifindex );
						}
					}
				}
				
				s = old_set;
				while(s)
				{
					struct src_entry *s_next=s->next;
					printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
					//Delete (X-A), Delete(Y-A)
					if( check_src( s->srcaddr, grec_src, srcnum )==0 )
					{
						printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
						mld_del_mr( mymcp->grp_addr, s->srcaddr,ifindex );
						del_from_if_srclist( mymcp, s->srcaddr );
					}
					s = s_next;
				}
								
				//Group Timer=GMI
				mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;

				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				mld_set_srcfilter( mymcp,ifindex );
			}
			break;
		default:
			break;
		}
#endif
	
	}
}


void handle_mldv2_isin( struct in6_addr group,struct in6_addr src, int srcnum, struct in6_addr *grec_src,int ifindex )
{
	struct mcft_entry *mymcp;
	struct ifrec_entry *myiflist;
	#if 0
	if(!IN_MULTICAST(group))
		return;
	/* check if it's protocol reserved group */
	if((group&0xFFFFFF00)==0xE0000000)
		return;
	#endif
	//#if 1
	if(!chk_mcft(group)) 
	{
		//this group is new
		add_mcft(group, src, ifindex);
		//printf("this group is not handled by this proxy![%s]:[%d].\n",__FUNCTION__,__LINE__);
		//return;
	}
	else{
		//group exited already
		mymcp = get_mcft(group);
		if (mymcp)
		{
			myiflist=get_ifrec(mymcp,ifindex);
			if(myiflist)
			{
			printf("find if record![%s]:[%d].\n",__FUNCTION__,__LINE__);
			
#ifdef KEEP_GROUP_MEMBER
			add_user(myiflist, src);
			printf("USER COUNT:%d.[%s]:[%d].\n",myiflist->user_count,__FUNCTION__,__LINE__);
#endif
			
			}else{
			printf("No if record!Add a new one![%s]:[%d].\n",__FUNCTION__,__LINE__);
			add_if(mymcp,ifindex,src);
			}
		printf("myiflist->filter_mode:%d,myiflist->ifindex:%d,ifindex:%d\n",myiflist->filter_mode,myiflist->ifindex,ifindex);
			}
	}
	mymcp = get_mcft(group);
	
	if(mymcp)
	{
	//group exited already
#if defined IPV6_READY_LOGOV2
	myiflist=get_ifrec(mymcp,ifindex);
	
//#if defined IPV6_READY_LOGOV2
		switch( myiflist->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;

				//printf("handle_mldv2_isin: MCAST_INCLUDE\n");				
				//IN(A), IN(B) => IN(A+B)
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					
					s = add_to_if_srclist( myiflist, grec_src[i] );
				
					if(s)
					{	// (B)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						if( check_src_set( s->srcaddr, old_set )==0 )
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
					}
				}

				//set the new state
				myiflist->filter_mode = MCAST_INCLUDE;
				mld_set_if_srcfilter( myiflist, mymcp->grp_addr,ifindex );
				handle_mldv2_grp_isin(mymcp, ifindex);
			
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_isin: MCAST_EXCLUDE\n");
				//EX(X,Y), IS_IN(A) => EX(X+A, Y-A)
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( myiflist, grec_src[i] );
					if(s)
					{	// (A)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
					}
				}
				
				//set the new state
				myiflist->filter_mode = MCAST_EXCLUDE;
				mld_set_if_srcfilter( myiflist,mymcp->grp_addr,ifindex );
				handle_mldv2_grp_isex(mymcp, ifindex);
			}
			break;
		default:
			break;
		}
		
#else
		switch( mymcp->filter_mode )
	
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;

				//printf("handle_mldv2_isin: MCAST_INCLUDE\n");				
				//IN(A), IN(B) => IN(A+B)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					
					s = add_to_if_srclist( mymcp, grec_src[i] );
				
					if(s)
					{	// (B)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						if( check_src_set( s->srcaddr, old_set )==0 )
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
					}
				}

				//set the new state
				mymcp->filter_mode = MCAST_INCLUDE;
				//mld_set_srcfilter( mymcp,ifindex );
				mld_set_if_srcfilter( myiflist,mymcp->grp_addr,ifindex );
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_isin: MCAST_EXCLUDE\n");
				//EX(X,Y), IS_IN(A) => EX(X+A, Y-A)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (A)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
					}
				}
				
				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_srcfilter( mymcp,ifindex );
			}
			break;
		default:
			break;
		}
#endif
	}
}

void handle_mldv2_toin( struct in6_addr group, struct in6_addr src, int srcnum,struct in6_addr *grec_src,int ifindex )
{
	struct mcft_entry *mymcp;	
	struct ifrec_entry *myiflist;
	#if 0
	if(!IN_MULTICAST(group))
		return;
	/* check if it's protocol reserved group */
	if((group&0xFFFFFF00)==0xE0000000)
		return;
	#endif
	if(!chk_mcft(group)) 
	{
		printf("group not exited![%s]:[%d].\n",__FUNCTION__,__LINE__);
		return 0;
		/*
		mymcp = add_mcft(group, src, ifindex);
		if(!mymcp) return;		
		mymcp->mld_ver= MLD_VER_2;
		mld_add_group( group, ifindex );
		*/
	}
		
	mymcp = get_mcft(group);
	if(mymcp)
	{
#if defined IPV6_READY_LOGOV2
		//struct if_rec_entry *myiflist;
		myiflist = get_ifrec(mymcp, ifindex);
		
		if(myiflist!=NULL){
		printf("myiflist->filter_mode:%d,[%s]:[%d].\n",myiflist->filter_mode,__FUNCTION__,__LINE__);
		switch( myiflist->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;

				// Mason Yu Test
				//printf("handle_mldv2_toin: MCAST_INCLUDE\n");

				//IN(A), TO_IN(B) => IN(A+B)
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( myiflist, grec_src[i] );
					if(s)
					{	// (B)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						if( check_src_set( s->srcaddr, old_set )==0 )
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
					}
				}
				
				//send Q(G,A-B)
				i=0;
				s = old_set;
				while(s)
				{
					if( check_src( s->srcaddr, grec_src, srcnum )==0 )
					{
						//gsrctmp[i]=s->srcaddr;
						memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}					
					s = s->next;
				}
				if(i>0) 
					mldv2_query( mymcp, i, gsrctmp );
				
				//set the new state
				myiflist->filter_mode = MCAST_INCLUDE;
				mld_set_if_srcfilter( myiflist, mymcp->grp_addr,ifindex );	
				
				//set group new state
				handle_mldv2_grp_toin(mymcp, ifindex);
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;

				// Mason Yu Test
				printf("handle_mldv2_toin: MCAST_EXCLUDE and srcnum=%d\n", srcnum);
#ifdef KEEP_GROUP_MEMBER
				if ( srcnum == 0 ) {
				int user_count;
				printf("user_countt:%d,if_count:%d.[%s]:[%d].\n",myiflist->user_count,mymcp->if_count,__FUNCTION__,__LINE__);
#if defined IPV6_READY_LOGOV2

				int if_count;
				user_count = del_user(myiflist, src);
				if(user_count == 0)
				{
					if_count = del_if(mymcp,ifindex);
				}
				//if_count = del_if(mymcp,ifindex);//¿´ÆðÀ´²»¶Ô
				printf("user_countt:%d,if_count:%d.[%s]:[%d].\n",myiflist->user_count,mymcp->if_count,__FUNCTION__,__LINE__);
#endif
				if (if_count == 0)
				{// no member, drop it!
					printf(" no member, drop it![%s]:[%d].\n",__FUNCTION__,__LINE__);
					del_mr(mymcp->grp_addr,ifindex);    			
					del_mcft(mymcp->grp_addr,ifindex);
				}
				else if (if_count != 0)
				{
					printf("need to updat mfc!");
					struct in6_addr src_0;
					memset(&src_0, 0, sizeof(struct in6_addr));
					//int mld_update_mr(struct in6_addr group, struct in6_addr src, int enable ,int ifindex)
					mld_update_mr(mymcp->grp_addr,src_0, 0, ifindex); 
				
				}
				}
#endif

				//EX(X,Y), TO_IN(A) => EX(X+A, Y-A)
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( myiflist, grec_src[i] );
					if(s)
					{	// (A)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1 , ifindex);
					}
				}	

				//send Q(G,X-A)
				i=0;
				s = old_set;
				while(s)
				{
					if( s->timer.lefttime>0 )
					{
						if( check_src( s->srcaddr, grec_src, srcnum )==0 )
						{
							memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
							i++;
							if(i==MLDV2_MAX_SRCNUM) break;
						}
					}					
					s = s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );

				//send Q(G)
				if( myiflist->mld_ver==MLD_VER_2 ){
					mldv2_query( mymcp, 0, NULL );
					}
				else{
					//struct in6_addr dst;
					mld_query(IS_MLD_ALL_HOSTS_ADDRESS, mymcp->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
				}
				
				//set the new state
				myiflist->filter_mode = MCAST_EXCLUDE;
				mld_set_if_srcfilter( myiflist,mymcp->grp_addr, ifindex );
				
				handle_mldv2_grp_toin(mymcp, ifindex);
			}
			break;
		default:
			break;
		}
	}
#else
		switch( mymcp->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;

				// Mason Yu Test
				//printf("handle_mldv2_toin: MCAST_INCLUDE\n");

				//IN(A), TO_IN(B) => IN(A+B)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (B)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						if( check_src_set( s->srcaddr, old_set )==0 )
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
					}
				}
				
				//send Q(G,A-B)
				i=0;
				s = old_set;
				while(s)
				{
					if( check_src( s->srcaddr, grec_src, srcnum )==0 )
					{
						//gsrctmp[i]=s->srcaddr;
						memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}					
					s = s->next;
				}
				if(i>0) 
					mldv2_query( mymcp, i, gsrctmp );
				
				//set the new state
				mymcp->filter_mode = MCAST_INCLUDE;
				mld_set_srcfilter( mymcp, ifindex );				
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;

				// Mason Yu Test
				//printf("handle_mldv2_toin: MCAST_EXCLUDE and srcnum=%d\n", srcnum);
#ifdef KEEP_GROUP_MEMBER
				if ( srcnum == 0 ) {
				int count;
				count = del_user(mymcp, src);

				if (count == 0) {// no member, drop it!
					del_mr(mymcp->grp_addr,ifindex);    			
					del_mcft(mymcp->grp_addr,ifindex);
				}
				}
#endif

				//EX(X,Y), TO_IN(A) => EX(X+A, Y-A)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (A)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1 , ifindex);
					}
				}	

				//send Q(G,X-A)
				i=0;
				s = old_set;
				while(s)
				{
					if( s->timer.lefttime>0 )
					{
						if( check_src( s->srcaddr, grec_src, srcnum )==0 )
						{
							memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
							i++;
							if(i==MLDV2_MAX_SRCNUM) break;
						}
					}					
					s = s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );

				//send Q(G)
				if( mymcp->mld_ver==MLD_VER_2 ){
					mldv2_query( mymcp, 0, NULL );
					}
				else{
					//struct in6_addr dst;
					mld_query(IS_MLD_ALL_HOSTS_ADDRESS, mymcp->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
				}
				//set the new state
				//mymcp->filter_mode = MCAST_EXCLUDE;//ÉèÎªexclude²»¶Ô°É
				mymcp->filter_mode = MCAST_INCLUDE;
				mld_set_srcfilter( mymcp, ifindex );
			}
			break;
		default:
			break;
		}
#endif
	}
}


void handle_mldv2_toex( struct in6_addr group, struct in6_addr src, int srcnum, struct in6_addr *grec_src , int ifindex)
{
	struct mcft_entry *mymcp;
	struct ifrec_entry *myiflist;
	#if 0
	if(!IN_MULTICAST(group))
		return;
	/* check if it's protocol reserved group */
	if((group&0xFFFFFF00)==0xE0000000)
		return;
	#endif
	if(!chk_mcft(group)) 
	{
		mymcp = add_mcft(group, src, ifindex);
		if(!mymcp){ 
			printf("MYMCP IS NULL!![%s]:[%d].\n",__FUNCTION__,__LINE__);	
			return -1;
		}
		//interface added in add_mcft
		myiflist = get_ifrec(mymcp,ifindex);
		if(myiflist ==NULL){
			printf("myif is null!![%s].\n",__FUNCTION__);
			return -1;
		}	
		mymcp->mld_ver = MLD_VER_2;
		mld_add_group( group, ifindex );
	}
#if defined IPV6_READY_LOGOV2
	else{
	//group exited
	mymcp = get_mcft(group);
	printf("group exited[%s]:[%d].\n",__FUNCTION__,__LINE__);

	if (mymcp){		
		printf("mymcp->if_count:%d.[%s]:[%d].\n",mymcp->if_count,__FUNCTION__,__LINE__);
			
		myiflist=get_ifrec(mymcp,ifindex);
		
		if(myiflist)
		{
			printf("find if record![%s]:[%d].\n",__FUNCTION__,__LINE__);
			
#ifdef KEEP_GROUP_MEMBER
			add_user(myiflist, src);
			printf("USER COUNT:%d.[%s]:[%d].\n",myiflist->user_count,__FUNCTION__,__LINE__);
#endif
			
		}
		else{
			printf("No if record!Add a new one![%s]:[%d].\n",__FUNCTION__,__LINE__);
			add_if(mymcp,ifindex,src);
		}
		printf("myiflist->filter_mode:%d,myiflist->ifindex:%d,ifindex:%d\n",myiflist->filter_mode,myiflist->ifindex,ifindex);
	
		}
	}
		
#endif
	printf("[%s]:[%d]\n",__FUNCTION__,__LINE__);
	mymcp = get_mcft(group);
	myiflist = get_ifrec(mymcp, ifindex);
	if(mymcp)
	{
#if defined IPV6_READY_LOGOV2

		switch(myiflist->filter_mode )
		{
			case MCAST_INCLUDE:
				{
					int i;
					struct src_entry *s, *old_set;
					
					printf("handle_mldv2_toex: MCAST_INCLUDE\n");
#ifdef KEEP_GROUP_MEMBER				
					//add_user(mymcp, src,ifindex);
					add_user(myiflist, src);
#endif
					//IN(A), TO_EX(B) => EX(A*B, B-A)
					old_set = myiflist->srclist;
					for(i=0;i<srcnum;i++)
					{
						s = add_to_if_srclist( myiflist, grec_src[i] );
						if(s)
						{	// (B-A)=0
							if( check_src_set( s->srcaddr, old_set )==0 )
							{
								s->timer.lefttime = 0;
								s->timer.retry_left = 0;
								mld_add_mr( mymcp->grp_addr, s->srcaddr, 0, ifindex );
							}
						}
					}
					
					s = old_set;
					while(s)
					{
						struct src_entry *s_next=s->next;
						
						//Delete (A-B)
						if( check_src( s->srcaddr, grec_src, srcnum )==0 )
						{
							mld_del_mr( mymcp->grp_addr, s->srcaddr, ifindex );
							del_from_if_srclist( myiflist, s->srcaddr );
						}					
						s = s_next;
					}

					//send Q(G,A*B)
					i=0;
					s = myiflist->srclist;
					while(s)
					{
						if( s->timer.lefttime > 0 )
						{
							memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
							i++;
							if(i==MLDV2_MAX_SRCNUM) break;
						}
						s=s->next;
					}
					if(i>0) mldv2_query( mymcp, i, gsrctmp );
					
					
					//If Timer
					myiflist->timer.lefttime = MEMBER_QUERY_INTERVAL;
					mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
					
					//Group Timer=GMI
					mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
					mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
					
					//set the new state
					myiflist->filter_mode = MCAST_EXCLUDE;
					mld_set_if_srcfilter( myiflist,mymcp->grp_addr, ifindex );
				}
				break;
			case MCAST_EXCLUDE:
				{
					int i;
					struct src_entry *s, *old_set;
					
					//printf("handle_mldv2_toex: MCAST_EXCLUDE\n");
#ifdef KEEP_GROUP_MEMBER	
					add_user(myiflist, src);
					//add_user(mymcp, src, ifindex);
#endif	

					//EX(X,Y), TO_EX(A) => EX(A-Y, Y*A)
					old_set = myiflist->srclist;
					for(i=0;i<srcnum;i++)
					{
						s = add_to_if_srclist( myiflist, grec_src[i] );
						if(s)
						{	// (A-X-Y)=Group Timer
							if( check_src_set( s->srcaddr, old_set )==0 )
							{
								s->timer.lefttime = mymcp->timer.lefttime;
								s->timer.retry_left = MEMBER_QUERY_COUNT;
								mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex);
							}
						}
					}
					
					s = old_set;
					while(s)
					{
						struct src_entry *s_next=s->next;
						
						//Delete (X-A), Delete(Y-A)
						if( check_src( s->srcaddr, grec_src, srcnum )==0 )
						{
							mld_del_mr( mymcp->grp_addr, s->srcaddr, ifindex );
							del_from_if_srclist( myiflist, s->srcaddr );
						}
						s = s_next;
					}

					//send Q(G,A-Y)
					i=0;
					s = myiflist->srclist;
					while(s)
					{
						if( s->timer.lefttime > 0 )
						{
							memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
							i++;
							if(i==MLDV2_MAX_SRCNUM) break;
						}
						s=s->next;
					}
					if(i>0) mldv2_query( mymcp, i, gsrctmp );
					//If Timer
					myiflist->timer.lefttime = MEMBER_QUERY_INTERVAL;
					mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
					
					//Group Timer=GMI
					mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
					mymcp->timer.retry_left = MEMBER_QUERY_COUNT;

					//set the new state
					myiflist->filter_mode = MCAST_EXCLUDE;
					mld_set_if_srcfilter( myiflist, mymcp->grp_addr,ifindex );
				}
				break;
			default:
				break;
			}
		//set new state of group
		handle_mldv2_grp_isex(mymcp, ifindex);
#else
		switch( mymcp->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_toex: MCAST_INCLUDE\n");
#ifdef KEEP_GROUP_MEMBER				
				add_user(mymcp, src,ifindex);
#endif
				//IN(A), TO_EX(B) => EX(A*B, B-A)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (B-A)=0
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							s->timer.lefttime = 0;
							s->timer.retry_left = 0;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 0, ifindex );
						}
					}
				}
				
				s = old_set;
				while(s)
				{
					struct src_entry *s_next=s->next;
					
					//Delete (A-B)
					if( check_src( s->srcaddr, grec_src, srcnum )==0 )
					{
						mld_del_mr( mymcp->grp_addr, s->srcaddr, ifindex );
						del_from_if_srclist( mymcp, s->srcaddr );
					}					
					s = s_next;
				}

				//send Q(G,A*B)
				i=0;
				s = mymcp->srclist;
				while(s)
				{
					if( s->timer.lefttime > 0 )
					{
						memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}
					s=s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );
				
				
				//Group Timer=GMI
				mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;
				
				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_srcfilter( mymcp, ifindex );
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_toex: MCAST_EXCLUDE\n");
#ifdef KEEP_GROUP_MEMBER				
				add_user(mymcp, src, ifindex);
#endif	
//#if defined IPV6_READY_LOGOV2
//				add_if(mymcp,ifindex);
//#endif
				//EX(X,Y), TO_EX(A) => EX(A-Y, Y*A)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (A-X-Y)=Group Timer
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							s->timer.lefttime = mymcp->timer.lefttime;
							s->timer.retry_left = MEMBER_QUERY_COUNT;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex);
						}
					}
				}
				
				s = old_set;
				while(s)
				{
					struct src_entry *s_next=s->next;
					
					//Delete (X-A), Delete(Y-A)
					if( check_src( s->srcaddr, grec_src, srcnum )==0 )
					{
						mld_del_mr( mymcp->grp_addr, s->srcaddr, ifindex );
						del_from_if_srclist( mymcp, s->srcaddr );
					}
					s = s_next;
				}

				//send Q(G,A-Y)
				i=0;
				s = mymcp->srclist;
				while(s)
				{
					if( s->timer.lefttime > 0 )
					{
						memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}
					s=s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );
				
				//Group Timer=GMI
				mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
				mymcp->timer.retry_left = MEMBER_QUERY_COUNT;

				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_srcfilter( mymcp, ifindex );
			}
			break;
		default:
			break;
		}
#endif
	}
}


void handle_mldv2_allow(struct in6_addr group, struct in6_addr src, int srcnum, struct in6_addr *grec_src, int ifindex )
{
#if 1
	struct mcft_entry *mymcp;
	#if 0
	if(!IN_MULTICAST(group))
		return;
	/* check if it's protocol reserved group */
	if((group&0xFFFFFF00)==0xE0000000)
		return;
	#endif
	if(!chk_mcft(group)) 
	{
		mymcp = add_mcft(group, src, ifindex);
		if(!mymcp) return;		
		mymcp->mld_ver = MLD_VER_2;
		mld_add_group( group, ifindex );
	}
		
	mymcp = get_mcft(group);
	if(mymcp)
	{
#if defined IPV6_READY_LOGOV2
		struct ifrec_entry *myiflist;
		myiflist=get_ifrec(mymcp,ifindex);
		switch(myiflist->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_allow: MCAST_INCLUDE\n");				
				//IN(A), ALLOW(B) => IN(A+B)
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist(myiflist, grec_src[i] );
					if(s)
					{	// (B)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						if( check_src_set( s->srcaddr, old_set )==0 )
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
					}
				}

				//set the new state
				myiflist->filter_mode = MCAST_INCLUDE;
				mld_set_if_srcfilter( myiflist,mymcp->grp_addr, ifindex );

				//set new state of group
				handle_mldv2_grp_isin(mymcp, ifindex);
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_allow: MCAST_EXCLUDE\n");
				//EX(X,Y), ALLOW(A) => EX(X+A, Y-A)
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( myiflist, grec_src[i] );
					if(s)
					{	// (A)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1,ifindex );
					}
				}
				
				//set the new state
				myiflist->filter_mode = MCAST_EXCLUDE;
				mld_set_if_srcfilter( myiflist, mymcp->grp_addr, ifindex );

				//set new state of group
				handle_mldv2_grp_isex(mymcp, ifindex);
			}
			break;
		default:
			break;
		}
#else
		switch( mymcp->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_allow: MCAST_INCLUDE\n");				
				//IN(A), ALLOW(B) => IN(A+B)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (B)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						if( check_src_set( s->srcaddr, old_set )==0 )
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
					}
				}

				//set the new state
				mymcp->filter_mode = MCAST_INCLUDE;
				mld_set_if_srcfilter( mymcp, mymcp->grp_addr, ifindex );
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_allow: MCAST_EXCLUDE\n");
				//EX(X,Y), ALLOW(A) => EX(X+A, Y-A)
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (A)= GMI
						s->timer.lefttime = MEMBER_QUERY_INTERVAL;
						s->timer.retry_left = MEMBER_QUERY_COUNT;
						mld_add_mr( mymcp->grp_addr, s->srcaddr, 1,ifindex );
					}
				}
				
				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_if_srcfilter( mymcp, mymcp->grp_addr, ifindex );
			}
			break;
		default:
			break;
		}
#endif
	}
#endif
}

void handle_mldv2_block(struct in6_addr group, struct in6_addr src, int srcnum, struct in6_addr *grec_src, int ifindex )
{
#if 1
	struct mcft_entry *mymcp;
	#if 0
	if(!IN_MULTICAST(group))
		return;
	/* check if it's protocol reserved group */
	if((group&0xFFFFFF00)==0xE0000000)
		return;
	#endif
	if(!chk_mcft(group)) 
	{
		mymcp = add_mcft(group, src, ifindex);
		if(!mymcp) return;		
		mymcp->mld_ver = MLD_VER_2;
		mld_add_group( group, ifindex );
	}
		
	mymcp = get_mcft(group);
	if(mymcp)
	{
#if defined IPV6_READY_LOGOV2
	struct ifrec_entry *myiflist;
	myiflist=get_ifrec(mymcp,ifindex);
	switch( myiflist->filter_mode )
	{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				// Mason Yu Test
				//printf("handle_mldv2_block: MCAST_INCLUDE\n");
				
				//IN(A), BLOCK(B) => IN(A)
				//send Q(G,A*B)
				i=0;
				s = myiflist->srclist;
				while(s)
				{
					if( check_src( s->srcaddr, grec_src, srcnum )==1 )
					{
						memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
#ifdef KEEP_GROUP_MEMBER

						int count;
						count = del_user(mymcp, src);
						if (count == 0) {// no member, drop it!
							//del_mr(mymcp->grp_addr);
							mld_del_group(mymcp->grp_addr, ifindex);
							mld_del_mr( mymcp->grp_addr, s->srcaddr, ifindex );
							del_mcft(mymcp->grp_addr,ifindex);
						}
#endif	
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}
					s=s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );
				
			
			}
			//set new state of group
			handle_mldv2_grp_isin(mymcp, ifindex);
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_block: MCAST_EXCLUDE\n");

				//EX(X,Y), BLOCK(A) => EX( X+(A-Y), Y )
				old_set = myiflist->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_if_srclist( myiflist, grec_src[i] );
					if(s)
					{	// (A-X-Y)=Group Timer
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							s->timer.lefttime = mymcp->timer.lefttime;
							s->timer.retry_left = MEMBER_QUERY_COUNT;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
						}
					}
				}
				
				//send Q(G,A-Y)
				i=0;
				s = myiflist->srclist;
				while(s)
				{
					if( s->timer.lefttime > 0 )
					{
						if( check_src( s->srcaddr, grec_src, srcnum )==1 )
						{
							memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
#ifdef KEEP_GROUP_MEMBER

							int count;
							count = del_user(mymcp, src);
							if (count == 0) {// no member, drop it!
								//del_mr(mymcp->grp_addr);
								mld_del_group(mymcp->grp_addr, ifindex);
								mld_del_mr( mymcp->grp_addr, s->srcaddr, ifindex );
								del_mcft(mymcp->grp_addr,ifindex);
							}
#endif
							i++;
							if(i==MLDV2_MAX_SRCNUM) break;
						}
					}
					s=s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );

				//set the new state
				myiflist->filter_mode = MCAST_EXCLUDE;
				mld_set_if_srcfilter( myiflist, mymcp->grp_addr, ifindex );

				//set new state of group
				handle_mldv2_grp_isex(mymcp, ifindex);
			}
			break;
		default:
			break;
		}
#else
		switch( mymcp->filter_mode )
		{
		case MCAST_INCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				// Mason Yu Test
				//printf("handle_mldv2_block: MCAST_INCLUDE\n");
				
				//IN(A), BLOCK(B) => IN(A)
				//send Q(G,A*B)
				i=0;
				s = mymcp->srclist;
				while(s)
				{
					if( check_src( s->srcaddr, grec_src, srcnum )==1 )
					{
						memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
#ifdef KEEP_GROUP_MEMBER

						int count;
						count = del_user(mymcp, src);
						if (count == 0) {// no member, drop it!
							//del_mr(mymcp->grp_addr);
							mld_del_group(mymcp->grp_addr, ifindex);
							mld_del_mr( mymcp->grp_addr, s->srcaddr, ifindex );
							del_mcft(mymcp->grp_addr,ifindex);
						}
#endif	
						i++;
						if(i==MLDV2_MAX_SRCNUM) break;
					}
					s=s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );
				
			
			}
			break;
		case MCAST_EXCLUDE:
			{
				int i;
				struct src_entry *s, *old_set;
				
				//printf("handle_mldv2_block: MCAST_EXCLUDE\n");

				//EX(X,Y), BLOCK(A) => EX( X+(A-Y), Y )
				old_set = mymcp->srclist;
				for(i=0;i<srcnum;i++)
				{
					s = add_to_srclist( mymcp, grec_src[i] );
					if(s)
					{	// (A-X-Y)=Group Timer
						if( check_src_set( s->srcaddr, old_set )==0 )
						{
							s->timer.lefttime = mymcp->timer.lefttime;
							s->timer.retry_left = MEMBER_QUERY_COUNT;
							mld_add_mr( mymcp->grp_addr, s->srcaddr, 1, ifindex );
						}
					}
				}
				
				//send Q(G,A-Y)
				i=0;
				s = mymcp->srclist;
				while(s)
				{
					if( s->timer.lefttime > 0 )
					{
						if( check_src( s->srcaddr, grec_src, srcnum )==1 )
						{
							memcpy(&gsrctmp[i],&s->srcaddr,sizeof(struct in6_addr));
#ifdef KEEP_GROUP_MEMBER

							int count;
							count = del_user(mymcp, src);
							if (count == 0) {// no member, drop it!
								//del_mr(mymcp->grp_addr);
								mld_del_group(mymcp->grp_addr, ifindex);
								mld_del_mr( mymcp->grp_addr, s->srcaddr, ifindex );
								del_mcft(mymcp->grp_addr,ifindex);
							}
#endif
							i++;
							if(i==MLDV2_MAX_SRCNUM) break;
						}
					}
					s=s->next;
				}
				if(i>0) mldv2_query( mymcp, i, gsrctmp );

				//set the new state
				mymcp->filter_mode = MCAST_EXCLUDE;
				mld_set_srcfilter( mymcp, ifindex );
			}
			break;
		default:
			break;
		}
#endif
	}
#endif
}

int mldv2_query( struct mcft_entry *entry, int srcnum, struct in6_addr *srcfilter )
{
  	struct mld2_query	*mldv2_q=(struct mld_msg *)mld6_send_buf;	
    //struct sockaddr_in6	sdst;
    struct IfDesc 	*down_dp = getIfByName(mld_down_if_name);
    struct in6_addr	grp;
    int		i;
	struct in6_addr dst;
    if(entry){ 
		//grp=entry->grp_addr;
		memcpy(&grp,&entry->grp_addr,sizeof(struct in6_addr));
	}
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	memcpy(&dst,&IS_MLD_ALL_HOSTS_ADDRESS,sizeof(struct in6_addr));
	memset(&grp,0,sizeof(struct in6_addr));
   
    printf("MAKE mldv2 query[%s]:[%d].\n",__FUNCTION__,__LINE__);
    int		totalsize=0;
    mldv2_q            = (struct mld2_query *)send_buf;
    mldv2_q->mld2q_type= ICMPV6_MGM_QUERY;
	mldv2_q->mld2q_code=LAST_MEMBER_QUERY_INTERVAL;
	mldv2_q->mld2q_mrc = htons(2000);
	memcpy(&(mldv2_q->mld2q_mca), &grp, sizeof(struct in6_addr));
    mldv2_q->mld2q_cksum     = 0;
    mldv2_q->mld2q_resv1     = 0;
    mldv2_q->mld2q_suppress  = 1;
    mldv2_q->mld2q_qrv       = 3;
    mldv2_q->mld2q_qqic      = MEMBER_QUERY_INTERVAL;
    mldv2_q->mld2q_nsrcs     = srcnum;
    totalsize	      = sizeof(struct mld2_query)+mldv2_q->mld2q_nsrcs*sizeof(struct in6_addr);//sizeof(__u32)ÊÇ·ñÐèÒªÐÞ¸Äå?
    //mldv2_q->mld2q_csum      = in_cksum((u_short *)mldv2, totalsize );
    MLDV2LOG( "%s> send to group:%s, src:", __FUNCTION__, inet_ntoa( grp ) );
    for(i=0;i<srcnum;i++)
    {
    	//mldv2_q->mld2q_srcs[i] = srcfilter[i];
		memcpy(&mldv2_q->mld2q_srcs[i],&srcfilter[i],sizeof(struct in6_addr));
		MLDV2LOG( "(%s)", inet_ntoa( mldv2_q->mld2q_srcs[i] ) );
    }
    //totalsize	      = sizeof(struct igmpv3_query)+mldv2_q->mld2q_nsrcs*sizeof(struct in6_addr);
    //mldv2_q->mld2q_csum      = in_cksum((u_short *) mldv2_q, totalsize );
    MLDV2LOG( "\n" );
	
	memset(&to, 0,sizeof(struct sockaddr_in6));
	to.sin6_family= AF_INET6;
	memcpy(&(to.sin6_addr) ,&dst,sizeof(struct in6_addr));
	/*printf("Set dst_sa![%s]:[%d].\n",__FUNCTION__,__LINE__);

	printf( "dst_sa addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	to.sin6_addr.s6_addr16[ 0],to.sin6_addr.s6_addr16[ 1],to.sin6_addr.s6_addr16[ 2],to.sin6_addr.s6_addr16[ 3],
	to.sin6_addr.s6_addr16[ 4],to.sin6_addr.s6_addr16[ 5],to.sin6_addr.s6_addr16[ 6],to.sin6_addr.s6_addr16[ 7]);
	*/
	sndmh.msg_name = (caddr_t) & to;
	sndmh.msg_namelen =sizeof(to);
#if defined IPV6_READY_LOGOV2
	struct IfDesc 	*up_dp = getIfByName(mld_up_if_name);
#if defined CONFIG_MLDPROXY_MULTIWAN
	int mld_if_num=mld_up_if_num+1;
#else
	int mld_if_num=2;
#endif
	struct IfDesc  *dp[mld_if_num];
	dp[0]=down_dp;

	for(i=1;i< mld_if_num;i++){
#ifdef CONFIG_MLDPROXY_MULTIWAN
		dp[i]= getIfByName(mld_up_if_name[i-1]);
#else
		dp[i]= getIfByName(mld_up_if_name);
#endif
		
	{
	int ctllen=0; 
	int hbhlen = 0;
	int alert=1;
	int ifindex=(int)dp[i]->pif_idx;
	struct in6_addr src;
	memcpy(&src,&(dp[i]->InAdr),sizeof(struct in6_addr));
	
	printf("Make header!![%s]:[%d].\n",__FUNCTION__,__LINE__);
	printf( "srcaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	src.s6_addr16[ 0],src.s6_addr16[ 1],src.s6_addr16[ 2],src.s6_addr16[ 3],
	src.s6_addr16[ 4],src.s6_addr16[ 5],src.s6_addr16[ 6],src.s6_addr16[ 7]);
	
	sndiov[0].iov_len = sizeof(struct mld_msg);
	//sndmh.msg_iov[0].iov_base=mldv1;
	//sndmh.msg_name=(void	*)dst_sa;
	log(LOG_DEBUG, 0, "estimate total ancillary data length\n");
	/* estimate total ancillary data length */
	if (dp[i])
		ctllen += CMSG_SPACE(sizeof(struct in6_pktinfo));
	if (alert) {
#ifdef USE_RFC2292BIS
	if ((hbhlen = inet6_opt_init(NULL, 0)) == -1)
		log(LOG_ERR, 0, "inet6_opt_init(0) failed");
	if ((hbhlen = inet6_opt_append(NULL, 0, hbhlen, IP6OPT_ROUTER_ALERT, 2,
					   2, NULL)) == -1)
		log(LOG_ERR, 0, "inet6_opt_append(0) failed");
	if ((hbhlen = inet6_opt_finish(NULL, 0, hbhlen)) == -1)
		log(LOG_ERR, 0, "inet6_opt_finish(0) failed");
	ctllen += CMSG_SPACE(hbhlen);
#else  /* old advanced API */
	hbhlen = inet6_option_space(sizeof(raopt));
	ctllen += hbhlen;
#endif
	}//end if (alert)
	log(LOG_DEBUG, 0, " extend ancillary data space\n");
	/* extend ancillary data space (if necessary) */
	if (ctlbuflen < ctllen) {
		//printf("ctlbuflen=%d,ctllen=%d,[%s]:[%d].\n",ctlbuflen,ctllen,__FUNCTION__,__LINE__);
		if (sndcmsgbuf)
			free(sndcmsgbuf);
		if ((sndcmsgbuf = malloc(ctllen)) == NULL)
			log(LOG_ERR, 0, "make_mld6_msg: malloc failed"); /* assert */
		ctlbuflen = ctllen;
	}
	log(LOG_DEBUG, 0, " store ancillary data\n");
	/* store ancillary data */
	if ((sndmh.msg_controllen = ctllen) > 0) {
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		struct cmsghdr *cmsgp=NULL;

		sndmh.msg_control = sndcmsgbuf;
		cmsgp = CMSG_FIRSTHDR(&sndmh);
		log(LOG_DEBUG, 0, " ifindex\n");
		if (dp[i]) {
			struct in6_pktinfo *pktinfo;

			cmsgp->cmsg_len = CMSG_LEN(sizeof(struct in6_pktinfo));
			cmsgp->cmsg_level = IPPROTO_IPV6;
			cmsgp->cmsg_type = IPV6_PKTINFO;
			pktinfo = (struct in6_pktinfo *)CMSG_DATA(cmsgp);
			memset((caddr_t)pktinfo, 0, sizeof(*pktinfo));
			
			pktinfo->ipi6_ifindex = ifindex;
			
			memcpy(&pktinfo->ipi6_addr, &src,sizeof(struct in6_addr));
			cmsgp = CMSG_NXTHDR(&sndmh, cmsgp);
		}
		
		log(LOG_DEBUG, 0, " alert\n");
		if (cmsgp) {
#ifdef USE_RFC2292BIS
			int currentlen;
			void *hbhbuf, *optp = NULL;

			cmsgp->cmsg_len = CMSG_LEN(hbhlen);
			cmsgp->cmsg_level = IPPROTO_IPV6;
			cmsgp->cmsg_type = IPV6_HOPOPTS;
			hbhbuf = CMSG_DATA(cmsgp);
			log(LOG_DEBUG, 0, " inet6_opt_init\n");
			if ((currentlen = inet6_opt_init(hbhbuf, hbhlen)) == -1)
				log(LOG_ERR, 0, "inet6_opt_init(len = %d) failed",
				hbhlen);
			log(LOG_DEBUG, 0, " inet6_opt_append\n");
			if ((currentlen = inet6_opt_append(hbhbuf, hbhlen,
							   currentlen,
							   IP6OPT_ROUTER_ALERT, 2,
							   2, &optp)) == -1)
				log(LOG_ERR, 0,
				"inet6_opt_append(len = %d/%d) failed",
				currentlen, hbhlen);
			log(LOG_DEBUG, 0, " inet6_opt_set_val\n");
			(void)inet6_opt_set_val(optp, 0, &rtalert_code,
						sizeof(rtalert_code));
			log(LOG_DEBUG, 0, " inet6_opt_finish\n");
			if (inet6_opt_finish(hbhbuf, hbhlen, currentlen) == -1)
				log(LOG_ERR, 0, "inet6_opt_finish(buf) failed");
#else  /* old advanced API */
			if (inet6_option_init((void *)cmsgp, &cmsgp, IPV6_HOPOPTS))
				log(LOG_ERR, 0, /* assert */
				"make_mld6_msg: inet6_option_init failed");
			if (inet6_option_append(cmsgp, raopt, 4, 0))
				log(LOG_ERR, 0, /* assert */
				"make_mld6_msg: inet6_option_append failed");
#endif 
			cmsgp = CMSG_NXTHDR(&sndmh, cmsgp);
		}
	}
	else{
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		sndmh.msg_control = NULL; /* clear for safety */
	}
	log(LOG_DEBUG, 0, "end make_mld6_msg\n");
	//printf("end make_mld6_msg![%s]:[%d].\n",__FUNCTION__,__LINE__);
	/*
	struct sockaddr_in6 *dstp=(struct sockaddr_in6 *)sndmh.msg_name;
	printf( "dstaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	dstp->sin6_addr.s6_addr16[ 0],dstp->sin6_addr.s6_addr16[ 1],dstp->sin6_addr.s6_addr16[ 2],dstp->sin6_addr.s6_addr16[ 3],
	dstp->sin6_addr.s6_addr16[ 4],dstp->sin6_addr.s6_addr16[ 5],dstp->sin6_addr.s6_addr16[ 6],dstp->sin6_addr.s6_addr16[ 7]);
	*/
	//printf("send_mld6_msg![%s]:[%d].\n",__FUNCTION__,__LINE__);
	if (sendmsg(dp[i]->sock, &sndmh, 0) < 0) {
		printf("SEND FAIL!!![%s]:[%d].\n",__FUNCTION__,__LINE__);
	
		if (errno == ENETDOWN){
			//check_vif_state();
			printf("ERRNO==ENETDOWN![%s]:[%d].\n",__FUNCTION__,__LINE__);
			}
		else
			log(LOG_ERR, errno, "MLD6 sendmsg.");
		return 0;
	}
 }	
	}//end for
#else
	 struct IfDesc 	*dp = down_dp;
	{
	int ctllen=0; 
	int hbhlen = 0;
	int alert=1;
	int ifindex=(int)dp->pif_idx;
	struct in6_addr src;
	memcpy(&src,&(dp->InAdr),sizeof(struct in6_addr));
	
	printf("Make header!![%s]:[%d].\n",__FUNCTION__,__LINE__);
	printf( "srcaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	src.s6_addr16[ 0],src.s6_addr16[ 1],src.s6_addr16[ 2],src.s6_addr16[ 3],
	src.s6_addr16[ 4],src.s6_addr16[ 5],src.s6_addr16[ 6],src.s6_addr16[ 7]);
	
	sndiov[0].iov_len = sizeof(struct mld_msg);
	//sndmh.msg_iov[0].iov_base=mldv1;
	//sndmh.msg_name=(void	*)dst_sa;
	log(LOG_DEBUG, 0, "estimate total ancillary data length\n");
	/* estimate total ancillary data length */
	if (dp)
		ctllen += CMSG_SPACE(sizeof(struct in6_pktinfo));
	if (alert) {
#ifdef USE_RFC2292BIS
	if ((hbhlen = inet6_opt_init(NULL, 0)) == -1)
		log(LOG_ERR, 0, "inet6_opt_init(0) failed");
	if ((hbhlen = inet6_opt_append(NULL, 0, hbhlen, IP6OPT_ROUTER_ALERT, 2,
					   2, NULL)) == -1)
		log(LOG_ERR, 0, "inet6_opt_append(0) failed");
	if ((hbhlen = inet6_opt_finish(NULL, 0, hbhlen)) == -1)
		log(LOG_ERR, 0, "inet6_opt_finish(0) failed");
	ctllen += CMSG_SPACE(hbhlen);
#else  /* old advanced API */
	hbhlen = inet6_option_space(sizeof(raopt));
	ctllen += hbhlen;
#endif
	}//end if (alert)
	log(LOG_DEBUG, 0, " extend ancillary data space\n");
	/* extend ancillary data space (if necessary) */
	if (ctlbuflen < ctllen) {
		//printf("ctlbuflen=%d,ctllen=%d,[%s]:[%d].\n",ctlbuflen,ctllen,__FUNCTION__,__LINE__);
		if (sndcmsgbuf)
			free(sndcmsgbuf);
		if ((sndcmsgbuf = malloc(ctllen)) == NULL)
			log(LOG_ERR, 0, "make_mld6_msg: malloc failed"); /* assert */
		ctlbuflen = ctllen;
	}
	log(LOG_DEBUG, 0, " store ancillary data\n");
	/* store ancillary data */
	if ((sndmh.msg_controllen = ctllen) > 0) {
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		struct cmsghdr *cmsgp=NULL;

		sndmh.msg_control = sndcmsgbuf;
		cmsgp = CMSG_FIRSTHDR(&sndmh);
		log(LOG_DEBUG, 0, " ifindex\n");
		if (dp) {
			struct in6_pktinfo *pktinfo;

			cmsgp->cmsg_len = CMSG_LEN(sizeof(struct in6_pktinfo));
			cmsgp->cmsg_level = IPPROTO_IPV6;
			cmsgp->cmsg_type = IPV6_PKTINFO;
			pktinfo = (struct in6_pktinfo *)CMSG_DATA(cmsgp);
			memset((caddr_t)pktinfo, 0, sizeof(*pktinfo));
			
			pktinfo->ipi6_ifindex = ifindex;
			
			memcpy(&pktinfo->ipi6_addr, &src,sizeof(struct in6_addr));
			cmsgp = CMSG_NXTHDR(&sndmh, cmsgp);
		}
		
		log(LOG_DEBUG, 0, " alert\n");
		if (cmsgp) {
#ifdef USE_RFC2292BIS
			int currentlen;
			void *hbhbuf, *optp = NULL;

			cmsgp->cmsg_len = CMSG_LEN(hbhlen);
			cmsgp->cmsg_level = IPPROTO_IPV6;
			cmsgp->cmsg_type = IPV6_HOPOPTS;
			hbhbuf = CMSG_DATA(cmsgp);
			log(LOG_DEBUG, 0, " inet6_opt_init\n");
			if ((currentlen = inet6_opt_init(hbhbuf, hbhlen)) == -1)
				log(LOG_ERR, 0, "inet6_opt_init(len = %d) failed",
				hbhlen);
			log(LOG_DEBUG, 0, " inet6_opt_append\n");
			if ((currentlen = inet6_opt_append(hbhbuf, hbhlen,
							   currentlen,
							   IP6OPT_ROUTER_ALERT, 2,
							   2, &optp)) == -1)
				log(LOG_ERR, 0,
				"inet6_opt_append(len = %d/%d) failed",
				currentlen, hbhlen);
			log(LOG_DEBUG, 0, " inet6_opt_set_val\n");
			(void)inet6_opt_set_val(optp, 0, &rtalert_code,
						sizeof(rtalert_code));
			log(LOG_DEBUG, 0, " inet6_opt_finish\n");
			if (inet6_opt_finish(hbhbuf, hbhlen, currentlen) == -1)
				log(LOG_ERR, 0, "inet6_opt_finish(buf) failed");
#else  /* old advanced API */
			if (inet6_option_init((void *)cmsgp, &cmsgp, IPV6_HOPOPTS))
				log(LOG_ERR, 0, /* assert */
				"make_mld6_msg: inet6_option_init failed");
			if (inet6_option_append(cmsgp, raopt, 4, 0))
				log(LOG_ERR, 0, /* assert */
				"make_mld6_msg: inet6_option_append failed");
#endif 
			cmsgp = CMSG_NXTHDR(&sndmh, cmsgp);
		}
	}
	else{
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		sndmh.msg_control = NULL; /* clear for safety */
	}
	log(LOG_DEBUG, 0, "end make_mld6_msg\n");
	//printf("end make_mld6_msg![%s]:[%d].\n",__FUNCTION__,__LINE__);
	/*
	struct sockaddr_in6 *dstp=(struct sockaddr_in6 *)sndmh.msg_name;
	printf( "dstaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	dstp->sin6_addr.s6_addr16[ 0],dstp->sin6_addr.s6_addr16[ 1],dstp->sin6_addr.s6_addr16[ 2],dstp->sin6_addr.s6_addr16[ 3],
	dstp->sin6_addr.s6_addr16[ 4],dstp->sin6_addr.s6_addr16[ 5],dstp->sin6_addr.s6_addr16[ 6],dstp->sin6_addr.s6_addr16[ 7]);
	*/
	//printf("send_mld6_msg![%s]:[%d].\n",__FUNCTION__,__LINE__);
	if (sendmsg(dp->sock, &sndmh, 0) < 0) {
		printf("SEND FAIL!!![%s]:[%d].\n",__FUNCTION__,__LINE__);
	
		if (errno == ENETDOWN){
			//check_vif_state();
			printf("ERRNO==ENETDOWN![%s]:[%d].\n",__FUNCTION__,__LINE__);
			}
		else
			log(LOG_ERR, errno, "MLD6 sendmsg.");
		return 0;
	}
 }
	#endif
	
 return 1;

	
}


#if defined IPV6_READY_LOGOV2
#if 0
void handle_group_timer(void)
{
#if 1
	struct mcft_entry *p,*next;
	struct in6_addr src_0;
	struct in6_addr src_tmp;
	int all_if=-1;
	memset (&src_tmp,0,sizeof(struct in6_addr));
	memset(&src_0,0,sizeof(struct in6_addr));
	p = mcpq;
	next = NULL;
	int ifindex;
	while( p!=NULL )
	{
		next = p->next;
		
		if( p->timer.lefttime )
		{
			p->timer.lefttime--;
			if( (p->timer.lefttime==0) && (p->timer.retry_left!=0) )
			{
				p->timer.retry_left--;
				if( p->timer.retry_left )
				{
					MLDV2LOG("%s> GROUP TIMEOUT, send Query to group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					p->timer.lefttime = LAST_MEMBER_QUERY_INTERVAL;
					
					printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
					printf("GROUP TIMEOUT, send Query![%s]:[%d]\n",__FUNCTION__,__LINE__);
					mld_query(IS_MLD_ALL_HOSTS_ADDRESS, p->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
				}
				
			}
			switch( p->filter_mode )
			{
			case MCAST_INCLUDE:
				break;
			case MCAST_EXCLUDE:
				if( p->timer.lefttime==0 )
				{
					struct src_entry *s, *s_next;
					
					MLDV2LOG("%s> group:%s is timeout(EXCLUDE mode)\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
					
					s=p->srclist;
					while(s)
					{
						s_next=s->next;
						if( s->timer.lefttime==0 )
						{
							//remove this source
							mld_del_mr( p->grp_addr, s->srcaddr,all_if );
							del_from_grp_srclist( p, s->srcaddr );
						}
						s=s_next;
					}
					
					if( p->srclist )
					{
						MLDV2LOG("%s> group:%s changes to INCLUDE mode\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						p->filter_mode=MCAST_INCLUDE;
						mld_set_grp_srcfilter( p,all_if );//ÐèÒªÐÞ¸Ä
					}else{
						MLDV2LOG"%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						//delete this group record
						if(p->mrt_state)
						{
							mld_del_mr( p->grp_addr, 0, all_if );
							p->mrt_state=0;
						}
						mld_del_group( p->grp_addr );
						del_mcft( p->grp_addr );
					}
				}
				break;
			default:
				break;
			}
			
			
		}

		p = next;
	}
#endif
}

void handle_grp_src_timer(void)
{
	struct mcft_entry *p,*next;
	int all_if=-1;
	p = mcpq;
	next = NULL;
	while( p!=NULL )
	{
		struct src_entry *s, *src_next;
		int	change_sf=0;
		
		next = p->next;
		s = p->srclist;
		src_next = NULL;
		while( s )
		{
			src_next = s->next;
			
			if( s->timer.lefttime )
			{
				s->timer.lefttime--;
				if( (s->timer.lefttime==0) && (s->timer.retry_left!=0) )
				{
					s->timer.retry_left--;
					if( s->timer.retry_left )
					{
						MLDV2LOG("%s> SRC TIMEOUT, send Query to group:%s", __FUNCTION__, inet_ntoa(p->grp_addr) );
						MLDV2LOG(", src:%s\n", __FUNCTION__, inet_ntoa(s->srcaddr) );
						
						s->timer.lefttime = LAST_MEMBER_QUERY_INTERVAL;
						mldv2_query( p, 1, &s->srcaddr );
					}
				}
				
					
				switch( p->filter_mode )
				{
				case MCAST_INCLUDE:
					if( s->timer.lefttime )
					{
						//delete default forwarding mfc
						if(p->mrt_state)
						{
							MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
							mld_del_mr( p->grp_addr, 0, all_if );
							p->mrt_state=0;
						}
					}else{
						MLDV2LOG("%s> remove src:%s", __FUNCTION__, inet_ntoa(s->srcaddr) );
						MLDV2LOG(" from group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
						
						//==0, stop this src
						mld_del_mr( p->grp_addr, s->srcaddr, all_if );
						del_from_grp_srclist( p, s->srcaddr );
						//NO MORE SOURCE, DELETE GROUP RECORD
						change_sf=1;
					}
					break;
				case MCAST_EXCLUDE:
					if( s->timer.lefttime )
					{
						#if 0
						//shouldn't delete default forwarding mfc
						if(p->mrt_state)
						{
							IGMPV3LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
							igmp_del_mr( p->grp_addr, 0 );
							p->mrt_state=0;
						}
						#endif
					}else{
						MLDV2LOG("%s> stop forwarding src:%s", __FUNCTION__, inet_ntoa(s->srcaddr) );
						MLDV2LOG(" for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						//==0, stop this src, do not remove record
						mld_add_mr( p->grp_addr, s->srcaddr, 0, );//ÐèÒªÐÞ¸Ä
						change_sf=1;
					}
					break;
				default:
					break;
				}
			}				
			s = src_next;
		}
		
		//set the new state
		if(change_sf)	mld_set_grp_srcfilter( p );//ÐèÒªÐÞ¸Ä
		
		//EX( {}, X )
		if( (p->filter_mode==MCAST_EXCLUDE) && (p->srclist!=NULL) )
		{
			int allsrcinex=1;
			s = p->srclist;
			while(s)
			{
				if(s->timer.lefttime>0)
				{
					allsrcinex=0;
					break;
				}
				s=s->next;
			}
			if(allsrcinex==1)
			{
				if( p->mrt_state==0 )
				{
					MLDV2LOG("%s> forward all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					mld_add_mr( p->grp_addr, 0, 1 );//ÐèÒªÐÞ¸Ä
					p->mrt_state = 1;
				}
			}
		}
		
		//for empty condition
		if( p->srclist==NULL )
		{
			switch( p->filter_mode )
			{
			case MCAST_INCLUDE:
				//not foreward all source
				//delete this group record
				if(p->mrt_state)
				{
					MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					mld_del_mr( p->grp_addr, 0 );
					p->mrt_state=0;
				}
				MLDV2LOG("%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
				mld_del_group( p->grp_addr );
				del_mcft( p->grp_addr );
				break;
			case MCAST_EXCLUDE:
				//forward all source
				if( p->mrt_state==0 )
				{
					MLDV2LOG("%s> forward all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					mld_add_mr( p->grp_addr, 0, 1 );
					p->mrt_state = 1;
				}
				break;
			default:
				break;
			}			
		}
		
		p = next;
	}
}
#endif
void handle_interface_timer(void)
{
#if 1
	struct mcft_entry *mymcp;
	struct ifrec_entry *myif;
	struct src_entry *src;
	
	struct mcft_entry *p,*next;
	struct in6_addr src_0;
	struct in6_addr src_tmp;
	memset (&src_tmp,0,sizeof(struct in6_addr));
	memset(&src_0,0,sizeof(struct in6_addr));
	p = mcpq;
	next = NULL;
	int ifindex;
	while( p!=NULL )
	{
		next = p->next;
		
		if( p->timer.lefttime )
		{
			p->timer.lefttime--;
			if( (p->timer.lefttime==0) && (p->timer.retry_left!=0) )
			{
				p->timer.retry_left--;
				if( p->timer.retry_left )
				{
					MLDV2LOG("%s> GROUP TIMEOUT, send Query to group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					p->timer.lefttime = LAST_MEMBER_QUERY_INTERVAL;
					
					printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
					printf("GROUP TIMEOUT, send Query![%s]:[%d]\n",__FUNCTION__,__LINE__);
					mld_query(IS_MLD_ALL_HOSTS_ADDRESS, p->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
				}
				
			}
			struct ifrec_entry *if_rec,*if_next;
			if_rec=p->iflist;
			if_next=NULL;
			while(if_rec!=NULL){
				if_next=if_rec->next;
			switch( if_rec->filter_mode )
			{
			case MCAST_INCLUDE:
				break;
			case MCAST_EXCLUDE:
				if( if_rec->timer.lefttime==0 )
				{
					struct src_entry *s, *s_next;
					int ifindex=if_rec->ifindex;
					int if_count;
					MLDV2LOG("%s> group:%s is timeout(EXCLUDE mode)\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
					printf("[%s]> interface timeout(EXCLUDE mode)!\n",__FUNCTION__);
					s=if_rec->srclist;
					while(s)
					{
						s_next=s->next;
						if( s->timer.lefttime==0 )
						{
							//remove this source
							
							//mld_del_mr( p->grp_addr, s->srcaddr, ifindex );
							del_user(if_rec,s->srcaddr);
							if(if_rec->user_count==0){
								del_if(p,ifindex);
								del_from_if_srclist( if_rec, s->srcaddr );
								if(p->if_count==0){
									printf("if_count=0,del mfc![%s]:[%d].\n",__FUNCTION__,__LINE__);
									mld_del_mr( p->grp_addr, s->srcaddr, ifindex );
									del_from_grp_srclist(p,s->srcaddr);
									
								}
								//del_from_if_srclist( if_rec, s->srcaddr );
							}
							else{
								mld_update_mr(p->grp_addr, s->srcaddr,1, ifindex);
							}
							
						}
						s=s_next;
					}
					
					if( if_rec->srclist )
					{
						MLDV2LOG("%s> group:%s changes to INCLUDE mode\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						if_rec->filter_mode=MCAST_INCLUDE;
						//mld_set_srcfilter(if_rec, ifindex );
						mld_set_if_srcfilter( if_rec,p->grp_addr,ifindex );
						//update grp state
						if(p->srclist)
						{
							p->filter_mode=MCAST_INCLUDE;
							mld_set_grp_srcfilter(p, ifindex);
						}
					}else{
						MLDV2LOG("%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
						printf("remove group![%s]:[%d].\n",__FUNCTION__,__LINE__);
						//delete this group record
						//del_user(p,src_tmp,ifindex);
						//if(if_rec->user_count==0)
						//del_if(p,ifindex);
						int mrt_state=if_rec->mrt_state;
						del_if(p,ifindex);
						if(mrt_state)
						{
							printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
							if(p->if_count==0){
								printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
								mld_del_mr( p->grp_addr, src_0, ifindex);
							//}
							//p->mrt_state=0;
							if_rec->mrt_state = 0;
							}
							else{
								
								mld_update_mr( p->grp_addr, src_0, 1, ifindex);
							}
						}
						
						if(p->if_count==0){
							mld_del_group( p->grp_addr, ifindex );
							del_mcft( p->grp_addr, ifindex );
						
						}
					}
				}
				break;
			default:
				break;
			}
			if_rec=if_next;
			}
			
			#if 0
			{
			struct ifrec_entry *if_rec,*if_next;
			if_rec=p->iflist;
			if_next=NULL;
			while(if_rec!=NULL){
			switch( if_rec->filter_mode )
			{
			case MCAST_INCLUDE:
				break;
			case MCAST_EXCLUDE:
				if( p->timer.lefttime==0 )
				{
					struct src_entry *s, *s_next;
					int ifindex=if_rec->ifindex;
					int if_count;
					MLDV2LOG("%s> group:%s is timeout(EXCLUDE mode)\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
					
					s=if_rec->srclist;
					while(s)
					{
						s_next=s->next;
						if( s->timer.lefttime==0 )
						{
							//remove this source
							
							//mld_del_mr( p->grp_addr, s->srcaddr, ifindex );
							del_user(p,src_tmp,ifindex);
							if(p->if_count==0){
								printf("if_count=0,del mfc![%s]:[%d].\n",__FUNCTION__,__LINE__);
								mld_del_mr( p->grp_addr, s->srcaddr, ifindex );
							}
							del_from_if_srclist( if_rec, s->srcaddr );
							
						}
						s=s_next;
					}
					
					if( if_rec->srclist )
					{
						MLDV2LOG("%s> group:%s changes to INCLUDE mode\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						if_rec->filter_mode=MCAST_INCLUDE;
						//mld_set_srcfilter(if_rec, ifindex );
						mld_set_if_srcfilter( if_rec,p->grp_addr,ifindex );
					}else{
						MLDV2LOG("%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
						printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
						//delete this group record
						del_user(p,src_tmp,ifindex);
						if(if_rec->mrt_state)
						{
							if(p->if_count==0){
								printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
								mld_del_mr( p->grp_addr, src_0, ifindex);
							}
							p->mrt_state=0;
							if_rec->mrt_state = 0;
						}
						printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
						if(p->if_count==0){
							mld_del_group( p->grp_addr, ifindex );
							del_mcft( p->grp_addr, ifindex );
						}
					}
				}
				break;
			default:
				break;
			}
			if_rec=if_next;
			}
			}
			#endif
		}

		p = next;
	}

#endif	
}

void handle_interface_src_timer(void)
{
#if 1
	struct mcft_entry *p,*next;
	//struct if_rec_entry *if_rec,*if_next;

	struct in6_addr src_0;
	struct in6_addr src_tmp;
	memset(&src_tmp, 0,sizeof(struct in6_addr));
	memset(&src_0, 0,sizeof(struct in6_addr));
	p = mcpq;
	next = NULL;
	int ifindex;
	while( p!=NULL )
	{
		struct ifrec_entry *if_rec,*if_next;
		struct src_entry *s, *src_next;
		/*printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
		p->grp_addr.s6_addr16[ 0],p->grp_addr.s6_addr16[ 1],p->grp_addr.s6_addr16[ 2],p->grp_addr.s6_addr16[ 3],
		p->grp_addr.s6_addr16[ 4],p->grp_addr.s6_addr16[ 5],p->grp_addr.s6_addr16[ 6],p->grp_addr.s6_addr16[ 7]);
		*/	
		int change_sf=0;
		
		next = p->next;

		if_rec=p->iflist;
		if_next=NULL;
		while(if_rec!=NULL){
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		if_next=if_rec->next;
		if(if_rec->timer.lefttime){
			if_rec->timer.lefttime--;
			if((if_rec->timer.lefttime==0)&&(if_rec->timer.retry_left!=0)){
				if_rec->timer.retry_left--;
				if(if_rec->timer.retry_left)
				{
					printf("%s> interface timeout, send query!\n",__FUNCTION__);
					if_rec->timer.lefttime =LAST_MEMBER_QUERY_INTERVAL;
					mldv2_query( p, 1, &s->srcaddr );
				}
			}
		s=if_rec->srclist;

		src_next = NULL;
		while( s )
		{
			src_next = s->next;
			printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
			if( s->timer.lefttime )
			{
				s->timer.lefttime--;
				if( (s->timer.lefttime==0) && (s->timer.retry_left!=0) )
				{
					s->timer.retry_left--;
					if( s->timer.retry_left )
					{
						MLDV2LOG("%s> SRC TIMEOUT, send Query to group:%s", __FUNCTION__, inet_ntoa(p->grp_addr) );
						MLDV2LOG(", src:%s\n", __FUNCTION__, inet_ntoa(s->srcaddr) );
						
						s->timer.lefttime = LAST_MEMBER_QUERY_INTERVAL;
						//igmpv3_query( p, 1, &s->srcaddr );
						mldv2_query( p, 1, &s->srcaddr );
					}
				}
				
				int ifindex= if_rec->ifindex;	
				switch( if_rec->filter_mode )
				{
				case MCAST_INCLUDE:
					if( s->timer.lefttime )
					{
						//forward this src. delete default mfc
						if(if_rec->mrt_state)
						{	
							//µÈ¼ÛÓÚinterface receive src_0 µÄleave report
							mld_update_mr(p->grp_addr, src_0,1, ifindex);
							/*
							MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
							del_user(if_rec,s->srcaddr);
							if (if_rec->user_count==0)
								del_if(if_rec, ifindex);
							if (p->if_count==0){
								mld_del_mr( p->grp_addr, src_0, ifindex);
							}
							*/
							if_rec->mrt_state=0;
						}
					}else{
						MLDV2LOG("%s> remove src:%s", __FUNCTION__, inet_ntoa(s->srcaddr) );
						MLDV2LOG(" from group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
						
						//==0, stop this src
						del_user(if_rec,s->srcaddr);
						if (if_rec->user_count==0){
							del_if(if_rec, ifindex);
							if(p->if_count==0){
								mld_del_mr( p->grp_addr, s->srcaddr, ifindex );
								del_from_grp_srclist(p,s->srcaddr);
							}else{
								mld_update_mr(p->grp_addr, s->srcaddr,1,ifindex);
							}
							del_from_if_srclist( if_rec, s->srcaddr );
							//NO MORE SOURCE, DELETE GROUP RECORD
							change_sf=1;
						}
						
					}
					break;
				case MCAST_EXCLUDE:
					if( s->timer.lefttime )
					{
						#if 0
						//should not delete default mfc
						//forward this src
						if(p->mrt_state)
						{
							MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
							del_user(if_rec,s->srcaddr);
							if (if_rec->user_count==0)
							if(p->if_count==0){
								mld_del_mr( p->grp_addr, src_0, ifindex );
							}
							p->mrt_state=0;
						}
						#endif
					}else{
						MLDV2LOG("%s> stop forwarding src:%s", __FUNCTION__, inet_ntoa(s->srcaddr) );
						MLDV2LOG(" for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						//==0, stop this src, do not remove record
						mld_add_mr( p->grp_addr, s->srcaddr, 0, ifindex );
						change_sf=1;
					}
					break;
				default:
					break;
				}
			}				
			s = src_next;
			}


		//set the new state
		if(change_sf){	
			printf("[%s]:[%d]\n",__FUNCTION__,__LINE__);
			//mld_set_srcfilter( if_rec, ifindex );
			mld_set_if_srcfilter( if_rec,p->grp_addr, ifindex );
			//update group state
			mld_set_grp_srcfilter(p,ifindex);
		}
		//EX( {}, X )
		if( (if_rec->filter_mode==MCAST_EXCLUDE) && (if_rec->srclist!=NULL) )
		{
			int allsrcinex=1;
			s = if_rec->srclist;
			while(s)
			{
				if(s->timer.lefttime>0)
				{
					allsrcinex=0;
					break;
				}
				s=s->next;
			}
			if(allsrcinex==1)
			{
				if( if_rec->mrt_state==0 )
				{
					MLDV2LOG("%s> forward all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					mld_add_mr( p->grp_addr, src_0, 1, ifindex );
					if_rec->mrt_state = 1;
				}
			}
		}
		
		//for empty condition
		if( if_rec->srclist==NULL )
		{
			//printf("for empty condition.if_rec->filter_mode:%d[%s]:[%d]\n",if_rec->filter_mode,__FUNCTION__,__LINE__);
			switch( if_rec->filter_mode )
			{
			case MCAST_INCLUDE:
				//not foreward all source
				//delete this group record
				if(if_rec->mrt_state)
				{
					MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
					printf("stop all sources for group![%s]:[%d].\n",__FUNCTION__,__LINE__);
					//del_user(if_rec,src_tmp);
					del_if(p,ifindex);
					if (p->if_count==0)
					{
						printf("if_count=0,del mfc![%s]\n",__FUNCTION__);
						mld_del_mr( p->grp_addr, src_0, ifindex );
						MLDV2LOG("%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
						printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
						mld_del_group( p->grp_addr, ifindex );
						del_mcft( p->grp_addr, ifindex );
					}
					else{
						printf("if_count!=0,update mfc![%s]\n",__FUNCTION__);
						mld_update_mr( p->grp_addr, src_0,1, ifindex );
						//update grp state
						//mld_set_grp_srcfilter( p, ifindex);
						//if_rec->mrt_state=0;
					}
					//p->mrt_state=0;
					//if_rec->mrt_state=0;
				}
				/*
				MLDV2LOG("%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
				printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				mld_del_group( p->grp_addr, ifindex );
				del_mcft( p->grp_addr, ifindex );
				*/
				break;
			case MCAST_EXCLUDE:
				//printf("forward all sources for group![%s]:[%d].\n",__FUNCTION__,__LINE__);
				//forward all source
				
				if( if_rec->mrt_state==0 )
				{
					int ifindex= if_rec->ifindex;
					MLDV2LOG("%s> forward all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
					printf("forward all sources for group![%s]:[%d].\n",__FUNCTION__,__LINE__);
					mld_add_mr( p->grp_addr, src_0, 1, ifindex );
					if_rec->mrt_state = 1;
					//p->mrt_state = 1;
				}
				
				break;
			default:
				break;
			}			
		}
		}
		if_rec = if_next;
		
		}
		p = next;
	}
#endif	

}
void handle_src_timer(void)
{
#if 0
	struct mcft_entry *p,*next;
	//struct if_rec_entry *if_rec,*if_next;

	struct in6_addr src_0;
	struct in6_addr src_tmp;
	memset(&src_tmp, 0,sizeof(struct in6_addr));
	memset(&src_0, 0,sizeof(struct in6_addr));
	p = mcpq;
	next = NULL;
	int ifindex;
	while( p!=NULL )
	{
		struct ifrec_entry *if_rec,*if_next;
		struct src_entry *s, *src_next;
		/*printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
		p->grp_addr.s6_addr16[ 0],p->grp_addr.s6_addr16[ 1],p->grp_addr.s6_addr16[ 2],p->grp_addr.s6_addr16[ 3],
		p->grp_addr.s6_addr16[ 4],p->grp_addr.s6_addr16[ 5],p->grp_addr.s6_addr16[ 6],p->grp_addr.s6_addr16[ 7]);
		*/	
		int	change_sf=0;
		
		next = p->next;

		if_rec=p->iflist;
		if_next=NULL;
		while(if_rec!=NULL){
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		if_next=if_rec->next;
#if defined IPV6_READY_LOGOV2
		s=if_rec->srclist;
#else
		s = p->srclist;
#endif
		src_next = NULL;
		while( s )
		{
			src_next = s->next;
			printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
			if( s->timer.lefttime )
			{
				s->timer.lefttime--;
				if( (s->timer.lefttime==0) && (s->timer.retry_left!=0) )
				{
					s->timer.retry_left--;
					if( s->timer.retry_left )
					{
						MLDV2LOG("%s> SRC TIMEOUT, send Query to group:%s", __FUNCTION__, inet_ntoa(p->grp_addr) );
						MLDV2LOG(", src:%s\n", __FUNCTION__, inet_ntoa(s->srcaddr) );
						
						s->timer.lefttime = LAST_MEMBER_QUERY_INTERVAL;
						//igmpv3_query( p, 1, &s->srcaddr );
						mldv2_query( p, 1, &s->srcaddr );
					}
				}
				
				int ifindex= if_rec->ifindex;	
				switch( if_rec->filter_mode )
				{
				case MCAST_INCLUDE:
					if( s->timer.lefttime )
					{
						//forward this src
						if(if_rec->mrt_state)
						{
							
							MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
							del_user(p,src_tmp,ifindex);
							if (p->if_count==0){
								mld_del_mr( p->grp_addr, src_0, ifindex);
							}
							if_rec->mrt_state=0;
						}
					}else{
						MLDV2LOG("%s> remove src:%s", __FUNCTION__, inet_ntoa(s->srcaddr) );
						MLDV2LOG(" from group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
						
						//==0, stop this src
						del_user(p,src_tmp,ifindex);
						if(p->if_count==0){
							mld_del_mr( p->grp_addr, s->srcaddr, ifindex );
						}
						del_from_if_srclist( if_rec, s->srcaddr );
						//NO MORE SOURCE, DELETE GROUP RECORD
						change_sf=1;
					}
					break;
				case MCAST_EXCLUDE:
					if( s->timer.lefttime )
					{
						//forward this src
						if(p->mrt_state)
						{
							MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
							del_user(p,src_tmp,ifindex);
							if(p->if_count==0){
								mld_del_mr( p->grp_addr, src_0, ifindex );
							}
							p->mrt_state=0;
						}
					}else{
						MLDV2LOG("%s> stop forwarding src:%s", __FUNCTION__, inet_ntoa(s->srcaddr) );
						MLDV2LOG(" for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						//==0, stop this src, do not remove record
						mld_add_mr( p->grp_addr, s->srcaddr, 0, ifindex );
						change_sf=1;
					}
					break;
				default:
					break;
				}
			}				
			s = src_next;
			}


		//set the new state
		if(change_sf){	
			printf("[%s]:[%d]\n",__FUNCTION__,__LINE__);
			//mld_set_srcfilter( if_rec, ifindex );
			mld_set_if_srcfilter( if_rec,p->grp_addr, ifindex );
		}
		//EX( {}, X )
		if( (if_rec->filter_mode==MCAST_EXCLUDE) && (if_rec->srclist!=NULL) )
		{
			int allsrcinex=1;
			s = if_rec->srclist;
			while(s)
			{
				if(s->timer.lefttime>0)
				{
					allsrcinex=0;
					break;
				}
				s=s->next;
			}
			if(allsrcinex==1)
			{
				if( p->mrt_state==0 )
				{
					MLDV2LOG("%s> forward all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					mld_add_mr( p->grp_addr, src_0, 1, ifindex );
					p->mrt_state = 1;
				}
			}
		}
		
		//for empty condition
		if( if_rec->srclist==NULL )
		{
			//printf("for empty condition.if_rec->filter_mode:%d[%s]:[%d]\n",if_rec->filter_mode,__FUNCTION__,__LINE__);
			switch( if_rec->filter_mode )
			{
			case MCAST_INCLUDE:
				//not foreward all source
				//delete this group record
				if(if_rec->mrt_state)
				{
					MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
					printf("stop all sources for group![%s]:[%d].\n",__FUNCTION__,__LINE__);
					del_user(p,src_tmp,ifindex);
					if (p->if_count==0)
					{
						printf("if_count=0,del mfc![%s]\n",__FUNCTION__);
						mld_del_mr( p->grp_addr, src_0, ifindex );
					}
					else{
						printf("if_count!=0,update mfc![%s]\n",__FUNCTION__);
						mld_update_mr( p->grp_addr, src_0,1, ifindex );
					}
					p->mrt_state=0;
					if_rec->mrt_state=0;
				}
				MLDV2LOG("%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
				printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				mld_del_group( p->grp_addr, ifindex );
				del_mcft( p->grp_addr, ifindex );
				break;
			case MCAST_EXCLUDE:
				//printf("forward all sources for group![%s]:[%d].\n",__FUNCTION__,__LINE__);
				//forward all source
				
				if( if_rec->mrt_state==0 )
				{
					int ifindex= if_rec->ifindex;
					MLDV2LOG("%s> forward all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
					printf("forward all sources for group![%s]:[%d].\n",__FUNCTION__,__LINE__);
					mld_add_mr( p->grp_addr, src_0, 1, ifindex );
					if_rec->mrt_state = 1;
					p->mrt_state = 1;
				}
				
				break;
			default:
				break;
			}			
		}
		if_rec = if_next;
		}
		p = next;
	}
#endif	
}

#else

void handle_group_timer(void)
{
#if 1
	struct mcft_entry *p,*next;
	struct in6_addr src;
	memset(&src,0,sizeof(struct in6_addr));
	p = mcpq;
	next = NULL;
	int ifindex;
	while( p!=NULL )
	{
		next = p->next;
		
		if( p->timer.lefttime )
		{
			p->timer.lefttime--;
			if( (p->timer.lefttime==0) && (p->timer.retry_left!=0) )
			{
				p->timer.retry_left--;
				if( p->timer.retry_left )
				{
					MLDV2LOG("%s> GROUP TIMEOUT, send Query to group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					p->timer.lefttime = LAST_MEMBER_QUERY_INTERVAL;
					//if( p->mld_ver==MLD_VER_2)
					//	igmpv3_query( p, 0, NULL );
					//else
						printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
						printf("GROUP TIMEOUT, send Query![%s]:[%d]\n",__FUNCTION__,__LINE__);
						mld_query(IS_MLD_ALL_HOSTS_ADDRESS, p->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
				}
				
			}
			
			switch( p->filter_mode )
			{
			case MCAST_INCLUDE:
				break;
			case MCAST_EXCLUDE:
				if( p->timer.lefttime==0 )
				{
					struct src_entry *s, *s_next;
					
					MLDV2LOG("%s> group:%s is timeout(EXCLUDE mode)\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
					
					s=p->srclist;
					while(s)
					{
						s_next=s->next;
						if( s->timer.lefttime==0 )
						{
							//remove this source
							mld_del_mr( p->grp_addr, s->srcaddr, ifindex );
							del_from_srclist( p, s->srcaddr );
						}
						s=s_next;
					}
					
					if( p->srclist )
					{
						MLDV2LOG("%s> group:%s changes to INCLUDE mode\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						p->filter_mode=MCAST_INCLUDE;
						mld_set_srcfilter( p, ifindex );
					}else{
						MLDV2LOG("%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						//delete this group record
						if(p->mrt_state)
						{
							mld_del_mr( p->grp_addr, src, ifindex);
							p->mrt_state=0;
						}
						printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
						mld_del_group( p->grp_addr, ifindex );
						del_mcft( p->grp_addr, ifindex );
					}
				}
				break;
			default:
				break;
			}
			
		}

		p = next;
	}
#endif
}

void handle_src_timer(void)
{
	struct mcft_entry *p,*next;
	struct in6_addr src_0;
	memset(&src_0, 0,sizeof(struct in6_addr));
	p = mcpq;
	next = NULL;
	int ifindex;
	while( p!=NULL )
	{
		struct src_entry *s, *src_next;

		int	change_sf=0;
		
		next = p->next;
#if defined IPV6_READY_LOGOV2
		struct ifrec_entry *if_rec,if_next;
		if_rec=p->iflist;
		if_next=if_rec->next;
		while(if_rec!=NULL){
#endif

#if defined IPV6_READY_LOGOV2
		s=if_rec->srclist;
#else
		s = p->srclist;
#endif
		src_next = NULL;
		while( s )
		{
			src_next = s->next;
			
			if( s->timer.lefttime )
			{
				s->timer.lefttime--;
				if( (s->timer.lefttime==0) && (s->timer.retry_left!=0) )
				{
					s->timer.retry_left--;
					if( s->timer.retry_left )
					{
						MLDV2LOG("%s> SRC TIMEOUT, send Query to group:%s", __FUNCTION__, inet_ntoa(p->grp_addr) );
						MLDV2LOG(", src:%s\n", __FUNCTION__, inet_ntoa(s->srcaddr) );
						
						s->timer.lefttime = LAST_MEMBER_QUERY_INTERVAL;
						//igmpv3_query( p, 1, &s->srcaddr );
						mldv2_query( p, 1, &s->srcaddr );
					}
				}
				
					
				switch( p->filter_mode )
				{
				case MCAST_INCLUDE:
					if( s->timer.lefttime )
					{
						//forward this src
						if(p->mrt_state)
						{
							
							MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
							mld_del_mr( p->grp_addr, src_0, ifindex);
							p->mrt_state=0;
						}
					}else{
						MLDV2LOG("%s> remove src:%s", __FUNCTION__, inet_ntoa(s->srcaddr) );
						MLDV2LOG(" from group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
						
						//==0, stop this src
						mld_del_mr( p->grp_addr, s->srcaddr, ifindex );
						del_from_srclist( p, s->srcaddr );
						//NO MORE SOURCE, DELETE GROUP RECORD
						change_sf=1;
					}
					break;
				case MCAST_EXCLUDE:
					if( s->timer.lefttime )
					{
						//forward this src
						if(p->mrt_state)
						{
							MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
							mld_del_mr( p->grp_addr, src_0, ifindex );
							p->mrt_state=0;
						}
					}else{
						MLDV2LOG("%s> stop forwarding src:%s", __FUNCTION__, inet_ntoa(s->srcaddr) );
						MLDV2LOG(" for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

						//==0, stop this src, do not remove record
						mld_add_mr( p->grp_addr, s->srcaddr, 0, ifindex );
						change_sf=1;
					}
					break;
				default:
					break;
				}
			}				
			s = src_next;
		}
#if defined IPV6_READY_LOGOV2
	}
#endif
		//set the new state
		if(change_sf)	
			mld_set_srcfilter( p, ifindex );
		
		//EX( {}, X )
		if( (p->filter_mode==MCAST_EXCLUDE) && (p->srclist!=NULL) )
		{
			int allsrcinex=1;
			s = p->srclist;
			while(s)
			{
				if(s->timer.lefttime>0)
				{
					allsrcinex=0;
					break;
				}
				s=s->next;
			}
			if(allsrcinex==1)
			{
				if( p->mrt_state==0 )
				{
					MLDV2LOG("%s> forward all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					mld_add_mr( p->grp_addr, src_0, 1, ifindex );
					p->mrt_state = 1;
				}
			}
		}
		
		//for empty condition
		if( p->srclist==NULL )
		{
			switch( p->filter_mode )
			{
			case MCAST_INCLUDE:
				//not foreward all source
				//delete this group record
				if(p->mrt_state)
				{
					MLDV2LOG("%s> stop all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					mld_del_mr( p->grp_addr, src_0, ifindex );
					p->mrt_state=0;
				}
				MLDV2LOG("%s> remove group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );
				printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				mld_del_group( p->grp_addr, ifindex );
				del_mcft( p->grp_addr, ifindex );
				break;
			case MCAST_EXCLUDE:
				//forward all source
				if( p->mrt_state==0 )
				{
					MLDV2LOG("%s> forward all sources for group:%s\n", __FUNCTION__, inet_ntoa(p->grp_addr) );

					mld_add_mr( p->grp_addr, src_0, 1, ifindex );
					p->mrt_state = 1;
				}
				break;
			default:
				break;
			}			
		}
		
		p = next;
	}
}
#endif


static struct timeval start_time={0,0};
static int init_stat_time=0;
void mldv2_timer(void)
{
	struct timeval cur_time;
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	if( init_stat_time==0 )
	{
		gettimeofday( &start_time, NULL );
		init_stat_time = 1;
		return;
	}


	
	gettimeofday( &cur_time, NULL );
	if( (cur_time.tv_sec > (start_time.tv_sec+1)) ||
	    ((cur_time.tv_sec == (start_time.tv_sec+1)) && (cur_time.tv_usec >=start_time.tv_usec)) )
	{
		//suppose 1 second passed
		//printf( "." );fflush(NULL);
		#if 0		
		handle_group_timer();
		handle_src_timer();		
		#endif
		handle_interface_timer();
		handle_interface_src_timer();
		start_time.tv_sec = cur_time.tv_sec;
		start_time.tv_usec = cur_time.tv_usec;
	}

	return;
}





//int mldv2_accept(int recvlen, struct IfDesc *dp)
int mldv2_accept(int recvlen)

{
	//struct in6_addr src, dst, group;
	struct in6_addr *group, *dst = NULL;
	struct iphdr *ip;
	//struct igmphdr *igmp;
	int ipdatalen, iphdrlen;
	struct mcft_entry *mymcp;
	struct ifrec_entry *myif;
#if defined IPV6_READY_LOGOV2
	struct ifrec_entry *myiflist;
#endif
	struct mld2_report *mldv2;
	struct mld_hdr *mldh;
	int type;
	/*if (recvlen < sizeof(struct mld2_report)) 
	{
		log(LOG_WARNING, 0, "received packet too short (%u bytes) for mld2_report", recvlen);
		return 0;
	}
	*/
	struct cmsghdr *cm;
	struct in6_pktinfo *pi = NULL;
	int *hlimp = NULL;
	int ifindex = 0;
	struct sockaddr_in6 *src = (struct sockaddr_in6 *) rcvmh.msg_name;
	
	if (recvlen < sizeof(struct mld_hdr))
	{
		log(LOG_WARNING, 0,
		    "received packet too short (%u bytes) for MLD header",
		    recvlen);
		printf("recvlen:%d,sizelimit:%d",recvlen,sizeof(struct mld_hdr));
		return 0;
	}
	mldh = (struct mld_hdr *) rcvmh.msg_iov[0].iov_base;
	/*
	 * Packets sent up from kernel to daemon have ICMPv6 type = 0.
	 * Note that we set filters on the mld6_socket, so we should never
	 * see a "normal" ICMPv6 packet with type 0 of ICMPv6 type.
	 */
 	if(!IS_MLD_TYPE(mldh->mld_icmp6_hdr.icmp6_type)){
		printf("IS NOT MLD TYPE!\n");
		return 0;
	}
	 #if 0
	if ((int)mldh->mld_icmp6_hdr.icmp6_type==134||(int)mldh->mld_icmp6_hdr.icmp6_type==135)
	{
		printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		return 0;
	}
	#endif
	/* extract optional information via Advanced API */
	for (cm = (struct cmsghdr *) CMSG_FIRSTHDR(&rcvmh);
	     cm;
	     cm = (struct cmsghdr *) CMSG_NXTHDR(&rcvmh, cm))
	{
		if (cm->cmsg_level == IPPROTO_IPV6 &&
		    cm->cmsg_type == IPV6_PKTINFO &&
		    cm->cmsg_len == CMSG_LEN(sizeof(struct in6_pktinfo)))
		{
			//printf("get information via Advanced API[%s]:[%d].\n",__FUNCTION__,__LINE__);	
			pi = (struct in6_pktinfo *) (CMSG_DATA(cm));
			ifindex = pi->ipi6_ifindex;
			dst = &pi->ipi6_addr;
		}
		if (cm->cmsg_level == IPPROTO_IPV6 &&
		    cm->cmsg_type == IPV6_HOPLIMIT &&
		    cm->cmsg_len == CMSG_LEN(sizeof(int)))
			hlimp = (int *) CMSG_DATA(cm);
	}
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);	
	#if 0
	if (!IS_IPV6_MULTICAST_ADDRESS(dst->s6_addr)){
			printf("NOT MULTICAST1[%s]:[%d].\n",__FUNCTION__,__LINE__);
			return -1; 
		
}
	/* source address check */
	if (!IS_IPV6_LINKLOCAL_ADDRESS(src->sin6_addr.s6_addr32)) {
		/*
		 * RFC3590 allows the IPv6 unspecified address as the source
		 * address of MLD report and done messages.  However, as this
		 * same document says, this special rule is for snooping
		 * switches and the RFC requires routers to discard MLD packets
		 * with the unspecified source address.
		 */
		
		printf("NOT link local address!\n");
		return -1;
	}
	
	#endif
	type=(int)mldh->mld_icmp6_hdr.icmp6_type;
	
		
	printf("mldh->mld_type:%d,ifindex:%d\n",type,ifindex);
	//for test ÔÝÊ±ºöÂÔ³ýv2 reportÖ®ÍâµÄmld packet!
	#if 0
	if (type!=ICMPV6_MLD2_REPORT){
		printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);	
		return 0;
	}
	#endif	
	if (hlimp == NULL)
	{
		log(LOG_WARNING, 0,
		    "failed to get receiving hop limit");
		return 0;
	}
	//groupµÄÈ·¶¨
	//group = &mldv2->mld2r_grec[0].grec_mca;
	/*
	group = &mldh->mld_addr;
	printf( "srcaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	src->sin6_addr.s6_addr16[ 0],src->sin6_addr.s6_addr16[ 1],src->sin6_addr.s6_addr16[ 2],src->sin6_addr.s6_addr16[ 3],
	src->sin6_addr.s6_addr16[ 4],src->sin6_addr.s6_addr16[ 5],src->sin6_addr.s6_addr16[ 6],src->sin6_addr.s6_addr16[ 7]);
	printf( "dstaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	dst->s6_addr16[ 0],dst->s6_addr16[ 1],dst->s6_addr16[ 2],dst->s6_addr16[ 3],
	dst->s6_addr16[ 4],dst->s6_addr16[ 5],dst->s6_addr16[ 6],dst->s6_addr16[ 7]);

	printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	group->s6_addr16[ 0],group->s6_addr16[ 1],group->s6_addr16[ 2],group->s6_addr16[ 3],
	group->s6_addr16[ 4],group->s6_addr16[ 5],group->s6_addr16[ 6],group->s6_addr16[ 7]);
	*/
	#if 0
	ip  = (struct iphdr *)recv_buf;
	src = ip->saddr;
	dst = ip->daddr;
	
	if(!IN_MULTICAST(dst))	/* It isn't a multicast */
		return -1; 
	if(chk_local(src)) 	/* It's our report looped back */
		return -1;
	if(dst == ALL_PRINTER)	/* It's MS-Windows UPNP all printers notify */
		return -1;
		
	//pkt_debug(recv_buf);
	
	iphdrlen  = ip->ihl << 2;
	ipdatalen = ip->tot_len;
	
	igmp    = (struct igmphdr *)(recv_buf + iphdrlen);
	group   = igmp->group;
	#endif
	/* determine message type */
	//MLDV2LOG("\n%s> receive IGMP type [%x] from %s to ", __FUNCTION__, igmp->type, inet_ntoa(ip->saddr));
	//MLDV2LOG("%s\n", inet_ntoa(ip->daddr));
	group = &mldh->mld_addr;
	//if(IN6_IS_ADDR_RESVER_0(group))
	#if 1
	if (IS_IPV6_RESVER_0_ADDRESS(group->s6_addr))
	{
		printf("IS_IPV6_RESVER_0_ADDRESS![%s].\n",__FUNCTION__);
		return 0;
	}
	#endif
	switch (mldh->mld_icmp6_hdr.icmp6_type) {
		#if 1
		case ICMPV6_MGM_QUERY:
			/* Linux Kernel will process local member query, it wont reach here */
			break;
		
		case ICMPV6_MGM_REPORT:

			{
				//MLDV2LOG("%s> REPORT(V1), group:%s\n", __FUNCTION__, inet_ntoa(group) );
				
				printf("Accept ICMPV6_MGM_REPORT.[%s]:[%d]\n",__FUNCTION__,__LINE__);
				group = &mldh->mld_addr;
				printf( "srcaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
				src->sin6_addr.s6_addr16[ 0],src->sin6_addr.s6_addr16[ 1],src->sin6_addr.s6_addr16[ 2],src->sin6_addr.s6_addr16[ 3],
				src->sin6_addr.s6_addr16[ 4],src->sin6_addr.s6_addr16[ 5],src->sin6_addr.s6_addr16[ 6],src->sin6_addr.s6_addr16[ 7]);
				printf( "dstaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
				dst->s6_addr16[ 0],dst->s6_addr16[ 1],dst->s6_addr16[ 2],dst->s6_addr16[ 3],
				dst->s6_addr16[ 4],dst->s6_addr16[ 5],dst->s6_addr16[ 6],dst->s6_addr16[ 7]);

				printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
				group->s6_addr16[ 0],group->s6_addr16[ 1],group->s6_addr16[ 2],group->s6_addr16[ 3],
				group->s6_addr16[ 4],group->s6_addr16[ 5],group->s6_addr16[ 6],group->s6_addr16[ 7]);
				
				if(!chk_mcft(*group)) 
				{
					//new group
					printf("new group![%s]:[%d].\n",__FUNCTION__,__LINE__);
					mymcp = add_mcft(*group, src->sin6_addr,ifindex);
					if(!mymcp){ 
						printf("MYMCP IS NULL!![%s]:[%d].\n",__FUNCTION__,__LINE__);	
						return -1;
					}
					//interface added in add_mcft
					myif = get_ifrec(mymcp,ifindex);
					if(myif==NULL){
						printf("myif is null!![%s].\n",__FUNCTION__);
						return -1;
					}
					myif->mld_ver=1;
					mld_add_group(*group,ifindex);
					//interface timer
					myif->timer.lefttime = MEMBER_QUERY_INTERVAL;
					myif->timer.retry_left = MEMBER_QUERY_COUNT;
					//set the new state of the interface
					myif->filter_mode =  MCAST_EXCLUDE;
					mld_set_if_srcfilter(myif,mymcp->grp_addr,ifindex);
					
					//Group Timer=GMI
					mymcp->timer.lefttime = MEMBER_QUERY_INTERVAL;
					mymcp->timer.retry_left = MEMBER_QUERY_COUNT;

					//set the new state of the group
					mymcp->filter_mode = MCAST_EXCLUDE;
					mld_set_grp_srcfilter( mymcp,ifindex );
					//printf("user_count:%d,[%s].\n",mymcp->user_count,__FUNCTION__);
					//handle_mldv2_isex( *group,src->sin6_addr, 0, NULL, ifindex );
				}
				else{
					//group exited already
					printf("group exited already![%s]:[%d].\n",__FUNCTION__,__LINE__);

					int retif=0;
					mymcp = get_mcft(*group);
					if(mymcp){
						myif = get_ifrec(mymcp,ifindex);
						if(myif==NULL){
						//add new interface;
							retif=add_if(mymcp,ifindex);
							
							myif->mld_ver=1;
							mld_add_group(*group,ifindex);
							//interface timer
							myif->timer.lefttime = MEMBER_QUERY_INTERVAL;
							myif->timer.retry_left = MEMBER_QUERY_COUNT;
							//set the new state of the interface
							myif->filter_mode =  MCAST_EXCLUDE;
							mld_set_if_srcfilter(myif,mymcp->grp_addr,ifindex);
						}
						else{
#ifdef KEEP_GROUP_MEMBER
							int retusr;
							retusr=add_user(myif, src->sin6_addr);
							printf("myif->usercount:%d,[%s]:[%d]",myif->user_count,__FUNCTION__,__LINE__);
#endif
							if(retusr==2){
							//add user
							mld_set_if_srcfilter(myif,mymcp->grp_addr,ifindex);
							}
						}
					
						printf("mymcp->ifcount:%d,[%s]:[%d]",mymcp->if_count,__FUNCTION__,__LINE__);
						
					}

					
				}	
				mymcp = get_mcft(*group);
				printf("myiflsit->usercount:%d,mymcp->ifcount:%d,[%s]:[%d]\n",myif->user_count,mymcp->if_count,__FUNCTION__,__LINE__);
				if(mymcp) 
					mymcp->mld_ver = MLD_VER_1;
				
				//Report => IS_EX( {} )	
				handle_mldv2_isex( *group,src->sin6_addr, 0, NULL, ifindex );
				
			}
			break;
			#endif
 		case ICMPV6_MLD2_REPORT:
		     {
			struct mld2_report *mldv2r;
			struct mld2_grec *mldv2grec=NULL;
			unsigned short rec_id;
			mldv2r  =(struct mld2_report *)mldh;
			MLDV2LOG("%s> REPORT(V2)\n", __FUNCTION__ );
			//igmpv3 = (struct igmpv3_report *)igmp;
			printf( "recv ICMPV6_MLD2_REPORT\n" );
			printf( "mldv2->type:0x%d\n", mldv2r->mld2r_type );
			printf( "mldv2->ngrec:0x%d\n", ntohs(mldv2r->mld2r_ngrec) );
		
			rec_id=0;
			//mldv2grec =  &mldv2->mld2r_grec[0];
			mldv2grec =  mldv2r->mld2r_grec;
			if(mldv2grec==NULL){
				printf("mldv2grec==NULL!\n");
			}
			//printf("")
			while( rec_id < ntohs(mldv2r->mld2r_ngrec) )
			{
				int srcnum;
				printf( "mldv2grec[%d]->grec_type:0x%x\n", rec_id, mldv2grec->grec_type );
				printf( "mldv2grec[%d]->grec_auxwords:0x%x\n", rec_id, mldv2grec->grec_auxwords );
				printf( "mldv2grec[%d]->grec_nsrcs:0x%x\n", rec_id, ntohs(mldv2grec->grec_nsrcs) );
				printf( "mldv2grec[%d]->grec_mca:0x%x-%x-%x-%x\n", 
					rec_id, mldv2grec->grec_mca.s6_addr32[0],mldv2grec->grec_mca.s6_addr32[1],mldv2grec->grec_mca.s6_addr32[2],mldv2grec->grec_mca.s6_addr32[3]);
			
				//memcpy(group , &mldv2grec->grec_mca,sizeof(struct in6_addr));
				if(mldv2grec!=NULL){
					printf("mldv2grec!=NULL");
					group=&mldv2grec->grec_mca;
				}
				srcnum = ntohs(mldv2grec->grec_nsrcs);
				printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
					group->s6_addr16[ 0],group->s6_addr16[ 1],group->s6_addr16[ 2],group->s6_addr16[ 3],
					group->s6_addr16[ 4],group->s6_addr16[ 5],group->s6_addr16[ 6],group->s6_addr16[ 7]);

	
				switch( mldv2grec->grec_type )
				{
				case MLD2_MODE_IS_INCLUDE:
					//MLDV2LOG("%s> IS_IN, group:%s, srcnum:%d\n", __FUNCTION__, inet_ntoa(group), srcnum );
					printf("%s> IS_IN, srcnum:%d\n", __FUNCTION__, srcnum );
					handle_mldv2_isin( *group,src->sin6_addr, srcnum, mldv2grec->grec_src,ifindex );
					break;
				case MLD2_MODE_IS_EXCLUDE:
					//MLDV2LOG("%s> IS_EX, group:%s, srcnum:%d\n", __FUNCTION__, inet_ntoa(group), srcnum );
					printf("%s> IS_EX, srcnum:%d\n", __FUNCTION__, srcnum );
					handle_mldv2_isex( *group,src->sin6_addr, srcnum, mldv2grec->grec_src,ifindex );
					break;
				case MLD2_CHANGE_TO_INCLUDE: 
					//MLDV2LOG("%s> TO_IN, group:%s, srcnum:%d\n", __FUNCTION__, inet_ntoa(group), srcnum );
					printf("%s> TO_IN, srcnum:%d\n", __FUNCTION__, srcnum );
					handle_mldv2_toin( *group,src->sin6_addr, srcnum, mldv2grec->grec_src,ifindex );
					break;
				case MLD2_CHANGE_TO_EXCLUDE: 
					//MLDV2LOG("%s> TO_EX, group:%s, srcnum:%d\n", __FUNCTION__, inet_ntoa(group), srcnum );
					printf("%s> TO_EX, srcnum:%d\n", __FUNCTION__, srcnum );
					handle_mldv2_toex( *group,src->sin6_addr, srcnum, mldv2grec->grec_src,ifindex );
					break;
				case MLD2_ALLOW_NEW_SOURCES:
					//MLDV2LOG("%s> ALLOW, group:%s, srcnum:%d\n", __FUNCTION__, inet_ntoa(group), srcnum );
					printf("%s> ALLOW, srcnum:%d\n", __FUNCTION__, srcnum );
					handle_mldv2_allow( *group,src->sin6_addr, srcnum, mldv2grec->grec_src,ifindex );
					break;
				case MLD2_BLOCK_OLD_SOURCES:
					//MLDV2LOG("%s> BLOCK, group:%s, srcnum:%d\n", __FUNCTION__, inet_ntoa(group), srcnum );
					printf("%s>  BLOCK, srcnum:%d\n", __FUNCTION__, srcnum );

					handle_mldv2_block( *group,src->sin6_addr, srcnum, mldv2grec->grec_src,ifindex );
					break;
				default:
					printf("%s>  Unknown Group Record Types .\n", __FUNCTION__);
					//MLDV2LOG("%s> Unknown Group Record Types [%x]\n", __FUNCTION__, mldv2grec->grec_type );
					break;
				}
			
				rec_id++;
				//printf( "count next: 0x%x %d %d %d %d\n", igmpv3grec, sizeof( struct igmpv3_grec ), igmpv3grec->grec_auxwords, ntohs(igmpv3grec->grec_nsrcs), sizeof( __u32 ) );
				mldv2grec = (struct mldv2grec *)( (char*)mldv2grec + sizeof( struct mld2_grec ) + (mldv2grec->grec_auxwords+ntohs(mldv2grec->grec_nsrcs))*sizeof(struct in6_addr ) );
				//printf( "count result: 0x%x\n", igmpv3grec );
			}
			break;
		     }
		case ICMPV6_MGM_REDUCTION :
			//MLDV2LOG("%s> LEAVE(V2), group:%s\n", __FUNCTION__, inet_ntoa(group) );
			MLDV2LOG("%s> LEAVE(V1).\n", __FUNCTION__ );
			printf("LEAVE(V1).\n");
			printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
				group->s6_addr16[ 0],group->s6_addr16[ 1],group->s6_addr16[ 2],group->s6_addr16[ 3],
				group->s6_addr16[ 4],group->s6_addr16[ 5],group->s6_addr16[ 6],group->s6_addr16[ 7]);
			if(chk_mcft(*group))
			{
				//Leave => TO_IN( {} )
				handle_mldv2_toin( *group,src->sin6_addr, 0, NULL, ifindex );
			}
			break;
		default:
			printf("%s> receive other type [%x]\n", __FUNCTION__, mldv2->mld2r_type );
			break;
    }
    return 0;
}


#endif /*CONFIG_MLDV2_SUPPORT*/
