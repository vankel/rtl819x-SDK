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
#include "brctl.h"

char *help_message =
"commands:\n"
"\taddbr\t\t<bridge>\t\tadd bridge\n"
"\taddif\t\t<bridge> <device>\tadd interface to bridge\n"
"\tdelbr\t\t<bridge>\t\tdelete bridge\n"
"\tdelif\t\t<bridge> <device>\tdelete interface from bridge\n"
"\tshow\t\t\t\t\tshow a list of bridges\n"
"\tshowmacs\t<bridge>\t\tshow a list of mac addrs\n"
"\tshowstp\t\t<bridge>\t\tshow bridge stp info\n"
"\n"
"\tsetageing\t<bridge> <time>\t\tset ageing time\n"
"\tsetbridgeprio\t<bridge> <prio>\t\tset bridge priority\n"
"\tsetfd\t\t<bridge> <time>\t\tset bridge forward delay\n"
"\tsetgcint\t<bridge> <time>\t\tset garbage collection interval\n"
"\tsethello\t<bridge> <time>\t\tset hello time\n"
"\tsetmaxage\t<bridge> <time>\t\tset max message age\n"
"\tsetpathcost\t<bridge> <port> <cost>\tset path cost\n"
"\tsetportprio\t<bridge> <port> <prio>\tset port priority\n"
"\tstp\t\t<bridge> <state>\tturn stp on/off\n"
"\tmeshsignaloff\t\tdisable signal to pathselection daemon(mesh)\n"
#ifdef MULTICAST_FILTER
"\n"
"\tclrfltrport\t<bridge>\t\tclear multicast filter\n"
"\tsetfltrport\t<bridge> <port>\t\tset port number of filter\n"
#endif
#ifdef MULTICAST_BWCTRL
"\n"
"\tsetbwctrl\t<bridge> <port> <bdwh>\tset multicast bandwidth (kbps)\n"
#endif
#ifdef RTL_BRIDGE_MAC_CLONE
"\n"
"\tclone\t\t<bridge> <ptFr> <ptTo>\tturn MAC clone on\n"
#endif
#ifdef GUEST_ZONE
"\tsetzone\t\t<bridge> <device> <val>\tset zone type (0: host, 1: guest, 2: gateway) for interface\n"
"\tsetzoneisolate\t<bridge> <value>\tset zone isolation (0: no, 1: yes)\n"
"\tsetguestisolate\t<bridge> <value>\tset guest isolation (0: no, 1: yes)\n"
"\tchkguestmac\t<bridge> <verb> <mac>\tcheck if mac addrs of client is come from guest zone\n"
"\tsetlockclient\t<bridge> <mac>\t\tset mac addrs of locked client list\n"
"\tshowguestinfo\t<bridge>\t\tshow zone and locked client info\n"
"\tsetgatewaymac\t<bridge> <mac>\t\tset mac addr of gateway\n"
#endif
;

void help()
{
	fprintf(stderr, help_message);
}

int main(int argc, char *argv[])
{
	int argindex;
	struct bridge *br;
	struct command *cmd;

	br_init();

	if (argc < 2)
		goto help;

	if ((cmd = br_command_lookup(argv[1])) == NULL) {
		fprintf(stderr, "never heard of command [%s]\n", argv[1]);
		goto help;
	}

	argindex = 2;
	br = NULL;
	if (cmd->needs_bridge_argument) {
		if (argindex >= argc) {
			fprintf(stderr, "this option requires a bridge name as argument\n");
			return 1;
		}

		br = br_find_bridge(argv[argindex]);

		if (br == NULL) {
			fprintf(stderr, "bridge %s doesn't exist!\n", argv[argindex]);
			return 1;
		}

		argindex++;
	}

	if (argc - argindex != cmd->num_string_arguments) {
		fprintf(stderr, "incorrect number of arguments for command\n");
		return 1;
	}

	cmd->func(br, argv[argindex], argv[argindex+1]);

	return 0;

help:
	help();
	return 1;
}
