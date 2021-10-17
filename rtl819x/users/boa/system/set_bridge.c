/*
 *      Utiltiy function for setting bridge
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/stat.h>


#include "apmib.h"
#include "sysconf.h"
#include "sys_utility.h"
extern int wlan_idx;	// interface index
extern int vwlan_idx;	// initially set interface index to root
extern int apmib_initialized;
#define BR_IFACE_FILE "/var/system/br_iface"
#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
#define BR_IFACE_FILE2 "/var/system/br_iface2"
#endif
#define MESH_PATHSEL "/bin/pathsel"
#define BR_INIT_FILE "/tmp/bridge_init"
#define ETH_VLAN_SWITCH "/proc/disable_l2_table"
#define DHCPD_CONF_FILE "/var/udhcpd.conf"
#define DHCPD_LEASE_FILE "/var/lib/misc/udhcpd.leases"

//#define SDEBUG(fmt, args...) printf("[%s %d]"fmt,__FUNCTION__,__LINE__,## args)
#define SDEBUG(fmt, args...) {}

int SetWlan_idx(char * wlan_iface_name);

char wlan_wan_iface[20];
/*

//eth0 eth1 eth2 eth3 eth4 wlan0 wlan0-msh wlan0-va0 wlan0-va1 wlan0-va2 wlan0-va3 wlan0-vxd
//wlan0-wds0 wlan0-wds1 wlan0-wds2 wlan0-wds3 wlan0-wds4 wlan0-wds5 wlan0-wds6 wlan0-wds7
*/

