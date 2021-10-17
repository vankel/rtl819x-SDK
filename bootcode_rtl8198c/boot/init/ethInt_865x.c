#include <linux/config.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/kernel_stat.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/random.h>
#include <asm/bitops.h>
#include <asm/bootinfo.h>
#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/system.h>
#include <linux/circ_buf.h>
#include <asm/io.h>
#include <rtl8196x/asicregs.h>        
#include <asm/rtl8196x.h>

#include "etherboot.h"

#include "nic.h"
#include "ethInt_865x.h"  
#include <rtl_types.h>
#include <rtl8196x/swCore.h>
#include <rtl8196x/swNic_poll.h>
#include <rtl8196x/vlanTable.h>

#define BUF_OFFSET 	4	// descriptor offset of data BUFFER
#define DATA_OFFSET	2	// real data offset of Rx packet in buffer

#define NUM_DESC	2//16//64 //wei del
#define BUF_SIZE	1600	// Byte Counts

typedef struct {
	unsigned long    StsLen;
	unsigned long    DataPtr;
 	unsigned long    VLan;
 	unsigned long    Reserved;	
}desc_t;

struct statistics
{
	unsigned int txpkt;
	unsigned int rxpkt;
	unsigned int txerr;
	unsigned int rxerr;
	unsigned int rxffov;
		
};

struct eth_private
{
	unsigned int nr;
	unsigned int io_addr;
	unsigned int irq;
	unsigned int num_desc;
	Int32 rx_descaddr;
	Int32 tx_descaddr;
	Int32 tx_skbaddr[NUM_DESC];	
	Int32 rx_skbaddr[NUM_DESC];
	struct statistics  res;
	unsigned int cur_rx;
	unsigned int cur_tx;
};

//--------------------------------------------------------------------------------------------
void eth_interrupt(int irq, void *dev_id, struct pt_regs *regs);
void eth_polltx(int etherport);
void SetOwnByNic(Int32* header, int len, int own,int index);
void prepare_txpkt(int etherport,Int16 type,Int8* destaddr,Int8* data ,Int16 len); 

//--------------------------------------------------------------------------------------------

#ifdef CONFIG_NIC_LOOPBACK
extern int nic_loopback;
#endif
extern void kick_tftpd(void);
extern struct nic nic;
extern struct arptable_t  arptable_tftp[3];
extern int32 swCore_init();
extern void flush_cache(void);
extern void ddump(unsigned char * pData, int len);
extern int flashread (unsigned long dst, unsigned int src, unsigned long length);
//--------------------------------------------------------------------------------------------
#if defined( CONFIG_NFBI) 
char eth0_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0x97};
#else
char eth0_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xe8};
char eth0_mac_httpd[6]={0};
char eth0_ip_httpd[4]={0};
#endif

#if defined(CONFIG_RTL_LAN_WAN_ISOLATION)
#define RTL_WAN_PORT_MASK 0x10	//port4
#ifdef CONFIG_NFBI
char eth1_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0x98};
#else
char eth1_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xe9};
#endif
#endif

#if defined(CONFIG_RTL_ALL_PORT_ISOLATION)
#define LAN_WAN_PORT_NUM	5

#define RTL_LAN0_PORT_MASK	0x1
#define RTL_LAN1_PORT_MASK	0x2
#define RTL_LAN2_PORT_MASK	0x4
#define RTL_LAN3_PORT_MASK	0x8
#define RTL_WAN_PORT_MASK 	0x10	//port4

#ifdef CONFIG_NFBI
char eth1_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0x98};
char eth2_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0x99};
char eth3_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0x9a};
char eth4_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0x9b};
#else
char eth1_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xe9};
char eth2_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xea};
char eth3_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xeb};
char eth4_mac[6]={0x56, 0xaa, 0xa5, 0x5a, 0x7d, 0xec};
#endif
#endif

static Int8 ETH0_tx_buf[NUM_DESC][BUF_SIZE];

static int ETH0_IRQ=25; //wei add for 8196 sw


static unsigned long ETH0_ADD=0x18000;
static struct eth_private ETH[2];
static struct irqaction irq_eth15 = {eth_interrupt, 0, 15,"eth0", NULL, NULL};  
//--------------------------------------------------------------------------------------------

void eth_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
#ifdef CONFIG_NIC_LOOPBACK
	int i=0;
#endif
	int status=*(volatile Int32*)(0xb801002c);
	*(volatile Int32*)(0xb801002c)=status;
	
