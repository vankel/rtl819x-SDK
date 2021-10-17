#ifdef HTTP_FILE_SERVER_HTM_UI
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "apmib.h"
#include "boa.h"
#include "asp_page.h"
#include "apform.h"
#include "utility.h"
#include "http_files.h"

#define DUMPLIST_BUFFER_SIZE 1024
int get_current_dir(char**pCurrent_dir)
{
	if(!httpfile_dirpath || strncmp(httpfile_dirpath,"/var/tmp/usb",strlen("/var/tmp/usb"))!=0)
		return -1;
	*pCurrent_dir = ((char*)httpfile_dirpath)+strlen("/var/tmp/usb");
	return 0;
}
int dump_httpFileDir_init(request *wp, int argc, char **argv)
{
	char MagicKey[32]={0};
	char * current_dir=NULL;

	if(get_current_dir(&current_dir)<0)
		return -1;
	GetMagicKey(MagicKey);
	//printf("%s:%d upload_st=%s,wp->query_string=%s,wp->UserBrowser=%s\n",
	//__FUNCTION__,__LINE__,upload_st,wp->query_string,wp->UserBrowser);
	if(upload_st[0])
	{ //file upload status
		req_format_write(wp, "\n\
		function init()\n\
		{\n\
			document.title = \"Index of %s\";\n\
			Magic_Key=\"%s\";\n\
			REDIR_URL=\"%s\";\n\
			POST_URL=\"/boafrm/formusbdisk_uploadfile?dest_dir=%%2F%s\";\n\
			var upload_result=\"%s\";\n\
			document.getElementById(\"current_directory\").innerHTML='%s';\n\
			show_div(0, \"progress_div\");\n\
			BrowserDetect.init();\n\
			if(upload_result == \"\"){}\n\
			else{alert(upload_result);}\n\
		}\n",current_dir, MagicKey, current_dir ,current_dir, upload_st,current_dir);  
		bzero(upload_st,sizeof(upload_st));
	}else{
		req_format_write(wp, "\n\
		function init()\n\
		{ \n\
			document.title = \"Index of %s\";\n\
			Magic_Key=\"%s\";\n\
			REDIR_URL=\"%s\";\n\
			POST_URL=\"/boafrm/formusbdisk_uploadfile?dest_dir=%%2F%s\";\n\
			var upload_result=\"\";\n\
			document.getElementById(\"current_directory\").innerHTML='%s';\n\
			show_div(0, \"progress_div\");\n\
			BrowserDetect.init();\n\
			if(upload_result == \"\"){}\n\
			else{alert(upload_result);}\n\
		}\n",current_dir, MagicKey, current_dir, current_dir,current_dir);  
	}
	return 0;
}
int dump_ListHead(request *wp, int argc, char **argv)
{
	char buffer[DUMPLIST_BUFFER_SIZE]={0};
	char name_order='D',modify_order='D',size_order='D';
	char * current_dir=NULL;

	switch(httpfile_type)
	{
		case 'N':
			name_order = (httpfile_order=='A'?'D':'A');
			break;
		case 'M':
			modify_order=(httpfile_order=='A'?'D':'A');
			break;
		case 'S':
			size_order = (httpfile_order=='A'?'D':'A');
			break;
		default:
			httpfile_type = 'N';
			httpfile_order = 'D';
			break;
	}
	if(get_current_dir(&current_dir)<0)
		return -1;
	req_format_write(wp, "<tr><td width=55%%><a id=\"rowa0\" href=\"%s?C=N;O=%c\">Name</a></td>\n",current_dir, name_order);
	req_format_write(wp, "<td width=25%%><a id=\"rowa1\" href=\"%s?C=M;O=%c\">Last modified</a></td>\n",current_dir,  modify_order);
	req_format_write(wp, "<td width=10%%><a id=\"rowa2\" href=\"%s?C=S;O=%c\">Size</td><td width=10%%></td></tr>\n", current_dir,  size_order);
	return 0;
}

