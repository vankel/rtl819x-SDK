/*
 *      Web server handler routines for TCP/IP stuffs
 *      Authors: David Hsu	<davidhsu@realtek.com.tw>
 *      Authors: Dick Tam	<dicktam@realtek.com.tw>
 *
 */





/*-- System inlcude files --*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <time.h>
#include <net/route.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <net/if.h>



/*-- Local inlcude files --*/
#include "apmib.h"
#include "boa.h"
#include "apform.h"
#include "utility.h"

#define CWMP_PPPOE_STR	"pppoe"
#define CWMP_DHCP_STR	"dhcp"
#define CWMP_STATIC_IP_STR	"static_ip"
#define CWMP_ACS_ADDR "/var/system/cwmp_ACS_Addr"
extern char *req_get_cstream_var(request *req, char *var, char *defaultGetValue);
extern int req_format_write(request *req, char *format, ...);
enum {LINK_INIT=0, LINK_NO, LINK_DOWN, LINK_WAIT, LINK_UP};
#define	CONFIG_DIR	"/var/cwmp_config"
#define CA_FNAME	CONFIG_DIR"/cacert.pem"
#define CERT_FNAME	CONFIG_DIR"/client.pem"
#define strACSURLWrong  "ACS's URL can't be empty!"
#define strSSLWrong "CPE does not support SSL! URL should not start with 'https://'!"
#define strSetACSURLerror "Set ACS's URL error!"
#define strSetUserNameerror "Set User Name error!"
#define strSetPasserror "Set Password error!"
#define strSetInformEnableerror "Set Inform Enable error!"
#define strSetInformIntererror "Set Inform Interval error!"
#define strSetConReqUsererror "Set Connection Request UserName error!"
#define strSetConReqPasserror "Set Connection Request Password error!"
#define strSetCWMPFlagerror "Set CWMP_FLAG error!"
#define strGetCWMPFlagerror "Get CWMP_FLAG error!"
#define strUploaderror "Upload error!"
#define strMallocFail "malloc failure!"
#define strArgerror "Insufficient args\n"
#define strSetCerPasserror  "Set CPE Certificat's Password error!"

const char IFCONFIG[] = "/bin/ifconfig";
const char IPTABLES[] = "/bin/iptables";
const char ARG_T[] = "-t";
const char FW_DEL[] = "-D";
const char FW_PREROUTING[] = "PREROUTING";
const char ARG_I[] = "-i";
const char LANIF[] = "br0";
const char ARG_TCP[] = "TCP";
const char ARG_UDP[] = "UDP";
const char FW_DPORT[] = "--dport";
const char RMACC_MARK[] = "0x1000";
const char FW_ADD[] = "-A";

#define RECONNECT_MSG(url) { \
	req_format_write(wp, ("<html><body><blockquote><h4>Change setting successfully!" \
                "<form><input type=button value=\"  OK  \" OnClick=window.location.replace(\"%s\")></form></blockquote></body></html>"), url);\
}


#define UPLOAD_MSG(url) { \
	req_format_write(wp, ("<html><body><blockquote><h4>Upload a file successfully!" \
                "<form><input type=button value=\"  OK  \" OnClick=window.location.replace(\"%s\")></form></blockquote></body></html>"), url);\
}

#define CWMPPID  "/var/run/cwmp.pid"
int  getPPPIface(char *wanIface,char * realIface)
{
	FILE *pMap = NULL;
	char pppName[10]={0},name[10]={0};
	int found=0;
	if((pMap = fopen("/etc/ppp/pppx_map2_ethm","r+"))!= NULL)
	{			
		
		while( fscanf(pMap,"%s %s",pppName,name) > 0 )
		{						   
		   if(!strcmp(name,realIface))
		   {
		   		strcpy(wanIface,pppName);
		   		found=1;
				break;
			
		   }	
		   memset(pppName, 0x00, 10);
		   memset(name, 0x00, 10);
		}
		close(pMap);
		return found;
	}else
		return found;	
	
}
void off_tr069()
{
	int cwmppid=0;

	cwmppid = getPid((char*)CWMPPID);

	printf("\ncwmppid=%d\n",cwmppid);

	if(cwmppid > 0)
		kill(cwmppid, 15);

}

void GetACSURL_String(char *newurl, char *acsurl) //star: remove "http://" from acs url string
{
	  register const char *s;
	  register size_t i, n;
	  newurl[0] = '\0';

	  if (!acsurl || !*acsurl)
	    return;
	  s = strchr(acsurl, ':');
	  if (s && s[1] == '/' && s[2] == '/')
	    s += 3;
	  else
	    s = acsurl;
	  n = strlen(s);
	  if(n>256)
	  	n=256;

	  for (i = 0; i < n; i++)
	  { newurl[i] = s[i];
	    if (s[i] == '/' || s[i] == ':')
	      break;
	  }

	  newurl[i] = '\0';
}



