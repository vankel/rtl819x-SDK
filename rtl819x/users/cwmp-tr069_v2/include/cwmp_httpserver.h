#ifndef _CWMP_HTTPSERVER_H_
#define _CWMP_HTTPSERVER_H_

#include "soapH.h"

int cwmp_webserver_init( struct soap *web_soap, void *data );
int cwmp_webserver_loop( struct soap *web_soap );

#endif /*_CWMP_HTTPSERVER_H_*/
