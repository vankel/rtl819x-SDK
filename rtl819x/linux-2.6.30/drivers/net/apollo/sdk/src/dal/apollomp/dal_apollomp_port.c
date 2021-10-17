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
 * $Revision: 16303 $
 * $Date: 2011-03-16 15:22:24 +0800 (?Ÿæ?ä¸? 16 ä¸‰æ? 2011) $
 *
 * Purpose : Definition of Port API
 *
 * Feature : The file have include the following module and sub-modules
 *           (1) Parameter settings for the port-based view
 *           (2) RTCT
 */




/*
 * Include Files
 */
#include <common/rt_type.h>
#include <dal/apollomp/dal_apollomp.h>
#include <rtk/port.h>
#include <dal/apollomp/dal_apollomp_port.h>
#include <dal/apollomp/raw/apollomp_raw_port.h>
#include <osal/lib.h>
#include <hal/common/miim.h>

/*
 * Symbol Definition
 */

typedef struct dal_apollomp_phy_info_s {
    uint8   force_mode_speed[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_duplex[RTK_MAX_NUM_OF_PORTS];
    uint8   force_mode_flowControl[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_pause[RTK_MAX_NUM_OF_PORTS];
    uint8   auto_mode_asy_pause[RTK_MAX_NUM_OF_PORTS];
} dal_apollomp_phy_info_t;

/*
 * Data Declaration
 */
static uint32    port_init = INIT_NOT_COMPLETED;

static dal_apollomp_phy_info_t   phy_info;

/*
 * Function Declaration
 */

/* Function Name:
 *      dal_apollomp_port_init
 * Description:
 *      Initialize port module of the specified device.
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      Module must be initialized before using all of APIs in this module
 */
int32
dal_apollomp_port_init(void)
{
    int32 ret;
    rtk_port_t port;
    rtk_portmask_t allPortmask;
    rtk_portmask_t allExtPortmask;
    rtk_port_phy_ability_t phy_ability;
    apollomp_raw_port_ability_t mac_ability;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    port_init = INIT_COMPLETED;

    osal_memset(&phy_info, 0x00, sizeof(dal_apollomp_phy_info_t));

    HAL_GET_ALL_PORTMASK(allPortmask);
    HAL_GET_ALL_EXT_PORTMASK(allExtPortmask);
    RTK_PORTMASK_PORT_CLEAR(allExtPortmask, HAL_GET_EXT_CPU_PORT());

    phy_ability.Half_10     = ENABLED;
    phy_ability.Full_10     = ENABLED;
    phy_ability.Half_100    = ENABLED;
    phy_ability.Full_100    = ENABLED;
    phy_ability.Half_1000   = ENABLED;
    phy_ability.Full_1000   = ENABLED;
    phy_ability.FC          = ENABLED;
    phy_ability.AsyFC       = ENABLED;

    HAL_SCAN_ALL_PORT(port)
    {
        if(HAL_IS_PHY_EXIST(port))
        {
            if((ret = dal_apollomp_port_phyAutoNegoAbility_set(port, &phy_ability)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_apollomp_port_phyAutoNegoEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_apollomp_port_greenEnable_set(port, ENABLED)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = dal_apollomp_port_phyCrossOverMode_set(port, PORT_CROSSOVER_MODE_AUTO)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if((ret = dal_apollomp_port_macRemoteLoopbackEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_macLocalLoopbackEnable_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_adminEnable_set(port, ENABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_specialCongest_set(port, 0)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_isolationIpmcLeaky_set(port, DISABLED)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_isolationEntry_set(RTK_PORT_ISO_CFG_0, port, &allPortmask, ENABLED, &allExtPortmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_isolationEntry_set(RTK_PORT_ISO_CFG_1, port, &allPortmask, ENABLED, &allExtPortmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        /* Force CPU port */
        if(port == HAL_GET_CPU_PORT())
        {
            osal_memset(&mac_ability, 0x00, sizeof(apollomp_raw_port_ability_t));
            mac_ability.speed           = PORT_SPEED_1000M;
            mac_ability.duplex          = PORT_FULL_DUPLEX;
        	mac_ability.linkFib1g       = DISABLED;
        	mac_ability.linkStatus      = PORT_LINKDOWN;
        	mac_ability.txFc            = DISABLED;
        	mac_ability.rxFc            = DISABLED;
        	mac_ability.nwayAbility     = DISABLED;
        	mac_ability.masterMod       = DISABLED;
        	mac_ability.nwayFault       = DISABLED;
        	mac_ability.lpi_100m        = DISABLED;
        	mac_ability.lpi_giga        = DISABLED;
            if((ret = apollomp_raw_port_ForceAbility_set(port, &mac_ability)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            mac_ability.linkStatus      = PORT_LINKUP;
            if((ret = apollomp_raw_port_ForceAbility_set(port, &mac_ability)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }

            data |= (0x01 << port);

            if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
            {
                RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
                port_init = INIT_NOT_COMPLETED;
                return ret;
            }
        }
    }

    HAL_SCAN_ALL_EXT_PORT_EX_CPU(port)
    {
        if((ret = dal_apollomp_port_isolationEntryExt_set(RTK_PORT_ISO_CFG_0, port, &allPortmask, ENABLED, &allExtPortmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }

        if((ret = dal_apollomp_port_isolationEntryExt_set(RTK_PORT_ISO_CFG_1, port, &allPortmask, ENABLED, &allExtPortmask)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            port_init = INIT_NOT_COMPLETED;
            return ret;
        }
    }

    if((ret = dal_apollomp_port_isolationCtagPktConfig_set(RTK_PORT_ISO_CFG_0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        port_init = INIT_NOT_COMPLETED;
        return ret;
    }

    if((ret = dal_apollomp_port_isolationL34PktConfig_set(RTK_PORT_ISO_CFG_0)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        port_init = INIT_NOT_COMPLETED;
        return ret;
    }


    return RT_ERR_OK;
} /* end of dal_apollomp_port_init */

/* Module Name    : Port                                       */
/* Sub-module Name: Parameter settings for the port-based view */

/* Function Name:
 *      dal_apollomp_port_link_get
 * Description:
 *      Get the link status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pStatus - pointer to the link status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      The link status of the port is as following:
 *      - LINKDOWN
 *      - LINKUP
 */
int32
dal_apollomp_port_link_get(rtk_port_t port, rtk_port_linkStatus_t *pStatus)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_STATUSf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    *pStatus = ((data == 1) ? PORT_LINKUP : PORT_LINKDOWN);
    return RT_ERR_OK;
} /* end of dal_apollomp_port_link_get */

/* Function Name:
 *      dal_apollomp_port_speedDuplex_get
 * Description:
 *      Get the negotiated port speed and duplex status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSpeed  - pointer to the port speed
 *      pDuplex - pointer to the port duplex
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      (1) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M
 *
 *      (2) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
int32
dal_apollomp_port_speedDuplex_get(
    rtk_port_t        port,
    rtk_port_speed_t  *pSpeed,
    rtk_port_duplex_t *pDuplex)
{
    int32  ret;
    uint32 speed;
    uint32 duplex;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_LINK_SPDf, &speed)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_DUPLEXf, &duplex)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    switch(speed)
    {
        case 0:
            *pSpeed = PORT_SPEED_10M;
            break;
        case 1:
            *pSpeed = PORT_SPEED_100M;
            break;
        case 2:
            *pSpeed = PORT_SPEED_1000M;
            break;
        default:
            return RT_ERR_FAILED;
            break;
    }

    *pDuplex = ((1 == duplex) ? PORT_FULL_DUPLEX : PORT_HALF_DUPLEX);
    return RT_ERR_OK;
} /* end of dal_apollomp_port_speedDuplex_get */

/* Function Name:
 *      dal_apollomp_port_flowctrl_get
 * Description:
 *      Get the negotiated flow control status of the specific port
 * Input:
 *      port      - port id
 * Output:
 *      pTxStatus - pointer to the negotiation result of the Tx flow control
 *      pRxStatus - pointer to the negotiation result of the Rx flow control
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 *      RT_ERR_PORT_LINKDOWN - link down port status
 * Note:
 *      None
 */
int32
dal_apollomp_port_flowctrl_get(
    rtk_port_t  port,
    uint32      *pTxStatus,
    uint32      *pRxStatus)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pTxStatus), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pRxStatus), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_TX_FCf, pTxStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if ((ret = reg_array_field_read(APOLLOMP_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_P_RX_FCf, pRxStatus)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_flowctrl_get */

/* Function Name:
 *      dal_apollomp_port_phyAutoNegoEnable_get
 * Description:
 *      Get PHY ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to PHY auto negotiation status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_phyAutoNegoEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32  ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = phy_autoNegoEnable_get(port, pEnable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyAutoNegoEnable_get */

/* Function Name:
 *      dal_apollomp_port_phyAutoNegoEnable_set
 * Description:
 *      Set PHY ability of the specific port
 * Input:
 *      port   - port id
 *      enable - enable PHY auto negotiation
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - input parameter out of range
 * Note:
 *      - ENABLED : switch to PHY auto negotiation mode
 *      - DISABLED: switch to PHY force mode
 *      - Once the abilities of both auto-nego and force mode are set,
 *        you can freely swtich the mode without calling ability setting API again
 */
int32
dal_apollomp_port_phyAutoNegoEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32  ret;
    rtk_port_phy_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if (ENABLED == enable)
    {
        if ((ret = phy_autoNegoAbility_get(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        ability.FC = phy_info.auto_mode_pause[port];
        ability.AsyFC = phy_info.auto_mode_asy_pause[port];

        if ((ret = phy_autoNegoAbility_set(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    if ((ret = phy_autoNegoEnable_set(port, enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if (DISABLED == enable)
    {
        if ((ret = phy_duplex_set(port, phy_info.force_mode_duplex[port])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_speed_set(port, phy_info.force_mode_speed[port])) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_autoNegoAbility_get(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        ability.FC = phy_info.force_mode_flowControl[port];
        ability.AsyFC = phy_info.force_mode_flowControl[port];

        if ((ret = phy_autoNegoAbility_set(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyAutoNegoEnable_set */

/* Function Name:
 *      dal_apollomp_port_phyAutoNegoAbility_get
 * Description:
 *      Get PHY auto negotiation ability of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pAbility - pointer to the PHY ability
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_phyAutoNegoAbility_get(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    int32  ret;
    rtk_enable_t enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    if ((ret = dal_apollomp_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    if ((ret = phy_autoNegoAbility_get(port, pAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if (DISABLED == enable)
    {
        /* Force mode currently, return database status */
        pAbility->FC    = phy_info.auto_mode_pause[port];
        pAbility->AsyFC = phy_info.auto_mode_asy_pause[port];
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyAutoNegoAbility_get */

/* Function Name:
 *      dal_apollomp_port_phyAutoNegoAbility_set
 * Description:
 *      Set PHY auto negotiation ability of the specific port
 * Input:
 *      port     - port id
 *      pAbility - pointer to the PHY ability
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      You can set these abilities no matter which mode PHY currently stays on
 */
int32
dal_apollomp_port_phyAutoNegoAbility_set(
    rtk_port_t              port,
    rtk_port_phy_ability_t  *pAbility)
{
    int32   ret;
    rtk_enable_t enable;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pAbility), RT_ERR_NULL_POINTER);

    if ((ret = dal_apollomp_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "dal_cypress_port_phyAutoNegoEnable_get(port=%d) failed!!", port);
        return ret;
    }

    if (DISABLED == enable)
    {
        /* PHY is in force mode currently, keep user configuration and set Force ability */
        phy_info.auto_mode_pause[port] = pAbility->FC;
        phy_info.auto_mode_asy_pause[port] = pAbility->AsyFC;
        pAbility->FC = phy_info.force_mode_flowControl[port];
        pAbility->AsyFC = phy_info.force_mode_flowControl[port];
    }

    if ((ret = phy_autoNegoAbility_set(port, pAbility)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT | MOD_DAL), "");
        return ret;
    }

    if (ENABLED == enable)
    {
        /* PHY is in auto-nego mode, store user configuration to database */
        phy_info.auto_mode_pause[port] = pAbility->FC;
        phy_info.auto_mode_asy_pause[port] = pAbility->AsyFC;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyAutoNegoAbility_set */

/* Function Name:
 *      dal_apollomp_port_phyForceModeAbility_get
 * Description:
 *      Get PHY ability status of the specific port
 * Input:
 *      port         - port id
 * Output:
 *      pSpeed       - pointer to the port speed
 *      pDuplex      - pointer to the port duplex
 *      pFlowControl - pointer to the flow control enable status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None.
 */
int32
dal_apollomp_port_phyForceModeAbility_get(
    rtk_port_t          port,
    rtk_port_speed_t    *pSpeed,
    rtk_port_duplex_t   *pDuplex,
    rtk_enable_t        *pFlowControl)
{
    int32   ret;
    rtk_enable_t enable;
    rtk_port_phy_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSpeed), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pDuplex), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pFlowControl), RT_ERR_NULL_POINTER);

    if ((ret = dal_apollomp_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    if (ENABLED == enable)
    {
        *pSpeed = phy_info.force_mode_speed[port];
        *pDuplex = phy_info.force_mode_duplex[port];
        *pFlowControl = phy_info.force_mode_flowControl[port];
    }
    else
    {
        if ((ret = phy_speed_get(port, pSpeed)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_duplex_get(port, pDuplex)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_autoNegoAbility_get(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        *pFlowControl = ability.FC;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyForceModeAbility_get */

/* Function Name:
 *      dal_apollomp_port_phyForceModeAbility_set
 * Description:
 *      Set the port speed/duplex mode/pause/asy_pause in the PHY force mode
 * Input:
 *      port        - port id
 *      speed       - port speed
 *      duplex      - port duplex mode
 *      flowControl - enable flow control
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID    - invalid port id
 *      RT_ERR_PHY_SPEED  - invalid PHY speed setting
 *      RT_ERR_PHY_DUPLEX - invalid PHY duplex setting
 *      RT_ERR_INPUT      - invalid input parameter
 * Note:
 *      (1) You can set these abilities no matter which mode PHY stays currently.
 *          However, these abilities only take effect when the PHY is in Force mode
 *
 *      (2) The speed type of the port is as following:
 *          - PORT_SPEED_10M
 *          - PORT_SPEED_100M
 *          - PORT_SPEED_1000M
 *
 *      (3) The duplex mode of the port is as following:
 *          - HALF_DUPLEX
 *          - FULL_DUPLEX
 */
int32
dal_apollomp_port_phyForceModeAbility_set(
    rtk_port_t          port,
    rtk_port_speed_t    speed,
    rtk_port_duplex_t   duplex,
    rtk_enable_t        flowControl)
{

    int32   ret;
    rtk_enable_t enable;
    rtk_port_phy_ability_t ability;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_SPEED_END <= speed), RT_ERR_INPUT);
    RT_PARAM_CHK((PORT_DUPLEX_END <= duplex), RT_ERR_INPUT);
    RT_PARAM_CHK((RTK_ENABLE_END <= flowControl), RT_ERR_INPUT);

    if ((ret = dal_apollomp_port_phyAutoNegoEnable_get(port, &enable)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL | MOD_PORT), "");
        return ret;
    }

    phy_info.force_mode_speed[port] = speed;
    phy_info.force_mode_duplex[port] = duplex;
    phy_info.force_mode_flowControl[port] = flowControl;

    if (DISABLED == enable)
    {
        if ((ret = phy_speed_set(port, speed)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_duplex_set(port, duplex)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if ((ret = phy_autoNegoAbility_get(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if (ENABLED == flowControl)
        {
            ability.FC = ENABLED;
            ability.AsyFC = ENABLED;
        }
        else
        {
            ability.FC = DISABLED;
            ability.AsyFC = DISABLED;
        }

        if ((ret = phy_autoNegoAbility_set(port, &ability)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyForceModeAbility_set */

/* Function Name:
 *      dal_apollomp_port_phyReg_get
 * Description:
 *      Get PHY register data of the specific port
 * Input:
 *      port  - port id
 *      page  - page id
 *      reg   - reg id
 * Output:
 *      pData - pointer to the PHY reg data
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_PAGE_ID  - invalid page id
 *      RT_ERR_PHY_REG_ID   - invalid reg id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_phyReg_get(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              *pData)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK(((page < HAL_MIIM_PAGE_ID_MIN()) && page != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((NULL == pData), RT_ERR_NULL_POINTER);

    /* Check Link status */
    ret = hal_miim_read(port, page, reg, pData);
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT), "pData=0x%x", *pData);

    return ret;
} /* end of dal_apollomp_port_phyReg_get */

/* Function Name:
 *      dal_apollomp_port_phyReg_set
 * Description:
 *      Set PHY register data of the specific port
 * Input:
 *      port - port id
 *      page - page id
 *      reg  - reg id
 *      data - reg data
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID     - invalid port id
 *      RT_ERR_PHY_PAGE_ID - invalid page id
 *      RT_ERR_PHY_REG_ID  - invalid reg id
 * Note:
 *      None
 */
int32
dal_apollomp_port_phyReg_set(
    rtk_port_t          port,
    uint32              page,
    rtk_port_phy_reg_t  reg,
    uint32              data)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((page > HAL_MIIM_PAGE_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK(((page < HAL_MIIM_PAGE_ID_MIN()) && page != 0), RT_ERR_INPUT);
    RT_PARAM_CHK((reg > HAL_MIIM_REG_ID_MAX()), RT_ERR_INPUT);
    RT_PARAM_CHK((data > HAL_MIIM_DATA_MAX()), RT_ERR_INPUT);

    ret = hal_miim_write(port, page, reg, data);

    return ret;
} /* end of dal_apollomp_port_phyReg_set */

/* Function Name:
 *      dal_apollomp_port_phyMasterSlave_get
 * Description:
 *      Get PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 * Output:
 *      pMasterSlave        - pointer to the PHY master slave configuration
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID       - invalid port id
 *      RT_ERR_NULL_POINTER  - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_phyMasterSlave_get(
    rtk_port_t          port,
    rtk_port_masterSlave_t   *pMasterSlave)
{
    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMasterSlave), RT_ERR_NULL_POINTER);

    *pMasterSlave = PORT_AUTO_MODE;
    return RT_ERR_OK;
}/* end of dal_apollomp_port_phyMasterSlave_get */

/* Function Name:
 *      dal_apollomp_port_phyMasterSlave_set
 * Description:
 *      Set PHY configuration of master/slave mode of the specific port
 * Input:
 *      port                - port id
 *      masterSlave         - PHY master slave configuration
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_INPUT        - RT_ERR_INPUT
 * Note:
 *      None
 */
int32
dal_apollomp_port_phyMasterSlave_set(
    rtk_port_t          port,
    rtk_port_masterSlave_t   masterSlave)
{
    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(masterSlave >= PORT_MASTER_SLAVE_END, RT_ERR_INPUT);


    return RT_ERR_OK;
}/* end of dal_apollomp_port_phyMasterSlave_set */

/* Function Name:
 *      dal_apollomp_port_cpuPortId_get
 * Description:
 *      Get CPU port id
 * Input:
 *      None
 * Output:
 *      pPort - pointer to CPU port id
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_cpuPortId_get(rtk_port_t *pPort)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPort), RT_ERR_NULL_POINTER);

    *pPort = (rtk_port_t)HAL_GET_CPU_PORT();
    return RT_ERR_OK;
} /* end of dal_apollomp_port_cpuPortId_get */

/* Function Name:
 *      dal_apollomp_port_isolationEntry_get
 * Description:
 *      Get Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 * Output:
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pIncludeCpu     - Include CPU or not
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationEntry_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, uint32 *pIncludeCpu, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIncludeCpu), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    regAddr = (RTK_PORT_ISO_CFG_0 == mode) ? APOLLOMP_PISO_P_MODE0_CTRLr : APOLLOMP_PISO_P_MODE1_CTRLr;
    if((ret = reg_array_field_read(regAddr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    pPortmask->bits[0]    = regData & 0x007F;
    *pIncludeCpu          = (regData & 0x0080) >> 7;
    pExtPortmask->bits[0] = (regData & 0x1F00) >> 8;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationEntry_get */

/* Function Name:
 *      dal_apollomp_port_isolationEntry_set
 * Description:
 *      Set Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      includeCpu      - Include CPU or not
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationEntry_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, uint32 includeCpu, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK((includeCpu >= RTK_ENABLE_END), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    regAddr = (RTK_PORT_ISO_CFG_0 == mode) ? APOLLOMP_PISO_P_MODE0_CTRLr : APOLLOMP_PISO_P_MODE1_CTRLr;
    regData = ((pExtPortmask->bits[0] << 8) | (includeCpu << 7) | (pPortmask->bits[0]));
    if((ret = reg_array_field_write(regAddr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationEntry_set */

/* Function Name:
 *      dal_apollomp_port_isolationEntryExt_get
 * Description:
 *      Get Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 * Output:
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      pIncludeCpu     - Include CPU or not
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationEntryExt_get(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, uint32 *pIncludeCpu, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pIncludeCpu), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK((NULL == pExtPortmask), RT_ERR_NULL_POINTER);

    regAddr = (RTK_PORT_ISO_CFG_0 == mode) ? APOLLOMP_PISO_EXT_MODE0_CTRLr : APOLLOMP_PISO_EXT_MODE1_CTRLr;
    if((ret = reg_array_field_read(regAddr, REG_ARRAY_INDEX_NONE, (int32)port, APOLLOMP_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    pPortmask->bits[0]    = regData & 0x007F;
    *pIncludeCpu          = (regData & 0x0080) >> 7;
    pExtPortmask->bits[0] = (regData & 0x1F00) >> 8;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationEntryExt_get */

/* Function Name:
 *      dal_apollomp_port_isolationEntryExt_set
 * Description:
 *      Set Port isolation portmask
 * Input:
 *      mode            - Configuration 0 or 1
 *      port            - Ingress port
 *      pPortmask       - Isolation portmask for specified ingress port.
 *      includeCpu      - Include CPU or not
 *      pExtPortmask    - Isolation extension portmask for specified ingress port.
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationEntryExt_set(rtk_port_isoConfig_t mode, rtk_port_t port, rtk_portmask_t *pPortmask, uint32 includeCpu, rtk_portmask_t *pExtPortmask)
{
    int32 ret;
    uint32 regAddr;
    uint32 regData;

    RT_PARAM_CHK(mode >= RTK_PORT_ISO_CFG_END, RT_ERR_INPUT);
    RT_PARAM_CHK(!HAL_IS_EXT_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(!HAL_IS_PORTMASKPRT_VALID(pPortmask), RT_ERR_PORT_ID);
    RT_PARAM_CHK((includeCpu >= RTK_ENABLE_END), RT_ERR_NULL_POINTER);
    RT_PARAM_CHK(!HAL_IS_EXTPORTMASK_VALID((*pExtPortmask)), RT_ERR_PORT_ID);

    regAddr = (RTK_PORT_ISO_CFG_0 == mode) ? APOLLOMP_PISO_EXT_MODE0_CTRLr : APOLLOMP_PISO_EXT_MODE1_CTRLr;
    regData = ((pExtPortmask->bits[0] << 8) | (includeCpu << 7) | (pPortmask->bits[0]));
    if((ret = reg_array_field_write(regAddr, REG_ARRAY_INDEX_NONE, (int32)port, APOLLOMP_PORTMASKf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationEntryExt_set */

/* Function Name:
 *      dal_apollomp_port_isolationCtagPktConfig_get
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      None
 * Output:
 *      pMode   - Isolation configuration selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationCtagPktConfig_get(rtk_port_isoConfig_t *pMode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_PISO_CTRLr, APOLLOMP_CTAG_SELf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pMode = (1 == regData) ? RTK_PORT_ISO_CFG_1 : RTK_PORT_ISO_CFG_0;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationCtagPktConfig_get */

/* Function Name:
 *      dal_apollomp_port_isolationCtagPktConfig_set
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      mode    - Isolation configuration selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationCtagPktConfig_set(rtk_port_isoConfig_t mode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK((mode >= RTK_PORT_ISO_CFG_END), RT_ERR_INPUT);

    regData = (uint32)mode;
    if((ret = reg_field_write(APOLLOMP_PISO_CTRLr, APOLLOMP_CTAG_SELf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationCtagPktConfig_set */

/* Function Name:
 *      dal_apollomp_port_isolationL34PktConfig_get
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      None
 * Output:
 *      pMode   - Isolation configuration selection
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NULL_POINTER
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationL34PktConfig_get(rtk_port_isoConfig_t *pMode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_PISO_CTRLr, APOLLOMP_L34_SELf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pMode = (1 == regData) ? RTK_PORT_ISO_CFG_1 : RTK_PORT_ISO_CFG_0;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationL34PktConfig_get */

/* Function Name:
 *      dal_apollomp_port_isolationL34PktConfig_set
 * Description:
 *      Isolation configuration selection for ingress Ctag packets
 * Input:
 *      mode    - Isolation configuration selection
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None.
 */
int32
dal_apollomp_port_isolationL34PktConfig_set(rtk_port_isoConfig_t mode)
{
    int32 ret;
    uint32 regData;

    RT_PARAM_CHK((mode >= RTK_PORT_ISO_CFG_END), RT_ERR_INPUT);

    regData = (uint32)mode;
    if((ret = reg_field_write(APOLLOMP_PISO_CTRLr, APOLLOMP_L34_SELf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationL34PktConfig_set */


/* Function Name:
 *      dal_apollomp_port_isolationIpmcLeaky_get
 * Description:
 *      Get the ip multicast leaky state of the port isolation
 * Input:
 *      port      - port id
 * Output:
 *      pEnable   - status of port isolation leaky for ip multicast packets
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_apollomp_port_isolationIpmcLeaky_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if ((ret = reg_array_field_read(APOLLOMP_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)val;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationIpmcLeaky_get */

/* Function Name:
 *      dal_apollomp_port_isolationIpmcLeaky_set
 * Description:
 *      Set the ip multicast leaky state of the port isolation
 * Input:
 *      port      - port id
 *      enable    - status of port isolation leaky for ip multicast packets
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
dal_apollomp_port_isolationIpmcLeaky_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  val;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    val = (uint32)enable;
    if ((ret = reg_array_field_write(APOLLOMP_L2_IPMC_ISO_LEAKYr, port, REG_ARRAY_INDEX_NONE, APOLLOMP_ENf, &val)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_PORT|MOD_DAL), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_isolationIpmcLeaky_set */


/* Function Name:
 *      dal_apollomp_port_macRemoteLoopbackEnable_get
 * Description:
 *      Get the mac remote loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of mac remote loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac remote loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Remote loopback is used to loopback packet RX to switch core back to the outer interface.
 */
int32
dal_apollomp_port_macRemoteLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_apollomp_port_macRemoteLoopbackEnable_get */

/* Function Name:
 *      dal_apollomp_port_macRemoteLoopbackEnable_set
 * Description:
 *      Set the mac remote loopback enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of mac remote loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac remote loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Remote loopback is used to loopback packet RX to switch core back to the outer interface.
 */
int32
dal_apollomp_port_macRemoteLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    return RT_ERR_OK;
} /* end of dal_apollomp_port_macRemoteLoopbackEnable_set */

/* Function Name:
 *      dal_apollomp_port_macLocalLoopbackEnable_get
 * Description:
 *      Get the mac local loopback enable status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the enable status of mac local loopback
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac local loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Local loopback is used to loopback packet TX from switch core back to switch core.
 */
int32
dal_apollomp_port_macLocalLoopbackEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(APOLLOMP_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_MAC_LOOPBACKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEnable = (rtk_enable_t)data ;
    return RT_ERR_OK;
} /* end of dal_apollomp_port_macLocalLoopbackEnable_get */

/* Function Name:
 *      dal_apollomp_port_macLocalLoopbackEnable_set
 * Description:
 *      Set the mac local loopback enable status of the specific port
 * Input:
 *      port   - port id
 *      enable - enable status of mac local loopback
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT - The module is not initial
 *      RT_ERR_PORT_ID  - invalid port id
 *      RT_ERR_INPUT    - invalid input parameter
 * Note:
 *      (1) The mac local loopback enable status of the port is as following:
 *          - DISABLE
 *          - ENABLE
 *      (2) Local loopback is used to loopback packet TX from switch core back to switch core.
 */
int32
dal_apollomp_port_macLocalLoopbackEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32  data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    data = (uint32)enable;
    if((ret = reg_array_field_write(APOLLOMP_P_MISCr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_MAC_LOOPBACKf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_macLocalLoopbackEnable_set */

/* Function Name:
 *      dal_apollomp_port_adminEnable_get
 * Description:
 *      Get port admin status of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to the port admin status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_adminEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
    int32   ret;
    uint32 data;
    uint32 link;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(data & (0x01 << port) )
    {
        if((ret = reg_array_field_read(APOLLOMP_FORCE_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_LINK_ABLTYf, &link)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }

        if(PORT_LINKDOWN == link)
            *pEnable = DISABLED;
        else
            *pEnable = ENABLED;
    }
    else
        *pEnable = ENABLED;

    return RT_ERR_OK;
} /* end of dal_apollomp_port_adminEnable_get */

/* Function Name:
 *      dal_apollomp_port_adminEnable_set
 * Description:
 *      Set port admin status of the specific port
 * Input:
 *      port    - port id
 *      enable  - port admin status
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      None
 */
int32
dal_apollomp_port_adminEnable_set(rtk_port_t port, rtk_enable_t enable)
{
    int32   ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    if((ret = reg_field_read(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(DISABLED == enable)
        data |= (0x01 << port);
    else
        data &= ~(0x01 << port);

    if((ret = reg_field_write(APOLLOMP_ABLTY_FORCE_MODEr, APOLLOMP_ABLTY_FORCE_MODEf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(DISABLED == enable)
    {
        data = 0;
        if((ret = reg_array_field_write(APOLLOMP_FORCE_P_ABLTYr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_LINK_ABLTYf, &data)) != RT_ERR_OK)
        {
            RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
            return ret;
        }
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_adminEnable_set */

/* Function Name:
 *      dal_apollomp_port_specialCongest_get
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pSecond - congest timer (seconds)
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_specialCongest_get(rtk_port_t port, uint32 *pSecond)
{
    int32   ret;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pSecond), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(APOLLOMP_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_CGST_SUST_TMR_LMTf, pSecond)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_specialCongest_get */

/* Function Name:
 *      dal_apollomp_port_specialCongest_set
 * Description:
 *      Set the congest seconds of the specific port
 * Input:
 *      port   - port id
 *      second - congest timer (seconds)
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_port_specialCongest_set(rtk_port_t port, uint32 second)
{
    int32  ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((second > HAL_MAX_SPECIAL_CONGEST_SEC()), RT_ERR_INPUT);

    data = second;
    if((ret = reg_array_field_write(APOLLOMP_SC_P_CTRL_0r, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_CGST_SUST_TMR_LMTf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_specialCongest_set */

/* Function Name:
 *      dal_apollomp_port_specialCongestStatus_get
 * Description:
 *      Get the congest status of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      pStatus - Congest status
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_port_specialCongestStatus_get(rtk_port_t port, uint32 *pStatus)
{
    int32  ret;
    uint32 regData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pStatus), RT_ERR_NULL_POINTER);

    if((ret = reg_field_read(APOLLOMP_SC_P_CTRL_1r, APOLLOMP_CGST_INDf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    if(regData & (0x01 << port))
        *pStatus = 1;
    else
        *pStatus = 0;

    return RT_ERR_OK;
}

/* Function Name:
 *      dal_apollomp_port_specialCongestStatus_get
 * Description:
 *      Get the congest status of the specific port
 * Input:
 *      port   - port id
 * Output:
 *      None.
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      None
 */
int32
dal_apollomp_port_specialCongestStatus_clear(rtk_port_t port)
{
    int32  ret;
    uint32 regData;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);

    regData = (0x01 << port);
    if((ret = reg_field_write(APOLLOMP_SC_P_CTRL_1r, APOLLOMP_CGST_INDf, &regData)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
}

/* Function Name:
 *      rtk_port_greenEnable_get
 * Description:
 *      Get the statue of green feature of the specific port
 * Input:
 *      port    - port id
 * Output:
 *      pEnable - pointer to status of green feature
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_greenEnable_get(rtk_port_t port, rtk_enable_t *pEnable)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEnable), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_apollomp_port_greenEnable_get */

/* Function Name:
 *      dal_apollomp_port_greenEnable_set
 * Description:
 *      Set the statue of green feature of the specific port
 * Input:
 *      port   - port id
 *      enable - status of green feature
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 * Note:
 *      None
 */
int32
dal_apollomp_port_greenEnable_set(rtk_port_t port, rtk_enable_t enable)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((RTK_ENABLE_END <= enable), RT_ERR_INPUT);

    return RT_ERR_OK;
} /* end of dal_apollomp_port_greenEnable_set */


/* Function Name:
 *      dal_apollomp_port_phyCrossOverMode_get
 * Description:
 *      Get cross over mode in the specified port.
 * Input:
 *      port  - port id
 * Output:
 *      pMode - pointer to cross over mode
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
dal_apollomp_port_phyCrossOverMode_get(rtk_port_t port, rtk_port_crossOver_mode_t *pMode)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pMode), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyCrossOverMode_get */

/* Function Name:
 *      dal_apollomp_port_phyCrossOverMode_set
 * Description:
 *      Set cross over mode in the specified port.
 * Input:
 *      port - port id
 *      mode - cross over mode
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID - invalid port id
 *      RT_ERR_INPUT   - invalid input parameter
 * Note:
 *      Following value is valid
 *      - PORT_CROSSOVER_MODE_AUTO
 *      - PORT_CROSSOVER_MODE_MDI
 *      - PORT_CROSSOVER_MODE_MDIX
 */
int32
dal_apollomp_port_phyCrossOverMode_set(rtk_port_t port, rtk_port_crossOver_mode_t mode)
{
#if 0
    int32   ret;
#endif
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((PORT_CROSSOVER_MODE_END <= mode), RT_ERR_INPUT);

    return RT_ERR_OK;
} /* end of dal_apollomp_port_phyCrossOverMode_set */

/* Function Name:
 *      dal_apollomp_port_enhancedFid_get
 * Description:
 *      Get port EFID
 * Input:
 *      port      - port id
 * Output:
 *      pEfid     - EFID
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      none
 */
int32
dal_apollomp_port_enhancedFid_get(rtk_port_t port, rtk_efid_t *pEfid)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pEfid), RT_ERR_NULL_POINTER);

    if((ret = reg_array_field_read(APOLLOMP_L2_EFIDr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_EFIDf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    *pEfid = (rtk_efid_t)data;
    return RT_ERR_OK;
} /* end of dal_apollomp_port_enhancedFid_get */

/* Function Name:
 *      dal_apollomp_port_enhancedFid_set
 * Description:
 *      Set port EFID
 * Input:
 *      port      - port id
 *      efid      - EFID
 * Output:
 *      none
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_PORT_ID      - invalid port id
 * Note:
 *      none
 */
int32
dal_apollomp_port_enhancedFid_set(rtk_port_t port, rtk_efid_t efid)
{
    int32 ret;
    uint32 data;

    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    RT_PARAM_CHK(!HAL_IS_PORT_EXIST(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK(efid > HAL_ENHANCED_FID_MAX(), RT_ERR_INPUT);

    data = (uint32)efid;
    if((ret = reg_array_field_write(APOLLOMP_L2_EFIDr, (int32)port, REG_ARRAY_INDEX_NONE, APOLLOMP_EFIDf, &data)) != RT_ERR_OK)
    {
        RT_ERR(ret, (MOD_DAL|MOD_PORT), "");
        return ret;
    }

    return RT_ERR_OK;
} /* end of dal_apollomp_port_enhancedFid_set */


/* Function Name:
 *      dal_apollomp_port_rtctResult_get
 * Description:
 *      Get test result of RTCT.
 * Input:
 *      port        - the port for retriving RTCT test result
 * Output:
 *      pRtctResult - RTCT result
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_PORT_ID      - invalid port id
 *      RT_ERR_PHY_RTCT_NOT_FINISH   - RTCT not finish. Need to wait a while.
 *      RT_ERR_TIMEOUT      - RTCT test timeout in this port.
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      If linkType is PORT_SPEED_1000M, test result will be stored in ge_result.
 *      If linkType is PORT_SPEED_10M or PORT_SPEED_100M, test result will be stored in fe_result.
 */
int32
dal_apollomp_port_rtctResult_get(rtk_port_t port, rtk_rtctResult_t *pRtctResult)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK(!HAL_IS_ETHER_PORT(port), RT_ERR_PORT_ID);
    RT_PARAM_CHK((NULL == pRtctResult), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /*end of dal_apollomp_port_rtctResult_get*/

/* Function Name:
 *      dal_apollomp_port_rtct_start
 * Description:
 *      Start RTCT for ports.
 *      When enable RTCT, the port won't transmit and receive normal traffic.
 * Input:
 *      pPortmask - the ports for RTCT test
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 *      RT_ERR_NOT_INIT     - The module is not initial
 *      RT_ERR_UNIT_ID      - invalid unit id
 *      RT_ERR_NULL_POINTER - input parameter may be null pointer
 * Note:
 *      None
 */
int32
dal_apollomp_port_rtct_start(rtk_portmask_t *pPortmask)
{
    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_PORT),"%s",__FUNCTION__);

    /* check Init status */
    RT_INIT_CHK(port_init);

    /* parameter check */
    RT_PARAM_CHK((NULL == pPortmask), RT_ERR_NULL_POINTER);

    return RT_ERR_OK;
} /* end of dal_apollomp_port_rtct_start */

