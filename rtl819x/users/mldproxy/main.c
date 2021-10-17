#include "mclab.h"
#include "timeout.h"
#include "mldproxy.h"
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include "built_time"
#include "mld.h"

#define IS_IPV6_RESVER_0_ADDRESS(ipv6addr)	((ipv6addr[1] & 0xf0)==0x00)

#define IPV6_READY_LOGO 1
char *mld6_recv_buf;		/* input packet buffer */
char *mld6_send_buf;		/* output packet buffer */
struct in6_addr IS_MLD_ALL_HOSTS_ADDRESS={{{0xFF,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
struct in6_addr IS_MLD_ALL_ROUTER_ADDRESS={{{0xFF,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}}};

struct msghdr 		sndmh, rcvmh;
struct sockaddr_in6 	from;
struct sockaddr_in6 	to;
struct iovec 		rcviov[2];
struct iovec 		sndiov[2];

int rcvcmsglen;
static u_char   		*rcvcmsgbuf = NULL;


#ifndef USE_RFC2292BIS
#define IP6OPT_RTALERT_LEN 4
u_int8_t raopt[IP6OPT_RTALERT_LEN];
#endif 
#define IP6OPT_RTALERT_MLD      0
#define IP6OPT_RTALERT          0x05
#ifndef IP6OPT_ROUTER_ALERT	/* XXX to be compatible older systems */
#define IP6OPT_ROUTER_ALERT IP6OPT_RTALERT
#endif

char *sndcmsgbuf;
int ctlbuflen = 0;
static u_int16_t rtalert_code;
extern int MLD_Socket;


struct mld_timer	startupQueryTimer;
static struct mld_timer	generalQueryTimer;
static struct mld_timer	linkChangeQueryTimer;


int mld_query(struct in6_addr dst,struct in6_addr grp,__u8 mrt);
struct mld_timer qtimer;

int MLD_rx_enable=0;
void mld_timer_expired(void *arg);
void mld_general_query_timer_expired(void *arg);
void mld_specific_grp_timer_expired(void *arg);
void mld_specific_if_timer_expired(void *arg);

#if 0
static void pkt_debug(const char *buf)
{
int num2print = 20;
int i = 0;
	if(buf[0]==0x46)
		num2print = 24;
	for (i = 0; i < num2print; i++) {
		printf("%2.2x ", 0xff & buf[i]);
	}
	printf("\n");
	num2print = buf[3];
	for (; i < num2print; i++) {
		printf("%2.2x ", 0xff & buf[i]);
	}
	printf("\n");
}
#else
#define pkt_debug(buf)	do {} while (0)
#endif

static int  mld_id = 0;

char mld_down_if_name[IFNAMSIZ];
char mld_down_if_idx;

#ifdef CONFIG_MLDPROXY_MULTIWAN
char mld_up_if_name[MAXWAN][IFNAMSIZ];
#else
char mld_up_if_name[IFNAMSIZ];
#endif
#if defined IPV6_READY_LOGO
char mld_if_name[MAXWAN+1][IFNAMSIZ];
#endif
#ifdef CONFIG_MLDPROXY_MULTIWAN
char mld_up_if_idx[MAXWAN];
#else
char mld_up_if_idx;
#endif
#ifdef CONFIG_MLDPROXY_MULTIWAN
int mld_up_if_num;
#endif
int mld_if_num;
/*
#if defined IPV6_READY_LOGO
char mld_if_name[MAXWAN+1];
#endif
*/
#ifdef USE_STATIC_ENTRY_BUFFER
struct mcft_entry_en {
	int valid;
	struct mcft_entry entry_mcft_;
};
struct ifrec_entry_en {
	int valid;
	struct ifrec_entry entry_ifrec_;
};

struct mbr_entry_en {
	int valid;
	struct mbr_entry entry_mbr_;
};
#ifdef CONFIG_MLDV2_SUPPORT

struct src_entry_en{
	int valid;
	struct src_entry entry_src_;
};
#endif

struct mcft_entry_en mcft_entry_tbl[MAX_MFCT_ENTRY];
struct ifrec_entry_en ifrec_entry_tbl[MAX_IFREC_ENTRY];
struct mbr_entry_en mbr_entry_tbl[MAX_MBR_ENTRY];
#ifdef CONFIG_MLDV2_SUPPORT
struct src_entry_en src_entry_tbl[MAX_SRC_ENTRY];
#endif
#endif




struct mcft_entry *mcpq = NULL;

#ifdef USE_STATIC_ENTRY_BUFFER
struct mcft_entry * find_mcft_entry_from_tbl(void)
{
	int i;
	struct mcft_entry_en *valid_entry;
	
	for(i=0;i<MAX_MFCT_ENTRY;i++){
		valid_entry = &mcft_entry_tbl[i];
		if(valid_entry->valid==0){
			valid_entry->valid=1;
			return (&(valid_entry->entry_mcft_));
		}
	}
	//printf("find_mcft_entry_from_tbl fail\n");
	return 0;
}

int del_mcft_entry_from_tbl(struct mcft_entry *del_mcft_entry)
{
	
	int i;
	struct mcft_entry_en *valid_entry;
	struct mcft_entry *check_entry;
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	for(i=0;i<MAX_MFCT_ENTRY;i++){
		valid_entry = &mcft_entry_tbl[i];
		check_entry = &(valid_entry->entry_mcft_);
		if(&(valid_entry->entry_mcft_)==del_mcft_entry){
			//printf("delmcft entry\n");
			valid_entry->valid=0;
			return 1;
		}
	}
	//printf("del_mcft_entry_from_tbl fail\n");
	return 0;
	
}

struct ifrec_entry * find_ifrec_entry_from_tbl(void)
{
	int i;
	struct ifrec_entry_en *valid_entry;
	
	for(i=0;i<MAX_IFREC_ENTRY;i++){
		valid_entry = &ifrec_entry_tbl[i];
		if(valid_entry->valid==0){
			valid_entry->valid=1;
			return (&(valid_entry->entry_ifrec_));
		}
	}
	//printf("find_ifrec_entry_from_tbl fail\n");
	return 0;
}

int del_ifrec_entry_from_tbl(struct ifrec_entry *del_ifrec_entry)
{
	
	int i;
	struct ifrec_entry_en *valid_entry;
	struct ifrec_entry *check_entry;
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	for(i=0;i<MAX_IFREC_ENTRY;i++){
		valid_entry = &ifrec_entry_tbl[i];
		check_entry = &(valid_entry->entry_ifrec_);
		if(&(valid_entry->entry_ifrec_)==del_ifrec_entry){
			//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
			valid_entry->valid=0;
			return 1;
		}
	}
	//printf("del_ifrec_entry_from_tbl fail\n");
	return 0;
	
}

struct mbr_entry * find_mbr_entry_from_tbl(void)
{
	int i;
	struct mbr_entry_en *valid_entry;
	
	for(i=0;i<MAX_MBR_ENTRY;i++){
		valid_entry = &mbr_entry_tbl[i];
		if(valid_entry->valid==0){
			valid_entry->valid=1;
			//printf("find_mbr_entry_from_tbl success![%s][%d]\n",__FUNCTION__,__LINE__);
			return (&(valid_entry->entry_mbr_));
		}
	}
	//printf("find_mbr_entry_from_tbl fail!!![%s][%d]\n",__FUNCTION__,__LINE__);
	return 0;
}

int del_mbr_entry_from_tbl(struct mbr_entry *del_mbr_entry)
{
	int i;
	struct mbr_entry_en *valid_entry;
	struct mbr_entry *check_entry;
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	for(i=0;i<MAX_MBR_ENTRY;i++){
		valid_entry = &mbr_entry_tbl[i];
		check_entry = &(valid_entry->entry_mbr_);
		if(&(valid_entry->entry_mbr_)==del_mbr_entry){
			//printf("del mbr entry:user_addr\n");
			valid_entry->valid=0;
			return 1;
		}
	}
	//printf("del_mbr_entry_from_tbl fail\n");
	return 0;
}
#ifdef CONFIG_MLDV2_SUPPORT

struct mbr_entry * find_src_entry_from_tbl(void)
{
	int i;
	struct src_entry_en *valid_entry;
	
	for(i=0;i<MAX_SRC_ENTRY;i++){
		valid_entry = &src_entry_tbl[i];
		if(valid_entry->valid==0){
			valid_entry->valid=1;
			//printf("find_mbr_entry_from_tbl success![%s][%d]\n",__FUNCTION__,__LINE__);
			return (&(valid_entry->entry_mbr_));
		}
	}
	//printf("find_mbr_entry_from_tbl fail!!![%s][%d]\n",__FUNCTION__,__LINE__);
	return 0;
}

int del_src_entry_from_tbl(struct src_entry *del_src_entry)
{
	int i;
	struct src_entry_en *valid_entry;
	struct src_entry *check_entry;
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	for(i=0;i<MAX_MBR_ENTRY;i++){
		valid_entry = &src_entry_tbl[i];
		check_entry = &(valid_entry->entry_src_);
		if(&(valid_entry->entry_mbr_)==del_src_entry){
			//printf("del src entry:user_addr\n");
			valid_entry->valid=0;
			return 1;
		}
	}
	//printf("del_mbr_entry_from_tbl fail\n");
	return 0;
}
#endif
#endif
struct mcft_entry * add_mcft(struct in6_addr grp_addr, struct in6_addr src_addr,int ifindex)
{
struct mcft_entry *mcp;
struct ifrec_entry *ifrec;
#ifdef KEEP_GROUP_MEMBER
	struct mbr_entry *gcp;
#endif
#ifndef USE_STATIC_ENTRY_BUFFER 

	mcp = malloc(sizeof(struct mcft_entry));
	if(!mcp)
		return 0;
	ifrec = malloc(sizeof(struct mcft_entry));
	if(!ifrec)
	{
		free(mcp);
		return 0;
	}
#ifdef KEEP_GROUP_MEMBER
	gcp = malloc(sizeof(struct ifrec_entry));
	if (!gcp) {
		free(ifrec);
		free(mcp);
		return 0;
	}
#endif
//static buffer
#else
	mcp =find_mcft_entry_from_tbl();
	if(!mcp){
		return 0;
	}
	ifrec =find_ifrec_entry_from_tbl();
	if(!ifrec){
		del_mcft_entry_from_tbl(mcp);
		return 0;
	}
#ifdef KEEP_GROUP_MEMBER
	gcp = find_mbr_entry_from_tbl();
	if (!gcp) {
		del_ifrec_entry_from_tbl(ifrec);
		del_mcft_entry_from_tbl(mcp);
		return 0;
	}
#endif

#endif
	memcpy(&(mcp->grp_addr),&(grp_addr),sizeof(struct in6_addr));
	memcpy(&(ifrec->grp_addr),&(grp_addr),sizeof(struct in6_addr));
	mcp->if_count = 1;

	ifrec->ifindex = ifindex;
	
	//If_entry->next = NULL;
	//mcp->if_mbr = If_entry;
	
#ifdef KEEP_GROUP_MEMBER
	ifrec->user_count = 1;
	memcpy(&(gcp->user_addr), &(src_addr), sizeof(struct in6_addr));
	/*
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	printf( "src_addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
				src_addr.s6_addr16[ 0],src_addr.s6_addr16[ 1],src_addr.s6_addr16[ 2],src_addr.s6_addr16[ 3],
				src_addr.s6_addr16[ 4],src_addr.s6_addr16[ 5],src_addr.s6_addr16[ 6],src_addr.s6_addr16[ 7]);
	printf( "group_user_addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
				gcp->user_addr.s6_addr16[ 0],gcp->user_addr.s6_addr16[ 1],gcp->user_addr.s6_addr16[ 2],gcp->user_addr.s6_addr16[ 3],
				gcp->user_addr.s6_addr16[ 4],gcp->user_addr.s6_addr16[ 5],gcp->user_addr.s6_addr16[ 6],gcp->user_addr.s6_addr16[ 7]);
	*/
	gcp->next = NULL;
	ifrec->grp_mbr = gcp;
#endif

#ifdef CONFIG_MLDV2_SUPPORT
	ifrec->mld_ver = MLD_VER_2;
	ifrec->filter_mode = MCAST_INCLUDE;
	ifrec->srclist = NULL;
	ifrec->mrt_state=0;	
	ifrec->timer.lefttime=LAST_MEMBER_QUERY_INTERVAL;
	
