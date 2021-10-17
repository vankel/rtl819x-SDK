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
 * $Revision: 23355 $
 * $Date: 2011-09-27 18:33:58 +0800 (?üÊ?‰∫? 27 ‰πùÊ? 2011) $
 *
 * Purpose : RTK switch high-level API for RTL8370/RTL8367
 * Feature : Here is a list of all functions and variables in this module.
 *
 */

#include <rtl8370_asicdrv_acl.h>
#include <rtl8370_asicdrv.h>
#include <rtl8370_asicdrv_dot1x.h>
#include <rtl8370_asicdrv_qos.h>
#include <rtl8370_asicdrv_scheduling.h>
#include <rtl8370_asicdrv_fc.h>
#include <rtl8370_asicdrv_port.h>
#include <rtl8370_asicdrv_phy.h>
#include <rtl8370_asicdrv_igmp.h>
#include <rtl8370_asicdrv_unknownMulticast.h>
#include <rtl8370_asicdrv_rma.h>
#include <rtl8370_asicdrv_vlan.h>
#include <rtl8370_asicdrv_lut.h>
#include <rtl8370_asicdrv_led.h>
#include <rtl8370_asicdrv_svlan.h>
#include <rtl8370_asicdrv_meter.h>
#include <rtl8370_asicdrv_inbwctrl.h>
#include <rtl8370_asicdrv_storm.h>
#include <rtl8370_asicdrv_misc.h>
#include <rtl8370_asicdrv_portIsolation.h>
#include <rtl8370_asicdrv_cputag.h>
#include <rtl8370_asicdrv_trunking.h>
#include <rtl8370_asicdrv_mirror.h>
#include <rtl8370_asicdrv_mib.h>
#include <rtl8370_asicdrv_interrupt.h>

#include <rtk_api.h>

#include <rtk_api_ext.h>
#include <rtk_error.h>
  
#define DELAY_800MS_FOR_CHIP_STATABLE() {  }

CONST_T uint32 filter_templateField[RTK_MAX_NUM_OF_FILTER_TYPE][RTK_MAX_NUM_OF_FILTER_FIELD] = {
    {DMAC2, DMAC1, DMAC0, SMAC2, SMAC1, SMAC0, ETHERTYPE},
    {IP4SIP1, IP4SIP0, IP4DIP1, IP4DIP0, IP4FLAGOFF, IP4TOSPROTO, CTAG},
    {IP6SIP7, IP6SIP6, IP6SIP4, IP6SIP3, IP6SIP2, IP6SIP1, IP6SIP0},
    {IP6DIP7, IP6DIP6, IP6DIP4, IP6DIP3, IP6DIP2, IP6DIP1, IP6DIP0},
    {TCPSPORT, TCPDPORT, TCPFLAG, ICMPCODETYPE, IGMPTYPE, TOSNH, STAG}
};

typedef enum rtk_filter_data_type_e
{
    RTK_FILTER_DATA_MAC = 0,
    RTK_FILTER_DATA_UINT16,
    RTK_FILTER_DATA_TAG,
    RTK_FILTER_DATA_IPV4,
    RTK_FILTER_DATA_UINT8_HIGH,    
    RTK_FILTER_DATA_UINT8_LOW,
    RTK_FILTER_DATA_IPV4FLAG,
    RTK_FILTER_DATA_UINT13_LOW,
    RTK_FILTER_DATA_TCPFLAG,
    RTK_FILTER_DATA_IPV6,
} rtk_filter_data_type_t;

static rtk_api_ret_t _rtk_filter_igrAcl_writeDataField(rtl8370_acl_rule_t *aclRule, uint32 *tempIdx, uint32 *fieldIdx, rtk_filter_field_t *fieldPtr, rtk_filter_data_type_t type);
static rtk_api_ret_t _rtk_switch_init0(void);
static rtk_api_ret_t _rtk_switch_init1(void);
static rtk_api_ret_t _rtk_switch_init2(void);



/* Function Name:
 *      rtk_port_macForceLinkExt0_set
 * Description:
 *      Set external interface 0 force linking configuration.
 * Input:
 *      mode - external interface mode
 *      pPortability - port ability configuration
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 * Note:
 *      This API can set external interface 0 force mode properties. 
 *      The external interface can be set to:
 *      MODE_EXT_DISABLE,
 *      MODE_EXT_RGMII,
 *      MODE_EXT_MII_MAC,
 *      MODE_EXT_MII_PHY, 
 *      MODE_EXT_TMII_MAC,
 *      MODE_EXT_TMII_PHY, 
 *      MODE_EXT_GMII, 
 *      MODE_EXT_RGMII_33V,
 */
rtk_api_ret_t rtk_port_macForceLinkExt0_set(rtk_mode_ext_t mode, rtk_port_mac_ability_t *pPortability)
{
    rtk_api_ret_t retVal;
    rtl8370_port_ability_t ability;

    if (mode >=MODE_EXT_END)
        return RT_ERR_INPUT;    

    if (pPortability->forcemode>1||pPortability->speed>2||pPortability->duplex>1||
       pPortability->link>1||pPortability->nway>1||pPortability->txpause>1||pPortability->rxpause>1)
        return RT_ERR_INPUT;

    if ((retVal = rtl8370_setAsicPortExtMode(RTK_EXT_0, mode))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_getAsicPortForceLinkExt(RTK_EXT_0,&ability))!=RT_ERR_OK)
        return retVal;
     
    ability.forcemode = pPortability->forcemode;
    ability.speed     = pPortability->speed;
    ability.duplex    = pPortability->duplex;
    ability.link      = pPortability->link;
    ability.nway      = pPortability->nway;
    ability.txpause   = pPortability->txpause;
    ability.rxpause   = pPortability->rxpause;

    if ((retVal = rtl8370_setAsicPortForceLinkExt(RTK_EXT_0,&ability))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}




/* Function Name:
 *      rtk_port_macForceLinkExt1_set
 * Description:
 *      Set external interface 1 force linking configuration.
 * Input:
 *      mode - external interface mode
 *      pPortability - port ability configuration
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 * Note:
 *      This API can set external interface 1 force mode properties. 
 *      The external interface can be set to:
 *      MODE_EXT_DISABLE,
 *      MODE_EXT_RGMII,
 *      MODE_EXT_MII_MAC,
 *      MODE_EXT_MII_PHY, 
 *      MODE_EXT_TMII_MAC,
 *      MODE_EXT_TMII_PHY, 
 *      MODE_EXT_GMII, 
 *      MODE_EXT_RGMII_33V,
 */
rtk_api_ret_t rtk_port_macForceLinkExt1_set(rtk_mode_ext_t mode, rtk_port_mac_ability_t *pPortability)
{
    rtk_api_ret_t retVal;
    rtl8370_port_ability_t ability;

    if (mode >=MODE_EXT_END)
        return RT_ERR_INPUT;

    if (pPortability->forcemode>1||pPortability->speed>2||pPortability->duplex>1||
       pPortability->link>1||pPortability->nway>1||pPortability->txpause>1||pPortability->rxpause>1)
        return RT_ERR_INPUT;

    if ((retVal = rtl8370_setAsicPortExtMode(RTK_EXT_1, mode))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_getAsicPortForceLinkExt(RTK_EXT_1,&ability))!=RT_ERR_OK)
        return retVal;
     
    ability.forcemode = pPortability->forcemode;
    ability.speed     = pPortability->speed;
    ability.duplex    = pPortability->duplex;
    ability.link      = pPortability->link;
    ability.nway      = pPortability->nway;
    ability.txpause   = pPortability->txpause;
    ability.rxpause   = pPortability->rxpause;

    if ((retVal = rtl8370_setAsicPortForceLinkExt(RTK_EXT_1,&ability))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}




/* Function Name:
 *      rtk_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port.
 * Input:
 *      port - port id.
 *      reg - Register id
 *      regData - Register data
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 *      RT_ERR_PHY_REG_ID - Invalid PHY address
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      This API can set PHY register data of the specific port.
 */
rtk_api_ret_t rtk_port_phyReg_set(rtk_port_t port, rtk_port_phy_reg_t reg, rtk_port_phy_data_t regData)
{
    rtk_api_ret_t retVal;
    
    if (port > RTK_PHY_ID_MAX)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8370_setAsicPHYReg(port,PHY_PAGE_ADDRESS,0))!=RT_ERR_OK)
        return retVal; 	

    if (reg==0&&((regData&0x1000)>0)&&((regData&0x0800)>0))
        regData = regData | 0x0200;

    if ((retVal = rtl8370_setAsicPHYReg(port,reg,regData))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_setAsicPHYReg(port,PHY_PAGE_ADDRESS,0))!=RT_ERR_OK)
        return retVal; 	
   
    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port.
 * Input:
 *      port - Port id.
 *      reg - Register id
 * Output:
 *      pData - Register data
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_PORT_ID - Invalid port number.
 *      RT_ERR_PHY_REG_ID - Invalid PHY address
 *      RT_ERR_BUSYWAIT_TIMEOUT - PHY access busy
 * Note:
 *      This API can get PHY register data of the specific port. 
 */
rtk_api_ret_t rtk_port_phyReg_get(rtk_port_t port, rtk_port_phy_reg_t reg, rtk_port_phy_data_t *pData) 
{
    rtk_api_ret_t retVal;

    if (port > RTK_PHY_ID_MAX)
        return RT_ERR_PORT_ID;

    if ((retVal = rtl8370_setAsicPHYReg(port,PHY_PAGE_ADDRESS,0))!=RT_ERR_OK)
        return retVal; 

    if ((retVal = rtl8370_getAsicPHYReg(port,reg,pData))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_setAsicPHYReg(port,PHY_PAGE_ADDRESS,0))!=RT_ERR_OK)
        return retVal; 

    return RT_ERR_OK;
}




/* Function Name:
 *      rtk_port_rgmiiDelayExt0_set
 * Description:
 *      Set RGMII interface 0 delay value for TX and RX.
 * Input:
 *      txDelay - TX delay value, 1 for delay 2ns and 0 for no-delay
 *      rxDelay - RX delay value, 0~7 for delay setup.
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 * Note:
 *      This API can set external interface 0 RGMII delay. 
 *      In TX delay, there are 2 selection: no-delay and 2ns delay.
 *      In RX dekay, there are 8 steps for delay tunning. 0 for no-delay, and 7 for maximum delay.  
 */
