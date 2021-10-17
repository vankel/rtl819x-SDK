/*DISK API*/
//#include "rtk_disk_adapter.h"
#include "rtk_api.h"


static int diskFormat(unsigned char *partition, unsigned char *systype)
{
	unsigned char tmpbuff[128] = {0};

	if (!partition || !systype)
		return -1;
	
	//umount first
	sprintf(tmpbuff,  "umount /dev/%s >/dev/null 2>&1", partition);
	system(tmpbuff);
	
	//format
	if (!strncmp(systype, EXT_SYSTYPE_PREFIX, strlen(EXT_SYSTYPE_PREFIX)))
	{
		sprintf(tmpbuff,  "%s -t %s /dev/%s >/dev/null 2>&1", EXT_FORMAT_TOOL, systype, partition);
	}
	else if (!strncmp(systype, FAT_SYSTYPE_PREFIX, strlen(FAT_SYSTYPE_PREFIX)))
	{
		if (!strncmp(systype, FAT16_SYSTYPE, strlen(FAT16_SYSTYPE)))
		{
			sprintf(tmpbuff,  "%s -F 16 /dev/%s >/dev/null 2>&1", FAT_FORMAT_TOOL,partition);
		}
		else if (!strncmp(systype, FAT32_SYSTYPE, strlen(FAT32_SYSTYPE)))
		{
			sprintf(tmpbuff,  "%s -F 32 /dev/%s >/dev/null 2>&1", FAT_FORMAT_TOOL,partition);
		}
		else
			sprintf(tmpbuff,  "%s /dev/%s >/dev/null 2>&1", FAT_FORMAT_TOOL,partition);
	}
	else if (!strncmp(systype, NTFS_SYSTYPE_PREFIX, strlen(NTFS_SYSTYPE_PREFIX)))
	{
		sprintf(tmpbuff,  "%s -f /dev/%s >/dev/null 2>&1", NTFS_FORMAT_TOOL, partition);
	}
	else
	{
		printf("%s %d currently not support! systype=%s \n", __FUNCTION__, __LINE__, systype);
		return -1;
	}		
	//printf("%s %d tmpbuff=%s \n", __FUNCTION__, __LINE__, tmpbuff);
	system(tmpbuff);

	//mount back
	sprintf(tmpbuff, "mkdir -p /var/tmp/usb/%s >/dev/null 2>&1", partition);
	system(tmpbuff);
	if (!strncmp(systype, NTFS_SYSTYPE_PREFIX, strlen(NTFS_SYSTYPE_PREFIX)))
	{
		sprintf(tmpbuff,  "%s /dev/%s /var/tmp/usb/%s -o force >/dev/null 2>&1", NTFS_COMMAND, partition, partition);
		system(tmpbuff);
	}
	else
	{
		sprintf(tmpbuff,  "mount /dev/%s /var/tmp/usb/%s >/dev/null 2>&1", partition, partition);
		system(tmpbuff);
	}
	
	return 0;		
}

/**************************************************
* @NAME:
* 	rtk_disk_format
* 
* @PARAMETERS:
* 	@Input
* 		partition_name: the partition name to format 
*		systype: the system type of partition_name
*		 	    
* 	@Output
*	 	none
*
* @RETRUN:
* 	-1 fail
*	0 success
* 
* @FUNCTION :
* 	format the partition 
*
***************************************************/
int rtk_disk_format(unsigned char *partition_name, unsigned char *systype)
{
	int ret = RTK_SUCCESS;

	if (!partition_name || !systype)
		return RTK_FAILED;
	
	if (diskFormat(partition_name, systype) != 0)
	{
		#if defined(RTK_DISK_ADAPTER_DEBUG)
		printf("%s %d disk formate error! name=%s systype=%s \n", __FUNCTION__, __LINE__, partition_name, systype);
		#endif
		ret = RTK_FAILED;
	}

	return ret;
}