//got wan ip already, we need to add host route for ACS server 
void set_ACSHostRoute(unsigned char urlChanged, int wan_type, char *gwip, char *wan_interface)
{
	char vStr[256+1];
	char acsurl[256+1];
	char ACSIP[64]={0};
	char GWIP[64]={0};
	char IFACE[64]={0};
	char WriteBuffer[128]={0};
	FILE *fp;
	char *tmpStr=NULL;
	struct hostent *host;
	struct in_addr acsaddr;
	
	fp = fopen(CWMP_ACS_ADDR, "r");
	if(fp !=NULL){
		fscanf(fp,"%s %s %s",ACSIP, GWIP, IFACE);
		fclose(fp);
		printf("%s, %d, ACSIP=%s, GWIP=%s, IFACE=%s\n", __FUNCTION__, __LINE__, ACSIP,GWIP, IFACE);  
		if(strcmp(GWIP, "NULL"))
			va_cmd("/bin/route", 7, 1,"del", "-host", ACSIP, "gw", GWIP, "dev", IFACE);
		else
			va_cmd("/bin/route", 5, 1, "del", "-host", ACSIP, "dev", IFACE);
	}
	if(urlChanged){
		apmib_get(MIB_CWMP_ACS_URL,(void*)vStr);
		GetACSURL_String(acsurl,vStr);
		fprintf(stderr, "ACS URL=%s\n",acsurl);
		host=gethostbyname(acsurl);
		if(host==NULL){
			unlink(CWMP_ACS_ADDR);
			printf("CANNOT get ACS IP Address\n");
			return;
		}
		memcpy((char *) &(acsaddr.s_addr), host->h_addr_list[0], host->h_length);
		if(!(fp = fopen(CWMP_ACS_ADDR, "w"))){
			printf("%s, %d, Open ACS IP file failed: %s\n", __FUNCTION__, __LINE__,strerror(errno));
			return;
		}
		tmpStr= inet_ntoa(acsaddr);
		if(gwip[0]){
			sprintf(WriteBuffer, "%s %s %s", tmpStr, gwip, wan_interface);
			sprintf(GWIP, "%s", gwip);
		}else{
			sprintf(WriteBuffer, "%s NULL %s", tmpStr, wan_interface);	
			sprintf(GWIP, "%s", "NULL");	
		}	
		sprintf(ACSIP,"%s", tmpStr);
		sprintf(IFACE, "%s", wan_interface);
		fwrite(WriteBuffer, strlen(WriteBuffer), 1, fp);
		fclose(fp);
	}
	if(ACSIP[0]){
		if(wan_type == PPPOE)
		{
			va_cmd("/bin/route", 4, 1, "add", "-host", ACSIP,  "dev", IFACE);
			return;
		}else{
			if(strcmp(GWIP, "NULL")){
				va_cmd("/bin/route", 7,1, "add", "-host", ACSIP, "gw", GWIP, "dev", IFACE);
			}else
				va_cmd("/bin/route", 5,1, "add", "-host", ACSIP,  "dev", IFACE);
		}
	}
	
}

int is_wan_connected(int wanType, char *ifaceName)
{	
	struct in_addr intaddr;
	int retValue=0;
	int wanip1=0;
	int wanip2=0;
	int wanip3=0;
	if (wanType == PPPOE) {
		if ( !isConnectPPP())
			return(LINK_DOWN);					
	}
	
	if (wanType == PPPOE) {
		retValue = getInAddr(ifaceName, IP_ADDR, (void *)&intaddr);
		if (retValue < 0) {
			return(LINK_DOWN);
		}	
	}
	if (ifaceName && getInAddr(ifaceName, IP_ADDR, (void *)&intaddr)){ 		
		if (wanType == PPPOE){
			wanip1 = (intaddr.s_addr & 0xFF000000);
			wanip2 = (intaddr.s_addr & 0x00FF0000);
			wanip3 = (intaddr.s_addr & 0x0000FF00);
			if((wanip1 == 0x0A000000) && (wanip2 == 0x00400000) && (wanip3 == 0x00004000)){
				return(LINK_WAIT);
			}
		}
		return(LINK_UP);
	}else{
		if (wanType == PPPOE){
			retValue = getInAddr(ifaceName, IP_ADDR, (void *)&intaddr);
			wanip1 = (intaddr.s_addr & 0xFF000000);
			wanip2 = (intaddr.s_addr & 0x00FF0000);
			wanip3 = (intaddr.s_addr & 0x0000FF00);
			if((wanip1 == 0x0A000000) && (wanip2 == 0x00400000) && (wanip3 == 0x00004000)){
					return(LINK_DOWN);
			}
		}else if(wanType ==DHCP_CLIENT)
			return(LINK_DOWN);
		else
			return(LINK_DOWN);
	}
	return(LINK_DOWN);	
}
#if defined(MULTI_WAN_SUPPORT)
void getRemoteIP(WANIFACE_T *wanEntry, char *gwip,char *ifaceName, unsigned int index)
{
	FILE *fp=NULL;
	char tmpBuf[64]={0};
	struct in_addr intaddr;
	if(wanEntry->AddressType == DHCP_CLIENT){
		sprintf(tmpBuf, "/var/dhcpRemoteIp_%d", index);
		fp = fopen(tmpBuf, "r");
		if(fp != NULL){
			fscanf(fp,"%s",gwip);
			fclose(fp);
		}
	}else if(wanEntry->AddressType == DHCP_DISABLED){
		sprintf(gwip,"%s",inet_ntoa(*((struct in_addr *)wanEntry->remoteIpAddr)));
	}else if(wanEntry->AddressType == PPPOE){
		if(getInAddr( ifaceName, DST_IP_ADDR, (void *)&intaddr)){
			sprintf(gwip,"%s",inet_ntoa(intaddr));
		}
	}
}
#endif //#if defined(MULTI_WAN_SUPPORT)

