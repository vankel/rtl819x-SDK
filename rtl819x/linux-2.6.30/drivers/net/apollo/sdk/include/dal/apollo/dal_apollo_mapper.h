/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * Purpose : Enterprise Switch RTK API mapper table
 *
 * Feature :
 *
 */

#ifndef __DAL_APOLLO_MAPPER_H__
#define __DAL_APOLLO_MAPPER_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <common/rt_error.h>
#include <dal/dal_mapper.h>


/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern dal_mapper_t dal_apollo_mapper;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */


/* Module Name    :  */

/* Function Name:
 *      dal_apollo_init
 * Description:
 *      Initilize DAL of enterprise switch
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_FAILED   - initialize fail
 *      RT_ERR_OK       - initialize success
 * Note:
 *      RTK must call this function before do other kind of action.
 */
extern int32
dal_apollo_init(void);

#endif  /* __DAL_APOLLO_MAPPER_H__ */
