/*
Copyright (c) 2008 Silicon Laboratories, Inc.
2009-03-09 16:06:28 */
/*ProSLIC API Tool Rev0.31 Beta*/


#include "proslic.h"
#include "si3217x.h"
Si3217x_General_Cfg Si3217x_General_Configuration  = {
BO_DCDC_BUCK_BOOST,	
0x5700000L,	/* VBATR_EXPECT */
0x39580EAL,	/* VBATH_EXPECT */
0x0L,	    /* DCDC_FSW_VTHLO */
0x0L,	    /* DCDC_FSW_VHYST */
0xC00000L,	/* DCDC_VREF_MIN */
0x1800000L,	/* DCDC_VREF_MIN_RING */
0x200000L,	/* DCDC_FSW_NORM */
0x200000L,	/* DCDC_FSW_NORM_LO */
0x200000L,	/* DCDC_FSW_RING */
0x200000L,	/* DCDC_FSW_RING_LO */
0xD980000L,	/* DCDC_DIN_LIM */
0xC00000L,	/* DCDC_VOUT_LIM */
0x10000000L,/* DCDC_DCFF_ENABLE */
0x100000L,	/* DCDC_UVHYST */
0x400000L,	/* DCDC_UVTHRESH */
0x400000L,	/* DCDC_OVTHRESH */
0x200000L,	/* DCDC_OITHRESH */
0x0L,	    /* DCDC_SWDRV_POL */
0x300000L,	/* DCDC_SWFET */
0x600000L,	/* DCDC_VREF_CTRL */
0x0L,	    /* DCDC_RNGTYPE */
0x300000L,	/* DCDC_ANA_GAIN */
0x600000L,	/* DCDC_ANA_TOFF */
0x300000L,	/* DCDC_ANA_TONMIN */
0xF00000,	/* DCDC_ANA_TONMAX */	//THLin: change DCDC_ANA_TONMAX from 0x800000 modify to 0xF00000 to fix REN issue
0xF00000L,	/* DCDC_ANA_DSHIFT */
0xFDA4000L,	/* DCDC_ANA_LPOLY */
0x7FeF000L,	/* COEF_P_HVIC */
0x48D600L,	/* P_TH_HVIC */
0x0,	    /* CM_CLAMP */
0x3F,	    /* AUTO */
0x1,	    /* DAA_CNTL */
0xFF,	    /* IRQEN1 */
0xFF,	    /* IRQEN2 */
0xFF,	    /* IRQEN3 */
0xFF,	    /* IRQEN4 */
0x30,	    /* ENHANCE */
0,	        /* DAA_ENABLE */
0x3A2E8BAL,	/* SCALE_KAUDIO */
0x99999AL,	/* AC_ADC_GAIN */
};

Si3217x_DTMFDec_Cfg Si3217x_DTMFDec_Presets[] = {
	{0x2d40000L,0x1a660000L,0x2d40000L,0x6ba0000L,0x1dcc0000L,0x33f0000L,0xbd30000L,0x19d20000L,0x4150000L,0x188F0000L,0x4150000L,0xd970000L,0x18620000L,0xf1c0000L}
};
Si3217x_GPIO_Cfg Si3217x_GPIO_Configuration = {
	0,0,0,0,0,0,0
};

Si3217x_PulseMeter_Cfg Si3217x_PulseMeter_Presets[] ={

    /* inputs:  freq = 12kHz, amp = 1.000Vrms, cal = First, ramp = 24kHz, power = Normal */
    { 0x7A2B6AL, 0x0, 0x0 }
};

Si3217x_CI_Cfg Si3217x_CI_Presets [] = {
	{0}
};
Si3217x_audioGain_Cfg Si3217x_audioGain_Presets [] = {
	{0x1377080L,0},
	{0x80C3180L,0}
};

