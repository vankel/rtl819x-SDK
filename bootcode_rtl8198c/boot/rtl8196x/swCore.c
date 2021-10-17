/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
*
* Abstract: Switch core driver source code.
*
* $Author: jasonwang $
*
* ---------------------------------------------------------------
*/

#include <rtl_types.h>
#include <rtl_errno.h>
#include <rtl8196x/loader.h>  //wei edit
#include <linux/config.h>
#include <rtl8196x/asicregs.h>
#include <rtl8196x/swCore.h>
#include <rtl8196x/phy.h>
#include <asm/rtl8198.h>
#include <asm/system.h>



#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

#define RTL8651_ETHER_AUTO_100FULL	0x00
#define RTL8651_ETHER_AUTO_100HALF	0x01
#define RTL8651_ETHER_AUTO_10FULL		0x02
#define RTL8651_ETHER_AUTO_10HALF	0x03
#define RTL8651_ETHER_AUTO_1000FULL	0x08
#define RTL8651_ETHER_AUTO_1000HALF	0x09
#define GIGA_PHY_ID	0x16
#define tick_Delay10ms(x) { int i=x; while(i--) __delay(5000); }






#ifndef CONFIG_NFBI
#if CONFIG_RTL865XC
static uint8 fidHashTable[]={0x00,0x0f,0xf0,0xff};

/*#define rtl8651_asicTableAccessAddrBase(type) (RTL8651_ASICTABLE_BASE_OF_ALL_TABLES + 0x10000 * (type)) */
#define		RTL8651_ASICTABLE_BASE_OF_ALL_TABLES		0xBB000000
#define		rtl8651_asicTableAccessAddrBase(type) (RTL8651_ASICTABLE_BASE_OF_ALL_TABLES + ((type)<<16) )
#define 		RTL865X_FAST_ASIC_ACCESS
#define		RTL865XC_ASIC_WRITE_PROTECTION				/* Enable/Disable ASIC write protection */
#define		RTL8651_ASICTABLE_ENTRY_LENGTH (8 * sizeof(uint32))
#define		RTL865X_TLU_BUG_FIXED		1


#ifdef RTL865X_FAST_ASIC_ACCESS
static uint32 _rtl8651_asicTableSize[] =
{
	2 /*TYPE_L2_SWITCH_TABLE*/,
	1 /*TYPE_ARP_TABLE*/,
    2 /*TYPE_L3_ROUTING_TABLE*/,
	3 /*TYPE_MULTICAST_TABLE*/,
	1 /*TYPE_PROTOCOL_TRAP_TABLE*/,
	5 /*TYPE_VLAN_TABLE*/,
	3 /*TYPE_EXT_INT_IP_TABLE*/,
    1 /*TYPE_ALG_TABLE*/,
    4 /*TYPE_SERVER_PORT_TABLE*/,
    3 /*TYPE_L4_TCP_UDP_TABLE*/,
    3 /*TYPE_L4_ICMP_TABLE*/,
    1 /*TYPE_PPPOE_TABLE*/,
    8 /*TYPE_ACL_RULE_TABLE*/,
    1 /*TYPE_NEXT_HOP_TABLE*/,
    3 /*TYPE_RATE_LIMIT_TABLE*/,
};
#endif

static void _rtl8651_asicTableAccessForward(uint32 tableType, uint32 eidx, void *entryContent_P) {
	ASSERT_CSP(entryContent_P);


	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done

#ifdef RTL865X_FAST_ASIC_ACCESS

	{
		register uint32 index;

		for( index = 0; index < _rtl8651_asicTableSize[tableType]; index++ )
		{
			WRITE_MEM32(TCR0+(index<<2), *((uint32 *)entryContent_P + index));
		}

	}
#else
	WRITE_MEM32(TCR0, *((uint32 *)entryContent_P + 0));
	WRITE_MEM32(TCR1, *((uint32 *)entryContent_P + 1));
	WRITE_MEM32(TCR2, *((uint32 *)entryContent_P + 2));
	WRITE_MEM32(TCR3, *((uint32 *)entryContent_P + 3));
	WRITE_MEM32(TCR4, *((uint32 *)entryContent_P + 4));
	WRITE_MEM32(TCR5, *((uint32 *)entryContent_P + 5));
	WRITE_MEM32(TCR6, *((uint32 *)entryContent_P + 6));
	WRITE_MEM32(TCR7, *((uint32 *)entryContent_P + 7));
#endif	
	WRITE_MEM32(SWTAA, ((uint32) rtl8651_asicTableAccessAddrBase(tableType) + eidx * RTL8651_ASICTABLE_ENTRY_LENGTH));//Fill address
}

static int32 _rtl8651_forceAddAsicEntry(uint32 tableType, uint32 eidx, void *entryContent_P) {

	#ifdef RTL865XC_ASIC_WRITE_PROTECTION
	if (RTL865X_TLU_BUG_FIXED)	/* No need to stop HW table lookup process */
	{	/* No need to stop HW table lookup process */
		WRITE_MEM32(SWTCR0,EN_STOP_TLU|READ_MEM32(SWTCR0));
		while ( (READ_MEM32(SWTCR0) & STOP_TLU_READY)==0);
	}
	#endif

	_rtl8651_asicTableAccessForward(tableType, eidx, entryContent_P);

 	WRITE_MEM32(SWTACR, ACTION_START | CMD_FORCE);//Activate add command
	while ( (READ_MEM32(SWTACR) & ACTION_MASK) != ACTION_DONE );//Wait for command done

	#ifdef RTL865XC_ASIC_WRITE_PROTECTION
	if (RTL865X_TLU_BUG_FIXED)	/* No need to stop HW table lookup process */
	{
		WRITE_MEM32(SWTCR0,~EN_STOP_TLU&READ_MEM32(SWTCR0));
	}
	#endif

	return SUCCESS;
}

uint32 rtl8651_filterDbIndex(ether_addr_t * macAddr,uint16 fid) {
    return ( macAddr->octet[0] ^ macAddr->octet[1] ^
                    macAddr->octet[2] ^ macAddr->octet[3] ^
                    macAddr->octet[4] ^ macAddr->octet[5] ^fidHashTable[fid]) & 0xFF;
}