rtk_api_ret_t rtk_port_rgmiiDelayExt0_set(rtk_data_t txDelay, rtk_data_t rxDelay)
{
    rtk_api_ret_t retVal;
    uint32 regData;

    if ((txDelay > 1)||(rxDelay > 7))
        return RT_ERR_INPUT;
    
    if ((retVal = rtl8370_getAsicReg(RTL8370_REG_EXT0_RGMXF, &regData))!=RT_ERR_OK)
        return retVal;

    regData = (regData&0xFFF0) | ((txDelay<<3)&0x0008) | (rxDelay&0x007);

    if ((retVal = rtl8370_setAsicReg(RTL8370_REG_EXT0_RGMXF, regData))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}



/* Function Name:
 *      rtk_port_rgmiiDelayExt1_set
 * Description:
 *      Set RGMII interface 1 delay value for TX and RX.
 * Input:
 *      txDelay - TX delay value, 1 for delay 2ns and 0 for no-delay
 *      rxDelay - RX delay value, 0~7 for delay setup.
 * Output:
 *      None 
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 * Note:
 *      This API can set external interface 1 RGMII delay. 
 *      In TX delay, there are 2 selection: no-delay and 2ns delay.
 *      In RX dekay, there are 8 steps for delay tunning. 0 for n0-delay, and 7 for maximum delay.  
 */
rtk_api_ret_t rtk_port_rgmiiDelayExt1_set(rtk_data_t txDelay, rtk_data_t rxDelay)
{
    rtk_api_ret_t retVal;
    uint32 regData;

    if ((txDelay > 1)||(rxDelay > 7))
        return RT_ERR_INPUT;
    
    if ((retVal = rtl8370_getAsicReg(RTL8370_REG_EXT1_RGMXF, &regData))!=RT_ERR_OK)
        return retVal;

    regData = (regData&0xFFF0) | ((txDelay<<3)&0x0008) | (rxDelay&0x007);

    if ((retVal = rtl8370_setAsicReg(RTL8370_REG_EXT1_RGMXF, regData))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}




/* Function Name:
 *      rtk_cpu_enable_set
 * Description:
 *      Set CPU port function enable/disable.
 * Input:
 *      enable - CPU port function enable
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_ENABLE - Invalid enable parameter.
 * Note:
 *      The API can set CPU port function enable/disable. 
 */
rtk_api_ret_t rtk_cpu_enable_set(rtk_enable_t enable)
{
    rtk_api_ret_t retVal;

    if (enable >=RTK_ENABLE_END)
        return RT_ERR_ENABLE;

    if ((retVal = rtl8370_setAsicCputagEnable(enable))!=RT_ERR_OK)
        return retVal;

    if (DISABLED == enable)
    {
        if ((retVal = rtl8370_setAsicCputagPortmask(0))!=RT_ERR_OK)
            return retVal;
    }    

    return RT_ERR_OK;
}



/* Function Name:
 *      rtk_cpu_tagPort_set
 * Description:
 *      Set CPU port and CPU tag insert mode.
 * Input:
 *      port - Port id.
 *      mode - CPU tag insert for packets egress from CPU port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameter.
 *      RT_ERR_PORT_ID - Invalid port number.
 * Note:
 *      The API can set CPU port and inserting proprietary CPU tag mode (Length/Type 0x8899)
 *      to the frame that transmitting to CPU port.
 *      The inset cpu tag mode is as following:
 *      CPU_INSERT_TO_ALL
 *      CPU_INSERT_TO_TRAPPING
 *      CPU_INSERT_TO_NONE   
 */
rtk_api_ret_t rtk_cpu_tagPort_set(rtk_port_t port, rtk_cpu_insert_t mode)
{
    rtk_api_ret_t retVal;

    if (port > RTK_PORT_ID_MAX)
        return RT_ERR_INPUT;

    if (mode >= CPU_INSERT_END)
        return RT_ERR_INPUT;
    
    if ((retVal = rtl8370_setAsicCputagPortmask(1<<port))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_setAsicCputagTrapPort(port))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_setAsicCputagInsertMode(mode))!=RT_ERR_OK)
        return retVal;
        
    return RT_ERR_OK;
}

