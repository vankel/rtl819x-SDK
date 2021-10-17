/*CGI API*/

#include "rtk_api.h"
#include "../../mxml-2.8/mxml.h"
#include "rtk_cgi_adapter.h"

int rtk_nas_login(char * username,char * password)
{
	char cmdBuf[256]={0};
	char outBuf[512]={0};
	FILE *fp=NULL;
	sprintf(cmdBuf,"cgiClient -a login -u %s -p %s 1>%s 2>/dev/null",username,password,TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	fgets(outBuf, 511, fp);
	if(!outBuf[0] || strstr(outBuf,"Wrong Username or Password") || strstr(outBuf,"error"))
	{
		fclose(fp);
		return RTK_FAILED;		
	}
	fclose(fp);
	return RTK_SUCCESS;
}

int rtk_nas_logout()
{
	char cmdBuf[256]={0};
	char outBuf[512]={0};
	FILE *fp=NULL;
	sprintf(cmdBuf,"cgiClient -a logout 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	fgets(outBuf, 511, fp);
	
	if(!outBuf[0] || strstr(outBuf,"error"))
	{
		fclose(fp);
		return RTK_FAILED;	
	}
	
	fclose(fp);
	return RTK_SUCCESS;
}

/*******************************************************
* get android version, max length RTK_VERSION_LEN
*****************************************************/
int rtk_get_nas_api_info(RTK_NAS_API_INFO_Tp nas_info)
{
	char cmdBuf[256]={0};
	char outBuf[512]={0};
	//json_value* pjsonValue={0};
	FILE *fp=NULL;
	sprintf(cmdBuf,"cgiClient -a json -d '{\"request\":{\"action\":\"get\"}}' -r \"api/api/info\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	fgets(outBuf, 511, fp);
	/*pjsonValue=json_parse(outBuf,strlen(outBuf));
	if(!pjsonValue)
	{
		fclose(fp);
		return RTK_FAILED;
	}*/

	fclose(fp);
	return RTK_SUCCESS;
}

int rtk_get_nas_info()
{
	char cmdBuf[256]={0};
	//char outBuf[2048]={0};
	//json_value* pjsonValue={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"nas/get/info\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 2047, fp);
	
//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "kernel", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  fclose(fp);
		  mxmlDelete(tree);
		  return RTK_FAILED;
		}
//	printf(node->child->value.opaque);
	fclose(fp);
	mxmlDelete(tree);

	return RTK_SUCCESS;
}

int rtk_get_cgi_disk_formatable(int *formatable)
{
	char cmdBuf[256]={0};
	//char outBuf[1024]={0};
	char status[8]={0};
	//json_value* pjsonValue={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m post  -r \"nas/get/mkfsdsk\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 2047, fp);
	
//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "status", NULL, NULL,
										MXML_DESCEND)) == NULL)
			{
			  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
			  goto FORMATABLE_FAIL;
			}
		//printf(node->child->value.opaque);
		strcpy(status,node->child->value.opaque);
		if(status[0]!='0' && status[0]!='1')
		{
			fprintf(stderr,"get invalid status!\n");
			goto FORMATABLE_FAIL;
		}
		*formatable=atoi(status);
		fclose(fp);
		mxmlDelete(tree);
		return RTK_SUCCESS;
	FORMATABLE_FAIL:
		fclose(fp);
		mxmlDelete(tree);
		return RTK_FAILED;

}

int rtk_cgi_disk_format()
{
	char cmdBuf[256]={0};
	//char outBuf[1024]={0};
	char status[8]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m post  -r \"nas/storage/allinone\" -d \"level=1&fs=ext4\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
//	fgets(outBuf, 1023,fp);
//	printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "fs", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto FORMAT_FAIL;
		}
	//printf(node->child->value.opaque);
	strcpy(status,node->child->value.opaque);
	if(strcmp(status,"ext4")!=0)
	{
		fprintf(stderr,"get invalid status!\n");
		goto FORMAT_FAIL;
	}

	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
FORMAT_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;
}

