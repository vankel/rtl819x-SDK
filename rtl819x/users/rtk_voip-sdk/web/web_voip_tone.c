#include <stdio.h>
#include "web_voip.h"

char tone_country[TONE_MAX][20] = {"USA", "UK", "AUSTRALIA", "HONG KONG", "JAPAN", 
				   "SWEDEN", "GERMANY", "FRANCE", "TAIWAN", "BELGIUM", 
#ifdef COUNTRY_TONE_RESERVED
				   "FINLAND", "ITALY", "CHINA", "EXT1", "EXT2", "EXT3", "EXT4", "RESERVE", "CUSTOMER"};
#else
				   "FINLAND", "ITALY", "CHINA", "EXT1", "EXT2", "EXT3", "EXT4", "CUSTOMER"};
#endif


char cust_tone[TONE_CUSTOMER_MAX][20] = {"Custom1", "Custom2", "Custom3", "Custom4",
					 "Custom5", "Custom6", "Custom7", "Custom8"};

char tone_type[TONE_TYPE_MAX][20] = {"ADDITIVE", "MODULATED", "SUCC", "SUCC_ADD", "FOUR_FREQ",
                                     "STEP_INC", "TWO_STEP"};
#if 0 // jwsyu 20121005 disable
#if 1
char tone_cycle[TONE_CYCLE_MAX][20] = {"CONTINUOUS", "BURST", "CADENCE"};
#else
char tone_cycle[TONE_CYCLE_MAX][20] = {"CONTINUOUS", "BURST"};
#endif
#endif

char number_of_distone[DIS_CONNECT_TONE_MAX][5] = {"0", "1", "2"};