#if 0
static rtk_api_ret_t _rtk_switch_init0(void)
{
    rtk_api_ret_t retVal;
    uint32 index,regData;
    uint32 busyFlag,cnt;
    CONST_T uint16 chipData0[][2] = {{0x133f,0x0030},{0x133e,0x000e},{0x221f,0x0000},{0x2215,0x1006},
                                     {0x221f,0x0005},{0x2200,0x00c6},{0x221f,0x0007},{0x221e,0x0048},
                                     {0x2215,0x6412},{0x2216,0x6412},{0x2217,0x6412},{0x2218,0x6412},
                                     {0x2219,0x6412},{0x221A,0x6412},{0x221f,0x0001},{0x220c,0xdbf0},
                                     {0x2209,0x2576},{0x2207,0x287E},{0x220A,0x68E5},{0x221D,0x3DA4},
                                     {0x221C,0xE7F7},{0x2214,0x7F52},{0x2218,0x7FCE},{0x2208,0x04B7},
                                     {0x2206,0x4072},{0x2210,0xF05E},{0x221B,0xB414},{0x221F,0x0003},
                                     {0x221A,0x06A6},{0x2210,0xF05E},{0x2213,0x06EB},{0x2212,0xF4D2},
                                     {0x220E,0xE120},{0x2200,0x7C00},{0x2202,0x5FD0},{0x220D,0x0207},
                                     {0x221f,0x0002},{0x2205,0x0978},{0x2202,0x8C01},{0x2207,0x3620},
                                     {0x221C,0x0001},{0x2203,0x0420},{0x2204,0x80C8},{0x133e,0x0ede},
                                     {0x221f,0x0002},{0x220c,0x0073},{0x220d,0xEB65},{0x220e,0x51d1},
                                     {0x220f,0x5dcb},{0x2210,0x3044},{0x2211,0x1800},{0x2212,0x7E00},
                                     {0x2213,0x0000},{0x133f,0x0010},{0x133e,0x0ffe},{0x207f,0x0002},
                                     {0x2074,0x3D22},{0x2075,0x2000},{0x2076,0x6040},{0x2077,0x0000},
                                     {0x2078,0x0f0a},{0x2079,0x50AB},{0x207a,0x0000},{0x207b,0x0f0f},
                                     {0x205f,0x0002},{0x2054,0xFF00},{0x2055,0x000A},{0x2056,0x000A},
                                     {0x2057,0x0005},{0x2058,0x0005},{0x2059,0x0000},{0x205A,0x0005},
                                     {0x205B,0x0005},{0x205C,0x0005},{0x209f,0x0002},{0x2094,0x00AA},
                                     {0x2095,0x00AA},{0x2096,0x00AA},{0x2097,0x00AA},{0x2098,0x0055},
                                     {0x2099,0x00AA},{0x209A,0x00AA},{0x209B,0x00AA},{0x1363,0x8354},
                                     {0x1270,0x3333},{0x1271,0x3333},{0x1272,0x3333},{0x1330,0x00DB},
                                     {0x1203,0xff00},{0x1200,0x7fc4},{0x121d,0x1006},{0x121e,0x03e8},
                                     {0x121f,0x02b3},{0x1220,0x028f},{0x1221,0x029b},{0x1222,0x0277},
                                     {0x1223,0x02b3},{0x1224,0x028f},{0x1225,0x029b},{0x1226,0x0277},
                                     {0x1227,0x00c0},{0x1228,0x00b4},{0x122f,0x00c0},{0x1230,0x00b4},
                                     {0x1229,0x0020},{0x122a,0x000c},{0x1231,0x0030},{0x1232,0x0024},
                                     {0x0219,0x0032},{0x0200,0x03e8},{0x0201,0x03e8},{0x0202,0x03e8},
                                     {0x0203,0x03e8},{0x0204,0x03e8},{0x0205,0x03e8},{0x0206,0x03e8},
                                     {0x0207,0x03e8},{0x0218,0x0032},{0x0208,0x029b},{0x0209,0x029b},
                                     {0x020a,0x029b},{0x020b,0x029b},{0x020c,0x029b},{0x020d,0x029b},
                                     {0x020e,0x029b},{0x020f,0x029b},{0x0210,0x029b},{0x0211,0x029b},
                                     {0x0212,0x029b},{0x0213,0x029b},{0x0214,0x029b},{0x0215,0x029b},
                                     {0x0216,0x029b},{0x0217,0x029b},{0x0900,0x0000},{0x0901,0x0000},
                                     {0x0902,0x0000},{0x0903,0x0000},{0x0865,0x3210},{0x087b,0x0000},
                                     {0x087c,0xff00},{0x087d,0x0000},{0x087e,0x0000},{0x0801,0x0100},
                                     {0x0802,0x0100},{0x1700,0x014C},{0x0301,0x00FF},{0x12AA,0x0096},
                                     {0x133f,0x0030},{0x133e,0x000e},{0x221f,0x0005},{0x2200,0x00C4},
                                     {0x221f,0x0000},{0x2210,0x05EF},{0x2204,0x05E1},{0x2200,0x1340},
                                     {0x133f,0x0010},{0x20A0,0x1940},{0x20C0,0x1940},{0x20E0,0x1940},
                                     {0xFFFF, 0xABCD}};
 
    CONST_T uint16 chipData1[][2] = {{0x133f,0x0030},{0x133e,0x000e},{0x221f,0x0000},{0x2215,0x1006}, 
                                     {0x221f,0x0005},{0x2200,0x00c6},{0x221f,0x0007},{0x221e,0x0048},
                                     {0x2215,0x6412},{0x2216,0x6412},{0x2217,0x6412},{0x2218,0x6412},
                                     {0x2219,0x6412},{0x221A,0x6412},{0x221f,0x0001},{0x220c,0xdbf0},
                                     {0x2209,0x2576},{0x2207,0x287E},{0x220A,0x68E5},{0x221D,0x3DA4},
                                     {0x221C,0xE7F7},{0x2214,0x7F52},{0x2218,0x7FCE},{0x2208,0x04B7},
                                     {0x2206,0x4072},{0x2210,0xF05E},{0x221B,0xB414},{0x221F,0x0003},
                                     {0x221A,0x06A6},{0x2210,0xF05E},{0x2213,0x06EB},{0x2212,0xF4D2},
                                     {0x220E,0xE120},{0x2200,0x7C00},{0x2202,0x5FD0},{0x220D,0x0207},
                                     {0x221f,0x0002},{0x2205,0x0978},{0x2202,0x8C01},{0x2207,0x3620},
                                     {0x221C,0x0001},{0x2203,0x0420},{0x2204,0x80C8},{0x133e,0x0ede},
                                     {0x221f,0x0002},{0x220c,0x0073},{0x220d,0xEB65},{0x220e,0x51d1},
                                     {0x220f,0x5dcb},{0x2210,0x3044},{0x2211,0x1800},{0x2212,0x7E00},
                                     {0x2213,0x0000},{0x133f,0x0010},{0x133e,0x0ffe},{0x207f,0x0002},
                                     {0x2074,0x3D22},{0x2075,0x2000},{0x2076,0x6040},{0x2077,0x0000},
                                     {0x2078,0x0f0a},{0x2079,0x50AB},{0x207a,0x0000},{0x207b,0x0f0f},
                                     {0x205f,0x0002},{0x2054,0xFF00},{0x2055,0x000A},{0x2056,0x000A},
                                     {0x2057,0x0005},{0x2058,0x0005},{0x2059,0x0000},{0x205A,0x0005},
                                     {0x205B,0x0005},{0x205C,0x0005},{0x209f,0x0002},{0x2094,0x00AA},
                                     {0x2095,0x00AA},{0x2096,0x00AA},{0x2097,0x00AA},{0x2098,0x0055},
                                     {0x2099,0x00AA},{0x209A,0x00AA},{0x209B,0x00AA},{0x1363,0x8354},
                                     {0x1270,0x3333},{0x1271,0x3333},{0x1272,0x3333},{0x1330,0x00DB},
                                     {0x1203,0xff00},{0x1200,0x7fc4},{0x121d,0x1b06},{0x121e,0x07f0},
                                     {0x121f,0x0438},{0x1220,0x040f},{0x1221,0x040f},{0x1222,0x03eb},
                                     {0x1223,0x0438},{0x1224,0x040f},{0x1225,0x040f},{0x1226,0x03eb},
                                     {0x1227,0x0144},{0x1228,0x0138},{0x122f,0x0144},{0x1230,0x0138},
                                     {0x1229,0x0020},{0x122a,0x000c},{0x1231,0x0030},{0x1232,0x0024},
                                     {0x0219,0x0032},{0x0200,0x07d0},{0x0201,0x07d0},{0x0202,0x07d0},
                                     {0x0203,0x07d0},{0x0204,0x07d0},{0x0205,0x07d0},{0x0206,0x07d0},
                                     {0x0207,0x07d0},{0x0218,0x0032},{0x0208,0x0190},{0x0209,0x0190},
                                     {0x020a,0x0190},{0x020b,0x0190},{0x020c,0x0190},{0x020d,0x0190},
                                     {0x020e,0x0190},{0x020f,0x0190},{0x0210,0x0190},{0x0211,0x0190},
                                     {0x0212,0x0190},{0x0213,0x0190},{0x0214,0x0190},{0x0215,0x0190},
                                     {0x0216,0x0190},{0x0217,0x0190},{0x0900,0x0000},{0x0901,0x0000},
                                     {0x0902,0x0000},{0x0903,0x0000},{0x0865,0x3210},{0x087b,0x0000},
                                     {0x087c,0xff00},{0x087d,0x0000},{0x087e,0x0000},{0x0801,0x0100},
                                     {0x0802,0x0100},{0x1700,0x0125},{0x0301,0x00FF},{0x12AA,0x0096},
                                     {0x133f,0x0030},{0x133e,0x000e},{0x221f,0x0005},{0x2200,0x00C4},
                                     {0x221f,0x0000},{0x2210,0x05EF},{0x2204,0x05E1},{0x2200,0x1340},
                                     {0x133f,0x0010},{0xFFFF, 0xABCD}};

    if ((retVal = rtl8370_setAsicReg(0x13C2,0x0249))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_getAsicRegBits(0x1302, 0x7,&regData))!=RT_ERR_OK)
        return retVal;   

    index = 0;
    switch(regData)
    {
    case 0x0000:
        while (chipData0[index][0] != 0xFFFF && chipData0[index][1] != 0xABCD)
        {    
            if ((chipData0[index][0]&0xF000)==0x2000)
            {
                cnt = 0;
                busyFlag = 1;
                while (busyFlag&&cnt<5)
                {
                    cnt++;
                    if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                        return retVal;
                }
                if (5 == cnt)
                    return RT_ERR_BUSYWAIT_TIMEOUT;
            
                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32)chipData0[index][1])) !=  RT_ERR_OK)
                    return retVal;
                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32)chipData0[index][0])) !=  RT_ERR_OK)
                    return retVal;    
                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                    return retVal; 
            }
            else
            {
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32) chipData0[index][0],(uint32) chipData0[index][1]))
                    return RT_ERR_FAILED;
            }
            index ++;    
        } 
    case 0x0001:    
        while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
        {    
            if ((chipData1[index][0]&0xF000)==0x2000)
            {
                cnt = 0;
                busyFlag = 1;
                while (busyFlag&&cnt<5)
                {
                    cnt++;
                    if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                        return retVal;
                }
                if (5 == cnt)
                    return RT_ERR_BUSYWAIT_TIMEOUT;

                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32) chipData1[index][1])) !=  RT_ERR_OK)
                    return retVal;
                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32) chipData1[index][0])) !=  RT_ERR_OK)
                    return retVal;    
                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                    return retVal; 
            }
            else
            {
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32) chipData1[index][0],(uint32) chipData1[index][1]))
                    return RT_ERR_FAILED;
            } 
            index ++;    
        }
        
        if (RT_ERR_OK != rtl8370_setAsicReg(0x1700,0x135))
            return RT_ERR_FAILED;    
        break;
    case 0x0002:    
        while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
        {    
            if ((chipData1[index][0]&0xF000)==0x2000)
            {
                cnt = 0;
                busyFlag = 1;
                while (busyFlag&&cnt<5)
                {
                    cnt++;
                    if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                        return retVal;
                }
                if (5 == cnt)
                    return RT_ERR_BUSYWAIT_TIMEOUT;

                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32) chipData1[index][1])) !=  RT_ERR_OK)
                    return retVal;
                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32) chipData1[index][0])) !=  RT_ERR_OK)
                    return retVal;    
                if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                    return retVal; 
            }
            else
            {
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32) chipData1[index][0],(uint32) chipData1[index][1]))
                    return RT_ERR_FAILED;
            } 
            index ++;    
        }
        
        if (RT_ERR_OK != rtl8370_setAsicReg(0x1700,0x135))
            return RT_ERR_FAILED;    
        break;
    default:
        return RT_ERR_FAILED;
    }

    if ((retVal = rtl8370_setAsicReg(RTL8370_REG_LED_ACTIVE_LOW_CFG0, 0x6677))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_setAsicReg(RTL8370_REG_LED_ACTIVE_LOW_CFG1, 0x7406))!=RT_ERR_OK)
        return retVal;     
    
    if ((retVal = rtl8370_getAsicReg(RTL8370_REG_MISCELLANEOUS_CONFIGURE0,&regData))!=RT_ERR_OK)
        return retVal;

    if (regData & RTL8370_EFUSE_EN_MASK)
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT3_LED_ACTIVE_LOW_OFFSET+2, 0))!=RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT3_LED_ACTIVE_LOW_OFFSET+2, 1))!=RT_ERR_OK)
            return retVal;
    }
            
    if (regData & RTL8370_AUTOLOAD_EN_MASK)
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT2_LED_ACTIVE_LOW_OFFSET, 0))!=RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT2_LED_ACTIVE_LOW_OFFSET, 1))!=RT_ERR_OK)
            return retVal;
    }

    if (regData & RTL8370_DW8051_EN_MASK)
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT2_LED_ACTIVE_LOW_OFFSET+2, 0))!=RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT2_LED_ACTIVE_LOW_OFFSET+2, 1))!=RT_ERR_OK)
            return retVal;
    }

    if ((retVal = rtl8370_getAsicReg(RTL8370_REG_MISCELLANEOUS_CONFIGURE1,&regData))!=RT_ERR_OK)
        return retVal;

    if (regData & RTL8370_EEPROM_ADDRESS_16B_MASK)
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG1, RTL8370_PORT4_LED_ACTIVE_LOW_OFFSET, 1))!=RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG1, RTL8370_PORT4_LED_ACTIVE_LOW_OFFSET, 0))!=RT_ERR_OK)
            return retVal;
    }
            
    if (regData & 0x1000)
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT3_LED_ACTIVE_LOW_OFFSET, 1))!=RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT3_LED_ACTIVE_LOW_OFFSET, 0))!=RT_ERR_OK)
            return retVal;
    }

    if ((retVal = rtl8370_getAsicReg(RTL8370_REG_DW8051_RDY,&regData))!=RT_ERR_OK)
        return retVal;

    if (regData & 0x10)
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG1, RTL8370_PORT5_LED_ACTIVE_LOW_OFFSET, 1))!=RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG1, RTL8370_PORT5_LED_ACTIVE_LOW_OFFSET, 0))!=RT_ERR_OK)
            return retVal;
    }
            
    if (regData & 0x20)
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT1_LED_ACTIVE_LOW_OFFSET+2, 1))!=RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT1_LED_ACTIVE_LOW_OFFSET+2, 0))!=RT_ERR_OK)
            return retVal;
    }

    if (regData & 0x40)
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT1_LED_ACTIVE_LOW_OFFSET, 1))!=RT_ERR_OK)
            return retVal;
    }
    else
    {
        if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_ACTIVE_LOW_CFG0, RTL8370_PORT1_LED_ACTIVE_LOW_OFFSET, 0))!=RT_ERR_OK)
            return retVal;
    }

    if ((retVal = rtl8370_setAsicReg(RTL8370_REG_PARA_LED_IO_EN1,0))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_setAsicReg(RTL8370_REG_PARA_LED_IO_EN2,0))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_setAsicReg(RTL8370_REG_SCAN0_LED_IO_EN,0))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_setAsicReg(RTL8370_REG_SCAN1_LED_IO_EN,0))!=RT_ERR_OK)
        return retVal;
        
    /*Set learn limit to 8256*/
    for (index= 0; index <= RTK_MAX_NUM_OF_PORT; index++)
    {
        if ((retVal = rtl8370_setAsicLutLearnLimitNo(index,RTK_MAX_NUM_OF_LEARN_LIMIT))!=RT_ERR_OK)
            return retVal;
    } 


    
    return RT_ERR_OK;
}