void start_lan_dhcpd(char *interface)
{
	char tmpBuff1[32]={0}, tmpBuff2[32]={0};
	int intValue=0, dns_mode=0;
	char line_buffer[100]={0};
	char tmp1[64]={0};
	char tmp2[64]={0};
	char *strtmp=NULL, *strtmp1=NULL;
	DHCPRSVDIP_T entry;
	int i, entry_Num=0;
#ifdef   HOME_GATEWAY
	char tmpBuff3[32]={0};
#endif
	sprintf(line_buffer,"interface %s\n",interface);
	write_line_to_file(DHCPD_CONF_FILE, 1, line_buffer);

	apmib_get(MIB_DHCP_CLIENT_START,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"start %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	apmib_get(MIB_DHCP_CLIENT_END,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"end %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	apmib_get(MIB_SUBNET_MASK,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"opt subnet %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	apmib_get(MIB_DHCP_LEASE_TIME, (void *)&intValue);
    if( (intValue==0) || (intValue==1) || (intValue<0) || (intValue>10080))
    {
	printf("===%s(%d), read MIB_DHCP_LEASE_TIME is %d\n", __FUNCTION__, __LINE__, intValue);
        //intValue = 480; //8 hours
        if(!apmib_set(MIB_DHCP_LEASE_TIME, (void *)&intValue))
        {
        	printf("set MIB_DHCP_LEASE_TIME error\n");
        }

		apmib_update(CURRENT_SETTING);
    }
	intValue *= 60;
    sprintf(line_buffer,"opt lease %ld\n",intValue);
    write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

		apmib_get(MIB_IP_ADDR,  (void *)tmp1);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(line_buffer,"opt router %s\n",strtmp);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

		sprintf(line_buffer,"opt dns %s\n",strtmp); /*now strtmp is ip address value */
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

		memset(tmp1, 0x00, 64);
		apmib_get( MIB_DOMAIN_NAME, (void *)&tmp1);
		if(tmp1[0]){
			sprintf(line_buffer,"opt domain %s\n",tmp1);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}

	/* may not need to set ip again*/
	apmib_get(MIB_IP_ADDR,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(tmpBuff1, "%s", strtmp);
	apmib_get(MIB_SUBNET_MASK,  (void *)tmp2);
	strtmp1= inet_ntoa(*((struct in_addr *)tmp2));
	sprintf(tmpBuff2, "%s", strtmp1);
	RunSystemCmd(NULL_FILE, "ifconfig", interface, tmpBuff1, "netmask", tmpBuff2,  NULL_STR);
	/*start dhcp server*/
	RunSystemCmd(NULL_FILE, "udhcpd", DHCPD_CONF_FILE, NULL_STR);


}


int SetWlan_idx(char * wlan_iface_name)
{
	int idx;

		idx = atoi(&wlan_iface_name[4]);
		if (idx >= NUM_WLAN_INTERFACE) {
				printf("invalid wlan interface index number!\n");
				return 0;
		}
		wlan_idx = idx;
		vwlan_idx = 0;

#ifdef MBSSID

		if (strlen(wlan_iface_name) >= 9 && wlan_iface_name[5] == '-' &&
				wlan_iface_name[6] == 'v' && wlan_iface_name[7] == 'a') {
				idx = atoi(&wlan_iface_name[8]);
				if (idx >= NUM_VWLAN_INTERFACE) {
					printf("invalid virtual wlan interface index number!\n");
					return 0;
				}

				vwlan_idx = idx+1;
				idx = atoi(&wlan_iface_name[4]);
				wlan_idx = idx;
		}
#endif

#ifdef UNIVERSAL_REPEATER
				if (strlen(wlan_iface_name) >= 9 && wlan_iface_name[5] == '-' &&
						!memcmp(&wlan_iface_name[6], "vxd", 3)) {
					vwlan_idx = NUM_VWLAN_INTERFACE;
					idx = atoi(&wlan_iface_name[4]);
					wlan_idx = idx;
				}
#endif

//printf("\r\n wlan_iface_name=[%s],wlan_idx=[%u],vwlan_idx=[%u],__[%s-%u]\r\n",wlan_iface_name,wlan_idx,vwlan_idx,__FILE__,__LINE__);

return 1;
}

#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
int create_dhcpd_configfile(char *interface)
{	
	DHCPS_SERVING_POOL_T entry;
	int entrynum, i;
	int k=1, count=0;
	char line_buffer[512]={0};

	int j, entry_Num, enabled;
	DHCPRSVDIP_T sdhcp_entry;
	
	if(!apmib_get(MIB_DHCPS_SERVING_POOL_TBL_NUM, (void *)&entrynum))
	{
		printf("%s:%d apmib_get MIB_DHCPS_SERVING_POOL_TBL_NUM fails!####\n",__FUNCTION__,__LINE__);
		return 0;
	}
	for (i=1; i<=entrynum; i++) 
	{
		*((char *)&entry) = (char)i;
		if(!apmib_get(MIB_DHCPS_SERVING_POOL_TBL, (void *)&entry))
			continue;		

		if(entry.enable==0)
			continue;

		if(!(entry.startaddr>0 && entry.endaddr>0 && entry.endaddr>entry.startaddr))
			continue;

		count++;
		
		if(strlen(entry.poolname)>0)
			sprintf(line_buffer,"poolname %s\n", entry.poolname);
		else
			sprintf(line_buffer,"poolname pool%d\n", count);
		write_line_to_file(DHCPD_CONF_FILE, k, line_buffer);

		if(entry.poolorder>0)
		{
			sprintf(line_buffer,"poolorder %d\n", entry.poolorder);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.sourceinterface>0)
		{
			sprintf(line_buffer,"sourceinterface %d\n", entry.sourceinterface);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(strlen(entry.vendorclass)>0)
		{
			sprintf(line_buffer,"vendorclass %s\n", entry.vendorclass);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.vendorclassflag>0)
		{
			sprintf(line_buffer,"vendorclassflag %d\n", entry.vendorclassflag);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}		
		if(strlen(entry.vendorclassmode)>0)
		{
			sprintf(line_buffer,"vendorclassmode %s\n", entry.vendorclassmode);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(strlen(entry.clientid)>0)
		{
			sprintf(line_buffer,"clientid %s\n", entry.clientid);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.clientidflag>0)
		{
			sprintf(line_buffer,"clientidflag %d\n", entry.clientidflag);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(strlen(entry.userclass)>0)
		{
			sprintf(line_buffer,"userclass %s\n", entry.userclass);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.userclassflag>0)
		{
			sprintf(line_buffer,"userclassflag %d\n", entry.userclassflag);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(strlen(entry.chaddr)>0)
		{
			sprintf(line_buffer,"chaddr %s\n", entry.chaddr);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(strlen(entry.chaddrmask)>0)
		{
			sprintf(line_buffer,"chaddrmask %s\n", entry.chaddrmask);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.chaddrflag>0)
		{
			sprintf(line_buffer,"chaddrflag %d\n", entry.chaddrflag);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}

		sprintf(line_buffer,"interface %s\n", interface);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		
		sprintf(line_buffer,"start %s\n", inet_ntoa(*((struct in_addr *)&entry.startaddr)));
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		
		sprintf(line_buffer,"end %s\n", inet_ntoa(*((struct in_addr *)&entry.endaddr)));
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		if(entry.subnetmask>0)
		{
			sprintf(line_buffer,"opt subnet %s\n", inet_ntoa(*((struct in_addr *)&entry.subnetmask)));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.iprouter>0)
		{
			sprintf(line_buffer,"opt router %s\n", inet_ntoa(*((struct in_addr *)&entry.iprouter)));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.dnsserver1>0)
		{
			sprintf(line_buffer,"opt dns %s\n", inet_ntoa(*((struct in_addr *)&entry.dnsserver1)));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.dnsserver2>0)
		{
			sprintf(line_buffer,"opt dns %s\n", inet_ntoa(*((struct in_addr *)&entry.dnsserver2)));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.dnsserver3>0)
		{
			sprintf(line_buffer,"opt dns %s\n", inet_ntoa(*((struct in_addr *)&entry.dnsserver3)));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(strlen(entry.domainname)>0)
		{
			sprintf(line_buffer,"opt domain %s\n", entry.domainname);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		if(entry.leasetime>0)
		{
			sprintf(line_buffer,"opt lease %u\n", entry.leasetime*60);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}	

		/*static dhcp entry static_lease 000102030405 192.168.1.199*/
		apmib_get(MIB_DHCPRSVDIP_ENABLED, (void *)&enabled);
		if(enabled==1)
		{
			apmib_get(MIB_DHCPRSVDIP_TBL_NUM, (void *)&entry_Num);
			if(entry_Num>0){
				for (j=1; j<=entry_Num; j++) 
				{
					*((char *)&sdhcp_entry) = (char)j;
					apmib_get(MIB_DHCPRSVDIP_TBL, (void *)&sdhcp_entry);
					if(sdhcp_entry.dhcpRsvdIpEntryEnabled==0)
						continue;
					sprintf(line_buffer, "static_lease %02x%02x%02x%02x%02x%02x %s\n", sdhcp_entry.macAddr[0], sdhcp_entry.macAddr[1], sdhcp_entry.macAddr[2],
					sdhcp_entry.macAddr[3], sdhcp_entry.macAddr[4], sdhcp_entry.macAddr[5], inet_ntoa(*((struct in_addr*)sdhcp_entry.ipAddr)));
					write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
				}
			}
		}
		/*end static dhcp entry*/
		
		sprintf(line_buffer,"poolend \n");
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);	
		k=2;		
	}
	return count;
}

void create_dhcpd_default_configfile(char *interface, int mode)
{	
	unsigned int intValue;
	char line_buffer[128]={0};	
	struct in_addr addr;
	int dns_mode=0;
	char tmp_buff[64]={0};
	
	int i, entry_Num;
	DHCPRSVDIP_T entry;
	
	sprintf(line_buffer,"poolname pool1\n");
	write_line_to_file(DHCPD_CONF_FILE, 1, line_buffer);

	sprintf(line_buffer,"poolorder 1\n");
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);		

	sprintf(line_buffer,"interface %s\n", interface);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	
	apmib_get(MIB_DHCP_CLIENT_START,  (void *)&addr);	
	sprintf(line_buffer,"start %s\n", inet_ntoa(addr));
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	apmib_get(MIB_DHCP_CLIENT_END,  (void *)&addr);
	sprintf(line_buffer,"end %s\n", inet_ntoa(addr));
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	apmib_get(MIB_SUBNET_MASK,  (void *)&addr);
	sprintf(line_buffer,"opt subnet %s\n", inet_ntoa(addr));
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	
	apmib_get(MIB_DHCP_LEASE_TIME, (void *)&intValue);
    	if((intValue==0) || (intValue<0) || (intValue>10080))
		intValue = 480; //8 hours
	intValue *= 60;
    	sprintf(line_buffer,"opt lease %ld\n",intValue);
    	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	if(mode==1)
	{//ap
		apmib_get(MIB_DEFAULT_GATEWAY,  (void *)&addr);
		if (addr.s_addr>0)
		{			
			sprintf(line_buffer,"opt router %s\n", inet_ntoa(addr));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		else
		{
			apmib_get(MIB_IP_ADDR, (void *)&addr);			
			sprintf(line_buffer,"opt router %s\n", inet_ntoa(addr));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
	}
	else
	{
		apmib_get(MIB_IP_ADDR, (void *)&addr);			
		sprintf(line_buffer,"opt router %s\n", inet_ntoa(addr));
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	}

	apmib_get( MIB_DNS_MODE, (void *)&dns_mode);
	if(dns_mode==0)
	{		
		sprintf(line_buffer,"opt dns %s\n", inet_ntoa(addr));
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	}
	else
	{
		apmib_get(MIB_DNS1, (void *)&addr);
		if(addr.s_addr>0)
		{
			sprintf(line_buffer,"opt dns %s\n", inet_ntoa(addr));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		apmib_get(MIB_DNS2, (void *)&addr);
		if(addr.s_addr>0)
		{
			sprintf(line_buffer,"opt dns %s\n", inet_ntoa(addr));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
		apmib_get(MIB_DNS3, (void *)&addr);
		if(addr.s_addr>0)
		{
			sprintf(line_buffer,"opt dns %s\n", inet_ntoa(addr));
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
	}
	
	apmib_get( MIB_DOMAIN_NAME, (void *)tmp_buff);
	if(tmp_buff[0])
	{
		sprintf(line_buffer,"opt domain %s\n",tmp_buff);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	}	

	/*static dhcp entry static_lease 000102030405 192.168.1.199*/
	apmib_get(MIB_DHCPRSVDIP_ENABLED, (void *)&intValue);
	if(intValue==1){
		apmib_get(MIB_DHCPRSVDIP_TBL_NUM, (void *)&entry_Num);
		if(entry_Num>0){
			for (i=1; i<=entry_Num; i++) {
				*((char *)&entry) = (char)i;
				apmib_get(MIB_DHCPRSVDIP_TBL, (void *)&entry);
				if(entry.dhcpRsvdIpEntryEnabled==0)
					continue;
				sprintf(line_buffer, "static_lease %02x%02x%02x%02x%02x%02x %s\n", entry.macAddr[0], entry.macAddr[1], entry.macAddr[2],
				entry.macAddr[3], entry.macAddr[4], entry.macAddr[5], inet_ntoa(*((struct in_addr*)entry.ipAddr)));
				write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
			}
		}
	}
	/*end static dhcp entry*/
	
	sprintf(line_buffer,"poolend \n");
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);	
}
#endif

void set_lan_dhcpd(char *interface, int mode)
{
	char tmpBuff1[32]={0}, tmpBuff2[32]={0};
	int intValue=0, dns_mode=0;
	char line_buffer[100]={0};
	char tmp1[64]={0};
	char tmp2[64]={0};
	char *strtmp=NULL, *strtmp1=NULL;
	DHCPRSVDIP_T entry;
	int i, entry_Num=0, ret_val=0;
#ifdef   HOME_GATEWAY
	char tmpBuff3[32]={0};
#endif
#if defined(CONFIG_RTL_ULINKER)
{//// must be gateway mode!
	int opmode;
	int auto_wan;
	apmib_get(MIB_OP_MODE,(void *)&opmode);
	if (opmode==GATEWAY_MODE || opmode==WISP_MODE) {
		system("brctl addif br0 usb0 > /dev/null 2>&1");
	#if 0
		apmib_get(MIB_ULINKER_AUTO,(void *)&auto_wan);
		if (auto_wan == 0)
			system("ifconfig usb0 0.0.0.0 > /dev/null 2>&1");
	#endif
	}
}
#endif

#ifdef _PRMT_X_TELEFONICA_ES_DHCPOPTION_
	ret_val=create_dhcpd_configfile(interface);
	if(ret_val<1)
		create_dhcpd_default_configfile(interface, mode);
#else	
	
	sprintf(line_buffer,"interface %s\n",interface);
	write_line_to_file(DHCPD_CONF_FILE, 1, line_buffer);

	apmib_get(MIB_DHCP_CLIENT_START,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"start %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	apmib_get(MIB_DHCP_CLIENT_END,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"end %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	apmib_get(MIB_SUBNET_MASK,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(line_buffer,"opt subnet %s\n",strtmp);
	write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	apmib_get(MIB_DHCP_LEASE_TIME, (void *)&intValue);
    if( (intValue==0) || (intValue<0) || (intValue>10080))
    {
		intValue = 480; //8 hours
		if(!apmib_set(MIB_DHCP_LEASE_TIME, (void *)&intValue))
		{
			printf("set MIB_DHCP_LEASE_TIME error\n");
		}

		apmib_update(CURRENT_SETTING);
    }
	intValue *= 60;
    sprintf(line_buffer,"opt lease %ld\n",intValue);
    write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);

	if(mode==1){//ap
		apmib_get( MIB_DEFAULT_GATEWAY,  (void *)tmp2);
		if (memcmp(tmp2, "\x0\x0\x0\x0", 4)){
			strtmp= inet_ntoa(*((struct in_addr *)tmp2));
			sprintf(line_buffer,"opt router %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}else
		{
			apmib_get(MIB_IP_ADDR,  (void *)tmp1);
			strtmp= inet_ntoa(*((struct in_addr *)tmp1));
			sprintf(line_buffer,"opt router %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}


	}else{
		apmib_get(MIB_IP_ADDR,  (void *)tmp1);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(line_buffer,"opt router %s\n",strtmp);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
#ifdef   HOME_GATEWAY
		apmib_get( MIB_DNS_MODE, (void *)&dns_mode);
		if(dns_mode==0){
			sprintf(line_buffer,"opt dns %s\n",strtmp); /*now strtmp is ip address value */
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
		}
#endif
	}
	if((mode==1) 
#if 1
	||(mode==2 && dns_mode==1)
#endif
	){
#if defined(HOME_GATEWAY) && !(defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT) || defined(CONFIG_RTL_ULINKER))
		apmib_get( MIB_DNS1,  (void *)tmpBuff1);
		apmib_get( MIB_DNS2,  (void *)tmpBuff2);
		apmib_get( MIB_DNS3,  (void *)tmpBuff3);

		if (memcmp(tmpBuff1, "\x0\x0\x0\x0", 4)){
			strtmp= inet_ntoa(*((struct in_addr *)tmpBuff1));
			sprintf(line_buffer,"opt dns %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
			intValue++;
		}
		if (memcmp(tmpBuff2, "\x0\x0\x0\x0", 4)){
			strtmp= inet_ntoa(*((struct in_addr *)tmpBuff2));
			sprintf(line_buffer,"opt dns %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
			intValue++;
		}
		if (memcmp(tmpBuff3, "\x0\x0\x0\x0", 4)){
			strtmp= inet_ntoa(*((struct in_addr *)tmpBuff3));
			sprintf(line_buffer,"opt dns %s\n",strtmp);
			write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
			intValue++;
		}
#endif

#if defined(CONFIG_DOMAIN_NAME_QUERY_SUPPORT) || defined(CONFIG_RTL_ULINKER)
		apmib_get(MIB_IP_ADDR,  (void *)tmp1);
		strtmp= inet_ntoa(*((struct in_addr *)tmp1));
		sprintf(line_buffer,"opt dns %s\n",strtmp); /*now strtmp is ip address value */
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
#endif //#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT

		if(intValue==0){ /*no dns option for dhcp server, use default gatewayfor dns opt*/

			if(mode==1){
				apmib_get( MIB_DEFAULT_GATEWAY,  (void *)tmp2);
				if (memcmp(tmp2, "\x0\x0\x0\x0", 4)){
					strtmp= inet_ntoa(*((struct in_addr *)tmp2));
					sprintf(line_buffer,"opt dns %s\n",strtmp);
					write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
				}
			}else {
				apmib_get( MIB_IP_ADDR,  (void *)tmp2);
				if (memcmp(tmp2, "\x0\x0\x0\x0", 4)){
					strtmp= inet_ntoa(*((struct in_addr *)tmp2));
					sprintf(line_buffer,"opt dns %s\n",strtmp);
					write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
				}
			}
		}
	}
	memset(tmp1, 0x00, 64);
	apmib_get( MIB_DOMAIN_NAME, (void *)&tmp1);
	if(tmp1[0]){
		sprintf(line_buffer,"opt domain %s\n",tmp1);
		write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
	}
/*static dhcp entry static_lease 000102030405 192.168.1.199*/
	intValue=0;
	apmib_get(MIB_DHCPRSVDIP_ENABLED, (void *)&intValue);
	if(intValue==1){
		apmib_get(MIB_DHCPRSVDIP_TBL_NUM, (void *)&entry_Num);
		if(entry_Num>0){
			for (i=1; i<=entry_Num; i++) {
				*((char *)&entry) = (char)i;
				apmib_get(MIB_DHCPRSVDIP_TBL, (void *)&entry);
				sprintf(line_buffer, "static_lease %02x%02x%02x%02x%02x%02x %s\n", entry.macAddr[0], entry.macAddr[1], entry.macAddr[2],
				entry.macAddr[3], entry.macAddr[4], entry.macAddr[5], inet_ntoa(*((struct in_addr*)entry.ipAddr)));
				write_line_to_file(DHCPD_CONF_FILE, 2, line_buffer);
			}
		}
	}
#endif

	/* may not need to set ip again*/
	apmib_get(MIB_IP_ADDR,  (void *)tmp1);
	strtmp= inet_ntoa(*((struct in_addr *)tmp1));
	sprintf(tmpBuff1, "%s", strtmp);
	apmib_get(MIB_SUBNET_MASK,  (void *)tmp2);
	strtmp1= inet_ntoa(*((struct in_addr *)tmp2));
	sprintf(tmpBuff2, "%s", strtmp1);
	RunSystemCmd(NULL_FILE, "ifconfig", interface, tmpBuff1, "netmask", tmpBuff2,  NULL_STR);
	/*start dhcp server*/
	char tmpBuff4[100];
	sprintf(tmpBuff4,"udhcpd %s\n",DHCPD_CONF_FILE);
	system(tmpBuff4);
	//RunSystemCmd(stdout, "udhcpd", DHCPD_CONF_FILE, NULL_STR);
	
#if defined(CONFIG_APP_SIMPLE_CONFIG)
	system("echo 1 > /var/sc_ip_status");
#endif

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

	//RunSystemCmd(NULL_FILE, "udhcpc", "-i", iface, "-p", pid_file, "-s", script_file,  "-n", "-x", NULL_STR);
	//sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s -n &", iface, pid_file, script_file);
	sprintf(cmdBuff, "udhcpc -i %s -p %s -s %s &", iface, pid_file, script_file);
	system(cmdBuff);

#if defined(CONFIG_APP_SIMPLE_CONFIG)
	system("echo 0 > /var/sc_ip_status");
#endif
}

int setbridge(char *argv)
{
	FILE *fp=NULL;
	int j;
	int opmode=-1;
	char bridge_iface[300], tmpBuff[200], cmdBuffer[100];
#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	char bridge_iface2[300];
#endif
	char iface_name[16], tmp_iface[16];
	char *token=NULL, *savestr1=NULL;
	int intVal=0;
	int iface_index=0;
	int vlan_enabled=0, wlan_disabled=0;
	int wlan_mode=0, wisp_wan_id=0;
	int iswlan_va=0, wlan_wds_enabled=0;
	int wlan_wds_num=0, wlan0_mesh_enabled=0, wlan1_mesh_enabled=0;
	int br_stp_enabled=0, dhcp_mode=0;
	char lanIp[30], lanMask[30], lanGateway[30];
	int lan_addr,lan_mask;


#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
	int hw_bridge_num = 0;
		struct hw_vlan_setting{
			int vlan_enabled;
			int vlan_bridge;
			int vlan_tag;
			int vlan_vid;
		} rtl_hw_vlan_info[5];
		memset(rtl_hw_vlan_info, 0, 5*sizeof(struct hw_vlan_setting));
		//int rtl_hw_vlan_enable = 0;
#endif

#if defined(VLAN_CONFIG_SUPPORTED)
	VLAN_CONFIG_T vlan_entry;
	int entry_num=0;
	int i;
	int VlanisLan=0;
#endif
    int multiple_repeater=0;    /*for support multiple repeater*/ 
#ifdef FOR_DUAL_BAND
    int multiple_repeater2=0;
#endif
#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	int vlan_wan_enable = 0;
	int vlan_wan_tag = 0;
	int vlan_wan_bridge_enable = 0; 
	int vlan_wan_bridge_tag = 0; 
	int vlan_wan_bridge_port = 0;
	int vlan_wan_bridge_multicast_enable = 0;
	int vlan_wan_bridge_multicast_tag = 0;
	int vlan_wan_host_enable = 0;
	int vlan_wan_host_tag = 0;
	int vlan_wan_host_pri = 0;
	int vlan_wan_wifi_root_enable = 0;
	int vlan_wan_wifi_root_tag = 0;
	int vlan_wan_wifi_root_pri = 0;
	int vlan_wan_wifi_vap0_enable = 0;
	int vlan_wan_wifi_vap0_tag = 0;
	int vlan_wan_wifi_vap0_pri = 0;
	int vlan_wan_wifi_vap1_enable = 0; 
	int vlan_wan_wifi_vap1_tag = 0;
	int vlan_wan_wifi_vap1_pri = 0;
	int vlan_wan_wifi_vap2_enable = 0;
	int vlan_wan_wifi_vap2_tag = 0;
	int vlan_wan_wifi_vap2_pri = 0;
	int vlan_wan_wifi_vap3_enable = 0;
	int vlan_wan_wifi_vap3_tag = 0;
	int vlan_wan_wifi_vap3_pri = 0;

#endif
#if defined(CONFIG_SMART_REPEATER)
	char vlan_netif[16]={0};
#endif

	printf("Init bridge interface...\n");
    apmib_get(MIB_REPEATER_ENABLED1,(void *)&multiple_repeater);/*for support multiple repeater*/ 
#ifdef FOR_DUAL_BAND
    apmib_get(MIB_REPEATER_ENABLED2,(void *)&multiple_repeater2);/*for support multiple repeater*/ 
#endif    

	apmib_get(MIB_OP_MODE,(void *)&opmode);
	apmib_get(MIB_WISP_WAN_ID,(void *)&wisp_wan_id);
	apmib_get(MIB_DHCP,(void *)&dhcp_mode);
//delete interface of bridge
	if(isFileExist(BR_IFACE_FILE)){
		memset(bridge_iface,0x00,sizeof(bridge_iface));
		fp=fopen(BR_IFACE_FILE, "r");
		if(fp!=NULL){
			fgets(bridge_iface,sizeof(bridge_iface),fp);
			for(iface_index=0;iface_index<strlen(bridge_iface);iface_index++){
				if(bridge_iface[iface_index]==0x0a)
					bridge_iface[iface_index]=0;
			}
			//printf("briface=%s\n",bridge_iface);
			bridge_iface[strlen(bridge_iface)]=0;
			token = strtok_r(bridge_iface,":", &savestr1);

			do{
				//printf("token=%s\n",token);
				if (token == NULL){
					break;
				}else{
                    /*for support multiple repeater*/ 
                    if(multiple_repeater==2 ){
    				    SDEBUG("\n");                        
                        if( !strcmp(token,"wlan0-va0") ||!strcmp(token,"wlan0-va2") ){
                            //RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);
                            RunSystemCmd(NULL_FILE, "brctl", "delif", "br1" ,token, NULL_STR);

                        }else if( !strcmp(token,"wlan0") ||!strcmp(token,"wlan0-va1") ){

                            if( !strcmp(token,"wlan0"))
                                RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);

                            RunSystemCmd(NULL_FILE, "brctl", "delif", "br0" ,token, NULL_STR);

                        }
                    }
                    #ifdef FOR_DUAL_BAND
                    if(multiple_repeater2==2 ){
                        SDEBUG("\n");                        
                        if( !strcmp(token,"wlan1-va0") ||!strcmp(token,"wlan1-va2") ){
                            //RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);
                            RunSystemCmd(NULL_FILE, "brctl", "delif", "br1" ,token, NULL_STR);

                        }else if( !strcmp(token,"wlan1") ||!strcmp(token,"wlan1-va1") ){

                            if( !strcmp(token,"wlan1"))
                                RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);

                            RunSystemCmd(NULL_FILE, "brctl", "delif", "br0" ,token, NULL_STR);

                        }
                    }
                    #endif                        
                    else            /*for support multiple repeater*/        
                    {
						RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);
						if( memcmp(token, "br0", 3) != 0 )
							RunSystemCmd(NULL_FILE, "brctl", "delif", "br0" ,token, NULL_STR);
                    }

				}
				token = strtok_r(NULL, ":", &savestr1);
			}while(token !=NULL);
			fclose(fp);
		}
	}
	else{
		if(setInAddr( "br0", 0,0,0, IFACE_FLAG_T)==0){
			token=NULL;
			savestr1=NULL;
			sprintf(bridge_iface, "%s", argv);
			token = strtok_r(bridge_iface," ", &savestr1);
			do{
				if (token == NULL){/*chec if the first arg is NULL*/
					break;
				}else{
					RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);
					if( memcmp(token, "br0", 3) != 0 ) {
						RunSystemCmd(NULL_FILE, "brctl", "delif", "br0" ,token, NULL_STR);
					}
				}
				token = strtok_r(NULL, " ", &savestr1);
			}while(token !=NULL);
		}
	}

#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if(isFileExist(BR_IFACE_FILE2)){
		memset(bridge_iface,0x00,sizeof(bridge_iface));
		fp=fopen(BR_IFACE_FILE2, "r");
		if(fp!=NULL){
			fgets(bridge_iface,sizeof(bridge_iface),fp);
			for(iface_index=0;iface_index<strlen(bridge_iface);iface_index++){
				if(bridge_iface[iface_index]==0x0a)
					bridge_iface[iface_index]=0;
			}
			bridge_iface[strlen(bridge_iface)]=0;
			token = strtok_r(bridge_iface,":", &savestr1);
			
			do{
				//printf("token=%s\n",token);
				if (token == NULL){
					break;
				}else{
					RunSystemCmd(NULL_FILE, "ifconfig", token, "down", NULL_STR);
					if( memcmp(token, "br1", 3) != 0 ) 
						RunSystemCmd(NULL_FILE, "brctl", "delif", "br1" ,token, NULL_STR);
				}	
				token = strtok_r(NULL, ":", &savestr1);
			}while(token !=NULL);
			fclose(fp);
		}

	}
#endif
#if defined(VLAN_CONFIG_SUPPORTED) || defined(CONFIG_RTL_MULTI_LAN_DEV)
	RunSystemCmd(NULL_FILE, "ifconfig", "eth2", "down", NULL_STR);
	RunSystemCmd(NULL_FILE, "ifconfig", "eth3", "down", NULL_STR);
	RunSystemCmd(NULL_FILE, "ifconfig", "eth4", "down", NULL_STR);

	RunSystemCmd(NULL_FILE, "brctl", "delif", "br0", "eth2", NULL_STR);
	RunSystemCmd(NULL_FILE, "brctl", "delif", "br0", "eth3", NULL_STR);
	RunSystemCmd(NULL_FILE, "brctl", "delif", "br0", "eth4", NULL_STR);
#endif

	if(isFileExist(MESH_PATHSEL)){
        SetWlan_idx("wlan0");
        apmib_get(MIB_WLAN_MODE, (void *)&wlan_mode);
        apmib_get(MIB_WLAN_MESH_ENABLE,(void *)&wlan0_mesh_enabled);    
        if(wlan_mode != AP_MESH_MODE && wlan_mode != MESH_MODE) {
            wlan0_mesh_enabled = 0;
        }
    
#if defined(FOR_DUAL_BAND)
        SetWlan_idx("wlan1");
        apmib_get(MIB_WLAN_MODE, (void *)&wlan_mode);
        apmib_get(MIB_WLAN_MESH_ENABLE,(void *)&wlan1_mesh_enabled);
        if(wlan_mode != AP_MESH_MODE && wlan_mode != MESH_MODE){
            wlan1_mesh_enabled = 0;
        }
#endif
        RunSystemCmd(NULL_FILE,"ifconfig", "wlan-msh", "down", NULL_STR);
	    RunSystemCmd(NULL_FILE,"brctl", "delif", "br0" ,"wlan-msh", NULL_STR);
        
	}
	if(setInAddr( "br0", 0,0,0, IFACE_FLAG_T)==0){
		RunSystemCmd(NULL_FILE,"brctl", "delif", "br0" ,"eth1", NULL_STR);
		RunSystemCmd(NULL_FILE,"brctl", "delif", "br0" ,"wlan0", NULL_STR);
	}
	if(isFileExist(BR_INIT_FILE)==0){//bridge init file is not exist
		RunSystemCmd(NULL_FILE,"brctl", "delbr", "br0", NULL_STR);
		RunSystemCmd(NULL_FILE,"brctl", "addbr", "br0", NULL_STR);
	}

    /*for support multiple repeater*/ 
    if(multiple_repeater==2){
		RunSystemCmd(NULL_FILE,"brctl", "delbr", "br1", NULL_STR);
		RunSystemCmd(NULL_FILE,"brctl", "addbr", "br1", NULL_STR);
    }
    /*for support multiple repeater*/ 
    #ifdef FOR_DUAL_BAND    
    if(multiple_repeater2==2){
		RunSystemCmd(NULL_FILE,"brctl", "delbr", "br1", NULL_STR);
		RunSystemCmd(NULL_FILE,"brctl", "addbr", "br1", NULL_STR);
    }    
    #endif
	apmib_get(MIB_STP_ENABLED,(void *)&br_stp_enabled);
	if(br_stp_enabled==1){
		RunSystemCmd(NULL_FILE,"brctl", "setfd", "br0", "4", NULL_STR);
		RunSystemCmd(NULL_FILE,"brctl", "stp", "br0", "1", NULL_STR);
	}else{
		RunSystemCmd(NULL_FILE,"brctl", "setfd", "br0", "0", NULL_STR);
		RunSystemCmd(NULL_FILE,"brctl", "stp", "br0", "0", NULL_STR);
	}


#if defined(VLAN_CONFIG_SUPPORTED)
		apmib_get(MIB_VLANCONFIG_ENABLED,(void *)&vlan_enabled);

	#if !defined(CONFIG_RTL_HW_VLAN_SUPPORT)
		if(vlan_enabled ==1 )
			RunSystemCmd("/proc/rtk_vlan_support", "echo", "1", NULL_STR);
		else
	#endif
			RunSystemCmd("/proc/rtk_vlan_support", "echo", "0", NULL_STR);

		//apmib_get(MIB_VLANCONFIG_NUM,(void *)&entry_num);
		apmib_get(MIB_VLANCONFIG_TBL_NUM,(void *)&entry_num);
		//printf("*********************vlan tbl=%d,vlan_enable(%d)\n", entry_num,vlan_enabled);
		for (i=1; i<=entry_num; i++) {
			*((char *)&vlan_entry) = (char)i;
			apmib_get(MIB_VLANCONFIG_TBL, (void *)&vlan_entry);
			sprintf(tmpBuff, "/proc/%s/mib_vlan", vlan_entry.netIface);
			if(isFileExist(tmpBuff)){
#ifdef RTK_USB3G_PORT5_LAN
				DHCP_T wan_dhcp = -1;
        apmib_get( MIB_DHCP, (void *)&wan_dhcp);
#endif

				if(strncmp(vlan_entry.netIface,"eth1",strlen("eth1")) == 0){
#ifdef RTK_USB3G_PORT5_LAN
					if(opmode == WISP_MODE || opmode == BRIDGE_MODE || wan_dhcp == USB3G)
#else
					if(opmode == WISP_MODE || opmode == BRIDGE_MODE)
#endif
						VlanisLan=1;
					else
						VlanisLan=0;

#if defined(CONFIG_RTK_VLAN_NEW_FEATURE)
					sprintf(cmdBuffer,"echo \"1 %d %d %d %d %d %d 1\" > /proc/eth7/mib_vlan", VlanisLan,vlan_entry.enabled, vlan_entry.tagged, vlan_entry.vlanId, vlan_entry.priority, vlan_entry.cfi);
					system(cmdBuffer);
					
#if !defined(CONFIG_RTL_HW_VLAN_SUPPORT)
//for pass thru
					if(vlan_enabled==1){			
						if(opmode == GATEWAY_MODE){
							sprintf(cmdBuffer,"echo \"1 %d %d %d %d %d %d %d\" > /proc/peth0/mib_vlan", VlanisLan,vlan_entry.enabled, vlan_entry.tagged, vlan_entry.vlanId, vlan_entry.priority, vlan_entry.cfi,vlan_entry.forwarding_rule);
							system(cmdBuffer);
						}
						else if(opmode == WISP_MODE)
						{
							//to-do :for wisp mode bridge vlan
						}
					}
					else
#endif		
		 			{			
						if(opmode == GATEWAY_MODE){
							sprintf(cmdBuffer,"echo \"0 %d %d %d %d %d %d %d\" > /proc/peth0/mib_vlan", VlanisLan,vlan_entry.enabled, vlan_entry.tagged, vlan_entry.vlanId, vlan_entry.priority, vlan_entry.cfi,vlan_entry.forwarding_rule);
							system(cmdBuffer);
						}
						else if(opmode == WISP_MODE)
						{
							//to-do :for wisp mode bridge vlan
						}
					}
#endif
			

				}
				else
				{
					sprintf(vlan_netif,"wlan%d",wisp_wan_id);
#if defined(CONFIG_SMART_REPEATER)
					getWispRptIfaceName(vlan_netif,wisp_wan_id);
#endif					
					if(strncmp(vlan_netif,vlan_entry.netIface, strlen(vlan_entry.netIface)) == 0)
					{
						if(opmode == WISP_MODE && (wisp_wan_id == 0 || wisp_wan_id==1))
							VlanisLan=0;
						else
							VlanisLan=1;
					}else
					{
						VlanisLan=1;
					}
				}
		#if defined(CONFIG_RTK_VLAN_NEW_FEATURE)
			#if !defined(CONFIG_RTL_HW_VLAN_SUPPORT)
				if(vlan_enabled==1){				//global_vlan, is_lan,vlan, tag, id, pri, cfi, forwarding_rule
					sprintf(cmdBuffer,"echo \"1 %d %d %d %d %d %d %d\" > %s", VlanisLan,vlan_entry.enabled, vlan_entry.tagged, vlan_entry.vlanId, vlan_entry.priority, vlan_entry.cfi, vlan_entry.forwarding_rule ,tmpBuff);
					system(cmdBuffer);
				}
				else
			#endif
				{
					sprintf(cmdBuffer,"echo \"0 %d %d %d %d %d %d %d\" > %s", VlanisLan,vlan_entry.enabled, vlan_entry.tagged, vlan_entry.vlanId, vlan_entry.priority, vlan_entry.cfi, vlan_entry.forwarding_rule, tmpBuff);
					system(cmdBuffer);
				}
		#else
			#if !defined(CONFIG_RTL_HW_VLAN_SUPPORT)
				if(vlan_enabled==1){				//global_vlan, is_lan,vlan, tag, id, pri, cfi
					sprintf(cmdBuffer,"echo \"1 %d %d %d %d %d %d\" > %s", VlanisLan,vlan_entry.enabled, vlan_entry.tagged, vlan_entry.vlanId, vlan_entry.priority, vlan_entry.cfi, tmpBuff);
					system(cmdBuffer);
				}
				else
			#endif
				{
					sprintf(cmdBuffer,"echo \"0 %d %d %d %d %d %d\" > %s", VlanisLan,vlan_entry.enabled, vlan_entry.tagged, vlan_entry.vlanId, vlan_entry.priority, vlan_entry.cfi, tmpBuff);
					system(cmdBuffer);
				}
		#endif
			}

#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
				if(i < 5){	   /*store lan port vlan info*/
					if(i != 1){   /*port1,2,3<-->eth2,3,4 on webpage*/
						rtl_hw_vlan_info[i].vlan_enabled = vlan_entry.enabled;
						rtl_hw_vlan_info[i].vlan_bridge = vlan_entry.forwarding_rule;
						rtl_hw_vlan_info[i].vlan_tag	  = vlan_entry.tagged;
						rtl_hw_vlan_info[i].vlan_vid	  = vlan_entry.vlanId;
					}else{	/*port0<-->eth0 on webpage*/
						rtl_hw_vlan_info[0].vlan_enabled = vlan_entry.enabled;
						rtl_hw_vlan_info[0].vlan_bridge = vlan_entry.forwarding_rule;
						rtl_hw_vlan_info[0].vlan_tag	  = vlan_entry.tagged;
						rtl_hw_vlan_info[0].vlan_vid	  = vlan_entry.vlanId;
					}
				}else if(strncmp(vlan_entry.netIface, "eth1", strlen("eth1")) == 0){ /*wan port*/
					rtl_hw_vlan_info[1].vlan_enabled = vlan_entry.enabled;
					rtl_hw_vlan_info[1].vlan_vid = vlan_entry.vlanId;
					rtl_hw_vlan_info[1].vlan_tag = vlan_entry.tagged;
				}
#endif

		}
#endif

#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
	for(i=0; i<5; i++)
	{
		if((rtl_hw_vlan_info[i].vlan_enabled == 1)&&(rtl_hw_vlan_info[i].vlan_bridge == 1))
			hw_bridge_num ++;
	}
	//apmib_get(MIB_HW_VLANCONFIG_ENABLED, (void *)&rtl_hw_vlan_enable);
	sprintf(tmpBuff, "/proc/rtl_hw_vlan_support");
	sprintf(cmdBuffer,"echo \"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\" > %s", \
		vlan_enabled, rtl_hw_vlan_info[1].vlan_enabled, rtl_hw_vlan_info[1].vlan_tag, rtl_hw_vlan_info[1].vlan_vid,\
		rtl_hw_vlan_info[0].vlan_enabled, rtl_hw_vlan_info[0].vlan_bridge, rtl_hw_vlan_info[0].vlan_tag, rtl_hw_vlan_info[0].vlan_vid,\
		rtl_hw_vlan_info[2].vlan_enabled, rtl_hw_vlan_info[2].vlan_bridge, rtl_hw_vlan_info[2].vlan_tag, rtl_hw_vlan_info[2].vlan_vid,\
		rtl_hw_vlan_info[3].vlan_enabled, rtl_hw_vlan_info[3].vlan_bridge, rtl_hw_vlan_info[3].vlan_tag, rtl_hw_vlan_info[3].vlan_vid,\
		rtl_hw_vlan_info[4].vlan_enabled, rtl_hw_vlan_info[4].vlan_bridge, rtl_hw_vlan_info[4].vlan_tag, rtl_hw_vlan_info[4].vlan_vid,\
		tmpBuff);
	system(cmdBuffer);
#endif


#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT	
	if(opmode == GATEWAY_MODE)
	{
		apmib_get( MIB_VLAN_WAN_ENALE, (void *)&vlan_wan_enable);
		apmib_get( MIB_VLAN_WAN_TAG, (void *)&vlan_wan_tag);
		apmib_get( MIB_VLAN_WAN_BRIDGE_ENABLE, (void *)&vlan_wan_bridge_enable);
		apmib_get( MIB_VLAN_WAN_BRIDGE_TAG, (void *)&vlan_wan_bridge_tag);
		apmib_get( MIB_VLAN_WAN_BRIDGE_PORT, (void *)&vlan_wan_bridge_port);
		apmib_get( MIB_VLAN_WAN_BRIDGE_MULTICAST_ENABLE, (void *)&vlan_wan_bridge_multicast_enable);
		apmib_get( MIB_VLAN_WAN_BRIDGE_MULTICAST_TAG, (void *)&vlan_wan_bridge_multicast_tag);
		apmib_get( MIB_VLAN_WAN_HOST_ENABLE, (void *)&vlan_wan_host_enable);
		apmib_get( MIB_VLAN_WAN_HOST_TAG, (void *)&vlan_wan_host_tag);
		apmib_get( MIB_VLAN_WAN_HOST_PRI, (void *)&vlan_wan_host_pri);
		apmib_get( MIB_VLAN_WAN_WIFI_ROOT_ENABLE, (void *)&vlan_wan_wifi_root_enable);
		apmib_get( MIB_VLAN_WAN_WIFI_ROOT_TAG, (void *)&vlan_wan_wifi_root_tag);
		apmib_get( MIB_VLAN_WAN_WIFI_ROOT_PRI, (void *)&vlan_wan_wifi_root_pri);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP0_ENABLE, (void *)&vlan_wan_wifi_vap0_enable);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP0_TAG, (void *)&vlan_wan_wifi_vap0_tag);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP0_PRI, (void *)&vlan_wan_wifi_vap0_pri);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP1_ENABLE, (void *)&vlan_wan_wifi_vap1_enable);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP1_TAG, (void *)&vlan_wan_wifi_vap1_tag);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP1_PRI, (void *)&vlan_wan_wifi_vap1_pri);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP2_ENABLE, (void *)&vlan_wan_wifi_vap2_enable);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP2_TAG, (void *)&vlan_wan_wifi_vap2_tag);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP2_PRI, (void *)&vlan_wan_wifi_vap2_pri);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP3_ENABLE, (void *)&vlan_wan_wifi_vap3_enable);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP3_TAG, (void *)&vlan_wan_wifi_vap3_tag);
		apmib_get( MIB_VLAN_WAN_WIFI_VAP3_PRI, (void *)&vlan_wan_wifi_vap3_pri);	

    sprintf(cmdBuffer,"echo \"%d %d %d %d %d %d %d\" > /proc/rtk_vlan_wan_tag", vlan_wan_enable, vlan_wan_tag, 
    	vlan_wan_bridge_enable, vlan_wan_bridge_tag, vlan_wan_bridge_port,vlan_wan_bridge_multicast_enable,vlan_wan_bridge_multicast_tag);

		system(cmdBuffer);

	  sprintf(cmdBuffer,"echo \"%d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d %d\" > /proc/rtk_vlan_wan_tag_cpu", 
    	vlan_wan_host_enable,      vlan_wan_host_tag,      vlan_wan_host_pri, 
      vlan_wan_wifi_root_enable, vlan_wan_wifi_root_tag,      vlan_wan_wifi_root_pri, 
      vlan_wan_wifi_vap0_enable, vlan_wan_wifi_vap0_tag, vlan_wan_wifi_vap0_pri,
      vlan_wan_wifi_vap1_enable, vlan_wan_wifi_vap1_tag, vlan_wan_wifi_vap1_pri,
      vlan_wan_wifi_vap2_enable, vlan_wan_wifi_vap2_tag, vlan_wan_wifi_vap2_pri,
      vlan_wan_wifi_vap3_enable, vlan_wan_wifi_vap3_tag, vlan_wan_wifi_vap3_pri);
		system(cmdBuffer);
	}
#endif
	if(isFileExist(BR_IFACE_FILE)){
		unlink(BR_IFACE_FILE);
	}
	memset(bridge_iface,0x00,sizeof(bridge_iface));
	token=NULL;
	savestr1=NULL;
	sprintf(tmpBuff, "%s", argv);
	
//printf("\r\n tmpBuff=[%s],__[%s-%u]\r\n",tmpBuff,__FILE__,__LINE__);
	
	token = strtok_r(tmpBuff," ", &savestr1);
	do{
		if (token == NULL){/*check if the first arg is NULL*/
			break;
		}else{
			sprintf(iface_name,"%s", token);
			
//printf("\r\n iface_name=[%s],__[%s-%u]\r\n",iface_name,__FILE__,__LINE__);
			
			if(strncmp(iface_name, "eth", 3)==0){//ether iface
				intVal=1;
//				if(isFileExist(ETH_VLAN_SWITCH)){
#if defined(CONFIG_RTL_MULTI_LAN_DEV)
#else
				if(
#if !defined(CONFIG_RTL_HW_VLAN_SUPPORT)
					(vlan_enabled==0)&&
#endif
					(iface_name[3]=='2' || iface_name[3]=='3' || iface_name[3]=='4'
#if defined(CONFIG_RTK_VLAN_NEW_FEATURE)
					|| iface_name[3]=='7'
#endif
					)
					)
					{
						intVal=0;
					}
#endif
//				}

#if defined(CONFIG_RTL_HW_VLAN_SUPPORT)
					switch(hw_bridge_num)
					{
						case 1:
							if(iface_name[3]=='2')
								intVal = 1;
						break;
						case 2:
							if(iface_name[3]=='2' || iface_name[3]=='3')
								intVal = 1;
						break;
						case 3:
							if(iface_name[3]=='2' || iface_name[3]=='3' || iface_name[3]=='4')
								intVal = 1;
						break;
						default:
						break;
					}
#endif

					if(intVal==1){
//printf("add iface to br %s\n", iface_name);
						RunSystemCmd(NULL_FILE, "brctl", "addif", "br0" ,iface_name, NULL_STR);
						RunSystemCmd(NULL_FILE, "ifconfig", iface_name, "0.0.0.0", NULL_STR);
						if(bridge_iface[0]){
							strcat(bridge_iface, iface_name);
							strcat(bridge_iface, ":");
						}else{
							sprintf(bridge_iface, "%s", iface_name);
							strcat(bridge_iface, ":");
						}
					}
			}
		}
		token = strtok_r(NULL, " ", &savestr1);
	}while(token !=NULL);

#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
	if(isFileExist(BR_IFACE_FILE2)){
		unlink(BR_IFACE_FILE2);
	}
	memset(bridge_iface2,0x00,sizeof(bridge_iface2));
  
  if(opmode == GATEWAY_MODE && vlan_wan_bridge_enable)
	{
		RunSystemCmd(NULL_FILE, "brctl", "addbr", "br1", NULL_STR);
		RunSystemCmd(NULL_FILE, "brctl", "addif", "br1" ,"eth2", NULL_STR);		
		RunSystemCmd(NULL_FILE, "ifconfig", "eth2", "0.0.0.0", NULL_STR); 
		sprintf(bridge_iface2, "%s", "eth2");
		strcat(bridge_iface2, ":");
	}
	else
	{
		RunSystemCmd(NULL_FILE, "ifconfig", "br1", "down", NULL_STR); 
		RunSystemCmd(NULL_FILE, "brctl", "delbr", "br1", NULL_STR);
	}
#endif	

	token=NULL;
	savestr1=NULL;
	sprintf(tmpBuff, "%s", argv);
	
//printf("\r\n tmpBuff=[%s],__[%s-%u]\r\n",tmpBuff,__FILE__,__LINE__);
	
	token = strtok_r(tmpBuff," ", &savestr1);
	do{
		if(token == NULL){/*check if the first arg is NULL*/
			break;
		}else
		{
	    sprintf(iface_name,"%s", token);
	    
//printf("\r\n iface_name=[%s],__[%s-%u]\r\n",iface_name,__FILE__,__LINE__);

	    
			if(strncmp(iface_name, "wlan", 4)==0){//wlan iface
				if (strlen(iface_name) >= 9 && iface_name[5] == '-' &&
						iface_name[6] == 'v' && iface_name[7] == 'a')
				{
					char wlanRootName[16];
					memset(wlanRootName, 0x00, sizeof(wlanRootName));
					strncpy(wlanRootName,iface_name, 5);

					if(SetWlan_idx(wlanRootName)){
						apmib_get( MIB_WLAN_WLAN_DISABLED, (void *)&intVal);//get root if enable/disable
						apmib_get( MIB_WLAN_MODE, (void *)&wlan_mode); //get root if mode
						if(intVal==0)
						{
							if(wlan_mode != AP_MODE && wlan_mode != AP_WDS_MODE)
								wlan_disabled=1;//root if is disabled
							else
							{
								if(SetWlan_idx( iface_name))
								{
									apmib_get( MIB_WLAN_WLAN_DISABLED, (void *)&intVal);//get va if enable/disable
									if(intVal==0)
#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
									{
										wlan_disabled=0;
										if(opmode == GATEWAY_MODE && vlan_wan_bridge_enable)
										{
											wlan_disabled=0;
									
											if(vwlan_idx)//vap
											{
                      	if(vlan_wan_bridge_port&(1<<(6+vwlan_idx)))
													wlan_disabled=2;
											}
										}
									}
#else
										wlan_disabled=0;
#endif
									else
										wlan_disabled=1;
								}else
										wlan_disabled=1;
							}
						}else
							wlan_disabled=1;
					}else
						wlan_disabled=1;//root if is disabled
				} 
                else if(strstr(iface_name, "msh")) { /*search for wlan-msh, wlan0-msh0 or wlan1-msh0*/
                    //for mesh interfaces
                    if(wlan0_mesh_enabled || wlan1_mesh_enabled)                    
                    {
                        wlan_disabled=0;
                    }
                    else {
                        wlan_disabled=1;
                    }

				} else {
					if(SetWlan_idx( iface_name)){
						apmib_get( MIB_WLAN_WLAN_DISABLED, (void *)&intVal);
						wlan_disabled=intVal;
#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
            if(opmode == GATEWAY_MODE && vlan_wan_bridge_enable)
						{
							if(vlan_wan_bridge_port&(1<<(6+vwlan_idx)))
								wlan_disabled=2;
						}
#endif
					}else
						wlan_disabled=1;
				}
				if(wlan_disabled==0){ //wlan if is enabled					
					sprintf(wlan_wan_iface,"wlan%d", wisp_wan_id);
#if defined(CONFIG_SMART_REPEATER)
					getWispRptIfaceName(wlan_wan_iface,wisp_wan_id);
					//strcat(wlan_wan_iface,"-vxd");
#endif					
					if(strlen(iface_name) >= 9 && iface_name[5] == '-' && iface_name[6] == 'v' && iface_name[7] == 'a')
						iswlan_va=1;

					if((iswlan_va==1) || (opmode != WISP_MODE) || (strcmp(wlan_wan_iface, iface_name) != 0) ){//do not add wlan wan  iface to br0
                        if(multiple_repeater==2 && (!strcmp(iface_name,"wlan0-va0") || !strcmp(iface_name,"wlan0-va2"))){
                            SDEBUG("    add if to br1 \n\n");                            
                            RunSystemCmd(NULL_FILE, "brctl", "addif", "br1" ,iface_name, NULL_STR);     
                        }
                        #ifdef FOR_DUAL_BAND                            
                        if(multiple_repeater2==2 && (!strcmp(iface_name,"wlan1-va0") || !strcmp(iface_name,"wlan1-va2"))){
                            SDEBUG("    add if to br1 \n\n");                            
                            RunSystemCmd(NULL_FILE, "brctl", "addif", "br1" ,iface_name, NULL_STR);     
                        }                        
                        #endif
                        else      
                        {
	    					RunSystemCmd(NULL_FILE, "brctl", "addif", "br0" ,iface_name, NULL_STR);		
                        }
						RunSystemCmd(NULL_FILE, "ifconfig", iface_name, "0.0.0.0", NULL_STR);
							if(bridge_iface[0]){
								strcat(bridge_iface, iface_name);
								strcat(bridge_iface, ":");
							}else{
								sprintf(bridge_iface, "%s", iface_name);
								strcat(bridge_iface, ":");
							}
					}
                    else{
						RunSystemCmd(NULL_FILE, "ifconfig", iface_name, "up", NULL_STR);
					}

					if(SetWlan_idx( iface_name)){
						apmib_get( MIB_WLAN_WDS_ENABLED, (void *)&wlan_wds_enabled);
						apmib_get( MIB_WLAN_WDS_NUM, (void *)&wlan_wds_num);
						apmib_get( MIB_WLAN_MODE, (void *)&wlan_mode); //get wlan if mode
						if(wlan_wds_enabled !=0 && wlan_wds_num !=0 && (wlan_mode==WDS_MODE || wlan_mode==AP_WDS_MODE)){//add wds inface to br0
							for(j=0;j<wlan_wds_num;j++){
								sprintf(tmp_iface, "%s-wds%d", iface_name, j);
								RunSystemCmd(NULL_FILE, "brctl", "addif", "br0" ,tmp_iface, NULL_STR);
								RunSystemCmd(NULL_FILE, "ifconfig", tmp_iface, "0.0.0.0", NULL_STR);
								if(bridge_iface[0]){
									strcat(bridge_iface, tmp_iface);
									strcat(bridge_iface, ":");
								}else{
									sprintf(bridge_iface, "%s", tmp_iface);
									strcat(bridge_iface, ":");
								}
							}
						}
					}
				}
#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
				else if(wlan_disabled==2)//only in gateway mode
				{
					RunSystemCmd(NULL_FILE, "brctl", "addif", "br1" ,iface_name, NULL_STR);		
					RunSystemCmd(NULL_FILE, "ifconfig", iface_name, "0.0.0.0", NULL_STR); 
					if(bridge_iface[0])
					{
						strcat(bridge_iface2, iface_name);
						strcat(bridge_iface2, ":");
					}
					else
					{
							sprintf(bridge_iface2, "%s", iface_name);
							strcat(bridge_iface2, ":");
					}
				}
#endif
				//printf("[%s] [%s] [%s]\n",iface_name,bridge_iface,bridge_iface2);
			}
		}
		token = strtok_r(NULL, " ", &savestr1);
	}while(token !=NULL);


#if defined(CONFIG_RTL_ULINKER)
	if (opmode == 1) {
		int auto_wan;
		system("echo 1 > /proc/sw_nat");
		system("brctl addif br0 usb0 > /dev/null 2>&1");
		apmib_get(MIB_ULINKER_AUTO, (void *)&auto_wan);
	#if 0
		if (auto_wan == 0)
			system("ifconfig usb0 0.0.0.0 > /dev/null 2>&1");
	#endif
	}
	else {
		/* op_mode == 0, set usb0 in set_lan_dhcpd */
	}
#endif

//printf("\r\n bridge_iface=[%s],__[%s-%u]\r\n",bridge_iface,__FILE__,__LINE__);

	RunSystemCmd(BR_IFACE_FILE, "echo", bridge_iface, NULL_STR);

#ifdef CONFIG_RTK_VLAN_WAN_TAG_SUPPORT
  if(opmode == GATEWAY_MODE && vlan_wan_bridge_enable)
	{
		RunSystemCmd(BR_IFACE_FILE2, "echo", bridge_iface2, NULL_STR);
		RunSystemCmd(NULL_FILE, "ifconfig", "br1", "up", NULL_STR); 
	}
#endif
	
	//if(br_stp_enabled==0){/* Comment here because Enable Spanning tree and Clone Mac Address, then Apply changes, cannot access DUT .*/

		apmib_get(MIB_ELAN_MAC_ADDR,  (void *)tmpBuff);


		if(!memcmp(tmpBuff, "\x00\x00\x00\x00\x00\x00", 6)){
			apmib_get(MIB_HW_NIC0_ADDR,  (void *)tmpBuff);
		}
		sprintf(cmdBuffer, "%02x%02x%02x%02x%02x%02x", (unsigned char)tmpBuff[0], (unsigned char)tmpBuff[1], (unsigned char)tmpBuff[2], (unsigned char)tmpBuff[3], (unsigned char)tmpBuff[4], (unsigned char)tmpBuff[5]);
		RunSystemCmd(NULL_FILE, "ifconfig", "br0", "hw", "ether",cmdBuffer, NULL_STR);
	//}
	if(isFileExist(BR_INIT_FILE)==0){//bridge init file is not exist
		RunSystemCmd(NULL_FILE, "ifconfig", "br0", "0.0.0.0", NULL_STR);
		RunSystemCmd(BR_INIT_FILE, "echo", "1", NULL_STR);
	}

	if(dhcp_mode==DHCP_DISABLED || dhcp_mode==DHCP_SERVER //dhcp disabled or server mode or auto
#ifdef CONFIG_DOMAIN_NAME_QUERY_SUPPORT
		|| dhcp_mode==DHCP_AUTO
#endif
	)
	{
		apmib_get(MIB_IP_ADDR,  (void *)tmpBuff);
		sprintf(lanIp,"%s",inet_ntoa(*((struct in_addr *)tmpBuff)));

		apmib_get(MIB_SUBNET_MASK,  (void *)tmpBuff);
		sprintf(lanMask,"%s",inet_ntoa(*((struct in_addr *)tmpBuff)));

		apmib_get(MIB_DEFAULT_GATEWAY,  (void *)tmpBuff);
		sprintf(lanGateway,"%s",inet_ntoa(*((struct in_addr *)tmpBuff)));
		RunSystemCmd(NULL_FILE, "ifconfig", "br0", lanIp, "netmask",lanMask, NULL_STR);
		//hyking:sure the hw l3 table is correct, delete lan route & add lan route
		apmib_get(MIB_IP_ADDR,  &lan_addr);
		apmib_get(MIB_SUBNET_MASK,&lan_mask);
		lan_addr &= lan_mask;
		sprintf(lanIp,"%s",inet_ntoa(*((struct in_addr *)(&lan_addr))));
		RunSystemCmd(NULL_FILE, "route", "del", "-net",lanIp, "netmask",lanMask, NULL_STR);
		RunSystemCmd(NULL_FILE, "route", "add", "-net",lanIp, "netmask",lanMask, "dev","br0",NULL_STR);
		//end hyking added

		if(strcmp(lanGateway,"0.0.0.0")){
			RunSystemCmd(NULL_FILE, "route", "del", "default", "dev","br0", NULL_STR);
			RunSystemCmd(NULL_FILE, "route", "add", "-net", "default","gw", lanGateway, "dev", "br0", NULL_STR);
		}
	}else if(dhcp_mode==DHCP_CLIENT){//dhcp client

//		if(br_stp_enabled==1){		/* Comment here because br0 entering forwarding state always need some time especially booting up.*/
			printf("wait for bridge initialization...\n");
			intVal=10;
			do{
				intVal--;
				sleep(1);
			}while(intVal !=0);
//		}
		//RunSystemCmd(NULL_FILE, "dhcpc.sh", "br0", "no", NULL_STR);
		set_lan_dhcpc("br0");
	}
	#if defined(CONFIG_RTL_ULINKER)
	else if (dhcp_mode==DHCP_AUTO_WAN) {//dhcp client
		disable_bridge_dhcp_filter();
		set_lan_dhcpc("br0");
	}
	#endif

#if defined(CONFIG_RTL_WLAN_DOS_FILTER)
	RunSystemCmd("/proc/sw_nat", "echo", "9", NULL_STR);
#endif


    if(multiple_repeater==2 ){
        SDEBUG(" up br1 \n");
        RunSystemCmd(NULL_FILE, "ifconfig", "br1", "up", NULL_STR);         
    }
    #ifdef FOR_DUAL_BAND                            
    if(multiple_repeater2==2 ){
        SDEBUG(" up br1 \n");
        RunSystemCmd(NULL_FILE, "ifconfig", "br1", "up", NULL_STR);         
    }
    #endif
	return 0;
}

void lan_connect(char *interface, char *option)
{
	char dns_server[2][32]={0};
	char arg_buff[128]={0};
	char line[128]={0};
	char wanip[32]={0}, mask[32]={0},remoteip[32]={0};
	char *token=NULL, *savestr1=NULL;
	int index=0,x=0;
	char dns_found=0;
	
	if(strcmp(interface,"br0")!=0)
		return;	
		   
	sprintf(arg_buff, "%s", option);
	
		token = strtok_r(arg_buff," ", &savestr1);
		index=1;
		do{
			dns_found=0;
			if (token == NULL){/*check if the first arg is NULL*/
				break;
			}else{   
				if(index==2)
					sprintf(wanip, "%s", token); /*wan ip address */
				if(index==3)
					sprintf(mask, "%s", token); /*subnet mask*/
				if(index==4)
					sprintf(remoteip, "%s", token); /*gateway ip*/			
				if(index > 4){
					for(x=0;x<2;x++){
						if(dns_server[x][0] != '\0'){
							if(!strcmp(dns_server[x], token)){
								dns_found = 1; 
								break;
							}
						}
					}
					if(dns_found ==0){
						for(x=0;x<2;x++){
							if(dns_server[x][0] == '\0'){
								sprintf(dns_server[x], "%s", token);
								break;
							}
						}
					}
				}
			}
			index++;
			token = strtok_r(NULL, " ", &savestr1);
		}while(token !=NULL);  
		
		RunSystemCmd(NULL_FILE, "ifconfig", interface, wanip, "netmask", mask, NULL_STR);	
		RunSystemCmd(NULL_FILE, "route", "del", "default", NULL_STR);
		RunSystemCmd(NULL_FILE, "route", "add", "-net", "default", "gw", remoteip, "dev", interface, NULL_STR);
		//printf("%s(%d): wan_type=%d,dns_mode=%d\n",__FUNCTION__,__LINE__, wan_type,dns_mode);//Added for test
		for(x=0;x<2;x++){
			if(dns_server[x][0] != '\0'){					
				sprintf(line,"nameserver %s\n", dns_server[x]);
				if(x==0)
					write_line_to_file(RESOLV_CONF, 1, line);
				else
					write_line_to_file(RESOLV_CONF, 2, line);
			}
		}	
#if defined(CONFIG_APP_SIMPLE_CONFIG)
		system("echo 2 > /var/sc_ip_status");
#endif
}
#ifdef SUPPORT_ZIONCOM_RUSSIA

void check_l2tp_status()
{	
	FILE *fp=NULL;
	if((fp=fopen("/var/dhcpc_route.conf", "r"))==NULL)
		return;
	
	unsigned char routebuf[16];
	unsigned char cmdbuf[128];
		
	fscanf(fp, "%s", routebuf);
	fclose(fp);

	system("route del default dev eth1 > /dev/null 2>&1");
	system("route del default dev ppp0 > /dev/null 2>&1");
	
	sprintf(cmdbuf, "route add -net default gw %s dev eth1 > /dev/null 2>&1", routebuf);
	system(cmdbuf);	

	setFirewallIptablesRules(-1, NULL);
//	RunSystemCmd(NULL_FILE, "killall", "-9", "l2tpd", NULL_STR);
//	sleep(2);
	
//	system("l2tpd &");
	system("echo c client > /var/run/l2tp-control 2>/dev/null");
	
#if 1
	if((fp=fopen("/var/dnrd_cmd_line", "r"))!=NULL)
	{
		fgets(cmdbuf, 128, fp);
		fclose(fp);
		system(cmdbuf);
	}	
	
	RunSystemCmd(NULL_FILE, "igmpproxy", "eth1", "br0", NULL_STR);
#endif
	
	return;
}
#endif
