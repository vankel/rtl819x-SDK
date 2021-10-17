
#ifndef __VE886_API_H__
#define __VE886_API_H__

#include "zarlinkCommon.h"

VpStatusType Ve886SetRingCadenceProfile(RTKLineObj *pLine, uint8 ring_cad);
VpStatusType Ve886SetImpedenceCountry(RTKLineObj *pLine, uint8 country);
VpStatusType Ve886SetFxsAcProfileByBand(RTKLineObj *pLine, int pcm_mode);

VpStatusType Ve886UpdIOState(RTKLineObj *pLine);
VpStatusType Ve886SetIOState(RTKLineObj *pLine, VPIO IO, int bHigh );
VpStatusType Ve886GetIOState(RTKLineObj *pLine, VPIO IO, int *bHigh);
VpStatusType Ve886SetIODir(RTKLineObj *pLine, VPIO IO, int bOut);

#if 0
VpProfilePtrType Ve886RingProfile(uint8 profileId);
VpProfilePtrType Ve886AcProfile(uint8 profileId);
#endif

/********** DAA Function **********/

#endif /* __VE886_API_H__*/

