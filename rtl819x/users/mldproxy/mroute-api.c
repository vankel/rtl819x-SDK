/*
**  smcroute - static multicast routing control 
**  Copyright (C) 2001 Carsten Schill <carsten@cschill.de>
**
**  This program is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 2 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software
**  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
**  $Id: mroute-api.c,v 1.1.1.1 2007-08-06 10:04:43 root Exp $	
**
**  This module contains the interface routines to the Linux mrouted API
**
*/

#include "mclab.h"
extern char mld_down_if_idx;
#define ALL_VIFS	((unsigned short)(-1))


// need an IGMP socket as interface for the mrouted API
// - receives the IGMP messages
int MLD_Socket=0;

// my internal virtual interfaces descriptor vector  
static struct VifDesc {
  struct IfDesc *IfDp;
} VifDescVc[ MAXVIFS ];

int enableMRouter()
/*
** Initialises the mrouted API and locks it by this exclusively.
**     
** returns: - 0 if the functions succeeds     
**          - the errno value for non-fatal failure condition
*/
{
	int Va = 1;
	int ret=0;
	if (MLD_Socket != 0) 
	{
		printf("already enabled!MLD_Socket :%d\n",MLD_Socket );
		return 0;
	}
	if( (MLD_Socket = socket( AF_INET6, SOCK_RAW, IPPROTO_ICMPV6)) < 0 ){
		printf("MLD_Socket :%d\n",MLD_Socket );
		log( LOG_ERR, errno, "ICMP socket open" );
	}
	if( ret=setsockopt( MLD_Socket, IPPROTO_IPV6, MRT6_INIT, 
		  (void *)&Va, sizeof( Va ) ) ) {
		printf("setsockopt MRT6_INIT fail!!!%d\n",ret);
		return errno;
	}
	//if(fcntl(MLD_Socket, F_SETFL, O_NONBLOCK) == -1)
	//return errno;

	//printf("Enable MRouter Success![%s]:[%d].\n",__FUNCTION__,__LINE__);
	return 0;
}

void disableMRouter()
/*
** Diables the mrouted API and relases by this the lock.
**          
*/
{
	if( setsockopt( MLD_Socket, IPPROTO_IPV6, MRT6_DONE, NULL, 0 ) 
	  || close( MLD_Socket )
	) {
		MLD_Socket = 0;
		printf("[%s]:[%d]",__FUNCTION__,__LINE__);
		log( LOG_ERR, errno, "MRT6_DONE/close" );
	}
	//printf("[%s]:[%d]",__FUNCTION__,__LINE__);
	MLD_Socket = 0;
}

int addVIF( struct IfDesc *IfDp )
/*
** Adds the interface '*IfDp' as virtual interface to the mrouted API
** 
*/
{
  //struct vifctl VifCtl;//
	struct VifDesc *VifDp;//
	struct mif6ctl VifCtl;


	/*check if IfDp has beed added*/
	for( VifDp = VifDescVc; VifDp < VCEP( VifDescVc ); VifDp++ ) {
		if( VifDp->IfDp  && strcmp(VifDp->IfDp->Name,IfDp->Name)==0){
			//printf("[%s]:[%d]\n",__FUNCTION__,__LINE__);
			return VifDp - VifDescVc; 
		}
	}
	/* search free VifDesc
	*/
	for( VifDp = VifDescVc; VifDp < VCEP( VifDescVc ); VifDp++ ) {
		if( ! VifDp->IfDp )
			break;
	}
    
	/* no more space
	*/
	if( VifDp >= VCEP( VifDescVc ) )
		log( LOG_ERR, ENOMEM, "addVIF, out of VIF space" );

	VifDp->IfDp = IfDp;

	VifCtl.mif6c_mifi  = VifDp - VifDescVc; 
	VifCtl.mif6c_flags =0;//VifDp->IfDp->Flags;        /* no tunnel, no source routing, register ? */
	VifCtl.vifc_threshold = 1;    /* Packet TTL must be at least 1 to pass them */
	VifCtl.vifc_rate_limit = 0;   /* hopefully no limit */
	VifCtl.mif6c_pifi  =VifDp->IfDp->pif_idx;
	//printf("[%s]:VifCtl.mif6c_mifi=%d,VifCtl.mif6c_pifi=%d\n",__FUNCTION__,VifCtl.mif6c_mifi,VifCtl.mif6c_pifi);

	if ((VifDp->IfDp->Flags & MIFF_REGISTER))
		log(LOG_DEBUG,0," register pifi : %d ", VifDp->IfDp->pif_idx);
	/*
	printf("adding VIF, idx=%d Fl flags=0x%x name=%s \n", 
	   VifCtl.mif6c_mifi, VifCtl.mif6c_flags, VifDp->IfDp->Name);
	*/
	if( setsockopt( MLD_Socket, IPPROTO_IPV6, MRT6_ADD_MIF, 
		  (char *)&VifCtl, sizeof( VifCtl ) )<0 )
		log( LOG_ERR, errno, "MRT6_ADD_VIF" );

	IfDp->vif_idx = VifCtl.mif6c_mifi;

	return VifCtl.mif6c_mifi;
}

