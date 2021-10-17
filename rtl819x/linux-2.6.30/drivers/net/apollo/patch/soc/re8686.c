/* re8670.c: A Linux Ethernet driver for the RealTek 8670 chips. */
/*
   Copyright 2001,2002 Jeff Garzik <jgarzik@mandrakesoft.com>

   Copyright (C) 2001, 2002 David S. Miller (davem@redhat.com) [tg3.c]
   Copyright (C) 2000, 2001 David S. Miller (davem@redhat.com) [sungem.c]
   Copyright 2001 Manfred Spraul				    [natsemi.c]
   Copyright 1999-2001 by Donald Becker.			    [natsemi.c]
   Written 1997-2001 by Donald Becker.			    [8139too.c]
   Copyright 1998-2001 by Jes Sorensen, <jes@trained-monkey.org>. [acenic.c]

   This software may be used and distributed according to the terms of
   the GNU General Public License (GPL), incorporated herein by reference.
   Drivers based on or derived from this code fall under the GPL and must
   retain the authorship, copyright and license notice.  This file is not
   a complete program and may only be used when the entire operating
   system is licensed under the GPL.

   See the file COPYING in this distribution for more information.

   TODO, in rough priority order:
 * dev->tx_timeout
 * LinkChg interrupt
 * Support forcing media type with a module parameter,
 like dl2k.c/sundance.c
 * Implement PCI suspend/resume
 * Constants (module parms?) for Rx work limit
 * support 64-bit PCI DMA
 * Complete reset on PciErr
 * Consider Rx interrupt mitigation using TimerIntr
 * Implement 8139C+ statistics dump; maybe not...
 h/w stats can be reset only by software reset
 * Tx checksumming
 * Handle netif_rx return value
 * ETHTOOL_GREGS, ETHTOOL_[GS]WOL,
 * Investigate using skb->priority with h/w VLAN priority
 * Investigate using High Priority Tx Queue with skb->priority
 * Adjust Rx FIFO threshold and Max Rx DMA burst on Rx FIFO error
 * Adjust Tx FIFO threshold and Max Tx DMA burst on Tx FIFO error
 * Implement Tx software interrupt mitigation via
 Tx descriptor bit
 * The real minimum of CP_MIN_MTU is 4 bytes.  However,
 for this to be supported, one must(?) turn on packet padding.

*/

#define DRV_NAME		"8686"
#define DRV_VERSION		"0.0.1"
#define DRV_RELDATE		"Feb 17, 2012"


#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/compiler.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/delay.h>
#include <linux/ethtool.h>
#include <linux/mii.h>
#include <linux/if_vlan.h>
#include <linux/crc32.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <net/xfrm.h>

#ifdef CONFIG_RTL8672	//shlee 2.6
#include <bspchip.h>
#include <linux/version.h>
#endif
#ifdef CONFIG_RTL865X_ETH_PRIV_SKB
#include "re_privskb.h"
#endif

#include "ethctl_implement.h"
#include "re8686.h"

#include "apollo/sdk/system/include/common/type.h"
#include "apollo/sdk/include/rtk/gpon.h"

/*static*/ int re8670_start_xmit (struct sk_buff *skb, struct net_device *dev);

/* Jonah + for FASTROUTE */
struct net_device *eth_net_dev;
struct tasklet_struct *eth_rx_tasklets=NULL;

#define WITH_NAPI		""

/* These identify the driver base version and may not be removed. */
static char version[] __devinitdata =
KERN_INFO DRV_NAME " Ethernet driver v" DRV_VERSION " (" DRV_RELDATE ")" WITH_NAPI "\n";

MODULE_AUTHOR("Krammer Liu <krammer@realtek.comw>");
MODULE_DESCRIPTION("RealTek RTL-8686 series 10/100/1000 Ethernet driver");
MODULE_LICENSE("GPL");

/* Maximum number of multicast addresses to filter (vs. Rx-all-multicast).
   The RTL chips use a 64 element hash table based on the Ethernet CRC.  */
static int multicast_filter_limit = 32;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 9)	//shlee 2.6
module_param(multicast_filter_limit, int, S_IRUGO|S_IWUSR)
#else
	MODULE_PARM (multicast_filter_limit, "i");
#endif
	MODULE_PARM_DESC (multicast_filter_limit, "8686 maximum number of filtered multicast addresses");

#define PFX			DRV_NAME ": "
#define CP_DEF_MSG_ENABLE	(NETIF_MSG_DRV		| \
		NETIF_MSG_PROBE 	| \
		NETIF_MSG_LINK)
#define CP_REGS_SIZE		(0xff + 1)

#define DESC_ALIGN		0x100
#define UNCACHE_MASK		0xa0000000
#define RE8670_RXRING_BYTES	( (sizeof(struct dma_rx_desc) * (RE8670_RX_RING_SIZE+1)) + DESC_ALIGN)
#define RE8670_TXRING_BYTES	( (sizeof(struct dma_tx_desc) * (RE8670_TX_RING_SIZE+1)) + DESC_ALIGN)

#define NEXT_TX(N)		(((N) + 1) & (RE8670_TX_RING_SIZE - 1))
#define NEXT_RX(N)		(((N) + 1) & (RE8670_RX_RING_SIZE - 1))

#define TX_HQBUFFS_AVAIL(CP)					\
		(((CP)->tx_hqtail - (CP)->tx_hqhead + RE8670_TX_RING_SIZE - 1)&(RE8670_TX_RING_SIZE - 1))

#define PKT_BUF_SZ		1536	/* Size of each temporary Rx buffer.*/
#define RX_OFFSET		2


/* The following settings are log_2(bytes)-4:  0 == 16 bytes .. 6==1024, 7==end of packet. */
/* Time in jiffies before concluding the transmitter is hung. */
#define TX_TIMEOUT		(3*HZ)
/* hardware minimum and maximum for a single frame's data payload */
#define CP_MIN_MTU		60	/* TODO: allow lower, but pad */
#define CP_MAX_MTU		4096

enum PHY_REGS{
	FORCE_TX = 1<<7,
	RXFCE= 1<<6,
	TXFCE= 1<<5,
	SP1000= 1<<4,
	SP10= 1<<3,
	LINK=1<<2,
	TXPF=1<<1,
	RXPF=1<<0,
};

enum RE8670_STATUS_REGS
{
	/*TX/RX share */
	DescOwn		= (1 << 31), /* Descriptor is owned by NIC */
	RingEnd		= (1 << 30), /* End of descriptor ring */
	FirstFrag		= (1 << 29), /* First segment of a packet */
	LastFrag		= (1 << 28), /* Final segment of a packet */

	/*Tx descriptor opt1*/
	IPCS		= (1 << 27),
	L4CS		= (1 << 26),
	KEEP		= (1 << 25),
	BLU			= (1 << 24),
	TxCRC		= (1 << 23),
	VSEL		= (1 << 22),
	DisLrn		= (1 << 21),
	CPUTag_ipcs 	= (1 << 20),
	CPUTag_l4cs	= (1 << 19),

	/*Tx descriptor opt2*/
	CPUTag		= (1 << 31),
	aspri		= (1 << 30),
	CPRI		= (1 << 27),
	TxVLAN_int	= (0 << 25),  //intact
	TxVLAN_ins	= (1 << 25),  //insert
	TxVLAN_rm	= (2 << 25),  //remove
	TxVLAN_re	= (3 << 25),  //remark
	//TxPPPoEAct	= (1 << 23),
	TxPPPoEAct	= 23,
	//TxPPPoEIdx	= (1 << 20),
	TxPPPoEIdx	= 20,
	Efid			= (1 << 19),
	//Enhan_Fid	= (1 << 16),
	Enhan_Fid 	= 16,
	/*Tx descriptor opt3*/
	SrcExtPort	= 29,
	TxDesPortM	= 23,
	TxDesStrID 	= 16,
	TxDesVCM	= 0,
	/*Tx descriptor opt4*/
	/*Rx descriptor  opt1*/
	CRCErr	= (1 << 27),
	IPV4CSF		= (1 << 26),
	L4CSF		= (1 << 25),
	RCDF		= (1 << 24),
	IP_FRAG		= (1 << 23),
	PPPoE_tag	= (1 << 22),
	RWT			= (1 << 21),
	PktType		= (1 << 17),
	RxProtoIP	= 1,
	RxProtoPPTP	= 2,
	RxProtoICMP	= 3,
	RxProtoIGMP	= 4,
	RxProtoTCP	= 5,   
	RxProtoUDP	= 6,
	RxProtoIPv6	= 7,
	RxProtoICMPv6	= 8,
	RxProtoTCPv6	= 9,
	RxProtoUDPv6	= 10,
	L3route		= (1 << 16),
	OrigFormat	= (1 << 15),
	PCTRL		= (1 << 14),
	/*Rx descriptor opt2*/
	PTPinCPU	= (1 << 30),
	SVlanTag		= (1 << 29),
	/*Rx descriptor opt3*/
	SrcPort		= (1 << 27),
	DesPortM	= (1 << 21),
	Reason		= (1 << 13),
	IntPriority	= (1 << 10),
	ExtPortTTL	= (1 << 5),
};

