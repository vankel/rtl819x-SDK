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
#ifdef CONFIG_RTL8881A

//#define CONFIG_RTL8196D 1 
#endif


#include <linux/config.h>

#include <rtl8196x/asicregs.h>
#include <rtl8196x/swCore.h>
#include <rtl8196x/phy.h>
#include <asm/rtl8198.h>
#include <asm/system.h>

#ifdef CONFIG_SW_8367R
#include <asm/delay.h>
#endif

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

#if 0
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
#endif

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

	//return _rtl8651_forceAddAsicEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
	return swTable_forceAddEntry(TYPE_L2_SWITCH_TABLE, row<<2 | column, &entry);
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

void FullAndSemiReset( void )
{

	//dprintf("\nSwitch FullAndSemiReset\n");    
	/* FIXME: Currently workable for FPGA, may need further modification for real chip */
	//REG32(0xb8000010)|=(1<<27);  //protect bit=1

	REG32(0xb8000010)&= ~(1<<11);  //active_swcore=0

	__delay(5000);
	
	REG32(0xb8000010)|= (1<<11);  //active_swcore=1

	//REG32(0xb8000010)&=~(1<<27);   //protect bit=0

	__delay(1000);
#if 0
	    /* Perform full-reset for sw-core. */ 
	    REG32(SIRR) |= FULL_RST;

		tick_Delay10ms(50*5);

		/* Enable TRXRDY */
		REG32(SIRR) |= TRXRDY;

#endif




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
#if defined(CONFIG_RTL8198)
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

#if  !defined(RTL8198)
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
#endif

//====================================================================
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
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

		if(page>=31)
		{	rtl8651_setAsicEthernetPHYReg( wphyid, 31, 7  );
			rtl8651_setAsicEthernetPHYReg( wphyid, 30, page  );
		}
		else
		{
			rtl8651_setAsicEthernetPHYReg( wphyid, 31, page  );
		}
		if(mask!=0)
		{
			rtl8651_getAsicEthernetPHYReg( wphyid, reg, &data);
			data=data&mask;
		}
		rtl8651_setAsicEthernetPHYReg( wphyid, reg, data|val  );


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
	}
}
#endif

//====================================================================

#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)
//#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
#define PHYTYPE_EMB 0
#define PHYTYPE_EXT 1
unsigned int Get_P0_PhyMode()
{
/*
	00: External  phy
	01: embedded phy
	10: olt
	11: deb_sel
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
/*
	0: MII-PHY
	1: MII-MAC
	2: GMII-MAC
	3: RGMII
*/
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf	
	
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	
	unsigned int v=REG32(SYS_HW_STRAP);
	unsigned int mode=GET_BITVAL(v, 30, RANG2);

	//printf("P0miimode=%02x, %s\n", mode, miimodename[mode] );


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

#ifdef CONFIG_RTL8196D
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

	//MAC PATCH START
	REG32(0xbb80450c)=0x009400A0;
	REG32(0xbb804510)=0x009400A0;
	REG32(0xbb804514)=0x009400A0;
	REG32(0xbb804518)=0x009400A0;
	REG32(0xbb80451c)=0x009400A0;
	REG32(0xbb804524)=0x009400A0;

REG32(0xbb804000)=0x80420185;	

	/* 100M half duplex enhancement */
	/* fix Smartbit Half duplex backpressure IOT issue */
 	REG32(MACCR)= (REG32(MACCR) & ~(CF_RXIPG_MASK | SELIPG_MASK)) | (0x05 | SELIPG_11);

	REG32(0xbb804904)=0x400b;
	REG32(0xbb804908)=0xc0;
	REG32(0xbb804910)=0x400b;

	REG32(0xbb804160)=0x28739ce7;	
	//MAC PATCH END

	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) &= ~(EnForceMode);

 
	return 0;
}
#endif

#ifdef CONFIG_RTL8196E
#define SYS_ECO_NO 0xb8000000

int Setting_RTL8196E_PHY(void)
{
	int i;
	
	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) |= (EnForceMode);

	// write page1, reg16, bit[15:13] Iq Current 110:175uA (default 100: 125uA)
	Set_GPHYWB(999, 1, 16, 0xffff-(0x7<<13), 0x6<<13);

	if (REG32(SYS_ECO_NO) == 0x8196e000) {
		// disable power saving mode in A-cut only
		Set_GPHYWB(999, 0, 0x18, 0xffff-(1<<15), 0<<15);
	}
	/* B-cut and later,
	    just increase a little power in long RJ45 cable case for Green Ethernet feature.
	 */
	else 
	{
		// adtune_lb setting
		Set_GPHYWB(999, 0, 22, 0xffff-(0x7<<4), 0x4<<4);
		//Setting SNR lb and hb
		Set_GPHYWB(999, 0, 21, 0xffff-(0xff<<0), 0xc2<<0);
		//auto bais current
		Set_GPHYWB(999, 1, 19, 0xffff-(0x1<<0), 0x0<<0);
		Set_GPHYWB(999, 0, 22, 0xffff-(0x1<<3), 0x0<<3);
	}
	
	/* 100M half duplex enhancement */
	/* fix Smartbit Half duplex backpressure IOT issue */
 	REG32(MACCR)= (REG32(MACCR) & ~(CF_RXIPG_MASK | SELIPG_MASK)) | (0x05 | SELIPG_11);

	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) &= ~(EnForceMode);
 
	return 0;
}
#endif


