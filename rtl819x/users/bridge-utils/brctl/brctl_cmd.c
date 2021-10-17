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
#include <errno.h>
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

void br_cmd_addbr(struct bridge *br, char *brname, char *arg1)
{
	int err;

	if ((err = br_add_bridge(brname)) == 0)
		return;

	switch (err) {
	case EEXIST:
		fprintf(stderr,	"device %s already exists; can't create "
			"bridge with the same name\n", brname);
		break;

	default:
		perror("br_add_bridge");
		break;
	}
}

void br_cmd_delbr(struct bridge *br, char *brname, char *arg1)
{
	int err;

	if ((err = br_del_bridge(brname)) == 0)
		return;

	switch (err) {
	case ENXIO:
		fprintf(stderr, "bridge %s doesn't exist; can't delete it\n",
			brname);
		break;

	case EBUSY:
		fprintf(stderr, "bridge %s is still up; can't delete it\n",
			brname);
		break;

	default:
		perror("br_del_bridge");
		break;
	}
}

void br_cmd_addif(struct bridge *br, char *ifname, char *arg1)
{
	int err;
	int ifindex;

	ifindex = if_nametoindex(ifname);
	if (!ifindex) {
		fprintf(stderr, "interface %s does not exist!\n", ifname);
		return;
	}

	if ((err = br_add_interface(br, ifindex)) == 0)
		return;

	switch (err) {
	case EBUSY:
		fprintf(stderr,	"device %s is already a member of a bridge; "
			"can't enslave it to bridge %s.\n", ifname,
			br->ifname);
		break;

	case ELOOP:
		fprintf(stderr, "device %s is a bridge device itself; "
			"can't enslave a bridge device to a bridge device.\n",
			ifname);
		break;

	default:
		perror("br_add_interface");
		break;
	}
}

void br_cmd_delif(struct bridge *br, char *ifname, char *arg1)
{
	int err;
	int ifindex;

	ifindex = if_nametoindex(ifname);
	if (!ifindex) {
		fprintf(stderr, "interface %s does not exist!\n", ifname);
		return;
	}

	if ((err = br_del_interface(br, ifindex)) == 0)
		return;

	switch (err) {
	case EINVAL:
		fprintf(stderr, "device %s is not a slave of %s\n",
			ifname, br->ifname);
		break;

	default:
		perror("br_del_interface");
		break;
	}
}

void br_cmd_setageing(struct bridge *br, char *time, char *arg1)
{
	double secs;
	struct timeval tv;

	sscanf(time, "%lf", &secs);
	tv.tv_sec = secs;
	tv.tv_usec = 1000000 * (secs - tv.tv_sec);
	br_set_ageing_time(br, &tv);
}

void br_cmd_setbridgeprio(struct bridge *br, char *_prio, char *arg1)
{
	int prio;

	sscanf(_prio, "%i", &prio);
	br_set_bridge_priority(br, prio);
}

void br_cmd_setfd(struct bridge *br, char *time, char *arg1)
{
	double secs;
	struct timeval tv;

	sscanf(time, "%lf", &secs);
	tv.tv_sec = secs;
	tv.tv_usec = 1000000 * (secs - tv.tv_sec);
	br_set_bridge_forward_delay(br, &tv);
}

void br_cmd_setgcint(struct bridge *br, char *time, char *arg1)
{
	double secs;
	struct timeval tv;

	sscanf(time, "%lf", &secs);
	tv.tv_sec = secs;
	tv.tv_usec = 1000000 * (secs - tv.tv_sec);
	br_set_gc_interval(br, &tv);
}

void br_cmd_sethello(struct bridge *br, char *time, char *arg1)
{
	double secs;
	struct timeval tv;

	sscanf(time, "%lf", &secs);
	tv.tv_sec = secs;
	tv.tv_usec = 1000000 * (secs - tv.tv_sec);
	br_set_bridge_hello_time(br, &tv);
}

