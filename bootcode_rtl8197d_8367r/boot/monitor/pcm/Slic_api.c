#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/sched.h>
#include <linux/delay.h>
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM8672
#include <linux/interrupt.h>
#endif
#include "Slic_api.h"
#include "rtk_voip.h"
#include "spi.h"
#include "type.h"
#include "voip_control.h"
#include "Daa_api.h"
#ifdef AUDIOCODES_VOIP
#include "RTK_AC49xApi_Interface.h"
#include "Ntt_sRing_det.h"
#else
#include "fsk.h"
#endif
#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
#include "w682388.h"
#endif
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
#include "si3210init.h"
#endif
#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226
#include "proslic_api/proslic_datatypes.h"
#include "proslic_api/proslic.h"
#include "proslic_api/sys_driv_type.h"


#define CHAN_PER_DEVICE 2
#define NUMBER_OF_CHAN 2
#define NUMBER_OF_PROSLIC NUMBER_OF_CHAN/CHAN_PER_DEVICE

typedef struct chanStatus chanState; //forward declaration

typedef void (*procState) (chanState *pState, ProslicInt eInput);


/*
** structure to hold state information for pbx demo
*/
struct chanStatus {
	proslicChanType *ProObj;
	timeStamp onHookTime;
	timeStamp offHookTime;
	procState currentState;
	uInt16 digitCount;
	uInt8 digits[20];
	uInt8 ringCount;
	uInt16 connectionWith;
	uInt16 powerAlarmCount;
	pulseDialType pulseDialData;
	BOOLEAN eventEnable;
} ;

ctrl_S spiGciObj; //spi interface object
//systemTimer_S timerObj;    //timer object
controlInterfaceType *ProHWIntf; //proslic hardware interface object
ProslicDeviceType *ProSLICDevices[NUMBER_OF_PROSLIC]; //proslic device object
proslicChanType_ptr arrayOfProslicChans[NUMBER_OF_CHAN]; //used for initialization only
chanState ports[NUMBER_OF_CHAN];  //declare channel state structures
proslicChanType *pSlic;

#endif // CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226


#ifdef AUDIOCODES_VOIP
TstCidDet cid_res[MAX_VOIP_CH_NUM] = {0};
#endif

static struct timer_list event_timer;
unsigned int flash_hook_time[MAX_VOIP_CH_NUM]; /* flash_hook_time * 10ms */
unsigned int flash_hook_min_time[MAX_VOIP_CH_NUM]; /* flash_hook_min_time * 10ms */
extern int hook_in(uint32 ch_id, char input);

/**** Digital gain compensation for SW DTMF detector 10log, normalized by 256 *******/
int16 tx_comp[10]={511, 456, 406, 362, 322, 287, 256, 228, 203, 181}; //-6dB to 3dB
/************************************************************************************/

//======================= FSK CID =========================//
char fsk_cid_state[MAX_VOIP_CH_NUM]={0};		// for FSK CID
char ntt_skip_dc_loop[MAX_VOIP_CH_NUM]={0};
static char slic_choice_flag = 1;

//extern int Hook_Polling_Silicon(hook_struck *hook, unsigned int flash_hook_duration);
//extern unsigned char Hook_Polling_Legerity(hook_struck *hook, unsigned int flash_hook_duration);

void SLIC_reset(int CH, int codec_law, unsigned char slic_number)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	reset_SLIC(codec_law, slic_number);

#elif  CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226
	si3226_reset();

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	Legerity_hard_reset();
	//Legerity_slic_init_all(slic_number);
	Legerity_GPIO_dir_set(0,1,1);
	Legerity_GPIO_dir_set(1,1,1);
	Legerity_GPIO_data(0,1,0);
	Legerity_GPIO_data(1,1,0);
	Legerity_slic_init_all(0);
	Legerity_slic_init_all(1);
#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
	SlicInit();
#endif
}


#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226
static void si3226_reset()
{
	// This function will cause system reset, if watch dog is enable!
	// Because calibration need mdelay(1900).
	unsigned long flags;
	save_flags(flags); cli();
	*(volatile unsigned long *)(0xB800311c) &=  0xFFFFFF;	// Disable watch dog
	*(volatile unsigned long *)(0xB800311c) |=  0xA5000000;
	restore_flags(flags);
	
	si3226_init(PCMMODE_ULAW);
	
	save_flags(flags); cli();
	*(volatile unsigned long *)(0xB800311c) &=  0xFFFFFF;	// Enable watch dog
	*(volatile unsigned long *)(0xB800311c) |=  1 << 23;
	restore_flags(flags);

}

