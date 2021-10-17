#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h> // memcpy
#include "hs2.h"

unsigned short convert_atob_sh(char *data, int base)
{
    char tmpbuf[10];
    unsigned short rbin;
    int bin;

    memcpy(tmpbuf, data, 4);
    tmpbuf[4]='\0';
    if (base == 16)
        sscanf(tmpbuf, "%04x", &bin);
    else
        sscanf(tmpbuf, "%04d", &bin);
    
    rbin = (((bin & 0xff) << 8) | ((bin & 0xff00) >>8));
    return rbin;
}

unsigned char convert_atob(char *data, int base)
{
    char tmpbuf[10];
    int bin;

    memcpy(tmpbuf, data, 2);
    tmpbuf[2]='\0';
    if (base == 16)
        sscanf(tmpbuf, "%02x", &bin);
    else
        sscanf(tmpbuf, "%02d", &bin);
    return((unsigned char)bin);
}

int isFileExist(char *file_name)
{
    struct stat status;

    if ( stat(file_name, &status) < 0)
        return 0;

    return 1;
}

char *get_token(char *data, char *token)
{
    char *ptr=data, *ptrbak=data;
    int len=0, idx=0;
	
	while ((*ptr == ' ')||(*ptr == '	'))
	{
		ptr++;
	}
	
	data = ptr;
    
	while (*ptr && *ptr != '\n' ) {
        if (*ptr == '=') {
            if (len <= 1)
			{
				data = ptrbak;
                return NULL;
			}
            memcpy(token, data, len);

            /* delete ending space */
            for (idx=len-1; idx>=0; idx--) {
                if (token[idx] !=  ' ')
                    break;
            }
            token[idx+1] = '\0';

			data = ptrbak;
            return ptr+1;
        }

		len++;
		ptr++;
    }

	data = ptrbak;
    return NULL;
}

int get_value(char *data, char *value)
{
    char *ptr=data;
    int len=0, idx, i;

    while (*ptr && *ptr != '\n' && *ptr != '\r') {
        len++;
        ptr++;
    }

    /* delete leading space */
    idx = 0;
    while (len-idx > 0) {
        if (data[idx] != ' ')
            break;
        idx++;
    }
    len -= idx;

    /* delete bracing '"' */
    if (data[idx] == '"') {
        for (i=idx+len-1; i>idx; i--) {
            if (data[i] == '"') {
                idx++;
                len = i - idx;
            }
            break;
        }
    }

    if (len > 0) {
        memcpy(value, &data[idx], len);
        value[len] = '\0';
    }
    return len;
}

// input: iconName, config
// output: iconLen,statusCode, iconType
// return: data
void * getIconData(unsigned char *iconName, struct hs2_config *config, unsigned short *iconLen, unsigned char *statusCode, unsigned char *iconType)
{
	FILE *fp;	
	unsigned char filename[200];
	unsigned short fz;
	unsigned char *data;
	size_t result;

	struct hs2_OSUProvider * OSUPdr_ptr;
	struct hs2_IconMetadata *iconMdata_ptr;		
	*statusCode = 1;
	//printf("test0,config=%x,OSUPdr=%x\n",config,config->OSUProviderList);
	OSUPdr_ptr = config->OSUProviderList;
	while(OSUPdr_ptr != NULL) {
		//printf("test1,OSUPdr_ptr->metadata_List=%x\n",OSUPdr_ptr->metadata_List);
		iconMdata_ptr = OSUPdr_ptr->metadata_List;
		while(iconMdata_ptr != NULL) {	
			DEBUG_INFO(2, "IconName=%s,Filename=%s,iconType=%s\n",iconName,iconMdata_ptr->FileName,iconMdata_ptr->IconType);
			if(!strcmp(iconMdata_ptr->FileName,iconName)) {
				DEBUG_INFO(2, "match\n");
				*statusCode = 0;
				strcpy(iconType,iconMdata_ptr->IconType);
				DEBUG_INFO(2, "iconType=%s\n",iconMdata_ptr->IconType);
				break;
			}
			//printf("iconMdata_ptr=%x,iconMdata_ptr->next=%x",iconMdata_ptr,iconMdata_ptr->next);
			iconMdata_ptr = iconMdata_ptr->next;
		}
		if(*statusCode == 0)
			break;
		//printf("OSU_URI=%s\n",OSUPdr_ptr->OSU_URI);
		OSUPdr_ptr = OSUPdr_ptr->next;
	}
	//printf("statusCode=%d\n",*statusCode);
	if(*statusCode != 0) {
		*iconLen = 0;
		iconType[0]='\0';
		return NULL;
	}
	
	sprintf(filename,"/etc/%s",iconName);
	fp = fopen(filename, "r");
	if (fp == NULL) {
		DEBUG_ERR("read icon file [%s] failed!\n", filename);
		*statusCode = 2; // Unspecified file error
		*iconLen = 0;
		iconType[0]='\0';
		return NULL;
	} else {
		fseek(fp,0,SEEK_END);
		fz = ftell(fp);
		rewind(fp);
		data = malloc(sizeof(unsigned char)*fz);
		if (data == NULL) {
			fputs ("Memory error",stderr); 
			*statusCode = 2; // Unspecified file error
			*iconLen = 0;		
		} else {
			
			result = fread (data,1,fz ,fp);
			
			if (result != fz) {
				fputs ("Reading error",stderr);
				*statusCode = 2; // Unspecified file error
				iconType[0]='\0';
				*iconLen = 0;
				free(data);
			} else {
				*iconLen = result;
			}
		}		
		fclose(fp);
		return data;

		
	}
}