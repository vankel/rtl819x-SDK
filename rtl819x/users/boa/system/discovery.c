/***********************************************************************
*
* discovery.c
*
* Perform PPPoE discovery
*
* Copyright (C) 1999 by Roaring Penguin Software Inc.
*
***********************************************************************/

static char const RCSID[] =
"$Id: discovery.c,v 1.1.2.1 2008/09/22 01:58:14 ella Exp $";

#include "pppoe.h"

#ifdef HAVE_SYSLOG_H
#include <syslog.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <errno.h>
#ifdef CONFIG_AUTO_DHCP_CHECK
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif

#ifdef HAVE_SYS_UIO_H
#include <sys/uio.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef USE_LINUX_PACKET
#include <sys/ioctl.h>
#include <fcntl.h>
#endif

#include <signal.h>

#include <net/ethernet.h>
#include <sys/types.h>
#include <sys/socket.h>

void rp_fatal(char const *str)
{

}

void fatalSys(char const *str)
{
 rp_fatal(str);

}

void sysErr(char const *str)
{
 	rp_fatal(str);
}


/**********************************************************************
*%FUNCTION: strDup
*%ARGUMENTS:
* str -- string to copy
*%RETURNS:
* A malloc'd copy of str.  Exits if malloc fails.
***********************************************************************/
char *
strDup(char const *str)
{
    char *copy = malloc(strlen(str)+1);
    if (!copy) {
	rp_fatal("strdup failed");
    }
    strcpy(copy, str);
    return copy;
}





/**********************************************************************
*%FUNCTION: parseForHostUniq
*%ARGUMENTS:
* type -- tag type
* len -- tag length
* data -- tag data.
* extra -- user-supplied pointer.  This is assumed to be a pointer to int.
*%RETURNS:
* Nothing
*%DESCRIPTION:
* If a HostUnique tag is found which matches our PID, sets *extra to 1.
***********************************************************************/
void
parseForHostUniq(UINT16_t type, UINT16_t len, unsigned char *data,
		 void *extra)
{
    int *val = (int *) extra;
    if (type == TAG_HOST_UNIQ && len == sizeof(pid_t)) {
	pid_t tmp;
	memcpy(&tmp, data, len);
	if (tmp == getpid()) {
	    *val = 1;
	}
    }
}
/**********************************************************************
*%FUNCTION: parsePacket
*%ARGUMENTS:
* packet -- the PPPoE discovery packet to parse
* func -- function called for each tag in the packet
* extra -- an opaque data pointer supplied to parsing function
*%RETURNS:
* 0 if everything went well; -1 if there was an error
*%DESCRIPTION:
* Parses a PPPoE discovery packet, calling "func" for each tag in the packet.
* "func" is passed the additional argument "extra".
***********************************************************************/
int
parsePacket(PPPoEPacket *packet, ParseFunc *func, void *extra)
{
    UINT16_t len = ntohs(packet->length);
    unsigned char *curTag;
    UINT16_t tagType, tagLen;

    if (packet->ver != 1) {
	syslog(LOG_ERR, "Invalid PPPoE version (%d)", (int) packet->ver);
	return -1;
    }
    if (packet->type != 1) {
	syslog(LOG_ERR, "Invalid PPPoE type (%d)", (int) packet->type);
	return -1;
    }

    /* Do some sanity checks on packet */
    if (len > ETH_DATA_LEN - 6) { /* 6-byte overhead for PPPoE header */
	syslog(LOG_ERR, "Invalid PPPoE packet length (%u)", len);
	return -1;
    }

    /* Step through the tags */
    curTag = packet->payload;
    while(curTag - packet->payload < len) {
	/* Alignment is not guaranteed, so do this by hand... */
	tagType = (((UINT16_t) curTag[0]) << 8) +
	    (UINT16_t) curTag[1];
	tagLen = (((UINT16_t) curTag[2]) << 8) +
	    (UINT16_t) curTag[3];
	if (tagType == TAG_END_OF_LIST) {
	    return 0;
	}
	if ((curTag - packet->payload) + tagLen + TAG_HDR_SIZE > len) {
	    syslog(LOG_ERR, "Invalid PPPoE tag length (%u)", tagLen);
	    return -1;
	}
	func(tagType, tagLen, curTag+TAG_HDR_SIZE, extra);
	curTag = curTag + TAG_HDR_SIZE + tagLen;
    }
    return 0;
}


