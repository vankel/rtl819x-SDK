#ifndef __WAPITEST_H__
#define __WAPITESH_H__

/* ------------- IOCTL STUFF FOR 802.1x DAEMON--------------------- */
#define SIOCGIWIND	0x89ff	
#define SIOCSWAPIPID     0x8b3f
#define MAXDATALEN      1560	// jimmylin: org:256, enlarge for pass EAP packet by event queue
#define CONFIG_RTL_WAPI_SUPPORT
typedef enum{
        DOT11_EVENT_NO_EVENT = 1,
        DOT11_EVENT_REQUEST = 2,
        DOT11_EVENT_ASSOCIATION_IND = 3,
        DOT11_EVENT_ASSOCIATION_RSP = 4,
        DOT11_EVENT_AUTHENTICATION_IND = 5,
        DOT11_EVENT_REAUTHENTICATION_IND = 6,
        DOT11_EVENT_DEAUTHENTICATION_IND = 7,
        DOT11_EVENT_DISASSOCIATION_IND = 8,
        DOT11_EVENT_DISCONNECT_REQ = 9,
        DOT11_EVENT_SET_802DOT11 = 10,
        DOT11_EVENT_SET_KEY = 11,
        DOT11_EVENT_SET_PORT = 12,
        DOT11_EVENT_DELETE_KEY = 13,
        DOT11_EVENT_SET_RSNIE = 14,
        DOT11_EVENT_GKEY_TSC = 15,
        DOT11_EVENT_MIC_FAILURE = 16,
        DOT11_EVENT_ASSOCIATION_INFO = 17,
        DOT11_EVENT_INIT_QUEUE = 18,
        DOT11_EVENT_EAPOLSTART = 19,

        DOT11_EVENT_ACC_SET_EXPIREDTIME = 31,
        DOT11_EVENT_ACC_QUERY_STATS = 32,
        DOT11_EVENT_ACC_QUERY_STATS_ALL = 33,
        DOT11_EVENT_REASSOCIATION_IND = 34,
        DOT11_EVENT_REASSOCIATION_RSP = 35,
        DOT11_EVENT_STA_QUERY_BSSID = 36,
        DOT11_EVENT_STA_QUERY_SSID = 37,
        DOT11_EVENT_EAP_PACKET = 41,

#ifdef RTL_WPA2_PREAUTH
        DOT11_EVENT_EAPOLSTART_PREAUTH = 45,
        DOT11_EVENT_EAP_PACKET_PREAUTH = 46,
#endif

        DOT11_EVENT_WPA2_MULTICAST_CIPHER = 47,
        DOT11_EVENT_WPA_MULTICAST_CIPHER = 48,

#ifdef WIFI_SIMPLE_CONFIG
		DOT11_EVENT_WSC_SET_IE = 55,
		DOT11_EVENT_WSC_PROBE_REQ_IND = 56,
		DOT11_EVENT_WSC_PIN_IND = 57,
		DOT11_EVENT_WSC_ASSOC_REQ_IE_IND = 58,
#endif
#ifdef	_DOT11_MESH_MODE_ 
	DOT11_EVENT_PATHSEL_GEN_RREQ = 59, 
	DOT11_EVENT_PATHSEL_GEN_RERR = 60,
	DOT11_EVENT_PATHSEL_RECV_RREQ = 61,                    
	DOT11_EVENT_PATHSEL_RECV_RREP = 62,                   
	DOT11_EVENT_PATHSEL_RECV_RERR = 63,
	DOT11_EVENT_PATHSEL_RECV_RREP_ACK = 64,
	DOT11_EVENT_PATHSEL_RECV_PANN = 65,
	DOT11_EVENT_PATHSEL_RECV_RANN = 66,
#endif // _DOT11_MESH_MODE_
#ifdef CONFIG_RTL_WAPI_SUPPORT
	DOT11_EVENT_WAPI_INIT_QUEUE =67,
	DOT11_EVENT_WAPI_READ_QUEUE = 68,
	DOT11_EVENT_WAPI_WRITE_QUEUE  =69
#endif
} DOT11_EVENT;

typedef struct _DOT11_REQUEST{
        unsigned char   EventId;
}DOT11_REQUEST;

typedef struct _my_test {
	unsigned char eventID;
	unsigned char moreEvent;
	unsigned long seq;
} MY_TEST,*MY_TESTp;

//communication between wapi driver and application
typedef	struct _commParams {
	unsigned char			eventID;
	unsigned char			moreData;
	unsigned short		type;
	void 				*ptr;
	char			name[IFNAMSIZ];
	unsigned char			data[0];
}COMM_PARAMS, *COMM_PARAMSp;

#define WAPI_COMM_PARAM_SIZE (8+IFNAMSIZ)

//Application to driver
#define	WAPI_IOCTL_TYPE_ACTIVEAUTH	0
#define	WAPI_IOCTL_TYPE_SETBK		1
#define	WAPI_IOCTL_TYPE_AUTHRSP		2

//Driver to application
#define	WAPI_IOCTL_TYPE_REQ_ACTIVE	3
#define	WAPI_IOCTL_TYPE_CA_AUTH		4

//void InitEvent(int sig_no);
//void ProcessEvent(int sig_no);
//void WriteEvent(int sig_no);

#endif
