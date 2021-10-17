/* clientpacket.c
 *
 * Packet generation and dispatching functions for the DHCP client.
 *
 * Russ Dill <Russ.Dill@asu.edu> July 2001
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
 
#include <string.h>
#include <sys/socket.h>
#include <features.h>
#if __GLIBC__ >=2 && __GLIBC_MINOR >= 1
#include <netpacket/packet.h>
#include <net/ethernet.h>
#else
#include <asm/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#endif
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "dhcpd.h"
#include "packet.h"
#include "options.h"
#include "dhcpc.h"
#include "debug.h"

#if defined(TR069_ANNEX_F) || defined(_PRMT_X_TELEFONICA_ES_DHCPOPTION_)
#ifndef HOME_GATEWAY
#define HOME_GATEWAY
#endif
#include "apmib.h"
#include "mibtbl.h"
#endif


/* Create a random xid */
unsigned long random_xid(void)
{
	static int initialized;
	if (!initialized) {
		int fd;
		unsigned long seed;

		fd = open("/dev/urandom", 0);
		if (fd < 0 || read(fd, &seed, sizeof(seed)) < 0) {
			LOG(LOG_WARNING, "Could not load seed from /dev/urandom: %s",
				strerror(errno));
			seed = time(0);
		}
		if (fd >= 0) close(fd);
		srand(seed);
		initialized++;
	}
	return rand();
}


/* initialize a packet with the proper defaults */
static void init_packet(struct dhcpMessage *packet, char type)
{
#ifndef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
	struct vendor  {
		char vendor, length;
		char str[sizeof("udhcp "VERSION)];
	} vendor_id = { DHCP_VENDOR,  sizeof("udhcp "VERSION) - 1, "udhcp "VERSION};
#endif

	init_header(packet, type);
	memcpy(packet->chaddr, client_config.arp, 6);
	add_option_string(packet->options, client_config.clientid);
	if (client_config.hostname) add_option_string(packet->options, client_config.hostname);

#ifndef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
	if(type!=DHCPDECLINE && type!=DHCPRELEASE)
	add_option_string(packet->options, (unsigned char *) &vendor_id);
#endif

#ifdef CONFIG_RTL865X_KLD
	if (client_config.broadcast_flag) packet->flags |= BROADCAST_FLAG;
#endif
#ifdef SUPPORT_ZIONCOM_RUSSIA
	if(type==DHCPDISCOVER || type==DHCPREQUEST)
		add_option_string(packet->options, client_config.max_msg_size);
#endif
}


/* Add a paramater request list for stubborn DHCP servers. Pull the data
 * from the struct in options.c. Don't do bounds checking here because it
 * goes towards the head of the packet. */
static void add_requests(struct dhcpMessage *packet)
{
	int end = end_option(packet->options);
	int i, len = 0;

	packet->options[end + OPT_CODE] = DHCP_PARAM_REQ;
	for (i = 0; options[i].code; i++)
		if (options[i].flags & OPTION_REQ)
			packet->options[end + OPT_DATA + len++] = options[i].code;
	packet->options[end + OPT_LEN] = len;
	packet->options[end + OPT_DATA + len] = DHCP_END;

}

#ifdef TR069_ANNEX_F
static void add_option_125(unsigned char *optionptr)
{
	char tmp[512];
	unsigned char optionStr[300] = {0};
	unsigned char serialNum[256] = {0};
	
	int ouiLen, serialNumLen, prodClsLen, subCodeLen, option125Len;
	int i;

	memset(optionStr, 0, sizeof(optionStr));
	
	// option 125
	optionStr[0] = 0x7d;

	// Enterprise Number: 3561 for "Broadband Forum"
	optionStr[2] = 0x00;
	optionStr[3] = 0x00;
	optionStr[4] = 0x0d;
	optionStr[5] = 0xe9;
	
	if (apmib_init())
	{
		apmib_get(MIB_HW_NIC1_ADDR, (void *)tmp);
		sprintf(serialNum, "%02x%02x%02x%02x%02x%02x",
			(unsigned char)tmp[0], (unsigned char)tmp[1], (unsigned char)tmp[2],
			(unsigned char)tmp[3], (unsigned char)tmp[4], (unsigned char)tmp[5]);

		ouiLen = strlen(MANUFACTURER_OUI);
		serialNumLen = strlen(serialNum);
		prodClsLen = strlen(PRODUCT_CLASS_DEVICE);

		subCodeLen = ouiLen + serialNumLen + prodClsLen + 6;
		option125Len = subCodeLen + 5;

		optionStr[1] = option125Len;
		optionStr[6] = subCodeLen;
		optionStr[7] = 0x01; // DeviceManufacturerOUI
		optionStr[8] = ouiLen;
		sprintf(&optionStr[9], "%s", MANUFACTURER_OUI);
		optionStr[8+ouiLen+1] = 0x02; // DeviceSerialNumber
		optionStr[8+ouiLen+2] = serialNumLen;
		sprintf(&optionStr[8+ouiLen+3], "%s", serialNum);
		optionStr[8+ouiLen+3+serialNumLen] = 0x03; // DeviceProductClass
		optionStr[8+ouiLen+3+serialNumLen+1] = prodClsLen;
		sprintf(&optionStr[8+ouiLen+3+serialNumLen+2], "%s", PRODUCT_CLASS_DEVICE);
		
#if 0
		printf("oui %d serial %d prod %d subcode %d optionlen %d\n", 
			ouiLen, serialNumLen, prodClsLen, subCodeLen, option125Len);
		
		for (i=0; i<300; i++) {
			printf("%02x ", optionStr[i]);

			if (i%15 == 0)
				printf("\n\n");
		}
#endif

		add_option_string(optionptr, optionStr);
	}
}
#endif