/**********************************************************************
*%FUNCTION: packetIsForMe
*%ARGUMENTS:
* conn -- PPPoE connection info
* packet -- a received PPPoE packet
*%RETURNS:
* 1 if packet is for this PPPoE daemon; 0 otherwise.
*%DESCRIPTION:
* If we are using the Host-Unique tag, verifies that packet contains
* our unique identifier.
***********************************************************************/
int
packetIsForMe(PPPoEConnection *conn, PPPoEPacket *packet)
{
    int forMe = 0;

    /* If packet is not directed to our MAC address, forget it */
    if (memcmp(packet->ethHdr.h_dest, conn->myEth, ETH_ALEN)) return 0;

    /* If we're not using the Host-Unique tag, then accept the packet */
    if (!conn->useHostUniq) return 1;

    parsePacket(packet, parseForHostUniq, &forMe);
    return forMe;
}

/**********************************************************************
*%FUNCTION: parsePADOTags
*%ARGUMENTS:
* type -- tag type
* len -- tag length
* data -- tag data
* extra -- extra user data.  Should point to a PacketCriteria structure
*          which gets filled in according to selected AC name and service
*          name.
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Picks interesting tags out of a PADO packet
***********************************************************************/
void
parsePADOTags(UINT16_t type, UINT16_t len, unsigned char *data,
	      void *extra)
{
    struct PacketCriteria *pc = (struct PacketCriteria *) extra;
    PPPoEConnection *conn = pc->conn;
    int i;

    switch(type) {
    case TAG_AC_NAME:
	pc->seenACName = 1;
	if (conn->printACNames) {
	    printf("Access-Concentrator: %.*s\n", (int) len, data);
	}
	if (conn->acName && len == strlen(conn->acName) &&
	    !strncmp((char *) data, conn->acName, len)) {
	    pc->acNameOK = 1;
	}
	break;
    case TAG_SERVICE_NAME:
	pc->seenServiceName = 1;
	if (conn->printACNames && len > 0) {
	    printf("       Service-Name: %.*s\n", (int) len, data);
	}
	if (conn->serviceName && len == strlen(conn->serviceName) &&
	    !strncmp((char *) data, conn->serviceName, len)) {
	    pc->serviceNameOK = 1;
	}
	break;
    case TAG_AC_COOKIE:
	if (conn->printACNames) {
	    printf("Got a cookie:");
	    /* Print first 20 bytes of cookie */
	    for (i=0; i<len && i < 20; i++) {
		printf(" %02x", (unsigned) data[i]);
	    }
	    if (i < len) printf("...");
	    printf("\n");
	}
	conn->cookie.type = htons(type);
	conn->cookie.length = htons(len);
	memcpy(conn->cookie.payload, data, len);
	break;
    case TAG_RELAY_SESSION_ID:
	if (conn->printACNames) {
	    printf("Got a Relay-ID:");
	    /* Print first 20 bytes of relay ID */
	    for (i=0; i<len && i < 20; i++) {
		printf(" %02x", (unsigned) data[i]);
	    }
	    if (i < len) printf("...");
	    printf("\n");
	}
	conn->relayId.type = htons(type);
	conn->relayId.length = htons(len);
	memcpy(conn->relayId.payload, data, len);
	break;
    case TAG_SERVICE_NAME_ERROR:
	if (conn->printACNames) {
	    printf("Got a Service-Name-Error tag: %.*s\n", (int) len, data);
	} else {
	    syslog(LOG_ERR, "PADO: Service-Name-Error: %.*s", (int) len, data);
	    exit(1);
	}
	break;
    case TAG_AC_SYSTEM_ERROR:
	if (conn->printACNames) {
	    printf("Got a System-Error tag: %.*s\n", (int) len, data);
	} else {
	    syslog(LOG_ERR, "PADO: System-Error: %.*s", (int) len, data);
	    exit(1);
	}
	break;
    case TAG_GENERIC_ERROR:
	if (conn->printACNames) {
	    printf("Got a Generic-Error tag: %.*s\n", (int) len, data);
	} else {
	    syslog(LOG_ERR, "PADO: Generic-Error: %.*s", (int) len, data);
	    exit(1);
	}
	break;
    }
}

