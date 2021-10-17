#include "rtk_eventd.h"
#include "rtk_api.h"
#include "rtk_adapter.h"
#define UPNP_FILE "/tmp/upnp_info"

int getPidFromFile(char *file_name)
{
	FILE *fp;
	char *pidfile = file_name;
	int result = -1;
	
	fp= fopen(pidfile, "r");
	if (!fp) {
        	printf("can not open:%s\n", file_name);
		return -1;
   	}
	fscanf(fp,"%d",&result);
	fclose(fp);
	
	return result;
}

void sendMsgToRtkEventd(int evnt_id, char *ifname, char *data, int data_len)
{
	//printf("%s:%d####data=%s data_len=%d\n",__FUNCTION__,__LINE__,data,data_len);
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	int sock_fd=0;
	struct msghdr msg;	
	int rtk_eventd_pid=0;

	char *msgBuf=NULL;
	rtkEventHdr *pEventdHdr=NULL;
	
	if(data_len>MAX_PAYLOAD-RTK_EVENTD_HDR_LEN)
	{
		printf("%s:%d##date len is too long!\n",__FUNCTION__,__LINE__);
		return;
	}	
	
	rtk_eventd_pid=getPidFromFile(RTK_EVENTD_PID_FILE);
	if(rtk_eventd_pid<1)
		goto ERROR_EXIT;
	
	sock_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_RTK_EVENTD);
	if(sock_fd<0)
	{
		printf("%s:%d ##create netlink socket fail!\n",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}

	msgBuf = (char *)malloc(data_len+RTK_EVENTD_HDR_LEN+1);
	if (msgBuf==NULL)
	{
		printf("%s:%d##allocate memory fail!\n",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}	
	memset(msgBuf, 0, sizeof(msgBuf));
	pEventdHdr=(rtkEventHdr *)msgBuf;

	pEventdHdr->eventID=evnt_id;
	if(ifname!=NULL)
		strcpy(pEventdHdr->name, ifname);
	
	strcpy(pEventdHdr->data, data);	
	
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid(); /* self pid */
	src_addr.nl_groups = 0; /* not in mcast groups */
	bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

	//printf("%s:%d####src_pid=%d\n",__FUNCTION__,__LINE__,src_addr.nl_pid);
	
	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = rtk_eventd_pid; //0; /* For Linux Kernel */
	dest_addr.nl_groups = 0; /* unicast */
	
	nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));		
	if(nlh==NULL)
		goto ERROR_EXIT;
	
	/* Fill the netlink message header */
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid(); /* self pid */
	nlh->nlmsg_flags = 0;	
	
	memcpy(NLMSG_DATA(nlh), msgBuf, data_len+RTK_EVENTD_HDR_LEN+1);

	memset(&msg, 0, sizeof(msg));
	memset(&iov, 0, sizeof(iov));
	
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	//printf("%s:%d####\n",__FUNCTION__,__LINE__);
	if(sendmsg(sock_fd, &msg, 0)<0)
	{
		perror("sendmsg");
		printf("%s:%d##sendmsg fails!\n",__FUNCTION__,__LINE__);
	}

ERROR_EXIT:
	if(sock_fd>0)
		close(sock_fd);	
	if(nlh)
		free(nlh);
	return;
}

static int getWanIf(char * wanIf)
{
	int wisp_id=0, wan_mode=0, opmode=0;
	
#ifdef CONFIG_SMART_REPEATER
	int rptEnabled=0;
#endif
	if(!wanIf){
		return RTK_FAILED;
	}
	apmib_get(MIB_OP_MODE,(void *)&opmode);
	switch(opmode)
	{
		case GATEWAY_MODE:
				sprintf(wanIf,"%s","eth1");
			break;
		case WISP_MODE:
				apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_id);				
				sprintf(wanIf,"wlan%d",wisp_id);
#ifdef CONFIG_SMART_REPEATER
				if(wisp_id == 0)
					apmib_get(MIB_REPEATER_ENABLED1, (void *)&rptEnabled);
				else if(wisp_id == 1)
					apmib_get(MIB_REPEATER_ENABLED2, (void *)&rptEnabled);
				if(rptEnabled)
				{
					sprintf(wanIf,"wlan%d-vxd",wisp_id);
				}
#endif				
			break;
		default:
			return RTK_FAILED;
	}
	return RTK_SUCCESS;
}

/**************************************************
* @NAME:
* 	rtk_start_upnp_igd
* 
* @PARAMETERS:
* 	@Input
* 	@Output
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	start upnp igd function
*
***************************************************/
#define BACKUP_IGD_RULES_FILENAME "/tmp/miniigd_rules.bak"
int rtk_start_upnp_igd()
{
    int op_mode,wan_type;
    char wanif[16],lanif[16];
    char cmd_buf[200];
    apmib_get(MIB_OP_MODE,(void *)&op_mode);
    apmib_get(MIB_DHCP,(void *)&wan_type);
    system("killall -15 miniigd > /dev/null 2>&1");
    system("route del -net 239.255.255.250 netmask 255.255.255.255 br0 > /dev/null 2>&1");
    system("route add -net 239.255.255.250 netmask 255.255.255.255 br0 > /dev/null 2>&1");
    if(op_mode==WISP_MODE)
    {
        if(getWanIf(wanif) !=RTK_SUCCESS)
            return RTK_FAILED;
        if(!strncmp(wanif,"wlan",4))
            sprintf(cmd_buf,"miniigd -e %d -i br0 -w %s  > /dev/null 2>&1",wan_type,wanif);
        else
            return RTK_FAILED;
    }
    else
    {
        sprintf(cmd_buf,"miniigd -e %d -i br0  > /dev/null 2>&1",wan_type);
    }
    system(cmd_buf);
    return RTK_SUCCESS;
}
/**************************************************
* @NAME:
* 	rtk_stop_upnp_igd
* 
* @PARAMETERS:
* 	@Input
* 	@Output
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	stop upnp igd function
*
***************************************************/

