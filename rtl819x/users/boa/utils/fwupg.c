#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "../src/boa.h"
#include "../src/apform.h"
#include "../src/utility.h"
#include "apmib.h"

#define CHECK_OK_STRING "Firmware upgrade check OK!\n"

extern int Decode(unsigned char *ucInput, unsigned int inLen, unsigned char *ucOutput);

#if 0
// in utility.c
static int write_line_to_file(char *filename, int mode, char *line_data)
{
	unsigned char tmpbuf[512];
	int fh=0;

	if(mode == 1) {/* write line datato file */
		
		fh = open(filename, O_RDWR|O_CREAT|O_TRUNC);
		
	}else if(mode == 2){/*append line data to file*/
		
		fh = open(filename, O_RDWR|O_APPEND);	
	}
	
	
	if (fh < 0) {
		fprintf(stderr, "Create %s error!\n", filename);
		return 0;
	}


	sprintf((char *)tmpbuf, "%s", line_data);
	write(fh, tmpbuf, strlen((char *)tmpbuf));



	close(fh);
	return 1;
}

static void kill_processes(void)
{


	printf("upgrade: killing tasks...\n");
	
	kill(1, SIGTSTP);		/* Stop init from reforking tasks */
	kill(1, SIGSTOP);		
	kill(2, SIGSTOP);		
	kill(3, SIGSTOP);		
	kill(4, SIGSTOP);		
	kill(5, SIGSTOP);		
	kill(6, SIGSTOP);		
	kill(7, SIGSTOP);		
	//atexit(restartinit);		/* If exit prematurely, restart init */
	sync();

	signal(SIGTERM,SIG_IGN);	/* Don't kill ourselves... */
	setpgrp(); 			/* Don't let our parent kill us */
	sleep(1);
	signal(SIGHUP, SIG_IGN);	/* Don't die if our parent dies due to
					 * a closed controlling terminal */
	
}

static int fwChecksumOk(char *data, int len)
{
	unsigned short sum=0;
	int i;

	for (i=0; i<len; i+=2) {
#ifdef _LITTLE_ENDIAN_
		sum += WORD_SWAP( *((unsigned short *)&data[i]) );
#else
		sum += *((unsigned short *)&data[i]);
#endif

	}
	return( (sum==0) ? 1 : 0);
}
#endif

#ifdef HOME_GATEWAY
/////////////////////////////////////////////////////////////////////////////
int isConnectPPP()
{
	struct stat status;
#ifdef MULTI_PPPOE
	if(PPPoE_Number == 1)
	{
		if ( stat("/etc/ppp/link", &status) < 0)
			return 0;
	}
	else if(PPPoE_Number == 2)
	{
		if ( stat("/etc/ppp/link2", &status) < 0)
			return 0;
	}
	else if(PPPoE_Number ==3)
	{
		if ( stat("/etc/ppp/link3", &status) < 0)
			return 0;	
	}
	else if(PPPoE_Number ==4)
	{
		if ( stat("/etc/ppp/link4", &status) < 0)
			return 0;
	}
	else
	{
		if ( stat("/etc/ppp/link", &status) < 0)
			return 0;		
	}
#else
	if ( stat("/etc/ppp/link", &status) < 0)
		return 0;
#endif

	return 1;
}
#endif

static int get_actvie_bank()
{
	FILE *fp;
	char buffer[2];
	int bootbank;
	fp = fopen("/proc/bootbank", "r");
	
	if (!fp) {
		fprintf(stderr,"%s\n","Read /proc/bootbank failed!\n");
	}else
	{
			//fgets(bootbank, sizeof(bootbank), fp);
			fgets(buffer, sizeof(buffer), fp);
			fclose(fp);
	}
	bootbank = buffer[0] - 0x30;	
	if ( bootbank ==1 || bootbank ==2)
		return bootbank;
	else
		return 1;	
}

void get_bank_info(int dual_enable,int *active,int *backup)
{
	int bootbank=0,backup_bank;
	
	bootbank = get_actvie_bank();	

	if(bootbank == 1 )
	{
		if( dual_enable ==0 )
			backup_bank =1;
		else
			backup_bank =2;
	}
	else if(bootbank == 2 )
	{
		if( dual_enable ==0 )
			backup_bank =2;
		else
			backup_bank =1;
	}
	else
	{
		bootbank =1 ;
		backup_bank =1 ;
	}	

	*active = bootbank;
	*backup = backup_bank;	

	//fprintf(stderr,"get_bank_info active_bank =%d , backup_bank=%d  \n",*active,*backup); //mark_debug	   
}