static void si3226_init(int pcm_mode)
{
	int i=0;
	int ret = 0;

	printk ("-------Si3226 Rev 0.24 ------\n");
	printk("Si3226 init....");


	spiGciObj.portID = 0; // SPI GPIO set
	SPI_Init (&spiGciObj);   //initialize SPI interface

	//initialize timer
	//TimerInit(&timerObj);

	ProSLIC_setControlInterfaceCtrlObj (ProHWIntf, &spiGciObj);

	// set control functions
	extern int ctrl_ResetWrapper (ctrl_S *hSpiGci, int status);
	ProSLIC_setControlInterfaceReset (ProHWIntf, ctrl_ResetWrapper);

	extern int ctrl_WriteRegisterWrapper (ctrl_S *hSpiGci, uInt8 channel, uInt8 regAddr, uInt8 data);
	ProSLIC_setControlInterfaceWriteRegister (ProHWIntf, ctrl_WriteRegisterWrapper);
	extern uInt8 ctrl_ReadRegisterWrapper (ctrl_S *hSpiGci, uInt8 channel, uInt8 regAddr);
	ProSLIC_setControlInterfaceReadRegister (ProHWIntf, ctrl_ReadRegisterWrapper);

	extern int ctrl_WriteRAMWrapper (ctrl_S *hSpiGci, uInt8 channel, uInt16 ramAddr, ramData data);
	ProSLIC_setControlInterfaceWriteRAM (ProHWIntf, ctrl_WriteRAMWrapper);
	extern ramData ctrl_ReadRAMWrapper (ctrl_S *hSpiGci, uInt8 channel, uInt16 ramAddr);
	ProSLIC_setControlInterfaceReadRAM (ProHWIntf, ctrl_ReadRAMWrapper);


	// set timer functions
	//ProSLIC_setControlInterfaceTimerObj (ProHWIntf, &timerObj);
	extern int time_DelayWrapper (void *hTimer, int timeInMs);
	ProSLIC_setControlInterfaceDelay (ProHWIntf, time_DelayWrapper);

	extern int time_TimeElapsedWrapper (void *hTimer, timeStamp *startTime, int *timeInMs);
	ProSLIC_setControlInterfaceTimeElapsed (ProHWIntf, time_TimeElapsedWrapper);

	extern int time_GetTimeWrapper (void *hTimer, timeStamp *time);
	ProSLIC_setControlInterfaceGetTime (ProHWIntf, time_GetTimeWrapper);

	ProSLIC_setControlInterfaceSemaphore (ProHWIntf, NULL);

	ProSLIC_Reset(ports[0].ProObj);	//Reset the ProSLIC(s) before we begin
	for (i=0;i<NUMBER_OF_CHAN;i++){
		arrayOfProslicChans[i] = ports[i].ProObj; //create array of channel pointers (for init)
		//ProSLIC_setSWDebugMode (ports[i].ProObj,TRUE);
	}
	//ProSLIC_Init(arrayOfProslicChans,NUMBER_OF_CHAN);
	ret = ProSLIC_InitBroadcast(ports[0].ProObj);
	if (ret != 0)
		goto si3226_err;
	//Initialize the channel state for each channel
	for (i=0;i<NUMBER_OF_CHAN;i++){
		pSlic = ports[i].ProObj;
		//InitStateMachine(&(ports[i])); //initialize the call state machine
		ProSLIC_InitializeDialPulseDetect(&(ports[i].pulseDialData),&(ports[i].offHookTime),&(ports[i].onHookTime));
	}


	for (i=0;i<NUMBER_OF_CHAN;i++)
	{
		pSlic = ports[i].ProObj;
		ProSLIC_SetLinefeedStatus(pSlic,LF_FWD_ACTIVE);

		ProSLIC_PCMTimeSlotSetup(pSlic, 1+i*8, 1+i*8);
		ProSLIC_PCMSetup(pSlic, pcm_mode);
		ProSLIC_RingSetup(pSlic, 0);
		ProSLIC_DCFeedSetup(pSlic, 0);

#if 0
		/* Disable Si3226 Automatic Common Mode Control */
		extern void W_reg(unsigned char chid, unsigned char regaddr, unsigned char data);
		SI3226_SetUserMode(pSlic, 1);	// enable user mode
		W_reg(i, 80, 0x1F);		// set register 80 to 0x1F
		SI3226_SetUserMode(pSlic, 0);	// disable user mode
		PRINT_MSG("Disable Si3226 Automatic Common Mode Control[%d].\n", i);
#else
		extern void W_ram(unsigned char chid, unsigned short reg, unsigned int data);
		W_ram(i, 750, 0x624F00); // skip Vbat tracking below 6V Vtr variation
#endif
	}

	printk("Si3226 init OK!\n");
	return;
si3226_err:
	printk("Si3226 init error!\n");
	mdelay(3000);
}

static void si3226_alloc_objs()
{
	int i;
	ProSLIC_createControlInterface(&ProHWIntf);
	for (i=0;i<NUMBER_OF_PROSLIC;i++)
		ProSLIC_createDevice (&(ProSLICDevices[i]));
	for (i=0;i<NUMBER_OF_CHAN;i++){
		ProSLIC_createChannel(&(ports[i].ProObj));
		ProSLIC_SWInitChan (ports[i].ProObj,i,SI3226_TYPE, ProSLICDevices[i/CHAN_PER_DEVICE], ProHWIntf);
	}
}
#endif // CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226


int SLIC_init(int pcm_mode)
{
	int ch;
	extern int slic_ch_num;
	int valid_ch = slic_ch_num;

#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	init_spi(SLIC0_SPI_DEV);
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM8651
	init_spi(SLIC1_SPI_DEV);
#endif
	for (ch = 0; ch < SLIC_CH_NUM; ch++)
	{
		proslic_init_all(ch, pcm_mode, ch+1);
	}

#endif

#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226
	// create objs
	si3226_alloc_objs();
	si3226_init(pcm_mode);

#endif // CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226

#if defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	init_spi(SLIC0_SPI_DEV);

#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM8651_T
	Legerity_GPIO_dir_set(1,1,1);
	Legerity_GPIO_dir_set(2,1,1);
	Legerity_GPIO_data(1,1,0);
	mdelay(50);
	Legerity_GPIO_data(2,1,0);
#endif

	for (ch = 0; ch <slic_ch_num; ch++)
	{
		printk("===================Legerity slic %d===================\n",ch+1);

		if (Legerity_slic_init_all(ch) != 0)
			valid_ch--;

		Legerity_system_state(ch,0,1);
	}
#endif

#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
	init_spi(SLIC0_SPI_DEV);
	printk("================== Winbond slic init... ===================\n");
	SlicInit();
#endif

	return valid_ch > 0 ? 0 : -1;
}

/* mode: 0-type 1 (on-hook), 1-type 2(off-hook)*/
void CID_for_FSK_HW(unsigned int chid, char mode, unsigned char msg_type, char *str, char *str2, char *cid_name)
{
	fsk_cid_state[chid]=1;
#if ! defined (AUDIOCODES_VOIP)
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	fsk_gen(chid, mode, msg_type, str, str2, cid_name);
#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	unsigned long flags;
	save_flags(flags);
	cli();
	Legerity_FSK_CallerID_main(chid, str, str2, cid_name, mode, msg_type);//This Function only HW gen 2007-04-12
	restore_flags(flags);
#endif
#endif
}

TstVoipCID cid_info[VOIP_CH_NUM] = {0};

void CID_for_FSK_SW(unsigned int chid, char mode, unsigned char msg_type, char *str, char *str2, char *cid_name)
{
	
#if ! defined (AUDIOCODES_VOIP)
	if ( fsk_cid_state[chid] == 0)
	{
		cid_info[chid].ch_id = chid;			// Channel ID
		cid_info[chid].daa_id = 0;			// Reserve
		cid_info[chid].cid_state = 1;			// CID State
		cid_info[chid].cid_gain = 1;			// Reserve
		cid_info[chid].cid_dtmf_mode = 0;		// Reserve
		cid_info[chid].cid_mode = mode;			// Service Type 
		cid_info[chid].cid_msg_type = msg_type;		// Message Type
		if (str != NULL)
			strcpy(cid_info[chid].string, str);	// Number
		if (str2 != NULL)	
			strcpy(cid_info[chid].string2, str2);	// Date and time
		if (cid_name != NULL)
			strcpy(cid_info[chid].cid_name, cid_name);// Name
		
		fsk_cid_state[chid] = 1;
	}
	else
	{
		// If fsk CID is during transmission, ignore new one.
	}
	
#endif
}


extern unsigned int fsk_gen_mode[];	// 0: hardware FSK caller id, 1:software FSK caller id