static rtk_api_ret_t _rtk_switch_init1(void)
{
    rtk_api_ret_t retVal;
    uint32 index,regData;
#ifndef MDC_MDIO_OPERATION 
    uint32 busyFlag,cnt;
#endif
    CONST_T uint16 chipData0[][2] ={{0x1B24, 0x0000},{0x1B25, 0x0000},{0x1B26, 0x0000},{0x1B27, 0x0000},
                                    {0x207F, 0x0007},{0x207E, 0x000B},{0x2076, 0x1A00},{0x207F, 0x0000},
                                    {0x205F, 0x0007},{0x205E, 0x000A},{0x2059, 0x0000},{0x205A, 0x0000},
                                    {0x205B, 0x0000},{0x205C, 0x0000},{0x205E, 0x000B},{0x2055, 0x0500},
                                    {0x2056, 0x0000},{0x2057, 0x0000},{0x2058, 0x0000},{0x205F, 0x0000},
                                    {0x133F, 0x0030},{0x133E, 0x000E},{0x221F, 0x0005},{0x2201, 0x0700},
                                    {0x2205, 0x8B82},{0x2206, 0x05CB},{0x221F, 0x0007},{0x221E, 0x0008},
                                    {0x2219, 0x80C2},{0x221A, 0x0938},{0x221F, 0x0000},{0x221F, 0x0003},
                                    {0x2212, 0xC4D2},{0x220D, 0x0207},{0x221F, 0x0001},{0x2207, 0x267E},
                                    {0x221C, 0xE5F7},{0x221B, 0x0424},{0x221F, 0x0007},{0x221E, 0x0040},
                                    {0x2218, 0x0000},{0x221F, 0x0007},{0x221E, 0x002C},{0x2218, 0x008B},
                                    {0x221F, 0x0005},{0x2205, 0xFFF6},{0x2206, 0x0080},{0x2205, 0x8000},
                                    {0x2206, 0xF8E0},{0x2206, 0xE000},{0x2206, 0xE1E0},{0x2206, 0x01AC},
                                    {0x2206, 0x2408},{0x2206, 0xE08B},{0x2206, 0x84F7},{0x2206, 0x20E4},
                                    {0x2206, 0x8B84},{0x2206, 0xFC05},{0x2206, 0xF8FA},{0x2206, 0xEF69},
                                    {0x2206, 0xE08B},{0x2206, 0x86AC},{0x2206, 0x201A},{0x2206, 0xBF80},
                                    {0x2206, 0x59D0},{0x2206, 0x2402},{0x2206, 0x803D},{0x2206, 0xE0E0},
                                    {0x2206, 0xE4E1},{0x2206, 0xE0E5},{0x2206, 0x5806},{0x2206, 0x68C0},
                                    {0x2206, 0xD1D2},{0x2206, 0xE4E0},{0x2206, 0xE4E5},{0x2206, 0xE0E5},
                                    {0x2206, 0xEF96},{0x2206, 0xFEFC},{0x2206, 0x05FB},{0x2206, 0x0BFB},
                                    {0x2206, 0x58FF},{0x2206, 0x9E11},{0x2206, 0x06F0},{0x2206, 0x0C81},
                                    {0x2206, 0x8AE0},{0x2206, 0x0019},{0x2206, 0x1B89},{0x2206, 0xCFEB},
                                    {0x2206, 0x19EB},{0x2206, 0x19B0},{0x2206, 0xEFFF},{0x2206, 0x0BFF},
                                    {0x2206, 0x0425},{0x2206, 0x0807},{0x2206, 0x2640},{0x2206, 0x7227},
                                    {0x2206, 0x267E},{0x2206, 0x2804},{0x2206, 0xB729},{0x2206, 0x2576},
                                    {0x2206, 0x2A68},{0x2206, 0xE52B},{0x2206, 0xAD00},{0x2206, 0x2CDB},
                                    {0x2206, 0xF02D},{0x2206, 0x67BB},{0x2206, 0x2E7B},{0x2206, 0x0F2F},
                                    {0x2206, 0x7365},{0x2206, 0x31AC},{0x2206, 0xCC32},{0x2206, 0x2300},
                                    {0x2206, 0x332D},{0x2206, 0x1734},{0x2206, 0x7F52},{0x2206, 0x3510},
                                    {0x2206, 0x0036},{0x2206, 0x1000},{0x2206, 0x3710},{0x2206, 0x0038},
                                    {0x2206, 0x7FCE},{0x2206, 0x3CE5},{0x2206, 0xF73D},{0x2206, 0x3DA4},
                                    {0x2206, 0x6530},{0x2206, 0x3E67},{0x2206, 0x0053},{0x2206, 0x69D2},
                                    {0x2206, 0x0F6A},{0x2206, 0x012C},{0x2206, 0x6C2B},{0x2206, 0x136E},
                                    {0x2206, 0xE100},{0x2206, 0x6F12},{0x2206, 0xF771},{0x2206, 0x006B},
                                    {0x2206, 0x7306},{0x2206, 0xEB74},{0x2206, 0x94C7},{0x2206, 0x7698},
                                    {0x2206, 0x0A77},{0x2206, 0x5000},{0x2206, 0x788A},{0x2206, 0x1579},
                                    {0x2206, 0x7F6F},{0x2206, 0x7A06},{0x2206, 0xA600},{0x2205, 0x8B90},
                                    {0x2206, 0x8000},{0x2205, 0x8B92},{0x2206, 0x8000},{0x2205, 0x8B94},
                                    {0x2206, 0x8014},{0x2208, 0xFFFA},{0x2202, 0x3C65},{0x2205, 0xFFF6},
                                    {0x2206, 0x00F7},{0x221F, 0x0000},{0x221F, 0x0007},{0x221E, 0x0042},
                                    {0x2218, 0x0000},{0x221E, 0x002D},{0x2218, 0xF010},{0x221E, 0x0020},
                                    {0x2215, 0x0000},{0x221E, 0x0023},{0x2216, 0x8000},{0x221F, 0x0000},
                                    {0x133F, 0x0010},{0x133E, 0x0FFE},{0x1362, 0x0115},{0x1363, 0x0002},
                                    {0x1363, 0x0000},{0x1306, 0x000C},{0x1307, 0x000C},{0x1303, 0x0367},
                                    {0x1304, 0x7777},{0x1203, 0xFF00},{0x1200, 0x7FC4},{0x121D, 0x7D16},
                                    {0x121E, 0x03E8},{0x121F, 0x024E},{0x1220, 0x0230},{0x1221, 0x0244},
                                    {0x1222, 0x0226},{0x1223, 0x024E},{0x1224, 0x0230},{0x1225, 0x0244},
                                    {0x1226, 0x0226},{0x1227, 0x00C0},{0x1228, 0x00B4},{0x122F, 0x00C0},
                                    {0x1230, 0x00B4},{0x0208, 0x03E8},{0x0209, 0x03E8},{0x020A, 0x03E8},
                                    {0x020B, 0x03E8},{0x020C, 0x03E8},{0x020D, 0x03E8},{0x020E, 0x03E8},
                                    {0x020F, 0x03E8},{0x0210, 0x03E8},{0x0211, 0x03E8},{0x0212, 0x03E8},
                                    {0x0213, 0x03E8},{0x0214, 0x03E8},{0x0215, 0x03E8},{0x0216, 0x03E8},
                                    {0x0217, 0x03E8},{0x0865, 0x3210},{0x087B, 0x0000},{0x087C, 0xFF00},
                                    {0x087D, 0x0000},{0x087E, 0x0000},{0x0801, 0x0100},{0x0802, 0x0100},
                                    {0x0A20, 0x2040},{0x0A21, 0x2040},{0x0A22, 0x2040},{0x0A23, 0x2040},
                                    {0x0A24, 0x2040},{0x0A28, 0x2040},{0x0A29, 0x2040},{0x20A0, 0x1940},
                                    {0x20C0, 0x1940},{0x20E0, 0x1940},{0x130C, 0x0050},{0x1B03, 0x0876},
                                    {0xFFFF, 0xABCD}};

    CONST_T uint16 chipData1[][2] ={{0x1B24, 0x0000},{0x1B25, 0x0000},{0x1B26, 0x0000},{0x1B27, 0x0000},
                                    {0x207F, 0x0007},{0x207E, 0x000B},{0x2076, 0x1A00},{0x207F, 0x0000},
                                    {0x205F, 0x0007},{0x205E, 0x000A},{0x2059, 0x0000},{0x205A, 0x0000},
                                    {0x205B, 0x0000},{0x205C, 0x0000},{0x205E, 0x000B},{0x2055, 0x0500},
                                    {0x2056, 0x0000},{0x2057, 0x0000},{0x2058, 0x0000},{0x205F, 0x0000},
                                    {0x133F, 0x0030},{0x133E, 0x000E},{0x221F, 0x0005},{0x2201, 0x0700},
                                    {0x2205, 0x8B82},{0x2206, 0x05CB},{0x221F, 0x0007},{0x221E, 0x0008},
                                    {0x2219, 0x80C2},{0x221A, 0x0938},{0x221F, 0x0000},{0x221F, 0x0003},
                                    {0x2212, 0xC4D2},{0x220D, 0x0207},{0x221F, 0x0001},{0x2207, 0x267E},
                                    {0x221C, 0xE5F7},{0x221B, 0x0424},{0x221F, 0x0007},{0x221E, 0x0040},
                                    {0x2218, 0x0000},{0x221F, 0x0007},{0x221E, 0x002C},{0x2218, 0x008B},
                                    {0x221F, 0x0005},{0x2205, 0xFFF6},{0x2206, 0x0080},{0x2205, 0x8000},
                                    {0x2206, 0xF8E0},{0x2206, 0xE000},{0x2206, 0xE1E0},{0x2206, 0x01AC},
                                    {0x2206, 0x2408},{0x2206, 0xE08B},{0x2206, 0x84F7},{0x2206, 0x20E4},
                                    {0x2206, 0x8B84},{0x2206, 0xFC05},{0x2206, 0xF8FA},{0x2206, 0xEF69},
                                    {0x2206, 0xE08B},{0x2206, 0x86AC},{0x2206, 0x201A},{0x2206, 0xBF80},
                                    {0x2206, 0x59D0},{0x2206, 0x2402},{0x2206, 0x803D},{0x2206, 0xE0E0},
                                    {0x2206, 0xE4E1},{0x2206, 0xE0E5},{0x2206, 0x5806},{0x2206, 0x68C0},
                                    {0x2206, 0xD1D2},{0x2206, 0xE4E0},{0x2206, 0xE4E5},{0x2206, 0xE0E5},
                                    {0x2206, 0xEF96},{0x2206, 0xFEFC},{0x2206, 0x05FB},{0x2206, 0x0BFB},
                                    {0x2206, 0x58FF},{0x2206, 0x9E11},{0x2206, 0x06F0},{0x2206, 0x0C81},
                                    {0x2206, 0x8AE0},{0x2206, 0x0019},{0x2206, 0x1B89},{0x2206, 0xCFEB},
                                    {0x2206, 0x19EB},{0x2206, 0x19B0},{0x2206, 0xEFFF},{0x2206, 0x0BFF},
                                    {0x2206, 0x0425},{0x2206, 0x0807},{0x2206, 0x2640},{0x2206, 0x7227},
                                    {0x2206, 0x267E},{0x2206, 0x2804},{0x2206, 0xB729},{0x2206, 0x2576},
                                    {0x2206, 0x2A68},{0x2206, 0xE52B},{0x2206, 0xAD00},{0x2206, 0x2CDB},
                                    {0x2206, 0xF02D},{0x2206, 0x67BB},{0x2206, 0x2E7B},{0x2206, 0x0F2F},
                                    {0x2206, 0x7365},{0x2206, 0x31AC},{0x2206, 0xCC32},{0x2206, 0x2300},
                                    {0x2206, 0x332D},{0x2206, 0x1734},{0x2206, 0x7F52},{0x2206, 0x3510},
                                    {0x2206, 0x0036},{0x2206, 0x1000},{0x2206, 0x3710},{0x2206, 0x0038},
                                    {0x2206, 0x7FCE},{0x2206, 0x3CE5},{0x2206, 0xF73D},{0x2206, 0x3DA4},
                                    {0x2206, 0x6530},{0x2206, 0x3E67},{0x2206, 0x0053},{0x2206, 0x69D2},
                                    {0x2206, 0x0F6A},{0x2206, 0x012C},{0x2206, 0x6C2B},{0x2206, 0x136E},
                                    {0x2206, 0xE100},{0x2206, 0x6F12},{0x2206, 0xF771},{0x2206, 0x006B},
                                    {0x2206, 0x7306},{0x2206, 0xEB74},{0x2206, 0x94C7},{0x2206, 0x7698},
                                    {0x2206, 0x0A77},{0x2206, 0x5000},{0x2206, 0x788A},{0x2206, 0x1579},
                                    {0x2206, 0x7F6F},{0x2206, 0x7A06},{0x2206, 0xA600},{0x2205, 0x8B90},
                                    {0x2206, 0x8000},{0x2205, 0x8B92},{0x2206, 0x8000},{0x2205, 0x8B94},
                                    {0x2206, 0x8014},{0x2208, 0xFFFA},{0x2202, 0x3C65},{0x2205, 0xFFF6},
                                    {0x2206, 0x00F7},{0x221F, 0x0000},{0x221F, 0x0007},{0x221E, 0x0042},
                                    {0x2218, 0x0000},{0x221E, 0x002D},{0x2218, 0xF010},{0x221E, 0x0020},
                                    {0x2215, 0x0000},{0x221E, 0x0023},{0x2216, 0x8000},{0x221F, 0x0000},
                                    {0x133F, 0x0010},{0x133E, 0x0FFE},{0x1362, 0x0115},{0x1363, 0x0002},
                                    {0x1363, 0x0000},{0x1306, 0x000C},{0x1307, 0x000C},{0x1303, 0x0367},
                                    {0x1304, 0x7777},{0x1203, 0xFF00},{0x1200, 0x7FC4},{0x0865, 0x3210},
                                    {0x087B, 0x0000},{0x087C, 0xFF00},{0x087D, 0x0000},{0x087E, 0x0000},
                                    {0x0801, 0x0100},{0x0802, 0x0100},{0x0A20, 0x2040},{0x0A21, 0x2040},
                                    {0x0A22, 0x2040},{0x0A23, 0x2040},{0x0A24, 0x2040},{0x0A25, 0x2040},
                                    {0x0A26, 0x2040},{0x0A27, 0x2040},{0x0A28, 0x2040},{0x0A29, 0x2040},
                                    {0x130C, 0x0050},{0x1B03, 0x0876},{0xFFFF, 0xABCD}};

    if ((retVal = rtl8370_setAsicReg(0x13C2,0x0249))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_getAsicRegBits(0x1302, 0x7,&regData))!=RT_ERR_OK)
        return retVal;  