#ifdef CONFIG_RTL8881A

#define SYS_ECO_NO 0xb8000000
int Setting_RTL8881A_PHY(void)
{
	int i;
	
	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) |= (EnForceMode);


#if 0 //Yozen for RTL8881A
	Page1, Reg23, 

//Vref[7:6] 10: default 

Page1, Reg16

//idac[11:9] 100-> 010

//cf1[6:4] 110->001


#endif	
if(((REG32(0xb8000000)&0xff)>0))
{
	#define PLL2 0xb8000058
	REG32(PLL2 )|=(5<<6);//20120906:Set RBG_L[8:6]=101 from RDC suggestion

	Set_GPHYWB(999, 1, 23, 0xffff-(3<<6), 2<<6);//SwitchPatch #2 Page1,Reg.23[7:6] = 10

	Set_GPHYWB(999, 1, 16, 0xffff-(7<<4), 1<<4);//SwitchPatch #3 Page1,Reg.16[6:4] = 001

	//Set_GPHYWB(999, 1, 29, 0xffff-(1<<0), 0<<0);//SwitchPatch #4 Page1,Reg.29[0] = 0 //A cut don't need it
	//Set_GPHYWB(999, 0, 0, 0xffff-(1<<9), 1<<9);//SwitchPatch #4 Page0,Reg.0[9] = 1 //A cut don't need it
	//Set_GPHYWB(999, 1, 29, 0xffff-(1<<0), 1<<0);//SwitchPatch #4 Page1.Reg.29[0] =1 //A cut don't need it

	//write page 31 = reg4 to check the PHY parameter

	Set_GPHYWB(999, 4, 16, 0, 0x4077);//SwitchPatch #5 Page4,Reg.16 = 0x4077

	Set_GPHYWB(999, 4, 24, 0, 0xc0f3);//SwitchPatch #6 Page4,Reg.24 = 0xc0f3

	Set_GPHYWB(999, 4, 25, 0, 0x7630);//SwitchPatch #7 Page4,Reg.25 = 0x7630

	//REG32( 0xBB804160)=0x0E739CE7 ;//SwitchPatch #8 BB804160 = 0x0E739CE7 ,Set EEE MAC register
	REG32( 0xBB804160)=0x0 ;//SwitchPatch #8 BB804160 = 0x0E739CE7 ,Set EEE MAC register

	Set_GPHYWB(999, 0, 21, 0xffff-(15<<4), 2<<4);//Patch #9 Page0,Reg.21[7:4]=0010  ,Set Green ethernet

	Set_GPHYWB(999, 0, 21, 0xffff-(15<<0), 1<<0);//Patch #10 Page0,Reg.21[3:0]=0001  ,Set Green ethernet

	Set_GPHYWB(999, 1, 19, 0xffff-(3<<3), 1<<3);//Patch #11 Page1,Reg.19[4:3]=01  ,低溫低壓corner internetl SWR Pass 1.12V

	Set_GPHYWB(999, 1, 17, 0xffff-(1<<14), 1<<14);//Patch #12 Page1,Reg.17[14]=1  ,Fix Low SNR issue.

	Set_GPHYWB(999, 1, 16, 0xffff-(3<<12), 0<<0);//Patch #13 Page1,Reg.16[13:12]=00  ,IOL For 10M MAU

}
else
{
	Set_GPHYWB(999, 1, 23, 0xffff-(3<<6), 2<<6);//SwitchPatch #2 Page1,Reg.23[7:6] = 10

	Set_GPHYWB(999, 1, 16, 0xffff-(7<<4), 1<<4);//SwitchPatch #3 Page1,Reg.16[6:4] = 001

	Set_GPHYWB(999, 1, 29, 0xffff-(1<<0), 0<<0);//SwitchPatch #4 Page1,Reg.29[0] = 0 //A cut don't need it
	Set_GPHYWB(999, 0, 0, 0xffff-(1<<9), 1<<9);//SwitchPatch #4 Page0,Reg.0[9] = 1 //A cut don't need it
	Set_GPHYWB(999, 1, 29, 0xffff-(1<<0), 1<<0);//SwitchPatch #4 Page1.Reg.29[0] =1 //A cut don't need it

	//write page 31 = reg4 to check the PHY parameter

	Set_GPHYWB(999, 4, 16, 0, 0x7377);//SwitchPatch #5 Page4,Reg.16 = 0x7377

	Set_GPHYWB(999, 4, 24, 0, 0xc0f3);//SwitchPatch #6 Page4,Reg.24 = 0xc0f3

	Set_GPHYWB(999, 4, 25, 0, 0x7630);//SwitchPatch #7 Page4,Reg.25 = 0x7630

	REG32( 0xBB804160)=0x0E739CE7 ;//SwitchPatch #8 BB804160 = 0x0E739CE7 ,Set EEE MAC register

	Set_GPHYWB(999, 0, 21, 0xffff-(15<<4), 3<<4);//Patch #9 Page0,Reg.21[7:4]=0011  ,Set Green ethernet

	Set_GPHYWB(999, 0, 21, 0xffff-(15<<0), 2<<0);//Patch #10 Page0,Reg.21[3:0]=0010  ,Set Green ethernet
}
#if 0
	#if 0// RTL8881A test chip
	
	Set_GPHYWB(999, 1, 23, 0xffff-(3<<6), 2<<6);//SwitchPatch #2 Page1,Reg.23[7:6] = 10

	Set_GPHYWB(999, 1, 16, 0xffff-(7<<4), 1<<4);//SwitchPatch #3 Page1,Reg.16[6:4] = 001

	Set_GPHYWB(999, 1, 29, 0xffff-(1<<0), 0<<0);//SwitchPatch #4 Page1,Reg.29[0] = 0 //A cut don't need it
	Set_GPHYWB(999, 0, 0, 0xffff-(1<<9), 1<<9);//SwitchPatch #4 Page0,Reg.0[9] = 1 //A cut don't need it
	Set_GPHYWB(999, 1, 29, 0xffff-(1<<0), 1<<0);//SwitchPatch #4 Page1.Reg.29[0] =1 //A cut don't need it

	//write page 31 = reg4 to check the PHY parameter

	Set_GPHYWB(999, 4, 16, 0, 0x7377);//SwitchPatch #5 Page4,Reg.16 = 0x7377

	Set_GPHYWB(999, 4, 24, 0, 0xc0f3);//SwitchPatch #6 Page4,Reg.24 = 0xc0f3

	Set_GPHYWB(999, 4, 25, 0, 0x7630);//SwitchPatch #7 Page4,Reg.25 = 0x7630

	REG32( 0xBB804160)=0x0E739CE7 ;//SwitchPatch #8 BB804160 = 0x0E739CE7 ,Set EEE MAC register

	Set_GPHYWB(999, 0, 21, 0xffff-(15<<4), 3<<4);//Patch #9 Page0,Reg.21[7:4]=0011  ,Set Green ethernet

	Set_GPHYWB(999, 0, 21, 0xffff-(15<<0), 2<<0);//Patch #10 Page0,Reg.21[3:0]=0010  ,Set Green ethernet

