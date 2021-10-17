#ifndef __RTK_CGI_ADAPTER_H__
#define __RTK_CGI_ADAPTER_H__
#include "rtk_api.h"

#define TMEP_CGI_OUT_FILE "/tmp/rtk_cgi_file_out"
typedef struct nas_api_info_ {
	char version[RTK_VERSION_LEN];
}__attribute__ ((packed)) RTK_NAS_API_INFO_T, *RTK_NAS_API_INFO_Tp;

#endif