//flag =1 only get partition name 'sdb1:sdb2:sdb3.....'
//flag=2  get disk name/size/.....
int partitionInfo(struct __disk_partition_info *info, unsigned char *command, unsigned char *tmpfile, 
					unsigned char *buff, int buff_len, int flag, int *total)
{
	unsigned char tmpbuff[512] = {0}, tmpbuff2[512] = {0};
	unsigned char name[16] = {0}, tmpname[16] = {0};
	unsigned char major[12] = {0}, minor[12] = {0}, block[32]={0}; 
	FILE *fp;
	unsigned char *ptr = NULL, *endptr = NULL;
	int ret = 0, i =  0;
	unsigned long long blocks, tmp_blocks; 
	

	
if ((flag == 2)&&(!info || !command || !tmpfile || !total))
		return -1;
	
if ((flag == 1)&&(!buff || !command || !tmpfile))
		return -1;
	
	sprintf(tmpbuff2, "%s > %s", command, tmpfile);
	system(tmpbuff2);
	sync();	
	fp = fopen(tmpfile,"r");
	if(fp == NULL)
	{
		//failed
		return -1;
	}
	memset(tmpbuff2, '\0', sizeof(tmpbuff2));
	memset(tmpbuff, '\0', sizeof(tmpbuff));
	memset(tmpname, '\0', sizeof(tmpname));
	tmp_blocks = 0;
	while (fgets(tmpbuff2, sizeof(tmpbuff2), fp))
	{
	
		memset(major, '\0', sizeof(major));
		memset(minor, '\0', sizeof(minor));
		memset(block, '\0', sizeof(block));
		memset(name, '\0', sizeof(name));
		
		ptr = strstr(tmpbuff2, "sd");
		if (ptr)
		{
			ret = sscanf(tmpbuff2, "%s %s %s %s", major, minor, block, name);
			//printf("%s %d: major=%s minor=%s blocks=%s name=%s\n", __FUNCTION__, __LINE__, major, minor, block, name);
			if (flag == 2)
			{
				if (strlen(name) == 3)//eg:sda/sdb.....
				{
					if (tmpname[0] && strncmp(name, tmpname, 3))//eg:sda ->sdb???
					{
						info[i].partedsize = tmp_blocks;
						
						tmp_blocks = 0;
						i++;//next
					}
					memcpy(tmpname, name, 3);
					//printf("%s %d: tmpname=%s name=%s\n", __FUNCTION__, __LINE__, tmpname, name);
					//blocks = atoll(block);
					blocks = strtoull(block, &endptr, 0);
					info[i].totalsize = blocks;
					memcpy(info[i].name, tmpname, 3);
					memset(tmpbuff2, '\0', sizeof(tmpbuff2));
					continue;
				}
				if (!strncmp(tmpname, name, 3))
				{
					//tmp_blocks += atoll(block);
					tmp_blocks += strtoull(block, &endptr, 0);
				}
			}
			if (flag == 1)
			{
				if (strlen(name) <= 3)//eg:skip sda/sdb.....
					continue;
				//if (atoll(block) < 100)//fix me ? extend partition
				if (strtoull(block, &endptr, 0) < 100)//fix me ? extend partition
					continue;
				strcat(tmpbuff, name);
				strcat(tmpbuff, ":");
			}
		}			
		memset(tmpbuff2, '\0', sizeof(tmpbuff2));
	}
	if (flag == 2)
	{
		info[i].partedsize = tmp_blocks;
		tmp_blocks = 0;
		i++;//next
		*total = i > MAX_DEVICE_NUMBER?MAX_DEVICE_NUMBER:i;
		//dump
		#if 0
		for(i = 0; i < *total; i++)
		{
			printf("%s %d: totolsize=%llu partedsize=%llu name=%s number=%d\n", __FUNCTION__, __LINE__, 
				info[i].totalsize, info[i].partedsize, info[i].name, *total);
		}
		#endif
	}
	if (flag == 1)
	{
		snprintf(buff, buff_len, "%s", tmpbuff);
		//printf("%s %d: buff=%s\n", __FUNCTION__, __LINE__, buff);
	}

	fclose(fp);
	sprintf(tmpbuff2, "rm %s", tmpfile);
	system(tmpbuff2);

	return 0;
	
}

