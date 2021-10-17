#include <osal/lib.h>
#include <osal/print.h>
#include <ioal/mem32.h>
#include <hal/common/halctrl.h>
#include <common/error.h>
#include <dal/dal_svlan_test_case.h>
#include <rtk/svlan.h>
#include <rtk/vlan.h>
#include <common/unittest_util.h>

int32 dal_svlan_test(uint32 caseNo)
{  
    uint32 dataR;
    uint32 dataW;
    rtk_enable_t enableR;
    rtk_enable_t enableW;
    rtk_vlan_t svidR;
    rtk_vlan_t svidW;
    rtk_port_t port;
  
    rtk_portmask_t pmsk;
    rtk_portmask_t memPmskW;
    rtk_portmask_t untagPmskW;
    rtk_portmask_t memPmskR;
    rtk_portmask_t untagPmskR;

    rtk_svlan_action_t actionR;
    rtk_svlan_action_t actionW;
    

    /*must initial for test*/
    if( rtk_svlan_create(1) != RT_ERR_OK && rtk_svlan_create(1) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_OK && rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    
    /*error input check*/
    /*out of range*/
    if( rtk_svlan_create(RTK_VLAN_ID_MAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_svlan_create(0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_destroy(RTK_VLAN_ID_MAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_destroy(0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    if( rtk_svlan_servicePort_set(HAL_GET_MAX_PORT()+1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_servicePort_set(HAL_GET_MIN_PORT()-1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_servicePort_get(HAL_GET_MAX_PORT()+1, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_servicePort_get(HAL_GET_MIN_PORT()-1, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_dmacVidSelState_set(HAL_GET_MAX_PORT()+1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_dmacVidSelState_set(HAL_GET_MIN_PORT()-1, ENABLED) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_dmacVidSelState_get(HAL_GET_MAX_PORT()+1, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_dmacVidSelState_get(HAL_GET_MIN_PORT()-1, &enableR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    

    if( rtk_svlan_deiKeepState_set(RTK_ENABLE_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_trapPri_set(HAL_INTERNAL_PRIORITY_MAX()+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_lookupType_set(SVLAN_LOOKUP_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*not group 0*/
    if( rtk_svlan_tpidEntry_set(1,RTK_ETHERTYPE_MAX) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    /*over max ether-type*/
    if( rtk_svlan_tpidEntry_set(0,RTK_ETHERTYPE_MAX+1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_priorityRef_set(REF_PRI_END) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_unmatchAction_set(SVLAN_ACTION_END, 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_unmatchAction_set(SVLAN_ACTION_USE_CVID, 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    /*not created svid 2*/
    if( rtk_svlan_unmatchAction_set(SVLAN_ACTION_SVLAN, 2) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    if( rtk_svlan_untagAction_set(SVLAN_ACTION_END, 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_untagAction_set(SVLAN_ACTION_SVLAN_AND_KEEP, 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*not created svid 2*/
    if( rtk_svlan_untagAction_set(SVLAN_ACTION_SVLAN, 2) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_portSvid_set(HAL_GET_MAX_PORT()+1, 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_portSvid_set(HAL_GET_MIN_PORT()- 1, 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    /*non-accepted svid*/
    if( rtk_svlan_portSvid_set(HAL_GET_MIN_PORT(), 0) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_portSvid_set(HAL_GET_MIN_PORT(), RTK_VLAN_ID_MAX + 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*not created svid 2*/
    if( rtk_svlan_portSvid_set(HAL_GET_MIN_PORT(), 2) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_portSvid_get(HAL_GET_MAX_PORT()+1, &svidR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_portSvid_get(HAL_GET_MIN_PORT()-1, &svidR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    HAL_GET_ALL_PORTMASK(memPmskW);
    HAL_GET_ALL_PORTMASK(untagPmskW);
    
    /*non-accepted svid*/
    if( rtk_svlan_memberPort_set(0, &memPmskW, &untagPmskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_memberPort_set(RTK_VLAN_ID_MAX+1, &memPmskW, &untagPmskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*not created svid 2*/
    if( rtk_svlan_memberPort_set(2, &memPmskW, &untagPmskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*port mask over*/    
    memPmskW.bits[0] = memPmskW.bits[0] + 1;
    if( rtk_svlan_memberPort_set(1, &memPmskW, &untagPmskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    untagPmskW.bits[0] = untagPmskW.bits[0] + 1;
    if( rtk_svlan_memberPort_set(1, &memPmskW, &untagPmskW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*non-accepted svid*/
    if( rtk_svlan_memberPort_get(0, &memPmskR, &untagPmskR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_memberPort_get(RTK_VLAN_ID_MAX+1, &memPmskR, &untagPmskR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*get set testing*/
    actionW = SVLAN_ACTION_SVLAN;
    for(svidW = 1; svidW <= RTK_VLAN_ID_MAX; svidW += (RTK_VLAN_ID_MAX-1))
    {
        if( rtk_svlan_unmatchAction_set(actionW, svidW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_unmatchAction_get(&actionR, &svidR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if(actionW != actionR || svidR != svidW)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if( rtk_svlan_untagAction_set(actionW, svidW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_untagAction_get(&actionR, &svidR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if(actionW != actionR || svidR != svidW)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }

    actionW = SVLAN_ACTION_SVLAN_AND_KEEP;
    for(svidW = 1; svidW <= RTK_VLAN_ID_MAX; svidW += (RTK_VLAN_ID_MAX-1))
    {
        if( rtk_svlan_unmatchAction_set(actionW, svidW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_unmatchAction_get(&actionR, &svidR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if(actionW != actionR || svidR != svidW)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }

    actionW = SVLAN_ACTION_TRAP;
    if( rtk_svlan_unmatchAction_set(actionW, 1) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_unmatchAction_get(&actionR, &svidR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(actionW != actionR)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_untagAction_set(actionW, 1) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_untagAction_get(&actionR, &svidR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(actionW != actionR)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    actionW = SVLAN_ACTION_USE_CVID;    
    if( rtk_svlan_untagAction_set(actionW, 1) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_untagAction_get(&actionR, &svidR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(actionW != actionR)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }




    for(dataW = REF_INTERNAL_PRI; dataW < REF_PRI_END; dataW++)
    {
        if( rtk_svlan_priorityRef_set(dataW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_priorityRef_get(&dataR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if(dataW != dataR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }

    
    for(dataW = 0; dataW <= RTK_ETHERTYPE_MAX; dataW+=0x1FFF)
    {
        if( rtk_svlan_tpidEntry_set(0, dataW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_tpidEntry_get(0, &dataR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if(dataW != dataR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }
    
    for(dataW = 0; dataW <= HAL_INTERNAL_PRIORITY_MAX(); dataW++)
    {
        if( rtk_svlan_trapPri_set(dataW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_trapPri_get(&dataR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if(dataW != dataR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }
    
    for(dataW = DISABLED; dataW < RTK_ENABLE_END; dataW ++)
    {
        if( rtk_svlan_deiKeepState_set(dataW) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if( rtk_svlan_deiKeepState_get(&dataR) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if(dataW != dataR)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }   

    HAL_SCAN_ALL_PORT(port)
    {
        for(svidW = 1; svidW <= RTK_VLAN_ID_MAX; svidW += (RTK_VLAN_ID_MAX-1))
        {
            if( rtk_svlan_portSvid_set(port, svidW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if( rtk_svlan_portSvid_get(port, &svidR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if(svidW != svidR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }
    }   

    HAL_SCAN_ALL_PORT(port)
    {
        for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
        {
            if( rtk_svlan_servicePort_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if( rtk_svlan_servicePort_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }    
    }   

    HAL_SCAN_ALL_PORT(port)
    {
        for(enableW = DISABLED; enableW < RTK_ENABLE_END; enableW ++)
        {
            if( rtk_svlan_dmacVidSelState_set(port, enableW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if( rtk_svlan_dmacVidSelState_get(port, &enableR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
            
            if(enableW != enableR)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }   
    }
    

    HAL_GET_ALL_PORTMASK(pmsk);
    for(memPmskW.bits[0] = 1; memPmskW.bits[0] <= pmsk.bits[0]; memPmskW.bits[0] = (memPmskW.bits[0] << 1))
    {
        for(untagPmskW.bits[0] = 2; untagPmskW.bits[0] <= pmsk.bits[0]; untagPmskW.bits[0] = (untagPmskW.bits[0] << 1))
        {
            if( rtk_svlan_memberPort_set(1, &memPmskW, &untagPmskW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_memberPort_get(1, &memPmskR, &untagPmskR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if(untagPmskR.bits[0] != untagPmskW.bits[0] || 
                memPmskR.bits[0] != memPmskW.bits[0])
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

        }

    }


    if( rtk_svlan_portSvid_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_servicePort_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_memberPort_get(1, NULL, &pmsk) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_memberPort_get(1, &pmsk, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_tpidEntry_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_priorityRef_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_dmacVidSelState_get(0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_unmatchAction_get(&dataR, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_unmatchAction_get(NULL, &svidR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    if( rtk_svlan_untagAction_get(&dataR, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_untagAction_get(NULL, &svidR) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_trapPri_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_deiKeepState_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

#if 0
    /*set value checking with register address checking*/
    ioal_mem32_write(HAL_GET_REG_ADDR(SVLAN_UPLINK_PMSKr),0x0);

    for(port=0; port<=APOLLO_PORTIDMAX ; port++)
        apollo_raw_svlan_portUplinkEnable_set(port,port&1);        

    ioal_mem32_read(HAL_GET_REG_ADDR(SVLAN_UPLINK_PMSKr),&regData);
   
#endif
    
    return RT_ERR_OK;
}    




int32 dal_svlan_test_differChip(uint32 caseNo)
{  

    rtk_svlan_lookupType_t lookUpTypeR;
    rtk_svlan_lookupType_t lookUpTypeW;
    rtk_enable_t stateR;
    rtk_enable_t stateW;
    
    if(UNITTEST_UTIL_CHIP_TYPE == APOLLO_CHIP_ID)
    {
        if( rtk_svlan_lookupType_set(SVLAN_LOOKUP_S64MBRCGF) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
        
        if( rtk_svlan_lookupType_set(SVLAN_LOOKUP_C4KVLAN) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_lookupType_get(&lookUpTypeR) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_sp2cUnmatchCtagging_set(ENABLED) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_sp2cUnmatchCtagging_set(DISABLED) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_sp2cUnmatchCtagging_get(&stateR) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }    
    
    else if(UNITTEST_UTIL_CHIP_TYPE == APOLLOMP_CHIP_ID)
    {
        if( rtk_svlan_lookupType_set(SVLAN_LOOKUP_END) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }

        if( rtk_svlan_sp2cUnmatchCtagging_set(RTK_ENABLE_END) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    

        for(lookUpTypeW = SVLAN_LOOKUP_S64MBRCGF; lookUpTypeW < SVLAN_LOOKUP_END; lookUpTypeW ++)
        {
            if( rtk_svlan_lookupType_set(lookUpTypeW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_lookupType_get(&lookUpTypeR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if(lookUpTypeR != lookUpTypeW)    
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
       	}		
        for(stateW = DISABLED; stateW < RTK_ENABLE_END; stateW ++)
        {
            if( rtk_svlan_sp2cUnmatchCtagging_set(stateW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_sp2cUnmatchCtagging_get(&stateR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if(stateR != stateW)    
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }

        if( rtk_svlan_lookupType_get(NULL) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }


        if( rtk_svlan_sp2cUnmatchCtagging_get(NULL) == RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }  
		
    }

    return RT_ERR_OK;
}

int32 dal_svlan_test_memberPortEntry(uint32 caseNo)
{  
    rtk_portmask_t pmsk;
    rtk_svlan_memberCfg_t mbrCfgW;
    rtk_svlan_memberCfg_t mbrCfgR;

    /*must initial for test*/
    if( rtk_svlan_create(1) != RT_ERR_OK && rtk_svlan_create(1) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_OK && rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }



    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    /*non-accepted svid*/
    mbrCfgW.svid = 0;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    mbrCfgW.svid = RTK_VLAN_ID_MAX+1;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_memberPortEntry_get(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*not created svid 2*/
    mbrCfgW.svid = 2;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_memberPortEntry_get(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    mbrCfgW.svid = 1;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    mbrCfgW.priority = HAL_INTERNAL_PRIORITY_MAX() + 1;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    mbrCfgW.svid = 1;
    HAL_GET_ALL_PORTMASK(mbrCfgW.memberport);
    mbrCfgW.memberport.bits[0] = mbrCfgW.memberport.bits[0] + 1;    
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    mbrCfgW.svid = 1;
    HAL_GET_ALL_PORTMASK(mbrCfgW.untagport);
    mbrCfgW.untagport.bits[0] = mbrCfgW.untagport.bits[0] + 1;    
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }


    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    mbrCfgW.svid = 1;
    mbrCfgW.efid = HAL_ENHANCED_FID_MAX() + 1;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    mbrCfgW.svid = 1;
    mbrCfgW.fid = HAL_VLAN_FID_MAX() + 1;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    mbrCfgW.svid = 1;
    mbrCfgW.efiden = RTK_ENABLE_END;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    mbrCfgW.svid = 1;
    mbrCfgW.fiden = RTK_ENABLE_END;
    if( rtk_svlan_memberPortEntry_set(&mbrCfgW) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&mbrCfgW,0x0,sizeof(rtk_svlan_memberCfg_t));
    osal_memset(&mbrCfgR,0x0,sizeof(rtk_svlan_memberCfg_t));
    mbrCfgW.svid = 1;
    mbrCfgR.svid = 1;
    HAL_GET_ALL_PORTMASK(mbrCfgW.memberport);
    HAL_GET_ALL_PORTMASK(mbrCfgW.untagport);
    HAL_GET_ALL_PORTMASK(pmsk);
    for(mbrCfgW.untagport.bits[0] = 0x1; mbrCfgW.untagport.bits[0] <= pmsk.bits[0]; mbrCfgW.untagport.bits[0] = mbrCfgW.untagport.bits[0] << 2)
    {
        for(mbrCfgW.memberport.bits[0] = 0x2; mbrCfgW.memberport.bits[0] <= pmsk.bits[0]; mbrCfgW.memberport.bits[0] = mbrCfgW.memberport.bits[0] << 2)
        {
            for(mbrCfgW.priority = 0; mbrCfgW.priority <= HAL_INTERNAL_PRIORITY_MAX(); mbrCfgW.priority += HAL_INTERNAL_PRIORITY_MAX())
            {
                for(mbrCfgW.efid = 0, mbrCfgW.efiden = ENABLED; mbrCfgW.efid <= HAL_ENHANCED_FID_MAX(); mbrCfgW.efid += HAL_ENHANCED_FID_MAX(), mbrCfgW.efiden = DISABLED)
                {
                    for(mbrCfgW.fid = 0, mbrCfgW.fiden = ENABLED; mbrCfgW.fid <= HAL_VLAN_FID_MAX(); mbrCfgW.fid += HAL_VLAN_FID_MAX(), mbrCfgW.fiden = DISABLED)
                    {
                        if( rtk_svlan_memberPortEntry_set(&mbrCfgW) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;       
                        }

                        if( rtk_svlan_memberPortEntry_get(&mbrCfgR) != RT_ERR_OK)
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;       
                        }

                        if(mbrCfgW.untagport.bits[0] != mbrCfgR.untagport.bits[0] ||
                            mbrCfgW.memberport.bits[0] != mbrCfgR.memberport.bits[0] || 
                            mbrCfgW.priority != mbrCfgR.priority || 
                            mbrCfgW.efid != mbrCfgR.efid || 
                            mbrCfgW.efiden != mbrCfgR.efiden || 
                            mbrCfgW.fid != mbrCfgR.fid || 
                            mbrCfgW.fiden != mbrCfgR.fiden) 
                        {
                            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                            return RT_ERR_FAILED;       
                        }                        
                    }
                }
            }
        }
    }




    if( rtk_svlan_memberPortEntry_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}




int32 dal_svlan_test_ipmcL2mc2s(uint32 caseNo)
{  
    rtk_vlan_t svidR;    
    rtk_mac_t macW;
    rtk_mac_t macMskW;


    /*must initial for test*/
    if( rtk_svlan_create(1) != RT_ERR_OK && rtk_svlan_create(1) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_OK && rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_ipmc2s_add(0x12345678, 0xFFFFFFFF, 2) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    osal_memset(&macW, 0x11, sizeof(rtk_mac_t));
    osal_memset(&macMskW, 0x22, sizeof(rtk_mac_t));
    if( rtk_svlan_l2mc2s_add(macW, macMskW, 2) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    

    /*get set del tesing*/
    if( rtk_svlan_ipmc2s_add(0x11111111, 0x22222222, 1) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_ipmc2s_add(0x33333333, 0x44444444, RTK_VLAN_ID_MAX) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&macW, 0x55, sizeof(rtk_mac_t));
    osal_memset(&macMskW, 0x66, sizeof(rtk_mac_t));
    if( rtk_svlan_l2mc2s_add(macW, macMskW, 1) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    osal_memset(&macW, 0x77, sizeof(rtk_mac_t));
    osal_memset(&macMskW, 0x88, sizeof(rtk_mac_t));
    if( rtk_svlan_l2mc2s_add(macW, macMskW, RTK_VLAN_ID_MAX) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    osal_memset(&macW, 0x77, sizeof(rtk_mac_t));
    osal_memset(&macMskW, 0x88, sizeof(rtk_mac_t));
    if( rtk_svlan_l2mc2s_get(macW, macMskW, &svidR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    if(svidR != RTK_VLAN_ID_MAX)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    osal_memset(&macW, 0x55, sizeof(rtk_mac_t));
    osal_memset(&macMskW, 0x66, sizeof(rtk_mac_t));
    if( rtk_svlan_l2mc2s_get(macW, macMskW, &svidR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    if(svidR != 1)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    

    
    if( rtk_svlan_ipmc2s_get(0x33333333, 0x44444444, &svidR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(svidR != RTK_VLAN_ID_MAX)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    } 


    if( rtk_svlan_ipmc2s_get(0x11111111, 0x22222222, &svidR) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if(svidR != 1)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    } 

    if( rtk_svlan_ipmc2s_del(0x11111111, 0x22222222) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_ipmc2s_del(0x33333333, 0x44444444) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&macW, 0x55, sizeof(rtk_mac_t));
    osal_memset(&macMskW, 0x66, sizeof(rtk_mac_t));
    if( rtk_svlan_l2mc2s_del(macW, macMskW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    osal_memset(&macW, 0x77, sizeof(rtk_mac_t));
    osal_memset(&macMskW, 0x88, sizeof(rtk_mac_t));
    if( rtk_svlan_l2mc2s_del(macW, macMskW) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    




    if( rtk_svlan_ipmc2s_get(0x12345678, 0xFFFFFFFF, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    osal_memset(&macW, 0x11, sizeof(rtk_mac_t));
    osal_memset(&macMskW, 0x22, sizeof(rtk_mac_t));
    if( rtk_svlan_l2mc2s_get(macW, macMskW, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    return RT_ERR_OK;
}

int32 dal_svlan_test_sp2c(uint32 caseNo)
{  
    rtk_vlan_t svidR;
    rtk_port_t port;
    rtk_vlan_t cvidR;
    rtk_vlan_t cvidW;

    /*must initial for test*/
    if( rtk_svlan_create(1) != RT_ERR_OK && rtk_svlan_create(1) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_OK && rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_vlan_create(1) != RT_ERR_OK && rtk_vlan_create(1) != RT_ERR_VLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_svlan_sp2c_add(2, 0, 1) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*get set del*/
    HAL_SCAN_ALL_PORT(port)
    { 
     
        for(cvidW = 1; cvidW <= 4095; cvidW += 500)
        {
             
            if( rtk_vlan_create(cvidW) != RT_ERR_OK && rtk_vlan_create(cvidW) != RT_ERR_VLAN_EXIST)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_sp2c_add(1, port, cvidW) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_sp2c_get(1, port, &cvidR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( cvidR != cvidW)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        if( rtk_svlan_sp2c_del(1, port) != RT_ERR_OK)
        {
            osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
            return RT_ERR_FAILED;       
        }
    }

    if( rtk_svlan_sp2c_add(1, 0, 1) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_sp2c_get(1, 0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}

int32 dal_svlan_test_c2s(uint32 caseNo)
{  
    rtk_vlan_t svidR;
    rtk_port_t port;
    rtk_vlan_t cvidR;
    rtk_vlan_t cvidW;

    /*must initial for test*/
    if( rtk_svlan_create(1) != RT_ERR_OK && rtk_svlan_create(1) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_OK && rtk_svlan_create(RTK_VLAN_ID_MAX) != RT_ERR_SVLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    if( rtk_vlan_create(1) != RT_ERR_OK && rtk_vlan_create(1) != RT_ERR_VLAN_EXIST)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    /*non-creat svid*/
    if( rtk_svlan_c2s_add(1, 0, 2) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    /*get set del*/
    HAL_SCAN_ALL_PORT(port)
    { 
        for(cvidW = 1; cvidW <= 4095; cvidW += 500)
        {
             
            if( rtk_vlan_create(cvidW) != RT_ERR_OK && rtk_vlan_create(cvidW) != RT_ERR_VLAN_EXIST)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_c2s_add(cvidW, port, 1) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_c2s_get(cvidW, port, &svidR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( svidR != 1)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }

        for(cvidW = 2; cvidW <= 4095; cvidW += 500)
        {
             
            if( rtk_vlan_create(cvidW) != RT_ERR_OK && rtk_vlan_create(cvidW) != RT_ERR_VLAN_EXIST)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_c2s_add(cvidW, port, RTK_VLAN_ID_MAX) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( rtk_svlan_c2s_get(cvidW, port, &svidR) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }

            if( svidR != RTK_VLAN_ID_MAX)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }
    }

    HAL_SCAN_ALL_PORT(port)
    {
        for(cvidW = 1; cvidW <= 4095; cvidW += 500)
        {
            if( rtk_svlan_c2s_del(cvidW, port, 1) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }
        
        for(cvidW = 2; cvidW <= 4095; cvidW += 500)
        {
            if( rtk_svlan_c2s_del(cvidW, port, RTK_VLAN_ID_MAX) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
        }

    }

    if( rtk_svlan_c2s_add(1, 0, 1) != RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    if( rtk_svlan_c2s_get(1, 0, NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }

    return RT_ERR_OK;
}




