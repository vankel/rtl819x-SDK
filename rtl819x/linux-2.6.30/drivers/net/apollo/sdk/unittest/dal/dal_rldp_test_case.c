#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <rtk/rldp.h>
#include <dal/dal_rldp_test_case.h>
#include <common/unittest_util.h>
#if defined(CONFIG_SDK_APOLLOMP)
#include <dal/apollomp/dal_apollomp_rldp.h>
#endif

static int32 dal_rldp_test_resetConfig(rtk_rldp_config_t *pConfig)
{
    pConfig->rldp_enable = DISABLED;
    pConfig->fc_mode = RTK_RLDP_FLOWCTRL_DROP;
    pConfig->magic.octet[0] = 0;
    pConfig->magic.octet[1] = 0;
    pConfig->magic.octet[2] = 0;
    pConfig->magic.octet[3] = 0;
    pConfig->magic.octet[4] = 0;
    pConfig->magic.octet[5] = 0;
    pConfig->compare_type = RTK_RLDP_CMPTYPE_MAGIC;
    pConfig->interval_check = 0;
    pConfig->num_check = 0;
    pConfig->interval_loop = 0;
    pConfig->num_loop = 0;

    return 0;
}

static int32 dal_rldp_test_resetPortConfig(rtk_rldp_portConfig_t *pPortConfig)
{
    pPortConfig->tx_enable = DISABLED;

    return 0;
}

static int32 dal_rldp_test_resetPortStatus(rtk_rldp_portStatus_t *pPortStatus)
{
    pPortStatus->loop_status = RTK_RLDP_LOOPSTS_NONE;
    pPortStatus->loop_enter = RTK_RLDP_LOOPSTS_NONE;
    pPortStatus->loop_leave = RTK_RLDP_LOOPSTS_NONE;
    pPortStatus->looped_port = 0;

    return 0;
}

