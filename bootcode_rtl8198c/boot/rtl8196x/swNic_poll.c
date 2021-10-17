/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
*
* Abstract: Switch core polling mode NIC driver source code.
*
*
* ---------------------------------------------------------------
*/
#include <linux/config.h>
#include <rtl_types.h>
#include <rtl_errno.h>
#include <rtl8196x/loader.h> //wei edit
#include <rtl8196x/asicregs.h>
#include <rtl8196x/swNic_poll.h>

#ifdef CONFIG_NIC_LOOPBACK
#define _FAST_TX		1
#endif

/* refer to rtl865xc_swNic.c & rtl865xc_swNic.h
 */
#define RTL865X_SWNIC_RXRING_MAX_PKTDESC	6
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
#define RTL865X_SWNIC_TXRING_MAX_PKTDESC	4
#else
#define RTL865X_SWNIC_TXRING_MAX_PKTDESC	2
#endif



/* RX Ring */
static uint32*	rxPkthdrRing[RTL865X_SWNIC_RXRING_MAX_PKTDESC];					/* Point to the starting address of RX pkt Hdr Ring */
static uint32	rxPkthdrRingCnt[RTL865X_SWNIC_RXRING_MAX_PKTDESC];				/* Total pkt count for each Rx descriptor Ring */
//static uint32	rxPkthdrRingIndex[RTL865X_SWNIC_RXRING_MAX_PKTDESC];			/* Current Index for each Rx descriptor Ring */

/* TX Ring */
static uint32*	txPkthdrRing[RTL865X_SWNIC_TXRING_MAX_PKTDESC];				/* Point to the starting address of TX pkt Hdr Ring */

static uint32	txPkthdrRingCnt[RTL865X_SWNIC_TXRING_MAX_PKTDESC];			/* Total pkt count for each Tx descriptor Ring */
//static uint32	txPkthdrRingFreeIndex[RTL865X_SWNIC_TXRING_MAX_PKTDESC];	/* Point to the entry can be set to SEND packet */

#define txPktHdrRingFull(idx)	(((txPkthdrRingFreeIndex[idx] + 1) & (txPkthdrRingMaxIndex[idx])) == (txPkthdrRingDoneIndex[idx]))

/* Mbuf */
uint32* rxMbufRing=NULL;                                                     /* Point to the starting address of MBUF Ring */
uint32	rxMbufRingCnt;													/* Total MBUF count */

static uint32  size_of_cluster;

/* descriptor ring tracing pointers */
static int32   currRxPkthdrDescIndex;      /* Rx pkthdr descriptor to be handled by CPU */
static int32   currRxMbufDescIndex;        /* Rx mbuf descriptor to be handled by CPU */
static int32   currTxPkthdrDescIndex;      /* Tx pkthdr descriptor to be handled by CPU */
static int32 txPktDoneDescIndex;

/* debug counters */
static int32   rxPktCounter;
static int32   txPktCounter;

#if 0
#define ARPTAB_SIZ 16
//--------------------------------------------------------------------------
struct arptab_s
    {
    uint32  port_list;
    uint8   arp_mac_addr[6];  /* hardware address */
    uint8   valid;
    uint8   reserved;
    };
//--------------------------------------------------------------------------
static struct arptab_s arptab[ARPTAB_SIZ];
static uint32 arptab_next_available;
#endif

#define     BUF_FREE            0x00   /* Buffer is Free  */
#define     BUF_USED            0x80   /* Buffer is occupied */
#define     BUF_ASICHOLD        0x80   /* Buffer is hold by ASIC */
#define     BUF_DRIVERHOLD      0xc0   /* Buffer is hold by driver */

