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
 */

/*
 * Include Files
 */
#include <dal/dal_common.h>

/* Function Name:
 *      dal_common_unavail
 * Description:
 *      Unsupported function callback
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FEATURE_NOT_SUPPORTED - Feature does not support
 * Note:
 *      None.
 */
int32 dal_common_unavail(void)
{
    return RT_ERR_FEATURE_NOT_SUPPORTED;
} /* end of dal_apollomp_init */