int startCWMP(unsigned char urlChanged)
{
	char strPort[16];
	char gwIpAddr[32]={0};
	char wanName[16];
	char pppName[16];
	char cmdBuffer[64];
	unsigned int conreq_port=0;
	unsigned int cwmp_flag;
	unsigned int i,wan_num=0;
#if defined(MULTI_WAN_SUPPORT)	
	WANIFACE_T *p,wan_entity;
#endif	


	/*add a wan port to pass */
	system("sysconf firewall");

	/*start the cwmpClient program*/
	apmib_get(MIB_CWMP_FLAG, (void *)&cwmp_flag);
	if( cwmp_flag&CWMP_FLAG_AUTORUN )
		va_cmd( "/bin/cwmpClient", 0, 0 );


	return 0;
}
   
#ifdef _CWMP_WITH_SSL_


unsigned char* CWMP_find_multiPartBoundary(unsigned char *data, int dlen, char *pattern, int plen, int *result)
{	
	int i;	
	unsigned char *end;	
	
	if (plen > dlen)
		return 0;	
	for (i=0; i<dlen;i++) {
		if(memcmp(data + i, pattern, plen)!=0) {
			continue;
		}
		else {		
			*result = 1;	
			end = (data + i);
			return end;
		}
	}  
	*result = 0;
	return 0;
}
int CWMP_Find_head_offset(char *upload_data)
{
	int head_offset=0 ;
	char *pStart=NULL;
	int iestr_offset=0;
	char *dquote;
	char *dquote1;
	
	if (upload_data==NULL) {
		fprintf(stderr, "upload data is NULL\n");
		return -1;
	}

	
	pStart = strstr(upload_data, "filename=");
	if (pStart == NULL) {
		return -1;
	}
	else {
		
		dquote =  strchr(pStart+strlen("filename="), '\n');
		if (dquote !=NULL) {
			dquote= dquote+2; 
			dquote1 = strchr(dquote, '\r');
			if (dquote1!=NULL) {
				iestr_offset = 4;
				pStart = dquote1;
			}
			else {
				return -1;
			}
		}
		else {
			return -1;
			}
	}

	

	
	//fprintf(stderr,"####%s:%d %d###\n",  __FILE__, __LINE__ , iestr_offset);
	head_offset = (int)(((unsigned long)pStart)-((unsigned long)upload_data)) + iestr_offset;
	return head_offset;
}

//copy from fmmgmt.c
//find the start and end of the upload file.
int uploadGetCert(request *wp, unsigned int *offset, unsigned int *len)
{
	
	int head_offset=0;
	int result_last=0;
	unsigned char* uploadedFileContent_end=NULL;
	unsigned char* uploadedFileContent=NULL;

	head_offset = CWMP_Find_head_offset((char *)wp->upload_data); 
	if (head_offset == -1) {
		printf("find_head_offset error\n");
		return -1;
	}
	uploadedFileContent = wp->upload_data+head_offset;
	
	uploadedFileContent_end = CWMP_find_multiPartBoundary(uploadedFileContent, wp->upload_len, wp->multipart_boundary, strlen(wp->multipart_boundary),&result_last);
	if (result_last==1 && uploadedFileContent_end != NULL) {
		uploadedFileContent_end = uploadedFileContent_end-2;

		(*len) = uploadedFileContent_end -  uploadedFileContent;
		(*offset) = head_offset;
		return 0;
		
	}
	
	return -1;
	

}

#endif //#ifdef _CWMP_WITH_SSL_

