#ifndef INCLUDE_WAPI_H
#define INCLUDE_WAPI_H
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/dir.h>

#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include <linux/if.h>

#if 0
#include "openssl/x509.h"
#include <openssl/ec.h>
#include <openssl/ecdsa.h>
#include <openssl/ecdh.h>
#include <openssl/objects.h>
#include <openssl/err.h>
#endif

#define SUPPORT_TWO_SIGNATURES
//For test
//#define	FL_TRACE(__FLG__)				printf("[%s][%d]:-[%d]\n", __FUNCTION__, __LINE__, __FLG__);
//#define DEBUG(args) (printf("DEBUG: "), printf(args))
#define TRACE(x...) x
#if 0
#define DEBUG(x...) TRACE(printf(x))
#else
#define DEBUG(x...)
#endif
#define ERR_PRINT(x...) TRACE(printf(x))

//#define __PACK__	__attribute__ ((packed))

#define SUCCESS 0
#define FAILED -1

//To support MSSID
//Note: this must sync with linux-2.6.x/drivers/net/wireless/rtl8190/8190n_cfg.h
//ex: {"wlan0","wlan0-va0","wlan0-va1","wlan0-va2","wlan0-va3"}
#define RTL8190_NUM_VWLAN  4
#define RTL8190_NUM_WLAN	RTL8190_NUM_VWLAN+1
#define NUM_WLAN_INTERFACE 2
char wlanInterface[RTL8190_NUM_WLAN*NUM_WLAN_INTERFACE][IFNAMSIZ];
char wlanInterfaceNum;

#if 1
int wapiCertSel;	//0: use certs from remote AS0; 1: use certs from remote AS1; 2: use certs from local AS.
char apCertFile[30], ca4apCertFile[30];

//Add for ASU certificate
char asuCertFile[30];
#endif

typedef struct __CERT_SELECT__
{
	int wapiCertSel;
	char apCertFile[30];
	char ca4apCertFile[30];
	char asuCertFile[30];
}Cert_Select, *pCert_Select;

//int send_flag;
//unsigned char saved_verify_flag[32];

//length of buffer for application to driver
#define APP2DRV_BUF_LEN 2400
#define CERT_VERIFY_RESULT_MAX_LEN 2400

#define CA_CERT 			"/var/myca/CA.cert"	//PEM(default)
#define CA_CERT_DER 	"/var/myca/CA.der"	//DER
#define CA_PUB_KEY		"/var/myca/CA_pub.key"	//not stored in flash, it can be get from CA_CERT
#define CA_PRIV_KEY		"/var/myca/CA.key"

#define TMP_CA_CERT		"/var/tmp/CA.cert" //temperary file for ca cert and cert private

//user cert just for test here
#define USER_CERT		"/var/myca/user.cert"	//PEM(default)
#define USER_CERT_DER	"/var/myca/user.der"	//DER

#define AP_CERT			"/var/myca/ap.cert"	//PEM(default)
#define AP_CERT_DER 	"/var/myca/ap.der"	//DER

#define CA4AP_CERT "/var/myca/ca4ap.cert"		//filetype: TYPE_CA4AP_CERT
#define CA4AP_DER "/var/myca/ca4ap.der"		//DER

#define AP_CERT_AS0	"/var/myca/ap_as0.cert"			//filetype: TYPE_AP_CERT_AS0
#define CA4AP_CERT_AS0 "/var/myca/ca4ap_as0.cert"		//filetype: TYPE_CA4AP_CERT_AS0

#define AP_CERT_AS1	"/var/myca/ap_as1.cert"			//filetype: TYPE_AP_CERT_AS1
#define CA4AP_CERT_AS1 "/var/myca/ca4ap_as1.cert"		//filetype: TYPE_CA4AP_CERT_AS1

//Add for ASU certificate
#define ASU_CERT "/var/myca/asu.cert"
#define ASU_CERT_AS0 "/var/myca/asu_as0.cert"
#define ASU_CERT_AS1 "/var/myca/asu_as1.cert"
#define ASU_CERT_DER "/var/myca/asu.der"


#define SAVE_CERT_SEL "/var/tmp/certsel.txt"