void SLIC_gen_FSK_CID(unsigned int chid, char mode, unsigned char msg_type, char *str, char *str2, char *cid_name)
{
#ifndef AUDIOCODES_VOIP
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	if (fsk_gen_mode[chid] == 0)// HW
		CID_for_FSK_HW(chid, mode, msg_type , str, str2, cid_name);
	else if (fsk_gen_mode[chid] == 1) // SW
		CID_for_FSK_SW(chid, mode, msg_type , str, str2, cid_name);

#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221 || defined CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226
	/* HW FSK CID gen is not supported, so it always use SW FSK CID gen. */
	CID_for_FSK_SW(chid, mode, msg_type , str, str2, cid_name);
#endif
#endif

}

int SLIC_gen_VMWI(unsigned int chid, char *str)
{
#ifndef AUDIOCODES_VOIP
	/* If CID mode is DTMF mode, don't gen VMWI message.*/
	if ((fsk_spec_areas[chid]&7) == 4)//DTMF mode
	{
		PRINT_MSG("\x1B[31mCID mode isn't FSK type for ch%d, don't support VMWI.\x1B[0m\n", chid);
		return 0;
	}
	if (fsk_gen_mode[chid] == 0)// HW
	{
		CID_for_FSK_HW(chid, 0 /*type 1*/, FSK_MSG_MWSETUP, str, NULL, NULL);
	}
	else if (fsk_gen_mode[chid] == 1)// SW
	{
		init_softfskcidGen(chid);
		CID_for_FSK_SW(chid, 0 /*type 1*/, FSK_MSG_MWSETUP, str, NULL, NULL);
	}
	return 1;
#endif
}

void FXS_Ring(ring_struct *ring)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	Ring_FXS_Silicon(ring);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226)
	pSlic = ports[ring->CH].ProObj;
	if (ring->ring_set)
		ProSLIC_RingStart(pSlic);
	else
		ProSLIC_RingStop(pSlic);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	Legerity_FXS_ring(ring);

#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
	WINBOND_FXS_ring(ring);
#endif
}

unsigned char FXS_Check_Ring(ring_struct *ring)
{
	unsigned char ringer; //0: ring off, 1: ring on

#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	ringer = Check_Ring_FXS_Silicon(ring);

#elif defined (CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226)
	pSlic = ports[ring->CH].ProObj;
	Si3226_GetLinefeedStatus(pSlic, &ringer);
	if ((ringer&0x70)==0x40)
		ringer = 1;
	else
		ringer = 0;

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	ringer = Check_Legerity_FXS_ring(ring);

#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
	ringer = Check_WINBOND_FXS_ring(ring);

#endif

	return ringer;
}

void Set_SLIC_Tx_Gain(unsigned char chid, unsigned char tx_gain)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	SLIC_Set_Tx_Gain_Silicon(chid, tx_gain);

#elif defined (CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226)
	printk("Not implemented! Support unity gain only!\n");

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	Legerity_TX_slic_gain(chid, tx_gain);

#endif

}

void Set_SLIC_Rx_Gain(unsigned char chid, unsigned char rx_gain)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	SLIC_Set_Rx_Gain_Silicon(chid, rx_gain);

#elif defined (CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226)
	printk("Not implemented! Support unity gain only!\n");

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	Legerity_RX_slic_gain(chid, rx_gain);

#endif

}
#ifdef CONFIG_RTK_VOIP_DRIVERS_VIRTUAL_DAA
extern char relay_2_PSTN_flag[];
#endif

static unsigned int on_cnt[4] = {0};
static unsigned char pre_status[4]={0}, on_set[4]={0}, off_set[4]={0}, check_flash[4] = {1, 1, 1, 1};

/*
* on_cnt: count how many time "ON-HOOK" event happens.
* on_set: if "ON-HOOK" event happens, on_set is set to 1.
* off_set: if "OFF-HOOK" event happens, off_set is set to 1.
* check_flash: if check_flash is 1, it means Hook_Polling_Silicon() need to detect FLASH HOOK event.
* pre_status: record previous hook status (only record 1: off-hook and 0: on-hook)
*/


#ifdef PULSE_DIAL_DET
/* PULSE_DIAL_PAUSE_TIME: the minimum dead time between adjacent dial pulse trains 
 * ex. If the pause duration of two pulse trains(digit 3 and digit 4) is smaller than PULSE_DIAL_PAUSE_TIME,
 * then the detection result will be digit 7 (3+4).
 */
//#define PULSE_DIAL_PAUSE_TIME 45
static unsigned int pulse_dial_pause_time = 45;
unsigned long connect_cnt[4];
unsigned long disconnect_cnt[4];
unsigned long pulse_cnt[4];
static unsigned int pulse_det_flag[VOIP_CH_NUM] = {0};
static unsigned int break_ths = 10;// threshold: break_ths*10 msec

void set_pulse_det(unsigned int chid, unsigned int enable, unsigned int pause_time)
{
	pulse_det_flag[chid] = enable;
	pulse_dial_pause_time = pause_time/10;
	//PRINT_Y("pulse_det_flag[%d]=%d\n", chid, pulse_det_flag[chid]);
}

unsigned int get_pulse_det(unsigned int chid)
{
	return pulse_det_flag[chid];
}

#endif

static unsigned int stop_poll[MAX_VOIP_CH_NUM] = {0}, stop_poll_cnt[MAX_VOIP_CH_NUM] = {0};

static unsigned int previous_daa_polarity[MAX_VOIP_CH_NUM];
static unsigned int daa_polarity;
static unsigned int pre_daa_status[MAX_VOIP_CH_NUM];