static int32 rtl8651_setAsicL2Table(ether_addr_t	*mac, uint32 column)
{
	rtl865xc_tblAsic_l2Table_t entry;
	uint32	row;

	row = rtl8651_filterDbIndex(mac, 0);
	if((row >= RTL8651_L2TBL_ROW) || (column >= RTL8651_L2TBL_COLUMN))
		return FAILED;
	if(mac->octet[5] != ((row^(fidHashTable[0])^ mac->octet[0] ^ mac->octet[1] ^ mac->octet[2] ^ mac->octet[3] ^ mac->octet[4] ) & 0xff))
		return FAILED;

	memset(&entry, 0,sizeof(entry));
	entry.mac47_40 = mac->octet[0];
	entry.mac39_24 = (mac->octet[1] << 8) | mac->octet[2];
	entry.mac23_8 = (mac->octet[3] << 8) | mac->octet[4];

//	entry.extMemberPort = 0;   
	entry.memberPort = 7;
	entry.toCPU = 1;
	entry.isStatic = 1;
//	entry.nxtHostFlag = 1;

	/* RTL865xC: modification of age from ( 2 -> 3 -> 1 -> 0 ) to ( 3 -> 2 -> 1 -> 0 ). modification of granularity 100 sec to 150 sec. */
	entry.agingTime = 0x03;
	
//	entry.srcBlock = 0;
	entry.fid=0;
	entry.auth=1;

	return _rtl8651_forceAddAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
}
#endif
#endif

//------------------------------------------------------------------------
static void _rtl8651_clearSpecifiedAsicTable(uint32 type, uint32 count) 
{
	struct { uint32 _content[8]; } entry;
	uint32 idx;
	
	bzero(&entry, sizeof(entry));
	for (idx=0; idx<count; idx++)// Write into hardware
		swTable_addEntry(type, idx, &entry);
}

#include <asm/delay.h>

extern unsigned long loops_per_jiffy;
#define __mdelay(x) { int i=x; while(i--) __udelay(1000, loops_per_jiffy * 100); }

void FullAndSemiReset( void )
{

	REG32(SIRR) |= FULL_RST;
	__mdelay(300);

	REG32(SYS_CLK_MAG) &= ~CM_ACTIVE_SWCORE;
	__mdelay(300);
	
	REG32(SYS_CLK_MAG) |= CM_ACTIVE_SWCORE;
	__mdelay(50);
}

#if 0
int32 rtl865xC_setAsicEthernetMIIMode(uint32 port, uint32 mode)
{
	if ( port != 0 && port != RTL8651_MII_PORTNUMBER )
		return FAILED;
	if ( mode != LINK_RGMII && mode != LINK_MII_MAC && mode != LINK_MII_PHY )
		return FAILED;

	if ( port == 0 )
	{
		/* MII port MAC interface mode configuration */
		WRITE_MEM32( P0GMIICR, ( READ_MEM32( P0GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << LINKMODE_OFFSET ) );
	}
	else
	{
		/* MII port MAC interface mode configuration */
		WRITE_MEM32( P5GMIICR, ( READ_MEM32( P5GMIICR ) & ~CFG_GMAC_MASK ) | ( mode << LINKMODE_OFFSET ) );
	}
	return SUCCESS;

}
#endif



int32 rtl8651_getAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 *rData)
{
	uint32 status;
	
	WRITE_MEM32( MDCIOCR, COMMAND_READ | ( phyId << PHYADD_OFFSET ) | ( regId << REGADD_OFFSET ) );

#ifdef RTL865X_TEST
	status = READ_MEM32( MDCIOSR );
#else
#if defined(CONFIG_RTL8198) && !defined(CONFIG_RTL8198C)
	REG32(GIMR_REG) = REG32(GIMR_REG) | (0x1<<8);    //add by jiawenjian
	delay_ms(10);   //wei add, for 8196C_test chip patch. mdio data read will delay 1 mdc clock.
#endif
	do { status = READ_MEM32( MDCIOSR ); } while ( ( status & STATUS ) != 0 );
#endif

	status &= 0xffff;
	*rData = status;

	return SUCCESS;
}


/* rtl8651_setAsicEthernetPHYReg( phyid, regnum, data );
    //dprintf("\nSet enable_10M_power_saving01!\n");
    rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );*/

int32 rtl8651_setAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 wData)
{
	WRITE_MEM32( MDCIOCR, COMMAND_WRITE | ( phyId << PHYADD_OFFSET ) | ( regId << REGADD_OFFSET ) | wData );

#ifdef RTL865X_TEST
#else
	while( ( READ_MEM32( MDCIOSR ) & STATUS ) != 0 );		/* wait until command complete */
#endif

	return SUCCESS;
}

