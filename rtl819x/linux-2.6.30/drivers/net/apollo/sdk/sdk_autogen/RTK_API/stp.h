/*
 * Copyright (C) 2012 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * $Revision: 10405 $
 * $Date: 2010-06-23 19:15:05 +0800 (Wed, 23 Jun 2010) $
 *
 * Purpose : Definition those public STP APIs and its data type in the SDK.
 *
 * Feature : The file have include the following module and sub-modules
 *           1) spanning tree (1D, 1w and 1s)
 *
 */

#ifndef __RTK_STP_H__
#define __RTK_STP_H__

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/stp.h>

/*
 * Symbol Definition
 */
/* spanning tree state */
typedef enum rtk_stp_state_e
{
    STP_STATE_DISABLED = 0,
    STP_STATE_BLOCKING,
    STP_STATE_LEARNING,
    STP_STATE_FORWARDING,
    STP_STATE_END
} rtk_stp_state_t;

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */

/* Module Name : STP */

/* Function Name:
 *      rtk_stp_init
 * Description:
 *      Initialize stp module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize stp module before calling any stp APIs.
 */
extern int32
rtk_stp_init(void);


/* Function Name:
 *      rtk_stp_mstpInstance_create
 * Description:
 *      Create one specified mstp instance of the specified device.
 * Input:
 *      msti - mstp instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_MSTI     - invalid msti
 * Note:
 *      None
 */
extern int32
rtk_stp_mstpInstance_create(uint32 msti);


/* Function Name:
 *      rtk_stp_mstpInstance_destroy
 * Description:
 *      Destroy one specified mstp instance from the specified device.
 * Input:
 *      msti - mstp instance
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_MSTI     - invalid msti
 * Note:
 *      None
 */
extern int32
rtk_stp_mstpInstance_destroy(uint32 msti);


/* Function Name:
 *      rtk_stp_isMstpInstanceExist_get
 * Description:
 *      Check one specified mstp instance is existing or not in the specified device.
 * Input:
 *      msti        - mstp instance
 * Output:
 *      pMstiExist - mstp instance exist or not?
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MSTI         - invalid msti
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The pMsti_exist value as following:
 *      0: this mstp instance not exist
 *      1: this mstp instance exist
 */
extern int32
rtk_stp_isMstpInstanceExist_get(uint32 msti, uint32 *pMstiExist);


/* Function Name:
 *      rtk_stp_mstpState_get
 * Description:
 *      Get port spanning tree state of the msti from the specified device.
 * Input:
 *      msti       - multiple spanning tree instance
 *      port       - port id
 * Output:
 *      pStpState - pointer buffer of spanning tree state
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_MSTI         - invalid msti
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      1. For single spanning tree mode, input CIST0 (msti=0).
 *      2. Spanning tree state as following
 *          - STP_STATE_DISABLED
 *          - STP_STATE_BLOCKING
 *          - STP_STATE_LEARNING
 *          - STP_STATE_FORWARDING
 */
extern int32
rtk_stp_mstpState_get(uint32 msti, rtk_port_t port, rtk_stp_state_t *pStpState);


/* Function Name:
 *      rtk_stp_mstpState_set
 * Description:
 *      Set port spanning tree state of the msti to the specified device.
 * Input:
 *      msti      - multiple spanning tree instance
 *      port      - port id
 *      stpState  - spanning tree state
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_MSTI       - invalid msti
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_MSTP_STATE - invalid spanning tree status
 * Note:
 *      1. For single spanning tree mode, input CIST0 (msti=0).
 *      2. Spanning tree state as following
 *          - STP_STATE_DISABLED
 *          - STP_STATE_BLOCKING
 *          - STP_STATE_LEARNING
 *          - STP_STATE_FORWARDING
 */
extern int32
rtk_stp_mstpState_set(uint32 msti, rtk_port_t port, rtk_stp_state_t stpState);

#endif /*__RTK_STP_H__*/