#if 1 //THLin: calculate from proslic API config tool v0.8
Si3217x_Ring_Cfg Si3217x_Ring_Presets[] ={
{
0x00050000L,	/* RTPER */
0x07EFE000L,	/* RINGFR (20.000 Hz) */
0x00175338L,	/* RINGAMP (38.000 vrms)  */
0x00000000L,	/* RINGPHAS */
0x00000000L,	/* RINGOF (0.000 vdc) */
0x15E5200EL,	/* SLOPE_RING (100.000 ohms) */
0x00D16348L,	/* IRING_LIM (90.000 mA) */
0x005897D6L,	/* RTACTH (48.924 mA) */
0x0FFFFFFFL,	/* RTDCTH (450.000 mA) */
0x00006000L,	/* RTACDB (75.000 ms) */
0x00006000L,	/* RTDCDB (75.000 ms) */
0x00C49BA0L,	/* VOV_RING_BAT (12.000 v) */
0x00000000L,	/* VOV_RING_GND (0.000 v) */
0x04A259CBL,	/* VBATR_EXPECT (72.409 v) */
0x80,			/* RINGTALO (2.000 s) */
0x3E,			/* RINGTAHI */
0x00,			/* RINGTILO (4.000 s) */
0x7D,			/* RINGTIHI */
0x00000000L,	/* ADAP_RING_MIN_I */
0x00003000L,	/* COUNTER_IRING_VAL */
0x00051EB8L,	/* COUNTER_VTR_VAL */
0x00000000L,	/* CONST_028 */
0x00000000L,	/* CONST_032 */
0x00000000L,	/* CONST_038 */
0x00000000L,	/* CONST_046 */
0x00000000L,	/* RRD_DELAY */
0x00000000L,	/* RRD_DELAY2 */
0x01893740L,	/* DCDC_VREF_MIN_RNG */
0x58,			/* RINGCON */
0x00,			/* USERSTAT */
0x02512CE5L,	/* VCM_RING (33.205 v) */
0x02512CE5L,	/* VCM_RING_FIXED */
0x003126E8L,	/* DELTA_VCM */
0xCCCCCCCCL,	/* DCDC_RNGTYPE */
}/* 20Hz_38VRMS_0VDC_BAL_600ohmCPE */
,
{
0x00050000L,	/* RTPER */
0x07EFE000L,	/* RINGFR (20.000 Hz) */
0x001B9F2EL,	/* RINGAMP (45.000 vrms)  */
0x00000000L,	/* RINGPHAS */
0x00000000L,	/* RINGOF (0.000 vdc) */
0x15E5200EL,	/* SLOPE_RING (100.000 ohms) */
0x00D16348L,	/* IRING_LIM (90.000 mA) */
0x0068E9B4L,	/* RTACTH (57.936 mA) */
0x0FFFFFFFL,	/* RTDCTH (450.000 mA) */
0x00006000L,	/* RTACDB (75.000 ms) */
0x00006000L,	/* RTDCDB (75.000 ms) */
0x00C49BA0L,	/* VOV_RING_BAT (12.000 v) */
0x00000000L,	/* VOV_RING_GND (0.000 v) */
0x0558ABFCL,	/* VBATR_EXPECT (83.537 v) */
0x80,			/* RINGTALO (2.000 s) */
0x3E,			/* RINGTAHI */
0x00,			/* RINGTILO (4.000 s) */
0x7D,			/* RINGTIHI */
0x00000000L,	/* ADAP_RING_MIN_I */
0x00003000L,	/* COUNTER_IRING_VAL */
0x00051EB8L,	/* COUNTER_VTR_VAL */
0x00000000L,	/* CONST_028 */
0x00000000L,	/* CONST_032 */
0x00000000L,	/* CONST_038 */
0x00000000L,	/* CONST_046 */
0x00000000L,	/* RRD_DELAY */
0x00000000L,	/* RRD_DELAY2 */
0x01893740L,	/* DCDC_VREF_MIN_RNG */
0x58,			/* RINGCON */
0x00,			/* USERSTAT */
0x02AC55FEL,	/* VCM_RING (38.769 v) */
0x02AC55FEL,	/* VCM_RING_FIXED */
0x003126E8L,	/* DELTA_VCM */
0xCCCCCCCCL,	/* DCDC_RNGTYPE */
}/* 20Hz_45VRMS_0VDC_BAL_600ohmCPE */
,
{
0x00050000L,	/* RTPER */
0x07EFE000L,	/* RINGFR (20.000 Hz) */
0x001D7698L,	/* RINGAMP (48.000 vrms)  */
0x00000000L,	/* RINGPHAS */
0x00000000L,	/* RINGOF (0.000 vdc) */
0x15E5200EL,	/* SLOPE_RING (100.000 ohms) */
0x00D16348L,	/* IRING_LIM (90.000 mA) */
0x006FE837L,	/* RTACTH (61.799 mA) */
0x0FFFFFFFL,	/* RTDCTH (450.000 mA) */
0x00006000L,	/* RTACDB (75.000 ms) */
0x00006000L,	/* RTDCDB (75.000 ms) */
0x00C49BA0L,	/* VOV_RING_BAT (12.000 v) */
0x00000000L,	/* VOV_RING_GND (0.000 v) */
0x05A6CF35L,	/* VBATR_EXPECT (88.306 v) */
0x80,			/* RINGTALO (2.000 s) */
0x3E,			/* RINGTAHI */
0x00,			/* RINGTILO (4.000 s) */
0x7D,			/* RINGTIHI */
0x00000000L,	/* ADAP_RING_MIN_I */
0x00003000L,	/* COUNTER_IRING_VAL */
0x00051EB8L,	/* COUNTER_VTR_VAL */
0x00000000L,	/* CONST_028 */
0x00000000L,	/* CONST_032 */
0x00000000L,	/* CONST_038 */
0x00000000L,	/* CONST_046 */
0x00000000L,	/* RRD_DELAY */
0x00000000L,	/* RRD_DELAY2 */
0x01893740L,	/* DCDC_VREF_MIN_RNG */
0x58,			/* RINGCON */
0x00,			/* USERSTAT */
0x02D3679AL,	/* VCM_RING (41.153 v) */
0x02D3679AL,	/* VCM_RING_FIXED */
0x003126E8L,	/* DELTA_VCM */
0xCCCCCCCCL,	/* DCDC_RNGTYPE */
}/* 20Hz_48VRMS_0VDC_BAL_600ohmCPE */
,
{
0x00040000L,	/* RTPER */
0x07E6C000L,	/* RINGFR (25.000 Hz) */
0x0024DF0DL,	/* RINGAMP (48.000 vrms)  */
0x00000000L,	/* RINGPHAS */
0x00000000L,	/* RINGOF (0.000 vdc) */
0x15E5200EL,	/* SLOPE_RING (100.000 ohms) */
0x00D16348L,	/* IRING_LIM (90.000 mA) */
0x0059D242L,	/* RTACTH (62.003 mA) */
0x0FFFFFFFL,	/* RTDCTH (450.000 mA) */
0x00008000L,	/* RTACDB (75.000 ms) */
0x00008000L,	/* RTDCDB (75.000 ms) */
0x00C49BA0L,	/* VOV_RING_BAT (12.000 v) */
0x00000000L,	/* VOV_RING_GND (0.000 v) */
0x05A75077L,	/* VBATR_EXPECT (88.337 v) */
0x80,			/* RINGTALO (2.000 s) */
0x3E,			/* RINGTAHI */
0x00,			/* RINGTILO (4.000 s) */
0x7D,			/* RINGTIHI */
0x00000000L,	/* ADAP_RING_MIN_I */
0x00003000L,	/* COUNTER_IRING_VAL */
0x00066666L,	/* COUNTER_VTR_VAL */
0x00000000L,	/* CONST_028 */
0x00000000L,	/* CONST_032 */
0x00000000L,	/* CONST_038 */
0x00000000L,	/* CONST_046 */
0x00000000L,	/* RRD_DELAY */
0x00000000L,	/* RRD_DELAY2 */
0x01893740L,	/* DCDC_VREF_MIN_RNG */
0x58,			/* RINGCON */
0x00,			/* USERSTAT */
0x02D3A83BL,	/* VCM_RING (41.169 v) */
0x02D3A83BL,	/* VCM_RING_FIXED */
0x003126E8L,	/* DELTA_VCM */
0xCCCCCCCCL,	/* DCDC_RNGTYPE */
}/* 25Hz_48VRMS_0VDC_BAL_600ohmCPE */
,
{
0x0005E000L,	/* RTPER */
0x07F46000L,	/* RINGFR (17.000 Hz) */
0x001A110DL,	/* RINGAMP (50.000 vrms)  */
0x00000000L,	/* RINGPHAS */
0x00000000L,	/* RINGOF (0.000 vdc) */
0x15E5200EL,	/* SLOPE_RING (100.000 ohms) */
0x00D16348L,	/* IRING_LIM (90.000 mA) */
0x00887E15L,	/* RTACTH (64.150 mA) */
0x0FFFFFFFL,	/* RTDCTH (450.000 mA) */
0x00006000L,	/* RTACDB (75.000 ms) */
0x00006000L,	/* RTDCDB (75.000 ms) */
0x00C49BA0L,	/* VOV_RING_BAT (12.000 v) */
0x00000000L,	/* VOV_RING_GND (0.000 v) */
0x05DA58A6L,	/* VBATR_EXPECT (91.452 v) */
0x80,			/* RINGTALO (2.000 s) */
0x3E,			/* RINGTAHI */
0x00,			/* RINGTILO (4.000 s) */
0x7D,			/* RINGTIHI */
0x00000000L,	/* ADAP_RING_MIN_I */
0x00003000L,	/* COUNTER_IRING_VAL */
0x00045A1CL,	/* COUNTER_VTR_VAL */
0x00000000L,	/* CONST_028 */
0x00000000L,	/* CONST_032 */
0x00000000L,	/* CONST_038 */
0x00000000L,	/* CONST_046 */
0x00000000L,	/* RRD_DELAY */
0x00000000L,	/* RRD_DELAY2 */
0x01893740L,	/* DCDC_VREF_MIN_RNG */
0x58,			/* RINGCON */
0x00,			/* USERSTAT */
0x02ED2C53L,	/* VCM_RING (42.726 v) */
0x02ED2C53L,	/* VCM_RING_FIXED */
0x003126E8L,	/* DELTA_VCM */
0xCCCCCCCCL,	/* DCDC_RNGTYPE */
}/* 17Hz_50VRMS_0VDC_BAL_600ohmCPE */
,
{
0x00050000L,	/* RTPER */
0x07EFE000L,	/* RINGFR (20.000 Hz) */
0x00188D7EL,	/* RINGAMP (40.000 vrms)  */
0x00000000L,	/* RINGPHAS */
0x00000000L,	/* RINGOF (0.000 vdc) */
0x15E5200EL,	/* SLOPE_RING (100.000 ohms) */
0x00D16348L,	/* IRING_LIM (90.000 mA) */
0x005D4183L,	/* RTACTH (51.499 mA) */
0x0FFFFFFFL,	/* RTDCTH (450.000 mA) */
0x00006000L,	/* RTACDB (75.000 ms) */
0x00006000L,	/* RTDCDB (75.000 ms) */
0x00C49BA0L,	/* VOV_RING_BAT (12.000 v) */
0x00000000L,	/* VOV_RING_GND (0.000 v) */
0x04D67147L,	/* VBATR_EXPECT (75.589 v) */
0x80,			/* RINGTALO (2.000 s) */
0x3E,			/* RINGTAHI */
0x00,			/* RINGTILO (4.000 s) */
0x7D,			/* RINGTIHI */
0x00000000L,	/* ADAP_RING_MIN_I */
0x00003000L,	/* COUNTER_IRING_VAL */
0x00051EB8L,	/* COUNTER_VTR_VAL */
0x00000000L,	/* CONST_028 */
0x00000000L,	/* CONST_032 */
0x00000000L,	/* CONST_038 */
0x00000000L,	/* CONST_046 */
0x00000000L,	/* RRD_DELAY */
0x00000000L,	/* RRD_DELAY2 */
0x01893740L,	/* DCDC_VREF_MIN_RNG */
0x58,			/* RINGCON */
0x00,			/* USERSTAT */
0x026B38A3L,	/* VCM_RING (34.794 v) */
0x026B38A3L,	/* VCM_RING_FIXED */
0x003126E8L,	/* DELTA_VCM */
0xCCCCCCCCL,	/* DCDC_RNGTYPE */
}/* 20Hz_40VRMS_0VDC_BAL_600ohmCPE */

};

