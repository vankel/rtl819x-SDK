/************************************  PROSLIC.H   *************************************************/

/* PROTOTYPE */
#if 0
void cadenceRingPhone(ringStruct ringRegs);
void printMenu(void);
void initialize(void);
#endif
void goActive(void);
void calibrate(void);
int powerUp(void);
int powerLeakTest(void);

void setState(int newState);
#if 0
void dtmfAction(void);
void stateMachine(void);
unsigned char powerDown(void);
void sendProSLICID(void);
void sendSiLabsID(void);
void key (void);
void dumpAllRegisters(void);
#endif
unsigned char Interrupt(void);
#if 0
void sleep(unsigned long wait);
void calibrateAndActivateProSlic(void);
void InitializePcm(void);
void loopBackOff (void);
#endif
void converterOn(void);
void calibrate(void);
#if 0
void ringAndWait(void);
void phoneSystemDemo(void);
void calibrateAndActivateProSlicMultiProslic(unsigned char chipNumber);
unsigned char interruptDaisy(unsigned char NumberOChips);
void enablePCMhighway(void);
void disablePCMhighway(void);
void debugCurrent(void);
void isr(void);
#endif
void disableOscillators(void);
#if 0
void ringPhone(void);
char /* boolian */  version_chip ( unsigned char chip_num);
void reset(void);
void unReset(void);
void loopAction(void);
void printIndirectRegisters(void);
unsigned char readDirectReg(unsigned char address);
void initializeIndirectRegisters(void);
void verifyIndirectRegisters(void);
void writeDirectReg(unsigned char address, unsigned char data);
void writeIndirectReg(unsigned char address, unsigned short data);
unsigned short readIndirectReg(unsigned char address);
void  (*findNumber(void))(void);
void nextCadence(void);
void stopRinging(void);
void quickNeon(void);
#endif
unsigned short manualCalibrate(void);
#if 0
void syssleep( unsigned long wait );
#endif

void activateRinging(void);
void init_spi(void);
//void proslic_init_all(int ch_____) //original prototype.
void proslic_init_all(int ch_____,unsigned char slic_number);//chiminer modify.
//void dump_registers(void);			//	for debugging
void set_channel(int channel);


/* STRUCTURES */
typedef struct {
	unsigned char address;
	char *name;
	unsigned short initial;
} indirectRegister;

enum slic_state{ 
	MAKEbUSY,				// 0
	STATEcHANGE,			// 1
	DIALtONE,				// 2
	INITIALIZING,			// 3
	POWERuP,				// 4
	CALIBRATE,				// 5
	PROGRAMMEDrING,		// 6
	POWERdOWN,			// 7
	POWERlEAKtEST,			// 8
	MAKErINGbACKjAPAN,	// 9
	MAKEbUSYjAPAN,		// 10
	RINGbACKjAPAN,			// 11
	MAKErINGbACK,			// 12
	RINGbACK,				// 13
	MAKErEORDER,			// 14
	REORDER,				// 15
	MAKEcONGESTION,		// 16
	CONGESTION,			// 17
	PRENEON,				// 18
	NEON,					// 19
	CALLBACKpROGRAMMED,	// 20
	BUSY,					// 21
	CALLBACK,				// 22
	CALLING,					// 23
	MAKEoFFHOOK,			// 24
	ONHOOK,					// 25
	OFFHOOK,				// 26
	DIGITDECODING,			// 27
	LOOPtRANSITION,			// 28
	FIRSTrING,				// 29
	DEFEREDcALLBACKpROGRAMMED,// 30
	CALLERiD,				// 31
	RINGING,				// 32
	DTMFtRANISTION			// 33
} ;

// enum state_type { BUSY, CALL_BACK, CALLING_BACK };
typedef struct {
	//unsigned char chip_number;
	enum slic_state state,newState,previousState;
	int digit_count;
	char DTMF_digits[20];
	unsigned long interrupt;
	unsigned char eventEnable;
	unsigned char hook_status;
	unsigned long On_Hook_time;
	unsigned long Off_Hook_time;
	char	version,type;
	struct{ enum { TYPE1, TYPE2, TYPE3 } ringType;
			int nextCadenceEntryIndex;
		}  ringCadenceCordinates;
	unsigned char ringCount;
	int qLog[6];
	unsigned long eventNumber;
} chipStruct;




