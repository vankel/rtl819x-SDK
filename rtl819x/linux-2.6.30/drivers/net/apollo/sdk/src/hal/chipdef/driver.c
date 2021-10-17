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
 * $Revision: 6401 $
 * $Date: 2009-10-14 16:03:12 +0800 (星期三, 14 十月 2009) $
 *
 * Purpose : driver symbol and data type definition in the SDK.
 *
 * Feature : driver symbol and data type definition
 *
 */

/*
 * Include Files
 */
#include <hal/chipdef/driver.h>


/*
 * Data Declaration
 */

/* Definition major driver table */
rt_driver_t *rt_major_driver_table[] =
{
#if defined(CONFIG_SDK_APOLLO)
    /* RT_DRIVER_APOLLO */
    &apollo_driver,
#endif

#if defined(CONFIG_SDK_APOLLOMP)
    &apollomp_a_driver,
#endif
}; /* end of rt_major_driver_table */


/*
 * Function Declaration
 */

/* Function Name:
 *      hal_find_driver
 * Description:
 *      Find the mac major driver from SDK supported driver lists.
 * Input:
 *      driver_id     - driver chip id
 *      driver_rev_id - driver chip revision id
 * Output:
 *      None
 * Return:
 *      NULL      - Not found
 *      Otherwise - Pointer of mac driver structure that found
 * Note:
 *      The function have found the exactly driver from SDK supported driver lists.
 */
rt_driver_t *
hal_find_driver(uint32 driver_id, uint32 driver_rev_id)
{
    uint32      driver_idx;
    rt_driver_t *pMdriver;

    for (driver_idx = 0; driver_idx < RT_DRIVER_END; driver_idx++)
    {
        pMdriver = rt_major_driver_table[driver_idx];
        if (pMdriver->driver_id == driver_id &&
            pMdriver->driver_rev_id == driver_rev_id)
        {
            return rt_major_driver_table[driver_idx];
        }
    }

    return NULL;
} /* end of hal_find_driver */