#else // for RTL8881A A cut chip
	#define PLL2 0xb8000058
	REG32(PLL2 )|=(5<<6);//20120906:Set RBG_L[8:6]=101 from RDC suggestion

	Set_GPHYWB(999, 1, 23, 0xffff-(3<<6), 2<<6);//SwitchPatch #2 Page1,Reg.23[7:6] = 10

	Set_GPHYWB(999, 1, 16, 0xffff-(7<<4), 1<<4);//SwitchPatch #3 Page1,Reg.16[6:4] = 001

	//Set_GPHYWB(999, 1, 29, 0xffff-(1<<0), 0<<0);//SwitchPatch #4 Page1,Reg.29[0] = 0 //A cut don't need it
	//Set_GPHYWB(999, 0, 0, 0xffff-(1<<9), 1<<9);//SwitchPatch #4 Page0,Reg.0[9] = 1 //A cut don't need it
	//Set_GPHYWB(999, 1, 29, 0xffff-(1<<0), 1<<0);//SwitchPatch #4 Page1.Reg.29[0] =1 //A cut don't need it

	//write page 31 = reg4 to check the PHY parameter

	Set_GPHYWB(999, 4, 16, 0, 0x4077);//SwitchPatch #5 Page4,Reg.16 = 0x4077

	Set_GPHYWB(999, 4, 24, 0, 0xc0f3);//SwitchPatch #6 Page4,Reg.24 = 0xc0f3

	Set_GPHYWB(999, 4, 25, 0, 0x7630);//SwitchPatch #7 Page4,Reg.25 = 0x7630

	//REG32( 0xBB804160)=0x0E739CE7 ;//SwitchPatch #8 BB804160 = 0x0E739CE7 ,Set EEE MAC register
	REG32( 0xBB804160)=0x0 ;//SwitchPatch #8 BB804160 = 0x0E739CE7 ,Set EEE MAC register

	Set_GPHYWB(999, 0, 21, 0xffff-(15<<4), 2<<4);//Patch #9 Page0,Reg.21[7:4]=0010  ,Set Green ethernet

	Set_GPHYWB(999, 0, 21, 0xffff-(15<<0), 1<<0);//Patch #10 Page0,Reg.21[3:0]=0001  ,Set Green ethernet

	Set_GPHYWB(999, 1, 19, 0xffff-(3<<3), 1<<3);//Patch #11 Page1,Reg.19[4:3]=01  ,低溫低壓corner internetl SWR Pass 1.12V

	Set_GPHYWB(999, 1, 17, 0xffff-(1<<14), 1<<14);//Patch #12 Page1,Reg.17[14]=1  ,Fix Low SNR issue.

	Set_GPHYWB(999, 1, 16, 0xffff-(3<<12), 0<<0);//Patch #13 Page1,Reg.16[13:12]=00  ,IOL For 10M MAU