typedef struct {
	unsigned short coeff;
	unsigned short x;
	unsigned short y;
	union {
		unsigned short onTime;
		struct	{
			unsigned char onLowByte;
			unsigned char onHiByte;
		}on;
	};

	union {
		unsigned short offTime;
		struct	{
			unsigned char offLowByte;
			unsigned char offHiByte;
		}off;
	};

}  ringStruct;


typedef struct {
	unsigned short coeff;
	unsigned short x;
	unsigned short y;
	unsigned char on_hi_byte;
	unsigned char on_low_byte;
	unsigned char off_hi_byte;
	unsigned char off_low_byte;
} Oscillator;




typedef struct {
	Oscillator osc1;
	Oscillator osc2;
} tone_struct;  

typedef struct {
unsigned short frequency;
unsigned short coeff; 
unsigned short x; 
unsigned short y; 
} coeffData;




/* CONSTANTS */

#define LPT 0X378

#define IDA_LO  28
#define IDA_HI	29

#define IAA 30
#define CLOCK_MASK 0x04
#define ID_ACCES_STATUS 31
#define IAS_BIT 1

//#define	I_STATUS	31

#define	SPI_MODE						0
#define	PCM_MODE						1
#define	PCM_XMIT_START_COUNT_LSB	2
#define	PCM_XMIT_START_COUNT_MSB	3
#define	PCM_RCV_START_COUNT_LSB	4
#define	PCM_RCV_START_COUNT_MSB	5
#define	DIO								6

#define	AUDIO_LOOPBACK				8
#define	AUDIO_GAIN					9
#define	LINE_IMPEDANCE				10
#define	HYBRID							11
#define	RESERVED12					12
#define	RESERVED13					13
#define	PWR_DOWN1						14
#define	PWR_DOWN2						15
#define	RESERVED16					16
#define	RESERVED17					17
#define	INTRPT_STATUS1				18
#define	INTRPT_STATUS2				19
#define	INTRPT_STATUS3				20
#define	INTRPT_MASK1					21
#define	INTRPT_MASK2					22
#define	INTRPT_MASK3					23
#define	DTMF_DIGIT					24
#define	RESERVED25					25
#define	RESERVED26					26
#define	RESERVED27	27
#define	I_DATA_LOW	28
#define	I_DATA_HIGH	29
#define	I_ADDRESS		30
#define	I_STATUS		31
#define	OSC1			32
#define	OSC2			33
#define	RING_OSC_CTL	34
#define	PULSE_OSC		35
#define	OSC1_ON__LO	36
#define	OSC1_ON_HI	37
#define	OSC1_OFF_LO	38
#define	OSC1_OFF_HI	39
#define	OSC2_ON__LO	40
#define	OSC2_ON_HI	41
#define	OSC2_OFF_LO	42
#define	OSC2_OFF_HI	43
#define	PULSE_ON__LO	44
#define	PULSE_ON_HI	45
#define	PULSE_OFF_LO	46
#define	PULSE_OFF_HI	47
#define	RING_ON__LO	48
#define	RING_ON_HI	49
#define	RING_OFF_LO	50
#define	RING_OFF_HI	51
#define	FSK_DATA	52	/*		0								fsk_data	*/
#define	RESERVED53	53
#define	RESERVED54	54
#define	RESERVED55	55
#define	RESERVED56	56
#define	RESERVED57	57
#define	RESERVED58	58
#define	RESERVED59	59
#define	RESERVED60	60
#define	RESERVED61	61
#define	RESERVED62	62
#define	RESERVED63	63
#define	LINE_STATE	64
#define			ACTIVATE_LINE 0x11
#define			RING_LINE     0x44
#define	BIAS_SQUELCH	65
#define	BAT_FEED	66
#define	AUTO_STATE	67
#define	LOOP_STAT	68
#define	LOOP_DEBOUCE	69
#define	RT_DEBOUCE	70
#define	LOOP_I_LIMIT	71
#define	OFF_HOOK_V	72
#define	COMMON_V	73
#define	BAT_V_HI	74
#define	BAT_V_LO	75
#define	PWR_STAT_DEV	76
#define	PWR_STAT	77
#define	LOOP_V_SENSE	78
#define	LOOP_I_SENSE	79
#define	TIP_V_SENSE	80
#define	RING_V_SENSE	81
#define	BAT_V_HI_SENSE	82
#define	BAT_V_LO_SENSE	83
#define	IQ1	84
#define	IQ2	85
#define	IQ3	86
#define	IQ4	87
#define	IQ5	88
#define	IQ6	89
#define	RESERVED90	90
#define	RESERVED91	91
#define	DCDC_PWM_OFF	92
#define	DCDC	93
#define	DCDC_PW_OFF	94
#define	RESERVED95	95
#define	CALIBR1	96
#define CALIBRATE_LINE 0x78
#define NORMAL_CALIBRATION_COMPLETE 0x20
#define	CALIBR2	97
#define	RING_GAIN_CAL	98
#define	TIP_GAIN_CAL	99
#define	DIFF_I_CAL	100
#define	COMMON_I_CAL	101
#define	I_LIMIT_GAIN_CAL	102
#define	ADC_OFFSET_CAL	103
#define	DAC_ADC_OFFSET	104
#define	DAC_OFFSET_CAL	105
#define	COMMON_BAL_CAL	106
#define	DC_PEAK_CAL	107

