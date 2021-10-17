/* project: miniupnp
 * webpage: http://miniupnp.free.fr/
 * (c) 2006 Thomas Bernard
 * this software is subject to the conditions detailed in the
 * LICENCE file provided in this distribution */
/* $Id: getifaddr.c,v 1.2 2007-08-31 11:36:38 chien_hsiang Exp $ */
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#if defined(USE_GETIFADDRS) || defined(ENABLE_IPV6)
#include "ifaddrs.h"
#endif

#include <stdio.h>
#include "getifaddr.h"


#define syslog(x, fmt, args...); 

int
getifaddr(const char * ifname, char * buf, int len)
{
//#ifndef USE_GETIFADDRS

	/* use ioctl SIOCGIFADDR. Works only for ip v4 */

	/* SIOCGIFADDR struct ifreq *  */
	int s;
	struct ifreq ifr;
	int ifrlen;
	struct sockaddr_in * addr;
	ifrlen = sizeof(ifr);
	s = socket(PF_INET, SOCK_DGRAM, 0);
	if(s<0)
	{
		syslog(LOG_ERR, "socket(PF_INET, SOCK_DGRAM, 0): %m");
		return -1;
	}
       // syslog(LOG_INFO,"name1=%s, name2=%s SIZE=%d\n",ifr.ifr_name,ifname,IFNAMSIZ);
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
       // syslog(LOG_INFO,"Xname1=%s, name2=%s SIZE=%d\n",ifr.ifr_name,ifname,IFNAMSIZ);
	if(ioctl(s, SIOCGIFADDR, &ifr, &ifrlen) < 0)
	{
		syslog(LOG_ERR, "ioctl(s, SIOCGIFADDR, ...): %m");
		close(s);
		return -1;
	}
	addr = (struct sockaddr_in *)&ifr.ifr_addr;
	if(!inet_ntop(AF_INET, &addr->sin_addr, buf, len))
	{
		syslog(LOG_ERR, "inet_ntop FAILED");
		close(s);
		return -1;
	}
	close(s);
	return 0;
}
 
#ifdef ENABLE_IPV6
int
getifaddrv6(const char * ifname, char * buf, int len)
{
		/* Works for all address families (both ip v4 and ip v6) */
		struct ifaddrs * ifap;
		struct ifaddrs * ife;
		unsigned char ipv6 = 0;
		
		if(!ifname || ifname[0]=='\0')
			return -1;
		if(getifaddrs(&ifap)<0)
		{
			syslog(LOG_ERR, "getifaddrs: %m");
			return -1;
		}
		if(len > INET_ADDRSTRLEN) 
			ipv6 = 1;
		if(ifap == NULL)
			return -1;
		for(ife = ifap; ife; ife = ife->ifa_next)
		{
			/* skip other interfaces if one was specified */
			if(ifname && ife->ifa_name && (0 != strcmp(ifname, ife->ifa_name)))
				continue;
			if(ife->ifa_addr == NULL)
				continue;
			switch(ife->ifa_addr->sa_family)
			{
			case AF_INET:
				if(ipv6)
					continue;
				inet_ntop(ife->ifa_addr->sa_family,
						  &((struct sockaddr_in *)ife->ifa_addr)->sin_addr,
						  buf, len);
				break;
	
			case AF_INET6:
				if(!ipv6)
					continue;
				inet_ntop(ife->ifa_addr->sa_family,
						  &((struct sockaddr_in6 *)ife->ifa_addr)->sin6_addr,
						  buf, len);
				break;
				
			}
		}
		freeifaddrs(ifap);
		return 0;
}
find_ipv6_addr(const char * ifname,
               char * dst, int n)
{
	struct ifaddrs * ifap;
	struct ifaddrs * ife;
	const struct sockaddr_in6 * addr;
	char buf[64];
	int r = 0;

	if(!dst)
		return -1;

	if(getifaddrs(&ifap)<0)
	{
		syslog(LOG_ERR, "getifaddrs: %m");
		return -1;
	}
	for(ife = ifap; ife; ife = ife->ifa_next)
	{
		/* skip other interfaces if one was specified */
		if(ifname && (0 != strcmp(ifname, ife->ifa_name)))
			continue;
		if(ife->ifa_addr == NULL)
			continue;
		if(ife->ifa_addr->sa_family == AF_INET6)
		{
			addr = (const struct sockaddr_in6 *)ife->ifa_addr;
			
			
				inet_ntop(ife->ifa_addr->sa_family,
				          &addr->sin6_addr,
				          buf, sizeof(buf));
				/* add brackets */
				snprintf(dst, n, "[%s]", buf);
				r = 1;
			
		}
	}
	freeifaddrs(ifap);
	return r;
}
#endif