#endif
	
#endif
	//-----------------------

#ifndef CONFIG_SW_8367R
	// refine for B-cut and later
	{
	unsigned int val= REG32(0xb80000dc);
	if ((val & 0x3) != 0x3)
		REG32(0xb80000dc) |= 0x3; // wlanmac_control, set active_wlanmac_sys and active_wlanmac_lx for RF revision ID

	if ((REG32(0xb86400f0) & 0x0000f000) >= 0x00001000)
		Set_GPHYWB(999, 1, 16, 0xffff-(7<<4), 4<<4); // fix 100M IOL overshoot issue

	REG32(0xb80000dc) = val; // restore value.
	}
#endif

	//patch end
	for(i=0; i<5; i++)
		REG32(PCRP0+i*4) &= ~(EnForceMode);
}
#endif


#define REG32_ANDOR(x,y,z)   (REG32(x)=(REG32(x)& (y))|(z))

#if defined(CONFIG_8211F_SUPPORT)	
extern __inline__ void __udelay(unsigned long usecs, unsigned long lps)
{
	unsigned long lo;

	lo = ((usecs * 0x000010c6) >> 12) * (lps >> 20);
	__delay(lo);
}
extern unsigned long loops_per_jiffy;

#define __mdelay(x) { int i=x; while(i--) __udelay(1000, loops_per_jiffy * 100); }
#define GPIOG1				17