//		Indirect Register (decimal)
#define	DTMF_ROW_0_PEAK	0
#define	DTMF_ROW_1_PEAK	1
#define	DTMF_ROW2_PEAK	2
#define	DTMF_ROW3_PEAK	3
#define	DTMF_COL1_PEAK	4
#define	DTMF_FWD_TWIST	5
#define	DTMF_RVS_TWIST	6
#define	DTMF_ROW_RATIO_THRESH	7
#define	DTMF_COL_RATIO_THRESH	8
#define	DTMF_ROW_2ND_HARM	9
#define	DTMF_COL_2ND_HARM	10
#define	DTMF_PWR_MIN_THRESH	11
#define	DTMF_HOT_LIM_THRESH	12
#define	OSC1_COEF	13
#define	OSC1X	14
#define	OSC1Y	15
#define	OSC2_COEF	16
#define	OSC2X	17
#define	OSC2Y	18
#define	RING_V_OFF	19
#define	RING_OSC_COEF	20
#define	RING_X	21
#define	RING_Y	22
#define	PULSE_ENVEL	23
#define	PULSE_X	24
#define	PULSE_Y	25
#define	RECV_DIGITAL_GAIN	26
#define	XMIT_DIGITAL_GAIN	27
#define	LOOP_CLOSE_THRESH	28
#define	RING_TRIP_THRESH	29
#define	COMMON_MIN_THRESH	30
#define	COMMON_MAX_THRESH	31
#define	PWR_ALARM_Q1Q2	32
#define	PWR_ALARM_Q3Q4	33
#define	PWR_ALARM_Q5Q6	34
#define	LOOP_CLOSURE_FILTER	35
#define	RING_TRIP_FILTER	36
#define	THERM_LP_POLE_Q1Q2	37
#define	THERM_LP_POLE_Q3Q4	38
#define	THERM_LP_POLE_Q5Q6	39
#define	CM_BIAS_RINGING	40
#define	DCDC_MIN_V	41
#define	DCDC_XTRA	42
#define 	ALL_CHIPS 0x09
#define 	NO_CHIPS 0
#define	REVC	108	/*		0	ilim_max	fsk_revc	dc_err_en	zs_ext	batsel_pd	lcr_sense	en_subtr	hyst_en	*/
#if 0 //for 3210.chimner
#define	FSK_X_0		99	/*	x	sign				fsk_x_0[15:0]												*/
#define	FSK_COEFF_0	100	/*	x	sign				fsk_coeff_0[15:0]												*/
#define	FSK_X_1		101	/*	x	sign				fsk_x_1[15:0]												*/
#define	FSK_COEFF_1	102	/*	x	sign				fsk_coeff_1[15:0]												*/
#define	FSK_X_01	103	/*	x	sign				fsk_x_01[15:0]												*/
#define	FSK_X_10	104	/*	x	sign				fsk_x_10[15:0]												*/
#else //for 3215
#define	FSK_X_0		69	/*	x	sign				fsk_x_0[15:0]												*/
#define	FSK_COEFF_0	70	/*	x	sign				fsk_coeff_0[15:0]												*/
#define	FSK_X_1		71	/*	x	sign				fsk_x_1[15:0]												*/
#define	FSK_COEFF_1	72	/*	x	sign				fsk_coeff_1[15:0]												*/
#define	FSK_X_01	73	/*	x	sign				fsk_x_01[15:0]												*/
#define	FSK_X_10	74	/*	x	sign				fsk_x_10[15:0]												*/
#endif