enum RE8670_THRESHOLD_REGS{
	//shlee	THVAL		= 2,
	TH_ON_VAL = 0x06,	//shlee flow control assert threshold: available desc <= 6
	TH_OFF_VAL= 0x30,	//shlee flow control de-assert threshold : available desc>=48
	//	RINGSIZE	= 0x0f,	//shlee 	2,
	LOOPBACK	= (0x3 << 8),
	AcceptErr	= 0x20,	     /* Accept packets with CRC errors */
	AcceptRunt	= 0x10,	     /* Accept runt (<64 bytes) packets */
	AcceptBroadcast	= 0x08,	     /* Accept broadcast packets */
	AcceptMulticast	= 0x04,	     /* Accept multicast packets */
	AcceptMyPhys	= 0x02,	     /* Accept pkts with our MAC as dest */
	AcceptAllPhys	= 0x01,	     /* Accept all pkts w/ physical dest */
	AcceptAll = AcceptBroadcast | AcceptMulticast | AcceptMyPhys |  AcceptAllPhys | AcceptErr | AcceptRunt,
	AcceptNoBroad = AcceptMulticast |AcceptMyPhys |  AcceptAllPhys | AcceptErr | AcceptRunt,
	AcceptNoMulti =  AcceptMyPhys |  AcceptAllPhys | AcceptErr | AcceptRunt,
	NoErrAccept = AcceptBroadcast | AcceptMulticast | AcceptMyPhys,
	NoErrPromiscAccept = AcceptBroadcast | AcceptMulticast | AcceptMyPhys |  AcceptAllPhys,
};

enum RE8670_ISR_REGS{
	RDU6	= (1 << 15),
	RDU5	= (1 << 14),
	RDU4	= (1 << 13),
	RDU3	= (1 << 12),
	RDU2	= (1 << 11),
	SW_INT 		= (1 <<10),
	TDU	= (1 << 9),
	LINK_CHG	= (1 <<	8),
	TER		= (1 << 7),
	TOK		= (1 << 6),
	RDU	= (1 << 5),
	RER_OVF	=(1 << 4),
	RER_RUNT	=(1 << 2),
	RX_OK		= (1 << 0),
	RDU_ALL = (RDU | RDU2 | RDU3 | RDU4 | RDU5 | RDU6),
	RX_ALL = (RX_OK | RER_RUNT | RER_OVF | RDU_ALL),
};

enum RTL8672GMAC_CPUtag_Control
{
	CTEN_RX     = (1<<31),
	CT_TSIZE	= 27,
	CT_DSLRN	= (1 << 24),
	CT_NORMK	= (1 << 23),
	CT_ASPRI	= (1 << 22),
	CT_APPLO	= (6 << 18),
	CT_8681	= (5 << 18),
	CT_8370S    = (4 <<18),
	CT_8307S	= (3 << 18),
	CT_8306S    = (2<<18),
	CT_8368S    = (1<<18),

	CT_RSIZE_H = 25,
	CT_RSIZE_L = 16,
	CTPM_8306   = (0xf0 << 8),
	CTPM_8368   = (0xe0 << 8),
	CTPM_8370   = (0xff << 8),
	CTPM_8307   = (0xff << 8),
	CTPV_8306   = 0x90,
	CTPV_8368   = 0xa0,
	CTPV_8370   = 0x04,
	CTPV_8307	  = 0x04,
};

enum RTL8672GMAC_PG_REG
{
	EN_PGLBK     = (1<<15),
	DATA_SEL     = (1<<14),
	LEN_SEL      = (1<<11),
	NUM_SEL      = (1<<10),
};


typedef enum
{
	FLAG_WRITE		= (1<<31),
	FLAG_READ		= (0<<31),

	MII_PHY_ADDR_SHIFT	= 26, 
	MII_REG_ADDR_SHIFT	= 16,
	MII_DATA_SHIFT		= 0,
}MIIAR_MASK;

struct rtl8686_dev_table_entry {
	unsigned char 			ifname[IFNAMSIZ];
	unsigned char			isWan;
	unsigned short			vid;
	unsigned int			phyPort;
	unsigned int                    to_dev;//means u want this port will be translate to which net_dev in the 8686_dev_table
	struct net_device *dev_instant;
};

typedef struct dma_tx_desc {
	u32		opts1;
	u32		addr;
	u32		opts2;
	u32		opts3;
	u32		opts4;
}DMA_TX_DESC;

typedef struct dma_rx_desc {
	u32		opts1;
	u32		addr;
	u32		opts2;
	u32		opts3;
}DMA_RX_DESC;

struct ring_info {
	struct sk_buff		*skb;
	dma_addr_t		mapping;
	unsigned		frag;
};

struct cp_extra_stats {
	unsigned long		rx_frags;
	unsigned long tx_timeouts;
	//krammer add for rx info
	unsigned int rx_hw_num;
	unsigned int rx_sw_num;
	unsigned int rer_runt;
	unsigned int rer_ovf;
	unsigned int rdu;
	unsigned int frag;
	unsigned int crcerr;
	unsigned int rcdf;
	unsigned int rx_no_mem;
	//krammer add for tx info
	unsigned int tx_sw_num;
	unsigned int tx_hw_num;
	unsigned int tx_no_desc;
};

struct re_private {
	unsigned		tx_hqhead;
	unsigned		tx_hqtail;
	unsigned		tx_lqhead;
	unsigned		tx_lqtail;
	unsigned		rx_tail;

	void			*regs;
	struct net_device	*dev;
	spinlock_t		lock;
	DMA_RX_DESC		*rx_ring;

	DMA_TX_DESC		*tx_hqring;
	DMA_TX_DESC		*tx_lqring;

	struct ring_info	tx_skb[RE8670_TX_RING_SIZE];
	struct ring_info	rx_skb[RE8670_RX_RING_SIZE];
	unsigned		rx_buf_sz;
	dma_addr_t		ring_dma;
	u32			msg_enable;

	struct cp_extra_stats	cp_stats;

	struct pci_dev		*pdev;
	u32			rx_config;

	struct sk_buff		*frag_skb;
	unsigned		dropping_frag : 1;
	char*			rxdesc_buf;
	char*			txdesc_buf;
	struct mii_if_info	mii_if;
	//struct tq_struct	rx_task;
	//struct tq_struct	tx_task;
	struct tasklet_struct rx_tasklets;
	//struct tasklet_struct tx_tasklets;
};

struct re_dev_private {
	struct re_private* pCp;
	struct net_device_stats net_stats;
};

static struct re_private re_private_data;
static struct pkt_dbg_s re_dbg_data;
int dev_num = 0;

#define DEV2CP(dev)  (((struct re_dev_private*)dev->priv)->pCp)
#define DEVPRIV(dev)  ((struct re_dev_private*)dev->priv)
#define VTAG2DESC(d) ( (((d) & 0x00ff)<<8) | (((d) & 0x0f00)>>8) )

static void __re8670_set_rx_mode (struct net_device *dev);
static void re8670_tx (struct re_private *cp);
static void re8670_clean_rings (struct re_private *cp);
static void re8670_tx_timeout (struct net_device *dev);

atomic_t re8670_rxskb_num = ATOMIC_INIT(0);
//int re8670_rxskb_num=0;
#define RE8670_MAX_ALLOC_RXSKB_NUM 512

#define SKB_BUF_SIZE  1600

unsigned int iocmd_reg=CMD_CONFIG;//=0x4009113d;	//shlee 8672
unsigned int iocmd1_reg=CMD1_CONFIG; //=0x0; //czyao
__DRAM unsigned int debug_enable=0;

enum RTL8686GMAC_DEBUG_LEVEL{
	RTL8686_PRINT_NOTHING = 0,
	RTL8686_SKB_RX = (1<<0),
	RTL8686_SKB_TX = (1<<1),
	RTL8686_RXINFO = (1<<2),
	RTL8686_TXINFO = (1<<3),
};
#define ETH_WAN_PORT 3//in virtual view
#define SW_PORT_NUM 7
__DRAM struct net_device* port2dev[SW_PORT_NUM] = {0};

void memDump (void *start, u32 size, char * strHeader)
{
	int row, column, index, index2, max;
//	uint32 buffer[5];
	u8 *buf, *line, ascii[17];
	char empty = ' ';

	if(!start ||(size==0))
		return;
	line = (u8*)start;

	/*
	16 bytes per line
	*/
	if (strHeader)
		printk("%s", strHeader);
	column = size % 16;
	row = (size / 16) + 1;
	for (index = 0; index < row; index++, line += 16) 
	{
		buf = line;

		memset (ascii, 0, 17);

		max = (index == row - 1) ? column : 16;
		if ( max==0 ) break; /* If we need not dump this line, break it. */

		printk("\n%08x ", (u32) line);
		
		//Hex
		for (index2 = 0; index2 < max; index2++)
		{
			if (index2 == 8)
			printk("  ");
			printk("%02x ", (u8) buf[index2]);
			ascii[index2] = ((u8) buf[index2] < 32) ? empty : buf[index2];
		}

		if (max != 16)
		{
			if (max < 8)
				printk("  ");
			for (index2 = 16 - max; index2 > 0; index2--)
				printk("   ");
		}

		//ASCII
		printk("  %s", ascii);
	}
	printk("\n");
	return;
}


#undef ETH_DBG
#define ETH_DBG
#ifdef ETH_DBG
static void skb_debug(const struct sk_buff *skb, int enable, int flag)
{
#define NUM2PRINT 1518
	if (unlikely(enable & flag)) {
		if (flag == RTL8686_SKB_RX)
			printk("\nI: ");
		else
			printk("\nO: ");
		printk("eth len = %d eth name %s", skb->len,skb->dev?skb->dev->name:"");
		memDump(skb->data, (skb->len > NUM2PRINT)?NUM2PRINT : skb->len, "");
		if(skb->len > NUM2PRINT){
			printk("........");
		}
		printk("\n");
	}
}

