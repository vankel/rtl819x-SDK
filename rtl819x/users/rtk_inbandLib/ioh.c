/*
 * IOH helper functions
 * Copyright (C)2010, Realtek Semiconductor Corp. All rights reserved
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <errno.h>

//#include <linux/if_packet.h>
#include "ioh.h"

#include <linux/if_arp.h>

#include "inband_if.h"
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
#include "../cloud/ipc/cloud_ipc.h"
#include "../cloud/ipc/cloud_inband_app.h"
#include "../cloud/ipc/local_access.h"
#include "../cloud/ipc/login_auth.h"
#include "../hcm/hcd/cmd.h"
#endif

#if defined(LETV_LOGIN_AUTH_IPC_SUPPORT)
#include <sys/shm.h>

RTK_LETV_LOGIN_INFO_Tp login_acount_info;
void *shm = NULL;
int shmid;
int shm_inited = 0;
#endif

int bin2hex(const unsigned char *bin, char *hex, const int len)
{
	int i, idx;
	char hex_char[] = "0123456789ABCDEF";

	for (i=0, idx=0; i<len; i++)
	{
		hex[idx++] = hex_char[(bin[i] & 0xf0) >> 4];
		hex[idx++] = hex_char[bin[i] & 0x0f];
	}

	hex[idx] = 0;
	return 0;
}

int hex2bin(const char *hex, unsigned char *bin, const int len)
{
	int i, idx;
	unsigned char bytes[2];

	for (i=0, idx=0; hex[i]; i++)
	{
		if (hex[i & 0x01] == 0)
			return -1; // hex length != even

		if (hex[i] >= '0' && hex[i] <= '9')
			bytes[i & 0x01] = hex[i] - '0';
		else if (hex[i] >= 'A' && hex[i] <= 'F')
			bytes[i & 0x01] = hex[i] - 'A' + 10;
		else if (hex[i] >= 'a' && hex[i] <= 'f')
			bytes[i & 0x01] = hex[i] - 'a' + 10;
		else
			return -1; // not hex

		if (i & 0x01)
		{
			if (idx >= len)
				return -1; // out of size

			bin[idx++] = (bytes[0] << 4) | bytes[1];
		}
	}

	return 0;
}

void hex_dump(void *data, int size)
{
    /* dumps size bytes of *data to stdout. Looks like:
     * [0000] 75 6E 6B 6E 6F 77 6E 20
     *                  30 FF 00 00 00 00 39 00 unknown 0.....9.
     * (in a single line of course)
     */

    unsigned char *p = data;
    unsigned char c;
    int n;
    char bytestr[4] = {0};
    char addrstr[10] = {0};
    char hexstr[ 16*3 + 5] = {0};
    char charstr[16*1 + 5] = {0};
    for(n=1;n<=size;n++) {
        if (n%16 == 1) {
            /* store address for this line */
            snprintf(addrstr, sizeof(addrstr), "%.4x",
               ((unsigned int)p-(unsigned int)data) );
        }
            
        c = *p;
        if (isalnum(c) == 0) {
            c = '.';
        }

        /* store hex str (for left side) */
        snprintf(bytestr, sizeof(bytestr), "%02X ", *p);
        strncat(hexstr, bytestr, sizeof(hexstr)-strlen(hexstr)-1);

        /* store char str (for right side) */
        snprintf(bytestr, sizeof(bytestr), "%c", c);
        strncat(charstr, bytestr, sizeof(charstr)-strlen(charstr)-1);

        if(n%16 == 0) { 
            /* line completed */
            printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
            hexstr[0] = 0;
            charstr[0] = 0;
        } else if(n%8 == 0) {
            /* half line: add whitespaces */
            strncat(hexstr, "  ", sizeof(hexstr)-strlen(hexstr)-1);
            strncat(charstr, " ", sizeof(charstr)-strlen(charstr)-1);
        }
        p++; /* next byte */
    }

    if (strlen(hexstr) > 0) {
        /* print rest of buffer if not empty */
        printf("[%4.4s]   %-50.50s  %s\n", addrstr, hexstr, charstr);
    }
}


