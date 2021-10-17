#ifndef _EC_COMMON_H_
#define _EC_COMMON_H_

#include "voip_types.h"

#define NLP_SMOOTHING
//#define NLP_SMOOTH_DEBUG

#ifdef NLP_SMOOTHING
extern short Attack_Stepsize[MAX_DSP_RTK_CH_NUM];
extern short Attack_Stepsize_C[MAX_DSP_RTK_CH_NUM];
extern short Release_Stepsize[MAX_DSP_RTK_CH_NUM];
extern short Release_Stepsize_C[MAX_DSP_RTK_CH_NUM];
extern short gain_factor[MAX_DSP_RTK_CH_NUM];
extern aec_count[MAX_DSP_RTK_CH_NUM];                                               
extern aec_cng_level[MAX_DSP_RTK_CH_NUM];
extern aec_smooth_gain_th[MAX_DSP_RTK_CH_NUM];
extern lec_cng_level[MAX_DSP_RTK_CH_NUM];
extern lec_smooth_gain_th[MAX_DSP_RTK_CH_NUM];
#endif

// function prototype of ec_common.c
int32 EC168_Enable(uint32 chid);
int32 EC168_Disable(uint32 chid);
int32 EC168_GetOnState(uint32 chid);
int32 EC168_SetHighVbdAutoMode(uint32 chid, uint32 mode);
int32 EC168_GetHighVbdAutoMode(uint32 chid);
int32 EC168_SetVbdAutoMode(uint32 chid, uint32 mode);
int32 EC168_GetVbdAutoMode(uint32 chid);
int32 EC168_SetCngFlag(uint32 chid, uint32 flag);
int32 EC168_GetCngFlag(uint32 chid);

#endif