void br_cmd_setmaxage(struct bridge *br, char *time, char *arg1)
{
	double secs;
	struct timeval tv;

	sscanf(time, "%lf", &secs);
	tv.tv_sec = secs;
	tv.tv_usec = 1000000 * (secs - tv.tv_sec);
	br_set_bridge_max_age(br, &tv);
}

void br_cmd_setpathcost(struct bridge *br, char *arg0, char *arg1)
{
	int cost;
	struct port *p;

	if ((p = br_find_port(br, arg0)) == NULL) {
		fprintf(stderr, "can't find port %s in bridge %s\n", arg0, br->ifname);
		return;
	}

	sscanf(arg1, "%i", &cost);
	br_set_path_cost(p, cost);
}

void br_cmd_setportprio(struct bridge *br, char *arg0, char *arg1)
{
	int cost;
	struct port *p;

	if ((p = br_find_port(br, arg0)) == NULL) {
		fprintf(stderr, "can't find port %s in bridge %s\n", arg0, br->ifname);
		return;
	}

	sscanf(arg1, "%i", &cost);
	br_set_port_priority(p, cost);
}

void br_cmd_stp(struct bridge *br, char *arg0, char *arg1)
{
	int stp;

	stp = 0;
	if (!strcmp(arg0, "on") || !strcmp(arg0, "yes") || !strcmp(arg0, "1"))
		stp = 1;

	br_set_stp_state(br, stp);
}

void br_cmd_meshsignaloff(struct bridge *br, char *arg0, char *arg1)
{
	br_turnoff_signal_pathsel(br);
}

void br_cmd_showstp(struct bridge *br, char *arg0, char *arg1)
{
	br_dump_info(br);
}

void br_cmd_show(struct bridge *br, char *arg0, char *arg1)
{
	printf("bridge name\tbridge id\t\tSTP enabled\tinterfaces\n");
	br = bridge_list;
	while (br != NULL) {
		printf("%s\t\t", br->ifname);
		br_dump_bridge_id((unsigned char *)&br->info.bridge_id);
		printf("\t%s\t\t", br->info.stp_enabled?"yes":"no");
		br_dump_interface_list(br);

		br = br->next;
	}
}

static int compare_fdbs(const void *_f0, const void *_f1)
{
	const struct fdb_entry *f0 = _f0;
	const struct fdb_entry *f1 = _f1;

#if 0
	if (f0->port_no < f1->port_no)
		return -1;

	if (f0->port_no > f1->port_no)
		return 1;
#endif

	return memcmp(f0->mac_addr, f1->mac_addr, 6);
}

void __dump_fdb_entry(struct fdb_entry *f)
{
	printf("%3i\t", f->port_no);
	printf("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x\t",
	       f->mac_addr[0], f->mac_addr[1], f->mac_addr[2],
	       f->mac_addr[3], f->mac_addr[4], f->mac_addr[5]);
	printf("%s\t\t", f->is_local?"yes":"no");
	br_show_timer(&f->ageing_timer_value);
	printf("\n");
}

void br_cmd_showmacs(struct bridge *br, char *arg0, char *arg1)
{
	struct fdb_entry fdb[1024];
	int offset;

	printf("port no\tmac addr\t\tis local?\tageing timer\n");

	offset = 0;
	while (1) {
		int i;
		int num;
#ifdef GUEST_ZONE
		num = br_read_fdb(br, fdb, offset, 1024, 0);
#else
		num = br_read_fdb(br, fdb, offset, 1024);
#endif
		if (!num)
			break;

		qsort(fdb, num, sizeof(struct fdb_entry), compare_fdbs);

		for (i=0;i<num;i++)
			__dump_fdb_entry(fdb+i);

		offset += num;
	}
}

#ifdef MULTICAST_FILTER
void br_cmd_clrfltrport(struct bridge *br, char *arg0, char *arg1)
{
	br_set_clrfltr(br);
}

void br_cmd_setfltrport(struct bridge *br, char *arg0, char *arg1)
{
	int port;

	sscanf(arg0, "%i", &port);
	br_set_fltrport(br, port);
}
#endif