//for test
#define TMP_USER_PUB_KEY	"/var/myca/user_pub.key"	//not stored in flash, it can be get from user cert
#define TMP_USER_CERT "/var/tmp/user.cert"	//PEM(default)
#define TMP_USER_CERT_DER "/var/tmp/user.der"	//DER
#define TMP_AP_CERT "/var/tmp/ap.cert"		//PEM(default)
#define TMP_AP_CERT_DER "/var/tmp/ap.der"		//DER

#define CERT_VERIFY_RESULT "/var/myca/certVerifyResult"

////////////
//NOT USED for signature NOW
#define TMP_IN_MSG "/var/myca/tmpInMsg.txt"
#define TMP_OUT_MSG "/var/myca/tmpOutMsg.txt"
#define TMP_DGST_VERIFY_RESULT "/var/myca/dgstVerifyResult"
////////////////////////////

//For signature and signature verify now
#define TMP_MSG 				"/var/tmp/msg.txt"			//source data
#define TMP_SIG					"/var/tmp/sig.txt"				//signature(input)
#define TMP_SIG_VERIFY_RES		"/var/tmp/sigVerifyRes.txt"		//signature verify result: if this file exist, verify OK; else verify wrong.
#define TMP_SIG_OUT				"/var/tmp/sig_out.txt"				//signature(output)

//For test
#define TMP_IN_MSG2 "/var/tmp/tmpInMsg.txt"
#define TMP_OUT_MSG2 "/var/tmp/tmpOutMsg.txt"
//End for test

///////////////////////////
//These tmp files are used for udp_sock communicate with openssl cmd
//For ECDH compute
//output to openssl ECDH
#define TMP_PEER_PUB_KEY "/var/tmp/peerPubKey"
#define ADDID_FILE "/var/tmp/ADDID"
#define NAE_FILE "/var/tmp/Nae"
#define NASUE_FILE "/var/tmp/Nasue"
//input from openssl ECDH
#define TMP_LOCAL_PUB_KEY "/var/tmp/localPubKey"
#define BK_FILE "/var/tmp/BK"
#define NEXT_AUTH_SEED "/var/tmp/nextAuthSeed"
#define BKID_FILE "/var/tmp/BKID"
//use this now!!!
#define KEY_FOR_BK "/var/tmp/key4bk"
////////////////////////////////////

#define TMP_BUF_MAX_LEN 1500

//#define WAPI_PKT_MAX_LEN 65535

#define CERT_DATA_MAX_LEN 1200			// user cert include user private key
#define ID_MAX_LENGTH	500				//Max length of identification
#define USER_NAME_MAX_LEN 200			// To limit subjector name length and issuer name length (ASN.1/DER)
#define SERIAL_MAX_LEN 10				//max length of string of serial in hex
#define ID_MAX_NUMBER 10				//max number of ID
#define SIGN_PARAMS_DATA_MAX_LEN 30	//max length of signature algorithm's data (indicated by OID)
#define ECDH_PARAMS_DATA_MAX_LEN 30	//max length of ECDH algorithm's data (indicated by OID)
#define SIGN_DATA_MAX_LEN 64			//max length of signature data
#define WAI_PKT_DATA_MAX_LEN 4000		//max length of wai packet data
#define WAI_PKT_HEADER_MAX_LEN 20		//max length of wai packet header
#define WAI_SEC_DATA_MAX_LEN 64		//max length of wai security data, such as: ASUE/AE temporary key data

typedef enum _WAPI_CERT_VERIFY_RESULT{ 
	RES_CERT_VALID=0, 
	RES_CERT_ISSUER_UNKNOWN=1,
	RES_ROOT_CERT_UNTRUSTED=2,
	RES_CERT_INVALID=3,				//Include 1) not reach valid days 2) expired
	RES_SIGN_INCORRECT=4,
	RES_CERT_REVOKED=5,
	RES_CERT_USE_INCORRECT=6,
	RES_CERT_REVOKE_STATE_UNKNOWN=7,
	RES_CERT_ERR_UNKNOWN=8
}WAPI_CERT_VERIFY_RESULT;

