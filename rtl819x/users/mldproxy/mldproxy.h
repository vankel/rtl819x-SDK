#ifndef _MLDPROXY_H_
#define _MLDPROXY_H_
#define IPV6_READY_LOGO 1

#define IN6_IS_ADDR_MULTICAST(a) (((__const uint8_t *) (a))[0] == 0xff)

#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
	(IN6_IS_ADDR_MULTICAST(a)					      \
	 && ((((__const uint8_t *) (a))[1] & 0xf) == 0x2))


#ifdef CONFIG_MLDPROXY_MULTIWAN
#define  MAXWAN 8
#else 
#define  MAXWAN 1
#endif

#define VERSION_STR	"v1.2"

#define USE_STATIC_ENTRY_BUFFER 1
#define MAX_MFCT_ENTRY 128
#define MAX_MBR_ENTRY 512
#define MAX_IFREC_ENTRY MAX_IF
#define MAX_SRC_ENTRY MLDV2_MAX_SRCNUM
	
#define DISPLAY_MLD_BANNER \
	printf("\nMLD Proxy %s (%s).\n\n", VERSION_STR, BUILT_TIME)	
// Enable to do the group-specific query periodically
#define PERIODICAL_QUERY
// Send Group-specific query periodically (keepalive polling)
#define PERIODICAL_SPECIFIC_QUERY
// Enable to maintain the group members in order to do immediately leave
#define KEEP_GROUP_MEMBER
//#define CONFIG_CHECK_MULTICASTROUTE
/* IGMP timer and default values */
#define STARTUP_GENERAL_QUERY_INTERVAL			3// second
#define PERIODICAL_GENERAL_QUERY_INTERVAL		60
#define GENERAL_QUERY_INTERVAL		15	// second

#define LINK_CHANGE_QUERY_INTERVAL					3
#define LINK_CHANGE_QUERY_TIMES						3

#define LAST_MEMBER_QUERY_INTERVAL	1	// second
#define LAST_MEMBER_QUERY_COUNT		2
// Kaohj --- group-specific query in periodical
#ifdef PERIODICAL_QUERY
#define MEMBER_QUERY_INTERVAL		(PERIODICAL_GENERAL_QUERY_INTERVAL*2+10)	// second
#define MEMBER_QUERY_COUNT		3
#endif

/* IGMP group address */
#define ALL_SYSTEMS		htonl(0xE0000001)	// General Query - 224.0.0.1
#define ALL_ROUTERS		htonl(0xE0000002)	// Leave - 224.0.0.2	
#define ALL_ROUTERS_V3	htonl(0xE0000016)	// Leave - 224.0.0.22
#define ALL_PRINTER		htonl(0xEFFFFFFA)	// notify all printer - 239.255.255.250	//¶ÔÓ¦ÓÚ#define RESERVE_MULTICAST_ADDR1 	0xEFFFFFFA
#define CLASS_D_MASK	0xE0000000		// the mask that defines IP Class D
#define IPMULTI_MASK	0x007FFFFF		// to get the low-order 23 bits

/*MLD  group address*/
//struct in6_addr IS_MLD_ALL_HOSTS_ADDRESS={{{0xFF,2,0,0,0,0,0,0,0,0,0,0,0,0,0,1}}};
//struct in6_addr IS_MLD_ALL_ROUTER_ADDRESS={{{0xFF,2,0,0,0,0,0,0,0,0,0,0,0,0,0,2}}};
#define IS_MLDv2_REPORT_ADDRESS					{{{0xFF,2,0,0,0,0,0,0,0,0,0,0,0,0,0,16}}}
#define IS_IPV6_MULTICAST_ADDRESS(ipv6addr)	((ipv6addr[0] & 0xFF)==0xff)
#define IS_IPV6_LINKLOCAL_ADDRESS(ipv6addr)	((ipv6addr[0] & 0xffc00000)==(0xfe800000))
/* header length */

#define ICMPV6_MGM_QUERY		130
#define ICMPV6_MGM_REPORT       	131
#define ICMPV6_MGM_REDUCTION    	132

#define ICMPV6_MLD2_REPORT		143

#define IS_MLD_TYPE(type)	((type==ICMPV6_MGM_QUERY)||(type==ICMPV6_MGM_REPORT)||(type==ICMPV6_MGM_REDUCTION)||(type==ICMPV6_MLD2_REPORT))

#define MIN_IP_HEADER_LEN	40
#define MLD_MINLEN			24

#ifdef CONFIG_CHECK_MULTICASTROUTE
/* Keep this in sync with /usr/src/linux/include/linux/route.h */
#define RTF_UP			0x0001          /* route usable                 */
#define RTF_GATEWAY		0x0002          /* destination is a gateway     */
#define	RTF_HOST	0x0004		/* host entry (net otherwise)	*/
#endif
static char *routefile = "/proc/net/route";

#define PERIODICAL_TIMER_TPYE 		1
#define LIMIT_RETRY_TIMER_TYPE 		2
#define BRCTL_SET_MLDPROXY_PID 200

// Send Group-specific query on leave (fast leave)
//#define LEAVE_SPECIFIC_QUERY
// Maintain the group members in order to do immediately leave
#define KEEP_GROUP_MEMBER
// Send IGMP General Query periodically
#define SEND_GENERAL_QUERY

#define TIMER_GENERAL_QUERY		1
#define TIMER_DELAY_QUERY		2

