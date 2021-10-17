/*	
 *	re8686.h
*/
#ifndef _RE8686_H_
#define _RE8686_H_

#ifdef CONFIG_RTK_VOIP
#if defined(CONFIG_6166_IAD_SILAB3217X) || defined(CONFIG_6166_IAD_ZARLINK)
#define CONFIG_INT_PHY 1
#endif
#else
#define CONFIG_INT_PHY 1
#endif

#define VPORT_CPU_TAG	0
#define VPORT_VLAN_TAG	1

#define ETHBASE	0xB8012000	//internal phy

#define RTL_W32(reg, value)			(*(volatile u32*)(ETHBASE+reg)) = (u32)value
#define RTL_W16(reg, value)			(*(volatile u16*)(ETHBASE+reg)) = (u16)value
#define RTL_W8(reg, value)			(*(volatile u8*)(ETHBASE+reg)) = (u8)value
#define RTL_R32(reg)				(*(volatile u32*)(ETHBASE+reg))
#define RTL_R16(reg)				(*(volatile u16*)(ETHBASE+reg))
#define RTL_R8(reg)					(*(volatile u8*)(ETHBASE+reg))

//for alignment issue
#define READWD(addr) ((unsigned char *)addr)[0]<<24 | ((unsigned char *)addr)[1]<<16 | ((unsigned char *)addr)[2]<<8 | ((unsigned char *)addr)[3]
#define READHWD(addr) ((unsigned char *)addr)[0]<<8 | ((unsigned char *)addr)[1]


//#define RE8670_RX_RING_SIZE	256//plz use the order of 2
#define RE8670_RX_RING_SIZE	512//plz use the order of 2
//#define RE8670_TX_RING_SIZE	128//64//512//plz use the order of 2
#define RE8670_TX_RING_SIZE	256//64//512//plz use the order of 2 //mark_apo

enum {
	/* NIC register offsets */
	IDR0 = 0,			/* Ethernet ID */
	IDR1 = 0x1,			/* Ethernet ID */
	IDR2 = 0x2,			/* Ethernet ID */
	IDR3 = 0x3,			/* Ethernet ID */
	IDR4 = 0x4,			/* Ethernet ID */
	IDR5 = 0x5,			/* Ethernet ID */
	MAR0 = 0x8,			/* Multicast register */
	MAR1 = 0x9,
	MAR2 = 0xa,
	MAR3 = 0xb,
	MAR4 = 0xc,
	MAR5 = 0xd,
	MAR6 = 0xe,
	MAR7 = 0xf,
	TXOKCNT=0x10,
	RXOKCNT=0x12,
	TXERR = 0x14,
	RXERRR = 0x16,
	MISSPKT = 0x18,
	FAE=0x1A,
	TX1COL = 0x1c,
	TXMCOL=0x1e,
	RXOKPHY=0x20,
	RXOKBRD=0x22,
	RXOKMUL=0x24,
	TXABT=0x26,
	TXUNDRN=0x28,
	TRSR=0x34,
	COM_REG=0x38,
	CMD=0x3B,
	IMR=0x3C,
	ISR=0x3E,
	TCR=0x40,
	RCR=0x44,
	CPUtagCR=0x48,
	CONFIG_REG=0x4C,	
	CPUtag1CR=0x50,
	MSR=0x58,
	MIIAR=0x5C,
	SWINT_REG=0x60,
	VLAN_REG=0x64,
	LEDCR=0x70,
	TxFDP1=0x1300,
	TxCDO1=0x1304,
    TxFDP2=0x1310, 
	TxCDO2=0x1314, 
	TxFDP3=0x1320, 
	TxCDO3=0x1324, 
	TxFDP4=0x1330, 
	TxCDO4=0x1334,
	RxFDP=0x13F0,
	RxCDO=0x13F4,	
	RxRingSize=0x13F6,
	SMSA=0x13FC,
	EthrntRxCPU_Des_Num=0x1430,
	EthrntRxCPU_Des_Wrap =	0x1431,
	Rx_Pse_Des_Thres = 	0x1432,	
	RxRingSize_h=0x13F7, 
	EthrntRxCPU_Des_Num_h=0x1433,
	EthrntRxCPU_Des_Wrap_h =0x1433,
	Rx_Pse_Des_Thres_h =0x142c,


	RST = (1<<0),
	RxChkSum = (1<<1),
	RxVLAN_Detag = (1<<2),
	IO_CMD = 0x1434,
	IO_CMD1 = 0x1438,  // RLE0371 and other_platform set different value.
	RX_IntNum_Shift = 0x8,             /// ????
	TX_OWN = (1<<5),
	RX_OWN = (1<<4),
	MII_RE = (1<<3),
	MII_TE = (1<<2),
	TX_FNL = (1<<1),
	TX_FNH = (1),
	/*TX_START= MII_RE | MII_TE | TX_FNL | TX_FNH,
	TX_START = 0x8113c,*/
	RXMII = MII_RE,
	TXMII= MII_TE,
	LSO_F	=(1<<28),
	EN_1GB	=(1<<24),