#ifdef MDC_MDIO_OPERATION  
    index = 0;
    switch(regData)
    {
        case 0x0000:
            while (chipData0[index][0] != 0xFFFF && chipData0[index][1] != 0xABCD)
            {    
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData0[index][0],(uint32)chipData0[index][1]))
                    return RT_ERR_FAILED;
                index ++;    
            } 
            break;
        case 0x0001:    
            while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
            {    
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData1[index][0],(uint32)chipData1[index][1]))
                    return RT_ERR_FAILED;
                index ++;    
            }            
            break;
        case 0x0002:    
            while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
            {    
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData1[index][0],(uint32)chipData1[index][1]))
                    return RT_ERR_FAILED;
                index ++;    
            }            
            break;
        default:
            return RT_ERR_FAILED;
    }
#else 
    index = 0;
    switch(regData)
    {
        
        case 0x0000:
            while (chipData0[index][0] != 0xFFFF && chipData0[index][1] != 0xABCD)
            {    
                if ((chipData0[index][0]&0xF000)==0x2000)
                {        
                    cnt = 0;
                    busyFlag = 1;
                    while (busyFlag&&cnt<5)
                    {
                        cnt++;
                        if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                            return retVal;
                    }
                    if (5 == cnt)
                        return RT_ERR_BUSYWAIT_TIMEOUT;
                    
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32)chipData0[index][1])) !=  RT_ERR_OK)
                        return retVal;
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32)chipData0[index][0])) !=  RT_ERR_OK)
                        return retVal;    
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                        return retVal; 
                }
                else
                {
                    if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData0[index][0],(uint32)chipData0[index][1]))
                        return RT_ERR_FAILED;
                }
                index ++;    
            } 
            break;
        case 0x0001:    
            while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
            {    
                if ((chipData1[index][0]&0xF000)==0x2000)
                {        
                    cnt = 0;
                    busyFlag = 1;
                    while (busyFlag&&cnt<5)
                    {
                        cnt++;
                        if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                            return retVal;
                    }
                    if (5 == cnt)
                        return RT_ERR_BUSYWAIT_TIMEOUT;
        
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32)chipData1[index][1])) !=  RT_ERR_OK)
                        return retVal;
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32)chipData1[index][0])) !=  RT_ERR_OK)
                        return retVal;    
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                        return retVal; 
                }
                else
                {
                    if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData1[index][0],(uint32)chipData1[index][1]))
                        return RT_ERR_FAILED;
                } 
                index ++;    
            }            
            break;
        case 0x0002:    
            while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
            {    
                if ((chipData1[index][0]&0xF000)==0x2000)
                {        
                    cnt = 0;
                    busyFlag = 1;
                    while (busyFlag&&cnt<5)
                    {
                        cnt++;
                        if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                            return retVal;
                    }
                    if (5 == cnt)
                        return RT_ERR_BUSYWAIT_TIMEOUT;
        
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32)chipData1[index][1])) !=  RT_ERR_OK)
                        return retVal;
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32)chipData1[index][0])) !=  RT_ERR_OK)
                        return retVal;    
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                        return retVal; 
                }
                else
                {
                    if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData1[index][0],(uint32)chipData1[index][1]))
                        return RT_ERR_FAILED;
                } 
                index ++;    
            }            
            break;
        default:
            return RT_ERR_FAILED;
    } 
#endif /*End of #ifdef MDC_MDIO_OPERATION*/

    return RT_ERR_OK;

}
#endif

