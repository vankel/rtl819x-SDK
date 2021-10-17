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
 * $Revision: 16212 $
 * $Date: 2011-03-10 10:39:02 +0800 (¬P´e|, 10 ¤T¤鞲011) $
 *
 * Purpose : Definition of TRAP API
 *
 * Feature : The file includes the following modules and sub-modules
 *           (1) Configuration for traping packet to CPU
 *           (2) RMA
 *           (3) User defined RMA
 *           (4) System-wise management frame
 *           (5) System-wise user defined management frame
 *           (6) Per port user defined management frame
 *           (7) Packet with special flag or option
 *           (8) CFM and OAM packet
 *
 */
     
#ifndef __DAL_APOLLOMP_TRAP_H__
#define __DAL_APOLLOMP_TRAP_H__
      

/*
 * Include Files
 */
#include <common/rt_type.h>
#include <rtk/port.h>


/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */
typedef enum apollomp_igmpAction_e
{
    APOLLOMP_IGMP_ACT_FORWARD  = 0,
    APOLLOMP_IGMP_ACT_DROP,
    APOLLOMP_IGMP_ACT_TRAP,
    APOLLOMP_IGMP_ACT_END,
} apollomp_igmpAction_t;

typedef enum apollomp_l2_commonAct_e
{
	APOLLOMP_L2_COMMON_ACT_FORWARD = 0,
	APOLLOMP_L2_COMMON_ACT_DROP,
	APOLLOMP_L2_COMMON_ACT_TRAP,
	APOLLOMP_L2_COMMON_ACT_COPY2CPU,
	APOLLOMP_L2_COMMON_ACT_END,
}apollomp_l2_commonAct_t;

typedef enum apollomp_rmaAction_e
{
    APOLLOMP_RMA_ACT_FORWARD  = 0,
    APOLLOMP_RMA_ACT_TRAP ,
    APOLLOMP_RMA_ACT_DROP ,
    APOLLOMP_RMA_ACT_FORWARD_EXCLUDE_CPU ,
    APOLLOMP_RMA_ACT_END ,
} apollomp_rmaAction_t;






/*
 * Function Declaration
 */

/* Function Name:
 *      dal_apollomp_trap_init
 * Description:
 *      Initial the trap module of the specified device..
 * Input:
 *      unit - unit id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_init(void);

/* Module Name    : Trap                                    */
/* Sub-module Name: Configuration for traping packet to CPU */

/* Function Name:
 *      dal_apollomp_trap_reasonTrapToCpuPriority_get
 * Description:
 *      Get priority value of a packet that trapped to CPU port according to specific reason.
 * Input:
 *      type      - reason that trap to CPU port.
 * Output:
 *      pPriority - configured internal priority for such reason.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_IPV4IGMP
 *      - TRAP_REASON_IPV6MLD
 *      - TRAP_REASON_1XEAPOL
 *      - TRAP_REASON_VLANERR
 *      - TRAP_REASON_SLPCHANGE
 *      - TRAP_REASON_MULTICASTDLF
 *      - TRAP_REASON_CFI
 *      - TRAP_REASON_1XUNAUTH
 */
extern int32
dal_apollomp_trap_reasonTrapToCpuPriority_get(rtk_trap_reason_type_t type, rtk_pri_t *pPriority);

/* Function Name:
 *      dal_apollomp_trap_reasonTrapToCpuPriority_set
 * Description:
 *      Set priority value of a packet that trapped to CPU port according to specific reason.
 * Input:
 *      type     - reason that trap to CPU port.
 *      priority - internal priority that is going to be set for specific trap reason.
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_INPUT    - Invalid input parameter
 * Note:
 *      Currently the trap reason that supported are listed as follows:
 *      - TRAP_REASON_RMA
 *      - TRAP_REASON_IPV4IGMP
 *      - TRAP_REASON_IPV6MLD
 *      - TRAP_REASON_1XEAPOL
 *      - TRAP_REASON_VLANERR
 *      - TRAP_REASON_SLPCHANGE
 *      - TRAP_REASON_MULTICASTDLF
 *      - TRAP_REASON_CFI
 *      - TRAP_REASON_1XUNAUTH
 */