int ioh_open(struct ioh_class *obj, char *dev, char *da,unsigned short eth_type, int debug)
{
	struct ifreq ifr;
	int ifindex;

	strcpy(obj->dev, dev);

	if (da == NULL) 
		; // da == NULL if iohd
	else
		hex2bin(da, obj->dest_mac, ETH_MAC_LEN); 
	
	obj->debug = debug;

	obj->tx_header = (void *) obj->tx_buffer;
	obj->rx_header = (void *) obj->rx_buffer;
	obj->tx_data = (unsigned char *) obj->tx_buffer + sizeof(*obj->tx_header);
	obj->rx_data = (unsigned char *) obj->rx_buffer + sizeof(*obj->rx_header);
	obj->eth_type = eth_type;
	// create raw socket
	obj->sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if (obj->sockfd == -1) 
	{
		perror("socket():");
        syslog(LOG_DEBUG, "open ioh error, socket error!");
		return -1;
	}

	if (obj->debug)
		printf("Successfully opened socket: %i\n", obj->sockfd);

	// retrieve ethernet interface index
	bzero(&ifr, sizeof(ifr));
	strncpy(ifr.ifr_name, obj->dev, IFNAMSIZ);
	if (ioctl(obj->sockfd, SIOCGIFINDEX, &ifr) == -1) 
	{
		perror("SIOCGIFINDEX");
        syslog(LOG_DEBUG, "open ioh error, ioctl error!");
		return -1;
	}
	ifindex = ifr.ifr_ifindex;
	if (obj->debug)
		printf("Successfully got interface index: %i\n", ifindex);

	// retrieve corresponding MAC
	if (ioctl(obj->sockfd, SIOCGIFHWADDR, &ifr) == -1) 
	{
        syslog(LOG_DEBUG, "open ioh error, ioctl error 2!");
		perror("SIOCGIFHWADDR");
		return -1;
	}

	memcpy(obj->src_mac, ifr.ifr_hwaddr.sa_data, sizeof(obj->src_mac));
	if (obj->debug)
	{
		printf("Successfully got our MAC address: %02X:%02X:%02X:%02X:%02X:%02X\n",
			obj->src_mac[0], obj->src_mac[1], obj->src_mac[2],
			obj->src_mac[3], obj->src_mac[4], obj->src_mac[5]);
	}

	// Bind our raw socket to this interface
	bzero(&obj->socket_address, sizeof(obj->socket_address));
	obj->socket_address.sll_family   = PF_PACKET;
	obj->socket_address.sll_protocol = htons(obj->eth_type);
	obj->socket_address.sll_ifindex  = ifindex;

	if((bind(obj->sockfd, (struct sockaddr *) &obj->socket_address, 
		sizeof(obj->socket_address)))== -1)
	{
		printf("Error binding socket to interface\n");
        syslog(LOG_DEBUG, "open ioh error, bind error!");
		return -1;
	}

#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
	//printf("obj->eth_type: %04x\n", obj->eth_type);
	if(0x8899 == obj->eth_type) {
		int retval = rtl_initCloudDomainSocketClient(&(obj->ds_fd), CLOUD_RTK_INBAND_TAG);
		printf("INBAND APP init domain socket(ret = %d) = %d\n", retval, obj->ds_fd);	
		if(retval != 0) {
			close(obj->sockfd);
			perror("domain socket():");
			return -1;
		}
	
#if defined(LETV_LOGIN_AUTH_IPC_SUPPORT)
		if(shm_inited == 0) {
			//printf("size of login info: %d\n", sizeof(RTK_LETV_LOGIN_INFO_T));
			shmid = shmget((key_t)LOGIN_INFO_SHM_KEY, sizeof(RTK_LETV_LOGIN_INFO_T), 0666|IPC_CREAT);
			if(shmid == -1)
			{
				fprintf(stderr, "shmget failed\n");
				return -1;
			}
	
			shm = shmat(shmid, 0, 0);
			if(shm == (void*)-1)
			{  
				fprintf(stderr, "shmat failed\n");	
				return -1;
			}  
			//printf("Share memory attached at %X\n", (int)shm);
			
			login_acount_info = (RTK_LETV_LOGIN_INFO_Tp)shm;
			shm_inited = 1;
		}
#if 0
		int i;
		for(i=0; i<MAX_LOGIN_ACOUNTS; i++) {
			printf("login_acount_info->mac[%d]: %s\n", i, login_acount_info->mac[i]);
			printf("login_acount_info->time[%d]: %d\n", i, login_acount_info->time[i]);
		}
#endif
#endif
	}
#endif

	return 0;
}

int ioh_close(struct ioh_class *obj)
{
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
	if(0x8899 == obj->eth_type) {
#if defined(LETV_LOGIN_AUTH_IPC_SUPPORT)
		if(shmdt(shm) == -1)  
		{  
			fprintf(stderr, "shmdt failed\n");	
		}  
		  
		if(shmctl(shmid, IPC_RMID, 0) == -1)  
		{  
			fprintf(stderr, "shmctl(IPC_RMID) failed\n");  
		}
#endif
		close(obj->ds_fd);
	}
#endif
	close(obj->sockfd);
	return 0;
}

