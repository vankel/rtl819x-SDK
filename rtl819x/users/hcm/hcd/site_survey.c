#include <stdio.h> 
#include <stdlib.h> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <linux/wireless.h>
#include "site_survey.h"
#include "apmib.h"


static inline int
iw_get_ext(int                  skfd,           /* Socket to the kernel */
           char *               ifname,         /* Device name */
           int                  request,        /* WE ID */
           struct iwreq *       pwrq)           /* Fixed part of the request */
{
  /* Set device name */
  strncpy(pwrq->ifr_name, ifname, IFNAMSIZ);
  /* Do the request */
  return(ioctl(skfd, request, pwrq));
}


int getWlSiteSurveyRequest(char *interface, int *pStatus)
{
#ifndef NO_ACTION
    int skfd=0;
    struct iwreq wrq;
    unsigned char result;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd==-1)
		return -1;

    /* Get wireless name */
    if ( iw_get_ext(skfd, interface, SIOCGIWNAME, &wrq) < 0){
      /* If no wireless name : no wireless extensions */
      close( skfd );
        return -1;
	}
    wrq.u.data.pointer = (caddr_t)&result;
    wrq.u.data.length = sizeof(result);

    if (iw_get_ext(skfd, interface, SIOCGIWRTLSCANREQ, &wrq) < 0){
    	close( skfd );
	return -1;
	}
    close( skfd );

    if ( result == 0xff )
    	*pStatus = -1;
    else
	*pStatus = (int) result;
#else
	*pStatus = -1;
#endif
#ifdef CONFIG_RTK_MESH 	
	return (int)*(char*)wrq.u.data.pointer; 
#else
	return 0;
#endif
}

int getWlSiteSurveyResult(char *interface, SS_STATUS_Tp pStatus )
{
#ifndef NO_ACTION
    int skfd=0;
    struct iwreq wrq;
    skfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(skfd==-1)
		return -1;
    /* Get wireless name */
    if ( iw_get_ext(skfd, interface, SIOCGIWNAME, &wrq) < 0){
      /* If no wireless name : no wireless extensions */
      close( skfd );
        return -1;
	}
	

    wrq.u.data.pointer = (caddr_t)pStatus;

    if ( pStatus->number == 0 )
    	wrq.u.data.length = sizeof(SS_STATUS_T);
    else
        wrq.u.data.length = sizeof(pStatus->number);


    if (iw_get_ext(skfd, interface, SIOCGIWRTLGETBSSDB, &wrq) < 0){
    	close( skfd );
	return -1;
	}
	
    close( skfd );
#else
	return -1 ;
#endif
    return 0;
}


 void get_ap_mode(unsigned char network,char *tmp1Buf)
{
	if (network==BAND_11B)
			strcpy(tmp1Buf, "B");
		else if (network==BAND_11G)
			strcpy(tmp1Buf, "G");	
		else if (network==(BAND_11G|BAND_11B))
			strcpy(tmp1Buf, "B+G");
		else if (network==(BAND_11N))
			strcpy(tmp1Buf, "N");		
		else if (network==(BAND_11G|BAND_11N))
			strcpy(tmp1Buf, "G+N");	
		else if (network==(BAND_11G|BAND_11B | BAND_11N))
			strcpy(tmp1Buf, "B+G+N");	
		else if(network== BAND_11A)
			strcpy(tmp1Buf, "A");	
		else if(network== (BAND_11A | BAND_11N))
			strcpy(tmp1Buf, "A+N");
		else if(network== BAND_11AC)
			strcpy(tmp1Buf, "AC");
		else if(network== (BAND_11A | BAND_11N | BAND_11AC))
			strcpy(tmp1Buf, (" (A+N+AC)"));
		else if(network== (BAND_11N | BAND_11AC))
			strcpy(tmp1Buf, (" (N+AC)"));	
		else if(network== (BAND_11A | BAND_11AC))
			strcpy(tmp1Buf, (" (A+AC)"));	
}

void get_ap_encrypt(BssDscr *pBss,char *tmp2Buf)
{
   char  wpa_tkip_aes[20],wpa2_tkip_aes[20];

   memset(wpa_tkip_aes,0x00,sizeof(wpa_tkip_aes));
   memset(wpa2_tkip_aes,0x00,sizeof(wpa2_tkip_aes));
		
   	if ((pBss->bdCap & cPrivacy) == 0)
		sprintf(tmp2Buf, "no");
	else {
		if (pBss->bdTstamp[0] == 0)
			sprintf(tmp2Buf, "WEP");
		else {
			int wpa_exist = 0, idx = 0;
			if (pBss->bdTstamp[0] & 0x0000ffff) {
				idx = sprintf(tmp2Buf, "WPA");
				if (((pBss->bdTstamp[0] & 0x0000f000) >> 12) == 0x4)
					idx += sprintf(tmp2Buf+idx, "-PSK");
				wpa_exist = 1;

				if (((pBss->bdTstamp[0] & 0x00000f00) >> 8) == 0x5)
					sprintf(wpa_tkip_aes,"%s","(aes/tkip)"); //mark_issue , cat with tmp2Buf?
				else if (((pBss->bdTstamp[0] & 0x00000f00) >> 8) == 0x4)
					sprintf(wpa_tkip_aes,"%s","(aes)");
				else if (((pBss->bdTstamp[0] & 0x00000f00) >> 8) == 0x1)
					sprintf(wpa_tkip_aes,"%s","(tkip)");

				if(wpa_tkip_aes[0] !=0)
					strcat(tmp2Buf,wpa_tkip_aes);				

			    }
			
				if (pBss->bdTstamp[0] & 0xffff0000) {
					if (wpa_exist)
						idx += sprintf(tmp2Buf+idx, "/");
					idx += sprintf(tmp2Buf+idx, "WPA2");
					if (((pBss->bdTstamp[0] & 0xf0000000) >> 28) == 0x4)
						idx += sprintf(tmp2Buf+idx, "-PSK");

					if (((pBss->bdTstamp[0] & 0x0f000000) >> 24) == 0x5)
						sprintf(wpa2_tkip_aes,"%s","(aes/tkip)");
					else if (((pBss->bdTstamp[0] & 0x0f000000) >> 24) == 0x4)
						sprintf(wpa2_tkip_aes,"%s","(aes)");
					else if (((pBss->bdTstamp[0] & 0x0f000000) >> 24) == 0x1)
						sprintf(wpa2_tkip_aes,"%s","(tkip)");

					if(wpa2_tkip_aes[0] !=0)
					strcat(tmp2Buf,wpa2_tkip_aes);	
				}
			}
		}
}