int32 rtl8651_restartAsicEthernetPHYNway(uint32 port, uint32 phyid)
{
	uint32 statCtrlReg0;

	/* read current PHY reg 0 */
	rtl8651_getAsicEthernetPHYReg( phyid, 0, &statCtrlReg0 );

	/* enable 'restart Nway' bit */
	statCtrlReg0 |= RESTART_AUTONEGO;

	/* write PHY reg 0 */
	rtl8651_setAsicEthernetPHYReg( phyid, 0, statCtrlReg0 );

	return SUCCESS;
}
#if 0
int32 rtl8651_setAsicEthernetPHY(uint32 port, int8 autoNegotiation, uint32 advCapability, uint32 speed, int8 fullDuplex, 
	uint32 phyId, uint32 isGPHY) 
{
	uint32 statCtrlReg0, statCtrlReg4, statCtrlReg9;

	/* ====================
		Arrange PHY reg 0
	   ==================== */

	/* Read PHY reg 0 (control register) first */
	rtl8651_getAsicEthernetPHYReg(phyId, 0, &statCtrlReg0);

	if ( autoNegotiation == TRUE )	
	{
		statCtrlReg0 |= ENABLE_AUTONEGO;
	}
	else
	{
		statCtrlReg0 &= ~ENABLE_AUTONEGO;

		/* Clear speed & duplex setting */
		if ( isGPHY )
			statCtrlReg0 &= ~SPEED_SELECT_1000M;
		statCtrlReg0 &= ~SPEED_SELECT_100M;
		statCtrlReg0 &= ~SELECT_FULL_DUPLEX;

		if ( speed == 1 )	/* 100Mbps, assume 10Mbps by default */
			statCtrlReg0 |= SPEED_SELECT_100M;

		if ( fullDuplex == TRUE )
			statCtrlReg0 |= SELECT_FULL_DUPLEX;
	}

	/* =============================================================
		Arrange PHY reg 4, if GPHY, also need to arrange PHY reg 9.
	   ============================================================= */
	rtl8651_getAsicEthernetPHYReg( phyId, 4, &statCtrlReg4 );

	/* Clear all capability */
	statCtrlReg4 &= ~CAP_100BASE_MASK;

	if ( isGPHY )
	{
		rtl8651_getAsicEthernetPHYReg( phyId, 9, &statCtrlReg9 );

		/* Clear all 1000BASE capability */
		statCtrlReg9 &= ~ADVCAP_1000BASE_MASK;
	}
	else
	{
		statCtrlReg9 = 0;
	}
	
	if ( advCapability == RTL8651_ETHER_AUTO_1000FULL )
	{
		statCtrlReg9 = statCtrlReg9 | CAPABLE_1000BASE_TX_FD | CAPABLE_1000BASE_TX_HD;
		statCtrlReg4 = statCtrlReg4 | CAPABLE_100BASE_TX_FD | CAPABLE_100BASE_TX_HD | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_1000HALF )
	{
		statCtrlReg9 = statCtrlReg9 | CAPABLE_1000BASE_TX_HD;
		statCtrlReg4 = statCtrlReg4 | CAPABLE_100BASE_TX_FD | CAPABLE_100BASE_TX_HD | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_100FULL )
	{
		statCtrlReg4 = statCtrlReg4 | CAPABLE_100BASE_TX_FD | CAPABLE_100BASE_TX_HD | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_100HALF )
	{
		statCtrlReg4 = statCtrlReg4 | CAPABLE_100BASE_TX_HD | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_10FULL )
	{
		statCtrlReg4 = statCtrlReg4 | CAPABLE_10BASE_TX_FD | CAPABLE_10BASE_TX_HD;
	}
	else if ( advCapability == RTL8651_ETHER_AUTO_10HALF )
	{
		statCtrlReg4 = statCtrlReg4 | CAPABLE_10BASE_TX_HD;
	}
	else
	{
//		RTL_WARN(RTL_MSG_GENERIC, "Invalid advertisement capability!");
		return FAILED;
	}

	/* ===============================
		Set PHY reg 4.
		Set PHY reg 9 if necessary.
	   =============================== */
	rtl8651_setAsicEthernetPHYReg( phyId, 4, statCtrlReg4 );

	if ( isGPHY )
	{
		rtl8651_setAsicEthernetPHYReg( phyId, 9, statCtrlReg9 );
	}

	/* =================
		Set PHY reg 0.
	   ================= */
	rtl8651_setAsicEthernetPHYReg( phyId, 0, statCtrlReg0 );

	/* =======================================================
		Restart Nway.
		If 'Nway enable' is FALSE, ASIC won't execute Nway.
	   ======================================================= */
	rtl8651_restartAsicEthernetPHYNway(port, phyId);

	return SUCCESS;
}
#endif

int32 rtl8651_setAsicFlowControlRegister(uint32 port, uint32 enable, uint32 phyid)
{
	uint32 statCtrlReg4;

	/* Read */
	rtl8651_getAsicEthernetPHYReg( phyid, 4, &statCtrlReg4 );

	if ( enable && ( statCtrlReg4 & CAPABLE_PAUSE ) == 0 )
	{
		statCtrlReg4 |= CAPABLE_PAUSE;		
	}
	else if ( enable == 0 && ( statCtrlReg4 & CAPABLE_PAUSE ) )
	{
		statCtrlReg4 &= ~CAPABLE_PAUSE;
	}
	else
		return SUCCESS;	/* The configuration does not change. Do nothing. */

	rtl8651_setAsicEthernetPHYReg( phyid, 4, statCtrlReg4 );
	
	/* restart N-way. */
	rtl8651_restartAsicEthernetPHYNway(port, phyid);

	return SUCCESS;
}

//====================================================================

#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E) || defined(CONFIG_RTL8198C)
void Set_GPHYWB(unsigned int phyid, unsigned int page, unsigned int reg, unsigned int mask, unsigned int val)
{

	unsigned int data=0;
	unsigned int wphyid=0;	//start
	unsigned int wphyid_end=1;   //end
	if(phyid==999)
	{	wphyid=0;
		wphyid_end=5;    //total phyid=0~4
	}
	else
	{	wphyid=phyid;
		wphyid_end=phyid+1;
	}

	for(; wphyid<wphyid_end; wphyid++)
	{
		//change page 
			rtl8651_setAsicEthernetPHYReg( wphyid, 31, page  );
		
		if(mask!=0)
		{
			rtl8651_getAsicEthernetPHYReg( wphyid, reg, &data);
			data=data&mask;
		}
		rtl8651_setAsicEthernetPHYReg( wphyid, reg, data|val  );

#if 0
		//switch to page 0
		//if(page>=40)
		{	
			rtl8651_setAsicEthernetPHYReg( wphyid, 31, 0  );
			//rtl8651_setAsicEthernetPHYReg( wphyid, 30, 0  );
		}
		/*
		else
		{
			rtl8651_setAsicEthernetPHYReg( wphyid, 31, 0  );
		}
		*/
#endif		
	}
}
#endif

//====================================================================
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
#define PHYTYPE_EMB 0
#define PHYTYPE_EXT 1
unsigned int Get_P0_PhyMode()
{
/*
	//8881A is different from 8196D
	00: olt
	01: deb_sel 
	10: External  phy 
	11: embedded phy
*/
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf	
	
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	
	unsigned int v=REG32(SYS_HW_STRAP);
	unsigned int mode=GET_BITVAL(v, 6, RANG1) *2 + GET_BITVAL(v, 7, RANG1);
	
	printf("P0phymode=%02x, %s phy\n", mode,   (mode==2) ? "external" : "embedded"  );
	
	if(mode==2)	return PHYTYPE_EXT;
	else 		return PHYTYPE_EMB;

	
}

//---------------------------------------------------------------------------------------
#define MACTYPE_MII_MAC 0
#define MACTYPE_MII_PHY 1
#define MACTYPE_RGMII 2
#define MACTYPE_GMII 3

unsigned int Get_P0_MiiMode()
{
		const unsigned char *miimodename[]={ "MII-MAC", "MII-PHY", "RGMII", "GMII-MAC" };

/*
	0: MII-MAC
	1: MII-PHY
	2: RGMII
	3: GMII-MAC
*/
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf	
	
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	
	unsigned int v=REG32(SYS_HW_STRAP);
	unsigned int mode=GET_BITVAL(v, 30, RANG2);

	printf("P0miimode=%02x, %s\n", mode, miimodename[mode] );


	if(mode==0)		return MACTYPE_MII_MAC;
	else if(mode==1)	return MACTYPE_MII_PHY;
	else if(mode==2) return MACTYPE_RGMII;
	else				return MACTYPE_GMII;

	
}
//---------------------------------------------------------------------------------------
unsigned int Get_P0_RxDelay()
{
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf	
	
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	
	unsigned int v=REG32(SYS_HW_STRAP);
	unsigned int val=GET_BITVAL(v, 26, RANG3);
	return val;

}
unsigned int Get_P0_TxDelay()
{
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf	
	
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	
	unsigned int v=REG32(SYS_HW_STRAP);
	unsigned int val=GET_BITVAL(v, 29, RANG1);
	return val;

}
#endif
//====================================================================