int ioh_send(struct ioh_class *obj , unsigned int send_len)
{
	int sent;	// length of sent packet

	//fill pkt header
	memcpy(obj->tx_header->da, obj->dest_mac, ETH_MAC_LEN);
	memcpy(obj->tx_header->sa, obj->src_mac, ETH_MAC_LEN);
	obj->tx_header->eth_type = htons(obj->eth_type);
	
	obj->socket_address.sll_hatype   = ARPHRD_ETHER;
	obj->socket_address.sll_pkttype  = PACKET_OTHERHOST;
	obj->socket_address.sll_halen    = ETH_ALEN;
	obj->socket_address.sll_addr[0]  = obj->dest_mac[0];
	obj->socket_address.sll_addr[1]  = obj->dest_mac[1];
	obj->socket_address.sll_addr[2]  = obj->dest_mac[2];
	obj->socket_address.sll_addr[3]  = obj->dest_mac[3];
	obj->socket_address.sll_addr[4]  = obj->dest_mac[4];
	obj->socket_address.sll_addr[5]  = obj->dest_mac[5];
	obj->socket_address.sll_addr[6]  = 0x00; 
	obj->socket_address.sll_addr[7]  = 0x00;
	
	if (obj->debug)
	{
		//printf("%s: tx len = %d\n", __FUNCTION__, send_len);			
		//hex_dump(obj->tx_buffer, send_len);
	}

	sent = sendto(obj->sockfd, obj->tx_buffer, 
		send_len, 0, 
		(struct sockaddr*) &obj->socket_address, sizeof(obj->socket_address));

	if (sent < 0) 
	{
		perror("sendto():");
		return -1;
	}

	return sent;
}

int check_rcv_header(struct ioh_class *obj,int rx_len)
{	
	if (rx_len < 0)
	{	
		perror("check_rcv_header: rx_len<0");
		return -1;
       }	

	/*   obj->rx_header->rrcp_type != RRCP_P_IOH) //mark_inband
		return -1;*/ 

	if (obj->rx_header->eth_type != ntohs(obj->eth_type) ){
		perror("check_rcv_header: eth_type wrong");
		return -2;
	}
#if 0 //mark_inband
	if (rx_len != ntohs(obj->rx_header->ioh_data_len) + sizeof(*obj->rx_header))  
	{
		if (ntohs(obj->rx_header->ioh_data_len) + sizeof(*obj->rx_header) < 
			ETH_MIN_FRAME_LEN && rx_len == ETH_MIN_FRAME_LEN)
			{
				// its ok for min ethernet packet padding
			}
			else
			{
				printf("%s: rx len (%d) != %d\n", __FUNCTION__,
					rx_len, ntohs(obj->rx_header->ioh_data_len) + sizeof(*obj->rx_header));
				return -1;
			}
	}
#endif

	return rx_len;

}

#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
#if defined(CLOUD_INBAND_DEBUG_FLAG)
static void rtl_dumpMessage(unsigned char *name, unsigned char *buff, int length)
{
	int i = 0;

	if (!buff || !name)
		return;

	printf("----start---- %s length=%d:\n", name, length);
	for (i = 0; i < length; i++)
	{
		if (i%16 == 0)
			printf("\n %d ", i/16);
		printf("%02x ", buff[i]);
	}
	printf("\n----end---- \n");
}
#endif

int rtl_get_uptime()
{
    unsigned char tmpBuff[64];
    unsigned char *ptr;
    FILE *fp = fopen("/proc/uptime", "r");
    if(fp==NULL){
        printf("Error: read /proc/uptime error!\n");
        syslog(LOG_DEBUG, "get uptime error!");
        return -1;
    }

	memset(tmpBuff, 0, sizeof(tmpBuff));
	fgets(tmpBuff, sizeof(tmpBuff), fp);
	ptr = strstr(tmpBuff, ".");

    memset(ptr, 0x0, 64-(ptr-tmpBuff));
    fclose(fp);

    return atoi(tmpBuff);
}

struct inband_cmd_id_table_t {
	char cmd_id;
	const char *cmd;	// command string	
};