int rtk_stop_upnp_igd()
{
    system("killall -15 miniigd > /dev/null 2>&1");
    system("route del -net 239.255.255.250 netmask 255.255.255.255 br0 > /dev/null 2>&1");
	system("rm -f /tmp/miniigd_rules.bak > /dev/null 2>&1");
	system("rm -rf  /tmp/upnp_info > /dev/null 2>&1");
    return RTK_SUCCESS;
}

/**************************************************
* @NAME:
* 	rtk_set_upnp_enable
* 
* @PARAMETERS:
* 	@Input
* 		enable: the flag of upnp enable or disable
*			0-diable, 1-enable 
* 	@Output
*		none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Set upnp enable or disable
*
***************************************************/

int rtk_set_upnp_enable(int enable)
{
	int intValue,ret = RTK_SUCCESS;
	intValue = enable;
	if(!apmib_set(MIB_UPNP_ENABLED,(void *)&intValue))
	{
		ret = RTK_FAILED;
		return ret;
	}

	switch(enable)
	{
		case 0:			/**diable upnp function ***/			
			ret = rtk_stop_upnp_igd();
			break;
		case 1:			/**enable upnp function ***/			
			ret = rtk_start_upnp_igd();
			break;
		default:
			break;
	}
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_upnp_enable
* 
* @PARAMETERS:
* 	@Input
* 		none
* 	@Output
*		enable: the flag of upnp enable or disable
*			0-diable, 1-enable 
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get the enable flag of upnp 
*
***************************************************/

int rtk_get_upnp_enable(int *enable)
{
	int ret = RTK_SUCCESS;
	if(!apmib_get(MIB_UPNP_ENABLED,(void *)enable))
		ret = RTK_FAILED;
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_upnp_list
* 
* @PARAMETERS:
* 	@Input
* 		max_num: the maximum number upnp port mapping entry to get
* 	@Output
*		info: the storage of upnp port mapping list
*		n: the actual number of upnp port mapping entry in the list
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	Get upnp IGD port mapping list
*
***************************************************/

int rtk_get_upnp_list(RTK_UPNP_MAPPING_INFO_T *info, unsigned int* n,unsigned int max_num)
{
	FILE* fp;
	struct stat status;
	RTK_UPNP_MAPPING_INFO_Tp upnp;
	int num = 0,ret = RTK_SUCCESS;
	char *lineptr,*str;
	char line[200];
	 //(int_ip,ext_port,int_port,protocol,remote_host,bool_enabled,desc) 
	if(stat(UPNP_FILE,&status) == 0){
		fp = fopen(UPNP_FILE,"r");
		if(NULL == fp)
			goto err_end;
		while ( fgets(line, 200, fp))
		{
			if(strlen(line) != 0 && num < max_num)
			{
				lineptr = line;
				upnp = &info[num];
				str = strsep(&lineptr,",");
				upnp->in_ip = inet_addr(str);
				if(lineptr)
				{
					upnp->eport = atoi(strsep(&lineptr,","));
					if(lineptr)
					{
						upnp->iport = atoi(strsep(&lineptr,","));
						if(lineptr)
						{		
							strcpy(upnp->proto,strsep(&lineptr,","));
							if(lineptr)
							{
								str = strsep(&lineptr,",");//skip remot host
								if(lineptr)
								{								
									upnp->enabled = atoi(strsep(&lineptr,","));
									if(lineptr)
									{
										int len = 0 ;
										str = strsep(&lineptr,",");
										len = (strlen(str)-1)>=63 ? 63:(strlen(str)-1);
										//strncpy(upnp->desc,str,63);
										strncpy(upnp->desc,str,len);
										upnp->desc[len] = '\0';
									}
								}
							}
						}
					}
				}
			}
			num ++;
		}
		fclose(fp);
		*n = num;
		
	}
	else{
		*n = 0;
	}
	return ret;
err_end:
	if(fp)
		fclose(fp);
	return RTK_FAILED;
}

int rtk_get_mib_value(int id, void *value)
{
	int ret=RTK_SUCCESS;
	
	if(!apmib_get(id, value))
		ret=RTK_FAILED;
	
	return RTK_SUCCESS;
}

int rtk_set_mib_value(int id, void *value)
{
	int ret=RTK_SUCCESS;
	
	if(!apmib_set(id, value))
		ret=RTK_FAILED;
	
	return RTK_SUCCESS;
}

