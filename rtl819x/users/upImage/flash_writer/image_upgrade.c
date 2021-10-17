#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <string.h>
//#include <sys/types.h> 
//#include <sys/uio.h> 
//#include <unistd.h> 
//#include <signal.h>
//#include <sys/ioctl.h>
//#include <sys/wait.h> 
#include <sys/stat.h>

#define FLASH_DEVICE_KERNEL "/dev/mtdblock0"
#define FLASH_DEVICE_ROOTFS "/dev/mtdblock1"
#define MAX_FILENAME_LENGTH 50

#define SIGNATURE_LEN                   4
#define FW_HEADER_WITH_ROOT     ((char *)"cr6c")
#define FW_HEADER                       ((char *)"cs6c")
#define ROOT_HEADER                     ((char *)"r6cr")
#define WEB_HEADER                      ((char *)"w6cg") //no ap sign now //mark_issue

#define SIGNATURE_ERROR		(1<<0)
#define WEBCHECKSUM_ERROR	(1<<1)
#define ROOTCHECKSUM_ERROR	(1<<2)
#define LINUXCHECKSUM_ERROR	(1<<3)

//flash|memory data cmp
#define FLASHDATA_ERROR		(1<<8)
#define WEBDATA_ERROR		(1<<9)
#define LINUXDATA_ERROR		(1<<10)
#define ROOTDATA_ERROR		(1<<11)

typedef struct img_header {
        unsigned char signature[SIGNATURE_LEN];
        unsigned int startAddr;
        unsigned int burnAddr;
        unsigned int len;
} IMG_HEADER_T, *IMG_HEADER_Tp;

static int firm_upgrade_status = 0;

