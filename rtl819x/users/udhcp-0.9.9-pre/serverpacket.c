/* serverpacket.c
 *
 * Constuct and send DHCP server packets
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

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>

#include "packet.h"
#include "debug.h"
#include "dhcpd.h"
#include "options.h"
#include "leases.h"
#ifdef STATIC_LEASE
#include "static_leases.h"
#endif

#ifdef TR069_ANNEX_F
#include "apmib.h"
#include "mibtbl.h"
#endif

#if defined(CONFIG_RTL865X_KLD)	
extern unsigned char update_lease_time;
extern unsigned char update_lease_time1;
#endif

#ifdef TR069_ANNEX_F
struct device_id_t opt125_deviceId;
#endif


#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
extern struct server_config_t* p_serverpool_config;

#ifdef CTC_DHCP_OPTION43
#define CONFIGVERSTR0     'C'
#define CONFIGVERSTR1     'T'
#define CONFIGVERSTR2     'C'
#define CONFIGVERSTR3     '0'
char option43[1+1+254]={43, 6, 1, 4, CONFIGVERSTR0, CONFIGVERSTR1, CONFIGVERSTR2, CONFIGVERSTR3,0,0};
#endif

#ifdef CTC_DHCP_OPTION60
char option60[1+1+254]={0x3c, 0x1F, 0x12 , 0x34, 0x01, 0x07, 'R', 'e', 'a', 'l', 't', 'e', 'k', 0x02, 0x03, 'C', 'P', 'E', 0x03, 0x08, 'E', '8', 'R', 'O', 'U', 'T', 'E', 'R', 0x04, 0x03, '1', '.', '0', 0, 0}; // FOR dhcp ack, we just send enterprise code to clients.
#endif

extern unsigned int serverpool;

static char *g_apszCTCDeviceName[] =
{
	"Computer",
	"Camera",
	"HGW",
	"STB",
	"Phone",
	NULL
};
#endif


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
		prodClsLen = strlen(PRODUCT_CLASS_GATEWAY);

		subCodeLen = ouiLen + serialNumLen + prodClsLen + 6;
		option125Len = subCodeLen + 5;

		optionStr[1] = option125Len;
		optionStr[6] = subCodeLen;
		optionStr[7] = 0x04; // GatewayManufacturerOUI
		optionStr[8] = ouiLen;
		sprintf(&optionStr[9], "%s", MANUFACTURER_OUI);
		optionStr[8+ouiLen+1] = 0x05; // GatewaySerialNumber
		optionStr[8+ouiLen+2] = serialNumLen;
		sprintf(&optionStr[8+ouiLen+3], "%s", serialNum);
		optionStr[8+ouiLen+3+serialNumLen] = 0x06; // GatewayProductClass
		optionStr[8+ouiLen+3+serialNumLen+1] = prodClsLen;
		sprintf(&optionStr[8+ouiLen+3+serialNumLen+2], "%s", PRODUCT_CLASS_GATEWAY);
		
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

/* Dump the option 125 device Identity to file
 * 0: fail
 * 1: successful
 */
int dump_deviceId()
{
	FILE *fp;
	int i;
	char tmp[160];
	struct device_id_t *pdevId;

	fp = fopen( TR069_ANNEX_F_DEVICE_FILE, "w" );
	if (!fp)
		return 0;

	pdevId = opt125_deviceId.next;

	i = 1;
	while (pdevId) {
		sprintf( tmp, "%d %s", i, pdevId->oui );
		if (pdevId->productClass[0])
			sprintf( tmp, "%s?%s?%s\n", tmp, pdevId->productClass, pdevId->serialNo);
		else
			sprintf( tmp, "%s?%s\n", tmp, pdevId->serialNo);
		fwrite( tmp, 1, strlen(tmp), fp );
		i++;
		pdevId = pdevId->next;
	}

	fclose(fp);
	return 1;
}

/* Device with option 125 has been cached, add device ID into option 125 device list.
 * 0: fail
 * 1: replace
 * 2: add
 */