void SLIC_Hook_Polling(hook_struck *hook, unsigned int fhk_min_time, unsigned int fhk_time)
{
	static unsigned int flash_hook_min_duration[MAX_VOIP_CH_NUM], flash_hook_duration[MAX_VOIP_CH_NUM];
	unsigned char status;
	unsigned long flags;
	extern int Is_DAA_Channel(int chid);

	flash_hook_min_duration[hook->CH] = fhk_min_time;
	flash_hook_duration[hook->CH] = fhk_time;

	save_flags(flags);
	cli();

#ifdef CONFIG_RTK_VOIP_DRIVERS_FXO
	if (Is_DAA_Channel(hook->CH) ==1)
	{
		status = DAA_Hook_Status(hook->CH);
		if(status) /* off-hook */
		{
			if(pre_daa_status[hook->CH])
			{
				daa_polarity = DAA_Get_Polarity(hook->CH) ;
				if (daa_polarity != previous_daa_polarity[hook->CH])
					hook_in(hook->CH, FXO_POLARITY_REVERSAL);
				previous_daa_polarity[hook->CH] = daa_polarity;
			}
			else
			{
				previous_daa_polarity[hook->CH] = DAA_Get_Polarity(hook->CH) ;
			}
		}
		pre_daa_status[hook->CH] = status;
	}
	else
#endif
	{

#ifdef CONFIG_RTK_VOIP_DRIVERS_VIRTUAL_DAA
		if (relay_2_PSTN_flag[hook->CH]==1)
		{
			status = virtual_daa_hook_detect(hook->CH); /* 1:off-hook  0:on-hook */
		}
		else
#endif
		{
#ifdef CONFIG_RTK_VOIP_DRIVERS_IP_PHONE
			status = 0;	//iphone_hook_detect();	/* ui process this key. */
#else
			status = SLIC_Get_Hook_Status(hook->CH); /* 1:off-hook  0:on-hook */

			if ((fsk_spec_areas[hook->CH]&7)==FSK_NTT)
			{
				if (fsk_cid_state[hook->CH] || ntt_skip_dc_loop[hook->CH] )    //when send caller id ignore the off hook event.
				{
					stop_poll[hook->CH] = 1;
				}

				if ((stop_poll[hook->CH] == 1) && (status == 0)) //when NTT phone, off-hook -> on-hook, continue to poll.
				{
					stop_poll[hook->CH] = 0;
					stop_poll_cnt[hook->CH] = 0;
				}

				if (stop_poll[hook->CH] == 1)
				{
					status=pre_status[hook->CH];

					if (stop_poll_cnt[hook->CH]++ > 70)
					{
						stop_poll[hook->CH] = 0;
						stop_poll_cnt[hook->CH] = 0;
						PRINT_MSG("Force to start poll hook status for NTT\n");
					}
				}
			}

#endif
		}
	}
	restore_flags(flags);


	if (status)
	{
		/* on_cnt[] >= 10*flash_hook_min_duration[hook->CH] ms and
		 * on_cnt[] < 10*flash_hook_duration[hook->CH] ms,
		 * then flash event happen.
		 */
		if (check_flash[hook->CH] == 1)
		{
			if (on_cnt[hook->CH] <= flash_hook_min_duration[hook->CH])
			{
				if (!(off_set[hook->CH]))
				{
					hook->hook_status = PHONE_OFF_HOOK;
					off_set[hook->CH] = 1;
					on_set[hook->CH] = 0;
					//printk("*** OFF ***\n");
				}
				else
				{
					hook->hook_status = PHONE_STILL_ON_HOOK;
					//printk("*** S-ON 1***\n");
				}

			}
			else if ((on_cnt[hook->CH] >= flash_hook_min_duration[hook->CH]) && (on_cnt[hook->CH] < flash_hook_duration[hook->CH]))
			{
				hook->hook_status = PHONE_FLASH_HOOK;
				//printk("*** FLASH ***\n");
			}
			else
			{
				if (!(off_set[hook->CH]))
				{
					hook->hook_status = PHONE_OFF_HOOK;
					off_set[hook->CH] = 1;
					on_set[hook->CH] = 0;
					//printk("*** OFF ***\n");
				}
				else
				{
					hook->hook_status = PHONE_STILL_ON_HOOK;
					//printk("*** S-ON 1***\n");
				}

			}

			check_flash[hook->CH] = 0;
		}
		else
		{
			hook->hook_status = PHONE_STILL_OFF_HOOK;
			//printk("*** S-OFF 1***\n");
		}

		on_cnt[hook->CH] = 0;
#ifdef PULSE_DIAL_DET
		if ( (pulse_det_flag[hook->CH] == 1) && (Is_DAA_Channel(hook->CH) != 1) )
		{
			if (disconnect_cnt[hook->CH] != 0)
			{
				//if (hook->CH == 0)
					//PRINT_Y("%d ", disconnect_cnt[hook->CH]); // print the break(on-hook) duration of a pulse
				if (disconnect_cnt[hook->CH] < break_ths)
					pulse_cnt[hook->CH]++;
				disconnect_cnt[hook->CH]=0;
			}
			else
			{
				if (connect_cnt[hook->CH] > pulse_dial_pause_time)
				{
					if (pulse_cnt[hook->CH] != 0)
					{
						if (pulse_cnt[hook->CH] == 10)
							pulse_cnt[hook->CH] = 0;
						dtmf_in(hook->CH, pulse_cnt[hook->CH]+48/*char*/);
						PRINT_MSG("pulse dial |%d|, ch= %d\n",pulse_cnt[hook->CH], hook->CH);					
					}
					pulse_cnt[hook->CH] = 0;
				}
			}
	
			connect_cnt[hook->CH]++;
		}
#endif
	}
	else
	{
		if (pre_status[hook->CH] == 1) 	/* prev = off-hook */
			check_flash[hook->CH] = 1;

		if (on_cnt[hook->CH] >= flash_hook_duration[hook->CH])
		{
			on_cnt[hook->CH] = flash_hook_duration[hook->CH]; /* avoid on_cnt[] to overflow */

			if (on_set[hook->CH] == 0)
			{
				hook->hook_status = PHONE_ON_HOOK;
				on_set[hook->CH] = 1;
				off_set[hook->CH] = 0;
				//printk("*** ON ***\n");
			}
			else
			{
				hook->hook_status = PHONE_STILL_ON_HOOK;
				//printk("*** S-ON 2***\n");
			}
		}
		else
		{
			hook->hook_status = PHONE_STILL_OFF_HOOK;
			//printk("*** S-OFF 2***\n");
		}

		//printk("%d\n", on_cnt[hook->CH]);
		on_cnt[hook->CH]++;
#ifdef PULSE_DIAL_DET
		if ( (pulse_det_flag[hook->CH] == 1) && (Is_DAA_Channel(hook->CH) != 1) )
		{
			disconnect_cnt[hook->CH]++;
			connect_cnt[hook->CH]=0;
		}
#endif
	}

	pre_status[hook->CH] = status;

	return 0;
}


void Init_Hook_Polling(unsigned char CH)
{
	check_flash[CH] = 1;
	pre_status[CH] = 0;
	on_cnt[CH] = 0;
	on_set[CH] = 0;
	off_set[CH] = 0;
}

////// Virtual DAA Ring Detection ////////
#define VIR_DAA_RING_DET_DEBUG	0		/* Enable to print the (ring_on_cnt, ring_off_cnt), enginner can fine tune the ring on/off threshold by this debug message. */
static unsigned int vir_Ring_On_Ths = 25;	/* This threshold is not mapping to sec, enginner need to tune it for desired Ring pattern.*/
static unsigned int vir_Ring_Off_Ths = 500;	/* 5 sec */
/* (50, 500)->For Ring pattern: 20Hz, AC:75 Vrms, DC:45 V, 1s on- 4s off */

void vir_daa_ring_det_set(unsigned int on_ths, unsigned int off_ths)
{
	vir_Ring_On_Ths = on_ths;
	vir_Ring_Off_Ths = off_ths;
}


////// DAA Ring Detection ////////
static unsigned int Ring_On_Cnt = 150;
static unsigned int Ring_Off_Cnt = 80;
static unsigned int Ring_Off_Event_Ths = 250;