struct inband_cmd_id_table_t inband_cmd_id_table[] =
{
    //8198 config command	
    {id_set_mib, "set_mib"},
    {id_get_mib, "get_mib"},
    {id_sysinit, "sysinit"},
    {id_syscall, "syscmd"},
    {id_send_config_file, "send_config_file"},
    {id_get_config_file, "get_config_file"},
    {id_get_file, "get_file"},
    {id_trigger_wps, "trigger_wps"}, 
    {id_getstainfo, "getstainfo"},
    {id_getassostanum, "getassostanum"},
    {id_getbssinfo, "getbssinfo"},
    {id_getstats, "getstats"},
    {id_getlanstatus, "getlanstatus"},    
  	{id_getlanRate, "getlanRate"},    
  	{id_setlanBandwidth, "setlanBandwidth"},	  
  	{id_firm_upgrade, "firm_upgrade"}, 
    {id_request_scan, "request_scan"},
	{id_get_scan_result, "get_scan_result"},
#if defined(CONFIG_RTL_LITE_CLNT)
	{id_start_lite_clnt_connect, "start_lite_clnt_connect"},
	{id_wlan_sync, "wlan_sync"},
	{id_set_wlan_on, "set_wlan_on"},
	{id_set_wlan_off, "set_wlan_off"},
#endif
#if defined(CONFIG_APP_ADAPTER_API)
    {id_get_storage_info, "get_storage_info"},
    {id_format_partition, "format_partition"},
	{id_get_wan_status, "getWanStatusByUrl"},
	{id_get_lan_terminal_info, "getLanStatus"},	
	{id_get_upload_speed, "getUploadSpeed"},		
	{id_get_download_speed, "getDownloadSpeed"},		
	{id_get_phy_port_status, "getPhyPortStatus"},	
	{id_get_lan_drop_rate, "getLanDropRate"}, 	
	{id_get_wan_drop_rate, "getWanDropRate"},
	{id_get_wlan_drop_rate, "getWlanDropRate"},
	{id_get_fw_version, "getFwVersion"},
#if defined (CONFIG_RTL_QOS_MONOPOLY_SUPPORT)
	{id_get_qos_rule_monopoly, "getQosMnpRule"},
	{id_set_qos_rule_monopoly, "setQosMnpRule"},
	{id_set_qos_rule_monopoly_imm, "setQosMnpRuleImm"},
#endif	
#if defined(QOS_BY_BANDWIDTH)
	{id_get_qos_rule, "getQosRule"},
	{id_add_qos_rule_imm, "addQosRuleImm"},
	{id_del_qos_rule_imm, "delQosRuleImm"},
#endif
	{id_get_pppoe_err_code, "getPppoeErrCode"},
	{id_restart_wan, "restartWan"},
	{id_wlan_immediately_work, "wlanImmediatelyWork"},

#if CONFIG_RTK_NAS_FILTER
    {id_enable_nas_filter, "enableNasFilter"},
    {id_get_status_nas_filter, "getNasFilterStatus"},
    {id_add_nas_filter, "addNasFilterEntry"},
    {id_del_nas_filter, "delNasFilterEntry"},
    {id_get_nas_filter_entry, "getNasFilterEntry"},
#endif
	{id_get_macfilter_rule,"getMacfilterRule"},
	{id_add_macfilter_rule_imm,"addMacfilterRuleImm" },
	{id_del_macfilter_rule_imm,"delMacfilterRuleImm"},
	
#endif
};

int rtl_domain_inband_cmd_send(unsigned char *sendbuf, unsigned char *recvbuf, 
												struct ioh_class *obj, int *len, int org_len)
{
	int ret, j, i = 0;
	cloud_tlv *ptlv = NULL;
	int fixed_len = sizeof(int) + sizeof(int);
	inband_cmd *inband = NULL;
	unsigned char *ptr1, *ptr2_pre, *ptr2, *ptr3, *ptr4;
    unsigned char *ptr0;
	char argv1[32], argv2[256], mac_buf[MAC_STR_LEN+1];
	int argv_len, cmd_len;
	unsigned char tmp_mac[MAC_STR_LEN+1];
	memset(tmp_mac,'\0',sizeof(tmp_mac));
	memset(argv1,0x0,sizeof(argv1));
	memset(argv2,0x0,sizeof(argv2));
	//safety check
	if (!sendbuf || !recvbuf || !len)
		return -1; 

	inband = (inband_cmd *)recvbuf;
    //#if defined(CLOUD_INBAND_DEBUG_FLAG)

    #if 1
    memcpy(mac_buf, inband->mac, MAC_STR_LEN);
    mac_buf[MAC_STR_LEN] = '\0';
//	printf("[%s %d] mac_buf: %s\n", __FUNCTION__, __LINE__, mac_buf);
//	printf("[%s %d] inband->argc=%d inband->argv=%s\n", 
//			__FUNCTION__, __LINE__, inband->argc, inband->argv);
	syslog(LOG_INFO, "mac_buf: %s\n", mac_buf);
	syslog(LOG_INFO, "inband->argc=%d inband->argv=%s\n", inband->argc, inband->argv);
    #endif

#if defined(LETV_LOGIN_AUTH_IPC_SUPPORT)
	FILE *fp;
	char tmp_buf[32] = {0};
	int passwd_change_time;
	int index, time_elapsed, auth_ret = 0;
	INBAND_TLV_T inband_tlv;
	unsigned char *ret_send_buf;

	passwd_change_time = 0;
	fp = fopen(PASSWD_CHANGE_NOTIFY_FILE, "r");
	if(fp) {
		memset(tmp_buf, 0, sizeof(tmp_buf));
		fgets(tmp_buf, sizeof(tmp_buf), fp);
		passwd_change_time = atoi(tmp_buf);
		//printf("passwd_change_time: %d\n", passwd_change_time);
		fclose(fp);
	}

	ret_send_buf = &inband_tlv;

	if(passwd_change_time > login_acount_info->lpct) { //login password has been changed
		printf("User password has been changed, please login again.\n");
		syslog(LOG_INFO, "User password has been changed, please login again.\n");
		//login_acount_info->lpct = passwd_change_time;  //Refresh password change time, but not here, do this only if login success

		inband_tlv.inner_tag = CLOUD_INBAND_PASSWD_CHANGED;
		auth_ret = -1;
		goto auth_fail_ret;
	}

