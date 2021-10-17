/*
 * Realtek Semiconductor Corp.
 * 2011/09/09
 *
 * HTTP file server handler routines
 *
 */
 
/********************************* Includes ***********************************/
#include <malloc.h>
#include <signal.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include "apmib.h"
#include "boa.h"
#include "asp_page.h"
#include "apform.h"
#include "utility.h"
#include "http_files.h"


#define EMPTY_FILE 0
#define EMPTY_STR ""
//#define DROP_CACHES "echo 1 > /proc/sys/vm/drop_caches"
#define DROP_CACHES "echo 3 > /proc/sys/vm/drop_caches"
static const char month_tab[48] ="Jan Feb Mar Apr May Jun Jul Aug Sep Oct Nov Dec ";
static const char day_tab[28] = "Sun,Mon,Tue,Wed,Thu,Fri,Sat,"; 
unsigned int CurSortOrderN='D'; 
unsigned int CurSortOrderM='D'; 
unsigned int CurSortOrderS='D'; 
char sub_str1[300]={0};
char sub_str2[300]={0};
char sub_str3[300]={0};
char sub_str4[300]={0};
char sub_str5[300]={0};
char sub_str6[300]={0};
char upload_st[512]={0};
char sub_str7[512]={0};
char sub_str8[512]={0};
char sub_str9[300]={0};
extern char* dump_directory(int *docLength, char* path, int sort_type, int sort_order);
//extern void get_ParentDirectory(char *dest_path, char *src_path, int usage);
//static	unsigned char indexhttp_head[]="HTTP/1.1 200 OK\r\nPragma: no-cache\r\nCache-Control: no-cache\r\nConnection: close\r\n";
//static	unsigned char indexhttp_head_end[]="\r\n";

#define indexdata_head \
"<html><head>" 

#define indexdata_head_05 \
"var BrowserDetect = {" \
"init: function(){this.browser = this.searchString(this.dataBrowser) || \"An unknown browser\";\n" \
"this.version = this.searchVersion(navigator.userAgent)|| this.searchVersion(navigator.appVersion)|| \"an unknown version\";\n" \
"this.OS = this.searchString(this.dataOS) || \"an unknown OS\";},\n" \
"searchString: function (data) {for (var i=0;i<data.length;i++){	var dataString = data[i].string;var dataProp = data[i].prop;this.versionSearchString = data[i].versionSearch || data[i].identity;if (dataString) {	if (dataString.indexOf(data[i].subString) != -1){return data[i].identity;}}else if (dataProp){return data[i].identity;}}},\n" \
"searchVersion: function (dataString) {var index = dataString.indexOf(this.versionSearchString);if(index == -1){return;}else{return parseFloat(dataString.substring(index+this.versionSearchString.length+1));	}}," 

#define indexdata_head_06 \
"dataBrowser: [{string: navigator.userAgent,subString: \"Chrome\",identity: \"Chrome\"},{string: navigator.userAgent,subString: \"OmniWeb\",versionSearch: \"OmniWeb/\",identity: \"OmniWeb\"	},\n" \
"{string: navigator.vendor,subString: \"Apple\",identity: \"Safari\",versionSearch: \"Version\"},\n" \
"{prop: window.opera,identity: \"Opera\"},{	string: navigator.vendor,subString: \"iCab\",	identity: \"iCab\"},\n" \
"{string: navigator.vendor,subString: \"KDE\",identity: \"Konqueror\"	},\n" \
"{string: navigator.userAgent,subString: \"Firefox\",identity: \"Firefox\"},\n" \
"{string: navigator.vendor,subString: \"Camino\",	identity: \"Camino\"},\n" \
"{string: navigator.userAgent,subString: \"Netscape\",identity: \"Netscape\"},\n" \
"{string: navigator.userAgent,subString: \"MSIE\",identity: \"Explorer\",versionSearch: \"MSIE\"},\n" \
"{string: navigator.userAgent,subString: \"Gecko\",identity: \"Mozilla\",versionSearch: \"rv\"},\n" \
"{string: navigator.userAgent,subString: \"Mozilla\",identity: \"Netscape\",	versionSearch: \"Mozilla\"}],\n" \
"dataOS : [{string: navigator.platform,	subString: \"Win\",	identity: \"Windows\"}," 
#define indexdata_head_07 \
"{string: navigator.platform,subString: \"Mac\",identity: \"Mac\"	}," \
"{string: navigator.userAgent,subString: \"iPhone\",identity: \"iPhone/iPod\"}," \
"{string: navigator.platform,subString: \"Linux\",identity: \"Linux\"}]};"

#define indexdata_head_0 \
"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">\n" \
"<META http-equiv=Pragma content=no-cache>\n" \
"<META HTTP-EQUIV=\"CACHE-CONTROL\" CONTENT=\"NO-CACHE\">\n" \
"<script type=\"text/javascript\" src=\"/util_gw.js\"></script>\n" \
"<script>\n" \
"var time=0;\n" \
"var delay_time=1000;\n" \
"var loop_num=0;\n" \
"function show_div(show,id) {\n" \
"if(show){\n" \
"document.getElementById(id).style.display  = \"block\";\n" \
"}else{\n" \
"document.getElementById(id).style.display  = \"none\";\n" \
"}}\n" 

#define indexdata_head_00 \
"function sendClickedRemove(file)\n" \
"{ var F=document.usb_upload;\n" \
"var act_query=F.action.split(\"?\");\n" \
"var destDir=act_query[1].split(\"=\");\n" \
"if(!confirm(\"Are you sure to remove \"+file)){ return false;}\n" \
"if(BrowserDetect.browser==\"Explorer\"){\n" \
"F.action=\"/boafrm/formusbdisk_uploadfile?del_file=\"+encodeURIComponent(destDir[1])+encodeURIComponent(file);\n" \
"}else{F.action=\"/boafrm/formusbdisk_uploadfile?del_file=\"+destDir[1]+file;}\n" \
"F.submit();\n" \
"}\n" \
"function sendClicked()\n" \
"{\n" \
"if(BrowserDetect.browser==\"Explorer\"){\n" \
"if(document.usb_upload.uploadedfile.value == \"\"){\n" \
"document.usb_upload.uploadedfile.focus();\n" \
"alert('File name can not be empty !');\n" \
"return false;\n" \
 "}\n" \
 "__update_state(document.usb_upload.uploadedfile.value);\n" \
"}else{\n" \
"if(File_Name == \"\"){\n" \
"alert('File name can not be empty !');\n" \
"return false;\n" \
 "}\n" \
"__update_state(File_Name);\n" \
"}\n" \
"}\n"


#define indexdata_head_01 \
"function t_disable(table_id)\n" \
"{var inputs=document.getElementById(table_id).getElementsByTagName('input');\n" \
"var test_href;\n" \
"for(var i=0; i<inputs.length; ++i){\n" \
"inputs[i].disabled=true;}\n" \
"test_href= document.getElementById(\"rowa0\");\n" \
"test_href.removeAttribute(\"href\");\n" \
"test_href= document.getElementById(\"rowa1\");\n" \
"test_href.removeAttribute(\"href\");\n" \
"test_href= document.getElementById(\"rowa2\");\n" \
"test_href.removeAttribute(\"href\");\n" \
"test_href= document.getElementById(\"rowa10\");\n" \
"test_href.removeAttribute(\"href\");\n" \
"var total_entry=inputs.length+1;\n" \
"var i;\n" \
"for(i=1;i<total_entry;i++){\n" \
"var entryname=\"row\"+i;\n" \
"test_href=document.getElementById(entryname);\n" \
"test_href.removeAttribute(\"href\");\n" \
"}}\n"
#define indexdata_head_02 \
"var __AjaxReq = null;\n" \
"var __AjaxReqIE = null;\n" \
"var REDIR_URL=\"\";\n" \
"var POST_URL=\"\";\n" \
"var File_Name=\"\";\n" \
"var Magic_Key=\"\";\n" \
"var KeepQuery=\"1\";\n" \
"function update_usb_connect(status)\n" \
"{if(status==\"OK\"){if(1){document.usb_upload.action=\"/boafrm/formusbdisk_uploadfile?dest_dir=\"+encodeURIComponent(document.getElementById(\"current_directory\").innerHTML)+\"&MKey=\"+Magic_Key;}\n" \
"else{}\n" \
"if(BrowserDetect.browser==\"Explorer\"){\n" \
"uploadFileIE();\n" \
"}else{\n" \
"uploadFile();\n" \
"document.getElementById(\"fileToUpload\").disabled=true;\n" \
"document.getElementById(\"Upload\").disabled=true;\n" \
"}\n" \
"t_disable(\"table1\");\n" \
"}else{show_div(false, \"progress_div\"); alert(status);location.reload(true);}\n" \
"}\n"


#define indexdata_head_02_1 \
"function __createRequest()\n" \
"{var request = null;\n" \
"try { request = new XMLHttpRequest(); }\n" \
"catch (trymicrosoft)\n" \
"{try { request = new ActiveXObject(\"Msxml2.XMLHTTP\"); }\n" \
"catch (othermicrosoft)\n" \
"{try { request = new ActiveXObject(\"Microsoft.XMLHTTP\"); }\n" \
"catch (failed)\n" \
"{request = null;}}}\n" \
"if (request == null){ alert(\"Error creating request object !\");}\n" \
"return request;}\n" 

#define indexdata_head_03 \
"function __send_request(url)\n" \
"{\n" \
"if (__AjaxReq == null) __AjaxReq = __createRequest();\n" \
"__AjaxReq.open(\"GET\", url, true);\n" \
"__AjaxReq.onreadystatechange = __update_page;\n" \
"__AjaxReq.send(null);}\n" \
"function generate_random_str()\n" \
"{var d = new Date();\n" \
"var str=d.getFullYear()+\".\"+(d.getMonth()+1)+\".\"+d.getDate()+\".\"+d.getHours()+\".\"+d.getMinutes()+\".\"+d.getSeconds();\n" \
"return str;}\n" 

#define indexdata_head_04 \
"function __update_page()\n" \
"{\n" \
"var conn_msg=\"\";\n" \
"if (__AjaxReq != null && __AjaxReq.readyState == 4)\n" \
"{\n" \
"if (__AjaxReq.responseText.substring(0,3)==\"var\")\n" \
"{\n" \
"eval(__AjaxReq.responseText);\n" \
"switch (__result[0])\n" \
"{case \"OK\":\n" \
"update_usb_connect(__result[1]);\n" \
"default :\n" \
"break;\n" \
"}\n" \
"delete __result;}}}\n" 

#define indexdata_head_04_0 \
"function uploadFile() {\n" \
"var fd = new FormData();\n" \
"fd.append(\"fileToUpload\", document.getElementById('fileToUpload').files[0]);\n" \
"var xhr = __createRequest();\n" \
"xhr.upload.addEventListener(\"progress\", uploadProgress, false);\n" \
"xhr.addEventListener(\"load\", uploadComplete, false);\n" \
"xhr.addEventListener(\"error\", uploadFailed, false);\n" \
"xhr.addEventListener(\"abort\", uploadCanceled, false);\n" \
"xhr.open(\"POST\",POST_URL, true);\n" \
"show_div(true, \"progress_div\");\n" \
"show_div(true, \"progressNumber\");\n" \
"xhr.send(fd);}\n" 