int add_deviceId(struct device_id_t *deviceId)
{
	struct device_id_t *pdevId;

	pdevId = opt125_deviceId.next;
	// find deviceId
	while (pdevId) {
		if (pdevId->yiaddr == deviceId->yiaddr)
			break;
		pdevId = pdevId->next;
	}

	if (pdevId) { // found, replace it
		//printf("replace deviceId: ip 0x%x\n", pdevId->yiaddr);
		memcpy(pdevId->oui, deviceId->oui, 7);
		memcpy(pdevId->serialNo, deviceId->serialNo, 65);
		memcpy(pdevId->productClass, deviceId->productClass, 65);
		return 1;
	}
	else { // add a new one
		//printf("add new deviceId: ip 0x%x\n", deviceId->yiaddr);
		pdevId = xmalloc(sizeof(struct device_id_t));
		if (pdevId) {
			memcpy(pdevId, deviceId, sizeof(struct device_id_t));
			pdevId->next = opt125_deviceId.next;
			opt125_deviceId.next = pdevId;
			return 2;
		}
		else
			printf("%s: xmalloc fail\n", __FUNCTION__);
	}

	return 0;
}

/* Device iaddr is out, remove its device ID from option 125 device list
 * 0: fail
 * 1: successful
 */
int del_deviceId(u_int32_t iaddr)
{
	struct device_id_t *preId, *curId;

	preId = &opt125_deviceId;
	curId = opt125_deviceId.next;
	// find deviceId
	while (curId) {
		if (curId->yiaddr == iaddr)
			break;
		preId = curId;
		curId = curId->next;
	}

	if (curId) { // found
		preId->next = curId->next;
		free(curId);
		return 1;
	}

	return 0;
}

void clear_all_deviceId()
{
	struct device_id_t *curId, *tmpId;

	curId = opt125_deviceId.next;

	while (curId) {
		tmpId = curId;
		curId = curId->next;
		free(tmpId);
	}
}

static int Option_VendorSpecInfo(struct dhcpMessage *packet, u_int32_t iaddr)
{
	int ret = 0;
	unsigned char *pOpt125 = NULL;
	struct device_id_t curDevId = {iaddr, 3561, "", "", "", 0};
	
#if 1
	pOpt125 = get_option(packet, DHCP_VI_VENSPEC);
#else

#if 0
	unsigned char testbuf[] = { 0x00, 0x00, 0x0d, 0xe9, 0x18, 0x01, 0x06, 0x30,
				  0x30, 0x65, 0x30, 0x64, 0x34, 0x02, 0x09, 0x30,
				  0x30, 0x30, 0x30, 0x2d, 0x30, 0x30, 0x30, 0x31,
				  0x03, 0x03, 0x49, 0x47, 0x44, 0x00};
#else
	unsigned char testbuf[] = {
	0x00 , 0x00 , 0x0d , 0xe9 , 0x2d , 0x01 , 0x06 , 0x30 , 0x30 , 0x30,
	0x31 , 0x30 , 0x32 , 0x02 , 0x10 , 0x30 , 0x30 , 0x30 , 0x31 , 0x30 , 0x32 , 0x2d , 0x34 , 0x32 , 0x38 , 0x32,
	0x38 , 0x38 , 0x38 , 0x32 , 0x39 , 0x03 , 0x11 , 0x43 , 0x44 , 0x52 , 0x6f , 0x75 , 0x74 , 0x65 , 0x72 , 0x20,
	0x56 , 0x6f , 0x49 , 0x50 , 0x20 , 0x41 , 0x54 , 0x41 };
#endif
	pOpt125 = testbuf;
#endif

	if (pOpt125)
	{
		unsigned int ent_num, *pUInt;
		unsigned short data_len;
		
		char *oui = curDevId.oui;
		char *serialNo = curDevId.serialNo;
		char *productClass = curDevId.productClass;

		pUInt = (unsigned int*)pOpt125;
		ent_num = ntohl(*pUInt);
		data_len = (unsigned short)pOpt125[4];

		if (ent_num == 3561)
		{
			unsigned char *pStart;

			// sub-option
			pStart = &pOpt125[5];
			while( data_len>0 )
			{
				unsigned char sub_code, sub_len, *sub_data;

				sub_code = pStart[0];
				sub_len = pStart[1];
				sub_data = &pStart[2];

				if( data_len < sub_len+2 )
					break;

				switch (sub_code)
				{
					case 1: // ManufacturerOUI
						if (sub_len < 7)
						{
							strncpy(oui, sub_data, sub_len);
							oui[sub_len] = 0;
						}
						break;
						
					case 2: // SerialNumber
						if (sub_len < 65)
						{
							strncpy(serialNo, sub_data, sub_len);
							serialNo[sub_len] = 0;
						}
						break;
						
					case 3: // ProductClass
						if (sub_len < 65)
						{
							strncpy(productClass, sub_data, sub_len);
							productClass[sub_len] = 0;
						}
						break;
						
					default:
						//unknown suboption
						break;
				} // end switch
				
				pStart = pStart+2+sub_len;
				data_len = data_len-sub_len-2;
			} // end while


			if( *oui && *serialNo && *productClass )
			{
				add_deviceId(&curDevId);
				dump_deviceId();
			}
			else if( *oui && *serialNo  )
			{
				add_deviceId(&curDevId);
				dump_deviceId();
			}
			else
			{
			}
			
		} // end if (ent_num == 3561)
	} // end if (pOpt125)
	else
	{
		/* no option 125, each Item must be empty */
		unlink(TR069_ANNEX_F_FILE);
	}

	return ret;
}
#endif