#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
void initialDhcpcOption60()
{
	int entrynum;
	MIB_CE_DHCP_OPTION_T entry;
	
	unsigned char opt60_val[]={0x12, 0x34, 0x01, 0x07, 'R', 'e', 'a', 'l', 't', 'e', 'k', 0x02, 0x03, 'C', 'P', 'E', 0x03, 0x08, 'E', '8', 'R', 'O', 'U', 'T', 'E', 'R', 0x04, 0x03, '1', '.', '0', 0, 0};

	if (!apmib_get(MIB_DHCP_CLIENT_OPTION_TBL_NUM, (void *)&entrynum))
	{
		printf("%s:%d apmib_get fails!####\n",__FUNCTION__,__LINE__);
		return;
	}
	
	if(entrynum>0)
	{
		printf("%s:%d####entrynum=%d!\n",__FUNCTION__,__LINE__,entrynum);
		return;
	}	

	if(entrynum>=MAX_DHCP_CLIENT_OPTION_NUM)
		return;
	
	entry.enable=1;
	entry.usedFor=eUsedFor_DHCPClient_Sent;
	entry.order=0;
	entry.tag=60;
	entry.len=0x1F;
	strcpy(entry.value, opt60_val);
	entry.ifIndex=client_config.ifindex;

	apmib_set(MIB_DHCP_CLIENT_OPTION_DEL, (void *)&entry);
	
	if (!apmib_set(MIB_DHCP_CLIENT_OPTION_ADD, (void *)&entry))
	{		
		printf("%s:%d apmib_set MIB_DHCP_CLIENT_OPTION_ADD fails!####\n",__FUNCTION__,__LINE__);
		return;		
	}
	else
	{
		//printf("%s:%d apmib_set MIB_DHCP_CLIENT_OPTION_ADD success!####\n",__FUNCTION__,__LINE__);
		apmib_update(CURRENT_SETTING);
	}
}

int addDhcpcOption(struct dhcpMessage *packet, int type)
{
	MIB_CE_DHCP_OPTION_T entry;
	int i, entrynum;
	unsigned char option[DHCP_OPT_VAL_LEN+2];
	//printf("%s:%d  !####\n",__FUNCTION__,__LINE__);
	if(!apmib_get(MIB_DHCP_CLIENT_OPTION_TBL_NUM, (void *)&entrynum))
	{
		printf("%s:%d apmib_get fails!####\n",__FUNCTION__,__LINE__);
		return -1;
	}
	//printf("%s:%d entrynum=%d###\n",__FUNCTION__,__LINE__,entrynum);
	for (i=1; i<=entrynum; i++) 
	{
		*((char *)&entry) = (char)i;
		if(!apmib_get(MIB_DHCP_CLIENT_OPTION_TBL, (void *)&entry))
			continue;		

		if(entry.enable==0)
			continue;
		
		//if (entry.ifIndex != client_config.ifindex)
		//	continue;

		if (entry.usedFor != eUsedFor_DHCPClient_Sent)
			continue;

		if(entry.tag == type)
		{			
			option[OPT_CODE] = (unsigned char)entry.tag;
			option[OPT_LEN] = entry.len;
			memcpy(option+OPT_DATA, entry.value, entry.len);

			add_option_string(packet->options, option);

			return 0;
		}
	}	
	return -1;
}
#endif