static unsigned int ring_on_time[VOIP_CH_NUM] = {0};
static unsigned int ring_off_time[VOIP_CH_NUM] = {0};
static int ring_on[VOIP_CH_NUM] ={0};
static int ring_off[VOIP_CH_NUM] = {0};
static int wait_ring_off[VOIP_CH_NUM] = {0}, wait_ring_off_timeout[VOIP_CH_NUM] = {0};
static int check_ring_off_time_out[VOIP_CH_NUM] = {0};

void ring_det_cad_set( unsigned int cad_on_msec, unsigned int cad_off_msec)
{
	Ring_On_Cnt = cad_on_msec/10;
	Ring_Off_Cnt = cad_off_msec/10;
	Ring_Off_Event_Ths = cad_off_msec/10;

	/* Accept 12.5% deviation of cadence */
	Ring_On_Cnt = Ring_On_Cnt - (Ring_On_Cnt>>3);
	Ring_Off_Cnt = Ring_Off_Cnt - (Ring_Off_Cnt>>3);

	/* Add 12.5% deviation to Ring-off Event judgement */
	Ring_Off_Event_Ths = Ring_Off_Event_Ths + (Ring_Off_Event_Ths>>3);

	//printk("((((( %d, %d, %d )))))\n", Ring_On_Cnt*10, Ring_Off_Cnt*10, Ring_Off_Event_Ths*10);
}

void ring_times_set(unsigned int chid, unsigned int ringOn, unsigned int ringOff)
{
	ring_on_time[chid] = ringOn;
	ring_off_time[chid] = ringOff;
	//printk("============> (%d, %d)<===============\n", ring_on_time[chid], ring_off_time[chid]);
}