#if defined(CONFIG_RTL8196D)  
int Setting_RTL8196D_PHY(void)
{
	int i;
	
	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) |= (EnForceMode);

/*
	page addr rtl8196d-default rtl8196d-new value Purpose 
	0 21 0x02c5 0x0232 	Green: up/low bond to 3/2 
	0 22 0x5b85 0x5bd5 	Green: ad current from 2 to 3 
	1 18 0x901c 0x9004 	finetune AOI waveform 
	1 19 0x4400 0x5400 	finetune 100M DAC current 
	1 25 0x00da 0x00d0 	enable pwdn10rx at pwr saving enable snr threshold = 18dB 

	4 16 0x4007 0x737f 	enable EEE, fine tune EEE parameter 
	4 24 0xc0a0 0xc0f3 	change EEE wake idle to 10us 
	4 25 0x0130 0x0730 	turn off tx/rx pwr at LPI state 
*/


	Set_GPHYWB(999, 0, 21, 0, 0x0232);

	/* purpose: to avoid 100M N-way link fail issue Set_p="1" */
	Set_GPHYWB(999, 0, 22, 0, 0x5bd5);

 	/* purpose: to adjust AOI waveform */
	Set_GPHYWB(999, 1, 18, 0, 0x9004);

	/* purpose: to enhance ethernet 100Mhz output voltage about 1.0(v) */
	Set_GPHYWB(999, 1, 19, 0, 0x5400);

	Set_GPHYWB(999, 1, 25, 0, 0x00d0);  //enable pwdn10rx at pwr saving enable snr threshold = 18dB 
	
	Set_GPHYWB(999, 4, 16, 0, 0x737f);// enable EEE, fine tune EEE parameter 
	Set_GPHYWB(999, 4, 24, 0, 0xc0f3);	//change EEE wake idle to 10us 
	Set_GPHYWB(999, 4, 25, 0, 0x0730);	 // turn off tx/rx pwr at LPI state 


/*
	修改為 rtl8196e 的default 值
	#fine tune port on/off threshold to 160/148
	0xbb80450c 0x009400A0
	0xbb804510 0x009400A0
	0xbb804514 0x009400A0
	0xbb804518 0x009400A0
	0xbb80451c 0x009400A0
	0xbb804524 0x009400A0
	 
	#Fine tune minRx IPG from 6 to 5 byte
	0xbb804000 0x80420185
	 
	#LB parameter for swclk = 50/25Mhz (default value)
	#Note: per unit = 64Kbps
	memcmd write 1 0xbb804904 0x400b      
	memcmd write 1 0xbb804908 0xc0        
	memcmd write 1 0xbb804910 0x400b      
	 
	#default enable MAC EEE
	memcmd write 1 0xbb804160 0x28739ce7
*/
#if 1
	//MAC PATCH START
	REG32(0xbb80450c)=0x009400A0;
	REG32(0xbb804510)=0x009400A0;
	REG32(0xbb804514)=0x009400A0;
	REG32(0xbb804518)=0x009400A0;
	REG32(0xbb80451c)=0x009400A0;
	REG32(0xbb804524)=0x009400A0;

	REG32(0xbb804000)=0x80420185;	
/*
重新測試並確認後，下列的leaky bucket 參數不需修正，所以保持原本的default 值即可，抱歉造成大家的困擾!
 
#LB parameter for swclk = 50/25Mhz (default value)
#Note: per unit = 64Kbps

	REG32(0xbb804904)=0x400b;
	REG32(0xbb804908)=0xc0;
	REG32(0xbb804910)=0x400b;
*/
	REG32(0xbb804160)=0x28739ce7;	
	//MAC PATCH END
#endif

	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) &= ~(EnForceMode);

 
	return 0;
}
#endif

#if defined ( CONFIG_RTL8196E)
int Setting_RTL8196E_PHY(void)
{
	int i;
	
	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) |= (EnForceMode);


	//patch start
	/*  3. write page1, reg16, bit[15:13] Iq Current 110:175uA (default 100: 125uA)  */

	Set_GPHYWB(999, 1, 16, 0xffff-(7<<13), (6<<13) );	

	//--------close power saving mode from joey
	// disable power saving mode in A-cut only
	Set_GPHYWB(999, 0, 0x18, 0xffff-(1<<15), 0<<15);
	//-----------------------


	//patch end
	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) &= ~(EnForceMode);
}
#endif

#ifdef CONFIG_RTL8198C
#define RTL8198C_PORT0_PHY_ID		8

static const unsigned int phy_98c_ado[]={	
    27, 0x83de,
    28, 0xaf83,
    28, 0xeaaf,
    28, 0x83ed,
    28, 0xaf83,
    28, 0xf0af,
    28, 0x83f3,
    28, 0xaf0c,
    28, 0x0caf,
    28, 0x83ed,
    28, 0xaf83,
    28, 0xf0af,
    28, 0x83f3,
    27, 0xb818,
    28, 0x0bf1,
    27, 0xb81a,
    28, 0xfffd,
    27, 0xb81c,
    28, 0xfffd,
    27, 0xb81e,
    28, 0xfffd,
    27, 0xb832,
    28, 0x0001,
};

int Ado_RamCode_Efuse(int phyport)
{
	unsigned int readReg;
	int j , len;

	while(1)
	{
		rtl8651_setAsicEthernetPHYReg( phyport, 31, 0xa46 );
		rtl8651_getAsicEthernetPHYReg( phyport, 21, &readReg );
		rtl8651_setAsicEthernetPHYReg( phyport, 31, 0);

		if(((readReg >> 8) & 7) == 3)
			break;
	}

	Set_GPHYWB(phyport, 0xb82, 16, (0xffff & ~(1<<4)), (1<<4));

	while(1)
	{
		rtl8651_setAsicEthernetPHYReg( phyport, 31, 0xb80 );
		rtl8651_getAsicEthernetPHYReg( phyport, 16, &readReg );
		rtl8651_setAsicEthernetPHYReg( phyport, 31, 0);

		if(((readReg >> 6) & 1) == 1)
			break;
	}

	len = sizeof(phy_98c_ado)/sizeof(unsigned int);
	for(j=0;j<len;j=j+2)
	{
		Set_GPHYWB(phyport, 0, phy_98c_ado[j], 0, phy_98c_ado[j+1]);
	}

	Set_GPHYWB(phyport, 0xb82, 16, (0xffff & ~(1<<4)), (0<<4));
	return 0;
}