/* IGMP group address */
#define ALL_SYSTEMS			htonl(0xE0000001)	// General Query - 224.0.0.1
#define ALL_ROUTERS			htonl(0xE0000002)	// Leave - 224.0.0.2	
#define ALL_ROUTERS_V3			htonl(0xE0000016)	// Leave - 224.0.0.22
#define ALL_PRINTER			htonl(0xEFFFFFFA)	// notify all printer - 239.255.255.250
#define CLASS_D_MASK			0xE0000000		// the mask that defines IP Class D
#define IPMULTI_MASK			0x007FFFFF		// to get the low-order 23 bits

/* header length */
//#define MIN_IP_HEADER_LEN		20
//#define MLD_MINLEN			8

/* IGMP v3 type */
#define IGMP_HOST_V3_MEMBERSHIP_REPORT	0x22
// group member entry
struct mbr_entry {
	struct mbr_entry	*next;
	struct in6_addr		user_addr;
	//int ifindex;
};

struct mld_timer {
	int		type;			// timer type
	__u32		timerInterval;
	__u32		retry_left;		// retry counts left
	struct callout	ch;
#ifdef CONFIG_MLDV2_SUPPORT
	unsigned int	lefttime;
#endif /*CONFIG_IGMPV3_SUPPORT*/
};

#ifdef CONFIG_MLDV2_SUPPORT
struct src_entry {
	struct src_entry	*next;
	struct in6_addr		srcaddr;
	struct mld_timer	timer;
	//int ifindex;
	int filter_mode;
};
#endif /*CONFIG_IGMPV3_SUPPORT*/
//interface member entry
/*
struct if_entry{
struct if_entry *next;
int ifindex;
};
*/
#if defined IPV6_READY_LOGO
struct ifrec_entry{
	
	struct if_rec_entry *next;
	__u32			user_count;	// group member count
	struct mbr_entry *grp_mbr;	//user entry
	struct in6_addr grp_addr;
	int ifindex;
	struct mld_timer timer;
#ifdef CONFIG_MLDV2_SUPPORT
	struct src_entry *srclist;
	int filter_mode;
	int	mld_ver;
	int			mrt_state;//0:disable, 1: enable all sources
#endif/*CONFIG_MLDV2_SUPPORT*/
};
#endif
struct mcft_entry {
	struct mcft_entry 	*next;
	struct in6_addr grp_addr;
	//__u32			user_count;	// group member count
	__u32			if_count;	//group interface count
	//struct mbr_entry 	*grp_mbr;//user entry
	struct ifrec_entry		*iflist; //if entry
	struct mld_timer	timer;
#ifdef CONFIG_MLDV2_SUPPORT
	
	int			filter_mode;
	int	mld_ver;
	struct src_entry	*srclist;
	int			mrt_state; //0:disable, 1: enable all sources
#endif /*CONFIG_MLDV2_SUPPORT*/
};


#ifdef CONFIG_MLDV2_SUPPORT
/*IGMP version*/
#define MLD_VER_1		1
#define MLD_VER_2		2
//extern int igmpv3_accept(int recvlen, struct IfDesc *dp);
//extern void igmpv3_timer(void);
//extern int mldv2_accept(int recvlen, struct IfDesc *dp);
extern int mldv2_accept(int recvlen);

extern void mldv2_timer(void);

#endif /*CONFIG_IGMPV3_SUPPORT*/

extern char mld_down_if_name[IFNAMSIZ];
extern char mld_down_if_idx;

#ifdef CONFIG_MLDPROXY_MULTIWAN
extern char mld_up_if_name[MAXWAN][IFNAMSIZ];
#else
extern char mld_up_if_name[IFNAMSIZ];
#endif

#ifdef CONFIG_MLDPROXY_MULTIWAN
extern char mld_up_if_idx[MAXWAN];
#else
extern char mld_up_if_idx;
#endif

#ifdef CONFIG_MLDPROXY_MULTIWAN
extern int mld_up_if_num;
#endif

extern char *recv_buf;
extern char *send_buf;
extern char *mld6_recv_buf;		/* input packet buffer */
extern char *mld6_send_buf;		/* output packet buffer */
extern struct msghdr 		sndmh, rcvmh;
extern struct sockaddr_in6 	from;
extern struct sockaddr_in6 	to;
extern struct iovec 		rcviov[2];
extern struct iovec 		sndiov[2];

extern int rcvcmsglen;
//static u_char   		*rcvcmsgbuf = NULL;


#ifndef USE_RFC2292BIS
#define IP6OPT_RTALERT_LEN 4
extern u_int8_t raopt[IP6OPT_RTALERT_LEN];
#endif 
#define IP6OPT_RTALERT_MLD      0
#define IP6OPT_RTALERT          0x05
#ifndef IP6OPT_ROUTER_ALERT	/* XXX to be compatible older systems */
#define IP6OPT_ROUTER_ALERT IP6OPT_RTALERT
#endif

extern char *sndcmsgbuf;
//extern int ctlbuflen = 0;



extern struct mcft_entry *mcpq;

extern int mld_query(struct in6_addr dst, struct in6_addr grp,__u8 mrt);

//extern struct mcft_entry * add_mcft(struct in6_addr grp_addr, struct in6_addr src_addr);
extern struct mcft_entry * add_mcft(struct in6_addr grp_addr, struct in6_addr src_addr,int ifindex);
#if defined IPV6_READY_LOGO
extern int del_mcft(struct in6_addr grp_addr,int ifindex);
#else
extern int del_mcft(struct in6_addr grp_addr);
#endif
extern int chk_mcft(struct in6_addr grp_addr);
extern struct mcft_entry * get_mcft(struct in6_addr grp_addr);



#endif /*_IGMPPROXY_H_*/