int pharsePartedPrint(disk_parted_print_info *info, unsigned char *cmd, unsigned char *dev, unsigned char *tmpfile, int *total)
{
	unsigned char tmpbuff[512] = {0};
	unsigned char number[4], start[16],end[16], size[16], filesys[16], type[16],flags[16];
	unsigned char *ptr;
	FILE *fp;
	int i, ret, flag;
	
	if (!info || !cmd || !dev || !tmpfile)
		return -1;
	
	sprintf(tmpbuff, "%s %s p > %s", cmd, dev, tmpfile);
	system(tmpbuff);
	sync();	
	fp = fopen(tmpfile,"r");
	if(fp == NULL)
	{
		//failed
		return -1;
	}
	flag = 0;
	i = 0;
	memset(tmpbuff, '\0', sizeof(tmpbuff));
	while (fgets(tmpbuff, sizeof(tmpbuff), fp))
	{
		memset(number, '\0', sizeof(number));
		memset(start, '\0', sizeof(start));
		memset(end, '\0', sizeof(end));
		memset(size, '\0', sizeof(size));
		memset(filesys, '\0', sizeof(filesys));
		memset(type, '\0', sizeof(type));
		memset(flags, '\0', sizeof(flags));
		//printf("%s %d tmpbuff=%s \n", __FUNCTION__, __LINE__, tmpbuff);
		if (tmpbuff[0] == '\r' || tmpbuff[0] == '\n')
			continue;
		
		ptr = strstr(tmpbuff, "Number");
		if (!flag && !ptr)//skip start message
			continue;
		if (ptr)
		{
			flag = 1;
			continue;
		}		
		if (flag)//start to handle partition info
		{
			ret = sscanf(tmpbuff, "%s %s %s %s %s %s %s", number, start, end, size, type, filesys, flags);
			//printf("%s %d: number=%s start=%s end=%s size=%s filessys=%s name=%s flags=%s ret=%d\n", __FUNCTION__, __LINE__, 
				//number, start, end, size, filesys, type, flags, ret);
			#if 0
			if (!strncmp(type, "extended", strlen("extended")))
				continue;
			#endif
			
			memcpy(info[i].number, number, strlen(number));
			memcpy(info[i].start, start, strlen(start));
			memcpy(info[i].end, end, strlen(end));
			memcpy(info[i].size, size, strlen(size));
			memcpy(info[i].type, type, strlen(type));
			//printf("%s %d filesys[0]=0x%x\n", __FUNCTION__, __LINE__, filesys[0]);
			if (!filesys[0])
				memcpy(info[i].filesystem, "unformat", strlen("unformat"));
			else
				memcpy(info[i].filesystem, filesys, strlen(filesys));
			memcpy(info[i].flag, flags, strlen(flags));
			i++;
			
		}
		memset(tmpbuff, '\0', sizeof(tmpbuff));
	}
	*total = i;
	
	#if 0
	//dump
	for(i = 0; i < *total; i++)
	{
		printf("%s %d: number=%s start=%s end=%s size=%s name=%s filesys=%s flag=%s *total=%d\n", __FUNCTION__, __LINE__, 
			info[i].number, info[i].start, info[i].end, info[i].size, info[i].type, info[i].filesystem, info[i].flag, *total);
	}
	#endif
	return 0;
}