/* JasonWang. 081128
  	if (swNic_receive((void **)&nic.packet, &nic.packetlen)== 0) 
 		kick_tftpd();  	
*/
//		printf("rx intterupt...\n");

	nic.packetlen=0;
	while(swNic_receive((void **)&nic.packet, &nic.packetlen)== 0) 
	{
#ifdef CONFIG_NIC_LOOPBACK
		if (nic_loopback) {
			swNic_send(nic.packet, nic.packetlen);

			if (((++i) % 32) == 0)
				swNic_txDone();
		}
		else 
#endif		
		{
			swNic_txDone();
			kick_tftpd();  	
			nic.packetlen=0;
		}
	}

	swNic_txDone();
}
//---------------------------------------------------------------------------------------

#if !(defined( CONFIG_NFBI) || defined(CONFIG_NONE_FLASH)|| defined(CONFIG_NAND_FLASH)||defined(CONFIG_NAND_FLASH_BOOTING))
void gethwmac(unsigned char *mac)
{
	unsigned char tmpbuf[6];
	unsigned short len;
	unsigned char *buf;
	unsigned char sum=0;
	int i;
	
	if (flashread(tmpbuf, HW_SETTING_OFFSET,6)==0 ) {
		return;
	}
	if(tmpbuf[0] == 'h')
	{
		memcpy(&len, &tmpbuf[4], 2);
		if(len > 0x2000)
			return;
		if(NULL==(buf=(unsigned char *)malloc(len)))
			return;
		flashread(buf,HW_SETTING_OFFSET+6,len);
		if(len != 0 && len <= 0x2000) {					
			for (i=0;i<len;i++) 
				sum += buf[i];
		}
		else
			sum=1;
		if(0 == sum)
		{			
			memcpy(mac,buf+HW_NIC0_MAC_OFFSET,6);
			if(memcmp(mac,"\x0\x0\x0\x0\x0\x0", 6) && !(mac[0] & 0x1))
			{
				/*normal mac*/
			}
			else
			{
				memset(mac,0x0,6);
			}
		}
		if(buf)
			free(buf);
	}
	return;
}

void getmacandip(unsigned char *mac,unsigned char *ip)
{
	unsigned char tmpbuf[6];
	unsigned short len;
	unsigned char *buf;
	unsigned char sum=0;
	int i;

	int currSettingMaxLen = 0x4000;

	
	if (flashread(tmpbuf, CURRENT_SETTING_OFFSET,6)==0 ) {
		return;
	}


#if defined(RTL8198)

	if(tmpbuf[0] == '6')

#else
	if(tmpbuf[0] == 'c')
#endif		
	{

		/*current setting*/
		memcpy(&len, &tmpbuf[4], 2);


		if(len > currSettingMaxLen)
			return;

		/*alloc mem for reading current setting*/
		if(NULL==(buf=(unsigned char *)malloc(len)))
			return;

		flashread(buf,CURRENT_SETTING_OFFSET+6,len);
		if(len != 0 && len <= currSettingMaxLen) {					
			for (i=0;i<len;i++) 
				sum += buf[i];
		}
		else
			sum=1;

		if(0 == sum)
		{
			/*check sum ok*/
			memcpy(ip,buf+CURRENT_IP_ADDR_OFFSET,4);
			memcpy(mac,buf+CURRENT_ELAN_MAC_OFFSET,6);

//printf("\n ip: %d.%d.%d.%d__%u\n",*ip,*(ip+1),*(ip+2),*(ip+3),__LINE__);
//printf("\n mac: %x-%x-%x-%x-%x-%x __%u\n",*mac,*(mac+1),*(mac+2),*(mac+3),*(mac+4),*(mac+5),__LINE__);

			
			if(memcmp(ip,"\x0\x0\x0\x0",4) && !(0xFF==ip[3] ||0x0==ip[3]))
			{
				/*normal ip*/
				if(memcmp(mac,"\x0\x0\x0\x0\x0\x0", 6) && !(mac[0] & 0x1))
				{
					/*normal mac*/

				}
				else
				{
					/*bad mac. user hw setting mac*/
					gethwmac(mac);
				}
				
				if(memcmp(ip,"\xC0\xA8\x0\x1",4) !=0)
				{
					/* different ip with 192.168.0.1, MUST use different MAC */
					eth0_mac[0]=0x56;
					eth0_mac[1]=0xaa;
					eth0_mac[2]=0xa5;
					eth0_mac[3]=0x5a;
					eth0_mac[4]=0x7d;
					eth0_mac[5]=0xe8;
				}
				else
				{
					/* same ip with 192.168.0.1, so use the same mac */
					gethwmac(eth0_mac);
				}
			}
			else
			{
				/*use hard code 192.168.1.6*/
				memset(ip,0x0,4);
			}
		}
		if(buf)
			free(buf);
	}
}
#endif

