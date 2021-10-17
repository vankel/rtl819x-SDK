/*
	Init to handle RA pocket
	should check M(manage) and O(others) field. 
		MO==1x get all config via dhcpv6,should generate dhcpv6 config and start dhcpv6c
		MO==01 get others config(not ip) via dhcpv6, should generate dhcpv6 config and start dhcpv6c
		MO==00 get no config via dhcpv6, should not start dhcpv6c

	also handle dhcp6c get options, when get
		prefix delegation's prefix, set the prefix to lan radvd, 
		dns, set dns
		...
*/
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <linux/netlink.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <fcntl.h>

#include "apmib.h"
#include "mibtbl.h"
#include "sysconf.h"
#include "sys_utility.h"

#include "ipv6_manage_inet.h"

#define BUFSIZE 1024

RA_INFO_ITEM_T last_Ra_data={0};

int restart_dhcp6c(RA_INFO_ITEM_Tp raData)
{
	FILE *fp = NULL;
	int fh;
	struct duid_t dhcp6c_duid;
	struct sockaddr hwaddr={0};
	uint16 len;
	int opmode,wisp_wan_id,wan_linkType,dhcpPdEnable,val,dhcpRapidCommitEnable,pid;
	char wan_interface[16]={0};
	char filename[64];
	char pidname[64];
	char tmpStr[256];
	int managed=0,others=0;

	managed=raData->icmp6_addrconf_managed;
	if(raData->slaacFail)//rfc6204 WAA-7 require
		managed=1;
	
	sprintf(pidname,DHCP6C_PID_FILE);
	if(isFileExist(pidname)){
		pid=getPid_fromFile(pidname);
		if(pid>0){
			sprintf(tmpStr, "%d", pid);
			RunSystemCmd(NULL_FILE, "kill", "-9", tmpStr, NULL_STR);
		}
		unlink(pidname);
	}	

	if(managed==0 && others==0)
		return;
	if ( !apmib_init()) {
		printf("Initialize AP MIB failed !\n");
		return -1;
	}

	apmib_get(MIB_OP_MODE,(void *)&opmode);
	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_wan_id);
	apmib_get(MIB_IPV6_LINK_TYPE,(void *)&wan_linkType);
	if(wan_linkType==IPV6_LINKTYPE_PPP){
		sprintf(wan_interface,"ppp0");
	}else
	{
		if(opmode==GATEWAY_MODE){
			sprintf(wan_interface, "%s", "eth1");
		}
		else if(opmode==WISP_MODE){				
			sprintf(wan_interface, "wlan%d", wisp_wan_id);
		}
	}
	
#ifdef TR181_SUPPORT
	{
		char value[64]={0};
		if(apmib_get(MIB_IPV6_DHCPC_IFACE,(void*)&value)==0)
		{
			printf("get MIB_IPV6_DHCPC_IFACE fail!\n");
			return -1;
		}
		if(value[0])
			strcpy(wan_interface,value);
	}
#endif
	/*for test use fixed duid of 0003000100e04c8196c9*/
	if(!isFileExist(DHCP6C_DUID_FILE)){
		/*create config file*/
		fp=fopen(DHCP6C_DUID_FILE,"w+");
		if(fp==NULL){
			fprintf(stderr, "Create %s file error!\n", DHCP6C_DUID_FILE);
			return;
		}
		
		dhcp6c_duid.duid_type=3;
		dhcp6c_duid.hw_type=1;
		if ( getInAddr(wan_interface, HW_ADDR_T, (void *)&hwaddr )==0)
		{
				fprintf(stderr, "Read hwaddr Error\n");
				return; 
		}
		memcpy(dhcp6c_duid.mac,hwaddr.sa_data,6);

		len=sizeof(dhcp6c_duid);
		if ((fwrite(&len, sizeof(len), 1, fp)) != 1) {
			fprintf(stderr, "write %s file error!\n", DHCP6C_DUID_FILE);
		}
		else if(fwrite(&dhcp6c_duid,sizeof(dhcp6c_duid),1,fp)!=1)
			fprintf(stderr, "write %s file error!\n", DHCP6C_DUID_FILE);
		
		fclose(fp);
	}

	
	if ( !apmib_get(MIB_IPV6_DHCP_PD_ENABLE,(void *)&dhcpPdEnable)){
				fprintf(stderr, "get mib %d error!\n", MIB_IPV6_DHCP_PD_ENABLE);
				return;
			}	
	
			
			sprintf(filename,DHCP6C_CONF_FILE);
			if(isFileExist(filename) == 0)
			/*create config file*/
				fh = open(filename, O_RDWR|O_CREAT|O_TRUNC, S_IRWXO|S_IRWXG);	
			else
				fh = open(filename, O_RDWR|O_TRUNC, S_IRWXO|S_IRWXG);	
		
			if (fh < 0){
				fprintf(stderr, "Create %s file error!\n", filename);
				return;
			}
	
		
			sprintf(tmpStr, "interface %s {\n",wan_interface);				
			write(fh, tmpStr, strlen(tmpStr));
			if(dhcpPdEnable){
				sprintf(tmpStr, "	send ia-pd %d;\n",100);
				write(fh, tmpStr, strlen(tmpStr));
			}
