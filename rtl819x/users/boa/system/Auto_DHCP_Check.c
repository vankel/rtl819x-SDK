#include "rtk_eventd.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>    
#include <sys/stat.h> 
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <sys/times.h>
#include <sys/select.h>

#include "apmib.h"
#include "mibtbl.h"
#include <sysconf.h>
#include "sys_utility.h"
#include <time.h> 


#define RTL8651_IOCTL_GETWANLINKSTATUS 2000
#define DHCPD_CONF_FILE "/var/udhcpd.conf"

static struct sockaddr_nl src_addr, dest_addr;
static struct nlmsghdr *nlh = NULL;
static struct iovec iov;
static int sock_fd=0;
static struct msghdr msg;

int pidfile_acquire(char *pidfile)
{
	int pid_fd;
	if (pidfile == NULL) 
	{
		return -1;
	}
	pid_fd = open(pidfile, O_CREAT | O_WRONLY, 0644);
	if (pid_fd < 0) 
	{
		printf("Unable to open pidfile %s\n",pidfile);
	}
	else 
	{
		lockf(pid_fd, F_LOCK, 0);
	}
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

void set_lan_dhcpc(char *iface)
{
	char script_file[100], deconfig_script[100], pid_file[100];
	char *strtmp=NULL;
	char tmp[32], Ip[32], Mask[32], Gateway[32];
	char cmdBuff[200];
#ifdef  HOME_GATEWAY
	int intValue=0;
#endif
	sprintf(script_file, "/usr/share/udhcpc/%s.sh", iface); /*script path*/
	sprintf(deconfig_script, "/usr/share/udhcpc/%s.deconfig", iface);/*deconfig script path*/
	sprintf(pid_file, "/etc/udhcpc/udhcpc-%s.pid", iface); /*pid path*/
	apmib_get( MIB_IP_ADDR,  (void *)tmp);
	strtmp= inet_ntoa(*((struct in_addr *)tmp));
	sprintf(Ip, "%s",strtmp);

	apmib_get( MIB_SUBNET_MASK,  (void *)tmp);
	strtmp= inet_ntoa(*((struct in_addr *)tmp));
	sprintf(Mask, "%s",strtmp);

	apmib_get( MIB_DEFAULT_GATEWAY,  (void *)tmp);
	strtmp= inet_ntoa(*((struct in_addr *)tmp));
	sprintf(Gateway, "%s",strtmp);

	Create_script(deconfig_script, iface, LAN_NETWORK, Ip, Mask, Gateway);

	sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s &", iface, pid_file, script_file);
	system(cmdBuff);

#if defined(CONFIG_APP_SIMPLE_CONFIG)
	system("echo 0 > /var/sc_ip_status");
#endif
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
	src_addr.nl_pid = getpid(); // self pid 
	src_addr.nl_groups = 0;     // not in mcast groups 
	bind(sock_fd, (struct sockaddr *)&src_addr, sizeof(src_addr));

	memset(&dest_addr, 0, sizeof(dest_addr));
	dest_addr.nl_family = AF_NETLINK;
	dest_addr.nl_pid = 0;    // For Linux Kernel 
	dest_addr.nl_groups = 0; // unicast 
	
	nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));	
	if(nlh==NULL)
		goto ERROR_EXIT;
	
	// fill the netlink message header 
	nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
	nlh->nlmsg_pid = getpid(); // self pid
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


	apmib_init();
	int opmode;              //router's mode
	int dhcp_type;           //DHCP status:client,server or disabled.if disabled,nothing will be done	
	if(!apmib_get(MIB_OP_MODE, (void *)&opmode))//get router's mode
	{		
		printf("Get router's mode,failed!\n");
		goto ERROR_EXIT;
	}
	if(!apmib_get(MIB_DHCP, (void *)&dhcp_type))//get DHCP's type
	{
		printf("Get router's DHCP type,failed!\n");
		goto ERROR_EXIT;
	}
	if(opmode!=BRIDGE_MODE || dhcp_type != DHCP_SERVER)//if router's mode is BRIDGE_MODE and DHCP is not disabled
	{
		goto ERROR_EXIT;
	}
	
    //avoid 60s waiting time when start up
	int tmepStatus = discovery_dhcp("br0");
	if(tmepStatus == 1) //if front-end router's DHCP is enabled ,this router's DHCP should be disabled
	{
		RunSystemCmd(NULL_FILE, "killall", "-9", "udhcpd", NULL_STR); 
		if(find_pid_by_name("udhcpc")==0)
		{
			set_lan_dhcpc("br0");
		}
	}
	else			   //if front-end router's DHCP is disabled and this router's DHCP is not running,this router's DHCP should be enabled
	{
		if(find_pid_by_name("udhcpd")==0)
		{
			RunSystemCmd(NULL_FILE, "udhcpd", DHCPD_CONF_FILE, NULL_STR); 
		}
		RunSystemCmd(NULL_FILE, "killall", "-9", "udhcpc", NULL_STR);
	}

	fd_set fds;              //file descriptor set
	struct timeval timeout={60,0};//60s time out
	while(1)                 //read message from kernel
	{
		FD_ZERO(&fds);       //clean up file descriptor set,else can't detect the change of file descriptor 
		FD_SET(sock_fd,&fds);//add file descriptor
		if(timeout.tv_sec == 0 && timeout.tv_usec== 0)
		{
			timeout.tv_sec = 60;
			timeout.tv_usec = 0;
		}
		switch(select(sock_fd+1,&fds,NULL,NULL,&timeout))
		{ 
			case -1: //select() function error  
				printf("select() function error!\n");
				break;
			case 0:  //time out  
			{
				int tmepStatus = discovery_dhcp("br0");
				if(tmepStatus == 1)	//if front-end router's DHCP is enabled ,this router's DHCP should be disabled
				{
					RunSystemCmd(NULL_FILE, "killall", "-9", "udhcpd", NULL_STR); 
					if(find_pid_by_name("udhcpc")==0)
					{
						set_lan_dhcpc("br0");
					}
				}
				else               //if front-end router's DHCP is disabled and this router's DHCP is not running,this router's DHCP should be enabled
				{
					if(find_pid_by_name("udhcpd")==0)
					{
						RunSystemCmd(NULL_FILE, "udhcpd", DHCPD_CONF_FILE, NULL_STR); 
					}
					RunSystemCmd(NULL_FILE, "killall", "-9", "udhcpc", NULL_STR);
				}
				break;
			}
			default: //capture wire plug-in event
			{
				memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
				recvmsg(sock_fd, &msg, 0);
				pEventdHdr=NLMSG_DATA(nlh);	
			
				if(pEventdHdr->eventID == WIRE_PLUG_ON)
				{
					int tmepStatus=discovery_dhcp("br0");
					if(tmepStatus==1) //if front-end router's DHCP is enabled ,this router's DHCP should be disabled
					{
						RunSystemCmd(NULL_FILE, "killall", "-9", "udhcpd", NULL_STR);
						if(find_pid_by_name("udhcpc")==0)
						{
							set_lan_dhcpc("br0");
						}
					}
					else              //if front-end router's DHCP is disabled and this router's DHCP is not running,this router's DHCP should be enabled
					{
						if(find_pid_by_name("udhcpd")==0)
						{
							RunSystemCmd(NULL_FILE, "udhcpd", DHCPD_CONF_FILE, NULL_STR);
						}
						RunSystemCmd(NULL_FILE, "killall", "-9", "udhcpc", NULL_STR);
					}
					//printf("WIRE_PLUG_ON: port:%s\n",pEventdHdr->data);
				}
			}
		}
	}
	
ERROR_EXIT:
	if(sock_fd>0)       // close netlink socket 
		close(sock_fd);	
	if(nlh)
		free(nlh);
	return 0;
}