#if defined(CONFIG_RTL_LAN_WAN_ISOLATION) || defined(CONFIG_RTL_ALL_PORT_ISOLATION)
int32 rtl8651_setAsicPvid(uint32 port, uint32 pvid)
{
	uint32 regValue,offset;

#if 0
	if(port>=RTL8651_AGGREGATOR_NUMBER || pvid>=RTL865XC_VLAN_NUMBER)
		return FAILED;;
#endif
	offset=(port*2)&(~0x3);
	regValue=READ_MEM32(PVCR0+offset);
	if((port&0x1))
	{
		regValue=  ((pvid &0xfff) <<16) | (regValue&~0xFFF0000);
	}
	else
	{	
		regValue =  (pvid &0xfff) | (regValue &~0xFFF);
	}
	WRITE_MEM32(PVCR0+offset,regValue);
	return SUCCESS;
}

#endif
//----------------------------------------------------------------------------------------
void eth_startup(int etherport)
{
	Int32 val;
	int i;

#if !(defined( CONFIG_NFBI) || defined(CONFIG_NONE_FLASH)|| defined(CONFIG_NAND_FLASH)||defined(CONFIG_NAND_FLASH_BOOTING))
	memset(eth0_ip_httpd, 0, 4);
	/*try to figure out http mac and ip*/
#ifdef CONFIG_HTTP_SERVER
	getmacandip(eth0_mac_httpd,eth0_ip_httpd);
#endif
#endif

	if (swCore_init()) {  	
		dprintf("\nSwitch core initialization failed!\n");        
		return;
	}

    	//avoid download bin checksum error
	uint32 rx[6] = {4, 0, 0, 0, 0, 0};
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
        uint32 tx[4] = {4, 2, 2, 2};
#else
        uint32 tx[2] = {4, 2};
#endif

	/* Initialize NIC module */
	if (swNic_init(rx, 4, tx, MBUF_LEN)) {
		dprintf("\nSwitch nic initialization failed!\n");            
		return;
	}

    rtl_vlan_param_t vp;
    int32 ret;
	rtl_netif_param_t np;
	rtl_acl_param_t ap;

	/* Create Netif */
#if !(defined(CONFIG_RTL_LAN_WAN_ISOLATION) || defined(CONFIG_RTL_ALL_PORT_ISOLATION))
	bzero((void *) &np, sizeof(rtl_netif_param_t));
	np.vid = 8;
	np.valid = 1;
	np.enableRoute = 0;
	np.inAclEnd = 0;
	np.inAclStart = 0;
	np.outAclEnd = 0;
	np.outAclStart = 0;
	memcpy(&np.gMac, &eth0_mac[0], 6);

	np.macAddrNumber = 1;
	np.mtu = 1500;
	ret = swCore_netifCreate(0, &np);
	if (ret != 0) {
		printf( "Creating intif fails:%d\n", ret );
		return;
	}

	/* Create vlan */
	bzero((void *) &vp, sizeof(rtl_vlan_param_t));
	vp.egressUntag = ALL_PORT_MASK;
	vp.memberPort = ALL_PORT_MASK;
	ret = swCore_vlanCreate(8, &vp);
	if (ret != 0) {       
		printf( "Creating vlan fails:%d\n", ret );
       return;
	}
#endif

#if defined(CONFIG_RTL_LAN_WAN_ISOLATION)
	bzero((void *) &np, sizeof(rtl_netif_param_t));
	np.vid = 8;
	np.valid = 1;
	np.enableRoute = 0;
	np.inAclEnd = 0;
	np.inAclStart = 0;
	np.outAclEnd = 0;
	np.outAclStart = 0;
	memcpy(&np.gMac, &eth0_mac[0], 6);

	np.macAddrNumber = 1;
	np.mtu = 1500;
	ret = swCore_netifCreate(0, &np);
	if (ret != 0) {
		printf( "Creating intif fails:%d\n", ret );
		return;
	}
	/* Create vlan */
	bzero((void *) &vp, sizeof(rtl_vlan_param_t));
	vp.egressUntag = ALL_PORT_MASK&(~(RTL_WAN_PORT_MASK));
	vp.memberPort = ALL_PORT_MASK&(~(RTL_WAN_PORT_MASK));
	ret = swCore_vlanCreate(8, &vp);
	if (ret != 0) {       
		printf( "Creating vlan fails:%d\n", ret );
       return;
	}

	bzero((void *) &np, sizeof(rtl_netif_param_t));
	np.vid = 9;
	np.valid = 1;
	np.enableRoute = 0;
	np.inAclEnd = 0;
	np.inAclStart = 0;
	np.outAclEnd = 0;
	np.outAclStart = 0;
	memcpy(&np.gMac, &eth1_mac[0], 6);

	np.macAddrNumber = 1;
	np.mtu = 1500;
	ret = swCore_netifCreate(1, &np);
	if (ret != 0) {
		printf( "Creating wan intif fails:%d\n", ret );
		return;
	}

	/* Create vlan */
	bzero((void *) &vp, sizeof(rtl_vlan_param_t));
	vp.egressUntag = RTL_WAN_PORT_MASK;
	vp.memberPort = RTL_WAN_PORT_MASK;
	ret = swCore_vlanCreate(9, &vp);
	if (ret != 0) {       
		printf( "Creating wan vlan fails:%d\n", ret );
       		return;
	}

	/* set pvid */
	rtl8651_setAsicPvid(4,9);
#endif

#if defined(CONFIG_RTL_ALL_PORT_ISOLATION)
	for(i = 0;i < LAN_WAN_PORT_NUM; i++){
		bzero((void *) &np, sizeof(rtl_netif_param_t));
		np.vid = 8 + i;
		np.valid = 1;
		np.enableRoute = 0;
		np.inAclEnd = 0;
		np.inAclStart = 0;
		np.outAclEnd = 0;
		np.outAclStart = 0;

		switch(i){
		case 0:
			memcpy(&np.gMac, &eth0_mac[0], 6);
			break;
		case 1:
			memcpy(&np.gMac, &eth1_mac[0], 6);
			break;
		case 2:
			memcpy(&np.gMac, &eth2_mac[0], 6);
			break;
		case 3:
			memcpy(&np.gMac, &eth3_mac[0], 6);
			break;
		case 4:
			memcpy(&np.gMac, &eth4_mac[0], 6);
			break;
		default:
			printf("%s:%d:should not happen\n",__func__,__LINE__);

		}

		np.macAddrNumber = 1;
		np.mtu = 1500;
		ret = swCore_netifCreate(i, &np);
		if (ret != 0) {
			printf( "Creating wan intif fails:%d\n", ret );
			return;
		}

		/* Create vlan */
		bzero((void *) &vp, sizeof(rtl_vlan_param_t));
		switch(i){
		case 0:
			vp.egressUntag = RTL_LAN0_PORT_MASK;
			vp.memberPort = RTL_LAN0_PORT_MASK;
			break;
		case 1:
			vp.egressUntag = RTL_LAN1_PORT_MASK;
			vp.memberPort = RTL_LAN1_PORT_MASK;
			break;
		case 2:
			vp.egressUntag = RTL_LAN2_PORT_MASK;
			vp.memberPort = RTL_LAN2_PORT_MASK;
			break;
		case 3:
			vp.egressUntag = RTL_LAN3_PORT_MASK;
			vp.memberPort = RTL_LAN3_PORT_MASK;
			break;
		case 4:
			vp.egressUntag = RTL_WAN_PORT_MASK;
			vp.memberPort = RTL_WAN_PORT_MASK;
			break;
		default:
			printf("%s:%d:should not happen\n",__func__,__LINE__);

		}
		ret = swCore_vlanCreate(8+i, &vp);
		if (ret != 0) {       
			printf( "Creating wan vlan fails:%d\n", ret );
	       		return;
		}

		rtl8651_setAsicPvid(i,8+i);
	}
#endif

    /* Set interrupt routing register */

#if !defined(CONFIG_RTL8198C)
	REG32(IRR1_REG) |= (3<<28); 
#endif	
 	request_IRQ(ETH0_IRQ, &irq_eth15,&(ETH[0]));
}