static void rxinfo_debug(struct rx_info *pRxInfo)
{
	if (unlikely(debug_enable & (RTL8686_RXINFO))) {
		printk("rxInfo:\n");
		printk("opts1\t= 0x%08x\n", pRxInfo->opts1.dw);
		printk("addr\t= 0x%08x\n", pRxInfo->addr);
		printk("opts2\t= 0x%08x\n", pRxInfo->opts2.dw);
		printk("opts3\t= 0x%08x\n", pRxInfo->opts3.dw);
	}
}

static void txinfo_debug(struct tx_info *pTxInfo)
{
	if (unlikely(debug_enable & (RTL8686_TXINFO))) {
		printk("txInfo:\n");
		printk("opts1\t= 0x%08x\n", pTxInfo->opts1.dw);
		printk("addr\t= 0x%08x\n", pTxInfo->addr);
		printk("opts2\t= 0x%08x\n", pTxInfo->opts2.dw);
		printk("opts3\t= 0x%08x\n", pTxInfo->opts3.dw);
		printk("opts4\t= 0x%08x\n", pTxInfo->opts4.dw);
	}
}

#define ETHDBG_PRINT(flag, fmt, args...)  if(unlikely(debug_enable & flag)) printk(fmt, ##args)
#define SKB_DBG(args...) skb_debug(args)
#define RXINFO_DBG(args...) rxinfo_debug(args)
#define TXINFO_DBG(args...) txinfo_debug(args)
#else
#define ETHDBG_PRINT(fmt, args...)
#define SKB_DBG(args...) 
#define RXINFO_DBG(args...)
#define TXINFO_DBG(args...)
#endif 

__IRAM_NIC
struct sk_buff *re8670_getAlloc(unsigned int size)
{	
	struct sk_buff *skb=NULL;
	if ( atomic_read(&re8670_rxskb_num) < RE8670_MAX_ALLOC_RXSKB_NUM ) {
		#ifdef CONFIG_RTL865X_ETH_PRIV_SKB  //For RTL8192CE
		skb = dev_alloc_skb_priv_eth(CROSS_LAN_MBUF_LEN);
		#else
		skb = dev_alloc_skb(size);
		#endif

		if (skb!=NULL) {
			atomic_inc(&re8670_rxskb_num);
			skb->src_port=IF_ETH;
		}
	}
	else {
		printk("%s(%d): limit reached (%d/%d)\n",__FUNCTION__,__LINE__,atomic_read(&re8670_rxskb_num),RE8670_MAX_ALLOC_RXSKB_NUM);
	}
	return skb;
}

static inline void re8670_set_rxbufsize (struct re_private *cp)
{
	unsigned int mtu = cp->dev->mtu;
	if (mtu > ETH_DATA_LEN)
		/* MTU + ethernet header + FCS + optional VLAN tag */
		cp->rx_buf_sz = mtu + ETH_HLEN + 8;
	else
		cp->rx_buf_sz = PKT_BUF_SZ;
}

atomic_t lock_tx_tail = ATOMIC_INIT(0);

/*__IRAM_NIC*/ void nic_tx2(struct sk_buff *skb)
{
	skb->users.counter=1;
	re8670_start_xmit(skb,eth_net_dev);
}

__IRAM_NIC
static struct net_device* decideRxDevice(struct rx_info *pRxInfo){
	unsigned int num = (pRxInfo->opts3.bit.src_port_num >= SW_PORT_NUM) ? 
		(SW_PORT_NUM - 1) : pRxInfo->opts3.bit.src_port_num ;
	return port2dev[num];
}

static inline void updateRxStatic(struct re_private *cp, struct sk_buff *skb){
	skb->dev->last_rx = jiffies;
	cp->cp_stats.rx_sw_num++;
	DEVPRIV(skb->dev)->net_stats.rx_packets++;
	DEVPRIV(skb->dev)->net_stats.rx_bytes += skb->len;
}

__IRAM_NIC
static void re8670_rx_skb (struct re_private *cp, struct sk_buff *skb, struct rx_info *pRxInfo)
{
	skb->dev = decideRxDevice(pRxInfo);
	
	ETHDBG_PRINT(RTL8686_SKB_RX, "This packet comes from %s \n", skb->dev->name);
	skb->from_dev=skb->dev;
	skb->protocol = eth_type_trans (skb, skb->dev);
	
	//do we need any wan dev rx hacking here?(before pass to netif_rx)
	
	updateRxStatic(cp, skb);
	SKB_DBG(skb, debug_enable, RTL8686_SKB_RX);
	if (netif_rx(skb) == NET_RX_DROP)
		DEVPRIV(skb->dev)->net_stats.rx_dropped++;
}

static 
__IRAM_NIC
unsigned int re8670_rx_csum_ok (struct rx_info *rxInfo)
{
	unsigned int protocol = rxInfo->opts1.bit.pkttype;

	switch(protocol){
		case RxProtoTCP:
		case RxProtoUDP:
		case RxProtoICMP:
		case RxProtoIGMP:/*we check both l4cs and ipv4cs when protocol is ipv4 l4*/
			if(likely((!rxInfo->opts1.bit.l4csf) && (!rxInfo->opts1.bit.ipv4csf))){
				return 1;
			}
			break;
		case RxProtoTCPv6:
		case RxProtoUDPv6:
		case RxProtoICMPv6:/*when protocol is ipv6, we only check l4cs*/
			if(likely(!rxInfo->opts1.bit.l4csf)){
				return 1;
			}
			break;
		default:/*no guarantee when the protocol is only ipv4*/
			break;
	}
	return 0;
}

unsigned char eth_close=0;

// Kaohj -- for polling mode
int eth_poll; // poll mode flag
void eth_poll_schedule(void)
{
	tasklet_hi_schedule(eth_rx_tasklets);
}

static inline void retriveRxInfo(DMA_RX_DESC *desc, struct rx_info *pRxInfo){
	pRxInfo->opts1.dw = desc->opts1;
	pRxInfo->addr= desc->addr;
	pRxInfo->opts2.dw = desc->opts2;
	pRxInfo->opts3.dw = desc->opts3;
}

static inline void updateGmacFlowControl(unsigned rx_tail){
	unsigned int new_cpu_desc_num;
	new_cpu_desc_num = RTL_R32(EthrntRxCPU_Des_Num);
	//printk("%08x\n", new_cpu_desc_num);
	new_cpu_desc_num &= 0x00FFFF0F;//clear
	new_cpu_desc_num |= (((rx_tail&0xFF)<<24)|(((rx_tail>>8)&0xF)<<4));//update
	//printk("%08x\n", new_cpu_desc_num);
	RTL_R32(EthrntRxCPU_Des_Num) = new_cpu_desc_num;
}

int re8686_rx_pktDump_set(unsigned int enable)
{
    if(0 == enable)
        re_dbg_data.enable = 0;
    else
        re_dbg_data.enable = 1;

    printk("Packet dump: %s\n", re_dbg_data.enable == 0 ? "diabled" : "enabled");

    return 0;
}

int re8686_rx_pktDump_get(unsigned char *pPayload, unsigned short buf_length, unsigned short *pPkt_len, void *pInfo, unsigned int *pEnable)
{
    int len;
    
    len = (re_dbg_data.pkt_length > buf_length) ? buf_length : re_dbg_data.pkt_length;
    *pEnable = re_dbg_data.enable;
    *pPkt_len = re_dbg_data.pkt_length;

    if(0 == len)
        return 0;

    *((struct rx_info *)pInfo) = re_dbg_data.rx_desc;
    memcpy(pPayload, &re_dbg_data.raw_pkt[0], buf_length);

    return 0;
}

int re8686_rx_pktDump_clear(void)
{
    memset(&re_dbg_data.rx_desc, 0, sizeof(struct rx_info));
    memset(&re_dbg_data.raw_pkt[0], 0, sizeof(unsigned char) * 1600);
    re_dbg_data.pkt_length = 0;

    return 0;
}