#ifdef REALNAMES


/*      OPTIONAL CONSTANTS */

/**********************SETUP*****************************************/													
#define	SPI_MODE	0	/*		b00xxxxxx	daisy_chn	spi_mode	part_num[1:0]			revision[3:0]			*/
#define	PCM_MODE	1	/*		8			pcm_en	pcm_fmt[1:0]		pcm_size	pcm_gci	pcm_tri_0	*/
#define	TX_START_LO	2	/*		0				tx_start[7:0]					*/
#define	TX_START_HI	3	/*		0						 	tx_start[9:8]		*/
#define	RX_START_LO	4	/*		0				rx_start[7:0]					*/
#define	RX_START_HI	5	/*		0						 	rx_start[9:8]		*/
//#define	GPIO		6	/*		0				relay_drv	gpio1_dir	gpio0_dir	gpio1	gpio0	*/
 //			/*											*/
///***********************AUDIO****************************************/											*/
#define	LOOPBACK	8	/*		2						ana_lpbk2	dig_lpbk	ana_lpbk	*/
#define	AUD_GAIN	9	/*		0	rx_hpf_dis	tx_hpf_dis	adc_mute	dac_mute	atx[1:0]		arx[1:0]		*/
#define	ZSYNTH		10	/*		8		 	clcomp[1:0]		zsynth_en		zsynth[2:0]		*/
#define	HYBRID_CNTL	11	/*		h44			hybp[2:0]				hyba[2:0]		*/
#define	POWER DOWN		/*											*/
#define	PDN1		14	/*		b010000			pm_on	dc_off	mon_off	pll_off	bias_off	slic_off	*/
#define	PDN2		15	/*		0		 	adc_man	a_on_offb	dac_man	d_on_offb	gm_man	g_on_offb	*/



/***********************INTERRUPTS***********************************/
#define	IRQ_VEC1	18	/*		0	pulse_t2	pulse_t1	ring_t2	ring_t1	osc2_t2	osc2_t1	osc1_t2	osc1_t1	*/
#define	IRQ_VEC2	19	/*		0	pq6	pq5	pq4	pq3	pq2	pq1	loop_stat	ring_trip	*/
#define	IRQ_VEC3	20	/*		0						cal_cm_bal	indirect	dtmf	*/
#define	IRQ_MASK1	21	/*		0	pulse_t2	pulse_t1	ring_t2	ring_t1	osc2_t2	osc2_t1	osc1_t2	osc1_t1	*/
#define	IRQ_MASK2	22	/*		0	pq6	pq5	pq4	pq3	pq2	pq1	loop_stat	ring_trip	*/
#define	IRQ_MASK3	23	/*		0						cal_cm_bal	indirect	dtmf	*/
#define	DTMF		24	/*		0				valid		dtmf_digit[3:0]		 	*/
#define	PASS_LO		25	/*		0				dtmf_pass[7:0]					*/
#define	PASS_HI		26	/*		0							dtmf_pass[9:8]		*/

/******************INDIRECT REGISTER ACCESS	***************************/
#define	IND_DATA_LO	28	/*		0			ind_data[7:0]						*/
#define	IND_DATA_HI	29	/*		0			ind_data[15:8]						*/
#define	IND_ADDR	30	/*		0			ind_add[7:0]						*/
#define	IND_STAT	31	/*		0	ind_add[8]							ind_stat	*/