void Event_Polling_Use_Timer(unsigned long data)
{
	int i;
	hook_struck hook_res[MAX_VOIP_CH_NUM];
        static int daa_ring_on_cnt[MAX_VOIP_CH_NUM] = {0}, daa_ring_off_cnt[MAX_VOIP_CH_NUM] = {0}, daa_ring_flag[MAX_VOIP_CH_NUM] = {0},
        		vir_daa_ring_on_cnt = 0, vir_daa_ring_off_cnt = 0, vir_daa_ring_flag =0;
#ifdef CONFIG_RTK_VOIP_LED
#ifdef CONFIG_RTK_VOIP_DRIVERS_VIRTUAL_DAA
	extern unsigned int pstn_ringing;
#elif defined CONFIG_RTK_VOIP_DRIVERS_SI3050
	extern unsigned int pstn_ringing[];
#endif
#endif
	static int fsk_decode_complete_flag[VOIP_CH_NUM]={0}; // for NTT CID detection usage

	for (i=0; i< VOIP_CH_NUM; i++)
	{
		(&hook_res[i])->CH = i;

		SLIC_Hook_Polling(&hook_res[i], flash_hook_min_time[i], flash_hook_time[i]);

		if ( (&hook_res[i])->hook_status == PHONE_ON_HOOK)
		{
			if (Is_DAA_Channel(i) == 1)
				hook_in(i, FXO_ON_HOOK);
			else
				hook_in(i, PHONE_ON_HOOK);
			//printk("hook_in: ON\n");
		}
		else if ( (&hook_res[i])->hook_status == PHONE_OFF_HOOK)
		{
			if (Is_DAA_Channel(i) == 1)
				hook_in(i, FXO_OFF_HOOK);
			else
				hook_in(i, PHONE_OFF_HOOK);
			//printk("hook_in: OFF\n");
		}
		else if ( (&hook_res[i])->hook_status == PHONE_FLASH_HOOK)
		{
			if (Is_DAA_Channel(i) == 1)
			{
				//hook_in(i, FXO_FLASH_HOOK);
			}
			else
				hook_in(i, PHONE_FLASH_HOOK);
			//printk("hook_in: Flash\n");
		}
		else
		{
			if (Is_DAA_Channel(i) == 1)
				hook_in(i, FXO_UNKNOWN);
			else
				hook_in(i, PHONE_UNKNOWN);
		}
	}

#ifdef CONFIG_RTK_VOIP_DRIVERS_VIRTUAL_DAA

	#if VIR_DAA_RING_DET_DEBUG
		printk("%d, %d\n", vir_daa_ring_on_cnt, vir_daa_ring_off_cnt);
	#endif
	
	/**************************************************************************************************/
	/***** Reset vir_daa_ring_on_cnt=0 if vir_daa_ring_on_cnt doesn't change for 10 times polling. ******/
	
	static int k=0;
	volatile static int y;
	unsigned int reset_cnt=10;
	
	k = (k+1)%reset_cnt;
	
	if (k==0) y=vir_daa_ring_on_cnt;
	
	if (k==(reset_cnt-1))
	{
		if ( y == vir_daa_ring_on_cnt)
		{
			vir_daa_ring_on_cnt = 0;
		}
	}
	
	/**************************************************************************************************/
	
	if (1 == virtual_daa_ring_incoming_detect())
	{
		if (++vir_daa_ring_on_cnt > vir_Ring_On_Ths )
		{
			vir_daa_ring_on_cnt = vir_Ring_On_Ths; // avoid overflow

			if ( 0 == vir_daa_ring_flag )
			{
				int x=0;
				for (x=0; x<SLIC_CH_NUM; x++)
				{
					hook_in(x, FXO_RING_ON);
				}
				vir_daa_ring_on_cnt = 0;
				vir_daa_ring_flag = 1;
				PRINT_MSG("Virtual DAA Ringing on.\n");
#ifdef CONFIG_RTK_VOIP_LED				
				pstn_ringing = 1;
#endif
			}
		}

		vir_daa_ring_off_cnt = 0;

	}
	else
	{
		if ( 1 == vir_daa_ring_flag )
		{

			vir_daa_ring_off_cnt++;			

			if (vir_daa_ring_off_cnt > vir_Ring_Off_Ths)
			{
				int x=0;
				for (x=0; x<SLIC_CH_NUM; x++)
				{
			#if 0
					if (relay_2_PSTN_flag[x] == 0)//relay is at SLIC
					{
						hook_in(x, FXO_RING_OFF);
					}
					else
					{
						// when relay is at PSTN, it means Phone<-> PSTN is connected.
						// So, do nothing.
					}
			#else
					hook_in(x, FXO_RING_OFF);
			#endif
				}

				vir_daa_ring_on_cnt = 0;
				vir_daa_ring_flag = 0;
				PRINT_MSG("Virtual DAA Ringing off.\n");
#ifdef CONFIG_RTK_VOIP_LED
				pstn_ringing = 0;
#endif
			}
		}
	}
#endif

#ifdef CONFIG_RTK_VOIP_DRIVERS_SI3050
	// Polling DAA ring and busy tone flag
	for (i=SLIC_CH_NUM; i < VOIP_CH_NUM; i++)
	{
		//printk("%d %d\n", daa_ring_on_cnt[i], daa_ring_off_cnt[i]);
		if (1 == DAA_Ring_Detection(i))
		{
			daa_ring_off_cnt[i] = 0;
			check_ring_off_time_out[i] = 0;

			if (++daa_ring_on_cnt[i] > Ring_On_Cnt )
			{
				daa_ring_on_cnt[i] = Ring_On_Cnt; // avoid overflow

				if ( 0 == daa_ring_flag[i])
				{
					if (daa_ring_on_cnt[i] >= Ring_On_Cnt)	   // should small than 30
					{
						if (wait_ring_off[i] ==0)
						{
							ring_on[i] ++;
							PRINT_MSG("ring_on[%d]=%d\n", i, ring_on[i]);
						}
						wait_ring_off[i] = 1;
					}

					if (ring_on[i] >= ring_on_time[i] && ring_off[i] >= ring_off_time[i])//  ring(1) -- silence -- ring(2) -- silence -- ring(3)
					{
#ifdef CONFIG_RTK_VOIP_LED
						pstn_ringing[i] = 1;
#endif
						hook_in(i, FXO_RING_ON);
						daa_ring_on_cnt[i] = 0;
						daa_ring_flag[i] = 1;
						ring_on[i] = 0;
						ring_off[i] = 0;
						wait_ring_off[i] = 0;
						PRINT_MSG("DAA Ringing on(%d).\n", i);
					}
				}

			}


		}
		else
		{
			daa_ring_on_cnt[i] = 0;

			if ( 1 == daa_ring_flag[i])
			{
				daa_ring_off_cnt[i]++;

				if (daa_ring_off_cnt[i] > Ring_Off_Event_Ths)
				{
					daa_ring_off_cnt[i] = Ring_Off_Event_Ths;

					if ( 0 == DAA_Hook_Status(i))// daa is on-hook (must, important)
					{
						hook_in(i, FXO_RING_OFF);
					}
					else
					{
						// when daa is off-hhok, it means FXS<->FXO VoIP is connected.
						// So, do nothing.
					}
					daa_ring_on_cnt[i] = 0;
					daa_ring_flag[i] = 0;
					PRINT_MSG("DAA Ringing off(%d).\n", i);
#ifdef CONFIG_RTK_VOIP_LED
					pstn_ringing[i] = 0;
#endif
				}
			}
			else
			{
				if (++daa_ring_off_cnt[i] > Ring_Off_Cnt)
					daa_ring_off_cnt[i] = Ring_Off_Cnt + 1;

				if (wait_ring_off[i] == 1)
				{
					if (daa_ring_off_cnt[i] > Ring_Off_Cnt)
					{
						ring_off[i] ++;
						PRINT_MSG("ring_off[%d]=%d\n", i, ring_off[i]);
						wait_ring_off[i] = 0;
						daa_ring_off_cnt[i] = Ring_Off_Cnt;
						
						wait_ring_off_timeout[i] = 0;
						check_ring_off_time_out[i] = 1;
					}
				}

				if (check_ring_off_time_out[i] == 1)
				{
					if ( ++wait_ring_off_timeout[i] > (Ring_Off_Event_Ths-Ring_Off_Cnt))
					{
						check_ring_off_time_out[i] = 0;
						wait_ring_off_timeout[i] = 0;
						ring_on[i] = 0;
						ring_off[i] = 0;
						PRINT_MSG("ring off(%d) time out\n", i);
					}
				}
			}

		}

#ifndef AUDIOCODES_VOIP

		if (1 == busy_tone_flag_get(i))	//should add daa chid
		{
			busy_tone_det_init(i);	//should add daa chid
			DAA_On_Hook(i);
			hook_in(i, FXO_BUSY_TONE);
		}

		int temp_ring_tone_flag_get;
		temp_ring_tone_flag_get = ring_tone_flag_get(i);
		if (temp_ring_tone_flag_get & 0x5)	//should add daa chid
		{
			//busy_tone_det_init(i);	//should add daa chid
			//DAA_On_Hook(i);
			if(1 == temp_ring_tone_flag_get)
				hook_in(i, FXO_RING_TONE_ON);
			else
				hook_in(i, FXO_RING_TONE_OFF);
		}
#endif


		// NTT Short Ring Detection

		if (DAA_Positive_Negative_Ring_Detect(i))
			NTT_sRing_det(i, 1);/* ringing */
		else
			NTT_sRing_det(i, 0);/* non-ringing */

#ifdef AUDIOCODES_VOIP

		// if short(alert) ring is detected, off-hook daa.
		long daa_stat[MAX_VOIP_CH_NUM]={0};
		static int ring_and_offhook[MAX_VOIP_CH_NUM] = {0};
		static int ntt_offhook_period_cnt[MAX_VOIP_CH_NUM] = {0};
		
		daa_stat[i] = DAA_Hook_Status(i);
		
		if ( (ntt_sRing_on_pass[i] >= ntt_sRing_on_cnt) &&  (ntt_sRing_off_pass[i] >= ntt_sRing_off_cnt) )
		{
			if(!daa_stat[i])// DAA on-hook
			{
				DAA_Off_Hook(i);	// for ntt cid, need off-hook to recive the cid data.
				ring_and_offhook[i] = 1;
				PRINT_MSG("offhook(NTT), ch%d\n", i);
			}
	
			ntt_sRing_on_pass[i]= 0 ;
			ntt_sRing_off_pass[i] = 0;
			ring_and_offhook[i] = 1;
			ntt_offhook_period_cnt[i] = 0;	/* reset ntt_offhook_period_cnt to zero , avoid on-hook earily  */
		}
		
		if (daa_stat[i]) // DAA off-hook
		{
			if (fsk_decode_complete_flag[i] == 1) // NTT cid is detected
			{
				if (ring_and_offhook[i] == 1)
				{
					DAA_On_Hook(i);	// for ntt cid, need on-hook when recive the cid data end.
					ring_and_offhook[i] = 0;
					PRINT_MSG("onhook(NTT), ch%d\n", i);
				}
				fsk_decode_complete_flag[i] = 0;
			}
			else if (ring_and_offhook[i] == 1)
			{
				ntt_offhook_period_cnt[i]++;
				if(ntt_offhook_period_cnt[i]>580)
				{
					DAA_On_Hook(i);	// for ntt cid, need on-hook when not recive the cid data over 6 sec.
					PRINT_MSG("time out: onhook(NTT), ch%d\n", i);
					ring_and_offhook[i] = 0;
					ntt_offhook_period_cnt[i] = 0;
					fsk_decode_complete_flag[i] = 0;
				}
			}
		}
#endif //AUDIOCODES_VOIP

#ifdef PULSE_DIAL_GEN
#ifdef CONFIG_RTK_VOIP_PULSE_DIAL_GEN_TIMER
	DAA_PollPulseGenFifo(i);
#else
	DAA_PulseGenProcess(i);
#endif
#endif
	}
#endif //CONFIG_RTK_VOIP_DRIVERS_SI3050

#ifdef AUDIOCODES_VOIP
	/* Polling DTMF, Fax, Modem Events */
	extern int dtmf_in(uint32 ch_id, char input);
	extern int fax_modem_in(uint32 ch_id, char input);
	static TeventDetectionResult event_res;
	memset(&event_res, 0, sizeof(TeventDetectionResult));

	RtkAc49xApiEventPolling(&event_res);

	if (event_res.dtmf_digit != 'Z')
	{
		dtmf_in(event_res.channel, event_res.dtmf_digit);
	}

	if (event_res.ced_flag == 1)
	{
		fax_modem_in(event_res.channel, 1);// fax
	}
	else if (event_res.modem_flag == 1)
	{
		fax_modem_in(event_res.channel, 2);// modem
	}

	if (Is_DAA_Channel(event_res.channel) == 1)
	{
		
#ifdef FXO_CALLER_ID_DET
		if (event_res.dtmf_cid_valid == 1)
		{
			cid_res[event_res.channel].dtmf_cid_valid = 1;
			cid_res[event_res.channel].cid_length = event_res.pCidMsg[1];
			int i=0;
			//printk("CID = ");
			for (i=0; i< cid_res[event_res.channel].cid_length; i++)
			{
				cid_res[event_res.channel].number[i] = event_res.pCidMsg[2+i];
				//printk("%c ", cid_res[event_res.channel].number[i]);
			}
			//printk("\n");
			cid_res[event_res.channel].number[i] = 0;
			cid_res[event_res.channel].cid_name[0] = 0;
			cid_res[event_res.channel].date[0] = 0;
			
		}
		else if (event_res.fsk_cid_valid == 1)
		{
			fsk_decode_complete_flag[event_res.channel] = 1;
			cid_res[event_res.channel].fsk_cid_valid = 1;
			cid_res[event_res.channel].cid_length = event_res.pCidMsg[1];		
			cid_res[event_res.channel].number_absence = event_res.num_absence;
			cid_res[event_res.channel].name_absence = event_res.name_absence;
			cid_res[event_res.channel].visual_indicator = event_res.visual_indicator;
#if 0
			printk("num_abs= %d, name_abs= %d, vmwi= %d\n", event_res.num_absence, event_res.name_absence, event_res.visual_indicator);
			printk("Number = ");
			for (i=0; i < (strlen(event_res.cid_num)); i++)
			{
				printk("%c ", event_res.cid_num[i]);
			}
			printk("\n");
	                        
			printk("Date and Time = ");
			for (i=0; i < (strlen(event_res.cid_date_time)); i++)
			{
				printk("%c ", event_res.cid_date_time[i]);
			}
			printk("\n");
	                        
			printk("Name = ");
			for (i=0; i < (strlen(event_res.cid_name)); i++)
			{
				printk("%c ", event_res.cid_name[i]);
			}
			printk("\n");
#endif

			if (cid_res[event_res.channel].number_absence == 1)
				cid_res[event_res.channel].number[0] = 0;
			else
				strcpy(cid_res[event_res.channel].number, event_res.cid_num);
			
			if (cid_res[event_res.channel].name_absence == 1)
				cid_res[event_res.channel].cid_name[0] = 0;
			else	
				strcpy(cid_res[event_res.channel].cid_name, event_res.cid_name);
			
			strcpy(cid_res[event_res.channel].date, event_res.cid_date_time);
			
#if 0
			printk("Number = ");
			for (i=0; i < (strlen(cid_res[event_res.channel].number)); i++)
			{
				printk("%c ", cid_res[event_res.channel].number[i]);
			}
			printk("\n");
	                        
			printk("Date and Time = ");
			for (i=0; i < (strlen(cid_res[event_res.channel].date)); i++)
			{
				printk("%c ", cid_res[event_res.channel].date[i]);
			}
			printk("\n");

			printk("Name = ");
			for (i=0; i < (strlen(cid_res[event_res.channel].cid_name)); i++)
			{
				printk("%c ", cid_res[event_res.channel].cid_name[i]);
			}
			printk("\n");
#endif	
		}
	
#endif //FXO_CALLER_ID_DET

#ifdef FXO_BUSY_TONE_DET
		if (event_res.IBS_CP == 9) // Busy tone detected
		{
			DAA_On_Hook(event_res.channel);
			hook_in(event_res.channel, FXO_BUSY_TONE);
		}
		else if (event_res.IBS_CP == 10) // Ringback tone detected
		{
			hook_in(event_res.channel, FXO_RING_TONE_ON);
			// Need to know FXO_RING_TONE_OFF
		}
		else if (event_res.IBS_CP == 8) // Dial tone detected
		{
			// Do Nothing
		}
#endif
	}

#endif	//AUDIOCODES_VOIP

#ifndef AUDIOCODES_VOIP
	// For fsk CID type-II alert tone
	fsk_alert_procrss();
	
	fsk_cid_process();
#endif

	event_timer.expires = jiffies + 1;
	add_timer(&event_timer);
}

