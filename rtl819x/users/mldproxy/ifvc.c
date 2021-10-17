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
**  $Id: ifvc.c,v 1.1.1.1 2007-08-06 10:04:43 root Exp $	
**
**  This module manages an interface vector of the machine
**
*/

#include "mclab.h"
#include <linux/sockios.h>

struct IfDesc IfDescVc[ MAX_IF ], *IfDescEp = IfDescVc;

#ifdef CONFIG_IGMPPROXY_MULTIWAN
#define  MAXWAN 8
#else 
#define  MAXWAN 1
#endif

extern char mld_down_if_name[IFNAMSIZ];

#ifdef CONFIG_MLDPROXY_MULTIWAN
extern char mld_up_if_name[MAXWAN][IFNAMSIZ];
#else
extern char mld_up_if_name[IFNAMSIZ];
#endif



int buildIfVc()
/*
** Builds up a vector with the interface of the machine. Calls to the other functions of 
** the module will fail if they are called before the vector is build.
**          
*/
{
	struct ifreq IfVc[ sizeof( IfDescVc ) / sizeof( IfDescVc[ 0 ] )  ];
	struct ifreq *IfEp;

	int Sock=0;
	int Sock2;
	//printf("[%s]:[%d]\n",__FUNCTION__,__LINE__);
	memset(IfDescVc, 0, sizeof( IfDescVc ) );
	IfDescEp = IfDescVc;
	
	if( (Sock = socket( AF_INET6, SOCK_DGRAM, 0 )) < 0 ){
		//printf("[%s]:[%d]Sock=%d\n",__FUNCTION__,__LINE__,Sock);
		log( LOG_ERR, errno, "RAW socket open" );
	}
	
	//config vifs from kernel
	struct ifreq  *IfPt;
	struct IfDesc *IfDp;
	struct IfDesc *IfDp_tmp;
	IfPt = IfVc;
	{
		register struct uvif *v;
		int i;
		struct sockaddr_in6 addr;
		struct in6_addr mask;
		FILE			*file;
		unsigned int		prefixlen, scope, flags;
		char			devname[IFNAMSIZ];
		unsigned int		ifindex = 0;
		file = fopen("/proc/net/if_inet6", "r");

		/* We can live without it though */
		if (!file)
		{
			printf("error!Couldn't open /proc/net/if_inet6\n\n");
			//log_msg(LOG_ERR, errno, "Couldn't open /proc/net/if_inet6\n");
			return 0;
		}
		
		/*
		 * Loop through all of the interfaces.
		 */
		while ((fscanf( file,
				"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx %x %x %x %x %8s",
				&addr.sin6_addr.s6_addr[ 0], &addr.sin6_addr.s6_addr[ 1], &addr.sin6_addr.s6_addr[ 2], &addr.sin6_addr.s6_addr[ 3],
				&addr.sin6_addr.s6_addr[ 4], &addr.sin6_addr.s6_addr[ 5], &addr.sin6_addr.s6_addr[ 6], &addr.sin6_addr.s6_addr[ 7],
				&addr.sin6_addr.s6_addr[ 8], &addr.sin6_addr.s6_addr[ 9], &addr.sin6_addr.s6_addr[10], &addr.sin6_addr.s6_addr[11],
				&addr.sin6_addr.s6_addr[12], &addr.sin6_addr.s6_addr[13], &addr.sin6_addr.s6_addr[14], &addr.sin6_addr.s6_addr[15],			
				&ifindex, &prefixlen, &scope, &flags, devname)!=EOF)){
			#if 0
			printf("ifa->name:%s\n",devname);
			printf( "ifindex:%x,prefixlen:%x,scope:%x,flags:%x\n",ifindex,prefixlen,scope,flags);
			printf( "addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
				addr.sin6_addr.s6_addr16[ 0],addr.sin6_addr.s6_addr16[ 1],addr.sin6_addr.s6_addr16[ 2],
				addr.sin6_addr.s6_addr16[ 3],addr.sin6_addr.s6_addr16[ 4],addr.sin6_addr.s6_addr16[ 5],
				addr.sin6_addr.s6_addr16[ 6],addr.sin6_addr.s6_addr16[ 7]);
			#endif
			if (strcmp(devname,"lo")==0)
					continue;
			if (strcmp(devname,"peth0")==0)
					continue;

			/* get If vector
			**  RFC 2710 mandates the use of a link-local IPv6 source address for the transmission of MLD message
			**  so we just need to record the link-local address
			*/
		    
		 	if (IS_IPV6_LINKLOCAL_ADDRESS(addr.sin6_addr.s6_addr32))
			{
				if(flags&IFA_F_TENTATIVE){
					printf("%s IFA_F_TENTATIVE is set!\n",devname);
					//if ((strcmp(devname,"eth1")==0)||(strcmp(devname,"br0")==0))
					if (strcmp(devname,mld_down_if_name)==0)
						return 0;
					#ifndef CONFIG_MLDPROXY_MULTIWAN
					if (strcmp(devname,mld_up_if_name)==0)
						return 0;
					#else
					int i=0;
					
					for(i=0;i<MAXWAN;i++){
						if (strcmp(devname,mld_up_if_name[i])==0)
							return 0;
					}	
						
					
					#endif
				}	
				char FmtBu[ 128 ];
				IfPt->ifr_addr.sa_family=AF_INET6;
				strncpy(IfPt->ifr_name,devname,IFNAMSIZ);
				IfPt->ifr_ifindex=ifindex;
				strncpy(IfDescEp->Name,devname,sizeof( IfDescEp->Name ));
				memcpy(&IfDescEp->InAdr,&addr.sin6_addr,sizeof(struct in6_addr));
				IfDescEp->Flags = flags;
				IfDescEp->pif_idx=ifindex;
				#if 0
				printf("buildIfVc: Interface %s pif_idx:%d, Flags: 0x%04x",
					   IfDescEp->Name,
					   IfDescEp->pif_idx,
					  IfDescEp->Flags );
				printf( "addr:%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
				IfDescEp->InAdr.s6_addr16[ 0],IfDescEp->InAdr.s6_addr16[ 1],IfDescEp->InAdr.s6_addr16[ 2],
				IfDescEp->InAdr.s6_addr16[ 3],IfDescEp->InAdr.s6_addr16[ 4],IfDescEp->InAdr.s6_addr16[ 5],
				IfDescEp->InAdr.s6_addr16[ 6],IfDescEp->InAdr.s6_addr16[ 7]);
				#endif
				IfDescEp++; 
				IfPt++;	
			}
			else
			{
				
			}
		}

		fclose(file);
	}

  	close( Sock );
	return 1;
}