static int updateConfigIntoFlash(unsigned char *data, int total_len, int *pType, int *pStatus)
{
	int len=0, status=1, type=0, ver, force;
	PARAM_HEADER_Tp pHeader;
#ifdef COMPRESS_MIB_SETTING
	COMPRESS_MIB_HEADER_Tp pCompHeader;
	unsigned char *expFile=NULL;
	unsigned int expandLen=0;
	int complen=0;
#endif
	char *ptr;
	unsigned char isValidfw = 0;

	do {
		if (
#ifdef COMPRESS_MIB_SETTING
			memcmp(&data[complen], COMP_HS_SIGNATURE, COMP_SIGNATURE_LEN) &&
			memcmp(&data[complen], COMP_DS_SIGNATURE, COMP_SIGNATURE_LEN) &&
			memcmp(&data[complen], COMP_CS_SIGNATURE, COMP_SIGNATURE_LEN)
#else
			memcmp(&data[len], CURRENT_SETTING_HEADER_TAG, TAG_LEN) &&
			memcmp(&data[len], CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN) &&
			memcmp(&data[len], CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) &&
			memcmp(&data[len], DEFAULT_SETTING_HEADER_TAG, TAG_LEN) &&
			memcmp(&data[len], DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) &&
			memcmp(&data[len], DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) &&
			memcmp(&data[len], HW_SETTING_HEADER_TAG, TAG_LEN) &&
			memcmp(&data[len], HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) &&
			memcmp(&data[len], HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) 
#endif
		) {
			if (isValidfw == 1)
				break;
		}
		
#ifdef COMPRESS_MIB_SETTING
		pCompHeader =(COMPRESS_MIB_HEADER_Tp)&data[complen];
#ifdef _LITTLE_ENDIAN_
		pCompHeader->compRate = WORD_SWAP(pCompHeader->compRate);
		pCompHeader->compLen = DWORD_SWAP(pCompHeader->compLen);
#endif
		/*decompress and get the tag*/
		expFile=malloc(pCompHeader->compLen*pCompHeader->compRate);
		if (NULL==expFile) {
			printf("malloc for expFile error!!\n");
			return 0;
		}
		expandLen = Decode(data+complen+sizeof(COMPRESS_MIB_HEADER_T), pCompHeader->compLen, expFile);
		pHeader = (PARAM_HEADER_Tp)expFile;
#else
		pHeader = (PARAM_HEADER_Tp)&data[len];
#endif
		
#ifdef _LITTLE_ENDIAN_
		pHeader->len = WORD_SWAP(pHeader->len);
#endif
		len += sizeof(PARAM_HEADER_T);

		if ( sscanf((char *)&pHeader->signature[TAG_LEN], "%02d", &ver) != 1)
			ver = -1;
			
		force = -1;
		if ( !memcmp(pHeader->signature, CURRENT_SETTING_HEADER_TAG, TAG_LEN) ) {
			isValidfw = 1;
			force = 1; // update
		}
		else if ( !memcmp(pHeader->signature, CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN)) {
			isValidfw = 1;
			force = 2; // force
		}
		else if ( !memcmp(pHeader->signature, CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN)) {
			isValidfw = 1;
			force = 0; // upgrade
		}

		if ( force >= 0 ) {
#if 0
			if ( !force && (ver < CURRENT_SETTING_VER || // version is less than current
				(pHeader->len < (sizeof(APMIB_T)+1)) ) { // length is less than current
				status = 0;
				break;
			}
#endif

#ifdef COMPRESS_MIB_SETTING
			ptr = (char *)(expFile+sizeof(PARAM_HEADER_T));
#else
			ptr = &data[len];
#endif

#ifdef COMPRESS_MIB_SETTING
#else
			DECODE_DATA(ptr, pHeader->len);
#endif
			if ( !CHECKSUM_OK((unsigned char *)ptr, pHeader->len)) {
				status = 0;
				break;
			}
#ifdef _LITTLE_ENDIAN_
			swap_mib_word_value((APMIB_Tp)ptr);
#endif

// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
		#ifndef VOIP_SUPPORT_TLV_CFG
			flash_voip_import_fix(&((APMIB_Tp)ptr)->voipCfgParam, &pMib->voipCfgParam);
#endif
#endif

#ifdef COMPRESS_MIB_SETTING
			apmib_updateFlash(CURRENT_SETTING, (char *)&data[complen], pCompHeader->compLen+sizeof(COMPRESS_MIB_HEADER_T), force, ver);
#else
			apmib_updateFlash(CURRENT_SETTING, ptr, pHeader->len-1, force, ver);
#endif

#ifdef COMPRESS_MIB_SETTING
			complen += pCompHeader->compLen+sizeof(COMPRESS_MIB_HEADER_T);
			if (expFile) {
				free(expFile);
				expFile=NULL;
			}
#else
			len += pHeader->len;
#endif
			type |= CURRENT_SETTING;
			continue;
		}


		if ( !memcmp(pHeader->signature, DEFAULT_SETTING_HEADER_TAG, TAG_LEN) ) {
			isValidfw = 1;
			force = 1;	// update
		}
		else if ( !memcmp(pHeader->signature, DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ) {
			isValidfw = 1;
			force = 2;	// force
		}
		else if ( !memcmp(pHeader->signature, DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ) {
			isValidfw = 1;
			force = 0;	// upgrade
		}

		if ( force >= 0 ) {
#if 0
			if ( (ver < DEFAULT_SETTING_VER) || // version is less than current
				(pHeader->len < (sizeof(APMIB_T)+1)) ) { // length is less than current
				status = 0;
				break;
			}
#endif

#ifdef COMPRESS_MIB_SETTING
			ptr = (char *)(expFile+sizeof(PARAM_HEADER_T));
#else
			ptr = &data[len];
#endif

#ifdef COMPRESS_MIB_SETTING
#else
			DECODE_DATA(ptr, pHeader->len);
#endif
			if ( !CHECKSUM_OK((unsigned char *)ptr, pHeader->len)) {
				status = 0;
				break;
			}

#ifdef _LITTLE_ENDIAN_
			swap_mib_word_value((APMIB_Tp)ptr);
#endif

// added by rock /////////////////////////////////////////
#ifdef VOIP_SUPPORT
		#ifndef VOIP_SUPPORT_TLV_CFG
			flash_voip_import_fix(&((APMIB_Tp)ptr)->voipCfgParam, &pMibDef->voipCfgParam);
#endif
#endif

#ifdef COMPRESS_MIB_SETTING
			apmib_updateFlash(DEFAULT_SETTING, (char *)&data[complen], pCompHeader->compLen+sizeof(COMPRESS_MIB_HEADER_T), force, ver);
#else
			apmib_updateFlash(DEFAULT_SETTING, ptr, pHeader->len-1, force, ver);
#endif

#ifdef COMPRESS_MIB_SETTING
			complen += pCompHeader->compLen+sizeof(COMPRESS_MIB_HEADER_T);
			if (expFile) {
				free(expFile);
				expFile=NULL;
			}	
#else
			len += pHeader->len;
#endif
			type |= DEFAULT_SETTING;
			continue;
		}

		if ( !memcmp(pHeader->signature, HW_SETTING_HEADER_TAG, TAG_LEN) ) {
			isValidfw = 1;
			force = 1;	// update
		}
		else if ( !memcmp(pHeader->signature, HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) ) {
			isValidfw = 1;
			force = 2;	// force
		}
		else if ( !memcmp(pHeader->signature, HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ) {
			isValidfw = 1;
			force = 0;	// upgrade
		}

		if ( force >= 0 ) {
#if 0
			if ( (ver < HW_SETTING_VER) || // version is less than current
				(pHeader->len < (sizeof(HW_SETTING_T)+1)) ) { // length is less than current
				status = 0;
				break;
			}
#endif
#ifdef COMPRESS_MIB_SETTING
			ptr = (char *)(expFile+sizeof(PARAM_HEADER_T));
#else
			ptr = &data[len];
#endif
			

#ifdef COMPRESS_MIB_SETTING
#else
			DECODE_DATA(ptr, pHeader->len);
#endif
			if ( !CHECKSUM_OK((unsigned char *)ptr, pHeader->len)) {
				status = 0;
				break;
			}
#ifdef COMPRESS_MIB_SETTING
			apmib_updateFlash(HW_SETTING, (char *)&data[complen], pCompHeader->compLen+sizeof(COMPRESS_MIB_HEADER_T), force, ver);
#else
			apmib_updateFlash(HW_SETTING, ptr, pHeader->len-1, force, ver);
#endif

#ifdef COMPRESS_MIB_SETTING
			complen += pCompHeader->compLen+sizeof(COMPRESS_MIB_HEADER_T);
			if (expFile) {
				free(expFile);
				expFile=NULL;
			}
#else
			len += pHeader->len;
#endif

			type |= HW_SETTING;
			continue;
		}
	}
#ifdef COMPRESS_MIB_SETTING	
	while (complen < total_len);

	if (expFile) {
		free(expFile);
		expFile=NULL;
	}
#else
	while (len < total_len);
#endif

	*pType = type;
	*pStatus = status;
#ifdef COMPRESS_MIB_SETTING	
	return complen;
#else
	return len;
#endif
}


int find_head_offset(char *upload_data)
{
	int head_offset=0 ;
	char *pStart=NULL;
	int iestr_offset=0;
	char *dquote;
	char *dquote1;
	
	if (upload_data==NULL) {
		//fprintf(stderr, "upload data is NULL\n");
		return -1;
	}

	pStart = strstr(upload_data, WINIE6_STR);
	if (pStart == NULL) {
		pStart = strstr(upload_data, LINUXFX36_FWSTR);
		if (pStart == NULL) {
			pStart = strstr(upload_data, MACIE5_FWSTR);
			if (pStart == NULL) {
				pStart = strstr(upload_data, OPERA_FWSTR);
				if (pStart == NULL) {
					pStart = strstr(upload_data, "filename=");
					if (pStart == NULL) {
						return -1;
					}
					else {
						dquote =  strstr(pStart, "\"");
						if (dquote !=NULL) {
							dquote1 = strstr(dquote, LINE_FWSTR);
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
				}
				else {
					iestr_offset = 16;
				}
			} 
			else {
				iestr_offset = 14;
			}
		}
		else {
			iestr_offset = 26;
		}
	}
	else {
		iestr_offset = 17;
	}
	//fprintf(stderr,"####%s:%d %d###\n",  __FILE__, __LINE__ , iestr_offset);
	head_offset = (int)(((unsigned long)pStart)-((unsigned long)upload_data)) + iestr_offset;
	return head_offset;
}


void clear_fwupload_shm(void *shm_memory, int shm_id)
{
	//shm_memory = (char *)shmat(shmid, NULL, 0);
	if(shm_memory){
		if (shmdt(shm_memory) == -1) {
      			fprintf(stderr, "shmdt failed\n");
  		}
  	}

	if (shm_id != 0) {
		if(shmctl(shm_id, IPC_RMID, 0) == -1) {
			fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		}
	}
	shm_id = 0;
	shm_memory = NULL;
}

int FirmwareUpgradePrepare(char *upload_data, int upload_len, int shm_id, char *errMsgBuf,
							int checkonly, int *update_cfg, int *update_fw)
{
	int head_offset=0 ;
	int isIncludeRoot=0;
	int		 len;
	int          locWrite;
	int          numLeft;
	int          numWrite;
	IMG_HEADER_Tp pHeader;
	int flag=0, startAddr=-1, startAddrWeb=-1;
	int fh;

	int fwSizeLimit = CONFIG_FLASH_SIZE;
#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE
	int active_bank,backup_bank;
	int dual_enable =0;
#endif
	unsigned char isValidfw = 0;

#if defined(CONFIG_APP_FWD)
#define FWD_CONF "/var/fwd.conf"
	int newfile = 1;
#endif

	*update_fw=0;
	*update_cfg=0;

#if 0
	if (isCFG_ONLY == 0) {
		/*
		#ifdef CONFIG_RTL_8196B
			sprintf(cmdBuf, "echo \"4 %d\" > /proc/gpio", (Reboot_Wait+12));
		#else	
			sprintf(cmdBuf, "echo \"4 %d\" > /proc/gpio", (Reboot_Wait+20));
		#endif
		
			system(cmdBuf);
		*/
		system("ifconfig br0 down 2> /dev/null");
	}
#endif

	/* find bank */
#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE
	apmib_get(MIB_DUALBANK_ENABLED,(void *)&dual_enable);   
	get_bank_info(dual_enable,&active_bank,&backup_bank);        
#endif

#if 0 
	TODO Babylon ???
	/* find head */
	head_offset = find_head_offset(upload_data);
	//fprintf(stderr,"####%s:%d %d upload_data=%p###\n",  __FILE__, __LINE__ , head_offset, upload_data);
	if (head_offset == -1) {
		strcpy(errMsgBuf, "Invalid file format!");
		goto ret_upload;
	}
#endif

	while ((head_offset+sizeof(IMG_HEADER_T)) < upload_len) {
		locWrite = 0;
		pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
		len = pHeader->len;
#ifdef _LITTLE_ENDIAN_
		len  = DWORD_SWAP(len);
#endif
		numLeft = len + sizeof(IMG_HEADER_T) ;
		// check header and checksum
		if (!memcmp(&upload_data[head_offset], FW_HEADER, SIGNATURE_LEN) ||
			!memcmp(&upload_data[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN)) {
			isValidfw = 1;
			flag = 1;
		}
		else if (!memcmp(&upload_data[head_offset], WEB_HEADER, SIGNATURE_LEN)) {
			isValidfw = 1;
			flag = 2;
		}
		else if (!memcmp(&upload_data[head_offset], ROOT_HEADER, SIGNATURE_LEN)) {
			isValidfw = 1;
			flag = 3;
			isIncludeRoot = 1;
		}else if (
#ifdef COMPRESS_MIB_SETTING
				!memcmp(&upload_data[head_offset], COMP_HS_SIGNATURE, COMP_SIGNATURE_LEN) ||
				!memcmp(&upload_data[head_offset], COMP_DS_SIGNATURE, COMP_SIGNATURE_LEN) ||
				!memcmp(&upload_data[head_offset], COMP_CS_SIGNATURE, COMP_SIGNATURE_LEN)
#else
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], CURRENT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], DEFAULT_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_FORCE_TAG, TAG_LEN) ||
				!memcmp(&upload_data[head_offset], HW_SETTING_HEADER_UPGRADE_TAG, TAG_LEN) 
#endif			
				) {				
			int configlen = 0;
			int type=0, status=0, cfg_len;

#ifdef COMPRESS_MIB_SETTING
			COMPRESS_MIB_HEADER_Tp pHeader_cfg;
			pHeader_cfg = (COMPRESS_MIB_HEADER_Tp)&upload_data[head_offset];
			if(!memcmp(&upload_data[head_offset], COMP_CS_SIGNATURE, COMP_SIGNATURE_LEN)) {
				configlen +=  pHeader_cfg->compLen+sizeof(COMPRESS_MIB_HEADER_T);
			}
#endif

			cfg_len = updateConfigIntoFlash((unsigned char *)&upload_data[head_offset],configlen , &type, &status);

			if (status == 0 || type == 0) { // checksum error
				strcpy(errMsgBuf, "Invalid configuration file!");
				goto ret_upload;
			}
			else { // upload success
				strcpy(errMsgBuf, "Update successfully!");
				head_offset += cfg_len;
				isValidfw = 1;
				*update_cfg = 1;
			}
			continue;
		}
		else {
			if (isValidfw == 1)
				break;
			strcpy(errMsgBuf, ("Invalid file format!"));
			goto ret_upload;
		}

		if (len > fwSizeLimit) { //len check by sc_yang 
			sprintf(errMsgBuf, ("Image len exceed max size 0x%x ! len=0x%x</b><br>"),fwSizeLimit, len);
			goto ret_upload;
		}
		if ( (flag == 1) || (flag == 3)) {
			if ( !fwChecksumOk(&upload_data[sizeof(IMG_HEADER_T)+head_offset], len)) {
				sprintf(errMsgBuf, ("Image checksum mismatched! len=0x%x, checksum=0x%x</b><br>"), len,
					*((unsigned short *)&upload_data[len-2]) );
				goto ret_upload;
			}
		}
		else {
			char *ptr = &upload_data[sizeof(IMG_HEADER_T)+head_offset];
			if ( !CHECKSUM_OK((unsigned char *)ptr, len) ) {
				sprintf(errMsgBuf, ("Image checksum mismatched! len=0x%x</b><br>"), len);
				goto ret_upload;
			}
		}

#ifndef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE
		if (flag == 3)
		{
			fh = open(FLASH_DEVICE_NAME1, O_RDWR);

#if defined(CONFIG_APP_FWD)
			if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), FLASH_DEVICE_NAME1);
			newfile = 2;
#endif			
		}
		else
		{
			fh = open(FLASH_DEVICE_NAME, O_RDWR);
#if defined(CONFIG_APP_FWD)			
			if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), FLASH_DEVICE_NAME);
			newfile = 2;
#endif			
		}
#else
		if (flag == 3) //rootfs
		{
			fh = open(Rootfs_dev_name[backup_bank-1], O_RDWR);
			
#if defined(CONFIG_APP_FWD)			
			if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), Rootfs_dev_name[backup_bank-1]);
			newfile = 2;
#endif			
		}
		else if (flag == 1) //linux
		{
			fh = open(Kernel_dev_name[backup_bank-1], O_RDWR);
#if defined(CONFIG_APP_FWD)			
			if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), Kernel_dev_name[backup_bank-1]);
			newfile = 2;
