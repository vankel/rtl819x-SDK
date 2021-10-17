#include <stdio.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <sys/stat.h>

#define CHECK_TIME 2 //s
#define USB_STORAGE_MAP_FILE "/tmp/usb/mnt_map"
#define SAMBA_CONF_FILE "/var/samba/smb.conf"
#define USB_STORAGE_DIR "/var/tmp/usb"

typedef struct UsbDevListNode_
{
	struct UsbDevListNode_ * next;
	char name[64];
} UsbDevListNode,*pUsbDevListNode;

static UsbDevListNode usbDevListHeader={0};

void dump_usbDevList()
{
	pUsbDevListNode listPtr=usbDevListHeader.next;
	printf("\ndump usbdev list\n");
	while(listPtr!=NULL)
	{
		printf("%s->",listPtr->name);
		listPtr=listPtr->next;
	}
	printf("null\n");
}

//static pUsbDevListNode usbDevlistHeader_new;
/****************************************
	return 0: not change,not update
	return 1: changed and updated
****************************************/
static int updateUsbDevList()
{
	DIR   *dir=NULL;   
	struct   dirent   *ptr=NULL;   
	int   retVal=0;
	pUsbDevListNode listPtr=&usbDevListHeader;
	char fullPath[128]={0};
	struct stat statbuf={0};
	//printf("%s:%d\n",__FUNCTION__,__LINE__);
	//dump_usbDevList();

	dir   =opendir(USB_STORAGE_DIR);

	while((ptr=readdir(dir))!=NULL)
	{
		//printf("%s:%d read name=%s\n",__FUNCTION__,__LINE__,ptr->d_name);
		  if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)
		  	continue;
		  bzero(fullPath,sizeof(fullPath));
		  sprintf(fullPath,"%s/%s",USB_STORAGE_DIR,ptr->d_name);
		  if(lstat(fullPath,&statbuf)<0)
		  	continue;
		//  printf("%s:%d read name=%s\n",__FUNCTION__,__LINE__,ptr->d_name);

		  if(S_ISDIR(statbuf.st_mode)==0)
		  	continue;
		  if(strcmp(ptr->d_name,"sda")==0)
		  	continue;
		  //printf("%s:%d\n",__FUNCTION__,__LINE__);
		  if(listPtr->next==NULL)
		  {//have new usbDev
		  	retVal=1;
			if((listPtr->next=(pUsbDevListNode)malloc(sizeof(UsbDevListNode)))==NULL)
			{
				printf("malloc fail!\n");
				return -1;
			}
			
			bzero(listPtr->next,sizeof(UsbDevListNode));
			strcpy(listPtr->next->name,ptr->d_name);
			listPtr=listPtr->next;
			//printf("%s:%d add name %s\n",__FUNCTION__,__LINE__,ptr->d_name);
			//dump_usbDevList();
		  }
		  else
		  {
		  	if(strcmp(ptr->d_name,listPtr->next->name)==0)
		  	{//the same dir
				//printf("%s:%d name=%s\n",__FUNCTION__,__LINE__,listPtr->next->name);
		  		listPtr=listPtr->next;
		  	}
			else
			{//not the same,changed
				//printf("%s:%d\n",__FUNCTION__,__LINE__);
				retVal=1;
				bzero(listPtr->next->name,sizeof(listPtr->next->name));
				strcpy(listPtr->next->name,ptr->d_name);
		  		listPtr=listPtr->next;
			}
		  }
	}
	closedir(dir);
	if(listPtr->next!=NULL)
	{
		pUsbDevListNode listPtrTmp=listPtr->next;
		listPtr->next=NULL;
		listPtr=listPtrTmp;
		retVal=1;
		//printf("%s:%d\n",__FUNCTION__,__LINE__);
		while(listPtrTmp!=NULL)
		{
			listPtrTmp=listPtrTmp->next;
			//printf("%s:%d free name=%s\n",__FUNCTION__,__LINE__,listPtr->name);
			free(listPtr);
			listPtr=listPtrTmp;
		}
	}
	//if(retVal)
	//dump_usbDevList();
	return retVal;
}
static int reStartUsbStorageApps()
{	
	char buff[256]={0};
	int fd=0;
	pUsbDevListNode listPtr=usbDevListHeader.next;
	//printf("%s:%d\n",__FUNCTION__,__LINE__);

	//dump_usbDevList();

	if(listPtr==NULL) return -1;
	system("killall smbd 2> /dev/null");
	system("killall nmbd 2> /dev/null");
	system("rm /var/samba/smb.conf");
	system("cp /etc/samba/smb.conf /var/samba/smb.conf");
	fd=open(SAMBA_CONF_FILE,O_WRONLY|O_APPEND);
	if(fd<0)
	{
		printf("can't open %s\n",SAMBA_CONF_FILE);
		return -1;
	}
	//dump_usbDevList();

	while(listPtr!=NULL)
	{
		bzero(buff,sizeof(buff));
		sprintf(buff,"\n[%s]\n comment = Temporary file space %s \n path = /tmp/usb/%s\n",listPtr->name,listPtr->name,listPtr->name);
		if(write(fd,buff,strlen(buff))<0)
		{
			goto WRITE_FAIL;
		}
		bzero(buff,sizeof(buff));
		sprintf(buff," read only = no\n writeable = yes\n public = yes\n oplocks = no\n kernel oplocks = no\n create mask = 0777\n browseable = yes\n guest ok = yes\n directory mask = 0777\n");
		if(write(fd,buff,strlen(buff))<0)
		{
			goto WRITE_FAIL;
		}
		listPtr=listPtr->next;
	}
	close(fd);
	
	//printf("%s:%d\n",__FUNCTION__,__LINE__);

	//dump_usbDevList();
	sleep(1);
	system("smbd -D");
	//printf("%s:%d\n",__FUNCTION__,__LINE__);
	system("nmbd -D");
	system("/bin/up_usbStorage_apps");
	return 0;
WRITE_FAIL:
	printf("can't write %s\n",SAMBA_CONF_FILE);
	close(fd);
	return -1;
	
}
static int killUsbStorageApps()
{
	system("killall smbd 2> /dev/null");
	system("killall nmbd 2> /dev/null");
	system("/bin/kill_usbStorage_apps");
}
static void timelyCheck(int signo)
{
	if(updateUsbDevList()<=0)//fail or not change
		return;
	if(usbDevListHeader.next==NULL)//have no usbDev
	{
		killUsbStorageApps();
	}
	else
	{//have new udbDev
		reStartUsbStorageApps();
	}	 
}
int main(int argc,char*argv[])
{
	signal(SIGALRM, timelyCheck);
	while(1){		
		alarm(CHECK_TIME);
		pause();
	}
	return 0;
}