static const unsigned int phy_ado_data[]={	
	27, 0x83de,
	28, 0xaf83,
	28, 0xeaaf,
	28, 0x83ed,
	28, 0xaf85,
	28, 0x85af,
	28, 0x8588,
	28, 0xaf0c,
	28, 0x0cf6,
	28, 0x0102,
	28, 0x83f5,
	28, 0xaf00,
	28, 0x8ff8,
	28, 0xf9cd,
	28, 0xf9fa,
	28, 0xef69,
	28, 0xfafb,
	28, 0xe080,
	28, 0x13ac,
	28, 0x2303,
	28, 0xaf85,
	28, 0x21d1,
	28, 0x00bf,
	28, 0x8597,
	28, 0x0241,
	28, 0x05d1,
	28, 0x0fbf,
	28, 0x859a,
	28, 0x0241,
	28, 0x05bf,
	28, 0x859d,
	28, 0x0245,
	28, 0xbbbf,
	28, 0x85a0,
	28, 0x0245,
	28, 0xb3bf,
	28, 0x85a3,
	28, 0x0245,
	28, 0xb3bf,
	28, 0x85a6,
	28, 0x0245,
	28, 0xb3ee,
	28, 0x87f4,
	28, 0x001f,
	28, 0x44e0,
	28, 0x87f4,
	28, 0xbfa8,
	28, 0xc4ef,
	28, 0x591a,
	28, 0x54e6,
	28, 0x87f5,
	28, 0xe787,
	28, 0xf6e1,
	28, 0x87f4,
	28, 0xd000,
	28, 0xbf85,
	28, 0x8b4c,
	28, 0x0003,
	28, 0x1a49,
	28, 0xe487,
	28, 0xf7e5,
	28, 0x87f8,
	28, 0xee87,
	28, 0xf907,
	28, 0xee87,
	28, 0xfaff,
	28, 0xee87,
	28, 0xfb00,
	28, 0xee87,
	28, 0xfc10,
	28, 0xee87,
	28, 0xfd00,
	28, 0x0285,
	28, 0x2bef,
	28, 0x67ad,
	28, 0x5f03,
	28, 0x7fff,
	28, 0xffe2,
	28, 0x87f9,
	28, 0xe387,
	28, 0xfa09,
	28, 0xef56,
	28, 0xef67,
	28, 0x0bc6,
	28, 0x0244,
	28, 0xf4ad,
	28, 0x5015,
	28, 0xcce4,
	28, 0x87f9,
	28, 0xe587,
	28, 0xfae0,
	28, 0x87f7,
	28, 0xe187,
	28, 0xf8ef,
	28, 0x9402,
	28, 0x4143,
	28, 0xe587,
	28, 0xfee1,
	28, 0x87fd,
	28, 0x3904,
	28, 0x9e38,
	28, 0xe187,
	28, 0xfd11,
	28, 0xe587,
	28, 0xfde0,
	28, 0x87f7,
	28, 0xe187,
	28, 0xf8ef,
	28, 0x9402,
	28, 0x4143,
	28, 0xad37,
	28, 0x08e5,
	28, 0x87fc,
	28, 0xe087,
	28, 0xfbae,
	28, 0x06e5,
	28, 0x87fb,
	28, 0xe087,
	28, 0xfc1a,
	28, 0x100d,
	28, 0x11d0,
	28, 0x0008,
	28, 0xe087,
	28, 0xf7e1,
	28, 0x87f8,
	28, 0xef94,
	28, 0x0802,
	28, 0x4105,
	28, 0xae8e,
	28, 0xe187,
	28, 0xfed0,
	28, 0x0008,
	28, 0xe087,
	28, 0xf7e1,
	28, 0x87f8,
	28, 0xef94,
	28, 0x0802,
	28, 0x4105,
	28, 0xe187,
	28, 0xf439,
	28, 0x039e,
	28, 0x0ae1,
	28, 0x87f4,
	28, 0x11e5,
	28, 0x87f4,
	28, 0xaf84,
	28, 0x33d1,
	28, 0x00bf,
	28, 0x859a,
	28, 0x0241,
	28, 0x05bf,
	28, 0x85a6,
	28, 0x0245,
	28, 0xbbbf,
	28, 0x859d,
	28, 0x0245,
	28, 0xb3bf,
	28, 0x85a0,
	28, 0x0245,
	28, 0xbbbf,
	28, 0x85a3,
	28, 0x0245,
	28, 0xbbff,
	28, 0xfeef,
	28, 0x96fe,
	28, 0xfdc5,
	28, 0xfdfc,
	28, 0x04f8,
	28, 0xf9fa,
	28, 0xef69,
	28, 0xfa1f,
	28, 0x441f,
	28, 0x77e2,
	28, 0x87f5,
	28, 0xe387,
	28, 0xf6ef,
	28, 0x95e2,
	28, 0x87f3,
	28, 0x1f66,
	28, 0x160c,
	28, 0x61b2,
	28, 0xfcda,
	28, 0x19db,
	28, 0x890c,
	28, 0x570d,
	28, 0x581a,
	28, 0x45ef,
	28, 0x54ef,
	28, 0x323a,
	28, 0x7f9e,
	28, 0x063b,
	28, 0x809e,
	28, 0x08ae,
	28, 0x0a17,
	28, 0x5c00,
	28, 0xffae,
	28, 0x0487,
	28, 0x6cff,
	28, 0x00b6,
	28, 0xdc4f,
	28, 0x007f,
	28, 0x0d48,
	28, 0x1a74,
	28, 0xe087,
	28, 0xf338,
	28, 0x089e,
	28, 0x050d,
	28, 0x7180,
	28, 0xaef9,
	28, 0xfeef,
	28, 0x96fe,
	28, 0xfdfc,
	28, 0x04af,
	28, 0x8585,
	28, 0xaf85,
	28, 0x8830,
	28, 0xbcfc,
	28, 0x74bc,
	28, 0xfcb8,
	28, 0xbcfc,
	28, 0xfcbc,
	28, 0xfc40,
	28, 0xa810,
	28, 0x96a8,
	28, 0x10aa,
	28, 0xbcd2,
	28, 0xccbc,
	28, 0xd2ee,
	28, 0xbcd2,
	28, 0xffbc,
	28, 0xd200,
	27, 0xb818,
	28, 0x0bf1,
	27, 0xb81a,
	28, 0x008d,
	27, 0xb81c,
	28, 0xfffd,
	27, 0xb81e,
	28, 0xfffd,
	27, 0xb832,
	28, 0x0003,
	31, 0x0000,
	27, 0x87f2,
	28, 0x000f,	
};  