#endif			
		}
		else //web
		{
#if defined(CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE)
#if defined(CONFIG_RTL_FLASH_DUAL_IMAGE_WEB_BACKUP_ENABLE)
			fh = open(Web_dev_name[backup_bank-1],O_RDWR);
#if defined(CONFIG_APP_FWD)			
			if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), Web_dev_name[backup_bank-1]);
			newfile = 2;
#endif			
#else		
			fh = open(FLASH_DEVICE_NAME, O_RDWR);
#if defined(CONFIG_APP_FWD)			
			if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), FLASH_DEVICE_NAME);
			newfile = 2;
#endif	
#endif		
#else		
			fh = open(FLASH_DEVICE_NAME, O_RDWR);		
#if defined(CONFIG_APP_FWD)			
			if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), FLASH_DEVICE_NAME);
			newfile = 2;
#endif
#endif			
		}
#endif

		if ( fh == -1 ) {
			strcpy(errMsgBuf, ("File open failed!"));
		} else {
			if (flag == 1) {
				if (startAddr == -1) {
					//startAddr = CODE_IMAGE_OFFSET;
					startAddr = pHeader->burnAddr ;
#ifdef _LITTLE_ENDIAN_
					startAddr = DWORD_SWAP(startAddr);
#endif
				}
			}
			else if (flag == 3) {
				if (startAddr == -1) {
					startAddr = 0; // always start from offset 0 for 2nd FLASH partition
				}
			}
			else {
				if (startAddrWeb == -1) {
					//startAddr = WEB_PAGE_OFFSET;
					startAddr = pHeader->burnAddr ;
#ifdef _LITTLE_ENDIAN_
					startAddr = DWORD_SWAP(startAddr);
#endif
				}
				else
					startAddr = startAddrWeb;
			}
			lseek(fh, startAddr, SEEK_SET);
			
#if defined(CONFIG_APP_FWD)			
			{
				char tmpStr[20]={0};
				sprintf(tmpStr,"\n%d",startAddr);
				if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), tmpStr);
				newfile = 2;
			}
