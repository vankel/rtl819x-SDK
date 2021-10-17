/*
** Copyright (c) 2007-2012 by Silicon Laboratories
**
** $Id: si_voice.c 3820 2013-02-04 20:23:25Z cdp $
**
** si_voice.c
**
** Author(s): 
** laj
**
** Distributed by: 
** Silicon Laboratories, Inc
**
** This file contains proprietary information.	 
** No dissemination allowed without prior written permission from
** Silicon Laboratories, Inc.
**
** File Description:
** This is the header file for the ProSLIC driver.
**
** Dependancies:
** si_voice_datatypes.h
**
*/
#include "si_voice_datatypes.h"
#include "si_voice.h"

#ifndef DISABLE_MALLOC
#ifdef REALTEK_PATCH_FOR_SILAB
#include <linux/slab.h>
#else
#include "stdlib.h" /* for malloc */
#include "string.h" /* memset */
#endif
#endif


/* 
** Control object constructor/descructor
*/
#ifdef REALTEK_PATCH_FOR_SILAB
int SiVoice_createControlInterface (SiVoiceControlInterfaceType **pCtrlIntf){

  	*pCtrlIntf = kmalloc(sizeof(SiVoiceControlInterfaceType), 0x20); //GFP_ATOMIC
	memset (*pCtrlIntf,0,sizeof(SiVoiceControlInterfaceType));
    	return RC_NONE;
}

int SiVoice_destroyControlInterface (SiVoiceControlInterfaceType **pCtrlIntf){

	kfree ((SiVoiceControlInterfaceType*)*pCtrlIntf);
	*pCtrlIntf = NULL;
	return RC_NONE;
}

/* 
** Device object constructor/destructor
*/
int SiVoice_createDevice (SiVoiceDeviceType **pDevice){

	*pDevice = kmalloc (sizeof(SiVoiceDeviceType), 0x20);	//GFP_ATOMIC
	memset(*pDevice,0,sizeof(SiVoiceDeviceType));
	return RC_NONE;
}
int SiVoice_destroyDevice (SiVoiceDeviceType **pDevice){

	kfree ((SiVoiceDeviceType*)*pDevice);
	*pDevice = NULL;
	return RC_NONE;
}

/*
** Channel object constructor/destructor
*/
int SiVoice_createChannel (SiVoiceChanType_ptr *pChan){

	*pChan = kmalloc(sizeof(SiVoiceChanType), 0x20);	//GFP_ATOMIC
	memset(*pChan,0,sizeof(SiVoiceChanType));
	return RC_NONE;
}
int SiVoice_destroyChannel (SiVoiceChanType_ptr *hProslic){

	kfree ((SiVoiceChanType_ptr)*hProslic);
	*hProslic = NULL;
	return RC_NONE;
}

#else
int SiVoice_createControlInterface (SiVoiceControlInterfaceType **pCtrlIntf){

#ifndef DISABLE_MALLOC
  	*pCtrlIntf = malloc(sizeof(SiVoiceControlInterfaceType));
	memset (*pCtrlIntf,0,sizeof(SiVoiceControlInterfaceType));
    return RC_NONE;
#else
	return RC_UNSUPPORTED_FEATURE;
#endif
}

int SiVoice_destroyControlInterface (SiVoiceControlInterfaceType **pCtrlIntf){

#ifndef DISABLE_MALLOC
	free ((SiVoiceControlInterfaceType*)*pCtrlIntf);
	*pCtrlIntf = NULL;
	return RC_NONE;
#else
	return RC_UNSUPPORTED_FEATURE;
#endif
}

/* 
** Device object constructor/destructor
*/
int SiVoice_createDevice (SiVoiceDeviceType **pDevice){

#ifndef DISABLE_MALLOC
	*pDevice = malloc (sizeof(SiVoiceDeviceType));
	memset(*pDevice,0,sizeof(SiVoiceDeviceType));
	return RC_NONE;
#else
	return RC_UNSUPPORTED_FEATURE;
#endif
}
int SiVoice_destroyDevice (SiVoiceDeviceType **pDevice){

#ifndef DISABLE_MALLOC
	free ((SiVoiceDeviceType*)*pDevice);
	*pDevice = NULL;
	return RC_NONE;
#else
	return RC_UNSUPPORTED_FEATURE;
#endif
}

/*
** Channel object constructor/destructor
*/
int SiVoice_createChannel (SiVoiceChanType_ptr *pChan){

#ifndef DISABLE_MALLOC
	*pChan = malloc(sizeof(SiVoiceChanType));
	memset(*pChan,0,sizeof(SiVoiceChanType));
	return RC_NONE;
#else
	return RC_UNSUPPORTED_FEATURE; 
#endif
}
int SiVoice_destroyChannel (SiVoiceChanType_ptr *hProslic){

#ifndef DISABLE_MALLOC
	free ((SiVoiceChanType_ptr)*hProslic);
	*hProslic = NULL;
	return RC_NONE;
#else
	return RC_UNSUPPORTED_FEATURE;
#endif
}
#endif