int phy_ready(int phyport)
{
	uint32 readReg, count=0;
	
	while(1)
	{
		rtl8651_setAsicEthernetPHYReg( phyport, 31, 0xa46 );
		rtl8651_getAsicEthernetPHYReg( phyport, 21, &readReg );
		rtl8651_setAsicEthernetPHYReg( phyport, 31, 0);

		if(((readReg >> 8) & 7) == 2)
			return 1;
		else if((count++) >= 10) 
			return 0;
	}
}

/*
 must do this adc refinement in phy_state 2 (page 0xa46, reg 21, bit 9-8),
 */
unsigned int efuse_value[5];
void ado_refine(void)
{
	int i,phyport;
	int j, len;
	uint32 readReg;

	REG32(SYS_CLK_MAG) |= CM_ACTIVE_SWCORE;

	for(i=0;i<5;i++) {
		REG32(PCRP0+i*4) |= (EnForceMode);
		efuse_value[i] = 0;
	}
	
	if ((REG32(REVR) & 0xfff) > 0) {
		REG32(EFUSE_TIMING_CONTROL)=0x030c174f; 
		REG32(EFUSE_CONFIG)=0x00040112; 

		for(i=0; i<5; i++) {
			REG32(EFUSE_CONFIG) |= EFUSE_CFG_INT_STS;		/* clear efuse interrupt status bit */
			REG32(EFUSE_CMD) = i;
			while( ( REG32(EFUSE_CONFIG) & EFUSE_CFG_INT_STS ) == 0 );		/* Wait efuse_interrupt */
			efuse_value[i] = REG32(EFUSE_RW_DATA);
		}
	}

	rtl8651_getAsicEthernetPHYReg( RTL8198C_PORT0_PHY_ID, 0, &readReg );
	
	for(i=0;i<5;i++)
	{
		if (i==0)
			phyport = RTL8198C_PORT0_PHY_ID;
		else 
			phyport = i;
		
		rtl8651_getAsicEthernetPHYReg( phyport, 0, &readReg );
		
		if((readReg & POWER_DOWN) == 0) // power_down=1, do it
			continue;
		
		if ((phy_ready(phyport)) == 0) // phy state=2, do it
			continue;
		
		if ((efuse_value[i] & 0x80))   // bit7=0, do it
			continue;

		len = sizeof(phy_ado_data)/sizeof(unsigned int);
		for(j=0;j<len;j=j+2)
		{
			Set_GPHYWB(phyport, 0, phy_ado_data[j], 0, phy_ado_data[j+1]);
		}	
	}
}

int Setting_RTL8198C_GPHY(void)
{
	int i, phyid;
	unsigned int data;

	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) |= (EnForceMode);

	// to disable the PCS INT (GPHY PCS interrupt (MAC_INTRUPT  signal )). 
	Set_GPHYWB(999, 0xa42, 18, 0, 0);

	// read to clear INT status. 
	for(i=0; i<5; i++) {
		if (i==0)
			phyid = RTL8198C_PORT0_PHY_ID;
		else 
			phyid = i;
		
		rtl8651_setAsicEthernetPHYReg( phyid, 31, 0xa42);
		rtl8651_getAsicEthernetPHYReg( phyid, 29, &data);		
		rtl8651_setAsicEthernetPHYReg( phyid, 31, 0);
	}

	// fix MDI 10pf codedsp shift
	Set_GPHYWB(999, 0xBCD, 21, 0, 0x2222);	// codedsp register = 2
	Set_GPHYWB(999, 0, 27, 0, 0x8277);
	Set_GPHYWB(999, 0, 28, 0xffff - 0xff00, 0x02 << 8);	// uc codedsp = 2

	// giga master sd_thd
	Set_GPHYWB(999, 0, 27, 0, 0x8101);
	Set_GPHYWB(999, 0, 28, 0, 0x4000);

	/* enlarge "Flow control DSC tolerance" from 36 pages to 48 pages
	    to prevent the hardware may drop incoming packets
	    after flow control triggered and Pause frame sent */
 	REG32(MACCR)= (REG32(MACCR) & ~CF_FCDSC_MASK) | (0x30 << CF_FCDSC_OFFSET);

	/* set "System clock selection" to 100MHz 
		2'b00: 50MHz, 2'b01: 100MHz, 2'b10, 2'b11 reserved
	*/
 	REG32(MACCR)= (REG32(MACCR) & ~(CF_SYSCLK_SEL_MASK)) | (0x01 << CF_SYSCLK_SEL_OFFSET); 

	/* set "Cport MAC clock selection with NIC interface" to lx_clk */
 	REG32(MACCTRL1) |= CF_CMAC_CLK_SEL;

	if ((REG32(REVR) & 0xfff) > 0) {
		for(i=0;i<5;i++)
		{
			if ((efuse_value[i] & 0x80)) { // bit7 != 0
			 	// to backfill adc_ioffset
				if (i==0)
					phyid = RTL8198C_PORT0_PHY_ID;
				else 
					phyid = i;

				Ado_RamCode_Efuse(phyid);								
				Set_GPHYWB(phyid, 0xbcf, 22, 0, (efuse_value[i]>>8)&0xffff);
			}
		}
	}	

	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) &= ~(EnForceMode);

	return 0;
}
#endif

int32 swCore_init()
{
	#define REG32_ANDOR(x,y,z)   (REG32(x)=(REG32(x)& (y))|(z))

	int port;

#ifdef CONFIG_RTL8198C
	ado_refine();
#endif

	/* Full reset and semreset */
	FullAndSemiReset();
	
#ifdef CONFIG_RTL8198C
	Setting_RTL8198C_GPHY();
#endif

#ifndef CONFIG_FPGA_ROMCODE
#if defined(CONFIG_RTL8196D)  
	Setting_RTL8196D_PHY();
#endif	

#if defined ( CONFIG_RTL8196E)
	Setting_RTL8196E_PHY();
#endif
#endif
#if 0//ndef CONFIG_FPGA_ROMCODE //FPGA 8196D phy patch  //20110727 maxod tell not patch
	int i;
	for(i=0; i<5; i++)
	{	
        REG32(PCRP0+i*4) |= (EnForceMode);	
		rtl8651_setAsicEthernetPHYReg( i, 31, 0x0001);
		rtl8651_setAsicEthernetPHYReg( i, 25, 0x00da );	
        REG32(PCRP0+i*4) &= ~(EnForceMode);	
	}
#endif		

	/* rtl8651_clearAsicAllTable */
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E) ||defined(CONFIG_RTL8881A)
	REG32(MEMCR) = 0;
	REG32(MEMCR) = 0x7f;
	_rtl8651_clearSpecifiedAsicTable(TYPE_MULTICAST_TABLE, RTL8651_IPMULTICASTTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_NETINTERFACE_TABLE, RTL865XC_NETINTERFACE_NUMBER);