#ifdef TR181_SUPPORT
			if(!apmib_get(MIB_IPV6_DHCPC_REQUEST_ADDR,(void *)&val)){	
				fprintf(stderr, "get mib MIB_IPV6_DHCPC_REQUEST_ADDR error!\n");
				return; 		
			}
			if(val)
			{
#endif
				if(managed)
				{
					sprintf(tmpStr, "	send ia-na %d;\n",101);
					write(fh, tmpStr, strlen(tmpStr));
				}
#ifdef TR181_SUPPORT
			}
#endif
	
			if(!apmib_get(MIB_IPV6_DHCP_RAPID_COMMIT_ENABLE,(void *)&dhcpRapidCommitEnable)){	
				fprintf(stderr, "get mib %d error!\n", MIB_IPV6_LINK_TYPE);
				close(fh);
				return; 		
			}
			if(dhcpRapidCommitEnable){
				sprintf(tmpStr, "	send rapid-commit;\n");
				write(fh, tmpStr, strlen(tmpStr));	
			}
			sprintf(tmpStr, "	allow reconfigure-accept;\n");
				write(fh, tmpStr, strlen(tmpStr));
#ifndef TR181_SUPPORT
			/*dns*/
			sprintf(tmpStr, "	request domain-name-servers;\n");
			write(fh, tmpStr, strlen(tmpStr));
#else
			{
				int i=0;
				DHCPV6C_SENDOPT_T entryTmp={0};
				for(i=1;i<=IPV6_DHCPC_SENDOPT_NUM;i++)
				{
					*((char *)&entryTmp) = (char)i;
					if ( !apmib_get(MIB_IPV6_DHCPC_SENDOPT_TBL, (void *)&entryTmp)){
						printf("get MIB_IPV6_DHCPC_SENDOPT_TBL fail!\n");
						return;
					}
					if(entryTmp.enable)
					{
						bzero(tmpStr,sizeof(tmpStr));
						switch(entryTmp.tag)
						{
							case 21:
								sprintf(tmpStr, "	request sip-domain-name;\n");
								break;
							case 22:
								sprintf(tmpStr, "	request sip-server-address;\n");
								break;
							case 23:
								sprintf(tmpStr, "	request domain-name-servers;\n");
								break;
							case 27:
								sprintf(tmpStr, "	request nis-server-address;\n");
								break;
							case 28:
								sprintf(tmpStr, "	request nisp-server-address;\n");
								break;
							case 29:
								sprintf(tmpStr, "	request nis-domain-name;\n");
								break;
							case 30:
								sprintf(tmpStr, "	request nisp-domain-name;\n");
								break;
							case 33:
								sprintf(tmpStr, "	request bcmcs-domain-name;\n");
								break;
							case 34:
								sprintf(tmpStr, "	request bcmcs-server-address;\n");
								break;
							default:
								break;
						}
						if(tmpStr[0])
							write(fh, tmpStr, strlen(tmpStr));
					}
				}
			}
