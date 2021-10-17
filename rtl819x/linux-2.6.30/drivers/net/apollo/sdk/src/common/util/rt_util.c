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
 * Purpose : Define the utility macro and function in the SDK.
 *
 * Feature : SDK common utility
 *
 */

/*  
 * Include Files 
 */
#include <common/util/rt_util.h>
#include <common/rt_error.h>


/* 
 * Symbol Definition 
 */


/* 
 * Data Declaration 
 */

/*
 * Macro Definition
 */

/* 
 * Function Declaration 
 */
/* Function Name:
 *      rt_util_macCmp
 * Description:
 *      Compare two mac address
 * Input:
 *      mac1    - mac address 1
 *      mac2    - mac address 2
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK           - two address is same
 *      RT_ERR_FAILED       - two address is different
 * Note:
 */
int32
rt_util_macCmp(const uint8 *mac1, const uint8 *mac2)
{
    if (memcmp(mac1, mac2, ETHER_ADDR_LEN) == 0)
        return RT_ERR_OK;
    else
        return RT_ERR_FAILED;

}
