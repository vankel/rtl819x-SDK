
#include <stdio.h>
#include <assert.h>
#include <arpa/inet.h>
#include "Console.h"
#include "../../boa/apmib/apmib.h"
#include "../rtk_api/rtk_api.h"
#include "../rtk_api/rtk_firewall_adapter.h"
#include "../rtk_api/rtk_disk_adapter.h"

#define RTK_DECLARE_STRUCT_EQUAL_FUNC(Name,TYPE,set_func,get_func) \
int Name##_is_equal(TYPE *setParam) \
{ \
	TYPE getParam; \
	memset(&getParam,0,sizeof(TYPE));\
	CU_ASSERT_EQUAL(set_func(setParam),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(&getParam),RTK_SUCCESS); \
	if(memcmp(&getParam,setParam,sizeof(TYPE))==0) \
		return 1; \
	return 0; \
}
#define RTK_DECLARE_INT_EQUAL_FUNC(Name,set_func,get_func) \
int Name##_is_equal(int setParam) \
{ \
	int getParam; \
	CU_ASSERT_EQUAL(set_func(setParam),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(&getParam),RTK_SUCCESS); \
	if(setParam == getParam) \
		return 1;\
	return 0; \
}

#define RTK_DECLARE_ARRAY_EQUAL_FUNC(Name,set_func,get_func) \
int Name##_is_equal(unsigned char *setParam) \
{ \
	unsigned char getParam[100] = {0}; \
	CU_ASSERT_EQUAL(set_func(setParam),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(getParam),RTK_SUCCESS); \
	if(strcmp(getParam,setParam) == 0) \
		return 1;\
	return 0; \
}