extern int32
dal_apollomp_trap_reasonTrapToCpuPriority_set(rtk_trap_reason_type_t type, rtk_pri_t priority);

/* Function Name:
 *      dal_apollomp_trap_igmpCtrlPkt2CpuEnable_get
 * Description:
 *      Get the configuration about whether IGMP control packets need be trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of IGMP control packet trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of IGMP control packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_apollomp_trap_igmpCtrlPkt2CpuEnable_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trap_igmpCtrlPkt2CpuEnable_set
 * Description:
 *      Set the configuration about whether IGMP control packets need be trapped to CPU.
 * Input:
 *      unit   - unit id
 *      enable - status of IGMP control packet trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of IGMP control packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_apollomp_trap_igmpCtrlPkt2CpuEnable_set(rtk_enable_t enable);

/* Function Name:
 *      dal_apollomp_trap_mldCtrlPkt2CpuEnable_get
 * Description:
 *      Get the configuration about whether MLD control packets need be trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of MLD control packet trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of MLD control packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_apollomp_trap_mldCtrlPkt2CpuEnable_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trap_mldCtrlPkt2CpuEnable_set
 * Description:
 *      Set the configuration about whether MLD control packets need be trapped to CPU.
 * Input:
 *      enable - status of MLD control packet trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of MLD control packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_apollomp_trap_mldCtrlPkt2CpuEnable_set(rtk_enable_t enable);

/* Function Name:
 *      dal_apollomp_trap_ipMcastPkt2CpuEnable_get
 * Description:
 *      Get the configuration about whether IP multicast packet lookup miss need be trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of IP multicast packet trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of IP multicast packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_apollomp_trap_ipMcastPkt2CpuEnable_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trap_ipMcastPkt2CpuEnable_set
 * Description:
 *      Set the configuration about whether IP multicast packet lookup miss need be trapped to CPU.
 * Input:
 *      enable - status of IP multicast packet trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of IP multicast packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_apollomp_trap_ipMcastPkt2CpuEnable_set(rtk_enable_t enable);

/* Function Name:
 *      dal_apollomp_trap_l2McastPkt2CpuEnable_get
 * Description:
 *      Get the configuration about whether L2 multicast packets lookup miss need be trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pEnable - status of L2 multicast packet trap to CPU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID      - Invalid unit id
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      The status of L2 multicast packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_apollomp_trap_l2McastPkt2CpuEnable_get(rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trap_l2McastPkt2CpuEnable_set
 * Description:
 *      Set the configuration about whether L2 multicast packets lookup miss need be trapped to CPU.
 * Input:
 *      enable - status of L2 multicast packet trap to CPU
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_UNIT_ID - Invalid unit id
 *      RT_ERR_INPUT   - Invalid input parameter
 * Note:
 *      The status of L2 multicast packet trap to CPU:
 *      - DISABLED
 *      - ENABLED
 */
extern int32
dal_apollomp_trap_l2McastPkt2CpuEnable_set(rtk_enable_t enable);

/* Module Name    : Trap     */
/* Sub-module Name: RMA      */

