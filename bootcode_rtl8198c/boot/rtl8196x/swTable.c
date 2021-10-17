/*
* ----------------------------------------------------------------
* Copyright c                  Realtek Semiconductor Corporation, 2002  
* All rights reserved.
* 
*
* Abstract: Switch core table access driver source code.
*	
*
* ---------------------------------------------------------------
*/

#include <rtl_types.h>
#include <rtl_errno.h>
#include <rtl8196x/asicregs.h>



/* LOCAL SUBPROGRAM SPECIFICATIONS
 */
RTL_STATIC_INLINE void tableAccessForeword(uint32, uint32, void *);

#ifdef CONFIG_RTL865XC
int32 swTable_addEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    tableAccessForeword(tableType, eidx, entryContent_P);
        
    /* Activate add command */
    REG32(SWTACR) = ACTION_START | CMD_ADD;
    
    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    /* Check status */
    if ( (REG32(SWTASR) & TABSTS_MASK) != TABSTS_SUCCESS )
        return ECOLLISION;
    else
        return 0;
}
#endif



#if 0//def CONFIG_RTL865XC
int32 swTable_modifyEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    tableAccessForeword(tableType, eidx, entryContent_P);
        
    /* Activate add command */
    REG32(SWTACR) = ACTION_START | CMD_MODIFY;
    
    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    /* Check status */
    if ( (REG32(SWTASR) & TABSTS_MASK) != TABSTS_SUCCESS )
        return EEMPTY;
    else
        return 0;
}
#endif



#if 0 //def CONFIG_RTL865XC
int32 swTable_forceAddEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    tableAccessForeword(tableType, eidx, entryContent_P);
        
    /* Activate add command */
    REG32(SWTACR) = ACTION_START | CMD_FORCE;
    
    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    /* Check status */
    if ( (REG32(SWTASR) & TABSTS_MASK) == TABSTS_SUCCESS )
        return 0;
        
    /* There might be something wrong */
    ASSERT_CSP( 0 );

}
#endif



#ifdef CONFIG_RTL865XC
int32 swTable_readEntry(uint32 tableType, uint32 eidx, void *entryContent_P)
{
    uint32 *    entryAddr;

    REG32(SWTCR0) = REG32(SWTCR0) | EN_STOP_TLU;
    while ((REG32(SWTCR0) & STOP_TLU_READY) == 0);

    ASSERT_CSP(entryContent_P);
    
    entryAddr = (uint32 *) (table_access_addr_base(tableType) + eidx * TABLE_ENTRY_DISTANCE);
    
    /* Wait for command ready */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );
    
    /* Read registers according to entry width of each table */
    *((uint32 *)entryContent_P + 7) = *(entryAddr + 7);
    *((uint32 *)entryContent_P + 6) = *(entryAddr + 6);
    *((uint32 *)entryContent_P + 5) = *(entryAddr + 5);
    *((uint32 *)entryContent_P + 4) = *(entryAddr + 4);
    *((uint32 *)entryContent_P + 3) = *(entryAddr + 3);
    *((uint32 *)entryContent_P + 2) = *(entryAddr + 2);
    *((uint32 *)entryContent_P + 1) = *(entryAddr + 1);
    *((uint32 *)entryContent_P + 0) = *(entryAddr + 0);

    REG32(SWTCR0) = REG32(SWTCR0) & ~EN_STOP_TLU;

    return 0;
}
#endif



//RTL_STATIC_INLINE void tableAccessForeword(uint32 tableType, uint32 eidx,     void *entryContent_P)
void tableAccessForeword(uint32 tableType, uint32 eidx,     void *entryContent_P)
{
    ASSERT_CSP(entryContent_P);

    /* Wait for command done */
    while ( (REG32(SWTACR) & ACTION_MASK) != ACTION_DONE );
    
    /* Write registers according to entry width of each table */
    REG32(TCR7) = *((uint32 *)entryContent_P + 7);
    REG32(TCR6) = *((uint32 *)entryContent_P + 6);
    REG32(TCR5) = *((uint32 *)entryContent_P + 5);
    REG32(TCR4) = *((uint32 *)entryContent_P + 4);
    REG32(TCR3) = *((uint32 *)entryContent_P + 3);
    REG32(TCR2) = *((uint32 *)entryContent_P + 2);
    REG32(TCR1) = *((uint32 *)entryContent_P + 1);
    REG32(TCR0) = *(uint32 *)entryContent_P;
    
    /* Fill address */
    REG32(SWTAA) = table_access_addr_base(tableType) + eidx * TABLE_ENTRY_DISTANCE;
}