#endif			
			
			
						
			if (flag == 3) {
				locWrite += sizeof(IMG_HEADER_T); // remove header
				numLeft -=  sizeof(IMG_HEADER_T);
#if 0
				system("ifconfig br0 down 2> /dev/null");
				system("ifconfig eth0 down 2> /dev/null");
				system("ifconfig eth1 down 2> /dev/null");
				system("ifconfig ppp0 down 2> /dev/null");
				system("ifconfig wlan0 down 2> /dev/null");
				system("ifconfig wlan0-vxd down 2> /dev/null");		
				system("ifconfig wlan0-va0 down 2> /dev/null");		
				system("ifconfig wlan0-va1 down 2> /dev/null");		
				system("ifconfig wlan0-va2 down 2> /dev/null");		
				system("ifconfig wlan0-va3 down 2> /dev/null");
				system("ifconfig wlan0-wds0 down 2> /dev/null");
				system("ifconfig wlan0-wds1 down 2> /dev/null");
				system("ifconfig wlan0-wds2 down 2> /dev/null");
				system("ifconfig wlan0-wds3 down 2> /dev/null");
				system("ifconfig wlan0-wds4 down 2> /dev/null");
				system("ifconfig wlan0-wds5 down 2> /dev/null");
				system("ifconfig wlan0-wds6 down 2> /dev/null");
				system("ifconfig wlan0-wds7 down 2> /dev/null");
#if defined(CONFIG_RTL_92D_SUPPORT)	
				system("ifconfig wlan1 down 2> /dev/null");
				system("ifconfig wlan1-vxd down 2> /dev/null");		
				system("ifconfig wlan1-va0 down 2> /dev/null");		
				system("ifconfig wlan1-va1 down 2> /dev/null");		
				system("ifconfig wlan1-va2 down 2> /dev/null");		
				system("ifconfig wlan1-va3 down 2> /dev/null");
				system("ifconfig wlan1-wds0 down 2> /dev/null");
				system("ifconfig wlan1-wds1 down 2> /dev/null");
				system("ifconfig wlan1-wds2 down 2> /dev/null");
				system("ifconfig wlan1-wds3 down 2> /dev/null");
				system("ifconfig wlan1-wds4 down 2> /dev/null");
				system("ifconfig wlan1-wds5 down 2> /dev/null");
				system("ifconfig wlan1-wds6 down 2> /dev/null");
				system("ifconfig wlan1-wds7 down 2> /dev/null");
#endif
				kill_processes();
				sleep(2);
#endif
			}