int32 dal_rldp_test(uint32 caseNo)
{
    int32 ret;
    rtk_port_t port;
    rtk_rldp_config_t config;
    rtk_rldp_config_t chkConfig;
    rtk_rldp_portConfig_t portConfig;
    rtk_rldp_portConfig_t chkPortConfig;
    rtk_rldp_status_t status;
    rtk_rldp_portStatus_t portStatus;
    rtk_rldp_portStatus_t chkPortStatus;
    rtk_rlpp_trapType_t trapType;

    /* Input Check
     * 1. Check for chip/API support
     * 2. NULL ponter check
     * 3. Range check
     * 4. Set/Get test
     */

    /* Step 1 */
	/* Initial for following test*/
    ret = rtk_rldp_init();
    if(RT_ERR_FEATURE_NOT_SUPPORTED == ret)
    {
        osal_printf("\n %s %d skipped\n",__FUNCTION__,__LINE__);
        return RT_ERR_OK;
    }
    if(RT_ERR_OK != ret)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /* Step 2 */
    if(RT_ERR_OK == rtk_rldp_config_set(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_rldp_config_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_rldp_portConfig_set(HAL_GET_MIN_PORT(), NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_rldp_portConfig_get(HAL_GET_MIN_PORT(), NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_rldp_status_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_rldp_portStatus_get(HAL_GET_MIN_PORT(), NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_rldp_portStatus_clear(HAL_GET_MIN_PORT(), NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_rlpp_trapType_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Step 3 */
    /* System config out of range */
    dal_rldp_test_resetConfig(&config);
    config.rldp_enable = RTK_ENABLE_END;
    if(RT_ERR_OK == rtk_rldp_config_set(&config))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    dal_rldp_test_resetConfig(&config);
    config.fc_mode = RTK_RLDP_FLOWCTRL_END;
    if(RT_ERR_OK == rtk_rldp_config_set(&config))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    dal_rldp_test_resetConfig(&config);
    config.compare_type = RTK_RLDP_CMPTYPE_END;
    if(RT_ERR_OK == rtk_rldp_config_set(&config))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    dal_rldp_test_resetConfig(&config);
#if defined(CONFIG_SDK_APOLLOMP)
    config.interval_check = APOLLOMP_RLDP_INTERVAL_MAX + 1;
#endif
    if(RT_ERR_OK == rtk_rldp_config_set(&config))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    dal_rldp_test_resetConfig(&config);
#if defined(CONFIG_SDK_APOLLOMP)
    config.interval_loop = APOLLOMP_RLDP_INTERVAL_MAX + 1;
#endif
    if(RT_ERR_OK == rtk_rldp_config_set(&config))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    dal_rldp_test_resetConfig(&config);
#if defined(CONFIG_SDK_APOLLOMP)
    config.num_check = APOLLOMP_RLDP_NUM_MAX + 1;
#endif
    if(RT_ERR_OK == rtk_rldp_config_set(&config))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    dal_rldp_test_resetConfig(&config);
#if defined(CONFIG_SDK_APOLLOMP)
    config.num_loop = APOLLOMP_RLDP_NUM_MAX + 1;
#endif
    if(RT_ERR_OK == rtk_rldp_config_set(&config))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Port out of range */
    dal_rldp_test_resetPortConfig(&portConfig);
    if(RT_ERR_OK == rtk_rldp_portConfig_set(HAL_GET_MIN_PORT() - 1, &portConfig))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_rldp_portConfig_set(HAL_GET_MAX_PORT() + 1, &portConfig))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_rldp_portConfig_get(HAL_GET_MIN_PORT() - 1, &portConfig))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_rldp_portConfig_get(HAL_GET_MAX_PORT() + 1, &portConfig))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Port config out of range */
    dal_rldp_test_resetPortConfig(&portConfig);
    portConfig.tx_enable = RTK_ENABLE_END;
    if(RT_ERR_OK == rtk_rldp_portConfig_set(HAL_GET_MIN_PORT(), &portConfig))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Port out of range */
    dal_rldp_test_resetPortStatus(&portStatus);
    if(RT_ERR_OK == rtk_rldp_portStatus_clear(HAL_GET_MIN_PORT() - 1, &portStatus))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_rldp_portStatus_clear(HAL_GET_MAX_PORT() + 1, &portStatus))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_rldp_portStatus_get(HAL_GET_MIN_PORT() - 1, &portStatus))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK == rtk_rldp_portStatus_get(HAL_GET_MAX_PORT() + 1, &portStatus))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Port status clear out of range */
    dal_rldp_test_resetPortStatus(&portStatus);
    portStatus.loop_enter = RTK_RLDP_LOOPSTS_END;
    if(RT_ERR_OK == rtk_rldp_portStatus_clear(HAL_GET_MIN_PORT(), &portStatus))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    dal_rldp_test_resetPortStatus(&portStatus);
    portStatus.loop_leave = RTK_RLDP_LOOPSTS_END;
    if(RT_ERR_OK == rtk_rldp_portStatus_clear(HAL_GET_MIN_PORT(), &portStatus))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* system config out of range */
    dal_rldp_test_resetPortStatus(&portStatus);
    portStatus.loop_leave = RTK_RLDP_LOOPSTS_END;
    if(RT_ERR_OK == rtk_rlpp_trapType_set(RTK_RLPP_TRAPTYPE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Step 4 */
    /* System config set/get */
    memset(&config, 0, sizeof(rtk_rldp_config_t));
    memset(&chkConfig, 0, sizeof(rtk_rldp_config_t));
    config.rldp_enable = ENABLED;
    config.fc_mode = RTK_RLDP_FLOWCTRL_BYPASS;
    config.magic.octet[0] = 0x11;
    config.magic.octet[1] = 0x22;
    config.magic.octet[2] = 0x33;
    config.magic.octet[3] = 0x44;
    config.magic.octet[4] = 0x55;
    config.magic.octet[5] = 0x66;
    config.compare_type = RTK_RLDP_CMPTYPE_MAGIC_ID;
    config.interval_check = 500;
    config.num_check = 100;
    config.interval_loop = 1000;
    config.num_loop = 200;
    if(RT_ERR_OK != rtk_rldp_config_set(&config))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != rtk_rldp_config_get(&chkConfig))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(memcmp(&config, &chkConfig, sizeof(rtk_rldp_config_t)) != 0)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Port config set/get */
    portConfig.tx_enable = ENABLED;
    HAL_SCAN_ALL_PORT(port)
    {
        if(!HAL_IS_CPU_PORT(port))
        {
            if(RT_ERR_OK != rtk_rldp_portConfig_set(port, &portConfig))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(RT_ERR_OK != rtk_rldp_portConfig_get(port, &chkPortConfig))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
            if(memcmp(&portConfig, &chkPortConfig, sizeof(rtk_rldp_portConfig_t)) != 0)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }
    
    /* System status get */
    if(RT_ERR_OK != rtk_rldp_status_get(&status))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* Port status get */
    HAL_SCAN_ALL_PORT(port)
    {
        if(!HAL_IS_CPU_PORT(port))
        {
            /* Only check if the status can be get */
            if(RT_ERR_OK != rtk_rldp_portStatus_get(port, &portStatus))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }

            /* Only check if the status can be cleared */
            if(RT_ERR_OK != rtk_rldp_portStatus_clear(port, &portStatus))
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;
            }
        }
    }

    /* System config set/get */
    if(RT_ERR_OK != rtk_rlpp_trapType_set(RTK_RLPP_TRAPTYPE_CPU))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RT_ERR_OK != rtk_rlpp_trapType_get(&trapType))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }
    if(RTK_RLPP_TRAPTYPE_CPU != trapType)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    return RT_ERR_OK;
}

