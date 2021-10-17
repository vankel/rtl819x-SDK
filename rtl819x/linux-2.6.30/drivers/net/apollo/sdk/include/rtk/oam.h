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
 * $Revision: 10372 $
 * $Date: 2010-06-22 20:39:54 +0800 (?��?�? 22 ?��? 2010) $
 *
 * Purpose : Definition of SVLAN API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) OAM (802.3ah) configuration
 *
 */


#ifndef __RTK_OAM_H__
#define __RTK_OAM_H__


/*
 * Include Files
 */
#include <common/rt_type.h>



/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */


/*
 * Macro Declaration
 */

typedef enum rtk_oam_parser_act_e
{
    OAM_PARSER_ACTION_FORWARD = 0,
    OAM_PARSER_ACTION_LOOPBACK,
    OAM_PARSER_ACTION_DISCARD,
    OAM_PARSER_ACTION_END,

} rtk_oam_parser_act_t;

typedef enum rtk_oam_multiplexer_act_e
{
    OAM_MULTIPLEXER_ACTION_FORWARD = 0,
    OAM_MULTIPLEXER_ACTION_DISCARD,
    OAM_MULTIPLEXER_ACTION_CPUONLY,
    OAM_MULTIPLEXER_ACTION_END,

} rtk_oam_multiplexer_act_t;


/*
 * Function Declaration
 */
 
/* Function Name:
 *      rtk_oam_init
 * Description:
 *      Initialize oam module.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Must initialize oam module before calling any oam APIs.
 */
extern int32
rtk_oam_init(void);


/* Module Name : OAM */

/* Function Name:
 *      rtk_oam_parserAction_set
 * Description:
 *      Set OAM parser action
 * Input:
 *      port    - port id
 *      action  - parser action 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
rtk_oam_parserAction_set(rtk_port_t port, rtk_oam_parser_act_t action);

/* Function Name:
 *      rtk_oam_parserAction_set
 * Description:
 *      Get OAM parser action
 * Input:
 *      port    - port id
 * Output:
 *      pAction  - parser action 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
rtk_oam_parserAction_get(rtk_port_t port, rtk_oam_parser_act_t *pAction);


/* Function Name:
 *      rtk_oam_multiplexerAction_set
 * Description:
 *      Set OAM multiplexer action
 * Input:
 *      port    - port id
 *      action  - parser action 
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
rtk_oam_multiplexerAction_set(rtk_port_t port, rtk_oam_multiplexer_act_t action);

/* Function Name:
 *      rtk_oam_multiplexerAction_set
 * Description:
 *      Get OAM multiplexer action
 * Input:
 *      port    - port id
 * Output:
 *      pAction  - parser action 
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
extern int32
rtk_oam_multiplexerAction_get(rtk_port_t port, rtk_oam_multiplexer_act_t *pAction);


#endif /* __RTK_OAM_H__ */