//#define HW_TABLE_DEBUG		1

#ifdef HW_TABLE_DEBUG

#include <linux/autoconf2.h>

int rtl8651_totalExtPortNum = 3;
#define rtlglue_printf    dprintf
#define RTL865XC_VLANTBL_SIZE				4096

#define RTL8651_MAC_NUMBER				5
#define RTL8651_PORT_NUMBER				RTL8651_MAC_NUMBER

typedef struct rtl865x_tblAsicDrv_vlanParam_s {
	uint32 	memberPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
	uint32 	untagPortMask; /*extension ports [rtl8651_totalExtPortNum-1:0] are located at bits [RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum-1:RTL8651_PORT_NUMBER]*/
	uint32  fid:2;
} rtl865x_tblAsicDrv_vlanParam_t;


typedef struct rtl865x_tblAsicDrv_intfParam_s {
	ether_addr_t macAddr;
	uint16 	macAddrNumber;
	uint16 	vid;
	uint32 	inAclStart, inAclEnd, outAclStart, outAclEnd;
	uint32 	mtu;
#if defined(CONFIG_RTL_8198C)
	uint32 	mtuv6;
#endif    
	uint32 	enableRoute:1;
	uint32 	valid:1;
#if defined(CONFIG_RTL_8198C)
	uint32 	enableRouteV6:1;
#endif    

} rtl865x_tblAsicDrv_intfParam_t;