//--------------------------------------------------------------------------
/* mbuf header associated with each cluster 
*/
struct mBuf
{
    struct mBuf *m_next;
    struct pktHdr *m_pkthdr;            /* Points to the pkthdr structure */
    uint16    m_len;                    /* data bytes used in this cluster */
#ifdef CONFIG_RTL865XC
    uint16    m_flags;                  /* mbuf flags; see below */
#else
    int8      m_flags;                  /* mbuf flags; see below */
#endif
#define MBUF_FREE            BUF_FREE   /* Free. Not occupied. should be on free list   */
#define MBUF_USED            BUF_USED   /* Buffer is occupied */
#define MBUF_EXT             0x10       /* has associated with an external cluster, this is always set. */
#define MBUF_PKTHDR          0x08       /* is the 1st mbuf of this packet */
#define MBUF_EOR             0x04       /* is the last mbuf of this packet. Set only by ASIC*/
    uint8     *m_data;                  /*  location of data in the cluster */
    uint8     *m_extbuf;                /* start of buffer*/
    uint16    m_extsize;                /* sizeof the cluster */
    int8      m_reserved[2];            /* padding */
};
//--------------------------------------------------------------------------
/* pkthdr records packet specific information. Each pkthdr is exactly 32 bytes.
 first 20 bytes are for ASIC, the rest 12 bytes are for driver and software usage.
*/
struct pktHdr
{
    union
    {
        struct pktHdr *pkthdr_next;     /*  next pkthdr in free list */
        struct mBuf *mbuf_first;        /*  1st mbuf of this pkt */
    }PKTHDRNXT;
#define ph_nextfree         PKTHDRNXT.pkthdr_next
#define ph_mbuf             PKTHDRNXT.mbuf_first
    uint16    ph_len;                   /*   total packet length */
    uint16    ph_reserved1: 1;           /* reserved */
    uint16    ph_queueId: 3;            /* bit 2~0: Queue ID */
    uint16    ph_extPortList: 4;        /* dest extension port list. must be 0 for TX */
    uint16    ph_reserved2: 3;          /* reserved */
    uint16    ph_hwFwd: 1;              /* hwFwd - copy from HSA bit 200 */
    uint16    ph_isOriginal: 1;         /* isOriginal - DP included cpu port or more than one ext port */
    uint16    ph_l2Trans: 1;            /* l2Trans - copy from HSA bit 129 */
    uint16    ph_srcExtPortNum: 2;      /* Both in RX & TX. Source extension port number. */

    uint16    ph_type: 3;
#define PKTHDR_ETHERNET      0
#define PKTHDR_IP            2
#define PKTHDR_ICMP          3
#define PKTHDR_IGMP          4
#define PKTHDR_TCP           5
#define PKTHDR_UDP           6
    uint16    ph_vlanTagged: 1;         /* the tag status after ALE */
    uint16    ph_LLCTagged: 1;          /* the tag status after ALE */
    uint16    ph_pppeTagged: 1;         /* the tag status after ALE */
    uint16    ph_pppoeIdx: 3;
    uint16    ph_linkID: 7;             /* for WLAN WDS multiple tunnel */
    uint16    ph_reason;                /* indicates wht the packet is received by CPU */

    uint16    ph_flags;                 /*  NEW:Packet header status bits */
#define PKTHDR_FREE          (BUF_FREE << 8)        /* Free. Not occupied. should be on free list   */
#define PKTHDR_USED          (BUF_USED << 8)
#define PKTHDR_ASICHOLD      (BUF_ASICHOLD<<8)      /* Hold by ASIC */
#define PKTHDR_DRIVERHOLD    (BUF_DRIVERHOLD<<8)    /* Hold by driver */
#define PKTHDR_CPU_OWNED     0x4000
#define PKT_INCOMING         0x1000     /* Incoming: packet is incoming */
#define PKT_OUTGOING         0x0800     /*  Outgoing: packet is outgoing */
#define PKT_BCAST            0x0100     /*send/received as link-level broadcast  */
#define PKT_MCAST            0x0080     /*send/received as link-level multicast   */
#define PKTHDR_PPPOE_AUTOADD    0x0004  /* PPPoE header auto-add */
#define CSUM_TCPUDP_OK       0x0001     /*Incoming:TCP or UDP cksum checked */
#define CSUM_IP_OK           0x0002     /* Incoming: IP header cksum has checked */
#define CSUM_TCPUDP          0x0001     /*Outgoing:TCP or UDP cksum offload to ASIC*/
#define CSUM_IP              0x0002     /* Outgoing: IP header cksum offload to ASIC*/