	for(i=0; i<MAX_LOGIN_ACOUNTS; i++) {
		for(j=0;j<MAC_STR_LEN;j++){//to upper/lower
			tmp_mac[j] = login_acount_info->mac[i][j]; 
			if('a' <= tmp_mac[j] && tmp_mac[j]  <= 'f')
				tmp_mac[j] -= 32;
			else if('A' <= tmp_mac[j] && tmp_mac[j]  <= 'F')
				tmp_mac[j] += 32;
		}
		if((memcmp(login_acount_info->mac[i], inband->mac, MAC_STR_LEN) == 0) ||
		   (memcmp(tmp_mac, inband->mac, MAC_STR_LEN) == 0)) { // MAC match ok
			time_elapsed = rtl_get_uptime()-login_acount_info->time[i];
			//printf("time_elapsed: %d\n", time_elapsed);
			if(time_elapsed > MAX_LOGIN_KICK_TIME) {
				printf("[%s %d] No operation timeout!\n", __FUNCTION__, __LINE__);
				syslog(LOG_INFO, "No operation timeout!\n", __FUNCTION__, __LINE__);
				inband_tlv.inner_tag = CLOUD_INBAND_EXPIRED;
				auth_ret = -1;
				break;
			}
			index = i;
			break;
		}
	}
	if(MAX_LOGIN_ACOUNTS == i) { // not find a matched MAC
		printf("[%s %d] Not authorized user!\n", __FUNCTION__, __LINE__);
		syslog(LOG_INFO, "[%s %d] Not authorized user!\n", __FUNCTION__, __LINE__);
		inband_tlv.inner_tag = CLOUD_INBAND_NOT_AUTHORIZED;
		auth_ret = -1;
		goto auth_fail_ret;
	}

auth_fail_ret:
	if(auth_ret != 0) {
		inband_tlv.magic = obj->local_magic;
		inband_tlv.start_tag = CLOUD_RTK_INBAND_TAG;
		inband_tlv.start_len = 17;
		inband_tlv.local_tag = CLOUD_INBAND_DEVICE_TYPE;
		inband_tlv.local_len = 1;
		inband_tlv.local_val = CLOUD_INBAND_DEVICE_LOCAL;
		inband_tlv.inner_len = 0;
		ret = rtl_domainSocketWrite(obj->ds_fd, ret_send_buf, sizeof(INBAND_TLV_T));
		return -1;;
	}
#endif

	argv_len = strlen(inband->argv);
	//printf("%s %d argv_len=%d \n", __FUNCTION__, __LINE__, argv_len);

	ptr0 = strstr(inband->argv, " ");
	if(ptr0==NULL) {
		printf("%s %d must have thread id at beginning\n", __FUNCTION__, __LINE__);
		syslog(LOG_NOTICE, "%s %d must have thread id at beginning\n", __FUNCTION__, __LINE__);
		ret = 0;
		goto out_ret; 
	}else if(ptr0 == inband->argv){
		printf("%s %d don't add blank at beginning!\n", __FUNCTION__, __LINE__);
		syslog(LOG_NOTICE, "%s %d don't add blank at beginning!\n", __FUNCTION__, __LINE__);
		ret = 0;
		goto out_ret; 
    }

    int thread_len = ptr0 - (inband->argv);
    char *thread_id_buf = (char *)malloc(thread_len+1);
    if(thread_id_buf==NULL){
		printf("%s %d malloc error!\n", __FUNCTION__, __LINE__);
		syslog(LOG_NOTICE, "%s %d malloc error!\n", __FUNCTION__, __LINE__);
		ret = 0;
		goto out_ret; 
    }
    memset(thread_id_buf, 0x0, thread_len+1);
    memcpy(thread_id_buf, inband->argv, thread_len);
    // change thread_id to number
    obj->thread_id = atoi(thread_id_buf);
    //printf("Thread id(%s) is %d\n", thread_id_buf, obj->thread_id);

	// point to the second arg
    while(*ptr0 == ' '){
        ptr0++;
    }
    ptr1 = strstr(ptr0, " ");
	//ptr1 = strstr(inband->argv, " ");
	if(ptr1==NULL) {
		printf("[%s %d] must have argv1\n", __FUNCTION__, __LINE__);
		syslog(LOG_NOTICE, "[%s %d] must have argv1\n", __FUNCTION__, __LINE__);
		ret = 0;
		goto out_ret; 
	}
	while(*ptr1 == ' '){
		ptr1++;
	}
	//printf("%s %d ptr1=%s \n", __FUNCTION__, __LINE__, ptr1);

