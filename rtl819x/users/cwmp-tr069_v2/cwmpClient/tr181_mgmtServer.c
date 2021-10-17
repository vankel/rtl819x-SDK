#include "tr181_mgmtServer.h"
#include "tr181_mgableDev.h"
#include "cwmp_notify.h"


#define CONFIG_SET(key, val) if ( mib_set(key, val)==0)  return ERR_9002
#define CONFIG_GET(key, ret) if ( mib_get(key, ret)==0)  return ERR_9002

#define CHECK_PARAM_NUM(input, min, max) if ( (input < min) || (input > max) ) return ERR_9007;
#define CHECK_PARAM_STR(str, min, max)  do { \
	int tmp; \
	if (!str) return ERR_9007; \
	tmp=strlen(str); \
	if ((tmp < min) || (tmp > max)) return ERR_9007; \
}	while (0)


/*******************************************************************************
DEVICE.ManagementServer Parameters
*******************************************************************************/
struct CWMP_OP tMgmtServerLeafOP = { getMgmtServer,setMgmtServer };
struct CWMP_PRMT tMgmtServerLeafInfo[] =
{
	/*(name,				type,		flag,			op)*/
	{"EnableCWMP",                        eCWMP_tBOOLEAN,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP}, // factory default true
	{"URL",                               eCWMP_tSTRING,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"Username",                          eCWMP_tSTRING,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"Password",                          eCWMP_tSTRING,	CWMP_PASSWORD|CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"PeriodicInformEnable",              eCWMP_tBOOLEAN,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"PeriodicInformInterval",            eCWMP_tUINT,		CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"PeriodicInformTime",                eCWMP_tDATETIME,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"ParameterKey",                      eCWMP_tSTRING,	CWMP_READ|CWMP_DENY_ACT,	&tMgmtServerLeafOP},
	{"ConnectionRequestURL",              eCWMP_tSTRING,	CWMP_READ|CWMP_FORCE_ACT,	&tMgmtServerLeafOP},
	{"ConnectionRequestUsername",         eCWMP_tSTRING,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"ConnectionRequestPassword",         eCWMP_tSTRING,	CWMP_PASSWORD|CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"UpgradesManaged",                   eCWMP_tBOOLEAN,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP}, // set problem
	{"KickURL",                           eCWMP_tSTRING,	CWMP_READ,	&tMgmtServerLeafOP},
	{"DownloadProgressURL",               eCWMP_tSTRING,	CWMP_READ,	&tMgmtServerLeafOP},
//	{"DefaultActiveNotificationThrottle", eCWMP_tUINT,		CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"CWMPRetryMinimumWaitInterval",      eCWMP_tUINT,		CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
	{"CWMPRetryIntervalMultiplier",       eCWMP_tUINT,		CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
//	{"UDPConnectionRequestAddress",       eCWMP_tSTRING,	CWMP_READ,	&tMgmtServerLeafOP},
//	{"STUNEnable",                        eCWMP_tBOOLEAN,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
//	{"STUNServerAddress",                 eCWMP_tSTRING,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
//	{"STUNServerPort",                    eCWMP_tUINT,		CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
//	{"STUNUsername",                      eCWMP_tSTRING,	CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
//	{"STUNPassword",                      eCWMP_tSTRING,	CWMP_PASSWORD|CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
//	{"STUNMaximumKeepAlivePeriod",        eCWMP_tINT,		CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
//	{"STUNMinimumKeepAlivePeriod",        eCWMP_tUINT,		CWMP_WRITE|CWMP_READ,	&tMgmtServerLeafOP},
//	{"NATDetected",                       eCWMP_tBOOLEAN,	CWMP_READ,	&tMgmtServerLeafOP},
	{"ManageableDeviceNumberOfEntries",   eCWMP_tUINT,		CWMP_READ,	&tMgmtServerLeafOP},
};

enum eMgmtServerLeaf
{
	eMS_EnableCWMP,
	eMS_URL,
	eMS_Username,
	eMS_Password,
	eMS_PeriodicInformEnable,
	eMS_PeriodicInformInterval,
	eMS_PeriodicInformTime,
	eMS_ParameterKey,
	eMS_ConnectionRequestURL,
	eMS_ConnectionRequestUsername,
	eMS_ConnectionRequestPassword,
	eMS_UpgradesManaged,
	eMS_KickURL,
	eMS_DownloadProgressURL,
//	eMS_DefaultActiveNotificationThrottle,
	eMS_CWMPRetryMinimumWaitInterval,
	eMS_CWMPRetryIntervalMultiplier,
//	eMS_UDPConnectionRequestAddress,
//	eMS_STUNEnable,
//	eMS_STUNServerAddress,
//	eMS_STUNServerPort,
//	eMS_STUNUsername,
//	eMS_STUNPassword,
//	eMS_STUNMaximumKeepAlivePeriod,
//	eMS_STUNMinimumKeepAlivePeriod,
//	eMS_NATDetected,
	eMS_ManageableDeviceNumberOfEntries
};

struct CWMP_LEAF tMgmtServerLeaf[] =
{
	{ &tMgmtServerLeafInfo[eMS_EnableCWMP] },
	{ &tMgmtServerLeafInfo[eMS_URL] },
	{ &tMgmtServerLeafInfo[eMS_Username] },
	{ &tMgmtServerLeafInfo[eMS_Password] },
	{ &tMgmtServerLeafInfo[eMS_PeriodicInformEnable] },
	{ &tMgmtServerLeafInfo[eMS_PeriodicInformInterval] },
	{ &tMgmtServerLeafInfo[eMS_PeriodicInformTime] },
	{ &tMgmtServerLeafInfo[eMS_ParameterKey] },
	{ &tMgmtServerLeafInfo[eMS_ConnectionRequestURL] },
	{ &tMgmtServerLeafInfo[eMS_ConnectionRequestUsername] },
	{ &tMgmtServerLeafInfo[eMS_ConnectionRequestPassword] },
	{ &tMgmtServerLeafInfo[eMS_UpgradesManaged] },
	{ &tMgmtServerLeafInfo[eMS_KickURL] },
	{ &tMgmtServerLeafInfo[eMS_DownloadProgressURL] },
//	{ &tMgmtServerLeafInfo[eMS_DefaultActiveNotificationThrottle] },
	{ &tMgmtServerLeafInfo[eMS_CWMPRetryMinimumWaitInterval] },
	{ &tMgmtServerLeafInfo[eMS_CWMPRetryIntervalMultiplier] },
//	{ &tMgmtServerLeafInfo[eMS_UDPConnectionRequestAddress] },
//	{ &tMgmtServerLeafInfo[eMS_STUNEnable] },
//	{ &tMgmtServerLeafInfo[eMS_STUNServerAddress] },
//	{ &tMgmtServerLeafInfo[eMS_STUNServerPort] },
//	{ &tMgmtServerLeafInfo[eMS_STUNUsername] },
//	{ &tMgmtServerLeafInfo[eMS_STUNPassword] },
//	{ &tMgmtServerLeafInfo[eMS_STUNMaximumKeepAlivePeriod] },
//	{ &tMgmtServerLeafInfo[eMS_STUNMinimumKeepAlivePeriod] },
//	{ &tMgmtServerLeafInfo[eMS_NATDetected] },
	{ &tMgmtServerLeafInfo[eMS_ManageableDeviceNumberOfEntries] },
	{ NULL	}
};


/*******************************************************************************
Function
*******************************************************************************/
int getMgmtServer(char *name, struct CWMP_LEAF *entity, int *type, void **data)
{
	unsigned char buf[256+1]={0};
	unsigned char ch=0;
	unsigned int  in=0;
	
	if( (name==NULL) || (type==NULL) || (data==NULL) || (entity==NULL)) 
		return -1;

	notify_set_attributes( "Device.GatewayInfo.ManufacturerOUI", CWMP_NTF_FORCED|CWMP_NTF_ACT, CWMP_ACS_MASK );
	notify_set_attributes( "Device.GatewayInfo.ProductClass", CWMP_NTF_FORCED|CWMP_NTF_ACT, CWMP_ACS_MASK );
	notify_set_attributes( "Device.GatewayInfo.SerialNumber", CWMP_NTF_FORCED|CWMP_NTF_ACT, CWMP_ACS_MASK );

	*type = entity->info->type;
	*data = NULL;
	switch(getIndexOf(tMgmtServerLeaf, entity->info->name)) {
	case eMS_URL:
		CONFIG_GET(MIB_CWMP_ACS_URL, buf);
		*data = strdup( buf );
		break;
	case eMS_Username:
		CONFIG_GET(MIB_CWMP_ACS_USERNAME, buf);
		*data = strdup(buf);
		break;
	case eMS_Password:
#if DEBUG
		CONFIG_GET(MIB_CWMP_ACS_PASSWORD, buf);
		*data = strdup(buf);
#else
		*data = strdup("");
#endif
		break;
	case eMS_PeriodicInformEnable:
		CONFIG_GET(MIB_CWMP_INFORM_ENABLE, &in);
		*data = booldup(in);
		break;	
	case eMS_PeriodicInformInterval:
		CONFIG_GET(MIB_CWMP_INFORM_INTERVAL, &in);
		*data = uintdup(in);
		break;	
	case eMS_PeriodicInformTime:
		CONFIG_GET(MIB_CWMP_INFORM_TIME, &in);
		*data = timedup(in);
		break;	
	case eMS_ParameterKey: // ParameterKey
		{
			unsigned char gParameterKey[32+1];
			CONFIG_GET(MIB_CWMP_PARAMETERKEY,gParameterKey);
			*data = strdup(gParameterKey);
		break;	
		}
	case eMS_ConnectionRequestURL:
		if (MgmtSrvGetConReqURL(buf, 256))			
			*data = strdup(buf);
		else
			*data = strdup("");
		break;	
	case eMS_ConnectionRequestUsername:
		CONFIG_GET(MIB_CWMP_CONREQ_USERNAME, buf);
		*data = strdup(buf);
		break;	
	case eMS_ConnectionRequestPassword:
	#if DEBUG
		CONFIG_GET(MIB_CWMP_CONREQ_PASSWORD, buf);
		*data = strdup(buf);
	#else
		*data = strdup("");
	#endif
		break;	
	case eMS_UpgradesManaged:
		CONFIG_GET(MIB_CWMP_ACS_UPGRADESMANAGED, &ch);
		*data = booldup(ch);
		break;
	case eMS_KickURL:
		CONFIG_GET(MIB_CWMP_ACS_KICKURL, buf);
		*data = strdup(buf);
		break;	
	case eMS_DownloadProgressURL:
		CONFIG_GET(MIB_CWMP_ACS_DOWNLOADURL, buf);
		*data = strdup(buf);
		break;				

	case eMS_CWMPRetryMinimumWaitInterval:
		CONFIG_GET(MIB_CWMP_RETRY_MIN_WAIT_INTERVAL, &in);
		*data = uintdup(in);
		break;

	case eMS_CWMPRetryIntervalMultiplier:
		CONFIG_GET(MIB_CWMP_RETRY_INTERVAL_MUTIPLIER, &in);
		*data = uintdup(in);
		break;

	case eMS_ManageableDeviceNumberOfEntries:
	{
		FILE *fp;
		int count=0;
		fp=fopen( TR069_ANNEX_F_DEVICE_FILE, "r" );
		
		while( fp && fgets( buf,160,fp ) )
		{
			char *p;
			
			p = strtok( buf, " \n\r" );
			if( p && atoi(p)>0 )
			{
				count++;
			}
		}
		if(fp) fclose(fp);
		gMgableDevNum = count;
		*data = uintdup(gMgableDevNum);
	}
		break;

	default:
		return ERR_9005;
				
	}

	return 0;

}

int setMgmtServer(char *name, struct CWMP_LEAF *entity, int type, void *data)
{
	char	*lastname = entity->info->name;
	char 	*buf=data;
	int  	len=0;	
	unsigned int *pNum;
	unsigned char byte;
	unsigned int iVal;
	
	if( (name==NULL) || (entity==NULL)) return -1;
	if( entity->info->type!=type ) return ERR_9006;

	switch(getIndexOf(tMgmtServerLeaf, entity->info->name)) {
	case eMS_URL:
		CHECK_PARAM_STR(buf, 0, 256+1);
		CONFIG_SET(MIB_CWMP_ACS_URL, buf);
		cwmpSettingChange(MIB_CWMP_ACS_URL);
		break;
	case eMS_Username:
		CHECK_PARAM_STR(buf, 0, 256+1);
		CONFIG_SET(MIB_CWMP_ACS_USERNAME, buf);
		cwmpSettingChange(MIB_CWMP_ACS_USERNAME);
		break;
	case eMS_Password:
		CHECK_PARAM_STR(buf, 0, 256+1);
		CONFIG_SET(MIB_CWMP_ACS_PASSWORD, buf);
		cwmpSettingChange(MIB_CWMP_ACS_PASSWORD);
		break;
	case eMS_PeriodicInformEnable:
		pNum = (unsigned int *)data;
		CHECK_PARAM_NUM(*pNum, 0, 1);
		iVal = (*pNum == 0) ? 0 : 1;
		CONFIG_SET(MIB_CWMP_INFORM_ENABLE, &iVal);	
		cwmpSettingChange(MIB_CWMP_INFORM_ENABLE);
		break;	
	case eMS_PeriodicInformInterval:
		pNum = (unsigned int *)data;
		if (*pNum < 1) return ERR_9007;		
		CONFIG_SET(MIB_CWMP_INFORM_INTERVAL, pNum);
		cwmpSettingChange(MIB_CWMP_INFORM_INTERVAL);
		break;	
	case eMS_PeriodicInformTime:
		pNum = (unsigned int *)buf;
		CONFIG_SET(MIB_CWMP_INFORM_TIME, buf);
		cwmpSettingChange(MIB_CWMP_INFORM_TIME);
		break;	

	case eMS_ConnectionRequestUsername:
		CHECK_PARAM_STR(buf, 0, 256+1);
		CONFIG_SET(MIB_CWMP_CONREQ_USERNAME, buf);
		cwmpSettingChange(MIB_CWMP_CONREQ_USERNAME);
		break;	
	case eMS_ConnectionRequestPassword:
		CHECK_PARAM_STR(buf, 0, 256+1);
		CONFIG_SET(MIB_CWMP_CONREQ_PASSWORD, buf);
		cwmpSettingChange(MIB_CWMP_CONREQ_PASSWORD);
		break;	
	case eMS_UpgradesManaged:
		pNum = (unsigned int *)data;
		CHECK_PARAM_NUM(*pNum, 0, 1);
		byte = (*pNum == 0) ? 0 : 1;
		CONFIG_SET(MIB_CWMP_ACS_UPGRADESMANAGED, &byte);	
		break;
	case eMS_CWMPRetryMinimumWaitInterval:
		pNum = (unsigned int *)data;
		if (*pNum < 1) return ERR_9007;		
		CONFIG_SET(MIB_CWMP_RETRY_MIN_WAIT_INTERVAL, pNum);
		cwmpSettingChange(MIB_CWMP_RETRY_MIN_WAIT_INTERVAL);
		break;	
	case eMS_CWMPRetryIntervalMultiplier:
		pNum = (unsigned int *)data;
		if (*pNum < 1000) return ERR_9007;		
		CONFIG_SET(MIB_CWMP_RETRY_INTERVAL_MUTIPLIER, pNum);
		cwmpSettingChange(MIB_CWMP_RETRY_INTERVAL_MUTIPLIER);
		break;	
	default:
		return ERR_9005;
				
	}

	return 0;


}