int dump_parentDir(request *wp)
{
	char parentPath[PATH_MAX_LEN]={0};
	char *current_dir=NULL;
	if(get_current_dir(&current_dir)<0)
		return -1;;
	//printf("%s:%d current_dir=%s\n",__FUNCTION__,__LINE__,current_dir);
	getParentDirectory(parentPath,current_dir);
	
	//printf("%s:%d parentPath=%s\n",__FUNCTION__,__LINE__,parentPath);
	req_format_write(wp,"\
	<tr>\
		<td>\
		<img src=\"/graphics/back.gif\" border=0 width=16 height=16>\
		<a id=\"rowa10\" href=\"%s\">Parent Directory </a>\
		</td>\
		<td></td><td></td><td></td>\n\
	</tr>",parentPath);
	return 0;
}
int dumpDirectList(request *wp, int argc, char **argv)
{
	int docLen=0;
	int i=0,j=0;
	char sort_type=0,sort_order=0;
	PATH_ENTRY_Tp FILE_sp;
	PATH_ENTRY_Tp DIR_sp;
	char fileLastModified[FILE_INFO_MAX_LEN];
	char fileSize[FILE_INFO_MAX_LEN];
	char buffer[DUMPLIST_BUFFER_SIZE]={0};
	char * current_dir=NULL;
	if(!httpfile_dirpath)
		return -1;
	dump_parentDir(wp);
	//printf("%s:%d path=%s type=%c order=%c\n",__FUNCTION__,__LINE__,httpfile_dirpath,httpfile_type,httpfile_order);
	if(!httpfile_type ||!httpfile_order)
	{
		sort_type= 'N';
		sort_order='D';
	}else
	{
		sort_type=httpfile_type;
		sort_order=httpfile_order;
	}
	
	//printf("%s:%d path=%s type=%c order=%c\n",__FUNCTION__,__LINE__,httpfile_dirpath,sort_type,sort_order);
	create_directory_list(httpfile_dirpath,sort_type,sort_order);
	get_current_dir(&current_dir);
	sprintf(fileLastModified, "%s", "-");
	sprintf(fileSize, "%s", "-");

	for(i=0;i<CurrDIRCount;i++){
			DIR_sp = &DirEntryHead[i];
			if(DIR_sp->name[0]){
				get_LastModified_Size(fileLastModified, fileSize, &DIR_sp->mtime, DIR_sp->size, 1);
				bzero(buffer,sizeof(buffer));
				if(DIR_sp->mtime==0 &&  DIR_sp->size==0){
					sprintf(buffer, "<tr style=\"display:none\"><td><img src=\"/graphics/dir.gif\" border=0 width=16 height=16><a id=\"row%d\" href=\"%s%s/\" onclick=\"return chk_CurrentDIRuri(this);\">%s/</a></td><td>%s</td><td>%s</td><td><input id=\"RemoveDIR\" onclick=\"sendClickedRemove('%s')\" type=button name=\"RemoveDIR\" value=\"Remove\" /></td></tr>\n",i+1,current_dir,DIR_sp->name,DIR_sp->name,fileLastModified, "-",DIR_sp->name);
				}else{
					sprintf(buffer, "<tr><td><img src=\"/graphics/dir.gif\" border=0 width=16 height=16><a id=\"row%d\" href=\"%s%s/\" onclick=\"return chk_CurrentDIRuri(this);\">%s/</a></td><td>%s</td><td>%s</td><td><input id=\"RemoveDIR\" onclick=\"sendClickedRemove('%s')\" type=button name=\"RemoveDIR\" value=\"Remove\" /></td></tr>\n",i+1,current_dir,DIR_sp->name,DIR_sp->name,fileLastModified, "-",DIR_sp->name);
				}
				//str = strcat_str(str, &len, &tmplen, buffer);
				req_format_write(wp,buffer);
			}
		}
		
		j=i;
		for(i=0;i<CurrFILECount;i++){
			FILE_sp = &FileEntryHead[i];
			if(FILE_sp->name[0]){
			get_LastModified_Size(fileLastModified, fileSize, &FILE_sp->mtime, FILE_sp->size, 0);
			bzero(buffer,sizeof(buffer));
			if(FILE_sp->mtime==0 &&  FILE_sp->size==0){
				sprintf(buffer, "<tr style=\"display:none\"><td><img src=\"/graphics/text.gif\" border=0 width=16 height=16><a id=\"row%d\" href=\"%s%s\" onclick=\"return chk_CurrentDIRuri(this);\">%s</a></td><td>%s</td><td>%s</td><td><input id=\"RemoveFile\" onclick=\"sendClickedRemove('%s')\" type=button name=\"RemoveFile\" value=\"Remove\"></td></tr>\n",j+i+1,current_dir, FILE_sp->name,FILE_sp->name, fileLastModified,fileSize,FILE_sp->name);
			}else{
				sprintf(buffer, "<tr><td><img src=\"/graphics/text.gif\" border=0 width=16 height=16><a id=\"row%d\" href=\"%s%s\" onclick=\"return chk_CurrentDIRuri(this);\">%s</a></td><td>%s</td><td>%s</td><td><input id=\"RemoveFile\" onclick=\"sendClickedRemove('%s')\" type=button name=\"RemoveFile\" value=\"Remove\"></td></tr>\n",j+i+1,current_dir, FILE_sp->name,FILE_sp->name, fileLastModified,fileSize,FILE_sp->name);
			}
			req_format_write(wp,buffer);
	      	//str = strcat_str(str, &len, &tmplen, buffer);
	      }
		}
		//str[len] = '\0';
		//*docLength = len;
		free_directory_list();
	return 0;
}
int dump_uploadDiv(request *wp, int argc, char **argv)
{	
	char buffer[DUMPLIST_BUFFER_SIZE]={0};
	if(strstr(wp->UserBrowser, "MSIE"))
	{
		//printf("**************IE upload!!!!!!!!!!!!!!!!!!!!!!!!!\n");
		sprintf(buffer,"<div>\
<input id=\"uploadedfile\" type=\"file\" size=\"35\" name=\"uploadedfile\">&nbsp;&nbsp;\n\
<input type=\"button\" onclick=\"sendClicked()\" id=\"Upload\" value=\"Upload\" />\n\
</div>&nbsp;&nbsp;<div id=\"progressNumber\" style=\"display:none\"></div>\n\
<div id=\"progress_div\" style=\"display:none;border: 1px solid black; width:300px; height:15px;\">\n\
<div id=\"progress_bar\" style=\"height:15px; width:0px; background-color:blue;\"></div></div></form>\n");
	}else
	{	
		sprintf(buffer,"<div>\
<label for=\"fileToUpload\">\
Select a File to Upload:</label>\n\
&nbsp;&nbsp;<input type=\"file\" name=\"fileToUpload[]\" id=\"fileToUpload\" onchange=\"fileSelected();\" />\n\
&nbsp;&nbsp;<input type=\"button\" onclick=\"sendClicked()\" name=\"Upload\" id=\"Upload\" value=\"Upload\" /></div>\n<div id=\"fileName\" style=\"display:none\"></div><div id=\"fileSize\" style=\"display:none\"></div><div id=\"fileType\" style=\"display:none\"></div>\n\
&nbsp;&nbsp;<div id=\"progressNumber\" style=\"display:none\"></div>\n<div id=\"progress_div\" style=\"display:none;border: 1px solid black; width:200px; height:15px;\"><div id=\"progress_bar\" style=\"height:15px; width:0px; background-color:blue;\"/></div></div>\
</form>\n");
	}
	req_format_write(wp,buffer);
}

#endif
