/*
 *   $Id: util.c,v 1.1 2008-01-11 08:01:30 hf_shi Exp $
 *
 *   Authors:
 *    Lars Fenneberg		<lf@elemental.net>	 
 *
 *   This software is Copyright 1996,1997 by the above mentioned author(s), 
 *   All Rights Reserved.
 *
 *   The license which is distributed with this software in the file COPYRIGHT
 *   applies to this software. If your distribution is missing this file, you
 *   may request it from <pekkas@netcore.fi>.
 *
 */

#include <config.h>
#include <includes.h>
#include <radvd.h>
               
void
mdelay(double msecs)
{
	struct timeval tv;
                
	tv.tv_sec = (time_t)(msecs / 1000.0);
	tv.tv_usec = (suseconds_t)((msecs - tv.tv_sec * 1000.0) * 1000.0);

	select(0,(fd_set *)NULL,(fd_set *)NULL,(fd_set *)NULL, &tv);
}

double
rand_between(double lower, double upper)
{
	return ((upper - lower) / (RAND_MAX + 1.0) * rand() + lower);
}

void
print_addr(struct in6_addr *addr, char *str)
{
	const char *res;

	/* XXX: overflows 'str' if it isn't big enough */
	res = inet_ntop(AF_INET6, (void *)addr, str, INET6_ADDRSTRLEN);
	
	if (res == NULL) 
	{
		strcpy(str, "[invalid address]");	
	}
}
/* Check if an in6_addr exists in the rdnss list */
#ifdef SUPPORT_RDNSS_OPTION
int
check_rdnss_presence(struct AdvRDNSS *rdnss, struct in6_addr *addr)
{
	while (rdnss) {
		if (    !memcmp(&rdnss->AdvRDNSSAddr1, addr, sizeof(struct in6_addr))
		     || !memcmp(&rdnss->AdvRDNSSAddr2, addr, sizeof(struct in6_addr))
		     || !memcmp(&rdnss->AdvRDNSSAddr3, addr, sizeof(struct in6_addr)) )
			break; /* rdnss address found in the list */
		else
			rdnss = rdnss->next; /* no match */
	}
	return (rdnss != NULL);
}
#endif
/* Check if a suffix exists in the dnssl list */
#ifdef SUPPORT_DNSSL_OPTION
int
check_dnssl_presence(struct AdvDNSSL *dnssl, const char *suffix)
{
	int i;
	while (dnssl) {
		for (i = 0; i < dnssl->AdvDNSSLNumber; i++) {
			if (strcmp(dnssl->AdvDNSSLSuffixes[i], suffix) == 0)
				break; /* suffix found in the list */
		}
		if (i != dnssl->AdvDNSSLNumber)
			break;

		dnssl = dnssl->next; /* no match */
	}
	return (dnssl != NULL);
}
#endif