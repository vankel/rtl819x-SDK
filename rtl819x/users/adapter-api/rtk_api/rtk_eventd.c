#include "rtk_eventd.h"

static struct sockaddr_nl src_addr, dest_addr;
static struct nlmsghdr *nlh = NULL;
static struct iovec iov;
static int sock_fd=0;
static struct msghdr msg;

int pidfile_acquire(char *pidfile)
{
	int pid_fd;
	if (pidfile == NULL) 
		return -1;

	pid_fd = open(pidfile, O_CREAT | O_WRONLY, 0644);
	if (pid_fd < 0) 
		printf("Unable to open pidfile %s\n",pidfile);
	else 
		lockf(pid_fd, F_LOCK, 0);
	
	return pid_fd;
}

void pidfile_write_release(int pid_fd)
{
	FILE *out;

	if (pid_fd < 0) 
		return;

	if ((out = fdopen(pid_fd, "w")) != NULL) 
	{
		fprintf(out, "%d\n", getpid());
		fclose(out);
	}
	lockf(pid_fd, F_UNLCK, 0);
	close(pid_fd);
}

int main() 
{	
	if (daemon(0, 1) == -1) 
	{
		perror("rtk_eventd fork error");
		goto ERROR_EXIT;
	}
	
	int pid_fd;
	pid_fd = pidfile_acquire(RTK_EVENTD_PID_FILE);
	pidfile_write_release(pid_fd);
	
	char msgBuf[256]={0};
	rtkEventHdr *pEventdHdr=msgBuf;

	pEventdHdr->eventID=0;
	strcpy(pEventdHdr->name, "br0");
	strcpy(pEventdHdr->data, "RTK EVENTD START!");	
	
	sock_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_RTK_EVENTD);
	if(sock_fd<0)
	{
		printf("%s:%d ##create netlink socket fail!\n",__FUNCTION__,__LINE__);
		goto ERROR_EXIT;
	}
	memset(&src_addr, 0, sizeof(src_addr));
	src_addr.nl_family = AF_NETLINK;
	src_addr.nl_pid = getpid(); /* self pid */
	src_addr.nl_groups = 0; /* not in mcast groups */
	bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0; /* For Linux Kernel */
	dest_addr.nl_groups = 0; /* unicast */
	
	nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));	
	if(nlh==NULL)
		goto ERROR_EXIT;
	
	/* Fill the netlink message header */
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid(); /* self pid */
	nlh->nlmsg_flags = 0;	
	
	memcpy(NLMSG_DATA(nlh), msgBuf, strlen(pEventdHdr->data)+RTK_EVENTD_HDR_LEN);

	memset(&msg, 0, sizeof(msg));
	memset(&iov, 0, sizeof(iov));
	
	iov.iov_base = (void *)nlh;
	iov.iov_len = nlh->nlmsg_len;
	msg.msg_name = (void *)&dest_addr;
	msg.msg_namelen = sizeof(dest_addr);
	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;
	sendmsg(sock_fd, &msg, 0);

	/* Read message from kernel */
	while(1)
	{
		memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
		recvmsg(sock_fd, &msg, 0);
		
		pEventdHdr=NLMSG_DATA(nlh);			
		switch(pEventdHdr->eventID)
		{
			case WIFI_USER_PASSWD_ERROR:
				printf("WIFI_USER_PASSWD_ERROR: data=%s\n", pEventdHdr->data);
				break;
				
			case WIRE_PLUG_OFF:
				printf("WIRE_PLUG_OFF: port:%s\n",pEventdHdr->data);
				break;

			case WIRE_PLUG_ON:
				printf("WIRE_PLUG_ON: port:%s\n",pEventdHdr->data);

				break;
				
			case SYSTEM_RESET:

				break;
				
			case NTP_UPDATE_SUCCESS:

				break;
				
			case START_WIFI_SERVICE:
				printf("START_WIFI_SERVICE: data=%s\n", pEventdHdr->data);
				break;
				
			case START_WAN_SUCCESS:

				break;

			case PPPOE_DIAL_SUCCESS:
				printf(" Received message: eventdID=%d\n",pEventdHdr->eventID);
				printf(" Received message: data=%s\n",pEventdHdr->data);
				break;

			case START_DHCP_SERVICE:				
				printf(" Received message: eventdID=%d\n",pEventdHdr->eventID);
				printf(" Received message: data=%s\n",pEventdHdr->data);
				break;
				
			case RESTORE_AP_INIT_STATE:

				break;						
		}		
		sleep(1);
	}
ERROR_EXIT:
	/* Close Netlink Socket */
	if(sock_fd>0)
		close(sock_fd);	
	if(nlh)
		free(nlh);
	return 0;
}



