	mcp->timer.lefttime = LAST_MEMBER_QUERY_INTERVAL;
	mcp->mld_ver = MLD_VER_2;
	mcp->filter_mode = MCAST_INCLUDE;
	mcp->srclist = NULL;
	mcp->mrt_state=0;	
#endif /*CONFIG_MLDV2_SUPPORT*/
	mcp->timer.retry_left = LAST_MEMBER_QUERY_COUNT;
	ifrec->timer.retry_left = LAST_MEMBER_QUERY_COUNT;
	ifrec->next = NULL;
	mcp->iflist = ifrec;
	mcp->next = mcpq;
	mcpq = mcp;
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	return mcp;
}

// Kaohj --- add group timer for mld group-specific query.
int add_mcft_timer(struct in6_addr grp_addr)
{
	struct mcft_entry **q, *p;
	
	/* Remove the entry from the  list. */
	for (p = mcpq; p!=0; p = p->next) {
		if(!memcmp(&(p->grp_addr) ,&(grp_addr),sizeof(grp_addr))) {
#ifdef PERIODICAL_SPECIFIC_QUERY
			p->timer.retry_left = MEMBER_QUERY_COUNT+1;
			timeout(mld_specific_grp_timer_expired , p, MEMBER_QUERY_INTERVAL, &p->timer.ch);
#endif
			return 0;
		}
	}
	return -1;
}
int del_mcft(struct in6_addr grp_addr,int ifindex)
{
struct mcft_entry **q, *p;
struct ifrec_entry *if_q,*if_p;
#ifdef KEEP_GROUP_MEMBER
	struct mbr_entry *gt, *gc;
#endif

	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	/* Remove the entry from the  list. */
	for (q = &mcpq; (p = *q); q = &p->next) {
		if(!memcmp(&(p->grp_addr) , &(grp_addr),(sizeof(struct in6_addr)))) {
			//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
			*q = p->next;
			// Kaohj -- free member list
#ifndef USE_STATIC_ENTRY_BUFFER 
			if(p->iflist!=NULL){
				if_p=p->iflist;
				while(if_p){
					if_q=if_p->next;
		
#ifdef KEEP_GROUP_MEMBER
					gc = if_p->grp_mbr;
					while (gc) {
						gt = gc->next;
						free(gc);
						gc = gt;
					}
#endif
#ifdef CONFIG_MLDV2_SUPPORT
					{
					  struct src_entry *s, *sn;
					  s=if_p->srclist;
					  while(s)
					  {
					  	sn=s->next;
					  	free(s);
					  	s=sn;
					  }
					}
#endif
					untimeout(&if_p->timer.ch);
					free(if_p);
					if_p = if_q;
				}
			}
			untimeout(&p->timer.ch);
			free(p);
			return 0;
#else//static buffer
			if(p->iflist!=NULL){
				if_p = p->iflist;
				while(if_p){
					//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
					if_q=if_p->next;
#ifdef KEEP_GROUP_MEMBER
					if(if_p->grp_mbr!=NULL){
						gc = if_p->grp_mbr;
						while (gc) {
							gt = gc->next;
							//printf("[%s]:[%d]del mcft user is 0x%x-%x-%x-%x\n",__FUNCTION__,__LINE__,
							//gc->useraddr.s6_addr32[0],gc->useraddr.s6_addr32[1],gc->useraddrr.s6_addr32[2],gc->useraddr.s6_addr32[3]);
							del_mbr_entry_from_tbl(gc);
							gc = gt;
						}
					}
#endif

#ifdef CONFIG_MLDV2_SUPPORT
				/*to-do:use static buffer for src list*/
					{
					  struct src_entry *s, *sn;
					  s=if_p->srclist;
					  while(s)
					  {
					  	sn=s->next;
					  	free(s);
					  	s=sn;
					  }
					}
#endif
					untimeout(&if_p->timer.ch);
					del_ifrec_entry_from_tbl(if_p);
					if_p=if_q;
				}
			}
			untimeout(&p->timer.ch);
			del_mcft_entry_from_tbl(p);
			
			return 0;
#endif			
		}
	}
	return -1;
}

// Kaohj --- delete group timer for mld group-specific query.
int del_mcft_timer(struct in6_addr grp_addr)
{
	struct mcft_entry *p;

	/* Remove the entry from the  list. */
	for (p = mcpq; p!=0; p = p->next) {
		if(!memcmp(&(p->grp_addr) , &(grp_addr),sizeof(struct in6_addr))) {
			untimeout(&p->timer.ch);
			return 0;
		}
	}
	return -1;
}

int chk_mcft(struct in6_addr grp_addr)
{
	struct mcft_entry *mcp = mcpq;
	while(mcp) {
		
		if(!memcmp(&(grp_addr),&(mcp->grp_addr),sizeof(struct in6_addr))){
			return 1;
			}	
		mcp = mcp->next;
	}
	return 0;
}

struct mcft_entry * get_mcft(struct in6_addr grp_addr)
{
	struct mcft_entry *mcp = mcpq;
	while(mcp) {
		if(!memcmp(&(mcp->grp_addr) , &(grp_addr),sizeof(struct in6_addr))){
			return mcp;
			}
		mcp = mcp->next;
	}
	return NULL;
}
struct ifrec_entry * get_ifrec(struct mcft_entry *mcp,int ifindex)
{
	struct ifrec_entry *if_rec=mcp->iflist;
	while(if_rec) {
		if(if_rec->ifindex==ifindex){
			//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
			return if_rec;
			}
		if_rec = if_rec->next;
	}
	return NULL;
}
int num_mcft(void)
{
	struct mcft_entry *mcp = mcpq;
	int n = 0;
		while(mcp) {
			n++;
			mcp = mcp->next;
		}
	return n;
}

//add if
//	0: fail
//	1: duplicate interface
//	2: added new interfacesuccessfully

int add_if(struct mcft_entry *mcp, struct in6_addr src, int ifindex)
{
	//struct mbr_entry *gcp_user,*gcp_if;
	struct ifrec_entry *if_rec;
	//printf("ifindex:%d.[%s]:[%d].\n",ifindex,__FUNCTION__,__LINE__);
	
	//check if
	if_rec = mcp->iflist;
	while (if_rec) {
		if (if_rec->ifindex==ifindex){
			//printf("if exists!mcp->ifcount:%d!!![%s]:[%d].\n",mcp->if_count,__FUNCTION__,__LINE__);
			return 1;	
			}
		if_rec = if_rec->next;
	}
	
#ifndef USE_STATIC_ENTRY_BUFFER 	
	// add if
	if_rec = malloc(sizeof(struct ifrec_entry));
#else
	if_rec = find_ifrec_entry_from_tbl();
#endif	
	if (!if_rec) {
		return 0;
	}
	memcpy(&(if_rec->grp_addr),&(mcp->grp_addr),sizeof(struct in6_addr));
	if_rec->ifindex = ifindex;
	if_rec->next = mcp->iflist;
	if_rec->grp_mbr = NULL;
	
#ifdef KEEP_GROUP_MEMBER
	int ret;
	ret=add_user(if_rec, src);
#endif
	if_rec->user_count=1;

	mcp->iflist= if_rec;
	mcp->if_count++;
#if defined CONFIG_MLDV2_SUPPORT
	if_rec->filter_mode=MCAST_INCLUDE;
	if_rec->srclist=NULL;
	if_rec->mrt_state=0;
	if_rec->mld_ver=MLD_VER_2;
	if_rec->timer.lefttime=LAST_MEMBER_QUERY_INTERVAL;
#endif
	if_rec->timer.retry_left = LAST_MEMBER_QUERY_COUNT;
	
	mcp->iflist = if_rec;
	
	//printf("Add new interface!!!mcp->if_count:%d.[%s].\n",mcp->if_count,__FUNCTION__);
	return 2;		//return value:added successfully
}

// remove if from group member list
// return: if count
int del_if(struct mcft_entry *mcp,  int ifindex)
{
	struct ifrec_entry **q, *p;
	struct mbr_entry *gc,*gt;
	//struct if_rec_entry **if_recq,*if_recp;
	/* Remove the entry from the  list. */
	if (mcp->if_count ==0){
		//printf("No interface joined the group!!!\n");
		return 0;
	}
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	q = &mcp->iflist;
	p = *q;

	while (p) {
		if(p->ifindex==ifindex) {
			*q = p->next;
#ifndef USE_STATIC_ENTRY_BUFFER 
#ifdef KEEP_GROUP_MEMBER
			gc = p->grp_mbr;
			while (gc) {
				gt = gc->next;
				free(gc);
				gc = gt;
			}
#endif
#ifdef CONFIG_MLDV2_SUPPORT
			{
			  struct src_entry *s, *sn;
			  s=p->srclist;
			  while(s)
			  {
				sn=s->next;
				free(s);
				s=sn;
			  }
			}
#endif

			untimeout(&p->timer.ch);
			free(p);
#else
			
#ifdef KEEP_GROUP_MEMBER
			gc = p->grp_mbr;
			while (gc) {
				gt = gc->next;
				//printf("[%s]:[%d]del mcft user is 0x%x-%x-%x-%x\n",__FUNCTION__,__LINE__,
		//gc->useraddr.s6_addr32[0],gc->useraddr.s6_addr32[1],gc->useraddrr.s6_addr32[2],gc->useraddr.s6_addr32[3]);
				del_mbr_entry_from_tbl(gc);
				gc = gt;
			}
#endif
			
#ifdef CONFIG_MLDV2_SUPPORT
			{
			  struct src_entry *s, *sn;
			  s=p->srclist;
			  while(s)
			  {
				sn=s->next;
				free(s);
				s=sn;
			  }
			}
#endif
			untimeout(&p->timer.ch);
			del_ifrec_entry_from_tbl(p);
#endif			
			mcp->if_count--;
			return mcp->if_count;
		}
		q = &p->next;
		p = p->next;
	}
	
	return mcp->if_count;
}



#ifdef KEEP_GROUP_MEMBER
// Kaohj -- attach user to group member list
//	0: fail
//	1: duplicate user
//	2: added successfully

int add_user(struct ifrec_entry *ifrec, struct in6_addr src)
{
	struct mbr_entry *gcp;
	// check user
	gcp = ifrec->grp_mbr;
	while (gcp) {
		if(!memcmp(&(gcp->user_addr) ,&src,sizeof(struct in6_addr)))
			return 1;	// user exists
		gcp = gcp->next;
	}
#ifndef USE_STATIC_ENTRY_BUFFER 	
	// add user
	gcp = malloc(sizeof(struct mbr_entry));
#else
	gcp = find_mbr_entry_from_tbl();
#endif	
	if (!gcp) {
		return 0;
	}
	
	memcpy(&(gcp->user_addr),&src,sizeof(struct in6_addr));
	gcp->next = ifrec->grp_mbr;
	ifrec->grp_mbr = gcp;
	ifrec->user_count++;

	return 2;		//return value:added successfully
}

// Kaohj -- remove user from group member list
// return: user count
int del_user(struct ifrec_entry *ifrec, struct in6_addr src)
{
	struct mbr_entry **q, *p;
	
	/* Remove the entry from the  list. */
	q = &ifrec->grp_mbr;
	p = *q;
	while (p) {
		if(!memcmp(&(p->user_addr) ,&src,sizeof(struct in6_addr))){
		
			*q = p->next;
#ifndef USE_STATIC_ENTRY_BUFFER 				
			free(p);
#else
			del_mbr_entry_from_tbl(p);
#endif			
			ifrec->user_count--;
			return ifrec->user_count;
		}
		q = &p->next;
		p = p->next;
	}
	
	return ifrec->user_count;
}

#endif



fd_set in_fds;		/* set of fds that wait_input waits for */
int max_in_fd;		/* highest fd set in in_fds */

/*
 * add_fd - add an fd to the set that wait_input waits for.
 */
void add_fd(int fd)
{
    FD_SET(fd, &in_fds);
    if (fd > max_in_fd)
	max_in_fd = fd;
}

/*
 * remove_fd - remove an fd from the set that wait_input waits for.
 */
void remove_fd(int fd)
{
    FD_CLR(fd, &in_fds);
}

/////////////////////////////////////////////////////////////////////////////
//	22/04/2004, Casey
/*
	Modified the following items:
	1.	delete all muticast router functions, xDSL router never use such function
	2.	igmp_handler only accept message for IGMP PROXY
	3.	IGMP proxy keep track on multicast address by mcft table, 
		not multicast router module.

	igmp_handler rule:
	1.	only accept IGMP query from upstream interface, and it trigger
		downstream interface to send IGMP query.
	2.	only accept IGMP report from downstream interface, and it trigger
		upstream interface to send IGMP report.
	3.	when received IGMP report, recorded its group address as forwarding rule.
	4.	only accept IGMP leave from downstream interface, downstream interface
		will send IGMP general query twice to make sure there is no other member.
		If it cannot find any member, upstream interface will send IGMP leave.
		
	forwarding rule:
	1.	system only forward multicast packets from upstream interface to downstream interface.
	2.	system only forward multicast packets which group address learned by IGMP report.
	
*/
/////////////////////////////////////////////////////////////////////////////
//



#define RECV_BUF_SIZE	2048
char *recv_buf, *send_buf;

int mld_inf_create(char *ifname)
{
	struct ipv6_mreq mreq;
	int i;
	int ret;
	struct IfDesc *dp;


	dp = getIfByName(ifname);
	if(dp==NULL)
		return 0;
    if ((dp->sock = socket(AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0) 
		log(LOG_ERR, errno, "ICMPv6 socket");
	#if 0
	{	
		/* Set router alert option*/
		char ra[4];
		ra[0] = 148;
		ra[1] = 4;
		ra[2] = 0;
		ra[3] = 0;
		setsockopt(dp->sock, IPPROTO_IPV6, IP_OPTIONS, ra, 4);//IP_OPTIONS这个选项??
	}
	#endif
	/* init mld */	
	/* Set reuseaddr, hops, loopback and set outgoing interface */
	i = 1;
	ret = setsockopt(dp->sock, SOL_SOCKET, SO_REUSEADDR, (void*)&i, sizeof(i));
	if(ret)
		printf("setsockopt SO_REUSEADDR error!\n");
	i = 1;
	/*IPv6 hops*/
	ret = setsockopt(dp->sock, IPPROTO_IPV6 , IPV6_MULTICAST_HOPS, 
		(void*)&i, sizeof(i));
	if(ret)
		printf("setsockopt IPV6_MULTICAST_HOPS error!\n");
	
	i = 0;
	ret = setsockopt(dp->sock, IPPROTO_IPV6 , IPV6_MULTICAST_LOOP, 
		(void*)&i, sizeof(i));
	if(ret)
		printf("setsockopt IPV6_MULTICAST_LOOP error!\n");
	i = 1;
	ret = setsockopt(dp->sock, IPPROTO_IPV6 , IPV6_PKTINFO, &i, sizeof(struct in6_pktinfo));
	if(ret)
		printf("setsockopt IPV6_PKTINFO error!\n");
	
	int ifindex=(int)(dp->pif_idx);
	ret = setsockopt(dp->sock, IPPROTO_IPV6 , IPV6_MULTICAST_IF, 
		(void*)&ifindex, sizeof(int));
	if(ret){
		printf("setsockopt IPV6_MULTICAST_IF error!\n");
		log( LOG_ERR, errno, "IPV6_MULTICAST_IF" );
	}
	

	return 0;
	
}


int init_mld(void)
{
	int val;
	recv_buf = malloc(RECV_BUF_SIZE);
	send_buf = malloc(RECV_BUF_SIZE);
	FD_ZERO(&in_fds);
	max_in_fd = 0;

	mld_inf_create(mld_down_if_name);

#ifdef CONFIG_MLDPROXY_MULTIWAN	
	int idx;
	for(idx=0;idx<mld_up_if_num;idx++)
		mld_inf_create(mld_up_if_name[idx]);

#else
	mld_inf_create(mld_up_if_name);
#endif

	/*arrange start up query*/
	startupQueryTimer.type=LIMIT_RETRY_TIMER_TYPE;
	startupQueryTimer.retry_left=2;
	startupQueryTimer.timerInterval=STARTUP_GENERAL_QUERY_INTERVAL;
	timeout(mld_general_query_timer_expired , &startupQueryTimer, startupQueryTimer.timerInterval, &startupQueryTimer.ch);
	
	/*schedule periodical general query*/
	generalQueryTimer.type=PERIODICAL_TIMER_TPYE;
	generalQueryTimer.retry_left=0xFFFFFFFF;
	generalQueryTimer.timerInterval=PERIODICAL_GENERAL_QUERY_INTERVAL;
	timeout(mld_general_query_timer_expired , &generalQueryTimer,  generalQueryTimer.timerInterval, &generalQueryTimer.ch);

	// Kaohj --- enable MLD rx
	MLD_rx_enable = 1;
	//printf("\n[%s]:[%d].MLD_rx_enable=%d\n",__FUNCTION__,__LINE__,MLD_rx_enable);
	return 0;
}

void shut_mld_proxy(void)
{
	/* all interface leave multicast group */
}

#ifdef CONFIG_MLDPROXY_MULTIWAN
// Kaohj -- add multicast membership to upstream interface(s)
int add_membership(struct in6_addr group,int infindex)
{
	struct ipv6_mreq mreq;
	struct IfDesc *up_dp ;
	int index;
	int ret;

	struct IfDesc *down_dp ;
	//add membership in the interface except the interface that receive report.
	down_dp=getIfByName(mld_down_if_name);
	if ((int)down_dp->pif_idx!=infindex){

		memcpy(&(mreq.ipv6mr_multiaddr),&(group),sizeof(struct in6_addr));
		mreq.ipv6mr_ifindex =(int)(down_dp->pif_idx);
		
		ret = setsockopt(down_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt IPV6_ADD_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			return ret;
		}
	}
	
	for(index=0;index<mld_up_if_num;index++)
	{
		up_dp= getIfByName(mld_up_if_name[index]);
		
		if(up_dp==NULL)
			continue;
		
		if ((int)up_dp->pif_idx!=infindex){
			memcpy(&(mreq.ipv6mr_multiaddr),&(group),sizeof(struct in6_addr));
			mreq.ipv6mr_ifindex =(int)(up_dp->pif_idx);
			
			ret = setsockopt(up_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
			if(ret) {
				printf("setsockopt IPV6_ADD_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
				return ret;
			}
		}
	}
	return 1;

}

// Add MRoute
int add_mfc(struct in6_addr group,struct in6_addr src,int infindex)
{
	struct MRouteDesc	mrd;
	struct ifrec_entry *if_rec;
	int index;
	struct IfDesc *up_dp ;
	struct in6_addr origin_tmp;
	struct mcft_entry *mymcp=get_mcft(group);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp;
	uint8 cin_vif_idx;//the vif_index of interface receive report
	int pifindex;
	memset(&origin_tmp,0,sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	
	//set TTL
	/*to set mfc Ttlvc*/
	if (mymcp->if_count==1)
	{
		//the first time receive report,and if_count=1
		
		cin_dp= getIfByPix(infindex);
		cin_vif_idx=(uint8)cin_dp->vif_idx;
		mrd.TtlVc[cin_vif_idx] = 1;
		//printf("New group!MFC out if:%d,[%s]:[%d].\n",cin_vif_idx,__FUNCTION__,__LINE__);
	}
	else{
		if_rec=mymcp->iflist;
		while(if_rec){
			pifindex=if_rec->ifindex;
			cin_dp= getIfByPix(pifindex);
			cin_vif_idx=(uint8)cin_dp->vif_idx;
			mrd.TtlVc[cin_vif_idx] = 1;	//data pkt out interface==interface receive interface
			if_rec=if_rec->next;
			
		}
		//printf("Exited group![%s]:[%d].\n",__FUNCTION__,__LINE__);
	}
	
	//up don't receive report,then add mfc in up
	for(index=0;index<mld_up_if_num;index++) {
		up_dp = getIfByName(mld_up_if_name[index]);
		if(up_dp==NULL)
			continue;
		if((int)(up_dp->pif_idx)!=infindex){
			
			mrd.InVif = mld_up_if_idx[index];
			origin_tmp.s6_addr[15]=(__u8)mrd.InVif;
			memcpy(&mrd.OriginAdr,&origin_tmp,sizeof(struct in6_addr));
			memcpy(&(mrd.SubsAdr),&(src),sizeof(struct in6_addr));
			memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
			
			//printf("vif=%d,cin_vif_idx=%d,[%s]:[%d].\n",vif,cin_vif_idx,__FUNCTION__,__LINE__);
			addMRoute(&mrd);
			
		}
	}
	//down don't receive report and add mfc in down
	if((int)(down_dp->pif_idx)!=infindex){
		
		mrd.InVif = mld_down_if_idx;
		origin_tmp.s6_addr[15]=(__u8)mrd.InVif;
		memcpy(&mrd.OriginAdr,&origin_tmp,sizeof(struct in6_addr));
		memcpy(&(mrd.SubsAdr),&(src),sizeof(struct in6_addr));
		memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
		addMRoute(&mrd);
	}

	return 1;
}


#if 0
//  update_mfc when a interface send leave report
int update_mfc(struct in6_addr group,struct in6_addr src,int infindex)
{
	struct MRouteDesc	mrd;
	struct in6_addr origin_tmp;
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	int pifindex;
	struct mcft_entry *mymcp=get_mcft(group);
	//struct mbr_entry	*gcp=mymcp->grp_mbr;
	struct IfDesc *up_dp ;
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp;
	uint8 cin_vif_idx;
	
	/* add multicast routing entry */
	memset(&(mrd.OriginAdr),0,sizeof(struct in6_addr));
	memset(&origin_tmp,0,sizeof(struct in6_addr));
	// Kaohj --- special case, save the subscriber IP to kernel
	// in order to take the subscriber IP (source IP) to the upstream server
	memcpy(&(mrd.SubsAdr),&(src),sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));

	//set ttl
	{
		
		struct mbr_entry	*gcp=mymcp->grp_mbr;
		while(gcp){
			pifindex=gcp->ifindex;
			cin_dp= getIfByPix(pifindex);
			cin_vif_idx=(uint8)cin_dp->vif_idx;
			if(pifindex!=infindex){
				mrd.TtlVc[cin_vif_idx] = 1;	//data pkt out interface==interface receive interface
			}
			else{
				printf("the interface leave group![%s]:[%d].\n",__FUNCTION__,__LINE__);
				mrd.TtlVc[cin_vif_idx] = 0;	//the interface leave group, and not set it 
			}
			gcp=gcp->next;
			
		}
		printf("Exited group![%s]:[%d].\n",__FUNCTION__,__LINE__);
	}


	//up  receive leave report,then add  up in
	for(index=0;index<mld_up_if_num;index++) {
		up_dp = getIfByName(mld_up_if_name[index]);
		if(up_dp==NULL)
			continue;
		if((int)(up_dp->pif_idx)==infindex){
			//vif =mld_up_if_idx[index];
			mrd.InVif = mld_up_if_idx[index];
			origin_tmp.s6_addr[15]=(__u8)mrd.InVif;
			memcpy(&mrd.OriginAdr,&origin_tmp,sizeof(struct in6_addr));
		
			//printf("vif=%d,cin_vif_idx=%d,[%s]:[%d].\n",vif,cin_vif_idx,__FUNCTION__,__LINE__);
			addMRoute(&mrd);
			
		}
	}
	//down receive leave report and add  down in
	if((int)(down_dp->pif_idx)==infindex){
		
		mrd.InVif = mld_down_if_idx;
		origin_tmp.s6_addr[15]=(__u8)mrd.InVif;
		memcpy(&mrd.OriginAdr,&origin_tmp,sizeof(struct in6_addr));
		
		//mrd.TtlVc[cin_vif_idx] = 1;	//data pkt out interface==interface receive  report
		addMRoute(&mrd);
	}
	

	return 1;

}

#else
//  update_mfc when a interface send leave report
int update_mfc(struct in6_addr group,struct in6_addr src,int infindex)

{
	struct MRouteDesc	mrd;
	struct in6_addr origin_tmp;
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	int pifindex[MAX_MC_VIFS];
	struct mcft_entry *mymcp=get_mcft(group);
	//struct mbr_entry	*gcp=mymcp->grp_mbr;
	struct IfDesc *up_dp ;
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp;
	uint8 cin_vif_idx;
	
	/* add multicast routing entry */
	memset(&(mrd.OriginAdr),0,sizeof(struct in6_addr));
	memset(&origin_tmp,0,sizeof(struct in6_addr));
	// Kaohj --- special case, save the subscriber IP to kernel
	// in order to take the subscriber IP (source IP) to the upstream server
	memcpy(&(mrd.SubsAdr),&(src),sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));

	//set ttl
	


	int join_state=0;
	int i=0;
	int ifnum=0;
	{
		struct ifrec_entry	*if_rec=mymcp->iflist;
	if (mymcp){
		//printf("[%s]:mymcp->ifcount=%d\n",__FUNCTION__,mymcp->if_count);
		while(if_rec){
			pifindex[i]=if_rec->ifindex;
			cin_dp= getIfByPix(pifindex[i]);
			cin_vif_idx=(uint8)cin_dp->vif_idx;
			//printf("pifindex[%d]:%d.cin_vif_idx:%d.[%s].\n",i,pifindex[i],cin_vif_idx,__FUNCTION__);
			if(pifindex[i]!=infindex){
				//printf("out interface:pif=%d.vif=%d.[%s].\n",pifindex[i],cin_vif_idx,__FUNCTION__);
				mrd.TtlVc[cin_vif_idx] = 1;	//interface joined , set it
				
			}
			else{
				//printf("the interface leave group!pif=%d.vif=%d.[%s].\n",pifindex[i],cin_vif_idx,__FUNCTION__);
				mrd.TtlVc[cin_vif_idx] = 0;	//the interface leave group, and not set it 
				mrd.InVif = cin_vif_idx;
			}
			i++;
			if_rec=if_rec->next;
			
		}
	}
		//printf("Exited group![%s]:[%d].\n",__FUNCTION__,__LINE__);
	}
	ifnum=i;
	
	
	{
		for(i=0;i<mld_if_num;i++){
			origin_tmp.s6_addr[15]=(__u8)i;
			memcpy(&mrd.OriginAdr ,&origin_tmp,sizeof(struct in6_addr));
			if ((mymcp->if_count<=2)&&(mrd.TtlVc[i] == 1)){
				//printf("DEL.[%s]:[%d]\n",__FUNCTION__,__LINE__);
				delMRoute(&mrd);
			}
			else{
				//printf("ADD.[%s]:[%d]\n",__FUNCTION__,__LINE__);
				addMRoute(&mrd);
			}
		}
	}

	return 1;

}

#endif

// Kaohj -- delete multicast membership to upstream interface(s)
int del_membership(struct in6_addr group,int infindex)
{
	struct ipv6_mreq mreq;
	struct IfDesc *up_dp ;
	int ret;
	int index;
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	
	for(index=0;index<mld_up_if_num;index++){
		up_dp= getIfByName(mld_up_if_name[index]);
		if(up_dp==NULL)
			continue;
		if ((int)up_dp->pif_idx!=infindex){
			mreq.ipv6mr_ifindex =(int)(up_dp->pif_idx);
			ret = setsockopt(up_dp->sock, IPPROTO_IPV6 ,  IPV6_DROP_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
			if(ret) {
				printf("setsockopt IPV6_DROP_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			
			}
		}
	}
	
	if ((int)down_dp->pif_idx!=infindex){
		mreq.ipv6mr_ifindex =(int)(down_dp->pif_idx);
		ret = setsockopt(down_dp->sock, IPPROTO_IPV6 ,  IPV6_DROP_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt  IPV6_DROP_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			
		}
	}

	return 1;
}

// Delete MRoute
int del_mfc(struct in6_addr group,int infindex)
{
	struct MRouteDesc	mrd;
	int index;
	struct in6_addr origin_tmp;
	
	memset(&origin_tmp,0,sizeof(struct in6_addr));

	struct IfDesc *up_dp ;
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	
	for(index=0;index<mld_up_if_num;index++) {
		up_dp = getIfByName(mld_up_if_name[index]);
		if((int)(up_dp->pif_idx)!=infindex){
			
			mrd.InVif = mld_up_if_idx[index];
			origin_tmp.s6_addr[15]=(__u8)mrd.InVif;
			memcpy(&mrd.OriginAdr ,&origin_tmp,sizeof(struct in6_addr));
			memcpy(&(mrd.McAdr),&group,sizeof(struct in6_addr));
			
			memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
			//mrd.TtlVc[cin_vif_idx] = 1;	//data pkt out interface==interface receive interface
			delMRoute(&mrd);
			
		}
	}
	
	//down don't receive report and del mfc in down
	if((int)(down_dp->pif_idx)!=infindex){
		
		mrd.InVif = mld_down_if_idx;
		origin_tmp.s6_addr[15]=(__u8)mrd.InVif;
		//mrd.OriginAdr.s_addr = 0;
		memcpy(&mrd.OriginAdr ,&origin_tmp,sizeof(struct in6_addr));
		memcpy(&(mrd.McAdr),&group,sizeof(struct in6_addr));
		
		memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
		//mrd.TtlVc[cin_vif_idx] = 1;	//data pkt out interface==interface receive interface
		delMRoute(&mrd);
	}

	return 1;
}

#else

// Kaohj -- add multicast membership to upstream interface(s)
int add_membership(struct in6_addr group,int infindex)
{
	struct ipv6_mreq mreq;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp=getIfByName(mld_down_if_name); ;
	int ret;
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);

	
	/* join multicast group */
	memcpy(&(mreq.ipv6mr_multiaddr),&(group),sizeof(struct in6_addr));

	//add membership except the interface receive report:infindex

	/*down inf receive mld report, add member at up inf*/
	if ((up_dp) && ((int)up_dp->pif_idx!=infindex)){
		//printf("infindex=%d,up_dp->pif_idx=%d,[%s]:[%d].\n",infindex,up_dp->pif_idx,__FUNCTION__,__LINE__);
		mreq.ipv6mr_ifindex =(int)(up_dp->pif_idx);
		ret = setsockopt(up_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt IPV6_ADD_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			return ret;
		}
	}
	
	/*up inf receive mld report, add member at down inf*/
	if ((down_dp) && ((int)down_dp->pif_idx!=infindex)){
		//printf("infindex=%d,up_dp->pif_idx=%d,[%s]:[%d].\n",infindex,up_dp->pif_idx,__FUNCTION__,__LINE__);
		mreq.ipv6mr_ifindex =(int)(down_dp->pif_idx);
		ret = setsockopt(down_dp->sock, IPPROTO_IPV6 , IPV6_ADD_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt IPV6_ADD_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			return ret;
		}
	}
	return 1;
		

}

// Add MRoute
int add_mfc(struct in6_addr group, struct in6_addr src,int infindex)
{
	struct MRouteDesc	mrd;
	
	int pifindex;
	struct ifrec_entry 	*if_rec=NULL;
	struct mcft_entry *mymcp=get_mcft(group);
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp;//= getIfByPix(infindex);
	uint8 cin_vif_idx;//=(uint8)cin_dp->vif_idx;
	struct in6_addr origin_tmp;
	int vif;
	if(mymcp==NULL)
	{
		return 0;
	}
	//printf("[%s]:[d].\n",__FUNCTION__,__LINE__);
	/* add multicast routing entry */
	memset(&(mrd.OriginAdr),0,sizeof(struct in6_addr));
	memset(&origin_tmp,0,sizeof(struct in6_addr));
	// Kaohj --- special case, save the subscriber IP to kernel
	// in order to take the subscriber IP (source IP) to the upstream server
	memcpy(&(mrd.SubsAdr),&(src),sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));

	/*to set mfc Ttlvc*/
	if (mymcp->if_count==1)
	{
		//the first time receive report,and if_count=1
		
		cin_dp= getIfByPix(infindex);
		cin_vif_idx=(uint8)cin_dp->vif_idx;
		mrd.TtlVc[cin_vif_idx] = 1;
		//printf("New group!MFC out if:%d,[%s]:[%d].\n",cin_vif_idx,__FUNCTION__,__LINE__);
	}
	else{
		if_rec=mymcp->iflist;
		while(if_rec){
			pifindex=if_rec->ifindex;
			cin_dp= getIfByPix(pifindex);
			cin_vif_idx=(uint8)cin_dp->vif_idx;
			mrd.TtlVc[cin_vif_idx] = 1;	//data pkt out interface==interface receive interface
			if_rec=if_rec->next;
			
		}
		//printf("Exited group![%s]:[%d].\n",__FUNCTION__,__LINE__);
	}
	//down inf receive mld join, set mrd.Invif to up_if_idx
	if((int)(up_dp->pif_idx)!=infindex){
		vif=mld_up_if_idx;
		mrd.InVif = mld_up_if_idx;
		origin_tmp.s6_addr[15]=(__u8)vif;
		memcpy(&mrd.OriginAdr,&origin_tmp,sizeof(struct in6_addr));
		//printf("up_dp->pif_idx:%d,infindex:%d,mrd.InVif:%d.[%s]:[%d]",up_dp->pif_idx,infindex,mrd.InVif,__FUNCTION__,__LINE__);
		addMRoute(&mrd);
		
	}
	
	//up inf receive mld join, set mrd.Invif to down_if_idx
	if((int)(down_dp->pif_idx)!=infindex){
		vif =mld_down_if_idx;
		mrd.InVif = mld_down_if_idx;
		origin_tmp.s6_addr[15]=(__u8)vif;
		memcpy(&mrd.OriginAdr,&origin_tmp,sizeof(struct in6_addr));
		//printf("down_dp->pif_idx:%d,infindex:%d,mrd.InVif:%d.[%s]:[%d]\n",down_dp->pif_idx,infindex,mrd.InVif,__FUNCTION__,__LINE__);
		addMRoute(&mrd);
	}

	return 1;

}


// Kaohj -- delete multicast membership to upstream interface(s)
int del_membership(struct in6_addr group,int infindex)
{
	struct ipv6_mreq mreq;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	int ret;
	/* drop multicast group */
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	memcpy(&(mreq.ipv6mr_multiaddr),&(group),sizeof(struct in6_addr));

	if ((int)up_dp->pif_idx!=infindex){
		//printf("infindex=%d,up_dp->pif_idx=%d,[%s]:[%d].\n",infindex,up_dp->pif_idx,__FUNCTION__,__LINE__);
		mreq.ipv6mr_ifindex =(int)(up_dp->pif_idx);
		ret = setsockopt(up_dp->sock, IPPROTO_IPV6 ,  IPV6_DROP_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt IPV6_DROP_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			
		}
	}
	if ((int)down_dp->pif_idx!=infindex){
		//printf("infindex=%d,up_dp->pif_idx=%d,[%s]:[%d].\n",infindex,up_dp->pif_idx,__FUNCTION__,__LINE__);
		mreq.ipv6mr_ifindex =(int)(down_dp->pif_idx);
		ret = setsockopt(down_dp->sock, IPPROTO_IPV6 ,  IPV6_DROP_MEMBERSHIP, (void*)&mreq, sizeof(mreq));
		if(ret) {
			printf("setsockopt  IPV6_DROP_MEMBERSHIP error%d !\n", mreq.ipv6mr_ifindex);
			
		}
	}
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	return 1;
}

// Delete MRoute
int del_mfc(struct in6_addr group,int infindex)
{
	struct MRouteDesc	mrd;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct in6_addr origin_tmp;
	/* delete multicast routing entry */
	memset(&origin_tmp,0,sizeof(struct in6_addr));
	memset(&(mrd.OriginAdr),0,sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&group,sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	/*
	if (infindex==-1){
		mrd.InVif = -1;
		delMRoute(&mrd);
	}
	
	else
	*/
	{
		
		if((int)(up_dp->pif_idx)!=infindex){
			mrd.InVif = mld_up_if_idx;
			origin_tmp.s6_addr[15]=(__u8)mrd.InVif;
			memcpy(&mrd.OriginAdr ,&origin_tmp,sizeof(struct in6_addr));
			//printf("up_dp->pif_idx:%d,infindex:%d,mrd.InVif:%d.[%s]:[%d]",up_dp->pif_idx,infindex,mrd.InVif,__FUNCTION__,__LINE__);
			//mrd.TtlVc[cin_vif_idx] = 1;	//data pkt out interface==interface receive interface
			delMRoute(&mrd);
			
		}
		//down don't receive report and del mfc in down
		if((int)(down_dp->pif_idx)!=infindex){
			mrd.InVif = mld_down_if_idx;
			origin_tmp.s6_addr[15]=(__u8)mrd.InVif;
			memcpy(&mrd.OriginAdr ,&origin_tmp,sizeof(struct in6_addr));
			//printf("down_dp->pif_idx:%d,infindex:%d,mrd.InVif:%d.[%s]:[%d]",down_dp->pif_idx,infindex,mrd.InVif,__FUNCTION__,__LINE__);
			//mrd.TtlVc[cin_vif_idx] = 1;	//data pkt out interface==interface receive interface
			delMRoute(&mrd);
		}
	}

	return 1;
}

//  update_mfc when a interface receive leave report or time out
int update_mfc(struct in6_addr group,struct in6_addr src,int infindex)
{
	struct MRouteDesc	mrd;
	int i,ifnum,join_state;
	int pifindex[MAX_MC_VIFS];
	struct mcft_entry *mymcp=get_mcft(group);
	//struct mbr_entry	*gcp=mymcp->grp_mbr;
	struct IfDesc *up_dp = getIfByName(mld_up_if_name);
	struct IfDesc *down_dp = getIfByName(mld_down_if_name);
	struct IfDesc *cin_dp;//= getIfByPix(infindex);
	uint8 cin_vif_idx;//=(uint8)cin_dp->vif_idx;
	struct in6_addr origin_tmp;
	memset(&origin_tmp,0,sizeof(struct in6_addr));
	
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	/* add multicast routing entry */
	
	//set OriginAdr as vif for co-work with kernel  patch
	memset(&(mrd.OriginAdr),0,sizeof(struct in6_addr));
	
	// Kaohj --- special case, save the subscriber IP to kernel
	// in order to take the subscriber IP (source IP) to the upstream server
	memcpy(&(mrd.SubsAdr),&(src),sizeof(struct in6_addr));
	memcpy(&(mrd.McAdr),&(group),sizeof(struct in6_addr));
	memset(mrd.TtlVc, 0, sizeof(mrd.TtlVc));
	join_state=0;
	i=0;
	ifnum=0;
	{
		struct ifrec_entry	*if_rec=mymcp->iflist;
	if (mymcp){
		//printf("[%s]:mymcp->ifcount=%d\n",__FUNCTION__,mymcp->if_count);
		while(if_rec){
			pifindex[i]=if_rec->ifindex;
			cin_dp= getIfByPix(pifindex[i]);
			cin_vif_idx=(uint8)cin_dp->vif_idx;
			//printf("pifindex[%d]:%d.cin_vif_idx:%d.[%s].\n",i,pifindex[i],cin_vif_idx,__FUNCTION__);
			if(pifindex[i]!=infindex){
				//printf("out interface:pif=%d.vif=%d.[%s].\n",pifindex[i],cin_vif_idx,__FUNCTION__);
				mrd.TtlVc[cin_vif_idx] = 1;	//interface joined , set it
				
			}
			else{
				//printf("the interface leave group!pif=%d.vif=%d.[%s].\n",pifindex[i],cin_vif_idx,__FUNCTION__);
				mrd.TtlVc[cin_vif_idx] = 0;	//the interface leave group, and not set it 
				mrd.InVif = cin_vif_idx;
			}
			i++;
			if_rec=if_rec->next;
			
		}
	}
		//printf("Exited group![%s]:[%d].\n",__FUNCTION__,__LINE__);
	}
	ifnum=i;
	//if (mymcp->ifcount-1==1)
	
	{
		for(i=0;i<mld_if_num;i++){
			origin_tmp.s6_addr[15]=(__u8)i;
			memcpy(&mrd.OriginAdr ,&origin_tmp,sizeof(struct in6_addr));
			if ((mymcp->if_count<=2)&&(mrd.TtlVc[i] == 1)){
				//printf("DEL.[%s]:[%d]\n",__FUNCTION__,__LINE__);
				delMRoute(&mrd);
			}
			else{
				//printf("ADD.[%s]:[%d]\n",__FUNCTION__,__LINE__);
				addMRoute(&mrd);
			}
		}
	}
	
	return 1;

}

#endif
// Add group membershipt and MRoute
int add_mr(struct in6_addr group, struct in6_addr src,int infindex)
{
	add_membership(group,infindex);
	add_mfc(group, src, infindex);
	return 1;
}


// Delete group membership and MRoute
int del_mr(struct in6_addr group,int infindex)
{
	del_membership(group,infindex);
	del_mfc(group,infindex);
	return 1;
}


void mld_specific_grp_timer_expired(void *arg)

{

	struct mcft_entry *mcp = arg;
	struct ifrec_entry *if_rec=mcp->iflist;
	struct in6_addr dst;
	memcpy(&dst,&IS_MLD_ALL_HOSTS_ADDRESS,sizeof(struct in6_addr));
	//int all_if=-1;/*no pif=-1*/
	int ifindex;
	//to-do: don't send leave to all interface
	//printf(" mld_specific_grp_timer_expired()\n");
	if(!mcp)
		return;

	mcp->timer.retry_left--;
		
	if(mcp->timer.retry_left <= 0) {
		// Kaohj --- check if group has already been dropped
		untimeout(&mcp->timer.ch);
		if ((mcp->if_count!= 0 ))
		{
			while(if_rec){
				ifindex=if_rec->ifindex;
				del_mr(mcp->grp_addr,ifindex);   
				if_rec=if_rec->next;
			}
			del_mcft(mcp->grp_addr,ifindex);
				

		}

		untimeout(&mcp->timer.ch);
	}
	else {
		
		timeout(mld_specific_grp_timer_expired , mcp, LAST_MEMBER_QUERY_INTERVAL, &mcp->timer.ch);
		mld_query(dst, mcp->grp_addr, LAST_MEMBER_QUERY_INTERVAL);
	}

}
void mld_specific_if_timer_expired(void *arg)

{
	struct mcft_entry *mcp;
	struct ifrec_entry *myif = arg;
	struct in6_addr src_0;
	struct in6_addr dst;
	struct in6_addr group;
	
	memset(&src_0, 0 , sizeof(struct in6_addr));
	memcpy(&dst,&IS_MLD_ALL_HOSTS_ADDRESS,sizeof(struct in6_addr));
	int ifindex;
	//printf(" mld_specific_if_timer_expired()\n");
	
	if(myif==NULL){
		printf("interface not exited yet![%s]:[%d].\n",__FUNCTION__,__LINE__);
		return;
	}
	
	//printf("retry_left:%d.[%s]:[%d].\n",myif->timer.retry_left--,__FUNCTION__,__LINE__);
	myif->timer.retry_left--;
	memcpy(&group,&myif->grp_addr,sizeof(struct in6_addr));	
	mcp=get_mcft(group);	
	if(myif->timer.retry_left <= 0) {
		// Kaohj --- check if interface has already been dropped
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		if ((myif->user_count!= 0 ))
		{
			
			//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
			if(mcp->if_count>1){
				update_mfc(group,src_0,myif->ifindex);
				del_if(mcp,myif->ifindex);
			}
			else
			{
				//if_count==1,  mld_specific_grp_timer_expired will process
				printf("The Group Time out![%s]:[%d].\n",__FUNCTION__,__LINE__);
				//del_mr(mcp->grp_addr,if_rec->ifindex);				
				//del_mcft(mcp->grp_addr,if_rec->ifindex);
			}
		}

		untimeout(&myif->timer.ch);
	}
	else {
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		timeout(mld_specific_if_timer_expired , myif, LAST_MEMBER_QUERY_INTERVAL, &myif->timer.ch);
		mld_query(dst, group, LAST_MEMBER_QUERY_INTERVAL);
	}
	
	
}


void mld_general_query_timer_expired(void *arg)	
{
	struct mld_timer	*timerPtr=arg;
	struct in6_addr dst,grp;
	memset(&grp,0,sizeof(struct in6_addr));
	memcpy(&dst,&IS_MLD_ALL_HOSTS_ADDRESS,sizeof(struct in6_addr));
	if(timerPtr!=NULL)
	{	
		if((timerPtr->type==PERIODICAL_TIMER_TPYE))
		{
			
			mld_query(dst, grp, 1);
			timeout(mld_general_query_timer_expired , timerPtr, timerPtr->timerInterval, &timerPtr->ch);
		}
		else
		{
			if(timerPtr->retry_left==0) 
			{
				untimeout(&timerPtr->ch);
			}
			else 
			{
				mld_query(dst, grp, 1);
				timerPtr->retry_left--;
				timeout(mld_general_query_timer_expired , timerPtr, timerPtr->timerInterval, &timerPtr->ch);
			}
		}
	}
}

#ifdef CONFIG_CHECK_MULTICASTROUTE
int check_entry1=0;
int check_entry2=0;
int check_multicast_route=0;
#endif

			
int add_group_and_src( struct in6_addr group ,struct in6_addr src,int ifindex )
{
	struct mcft_entry *mymcp;
	struct ifrec_entry *myif;
	int retif;
	if(!IS_IPV6_MULTICAST_ADDRESS(group.s6_addr)){
		printf("NOT_IPV6_MULTICAST_ADDRESS![%s][%d]\n",__FUNCTION__,__LINE__);
		return 0;
	}
	
	/* TBD */			
	/* should check if it's from downtream interface */
	if(!chk_mcft(group)) {
		// Group does not exist on router, add multicast address into if_table
		int ret;
		//printf("Group does not exist on router![%s]:[%d].\n",__FUNCTION__,__LINE__);
		mymcp = add_mcft(group, src,ifindex);
		if(!mymcp) {
			printf("MLD_accept> add group to list fail!\n");
			return 0;
		}
		myif=get_ifrec(mymcp,ifindex);
		if(!myif){
			del_mcft(mymcp->grp_addr,ifindex);
			printf("MLD_accept> add interface to list fail!\n");
			return 0;
		}
				
		add_mr(group, src,ifindex);
		
#ifdef PERIODICAL_SPECIFIC_QUERY
		mymcp->timer.retry_left = MEMBER_QUERY_COUNT+1;
		timeout(mld_specific_grp_timer_expired , mymcp, MEMBER_QUERY_INTERVAL, &mymcp->timer.ch);
		myif->timer.retry_left = MEMBER_QUERY_COUNT+1;
		timeout(mld_specific_if_timer_expired , myif, MEMBER_QUERY_INTERVAL, &myif->timer.ch);
#endif
	}
	else {
		mymcp = get_mcft(group);
		if (mymcp)
		{
			//printf("group existed![%s]:[%d].\n",__FUNCTION__,__LINE__);
			untimeout(&mymcp->timer.ch);
			myif = get_ifrec(mymcp,ifindex);
			if(myif==NULL){
				//interface not found, add new interface
				//printf("new interface , mfc update![%s]:[%d].\n",__FUNCTION__,__LINE__);
				retif=add_if(mymcp,src,ifindex);
				add_membership(group,ifindex);
				add_mfc(group,src,ifindex);
			}
			else{
				//interface existed!
#ifdef KEEP_GROUP_MEMBER
				int ret;
				ret=add_user(myif, src);
#endif
				untimeout(&myif->timer.ch);
			}
			//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
#ifdef PERIODICAL_SPECIFIC_QUERY
			myif= get_ifrec(mymcp,ifindex);
			mymcp->timer.retry_left = MEMBER_QUERY_COUNT+1;
			timeout(mld_specific_grp_timer_expired , mymcp, MEMBER_QUERY_INTERVAL, &mymcp->timer.ch);
			myif->timer.retry_left = MEMBER_QUERY_COUNT+1;
			timeout(mld_specific_if_timer_expired , myif, MEMBER_QUERY_INTERVAL, &myif->timer.ch);
#endif
		}
	}

		
	return 0;	
}


int del_group_and_src( struct in6_addr group, struct in6_addr src,int ifindex )
{
	struct mcft_entry *mymcp;
	struct ifrec_entry *myif;
	int query_count, query_interval;
	struct in6_addr dst;
	memcpy(&dst,&IS_MLD_ALL_HOSTS_ADDRESS,sizeof(struct in6_addr));
#ifdef KEEP_GROUP_MEMBER
	int user_count;
#endif
	int if_count;
	
	if(!IS_IPV6_MULTICAST_ADDRESS(group.s6_addr))
	{
		printf("mld_accept> invalid multicast address or mldleave\n");
		return 0;		
	}	
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	/* TBD */			
	/* should check if it's from downtream interface */
	if(chk_mcft(group)) {
	
		mymcp = get_mcft(group);
		// Group does exist on router
		if(mymcp) {
			query_count = LAST_MEMBER_QUERY_COUNT;
			query_interval = LAST_MEMBER_QUERY_INTERVAL;

			myif = get_ifrec(mymcp,ifindex);
#ifdef KEEP_GROUP_MEMBER
			user_count = del_user(myif, src);
			if (user_count == 0){
				//untimeout(&myif->timer.ch);
				//the interface have no member, del interface
				//if_count=del_if(mymcp,ifindex);
				if_count=mymcp->if_count;
				if (if_count <= 1) {
					// no interface join, drop it!
					//untimeout(&mymcp->timer.ch);
					//printf("del mfc!!![%s]:[%d].\n",__FUNCTION__,__LINE__);
					del_if(mymcp,ifindex);
					del_mr(mymcp->grp_addr,ifindex);    			
					del_mcft(mymcp->grp_addr,ifindex);
				
#ifdef CONFIG_CHECK_MULTICASTROUTE
					alarm(5);
#endif  	
				}
				else {
					//update mfc
					//printf("update mfc!!![%s]:[%d].\n",__FUNCTION__,__LINE__);
					del_membership(mymcp->grp_addr,ifindex); 
					update_mfc(mymcp->grp_addr, src, ifindex);  
					del_if(mymcp,ifindex);
					//printf("if_count=%d.\n[%s]:[%d].\n",if_count,__FUNCTION__,__LINE__);
					
				}
			}

#endif
			mymcp->timer.retry_left = LAST_MEMBER_QUERY_COUNT;
			timeout(mld_specific_grp_timer_expired , mymcp, LAST_MEMBER_QUERY_INTERVAL, &mymcp->timer.ch);
			mld_query(dst, mymcp->grp_addr, LAST_MEMBER_QUERY_INTERVAL);

		}
	}

	return 0;
}

/*
 * Process a newly received IPv6 MLD packet that is sitting in the input packet
 * buffer.
 * the MLD version of a multicast listener Query is determined as
 * follow : MLDv1 query : recvlen = 24
 *          MLDv2 query : recvlen >= 28
 *          MLDv2 report type!= MLDv1 report type
 * Query messages that do not match any of the above conditions are ignored.
 */
static int
mld_accept(recvlen)
	int recvlen;

{
	struct mrt6msg *msg;
	struct in6_addr *group, *dst = NULL;
	struct mld_hdr *mldh;
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
		//printf("received packet too short!recvlen:%d,sizelimit:%d",recvlen,sizeof(struct mld_hdr));
		return -1;
	}
	mldh = (struct mld_hdr *) rcvmh.msg_iov[0].iov_base;

	/*
	 * Packets sent up from kernel to daemon have ICMPv6 type = 0.
	 * Note that we set filters on the mld6_socket, so we should never
	 * see a "normal" ICMPv6 packet with type 0 of ICMPv6 type.
	 */
	if(!IS_MLD_TYPE(mldh->mld_icmp6_hdr.icmp6_type)){
		//printf("NOT MLD TYPE!\n");
		return 0;
	}
	/*
	if (IS_IPV6_RESVER_0_ADDRESS(group->s6_addr))
	{
		printf("IS_IPV6_RESVER_0_ADDRESS![%s].\n",__FUNCTION__);
		return 0;
	}
	*/
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
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);	
	//printf("mldh->mld_type:%d,ifindex:%d\n",mldh->mld_icmp6_hdr.icmp6_type,ifindex);
	if (hlimp == NULL)
	{
		log(LOG_WARNING, 0,
		    "failed to get receiving hop limit");
		return;
	}
	
	group = &mldh->mld_addr;
	/*
	printf("ifindex=%d\n",ifindex);
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
	//check 

	if (!IS_IPV6_MULTICAST_ADDRESS(group->s6_addr)){
			//printf("NOT MULTICAST1[%s]:[%d].\n",__FUNCTION__,__LINE__);
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
		
		//printf("NOT link local address!\n");
		return -1;
	}
	
	switch (mldh->mld_icmp6_hdr.icmp6_type) 
	{
		
		case ICMPV6_MGM_QUERY:
			//printf("ACCEPT ICMPV6_MGM_QUERY![%s]:[%d].\n",__FUNCTION__,__LINE__);
			/* Linux Kernel will process local member query, it won't reach here */
			
			break;

		case ICMPV6_MGM_REPORT:
			/*
			printf("ACCEPT ICMPV6_MGM_REPORT![%s]:[%d].\n",__FUNCTION__,__LINE__);
			printf("ifindex=%d\n",ifindex);
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
			add_group_and_src(*group,src->sin6_addr,ifindex);		
			
			break;

		case ICMPV6_MLD2_REPORT:
			//printf("ICMPV6_MLD2_REPORT![%s]:[%d].\n",__FUNCTION__,__LINE__);
			/* TBD */			
			/* should check if it's from downtream interface */

		{
			struct mld2_report	*mldv2_rp;
			struct mld2_grec	*mldv2grec;
			unsigned short rec_id;
			int num_grp;
			int num_src;
			struct sockaddr_in6 group_sa = { sizeof(group_sa), AF_INET6 };
			mldv2_rp  =(struct mld2_report *)mldh;
			rec_id=0;
			
			mldv2grec=&mldv2_rp->mld2r_grec[0];
			num_grp=ntohs(mldv2_rp->mld2r_ngrec);
			/*
			printf( "recv ICMPV6_MLD2_REPORT\n" );
			printf( "mldv2->type:0x%d\n",mldv2_rp->mld2r_type );
			printf( "num_grp:mldv2->ngrec:0x%d\n", ntohs(mldv2_rp->mld2r_ngrec) );
			*/
			while( rec_id < num_grp )
			{
				
				/*
				printf( "mldv2grec[%d]->grec_type:0x%x\n", rec_id, mldv2grec->grec_type );
				printf( "mldv2grec[%d]->grec_auxwords:0x%x\n", rec_id, mldv2grec->grec_auxwords );
				printf( "mldv2grec[%d]->grec_nsrcs:0x%x\n", rec_id, ntohs(mldv2grec->grec_nsrcs) );
				printf( "mldv2grec[%d]->grec_mca:0x%x-%x-%x-%x\n", 
					rec_id, mldv2grec->grec_mca.s6_addr32[0],mldv2grec->grec_mca.s6_addr32[1],mldv2grec->grec_mca.s6_addr32[2],mldv2grec->grec_mca.s6_addr32[3]);
				*/
				memcpy(group,&mldv2grec->grec_mca,sizeof(struct in6_addr));
				num_src = ntohs(mldv2grec->grec_nsrcs);
				/*
				printf( "grouppaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
							group->s6_addr16[ 0],group->s6_addr16[ 1],group->s6_addr16[ 2],group->s6_addr16[ 3],
							group->s6_addr16[ 4],group->s6_addr16[ 5],group->s6_addr16[ 6],group->s6_addr16[ 7]);
				*/
				
				
				/*
				 *	Definitions for MLDv2

				*#define MLD2_MODE_IS_INCLUDE	1
				*#define MLD2_MODE_IS_EXCLUDE	2
				*#define MLD2_CHANGE_TO_INCLUDE	3
				*#define MLD2_CHANGE_TO_EXCLUDE	4
				*#define MLD2_ALLOW_NEW_SOURCES	5
				*#define MLD2_BLOCK_OLD_SOURCES	6
				*/

				switch( mldv2grec->grec_type )
				{
					case MLD2_MODE_IS_INCLUDE:
						//printf("%s> IS_IN, num_src:%d\n", __FUNCTION__, num_src );
						if( mldv2grec->grec_nsrcs )
							add_group_and_src( *group, src->sin6_addr,ifindex);
						else //empty
							del_group_and_src( *group, src->sin6_addr,ifindex );
					
						break;
					case MLD2_MODE_IS_EXCLUDE:
						//printf("%s> IS_EX, num_src:%d\n", __FUNCTION__, num_src );
						//if(chk_mcft(*group)) /*need check group exist or not???*/
						add_group_and_src( *group, src->sin6_addr,ifindex );
			
						break;
					case MLD2_CHANGE_TO_INCLUDE: 
						//printf("%s> TO_IN, num_src:%d\n", __FUNCTION__, num_src );
						if( mldv2grec->grec_nsrcs )
							add_group_and_src( *group, src->sin6_addr,ifindex);
						else //empty
							del_group_and_src( *group, src->sin6_addr,ifindex );
				
						break;
					case MLD2_CHANGE_TO_EXCLUDE: 
						//printf("%s> IS_EX, num_src:%d\n", __FUNCTION__, num_src );
						//printf( "TO_EX\n" );
						add_group_and_src( *group, src->sin6_addr,ifindex );
						break;
					case MLD2_ALLOW_NEW_SOURCES:
						//printf( "ALLOW\n" );
						break;
					case MLD2_BLOCK_OLD_SOURCES:
						//printf( "BLOCK\n" );
						break;
					default:
						//printf( "!!! can't handle the group record types: %d\n", mldv3grec->grec_type );
						break;
				}
			
				rec_id++;
				//printf( "count next: 0x%x %d %d %d %d\n", igmpv3grec, sizeof( struct igmpv3_grec ), igmpv3grec->grec_auxwords, ntohs(igmpv3grec->grec_nsrcs), sizeof( __u32 ) );
				//mldv2grec  用在循环中
				mldv2grec = (struct mldv2grec *)( (char*)mldv2grec + sizeof( struct mld2_grec ) + (mldv2grec->grec_auxwords+ntohs(mldv2grec->grec_nsrcs))*sizeof(struct in6_addr ) );
				//printf( "count result: 0x%x\n", igmpv3grec );
			}		
			break;
		}

			//break;

		case ICMPV6_MGM_REDUCTION:
			//printf("ICMPV6_MGM_REDUCTION![%s]:[%d].\n",__FUNCTION__,__LINE__);
			del_group_and_src( *group,src->sin6_addr,ifindex );
			
			break;
		
		default:
			//printf("mld_accept> receive mld Unknown type [%x] from %s:", igmp->type, inet_ntoa(ip->saddr));
			//printf("%s\n", inet_ntoa(ip->daddr));
			break;
	}

}





/*
 * mld_query - send a mld Query packet to downstream interface
 *
 *int mld_query(struct in6_addr dst, struct in6_addr grp,__u8 mrt)
 * Where:
 *  dst		destination address
 *  grp		query group address
 *  MRT		Max Response Time in IGMP header (in 1/10 second unit)
 *
 * Returns:
 *	0	if unable to send
 *	1	packet was sent successfully
 */
int mld_query(struct in6_addr dst, struct in6_addr grp,__u8 mrt)
{

#if defined (CONFIG_MLDV2_SUPPORT)	
	struct mld2_query	*mldv2= (struct mld2_query *)mld6_send_buf;;
#else
	struct mld_msg *mldv1=(struct mld_msg *)mld6_send_buf;
#endif
	struct sockaddr_in6 dst_sa;
	struct IfDesc *down_dp=getIfByName(mld_down_if_name);
	if(down_dp==NULL)
	{
		printf("get if(%s) failed\n",mld_down_if_name);
		return 0;
	}	
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	/*
	dp= getIfByName(mld_down_if_name);

	if(dp == NULL)
	{
		printf("get if(%s) failed\n",mld_down_if_name);
		return 0;
	}
	*/
	/*
	printf("mldquery info:\n");
	printf( "dstaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	dst.s6_addr16[ 0],dst.s6_addr16[ 1],dst.s6_addr16[ 2],dst.s6_addr16[ 3],
	dst.s6_addr16[ 4],dst.s6_addr16[ 5],dst.s6_addr16[ 6],dst.s6_addr16[ 7]);
	printf( "grpaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	grp.s6_addr16[ 0],grp.s6_addr16[ 1],grp.s6_addr16[ 2],grp.s6_addr16[ 3],
	grp.s6_addr16[ 4],grp.s6_addr16[ 5],grp.s6_addr16[ 6],grp.s6_addr16[ 7]);
	printf("Make mld query![%s]:[%d].\n",__FUNCTION__,__LINE__);
	*/
//make packet
#if defined (CONFIG_MLDV2_SUPPORT)
	//printf("Make mldv2![%s]:[%d].\n",__FUNCTION__,__LINE__);
	memset(mldv2,0,sizeof(struct mld_msg));

	int 	totalsize=0;
   //mldv2			  = (struct mld2_query *)mld6_send_buf;
	mldv2->mld2q_type= ICMPV6_MGM_QUERY;
	mldv2->mld2q_code=0;
	mldv2->mld2q_mrc = mrt;
	memcpy(&(mldv2->mld2q_mca), &grp, sizeof(struct in6_addr));
	mldv2->mld2q_cksum		= 0;
	mldv2->mld2q_resv1	   = 0;
	mldv2->mld2q_suppress  = 1;
	mldv2->mld2q_qrv	   = 2;
	mldv2->mld2q_qqic	   = PERIODICAL_GENERAL_QUERY_INTERVAL;
	mldv2->mld2q_nsrcs	   = 0;
	totalsize		  = sizeof(struct mld2_query)+mldv2->mld2q_nsrcs*sizeof(struct in6_addr);
	//mldv2->mld2q_cksum	 = in_cksum((u_short *)mldv2, totalsize );
	//printf("sizeof(struct mld2_query):%d, totalsize:%d",sizeof(struct mld2_query), totalsize);
#else

	//printf("Make mldv1![%s]:[%d].\n",__FUNCTION__,__LINE__);
	memset(mldv1,0,sizeof(struct mld_msg));
	mldv1->mld_type 	=ICMPV6_MGM_QUERY;
	mldv1->mld_code 	=0;//set to 1 when send and ingnore it when receive
	mldv1->mld_cksum	=0;
	mldv1->mld_maxdelay =mrt;
	memcpy(&(mldv1->mld_mca), &grp, sizeof(struct in6_addr));
	//printf("Make mldv1 end![%s]:[%d].\n",__FUNCTION__,__LINE__);
	//mldv1->mld_cksum	=in_cksum((u_short *)mldv1, MLD_MINLEN); //MLD_MINLEN 应该设置为多少??
#endif

	struct IfDesc  *dp[mld_if_num];
	int i;
	dp[0]=down_dp;

	for(i=0;i< mld_if_num;i++){
#ifdef CONFIG_MLDPROXY_MULTIWAN
		dp[i+1]= getIfByName(mld_up_if_name[i]);
#else
		dp[i+1]= getIfByName(mld_up_if_name);
#endif
	
		//dp= getIfByName(mld_up_if_name);

		if(dp[i] == NULL)
		{
		#ifdef CONFIG_MLDPROXY_MULTIWAN
			printf("get if(%s) failed\n",mld_up_if_name[i]);
		#else
			printf("get if(%s) failed\n",mld_up_if_name);
		#endif
			return 0;
		}
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

		int ctllen=0; 
		int hbhlen = 0;
		int alert=1;
		int ifindex=(int)dp[i]->pif_idx;
		struct in6_addr src;
		memcpy(&src,&(dp[i]->InAdr),sizeof(struct in6_addr));
		/*
		printf("Make header!![%s]:[%d].\n",__FUNCTION__,__LINE__);
		printf( "srcaddr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
		src.s6_addr16[ 0],src.s6_addr16[ 1],src.s6_addr16[ 2],src.s6_addr16[ 3],
		src.s6_addr16[ 4],src.s6_addr16[ 5],src.s6_addr16[ 6],src.s6_addr16[ 7]);
		*/
#if defined (CONFIG_MLDV2_SUPPORT)
		sndiov[0].iov_len = totalsize;
#else
		sndiov[0].iov_len = sizeof(struct mld_msg);
#endif
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
		}/*end if (alert)*/
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
		//printf("send mld_query![%s]:[%d].\n",__FUNCTION__,__LINE__);
		
		if (sendmsg(dp[i]->sock, &sndmh, 0) < 0) {
			printf("if:%s,errno:%d,SEND FAIL!!![%s]:[%d].\n",dp[i]->Name,errno,__FUNCTION__,__LINE__);
		
			if (errno == ENETDOWN){
				//check_vif_state();
				printf("ERRNO==ENETDOWN![%s]:[%d].\n",__FUNCTION__,__LINE__);
			}
			else
				log(LOG_ERR, errno, "MLD6 sendmsg.");
			return 0;
		}
 
	}
 return 1;

}
	



////////////////////////////////////////////////////////////////////////////////////


char* runPath = "/bin/mldproxy";
char* pidfile = "/var/run/mld_pid";


/*
 * On hangup, let everyone know we're going away.
 */
 
void hup(int signum)
{
	(void)signum;

	log( LOG_DEBUG, 0, "clean handler called" );
#ifdef	MLD_MCAST2UNI  
	Update_igmpProxyStateToKernel(0);
#endif
	disableMRouter();
	unlink(pidfile);
	exit(EXIT_SUCCESS);

}
#ifdef CONFIG_CHECK_MULTICASTROUTE
void singnalAlrm(int signum)
{
	(void)signum;
	int checkroute=0;
	
	checkroute=check_kernel_multicast_route(1);
	if(check_entry1==1 && checkroute==0){
		system("route add -net 224.0.0.0 netmask 240.0.0.0 dev br0 2> /dev/null");
		check_entry1=0;
	}
	checkroute = 0;
	checkroute=check_kernel_multicast_route(2);
	if( check_entry2==1 && checkroute ==0){
		system("route add -net 255.255.255.255 netmask 255.255.255.255 dev br0 2> /dev/null");
		check_entry2=0;
	}
	if(check_multicast_route ==1)
		check_multicast_route= 0;
			
}
#endif

// Kaohj added
// Comes here because upstream or downstream interface ip changed
// Usually, it is used by dynamic interface to sync its interface with
// the mldproxy local database.
void sigifup(int signum)
{
	(void)signum;
	struct ifreq IfVc[ MAX_IF  ];
	struct ifreq *IfEp, *IfPt;
	struct ifconf IoCtlReq;
	struct IfDesc *Dup, *Ddp;
	int ifindex;
	int Sock;
	#if 1
	syslog(LOG_INFO, "mldproxy: SIGUSR1 caught\n");
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	// get information of all the interfaces
	if( (Sock = socket( AF_INET6, SOCK_DGRAM, 0 )) < 0 ){
		printf("RAW socket open![%s]:[%d].\n",__FUNCTION__,__LINE__);
		log( LOG_ERR, errno, "RAW socket open" );
	}
	IoCtlReq.ifc_buf = (void *)IfVc;
	IoCtlReq.ifc_len = sizeof( IfVc );
	//ioctl获取interface的信息ipv6?只适用于ipv4,ipv6可能不能获取
	if( ioctl( Sock, SIOCGIFCONF, &IoCtlReq ) < 0 ){
		printf("ioctl SIOCGIFCONF Fail![%s]:[%d].\n",__FUNCTION__,__LINE__);
		log( LOG_ERR, errno, "ioctl SIOCGIFCONF" );
	}
	close( Sock );
	IfEp = (void *)((char *)IfVc + IoCtlReq.ifc_len);
	#else

	{
	register struct uvif *v;
	int i;
	struct sockaddr_in6 addr;
	struct in6_addr mask;
	FILE			*file;
	unsigned int		prefixlen, scope, flags;
	char			devname[IFNAMSIZ];
	unsigned int		ifindex = 0;
	int total_interfaces= 0;	/* The total number of physical interfaces */	
	file = fopen("/proc/net/if_inet6", "r");

	/* We can live without it though */
	if (!file)
	{
		printf("error!\n");
		//log_msg(LOG_ERR, errno, "Couldn't open /proc/net/if_inet6\n");
		return;
	}
	
	/*
	 * Loop through all of the interfaces.
	 */
	while ((fscanf( file,
			"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx %x %x %x %x %8s",
			&addr.sin6_addr.s6_addr[ 0], &addr.sin6_addr.s6_addr[ 1], &addr.sin6_addr.s6_addr[ 2], &addr.sin6_addr.s6_addr[ 3],
			&addr.sin6_addr.s6_addr[ 4], &addr.sin6_addr.s6_addr[ 5], &addr.sin6_addr.s6_addr[ 6], &addr.sin6_addr.s6_addr[ 7],
			&addr.sin6_addr.s6_addr[ 8], &addr.sin6_addr.s6_addr[ 9], &addr.sin6_addr.s6_addr[10], &addr.sin6_addr.s6_addr[11],
			&addr.sin6_addr.s6_addr[12], &addr.sin6_addr.s6_addr[13], &addr.sin6_addr.s6_addr[14], &addr.sin6_addr.s6_addr[15],			
			&ifindex, &prefixlen, &scope, &flags, devname)!=EOF)){
		/*
		printf("ifa->name:%s\n",devname);
		printf( "ifindex:%x,prefixlen:%x,scope:%x,flags:%x\n",ifindex,prefixlen,scope,flags);
		printf( "addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
			addr.sin6_addr.s6_addr16[ 0],addr.sin6_addr.s6_addr16[ 1],addr.sin6_addr.s6_addr16[ 2],
			addr.sin6_addr.s6_addr16[ 3],addr.sin6_addr.s6_addr16[ 4],addr.sin6_addr.s6_addr16[ 5],
			addr.sin6_addr.s6_addr16[ 6],addr.sin6_addr.s6_addr16[ 7]);
		*/
		if (strcmp(devname,"lo")==0)
				continue;
		if (strcmp(devname,"peth0")==0)
				continue;

		/* get If vector
		**  RFC 2710 mandates the use of a link-local IPv6 source address for the transmission of MLD message
		**  so we just need to record the link-local address
		*/
	    
	 	if (IS_IPV6_LINKLOCAL_ADDRESS(addr.sin6_addr.s6_addr32))//[0]& htonl(0xffc00000)) == htonl (0xfe800000)){
		{
			char FmtBu[ 128 ];
			IfPt->ifr_addr.sa_family=AF_INET6;
			strncpy(IfPt->ifr_name,devname,IFNAMSIZ);
			IfPt->ifr_ifindex=ifindex;
			strncpy(IfDescEp->Name,devname,sizeof( IfDescEp->Name ));
			memcpy(&IfDescEp->InAdr,&addr.sin6_addr,sizeof(struct in6_addr));
			IfDescEp->Flags = flags;
			IfDescEp->pif_idx=ifindex;
			/*
			printf("buildIfVc: Interface %s pif_idx:%d, Flags: 0x%04x",
				   IfDescEp->Name,
				   IfDescEp->pif_idx,
				  IfDescEp->Flags );
			printf( "addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
			IfDescEp->InAdr.s6_addr16[ 0],IfDescEp->InAdr.s6_addr16[ 1],IfDescEp->InAdr.s6_addr16[ 2],
			IfDescEp->InAdr.s6_addr16[ 3],IfDescEp->InAdr.s6_addr16[ 4],IfDescEp->InAdr.s6_addr16[ 5],
			IfDescEp->InAdr.s6_addr16[ 6],IfDescEp->InAdr.s6_addr16[ 7]);
			*/
			IfDescEp++; 
			IfPt++;	
		}
		else
		{
			
		}
	}

	fclose(file);
	}
	#endif
#ifdef CONFIG_MLDPROXY_MULTIWAN
        int index;
     // get descriptors of upstream and downstream interfaces
	
	Ddp = getIfByName(mld_down_if_name);
	if ( Ddp == NULL)
		return;
	
	// update upstream/downstream interface ip into local database
	for( IfPt = IfVc; IfPt < IfEp; IfPt++ ) {
		
		for(index = 0;index<mld_up_if_num;index++)
		{
			Dup = getIfByName(mld_up_if_name[index]);
			
			if ( Dup == NULL)
				continue;  
			
			mld_up_if_idx[index] = addVIF(Dup);
			if (!strcmp(IfPt->ifr_name, Dup->Name)) {
				//Dup->InAdr = ((struct sockaddr_in6 *)&IfPt->ifr_addr)->sin6_addr;
				//memcpy(&Dup->InAdr,&(((struct sockaddr_in6 *)&IfPt->ifr_addr)->sin6_addr),sizeof( struct in6_addr));
				Dup->pif_idx=IfPt->ifr_ifindex;
				ifindex=(int)IfPt->ifr_ifindex;	//init when build ifvc
				//printf("update upstream ip to %s\n", inet_ntoa(Dup->InAdr));
				// Update default multicast interface for this socket.
				setsockopt(Dup->sock, IPPROTO_IPV6 , IPV6_MULTICAST_IF, 
					(void*)&ifindex, sizeof(int));
			}
		}
		
		if (!strcmp(IfPt->ifr_name, Ddp->Name)) {
			//Ddp->InAdr = ((struct sockaddr_in6 *)&IfPt->ifr_addr)->sin6_addr;
			//printf("update downstream ip to %s\n", inet_ntoa(Ddp->InAdr));
			// Update default multicast interface for this socket.
			Dup->pif_idx=IfPt->ifr_ifindex;
			ifindex=(int)IfPt->ifr_ifindex;	
			//IPPROTO_IP   这个应该对应于ipv6的什么?
			setsockopt(Ddp->sock, IPPROTO_IPV6 , IPV6_MULTICAST_IF, 
				(void*)&ifindex, sizeof(int));
		}
	}

#else
	// get descriptors of upstream and downstream interfaces
	Dup = getIfByName(mld_up_if_name);
	Ddp = getIfByName(mld_down_if_name);
	if (Dup == NULL || Ddp == NULL){
		printf("dp=null![%s]:[%d].\n",__FUNCTION__,__LINE__);
		return;
	}
	// update upstream/downstream interface ip into local database
	for( IfPt = IfVc; IfPt < IfEp; IfPt++ ) {
		if (!strcmp(IfPt->ifr_name, Dup->Name)) {
			
			//memcpy(&Dup->InAdr,&(((struct sockaddr_in6 *)&IfPt->ifr_addr)->sin6_addr),sizeof( struct in6_addr));
			Dup->pif_idx=IfPt->ifr_ifindex;
			ifindex=(int)IfPt->ifr_ifindex;
			//printf("update upstream ip to %s\n", inet_ntoa(Dup->InAdr));
			// Update default multicast interface for this socket.
			if(setsockopt(Dup->sock, IPPROTO_IPV6 , IPV6_MULTICAST_IF, 
				(void*)&ifindex, sizeof(int))<0){
				printf("setsockopt Dup->sock Failed!![%s]:[%d].\n",__FUNCTION__,__LINE__);
			}
		}
		else if (!strcmp(IfPt->ifr_name, Ddp->Name)) {
			
			//memcpy(&Ddp->InAdr,&(((struct sockaddr_in6 *)&IfPt->ifr_addr)->sin6_addr),sizeof( struct in6_addr));
			Dup->pif_idx=IfPt->ifr_ifindex;
			ifindex=(int)IfPt->ifr_ifindex;
			//printf("update downstream ip to %s\n", inet_ntoa(Ddp->InAdr));
			// Update default multicast interface for this socket.
			if(setsockopt(Ddp->sock, IPPROTO_IPV6 , IPV6_MULTICAST_IF, 
				(void*)&ifindex, sizeof(int))<0){
				printf("setsockopt Ddp->sock Failed!![%s]:[%d].\n",__FUNCTION__,__LINE__);
			}
		}
	}
#endif
}

static int initMRouter(void)
/*
** Inits the necessary resources for MRouter.
**
*/
{
	int Err;
	int i;
	struct IfDesc *Ddp, *Dup;
	int proxyUp = 0;
	// get information of interface  
	if(!buildIfVc()) 
		return 0;
	rtalert_code = htons(IP6OPT_RTALERT_MLD);
    if (!mld6_recv_buf && (mld6_recv_buf = malloc(RECV_BUF_SIZE)) == NULL)
	    log(LOG_ERR, 0, "malloc failed");
    if (!mld6_send_buf && (mld6_send_buf = malloc(RECV_BUF_SIZE)) == NULL)
	    log(LOG_ERR, 0, "malloc failed");

    rcvcmsglen = CMSG_SPACE(sizeof(struct in6_pktinfo)) +
	    CMSG_SPACE(sizeof(int));
    if (rcvcmsgbuf == NULL && (rcvcmsgbuf = malloc(rcvcmsglen)) == NULL)
	    log(LOG_ERR, 0,"malloc failed");
    
    
	switch( Err = enableMRouter() ) {
		case 0: break;
		case EADDRINUSE: log( LOG_ERR, EADDRINUSE, "MC-Router API already in use" ); break;
		default: log( LOG_ERR, Err, "MRT_INIT failed" );
	}
	{
	int on=1;
	int ret;
	
	ret = setsockopt(MLD_Socket, IPPROTO_IPV6 , IPV6_MULTICAST_HOPS, 
		(void*)&on, sizeof(on));
	if(ret)
		printf("setsockopt IPV6_MULTICAST_HOPS error!\n");
	on=1;
#ifdef IPV6_RECVPKTINFO

    if (setsockopt(MLD_Socket, IPPROTO_IPV6, IPV6_RECVPKTINFO, &on,
		   sizeof(on)) < 0)
		log(LOG_ERR, errno, "setsockopt(IPV6_RECVPKTINFO)");
#else  /* old adv. API */
    if (setsockopt(MLD_Socket, IPPROTO_IPV6, IPV6_PKTINFO, &on,
		   sizeof(on)) < 0)
		log(LOG_ERR, errno, "setsockopt(IPV6_PKTINFO)");
#endif 
	 on = 1;
    /* specify to tell value of hoplimit field of received IP6 hdr */
#ifdef IPV6_RECVHOPLIMIT

    if (setsockopt(MLD_Socket, IPPROTO_IPV6, IPV6_RECVHOPLIMIT, &on,
		   sizeof(on)) < 0)
		log(LOG_ERR, errno, "setsockopt(IPV6_RECVHOPLIMIT)");
#else  /* old adv. API */
    if (setsockopt(MLD_Socket, IPPROTO_IPV6, IPV6_HOPLIMIT, &on,
		   sizeof(on)) < 0)
		log(LOG_ERR, errno, "setsockopt(IPV6_HOPLIMIT)");
#endif 
	    /* initialize msghdr for receiving packets */
    rcviov[0].iov_base = (caddr_t) mld6_recv_buf;
    rcviov[0].iov_len = RECV_BUF_SIZE;
    rcvmh.msg_name = (caddr_t) & from;
    rcvmh.msg_namelen = sizeof(from);
    rcvmh.msg_iov = rcviov;
    rcvmh.msg_iovlen = 1;
    rcvmh.msg_control = (caddr_t) rcvcmsgbuf;
    rcvmh.msg_controllen = rcvcmsglen;

    /* initialize msghdr for sending packets */
    sndiov[0].iov_base = (caddr_t)mld6_send_buf;
	sndmh.msg_name = (caddr_t) & to;
    sndmh.msg_namelen =sizeof(to);
    sndmh.msg_iov = sndiov;
    sndmh.msg_iovlen = 1;
    /* specifiy to insert router alert option in a hop-by-hop opt hdr. */
#ifndef USE_RFC2292BIS
    raopt[0] = IP6OPT_ROUTER_ALERT;
    raopt[1] = IP6OPT_RTALERT_LEN - 2;
    memcpy(&raopt[2], (caddr_t) & rtalert_code, sizeof(u_int16_t));
#endif 
	
	/* In linux use IP_PKTINFO */
	//IP_RECVIF returns the interface of received datagram
	
	}
#ifdef CONFIG_MLDPROXY_MULTIWAN
    Ddp = getIfByName(mld_down_if_name);
	
	if (Ddp==NULL )
		return 0;
	
	/* add downstream interface */
	mld_down_if_idx = addVIF(Ddp);

	/* add upstream interface */
	for(i = 0;i<mld_up_if_num;i++)
	{
		Dup = getIfByName(mld_up_if_name[i]);
		if ( Dup == NULL)
		   continue;  
		else
			proxyUp = 1;
		mld_up_if_idx[i] = addVIF(Dup);
	 }

	if(proxyUp == 0)
	{		
		return 0;
	}
#else
	Ddp = getIfByName(mld_down_if_name);
	Dup = getIfByName(mld_up_if_name);
	if (Ddp==NULL || Dup==NULL)
		return 0;
	/*
	printf("add down VIF: Interface %s ,pif_idx:%d, Flags: 0x%04x",
		   Ddp->Name,
		   Ddp->pif_idx,
		   Ddp->Flags );
	printf( "addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	Ddp->InAdr.s6_addr16[ 0],Ddp->InAdr.s6_addr16[ 1],Ddp->InAdr.s6_addr16[ 2],
	Ddp->InAdr.s6_addr16[ 3],Ddp->InAdr.s6_addr16[ 4],Ddp->InAdr.s6_addr16[ 5],
	Ddp->InAdr.s6_addr16[ 6],Ddp->InAdr.s6_addr16[ 7]);
	printf("add up VIF: Interface %s ,pif_idx:%d, Flags: 0x%04x",
		   Dup->Name,
		   Dup->pif_idx,
		   Dup->Flags );
	printf( "addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
	Dup->InAdr.s6_addr16[ 0],Dup->InAdr.s6_addr16[ 1],Dup->InAdr.s6_addr16[ 2],
	Dup->InAdr.s6_addr16[ 3],Dup->InAdr.s6_addr16[ 4],Dup->InAdr.s6_addr16[ 5],
	Dup->InAdr.s6_addr16[ 6],Dup->InAdr.s6_addr16[ 7]);
	*/
	/* add downstream interface */
	mld_down_if_idx = addVIF(Ddp);
	//printf("[%s]:[%d].mld_down_if_idx:%d\n",__FUNCTION__,__LINE__,mld_down_if_idx);
	/* add upstream interface */
	mld_up_if_idx = addVIF(Dup);
	//printf("[%s]:[%d].mld_up_if_idx:%d\n",__FUNCTION__,__LINE__,mld_down_if_idx);
#endif
	
	signal(SIGTERM, hup);
  //atexit( clean );
  	return 1;
}

void
write_pid()
{
	FILE *fp = fopen(pidfile, "w+");
	if (fp) {
		//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
		fprintf(fp, "%d\n", getpid());
		fclose(fp);
	}
	else
	 	printf("Cannot create pid file\n");
}

void
clear_pid()
{
	FILE *fp = fopen(pidfile, "w+");
	if (fp) {
		fprintf(fp, "%d\n", 0);
		fclose(fp);
	}
	else
	 	printf("Cannot create pid file\n");
}


extern int MLD_Socket;

void callback_usr2()
{	
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	linkChangeQueryTimer.type=LIMIT_RETRY_TIMER_TYPE;
	linkChangeQueryTimer.retry_left=LINK_CHANGE_QUERY_TIMES;
	linkChangeQueryTimer.timerInterval=LINK_CHANGE_QUERY_INTERVAL;
	timeout(mld_general_query_timer_expired , &linkChangeQueryTimer, linkChangeQueryTimer.timerInterval, &linkChangeQueryTimer.ch);
}

int main(int argc, char **argv)
{
	int _argc = 0;
#ifdef CONFIG_MLDPROXY_MULTIWAN
    char *_argv[12];
	int index;		
#else
	char *_argv[5];
#endif
	pid_t pid;
	int execed = 0;
	char cmdBuffer[50];//Brad add 20080605
	struct IfDesc *IfDp;
	int flags;
#ifdef CONFIG_MLDPROXY_MULTIWAN
	if (argc >= 12) {
#else
	if (argc >= 5) {
#endif
		fprintf(stderr, "To many arguments \n");
		exit(1);
	}
	
	if (strcmp(argv[argc-1], "-D") == 0) {
		argc--;
		execed = 1;
	}
	
	if(argc < 2) {
		printf("Usage: mldproxy <up interface> [down interface]\n\n");
		return;
	}

	if (!execed) {
		if ((pid = vfork()) < 0) {
			printf("[%s]:[%d].vfork failed\n",__FUNCTION__,__LINE__);
			fprintf(stderr, "vfork failed\n");
			exit(1);
		} else if (pid != 0) {
			//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
			exit(0);
		}
		
		for (_argc=0; _argc < argc; _argc++ )
			_argv[_argc] = argv[_argc];
		_argv[0] = runPath;
		_argv[argc++] = "-D";
		_argv[argc++] = NULL;
		execv(_argv[0], _argv);
		/* Not reached */
		fprintf(stderr, "Couldn't exec\n");
		_exit(1);

	} else {
		setsid();
	}
	
#ifdef CONFIG_MLDPROXY_MULTIWAN

    if(argc == 2)
		strcpy(mld_down_if_name, "eth0");
	else
		strcpy(mld_down_if_name, argv[argc-1]);
	
    for(index=1;index<argc-1;index++)
		strcpy(mld_up_if_name[index-1], argv[index]);
		
	mld_up_if_num=argc-2;
	mld_if_num=mld_up_if_num+1;
#else 
	if(argc == 2)
		strcpy(mld_down_if_name, "eth0");
	else
		strcpy(mld_down_if_name, argv[2]);
		
	strcpy(mld_up_if_name, argv[1]);
	mld_if_num=2;
#endif

#ifdef CONFIG_MLDPROXY_MULTIWAN	
	for(index=0;index<mld_up_if_num;index++)
	{
		memset(cmdBuffer, '\0',sizeof(cmdBuffer));
		sprintf(cmdBuffer, "echo %s > /var/mld_up", mld_up_if_name[index]);
		system(cmdBuffer);
	}
		
#else

	//Brad add 20080605			
	memset(cmdBuffer, '\0',sizeof(cmdBuffer));
	sprintf(cmdBuffer, "echo %s > /var/mld_up", mld_up_if_name);
	system(cmdBuffer);
#endif
	write_pid();
	#if defined(CONFIG_MLDV2_SUPPORT)
	system("echo 2 > /proc/br_mldVersion");		
	#else
	system("echo 1 > /proc/br_mldVersion");
	#endif
	/*here to set mld proxy daemon pid to kernel*/
	{
		int br_socket_fd = -1;
		unsigned long arg[3];

		arg[0] = BRCTL_SET_MLDPROXY_PID;	
		arg[1] = getpid();
		arg[2] = 0;

		if ((br_socket_fd = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
		{
			printf("create socket to connect bridge fail! \n");
			
		}
		else 
		{	
			ioctl(br_socket_fd, SIOCGIFBR, arg);
		}
		close(br_socket_fd);
	}
	
	signal(SIGHUP, hup);
	signal(SIGTERM, hup);
	signal(SIGUSR1, sigifup);
	signal(SIGUSR2, callback_usr2);
#ifdef CONFIG_CHECK_MULTICASTROUTE	
	signal(SIGALRM,singnalAlrm);
#endif
	while (!initMRouter())
	{
		// Kaohj, polling every 2 seconds
		//printf("initMRouter fail\n");
		sleep(2);
	}

	init_mld();

	//hyking:recv the sock for avoid dst cache refcnt issue.
	//2010-8-3
	//printf("[%s]:[%d]\n",__FUNCTION__,__LINE__);
	IfDp = getIfByName(mld_down_if_name);
	if((IfDp!=NULL)&&(IfDp->sock > 0))
	{
		add_fd(IfDp->sock);
		flags = fcntl(IfDp->sock, F_GETFL);
		if (flags == -1 || fcntl(IfDp->sock, F_SETFL, flags | O_NONBLOCK) == -1)
		   	printf("Couldn't set %s to nonblock\n",mld_down_if_name);

	}

#ifdef CONFIG_MLDPROXY_MULTIWAN
	for(index=0;index<mld_up_if_num;index++)
	{
		IfDp = getIfByName(mld_up_if_name[index]);
		if((IfDp!=NULL)&&(IfDp->sock > 0))
		{
			add_fd(IfDp->sock);
			flags = fcntl(IfDp->sock, F_GETFL);
			if (flags == -1 || fcntl(IfDp->sock, F_SETFL, flags | O_NONBLOCK) == -1)
			   	printf("Couldn't set sock of %s to nonblock\n",mld_up_if_name[index]);
		}
	}
		
#else
	IfDp = getIfByName(mld_up_if_name);
	if((IfDp!=NULL)&&(IfDp->sock > 0))
	{
		add_fd(IfDp->sock);
		flags = fcntl(IfDp->sock, F_GETFL);
		if (flags == -1 || fcntl(IfDp->sock, F_SETFL, flags | O_NONBLOCK) == -1)
		   	printf("Couldn't set sock of %s to nonblock\n",mld_up_if_name);
	}
#endif
	
	if(MLD_Socket>0)
	{
		add_fd(MLD_Socket);
		flags = fcntl(MLD_Socket, F_GETFL);
		if (flags == -1 || fcntl(MLD_Socket, F_SETFL, flags | O_NONBLOCK) == -1)
		   	printf("Couldn't set MLD_Socket to nonblock\n");

	}
#if defined(USE_STATIC_ENTRY_BUFFER)
	memset(mcft_entry_tbl, 0x00, sizeof(struct mcft_entry_en)*MAX_MFCT_ENTRY);
	memset(ifrec_entry_tbl,0x00, sizeof(struct ifrec_entry_en)*MAX_IFREC_ENTRY);
	memset(mbr_entry_tbl, 0x00, sizeof(struct mbr_entry_en)*MAX_MBR_ENTRY);
#if defined (CONFIG_MLDV2_SUPPORT)
	memset(src_entry_tbl,0x00, sizeof(struct src_entry_en)*MAX_SRC_ENTRY);
#endif
#endif
	
	DISPLAY_MLD_BANNER;
	/* process loop */
	/*2008-0919 add ,when l2pt disconnection or any reason ;when igmpProxy be restart
	should issue query first*/ 
	
	struct in6_addr group_0;
	memset(&group_0,0,sizeof(struct in6_addr));
	//printf("send query!!![%s]:[%d]\n",__FUNCTION__,__LINE__);
	mld_query(IS_MLD_ALL_HOSTS_ADDRESS, group_0, 1);

	while(1)
	{
		fd_set in;
		struct timeval tv;
		int ret;
		int recvlen;
		int    mld6_recvlen;
#ifdef CONFIG_MLDV2_SUPPORT
		mldv2_timer();
#endif
		calltimeout();		

		tv.tv_sec = 0;
		tv.tv_usec = 100000;
		
		in = in_fds;
		
		ret = select(max_in_fd+1, &in, NULL, NULL, &tv);

		if( ret <= 0 ){
			//printf("mld: timeout\n");
			continue;
		} 
		IfDp=NULL;
		//printf("ret=%d,[%s]:[%d]\n",ret,__FUNCTION__,__LINE__);
		if(FD_ISSET(MLD_Socket, &in_fds)) 
		{
		    mld6_recvlen = recvmsg(MLD_Socket, &rcvmh, 0);

		    if (mld6_recvlen < 0)
		    {
			if (errno != EINTR)
			    log(LOG_ERR, errno, "MLD6 recvmsg");
			return;
		    }
			//printf("mld6_recvlen:%d\n",mld6_recvlen);
#ifdef CONFIG_MLDV2_SUPPORT
			mldv2_accept(mld6_recvlen);
#else
		    mld_accept(mld6_recvlen);
#endif
		}
			
		//hyking:recv the sock for avoid dst cache refcnt issue.
		//2010-8-3
		IfDp =  getIfByName(mld_down_if_name);
		if((IfDp!=NULL)&&(FD_ISSET(IfDp->sock, &in_fds)))
		{
			recvlen = recvfrom(IfDp->sock,recv_buf,RECV_BUF_SIZE,
					   0, NULL, &recvlen);
			if (recvlen < 0) 
			{
		    		if (errno != EINTR && errno !=EAGAIN) 
						log(LOG_ERR, errno, "recvfrom down interface");
			}
			
			//printf("recvlen=%d,Receive pkts[%s]:[%d]\n",recvlen,__FUNCTION__,__LINE__);
			
		}
		
#ifdef CONFIG_MLDPROXY_MULTIWAN
		for(index=0;index<mld_up_if_num;index++)
		{
			IfDp =  getIfByName(mld_up_if_name[index]);		
			if((IfDp!=NULL)&&(FD_ISSET(IfDp->sock, &in_fds)))
			{
				recvlen = recvfrom(IfDp->sock,recv_buf,RECV_BUF_SIZE,
					   0, NULL, &recvlen);
				if (recvlen < 0) 
				{
			    		if (errno != EINTR && errno !=EAGAIN) log(LOG_ERR, errno, "recvfrom up interface");
				}
			}
		}
			
#else
			IfDp =  getIfByName(mld_up_if_name);		
			if((IfDp!=NULL)&&(FD_ISSET(IfDp->sock, &in_fds)))
			{
				recvlen = recvfrom(IfDp->sock,recv_buf,RECV_BUF_SIZE,
					   0, NULL, &recvlen);
				if (recvlen < 0) 
				{
			    		if (errno != EINTR && errno !=EAGAIN) log(LOG_ERR, errno, "recvfrom up interface");
				}
			}
		
#endif
		
	}
	printf("!!!JUMP out of loop!!!\n[%s]:[%d]\n",__FUNCTION__,__LINE__);
	return 0;
}