#ifdef MULTICAST_BWCTRL
void br_cmd_setbwctrl(struct bridge *br, char *arg0, char *arg1)
{
	int bandwidth;
	struct port *p;

	if ((p = br_find_port(br, arg0)) == NULL) {
		fprintf(stderr, "can't find port %s in bridge %s\n", arg0, br->ifname);
		return;
	}

	sscanf(arg1, "%i", &bandwidth);
	br_set_mlticst_bw(p, bandwidth);
}
#endif

#ifdef RTL_BRIDGE_MAC_CLONE
void br_cmd_enable_macclone(struct bridge *br, char *arg0, char *arg1)
{
	struct port *p, *target;

	if ((p = br_find_port(br, arg0)) == NULL) {
		fprintf(stderr, "can't find port %s in bridge %s\n", arg0, br->ifname);
		return;
	}

	if ((target = br_find_port(br, arg1)) == NULL) {
		fprintf(stderr, "can't find port %s in bridge %s\n", arg1, br->ifname);
		return;
	}

	br_set_port_enable_macclone(p, target);
}
#endif


#ifdef GUEST_ZONE
void br_cmd_set_zone(struct bridge *br, char *arg0, char *arg1)
{
	struct port *p;
	int val;
	//char tmpbuf[32];

	if ((p = br_find_port(br, arg0)) == NULL) {
		fprintf(stderr, "can't find port %s in bridge %s\n", arg0, br->ifname);
		return;
	}
	sscanf(arg1, "%i", &val);
	br_set_port_zone(p, val);

#if 0
	// Don't disable bridge shortcut for performance
	if (!memcmp("wlan", arg0, 4)) {
		if (val == 0) {
			sprintf(tmpbuf, "iwpriv %s set_mib disable_brsc=0", arg0);
			system(tmpbuf);
		}
		else {
			sprintf(tmpbuf, "iwpriv %s set_mib disable_brsc=1", arg0);
			system(tmpbuf);
		}
	}
#endif
}

void br_cmd_set_isolation_zone(struct bridge *br, char *arg0, char *arg1)
{
	int val;
	sscanf(arg0, "%i", &val);	

	br_set_isolation_zone(br, val);
}

void br_cmd_set_isolation_guest(struct bridge *br, char *arg0, char *arg1)
{
	int val;
	sscanf(arg0, "%i", &val);	
	
	br_set_isolation_guest(br, val);
}

void br_cmd_chk_guestmac(struct bridge *br, char *arg0, char *arg1)
{
	unsigned char mac[6];
	unsigned int tmp[6];
	struct fdb_entry fdb[1024];
	int offset;
	int verbose;

	sscanf(arg0, "%i", &verbose);	

	if (sscanf(arg1, "%02x%02x%02x%02x%02x%02x", 
			&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) != 6) {
		fprintf(stderr, "invalid mac address format [xxxxxxxxxxxx]!\n");
		return;			
	}
	mac[0] = (unsigned char)tmp[0];
	mac[1] = (unsigned char)tmp[1];
	mac[2] = (unsigned char)tmp[2];
	mac[3] = (unsigned char)tmp[3];
	mac[4] = (unsigned char)tmp[4];
	mac[5] = (unsigned char)tmp[5];

	offset = 0;
	while (1) {
		int i;
		int num;
		num = br_read_fdb(br, fdb, offset, 1024, 1);
		if (!num) {
			break;
		}
		
		for (i=0;i<num;i++) {			
			if (!memcmp(fdb[i].mac_addr, mac, 6)) {
				if (verbose)
					fprintf(stderr, "is guest address!\n");
				exit(1);
			}			
		}
		offset += num;
	}
	if (verbose)
		fprintf(stderr, "not guest address!\n");	
}