int rtk_check_upgradable(int *upgradable)
{
	char cmdBuf[256]={0};
//	char outBuf[1024]={0};
	char status[8]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"letvcgi\"  -d \"id=8&cmd=is_upgradable\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
//	printf(cmdBuf);
	system(cmdBuf);
//	sleep(1);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");

	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "status", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto UPGRADABLE_FAIL;
		}
	//printf(node->child->value.opaque);
	strcpy(status,node->child->value.opaque);
	if(status[0]!='0' && status[0]!='1')
	{
		fprintf(stderr,"get invalid status!\n");
		goto UPGRADABLE_FAIL;
	}
	*upgradable=atoi(status);
	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
UPGRADABLE_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;
}

int rtk_upgrade()
{
	char cmdBuf[256]={0};
	//char outBuf[1024]={0};
	char status[8]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"letvcgi\"  -d \"id=8&cmd=upgrade\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 1023,fp);
	//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "status", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto UPGRADE_FAIL;
		}
	//printf(node->child->value.opaque);
	strcpy(status,node->child->value.opaque);
	if(status[0]!='1')
	{
		fprintf(stderr,"get invalid status %s!\n",status);
		goto UPGRADE_FAIL;
	}
	
	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
UPGRADE_FAIL:
		fclose(fp);
		mxmlDelete(tree);
		return RTK_FAILED;


}

int rtk_check_upgrade_result(int *upgrad_result)
{
	char cmdBuf[256]={0};
	//char outBuf[1024]={0};
	char status[8]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"letvcgi\"  -d \"id=8&cmd=upgrade_result\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 1023,fp);
	//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "status", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto CHECK_UPGRADE_RESULT_FAIL;
		}
//	printf(node->child->value.opaque);
	strcpy(status,node->child->value.opaque);
	if(status[0]!='0' && status[0]!='1')
	{
		fprintf(stderr,"get invalid status!\n");
		goto CHECK_UPGRADE_RESULT_FAIL;
	}
	*upgrad_result=atoi(status);
	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
CHECK_UPGRADE_RESULT_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;
}

int rtk_send_le_id_pass(char * username,char * password)
{
	char cmdBuf[256]={0};
	//char outBuf[1024]={0};
	char status[8]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"letvcgi\"  -d \"id=8&cmd=id_pass&userid=%s&pass=%s\" 1>%s 2>/dev/null",username,password,TMEP_CGI_OUT_FILE);
	system(cmdBuf);	

	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 1023,fp);
	//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "status", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto SEND_LE_ID_PASS_FAIL;
		}
	//printf(node->child->value.opaque);
	strcpy(status,node->child->value.opaque);
	if(status[0]!='1')
	{
		fprintf(stderr,"get invalid status! status=%s\n",status);
		goto SEND_LE_ID_PASS_FAIL;
	}

	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
SEND_LE_ID_PASS_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;

}

int rtk_logout_le_id_pass(char * username)
{
	char cmdBuf[256]={0};
	//char outBuf[1024]={0};
	char status[8]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;

	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"letvcgi\"  -d \"id=8&cmd=id_delete&id=%s\" 1>%s 2>/dev/null",username,TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 1023,fp);
	//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "status", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto LOGOUT_LE_ID_PASS_FAIL;
		}
	//printf(node->child->value.opaque);
	strcpy(status,node->child->value.opaque);
	if(status[0]!='1')
	{
		fprintf(stderr,"get invalid status! status=%s\n",status);
		goto LOGOUT_LE_ID_PASS_FAIL;
	}

	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
LOGOUT_LE_ID_PASS_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;
}

int rtk_upload_log()
{
	char cmdBuf[256]={0};
	//char outBuf[1024]={0};
	char status[8]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"letvcgi\"  -d \"id=8&cmd=upload_log\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 1023,fp);
	//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "status", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto UPLOAD_LOG_FAIL;
		}
	//printf(node->child->value.opaque);
	strcpy(status,node->child->value.opaque);
	if(status[0]!='1')
	{
		fprintf(stderr,"get invalid status! status=%s\n",status);
		goto UPLOAD_LOG_FAIL;
	}

	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