/***********************************************************************
*%FUNCTION: sendPADI
*%ARGUMENTS:
* conn -- PPPoEConnection structure
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Sends a PADI packet
***********************************************************************/
void
sendPADI(PPPoEConnection *conn)
{
    PPPoEPacket packet;
    unsigned char *cursor = packet.payload;
    PPPoETag *svc = (PPPoETag *) (&packet.payload);
    UINT16_t namelen = 0;
    UINT16_t plen;

    if (conn->serviceName) {
	namelen = (UINT16_t) strlen(conn->serviceName);
    }
    plen = TAG_HDR_SIZE + namelen;
    CHECK_ROOM(cursor, packet.payload, plen);

    /* Set destination to Ethernet broadcast address */
    memset(packet.ethHdr.h_dest, 0xFF, ETH_ALEN);
    memcpy(packet.ethHdr.h_source, conn->myEth, ETH_ALEN);

    packet.ethHdr.h_proto = htons(Eth_PPPOE_Discovery);
    packet.ver = 1;
    packet.type = 1;
    packet.code = CODE_PADI;
    packet.session = 0;

    svc->type = TAG_SERVICE_NAME;
    svc->length = htons(namelen);
    CHECK_ROOM(cursor, packet.payload, namelen+TAG_HDR_SIZE);

    if (conn->serviceName) {
	memcpy(svc->payload, conn->serviceName, strlen(conn->serviceName));
    }
    cursor += namelen + TAG_HDR_SIZE;

    /* If we're using Host-Uniq, copy it over */
    if (conn->useHostUniq) {
	PPPoETag hostUniq;
	pid_t pid = getpid();
	hostUniq.type = htons(TAG_HOST_UNIQ);
	hostUniq.length = htons(sizeof(pid));
	memcpy(hostUniq.payload, &pid, sizeof(pid));
	CHECK_ROOM(cursor, packet.payload, sizeof(pid) + TAG_HDR_SIZE);
	memcpy(cursor, &hostUniq, sizeof(pid) + TAG_HDR_SIZE);
	cursor += sizeof(pid) + TAG_HDR_SIZE;
	plen += sizeof(pid) + TAG_HDR_SIZE;
    }

    packet.length = htons(plen);

    sendPacket(conn, conn->discoverySocket, &packet, (int) (plen + HDR_SIZE));

}