void asp_voip_ToneSet(webs_t wp, char_t *path, char_t *query)
{
	char *ptr;
	int i, cust_flag = 0, cust_idx, idx;
	voipCfgParam_t *pCfg;

	if (web_flash_get(&pCfg) != 0)
		return;


	ptr = websGetVar(wp, T("Country"), T(""));	
	if (strcmp(ptr, "Apply") == 0)
	{
		/* select country */
		idx = atoi(websGetVar(wp, T("tone_country"), T("")));
		if (idx < 0 || idx >= TONE_MAX)
			idx = 0;
			
		pCfg->tone_of_country = idx;
		if (idx == TONE_CUSTOMER)
			cust_flag = 1;

		
		if (cust_flag)
		{
			/* select dial */
			ptr = websGetVar(wp, T("dial"), T(""));
			
			for(i=0; i < TONE_CUSTOMER_MAX; i++)
			{
				if (!gstrcmp(ptr, cust_tone[i]))
					break;
			}
			if (i == TONE_CUSTOMER_MAX)
				i = TONE_CUSTOMER_1;

			pCfg->tone_of_custdial = i;
		
			/* select ring */
			ptr = websGetVar(wp, T("ring"), T(""));
			
			for(i=0; i < TONE_CUSTOMER_MAX; i++)
			{
				if (!gstrcmp(ptr, cust_tone[i]))
					break;
			}
			if (i == TONE_CUSTOMER_MAX)
				i = TONE_CUSTOMER_2;

			pCfg->tone_of_custring = i;
			
			/* select busy */
			ptr = websGetVar(wp, T("busy"), T(""));
			
			for(i=0; i < TONE_CUSTOMER_MAX; i++)
			{
				if (!gstrcmp(ptr, cust_tone[i]))
					break;
			}
			if (i == TONE_CUSTOMER_MAX)
				i = TONE_CUSTOMER_3;

			pCfg->tone_of_custbusy = i;
			
			/* select waiting */
			ptr = websGetVar(wp, T("waiting"), T(""));
			
			for(i=0; i < TONE_CUSTOMER_MAX; i++)
			{
				if (!gstrcmp(ptr, cust_tone[i]))
					break;
			}
			if (i == TONE_CUSTOMER_MAX)
				i = TONE_CUSTOMER_4;
				
			pCfg->tone_of_custwaiting = i;
		}
	}
	
	/* Select Custom Tone */
	ptr = websGetVar(wp, T("selfItem"), T(""));	
	for(i=0; i < TONE_CUSTOMER_MAX; i++)
	{
		if (!gstrcmp(ptr, cust_tone[i]))
			break;
	}
	if (i == TONE_CUSTOMER_MAX)
		i = TONE_CUSTOMER_1;
	
	pCfg->tone_of_customize = i;	
	
	/* Tone Parameters */
	ptr = websGetVar(wp, T("Tone"), T(""));	
	if (strcmp(ptr, "Apply") == 0)
	{
		// Custom Tone Parameters Set
		cust_idx = pCfg->tone_of_customize;
		
		ptr = websGetVar(wp, T("type"), T(""));
		
		for(i=0; i < TONE_TYPE_MAX; i++)
		{
			if (!gstrcmp(ptr, tone_type[i]))
				break;
		}
		if (i == TONE_TYPE_MAX)
			i = TONE_TYPE_ADDITIVE;
		
		pCfg->cust_tone_para[cust_idx].toneType = i;

		///////////////////////////////////////////
#if 0 // jwsyu 20121005 disable
		ptr = websGetVar(wp, T("cycle"), T(""));
		
		for(i=0; i < TONE_CYCLE_MAX; i++)
		{
			if (!gstrcmp(ptr, tone_cycle[i]))
				break;
		}
		if (i == TONE_CYCLE_MAX)
			i = TONE_CYCLE_CONTINUOUS;
			
		pCfg->cust_tone_para[cust_idx].cycle = i;
#endif
		pCfg->cust_tone_para[cust_idx].cycle = atoi(websGetVar(wp, T("cycle"), T("")));
		pCfg->cust_tone_para[cust_idx].cadNUM = atoi(websGetVar(wp, T("cadNUM"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn0 = atoi(websGetVar(wp, T("CadOn0"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn1 = atoi(websGetVar(wp, T("CadOn1"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn2 = atoi(websGetVar(wp, T("CadOn2"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn3 = atoi(websGetVar(wp, T("CadOn3"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff0 = atoi(websGetVar(wp, T("CadOff0"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff1 = atoi(websGetVar(wp, T("CadOff1"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff2 = atoi(websGetVar(wp, T("CadOff2"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff3 = atoi(websGetVar(wp, T("CadOff3"), T("")));
		pCfg->cust_tone_para[cust_idx].PatternOff = atoi(websGetVar(wp, T("PatternOff"), T("")));
		pCfg->cust_tone_para[cust_idx].ToneNUM = atoi(websGetVar(wp, T("ToneNUM"), T("")));
		pCfg->cust_tone_para[cust_idx].Freq0 = atoi(websGetVar(wp, T("Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].Freq1 = atoi(websGetVar(wp, T("Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].Freq2 = atoi(websGetVar(wp, T("Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].Freq3 = atoi(websGetVar(wp, T("Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].Gain0 = atoi(websGetVar(wp, T("Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].Gain1 = atoi(websGetVar(wp, T("Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].Gain2 = atoi(websGetVar(wp, T("Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].Gain3 = atoi(websGetVar(wp, T("Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C1_Freq0 = atoi(websGetVar(wp, T("C1_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C1_Freq1 = atoi(websGetVar(wp, T("C1_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C1_Freq2 = atoi(websGetVar(wp, T("C1_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C1_Freq3 = atoi(websGetVar(wp, T("C1_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C1_Gain0 = atoi(websGetVar(wp, T("C1_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C1_Gain1 = atoi(websGetVar(wp, T("C1_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C1_Gain2 = atoi(websGetVar(wp, T("C1_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C1_Gain3 = atoi(websGetVar(wp, T("C1_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C2_Freq0 = atoi(websGetVar(wp, T("C2_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C2_Freq1 = atoi(websGetVar(wp, T("C2_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C2_Freq2 = atoi(websGetVar(wp, T("C2_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C2_Freq3 = atoi(websGetVar(wp, T("C2_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C2_Gain0 = atoi(websGetVar(wp, T("C2_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C2_Gain1 = atoi(websGetVar(wp, T("C2_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C2_Gain2 = atoi(websGetVar(wp, T("C2_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C2_Gain3 = atoi(websGetVar(wp, T("C2_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C3_Freq0 = atoi(websGetVar(wp, T("C3_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C3_Freq1 = atoi(websGetVar(wp, T("C3_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C3_Freq2 = atoi(websGetVar(wp, T("C3_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C3_Freq3 = atoi(websGetVar(wp, T("C3_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C3_Gain0 = atoi(websGetVar(wp, T("C3_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C3_Gain1 = atoi(websGetVar(wp, T("C3_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C3_Gain2 = atoi(websGetVar(wp, T("C3_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C3_Gain3 = atoi(websGetVar(wp, T("C3_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].CadOn4 = atoi(websGetVar(wp, T("CadOn4"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff4 = atoi(websGetVar(wp, T("CadOff4"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn5 = atoi(websGetVar(wp, T("CadOn5"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff5 = atoi(websGetVar(wp, T("CadOff5"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn6 = atoi(websGetVar(wp, T("CadOn6"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff6 = atoi(websGetVar(wp, T("CadOff6"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn7 = atoi(websGetVar(wp, T("CadOn7"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff7 = atoi(websGetVar(wp, T("CadOff7"), T("")));
		pCfg->cust_tone_para[cust_idx].C4_Freq0 = atoi(websGetVar(wp, T("C4_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C4_Freq1 = atoi(websGetVar(wp, T("C4_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C4_Freq2 = atoi(websGetVar(wp, T("C4_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C4_Freq3 = atoi(websGetVar(wp, T("C4_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C4_Gain0 = atoi(websGetVar(wp, T("C4_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C4_Gain1 = atoi(websGetVar(wp, T("C4_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C4_Gain2 = atoi(websGetVar(wp, T("C4_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C4_Gain3 = atoi(websGetVar(wp, T("C4_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C5_Freq0 = atoi(websGetVar(wp, T("C5_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C5_Freq1 = atoi(websGetVar(wp, T("C5_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C5_Freq2 = atoi(websGetVar(wp, T("C5_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C5_Freq3 = atoi(websGetVar(wp, T("C5_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C5_Gain0 = atoi(websGetVar(wp, T("C5_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C5_Gain1 = atoi(websGetVar(wp, T("C5_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C5_Gain2 = atoi(websGetVar(wp, T("C5_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C5_Gain3 = atoi(websGetVar(wp, T("C5_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C6_Freq0 = atoi(websGetVar(wp, T("C6_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C6_Freq1 = atoi(websGetVar(wp, T("C6_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C6_Freq2 = atoi(websGetVar(wp, T("C6_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C6_Freq3 = atoi(websGetVar(wp, T("C6_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C6_Gain0 = atoi(websGetVar(wp, T("C6_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C6_Gain1 = atoi(websGetVar(wp, T("C6_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C6_Gain2 = atoi(websGetVar(wp, T("C6_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C6_Gain3 = atoi(websGetVar(wp, T("C6_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C7_Freq0 = atoi(websGetVar(wp, T("C7_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C7_Freq1 = atoi(websGetVar(wp, T("C7_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C7_Freq2 = atoi(websGetVar(wp, T("C7_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C7_Freq3 = atoi(websGetVar(wp, T("C7_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C7_Gain0 = atoi(websGetVar(wp, T("C7_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C7_Gain1 = atoi(websGetVar(wp, T("C7_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C7_Gain2 = atoi(websGetVar(wp, T("C7_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C7_Gain3 = atoi(websGetVar(wp, T("C7_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].CadOn8 = atoi(websGetVar(wp, T("CadOn8"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff8 = atoi(websGetVar(wp, T("CadOff8"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn9 = atoi(websGetVar(wp, T("CadOn9"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff9 = atoi(websGetVar(wp, T("CadOff9"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn10 = atoi(websGetVar(wp, T("CadOn10"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff10 = atoi(websGetVar(wp, T("CadOff10"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn11 = atoi(websGetVar(wp, T("CadOn11"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff11 = atoi(websGetVar(wp, T("CadOff11"), T("")));
		pCfg->cust_tone_para[cust_idx].C8_Freq0 = atoi(websGetVar(wp, T("C8_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C8_Freq1 = atoi(websGetVar(wp, T("C8_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C8_Freq2 = atoi(websGetVar(wp, T("C8_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C8_Freq3 = atoi(websGetVar(wp, T("C8_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C8_Gain0 = atoi(websGetVar(wp, T("C8_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C8_Gain1 = atoi(websGetVar(wp, T("C8_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C8_Gain2 = atoi(websGetVar(wp, T("C8_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C8_Gain3 = atoi(websGetVar(wp, T("C8_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C9_Freq0 = atoi(websGetVar(wp, T("C9_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C9_Freq1 = atoi(websGetVar(wp, T("C9_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C9_Freq2 = atoi(websGetVar(wp, T("C9_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C9_Freq3 = atoi(websGetVar(wp, T("C9_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C9_Gain0 = atoi(websGetVar(wp, T("C9_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C9_Gain1 = atoi(websGetVar(wp, T("C9_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C9_Gain2 = atoi(websGetVar(wp, T("C9_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C9_Gain3 = atoi(websGetVar(wp, T("C9_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C10_Freq0 = atoi(websGetVar(wp, T("C10_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C10_Freq1 = atoi(websGetVar(wp, T("C10_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C10_Freq2 = atoi(websGetVar(wp, T("C10_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C10_Freq3 = atoi(websGetVar(wp, T("C10_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C10_Gain0 = atoi(websGetVar(wp, T("C10_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C10_Gain1 = atoi(websGetVar(wp, T("C10_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C10_Gain2 = atoi(websGetVar(wp, T("C10_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C10_Gain3 = atoi(websGetVar(wp, T("C10_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C11_Freq0 = atoi(websGetVar(wp, T("C11_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C11_Freq1 = atoi(websGetVar(wp, T("C11_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C11_Freq2 = atoi(websGetVar(wp, T("C11_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C11_Freq3 = atoi(websGetVar(wp, T("C11_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C11_Gain0 = atoi(websGetVar(wp, T("C11_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C11_Gain1 = atoi(websGetVar(wp, T("C11_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C11_Gain2 = atoi(websGetVar(wp, T("C11_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C11_Gain3 = atoi(websGetVar(wp, T("C11_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].CadOn12 = atoi(websGetVar(wp, T("CadOn12"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff12 = atoi(websGetVar(wp, T("CadOff12"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn13 = atoi(websGetVar(wp, T("CadOn13"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff13 = atoi(websGetVar(wp, T("CadOff13"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn14 = atoi(websGetVar(wp, T("CadOn14"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff14 = atoi(websGetVar(wp, T("CadOff14"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn15 = atoi(websGetVar(wp, T("CadOn15"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff15 = atoi(websGetVar(wp, T("CadOff15"), T("")));
		pCfg->cust_tone_para[cust_idx].C12_Freq0 = atoi(websGetVar(wp, T("C12_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C12_Freq1 = atoi(websGetVar(wp, T("C12_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C12_Freq2 = atoi(websGetVar(wp, T("C12_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C12_Freq3 = atoi(websGetVar(wp, T("C12_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C12_Gain0 = atoi(websGetVar(wp, T("C12_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C12_Gain1 = atoi(websGetVar(wp, T("C12_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C12_Gain2 = atoi(websGetVar(wp, T("C12_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C12_Gain3 = atoi(websGetVar(wp, T("C12_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C13_Freq0 = atoi(websGetVar(wp, T("C13_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C13_Freq1 = atoi(websGetVar(wp, T("C13_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C13_Freq2 = atoi(websGetVar(wp, T("C13_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C13_Freq3 = atoi(websGetVar(wp, T("C13_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C13_Gain0 = atoi(websGetVar(wp, T("C13_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C13_Gain1 = atoi(websGetVar(wp, T("C13_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C13_Gain2 = atoi(websGetVar(wp, T("C13_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C13_Gain3 = atoi(websGetVar(wp, T("C13_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C14_Freq0 = atoi(websGetVar(wp, T("C14_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C14_Freq1 = atoi(websGetVar(wp, T("C14_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C14_Freq2 = atoi(websGetVar(wp, T("C14_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C14_Freq3 = atoi(websGetVar(wp, T("C14_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C14_Gain0 = atoi(websGetVar(wp, T("C14_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C14_Gain1 = atoi(websGetVar(wp, T("C14_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C14_Gain2 = atoi(websGetVar(wp, T("C14_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C14_Gain3 = atoi(websGetVar(wp, T("C14_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C15_Freq0 = atoi(websGetVar(wp, T("C15_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C15_Freq1 = atoi(websGetVar(wp, T("C15_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C15_Freq2 = atoi(websGetVar(wp, T("C15_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C15_Freq3 = atoi(websGetVar(wp, T("C15_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C15_Gain0 = atoi(websGetVar(wp, T("C15_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C15_Gain1 = atoi(websGetVar(wp, T("C15_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C15_Gain2 = atoi(websGetVar(wp, T("C15_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C15_Gain3 = atoi(websGetVar(wp, T("C15_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].CadOn16 = atoi(websGetVar(wp, T("CadOn16"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff16 = atoi(websGetVar(wp, T("CadOff16"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn17 = atoi(websGetVar(wp, T("CadOn17"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff17 = atoi(websGetVar(wp, T("CadOff17"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn18 = atoi(websGetVar(wp, T("CadOn18"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff18 = atoi(websGetVar(wp, T("CadOff18"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn19 = atoi(websGetVar(wp, T("CadOn19"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff19 = atoi(websGetVar(wp, T("CadOff19"), T("")));
		pCfg->cust_tone_para[cust_idx].C16_Freq0 = atoi(websGetVar(wp, T("C16_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C16_Freq1 = atoi(websGetVar(wp, T("C16_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C16_Freq2 = atoi(websGetVar(wp, T("C16_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C16_Freq3 = atoi(websGetVar(wp, T("C16_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C16_Gain0 = atoi(websGetVar(wp, T("C16_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C16_Gain1 = atoi(websGetVar(wp, T("C16_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C16_Gain2 = atoi(websGetVar(wp, T("C16_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C16_Gain3 = atoi(websGetVar(wp, T("C16_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C17_Freq0 = atoi(websGetVar(wp, T("C17_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C17_Freq1 = atoi(websGetVar(wp, T("C17_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C17_Freq2 = atoi(websGetVar(wp, T("C17_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C17_Freq3 = atoi(websGetVar(wp, T("C17_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C17_Gain0 = atoi(websGetVar(wp, T("C17_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C17_Gain1 = atoi(websGetVar(wp, T("C17_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C17_Gain2 = atoi(websGetVar(wp, T("C17_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C17_Gain3 = atoi(websGetVar(wp, T("C17_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C18_Freq0 = atoi(websGetVar(wp, T("C18_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C18_Freq1 = atoi(websGetVar(wp, T("C18_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C18_Freq2 = atoi(websGetVar(wp, T("C18_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C18_Freq3 = atoi(websGetVar(wp, T("C18_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C18_Gain0 = atoi(websGetVar(wp, T("C18_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C18_Gain1 = atoi(websGetVar(wp, T("C18_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C18_Gain2 = atoi(websGetVar(wp, T("C18_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C18_Gain3 = atoi(websGetVar(wp, T("C18_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C19_Freq0 = atoi(websGetVar(wp, T("C19_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C19_Freq1 = atoi(websGetVar(wp, T("C19_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C19_Freq2 = atoi(websGetVar(wp, T("C19_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C19_Freq3 = atoi(websGetVar(wp, T("C19_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C19_Gain0 = atoi(websGetVar(wp, T("C19_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C19_Gain1 = atoi(websGetVar(wp, T("C19_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C19_Gain2 = atoi(websGetVar(wp, T("C19_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C19_Gain3 = atoi(websGetVar(wp, T("C19_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].CadOn20 = atoi(websGetVar(wp, T("CadOn20"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff20 = atoi(websGetVar(wp, T("CadOff20"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn21 = atoi(websGetVar(wp, T("CadOn21"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff21 = atoi(websGetVar(wp, T("CadOff21"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn22 = atoi(websGetVar(wp, T("CadOn22"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff22 = atoi(websGetVar(wp, T("CadOff22"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn23 = atoi(websGetVar(wp, T("CadOn23"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff23 = atoi(websGetVar(wp, T("CadOff23"), T("")));
		pCfg->cust_tone_para[cust_idx].C20_Freq0 = atoi(websGetVar(wp, T("C20_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C20_Freq1 = atoi(websGetVar(wp, T("C20_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C20_Freq2 = atoi(websGetVar(wp, T("C20_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C20_Freq3 = atoi(websGetVar(wp, T("C20_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C20_Gain0 = atoi(websGetVar(wp, T("C20_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C20_Gain1 = atoi(websGetVar(wp, T("C20_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C20_Gain2 = atoi(websGetVar(wp, T("C20_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C20_Gain3 = atoi(websGetVar(wp, T("C20_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C21_Freq0 = atoi(websGetVar(wp, T("C21_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C21_Freq1 = atoi(websGetVar(wp, T("C21_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C21_Freq2 = atoi(websGetVar(wp, T("C21_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C21_Freq3 = atoi(websGetVar(wp, T("C21_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C21_Gain0 = atoi(websGetVar(wp, T("C21_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C21_Gain1 = atoi(websGetVar(wp, T("C21_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C21_Gain2 = atoi(websGetVar(wp, T("C21_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C21_Gain3 = atoi(websGetVar(wp, T("C21_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C22_Freq0 = atoi(websGetVar(wp, T("C22_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C22_Freq1 = atoi(websGetVar(wp, T("C22_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C22_Freq2 = atoi(websGetVar(wp, T("C22_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C22_Freq3 = atoi(websGetVar(wp, T("C22_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C22_Gain0 = atoi(websGetVar(wp, T("C22_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C22_Gain1 = atoi(websGetVar(wp, T("C22_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C22_Gain2 = atoi(websGetVar(wp, T("C22_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C22_Gain3 = atoi(websGetVar(wp, T("C22_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C23_Freq0 = atoi(websGetVar(wp, T("C23_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C23_Freq1 = atoi(websGetVar(wp, T("C23_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C23_Freq2 = atoi(websGetVar(wp, T("C23_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C23_Freq3 = atoi(websGetVar(wp, T("C23_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C23_Gain0 = atoi(websGetVar(wp, T("C23_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C23_Gain1 = atoi(websGetVar(wp, T("C23_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C23_Gain2 = atoi(websGetVar(wp, T("C23_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C23_Gain3 = atoi(websGetVar(wp, T("C23_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].CadOn24 = atoi(websGetVar(wp, T("CadOn24"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff24 = atoi(websGetVar(wp, T("CadOff24"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn25 = atoi(websGetVar(wp, T("CadOn25"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff25 = atoi(websGetVar(wp, T("CadOff25"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn26 = atoi(websGetVar(wp, T("CadOn26"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff26 = atoi(websGetVar(wp, T("CadOff26"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn27 = atoi(websGetVar(wp, T("CadOn27"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff27 = atoi(websGetVar(wp, T("CadOff27"), T("")));
		pCfg->cust_tone_para[cust_idx].C24_Freq0 = atoi(websGetVar(wp, T("C24_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C24_Freq1 = atoi(websGetVar(wp, T("C24_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C24_Freq2 = atoi(websGetVar(wp, T("C24_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C24_Freq3 = atoi(websGetVar(wp, T("C24_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C24_Gain0 = atoi(websGetVar(wp, T("C24_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C24_Gain1 = atoi(websGetVar(wp, T("C24_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C24_Gain2 = atoi(websGetVar(wp, T("C24_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C24_Gain3 = atoi(websGetVar(wp, T("C24_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C25_Freq0 = atoi(websGetVar(wp, T("C25_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C25_Freq1 = atoi(websGetVar(wp, T("C25_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C25_Freq2 = atoi(websGetVar(wp, T("C25_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C25_Freq3 = atoi(websGetVar(wp, T("C25_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C25_Gain0 = atoi(websGetVar(wp, T("C25_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C25_Gain1 = atoi(websGetVar(wp, T("C25_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C25_Gain2 = atoi(websGetVar(wp, T("C25_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C25_Gain3 = atoi(websGetVar(wp, T("C25_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C26_Freq0 = atoi(websGetVar(wp, T("C26_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C26_Freq1 = atoi(websGetVar(wp, T("C26_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C26_Freq2 = atoi(websGetVar(wp, T("C26_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C26_Freq3 = atoi(websGetVar(wp, T("C26_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C26_Gain0 = atoi(websGetVar(wp, T("C26_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C26_Gain1 = atoi(websGetVar(wp, T("C26_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C26_Gain2 = atoi(websGetVar(wp, T("C26_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C26_Gain3 = atoi(websGetVar(wp, T("C26_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C27_Freq0 = atoi(websGetVar(wp, T("C27_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C27_Freq1 = atoi(websGetVar(wp, T("C27_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C27_Freq2 = atoi(websGetVar(wp, T("C27_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C27_Freq3 = atoi(websGetVar(wp, T("C27_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C27_Gain0 = atoi(websGetVar(wp, T("C27_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C27_Gain1 = atoi(websGetVar(wp, T("C27_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C27_Gain2 = atoi(websGetVar(wp, T("C27_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C27_Gain3 = atoi(websGetVar(wp, T("C27_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].CadOn28 = atoi(websGetVar(wp, T("CadOn28"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff28 = atoi(websGetVar(wp, T("CadOff28"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn29 = atoi(websGetVar(wp, T("CadOn29"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff29 = atoi(websGetVar(wp, T("CadOff29"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn30 = atoi(websGetVar(wp, T("CadOn30"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff30 = atoi(websGetVar(wp, T("CadOff30"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOn31 = atoi(websGetVar(wp, T("CadOn31"), T("")));
		pCfg->cust_tone_para[cust_idx].CadOff31 = atoi(websGetVar(wp, T("CadOff31"), T("")));
		pCfg->cust_tone_para[cust_idx].C28_Freq0 = atoi(websGetVar(wp, T("C28_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C28_Freq1 = atoi(websGetVar(wp, T("C28_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C28_Freq2 = atoi(websGetVar(wp, T("C28_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C28_Freq3 = atoi(websGetVar(wp, T("C28_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C28_Gain0 = atoi(websGetVar(wp, T("C28_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C28_Gain1 = atoi(websGetVar(wp, T("C28_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C28_Gain2 = atoi(websGetVar(wp, T("C28_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C28_Gain3 = atoi(websGetVar(wp, T("C28_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C29_Freq0 = atoi(websGetVar(wp, T("C29_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C29_Freq1 = atoi(websGetVar(wp, T("C29_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C29_Freq2 = atoi(websGetVar(wp, T("C29_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C29_Freq3 = atoi(websGetVar(wp, T("C29_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C29_Gain0 = atoi(websGetVar(wp, T("C29_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C29_Gain1 = atoi(websGetVar(wp, T("C29_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C29_Gain2 = atoi(websGetVar(wp, T("C29_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C29_Gain3 = atoi(websGetVar(wp, T("C29_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C30_Freq0 = atoi(websGetVar(wp, T("C30_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C30_Freq1 = atoi(websGetVar(wp, T("C30_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C30_Freq2 = atoi(websGetVar(wp, T("C30_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C30_Freq3 = atoi(websGetVar(wp, T("C30_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C30_Gain0 = atoi(websGetVar(wp, T("C30_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C30_Gain1 = atoi(websGetVar(wp, T("C30_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C30_Gain2 = atoi(websGetVar(wp, T("C30_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C30_Gain3 = atoi(websGetVar(wp, T("C30_Gain3"), T("")));	
		pCfg->cust_tone_para[cust_idx].C31_Freq0 = atoi(websGetVar(wp, T("C31_Freq0"), T("")));
		pCfg->cust_tone_para[cust_idx].C31_Freq1 = atoi(websGetVar(wp, T("C31_Freq1"), T("")));
		pCfg->cust_tone_para[cust_idx].C31_Freq2 = atoi(websGetVar(wp, T("C31_Freq2"), T("")));
		pCfg->cust_tone_para[cust_idx].C31_Freq3 = atoi(websGetVar(wp, T("C31_Freq3"), T("")));
		pCfg->cust_tone_para[cust_idx].C31_Gain0 = atoi(websGetVar(wp, T("C31_Gain0"), T("")));
		pCfg->cust_tone_para[cust_idx].C31_Gain1 = atoi(websGetVar(wp, T("C31_Gain1"), T("")));
		pCfg->cust_tone_para[cust_idx].C31_Gain2 = atoi(websGetVar(wp, T("C31_Gain2"), T("")));
		pCfg->cust_tone_para[cust_idx].C31_Gain3 = atoi(websGetVar(wp, T("C31_Gain3"), T("")));	
	}
	
	/* Disconnect tone det config */
	ptr = websGetVar(wp, T("Distone"), T(""));	
	if (strcmp(ptr, "Apply") == 0)
	{
		/* select disconnect tone num */
		ptr = websGetVar(wp, T("distone_num"), T(""));	
		for(i=0; i < DIS_CONNECT_TONE_MAX; i++)
		{
			if (!gstrcmp(ptr, number_of_distone[i]))
				break;
		}
		if (i == DIS_CONNECT_TONE_MAX)
			i = 2;
		pCfg->distone_num = i;

		ptr = websGetVar(wp, T("d1freqnum"), T(""));	
		for(i=1; i < DIS_CONNECT_TONE_FREQ_MAX; i++)
		{
			if (!gstrcmp(ptr, number_of_distone[i]))
				break;
		}
		if (i == DIS_CONNECT_TONE_FREQ_MAX)
			i = 2;
		pCfg->d1freqnum = i;


		pCfg->d1Freq1 = atoi(websGetVar(wp, T("d1Freq1"), T("")));
		pCfg->d1Freq2 = atoi(websGetVar(wp, T("d1Freq2"), T("")));
		pCfg->d1Accur = atoi(websGetVar(wp, T("d1Accur"), T("")));
		pCfg->d1Level = atoi(websGetVar(wp, T("d1Level"), T("")));
		pCfg->d1ONup = atoi(websGetVar(wp, T("d1ONup"), T("")));
		pCfg->d1ONlow = atoi(websGetVar(wp, T("d1ONlow"), T("")));
		pCfg->d1OFFup = atoi(websGetVar(wp, T("d1OFFup"), T("")));
		pCfg->d1OFFlow = atoi(websGetVar(wp, T("d1OFFlow"), T("")));

		ptr = websGetVar(wp, T("d2freqnum"), T(""));	
		for(i=1; i < DIS_CONNECT_TONE_FREQ_MAX; i++)
		{
			if (!gstrcmp(ptr, number_of_distone[i]))
				break;
		}
		if (i == DIS_CONNECT_TONE_FREQ_MAX)
			i = 2;
		pCfg->d2freqnum = i;

		pCfg->d2Freq1 = atoi(websGetVar(wp, T("d2Freq1"), T("")));
		pCfg->d2Freq2 = atoi(websGetVar(wp, T("d2Freq2"), T("")));
		pCfg->d2Accur = atoi(websGetVar(wp, T("d2Accur"), T("")));
		pCfg->d2Level = atoi(websGetVar(wp, T("d2Level"), T("")));
		pCfg->d2ONup = atoi(websGetVar(wp, T("d2ONup"), T("")));
		pCfg->d2ONlow = atoi(websGetVar(wp, T("d2ONlow"), T("")));
		pCfg->d2OFFup = atoi(websGetVar(wp, T("d2OFFup"), T("")));
		pCfg->d2OFFlow = atoi(websGetVar(wp, T("d2OFFlow"), T("")));

	}



	web_flash_set(pCfg);
	
#ifdef REBOOT_CHECK
	OK_MSG("/voip_tone.asp");
#else
	web_restart_solar();

	websRedirect(wp, T("/voip_tone.asp"));
#endif

}


#ifdef CONFIG_APP_BOA
int asp_voip_ToneGet(webs_t wp, int argc, char_t **argv)
#else
int asp_voip_ToneGet(int ejid, webs_t wp, int argc, char_t **argv)
#endif
{
	int i, cust_idx;
	voipCfgParam_t *pCfg;

	if (web_flash_get(&pCfg) != 0)
		return -1;

	cust_idx = pCfg->tone_of_customize;
	
	if (strcmp(argv[0], "tone_country")==0)
	{
		for (i=0; i < TONE_MAX ;i++)
		{
			if (i == pCfg->tone_of_country)
				websWrite(wp, "<option value=%d selected>%s</option>", i, tone_country[i]);
			else
				websWrite(wp, "<option value=%d>%s</option>", i, tone_country[i]);
		}
	}
	else if (strcmp(argv[0], "dial")==0)
	{
		for (i=0; i < TONE_CUSTOMER_MAX ;i++)
		{
			if (i == pCfg->tone_of_custdial) // dial
				websWrite(wp, "<option selected>%s</option>", cust_tone[i]);
			else
				websWrite(wp, "<option>%s</option>", cust_tone[i]);
		}
	}
	else if (strcmp(argv[0], "ring")==0)
	{
		for (i=0; i < TONE_CUSTOMER_MAX ;i++)
		{
			if (i == pCfg->tone_of_custring) // ring
				websWrite(wp, "<option selected>%s</option>", cust_tone[i]);
			else
				websWrite(wp, "<option>%s</option>", cust_tone[i]);
		}
	}
	else if (strcmp(argv[0], "busy")==0)
	{
		for (i=0; i < TONE_CUSTOMER_MAX ;i++)
		{
			if (i == pCfg->tone_of_custbusy) // busy
				websWrite(wp, "<option selected>%s</option>", cust_tone[i]);
			else
				websWrite(wp, "<option>%s</option>", cust_tone[i]);
		}
	}
	else if (strcmp(argv[0], "waiting")==0)
	{
		for (i=0; i < TONE_CUSTOMER_MAX ;i++)
		{
			if (i == pCfg->tone_of_custwaiting) // waiting
				websWrite(wp, "<option selected>%s</option>", cust_tone[i]);
			else
				websWrite(wp, "<option>%s</option>", cust_tone[i]);
		}
	}
	// Get Custome Tone 
	else if (strcmp(argv[0], "selfItem")==0)
	{
		for (i=0; i < TONE_CUSTOMER_MAX ;i++)
		{
			if (i == pCfg->tone_of_customize)
				websWrite(wp, "<option selected>%s</option>", cust_tone[i]);
			else
				websWrite(wp, "<option>%s</option>", cust_tone[i]);
		}
	}
	// Get Custom Tone Parameters	
	else if (strcmp(argv[0], "type")==0)
	{
		for (i=0; i < TONE_TYPE_MAX ;i++)
		{
			if (i == pCfg->cust_tone_para[cust_idx].toneType)
				websWrite(wp, "<option selected>%s</option>", tone_type[i]);
			else
				websWrite(wp, "<option>%s</option>", tone_type[i]);
		}
	}
	else if (strcmp(argv[0], "cycle")==0)
	{
#if 0 // jwsyu 20121005 disable
		for (i=0; i < TONE_CYCLE_MAX ;i++)
		{
			if (i == pCfg->cust_tone_para[cust_idx].cycle)
				websWrite(wp, "<option selected>%s</option>", tone_cycle[i]);
			else
				websWrite(wp, "<option>%s</option>", tone_cycle[i]);
		}
#endif
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].cycle);
	}	
	else if (strcmp(argv[0], "cadNUM")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].cadNUM);
	else if (strcmp(argv[0], "CadOn0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn0);
	else if (strcmp(argv[0], "CadOn1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn1);
	else if (strcmp(argv[0], "CadOn2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn2);
	else if (strcmp(argv[0], "CadOn3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn3);
	else if (strcmp(argv[0], "CadOff0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff0);
	else if (strcmp(argv[0], "CadOff1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff1);
	else if (strcmp(argv[0], "CadOff2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff2);
	else if (strcmp(argv[0], "CadOff3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff3);
	else if (strcmp(argv[0], "PatternOff")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].PatternOff);
	else if (strcmp(argv[0], "ToneNUM")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].ToneNUM);
	else if (strcmp(argv[0], "Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].Freq0);
	else if (strcmp(argv[0], "Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].Freq1);
	else if (strcmp(argv[0], "Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].Freq2);
	else if (strcmp(argv[0], "Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].Freq3);
	else if (strcmp(argv[0], "Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].Gain0);
	else if (strcmp(argv[0], "Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].Gain1);
	else if (strcmp(argv[0], "Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].Gain2);
	else if (strcmp(argv[0], "Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].Gain3);
	else if (strcmp(argv[0], "C1_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C1_Freq0);
	else if (strcmp(argv[0], "C1_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C1_Freq1);
	else if (strcmp(argv[0], "C1_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C1_Freq2);
	else if (strcmp(argv[0], "C1_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C1_Freq3);
	else if (strcmp(argv[0], "C1_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C1_Gain0);
	else if (strcmp(argv[0], "C1_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C1_Gain1);
	else if (strcmp(argv[0], "C1_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C1_Gain2);
	else if (strcmp(argv[0], "C1_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C1_Gain3);
	else if (strcmp(argv[0], "C2_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C2_Freq0);
	else if (strcmp(argv[0], "C2_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C2_Freq1);
	else if (strcmp(argv[0], "C2_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C2_Freq2);
	else if (strcmp(argv[0], "C2_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C2_Freq3);
	else if (strcmp(argv[0], "C2_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C2_Gain0);
	else if (strcmp(argv[0], "C2_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C2_Gain1);
	else if (strcmp(argv[0], "C2_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C2_Gain2);
	else if (strcmp(argv[0], "C2_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C2_Gain3);
	else if (strcmp(argv[0], "C3_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C3_Freq0);
	else if (strcmp(argv[0], "C3_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C3_Freq1);
	else if (strcmp(argv[0], "C3_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C3_Freq2);
	else if (strcmp(argv[0], "C3_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C3_Freq3);
	else if (strcmp(argv[0], "C3_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C3_Gain0);
	else if (strcmp(argv[0], "C3_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C3_Gain1);
	else if (strcmp(argv[0], "C3_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C3_Gain2);
	else if (strcmp(argv[0], "C3_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C3_Gain3);
	else if (strcmp(argv[0], "CadOn4")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn4);
	else if (strcmp(argv[0], "CadOn5")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn5);
	else if (strcmp(argv[0], "CadOn6")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn6);
	else if (strcmp(argv[0], "CadOn7")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn7);
	else if (strcmp(argv[0], "CadOff4")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff4);
	else if (strcmp(argv[0], "CadOff5")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff5);
	else if (strcmp(argv[0], "CadOff6")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff6);
	else if (strcmp(argv[0], "CadOff7")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff7);
	else if (strcmp(argv[0], "C4_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C4_Freq0);
	else if (strcmp(argv[0], "C4_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C4_Freq1);
	else if (strcmp(argv[0], "C4_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C4_Freq2);
	else if (strcmp(argv[0], "C4_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C4_Freq3);
	else if (strcmp(argv[0], "C4_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C4_Gain0);
	else if (strcmp(argv[0], "C4_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C4_Gain1);
	else if (strcmp(argv[0], "C4_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C4_Gain2);
	else if (strcmp(argv[0], "C4_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C4_Gain3);
	else if (strcmp(argv[0], "C5_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C5_Freq0);
	else if (strcmp(argv[0], "C5_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C5_Freq1);
	else if (strcmp(argv[0], "C5_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C5_Freq2);
	else if (strcmp(argv[0], "C5_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C5_Freq3);
	else if (strcmp(argv[0], "C5_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C5_Gain0);
	else if (strcmp(argv[0], "C5_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C5_Gain1);
	else if (strcmp(argv[0], "C5_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C5_Gain2);
	else if (strcmp(argv[0], "C5_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C5_Gain3);
	else if (strcmp(argv[0], "C6_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C6_Freq0);
	else if (strcmp(argv[0], "C6_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C6_Freq1);
	else if (strcmp(argv[0], "C6_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C6_Freq2);
	else if (strcmp(argv[0], "C6_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C6_Freq3);
	else if (strcmp(argv[0], "C6_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C6_Gain0);
	else if (strcmp(argv[0], "C6_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C6_Gain1);
	else if (strcmp(argv[0], "C6_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C6_Gain2);
	else if (strcmp(argv[0], "C6_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C6_Gain3);
	else if (strcmp(argv[0], "C7_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C7_Freq0);
	else if (strcmp(argv[0], "C7_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C7_Freq1);
	else if (strcmp(argv[0], "C7_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C7_Freq2);
	else if (strcmp(argv[0], "C7_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C7_Freq3);
	else if (strcmp(argv[0], "C7_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C7_Gain0);
	else if (strcmp(argv[0], "C7_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C7_Gain1);
	else if (strcmp(argv[0], "C7_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C7_Gain2);
	else if (strcmp(argv[0], "C7_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C7_Gain3);
	else if (strcmp(argv[0], "CadOn8")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx] .CadOn8);
	else if (strcmp(argv[0], "CadOn9")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx] .CadOn9);
	else if (strcmp(argv[0], "CadOn10")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn10);
	else if (strcmp(argv[0], "CadOn11")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn11);
	else if (strcmp(argv[0], "CadOff8")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff8);
	else if (strcmp(argv[0], "CadOff9")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff9);
	else if (strcmp(argv[0], "CadOff10")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff10);
	else if (strcmp(argv[0], "CadOff11")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff11);
	else if (strcmp(argv[0], "C8_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C8_Freq0);
	else if (strcmp(argv[0], "C8_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C8_Freq1);
	else if (strcmp(argv[0], "C8_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C8_Freq2);
	else if (strcmp(argv[0], "C8_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C8_Freq3);
	else if (strcmp(argv[0], "C8_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C8_Gain0);
	else if (strcmp(argv[0], "C8_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C8_Gain1);
	else if (strcmp(argv[0], "C8_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C8_Gain2);
	else if (strcmp(argv[0], "C8_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C8_Gain3);
	else if (strcmp(argv[0], "C9_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C9_Freq0);
	else if (strcmp(argv[0], "C9_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C9_Freq1);
	else if (strcmp(argv[0], "C9_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C9_Freq2);
	else if (strcmp(argv[0], "C9_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C9_Freq3);
	else if (strcmp(argv[0], "C9_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C9_Gain0);
	else if (strcmp(argv[0], "C9_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C9_Gain1);
	else if (strcmp(argv[0], "C9_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C9_Gain2);
	else if (strcmp(argv[0], "C9_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C9_Gain3);
	else if (strcmp(argv[0], "C10_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C10_Freq0);
	else if (strcmp(argv[0], "C10_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C10_Freq1);
	else if (strcmp(argv[0], "C10_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C10_Freq2);
	else if (strcmp(argv[0], "C10_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C10_Freq3);
	else if (strcmp(argv[0], "C10_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C10_Gain0);
	else if (strcmp(argv[0], "C10_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C10_Gain1);
	else if (strcmp(argv[0], "C10_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C10_Gain2);
	else if (strcmp(argv[0], "C10_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C10_Gain3);
	else if (strcmp(argv[0], "C11_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C11_Freq0);
	else if (strcmp(argv[0], "C11_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C11_Freq1);
	else if (strcmp(argv[0], "C11_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C11_Freq2);
	else if (strcmp(argv[0], "C11_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C11_Freq3);
	else if (strcmp(argv[0], "C11_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C11_Gain0);
	else if (strcmp(argv[0], "C11_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C11_Gain1);
	else if (strcmp(argv[0], "C11_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C11_Gain2);
	else if (strcmp(argv[0], "C11_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C11_Gain3);
	else if (strcmp(argv[0], "CadOn12")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx] .CadOn12);
	else if (strcmp(argv[0], "CadOn13")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx] .CadOn13);
	else if (strcmp(argv[0], "CadOn14")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn14);
	else if (strcmp(argv[0], "CadOn15")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn15);
	else if (strcmp(argv[0], "CadOff12")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff12);
	else if (strcmp(argv[0], "CadOff13")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff13);
	else if (strcmp(argv[0], "CadOff14")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff14);
	else if (strcmp(argv[0], "CadOff15")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff15);
	else if (strcmp(argv[0], "C12_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C12_Freq0);
	else if (strcmp(argv[0], "C12_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C12_Freq1);
	else if (strcmp(argv[0], "C12_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C12_Freq2);
	else if (strcmp(argv[0], "C12_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C12_Freq3);
	else if (strcmp(argv[0], "C12_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C12_Gain0);
	else if (strcmp(argv[0], "C12_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C12_Gain1);
	else if (strcmp(argv[0], "C12_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C12_Gain2);
	else if (strcmp(argv[0], "C12_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C12_Gain3);
	else if (strcmp(argv[0], "C13_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C13_Freq0);
	else if (strcmp(argv[0], "C13_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C13_Freq1);
	else if (strcmp(argv[0], "C13_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C13_Freq2);
	else if (strcmp(argv[0], "C13_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C13_Freq3);
	else if (strcmp(argv[0], "C13_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C13_Gain0);
	else if (strcmp(argv[0], "C13_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C13_Gain1);
	else if (strcmp(argv[0], "C13_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C13_Gain2);
	else if (strcmp(argv[0], "C13_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C13_Gain3);
	else if (strcmp(argv[0], "C14_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C14_Freq0);
	else if (strcmp(argv[0], "C14_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C14_Freq1);
	else if (strcmp(argv[0], "C14_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C14_Freq2);
	else if (strcmp(argv[0], "C14_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C14_Freq3);
	else if (strcmp(argv[0], "C14_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C14_Gain0);
	else if (strcmp(argv[0], "C14_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C14_Gain1);
	else if (strcmp(argv[0], "C14_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C14_Gain2);
	else if (strcmp(argv[0], "C14_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C14_Gain3);
	else if (strcmp(argv[0], "C15_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C15_Freq0);
	else if (strcmp(argv[0], "C15_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C15_Freq1);
	else if (strcmp(argv[0], "C15_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C15_Freq2);
	else if (strcmp(argv[0], "C15_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C15_Freq3);
	else if (strcmp(argv[0], "C15_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C15_Gain0);
	else if (strcmp(argv[0], "C15_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C15_Gain1);
	else if (strcmp(argv[0], "C15_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C15_Gain2);
	else if (strcmp(argv[0], "C15_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C15_Gain3);
	else if (strcmp(argv[0], "CadOn16")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn16);
	else if (strcmp(argv[0], "CadOn17")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn17);
	else if (strcmp(argv[0], "CadOn18")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn18);
	else if (strcmp(argv[0], "CadOn19")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn19);
	else if (strcmp(argv[0], "CadOff16")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff16);
	else if (strcmp(argv[0], "CadOff17")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff17);
	else if (strcmp(argv[0], "CadOff18")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff18);
	else if (strcmp(argv[0], "CadOff19")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff19);
	else if (strcmp(argv[0], "C16_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C16_Freq0);
	else if (strcmp(argv[0], "C16_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C16_Freq1);
	else if (strcmp(argv[0], "C16_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C16_Freq2);
	else if (strcmp(argv[0], "C16_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C16_Freq3);
	else if (strcmp(argv[0], "C16_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C16_Gain0);
	else if (strcmp(argv[0], "C16_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C16_Gain1);
	else if (strcmp(argv[0], "C16_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C16_Gain2);
	else if (strcmp(argv[0], "C16_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C16_Gain3);
	else if (strcmp(argv[0], "C17_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C17_Freq0);
	else if (strcmp(argv[0], "C17_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C17_Freq1);
	else if (strcmp(argv[0], "C17_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C17_Freq2);
	else if (strcmp(argv[0], "C17_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C17_Freq3);
	else if (strcmp(argv[0], "C17_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C17_Gain0);
	else if (strcmp(argv[0], "C17_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C17_Gain1);
	else if (strcmp(argv[0], "C17_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C17_Gain2);
	else if (strcmp(argv[0], "C17_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C17_Gain3);
	else if (strcmp(argv[0], "C18_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C18_Freq0);
	else if (strcmp(argv[0], "C18_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C18_Freq1);
	else if (strcmp(argv[0], "C18_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C18_Freq2);
	else if (strcmp(argv[0], "C18_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C18_Freq3);
	else if (strcmp(argv[0], "C18_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C18_Gain0);
	else if (strcmp(argv[0], "C18_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C18_Gain1);
	else if (strcmp(argv[0], "C18_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C18_Gain2);
	else if (strcmp(argv[0], "C18_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C18_Gain3);
	else if (strcmp(argv[0], "C19_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C19_Freq0);
	else if (strcmp(argv[0], "C19_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C19_Freq1);
	else if (strcmp(argv[0], "C19_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C19_Freq2);
	else if (strcmp(argv[0], "C19_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C19_Freq3);
	else if (strcmp(argv[0], "C19_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C19_Gain0);
	else if (strcmp(argv[0], "C19_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C19_Gain1);
	else if (strcmp(argv[0], "C19_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C19_Gain2);
	else if (strcmp(argv[0], "C19_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C19_Gain3);
	else if (strcmp(argv[0], "CadOn20")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn20);
	else if (strcmp(argv[0], "CadOn21")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn21);
	else if (strcmp(argv[0], "CadOn22")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn22);
	else if (strcmp(argv[0], "CadOn23")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn23);
	else if (strcmp(argv[0], "CadOff20")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff20);
	else if (strcmp(argv[0], "CadOff21")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff21);
	else if (strcmp(argv[0], "CadOff22")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff22);
	else if (strcmp(argv[0], "CadOff23")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff23);
	else if (strcmp(argv[0], "C20_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C20_Freq0);
	else if (strcmp(argv[0], "C20_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C20_Freq1);
	else if (strcmp(argv[0], "C20_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C20_Freq2);
	else if (strcmp(argv[0], "C20_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C20_Freq3);
	else if (strcmp(argv[0], "C20_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C20_Gain0);
	else if (strcmp(argv[0], "C20_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C20_Gain1);
	else if (strcmp(argv[0], "C20_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C20_Gain2);
	else if (strcmp(argv[0], "C20_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C20_Gain3);
	else if (strcmp(argv[0], "C21_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C21_Freq0);
	else if (strcmp(argv[0], "C21_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C21_Freq1);
	else if (strcmp(argv[0], "C21_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C21_Freq2);
	else if (strcmp(argv[0], "C21_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C21_Freq3);
	else if (strcmp(argv[0], "C21_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C21_Gain0);
	else if (strcmp(argv[0], "C21_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C21_Gain1);
	else if (strcmp(argv[0], "C21_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C21_Gain2);
	else if (strcmp(argv[0], "C21_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C21_Gain3);
	else if (strcmp(argv[0], "C22_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C22_Freq0);
	else if (strcmp(argv[0], "C22_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C22_Freq1);
	else if (strcmp(argv[0], "C22_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C22_Freq2);
	else if (strcmp(argv[0], "C22_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C22_Freq3);
	else if (strcmp(argv[0], "C22_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C22_Gain0);
	else if (strcmp(argv[0], "C22_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C22_Gain1);
	else if (strcmp(argv[0], "C22_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C22_Gain2);
	else if (strcmp(argv[0], "C22_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C22_Gain3);
	else if (strcmp(argv[0], "C23_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C23_Freq0);
	else if (strcmp(argv[0], "C23_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C23_Freq1);
	else if (strcmp(argv[0], "C23_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C23_Freq2);
	else if (strcmp(argv[0], "C23_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C23_Freq3);
	else if (strcmp(argv[0], "C23_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C23_Gain0);
	else if (strcmp(argv[0], "C23_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C23_Gain1);
	else if (strcmp(argv[0], "C23_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C23_Gain2);
	else if (strcmp(argv[0], "C23_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C23_Gain3);
	else if (strcmp(argv[0], "CadOn24")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn24);
	else if (strcmp(argv[0], "CadOn25")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn25);
	else if (strcmp(argv[0], "CadOn26")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn26);
	else if (strcmp(argv[0], "CadOn27")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn27);
	else if (strcmp(argv[0], "CadOff24")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff24);
	else if (strcmp(argv[0], "CadOff25")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff25);
	else if (strcmp(argv[0], "CadOff26")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff26);
	else if (strcmp(argv[0], "CadOff27")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff27);
	else if (strcmp(argv[0], "C24_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C24_Freq0);
	else if (strcmp(argv[0], "C24_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C24_Freq1);
	else if (strcmp(argv[0], "C24_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C24_Freq2);
	else if (strcmp(argv[0], "C24_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C24_Freq3);
	else if (strcmp(argv[0], "C24_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C24_Gain0);
	else if (strcmp(argv[0], "C24_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C24_Gain1);
	else if (strcmp(argv[0], "C24_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C24_Gain2);
	else if (strcmp(argv[0], "C24_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C24_Gain3);
	else if (strcmp(argv[0], "C25_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C25_Freq0);
	else if (strcmp(argv[0], "C25_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C25_Freq1);
	else if (strcmp(argv[0], "C25_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C25_Freq2);
	else if (strcmp(argv[0], "C25_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C25_Freq3);
	else if (strcmp(argv[0], "C25_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C25_Gain0);
	else if (strcmp(argv[0], "C25_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C25_Gain1);
	else if (strcmp(argv[0], "C25_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C25_Gain2);
	else if (strcmp(argv[0], "C25_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C25_Gain3);
	else if (strcmp(argv[0], "C26_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C26_Freq0);
	else if (strcmp(argv[0], "C26_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C26_Freq1);
	else if (strcmp(argv[0], "C26_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C26_Freq2);
	else if (strcmp(argv[0], "C26_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C26_Freq3);
	else if (strcmp(argv[0], "C26_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C26_Gain0);
	else if (strcmp(argv[0], "C26_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C26_Gain1);
	else if (strcmp(argv[0], "C26_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C26_Gain2);
	else if (strcmp(argv[0], "C26_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C26_Gain3);
	else if (strcmp(argv[0], "C27_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C27_Freq0);
	else if (strcmp(argv[0], "C27_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C27_Freq1);
	else if (strcmp(argv[0], "C27_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C27_Freq2);
	else if (strcmp(argv[0], "C27_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C27_Freq3);
	else if (strcmp(argv[0], "C27_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C27_Gain0);
	else if (strcmp(argv[0], "C27_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C27_Gain1);
	else if (strcmp(argv[0], "C27_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C27_Gain2);
	else if (strcmp(argv[0], "C27_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C27_Gain3);
	else if (strcmp(argv[0], "CadOn28")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn28);
	else if (strcmp(argv[0], "CadOn29")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn29);
	else if (strcmp(argv[0], "CadOn30")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn30);
	else if (strcmp(argv[0], "CadOn31")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOn31);
	else if (strcmp(argv[0], "CadOff28")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff28);
	else if (strcmp(argv[0], "CadOff29")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff29);
	else if (strcmp(argv[0], "CadOff30")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff30);
	else if (strcmp(argv[0], "CadOff31")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].CadOff31);
	else if (strcmp(argv[0], "C28_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C28_Freq0);
	else if (strcmp(argv[0], "C28_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C28_Freq1);
	else if (strcmp(argv[0], "C28_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C28_Freq2);
	else if (strcmp(argv[0], "C28_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C28_Freq3);
	else if (strcmp(argv[0], "C28_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C28_Gain0);
	else if (strcmp(argv[0], "C28_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C28_Gain1);
	else if (strcmp(argv[0], "C28_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C28_Gain2);
	else if (strcmp(argv[0], "C28_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C28_Gain3);
	else if (strcmp(argv[0], "C29_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C29_Freq0);
	else if (strcmp(argv[0], "C29_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C29_Freq1);
	else if (strcmp(argv[0], "C29_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C29_Freq2);
	else if (strcmp(argv[0], "C29_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C29_Freq3);
	else if (strcmp(argv[0], "C29_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C29_Gain0);
	else if (strcmp(argv[0], "C29_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C29_Gain1);
	else if (strcmp(argv[0], "C29_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C29_Gain2);
	else if (strcmp(argv[0], "C29_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C29_Gain3);
	else if (strcmp(argv[0], "C30_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C30_Freq0);
	else if (strcmp(argv[0], "C30_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C30_Freq1);
	else if (strcmp(argv[0], "C30_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C30_Freq2);
	else if (strcmp(argv[0], "C30_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C30_Freq3);
	else if (strcmp(argv[0], "C30_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C30_Gain0);
	else if (strcmp(argv[0], "C30_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C30_Gain1);
	else if (strcmp(argv[0], "C30_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C30_Gain2);
	else if (strcmp(argv[0], "C30_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C30_Gain3);
	else if (strcmp(argv[0], "C31_Freq0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C31_Freq0);
	else if (strcmp(argv[0], "C31_Freq1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C31_Freq1);
	else if (strcmp(argv[0], "C31_Freq2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C31_Freq2);
	else if (strcmp(argv[0], "C31_Freq3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C31_Freq3);
	else if (strcmp(argv[0], "C31_Gain0")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C31_Gain0);
	else if (strcmp(argv[0], "C31_Gain1")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C31_Gain1);
	else if (strcmp(argv[0], "C31_Gain2")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C31_Gain2);
	else if (strcmp(argv[0], "C31_Gain3")==0)
		websWrite(wp, "%d", pCfg->cust_tone_para[cust_idx].C31_Gain3);
	else if (strcmp(argv[0], "display")==0)
	{
		if (pCfg->tone_of_country == TONE_CUSTOMER)
			websWrite(wp, "style=\"display:online\"");		
		else
			websWrite(wp, "style=\"display:none\"");		
	}
#if defined(CONFIG_RTK_VOIP_DRIVERS_SI3050) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89316)
	else if (strcmp(argv[0], "display_distone")==0)
		websWrite(wp, "%s", "");
#else
	else if (strcmp(argv[0], "display_distone")==0)
		websWrite(wp, "%s", "style=\"display:none\"");
#endif
	else if (strcmp(argv[0], "distone_num")==0)
	{
		for (i=0; i < DIS_CONNECT_TONE_MAX ;i++)
		{
			if (i == pCfg->distone_num)
				websWrite(wp, "<option value=%d selected>%s</option>", i, number_of_distone[i]);
			else
				websWrite(wp, "<option value=%d >%s</option>", i, number_of_distone[i]);
		}
	}
	else if (strcmp(argv[0], "d1display")==0)
	{
		if (pCfg->distone_num > 0)
#if defined(CONFIG_RTK_VOIP_DRIVERS_SI3050) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_LE89316)
			websWrite(wp, "style=\"display:online\"");		
#else
			websWrite(wp, "style=\"display:none\"");
#endif
		else
			websWrite(wp, "style=\"display:none\"");		
	}
	else if (strcmp(argv[0], "d1freqnum")==0)
	{
		for (i=1; i < DIS_CONNECT_TONE_FREQ_MAX ;i++)
		{
			if (i == pCfg->d1freqnum)
				websWrite(wp, "<option selected>%s</option>", number_of_distone[i]);
			else
				websWrite(wp, "<option>%s</option>", number_of_distone[i]);
		}
	}
	else if (strcmp(argv[0], "d1Freq1")==0)
		websWrite(wp, "%d", pCfg->d1Freq1);
	else if (strcmp(argv[0], "d1Freq2")==0)
		websWrite(wp, "%d", pCfg->d1Freq2);
	else if (strcmp(argv[0], "d1Accur")==0)
		websWrite(wp, "%d", pCfg->d1Accur);
	else if (strcmp(argv[0], "d1Level")==0)
		websWrite(wp, "%d", pCfg->d1Level);
	else if (strcmp(argv[0], "d1ONup")==0)
		websWrite(wp, "%d", pCfg->d1ONup);
	else if (strcmp(argv[0], "d1ONlow")==0)
		websWrite(wp, "%d", pCfg->d1ONlow);
	else if (strcmp(argv[0], "d1OFFup")==0)
		websWrite(wp, "%d", pCfg->d1OFFup);
	else if (strcmp(argv[0], "d1OFFlow")==0)
		websWrite(wp, "%d", pCfg->d1OFFlow);
	else if (strcmp(argv[0], "d2display")==0)
	{
		if (pCfg->distone_num > 1)
			websWrite(wp, "style=\"display:online\"");		
		else
			websWrite(wp, "style=\"display:none\"");		
	}
	else if (strcmp(argv[0], "d2freqnum")==0)
	{
		for (i=1; i < DIS_CONNECT_TONE_FREQ_MAX ;i++)
		{
			if (i == pCfg->d2freqnum)
				websWrite(wp, "<option selected>%s</option>", number_of_distone[i]);
			else
				websWrite(wp, "<option>%s</option>", number_of_distone[i]);
		}
	}
	else if (strcmp(argv[0], "d2Freq1")==0)
		websWrite(wp, "%d", pCfg->d2Freq1);
	else if (strcmp(argv[0], "d2Freq2")==0)
		websWrite(wp, "%d", pCfg->d2Freq2);
	else if (strcmp(argv[0], "d2Accur")==0)
		websWrite(wp, "%d", pCfg->d2Accur);
	else if (strcmp(argv[0], "d2Level")==0)
		websWrite(wp, "%d", pCfg->d2Level);
	else if (strcmp(argv[0], "d2ONup")==0)
		websWrite(wp, "%d", pCfg->d2ONup);
	else if (strcmp(argv[0], "d2ONlow")==0)
		websWrite(wp, "%d", pCfg->d2ONlow);
	else if (strcmp(argv[0], "d2OFFup")==0)
		websWrite(wp, "%d", pCfg->d2OFFup);
	else if (strcmp(argv[0], "d2OFFlow")==0)
		websWrite(wp, "%d", pCfg->d2OFFlow);



	

	return 0;
}