typedef enum _WAPI_PKT_SUBTYPE{ 
	TYPE_PREVERIFY_START=1,
	TYPE_STA_KEY_REQ=2,
	TYPE_ACTIVATE_VERIFY=3,
	TYPE_ACCESS_VERIFY_REQ=4,
	TYPE_ACCESS_VERIFY_RES=5,
	TYPE_CERT_VERIFY_REQ=6,
	TYPE_CERT_VERIFY_RES=7,
	TYPE_UNICAST_KEY_ASSOCIATE_REQ=8,
	TYPE_UNICAST_KEY_ASSOCIATE_RES=9,
	TYPE_UNICAST_KEY_ASSOCIATE_ACK=10,
	TYPE_MULTICAST_KEY_NOTIFY=11,
	TYPE_MULTICAST_KEY_RES=12
}WAPI_PKT_SUBTYPE;

typedef enum _WAPI_ACCESS_RESULT{ 
	RES_ACCESS_SUCCESS=0,			//RES_CERT_VALID
	RES_CANNOT_VERIFY_CERT=1,	//RES_CERT_ISSUER_UNKNOWN
	RES_WRONG_CERT=2,				//other WAPI_CERT_VERIFY_RESULT except RES_CERT_VALID and RES_CERT_ISSUER_UNKNOWN
	RES_LOCAL_POLICY_FORBID=3
}WAPI_ACCESS_RESULT;

#if 0
typedef struct _wapiIdData_ {
	unsigned char subjector[USER_NAME_MAX_LEN];
	unsigned char issuer[USER_NAME_MAX_LEN];
	unsigned char serial[SERIAL_MAX_LEN];	// ??????????
} WAPI_ID_DATA_T, *WAPI_ID_DATA_Tp;


typedef struct _wapiID_ {
	unsigned short idFlag;	// 1: to indicate X.509 V3's subjector + issuer + serial; 2: to indicate GBW's subjector + issuer + serial
	unsigned short idLen;	//length of ID data
	WAPI_ID_DATA_T idData;
} WAPI_ID_T, *WAPI_ID_Tp;
#endif

typedef struct _wapiID_ {
	unsigned short idFlag;	// 1: to indicate X.509 V3's subjector + issuer + serial; 2: to indicate GBW's subjector + issuer + serial
	unsigned short idLen;	//length of ID data
	unsigned char idData[ID_MAX_LENGTH];	//subjector + issuer + serial
} WAPI_ID_T, *WAPI_ID_Tp;

typedef struct _wapiIdList_ {
	unsigned char type;	// 3: for ID list
	unsigned short length;
	unsigned char reserved;
	unsigned short idNumber;
	WAPI_ID_T idListData[ID_MAX_NUMBER];
} WAPI_ID_LIST_T, *WAPI_ID_LIST_Tp;

//signature value
typedef struct _wapiSignVal_ {
	unsigned short length;		// length of signature data
	unsigned char data[SIGN_DATA_MAX_LEN];		//signature data
} WAPI_SIGN_VAL_T, *WAPI_SIGN_VAL_Tp;

//parameters of signature algorithm
typedef struct _wapiSignAlgParam_ {
	unsigned char flag;	// 1: parameters indicated by OID
	unsigned short length;	// length of parameters' data
	unsigned char data[SIGN_PARAMS_DATA_MAX_LEN];	// For wapi: ECC parameters OID is 1.2.156.11235.1.1.2.1
} WAPI_SIGN_ALG_PARAM_T, *WAPI_SIGN_ALG_PARAM_Tp;

//algorithm of signature
typedef struct _wapiSignAlg_ {
	unsigned short length;
	unsigned char hashAlgFlag;				// 1: SHA_256
	unsigned char signAlgFlag;					// 1: ECDSA_192
	WAPI_SIGN_ALG_PARAM_T signAlgParams;	// parameters of signature algorithm
} WAPI_SIGN_ALG_T, *WAPI_SIGN_ALG_Tp;

typedef struct _wapiSign_ {
	unsigned char type;			// 1: for signature
	unsigned short length;
	WAPI_ID_T signId;			// ID of signaturer
	WAPI_SIGN_ALG_T signAlg;	// algorithm of signature
	WAPI_SIGN_VAL_T signVal;		// signature
} WAPI_SIGN_T, *WAPI_SIGN_Tp;