/*
** Host control linkage
*/
int SiVoice_setControlInterfaceCtrlObj (SiVoiceControlInterfaceType *pCtrlIntf, void *hCtrl){
	pCtrlIntf->hCtrl = hCtrl;
	return RC_NONE;
}

/*
** Host reset linkage
*/
int SiVoice_setControlInterfaceReset (SiVoiceControlInterfaceType *pCtrlIntf, ctrl_Reset_fptr Reset_fptr){
	pCtrlIntf->Reset_fptr = Reset_fptr;
	return RC_NONE;
}

/*
** Host register/RAM read/write linkage
*/
int SiVoice_setControlInterfaceWriteRegister (SiVoiceControlInterfaceType *pCtrlIntf, ctrl_WriteRegister_fptr WriteRegister_fptr){
	pCtrlIntf->WriteRegister_fptr = WriteRegister_fptr;
	return RC_NONE;
}
int SiVoice_setControlInterfaceReadRegister (SiVoiceControlInterfaceType *pCtrlIntf, ctrl_ReadRegister_fptr ReadRegister_fptr){
	pCtrlIntf->ReadRegister_fptr = ReadRegister_fptr;
	return RC_NONE;
}
int SiVoice_setControlInterfaceWriteRAM (SiVoiceControlInterfaceType *pCtrlIntf, ctrl_WriteRAM_fptr WriteRAM_fptr){
	pCtrlIntf->WriteRAM_fptr = WriteRAM_fptr;
	return RC_NONE;
}
int SiVoice_setControlInterfaceReadRAM (SiVoiceControlInterfaceType *pCtrlIntf, ctrl_ReadRAM_fptr ReadRAM_fptr){
	pCtrlIntf->ReadRAM_fptr = ReadRAM_fptr; 
	return RC_NONE;
}
/*
** Host timer linkage
*/
int SiVoice_setControlInterfaceTimerObj (SiVoiceControlInterfaceType *pCtrlIntf, void *hTimer){
	pCtrlIntf->hTimer = hTimer;
	return RC_NONE;
}
int SiVoice_setControlInterfaceDelay (SiVoiceControlInterfaceType *pCtrlIntf, system_delay_fptr Delay_fptr){
	pCtrlIntf->Delay_fptr = Delay_fptr;
	return RC_NONE;
}
int SiVoice_setControlInterfaceSemaphore (SiVoiceControlInterfaceType *pCtrlIntf, ctrl_Semaphore_fptr semaphore_fptr){
	pCtrlIntf->Semaphore_fptr = semaphore_fptr;
	return RC_NONE;
}
int SiVoice_setControlInterfaceTimeElapsed (SiVoiceControlInterfaceType *pCtrlIntf, system_timeElapsed_fptr timeElapsed_fptr){
	pCtrlIntf->timeElapsed_fptr = timeElapsed_fptr;
	return RC_NONE;
}
int SiVoice_setControlInterfaceGetTime (SiVoiceControlInterfaceType *pCtrlIntf, system_getTime_fptr getTime_fptr){
	pCtrlIntf->getTime_fptr = getTime_fptr;
	return RC_NONE;
}