__IRAM_NIC
static void re8670_rx (struct re_private *cp)
{
	unsigned rx_tail = cp->rx_tail;
	unsigned rx_work = RE8670_RX_RING_SIZE;
	unsigned long flags;  

	spin_lock_irqsave(&cp->lock,flags);   
	//protect eth rx while reboot
	if(unlikely(eth_close == 1)){
		spin_unlock_irqrestore (&cp->lock, flags);   
		return;
	}
	// Kaohj
	if (unlikely(eth_poll)) // in poll mode
		rx_work = 2; // rx rate is rx_work * 100 pps (timer tick is 10ms)

	while (rx_work--)
	{
		u32 len;
		struct sk_buff *skb, *new_skb;
		DMA_RX_DESC *desc;
		unsigned buflen;
		struct rx_info rxInfo;

		cp->cp_stats.rx_hw_num++;
		
		skb = cp->rx_skb[rx_tail].skb;
		if (unlikely(!skb))
			BUG();

		desc = &cp->rx_ring[rx_tail];
		
		//translate desc into a local data structure
		retriveRxInfo(desc, &rxInfo);
		
		if (rxInfo.opts1.bit.own)
			break;

		RXINFO_DBG(&rxInfo);

		len = (rxInfo.opts1.bit.data_length & 0x07ff) - 4;

		if (unlikely((rxInfo.opts1.dw & (FirstFrag | LastFrag)) != (FirstFrag | LastFrag))) {
			cp->cp_stats.frag++;
			goto rx_next;
		}
		if (unlikely(rxInfo.opts1.bit.rcdf)){
			cp->cp_stats.rcdf++;
			goto rx_next;
		}
		if (unlikely(rxInfo.opts1.bit.crcerr)){
			cp->cp_stats.crcerr++;
			goto rx_next;
		}

		buflen = cp->rx_buf_sz + RX_OFFSET;

		new_skb = re8670_getAlloc(SKB_BUF_SIZE);
		if (unlikely(!new_skb)) {
			cp->cp_stats.rx_no_mem++;
			dma_cache_wback_inv((unsigned long)skb->data,buflen);
			goto rx_next;
		}

		/* Handle checksum offloading for incoming packets. */
		if (re8670_rx_csum_ok(&rxInfo))
			skb->ip_summed = CHECKSUM_UNNECESSARY;
		else
			skb->ip_summed = CHECKSUM_NONE;

		skb_reserve(skb, RX_OFFSET);
		skb_put(skb, len);

#if 1 /* For debug usage Elliot */
        if(unlikely(re_dbg_data.enable != 0)) {
    		retriveRxInfo(desc, &re_dbg_data.rx_desc);
            re_dbg_data.pkt_length = SKB_BUF_SIZE > skb->len ? skb->len : SKB_BUF_SIZE;
            memcpy(&re_dbg_data.raw_pkt[0], skb->data, re_dbg_data.pkt_length);
        }
#endif

		SKB_DBG(skb, debug_enable, RTL8686_SKB_RX);
#if 1 /* Apollo rx OMCI, Scott */
    if((rxInfo.opts3.bit.src_port_num == 3) && (rxInfo.opts1.bit.pctrl == 1))
    //if((pRxInfo->opts3.bit.src_port_num == 3) && (pRxInfo->opts3.bit.reason == 209))
    {
        rtk_gpon_omci_rx(skb->data, skb->len);
    }
#endif

		cp->rx_ring[rx_tail].addr = (u32)new_skb->data;
		cp->rx_skb[rx_tail].skb = new_skb;
		dma_cache_wback_inv((unsigned long)new_skb->data,buflen);

		re8670_rx_skb(cp, skb, &rxInfo);
rx_next:
		desc->opts1 = (DescOwn | cp->rx_buf_sz) | 
			((rx_tail == (RE8670_RX_RING_SIZE - 1))?RingEnd:0);
		updateGmacFlowControl(rx_tail);
		rx_tail = NEXT_RX(rx_tail);
	}
	cp->rx_tail = rx_tail;
	// Enable RX interrupt
	if(!rx_work){
		tasklet_hi_schedule(&cp->rx_tasklets);
	}
	RTL_R16(IMR)|=(u16)(RX_ALL);//we still open imr when rx_work==0 for a quickly schedule
	spin_unlock_irqrestore (&cp->lock, flags); 
}

__IRAM_NIC
static irqreturn_t re8670_interrupt(int irq, void * dev_instance, struct pt_regs *regs)
{
	struct net_device *dev = dev_instance;
	struct re_private *cp = DEV2CP(dev);
	u16 status;

	if (!(status = RTL_R16(ISR)))  
	{
		//printk("%s: no status indicated in interrupt, weird!\n", __func__);	//shlee 2.6
		return IRQ_RETVAL(IRQ_NONE);
	}

	if (status & RX_ALL) {	
		if(status & RER_RUNT){
			cp->cp_stats.rer_runt++;
		}
		if(status & RER_OVF){
			cp->cp_stats.rer_ovf++;
		}
		if(status & RDU_ALL){
			cp->cp_stats.rdu++;
		}
		RTL_R16(IMR)&=(u16)(~RX_ALL);
		tasklet_hi_schedule(&cp->rx_tasklets);
	}
	RTL_W16(ISR,status);

	return IRQ_RETVAL(IRQ_HANDLED);
}

static inline void updateTxStatic(struct sk_buff *skb){
	DEVPRIV(skb->dev)->net_stats.tx_packets++;
	DEVPRIV(skb->dev)->net_stats.tx_bytes += skb->len;
}

__IRAM_NIC void re8670_tx (struct re_private *cp)
{
	unsigned tx_tail = cp->tx_hqtail;
	u32 status;
	struct sk_buff *skb;
	if(unlikely(eth_close))
		return;

	while (!((status = (cp->tx_hqring[tx_tail].opts1))& DescOwn)) {
		if (tx_tail == cp->tx_hqhead)
			break;

		skb = cp->tx_skb[tx_tail].skb;
		if (unlikely(!skb))
			break;
		updateTxStatic(skb);		
		dev_kfree_skb_any(skb); 

		cp->tx_skb[tx_tail].skb = NULL;
		tx_tail = NEXT_TX(tx_tail);
	}

	cp->tx_hqtail = tx_tail;
	if (netif_queue_stopped(cp->dev) && (TX_HQBUFFS_AVAIL(cp) > (MAX_SKB_FRAGS + 1)))
		netif_wake_queue(cp->dev);
}

__IRAM_NIC
void checkTXDesc(void){
	struct re_private *cp = DEV2CP(eth_net_dev);
	if (likely(!atomic_read(&lock_tx_tail))) {
		atomic_set(&lock_tx_tail, 1);
		re8670_tx(cp);
		atomic_set(&lock_tx_tail, 0);
	}
}

#ifdef CONFIG_PORT_MIRROR
void nic_tx_mirror(struct sk_buff *skb)
{
	re8670_start_xmit(skb, eth_net_dev);
}
#endif

static 
__IRAM_NIC
void	tx_additional_setting(struct tx_info *pTxInfo){
	pTxInfo->opts2.bit.cputag = 1;
	TXINFO_DBG(pTxInfo);
}

__IRAM_NIC
static inline void apply_to_txdesc(DMA_TX_DESC  *txd, struct tx_info *pTxInfo){
	txd->addr = pTxInfo->addr;
	txd->opts2 = pTxInfo->opts2.dw;
	txd->opts3 = pTxInfo->opts3.dw;
	//must be last write....
	wmb();
	txd->opts1 = pTxInfo->opts1.dw;
}

int re8686_tx_with_Info(unsigned char *pPayload, unsigned short length, void *pInfo)
{
    struct sk_buff *skb;

    if(NULL == pPayload) {
        return -1;
    }

    skb = re8670_getAlloc(SKB_BUF_SIZE);
	if (unlikely(NULL == skb)) {
        return -1;
	}

    skb_put(skb, (length <= SKB_BUF_SIZE) ? length : SKB_BUF_SIZE);
    memcpy(skb->data, pPayload, (length <= SKB_BUF_SIZE) ? length : SKB_BUF_SIZE);

    if(NULL == pInfo) {
        return re8670_start_xmit(skb, eth_net_dev);
    }
    else {
        return re8686_send_with_txInfo(skb, (struct tx_info *)pInfo);
    }
}

int re8686_send_with_txInfo(struct sk_buff *skb, struct tx_info* ptxInfo)
{
	struct net_device *dev = eth_net_dev;
	struct re_private *cp = DEV2CP(dev);
	unsigned entry;
	u32 eor;
	unsigned long flags;

	skb->dev = dev;

	ETHDBG_PRINT( RTL8686_SKB_TX, "tx %s\n", dev->name );
	SKB_DBG(skb, debug_enable, RTL8686_SKB_TX);
	cp->cp_stats.tx_sw_num++;

	checkTXDesc();
	if(unlikely(eth_close == 1)) {
		dev_kfree_skb_any(skb);
		DEVPRIV(skb->dev)->net_stats.tx_dropped++;	
		return -1;
	}

	spin_lock_irqsave(&cp->lock, flags);

	if (unlikely(TX_HQBUFFS_AVAIL(cp) <= (skb_shinfo(skb)->nr_frags + 1)))
	{
		spin_unlock_irqrestore(&cp->lock, flags);
		dev_kfree_skb_any(skb);
		DEVPRIV(skb->dev)->net_stats.tx_dropped++;
		cp->cp_stats.tx_no_desc++;
		return -1;
	}

	entry = cp->tx_hqhead;
	eor = (entry == (RE8670_TX_RING_SIZE - 1)) ? RingEnd : 0;
	if (skb_shinfo(skb)->nr_frags == 0) {
		DMA_TX_DESC  *txd = &cp->tx_hqring[entry];
		u32 len;
		
		len = ETH_ZLEN < skb->len ? skb->len : ETH_ZLEN;				
		// Kaohj --- invalidate DCache before NIC DMA
		dma_cache_wback_inv((unsigned long)skb->data, len);
		cp->tx_skb[entry].skb = skb;
		cp->tx_skb[entry].mapping = (dma_addr_t)(skb->data);
		cp->tx_skb[entry].frag = 0;

		//default setting, always need this
		ptxInfo->addr = (u32)(skb->data);
		ptxInfo->opts1.dw = (eor | len | DescOwn | FirstFrag |LastFrag | TxCRC | IPCS | L4CS);

		//plz put tx additional setting into this function
		tx_additional_setting(ptxInfo);

		//apply to txdesc
		apply_to_txdesc(txd, ptxInfo);
		
		entry = NEXT_TX(entry);
	} else {
		//re8670_mFrag_xmit(skb, cp, &entry);
		printk("%s %d: not support frag xmit now\n", __func__, __LINE__);
		dev_kfree_skb_any(skb);
	}
	cp->tx_hqhead = entry;

	if (unlikely(TX_HQBUFFS_AVAIL(cp) <= 1))
		netif_stop_queue(dev);

	//for memory controller's write buffer
	write_buffer_flush();
	
	spin_unlock_irqrestore(&cp->lock, flags);
	wmb();
	cp->cp_stats.tx_hw_num++;
	RTL_W32(IO_CMD,iocmd_reg | TX_POLL);
	dev->trans_start = jiffies;
	return 0;
}