/**********************************************************************
*%FUNCTION: waitForPADO
*%ARGUMENTS:
* conn -- PPPoEConnection structure
* timeout -- how long to wait (in seconds)
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Waits for a PADO packet and copies useful information
***********************************************************************/
void
waitForPADO(PPPoEConnection *conn, int timeout)
{
    fd_set readable;
    int r;
    struct timeval tv;
    PPPoEPacket packet;
    int len;

    struct PacketCriteria pc;
    pc.conn          = conn;
    pc.acNameOK      = (conn->acName)      ? 0 : 1;
    pc.serviceNameOK = (conn->serviceName) ? 0 : 1;
    pc.seenACName    = 0;
    pc.seenServiceName = 0;
	
    do {
	if (BPF_BUFFER_IS_EMPTY) {
	    tv.tv_sec = timeout;
	    tv.tv_usec = 0;
	
	    FD_ZERO(&readable);
	    FD_SET(conn->discoverySocket, &readable);

	    while(1) {
		r = select(conn->discoverySocket+1, &readable, NULL, NULL, &tv);
		if (r >= 0 || errno != EINTR) break;
	    }
	    if (r < 0) {
		fatalSys("select (waitForPADO)");
	    }
	    if (r == 0) return;        /* Timed out */
	}
	
	/* Get the packet */
	receivePacket(conn->discoverySocket, &packet, &len);

	/* Check length */
	if (ntohs(packet.length) + HDR_SIZE > len) {
	    syslog(LOG_ERR, "Bogus PPPoE length field (%u)",
		   (unsigned int) ntohs(packet.length));
	    continue;
	}

#ifdef USE_BPF
	/* If it's not a Discovery packet, loop again */
	if (etherType(&packet) != Eth_PPPOE_Discovery) continue;
#endif

	/* If it's not for us, loop again */
	if (!packetIsForMe(conn, &packet)) continue;

	if (packet.code == CODE_PADO) {
	    if (NOT_UNICAST(packet.ethHdr.h_source)) {
		printf("Ignoring PADO packet from non-unicast MAC address");
		continue;
	    }
	    parsePacket(&packet, parsePADOTags, &pc);
	    if (!pc.seenACName) {
		printf("Ignoring PADO packet with no AC-Name tag");
		continue;
	    }
	    if (!pc.seenServiceName) {
		printf("Ignoring PADO packet with no Service-Name tag");
		continue;
	    }
	    conn->numPADOs++;
	    if (conn->printACNames) {
		printf("--------------------------------------------------\n");
	    }
	    if (pc.acNameOK && pc.serviceNameOK) {
		memcpy(conn->peerEth, packet.ethHdr.h_source, ETH_ALEN);
		if (conn->printACNames) {
		    printf("AC-Ethernet-Address: %02x:%02x:%02x:%02x:%02x:%02x\n",
			   (unsigned) conn->peerEth[0], 
			   (unsigned) conn->peerEth[1],
			   (unsigned) conn->peerEth[2],
			   (unsigned) conn->peerEth[3],
			   (unsigned) conn->peerEth[4],
			   (unsigned) conn->peerEth[5]);
		    continue;
		}
		conn->discoveryState = STATE_RECEIVED_PADO;
		break;
	    }
	}
    } while (conn->discoveryState != STATE_RECEIVED_PADO);
}

/**********************************************************************
*%FUNCTION: discovery
*%ARGUMENTS:
* conn -- PPPoE connection info structure
*%RETURNS:
* Nothing
*%DESCRIPTION:
* Performs the PPPoE discovery phase
***********************************************************************/


int 
discovery(PPPoEConnection *conn)
{
    int padiAttempts = 0;
    int padrAttempts = 0;
    int timeout = 1;
	unsigned char buf_myeth[10];
	unsigned char buf_peereth[10];
	char Tmp[512];

	FILE * fd = NULL;

	memset(Tmp, 0, sizeof(Tmp));
	memset(buf_myeth, 0, sizeof(buf_myeth));
	memset(buf_peereth, 0, sizeof(buf_peereth));
    
	conn->discoverySocket =openInterface(conn->ifName, Eth_PPPOE_Discovery, conn->myEth);
	do {
		padiAttempts++;

		if (padiAttempts > 1) {
			return -1;
		}
		
	sendPADI(conn);
	usleep(50000);
	sendPADI(conn);
	usleep(50000);
	sendPADI(conn);
	conn->discoveryState = STATE_SENT_PADI;
	usleep(50000);
	waitForPADO(conn, timeout);
    
	} while (conn->discoveryState == STATE_SENT_PADI);
    return 1;
}