typedef struct _wapiCert_ {
	unsigned short certFlag; 						// 1: X.509 V3; 2: GBW
	unsigned short certLen;						//length of cert
	unsigned char certData[CERT_DATA_MAX_LEN];	//cert data
} WAPI_CERT_T, *WAPI_CERT_Tp;

//wapi cert verify request
typedef struct _wapiCertVerifyReq_ {
	unsigned char addID[12]; 			// MAC_AE || MAC_ASUE
	unsigned char aeChallenge[32];		//random digit generated by AE
	unsigned char asueChallenge[32];	//andom digit generated by ASUE
	WAPI_CERT_T asueCert;			// ASUE cert
	WAPI_CERT_T aeCert;				// AE cert
	WAPI_ID_LIST_T asuTrustedbyAsue;	//(optional) list of ASU which ASUE trust
} WAPI_CERT_VERIFY_REQ_T, *WAPI_CERT_VERIFY_REQ_Tp;


typedef struct _wapiCertVerifyResult_ {
	unsigned char type;		// 2: for cert verify result
	unsigned short length;
	unsigned char rand_1[32];	//AE challenge
	unsigned char rand_2[32];	//ASUE challenge
	//WAPI_CERT_VERIFY_RESULT result_1;	//verify result of ASUE cert
	unsigned char result_1;	//verify result of ASUE cert
	WAPI_CERT_T cert_1;					//ASUE cert
	//WAPI_CERT_VERIFY_RESULT result_2;	//verify result of AE cert
	unsigned char result_2;	//verify result of AE cert
	WAPI_CERT_T cert_2;					//AE cert
} WAPI_CERT_VERIFY_RESULT_T, *WAPI_CERT_VERIFY_RESULT_Tp;

//wapi cert verify response
typedef struct _wapiCertVerifyRes_ {
	unsigned char addID[12]; 						// MAC_AE || MAC_ASUE
	WAPI_CERT_VERIFY_RESULT_T certVerifyResult;
	WAPI_SIGN_T asueTrustServerSign;			// signature by servers trusted by ASUE
	WAPI_SIGN_T aeTrustServerSign;				// signature by servers trusted by AE
} WAPI_CERT_VERIFY_RES_T, *WAPI_CERT_VERIFY_RES_Tp;

////////////
//compound cert verify result
typedef struct _wapiCompoundCertVerifyRes_ {
	WAPI_CERT_VERIFY_RESULT_T certVerifyResult;
	WAPI_SIGN_T asueTrustServerSign;			// signature by servers trusted by ASUE
	WAPI_SIGN_T aeTrustServerSign;				// signature by servers trusted by AE
} WAPI_COMPOUND_CERT_VERIFY_RES_T, *WAPI_COMPOUND_CERT_VERIFY_RES_Tp;

//parameters of ECDH
typedef struct _wapiEcdhParam_ {
	unsigned char flag;	// 1: parameters indicated by OID
	unsigned short length;	// length of parameters' data
	unsigned char data[ECDH_PARAMS_DATA_MAX_LEN];	// For wapi: ECC parameters OID is 1.2.156.11235.1.1.2.1
} WAPI_ECDH_PARAM_T, *WAPI_ECDH_PARAM_Tp;

//wapi verify activate
typedef struct _wapiVerifyActivate_ {
	unsigned char flag;
	unsigned char verifyFlag[32];
	WAPI_ID_T asuID;				//ASU ID trusted by AE
	WAPI_CERT_T aeCert;				//AE cert
	WAPI_ECDH_PARAM_T ecdhParms;	//parameters of ECDH
} WAPI_VERIFY_ACTIVATE_T, *WAPI_VERIFY_ACTIVATE_Tp;

//wapi security data, such as: ASUE/AE temporary key data
typedef struct _wapiSecData_ {
	unsigned char length;
	unsigned char content[WAI_SEC_DATA_MAX_LEN];
} WAPI_SEC_DATA_T, *WAPI_SEC_DATA_Tp;