/* Function Name:
 *      dal_apollomp_trap_rmaAction_get
 * Description:
 *      Get action of reserved multicast address(RMA) frame.
 * Input:
 *      pRmaFrame  - Reserved multicast address.
 * Output:
 *      pRmaAction - RMA action
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT        - Invalid input parameter
 *      RT_ERR_NULL_POINTER - NULL pointer
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaAction_get(rtk_mac_t *pRmaFrame, rtk_trap_rma_action_t *pRmaAction);

/* Function Name:
 *      dal_apollomp_trap_rmaAction_set
 * Description:
 *      Set action of reserved multicast address(RMA) frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      rmaAction - RMA action
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_INPUT      - Invalid input parameter
 *      RT_ERR_RMA_ACTION - Invalid RMA action
 * Note:
 *      The supported Reserved Multicast Address frame:
 *      Assignment                                                                  Address
 *      RMA_BRG_GROUP (Bridge Group Address)                                        01-80-C2-00-00-00
 *      RMA_FD_PAUSE (IEEE Std 802.3, 1988 Edition, Full Duplex PAUSE operation)    01-80-C2-00-00-01
 *      RMA_SP_MCAST (IEEE Std 802.3ad Slow Protocols-Multicast address)            01-80-C2-00-00-02
 *      RMA_1X_PAE (IEEE Std 802.1X PAE address)                                    01-80-C2-00-00-03
 *      RMA_RESERVED04 (Reserved)                                                   01-80-C2-00-00-04
 *      RMA_MEDIA_ACCESS_USE (Media Access Method Specific Use)                     01-80-C2-00-00-05
 *      RMA_RESERVED06 (Reserved)                                                   01-80-C2-00-00-06
 *      RMA_RESERVED07 (Reserved)                                                   01-80-C2-00-00-07
 *      RMA_PVD_BRG_GROUP (Provider Bridge Group Address)                           01-80-C2-00-00-08
 *      RMA_RESERVED09 (Reserved)                                                   01-80-C2-00-00-09
 *      RMA_RESERVED0A (Reserved)                                                   01-80-C2-00-00-0A
 *      RMA_RESERVED0B (Reserved)                                                   01-80-C2-00-00-0B
 *      RMA_RESERVED0C (Reserved)                                                   01-80-C2-00-00-0C
 *      RMA_MVRP (Provider Bridge MVRP Address)                                     01-80-C2-00-00-0D
 *      RMA_1ab_LL_DISCOVERY (802.1ab Link Layer Discover Protocol Address)         01-80-C2-00-00-0E
 *      RMA_RESERVED0F (Reserved)                                                   01-80-C2-00-00-0F
 *      RMA_BRG_MNGEMENT (All LANs Bridge Management Group Address)                 01-80-C2-00-00-10
 *      RMA_LOAD_SERV_GENERIC_ADDR (Load Server Generic Address)                    01-80-C2-00-00-11
 *      RMA_LOAD_DEV_GENERIC_ADDR (Loadable Device Generic Address)                 01-80-C2-00-00-12
 *      RMA_RESERVED13 (Reserved)                                                   01-80-C2-00-00-13
 *      RMA_RESERVED14 (Reserved)                                                   01-80-C2-00-00-14
 *      RMA_RESERVED15 (Reserved)                                                   01-80-C2-00-00-15
 *      RMA_RESERVED16 (Reserved)                                                   01-80-C2-00-00-16
 *      RMA_RESERVED17 (Reserved)                                                   01-80-C2-00-00-17
 *      RMA_MANAGER_STA_GENERIC_ADDR (Generic Address for All Manager Stations)     01-80-C2-00-00-18
 *      RMA_RESERVED19 (Reserved)                                                   01-80-C2-00-00-19
 *      RMA_AGENT_STA_GENERIC_ADDR (Generic Address for All Agent Stations)         01-80-C2-00-00-1A
 *      RMA_RESERVED1B (Reserved)                                                   01-80-C2-00-00-1B
 *      RMA_RESERVED1C (Reserved)                                                   01-80-C2-00-00-1C
 *      RMA_RESERVED1D (Reserved)                                                   01-80-C2-00-00-1D
 *      RMA_RESERVED1E (Reserved)                                                   01-80-C2-00-00-1E
 *      RMA_RESERVED1F (Reserved)                                                   01-80-C2-00-00-1F
 *      RMA_GMRP (GMRP Address)                                                     01-80-C2-00-00-20
 *      RMA_GVRP (GVRP address)                                                     01-80-C2-00-00-21
 *      RMA_UNDEF_GARP22~2F (Undefined GARP address)                                01-80-C2-00-00-22
 *                                                                                ~ 01-80-C2-00-00-2F
 *      CDP                                                                         01-00-0C-CC-CC-CC
 *      CDP                                                                         01-00-0C-CC-CC-CD
 *
 *      The supported Reserved Multicast Address action:
 *      - RMA_ACTION_FORWARD
 *      - RMA_ACTION_DROP
 *      - RMA_ACTION_TRAP2CPU
 */
