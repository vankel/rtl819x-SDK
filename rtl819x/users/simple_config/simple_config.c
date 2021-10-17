
#include "simple_config.h"

static int parse_argument(int argc, char *argv[], RTK_SC_CTXp pCtx)
{
	int argNum=1;
	char tmpbuf[100];
	FILE fp;
	int pid;
	unsigned char line[100];
	
	while (argNum < argc) {
		if ( !strcmp(argv[argNum], "-i")) {
			if (++argNum >= argc)
				break;
			strcpy(pCtx->sc_wlan_ifname, argv[argNum]);
			if(strcmp(pCtx->sc_wlan_ifname, "wlan0") == 0)
				strcpy(pCtx->sc_mib_prefix, "WLAN0_");
			else if(strcmp(pCtx->sc_wlan_ifname, "wlan0-vxd") == 0)
			{
				strcpy(pCtx->sc_mib_prefix, "WLAN0_VXD_");
				strcpy(pCtx->sc_mib_sync_prefix, "WLAN0_");
			}
			else if(strcmp(pCtx->sc_wlan_ifname, "wlan1") == 0)
				strcpy(pCtx->sc_mib_prefix, "WLAN1_");
			else if(strcmp(pCtx->sc_wlan_ifname, "wlan1-vxd") == 0)
			{
				strcpy(pCtx->sc_mib_prefix, "WLAN1_VXD_");
				strcpy(pCtx->sc_mib_sync_prefix, "WLAN1_");
			}
				
		}
		else if( !strcmp(argv[argNum], "-store"))
		{
			if (++argNum >= argc)
				break;

			pCtx->sc_save_profile=atoi(argv[argNum]);
		}
		else if( !strcmp(argv[argNum], "-wps"))
		{
			pCtx->sc_wps_support= 1;
			if (++argNum >= argc)
				break;
		}
		else if( !strcmp(argv[argNum], "-sync"))
		{
			pCtx->sc_sync_profile= 1;
			if (++argNum >= argc)
				break;
		}
		else if( !strcmp(argv[argNum], "-debug"))
		{
			if (++argNum >= argc)
				break;
			
			pCtx->sc_debug= atoi(argv[argNum]);
		}
		else
		{
			printf("invalid argument - %s\n", argv[argNum]);
			return -1;
		}
		argNum++;
	}

	return 0;
}

static int get_file_value(unsigned char *value)
{
	FILE *stream;
	char line[100];
	char string[64];
	unsigned char *p;
	
	stream = fopen (SC_SECURITY_FILE, "r" );
	if ( stream != NULL ) {		
		if(fgets(line, sizeof(line), stream))
		{
			sscanf(line, "%*[^:]:%[^?]",string);
			p=string;
			while(*p == ' ')
				p++;
			strcpy(value, p);
		}
		fclose(stream );
				
	}	
}

static int _is_hex(char c)
{
    return (((c >= '0') && (c <= '9')) ||
            ((c >= 'A') && (c <= 'F')) ||
            ((c >= 'a') && (c <= 'f')));
}

static int string_to_hex(char *string, unsigned char *key, int len)
{
	char tmpBuf[4];
	int idx, ii=0;
	for (idx=0; idx<len; idx+=2) {
		tmpBuf[0] = string[idx];
		tmpBuf[1] = string[idx+1];
		tmpBuf[2] = 0;
		if ( !_is_hex(tmpBuf[0]) || !_is_hex(tmpBuf[1]))
			return 0;

		key[ii++] = (unsigned char) strtol(tmpBuf, (char**)NULL, 16);
	}
	return 1;
}

static int get_sc_status(RTK_SC_CTXp pCtx)
{
	unsigned char buffer[64];
	unsigned char value[64];
	unsigned char *p;
	
	sprintf(buffer, "cat /proc/%s/mib_staconfig | grep scStatus > %s", pCtx->sc_wlan_ifname, SC_SECURITY_FILE);
	system(buffer);
	get_file_value(&value);
	pCtx->sc_status = atoi(value);

	if((pCtx->sc_status >= 10) && (pCtx->sc_control_ip == 0))
	{
		sprintf(buffer, "cat /proc/%s/mib_staconfig | grep scControlIP > %s", pCtx->sc_wlan_ifname, SC_SECURITY_FILE);
		system(buffer);
		get_file_value(&value);
		string_to_hex(value, (unsigned char *)&pCtx->sc_control_ip, 8);
		if(pCtx->sc_debug)
			printf("the control IP is %x, value is %s\n", pCtx->sc_control_ip, value);
	}
	return 0;
}

static int get_gpio_status()
{
	FILE *fp;
	char line[20];
	int ret;
	char tmpbuf;

	if ((fp = fopen("/proc/gpio", "r")) != NULL) {
		fgets(line, sizeof(line), fp);
		if (sscanf(line, "%c", &tmpbuf)) {
			if (tmpbuf == '0')
				ret = 0;
			else
				ret = 1;
		}
		else
			ret = 0;
		fclose(fp);
	}

	return ret;
}

static int Check_Wlan_isConnected(RTK_SC_CTXp pCtx)
{
	FILE *stream;
	int result=0;
	unsigned char buffer[64];

	sprintf(buffer, "/proc/%s/sta_info", pCtx->sc_wlan_ifname);
	stream = fopen ( buffer, "r" );
	if ( stream != NULL ) {		
		char *strtmp;
		char line[100];
		while (fgets(line, sizeof(line), stream))
		{
			unsigned char *p;
			strtmp = line;
			while(*strtmp == ' ')
				strtmp++;
				
			if(strstr(strtmp,"active") != 0){
				unsigned char str1[10];
						
				//-- STA info table -- (active: 1)
				sscanf(strtmp, "%*[^:]:%[^)]",str1);
						
				p = str1;
				while(*p == ' ')
					p++;										
				if(strcmp(p,"0") == 0){
					result=0;
				}else{
					result=1;		
				}										
				break;
			}
					
		}
		fclose(stream );
				
	}

	return result;
}



