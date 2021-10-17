#ifndef INCLUDE_WAPIFILES_H
#define INCLUDE_WAPIFILES_H

#include <stdio.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/dir.h>
#include <string.h>

////////////////////////////////////
//For parse certs database: index.txt
#include <time.h>
//////////////////////////////////////////

//For test
//#define	FL_TRACE(__FLG__)				printf("[%s][%d]:-[%d]\n", __FUNCTION__, __LINE__, __FLG__);
//#define DEBUG(args) (printf("DEBUG: "), printf(args))
#define TRACE(x...) x
#if 0
#define DEBUG(x...) TRACE(printf(x))
#else
#define DEBUG(x...)
#endif
#if 0
#define ERR_PRINT(x...) TRACE(printf(x))
#else
#define ERR_PRINT(x...)
#endif

//#define FLASH_DEVICE_NAME		("/dev/mtd")
#define FLASH_DEVICE_NAME1		("/dev/mtdblock1")

#define AS_CERT	"/web/ca.cer"	//This file is for download from webpage
//#define CA_CERT	"/web/ca.cer"	//This file is for download from webpage

#define CA_CERT	"/var/myca/CA.cert"			//filetype: TYPE_CA_CERT
#define CA_PUB_KEY	"/var/myca/CA_pub.key"	//not stored in flash, it can be get from CA_CERT
#define CA_PRIV_KEY	"/var/myca/CA.key"		//filetype: TYPE_CA_PRIV_KEY
#define CA_CRL	"/var/myca/CA.crl"			//filetype: TYPE_CA_CRL
#define USER_CERT	"/var/myca/user.cert"		//filetype: TYPE_USER_CERT
#define CERTS_DATABASE	"/var/myca/index.txt"	//filetype: TYPE_CERTS_DATABASE
#define AP_CERT	"/var/myca/ap.cert"			//filetype: TYPE_AP_CERT
#define CA4AP_CERT "/var/myca/ca4ap.cert"		//filetype: TYPE_CA4AP_CERT

#define AP_CERT_AS0	"/var/myca/ap_as0.cert"			//filetype: TYPE_AP_CERT_AS0
#define CA4AP_CERT_AS0 "/var/myca/ca4ap_as0.cert"		//filetype: TYPE_CA4AP_CERT_AS0

#define AP_CERT_AS1	"/var/myca/ap_as1.cert"			//filetype: TYPE_AP_CERT_AS1
#define CA4AP_CERT_AS1 "/var/myca/ca4ap_as1.cert"		//filetype: TYPE_CA4AP_CERT_AS1

//Add for ASU certificate
#define ASU_CERT "/var/myca/asu.cert"
#define ASU_CERT_AS0 "/var/myca/asu_as0.cert"
#define ASU_CERT_AS1 "/var/myca/asu_as1.cert" 

#define NEW_USER_CERT	"/var/myca/user.cert"
#define NEW_USER_CERT_SERIAL	"/var/myca/serial.old"

#define NEXT_SERIAL "/var/myca/serial"
#define NEXT_CRLNUMBER "/var/myca/crlnumber"

#define USER_CERT_DIR "/var/myca/certs"		//This is used by me
#define NEW_CERTS_DIR "/var/myca/newcerts"	//This is used by only openssl

#define TYPE_CA_CERT 			0x80000001
#define TYPE_CA_PRIV_KEY 		0x80000002
#define TYPE_CA_CRL 			0x80000003
#define TYPE_USER_CERT 			0x80000004
#define TYPE_CERTS_DATABASE 	0x80000005
#define TYPE_AP_CERT		 	0x80000006
#define TYPE_CA4AP_CERT		0x80000007

//Add for ASU certificate
#define TYPE_ASU_CERT			0x80000008

#define TYPE_AP_CERT_AS0		TYPE_AP_CERT
#define TYPE_CA4AP_CERT_AS0	TYPE_CA4AP_CERT

#define TYPE_AP_CERT_AS1		TYPE_AP_CERT
#define TYPE_CA4AP_CERT_AS1	TYPE_CA4AP_CERT