void br_cmd_set_lockclient(struct bridge *br, char *arg0, char *arg1)
{
	unsigned char mac[6];
	unsigned int tmp[6];

	if (sscanf(arg0, "%02x%02x%02x%02x%02x%02x", 
			&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) != 6) {
		fprintf(stderr, "invalid mac address format [xxxxxxxxxxxx]!\n");
		return;			
	}
	mac[0] = (unsigned char)tmp[0];
	mac[1] = (unsigned char)tmp[1];
	mac[2] = (unsigned char)tmp[2];
	mac[3] = (unsigned char)tmp[3];
	mac[4] = (unsigned char)tmp[4];
	mac[5] = (unsigned char)tmp[5];

	br_set_lock_client(br, mac);
}

void br_cmd_show_guestinfo(struct bridge *br, char *arg0, char *arg1)
{
	br_show_guestinfo(br);
}

void br_cmd_set_gatewaymac(struct bridge *br, char *arg0, char *arg1)
{
	unsigned char mac[6];
	unsigned int tmp[6];

	if (sscanf(arg0, "%02x%02x%02x%02x%02x%02x", 
			&tmp[0], &tmp[1], &tmp[2], &tmp[3], &tmp[4], &tmp[5]) != 6) {
		fprintf(stderr, "invalid mac address format [xxxxxxxxxxxx]!\n");
		return;
	}
	mac[0] = (unsigned char)tmp[0];
	mac[1] = (unsigned char)tmp[1];
	mac[2] = (unsigned char)tmp[2];
	mac[3] = (unsigned char)tmp[3];
	mac[4] = (unsigned char)tmp[4];
	mac[5] = (unsigned char)tmp[5];

	br_set_gateway_mac(br, mac);
}
#endif // GUEST_ZONE

static struct command commands[] = {
	{0, 1, "addbr", br_cmd_addbr},
	{1, 1, "addif", br_cmd_addif},
	{0, 1, "delbr", br_cmd_delbr},
	{1, 1, "delif", br_cmd_delif},
	{1, 1, "setageing", br_cmd_setageing},
	{1, 1, "setbridgeprio", br_cmd_setbridgeprio},
	{1, 1, "setfd", br_cmd_setfd},
	{1, 1, "setgcint", br_cmd_setgcint},
	{1, 1, "sethello", br_cmd_sethello},
	{1, 1, "setmaxage", br_cmd_setmaxage},
	{1, 2, "setpathcost", br_cmd_setpathcost},
	{1, 2, "setportprio", br_cmd_setportprio},
	{0, 0, "show", br_cmd_show},
	{1, 0, "showmacs", br_cmd_showmacs},
	{1, 0, "showstp", br_cmd_showstp},
	{1, 1, "stp", br_cmd_stp},
//#if defined(CONFIG_RTK_MESH) && defined(MESH_DYPORTAL)
	{0, 0, "meshsignaloff", br_cmd_meshsignaloff},
//#endif

#ifdef MULTICAST_FILTER
	{1, 0, "clrfltrport", br_cmd_clrfltrport},
	{1, 1, "setfltrport", br_cmd_setfltrport},
#endif

#ifdef MULTICAST_BWCTRL
	{1, 2, "setbwctrl", br_cmd_setbwctrl},
#endif

#ifdef RTL_BRIDGE_MAC_CLONE
	{1, 2, "clone", br_cmd_enable_macclone},
#endif

#ifdef GUEST_ZONE
	{1, 2, "setzone", br_cmd_set_zone},	
	{1, 1, "setzoneisolate", br_cmd_set_isolation_zone},	
	{1, 1, "setguestisolate", br_cmd_set_isolation_guest},
	{1, 2, "chkguestmac", br_cmd_chk_guestmac},
	{1, 1, "setlockclient", br_cmd_set_lockclient},	
	{1, 0, "showguestinfo", br_cmd_show_guestinfo},	
	{1, 1, "setgatewaymac", br_cmd_set_gatewaymac},
#endif
};

struct command *br_command_lookup(char *cmd)
{
	int i;
	int numcommands;

	numcommands = sizeof(commands)/sizeof(commands[0]);

	for (i=0;i<numcommands;i++)
		if (!strcmp(cmd, commands[i].name))
			return &commands[i];

	return NULL;
}
