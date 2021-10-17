/*
 *      Web server handler routines for ethernet dot1x
 *
 *
 *
 */

/*-- System inlcude files --*/
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>

#include "boa.h"
#include "globals.h"
#include "apform.h"
#include "apmib.h"
#include "utility.h"
#include "asp_page.h"

int getMaxPortNumber(void)
{
	int maxNum, opmode;

	apmib_get( MIB_OP_MODE, (void *)&opmode);
	
	maxNum =  MAX_ELAN_DOT1X_PORTNUM - 1;
	if (opmode ==BRIDGE_MODE || opmode == WISP_MODE)
	{
	#if !defined(CONFIG_RTL_IVL_SUPPORT)
		maxNum =  MAX_ELAN_DOT1X_PORTNUM;
	#endif
	}

	return maxNum;
}
void formEthDot1x(request *wp, char *path, char *query)
{
	ETHDOT1X_T entry;
	char *submitUrl,*strTmp, *strIp, *strPort, *strPassword, *strUnicast;
	int i, onoff = 0, onoff2 = 0, type = 0, mode = 0, mode2 = 0, serverport = 0, intVal = 0, maxNum = 0;
	char tmpBuf[100];
	struct in_addr inIp;

	//displayPostDate(wp->post_data);
	//printf("--%s(%d)--\n", __FUNCTION__, __LINE__);

	strTmp= req_get_cstream_var(wp, ("ethdot1x_onoff"), "");
	if(strTmp[0])
	{
		onoff = atoi(strTmp);
	}
	/* MIB_ELAN_ENABLE_1X bit0-->proxy/snooping enable/disable
	 * MIB_ELAN_ENABLE_1X bit1-->client mode enable/disable
	 */
	
	apmib_get( MIB_ELAN_ENABLE_1X, (void *)&onoff2);
	if (onoff)
	{
		onoff2 |= ETH_DOT1X_PROXY_SNOOPING_MODE_ENABLE_BIT;
	}
	else
	{
		onoff2 &= ~ETH_DOT1X_PROXY_SNOOPING_MODE_ENABLE_BIT;
	}
	if (!apmib_set(MIB_ELAN_ENABLE_1X, (void *)&onoff2))
	{
		strcpy(tmpBuf, ("set  MIB_ELAN_ENABLE_1X error!"));
	//	printf("--%s(%d)--\n", __FUNCTION__, __LINE__);
		goto setErr;
	}
	
	if(onoff == 1)
	{
		
		strTmp= req_get_cstream_var(wp, ("type"), "");
		if(strTmp[0])
		{
			type = atoi(strTmp);
			if (!apmib_set(MIB_ELAN_DOT1X_PROXY_TYPE, (void *)&type))
			{
				strcpy(tmpBuf, ("set  MIB_ELAN_DOT1X_PROXY_TYPE error!"));
				goto setErr;
			}
		}

		/* mode:bit0->client mode, bit1->proxy mode, bit2->snooping mode */
		strTmp= req_get_cstream_var(wp, ("mode"), "");
		if(strTmp[0])
		{
			mode = atoi(strTmp);
			apmib_get( MIB_ELAN_DOT1X_MODE, (void *)&mode2);
			if (mode == 0)//snooping mode
			{
				mode2 |= ETH_DOT1X_SNOOPING_MODE_BIT;
			}
			else
			{
				mode2 &= ~ETH_DOT1X_SNOOPING_MODE_BIT;
			}
			
			if (mode == 1)//proxy mode
			{
				mode2 |= ETH_DOT1X_PROXY_MODE_BIT;
			}
			else
			{
				mode2 &= ~ETH_DOT1X_PROXY_MODE_BIT;
			}
			
			if (!apmib_set(MIB_ELAN_DOT1X_MODE, (void *)&mode2))
			{
				strcpy(tmpBuf, ("set  MIB_ELAN_DOT1X_MODE error!"));
				goto setErr;
			}
		}

		if (mode == 0)//snooping mode
		{
			
			strTmp= req_get_cstream_var(wp, ("ethdot1x_server_port_number"), "");
			if(strTmp[0])
			{
				serverport = atoi(strTmp); 
				serverport -= 1;
				if (!apmib_set(MIB_ELAN_DOT1X_SERVER_PORT, (void *)&serverport))
				{
					strcpy(tmpBuf, ("set  MIB_ELAN_DOT1X_SERVER_PORT error!"));
					goto setErr;
				}
			}
			
		}
		else if (mode == 1)//proxy mode
		{
			strIp = req_get_cstream_var(wp, ("ethdot1x_radius_ip"), "");
			if ( strIp[0] ) {
				if ( !inet_aton(strIp, &inIp) ) {
					strcpy(tmpBuf, ("Invalid IP-address value!"));
					goto setErr;
				}
				if ( !apmib_set( MIB_ELAN_RS_IP, (void *)&inIp)) {
					strcpy(tmpBuf, ("Set rs IP-address error!"));
					goto setErr;
				}
			}
			
			strPort = req_get_cstream_var(wp, ("ethdot1x_radius_port"), "");
			if (strPort[0])
			{
				if ( !string_to_dec(strPort, &intVal) || intVal<1 || intVal>65535) {
					strcpy(tmpBuf, ("Error! Invalid value of from-port."));
					goto setErr;
				}
				
				if ( !apmib_set( MIB_ELAN_RS_PORT, (void *)&intVal)) {
					strcpy(tmpBuf, ("Set rs port error!"));
					goto setErr;
				}
			}

			
 			strPassword = req_get_cstream_var(wp, ("ethdot1x_radius_pass"), "");
			if ( strPassword[0] ) {
				if ( apmib_set(MIB_ELAN_RS_PASSWORD, (void *)strPassword) == 0) {
					strcpy(tmpBuf, ("Set rs password MIB error!"));
					goto setErr;
				}
			}
			
			strUnicast = req_get_cstream_var(wp, ("ethdot1x_unicastresp_onoff"), "");
			if(strUnicast[0])
			{
				intVal = atoi(strUnicast); 
				if (!apmib_set(MIB_ELAN_EAPOL_UNICAST_ENABLED, (void *)&intVal))
				{
					strcpy(tmpBuf, ("set  MIB_ELAN_EAPOL_UNICAST_ENABLED error!"));
					goto setErr;
				}
			}

			
		}
		
		if ( !apmib_set(MIB_ELAN_DOT1X_DELALL, (void *)&entry))
		{
			strcpy(tmpBuf, ("Delete all table error!"));
		//	printf("--%s(%d)--\n", __FUNCTION__, __LINE__);
			goto setErr;
		}
		
		maxNum = getMaxPortNumber();
		for(i=1; i<=maxNum ; i++)
		{
			memset(&entry, '\0', sizeof(entry));
		//	printf("--%s(%d)--i is %d\n", __FUNCTION__, __LINE__, i);
			
			*((char *)&entry) = (char)i;
			apmib_get(MIB_ELAN_DOT1X_TBL, (void *)&entry);

			memset(tmpBuf,0x00, sizeof(tmpBuf));
			sprintf(tmpBuf,"port_enable_%d",i);
			strTmp = req_get_cstream_var(wp, tmpBuf, "");
			if(strTmp[0])
			{
				entry.enabled = atoi(strTmp);
				//printf("%s %d entry.enabled=%d\n", __FUNCTION__, __LINE__, entry.enabled);
			}

			memset(tmpBuf,0x00, sizeof(tmpBuf));
			sprintf(tmpBuf,"port_number_%d",i);
			strTmp = req_get_cstream_var(wp, tmpBuf, "");
			if(strTmp[0])
			{
				entry.portnum = i - 1;//start from 0....
				//printf("%s %d entry.portnum=%d\n", __FUNCTION__, __LINE__, entry.portnum);
			}
			
			//in snooping mode , donot set server port
			if ((mode == 0) && (serverport == i))//snooping mode and this port is server port ,disable 1x
			{
				entry.enabled = 0;
			}
			if ( apmib_set(MIB_ELAN_DOT1X_ADD, (void *)&entry) == 0)
			{
				strcpy(tmpBuf, ("Add 1x table entry error!"));
//				printf("--%s(%d)--\n", __FUNCTION__, __LINE__);
				goto setErr;
			}




		}

	}

	apmib_update_web(CURRENT_SETTING);

#ifndef NO_ACTION
	run_init_script("all");
#endif

	submitUrl = req_get_cstream_var(wp, "submit-url", "");	 // hidden page
	if (submitUrl[0])
	{
		OK_MSG(submitUrl);
	}
	return;

setErr:
	ERR_MSG(tmpBuf);
	return;

}