/**************************OSCILLATORS**********************************/
#define	OSC1_CNTL	32	/*		0	en_sync	reload	zero_en	t1_en	t2_en	enable	routing[1:0]		*/
#define	OSC2_CNTL	33	/*		0	en_sync		zero_en	t1_en	t2_en	enable	routing[1:0]		*/
#define	RING_CNTL	34	/*		0	en_sync		ring_dac	t1_en	t2_en	ring_en	offset	trap	*/
#define	PULSE_CNTL	35	/*		0	en_sync		zero_en	t1_en	t2_en	enable	env_en		*/
#define	OSC1_T1_LO	36	/*		0				osc1_t1[7:0]					*/
#define	OSC1_T1_HI	37	/*		0				osc1_t1[15:8]					*/
#define	OSC1_T2_LO	38	/*		0				osc1_t2[7:0]					*/
#define	OSC1_T2_HI	39	/*		0				osc1_t2[15:8]					*/
#define	OSC2_T1_LO	40	/*		0				osc2_t1[7:0]					*/
#define	OSC2_T1_HI	41	/*		0				osc2_t1[15:8]					*/
#define	OSC2_T2_LO	42	/*		0				osc2_t2[7:0]					*/
#define	OSC2_T2_HI	43	/*		0				osc2_t2[15:8]					*/
#define	PULSE_T1_LO	44	/*		0				pulse_t1[7:0]					*/
#define	PULSE_T1_HI	45	/*		0				pulse_t1[15:8]					*/
#define	PULSE_T2_LO	46	/*		0				pulse_t2[7:0]					*/
#define	PULSE_T2_HI	47	/*		0				pulse_t2[15:8]					*/
#define	RING_T1_LO	48	/*		0				ring_t1[7:0]					*/
#define	RING_T1_HI	49	/*		0				ring_t1[15:8]					*/
#define	RING_T2_LO	50	/*		0				ring_t2[7:0]					*/
#define	RING_T2_HI	51	/*		0				ring_t2[15:8]					*/
#define	FSK_DATA	52	/*		0								fsk_data	*/

/*****************************	SLIC *********************************************/

#define	DBI_LCR_RING	63	/*		52				dbi_lcr_ring[6:0]					*/
#define	LINEFEED		64	/*		0			linefeed_shadow[2:0]				linefeed[2:0]		*/
#define	BJTBIAS			65	/*		b1100001		squelch	cap_bypass	en_bjtbias	bjtbias_oht[1:0]		bjtbias_act[1:0]		*/
#define	BAT_CNTL		66	/*		b1011				vov_1p5	set_vreg	extbat	batsel	track	*/
#define	AUTO			67	/*		b0011111		mancm	mandiff	en_speedup	autobat	autord	autold	autoopen	*/
#define	LCR_RTP			68	/*		0					 	dbi_raw	rtp	lcr	*/
#define	DBI_LCR			69	/*		10				dbi_lcr[6:0]					*/
#define	DBI_RTP			70	/*		10				dbi_rtp[6:0]					*/
#define	ILIM			71	/*		0							ilim[2:0]		*/
#define	VOC				72	/*		32		vocsgn			voc[5:0]				*/
#define	VCM				73	/*		2					vcm[5:0]				*/
#define	VBATH			74	/*		50					vbath[5:0]				*/
#define	VBATL			75	/*		16					vbatl[5:0]				*/
#define	PWR_PTR			76	/*		0							pwr_ptr[2:0]		*/
#define	PWR_OUT			77	/*		0				pwr_out[7:0]					*/
#define	LVS				78	/*		0				lvs[6:0]					*/
#define	LCS				79	/*		0				lcs[6:0]					*/
#define	VTIP			80	/*		0				vtip[7:0]					*/
#define	VRING			81	/*		0				vring[7:0]					*/
#define	VREG1			82	/*		0				vreg1[7:0]					*/
#define	VREG2			83	/*		0				vreg2[7:0]					*/
#define	IQ1				84	/*		0				iq1[7:0]					*/
#define	IQ2				85	/*		0				iq2[7:0]					*/
#define	IQ3				86	/*		0				iq3[7:0]					*/
#define	IQ4				87	/*		0				iq4[7:0]					*/
#define	IQ5				88	/*		0				iq5[7:0]					*/
#define	IQ6				89	/*		0				iq6[7:0]					*/