//wapi access verify request
typedef struct _wapiAccessVerifyReq_ {
	unsigned char flag;
	unsigned char verifyFlag[32];
	unsigned char asueChallenge[32];
	WAPI_SEC_DATA_T asueKey;
	WAPI_ID_T aeID;
	WAPI_CERT_T asueCert;
	WAPI_ECDH_PARAM_T ecdhParms;
	WAPI_ID_LIST_T asuTrustedbyAsue;	//(optional) list of ASU which ASUE trust
	WAPI_SIGN_T signByAsue;				// signature by ASUE
} WAPI_ACCESS_VERIFY_REQ_T, *WAPI_ACCESS_VERIFY_REQ_Tp;

//wapi access verify response
typedef struct _wapiAccessVerifyRes_ {
	unsigned char flag;
	unsigned char asueChallenge[32];
	unsigned char aeChallenge[32];
	//WAPI_ACCESS_RESULT accessResult;
	unsigned char accessResult;
	WAPI_SEC_DATA_T asueKey;
	WAPI_SEC_DATA_T aeKey;
	WAPI_ID_T aeID;
	WAPI_ID_T asueID;
	WAPI_COMPOUND_CERT_VERIFY_RES_T compoundCertVerifyRes;		//(optional)
	WAPI_SIGN_T signByAe;											// signature by AE
} WAPI_ACCESS_VERIFY_RES_T, *WAPI_ACCESS_VERIFY_RES_Tp;
////////////////

//wapi pkt
typedef struct _wapiPkt_ {
	unsigned short ver;			// version: 1 (at present)
	unsigned char type;			// 1: WAI packet
	//WAPI_PKT_SUBTYPE subType;	//only support type: 1, that is WAI packet
	unsigned char subType;	//only support type: 1, that is WAI packet
	unsigned short reserved;		//0: default
	unsigned short  length;		// length of WAI packet
	unsigned short  pktSeqNo;		// sequence number of packet, the first is 1, then increase 1 every pkt
	unsigned char  segSegNo;		// segment number of one packet, the first is 0, then increase 1 every segment
	unsigned char segFlag;		// 0: no more segment; 1: more segment
	unsigned char data[WAI_PKT_DATA_MAX_LEN];
} WAPI_PKT_T, *WAPI_PKT_Tp;

//For debug
#if 0
void dumpHex(const unsigned char * buf, int bufLen);
void dumpStr(const char * buf, int bufLen);
#endif

//void GenerateRandomData(unsigned char * data, unsigned int len);
int isFileExist(const char * filename);
int storeStr2File(const char * tmpFile, const unsigned char * str, const int strLen);
int readFile2Str(unsigned char * str, int * strLen, const char * tmpFile);
void WapiGetParameterFromCert(unsigned char*x509Buf,int x509Len,unsigned char*serialNum, int*serialNumLen,unsigned char *issuerName, int*issuerLen,unsigned char *subjectName,int*subjectLen);
//int signMsg(const char * inMsg, const int inMsgLen, const char *privKeyFile, char * outMsg, int * outMsgLen);
//int verifyMsgSignature(const char * msg, const int msgLen, const char * msgSig, const int msgSigLen, const char *pubKeyFile, char * verifyOK);
//int genCertVerifyReq(WAPI_CERT_VERIFY_REQ_Tp req, const char * asueCertFile, const char * aeCertFile);
//int genCertVerifyReqPkt(WAPI_PKT_Tp reqPkt, const char * asueCertFile, const char * aeCertFile);
int processCertVerifyReq(WAPI_CERT_VERIFY_REQ_Tp req, WAPI_CERT_VERIFY_RES_Tp res, int * certVerifyResBufLen);
int genVerifyActivate(WAPI_VERIFY_ACTIVATE_Tp verifyActivate);
//int genAccessVerifyRes(WAPI_ACCESS_VERIFY_RES_Tp accessVerifyRes, WAPI_ACCESS_VERIFY_REQ_Tp accessVerifyReq, WAPI_CERT_VERIFY_RES_Tp certVerifyRes, WAPI_CERT_VERIFY_REQ_Tp certVerifyReq);
int parseBuf2AccessVerifyReq(WAPI_ACCESS_VERIFY_REQ_Tp accessVerifyReq, int * infoLen, const unsigned char * msg, int msgLen);
int   WapiECDSAVerify(unsigned char*src,int srcLen, unsigned char*signature,unsigned short signatureLen,const char* pubKeyFile);
#endif	//INCLUDE_WAPI_H

