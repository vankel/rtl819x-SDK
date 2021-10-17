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
 * Purpose : PORT SDK test case
 *
 * Feature : Provide the APIs to access PORT
 *
 */

#ifndef __DAL_PORT_TEST_CASE_H__
#define __DAL_PORT_TEST_CASE_H__


extern int32 dal_port_adminEnable_test(uint32 caseNo);
extern int32 dal_port_cpuPortId_test(uint32 caseNo);
extern int32 dal_port_enhancedFid_test(uint32 caseNo);
extern int32 dal_port_isolation_test(uint32 caseNo);
extern int32 dal_port_isolationExt_test(uint32 caseNo);
extern int32 dal_port_isolationL34_test(uint32 caseNo);
extern int32 dal_port_isolationExtL34_test(uint32 caseNo);
extern int32 dal_port_isolationEntry_test(uint32 caseNo);
extern int32 dal_port_isolationEntryExt_test(uint32 caseNo);
extern int32 dal_port_isolationCtagPktConfig_test(uint32 caseNo);
extern int32 dal_port_isolationL34PktConfig_test(uint32 caseNo);
extern int32 dal_port_linkStatus_test(uint32 caseNo);
extern int32 dal_port_macLocalLoopbackEnable_test(uint32 caseNo);
extern int32 dal_port_specialCongest_test(uint32 caseNo);
extern int32 dal_port_specialCongestStatus_test(uint32 caseNo);

#endif /* __DAL_PORT_TEST_CASE_H__ */