static rtk_api_ret_t _rtk_switch_init2(void)
{
    rtk_api_ret_t retVal;
    uint32 index,regData;
#ifndef MDC_MDIO_OPERATION 
    uint32 busyFlag,cnt;
#endif

#if 0
    CONST_T uint16 chipData0[][2] ={{0x1B24, 0x0000},{0x1B25, 0x0000},{0x1B26, 0x0000},{0x1B27, 0x0000},
                                    {0x207F, 0x0007},{0x207E, 0x000B},{0x2076, 0x1A00},{0x207E, 0x000A},
                                    {0x2078, 0x1D22},{0x207F, 0x0000},{0x205F, 0x0007},{0x205E, 0x000A},
                                    {0x2059, 0x0000},{0x205A, 0x0000},{0x205B, 0x0000},{0x205C, 0x0000},
                                    {0x205E, 0x000B},{0x2055, 0x0500},{0x2056, 0x0000},{0x2057, 0x0000},
                                    {0x2058, 0x0000},{0x205F, 0x0000},{0x1362, 0x0115},{0x1363, 0x0002},
                                    {0x1363, 0x0000},{0x133F, 0x0030},{0x133E, 0x000E},{0x221F, 0x0007},
                                    {0x221E, 0x0040},{0x2218, 0x0000},{0x221F, 0x0007},{0x221E, 0x002C},
                                    {0x2218, 0x008B},{0x221F, 0x0005},{0x2205, 0x8B6E},{0x2206, 0x0000},
                                    {0x220F, 0x0100},{0x2205, 0xFFF6},{0x2206, 0x0080},{0x2205, 0x8000},
                                    {0x2206, 0x0280},{0x2206, 0x1EF7},{0x2206, 0x00E0},{0x2206, 0xFFF7},
                                    {0x2206, 0xA080},{0x2206, 0x02AE},{0x2206, 0xF602},{0x2206, 0x8046},
                                    {0x2206, 0x0201},{0x2206, 0x5002},{0x2206, 0x0163},{0x2206, 0x0280},
                                    {0x2206, 0xCD02},{0x2206, 0x0179},{0x2206, 0xAEE7},{0x2206, 0xBF80},
                                    {0x2206, 0x61D7},{0x2206, 0x8580},{0x2206, 0xD06C},{0x2206, 0x0229},
                                    {0x2206, 0x71EE},{0x2206, 0x8B64},{0x2206, 0x00EE},{0x2206, 0x8570},
                                    {0x2206, 0x00EE},{0x2206, 0x8571},{0x2206, 0x00EE},{0x2206, 0x8AFC},
                                    {0x2206, 0x07EE},{0x2206, 0x8AFD},{0x2206, 0x73EE},{0x2206, 0xFFF6},
                                    {0x2206, 0x00EE},{0x2206, 0xFFF7},{0x2206, 0xFC04},{0x2206, 0xBF85},
                                    {0x2206, 0x80D0},{0x2206, 0x6C02},{0x2206, 0x2978},{0x2206, 0xE0E0},
                                    {0x2206, 0xE4E1},{0x2206, 0xE0E5},{0x2206, 0x5806},{0x2206, 0x68C0},
                                    {0x2206, 0xD1D2},{0x2206, 0xE4E0},{0x2206, 0xE4E5},{0x2206, 0xE0E5},
                                    {0x2206, 0x0425},{0x2206, 0x0807},{0x2206, 0x2640},{0x2206, 0x7227},
                                    {0x2206, 0x267E},{0x2206, 0x2804},{0x2206, 0xB729},{0x2206, 0x2576},
                                    {0x2206, 0x2A68},{0x2206, 0xE52B},{0x2206, 0xAD00},{0x2206, 0x2CDB},
                                    {0x2206, 0xF02D},{0x2206, 0x67BB},{0x2206, 0x2E7B},{0x2206, 0x0F2F},
                                    {0x2206, 0x7365},{0x2206, 0x31AC},{0x2206, 0xCC32},{0x2206, 0x2300},
                                    {0x2206, 0x332D},{0x2206, 0x1734},{0x2206, 0x7F52},{0x2206, 0x3510},
                                    {0x2206, 0x0036},{0x2206, 0x1000},{0x2206, 0x3710},{0x2206, 0x0038},
                                    {0x2206, 0x7FCE},{0x2206, 0x3CE5},{0x2206, 0xF73D},{0x2206, 0x3DA4},
                                    {0x2206, 0x6530},{0x2206, 0x3E67},{0x2206, 0x0053},{0x2206, 0x69D2},
                                    {0x2206, 0x0F6A},{0x2206, 0x012C},{0x2206, 0x6C2B},{0x2206, 0x136E},
                                    {0x2206, 0xE100},{0x2206, 0x6F12},{0x2206, 0xF771},{0x2206, 0x006B},
                                    {0x2206, 0x7306},{0x2206, 0xEB74},{0x2206, 0x94C7},{0x2206, 0x7698},
                                    {0x2206, 0x0A77},{0x2206, 0x5000},{0x2206, 0x788A},{0x2206, 0x1579},
                                    {0x2206, 0x7F6F},{0x2206, 0x7A06},{0x2206, 0xA6F8},{0x2206, 0xE08B},
                                    {0x2206, 0x8EAD},{0x2206, 0x2006},{0x2206, 0x0280},{0x2206, 0xDC02},
                                    {0x2206, 0x8109},{0x2206, 0xFC04},{0x2206, 0xF8F9},{0x2206, 0xE08B},
                                    {0x2206, 0x87AD},{0x2206, 0x2022},{0x2206, 0xE0E2},{0x2206, 0x00E1},
                                    {0x2206, 0xE201},{0x2206, 0xAD20},{0x2206, 0x11E2},{0x2206, 0xE022},
                                    {0x2206, 0xE3E0},{0x2206, 0x23AD},{0x2206, 0x3908},{0x2206, 0x5AC0},
                                    {0x2206, 0x9F04},{0x2206, 0xF724},{0x2206, 0xAE02},{0x2206, 0xF624},
                                    {0x2206, 0xE4E2},{0x2206, 0x00E5},{0x2206, 0xE201},{0x2206, 0xFDFC},
                                    {0x2206, 0x04F8},{0x2206, 0xF9E0},{0x2206, 0x8B85},{0x2206, 0xAD25},
                                    {0x2206, 0x48E0},{0x2206, 0x8AD2},{0x2206, 0xE18A},{0x2206, 0xD37C},
                                    {0x2206, 0x0000},{0x2206, 0x9E35},{0x2206, 0xEE8A},{0x2206, 0xD200},
                                    {0x2206, 0xEE8A},{0x2206, 0xD300},{0x2206, 0xE08A},{0x2206, 0xFCE1},
                                    {0x2206, 0x8AFD},{0x2206, 0xE285},{0x2206, 0x70E3},{0x2206, 0x8571},
                                    {0x2206, 0x0229},{0x2206, 0x3AAD},{0x2206, 0x2012},{0x2206, 0xEE8A},
                                    {0x2206, 0xD203},{0x2206, 0xEE8A},{0x2206, 0xD3B7},{0x2206, 0xEE85},
                                    {0x2206, 0x7000},{0x2206, 0xEE85},{0x2206, 0x7100},{0x2206, 0xAE11},
                                    {0x2206, 0x15E6},{0x2206, 0x8570},{0x2206, 0xE785},{0x2206, 0x71AE},
                                    {0x2206, 0x08EE},{0x2206, 0x8570},{0x2206, 0x00EE},{0x2206, 0x8571},
                                    {0x2206, 0x00FD},{0x2206, 0xFC04},{0x2206, 0xCCE2},{0x2206, 0x0000},
                                    {0x2205, 0xE142},{0x2206, 0x0701},{0x2205, 0xE140},{0x2206, 0x0405},
                                    {0x220F, 0x0000},{0x221F, 0x0000},{0x221F, 0x0005},{0x2205, 0x85E4},
                                    {0x2206, 0x8A14},{0x2205, 0x85E7},{0x2206, 0x7F6E},{0x221F, 0x0007},
                                    {0x221E, 0x002D},{0x2218, 0xF030},{0x221E, 0x0023},{0x2216, 0x0005},
                                    {0x2215, 0x005C},{0x2219, 0x0068},{0x2215, 0x0082},{0x2219, 0x000A},
                                    {0x2215, 0x00A1},{0x2219, 0x0081},{0x2215, 0x00AF},{0x2219, 0x0080},
                                    {0x2215, 0x00D4},{0x2219, 0x0000},{0x2215, 0x00E4},{0x2219, 0x0081},
                                    {0x2215, 0x00E7},{0x2219, 0x0080},{0x2215, 0x010D},{0x2219, 0x0083},
                                    {0x2215, 0x0118},{0x2219, 0x0083},{0x2215, 0x0120},{0x2219, 0x0082},
                                    {0x2215, 0x019C},{0x2219, 0x0081},{0x2215, 0x01A4},{0x2219, 0x0080},
                                    {0x2215, 0x01CD},{0x2219, 0x0000},{0x2215, 0x01DD},{0x2219, 0x0081},
                                    {0x2215, 0x01E0},{0x2219, 0x0080},{0x2215, 0x0147},{0x2219, 0x0096},
                                    {0x2216, 0x0000},{0x221E, 0x002D},{0x2218, 0xF010},{0x221F, 0x0005},
                                    {0x2205, 0x8B84},{0x2206, 0x0062},{0x221F, 0x0000},{0x220D, 0x0003},
                                    {0x220E, 0x0015},{0x220D, 0x4003},{0x220E, 0x0006},{0x133F, 0x0010},
                                    {0x133E, 0x0FFE},{0x12A4, 0x380A},{0x1303, 0x0367},{0x1304, 0x7777},
                                    {0x1203, 0xFF00},{0x1200, 0x7FC4},{0x121D, 0x7D16},{0x121E, 0x03E8},
                                    {0x121F, 0x024E},{0x1220, 0x0230},{0x1221, 0x0244},{0x1222, 0x0226},
                                    {0x1223, 0x024E},{0x1224, 0x0230},{0x1225, 0x0244},{0x1226, 0x0226},
                                    {0x1227, 0x00C0},{0x1228, 0x00B4},{0x122F, 0x00C0},{0x1230, 0x00B4},
                                    {0x0208, 0x03E8},{0x0209, 0x03E8},{0x020A, 0x03E8},{0x020B, 0x03E8},
                                    {0x020C, 0x03E8},{0x020D, 0x03E8},{0x020E, 0x03E8},{0x020F, 0x03E8},
                                    {0x0210, 0x03E8},{0x0211, 0x03E8},{0x0212, 0x03E8},{0x0213, 0x03E8},
                                    {0x0214, 0x03E8},{0x0215, 0x03E8},{0x0216, 0x03E8},{0x0217, 0x03E8},
                                    {0x0865, 0x3210},{0x087B, 0x0000},{0x087C, 0xFF00},{0x087D, 0x0000},
                                    {0x087E, 0x0000},{0x0801, 0x0100},{0x0802, 0x0100},{0x0A20, 0x2040},
                                    {0x0A21, 0x2040},{0x0A22, 0x2040},{0x0A23, 0x2040},{0x0A24, 0x2040},
                                    {0x0A28, 0x2040},{0x0A29, 0x2040},{0x20A0, 0x1940},{0x20C0, 0x1940},
                                    {0x20E0, 0x1940},{0x1B03, 0x0876},{0xFFFF, 0xABCD}};
#endif

    CONST_T uint16 chipData1[][2] ={{0x1B24, 0x0000},{0x1B25, 0x0000},{0x1B26, 0x0000},{0x1B27, 0x0000},
                                    {0x207F, 0x0007},{0x207E, 0x000B},{0x2076, 0x1A00},{0x207E, 0x000A},
                                    {0x2078, 0x1D22},{0x207F, 0x0000},{0x205F, 0x0007},{0x205E, 0x000A},
                                    {0x2059, 0x0000},{0x205A, 0x0000},{0x205B, 0x0000},{0x205C, 0x0000},
                                    {0x205E, 0x000B},{0x2055, 0x0500},{0x2056, 0x0000},{0x2057, 0x0000},
                                    {0x2058, 0x0000},{0x205F, 0x0000},{0x1362, 0x0115},{0x1363, 0x0002},
                                    {0x1363, 0x0000},{0x133F, 0x0030},{0x133E, 0x000E},{0x221F, 0x0007},
                                    {0x221E, 0x0040},{0x2218, 0x0000},{0x221F, 0x0007},{0x221E, 0x002C},
                                    {0x2218, 0x008B},{0x221F, 0x0005},{0x2205, 0x8B6E},{0x2206, 0x0000},
                                    {0x220F, 0x0100},{0x2205, 0xFFF6},{0x2206, 0x0080},{0x2205, 0x8000},
                                    {0x2206, 0x0280},{0x2206, 0x1EF7},{0x2206, 0x00E0},{0x2206, 0xFFF7},
                                    {0x2206, 0xA080},{0x2206, 0x02AE},{0x2206, 0xF602},{0x2206, 0x8046},
                                    {0x2206, 0x0201},{0x2206, 0x5002},{0x2206, 0x0163},{0x2206, 0x0280},
                                    {0x2206, 0xCD02},{0x2206, 0x0179},{0x2206, 0xAEE7},{0x2206, 0xBF80},
                                    {0x2206, 0x61D7},{0x2206, 0x8580},{0x2206, 0xD06C},{0x2206, 0x0229},
                                    {0x2206, 0x71EE},{0x2206, 0x8B64},{0x2206, 0x00EE},{0x2206, 0x8570},
                                    {0x2206, 0x00EE},{0x2206, 0x8571},{0x2206, 0x00EE},{0x2206, 0x8AFC},
                                    {0x2206, 0x07EE},{0x2206, 0x8AFD},{0x2206, 0x73EE},{0x2206, 0xFFF6},
                                    {0x2206, 0x00EE},{0x2206, 0xFFF7},{0x2206, 0xFC04},{0x2206, 0xBF85},
                                    {0x2206, 0x80D0},{0x2206, 0x6C02},{0x2206, 0x2978},{0x2206, 0xE0E0},
                                    {0x2206, 0xE4E1},{0x2206, 0xE0E5},{0x2206, 0x5806},{0x2206, 0x68C0},
                                    {0x2206, 0xD1D2},{0x2206, 0xE4E0},{0x2206, 0xE4E5},{0x2206, 0xE0E5},
                                    {0x2206, 0x0425},{0x2206, 0x0807},{0x2206, 0x2640},{0x2206, 0x7227},
                                    {0x2206, 0x267E},{0x2206, 0x2804},{0x2206, 0xB729},{0x2206, 0x2576},
                                    {0x2206, 0x2A68},{0x2206, 0xE52B},{0x2206, 0xAD00},{0x2206, 0x2CDB},
                                    {0x2206, 0xF02D},{0x2206, 0x67BB},{0x2206, 0x2E7B},{0x2206, 0x0F2F},
                                    {0x2206, 0x7365},{0x2206, 0x31AC},{0x2206, 0xCC32},{0x2206, 0x2300},
                                    {0x2206, 0x332D},{0x2206, 0x1734},{0x2206, 0x7F52},{0x2206, 0x3510},
                                    {0x2206, 0x0036},{0x2206, 0x1000},{0x2206, 0x3710},{0x2206, 0x0038},
                                    {0x2206, 0x7FCE},{0x2206, 0x3CE5},{0x2206, 0xF73D},{0x2206, 0x3DA4},
                                    {0x2206, 0x6530},{0x2206, 0x3E67},{0x2206, 0x0053},{0x2206, 0x69D2},
                                    {0x2206, 0x0F6A},{0x2206, 0x012C},{0x2206, 0x6C2B},{0x2206, 0x136E},
                                    {0x2206, 0xE100},{0x2206, 0x6F12},{0x2206, 0xF771},{0x2206, 0x006B},
                                    {0x2206, 0x7306},{0x2206, 0xEB74},{0x2206, 0x94C7},{0x2206, 0x7698},
                                    {0x2206, 0x0A77},{0x2206, 0x5000},{0x2206, 0x788A},{0x2206, 0x1579},
                                    {0x2206, 0x7F6F},{0x2206, 0x7A06},{0x2206, 0xA6F8},{0x2206, 0xE08B},
                                    {0x2206, 0x8EAD},{0x2206, 0x2006},{0x2206, 0x0280},{0x2206, 0xDC02},
                                    {0x2206, 0x8109},{0x2206, 0xFC04},{0x2206, 0xF8F9},{0x2206, 0xE08B},
                                    {0x2206, 0x87AD},{0x2206, 0x2022},{0x2206, 0xE0E2},{0x2206, 0x00E1},
                                    {0x2206, 0xE201},{0x2206, 0xAD20},{0x2206, 0x11E2},{0x2206, 0xE022},
                                    {0x2206, 0xE3E0},{0x2206, 0x23AD},{0x2206, 0x3908},{0x2206, 0x5AC0},
                                    {0x2206, 0x9F04},{0x2206, 0xF724},{0x2206, 0xAE02},{0x2206, 0xF624},
                                    {0x2206, 0xE4E2},{0x2206, 0x00E5},{0x2206, 0xE201},{0x2206, 0xFDFC},
                                    {0x2206, 0x04F8},{0x2206, 0xF9E0},{0x2206, 0x8B85},{0x2206, 0xAD25},
                                    {0x2206, 0x48E0},{0x2206, 0x8AD2},{0x2206, 0xE18A},{0x2206, 0xD37C},
                                    {0x2206, 0x0000},{0x2206, 0x9E35},{0x2206, 0xEE8A},{0x2206, 0xD200},
                                    {0x2206, 0xEE8A},{0x2206, 0xD300},{0x2206, 0xE08A},{0x2206, 0xFCE1},
                                    {0x2206, 0x8AFD},{0x2206, 0xE285},{0x2206, 0x70E3},{0x2206, 0x8571},
                                    {0x2206, 0x0229},{0x2206, 0x3AAD},{0x2206, 0x2012},{0x2206, 0xEE8A},
                                    {0x2206, 0xD203},{0x2206, 0xEE8A},{0x2206, 0xD3B7},{0x2206, 0xEE85},
                                    {0x2206, 0x7000},{0x2206, 0xEE85},{0x2206, 0x7100},{0x2206, 0xAE11},
                                    {0x2206, 0x15E6},{0x2206, 0x8570},{0x2206, 0xE785},{0x2206, 0x71AE},
                                    {0x2206, 0x08EE},{0x2206, 0x8570},{0x2206, 0x00EE},{0x2206, 0x8571},
                                    {0x2206, 0x00FD},{0x2206, 0xFC04},{0x2206, 0xCCE2},{0x2206, 0x0000},
                                    {0x2205, 0xE142},{0x2206, 0x0701},{0x2205, 0xE140},{0x2206, 0x0405},
                                    {0x220F, 0x0000},{0x221F, 0x0000},{0x221F, 0x0005},{0x2205, 0x85E4},
                                    {0x2206, 0x8A14},{0x2205, 0x85E7},{0x2206, 0x7F6E},{0x221F, 0x0007},
                                    {0x221E, 0x002D},{0x2218, 0xF030},{0x221E, 0x0023},{0x2216, 0x0005},
                                    {0x2215, 0x005C},{0x2219, 0x0068},{0x2215, 0x0082},{0x2219, 0x000A},
                                    {0x2215, 0x00A1},{0x2219, 0x0081},{0x2215, 0x00AF},{0x2219, 0x0080},
                                    {0x2215, 0x00D4},{0x2219, 0x0000},{0x2215, 0x00E4},{0x2219, 0x0081},
                                    {0x2215, 0x00E7},{0x2219, 0x0080},{0x2215, 0x010D},{0x2219, 0x0083},
                                    {0x2215, 0x0118},{0x2219, 0x0083},{0x2215, 0x0120},{0x2219, 0x0082},
                                    {0x2215, 0x019C},{0x2219, 0x0081},{0x2215, 0x01A4},{0x2219, 0x0080},
                                    {0x2215, 0x01CD},{0x2219, 0x0000},{0x2215, 0x01DD},{0x2219, 0x0081},
                                    {0x2215, 0x01E0},{0x2219, 0x0080},{0x2215, 0x0147},{0x2219, 0x0096},
                                    {0x2216, 0x0000},{0x221E, 0x002D},{0x2218, 0xF010},{0x221F, 0x0005},
                                    {0x2205, 0x8B84},{0x2206, 0x0062},{0x221F, 0x0000},{0x220D, 0x0003},
                                    {0x220E, 0x0015},{0x220D, 0x4003},{0x220E, 0x0006},{0x133F, 0x0010},
                                    {0x133E, 0x0FFE},{0x12A4, 0x380A},{0x1303, 0x0367},{0x1304, 0x7777},
                                    {0x1203, 0xFF00},{0x1200, 0x7FC4},{0x0865, 0x3210},{0x087B, 0x0000},
                                    {0x087C, 0xFF00},{0x087D, 0x0000},{0x087E, 0x0000},{0x0801, 0x0100},
                                    {0x0802, 0x0100},{0x0A20, 0x2040},{0x0A21, 0x2040},{0x0A22, 0x2040},
                                    {0x0A23, 0x2040},{0x0A24, 0x2040},{0x0A25, 0x2040},{0x0A26, 0x2040},
                                    {0x0A27, 0x2040},{0x0A28, 0x2040},{0x0A29, 0x2040},{0x1B03, 0x0876},
                                    {0xFFFF, 0xABCD}};


    if ((retVal = rtl8370_setAsicReg(0x13C2,0x0249))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_getAsicRegBits(0x1302, 0x7,&regData))!=RT_ERR_OK)
        return retVal;   