#define indexdata_head_04_1 \
"function FillprogressBar(BarLen) {\n" \
"var node = document.getElementById('progress_bar');\n" \
"var w    = Math.round(BarLen * 200 /100);\n" \
"node.style.width = parseInt(w) + 'px';\n" \
"}\n" \
"function uploadProgress(evt) {\n" \
"if (evt.lengthComputable) {\n" \
"var percentComplete = Math.round(evt.loaded * 100 / evt.total);\n" \
"document.getElementById('progressNumber').innerHTML = 'Please wait... '+percentComplete.toString() + '%';\n" \
"FillprogressBar(percentComplete);\n" \
"}\n" \
"else {\n" \
"document.getElementById('progressNumber').innerHTML = 'unable to compute';\n" \
"}\n" \
"}\n" \
"function uploadComplete(evt) {\n" \
"location.href=REDIR_URL+'?upload_st='+new Date().getTime();\n" \
"}\n" 

#define indexdata_head_04_2 \
"function uploadFailed(evt) {\n" \
"alert('There was an error attempting to upload the file.');\n" \
"}\n" \
"function uploadCanceled(evt) {\n" \
"alert('The upload has been canceled by the user or the browser dropped the connection.');\n" \
"}\n" \
"function fileSelected() {\n" \
"var file = document.getElementById('fileToUpload').files[0];\n" \
"if (file) {\n" \
"var fileSize = 0;\n" \
"if (file.size > 1024 * 1024)\n" \
"fileSize = (Math.round(file.size * 100 / (1024 * 1024)) / 100).toString() + 'MB';\n" \
"else\n" \
"fileSize = (Math.round(file.size * 100 / 1024) / 100).toString() + 'KB';\n" \
"File_Name = file.name;\n" \
"document.getElementById('fileName').innerHTML = 'Name: ' + file.name;\n" \
"document.getElementById('fileSize').innerHTML = 'Size: ' + fileSize;\n" \
"document.getElementById('fileType').innerHTML = 'Type: ' + file.type;\n" \
"}\n" \
"}\n"

#define indexdata_head_04_3 \
"function FillprogressBarIE(TotalLen, CurrentLen)\n" \
"{\n" \
"if(isNaN(TotalLen) && isNaN(CurrentLen)){\n" \
"}else{\n" \
"var node = document.getElementById('progress_bar');\n" \
"if(TotalLen > 0 && CurrentLen > 0){\n" \
"var currentbytes=Math.round(CurrentLen* 100 /TotalLen);\n" \
"var w    = Math.round(currentbytes * 300 /100);\n" \
"if(currentbytes < 100)\n" \
"KeepQuery=\"1\";\n" \
"else\n" \
"KeepQuery=\"0\";\n" \
"node.style.width =parseInt(w)+\"px\";\n" \
"document.getElementById('progressNumber').innerHTML = 'Please wait... '+currentbytes.toString() + '%';\n" \
"}else{\n" \
"document.getElementById('progressNumber').innerHTML = 'Please wait... ';\n" \
"}}}\n" 

#define indexdata_head_04_4 \
"function __update_pageIE()\n" \
"{\n" \
"if (__AjaxReqIE != null && __AjaxReqIE.readyState == 4)\n" \
"{\n" \
"if (__AjaxReqIE.responseText.substring(0,3)==\"var\")\n" \
"{\n" \
"eval(__AjaxReqIE.responseText);\n" \
"switch (__result[0])\n" \
"{\n" \
"case \"OK\":\n" \
"FillprogressBarIE(parseInt(__result[1], 10),parseInt(__result[2], 10));\n" \
"default :\n" \
"break;\n" \
"}\n" \
"if(KeepQuery == \"1\")\n" \
"setTimeout(\"__update_stateIE()\", 1000);\n" \
"delete __result;}}}\n" \
"function __update_stateIE()\n" \
"{__send_requestIE(\"/upload_st.htm?MKey=\"+Magic_Key);}\n"

#define indexdata_head_04_5 \
"function uploadFileIE()\n" \
"{\n" \
"document.usb_upload.submit();\n" \
"document.getElementById(\"uploadedfile\").disabled=true;\n" \
"document.getElementById(\"Upload\").disabled=true;\n" \
"__update_stateIE();\n" \
"show_div(true, \"progress_div\");\n" \
"show_div(true, \"progressNumber\");\n" \
"}\n" \
"function __send_requestIE(url)\n" \
"{\n" \
"if (__AjaxReqIE == null) __AjaxReqIE = __createRequest();\n" \
"__AjaxReqIE.open(\"GET\", url, true);\n" \
"__AjaxReqIE.onreadystatechange = __update_pageIE;\n" \
"__AjaxReqIE.send(null);}\n"


#define indexdata_head_04_6 \
"function chk_CurrentDIRuri(this_id)\n" \
"{\n" \
"var CurrentDIR=document.getElementById(\"current_directory\").innerHTML;\n" \
"var URIStringOffset;\n" \
"var URIStringA;\n" \
"var URIStringOffsetA;\n" \
"var URIStringB;\n" \
"var URIStringOffsetB;\n" \
"var URIStringC;\n" \
"var URIString;\n" \
"var herfURi=this_id.href;\n" \
"var CurrentherfURi=location.href;\n" \
"if(herfURi.indexOf(CurrentDIR) == -1 && herfURi.indexOf(CurrentherfURi) == -1){\n" \
"URIStringOffset = herfURi.indexOf(\"/\");\n" \
"URIStringA= herfURi.substr(URIStringOffset+1);\n" \
"URIStringOffsetA  = URIStringA.indexOf(\"/\");\n" \
"URIStringB= URIStringA.substr(URIStringOffsetA+1);\n" \
"URIStringOffsetB = URIStringB.indexOf(\"/\");\n" \
"URIStringC= URIStringB.substr(URIStringOffsetB+1);\n" \
"herfURi = CurrentDIR + URIStringC;\n" \
"URIString =encodeURIComponent(herfURi);\n" \
"}else\n"

#define indexdata_head_04_61 \
"{URIStringOffset = herfURi.indexOf(\"/\");\n" \
"URIStringA= herfURi.substr(URIStringOffset+1);\n" \
"URIStringOffsetA  = URIStringA.indexOf(\"/\");\n" \
"URIStringB= URIStringA.substr(URIStringOffsetA+1);\n" \
"URIStringOffsetB = URIStringB.indexOf(\"/\");\n" \
"URIStringC= URIStringB.substr(URIStringOffsetB+1);\n" \
"URIString =encodeURIComponent(URIStringC);\n" \
"}\n"


#define indexdata_head_04_62 \
"if(URIString.search(/\%257B/g) != -1){\n" \
"URIString = URIString.replace(/\%257B/g,\"%7B\");\n" \
"}\n" \
"if(URIString.search(/\%257D/g) != -1){\n" \
"URIString = URIString.replace(/\%257D/g,\"%7D\");\n" \
"}\n" \
"if(URIString.search(/\%2524/g) != -1){\n" \
"URIString = URIString.replace(/\%2524/g,\"%24\");\n" \
"}\n" \
"if(URIString.search(/\%252F/g) != -1){\n" \
"URIString = URIString.replace(/\%252F/g,\"%2F\");\n" \
"}\n" \
"if(URIString.search(/\%2520/g) != -1){\n" \
"URIString = URIString.replace(/\%2520/g,\"%20\");\n" \
"}\n" \
"if(BrowserDetect.browser==\"Explorer\"){\n" \
"this_id.href =\"/\"+URIString;\n" \
"}\n" \
"return true;\n" \
"}\n"




#define indexdata_head_1 \
"<table id=\"table1\" name=\"table1\" cellSpacing=1 cellPadding=2 width=100% border=0>\n" 

#define indexdata_head_2 \
"<tr><td colspan=\"4\"><hr size=1 noshade align=top></td></tr>\n"

static unsigned char indexdata_tail_A[]="<tr><td colspan=\"4\"><hr size=1 noshade align=top></td></tr></table>\n";



static unsigned char indexdata_tail_B[]="<div>" \
"<label for=\"fileToUpload\">" \
"Select a File to Upload:</label>\n" \
"&nbsp;&nbsp;<input type=\"file\" name=\"fileToUpload[]\" id=\"fileToUpload\" onchange=\"fileSelected();\" />\n" \
"&nbsp;&nbsp;<input type=\"button\" onclick=\"sendClicked()\" name=\"Upload\" id=\"Upload\" value=\"Upload\" /></div>\n<div id=\"fileName\" style=\"display:none\"></div><div id=\"fileSize\" style=\"display:none\"></div><div id=\"fileType\" style=\"display:none\"></div>\n" \
"&nbsp;&nbsp;<div id=\"progressNumber\" style=\"display:none\"></div>\n<div id=\"progress_div\" style=\"display:none;border: 1px solid black; width:200px; height:15px;\"><div id=\"progress_bar\" style=\"height:15px; width:0px; background-color:blue;\"/></div></div>" \
"</form>\n";


static unsigned char indexdata_tail_BIE[]="<div>" \
"<input id=\"uploadedfile\" type=\"file\" size=\"35\" name=\"uploadedfile\">&nbsp;&nbsp;\n" \
"<input type=\"button\" onclick=\"sendClicked()\" id=\"Upload\" value=\"Upload\" />\n" \
"</div>&nbsp;&nbsp;<div id=\"progressNumber\" style=\"display:none\"></div>\n" \
"<div id=\"progress_div\" style=\"display:none;border: 1px solid black; width:300px; height:15px;\">\n" \
"<div id=\"progress_bar\" style=\"height:15px; width:0px; background-color:blue;\"></div></div></form>\n";


#if 0
static	unsigned char indexdata_tail_B[]="Select File:<input id=\"uploadedfile\" type=\"file\" size=\"35\" name=\"uploadedfile\">&nbsp;&nbsp;\n" \
"<input id=\"Upload\" onclick=sendClicked(this.form) type=button name=\"Upload\" value=\"Upload\"></p></form>\n" \
"<script type=\"text/javascript\" language=\"javascript1.2\">\n" \
"var MWJ_progBar = 0;\n" \
"var myProgBar = new progressBar(1,'#000000', '#ffffff','#043db2',300,15,1);\n" \
"</script>";
#endif

#define SEG_SIZE 800



extern void websDefaultWriteEvent(request *wp);
extern int websGetTimeSinceMark(request *wp);





extern char upload_st[];