#if defined(_PRMT_X_TELEFONICA_ES_DHCPOPTION_)
int parse_CTC_Vendor_Class(struct dhcpMessage *packet, unsigned char* option60, struct dhcp_ctc_client_info *pstClientInfo)
{
	unsigned short enterprise_code=(*option60<<8)+*(option60+1);
	unsigned char *fieldtype;
	unsigned char fieldlength;
	int iOptlen, i;
	struct server_config_t *pDhcp;

	//printf("%s:%d ####enterprise_code=%04x\n", __FUNCTION__,__LINE__,enterprise_code);

	pstClientInfo->iCategory = NULL;

	// Magicia: Every device other than default type is set to Computer.
	for (pDhcp=&server_config; pDhcp; pDhcp=pDhcp->next)
	{
		if(pDhcp->vendorclass && !strcmp(pDhcp->vendorclass, "Computer"))
		{
			pstClientInfo->iCategory = pDhcp->clientRange;
			break;
		}
	}
	pstClientInfo->category = CTC_Computer;

	if(enterprise_code!=0x0000)
	{		
		printf("%s:%d ####enterprise_code is not 0x0000\n", __FUNCTION__,__LINE__);
		//return -1;// not china telecom enterprise code..
	}

	fieldtype=(option60+2);
	iOptlen = *(option60-1);

	while (iOptlen > 0)
	{
		fieldlength = *(fieldtype + 1);
		switch (*fieldtype)
		{
			case Vendor:
				if ((fieldlength < DHCP_CTC_MIN_FIELD_LEN) || (fieldlength > DHCP_CTC_MAX_FIELD_LEN))
				{
				  return -1;
				}

				memcpy(pstClientInfo->szVendor, fieldtype + 2, fieldlength);
				pstClientInfo->szVendor[fieldlength] = 0;
				break;

			case Category:
				if ((fieldlength < DHCP_CTC_MIN_FIELD_LEN) || (fieldlength > DHCP_CTC_MAX_FIELD_LEN))
				{
					return -1;
				}
				for (pDhcp=&server_config; pDhcp; pDhcp=pDhcp->next)
				{
					if(pDhcp->vendorclass && strstr(fieldtype + 2, pDhcp->vendorclass))
					{
						pstClientInfo->iCategory = pDhcp->clientRange;

						for (i = 0; g_apszCTCDeviceName[i]; i++)
						{
							if ((fieldlength == strlen(g_apszCTCDeviceName[i])) && (0 == memcmp(fieldtype + 2, g_apszCTCDeviceName[i], fieldlength)))
							{
								pstClientInfo->category = i;
								break;
							}
						}
						break;
					}
				}
				break;
			case Model:
				if ((fieldlength < DHCP_CTC_MIN_FIELD_LEN) || (fieldlength > DHCP_CTC_MAX_FIELD_LEN))
				{
					return -1;
				}

				memcpy(pstClientInfo->szModel, fieldtype + 2, fieldlength);
				pstClientInfo->szModel[fieldlength] = 0;
				break;
			case Version:
				if ((fieldlength < DHCP_CTC_MIN_FIELD_LEN) || (fieldlength > DHCP_CTC_MAX_FIELD_LEN))
				{
				return -1;
				}

				memcpy(pstClientInfo->szVersion, fieldtype + 2, fieldlength);
				pstClientInfo->szVersion[fieldlength] = 0;
				break;
			case ProtocolType:
				memcpy((char *)(&pstClientInfo->stPortForwarding.usProtocol), fieldtype + 2, sizeof(unsigned short));
				memcpy((char *)(&pstClientInfo->stPortForwarding.usPort), fieldtype + 4, sizeof(unsigned short));
				break;
			default:
				break;
		}
		iOptlen -= fieldlength + 2;
		fieldtype += fieldlength + 2;
	}

	return 0;
}