__IRAM_NIC int re8670_start_xmit (struct sk_buff *skb, struct net_device *dev)	//shlee temp, fix this later
{
	struct re_private *cp = DEV2CP(dev);
	unsigned entry;
	u32 eor;
	unsigned long flags;
	struct tx_info txInfo;

	memset(&txInfo, 0, sizeof(struct tx_info));

	ETHDBG_PRINT( RTL8686_SKB_TX, "tx %s\n", dev->name );
	SKB_DBG(skb, debug_enable, RTL8686_SKB_TX);
	cp->cp_stats.tx_sw_num++;

	checkTXDesc();
	if(unlikely(eth_close == 1)) {
		dev_kfree_skb_any(skb);
		DEVPRIV(skb->dev)->net_stats.tx_dropped++;	
		return 0;
	}

	spin_lock_irqsave(&cp->lock, flags);

	if (unlikely(TX_HQBUFFS_AVAIL(cp) <= (skb_shinfo(skb)->nr_frags + 1)))
	{
		spin_unlock_irqrestore(&cp->lock, flags);
		dev_kfree_skb_any(skb);
		DEVPRIV(skb->dev)->net_stats.tx_dropped++;
		cp->cp_stats.tx_no_desc++;
		return 0;
	}

	entry = cp->tx_hqhead;
	eor = (entry == (RE8670_TX_RING_SIZE - 1)) ? RingEnd : 0;
	if (skb_shinfo(skb)->nr_frags == 0) {
		DMA_TX_DESC  *txd = &cp->tx_hqring[entry];
		u32 len;
		
		len = ETH_ZLEN < skb->len ? skb->len : ETH_ZLEN;				
		// Kaohj --- invalidate DCache before NIC DMA
		dma_cache_wback_inv((unsigned long)skb->data, len);
		cp->tx_skb[entry].skb = skb;
		cp->tx_skb[entry].mapping = (dma_addr_t)(skb->data);
		cp->tx_skb[entry].frag = 0;

		//default setting, always need this
		txInfo.addr = (u32)(skb->data);
		txInfo.opts1.dw = (eor | len | DescOwn | FirstFrag |LastFrag | TxCRC | IPCS | L4CS);

		//plz put tx additional setting into this function
		tx_additional_setting(&txInfo);

		//apply to txdesc
		apply_to_txdesc(txd, &txInfo);
		
		entry = NEXT_TX(entry);
	} else {
		//re8670_mFrag_xmit(skb, cp, &entry);
		printk("%s %d: not support frag xmit now\n", __func__, __LINE__);
		dev_kfree_skb_any(skb);
	}
	cp->tx_hqhead = entry;

	if (unlikely(TX_HQBUFFS_AVAIL(cp) <= 1))
		netif_stop_queue(dev);

	//for memory controller's write buffer
	write_buffer_flush();
	
	spin_unlock_irqrestore(&cp->lock, flags);
	wmb();
	cp->cp_stats.tx_hw_num++;
	RTL_W32(IO_CMD,iocmd_reg | TX_POLL);
	dev->trans_start = jiffies;
	return 0;
}

/* Set or clear the multicast filter for this adaptor.
   This routine is not state sensitive and need not be SMP locked. */
static void __re8670_set_rx_mode (struct net_device *dev)
{
	int rx_mode;

	rx_mode = AcceptBroadcast | AcceptMulticast | AcceptMyPhys | AcceptAllPhys;

	/* We can safely update without stopping the chip. */
	// Kao, 2004/01/07
	RTL_W32(MAR0, 0xFFFFFFFF);
	RTL_W32(MAR4, 0xFFFFFFFF);
	RTL_W32(RCR, rx_mode);
}

static void re8670_set_rx_mode (struct net_device *dev)
{
	unsigned long flags;
	struct re_private *cp = DEV2CP(dev);
	spin_lock_irqsave (&cp->lock, flags);
	__re8670_set_rx_mode(dev);
	spin_unlock_irqrestore (&cp->lock, flags);
}

static struct net_device_stats *re8670_get_stats(struct net_device *dev)
{
	return &(DEVPRIV(dev)->net_stats);
}

static void re8670_stop_hw (struct re_private *cp)
{
	RTL_W32(IO_CMD,0); /* timer  rx int 1 packets*/
	RTL_W32(IO_CMD1,0); //czyao
	RTL_W16(IMR, 0);
	RTL_W16(ISR, 0xffff);
	//synchronize_irq();
	synchronize_irq(cp->dev->irq);/*linux-2.6.19*/
	udelay(10);

	cp->rx_tail = 0;
	cp->tx_hqhead = cp->tx_hqtail = 0;
}

static void re8670_reset_hw (struct re_private *cp)
{
	unsigned work = 1000;

	RTL_W8(CMD,RST);	 /* Reset */	
	while (work--) {
		if (!(RTL_R8(CMD) & RST))
			return;
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(10);
	}
	RTL_W8(CMD,RST);	 /* Reset */	
	while (work--) {
		if (!(RTL_R8(CMD) & RST))
			return;
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(10);
	}

	RTL_W8(CMD,RxChkSum);	 /* checksum */	

	printk(KERN_ERR "%s: hardware reset timeout\n", cp->dev->name);
}

static inline void re8670_start_hw (struct re_private *cp)
{
	RTL_W32(IO_CMD1,iocmd1_reg);
	RTL_W32(IO_CMD,iocmd_reg); /* timer  rx int 1 packets*/
	//printk("Start NIC in Pkt Processor disabled mode.. IO_CMD %x\n", RTL_R32(IO_CMD));	
}

static void re8670_init_hw (struct re_private *cp)
{
	struct net_device *dev = cp->dev;
	u8 status;
	// Kao
	u32 *hwaddr, cputag_info;
	u16 desc_l, desc_h;
	//unsigned int value;
	re8670_reset_hw(cp);
	RTL_W8(CMD,RxChkSum);	 /* checksum */		//shlee 8672
	RTL_W16(ISR,0xffff);/*clear all interrupt*/
	RTL_W16(IMR,RX_ALL);   	
	RTL_W32(RxFDP,(u32)(cp->rx_ring));	
	RTL_W32(TxFDP1,(u32)(cp->tx_hqring));
	RTL_W16(TxCDO1,0);
	RTL_W32(TxFDP2,0);	
	RTL_W16(TxCDO2,0);
	RTL_W32(TxFDP3,0);	
	RTL_W16(TxCDO3,0);
	RTL_W32(TxFDP4,0);	
	RTL_W16(TxCDO4,0);

	// Kao
	RTL_W32(TCR,(u32)(0x0C00));
	RTL_W32(CPUtagCR,(u32)(0x0000));  /* Turn off CPU tag function */
	//cpu tag function
	cputag_info = (CTEN_RX | 2<<CT_RSIZE_L | 3<<CT_TSIZE |CT_APPLO |CTPM_8370 | CTPV_8370);
	RTL_W32(CPUtagCR,cputag_info); /* Turn on the cpu tag adding function */  //czyao 8672c
	RTL_W32(CPUtag1CR, 0x00000073);/*spa_dsl no need now, spa_pon is 3[from switch team]*/

	RTL_W32(EthrntRxCPU_Des_Num,0);  //initialize the register content
	RTL_W8(EthrntRxCPU_Des_Wrap,(TH_ON_VAL%256));
	RTL_W8(Rx_Pse_Des_Thres,(TH_OFF_VAL%256));
	desc_l = (RE8670_RX_RING_SIZE > 256) ? 256 : RE8670_RX_RING_SIZE;
	desc_h = (RE8670_RX_RING_SIZE > 256) ? (RE8670_RX_RING_SIZE/256)%16 : 1;
	RTL_W8(EthrntRxCPU_Des_Num,desc_l-1);
	RTL_W8(RxRingSize,desc_l-1);

	RTL_W8(EthrntRxCPU_Des_Wrap_h,(TH_ON_VAL/256)%16);
	RTL_W8(Rx_Pse_Des_Thres_h,(TH_OFF_VAL/256)%16);
	RTL_W8(EthrntRxCPU_Des_Num_h,(RTL_R8(EthrntRxCPU_Des_Num_h)|(desc_h-1)<<4));
	RTL_W8(RxRingSize_h,desc_h-1);
	RTL_W32(SMSA,0);

	status = RTL_R8(MSR);
	status = status | (TXFCE|FORCE_TX);	// enable tx flowctrl
	status = status | RXFCE;
	RTL_W8(MSR,status);	
	// Kao, set hw ID for physical match
	hwaddr = (u32 *)cp->dev->dev_addr;
	RTL_W32(IDR0, *hwaddr);
	hwaddr = (u32 *)(cp->dev->dev_addr+4);
	RTL_W32(IDR4, *hwaddr);

	RTL_W32(CONFIG_REG, Rff_size_sel_2k);

	re8670_start_hw(cp);
	__re8670_set_rx_mode(dev);
}

static int re8670_refill_rx (struct re_private *cp)
{
	unsigned i;

	for (i = 0; i < RE8670_RX_RING_SIZE; i++) {
		struct sk_buff *skb;
		skb = re8670_getAlloc(SKB_BUF_SIZE);
		if (!skb)
			goto err_out;
		// Kaohj --- invalidate DCache for uncachable usage
		//ql_xu
		dma_cache_wback_inv((unsigned long)skb->data, SKB_BUF_SIZE);
		skb->dev = cp->dev;
		cp->rx_skb[i].skb = skb;
		cp->rx_skb[i].frag = 0;
		if ((u32)skb->data &0x3)
			printk(KERN_DEBUG "skb->data unaligment %8x\n",(u32)skb->data);

		cp->rx_ring[i].addr = (u32)skb->data|UNCACHE_MASK;
		if (i == (RE8670_RX_RING_SIZE - 1))
			cp->rx_ring[i].opts1 = (DescOwn | RingEnd | cp->rx_buf_sz);
		else
			cp->rx_ring[i].opts1 =(DescOwn | cp->rx_buf_sz);
		cp->rx_ring[i].opts2 = 0;
	}
	return 0;
err_out:
	re8670_clean_rings(cp);
	return -ENOMEM;
}