/**********************	DC/DC************************************************/
#define	DC_N			92	/*		255				dc_n[7:0]					*/
#define	DC_S_DELAY		93	/*		20	dcdc_cal		dc_ff			dc_s_delay[4:0]			*/
#define	DC_X			94	/*		0				dc_x[7:0]					*/
#define	DC_ERR			95	/*		x							dc_err[2:0]		*/
 
/************************CALIBRATION****************************************/
#define	CAL_R1			96	/*		h1f		cal	cal_spdup	cal_gmisr	cal_gmist	cal_ding	cal_cing	cal_ilim	*/
#define	CAL_R2			97	/*		h1f				cal_madc1	cal_madc2	cal_dac_o	cal_adc_o	cal_cm_bal	*/
#define	CAL_GMIS_ICR	98	/*		16						cal_gmis_icr[4:0]			*/
#define	CAL_GMIS_ICT	99	/*		16						cal_gmis_ict[4:0]			*/
#define	CAL_DIN_GAIN	100	/*		17						cal_din_gain[4:0]			*/
#define	CAL_CIN_GAIN	101	/*		17						cal_cin_gain[4:0]			*/
#define	CAL_ILIM		102	/*		8						cal_ilim[3:0]			*/
#define	CAL_MADC		103	/*		h88		cal_madc_gm2[3:0]				cal_madc_gm1[3:0]			*/
#define	CAL_DAC_O_A		104	/*		0		 	 	 	dac_os_p	dac_os_n	adc_os_p	adc_os_n	*/
#define	CAL_DAC_O_D		105	/*		0				dac_offset[7:0]					*/
#define	CAL_CM_BAL		106	/*		h20					cm_bal[5:0]				*/
#define	CAL_DCPK		107	/*		8					 	cal_dcpk[3:0]			*/


#define	REVC	108	/*		0	ilim_max	fsk_revc	dc_err_en	zs_ext	batsel_pd	lcr_sense	en_subtr	hyst_en	*/


/*	INDIRECT REGISTERS																				*/
																					
/*	Name	#		D	b15	b14	b13	b12	b11	b10	b9	b8	b7	b6	b5	b4	b3	b2	b1	b0	*/
																				
/*	DTMF				(Constant values provided by Silabs after DTMF performance evaluation)																*/
#define	ROW0_THR	0	/*	x	sign				row0_thr[11:0]												*/
#define	ROW1_THR	1	/*	x	sign				row1_thr[11:0]												*/
#define	ROW2_THR	2	/*	x	sign				row2_thr[11:0]												*/
#define	ROW3_THR	3	/*	x	sign				row3_thr[11:0]												*/
#define	COL_THR		4	/*	x	sign				col_thr[11:0]												*/
#define	FWD_TW_THR	5	/*	x					sign			fwd_tw_thr[7:0]									*/
#define	REV_TW_THR	6	/*	x					sign			rev_tw_thr[7:0]									*/
#define	ROW_REL_THR	7	/*	x					sign			row_rel_thr[7:0]									*/
#define	COL_REL_THR	8	/*	x					sign			col_rel_thr[7:0]									*/
#define	ROW_2ND_THR	9	/*	x					sign			row_2nd_thr[7:0]									*/
#define	COL_2ND_THR	10	/*	x					sign			col_2nd_thr[7:0]									*/
#define	PWR_MIN_THR	11	/*	x	sign				pwr_min_thr[15:0]												*/
#define	HOT_LIMIT	12	/*	x	sign				hot_limit[15:0]												*/
																					