#elif defined(RTL8198)
	// SERVER_PORT_TABLE, ALG_TABLE and L4_ICMP_TABLE are removed in real chip
	_rtl8651_clearSpecifiedAsicTable(TYPE_L2_SWITCH_TABLE, RTL8651_L2TBL_ROW*RTL8651_L2TBL_COLUMN);
	_rtl8651_clearSpecifiedAsicTable(TYPE_ARP_TABLE, RTL8651_ARPTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_L3_ROUTING_TABLE, RTL8651_ROUTINGTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_MULTICAST_TABLE, RTL8651_IPMULTICASTTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_NETINTERFACE_TABLE, RTL865XC_NETINTERFACE_NUMBER);
	_rtl8651_clearSpecifiedAsicTable(TYPE_VLAN_TABLE, RTL865XC_VLAN_NUMBER);
	_rtl8651_clearSpecifiedAsicTable(TYPE_EXT_INT_IP_TABLE, RTL8651_IPTABLE_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_L4_TCP_UDP_TABLE, RTL8651_TCPUDPTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_PPPOE_TABLE, RTL8651_PPPOE_NUMBER);
	_rtl8651_clearSpecifiedAsicTable(TYPE_ACL_RULE_TABLE, RTL8651_ACLTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_NEXT_HOP_TABLE, RTL8651_NEXTHOPTBL_SIZE);
	_rtl8651_clearSpecifiedAsicTable(TYPE_RATE_LIMIT_TABLE, RTL8651_RATELIMITTBL_SIZE);	

	//_rtl8651_clearSpecifiedAsicTable(TYPE_ALG_TABLE, RTL8651_ALGTBL_SIZE);
	//_rtl8651_clearSpecifiedAsicTable(TYPE_SERVER_PORT_TABLE, RTL8651_SERVERPORTTBL_SIZE);

	//_rtl8651_clearSpecifiedAsicTable(TYPE_L4_ICMP_TABLE, RTL8651_ICMPTBL_SIZE);
#endif



