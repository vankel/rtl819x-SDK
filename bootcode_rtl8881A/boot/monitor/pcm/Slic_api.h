#ifndef _SLIC_API_H_
#define _SLIC_API_H_

#include <linux/config.h>
#define PHONE_ON_HOOK		0
#define PHONE_OFF_HOOK		1
#define PHONE_FLASH_HOOK	2
#define PHONE_STILL_ON_HOOK	3
#define PHONE_STILL_OFF_HOOK	4
#define PHONE_UNKNOWN		5
#define FXO_ON_HOOK		6
#define FXO_OFF_HOOK		7
#define	FXO_FLASH_HOOK		8
#define FXO_STILL_ON_HOOK	9
#define FXO_STILL_OFF_HOOK	10
#define FXO_RING_ON		11
#define FXO_RING_OFF		12
#define FXO_BUSY_TONE		13
#define FXO_CALLER_ID		14
#define FXO_RING_TONE_ON	15
#define FXO_RING_TONE_OFF	16
#define FXO_POLARITY_REVERSAL	17
#define FXO_UNKNOWN		18

#define SLIC_PCM_OFF		0
#define SLIC_PCM_ON		1

#define FSK_Bellcore	0
#define FSK_ETSI	1
#define FSK_BT		2
#define FSK_NTT		3


typedef enum
{
	PCMMODE_LINEAR,
	PCMMODE_ALAW,
	PCMMODE_ULAW
} PCM_MODE;

typedef enum
{
    COUNTRY_USA,
    COUNTRY_UK,
    COUNTRY_AUSTRALIA,
    COUNTRY_HK,
    COUNTRY_JP,
    COUNTRY_SE,
    COUNTRY_GR,
    COUNTRY_FR,
    COUNTRY_TW,
    COUNTRY_BE,
    COUNTRY_FL,
    COUNTRY_IT,
    COUNTRY_CN,
    COUNTRY_CUSTOME
}COUNTRY;


typedef struct {
	unsigned char CH;		// CH = 0 ~ 3
	unsigned char ring_set;		// Ring_ON: ring_set = 1 ,  Ring_OFF: ring_set = 0

} ring_struct;

typedef struct {
	unsigned char CH;		// CH:0 - 3
	unsigned char change;		// 1: Change. 0: No-Change
	unsigned char hook_status;	// 1: Off-Hook, 0: On-Hook
} hook_struck;

/*Slic_api.c function prototype*/
void SLIC_reset(int CH, int codec_law, unsigned char slic_number);
void CID_for_FSK_HW(unsigned int chid, char mode, unsigned char msg_type, char *str, char *str2, char *cid_name);
void CID_for_FSK_SW(unsigned int chid, char mode, unsigned char msg_type, char *str, char *str2, char *cid_name);
void FXS_Ring(ring_struct *ring);
unsigned char FXS_Check_Ring(ring_struct *ring);
void Hook_state(hook_struck *hook);
void Set_SLIC_Tx_Gain(unsigned char chid, unsigned char tx_gain);
void Set_SLIC_Rx_Gain(unsigned char chid, unsigned char rx_gain);
void SLIC_Set_Ring_Cadence_ON(unsigned char Slic_order, unsigned short Msec);
void SLIC_Set_Ring_Cadence_OFF(unsigned char Slic_order, unsigned short Msec);
void Init_Event_Polling_Use_Timer(void);
void SLIC_Hook_Polling(hook_struck *hook, unsigned int fhk_min_time, unsigned int fhk_time);
void OnHookLineReversal(int chid, unsigned char bReversal);
void SendNTTCAR(int chid);
void disableOscillators(unsigned int chid);
void SetOnHookTransmissionAndBackupRegister(int chid);
void RestoreBackupRegisterWhenSetOnHookTransmission(unsigned int chid);
void SLIC_Set_Impendance(unsigned char chid, unsigned short country, unsigned short impd);
void SLIC_Set_PCM_state(int chid, int enable);
void SLIC_gen_FSK_CID(unsigned int chid, char mode, unsigned char msg_type, char *str, char *str2, char *cid_name);
int SLIC_gen_VMWI(unsigned int chid, char *str);
void ring_det_cad_set( unsigned int cad_on_msec, unsigned int cad_off_msec);
void ring_times_set(unsigned int chid, unsigned int ringOn, unsigned int ringOff);
void vir_daa_ring_det_set(unsigned int on_ths, unsigned int off_ths);
#ifdef PULSE_DIAL_DET
void set_pulse_det(unsigned int chid, unsigned int enable, unsigned int pause_time);
unsigned int get_pulse_det(unsigned int chid);
#endif

/*Slic_api.c variable extern*/
extern char fsk_cid_state[];
extern char ntt_skip_dc_loop[];
extern volatile char fsk_alert_state[];
extern volatile char fsk_alert_time[];

extern char fsk_spec_areas[];

#endif	/*end _SLIC_API_H_ */