#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE
			if (flag == 1) {  //kernel image
				pHeader->burnAddr = get_next_bankmark(Kernel_dev_name[active_bank-1],dual_enable);	//replace the firmware header with new bankmark //mark_debug		
			}
#endif

#if defined(CONFIG_APP_FWD)
			{
				char tmpStr[20]={0};
				sprintf(tmpStr,"\n%d",numLeft);
				if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), tmpStr);
				sprintf(tmpStr,"\n%d\n",locWrite+head_offset);
				if(!checkonly) write_line_to_file(FWD_CONF, (newfile==1?1:2), tmpStr);					
				newfile = 2;
			}

#else //#if defined(CONFIG_APP_FWD)
			numWrite = write(fh, &(upload_data[locWrite+head_offset]), numLeft);
			if (numWrite < numLeft) {
				sprintf(errMsgBuf, ("File write failed. locWrite=%d numLeft=%d numWrite=%d Size=%d bytes."), locWrite, numLeft, numWrite, upload_len);
				goto ret_upload;
			}

#endif //#if defined(CONFIG_APP_FWD)
			
			locWrite += numWrite;
			numLeft -= numWrite;
			sync();
			close(fh);

			head_offset += len + sizeof(IMG_HEADER_T) ;
			startAddr = -1 ; //by sc_yang to reset the startAddr for next image
			*update_fw = 1;
		}
	} //while //sc_yang   

	//fprintf(stderr,"####isUpgrade_OK###\n");
	
	
	return 1;
