#ifndef __RTK_SYSLOG_H__
#define __RTK_SYSLOG_H__
#include <syslog.h>
//#define fprintf(std, ...) do{ if((std)==stderr || (std)==stdout) { syslog(LOG_INFO, __VA_ARGS__);} fprintf((std), __VA_ARGS__); }while(0)
#define printf(...) do{syslog(LOG_INFO, __VA_ARGS__);printf(__VA_ARGS__); }while(0)
#endif
