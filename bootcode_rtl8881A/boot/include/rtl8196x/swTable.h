/*
* ----------------------------------------------------------------
* Copyright  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
*
* Abstract: Switch core polling mode NIC header file.
*

*
* ---------------------------------------------------------------
*/


#ifndef _SWNIC_TABLE_H
#define _SWNIC_TABLE_H
#define CONFIG_RTL865XC 1

//#include <rtl_types.h>
//#include <rtl_errno.h>
//#include <rtl8650/asicregs.h>


void tableAccessForeword(uint32, uint32, void *);
int32 swTable_addEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 swTable_modifyEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 swTable_forceAddEntry(uint32 tableType, uint32 eidx, void *entryContent_P);
int32 swTable_readEntry(uint32 tableType, uint32 eidx, void *entryContent_P);

#ifdef ASIC_TABLE_DEBUG
#define RTL8651_MAC_NUMBER				6
#define RTL8651_PORT_NUMBER				RTL8651_MAC_NUMBER
#define RTL8651_PHYSICALPORTMASK			((1<<RTL8651_MAC_NUMBER)-1)

#define RTL865XC_NETIFTBL_SIZE			8

/* Private ACL rule type: */
#define RTL8651_ACL_IFSEL					0x06

/*	dummy acl type for qos	*/
#define	RTL8651_ACL_802D1P					0x1f


#define RTL8651_ACLTBL_ALL_TO_CPU			127  // This rule is always "To CPU"
#define RTL8651_ACLTBL_DROP_ALL				126 //This rule is always "Drop"
#define RTL8651_ACLTBL_PERMIT_ALL			125	// This rule is always "Permit"
#define RTL8651_ACLTBL_PPPOEPASSTHRU		124 //For PPPoE Passthru Only
#define RTL8651_ACLTBL_RESERV_SIZE			4	//this many of ACL rules are reserved for internal use


typedef struct rtl865x_tblAsicDrv_l2Param_s {
	ether_addr_t	macAddr;
	uint32 		memberPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
	uint32 		ageSec;
	uint32	 	cpu:1,
				srcBlk:1,
				isStatic:1,				
				nhFlag:1,
				fid:2,
				auth:1;

} rtl865x_tblAsicDrv_l2Param_t;

typedef struct rtl865x_tblAsicDrv_vlanParam_s {
	uint32 	memberPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
	uint32 	untagPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
	uint32  fid:2;
#if defined(CONFIG_RTL8196D)  || defined(CONFIG_RTL8881A)
	uint32  vid:12;
#endif
	
} rtl865x_tblAsicDrv_vlanParam_t;

typedef struct rtl865x_tblAsicDrv_intfParam_s {
	ether_addr_t macAddr;
	uint16 	macAddrNumber;
	uint16 	vid;
	uint32 	inAclStart, inAclEnd, outAclStart, outAclEnd;
	uint32 	mtu;
	uint32 	enableRoute:1,
			valid:1;
} rtl865x_tblAsicDrv_intfParam_t;