///////////////////////////////////////////////////////////////////
void formTR069Config(request *wp, char *path, char *query)
{
	char	*strData;
	char tmpBuf[100];
	char orig_acsUserName[64]={0};
	char orig_acsPassword[64]={0};
	char new_acsUserName[64]={0};
	char new_acsPassword[64]={0};
	char orig_ConReqUserName[64]={0};
	char orig_ConReqPassword[64]={0};
	char new_ConReqUserName[64]={0};
	char new_ConReqPassword[64]={0};
	unsigned char vChar;
	unsigned int cwmp_flag;
	int vInt;
	// Mason Yu
	char changeflag=0;
	unsigned char acsurlchangeflag=0;
	unsigned int informEnble;
	unsigned int informInterv;
	char cwmp_flag_value=1;
	char tmpStr[256+1];
	char origACSURL[256+1];
	char NewACSURL[256+1];
	int cur_port;
	char isDisConReqAuth=0;

//displayPostDate(wp->post_data);
	apmib_get( MIB_CWMP_ACS_URL, (void *)origACSURL);
#ifdef _CWMP_WITH_SSL_
	//CPE Certificat Password
	strData = req_get_cstream_var(wp, ("CPE_Cert"), (""));
	if( strData[0] )
	{
		strData = req_get_cstream_var(wp, ("certpw"), (""));

		changeflag = 1;
		if ( !apmib_set( MIB_CWMP_CERT_PASSWORD, (void *)strData))
		{
			strcpy(tmpBuf, strSetCerPasserror);
			goto setErr_tr069;
		}
		else
			goto end_tr069;
	}
#endif

	strData = req_get_cstream_var(wp, ("url"), (""));
	//if ( strData[0] )
	{
		if ( strlen(strData)==0 )
		{
			strcpy(tmpBuf, (strACSURLWrong));
			goto setErr_tr069;
		}
#ifndef _CWMP_WITH_SSL_
		if ( strstr(strData, "https://") )
		{
			strcpy(tmpBuf, (strSSLWrong));
			goto setErr_tr069;
		}
#endif
		if ( !apmib_set( MIB_CWMP_ACS_URL, (void *)strData))
		{
			strcpy(tmpBuf, (strSetACSURLerror));
			goto setErr_tr069;
		}
	}

	apmib_get( MIB_CWMP_ACS_URL, (void *)NewACSURL);
	if(strcmp(origACSURL, NewACSURL)){
		changeflag=1;
		acsurlchangeflag=1;
	}



	apmib_get( MIB_CWMP_ACS_PASSWORD, (void *)orig_acsUserName);
	apmib_get( MIB_CWMP_ACS_USERNAME, (void *)orig_acsPassword);

	apmib_get( MIB_CWMP_CONREQ_USERNAME, (void *)orig_ConReqUserName);
	apmib_get( MIB_CWMP_CONREQ_PASSWORD, (void *)orig_ConReqPassword);
	
	
	strData = req_get_cstream_var(wp, ("username"), (""));
	//if ( strData[0] )
	{
		if ( !apmib_set( MIB_CWMP_ACS_USERNAME, (void *)strData)) {
			strcpy(tmpBuf, (strSetUserNameerror));
			goto setErr_tr069;
		}
	}

	strData = req_get_cstream_var(wp, ("password"), (""));
	//if ( strData[0] )
	{
		if ( !apmib_set( MIB_CWMP_ACS_PASSWORD, (void *)strData)) {
			strcpy(tmpBuf, (strSetPasserror));
			goto setErr_tr069;
		}
	}

	strData = req_get_cstream_var(wp, ("enable"), (""));
	if ( strData[0] ) {
		informEnble = (strData[0]=='0')? 0:1;
		apmib_get( MIB_CWMP_INFORM_ENABLE, (void*)&vInt);
		if(vInt != informEnble){
			//int allow=1;
			changeflag = 1;
			if ( !apmib_set( MIB_CWMP_INFORM_ENABLE, (void *)&informEnble)) {
				strcpy(tmpBuf, (strSetInformEnableerror));
				goto setErr_tr069;
			}
		}
	}

	strData = req_get_cstream_var(wp, ("interval"), (""));
	if ( strData[0] ) {
		informInterv = atoi(strData);
		
		if(informEnble == 1){
			apmib_get( MIB_CWMP_INFORM_INTERVAL, (void*)&vInt);

			if(vInt != informInterv){
				changeflag = 1;
				if ( !apmib_set( MIB_CWMP_INFORM_INTERVAL, (void *)&informInterv)) {
					strcpy(tmpBuf, (strSetInformIntererror));
					goto setErr_tr069;
				}
			}
		}
	}

#ifdef _TR069_CONREQ_AUTH_SELECT_
	strData = req_get_cstream_var(wp, ("disconreqauth"), (""));
	if ( strData[0] ) {
		cwmp_flag=0;
		vChar=0;

		if( apmib_get( MIB_CWMP_FLAG2, (void *)&cwmp_flag ) )
		{
			changeflag = 1;

			if(strData[0]=='0')
				cwmp_flag = cwmp_flag & (~CWMP_FLAG2_DIS_CONREQ_AUTH);
			else{
				cwmp_flag = cwmp_flag | CWMP_FLAG2_DIS_CONREQ_AUTH;
				isDisConReqAuth = 1;
			}

			if ( !apmib_set( MIB_CWMP_FLAG2, (void *)&cwmp_flag)) {
				strcpy(tmpBuf, (strSetCWMPFlagerror));
				goto setErr_tr069;
			}
		}else{
			strcpy(tmpBuf, (strGetCWMPFlagerror));
			goto setErr_tr069;
		}
	}
#endif

	//if connection reuqest auth is enabled, don't handle conreqname & conreqpw to keep the old values
	if(!isDisConReqAuth)
	{
		strData = req_get_cstream_var(wp, ("conreqname"), (""));
		//if ( strData[0] )
		{
			if ( !apmib_set( MIB_CWMP_CONREQ_USERNAME, (void *)strData)) {
				strcpy(tmpBuf, (strSetConReqUsererror));
				goto setErr_tr069;
			}
		}

		strData = req_get_cstream_var(wp, ("conreqpw"), (""));
		//if ( strData[0] )
		{
			if ( !apmib_set( MIB_CWMP_CONREQ_PASSWORD, (void *)strData)) {
				strcpy(tmpBuf, (strSetConReqPasserror));
				goto setErr_tr069;
			}
		}
	}//if(isDisConReqAuth)

	strData = req_get_cstream_var(wp, ("conreqpath"), (""));
	//if ( strData[0] )
	{
		apmib_get( MIB_CWMP_CONREQ_PATH, (void *)tmpStr);
		if (strcmp(tmpStr,strData)!=0){
			changeflag = 1;
			if ( !apmib_set( MIB_CWMP_CONREQ_PATH, (void *)strData)) {
				strcpy(tmpBuf, ("Set Connection Request Path error!"));
				goto setErr_tr069;
			}
		}
	}

	strData = req_get_cstream_var(wp, ("conreqport"), (""));
	if ( strData[0] ) {
		cur_port = atoi(strData);
		apmib_get( MIB_CWMP_CONREQ_PORT, (void *)&vInt);
		if ( vInt != cur_port ) {
			changeflag = 1;
			if ( !apmib_set( MIB_CWMP_CONREQ_PORT, (void *)&cur_port)) {
				strcpy(tmpBuf, ("Set Connection Request Port error!"));
				goto setErr_tr069;
			}
		}
	}

/*for debug*/
	strData = req_get_cstream_var(wp, ("dbgmsg"), (""));
	if ( strData[0] ) {
		cwmp_flag=0;
		vChar=0;

		if( apmib_get( MIB_CWMP_FLAG, (void *)&cwmp_flag ) )
		{
			if(strData[0]=='0')
				cwmp_flag = cwmp_flag & (~CWMP_FLAG_DEBUG_MSG);
			else
				cwmp_flag = cwmp_flag | CWMP_FLAG_DEBUG_MSG;

			if ( !apmib_set( MIB_CWMP_FLAG, (void *)&cwmp_flag)) {
				strcpy(tmpBuf, (strSetCWMPFlagerror));
				goto setErr_tr069;
			}
		}else{
			strcpy(tmpBuf, (strGetCWMPFlagerror));
			goto setErr_tr069;
		}
	}

#ifdef _CWMP_WITH_SSL_
	strData = req_get_cstream_var(wp, ("certauth"), (""));
	if ( strData[0] ) {
		cwmp_flag=0;
		vChar=0;

		if( apmib_get( MIB_CWMP_FLAG, (void *)&cwmp_flag ) )
		{
			if(strData[0]=='0')
				cwmp_flag = cwmp_flag & (~CWMP_FLAG_CERT_AUTH);
			else
				cwmp_flag = cwmp_flag | CWMP_FLAG_CERT_AUTH;

			changeflag = 1;
			if ( !apmib_set( MIB_CWMP_FLAG, (void *)&cwmp_flag)) {
				strcpy(tmpBuf, (strSetCWMPFlagerror));
				goto setErr_tr069;
			}
		}else{
			strcpy(tmpBuf, (strGetCWMPFlagerror));
			goto setErr_tr069;
		}
	}
#endif

	strData = req_get_cstream_var(wp, ("sendgetrpc"), (""));
	if ( strData[0] ) {
		cwmp_flag=0;
		vChar=0;

		if( apmib_get( MIB_CWMP_FLAG, (void *)&cwmp_flag ) )
		{
			if(strData[0]=='0')
				cwmp_flag = cwmp_flag & (~CWMP_FLAG_SENDGETRPC);
			else
				cwmp_flag = cwmp_flag | CWMP_FLAG_SENDGETRPC;

			if ( !apmib_set(MIB_CWMP_FLAG, (void *)&cwmp_flag)) {
				strcpy(tmpBuf, (strSetCWMPFlagerror));
				goto setErr_tr069;
			}
		}else{
			strcpy(tmpBuf, (strGetCWMPFlagerror));
			goto setErr_tr069;
		}
	}

	strData = req_get_cstream_var(wp, ("skipmreboot"), (""));
	if ( strData[0] ) {
		cwmp_flag=0;
		vChar=0;

		if( apmib_get( MIB_CWMP_FLAG, (void *)&cwmp_flag ) )
		{
			if(strData[0]=='0')
				cwmp_flag = cwmp_flag & (~CWMP_FLAG_SKIPMREBOOT);
			else
				cwmp_flag = cwmp_flag | CWMP_FLAG_SKIPMREBOOT;

			if ( !apmib_set( MIB_CWMP_FLAG, (void *)&cwmp_flag)) {
				strcpy(tmpBuf, (strSetCWMPFlagerror));
				goto setErr_tr069;
			}
		}else{
			strcpy(tmpBuf, (strGetCWMPFlagerror));
			goto setErr_tr069;
		}
	}

	strData = req_get_cstream_var(wp, ("delay"), (""));
	if ( strData[0] ) {
		cwmp_flag=0;
		vChar=0;

		if( apmib_get( MIB_CWMP_FLAG, (void *)&cwmp_flag ) )
		{
			if(strData[0]=='0')
				cwmp_flag = cwmp_flag & (~CWMP_FLAG_DELAY);
			else
				cwmp_flag = cwmp_flag | CWMP_FLAG_DELAY;

			if ( !apmib_set( MIB_CWMP_FLAG, (void *)&cwmp_flag)) {
				strcpy(tmpBuf, (strSetCWMPFlagerror));
				goto setErr_tr069;
			}
		}else{
			strcpy(tmpBuf, (strGetCWMPFlagerror));
			goto setErr_tr069;
		}
	}
	strData = req_get_cstream_var(wp, ("autoexec"), (""));
	if ( strData[0] ) {
		cwmp_flag=0;
		vChar=0;

		if( apmib_get( MIB_CWMP_FLAG, (void *)&cwmp_flag ) )
		{
			int onoff_tr069 = 0;
			if(strData[0]=='0') {
				if ( cwmp_flag & CWMP_FLAG_AUTORUN )
					changeflag = 1;

				cwmp_flag = cwmp_flag & (~CWMP_FLAG_AUTORUN);
				cwmp_flag_value = 0;
			}else {
				if ( !(cwmp_flag & CWMP_FLAG_AUTORUN) )
					changeflag = 1;

				cwmp_flag = cwmp_flag | CWMP_FLAG_AUTORUN;
				cwmp_flag_value = 1;
			}

			if ( !apmib_set( MIB_CWMP_FLAG, (void *)&cwmp_flag)) {
				strcpy(tmpBuf, (strSetCWMPFlagerror));
				goto setErr_tr069;
			}
			
			onoff_tr069 = (cwmp_flag & CWMP_FLAG_AUTORUN)==0?0:1;
			apmib_set( MIB_CWMP_ENABLED, (void *)&onoff_tr069);
			
		}else{
			strcpy(tmpBuf, (strGetCWMPFlagerror));
			goto setErr_tr069;
		}
	}
/*end for debug*/
end_tr069:

	apmib_get( MIB_CWMP_ACS_PASSWORD, (void *)new_acsUserName);
	apmib_get( MIB_CWMP_ACS_USERNAME, (void *)new_acsPassword);
	
	if(orig_acsUserName[0] && orig_acsPassword[0] && new_acsUserName[0] && new_acsPassword[0]) {
		if((strcmp(orig_acsUserName, new_acsUserName)) || (strcmp(orig_acsPassword, new_acsPassword)))
			changeflag=1;
	}

	apmib_get( MIB_CWMP_CONREQ_USERNAME, (void *)new_ConReqUserName);
	apmib_get( MIB_CWMP_CONREQ_PASSWORD, (void *)new_ConReqPassword);
	if(orig_ConReqUserName[0] && orig_ConReqPassword[0] && new_ConReqUserName[0] && new_ConReqPassword[0]) {
		if((strcmp(orig_ConReqUserName, new_ConReqUserName)) || (strcmp(orig_ConReqPassword, new_ConReqPassword)))
			changeflag=1;
	}
	

	if ( changeflag ) {
		if ( cwmp_flag_value == 0 ) {  // disable TR069
			off_tr069();
		} else {                       // enable TR069
			off_tr069();
			if (-1==startCWMP(acsurlchangeflag)){
				strcpy(tmpBuf, ("Start tr069 Fail *****"));
				printf("Start tr069 Fail *****\n");
				goto setErr_tr069;
			}
		}
	}


// Magician: Commit immediately
#ifdef COMMIT_IMMEDIATELY
	Commit();
#endif

	apmib_update_web(CURRENT_SETTING);
	
	strData = req_get_cstream_var(wp, ("submit-url"), (""));
	RECONNECT_MSG(strData);// display reconnect msg to remote
	return;



setErr_tr069:
	ERR_MSG(tmpBuf);
}


