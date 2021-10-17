/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision: 14257 $
 * $Date: 2010-11-17 16:08:38 +0800 (Wed, 17 Nov 2010) $
 *
 * Purpose : Definition of HAL API test APIs in the SDK
 *
 * Feature : HAL API test APIs
 *
 */

#ifndef __HAL_REG_TEST_CASE_H__
#define __HAL_REG_TEST_CASE_H__

/*
 * Include Files
 */
#include <common/rt_type.h>


/*
 * Function Declaration
 */

extern int32
hal_reg_def_test(uint32 testcase);

extern int32
hal_reg_rw_test(uint32 testcase);


#endif  /* __HAL_REG_TEST_CASE_H__ */