#ifdef MDC_MDIO_OPERATION  
    index = 0;
    switch(regData)
    {
        case 0x0000:
            while (chipData0[index][0] != 0xFFFF && chipData0[index][1] != 0xABCD)
            {    
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData0[index][0],(uint32)chipData0[index][1]))
                    return RT_ERR_FAILED;
                index ++;    
            } 
            break;
        case 0x0001:    
            while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
            {    
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData1[index][0],(uint32)chipData1[index][1]))
                    return RT_ERR_FAILED;
                index ++;    
            }            
            break;
        case 0x0002:    
            while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
            {    
                if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData1[index][0],(uint32)chipData1[index][1]))
                    return RT_ERR_FAILED;
                index ++;    
            }            
            break;
        default:
            return RT_ERR_FAILED;
    }
#else 
    index = 0;

    switch(regData)
    {
#if 0        
        case 0x0000:
            while (chipData0[index][0] != 0xFFFF && chipData0[index][1] != 0xABCD)
            {    
                if ((chipData0[index][0]&0xF000)==0x2000)
                {       
                    cnt = 0;
                    busyFlag = 1;
                    while (busyFlag&&cnt<5)
                    {
                        cnt++;
                        if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                            return retVal;
                    }
                    if (5 == cnt)
                        return RT_ERR_BUSYWAIT_TIMEOUT;
                    
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32)chipData0[index][1])) !=  RT_ERR_OK)
                        return retVal;
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32)chipData0[index][0])) !=  RT_ERR_OK)
                        return retVal;    
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                        return retVal; 
                }
                else
                {
                    if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData0[index][0],(uint32)chipData0[index][1]))
                        return RT_ERR_FAILED;
                }
                index ++;    
            } 
            break;
        case 0x0001:            
            while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
            {    
                if ((chipData1[index][0]&0xF000)==0x2000)
                {        
                    cnt = 0;
                    busyFlag = 1;
                    while (busyFlag&&cnt<5)
                    {
                        cnt++;
                        if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                            return retVal;
                    }
                    if (5 == cnt)
                        return RT_ERR_BUSYWAIT_TIMEOUT;
        
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32)chipData1[index][1])) !=  RT_ERR_OK)
                        return retVal;
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32)chipData1[index][0])) !=  RT_ERR_OK)
                        return retVal;    
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                        return retVal; 
                }
                else
                {
                    if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData1[index][0],(uint32)chipData1[index][1]))
                        return RT_ERR_FAILED;
                } 
                index ++;    
            }            
            break;