	// point to the third arg
	ptr2_pre = ptr2 = strstr(ptr1, " ");
	if(ptr2==NULL && *ptr1=='\0') {
        // not enough args
		printf("%s %d must have argv2\n", __FUNCTION__, __LINE__);
		syslog(LOG_NOTICE, "%s %d must have argv2\n", __FUNCTION__, __LINE__);
		ret = 0;
		goto out_ret; 
	}else if(ptr2==NULL){
        // 2 arguments totally
        int argv1_len  = argv_len - (ptr1-inband->argv);
        if((argv1_len+1)>sizeof(argv1)){
            printf("%s %d argv1 too long\n", __FUNCTION__, __LINE__);
			syslog(LOG_NOTICE, "%s %d argv1 too long\n", __FUNCTION__, __LINE__);
            ret = 0;
            goto out_ret; 
        }
        memcpy(argv1, ptr1, argv1_len);
        argv1[argv_len] = '\0';
        //printf("[%s:%d] argv1(%d bytes)=%s\n", __FUNCTION__, __LINE__, argv1_len, argv1);

        i = 0;
        while (inband_cmd_id_table[i].cmd != NULL) {
            //printf("%s %d cmd=%s \n", __FUNCTION__, __LINE__, inband_cmd_id_table[i].cmd);
            if (0 == strcmp(argv1, inband_cmd_id_table[i].cmd)) 
            {
                obj->inband_cmd = inband_cmd_id_table[i].cmd_id;
                break;
            }
            i++;
        }
        obj->inband_data_len = 0;
        memset(obj->rx_buffer, 0x0, sizeof(obj->rx_buffer));
        ret = 1;
    }else{
        // more than 2 arguments
        while(*ptr2 == ' '){
            ptr2++;
        }
        //printf("%s %d ptr2=%s \n", __FUNCTION__, __LINE__, ptr2);

        // get the second arg
        if((ptr2-ptr1)>sizeof(argv1)) {
            printf("%s %d argv1 too long\n", __FUNCTION__, __LINE__);
			syslog(LOG_NOTICE, "%s %d argv1 too long\n", __FUNCTION__, __LINE__);
            ret = 0;
            goto out_ret; 
        }
        memcpy(argv1, ptr1, ptr2_pre-ptr1);
        argv1[ptr2_pre-ptr1] = '\0';
        //printf("%s %d argv1=%s \n", __FUNCTION__, __LINE__, argv1);

        // find the arg id
        i = 0;
        while (inband_cmd_id_table[i].cmd != NULL) {
            //printf("%s %d cmd=%s \n", __FUNCTION__, __LINE__, inband_cmd_id_table[i].cmd);
            if (0 == strcmp(argv1, inband_cmd_id_table[i].cmd)) 
            {
                obj->inband_cmd = inband_cmd_id_table[i].cmd_id;
                break;
            }
            i++;
        }
        //printf("%s %d obj->inband_cmd=%d \n", __FUNCTION__, __LINE__, obj->inband_cmd);

        // get the third arg
        obj->inband_data_len = argv_len-(ptr2-inband->argv);
        if(obj->inband_data_len>(sizeof argv2)) {
            printf("%s %d argv2 too long\n", __FUNCTION__, __LINE__);
			syslog(LOG_NOTICE, "%s %d argv2 too long\n", __FUNCTION__, __LINE__);
            ret = 0;
            goto out_ret; 
        }
        memcpy(argv2, ptr2, obj->inband_data_len);
        argv2[obj->inband_data_len] = '\0';
        // if third arg is syscmd
        ptr3 = strstr(ptr2, "\'");
        if(ptr3 != NULL) {
            *ptr3++;
            ptr4 = strstr(ptr3, "\'");
            if(ptr4 != NULL) {
                obj->inband_data_len = ptr4-ptr3;
                memset(argv2, 0, sizeof(argv2));
                memcpy(argv2, ptr3, obj->inband_data_len);
            }
        }
        //printf("%s %d obj->inband_data_len=%d \n", __FUNCTION__, __LINE__, obj->inband_data_len);
        //printf("%s %d argv2=%s \n", __FUNCTION__, __LINE__, argv2);

        // set the return data
        memcpy(obj->rx_buffer, argv2, strlen(argv2));
        ret = 1;
    }

out_ret:
    //build response message
	for (i = 0; i < (fixed_len); i++)
	{
		sendbuf[org_len + i] = recvbuf[i]; //t l v
	}
	ptlv = (cloud_tlv *)(sendbuf + org_len);
	sendbuf[org_len + fixed_len] = (unsigned char)ret;
	ptlv->length =  htonl(sizeof(unsigned char));
	*len = fixed_len + sizeof(unsigned char);
	
#if defined(LETV_LOGIN_AUTH_IPC_SUPPORT)
    //printf("index: %d\n", index);
	login_acount_info->time[index] = rtl_get_uptime(); // if has operation, refresh time value
#endif

	return ret;
}
#endif