	Rff_size_sel_2k = (0x2 << 28),
};

enum RE8670_IOCMD_REG
{

	RX_MIT 		= 7,
	RX_TIMER 	= 1,
	RX_FIFO 	= 2,
	TX_FIFO		= 1,
	TX_MIT		= 7,
	TX_POLL4	= 1 << 3,
	TX_POLL3	= 1 << 2,
	TX_POLL2	= 1 << 1,
	TX_POLL		= 1 << 0,
};

//#define CMD_CONFIG 0x4049E130// pkt timer = 15 => 4pkt trigger int
#define CMD_CONFIG 0xc049E130// pkt timer = 15 => 4pkt trigger int
#define CMD1_CONFIG 0x30010000   //desc format ==> apollo type, not support multiple ring, enable RxRing 1

enum RE8670_IOCMD1_REG
{
	TX_POLL5	= 1 << 8,
	txq5_h		= 1 << 4,
	txq4_h		= 1 << 3,
	txq3_h		= 1 << 2,
	txq2_h		= 1 << 1,
	txq1_h		= 1 << 0,
};

struct rx_info{
	union{
		struct{
			u32 own:1;//31
			u32 eor:1;//30
			u32 fs:1;//29
			u32 ls:1;//28
			u32 crcerr:1;//27
			u32 ipv4csf:1;//26
			u32 l4csf:1;//25
			u32 rcdf:1;//24
			u32 ipfrag:1;//23
			u32 pppoetag:1;//22
			u32 rwt:1;//21
			u32 pkttype:4;//17~20
			u32 l3routing:1;//16
			u32 origformat:1;//15
			u32 pctrl:1;//14
			u32 rsvd:2;//12~13
			u32 data_length:12;//0~11
		}bit;
		u32 dw;//double word
	}opts1;
	u32 addr;
	union{
		struct{
			u32 cputag:1;//31
			u32 ptp_in_cpu_tag_exist:1;//30
			u32 svlan_tag_exist:1;//29
			u32 rsvd_2:2;//27~28
			u32 pon_stream_id:7;//20~26
			u32 rsvd_1:3;//17~19
			u32 ctagva:1;//16
			u32 cvaln_tag:16;//0~15
		}bit;
		u32 dw;//double word
	}opts2;
	union{
		struct{
			u32 src_port_num:5;//27~31
			u32 dst_port_mask:6;//21~26
			u32 reason:8;//13~20
			u32 internal_priority:3;//10~12
			u32 ext_port_ttl_1:5;//5~9
			u32 rsvd:5;//0~4
		}bit;
		u32 dw;//double word
	}opts3;
};

struct tx_info{
	union{
		struct{
			u32 own:1;//31
			u32 eor:1;//30
			u32 fs:1;//29
			u32 ls:1;//28
			u32 ipcs:1;//27
			u32 l4cs:1;//26
			u32 keep:1;//25
			u32 blu:1;//24
			u32 crc:1;//23
			u32 vsel:1;//22
			u32 dislrn:1;//21
			u32 cputag_ipcs:1;//20
			u32 cputag_l4cs:1;//19
			u32 cputag_psel:1;//18
			u32 rsvd:1;//17
			u32 data_length:17;//0~16
		}bit;
		u32 dw;//double word
	}opts1;
	u32 addr;
	union{
		struct{
			u32 cputag:1;//31
			u32 aspri:1;//30
			u32 cputag_pri:3;//27~29
			u32 tx_vlan_action:2;//25~26
			u32 tx_pppoe_action:2;//23~24
			u32 tx_pppoe_idx:3;//20~22
			u32 efid:1;//19
			u32 enhance_fid:3;//16~18
			u32 vidl:8;//8~15
			u32 prio:3;//5~7
			u32 cfi:1;// 4
			u32 vidh:4;//0~3
		}bit;
		u32 dw;//double word
	}opts2;
	union{
		struct{
			u32 extspa:3;//29~31
			u32 tx_portmask:6;//23~28
			u32 tx_dst_stream_id:7;//16~22
			u32 tx_dst_vc_mask:16;//0~15
		}bit;
		u32 dw;//double word
	}opts3;
	union{
		u32 dw;
	}opts4;
};

int re8686_send_with_txInfo(struct sk_buff *skb, struct tx_info* ptxInfo);

#define VDSL_PORT 7
#define PON_PORT 3

#endif /*_RE8686_H_*/