static void re8670_tx_timeout (struct net_device *dev)
{
	//unsigned long flags;
	struct re_private *cp = DEV2CP(dev);
	#if 0//krammer test
	unsigned tx_head;
	unsigned tx_tail;
	
	cp->cp_stats.tx_timeouts++;

	spin_lock_irqsave(&cp->lock, flags);
	tx_head = cp->tx_hqhead;
	tx_tail = cp->tx_hqtail;
	while (tx_tail != tx_head) {
		struct sk_buff *skb;
		u32 status;
		rmb();
		status = (cp->tx_hqring[tx_tail].opts1);
		if (status & DescOwn)
			break;
		skb = cp->tx_skb[tx_tail].skb;
		if (!skb)
			BUG();
		DEVPRIV(dev)->net_stats.tx_packets++;	

		dev_kfree_skb(skb);
		cp->tx_skb[tx_tail].skb = NULL;
		tx_tail = NEXT_TX(tx_tail);
	}

	cp->tx_hqtail = tx_tail;

	spin_unlock_irqrestore(&cp->lock, flags);
	#endif
	if (netif_queue_stopped(cp->dev))
		netif_wake_queue(cp->dev);

}

static int re8670_init_rings (struct re_private *cp)
{
	cp->rx_tail = 0;
	cp->tx_hqhead = cp->tx_hqtail = 0;

	return re8670_refill_rx (cp);
}

static int re8670_alloc_rings (struct re_private *cp)
{
	void*	pBuf;
	pBuf = kzalloc(RE8670_RXRING_BYTES, GFP_KERNEL|__GFP_NOFAIL);
	if (!pBuf)
		goto ErrMem;
	dma_cache_wback_inv((unsigned long)pBuf, RE8670_RXRING_BYTES);
	cp->rxdesc_buf = pBuf;
	pBuf = (void*)( (u32)(pBuf + DESC_ALIGN-1) &  ~(DESC_ALIGN -1) ) ;
	cp->rx_ring = (DMA_RX_DESC*)((u32)(pBuf) | UNCACHE_MASK);
	
	pBuf = kzalloc(RE8670_TXRING_BYTES, GFP_KERNEL|__GFP_NOFAIL);
	if (!pBuf)
		goto ErrMem;
	dma_cache_wback_inv((unsigned long)pBuf, RE8670_TXRING_BYTES);
	cp->txdesc_buf = pBuf;
	pBuf = (void*)( (u32)(pBuf + DESC_ALIGN-1) &  ~(DESC_ALIGN -1) ) ;
	cp->tx_hqring = (DMA_TX_DESC*)((u32)(pBuf) | UNCACHE_MASK);

	return re8670_init_rings(cp);

ErrMem:
	if (cp->rxdesc_buf)
		kfree(cp->rxdesc_buf);
	if (cp->txdesc_buf)
		kfree(cp->txdesc_buf);
	return -ENOMEM;


}

static void re8670_clean_rings (struct re_private *cp)
{
	unsigned i;
	for (i = 0; i < RE8670_RX_RING_SIZE; i++) {
		if (cp->rx_skb[i].skb) {
			dev_kfree_skb(cp->rx_skb[i].skb);
		}
	}
	for (i = 0; i < RE8670_TX_RING_SIZE; i++) {
		if (cp->tx_skb[i].skb) {
			struct sk_buff *skb = cp->tx_skb[i].skb;
			dev_kfree_skb(skb);
			DEVPRIV(skb->dev)->net_stats.tx_dropped++;
		}
	}
	memset(&cp->rx_skb, 0, sizeof(struct ring_info) * RE8670_RX_RING_SIZE);
	memset(&cp->tx_skb, 0, sizeof(struct ring_info) * RE8670_TX_RING_SIZE);
}

static void re8670_free_rings (struct re_private *cp)
{
	re8670_clean_rings(cp);
	/*pci_free_consistent(cp->pdev, CP_RING_BYTES, cp->rx_ring, cp->ring_dma);*/
	kfree(cp->rxdesc_buf);
	kfree(cp->txdesc_buf);

	cp->rx_ring = NULL;
	cp->tx_hqring = NULL;
}

static int re8670_open (struct net_device *dev)
{
	struct re_private *cp = DEV2CP(dev);
	int rc;

	eth_close=0;

	if (netif_msg_ifup(cp))
		printk(KERN_DEBUG "%s: enabling interface\n", dev->name);

	if(dev_num == 0){
		printk("%s %d\n", __func__, __LINE__);
		re8670_set_rxbufsize(cp);	/* set new rx buf size */
		rc = re8670_alloc_rings(cp);
		if (rc)
			return rc;

		re8670_init_hw(cp);
		rc = request_irq(dev->irq, (irq_handler_t)re8670_interrupt, IRQF_DISABLED, dev->name, dev);
		if (rc)
			goto err_out_hw;
	}
	dev_num++;
	netif_start_queue(dev);
	return 0;

err_out_hw:
	re8670_stop_hw(cp);
	re8670_free_rings(cp);
	return rc;
}

static int re8670_close (struct net_device *dev)
{
	struct re_private *cp = DEV2CP(dev);
	dev_num--;

	if(dev_num == 0){
		eth_close=1;
		netif_stop_queue(dev);
		re8670_stop_hw(cp);
		free_irq(dev->irq, dev);
		re8670_free_rings(cp);
	}

	if (netif_msg_ifdown(cp))
		printk(KERN_DEBUG "%s: disabling interface\n", dev->name);

	return 0;
}

// Mason Yu. For Set IPQOS
#define		SETIPQOS		0x01

/*
 * Structure used in SIOCSIPQOS request.
 */

struct ifIpQos
{
	int	cmd;
	char	enable;	
};

/*
static void re8670_ipqos_ioctl(struct ifIpQos *req)
{
	switch(req->cmd) {
		case SETIPQOS:
			break;
		default: 
			break;
	}
}
*/

// Mason Yu. combine_1p_4p_PortMapping
//#ifdef CONFIG_EXT_SWITCH
#define  VLAN_ENABLE 0x01
#define  VLAN_SETINFO 0x02
#define  VLAN_SETPVIDX 0x03
#define  VLAN_SETTXTAG 0x04
#define  VLAN_DISABLE1PPRIORITY 0x05
#define	 VLAN_SETIGMPSNOOP	0x06
#define	 VLAN_SETPORTMAPPING	0x07
#define	 VLAN_SETIPQOS		0x08
#define	 VLAN_VIRTUAL_PORT	0x09
#define	 VLAN_SETVLANGROUPING	0x0a
#ifdef CONFIG_PPPOE_PROXY
#define    SET_PPPOE_PROXY_PORTMAP  0x0b 
#endif
#define		VLAN_SETMLDSNOOP		0x0c		// Mason Yu. MLD snooping

// Kaohj
/*
 * Structure used in SIOCSIFVLAN request.
 */

struct ifvlan
{
	int cmd;
	char enable;
	short vlanIdx;
	short vid;
	char	disable_priority;
	int member;
	int port;
	char txtag;
};

void pp_ifgroup_setup(void);
/*
static void re8670_8305vlan_portMapping_ioctl(struct ifvlan *req)
{
	switch(req->cmd) {
		// Mason Yu. combine_1p_4p_PortMapping
		case VLAN_ENABLE:
			break;
		case VLAN_SETINFO:
			break;
		case VLAN_SETPVIDX:
			break;
		case VLAN_SETTXTAG:
			break;
		case VLAN_DISABLE1PPRIORITY:
			break;
		case VLAN_SETIGMPSNOOP:
			break;
			// Mason Yu. MLD snooping
#ifdef CONFIG_IPV6
		case VLAN_SETMLDSNOOP:
			break;
#endif
		case VLAN_SETPORTMAPPING:
			break;
		case VLAN_SETVLANGROUPING:
			break;
		case VLAN_SETIPQOS:
			break;
		case VLAN_VIRTUAL_PORT:
			break;
		default: 
			break;
	}
}
//#endif  // #ifdef CONFIG_EXT_SWITCH
*/

int re8670_ioctl (struct net_device *dev, struct ifreq *rq, int cmd)
{
	//struct re_private *cp = dev->priv;
	int rc = 0;

	if (!netif_running(dev) && cmd!=SIOCETHTEST)
		return -EINVAL;

	switch (cmd) {
		case SIOCETHTOOL:
			break;
		case SIOCGMIIPHY:
			break;
		case SIOCGMIIREG:
			break;	
		case SIOCS8305VLAN:
			break;
		case SIOCDIRECTBR:  // ioctl for direct bridge mode, jiunming
			rc = -EINVAL;
			break;
		case SIOCGMEDIALS:	// Jenny, ioctl for media link status
			break;
		case SIOCETHTEST:
			break;
			// Mason Yu. For Set IPQOS
		case SIOCSIPQOS:		
			break;
		default:
			rc = -EOPNOTSUPP;
			break;
	}

	return rc;
}