void Init_Event_Polling_Use_Timer(void)
{
	init_timer(&event_timer);
	event_timer.expires = jiffies + 1;
	//event_timer.data = 30;
	event_timer.function = Event_Polling_Use_Timer;
	add_timer(&event_timer);
	PRINT_MSG("Add Event Timer For Polling\n");
}

void Reinit_Event_Polling_Use_Timer(void)
{
	del_timer(&event_timer);
	Init_Event_Polling_Use_Timer();
}



void SLIC_Set_Ring_Cadence(unsigned char chid, unsigned short OnMsec, unsigned short OffMsec)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	SLIC_Set_Ring_Cadence_ON_Silicon(chid, OnMsec);
	SLIC_Set_Ring_Cadence_OFF_Silicon(chid, OffMsec);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226)
	pSlic = ports[chid].ProObj;
	Si3226_Set_Ring_Cadence_ON(pSlic, OnMsec);
	Si3226_Set_Ring_Cadence_OFF(pSlic, OffMsec);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	SLIC_Set_Ring_Cadence_Legerity(chid, OnMsec, OffMsec);
#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
	SetupRing(0x7E6C,0x1039,OnMsec,OffMsec,chid);
#endif
}

void SLIC_Set_Impendance(unsigned char chid, unsigned short country, unsigned short impd)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	SLIC_Set_Impendance_Silicon(chid, country, impd);

#elif defined (CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226)
	pSlic = ports[chid].ProObj;
	SLIC_Set_Impendance_Silicon_3226(pSlic, country, impd);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	SLIC_Set_Impendance_Legerity(chid, country, impd);

