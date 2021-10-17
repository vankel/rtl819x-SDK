/*the internal api.h for struct defines*/
#ifndef __RTK_DISK_ADAPTER_H__
#define __RTK_DISK_ADAPTER_H__
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <linux/sockios.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/stat.h>
//#include <net/if.h>
#include <linux/sockios.h>
#include <arpa/inet.h>

//#include "rtk_api.h"
#include "../common/common_utility.h"


//#define RTK_DISK_ADAPTER_DEBUG

//format related
#define EXT_FORMAT_TOOL  "mke2fs"
#define FAT_FORMAT_TOOL  "mkdosfs"
#define NTFS_FORMAT_TOOL "mkntfs"

#define EXT_SYSTYPE_PREFIX  "ext"
#define FAT_SYSTYPE_PREFIX  "fat"
#define NTFS_SYSTYPE_PREFIX "ntfs"

#define FAT16_SYSTYPE  "fat16"
#define FAT32_SYSTYPE  "fat32"

#define NTFS_COMMAND "ntfs-3g"


//partition related
#define MAX_ADD_PARTITION_NUMBER 8
#define MAX_LIST_PARTITION_NUMBER 16
#define MAX_DEVICE_NUMBER 8

#define PARTED_PARTITION_TYPE_LOGICAL "logical"
#define PARTED_PARTITION_TYPE_EXTENDED "extended"

#define NTFS_SYSTYPE_VALUE 1
#define EXT2_SYSTYPE_VALUE 2
#define EXT3_SYSTYPE_VALUE 3
#define EXT4_SYSTYPE_VALUE 4
#define FAT_SYSTYPE_VALUE  5

#define EXT2_SYSTYPE "ext2"
#define EXT3_SYSTYPE "ext3"
#define EXT4_SYSTYPE "ext4"
#define VFAT_SYSTYPE  "vfat"
#define FUSEBLK_SYSTYPE "fuseblk"  /* use userspace filesystem , currently only ntfs use userspace filesystem */

#define STORAGE_TYPE_USB_DEV    0x1  /*usb device */
#define STORAGE_TYPE_SD_DEV    0x2 /* SD  device  */


//structure used for df command info ---start
typedef struct __df_command_info {
	unsigned char name[16];
	unsigned char total[16];      //units blocks *1024 =bytes
	unsigned char used[16];       //units blocks *1024 =bytes
	unsigned char available[16];  //units blocks *1024 =bytes
	unsigned char percentage[16]; //use percentage
	unsigned char mounton[64];    
}df_command_info;
//structure used for df command info ---end

//structure used for list device info ---start
typedef struct __disk_partition_info {
	//int total_num;
	unsigned char name[16];
	unsigned long long totalsize; //units blocks *1024 =bytes
	unsigned long long partedsize; 
	
}disk_partition_info;
//structure used for list device info ---end

//structure used for list parted print info ---start
typedef struct __disk_parted_print_info{
	//int total;
	unsigned char number[4];
	unsigned char start[16]; //units MB??
	unsigned char end[16]; //units MB??
	unsigned char size[16]; //units MB??
	unsigned char filesystem[16];
	unsigned char type[16];
	unsigned char flag[16];
}disk_parted_print_info;
//structure used for list parted print info ---end

typedef struct partition_info {
	unsigned char name[16];   /* partition name, eg: sda1 */
	unsigned short index;     /* partition index, eg:1 */
	unsigned short systype;	  /* indicate the partition filesystem type, umount/unformat=0??? */
	unsigned char label[64];  /* partition label */
	unsigned char uuid[64];   /* partition uuid */
	unsigned long long total; /* total size, units: bytes */
	unsigned long long used;  /* already used size, units: bytes */
	unsigned long long free;  /* free size, units: bytes*/
}__attribute__ ((packed)) RTK_PARTITIONINFO_T, *RTK_PARTITIONINFO_Tp;

typedef struct device_info {
	unsigned char name[16];   /* device name, eg: sda */
	unsigned int number;      /* total partition count of each device */
	unsigned int type; 		  /* device type */
	unsigned long long total; /* total size, units: bytes  */
	unsigned long long used;  /* already used size, units: bytes */
	unsigned long long free;  /* free size, units: bytes */
	RTK_PARTITIONINFO_T partition_info[MAX_LIST_PARTITION_NUMBER];
}__attribute__ ((packed)) RTK_DEVICEINFO_T, *RTK_DEVICEINFO_Tp;

#endif