/*******************************************************/
/*show extra fileds at tr069config.htm*/
/*******************************************************/
#ifdef _CWMP_WITH_SSL_


void formTR069CPECert(request *wp, char *path, char *query)
{
	char	*strData;
	char tmpBuf[100];
	FILE	*fp_input=NULL;
	char *buf;
	unsigned int nLen, head_offset=0;;
	if ((uploadGetCert(wp, &head_offset, &nLen)) == -1)
	{
		strcpy(tmpBuf, (strUploaderror));
 		goto setErr_tr069cpe;
 	}

	
	printf("filesize is %d\n", nLen);
	buf = calloc(1,nLen+1);
	if (!buf)
	{
		strcpy(tmpBuf, (strMallocFail));
 		goto setErr_tr069cpe;
 	}
	snprintf(buf, nLen+1, "%s", wp->upload_data+head_offset);

	fp_input=fopen(CERT_FNAME,"w");
	if (!fp_input)
		printf("create %s file fail!\n", CERT_FNAME);
	fprintf(fp_input,"%s",buf);
	printf("create file %s\n", CERT_FNAME);
	free(buf);
	fclose(fp_input);

	if( va_cmd( "/bin/flatfsd",1,1,"-s" ) )
		printf( "%s[%d]:exec 'flatfsd -s' error!",__FILE__ ,__LINE__);

	off_tr069();

	if (startCWMP(0) == -1)
	{
		strcpy(tmpBuf, ("Start tr069 Fail *****"));
		printf("Start tr069 Fail *****\n");
		goto setErr_tr069cpe;
	}

	strData = req_get_cstream_var(wp, ("submit-url"), ("/tr069config.htm"));
	UPLOAD_MSG(strData);// display reconnect msg to remote
	return;

setErr_tr069cpe:
	ERR_MSG(tmpBuf);
}