#endif			
        case 0x0002:
            while (chipData1[index][0] != 0xFFFF && chipData1[index][1] != 0xABCD)
            {    
                if ((chipData1[index][0]&0xF000)==0x2000)
                {       
                    cnt = 0;
                    busyFlag = 1;
                    while (busyFlag&&cnt<5)
                    {
                        cnt++;
                        if ((retVal = rtl8370_getAsicRegBit(RTK_INDRECT_ACCESS_STATUS, RTK_PHY_BUSY_OFFSET,&busyFlag)) !=  RT_ERR_OK)
                            return retVal;
                    }
                    if (5 == cnt)
                        return RT_ERR_BUSYWAIT_TIMEOUT;
        
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_WRITE_DATA, (uint32)chipData1[index][1])) !=  RT_ERR_OK)
                        return retVal;
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_ADDRESS, (uint32)chipData1[index][0])) !=  RT_ERR_OK)
                        return retVal;    
                    if ((retVal = rtl8370_setAsicReg(RTK_INDRECT_ACCESS_CRTL, RTK_CMD_MASK | RTK_RW_MASK)) !=  RT_ERR_OK)
                        return retVal; 
                }
                else
                {
                    if (RT_ERR_OK != rtl8370_setAsicReg((uint32)chipData1[index][0],(uint32)chipData1[index][1]))
                        return RT_ERR_FAILED;
                } 
                index ++;    
            }            
            break;
        default:
            return RT_ERR_FAILED;
    } 
#endif /*End of #ifdef MDC_MDIO_OPERATION*/

    return RT_ERR_OK;
}




/* Function Name:
 *      rtk_switch_init 
 * Description:
 *      Set chip to default configuration enviroment
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 * Note:
 *      The API can set chip registers to default configuration for different release chip model.
 */


rtk_api_ret_t rtk_switch_init(void)
{

    rtk_api_ret_t retVal;
    uint32 regData1,regData2;

    if ((retVal = rtl8370_setAsicReg(0x13C2,0x0249))!=RT_ERR_OK)
        return retVal;

    if ((retVal = rtl8370_getAsicRegBits(0x1301, 0xF000,&regData1))!=RT_ERR_OK)
        return retVal; 

    if ((retVal = rtl8370_setAsicPHYReg(0,31,5))!=RT_ERR_OK)
        return retVal; 
    if ((retVal = rtl8370_setAsicPHYReg(0,5,0x3ffe))!=RT_ERR_OK)
        return retVal; 
    if ((retVal = rtl8370_getAsicPHYReg(0,6,&regData2))!=RT_ERR_OK)
        return retVal; 

#if 0	
    if (0 == regData1)
    {  
        if ((retVal = _rtk_switch_init0()) != RT_ERR_OK)
            return retVal;
    }
	
    else if (1 == regData1)
    {
        if (0x94eb == regData2)
        {
            if ((retVal = _rtk_switch_init1()) != RT_ERR_OK)
                return retVal;
        }
        else if (0x2104 == regData2)
        {       
            if ((retVal = _rtk_switch_init2()) != RT_ERR_OK)
                return retVal;
        }
    }
#else
	if ((1 == regData1) && (0x2104 == regData2))
	{       
		if ((retVal = _rtk_switch_init2()) != RT_ERR_OK)
			return retVal;
	}
#endif

    /*Enable System Based LED*/
    if ((retVal = rtl8370_setAsicRegBit(RTL8370_REG_LED_SYS_CONFIG, RTL8370_LED_IO_DISABLE_OFFSET, 0))!=RT_ERR_OK)
        return retVal;  
    
    return RT_ERR_OK;
}

/*
@func ret_t | rtl8370_setAsicLedGroupEnable | Turn on/off Led of all system ports
@parm uint32 | group | LED group id.
@parm uint32 | portmask | LED port mask. 
@rvalue RT_ERR_OK | Success.
@rvalue RT_ERR_SMI | SMI access error.
@rvalue RT_ERR_PORT_ID | Invalid port number.
@rvalue RT_ERR_INPUT | Invalid input value.
@comm
    The API can turn on/off leds of dedicated port while indicated information configuration of LED group is set to force mode.
 */
ret_t rtl8370_setAsicLedGroupEnable(uint32 group, uint32 portmask)
{
    ret_t retVal;
    uint32 regAddr;
    uint32 regDataMask;

    if ( group > RTL8370_LEDGROUPMAX )
        return RT_ERR_INPUT;

    if ( portmask > 0xFF )
        return RT_ERR_INPUT;

    regAddr = RTL8370_REG_PARA_LED_IO_EN1 + group/2;

    regDataMask = 0xFF << ((group%2)*8);

    if ((retVal = rtl8370_setAsicRegBits(regAddr, regDataMask, portmask))!=RT_ERR_OK)
        return retVal;

    return RT_ERR_OK;
}


int RTL8370_init(void)
{
    rtk_portmask_t portmask;
    unsigned int ret, regData;
 
    /* Set external interface 0 to RGMII with Force mode, 1000M, Full-duple, enable TX&RX pause*/
    rtk_port_mac_ability_t mac_cfg;
    rtk_mode_ext_t mode ;
 
    /* Initial Chip */
    ret = rtk_switch_init();
 
    /* Enable LED Group 0&1 from P0 to P7 */
    portmask.bits[0]=0x0FF;
    rtk_led_enable_set(LED_GROUP_0, portmask);
    //rtk_led_enable_set(LED_GROUP_1, portmask);

 	/*
	    port 0 ~ 7: Giga MAC/PHY  (as LAN)
    	MAC8 <--> GMAC 0 <--> rtk_port_macForceLinkExt1_set  (Ext1) <--> 8211E (as WAN)
	    MAC9 <--> GMAC 1 <--> rtk_port_macForceLinkExt0_set  (Ext0) <--> 8197DN port 0
 	*/
    mode = MODE_EXT_RGMII ;
    mac_cfg.forcemode = MAC_FORCE;
    mac_cfg.speed = SPD_1000M;
    mac_cfg.duplex = FULL_DUPLEX;
    mac_cfg.link = PORT_LINKUP;
    mac_cfg.nway = DISABLED;
    mac_cfg.txpause = ENABLED;
    mac_cfg.rxpause = ENABLED;
    rtk_port_macForceLinkExt0_set(mode,&mac_cfg);
 
    /* Set RGMII Interface 0 TX delay to 2ns and RX to step 4 */
    // set the tx/rx delay in 8197D site
//    rtk_port_rgmiiDelayExt1_set(0, 2); // change rxDelay to 2 to enhance the compatibility of 8197D and 8367RB
    rtk_port_rgmiiDelayExt0_set(0, 2);

	// for port 8, 8211E port
    mode = MODE_EXT_RGMII ;
    mac_cfg.forcemode = MAC_FORCE;
    mac_cfg.speed = SPD_1000M;
    mac_cfg.duplex = FULL_DUPLEX;
    mac_cfg.link = PORT_LINKUP;
    mac_cfg.nway = DISABLED;
    mac_cfg.txpause = ENABLED;
    mac_cfg.rxpause = ENABLED;
    rtk_port_macForceLinkExt1_set(mode,&mac_cfg);
 
    rtk_port_rgmiiDelayExt1_set(1, 2);
 
    /* set port 9 as CPU port */
    rtk_cpu_enable_set(ENABLE);
    rtk_cpu_tagPort_set(RTK_EXT_0_MAC, CPU_INSERT_TO_NONE);

	// for LED
    rtl8370_setAsicReg(0x1b00, 0x1497);
    rtl8370_setAsicReg(0x1b24, 0x0);
    rtl8370_setAsicReg(0x1b25, 0x0);
    rtl8370_setAsicReg(0x1b26, 0x3);
    rtl8370_setAsicReg(0x1b03, 0x222);
	
//    rtl8370_setAsicReg(RTL8370_REG_UNUCAST_FLOADING_PMSK, 0xff);
//    rtl8370_setAsicReg(RTL8370_REG_UNMCAST_FLOADING_PMSK, 0xff);
//    rtl8370_setAsicReg(RTL8370_REG_BCAST_FLOADING_PMSK, 0xff);

    /* clear bit 2 of reg. RTL8370_REG_EXT0_RGMXF  */
//    ret = rtl8370_getAsicReg(RTL8370_REG_EXT0_RGMXF, &regData); 
//    if(ret==RT_ERR_OK)
//        rtl8370_setAsicReg(RTL8370_REG_EXT0_RGMXF, (regData & ~0x4));

    //prom_printf("  ==> RTL8370 initialized.\n"); 
    return ret; 
}


/* Function Name:
 *      rtk_led_enable_set
 * Description:
 *      Set Led parallel mode enable congiuration
 * Input:
 *      group - LED group id.
 *      portmask - LED enable port mask.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK              - OK
 *      RT_ERR_FAILED          - FAILED
 *      RT_ERR_SMI             - SMI access error
 *      RT_ERR_INPUT - Invalid input parameters.
 * Note:
 *      The API can be used to enable LED parallel mode per port per group. 
 */

rtk_api_ret_t rtk_led_enable_set(rtk_led_group_t group, rtk_portmask_t portmask)
{
    rtk_api_ret_t retVal;

    if (group >= LED_GROUP_END)
        return RT_ERR_INPUT;

    if (portmask.bits[0] > (1<<RTK_PHY_ID_MAX))
        return RT_ERR_INPUT;  

    if ((retVal = rtl8370_setAsicLedGroupEnable(group, portmask.bits[0]))!=RT_ERR_OK)        
        return retVal;    

    return RT_ERR_OK;
}