int init_p0(void)
{
	unsigned int data;

	REG32(PCRP0) |=  (0x5 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;
	
	REG32(MACCR) |= (1<<12);
	REG32_ANDOR(P0GMIICR, ~((1<<4)|(7<<0)) , (1<<4) | (3<<0) );	
	REG32(P0GMIICR) |=(Conf_done);
	REG32(PITCR) |= (1<<0);

	// 0xb8000040 bit [4:2] Configure P0 mdc/mdio PAD as P0-MDIO (3'b100)
	// 0xb8000040 bit [9:7] Configure P0 PAD as P0-MII (3'b000)
	REG32(PIN_MUX_SEL) = (REG32(PIN_MUX_SEL) & ~((7<<7) | (7<<2))) | (4<<2);

	// Reset 8211F
	REG32(PIN_MUX_SEL) |= (3<<10);
	REG32(PEFGHCNR) &= ~(1<<GPIOG1);
	REG32(PEFGHDIR) |= (1<<GPIOG1);
	REG32(PEFGHDAT) &= ~(1<<GPIOG1);
	__mdelay(500);
	REG32(PEFGHDAT) |= (1<<GPIOG1);
	__mdelay(300);

	REG32(PITCR) |= (1<<0);
	REG32(P0GMIICR) |=(Conf_done);

	rtl8651_getAsicEthernetPHYReg(5, 4, &data );
	// Advertise support of asymmetric pause 
	// Advertise support of pause frames
	rtl8651_setAsicEthernetPHYReg(5, 4, (data | 0xC00));

	//printf(" --> config port 0 done.\n");
	return 0;
}
#endif

int32 swCore_init()
{
	int port;
	unsigned int P0phymode;
	unsigned int P0miimode;
	unsigned int P0txdly;
	unsigned int P0rxdly;	



	/* Full reset and semreset */
	FullAndSemiReset();



#ifdef CONFIG_RTL8881A	
	//dprintf("\n---Setting_RTL8881A_Switch PHY\n"	);
	Setting_RTL8881A_PHY();
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

#if defined(CONFIG_SW_8367R)&&(!defined(CONFIG_LAN_WAN_ISOLATION))
	{	
	int i, ret = -1;
	#define REG_IOCFG_GPIO		0x00000018
	
	/* define GPIO port */
	enum GPIO_PORT
	{
	GPIO_PORT_A = 0,
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_D,
	GPIO_PORT_E,
	GPIO_PORT_F,
	GPIO_PORT_G,
	GPIO_PORT_H,
	GPIO_PORT_I,
	GPIO_PORT_MAX,
	};

	extern unsigned long loops_per_jiffy;
	#define __mdelay(x) { int i=x; while(i--) __udelay(1000, loops_per_jiffy * 100); }

	REG32(0xb8000044) |=  (3<<12); // reg_iocfg_fcs1, set to GPIO mode
	REG32(PABCDCNR) &= (~(0x00004000)); //set GPIO pin, A1
	REG32(PABCDDIR) |= (0x00004000); //output pin

	for (i=0; i<3; i++) 
	{
#if 1
	// for 8367r h/w reset pin
		{
		REG32(PABCDDAT) &= (~(0x00004000)); 
		__mdelay(1000);
		REG32(PABCDDAT) |= (0x00004000); 
		__mdelay(1000);
		}
#endif	
		// set to GPIO mode
		#ifndef CONFIG_NAND_FLASH
		REG32(PIN_MUX_SEL) |= ((REG32(PIN_MUX_SEL)&(~(7<<2))|0x3<<2));

		// MDC: F5, MDIO: F6
		WRITE_MEM32(PEFGHCNR, READ_MEM32(PEFGHCNR) & (~(0x00006000)));	//set GPIO pin, F5 and F6
		WRITE_MEM32(PEFGHDIR, READ_MEM32(PEFGHDIR) | ((0x00006000))); //output pin

		//smi_init(GPIO_PORT_A, 3, 2);
		smi_init(GPIO_PORT_F, 5, 6);
		#else
//		REG32(PIN_MUX_SEL) = (REG32(PIN_MUX_SEL)&(~(7<<2))) | (0x3<<2);
		REG32(PIN_MUX_SEL2) = (REG32(PIN_MUX_SEL2)&(~(0xe38))) | (0x618); // bit5~3=0b011, bit11~9=0b011

		// MDC: F5, MDIO: F6
//		WRITE_MEM32(PEFGHCNR, READ_MEM32(PEFGHCNR) & (~(0x00006000)));	//set GPIO pin, F5 and F6
//		WRITE_MEM32(PEFGHDIR, READ_MEM32(PEFGHDIR) | ((0x00006000))); //output pin

		// MDC: B5, MDIO: B3
		WRITE_MEM32(PABCDCNR, READ_MEM32(PABCDCNR) & (~(0x00002800)));	//set GPIO pin, B3 and B5
		WRITE_MEM32(PABCDDIR, READ_MEM32(PABCDDIR) | ((0x00002800))); //output pin

//		prom_printf(" =222=> REG32(PIN_MUX_SEL2)= %x\n",REG32(PIN_MUX_SEL2));

		//smi_init(GPIO_PORT_A, 3, 2);
//		smi_init(GPIO_PORT_F, 5, 6);
		smi_init(GPIO_PORT_B, 5, 3);
		
		
		#endif
		
		
		__mdelay(1000);
		ret = RTL8367R_init();
		if (ret == 0) 
			break;
	}
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
#ifndef CONFIG_RTL8881A
		REG32(PIN_MUX_SEL2)= 0;
#endif
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

	#if defined (CONFIG_SW_8367R)
		P0phymode=3;
		P0miimode=3;	
	#else //if 1//for RTL8881A
		P0phymode=1;
		P0miimode=0;
	#endif

	#if defined(CONFIG_8211F_SUPPORT)	
	P0phymode=0;
	#endif
	//printf("P0phymode=%02x, %s phy\n", P0phymode,   (P0phymode==0) ? "external" : "embedded");

	#if (defined (CONFIG_NAND_FLASH_BOOTING) ||defined (CONFIG_NAND_FLASH))	
		//dprintf("\n\n=========Switch for NAND Flash mode setting========= \n");		
		//dprintf("swcore.c:Set PIN_MUX to NAND Flash pin (No/Disable P0 GMII) ");
		//dprintf("\nswcore.c:P5 Config as NAND Flash mode .");
		//dprintf("\n========================================= \n");
		#define PINMUX_REG_8196D_97D   0xb8000040
		#if 0
		REG32(PINMUX_REG_8196D_97D) &=0xFFFFCCFF;  
		REG32(PINMUX_REG_8196D_97D)|=0x1108;
		#endif
		
		REG32(PINMUX_REG_8196D_97D) &= 0xFFFFCC63;
		//REG32(PINMUX_REG_8196D_97D) |= 0x108C;
		REG32(PINMUX_REG_8196D_97D) |= 0x1084;

		printf("====swcore:%x====\n",REG32(PINMUX_REG_8196D_97D));
		/*Set to embedded 10/100*/
		//return ;
		
	#endif


	if(P0phymode==1)  //embedded phy
	{
#ifdef CONFIG_FPGA_ROMCODE	
		REG32_ANDOR(0xbb804050, ~(0x1f<<0), 0x11<<0); 
		REG32(PCRP0) |=  (0<< ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;	//emabedded
#else
		REG32(PCRP0) |=  (0 << ExtPHYID_OFFSET) | EnablePHYIf | MacSwReset;	//emabedded
#endif
	}
	else //external phy
	{
		#if defined(CONFIG_8211F_SUPPORT)	
		init_p0();	
		#else
		REG32(PCRP0) |=  (0x06 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;	//external
		{
		int reg;

		// enable flow control ability
		rtl8651_getAsicEthernetPHYReg(0x06, 4, &reg );
		reg |= (BIT(10) | BIT(11));
		rtl8651_setAsicEthernetPHYReg( 0x06, 4, reg );
		}

		if((P0miimode==2)  ||(P0miimode==3))
		{	
			REG32(MACCR) |= (1<<12);   //giga link
		}

#ifdef CONFIG_SW_8367R
		// P0txdly = 1; P0rxdly = 5
		REG32_ANDOR(P0GMIICR, ~((1<<4)|(3<<0)) , (1<<4) | (5<<0) );			
		// force mode
		REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );
#else
		//unsigned int P0miimode=1;
		const unsigned char *miimodename[]={ "MII-PHY", "MII-MAC", "GMII-MAC", "RGMII" };
		printf("P0miimode=%02x, %s\n", P0miimode, miimodename[P0miimode] );
		
		if(P0miimode==0)       		REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_PHY<<23); 
		else if(P0miimode==1)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_MAC<<23);  
		else if(P0miimode==2)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_MAC<<23);  //GMII
		else if(P0miimode==3)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_RGMII<<23);   
		
		 if(P0miimode==3) 
		 {
			P0txdly=1;//Get_P0_TxDelay();
			P0rxdly=3;//Get_P0_RxDelay();	
			REG32_ANDOR(P0GMIICR, ~((1<<4)|(3<<0)) , (P0txdly<<4) | (P0rxdly<<0) );			
		}
#endif

#if 0
		if(P0miimode==0)       	 	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ;
#ifdef CONFIG_FPGA_ROMCODE	
		else if(P0miimode==1)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed10M |ForceDuplex) ; 