#define RTK_DECLARE_TBL_EQUAL_FUNC(Name,TYPE,set_func,get_func,del_func) \
int Name##_is_equal(TYPE* setParam) \
{ \
	TYPE getParam; \
	int num; \
	memset(&getParam,0,sizeof(TYPE));\
	CU_ASSERT_EQUAL(del_func(setParam,1),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(set_func(setParam),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(&num,&getParam,1),RTK_SUCCESS); \
	if(memcmp(&getParam,setParam,sizeof(TYPE))==0) \
		return 1; \
	return 0; \
}
// wlan
#define RTK_DECLARE_WLAN_STRUCT_EQUAL_FUNC(Name,TYPE,set_func,get_func) \
int Name##_is_equal(unsigned char *ifname,TYPE *setParam) \
{ \
	TYPE getParam; \
	memset(&getParam,0,sizeof(TYPE));\
	CU_ASSERT_EQUAL(set_func(ifname,setParam),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(ifname,&getParam),RTK_SUCCESS); \
	if(memcmp(&getParam,setParam,sizeof(TYPE))==0) \
		return 1; \
	return 0; \
}
#define RTK_DECLARE_WLAN_INT_EQUAL_FUNC(Name,set_func,get_func) \
int Name##_is_equal(unsigned char *ifname,int setParam) \
{ \
	int getParam; \
	CU_ASSERT_EQUAL(set_func(ifname,setParam),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(ifname,&getParam),RTK_SUCCESS); \
	if(setParam == getParam) \
		return 1;\
	return 0; \
}
#define RTK_DECLARE_WLAN_ARRAY_EQUAL_FUNC(Name,set_func,get_func) \
int Name##_is_equal(unsigned char *ifname,unsigned char *setParam) \
{ \
	unsigned char getParam[100] = {0}; \
	CU_ASSERT_EQUAL(set_func(ifname,setParam),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(ifname,getParam),RTK_SUCCESS); \
	if(strcmp(getParam,setParam) == 0) \
		return 1;\
	return 0; \
}

#define RTK_DECLARE_WLAN_TBL_EQUAL_FUNC(Name,TYPE,set_func,get_func,del_func) \
int Name##_is_equal(unsigned char *ifname,TYPE* setParam) \
{ \
	TYPE getParam; \
	int num; \
	memset(&getParam,0,sizeof(TYPE));\
	CU_ASSERT_EQUAL(del_func(ifname,setParam,1),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(set_func(ifname,setParam),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(ifname,&num,&getParam,1),RTK_SUCCESS); \
	if(memcmp(&getParam,setParam,sizeof(TYPE))==0) \
		return 1; \
	return 0; \
}

//wlan security setting
#define RTK_DECLARE_WLAN_SECURITY_5PARAM_EQUAL_FUNC(Name,set_func,get_func) \
int Name##_is_equal(unsigned char *ifname,unsigned int setParam1,unsigned int setParam2,unsigned int setParam3,unsigned int setParam4,unsigned char* setParam5) \
{ \
	unsigned int getParam1,getParam2,getParam3,getParam4; \
	unsigned char getParam5[64]; \
	CU_ASSERT_EQUAL(set_func(ifname,setParam1,setParam2,setParam3,setParam4,setParam5),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(ifname,&getParam1,&getParam2,&getParam3,&getParam4,getParam5),RTK_SUCCESS); \
	if(setParam1==getParam1 && setParam2==getParam2 && setParam3==getParam3 && setParam4==getParam4 && (!strcmp(setParam5,getParam5)))  \
		return 1; \
}
#define RTK_DECLARE_WLAN_SECURITY_4PARAM_EQUAL_FUNC(Name,set_func,get_func) \
int Name##_is_equal(unsigned char *ifname,unsigned int setParam1,unsigned int setParam2,unsigned int setParam3,unsigned int setParam4) \
{ \
	unsigned int getParam1,getParam2,getParam3; \
	unsigned char getParam4[64]; \
	CU_ASSERT_EQUAL(set_func(ifname,setParam1,setParam2,setParam3,setParam4),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(ifname,&getParam1,&getParam2,&getParam3,getParam4),RTK_SUCCESS); \
	if(setParam1==getParam1 && setParam2==getParam2 && setParam3==getParam3 && (!strcmp(setParam4,getParam4)))  \
		return 1; \
	return 0; \
}

//wlan access control
#define RTK_DECLARE_WLAN_ACL_EQUAL_FUNC(Name,add_func,get_func,del_func) \
int Name##_is_equal(unsigned char *ifname,unsigned int setParam1,unsigned char* setParam2) \
{ \
	unsigned int i,getParam1; \
	unsigned char getParam2[20 * 6]; \
	CU_ASSERT_EQUAL(del_func(ifname,getParam2,1),RTK_SUCCESS);\
	for(i=0;i<setParam1;i++) \
		CU_ASSERT_EQUAL(add_func(ifname,setParam2+(i*6)),RTK_SUCCESS);\
	CU_ASSERT_EQUAL(get_func(ifname,&getParam1,getParam2,20),RTK_SUCCESS); \
	if(!(setParam1==getParam1 && (!memcmp(setParam2,getParam2,getParam1*6)))) \
		return 0; \
	for(i=0;i<setParam1;i++) \
		CU_ASSERT_EQUAL(del_func(ifname,setParam2+(i*6),0),RTK_SUCCESS); \
	CU_ASSERT_EQUAL(get_func(ifname,&getParam1,getParam2,20),RTK_SUCCESS); \
	if(getParam1 == 0) \
		return 1;\
	return 0; \
}


#define RTK_DECLARE_TEST_FUNC(Name)\
void Name##_test()

#define RTK_TEST_INFO_DEF(Name)\
	{""#Name"",Name##_test}




RTK_DECLARE_TEST_FUNC(wlan_enable)
{
	int enable;
	CU_ASSERT_EQUAL(rtk_wlan_enable("wlan0"),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_wlan_get_enable("wlan0",&enable),RTK_SUCCESS);
	CU_ASSERT_EQUAL(enable,1);
	CU_ASSERT_EQUAL(rtk_wlan_disable("wlan0"),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_wlan_get_enable("wlan0",&enable),RTK_SUCCESS);
	CU_ASSERT_EQUAL(enable,0);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_mode,rtk_wlan_set_mode,rtk_wlan_get_mode)

RTK_DECLARE_TEST_FUNC(wlan_mode)
{
	int mode = 1;
	CU_ASSERT_EQUAL(wlan_mode_is_equal("wlan0",mode),1);
}

RTK_DECLARE_WLAN_ARRAY_EQUAL_FUNC\
(wlan_ssid,rtk_wlan_set_ssid,rtk_wlan_get_ssid)
	

RTK_DECLARE_TEST_FUNC(wlan_ssid)
{
	unsigned char *ssid = "12345679";
	CU_ASSERT_EQUAL(wlan_ssid_is_equal("wlan0",ssid),1);
}

//wlan advance setting
RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_fragthres,rtk_wlan_set_fragthres,rtk_wlan_get_fragthres)

RTK_DECLARE_TEST_FUNC(wlan_fragthres)
{
	int fragthres = 2346;
	CU_ASSERT_EQUAL(wlan_fragthres_is_equal("wlan0",fragthres),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_rtsthres,rtk_wlan_set_rtsthres,rtk_wlan_get_rtsthres)

RTK_DECLARE_TEST_FUNC(wlan_rtsthres)
{
	int rtsthres = 2347;
	CU_ASSERT_EQUAL(wlan_rtsthres_is_equal("wlan0",rtsthres),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_beacon_interval,rtk_wlan_set_beacon_interval,rtk_wlan_get_beacon_interval)

RTK_DECLARE_TEST_FUNC(wlan_beacon_interval)
{
	int beacon_interval = 100;
	CU_ASSERT_EQUAL(wlan_beacon_interval_is_equal("wlan0",beacon_interval),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_preamble,rtk_wlan_set_preamble,rtk_wlan_get_preamble)

RTK_DECLARE_TEST_FUNC(wlan_preamble)
{
	int enable = 0;
	CU_ASSERT_EQUAL(wlan_preamble_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_protection,rtk_wlan_set_protection,rtk_wlan_get_protection)

RTK_DECLARE_TEST_FUNC(wlan_protection)
{
	int disabled = 1;
	CU_ASSERT_EQUAL(wlan_protection_is_equal("wlan0",disabled),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_aggregation,rtk_wlan_set_aggregation,rtk_wlan_get_aggregation)

RTK_DECLARE_TEST_FUNC(wlan_aggregation)
{
	int enable = 1;
	CU_ASSERT_EQUAL(wlan_aggregation_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_shortgi,rtk_wlan_set_shortgi,rtk_wlan_get_shortgi)

RTK_DECLARE_TEST_FUNC(wlan_shortgi)
{
	int enable = 1;
	CU_ASSERT_EQUAL(wlan_shortgi_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_isolation,rtk_wlan_set_isolation,rtk_wlan_get_isolation)

RTK_DECLARE_TEST_FUNC(wlan_isolation)
{
	int enable = 0;
	CU_ASSERT_EQUAL(wlan_isolation_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_stbc,rtk_wlan_set_stbc,rtk_wlan_get_stbc)

RTK_DECLARE_TEST_FUNC(wlan_stbc)
{
	int enable = 1;
	CU_ASSERT_EQUAL(wlan_stbc_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_ldpc,rtk_wlan_set_ldpc,rtk_wlan_get_ldpc)

RTK_DECLARE_TEST_FUNC(wlan_ldpc)
{
	int enable = 1;
	CU_ASSERT_EQUAL(wlan_ldpc_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_coexist,rtk_wlan_set_coexist,rtk_wlan_get_coexist)

RTK_DECLARE_TEST_FUNC(wlan_coexist)
{
	int enable = 0;
	CU_ASSERT_EQUAL(wlan_coexist_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_txbeamforming,rtk_wlan_set_txbeamforming,rtk_wlan_get_txbeamforming)

RTK_DECLARE_TEST_FUNC(wlan_txbeamforming)
{
	int enable = 1;
	CU_ASSERT_EQUAL(wlan_txbeamforming_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_mc2u,rtk_wlan_set_mc2u,rtk_wlan_get_mc2u)

RTK_DECLARE_TEST_FUNC(wlan_mc2u)
{
	int enable = 0;
	CU_ASSERT_EQUAL(wlan_mc2u_is_equal("wlan0",enable),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_rfpower,rtk_wlan_set_rfpower,rtk_wlan_get_rfpower)

RTK_DECLARE_TEST_FUNC(wlan_rfpower)
{
	int level = 0;
	CU_ASSERT_EQUAL(wlan_rfpower_is_equal("wlan0",level),1);
}

//wlan security setting
RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_encryption,rtk_wlan_set_encryption,rtk_wlan_get_encryption)

RTK_DECLARE_TEST_FUNC(wlan_encryption)
{
	int encrypt = 0;
	CU_ASSERT_EQUAL(wlan_encryption_is_equal("wlan0",encrypt),1);
}

RTK_DECLARE_WLAN_SECURITY_5PARAM_EQUAL_FUNC\
(wlan_security_wep,rtk_wlan_set_security_wep,rtk_wlan_get_security_wep)

RTK_DECLARE_TEST_FUNC(wlan_security_wep)
{ 	unsigned int auth_type=2, key_len=0, key_type=1, default_key_idx=0;
	unsigned char key[16];
	memset(key,0,10);
	key[10] = '\0';
	CU_ASSERT_EQUAL(wlan_security_wep_is_equal("wlan0",auth_type,key_len,key_type,default_key_idx,key),1);
}

RTK_DECLARE_WLAN_SECURITY_4PARAM_EQUAL_FUNC\
(wlan_security_wpa,rtk_wlan_set_security_wpa,rtk_wlan_get_security_wpa)

RTK_DECLARE_TEST_FUNC(wlan_security_wpa)
{	
	unsigned int auth_type=2,cipher_suite=1,key_type=0;
	unsigned char key[4] = "";
	CU_ASSERT_EQUAL(wlan_security_wpa_is_equal("wlan0",auth_type,cipher_suite,key_type,key),1);
}

RTK_DECLARE_WLAN_SECURITY_4PARAM_EQUAL_FUNC\
(wlan_security_wpa2,rtk_wlan_set_security_wpa2,rtk_wlan_get_security_wpa2)

RTK_DECLARE_TEST_FUNC(wlan_security_wpa2)
{	
	unsigned int auth_type=2,cipher_suite=2,key_type=0;
	unsigned char key[4] = "";
	CU_ASSERT_EQUAL(wlan_security_wpa2_is_equal("wlan0",auth_type,cipher_suite,key_type,key),1);
}

RTK_DECLARE_WLAN_SECURITY_5PARAM_EQUAL_FUNC\
(wlan_security_wpamix,rtk_wlan_set_security_wpamix,rtk_wlan_get_security_wpamix)

RTK_DECLARE_TEST_FUNC(wlan_security_wpamix)
{	
	unsigned int auth_type=2,wpa_cipher_suite=1,wpa2_cipher_suite=2,key_type=0;
	unsigned char key[4] = "";
	CU_ASSERT_EQUAL(wlan_security_wpamix_is_equal("wlan0",auth_type,wpa_cipher_suite,wpa2_cipher_suite,key_type,key),1);
}

//wlan access control
RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_acl_mode,rtk_wlan_set_acl_mode,rtk_wlan_get_acl_mode)

RTK_DECLARE_TEST_FUNC(wlan_acl_mode)
{	
	unsigned acl_mode = 0;
	CU_ASSERT_EQUAL(wlan_acl_mode_is_equal("wlan0",acl_mode),1);
}

RTK_DECLARE_WLAN_ACL_EQUAL_FUNC\
(wlan_acl_entry,rtk_wlan_add_acl_entry,rtk_wlan_get_acl_entry,rtk_wlan_del_acl_entry)

RTK_DECLARE_TEST_FUNC(wlan_acl_entry)
{	
	unsigned int mac_num=3;
	unsigned int empty_num = 20;
	unsigned char mac[6*empty_num];
	memset(mac,1,6);
	memset(&mac[6],2,6);
	memset(&mac[12],3,6);
	CU_ASSERT_EQUAL(wlan_acl_entry_is_equal("wlan0",mac_num,mac),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_band,rtk_wlan_set_band,rtk_wlan_get_band)

RTK_DECLARE_TEST_FUNC(wlan_band)
{
	int band = 8;
	CU_ASSERT_EQUAL(wlan_band_is_equal("wlan0",band),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_network_type,rtk_wlan_set_network_type,rtk_wlan_get_network_type)

RTK_DECLARE_TEST_FUNC(wlan_network_type)
{
	int network_type = 1;
	CU_ASSERT_EQUAL(wlan_network_type_is_equal("wlan0",network_type),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_channel_bandwidth,rtk_wlan_set_channel_bandwidth,rtk_wlan_get_channel_bandwidth)

RTK_DECLARE_TEST_FUNC(wlan_channel_bandwidth)
{
	int channel_bandwidth = 1;
	CU_ASSERT_EQUAL(wlan_channel_bandwidth_is_equal("wlan0",channel_bandwidth),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_2ndchoff,rtk_wlan_set_2ndchoff,rtk_wlan_get_2ndchoff)

RTK_DECLARE_TEST_FUNC(wlan_2ndchoff)
{
	int secondchoff = 1;
	CU_ASSERT_EQUAL(wlan_2ndchoff_is_equal("wlan0",secondchoff),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_channel,rtk_wlan_set_channel,rtk_wlan_get_channel)

RTK_DECLARE_TEST_FUNC(wlan_channel)
{
	int channel = 5;
	CU_ASSERT_EQUAL(wlan_channel_is_equal("wlan0",channel),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_hidden_ssid,rtk_wlan_set_hidden_ssid,rtk_wlan_get_hidden_ssid)

RTK_DECLARE_TEST_FUNC(wlan_hidden_ssid)
{
	int hidden_ssid = 1;
	CU_ASSERT_EQUAL(wlan_hidden_ssid_is_equal("wlan0",hidden_ssid),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_fixrate,rtk_wlan_set_fixrate,rtk_wlan_get_fixrate)

RTK_DECLARE_TEST_FUNC(wlan_fixrate)
{
	int fixrate = 5;
	CU_ASSERT_EQUAL(wlan_fixrate_is_equal("wlan0",fixrate),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_txrestrict,rtk_wlan_set_txrestrict,rtk_wlan_get_txrestrict)

RTK_DECLARE_TEST_FUNC(wlan_txrestrict)
{
	int txrestrict = 5;
	CU_ASSERT_EQUAL(wlan_txrestrict_is_equal("wlan0",txrestrict),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_rxrestrict,rtk_wlan_set_rxrestrict,rtk_wlan_get_rxrestrict)

RTK_DECLARE_TEST_FUNC(wlan_rxrestrict)
{
	int rxrestrict = 5;
	CU_ASSERT_EQUAL(wlan_rxrestrict_is_equal("wlan0",rxrestrict),1);
}

RTK_DECLARE_TEST_FUNC(wlan_enable_repeater)
{
	int enable;
	CU_ASSERT_EQUAL(rtk_wlan_set_enable_repeater("wlan0"),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_wlan_get_enable_repeater("wlan0",&enable),RTK_SUCCESS);
	CU_ASSERT_EQUAL(enable,1);
	CU_ASSERT_EQUAL(rtk_wlan_set_disable_repeater("wlan0"),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_wlan_get_enable_repeater("wlan0",&enable),RTK_SUCCESS);
	CU_ASSERT_EQUAL(enable,0);
}

RTK_DECLARE_WLAN_ARRAY_EQUAL_FUNC\
(wlan_repeater_ssid,rtk_wlan_set_repeater_ssid,rtk_wlan_get_repeater_ssid)

RTK_DECLARE_TEST_FUNC(wlan_repeater_ssid)
{
	char *ssid = "567891023";
	CU_ASSERT_EQUAL(wlan_repeater_ssid_is_equal("wlan0",ssid),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_wds,rtk_wlan_set_wds,rtk_wlan_get_wds)

RTK_DECLARE_TEST_FUNC(wlan_wds)
{
	int wds = 1;
	CU_ASSERT_EQUAL(wlan_wds_is_equal("wlan0",wds),1);
}

int wlan_wds_entry_is_equal(unsigned char *ifname,unsigned char *Set_mac)
{
	unsigned char Get_mac[12];
	int num;
	CU_ASSERT_EQUAL(rtk_wlan_del_wds_entry(ifname,Set_mac,1),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_wlan_add_wds_entry(ifname,Set_mac),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_wlan_get_wds_entry(ifname,&num,Get_mac,1),RTK_SUCCESS);
	if(memcmp(Get_mac,Set_mac,6) == 0)
		return 1;
	return 0;
}
RTK_DECLARE_TEST_FUNC(wlan_wds_entry)
{
	unsigned char mac[6] = {0x00,0x55,0x66,0x77,0x88,0xff};
	CU_ASSERT_EQUAL(wlan_wds_entry_is_equal("wlan0",mac),1);
}

RTK_DECLARE_WLAN_INT_EQUAL_FUNC\
(wlan_schedule,rtk_wlan_set_schdule,rtk_wlan_get_schdule)

RTK_DECLARE_TEST_FUNC(wlan_schedule)
{
	int schedule = 1;
	CU_ASSERT_EQUAL(wlan_schedule_is_equal("wlan0",schedule),1);
}

RTK_DECLARE_WLAN_TBL_EQUAL_FUNC\
(wlan_schedule_entry,RTK_SCHEDULE_T,rtk_wlan_add_schedule_entry,\
rtk_wlan_get_schedule_entry,rtk_wlan_del_schedule_entry)

RTK_DECLARE_TEST_FUNC(wlan_schedule_entry)
{
	RTK_SCHEDULE_T entry;
	int num;
	memset(&entry,0,sizeof(RTK_SCHEDULE_T));
	entry.day = 1;
	entry.eco = 0;
	entry.from_time= 880;
	entry.to_time = 990;
	CU_ASSERT_EQUAL(wlan_schedule_entry_is_equal("wlan0",&entry),1);
}

RTK_DECLARE_TEST_FUNC(wlan_sta_list)
{
	char *buff;
	RTK_WLAN_STA_INFO_Tp pInfo;
	int i;
	buff = calloc(1, sizeof(RTK_WLAN_STA_INFO_T) * (MAX_STA_NUM+1));
	if(!buff)
		return;
	memset(buff,0,sizeof(RTK_WLAN_STA_INFO_T) * (MAX_STA_NUM +1));
	CU_ASSERT_EQUAL(rtk_get_wlan_sta("wlan0",(RTK_WLAN_STA_INFO_Tp)buff),RTK_SUCCESS);
	printf("\nClient MAC		TX packets			RX packets\n");
	for (i=1; i<=MAX_STA_NUM; i++) {
		pInfo = (RTK_WLAN_STA_INFO_Tp)&buff[i*sizeof(RTK_WLAN_STA_INFO_T)];
		if (pInfo->aid && (pInfo->flag & STA_INFO_FLAG_ASOC)){
			printf("%02x:%02x:%02x:%02x:%02x:%02x	%d	%d\n",
				pInfo->addr[0],pInfo->addr[1],pInfo->addr[2],pInfo->addr[3],pInfo->addr[4],pInfo->addr[5],
				pInfo->tx_packets, pInfo->rx_packets);
		}
	}
}


RTK_DECLARE_INT_EQUAL_FUNC\
(upnp_enable,rtk_set_upnp_enable,rtk_get_upnp_enable)

RTK_DECLARE_TEST_FUNC(upnp_enable)
{
	int enable =1;
	CU_ASSERT_EQUAL(upnp_enable_is_equal(enable),1);
}

RTK_DECLARE_TEST_FUNC(upnp_list)
{
	RTK_UPNP_MAPPING_INFO_T info[10];
	RTK_UPNP_MAPPING_INFO_Tp pinfo;
	int num;
	int i;
	CU_ASSERT_EQUAL(rtk_get_upnp_list(info, &num,10),RTK_SUCCESS);
	printf("\ndescription		client		protocol	iport	export\n");
	for(i=0;i < 10 && i < num;i++){
		pinfo = &info[i];
		printf("%s		%s		%s	%d	%d\n",pinfo->desc,inet_ntoa(*(struct in_addr *)&pinfo->in_ip),
			pinfo->proto,pinfo->iport,pinfo->eport);
	}
}

//port forwarding related
RTK_DECLARE_INT_EQUAL_FUNC\
(fwall_port_fw_enable, rtk_set_port_forward_enable, rtk_get_port_forward_enable)

RTK_DECLARE_TEST_FUNC(fwall_port_fw_enable)
{
		int enable;

		enable =  1;
		CU_ASSERT_EQUAL(fwall_port_fw_enable_is_equal(enable),1);
		
		enable =  0;
		CU_ASSERT_EQUAL(fwall_port_fw_enable_is_equal(enable),1);
}

RTK_DECLARE_TBL_EQUAL_FUNC\
(fwall_port_fw,RTK_PORTFW_T,rtk_add_port_forward_entry,rtk_get_port_forward_entry,rtk_del_port_forward_entry)

RTK_DECLARE_TEST_FUNC(fwall_port_fw)
{
		RTK_PORTFW_T entry;
		memset((void *)&entry, '\0', sizeof(entry));
		entry.ipAddr[0] = 192;
		entry.ipAddr[1] = 168;
		entry.ipAddr[2] = 1;
		entry.ipAddr[3] = 111;
		entry.fromPort = 1000;
		entry.toPort = 1111;
		entry.protoType = 3;
		CU_ASSERT_EQUAL(fwall_port_fw_is_equal(&entry),1);
}

//port filter related
RTK_DECLARE_INT_EQUAL_FUNC\
(fwall_port_filter_enable, rtk_set_port_filter_enable, rtk_get_port_filter_enable)

RTK_DECLARE_TEST_FUNC(fwall_port_filter_enable)
{
		int enable;

		enable =  1;
		CU_ASSERT_EQUAL(fwall_port_filter_enable_is_equal(enable),1);
		
		enable =  0;
		CU_ASSERT_EQUAL(fwall_port_filter_enable_is_equal(enable),1);
}

RTK_DECLARE_TBL_EQUAL_FUNC\
(fwall_port_filter,RTK_PORTFILTER_T,rtk_add_port_filter_entry,rtk_get_port_filter_entry,rtk_del_port_filter_entry)

RTK_DECLARE_TEST_FUNC(fwall_port_filter)
{
		RTK_PORTFILTER_T entry;
		memset((void *)&entry, '\0', sizeof(entry));
		entry.fromPort = 1000;
		entry.toPort = 1111;
		entry.protoType = 3;
		CU_ASSERT_EQUAL(fwall_port_filter_is_equal(&entry),1);
}

//ip filter related
RTK_DECLARE_INT_EQUAL_FUNC\
(fwall_ip_filter_enable, rtk_set_ip_filter_enable, rtk_get_ip_filter_enable)

RTK_DECLARE_TEST_FUNC(fwall_ip_filter_enable)
{
		int enable;

		enable =  1;
		CU_ASSERT_EQUAL(fwall_ip_filter_enable_is_equal(enable),1);
		
		enable =  0;
		CU_ASSERT_EQUAL(fwall_ip_filter_enable_is_equal(enable),1);
}

RTK_DECLARE_TBL_EQUAL_FUNC\
(fwall_ip_filter,RTK_IPFILTER_T,rtk_add_ip_filter_entry,rtk_get_ip_filter_entry,rtk_del_ip_filter_entry)

RTK_DECLARE_TEST_FUNC(fwall_ip_filter)
{
		RTK_IPFILTER_T entry;
		memset((void *)&entry, '\0', sizeof(entry));
		entry.ipAddr[0] = 192;
		entry.ipAddr[1] = 168;
		entry.ipAddr[2] = 1;
		entry.ipAddr[3] = 111;
		entry.protoType = 3;

		CU_ASSERT_EQUAL(fwall_ip_filter_is_equal(&entry),1);
}

//mac filter related
RTK_DECLARE_INT_EQUAL_FUNC\
(fwall_mac_filter_enable, rtk_set_mac_filter_enable, rtk_get_mac_filter_enable)

RTK_DECLARE_TEST_FUNC(fwall_mac_filter_enable)
{
		int enable;

		enable =  1;
		CU_ASSERT_EQUAL(fwall_mac_filter_enable_is_equal(enable),1);
		
		enable =  0;
		CU_ASSERT_EQUAL(fwall_mac_filter_enable_is_equal(enable),1);
}

RTK_DECLARE_TBL_EQUAL_FUNC\
(fwall_mac_filter,RTK_MACFILTER_T,rtk_add_mac_filter_entry,rtk_get_mac_filter_entry,rtk_del_mac_filter_entry)

RTK_DECLARE_TEST_FUNC(fwall_mac_filter)
{
		RTK_MACFILTER_T entry;
		memset((void *)&entry, '\0', sizeof(entry));
		entry.macAddr[0] = 0x00;
		entry.macAddr[1] = 0x00;
		entry.macAddr[2] = 0x00;
		entry.macAddr[3] = 0x00;
		entry.macAddr[4] = 0x01;
		entry.macAddr[5] = 0x02;
		CU_ASSERT_EQUAL(fwall_mac_filter_is_equal(&entry),1);
}

//nas filter related
RTK_DECLARE_INT_EQUAL_FUNC\
(fwall_nas_filter_enable, rtk_set_nas_filter_enable, rtk_get_nas_filter_enable)

RTK_DECLARE_TEST_FUNC(fwall_nas_filter_enable)
{
		int enable;

		enable =  1;
		CU_ASSERT_EQUAL(fwall_nas_filter_enable_is_equal(enable),1);
		
		enable =  0;
		CU_ASSERT_EQUAL(fwall_nas_filter_enable_is_equal(enable),1);
}

RTK_DECLARE_TBL_EQUAL_FUNC\
(fwall_nas_filter,RTK_NASFILTER_T,rtk_add_nas_filter_entry,rtk_get_nas_filter_entry,rtk_del_nas_filter_entry)

RTK_DECLARE_TEST_FUNC(fwall_nas_filter)
{
		RTK_NASFILTER_T entry;
		memset((void *)&entry, '\0', sizeof(entry));
		entry.macAddr[0] = 0x04;
		entry.macAddr[1] = 0x7d;
		entry.macAddr[2] = 0x7b;
		entry.macAddr[3] = 0x65;
		entry.macAddr[4] = 0x70;
		entry.macAddr[5] = 0xc3;
		CU_ASSERT_EQUAL(fwall_nas_filter_is_equal(&entry),1);
}


//url filter related
RTK_DECLARE_INT_EQUAL_FUNC\
(fwall_url_filter_enable, rtk_set_url_filter_enable, rtk_get_url_filter_enable)

RTK_DECLARE_TEST_FUNC(fwall_url_filter_enable)
{
		int enable;

		enable =  1;
		CU_ASSERT_EQUAL(fwall_url_filter_enable_is_equal(enable),1);
		
		//enable =  0;
		//CU_ASSERT_EQUAL(fwall_url_filter_enable_is_equal(enable),1);
}

RTK_DECLARE_INT_EQUAL_FUNC\
(fwall_url_filter_mode, rtk_set_url_filter_mode, rtk_get_url_filter_mode)

RTK_DECLARE_TEST_FUNC(fwall_url_filter_mode)
{
		int mode;

		mode =  1;
		CU_ASSERT_EQUAL(fwall_url_filter_mode_is_equal(mode),1);
}

RTK_DECLARE_TBL_EQUAL_FUNC\
(fwall_url_filter,RTK_URLFILTER_T,rtk_add_url_filter_entry,rtk_get_url_filter_entry,rtk_del_url_filter_entry)

RTK_DECLARE_TEST_FUNC(fwall_url_filter)
{
		RTK_URLFILTER_T entry;
		memset((void *)&entry, '\0', sizeof(entry));
		entry.ruleMode = 1;
		memcpy(entry.urlAddr, "baidu.com", strlen("baidu.com"));
		
		CU_ASSERT_EQUAL(fwall_url_filter_is_equal(&entry),1);
}

RTK_DECLARE_TEST_FUNC(fwall_url_filter_take_effect)
{
	RTK_URLFILTER_T entry;
	unsigned char cmd[128];
	int enable = 0, mode = 0;

	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "echo flush >/proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);
	
	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "echo init 3 >/proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);

	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "baidu.com", strlen("baidu.com"));
	
	CU_ASSERT_EQUAL(rtk_add_url_filter_entry(&entry),RTK_SUCCESS);

	
	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "hujiang.com", strlen("hujiang.com"));
	
	CU_ASSERT_EQUAL(rtk_add_url_filter_entry(&entry),RTK_SUCCESS);

	
	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "www.sina.com", strlen("www.sina.com"));
	
	CU_ASSERT_EQUAL(rtk_add_url_filter_entry(&entry),RTK_SUCCESS);

	
	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "cat /proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);

	//delete
	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "www.sina.com", strlen("www.sina.com"));
	
	CU_ASSERT_EQUAL(rtk_del_url_filter_entry(&entry, 0),RTK_SUCCESS);

	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "cat /proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);

	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "hujiang.com", strlen("hujiang.com"));
	
	CU_ASSERT_EQUAL(rtk_del_url_filter_entry(&entry, 0),RTK_SUCCESS);

	
	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "cat /proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);

	
	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "baidu.com", strlen("baidu.com"));
	
	CU_ASSERT_EQUAL(rtk_del_url_filter_entry(&entry, 0),RTK_SUCCESS);

	
	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "cat /proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);


	enable = 1;
	CU_ASSERT_EQUAL(rtk_set_url_filter_enable(enable),RTK_SUCCESS);
	
	mode = 0;
	CU_ASSERT_EQUAL(rtk_set_url_filter_mode(mode),RTK_SUCCESS);
	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "baidu.com", strlen("baidu.com"));
	
	CU_ASSERT_EQUAL(rtk_add_url_filter_entry(&entry),RTK_SUCCESS);

	
	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "hujiang.com", strlen("hujiang.com"));
	
	CU_ASSERT_EQUAL(rtk_add_url_filter_entry(&entry),RTK_SUCCESS);

	
	memset((void *)&entry, '\0', sizeof(entry));
	entry.ruleMode = 0;
	memcpy(entry.urlAddr, "www.sina.com", strlen("www.sina.com"));
	
	CU_ASSERT_EQUAL(rtk_add_url_filter_entry(&entry),RTK_SUCCESS);

	
	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "cat /proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);
	
	enable = 0;
	CU_ASSERT_EQUAL(rtk_set_url_filter_enable(enable),RTK_SUCCESS);
	
	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "cat /proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);
	
	enable = 1;
	CU_ASSERT_EQUAL(rtk_set_url_filter_enable(enable),RTK_SUCCESS);

	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "cat /proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);

	
	mode = 0;
	CU_ASSERT_EQUAL(rtk_set_url_filter_mode(mode),RTK_SUCCESS);
	
	memset(cmd, '\0', sizeof(cmd));
	sprintf(cmd, "cat /proc/filter_table");
	//printf("%s %d cmd=%s\n", __FUNCTION__, __LINE__, cmd);
	system(cmd);
		
}

//dmz related
RTK_DECLARE_TEST_FUNC(fwall_dmz)
{
	unsigned int enabled; 
	unsigned int dmz_ip;
	enabled = 1;
	dmz_ip = 0xc0a80170;
	

	CU_ASSERT_EQUAL(rtk_set_dmz(enabled, dmz_ip),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_get_dmz(&enabled, &dmz_ip),RTK_SUCCESS);
	CU_ASSERT_EQUAL(enabled,1);
	CU_ASSERT_EQUAL(dmz_ip,0xc0a80170);
}

//qos related
RTK_DECLARE_INT_EQUAL_FUNC\
(qos_enable, rtk_set_qos_enable, rtk_get_qos_enable)

RTK_DECLARE_TEST_FUNC(qos_enable)
{
		int enable;

		enable =  1;
		CU_ASSERT_EQUAL(qos_enable_is_equal(enable),1);
		
		enable =  0;
		CU_ASSERT_EQUAL(qos_enable_is_equal(enable),1);
}

RTK_DECLARE_TEST_FUNC(qos_speed)
{
	unsigned int uplink_speed;
	unsigned downlink_speed;
	uplink_speed = downlink_speed = 0;//auto speed test
	
	CU_ASSERT_EQUAL(rtk_set_qos_speed(uplink_speed, downlink_speed),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_get_qos_speed(&uplink_speed, &downlink_speed),RTK_SUCCESS);
	CU_ASSERT_EQUAL(uplink_speed,0);
	CU_ASSERT_EQUAL(downlink_speed,0);

	uplink_speed = 512;
	downlink_speed = 1024;
	CU_ASSERT_EQUAL(rtk_set_qos_speed(uplink_speed, downlink_speed),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_get_qos_speed(&uplink_speed, &downlink_speed),RTK_SUCCESS);
	CU_ASSERT_EQUAL(uplink_speed,512);
	CU_ASSERT_EQUAL(downlink_speed,1024);
}

RTK_DECLARE_TBL_EQUAL_FUNC\
(qos_entry,RTK_IPQOS_T,rtk_add_qos_rule_entry,get_qos_rule_entry,rtk_del_qos_rule_entry)

RTK_DECLARE_TEST_FUNC(qos_entry)
{
		RTK_IPQOS_T entry;
		//mac mode
		memset((void *)&entry, '\0', sizeof(entry));
		entry.mode = QOS_RESTRICT_MAC;
		entry.mac[0] = 0x00;
		entry.mac[1] = 0x00;
		entry.mac[2] = 0x00;
		entry.mac[3] = 0x00;
		entry.mac[4] = 0x11;
		entry.mac[5] = 0x22;
		entry.bandwidth = 512;
		entry.bandwidth_downlink = 256;
		CU_ASSERT_EQUAL(qos_entry_is_equal(&entry),1);
		//ip mode
		memset((void *)&entry, '\0', sizeof(entry));
		entry.mode = QOS_RESTRICT_IP;
		entry.local_ip_start[0] = 0xc0;
		entry.local_ip_start[1] = 0xa8;
		entry.local_ip_start[2] = 0x01;
		entry.local_ip_start[3] = 0x7b;
		entry.bandwidth = 512;
		entry.bandwidth_downlink = 256;
		CU_ASSERT_EQUAL(qos_entry_is_equal(&entry),1);
}

RTK_DECLARE_TEST_FUNC(qos_modify)
{
	RTK_IPQOS_T entry, tmp_entry;
	int num = 0;
	//mac mode
	memset((void *)&entry, '\0', sizeof(entry));
	memset((void *)&tmp_entry, '\0', sizeof(tmp_entry));
	entry.mode = QOS_RESTRICT_MAC;
	entry.mac[0] = 0x00;
	entry.mac[1] = 0x00;
	entry.mac[2] = 0x00;
	entry.mac[3] = 0x00;
	entry.mac[4] = 0x11;
	entry.mac[5] = 0x22;
	entry.bandwidth = 512;
	entry.bandwidth_downlink = 256;

	CU_ASSERT_EQUAL(rtk_del_qos_rule_entry(&entry, 1),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_add_qos_rule_entry(&entry),RTK_SUCCESS);
	CU_ASSERT_EQUAL(get_qos_rule_entry(&num, &tmp_entry, 1),RTK_SUCCESS);
	CU_ASSERT_EQUAL(memcmp((void *)&entry, (void *)&tmp_entry, sizeof(entry)), 0);
	entry.bandwidth = 1024;
	entry.bandwidth_downlink = 512;
	
	CU_ASSERT_EQUAL(rtk_modify_qos_rule_entry(&entry),RTK_SUCCESS);
	CU_ASSERT_EQUAL(get_qos_rule_entry(&num, &tmp_entry, 1),RTK_SUCCESS);
	CU_ASSERT_EQUAL(memcmp((void *)&entry, (void *)&tmp_entry, sizeof(entry)), 0);		
}

RTK_DECLARE_TEST_FUNC(qos_add_imm)
{
	RTK_IPQOS_T entry;
	int num = 0;
	unsigned char * ip_addr ="192.168.1.22" ;
	//mac mode
	memset((void *)&entry, '\0', sizeof(entry));
	rtk_set_terminal_ip(ip_addr);
	entry.mode = QOS_RESTRICT_MAC|QOS_RESTRICT_MAX;
	entry.mac[0] = 0x38;
	entry.mac[1] = 0x83;
	entry.mac[2] = 0x45;
	entry.mac[3] = 0xf2;
	entry.mac[4] = 0xca;
	entry.mac[5] = 0x47;
	entry.bandwidth = 10000;
	entry.bandwidth_downlink = 20000;
	entry.enabled =1;
	
	CU_ASSERT_EQUAL(rtk_add_qos_rule_entry_immediately(&entry),RTK_SUCCESS);
		
}
RTK_DECLARE_TEST_FUNC(qos_del_imm)
{
	RTK_IPQOS_T entry;
	int num = 0;
	unsigned char * ip_addr ="192.168.1.22" ;
	//mac mode
	memset((void *)&entry, '\0', sizeof(entry));
	rtk_set_terminal_ip(ip_addr);
	entry.mode = QOS_RESTRICT_MAC|QOS_RESTRICT_MAX;
	entry.mac[0] = 0x38;
	entry.mac[1] = 0x83;
	entry.mac[2] = 0x45;
	entry.mac[3] = 0xf2;
	entry.mac[4] = 0xca;
	entry.mac[5] = 0x47;
	entry.bandwidth = 10000;
	entry.bandwidth_downlink = 20000;
	entry.enabled =1;
	
	CU_ASSERT_EQUAL(rtk_del_qos_rule_entry_immediately(&entry,0),RTK_SUCCESS);
		
}

#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)

//qos mon related
RTK_DECLARE_TEST_FUNC(qos_mon)
{
	unsigned int enabled=0,enabled2=0;
	unsigned char macAddr[6]={0},macAddr2[6]={0};
	unsigned int qosTime=0,qosTime2=0;//s

	enabled =1;
	macAddr[0] = 0x00;
	macAddr[1] = 0x00;
	macAddr[2] = 0x00;
	macAddr[3] = 0x00;
	macAddr[4] = 0x11;
	macAddr[5] = 0x22;
	qosTime =120;
	
	CU_ASSERT_EQUAL(rtk_set_qos_rule_monopoly(enabled, macAddr, qosTime),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_get_qos_rule_monopoly(&enabled2, macAddr2,&qosTime2),RTK_SUCCESS);

	CU_ASSERT_EQUAL(enabled,enabled2);
	CU_ASSERT_EQUAL((memcmp(macAddr,macAddr2,6)),0);
	CU_ASSERT_EQUAL(qosTime,qosTime2);
}
#endif

//vlan related
RTK_DECLARE_INT_EQUAL_FUNC\
(vlan_enable, rtk_set_vlan_enable, rtk_get_vlan_enable)

RTK_DECLARE_TEST_FUNC(vlan_enable)
{
		int enable;

		enable =  1;
		CU_ASSERT_EQUAL(vlan_enable_is_equal(enable),1);
}

RTK_DECLARE_TEST_FUNC(vlan_modify)
{
	RTK_VLAN_CONFIG_T entry, tmp_entry[MAX_IFACE_VLAN_CONFIG];
	int i = 0;
	memset((void *)&entry, '\0', sizeof(entry));
	memset((void *)&tmp_entry, '\0', sizeof(tmp_entry));
	strcpy(entry.netIface, "eth2");
	entry.enabled = 1;
	entry.vlanId = 10;
	entry.tagged = 1;
	
	CU_ASSERT_EQUAL(rtk_modify_vlan_config(&entry),RTK_SUCCESS);
	CU_ASSERT_EQUAL(rtk_get_vlan_config(&tmp_entry),RTK_SUCCESS);
	for (i = 0; i < MAX_IFACE_VLAN_CONFIG; i++)
	{
		if (!strncmp(entry.netIface, tmp_entry[i].netIface, strlen(entry.netIface)))
			CU_ASSERT_EQUAL(memcmp((void *)&entry, (void *)&tmp_entry[i], sizeof(entry)), 0);		
	}
}

/* disk related */
/*  */
RTK_DECLARE_TEST_FUNC(disk_format)
{
	unsigned char name[16], systemtype[16];

	memset(name, '\0', sizeof(name));
	memset(systemtype, '\0', sizeof(systemtype));
	memcpy(name, "sda1", strlen("sda1"));
	memcpy(systemtype, "fat32", strlen("fat32"));
	
	CU_ASSERT_EQUAL(rtk_disk_format(name, systemtype),RTK_SUCCESS);
}

RTK_DECLARE_TEST_FUNC(disk_get_storage_info)
{
	unsigned int number = 0, i = 0, j = 0, par_num = 0, count = 0;
	RTK_DEVICEINFO_T info[MAX_DEVICE_NUMBER];

	memset((void *)&info, '\0', sizeof(info));
	CU_ASSERT_EQUAL(rtk_get_storage_info(&number, info, MAX_DEVICE_NUMBER),RTK_SUCCESS);

	count = (number > MAX_DEVICE_NUMBER)?MAX_DEVICE_NUMBER:number;
	for (i = 0; i < count; i++)
	{
		printf("\ndevice name: %s partition number: %d \n", info[i].name, info[i].number);
		printf("total size: %lld bytes used size: %lld bytes free size: %lld bytes \n", info[i].total, info[i].used, info[i].free);
		par_num = info[i].number;
		par_num = (par_num >=MAX_LIST_PARTITION_NUMBER)?MAX_LIST_PARTITION_NUMBER:par_num;
		printf("\npartition information:\n");
		for (j = 0; j < par_num; j++)
		{
			printf("partition name: %s partition index: %d filesystem type: %d \n", info[i].partition_info[j].name, info[i].partition_info[j].index, info[i].partition_info[j].systype);
			printf("label: %s uuid: %s \n", info[i].partition_info[j].label, info[i].partition_info[j].uuid);
			printf("total size: %lld bytes used size: %lld bytes free size: %lld bytes \n", info[i].partition_info[j].total, info[i].partition_info[j].used, info[i].partition_info[j].free);
			printf("\n");
		}
		printf("\n");
	}
	//format test 
	#if 1
	for (i = 0; i < count; i++)
	{
		par_num = info[i].number;
		par_num = (par_num >=MAX_LIST_PARTITION_NUMBER)?MAX_LIST_PARTITION_NUMBER:par_num;
		for (j = 0; j < par_num; j++)
		{
			if (j == 0)
				CU_ASSERT_EQUAL(rtk_disk_format(info[i].partition_info[j].name, "fat32"),RTK_SUCCESS);
		}
	}
	#endif
}


/*lan_testcases*/
RTK_DECLARE_STRUCT_EQUAL_FUNC\
(lan_ip,struct rtk_static_config,rtk_set_lan_ip,rtk_get_lan_ip)

RTK_DECLARE_TEST_FUNC(lan_ip)
{
	struct rtk_static_config s_config;
	s_config.ip = 0xc0a801fe;
	s_config.mask = 0xffffff00;
	s_config.gw = 0xc0a801fe;
	CU_ASSERT_EQUAL(lan_ip_is_equal(&s_config),1);
}

RTK_DECLARE_STRUCT_EQUAL_FUNC\
(lan_dhcp,struct rtk_lan_dhcp_config,rtk_set_lan_dhcp,rtk_get_lan_dhcp)

RTK_DECLARE_TEST_FUNC(lan_dhcp)
{
	struct rtk_lan_dhcp_config l_config;
	memset(&l_config,0,sizeof(struct rtk_lan_dhcp_config));
	l_config.type = 2;
	l_config.start_ip = 0xc0a80101;
	l_config.end_ip = 0xc0a801fe;
	l_config.lease_time = 360;
	CU_ASSERT_EQUAL(lan_dhcp_is_equal(&l_config),1);
}

RTK_DECLARE_ARRAY_EQUAL_FUNC\
(lan_clone_mac,rtk_set_lan_clone_mac,rtk_get_lan_clone_mac)

RTK_DECLARE_TEST_FUNC(lan_clone_mac)
{
	unsigned char mac[6] = {0x00,0x00,0x11,0x23,0x45,0x55};
	CU_ASSERT_EQUAL(lan_clone_mac_is_equal(mac),1);
}

RTK_DECLARE_TBL_EQUAL_FUNC\
(lan_static_dhcp,struct rtk_static_lease,rtk_add_lan_static_dhcp,rtk_get_lan_static_dhcp,rtk_del_lan_static_dhcp)

RTK_DECLARE_TEST_FUNC(lan_static_dhcp)
{
	struct rtk_static_lease s_lease;
	memset(&s_lease,0,sizeof(struct rtk_static_lease));
	s_lease.ip = 0xc0a80101;
	s_lease.mac[0] = 0x00;
	s_lease.mac[1] = 0x11;
	s_lease.mac[2] = 0x33;
	s_lease.mac[3] = 0x44;
	s_lease.mac[4] = 0x55;
	s_lease.mac[5] = 0x99;
	CU_ASSERT_EQUAL(lan_static_dhcp_is_equal(&s_lease),1);
}

RTK_DECLARE_INT_EQUAL_FUNC\
(lan_static_dhcp_enable,rtk_set_lan_static_dhcp_enable,rtk_get_lan_static_dhcp_enable)
RTK_DECLARE_TEST_FUNC(lan_static_dhcp_enable)
{
	int enable = 1;
	CU_ASSERT_EQUAL(lan_static_dhcp_enable_is_equal(enable),1);
}
	

/*wan_testcases*/

RTK_DECLARE_INT_EQUAL_FUNC\
(wan_type,rtk_set_wan_type,rtk_get_wan_type)

RTK_DECLARE_TEST_FUNC(wan_type)
{
	int type = 1;
	CU_ASSERT_EQUAL(wan_type_is_equal(type),1);
}

RTK_DECLARE_STRUCT_EQUAL_FUNC\
(wan_static,struct rtk_static_config,rtk_set_wan_static,rtk_get_wan_static)

RTK_DECLARE_TEST_FUNC(wan_static)
{
	struct rtk_static_config s_config;
	memset(&s_config,0,sizeof(struct rtk_static_config));
	s_config.ip = 0xc0a80202;
	s_config.mask = 0xffffff00;
	s_config.gw = 0xc0a80203;
	CU_ASSERT_EQUAL(wan_static_is_equal(&s_config),1);
}

RTK_DECLARE_STRUCT_EQUAL_FUNC\
(wan_dhcp,struct rtk_dhcp_config,rtk_set_wan_dhcp,rtk_get_wan_dhcp)

RTK_DECLARE_TEST_FUNC(wan_dhcp)
{
	struct rtk_dhcp_config d_config;
	memset(&d_config,0,sizeof(d_config));
	strcpy(d_config.host_name,"wan_dhcp_server");
	CU_ASSERT_EQUAL(wan_dhcp_is_equal(&d_config),1);
}

RTK_DECLARE_STRUCT_EQUAL_FUNC\
(wan_pppoe,struct rtk_pppoe_config,rtk_set_wan_pppoe,rtk_get_wan_pppoe)

RTK_DECLARE_TEST_FUNC(wan_pppoe)
{
	struct rtk_pppoe_config p_config;
	memset(&p_config,0,sizeof(struct rtk_pppoe_config));
	strcpy(p_config.ac_name,"pppoe_service");
	p_config.ppp_config.connection_type = 0;
	strcpy(p_config.ppp_config.user_name,"abc");
	strcpy(p_config.ppp_config.passwd,"test");
	p_config.ppp_config.idle_time = 300;
	CU_ASSERT_EQUAL(wan_pppoe_is_equal(&p_config),1);
}

RTK_DECLARE_STRUCT_EQUAL_FUNC\
(wan_l2tp,struct rtk_l2tp_config,rtk_set_wan_l2tp,rtk_get_wan_l2tp)

RTK_DECLARE_TEST_FUNC(wan_l2tp)
{
	struct rtk_l2tp_config l_config;
	memset(&l_config,0,sizeof(struct rtk_l2tp_config));
	l_config.local_ip_selection = 1;
	l_config.static_config.ip = 0xc0010102;
	l_config.static_config.mask = 0xffffff00;
	l_config.static_config.gw = 0xc0010104;
	strcpy(l_config.ppp_config.user_name,"ppp");
	strcpy(l_config.ppp_config.passwd,"test123");
	l_config.ppp_config.connection_type = 1;
	l_config.ppp_config.idle_time = 3000;
	l_config.server_info_flag = 0;
	l_config.l2tp_server_info.server_ip = 0xc0a80101;
	CU_ASSERT_EQUAL(wan_l2tp_is_equal(&l_config),1);
}

RTK_DECLARE_STRUCT_EQUAL_FUNC\
(wan_pptp,struct rtk_pptp_config,rtk_set_wan_pptp,rtk_get_wan_pptp)

RTK_DECLARE_TEST_FUNC(wan_pptp)
{
	struct rtk_pptp_config p_config;
	memset(&p_config,0,sizeof(struct rtk_pptp_config));
	p_config.local_ip_selection = 1;
	p_config.static_config.ip = 0xc0a10101;
	p_config.static_config.mask = 0xffffff00;
	p_config.static_config.gw = 0xc0a10102;
	sprintf(p_config.ppp_config.user_name,"pptp_user1");
	sprintf(p_config.ppp_config.passwd,"pass123");
	p_config.ppp_config.connection_type = 1;
	p_config.ppp_config.idle_time = 4800;
	p_config.server_info_flag= 1;
	sprintf(p_config.pptp_server_info.server_name,"pptp_server");
	CU_ASSERT_EQUAL(wan_pptp_is_equal(&p_config),1);
}


RTK_DECLARE_INT_EQUAL_FUNC\
(wan_mtu,rtk_set_wan_mtu,rtk_get_wan_mtu)

RTK_DECLARE_TEST_FUNC(wan_mtu)
{
	int mtu = 1500;
	CU_ASSERT_EQUAL(wan_mtu_is_equal(mtu),1);
}

RTK_DECLARE_ARRAY_EQUAL_FUNC\
(wan_clone_mac,rtk_set_wan_clone_mac,rtk_get_wan_clone_mac)

RTK_DECLARE_TEST_FUNC(wan_clone_mac)
{
	unsigned char mac[6] = {0x00,0x00,0x21,0x23,0x66,0x57};
	CU_ASSERT_EQUAL(wan_clone_mac_is_equal(mac),1);
}

RTK_DECLARE_TEST_FUNC(internet_status)
{
	int connected;
	char *status;

	CU_ASSERT_EQUAL(rtk_get_internet_status(&connected), RTK_SUCCESS);
	if(connected == 0)
		status = "connect fail";
	else if(connected == 1)
		status = "connect successful";
	else
		status = "unknown";
	
	printf("\ninternet status: %s\n", status);
}

RTK_DECLARE_TEST_FUNC(phy_port_status)
{
#if 1
	struct rtk_port_status info={0};
	char *status=NULL;
	info.port_number=2;

	CU_ASSERT_EQUAL(rtk_get_phy_port_status(&info), RTK_SUCCESS);
	if(info.port_status == LINK_UP)
		status = "linkup";
	else if(info.port_status == LINK_DOWN)
		status = "linkdown";
	else
		status = "unknown";

	printf("\nwan phy status: %s\n", status);
#endif
}

RTK_DECLARE_TEST_FUNC(link_status)
{
	RTK_NET_DEVICE_STATS info;

	CU_ASSERT_EQUAL(rtk_get_link_status(&info, "br0"), RTK_SUCCESS);
	
	printf("\nport	revpkt	sndpkt	recbytes	sndbytes	recdrop	snddrop\n");
	printf("br0	%llu	%llu	%llu	%llu	%lu	%lu\n",
		info.rx_packets, info.tx_packets, info.rx_bytes, info.tx_bytes, info.rx_dropped, info.tx_dropped);
}

RTK_DECLARE_TEST_FUNC(download_statics)
{
	RTK_DOWNLOAD_STATICS info;

	CU_ASSERT_EQUAL(rtk_get_download_statics(&info), RTK_SUCCESS);
	
	printf("\nmax		min		avg		cur		total\n");
	printf("%lu		%lu		%lu		%lu		%llu\n",
		info.max_download_speed, info.min_download_speed,
		info.avg_download_speed, info.cur_download_speed, info.download_total);
}

RTK_DECLARE_TEST_FUNC(upload_statics)
{
	RTK_UPLOAD_STATICS info;

	CU_ASSERT_EQUAL(rtk_get_upload_statics(&info), RTK_SUCCESS);
	
	printf("\nmax		min		avg		cur		total\n");
	printf("%lu		%lu		%lu		%lu		%llu\n",
		info.max_upload_speed, info.min_upload_speed,
		info.avg_upload_speed, info.cur_upload_speed, info.upload_total);
}

RTK_DECLARE_TEST_FUNC(router_info)
{
	char version[32];
	char cpu[32];

	char *p_version = version;
	char *p_cpu = cpu;

	CU_ASSERT_EQUAL(rtk_get_router_info(&p_version, &p_cpu), RTK_SUCCESS);

	printf("\nversion: %s\n", version);
	printf("cpu: %s\n", cpu);
}

RTK_DECLARE_TEST_FUNC(uptime)
{
	RTK_TIME uptime;

	CU_ASSERT_EQUAL(rtk_get_uptime(&uptime), RTK_SUCCESS);

	printf("\niptime: %dday, %dhour, %dminite, %dsecond.\n",
		uptime.day, uptime.hour, uptime.min, uptime.sec);
}

RTK_DECLARE_TEST_FUNC(firmware_info)
{
	RTK_FIRMWARE_INFO info;
	
	CU_ASSERT_EQUAL(rtk_get_firmware_info(&info), RTK_SUCCESS);

	printf("\nversion: %s\n", info.version);
	printf("buildtime: %s\n", info.buildtime);
}

RTK_DECLARE_TEST_FUNC(wlan_info)
{
	RTK_WLAN_INFO info;

	CU_ASSERT_EQUAL(rtk_get_wlan_info(&info, "wlan0"), RTK_SUCCESS);

	printf("\nwlan0 information\n");
	
	if(info.enabled == 0)
		printf("enabled: yes\n");
	else
		printf("enabled: no\n");

	if(info.mode == 0)
		printf("mode: AP\n");
	else if(info.mode == 1)
		printf("mode: Client\n");
	else if(info.mode == 2)
		printf("mode: WDS\n");
	else if(info.mode == 3)
		printf("mode: AP+WDS\n");
	else
		printf("mode: unknown\n");

	if(info.network_type == 0)
		printf("network type: Infrastructure\n");
	else if(info.network_type == 1)
		printf("network type: Ad hoc\n");
	else
		printf("network type: unknown\n");

	if(info.band == 1)
		printf("band: 2.4GHz (B)\n");
	else if(info.band == 2)
		printf("band: 2.4GHz (G)\n");
	else if(info.band == 3)
		printf("band: 2.4GHz (B+G)\n");
	else if(info.band == 4)
		printf("band: 5 GHz (A)\n");
	else if(info.band == 8)
	{
		if(info.channel_num > 14)
			printf("band: 5 GHz (N)\n");
		else
			printf("band: 2.4 GHz (N)\n");
	}
	else if(info.band == 10)
		printf("band: 2.4 GHz (G+N)\n");
	else if(info.band == 11)
		printf("band: 2.4 GHz (B+G+N)\n");
	else if(info.band == 12)
		printf("band: 5 GHz (A+N)\n");
	else if(info.band == 15)
		printf("band: 2.4GHz+5 GHz (A+B+G+N)\n");
	else if(info.band == 64)
		printf("band: 5 GHz (AC)\n");
	else if(info.band == 68)
		printf("band: 5 GHz (A+AC)\n");
	else if(info.band == 72)
		printf("band: 5 GHz (N+AC)\n");
	else if(info.band == 76)
		printf("band: 5 GHz (A+N+AC)\n");
	else
		printf("band: unknown\n");

	if(info.channel_num == 0)
		printf("channel number: auto\n");
	else
		printf("channel number: %d\n", info.channel_num);

	if(info.channel_width == 0)
		printf("channel width: 20MHz\n");
	else if(info.channel_width == 1)
		printf("channel width: 40MHz\n");
	else if(info.channel_width == 2)
		printf("channel width: 80MHz\n");
	else
		printf("channel width: unknown\n");

	if(info.encryption == 0)
		printf("encryption: disabled\n");
	else if(info.encryption == 1)
		printf("encryption: WEP\n");
	else if(info.encryption == 2)
		printf("encryption: WPA\n");
	else if(info.encryption == 3)
		printf("encryption: WPA2\n");
	else
		printf("encryption: unknown\n");

	printf("client number: %d\n", info.client_num);

	if(info.state == 0)
		printf("state: disabled\n");
	else if(info.state == 1)
		printf("state: idle\n");
	else if(info.state == 2)
		printf("state: scanning\n");
	else if(info.state == 3)
		printf("state: started\n");
	else if(info.state == 4)
		printf("state: connected\n");
	else if(info.state == 5)
		printf("state: wait for key\n");
	else
		printf("state: unknown\n");

	printf("BSSID: %02x:%02x:%02x:%02x:%02x:%02x\n",
		info.BBSID[0], info.BBSID[1], info.BBSID[2],
		info.BBSID[3], info.BBSID[4], info.BBSID[5]);

	printf("SSID: %s\n", info.SSID);
	
}

RTK_DECLARE_TEST_FUNC(lan_info)
{
	RTK_LAN_INFO info;

	CU_ASSERT_EQUAL(rtk_get_lan_info(&info), RTK_SUCCESS);

	printf("\nlan information:\n");

	if(info.status == 0)
		printf("status: Getting IP from DHCP server\n");
	else if(info.status == 1)
		printf("status: DHCP\n");
	else if(info.status == 2)
		printf("status: Fixed IP\n");
	else
		printf("status: unknown\n");
	
	printf("ip: %s\n", inet_ntoa(info.ip));
	printf("mask: %s\n", inet_ntoa(info.mask));
	printf("default gateway: %s\n", inet_ntoa(info.def_gateway));

	if(info.dhcp_server == 0)
		printf("dhcp server: disabled\n");
	else if(info.dhcp_server == 1)
		printf("dhcp server: enabled\n");
	else
		printf("dhcp server: auto\n");

	printf("mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
		info.mac[0], info.mac[1], info.mac[2],
		info.mac[3], info.mac[4], info.mac[5]);
}

RTK_DECLARE_TEST_FUNC(wan_info)
{
	RTK_WAN_INFO info;

	system("flash set WAN_DHCP 1");

	CU_ASSERT_EQUAL(rtk_get_wan_info(&info), RTK_SUCCESS);

	printf("\nwan information:\n");
	
	if(info.status == DISCONNECTED)
		printf("status: disconnected\n");
	else if(info.status == FIXED_IP_CONNECTED)
		printf("status: fixed IP connected\n");
	else if(info.status == FIXED_IP_DISCONNECTED)
		printf("status: fixed IP disconnected\n");
	else if(info.status == GETTING_IP_FROM_DHCP_SERVER)
		printf("status: getting IP from DHCP server\n");
	else if(info.status == DHCP)
		printf("status: DHCP\n");
	else if(info.status == PPPOE_CONNECTED)
		printf("status: PPPoE connected\n");
	else if(info.status == PPPOE_DISCONNECTED)
		printf("status: PPPoE disconnected\n");
	else if(info.status == PPTP_CONNECTED)
		printf("status: PPTP connected\n");
	else if(info.status == PPTP_DISCONNECTED)
		printf("status: PPTP disconnected\n");
	else if(info.status == L2TP_CONNECTED)
		printf("status: L2TP connected\n");
	else if(info.status == L2TP_DISCONNECED)
		printf("status: L2TP disconnected\n");
	else if(info.status == USB3G_CONNECTED)
		printf("status: USB3G connected\n");
	else if(info.status == USB3G_MODEM_INIT)
		printf("status: USB3G modem Initializing\n");
	else if(info.status == USB3G_DAILING)
		printf("status: USB3G dialing\n");
	else if(info.status == USB3G_REMOVED)
		printf("status: USB3G removed\n");
	else if(info.status == USB3G_DISCONNECTED)
		printf("status: USB3G disconnected\n");
	else
		printf("status: unknown\n");

	
	printf("ip: %s\n", inet_ntoa(info.ip));
	printf("mask: %s\n", inet_ntoa(info.mask));
	printf("default gateway: %s\n", inet_ntoa(info.def_gateway));
	printf("dns1: %s\n", inet_ntoa(info.dns1));
	printf("dns2: %s\n", inet_ntoa(info.dns2));
	printf("dns3: %s\n", inet_ntoa(info.dns3));

	printf("mac: %02x:%02x:%02x:%02x:%02x:%02x\n",
		info.mac[0], info.mac[1], info.mac[2],
		info.mac[3], info.mac[4], info.mac[5]);
}
RTK_DECLARE_TEST_FUNC(sys_diagnostic)
{	
	rtk_sys_diagnostic();
}


RTK_DECLARE_TEST_FUNC(get_terminal_info)
{	
	int real_num;
	struct rtk_link_type_info infos[MAX_TERM_NUMBER];
	memset(infos,0,sizeof(struct rtk_link_type_info)*MAX_TERM_NUMBER);
	rtk_get_terminal_info(&real_num,infos,MAX_TERM_NUMBER);
	//rtk_get_terminal_info(infos);
	free(infos);
}
#ifdef CGI_SUPPORT
RTK_DECLARE_TEST_FUNC(nas_login)
{	
	int result=RTK_FAILED;
	result=rtk_nas_login("admin","Realtek");
	printf("\nrtk_nas_login result=%d\n",result);
}
RTK_DECLARE_TEST_FUNC(get_nas_info)
{	
	int result=RTK_FAILED;
	result=rtk_get_nas_info();
	printf("\nrtk_get_nas_info result=%d\n",result);
}
RTK_DECLARE_TEST_FUNC(nas_logout)
{	
	int result=RTK_FAILED;
	result=rtk_nas_logout();
	printf("\nrtk_nas_logout result=%d\n",result);
}

RTK_DECLARE_TEST_FUNC(get_cgi_disk_formatable)
{	
	int result=RTK_FAILED;
	int formatable=-1;
	result=rtk_get_cgi_disk_formatable(&formatable);
	printf("\nformatable=%d\n",formatable);
	printf("\nrtk_get_cgi_disk_formatable result=%d\n",result);
}
RTK_DECLARE_TEST_FUNC(cgi_disk_format)
{	
	int result=RTK_FAILED;
	result=rtk_cgi_disk_format();
	printf("\nrtk_cgi_disk_format result=%d\n",result);
}

RTK_DECLARE_TEST_FUNC(check_upgradable)
{	
	int result=RTK_FAILED;
	int upgradable=-1;
	result=rtk_check_upgradable(&upgradable);
	printf("\nformatable=%d\n",upgradable);
	printf("\nrtk_check_upgradable result=%d\n",result);
}
RTK_DECLARE_TEST_FUNC(upgrade)
{	
	int result=RTK_FAILED;
	result=rtk_upgrade();
	printf("\nrtk_upgrade result=%d\n",result);
}
RTK_DECLARE_TEST_FUNC(check_upgrade_result)
{	
	int result=RTK_FAILED;
	int upgrad_result=-1;
	result=rtk_check_upgrade_result(&upgrad_result);
	printf("\nformatable=%d\n",upgrad_result);
	printf("\nrtk_check_upgrade_result result=%d\n",result);
}



RTK_DECLARE_TEST_FUNC(send_le_id_pass)
{	
	int result=RTK_FAILED;
	char name[32]={"name"};
	char password[32]={"password"};
	result=rtk_send_le_id_pass(name,password);

	printf("\nrtk_check_upgrade_result result=%d\n",result);
}
RTK_DECLARE_TEST_FUNC(logout_le_id_pass)
{	
	int result=RTK_FAILED;

	result=rtk_logout_le_id_pass("name");

	printf("\nrtk_logout_le_id_pass result=%d\n",result);
}
RTK_DECLARE_TEST_FUNC(upload_log)
{	
	int result=RTK_FAILED;

	result=rtk_upload_log();

	printf("\nrtk_upload_log result=%d\n",result);
}

RTK_DECLARE_TEST_FUNC(query_software_version)
{	
	int result=RTK_FAILED;
	char version[64]={0};
	result=rtk_query_software_version(version);

	printf("\nrtk_query_software_version version=%s result=%d\n",version,result);
}

RTK_DECLARE_TEST_FUNC(is_wizard_done)
{	
	int result=RTK_FAILED;
	int is_wizard_done=2;
	result=rtk_is_wizard_done(&is_wizard_done);
	printf("\nis_wizard_done=%d\n",is_wizard_done);
	printf("\nrtk_is_wizard_done result=%d\n",result);
}

RTK_DECLARE_TEST_FUNC(rtk_common_cgi_api)
{	
	int result=RTK_FAILED,i=0;
	
	char * nameArray[]={"available","available","available","available","available","available"};
	char * nameArray1[]={"available","fs","blocks","available","fs","blocks"};
	char* outputArray[6]={0};
	for(i=0;i<6;i++)
	{
		outputArray[i]=(char*)malloc(64);
		bzero(outputArray[i],64);
	}

	result=rtk_common_cgi_api("get","nas/get/devices","",6,nameArray,outputArray);

	for(i=0;i<6;i++)
	{
		printf("%s=%s\n",nameArray[i],outputArray[i]);
		
	}	
	printf("\nrtk_common_cgi_api result=%d\n",result);

	
	result=rtk_common_cgi_api("get","nas/get/devices","",6,nameArray1,outputArray);

	for(i=0;i<6;i++)
	{
		printf("%s=%s\n",nameArray1[i],outputArray[i]);
		
	}	
	printf("\nrtk_common_cgi_api result=%d\n",result);
	for(i=0;i<6;i++)
		free(outputArray[i]);

	
}

#endif

RTK_DECLARE_TEST_FUNC(default_mac_address)
{
	unsigned char mac[6];

	CU_ASSERT_EQUAL(rtk_get_default_mac_address(mac, "wlan0-va4"), RTK_SUCCESS);

	printf("\nwlan0-va4 default mac address:\n");
	printf("%02x:%02x:%02x:%02x:%02x:%02x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}


int InitSuite()
{
	if(apmib_init()==0)
		return 1;
	return 0;
} 

int EndSuite()
{
	apmib_update(CURRENT_SETTING);
	return 0;
}


CU_TestInfo wlan_testcases[] ={
	RTK_TEST_INFO_DEF(wlan_enable),
	RTK_TEST_INFO_DEF(wlan_mode),
	RTK_TEST_INFO_DEF(wlan_ssid),
	RTK_TEST_INFO_DEF(wlan_band),
	RTK_TEST_INFO_DEF(wlan_network_type),
	RTK_TEST_INFO_DEF(wlan_channel_bandwidth),
	RTK_TEST_INFO_DEF(wlan_2ndchoff),
	RTK_TEST_INFO_DEF(wlan_channel),
	RTK_TEST_INFO_DEF(wlan_hidden_ssid),
	RTK_TEST_INFO_DEF(wlan_fixrate),
	RTK_TEST_INFO_DEF(wlan_txrestrict),
	RTK_TEST_INFO_DEF(wlan_rxrestrict),
	RTK_TEST_INFO_DEF(wlan_enable_repeater),
	RTK_TEST_INFO_DEF(wlan_repeater_ssid),
	RTK_TEST_INFO_DEF(wlan_wds),
	RTK_TEST_INFO_DEF(wlan_wds_entry),
	RTK_TEST_INFO_DEF(wlan_schedule),
	RTK_TEST_INFO_DEF(wlan_schedule_entry),
	//wlan advance setting
	RTK_TEST_INFO_DEF(wlan_fragthres),
	RTK_TEST_INFO_DEF(wlan_rtsthres),
	RTK_TEST_INFO_DEF(wlan_beacon_interval),
	RTK_TEST_INFO_DEF(wlan_preamble),
	RTK_TEST_INFO_DEF(wlan_protection),
	RTK_TEST_INFO_DEF(wlan_aggregation),
	RTK_TEST_INFO_DEF(wlan_shortgi),
	RTK_TEST_INFO_DEF(wlan_isolation),
	RTK_TEST_INFO_DEF(wlan_stbc),
	RTK_TEST_INFO_DEF(wlan_ldpc),
	RTK_TEST_INFO_DEF(wlan_coexist),
	RTK_TEST_INFO_DEF(wlan_txbeamforming),
	RTK_TEST_INFO_DEF(wlan_mc2u),
	RTK_TEST_INFO_DEF(wlan_rfpower),
	//wlan security setting
	RTK_TEST_INFO_DEF(wlan_encryption),
	RTK_TEST_INFO_DEF(wlan_security_wep),
	RTK_TEST_INFO_DEF(wlan_security_wpa),
	RTK_TEST_INFO_DEF(wlan_security_wpa2),
	RTK_TEST_INFO_DEF(wlan_security_wpamix),
	//wlan access control
	RTK_TEST_INFO_DEF(wlan_acl_mode),
	RTK_TEST_INFO_DEF(wlan_acl_entry),
	CU_TEST_INFO_NULL
};
CU_TestInfo wan_testcases[]={
	RTK_TEST_INFO_DEF(wan_type),
	RTK_TEST_INFO_DEF(wan_static),
	RTK_TEST_INFO_DEF(wan_dhcp),
	RTK_TEST_INFO_DEF(wan_pppoe),
	RTK_TEST_INFO_DEF(wan_l2tp),
	RTK_TEST_INFO_DEF(wan_pptp),
	RTK_TEST_INFO_DEF(wan_mtu),
	RTK_TEST_INFO_DEF(wan_clone_mac),
	CU_TEST_INFO_NULL
};
CU_TestInfo lan_testcases[]={
	RTK_TEST_INFO_DEF(lan_ip),
	RTK_TEST_INFO_DEF(lan_dhcp),
	RTK_TEST_INFO_DEF(lan_clone_mac),
	RTK_TEST_INFO_DEF(lan_static_dhcp),
	RTK_TEST_INFO_DEF(lan_static_dhcp_enable),
	CU_TEST_INFO_NULL
};
CU_TestInfo fwall_testcases[]={
	RTK_TEST_INFO_DEF(fwall_port_fw_enable),
	RTK_TEST_INFO_DEF(fwall_port_fw),
	RTK_TEST_INFO_DEF(fwall_port_filter_enable),
	RTK_TEST_INFO_DEF(fwall_port_filter),
	RTK_TEST_INFO_DEF(fwall_ip_filter_enable),
	RTK_TEST_INFO_DEF(fwall_ip_filter),
	RTK_TEST_INFO_DEF(fwall_mac_filter_enable),
	RTK_TEST_INFO_DEF(fwall_mac_filter),
	RTK_TEST_INFO_DEF(fwall_nas_filter_enable),
	RTK_TEST_INFO_DEF(fwall_nas_filter),
	RTK_TEST_INFO_DEF(fwall_url_filter_enable),
	RTK_TEST_INFO_DEF(fwall_url_filter_mode),
	//RTK_TEST_INFO_DEF(fwall_url_filter),
	//RTK_TEST_INFO_DEF(fwall_url_filter_take_effect),
	//RTK_TEST_INFO_DEF(fwall_dmz),
	CU_TEST_INFO_NULL
};
CU_TestInfo qos_testcases[]={

	RTK_TEST_INFO_DEF(qos_enable),
	RTK_TEST_INFO_DEF(qos_speed),
	RTK_TEST_INFO_DEF(qos_entry),
	RTK_TEST_INFO_DEF(qos_modify),
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
	RTK_TEST_INFO_DEF(qos_mon),
#endif
	CU_TEST_INFO_NULL
};
CU_TestInfo qosadd_testcases[]={
	RTK_TEST_INFO_DEF(qos_add_imm),
	CU_TEST_INFO_NULL
};

CU_TestInfo qosdel_testcases[]={
	RTK_TEST_INFO_DEF(qos_del_imm),	
	CU_TEST_INFO_NULL
};

CU_TestInfo vlan_testcases[]={
	RTK_TEST_INFO_DEF(vlan_enable),
	RTK_TEST_INFO_DEF(vlan_modify),
	CU_TEST_INFO_NULL
};
CU_TestInfo disk_testcases[]={
	RTK_TEST_INFO_DEF(disk_get_storage_info),
	//RTK_TEST_INFO_DEF(disk_format),
	CU_TEST_INFO_NULL
};
CU_TestInfo misc_testcases[]={
	RTK_TEST_INFO_DEF(upnp_enable),
	CU_TEST_INFO_NULL
};
CU_TestInfo dump_testcases[]={
	RTK_TEST_INFO_DEF(upnp_list),
	RTK_TEST_INFO_DEF(wlan_sta_list),
	RTK_TEST_INFO_DEF(internet_status),
	RTK_TEST_INFO_DEF(phy_port_status),
	RTK_TEST_INFO_DEF(link_status),
	RTK_TEST_INFO_DEF(download_statics),
	RTK_TEST_INFO_DEF(upload_statics),
	RTK_TEST_INFO_DEF(router_info),
	RTK_TEST_INFO_DEF(uptime),
	RTK_TEST_INFO_DEF(firmware_info),
	RTK_TEST_INFO_DEF(wlan_info),
	RTK_TEST_INFO_DEF(lan_info),
	RTK_TEST_INFO_DEF(wan_info),
	RTK_TEST_INFO_DEF(default_mac_address),
	CU_TEST_INFO_NULL
};

CU_TestInfo management_testcases[]={
	//RTK_TEST_INFO_DEF(get_link_type_by_mac),
	RTK_TEST_INFO_DEF(get_terminal_info),
	RTK_TEST_INFO_DEF(sys_diagnostic),
	CU_TEST_INFO_NULL
};
#ifdef CGI_SUPPORT

CU_TestInfo cgi_testcases[]={
	RTK_TEST_INFO_DEF(nas_login),
	RTK_TEST_INFO_DEF(get_nas_info),
	RTK_TEST_INFO_DEF(nas_logout),
	RTK_TEST_INFO_DEF(get_cgi_disk_formatable),
	RTK_TEST_INFO_DEF(cgi_disk_format),
	RTK_TEST_INFO_DEF(check_upgradable),
	RTK_TEST_INFO_DEF(upgrade),
	RTK_TEST_INFO_DEF(check_upgrade_result),
	RTK_TEST_INFO_DEF(send_le_id_pass),
	RTK_TEST_INFO_DEF(logout_le_id_pass),
	RTK_TEST_INFO_DEF(upload_log),
	RTK_TEST_INFO_DEF(query_software_version),
	RTK_TEST_INFO_DEF(is_wizard_done),
	RTK_TEST_INFO_DEF(rtk_common_cgi_api),
	CU_TEST_INFO_NULL
};
#endif
/*
CU_TestInfo testcases[] = {
	{"Test1:", Test1},
	{"Test2:", Test2},
	CU_TEST_INFO_NULL
};
*/

CU_SuiteInfo suites[] = {
	{"Testing the wlan  api function:", InitSuite, EndSuite, wlan_testcases},
	{"Testing the wan   api function:", InitSuite, EndSuite, wan_testcases},
	{"Testing the lan   api function:", InitSuite, EndSuite, lan_testcases},
	{"Testing the fwall api function:", InitSuite, EndSuite, fwall_testcases},
	{"Testing the qos   api function:", InitSuite, EndSuite, qos_testcases},
	{"Testing the qos_add_imm  api function:", InitSuite, NULL, qosadd_testcases},
	{"Testing the qos_del_imm  api function:", InitSuite, NULL, qosdel_testcases},
	{"Testing the vlan	api function:", InitSuite, EndSuite, vlan_testcases},
	{"Testing the disk	api function:", InitSuite, EndSuite, disk_testcases},
	{"Testing the misc  api function:", InitSuite, EndSuite, misc_testcases},
	{"Testing the dump  api function:", InitSuite, EndSuite, dump_testcases},
	{"Testing the manag	api function:", InitSuite, EndSuite, management_testcases},
#ifdef CGI_SUPPORT
	{"Testing the cgi	api function:", InitSuite, EndSuite, cgi_testcases},
#endif
	CU_SUITE_INFO_NULL
};



int main(int argc,char ** agrv)
{
	sleep(1);
	if( CUE_SUCCESS != CU_initialize_registry())
	{
		sleep(1);
		return CU_get_error();
	}
	sleep(1);
	assert(NULL != CU_get_registry());
	assert(!CU_is_test_running());
	sleep(1);
	if(CUE_SUCCESS != CU_register_suites(suites))
	{
		CU_cleanup_registry();
		return CU_get_error();
	} 
	sleep(1);
	CU_console_run_tests();

	/***使用自动产生XML文件的模式********
		CU_set_output_filename("TestMax");
   		CU_list_tests_to_file();
		CU_automated_run_tests();
	***********************************/
	CU_cleanup_registry();
	
	return 0;
}