int addMRoute( struct MRouteDesc *Dp )
/*
** Adds the multicast routed '*Dp' to the kernel routes
**
** returns: - 0 if the function succeeds
**          - the errno value for non-fatal failure condition
*/
{
	struct mf6cctl CtlReq;
	short vifi;
	memcpy(&(CtlReq.mf6cc_origin.sin6_addr),&(Dp->OriginAdr),sizeof (struct in6_addr));
	memcpy(&(CtlReq.mf6cc_mcastgrp.sin6_addr),&(Dp->McAdr),sizeof(struct in6_addr));
	CtlReq.mf6cc_parent	=Dp->InVif;
	/*
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	printf("Dp->OriginAdr is 0x%x-%x-%x-%x\n",
		Dp->OriginAdr.s6_addr32[0],Dp->OriginAdr.s6_addr32[1],Dp->OriginAdr.s6_addr32[2],Dp->OriginAdr.s6_addr32[3]);
	*/
	IF_ZERO(&CtlReq.mf6cc_ifset);
	//IF_CLR(CtlReq.mf6cc_parent, &CtlReq.mf6cc_ifset);
	for (vifi = 0; vifi < MAXVIFS; vifi++)
	{

		if((Dp->TtlVc[vifi])==1){
			//printf("VIFI=%d,[%s]:[%d].\n",vifi,__FUNCTION__,__LINE__);
			IF_SET(vifi, &CtlReq.mf6cc_ifset);		//set  the interface receive report
		}

	}

	
	IF_CLR(CtlReq.mf6cc_parent, &CtlReq.mf6cc_ifset);
	/*
	if(Dp->TtlVc[mld_down_if_idx]==0){
		
		IF_SET(mld_down_if_idx,&CtlReq.mf6cc_ifset);
	}
	*/
	//printf("CtlReq.mf6cc_parent=%d,[%s]:[%d].\n",CtlReq.mf6cc_parent,__FUNCTION__,__LINE__);
	if( setsockopt( MLD_Socket, IPPROTO_IPV6, MRT6_ADD_MFC,
		  (void *)&CtlReq, sizeof( CtlReq ) ) ){ 
		printf("MRT6_ADD_MFC!![%s]:[%d].\n",__FUNCTION__,__LINE__);
		log( LOG_WARNING, errno, "MRT6_ADD_MFC" );
	}
	return errno;
}

int delMRoute( struct MRouteDesc *Dp )
/*
** Removes the multicast routed '*Dp' from the kernel routes
**
** returns: - 0 if the function succeeds
**          - the errno value for non-fatal failure condition
*/
{
	struct mf6cctl CtlReq;
	short vifi;
	memcpy(&(CtlReq.mf6cc_origin.sin6_addr),&(Dp->OriginAdr),sizeof (struct in6_addr));
	memcpy(&(CtlReq.mf6cc_mcastgrp.sin6_addr),&(Dp->McAdr),sizeof(struct in6_addr));
	/*
	printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	printf("Dp->OriginAdr is 0x%x-%x-%x-%x\n",
			Dp->OriginAdr.s6_addr32[0],Dp->OriginAdr.s6_addr32[1],Dp->OriginAdr.s6_addr32[2],Dp->OriginAdr.s6_addr32[3]);
	*/
	if(Dp->InVif==-1)
		CtlReq.mf6cc_parent=ALL_VIFS;
	else	
		CtlReq.mf6cc_parent=Dp->InVif;
	
	IF_ZERO(&CtlReq.mf6cc_ifset);

	for (vifi = 0; vifi < MAXVIFS; vifi++)
	{

	    IF_CLR(vifi, &CtlReq.mf6cc_ifset);

	}
	//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
	if( setsockopt( MLD_Socket, IPPROTO_IPV6, MRT6_DEL_MFC,
		  (void *)&CtlReq, sizeof( CtlReq ) ) ){ 
		printf("MRT6_DEL_MFC!![%s]:[%d].\n",__FUNCTION__,__LINE__); 
		log( LOG_WARNING, errno, "MRT6_DEL_MFC" );
	}
}

int getVifIx( struct IfDesc *IfDp )
/*
** Returns for the virtual interface index for '*IfDp'
**
** returns: - the vitrual interface index if the interface is registered
**          - -1 if no virtual interface exists for the interface 
**          
*/
{
	struct VifDesc *Dp;

	for( Dp = VifDescVc; Dp < VCEP( VifDescVc ); Dp++ ) 
		if( Dp->IfDp == IfDp )
		  return Dp - VifDescVc;

	return -1;
}

unsigned long getAddrByVifIx(int ix)
{
	if(ix >= MAXVIFS)
		return 0;
	return VifDescVc[ix].IfDp->InAdr.s6_addr;
}

