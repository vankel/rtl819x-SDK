#ifndef __HAL8813AE_DEF_H__
#define __HAL8813AE_DEF_H__

/*++
Copyright (c) Realtek Semiconductor Corp. All rights reserved.

Module Name:
	Hal8192EEDef.h
	
Abstract:
	Defined HAL 8813AE data structure & Define
	    
Major Change History:
	When       Who               What
	---------- ---------------   -------------------------------
	2013-05-28 Filen            Create.	
--*/


RT_STATUS
InitPON8813AE(
    IN  HAL_PADAPTER    Adapter,
    IN  u4Byte          ClkSel        
);

RT_STATUS
StopHW8813AE(
    IN  HAL_PADAPTER    Adapter
);


RT_STATUS	
hal_Associate_8813AE(
    HAL_PADAPTER        Adapter,
    BOOLEAN             IsDefaultAdapter
);
















#endif  //__HAL8813AE_DEF_H__

