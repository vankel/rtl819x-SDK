
#ifndef RTK_API360 //for WiFi driver to include this file

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <linux/genetlink.h>
#include <sys/socket.h>

#endif

#define MAX_PAYLOAD 1024 /* maximum payload size*/

#define NETLINK_RTK_EVENTD 26
#define RTK_EVENTD_PID_FILE "/var/run/rtk_eventd.pid"

typedef struct __rtkEventHdr 
{
	int eventID;	
	char name[16];
	unsigned char data[0];
}rtkEventHdr;

#define RTK_EVENTD_HDR_LEN sizeof(rtkEventHdr)

enum RTK_EVENT_ID
{
	WIFI_USER_PASSWD_ERROR=1,
	WIRE_PLUG_OFF,
	WIRE_PLUG_ON,
	SYSTEM_RESET,
	NTP_UPDATE_SUCCESS,
	START_WIFI_SERVICE,
	START_WAN_SUCCESS,
	PPPOE_DIAL_SUCCESS,
	START_DHCP_SERVICE,
	RESTORE_AP_INIT_STATE	
};

