#else
		else if(P0miimode==1)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ; 
#endif	
		else if(P0miimode==2)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );
		else if(P0miimode==3)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );
#endif		
		
		REG32(PITCR) |= (1<<0);   //00: embedded , 01L GMII/MII/RGMII
		REG32(P0GMIICR) |=(Conf_done);
		#endif
	}   
#endif

	#endif

#ifdef CONFIG_RTL8196E
	if ((REG32(BOND_OPTION) & BOND_ID_MASK) == BOND_8196ES) {
		P0phymode=Get_P0_PhyMode();

		if(P0phymode==1)  //embedded phy
		{
			REG32(PCRP0) |=  (0 << ExtPHYID_OFFSET) | EnablePHYIf | MacSwReset;	//emabedded
		}
		else //external phy
		{
			REG32(PCRP0) |=  (0x06 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;	//external

			P0miimode=Get_P0_MiiMode();

			if(P0miimode==0)       		
				REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_PHY<<23); 
			else if ((P0miimode==1) || (P0miimode==2) )
				REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_MAC<<23);  
			else if(P0miimode==3)     	
				REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_RGMII<<23);   
		
			if(P0miimode==3) 
			{
				P0txdly=Get_P0_TxDelay();
				P0rxdly=Get_P0_RxDelay();	
				REG32_ANDOR(P0GMIICR, ~((1<<4)|(3<<0)) , (P0txdly<<4) | (P0rxdly<<0) );			
			}

			if ((P0miimode==0) || (P0miimode==1))
				REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ;
			else if ((P0miimode==2) || (P0miimode==3))
			{
				REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );
				REG32(MACCR) |= (1<<12);   //giga link
			}
			REG32(PITCR) |= (1<<0);   //00: embedded , 01L GMII/MII/RGMII
			REG32(P0GMIICR) |=(Conf_done);
		}   		
	}
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
#if defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E)||defined(CONFIG_RTL8881A)
	/*
		#LED = direct mode
		set mode 0x0
		swwb 0xbb804300 21-20 0x2 19-18 $mode 17-16 $mode 15-14 $mode 13-12 $mode 11-10 $mode 9-8 $mode
	*/
#if (defined (CONFIG_NAND_FLASH)||defined(CONFIG_NAND_FLASH_BOOTING))
	//REG32(PINMUX2)|= (1<<9); //Set led_s3 to Reset_button
	//REG32(PIN_MUX_SEL) = (REG32(PIN_MUX_SEL) | ((3<<2))); 
#else
#ifdef CONFIG_RTL8881A
#ifdef CONFIG_SW_8367R
	REG32(PIN_MUX_SEL) = (REG32(PIN_MUX_SEL) &~((7<<7)|(7<<2)|(3<<10)))|((3<<2));
#elif defined(CONFIG_8211F_SUPPORT)
	REG32(PIN_MUX_SEL)= (REG32(PIN_MUX_SEL) & ~( (7<<2) | (7<<7) | (1<<15) )) | (4<<2);  //let P0 to mii mode
#else	
	REG32(PIN_MUX_SEL)= (REG32(PIN_MUX_SEL) & ~( (7<<2) | (7<<7) | (3<<10) |  (1<<15) )) | (4<<2);  //let P0 to mii mode
#endif	
	REG32(PIN_MUX_SEL2)&=~ ((7<<0) | (7<<3) | (7<<6) | (7<<9) | (3<<12)   );  //S0-S3, P0

#else	
	REG32(PIN_MUX_SEL)&=~( (3<<8) | (3<<10) | (3<<3) | (1<<15) );  //let P0 to mii mode
	REG32(PIN_MUX_SEL2)&=~ ((3<<0) | (3<<3) | (3<<6) | (3<<9) | (3<<12) | (7<<15) );  //S0-S3, P0-P1
#endif
#endif
	REG32(LEDCR)  = (2<<20) | (0<<18) | (0<<16) | (0<<14) | (0<<12) | (0<<10) | (0<<8);  //P0-P5
