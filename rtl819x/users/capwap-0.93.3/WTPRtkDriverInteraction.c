/*
 *  Realtek wlan driver interaction routines
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include "wireless_copy.h"
#include "CWWTP.h"

#ifdef DMALLOC
#include "../dmalloc-5.5.0/dmalloc.h"
#endif

#define RTL8192CD_IOCTL_SET_MIB			(SIOCDEVPRIVATE + 0x1)	
#define RTL8192CD_IOCTL_GET_MIB			(SIOCDEVPRIVATE + 0x2)	


/**************************** iwpriv ****************************/

/*--------------------------- Frequency ---------------------------*/
int set_freq(int sock, struct iwreq wrq, int value)
{
/*
	wrq.u.freq.m=value;		//in Ghz/10
	wrq.u.freq.e=1;		
	
      	if(ioctl(sock, SIOCSIWFREQ, &wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	printf("\nFrequenza impostata a: %d\n", wrq.u.freq.m);
*/
	return 1;
}

int get_freq(int sock, struct iwreq* wrq)
{
/*
      	if(ioctl(sock, SIOCGIWFREQ, wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	printf("\nFrequenza: %d\n", wrq->u.freq.m);
*/

	return 1;
}

/*--------------------------- Bit rate ---------------------------*/
int set_bitrate(int sock, struct iwreq wrq, int value)
{
/*
	wrq.u.bitrate.value=value;
	wrq.u.bitrate.fixed=1;
	
      	if(ioctl(sock, SIOCSIWRATE, &wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	printf("\nBit rate impostato a: %d\n", wrq.u.bitrate.value);
*/
	return 1;
}

int get_bitrate(int sock, struct iwreq* wrq)
{
/*
      	if(ioctl(sock, SIOCGIWRATE, wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	printf("\nBit rate: %d\n", wrq->u.bitrate.value);
*/
	return 1;
}

/*--------------------------- RTS/CTS Threshold ---------------------------*/
int set_rts_cts(int sock, struct iwreq wrq, int value)
{
/*
	if (value!=0) {wrq.u.rts.value=value;}
	else {wrq.u.rts.disabled=1;}	

      	if(ioctl(sock, SIOCSIWRTS, &wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	printf("\nRTS/CTS threshold impostato a: %d\n", wrq.u.rts.value);
*/
	return 1;
}

int get_rts_cts(int sock, struct iwreq* wrq)
{
/*
      	if(ioctl(sock, SIOCGIWRTS, wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	if (wrq->u.rts.disabled!=1) {printf("\nRTS/CTS threshold: %d\n", wrq->u.rts.value);}
	else {printf("\nRTS/CTS threshold off\n");}
*/
	return 1;
}

/*--------------------------- Fragmentation Threshold ---------------------------*/
int set_frag(int sock, struct iwreq wrq, int value)
{
/*
	if (value!=0) {wrq.u.frag.value=value;}
	else {wrq.u.frag.disabled=1;}	

      	if(ioctl(sock, SIOCSIWFRAG, &wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	printf("\nFragmentation threshold impostato a: %d\n", wrq.u.frag.value);
*/
	return 1;
}

int get_frag(int sock, struct iwreq* wrq)
{
/*
      	if(ioctl(sock, SIOCGIWFRAG, wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	if (wrq->u.frag.disabled!=1) {printf("\nFragmentation threshold: %d\n", wrq->u.frag.value);}
	else {printf("\nFragmentation threshold off\n");}
*/
	return 1;
}

/*--------------------------- Transmit Power ---------------------------*/
int set_txpower(int sock, struct iwreq wrq, int value)
{
/*
	wrq.u.txpower.value=value; 
	wrq.u.txpower.fixed=1;

      	if(ioctl(sock, SIOCSIWTXPOW, &wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	printf("\nTransmit power impostato a: %d\n", wrq.u.txpower.value);
*/
	return 1;
}

int get_txpower(int sock, struct iwreq* wrq)
{
/*
      	if(ioctl(sock, SIOCGIWTXPOW, wrq) < 0)
	{
		perror("Ioctl error");
		return(0);
	}	
	
	if (wrq->u.txpower.disabled!=1) {printf("\nTransmit power: %d\n", wrq->u.txpower.value);}
	else {printf("\nTransmit power off\n");}
*/
	return 1;
}


/*--------------------------- CWMIN ---------------------------*/
int set_cwmin(int sock, struct iwreq wrq, int acclass, int sta, int value)
{
	char buffer[64];

	if (acclass == 0)
		sprintf(buffer, "ap_beq_cwmin=%d", value);
	else if (acclass == 1)
			sprintf(buffer, "ap_bkq_cwmin=%d", value);
	else if (acclass == 2)
			sprintf(buffer, "ap_viq_cwmin=%d", value);
	else if (acclass == 3)
			sprintf(buffer, "ap_voq_cwmin=%d", value);
	
    wrq.u.data.pointer = (caddr_t)buffer;
    wrq.u.data.length = strlen(buffer) + 1;

	if(ioctl(sock, RTL8192CD_IOCTL_SET_MIB, &wrq) < 0) {	
		perror("Ioctl error");
		return(0);
	}		

	return 1;
}

