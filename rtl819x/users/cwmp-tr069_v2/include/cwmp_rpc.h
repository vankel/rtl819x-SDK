#ifndef _CWMP_RPC_H_
#define _CWMP_RPC_H_

#include "libcwmp.h"
//#include <rtk/options.h> Keith remove


/*after setparametervalue/addobject/deleteobject & statu==1, reboot the system*/
//#define SELF_REBOOT

#define EC_BOOTSTRAP		0x00001	/*0 BOOTSTRAP*/
#define EC_BOOT			0x00002	/*1 BOOT*/
#define EC_PERIODIC		0x00004	/*2 PERIODIC*/
#define EC_SCHEDULED		0x00008	/*3 SCHEDULED*/
#define EC_VALUECHANGE		0x00010	/*4 VALUE CHANGE*/
#define EC_KICKED		0x00020	/*5 KICKED*/
#define EC_CONNREQUEST		0x00040	/*6 CONNECTION REQUEST*/
#define EC_TRANSFER		0x00080	/*7 TRANSFER COMPLETE*/
#define EC_DIAGNOSTICS		0x00100	/*8 DIAGNOSTICS COMPLETE*/
#define EC_REQUESTDL		0x00200	/*9 REQUEST DOWNLOAD*/
#define EC_M_REBOOT		0x00400	/*M Reboot*/
#define EC_M_SCHEDULED		0x00800	/*M ScheduleInform*/
#define EC_M_DOWNLOAD		0x01000	/*M Download*/
#define EC_M_UPLOAD		0x02000	/*M Upload*/
#define EC_M_VENDOR		0x04000	/*M <vendor-specific method>*/
#define EC_X_VENDOR		0x08000	/*X <OUI> <event>*/
#define EC_X_CT_COM_ACCOUNT	0x10000	/*X_CT-COM_ACCOUNTCHANGE*/
#ifdef _PRMT_X_CT_COM_USERINFO_
#define EC_X_CT_COM_BIND	0x20000	/*X_CT-COM_BIND*/
#endif
/*ping_zhang:20100128 START:add for e8b tr069 Alarm*/
#ifdef _PRMT_X_CT_COM_ALARM_
#define EC_X_CT_COM_ALARM	0x40000	/*X_CT-COM_ALARM*/
#define EC_X_CT_COM_CLEARALARM	0x80000	/*X_CT-COM_CLEARALARM*/
#endif
/*ping_zhang:20100128 END*/


// event we must store across reboot
#ifdef _PRMT_X_CT_COM_USERINFO_
#define EC_SAVE_MASK (EC_SCHEDULED|EC_TRANSFER|EC_M_REBOOT|EC_M_SCHEDULED|EC_M_DOWNLOAD|EC_M_UPLOAD|EC_X_CT_COM_ACCOUNT|EC_X_CT_COM_BIND)
#else
#define EC_SAVE_MASK (EC_SCHEDULED|EC_TRANSFER|EC_M_REBOOT|EC_M_SCHEDULED|EC_M_DOWNLOAD|EC_M_UPLOAD|EC_X_CT_COM_ACCOUNT)
#endif

/*fault string*/
extern char *strERR_9000;
extern char *strERR_9001;
extern char *strERR_9002;
extern char *strERR_9003;
extern char *strERR_9004;
extern char *strERR_9005;
extern char *strERR_9006;
extern char *strERR_9007;
extern char *strERR_9008;
extern char *strERR_9009;
extern char *strERR_9010;
extern char *strERR_9011;
extern char *strERR_9012;
extern char *strERR_9013;
extern char *strERR_default;




/*create request*/
int cwmp_CreateInform(
	struct soap *soap,
	int *type,
	void **data,
	unsigned int e,
	char *opt);
int cwmp_CreateTransferComplete(
	struct soap *soap,
	int *type,
	void **data);
int cwmp_CreateGetRPCMethods(
	struct soap *soap,
	int *type,
	void **data);

/*send & recv message*/
int cwmp_process_send(
	struct soap *soap,
	const char *soap_endpoint,
	const char *soap_action,
	const int type,
	const void *data);
int cwmp_process_recv(
	struct soap *soap,
	int *type,
	void **data);


/*init&uninit userdata struct*/
struct cwmp_userdata *cwmp_init_userdata( void );
int cwmp_free_userdata( struct cwmp_userdata *user );


/*utility*/
void MgmtSrvSetParamKey(const char *key);
int MgmtSrvGetConReqURL(char *url, unsigned int size);

void cwmp_SaveReboot( struct cwmp_userdata *user, int reboot_flag );
void cwmp_reset_DownloadInfo( DownloadInfo_T *dlinfo, int dlway );
void *cwmp_valuedup( struct soap *soap, int type, void *data );


#endif /*#ifndef _CWMP_RPC_H_*/
