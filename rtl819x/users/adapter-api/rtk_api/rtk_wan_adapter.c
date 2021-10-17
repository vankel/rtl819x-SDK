#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/wait.h>

#include "rtk_api.h"
#include "rtk_adapter.h"


int DoCmd(char *const argv[], char *file)
{    
	pid_t pid;
	int status;
	int fd;
	char _msg[30];
	switch (pid = fork()) {
			case -1:	/* error */
				perror("fork");
				return errno;
			case 0:	/* child */
				
				signal(SIGINT, SIG_IGN);
				if(file){
					if((fd = open(file, O_RDWR | O_CREAT))==-1){ /*open the file */
						sprintf(_msg, "open %s", file); 
  						perror(_msg);
  						exit(errno);
					}
					dup2(fd,STDOUT_FILENO); /*copy the file descriptor fd into standard output*/
					dup2(fd,STDERR_FILENO); /* same, for the standard error */
					close(fd); /* close the file descriptor as we don't need it more  */
				}else{
			#ifndef SYS_DEBUG		
					close(2); //do not output error messages
			#endif	
				}
				setenv("PATH", "/sbin:/bin:/usr/sbin:/usr/bin", 1);
				execvp(argv[0], argv);
				perror(argv[0]);
				exit(errno);
			default:	/* parent */
			{
				
				waitpid(pid, &status, 0);
			#ifdef SYS_DEBUG	
				if(status != 0)
					printf("parent got child's status:%d, cmd=%s %s %s\n", status, argv[0], argv[1], argv[2]);
			#endif		
				if (WIFEXITED(status)){
			#ifdef SYS_DEBUG	
					printf("parent will return :%d\n", WEXITSTATUS(status));
			#endif		
					return WEXITSTATUS(status);
				}else{
					
					return status;
				}
			}
	}
}
int RunSystemCmd(char *filepath, ...)
{
	va_list argp;
	char *argv[24]={0};
	int status;
	char *para;
	int argno = 0;
	va_start(argp, filepath);
    #ifdef DISPLAY_CMD
	printf("\n"); 
    #endif
	while (1){ 
		para = va_arg( argp, char*);
		if ( strcmp(para, "") == 0 )
			break;
		argv[argno] = para;
        #ifdef DISPLAY_CMD
		printf(" %s ", para); 
        #endif
		argno++;
	} 
    #ifdef DISPLAY_CMD    
	printf("\n");     
    #endif
	argv[argno+1] = NULL;
	status = DoCmd(argv, filepath);
	va_end(argp);
	return status;
}

static int isValidName(char *str)
{
	int i, len=strlen(str);

	for (i=0; i<len; i++) {
		if (str[i] == ' ' || str[i] == '"' || str[i] == '\x27' || str[i] == '\x5c' || str[i] == '$')
			return 0;
	}
	return 1;
}

/*WAN API*/

/*
  *@ name: rtk_set_wan_type
  *@ input
  	type,unsigned int, wan type
  	0:static ip
  	1:dhcp client
  	3:pppoe
  	4:pptp
  	6:l2tp
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function set the wan type
  *
  */