//----------------------------------------------------------------------------------------
/*Just a start address, and the data length*/
void prepare_txpkt(int etherport, Int16 type, Int8* destaddr, Int8* data, Int16 len) 
{
	char *tx_buffer=&ETH0_tx_buf[0][0];
 	Int16 nstype;	
	int Length=len;
	
	memcpy(tx_buffer,destaddr,6);

	/*Source Address*/
	memcpy(tx_buffer+6,eth0_mac,6);

	/*Payload type*/
	nstype = htons(type);
	memcpy(tx_buffer + 12,(Int8*)&nstype,2);

	/*Payload */
	memcpy(tx_buffer + 14,(Int8*)data,Length);
	Length += 14;

//	printf("tx pkt\n");
	swNic_send(tx_buffer,Length);
}

//----------------------------------------------------------------------------------------
/*
extern Int8 one_tftp_lock;
void eth_listening()
{
	int tftp_status=0;

   while(1)
   {
		if (swNic_receive((void **)&nic.packet, &nic.packetlen)== 0) {
	 		kick_tftpd();

			if (one_tftp_lock==1 && tftp_status==0)
				tftp_status=1;  //start
			if (one_tftp_lock==0 && tftp_status==1)
					tftp_status=2;	//end	
			swNic_txDone();					
	  	}
		if (tftp_status==2)
			break;
   }
}
*/
