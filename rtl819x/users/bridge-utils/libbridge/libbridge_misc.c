/*
 * Copyright (C) 2000 Lennert Buytenhek
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <asm/param.h>

#if 1
/* fix the error temporarily:
	toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227/include/linux/if_bridge.h:183: error: field 'ip6' has incomplete type
    Wen-jain, please help to patch it.
 */
#include <linux/types.h>
struct in6_addr {
        union {
                __u8            u6_addr8[16];
                __be16          u6_addr16[8];
                __be32          u6_addr32[4];
        } in6_u;
};
#endif

#include "libbridge.h"
#include "libbridge_private.h"

unsigned long __tv_to_jiffies(struct timeval *tv)
{
	unsigned long long jif;

	jif = 1000000ULL * tv->tv_sec + tv->tv_usec;

	return (HZ*jif)/1000000;
}

void __jiffies_to_tv(struct timeval *tv, unsigned long jiffies)
{
	unsigned long long tvusec;

	tvusec = (1000000ULL*jiffies)/HZ;
	tv->tv_sec = tvusec/1000000;
	tv->tv_usec = tvusec - 1000000 * tv->tv_sec;
}

static char *state_names[5] = {"disabled", "listening", "learning", "forwarding", "blocking"};

char *br_get_state_name(int state)
{
	if (state >= 0 && state <= 4)
		return state_names[state];

	return "<INVALID STATE>";
}

struct bridge *br_find_bridge(char *brname)
{
	struct bridge *b;

	b = bridge_list;
	while (b != NULL) {
		if (!strcmp(b->ifname, brname))
			return b;

		b = b->next;
	}

	return NULL;
}

struct port *br_find_port(struct bridge *br, char *portname)
{
	char index;
	struct port *p;

	if (!(index = if_nametoindex(portname)))
		return NULL;

	p = br->firstport;
	while (p != NULL) {
		if (p->ifindex == index)
			return p;

		p = p->next;
	}

	return NULL;
}
