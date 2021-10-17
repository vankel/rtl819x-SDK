/*
 * static_leases.c -- Couple of functions to assist with storing and
 * retrieving data for static leases
 *
 * Wade Berrier <wberrier@myrealbox.com> September 2004
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include "static_leases.h"
#include "dhcpd.h"
#ifdef SUPPORT_DHCP_PORT_IP_BIND

//#define	RTL8651_IOCTL_GETPORTIDBYCLIENTMAC	2013

//#define LAN_IFNAME "eth0"

static int re865xIoctl(char *name, unsigned int arg0, unsigned int arg1, unsigned int arg2, unsigned int arg3)
{
	unsigned int args[4];
	struct ifreq ifr;
	int sockfd;

	unsigned short retval=0;
	
	args[0] = arg0;
	args[1] = arg1;
	args[2] = arg2;
	args[3] = arg3;

	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		perror("fatal error socket\n");
		return -3;
	}

	strcpy((char*)&ifr.ifr_name, name);
	((unsigned int *)(&ifr.ifr_data))[0] = (unsigned int *)args;

	if (ioctl(sockfd, SIOCDEVPRIVATE, &ifr)<0)
	{
		perror("device ioctl:");
		close(sockfd);
		return -1;
	}
	close(sockfd);
	memcpy(&retval, ifr.ifr_data, sizeof(retval));
	//printf("%s:%d port_id=%d\n",__FUNCTION__,__LINE__,retval);
	return retval;
}

int getPortIdByMac(char *interface, unsigned char *mac_addr)
{
	unsigned int port_id=0;
	
	unsigned int arg[2];	
	unsigned char *pmac=(unsigned char *)arg;
	
	memset(pmac, 0, sizeof(unsigned int)*2);
	memcpy(pmac, mac_addr, 6);	
		
	//printf("%s:%d client_mac=%02x%02x%02x%02x%02x%02x\n",__FUNCTION__,__LINE__,
	//		pmac[0],pmac[1],pmac[2],pmac[3],pmac[4],pmac[5]);
	
	port_id=re865xIoctl(interface, RTL8651_IOCTL_GETPORTIDBYCLIENTMAC, arg[0], arg[1], port_id) ;

	//printf("%s:%d port_id=%d\n",__FUNCTION__,__LINE__,port_id);
	return port_id;
}

int addStaticLeaseWithPort(struct static_lease **lease_struct, int port_id, u_int32_t *ip, char *host)
{
	struct static_lease *cur;
	struct static_lease *new_static_lease;

	/* Build new node */
	new_static_lease = xmalloc(sizeof(struct static_lease));
	new_static_lease->port_id= port_id;
	new_static_lease->ip = ip;
	new_static_lease->host = host;	
	new_static_lease->next = NULL;

	/* If it's the first node to be added... */
	if(*lease_struct == NULL)
	{
		*lease_struct = new_static_lease;
	}
	else
	{
		cur = *lease_struct;
		while(cur->next != NULL)
		{
			cur = cur->next;
		}

		cur->next = new_static_lease;
	}

	return 1;

}

#endif

/* Takes the address of the pointer to the static_leases linked list,
 *   Address to a 6 byte mac address
 *   Address to a 4 byte ip address */
int addStaticLease(struct static_lease **lease_struct, unsigned char *mac, u_int32_t *ip, char *host)
{
	struct static_lease *cur;
	struct static_lease *new_static_lease;

	/* Build new node */
	new_static_lease = xmalloc(sizeof(struct static_lease));
	new_static_lease->mac = mac;
	new_static_lease->ip = ip;
	new_static_lease->host = host;	
	new_static_lease->next = NULL;

	/* If it's the first node to be added... */
	if(*lease_struct == NULL)
	{
		*lease_struct = new_static_lease;
	}
	else
	{
		cur = *lease_struct;
		while(cur->next != NULL)
		{
			cur = cur->next;
		}

		cur->next = new_static_lease;
	}

	return 1;

}

/* Check to see if a mac has an associated static lease */
u_int32_t getIpByMac(struct static_lease *lease_struct, void *arg, char **host)
{
	u_int32_t return_ip;
	struct static_lease *cur = lease_struct;
	unsigned char *mac = arg;

	return_ip = 0;

	while(cur != NULL)
	{
		/* If the client has the correct mac  */
		if((cur->mac!=NULL) && (memcmp(cur->mac, mac, 6) == 0))
		{
			return_ip = *(cur->ip);
			*host = cur->host;
		}

		cur = cur->next;
	}

	if(return_ip==server_config.server)
		return 0;
	else
		return return_ip;

}

#ifdef SUPPORT_DHCP_PORT_IP_BIND
u_int32_t getIpByPort(struct static_lease *lease_struct, void *arg, char **host)
{
	u_int32_t return_ip;
	struct static_lease *cur = lease_struct;
	unsigned char *mac = arg;

	unsigned int port_id=0;
	return_ip = 0;

	port_id=getPortIdByMac(LAN_IFNAME, mac);

	if(port_id>1000 || port_id<1)
		return return_ip;
	
	while(cur != NULL)
	{		
		if(cur->port_id==port_id)
		{
			return_ip = *(cur->ip);
			*host = cur->host;
			break;
		}

		cur = cur->next;
	}
	//printf("%s:%d port_id=%d ip=%s\n", __FUNCTION__,__LINE__,port_id, inet_ntoa(*((struct in_addr *)&return_ip)));
	return return_ip;
}
#endif

/* Check to see if a host has an associated static lease */
u_int32_t getIpByHost(struct static_lease *lease_struct, void *arg, int len, char **host)
{
	u_int32_t return_ip;
	struct static_lease *cur = lease_struct;
	unsigned char *name = arg;

	return_ip = 0;

	while(cur != NULL)
	{
		/* If the client has the correct host  */
		if(cur->host && (strlen(cur->host) == (size_t)len) &&
				memcmp(cur->host, name, len) == 0)
		{
			return_ip = *(cur->ip);
			*host = cur->host;			
		}

		cur = cur->next;
	}
	
	if(return_ip==server_config.server)
		return 0;
	else
		return return_ip;
}


/* Check to see if an ip is reserved as a static ip */
u_int32_t reservedIp(struct static_lease *lease_struct, u_int32_t ip)
{
	struct static_lease *cur = lease_struct;

	u_int32_t return_val = 0;

	while(cur != NULL)
	{
		/* If the client has the correct ip  */
		if(*cur->ip == ip)
			return_val = 1;

		cur = cur->next;
	}

	return return_val;

}
#ifdef UDHCP_DEBUG
/* Print out static leases just to check what's going on */
/* Takes the address of the pointer to the static_leases linked list */
void printStaticLeases(struct static_lease **arg)
{
	/* Get a pointer to the linked list */
	struct static_lease *cur = *arg;

	while(cur != NULL)
	{
		/* printf("PrintStaticLeases: Lease mac Address: %x\n", cur->mac); */
		printf("PrintStaticLeases: Lease mac Value: %02x%02x%02x%02x%02x%02x\n", *(cur->mac), 
		*(cur->mac+1), *(cur->mac+2), *(cur->mac+3), *(cur->mac+4), *(cur->mac+5));
		/* printf("PrintStaticLeases: Lease ip Address: %x\n", cur->ip); */
		printf("PrintStaticLeases: Lease ip Value: %x\n", *(cur->ip));
		printf("PrintStaticLeases: hostname: %s\n", cur->host);
		cur = cur->next;
	}


}
#endif