ret_upload: 
	fprintf(stderr, "%s\n", errMsgBuf);
	
	return 0;
}

void FirmwareUpgradeReboot(int shm_id, int update_cfg, int update_fw)
{
#if 1
	system("ifconfig br0 down 2> /dev/null");
	system("ifconfig eth0 down 2> /dev/null");
	system("ifconfig eth1 down 2> /dev/null");
	system("ifconfig ppp0 down 2> /dev/null");
	system("ifconfig wlan0 down 2> /dev/null");
	system("ifconfig wlan0-vxd down 2> /dev/null"); 	
	system("ifconfig wlan0-va0 down 2> /dev/null"); 	
	system("ifconfig wlan0-va1 down 2> /dev/null"); 	
	system("ifconfig wlan0-va2 down 2> /dev/null"); 	
	system("ifconfig wlan0-va3 down 2> /dev/null");
	system("ifconfig wlan0-wds0 down 2> /dev/null");
	system("ifconfig wlan0-wds1 down 2> /dev/null");
	system("ifconfig wlan0-wds2 down 2> /dev/null");
	system("ifconfig wlan0-wds3 down 2> /dev/null");
	system("ifconfig wlan0-wds4 down 2> /dev/null");
	system("ifconfig wlan0-wds5 down 2> /dev/null");
	system("ifconfig wlan0-wds6 down 2> /dev/null");
	system("ifconfig wlan0-wds7 down 2> /dev/null");
#if defined(CONFIG_RTL_92D_SUPPORT)	
	system("ifconfig wlan1 down 2> /dev/null");
	system("ifconfig wlan1-vxd down 2> /dev/null"); 	
	system("ifconfig wlan1-va0 down 2> /dev/null"); 	
	system("ifconfig wlan1-va1 down 2> /dev/null"); 	
	system("ifconfig wlan1-va2 down 2> /dev/null"); 	
	system("ifconfig wlan1-va3 down 2> /dev/null");
	system("ifconfig wlan1-wds0 down 2> /dev/null");
	system("ifconfig wlan1-wds1 down 2> /dev/null");
	system("ifconfig wlan1-wds2 down 2> /dev/null");
	system("ifconfig wlan1-wds3 down 2> /dev/null");
	system("ifconfig wlan1-wds4 down 2> /dev/null");
	system("ifconfig wlan1-wds5 down 2> /dev/null");
	system("ifconfig wlan1-wds6 down 2> /dev/null");
	system("ifconfig wlan1-wds7 down 2> /dev/null");
#endif
	kill_processes();
	sleep(2);

#endif
	
#ifndef NO_ACTION
//	isUpgrade_OK=1;
	
#if defined(CONFIG_APP_FWD)
	{			
			char tmpStr[20]={0};
			sprintf(tmpStr,"%d",shm_id);
			write_line_to_file("/var/fwd.ready", 1, tmpStr);
			sync();
			exit(0);
	}
#else	//#if defined(CONFIG_APP_FWD)
	system("reboot");
	for(;;);
#endif //#if defined(CONFIG_APP_FWD)

	
#else
#ifdef VOIP_SUPPORT
	// rock: for x86 simulation
	if (update_cfg && !update_fw) {
		if (apmib_reinit()) {
			//reset_user_profile();  // re-initialize user password
		}
	}
#endif
#endif
}