//Add for ASU certificate
#define TYPE_ASU_CERT_AS0		TYPE_ASU_CERT
#define TYPE_ASU_CERT_AS1		TYPE_ASU_CERT


//Pay attention!!!!
//Be same with skylark/AP/openssl-0.9.8b/apps/demoCA/serial at coding PC
#define SERIAL_INIT_VAL			0x40000000
//Be same with skylark/AP/openssl-0.9.8b/apps/demoCA/crlnumber at coding PC
#define CRLNUMBER_INIT_VAL		0x60000000

#define SIZE_OF_SQFS_SUPER_BLOCK 640
#define SIZE_OF_CHECKSUM 2
#define OFFSET_OF_LEN 2

#define ROOTFS_HEADER_SIZE 16
#define WAPI_AREA_HEADER_SIZE 16	//According to size of struct  _wapiAreaHeader_
#define WAPI_FILE_HEADER_SIZE 12	//According to size of struct _wapiFileHeader_

//fixed max wapi file size
#define CA_CERT_MAXSIZE 1100
#define CA_PRIV_KEY_MAXSIZE 300
#define CA_CRL_MAXSIZE 7000	//???
#define USER_CERT_MAXSIZE 1200

//#define USER_CERT_MAXNUM 40
#define USER_CERT_MAXNUM 35 //reserve some space for ASU certificate

#define USER_CERT_DATABASE_MAXSIZE 2220		//6820
#define AP_CERT_MAXSIZE 1200
#define CA4AP_CERT_MAXSIZE CA_CERT_MAXSIZE

#define AP_CERT_MAXSIZE_AS0 1200
#define CA4AP_CERT_MAXSIZE_AS0 CA_CERT_MAXSIZE
#define AP_CERT_MAXSIZE_AS1 1200
#define CA4AP_CERT_MAXSIZE_AS1 CA_CERT_MAXSIZE

#define ASU_CERT_MAXSIZE 1200
#define ASU_CERT_MAXSIZE_AS0 1200
#define ASU_CERT_MAXSIZE_AS1 1200

#define SIG_LEN			4
#define SQSH_SIGNATURE		((char *)"sqsh")
#define SQSH_SIGNATURE_LE		((char *)"hsqs")
#define WAPI_SIGNATURE		((char *)"wapi")

#ifdef CONFIG_RTL_FLASH_MAPPING_ENABLE
#define FLASH_SIZE CONFIG_RTL_FLASH_SIZE
#define ROOT_IMAGE_OFFSET CONFIG_RTL_ROOT_IMAGE_OFFSET
#else
//#define FLASH_SIZE 0x200000//Just for test
//#define ROOT_IMAGE_OFFSET 0xf0000//Just for test
#define FLASH_SIZE 0x400000//default for 4M flash
#define ROOT_IMAGE_OFFSET 0x130000//default for 4M flash
#endif

#define MTD1_SIZE (FLASH_SIZE - ROOT_IMAGE_OFFSET)

// should be sync with boards/rtl8196c/tools/mkimg or boards/rtl8198/tools/mkimg
// to check root image size when CONFIG_RTL_FLASH_MAPPING_ENABLE is opened.
#define WAPI_SIZE 0x10000	//Address space: 64K

//Wapi area layout
#define WAPI_AREA_BASE (MTD1_SIZE-WAPI_SIZE)
#define CA_CERT_BASE (WAPI_AREA_BASE+WAPI_AREA_HEADER_SIZE)
#define CA_PRIV_KEY_BASE (CA_CERT_BASE+CA_CERT_MAXSIZE)
#define CA_CRL_BASE (CA_PRIV_KEY_BASE+CA_PRIV_KEY_MAXSIZE)
#define USER_CERT_BASE (CA_CRL_BASE+CA_CRL_MAXSIZE)
#define USER_CERT_DATABASE_BASE (USER_CERT_BASE+USER_CERT_MAXSIZE*USER_CERT_MAXNUM)
#define AP_CERT_BASE (USER_CERT_DATABASE_BASE+USER_CERT_DATABASE_MAXSIZE) // ap cert is for our ap,  and ap cert is also user cert at user cert area, which need to be uploaded from webpage
#define CA4AP_CERT_BASE (AP_CERT_BASE+AP_CERT_MAXSIZE)	//ca cert for our ap which need to be uploaded from webpage

