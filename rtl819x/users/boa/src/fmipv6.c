/*Web server handler routines for IPv6
  *
  *Authors hf_shi	(hf_shi@realsil.com.cn) 2008.1.24
  *
  */

/*-- System inlcude files --*/
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>

#include "boa.h"
#include "asp_page.h"
#include "apmib.h"
#include "apform.h"
#include "utility.h"
#include "../system/sysconf.h"

#ifdef HOME_GATEWAY
#ifdef CONFIG_IPV6

int getRadvdInfo(radvdCfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_RADVD_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getDnsv6Info(dnsv6CfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_DNSV6_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getDhcpv6sInfo(dhcp6sCfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_DHCPV6S_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getDhcpv6cInfo(dhcp6cCfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_DHCPV6C_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getWanAdd6Info(addr6CfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_ADDR_WAN_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getLanAddv6Info(addr6CfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_ADDR_LAN_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getLanv6PrefixInfo(addr6CfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_ADDR_PFEFIX_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getGatewayv6Info(addr6CfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_ADDR_GW_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getDnsAddv6Info(addr6CfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_ADDR_DNS_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int getTunnel6Info(tunnelCfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_TUNNEL_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int create_Dhcp6CfgFile(dhcp6sCfgParam_t *dhcp6sCfg)
{
	FILE *fp;
	/*open /var/radvd.conf*/
	fp = fopen("/var/dhcp6s.conf", "w");
	if(NULL == fp)
		return -1;
	
	fprintf(fp,"#Dns\n");
	fprintf(fp,"option domain-name-servers %s;\n\n",dhcp6sCfg->DNSaddr6);

	fprintf(fp,"#Interface\n");
	fprintf(fp,"interface %s {\n",dhcp6sCfg->interfaceNameds);
	fprintf(fp,"	address-pool pool1 3600;\n");
	fprintf(fp,"};\n\n");

	fprintf(fp,"#Addrs Pool\n");
	fprintf(fp,"pool pool1 {\n");
	fprintf(fp,"	range %s to %s ;\n",dhcp6sCfg->addr6PoolS,dhcp6sCfg->addr6PoolE);
	fprintf(fp,"};\n\n");
	
      	fclose(fp);
	return 0;
}

int getAddr6Info(addrIPv6CfgParam_t *entry)
{
	if ( !apmib_get(MIB_IPV6_ADDR_PARAM,(void *)entry)){
		return -1 ;        
	}
	return 0;
}

int set_RadvdInterfaceParam(request *wp,  char *path, char *query, radvdCfgParam_t *pradvdCfgParam)
{
	char *tmp;
	uint32 value;

	/*check if enabled*/
	/*get cfg data from web*/
	tmp=req_get_cstream_var(wp,"interfacename","");
	if(strcmp(tmp,pradvdCfgParam->interface.Name))
	{
		/*interface name changed*/
		strcpy(pradvdCfgParam->interface.Name, tmp);
	}
	value =atoi(req_get_cstream_var(wp,"MaxRtrAdvInterval",""));
	if(value != pradvdCfgParam->interface.MaxRtrAdvInterval)
	{
		pradvdCfgParam->interface.MaxRtrAdvInterval = value;
	}
	value =atoi(req_get_cstream_var(wp,"MinRtrAdvInterval",""));
	if(value != pradvdCfgParam->interface.MinRtrAdvInterval)
	{
		pradvdCfgParam->interface.MinRtrAdvInterval = value;
	}
	value =atoi(req_get_cstream_var(wp,"MinDelayBetweenRAs",""));
	if(value != pradvdCfgParam->interface.MinDelayBetweenRAs)
	{
		pradvdCfgParam->interface.MinDelayBetweenRAs = value;
	}
	value =atoi(req_get_cstream_var(wp,"AdvManagedFlag",""));
	if(value > 0)
	{
		pradvdCfgParam->interface.AdvManagedFlag = 1;
	}
	else
	{
		pradvdCfgParam->interface.AdvManagedFlag =0; 
	}
	value =atoi(req_get_cstream_var(wp,"AdvOtherConfigFlag",""));
	if(value >0)
	{
		pradvdCfgParam->interface.AdvOtherConfigFlag = 1;
	}
	else
	{
		pradvdCfgParam->interface.AdvOtherConfigFlag =0;
	}
	value =atoi(req_get_cstream_var(wp,"AdvLinkMTU",""));
	if(value != pradvdCfgParam->interface.AdvLinkMTU)
	{
		pradvdCfgParam->interface.AdvLinkMTU = value;
	}
	/*replace atoi by strtoul to support max value test of ipv6 phase 2 test*/
	tmp = req_get_cstream_var(wp,"AdvReachableTime","");
	value = strtoul(tmp,NULL,10);
	if(value != pradvdCfgParam->interface.AdvReachableTime)
	{
		pradvdCfgParam->interface.AdvReachableTime = value;
	}
	
	/*replace atoi by strtoul to support max value test of ipv6 phase 2 test*/
	tmp = req_get_cstream_var(wp,"AdvRetransTimer","");
	value = strtoul(tmp,NULL,10);	
	if(value != pradvdCfgParam->interface.AdvRetransTimer)
	{
		pradvdCfgParam->interface.AdvRetransTimer = value;
	}
	value =atoi(req_get_cstream_var(wp,"AdvCurHopLimit",""));
	if(value != pradvdCfgParam->interface.AdvCurHopLimit)
	{
		pradvdCfgParam->interface.AdvCurHopLimit = value;
	}
	value =atoi(req_get_cstream_var(wp,"AdvDefaultLifetime",""));
	if(value != pradvdCfgParam->interface.AdvDefaultLifetime)
	{
		pradvdCfgParam->interface.AdvDefaultLifetime = value;
	}
	tmp=req_get_cstream_var(wp,"AdvDefaultPreference","");
	if(strcmp(tmp,pradvdCfgParam->interface.AdvDefaultPreference))
	{
		/*interface name changed*/
		strcpy(pradvdCfgParam->interface.AdvDefaultPreference, tmp);
	}
	value =atoi(req_get_cstream_var(wp,"AdvSourceLLAddress",""));
	
	if(value > 0)
	{
		pradvdCfgParam->interface.AdvSourceLLAddress = 1;
	}
	else
	{
		pradvdCfgParam->interface.AdvSourceLLAddress=0; 
	}
	value =atoi(req_get_cstream_var(wp,"UnicastOnly",""));
	if(value > 0)
	{
		pradvdCfgParam->interface.UnicastOnly = 1;
	}
	else
	{
		pradvdCfgParam->interface.UnicastOnly =0;
	}

	return 0;
}

int set_RadvdPrefixParam(request *wp,  char *path, char *query, radvdCfgParam_t *pradvdCfgParam)
{
	/*get cfg data from web*/
	char *tmpstr;
	char tmpname[30]={0};
	char tmpaddr[30]={0};
	uint32 value;
	int i,j;

	for(j=0;j<MAX_PREFIX_NUM;j++)
	{
		/*get prefix j*/
		sprintf(tmpname,"Enabled_%d",j);
		value=atoi(req_get_cstream_var(wp,tmpname,""));
		if(value >0)
		{
			pradvdCfgParam->interface.prefix[j].enabled = 1;
		}
		else
		{
			pradvdCfgParam->interface.prefix[j].enabled = 0;
		}
		
		for(i=0;i<8;i++)
		{			
			sprintf(tmpname,"radvdprefix%d_%d",j, i+1);
			sprintf(tmpaddr,"0x%s",req_get_cstream_var(wp, tmpname, ""));
			value =strtol(tmpaddr,NULL,16);
			pradvdCfgParam->interface.prefix[j].Prefix[i]= value;
		}

		sprintf(tmpname,"radvdprefix%d_len",j);
		value =atoi(req_get_cstream_var(wp,tmpname,""));
		if(value != pradvdCfgParam->interface.prefix[j].PrefixLen)
		{
			pradvdCfgParam->interface.prefix[j].PrefixLen = value;
		}
		sprintf(tmpname,"AdvOnLinkFlag_%d",j);
		value =atoi(req_get_cstream_var(wp,tmpname,""));
		if(value >0)
		{
			pradvdCfgParam->interface.prefix[j].AdvOnLinkFlag = 1;
		}
		else
		{
			pradvdCfgParam->interface.prefix[j].AdvOnLinkFlag = 0;
		}

		sprintf(tmpname,"AdvAutonomousFlag_%d",j);
		value =atoi(req_get_cstream_var(wp,tmpname,""));
		if(value >0)
		{
			pradvdCfgParam->interface.prefix[j].AdvAutonomousFlag = 1;
		}
		else
		{					
			pradvdCfgParam->interface.prefix[j].AdvAutonomousFlag = 0;
		}		
		sprintf(tmpname,"AdvValidLifetime_%d",j);
		tmpstr = req_get_cstream_var(wp,tmpname,"");
		/*replace atoi by strtoul to support max value test of ipv6 phase 2 test*/
		value = strtoul(tmpstr,NULL,10);
		if(value != pradvdCfgParam->interface.prefix[j].AdvValidLifetime)
		{
			pradvdCfgParam->interface.prefix[j].AdvValidLifetime = value;
		}
		sprintf(tmpname,"AdvPreferredLifetime_%d",j);
		tmpstr = req_get_cstream_var(wp,tmpname,"");
		/*replace atoi by strtoul to support max value test of ipv6 phase 2 test*/
		value = strtoul(tmpstr,NULL,10);
		if(value != pradvdCfgParam->interface.prefix[j].AdvPreferredLifetime)
		{
			pradvdCfgParam->interface.prefix[j].AdvPreferredLifetime = value;
		}
		sprintf(tmpname,"AdvRouterAddr_%d",j);
		value =atoi(req_get_cstream_var(wp,tmpname,""));
		if(value >0)
		{
			pradvdCfgParam->interface.prefix[j].AdvRouterAddr = 1;
		}
		else
		{
			pradvdCfgParam->interface.prefix[j].AdvRouterAddr=0;
		}
		sprintf(tmpname,"if6to4_%d",j);
		tmpstr =req_get_cstream_var(wp,tmpname,"");
		if(strcmp(pradvdCfgParam->interface.prefix[j].if6to4, tmpstr))
		{
			/*interface name changed*/
			strcpy(pradvdCfgParam->interface.prefix[j].if6to4, tmpstr);
		}
	}

	return 0;
}

int  set_RadvdParam(request *wp, char *path, char *query, radvdCfgParam_t *pradvdCfgParam)
{
	
	int enable;
	/*get the configured paramter*/

	/*check if enabled*/
	/*get cfg data from web*/
	enable=atoi(req_get_cstream_var(wp,"enable_radvd",""));
	if(enable ^ pradvdCfgParam->enabled )
	{
       	pradvdCfgParam->enabled = enable;
	}
	if(enable)
	{
		/*set interface data*/
		set_RadvdInterfaceParam(wp, path, query,pradvdCfgParam);
		/*set prefix data*/
		set_RadvdPrefixParam(wp, path, query,pradvdCfgParam);
	}
	return 0;
}

int write_V6Prefix_V6Addr(char *buf, uint16 prefix[] , uint8 len)	
{
	/*valid check*/
	if(NULL == buf )
		return -1;
	if(len>128)
		return -1;
	/*an ipv6 address.full form*/
	sprintf(buf,"%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",prefix[0], prefix[1], prefix[2], prefix[3],
		prefix[4], prefix[5], prefix[6], prefix[7]);
	if(len<128)
		sprintf(buf+strlen(buf),"/%d",len);
	return 0;
}
int create_RadvdPrefixCfg(FILE *fp,struct AdvPrefix *prefix)
{
	char tmp[256];
	if(NULL == fp)
		return -1;
	/*create prefix part of radvd.conf file*/
 	memset(tmp,0,256);
	write_V6Prefix_V6Addr(tmp,prefix->Prefix,prefix->PrefixLen);
      fprintf(fp,"prefix %s\n",tmp);
      fprintf(fp,"{\n"); 
      /*on/off*/
      if(prefix->AdvOnLinkFlag)
	  	fprintf(fp,"AdvOnLink on;\n");
	/*on/off*/
      if(prefix->AdvAutonomousFlag)
	  	fprintf(fp,"AdvAutonomous on;\n");
	/*seconds|infinity Default: 2592000 seconds (30 days)*/
      /*if(prefix->AdvValidLifetime)*/
	fprintf(fp,"AdvValidLifetime %u;\n",prefix->AdvValidLifetime);
	/*seconds|infinity Default: 604800 seconds (7 days)*/
      /*if(prefix->AdvPreferredLifetime)*/
 	fprintf(fp,"AdvPreferredLifetime %u;\n",prefix->AdvPreferredLifetime);
        /* Mobile IPv6 extensions on/off*/
       if(prefix->AdvRouterAddr)
 	  	fprintf(fp,"AdvRouterAddr on;\n");
        /*6to4 interface*/
	 if(prefix->if6to4[0])
	 	fprintf(fp,"Base6to4Interface %s;\n",prefix->if6to4);
       fprintf(fp,"};\n");
	 return 0;
}
int create_radvdIntCfg(FILE *fp,struct Interface *interface)
{
	int i;
	if(NULL == fp)
		return -1;
	
	/*write the conf file according the radvdcfg*/
	fprintf(fp,"interface %s \n{\n",interface->Name);
	/*default send advertisement*/
	fprintf(fp,"AdvSendAdvert on;\n");
	/*seconds*/
	/*if the parameter default value !=0. but now not specified we take it as 0*/
	/*if(interface->MaxRtrAdvInterval)*/
	fprintf(fp,"MaxRtrAdvInterval %u;\n",interface->MaxRtrAdvInterval);
	/*seconds*/
      /*if(interface->MinRtrAdvInterval)*/
	 fprintf(fp,"MinRtrAdvInterval %u;\n",interface->MinRtrAdvInterval);
	/*seconds*/
      /*if(interface->MinDelayBetweenRAs)*/
	fprintf(fp,"MinDelayBetweenRAs %u;\n",interface->MinDelayBetweenRAs);
	/*on/off*/
      if(interface->AdvManagedFlag)
	  	fprintf(fp,"AdvManagedFlag on;\n");
	/*on/off*/
      if(interface->AdvOtherConfigFlag)
	  	fprintf(fp,"AdvOtherConfigFlag on;\n");
	/*integer*/
      /*if(interface->AdvLinkMTU)*/
	fprintf(fp,"AdvLinkMTU %d;\n",interface->AdvLinkMTU);
	/*milliseconds*/
	/*the following 2  default value is 0.*/
      //if(interface->AdvReachableTime)
	fprintf(fp,"AdvReachableTime %u;\n",interface->AdvReachableTime);
	/*milliseconds*/
      //if(interface->AdvRetransTimer)
 	fprintf(fp,"AdvRetransTimer %u;\n",interface->AdvRetransTimer);
	/*integer*/
      /*if(interface->AdvCurHopLimit)*/
	fprintf(fp,"AdvCurHopLimit %d;\n",interface->AdvCurHopLimit);
	/*seconds*/
      /*if(interface->AdvDefaultLifetime)*/
	fprintf(fp,"AdvDefaultLifetime %d;\n",interface->AdvDefaultLifetime);
      /*low,medium,high default medium*/
      if(interface->AdvDefaultPreference[0])
	  	fprintf(fp,"AdvDefaultPreference %s;\n",interface->AdvDefaultPreference);
	/*on/off*/
      if(interface->AdvSourceLLAddress)
	  	fprintf(fp,"AdvSourceLLAddress on;\n");
	/*on/off*/
      if(interface->UnicastOnly)
	  	fprintf(fp,"UnicastOnly on;\n");

      /*write prefix cfg*/
	for(i=0;i<MAX_PREFIX_NUM;i++)
	{
		if(interface->prefix[i].enabled)
			create_RadvdPrefixCfg(fp,&(interface->prefix[i]));
	}
	fprintf(fp,"};\n");
	return 0;
}
int create_RadvdCfgFile(radvdCfgParam_t *radvdcfg)
{
	FILE *fp;
	/*open /var/radvd.conf*/
	fp = fopen("/var/radvd.conf", "w");
	if(NULL == fp)
		return -1;
	create_radvdIntCfg(fp,&(radvdcfg->interface));
      fclose(fp);
	return 0;
}
void formRadvd(request *wp, char *path, char *query)
{
	int pid;
	char tmpBuf[256];
	char *submitUrl;
	char* value;
	radvdCfgParam_t radvdCfgParam;
	/*Get parameters*/
	getRadvdInfo(&radvdCfgParam);
	
	/*Set parameters*/
	value=req_get_cstream_var(wp,"submit","");
	if(0 == strcmp(value,"Save"))
	{
		set_RadvdParam(wp, path, query,&radvdCfgParam);
	}
	
	/*Set to pMIb*/
	apmib_set(MIB_IPV6_RADVD_PARAM,&radvdCfgParam);
	
	/*Update it to flash*/
	apmib_update(CURRENT_SETTING);

	/*create the config file*/
	create_RadvdCfgFile(&radvdCfgParam);
	/*start the Daemon*/
#ifndef NO_ACTION
	pid = fork();
        if (pid) {
	      	waitpid(pid, NULL, 0);
	}
        else if (pid == 0) {
		snprintf(tmpBuf, 100, "%s/%s", _CONFIG_SCRIPT_PATH, _IPV6_RADVD_SCRIPT_PROG);
		execl( tmpBuf, _IPV6_RADVD_SCRIPT_PROG, NULL);
               	exit(1);
        }
#endif

	submitUrl = req_get_cstream_var(wp, "submit-url", "");   // hidden page
	OK_MSG(submitUrl);

  	return;
	
}

int  set_DnsParam(request *wp, char *path, char *query, dnsv6CfgParam_t *pdnsv6CfgParam)
{
	char *value;
	int enable;
	/*check if enabled*/
	enable=atoi(req_get_cstream_var(wp,"enable_dnsv6",""));
	if(enable ^ pdnsv6CfgParam->enabled )
	{
       	pdnsv6CfgParam->enabled = enable;
	}
	if(enable)
	{
		value = req_get_cstream_var(wp,"routername","");
		strcpy(pdnsv6CfgParam->routerName,value);
	}
	return 0;
}

int  set_DhcpSParam(request *wp, char *path, char *query, dhcp6sCfgParam_t *dhcp6sCfgParam)
{
	char *value;
	int enable;
	/*check if enabled*/
	enable=atoi(req_get_cstream_var(wp,"enable_dhcpv6s",""));
	if(enable ^ dhcp6sCfgParam->enabled )
	{
       	dhcp6sCfgParam->enabled = enable;
	}

	value = req_get_cstream_var(wp,"dnsaddr","");
	strcpy(dhcp6sCfgParam->DNSaddr6,value);
	
	value = req_get_cstream_var(wp,"interfacenameds","");
	if(!strcmp(value,""))
	{
		sprintf(dhcp6sCfgParam->interfaceNameds,"%s","br0");
	}
	else
	{
		strcpy(dhcp6sCfgParam->interfaceNameds,value);
	}

	value = req_get_cstream_var(wp,"addrPoolStart","");
	strcpy(dhcp6sCfgParam->addr6PoolS,value);

	value = req_get_cstream_var(wp,"addrPoolEnd","");
	strcpy(dhcp6sCfgParam->addr6PoolE,value);
	
	return 0;
}

int  set_lan_addr6(request *wp, char *path, char *query, addr6CfgParam_t *addr6)
{
	char *value;
	
	value=req_get_cstream_var(wp,"lan_ip_0","");	
	if(value!=NULL)
		addr6->addrIPv6[0]=strtol(value,NULL,16);
	
	value=req_get_cstream_var(wp,"lan_ip_1","");
	if(value!=NULL)
		addr6->addrIPv6[1]=strtol(value,NULL,16);
	
	value=req_get_cstream_var(wp,"lan_ip_2","");
	if(value!=NULL)
		addr6->addrIPv6[2]=strtol(value,NULL,16);
	
	value=req_get_cstream_var(wp,"lan_ip_3","");
	if(value!=NULL)
		addr6->addrIPv6[3]=strtol(value,NULL,16);
	
	value=req_get_cstream_var(wp,"lan_ip_4","");
	if(value!=NULL)
		addr6->addrIPv6[4]=strtol(value,NULL,16);
	
	value=req_get_cstream_var(wp,"lan_ip_5","");
	if(value!=NULL)
		addr6->addrIPv6[5]=strtol(value,NULL,16);
	
	value=req_get_cstream_var(wp,"lan_ip_6","");
	if(value!=NULL)
		addr6->addrIPv6[6]=strtol(value,NULL,16);	

	value=req_get_cstream_var(wp,"lan_ip_7","");
	if(value!=NULL)
		addr6->addrIPv6[7]=strtol(value,NULL,16);

	value=req_get_cstream_var(wp,"prefix_len_lan","");
	if(value!=NULL)					
		addr6->prefix_len=atoi(value);				
	
	return 0;
}


int  check_Addr6Param(request *wp, char *path, char *query,addrIPv6CfgParam_t *addrIPv6CfgParam)
{
	char *tmpvalue;
	int iLoop;
	char paramName[20];
	uint32 validFlag=0;
	uint32 changeFlag=0;
	

	for(iLoop=1;iLoop<=8;iLoop++)
	{
		sprintf(paramName,"addr_1_%d",iLoop);
		tmpvalue = req_get_cstream_var(wp,paramName,"");
		if((strtol(tmpvalue,NULL,16) != 0x0)) break;
		//bzero(tmpvalue,sizeof(tmpvalue));
		bzero(paramName,sizeof(paramName));
	}		
	if(iLoop < 9)	validFlag |= 0x10;	
	
	for(iLoop=1;iLoop<=8;iLoop++)
	{
		sprintf(paramName,"addr_2_%d",iLoop);
		tmpvalue = req_get_cstream_var(wp,paramName,"");
		if((strtol(tmpvalue,NULL,16) != 0x0)) break;
		//bzero(tmpvalue,sizeof(tmpvalue));
		bzero(paramName,sizeof(paramName));
	}		
	if(iLoop < 9)	validFlag |= 0x20;	

	if(validFlag != 0x0) 
	{
		if((validFlag & 0x00f0) & 0x10)
		{
			for(iLoop=1;iLoop<=8;iLoop++)
			{
				sprintf(paramName,"addr_1_%d",iLoop);
				tmpvalue = req_get_cstream_var(wp,paramName,"");
				if((strtol(tmpvalue,NULL,16) != addrIPv6CfgParam->addrIPv6[0][iLoop-1])) 
				{
					changeFlag |=0x1;
					break;
				}
				//bzero(tmpvalue,sizeof(tmpvalue));
				bzero(paramName,sizeof(paramName));
			}	
			
			tmpvalue = req_get_cstream_var(wp,"prefix_len_1","");
			if((atoi(tmpvalue) != addrIPv6CfgParam->prefix_len[0])) 
			{
				addrIPv6CfgParam->prefix_len[0]=atoi(tmpvalue);
				changeFlag |=0x4;
			}
			//bzero(tmpvalue,sizeof(tmpvalue));
		}

		if((validFlag & 0x00f0) & 0x20)
		{
			for(iLoop=1;iLoop<=8;iLoop++)
			{
				sprintf(paramName,"addr_2_%d",iLoop);
				tmpvalue = req_get_cstream_var(wp,paramName,"");
				if((strtol(tmpvalue,NULL,16) != addrIPv6CfgParam->addrIPv6[1][iLoop-1])) 
				{
					changeFlag |=0x2;
					break;
				}
				//bzero(tmpvalue,sizeof(tmpvalue));
				bzero(paramName,sizeof(paramName));
			}	

			tmpvalue = req_get_cstream_var(wp,"prefix_len_2","");
			if((atoi(tmpvalue) != addrIPv6CfgParam->prefix_len[1])) 
			{
				addrIPv6CfgParam->prefix_len[1]=atoi(tmpvalue);
				changeFlag |=0x8;
			}
			//bzero(tmpvalue,sizeof(tmpvalue));
		}
	}
	
	changeFlag |= validFlag;	
	return changeFlag;		
}

void  del_PreAddr6Param(addrIPv6CfgParam_t addrIPv6CfgParam,uint32 _changFlag)
{
	#ifndef NO_ACTION
	char tmpBuf[256];
//	if(addrIPv6CfgParam.enabled == 1)	
	{
		if((_changFlag & 0x1) || (_changFlag & 0x4))
		{
			sprintf(tmpBuf,"ifconfig %s del %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\/%d",_IPV6_LAN_INTERFACE, 
							addrIPv6CfgParam.addrIPv6[0][0],addrIPv6CfgParam.addrIPv6[0][1],addrIPv6CfgParam.addrIPv6[0][2],addrIPv6CfgParam.addrIPv6[0][3],
							addrIPv6CfgParam.addrIPv6[0][4],addrIPv6CfgParam.addrIPv6[0][5],addrIPv6CfgParam.addrIPv6[0][6],addrIPv6CfgParam.addrIPv6[0][7],
							addrIPv6CfgParam.prefix_len[0]);
			system(tmpBuf);
			bzero(tmpBuf,sizeof(tmpBuf));
		}
		if((_changFlag & 0x2) || (_changFlag & 0x8))
		{
			sprintf(tmpBuf,"ifconfig %s del %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\/%d",_IPV6_WAN_INTERFACE, 
							addrIPv6CfgParam.addrIPv6[1][0],addrIPv6CfgParam.addrIPv6[1][1],addrIPv6CfgParam.addrIPv6[1][2],addrIPv6CfgParam.addrIPv6[1][3],
							addrIPv6CfgParam.addrIPv6[1][4],addrIPv6CfgParam.addrIPv6[1][5],addrIPv6CfgParam.addrIPv6[1][6],addrIPv6CfgParam.addrIPv6[1][7],
							addrIPv6CfgParam.prefix_len[1]);
			system(tmpBuf);
			bzero(tmpBuf,sizeof(tmpBuf));
		}
	}
	#endif	
	return;
}


int  set_Addr6Param(request *wp, char *path, char *query, addrIPv6CfgParam_t *addrIPv6CfgParam,uint32 _changFlag)
{
	char *value;

	if(_changFlag & 0x1)
	{
		value = req_get_cstream_var(wp,"addr_1_1","");
		addrIPv6CfgParam->addrIPv6[0][0]=strtol(value,NULL,16);	
		value = req_get_cstream_var(wp,"addr_1_2","");
		addrIPv6CfgParam->addrIPv6[0][1]=strtol(value,NULL,16);
		value = req_get_cstream_var(wp,"addr_1_3","");
		addrIPv6CfgParam->addrIPv6[0][2]=strtol(value,NULL,16);	
		value = req_get_cstream_var(wp,"addr_1_4","");
		addrIPv6CfgParam->addrIPv6[0][3]=strtol(value,NULL,16);	
		value = req_get_cstream_var(wp,"addr_1_5","");
		addrIPv6CfgParam->addrIPv6[0][4]=strtol(value,NULL,16);
		value = req_get_cstream_var(wp,"addr_1_6","");
		addrIPv6CfgParam->addrIPv6[0][5]=strtol(value,NULL,16);
		value = req_get_cstream_var(wp,"addr_1_7","");
		addrIPv6CfgParam->addrIPv6[0][6]=strtol(value,NULL,16);	
		value = req_get_cstream_var(wp,"addr_1_8","");
		addrIPv6CfgParam->addrIPv6[0][7]=strtol(value,NULL,16);	
	}	
		
	if(_changFlag & 0x2)
	{
		value = req_get_cstream_var(wp,"addr_2_1","");
		addrIPv6CfgParam->addrIPv6[1][0]=strtol(value,NULL,16);	
		value = req_get_cstream_var(wp,"addr_2_2","");
		addrIPv6CfgParam->addrIPv6[1][1]=strtol(value,NULL,16);
		value = req_get_cstream_var(wp,"addr_2_3","");
		addrIPv6CfgParam->addrIPv6[1][2]=strtol(value,NULL,16);	
		value = req_get_cstream_var(wp,"addr_2_4","");
		addrIPv6CfgParam->addrIPv6[1][3]=strtol(value,NULL,16);	
		value = req_get_cstream_var(wp,"addr_2_5","");
		addrIPv6CfgParam->addrIPv6[1][4]=strtol(value,NULL,16);
		value = req_get_cstream_var(wp,"addr_2_6","");
		addrIPv6CfgParam->addrIPv6[1][5]=strtol(value,NULL,16);
		value = req_get_cstream_var(wp,"addr_2_7","");
		addrIPv6CfgParam->addrIPv6[1][6]=strtol(value,NULL,16);	
		value = req_get_cstream_var(wp,"addr_2_8","");
		addrIPv6CfgParam->addrIPv6[1][7]=strtol(value,NULL,16);	
	}

	addrIPv6CfgParam->enabled=1;
	return 0;
}
int get_v6address(uint16 addr[])
{
	unsigned char mac[6];
	unsigned char zero[6]={0};
	apmib_get(MIB_ELAN_MAC_ADDR,mac);
	if(!memcmp(mac,zero,6))
		apmib_get(MIB_HW_NIC0_ADDR,mac);
	
	addr[0]=0xfe80;
	addr[1]=0x0000;
	addr[2]=0x0000;
	addr[3]=0x0000;
	addr[4]=(mac[0]<<8 | mac[1]) | 0x0200;
	addr[5]=0x00ff  |(mac[2]<<8);
	addr[6]=0xfe00 | (mac[3]);
	addr[7]=(mac[4]<<8 | mac[5]);
	return 0;
}
int create_Dnsv6CfgFile(dnsv6CfgParam_t *dnsv6cfg)
{
      FILE *fp;
	uint16 v6linkaddr[8];
	/*open /var/dnsmasq.conf*/
	fp = fopen("/var/dnsmasq.conf","w");
	if(NULL == fp)
		return -1;
	/*Never forward plain names (without a dot or domain part)*/
	fprintf(fp,"domain-needed\n");
	/*Never forward addresses in the non-routed address spaces*/
	fprintf(fp,"bogus-priv\n");
	/*refer to /etc/resolv.conf*/
	fprintf(fp,"resolv-file=/etc/resolv.conf\n");
	/*strict-order disable*/
	fprintf(fp,"#strict-order\n");
	/*no resolv disable*/
	fprintf(fp,"#no-resolv\n");
	/*no poll disable*/
	fprintf(fp,"#no-poll\n");
	
	/*add router name and link-local address for ipv6 address query*/
	get_v6address(v6linkaddr);

	/*get route address eth1 link local ?*/
	if(dnsv6cfg->routerName[0])
	{
		
		fprintf(fp,"address=/%s/%x::%x:%x:%x:%x\n",dnsv6cfg->routerName,v6linkaddr[0],v6linkaddr[4],
			v6linkaddr[5],v6linkaddr[6],v6linkaddr[7]);	
	}
	else/*default name myrouter*/
	{
		fprintf(fp,"address=/myrouter/%x::%x:%x:%x:%x",v6linkaddr[0],v6linkaddr[4],
			v6linkaddr[5],v6linkaddr[6],v6linkaddr[7]);
	}
	fprintf(fp,"#listen-address=\n");
	fprintf(fp,"#bind-interfaces\n");
	fprintf(fp,"#no-hosts\n");
	fclose(fp);
	return 0;
}

void formDnsv6(request *wp, char *path, char *query)
{
	int pid;
	char tmpBuf[256];
	char *submitUrl;
	char* value;
	dnsv6CfgParam_t dnsCfgParam;

	/*Get parameters*/
	getDnsv6Info(&dnsCfgParam);

	/*Set to Parameters*/
	value=req_get_cstream_var(wp,"submit","");
	if(0 == strcmp(value, "Save"))
	{
		set_DnsParam(wp, path, query,&dnsCfgParam);
	}
	
	/*Set to pMIb*/
	apmib_set(MIB_IPV6_DNSV6_PARAM,&dnsCfgParam);

	/*Update it to flash*/
	apmib_update(CURRENT_SETTING);

#if 0
	/*create the config file*/
	create_Dnsv6CfgFile(&dnsCfgParam);
	
	/*start the Daemon*/
#ifndef NO_ACTION
	pid = fork();
        if (pid) {
	      	waitpid(pid, NULL, 0);
	}
        else if (pid == 0) {
		snprintf(tmpBuf, 100, "%s/%s", _CONFIG_SCRIPT_PATH, _IPV6_DNSMASQ_SCRIPT_PROG);
		execl( tmpBuf, _IPV6_DNSMASQ_SCRIPT_PROG, NULL);
               	exit(1);
        }
#endif
#endif

	submitUrl = req_get_cstream_var(wp, "submit-url", "");   // hidden page
	OK_MSG(submitUrl);
	return;
}

void formDhcpv6s(request *wp, char *path, char *query)
{
	dhcp6sCfgParam_t dhcp6sCfgParam;
	addr6CfgParam_t	addr6;
	char tmpBuf[256];
	char *submitUrl;
	char* value;
	
	/*Get parameters**/
	getDhcpv6sInfo(&dhcp6sCfgParam);
	
	getLanAddv6Info(&addr6);
	sprintf(tmpBuf,"ifconfig br0 del %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d 2> /dev/null",
						addr6.addrIPv6[0],addr6.addrIPv6[1],addr6.addrIPv6[2],
						addr6.addrIPv6[3],addr6.addrIPv6[4],addr6.addrIPv6[5],
						addr6.addrIPv6[6],addr6.addrIPv6[7],addr6.prefix_len);
	system(tmpBuf);

	/*Set to Parameters*/
	value=req_get_cstream_var(wp,"submit","");
	if(0 == strcmp(value, "Save"))
	{
		set_lan_addr6(wp, path, query,&addr6);
		apmib_set(MIB_IPV6_ADDR_LAN_PARAM,&addr6);
		
		set_DhcpSParam(wp, path, query,&dhcp6sCfgParam);
		apmib_set(MIB_IPV6_DHCPV6S_PARAM,&dhcp6sCfgParam);

		/*Update it to flash*/
		apmib_update(CURRENT_SETTING);	
	}	
	
	/*create the config file*/
	create_Dhcp6CfgFile(&dhcp6sCfgParam);	
	
	/*start the Daemon*/
#ifndef NO_ACTION
	sprintf(tmpBuf,"ifconfig br0 add %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d",
					addr6.addrIPv6[0],addr6.addrIPv6[1],addr6.addrIPv6[2],
					addr6.addrIPv6[3],addr6.addrIPv6[4],addr6.addrIPv6[5],
					addr6.addrIPv6[6],addr6.addrIPv6[7],addr6.prefix_len);
	system(tmpBuf);
	sprintf(tmpBuf,"%s %s",_IPV6_DHCPV6S_SCRIPT_PROG, dhcp6sCfgParam.interfaceNameds);
	system(tmpBuf);
#endif

	submitUrl = req_get_cstream_var(wp, "submit-url", "");   // hidden page
	OK_MSG(submitUrl);
	return;
}

void formIpv6Setup(request *wp, char *path, char *query)
{
		addr6CfgParam_t	addr6_wan;
		addr6CfgParam_t addr6_gw;
		addr6CfgParam_t addr6_dns;
		addr6CfgParam_t addr6_prefix;
		dhcp6cCfgParam_t dhcp6cCfgParam;
		dnsv6CfgParam_t dnsCfgParam;
		char *submitUrl;
		char* strval;
		uint32 val;
		int repid_commit=0;
			
		/*Set to Parameters*/
		strval=req_get_cstream_var(wp,"save","");
	
		if(0 == strcmp(strval, "Apply Changes"))
		{			
			strval=req_get_cstream_var(wp,"wan_enable","");
			val= atoi(strval);
			apmib_set(MIB_IPV6_WAN_ENABLE,&val);
		
			if(val){
				strval=req_get_cstream_var(wp,"OriginType","");
				if(strval!=NULL){
					val= atoi(strval);
					apmib_set(MIB_IPV6_ORIGIN_TYPE,&val);
				}
				
				strval=req_get_cstream_var(wp,"linkType","");
				if(strval!=NULL){
					val=atoi(strval);			
					apmib_set(MIB_IPV6_LINK_TYPE,&val);
				}

				strval=req_get_cstream_var(wp,"dnsType","");
				if(strval[0]){
					val=atoi(strval);			
					apmib_set(MIB_IPV6_DNS_AUTO,&val);
#ifdef TR181_SUPPORT
					if(val)
					{
						int i=0,find=0,maxNum=IPV6_DHCPC_SENDOPT_NUM;
						DHCPV6C_SENDOPT_T entryTmp={0};
						DHCPV6C_SENDOPT_T entry[2]={0};

						apmib_set(MIB_IPV6_DHCPC_SENDOPT_TBL_NUM,(void*)&maxNum);
						for(i=1;i<=IPV6_DHCPC_SENDOPT_NUM;i++)
						{
							*((char *)&entryTmp) = (char)i;
							if ( !apmib_get(MIB_IPV6_DHCPC_SENDOPT_TBL, (void *)&entryTmp)){
								printf("get MIB_IPV6_DHCPC_SENDOPT_TBL fail!\n");
								return;
							}
							if(entryTmp.tag==23)
							{
								if(!entryTmp.enable)
								{//enable it
									
									*((char*)entry)=(char)i;
									if(apmib_get(MIB_IPV6_DHCPC_SENDOPT_TBL,(void*)entry)==0)
									{
										printf("get MIB_IPV6_DHCPC_SENDOPT_TBL fail!\n");
										return -1;
									}
									entry[1]=entry[0];
									entry[1].enable=1;
									if(apmib_set(MIB_IPV6_DHCPC_SENDOPT_MOD,(void*)entry)==0)
									{
										printf("set MIB_IPV6_DHCPC_SENDOPT_MOD fail!\n");
										return ;
									}
								}
								find=1;
								break;
							}							
						}
						if(!find)
						{
							for(i=1;i<=IPV6_DHCPC_SENDOPT_NUM;i++)
							{
								*((char*)entry)=(char)i;
								if(apmib_get(MIB_IPV6_DHCPC_SENDOPT_TBL,(void*)entry)==0)
								{
									printf("get MIB_IPV6_DHCPC_SENDOPT_TBL fail!\n");
									return;
								}
								if(entry[0].tag==0)//not used
								{
									entry[1]=entry[0];
									entry[1].tag=23;
									entry[1].enable=1;
								}
								if(apmib_set(MIB_IPV6_DHCPC_SENDOPT_MOD,(void*)entry)==0)
								{
									printf("set MIB_IPV6_DHCPC_SENDOPT_MOD fail!%d\n",__LINE__);
									return;
								}
								break;
							}
						}
					}else
					{
						int i=0,find=0,maxNum=IPV6_DHCPC_SENDOPT_NUM;
						DHCPV6C_SENDOPT_T entry[2]={0};

						apmib_set(MIB_IPV6_DHCPC_SENDOPT_TBL_NUM,(void*)&maxNum);
						for(i=1;i<=IPV6_DHCPC_SENDOPT_NUM;i++)
						{
							*((char*)entry)=(char)i;
							if(apmib_get(MIB_IPV6_DHCPC_SENDOPT_TBL,(void*)entry)==0)
							{
								printf("get MIB_IPV6_DHCPC_SENDOPT_TBL fail!\n");
								return;
							}
							if(entry[0].tag==23 && entry[0].enable!=0)
							{
								entry[1]=entry[0];
								entry[1].enable=0;
							}
							if(apmib_set(MIB_IPV6_DHCPC_SENDOPT_MOD,(void*)entry)==0)
							{
								printf("set MIB_IPV6_DHCPC_SENDOPT_MOD fail!%d\n",__LINE__);
								return;
							}
						}
					}
#endif
				}
				strval=req_get_cstream_var(wp,"enable_dhcpv6RapidCommit","");
				if(strval[0]){
					repid_commit=1;					
				}
				if(apmib_set(MIB_IPV6_DHCP_RAPID_COMMIT_ENABLE,(void*)&repid_commit)==0)
				{
					printf("set MIB_IPV6_DHCP_RAPID_COMMIT_ENABLE fail!\n");
					return;
				}
				
				strval=req_get_cstream_var(wp,"dhcpMode","");
				if(strval[0]){
					if(strcmp(strval,"stateless")==0)
						val=IPV6_DHCP_STATELESS;
					else
					{
						val=IPV6_DHCP_STATEFUL;
						
					}
#ifdef TR181_SUPPORT
					apmib_set(MIB_IPV6_DHCPC_REQUEST_ADDR,(void*)&val);
#endif

					apmib_set(MIB_IPV6_DHCP_MODE,&val);
				}
				strval=req_get_cstream_var(wp,"enable_dhcpv6pd","");	
				
				if(strval!=NULL){
					val = atoi(strval);
					apmib_set(MIB_IPV6_DHCP_PD_ENABLE,&val);
				}				
				
				strval=req_get_cstream_var(wp,"wan_ip_0","");	
				if(strval!=NULL)
					addr6_wan.addrIPv6[0]=strtol(strval,NULL,16);		
				strval=req_get_cstream_var(wp,"wan_ip_1","");	
				if(strval!=NULL)
					addr6_wan.addrIPv6[1]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_ip_2","");
				if(strval!=NULL)
					addr6_wan.addrIPv6[2]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_ip_3","");
				if(strval!=NULL)
					addr6_wan.addrIPv6[3]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_ip_4","");
				if(strval!=NULL)
					addr6_wan.addrIPv6[4]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_ip_5","");
				if(strval!=NULL)
					addr6_wan.addrIPv6[5]=strtol(strval,NULL,16);				
				strval=req_get_cstream_var(wp,"wan_ip_6","");
				if(strval!=NULL)
					addr6_wan.addrIPv6[6]=strtol(strval,NULL,16);	
				strval=req_get_cstream_var(wp,"wan_ip_7","");
				if(strval!=NULL)
					addr6_wan.addrIPv6[7]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"prefix_len_ip","");
				if(strval!=NULL)					
					addr6_wan.prefix_len=atoi(strval);				
				apmib_set(MIB_IPV6_ADDR_WAN_PARAM,&addr6_wan);

				strval=req_get_cstream_var(wp,"wan_gw_0","");
				if(strval!=NULL)
					addr6_gw.addrIPv6[0]=strtol(strval,NULL,16);	
				strval=req_get_cstream_var(wp,"wan_gw_1","");
				if(strval!=NULL)
					addr6_gw.addrIPv6[1]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_gw_2","");
				if(strval!=NULL)
					addr6_gw.addrIPv6[2]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_gw_3","");
				if(strval!=NULL)
					addr6_gw.addrIPv6[3]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_gw_4","");
				if(strval!=NULL)
					addr6_gw.addrIPv6[4]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_gw_5","");
				if(strval!=NULL)
					addr6_gw.addrIPv6[5]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_gw_6","");
				if(strval!=NULL)
					addr6_gw.addrIPv6[6]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_gw_7","");
				if(strval!=NULL)
					addr6_gw.addrIPv6[7]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"prefix_len_gw","");
				if(strval!=NULL)
					addr6_gw.prefix_len=atoi(strval);
				apmib_set(MIB_IPV6_ADDR_GW_PARAM,&addr6_gw);				

				strval=req_get_cstream_var(wp,"wan_dns1_0","");
				if(strval!=NULL)
					addr6_dns.addrIPv6[0]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_dns1_1","");
				if(strval!=NULL)
					addr6_dns.addrIPv6[1]=strtol(strval,NULL,16);	
				strval=req_get_cstream_var(wp,"wan_dns1_2","");
				if(strval!=NULL)
					addr6_dns.addrIPv6[2]=strtol(strval,NULL,16);	
				strval=req_get_cstream_var(wp,"wan_dns1_3","");
				if(strval!=NULL)
					addr6_dns.addrIPv6[3]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_dns1_4","");
				if(strval!=NULL)
					addr6_dns.addrIPv6[4]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_dns1_5","");
				if(strval!=NULL)
					addr6_dns.addrIPv6[5]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_dns1_6","");
				if(strval!=NULL)
					addr6_dns.addrIPv6[6]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"wan_dns1_7","");
				if(strval!=NULL)
					addr6_dns.addrIPv6[7]=strtol(strval,NULL,16);
				strval=req_get_cstream_var(wp,"prefix_len_dns1","");
				if(strval!=NULL)
					addr6_dns.prefix_len=atoi(strval);
				apmib_set(MIB_IPV6_ADDR_DNS_PARAM,&addr6_dns);

				/*Get parameters*/
				getDnsv6Info(&dnsCfgParam);
				/*Set to Parameters*/
				set_DnsParam(wp, path, query,&dnsCfgParam);				
				/*Set to pMIb*/
				apmib_set(MIB_IPV6_DNSV6_PARAM,&dnsCfgParam);
				
				strval = req_get_cstream_var(wp, ("mldproxyEnabled"), "");
				if ( !strcmp(strval, "ON"))
					val = 0;
				else
					val = 1;
				if ( !apmib_set(MIB_MLD_PROXY_DISABLED, (void *)&val)) {
					printf ("Set MIB_MLD_PROXY_DISABLED error!");
					return;
				}
	
			}
			/*Update it to flash*/
			apmib_update(CURRENT_SETTING);	
		}
			
		submitUrl = req_get_cstream_var(wp, "submit-url", "");	 // hidden page
		OK_MSG(submitUrl);
		return;
		
}


void formIPv6Addr(request *wp, char *path, char *query)
{
	addrIPv6CfgParam_t addrIPv6CfgParam,addrIPv6CfgParamBak;
	char tmpBuf[256];
	char *submitUrl;
	char *msg;
	char* value;
	uint32 isChangFlag=0;

	/*Get parameters**/
	getAddr6Info(&addrIPv6CfgParam);
	addrIPv6CfgParamBak = addrIPv6CfgParam;

	/*Set to Parameters*/
	value=req_get_cstream_var(wp,"submit","");
	if(0 == strcmp(value, "Save"))
	{
		isChangFlag=check_Addr6Param(wp, path, query,&addrIPv6CfgParam);

		//Case: invalid address
		if((isChangFlag & 0x00f0) == 0x0)
		{
			msg = "Invalid Addresses!";
			goto FAIL;
		}
		
		//Case: No change
		if((isChangFlag & 0x000f) == 0x0)
		{
			/*
			if((isChangFlag & 0x00f0)&0x20)
				msg = "Br's Address is invalid!";
			else if((isChangFlag & 0x00f0)&0x10)
				msg = "Eth0's Address is invalid!";
			else
			*/
				msg = "No Address Changed!";
			goto FAIL;
		}		
		
		//set to Parameters		
		set_Addr6Param(wp, path, query,&addrIPv6CfgParam,isChangFlag);
		
	}
	
	/*Set to pMIb*/
	apmib_set(MIB_IPV6_ADDR_PARAM,&addrIPv6CfgParam);

	/*Update it to flash*/
	apmib_update(CURRENT_SETTING);		

	#ifndef NO_ACTION	
	//Del Old Addr6
		del_PreAddr6Param(addrIPv6CfgParamBak,isChangFlag);
	//Add New Addr6
	if((isChangFlag & 0x1) ||(isChangFlag & 0x4))
	{
		sprintf(tmpBuf,"ifconfig %s %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\/%d",_IPV6_LAN_INTERFACE, 
						addrIPv6CfgParam.addrIPv6[0][0],addrIPv6CfgParam.addrIPv6[0][1],addrIPv6CfgParam.addrIPv6[0][2],addrIPv6CfgParam.addrIPv6[0][3],
						addrIPv6CfgParam.addrIPv6[0][4],addrIPv6CfgParam.addrIPv6[0][5],addrIPv6CfgParam.addrIPv6[0][6],addrIPv6CfgParam.addrIPv6[0][7],
						addrIPv6CfgParam.prefix_len[0]);
		system(tmpBuf);
		bzero(tmpBuf,sizeof(tmpBuf));
	}

	if((isChangFlag & 0x2)||(isChangFlag & 0x8))
	{
		sprintf(tmpBuf,"ifconfig %s %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\/%d",_IPV6_WAN_INTERFACE, 
						addrIPv6CfgParam.addrIPv6[1][0],addrIPv6CfgParam.addrIPv6[1][1],addrIPv6CfgParam.addrIPv6[1][2],addrIPv6CfgParam.addrIPv6[1][3],
						addrIPv6CfgParam.addrIPv6[1][4],addrIPv6CfgParam.addrIPv6[1][5],addrIPv6CfgParam.addrIPv6[1][6],addrIPv6CfgParam.addrIPv6[1][7],
						addrIPv6CfgParam.prefix_len[1]);
		system(tmpBuf);
		bzero(tmpBuf,sizeof(tmpBuf));
	}
	#endif
	
	submitUrl = req_get_cstream_var(wp, "submit-url", "");   // hidden page
	//OK_MSG(submitUrl);
	//return;

#ifdef REBOOT_CHECK
	if(needReboot == 1)
	{
		OK_MSG(submitUrl);
		return;
	}
#endif

	if (submitUrl[0])
		send_redirect_perm(wp, submitUrl);
  	return;


FAIL:
	ERR_MSG(msg);
	return;
}
void formTunnel6(request *wp, char *path, char *query)
{
	tunnelCfgParam_t tunnelCfgParam;
	char tmpBuf[256];
	char *submitUrl;
	char* value;
	unsigned char buffer[50],wanIP[50];
	int enable;
	
	/*Get parameters**/
	getTunnel6Info(&tunnelCfgParam);

	/*Set to Parameters*/
	value=req_get_cstream_var(wp,"submit","");
	if(0 == strcmp(value, "Save"))
	{
		enable=atoi(req_get_cstream_var(wp,"enable_tunnel6",""));
		if(enable ^ tunnelCfgParam.enabled )
		{
	       	tunnelCfgParam.enabled = enable;
		}
	}

	/*Set to pMIb*/
	apmib_set(MIB_IPV6_TUNNEL_PARAM,&tunnelCfgParam);

	/*Update it to flash*/
	apmib_update(CURRENT_SETTING);	
	
	/*start the Daemon*/
//#ifndef NO_ACTION
#if 0
	//tunnel add 
	if ( !apmib_get( MIB_WAN_IP_ADDR,  (void *)buffer) ) goto setErr_tunnel;
	sprintf(wanIP,"%s",inet_ntoa(*((struct in_addr *)buffer)));
	sprintf(tmpBuf,"ip tunnel add tun mode sit remote any local %s",wanIP);
	system(tmpBuf);	

	bzero(tmpBuf,sizeof(tmpBuf));
	sprintf(tmpBuf,"ifconfig tun up");
	system(tmpBuf);

	char *p1,*p2,*p3,*p4;
	p1=strtok(wanIP,".");
	p2=strtok(NULL,".");
	p3=strtok(NULL,".");
	p4=strtok(NULL,".");

	bzero(tmpBuf,sizeof(tmpBuf));
	sprintf(tmpBuf,"ifconfig tun 2002:%02x%02x:%02x%02x:1::1\/16",atoi(p1),atoi(p2),atoi(p3),atoi(p4));
	system(tmpBuf);

	//br0
	bzero(tmpBuf,sizeof(tmpBuf));
	sprintf(tmpBuf,"ifconfig br0 2002:%02x%02x:%02x%02x:2::1\/64",atoi(p1),atoi(p2),atoi(p3),atoi(p4));
	system(tmpBuf);
	
#endif
	submitUrl = req_get_cstream_var(wp, "submit-url", "");   // hidden page
	OK_MSG(submitUrl);
	return;
setErr_tunnel:
	
	return;
}
/* 
  *  Get constant string :Router Advertisement Setting
  */
uint32 getIPv6Info(request *wp, int argc, char **argv)
{
	char	*name;
	radvdCfgParam_t radvdCfgParam;
	dnsv6CfgParam_t dnsv6CfgParam;
	dhcp6sCfgParam_t dhcp6sCfgParam;
	
	tunnelCfgParam_t tunnelCfgParam;
	
	//printf("get parameter=%s\n", argv[0]);
	name = argv[0];
	if (name == NULL) {
   		fprintf(stderr, "Insufficient args\n");
   		return -1;
   	}
	
//////MENU//////////////////////////////////////////////////
	if(!strcmp(name,"IPv6_Menu"))
	{
		req_format_write(wp,"menu.addItem(\"ipv6\");");
		req_format_write(wp,"ipv6 = new MTMenu();");
		req_format_write(wp,"ipv6.addItem(\"IPv6 Wan Setting\", \"ipv6_wan.htm\", \"\", \"Configure IPv6 Wan Setting\");\n");
		req_format_write(wp,"ipv6.addItem(\"IPv6 Lan Setting\", \"dhcp6s.htm\", \"\", \"Setup IPv6 Lan\");\n");
		req_format_write(wp,"ipv6.addItem(\"Router Advertisement Daemon\", \"radvd.htm\", \"\", \"Setup Radvd Daemon\");\n");
		//req_format_write(wp,"ipv6.addItem(\"DNS Proxy Daemon\", \"dnsv6.htm\", \"\", \"Setup Dnsmasq Daemon\");\n");
		req_format_write(wp,"ipv6.addItem(\"Tunnel (6 over 4)\", \"tunnel6.htm\", \"\", \"Tunnel (6to4)\");\n"); 
		req_format_write(wp,"menu.makeLastSubmenu(ipv6);\n");
		return 0;
	}

	if(!strcmp(name,"IPv6_nojs_Menu"))
	{
		req_format_write(wp,"<tr><td><b>IPv6</b></td></tr>");
		req_format_write(wp,"<tr><td><a href=\"ipv6_wan.htm\" target=\"view\">IPv6 Wan Setting</a></td></tr>");
		req_format_write(wp,"<tr><td><a href=\"dhcp6s.htm\" target=\"view\">IPv6 LAN Setting</a></td></tr>");
		req_format_write(wp,"<tr><td><a href=\"radvd.htm\" target=\"view\">Router Advertisement Daemon</a></td></tr>");
		req_format_write(wp,"<tr><td><a href=\"dnsv6.htm\" target=\"view\">DNS Proxy Daemon</a></td></tr>");
		req_format_write(wp,"<tr><td><a href=\"tunnel6.htm\" target=\"view\">Tunnel 6over4</a></td></tr>");
		return 0;
	}
//////////radvd///////////////////////////////////////////////////////////////
	if(getRadvdInfo(&radvdCfgParam)<0)
	{
		req_format_write(wp,"Read Radvd Configuration Error");
		return -1;
	}
      if(!strcmp(name,"enable_radvd"))
      {
       	if(radvdCfgParam.enabled)
        		req_format_write(wp,"checked");
      }
      else if(!strcmp(name,"radvdinterfacename"))
        {
        	req_format_write(wp,"%s",radvdCfgParam.interface.Name);
        }
      else if(!strcmp(name,"MaxRtrAdvInterval"))
        {
        	req_format_write(wp,"%u",radvdCfgParam.interface.MaxRtrAdvInterval);
        }
	else  if(!strcmp(name,"MinRtrAdvInterval"))
        {
        	req_format_write(wp,"%u",radvdCfgParam.interface.MinRtrAdvInterval);
        }
	else  if(!strcmp(name,("MinDelayBetweenRAs")))
        {
        	req_format_write(wp,"%u",radvdCfgParam.interface.MinDelayBetweenRAs);
        }
	else  if(!strcmp(name,("AdvManagedFlag")))
        {
		if(radvdCfgParam.interface.AdvManagedFlag)
        		req_format_write(wp,"checked");
        }
	 else if(!strcmp(name,("AdvOtherConfigFlag")))
        {
        	if(radvdCfgParam.interface.AdvOtherConfigFlag)
        		req_format_write(wp,"checked");
        }
	else  if(!strcmp(name,("AdvLinkMTU")))
        {
        	req_format_write(wp,"%d",radvdCfgParam.interface.AdvLinkMTU);
        }
	 else if(!strcmp(name,("AdvReachableTime")))
        {
        	req_format_write(wp,"%u",radvdCfgParam.interface.AdvReachableTime);
        }
	else  if(!strcmp(name,("AdvRetransTimer")))
        {
        	req_format_write(wp,"%u",radvdCfgParam.interface.AdvRetransTimer);
        }
	 else if(!strcmp(name,("AdvCurHopLimit")))
        {
        	req_format_write(wp,"%d",radvdCfgParam.interface.AdvCurHopLimit);
        }
	else  if(!strcmp(name,("AdvDefaultLifetime")))
        {
        	req_format_write(wp,"%d",radvdCfgParam.interface.AdvDefaultLifetime);
        }
	 else if(!strcmp(name,("AdvDefaultPreference_high")))
        {
        	if(!strcmp("high",radvdCfgParam.interface.AdvDefaultPreference))
			req_format_write(wp,"selected");	
         }
	 else if(!strcmp(name,("AdvDefaultPreference_medium")))
        {
        	if(!strcmp("medium",radvdCfgParam.interface.AdvDefaultPreference))
			req_format_write(wp,"selected");	
        }
	 else if(!strcmp(name,("AdvDefaultPreference_low")))
        {
        	if(!strcmp("low",radvdCfgParam.interface.AdvDefaultPreference))
			req_format_write(wp,"selected");	
        }
	 else if(!strcmp(name,("AdvSourceLLAddress")))
        {
                if(radvdCfgParam.interface.AdvSourceLLAddress)
        		req_format_write(wp,"checked");
        }
	else  if(!strcmp(name,("UnicastOnly")))
        {
                	if(radvdCfgParam.interface.UnicastOnly)
        		req_format_write(wp,"checked");
        }

	 /*prefix0*/
	else if(!strcmp(name,("Enabled_0")))
	{
		if(radvdCfgParam.interface.prefix[0].enabled)
			req_format_write(wp,"checked");
	}
	else if(!strcmp(name,("radvdprefix0_1")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[0].Prefix[0]);
	 }
	else if(!strcmp(name,("radvdprefix0_2")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[0].Prefix[1]);
	 }
	else if(!strcmp(name,("radvdprefix0_3")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[0].Prefix[2]);
	 }
	else if(!strcmp(name,("radvdprefix0_4")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[0].Prefix[3]);
	 }
	else if(!strcmp(name,("radvdprefix0_5")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[0].Prefix[4]);
	 }
	else if(!strcmp(name,("radvdprefix0_6")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[0].Prefix[5]);
	 }
	else if(!strcmp(name,("radvdprefix0_7")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[0].Prefix[6]);
	 }
	else if(!strcmp(name,("radvdprefix0_8")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[0].Prefix[7]);
	 }
	else if(!strcmp(name,("radvdprefix0_len")))
	 {
	 	req_format_write(wp,"%d",radvdCfgParam.interface.prefix[0].PrefixLen);
	 }


	else if(!strcmp(name,("AdvOnLinkFlag_0")))
	 {
	       if(radvdCfgParam.interface.prefix[0].AdvOnLinkFlag)
        		req_format_write(wp,"checked");
	 }
	
	else if(!strcmp(name,("AdvAutonomousFlag_0")))
	 {
	 	 if(radvdCfgParam.interface.prefix[0].AdvAutonomousFlag)
        		req_format_write(wp,"checked");
	 }
	else if(!strcmp(name,("AdvValidLifetime_0")))
	 {
	 	req_format_write(wp,"%u",radvdCfgParam.interface.prefix[0].AdvValidLifetime);
	 }
	else if(!strcmp(name,("AdvPreferredLifetime_0")))
	 {
	 	req_format_write(wp,"%u",radvdCfgParam.interface.prefix[0].AdvPreferredLifetime);
	 }
	else if(!strcmp(name,("AdvRouterAddr_0")))
	 {
	 	if(radvdCfgParam.interface.prefix[0].AdvRouterAddr)
        		req_format_write(wp,"checked");
	 }
	else if(!strcmp(name,("if6to4_0")))
	 {
	 	req_format_write(wp,"%s",radvdCfgParam.interface.prefix[0].if6to4);
	 }
	  
	 /*prefix1*/
	else if(!strcmp(name,("Enabled_1")))
	{
		if(radvdCfgParam.interface.prefix[1].enabled)
			req_format_write(wp,"checked");
	}
      else  if(!strcmp(name,("radvdprefix1_1")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[1].Prefix[0]);
	 }
	else if(!strcmp(name,("radvdprefix1_2")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[1].Prefix[1]);
	 }
	else if(!strcmp(name,("radvdprefix1_3")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[1].Prefix[2]);
	 }
	else if(!strcmp(name,("radvdprefix1_4")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[1].Prefix[3]);
	 }
	else if(!strcmp(name,("radvdprefix1_5")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[1].Prefix[4]);
	 }
	else if(!strcmp(name,("radvdprefix1_6")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[1].Prefix[5]);
	 }
	else if(!strcmp(name,("radvdprefix1_7")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[1].Prefix[6]);
	 }
	else if(!strcmp(name,("radvdprefix1_8")))
	 {
	 	req_format_write(wp,"%04x",radvdCfgParam.interface.prefix[1].Prefix[7]);
	 }
	else if(!strcmp(name,("radvdprefix1_len")))
	 {
	 	req_format_write(wp,"%d",radvdCfgParam.interface.prefix[1].PrefixLen);
	 }
	else if(!strcmp(name,("AdvOnLinkFlag_1")))
	 {
	 	if(radvdCfgParam.interface.prefix[1].AdvOnLinkFlag)
			req_format_write(wp,"checked");
	 }
	else if(!strcmp(name,("AdvAutonomousFlag_1")))
	 {
	 	 if(radvdCfgParam.interface.prefix[1].AdvAutonomousFlag)
			req_format_write(wp,"checked");
	 }
	else if(!strcmp(name,("AdvValidLifetime_1")))
	 {
	 	req_format_write(wp,"%u",radvdCfgParam.interface.prefix[1].AdvValidLifetime);
	 }
	else if(!strcmp(name,("AdvPreferredLifetime_1")))
	 {
	 	req_format_write(wp,"%u",radvdCfgParam.interface.prefix[1].AdvPreferredLifetime);
	 }
	else if(!strcmp(name,("AdvRouterAddr_1")))
	 {
	 	 if(radvdCfgParam.interface.prefix[1].AdvRouterAddr)
			req_format_write(wp,"checked");
	 }
	else if(!strcmp(name,("if6to4_1")))
	 {
	 	req_format_write(wp,"%s",radvdCfgParam.interface.prefix[1].if6to4);
	 }
////////////dnsmasq///////////////////////////////////////
	if(getDnsv6Info(&dnsv6CfgParam)<0)
	{
		req_format_write(wp,"Read Dnsmasq Configuration Error");
		return -1;
	}
#if 0
	 if(!strcmp(name,("enable_dnsv6")))
        {
        	if(dnsv6CfgParam.enabled)
        		req_format_write(wp,"checked");
        }
#endif
        else if(!strcmp(name,("routername")))
        {
        	req_format_write(wp,"%s",dnsv6CfgParam.routerName);
        }
///////////////DHCPv6 server//////////////////////////////////////
	if(getDhcpv6sInfo(&dhcp6sCfgParam)<0)
	{
		req_format_write(wp,"Read Dnsmasq Configuration Error");
		return -1;
	}	
	
	if(!strcmp(name,("enable_dhcpv6s")))
      {
      		if(dhcp6sCfgParam.enabled)
      			req_format_write(wp,"checked");
      }
      else if(!strcmp(name,("interfacenameds")))
      {
      		req_format_write(wp,"%s",dhcp6sCfgParam.interfaceNameds);
      }
	else if(!strcmp(name,("dnsaddr")))
      {
      		req_format_write(wp,"%s",dhcp6sCfgParam.DNSaddr6);
      }	
	else if(!strcmp(name,("addrPoolStart")))
      {
      		req_format_write(wp,"%s",dhcp6sCfgParam.addr6PoolS);
      }	
	else if(!strcmp(name,("addrPoolEnd")))
      {
      		req_format_write(wp,"%s",dhcp6sCfgParam.addr6PoolE);
      }	
	
	
	///////////////Tunnel//////////////////////////////////////
	if(!strcmp(name,("enable_tunnel6")))
      {
      		if(getTunnel6Info(&tunnelCfgParam)<0)
		{
			req_format_write(wp,"Read Tunnel Configuration Error");
			return -1;
		}	
      		if(tunnelCfgParam.enabled)
	      		req_format_write(wp,"checked");
      }	
	return 0;
}

uint32 getIPv6WanInfo(request *wp, int argc, char **argv)
{
	char	*name;
	addr6CfgParam_t	addr6_wan;
	addr6CfgParam_t	addr6_lan;
	addr6CfgParam_t addr6_gw;
	addr6CfgParam_t addr6_dns;
	addr6CfgParam_t addr6_prefix;
	dhcp6cCfgParam_t dhcp6cCfgParam;
	int val;

	name = argv[0];
	if (name == NULL) {
   		fprintf(stderr, "Insufficient args\n");
   		return -1;
   	}

	if(!strcmp(name,"ipv6WanEnabled")){
		if(!apmib_get(MIB_IPV6_WAN_ENABLE,&val)){
			fprintf(stderr, "Read MIB_IPV6_WAN_ENABLE Error\n");	
			return -1;			
		}
		else{
			req_format_write(wp,"%d",val);
		}
	}	
	else if(!strcmp(name,"ipv6Origin")){
			if(!apmib_get(MIB_IPV6_ORIGIN_TYPE,&val)){
				fprintf(stderr, "Read MIB_IPV6_ORIGIN_TYPE Error\n");
				return -1;			
			}
			else{
				req_format_write(wp,"%d",val);
			}
	}
	else if(!strcmp(name,"ipv6LinkType")){
			if(!apmib_get(MIB_IPV6_LINK_TYPE,&val)){
				fprintf(stderr, "Read MIB_IPV6_LINK_TYPE Error\n");
				return -1;			
			}
			else{
				req_format_write(wp,"%d",val);
			}
		}
	else if(!strcmp(name,"wan_ipv6DnsAuto")){
			if(!apmib_get(MIB_IPV6_DNS_AUTO,&val)){
				fprintf(stderr, "Read MIB_IPV6_DNS_AUTO Error\n");
				return -1;			
			}
			else{
				req_format_write(wp,"%d",val);
			}
		}
		else if(!strcmp(name,"wan_ipv6DhcpMode")){
				if(!apmib_get(MIB_IPV6_DHCP_MODE,&val)){
				fprintf(stderr, "Read MIB_IPV6_DHCP_MODE Error\n");
				return -1;			
			}
			else{
				req_format_write(wp,"%d",val);
			}
		}
	else if(!strcmp(name,"wan_duid")){
		struct duid_t dhcp6c_duid={0};
		struct sockaddr hwaddr={0};
		dhcp6c_duid.duid_type=3;
		dhcp6c_duid.hw_type=1;
		if ( getInAddr(WAN_IF, HW_ADDR, (void *)&hwaddr )==0)
		{
				fprintf(stderr, "Read hwaddr Error\n");
				return -1;	
		}
		memcpy(dhcp6c_duid.mac,hwaddr.sa_data,6);
		req_format_write(wp,"%04x%04x%02x%02x%02x%02x%02x%02x",dhcp6c_duid.duid_type,dhcp6c_duid.hw_type,dhcp6c_duid.mac[0],dhcp6c_duid.mac[1],dhcp6c_duid.mac[2],dhcp6c_duid.mac[3],dhcp6c_duid.mac[4],dhcp6c_duid.mac[5]);
	}
	/*lan ipv6 address*/
	else if(getLanAddv6Info(&addr6_lan)<0){
		fprintf(stderr, "Read lan addr6 mib Error\n");		
		return -1;
	}
	if(!strcmp(name,"lan_ipv6Addr_0")){
		req_format_write(wp,"%04x",addr6_lan.addrIPv6[0]);
	}
	else if(!strcmp(name,"lan_ipv6Addr_1")){
		req_format_write(wp,"%04x",addr6_lan.addrIPv6[1]);
	}
	else if(!strcmp(name,"lan_ipv6Addr_2")){
		req_format_write(wp,"%04x",addr6_lan.addrIPv6[2]);
	}
	else if(!strcmp(name,"lan_ipv6Addr_3")){
		req_format_write(wp,"%04x",addr6_lan.addrIPv6[3]);
	}
	else if(!strcmp(name,"lan_ipv6Addr_4")){
		req_format_write(wp,"%04x",addr6_lan.addrIPv6[4]);
	}
	else if(!strcmp(name,"lan_ipv6Addr_5")){
		req_format_write(wp,"%04x",addr6_lan.addrIPv6[5]);
	}
	else if(!strcmp(name,"lan_ipv6Addr_6")){
		req_format_write(wp,"%04x",addr6_lan.addrIPv6[6]);
	}
	else if(!strcmp(name,"lan_ipv6Addr_7")){
		req_format_write(wp,"%04x",addr6_lan.addrIPv6[7]);
	}
	else if(!strcmp(name,"lan_ipv6Addr_prefixLen")){
		req_format_write(wp,"%d",addr6_lan.prefix_len);
	}
	else if(!strcmp(name,("enable_dhcpv6pd")))
    {
    	if(!apmib_get(MIB_IPV6_DHCP_PD_ENABLE,&val)){
			fprintf(stderr, "Read MIB_IPV6_DHCP_PD_ENABLE Error\n");
			return -1;			
		}
      	if(val)
      		req_format_write(wp,"true");
		else
			req_format_write(wp,"false");
    }
	else if(!strcmp(name,("enable_dhcpv6RapidCommit")))
    {
    	if(!apmib_get(MIB_IPV6_DHCP_RAPID_COMMIT_ENABLE,&val)){
			fprintf(stderr, "Read MIB_IPV6_DHCP_RAPID_COMMIT_ENABLE Error\n");
			return -1;			
		}
      	if(val)
      		req_format_write(wp,"true");
		else
			req_format_write(wp,"false");
    }
#if 0
	else if(getDhcpv6cInfo(&dhcp6cCfgParam)<0)
	{
		fprintf(stderr, "Read dhcp6c mib Error\n");		
		//req_format_write(wp,"Read dhcp6c mib Error");
		return -1;
	}
	else if(!strcmp(name,("enable_dhcpv6pd")))
    {
      	if(dhcp6cCfgParam.enabled)
      		req_format_write(wp,"true");
		else
			req_format_write(wp,"false");
    }
	else if(!strcmp(name,("interfacename_pd")))
    {
      		req_format_write(wp,"%s",dhcp6cCfgParam.dhcp6pd.ifName);
    }	
	else if(!strcmp(name,("sla_len")))
    {
      		req_format_write(wp,"%d",dhcp6cCfgParam.dhcp6pd.sla_len);
    }
	else if(!strcmp(name,("sla_id")))
    {
      		req_format_write(wp,"%d",dhcp6cCfgParam.dhcp6pd.sla_id);
    }
#endif	

	else if(!strcmp(name,"mldproxyDisabled")){
		if(!apmib_get(MIB_MLD_PROXY_DISABLED,&val)){
			fprintf(stderr, "Read MIB_MLD_PROXY_DISABLED Error\n");
			return -1;			
		}
		else{
			req_format_write(wp,"%d",val);
		}
	}
	
	/*wan ipv6 address*/
	else if(getWanAdd6Info(&addr6_wan)<0){
		fprintf(stderr, "Read wan addr6 mib Error\n");		
		return -1;
	}
	else if(!strcmp(name,"wan_ipv6Addr_0")){
		req_format_write(wp,"%04x",addr6_wan.addrIPv6[0]);
	}
	else if(!strcmp(name,"wan_ipv6Addr_1")){
		req_format_write(wp,"%04x",addr6_wan.addrIPv6[1]);
	}
	else if(!strcmp(name,"wan_ipv6Addr_2")){
		req_format_write(wp,"%04x",addr6_wan.addrIPv6[2]);
	}
	else if(!strcmp(name,"wan_ipv6Addr_3")){
		req_format_write(wp,"%04x",addr6_wan.addrIPv6[3]);
	}
	else if(!strcmp(name,"wan_ipv6Addr_4")){
		req_format_write(wp,"%04x",addr6_wan.addrIPv6[4]);
	}
	else if(!strcmp(name,"wan_ipv6Addr_5")){
		req_format_write(wp,"%04x",addr6_wan.addrIPv6[5]);
	}
	else if(!strcmp(name,"wan_ipv6Addr_6")){
		req_format_write(wp,"%04x",addr6_wan.addrIPv6[6]);
	}
	else if(!strcmp(name,"wan_ipv6Addr_7")){
		req_format_write(wp,"%04x",addr6_wan.addrIPv6[7]);
	}
	else if(!strcmp(name,"wan_ipv6Addr_prefixLen")){
		req_format_write(wp,"%d",addr6_wan.prefix_len);
	}

	/*gateway*/
	else if(getGatewayv6Info(&addr6_gw)<0){
		fprintf(stderr, "Read gw addr6 mib Error\n");
		return -1;
	}
	if(!strcmp(name,"wan_ipv6Gw_0")){
		req_format_write(wp,"%04x",addr6_gw.addrIPv6[0]);
	}
	else if(!strcmp(name,"wan_ipv6Gw_1")){
		req_format_write(wp,"%04x",addr6_gw.addrIPv6[1]);
	}
	else if(!strcmp(name,"wan_ipv6Gw_2")){
		req_format_write(wp,"%04x",addr6_gw.addrIPv6[2]);
	}
	else if(!strcmp(name,"wan_ipv6Gw_3")){
		req_format_write(wp,"%04x",addr6_gw.addrIPv6[3]);
	}
	else if(!strcmp(name,"wan_ipv6Gw_4")){
		req_format_write(wp,"%04x",addr6_gw.addrIPv6[4]);
	}
	else if(!strcmp(name,"wan_ipv6Gw_5")){
		req_format_write(wp,"%04x",addr6_gw.addrIPv6[5]);
	}
	else if(!strcmp(name,"wan_ipv6Gw_6")){
		req_format_write(wp,"%04x",addr6_gw.addrIPv6[6]);
	}
	else if(!strcmp(name,"wan_ipv6Gw_7")){
		req_format_write(wp,"%04x",addr6_gw.addrIPv6[7]);
	}
	else if(!strcmp(name,"wan_ipv6Gw_prefixLen")){
		req_format_write(wp,"%d",addr6_gw.prefix_len);
	}

	/*dns*/
	else if(getDnsAddv6Info(&addr6_dns)<0){
		fprintf(stderr, "Read dns addr6 mib Error\n");
		return -1;
	}
	else if(!strcmp(name,"wan_ipv6Dns1_0")){
		req_format_write(wp,"%04x",addr6_dns.addrIPv6[0]);
	}
	else if(!strcmp(name,"wan_ipv6Dns1_1")){
		req_format_write(wp,"%04x",addr6_dns.addrIPv6[1]);
	}
	else if(!strcmp(name,"wan_ipv6Dns1_2")){
		req_format_write(wp,"%04x",addr6_dns.addrIPv6[2]);
	}
	else if(!strcmp(name,"wan_ipv6Dns1_3")){
		req_format_write(wp,"%04x",addr6_dns.addrIPv6[3]);
	}
	else if(!strcmp(name,"wan_ipv6Dns1_4")){
		req_format_write(wp,"%04x",addr6_dns.addrIPv6[4]);
	}
	else if(!strcmp(name,"wan_ipv6Dns1_5")){
		req_format_write(wp,"%04x",addr6_dns.addrIPv6[5]);
	}
	else if(!strcmp(name,"wan_ipv6Dns1_6")){
		req_format_write(wp,"%04x",addr6_dns.addrIPv6[6]);
	}
	else if(!strcmp(name,"wan_ipv6Dns1_7")){
		req_format_write(wp,"%04x",addr6_dns.addrIPv6[7]);
	}
	else if(!strcmp(name,"wan_ipv6Dns1_prefixLen")){
		req_format_write(wp,"%d",addr6_dns.prefix_len);
	}

	/*prefix*/
	else if(getLanv6PrefixInfo(&addr6_prefix)<0){
		fprintf(stderr, "Read Lan addr6 mib Error\n");
		return -1;
	}
	else if(!strcmp(name,"child_prefixAddr_0")){
		req_format_write(wp,"%04x",addr6_prefix.addrIPv6[0]);
	}
	else if(!strcmp(name,"child_prefixAddr_1")){
		req_format_write(wp,"%04x",addr6_prefix.addrIPv6[1]);
	}
	else if(!strcmp(name,"child_prefixAddr_2")){
		req_format_write(wp,"%04x",addr6_prefix.addrIPv6[2]);
	}
	else if(!strcmp(name,"child_prefixAddr_3")){
		req_format_write(wp,"%04x",addr6_prefix.addrIPv6[3]);
	}
	else if(!strcmp(name,"child_prefixAddr_4")){
		req_format_write(wp,"%04x",addr6_prefix.addrIPv6[4]);
	}
	else if(!strcmp(name,"child_prefixAddr_5")){
		req_format_write(wp,"%04x",addr6_prefix.addrIPv6[5]);
	}
	else if(!strcmp(name,"child_prefixAddr_6")){
		req_format_write(wp,"%04x",addr6_prefix.addrIPv6[6]);
	}
	else if(!strcmp(name,"child_prefixAddr_7")){
		req_format_write(wp,"%04x",addr6_prefix.addrIPv6[7]);
	}
	else if(!strcmp(name,"prefix_len_childPrefixAddr")){
		req_format_write(wp,"%d",addr6_prefix.prefix_len);
	}
	
	return 0;
}



int getIPv6Status(request *wp, int argc, char **argv)
{

	FILE *fp=NULL;
	struct in6_addr	addr6;
	struct in6_addr	src_addr;
	int src_prefix;
	struct in6_addr	dst_addr;
	int dst_prefix;
	struct in6_addr	gw_addr;
	struct in6_addr	any_addr;
	uint32 metrix;
	uint32 ref_cnt;
	uint32 usr_cnt;
	uint32 flag;
	char ifname[IFNAMESIZE];
	char devname[IFNAMESIZE];
	char src[64];
	char dst[64];
	char gw[64];
	int ret;
	char	*name;
	int val;
	int	if_index;
	int	prefix_len;
	int	if_scope;
	int	if_flag;
	
	name = argv[0];
	if (name == NULL) {
   		fprintf(stderr, "Insufficient args\n");
   		return -1;
   	}

	if(!strcmp(name,"ipv6LinkType")){
		if(!apmib_get(MIB_IPV6_LINK_TYPE,&val)){
			fprintf(stderr, "Read MIB_IPV6_LINK_TYPE Error\n");
			return -1;			
		}
		else{
			if(val==IPV6_LINKTYPE_IP)
				req_format_write(wp,"%s","IP link");
			else
				req_format_write(wp,"%s","PPP link");
		}
	}
	
	if(!strcmp(name,"ipv6Origin")){
		if(!apmib_get(MIB_IPV6_ORIGIN_TYPE,&val)){
			fprintf(stderr, "Read MIB_IPV6_ORIGIN_TYPE Error\n");
			return -1;			
		}
		else{
			if(val==IPV6_ORIGIN_DHCP)
				req_format_write(wp,"%s","DHCPv6");
			else if(val==IPV6_ORIGIN_STATIC)
				req_format_write(wp,"%s","STATIC");
		}
	}

	if(!apmib_get(MIB_IPV6_LINK_TYPE,&val)){
		fprintf(stderr, "Read MIB_IPV6_ORIGIN_TYPE Error\n");				
		return -1;			
	}
	
	if(val==IPV6_LINKTYPE_IP)
		sprintf(ifname,"eth1");
	else
		sprintf(ifname,"ppp0");
	
	if(!strcmp(name,"gw_addr6"))
	{		
		fp = fopen(IPV6_ROUTE_PROC,"r");
		if(fp!=NULL){			
			memset(&any_addr,0,sizeof(any_addr));						
			while((ret=fscanf(fp,"%s %x %s %x %s %x %x %x %x %s",
						dst,&dst_prefix,src,&src_prefix,gw,
						&metrix,&ref_cnt,&usr_cnt,&flag,devname))!=EOF){	
				/*return value must be parameter's number*/							
				if(ret!=10){								
					continue;
				}
				/*interface match?*/	
				if(strcmp(ifname,devname)==0){							
					ret=sscanf(dst,"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
								&dst_addr.s6_addr[ 0], &dst_addr.s6_addr[ 1], &dst_addr.s6_addr[ 2], &dst_addr.s6_addr[ 3],
								&dst_addr.s6_addr[ 4], &dst_addr.s6_addr[ 5], &dst_addr.s6_addr[ 6], &dst_addr.s6_addr[ 7],
								&dst_addr.s6_addr[ 8], &dst_addr.s6_addr[ 9], &dst_addr.s6_addr[10], &dst_addr.s6_addr[11],
								&dst_addr.s6_addr[12], &dst_addr.s6_addr[13], &dst_addr.s6_addr[14], &dst_addr.s6_addr[15]);

					/*default route?*/
					if(memcmp(&dst_addr,&any_addr,sizeof(struct in6_addr))==0){
						sscanf(gw,"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
							&gw_addr.s6_addr[ 0], &gw_addr.s6_addr[ 1], &gw_addr.s6_addr[ 2], &gw_addr.s6_addr[ 3],
							&gw_addr.s6_addr[ 4], &gw_addr.s6_addr[ 5], &gw_addr.s6_addr[ 6], &gw_addr.s6_addr[ 7],
							&gw_addr.s6_addr[ 8], &gw_addr.s6_addr[ 9], &gw_addr.s6_addr[10], &gw_addr.s6_addr[11],
							&gw_addr.s6_addr[12], &gw_addr.s6_addr[13], &gw_addr.s6_addr[14], &gw_addr.s6_addr[15]);
						if(gw_addr.s6_addr16[0]==0xFE80){										
							req_format_write(wp,"%s",gw);									
							break;
						}
					}								
				}												
			}
			fclose(fp);			
			return 0;
		}
		else{
			return -1;
		}
	}

	if(!strcmp(name,"wan_addr6_global"))
	{
		fp = fopen(IPV6_ADDR_PROC,"r");
		if(fp!=NULL){
			while((ret=fscanf(fp,"%s %x %x %x %x %s",src,&if_index,
					&prefix_len,&if_scope,&if_flag,devname))!=EOF){
				if(ret!=6)
					continue;
				
				/*interface match?*/	
				if(strcmp(ifname,devname)==0){									
					ret=sscanf(src,"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
								&addr6.s6_addr[ 0], &addr6.s6_addr[ 1], &addr6.s6_addr[ 2], &addr6.s6_addr[ 3],
								&addr6.s6_addr[ 4], &addr6.s6_addr[ 5], &addr6.s6_addr[ 6], &addr6.s6_addr[ 7],
								&addr6.s6_addr[ 8], &addr6.s6_addr[ 9], &addr6.s6_addr[10], &addr6.s6_addr[11],
								&addr6.s6_addr[12], &addr6.s6_addr[13], &addr6.s6_addr[14], &addr6.s6_addr[15]);
					if(addr6.s6_addr16[0]!=0xFE80){
						req_format_write(wp,"%s/%d",src,prefix_len);
						break;
					}
				}
			}
			fclose(fp);
			return 0;
		}
		else{
			return -1;
		}
	}
		
	if(!strcmp(name,"wan_addr6_ll"))
	{
		fp = fopen(IPV6_ADDR_PROC,"r");
		if(fp!=NULL){
			while((ret=fscanf(fp,"%s %x %x %x %x %s",src,&if_index,
				&prefix_len,&if_scope,&if_flag,devname))!=EOF){				
				if(ret!=6)
					continue;
					
				/*interface match?*/	
				if(strcmp(ifname,devname)==0){									
					ret=sscanf(src,"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
								&addr6.s6_addr[ 0], &addr6.s6_addr[ 1], &addr6.s6_addr[ 2], &addr6.s6_addr[ 3],
								&addr6.s6_addr[ 4], &addr6.s6_addr[ 5], &addr6.s6_addr[ 6], &addr6.s6_addr[ 7],
								&addr6.s6_addr[ 8], &addr6.s6_addr[ 9], &addr6.s6_addr[10], &addr6.s6_addr[11],
								&addr6.s6_addr[12], &addr6.s6_addr[13], &addr6.s6_addr[14], &addr6.s6_addr[15]
								);
					if(addr6.s6_addr16[0]==0xFE80){
						req_format_write(wp,"%s/%d",src,prefix_len);
						break;
					}
				}
			}
			fclose(fp);
			return 0;	
		}
		else{
			return -1;
		}
	}
	
	if(!strcmp(name,"lan_addr6_global"))
	{
		fp = fopen(IPV6_ADDR_PROC,"r");
		if(fp!=NULL){
			while((ret=fscanf(fp,"%s %x %x %x %x %s",src,&if_index,
				&prefix_len,&if_scope,&if_flag,devname))!=EOF){
				if(ret!=6)
					continue;
				/*interface match?*/	
				if(strcmp("br0",devname)==0){									
					ret=sscanf(src,"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
								&addr6.s6_addr[ 0], &addr6.s6_addr[ 1], &addr6.s6_addr[ 2], &addr6.s6_addr[ 3],
								&addr6.s6_addr[ 4], &addr6.s6_addr[ 5], &addr6.s6_addr[ 6], &addr6.s6_addr[ 7],
								&addr6.s6_addr[ 8], &addr6.s6_addr[ 9], &addr6.s6_addr[10], &addr6.s6_addr[11],
								&addr6.s6_addr[12], &addr6.s6_addr[13], &addr6.s6_addr[14], &addr6.s6_addr[15]);
					if(addr6.s6_addr16[0]!=0xFE80){
						req_format_write(wp,"%s/%d",src,prefix_len);
						break;
					}
				}
			}
			fclose(fp);
			return 0;	
		}
		else{
			return -1;
		}
	}

	if(!strcmp(name,"lan_addr6_ll"))
	{
		fp = fopen(IPV6_ADDR_PROC,"r");
		if(fp!=NULL){
			while((ret=fscanf(fp,"%s %x %x %x %x %s",src,&if_index,
					&prefix_len,&if_scope,&if_flag,devname))!=EOF){
				if(ret!=6)
					continue;
				
				/*interface match?*/	
				if(strcmp("br0",devname)==0){									
					ret=sscanf(src,"%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx%02hhx",
								&addr6.s6_addr[ 0], &addr6.s6_addr[ 1], &addr6.s6_addr[ 2], &addr6.s6_addr[ 3],
								&addr6.s6_addr[ 4], &addr6.s6_addr[ 5], &addr6.s6_addr[ 6], &addr6.s6_addr[ 7],
								&addr6.s6_addr[ 8], &addr6.s6_addr[ 9], &addr6.s6_addr[10], &addr6.s6_addr[11],
								&addr6.s6_addr[12], &addr6.s6_addr[13], &addr6.s6_addr[14], &addr6.s6_addr[15]
								);
					if(addr6.s6_addr16[0]==0xFE80){
						req_format_write(wp,"%s/%d",src,prefix_len);
						break;
					}
				}
			}
			fclose(fp);
			return 0;	
		}
		else{
			return -1;
		}
	}
}

int getIPv6BasicInfo(request *wp, int argc, char **argv)
{
	char	*name;
	addrIPv6CfgParam_t addrIPv6CfgParam;
	
	//printf("get parameter=%s\n", argv[0]);
	name = argv[0];
	if (name == NULL) {
   		fprintf(stderr, "Insufficient args\n");
   		return -1;
   	}

	if(getAddr6Info(&addrIPv6CfgParam)<0)
	{
		req_format_write(wp,"Read Dnsmasq Configuration Error");
		return -1;
	}
	
	if(!strcmp(name,("addrIPv6_1_1")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[0][0]);
      }
	else if(!strcmp(name,("addrIPv6_1_2")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[0][1]);
      }
	else if(!strcmp(name,("addrIPv6_1_3")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[0][2]);
      }
	else if(!strcmp(name,("addrIPv6_1_4")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[0][3]);
      }
	else if(!strcmp(name,("addrIPv6_1_5")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[0][4]);
      }
	else if(!strcmp(name,("addrIPv6_1_6")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[0][5]);
      }
	else if(!strcmp(name,("addrIPv6_1_7")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[0][6]);
      }
	else if(!strcmp(name,("addrIPv6_1_8")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[0][7]);
      }
	else if(!strcmp(name,("prefix_len_1")))
      {
		req_format_write(wp,"%d",addrIPv6CfgParam.prefix_len[0]);
      }

	if(!strcmp(name,("addrIPv6_2_1")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[1][0]);
      }
	else if(!strcmp(name,("addrIPv6_2_2")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[1][1]);
      }
	else if(!strcmp(name,("addrIPv6_2_3")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[1][2]);
      }
	else if(!strcmp(name,("addrIPv6_2_4")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[1][3]);
      }
	else if(!strcmp(name,("addrIPv6_2_5")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[1][4]);
      }
	else if(!strcmp(name,("addrIPv6_2_6")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[1][5]);
      }
	else if(!strcmp(name,("addrIPv6_2_7")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[1][6]);
      }
	else if(!strcmp(name,("addrIPv6_2_8")))
      {
		req_format_write(wp,"%04x",addrIPv6CfgParam.addrIPv6[1][7]);
      }
	else if(!strcmp(name,("prefix_len_2")))
      {
		req_format_write(wp,"%d",addrIPv6CfgParam.prefix_len[1]);
      }
	return 0;
}

#endif
#endif