#endif

#if (defined (CONFIG_NAND_FLASH_BOOTING) ||defined (CONFIG_NAND_FLASH))	
		goto NAND_SwitchInit_jump;
#endif
	
	/*PHY FlowControl. Default enable*/
	for(port=0;port<MAX_PORT_NUMBER;port++)
	{
		/* Set Flow Control capability. */

#if  defined(RTL8198)
#if 0//FPGA
		if(port ==0)
					rtl8651_restartAsicEthernetPHYNway(port+1, 0x11);
#endif
            rtl8651_restartAsicEthernetPHYNway(port+1, port);
        #else
		if (port == MAX_PORT_NUMBER-1)
			rtl8651_setAsicFlowControlRegister(RTL8651_MII_PORTNUMBER, TRUE, GIGA_PHY_ID);
		else				
			rtl8651_restartAsicEthernetPHYNway(port+1, port);	
#endif			
			
	}
	
NAND_SwitchInit_jump: 
	
#if ! (defined( CONFIG_NFBI) || defined(CONFIG_NONE_FLASH))

	{		
		extern char eth0_mac[6];
		rtl8651_setAsicL2Table((ether_addr_t*)(&eth0_mac), 0);
#ifdef CONFIG_HTTP_SERVER
		{
		extern char eth0_mac_httpd[6];
		rtl8651_setAsicL2Table((ether_addr_t*)(&eth0_mac_httpd), 1);
		}
#endif
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

#if defined (CONFIG_RTL8881A_FT1_TEST)
int32 swCore_init_ft()
{

	dprintf("\nFT2 Switch core initialization\n");    
	#define REG32_ANDOR(x,y,z)   (REG32(x)=(REG32(x)& (y))|(z))

	int port;

	/* Full reset and semreset */
	FullAndSemiReset();


#ifdef CONFIG_RTL8881A	
	dprintf("\n---Setting_RTL8881A_Switch PHY\n"	);
	Setting_RTL8881A_PHY();
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
#if defined ( CONFIG_RTL8196D) || defined ( CONFIG_RTL8196E) ||defined(CONFIG_RTL8881A)
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


#else  //8198, not NFBI
		//anson add
		REG32(0xb8000044)= 0;
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

#if defined ( CONFIG_RTL8196D) || defined ( CONFIG_RTL8196E) ||defined(CONFIG_RTL8881A)
	unsigned int rtl96d_P0phymode;
	unsigned int rtl96d_P0miimode;
	rtl96d_P0phymode=Get_P0_PhyMode();
	//unsigned int rtl96d_P0phymode=1;

	
   rtl96d_P0phymode=3;
      rtl96d_P0miimode=3;
	
	
	#ifdef CONFIG_P0_GMII_EQC_8211BN //for 8196DS EQC 100Mbps MII Loopback test	
		if(READ_MEM32(Bond_Option_REG)==13)  //RTL8196DS-VA-CG , QFN88-MII ,RGMII	
		{
			rtl96d_P0phymode=3;
                     rtl96d_P0miimode=3;
		}

		if(READ_MEM32(Bond_Option_REG)==8)  //RTL8197DS-VA-CG , QFN176-MII ,GMII	
		{
			rtl96d_P0phymode=1;
                     rtl96d_P0miimode=1;
		}

		 //RTL8972-VA-CG , QFN176-MII ,GMII  //RTL8982-VA-CG , QFN176-MII ,GMII
		if((READ_MEM32(Bond_Option_REG)==0)||(READ_MEM32(Bond_Option_REG)==1) 	)
		{
			rtl96d_P0phymode=1;
                     rtl96d_P0miimode=1;
		}
	#endif

	#ifdef CONFIG_RTL8196D_FT2_TEST //1F 
		rtl96d_P0phymode=0; //external Port0
	#endif


	#ifdef CONFIG_RTL8196D_FT1_TEST //DS
		rtl96d_P0phymode=0; //external Port0

	if((READ_MEM32(Bond_Option_REG)==15)||(READ_MEM32(Bond_Option_REG)==14))  //JSW:20120712 for RTL8196D/E 128pin
	{
			rtl96d_P0phymode=1;
	}	

	if(READ_MEM32(Bond_Option_REG)==12)  //JSW:20120224 for RTL8196EU QFN88 UTP
	{
			rtl96d_P0phymode=1;
	}	
	#endif
	
	
    
	printf("P0phymode=%02x, %s phy\n", rtl96d_P0phymode,   (rtl96d_P0phymode==0) ? "external" : "embedded");


	if(rtl96d_P0phymode==1)  //embedded phy
	{
#ifdef CONFIG_FPGA_ROMCODE	
		REG32_ANDOR(0xbb804050, ~(0x1f<<0), 0x11<<0); 
		REG32(PCRP0) |=  (0<< ExtPHYID_OFFSET) |  EnablePHYIf | MacSwReset;	//emabedded
#else
		REG32(PCRP0) |=  (0 << ExtPHYID_OFFSET) | EnablePHYIf | MacSwReset;	//emabedded
#endif
	}
	else //external phy
	{
		REG32(PCRP0) |=  (0x6 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;	//external

			
		unsigned int rtl96d_P0miimode=Get_P0_MiiMode();

		
	
		if(READ_MEM32(Bond_Option_REG)==13)  //RTL8196DS-VA-CG , QFN88-MII ,RGMII
		//if(READ_MEM32(Bond_Option_REG)==8)  //GMII
		{
			rtl96d_P0miimode=3;
		}	
		else//819xD ID= 0, 1 ,8 ,9 ,test GMII mode
		{
			rtl96d_P0miimode=2;
		}

		if((READ_MEM32(Bond_Option_REG)==15)||(READ_MEM32(Bond_Option_REG)==14))  //JSW:20120712 for RTL8196D/E 128pin
		{
			rtl96d_P0miimode=0;
		}	

		if(READ_MEM32(Bond_Option_REG)==12)  //JSW:20120224 for RTL8196EU QFN88 UTP
		{
			rtl96d_P0miimode=0;
		}	
		

		#ifdef CONFIG_RTL8196D_FT2_TEST //1F 
			rtl96d_P0miimode=0;
		#endif


		rtl96d_P0miimode=3;

                if((rtl96d_P0miimode==2)  ||(rtl96d_P0miimode==3))
		{	
			REG32(0xbb804000) |= (1<<12);   //giga link
		}		
		
		//unsigned int rtl96d_P0miimode=1;
		const unsigned char *miimodename[]={ "MII-PHY", "MII-MAC", "GMII-MAC", "RGMII" };
		printf("P0miimode=%02x, %s\n", rtl96d_P0miimode, miimodename[rtl96d_P0miimode] );
		
		if(rtl96d_P0miimode==0)       		REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_PHY<<23); 
		else if(rtl96d_P0miimode==1)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_MAC<<23);  
		else if(rtl96d_P0miimode==2)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_MAC<<23);  //GMII
		else if(rtl96d_P0miimode==3)     	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_RGMII<<23);   
		
		 if(rtl96d_P0miimode==3) 
		 {
			unsigned int rtl96d_P0txdly=1;//Get_P0_TxDelay();
//			unsigned int rtl96d_P0rxdly=3;//Get_P0_RxDelay();	
			unsigned int rtl96d_P0rxdly=5;//Get_P0_RxDelay();	
			REG32_ANDOR(P0GMIICR, ~((1<<4)|(3<<0)) , (rtl96d_P0txdly<<4) | (rtl96d_P0rxdly<<0) );	
			REG32(P0GMIICR) =0x37d55; //OK with 8211BN 8211DG 8211E RGMII
		}


		if(rtl96d_P0miimode==0)       	 	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ;		