#endif
			sprintf(tmpStr, "	script \"/var/dhcp6cRcv.sh\";\n");
			write(fh, tmpStr, strlen(tmpStr));
			system("cp /bin/dhcp6cRcv.sh /var/dhcp6cRcv.sh");
			sprintf(tmpStr, "};\n\n");
			write(fh, tmpStr, strlen(tmpStr));
	
			if(dhcpPdEnable){
				sprintf(tmpStr, "id-assoc pd %d {\n",100);
				write(fh, tmpStr, strlen(tmpStr));	
#ifdef TR181_SUPPORT
				sprintf(tmpStr, "	suggest-t {\n");
				write(fh, tmpStr, strlen(tmpStr));
				if(!apmib_get(MIB_IPV6_DHCPC_SUGGESTEDT1,(void *)&val)){	
					fprintf(stderr, "get mib MIB_IPV6_DHCPC_SUGGESTEDT1 error!\n");
					return; 		
				}
				if(val >0)
				{
					sprintf(tmpStr, "		t1 %d;\n", val);
					write(fh, tmpStr, strlen(tmpStr));
				}
				if(!apmib_get(MIB_IPV6_DHCPC_SUGGESTEDT2,(void *)&val)){	
					fprintf(stderr, "get mib MIB_IPV6_DHCPC_SUGGESTEDT2 error!\n");
					return; 		
				}
				if(val > 0)
				{
					sprintf(tmpStr, "		t2 %d;\n", val);
					write(fh, tmpStr, strlen(tmpStr));
				}
				sprintf(tmpStr, "	};\n");
				write(fh, tmpStr, strlen(tmpStr));
#endif
				sprintf(tmpStr, "		prefix-interface br0 {\n");
				write(fh, tmpStr, strlen(tmpStr));				
				sprintf(tmpStr, "			sla-id 0;\n");
				write(fh, tmpStr, strlen(tmpStr));
				sprintf(tmpStr, "			sla-len 0;\n");
				write(fh, tmpStr, strlen(tmpStr));
				sprintf(tmpStr, "		};\n");
				write(fh, tmpStr, strlen(tmpStr));
				sprintf(tmpStr, "};\n\n");
				write(fh, tmpStr, strlen(tmpStr));	
			}						

			if(managed)
			{
				/*ia-na*/
				sprintf(tmpStr, "id-assoc na %d {\n",101);
				write(fh, tmpStr, strlen(tmpStr));
#ifdef TR181_SUPPORT
				sprintf(tmpStr, "	suggest-t {\n");
				write(fh, tmpStr, strlen(tmpStr));
		
				if(!apmib_get(MIB_IPV6_DHCPC_SUGGESTEDT1,(void *)&val)){	
					fprintf(stderr, "get mib MIB_IPV6_DHCPC_SUGGESTEDT1 error!\n");
					return; 		
				}
				if(val >0)
				{
					sprintf(tmpStr, "		t1 %d;\n", val);
					write(fh, tmpStr, strlen(tmpStr));
				}
				if(!apmib_get(MIB_IPV6_DHCPC_SUGGESTEDT2,(void *)&val)){	
					fprintf(stderr, "get mib MIB_IPV6_DHCPC_SUGGESTEDT2 error!\n");
					return; 		
				}
				if(val > 0)
				{
					sprintf(tmpStr, "		t2 %d;\n", val);
					write(fh, tmpStr, strlen(tmpStr));
				}
				sprintf(tmpStr, "	};\n");
				write(fh, tmpStr, strlen(tmpStr));
#endif
				sprintf(tmpStr, "};\n\n");
				write(fh, tmpStr, strlen(tmpStr));	
			}

			if(dhcpPdEnable||managed)
			{
				sprintf(tmpStr, "authentication reconfigure {\n\
					protocol reconfig;\n\
					algorithm hmac-md5;\n\
					rdm monocounter;\n\
					};");
				write(fh, tmpStr, strlen(tmpStr));
			}
			close(fh);
		
	
			
			/*start daemon*/
			sprintf(tmpStr, "dhcp6c -c %s -p %s %s ", DHCP6C_CONF_FILE,DHCP6C_PID_FILE,wan_interface);
			/*Use system() instead of RunSystemCmd() to avoid stderr closing, 
			process itself will redirect stderr when it wants to run as deamon() */
			system(tmpStr);
			//printf("%s\n",tmpStr);
		return;
}
int handle_ra(struct msghdr * pMsg)
{
	RA_INFO_ITEM_T recv_data;
	memcpy(&recv_data,NLMSG_DATA(pMsg->msg_iov->iov_base),sizeof(recv_data));
	printf("%s:%d icmp6_addrconf_managed=%d icmp6_addrconf_other=%d slaacFail=%d\n",__FUNCTION__,__LINE__,recv_data.icmp6_addrconf_managed,recv_data.icmp6_addrconf_other,recv_data.slaacFail);
	if(last_Ra_data.icmp6_addrconf_managed==recv_data.icmp6_addrconf_managed
		&& last_Ra_data.icmp6_addrconf_other==recv_data.icmp6_addrconf_other
		&& last_Ra_data.slaacFail==recv_data.slaacFail)
		{//the same as last, do nothing
			return 0;
		}
	//It changed, save and apply the change
	memcpy(&last_Ra_data,&recv_data,sizeof(recv_data));

	restart_dhcp6c(&recv_data);
	
}
int get_handle_ra(int enable)
{
//use netlink to fit kernel's RA receive
	struct sockaddr_nl src_addr, dest_addr;
	struct nlmsghdr *nlh = NULL;
	struct iovec iov;
	int sock_fd;
	ssize_t n;
	struct msghdr msg;
	RA_INFO_ITEM_T send_data={0};
	int pid=0;
	char tmpbuf[16]={0};

	send_data.enable=enable;	
	send_data.pid=getpid();
	
//init
        sock_fd = socket(PF_NETLINK, SOCK_RAW, NETLINK_RTK_IPV6_RA);
        memset(&msg, 0, sizeof(msg));
        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid(); /* self pid */
        src_addr.nl_groups = 0; /* not in mcast groups */
        bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
        memset(&dest_addr, 0, sizeof(dest_addr));
        dest_addr.nl_family = AF_NETLINK;
        dest_addr.nl_pid = 0; /* For Linux Kernel */
        dest_addr.nl_groups = 0; /* unicast */

//prepare data
        nlh=(struct nlmsghdr *)malloc(NLMSG_SPACE(BUFSIZE));
/* Fill the netlink message header */
        nlh->nlmsg_len = NLMSG_SPACE(BUFSIZE);
        nlh->nlmsg_pid = getpid(); /* self pid */
        nlh->nlmsg_flags = 0;
/* Fill in the netlink message payload */
		memcpy(NLMSG_DATA(nlh), &send_data,sizeof(send_data));
		iov.iov_base = (void *)nlh;
		iov.iov_len = nlh->nlmsg_len;
		msg.msg_name = (void *)&dest_addr;
		msg.msg_namelen = sizeof(dest_addr);
		msg.msg_iov = &iov;
		msg.msg_iovlen = 1;

	sendmsg(sock_fd, &msg, 0);
	
	// kill enable daemon if exist
	if(isFileExist(IPV6_MANG_PID_FILE)) {
		pid=getPid_fromFile(IPV6_MANG_PID_FILE);
		if(pid){
			sprintf(tmpbuf, "%d", pid);
			RunSystemCmd(NULL_FILE, "kill", "-9", tmpbuf, NULL_STR);							
		}
		unlink(IPV6_MANG_PID_FILE);
	}
	if(send_data.enable==0)
	{
		
		usleep(100);
		close(sock_fd);		

		return 1;
	}
	
	setPid_toFile(IPV6_MANG_PID_FILE);
	for(;;)
	{
		//bzero(serverSock.sa_data,sizeof(serverSock.sa_data));
		n=recvmsg(sock_fd,&msg,0);
		if(n<0)
		{
			if(errno==EINTR)
				continue;
			else{
				fprintf(stderr,"recvmsg fail! n=%d\n",n);
				return -1;
			}
		}
		if(handle_ra(&msg)<0)
			fprintf(stderr,"handle_ra fail!\n");
		
	}
	close(sock_fd);
	return 0;
}
int main(int argc, char *argv[])
{
	if(argc!=2)
	{
		fprintf(stderr,"invalid input!\n");
		return -1;
	}
	if(strcmp(argv[1],"start")==0)
	{
		get_handle_ra(1);	
	}
	else if(strcmp(argv[1],"stop")==0)
	{
		get_handle_ra(0);	
	}
	
	return 0;
}
