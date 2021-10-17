#if !defined(_LEC_H_)
#define _LEC_H_

#include "../../include/voip_types.h"

/* LEC Mode Mask */
#define LEC             0x01	/* LEC */
#define LEC_NLP   	0x02	/* LEC+NLP */
#define LEC_NLP_MUTE  	0x04	/* LEC+NLP_mute */
#define LEC_NLP_CNG     0x08	/* LEC+NLP_CNG */
#define LEC_NLP_SHIFT   0x10	/* LEC+NLP_shift */

#ifdef CONFIG_RTK_VOIP_MODULE
#define DMEN_LEC	0
#define ASM_LEC		1
#define ASM_FIR		1
#define DMEN_STACK_LEC  0
#else
#define DMEN_LEC	0
#define ASM_LEC		1
#define ASM_FIR		1
#define DMEN_STACK_LEC  0
#endif

typedef struct {
	unsigned int ec_select;	// 1:Freq-domain LEC   0:Time-domain LEC
	/* Echo cancellation callback functions */
	void ( *EC_G168Process)(unsigned char chid, const Word16 *pRin, Word16 *pSin, Word16 *pEx); 	
	void ( *EC_G168Init)(unsigned char chid, unsigned char type);
	void ( *EC_G168ReInit)(unsigned char chid);
	void ( *EC_G168SetTailLength)(unsigned int chid, unsigned int tail_length);
	int  ( *EC_G168VbdAuto)(char chid, int vbd_high, int vbd_low, int lec_bk);
	int  ( *EC_G168Enable)(unsigned char chid);
	int  ( *EC_G168Disable)(unsigned char chid);
	int  ( *EC_G168SetNlp)(unsigned char chid, unsigned char mode); 	
#ifdef SUPPORT_RTCP_XR
	unsigned int ( *EC_G168GetErle)(unsigned int chid);
#endif
	void ( *EC_G168NlpConfig)(unsigned char chid, int nlp_sensitivity);
	void ( *EC_G168NlpInit)(unsigned int chid, int at_stepsize, int rt_stepsize, unsigned int cng_flag);	
	void ( *EC_G168NlpCngSmoothConfig)(unsigned int chid, unsigned int cng_level, int smooth_gain_threshold);
} EcObj_t;

extern EcObj_t RtkEcObj[];
extern const EcObj_t LecFreqDomainObj;
extern const EcObj_t LecTimeDomainObj;

/* EC function calls prototype */

/*  sensitivity range :3 ~ 8
	The smaller sensitivity value, the large NLP threshold, and the worse double talk performance.
	The larger sensitivity value, the smaller NLP threshold, and better double talk performance.But the worse LEC performance.
	Suggested value is 6.
*/

int32 EC168_SetOnState(uint32 chid, uint32 state);

int32 EC168_GetOnState(uint32 chid);

int32 EC168_OnOffCheck(uint32 chid);

int32 EC168_GetFlag(uint32 chid);

int32 EC168_RestoreFlag(uint32 chid, uint32 flag);

int32 EC168_GetHighVbdAutoMode(uint32 chid);

int32 EC168_GetVbdAutoMode(uint32 chid);

#endif