//int check_type(u_int32_t addr, enum DeviceType devicetype)
int check_type(u_int32_t addr, struct client_category_t *deviceCategory)
{
	int ret=0;

	if (serverpool)
	{
		if ((addr<ntohl(server_config.start)) || (addr>ntohl(server_config.end)))
			ret = 1;
	}
	else
	{
		if (deviceCategory == NULL)
		{
			if ((addr<ntohl(server_config.start)) || (addr>ntohl(server_config.end)))
				ret = 1;
		}
		else
		{
			if ((addr<ntohl(deviceCategory->ipstart)) ||
				(addr>ntohl(deviceCategory->ipend)))
				ret = 1;
		}
	}
	return ret;
}

static int _is_hex(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||
            ((c >= 'a') && (c <= 'f')));
}

static int string_to_hex(char *string, unsigned char *key, int len)
{
	char tmpBuf[4];
	int idx, ii=0;
	for (idx=0; idx<len; idx+=2) {
		tmpBuf[0] = string[idx];
		tmpBuf[1] = string[idx+1];
		tmpBuf[2] = 0;
		if ( !_is_hex(tmpBuf[0]) || !_is_hex(tmpBuf[1]))
			return 0;

		key[ii++] = (unsigned char) strtol(tmpBuf, (char**)NULL, 16);
	}
	return 1;
}