int getDiskFileSystem(unsigned char *name, unsigned char *filesystem, unsigned char *tmpfile)
{
	FILE *fp;
	unsigned char tmpbuf[256] = {0}, type[10] = {0};
	unsigned char *ptr = NULL, *ptr2 = NULL, *ptr3 = NULL;
	int j = 0, ret = -1, len = 0; 
	
	if (!name || !filesystem || !tmpfile)
		return ret;
	snprintf(tmpbuf, 256, "mount | grep %s > %s", name, tmpfile);
	system(tmpbuf);
	//sleep(5);
	fp = fopen(tmpfile,"r");
	if(fp == NULL)
		return ret;
	
	memset(tmpbuf, '\0', sizeof(tmpbuf));
	while (fgets(tmpbuf, 256, fp)) 
	{
		ptr = strstr(tmpbuf, name);
		if (ptr)
		{
			//exist
			ptr = strstr(tmpbuf, "type");
			if (ptr)
			{
				#if 1
				if (sscanf(ptr, "%s %[^ ]", type, filesystem) > 0)
				{
					ret = 0;
					break;
				}
				#else
				ptr2 = strstr(ptr, " ");
				if (ptr2)
				{
					ptr3 = strstr(ptr2 + 1, " ");
					//find
					if (ptr3)
					{
						len = ptr3 - ptr2 - 1;
						if (len >= 0)
						{
							memcpy(filesystem, ptr2 + 1, len);
							printf("%s %d name=%s filesystem=%s \n", __FUNCTION__, __LINE__, name, filesystem);
							ret = 0;
							break;
						}
					}
				}
				#endif
			}

		}
		memset(tmpbuf, '\0', sizeof(tmpbuf));
		memset(type, '\0', sizeof(type));
	}
	
	if (ret == 0)
	{	
		if (!strncmp(filesystem, "vfat", strlen("vfat")))
		{
			strncpy(filesystem, FAT_SYSTYPE_PREFIX, strlen(FAT_SYSTYPE_PREFIX));
			filesystem[strlen(FAT_SYSTYPE_PREFIX)] = '\0';
		}
	}
	
	fclose(fp);
	sprintf(tmpbuf, "rm %s", tmpfile);
	system(tmpbuf);

	return ret;
}

int get_item_value(unsigned char *str, unsigned char *value)
{
	unsigned char *p, *pend;
	unsigned char tmpbuff[128] = {0};
	
	
	if (!str || !value)
		return -1;
	
	memset(tmpbuff, '\0', sizeof(tmpbuff));
	sscanf(str, "%*[^=]=%[^ ]", tmpbuff);
	//printf("%s %d tmpbuff=%s \n", __FUNCTION__, __LINE__, tmpbuff);
	p=tmpbuff;
	while(*p == ' ' || *p == '"')
		p++;

	pend = p + strlen(p) - 1;

	while(*pend == ' ' || *pend == '"')
		*pend = '\0';

	strcpy(value, p);
	//printf("%s %d value=%s \n", __FUNCTION__, __LINE__, value);
	
	return 0;
}

/*
 * convert ASCII characters using '^' and M- notation to original characters.
 * 
 */