int ethDot1xList(request *wp, int idx)
{
	ETHDOT1X_T entry;
	//char *strToken;
	int  maxNum = 0;
	int  index=0;
	char temp[32];
	OPMODE_T opmode=-1;
	char bufStr[128];


	memset(temp,0x00,sizeof(temp));
	memset(bufStr,0x00,sizeof(bufStr));

	index = idx;
	maxNum = getMaxPortNumber();
	
	if( index <= maxNum && index != 0) /* ignore item 0 */
	{

		memset((void *)&entry, 0x00, sizeof(entry));
		*((char *)&entry) = (char)index;

		if ( !apmib_get(MIB_ELAN_DOT1X_TBL, (void *)&entry))
		{
			fprintf(stderr,"Get dot1x entry fail\n");
			return -1;
		}
		apmib_get( MIB_OP_MODE, (void *)&opmode);
		
		snprintf(temp, 32, "port%d", index);

		/* enabled/portnum */
		sprintf(bufStr, "token[%d] =\'%d|%s\';\n",idx, entry.enabled,temp);
	}
	else
	{
		sprintf(bufStr, "token[%d] =\'0|none\';\n", idx);
	}
	//printf("%s %d bufStr=%s\n", __FUNCTION__, __LINE__, bufStr);
	req_format_write(wp, bufStr);
	
	return 0;
}

int getEthDot1xList(request *wp, int argc, char **argv)
{
	int i, maxNum;

	maxNum = getMaxPortNumber();
	
	for (i=1; i<=maxNum; i++) {
		ethDot1xList(wp, i);
	}
	return 0;
}


