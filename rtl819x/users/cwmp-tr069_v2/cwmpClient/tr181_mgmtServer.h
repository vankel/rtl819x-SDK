#ifndef _TR181_MGMTSERVER_H_
#define _TR181_MGMTSERVER_H_

#include "tr181_device.h"

#ifdef __cplusplus
extern "C" {
#endif

extern struct CWMP_LEAF tMgmtServerLeaf[];

int getMgmtServer(char *name, struct CWMP_LEAF *entity, int *type, void **data);
int setMgmtServer(char *name, struct CWMP_LEAF *entity, int type, void *data);

void cwmpStartStun();
void cwmpStopStun();

#ifdef __cplusplus
}
#endif

#endif /*_TR181_MGMTSERVER_H_*/
