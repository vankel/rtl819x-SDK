/*
 * Copyright (C) 2011 Realtek Semiconductor Corp.
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated,
 * modified or distributed under the authorized license from Realtek.
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
 *
 *
 *
 * $Revision:  $
 * $Date: 2011-04-19 $
 *
 * Purpose : ACL Driver API test
 *
 * Feature : Provide the APIs to access RMA fratures
 *
 */

#ifndef __DAL_RAW_TRAP_TEST_CASE_H__
#define __DAL_RAW_TRAP_TEST_CASE_H__

extern int32 dal_raw_trap_igmp_test(uint32 caseNo);
extern int32 dal_raw_trap_igmp_BypassStorm_test(uint32 caseNo);
extern int32 dal_raw_trap_igmp_ChecksumError_test(uint32 caseNo);
extern int32 dal_raw_trap_igmp_IsolationLeaky_test(uint32 caseNo);
extern int32 dal_raw_trap_igmp_vlanLeaky_test(uint32 caseNo);

#endif /* __DAL_RAW_TRAP_TEST_CASE_H__ */