/* Broadcast a DHCP discover packet to the network, with an optionally requested IP */
int send_discover(unsigned long xid, unsigned long requested)
{
	struct dhcpMessage packet;
	
#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
	int ret_val;
#endif

	init_packet(&packet, DHCPDISCOVER);
	packet.xid = xid;
	if (requested)
		add_simple_option(packet.options, DHCP_REQUESTED_IP, requested);

#ifdef TR069_ANNEX_F
#if 0
	{
		unsigned char testbuf[] = {
			0x7d , // option 125
			0x32 , // len
			0x00 , 0x00 , 0x0d , 0xe9 , // Enterprise Number: 3561 for "ADSL Forum"
			0x2d , // len
			0x01 , 0x06 , 0x30 , 0x30 , 0x30, 0x31 , 0x30 , 0x32 , // subcode, len, data
			0x02 , 0x10 , 0x30 , 0x30 , 0x30 , 0x31 , 0x30 , 0x32 , 0x2d , 0x34 , 0x32 , 0x38 , 0x32, // subcode, len, data
			0x38 , 0x38 , 0x38 , 0x32 , 0x39 , 
			0x03 , 0x11 , 0x43 , 0x44 , 0x52 , 0x6f , 0x75 , 0x74 , 0x65 , 0x72 , 0x20, // subcode, len, data
			0x56 , 0x6f , 0x49 , 0x50 , 0x20 , 0x41 , 0x54 , 0x41 };
	
		add_option_string(packet.options, testbuf);
	}
#else
	add_option_125(packet.options);
#endif
#endif

#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
	//printf("%s:%d Add DHCP option 60!\n",__FUNCTION__,__LINE__);
	ret_val=addDhcpcOption(&packet, DHCP_VENDOR); //and option 60
	if(ret_val<0)
	{
		initialDhcpcOption60();
		addDhcpcOption(&packet, DHCP_VENDOR); //and option 60
	}
#endif

	add_requests(&packet);
// david, disable message. 2003-5-21	
//	LOG(LOG_DEBUG, "Sending discover...");
	return raw_packet(&packet, INADDR_ANY, CLIENT_PORT, INADDR_BROADCAST, 
				SERVER_PORT, MAC_BCAST_ADDR, client_config.ifindex);
}


/* Broadcasts a DHCP request message */
int send_selecting(unsigned long xid, unsigned long server, unsigned long requested)
{
	struct dhcpMessage packet;
	struct in_addr addr;

	init_packet(&packet, DHCPREQUEST);
	packet.xid = xid;

	add_simple_option(packet.options, DHCP_REQUESTED_IP, requested);
	add_simple_option(packet.options, DHCP_SERVER_ID, server);

#ifdef TR069_ANNEX_F
	add_option_125(packet.options);
#endif

#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_		
	addDhcpcOption(&packet, DHCP_VENDOR); //and option 60
#endif

	add_requests(&packet);
	addr.s_addr = requested;
	LOG(LOG_DEBUG, "Sending select for %s...", inet_ntoa(addr));
	return raw_packet(&packet, INADDR_ANY, CLIENT_PORT, INADDR_BROADCAST, 
				SERVER_PORT, MAC_BCAST_ADDR, client_config.ifindex);
}


/* Unicasts or broadcasts a DHCP renew message */
int send_renew(unsigned long xid, unsigned long server, unsigned long ciaddr)
{
	struct dhcpMessage packet;
	int ret = 0;

	init_packet(&packet, DHCPREQUEST);
	packet.xid = xid;
	packet.ciaddr = ciaddr;

	add_simple_option(packet.options, DHCP_REQUESTED_IP, ciaddr);
	
#ifdef TR069_ANNEX_F
	add_option_125(packet.options);
#endif

#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_		
	addDhcpcOption(&packet, DHCP_VENDOR); //and option 60
#endif

	add_requests(&packet);
	LOG(LOG_DEBUG, "Sending renew...");
	if (server) 
		ret = kernel_packet(&packet, ciaddr, CLIENT_PORT, server, SERVER_PORT);
	else 
//		ret = raw_packet(&packet, INADDR_ANY, CLIENT_PORT, INADDR_BROADCAST,
//				SERVER_PORT, MAC_BCAST_ADDR, client_config.ifindex);
		ret = raw_packet(&packet, ciaddr, CLIENT_PORT, INADDR_BROADCAST,
				SERVER_PORT, MAC_BCAST_ADDR, client_config.ifindex);
#ifdef TR069_ANNEX_F
	if (ret > 0)
		unlink(TR069_ANNEX_F_FILE);
#endif
	return ret;
}	