/*
** Channel object initialization
*/
int SiVoice_SWInitChan (SiVoiceChanType_ptr pChan,int channel,int chipType, SiVoiceDeviceType*pDeviceObj, SiVoiceControlInterfaceType *pCtrlIntf){
	pChan->channel = (uInt8)channel;
	pChan->deviceId = pDeviceObj;
	pChan->deviceId->ctrlInterface = pCtrlIntf;
	pChan->channelEnable=1;
	pChan->deviceId->usermodeStatus = 0;
	pChan->deviceId->ctrlInterface->usermodeStatus = 0;
	pChan->error = RC_NONE;
    pChan->dcdc_polarity_invert = 0;
#ifdef PROSLIC_BOM_DEFAULT
	pChan->bomOption = PROSLIC_BOM_DEFAULT;
#else
	pChan->bomOption = 0;
#endif
	switch (chipType){
		case SI321X_TYPE:
			pChan->deviceId->chipType = SI3210;
#ifdef REALTEK_PATCH_FOR_SILAB
			printk("Warning: need to set correct DC-DC structure for your SLLC type in %s - %s - line %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
			break;
		case SI324X_TYPE:
			pChan->deviceId->chipType = SI3240;
#ifdef REALTEK_PATCH_FOR_SILAB
			printk("Warning: need to set correct DC-DC structure for your SLLC type in %s - %s - line %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
			break;
		case SI3220_TYPE:
			pChan->deviceId->chipType = SI3220;
#ifdef REALTEK_PATCH_FOR_SILAB
			printk("Warning: need to set correct DC-DC structure for your SLLC type in %s - %s - line %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
			break;
		case SI3226_TYPE:
#ifndef REALTEK_PATCH_FOR_SILAB
			pChan->deviceId->chipType = SI3226;
#else
			pChan->deviceId->chipType = SI3226;
			pChan->channelType = SI3226;
			pChan->bomOption = BO_DCDC_QCUK;
			printk("Set to BO_DCDC_QCUK\n");
#endif
			break;
		case SI3217X_TYPE:
#ifndef REALTEK_PATCH_FOR_SILAB
			pChan->deviceId->chipType = SI32171;
#else
			pChan->deviceId->chipType = SI32178;
			pChan->channelType = SI32178;
	#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3217x_BB 
			pChan->bomOption = BO_DCDC_BUCK_BOOST;
			printk("Set to BO_DCDC_BUCK_BOOST\n");
	#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3217x_FB
			pChan->bomOption = BO_DCDC_FLYBACK;
			printk("Set to BO_DCDC_FLYBACK\n");
	#elif defined CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3217x_PBB
			pChan->bomOption = BO_DCDC_PMOS_BUCK_BOOST;
			printk("Set to BO_DCDC_PMOS_BUCK_BOOST\n");
	#else
			printk("Warning: need to set correct DC-DC structure for your SLLC type in %s - %s - line %d\n", __FILE__, __FUNCTION__, __LINE__);
	#endif
#endif
			break;
		case SI3226X_TYPE:
#ifndef REALTEK_PATCH_FOR_SILAB
			pChan->deviceId->chipType = SI32260;
#else
			pChan->deviceId->chipType = SI32261;
			pChan->channelType = SI32261;
#ifdef CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226x_FB
			pChan->bomOption = BO_DCDC_FLYBACK;
			printk("Set to BO_DCDC_FLYBACK\n");
#elif defined (CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226x_QCUK)
			pChan->bomOption = BO_DCDC_QCUK;
			printk("Set to BO_DCDC_QCUK\n");
#elif defined (CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3226x_PBB)
			pChan->bomOption = BO_DCDC_PMOS_BUCK_BOOST;
			printk("Set to BO_DCDC_PMOS_BUCK_BOOST\n");
#else
			printk("Warning: need to set correct DC-DC structure for your SLLC type in %s - %s - line %d\n", __FILE__, __FUNCTION__, __LINE__);
#endif
#endif
			break;
        case SI3050_TYPE:
            pChan->deviceId->chipType = SI3050;
            break;
	}
	return RC_NONE;
}

/*
** Reset control
*/
int SiVoice_Reset (SiVoiceChanType_ptr pChan){
    /*
    ** assert reset, wait 250ms, release reset, wait 250ms
    */
    pChan->deviceId->ctrlInterface->Reset_fptr(pChan->deviceId->ctrlInterface->hCtrl,1);
    pChan->deviceId->ctrlInterface->Delay_fptr(pChan->deviceId->ctrlInterface->hTimer,250);
    pChan->deviceId->ctrlInterface->Reset_fptr(pChan->deviceId->ctrlInterface->hCtrl,0);
    pChan->deviceId->ctrlInterface->Delay_fptr(pChan->deviceId->ctrlInterface->hTimer,250);

    /* Special case for 321x: need to enable daisy chain... */
#ifdef SI321X
	if ( (pChan->deviceId->chipType >= SI3210)
		&& (pChan->deviceId->chipType <= SI3216M) )
	{
		setDaisyChainMode(pChan);
	}
#endif
    return RC_NONE;
}

/*
** Debug Mode Control
*/
int SiVoice_setSWDebugMode (SiVoiceChanType_ptr pChan, int debugEn){
	pChan->debugMode = debugEn;
	return RC_NONE;
}

/*
** Error status
*/
int SiVoice_getErrorFlag (SiVoiceChanType_ptr pChan, int*error){
	*error = pChan->error;
	return RC_NONE;
}
int SiVoice_clearErrorFlag (SiVoiceChanType_ptr pChan){
	pChan->error = RC_NONE;
	return RC_NONE;
}

/* 
** Channel status
*/
int SiVoice_setChannelEnable (SiVoiceChanType_ptr pChan, int chanEn){
	pChan->channelEnable = chanEn;
	return RC_NONE;
}
int SiVoice_getChannelEnable (SiVoiceChanType_ptr pChan, int* chanEn){
	*chanEn = pChan->channelEnable;
	return RC_NONE;
}
