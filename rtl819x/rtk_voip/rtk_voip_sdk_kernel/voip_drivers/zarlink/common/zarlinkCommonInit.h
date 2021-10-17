
#ifndef __ZARLINKCOMMONINIT_H__
#define __ZARLINKCOMMONINIT_H__

#include "zarlinkCommon.h"

BOOL zarlinkInitDevice( RTKDevObj *pDev );
int zarlinkCaculateDevObj(RTKDevType dev_type);
int zarlinkRegDevForEvHandle(RTKDevObj * pDev);
int rtkGetNewChID(void);
RTKLineObj * rtkGetLine(int chid);
RTKDevObj * rtkGetDev(int chid);
void * rtkDumpReg( RTKDevObj *pDev );
void * rtkDumpObj( RTKDevObj *pDev );
void * rtkRxGain( RTKDevObj *pDev, int gain );
#endif /* __ZARLINKCOMMONINIT_H__ */