int  discovery_ppp(char *wan_if)
{
	int i, ses;
	int ret = -1;

	PPPoEConnection *conn =malloc(sizeof(PPPoEConnection)) ;
	memset(conn, 0, sizeof(PPPoEConnection));
	
	SET_STRING(conn->ifName,wan_if);
	conn->discoverySocket = -1;
  	conn->sessionSocket = -1;
	ret = discovery(conn);
	free(conn->ifName);
	free(conn);
	return ret;
}
#ifdef CONFIG_AUTO_DHCP_CHECK
u_int16_t checksum(void *addr, int count)
{
	/* Compute Internet Checksum for "count" bytes
	 *         beginning at location "addr".
	 */
	register int32_t sum = 0;
	u_int16_t *source = (u_int16_t *) addr;

	while (count > 1)  {
		/*  This is the inner loop */
		sum += *source++;
		count -= 2;
	}

	/*  Add left-over byte, if any */
	if (count > 0) {
		/* Make sure that the left-over byte is added correctly both
		 * with little and big endian hosts */
		u_int16_t tmp = 0;
		*(unsigned char *) (&tmp) = * (unsigned char *) source;
		sum += tmp;
	}
	/*  Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}
int waitForOffer(int sock_fd, int timeout, unsigned char *myEth)
{
	fd_set readable;
	int r;
	struct timeval tv;
	int len;
	struct dhcpPacket dhcpPkt; 
	
	while(1)
	{
		memset(&dhcpPkt, 0, sizeof(struct dhcpPacket));//clean up dhcpPkt
		
		tv.tv_sec = timeout;
		tv.tv_usec = 0;

		FD_ZERO(&readable);
		FD_SET(sock_fd, &readable);

		while(1) 
		{
			r = select(sock_fd+1, &readable, NULL, NULL, &tv);
			if (r >= 0 || errno != EINTR) 
			{
				//printf("%s:%d####\n",__FUNCTION__,__LINE__);
				break;
			}
		}
		
		if (r < 0) 		
			return -1;
		if (r == 0) 
			return 0;        /* Timed out */

		/* Get the packet */
		if ((len = recv(sock_fd, &dhcpPkt, sizeof(dhcpPkt), 0)) < 0)
		{
			//printf("%s:%d####\n",__FUNCTION__,__LINE__);
			return -1;
		}		

		/* If it's not for us, loop again */
		if(memcmp(dhcpPkt.data.chaddr, myEth, 6))
		{
			//printf("%s:%d####client mac does not match!\n",__FUNCTION__,__LINE__);
			continue;
		}

		if(ntohl(dhcpPkt.data.cookie)!=DHCP_MAGIC)	
		{
			//printf("%s:%d####cookie does not match!\n",__FUNCTION__,__LINE__);
			continue;
		}

		if(dhcpPkt.data.options[0]==0x35 && dhcpPkt.data.options[1]==1 && dhcpPkt.data.options[2]==DHCPOFFER)
		{	
			return 1;
		}
	}
	return 1;
}
int createSock(char *wan_if, unsigned char *myEth, struct sockaddr_ll *ret_dest)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_ll dest;

	if ((fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP))) < 0)
		return -1;

	if (myEth) 
	{
		strncpy(ifr.ifr_name, wan_if, sizeof(ifr.ifr_name));
		if (ioctl(fd, SIOCGIFHWADDR, &ifr) < 0) 
	    		return -1;

		memcpy(myEth, ifr.ifr_hwaddr.sa_data, ETH_ALEN);	
    	}
	
	memset(&dest, 0, sizeof(dest));	
	
	dest.sll_family = AF_PACKET;
	dest.sll_protocol = htons(ETH_P_IP);

	strncpy(ifr.ifr_name, wan_if, sizeof(ifr.ifr_name));
	if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0) 
		return -1;
		
	dest.sll_ifindex = ifr.ifr_ifindex;
	dest.sll_halen = 6;
	
	memcpy(dest.sll_addr, MAC_BCAST_ADDR, 6);

	memcpy(ret_dest, &dest, sizeof(struct sockaddr_ll));
	
	if (bind(fd, (struct sockaddr *)&dest, sizeof(struct sockaddr_ll)) < 0) 
	{		
		//close(fd);
		return -1;
	}	
	//close(fd);
	return fd;
}
int sendDiscovery(int sock_fd, struct dhcpPacket *dhcpPkt, unsigned char *myEth, struct sockaddr_ll *dest)
{
	int padiAttempts = 0;
	int timeout = 1;
	int retval;
	while (1)
	{
		padiAttempts++;
		if (padiAttempts > 2) 
			return -1;
		
		//printf("%s:%d####\n",__FUNCTION__,__LINE__);		
		
		if (sendto(sock_fd, dhcpPkt, sizeof(struct dhcpPacket), 0, (struct sockaddr *)dest, sizeof(*dest)) < 0) 
		{
			printf("%s:%d##send DHCP discovery fail!\n",__FUNCTION__,__LINE__);
			return -1;
		}
		//printf("%s:%d####\n",__FUNCTION__,__LINE__);
		
		retval=waitForOffer(sock_fd, timeout, myEth);
	
		if(retval<=0 && padiAttempts>1)
		{
			//printf("%s:%d####\n",__FUNCTION__,__LINE__);
			return -1;  
		}
		
		if(retval==1)
		{	
			return 1;
		}
	} 
    	return 1;
}
void init_dhcp_discovery_pkt(struct dhcpMessage *dhcpMsg, struct dhcpPacket *dhcpPkt, unsigned char *myEth)
{	
	dhcpMsg->op = BOOTREQUEST;	
	dhcpMsg->htype = ETH_10MB;
	dhcpMsg->hlen = ETH_10MB_LEN;
	dhcpMsg->cookie = htonl(DHCP_MAGIC);

	dhcpMsg->options[0]=DHCP_MESSAGE_TYPE;
	dhcpMsg->options[1]=1;
	dhcpMsg->options[2]=DHCPDISCOVER;
	
	dhcpMsg->options[3] = DHCP_END;
	
	memcpy(dhcpMsg->chaddr, myEth, 6);
	
	dhcpMsg->flags |= BROADCAST_FLAG;
	dhcpMsg->xid=rand();

	//memcpy(dhcpPkt->ethHdr.h_dest, MAC_BCAST_ADDR, 6);
	//memcpy(dhcpPkt->ethHdr.h_source, myEth, 6);
	//dhcpPkt->ethHdr.h_proto=htons(ETH_P_IP);

	dhcpPkt->ip.protocol = IPPROTO_UDP;
	dhcpPkt->ip.saddr = INADDR_ANY;
	dhcpPkt->ip.daddr = INADDR_BROADCAST;
	dhcpPkt->udp.source = htons(CLIENT_PORT);
	dhcpPkt->udp.dest = htons(SERVER_PORT);
	dhcpPkt->udp.len = htons(sizeof(dhcpPkt->udp) + sizeof(struct dhcpMessage)); 
	dhcpPkt->ip.tot_len = dhcpPkt->udp.len;
	
	memcpy(&(dhcpPkt->data), dhcpMsg, sizeof(struct dhcpMessage));
	
	dhcpPkt->udp.check = checksum(dhcpPkt, sizeof(struct dhcpPacket));
	
	dhcpPkt->ip.tot_len = htons(sizeof(struct dhcpPacket));
	dhcpPkt->ip.ihl = sizeof(dhcpPkt->ip) >> 2;
	dhcpPkt->ip.version = IPVERSION;
	dhcpPkt->ip.ttl = IPDEFTTL;
	dhcpPkt->ip.check = checksum(&(dhcpPkt->ip), sizeof(dhcpPkt->ip));
}
int discovery_dhcp(char *wan_if)
{
	int retval;
	struct dhcpMessage dhcpMsg;
	struct dhcpPacket dhcpPkt;
	memset(&dhcpMsg, 0, sizeof(struct dhcpMessage));
	memset(&dhcpPkt, 0, sizeof(struct dhcpPacket));

	int sock_fd;
	unsigned char myEth[ETH_ALEN];
	
	struct sockaddr_ll ret_dest;
	memset(&ret_dest, 0, sizeof(struct sockaddr_ll));
	
	sock_fd=createSock(wan_if, myEth, &ret_dest);
	
	init_dhcp_discovery_pkt(&dhcpMsg, &dhcpPkt, myEth);

	retval=sendDiscovery(sock_fd, &dhcpPkt, myEth, &ret_dest);

	close(sock_fd);
	return retval;	
}
#endif