extern int32
dal_apollomp_trap_rmaAction_set(rtk_mac_t *pRmaFrame, rtk_trap_rma_action_t rmaAction);

/* Function Name:
 *      dal_apollomp_trap_rmaPri_get
 * Description:
 *      Get priority of packets trapped to CPU.
 * Input:
 *      None.
 * Output:
 *      pPriority  - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaPri_get(rtk_pri_t *pPriority);

/* Function Name:
 *      dal_apollomp_trap_rmaPri_set
 * Description:
 *      Set priority of packets trapped to CPU.
 * Input:
 *      priority   - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaPri_set(rtk_pri_t priority);

/* Function Name:
 *      dal_apollomp_trap_rmaVlanCheckEnable_get
 * Description:
 *      Get enable status of vlan checking on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Applicable:
 *      8328
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaVlanCheckEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trap_rmaVlanCheckEnable_set
 * Description:
 *      Set enable status of vlan checking on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaVlanCheckEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable);

/* Function Name:
 *      dal_apollomp_trap_rmaPortIsolationEnable_get
 * Description:
 *      Get enable status of port isolation on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaPortIsolationEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trap_rmaPortIsolationEnable_set
 * Description:
 *      Set enable status of port isolation on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaPortIsolationEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable);

/* Function Name:
 *      dal_apollomp_trap_rmaStormControlEnable_get
 * Description:
 *      Get enable status of storm control on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaStormControlEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trap_rmaStormControlEnable_set
 * Description:
 *      Set enable status of storm control on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaStormControlEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable);

/* Function Name:
 *      dal_apollomp_trap_rmaKeepCtagEnable_get
 * Description:
 *      Get enable status of keeping ctag format on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 * Output:
 *      pEnable    - pointer to enable status of vlan checking
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_RMA_ADDR     - invalid invalid RMA address
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaKeepCtagEnable_get(rtk_mac_t *pRmaFrame, rtk_enable_t *pEnable);

/* Function Name:
 *      dal_apollomp_trap_rmaKeepCtagEnable_set
 * Description:
 *      Set enable status of keeping ctag format on specified RMA frame.
 * Input:
 *      pRmaFrame - Reserved multicast address.
 *      enable     - enable status of vlan checking
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_RMA_ADDR - invalid invalid RMA address
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_rmaKeepCtagEnable_set(rtk_mac_t *pRmaFrame, rtk_enable_t enable);


/* Module Name    : Trap       */
/* Sub-module Name: OAM packet */

/* Function Name:
 *      dal_apollomp_trap_oamPduAction_get
 * Description:
 *      Get forwarding action of trapped oam PDU on specified port.
 * Input:
 *      None.
 * Output:
 *      pAction - pointer to forwarding action of trapped oam PDU
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_apollomp_trap_oamPduAction_get(rtk_action_t *pAction);

/* Function Name:
 *      dal_apollomp_trap_oamPduAction_set
 * Description:
 *      Set forwarding action of trapped oam PDU on specified port.
 * Input:
 *      action - forwarding action of trapped oam PDU
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT   - The module is not initial
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_FWD_ACTION - invalid forwarding action
 * Note:
 *      Forwarding action is as following
 *      - ACTION_FORWARD
 *      - ACTION_TRAP2CPU
 */
extern int32
dal_apollomp_trap_oamPduAction_set(rtk_action_t action);

/* Function Name:
 *      dal_apollomp_trap_oamPduPri_get
 * Description:
 *      Get priority of trapped OAM PDU.
 * Input:
 *      None.
 * Output:
 *      pPriority - pointer to priority
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_oamPduPri_get(rtk_pri_t *pPriority);

/* Function Name:
 *      dal_apollomp_trap_oamPduPri_set
 * Description:
 *      Set priority of trapped OAM PDU.
 * Input:
 *      priority - priority
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PRIORITY - invalid priority value
 * Note:
 *      None
 */
extern int32
dal_apollomp_trap_oamPduPri_set(rtk_pri_t priority);


#endif /* __DAL_APOLLOMP_TRAP_H__ */