typedef struct {

	 /* word 0 */
	uint32	reserved1:12;

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





int32 rtl8651_getAsicVlan(uint16 vid, rtl865x_tblAsicDrv_vlanParam_t *vlanp) {	
	rtl865xc_tblAsic_vlanTable_t entry;
	if(vlanp == NULL||vid>=4096)
		return FAILED;        

	swTable_readEntry(TYPE_VLAN_TABLE, vid, &entry);	
	if((entry.extMemberPort | entry.memberPort) == 0)
	{
		return FAILED;
	}
	vlanp->memberPortMask = (entry.extMemberPort<<RTL8651_PORT_NUMBER) | entry.memberPort;
	vlanp->untagPortMask = (entry.extEgressUntag<<RTL8651_PORT_NUMBER) |entry.egressUntag;
	vlanp->fid=entry.fid;

	
	return SUCCESS;
}

void dump_vlan(void)
{
	int i,j;
	rtl8651_totalExtPortNum=3;
	rtlglue_printf(">>ASIC VLAN Table:\n\n");
		for(i=0; i<RTL865XC_VLANTBL_SIZE; i++) {
			rtl865x_tblAsicDrv_vlanParam_t vlan;

			if (rtl8651_getAsicVlan(i,&vlan ) == FAILED)
				continue;

			rtlglue_printf("  VID[%d] ", i);			

			rtlglue_printf("\n\tmember ports:");				
			for(j=0; j<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; j++)
				if(vlan.memberPortMask & (1<<j))
						rtlglue_printf("%d ", j);
			rtlglue_printf("\n\tUntag member ports:");				
				for(j=0; j<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; j++)
					if(vlan.untagPortMask & (1<<j))
						rtlglue_printf("%d ", j);
					

			rtlglue_printf("\n\tFID:\t%d\n",vlan.fid);

		}

}

#define RTL8651_ACLTBL_ALL_TO_CPU			127  // This rule is always "To CPU"
#define RTL8651_ACLTBL_DROP_ALL				126 //This rule is always "Drop"
#define RTL8651_ACLTBL_PERMIT_ALL			125	// This rule is always "Permit"
#define RTL8651_ACLTBL_PPPOEPASSTHRU		124 //For PPPoE Passthru Only
#define RTL8651_ACLTBL_RESERV_SIZE			4	//this many of ACL rules are reserved for internal use

#define RTL865XC_NETIFTBL_SIZE			8
#define STP_PortST_MASK					(3<<4)		/* Mask Spanning Tree Protocol Port State Control */
#define STP_PortST_OFFSET					(4)			/* Offset */


typedef struct {
    /* word 0 */
    uint32          mac18_0:19;
    uint32          vid		 : 12;
    uint32          valid       : 1;	

#if defined(CONFIG_RTL_8198C)
    /* word 1 */
    uint32          inACLStartL:1;
    uint32          enHWRouteV6    : 1;	
    uint32         enHWRoute : 1;	
    uint32         mac47_19:29;

    /* word 2 */
    uint32         macMaskL    :1;
    uint32         outACLEnd   :8;
    uint32         outACLStart :8;
    uint32         inACLEnd    :8;	
    uint32         inACLStartH :7;	

    /* word 3 */
    uint32         mtuv6       :15;
    uint32         mtu         :15;
    uint32         macMaskH    :2;
#else	
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
#endif

    /* word 4 */
    uint32          reservw4;
    /* word 5 */
    uint32          reservw5;
    /* word 6 */
    uint32          reservw6;
    /* word 7 */
    uint32          reservw7;
} netif_table_t;


int32 rtl8651_getAsicNetInterface( uint32 idx, rtl865x_tblAsicDrv_intfParam_t *intfp )
{
	netif_table_t entry;
	uint32 i;
#if defined(CONFIG_RTL_8198C)
	uint32   macMask;
#endif

	if(intfp == NULL)
		return FAILED;

	intfp->valid=0;

	if ( idx == RTL865XC_NETIFTBL_SIZE )
	{
		/* idx is not specified, we search whole interface table. */
		for( i = 0; i < RTL865XC_NETIFTBL_SIZE; i++ )
		{

			swTable_readEntry(TYPE_NETINTERFACE_TABLE, i, &entry);
			if ( entry.valid && entry.vid==intfp->vid ){
				goto found;
			}
		}

		/* intfp.vid is not found. */
		return FAILED;
	}
	else
	{
		/* idx is specified, read from ASIC directly. */
		swTable_readEntry(TYPE_NETINTERFACE_TABLE, idx, &entry);
	}

found:
	intfp->valid=entry.valid;
	intfp->vid=entry.vid;
	intfp->macAddr.octet[0] = entry.mac47_19>>21;
	intfp->macAddr.octet[1] = (entry.mac47_19 >>13)&0xff;
	intfp->macAddr.octet[2] = (entry.mac47_19 >>5)&0xff;
	intfp->macAddr.octet[3] = ((entry.mac47_19 &0x3f) <<3) | (entry.mac18_0 >>16);
	intfp->macAddr.octet[4] = (entry.mac18_0 >> 8)&0xff;
	intfp->macAddr.octet[5] = entry.mac18_0 & 0xff;

#if defined(CONFIG_RTL_8198C)
	intfp->inAclEnd     = entry.inACLEnd;
	intfp->inAclStart   = (entry.inACLStartH<<1)|entry.inACLStartL;  
	intfp->outAclStart  = entry.outACLStart;
	intfp->outAclEnd    = entry.outACLEnd;
	intfp->enableRoute  = entry.enHWRoute==1? TRUE: FALSE;
	intfp->enableRouteV6= entry.enHWRouteV6==1? TRUE: FALSE;

	macMask = (entry.macMaskH<<1)|entry.macMaskL;
	switch(macMask)
	{
		case 0:
			intfp->macAddrNumber =8;
			break;
		case 6:
			intfp->macAddrNumber =2;
			break;
		case 4:
			intfp->macAddrNumber =4;
			break;
		case 7:
			intfp->macAddrNumber =1;
			break;			
	}  
	intfp->mtu = entry.mtu;
	intfp->mtuv6 = entry.mtuv6;
#else	
	intfp->inAclEnd = entry.inACLEnd;
	intfp->inAclStart= (entry.inACLStartH<<2)|entry.inACLStartL;
	intfp->outAclStart = entry.outACLStart;
	intfp->outAclEnd = entry.outACLEnd;
	intfp->enableRoute = entry.enHWRoute==1? TRUE: FALSE;

	switch(entry.macMask)
	{
		case 0:
			intfp->macAddrNumber =8;
			break;
		case 6:
			intfp->macAddrNumber =2;
			break;
		case 4:
			intfp->macAddrNumber =4;
			break;
		case 7:
			intfp->macAddrNumber =1;
			break;

			
	}
	intfp->mtu = (entry.mtuH <<3)|entry.mtuL;
#endif

	return SUCCESS;
}

void dump_netif(void)
{
		int8	*pst[] = { "DIS/BLK",  "LIS", "LRN", "FWD" };
		uint8	*mac;
		int32	i, j;

		rtlglue_printf(">>ASIC Netif Table:\n\n");
		for(i=0; i<RTL865XC_NETIFTBL_SIZE; i++) {
			rtl865x_tblAsicDrv_intfParam_t intf;
			rtl865x_tblAsicDrv_vlanParam_t vlan;

			if (rtl8651_getAsicNetInterface( i, &intf ) == FAILED)
				continue;

			if (intf.valid)
			{
				mac = (uint8 *)&intf.macAddr.octet[0];
				rtlglue_printf("%d-vid[%d] %02x:%02x:%02x:%02x:%02x:%02x", 
					i, intf.vid, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
				rtlglue_printf("  L3/4 HW acc %s \n",
					intf.enableRoute==TRUE? "enabled": "disabled" );
				rtlglue_printf("      ingress ");
				if(RTL8651_ACLTBL_DROP_ALL<=intf.inAclStart){
					if(intf.inAclStart==RTL8651_ACLTBL_PERMIT_ALL)
						rtlglue_printf("permit all,");
					if(intf.inAclStart==RTL8651_ACLTBL_ALL_TO_CPU)
						rtlglue_printf("all to cpu,");
					if(intf.inAclStart==RTL8651_ACLTBL_DROP_ALL)
						rtlglue_printf("drop all,");
				}else
					rtlglue_printf("ACL %d-%d, ",intf.inAclStart, intf.inAclEnd);
				rtlglue_printf("  egress ");
				if(RTL8651_ACLTBL_DROP_ALL<=intf.outAclStart){
					if(intf.outAclStart==RTL8651_ACLTBL_PERMIT_ALL)
						rtlglue_printf("permit all,");
					if(intf.outAclStart==RTL8651_ACLTBL_ALL_TO_CPU)
						rtlglue_printf("all to cpu,");
					if(intf.outAclStart==RTL8651_ACLTBL_DROP_ALL)
						rtlglue_printf("drop all,");
				}else
					rtlglue_printf("ACL %d-%d, ",intf.outAclStart, intf.outAclEnd);
				rtlglue_printf("\n      %d MAC Addresses, MTU %d Bytes\n", intf.macAddrNumber, intf.mtu);


				rtl8651_getAsicVlan( intf.vid, &vlan );
				rtlglue_printf("\n      Untag member ports:");
				for(j=0; j<RTL8651_PORT_NUMBER+3; j++)
					if(vlan.untagPortMask & (1<<j))
						rtlglue_printf("%d ", j);
				rtlglue_printf("\n      Active member ports:");
				for(j=0; j<RTL8651_PORT_NUMBER+3; j++)
					if(vlan.memberPortMask & (1<<j))
						rtlglue_printf("%d ", j);
				
				rtlglue_printf("      Port state(");
				for(j=0; j<RTL8651_PORT_NUMBER+rtl8651_totalExtPortNum; j++)
				{
					if((vlan.memberPortMask & (1<<j))==0)
						continue;
					if ((( REG32( PCRP0+j*4 )&STP_PortST_MASK)>>STP_PortST_OFFSET ) > 4 )
						rtlglue_printf("--- ");
					else
						rtlglue_printf("%d:%s ", j, pst[(( REG32( PCRP0+j*4 )&STP_PortST_MASK)>>STP_PortST_OFFSET )]);

				}
				rtlglue_printf(")\n\n");
			}

	}
}
#endif

