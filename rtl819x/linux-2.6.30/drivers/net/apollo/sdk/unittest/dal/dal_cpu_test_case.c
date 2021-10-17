#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <common/unittest_util.h>
#include <rtk/cpu.h>
#include <dal/dal_cpu_test_case.h>

/* Define symbol used for test input */


int32 dal_cpu_port_test(uint32 caseNo)
{
    int32 ret;
    rtk_port_t cpu_port, tmp_port;
    rtk_portmask_t port_mask, tmp_mask;
    rtk_cpu_tag_fmt_t mode, tmp_mode;
    rtk_enable_t state, tmp_state;

    /*error input check*/
    /*out of range*/
    port_mask.bits[0] = 0xFF;
    if(RT_ERR_OK == rtk_cpu_awarePortMask_set(port_mask))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_cpu_tagFormat_set(CPU_TAG_FMT_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_cpu_trapInsertTag_set(RTK_ENABLE_END))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }


    /*NULL pointer*/
    if(RT_ERR_OK == rtk_cpu_awarePortMask_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_cpu_tagFormat_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    if(RT_ERR_OK == rtk_cpu_trapInsertTag_get(NULL))
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;
    }

    /* get/set test*/
    /* aware port mask */
    for(port_mask.bits[0] = 0; port_mask.bits[0] < 0x7F; port_mask.bits[0]++)
    {
        if(RT_ERR_OK != rtk_cpu_awarePortMask_set(port_mask))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_cpu_awarePortMask_get(&tmp_mask))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(port_mask.bits[0] != tmp_mask.bits[0])
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    /* tag format */
    for(mode = 0; mode <CPU_TAG_FMT_END; mode++)
    {
        if(RT_ERR_OK != rtk_cpu_tagFormat_set(mode))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_cpu_tagFormat_get(&tmp_mode))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(mode != tmp_mode)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }


    for(state = 0; state < RTK_ENABLE_END; state++)
    {
        /* trap insert tag */
        if(RT_ERR_OK != rtk_cpu_trapInsertTag_set(state))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(RT_ERR_OK != rtk_cpu_trapInsertTag_get(&tmp_state))
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
        if(state != tmp_state)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;
        }
    }

    return RT_ERR_OK;
}