static int set_profile_to_flash(RTK_SC_CTXp pCtx, unsigned char *config_prefix)
{
	unsigned char buffer[128]={0};
	unsigned char value[128];
	unsigned char tmp[128];
	unsigned char *p;
	unsigned int security_type=0;
	unsigned int wsc_auth=0, wsc_enc=0;
	unsigned char wsc_psk[65] = {0};

	security_type = pCtx->sc_status - 10;

	system("flash setconf start");
	sprintf(buffer, "cat /proc/%s/mib_staconfig | grep scPassword > %s", pCtx->sc_wlan_ifname, SC_SECURITY_FILE);
	system(buffer);
	get_file_value(&value);
	value[strlen(value)-1] = '\0';
	strcpy(pCtx->sc_passwd, value);
	if(pCtx->sc_debug)
		printf("the password is %s", pCtx->sc_passwd);
	sprintf(buffer, "flash setconf %sSC_PASSWD \"%s\"", config_prefix, pCtx->sc_passwd);
	system(buffer);

	sprintf(buffer, "cat /proc/%s/mib_staconfig | grep scSSID > %s", pCtx->sc_wlan_ifname, SC_SECURITY_FILE);
	system(buffer);
	get_file_value(&value);
	value[strlen(value)-1] = '\0';
	strcpy(pCtx->sc_ssid, value);
	
	if(pCtx->sc_debug)
		printf("the SSID is %s, the strlen is %d\n", value, strlen(value));
	sprintf(buffer, "flash setconf %sSSID \"%s\"", config_prefix, pCtx->sc_ssid);
	system(buffer);

	if(strstr(config_prefix, "WLAN0_VXD"))
	{
		sprintf(buffer, "flash setconf REPEATER_SSID1 \"%s\"", pCtx->sc_ssid);
		system(buffer);
	}
	else if(strstr(config_prefix, "WLAN1_VXD"))
	{
		sprintf(buffer, "flash setconf REPEATER_SSID2 \"%s\"", pCtx->sc_ssid);
		system(buffer);
	}
	
#if defined(CONFIG_APP_WSC)
	sprintf(buffer, "flash setconf %sWSC_SSID \"%s\"", config_prefix, pCtx->sc_ssid);
	system(buffer);
#endif
	
	switch(security_type)
	{
		case 0:
			sprintf(buffer, "flash setconf %sENCRYPT 0", config_prefix);
			system(buffer);
			wsc_enc = 0;
			wsc_auth= WSC_AUTH_OPEN;
			break;
		case 1:
			sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP 1", config_prefix);
			system(buffer);
			sprintf(value, "%02x%02x%02x%02x%02x", pCtx->sc_passwd[0], pCtx->sc_passwd[1], pCtx->sc_passwd[2], pCtx->sc_passwd[3],pCtx->sc_passwd[4]);
			value[10] = 0;
			sprintf(buffer, "flash setconf %sWEP64_KEY1  %s", config_prefix, value);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 0", config_prefix);
			system(buffer);			
			sprintf(buffer, "flash setconf %sAUTH_TYPE  2", config_prefix);
			system(buffer);
			break;
		case 2:
			sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP64_KEY1	%s", config_prefix, pCtx->sc_passwd);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 1", config_prefix);
			system(buffer); 		
			sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
			system(buffer); 		
			break;
		case 3:
			sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP 2", config_prefix);
			system(buffer);
			sprintf(value, "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
				pCtx->sc_passwd[0], pCtx->sc_passwd[1], pCtx->sc_passwd[2], pCtx->sc_passwd[3],pCtx->sc_passwd[4], pCtx->sc_passwd[5], pCtx->sc_passwd[6],
				pCtx->sc_passwd[7], pCtx->sc_passwd[8],pCtx->sc_passwd[9], pCtx->sc_passwd[10], pCtx->sc_passwd[11],pCtx->sc_passwd[12]
				);
			value[26] = 0;
			sprintf(buffer, "flash setconf %sWEP128_KEY1 %s", config_prefix, value);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 0", config_prefix);
			system(buffer); 		
			sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
			system(buffer); 		
			break;
		case 4:
			sprintf(buffer, "flash setconf %sENCRYPT 1", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP128_KEY1 %s", config_prefix, pCtx->sc_passwd);
			system(buffer);
			sprintf(buffer, "flash setconf %sWEP_KEY_TYPE 1", config_prefix);
			system(buffer); 		
			sprintf(buffer, "flash setconf %sAUTH_TYPE 2", config_prefix);
			system(buffer); 		
			break;
		case 5:
			sprintf(buffer, "flash setconf %sENCRYPT 4", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 2", config_prefix);
			system(buffer);
			if(strlen(pCtx->sc_passwd)== 64)
				sprintf(buffer, "flash setconf %sPSK_FORMAT 1", config_prefix);
			else
				sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK \"%s\"", config_prefix, pCtx->sc_passwd);
			system(buffer);
			wsc_enc = WSC_ENCRYPT_AES;
			wsc_auth = WSC_AUTH_WPA2PSK;
			break;
		case 6:
			sprintf(buffer, "flash setconf %sENCRYPT 4", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA2_CIPHER_SUITE 1", config_prefix);
			system(buffer);
			if(strlen(pCtx->sc_passwd)== 64)
				sprintf(buffer, "flash setconf %sPSK_FORMAT 1", config_prefix);
			else
				sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK \"%s\"", config_prefix, pCtx->sc_passwd);
			system(buffer);
			wsc_enc = WSC_ENCRYPT_TKIP;
			wsc_auth = WSC_AUTH_WPA2PSK;
			break;
		case 7:
			sprintf(buffer, "flash setconf %sENCRYPT 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 2", config_prefix);
			system(buffer);
			if(strlen(pCtx->sc_passwd)== 64)
				sprintf(buffer, "flash setconf %sPSK_FORMAT 1", config_prefix);
			else
				sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK \"%s\"", config_prefix, pCtx->sc_passwd);
			system(buffer);
			wsc_enc = WSC_ENCRYPT_AES;
			wsc_auth = WSC_AUTH_WPAPSK;
			break;
		case 8:
			sprintf(buffer, "flash setconf %sENCRYPT 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_AUTH 2", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_CIPHER_SUITE 1", config_prefix);
			system(buffer);
			if(strlen(pCtx->sc_passwd)== 64)
				sprintf(buffer, "flash setconf %sPSK_FORMAT 1", config_prefix);
			else
				sprintf(buffer, "flash setconf %sPSK_FORMAT 0", config_prefix);
			system(buffer);
			sprintf(buffer, "flash setconf %sWPA_PSK \"%s\"", config_prefix, pCtx->sc_passwd);
			system(buffer);
			wsc_enc = WSC_ENCRYPT_TKIP;
			wsc_auth = WSC_AUTH_WPAPSK;
			break;
		default:
			sprintf(buffer, "flash setconf %sENCRYPT 0", config_prefix);
			system(buffer);
			break;
	}
	
	sprintf(buffer, "flash setconf %sSC_SAVE_PROFILE 2", config_prefix);
	system(buffer);
#if defined(CONFIG_APP_WSC)
	sprintf(buffer, "flash setconf %sWSC_ENC %d", config_prefix, wsc_enc);
	system(buffer);
	sprintf(buffer, "flash setconf %sWSC_AUTH %d", config_prefix, wsc_auth);
	system(buffer);
	if(wsc_enc >= 2)
	{
		sprintf(buffer, "flash setconf %sWSC_PSK \"%s\"", config_prefix, pCtx->sc_passwd);
		system(buffer);
	}
#endif

	pCtx->sc_save_profile = 2;
	
	system("flash setconf end");
	return 1;
	
}


