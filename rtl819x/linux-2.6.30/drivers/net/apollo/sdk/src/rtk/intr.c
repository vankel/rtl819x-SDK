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


/*
 * Include Files
 */
#include <dal/dal_mgmt.h> 
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <rtk/intr.h> 
#include <common/rt_type.h>
#include <osal/print.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_intr_init
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
int32
rtk_intr_init(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_init */


/* Function Name:
 *      rtk_intr_polarity_set
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
int32 
rtk_intr_polarity_set(rtk_intr_polarity_t polar)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_polarity_set( polar);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_polarity_set */


/* Function Name:
 *      rtk_intr_polarity_mode_get
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
int32 
rtk_intr_polarity_get(rtk_intr_polarity_t *pPolar)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_polarity_get( pPolar);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_polarity_get */


/* Function Name:
 *      rtk_intr_imr_set
 * Description:
 *      Set interrupt mask.
 * Input:
 *      intr          - interrupt type
 *      enable       - interrupt state 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_OUT_OF_RANGE
 * Note:
 *	   None.	
 */
int32 
rtk_intr_imr_set(rtk_intr_type_t intr, rtk_enable_t enable)
{
    int32   ret;
    RTK_API_LOCK();
	ret = RT_MAPPER->intr_imr_set( intr, enable);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_imr_set */


/* Function Name:
 *      rtk_intr_imr_get
 * Description:
 *      Get interrupt mask.
 * Input:
 *      intr            - interrupt type
 *      pEnable      - pointer of status 
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
rtk_intr_imr_get(rtk_intr_type_t intr, rtk_enable_t *pEnable)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_imr_get( intr, pEnable);
    RTK_API_UNLOCK();
    return ret;
}/* end of rtk_intr_imr_get */


/* Function Name:
 *      rtk_intr_ims_get
 * Description:
 *      Get interrupt status.
 * Input:
 *      intr            - interrupt type
 *      pStatus      - pointer of return status 
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
int32 
rtk_intr_ims_get(rtk_intr_type_t intr, rtk_enable_t *pStatus)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_ims_get( intr, pStatus);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_ims_get */


/* Function Name:
 *      rtk_intr_ims_clear
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
int32 
rtk_intr_ims_clear(rtk_intr_type_t intr)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_ims_clear( intr);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_ims_clear */


/* Function Name:
 *      rtk_intr_speedChangeStatus_get
 * Description:
 *      Get interrupt status of speed change.
 * Input:
 *      pPortMask          - pointer of return port status (bitmask)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32 
rtk_intr_speedChangeStatus_get(rtk_portmask_t *pPortMask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_speedChangeStatus_get(pPortMask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_sts_speed_change_get */


/* Function Name:
 *      rtk_intr_speedChangeStatus_clear
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
int32 
rtk_intr_speedChangeStatus_clear(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_speedChangeStatus_clear();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_sts_speed_change_clear */


/* Function Name:
 *      rtk_intr_linkupStatus_get
 * Description:
 *      Get interrupt status of linkup.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
 int32 
rtk_intr_linkupStatus_get(rtk_portmask_t *pPortMask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_linkupStatus_get( pPortMask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_sts_linkup_get */


/* Function Name:
 *      rtk_intr_linkupStatus_clear
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
int32 
rtk_intr_linkupStatus_clear(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_linkupStatus_clear();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_sts_linkup_clear */


/* Function Name:
 *      rtk_intr_linkdownStatus_get
 * Description:
 *      Get interrupt status of linkdown.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32 
rtk_intr_linkdownStatus_get(rtk_portmask_t *pPortMask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_linkdownStatus_get( pPortMask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_sts_linkdown_get */


/* Function Name:
 *      rtk_intr_linkdownStatus_clear
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
int32 
rtk_intr_linkdownStatus_clear(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_linkdownStatus_clear();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_sts_linkdown_clear */


/* Function Name:
 *      rtk_intr_sts_gphy_get
 * Description:
 *      Get interrupt status of GPHY.
 * Input:
 *      pPortMask          - pointer of return status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32 
rtk_intr_gphyStatus_get(rtk_portmask_t *pPortMask)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_gphyStatus_get( pPortMask);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_sts_gphy_get */


/* Function Name:
 *      rtk_intr_gphyStatus_clear
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
int32 
rtk_intr_gphyStatus_clear(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->intr_gphyStatus_clear();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_intr_sts_gphy_clear */



/* Function Name:
 *      rtk_intr_imr_restore
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
int32 
rtk_intr_imr_restore(uint32 imrValue)
{
	int32	ret;
	RTK_API_LOCK();
	ret = RT_MAPPER->intr_imr_restore(imrValue);
	RTK_API_UNLOCK();
	return ret;
}