void formTR069CACert(request *wp, char *path, char *query)
{
	char	*strData;
	char tmpBuf[100];
	FILE	*fp_input=NULL;
	char *buf;
	unsigned int nLen,head_offset=0;
	if ((uploadGetCert(wp, &head_offset, &nLen)) == -1)
	{
		strcpy(tmpBuf, (strUploaderror));
 		goto setErr_tr069ca;
 	}
    
	
	printf("filesize is %d\n", nLen);
	buf = calloc(1,nLen+1);
	if (!buf)
	{
		strcpy(tmpBuf, (strMallocFail));
 		goto setErr_tr069ca;
 	}
	snprintf(buf, nLen+1, "%s", wp->upload_data+head_offset);
	
	fp_input=fopen(CA_FNAME,"w");
	if (!fp_input)
		printf("create %s file fail!\n", CA_FNAME );
	fprintf(fp_input,"%s", buf);
	printf("create file %s\n",CA_FNAME);
	free(buf);
	fclose(fp_input);

	if( va_cmd( "/bin/flatfsd",1,1,"-s" ) )
		printf( "%s[%d]:exec 'flatfsd -s' error!",__FILE__ ,__LINE__);

	off_tr069();

	if (startCWMP(0) == -1)
	{
		strcpy(tmpBuf, ("Start tr069 Fail *****"));
		printf("Start tr069 Fail *****\n");
		goto setErr_tr069ca;
	}

	strData = req_get_cstream_var(wp, ("submit-url"), ("/tr069config.htm"));

	UPLOAD_MSG(strData);// display reconnect msg to remote
	return;

setErr_tr069ca:
	ERR_MSG(tmpBuf);
}

