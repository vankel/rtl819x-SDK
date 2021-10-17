//=============================================================================
// Copyright (c) 2013 Realtek Semiconductor Corporation.	All Rights Reserved.
//
//	Title:
//		Sigmautil.h
//	Desc:
//		Main Program for IEEE 802.11ac WFA Sigma Test
//=============================================================================

#ifndef _SIGMA_UTIL_H_
#define _SIGMA_UTIL_H_

#define MAXRSNIELEN				600
#define MACADDRLEN				6

#define DOT11_EVENT_HS2_TSM_REQ 115
#define DOT11_EVENT_QOS_MAP_CONF 119

#define SIOCGIWIND      		0x89ff

typedef struct _DOT11_BSS_SessInfo_URL{
        unsigned char   EventId;
        unsigned char   IsMoreEvent;
		unsigned char   macAddr[MACADDRLEN];
		unsigned char   SWT;
        unsigned char   URL[2048];
}DOT11_BSS_SessInfo_URL;

typedef struct _DOT11_QoSMAPConf{
        unsigned char   EventId;
        unsigned char   IsMoreEvent;
		unsigned char   macAddr[MACADDRLEN];
		unsigned char   indexQoSMAP;
}DOT11_QoSMAPConf;

#endif // _DISCOVER_H_
