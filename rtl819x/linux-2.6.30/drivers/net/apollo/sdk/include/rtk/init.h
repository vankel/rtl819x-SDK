/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 * $Revision: 7206 $
 * $Date: 2009-11-23 19:04:36 +0800 (星期一, 23 十一月 2009) $
 *
 * Purpose : Definition of Init API
 *
 * Feature : Initialize All Layers of RTK Module
 *
 */

#ifndef __RTK_INIT_H__
#define __RTK_INIT_H__

/*
 * Include Files
 */
#include <common/rt_type.h>

/*
 * Symbol Definition
 */


#define RTK_API_LOCK()
#define RTK_API_UNLOCK()

/*
 * Data Declaration
 */


/*
 * Function Declaration
 */

/* Function Name:
 *      rtk_init
 * Description:
 *      Initialize the specified device
 * Input:
 *      unit - unit id
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - invalid unit id
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
extern int32
rtk_init(void);

/* Function Name:
 *      rtk_deinit
 * Description:
 *      De-Initialize the driver, release irq
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_deinit(void);


extern int32
rtk_core_init(void);

extern int32
rtk_all_module_init(void);


#endif /* __RTK_INIT_H__ */