#else

Si3217x_Ring_Cfg Si3217x_Ring_Presets[] ={
{ /* RING_F20_45VRMS_0VDC_LPR */
0x00050000L,		/* RTPER */
0x07EFD9D5L,		/* RINGFR */
0x001BD493L,		/* RINGAMP */
0x00000000L,		/* RINGPHAS */
0x00000000L,		/* RINGOF */
0x15E5200EL,		/* SLOPE_RING */
0x00D16348L,		/* IRING_LIM */
0x0068C6BBL,		/* RTACTH */
0x0FFFFFFFL,		/* RTDCTH */
0x00006000L,		/* RTACDB */
0x00006000L,		/* RTDCDB */
0x0064874DL,		/* VOV_RING_BAT */
0x0064874DL,		/* VOV_RING_GND */
0x049CE106L,		/* VBATR_EXPECT */
0x80,		/* RINGTALO */
0x3E,		/* RINGTAHI */
0x00,		/* RINGTILO */
0x7D,		/* RINGTIHI */
0x01000547L,		/* ADAP_RING_MIN_I */
0x00003000L,		/* COUNTER_IRING_VAL */
0x00051EB8L,		/* COUNTER_VTR_VAL */
0x0163063FL,		/* CONST_028 */
0x019E31F4L,		/* CONST_032 */
0x01F108BEL,		/* CONST_036 */
0x026D4AEEL,		/* CONST_046 */
0x00370000L,		/* RRD_DELAY */
0x00190000L,		/* RRD_DELAY2 */
0x01893740L,		/* DCDC_VREF_MIN_RNG */
0x58,		/* RINGCON */
0x01,		/* USERSTAT */
0x024E7083L,		/* VCM_RING */
0x024E7083L,		/* VCM_RING_FIXED */
0x003126E8L,		/* DELTA_VCM */
0x200000L,		/* DCDC_RNGTYPE */
},
{ /* RING_F20_45VRMS_0VDC_BAL */
0x00050000L,		/* RTPER */
0x07EFD9D5L,		/* RINGFR */
0x001BD493L,		/* RINGAMP */
0x00000000L,		/* RINGPHAS */
0x00000000L,		/* RINGOF */
0x15E5200EL,		/* SLOPE_RING */
0x00D16348L,		/* IRING_LIM */
0x0068C6BBL,		/* RTACTH */
0x0FFFFFFFL,		/* RTDCTH */
0x00006000L,		/* RTACDB */
0x00006000L,		/* RTDCDB */
0x0064874DL,		/* VOV_RING_BAT */
0x0064874DL,		/* VOV_RING_GND */
0x0565EFA2L,		/* VBATR_EXPECT */
0x80,		/* RINGTALO */
0x3E,		/* RINGTAHI */
0x00,		/* RINGTILO */
0x7D,		/* RINGTIHI */
0x01000547L,		/* ADAP_RING_MIN_I */
0x00003000L,		/* COUNTER_IRING_VAL */
0x00051EB8L,		/* COUNTER_VTR_VAL */
0x0163063FL,		/* CONST_028 */
0x019E31F4L,		/* CONST_032 */
0x01F108BEL,		/* CONST_036 */
0x026D4AEEL,		/* CONST_046 */
0x00370000L,		/* RRD_DELAY */
0x00190000L,		/* RRD_DELAY2 */
0x01893740L,		/* DCDC_VREF_MIN_RNG */
0x58,		/* RINGCON */
0x00,		/* USERSTAT */
0x02B2F7D1L,		/* VCM_RING */
0x02B2F7D1L,		/* VCM_RING_FIXED */
0x003126E8L,		/* DELTA_VCM */
0x200000L,		/* DCDC_RNGTYPE */
},
{ /* RING_F20_55VRMS_48VDC_LPR */
0x00050000L,		/* RTPER */
0x07EFD9D5L,		/* RINGFR */
0x002203D0L,		/* RINGAMP */
0x00000000L,		/* RINGPHAS */
0x049A5F69L,		/* RINGOF */
0x15E5200EL,		/* SLOPE_RING */
0x00D16348L,		/* IRING_LIM */
0x032EDF91L,		/* RTACTH */
0x0038E38EL,		/* RTDCTH */
0x00006000L,		/* RTACDB */
0x00006000L,		/* RTDCDB */
0x007ADE42L,		/* VOV_RING_BAT */
0x007ADE42L,		/* VOV_RING_GND */
0x08B5BA6CL,		/* VBATR_EXPECT */
0x80,		/* RINGTALO */
0x3E,		/* RINGTAHI */
0x00,		/* RINGTILO */
0x7D,		/* RINGTIHI */
0x0138EA01L,		/* ADAP_RING_MIN_I */
0x00003000L,		/* COUNTER_IRING_VAL */
0x00051EB8L,		/* COUNTER_VTR_VAL */
0x01A40BA6L,		/* CONST_028 */
0x01EA0D97L,		/* CONST_032 */
0x024C104FL,		/* CONST_036 */
0x02DF1463L,		/* CONST_046 */
0x00370000L,		/* RRD_DELAY */
0x00190000L,		/* RRD_DELAY2 */
0x01893740L,		/* DCDC_VREF_MIN_RNG */
0x58,		/* RINGCON */
0x01,		/* USERSTAT */
0x045ADD36L,		/* VCM_RING */
0x045ADD36L,		/* VCM_FIXED_RING */
0x003126E8L,		/* DELTA_VCM */
0x200000L,		/* DCDC_RNGTYPE */
}
};
#endif