int get_cwmin(int sock, struct iwreq *wrq, int acclass, int sta)
{
	char buffer[64];

	if (acclass == 0)
		strcpy(buffer, "ap_beq_cwmin");
	else if (acclass == 1)
			strcpy(buffer, "ap_bkq_cwmin");
	else if (acclass == 2)
			strcpy(buffer, "ap_viq_cwmin");
	else if (acclass == 3)
			strcpy(buffer, "ap_voq_cwmin");
	
    wrq->u.data.pointer = (caddr_t)buffer;
    wrq->u.data.length = strlen(buffer) + 1;

	if(ioctl(sock, RTL8192CD_IOCTL_GET_MIB, wrq) < 0) {	
		perror("Ioctl error");
		return(0);
	}		
	memcpy(&wrq->u.param.value, buffer, sizeof(int));

	return 1;
}

/*--------------------------- CWMAX ---------------------------*/
int set_cwmax(int sock, struct iwreq wrq, int acclass, int sta, int value)
{
	char buffer[64];

	if (acclass == 0)
		sprintf(buffer, "ap_beq_cwmax=%d", value);
	else if (acclass == 1)
			sprintf(buffer, "ap_bkq_cwmax=%d", value);
	else if (acclass == 2)
			sprintf(buffer, "ap_viq_cwmax=%d", value);
	else if (acclass == 3)
			sprintf(buffer, "ap_voq_cwmax=%d", value);
	
    wrq.u.data.pointer = (caddr_t)buffer;
    wrq.u.data.length = strlen(buffer) + 1;

	if(ioctl(sock, RTL8192CD_IOCTL_SET_MIB, &wrq) < 0) {	
		perror("Ioctl error");
		return(0);
	}		

	return 1;
}

int get_cwmax(int sock, struct iwreq *wrq, int acclass, int sta)
{
	char buffer[64];

	if (acclass == 0)
		strcpy(buffer, "ap_beq_cwmax");
	else if (acclass == 1)
			strcpy(buffer, "ap_bkq_cwmax");
	else if (acclass == 2)
			strcpy(buffer, "ap_viq_cwmax");
	else if (acclass == 3)
			strcpy(buffer, "ap_voq_cwmax");
	
    wrq->u.data.pointer = (caddr_t)buffer;
    wrq->u.data.length = strlen(buffer) + 1;

	if(ioctl(sock, RTL8192CD_IOCTL_GET_MIB, wrq) < 0) {	
		perror("Ioctl error");
		return(0);
	}		
	memcpy(&wrq->u.param.value, buffer, sizeof(int));

	return 1;
}

/*--------------------------- AIFS ---------------------------*/
int set_aifs(int sock, struct iwreq wrq, int acclass, int sta, int value)
{
	char buffer[64];

	if (acclass == 0)
		sprintf(buffer, "ap_beq_aifsn=%d", value);
	else if (acclass == 1)
			sprintf(buffer, "ap_bkq_aifsn=%d", value);
	else if (acclass == 2)
			sprintf(buffer, "ap_viq_aifsn=%d", value);
	else if (acclass == 3)
			sprintf(buffer, "ap_voq_aifsn=%d", value);
	
    wrq.u.data.pointer = (caddr_t)buffer;
    wrq.u.data.length = strlen(buffer) + 1;

	if(ioctl(sock, RTL8192CD_IOCTL_SET_MIB, &wrq) < 0) {	
		perror("Ioctl error");
		return(0);
	}
	
	return 1;
}

int get_aifs(int sock, struct iwreq *wrq, int acclass, int sta)
{
	char buffer[64];

	if (acclass == 0)
		strcpy(buffer, "ap_beq_aifsn");
	else if (acclass == 1)
			strcpy(buffer, "ap_bkq_aifsn");
	else if (acclass == 2)
			strcpy(buffer, "ap_viq_aifsn");
	else if (acclass == 3)
			strcpy(buffer, "ap_voq_aifsn");
	
    wrq->u.data.pointer = (caddr_t)buffer;
    wrq->u.data.length = strlen(buffer) + 1;

	if(ioctl(sock, RTL8192CD_IOCTL_GET_MIB, wrq) < 0) {	
		perror("Ioctl error");
		return(0);
	}		

	memcpy(&wrq->u.param.value, buffer, sizeof(int));

	return 1;
}

/*--------------------------- manual EDCA  ---------------------------*/
int set_edca_flag(int sock, struct iwreq wrq, int value)
{
	char buffer[64];

	sprintf(buffer, "manual_edca=%d", value);	
    wrq.u.data.pointer = (caddr_t)buffer;
    wrq.u.data.length = strlen(buffer) + 1;

	if(ioctl(sock, RTL8192CD_IOCTL_SET_MIB, &wrq) < 0) {	
		perror("Ioctl error");
		return(0);
	}		
	
	return 1;
}

int get_edca_flag(int sock, struct iwreq *wrq)
{
	char buffer[64];

	strcpy(buffer, "manual_edca");	
    wrq->u.data.pointer = (caddr_t)buffer;
    wrq->u.data.length = strlen(buffer) + 1;

	if(ioctl(sock, RTL8192CD_IOCTL_GET_MIB, wrq) < 0) {	
		perror("Ioctl error");
		return(0);
	}		

	memcpy(&wrq->u.param.value, buffer, sizeof(int));

	return 1;
}