int ShowACSCertCPE(request *wp)
{
	int nBytesSent=0;
	//unsigned char vChar=0;
	int isEnable=0;
	unsigned int cwmp_flag;

	if ( apmib_get( MIB_CWMP_FLAG, (void *)&cwmp_flag) )
		if ( (cwmp_flag & CWMP_FLAG_CERT_AUTH)!=0 )
			isEnable=1;

	nBytesSent += req_format_write(wp, ("  <tr>\n"));
	nBytesSent += req_format_write(wp, ("      <td width=\"30%%\"><font size=2><b>ACS Certificates CPE:</b></td>\n"));
	nBytesSent += req_format_write(wp, ("      <td width=\"70%%\"><font size=2>\n"));
	nBytesSent += req_format_write(wp, ("      <input type=\"radio\" name=certauth value=0 %s >No&nbsp;&nbsp;\n"), isEnable==0?"checked":"" );
	nBytesSent += req_format_write(wp, ("      <input type=\"radio\" name=certauth value=1 %s >Yes\n"), isEnable==1?"checked":"" );
	nBytesSent += req_format_write(wp, ("      </td>\n"));
	nBytesSent += req_format_write(wp, ("  </tr>\n"));

//		"\n"), isEnable==0?"checked":"", isEnable==1?"checked":"" );

	return nBytesSent;
}

int ShowMNGCertTable(request *wp)
{
	int nBytesSent=0;
	char buffer[256]="";

	apmib_get(MIB_CWMP_CERT_PASSWORD,buffer);

	nBytesSent += req_format_write(wp, ("\n"
		"<table border=0 width=\"500\" cellspacing=4 cellpadding=0>\n"
		"  <tr><hr size=1 noshade align=top></tr>\n"
		"  <tr>\n"
		"      <td width=\"30%%\"><font size=2><b>Certificat Management:</b></td>\n"
		"      <td width=\"70%%\"><b></b></td>\n"
		"  </tr>\n"
		"\n"));

//usually do not need to upload CPE cert file
#if 0
	nBytesSent += req_format_write(wp, ("\n"
		"  <tr>\n"
		"      <td width=\"30%%\"><font size=2><b>CPE Certificat Password:</b></td>\n"
		"      <td width=\"70%%\">\n"
		"		<form action=/boafrm/formTR069Config method=POST name=\"cpe_passwd\">\n"
		"		<input type=\"text\" name=\"certpw\" size=\"24\" maxlength=\"64\" value=\"%s\">\n"
		"		<input type=\"submit\" value=\"Apply\" name=\"CPE_Cert\">\n"
		"		<input type=\"reset\" value=\"Undo\" name=\"reset\">\n"
		"		<input type=\"hidden\" value=\"/tr069config.htm\" name=\"submit-url\">\n"
		"		</form>\n"
		"      </td>\n"
		"  </tr>\n"
		"\n"), buffer);

	nBytesSent += req_format_write(wp, ("\n"
		"  <tr>\n"
		"      <td width=\"30%%\"><font size=2><b>CPE Certificat:</b></td>\n"
		"      <td width=\"70%%\"><font size=2>\n"
		"           <form action=/boafrm/formTR069CPECert method=POST enctype=\"multipart/form-data\" name=\"cpe_cert\">\n"
		"           <input type=\"file\" name=\"binary\" size=24>&nbsp;&nbsp;\n"
		"           <input type=\"submit\" value=\"Upload\" name=\"load\">\n"
		"           </form>\n"
		"      </td>\n"
		"  </tr>\n"
		"\n"));
#endif
	nBytesSent += req_format_write(wp, ("\n"
		"  <tr>\n"
		"      <td width=\"30%%\"><font size=2><b>CA Certificat:</b></td>\n"
		"      <td width=\"70%%\"><font size=2>\n"
		"           <form action=/boafrm/formTR069CACert method=POST enctype=\"multipart/form-data\" name=\"ca_cert\">\n"
		"           <input type=\"file\" name=\"binary\" size=24>&nbsp;&nbsp;\n"
		"           <input type=\"submit\" value=\"Upload\" name=\"load\">\n"
		"           </form>\n"
		"      </td>\n"
		"  </tr>\n"
		"\n"));

	nBytesSent += req_format_write(wp, ("\n"
		"</table>\n"
		"\n"));


	return nBytesSent;
}
#endif //#ifdef _CWMP_WITH_SSL_