Si3217x_DCfeed_Cfg Si3217x_DCfeed_Presets[] = {
{ /* DCFEED_48V_20MA */
0x1DDF41C9L,    /* SLOPE_VLIM */
0x1EF68D5EL,    /* SLOPE_RFEED */
0x40A0E0L,      /* SLOPE_ILIM */
0x18AAD168L,    /* SLOPE_DELTA1 */
0x1CA39FFAL,    /* SLOPE_DELTA2 */
0x5A38633L,     /* V_VLIM */
0x5072476L,     /* V_RFEED */
0x3E67006L,     /* V_ILIM */
0xFDFAB5L,      /* CONST_RFEED */
0x5D0FA6L,      /* CONST_ILIM */
0x2D8D96L,      /* I_VLIM */
// thlin
#if 0 // default
0x5B0AFBL,      /* LCRONHK */
0x6D4060L,      /* LCROFFHK */
#else // for 20pps pulse dial det
0x500000L,      /* LCRONHK */
0x600000L,      /* LCROFFHK */
#endif
0x8000L,        /* LCRDBI */
0x48D595L,      /* LONGHITH */
0x3FBAE2L,      /* LONGLOTH */
0x8000L,        /* LONGDBI */
0x120000L,       /* LCRMASK */
0x80000L,       /* LCRMASK_POLREV */
0x140000L,      /* LCRMASK_STATE */
0x140000L,      /* LCRMASK_LINECAP */
0x1BA5E35L,     /* VCM_OH */
0x418937L,      /* VOV_BAT */
0x418937L       /* VOV_GND */
},
{ /* DCFEED_48V_25MA */
0x1D46EA04L,    /* SLOPE_VLIM */
0x1EA4087EL,    /* SLOPE_RFEED */
0x40A0E0L,      /* SLOPE_ILIM */
0x1A224120L,    /* SLOPE_DELTA1 */
0x1D4FB32EL,    /* SLOPE_DELTA2 */
0x5A38633L,     /* V_VLIM */ 
0x5072476L,     /* V_RFEED */ 
0x3E67006L,     /* V_ILIM */ 
0x13D7962L,     /* CONST_RFEED */ 
0x74538FL,      /* CONST_ILIM */ 
0x2D8D96L,      /* I_VLIM */ 
// thlin
#if 0 // default
0x5B0AFBL,      /* LCRONHK */
0x6D4060L,      /* LCROFFHK */
#else // for 20pps pulse dial det
0x500000L,      /* LCRONHK */
0x600000L,      /* LCROFFHK */
#endif
0x8000L,        /* LCRDBI */
0x48D595L,      /* LONGHITH */
0x3FBAE2L,      /* LONGLOTH */
0x8000L,        /* LONGDBI */
0x120000L,       /* LCRMASK */
0x80000L,       /* LCRMASK_POLREV */
0x140000L,      /* LCRMASK_STATE */
0x140000L,      /* LCRMASK_LINECAP */
0x1BA5E35L,     /* VCM_OH */
0x418937L,      /* VOV_BAT */
0x418937L       /* VOV_GND */
}
};

#if 1