   uint8      ph_orgtos;                /* RX: original TOS of IP header's value before remarking, TX: undefined */
   uint8      ph_portlist;              /* RX: source port number, TX: destination portmask */

   uint16     ph_vlanId_resv: 1;
   uint16     ph_txPriority: 3;
   uint16     ph_vlanId: 12;
   uint16     ph_flags2;
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
   uint8      ph_ptpResv:1;
   uint8      ph_ptpMsgType:4;	/* message type */
   uint8      ph_ptpVer:2;	/* PTP version, 0: 1588v1; 1: 1588v2 or 802.1as; others: reserved */
   uint8      ph_ptpPkt:1;	/* 1: PTP */
   int8       ph_reserved[3];            /* padding */
#endif
};
//--------------------------------------------------------------------------


/* LOCAL SUBPROGRAM SPECIFICATIONS
 */
static void arpInput(uint8*,uint32);
static int32 arpResolve(uint8*,uint8*);


#pragma ghs section text=".iram"
/*************************************************************************
*   FUNCTION                                                              
*       swNic_intHandler                                         
*                                                                         
*   DESCRIPTION                                                           
*       This function is the handler of NIC interrupts
*                                                                         
*   INPUTS                                                                
*       intPending      Pending interrupt sources.
*                                                                         
*   OUTPUTS                                                               
*       None
*************************************************************************/
void swNic_intHandler(uint32 intPending) {return;}


/*************************************************************************
*   FUNCTION                                                              
*       swNic_receive                                         
*                                                                         
*   DESCRIPTION                                                           
*       This function reads one packet from rx descriptors, and return the 
*       previous read one to the switch core. This mechanism is based on 
*       the assumption that packets are read only when the handling 
*       previous read one is done.
*                                                                         
*   INPUTS                                                                
*       None
*                                                                         
*   OUTPUTS                                                               
*       None
*************************************************************************/
int32 swNic_receive(void** input, uint32* pLen)
{
    struct pktHdr * pPkthdr;
    int32 pkthdr_index;
    int32 mbuf_index;
    static int32 firstTime = 1;
    char	*data;
    int	ret=-1;

    /* Check OWN bit of descriptors */
    if ( (rxPkthdrRing[0][currRxPkthdrDescIndex] & DESC_OWNED_BIT) == DESC_RISC_OWNED )
    {
        //ASSERT_ISR(currRxPkthdrDescIndex < rxPkthdrRingCnt[0]);
        
        /* Fetch pkthdr */
        pPkthdr = (struct pktHdr *) (rxPkthdrRing[0][currRxPkthdrDescIndex] & 
                                            ~(DESC_OWNED_BIT | DESC_WRAP));
    
        //ASSERT_ISR(pPkthdr->ph_len); /* Not allow zero packet length */
        //ASSERT_ISR(pPkthdr->ph_len >= 64);
        //ASSERT_ISR(pPkthdr->ph_len <= 1522);

        /* Increment counter */
        rxPktCounter++;

	data = pPkthdr->ph_mbuf->m_data;
	{
#if defined( CONFIG_NFBI) || defined(CONFIG_NONE_FLASH)
  //      if (1)
 		extern char eth0_mac[6]; 
	if ( (data[0]&0x1)||!memcmp(data, eth0_mac, 6) )

#else
		extern char eth0_mac[6];
		if ( (data[0]&0x1)||!memcmp(data, eth0_mac, 6) )
#endif
		{

		
        /* Output packet */
		        *input = data;
        *pLen = pPkthdr->ph_len - 4;
			ret = 0;


#if 1 //joey patch for inic
	   // for iNic & 8306 board, the received packet will has VLAN tag
	   if (*(unsigned short *)&(data[12]) == 0x8100)
	   {
		    memcpy(&(data[12]), &(data[16]), *pLen - 16);
	   } 
 #endif			
		}
		else
			ret = -1;
	}
#if 0//FPGA
	{
		dprintf("rx\n");
		dprintf("pkthdr=%x mbuf=%x mddata=%x len=%d\n", pPkthdr, pPkthdr->ph_mbuf, pPkthdr->ph_mbuf->m_data, *pLen);
//		if (*pLen > 64)
			ddump(*input, 64);
//		else
//			ddump(*input, *pLen);
	}
#endif
        if ( !firstTime )
        {
            /* Calculate previous pkthdr and mbuf index */
            pkthdr_index = currRxPkthdrDescIndex;
            if ( --pkthdr_index < 0 )
                pkthdr_index = rxPkthdrRingCnt[0] - 1;
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
            // for rx descriptor runout
            pPkthdr = (struct pktHdr *) (rxPkthdrRing[0][pkthdr_index] & ~(DESC_OWNED_BIT | DESC_WRAP));
            mbuf_index = ((uint32)(pPkthdr->ph_mbuf) - (rxMbufRing[0] & ~(DESC_OWNED_BIT | DESC_WRAP))) /
                (sizeof(struct mBuf));
#else
            mbuf_index = currRxMbufDescIndex;
            if ( --mbuf_index < 0 )
                mbuf_index = rxPkthdrRingCnt[0] - 1;
#endif
        
            /* Reset OWN bit */
            rxPkthdrRing[0][pkthdr_index] |= DESC_SWCORE_OWNED;
            rxMbufRing[mbuf_index] |= DESC_SWCORE_OWNED;
        }
        else
            firstTime = 0;
        
        /* Increment index */
        if ( ++currRxPkthdrDescIndex == rxPkthdrRingCnt[0] )
            currRxPkthdrDescIndex = 0;
        if ( ++currRxMbufDescIndex == rxMbufRingCnt )
            currRxMbufDescIndex = 0;

        if ( REG32(CPUIISR) & PKTHDR_DESC_RUNOUT_IP_ALL )
        {
            /* Enable and clear interrupt for continue reception */
            REG32(CPUIIMR) |= PKTHDR_DESC_RUNOUT_IE_ALL;
            REG32(CPUIISR) = PKTHDR_DESC_RUNOUT_IP_ALL;
        }
        return ret;
    }
    else
        return -1;
}



    uint8 pktbuf[2048];