int rtk_set_wan_type(unsigned int type)
{
	if(type!=0&&type!=1&&type!=3&&type!=4&&type!=6)
	{
		printf("Invalid wan type!\n");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_WAN_DHCP, (void *)&type))
	{
		printf("Set WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	return RTK_SUCCESS;
}


/*SET*/
/*
  *@ name: rtk_set_wan_static
  *@ input
  	pstatic_config, struct rtk_static_config *, wan ip and ipmask and default gw ip
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function set wan using static ip connection. the wan ip , mask,
  	default gw ip should be specified by rtk_static_config
  *
  */

int rtk_set_wan_static(struct rtk_static_config *pstatic_config)
{
	struct in_addr addr;
	DHCP_T dhcp;
	if(pstatic_config == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
	dhcp = DHCP_DISABLED;
	if(!apmib_set(MIB_WAN_DHCP, (void *)&dhcp))
	{
		printf("Set DHCP error!\n");
		return RTK_FAILED;
	}
	addr.s_addr = pstatic_config->ip;
	if(!apmib_set(MIB_WAN_IP_ADDR, (void *)&addr))
	{
		printf("Set wan ip fail!\n");
		return RTK_FAILED;
	}
	addr.s_addr = pstatic_config->mask;
	if(!apmib_set(MIB_WAN_SUBNET_MASK, (void *)&addr))
	{
		printf("Set wan subnet mask fail!\n");
		return RTK_FAILED;
	}
	addr.s_addr = pstatic_config->gw;
	if(!apmib_set(MIB_WAN_DEFAULT_GATEWAY, (void *)&addr))
	{
		printf("Set wan default gateway fail!\n");
		return RTK_FAILED;
	}
	return RTK_SUCCESS;
}


/*
  *@ name: rtk_set_wan_dhcp
  *@ input
  	pdhcp_config, struct rtk_dhcp_config *,  the hostname using
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function set wan using chcpc to get the ip. pdhcp_config specific the 
  	host name report to dhcp server
  *
  */

int rtk_set_wan_dhcp(struct rtk_dhcp_config *pdhcp_config)
{
	DHCP_T dhcp;
	if(pdhcp_config == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
	dhcp = DHCP_CLIENT;
	if(!apmib_set(MIB_WAN_DHCP, (void *)&dhcp))
	{
		printf("Set DHCP error!\n");
		return RTK_FAILED;
	}
	if(pdhcp_config->host_name)
	{
		if(!isValidName(pdhcp_config->host_name))
		{
			printf("Invalid host name!\n");
			return RTK_FAILED;
		}
		if(!apmib_set(MIB_HOST_NAME,(void *)pdhcp_config->host_name))
		{
			printf("Set MIB_HOST_NAME MIB error!\n");
			return RTK_FAILED;
		}
	}
	else
	{
		if(!apmib_set(MIB_HOST_NAME,(void *)""))
		{
			printf("Set MIB_HOST_NAME MIB error!");
			return RTK_FAILED;
		}
	}
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_set_wan_pppoe
  *@ input
  	ppppoe_config, struct rtk_pppoe_config *,  ppp configuration incluing 
  	usname/password/connection type/idle time  and ac name
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function set wan using pppoe type. ppppoe_config specific the 
  	ppp config and ac name.
  	username and password is necessary for ppp connection.
  	connection type : 0 continues 1 mannual  2 dial on demand
  	idle timeout. the idle time > idle timeout the ppp will disconnect
  	ac name is for selection of different server , only the server with same
  	ac name can be connected
  *
  */
int rtk_set_wan_pppoe(struct rtk_pppoe_config *pppoe_config)
{
	DHCP_T dhcp;
	if(pppoe_config == NULL || 
	pppoe_config->ppp_config.user_name == NULL ||
	pppoe_config->ppp_config.passwd == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
	if(pppoe_config->ppp_config.connection_type > 2)
	{
		printf("Invalid connection type!\n");
		return RTK_FAILED;
	}
	dhcp = PPPOE;
	if(!apmib_set(MIB_WAN_DHCP, (void *)&dhcp))
	{
		printf("Set DHCP error!\n");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_PPP_USER_NAME,(void *)pppoe_config->ppp_config.user_name)) 
	{
		printf("Set PPP user name MIB error!\n");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_PPP_PASSWORD, (void *)pppoe_config->ppp_config.passwd))
	{
		printf("Set PPP user password MIB error!");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_PPP_CONNECT_TYPE, (void *)&pppoe_config->ppp_config.connection_type))
	{
		printf("Set PPP type MIB error!");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_PPP_IDLE_TIME, (void *)&pppoe_config->ppp_config.idle_time))
	{
		printf("Set PPP idle time MIB error!");
		return RTK_FAILED;
	}
	if(pppoe_config->ac_name != NULL)
	{
		if(!apmib_set(MIB_PPP_SERVICE_NAME, (void *)pppoe_config->ac_name))
		{
			printf("Set PPP serice name MIB error!");
			return RTK_FAILED;
		}
	}
	else
	{
		if(!apmib_set(MIB_PPP_SERVICE_NAME, (void *)""))
		{
			printf("Set PPP serice name MIB error!");
			return RTK_FAILED;
		}
	}
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_set_wan_l2tp
  *@ input
  	pl2tp_config, struct rtk_l2tp_config *,  ppp configuration incluing 
  	usname/password/connection type/idle time 
  	for l2pt , the local ip and remote sever ip should be specified.
 
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function set wan using l2tp type. pl2tp_config specific the 
  	ppp config and local ip and remote server ip.
  	username and password is necessary for ppp connection.
  	connection type : 0 continues 1 mannual  2 dial on demand
  	idle timeout. the idle time > idle timeout the ppp will disconnect
	there are serveral ways to decide the local ip and remote server ip
	for local ip: static ,dhcp
	for remote ip: static , ip from domain name 
  *
  */

int rtk_set_wan_l2tp(struct rtk_l2tp_config *pl2tp_config)
{
	WAN_IP_TYPE_T wanIpType;
	PPP_CONNECT_TYPE_T conn_type;
	struct in_addr addr;
	DHCP_T dhcp;
	dhcp = L2TP;
	if(!apmib_set(MIB_WAN_DHCP, (void *)&dhcp))
	{
		printf("Set DHCP error!");
		return RTK_FAILED;
	}
	if(pl2tp_config->local_ip_selection != 0 && pl2tp_config->local_ip_selection != 1)
	{
		printf("Invalid local ip selection value!\n");
		return RTK_FAILED;
	}
	wanIpType = (WAN_IP_TYPE_T)pl2tp_config->local_ip_selection;
	if (!apmib_set(MIB_L2TP_WAN_IP_DYNAMIC, (void *)&wanIpType))
	{
		printf("Set MIB_L2TP_WAN_IP_DYNAMIC error!\n");
		return RTK_FAILED;
	}
	if(wanIpType == STATIC_IP)//set static ip
	{
		addr.s_addr = pl2tp_config->static_config.ip;
		if (!apmib_set(MIB_L2TP_IP_ADDR, (void *)&addr))
		{
			printf("Set l2tp IP-address error!\n");
			return RTK_FAILED;
		}
		addr.s_addr = pl2tp_config->static_config.mask;
		if(!apmib_set(MIB_L2TP_SUBNET_MASK,(void *)&addr))
		{
			printf("Set l2tp subnet-mask error!\n");
			return RTK_FAILED;
		}
		addr.s_addr = pl2tp_config->static_config.gw;
		if(!apmib_set(MIB_L2TP_DEFAULT_GW,(void *)&addr))
		{
			printf("Set l2tp default-gateway error!\n");
			return RTK_FAILED;
		}
	}
	if(pl2tp_config->server_info_flag == 1)//by domain name
	{
		if(!apmib_set(MIB_L2TP_GET_SERV_BY_DOMAIN,(void*)&pl2tp_config->server_info_flag))
		{
			printf("Set l2tp get server by domain error!");
			return RTK_FAILED;
		}
		if(pl2tp_config->l2tp_server_info.server_name == NULL)
		{
			printf("Invalid l2tp server name!\n");
			return RTK_FAILED;
		}
		if (!apmib_set(MIB_L2TP_SERVER_DOMAIN, (void *)pl2tp_config->l2tp_server_info.server_name))
		{
			printf("Set l2tp server domain error!");
			return RTK_FAILED;
		}
	}
	else if(pl2tp_config->server_info_flag == 0)//by ip
	{
		if(!apmib_set(MIB_L2TP_GET_SERV_BY_DOMAIN,(void*)&pl2tp_config->server_info_flag))
		{
			printf("Set l2tp get server by domain error!");
			return RTK_FAILED;
		}
		addr.s_addr = pl2tp_config->l2tp_server_info.server_ip;
		if(!apmib_set(MIB_L2TP_SERVER_IP_ADDR, (void *)&addr))
		{
			printf("Set l2tp server ip error!");
			return RTK_FAILED;
		}
	}
	else
	{
		printf("Invalid server info flag!\n");
		return RTK_FAILED;
	}
	/*l2tp user name pwd etc. config*/
	if(pl2tp_config->ppp_config.user_name == NULL || pl2tp_config->ppp_config.passwd == NULL)
	{
		printf("Invalid username or password!\n");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_L2TP_USER_NAME, (void *)pl2tp_config->ppp_config.user_name))
	{
		printf("Set L2TP user name MIB error!\n");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_L2TP_PASSWORD, (void *)pl2tp_config->ppp_config.passwd))
	{
		printf("Set L2TP password MIB error!\n");
		return RTK_FAILED;
	}
	if(pl2tp_config->ppp_config.connection_type > 2)
	{
		printf("Invalid ppp connection type!\n");
		return RTK_FAILED;
	}
	conn_type = (PPP_CONNECT_TYPE_T)pl2tp_config->ppp_config.connection_type;
	if(!apmib_set(MIB_L2TP_CONNECTION_TYPE, (void *)&conn_type))
	{
		printf("Set L2TP connection type MIB error!\n");
		return RTK_FAILED;
	}
	if(conn_type != CONTINUOUS)
	{
		if(!apmib_set(MIB_L2TP_IDLE_TIME,(void *)&pl2tp_config->ppp_config.idle_time))
		{
			printf("Set L2TP idle time MIB error!");
			return RTK_FAILED;
		}
	}
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_set_wan_pptp
  *@ input
  	pptp_config, struct rtk_pptp_config *,  ppp configuration incluing 
  	usname/password/connection type/idle time 
  	for pptp , the local ip and remote sever ip should be specified.
 
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function set wan using pptp type. pptp_config specific the 
  	ppp config and local ip and remote server ip.
  	username and password is necessary for ppp connection.
  	connection type : 0 continues 1 mannual  2 dial on demand
  	idle timeout. the idle time > idle timeout the ppp will disconnect
	there are serveral ways to decide the local ip and remote server ip
	for local ip: static ,dhcp
	for remote ip: static , ip from domain name 
  *
  */

int rtk_set_wan_pptp(struct rtk_pptp_config *pptp_config)
{
	WAN_IP_TYPE_T wanIpType;
	PPP_CONNECT_TYPE_T conn_type;
	struct in_addr addr;
	DHCP_T dhcp;
	if(pptp_config == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
	dhcp = PPTP;
	if(!apmib_set(MIB_WAN_DHCP, (void *)&dhcp))
	{
		printf("Set DHCP error!");
		return RTK_FAILED;
	}
	if(pptp_config->local_ip_selection != 0 && pptp_config->local_ip_selection != 1)
	{
		printf("Invalid local ip selection value!\n");
		return RTK_FAILED;
	}
	wanIpType = (WAN_IP_TYPE_T)pptp_config->local_ip_selection;
	if(!apmib_set(MIB_PPTP_WAN_IP_DYNAMIC, (void *)&wanIpType))
	{
		printf("Set MIB_PPTP_WAN_IP_DYNAMIC error!\n");
		return RTK_FAILED;
	}
	if(wanIpType == STATIC_IP)//set static ip
	{
		addr.s_addr = pptp_config->static_config.ip;
		if (!apmib_set(MIB_PPTP_IP_ADDR, (void *)&addr))
		{
			printf("Set pptp IP-address error!\n");
			return RTK_FAILED;
		}
		addr.s_addr = pptp_config->static_config.mask;
		if(!apmib_set(MIB_PPTP_SUBNET_MASK,(void *)&addr))
		{
			printf("Set pptp subnet-mask error!\n");
			return RTK_FAILED;
		}
		addr.s_addr = pptp_config->static_config.gw;
		if(!apmib_set(MIB_PPTP_DEFAULT_GW,(void *)&addr))
		{
			printf("Set pptp default-gateway error!\n");
			return RTK_FAILED;
		}
	}
	if(pptp_config->server_info_flag == 1)//by domain name
	{
		if(!apmib_set(MIB_PPTP_GET_SERV_BY_DOMAIN,(void*)&pptp_config->server_info_flag))
		{
			printf("Set pptp get server by domain error!");
			return RTK_FAILED;
		}
		if(pptp_config->pptp_server_info.server_name == NULL)
		{
			printf("Invalid pptp server name!\n");
			return RTK_FAILED;
		}
		if (!apmib_set(MIB_PPTP_SERVER_DOMAIN, (void *)pptp_config->pptp_server_info.server_name))
		{
			printf("Set pptp server domain error!");
			return RTK_FAILED;
		}
	}
	else if(pptp_config->server_info_flag == 0)//by ip
	{
		if(!apmib_set(MIB_PPTP_GET_SERV_BY_DOMAIN,(void*)&pptp_config->server_info_flag))
		{
			printf("Set pptp get server by domain error!");
			return RTK_FAILED;
		}
		addr.s_addr = pptp_config->pptp_server_info.server_ip;
		if(!apmib_set(MIB_PPTP_SERVER_IP_ADDR, (void *)&addr))
		{
			printf("Set pptp server ip error!");
			return RTK_FAILED;
		}
	}
	else
	{
		printf("Invalid server info flag!\n");
		return RTK_FAILED;
	}
	/*pptp user name pwd etc. config*/
	if(pptp_config->ppp_config.user_name == NULL || pptp_config->ppp_config.passwd == NULL)
	{
		printf("Invalid username or password!\n");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_PPTP_USER_NAME, (void *)pptp_config->ppp_config.user_name))
	{
		printf("Set PPTP user name MIB error!\n");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_PPTP_PASSWORD, (void *)pptp_config->ppp_config.passwd))
	{
		printf("Set PPTP password MIB error!\n");
		return RTK_FAILED;
	}
	if(pptp_config->ppp_config.connection_type > 2)
	{
		printf("Invalid ppp connection type!\n");
		return RTK_FAILED;
	}
	conn_type = (PPP_CONNECT_TYPE_T)pptp_config->ppp_config.connection_type;
	if(!apmib_set(MIB_PPTP_CONNECTION_TYPE, (void *)&conn_type))
	{
		printf("Set PPTP connection type MIB error!\n");
		return RTK_FAILED;
	}
	if(conn_type != CONTINUOUS)
	{
		if(!apmib_set(MIB_PPTP_IDLE_TIME,(void *)&pptp_config->ppp_config.idle_time))
		{
			printf("Set PPTP idle time MIB error!");
			return RTK_FAILED;
		}
	}
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_set_wan_mtu
  *@ input
	mtu, int , the mtu configured to wan interface
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
	for static & dhcp  wan type. normally 1500
	for pppoe, normally 1492
	for l2tp/pptp, normally 1462
  *
  */

int rtk_set_wan_mtu(unsigned int mtu)
{
	DHCP_T curDhcp;	
	if(!apmib_get(MIB_WAN_DHCP, (void *)&curDhcp))
	{
		printf("Get WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	switch(curDhcp)
	{
		case DHCP_DISABLED:
			if(!apmib_set(MIB_FIXED_IP_MTU_SIZE, (void *)&mtu))
			{
				printf("Set Fixed-IP mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		case DHCP_CLIENT:
			if(!apmib_set(MIB_DHCP_MTU_SIZE, (void *)&mtu))
			{
				printf("Set DHCP mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		case PPPOE:
			if(!apmib_set(MIB_PPP_MTU_SIZE, (void *)&mtu))
			{
				printf("Set PPPOE mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		case PPTP:
			if(!apmib_set(MIB_PPTP_MTU_SIZE, (void *)&mtu))
			{
				printf("Set PPTP mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		case L2TP:
			if(!apmib_set(MIB_L2TP_MTU_SIZE, (void *)&mtu))
			{
				printf("Set L2TP mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		default:
			break;
	}
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_set_wan_dns
  *@ input
	dns_type unsigned int  0 auto ,1 manual
	dnsx, unsigned int, ip address of dns server x
	
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
	set dns type , if auto , the dns server will be getted from dhcp sever , ppp server
	if manual,at least one dns server should be specifed. 
	for static wan type , the dns sever should be configured manually.
  *
  */

int rtk_set_wan_dns(unsigned int dns_type, unsigned int dns1, unsigned int dns2, unsigned dns3)
{
	DNS_TYPE_T dns;
	struct in_addr addr;
	if(dns_type != 0 && dns_type != 1)
	{
		printf("Invalid dns type!\n");
		return RTK_FAILED;
	}
	dns = (DNS_TYPE_T)dns_type;
	if(!apmib_set(MIB_DNS_MODE, (void *)&dns))
	{
		printf("Set DNS MIB error!");
		return RTK_FAILED;
	}
	if(dns == DNS_MANUAL)
	{
		if(dns1 == 0 && dns2 == 0 && dns3 == 0)
		{
			printf("Error!No DNS specified!\n");
			return RTK_FAILED;
		}
		if(dns1 != 0)
		{
			addr.s_addr = dns1;
			if (!apmib_set(MIB_DNS1, (void *)&addr))
			{
				printf("Set DNS1 MIB error!");
				return RTK_FAILED;
			}
		}
		if(dns2 != 0)
		{
			addr.s_addr = dns2;
			if (!apmib_set(MIB_DNS2, (void *)&addr))
			{
				printf("Set DNS2 MIB error!");
				return RTK_FAILED;
			}
		}
		if(dns3 != 0)
		{
			addr.s_addr = dns3;
			if (!apmib_set(MIB_DNS3, (void *)&addr))
			{
				printf("Set DNS3 MIB error!");
				return RTK_FAILED;
			}
		}
	}
	return RTK_SUCCESS;
}


/*
  *@ name: rtk_set_wan_clone_mac
  *@ input
	mac , unsigned char *, the mac in hex array. 112233445566h means 11:22:33:44:55:66
	
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	Set Wan interface Clone MAC
  *
  */
int rtk_set_wan_clone_mac(unsigned char *mac)
{
	char buff[128]={0};
	if(mac == NULL)
	{
		printf("Invalid Input.\n");
		return RTK_FAILED;
	}
	if(!apmib_set(MIB_WAN_MAC_ADDR, (void *)mac))
	{
		printf("Set MIB_WAN_MAC_ADDR mib error!\n");
		return RTK_FAILED;
	}
	sprintf(buff,"ifconfig eth1 hw ether %02x%02x%02x%02x%02x%02x",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
//printf(buff);printf("\n");
	system(buff);
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_get_wan_type
  *@ input
  	type,unsigned int*, wan type
  	0:static ip
  	1:dhcp client
  	3:pppoe
  	4:pptp
  	6:l2tp
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function get the current wan type
  *
  */

int rtk_get_wan_type(unsigned int *type)
{
	DHCP_T curDhcp;
	if(type == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
	if(!apmib_get(MIB_WAN_DHCP, (void *)&curDhcp))
	{
		printf("Get WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	*type = curDhcp;
	return RTK_SUCCESS;
}


/*GET*/
/*
  *@ name: rtk_get_wan_static
  *@ input
  	pstatic_config, struct rtk_static_config *, wan ip and ipmask and default gw ip
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function get wan static ip connection. the wan ip , mask, default gw
  *
  */
int rtk_get_wan_static(struct rtk_static_config *pstatic_config)
{
#if 0
	DHCP_T curDhcp;
#endif
	struct in_addr *addr;
	char tmpBuff[64];
	if(pstatic_config == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
#if 0 //for when ui tcpipwan change to static, need to fetch static info
	if(!apmib_get(MIB_WAN_DHCP, (void *)&curDhcp))
	{
		printf("Get WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	if(curDhcp != DHCP_DISABLED)
	{
		printf("Current wan type is not static ip!\n");
		return RTK_FAILED;
	}
#endif
	memset(tmpBuff,0,sizeof(tmpBuff));
	if(!apmib_get(MIB_WAN_IP_ADDR,(void *)tmpBuff))
	{
		printf("Failed to get WAN IP address!\n");
		return RTK_FAILED;
	}
	addr = (struct in_addr*)tmpBuff;
	pstatic_config->ip = addr->s_addr;
	
	memset(tmpBuff,0,sizeof(tmpBuff));
	if(!apmib_get(MIB_WAN_SUBNET_MASK,(void *)tmpBuff))
	{
		printf("Failed to get WAN sub-net mask!\n");
		return RTK_FAILED;
	}
	addr = (struct in_addr*)tmpBuff;
	pstatic_config->mask = addr->s_addr;
	
	memset(tmpBuff,0,sizeof(tmpBuff));
	if(!apmib_get(MIB_WAN_DEFAULT_GATEWAY,(void *)tmpBuff))
	{
		printf("Failed to get WAN default gateway!\n");
		return RTK_FAILED;
	}
	addr = (struct in_addr*)tmpBuff;
	pstatic_config->gw = addr->s_addr;
	return RTK_SUCCESS;
}


/*
  *@ name: rtk_get_wan_dhcp
  *@ input
  	pdhcp_config, struct rtk_dhcp_config *,  the hostname using
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function get wan dhcp settings i.e. the host name of dhcp server 
  *
  */
int rtk_get_wan_dhcp(struct rtk_dhcp_config *pdhcp_config)
{
#if 0
	DHCP_T curDhcp;
#endif
	if(pdhcp_config == NULL || pdhcp_config->host_name == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
#if 0 //for when ui tcpipwan change to dhcp, need to fetch dhcp info
	if(!apmib_get(MIB_WAN_DHCP,(void *)&curDhcp))
	{
		printf("Get WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	if(curDhcp != DHCP_CLIENT)
	{
		printf("Current wan type is not dhcp client!\n");
		return RTK_FAILED;
	}
#endif
	memset(pdhcp_config->host_name,0,RTK_LEN_32);
	if(!apmib_get(MIB_HOST_NAME, (void *)&pdhcp_config->host_name))
	{
		printf("Failed to get host name!\n");
		return RTK_FAILED;
	}
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_get_wan_pppoe
  *@ input
  	ppppoe_config, struct rtk_pppoe_config *,  ppp configuration incluing 
  	usname/password/connection type/idle time  and ac name
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function get wan  pppoe settings. ppppoe_config specific the 
  	ppp config and ac name.
  	username and password is necessary for ppp connection.
  	connection type : 0 continues 1 mannual  2 dial on demand
  	idle timeout. the idle time > idle timeout the ppp will disconnect
  	ac name is for selection of different server , only the server with same
  	ac name can be connected
  *
  */
int rtk_get_wan_pppoe(struct rtk_pppoe_config *ppppoe_config)
{
#if 0
	DHCP_T curDhcp;
#endif
	PPP_CONNECT_TYPE_T type;
	int intVal;
	if(ppppoe_config == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
#if 0 //for when ui tcpipwan change to pppoe, need to fetch pppoe info
	if(!apmib_get(MIB_WAN_DHCP, (void *)&curDhcp))
	{
		printf("Get WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	if(curDhcp != PPPOE)
	{
		printf("Current wan type is not pppoe!\n");
		return RTK_FAILED;
	}
#endif
	memset(ppppoe_config->ppp_config.user_name,0,RTK_LEN_32);
	if(!apmib_get(MIB_PPP_USER_NAME,(void *)ppppoe_config->ppp_config.user_name))
	{
		printf("Failed to get ppp user name!\n");
		return RTK_FAILED;
	}
	memset(ppppoe_config->ppp_config.passwd,0,RTK_LEN_32);
	if(!apmib_get(MIB_PPP_PASSWORD,(void *)ppppoe_config->ppp_config.passwd))
	{
		printf("Failed to get ppp password!\n");
		return RTK_FAILED;
	}
	memset(ppppoe_config->ac_name,0,RTK_LEN_32);
	if(!apmib_get(MIB_PPP_SERVICE_NAME,(void *)ppppoe_config->ac_name))
	{
		printf("Failed to get ppp service name!\n");
		return RTK_FAILED;
	}
	if(!apmib_get(MIB_PPP_CONNECT_TYPE, (void *)&type))
	{
		printf("Failed to get ppp connection type!\n");
		return RTK_FAILED;
	}
	ppppoe_config->ppp_config.connection_type = (unsigned int)type;
	if(!apmib_get( MIB_PPP_IDLE_TIME,  (void *)&intVal))
	{
		printf("Failed to get idle time!\n");
		return RTK_FAILED;
	}
	ppppoe_config->ppp_config.idle_time = (unsigned int)intVal;
	return RTK_SUCCESS;
}


/*
  *@ name: rtk_get_wan_l2tp
  *@ input
  	pl2tp_config, struct rtk_l2tp_config *,  ppp configuration incluing 
  	usname/password/connection type/idle time 
  	for l2pt , the local ip and remote sever ip should be specified.
 
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function get wan  l2tp settings. pl2tp_config specific the 
  	ppp config and local ip and remote server ip.
  	username and password is necessary for ppp connection.
  	connection type : 0 continues 1 mannual  2 dial on demand
  	idle timeout. the idle time > idle timeout the ppp will disconnect
	there are serveral ways to decide the local ip and remote server ip
	for local ip: static ,dhcp
	for remote ip: static , ip from domain name 
  *
  */
int rtk_get_wan_l2tp(struct rtk_l2tp_config *pl2tp_config)
{
#if 0
	DHCP_T curDhcp;
#endif
	WAN_IP_TYPE_T wanIpType;
	char buffer[64];
	struct in_addr *addr;
	int val;
	PPP_CONNECT_TYPE_T type;
	if(pl2tp_config == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
#if 0 //for when ui tcpipwan change to l2tp, need to fetch l2tp info
	if(!apmib_get(MIB_WAN_DHCP, (void *)&curDhcp))
	{
		printf("Get WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	if(curDhcp != L2TP)
	{
		printf("Current wan type is not l2tp!\n");
		return RTK_FAILED;
	}
#endif
	if (!apmib_get(MIB_L2TP_WAN_IP_DYNAMIC,(void *)&wanIpType))
	{
		printf("Failed to get l2tp wan ip type!\n");
		return RTK_FAILED;
	}
	pl2tp_config->local_ip_selection = (unsigned int)wanIpType;
	if(wanIpType == STATIC_IP)//static IP
	{
		memset(buffer,0,sizeof(buffer));
		if(!apmib_get(MIB_L2TP_IP_ADDR,(void *)buffer))
		{
			printf("Failed to get l2tp static IP!\n");
			return RTK_FAILED;
		}
		addr = (struct in_addr*)buffer;
		pl2tp_config->static_config.ip = addr->s_addr;

		memset(buffer,0,sizeof(buffer));
		if(!apmib_get(MIB_L2TP_SUBNET_MASK,(void *)buffer))
		{
			printf("Failed to get l2tp subnet mask!\n");
			return RTK_FAILED;
		}
		addr = (struct in_addr*)buffer;
		pl2tp_config->static_config.mask = addr->s_addr;
		
		memset(buffer,0,sizeof(buffer));
		if(!apmib_get(MIB_L2TP_DEFAULT_GW,(void *)buffer))
		{
			printf("Failed to get l2tp default gateway!\n");
			return RTK_FAILED;
		}
		addr = (struct in_addr*)buffer;
		pl2tp_config->static_config.gw = addr->s_addr;
	}
	
	if(!apmib_get(MIB_L2TP_GET_SERV_BY_DOMAIN,(void*)&val))
	{
		printf("Failed to get MIB_L2TP_GET_SERV_BY_DOMAIN\n");
		return RTK_FAILED;
	}
	pl2tp_config->server_info_flag = (unsigned int)val;
	if(val == 1)//by domain name
	{
		memset(pl2tp_config->l2tp_server_info.server_name,0,sizeof(pl2tp_config->l2tp_server_info.server_name));
		if(!apmib_get(MIB_L2TP_SERVER_DOMAIN, (void *)pl2tp_config->l2tp_server_info.server_name))
		{
			printf("Failed to get l2tp server domain!\n");
			return RTK_FAILED;
		}
	}
	else if(val == 0)//by IP
	{
		memset(buffer,0,sizeof(buffer));
		if(!apmib_get(MIB_L2TP_SERVER_IP_ADDR,  (void *)buffer))
		{
			printf("Failed to get l2tp server IP!\n");
			return RTK_FAILED;
		}
		addr = (struct in_addr*)buffer;
		pl2tp_config->l2tp_server_info.server_ip = addr->s_addr;
	}
	else
	{
		printf("Invalid MIB_L2TP_GET_SERV_BY_DOMAIN\n");
		return RTK_FAILED;
	}
	memset(pl2tp_config->ppp_config.user_name,0,sizeof(pl2tp_config->ppp_config.user_name));
	if(!apmib_get(MIB_L2TP_USER_NAME,	(void *)pl2tp_config->ppp_config.user_name))
	{
		printf("Failed to get l2tp user name!\n");
		return RTK_FAILED;
	}
	memset(pl2tp_config->ppp_config.passwd,0,sizeof(pl2tp_config->ppp_config.passwd));
	if(!apmib_get(MIB_L2TP_PASSWORD,	(void *)pl2tp_config->ppp_config.passwd))
	{
		printf("Failed to get l2tp password!\n");
		return RTK_FAILED;
	}
	if(!apmib_get(MIB_L2TP_CONNECTION_TYPE, (void *)&type))
	{
		printf("Failed to get l2tp ppp connection type!\n");
		return RTK_FAILED;
	}
	pl2tp_config->ppp_config.connection_type = (unsigned int)type;
	if(!apmib_get(MIB_L2TP_IDLE_TIME, (void *)&val))
	{
		printf("Failed to get l2tp idle time!\n");
		return RTK_FAILED;
	}
	pl2tp_config->ppp_config.idle_time = (unsigned int)val;
	return RTK_SUCCESS;	
}

/*
  *@ name: rtk_get_wan_pptp
  *@ input
  	pptp_config, struct rtk_pptp_config *,  ppp configuration incluing 
  	usname/password/connection type/idle time 
  	for pptp , the local ip and remote sever ip should be specified.
 
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function get wan pptp settings. pptp_config specific the 
  	ppp config and local ip and remote server ip.
  	username and password is necessary for ppp connection.
  	connection type : 0 continues 1 mannual  2 dial on demand
  	idle timeout. the idle time > idle timeout the ppp will disconnect
	there are serveral ways to decide the local ip and remote server ip
	for local ip: static ,dhcp
	for remote ip: static , ip from domain name 
  *
  */
int rtk_get_wan_pptp(struct rtk_pptp_config *ppptp_config)
{
#if 0
	DHCP_T curDhcp;
#endif
	struct in_addr *addr;
	WAN_IP_TYPE_T wanIpType;
	char buffer[64];
	int val;
	PPP_CONNECT_TYPE_T type;
	if(ppptp_config == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
#if 0 //for when ui tcpipwan change to pptp, need to fetch pptp info
	if(!apmib_get(MIB_WAN_DHCP, (void *)&curDhcp))
	{
		printf("Get WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	if(curDhcp != PPTP)
	{
		printf("Current wan type is not pptp!\n");
		return RTK_FAILED;
	}
#endif
	if (!apmib_get(MIB_PPTP_WAN_IP_DYNAMIC,(void *)&wanIpType))
	{
		printf("Failed to get pptp wan ip type!\n");
		return RTK_FAILED;
	}
	ppptp_config->local_ip_selection = (unsigned int)wanIpType;
	if(wanIpType == STATIC_IP)//static IP
	{
		memset(buffer,0,sizeof(buffer));
		if(!apmib_get(MIB_PPTP_IP_ADDR,(void *)buffer))
		{
			printf("Failed to get pptp static IP!\n");
			return RTK_FAILED;
		}
		addr = (struct in_addr*)buffer;
		ppptp_config->static_config.ip = addr->s_addr;

		memset(buffer,0,sizeof(buffer));
		if(!apmib_get(MIB_PPTP_SUBNET_MASK,(void *)buffer))
		{
			printf("Failed to get pptp subnet mask!\n");
			return RTK_FAILED;
		}
		addr = (struct in_addr*)buffer;
		ppptp_config->static_config.mask = addr->s_addr;
		
		memset(buffer,0,sizeof(buffer));
		if(!apmib_get(MIB_PPTP_DEFAULT_GW,(void *)buffer))
		{
			printf("Failed to get pptp default gateway!\n");
			return RTK_FAILED;
		}
		addr = (struct in_addr*)buffer;
		ppptp_config->static_config.gw = addr->s_addr;
	}
	if(!apmib_get(MIB_PPTP_GET_SERV_BY_DOMAIN,(void*)&val))
	{
		printf("Failed to get MIB_PPTP_GET_SERV_BY_DOMAIN\n");
		return RTK_FAILED;
	}
	ppptp_config->server_info_flag = (unsigned int)val;
	if(val == 1)//by domain name
	{
		memset(ppptp_config->pptp_server_info.server_name,0,sizeof(ppptp_config->pptp_server_info.server_name));
		if(!apmib_get(MIB_PPTP_SERVER_DOMAIN, (void *)ppptp_config->pptp_server_info.server_name))
		{
			printf("Failed to get MIB_PPTP_SERVER_DOMAIN!\n");
			return RTK_FAILED;
		}
	}
	else if(val == 0)//by IP
	{
		memset(buffer,0,sizeof(buffer));
		if(!apmib_get(MIB_PPTP_SERVER_IP_ADDR,(void *)buffer))
		{
			printf("Failed to get pptp server IP!\n");
			return RTK_FAILED;
		}
		addr = (struct in_addr*)buffer;
		ppptp_config->pptp_server_info.server_ip = addr->s_addr;
	}
	else
	{
		printf("Invalid MIB_PPTP_GET_SERV_BY_DOMAIN\n");
		return RTK_FAILED;
	}
	memset(ppptp_config->ppp_config.user_name,0,sizeof(ppptp_config->ppp_config.user_name));
	if(!apmib_get(MIB_PPTP_USER_NAME,	(void *)ppptp_config->ppp_config.user_name))
	{
		printf("Failed to get pptp user name!\n");
		return RTK_FAILED;
	}
	memset(ppptp_config->ppp_config.passwd,0,sizeof(ppptp_config->ppp_config.passwd));
	if(!apmib_get(MIB_PPTP_PASSWORD,	(void *)ppptp_config->ppp_config.passwd))
	{
		printf("Failed to get pptp password!\n");
		return RTK_FAILED;
	}
	if(!apmib_get(MIB_PPTP_CONNECTION_TYPE, (void *)&type))
	{
		printf("Failed to get pptp ppp connection type!\n");
		return RTK_FAILED;
	}
	ppptp_config->ppp_config.connection_type = (unsigned int)type;
	if(!apmib_get(MIB_PPTP_IDLE_TIME, (void *)&val))
	{
		printf("Failed to get pptp idle time!\n");
		return RTK_FAILED;
	}
	ppptp_config->ppp_config.idle_time = (unsigned int)val;
	return RTK_SUCCESS;	
}

/*
  *@ name: rtk_get_wan_mtu
  *@ input
	mtu, int * , the mtu configured to wan interface
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	get wan mtu
	for static & dhcp  wan type. normally 1500
	for pppoe, normally 1492
	for l2tp/pptp, normally 1462
  *
  */
int rtk_get_wan_mtu(unsigned int *mtu)
{
	DHCP_T curDhcp;
	int intVal;
	if(mtu == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
	if(!apmib_get(MIB_WAN_DHCP, (void *)&curDhcp))
	{
		printf("Get WAN DHCP MIB error!\n");
		return RTK_FAILED;
	}
	switch(curDhcp)
	{
		case DHCP_DISABLED:
			if(!apmib_get(MIB_FIXED_IP_MTU_SIZE, (void *)&intVal))
			{
				printf("Get Fixed-IP mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		case DHCP_CLIENT:
			if(!apmib_get(MIB_DHCP_MTU_SIZE, (void *)&intVal))
			{
				printf("Get DHCP mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		case PPPOE:
			if(!apmib_get(MIB_PPP_MTU_SIZE, (void *)&intVal))
			{
				printf("Get PPPOE mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		case PPTP:
			if(!apmib_get(MIB_PPTP_MTU_SIZE,(void *)&intVal))
			{
				printf("Get PPTP mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		case L2TP:
			if(!apmib_get(MIB_L2TP_MTU_SIZE, (void *)&intVal))
			{
				printf("Get L2TP mtu size MIB error!");
				return RTK_FAILED;
			}
			break;
		default:
			return RTK_FAILED;
			break;
	}
	*mtu = (unsigned int)intVal;
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_get_wan_dns
  *@ input
	dns_type unsigned int*  0 auto ,1 manual
	dnsx, unsigned int*, ip address of dns server x
	
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
	get dns type , if auto , the dns server will not be getted
	if manual,at least one dns server will be specifed. 
  *
  */
int rtk_get_wan_dns(unsigned int *dns_type, unsigned int *dns1, unsigned int *dns2, unsigned *dns3)
{
	DNS_TYPE_T dns;
	struct in_addr *addr;
	char buffer[100];
	if(dns_type == NULL)
	{
		printf("Invalid input!\n");
		return RTK_FAILED;
	}
	if(!apmib_get(MIB_DNS_MODE, (void *)&dns))
	{
		printf("Get DNS MIB error!\n");
		return RTK_FAILED;
	}
	*dns_type = (unsigned int)dns;
	if(dns == DNS_MANUAL)
	{
		if(dns1 == 0 && dns2 == 0 && dns3 == 0)
		{
			printf("Invalid input!\n");
			return RTK_FAILED;
		}
		if(dns1 != NULL)
		{
			memset(buffer,0,sizeof(buffer));
			if (!apmib_get(MIB_DNS1, (void *)&buffer))
			{
				printf("Get DNS1 MIB error!\n");
				return RTK_FAILED;
			}
			addr = (struct in_addr*)buffer;
			*dns1 = addr->s_addr;
		}
		if(dns2 != NULL)
		{
			memset(buffer,0,sizeof(buffer));
			if (!apmib_get(MIB_DNS2, (void *)&buffer))
			{
				printf("Get DNS2 MIB error!\n");
				return RTK_FAILED;
			}
			addr = (struct in_addr*)buffer;
			*dns2 = addr->s_addr;
		}
		if(dns3 != NULL)
		{
			memset(buffer,0,sizeof(buffer));
			if (!apmib_get(MIB_DNS3, (void *)&buffer))
			{
				printf("Get DNS3 MIB error!\n");
				return RTK_FAILED;
			}
			addr = (struct in_addr*)buffer;
			*dns3 = addr->s_addr;
		}
	}
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_get_wan_clone_mac
  *@ input
	mac , unsigned char *, the mac in hex array. 112233445566h means 11:22:33:44:55:66
	
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	Get Wan interface Clone MAC
  *
  */
int rtk_get_wan_clone_mac(unsigned char *mac)
{
	if(mac == NULL)
	{
		printf("Invalid Input.\n");
		return RTK_FAILED;
	}
	memset(mac,0,6);
	if(!apmib_get(MIB_WAN_MAC_ADDR, (void *)mac))
	{
		printf("Get MIB_WAN_MAC_ADDR mib error!\n");
		return RTK_FAILED;
	}
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_close_wan_connection
  *@ input
	none	
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	Close the wan connection
  *
  */
int rtk_close_wan_connection(void)
{
	system("sysconf rtk_close_wan");
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_start_wan_connection
  *@ input
	none	
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	Start the wan connection
  *
  */
int rtk_start_wan_connection(void)
{
	system("sysconf rtk_start_wan");
	return RTK_SUCCESS;
}

/*
  *@ name: rtk_restart_wan
  *@ input
	none	
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	Restart the wan service
  *
  */
int rtk_restart_wan(void)
{
#define NULL_FILE 0
#define NULL_STR ""

	system("rm -r /var/ppp_error 2> /dev/null");
	
	RunSystemCmd(NULL_FILE, "sysconf", "rtk_close_wan", NULL_STR);
	RunSystemCmd(NULL_FILE, "sysconf", "rtk_start_wan", NULL_STR);	
	//system("sysconf rtk_close_wan");
	//system("sysconf rtk_start_wan");
	return RTK_SUCCESS;
}

/**************************************************
* @NAME:
* 	rtk_get_wan_info
* 
* @PARAMETERS:
* 	@Input
* 		info: point of wan_info
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get wan information
*
***************************************************/
int rtk_get_wan_info(RTK_WAN_INFOp info)
{
	if(info == NULL)
		return RTK_FAILED;
	
	memset(info, 0, sizeof(RTK_WAN_INFO));

	DHCP_T dhcp;
	OPMODE_T opmode=-1;
	unsigned int wispWanId=0;
	char *iface=NULL;
	struct in_addr	intaddr;
	struct sockaddr hwaddr;
	unsigned char *pMacAddr;
	int isWanPhyLink = 0;
	RTK_BSS_INFO bss;
	FILE *fp;
	char str[32];
	
	if ( !apmib_get( MIB_WAN_DHCP, (void *)&dhcp) )
		return RTK_FAILED;

	if ( !apmib_get( MIB_OP_MODE, (void *)&opmode) )
		return RTK_FAILED;

	if( !apmib_get(MIB_WISP_WAN_ID, (void *)&wispWanId))
		return RTK_FAILED;

	if(opmode == BRIDGE_MODE){
		return RTK_FAILED;
	}

	if(opmode != WISP_MODE){
 		isWanPhyLink=getWanLink("eth1");
 	}

	// get status
	if ( dhcp == DHCP_CLIENT)
	{
		if(opmode == WISP_MODE)
		{
			if(0 == wispWanId)
				iface = "wlan0";
			else if(1 == wispWanId)
				iface = "wlan1";
#ifdef CONFIG_SMART_REPEATER
			if(getWispRptIface(&iface,wispWanId)<0)
				return RTK_FAILED;
#endif
		}
		else
			iface = "eth1";
	 	if (!isDhcpClientExist(iface))
			info->status = GETTING_IP_FROM_DHCP_SERVER;
		else{
			if(isWanPhyLink < 0)
				info->status = GETTING_IP_FROM_DHCP_SERVER;
			else
				info->status = DHCP;
		}
	}
	else if ( dhcp == DHCP_DISABLED )
	{
		if (opmode == WISP_MODE)
		{
			char wan_intf[MAX_NAME_LEN] = {0};
			char lan_intf[MAX_NAME_LEN] = {0};
			
			getInterfaces(lan_intf,wan_intf);
			memset(&bss, 0x00, sizeof(bss));
			getWlBssInfo(wan_intf, &bss);
			if (bss.state == RTK_STATE_CONNECTED){
				info->status = FIXED_IP_CONNECTED;
			}
			else
			{
				info->status = FIXED_IP_DISCONNECTED;
			}				
		}
		else
		{
			if(isWanPhyLink < 0)
				info->status = FIXED_IP_DISCONNECTED;
			else
				info->status = FIXED_IP_CONNECTED;
		}
	}
	else if ( dhcp ==  PPPOE )
	{
		if ( isConnectPPP())
		{
			if(isWanPhyLink < 0)
				info->status = PPPOE_DISCONNECTED;
			else
				info->status = PPPOE_CONNECTED;
		}else
			info->status = PPPOE_DISCONNECTED;
	}
	else if ( dhcp ==  PPTP )
	{
		if ( isConnectPPP()){
			if(isWanPhyLink < 0)
				info->status = PPTP_DISCONNECTED;
			else
				info->status = PPTP_CONNECTED;
		}else
			info->status = PPTP_DISCONNECTED;
	}
	else if ( dhcp ==  L2TP )
	{
		if ( isConnectPPP()){
			if(isWanPhyLink < 0)
				info->status = L2TP_DISCONNECED;
			else
				info->status = L2TP_CONNECTED;
		}else
			info->status = L2TP_DISCONNECED;
	}
#ifdef RTK_USB3G
	else if ( dhcp == USB3G ) {
		int inserted = 0;
		char str[32];

		if (isConnectPPP()){
			info->status = USB3G_CONNECTED;
		}else {
			int retry = 0;

OPEN_3GSTAT_AGAIN:
			fp = fopen("/var/usb3g.stat", "r");

			if (fp !=NULL) {
				fgets(str, sizeof(str),fp);
				fclose(fp);
			}
			else if (retry < 5) {
				retry++;
				goto OPEN_3GSTAT_AGAIN;
			}

			if (str != NULL && strstr(str, "init")) {
				info->status = USB3G_MODEM_INIT;
			}
			else if (str != NULL && strstr(str, "dial")) {
				info->status = USB3G_DAILING;
			}
			else if (str != NULL && strstr(str, "remove")) {
				info->status = USB3G_REMOVED;
			}
			else
				info->status = USB3G_DISCONNECTED;
		}
    }
#endif /* #ifdef RTK_USB3G */

	// get ip, mask, default gateway

	if ( dhcp == PPPOE || dhcp == PPTP || dhcp == L2TP || dhcp == USB3G )
	{
		iface = "ppp0";
		if ( !isConnectPPP() )
			iface = NULL;
	}
	else if (opmode == WISP_MODE){
		if(0 == wispWanId)
			iface = "wlan0";
		else if(1 == wispWanId)
			iface = "wlan1";
#ifdef CONFIG_SMART_REPEATER
		if(getWispRptIface(&iface,wispWanId)<0)
			return RTK_FAILED;
#endif			
	}
	else
		iface = "eth1";

	if(opmode != WISP_MODE)
	{
		if(iface){
			if((isWanPhyLink = getWanLink("eth1")) < 0){
				info->ip.s_addr = 0;
			}
		}	
	}
	
	intaddr.s_addr = 0;
	if ( iface && rtk_getInAddr(iface, IP_ADDR_T, (void *)&intaddr ) && ((isWanPhyLink >= 0)) )
		info->ip.s_addr = intaddr.s_addr;
	else
		info->ip.s_addr = 0;

	intaddr.s_addr = 0;
	if ( iface && rtk_getInAddr(iface, NET_MASK_T, (void *)&intaddr ) && ((isWanPhyLink >= 0) ))
		info->mask.s_addr = intaddr.s_addr;
	else
		info->mask.s_addr = 0;

	intaddr.s_addr = 0;
	if ( iface && getDefaultRoute(iface, &intaddr) && ((isWanPhyLink >= 0) ))
		info->def_gateway.s_addr = intaddr.s_addr;
	else
		info->def_gateway.s_addr = 0;

	if(opmode == WISP_MODE)
	{
		if(0 == wispWanId)
			iface = "wlan0";
		else if(1 == wispWanId)
			iface = "wlan1";
#ifdef CONFIG_SMART_REPEATER
		if(getWispRptIface(&iface,wispWanId)<0)
			return RTK_FAILED;
#endif			
	}	
	else
		iface = "eth1";
	
	if ( rtk_getInAddr(iface, HW_ADDR_T, (void *)&hwaddr ) )
	{
		pMacAddr = (unsigned char *)hwaddr.sa_data;
		memcpy(info->mac, pMacAddr, 6);
	}
	else
		memset(info->mac, 0, 6);

#ifdef RTK_USB3G
	if (dhcp == USB3G)
		memset(info->mac, 0, 6);
#endif /* #ifdef RTK_USB3G */

	// get dns
	if(isWanPhyLink<0 || info->ip.s_addr == 0)
	{
		info->dns1.s_addr=0;
		info->dns2.s_addr=0;
		info->dns3.s_addr=0;
	}
	else
	{	
	fp = fopen("/var/resolv.conf", "r");

	if (fp ==NULL) {
		return RTK_FAILED;
	}

	fgets(str, sizeof(str),fp);
	inet_aton(str+11, &intaddr);
	info->dns1.s_addr = intaddr.s_addr;
	intaddr.s_addr = 0;
	if(!feof(fp))
	{
		fgets(str, sizeof(str),fp);
		inet_aton(str+11, &intaddr);
		info->dns2.s_addr = intaddr.s_addr;
		intaddr.s_addr = 0;
		if(!feof(fp))
		{
			fgets(str, sizeof(str),fp);
			inet_aton(str+11, &intaddr);
			info->dns3.s_addr = intaddr.s_addr;
		}
		else
			info->dns3.s_addr = 0;
	}
	else
		info->dns2.s_addr = 0;
	
	fclose(fp);
	}
	return RTK_SUCCESS;
}

/**************************************************
* @NAME:
* 	get_pppoe_err_code
* 
* @PARAMETERS:
* 	@Input
* 		info: null
* 	@Output
*	 	pppoe error number
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get pppoe error code
*
***************************************************/

int rtk_get_pppoe_err_code(int *err_code)
{
	FILE *fp = NULL;
	fp = fopen("/var/ppp_error","r");
	if(NULL != fp)
	{
		fscanf(fp,"%d",err_code);
		fclose(fp);
		return RTK_SUCCESS;
	}
	return RTK_FAILED;
}

/*
  *@ name: rtk_start_pppoe
  *@ input
  	none
  *@ output
  	none
  *@ return value
  	RTK_SUCCESS
  	RTK_FAILED
  *@ function description
  	this function is used to start the pppoe connection
  *
  */
int rtk_start_pppoe(void)
{
	struct rtk_pppoe_config p_config;
	if(rtk_get_wan_pppoe(&p_config)!=RTK_SUCCESS)
	{
		return RTK_FAILED;
	}
	system("sysconf pppoe");
	return RTK_SUCCESS;
}

