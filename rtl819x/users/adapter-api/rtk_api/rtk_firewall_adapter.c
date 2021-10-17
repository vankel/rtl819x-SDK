/*FIREWALL API*/
//#include "rtk_firewall_adapter.h"
#include "rtk_api.h"
#include "rtk_adapter.h"

/**************************************************
* @NAME:
* 	rtk_within_subnet_check
* 
* @PARAMETERS:
* 	@Input
* 		ipaddr: ip address
* 	@Output
*	 	flag: within the current lan subnet or not
*		        1: within the current lan subnet, 0: not within the current lan subnet
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	check the ipaddr within the current lan subnet or not 
*
***************************************************/
int rtk_within_subnet_check(unsigned int ipaddr, int *flag)
{
	int ret = RTK_SUCCESS;
	struct in_addr curIpAddr, curSubnet;
	unsigned int v1, v2, v3;
	
	if (!flag)
		return RTK_FAILED;
	
	rtk_getInAddr(RTK_BRIDGE_INTERFACE_NAME, IP_ADDR_T, (void *)&curIpAddr);
	rtk_getInAddr(RTK_BRIDGE_INTERFACE_NAME, NET_MASK_T, (void *)&curSubnet);
	
	v1 = ipaddr;
	v2 = *((unsigned int *)&curIpAddr);
	v3 = *((unsigned int *)&curSubnet);
	
	if ( (v1 & v3) != (v2 & v3) ) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d ipaddr=0x%x not within the current lan subnet.\n", __FUNCTION__, __LINE__, ipaddr);
		#endif
		*flag = 0;		
	}
	else
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d ipaddr=0x%x within the current lan subnet.\n", __FUNCTION__, __LINE__, ipaddr);
		#endif
		*flag = 1;		
	}
	
	return	ret;
}
/*SET*/
/**************************************************
* @NAME:
* 	rtk_set_port_forward_enable
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the port forward function or not
*				 1: enable, 0:disable
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set port forward enable 
*
***************************************************/
int rtk_set_port_forward_enable(unsigned int enabled)
{
	int ret = RTK_SUCCESS;
	
	if (!apmib_set(MIB_PORTFW_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set port forward enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_add_port_forward_entry
* 
* @PARAMETERS:
* 	@Input
* 		pPortFW: the port forward entry to add
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add port forward entry 
*
***************************************************/
int rtk_add_port_forward_entry(RTK_PORTFW_T *pPortFW)
{
	int ret = RTK_SUCCESS;
	int number = 0, i, flag = 0;
	RTK_PORTFW_T entry;
	unsigned int ipaddr = 0;
		
	if (!pPortFW)
		return RTK_FAILED;
	
	apmib_get(MIB_PORTFW_TBL_NUM,(void*)&number);
	if(number >= MAX_FILTER_NUM){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d port forward number shoule not be more than %d\n",__FUNCTION__, __LINE__, MAX_FILTER_NUM);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
	//subnet check
	ipaddr = (*((unsigned int *)&pPortFW->ipAddr[0]));
	rtk_within_subnet_check(ipaddr, &flag);
	if (!flag)
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d Invalid IP address ipaddr=0x%x! It should be set within the current subnet.\n", __FUNCTION__, __LINE__, ipaddr);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}	
	
	// Check if there is any port overlapped
	for (i=1; i<=number; i++) {
		memset((void *)&entry, '\0', sizeof(entry));
		*((char *)&entry) = (char)i;
		if ( !apmib_get(MIB_PORTFW_TBL, (void *)&entry)) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d Get table entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
		if ( ( (pPortFW->fromPort <= entry.fromPort &&
				pPortFW->toPort >= entry.fromPort) ||
		       (pPortFW->fromPort >= entry.fromPort &&
				pPortFW->fromPort <= entry.toPort))&&
		       (pPortFW->protoType & entry.protoType) ) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d Setting port range has overlapped with used port numbers!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}	
	
	apmib_set(MIB_PORTFW_DEL, pPortFW);
	if ( apmib_set(MIB_PORTFW_ADD, pPortFW) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d port forward set mib error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}

set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_del_port_forward_entry
* 
* @PARAMETERS:
* 	@Input
* 		pPortFW: the port forward entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete port forward entry 
*
***************************************************/
int rtk_del_port_forward_entry(RTK_PORTFW_T *pPortFW, unsigned int delall)
{
	int ret = RTK_SUCCESS;
	RTK_PORTFW_T entry;

	memset((void *)&entry, '\0', sizeof(entry));
	if (delall)
	{
		if (apmib_set(MIB_PORTFW_DELALL, (void *)&entry) == 0) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d port forward delete all error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}
	else
	{
		if (!pPortFW)
			return RTK_FAILED;

		if (!apmib_set(MIB_PORTFW_DEL, (void *)pPortFW)) 
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d port forward delete an entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}		
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_set_port_filter_enable
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the port filter function or not
*				 1: enable, 0:disable
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set port filter enable 
*
***************************************************/
int rtk_set_port_filter_enable(unsigned int enabled)
{
	int ret = RTK_SUCCESS;
	
	if (!apmib_set(MIB_PORTFILTER_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set port filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_add_port_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pPortFilter: the port filter entry to add
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add port filter entry 
*
***************************************************/
int rtk_add_port_filter_entry(RTK_PORTFILTER_T *pPortFilter)
{
	int ret = RTK_SUCCESS;
	int number = 0, i;
	RTK_PORTFILTER_T entry;
		
	if (!pPortFilter)
		return RTK_FAILED;
	
	apmib_get(MIB_PORTFILTER_TBL_NUM,(void*)&number);
	if(number >= MAX_FILTER_NUM){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d port filter number shoule not be more than %d\n",__FUNCTION__, __LINE__, MAX_FILTER_NUM);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	//valid check
	if ((pPortFilter->fromPort < 1) ||
		(pPortFilter->toPort < 1) || 
		(pPortFilter->fromPort > pPortFilter->toPort))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d port filter invalid port number\n",__FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	// valid check
	for (i=1; i<=number; i++) {
		memset((void *)&entry, '\0', sizeof(entry));
		*((char *)&entry) = (char)i;
		if ( !apmib_get(MIB_PORTFILTER_TBL, (void *)&entry)) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d Get table entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
		if ((entry.fromPort == pPortFilter->fromPort) &&
		(entry.toPort == pPortFilter->toPort)&&
		((entry.protoType == pPortFilter->protoType)||
		((entry.protoType==PROTO_BOTH)&&pPortFilter->protoType==PROTO_UDP)||
		((entry.protoType==PROTO_BOTH)&&pPortFilter->protoType==PROTO_TCP)||
		((entry.protoType==PROTO_TCP)&&pPortFilter->protoType==PROTO_BOTH)||
		((entry.protoType==PROTO_UDP)&&pPortFilter->protoType==PROTO_BOTH)))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d rule exist!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
		if ((((entry.fromPort <= pPortFilter->fromPort) &&
		(entry.toPort >= pPortFilter->fromPort))||
		((entry.fromPort <= pPortFilter->toPort) &&
		(entry.toPort >= pPortFilter->toPort)))&&
		((entry.protoType == pPortFilter->protoType)||
		((entry.protoType==PROTO_BOTH)&&pPortFilter->protoType==PROTO_UDP)||
		((entry.protoType==PROTO_BOTH)&&pPortFilter->protoType==PROTO_TCP)||
		((entry.protoType==PROTO_TCP)&&pPortFilter->protoType==PROTO_BOTH)||
		((entry.protoType==PROTO_UDP)&&pPortFilter->protoType==PROTO_BOTH)))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d port overlap!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
		if ((((entry.fromPort >= pPortFilter->fromPort) &&
		(entry.fromPort <= pPortFilter->toPort))||
		((entry.toPort >= pPortFilter->fromPort) &&
		(entry.toPort <= pPortFilter->toPort)))&&
		((entry.protoType == pPortFilter->protoType)||
		((entry.protoType==PROTO_BOTH)&&pPortFilter->protoType==PROTO_UDP)||
		((entry.protoType==PROTO_BOTH)&&pPortFilter->protoType==PROTO_TCP)||
		((entry.protoType==PROTO_TCP)&&pPortFilter->protoType==PROTO_BOTH)||
		((entry.protoType==PROTO_UDP)&&pPortFilter->protoType==PROTO_BOTH)))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d port overlap!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}	
	
	apmib_set(MIB_PORTFILTER_DEL, (void *)pPortFilter);
	if ( apmib_set(MIB_PORTFILTER_ADD, (void *)pPortFilter) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d port filter set mib error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}

set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_del_port_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pPortFilter: the port filter entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete port filter entry 
*
***************************************************/
int rtk_del_port_filter_entry(RTK_PORTFILTER_T *pPortFilter, unsigned int delall)
{
	int ret = RTK_SUCCESS;
	RTK_PORTFILTER_T entry;

	memset((void *)&entry, '\0', sizeof(entry));
	if (delall)
	{
		if (apmib_set(MIB_PORTFILTER_DELALL, (void *)&entry) == 0) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d port filter delete all error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}
	else
	{
		if (!pPortFilter)
			return RTK_FAILED;

		if (!apmib_set(MIB_PORTFILTER_DEL, (void *)pPortFilter)) 
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d port filter delete an entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}		
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_set_ip_filter_enable
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the ip filter function or not
*				 1: enable, 0:disable
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set ip filter enable 
*
***************************************************/
int rtk_set_ip_filter_enable(unsigned int enabled)
{
	int ret = RTK_SUCCESS;
	
	if (!apmib_set(MIB_IPFILTER_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set ip filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_add_ip_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pIpFilter: the ip filter entry to add
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add ip filter entry 
*
***************************************************/
int rtk_add_ip_filter_entry(RTK_IPFILTER_T *pIpFilter)
{
	int ret = RTK_SUCCESS;
	int number = 0, i, flag = 0;
	RTK_IPFILTER_T entry;
	unsigned int ipaddr = 0;
		
	if (!pIpFilter)
		return RTK_FAILED;
	
	apmib_get(MIB_IPFILTER_TBL_NUM,(void*)&number);
	if(number >= MAX_FILTER_NUM){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d ip filter number shoule not be more than %d\n",__FUNCTION__, __LINE__, MAX_FILTER_NUM);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	//subnet check
	ipaddr=(*((struct in_addr *)pIpFilter->ipAddr)).s_addr;
	rtk_within_subnet_check(ipaddr, &flag);
	if (!flag)
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d Invalid IP address ip=0x%x! It should be set within the current subnet.\n", __FUNCTION__, __LINE__, ipaddr);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}	
	// valid check
	for (i=1; i<=number; i++) {
		memset((void *)&entry, '\0', sizeof(entry));
		*((char *)&entry) = (char)i;
		if ( !apmib_get(MIB_IPFILTER_TBL, (void *)&entry)) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d Get table entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
		if (((*((unsigned int*)entry.ipAddr)) == (*((unsigned int*)pIpFilter->ipAddr)))&&
			((entry.protoType==pIpFilter->protoType)||
			(entry.protoType==PROTO_BOTH&&pIpFilter->protoType==PROTO_TCP)||
			(entry.protoType==PROTO_BOTH&&pIpFilter->protoType==PROTO_UDP)||
			(entry.protoType==PROTO_TCP&&pIpFilter->protoType==PROTO_BOTH)||
			(entry.protoType==PROTO_UDP&&pIpFilter->protoType==PROTO_BOTH)))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d rule already exist!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}	
	
	apmib_set(MIB_IPFILTER_DEL, (void *)pIpFilter);
	if ( apmib_set(MIB_IPFILTER_ADD, (void *)pIpFilter) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d ip filter set mib error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}

set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_del_ip_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pIpFilter: the ip filter entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete ip filter entry 
*
***************************************************/
int rtk_del_ip_filter_entry(RTK_IPFILTER_T *pIpFilter, unsigned int delall)
{
	int ret = RTK_SUCCESS;
	RTK_IPFILTER_T entry;

	memset((void *)&entry, '\0', sizeof(entry));
	if (delall)
	{
		if (apmib_set(MIB_IPFILTER_DELALL, (void *)&entry) == 0) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d ip filter delete all error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}
	else
	{
		if (!pIpFilter)
			return RTK_FAILED;

		if (!apmib_set(MIB_IPFILTER_DEL, (void *)pIpFilter)) 
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d ip filter delete an entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}		
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_set_terminal_ip
* 
* @PARAMETERS:
* 	@Input
* 		terminal ip string
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set terminal ip info
*
***************************************************/

unsigned char ip_str[32]; 
void rtk_set_terminal_ip(char *ip_addr)
{
	memset(ip_str,0,sizeof(ip_str));
	strcpy(ip_str,ip_addr);
}

/*************************************************
	check mac/ip pair weather valid.
	return value
	1:	ip&mac pair valid
	0:	ip&mac pair invalid
**************************************************/
int terminal_ip_mac_valid(unsigned char *mac,unsigned char *ip_str)
{	
	int index;
	int count =0;
	FILE *fp = NULL;
	unsigned char buffer[128],__ip_str[32],mac_str[32];	
	unsigned char macEntry[30];
	for(index = 0 ; index < 18; ++index)
	{
		if((mac[index]  >= 'a')  && (mac[index]<='f'))
	  		mac[index] -= 32;
	}

	sprintf(macEntry,"%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);	
	//system("cat /proc/net/arp  > /var/tmpResult");
	
	//if((fp = fopen("/var/tmpResult","r+")) != NULL)
	if((fp = fopen("/proc/net/arp","r+")) != NULL)
	{	
		while(fgets(buffer, 128, fp))
		{
			++count;
			if(count ==1)	continue;
			
			
			sscanf(buffer,"%s %*s %*s %s %*s %*s",__ip_str,mac_str);	

			for(index = 0 ; index < 18; ++index)			
			{
				if((mac_str[index]  >= 'a')  && (mac_str[index]<='f'))
					mac_str[index] -= 32;
			}
						
			if((!strcmp(macEntry,mac_str)) && (!strcmp(ip_str,__ip_str)))
			{
			
				fclose(fp);
				return 1;
			}
		}
		fclose(fp);
	}
	return 0;
}

int get_terminal_ip_by_mac(unsigned char *mac,unsigned char *ip_str)
{	
	int index;
	int count =0;
	FILE *fp = NULL;
	unsigned int ret =RTK_FAILED;
	unsigned char buffer[128],__ip_str[32],mac_str[32];	
	unsigned char macEntry[30];
	for(index = 0 ; index < 18; ++index)
	{
		if((mac[index]  >= 'a')  && (mac[index]<='f'))
	  		mac[index] -= 32;
	}

	sprintf(macEntry,"%02X:%02X:%02X:%02X:%02X:%02X", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);	
	//system("cat /proc/net/arp  > /var/tmpResult");
	
	//if((fp = fopen("/var/tmpResult","r+")) != NULL)
	if((fp = fopen("/proc/net/arp","r+")) != NULL)
	{	
		while(fgets(buffer, 128, fp))
		{
			++count;
			if(count ==1)	continue;
			
			
			sscanf(buffer,"%s %*s %*s %s %*s %*s",__ip_str,mac_str);	

			for(index = 0 ; index < 18; ++index)			
			{
				if((mac_str[index]  >= 'a')  && (mac_str[index]<='f'))
					mac_str[index] -= 32;
			}
						
			if((!strcmp(macEntry,mac_str)) )
			{
				strcpy(ip_str,__ip_str);
				fclose(fp);
				ret=RTK_SUCCESS;
				goto out;
			}
		}
		fclose(fp);
	}
out:	
	return ret;
}


int  direct_set_mac_filter_rule(RTK_MACFILTER_T *pMacFilter)
{
	char macEntry[30];
	char cmdbuf[128];
	int ret=RTK_SUCCESS;
	unsigned char _ip_str[32]={0};
	//add iptables
	memset(&macEntry,0,sizeof(macEntry));
	sprintf(macEntry,"%02X:%02X:%02X:%02X:%02X:%02X", 
	pMacFilter->macAddr[0], pMacFilter->macAddr[1], pMacFilter->macAddr[2], 
	pMacFilter->macAddr[3], pMacFilter->macAddr[4], pMacFilter->macAddr[5]);

	//printf("%s.%d.start set mac filter firewall\n",__FUNCTION__,__LINE__);
	#if 0
	memset(cmdbuf,0,sizeof(cmdbuf));
	sprintf(cmdbuf,"iptables -I INPUT 1 -m mac --mac-source %s -p tcp ! --dport 80 -j DROP",macEntry);
	system(cmdbuf);
	#endif
	
	memset(cmdbuf,0,sizeof(cmdbuf));
	sprintf(cmdbuf,"iptables -I FORWARD  -m mac --mac-source %s -j DROP",macEntry);	
	system(cmdbuf);
	
	if(!terminal_ip_mac_valid(pMacFilter->macAddr,ip_str)){
		ret=get_terminal_ip_by_mac(pMacFilter->macAddr,_ip_str);
		if(ret==RTK_FAILED){
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d get_terminal_ip_by_mac!\n", __FUNCTION__, __LINE__);
			#endif
			return ret ;
		}
		else
		{
			rtk_set_terminal_ip(_ip_str);
		}
	}
	
//	printf("%s.%d.start flush kernel info. ip_str(%s).\n",__FUNCTION__,__LINE__,ip_str);
	/*flush fastpath & hw session*/	
	memset(cmdbuf,0,sizeof(cmdbuf));
	sprintf(cmdbuf,"echo clear %s > /proc/fast_flush",ip_str);
	system(cmdbuf);
	
	return ret;
}

int  direct_del_mac_filter_rule(RTK_MACFILTER_T *pMacFilter)
{
	char macEntry[30];
	char cmdbuf[128];
	int ret=RTK_SUCCESS;
	unsigned char _ip_str[32]; 
	
	memset(&macEntry,0,sizeof(macEntry));
	sprintf(macEntry,"%02X:%02X:%02X:%02X:%02X:%02X", 
	pMacFilter->macAddr[0], pMacFilter->macAddr[1], pMacFilter->macAddr[2], 
	pMacFilter->macAddr[3], pMacFilter->macAddr[4], pMacFilter->macAddr[5]);

	//printf("%s.%d.start set mac filter firewall\n",__FUNCTION__,__LINE__);
	#if 0
	memset(cmdbuf,0,sizeof(cmdbuf));
	//iptables -D INPUT -m mac --mac-source 00:40:05:40:39:37 -p tcp ! --dport 80 -j DROP
	sprintf(cmdbuf,"iptables -D INPUT  -m mac --mac-source %s -p tcp ! --dport 80 -j DROP",macEntry);
	system(cmdbuf);
	#endif
	memset(cmdbuf,0,sizeof(cmdbuf));
	//iptables -D FORWARD   -m mac --mac-source 00:40:05:40:39:37 -j DROP
	sprintf(cmdbuf,"iptables -D FORWARD -m mac --mac-source %s -j DROP",macEntry);
	system(cmdbuf);
	
	if(!terminal_ip_mac_valid(pMacFilter->macAddr,ip_str)){
		ret=get_terminal_ip_by_mac(pMacFilter->macAddr,_ip_str);
		if(ret==RTK_SUCCESS)
			rtk_set_terminal_ip(_ip_str);
		else{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d get_terminal_ip_by_mac fail!\n", __FUNCTION__, __LINE__);
			#endif
			return ret;
		}	
	}

//	printf("%s.%d.start flush kernel info. ip_str(%s).\n",__FUNCTION__,__LINE__,ip_str);
	/*flush fastpath & hw session*/	
	memset(cmdbuf,0,sizeof(cmdbuf));
	sprintf(cmdbuf,"echo enable %s > /proc/fast_flush",ip_str);
	system(cmdbuf);	
	return ret;
}


/**************************************************
* @NAME:
* 	rtk_set_mac_filter_enable
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the mac filter function or not
*				 1: enable, 0:disable
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set mac filter enable 
*
***************************************************/
int rtk_set_mac_filter_enable(unsigned int enabled)
{
	int ret = RTK_SUCCESS;
	
	if (!apmib_set(MIB_MACFILTER_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set mac filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_add_mac_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pMacFilter: the mac filter entry to add
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add mac filter entry 
*
***************************************************/
int rtk_add_mac_filter_entry(RTK_MACFILTER_T *pMacFilter)
{
	int ret = RTK_SUCCESS;
	int number = 0, i;
	RTK_MACFILTER_T entry;
		
	if (!pMacFilter)
	{
#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d mac filter entry is null!\n",__FUNCTION__, __LINE__);
#endif
		return RTK_FAILED;
	}
	
	apmib_get(MIB_MACFILTER_TBL_NUM,(void*)&number);
	if(number >= MAX_FILTER_NUM){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d mac filter number shoule not be more than %d\n",__FUNCTION__, __LINE__, MAX_FILTER_NUM);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	//valid check
	if (!memcmp(pMacFilter->macAddr, "\x00\x00\x00\x00\x00\x00", 6))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d mac filter invalid mac address!\n",__FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	// add same mac check
	for(i = 1;i <= number;i++)
	{
		memset((void *)&entry, '\0', sizeof(entry));
		*((char *)&entry) = (char)i;
		if ( apmib_get(MIB_MACFILTER_TBL, (void *)&entry))
		{
			if (equal_mac(entry.macAddr, pMacFilter->macAddr)==RTK_SUCCESS)
			{
				#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
				printf("%s %d rule already exist!\n", __FUNCTION__, __LINE__);
				#endif
				ret = RTK_FAILED;
				goto set;
			}
				
		}
	}
		
	apmib_set(MIB_MACFILTER_DEL, (void *)pMacFilter);
	if ( apmib_set(MIB_MACFILTER_ADD, (void *)pMacFilter) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d mac filter set mib error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set:
	/*set firwall & flush nf_conntrack & fastpath hw info*/
	direct_set_mac_filter_rule(pMacFilter);
set_error:
	return ret;
}


/**************************************************
* @NAME:
* 	rtk_del_mac_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pMacFilter: the mac filter entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete mac filter entry 
*
***************************************************/
int rtk_del_mac_filter_entry(RTK_MACFILTER_T *pMacFilter, unsigned int delall)
{
	int ret = RTK_SUCCESS;
	int number=0;
	int i=0;
	RTK_MACFILTER_T entry;

	memset((void *)&entry, '\0', sizeof(entry));
	if (delall)
	{
		apmib_get(MIB_MACFILTER_TBL_NUM,(void*)&number);
		if(number>MAX_FILTER_NUM)
		{
			
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d invalid mac filter table number!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
		for (i = 1; i <= number; i++)
		{
			memset((void *)&entry, '\0', sizeof(RTK_MACFILTER_T));
			*((char *)&entry) = (char)(i);
			if (apmib_get(MIB_MACFILTER_TBL, (void *)&entry))
			{
				/*set firwall & flush nf_conntrack & fastpath hw info*/
				direct_del_mac_filter_rule(&entry);
			}
		}
		if (!apmib_set(MIB_MACFILTER_DELALL, (void *)&entry)) 
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d mac filter delete all entry error!\n",__FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}
	else
	{
		if (!pMacFilter)
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d mac filter entry is null!\n",__FUNCTION__, __LINE__);
			#endif
			return RTK_FAILED;
		}
		
		/*set firwall & flush nf_conntrack & fastpath hw info*/
		memcpy(&entry,pMacFilter,sizeof(RTK_MACFILTER_T));
		direct_del_mac_filter_rule(&entry);	
		if (!apmib_set(MIB_MACFILTER_DEL, (void *)pMacFilter)) 
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d mac filter delete an entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}	
		
	}
		
set_error:
	return ret;
}

int rtk_add_macfilter_rule_entry_imm_inband(RTK_MACFILTER_T *pMacFilter)
{
	int ret = RTK_FAILED;
	int enabled=0;
	unsigned char ipstr[32]; 
	
	rtk_get_mac_filter_enable(&enabled);
	if(enabled ==0)
	{
		rtk_set_mac_filter_enable(1);
	}
	
	ret=rtk_add_mac_filter_entry(pMacFilter);
	if(ret ==RTK_SUCCESS)
	{
		apmib_update(CURRENT_SETTING);
	}
	
set_error:
	return ret;
}

int rtk_del_macfilter_rule_entry_imm_inband(RTK_MACFILTER_T *pMacFilter, int delall)
{
	int ret = RTK_FAILED;

	unsigned char ipstr[32]; 
	
	ret=rtk_del_mac_filter_entry(pMacFilter,delall);	
	if(ret ==RTK_SUCCESS)
	{
		apmib_update(CURRENT_SETTING);
	}
set_error:
	
	return ret;
}

int rtk_find_macfilter_rule_by_mac(RTK_MACFILTER_T *pMacFilter)
{
	int ret = RTK_FAILED;
	int i=0,number;
	unsigned char ipstr[32]; 
	RTK_MACFILTER_T entry;
	
	apmib_get(MIB_MACFILTER_TBL_NUM,(void*)&number);
	if(number >= MAX_FILTER_NUM)
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d mac filter number shoule not be more than %d\n",__FUNCTION__, __LINE__, MAX_FILTER_NUM);
		#endif
		ret = RTK_FAILED;
		goto out;
	}
	
	for(i = 1;i <= number;i++)
	{
		memset((void *)&entry, '\0', sizeof(entry));
		*((char *)&entry) = (char)i;
		if ( apmib_get(MIB_MACFILTER_TBL, (void *)&entry))
		{
			if (equal_mac(entry.macAddr, pMacFilter->macAddr)==RTK_SUCCESS)
			{
				#if 0//defined(RTK_FIREWALL_ADAPTER_DEBUG)
				printf("%s %d find mac related macfilter rule!\n", __FUNCTION__, __LINE__);
				#endif
				ret = i;
				goto out;
			}
		}
	}
	
out:
	
	return ret;
}


/**************************************************
* @NAME:
* 	rtk_set_nas_filter_enable
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the nas filter function or not
*				 1: enable, 0:disable
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set nas filter enable 
*
***************************************************/
int rtk_set_nas_filter_enable(unsigned int enabled)
{
	int ret = RTK_SUCCESS;

	if (!apmib_set(MIB_NASFILTER_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set nas filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}

set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_nas_filter_enable
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the mac filter function or not
*				 1: enable, 0:disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get nas filter enable status 
*
***************************************************/
int rtk_get_nas_filter_enable(unsigned int *enabled)
{
	int ret = RTK_SUCCESS;
	int value =0;

	if (!enabled)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_NASFILTER_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get nas filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;

get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_add_nas_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pMacFilter: the nas filter entry to add
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add nas filter entry 
*
***************************************************/
int rtk_add_nas_filter_entry(RTK_NASFILTER_T *pNasFilter)
{
	int ret = RTK_SUCCESS;
	int number = 0, i;
	RTK_NASFILTER_T entry;
		
	if (!pNasFilter)
		return RTK_FAILED;
	
	apmib_get(MIB_NASFILTER_TBL_NUM,(void*)&number);
	if(number >= MAX_FILTER_NUM){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d nas filter number shoule not be more than %d\n",__FUNCTION__, __LINE__, MAX_FILTER_NUM);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	//valid check
	if (!memcmp(pNasFilter->macAddr, "\x00\x00\x00\x00\x00\x00", 6))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d nas filter invalid mac address!\n",__FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	// add same mac check
	for(i = 1;i <= number;i++)
	{
		memset((void *)&entry, '\0', sizeof(entry));
		*((char *)&entry) = (char)i;
		if ( apmib_get(MIB_NASFILTER_TBL, (void *)&entry))
		{
			if (!memcmp(entry.macAddr, pNasFilter->macAddr, 6))
			{
				#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
				printf("%s %d rule already exist!\n", __FUNCTION__, __LINE__);
				#endif
				ret = RTK_FAILED;
				goto set_error;
			}
				
		}
	}

	//add it to nas server before add to mib

		
		//add new entry to the list		


	
	apmib_set(MIB_NASFILTER_DEL, (void *)pNasFilter);
	if ( apmib_set(MIB_NASFILTER_ADD, (void *)pNasFilter) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d nas filter set mib error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set_error:
	return ret;
}
/**************************************************
* @NAME:
* 	rtk_del_nas_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pNasFilter: the nas filter entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete nas filter entry 
*
***************************************************/
int rtk_del_nas_filter_entry(RTK_NASFILTER_T *pNasFilter, unsigned int delall)
{
	int ret = RTK_SUCCESS;
	RTK_NASFILTER_T entry;
	
	memset((void *)&entry, '\0', sizeof(entry));
	if (delall)
	{
		//deleta all from nas server before from mib
		if (apmib_set(MIB_NASFILTER_DELALL, (void *)&entry) == 0) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d nas filter delete all error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}
	else
	{
		if (!pNasFilter)
			return RTK_FAILED;
		//deleta the entry from nas server before from mib
			
			//not add the entry mac to the cmd line	

		if (!apmib_set(MIB_NASFILTER_DEL, (void *)pNasFilter)) 
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d nas filter delete an entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}		
	}
	
set_error:	
	return ret;
}
/**************************************************
* @NAME:
* 	rtk_get_nas_filter_entry
* 
* @PARAMETERS:
* 	@Input
*		empty_entry_num: the number of pNasFilter 
* 	@Output
*	 	num: the actual number of nas filter table
* 		pNasFilter: store the nas filter entry
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get nas filter entry 
*
***************************************************/
int rtk_get_nas_filter_entry(unsigned int *num, RTK_NASFILTER_T *pNasFilter, unsigned int empty_entry_num)
{
	int number = 0, count = 0, i = 0;
	int ret = RTK_SUCCESS;
	
	if (!pNasFilter)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_NASFILTER_TBL_NUM,(void*)&number))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d nas filter get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	
	*num = number;
	
	count = (number >= empty_entry_num)?empty_entry_num:number;
	for (i = 0; i < count; i++)
	{
		memset((void *)&pNasFilter[i], '\0', sizeof(pNasFilter[i]));
		*((char *)&pNasFilter[i]) = (char)(i+1);
		if ( !apmib_get(MIB_NASFILTER_TBL, (void *)&pNasFilter[i]))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d nas filter get tbl error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
	}
get_error:
	
	return ret;
}


static int charInString(char c,const char*str)
{
	int i=0,len=strlen(str);
	for(i=0;i<len;i++)
		if(c==str[i])
		{			
			return 1;
		}
	return 0;
}

static int changeDividerToESC(char *src,unsigned int size,const char*dividerChars)
{
	int srclen=0,i=0,j=0;
	if(!src||!dividerChars)
	{
		printf("%s : input value is null!\n",__FUNCTION__);
		return -1;
	}
	srclen=strlen(src);
	if(srclen>=size)
	{
		printf("%s : invalid input value!\n",__FUNCTION__);
		return -1;
	}
	for(i=srclen-1;i>=0;i--)
	{
		if(charInString(src[i],dividerChars))
		{
			srclen++;
			if(srclen>=size)
			{
				printf("%s : over size!\n",__FUNCTION__);
				return -1;
			}
			for(j=srclen-1;j>i;j--)
			{
				src[j]=src[j-1];
			}
			//assert(j==i);
			src[j]='\\';			
		}
	}
	return 0;
}

int rtk_convert_url_address(unsigned char *org_url, unsigned char *new_org)
{
	int ret = RTK_SUCCESS, i = 0;
	unsigned char tmp1[64]={0};
	
	if (!org_url || !new_org)
		return RTK_FAILED;
	
	strcpy(tmp1,org_url);
	if(!strncmp(tmp1,"http://",7))
	{
		for(i=7;i<sizeof(tmp1);i++)
			tmp1[i-7]=tmp1[i];
	}
	//printf("%s:%d tmp1=%s\n",__FUNCTION__,__LINE__,tmp1);
	
	if(!strncmp(tmp1,"www.",4))
	{
		for(i=4;i<sizeof(tmp1);i++)
			tmp1[i-4]=tmp1[i];
	}

	if(changeDividerToESC(tmp1,sizeof(tmp1)," #:\\")<0)
		return -1;
	//printf("%s:%d tmp1=%s\n",__FUNCTION__,__LINE__,tmp1);
	
	sprintf(tmp1, "%s ;", tmp1);
	//printf("%s:%d tmp1=%s\n",__FUNCTION__,__LINE__,tmp1);

	strcpy(new_org, tmp1);

	return ret;
}

/**************************************************
* @NAME:
* 	rtk_url_filter_take_effect_cmd
* 
* @PARAMETERS:
* 	@Input
* 		pUrlFilter: the url filter entry to handle
*		cmd: command type
* 		1: add an entry
* 		2: delete an entry
* 		3: flush all entry
* 		4: init url table
* 		5: enable white list
* 		6: enable black list
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	url filter take effect command handle 
*
***************************************************/
int rtk_url_filter_take_effect_cmd(RTK_URLFILTER_T *pUrlFilter, int cmd)
{
	int ret = RTK_SUCCESS;
	unsigned char keywords[64] = {0};
	unsigned char tmp_buff[64] = {0};
	unsigned char cmdbuffer[128] = {0};
	

	memset(keywords, '\0', sizeof(keywords));
	memset(cmdbuffer, '\0', sizeof(cmdbuffer));
	memset(tmp_buff, '\0', sizeof(tmp_buff));
	
	switch (cmd)
	{
		case 1:
			/* echo "add:0#3 3 url1 ;url2;" > /proc/filter_table */
			rtk_convert_url_address(pUrlFilter->urlAddr, keywords);
			sprintf(tmp_buff, "\"add:0#3 3 %s\"", keywords);			
			break;
		case 2:
			/* echo "del:0#3 3 url1 ;url2;" > /proc/filter_table */
			rtk_convert_url_address(pUrlFilter->urlAddr, keywords);
			sprintf(tmp_buff, "\"del:0#3 3 %s\"", keywords);			
			break;
		case 3:
			/* echo flush > /proc/filter_table */
			sprintf(tmp_buff, "%s", "flush");			
			break;
		case 4:
			/* echo init 3 > /proc/filter_table */
			sprintf(tmp_buff, "%s", "init 3");
			break;
		case 5:
			/* echo "white" > /proc/filter_table */
			sprintf(tmp_buff, "%s", "white");
			break;
			
		case 6:
			/* echo "black" > /proc/filter_table */
			sprintf(tmp_buff, "%s", "black");
			break;
			
		default:
			break;
	}

	if (tmp_buff[0])
	{
		sprintf(cmdbuffer, "echo %s >/proc/filter_table",tmp_buff);
		//printf("%s %d cmdbuffer=%s\n", __FUNCTION__, __LINE__, cmdbuffer);
		system(cmdbuffer);
	}
	
	#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
	if (pUrlFilter)
		printf("%s %d pUrlFilter->urlAddr=%s cmd=%d \n", __FUNCTION__, __LINE__, pUrlFilter->urlAddr, cmd);
	else
		printf("%s %d cmd=%d \n",__FUNCTION__, __LINE__,cmd);
	#endif
	
	return ret;
}

int rtk_set_url_tbl_take_effect(void)
{
	char keywords[500] = {0};
	char cmdBuffer[500]= {0};
	char macAddr[30] = {0};
	char tmp1[64]={0};
	RTK_URLFILTER_T entry;
	int entryNum=0, index;
	int mode,i=0, enable = 0;
	char c = 22;	//unseen char to distinguish
	
	/*add URL filter Mode 0:Black list 1:White list*/	
	apmib_get(MIB_URLFILTER_ENABLED, (void *)&enable);
	if (!enable)
		return 0;
	
	apmib_get(MIB_URLFILTER_MODE,  (void *)&mode);
	apmib_get(MIB_URLFILTER_TBL_NUM, (void *)&entryNum);
	//sprintf(keywords, "%d ", entryNum);
	bzero(keywords,sizeof(keywords));
	for (index=1; index<=entryNum; index++) {
		memset(&entry, '\0', sizeof(entry));
		bzero(tmp1,sizeof(tmp1));
		*((char *)&entry) = (char)index;
		apmib_get(MIB_URLFILTER_TBL, (void *)&entry);
		if(mode!=entry.ruleMode)
			continue;

		
		rtk_convert_url_address(entry.urlAddr, tmp1);

#if defined(CONFIG_RTL_FAST_FILTER)
		memset(cmdBuffer, 0, sizeof(cmdBuffer));
		sprintf(cmdBuffer, "rtk_cmd filter add --url-key %s", tmp1);
		system(cmdBuffer);
#else
		strcat(keywords, tmp1);
#endif
	}
	
	if(mode)		
		rtk_url_filter_take_effect_cmd(NULL, 5);	
	else
		rtk_url_filter_take_effect_cmd(NULL, 6);

#if defined(CONFIG_RTL_FAST_FILTER)
#else
	sprintf(cmdBuffer, "echo \"add:0#3 3 %s\" > /proc/filter_table",keywords);
	system(cmdBuffer);
#endif

	return 0;
}

/**************************************************
* @NAME:
* 	rtk_set_url_filter_enable
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the url filter function or not
*				 1: enable, 0:disable
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set url filter enable 
*
***************************************************/
int rtk_set_url_filter_enable(unsigned int enabled)
{
	int ret = RTK_SUCCESS;
	
	if (!apmib_set(MIB_URLFILTER_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set url filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}

	if (enabled)
	{
		//if enabled, disable hw nat->flush first->init url table
		system("echo 0 > /proc/hw_nat");
		rtk_url_filter_take_effect_cmd(NULL, 3);
		rtk_url_filter_take_effect_cmd(NULL, 4);
	}
	else
	{
		//if disabled, flush->enable hw nat
		rtk_url_filter_take_effect_cmd(NULL, 3);
		system("echo 1 > /proc/hw_nat");
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_set_url_filter_mode
* 
* @PARAMETERS:
* 	@Input
* 		url_filter_mode: url filter mode
*					     1: white list, 0: black list
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set url filter mode 
*
***************************************************/
int rtk_set_url_filter_mode(int url_filter_mode)
{
	int ret = RTK_SUCCESS;
	
	if ( apmib_set(MIB_URLFILTER_MODE, (void *)&url_filter_mode) == 0) 
	{
		ret = RTK_FAILED;
	}

	rtk_set_url_tbl_take_effect();
	
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_add_url_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pUrlFilter: the url filter entry to add
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add url filter entry 
*
***************************************************/
int rtk_add_url_filter_entry(RTK_URLFILTER_T *pUrlFilter)
{
	int ret = RTK_SUCCESS;
	int number = 0, i;
	RTK_URLFILTER_T entry;
		
	if (!pUrlFilter)
		return RTK_FAILED;
	
	apmib_get(MIB_URLFILTER_TBL_NUM,(void*)&number);
	if(number >= MAX_URLFILTER_NUM){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d url filter number shoule not be more than %d\n",__FUNCTION__, __LINE__, MAX_URLFILTER_NUM);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
	//dumplicate check
	for(i = 1;i <= number;i++)
	{
		memset(&entry,'\0',sizeof(RTK_URLFILTER_T));
		*((char*)&entry) = (char)(i);
		apmib_get(MIB_URLFILTER_TBL,(void*)&entry);
		if(strlen(entry.urlAddr) == strlen(pUrlFilter->urlAddr)
			&& !strncmp(entry.urlAddr,pUrlFilter->urlAddr,strlen(pUrlFilter->urlAddr))
			&& (entry.ruleMode == pUrlFilter->ruleMode))
		{			
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d url filter rule already exist!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}	
	
	apmib_set(MIB_URLFILTER_DEL, pUrlFilter);
	if ( apmib_set(MIB_URLFILTER_ADD, pUrlFilter) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d url filter set mib error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
	rtk_url_filter_take_effect_cmd(pUrlFilter, 1);

set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_del_url_filter_entry
* 
* @PARAMETERS:
* 	@Input
* 		pUrlFilter: the url filter entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete url filter entry 
*
***************************************************/
int rtk_del_url_filter_entry(RTK_URLFILTER_T *pUrlFilter,unsigned int delall)
{
	int ret = RTK_SUCCESS;
	RTK_URLFILTER_T entry;

	memset((void *)&entry, '\0', sizeof(entry));
	switch(delall)
	{
		case 0:
			if (!pUrlFilter)
	                        return RTK_FAILED;

        		if (!apmib_set(MIB_URLFILTER_DEL, (void *)pUrlFilter))
               		{
                        	#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
                        	printf("%s %d url filter delete an entry error!\n", __FUNCTION__, __LINE__);
                        	#endif
                        	ret = RTK_FAILED;
                        	goto set_error;
            		}
				
			rtk_url_filter_take_effect_cmd(pUrlFilter, 2);
			break;
		case 1://delete all black items
		case 2://delete all white items
		{
			int num=0,i=0;
			if(!apmib_get(MIB_URLFILTER_TBL_NUM,(void*)&num))
			{
				#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
                        	printf("%s %d get MIB_URLFILTER_TBL_NUM error!\n", __FILE__, __LINE__);
                        	#endif
			}
			for(i=num;i>0;i--)
			{
				*((char *)&entry) = (char)i;
				apmib_get(MIB_URLFILTER_TBL,(void*)&entry);
				
				if((entry.ruleMode==1 && delall==2)//white
				 || (entry.ruleMode==0 && delall==1))//black
				{
					if (!apmib_set(MIB_URLFILTER_DEL, (void *)&entry))
		                        {
                	        	        #if defined(RTK_FIREWALL_ADAPTER_DEBUG)
                		                printf("%s %d url filter delete an entry error!\n", __FUNCTION__, __LINE__);
        	                	        #endif
	                                	ret = RTK_FAILED;
	                                	goto set_error;
        	                	}				
				rtk_url_filter_take_effect_cmd(pUrlFilter, 2);
				}
			}
			break;
		}
		case 3://delete all items
			if (apmib_set(MIB_URLFILTER_DELALL, (void *)&entry) == 0) {
                        #if defined(RTK_FIREWALL_ADAPTER_DEBUG)
                        printf("%s %d url filter delete all error!\n", __FUNCTION__, __LINE__);
                        #endif
                        ret = RTK_FAILED;
                        goto set_error;
                }
			
			rtk_url_filter_take_effect_cmd(NULL, 3);
			rtk_url_filter_take_effect_cmd(NULL, 4);
			break;	

		
	}

set_error:
	return ret;
}


/**************************************************
* @NAME:
* 	rtk_set_dmz
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the dmz function or not
*				 1: enable, 0:disable
*		dmz_ip: the dmz host ip address
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set dmz enable and dmz host ip address 
*
***************************************************/
int rtk_set_dmz(unsigned int enabled, unsigned int dmz_ip)
{
	int ret = RTK_SUCCESS;
	int flag = 0;
		
	if (!apmib_set(MIB_DMZ_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set dmz enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	//subnet check
	rtk_within_subnet_check(dmz_ip, &flag);
	if (!flag)
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d Invalid IP address dmz_ip=0x%x! It should be set within the current subnet.\n", __FUNCTION__, __LINE__, dmz_ip);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}	
	if (!apmib_set(MIB_DMZ_HOST,  (void *)&dmz_ip))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set dmz host error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set_error:
	return ret;
}

#if defined(VLAN_CONFIG_SUPPORTED)
/**************************************************
* @NAME:
* 	rtk_set_vlan_enable
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the vlan function or not
*				 1: enable, 0:disable
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set vlan enable 
*
***************************************************/
int rtk_set_vlan_enable(unsigned int enabled)
{
	int ret = RTK_SUCCESS;
	
	if (!apmib_set(MIB_VLANCONFIG_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set vlan enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_modify_vlan_config
* 
* @PARAMETERS:
* 	@Input
* 		pVlanConfig: the vlan entry to modify
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	modify already exists vlan entry
*	Note: Fill in each member of the structure(RTK_VLAN_CONFIG_T) 
*		   when modify already exists vlan entry
*	
***************************************************/
int rtk_modify_vlan_config(RTK_VLAN_CONFIG_T *pVlanConfig)
{
	int ret = RTK_SUCCESS;
	int i;
	RTK_VLAN_CONFIG_T entry[2] = {0};
	unsigned int ip1, ip2, ips, ipe;
	
	if (!pVlanConfig)
		return RTK_FAILED;
	
	for(i = 1;i <= MAX_IFACE_VLAN_CONFIG;i++)
	{
		memset((void *)entry,'\0',sizeof(entry));
		*((char*)&entry) = (char)(i);
		apmib_get(MIB_VLANCONFIG_TBL,(void*)&entry);
		if(!memcmp(entry[0].netIface, pVlanConfig->netIface, strlen(pVlanConfig->netIface)))
		{
			//memcpy(&(entry[1]), &(entry[0]), sizeof(RTK_VLAN_CONFIG_T));
			memcpy((void *)&entry[1], (void *)pVlanConfig, sizeof(RTK_VLAN_CONFIG_T));
			if(apmib_set(MIB_VLANCONFIG_MOD,(void*)entry)==0)
			{
				#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
				printf("%s %d vlan tbl modify error!\n",__FUNCTION__, __LINE__);
				#endif
				ret = RTK_FAILED;
				goto set_error;
			}
			break;
		}
	}
	if (i > MAX_IFACE_VLAN_CONFIG)
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d can not find exist vlan entry! netIface=%s\n",__FUNCTION__, __LINE__, pVlanConfig->netIface);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}

set_error:
	return ret;
}

#endif

#if defined(QOS_BY_BANDWIDTH)
/**************************************************
* @NAME:
* 	rtk_set_qos_enable
* 
* @PARAMETERS:
* 	@Input
*	 	enabled: indicate whether enable the qos function or not
*				 1: enable, 0:disable
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set qos enable 
*
***************************************************/
int rtk_set_qos_enable(unsigned int enabled)
{
	int ret = RTK_SUCCESS;
	
	if (!apmib_set(MIB_QOS_ENABLED, (void *)&enabled))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set qos enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_set_qos_speed
* 
* @PARAMETERS:
* 	@Input
*	 	uplink_speed: uplink speed
*					  0:means auto speed, other means manual speed
*	 	downlink_speed: downlink speed
*					  0:means auto speed, other means manual speed
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set qos speed 
*
***************************************************/
int rtk_set_qos_speed(unsigned int uplink_speed, unsigned downlink_speed) //0 is auto, unit:kbps
{
	int ret = RTK_SUCCESS;
	int value = 0;
	
	if (!uplink_speed)
	{
		//auto speed
		value = 1;
	}
	else
	{
		//manual speed
		value = 0;		
	}
	if ( apmib_set( MIB_QOS_AUTO_UPLINK_SPEED, (void *)&value) == 0) 
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set qos auto uplink error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	if (!value)
	{
		if ( apmib_set( MIB_QOS_MANUAL_UPLINK_SPEED, (void *)&uplink_speed) == 0) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d set qos manual uplink error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}

	if (!downlink_speed)
	{
		//auto speed
		value = 1;
	}
	else
	{
		//manual speed
		value = 0;		
	}
	if ( apmib_set( MIB_QOS_AUTO_DOWNLINK_SPEED, (void *)&value) == 0) 
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d set qos auto downlink error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	if (!value)
	{
		if ( apmib_set( MIB_QOS_MANUAL_DOWNLINK_SPEED, (void *)&downlink_speed) == 0) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d set qos manual downlink error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}
	
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_modify_qos_rule_entry
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry to modify
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	modify already exists qos entry
*	Note: only support modify uplink bandwidth and downlink bandwidth
*	
***************************************************/
int rtk_modify_qos_rule_entry(RTK_IPQOS_T *pQos)
{
	int ret = RTK_SUCCESS;
	int number = 0, i;
	RTK_IPQOS_T entry[2] = {0};
	unsigned int ip1, ip2, ips, ipe;
	
	if (!pQos)
		return RTK_FAILED;
	
	apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number);
	for(i = 1;i <= number;i++)
	{
		memset((void *)entry,'\0',sizeof(entry));
		*((char*)&entry) = (char)(i);
		apmib_get(MIB_QOS_RULE_TBL,(void*)&entry);
		if(pQos->mode & QOS_RESTRICT_MAC)
		{
			if(entry[0].mode & QOS_RESTRICT_MAC)
			{
				/*printf("[%s]:[%d]%02x%02x%02x%02x%02x%02x\n",__FUNCTION__,__LINE__,
				entry.mac[0],entry.mac[1],entry.mac[2],entry.mac[3],entry.mac[4],entry.mac[5]);*/
				if((entry[0].mac[0]==pQos->mac[0])&&(entry[0].mac[1]==pQos->mac[1])
				&&(entry[0].mac[2]==pQos->mac[2])&&(entry[0].mac[3]==pQos->mac[3])
				&&(entry[0].mac[4]==pQos->mac[4])&&(entry[0].mac[5]==pQos->mac[5]))
				{
					
					memcpy(&(entry[1]), &(entry[0]), sizeof(RTK_IPQOS_T));
					//memcpy((void *)&entry[1], (void *)pQos, sizeof(RTK_IPQOS_T));
					entry[1].bandwidth = pQos->bandwidth;
					entry[1].bandwidth_downlink = pQos->bandwidth_downlink;
					if(apmib_set(MIB_QOS_MOD,(void*)entry)==0)
					{
						#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
						printf("%s %d qos modify error!\n",__FUNCTION__, __LINE__);
						#endif
						ret = RTK_FAILED;
						goto set_error;
					}
					break;
				}
				
			}
		}
		else if (pQos->mode & QOS_RESTRICT_IP)
		{
			if(entry[0].mode & QOS_RESTRICT_IP)
			{
				ip1=(*((struct in_addr *)entry[0].local_ip_start)).s_addr;
				ip2=(*((struct in_addr *)entry[0].local_ip_end)).s_addr;
				ips=(*((struct in_addr *)pQos->local_ip_start)).s_addr;
				ipe=(*((struct in_addr *)pQos->local_ip_end)).s_addr;
				if(((ip1 == ips) && (ip2 == ipe)))
				{
					memcpy(&(entry[1]), &(entry[0]), sizeof(RTK_IPQOS_T));
					//memcpy((void *)&entry[1], (void *)pQos, sizeof(RTK_IPQOS_T));
					entry[1].bandwidth = pQos->bandwidth;
					entry[1].bandwidth_downlink = pQos->bandwidth_downlink;
					if(apmib_set(MIB_QOS_MOD,(void*)entry)==0)
					{
						#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
						printf("%s %d qos modify error!\n",__FUNCTION__, __LINE__);
						#endif
						ret = RTK_FAILED;
						goto set_error;
					}
					break;
				}
				
			}
		}
	}
	
	if (i > number)
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d can not find exist qos entry!\n",__FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
set_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_qos_rule_entry_index
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry 
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	QoS table index 
*	RTK_FAILED
* 
* @FUNCTION :
* 	get qos table index of client to set qos entry
*	Note: only support based on mac or ip address
*	 
***************************************************/

int rtk_get_qos_rule_entry_index(RTK_IPQOS_T *pQos)
{
	int ret = RTK_FAILED;
	int number = 0, i=0;
	RTK_IPQOS_T entry = {0};
	unsigned int ip1, ip2, ips, ipe;
	
	if (!pQos){
	#if defined(RTK_FIREWALL_ADAPTER_DEBUG)	
		printf("pQos =NULL![%s]:[%d].\n",__FUNCTION__,__LINE__);
	#endif
		goto get_error;
	}
	
	apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number);
				
	for(i = 1;i <= number;i++)
	{
		memset(&entry,0,sizeof(entry));
		*((char*)&entry) = (char)(i);
		apmib_get(MIB_QOS_RULE_TBL,(void*)&entry);
	
		if(pQos->mode & QOS_RESTRICT_MAC)
		{
			if(entry.mode & QOS_RESTRICT_MAC)
			{
				//printf("[%s]:[%d]%02x%02x%02x%02x%02x%02x\n",__FUNCTION__,__LINE__,
				//entry.mac[0],entry.mac[1],entry.mac[2],entry.mac[3],entry.mac[4],entry.mac[5]);
				if((entry.mac[0]==pQos->mac[0])&&(entry.mac[1]==pQos->mac[1])
				&&(entry.mac[2]==pQos->mac[2])&&(entry.mac[3]==pQos->mac[3])
				&&(entry.mac[4]==pQos->mac[4])&&(entry.mac[5]==pQos->mac[5]))
				{
					
					ret=i;
					break;
				}
				
			}
		}
		else if (pQos->mode & QOS_RESTRICT_IP)
		{
			if(entry.mode & QOS_RESTRICT_IP)
			{
				ip1=(*((struct in_addr *)entry.local_ip_start)).s_addr;
				ip2=(*((struct in_addr *)entry.local_ip_end)).s_addr;
				ips=(*((struct in_addr *)pQos->local_ip_start)).s_addr;
				ipe=(*((struct in_addr *)pQos->local_ip_end)).s_addr;
				if(((ip1 == ips) && (ip2 == ipe)))
				{
					ret=i;
					break;
				}
				
			}
		}
	}
	if (i > number)
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d can not find exist qos entry!\n",__FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
get_error:
	return ret;
}



/**************************************************
* @NAME:
* 	rtk_get_qos_rule_entry_by_index
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry index
*
* 	@Output
*	 	qos_rule_entry
*
* @RETRUN:
* 	QoS table index 
*	RTK_FAILED
* 
* @FUNCTION :
* 	get qos rule entry by index of qos table
*	
*	
***************************************************/

int rtk_get_qos_rule_entry_by_index(RTK_IPQOS_T *pQos, unsigned int index)
{
	int ret = RTK_FAILED;
	int number = 0, i;
	RTK_IPQOS_T entry = {0};
	unsigned int ip1, ip2, ips, ipe;
	if (!pQos){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG) 
		printf("pQos =NULL![%s]:[%d].\n",__FUNCTION__,__LINE__);
		#endif
		goto get_error;
	}	
	
	apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number);
	if(index >number||index==0)
	{	
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)	
		printf("%s %d can not find exist qos entry!\n",__FUNCTION__, __LINE__);
		#endif
		goto get_error;
	}
	
	memset(&entry,0,sizeof(entry));
	*((char*)&entry) = (char)(index);
	if(!apmib_get(MIB_QOS_RULE_TBL,(void*)&entry))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("get qos rule failed![%s]:[%d].\n",__FUNCTION__,__LINE__);
		#endif
		goto get_error;
	}
	memcpy(pQos,&entry,sizeof(RTK_IPQOS_T));
	ret = RTK_SUCCESS;
	
get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_add_qos_rule_entry
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry to add
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add qos entry
*	Note: if value of (pQos->mode & QOS_RESTRICT_MIN) as true,
*		  first call function rtk_set_qos_speed, then call this function,
*		  due to need check uplink & downlink guaranteed minimum bandwidth.
*
***************************************************/
int rtk_add_qos_rule_entry(RTK_IPQOS_T *pQos)
{
	int ret = RTK_SUCCESS;
	int number = 0, i;
	RTK_IPQOS_T entry;
	unsigned int ip1, ip2, ips, ipe;
	unsigned long total_uplink_bw = 0, total_downlink_bw = 0;
	unsigned long tmp_total_uplink_bw = 0, tmp_total_downlink_bw = 0;
	
	if (!pQos){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG) 
		printf("pQos =NULL![%s]:[%d].\n",__FUNCTION__,__LINE__);
		#endif
		return RTK_FAILED;
	}
	
	apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number);
	if(number >= MAX_QOS_RULE_NUM){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d qos number shoule not be more than %d\n",__FUNCTION__, __LINE__, MAX_QOS_RULE_NUM);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}
	
	
	rtk_get_qos_total_speed(&total_uplink_bw, &total_downlink_bw);
	//check
	for(i = 1;i <= number;i++)
	{
		memset(&entry,'\0',sizeof(entry));
		*((char*)&entry) = (char)(i);
		apmib_get(MIB_QOS_RULE_TBL,(void*)&entry);
		if(pQos->mode & QOS_RESTRICT_MAC)
		{
			if(entry.mode & QOS_RESTRICT_MAC)
			{
				/*printf("[%s]:[%d]%02x%02x%02x%02x%02x%02x\n",__FUNCTION__,__LINE__,
				entry.mac[0],entry.mac[1],entry.mac[2],entry.mac[3],entry.mac[4],entry.mac[5]);*/
				if((entry.mac[0]==pQos->mac[0])&&(entry.mac[1]==pQos->mac[1])
				&&(entry.mac[2]==pQos->mac[2])&&(entry.mac[3]==pQos->mac[3])
				&&(entry.mac[4]==pQos->mac[4])&&(entry.mac[5]==pQos->mac[5]))
				{
					#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
					printf("%s %d qos mac address conflict\n",__FUNCTION__, __LINE__);
					#endif
					ret = RTK_FAILED;
					goto set_error;
				}
				
			}
		}
		else if (pQos->mode & QOS_RESTRICT_IP)
		{
			if(entry.mode & QOS_RESTRICT_IP)
			{
				ip1=(*((struct in_addr *)entry.local_ip_start)).s_addr;
				ip2=(*((struct in_addr *)entry.local_ip_end)).s_addr;
				ips=(*((struct in_addr *)pQos->local_ip_start)).s_addr;
				ipe=(*((struct in_addr *)pQos->local_ip_end)).s_addr;
				if(((ips >= ip1) && (ips <= ip2))
					||((ipe >= ip1) && (ipe <=ip2))
					||((ips < ip1) && (ipe > ip2)))
				{
					#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
					printf("%s %d qos ip address conflict\n",__FUNCTION__, __LINE__);
					#endif
					ret = RTK_FAILED;
					goto set_error;

				}
				
			}
		}
		
		if (pQos->mode & QOS_RESTRICT_MIN)
		{			
			if (entry.mode & QOS_RESTRICT_MIN)
			{	//Statistical bandwidth
				tmp_total_uplink_bw += entry.bandwidth;
				tmp_total_downlink_bw += entry.bandwidth_downlink;
			}
		}
	}

	if (pQos->mode & QOS_RESTRICT_MIN)
	{
		//Do check for "Guaranteed minimum bandwidth" here
		if(tmp_total_uplink_bw > total_uplink_bw)
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d Error: for guaranteed minimum bandwidth of uplink, the sum bandwidth of all qos rules is larger than the total uplink bandwidth!\n",__FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}

		if(tmp_total_downlink_bw > total_downlink_bw)
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d Error: for guaranteed minimum bandwidth of downlink, the sum bandwidth of all qos rules is larger than the total downlink bandwidth!\n",__FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}

	apmib_set(MIB_QOS_DEL, (void *)pQos);
	if ( apmib_set(MIB_QOS_ADD, (void *)pQos) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d qos set mib error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto set_error;
	}

set_error:
	return ret;
}


/**************************************************
* @NAME:
* 	rtk_del_qos_rule_entry
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete qos entry 
*
***************************************************/
int rtk_del_qos_rule_entry(RTK_IPQOS_T *pQos, unsigned int delall)
{
	int ret = RTK_SUCCESS;
	RTK_IPQOS_T entry;

	memset((void *)&entry, '\0', sizeof(entry));
	if (delall)
	{
		if (apmib_set(MIB_QOS_DELALL, (void *)&entry) == 0) {
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d qos delete all error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}
	}
	else
	{
		if (!pQos){
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d qos delete an entry error!\n", __FUNCTION__, __LINE__);
			#endif
			return RTK_FAILED;
		}	

		if (!apmib_set(MIB_QOS_DEL, (void *)pQos)) 
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d qos delete an entry error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto set_error;
		}		
	}
	
set_error:
	return ret;
}


/**************************************************
* @NAME:
* 	rtk_add_qos_rule_entry_immediately
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry to add
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add qos entry and take effect immediately
*	Note: if value of (pQos->mode & QOS_RESTRICT_MIN) as true,
*		  first call function rtk_set_qos_speed, then call this function,
*		  due to need check uplink & downlink guaranteed minimum bandwidth.
*
***************************************************/
int rtk_add_qos_rule_entry_immediately(RTK_IPQOS_T *pQos)
{
	int ret = RTK_FAILED;
	int number = 0;
	int index =0;
	int find =0;
	unsigned char cmdbuf[100]={0};
	RTK_IPQOS_T entry={0};
	unsigned char _ip_str[32]={0};
	
	apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number);
	
	rtk_set_qos_enable(1);
	//auto mode
	rtk_set_qos_speed(0, 0);
	
	index=rtk_get_qos_rule_entry_index(pQos);

	if(index &&(index<=number))
	{
		find =1;
	}
	if(find)
	{
		ret =rtk_modify_qos_rule_entry(pQos);
		if(ret==RTK_FAILED)
		{
			printf("modify qos entry rule fail![%s][%d]\n",__FUNCTION__,__LINE__);
			goto set_error;
		}	
	}
	else
	{
		ret =rtk_add_qos_rule_entry(pQos);	
		if(ret ==RTK_FAILED)
		{
			printf("add qos entry rule fail![%s][%d]\n",__FUNCTION__,__LINE__);
			goto set_error;	
		}
	}
	
	if(!terminal_ip_mac_valid(pQos->mac,ip_str))
	{
		ret=get_terminal_ip_by_mac(pQos->mac,_ip_str);
		rtk_set_terminal_ip(_ip_str);
		
		if(ret==RTK_FAILED){
			printf("ip %s and mac %x:%x:%x:%x:%x:%x not match![%s][%d]\n",ip_str,
			pQos->mac[0],pQos->mac[1],pQos->mac[2],pQos->mac[3],pQos->mac[4],pQos->mac[5]
			,__FUNCTION__,__LINE__);
			goto out;
		}
		
	}
	system("ip_qos.sh");

	memset(cmdbuf,0,sizeof(cmdbuf));
	sprintf(cmdbuf,"echo clear %s > /proc/fast_flush",ip_str);
	system(cmdbuf);
	
	memset(cmdbuf,0,sizeof(cmdbuf));
	sprintf(cmdbuf,"echo enable %s > /proc/fast_flush",ip_str);
	system(cmdbuf);
	
out:
	ret =RTK_SUCCESS;
set_error:
	
	return ret;
}


/**************************************************
* @NAME:
* 	rtk_del_qos_rule_entry_immediately
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete qos entry and take effect immediately
*
***************************************************/
int rtk_del_qos_rule_entry_immediately(RTK_IPQOS_T *pQos, unsigned int delall)
{
	int ret = RTK_FAILED;
	int number = 0;
	unsigned char cmdbuf[100]={0};
	unsigned char macAddr[6]={0};
	unsigned char _ip_str[32]={0};
	memcpy(macAddr,pQos->mac,6);
	
	ret=rtk_del_qos_rule_entry(pQos, delall);
	if(ret==RTK_FAILED)
	{
		
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG) 
		printf("del old qos entry rule fail![%s][%d]\n",__FUNCTION__,__LINE__);
		#endif
		goto set_error;
	}

	apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number);
	if(number==0){
		rtk_set_qos_enable(0);
		memset(cmdbuf,0,sizeof(cmdbuf));
		sprintf(cmdbuf,"echo 0 > /proc/qos");
	}	
	system("ip_qos.sh");
	if(delall==0)
	{
		if(!terminal_ip_mac_valid(macAddr,ip_str))
		{	
			ret=get_terminal_ip_by_mac(macAddr,_ip_str);
			rtk_set_terminal_ip(_ip_str);
			if(ret==RTK_FAILED){
				printf("ip %s and mac %x:%x:%x:%x:%x:%x not match![%s][%d]\n",ip_str,
				pQos->mac[0],pQos->mac[1],pQos->mac[2],pQos->mac[3],pQos->mac[4],pQos->mac[5]
				,__FUNCTION__,__LINE__);
				goto out;
			}
		}
		memset(cmdbuf,0,sizeof(cmdbuf));
		sprintf(cmdbuf,"echo enable %s > /proc/fast_flush",ip_str);
		system(cmdbuf);
	}
	
out:
	ret =RTK_SUCCESS;
	
set_error:	
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_add_qos_rule_entry_imm_inband
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry to add
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	add qos entry and take effect immediately api for inband
*	Note: if value of (pQos->mode & QOS_RESTRICT_MIN) as true,
*		  first call function rtk_set_qos_speed, then call this function,
*		  due to need check uplink & downlink guaranteed minimum bandwidth.
*
***************************************************/
int rtk_add_qos_rule_entry_imm_inband(RTK_IPQOS_T *pQos)
{
	int ret = RTK_FAILED;
	unsigned char ipstr[32]; 
	
	ret=rtk_add_qos_rule_entry_immediately(pQos);
	if(ret ==RTK_SUCCESS)
	{
		apmib_update(CURRENT_SETTING);
	}
set_error:
	
	return ret;
}


/**************************************************
* @NAME:
* 	rtk_del_qos_rule_entry_immediately
* 
* @PARAMETERS:
* 	@Input
* 		pQos: the qos entry to delete 
*		delall: delete flag
*		 	   0: delete an entry, 1: delete all entry 
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	delete qos entry and take effect immediately api for inband
*
***************************************************/
int rtk_del_qos_rule_entry_imm_inband(RTK_IPQOS_T *pQos, unsigned int delall)
{
	int ret = RTK_FAILED;
	unsigned char ipstr[32]; 
	int index =0;
	int number =0;
	RTK_IPQOS_T entry = {0};
		
	if(delall)
		ret =rtk_del_qos_rule_entry_immediately(pQos,delall);
	else
	{
		index=rtk_get_qos_rule_entry_index(pQos);
		apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number);
		if(index &&(index<=number))
		{
			memset(&entry,0,sizeof(entry));
			*((char*)&entry) = (char)(index);
			apmib_get(MIB_QOS_RULE_TBL,(void*)&entry);
			
			ret =rtk_del_qos_rule_entry_immediately(pQos,delall);
			if(ret ==RTK_FAILED)
			{
				//printf("[%s]:[%d].\n",__FUNCTION__,__LINE__);
				ret =rtk_del_qos_rule_entry_immediately(&entry,delall);
				if(ret ==RTK_FAILED)
				{
					#if defined(RTK_FIREWALL_ADAPTER_DEBUG) 
					printf("del qos rule entry failed![%s][%d]\n",__FUNCTION__,__LINE__);
					#endif
					goto set_error;
				}
			}
		}
		else
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG) 
				printf("can't find qos rule,del qos rule entry failed![%s][%d]\n",__FUNCTION__,__LINE__);
			#endif
		}
	}

	if(ret ==RTK_SUCCESS)
	{
		apmib_update(CURRENT_SETTING);
	}
		
set_error:	
	return ret;
}

#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
/**************************************************
* @NAME:
* 	rtk_get_qos_rule_monopoly
* 
* @PARAMETERS:
* 	@Input
* 		none
*
* 	@Output
*	 	enabled: enable/disable one client monopoly
		macAddr: client mac address
		qosTime: the time to monopolize bandwidth
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get client monopolize bandwidth info
*	
*	
***************************************************/

int rtk_get_qos_rule_monopoly(unsigned int* enabled, unsigned char * macAddr, unsigned int* qosTime)
{
	int ret = RTK_SUCCESS;
	int value =0;
	
	if((enabled ==NULL)||(macAddr ==NULL)||(qosTime==NULL))
	{
		ret = RTK_FAILED;
		goto get_error;	
	}
	

	if (!apmib_get(MIB_QOS_MONOPOLY_ENABLED, (void *)&value))	
	{
		printf("%s %d get MIB_QOS_MONOPOLY_ENABLED mib error!",__FUNCTION__, __LINE__);
		ret = RTK_FAILED;
		goto get_error;	
	}
	*enabled = value;

	
	memset(macAddr,0,6);
	if ( !apmib_get(MIB_QOS_MONOPOLY_MAC, (void *)macAddr)) {
		printf("%s %d get MIB_QOS_MONOPOLY_MAC mib error!",__FUNCTION__, __LINE__);
		ret = RTK_FAILED;
		goto get_error;	
	}
	
	value =0;
	if ( !apmib_get(MIB_QOS_MONOPOLY_TIME, (void *)&value)) {
		printf("%s %d get MIB_QOS_MONOPOLY_TIME mib error!",__FUNCTION__, __LINE__);
		ret = RTK_FAILED;
		goto get_error;
	}
	*qosTime = value;	
	
get_error:
	return ret;

}

int rtk_get_qos_rule_monopoly_info(RTK_QOSMNP_INFOp info)
{
	int ret = RTK_SUCCESS;
	int value =0;
	
	if(info ==NULL)
	{
		ret = RTK_FAILED;
		goto get_error;	
	}
	
	ret=rtk_get_qos_rule_monopoly(&(info->enabled),info->macAddr,&(info->qosTime));
	
	
get_error:
	return ret;

}

/**************************************************
* @NAME:
* 	rtk_set_qos_rule_monopoly
* 
* @PARAMETERS:
* 	@Input
* 		enabled: enable/disable one client monopoly
		macAddr: client mac address
		qosTime: the time to monopolize bandwidth
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set client monopolize bandwidth info
*	
*	
***************************************************/

int rtk_set_qos_rule_monopoly(unsigned int enabled, unsigned char * macAddr, unsigned int qosTime)
{
	int ret = RTK_SUCCESS;
	char buffer[128];
	sprintf(buffer, "flash set QOS_MONOPOLY_ENABLED %d", enabled);
	system(buffer);
	sprintf(buffer, "flash set QOS_MONOPOLY_MAC %02x%02x%02x%02x%02x%02x", macAddr[0],macAddr[1],macAddr[2],macAddr[3],macAddr[4],macAddr[5]);
	system(buffer);
	sprintf(buffer, "flash set QOS_MONOPOLY_TIME %d", qosTime);
	system(buffer);
set_error:
	return ret;

}
/**************************************************
* @NAME:
* 	rtk_set_qos_rule_monopoly
* 
* @PARAMETERS:
* 	@Input
* 		enabled: enable/disable one client monopoly
		macAddr: client mac address
		qosTime: the time to monopolize bandwidth
*
* 	@Output
*	 	none
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	set client monopolize bandwidth info and take effect immediately
*	
*	
***************************************************/


int rtk_set_qos_rule_monopoly_immediately(unsigned int enabled, unsigned char * macAddr, unsigned int qosTime)
{
	int ret = RTK_SUCCESS;
	unsigned char command[100]={0};
	ret =rtk_set_qos_rule_monopoly(enabled, macAddr, qosTime);
	if(ret == RTK_SUCCESS)
	{
		system("ip_qos.sh");
		system("echo 2 > /proc/fast_nat");
	
	}
	
	return ret;
}
#endif
#endif

/*GET*/
/**************************************************
* @NAME:
* 	rtk_get_port_forward_enable
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the port forward function or not
*				 1: enable, 0:disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get port forward enable status 
*
***************************************************/
int rtk_get_port_forward_enable(unsigned int *enabled)
{
	int ret = RTK_SUCCESS;
	int value =0;

	if (!enabled)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_PORTFW_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get port forward enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;

get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_port_forward_entry
* 
* @PARAMETERS:
* 	@Input
*		empty_entry_num: the number of pPortFW 
* 	@Output
*	 	num: the actual number of port forward table
* 		pPortFW: store the port forward entry
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get port forward entry 
*
***************************************************/
int rtk_get_port_forward_entry(unsigned int *num, RTK_PORTFW_T *pPortFW, unsigned int empty_entry_num)
{
	int number = 0, count = 0, i = 0;
	int ret = RTK_SUCCESS;
	
	
	if (!pPortFW)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_PORTFW_TBL_NUM,(void*)&number))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d port forward get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*num = number;
	
	count = (number >= empty_entry_num)?empty_entry_num:number;
	for (i = 0; i < count; i++)
	{
		memset((void *)&pPortFW[i], '\0', sizeof(pPortFW[i]));
		*((char *)&pPortFW[i]) = (char)(i+1);
		if ( !apmib_get(MIB_PORTFW_TBL, (void *)&pPortFW[i]))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d port forward get tbl error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
	}
	
get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_port_filter_enable
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the port filter function or not
*				 1: enable, 0:disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get port filter enable status 
*
***************************************************/
int rtk_get_port_filter_enable(unsigned int *enabled)
{
	int ret = RTK_SUCCESS;
	int value =0;

	if (!enabled)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_PORTFILTER_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get port filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;

get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_port_filter_entry
* 
* @PARAMETERS:
* 	@Input
*		empty_entry_num: the number of pPortFilter 
* 	@Output
*	 	num: the actual number of port filter table
* 		pPortFilter: store the port filter entry
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get port filter entry 
*
***************************************************/
int rtk_get_port_filter_entry(unsigned int *num, RTK_PORTFILTER_T *pPortFilter, unsigned int empty_entry_num)
{
	int number = 0, count = 0, i = 0;
	int ret = RTK_SUCCESS;
	
	
	if (!pPortFilter)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_PORTFILTER_TBL_NUM,(void*)&number))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d port filter get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*num = number;
	
	count = (number >= empty_entry_num)?empty_entry_num:number;
	for (i = 0; i < count; i++)
	{
		memset((void *)&pPortFilter[i], '\0', sizeof(pPortFilter[i]));
		*((char *)&pPortFilter[i]) = (char)(i+1);
		if ( !apmib_get(MIB_PORTFILTER_TBL, (void *)&pPortFilter[i]))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d port filter get tbl error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
	}
	
get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_ip_filter_enable
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the ip filter function or not
*				 1: enable, 0:disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get ip filter enable status 
*
***************************************************/
int rtk_get_ip_filter_enable(unsigned int *enabled)
{
	int ret = RTK_SUCCESS;
	int value =0;

	if (!enabled)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_IPFILTER_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get ip filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;

get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_ip_filter_entry
* 
* @PARAMETERS:
* 	@Input
*		empty_entry_num: the number of pIpFilter 
* 	@Output
*	 	num: the actual number of ip filter table
* 		pIpFilter: store the ip filter entry
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get ip filter entry 
*
***************************************************/
int rtk_get_ip_filter_entry(unsigned int *num, RTK_IPFILTER_T *pIpFilter, unsigned int empty_entry_num)
{
	int number = 0, count = 0, i = 0;
	int ret = RTK_SUCCESS;
	
	
	if (!pIpFilter)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_IPFILTER_TBL_NUM,(void*)&number))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d ip filter get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*num = number;
	
	count = (number >= empty_entry_num)?empty_entry_num:number;
	for (i = 0; i < count; i++)
	{
		memset((void *)&pIpFilter[i], '\0', sizeof(pIpFilter[i]));
		*((char *)&pIpFilter[i]) = (char)(i+1);
		if ( !apmib_get(MIB_IPFILTER_TBL, (void *)&pIpFilter[i]))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d ip filter get tbl error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
	}
	
get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_mac_filter_enable
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the mac filter function or not
*				 1: enable, 0:disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get mac filter enable status 
*
***************************************************/
int rtk_get_mac_filter_enable(unsigned int *enabled)
{
	int ret = RTK_SUCCESS;
	int value =0;

	if (!enabled)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_MACFILTER_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get mac filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;

get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_mac_filter_entry
* 
* @PARAMETERS:
* 	@Input
*		empty_entry_num: the number of pMacFilter 
* 	@Output
*	 	num: the actual number of mac filter table
* 		pMacFilter: store the mac filter entry
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get mac filter entry 
*
***************************************************/
int rtk_get_mac_filter_entry(unsigned int *num, RTK_MACFILTER_T *pMacFilter, unsigned int empty_entry_num)
{
	int number = 0, count = 0, i = 0;
	int ret = RTK_SUCCESS;
	
	
	if (!pMacFilter)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_MACFILTER_TBL_NUM,(void*)&number))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d mac filter get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*num = number;
	
	count = (number >= empty_entry_num)?empty_entry_num:number;
	for (i = 0; i < count; i++)
	{
		memset((void *)&pMacFilter[i], '\0', sizeof(pMacFilter[i]));
		*((char *)&pMacFilter[i]) = (char)(i+1);
		if ( !apmib_get(MIB_MACFILTER_TBL, (void *)&pMacFilter[i]))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d mac filter get tbl error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
	}
	
get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_url_filter_enable
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the url filter function or not
*				 1: enable, 0:disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get url filter enable status 
*
***************************************************/
int rtk_get_url_filter_enable(unsigned int *enabled)
{
	int ret = RTK_SUCCESS;
	int value =0;

	if (!enabled)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_URLFILTER_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get url filter enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;

get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_url_filter_mode
* 
* @PARAMETERS:
* 	@Input
* 		none
* 	@Output
*	 	url_filter_mode: url filter mode, 1: white list, 0: black list
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get url filter mode 
*
***************************************************/
int rtk_get_url_filter_mode(int *url_filter_mode)
{
	int ret = RTK_SUCCESS;
	
	if (!url_filter_mode)
		return RTK_FAILED;
		
	if (apmib_get(MIB_URLFILTER_MODE,  (void *)url_filter_mode) == 0)
	{
		ret = RTK_FAILED;
	}

	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_url_filter_entry
* 
* @PARAMETERS:
* 	@Input
*		empty_entry_num: the number of pUrlFilter 
* 	@Output
*	 	num: the actual number of url filter table
* 		pUrlFilter: store the url filter entry
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get url filter entry 
*
***************************************************/
int rtk_get_url_filter_entry(unsigned int *num, RTK_URLFILTER_T *pUrlFilter, unsigned int empty_entry_num)
{
	int number = 0, count = 0, i = 0;
	int ret = RTK_SUCCESS;
	
	
	if (!pUrlFilter)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_URLFILTER_TBL_NUM,(void*)&number))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d url filter get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*num = number;
	
	count = (number >= empty_entry_num)?empty_entry_num:number;
	for (i = 0; i < count; i++)
	{
		memset((void *)&pUrlFilter[i], '\0', sizeof(pUrlFilter[i]));
		*((char *)&pUrlFilter[i]) = (char)(i+1);
		if ( !apmib_get(MIB_URLFILTER_TBL, (void *)&pUrlFilter[i]))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d url filter get tbl error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
	}
	
get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_dmz
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the dmz function or not
*				 1: enable, 0:disable
*		dmz_ip: the dmz host ip address
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get dmz enable status and dmz host ip address 
*
***************************************************/
int rtk_get_dmz(unsigned int *enabled, unsigned int *dmz_ip)
{
	int ret = RTK_SUCCESS;
	int value =0;
	unsigned char buff[6] = {0};

	if (!enabled || !dmz_ip)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_DMZ_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get dmz enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;
	
	memset(buff, '\0', sizeof(buff));
	if (!apmib_get(MIB_DMZ_HOST,  (void *)buff))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get dmz host error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*dmz_ip = (*((unsigned int*)buff));
	
get_error:
	return ret;
}

#if defined(VLAN_CONFIG_SUPPORTED)
/**************************************************
* @NAME:
* 	rtk_get_vlan_enable
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the vlan function or not
*				 1: enable, 0:disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get vlan enable status 
*
***************************************************/
int rtk_get_vlan_enable(unsigned int *enabled)
{
	int ret = RTK_SUCCESS;
	int value =0;

	if (!enabled)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_VLANCONFIG_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get vlan enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;

get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_vlan_config
* 
* @PARAMETERS:
* 	@Input
* 		none
*		
* 	@Output
*	 	pVlanConfig: store the vlan configure  entry
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get vlan configure entry 
*
***************************************************/
int rtk_get_vlan_config(RTK_VLAN_CONFIG_T *pVlanConfig)
{
	int ret = RTK_SUCCESS;
	int i, max_vlan_num;
	#if defined(CONFIG_RTL_8198_AP_ROOT) && defined(GMII_ENABLED)
	max_vlan_num = MAX_IFACE_VLAN_CONFIG-2;
	#else
	max_vlan_num = MAX_IFACE_VLAN_CONFIG-1;
	#endif

	if (!pVlanConfig)
		return RTK_FAILED;

	for (i = 0; i <= max_vlan_num; i++)
	{		
		memset((void *)&pVlanConfig[i], '\0', sizeof(pVlanConfig[i]));
		*((char *)&pVlanConfig[i]) = (char)(i+1);
		if (!apmib_get(MIB_VLANCONFIG_TBL, (void *)&pVlanConfig[i]))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d vlan tbl error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
	}
	
get_error:
	return ret;
}
#endif

#if defined(QOS_BY_BANDWIDTH)
/**************************************************
* @NAME:
* 	rtk_get_qos_enable
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	enabled: indicate whether enable the qos function or not
*				 1: enable, 0:disable
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get qos enable status 
*
***************************************************/
int rtk_get_qos_enable(unsigned int *enabled)
{
	int ret = RTK_SUCCESS;
	int value =0;

	if (!enabled)
		return RTK_FAILED;
	
	if (!apmib_get(MIB_QOS_ENABLED, (void *)&value))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get qos enable error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*enabled = value;

get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_qos_speed
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	uplink_speed: uplink speed
*					  0:means auto speed, other means manual speed
*	 	downlink_speed: downlink speed
*					  0:means auto speed, other means manual speed
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get qos uplink and downlink speed 
*
***************************************************/
int rtk_get_qos_speed(unsigned int *uplink_speed, unsigned int *downlink_speed)
{
	int ret = RTK_SUCCESS;
	int value = 0;
	int auto_uplink = 0, manual_uplink = 0;
	int auto_downlink = 0, manual_downlink = 0;
	
	if ( apmib_get( MIB_QOS_AUTO_UPLINK_SPEED, (void *)&auto_uplink) == 0) 
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get qos auto uplink error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}

	if ( apmib_get( MIB_QOS_MANUAL_UPLINK_SPEED, (void *)&manual_uplink) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get qos manual uplink error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	if ( apmib_get( MIB_QOS_AUTO_DOWNLINK_SPEED, (void *)&auto_downlink) == 0) 
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get qos auto downlink error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	if ( apmib_get( MIB_QOS_MANUAL_DOWNLINK_SPEED, (void *)&manual_downlink) == 0) {
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d get qos manual downlink error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}

	if (auto_uplink)
		*uplink_speed =0;
	else
		*uplink_speed = manual_uplink;
		
	if (auto_downlink)
		*downlink_speed =0;
	else
		*downlink_speed = manual_downlink;
	
get_error:
	return ret;
}

/**************************************************
* @NAME:
* 	rtk_get_qos_total_speed
* 
* @PARAMETERS:
* 	@Input
* 		none 
* 	@Output
*	 	total_uplink_speed: total uplink speed
*	 	total_downlink_speed: total downlink speed
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get qos uplink and downlink speed 
*
***************************************************/
int rtk_get_qos_total_speed(unsigned int *total_uplink_speed, unsigned int *total_downlink_speed)
{
	int ret = RTK_SUCCESS;
	unsigned int uplink_speed = 0, downlink_speed = 0;
	
	if (rtk_get_qos_speed(&uplink_speed, &downlink_speed)==RTK_FAILED)
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d rtk_get_qos_speed error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
		
	if (!uplink_speed) //auto
	{
		#if defined (CONFIG_RTL_8198)||defined (CONFIG_RTL_8198C)
		*total_uplink_speed = 1024000;		// 1000Mbps
		#else
		*total_uplink_speed = 102400;		// 100Mbps
		#endif
	}
	else //manual
		*total_uplink_speed = uplink_speed;
	
	if (!downlink_speed)//auto
	{
		#if defined (CONFIG_RTL_8198)||defined (CONFIG_RTL_8198C)
		*total_downlink_speed = 1024000;		// 1000Mbps
		#else
		*total_downlink_speed = 102400;		// 100Mbps
		#endif
	}
	else //manual
		*total_downlink_speed = downlink_speed;
	
get_error:
	return ret;
}
	
int rtk_get_qos_rule_entry_num(unsigned int *num)
{
	int number = 0;
	int ret = RTK_SUCCESS;
	
	if (!apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d qos get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*num = number;
	
get_error:
	return ret;
}


/**************************************************
* @NAME:
* 	get_qos_rule_entry
* 
* @PARAMETERS:
* 	@Input
*		empty_entry_num: the number of pQos 
* 	@Output
*	 	num: the actual number of qos table
* 		pQos: store the qos entry
*
* @RETRUN:
* 	RTK_SUCCESS
*	RTK_FAILED
* 
* @FUNCTION :
* 	get qos entry 
*
***************************************************/
int get_qos_rule_entry(unsigned int *num, RTK_IPQOS_T *pQos, unsigned int empty_entry_num)
{
	int number = 0, count = 0, i = 0;
	int ret = RTK_SUCCESS;
	
	if (!pQos){
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d qos get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		return RTK_FAILED;
	}
	if (!apmib_get(MIB_QOS_RULE_TBL_NUM,(void*)&number))
	{
		#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
		printf("%s %d qos get tbl number error!\n", __FUNCTION__, __LINE__);
		#endif
		ret = RTK_FAILED;
		goto get_error;
	}
	*num = number;
	
	count = (number >= empty_entry_num)?empty_entry_num:number;
	for (i = 0; i < count; i++)
	{
		memset((void *)&pQos[i], '\0', sizeof(pQos[i]));
		*((char *)&pQos[i]) = (char)(i+1);
		if ( !apmib_get(MIB_QOS_RULE_TBL, (void *)&pQos[i]))
		{
			#if defined(RTK_FIREWALL_ADAPTER_DEBUG)
			printf("%s %d qos get tbl error!\n", __FUNCTION__, __LINE__);
			#endif
			ret = RTK_FAILED;
			goto get_error;
		}
	}
	
get_error:
	return ret;
}



#endif