/*************************************************************************
*   FUNCTION                                                              
*       swNic_send                                         
*                                                                         
*   DESCRIPTION                                                           
*       This function writes one packet to tx descriptors, and waits until 
*       the packet is successfully sent.
*                                                                         
*   INPUTS                                                                
*       None
*                                                                         
*   OUTPUTS                                                               
*       None
*************************************************************************/
int32 swNic_send(void * output, uint32 len)
{


    struct pktHdr * pPkthdr;
    //uint8 pktbuf[2048];
    uint8* pktbuf_alligned;

	int next_index;
	if ((currTxPkthdrDescIndex+1) == txPkthdrRingCnt[0])
		next_index = 0;
	else
		next_index = currTxPkthdrDescIndex+1;
	if (next_index == txPktDoneDescIndex) {
		dprintf("Tx Desc full!\n");
		return -1;
	}		

#ifdef _FAST_TX
	pktbuf_alligned = (uint8 *)output;
#else
	pktbuf_alligned = (uint8 *) (( (uint32) pktbuf & 0xfffffffc) | 0xa0000000);
    /* Copy Packet Content */
    memcpy(pktbuf_alligned, output, len);
#endif
	
#if 0  //FPGA

	{
		dprintf("\ntx\n");
//		if (len > 64)
			ddump(output, 32);
//		else
//			ddump(output, len);
	}
#endif
    ASSERT_CSP( ((int32) txPkthdrRing[0][currTxPkthdrDescIndex] & DESC_OWNED_BIT) == DESC_RISC_OWNED );

    /* Fetch packet header from Tx ring */
    pPkthdr = (struct pktHdr *) ((int32) txPkthdrRing[0][currTxPkthdrDescIndex] 
                                                & ~(DESC_OWNED_BIT | DESC_WRAP));

    /* Pad small packets and add CRC */
    if ( len < 60 )
        pPkthdr->ph_len = 64;
    else
        pPkthdr->ph_len = len + 4;
    pPkthdr->ph_mbuf->m_len       = pPkthdr->ph_len;
    pPkthdr->ph_mbuf->m_extsize = pPkthdr->ph_len;

    /* Set cluster pointer to buffer */
    pPkthdr->ph_mbuf->m_data    = pktbuf_alligned;
    pPkthdr->ph_mbuf->m_extbuf = pktbuf_alligned;
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
    pPkthdr->ph_ptpPkt = 0;
#endif

    /* Set destination port */
#if defined(CONFIG_RTL8198)
    pPkthdr->ph_portlist = ALL_PORT_MASK;
#else
        #define HW_STRAT_ROUTER_MODE 0x00100000
        if((REG32(HW_STRAP)&(HW_STRAT_ROUTER_MODE))==HW_STRAT_ROUTER_MODE)
        {
                pPkthdr->ph_portlist = ALL_PORT_MASK;
        }
        else
        {
                pPkthdr->ph_portlist = AP_MODE_PORT_MASK;//Port 4 Only for AP Mode
        }
#endif		
    /* Give descriptor to switch core */
    txPkthdrRing[0][currTxPkthdrDescIndex] |= DESC_SWCORE_OWNED;

    /* Set TXFD bit to start send */
    REG32(CPUICR) |= TXFD;
    txPktCounter++;

	currTxPkthdrDescIndex = next_index;
    return 0;
}