/*	OSCILLATORS				(See descriptions of tone generation, ringing and pulse metering for quidelines on computing register values)																*/
#define	OSC1_COEFF	13	/*	x	sign				osc1_coeff[15:0]												*/
#define	OSC1_X_REG	14	/*	x	sign				osc1_x_reg[15:0]												*/
#define	OSC1_Y_REG	15	/*	x	sign				osc1_y_reg[15:0]												*/
#define	OSC2_COEFF	16	/*	x	sign				osc2_coeff[15:0]												*/
#define	OSC2_X_REG	17	/*	x	sign				osc2_x_reg[15:0]												*/
#define	OSC2_Y_REG	18	/*	x	sign				osc2_y_reg[15:0]												*/
#define	RING_OFFSET	19	/*	x	sign				ring_offset[15:0]												*/
#define	RING_COEFF	20	/*	x	sign				ring_coeff[15:0]												*/
#define	RING_X_REG	21	/*	x	sign				ring_x_reg[15:0]												*/
#define	RING_Y_REG	22	/*	x	sign				ring_y_reg[15:0]												*/
#define	PULSE_DELTA	23	/*	x	sign				pulse_delta[15:0]												*/
#define	PULSE_X_REG	24	/*	x	sign				pulse_x_reg[15:0]												*/
#define	PULSE_COEFF	25	/*	x	sign				pulse_coeff[15:0]												*/
																					
/*	PGAs				(See description of PGA for quidelines on computing register values)																*/
#define	DAC_GAIN	26	/*	x	sign				dac_gain[11:0]												*/
#define	ADC_GAIN	27	/*	x	sign				adc_gain[11:0]												*/
																					
/*	SLIC CONTROL																				*/
#define	LCR_THR		28	/*		0			lcr_thr_high[5:0]													*/
#define	RTP_THR		29	/*		0			rtp_thr[5:0]													*/
#define	CM_LOW_THR	30	/*		0	0	0			cm_low_thr[5:0]											*/
#define	CM_HIGH_THR	31	/*		0	0	0			cm_high_thr[5:0]											*/
#define	PPTHR12		32	/*		0				ppthr12[7:0]												*/
#define	PPTHR34		33	/*		0				ppthr34[7:0]												*/
#define	NPTHR56		34	/*		0				npthr56[7:0]												*/
#define	NLCR		35	/*						nclr[12:0]												*/
#define	NRTP		36	/*						nrtp[12:0]												*/
#define	NQ12		37	/*						nq12[12:0]												*/
#define	NQ34		38	/*						nq34[12:0]												*/
#define	NQ56		39	/*						nq56[12:0]												*/
#define	VCM_RING_DELTA	40	/*		0	0	0		vcm_ring_delta[3:0]												*/
#define	VMIN_DELTA	41	/*		0	0	0		vmin_delta[3:0]												*/
#define	VXTRA_DELTA	42	/*		0	0	0		vxtra_delta[3:0]												*/
#define	LCR_THR2	43	/*		0			lcr_thr_low[5:0]													*/
																					
#define	FSK_X_0		99	/*	x	sign				fsk_x_0[15:0]												*/
#define	FSK_COEFF_0	100	/*	x	sign				fsk_coeff_0[15:0]												*/
#define	FSK_X_1		101	/*	x	sign				fsk_x_1[15:0]												*/
#define	FSK_COEFF_1	102	/*	x	sign				fsk_coeff_1[15:0]												*/
#define	FSK_X_01	103	/*	x	sign				fsk_x_01[15:0]												*/
#define	FSK_X_10	104	/*	x	sign				fsk_x_10[15:0]												*/



#endif  /* REAL NAMES */


enum exceptions 
{
	PROSLICiNSANE,
	TIMEoUTpOWERuP,
	TIMEoUTpOWERdOWN,
	POWERlEAK,
	TIPoRrINGgROUNDsHORT,
	POWERaLARMQ1,
	POWERaLARMQ2,
	POWERaLARMQ3,
	POWERaLARMQ4, 
	POWERaLARMQ5,
	POWERaLARMQ6,
};

enum FXS_EVENT
{
	DTMF_DIGIT1 = 1,
	DTMF_DIGIT2 ,
	DTMF_DIGIT3 ,
	DTMF_DIGIT4 ,
	DTMF_DIGIT5 ,
	DTMF_DIGIT6 ,
	DTMF_DIGIT7 ,
	DTMF_DIGIT8 ,
	DTMF_DIGIT9 ,
	DTMF_DIGIT0 ,
	DTMF_DIGITSTART ,
	DTMF_DIGITHASH ,	// 12
	FXS_ONHOOK ,			// 13
	FXS_LEFT_ARROW ,
	FXS_RIGHT_ARROW ,
	FXS_CANCEL ,
	FXS_OK ,
	FXS_OFFHOOK ,		// 18
} ;
	