void print_usage(const char *exec_name)
{
	printf(	"usage: %s {check | reboot} {shmid <id> | file <image path>}\n"
		"shmid: specify the shared memory id, where the image already in.\n"
		"file: specify the image file path. The path is also used to "
		"generate key of the shared memory.\n"
		"ex: %s check file fw.bin; %s reboot check file fw.bin\n", exec_name, exec_name, exec_name);
}


int main(int argc, char *argv[])
{
	int shm_id;
	size_t shm_size;
	char *shm_ptr;
	char err_msg_buff[4096];
	int update_cfg, update_fw;
	int checkonly = 0;

	enum {
		ARG_IDX_EXEC   = 0,
		ARG_IDX_ACTION = 1,
		ARG_IDX_TYPE   = 2,
		ARG_IDX_PARAM  = 3,
	};

	if (argc <= ARG_IDX_ACTION) {
		print_usage(argv[ARG_IDX_EXEC]);
		return 0;
	}

	if (strcmp(argv[ARG_IDX_ACTION], "check")==0) {
		checkonly = 1;		
	}else if (strcmp(argv[ARG_IDX_ACTION], "reboot")==0) {
		checkonly = 0;
	} else {
		// not prep nor reboot
		print_usage(argv[ARG_IDX_EXEC]);
		return 0;
	}

	
	// check parameters and get shared memory
	if (argc <= ARG_IDX_TYPE) {
		print_usage(argv[ARG_IDX_EXEC]);
		return 0;
	}
	if (strcmp(argv[ARG_IDX_TYPE], "shmid")==0) {
		struct stat stat_buff;
		if (argc<=ARG_IDX_PARAM) {
			print_usage(argv[ARG_IDX_EXEC]);
			return 0;
		}
		shm_id = atoi (argv[ARG_IDX_PARAM]);
		shm_ptr = shmat(shm_id, NULL, 0);
		if (shm_ptr == NULL) {
			printf("Cannot get shared memory by shm_id=%d\n", shm_id);
			return 0;
		}
		if (fstat(shm_id, &stat_buff) == -1) {
			if (shmdt(shm_ptr) == -1) {
      				fprintf(stderr, "shmdt failed\n");
  			}
			printf("Cannot get size of shared memory shm_id=%d\n", shm_id);
			return 0;
		}
		shm_size = stat_buff.st_size;
	} else if (strcmp(argv[ARG_IDX_TYPE], "file")==0) {
		struct stat stat_buff;
		key_t key;
		FILE *fp;
		int left_size, offset, r;
		if (argc<=ARG_IDX_PARAM) {
			print_usage(argv[ARG_IDX_EXEC]);
			return 0;
		}

		// get file size
		if (stat(argv[ARG_IDX_PARAM], &stat_buff) == -1) {
			printf("Cannot get the filesize of %s.\n", argv[ARG_IDX_PARAM]);
			return 0;
		}
		shm_size = stat_buff.st_size;
		// get shared memory
		key = ftok(argv[ARG_IDX_PARAM], 0xE04);
		// shm_id = shmget(key, shm_size, IPC_CREAT | IPC_EXCL | 0666);	 TODO babylon: maybe should like this?
		shm_id = shmget(key, shm_size, IPC_CREAT | 0666);	
		if (shm_id == -1) {
			printf("Cannot create shared memory for file %s, key=%d, errno=%d\n", argv[ARG_IDX_PARAM], key, errno);
			return 0;
		}
		shm_ptr = shmat(shm_id, NULL, 0);
		if (shm_ptr == NULL) {
			printf("Cannot attach shared memory for file %s, key=%d, shm_id=%d, errno=%d\n", argv[ARG_IDX_PARAM], key, shm_id, errno);
			return 0;
		} 

		// copy file content to shared memory
		fp = fopen(argv[ARG_IDX_PARAM], "rb"); 
		if (fp==NULL) {
			printf("Cannot open file for read: %s\n", argv[ARG_IDX_PARAM]);
			if (shmdt(shm_ptr) == -1) 
      				fprintf(stderr, "shmdt failed\n");
			if(shmctl(shm_id, IPC_RMID, 0) == -1)
				fprintf(stderr, "shmctl(IPC_RMID) failed\n");
			return 0;
		}

		for (offset = 0, left_size = stat_buff.st_size; left_size>0; offset+=r, left_size-=r) {
			r = fread(&shm_ptr[offset], 1, left_size, fp);
		}

		fclose(fp);
	} else {
		print_usage(argv[ARG_IDX_EXEC]);
		return 0;
	}

	// Upgrade!!
	if(!FirmwareUpgradePrepare(shm_ptr, shm_size, shm_id, err_msg_buff, checkonly, &update_cfg, &update_fw)) {
		printf("Failed to upgrade firmware! error:\n");
		printf("%s\n", err_msg_buff);

#if defined(CONFIG_APP_FWD)		
		clear_fwupload_shm(shm_ptr, shm_id);
#endif
		return 0;
	}
	if (checkonly) {		
		printf(CHECK_OK_STRING);	
		clear_fwupload_shm(shm_ptr, shm_id);
	} else {
printf("Reboot!\n");	
		// Reboot!
		FirmwareUpgradeReboot(shm_id, update_cfg, update_fw);
	}
	return 0;
}