UPLOAD_LOG_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;
}


int rtk_query_software_version(char *version)
{
	char cmdBuf[256]={0};
	//char outBuf[1024]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"letvcgi\"  -d \"id=8&cmd=query_software_version\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 1023,fp);
	//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "android_version", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto CHECK_UPGRADE_RESULT_FAIL;
		}
	//printf(node->child->value.opaque);
	strcpy(version,node->child->value.opaque);	

	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
CHECK_UPGRADE_RESULT_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;
}
int rtk_is_wizard_done(int *is_wizard_done)
{
	char cmdBuf[256]={0};
//	char outBuf[1024]={0};
	char status[8]={0};
	FILE *fp=NULL;
	mxml_node_t *tree;
	mxml_node_t *node;
	sprintf(cmdBuf,"cgiClient -a command -m get  -r \"letvcgi\"  -d \"id=8&cmd=is_wizard_done\" 1>%s 2>/dev/null",TMEP_CGI_OUT_FILE);
	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	//fgets(outBuf, 1023,fp);
	//printf(outBuf);
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	
	if ((node = mxmlFindElement(tree, tree, "status", NULL, NULL,
									MXML_DESCEND)) == NULL)
		{
		  fprintf(stderr,"Unable to find first <choice> element in XML tree!\n", stderr);
		  goto CHECK_UPGRADE_RESULT_FAIL;
		}
	//printf(node->child->value.opaque);
	strcpy(status,node->child->value.opaque);
	if(status[0]!='0' && status[0]!='1')
	{
		fprintf(stderr,"get invalid status!\n");
		goto CHECK_UPGRADE_RESULT_FAIL;
	}
	*is_wizard_done=atoi(status);
	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;
CHECK_UPGRADE_RESULT_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;
}

int rtk_common_cgi_api(char * httpMethod,char *url,char*httpData,int parmNum,char *nameArray[],char*outPutArray[])
{
	char cmdBuf[256]={0};
//	char outBuf[1024]={0};
	char status[8]={0};
	int i=0;
	FILE *fp=NULL;
	mxml_node_t *tree=NULL;
	mxml_node_t *node=NULL;
	if(strcmp(httpMethod,"get")!=0 && strcmp(httpMethod,"post")!=0)
	{
		fprintf(stderr,"http method must be post/get\n");
		return RTK_FAILED;
	}
	if(parmNum==0)
	{
		fprintf(stderr,"Must at least return one value!\n");
		return RTK_FAILED;
	}
	if(!nameArray||!outPutArray)
	{
		fprintf(stderr,"invalid input!\n");
		return RTK_FAILED;
	}
	for(i=0;i<parmNum;i++)
	{
		if(!nameArray[i]||!outPutArray[i])
		{
			fprintf(stderr,"invalid input!\n");
			return RTK_FAILED;
		}
	}
	if(httpData[0])
		sprintf(cmdBuf,"cgiClient -a command -m %s  -r \"%s\"  -d \"%s\" 1>%s 2>/dev/null",httpMethod,url,httpData,TMEP_CGI_OUT_FILE);
	else
		sprintf(cmdBuf,"cgiClient -a command -m %s  -r \"%s\"  1>%s 2>/dev/null",httpMethod,url,TMEP_CGI_OUT_FILE);

	system(cmdBuf);
	fp=fopen(TMEP_CGI_OUT_FILE,"r");
	tree = mxmlLoadFile(NULL, fp,MXML_OPAQUE_CALLBACK);
	node=tree;
	for(i=0;i<parmNum;i++)
	{
		while(node!=NULL)
		{
			node= mxmlWalkNext(node, tree,MXML_DESCEND);
			if(node==NULL) break;
			if(strcmp(node->value.element.name,nameArray[i])==0)
			{
				strcpy(outPutArray[i],node->child->value.opaque);
				break;
			}
		}
	}
	fclose(fp);
	mxmlDelete(tree);
	return RTK_SUCCESS;

RTK_COMMON_CGI_FAIL:
	fclose(fp);
	mxmlDelete(tree);
	return RTK_FAILED;

}