/* Unicasts a DHCP release message */
int send_release(unsigned long server, unsigned long ciaddr)
{
	struct dhcpMessage packet;
#ifdef TR069_ANNEX_F
	int ret = 0;
#endif

	init_packet(&packet, DHCPRELEASE);
	packet.xid = random_xid();
	packet.ciaddr = ciaddr;
	
	add_simple_option(packet.options, DHCP_REQUESTED_IP, ciaddr);
	add_simple_option(packet.options, DHCP_SERVER_ID, server);

	LOG(LOG_DEBUG, "Sending release...");
#ifdef TR069_ANNEX_F
	ret = kernel_packet(&packet, ciaddr, CLIENT_PORT, server, SERVER_PORT);
	if (ret > 0)
		unlink(TR069_ANNEX_F_FILE);
#else
	return kernel_packet(&packet, ciaddr, CLIENT_PORT, server, SERVER_PORT);
#endif
}

int send_decline(uint32_t server, uint32_t requested)
{
	struct dhcpMessage packet;

	init_packet(&packet, DHCPDECLINE);
	packet.xid = random_xid();
	add_simple_option(packet.options, DHCP_REQUESTED_IP, requested);
	add_simple_option(packet.options, DHCP_SERVER_ID, server);

	LOG(LOG_DEBUG, "Sending decline...");
	
	return raw_packet(&packet, INADDR_ANY, CLIENT_PORT, INADDR_BROADCAST, 
				SERVER_PORT, MAC_BCAST_ADDR, client_config.ifindex);	
}

/* return -1 on errors that are fatal for the socket, -2 for those that aren't */
int get_raw_packet(struct dhcpMessage *payload, int fd)
{
	int bytes;
	struct udp_dhcp_packet packet;
	u_int32_t source, dest;
	u_int16_t check;

	memset(&packet, 0, sizeof(struct udp_dhcp_packet));
	bytes = read(fd, &packet, sizeof(struct udp_dhcp_packet));
	if (bytes < 0) {
		DEBUG(LOG_INFO, "couldn't read on raw listening socket -- ignoring");
		usleep(500000); /* possible down interface, looping condition */
		return -1;
	}
	
	if (bytes < (int) (sizeof(struct iphdr) + sizeof(struct udphdr))) {
		DEBUG(LOG_INFO, "message too short, ignoring");
		return -2;
	}
	
	if (bytes < ntohs(packet.ip.tot_len)) {
		DEBUG(LOG_INFO, "Truncated packet");
		return -2;
	}
	
	/* ignore any extra garbage bytes */
	bytes = ntohs(packet.ip.tot_len);
	
	/* Make sure its the right packet for us, and that it passes sanity checks */
	if (packet.ip.protocol != IPPROTO_UDP || packet.ip.version != IPVERSION ||
	    packet.ip.ihl != sizeof(packet.ip) >> 2 || packet.udp.dest != htons(CLIENT_PORT) ||
	    bytes > (int) sizeof(struct udp_dhcp_packet) ||
	    ntohs(packet.udp.len) != (short) (bytes - sizeof(packet.ip))) {
	    	DEBUG(LOG_INFO, "unrelated/bogus packet");
	    	return -2;
	}

	/* check IP checksum */
	check = packet.ip.check;
	packet.ip.check = 0;
	if (check != checksum(&(packet.ip), sizeof(packet.ip))) {
		DEBUG(LOG_INFO, "bad IP header checksum, ignoring");
		return -1;
	}
	
	/* verify the UDP checksum by replacing the header with a psuedo header */
	source = packet.ip.saddr;
	dest = packet.ip.daddr;
	check = packet.udp.check;
	packet.udp.check = 0;
	memset(&packet.ip, 0, sizeof(packet.ip));

	packet.ip.protocol = IPPROTO_UDP;
	packet.ip.saddr = source;
	packet.ip.daddr = dest;
	packet.ip.tot_len = packet.udp.len; /* cheat on the psuedo-header */
	if (check && check != checksum(&packet, bytes)) {
		DEBUG(LOG_ERR, "packet with bad UDP checksum received, ignoring");
		return -2;
	}
	
	memcpy(payload, &(packet.data), bytes - (sizeof(packet.ip) + sizeof(packet.udp)));
	
	if (ntohl(payload->cookie) != DHCP_MAGIC) {
		LOG(LOG_ERR, "received bogus message (bad magic) -- ignoring");
		return -2;
	}
	DEBUG(LOG_INFO, "oooooh!!! got some!");
	return bytes - (sizeof(packet.ip) + sizeof(packet.udp));
	
}