void find_match_serving_pool(struct dhcpMessage *dhcppacket)
{
	struct server_config_t* p_servingpool_tmp=p_serverpool_config->next;
	unsigned char vendorclass[256]={0},clientid[256]={0},userclass[256]={0};
	unsigned char *tmpstr;
	unsigned char len=0;
	unsigned char vendormachflag=0,clientidmachflag=0,userclassmatchflag=0,chaddrmatchflag=0, srcIntfflag=0/*base on sourceinterface*/;
	unsigned char chaddr[6],chaddrmask[6],chaddrmaskresult[6];
	int i;
	unsigned char srcIntf; // Base on sourceinterface

	serverpool = 1;

	tmpstr=get_option(dhcppacket, DHCP_VENDOR);
	if(tmpstr!=NULL){
		len=*(unsigned char*)(tmpstr-OPT_LEN);
		memcpy(vendorclass,tmpstr,len);
		vendorclass[len]=0;
		printf("%s:%d ##vendorclass=%s, len=%d\n",__FUNCTION__,__LINE__,vendorclass,len);
	}
	tmpstr=get_option(dhcppacket, DHCP_CLIENT_ID);
	if(tmpstr!=NULL){
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		len=*(unsigned char*)(tmpstr-OPT_LEN);
		memcpy(clientid,tmpstr,len);
		clientid[len]=0;
	}
	tmpstr=get_option(dhcppacket, DHCP_USER_ID);
	if(tmpstr!=NULL){
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		len=*(unsigned char*)(tmpstr-OPT_LEN);
		memcpy(userclass,tmpstr,len);
		userclass[len]=0;
	}
	// Mason Yu. Base on sourceinterface
	tmpstr=get_option(dhcppacket, DHCP_SRC_INTF);
	if(tmpstr!=NULL){
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		srcIntf = *tmpstr;
	}

	while(p_servingpool_tmp!=NULL){
		vendormachflag=0;
		clientidmachflag=0;
		userclassmatchflag=0;
		chaddrmatchflag=0;
		srcIntfflag=0; // Base on sourceinterface

		if(p_servingpool_tmp->vendorclass==NULL
			&& p_servingpool_tmp->sourceinterface==NULL // Base on sourceinterface
			&& p_servingpool_tmp->clientid==NULL
			&& p_servingpool_tmp->userclass==NULL
			&& p_servingpool_tmp->chaddr==NULL){
			p_servingpool_tmp=p_servingpool_tmp->next;
			continue;
		}

		if(p_servingpool_tmp->vendorclass){
			if(p_servingpool_tmp->vendorclassflag==0){
				int classlen=0,tmplen=0;
				char *classtmp=0;
				if(!strcmp(p_servingpool_tmp->vendorclassmode,"Exact")){
					if(!strcmp(p_servingpool_tmp->vendorclass,vendorclass))
						vendormachflag=1;
				}else if(!strcmp(p_servingpool_tmp->vendorclassmode,"Prefix")){
					classtmp=strstr(vendorclass,p_servingpool_tmp->vendorclass);
					if(classtmp==vendorclass)
						vendormachflag=1;
				}else if(!strcmp(p_servingpool_tmp->vendorclassmode,"Suffix")){
					classlen=strlen(p_servingpool_tmp->vendorclass);
					classtmp=strstr(vendorclass,p_servingpool_tmp->vendorclass);
					tmplen=strlen(classtmp);
					if(tmplen==classlen)
						vendormachflag=1;
				}else{ //Substring
					classtmp=strstr(vendorclass,p_servingpool_tmp->vendorclass);
					if(classtmp!=NULL)
						vendormachflag=1;
				}
			}
			else{
				if(strcmp(p_servingpool_tmp->vendorclass,vendorclass))
					vendormachflag=1;
			}
		}else
			vendormachflag=1;

		if(p_servingpool_tmp->clientid){
			if(p_servingpool_tmp->clientidflag==0){
				if(!strcmp(p_servingpool_tmp->clientid,clientid))
					clientidmachflag=1;
			}
			else{
				if(strcmp(p_servingpool_tmp->clientid,clientid))
					clientidmachflag=1;
			}
		}else
			clientidmachflag=1;

		if(p_servingpool_tmp->userclass){
			if(p_servingpool_tmp->userclassflag==0){
				if(!strcmp(p_servingpool_tmp->userclass,userclass))
					userclassmatchflag=1;
			}
			else{
				if(strcmp(p_servingpool_tmp->userclass,userclass))
					userclassmatchflag=1;
			}
		}else
			userclassmatchflag=1;

		if(p_servingpool_tmp->chaddr){
			if(!p_servingpool_tmp->chaddrmask)
				memset(chaddrmask,0xff,6);
			else
				string_to_hex(p_servingpool_tmp->chaddrmask,chaddrmask,12);
			string_to_hex(p_servingpool_tmp->chaddr,chaddr,12);
			for(i=0;i<6;i++){
				chaddrmaskresult[i]=(dhcppacket->chaddr[i])&chaddrmask[i];
			}
			if(p_servingpool_tmp->chaddrflag==0){
				if(!memcmp(chaddr,chaddrmaskresult,6))
					chaddrmatchflag=1;
			}
			else{
				if(memcmp(chaddr,chaddrmaskresult,6))
					chaddrmatchflag=1;
			}
		}else
			chaddrmatchflag=1;

		// Mason Yu. base on sourceinterface
		if(p_servingpool_tmp->sourceinterface !=0 ){
			if ( ( srcIntf!=0 && (p_servingpool_tmp->sourceinterface&srcIntf)==srcIntf) ) {
				//printf("Find match source Interface. p_servingpool_tmp->sourceinterface=%d, srcIntf=%d\n", p_servingpool_tmp->sourceinterface, srcIntf);
				srcIntfflag=1;
			}
		}else
			srcIntfflag=1;

		if(vendormachflag==1 && clientidmachflag==1 && userclassmatchflag==1 && chaddrmatchflag==1 && srcIntfflag==1)
			break;

		p_servingpool_tmp=p_servingpool_tmp->next;
	}

	if(p_servingpool_tmp==NULL) {		
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		p_servingpool_tmp=p_serverpool_config;
		serverpool = 0;
	}
	//printf("%s:%d ####find servingpool:%s\n",__FUNCTION__,__LINE__,p_servingpool_tmp->poolname);
	memcpy(&server_config,p_servingpool_tmp,sizeof(struct server_config_t));

}

#endif


/* send a packet to giaddr using the kernel ip stack */
static int send_packet_to_relay(struct dhcpMessage *payload)
{
	DEBUG(LOG_INFO, "Forwarding packet to relay");

	return kernel_packet(payload, server_config.server, SERVER_PORT,
			payload->giaddr, SERVER_PORT);
}