int convertLabel(unsigned char *oldlabel, unsigned char *newlabel)
{
	unsigned char ch, ch1, ch2;
	unsigned char *pstr = NULL, *pnew = NULL;
	int len = 0; 
	
	if (!oldlabel || !newlabel)
		return -1;
		
	len = strlen(oldlabel);	
	pstr = oldlabel;
	pnew =  newlabel;
	while (len--) 
	{
		ch = *pstr;
		ch1 = *(pstr + 1);
		ch2 = *(pstr + 2);
		if ((ch == 'M')&&(ch1 == '-'))
		{
			//"M-'' case
			ch2 += 128;
			pstr = pstr + 3;
			*pnew = ch2;
			pnew++;
		}
		else if (ch == '^')
		{
			//'^' case
			ch1 ^= 0x40;			
			pstr = pstr + 2;
			*pnew = ch1;
			pnew++;
		}
		else
		{
			//normal case
			pstr = pstr + 1;			
			*pnew = ch;
			pnew++;
		}
	}
	//printf("%s %d oldlabel=%s newlabel=%s \n", __FUNCTION__, __LINE__, oldlabel, newlabel);
	
	return 0;
}
int getBlkidCmdinfo(unsigned char *name, unsigned char *label, unsigned char *uuid, unsigned char *type, unsigned char *tmpfile)
{
	FILE *fp;
	unsigned char tmpbuf[256] = {0}, tmplabel[64] = {0};
	unsigned char *ptr = NULL;
	int j = 0, ret = -1, len = 0; 
	
	if (!name || !tmpfile)
		return ret;
	
	snprintf(tmpbuf, 256, "blkid %s > %s", name, tmpfile);
	system(tmpbuf);

	fp = fopen(tmpfile,"r");
	if(fp == NULL)
		return ret;
	
	memset(tmpbuf, '\0', sizeof(tmpbuf));
	while (fgets(tmpbuf, 256, fp)) 
	{
		ptr = strstr(tmpbuf, name);
		if (ptr)
		{
			//exist
			if (label)
			{
				ptr = strstr(tmpbuf, "LABEL");
				get_item_value(ptr, tmplabel);
				convertLabel(tmplabel, label);
				
			}
			if (uuid)
			{
				ptr = strstr(tmpbuf, "UUID");
				get_item_value(ptr, uuid);
			}
			if (type)
			{
				ptr = strstr(tmpbuf, "TYPE");
				get_item_value(ptr, type);
			}
			break;
		}
		memset(tmpbuf, '\0', sizeof(tmpbuf));
	}
	
	fclose(fp);
	sprintf(tmpbuf, "rm %s", tmpfile);
	system(tmpbuf);

	return ret;
}

//case-insensitive strstr function
const char* strstri(const char* str, const char* subStr)
{
	int len = strlen(subStr);
	if(len == 0)
	{
		return NULL;
	}

	while(*str)
	{
		if(strncasecmp(str, subStr, len) == 0)
		{
			return str;
		}
		++str;
	}
	return NULL;
}

//default unit bytes
int strtolonglongbytes(unsigned char *str, unsigned long long *value)
{
	unsigned char *ptr, *endptr;
	
	if (!str || !value)
		return -1;
	
	*value = strtoull(str, &endptr, 0);
	ptr = strstri(str, "MB");
	if (ptr)
	{
		*value = (*value)*1000*1000;
		return 0;
	}
		
	ptr = strstri(str, "GB");
	if (ptr)
	{
		*value = (*value)*1000*1000*1000;
		return 0;
	}
	
	ptr = strstri(str, "KB");
	if (ptr)
	{
		*value = (*value)*1000;
		return 0;
	}
		
	return 0;
}

int convertFileSystemToNumber(unsigned char *fs, unsigned short *systype)
{
	if (!fs || !systype)
		return -1;

	if (!strncmp(fs, EXT2_SYSTYPE, strlen(EXT2_SYSTYPE)))
	{
		*systype = EXT2_SYSTYPE_VALUE;
	}
	else if (!strncmp(fs, EXT3_SYSTYPE, strlen(EXT3_SYSTYPE)))
	{
		*systype = EXT3_SYSTYPE_VALUE;
	}
	else if (!strncmp(fs, EXT4_SYSTYPE, strlen(EXT4_SYSTYPE)))
	{
		*systype = EXT4_SYSTYPE_VALUE;
	}
	else if (!strncmp(fs, FAT_SYSTYPE_PREFIX, strlen(FAT_SYSTYPE_PREFIX)))
	{
		*systype = FAT_SYSTYPE_VALUE;
	}	
	else if (!strncmp(fs, FUSEBLK_SYSTYPE, strlen(FUSEBLK_SYSTYPE)))
	{
		*systype = NTFS_SYSTYPE_VALUE;
	}
	else
		*systype = 0;

	return 0;
}
	
