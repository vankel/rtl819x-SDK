#ifndef __CODEC_DEF_H__
#define __CODEC_DEF_H__

#include "rtk_voip.h"

// Note: Don't use this enum, plz use variable nCodecTypeID_XXX in codec_descriptor.c
enum START_CODEC_TYPE 
{
	CODEC_TYPE_G711, 
#ifdef CONFIG_RTK_VOIP_G7231
	CODEC_TYPE_G7231, 
#endif
#ifdef CONFIG_RTK_VOIP_G729AB
	CODEC_TYPE_G729,
#endif
#ifdef CONFIG_RTK_VOIP_G726
	CODEC_TYPE_G726,
#endif
#ifdef CONFIG_RTK_VOIP_GSMFR
	CODEC_TYPE_GSMFR,
#endif
#ifdef CONFIG_RTK_VOIP_ILBC
	CODEC_TYPE_ILBC30MS,
	CODEC_TYPE_ILBC20MS,
#endif
#ifdef CONFIG_RTK_VOIP_G722
	CODEC_TYPE_G722,
#endif
#ifdef CONFIG_RTK_VOIP_G7111
	CODEC_TYPE_G7111NB,
	CODEC_TYPE_G7111WB,
#endif
#ifdef CONFIG_RTK_VOIP_AMR_NB
	CODEC_TYPE_AMR_NB,
#endif
#ifdef CONFIG_RTK_VOIP_AMR_WB
	CODEC_TYPE_AMR_WB,
#endif
#ifdef CONFIG_RTK_VOIP_T38
	CODEC_TYPE_T38,
#endif
#ifdef CONFIG_RTK_VOIP_SPEEX_NB
	CODEC_TYPE_SPEEX_NB,
#endif
#ifdef CONFIG_RTK_VOIP_PCM_LINEAR_8K
	CODEC_TYPE_PCM_LINEAR_8K,
#endif
#ifdef CONFIG_RTK_VOIP_PCM_LINEAR_16K
	CODEC_TYPE_PCM_LINEAR_16K,
#endif
#ifdef CONFIG_RTK_VOIP_SILENCE
	CODEC_TYPE_SILENCE,
#endif
	CODEC_TYPE_UNKNOW,
	CODEC_TYPE_NUMBER = CODEC_TYPE_UNKNOW,
};

// what kind of coding algorism
// Note: Don't use this enum, plz use variable nCodecAlgorithm_XXX in codec_descriptor.c
typedef enum
{
    DSPCODEC_ALGORITHM_G711U,
    DSPCODEC_ALGORITHM_G711A,
#ifdef CONFIG_RTK_VOIP_G7231
    DSPCODEC_ALGORITHM_G7231A53,
    DSPCODEC_ALGORITHM_G7231A63,
#endif
#ifdef CONFIG_RTK_VOIP_G729AB
    DSPCODEC_ALGORITHM_G729,
#endif
#ifdef CONFIG_RTK_VOIP_G726
    DSPCODEC_ALGORITHM_G72616,
    DSPCODEC_ALGORITHM_G72624,
    DSPCODEC_ALGORITHM_G72632,
    DSPCODEC_ALGORITHM_G72640,
#endif
#ifdef CONFIG_RTK_VOIP_GSMFR
	DSPCODEC_ALGORITHM_GSMFR,
#endif
#ifdef CONFIG_RTK_VOIP_ILBC
	DSPCODEC_ALGORITHM_ILBC30MS,
	DSPCODEC_ALGORITHM_ILBC20MS,
#endif
#ifdef CONFIG_RTK_VOIP_G722
    DSPCODEC_ALGORITHM_G72248,
    DSPCODEC_ALGORITHM_G72256,
    DSPCODEC_ALGORITHM_G72264,
#endif
#ifdef CONFIG_RTK_VOIP_G7111
    DSPCODEC_ALGORITHM_G7111R1U,
    DSPCODEC_ALGORITHM_G7111R2aU,
    DSPCODEC_ALGORITHM_G7111R2bU,
    DSPCODEC_ALGORITHM_G7111R3U,
    DSPCODEC_ALGORITHM_G7111R1A,
    DSPCODEC_ALGORITHM_G7111R2aA,
    DSPCODEC_ALGORITHM_G7111R2bA,
    DSPCODEC_ALGORITHM_G7111R3A,
#endif
#ifdef CONFIG_RTK_VOIP_AMR_NB
	DSPCODEC_ALGORITHM_AMR_NB,	
#endif
#ifdef CONFIG_RTK_VOIP_AMR_WB
	DSPCODEC_ALGORITHM_AMR_WB_6P6,	
	DSPCODEC_ALGORITHM_AMR_WB_8P85,
	DSPCODEC_ALGORITHM_AMR_WB_12P65,
	DSPCODEC_ALGORITHM_AMR_WB_14P25,
	DSPCODEC_ALGORITHM_AMR_WB_15P85,
	DSPCODEC_ALGORITHM_AMR_WB_18P25,
	DSPCODEC_ALGORITHM_AMR_WB_19P85,
	DSPCODEC_ALGORITHM_AMR_WB_23P05,
	DSPCODEC_ALGORITHM_AMR_WB_23P85,
#endif
#ifdef CONFIG_RTK_VOIP_T38
	DSPCODEC_ALGORITHM_T38,
#endif
#ifdef CONFIG_RTK_VOIP_SPEEX_NB
	DSPCODEC_ALGORITHM_SPEEX_NB_2P15,
	DSPCODEC_ALGORITHM_SPEEX_NB_5P95,
	DSPCODEC_ALGORITHM_SPEEX_NB_8,
	DSPCODEC_ALGORITHM_SPEEX_NB_11,
	DSPCODEC_ALGORITHM_SPEEX_NB_15,
	DSPCODEC_ALGORITHM_SPEEX_NB_18P2,
	DSPCODEC_ALGORITHM_SPEEX_NB_24P6,
	DSPCODEC_ALGORITHM_SPEEX_NB_3P95,
#endif
#ifdef CONFIG_RTK_VOIP_PCM_LINEAR_8K
	DSPCODEC_ALGORITHM_PCM_LINEAR_8K,
#endif
#ifdef CONFIG_RTK_VOIP_PCM_LINEAR_16K
	DSPCODEC_ALGORITHM_PCM_LINEAR_16K,
#endif
#ifdef CONFIG_RTK_VOIP_SILENCE
	DSPCODEC_ALGORITHM_SILENCE,
#endif
    DSPCODEC_ALGORITHM_UNKNOW,
    DSPCODEC_ALGORITHM_NUMBER = DSPCODEC_ALGORITHM_UNKNOW,
} DSPCODEC_ALGORITHM;

enum CODEC_STATE
{
	INVALID,
	ENCODE,
	DECODE,
	BOTH
};

#endif /* __CODEC_DEF_H__ */