int getInAddr( char *interface, ADDR_T type, void *pAddr )
{
    struct ifreq ifr;
    int skfd=0, found=0;
    struct sockaddr_in *addr;

    skfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(skfd==-1)
        return 0;

    strcpy(ifr.ifr_name, interface);
    if (ioctl(skfd, SIOCGIFFLAGS, &ifr) < 0){
        close( skfd );
        return (0);
    }
    if (type == HW_ADDR) {
        if (ioctl(skfd, SIOCGIFHWADDR, &ifr) >= 0) {
        memcpy(pAddr, &ifr.ifr_hwaddr, sizeof(struct sockaddr));
        found = 1;
    }
    }
    else if (type == IP_ADDR) {
    if (ioctl(skfd, SIOCGIFADDR, &ifr) == 0) {
        addr = ((struct sockaddr_in *)&ifr.ifr_addr);
        *((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
        found = 1;
    }
    }
    else if (type == SUBNET_MASK) {
    if (ioctl(skfd, SIOCGIFNETMASK, &ifr) >= 0) {
        addr = ((struct sockaddr_in *)&ifr.ifr_addr);
        *((struct in_addr *)pAddr) = *((struct in_addr *)&addr->sin_addr);
        found = 1;
    }
    }
    close( skfd );
    return found;

}

int init_config(RTK_SC_CTXp pCtx)
{
	
	unsigned char name[128]={0};
	unsigned char value[128]={0};
	unsigned char line[256]={0};
	unsigned char tmp[128];
	unsigned char *p;
	FILE *fp;
	
	sprintf(pCtx->sc_config_file, "/proc/%s/mib_staconfig", pCtx->sc_wlan_ifname);
	fp = fopen(pCtx->sc_config_file, "r");
	if (fp == NULL) {
		printf("read config file [%s] failed!\n", pCtx->sc_config_file);
		return -1;
	}


	while ( fgets(line, 200, fp) ) {
		if (line[0] == '#')
			continue;
		if(strstr(line, "sc")==0)
			continue;
		
		sscanf(line, "%[^:]",tmp);
		p=tmp;
		while(*p == ' ')
			p++;
		strcpy(name, p);
		
		sscanf(line, "%*[^:]:%[^?]",tmp);
		p=tmp;
		while(*p == ' ')
			p++;
		strcpy(value, p);
		if (!strcmp(name, "scDeviceName")) {
			strcpy(pCtx->sc_device_name, value);
		}
		else if (!strcmp(name, "scDeviceType")) {
			pCtx->sc_device_type = atoi(value);
		}
		else if (!strcmp(name, "scPin")) {
			strcpy(pCtx->sc_pin, value);
		}
		else if (!strcmp(name, "scDefaultPin")) {
			strcpy(pCtx->sc_default_pin, value);
		}
		else if (!strcmp(name, "scSyncVxdToRoot")) {
			pCtx->sc_sync_profile = atoi(value);
		}
		if(pCtx->sc_debug)
			printf("%s:	%s", name, value);
	}

	if(fp)
		fclose(fp);

}


int is_valid_control_pkt(unsigned char *nonce, unsigned char *digest, unsigned char *key, unsigned int key_len, RTK_SC_CTXp pCtx)
{
	unsigned char buffer[256];
	struct sockaddr hw_addr;
	unsigned char md5_digest[16];
	int len=0;
	int i=0;
	memset(buffer,0x0,256);
	MD5_CTX md5;

	memcpy(buffer, nonce, SC_NONCE_LEN);
	len+=SC_NONCE_LEN;
/*
	printf("the nonce is ");
	for(i=0; i<len; i++)
		printf("%02x", buffer[i]);
	printf("\n");
*/
	memcpy(buffer+len, key, key_len);
	len+=key_len;
/*
	printf("the buf is ");
	for(i=0; i<len; i++)
		printf("%02x", buffer[i]);
	printf("\n");
*/
	wlan_MD5_Init(&md5);
	wlan_MD5_Update(&md5,buffer,len);
	wlan_MD5_Final(md5_digest,&md5);

/*
	printf("the digest is ");
	for(i=0; i<16; i++)
		printf("%02x", md5_digest[i]);
	printf("\n");
*/
	if(memcmp(digest, md5_digest, 16) == 0)
		return 1;
	else
		return 0;
}

int is_bridge_if(RTK_SC_CTXp pCtx)
{
	unsigned char line[256]={0};
	unsigned char tmp[128]={0};
	unsigned char *p;
	FILE *fp;
	int ret;

	strcpy(tmp, pCtx->sc_wlan_ifname);
	tmp[strlen(pCtx->sc_wlan_ifname)] = ':';
	
	fp = fopen(BR_IFACE_FILE, "r");
	
	if (fp == NULL) {
		printf("read bridge interface file [%s] failed!\n", BR_IFACE_FILE);
		return -1;
	}

	while ( fgets(line, 200, fp) ) {
		if(strstr(line, tmp))
			ret= 1;
		else
			ret = 0;
	}
	fclose(fp);
	
	return ret;
}

int get_device_ip_status()
{
	FILE *stream;
	char line[100];
	int ret=0;
	char tmpbuf;
	
	stream = fopen (SC_IP_STATUS_FILE, "r" );
	if ( stream != NULL ) {		
		if(fgets(line, sizeof(line), stream))
		{
			if (sscanf(line, "%c", &tmpbuf)) {
				if (tmpbuf == '0')
					ret = SC_DHCP_GETTING_IP;
				else if (tmpbuf == '1')
					ret = SC_DHCP_STATIC_IP;
				else if (tmpbuf == '2')
					ret = SC_DHCP_GOT_IP;
			}
			else
				ret = 0;
		}
		fclose(stream );
	}	
	return ret;
}
int send_connect_ack(RTK_SC_CTXp pCtx)
{
	int i=0;
	struct ack_msg ack_msg;
	unsigned int intaddr;
	struct sockaddr hwaddr;
	unsigned char *pMacAddr;
	unsigned char *p;
	int sockfd_ack;
	struct sockaddr_in control_addr;
	int addr_len;
	struct ifreq interface;
	unsigned char ifname[16];

	memset(&ack_msg, 0, sizeof(struct ack_msg));
	
	pCtx->sc_ip_status = get_device_ip_status();

	if(is_bridge_if(pCtx))
	{
		getInAddr("br0", IP_ADDR, (void *)&intaddr);
		strcpy(ifname, "br0");
	}
	else
	{
		getInAddr(pCtx->sc_wlan_ifname, IP_ADDR, (void *)&intaddr);
		strcpy(ifname, pCtx->sc_wlan_ifname);
	}

	if(pCtx->sc_ip_status == SC_DHCP_GETTING_IP)
	{
		intaddr = 0;
		if(pCtx->sc_send_ack == SC_SUCCESS_IP)
			return 0;
	}

	if ((sockfd_ack = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket");
		exit(1);
	}

	strncpy(interface.ifr_ifrn.ifrn_name, ifname, IFNAMSIZ);
	if (setsockopt(sockfd_ack, SOL_SOCKET, SO_BINDTODEVICE,(char *)&interface, sizeof(interface)) < 0) {
		perror("setsockopt - SO_BINDTODEVICE");
	}
	

	bzero(&control_addr,sizeof(struct sockaddr_in)); 
	control_addr.sin_family = AF_INET;         		
	control_addr.sin_port = htons(ACK_DEST_PORT);     	
	control_addr.sin_addr.s_addr = pCtx->sc_control_ip;

	if ( getInAddr(pCtx->sc_wlan_ifname, HW_ADDR, (void *)&hwaddr ) )
	{
		pMacAddr = (unsigned char *)hwaddr.sa_data;
		for(i=0; i<6; i++)
			ack_msg.smac[i] = pMacAddr[i];
	}
	
	ack_msg.flag = SC_RSP_ACK;
	ack_msg.length = sizeof(struct ack_msg)-3;
	ack_msg.device_type = 0;
	ack_msg.device_ip = intaddr;
	memset(ack_msg.device_name, 0, 64);
	strcpy(ack_msg.device_name, pCtx->sc_device_name);
	
	if(pCtx->sc_debug == 2)
	{
		p=&ack_msg;
		printf("the ack from application is:\n");
		for(i=0; i<sizeof(struct ack_msg); i++)
			printf("%02x", p[i]);
		printf("\n");
	}
	
	addr_len = sizeof(struct sockaddr);
	
	for(i=0; i<10;i++)
	{
		sendto(sockfd_ack,(unsigned char *)&ack_msg,sizeof(struct ack_msg),0,(struct sockaddr *)&control_addr,addr_len);
	}

	close(sockfd_ack);
	
	return 0;
	
}

int get_random_ssid(RTK_SC_CTXp pCtx)
{
	int i=0, j=0;
	srand( (unsigned)time( NULL ) );
	for(i=0; i<16; i++)
	{
		j = rand();
		sprintf(&(pCtx->sc_ssid[i]), "%d", (j%10));
	}
	pCtx->sc_ssid[16] = '\0';
	return 0;
}


static int ConvertString(char *str, char *buffer)
{
 int i, len=strlen(str);
 int j=0;
 for (i=0; i<len; i++) {
  if (str[i] == '"' || str[i] == '\x27' || str[i] == '\x5c' || str[i] == '`' || str[i] == '\x24'){
   buffer[j]='\\';
   buffer[j+1] = str[i];
   j = j +2;
  }else{
   buffer[j] = str[i];
   j = j +1;
  }
 }
 return 0;
}

int sync_vxd_to_root(RTK_SC_CTXp pCtx)
{
	unsigned char buffer[128], ifname[16];
	unsigned int security_type;

	security_type = pCtx->sc_status-10;
	if(strstr(pCtx->sc_wlan_ifname, "wlan0-vxd"))
		strcpy(ifname, "wlan0");
	else if(strstr(pCtx->sc_wlan_ifname, "wlan1-vxd"))
		strcpy(ifname, "wlan1");
	sprintf(buffer, "ifconfig %s down", pCtx->sc_wlan_ifname);
	system(buffer);
	sprintf(buffer, "ifconfig %s down", ifname);
	system(buffer);
	sprintf(buffer, "iwpriv %s set_mib ssid=\"%s\"", ifname, pCtx->sc_ssid);
	system(buffer);
	sprintf(buffer, "iwpriv %s set_mib sc_enabled=0", pCtx->sc_wlan_ifname);
	system(buffer);

	switch(security_type)
	{
		case 0:
			sprintf(buffer, "iwpriv %s set_mib encmode=0", ifname);
			system(buffer);

			break;
		case 1:
			sprintf(buffer, "iwpriv %s set_mib encmode=1", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib wepkey1=\"%s\"", ifname, pCtx->sc_passwd);
			system(buffer);
			break;
		case 2:
			sprintf(buffer, "iwpriv %s set_mib encmode=1", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib wepkey1=\"%s\"", ifname, pCtx->sc_passwd);
			system(buffer);
			break;
		case 3:
			sprintf(buffer, "iwpriv %s set_mib encmode=5", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib wepkey1=\"%s\"", ifname, pCtx->sc_passwd);
			system(buffer);
			break;
		case 4:
			sprintf(buffer, "iwpriv %s set_mib encmode=5", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib wepkey1=\"%s\"", ifname, pCtx->sc_passwd);
			system(buffer);
			break;
		case 5:
			//WPA2 AES
			sprintf(buffer, "iwpriv %s set_mib encmode=2", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib psk_enable=2", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib wpa2_cipher=8", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib passphrase=\"%s\"", ifname, pCtx->sc_passwd);
			system(buffer);
			break;
		case 6:
			sprintf(buffer, "iwpriv %s set_mib encmode=2", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib psk_enable=2", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib wpa2_cipher=2", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib passphrase=\"%s\"", ifname, pCtx->sc_passwd);
			system(buffer);
			break;
		case 7:
			sprintf(buffer, "iwpriv %s set_mib encmode=2", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib psk_enable=1", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib wpa_cipher=8", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib passphrase=\"%s\"", ifname, pCtx->sc_passwd);
			system(buffer);
			break;
		case 8:
			sprintf(buffer, "iwpriv %s set_mib encmode=2", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib psk_enable=1", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib wpa_cipher=2", ifname);
			system(buffer);
			sprintf(buffer, "iwpriv %s set_mib passphrase=\"%s\"", ifname, pCtx->sc_passwd);
			system(buffer);
			break;
		default:
			sprintf(buffer, "iwpriv wlan0 set_mib encmode=0", ifname);
			system(buffer);
			break;
	}
	sprintf(buffer, "ifconfig %s up", ifname);
	system(buffer);
	sprintf(buffer, "ifconfig %s up", pCtx->sc_wlan_ifname);
	system(buffer);

	//don't restart simple config when up interface again.
	pCtx->sc_run_time = 0;
	

	return 0;
}

int main(int argc, char *argv[])
{
	int i=0, ret=0, start_sc=0, accept_control=0, on=1, reinit=0, wps_sc_concurrent=0;
	int sockfd_scan, sockfd_control;                     				// socket descriptors
	struct sockaddr_in device_addr;     		// my address information
	struct sockaddr_in control_addr;  			// connector¡¦s address information
	int addr_len, numbytes;
	FILE *fp;
	fd_set fds;	
	int max_fd, selret;
	unsigned char buf[256];
	RTK_SC_CTXp pCtx;
	unsigned char *p;
	struct timeval timeout; 
	struct ack_msg ack_msg;
	struct response_msg res_msg;
	//int buf_len=2048;
	int configured=0;
	int disconnect_time=0;
	
	pCtx = &g_sc_ctx;
	pCtx->sc_debug = 1;
	if(parse_argument(argc, argv, pCtx)<0)
		return 0;

	init_config(pCtx);
	
	if ((sockfd_scan = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket");
		exit(1);
	}

	//ret = setsockopt( sockfd_scan, SOL_SOCKET, SO_BROADCAST|SO_REUSEADDR, &on, sizeof(on) );
	ret = setsockopt( sockfd_scan, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );
	//ret = setsockopt( sockfd_scan, SOL_SOCKET, SO_RCVBUF, &buf_len, sizeof(int));

	bzero(&device_addr,sizeof(struct sockaddr_in)); 
	device_addr.sin_family = AF_INET;         		// host byte order
	device_addr.sin_port = htons(18864);     	// short, network byte order
	device_addr.sin_addr.s_addr = INADDR_ANY;// automatically fill with my IP

	// bind the socket with the address
	if (bind(sockfd_scan, (struct sockaddr *)&device_addr, sizeof(struct sockaddr)) == -1) 
	{
		perror("bind");
		close(sockfd_scan);
		exit(1);
	}
	
#if 1
	if ((sockfd_control = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		perror("socket");
		exit(1);
	}
	ret = setsockopt( sockfd_control, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

	bzero(&device_addr,sizeof(struct sockaddr_in)); 
	device_addr.sin_family = AF_INET;         		// host byte order
	device_addr.sin_port = htons(ACK_DEST_PORT);     	// short, network byte order
	device_addr.sin_addr.s_addr = INADDR_ANY;// automatically fill with my IP

	// bind the socket with the address
	if (bind(sockfd_control, (struct sockaddr *)&device_addr, sizeof(struct sockaddr)) == -1) 
	{
		perror("bind");
		close(sockfd_scan);
		exit(1);
	}
#endif	

	addr_len = sizeof(struct sockaddr);

	pCtx->sc_run_time=0;
	pCtx->sc_pbc_duration_time = 0;

	if(pCtx->sc_save_profile !=2 )
	{
		sprintf(buf, "iwpriv %s set_mib sc_enabled=1", pCtx->sc_wlan_ifname);
		system(buf);
	}
	else
	{
		configured = 1;
		pCtx->sc_config_success = 1;
	}

	pCtx->sc_ip_status = get_device_ip_status();

	while (1) {
		pCtx->sc_run_time++;
		pCtx->sc_wlan_status = Check_Wlan_isConnected(pCtx);
		get_sc_status(pCtx);
		/*sc_save_profile is 2 means the config info has be saved to flash.
		    if run time is 30s but the sc_wlan_status is 0, it meas DUT can't connect to the saved target AP, it should enable Simple Config now.
		    if run time is large than 30s,  the linkd time is 0, but the sc_status is connected, it means connect to target AP by Simple Config, but it can't connect to target AP now.
		*/
		if((pCtx->sc_save_profile ==2) && (pCtx->sc_wlan_status==0))
		{
			disconnect_time++;
			if((disconnect_time == 30 && pCtx->sc_status == 0) || (pCtx->sc_status >=1 && disconnect_time>120))
		
		{
			sprintf(buf, "ifconfig %s down", pCtx->sc_wlan_ifname);
			system(buf);
			sprintf(buf, "iwpriv %s set_mib sc_enabled=1", pCtx->sc_wlan_ifname);
			system(buf);
			sprintf(buf, "iwpriv %s set_mib sc_status=0", pCtx->sc_wlan_ifname);
			system(buf);
				//sprintf(buf, "flash set %s SC_SAVE_PROFILE 0", pCtx->sc_wlan_ifname);
				//system(buf);
			sprintf(buf, "ifconfig %s up", pCtx->sc_wlan_ifname);
			system(buf);
			pCtx->sc_save_profile = 0;
				disconnect_time = 0;
			}
		}
		
		if(1)//(pCtx->sc_wlan_status == 1)
		{
			timeout.tv_sec = 1;
			timeout.tv_usec = 0;
			max_fd = 0;
			FD_ZERO(&fds);
			FD_SET(sockfd_scan, &fds);
			FD_SET(sockfd_control, &fds);

			max_fd = (sockfd_control > sockfd_scan) ? sockfd_control : sockfd_scan;
			selret = select(max_fd+1, &fds, NULL, NULL, &timeout);
			
			if (selret && FD_ISSET(sockfd_scan, &fds)) 
			{
				struct scan_msg *pMsg;
				unsigned int intaddr;
				struct sockaddr hwaddr;
				unsigned char *pMacAddr;
				
				memset(buf, 0, 256);
				if ((numbytes = recvfrom(sockfd_scan, buf, 256, 0,
					(struct sockaddr *)&control_addr, &addr_len)) == -1) {
					fprintf(stderr,"Receive failed!!!\n");
					close(sockfd_scan);
					exit(1);
				}
				else if(pCtx->sc_wlan_status == 1)
				{
					control_addr.sin_port = htons(ACK_DEST_PORT); 
					pMsg = (struct scan_msg *)buf;
					switch(pMsg->flag)
					{
						case SC_SUCCESS_ACK:
							if(pCtx->sc_send_ack == SC_SUCCESS_ACK)
							{
								if(pCtx->sc_ip_status == SC_DHCP_GETTING_IP)
								{
									if(pCtx->sc_debug)
										printf("receive config success ack, but the device haven't get IP now!\n");
									pCtx->sc_send_ack = SC_SUCCESS_IP;
								}
							}
							if((pCtx->sc_send_ack == SC_SUCCESS_ACK) || (pCtx->sc_send_ack == SC_SUCCESS_IP))
							{
								if(pCtx->sc_ip_status != SC_DHCP_GETTING_IP)
								{
									if(pCtx->sc_debug)
										printf("receive config success ack\n");
									if(pCtx->sc_save_profile == 0)
										pCtx->sc_save_profile = 1;
									pCtx->sc_send_ack = 0;
								}
							}
							break;
						case SC_SCAN:
							if(pCtx->sc_save_profile != 2)
							{
								if(pCtx->sc_debug == 3)
								{
									printf("receive scan message, don't send reply before the setting is saved to flash.\n");
								}
								break;
							}
							
							if(pCtx->sc_debug == 2)
								printf("receive scan from %s\n ", inet_ntoa( control_addr.sin_addr));
							pCtx->sc_ip_status = get_device_ip_status();
							if(pCtx->sc_ip_status != SC_DHCP_GETTING_IP)
							{
								if(is_bridge_if(pCtx))
									getInAddr("br0", IP_ADDR, (void *)&intaddr);
								else
									getInAddr(pCtx->sc_wlan_ifname, IP_ADDR, (void *)&intaddr);
							}
							else
							{
								intaddr = 0;
								//don't response scan when ip is NULL.
								break;
							}

							if ( getInAddr(pCtx->sc_wlan_ifname, HW_ADDR, (void *)&hwaddr ) )
							{
								pMacAddr = (unsigned char *)hwaddr.sa_data;
								for(i=0; i<6; i++)
									ack_msg.smac[i] = pMacAddr[i];
							}

							ack_msg.flag = SC_RSP_SCAN;
							ack_msg.length = sizeof(struct ack_msg)-3;
							ack_msg.status = 1;
							if(pMsg->sec_level != 0)
							{
								if(is_valid_control_pkt(pMsg->nonce, pMsg->digest, pCtx->sc_default_pin, 8, pCtx) == 0)
								{
									ack_msg.status = 0;
									printf("the scan info is invalid!\n");
								}
							}
							ack_msg.device_type = 0;
							ack_msg.device_ip = intaddr;
							memset(ack_msg.device_name, 0, 64);
							strcpy(ack_msg.device_name, pCtx->sc_device_name);
							
							if(pCtx->sc_debug == 2)
							{
								p=&ack_msg;
								printf("the response packet for scan is:\n");
								for(i=0; i<sizeof(struct ack_msg); i++)
									printf("%02x", p[i]);
								printf("\n");
							}
							for(i=0; i<SC_ACK_ROUND;i++)
							{
								sendto(sockfd_scan,(unsigned char *)&ack_msg,sizeof(struct ack_msg),0,(struct sockaddr *)&control_addr,addr_len);
							}
							break;
						default:
							printf("invalid request\n");
							break;
					}
				}
				else
				{
					if(pCtx->sc_debug == 2)
						printf("receive scan info when wlan is disconnect. this packets is queued by socket\n");
				}
			}
			
			if (selret && FD_ISSET(sockfd_control, &fds) ) 
			{
				//receive the command from the client
				unsigned char status;
				struct request_msg *pMsg;

				memset(buf, 0, 256);
				if ((numbytes = recvfrom(sockfd_control, buf, 256, 0,
					(struct sockaddr *)&control_addr, &addr_len)) == -1 ) {
					fprintf(stderr,"Receive failed!!!\n");
					close(sockfd_control);
					exit(1);
				}
				else if(pCtx->sc_wlan_status == 1)
				{	
					control_addr.sin_port = htons(ACK_DEST_PORT); 
					pMsg = (struct request_msg *)buf;
					if(accept_control == 1)
						memset(&res_msg, 0, sizeof(struct response_msg));
					
					if(pCtx->sc_debug == 2)
					{
						printf("the receiv control buf is ", buf);
						for(i=0; i<numbytes; i++)
							printf("%02x", buf[i]);
						printf("\n");
						printf("the flag is %d, the length is %d\n",pMsg->flag, pMsg->length);
					}
					switch(pMsg->flag)
					{
						case SC_SAVE_PROFILE:
							if(accept_control == 1)
							{
								if(pCtx->sc_debug)
									printf("receive info to save profile!\n");
								res_msg.flag = SC_RSP_SAVE;
								res_msg.status = 1;
								if(pMsg->sec_level != 0)
								{
									if(is_valid_control_pkt(pMsg->nonce, pMsg->digest1, pCtx->sc_default_pin, 8, pCtx) == 0)
									{
										res_msg.status = 0;
										printf("the control info is invalid!\n");
									}
									else if(is_valid_control_pkt(pMsg->nonce, pMsg->digest2, pCtx->sc_pin, 8, pCtx) == 0)
									{
										res_msg.status = 0;
										printf("the control info is invalid!\n");
									}
								}
								if(res_msg.status != 0)
								{
									
									if(pCtx->sc_debug)
										printf("save profile to flash\n");
									accept_control = 0;

									pCtx->sc_send_ack = SC_SAVE_PROFILE;
								}
							}
							break;
						case SC_DEL_PROFILE:
							if(pCtx->sc_save_profile != 2)
								break;
							
							if(accept_control == 1)
							{
								if(pCtx->sc_debug)
									printf("receive info to remove device!\n");
								res_msg.status = 1;
								res_msg.flag = SC_RSP_DEL;
								if(pMsg->sec_level != 0)
								{
									if(is_valid_control_pkt(pMsg->nonce, pMsg->digest1, pCtx->sc_default_pin, 8, pCtx) == 0)
									{
										res_msg.status = 0;
										if(pCtx->sc_debug)
											printf("the control info is invalid!\n");
									}
									else if(is_valid_control_pkt(pMsg->nonce, pMsg->digest2, pCtx->sc_pin, 8, pCtx) == 0)
									{
										res_msg.status = 0;
										if(pCtx->sc_debug)
											printf("the control info is invalid!\n");
									}
								}
								if(res_msg.status != 0)
								{
								
									accept_control = 0;
									pCtx->sc_send_ack = SC_DEL_PROFILE;

									if(1)
										reinit = SC_REINIT_WLAN;
									else
										reinit = SC_REINIT_SYSTEM;
								}
								
							}
							break;
						case SC_RENAME:
							if(pCtx->sc_save_profile != 2)
								break;
							
							if(accept_control == 1)
							{
								if(pCtx->sc_debug)
									printf("receive info to rename device\n");
								res_msg.status = 1;
								res_msg.flag = SC_RSP_RENAME;
								if(pMsg->sec_level != 0)
								{
									if(is_valid_control_pkt(pMsg->nonce, pMsg->digest1, pCtx->sc_default_pin, 8, pCtx) == 0)
									{
										res_msg.status = 0;
										printf("the control info is invalid!\n");
									}
									else if(is_valid_control_pkt(pMsg->nonce, pMsg->digest2, pCtx->sc_pin, 8, pCtx) == 0)
									{
										res_msg.status = 0;
										printf("the control info is invalid!\n");
									}
								}
								if(res_msg.status != 0)
								{
									accept_control = 0;
									
									strcpy(pCtx->sc_device_name, pMsg->device_name);
									pCtx->sc_send_ack = SC_RENAME;
									if(pCtx->sc_debug)
										printf("the new device name is %s\n", pCtx->sc_device_name);
								}
							}
							break;
						case SC_SUCCESS_ACK:
							if((pCtx->sc_send_ack == SC_SUCCESS_ACK) || (pCtx->sc_send_ack == SC_SUCCESS_IP))
							{
								if(pCtx->sc_ip_status != SC_DHCP_GETTING_IP)
								{
									if(pCtx->sc_debug)
										printf("receive config success ack\n");
									if(pCtx->sc_save_profile == 0)
										pCtx->sc_save_profile = 1;
									pCtx->sc_send_ack = 0;
									pCtx->sc_config_success = 1;
									
									break;
								}
							}
							
							if(pMsg->sec_level != 0)
							{
								if(is_valid_control_pkt(pMsg->nonce, pMsg->digest1, pCtx->sc_default_pin, 8, pCtx) == 0)
								{
									res_msg.status = 0;
									printf("the control info is invalid!\n");
								}
								else if(is_valid_control_pkt(pMsg->nonce, pMsg->digest2, pCtx->sc_pin, 8, pCtx) == 0)
								{
									res_msg.status = 0;
									printf("the control info is invalid!\n");
								}
							}
							
							if(res_msg.status != 0)
							{
							
								if(pCtx->sc_debug == 2)
									printf("receive ack type is %d, request ack type is %d\n", pMsg->device_name[0],  pCtx->sc_send_ack);
								if(pMsg->device_name[0] == pCtx->sc_send_ack)
								{
									pCtx->sc_config_success = 1;
								}
							}

							break;							
						default:
							if(pCtx->sc_debug)
								printf("invalid request\n");
							res_msg.flag = SC_RSP_INVALID;
							status = 0;
							break;
					}
					
					if((pCtx->sc_send_ack && (pCtx->sc_config_success==0)) || (res_msg.status== 0))
					{
						res_msg.length = sizeof(struct response_msg)-3;
						p= &res_msg;
						
						if(pCtx->sc_debug == 2)
						{
							for(i=0; i<sizeof(struct response_msg); i++)
								printf("%02x", p[i]);
							printf("\n");
						}
						for(i=0; i<SC_ACK_ROUND;i++)
						{
							sendto(sockfd_control,(unsigned char *)&res_msg,sizeof(struct response_msg),0,(struct sockaddr *)&control_addr,addr_len);
						}
					}

				}
				else
				{
					if(pCtx->sc_debug == 2)
						printf("receive control info when wlan is disconnect. this packets is queued by socket\n");
				}
			}
			else
			{
				accept_control = 1;
				if(pCtx->sc_config_success == 1)
				{
					switch(pCtx->sc_send_ack)
					{
						case SC_SAVE_PROFILE:
							set_profile_to_flash(pCtx, pCtx->sc_mib_prefix);
							if(strstr(pCtx->sc_wlan_ifname, "vxd") && (pCtx->sc_sync_profile))
							{
								set_profile_to_flash(pCtx, pCtx->sc_mib_sync_prefix);
							}
							break;								
						case SC_DEL_PROFILE:
							pCtx->sc_ip_status = get_device_ip_status();
							if(pCtx->sc_ip_status != SC_DHCP_STATIC_IP)
							{
								pCtx->sc_ip_status = SC_DHCP_GETTING_IP;
								sprintf(buf, "echo 0 > %s", SC_IP_STATUS_FILE);
								system(buf);
								if(pCtx->sc_debug)
									printf("set sc ip status to getting IP!\n");
							}
							get_random_ssid(pCtx);
							system("flash setconf start");
							sprintf(buf, "flash setconf %sSSID %s", pCtx->sc_mib_prefix, pCtx->sc_ssid);
							system(buf);
							sprintf(buf, "flash setconf %sENCRYPT 0", pCtx->sc_mib_prefix);
							system(buf);
							sprintf(buf, "flash setconf %sSC_SAVE_PROFILE 0", pCtx->sc_mib_prefix);
							system(buf);
							system("flash setconf end");
							sprintf(buf, "iwpriv %s set_mib sc_pin_enabled=1", pCtx->sc_wlan_ifname);
							system(buf);
							sprintf(buf, "iwpriv %s set_mib ssid=%s", pCtx->sc_wlan_ifname, pCtx->sc_ssid);
							system(buf);
							sprintf(buf, "iwpriv %s set_mib sc_status=0", pCtx->sc_wlan_ifname);
							system(buf);
							break;
						case SC_RENAME:
							sprintf(buf, "flash set SC_DEVICE_NAME \"%s\"", pCtx->sc_device_name);
							system(buf);
							sprintf(buf, "iwpriv %s set_mib sc_device_name=\"%s\"", pCtx->sc_wlan_ifname, pCtx->sc_device_name);
							system(buf);
							break;
						
					}
					
					if(reinit == SC_REINIT_SYSTEM)
					{
						reinit = 0;
						close(sockfd_scan);
						close(sockfd_control);
						system("init.sh gw all");
					}
					else if(reinit == SC_REINIT_WLAN)
					{
						pCtx->sc_save_profile = 0;
						configured = 0;
						pCtx->sc_ip_status = get_device_ip_status();
						if(pCtx->sc_ip_status != SC_DHCP_STATIC_IP)
						{
							system("killall udhcpc");
						}
						sprintf(buf, "ifconfig %s down", pCtx->sc_wlan_ifname);
						system(buf);
						sprintf(buf, "iwpriv %s set_mib sc_enabled=1", pCtx->sc_wlan_ifname);
						system(buf);
						sprintf(buf, "ifconfig %s up", pCtx->sc_wlan_ifname);
						system(buf);
						pCtx->sc_status = 0;
						if(pCtx->sc_ip_status != SC_DHCP_STATIC_IP)
						{
							system("udhcpc -i br0 -p /etc/udhcpc/udhcpc-br0.pid -s /usr/share/udhcpc/br0.sh");
						}
					}

					pCtx->sc_send_ack = 0;
					pCtx->sc_config_success= 0;
					reinit = 0;
				}

			}
		}
		else
		{
			sleep(1);
		}

		if(pCtx->sc_wlan_status== 0)
		{
			pCtx->sc_linked_time= 0;
			pCtx->sc_send_ack = 0;
			pCtx->sc_control_ip = 0;
		}
		else
		{
			disconnect_time = 0;
			pCtx->sc_linked_time++;
			if(pCtx->sc_linked_time >= 30 && pCtx->sc_save_profile == 0)
			{
				if(configured == 1)
				{
					pCtx->sc_save_profile = 2;
				}
			}
			
			if( (pCtx->sc_status >= 10))
			{
				if(pCtx->sc_linked_time == 1 && pCtx->sc_save_profile == 0)
				{
					reinit= 0;
					//pCtx->sc_save_profile = 0;
					pCtx->sc_send_ack = SC_SUCCESS_ACK;
					pCtx->sc_config_success = 0;
#if 0
					pCtx->sc_ip_status = get_device_ip_status();
					if(pCtx->sc_ip_status != SC_DHCP_STATIC_IP)
					{
						system("killall udhcpc");
						system("udhcpc -i br0 -p /etc/udhcpc/udhcpc-br0.pid -s /usr/share/udhcpc/br0.sh");
					}
#endif
				}

				
				
				if(pCtx->sc_save_profile==1)
				{
					set_profile_to_flash(pCtx, pCtx->sc_mib_prefix);
					if(pCtx->sc_sync_profile && strstr(pCtx->sc_wlan_ifname, "vxd"))
					{
						printf("sync vxd setting to root interface now\n");
						set_profile_to_flash(pCtx, pCtx->sc_mib_sync_prefix);
						if(pCtx->sc_sync_profile == 2)
							sync_vxd_to_root(pCtx);
					}
					pCtx->sc_save_profile = 2;
				}
				if((pCtx->sc_send_ack == SC_SUCCESS_ACK) || (pCtx->sc_send_ack == SC_SUCCESS_IP))
					send_connect_ack(pCtx);
			}
			
			pCtx->sc_pbc_duration_time = 0;
		}

#if defined(SIMPLE_CONFIG_PBC_SUPPORT)			
		if(1)//(g_sc_connect_status == 0)
		{
			ret = get_gpio_status();
			if(ret == 1)
			{
			
				if(pCtx->sc_debug)
					printf("GPIO is Pressed\n");
				system("echo 0 > /proc/gpio");
				pCtx->sc_pbc_pressed_time++;
				pCtx->sc_pbc_duration_time = 0;

			}
			if((ret == 0) &&(pCtx->sc_pbc_pressed_time>0))
			{
#if defined(CONFIG_RTL_SIMPLE_CONFIG_USE_WPS_BUTTON)
				if(pCtx->sc_pbc_pressed_time>=WPS_BUTTON_HOLD_TIME)
					wps_sc_concurrent = 1;
				else 
					wps_sc_concurrent = 0;
#endif
				pCtx->sc_pbc_pressed_time = 0;
				//if(start_sc ==0)
				{
					
					if(pCtx->sc_debug)
						printf("start RTK simple config now\n");
					pCtx->sc_linked_time = 0; //for LED
					pCtx->sc_pbc_duration_time = 1;
					
					pCtx->sc_ip_status = get_device_ip_status();
					if(pCtx->sc_debug)
						printf("the ip status is %d\n", pCtx->sc_ip_status);
					if(pCtx->sc_ip_status != SC_DHCP_STATIC_IP)
					{
						pCtx->sc_ip_status = SC_DHCP_GETTING_IP;
						sprintf(buf, "echo 0 > %s", SC_IP_STATUS_FILE);
						system(buf);
					}
					get_random_ssid(pCtx);
					system("flash setconf start");
					sprintf(buf, "flash setconf %sSSID %s", pCtx->sc_mib_prefix, pCtx->sc_ssid);
					system(buf);
					sprintf(buf, "flash setconf %sENCRYPT 0", pCtx->sc_mib_prefix);
					system(buf);
					sprintf(buf, "flash setconf %sSC_SAVE_PROFILE 0", pCtx->sc_mib_prefix);
					system(buf);
					system("flash setconf end");
					sprintf(buf, "ifconfig %s down", pCtx->sc_wlan_ifname);
					system(buf);
					sprintf(buf, "iwpriv %s set_mib sc_pin_enabled=0", pCtx->sc_wlan_ifname);
					system(buf);
					sprintf(buf, "iwpriv %s set_mib ssid=%s", pCtx->sc_wlan_ifname, pCtx->sc_ssid);
					system(buf);
					sprintf(buf, "iwpriv %s set_mib sc_status=0", pCtx->sc_wlan_ifname);
					system(buf);
					sprintf(buf, "iwpriv %s set_mib sc_enabled=1", pCtx->sc_wlan_ifname);
					system(buf);
					sprintf(buf, "ifconfig %s up", pCtx->sc_wlan_ifname);
					system(buf);
					pCtx->sc_status = 0;
					pCtx->sc_save_profile = 0;
					configured = 0;
#if 0					
					system("echo 1 > /proc/gpio");
					sleep(1);
					sprintf(buf, "iwpriv %s set_mib sc_enabled=1", pCtx->sc_wlan_ifname);
					system(buf);
					system("echo 0 > /proc/gpio");
					sleep(1);
					system("echo 1 > /proc/gpio");
#endif
				}
			}

			if(pCtx->sc_status != 0)
			{
				if(pCtx->sc_pbc_duration_time > 0)
				{
					pCtx->sc_pbc_duration_time++;
					if(pCtx->sc_pbc_duration_time > 120)
					{
						pCtx->sc_pbc_duration_time = 0;
						sprintf(buf, "ifconfig %s down", pCtx->sc_wlan_ifname);
						system(buf);
						sprintf(buf, "iwpriv %s set_mib sc_enabled=0", pCtx->sc_wlan_ifname);
						system(buf);
						sprintf(buf, "iwpriv %s set_mib sc_duration_time=-1", pCtx->sc_wlan_ifname);
						system(buf);
						sprintf(buf, "iwpriv %s set_mib sc_pin_enabled=1", pCtx->sc_wlan_ifname);
						system(buf);
						sprintf(buf, "ifconfig %s up", pCtx->sc_wlan_ifname);
						system(buf);
						sleep(2);
						sprintf(buf, "iwpriv %s set_mib sc_enabled=1", pCtx->sc_wlan_ifname);
						system(buf);
					}
				}
	
			}
#if defined(WPS_SC_CONCURRENT)
			if(pCtx->sc_wps_support == 1)
			{
				if(start_sc == 1)
				{
					pCtx->sc_wps_duration_time = 0;
					if(pCtx->sc_pbc_duration_time > MAX_SC_TIME)
					{
						sprintf(buf, "iwpriv %s set_mib sc_enabled=0", pCtx->sc_wlan_ifname);
						system(buf);
						sprintf(buf, "wscd -sig_pbc %s", pCtx->sc_wlan_ifname);
						system(buf);
						pCtx->sc_wps_duration_time = 1;
						pCtx->sc_pbc_duration_time = 0;
					}
				}
				else if(pCtx->sc_wps_duration_time>0)
				{
					printf("it is doing WPS now!!!\n");
					pCtx->sc_wps_duration_time++;
					if(pCtx->sc_wps_duration_time > MAX_SC_TIME)
					{
						system("echo 1 > /tmp/wscd_cancel");	
						sprintf(buf, "iwpriv %s set_mib sc_enabled=1", pCtx->sc_wlan_ifname);
						system(buf);
						pCtx->sc_wps_duration_time = 0;
						pCtx->sc_pbc_duration_time = 1;
					}
				}
			}
#endif
		}

#if defined(CONFIG_RTL_SIMPLE_CONFIG_USE_WPS_BUTTON)
		if(wps_sc_concurrent == 1)
		{
			if(pCtx->sc_status >=2)
			{
				printf("stop wps\n");
				system("echo 1 > /tmp/wscd_cancel");	
				wps_sc_concurrent = 0;
				sleep(2);
				sprintf(buf, "ifconfig %s down", pCtx->sc_wlan_ifname);
				system(buf);
				sprintf(buf, "ifconfig %s up", pCtx->sc_wlan_ifname);
				system(buf);
			}
		}
#endif
		
		if((pCtx->sc_linked_time >=10) && (pCtx->sc_pbc_pressed_time == 0))
		{
			system("echo 1 > /proc/gpio");
		}
		else if(start_sc == 1) 
		{
			if(pCtx->sc_pbc_pressed_time == 0)
			{
				if((pCtx->sc_run_time%2) == 0)
					system("echo 1 > /proc/gpio");
				else
					system("echo 0 > /proc/gpio");
			}
			
		}
		else
			system("echo 0 > /proc/gpio");
#endif
	}
	
}