#ifdef CONFIG_FPGA_ROMCODE	
		else if(rtl96d_P0miimode==1)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed10M |ForceDuplex) ; 
#else
		else if(rtl96d_P0miimode==1)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ; 
#endif	
		else if(rtl96d_P0miimode==2)     	 REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );
		else if(rtl96d_P0miimode==3)     	 
		{
			#if 1//def CONFIG_P0_GMII_EQC_8211BN //for 8196DS EQC 100Mbps MII Loopback test	
				printf("JSW EQC: for RTL8196DS-VA-CG , QFN88-MII , test 100Mbps\n");
				REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex );//for 8196DS EQC 100Mbps MII Loopback test	
			#else
				REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );
			#endif
		}

		REG32(P0GMIICR) |=(Conf_done);

		
		REG32(PITCR) |= (1<<0);   //00: embedded , 01L GMII/MII/RGMII

	

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
#if defined ( CONFIG_RTL8196D) || defined ( CONFIG_RTL8196E)
	/*
		#LED = direct mode
		set mode 0x0
		swwb 0xbb804300 21-20 0x2 19-18 $mode 17-16 $mode 15-14 $mode 13-12 $mode 11-10 $mode 9-8 $mode
	*/	
	#define PINMUX   0xb8000040
	#define PINMUX2 0xb8000044
	#ifndef CONFIG_NAND_FLASH	
		REG32(PINMUX)&=~( (3<<8) | (3<<10) | (3<<3) | (1<<15) );  //let P0 to mii mode	
		REG32(PINMUX2)&=~ ((3<<0) | (3<<3) | (3<<6) | (3<<9) | (3<<12) | (7<<15) );  //S0-S3, P0-P1
	#endif

	REG32(LEDCR)  = (2<<20) | (0<<18) | (0<<16) | (0<<14) | (0<<12) | (0<<10) | (0<<8);  //P0-P5
#endif

	
	/*PHY FlowControl. Default enable*/
	for(port=0;port<MAX_PORT_NUMBER;port++)
	{
		/* Set Flow Control capability. */

#if  defined(RTL8198)
#if 0//FPGA
		if(port ==0)
					rtl8651_restartAsicEthernetPHYNway(port+1, 0x11);
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

#endif