/* send a packet to a specific arp address and ip address by creating our own ip packet */
static int send_packet_to_client(struct dhcpMessage *payload, int force_broadcast)
{
	unsigned char *chaddr;
	u_int32_t ciaddr;
	
	if (force_broadcast) {
		DEBUG(LOG_INFO, "broadcasting packet to client (NAK)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;	
	} else if (ntohs(payload->flags) & BROADCAST_FLAG) {
		DEBUG(LOG_INFO, "broadcasting packet to client (requested)");
		ciaddr = INADDR_BROADCAST;
		chaddr = MAC_BCAST_ADDR;
	} else if (payload->ciaddr) {
		DEBUG(LOG_INFO, "unicasting packet to client ciaddr");
		ciaddr = payload->ciaddr;
		chaddr = payload->chaddr;		
	} else {
		DEBUG(LOG_INFO, "unicasting packet to client yiaddr");
		ciaddr = payload->yiaddr;
		chaddr = payload->chaddr;
	}
	return raw_packet(payload, server_config.server, SERVER_PORT, 
			ciaddr, CLIENT_PORT, chaddr, server_config.ifindex);
}


/* send a dhcp packet, if force broadcast is set, the packet will be broadcast to the client */
static int send_packet(struct dhcpMessage *payload, int force_broadcast)
{
	int ret;

	if (payload->giaddr)
		ret = send_packet_to_relay(payload);
	else ret = send_packet_to_client(payload, force_broadcast);
	return ret;
}


static void init_packet(struct dhcpMessage *packet, struct dhcpMessage *oldpacket, char type)
{
	init_header(packet, type);
	packet->xid = oldpacket->xid;
	memcpy(packet->chaddr, oldpacket->chaddr, 16);
	packet->flags = oldpacket->flags;
	packet->giaddr = oldpacket->giaddr;
	packet->ciaddr = oldpacket->ciaddr;
	add_simple_option(packet->options, DHCP_SERVER_ID, server_config.server);

#ifdef SUPPORT_T1_T2_OPTION
	if(type==DHCPOFFER|| type==DHCPACK)
	{
		add_option_string(packet->options, server_config.t1_time);
		add_option_string(packet->options, server_config.t2_time);
	}
#endif
}


/* add in the bootp options */
static void add_bootp_options(struct dhcpMessage *packet)
{
	packet->siaddr = server_config.siaddr;
	if (server_config.sname)
		strncpy(packet->sname, server_config.sname, sizeof(packet->sname) - 1);
	if (server_config.boot_file)
		strncpy(packet->file, server_config.boot_file, sizeof(packet->file) - 1);
}
	

/* send a DHCP OFFER to a DHCP DISCOVER */
int sendOffer(struct dhcpMessage *oldpacket)
{
	struct dhcpMessage packet;
	struct dhcpOfferedAddr *lease = NULL;
	u_int32_t req_align, lease_time_align = server_config.lease;
	unsigned char *req, *lease_time;
	struct option_set *curr;
	struct in_addr addr;
#ifdef STATIC_LEASE
	u_int32_t static_lease_ip;
	char *host, *sname;
	int len;
#endif

	{ 
                   u_int8_t empty_haddr[16]; 
    
                   memset(empty_haddr, 0, 16); 
                   if (!memcmp(oldpacket->chaddr, empty_haddr, 16)) { 
                           LOG(LOG_WARNING, "Empty Client Hardware Addresses"); 
                           return -1; 
                   } 
        } 	


	init_packet(&packet, oldpacket, DHCPOFFER);
	
#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
	enum DeviceType devicetype;
	struct client_category_t *deviceCategory;
	unsigned char *classVendor;
	struct dhcp_ctc_client_info stClientInfo;
	/*ping_zhang:20090313 START:Telefonica DHCP option new request*/
	unsigned char cvlen = 0;
	unsigned char classVendorStr[256] = {0};
	/*ping_zhang:20090313 END*/

	if(!(classVendor=get_option(oldpacket, DHCP_VENDOR))) {
		struct server_config_t *pDhcp;
		//default : PC clients....
		devicetype = CTC_Computer;
		deviceCategory=NULL;
		// Magicia: Every device other than default type is set to Computer.
		for (pDhcp=&server_config; pDhcp; pDhcp=pDhcp->next)
		{
			if(pDhcp->vendorclass && !strcmp(pDhcp->vendorclass, "Computer"))
			{
				deviceCategory = pDhcp->clientRange;
				break;
			}
		}
	}
	else
	{		
		/*ping_zhang:20090313 START:Telefonica DHCP option new request*/
		cvlen=*(unsigned char*)(classVendor-OPT_LEN);
		memcpy(classVendorStr,classVendor,cvlen);
		classVendorStr[cvlen]=0;
		
		//printf("%s:%d####classVendorStr=%s\n",__FUNCTION__,__LINE__,classVendorStr);

		/*ping_zhang:20090313 END*/
		memset(&stClientInfo, 0, sizeof(struct dhcp_ctc_client_info));
		/*ping_zhang:20090313 START:Telefonica DHCP option new request*/
		parse_CTC_Vendor_Class(oldpacket, classVendor, &stClientInfo);
//		parse_CTC_Vendor_Class(oldpacket, classVendorStr, &stClientInfo);
		/*ping_zhang:20090313 END*/
		/*ping_zhang:20090319 START:replace ip range with serving pool of tr069*/
		devicetype = (enum DeviceType)(stClientInfo.category);

		/*ping_zhang:20090319 END*/
		deviceCategory = stClientInfo.iCategory;		
	}
#endif  
	
#ifdef STATIC_LEASE
	static_lease_ip = getIpByMac(server_config.static_leases, oldpacket->chaddr, &host);
	sname = get_option(oldpacket, DHCP_HOST_NAME);
	if (sname)
		len = (int)sname[-1];
	else
		len = 0;

	if (!static_lease_ip && len)
		static_lease_ip = getIpByHost(server_config.static_leases, sname, len, &host);
	
	if(!static_lease_ip || 
		(static_lease_ip && host && 
			((strlen(host)!=(size_t)len) || memcmp(host, sname, len))))
#endif		
	{
		/* the client is in our lease/offered table */
		if ((lease = find_lease_by_chaddr(oldpacket->chaddr))) {
			if (!lease_expired(lease)) 
				lease_time_align = lease->expires - time(0);
			packet.yiaddr = lease->yiaddr;
			
		/* Or the client has a requested ip */
		} else if ((req = get_option(oldpacket, DHCP_REQUESTED_IP)) &&

			   /* Don't look here (ugly hackish thing to do) */
			   memcpy(&req_align, req, 4) &&

			   /* and the ip is in the lease range */
#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
			(!check_type(req_align, deviceCategory)) &&
#else
			   ntohl(req_align) >= ntohl(server_config.start) &&
			   ntohl(req_align) <= ntohl(server_config.end) &&
			   ntohl(req_align)!=ntohl(server_config.server) &&
#endif

#ifdef STATIC_LEASE
				!reservedIp(server_config.static_leases, req_align) &&
#endif		
			   
			   /* and its not already taken/offered */ /* ADDME: check that its not a static lease */
			   ((!(lease = find_lease_by_yiaddr(req_align)) ||
			   
			   /* or its taken, but expired */ /* ADDME: or maybe in here */
			   lease_expired(lease)))) {
					packet.yiaddr = req_align; /* FIXME: oh my, is there a host using this IP? */

		/* otherwise, find a free IP */ /*ADDME: is it a static lease? */
		} else {
#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
			packet.yiaddr = find_address(0, deviceCategory);
			
			/* try for an expired lease */
			if (!packet.yiaddr) packet.yiaddr = find_address(1, deviceCategory);
			if (!packet.yiaddr) packet.yiaddr = find_address(2, deviceCategory);
#else
			packet.yiaddr = find_address(0);
			
			/* try for an expired lease */
			if (!packet.yiaddr) packet.yiaddr = find_address(1);
			if (!packet.yiaddr) packet.yiaddr = find_address(2);
#endif
		}
		
		if(!packet.yiaddr) {
			LOG(LOG_WARNING, "no IP addresses to give -- OFFER abandoned");
			return -1;
		}
		
		if (!add_lease(packet.chaddr, packet.yiaddr, server_config.offer_time)) {
			LOG(LOG_WARNING, "lease pool is full -- OFFER abandoned");
			return -1;
		}		

		if ((lease_time = get_option(oldpacket, DHCP_LEASE_TIME))) {
			memcpy(&lease_time_align, lease_time, 4);
			lease_time_align = ntohl(lease_time_align);
			if (lease_time_align > server_config.lease) 
				lease_time_align = server_config.lease;
		}

		/* Make sure we aren't just using the lease time from the previous offer */
		if (lease_time_align < server_config.min_lease) 
			lease_time_align = server_config.lease;
	}
#ifdef STATIC_LEASE
	/* ADDME: end of short circuit */		
	else
	{
		/* It is a static lease... use it */
		packet.yiaddr = static_lease_ip;
		if (!add_lease(packet.chaddr, packet.yiaddr, 0xffffffff)) {
			LOG(LOG_WARNING, "lease pool is full -- OFFER abandoned");
			return -1;
		}				
		lease_time_align = 0xffffffff;
	}
#endif	
#if defined(CONFIG_RTL865X_KLD)	
	if(server_config.upateConfig_isp == 1){
		if(update_lease_time){
			lease_time_align = server_config.min_lease;
		}
	}
	if(server_config.upateConfig_isp_dns == 1){
		if(update_lease_time1){	
			lease_time_align = server_config.min_lease;
		}
	}
#endif
	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));

	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}

	add_bootp_options(&packet);