#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
	SetImpedance(chid, country);	//country ref:w682388.h(register 168/170 IMPED_SYNTH_CTRL)
#endif
	PRINT_MSG("Set SLIC impedance according to the country...\n");
}

#if 0
void SLIC_GenProcessTone(unsigned int chid, genTone_struct *gen_tone)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	GenProcessTone_Silicon(chid, gen_tone);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	//GenProcessTone_Legerity(gen_tone);

#endif
}
#endif


void OnHookLineReversal(int chid, unsigned char bReversal) //0: Forward On-Hook Transmission, 1: Reverse On-Hook Transmission
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	OnHookLineReversal_Silicon(chid, bReversal);

#elif defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
	pSlic = ports[chid].ProObj;
	if (bReversal)
		ProSLIC_SetLinefeedStatus(pSlic, LF_REV_OHT);
	else
		ProSLIC_SetLinefeedStatus(pSlic, LF_FWD_OHT);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	Legerity_OnHookLineReversal(chid, bReversal);

#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
	W682388_OnHookLineReversal(chid, bReversal);
#endif
}

void SendNTTCAR(int chid)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	SendNTTCAR_Silicon(chid);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226)
	pSlic = ports[chid].ProObj;
	Si3226_SendNTTCAR(pSlic);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	SendNTTCAR_Legerity(chid);
#endif
}

unsigned int SendNTTCAR_check(unsigned int chid, unsigned long time_out)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	return SendNTTCAR_check_Silicon(chid, time_out);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226)
	pSlic = ports[chid].ProObj;
	return Si3226_SendNTTCAR_check(chid, pSlic, time_out);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	return SendNTTCAR_check_Legerity(chid, time_out);
#endif
}

void disableOscillators(unsigned int chid)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	disableOscillators_Silicon(chid);
#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	//disableOscillators_Legerity();
#endif
}

#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
static char cid_reg64, cid_reg64_prev;
#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
static Le88xxx data;
#endif

void SetOnHookTransmissionAndBackupRegister(int chid) // use for DTMF caller id
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )

#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226
	pSlic = ports[chid].ProObj;
	Si3226_GetLinefeedStatus(pSlic, &cid_reg64);
#else
	cid_reg64 = readDirectReg(chid, 64);
#endif

	if ( (cid_reg64 & 0x07) != 2 )  // force for DTMF CID display
	{
		cid_reg64_prev = cid_reg64; // record it
		PRINT_MSG("Reg64 = 0x%02x\n", cid_reg64);
		OnHookLineReversal(chid, 0); //Forward On-Hook Transmission
	}

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)

	readLegerityReg( chid, 0x56, &data);	//back up
	OnHookLineReversal(chid, 0); //Forward On-Hook Transmission

#endif
}

void RestoreBackupRegisterWhenSetOnHookTransmission(unsigned int chid) // use for DTMF caller id
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )

	writeDirectReg(chid, 64,cid_reg64_prev);

#elif defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
	pSlic = ports[chid].ProObj;
	Si3226_SetLinefeedStatus(pSlic, cid_reg64_prev);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	writeLegerityReg( chid, 0x56, &data);	// restore back-up value

#endif
}

#ifndef CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221
#ifndef CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226
unsigned char Legerity_system_state(unsigned char slic_id, unsigned char state, unsigned char wri_re)
{
	return 0;
}
#endif
#endif

#ifndef CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210
#ifndef CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215
void fskInitialization (void)
{}
#endif
#endif


void SLIC_Set_PCM_state(int chid, int enable)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )

	SetSi321xPCM(chid, enable);

#elif defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
	pSlic = ports[chid].ProObj;
	if (enable)
		ProSLIC_PCMStart(pSlic);
	else
		ProSLIC_PCMStop(pSlic);

#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)

	Legerity_slic_set_tx_pcm(chid, enable);
	Legerity_slic_set_rx_pcm(chid, enable);

#endif
}



int SLIC_Get_Hook_Status(int chid)
{
	unsigned char status;

#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210 ) || defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215 )
	status = readDirectReg(chid, 68)&0x01; /* 1:off-hook  0:on-hook */
#elif defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
	pSlic = ports[chid].ProObj;
	ProSLIC_ReadHookStatus(pSlic, &status); /* 1:off-hook  0:on-hook */
#elif defined CONFIG_RTK_VOIP_DRIVERS_IP_PHONE
	status = iphone_hook_detect();	/* 1:off-hook  0:on-hook */
#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388
	status = GetHookState(chid);	/* 1:off-hook  0:on-hook */
#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88221) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE88226)
	
	if ((chid < 0) || (chid > 1))
	{
		printk("Not support CH%d: %s:%s:%d\n", chid, __FILE__, __FUNCTION__, __LINE__);
		return 0;
	}
	Le88xxx data;
	readLegerityReg(chid, 0x4F, &data);
	if (chid == 0)
		status = data.byte1&0x01;	/* 1:off-hook  0:on-hook */
	else if (chid == 1)
		status = data.byte2&0x01;	/* 1:off-hook  0:on-hook */
#endif

	return status;
}

void SLIC_read_reg(unsigned char chid, unsigned int num, unsigned char *val)
{
#if defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215)
    	*val = readDirectReg(chid, num);
#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388)
       	*val = ReadReg(num);
#elif defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
	extern unsigned char R_reg(unsigned char chid, unsigned char regaddr);
	extern int SI3226_SetUserMode(proslicChanType *pProslic, int on);
	if (num == 32261)
	{
		SI3226_SetUserMode( ports[chid].ProObj, 1); // enable user mode
		printk("Si3226 channel %d enter user mode.\n", chid);
	}
	else if (num == 32260)
	{
		SI3226_SetUserMode( ports[chid].ProObj, 0); // disable user mode
		printk("Si3226 channel %d leave user mode.\n", chid);
	}
	else
		*val = R_reg(chid, num);	
#endif
}


void SLIC_write_reg(unsigned char chid, unsigned char num, unsigned char val)
{
#if defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3210) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3215)
        writeDirectReg(chid, num, val);
#elif defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_W682388)
        WriteReg(num, val);
#elif defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
	extern void W_reg(unsigned char chid, unsigned char regaddr, unsigned char data);
	W_reg(chid, num, val);
#endif
}

void SLIC_read_ram(unsigned char chid, unsigned short num, unsigned int *val)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
	extern unsigned int R_ram(unsigned char chid, unsigned short reg);
	*val = R_ram(chid, num);
#endif
}


void SLIC_write_ram(unsigned char chid, unsigned short num, unsigned int val)
{
#if defined( CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226 )
	extern void W_ram(unsigned char chid, unsigned short reg, unsigned int data);
	W_ram(chid, num, val);
#endif
}