static int dbg_level_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "[debug_enable = 0x%08x]\n", debug_enable);
	len += sprintf(page + len, "RTL8686_PRINT_NOTHING\t%08x\n", RTL8686_PRINT_NOTHING);
	len += sprintf(page + len, "RTL8686_SKB_RX\t\t0x%08x\n", RTL8686_SKB_RX);
	len += sprintf(page + len, "RTL8686_SKB_TX\t\t0x%08x\n", RTL8686_SKB_TX);
	len += sprintf(page + len, "RTL8686_RXINFO\t\t0x%08x\n", RTL8686_RXINFO);
	len += sprintf(page + len, "RTL8686_TXINFO\t\t0x%08x\n", RTL8686_TXINFO);

	return len;
}
static int dbg_level_write(struct file *file, const char *buffer, unsigned long count, void *data)
{
	unsigned char tmpBuf[16] = {0};
	int len = (count > 15) ? 15 : count;

	if (buffer && !copy_from_user(tmpBuf, buffer, len))
	{
		debug_enable=simple_strtoul(tmpBuf, NULL, 16);
		printk("write debug_enable to 0x%08x\n", debug_enable);
		return count;
	}
	return -EFAULT;
}

static int memrw_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	return 0;
}
static int memrw_write(struct file *file, const char *buff, unsigned long len, void *data)
{
	char 		tmpbuf[64];
	unsigned int	*mem_addr, mem_data, mem_len;
	char		*strptr, *cmd_addr;
	char		*tokptr;

	if (buff && !copy_from_user(tmpbuf, buff, len)) {
		tmpbuf[len] = '\0';
		strptr=tmpbuf;
		cmd_addr = strsep(&strptr," ");
		if (cmd_addr==NULL)
		{
			goto errout;
		}
		printk("cmd %s\n", cmd_addr);
		tokptr = strsep(&strptr," ");
		if (tokptr==NULL)
		{
			goto errout;
		}

		if (!memcmp(cmd_addr, "r", 1))
		{
			mem_addr=(unsigned int*)simple_strtol(tokptr, NULL, 0);
			tokptr = strsep(&strptr," ");
			if (tokptr==NULL)
			{
				goto errout;
			}
			mem_len=simple_strtol(tokptr, NULL, 0);
			memDump(mem_addr, mem_len, "");
		}
		else if (!memcmp(cmd_addr, "w", 1))
		{
			mem_addr=(unsigned int*)simple_strtol(tokptr, NULL, 0);
			tokptr = strsep(&strptr," ");
			if (tokptr==NULL)
			{
				goto errout;
			}
			mem_data=simple_strtol(tokptr, NULL, 0);
			WRITE_MEM32(mem_addr, mem_data);
			printk("Write memory 0x%p dat 0x%x: 0x%x\n", mem_addr, mem_data, READ_MEM32(mem_addr));
		}
		else
		{
			goto errout;
		}
	}
	else
	{
errout:
		printk("Memory operation only support \"r\" and \"w\" as the first parameter\n");
		printk("Read format:	\"r mem_addr length\"\n");
		printk("Write format:	\"w mem_addr mem_data\"\n");
	}

	return len;
}

static int hw_reg_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;

	printk("ETHBASE		=0x%08x\n", ETHBASE);
	printk("IDR		=%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x\n", 
		RTL_R8(IDR0), RTL_R8(IDR1), RTL_R8(IDR2), RTL_R8(IDR3), RTL_R8(IDR4), RTL_R8(IDR5));
	printk("MAR		=%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x%2.2x\n", 
		RTL_R8(MAR0), RTL_R8(MAR1), RTL_R8(MAR2), RTL_R8(MAR3), 
		RTL_R8(MAR4), RTL_R8(MAR5), RTL_R8(MAR6), RTL_R8(MAR7));
	printk("TXOKCNT		=0x%04x		RXOKCNT		=0x%04x\n", RTL_R16(TXOKCNT), RTL_R16(RXOKCNT));
	printk("TXERR		=0x%04x		RXERRR		=0x%04x\n", RTL_R16(TXERR), RTL_R16(RXERRR));
	printk("MISSPKT		=0x%04x		FAE		=0x%04x\n", RTL_R16(MISSPKT), RTL_R16(FAE));
	printk("TX1COL		=0x%04x		TXMCOL		=0x%04x\n", RTL_R16(TX1COL), RTL_R16(TXMCOL));
	printk("RXOKPHY		=0x%04x		RXOKBRD		=0x%04x\n", RTL_R16(RXOKPHY), RTL_R16(RXOKBRD));
	printk("RXOKMUL		=0x%04x		TXABT		=0x%04x\n", RTL_R16(RXOKMUL), RTL_R16(TXABT));
	printk("TXUNDRN		=0x%04x		TRSR		=0x%08x\n", RTL_R16(TXUNDRN), RTL_R32(TRSR));
	printk("CMD		=0x%02x		IMR		=0x%04x\n", RTL_R8(CMD), RTL_R16(IMR));
	printk("ISR		=0x%04x		TCR		=0x%08x\n", RTL_R16(ISR), RTL_R32(TCR));
	printk("RCR		=0x%08x	CPUtagCR	=0x%08x\n", RTL_R32(RCR), RTL_R32(CPUtagCR));
	printk("CONFIG_REG	=0x%08x	CPUtag1CR	=0x%08x\n", RTL_R32(CONFIG_REG), RTL_R32(CPUtag1CR));
	printk("MSR		=0x%08x	VLAN_REG	=0x%08x\n", RTL_R32(MSR), RTL_R32(VLAN_REG));
	printk("LEDCR		=0x%08x\n", RTL_R32(LEDCR));
	printk("TxFDP1		=0x%08x	TxCDO1		=0x%04x\n", RTL_R32(TxFDP1), RTL_R16(TxCDO1));
	printk("TxFDP2		=0x%08x	TxCDO2		=0x%04x\n", RTL_R32(TxFDP2), RTL_R16(TxCDO2));
	printk("TxFDP3		=0x%08x	TxCDO3		=0x%04x\n", RTL_R32(TxFDP3), RTL_R16(TxCDO3));
	printk("TxFDP4		=0x%08x	TxCDO4		=0x%04x\n", RTL_R32(TxFDP4), RTL_R16(TxCDO4));
	printk("RxFDP		=0x%08x	RxCDO		=0x%04x\n", RTL_R32(RxFDP), RTL_R16(RxCDO));
	printk("RxRingSize	=0x%04x		SMSA		=0x%08x\n", RTL_R16(RxRingSize), RTL_R32(SMSA));
	printk("EthrntRxCPU_Des_Num	=0x%02x	EthrntRxCPU_Des_Wrap	=0x%02x\n", 
		RTL_R8(EthrntRxCPU_Des_Num), RTL_R8(EthrntRxCPU_Des_Wrap));
	printk("Rx_Pse_Des_Thres	=0x%02x	EthrntRxCPU_Des_Num_h	=0x%02x\n", 
		RTL_R8(Rx_Pse_Des_Thres), RTL_R8(EthrntRxCPU_Des_Num_h));
	printk("Rx_Pse_Des_Thres_h	=0x%02x	IO_CMD	=0x%08x\n", 
		RTL_R8(Rx_Pse_Des_Thres_h), RTL_R32(IO_CMD));
	printk("IO_CMD1		=0x%08x\n", RTL_R32(IO_CMD1));

	return len;
}
static int sw_cnt_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;

	printk("rx_hw_num	=0x%08x		rx_sw_num	=0x%08x\n", 
		re_private_data.cp_stats.rx_hw_num, re_private_data.cp_stats.rx_sw_num);
	printk("rer_runt	=0x%08x		rer_ovf		=0x%08x\n", 
		re_private_data.cp_stats.rer_runt, re_private_data.cp_stats.rer_ovf);
	printk("rdu		=0x%08x		frag		=0x%08x\n", 
		re_private_data.cp_stats.rdu, re_private_data.cp_stats.frag);
	printk("crcerr		=0x%08x		rcdf		=0x%08x\n", 
		re_private_data.cp_stats.crcerr, re_private_data.cp_stats.rcdf);
	printk("rx_no_mem	=0x%08x		tx_sw_num	=0x%08x\n", 
		re_private_data.cp_stats.rx_no_mem, re_private_data.cp_stats.tx_sw_num);
	printk("tx_hw_num	=0x%08x		tx_no_desc	=0x%08x\n", 
		re_private_data.cp_stats.tx_hw_num, re_private_data.cp_stats.tx_no_desc);

	return len;
}
static int rx_ring_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	int i = 0;

	if(re_private_data.rx_ring == 0 || re_private_data.rx_skb == 0){
		printk("no rx_ring info!\n");
		return len;
	}

	for(i=0;i<RE8670_RX_RING_SIZE;i++){
		printk("[idx%3d]:desc[0x%p]->skb[0x%p]->buf[0x%08x]:%s", 
			i, &re_private_data.rx_ring[i], re_private_data.rx_skb[i].skb, 
			re_private_data.rx_ring[i].addr, 
			(re_private_data.rx_ring[i].opts1 & DescOwn)? "NIC" : "CPU");
		if(i == re_private_data.rx_tail){
			printk("<=rx_tail");
		}
		printk("\n");
	}

	return len;
}
static int tx_ring_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
	int len = 0;
	int i = 0;

	if(re_private_data.tx_hqring == 0 || re_private_data.tx_skb == 0){
		printk("no tx_ring info\n");
		return len;
	}

	for(i=0;i<RE8670_TX_RING_SIZE;i++){
		printk("[idx%3d]:desc[0x%p]->skb[0x%p]->buf[0x%08x]:%s", 
			i, &re_private_data.tx_hqring[i], re_private_data.tx_skb[i].skb, 
			re_private_data.tx_hqring[i].addr, 
			(re_private_data.tx_hqring[i].opts1 & DescOwn)? "NIC" : "CPU");
		if(i == re_private_data.tx_hqtail){
			printk("<=tx_hqtail");
		}
		if(i == re_private_data.tx_hqhead){
			printk("<=tx_hqhead");
		}
		printk("\n");
	}

	return len;
}