#if defined(CTC_DHCP_OPTION43)
		// jim should attach option  43 to dhcp ack packet.
	add_option_string(packet.options, option43);
#endif

#if defined(CTC_DHCP_OPTION60)
		// jim should attach option  60 to dhcp ack packet.
	add_option_string(packet.options, option60);
#endif

#ifdef TR069_ANNEX_F
	add_option_125(packet.options);
#endif
	
	addr.s_addr = packet.yiaddr;
	LOG(LOG_INFO, "sending OFFER of %s", inet_ntoa(addr));
	
	//printf("%s:%d ####\n",__FUNCTION__,__LINE__);
	return send_packet(&packet, 0);
}


int sendNAK(struct dhcpMessage *oldpacket)
{
	struct dhcpMessage packet;

	init_packet(&packet, oldpacket, DHCPNAK);
	
	DEBUG(LOG_INFO, "sending NAK");
	#if defined(CONFIG_RTL865X_KLD)
		return send_packet(&packet, server_config.response_broadcast);
	#elif defined(CONFIG_RTL_ULINKER)
		return send_packet(&packet, 1);
	#else
		return send_packet(&packet, 0);
	#endif
}


int sendACK(struct dhcpMessage *oldpacket, u_int32_t yiaddr)
{
	struct dhcpMessage packet;
	struct option_set *curr;
	unsigned char *lease_time;
	u_int32_t lease_time_align = server_config.lease;
	struct in_addr addr;

	init_packet(&packet, oldpacket, DHCPACK);
	packet.yiaddr = yiaddr;
	
	if ((lease_time = get_option(oldpacket, DHCP_LEASE_TIME))) {
		memcpy(&lease_time_align, lease_time, 4);
		lease_time_align = ntohl(lease_time_align);
		if (lease_time_align > server_config.lease) 
			lease_time_align = server_config.lease;
		else if (lease_time_align < server_config.min_lease) 
			lease_time_align = server_config.lease;
	}

#ifdef STATIC_LEASE
	if (reservedIp(server_config.static_leases, yiaddr))
		lease_time_align = 0xffffffff;
#endif
#if defined(CONFIG_RTL865X_KLD)	
	if(server_config.upateConfig_isp == 1){
		if(update_lease_time)
			lease_time_align = server_config.min_lease;
	}
	if(server_config.upateConfig_isp_dns == 1){
		if(update_lease_time1){
			lease_time_align = server_config.min_lease;
		}
	}
#endif	
	add_simple_option(packet.options, DHCP_LEASE_TIME, htonl(lease_time_align));
	
	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}

	add_bootp_options(&packet);