#ifdef CONFIG_NFBI
#if defined(RTL8198)
	REG32(PCRP0) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
	REG32(PCRP1) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
	REG32(PCRP2) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
	REG32(PCRP3) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
	REG32(PCRP4) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
	REG32(PCRP5) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
	
	REG32(PCRP0) = REG32(PCRP0) | (0 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;
	REG32(PCRP1) = REG32(PCRP1) | (1 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;
	REG32(PCRP2) = REG32(PCRP2) | (2 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;
	REG32(PCRP3) = REG32(PCRP3) | (3 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;
	REG32(PCRP4) = REG32(PCRP4) | (4 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;
	
	REG32(PITCR) = REG32(PITCR) & 0xFFFFF3FF; //configure port 5 to be a MII interface

	 #ifdef  CONFIG_NFBI_SWITCH_P5_MII_PHY_MODE //8198 P5 MII
	 	dprintf("Set 8198 Switch Port5 as MII PHY mode OK\n");
		rtl865xC_setAsicEthernetMIIMode(5, LINK_MII_PHY); //port 5 MII PHY mode
		REG32(P5GMIICR) = REG32(P5GMIICR) | 0x40; //Conf_done=1
		REG32(PCRP5) = 0 | (0x10<<ExtPHYID_OFFSET) |
				EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex |
				MIIcfg_RXER | EnablePHYIf | MacSwReset;
 	#endif

	 #ifdef  CONFIG_NFBI_SWITCH_P5_GMII_MAC_MODE //8198 P5 GMII
	 	dprintf("swcore.c:Set 8198 Switch Port5 as GMII MAC mode OK\n");
	 	rtl865xC_setAsicEthernetMIIMode(5, LINK_MII_MAC); //port 5  GMII/MII MAC auto mode 

		REG32(P5GMIICR) = REG32(P5GMIICR) | 0x40; //Conf_done=1
		
	 		REG32(PCRP5) = 0 | (0x5<<ExtPHYID_OFFSET) |	 //JSW@20100309:For external 8211BN GMII ,PHYID must be 5		
				EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex |
				MIIcfg_RXER | EnablePHYIf | MacSwReset; 		

	 #endif
#endif  //defined(RTL8198)

#else  //8198, not NFBI
		//anson add
		//REG32(0xbb804300)= 0x00055500;

		REG32(PCRP0) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
                REG32(PCRP1) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
                REG32(PCRP2) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
                REG32(PCRP3) &= (0xFFFFFFFF-(0x00000000|MacSwReset));
                REG32(PCRP4) &= (0xFFFFFFFF-(0x00000000|MacSwReset));

//		REG32(PCRP0) = REG32(PCRP0) | (0 << ExtPHYID_OFFSET) | AcptMaxLen_16K | EnablePHYIf | MacSwReset;   //move to below

		REG32(PCRP1) = REG32(PCRP1) | (1 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;
		REG32(PCRP2) = REG32(PCRP2) | (2 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;
		REG32(PCRP3) = REG32(PCRP3) | (3 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;
		REG32(PCRP4) = REG32(PCRP4) | (4 << ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;

#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)

	#if defined(CONFIG_RTL8198C)
	unsigned int rtl96d_P0phymode=PHYTYPE_EMB;
	#else
	unsigned int rtl96d_P0phymode=Get_P0_PhyMode();
	#endif

	if(rtl96d_P0phymode==PHYTYPE_EMB)  //embedded phy
	{	
#ifdef CONFIG_FPGA_ROMCODE	
		/*
			PORT= ExtP0	: EmbP0	: EmbP1
			PHYID= 0x10	: 0x011	:	0x01
		*/
		REG32_ANDOR(0xbb804050, ~(0x1f<<0), 0x11<<0); 
		REG32(PCRP0) |=  (0<< ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;	//emabedded
#else
		REG32(PCRP0) |=  (0 << ExtPHYID_OFFSET) | EnablePHYIf | MacSwReset;	//emabedded
#endif
	}	
	else //external phy
	{
		REG32(PCRP0) |=  (0x10 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;	//external

			
		unsigned int rtl96d_P0miimode=Get_P0_MiiMode();
		
		if(rtl96d_P0miimode==MACTYPE_MII_MAC)       		REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_MAC<<23); 
		else if(rtl96d_P0miimode==MACTYPE_MII_PHY)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_PHY<<23);  
		else if(rtl96d_P0miimode==MACTYPE_RGMII)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_RGMII<<23);  //GMII
		else if(rtl96d_P0miimode==MACTYPE_GMII)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  ,  LINK_MII_MAC<<23);   

		 if(rtl96d_P0miimode==2) 
		 {
			unsigned int rtl96d_P0txdly=Get_P0_TxDelay();
			unsigned int rtl96d_P0rxdly=Get_P0_RxDelay();	
			REG32_ANDOR(P0GMIICR, ~((1<<4)|(3<<0)) , (rtl96d_P0txdly<<4) | (rtl96d_P0rxdly<<0) );			
		 }

#ifdef CONFIG_FPGA_ROMCODE	
		if(rtl96d_P0miimode==MACTYPE_MII_MAC)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed10M |ForceDuplex) ; 
#else
		if(rtl96d_P0miimode==MACTYPE_MII_MAC)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ; 
#endif
		else if(rtl96d_P0miimode==MACTYPE_MII_PHY)    	 	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ;

		else if(rtl96d_P0miimode==MACTYPE_RGMII)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );
		else if(rtl96d_P0miimode==MACTYPE_GMII)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );


		REG32(P0GMIICR) |=(Conf_done);

		
		REG32(PITCR) |= (1<<0);   //00: embedded , 01L GMII/MII/RGMII

		if((rtl96d_P0miimode==MACTYPE_RGMII)  ||(rtl96d_P0miimode==MACTYPE_GMII))
		{	REG32(0xbb804000) |= (1<<12);   //giga link
		}

	}
#endif  


	
#endif


	/* Set PVID of all ports to 8 */
	REG32(PVCR0) = (0x8 << 16) | 0x8;
	REG32(PVCR1) = (0x8 << 16) | 0x8;
	REG32(PVCR2) = (0x8 << 16) | 0x8;
	REG32(PVCR3) = (0x8 << 16) | 0x8;

	
	/* Enable L2 lookup engine and spanning tree functionality */
	// REG32(MSCR) = EN_L2 | EN_L3 | EN_L4 | EN_IN_ACL;
	REG32(MSCR) = EN_L2;
	REG32(QNUMCR) = P0QNum_1 | P1QNum_1 | P2QNum_1 | P3QNum_1 | P4QNum_1;

	/* Start normal TX and RX */
	REG32(SIRR) |= TRXRDY;
	
	/* Init PHY LED style */

#if defined(CONFIG_RTL8198C)

	#ifdef CONFIG_USING_JTAG
	REG32(PIN_MUX_SEL3) = (REG32(PIN_MUX_SEL3) & ~ (0x7FFF));  //LED0~LED4
	#else
	REG32(PIN_MUX_SEL3) = (REG32(PIN_MUX_SEL3) & ~ (0x7FFF)) | ((1<<0) | (1<<3) | (1<<6) | (1<<9) | (1<<12));  //LED0~LED4
	#endif

	REG32(LEDCR0) = (REG32(LEDCR0) & ~ LEDTOPOLOGY_MASK) |LEDMODE_DIRECT;
	REG32(DIRECTLCR) = (REG32(DIRECTLCR) & ~ LEDONSCALEP0_MASK) |(7<<LEDONSCALEP0_OFFSET);

#elif defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
	/*
		#LED = direct mode
		set mode 0x0
		swwb 0xbb804300 21-20 0x2 19-18 $mode 17-16 $mode 15-14 $mode 13-12 $mode 11-10 $mode 9-8 $mode
	*/	
	REG32(PIN_MUX_SEL)= (REG32(PIN_MUX_SEL) & ~( (7<<2) | (7<<7) | (3<<10) |  (1<<15) )) | (4<<2);  //let P0 to mii mode
	
	REG32(PIN_MUX_SEL2)&=~ ((7<<0) | (7<<3) | (7<<6) | (7<<9) | (3<<12)   );  //S0-S3, P0
	
	REG32(LEDCR)  = (2<<20) | (0<<18) | (0<<16) | (0<<14) | (0<<12) | (0<<10) | (0<<8);  //P0-P5
#endif

	
	/*PHY FlowControl. Default enable*/
	for(port=0;port<MAX_PORT_NUMBER;port++)
	{
		/* Set Flow Control capability. */

#if  defined(RTL8198)
#ifdef CONFIG_FPGA_ROMCODE //0//FPGA
		if(port ==0)
					rtl8651_restartAsicEthernetPHYNway(port+1, 0x11);
		else
			
#endif
		rtl8651_restartAsicEthernetPHYNway(port+1, port);
#else
		if (port == MAX_PORT_NUMBER-1)
			rtl8651_setAsicFlowControlRegister(RTL8651_MII_PORTNUMBER, TRUE, GIGA_PHY_ID);
		else				
			rtl8651_restartAsicEthernetPHYNway(port+1, port);	
#endif			
			
	}
	

#if ! (defined( CONFIG_NFBI) || defined(CONFIG_NONE_FLASH))

	{		
		extern char eth0_mac[6];
		extern char eth0_mac_httpd[6];
		rtl8651_setAsicL2Table((ether_addr_t*)(&eth0_mac), 0);
		rtl8651_setAsicL2Table((ether_addr_t*)(&eth0_mac_httpd), 1);
	}
#endif

	REG32(FFCR) = EN_UNUNICAST_TOCPU | EN_UNMCAST_TOCPU; // rx broadcast and unicast packet
	return 0;
}



#define BIT(x)     (1 << (x))
void set_phy_pwr_save(int val)
{
	int i;
	uint32 reg_val;
	
	for(i=0; i<5; i++)
	{
		rtl8651_getAsicEthernetPHYReg( i, 24, &reg_val);

		if (val == 1)
			rtl8651_setAsicEthernetPHYReg( i, 24, (reg_val | BIT(15)) );
		else 
			rtl8651_setAsicEthernetPHYReg( i, 24, (reg_val & (~BIT(15))) );
		
//		rtl8651_restartAsicEthernetPHYNway(i+1, i);							
			}
}


