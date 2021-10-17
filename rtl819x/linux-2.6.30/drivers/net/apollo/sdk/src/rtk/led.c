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
 * $Revision: 14000 $
 * $Date: 2010-11-08 17:47:25 +0800 (?üÊ?‰∏Ä, 08 ?Å‰???2010) $
 *
 * Purpose : Definition of Security API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) attack prevention 
 */

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/init.h> 
#include <rtk/default.h> 
#include <rtk/led.h> 
#include <dal/dal_mgmt.h> 


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
 *      rtk_led_init
 * Description:
 *      Initialize led module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize led module before calling any led APIs.
 */
int32
rtk_led_init(void)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_init();
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_init */

/* Function Name:
 *      rtk_led_operation_set
 * Description:
 *      Get Led operation mode
 * Input:
 *      None
 * Output:
 *      pMode - LED operation mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can set Led operation mode.
 *      The modes that can be set are as following:
 *      - LED_OP_PARALLEL 17 led
 *      - LED_OP_SERIAL   32 led 
 */
int32 
rtk_led_operation_get(rtk_led_operation_t *pMode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_operation_get( pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_operation_get */


/* Function Name:
 *      rtk_led_operation_set
 * Description:
 *      Set Led operation mode
 * Input:
 *      mode - LED operation mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can set Led operation mode.
 *      The modes that can be set are as following:
 *      - LED_OP_PARALLEL 17 led
 *      - LED_OP_SERIAL   32 led 
 */
int32 
rtk_led_operation_set(rtk_led_operation_t mode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_operation_set( mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_operation_set */


/* Function Name:
 *      rtk_led_serialMode_set
 * Description:
 *      Set Led serial mode active congiuration
 * Input:
 *      None
 * Output:
 *      pActive - high low active mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can set LED serial mode active congiuration.
 */
int32 
rtk_led_serialMode_get(rtk_led_active_t *pActive)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_serialMode_get( pActive);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_serialMode_get */

/* Function Name:
 *      rtk_led_serialMode_set
 * Description:
 *      Set Led serial mode active congiuration
 * Input:
 *      active - high low active mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can set LED serial mode active congiuration.
 */
int32 
rtk_led_serialMode_set(rtk_led_active_t active)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_serialMode_set( active);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_serialMode_set */

/* Function Name:
 *      rtk_led_blinkRate_get
 * Description:
 *      Get LED blinking rate at mode 0 to mode 3
 * Input:
 *      group      - led blinking group
 * Output:
 *      pBlinkRate - blinking rate.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      There are 8 types of LED blinking rates at 32ms, 48ms, 64ms, 96ms, 128ms, 256ms, 512ms, and 1024ms.
 */
int32 
rtk_led_blinkRate_get(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t *pBlinkRate)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_blinkRate_get( group, pBlinkRate);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_blinkRate_get */

/* Function Name:
 *      rtk_led_blinkRate_set
 * Description:
 *      Get LED blinking rate
 * Input:
 *      group      - led blinking group
 *      blinkRate  - blinking rate.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      There are 8 types of LED blinking rates at 32ms, 48ms, 64ms, 96ms, 128ms, 256ms, 512ms, and 1024ms.
 */
int32 
rtk_led_blinkRate_set(rtk_led_blinkGroup_t group, rtk_led_blink_rate_t blinkRate)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_blinkRate_set( group, blinkRate);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_blinkRate_set */


/* Function Name:
 *      rtk_led_groupConfig_set
 * Description:
 *      Set per group Led to congiuration mode
 * Input:
 *      ledIdx  - LED index id.
 *      type    - LED type
 *      pConfig  - LED configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32 
rtk_led_config_set(uint32 ledIdx, rtk_led_type_t type, rtk_led_config_t *pConfig)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_config_set( ledIdx, type, pConfig);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_config_set */

/* Function Name:
 *      rtk_led_groupConfig_set
 * Description:
 *      Set per group Led to congiuration mode
 * Input:
 *      ledIdx  - LED index id.
 * Output:
 *      pType    - LED type
 *      pConfig  - LED configuration
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 */
int32 
rtk_led_config_get(uint32 ledIdx, rtk_led_type_t *pType, rtk_led_config_t *pConfig)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_config_get( ledIdx, pType, pConfig);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_config_get */

/* Function Name:
 *      rtk_led_modeForce_get
 * Description:
 *      Get Led group to congiuration force mode
 * Input:
 *      ledIdx - LED index id.
 * Output:
 *      pMode  - LED force mode.
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_NORMAL,
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 *      For LED_OP_SERIAL the max led index is 31  
 *      For LED_OP_PARALLEL the max led index is 16  
 */
int32
rtk_led_modeForce_get(uint32 ledIdx, rtk_led_force_mode_t *pMode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_modeForce_get( ledIdx, pMode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_modeForce_get */

/* Function Name:
 *      rtk_led_modeForce_set
 * Description:
 *      Set Led group to congiuration force mode
 * Input:
 *      ledIdx - LED index id.
 *      mode  - LED force mode.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - OK
 *      RT_ERR_FAILED       - Failed
 *      RT_ERR_INPUT 		- Invalid input parameters.
 * Note:
 *      The API can get forced Led group mode.
 *      The force modes that can be set are as following:
 *      - LED_FORCE_NORMAL,
 *      - LED_FORCE_BLINK,
 *      - LED_FORCE_OFF,
 *      - LED_FORCE_ON.
 *      For LED_OP_SERIAL the max led index is 31  
 *      For LED_OP_PARALLEL the max led index is 16  
 */
int32
rtk_led_modeForce_set(uint32 ledIdx, rtk_led_force_mode_t mode)
{
    int32   ret;
    RTK_API_LOCK();
    ret = RT_MAPPER->led_modeForce_set( ledIdx, mode);
    RTK_API_UNLOCK();
    return ret;
} /* end of rtk_led_modeForce_set */