PATH_ENTRY_T *FileEntryHead=NULL;
PATH_ENTRY_T *DirEntryHead=NULL;
int CurrDIRCount=0;
int CurrFILECount=0;
int TestCount=0;
////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
int Upload_st(request *wp, int argc, char **argv)
{
	int nBytesSent=0;
	char	*name;
	char tmpBuf[256]={0};
	char MagicKey[32]={0};
	char *tmp_tokenS;
	char *tmp_tokenE;
	int copyLen=0;
	request *current=NULL;
	int found=0;
   	name = argv[0];
	if (name == NULL) {
   		fprintf(stderr, "Insufficient args\n");
   		return -1;
   	}
   	if ( !strcmp(name, "Stat") ) {
	         if(wp->query_string != NULL  && strstr(wp->query_string, "MKey=")){
            	
	            	tmp_tokenS =strstr(wp->query_string, "MKey=");
	            	tmp_tokenE = strstr(tmp_tokenS+strlen("MKey="), "&");
	            	if(tmp_tokenE == NULL){
	            		sprintf(MagicKey, "%s", tmp_tokenS+strlen("MKey="));
	            		///printf("%s:1 MagicKey=%s\n",__FUNCTION__,MagicKey);
	            	}else{
	            		copyLen = tmp_tokenE -(tmp_tokenS+strlen("MKey="));
	            		memcpy(MagicKey, tmp_tokenS+strlen("MKey="),copyLen);
	            		 MagicKey[copyLen]='\0';
	            		 //printf("%s:2 MagicKey=%s\n",__FUNCTION__,MagicKey);
	            	}

			 current = request_ready;
			  while (current) {
			  if(current->MagicKey[0] && !strcmp(current->MagicKey,MagicKey)){
					//printf("%s:Got WP,current->MagicKey=%s\n",__FUNCTION__,current->MagicKey);
					found=1;
					break; 
				}else{
			
				}
			  current = current->next;
			  }
	
		if(found==1){
	            	#if defined(ENABLE_LFS)
	            	//printf("TotalConetLen=%lld, BytesWrite=%lld\n",current->TotalContentLen, current->lenFileData);
	            	#else
	            	//printf("TotalConetLen=%d, BytesWrite=%d\n",current->TotalContentLen, current->lenFileData);
	            	#endif
	            	#if defined(ENABLE_LFS)
			sprintf(tmpBuf, "%lld\",\"%lld", current->TotalContentLen,current->lenFileData);
			#else
			sprintf(tmpBuf, "%d\",\"%d", current->TotalContentLen,current->lenFileData);
			#endif
		}else
			sprintf(tmpBuf, "%s", "0\",\"0");
            }else
            	sprintf(tmpBuf, "%s", "0\",\"0");
		nBytesSent=req_format_write(wp,tmpBuf);
		
	}
	return nBytesSent;
}

int dump_directory_index(request *wp, int argc, char **argv)
{
 	int nBytesSent=0;
 	DIR *dir;
	struct dirent *next;
	int total_count=0;
	int len=0;
	int tmplen = 512;
	char *str=NULL;
	//char *str1=NULL;
	char buffer[512]={0};

	/*
	TestCount++;
	if(TestCount%4){
		nBytesSent=req_format_write(wp, "connected");
	}else{
		nBytesSent=req_format_write(wp, "disconnected");
	}
	return nBytesSent;
	*/
	str = (char *)malloc(tmplen);
	if (str !=NULL ) {
		memset(str, 0x00,tmplen);
		//printf("request url=%s\n", wp->url);
		dir = opendir("/var/tmp/usb");
		if (!dir) {
			printf("Cannot open /var/tmp/usb");
			free(str);
			return 0;
		}

		while ((next = readdir(dir)) != NULL) {
	//printf("next->d_reclen=%d, next->d_name=%s\n",next->d_reclen, next->d_name);
			/* Must skip ".." */
			if (strcmp(next->d_name, "..") == 0)
				continue;
			if (strcmp(next->d_name, ".") == 0)
				continue;
			if (strcmp(next->d_name, "sda") == 0)
				continue;
			if (strcmp(next->d_name, "mnt_map") == 0)
				continue;
			if (strlen(next->d_name) < 4)
				continue;
			
			sprintf(buffer, "%s?",next->d_name);
			str = strcat_str(str, &len, &tmplen, buffer);
			total_count++;
		}
		closedir(dir);
		if (str[0]) {
			nBytesSent=req_format_write(wp, str);
		}
		free(str);
	}
 	return nBytesSent;
}
//Query=file=/sda1*X:\DUB-2540
//Query=file=/sda1*X:\DUB-2540_Package\DUB-2540_Package_Release\0917\usbip\branch-rtl819x-sdk-v2.2\boards\rtl8196c\image\fw.bin
int Check_directory_status(request *wp, int argc, char **argv)
{
		int nBytesSent=0;
	char *path=NULL;
	char *Pathname=NULL;
	char *PathnameContent=NULL;
	char *tmppath=NULL;
	char *ptr = strrchr(wp->query_string,'\\');    
	int filename_len=0;
	int pathname_len=0;
	FILE *destFile=NULL;
	//printf("Query=%s\n", wp->query_string);
	path=malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
	tmppath=malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
	if(path != NULL && tmppath != NULL){
			if(strstr(wp->query_string,"%")){
				if(rm_url_escape_char(wp->query_string, tmppath)==0){
					sprintf(wp->query_string, "%s", tmppath);
				}
			}
			ptr = strrchr(wp->query_string,'\\');
			Pathname = strstr(wp->query_string, "*");
			if(Pathname != NULL){
				pathname_len =  Pathname - (wp->query_string +5);
				PathnameContent = malloc(pathname_len+1);
				if(PathnameContent != NULL){
				 		snprintf(PathnameContent, pathname_len+1, "%s",wp->query_string+5); 
				 		//printf("PathnameContent=%s\n",PathnameContent);
						sprintf(path , "/tmp/usb%s/", PathnameContent);
					if( ptr != NULL){        
							ptr++;   
						filename_len = strlen(wp->query_string) - (ptr-wp->query_string); 
					 	//printf("pathname_len=%d, filename_len=%d\n",pathname_len, filename_len);
						//printf("1 path=%s, ptr=%s\n",path, ptr);
						strcat(path, ptr);
						//printf("2 path=%s\n", path);
					}else{ //firefox case
						strcat(path, Pathname+1);
					}
					
					memset(tmppath,0x00, strlen(wp->query_string)+strlen("/tmp/usb")+1);
					if(strstr(path,"%")){
						if(rm_url_escape_char(path, tmppath)==0){
								sprintf(path, "%s", tmppath);
						}
					}	
						destFile = fopen(path, "w+b");
						if(destFile != NULL){
								nBytesSent=req_format_write(wp, "OK");
								fclose(destFile);
						}else{
							if(ptr)
								nBytesSent=req_format_write(wp, "open file %s fail!", ptr);
							else
								nBytesSent=req_format_write(wp, "open file %s fail!",  Pathname+1);
						}
						
					free(PathnameContent);
			}else{
				nBytesSent=req_format_write(wp, "fail");
			}	
		}else{
			nBytesSent=req_format_write(wp, "fail");
		}
		free(tmppath);
		free(path);
	}else{
		nBytesSent=req_format_write(wp, "fail");
	}
	return nBytesSent;
}