int ioh_recv(struct ioh_class *obj, int timeout_ms)
{
    fd_set rfds;
    struct timeval timeout;
    int retval;
	int rx_len;
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
	static unsigned char buf[BUF_LEN_MAX] = {0};
	static unsigned char send_buf[BUF_LEN_MAX] = {0};
	int i = 0;
	int variable_len = 0, tlv_len_tmp =  0, access_type = -1;
	int add_len = 0, start_tlv_len = 0, tlv_tag_tmp = 0;
	int total_send_len = 0;
	cloud_tlv *tlv_tmp =  NULL, *ptlv = NULL;
	local_dis_magic *local_magic_tmp = NULL;
	unsigned char *value, *pbuf, *pvalue;
	int fixed_len = sizeof(int) + sizeof(int);
	int max_fd = 0;
#endif

RESELECT:

	FD_ZERO(&rfds);
	FD_SET(obj->sockfd, &rfds);
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
	FD_SET(obj->ds_fd, &rfds);
	max_fd = (obj->ds_fd > obj->sockfd) ? obj->ds_fd : obj->sockfd;
	obj->which_fd = 1;
#endif

	if (timeout_ms < 0)
	{
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
		retval = select(max_fd + 1, &rfds, NULL, NULL, NULL);
#else
		retval = select(obj->sockfd + 1, &rfds, NULL, NULL, NULL);
#endif
	}
	else
	{
		timeout.tv_sec = timeout_ms/1000;
		timeout.tv_usec = (timeout_ms%1000) * 1000;
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
		retval = select(max_fd + 1, &rfds, NULL, NULL, &timeout);
#else
		retval = select(obj->sockfd + 1, &rfds, NULL, NULL, &timeout);
#endif
	}

	if (retval && FD_ISSET(obj->sockfd, &rfds)) 
	{
		rx_len = recvfrom(obj->sockfd, obj->rx_buffer, 
			sizeof(obj->rx_buffer), 0, NULL, NULL);		

		retval=check_rcv_header(obj,rx_len);
		if(-2 == retval)
		{
			/*if received not 8899 packets.try rx again*/
			goto RESELECT;
		}
		return retval;
	}
#if defined(IS_CLOUD_DEVICE_DOMAIN_APP_SUPPORT)
	else if (retval && FD_ISSET(obj->ds_fd, &rfds))
	{ 
		obj->which_fd = 2;
		// get tlv message from cloud device
		memset(buf, '\0', sizeof(buf));
		memset(send_buf, '\0', sizeof(send_buf));
		add_len = 0;
		total_send_len = 0;
        #if 0
		rx_len = read(obj->ds_fd, buf, BUF_LEN_MAX);
        #else
        int val_len = 0, val_len_raw = 0;
        rx_len = read(obj->ds_fd, buf, 12);     // 12=4+4+4=magic_len + tag_len + length_len
        if(rx_len==12){
            memcpy(&val_len_raw, buf+8, 4);
            // actual length for this single packet
            val_len = ntohl(val_len_raw);
        }else{
            //printf("[%s:%d]First read error, rx_len=%d!\n", __FUNCTION__,__LINE__, rx_len);
            return -1;
        }

        if((val_len+12) > BUF_LEN_MAX){
            int buf_len_max = BUF_LEN_MAX;
            printf("[%s:%d]This single packet is too big(%d bytes), can't be larger than %d bytes\n", __FUNCTION__, __LINE__, val_len+12, buf_len_max);
            return -1;
        }
        
        if(val_len > 0){
            int read_len = 0;   //track bytes that already read to buf
            while(read_len < val_len){
                rx_len = read(obj->ds_fd, buf+12+read_len, val_len-read_len);
                if(rx_len<0)
                {
                    printf("[%s:%d]Error, read failed(code %d), read_len=%d, total_val_len=%d\n", __FUNCTION__, __LINE__, rx_len, read_len, val_len);
                    return -1;
                }else if(rx_len == 0)
                    break;
                read_len += rx_len; // increase bytes that already read
            }
            if(read_len!=val_len){
                printf("[%s:%d]Second read error, read_len=%d, rx_len=%d, total_val_len=%d\n", __FUNCTION__,__LINE__, read_len, rx_len, val_len);
                return -1;
            }else
                rx_len = read_len;  // following codes are based on rx_len
        }else{
            printf("[%s:%d]Useless packet\n", __FUNCTION__, __LINE__);
            return -1;
        }
        
        #endif

		if(rx_len != 0)
		{
			#if defined(CLOUD_INBAND_DEBUG_FLAG)
			rtl_dumpMessage("receive message", buf, rx_len>BUF_LEN_MAX?BUF_LEN_MAX:rx_len);
			#endif

			//safety check
			if (rx_len < sizeof(unsigned int)){
                printf("[%s:%d]ERROR packet\n",__FUNCTION__, __LINE__);
				return -1;
				//continue;
            }
				
			local_magic_tmp = (local_dis_magic *)buf;
			if (ntohl(local_magic_tmp->magic) >= LOCAL_ACCESS_DATA_MAGIC)
			{
				obj->local_magic = local_magic_tmp->magic;
				//printf("local_magic = %x\n", obj->local_magic);
				//local access
				pbuf = buf + LOCAL_ACCESS_MAGIC_LEN;
				access_type = CLOUD_INBAND_DEVICE_LOCAL;
			}
			else
			{
				//cloud access 
				pbuf = buf;
				access_type = CLOUD_INBAND_DEVICE_REMOTE;
			}
				
			tlv_tmp = (cloud_tlv *)pbuf;
			if ((ntohl(tlv_tmp->tag) == CLOUD_RTK_INBAND_TAG))
			{
				#if defined(CLOUD_INBAND_DEBUG_FLAG)
				printf("INBAND APP receive data: \ntag=%d, length=%d\n", tlv_tmp->tag, ntohl(tlv_tmp->length));
				#endif
	            /* deal with variable value ,eg: disk capacity -->0x00 0xL 0x00 0x01 0x00 0x03 0xL 0x00 */
	            variable_len = ntohl(tlv_tmp->length);//total variable length
				//must have start tag (access type), at least fixed_len +1
				if (variable_len < (fixed_len + 1))
				{
					//The data is incomplete, must have start tlv
					printf("The data is incomplete, must have start tlv.\n");
					return -1;
					//continue;
				}
				pvalue = pbuf + fixed_len;
				//start tag check and skip.....
				if (ntohl(*(int *)(pvalue)) != CLOUD_INBAND_DEVICE_TYPE)
				{
					printf("The start tag is wrong, should be 0x%x.\n", CLOUD_INBAND_DEVICE_TYPE);
					return -1;
					//continue;
				}
				ptlv = (cloud_tlv *)pvalue;
				start_tlv_len = tlv_len_tmp = ntohl(ptlv->length);
				//skip start tlv
				pvalue = pvalue + (fixed_len + start_tlv_len);
				variable_len = variable_len -(fixed_len + start_tlv_len);
				//copy the outer tlv and start tlv back to sendbuf, modify total variable length before send.
				total_send_len = (start_tlv_len + fixed_len + fixed_len + ((access_type==CLOUD_INBAND_DEVICE_LOCAL)?LOCAL_ACCESS_MAGIC_LEN:0));
				#if defined(CLOUD_INBAND_DEBUG_FLAG)
				printf("%s %d total_send_len=%d start_tlv_len=%d fixed_len=%d\n", __FUNCTION__, __LINE__, total_send_len, start_tlv_len, fixed_len);
				#endif
				for (i= 0; i < total_send_len; i++)
				{
					send_buf[i] = buf[i];
				}
				//process inner tlv 
            	ptlv = (cloud_tlv *)pvalue;
				tlv_tag_tmp = ntohl(ptlv->tag);
				tlv_len_tmp = ntohl(ptlv->length);
				#if defined(CLOUD_INBAND_DEBUG_FLAG)
				printf("%s %d inner tlv tag=%d len=%d\n", __FUNCTION__, __LINE__, tlv_tag_tmp, tlv_len_tmp);
				#endif
				if (CLOUD_INBAND_CMD == tlv_tag_tmp)
				{
					if(rtl_domain_inband_cmd_send(send_buf, pvalue, obj, &add_len, total_send_len) == -1) {
						return -1;
					}
					//printf("%s %d obj->rx_buffer=%s \n", __FUNCTION__, __LINE__, obj->rx_buffer);
				}
				else 
				{
					//unkonwn command, change variable_len to zero??
					printf("unkonwn command!\n");
					variable_len = 0;
					return -1;
					//continue;//???fix me? next loop
				}

				//send prepare, modify outer tlv length....
				if (access_type==CLOUD_INBAND_DEVICE_LOCAL)
				{
					value = send_buf + LOCAL_ACCESS_MAGIC_LEN;
				}
				else
				{
					value = send_buf;
				}
					
				total_send_len = (add_len + 2*fixed_len + start_tlv_len);
				#if defined(CLOUD_INBAND_DEBUG_FLAG)
				printf("%s %d total_send_len=%d\n", __FUNCTION__, __LINE__, total_send_len);
				#endif
				*((int *)(value + sizeof(int))) = htonl(total_send_len);
				total_send_len += fixed_len + ((access_type==CLOUD_INBAND_DEVICE_LOCAL)?LOCAL_ACCESS_MAGIC_LEN:0);
				//pvalue = pvalue + (fixed_len + tlv_len_tmp);
				//variable_len = variable_len - (fixed_len + tlv_len_tmp);
				#if defined(CLOUD_INBAND_DEBUG_FLAG)
				printf("%s %d total_send_len = %d \n", __FUNCTION__, __LINE__, total_send_len);
				#endif

				//send back to .....
				#if defined(CLOUD_INBAND_DEBUG_FLAG)
				rtl_dumpMessage("send message", send_buf, total_send_len>BUF_LEN_MAX?BUF_LEN_MAX:total_send_len);
				#endif
				//retval = rtl_domainSocketWrite(obj->ds_fd, (void *)send_buf, total_send_len);
				#if defined(CLOUD_INBAND_DEBUG_FLAG)
				printf("%s %d retval=%d\n", __FUNCTION__, __LINE__, retval);
				#endif

				//return check_rcv_header(obj, rx_len);
				//printf("%s %d check_rcv_header=%d\n", __FUNCTION__, __LINE__, check_rcv_header(obj, rx_len));
				//printf("%s %d rx_len=%d\n", __FUNCTION__, __LINE__, rx_len);
				return rx_len;
			}
		}
	}
#endif
	else if (retval == 0)
	{ 
		if (obj->debug)
			printf("Timeout!!!\n");

		return -1;
	}
	else
	{
		perror("select():");
		if(errno == EINTR){
			goto RESELECT;
		}	
		return -1;
	}

	return -2;
}

