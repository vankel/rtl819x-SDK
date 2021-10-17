/*
 * Copyright (C) 2012 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 
 * $Date: 2012-04-20 14:45:20 +0800 (FRI, 20 Apr. 2012) $
 *
 * Purpose : Definition those public Interrupt APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Interrupt parameter settings
 */


#ifndef __DAL_APOLLOMP_INTR_H__
#define __DAL_APOLLOMP_INTR_H__


/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/intr.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
#define INTR_PORT_MASK 0x7f
#define INTR_IMR_MASK  0xffff


/*
 * Function Declaration
 */

/* Function Name:
 *      dal_apollomp_intr_init
 * Description:
 *      Initialize interrupt module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize interrupt module before calling any interrupt APIs.
 */
extern int32
dal_apollomp_intr_init(void);


/* Function Name:
 *      dal_apollomp_intr_polarity_set
 * Description:
 *      Set interrupt polarity mode
 * Input:
 *      mode - Interrupt polarity mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK		- OK
 *      RT_ERR_FAILED	- Failed
 *      RT_ERR_INPUT	- Invalid input parameters.
 * Note:
 *      The API can set Interrupt polarity mode.
 *      The modes that can be set as following:
 *      - INTR_POLAR_HIGH
 *      - INTR_POLAR_LOW 
 */
extern int32 
dal_apollomp_intr_polarity_set(rtk_intr_polarity_t polar);


/* Function Name:
 *      dal_apollomp_intr_polarity_get
 * Description:
 *      Get Interrupt polarity mode
 * Input:
 *      None
 * Output:
 *      pMode - Interrupt polarity mode
 * Return:
 *      RT_ERR_OK      	- OK
 *      RT_ERR_FAILED	- Failed
 *      RT_ERR_INPUT 	- Invalid input parameters.
 * Note:
 *      The API can get Interrupt polarity mode.
 *	  The modes that can be got as following:
 *      - INTR_POLAR_HIGH
 *      - INTR_POLAR_LOW 
 */
extern int32 
dal_apollomp_intr_polarity_get(rtk_intr_polarity_t *pPolar);


/* Function Name:
 *      dal_apollomp_intr_imr_set
 * Description:
 *      Set interrupt mask.
 * Input:
 *      intr          - interrupt type
 *      enable      - interrupt status 
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *	   None.	
 */
extern int32 
dal_apollomp_intr_imr_set(rtk_intr_type_t intr, rtk_enable_t enable);


/* Function Name:
 *      dal_apollomp_intr_imr_get
 * Description:
 *      Get interrupt mask.
 * Input:
 *      intr            - interrupt type
 *      pEnable      - pointer of return  state 
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
 extern int32 
dal_apollomp_intr_imr_get(rtk_intr_type_t intr, rtk_enable_t *pEnable);


/* Function Name:
 *      dal_apollomp_intr_ims_get
 * Description:
 *      Get interrupt status.
 * Input:
 *      intr            - interrupt type
 *      pState        - pointer of return status 
 *
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_ims_get(rtk_intr_type_t intr, rtk_enable_t *pState);


/* Function Name:
 *      dal_apollomp_intr_ims_clear
 * Description:
 *      Clear interrupt status.
 * Input:
 *      intr            - interrupt type
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_ims_clear(rtk_intr_type_t intr);


/* Function Name:
 *      dal_apollomp_intr_speedChangeStatus_get
 * Description:
 *      Get interrupt status of speed change.
 * Input:
 *      pPortMask        - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_speedChangeStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      dal_apollomp_intr_speedChangeStatus_clear
 * Description:
 *      Clear interrupt status of port speed change.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_speedChangeStatus_clear(void);


/* Function Name:
 *      dal_apollomp_intr_linkupStatus_get
 * Description:
 *      Get interrupt status of linkup.
 * Input:
 *      pPortMask      - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_linkupStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      dal_apollomp_intr_linkupStatus_clear
 * Description:
 *      Clear interrupt status of linkup.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_linkupStatus_clear(void);


/* Function Name:
 *      dal_apollomp_intr_linkdownStatus_get
 * Description:
 *      Get interrupt status of linkdown.
 * Input:
 *      pPortMask        - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_linkdownStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      dal_apollomp_intr_linkdownStatus_clear
 * Description:
 *      Clear interrupt status of linkdown.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_linkdownStatus_clear(void);


/* Function Name:
 *      dal_apollomp_intr_gphyStatus_get
 * Description:
 *      Get interrupt status of GPHY.
 * Input:
 *      pPortMask      - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_gphyStatus_get(rtk_portmask_t *pPortMask);


/* Function Name:
 *      dal_apollomp_intr_gphyStatus_clear
 * Description:
 *      Clear interrupt status of GPHY.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_gphyStatus_clear(void);


/* Function Name:
 *      dal_apollomp_intr_imr_restore
 * Description:
 *     set imr mask from input value
 * Input:
 *      imrValue: imr value
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
extern int32 
dal_apollomp_intr_imr_restore(uint32 imrValue);




#endif /* __DAL_APOLLOMP_INTR_H__ */