int getDfCmdInfoByDevName(unsigned char *name, df_command_info *pinfo, unsigned char *tmpfile)
{
	FILE *fp;
	unsigned char tmpbuf[256] = {0};
	unsigned char *ptr;
	int ret = -1; 
	
	if (!name || !pinfo || !tmpfile)
		return ret;
	
	snprintf(tmpbuf, 256, "df | grep %s > %s", name, tmpfile);
	system(tmpbuf);

	fp = fopen(tmpfile,"r");
	if(fp == NULL)
		return ret;
	
	memset(tmpbuf, '\0', sizeof(tmpbuf));
	while (fgets(tmpbuf, 256, fp)) 
	{
		ptr = strstr(tmpbuf, name);
		if (ptr)
		{
			//exist
			memset((void *)pinfo, '\0', sizeof(df_command_info));
			if (sscanf(ptr, "%s %s %s %s %s %s", pinfo->name, pinfo->total, pinfo->used, pinfo->available, pinfo->percentage, pinfo->mounton) > 0)
			{
				ret = 0;
				break;
			}

		}
		memset(tmpbuf, '\0', sizeof(tmpbuf));
	}
	
	fclose(fp);
	sprintf(tmpbuf, "rm %s", tmpfile);
	system(tmpbuf);

	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_storage_info
* 
* @PARAMETERS:
* 	@Input
* 		pinfo: store the storage device info entry
*		empty_entry_num: the number of pinfo
*		
* 	@Output
*	 	num: the actual number of the storage device
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get storage info 
*
***************************************************/
int rtk_get_storage_info(unsigned int *num, RTK_DEVICEINFO_T *pinfo, unsigned int empty_entry_num)
{
	int ret = RTK_SUCCESS;
	int i = 0, j = 0, number = 0, total = 0, flag = 0, count_dev = 0, count_partition = 0, real_count = 0;
	unsigned char tmpbuff[256] = {0}, partition_name[16] = {0}, filesystem[20] = {0};
	struct __disk_partition_info cat_partition_info[MAX_DEVICE_NUMBER];
	disk_parted_print_info parted_print_info[MAX_LIST_PARTITION_NUMBER];
	df_command_info dfcmd;
	unsigned char *endptr;
	unsigned long long used;
	unsigned char type[16] = {0};
	//safety check
	if (!num || !pinfo)
		return RTK_FAILED;

	memset(cat_partition_info, 0x00, sizeof(cat_partition_info));
	ret = partitionInfo(cat_partition_info, "cat /proc/partitions", "/var/tmp/api_parttiton.txt", tmpbuff, 256, 2, &number);
	if (ret != 0)
	{
		#if defined(RTK_DISK_ADAPTER_DEBUG)
		printf("%s %d get partitionInfo error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*num = number;

	memset((void *)pinfo, '\0', sizeof(RTK_DEVICEINFO_T)*empty_entry_num);
	
	count_dev = (number >= empty_entry_num)?empty_entry_num:number;
	for (i = 0; i < count_dev; i++)
	{
		memset(parted_print_info, 0x00, sizeof(parted_print_info));
		sprintf(tmpbuff, "/dev/%s", cat_partition_info[i].name);
		ret = pharsePartedPrint(parted_print_info, "parted", tmpbuff, "/var/tmp/api_parted_tmp.txt", &total);
		if (ret != 0)
		{
			#if defined(RTK_DISK_ADAPTER_DEBUG)
			printf("%s %d pharsePartedPrint error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
		
		flag = 0;
		used = 0;
		real_count = 0;
		count_partition = (total >= MAX_LIST_PARTITION_NUMBER)?MAX_LIST_PARTITION_NUMBER:total;
		#if defined(RTK_DISK_ADAPTER_DEBUG)
		printf("%s %d total=%d count_partition=%d\n", __FUNCTION__, __LINE__, total,count_partition );
		#endif
		for (j = 0; j < count_partition; j++)
		{
			
			if (!strncmp(parted_print_info[j].type, PARTED_PARTITION_TYPE_EXTENDED, strlen(PARTED_PARTITION_TYPE_EXTENDED)))
			{
				flag = 1;
				continue; //skip extended partition
			}
			
			memset(filesystem, '\0', sizeof(filesystem));
			memset(partition_name, '\0', sizeof(partition_name));
			#if defined(RTK_DISK_ADAPTER_DEBUG)
			printf("%s %d j=%d filesystem=%s \n", __FUNCTION__, __LINE__, j, parted_print_info[j].filesystem);
			#endif
			sprintf(partition_name, "/dev/%s%s", cat_partition_info[i].name, parted_print_info[j].number);
			#if defined(RTK_DISK_ADAPTER_DEBUG)
			printf("%s %d partition_name=%s \n", __FUNCTION__, __LINE__, partition_name);
			#endif
			
			//partition uuid and label
			getBlkidCmdinfo(partition_name, &pinfo[i].partition_info[real_count].label[0], &pinfo[i].partition_info[real_count].uuid[0], type, "/tmp/api_blkid.txt");
			#if defined(RTK_DISK_ADAPTER_DEBUG)
			printf("%s %d label=%s uuid=%s type=%s \n", __FUNCTION__, __LINE__, &pinfo[i].partition_info[real_count].label[0], &pinfo[i].partition_info[real_count].uuid[0], type);
			#endif
			
			if(getDiskFileSystem(partition_name, filesystem, "/tmp/boa_par_mount_tmp.txt") != 0)
			{
				sprintf(filesystem, "%s", "unformat");
			}
			//partition filesystem type
			convertFileSystemToNumber(filesystem, &pinfo[i].partition_info[real_count].systype);
			//partition index
			pinfo[i].partition_info[real_count].index = (unsigned short)atoi(parted_print_info[real_count].number);
			//partition name
			sprintf(pinfo[i].partition_info[real_count].name, "%s%s", cat_partition_info[i].name, parted_print_info[real_count].number);
			//run df cmd 
			memset((void *)&dfcmd, '\0', sizeof(dfcmd));
			ret = getDfCmdInfoByDevName(partition_name, &dfcmd, "/var/tmp/api_df.txt");
			if (ret == 0) //format and mount case
			{
				pinfo[i].partition_info[real_count].total = strtoull(dfcmd.total, &endptr, 0);
				pinfo[i].partition_info[real_count].total = pinfo[i].partition_info[real_count].total*1024;
				pinfo[i].partition_info[real_count].used = strtoull(dfcmd.used, &endptr, 0);
				pinfo[i].partition_info[real_count].used = pinfo[i].partition_info[real_count].used*1024;
				used += pinfo[i].partition_info[real_count].used;
				pinfo[i].partition_info[real_count].free = strtoull(dfcmd.available, &endptr, 0);
				pinfo[i].partition_info[real_count].free = pinfo[i].partition_info[real_count].free*1024;
			}
			else //get partition total size from parted /dev/sdx p
			{
				 strtolonglongbytes(parted_print_info[real_count].size, &pinfo[i].partition_info[real_count].total);
			}
			real_count++;
		}
		//total partition of the device
		pinfo[i].number = flag?(total -1):total;
		//device name
		sprintf(pinfo[i].name, "%s", cat_partition_info[i].name);
		//device type, todo
		pinfo[i].type = STORAGE_TYPE_USB_DEV;
		//device size, total/free/used
		pinfo[i].total = cat_partition_info[i].totalsize*1024;
		pinfo[i].used = used;
		pinfo[i].free = pinfo[i].total - pinfo[i].used;
	}
	ret = RTK_SUCCESS;
	
get_error:
	return ret;
}