#define RTL8686_PROC_DIR_NAME "rtl8686gmac"
struct proc_dir_entry *rtl8686_proc_dir=NULL;
static struct proc_dir_entry *dbg_level, *hw_reg, *sw_cnt, *rx_ring, *tx_ring, *memrw;

static void rtl8686_proc_debug_init(void){
	if(rtl8686_proc_dir==NULL)
		rtl8686_proc_dir = proc_mkdir(RTL8686_PROC_DIR_NAME,NULL);
	if(rtl8686_proc_dir)
	{
		dbg_level = create_proc_entry("dbg_level",0,rtl8686_proc_dir);
		if(dbg_level != NULL)
		{
			dbg_level->read_proc = dbg_level_read;
			dbg_level->write_proc= dbg_level_write;			
		}
		else
		{
			printk("can't create proc entry for dbg_level\n");
		}

		memrw = create_proc_entry("mem",0,rtl8686_proc_dir);
		if(memrw != NULL)
		{
			memrw->read_proc = memrw_read;
			memrw->write_proc= memrw_write;			
		}
		else
		{
			printk("can't create proc entry for mem\n");
		}
		
		hw_reg = create_proc_read_entry("hw_reg", 0444, rtl8686_proc_dir, hw_reg_read, NULL);
		if(hw_reg == NULL) {
			printk("can't create proc entry for hw_reg\n");
		}

		sw_cnt = create_proc_read_entry("sw_cnt", 0444, rtl8686_proc_dir, sw_cnt_read, NULL);
		if(sw_cnt == NULL) {
			printk("can't create proc entry for sw_cnt\n");
		}

		rx_ring = create_proc_read_entry("rx_ring", 0444, rtl8686_proc_dir, rx_ring_read, NULL);
		if(rx_ring == NULL) {
			printk("can't create proc entry for rx_ring\n");
		}

		tx_ring = create_proc_read_entry("tx_ring", 0444, rtl8686_proc_dir, tx_ring_read, NULL);
		if(tx_ring == NULL) {
			printk("can't create proc entry for tx_ring\n");
		}
	}
}

static struct rtl8686_dev_table_entry rtl8686_dev_table[] = {
	//ifname, isWan, vid, phyPort, to_dev, dev_instant
	{"eth",	0, 0, 6, 0, NULL},//root dev eth0 must be first
	{"eth0.2",	0, 0, 0, 0, NULL},
	{"eth0.3",	0, 0, 1, 0, NULL},
	{"eth0.4",	0, 0, 4, 0, NULL},
	{"eth0.5",	0, 0, 5, 0, NULL},
	{"nas",	1, 0, 3, 0, NULL},
	{"pon",	1, 0, 2, 6, NULL},
};

void fill_phy2dev_table(void){
	int i, j;
	unsigned int totalDev = ((sizeof(rtl8686_dev_table))/(sizeof(struct rtl8686_dev_table_entry)));
	unsigned int totalPortTable = (sizeof(port2dev)/sizeof(struct net_device*));
	//default set to eth0
	for(i=0;i<totalPortTable;i++){
		port2dev[i] = eth_net_dev;
	}
	for(i=0;i<totalDev;i++){
		for(j=0;j<totalDev;j++){
			if(rtl8686_dev_table[j].phyPort == i){
				port2dev[i] = rtl8686_dev_table[rtl8686_dev_table[j].to_dev].dev_instant;
				printk("port %d use dev %s\n", i, (port2dev[i])->name);
				break;
			}
		}
	}
}

int __init re8670_probe (void)
{
#ifdef MODULE
	printk("%s", version);
#endif
	struct net_device *dev;
	int rc;
	void *regs=(void*)ETHBASE;	
	unsigned i, j;
	unsigned int totalDev = ((sizeof(rtl8686_dev_table))/(sizeof(struct rtl8686_dev_table_entry)));
#ifndef MODULE
	static int version_printed;
	if (version_printed++ == 0)
		printk("%s", version);
#endif

	//allocate and register root dev first
	dev = alloc_etherdev(sizeof(struct re_dev_private));
	if (!dev){
		printk("%s %d\n", __func__, __LINE__);
		goto err_out_iomap;
	}

	//reset re_private_data
	memset(&re_private_data, 0, sizeof(struct re_private));
	re_private_data.dev = dev;
	spin_lock_init (&(re_private_data.lock));
	re_private_data.regs = regs;
	memset(&re_private_data.rx_tasklets, 0, sizeof(struct tasklet_struct));
	re_private_data.rx_tasklets.func=(void (*)(unsigned long))re8670_rx;
	re_private_data.rx_tasklets.data=(unsigned long)&re_private_data;
	eth_rx_tasklets = &(re_private_data.rx_tasklets);
	eth_net_dev = dev;

    //Add for debug usage
    memset(&re_dbg_data, 0, sizeof(struct pkt_dbg_s));
    
	//stop hw
	re8670_stop_hw(&re_private_data);
	
	dev->base_addr = (unsigned long) regs;
	/* read MAC address from EEPROM */
	for (i = 0; i < 3; i++)
		((u16 *) (dev->dev_addr))[i] = i;

	dev->open = re8670_open;
	dev->stop = re8670_close;
	dev->set_multicast_list = re8670_set_rx_mode;
	dev->hard_start_xmit = re8670_start_xmit;
	dev->get_stats = re8670_get_stats;
	dev->do_ioctl = re8670_ioctl;
	dev->tx_timeout = re8670_tx_timeout;
	dev->watchdog_timeo = TX_TIMEOUT;
	dev->irq = BSP_GMAC_IRQ;	// internal phy
	//priv data setting
	dev->priv_flags = rtl8686_dev_table[0].isWan ? IFF_DOMAIN_WAN : IFF_DOMAIN_ELAN;
	DEV2CP(dev) = &re_private_data;
	rtl8686_dev_table[0].dev_instant = dev;

	memcpy(dev->name, rtl8686_dev_table[0].ifname, strlen(rtl8686_dev_table[0].ifname));
	rc = register_netdev(dev);
	if (rc){
		printk("%s %d rc = %d\n", __func__, __LINE__, rc);
		goto err_out_iomap;
	}

	printk (KERN_INFO "%s: %s at 0x%lx, "
			"%02x:%02x:%02x:%02x:%02x:%02x, "
			"IRQ %d\n",
			dev->name,
			"RTL-8686",
			dev->base_addr,
			dev->dev_addr[0], dev->dev_addr[1],
			dev->dev_addr[2], dev->dev_addr[3],
			dev->dev_addr[4], dev->dev_addr[5],
			dev->irq);
	
	//allocate and register other device
	for(j=1; j < totalDev; j++){
		dev = alloc_etherdev(sizeof(struct re_dev_private));
		if (!dev){
			printk("%s %d\n", __func__, __LINE__);
			goto err_out_iomap;
		}
		dev->base_addr = (unsigned long) regs;
		/* read MAC address from EEPROM */
		for (i = 0; i < 3; i++)
			((u16 *) (dev->dev_addr))[i] = i;

		dev->open = re8670_open;
		dev->stop = re8670_close;
		dev->set_multicast_list = re8670_set_rx_mode;
		dev->hard_start_xmit = re8670_start_xmit;
		dev->get_stats = re8670_get_stats;
		dev->do_ioctl = re8670_ioctl;
		dev->tx_timeout = re8670_tx_timeout;
		dev->watchdog_timeo = TX_TIMEOUT;
		dev->irq = BSP_GMAC_IRQ;	// internal phy
		// priv data setting
		dev->priv_flags = rtl8686_dev_table[j].isWan ? IFF_DOMAIN_WAN : IFF_DOMAIN_ELAN;
		DEV2CP(dev) = &re_private_data;
		rtl8686_dev_table[j].dev_instant = dev;

		memcpy(dev->name, rtl8686_dev_table[j].ifname, strlen(rtl8686_dev_table[j].ifname));
		rc = register_netdev(dev);
		if (rc){
			printk("%s %d rc = %d\n", __func__, __LINE__, rc);
			goto err_out_iomap;
		}

		printk (KERN_INFO "%s: %s at 0x%lx, "
				"%02x:%02x:%02x:%02x:%02x:%02x, "
				"IRQ %d\n",
				dev->name,
				"RTL-8686",
				dev->base_addr,
				dev->dev_addr[0], dev->dev_addr[1],
				dev->dev_addr[2], dev->dev_addr[3],
				dev->dev_addr[4], dev->dev_addr[5],
				dev->irq);
	}

	printk("RTL8686 GMAC Probing..\n");	//shlee 2.6

	//sw stuff
#ifdef CONFIG_RTL865X_ETH_PRIV_SKB
	init_priv_eth_skb_buf();
#endif
	rtl8686_proc_debug_init();
	fill_phy2dev_table();
	return 0;

err_out_iomap:
	printk("%s %d: here is a error when probe!!!!!!!!!!0rzzzzzzzzzzzzzzzz\n", __func__, __LINE__);
	return -1 ;
}

static void __exit re8670_exit (void)
{
}
module_init(re8670_probe);
module_exit(re8670_exit);

//need this?
int bitmap_virt2phy(int mbr)
{
#ifdef CONFIG_EXT_SWITCH
	int k, mask, phymap, phyid;
	
	phymap = mbr&0xffffffc0;
	
	for (k=0; k<=SW_PORT_NUM; k++) {
		mask = mbr & (1<<k);
		if (mask) {
			phyid = virt2phy[k];
			if (phyid >= 6)
				phyid = 5;
			phymap |= (1 << phyid);
		}
	}
	return phymap;
// Mason Yu. combine_1p_4p_PortMapping
#else
	return mbr;
#endif
}