void swNic_txDone(void)
{
	struct pktHdr * pPkthdr;
	
	while (txPktDoneDescIndex != currTxPkthdrDescIndex) {		
	    if ( (*(volatile uint32 *)&txPkthdrRing[0][txPktDoneDescIndex] 
                    & DESC_OWNED_BIT) == DESC_RISC_OWNED ) {										
	    
		    pPkthdr = (struct pktHdr *) ((int32) txPkthdrRing[0][txPktDoneDescIndex] 
                                                & ~(DESC_OWNED_BIT | DESC_WRAP));

			if (++txPktDoneDescIndex == txPkthdrRingCnt[0])
				txPktDoneDescIndex = 0;
		}
		else
			break;
	}
}


#ifdef	CONFIG_RTL865X_MODEL_TEST_FT2
int32 swNic_send_portmbr(void * output, uint32 len, uint32 portmbr)
{
    struct pktHdr * pPkthdr;
    uint8 pktbuf[2048];
    uint8* pktbuf_alligned = (uint8*) (( (uint32) pktbuf & 0xfffffffc) | 0xa0000000);

    /* Copy Packet Content */
    memcpy(pktbuf_alligned, output, len);

    ASSERT_CSP( ((int32) txPkthdrRing[0][currTxPkthdrDescIndex] & DESC_OWNED_BIT) == DESC_RISC_OWNED );

    /* Fetch packet header from Tx ring */
    pPkthdr = (struct pktHdr *) ((int32) txPkthdrRing[0][currTxPkthdrDescIndex] 
                                                & ~(DESC_OWNED_BIT | DESC_WRAP));

    /* Pad small packets and add CRC */
    if ( len < 60 )
        pPkthdr->ph_len = 64;
    else
        pPkthdr->ph_len = len + 4;

    pPkthdr->ph_mbuf->m_len = pPkthdr->ph_len;
    pPkthdr->ph_mbuf->m_extsize = pPkthdr->ph_len;

    /* Set cluster pointer to buffer */
    pPkthdr->ph_mbuf->m_data = pktbuf_alligned;
    pPkthdr->ph_mbuf->m_extbuf = pktbuf_alligned;

    /* Set destination port */
    pPkthdr->ph_portlist = portmbr;

    /* Give descriptor to switch core */
    txPkthdrRing[0][currTxPkthdrDescIndex] |= DESC_SWCORE_OWNED;

    /* Set TXFD bit to start send */
    REG32(CPUICR) |= TXFD;
    
    /* Wait until packet is successfully sent */
    while ( (*(volatile uint32 *)&txPkthdrRing[0][currTxPkthdrDescIndex] 
                    & DESC_OWNED_BIT) == DESC_SWCORE_OWNED )
	{

        }
    txPktCounter++;
    
    if ( ++currTxPkthdrDescIndex == txPkthdrRingCnt[0] )
        currTxPkthdrDescIndex = 0;

    return 0;
}
#endif



#pragma ghs section text=default