#define AP_CERT_BASE_AS0 (CA4AP_CERT_BASE+CA4AP_CERT_MAXSIZE) // ap cert is for our ap,  and ap cert is also user cert at user cert area, which need to be uploaded from webpage
#define CA4AP_CERT_BASE_AS0 (AP_CERT_BASE_AS0+AP_CERT_MAXSIZE_AS0)	//ca cert for our ap which need to be uploaded from webpage

#define AP_CERT_BASE_AS1 (CA4AP_CERT_BASE_AS0+CA4AP_CERT_MAXSIZE_AS0) // ap cert is for our ap,  and ap cert is also user cert at user cert area, which need to be uploaded from webpage
#define CA4AP_CERT_BASE_AS1 (AP_CERT_BASE_AS1+AP_CERT_MAXSIZE_AS1)	//ca cert for out ap which need to be uploaded from webpage

//Add for ASU certificate
#define ASU_CERT_BASE (CA4AP_CERT_BASE_AS1+CA4AP_CERT_MAXSIZE_AS1)
#define ASU_CERT_BASE_AS0 (ASU_CERT_BASE+ASU_CERT_MAXSIZE)
#define ASU_CERT_BASE_AS1 (ASU_CERT_BASE_AS0+ASU_CERT_MAXSIZE_AS0)

//#define WAPI_AREA_END (CA4AP_CERT_BASE_AS1+CA4AP_CERT_MAXSIZE_AS1)
#define WAPI_AREA_END (ASU_CERT_BASE_AS1+ASU_CERT_MAXSIZE_AS1)

#define SUCCESS 0
#define FAILED -1

//For user certs now
#define FILE_NAME_MAX_LEN       	20
#define FILE_MAX_NUM            	40
#define USER_CERT_DIR_MAX_LEN	20
#define SERIAL_MAX_LEN          10	//used for serial max length and ca crlNumber max length

///////////////////////////////
//For parse certs database: index.txt
//#define CERTS_DB "/var/index.txt"
#define USER_NAME_LEN 30
#define ONE_DAY_SECONDS 86400
 
typedef struct _CertsDbEntry_ {
        unsigned char userName[USER_NAME_LEN];  //user name of this user cert
        unsigned long serial;                   //serial of this cert
        unsigned short validDays;               //total valid days of this cert
        unsigned short validDaysLeft;           //the left valid days of this cert
        unsigned char certType;                 //0(default): X.509; others: reserved
        unsigned char certStatus;               //0(default): valid; 1: expired; 2: revoked
} CERTS_DB_ENTRY_T, *CERTS_DB_ENTRY_Tp;
/////////////////////////////////////////

/* Flash store format: wapi area header */
typedef struct _wapiAreaHeader_ {
	unsigned char signature[SIG_LEN];
	unsigned long fileNum;			//Number of user certs at wapi area
	unsigned long nextSerial;		//reserve serial for next user cert
	unsigned long nextCrlNumber;	//reserve crl number for next ca crl
} WAPI_AREA_HEADER_T, *WAPI_AREA_HEADER_Tp;

/* Flash store format: wapi file header */
typedef struct _wapiFileHeader_ {
	unsigned long fileType;	// 1: CA cert (not include CA private key), 2: CA private key, 3: CA crl
							// 4: user cert (include user private key), 5: index.txt (database for crl)
	unsigned long fileSerial;	//serial of certificate, only used for user certificates
	unsigned long fileLen;		//lenght of this wapi file
} WAPI_FILE_HEADER_T, *WAPI_FILE_HEADER_Tp;

int isFileExist(const char * filename);
off_t list(const char *name, char ** outFile, int * outFileNum);
int getHexFromFile(const char *srcFile, unsigned long *outHex);
unsigned long getUserCertNum(unsigned long * userCertNum);

///////////////////////////////
//For parse certs database: index.txt
int getCertsDb(CERTS_DB_ENTRY_Tp certsInfo, int count);
/////////////////////////////////////
#endif //INCLUDE_WAPIFILES_H

