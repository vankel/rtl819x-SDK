#ifndef _SITE_SURVEY_H
#define _SITE_SURVEY_H

#define SIOCGIWRTLSCANREQ		0x8B33	// scan request
#define SIOCGIWRTLGETBSSDB		0x8B34	// get bss data base

#define SSID_LEN	32
#define MESHID_LEN 32 
#define	MAX_BSS_DESC	64



typedef enum _BssType {
    infrastructure = 1,
    independent = 2,
} BssType;

typedef enum _Capability {
    cESS 		= 0x01,
    cIBSS		= 0x02,
    cPollable		= 0x04,
    cPollReq		= 0x01,
    cPrivacy		= 0x10,
    cShortPreamble	= 0x20,
} Capability;

typedef	struct _IbssParms {
    unsigned short	atimWin;
} IbssParms;

typedef struct _OCTET_STRING {
    unsigned char *Octet;
    unsigned short Length;
} OCTET_STRING;

typedef struct _BssDscr {
       unsigned char bdBssId[6];
    unsigned char bdSsIdBuf[SSID_LEN];
    OCTET_STRING  bdSsId;


	//by GANTOE for site survey 2008/12/26
	unsigned char bdMeshIdBuf[MESHID_LEN]; 
	OCTET_STRING bdMeshId; 

    BssType bdType;
    unsigned short bdBcnPer;			// beacon period in Time Units
    unsigned char bdDtimPer;			// DTIM period in beacon periods
    unsigned long bdTstamp[2];			// 8 Octets from ProbeRsp/Beacon
    IbssParms bdIbssParms;			// empty if infrastructure BSS
    unsigned short bdCap;				// capability information
    unsigned char ChannelNumber;			// channel number
    unsigned long bdBrates;
    unsigned long bdSupportRates;		
    unsigned char bdsa[6];			// SA address
    unsigned char rssi, sq;			// RSSI and signal strength
    unsigned char network;			// 1: 11B, 2: 11G, 4:11G
	// P2P_SUPPORT
	unsigned char	p2pdevname[33];		
	unsigned char	p2prole;	
	unsigned short	p2pwscconfig;		
	unsigned char	p2paddress[6];	
} BssDscr, *pBssDscr;


typedef struct _sitesurvey_status {
    unsigned char number;
    unsigned char pad[3];
    BssDscr bssdb[MAX_BSS_DESC];
} SS_STATUS_T, *SS_STATUS_Tp;

int getWlSiteSurveyRequest(char *interface, int *pStatus);
int getWlSiteSurveyResult(char *interface, SS_STATUS_Tp pStatus );
void get_ap_mode(unsigned char network,char *tmp1Buf);
void get_ap_encrypt(BssDscr *pBss,char *tmp2Buf);




#endif