#if defined(ENABLE_LFS)
void get_LastModified_Size(char *dest_time,  char *Size, time_t *entryTime, off64_t Length, int isdir)
#else
void get_LastModified_Size(char *dest_time,  char *Size, time_t *entryTime, unsigned long Length, int isdir)
#endif
{
	struct tm * tm_time;
	char *month_index[]={"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
	double kbyte_count=0.0;
	double mbyte_count=0.0;
	char strMon[32];

		if(*entryTime != 0){
			tm_time = localtime(entryTime);
			sprintf(strMon, "%s",month_index[tm_time->tm_mon]);
			sprintf(dest_time, "%02d-%s-%d %02d:%02d:%02d",tm_time->tm_mday, strMon, (tm_time->tm_year+ 1900),tm_time->tm_hour, tm_time->tm_min, tm_time->tm_sec);  
			//printf("%s:dest_time is %s\n", __FUNCTION__, dest_time); 
		}else{
			sprintf(dest_time,"%s", "-");
		}
		if(Length != 0){
			mbyte_count = (Length/1048576.0); //1M
			if(mbyte_count > 1.00){
				sprintf(Size, "%6.2fM", mbyte_count);
			}else {
				kbyte_count = (Length /1024.0); //1K
				if(kbyte_count >1.00){
					sprintf(Size, "%3.2fK", kbyte_count);
				}else{
					#if defined(ENABLE_LFS)
					sprintf(Size, "%lldbytes", Length);
					#else
					sprintf(Size, "%ldbytes", Length);
					#endif
				}
			}
		}else{
			if (isdir)
				sprintf(Size, "%s", "-");
			else
				sprintf(Size, "0");
		}
	return;	
}

char* dump_directory(int *docLength, char* path, int sort_type, int sort_order)
{
	char buffer[1024];
	char ParentDirectory[200];
	char fileLastModified[100];
	char fileSize[100];
	int len;
	int tmplen = 512;
	char *str=NULL;
	int i=0, j;
	PATH_ENTRY_Tp FILE_sp;
	PATH_ENTRY_Tp DIR_sp;

	create_directory_list(path,sort_type,sort_order);
	len=0;
	sprintf(fileLastModified, "%s", "-");
	sprintf(fileSize, "%s", "-");
	str = (char *)malloc(tmplen);
	if(str !=NULL ){
		get_ParentDirectory(ParentDirectory, path, 1);
		//printf("path=%s ParentDirectory=%s\n", path, ParentDirectory);
		sprintf(buffer, "<tr><td><img src=\"/graphics/back.gif\" border=0 width=16 height=16><a id=\"rowa10\" href=\"%s/\">Parent Directory </a></td><td></td><td></td><td></td></tr>\n",ParentDirectory);
		str = strcat_str(str, &len, &tmplen, buffer);
		
		for(i=0;i<CurrDIRCount;i++){
			DIR_sp = &DirEntryHead[i];
			if(DIR_sp->name[0]){
				get_LastModified_Size(fileLastModified, fileSize, &DIR_sp->mtime, DIR_sp->size, 1);
				if(DIR_sp->mtime==0 &&  DIR_sp->size==0){
					sprintf(buffer, "<tr style=\"display:none\"><td><img src=\"/graphics/dir.gif\" border=0 width=16 height=16><a id=\"row%d\" href=\"%s/\" onclick=\"return chk_CurrentDIRuri(this);\">%s/</a></td><td>%s</td><td>%s</td><td><input id=\"RemoveDIR\" onclick=\"sendClickedRemove('%s')\" type=button name=\"RemoveDIR\" value=\"Remove\" /></td></tr>\n",i+1,DIR_sp->name,DIR_sp->name,fileLastModified, "-",DIR_sp->name);
				}else{
					sprintf(buffer, "<tr><td><img src=\"/graphics/dir.gif\" border=0 width=16 height=16><a id=\"row%d\" href=\"%s/\" onclick=\"return chk_CurrentDIRuri(this);\">%s/</a></td><td>%s</td><td>%s</td><td><input id=\"RemoveDIR\" onclick=\"sendClickedRemove('%s')\" type=button name=\"RemoveDIR\" value=\"Remove\" /></td></tr>\n",i+1,DIR_sp->name,DIR_sp->name,fileLastModified, "-",DIR_sp->name);
				}
				str = strcat_str(str, &len, &tmplen, buffer);
			}
		}
		
		j=i;
		for(i=0;i<CurrFILECount;i++){
			FILE_sp = &FileEntryHead[i];
			if(FILE_sp->name[0]){
			get_LastModified_Size(fileLastModified, fileSize, &FILE_sp->mtime, FILE_sp->size, 0);
			if(FILE_sp->mtime==0 &&  FILE_sp->size==0){
				sprintf(buffer, "<tr style=\"display:none\"><td><img src=\"/graphics/text.gif\" border=0 width=16 height=16><a id=\"row%d\" href=\"%s\" onclick=\"return chk_CurrentDIRuri(this);\">%s</a></td><td>%s</td><td>%s</td><td><input id=\"RemoveFile\" onclick=\"sendClickedRemove('%s')\" type=button name=\"RemoveFile\" value=\"Remove\"></td></tr>\n",j+i+1, FILE_sp->name,FILE_sp->name, fileLastModified,fileSize,FILE_sp->name);
			}else{
				sprintf(buffer, "<tr><td><img src=\"/graphics/text.gif\" border=0 width=16 height=16><a id=\"row%d\" href=\"%s\" onclick=\"return chk_CurrentDIRuri(this);\">%s</a></td><td>%s</td><td>%s</td><td><input id=\"RemoveFile\" onclick=\"sendClickedRemove('%s')\" type=button name=\"RemoveFile\" value=\"Remove\"></td></tr>\n",j+i+1, FILE_sp->name,FILE_sp->name, fileLastModified,fileSize,FILE_sp->name);
			}
	      	str = strcat_str(str, &len, &tmplen, buffer);
	      }
		}
		str[len] = '\0';
		*docLength = len;
		
		//printf("doc len=%d\n",len);
		//printf("dest_buff=\n");
		//printf("%s\n", str);
		free_directory_list();
		
	 	return str;
	}else{
		return NULL;
	}
}




void free_directory_list(void)
{
	CurrFILECount=0;
	CurrDIRCount=0;
	if(FileEntryHead)
		free(FileEntryHead);
	if(DirEntryHead)
		free(DirEntryHead);
	FileEntryHead=NULL;
	DirEntryHead=NULL;
}




void formusbdisk_uploadfile(request *wp, char *path, char *query)
{
	char tmpBuf[100];
	char tmpurl[64];
	char Ip_address[64]={0};
	char *buf_url=NULL, *tmppath=NULL, *tmp_redir=NULL;
	char *str_file_name=NULL;
	char *tmp_str_file_name=NULL;
	char *str_file_path=NULL;
	char *tmp_str_path_name=NULL;
	char *str_file_path_content=NULL;
	char *cmd_buffer=NULL;
	char *tmp_query_path=NULL;
	char *tmp_redir_path=NULL;
	struct stat sbuf;
	int found=0;
	char *tmp_query_content=NULL;
	int copyLen=0;
	char *tokenS=NULL;
	//int test_index=0;
	
	//printf("client_stream=%s\n", wp->client_stream);
	//printf("%s, %d, query_string=%s\n", __FUNCTION__, __LINE__,wp->query_string);
	//printf("pathname=%s\n", wp->pathname);
	if (rm_url_escape_char2(wp->query_string) != 0) {
		printf("rm_url_escape_char2 fail\n");
	}
	//printf("query_string=%s\n", wp->query_string);
	//printf("%s, %d, query_string=%s\n", __FUNCTION__, __LINE__,wp->query_string);
	apmib_get( MIB_IP_ADDR,  (void *)tmpBuf);
	sprintf(Ip_address, "%s", inet_ntoa(*((struct in_addr *)tmpBuf)));	
	buf_url = malloc(strlen(wp->query_string)+strlen("/tmp/usb")+23);
	tmp_query_content = malloc(strlen(wp->query_string)+strlen("/tmp/usb")+23);
	//printf("Query=%s\n", wp->query_string);
	//for(test_index=0;test_index<(strlen(wp->query_string));test_index++){
	//	printf("%02X ", (unsigned char)wp->query_string[test_index]);
	//}
	//printf("\n");
	if(strstr(wp->query_string, "&")){
		memset(tmp_query_content, 0x00, strlen(wp->query_string)+strlen("/tmp/usb")+23);
		tokenS = strstr(wp->query_string, "&");
		copyLen = tokenS -wp->query_string;
	       memcpy(tmp_query_content, wp->query_string,copyLen);
	       	tmp_query_content[copyLen]='\0';
	      // printf("%s:tmp_query_content=%s\n",__FUNCTION__, 	tmp_query_content);
	}
	
	
	
	if(buf_url != NULL && tmp_query_content !=NULL){
		if(tmp_query_content[0]){
			if(!strncmp((tmp_query_content+9), "\x25\x32\x46", 3))
				sprintf(buf_url, "%s/",tmp_query_content+9+3);
			else
				sprintf(buf_url, "%s",tmp_query_content+9);
		}else if (wp->query_string[0]) {
			//sprintf(buf_url, "http://%s%s/", Ip_address, wp->query_string+9);
			if(!strncmp((wp->query_string+9), "\x25\x32\x46", 3)) // %2F => '/'
				sprintf(buf_url, "%s",wp->query_string+9+3);
			else
				sprintf(buf_url, "%s",wp->query_string+9);
		} else {
			sprintf(buf_url, "%s", "/");
		}
	}
	else {
		sprintf(tmpurl,"%s", "/");
		sprintf(upload_st, "line=%d:out of memory for Req path", __LINE__);
		send_redirect_perm(wp, tmpurl);
		return;
	}
	
	//printf("wp->pathname=%s, wp->query_string=%s\n",wp->pathname, wp->query_string);
	if (!strncmp(wp->query_string, "del_file=", 9)) {
//		printf("%s:alloc string length=%d\n",__FUNCTION__, strlen(wp->query_string)+strlen("/tmp/usb")+1);
		str_file_name = malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
		tmp_str_file_name = malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
		tmp_str_path_name = malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
		str_file_path_content = malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
		tmppath=malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
		tmp_query_path=malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
		tmp_redir_path=malloc(strlen(wp->query_string)+strlen("/tmp/usb")+1);
		if ((str_file_name != NULL) && (tmp_str_file_name != NULL) &&
		    (tmp_str_path_name != NULL) && (str_file_path_content != NULL) &&
		    (tmppath !=NULL) && (tmp_query_path !=NULL) && (tmp_redir_path != NULL)) {
			if (strstr(wp->query_string,"%")) {
				if (rm_url_escape_char(wp->query_string, tmppath)==0) {
					sprintf(tmp_query_path, "%s", tmppath);
					str_file_path = get_filenPath(tmp_query_path+9);
					snprintf(str_file_path_content, str_file_path-(tmp_query_path+9)+1,"%s",tmp_query_path+9); 
					sprintf(str_file_name, "/tmp/usb%s", tmp_query_path+9);
				}
				else {
					str_file_path = get_filenPath(wp->query_string+9);
					snprintf(str_file_path_content, str_file_path-(wp->query_string+9)+1,"%s",wp->query_string+9); 
					sprintf(str_file_name, "/tmp/usb%s", wp->query_string+9);
				}
			}
			else {
				str_file_path = get_filenPath(wp->query_string+9);
				snprintf(str_file_path_content, str_file_path-(wp->query_string+9)+1,"%s",wp->query_string+9); 
				sprintf(str_file_name, "/tmp/usb%s", wp->query_string+9);
			}
			
			if (strstr(str_file_name,"%")) {
				if (rm_url_escape_char(str_file_name, tmp_str_file_name)==0) {
					sprintf(str_file_name, "%s", tmp_str_file_name);
				}
			}
			
			if (strstr(str_file_path_content,"%")) {
				if (rm_url_escape_char(str_file_path_content, tmp_str_path_name)==0) {
					sprintf(str_file_path_content, "%s", tmp_str_path_name);
				}
			}
			
			free(tmp_str_path_name);
			tmp_str_path_name=NULL;
			free(tmppath);
			tmppath=NULL;
			free(tmp_str_file_name);
			tmp_str_file_name=NULL;
			//printf("%s:str_file_name=%s,length=%d, str_file_path_content=%s\n", __FUNCTION__,str_file_name,  strlen(str_file_name), str_file_path_content);
		}
		else {
			sprintf(tmpurl,"%s", "/");
			sprintf(upload_st, "line=%d:out of memory for Req path", __LINE__);
			send_redirect_perm(wp, tmpurl);
			if (buf_url != NULL) {
				free(buf_url);
			}	
			if(str_file_name != NULL)
				free(str_file_name);
			if(tmp_str_file_name != NULL)
				free(tmp_str_file_name);
			if(tmp_str_path_name != NULL)
				free(tmp_str_path_name);
			if(str_file_path_content != NULL) 
				free(str_file_path_content);
			if(tmppath !=NULL)
				free(tmppath);
			if(tmp_query_path!=NULL)
			 	free(tmp_query_path);
			if(tmp_redir_path != NULL)
			 	free(tmp_redir_path);
			return;
		}
		//printf("remove file %s\n", str_file_name);
		if(remove_file(str_file_name)==0){
			tmp_redir=find_Last2F_boundary(wp->query_string+9, strlen(wp->query_string)-9, "\x25\x32\x46",3,&found);
			if (tmp_redir != NULL && found) {
				if (!strncmp((wp->query_string+9), "\x25\x32\x46", 3))
					snprintf(tmp_redir_path, tmp_redir-(wp->query_string+9)+1-3,"%s",wp->query_string+9+3); 
				else
					snprintf(tmp_redir_path, tmp_redir-(wp->query_string+9)+1,"%s",wp->query_string+9); 
				sprintf(buf_url, "%s/", tmp_redir_path);
			}
			else {
				sprintf(buf_url, "%s", str_file_path_content);
			}
			sync();
		}else{
			printf("Can NOT Stat str_file_name=%s, str_file_path_content=%s\n",str_file_name, str_file_path_content);
			sprintf(upload_st, "line=%d:file is not exist", __LINE__);
			sprintf(tmpurl,"%s", "/");
			send_redirect_perm(wp, tmpurl);
			printf("file is not exist\n");
			
			if (buf_url != NULL)
				free(buf_url);
			if(str_file_name != NULL)
				free(str_file_name);
			if(str_file_path_content != NULL)
				free(str_file_path_content);
			if(tmp_str_path_name != NULL)
				free(tmp_str_path_name);
			if(tmp_str_file_name != NULL)
				free(tmp_str_file_name);
			if(tmppath !=NULL)
				free(tmppath);
			if(tmp_query_path!=NULL)
			 	free(tmp_query_path);
			 if(tmp_redir_path != NULL)
			 	free(tmp_redir_path);
			return;
		}
		
		
	
//remove this code since we use rm from busybox	
#if 0		
		memset(&sbuf, 0x00, sizeof(sbuf));
		//printf("str_file_name=%s\n", str_file_name);
		if (stat(str_file_name, &sbuf)==0) {
			cmd_buffer = malloc(strlen(str_file_name)+strlen("rm -Rf ")+10);
			if (cmd_buffer != NULL) {
				if (sbuf.st_mode & S_IFDIR) { //del folder
					sprintf(cmd_buffer, "rm -Rf \"%s\"", str_file_name);
					system(cmd_buffer);
				}
				else { //del file
					//unlink(str_file_name);
					sprintf(cmd_buffer, "rm -f \"%s\"", str_file_name);
					system(cmd_buffer);
				}
sleep(2);
			sync();
//				sprintf(buf_url, "http://%s%s", Ip_address,str_file_path_content);
//				sprintf(buf_url, "http://%s%s", Ip_address,wp->query_string+9);
				//printf("str_file_path_content=%s\n", str_file_path_content);
				tmp_redir=find_Last2F_boundary(wp->query_string+9, strlen(wp->query_string)-9, "\x25\x32\x46",3,&found);
				if (tmp_redir != NULL && found) {
					if (!strncmp((wp->query_string+9), "\x25\x32\x46", 3))
						snprintf(tmp_redir_path, tmp_redir-(wp->query_string+9)+1-3,"%s",wp->query_string+9+3); 
					else
						snprintf(tmp_redir_path, tmp_redir-(wp->query_string+9)+1,"%s",wp->query_string+9); 
					sprintf(buf_url, "%s/", tmp_redir_path);
				}
				else {
					sprintf(buf_url, "%s", str_file_path_content);
				}
				free(cmd_buffer);
			}
			else {
				sprintf(tmpurl,"%s", "/");
				sprintf(upload_st, "line=%d:out of memory for Req path", __LINE__);
				send_redirect_perm(wp, tmpurl);
				if (buf_url != NULL) {
					free(buf_url);
				}	
				if(str_file_path_content != NULL)
					free(str_file_path_content);
				if(tmp_str_path_name != NULL)
					free(tmp_str_path_name);
				if(tmp_str_file_name != NULL)
					free(tmp_str_file_name);
				if(str_file_name != NULL)
					free(str_file_name);
				if(tmppath !=NULL)
					free(tmppath);
				 if(tmp_query_path!=NULL)
				 	free(tmp_query_path);
				 if(tmp_redir_path != NULL)
					free(tmp_redir_path);	
				return;
			}
		}
		else {
			sprintf(upload_st, "line=%d:file is not exist", __LINE__);
			sprintf(tmpurl,"%s", "/");
			send_redirect_perm(wp, tmpurl);
			printf("file is not exist\n");
			if (buf_url != NULL)
				free(buf_url);
			if(str_file_name != NULL)
				free(str_file_name);
			if(str_file_path_content != NULL)
				free(str_file_path_content);
			if(tmp_str_path_name != NULL)
				free(tmp_str_path_name);
			if(tmp_str_file_name != NULL)
				free(tmp_str_file_name);
			if(tmppath !=NULL)
				free(tmppath);
			if(tmp_query_path!=NULL)
			 	free(tmp_query_path);
			 if(tmp_redir_path != NULL)
			 	free(tmp_redir_path);
			return;
		}
#endif		
		if(str_file_name != NULL)
			free(str_file_name);
		if(str_file_path_content != NULL)
			free(str_file_path_content);
		if(tmp_query_path != NULL)
			free(tmp_query_path);
		if(tmp_redir_path != NULL)
			 free(tmp_redir_path);
	}
	//printf("buf_url=%s\n", buf_url);
	//for(test_index=0;test_index<(strlen(buf_url));test_index++){
	//	printf("%02X ", (unsigned char)buf_url[test_index]);
	//}
	//printf("\n");
	//send_redirect_perm(wp, buf_url);
	
	if(!strncmp(wp->query_string, "del_file=", 9)){	
		send_redirect_perm(wp, buf_url);
	}else if(!strncmp(wp->query_string, "dest_dir=", 9)){
		
		if(strstr(wp->UserBrowser, "MSIE")){
			send_redirect_perm(wp, buf_url);
		}else{
			//websResponse(wp, 200, NULL, NULL);
		}
		
	}	
	
	
	
	if (buf_url != NULL)
		free(buf_url);
}





#ifdef HTTP_FILE_SERVER_HTM_UI
void formHttpFilePathRedirect(request *req,char* http_C,char* http_O)
{
	if(!req)
		return;
	bzero(httpfile_dirpath,sizeof(httpfile_dirpath));
	strcpy(httpfile_dirpath,req->pathname);
	if(http_C) httpfile_type=http_C[0];
	else httpfile_type=0;
	if(http_O) httpfile_order=http_O[0];
	else httpfile_order=0;
	//free(req->pathname);
	//req->pathname=strdup("http_files_dir.htm");
	//printf("%s:%d httpfile_dirpath=%s http_C=%c http_O=%c\n",
	//__FUNCTION__,__LINE__,httpfile_dirpath,httpfile_type,httpfile_order);
	send_redirect_perm(req,"http_files_dir.htm");
}
int getParentDirectory(char *parentDir, char *currentDir)
{
	if(!parentDir || !currentDir)
		return -1;
	strcpy(parentDir,currentDir);
	if(strcmp(parentDir,"/")!=0)
	{
		int count_=0;
		int last_=strlen(parentDir)-1;	
		parentDir[last_]=0;
		for(count_=last_;parentDir[count_]!='/';count_--)
		{
			parentDir[count_]=0;
		}
	}
	return 0;
}
#endif

int RunCmd(char *const argv[], char *file)
{    
	pid_t pid;
	int status;
	int fd;
	char _msg[30];
	switch (pid = fork()) {
			case -1:	/* error */
				perror("fork");
				return errno;
			case 0:	/* child */
				
				signal(SIGINT, SIG_IGN);
				if(file){
					if((fd = open(file, O_RDWR | O_CREAT))==-1){ /*open the file */
						sprintf(_msg, "open %s", file); 
  						perror(_msg);
  						exit(errno);
					}
					dup2(fd,STDOUT_FILENO); /*copy the file descriptor fd into standard output*/
					dup2(fd,STDERR_FILENO); /* same, for the standard error */
					close(fd); /* close the file descriptor as we don't need it more  */
				}else{
			#ifndef DOSYS_DEBUG		
					close(2); //do not output error messages
			#endif	
				}
				setenv("PATH", "/sbin:/bin:/usr/sbin:/usr/bin", 1);
				execvp(argv[0], argv);
				perror(argv[0]);
				exit(errno);
			default:	/* parent */
			{
				
				waitpid(pid, &status, 0);
			#ifdef DOSYS_DEBUG	
				if(status != 0)
					printf("parent got child's status:%d, cmd=%s %s %s\n", status, argv[0], argv[1], argv[2]);
			#endif		
				if (WIFEXITED(status)){
			#ifdef DOSYS_DEBUG	
					printf("parent will return :%d\n", WEXITSTATUS(status));
			#endif		
					return WEXITSTATUS(status);
				}else{
					
					return status;
				}
			}
	}
}
int ExecuteSystemCmd(char *filepath, ...)
{
	va_list argp;
	char *argv[24]={0};
	int status;
	char *para;
	int argno = 0;
	va_start(argp, filepath);

	while (1){ 
		para = va_arg( argp, char*);
		if ( strcmp(para, "") == 0 )
			break;
		argv[argno] = para;
		//printf("Parameter %d is: %s\n", argno, para); 
		argno++;
	} 
	argv[argno+1] = NULL;
	status = RunCmd(argv, filepath);
	va_end(argp);
	return status;
}

void CheckUA(request *wp)
{
	char tmpUsrAgent[256]={0};
	char  *str_Token=NULL;
	char  *str_subToken=NULL;
             memset(wp->UserBrowser, 0x00, 32);
            
            snprintf(tmpUsrAgent, 256, "%s", wp->header_user_agent);
            if(tmpUsrAgent[0]){
			
			str_Token = strstr(tmpUsrAgent, "MSIE");
			if(str_Token){
				str_subToken =  strstr(str_Token, ";");
				if(str_subToken)
					snprintf(wp->UserBrowser, (str_subToken-str_Token)+1, "%s",str_Token); 
				else
					sprintf(wp->UserBrowser, "%s","MSIE");
			}else {
				str_Token = strstr(tmpUsrAgent, "Firefox");
				if(str_Token)
					snprintf(wp->UserBrowser, 32, "%s",str_Token); 
				else{
					str_Token = strstr(tmpUsrAgent, "Chrome");
					if(str_Token){
						str_subToken =  strstr(str_Token, " ");
						if(str_subToken)
							snprintf(wp->UserBrowser, (str_subToken-str_Token)+1, "%s",str_Token); 
						else
							sprintf(wp->UserBrowser, "%s","Chrome");
					}else{
						str_Token = strstr(tmpUsrAgent, "Android");
						if(str_Token){
							str_subToken =  strstr(str_Token, ";");
							if(str_subToken)
								snprintf(wp->UserBrowser, (str_subToken-str_Token)+1, "%s",str_Token); 
							else
								sprintf(wp->UserBrowser, "%s","Android");
						}else{
						
							str_Token = strstr(tmpUsrAgent, "Safari");
							if(str_Token){
								snprintf(wp->UserBrowser, 32, "%s",str_Token); 
							}else{
								memset(wp->UserBrowser, 0x00, 32);
							}
						}
					}
				}
			}
			//printf("UserBrowser=%s\n",wp->UserBrowser);
		}
}








static void MKey_Get_ascii(unsigned long code, char *out)
{
	*out++ = '0' + ((code / 10000000) % 10);  
	*out++ = '0' + ((code / 1000000) % 10);
	*out++ = '0' + ((code / 100000) % 10);
	*out++ = '0' + ((code / 10000) % 10);
	*out++ = '0' + ((code / 1000) % 10);
	*out++ = '0' + ((code / 100) % 10);
	*out++ = '0' + ((code / 10) % 10);
	*out++ = '0' + ((code / 1) % 10);
	*out = '\0';
}
static int MKey_Get_checksum(unsigned long int PIN)
{
	unsigned long int accum = 0;
	int digit;
	
	PIN *= 10;
	accum += 3 * ((PIN / 10000000) % 10); 	
	accum += 1 * ((PIN / 1000000) % 10);
	accum += 3 * ((PIN / 100000) % 10);
	accum += 1 * ((PIN / 10000) % 10); 
	accum += 3 * ((PIN / 1000) % 10); 
	accum += 1 * ((PIN / 100) % 10); 
	accum += 3 * ((PIN / 10) % 10);

	digit = (accum % 10);
	return (10 - digit) % 10;
}
void GetMagicKey(char *Key)
{
	struct timeval tod;
	unsigned long num;
	char tmp1[32]={0}, Magic[32]={0};
	
		gettimeofday(&tod , NULL);

		apmib_get(MIB_HW_NIC0_ADDR, (void *)&tmp1);			
		tod.tv_sec += tmp1[5]+tmp1[4];		
		srand(tod.tv_sec);
		num = rand() % 10000000;
		num = num*10 + MKey_Get_checksum(num);
		MKey_Get_ascii((unsigned long)num, Magic);
		sprintf(Key, "%s", Magic);
}
			

void websCheckAction(request *wp)
{
	char *tmp_tokenS;
	char *tmp_tokenE;
	int copyLen=0;
	if (strstr(wp->pathname, "formusbdisk_uploadfile")) {
		if (!strncmp(wp->query_string, "dest_dir=", 9))
			wp->FileUploadAct = 1;
		else if (!strncmp(wp->query_string, "del_file=", 9))
			wp->FileUploadAct = 2;
		else if (!strncmp(wp->query_string, "check_file=", 11))
			wp->FileUploadAct = 3;
		else
			wp->FileUploadAct = 0;
			
		if(wp->query_string != NULL  && strstr(wp->query_string, "MKey=")){
            	
	            	tmp_tokenS =strstr(wp->query_string, "MKey=");
	            	tmp_tokenE = strstr(tmp_tokenS+strlen("MKey="), "&");
	            	if(tmp_tokenE == NULL){
	            		sprintf(wp->MagicKey, "%s",tmp_tokenS+strlen("MKey="));
	            	}else{
	            		copyLen = tmp_tokenE -(tmp_tokenS+strlen("MKey="));
	            		memcpy(wp->MagicKey, tmp_tokenS+strlen("MKey="),copyLen);
	            		 wp->MagicKey[copyLen]='\0';
	            	}
		}
	}
}

char* websGetFileStart(request *wp, char *srcbuf, int buf_len, char *fileName, int *FisrtLength)
{
	char *tmpStartStr = NULL;
	char *tmpEndStr = NULL;
	char tmpNameStr[1024];
	int tmpValLenByte = 0;
	char *part;
	char *endPart=srcbuf;
	char *endLine;
	char *fileContent = NULL;
	int lineLen;
	char *buf=NULL;
	char *token=NULL, *savestr1=NULL;
	char tmpBuff[200];
	
	while ((endPart != NULL) && ((part = strstr(endPart, wp->multipart_boundary)) != NULL)) {
		part = strchr(part, '\n');
        	if (part == NULL) {
			endPart = part;
		}
		else {
			part++;
			/* supposed to be the Content-Disposition line */
			if (strstr(part, "Content-Disposition") == NULL) {
				endPart = part;
			} else {
				endLine = strchr(part, '\n');
				if (endLine != NULL) {
					int endOfHeader = 0;
					buf = malloc(BUFFER_SIZE+1);
					if (buf==NULL) {
						//fileContent=NULL;
						return NULL;
					}
					lineLen = (int)endLine - (int)part;
					lineLen = (lineLen < BUFFER_SIZE) ? lineLen : (BUFFER_SIZE-1);
					strncpy(buf, part, lineLen);
					buf[lineLen] = 0;
					while (!endOfHeader) {
              					/*
               					 * look for the next empty line that separates
               					 * between the header and the body of the part
               					 */
              					part = strchr(part, '\n');
              					if (part == NULL) {
                  					break;
              					}
              					else {
                  					part++;
              						if ((*part) == '\n') {
                  						part +=1;
                  						endOfHeader = 1;
              						}
              						else if (((*part) == '\r') && ((*(part+1)) == '\n')) {
								part += 2;
                  						endOfHeader = 1;
              						}
            					}    /* if (part != NULL) */
           				}    /* while (!endOfHeader) */
					fileContent = part;
					*FisrtLength=buf_len- (part - srcbuf);
					if (endOfHeader) {
						if ((tmpStartStr = strstr(buf, "filename=")) != NULL) {
							if ((tmpEndStr = strchr(&tmpStartStr[10], '\"')) != NULL) {
								if ((tmpValLenByte = ((int)tmpEndStr - (int)&tmpStartStr[10])) > 0) {
									tmpValLenByte = (tmpValLenByte < BUFFER_SIZE) ? tmpValLenByte : (BUFFER_SIZE-1);
									strcpy(tmpNameStr, "filename=");
									strncat(tmpNameStr, &tmpStartStr[10], tmpValLenByte);
									if(strstr(tmpNameStr, "\\")){
										token=NULL;
										savestr1=NULL;	     
										sprintf(tmpBuff, "%s", tmpNameStr);     
										token = strtok_r(tmpBuff,"\\", &savestr1);
										do {
											if (token == NULL) /*check if the first arg is NULL*/
												break;
											else
												sprintf(fileName, "%s", token);
											token = strtok_r(NULL, "\\", &savestr1);
										} while(token !=NULL);
									}
									else {
										sprintf(fileName, "%s", tmpNameStr+9);
									}
								}
							}    
						}
						endPart = part;
					}
					else {
						endPart = part;
					}
				}
			}
		}
	}   
	free(buf);
	return fileContent;
}
char* find_multiPartBoundary(char *data, int dlen, char *pattern, int plen, int *result)
{	
	int i;	
	char *end;	
	
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

int websDataWrite2File(request *wp, char *socket_data, int data_len)
{
	char* uploadedFileContent=NULL;
	char* uploadedFileContent_end=NULL;
	char file_name[256]={0};
	char tmp_path[32]={0};
	int file_content_len_1=0;
	int file_write_len=0;
	char* file_path=NULL;
#if defined(ENABLE_LFS)			
	off64_t last_clen=0;
#else
	int last_clen=0;
#endif		
	int last_nbytes=0;
	int Current_filelen=0;
	char	*tempbuf=NULL;
	char *tmp_query_path=NULL;
	char *tmp_query_content=NULL;
	int copyLen=0;
	char *tokenS=NULL;
	int result=0;
	//int dump_index=0; //for debug only
	int dump_data_index=0;
	int result_last=0;
	int truncat_length=0;
	int cmdRet=0;
	if (wp->FileUploadAct==1) {  //file upload action
		struct stat sbuf;
		wp->req_timeout_count=0;
		if ((wp->destfile ==NULL) && (wp->destpath ==NULL) && (wp->req_error_state==0)) {
			uploadedFileContent = websGetFileStart(wp, socket_data, data_len, file_name, &file_content_len_1);
			if (uploadedFileContent) {
				sprintf(tmp_path,"%s", "/tmp/usb");
				wp->destpath = malloc(strlen(tmp_path)+strlen(wp->query_string)+strlen(file_name)+1);
				file_path = malloc(strlen(tmp_path)+strlen(wp->query_string)+strlen(file_name)+1);
				tmp_query_path = malloc(strlen(tmp_path)+strlen(wp->query_string)+strlen(file_name)+1);
				tmp_query_content = malloc(strlen(tmp_path)+strlen(wp->query_string)+strlen(file_name)+1); 

				if ((wp->destpath != NULL) && (file_path != NULL) && (tmp_query_path != NULL) && (tmp_query_content != NULL) && (wp->req_error_state==0)) {
					memset(tmp_query_path, 0x00, strlen(tmp_path)+strlen(wp->query_string)+strlen(file_name)+1);
					memset(tmp_query_content, 0x00, strlen(tmp_path)+strlen(wp->query_string)+strlen(file_name)+1);
					if(wp->query_string != NULL && strstr(wp->query_string, "&")){
						tokenS = strstr(wp->query_string, "&");
						copyLen = tokenS -wp->query_string;
	            				memcpy(tmp_query_content, wp->query_string,copyLen);
	            		 		tmp_query_content[copyLen]='\0';
					}
					if(tmp_query_content[0]){
						if(strstr(tmp_query_content,"%")){
							if(rm_url_escape_char(tmp_query_content, file_path)==0){
								sprintf(tmp_query_path, "%s", file_path);
							}
						}	
					}else{
						if (wp->query_string != NULL && strstr(wp->query_string,"%")) {
							if (rm_url_escape_char(wp->query_string, file_path)==0) {
								sprintf(tmp_query_path, "%s", file_path);
							}
						}	
					}
					memset(file_path, 0x00, strlen(tmp_path)+strlen(wp->query_string)+strlen(file_name)+1);
					sprintf(wp->destpath, "%s%s/%s", tmp_path, tmp_query_path+9, file_name);
					if (rm_url_escape_char(wp->destpath, file_path)==0) {
						sprintf(wp->destpath,"%s",file_path); 
					}
					free(file_path);
					free(tmp_query_path);
					free(tmp_query_content);
					if (stat(wp->destpath, &sbuf) == 0) {
						unlink(wp->destpath);
						usleep(60000);
					}
					wp->destfile = fopen(wp->destpath, "w+b");
					if ((wp->destfile !=NULL) && (wp->req_error_state==0)) {
						//system("cat /proc/uptime");
						file_write_len = fwrite(uploadedFileContent, 1, file_content_len_1,wp->destfile);
						wp->lenFileData += file_write_len;
						if (file_write_len !=file_content_len_1) {
							printf("%d:file write error, file_write_len=%d, expect len=%d\n",__LINE__,file_write_len,file_content_len_1);
							sprintf(upload_st, "line=%d:file write error, file_write_len=%d, expect len=%d", __LINE__, file_write_len,file_content_len_1);
							wp->req_error_state=1;
						}
						//sync();
						if (!(wp->clen - data_len > 0)) { //means the first segment include the end of bounary
							tempbuf = malloc((BUFFER_SIZE)+1);
							if (wp->req_error_state==0 && tempbuf != NULL) {
								Current_filelen = ftell(wp->destfile);  
								fseek(wp->destfile, -(file_content_len_1), SEEK_CUR);
								memset(tempbuf, 0x00, (BUFFER_SIZE)+1);
								dump_data_index = fread(tempbuf,1, (file_content_len_1),wp->destfile);
								uploadedFileContent_end = find_multiPartBoundary(tempbuf, (file_content_len_1), wp->multipart_boundary, strlen(wp->multipart_boundary),&result_last);
								if (result_last==1 && uploadedFileContent_end != NULL) {
									uploadedFileContent_end = uploadedFileContent_end-2;
									truncat_length = ((file_content_len_1)-(uploadedFileContent_end-tempbuf));
									fseek(wp->destfile, Current_filelen, SEEK_SET);
									truncate(wp->destpath, Current_filelen-truncat_length); 
									wp->lenFileData = Current_filelen-truncat_length;
									fclose(wp->destfile);
									sync();
									wp->destfile=NULL;
									system(DROP_CACHES);
								}
								free(tempbuf);		
							}
							if(wp->req_error_state==0)
								sprintf(upload_st, "%s", "File upload successfully!");
							return 1;
						}
					}
					else {
						printf("open file error\n");
						sprintf(upload_st, "line=%d:open file : %s error", __LINE__, file_name);
						wp->req_error_state=1;
					}
				}
				else {
					printf("line=%d:out of memory for Req path", __LINE__);
					sprintf(upload_st, "line=%d:out of memory for Req path", __LINE__);
					wp->req_error_state=1;
				}
			}
		}
		else {
			//if((data_len < BUFFER_SIZE) && wp->clen >104 && wp->req_error_state==0){
			if (wp->clen < BUFFER_SIZE && wp->req_error_state==0) {
				uploadedFileContent_end = find_multiPartBoundary(socket_data, data_len, wp->multipart_boundary, strlen(wp->multipart_boundary),&result);
				if (result ==1 && uploadedFileContent_end !=NULL) {
					#if 0
					printf("dump 1\n");
					for(dump_index=0;dump_index<data_len;dump_index++){
						printf("%02X ", (unsigned char)socket_data[dump_index]);
						if(dump_index > 0 && (dump_index%16==0)){
							printf("\n");
						}
					}
					printf("\n");
					#endif
					uploadedFileContent_end = uploadedFileContent_end-2;
					//printf("write last segment, length=%d, write to file\n", (uploadedFileContent_end-socket_data));
					#if 0
					for(dump_index=0;dump_index<(uploadedFileContent_end-socket_data);dump_index++) {
						printf("%02X ", (unsigned char)socket_data[dump_index]);
						if(dump_index > 0 && (dump_index%16==0)){
								printf("\n");
						}
					}
					printf("\n");
					#endif
				
					file_write_len = fwrite(socket_data, 1, (uploadedFileContent_end-socket_data),wp->destfile);
					wp->lenFileData += file_write_len;
					if (file_write_len !=(uploadedFileContent_end-socket_data)) {
						printf("%d:file write error, file_write_len=%d, expect len=%d\n",__LINE__,file_write_len,(uploadedFileContent_end-socket_data));
						sprintf(upload_st, "line=%d:file write error, file_write_len=%d, expect len=%d", __LINE__, file_write_len,file_content_len_1);
						wp->req_error_state=1;
					}
					fclose(wp->destfile);
					sync();
					wp->destfile=NULL;
					//system(DROP_CACHES);
					//system("cat /proc/uptime");
					//printf("%s:Line=%d, file len=%llu\n", __FUNCTION__, __LINE__, wp->lenFileData);
					cmdRet = ExecuteSystemCmd("/proc/sys/vm/drop_caches", "echo","3",EMPTY_STR);
					if(cmdRet != 0){
						//printf("%s:Line=%d, file len=%lld, drop cache is fail\n", __FUNCTION__, __LINE__, wp->lenFileData);
					}else{
						//printf("%s:Line=%d, file len=%lld\n", __FUNCTION__, __LINE__, wp->lenFileData);
					}
					
					if (wp->req_error_state==0)
						sprintf(upload_st, "%s", "File upload successfully!");
					return 1;
				}
				else {
					//printf("%s:write sub part that is not max socket len but not end part, nbytes=%d\n", __FUNCTION__,data_len);
					#if 0
					printf("%s:dump subpart data\n", __FUNCTION__);
					for(dump_index=0;dump_index<(data_len);dump_index++){
						printf("%02X ", (unsigned char)socket_data[dump_index]);
						if(dump_index > 0 && (dump_index%16==0)){
							printf("\n");
						}
					}
					printf("\n");
					#endif
					file_write_len = fwrite(socket_data, 1, data_len,wp->destfile);
					wp->lenFileData += file_write_len;
					if (file_write_len !=data_len) {
						printf("%d:file write error, file_write_len=%d, expect len=%d\n",__LINE__,file_write_len,data_len);
						sprintf(upload_st, "line=%d:file write error, file_write_len=%d, expect len=%d", __LINE__, file_write_len,file_content_len_1);
						wp->req_error_state=1;
					}
					//sync();
				}
			}
			else {
				if (wp->req_error_state==0) {
					file_write_len = fwrite(socket_data, 1, data_len,wp->destfile);
					wp->lenFileData += file_write_len;
					if (file_write_len !=data_len) {
						printf("%d:file write error, file_write_len=%d, expect len=%d\n",__LINE__,file_write_len,data_len);
						sprintf(upload_st, "line=%d:file write error, file_write_len=%d, expect len=%d", __LINE__, file_write_len,file_content_len_1);
						wp->req_error_state=1;
					}
					//sync();
				}
			}
		}
		last_clen = wp->clen;
		wp->clen -= data_len;
		if (wp->clen > 0) {
			if (data_len > 0) {
				last_nbytes=data_len;
				return 0;
			}
			return 2;
		}
		else {
			tempbuf = malloc((2*BUFFER_SIZE)+1);
			if(wp->req_error_state==0 && tempbuf != NULL){
				Current_filelen = ftell(wp->destfile);  
				//printf("%s:line =%d, Current_filelen=%d\n", __FUNCTION__, __LINE__, Current_filelen);
				fseek(wp->destfile, -(data_len+last_nbytes), SEEK_CUR);
				memset(tempbuf, 0x00, (2*BUFFER_SIZE)+1);
				dump_data_index = fread(tempbuf,1, (last_nbytes+data_len),wp->destfile);
				uploadedFileContent_end = find_multiPartBoundary(tempbuf, (last_nbytes+data_len), wp->multipart_boundary, strlen(wp->multipart_boundary),&result_last);
				if(result_last==1 && uploadedFileContent_end != NULL){
					uploadedFileContent_end = uploadedFileContent_end-2;
					truncat_length = ((last_nbytes+data_len)-(uploadedFileContent_end-tempbuf));
					//printf("extra length=%d\n",truncat_length);
					fseek(wp->destfile, Current_filelen, SEEK_SET);
					truncate(wp->destpath, Current_filelen-truncat_length); 
					//printf("%s:line=%d, file length=%d\n", __FUNCTION__, __LINE__,Current_filelen-truncat_length);
					wp->lenFileData = Current_filelen-truncat_length;
					fclose(wp->destfile);
					sync();
					wp->destfile=NULL;
					//system(DROP_CACHES);
					//system("cat /proc/uptime");
					//printf("%s:Line=%d, file len=%lld\n", __FUNCTION__, __LINE__, wp->lenFileData);
					
					cmdRet = ExecuteSystemCmd("/proc/sys/vm/drop_caches", "echo","3",EMPTY_STR);
					if(cmdRet != 0){
						//printf("%s:Line=%d, file len=%lld, drop cache is fail\n", __FUNCTION__, __LINE__, wp->lenFileData);
					}else{
						//printf("%s:Line=%d, file len=%lld\n", __FUNCTION__, __LINE__, wp->lenFileData);
					}
					
				}
				free(tempbuf);		
			}
		}
		//printf("wp->req_error_state=%d\n", wp->req_error_state);
		if (wp->req_error_state==0) {
			sprintf(upload_st, "%s", "File upload successfully!");
		}
		return 1;
	}
	else if (wp->FileUploadAct==2) {  //file delete action
		return 1;
	}
	return -1;
}

#if 1
int generate_directory_page(request *req, char *dest_filename)
{
	int bytes = 0;
	char *dir_dest_buf=NULL;
	int doc_length=0;
	int doc_index=0;
	int webWrite_length=0;
	char tmpBuf[SEG_SIZE+20]={0};
	char *op;
	char MagicKey[32]={0};
	send_r_request_ok2(req);
	bytes += req_format_write(req, "<html><head><title></title>\n");
	
	if(req->header_user_agent[0]){
		//fprintf(stderr, "%s:%d, UserBrowser=%s, req->request_uri=%s\n", __FUNCTION__, __LINE__, req->UserBrowser,req->request_uri);
	}	
	GetMagicKey(MagicKey);
	//javascript portion start
	bytes += req_format_write(req, "%s", indexdata_head_0);
	bytes += req_format_write(req, "%s", indexdata_head_00);
	bytes += req_format_write(req, "%s", indexdata_head_01);
	bytes += req_format_write(req, "%s", indexdata_head_02);
	bytes += req_format_write(req,"%s",indexdata_head_02_1);
	bytes += req_format_write(req, "%s", indexdata_head_03);
	bytes += req_format_write(req, "%s", indexdata_head_04);
	bytes+=req_format_write(req,"%s", indexdata_head_04_0);
	bytes+=req_format_write(req,"%s", indexdata_head_04_1);
	bytes+=req_format_write(req,"%s", indexdata_head_04_2);
	bytes+=req_format_write(req,"%s", indexdata_head_04_3);
	bytes+=req_format_write(req,"%s", indexdata_head_04_4);
	bytes+=req_format_write(req,"%s", indexdata_head_04_5);
	bytes+=req_format_write(req,"%s", indexdata_head_04_6);
	bytes+=req_format_write(req,"%s", indexdata_head_04_61);
	bytes+=req_format_write(req,"%s", indexdata_head_04_62);
	bytes += req_format_write(req, "%s", indexdata_head_05);
	bytes += req_format_write(req, "%s", indexdata_head_06);
	bytes += req_format_write(req, "%s", indexdata_head_07);
	bytes += req_format_write(req, "\nfunction __update_state(filename){var cur_dir=document.getElementById(\"current_directory\").innerHTML;if(BrowserDetect.browser==\"Explorer\"){__send_request(\"/usb_fileinfo.htm?file=\"+encodeURIComponent(cur_dir)+\"*\"+encodeURIComponent(filename));}else{__send_request(\"/usb_fileinfo.htm?file=%s*\"+filename);}}\n",req->request_uri);
	
	if(upload_st[0] && ((req->query_string != NULL && strstr(req->query_string, "upload_st"))||(req->UserBrowser[0] && strstr(req->UserBrowser, "MSIE")))){ //file upload status
		bytes += req_format_write(req, "\nfunction init(){ document.title = \"Index of %s\";Magic_Key=\"%s\";REDIR_URL=\"%s\";\nPOST_URL=\"/boafrm/formusbdisk_uploadfile?dest_dir=%%2F%s\";var upload_result=\"%s\";document.getElementById(\"current_directory\").innerHTML='%s';\nshow_div(0, \"progress_div\");BrowserDetect.init();if(upload_result == \"\"){}else{alert(upload_result);}}</script>\n",req->request_uri, MagicKey, req->request_uri ,&req->request_uri[1], upload_st,req->request_uri);  
		memset(upload_st, 0x00, 512);
	}else{
		bytes += req_format_write(req, "\nfunction init(){ document.title = \"Index of %s\";Magic_Key=\"%s\";REDIR_URL=\"%s\";\nPOST_URL=\"/boafrm/formusbdisk_uploadfile?dest_dir=%%2F%s\";var upload_result=\"\";document.getElementById(\"current_directory\").innerHTML='%s';\nshow_div(0, \"progress_div\");BrowserDetect.init();if(upload_result == \"\"){}else{alert(upload_result);}}</script>\n",req->request_uri, MagicKey, req->request_uri, &req->request_uri[1], req->request_uri);  
	}
	
	//javascript portion end
	
	bytes += req_format_write(req, "\n</head>\n<body onload=init();>\n<h1>Index of %s</h1>\n",
                     req->request_uri);
                     
	bytes += req_format_write(req, "%s", sub_str2);
	
	bytes += req_format_write(req, "%s", indexdata_head_1);
	//Parse C=N;O=D
	//printf("client_stream=%s\n", req->client_stream);
	//printf("query_string=%s\n", req->query_string);
	//printf("pathname=%s\n", req->pathname);
	if ((op = strstr(req->client_stream, "?C=")) != NULL) {
		req->req_sort_type = *(op+3);
		req->req_sort_order = *(op+7);
	}
	else {
		req->req_sort_type=0; //sort by name
		req->req_sort_order=0;
	}

	if ((req->req_sort_type !=0) && (req->req_sort_order !=0)) {
		if (req->req_sort_type=='N') {
			if (req->req_sort_order == 'D')
				CurSortOrderN = 'A';
			if(req->req_sort_order == 'A')
				CurSortOrderN = 'D';
		}
		else if (req->req_sort_type=='M') {
			if(req->req_sort_order == 'D')
				CurSortOrderM = 'A';
			if(req->req_sort_order == 'A')
				CurSortOrderM = 'D';
		}
		else if (req->req_sort_type=='S') {
			if(req->req_sort_order == 'D')
				CurSortOrderS = 'A';
			if(req->req_sort_order == 'A')
				CurSortOrderS = 'D';
		}
	}
	else {
		CurSortOrderN='D';
		CurSortOrderS='D';
		CurSortOrderM='D';
	}
				
	bytes += req_format_write(req, "<tr><td width=55%%><a id=\"rowa0\" href=\"%s?C=N;O=%c\">Name</a></td>\n",req->request_uri, CurSortOrderN);
	bytes += req_format_write(req, "<td width=25%%><a id=\"rowa1\" href=\"%s?C=M;O=%c\">Last modified</a></td>\n",req->request_uri,  CurSortOrderM);
	bytes += req_format_write(req, "<td width=10%%><a id=\"rowa2\" href=\"%s?C=S;O=%c\">Size</td><td width=10%%></td></tr>\n", req->request_uri,  CurSortOrderS);
	bytes += req_format_write(req, "%s", indexdata_head_2);
	
	//printf("req->pathname=%s dest_filename=%s doc_length=%d\n", req->pathname, dest_filename, doc_length);
	dir_dest_buf =dump_directory(&doc_length, req->pathname, req->req_sort_type, req->req_sort_order);
	//printf("req->pathname=%s dest_filename=%s doc_length=%d\n", req->pathname, dest_filename, doc_length);
	//printf("dir_dest_buf=%s\n", dir_dest_buf);
	if (doc_length > (SEG_SIZE)) {
		do {
			if ((webWrite_length+(SEG_SIZE)) > doc_length) {
				doc_index =  webWrite_length;
				break;
			}

			if (webWrite_length ==0) {
				//snprintf(tmpBuf, (SEG_SIZE), "%s", dir_dest_buf);
				memcpy(tmpBuf, dir_dest_buf, SEG_SIZE);
				tmpBuf[SEG_SIZE]='\0';
			}
			else {
				//snprintf(tmpBuf, (SEG_SIZE+1), "%s", dir_dest_buf+webWrite_length-1);
				memcpy(tmpBuf, dir_dest_buf+webWrite_length, SEG_SIZE);
				tmpBuf[SEG_SIZE]='\0';
			}
			bytes += req_format_write(req, "%s", tmpBuf);
			//printf("\n\n%s\n",tmpBuf);
			webWrite_length = webWrite_length+(SEG_SIZE);
		} while (webWrite_length < doc_length);

		if (doc_index>0) {
			//snprintf(tmpBuf, (doc_length-doc_index)+1, "%s", dir_dest_buf+doc_index-1);
			memcpy(tmpBuf, dir_dest_buf+doc_index, (doc_length-doc_index));
			tmpBuf[(doc_length-doc_index)]='\0';
			bytes += req_format_write(req, "%s", tmpBuf);
			//printf("\n\n%s\n",tmpBuf);
		}
	}
	else {
		bytes += req_format_write(req, "%s", dir_dest_buf);
	}
	free(dir_dest_buf);
	bytes += req_format_write(req, "%s", indexdata_tail_A);

	bytes += req_format_write(req, "<form name=\"usb_upload\" enctype=\"multipart/form-data\" action=\"/boafrm/formusbdisk_uploadfile?dest_dir=%s\" method=\"post\"><p>\n", req->request_uri);
	
	
	if(strstr(req->UserBrowser, "MSIE")){
		bytes+=req_format_write(req,"%s",indexdata_tail_BIE);
	}else
		bytes+=req_format_write(req,"%s",indexdata_tail_B);
	     
	//bytes += req_format_write(req, "%s", indexdata_tail_B);
	bytes += req_format_write(req, "<div id=\"current_directory\" style=\"display:none\"></div>");
	
	bytes += req_format_write(req, "</body>\n</html>\n");
	update_content_length(req);
	return bytes;
}
#else
int generate_directory_page(request *req, char *dest_filename)
{
	FILE *fds;
	int bytes = 0;
	char *dir_dest_buf=NULL;
	int doc_length=0;
	char *op;

	fds = fopen(dest_filename, "w");
	if (fds == NULL) {
		boa_perror(req, "dircache fopen");
		return 0;
	}
	bytes += fprintf(fds, "<html><head><title></title>\n");
	
	//javascript portion start
	bytes += fprintf(fds, "%s", indexdata_head_0);
	bytes += fprintf(fds, "%s", indexdata_head_00);
	bytes += fprintf(fds, "%s", indexdata_head_01);
	bytes += fprintf(fds, "%s", indexdata_head_02);
	bytes += fprintf(fds, "%s", indexdata_head_03);
	bytes += fprintf(fds, "%s", indexdata_head_04);
	bytes += fprintf(fds, "%s", indexdata_head_05);
	bytes += fprintf(fds, "%s", indexdata_head_06);
	bytes += fprintf(fds, "%s", indexdata_head_07);
	bytes += fprintf(fds, "\nfunction __update_state(filename){var cur_dir=document.getElementById(\"current_directory\").innerHTML;if(BrowserDetect.browser==\"Explorer\"){__send_request(\"/usb_fileinfo.htm?file=\"+encodeURIComponent(cur_dir)+\"*\"+encodeURIComponent(filename));}else{__send_request(\"/usb_fileinfo.htm?file=%s*\"+filename);}}\n",req->request_uri);
	if (upload_st[0]) //file upload status
		//sprintf(sub_str7, "function init(){ var upload_result=\"%s\";if(upload_result == \"File upload successfully!\"){location.href='http://192.168.1.254/home.htm'}else{alert(upload_result);}}</script></head><body onload=init();>",upload_st);   //debug only
		bytes += fprintf(fds, "\nfunction init(){ document.title = \"Index of %s\";var upload_result=\"%s\";document.getElementById(\"current_directory\").innerHTML='%s';show_div(0, \"progress_div\");BrowserDetect.init();if(upload_result == \"\"){}else{alert(upload_result);}}\n</script>",req->request_uri, upload_st,req->request_uri);
	else
		bytes += fprintf(fds, "\nfunction init(){ document.title = \"Index of %s\";var upload_result=\"\";document.getElementById(\"current_directory\").innerHTML='%s';show_div(0, \"progress_div\");BrowserDetect.init();if(upload_result == \"\"){}else{alert(upload_result);}}\n</script>",req->request_uri, req->request_uri);
	memset(upload_st, 0x00, 128);
	//javascript portion end
	
	bytes += fprintf(fds, "\n</head>\n<body onload=init();>\n<h1>Index of %s</h1>\n",
                     req->request_uri);
                     
	bytes += fprintf(fds, "%s", sub_str2);
	
	bytes += fprintf(fds, "%s", indexdata_head_1);
	
	//Parse C=N;O=D
	//printf("client_stream=%s\n", req->client_stream);
	//printf("query_string=%s\n", req->query_string);
	//printf("pathname=%s\n", req->pathname);
	if ((op = strstr(req->client_stream, "?C=")) != NULL) {
		req->req_sort_type = *(op+3);
		req->req_sort_order = *(op+7);
	}
	else {
		req->req_sort_type=0; //sort by name
		req->req_sort_order=0;
	}

	if ((req->req_sort_type !=0) && (req->req_sort_order !=0)) {
		if (req->req_sort_type=='N') {
			if (req->req_sort_order == 'D')
				CurSortOrderN = 'A';
			if(req->req_sort_order == 'A')
				CurSortOrderN = 'D';
		}
		else if (req->req_sort_type=='M') {
			if(req->req_sort_order == 'D')
				CurSortOrderM = 'A';
			if(req->req_sort_order == 'A')
				CurSortOrderM = 'D';
		}
		else if (req->req_sort_type=='S') {
			if(req->req_sort_order == 'D')
				CurSortOrderS = 'A';
			if(req->req_sort_order == 'A')
				CurSortOrderS = 'D';
		}
	}
	else {
		CurSortOrderN='D';
		CurSortOrderS='D';
		CurSortOrderM='D';
	}
				
	bytes += fprintf(fds, "<tr><td width=55%%><a id=\"rowa0\" href=\"%s?C=N;O=%c\">Name</a></td>\n",req->request_uri, CurSortOrderN);
	bytes += fprintf(fds, "<td width=25%%><a id=\"rowa1\" href=\"%s?C=M;O=%c\">Last modified</a></td>\n",req->request_uri,  CurSortOrderM);
	bytes += fprintf(fds, "<td width=10%%><a id=\"rowa2\" href=\"%s?C=S;O=%c\">Size</td><td width=10%%></td></tr>\n", req->request_uri,  CurSortOrderS);
	bytes += fprintf(fds, "%s", indexdata_head_2);
	
	//printf("req->pathname=%s dest_filename=%s doc_length=%d\n", req->pathname, dest_filename, doc_length);
	dir_dest_buf =dump_directory(&doc_length, req->pathname, req->req_sort_type, req->req_sort_order);
	//printf("req->pathname=%s dest_filename=%s doc_length=%d\n", req->pathname, dest_filename, doc_length);
	//printf("dir_dest_buf=%s\n", dir_dest_buf);
	bytes += fprintf(fds, "%s", dir_dest_buf);
	free(dir_dest_buf);
	
	bytes += fprintf(fds, "%s", indexdata_tail_A);

	bytes += fprintf(fds, "<form name=\"usb_upload\" enctype=\"multipart/form-data\" action=\"/boafrm/formusbdisk_uploadfile?dest_dir=%s\" method=\"post\"><p>\n", req->request_uri);
	bytes += fprintf(fds, "%s", indexdata_tail_B);
	bytes += fprintf(fds, "<div id=\"current_directory\" style=\"display:none\"></div>");
	
	bytes += fprintf(fds, "</body>\n</html>\n");

	fclose(fds);
	return bytes;
}
#endif

void http_file_server_req_init(request *wp)
{
	wp->re_set_req_timeout=0;
	wp->destfile=NULL;
	wp->FileUploadAct=0;
	wp->req_timeout_count=0;
	wp->req_sort_type=0;
	wp->req_sort_order=0;
	wp->req_error_state=0;
	wp->lenFileData = 0;
	wp->TotalContentLen=0;
	wp->destpath=NULL;
	memset(wp->MagicKey, 0x00, 32);
	memset(wp->UserBrowser, 0x00, 32);
}

void http_file_server_req_free(request *wp)
{
	struct stat sbuf;
	if(wp->destfile != NULL){
		//printf("%s:will close file\n",__FUNCTION__);
			fclose(wp->destfile);
			wp->destfile=NULL;
			sync();
			system(DROP_CACHES);
	}
	if (wp->destpath){
		//printf("%s:free wp->destpath=%s, errorn=%d\n", __FUNCTION__, wp->destpath, errno);
		if(wp->req_timeout_count > 1){
			if(stat(wp->destpath, &sbuf) == 0){
				//printf("%s:Remove the file %s\n", __FUNCTION__,wp->destpath); 
				//perror("webs_t_free");
				unlink(wp->destpath);
				usleep(60000);
			}
		}
		free(wp->destpath);
		wp->destpath=NULL;
	}
}