typedef struct {
	 /* word 0 */
#if defined(CONFIG_RTL8196D)  || defined(CONFIG_RTL8881A)
	uint32	vid:12;
#else
	uint32	reserved1:12;
#endif
	 
	uint32	fid:2;
	uint32     extEgressUntag  : 3;
	uint32     egressUntag : 6;
	uint32     extMemberPort   : 3;
	uint32     memberPort  : 6;

    /* word 1 */
    uint32          reservw1;
    /* word 2 */
    uint32          reservw2;
    /* word 3 */
    uint32          reservw3;
    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_vlanTable_t;

typedef struct {
    /* word 0 */
    uint32          mac18_0:19;
    uint32          vid		 : 12;
    uint32          valid       : 1;	
    /* word 1 */
    uint32         inACLStartL:2;	
    uint32         enHWRoute : 1;	
    uint32         mac47_19:29;

    /* word 2 */
    uint32         mtuL       : 3;
    uint32         macMask :3;	
    uint32         outACLEnd : 7;	
    uint32         outACLStart : 7;	
    uint32         inACLEnd : 7;	
    uint32         inACLStartH: 5;	
    /* word 3 */
    uint32          reserv10   : 20;
    uint32          mtuH       : 12;

    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} rtl865xc_tblAsic_netifTable_t;

#define CTAILQ_ENTRY(type)						\
struct {								\
	struct type *tqe_next;	/* next element */			\
	struct type **tqe_prev;	/* address of previous next element */	\
}

typedef struct _rtl8651_tblDrvAclRule_s {
	union {
		/* MAC ACL rule */
		struct {
			ether_addr_t _dstMac, _dstMacMask;
			ether_addr_t _srcMac, _srcMacMask;
			uint16 _typeLen, _typeLenMask;
		} MAC; 
		/* IFSEL ACL rule */
		struct {
			uint8 _gidxSel;
		} IFSEL; 
		/* IP Group ACL rule */
		struct {
			ipaddr_t _srcIpAddr, _srcIpAddrMask;
			ipaddr_t _dstIpAddr, _dstIpAddrMask;
			uint8 _tos, _tosMask;
			union {
				/* IP ACL rle */
				struct {
					uint8 _proto, _protoMask, _flagMask;// flag & flagMask only last 3-bit is meaning ful
#if 1 //chhuang: #ifdef RTL8650B
					uint32 _FOP:1, _FOM:1, _httpFilter:1, _httpFilterM:1, _identSrcDstIp:1, _identSrcDstIpM:1;
#endif /* RTL8650B */
					union {
						uint8 _flag;
						struct {
							uint8 pend1:5,
								 pend2:1,
								 _DF:1,	//don't fragment flag
								 _MF:1;	//more fragments flag
						} s;
					} un;							
				} ip; 
				/* ICMP ACL rule */
				struct {
					uint8 _type, _typeMask, _code, _codeMask;
				} icmp; 
				/* IGMP ACL rule */
				struct {
					uint8 _type, _typeMask;
				} igmp; 
				/* TCP ACL rule */
				struct {
					ether_addr_t _l2srcMac, _l2srcMacMask;	// for srcMac & destPort ACL rule
					uint8 _flagMask;
					uint16 _srcPortUpperBound, _srcPortLowerBound;
					uint16 _dstPortUpperBound, _dstPortLowerBound;
					union {
						uint8 _flag;
						struct {
							uint8 _pend:2,
								  _urg:1, //urgent bit
								  _ack:1, //ack bit
								  _psh:1, //push bit
								  _rst:1, //reset bit
								  _syn:1, //sync bit
								  _fin:1; //fin bit
						}s;
					}un;					
				}tcp; 
				/* UDP ACL rule */
				struct {
					ether_addr_t _l2srcMac, _l2srcMacMask;	// for srcMac & destPort ACL rule
					uint16 _srcPortUpperBound, _srcPortLowerBound;
					uint16 _dstPortUpperBound, _dstPortLowerBound;										
				}udp; 
			}is;
		}L3L4; 
#if 1 //chhuang: #ifdef RTL8650B
		/* Source filter ACL rule */
		struct {
			ether_addr_t _srcMac, _srcMacMask;
			uint16 _srcPort, _srcPortMask;
			uint16 _srcVlanIdx, _srcVlanIdxMask;
			ipaddr_t _srcIpAddr, _srcIpAddrMask;
			uint16 _srcPortUpperBound, _srcPortLowerBound;
			uint32 _ignoreL4:1, //L2 rule
				  	 _ignoreL3L4:1; //L3 rule
		} SRCFILTER;
		/* Destination filter ACL rule */
		struct {
			ether_addr_t _dstMac, _dstMacMask;
			uint16 _vlanIdx, _vlanIdxMask;
			ipaddr_t _dstIpAddr, _dstIpAddrMask;
			uint16 _dstPortUpperBound, _dstPortLowerBound;
			uint32 _ignoreL4:1, //L3 rule
				   _ignoreL3L4:1; //L2 rule
		} DSTFILTER;
#endif /* RTL8650B */
		struct {
			uint8	vlanTagPri;
		} VLANTAG;
	}un_ty;
	uint32	ruleType_:5;
	uint32	actionType_:4;
#if 1	/* RTL8650B */
	uint32  	pktOpApp:3;
#endif	/* RTL8650B */
	uint32	isEgressRateLimitRule_:1;
	uint32	naptProcessType:4;
	uint32	naptProcessDirection:2;
	uint32	matchType_;
	
	uint16	dsid; /* 2004/1/19 orlando */
	uint16	priority:3;
	uint32	dvid_:3;
	uint32	priority_:1;
	uint32	nextHop_:10; /* Index of L2 table */
	uint32	pppoeIdx_:3;
	uint32	isIPRange_:1;			/* support IP Range ACL */
	uint32	isRateLimitCounter_:1;	/* support Rate Limit Counter Mode */
#if 1 //chhuang: #ifdef RTL8650B
	uint16	nhIndex; /* Index of nexthop table (NOT L2 table) */
	uint16	rlIndex; /* Index of rate limit table */
#endif /* RTL8650B */

	uint32	aclIdx;
	CTAILQ_ENTRY(_rtl8651_tblDrvAclRule_s) nextRule;
} _rtl8651_tblDrvAclRule_t;

typedef struct {
    union {
        struct {
            /* word 0 */
            uint16          dMacP31_16;
            uint16          dMacP15_0;
            /* word 1 */
            uint16          dMacM15_0;
            uint16          dMacP47_32;
            /* word 2 */
            uint16          dMacM47_32;
            uint16          dMacM31_16;
            /* word 3 */
            uint16          sMacP31_16;
            uint16          sMacP15_0;
            /* word 4 */
            uint16          sMacM15_0;
            uint16          sMacP47_32;
            /* word 5 */
            uint16          sMacM47_32;
            uint16          sMacM31_16;
            /* word 6 */
            uint16          ethTypeM;
            uint16          ethTypeP;
        } ETHERNET;
        struct {
            /* word 0 */
            uint32          reserv1     : 24;
            uint32          gidxSel     : 8;
            /* word 1~6 */
            uint32          reserv2[6];
        } IFSEL;
        struct {
            /* word 0 */
            ipaddr_t        sIPP;
            /* word 1 */
            ipaddr_t        sIPM;
            /* word 2 */
            ipaddr_t        dIPP;
            /* word 3 */
            ipaddr_t        dIPM;
            union {
                struct {
                    /* word 4 */
                    uint8           IPProtoM;
                    uint8           IPProtoP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint32          reserv0     : 20;
                    uint32          identSDIPM  : 1;
                    uint32          identSDIPP  : 1;
                    uint32          HTTPM       : 1;
                    uint32          HTTPP       : 1;
                    uint32          FOM         : 1;
                    uint32          FOP         : 1;
                    uint32          IPFlagM     : 3;
                    uint32          IPFlagP     : 3;
                    /* word 6 */
                    uint32          reserv1;
                } IP;
                struct {
                    /* word 4 */
                    uint8           ICMPTypeM;
                    uint8           ICMPTypeP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          reserv0;
                    uint8           ICMPCodeM;
                    uint8           ICMPCodeP;
                    /* word 6 */
                    uint32          reserv1;
                } ICMP;
                struct {
                    /* word 4 */
                    uint8           IGMPTypeM;
                    uint8           IGMPTypeP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5,6 */
                    uint32          reserv0[2];
                } IGMP;
                struct {
                    /* word 4 */
                    uint8           TCPFlagM;
                    uint8           TCPFlagP;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          TCPSPLB;
                    uint16          TCPSPUB;
                    /* word 6 */
                    uint16          TCPDPLB;
                    uint16          TCPDPUB;
                } TCP;
                struct {
                    /* word 4 */
                    uint16          reserv0;
                    uint8           IPTOSM;
                    uint8           IPTOSP;
                    /* word 5 */
                    uint16          UDPSPLB;
                    uint16          UDPSPUB;
                    /* word 6 */
                    uint16          UDPDPLB;
                    uint16          UDPDPUB;
                } UDP;
            } is;
        } L3L4;

        struct {
            /* word 0 */
            uint16          sMacP31_16;
            uint16          sMacP15_0;
            /* word 1 */
            uint16          reserv1:3;
            uint16          spaP:9;
            uint16          sMacM3_0:4;
            uint16          sMacP47_32;
			/* word 2 */
		    uint32	        reserv3:2;
            uint32          sVidM:12;
		    uint32          sVidP:12;
		    uint32		    reserv2:6;
            /* word 3 */
            uint32          reserv5     : 6;
            uint32          protoType   : 2;
     	    uint32          reserv4        : 24;
			/* word 4 */
            ipaddr_t        sIPP;
            /* word 5 */
            ipaddr_t        sIPM;
            /* word 6 */
            uint16          SPORTLB;
            uint16          SPORTUB;
        } SRC_FILTER;
        struct {
            /* word 0 */
            uint16          dMacP31_16;
            uint16          dMacP15_0;
            /* word 1 */
		    uint16 	        vidP:12;	
            uint16          dMacM3_0:4;
            uint16          dMacP47_32;			
            /* word 2 */
		    uint32          reserv2:20;
		    uint32          vidM:12;			
            /* word 3 */
            uint32          reserv4     : 24;
            uint32          protoType   : 2;
		     uint32         reserv3:6;
            /* word 4 */
            ipaddr_t        dIPP;
            /* word 5 */
            ipaddr_t        dIPM;
            /* word 6 */
            uint16          DPORTLB;
            uint16          DPORTUB;
        } DST_FILTER;

    } is;

    /* word 7 */
    uint32          reserv0     : 5;
    uint32          pktOpApp    : 3;
    uint32          PPPoEIndex  : 3;
    uint32          vid         : 3;
    uint32          nextHop     : 10; //index of l2, next hop, or rate limit tables
    uint32          actionType  : 4;
    uint32          ruleType    : 4;

} rtl865xc_tblAsic_aclTable_t;

/* MAC ACL rule Definition */
#define dstMac_				un_ty.MAC._dstMac
#define dstMacMask_			un_ty.MAC._dstMacMask
#define srcMac_				un_ty.MAC._srcMac
#define srcMacMask_			un_ty.MAC._srcMacMask
#define typeLen_				un_ty.MAC._typeLen
#define typeLenMask_			un_ty.MAC._typeLenMask

/* IFSEL ACL rule Definition */
#define gidxSel_				un_ty.IFSEL._gidxSel

/* Common IP ACL Rule Definition */
#define srcIpAddr_				un_ty.L3L4._srcIpAddr
#define srcIpAddrMask_			un_ty.L3L4._srcIpAddrMask
#define srcIpAddrUB_				un_ty.L3L4._srcIpAddr
#define srcIpAddrLB_				un_ty.L3L4._srcIpAddrMask
#define dstIpAddr_				un_ty.L3L4._dstIpAddr
#define dstIpAddrMask_			un_ty.L3L4._dstIpAddrMask
#define dstIpAddrUB_				un_ty.L3L4._dstIpAddr
#define dstIpAddrLB_				un_ty.L3L4._dstIpAddrMask
#define tos_					un_ty.L3L4._tos
#define tosMask_				un_ty.L3L4._tosMask
/* IP Rrange */
/*Hyking:Asic use Addr to store Upper address
	and use Mask to store Lower address
*/

/* IP ACL Rule Definition */
#define ipProto_				un_ty.L3L4.is.ip._proto
#define ipProtoMask_			un_ty.L3L4.is.ip._protoMask
#define ipFlagMask_			un_ty.L3L4.is.ip._flagMask
#if 1 //chhuang: #ifdef RTL8650B
#define ipFOP_      				un_ty.L3L4.is.ip._FOP
#define ipFOM_      				un_ty.L3L4.is.ip._FOM
#define ipHttpFilter_      			un_ty.L3L4.is.ip._httpFilter
#define ipHttpFilterM_			un_ty.L3L4.is.ip._httpFilterM
#define ipIdentSrcDstIp_   		un_ty.L3L4.is.ip._identSrcDstIp
#define ipIdentSrcDstIpM_		un_ty.L3L4.is.ip._identSrcDstIpM
#endif /* RTL8650B */
#define ipFlag_				un_ty.L3L4.is.ip.un._flag
#define ipDF_					un_ty.L3L4.is.ip.un.s._DF
#define ipMF_					un_ty.L3L4.is.ip.un.s._MF

/* ICMP ACL Rule Definition */
#define icmpType_				un_ty.L3L4.is.icmp._type
#define icmpTypeMask_			un_ty.L3L4.is.icmp._typeMask	
#define icmpCode_				un_ty.L3L4.is.icmp._code
#define icmpCodeMask_			un_ty.L3L4.is.icmp._codeMask

/* IGMP ACL Rule Definition */
#define igmpType_				un_ty.L3L4.is.igmp._type
#define igmpTypeMask_			un_ty.L3L4.is.igmp._typeMask

/* TCP ACL Rule Definition */
#define tcpl2srcMac_				un_ty.L3L4.is.tcp._l2srcMac		// for srcMac & destPort ACL rule
#define tcpl2srcMacMask_			un_ty.L3L4.is.tcp._l2srcMacMask
#define tcpSrcPortUB_			un_ty.L3L4.is.tcp._srcPortUpperBound
#define tcpSrcPortLB_			un_ty.L3L4.is.tcp._srcPortLowerBound
#define tcpDstPortUB_			un_ty.L3L4.is.tcp._dstPortUpperBound
#define tcpDstPortLB_			un_ty.L3L4.is.tcp._dstPortLowerBound
#define tcpFlagMask_			un_ty.L3L4.is.tcp._flagMask
#define tcpFlag_				un_ty.L3L4.is.tcp.un._flag
#define tcpURG_				un_ty.L3L4.is.tcp.un.s._urg
#define tcpACK_				un_ty.L3L4.is.tcp.un.s._ack
#define tcpPSH_				un_ty.L3L4.is.tcp.un.s._psh
#define tcpRST_				un_ty.L3L4.is.tcp.un.s._rst
#define tcpSYN_				un_ty.L3L4.is.tcp.un.s._syn
#define tcpFIN_				un_ty.L3L4.is.tcp.un.s._fin

/* UDP ACL Rule Definition */
#define udpl2srcMac_				un_ty.L3L4.is.udp._l2srcMac		// for srcMac & destPort ACL rule
#define udpl2srcMacMask_		un_ty.L3L4.is.udp._l2srcMacMask
#define udpSrcPortUB_			un_ty.L3L4.is.udp._srcPortUpperBound
#define udpSrcPortLB_			un_ty.L3L4.is.udp._srcPortLowerBound
#define udpDstPortUB_			un_ty.L3L4.is.udp._dstPortUpperBound
#define udpDstPortLB_			un_ty.L3L4.is.udp._dstPortLowerBound

#if 1 //chhuang: #ifdef RTL8650B
/* Source Filter ACL Rule Definition */
#define srcFilterMac_				un_ty.SRCFILTER._srcMac
#define srcFilterMacMask_		un_ty.SRCFILTER._srcMacMask
#define srcFilterPort_				un_ty.SRCFILTER._srcPort
#define srcFilterPortMask_		un_ty.SRCFILTER._srcPortMask
#define srcFilterVlanIdx_			un_ty.SRCFILTER._srcVlanIdx
#define srcFilterVlanId_			un_ty.SRCFILTER._srcVlanIdx
#define srcFilterVlanIdxMask_		un_ty.SRCFILTER._srcVlanIdxMask
#define srcFilterVlanIdMask_		un_ty.SRCFILTER._srcVlanIdxMask
#define srcFilterIpAddr_			un_ty.SRCFILTER._srcIpAddr
#define srcFilterIpAddrMask_		un_ty.SRCFILTER._srcIpAddrMask
#define srcFilterIpAddrUB_		un_ty.SRCFILTER._srcIpAddr
#define srcFilterIpAddrLB_		un_ty.SRCFILTER._srcIpAddrMask
#define srcFilterPortUpperBound_	un_ty.SRCFILTER._srcPortUpperBound
#define srcFilterPortLowerBound_	un_ty.SRCFILTER._srcPortLowerBound
#define srcFilterIgnoreL3L4_		un_ty.SRCFILTER._ignoreL3L4
#define srcFilterIgnoreL4_		un_ty.SRCFILTER._ignoreL4

/* Destination Filter ACL Rule Definition */
#define dstFilterMac_				un_ty.DSTFILTER._dstMac
#define dstFilterMacMask_		un_ty.DSTFILTER._dstMacMask
#define dstFilterVlanIdx_			un_ty.DSTFILTER._vlanIdx
#define dstFilterVlanIdxMask_		un_ty.DSTFILTER._vlanIdxMask
#define dstFilterVlanId_			un_ty.DSTFILTER._vlanIdx
#define dstFilterVlanIdMask_		un_ty.DSTFILTER._vlanIdxMask
#define dstFilterIpAddr_			un_ty.DSTFILTER._dstIpAddr
#define dstFilterIpAddrMask_		un_ty.DSTFILTER._dstIpAddrMask
#define dstFilterPortUpperBound_	un_ty.DSTFILTER._dstPortUpperBound
#define dstFilterIpAddrUB_		un_ty.DSTFILTER._dstIpAddr
#define dstFilterIpAddrLB_		un_ty.DSTFILTER._dstIpAddrMask
#define dstFilterPortLowerBound_	un_ty.DSTFILTER._dstPortLowerBound
#define dstFilterIgnoreL3L4_		un_ty.DSTFILTER._ignoreL3L4
#define dstFilterIgnoreL4_		un_ty.DSTFILTER._ignoreL4
#endif /* RTL8650B */



/* ACL Rule Action type Definition */
#define RTL8651_ACL_PERMIT					0x01
#define RTL8651_ACL_DROP					0x02
#define RTL8651_ACL_CPU					0x03
#define RTL8651_ACL_DROP_LOG				0x04
#define RTL8651_ACL_DROP_NOTIFY			0x05
#define RTL8651_ACL_L34_DROP				0x06	/* special for default ACL rule */
#define RTL8651_ACL_DEFAULT_REDIRECT		0x08

/* Private ACL action type: */
#define RTL8651_ACL_REDIRECT				0x06
#define RTL8651_ACL_REDIRECT_PPPOE		0x07
#define RTL8651_ACL_MIRROR					0x08
#define RTL8651_ACL_MIRROR_KEEP_MATCH	0x09
#define RTL8651_ACL_DROP_RATE_EXCEED_PPS	0x0a
#define RTL8651_ACL_LOG_RATE_EXCEED_PPS	0x0b
#define RTL8651_ACL_DROP_RATE_EXCEED_BPS	0x0c
#define RTL8651_ACL_LOG_RATE_EXCEED_BPS	0x0d
#define RTL8651_ACL_POLICY					0x0e
#define RTL8651_ACL_PRIORITY				0x0f

#ifdef CONFIG_RTL_8881A
#define RTL8651_ACL_CHANGE_VID			0x0f
#define RTL8651_ACL_CHANGE_VID_ALIAS		0x04
#endif

/* ACL Rule type Definition */
#define RTL8651_ACL_MAC				0x00
#define RTL8651_ACL_IP					0x01
#define RTL8651_ACL_ICMP				0x02
#define RTL8651_ACL_IGMP				0x03
#define RTL8651_ACL_TCP					0x04
#define RTL8651_ACL_UDP				0x05

/* 6-8*/ 
#define RTL8652_ACL_IP_RANGE			0x0A
#define RTL8652_ACL_ICMP_IPRANGE		0x0B
#define RTL8652_ACL_TCP_IPRANGE		0x0C
#define RTL8652_ACL_IGMP_IPRANGE		0x0D
#define RTL8652_ACL_UDP_IPRANGE		0x0E
#define RTL8652_ACL_SRCFILTER_IPRANGE 0x09
#define RTL8652_ACL_DSTFILTER_IPRANGE 0x0F
#define RTL8651_ACL_SRCFILTER				0x07
#define RTL8651_ACL_DSTFILTER				0x08



/* For PktOpApp */
#define RTL8651_ACLTBL_BACKWARD_COMPATIBLE	0 /* For backward compatible */
#define RTL865XC_ACLTBL_ALL_LAYER			RTL8651_ACLTBL_BACKWARD_COMPATIBLE
#define RTL8651_ACLTBL_ONLY_L2				1 /* Only for L2 switch */
#define RTL8651_ACLTBL_ONLY_L3				2 /* Only for L3 routing (including IP multicast) */
#define RTL8651_ACLTBL_L2_AND_L3			3 /* Only for L2 switch and L3 routing (including IP multicast) */
#define RTL8651_ACLTBL_ONLY_L4				4 /* Only for L4 translation packets */
#define RTL8651_ACLTBL_L3_AND_L4			6 /* Only for L3 routing and L4 translation packets (including IP multicast) */
#define RTL8651_ACLTBL_NOOP				7 /* No operation. Don't apply this rule. */

/*	for NAPT process type	*/
/* it's value associated with the default napt process priority */
/* type must = priority */
/* 4 bits */
#define	RTL8651_ACL_L4_SERVERPORT	0x1 /* add flow for serverport match	*/
#define	RTL8651_ACL_L4_TRIGGERPORT	0x2	/* add flow for trigger port match	*/
#define	RTL8651_ACL_L4_UPNP			0x3	/* add flow for upnp match	*/
#define	RTL8651_ACL_L4_DMZ			0x5	/* add flow for dmz match	*/
#define	RTL8651_ACL_L4_GETEXTIP		0x6	/* add flow for get ip	*/

#define	RTL8651_ACL_L4_ICMP			0Xf		/* add flow for get ip	*/

/*	for Napt process direction	*/
/* 2 bits */
#define	RTL8651_ACL_L4_INBOUND			0x1
#define	RTL8651_ACL_L4_OUTBOUND		0x2


/* User ACL rule match type definition */
#define RTL8651_ACL_GENERIC		0x00
#define RTL8651_ACL_L4NEWFLOW		0x01
#endif

#endif /* _SWNIC_H */