/*************************************************************************
*   FUNCTION                                                              
*       swNic_init                                         
*                                                                         
*   DESCRIPTION                                                           
*       This function initializes descriptors and data structures.
*                                                                         
*   INPUTS                                                                
*       userNeedRxPkthdrRingCnt[RTL865X_SWNIC_RXRING_MAX_PKTDESC] :
*          Number of Rx pkthdr descriptors of each ring.
*       userNeedRxMbufRingCnt :
*          Number of Tx mbuf descriptors.
*       userNeedTxPkthdrRingCnt[RTL865X_SWNIC_TXRING_MAX_PKTDESC] :
*          Number of Tx pkthdr descriptors of each ring.
*       clusterSize :
*          Size of cluster.
*                                                                         
*   OUTPUTS                                                               
*       Status.
*************************************************************************/

int32 swNic_init(uint32 userNeedRxPkthdrRingCnt[RTL865X_SWNIC_RXRING_MAX_PKTDESC],
                 uint32 userNeedRxMbufRingCnt,
                 uint32 userNeedTxPkthdrRingCnt[RTL865X_SWNIC_TXRING_MAX_PKTDESC],
                 uint32 clusterSize)
{
    uint32 i, j, k;
	uint32 totalRxPkthdrRingCnt = 0, totalTxPkthdrRingCnt = 0;
    struct pktHdr *pPkthdrList;
    struct mBuf *pMbufList;
    uint8 * pClusterList;
    struct pktHdr * pPkthdr;
    struct mBuf * pMbuf;

    /* Cluster size is always 2048 */
    size_of_cluster = 2048;

    /* Allocate Rx descriptors of rings */
    for (i = 0; i < RTL865X_SWNIC_RXRING_MAX_PKTDESC; i++) {
		rxPkthdrRingCnt[i] = userNeedRxPkthdrRingCnt[i];
		if (rxPkthdrRingCnt[i] == 0)
			continue;

		rxPkthdrRing[i] = (uint32 *) UNCACHED_MALLOC(rxPkthdrRingCnt[i] * sizeof(uint32));
		ASSERT_CSP( (uint32) rxPkthdrRing[i] & 0x0fffffff );
		memset(rxPkthdrRing[i],0,rxPkthdrRingCnt[i] * sizeof(uint32));
		totalRxPkthdrRingCnt += rxPkthdrRingCnt[i];
    }
	
	if (totalRxPkthdrRingCnt == 0)
		return EINVAL;

    /* Allocate Tx descriptors of rings */
    for (i = 0; i < RTL865X_SWNIC_TXRING_MAX_PKTDESC; i++) {
		txPkthdrRingCnt[i] = userNeedTxPkthdrRingCnt[i];

		if (txPkthdrRingCnt[i] == 0)
			continue;

		txPkthdrRing[i] = (uint32 *) UNCACHED_MALLOC(txPkthdrRingCnt[i] * sizeof(uint32));
		
		ASSERT_CSP( (uint32) txPkthdrRing[i] & 0x0fffffff );
		memset(txPkthdrRing[i],0,(txPkthdrRingCnt[i] * sizeof(uint32)));
		totalTxPkthdrRingCnt += txPkthdrRingCnt[i];
    }

	if (totalTxPkthdrRingCnt == 0)
		return EINVAL;

    /* Allocate MBuf descriptors of rings */
	rxMbufRingCnt = userNeedRxMbufRingCnt;

	if (userNeedRxMbufRingCnt == 0)
		return EINVAL;

	rxMbufRing = (uint32 *) UNCACHED_MALLOC(userNeedRxMbufRingCnt * sizeof(uint32));
    ASSERT_CSP( (uint32) rxMbufRing & 0x0fffffff );
	memset(rxMbufRing,0,userNeedRxMbufRingCnt * sizeof(uint32));
    /* Allocate pkthdr */
    pPkthdrList = (struct pktHdr *) UNCACHED_MALLOC(
                    (totalRxPkthdrRingCnt + totalTxPkthdrRingCnt) * sizeof(struct pktHdr));
    ASSERT_CSP( (uint32) pPkthdrList & 0x0fffffff );
   memset(pPkthdrList,0, (totalRxPkthdrRingCnt + totalTxPkthdrRingCnt) * sizeof(struct pktHdr));                 
    /* Allocate mbufs */
    pMbufList = (struct mBuf *) UNCACHED_MALLOC(
                    (rxMbufRingCnt + totalTxPkthdrRingCnt) * sizeof(struct mBuf));
    ASSERT_CSP( (uint32) pMbufList & 0x0fffffff );
    memset(pMbufList,0,((rxMbufRingCnt + totalTxPkthdrRingCnt) * sizeof(struct mBuf)));                
    /* Allocate clusters */
    pClusterList = (uint8 *) UNCACHED_MALLOC(rxMbufRingCnt * size_of_cluster + 8 - 1+2*rxMbufRingCnt);
    ASSERT_CSP( (uint32) pClusterList & 0x0fffffff );
    memset(pClusterList,0,(rxMbufRingCnt * size_of_cluster + 8 - 1+2*rxMbufRingCnt));
    pClusterList = (uint8*)(((uint32) pClusterList + 8 - 1) & ~(8 - 1));

    /* Initialize interrupt statistics counter */
    rxPktCounter = txPktCounter = 0;

    /* Initialize index of Tx pkthdr descriptor */
    currTxPkthdrDescIndex = 0;
    txPktDoneDescIndex=0;

    /* Initialize Tx packet header descriptors */
    for (i = 0; i < RTL865X_SWNIC_TXRING_MAX_PKTDESC; i++)
	{
		for (j = 0; j < txPkthdrRingCnt[i]; j++)
		{
			/* Dequeue pkthdr and mbuf */
			pPkthdr = pPkthdrList++;
			pMbuf = pMbufList++;

         bzero((void *) pPkthdr, sizeof(struct pktHdr));
         bzero((void *) pMbuf, sizeof(struct mBuf));

			pPkthdr->ph_mbuf = pMbuf;
			pPkthdr->ph_len = 0;
			pPkthdr->ph_flags = PKTHDR_USED | PKT_OUTGOING;
			pPkthdr->ph_type = PKTHDR_ETHERNET;
			pPkthdr->ph_portlist = 0;

			pMbuf->m_next = NULL;
			pMbuf->m_pkthdr = pPkthdr;
			pMbuf->m_flags = MBUF_USED | MBUF_EXT | MBUF_PKTHDR | MBUF_EOR;
			pMbuf->m_data = NULL;
			pMbuf->m_extbuf = NULL;
			pMbuf->m_extsize = 0;

			txPkthdrRing[i][j] = (int32) pPkthdr | DESC_RISC_OWNED;

		}

		/* Set wrap bit of the last descriptor */
        if (txPkthdrRingCnt[i] != 0)
{
            txPkthdrRing[i][txPkthdrRingCnt[i] - 1] |= DESC_WRAP;

}

	}

    /* Fill Tx packet header FDP */
    REG32(CPUTPDCR0) = (uint32) txPkthdrRing[0];
    REG32(CPUTPDCR1) = (uint32) txPkthdrRing[1];
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
    REG32(CPUTPDCR2) = (uint32) txPkthdrRing[2];
    REG32(CPUTPDCR3) = (uint32) txPkthdrRing[3];
#endif

    /* Initialize index of current Rx pkthdr descriptor */
    currRxPkthdrDescIndex = 0;

    /* Initialize index of current Rx Mbuf descriptor */
    currRxMbufDescIndex = 0;

    /* Initialize Rx packet header descriptors */
	k = 0;

    for (i = 0; i < RTL865X_SWNIC_RXRING_MAX_PKTDESC; i++)
	{
		for (j = 0; j < rxPkthdrRingCnt[i]; j++)
		{
			/* Dequeue pkthdr and mbuf */
			pPkthdr = pPkthdrList++;
			pMbuf = pMbufList++;

         bzero((void *) pPkthdr, sizeof(struct pktHdr));
         bzero((void *) pMbuf, sizeof(struct mBuf));

			/* Setup pkthdr and mbuf */
			pPkthdr->ph_mbuf = pMbuf;
			pPkthdr->ph_len = 0;
			pPkthdr->ph_flags = PKTHDR_USED | PKT_INCOMING;
			pPkthdr->ph_type = PKTHDR_ETHERNET;
			pPkthdr->ph_portlist = 0;
			pMbuf->m_next = NULL;
			pMbuf->m_pkthdr = pPkthdr;
			pMbuf->m_len = 0;
			pMbuf->m_flags = MBUF_USED | MBUF_EXT | MBUF_PKTHDR | MBUF_EOR;
			pMbuf->m_data = NULL;
			pMbuf->m_extsize = size_of_cluster;
			/*offset 2 bytes for 4 bytes align of ip packet*/
			pMbuf->m_data = pMbuf->m_extbuf = (pClusterList+2);
			pClusterList += size_of_cluster;
			
			/* Setup descriptors */
			rxPkthdrRing[i][j] = (int32) pPkthdr | DESC_SWCORE_OWNED;
			rxMbufRing[k++] = (int32) pMbuf | DESC_SWCORE_OWNED;
		}

		/* Set wrap bit of the last descriptor */
        if (rxPkthdrRingCnt[i] != 0)
		    rxPkthdrRing[i][rxPkthdrRingCnt[i] - 1] |= DESC_WRAP;
	}

	rxMbufRing[rxMbufRingCnt - 1] |= DESC_WRAP;

    /* Fill Rx packet header FDP */
    REG32(CPURPDCR0) = (uint32) rxPkthdrRing[0];
    REG32(CPURPDCR1) = (uint32) rxPkthdrRing[1];
    REG32(CPURPDCR2) = (uint32) rxPkthdrRing[2];
    REG32(CPURPDCR3) = (uint32) rxPkthdrRing[3];
    REG32(CPURPDCR4) = (uint32) rxPkthdrRing[4];
    REG32(CPURPDCR5) = (uint32) rxPkthdrRing[5];

    REG32(CPURMDCR0) = (uint32) rxMbufRing;

// for debug
#if 0
    /* Initialize ARP table */
    bzero((void *) arptab, ARPTAB_SIZ * sizeof(struct arptab_s));
    arptab_next_available = 0;
#endif

	//dprintf("addr=%x, val=%x\r\n",(CPUIIMR),REG32(CPUIIMR));
    /* Enable runout interrupts */
    //REG32(CPUIIMR) |= RX_ERR_IE_ALL | TX_ERR_IE_ALL | PKTHDR_DESC_RUNOUT_IE_ALL;  //8651c
	//REG32(CPUIIMR) = 0xffffffff; //RX_DONE_IE_ALL;  //   0xffffffff;  //wei test irq
	
	//*(volatile unsigned int*)(0xb8010028)=0xffffffff;	
	//dprintf("eth0 CPUIIMR status=%x\r\n", *(volatile unsigned int*)(0xb8010028));   //ISR	
	
    /* Enable Rx & Tx. Config bus burst size and mbuf size. */
    //REG32(CPUICR) = TXCMD | RXCMD | BUSBURST_256WORDS | icr_mbufsize;
    //REG32(CPUICR) = TXCMD | RXCMD | BUSBURST_32WORDS | MBUF_2048BYTES;	//8651c
	REG32(CPUICR) = TXCMD | RXCMD | BUSBURST_32WORDS | MBUF_2048BYTES; //wei test irq

	REG32(CPUIIMR) = RX_DONE_IE_ALL | TX_DONE_IE_ALL; 


	REG32(MDCIOCR)=0x96181441;      // enable Giga port 8211B LED
	//dprintf("eth0 CPUIIMR status=%x\r\n", *(volatile unsigned int*)(0xb8010028));   //ISR
	
    return SUCCESS;
}


#ifdef FAT_CODE
/*************************************************************************
*   FUNCTION                                                              
*       swNic_resetDescriptors                                         
*                                                                         
*   DESCRIPTION                                                           
*       This function resets descriptors.
*                                                                         
*   INPUTS                                                                
*       None.
*                                                                         
*   OUTPUTS                                                               
*       None.
*************************************************************************/
void swNic_resetDescriptors(void)
{
    /* Disable Tx/Rx and reset all descriptors */
    REG32(CPUICR) &= ~(TXCMD | RXCMD);
    return;
}
#endif//FAT_CODE