#ifdef _TR069_CONREQ_AUTH_SELECT_
int ShowAuthSelect(request *wp)
{
	int nBytesSent=0;
	unsigned char vChar=0;
	int isDisable=0;
	unsigned int cwmp_flag;

	if ( apmib_get( MIB_CWMP_FLAG2, (void *)&cwmp_flag) )
		if ( (cwmp_flag & CWMP_FLAG2_DIS_CONREQ_AUTH)!=0 )
			isDisable=1;

	nBytesSent += req_format_write(wp, ("  <tr>\n"));
	nBytesSent += req_format_write(wp, ("      <td width=\"30%%\"><font size=2><b>Authentication:</b></td>\n"));
	nBytesSent += req_format_write(wp, ("      <td width=\"70%%\"><font size=2>\n"));
	nBytesSent += req_format_write(wp, ("      <input type=\"radio\" name=disconreqauth value=1 %s onClick=\"return authSel()\">Disabled&nbsp;&nbsp;\n"), isDisable==1?"checked":"" );
	nBytesSent += req_format_write(wp, ("      <input type=\"radio\" name=disconreqauth value=0 %s onClick=\"return authSel()\">Enabled\n"), isDisable==0?"checked":"" );
	nBytesSent += req_format_write(wp, ("      </td>\n"));
	nBytesSent += req_format_write(wp, ("  </tr>\n"));

	return nBytesSent;
}
int ShowAuthSelFun(request *wp)
{
	int nBytesSent=0;
	nBytesSent += req_format_write(wp, ("function authSel()\n"));
	nBytesSent += req_format_write(wp, ("{\n"));
	nBytesSent += req_format_write(wp, ("		if ( document.tr069.disconreqauth[0].checked ) {\n"));
	nBytesSent += req_format_write(wp, ("			disableTextField(document.tr069.conreqname);\n"));
	nBytesSent += req_format_write(wp, ("			disableTextField(document.tr069.conreqpw);\n"));
	nBytesSent += req_format_write(wp, ("		} else {\n"));
	nBytesSent += req_format_write(wp, ("			enableTextField(document.tr069.conreqname);\n"));
	nBytesSent += req_format_write(wp, ("			enableTextField(document.tr069.conreqpw);\n"));
	nBytesSent += req_format_write(wp, ("		}\n"));
	nBytesSent += req_format_write(wp, ("}\n"));
	return nBytesSent;
}
#endif

int TR069ConPageShow(request *wp, int argc, char **argv)
{
	int nBytesSent=0;
	char *name;
	//unsigned int cwmp_flag;
	
	//printf("get parameter=%s\n", argv[0]);
	name = argv[0];
	if (name == NULL) {
   		fprintf(stderr, strArgerror);
   		return -1;
   	}

#ifdef _CWMP_WITH_SSL_
	if ( !strcmp(name, ("ShowACSCertCPE")) )
		return ShowACSCertCPE( wp );
	else if ( !strcmp(name, ("ShowMNGCertTable")) )
		return ShowMNGCertTable( wp );
#endif

#ifdef _TR069_CONREQ_AUTH_SELECT_
	if ( !strcmp(name, ("ShowAuthSelect")) )
		return ShowAuthSelect( wp );
	if ( !strcmp(name, ("ShowAuthSelFun")) )
		return ShowAuthSelFun( wp );
	if ( !strcmp(name, ("DisConReqName")) ||
             !strcmp(name, ("DisConReqPwd"))   )
        {
		unsigned char vChar=0;
		int isDisable=0;

		if ( apmib_get( MIB_CWMP_FLAG2, (void *)&cwmp_flag) )
			if ( (cwmp_flag & CWMP_FLAG2_DIS_CONREQ_AUTH)!=0 )
				isDisable=1;
		if(isDisable) return req_format_write(wp, "disabled");
	}
#endif

	return nBytesSent;
}