#if defined(CTC_DHCP_OPTION43)
	add_option_string(packet.options, option43);
#endif

#ifdef TR069_ANNEX_F
	add_option_125(packet.options);
#endif

	addr.s_addr = packet.yiaddr;
	LOG(LOG_INFO, "sending ACK to %s", inet_ntoa(addr));
	#if defined(CONFIG_RTL865X_KLD)
			if (send_packet(&packet, server_config.response_broadcast) < 0) 	
				return -1;
	#else
	if (send_packet(&packet, 0) < 0)
		return -1;
	#endif
	add_lease(packet.chaddr, packet.yiaddr, lease_time_align);

#ifdef TR069_ANNEX_F
	Option_VendorSpecInfo(oldpacket, oldpacket->yiaddr);
#endif

	return 0;
}


int send_inform(struct dhcpMessage *oldpacket)
{
	struct dhcpMessage packet;
	struct option_set *curr;

	init_packet(&packet, oldpacket, DHCPACK);
	
	curr = server_config.options;
	while (curr) {
		if (curr->data[OPT_CODE] != DHCP_LEASE_TIME)
			add_option_string(packet.options, curr->data);
		curr = curr->next;
	}

	add_bootp_options(&packet);

#ifdef TR069_ANNEX_F
	add_option_125(packet.options);
	Option_VendorSpecInfo(oldpacket, oldpacket->yiaddr);
#endif
	
	#if defined(CONFIG_RTL865X_KLD)
		return send_packet(&packet, server_config.response_broadcast);				
	#else
	return send_packet(&packet, 0);
	#endif
}