struct IfDesc *getIfByName( const char *IfName )
/*
** Returns a pointer to the IfDesc of the interface 'IfName'
**
** returns: - pointer to the IfDesc of the requested interface
**          - NULL if no interface 'IfName' exists
**          
*/
{
	struct IfDesc *Dp=NULL;
	int i;
	for( i=0; i<MAX_IF; i++ ) {
		Dp=IfDescVc+i;
		if( ! strcmp( IfName, Dp->Name ) ) {
			//printf("Find dp:name:%s,vif:%d,[%s]:[%d].\n",Dp->Name,(Dp-IfDescVc),__FUNCTION__,__LINE__);
			return Dp;
		}
	}
		
	//printf("[%s]:[%d].failed!%s\n\n",__FUNCTION__,__LINE__,IfName);
	return NULL;
}
struct IfDesc *getIfByPix( int pix)
/*
** Returns a pointer to the IfDesc of the interface 'IfName'
**
** returns: - pointer to the IfDesc of the requested interface
**          - NULL if no interface 'Pix' exists
**          
*/
{
	struct IfDesc *Dp=NULL;
	int i;
	for( i=0; i<MAX_IF; i++ ) {
		Dp=IfDescVc+i;
		if( pix==((int)Dp->pif_idx))  {
			//printf("Find dp:name:%s,vif:%d,[%s]:[%d].\n",Dp->Name,(Dp-IfDescVc),__FUNCTION__,__LINE__);
			return Dp;
		}
	}
		
	//printf("[%s]:[%d].failed!%s\n\n",__FUNCTION__,__LINE__,IfName);
	return NULL;
}

struct IfDesc *getIfByIx( unsigned Ix )
/*
** Returns a pointer to the IfDesc of the interface 'Ix'
**
** returns: - pointer to the IfDesc of the requested interface
**          - NULL if no interface 'Ix' exists
**          
*/
{
	struct IfDesc *Dp = &IfDescVc[ Ix ];
	return Dp < IfDescEp ? Dp : NULL;
}



int chk_local( struct in6_addr addr )
/*
** Returns a pointer to the IfDesc of the interface 'IfName'
**
** returns: - pointer to the IfDesc of the requested interface
**          - NULL if no interface 'IfName' exists
**          
*/
{
	struct IfDesc *Dp;
	int i;
	for( Dp = IfDescVc; Dp < IfDescVc+MAX_IF; i++ ) 
  		if( !memcmp(&(Dp->InAdr),&(addr),sizeof(struct in6_addr) ))
  			return 1;

	return 0;
}