Si3217x_Impedance_Cfg Si3217x_Impedance_Presets[] ={
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=600_0_0 rprot=30 rfuse=0 emi_cap=10*/
{
{0x07F46C00L, 0x000E4600L, 0x00008580L, 0x1FFD6100L,    /* TXACEQ */
 0x07EF5000L, 0x0013F580L, 0x1FFDE000L, 0x1FFCB280L},   /* RXACEQ */
{0x0027CB00L, 0x1F8A8880L, 0x02801180L, 0x1F625C80L,    /* ECFIR/ECIIR */
 0x0314FB00L, 0x1E6B8E80L, 0x00C5FF00L, 0x1FC96F00L,
 0x1FFD1200L, 0x00023C00L, 0x0ED29D00L, 0x192A9400L},
{0x00810E00L, 0x1EFEBE80L, 0x00803500L, 0x0FF66D00L,    /* ZSYNTH */
 0x18099080L, 0x59}, 
 0x088E0D80L,   /* TXACGAIN */
 0x01456D80L,   /* RXACGAIN */
 0x07ABE580L, 0x18541B00L, 0x0757CB00L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /*  */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=900_0_0 rprot= rfuse= emi_cap=10*/
{
{0x06D02180L, 0x00234A00L, 0x1FFE1B80L, 0x1FFFA980L,    /* TXACEQ */
 0x07DD8F80L, 0x002C2B00L, 0x1FF9FE00L, 0x1FFDF780L},   /* RXACEQ */
{0x00117700L, 0x00063380L, 0x02193F80L, 0x0197A500L,    /* ECFIR/ECIIR */
 0x01F8E200L, 0x1FAF6C80L, 0x1FF5B100L, 0x003DE280L,
 0x1FAEB480L, 0x00465880L, 0x00E9E680L, 0x06ECCC00L},
{0x00624000L, 0x1F8FAE80L, 0x000E5100L, 0x0AC84A00L,    /* ZSYNTH */
 0x1D35A180L, 0x2C}, 
 0x08000000L,   /* TXACGAIN */
 0x0106C500L,   /* RXACGAIN */
 0x07B13C80L, 0x184EC400L, 0x07627880L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 900 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=600_INF_1000 rprot= rfuse= emi_cap=10*/
{
{0x07F83980L, 0x00056200L, 0x1FFEE880L, 0x1FFB1900L,    /* TXACEQ */
 0x08405000L, 0x001DD200L, 0x0003DB80L, 0x00008700L},   /* RXACEQ */
{0x1FEE8700L, 0x009B2B00L, 0x000C9680L, 0x03700100L,    /* ECFIR/ECIIR */
 0x1F62E400L, 0x01C77400L, 0x1FBBCF80L, 0x0089D500L,
 0x005CFF80L, 0x1FA96E80L, 0x0F679480L, 0x18962A80L},
{0x00657C00L, 0x1F2FA580L, 0x006ADE00L, 0x0FE12100L,    /* ZSYNTH */
 0x181ED080L, 0x57}, 
 0x08618D80L,   /* TXACGAIN */
 0x013E3400L,   /* RXACGAIN */
 0x0717CE80L, 0x18E83200L, 0x062F9C80L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 600+1000nF */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=200_560_100 rprot=30 rfuse=0 emi_cap=10*/
{
{0x07B53100L, 0x1FCCEE80L, 0x00048B80L, 0x1FFD3A80L,    /* TXACEQ */
 0x09562C80L, 0x1DDD4C80L, 0x00544380L, 0x1FE66D80L},   /* RXACEQ */
{0x000E5780L, 0x00064900L, 0x00D56800L, 0x01BEC580L,    /* ECFIR/ECIIR */
 0x01210680L, 0x00A6B700L, 0x00873A80L, 0x1F7DA700L,
 0x00905900L, 0x1F6A5F00L, 0x02B04D00L, 0x052DD380L},
{0x011EBD00L, 0x1CF39680L, 0x01ED7300L, 0x0A135500L,    /* ZSYNTH */
 0x1DEA4180L, 0x95}, 
 0x08000000L,   /* TXACGAIN */
 0x011FFC00L,   /* RXACGAIN */
 0x07B81580L, 0x1847EB00L, 0x07702B80L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 200+560||100 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=200_680_100 rprot=30 rfuse=0 emi_cap=10*/
{
{0x073A7B00L, 0x1FCEB400L, 0x0002C680L, 0x1FFD0780L,    /* TXACEQ */
 0x09BA8580L, 0x1D2DF780L, 0x006F5000L, 0x1FDFE200L},   /* RXACEQ */
{0x0004B700L, 0x000F9800L, 0x01201200L, 0x00E1D880L,    /* ECFIR/ECIIR */
 0x03314A00L, 0x1E84A580L, 0x029D2380L, 0x1E6F3400L,
 0x00E99200L, 0x1F121100L, 0x0588BC00L, 0x025CAE00L},
{0x01415C00L, 0x1C98C180L, 0x0225A500L, 0x0A138200L,    /* ZSYNTH */
 0x1DEA2280L, 0x8E}, 
 0x08000000L,   /* TXACGAIN */
 0x010DFD80L,   /* RXACGAIN */
 0x07BA2180L, 0x1845DF00L, 0x07744380L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 200+680||100 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=270_750_150 rprot=30 rfuse=0 emi_cap=10*/
{
{0x071F7A80L, 0x1FD01280L, 0x00132700L, 0x1FFEF980L,    /* TXACEQ */
 0x0A8AA300L, 0x1B9A5500L, 0x008E7F00L, 0x1FD7F300L},   /* RXACEQ */
{0x0068CA00L, 0x1EAE1E00L, 0x0394FA00L, 0x1E94AE80L,    /* ECFIR/ECIIR */
 0x0356D800L, 0x0166CA80L, 0x1EC16380L, 0x01DE2780L,
 0x1F852300L, 0x0046BE80L, 0x02F17C80L, 0x1EBCD280L},
{0x028A0C00L, 0x19EE4580L, 0x03876100L, 0x0A762700L,    /* ZSYNTH */
 0x1D87A380L, 0x93}, 
 0x08000000L,   /* TXACGAIN */
 0x0109C280L,   /* RXACGAIN */
 0x07BC6F00L, 0x18439180L, 0x0778DE00L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 270+750||150 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=220_820_120 rprot=30 rfuse=0 emi_cap=10*/
{
{0x06E38480L, 0x1FD33B00L, 0x00069780L, 0x1FFCAB80L,    /* TXACEQ */
 0x0A78F680L, 0x1BC5C880L, 0x009AEA00L, 0x1FD66D80L},   /* RXACEQ */
{0x00378B00L, 0x1F3FCA00L, 0x02B5ED00L, 0x1F2B6200L,    /* ECFIR/ECIIR */
 0x04189080L, 0x1F8A4480L, 0x01113680L, 0x00373100L,
 0x001DAE80L, 0x1FE02F00L, 0x0C89C780L, 0x1B689680L},
{0x02391100L, 0x1A886080L, 0x033E3B00L, 0x0A136200L,    /* ZSYNTH */
 0x1DEA4180L, 0x8C}, 
 0x08000000L,   /* TXACGAIN */
 0x01019200L,   /* RXACGAIN */
 0x07BD1680L, 0x1842EA00L, 0x077A2D00L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 220+820||120 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=220_820_115 rprot=30 rfuse=0 emi_cap=10*/
{
{0x06D56380L, 0x1FDF1900L, 0x00095A00L, 0x1FFDAA80L,    /* TXACEQ */
 0x0A596300L, 0x1C067880L, 0x0095EF00L, 0x1FD7AF00L},   /* RXACEQ */
{0x00687000L, 0x1EAE1800L, 0x03983D80L, 0x1EB14B00L,    /* ECFIR/ECIIR */
 0x037B3E80L, 0x016FC900L, 0x1ED60100L, 0x01B17D80L,
 0x1FA20D00L, 0x001CE900L, 0x027D3380L, 0x1DBDBA80L},
{0x00246300L, 0x1E5E0580L, 0x017D2300L, 0x0A138100L,    /* ZSYNTH */
 0x1DEA2280L, 0xA7}, 
 0x08000000L,   /* TXACGAIN */
 0x01009500L,   /* RXACGAIN */
 0x07BBEE80L, 0x18441200L, 0x0777DD80L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 220+820||115 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=400_700_200 rprot=30 rfuse=0 emi_cap=10*/
{
{0x06FC6880L, 0x1FE12200L, 0x00025500L, 0x1FFE7A00L,    /* TXACEQ */
 0x0A23C480L, 0x1C2BF980L, 0x1FF5B780L, 0x1FED5880L},   /* RXACEQ */
{0x00160F00L, 0x1FF2F880L, 0x0134E300L, 0x01D77400L,    /* ECFIR/ECIIR */
 0x01329D00L, 0x01752D00L, 0x000EA100L, 0x00881A00L,
 0x002CE900L, 0x1FD1C400L, 0x0D377680L, 0x1ABB2A00L},
{0x003DBD00L, 0x1F5CDE80L, 0x00655200L, 0x0EB5D300L,    /* ZSYNTH */
 0x19496A80L, 0x71}, 
 0x08000000L,   /* TXACGAIN */
 0x01068B00L,   /* RXACGAIN */
 0x07B8AD80L, 0x18475300L, 0x07715B00L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 400+700||200 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=200_1000_100 rprot=30 rfuse=50 emi_cap=10*/
{
{0x06C36000L, 0x1FB87580L, 0x00025D00L, 0x1FFA3F80L,    /* TXACEQ */
 0x0AB92680L, 0x1B448B00L, 0x00BFC580L, 0x1FCCCF80L},   /* RXACEQ */
{0x00210000L, 0x1FC5E280L, 0x015BF200L, 0x01C9BE00L,    /* ECFIR/ECIIR */
 0x018EE300L, 0x01FCF780L, 0x00062C00L, 0x00A18680L,
 0x00262600L, 0x1FD72480L, 0x0C93B080L, 0x1B63C180L},
{0x1FFFE980L, 0x1E861680L, 0x01797F00L, 0x0A138100L,    /* ZSYNTH */
 0x1DEA2380L, 0xA8}, 
 0x08000000L,   /* TXACGAIN */
 0x00FA5980L,   /* RXACGAIN */
 0x07BEC100L, 0x18413F80L, 0x077D8180L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 200+1000||100 R=50 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=150_830_72 rprot=30 rfuse=4 emi_cap=10*/
{
{0x06C61F80L, 0x1FEDC200L, 0x000A4C80L, 0x1FFD5980L,    /* TXACEQ */
 0x09746080L, 0x1DA76280L, 0x006BE900L, 0x1FDED780L},   /* RXACEQ */
{0x00078F80L, 0x0013A900L, 0x00E97680L, 0x02038500L,    /* ECFIR/ECIIR */
 0x019FDE80L, 0x01169700L, 0x005BED80L, 0x1FCC5400L,
 0x00699800L, 0x1F91FA80L, 0x023E0000L, 0x059F0380L},
{0x1F502180L, 0x01216300L, 0x1F4CF480L, 0x0EB01C00L,    /* ZSYNTH */
 0x18093280L, 0x9F}, 
 0x08000000L,   /* TXACGAIN */
 0x00FFE280L,   /* RXACGAIN */
 0x07B89400L, 0x18476C80L, 0x07712800L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 150+830||72 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=215_1000_137 rprot=30 rfuse=0 emi_cap=10*/
{
{0x06B8F480L, 0x1FD68A80L, 0x0006F100L, 0x1FFC9E00L,    /* TXACEQ */
 0x0B23A880L, 0x1A3F0A00L, 0x00BF3A80L, 0x1FCDF180L},   /* RXACEQ */
{0x00173680L, 0x1FD13A80L, 0x014F9E80L, 0x01285A00L,    /* ECFIR/ECIIR */
 0x0211A080L, 0x013D4900L, 0x00933D00L, 0x00962100L,
 0x00439100L, 0x1FBA9500L, 0x0D1BBC80L, 0x1AD88700L},
{0x1F257880L, 0x00D78D00L, 0x0002B600L, 0x0D102F00L,    /* ZSYNTH */
 0x1AEEBF80L, 0xBC}, 
 0x08000000L,   /* TXACGAIN */
 0x00FB9C00L,   /* RXACGAIN */
 0x07C0AF80L, 0x183F5100L, 0x07815F00L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 215+1000||137 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=300_1000_220 rprot=30 rfuse=0 emi_cap=10*/
{
{0x074FA800L, 0x1FCA8780L, 0x00047880L, 0x1FFDFE00L,    /* TXACEQ */
 0x0AE6E080L, 0x19FA5A00L, 0x00019980L, 0x1FE4D780L},   /* RXACEQ */
{0x1FDB4F00L, 0x00E48280L, 0x1ECE1400L, 0x04DB4400L,    /* ECFIR/ECIIR */
 0x1DCCF080L, 0x04008700L, 0x1F366C80L, 0x0103ED80L,
 0x00651800L, 0x1F99F080L, 0x0E032980L, 0x19F7F380L},
{0x1FE09F80L, 0x1FEA2580L, 0x00350D00L, 0x0EAC2600L,    /* ZSYNTH */
 0x19531180L, 0xA4}, 
 0x08000000L,   /* TXACGAIN */
 0x01109680L,   /* RXACGAIN */
 0x07BD5B00L, 0x1842A580L, 0x077AB600L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 300+1000||220 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=900_INF_2160 rprot=30 rfuse= emi_cap=10*/
{
{0x06CF0B00L, 0x00212200L, 0x1FFBDB00L, 0x1FFDF180L,    /* TXACEQ */
 0x07E5EB80L, 0x002F5B80L, 0x1FFA2F00L, 0x1FFE3A80L},   /* RXACEQ */
{0x001FA000L, 0x1FDA6A80L, 0x02626580L, 0x0172FF80L,    /* ECFIR/ECIIR */
 0x0247E800L, 0x1FC47680L, 0x003D1F00L, 0x004AEA00L,
 0x1FFDEF80L, 0x00478E80L, 0x01D1A600L, 0x060A4280L},
{0x00656400L, 0x1F731280L, 0x00273000L, 0x0C0B2700L,    /* ZSYNTH */
 0x1BEF1B80L, 0x2B}, 
 0x08000000L,   /* TXACGAIN */
 0x0106B880L,   /* RXACGAIN */
 0x078FDD00L, 0x18702380L, 0x071FBA00L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /* 900+2160 */
 /*Source: Script file: C:\Documents and Settings\thlin\????\SLIC constance\250_750_150nF.txt */
{
{0x08277600L, 0x1FB1BE80L, 0x1FFC1100L, 0x1FEED700L,    /* TXACEQ */
 0x09137680L, 0x1FBB6580L, 0x1FFF3600L, 0x1FF38580L},   /* RXACEQ */
{0x1F9E8C80L, 0x003F7200L, 0x01343A80L, 0x1F6F7080L,    /* ECFIR/ECIIR */
 0x013CA900L, 0x00769480L, 0x1FF8B380L, 0x1F943780L,
 0x00FF1000L, 0x1FAF0B00L, 0x002BAB00L, 0x0089A780L},
{0x08CC5600L, 0x10000300L, 0x07089480L, 0x0073CD80L,    /* ZSYNTH */
 0x0716D180L, 0xF1}, 
 0x0CD02680L,   /* TXACGAIN */
 0x01CFF400L,   /* RXACGAIN */
 0x069BB380L, 0x19644C80L, 0x05376700L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },  /*  */
/*Source: Script file: C:\Documents and Settings\thlin\????\SLIC constance\320_1150_230nF.txt */
{
{0x0808B300L, 0x1FE95600L, 0x00000C80L, 0x1FF7CB00L,    /* TXACEQ */
 0x084B8400L, 0x00001D80L, 0x00004100L, 0x00009D80L},   /* RXACEQ */
{0x1FEFB980L, 0x0069B400L, 0x00086B80L, 0x01A42580L,    /* ECFIR/ECIIR */
 0x00200080L, 0x0031E000L, 0x00764500L, 0x1FD7BF80L,
 0x004E5680L, 0x1FE39E00L, 0x054FFD00L, 0x0260D600L},
{0x1FE0CF80L, 0x00323480L, 0x1FECF380L, 0x0FDC7680L,    /* ZSYNTH */
 0x18237400L, 0xED}, 
 0x0BD9FB80L,   /* TXACGAIN */
 0x01AEF800L,   /* RXACGAIN */
 0x0721BA00L, 0x18DE4600L, 0x06437400L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 },    /* 320+1150||230 */
/* Source: Database file: C:\Program Files\Silicon Laboratories\ProSLIC API General Release Version 5.2.1\proslic_api\config_tools\cwdb.db */
/* Database information: version: 1.0.0 build date: 2010-01-06*/
/* parameters: zref=350_1000_210 rprot=30 rfuse=0 emi_cap=10*/
{
{0x06F46C80L, 0x1FDBB900L, 0x00031280L, 0x1FFD2C80L,    /* TXACEQ */
 0x0AA3BE80L, 0x1A96E300L, 0x1FD90800L, 0x1FE59F00L},   /* RXACEQ */
{0x0019AA80L, 0x1FD99F80L, 0x015EBF80L, 0x01578100L,    /* ECFIR/ECIIR */
 0x01C7AA00L, 0x01386780L, 0x00908C80L, 0x00AB1180L,
 0x00656D80L, 0x1F9ACD80L, 0x0DD7CA80L, 0x1A1BC180L},
{0x1FFD3180L, 0x1FCAE180L, 0x0037CF00L, 0x0ED0D100L,    /* ZSYNTH */
 0x192E7C80L, 0x87}, 
 0x08000000L,   /* TXACGAIN */
 0x0104BF80L,   /* RXACGAIN */
 0x07BC2380L, 0x1843DD00L, 0x07784700L,    /* RXACHPF */
 0, 0  /* TXGAIN, RXGAIN */
 }  /* 350+1000||210 */
};
#else

Si3217x_Impedance_Cfg Si3217x_Impedance_Presets[] ={
{  /* Si3217x_600_0_0_30_0.txt - ZSYN_600_0_0 */
  {0x07F46C00L,0x000E4600L,0x00008580L,0x1FFD6100L, /* TXACEQ */
   0x07EF5000L,0x0013F580L,0x1FFDE000L,0x1FFCB280L}, /* RXACEQ */
  {0x0027CB00L,0x1F8A8880L,0x02801180L,0x1F625C80L,  /* ECFIR/ECIIR */
   0x0314FB00L,0x1E6B8E80L,0x00C5FF00L,0x1FC96F00L,
   0x1FFD1200L,0x00023C00L,0x0ED29D00L,0x192A9400L},  
  {0x00810E00L,0x1EFEBE80L,0x00803500L,0x0FF66D00L, /* ZSYNTH */
   0x18099080L,0x59},  
  0x088E0D80L, /* TXACGAIN */
  0x01456D80L, /* RXACGAIN */
   0x07ABE580L,0x18541B00L,0x0757CB00L,  /* RXHPF */
  0, 0 /* 0dB RX, 0dB TX */
},
{  /* Si3217x_270_750_150_30_0.txt - ZSYN_270_750_150*/
  {0x071F7A80L,0x1FD01280L,0x00132700L,0x1FFEF980L, /* TXACEQ */
   0x0A8AA300L,0x1B9A5500L,0x008E7F00L,0x1FD7F300L}, /* RXACEQ */
  {0x0068CA00L,0x1EAE1E00L,0x0394FA00L,0x1E94AE80L,  /* ECFIR/ECIIR */
   0x0356D800L,0x0166CA80L,0x1EC16380L,0x01DE2780L,
   0x1F852300L,0x0046BE80L,0x02F17C80L,0x1EBCD280L},  
  {0x028A0C00L,0x19EE4580L,0x03876100L,0x0A762700L, /* ZSYNTH */
   0x1D87A380L,0x93},  
  0x08000000L, /* TXACGAIN */
  0x0109C280L, /* RXACGAIN */
   0x07BC6F00L,0x18439180L,0x0778DE00L,  /* RXHPF */
  0, 0 /* 0dB RX, 0dB TX */
},
{  /* Si3217x_370_620_310_30_0.txt - ZSYN_370_620_310 */
  {0x07E59E80L,0x1FD33400L,0x1FFDF800L,0x1FFD8300L, /* TXACEQ */
   0x09F38000L,0x1C1C5A00L,0x1F94D700L,0x1FDE5800L}, /* RXACEQ */
  {0x00234480L,0x1F9CDD00L,0x01F5D580L,0x1FF39000L,  /* ECFIR/ECIIR */
   0x02C17180L,0x1FBE2500L,0x00DFFE80L,0x00441A80L,
   0x003BF800L,0x1FC42400L,0x0D9EB380L,0x1A514580L},  
  {0x003ED200L,0x1F5D6B80L,0x0063B100L,0x0F12E200L, /* ZSYNTH */
   0x18EC9380L,0x8B},  
  0x08000000L, /* TXACGAIN */
  0x0127C700L, /* RXACGAIN */
   0x07B51200L,0x184AEE80L,0x076A2480L,  /* RXHPF */
  0, 0 /* 0dB RX, 0dB TX */
},
{  /* Si3217x_220_820_120_30_0.txt - ZSYN_220_820_120 */
  {0x06E38480L,0x1FD33B00L,0x00069780L,0x1FFCAB80L, /* TXACEQ */
   0x0A78F680L,0x1BC5C880L,0x009AEA00L,0x1FD66D80L}, /* RXACEQ */
  {0x00378B00L,0x1F3FCA00L,0x02B5ED00L,0x1F2B6200L,  /* ECFIR/ECIIR */
   0x04189080L,0x1F8A4480L,0x01113680L,0x00373100L,
   0x001DAE80L,0x1FE02F00L,0x0C89C780L,0x1B689680L},  
  {0x02391100L,0x1A886080L,0x033E3B00L,0x0A136200L, /* ZSYNTH */
   0x1DEA4180L,0x8C},  
  0x08000000L, /* TXACGAIN */
  0x01019200L, /* RXACGAIN */
   0x07BD1680L,0x1842EA00L,0x077A2D00L,  /* RXHPF */
  0, 0 /* 0dB RX, 0dB TX */
},
{  /* Si3217x_600_0_1000_30_0.txt - ZSYN_600_0_1000 */
  {0x07F83980L,0x00056200L,0x1FFEE880L,0x1FFB1900L, /* TXACEQ */
   0x08405000L,0x001DD200L,0x0003DB80L,0x00008700L}, /* RXACEQ */
  {0x1FEE8700L,0x009B2B00L,0x000C9680L,0x03700100L,  /* ECFIR/ECIIR */
   0x1F62E400L,0x01C77400L,0x1FBBCF80L,0x0089D500L,
   0x005CFF80L,0x1FA96E80L,0x0F679480L,0x18962A80L},  
  {0x00657C00L,0x1F2FA580L,0x006ADE00L,0x0FE12100L, /* ZSYNTH */
   0x181ED080L,0x57},  
  0x08618D80L, /* TXACGAIN */
  0x013E3400L, /* RXACGAIN */
   0x0717CE80L,0x18E83200L,0x062F9C80L,  /* RXHPF */
  0, 0 /* 0dB RX, 0dB TX */
},
{  /* Si3217x_200_680_100_30_0.txt - ZSYN_200_680_100 */
  {0x073A7B00L,0x1FCEB400L,0x0002C680L,0x1FFD0780L, /* TXACEQ */
   0x09BA8580L,0x1D2DF780L,0x006F5000L,0x1FDFE200L}, /* RXACEQ */
  {0x0004B700L,0x000F9800L,0x01201200L,0x00E1D880L,  /* ECFIR/ECIIR */
   0x03314A00L,0x1E84A580L,0x029D2380L,0x1E6F3400L,
   0x00E99200L,0x1F121100L,0x0588BC00L,0x025CAE00L},  
  {0x01415C00L,0x1C98C180L,0x0225A500L,0x0A138200L, /* ZSYNTH */
   0x1DEA2280L,0x8E},  
  0x08000000L, /* TXACGAIN */
  0x010DFD80L, /* RXACGAIN */
   0x07BA2180L,0x1845DF00L,0x07744380L,  /* RXHPF */
  0, 0 /* 0dB RX, 0dB TX */
},
{  /* Si3217x_220_820_115_30_0.txt - ZSYN_200_820_115 */
  {0x06D56380L,0x1FDF1900L,0x00095A00L,0x1FFDAA80L, /* TXACEQ */
   0x0A596300L,0x1C067880L,0x0095EF00L,0x1FD7AF00L}, /* RXACEQ */
  {0x00687000L,0x1EAE1800L,0x03983D80L,0x1EB14B00L,  /* ECFIR/ECIIR */
   0x037B3E80L,0x016FC900L,0x1ED60100L,0x01B17D80L,
   0x1FA20D00L,0x001CE900L,0x027D3380L,0x1DBDBA80L},  
  {0x00246300L,0x1E5E0580L,0x017D2300L,0x0A138100L, /* ZSYNTH */
   0x1DEA2280L,0xA7},  
  0x08000000L, /* TXACGAIN */
  0x01009500L, /* RXACGAIN */
   0x07BBEE80L,0x18441200L,0x0777DD80L,  /* RXHPF */
  0, 0 /* 0dB RX, 0dB TX */
}
};

#endif

Si3217x_FSK_Cfg Si3217x_FSK_Presets[] ={

    /* inputs: mark freq=1200.000, space freq2200.000, amp=0.220, baud=1200.000, startStopDis=0, interrupt depth = 0 */
    { 0x2232000L, 0x77C2000L, 0x3C0000L, 0x200000L, 0x6B60000L, 0x79C0000L,0, 0 }
};

Si3217x_Tone_Cfg Si3217x_Tone_Presets[] ={

    /* inputs:  freq1 = 350.000, amp1 = -18.000, freq2 = 440.000, amp2 = -18.000, ta1 = 0.000, ti1 = 0.000, ta2 = 0.000, ti2 = 0.000*/
    { {0x7B30000L, 0x3A000L, 0x0L, 0x0, 0x0, 0x0, 0x0}, {0x7870000L, 0x4A000L, 0x0L, 0x0, 0x0, 0x0, 0x0}, 0x66 },
    /* inputs:  freq1 = 480.000, amp1 = -18.000, freq2 = 620.000, amp2 = -18.000, ta1 = 0.500, ti1 = 0.500, ta2 = 0.500, ti2 = 0.500*/
    { {0x7700000L, 0x52000L, 0x0L, 0xA0, 0xF, 0xA0, 0xF}, {0x7120000L, 0x6A000L, 0x0L, 0xA0, 0xF, 0xA0, 0xF}, 0x66 },
    /* inputs:  freq1 = 480.000, amp1 = -18.000, freq2 = 440.000, amp2 = -18.000, ta1 = 2.000, ti1 = 4.000, ta2 = 2.000, ti2 = 4.000*/
    { {0x7700000L, 0x52000L, 0x0L, 0x80, 0x3E, 0x0, 0x7D}, {0x7870000L, 0x4A000L, 0x0L, 0x80, 0x3E, 0x0, 0x7D}, 0x66 },
    /* inputs:  freq1 = 480.000, amp1 = -18.000, freq2 = 620.000, amp2 = -18.000, ta1 = 0.300, ti1 = 0.200, ta2 = 0.300, ti2 = 0.200*/
    { {0x7700000L, 0x52000L, 0x0L, 0x60, 0x9, 0x40, 0x6}, {0x7120000L, 0x6A000L, 0x0L, 0x60, 0x9, 0x40, 0x6}, 0x66 },
    /* inputs:  freq1 = 480.000, amp1 = -18.000, freq2 = 620.000, amp2 = -18.000, ta1 = 0.200, ti1 = 0.200, ta2 = 0.200, ti2 = 0.200*/
    { {0x7700000L, 0x52000L, 0x0L, 0x40, 0x6, 0x40, 0x6}, {0x7120000L, 0x6A000L, 0x0L, 0x40, 0x6, 0x40, 0x6}, 0x66 }
};
#if 1 // thlin add
//#define ALAW			0
#define ALAW			4	//invert alaw even bit
#define ULAW			1
#define LINEAR_8BIT		2
#define LINEAR_16BIT		3
#endif

Si3217x_PCM_Cfg Si3217x_PCM_Presets[] ={

    /* inputs:  u-law narrowband positive edge, dtx positive edge, both disabled, tx hwy = A, rx hwy = A */
#if 1 // thlin add
    // narrowband
    { LINEAR_16BIT, 0x0, 0x1, 0x0 },
    { ALAW, 0x0, 0x1, 0x0 },
    { ULAW, 0x0, 0x1, 0x0 },
    // wideband
    { LINEAR_16BIT, 0x1, 0x1, 0x0 },
    { ALAW, 0x1, 0x1, 0x0 },
    { ULAW, 0x1, 0x1, 0x0 }
#else
    { 0x1, 0x0, 0x0, 0x0 }
#endif
};