void rtk_all_interface(int action)
{
	switch(action)
	{
		case 0:
		    system("ifconfig ppp0 down> /dev/console");
			system("ifconfig eth0 down> /dev/console");
			system("ifconfig eth1 down> /dev/console");
			system("ifconfig eth2 down> /dev/console");
			system("ifconfig eth3 down> /dev/console");
			system("ifconfig eth4 down> /dev/console");
			system("ifconfig wlan0 down> /dev/console");
			system("ifconfig wlan1 down> /dev/console");
			break;
		case 1:
			system("ifconfig eth0 up > /dev/console");
			system("ifconfig eth1 up > /dev/console");
			system("ifconfig eth2 up > /dev/console");
			system("ifconfig eth3 up > /dev/console");
			system("ifconfig eth4 up> /dev/console");
			system("ifconfig wlan0 up> /dev/console");
			system("ifconfig wlan1 up> /dev/console");
			system("ifconfig ppp0 up> /dev/console");
			break;
		default:
			printf("error rtk_all_interface\n");
			break;
	}
}
void rtk_check_flash_mem(int linuxOffset,int linuxlen,int webOffset,int weblen,int rootOffset,int rootlen,char* upload_data,
						int webhead,int roothead,int linuxhead)
{
	int fh;
	char *flash_mem = NULL;

	/* mtdblock0 */
	fh = open(FLASH_DEVICE_KERNEL, O_RDWR);
	if(fh == -1)
	{
		firm_upgrade_status |= FLASHDATA_ERROR;
		return;
	}

	/* linux */
	if(linuxOffset  != -1)
	{
		lseek(fh, linuxOffset, SEEK_SET);
		flash_mem = (char*)malloc(linuxlen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,linuxlen);
		if(memcmp(&(upload_data[linuxhead]),flash_mem,linuxlen) != 0)
		{
			firm_upgrade_status |= LINUXDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}

	/* web */
	if(webOffset != -1)
	{
		lseek(fh, webOffset, SEEK_SET);
		flash_mem = (char*)malloc(weblen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,weblen);
		if(memcmp(&(upload_data[webhead]),flash_mem,weblen) != 0)
		{
			firm_upgrade_status |= WEBDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}
	close(fh);

	/* mtdblock1 */
	/* root */
	fh = open(FLASH_DEVICE_ROOTFS, O_RDWR);
	if(fh == -1)
	{
		firm_upgrade_status |= FLASHDATA_ERROR;
		return;
	}
	
	if(rootOffset != -1)
	{
		lseek(fh, rootOffset, SEEK_SET);
		flash_mem = (char*)malloc(rootlen);
		if(flash_mem == NULL)
		{
			firm_upgrade_status |= FLASHDATA_ERROR;
			close(fh);
			return;
		}

		read(fh,flash_mem,rootlen);
		if(memcmp(&(upload_data[roothead]),flash_mem,rootlen) != 0)
		{
			firm_upgrade_status |= ROOTDATA_ERROR;
		}
		free(flash_mem);
		flash_mem == NULL;
	}

	close(fh);
}


int rtk_FirmwareUpgrade(char *upload_data, int upload_len)
{
    int fh = 0,head_offset = 0,len = 0,locWrite = 0,numLeft = 0,numWrite = 0;;
	int flag=0, startAddr=-1, startAddrWeb=-1;	
	IMG_HEADER_Tp pHeader;
	unsigned char buffer[200];
	int webOffset = -1,weblen,rootOffset = -1,rootlen,linuxOffset = -1,linuxlen;
	int webhead,roothead,linuxhead; /*data will burn to flash start address*/
	int fwSizeLimit = 0x800000;
    int ret = 0;

	firm_upgrade_status = 0;
	/* do interface down */
	//rtk_all_interface(0);
	while(head_offset <   upload_len) {
    	locWrite = 0;
		pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
		len = pHeader->len;
		numLeft = len + sizeof(IMG_HEADER_T) ;

		// check header and checksum
		if (!memcmp(&upload_data[head_offset], FW_HEADER, SIGNATURE_LEN) ||
		    !memcmp(&upload_data[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN))
			flag = 1;
		else if (!memcmp(&upload_data[head_offset], WEB_HEADER, SIGNATURE_LEN))
		    flag = 2;
		else if (!memcmp(&upload_data[head_offset], ROOT_HEADER, SIGNATURE_LEN)){
		    flag = 3;
		}	
		else {
		    strcpy(buffer, "Invalid File Format!\n");
			firm_upgrade_status |= SIGNATURE_ERROR;
			goto ret_upload;
		}
		if(flag == 3)
		    fh = open(FLASH_DEVICE_ROOTFS, O_RDWR);
		else
		    fh = open(FLASH_DEVICE_KERNEL, O_RDWR);

		if ( fh < 0 ) {
		    strcpy(buffer, "MTD Devices open failed!\n");
			goto ret_upload;
		} else {
		    if (flag == 1) {
				if ( startAddr == -1){
				    startAddr = pHeader->burnAddr ;
				}
				linuxOffset = startAddr;
			}
			else if (flag == 3) {
				if ( startAddr == -1)
				    startAddr = 0; // always start from offset 0 for 2nd FLASH partition
				rootOffset = startAddr;
			}
			else {
				if ( startAddrWeb == -1)
				    startAddr = pHeader->burnAddr ;
				else
				    startAddr = startAddrWeb;
				webOffset = startAddr;
			}
			ret = lseek(fh, startAddr, SEEK_SET);
            if(ret < 0){
                printf("lseek ERROR\n");
                goto ret_upload;
            }
            //printf("ret = 0x%x , startAddr = 0x%x\n",ret,startAddr);
			if(flag == 3){
				//fprintf(stderr,"\r\n close all interface\n");		
				locWrite += sizeof(IMG_HEADER_T); // remove header
				numLeft -=  sizeof(IMG_HEADER_T);
			}	
			//fprintf(stderr,"\r\n flash write");	

			if(flag == 1){
				linuxlen = numLeft;
				linuxhead = locWrite+head_offset;
				printf("burn kernel start,please wait...\n");
			}else if(flag == 2){
				weblen  = numLeft;
				webhead = locWrite+head_offset;
				printf("burn webpage start,please wait...\n");
			}else if(flag == 3){
				rootlen = numLeft;
				roothead = locWrite+head_offset;
				printf("burn rootfs start,please wait...\n");
			}

			//numWrite = write(fh, &(upload_data[locWrite+head_offset]), numLeft);
            numWrite = 0;
            ret = 0;
            while(numWrite < numLeft&& numWrite >= 0)
            {
                ret = write(fh, &(upload_data[locWrite+head_offset+numWrite]), numLeft - numWrite);
                if(ret <= 0){
                    //printf("write ERROR\n");
                    break;
                }
                numWrite += ret;
            }
			//numWrite = numLeft;
			/*sprintf(buffer,"write flash flag=%d,locWrite+head_offset=%d,numLeft=%d,startAddr=%x\n",
			  flag,locWrite+head_offset,numLeft,startAddr);
			  fprintf(stderr, "%s\n", buffer);
			  hex_dump(&(upload_data[locWrite+head_offset]),16);*/

			if (numWrite != numLeft) {
		        sprintf(buffer, "File write failed. currentWrite = %d locWrite=%d numLeft=%d numWrite=%d Size=%d bytes.",ret, locWrite, numLeft, numWrite, upload_len);
					goto ret_upload;
			}
			locWrite += numWrite;
			numLeft -= numWrite;
			sync();

			if(flag == 1)
				printf("burn kernel done\n");
			else if(flag == 2)
				printf("burn webpage done\n");
			else 
				printf("burn rootfs done\n");
#ifdef KERNEL_3_10
			if(ioctl(fh,BLKFLSBUF,NULL) < 0){
		       	printf("flush mtd system cache error\n");
			}
#endif
			close(fh);

			head_offset += len + sizeof(IMG_HEADER_T) ;
			startAddr = -1 ; //to reset the startAddr for next image	
		}
	} /*while*/
	//rtk_check_flash_mem(linuxOffset,linuxlen,webOffset,weblen,rootOffset,rootlen,upload_data,webhead,roothead,linuxhead);

	/* ifconfig interface up */
	//rtk_all_interface(1);

	return 0;
ret_upload:	
	fprintf(stderr, "%s\n", buffer);	
    ret = -1;

	//rtk_all_interface(1);

	return ret;
}

static inline int CHECKSUM_OK(unsigned char *data, int len)
{
        int i;
        unsigned char sum=0;
        for (i=0; i<len; i++)
            sum += data[i]; /*per byte*/
        if (sum == 0)
            return 1;
		else
            return 0;
}

static int fwChecksumOk(char *data, int len)
{
        unsigned short sum=0;
        int i;
        for (i=0; i<len; i+=2) { /*per short*/
#ifdef _LITTLE_ENDIAN_
            sum += WORD_SWAP( *((unsigned short *)&data[i]) );
#else /*default*/
			sum += *((unsigned short *)&data[i]);
#endif
        }
        return((sum==0) ? 1 : 0);
}

static int rtk_FirmwareCheck(char *upload_data, int upload_len)
{
	int len = 0, head_offset = 0, flag = 0;
    IMG_HEADER_Tp pHeader = NULL;
   	unsigned char buffer[200];
	int fwSizeLimit = 0x800000;
   	firm_upgrade_status = 0;

	while(head_offset <  upload_len) {
		pHeader = (IMG_HEADER_Tp) &upload_data[head_offset];
		len = pHeader->len;
		// check header and checksum
		if (!memcmp(&upload_data[head_offset], FW_HEADER, SIGNATURE_LEN) ||
		    !memcmp(&upload_data[head_offset], FW_HEADER_WITH_ROOT, SIGNATURE_LEN))
		    flag = 1;
		else if (!memcmp(&upload_data[head_offset], WEB_HEADER, SIGNATURE_LEN))
		    flag = 2;
		else if (!memcmp(&upload_data[head_offset], ROOT_HEADER, SIGNATURE_LEN)){
		    flag = 3;
		}
		else {
		    //strcpy(buffer, "Invalid file format!");
			firm_upgrade_status |= SIGNATURE_ERROR;
			goto ret_upload;
		}

		if((head_offset + len + sizeof(IMG_HEADER_T)) > upload_len)
		{
		    printf("ERROR happend...image may be only part upload\n");
			goto ret_upload;
		}

		if(len > fwSizeLimit){ //len check 
		    sprintf(buffer, "Image len exceed max size 0x%x ! len=0x%x",fwSizeLimit, len);
			goto ret_upload;
		}
		if ((flag == 1) || (flag == 3)) {
		    if ( !fwChecksumOk(&upload_data[sizeof(IMG_HEADER_T)+head_offset], len)) {
				//sprintf(buffer, "Image checksum mismatched! len=0x%x, checksum=0x%x", len, *((unsigned short *)&upload_data[len-2]));
				if(flag == 1)
				    firm_upgrade_status |= LINUXCHECKSUM_ERROR;
				else if(flag == 3)
				    firm_upgrade_status |= ROOTCHECKSUM_ERROR;
				goto ret_upload;
			}
			if(flag == 1)
				printf("Linux CheckSum Complete,Get Image Right\n");
			else if(flag == 3)
				printf("Rootfs CheckSum Complete,Get Image Right\n");
		}
		else {
		    char *ptr = &upload_data[sizeof(IMG_HEADER_T)+head_offset];
			if ( !CHECKSUM_OK(ptr, len) ) {
				//sprintf(buffer, "Image checksum mismatched! len=0x%x", len);
				firm_upgrade_status |= WEBCHECKSUM_ERROR;
				goto ret_upload;
			}
			printf("WebPages Check Sum Complete,Get Image Right\n");
		}
		head_offset += len + sizeof(IMG_HEADER_T) ;
	}
	return 0;

ret_upload:	
	fprintf(stderr, "%s\n", buffer);	
	if(firm_upgrade_status & SIGNATURE_ERROR)
		printf("Signature ERROR...Invalid File Format\n");
	if(firm_upgrade_status & LINUXCHECKSUM_ERROR)
		printf("Linux checksum ERROR\n");
	if(firm_upgrade_status & ROOTCHECKSUM_ERROR)
		printf("Rootfs checksum ERROR\n");
	if(firm_upgrade_status & WEBCHECKSUM_ERROR)
		printf("Webpage checksum ERROR\n");
	firm_upgrade_status  = 0;
	return -1;	
}

static int firm_upgrade(char *data, int data_len)
{
	int fd,ret=0;
	unsigned char cmd2[2];
	ret=rtk_FirmwareCheck(data,data_len);
	if(ret < 0 ) {
		return ret;
	}
	//printf("Start upgrage Image\n");
	ret = rtk_FirmwareUpgrade(data,data_len);		
	//printf("end upgrage Image\n");
	return ret;
}

int main(int argc, char *argv[])
{
	char filename[MAX_FILENAME_LENGTH];
	char *data = NULL;
	int ret = 0, fd = 0, flen = 0;
	struct stat ffstat;

	memset(&ffstat,0,sizeof(struct stat));
	memset(filename,0,MAX_FILENAME_LENGTH);
	if(argc < 2)
	{
		printf("please enter filename\n");
		return 1;
	}

	sprintf(filename,"%s",argv[1]); 
	fd = open(filename, O_RDONLY);
	if (fd < 0)	{
		printf("Cannot Open file %s!\n", filename);
		return 1;
	}
	fstat(fd, &ffstat);
	flen = ffstat.st_size;
	printf("burn filename :%s, burn length:%d bytes\n",filename,flen);

	if((data = (char *)malloc(flen)) == NULL)
	{
		printf("Out Of Memory,Data buffer allocation failed!\n");
		return 1;
	}
	ret = read(fd, data, flen);
	if (ret != flen) {
		printf("Read File ERROR\n");
		free(data);
		return 1;
	}
	close(fd);

	ret = firm_upgrade(data, flen);
	if(ret < 0)
		printf("Rtk firmware update fail \n");
	else
        printf("Rtk firmware update ok \n");  	
	free(data);
	/*system("reboot");*/
	if( ret < 0) return 1;
	else return 0;
}
