/*
 * Copyright(c) Realtek Semiconductor Corporation, 2011
 * All rights reserved.
 *
 * $Revision: 14276 $
 * $Date: 2010-11-18 09:37:36 +0800 (Thu, 18 Nov 2010) $
 *
 * Purpose : Test SWITCH API
 *
 * Feature : SWITCH API test 
 *
 */

/*
 * Include Files
 */
#include <osal/lib.h>
#include <common/debug/rt_log.h>
#include <rtk/switch.h>
#include <dal/dal_switch_test_case.h>
#include <hal/common/halctrl.h>
#include <common/unittest_util.h>


#if 1
#ifdef CONFIG_SDK_APOLLO 
    #include <hal/chipdef/apollo/apollo_def.h>
#endif
#ifdef CONFIG_SDK_APOLLOMP 
    #include <hal/chipdef/apollomp/apollomp_def.h>
#endif
#endif

#define SWITCH_RANDOM_RUN_TIMES        100

/*
 * Function Declaration
 */


/*
 * Function Declaration
 */
int32 dal_switch_maxPktLenLinkSpeed_test(uint32 testcase)
{  
    int32  ret;
    rtk_switch_maxPktLen_linkSpeed_t   speed;
    uint32 setLen,getLen;
    uint32 maxLen;
    int32 randIdx;
     
    switch(UNITTEST_UTIL_CHIP_TYPE)
    {
#ifdef CONFIG_SDK_APOLLO 
        case APOLLO_CHIP_ID:
            maxLen = APOLLO_PACEKT_LENGTH_MAX;
            break;
#endif
#ifdef CONFIG_SDK_APOLLOMP 
        case APOLLOMP_CHIP_ID:
            maxLen = APOLLOMP_PACEKT_LENGTH_MAX;
            break;
#endif
        default:
            maxLen = 0x3FFF;
            break;
    }    
    
    
    speed = MAXPKTLEN_LINK_SPEED_FE;
    setLen = maxLen;
    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_maxPktLenLinkSpeed_set(speed,setLen)) 
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_maxPktLenLinkSpeed_get(speed,&getLen)))
    {
        return RT_ERR_OK;
    }
    /*error input check*/
    /*1. out of range*/
        /*index out of range*/
    speed = MAXPKTLEN_LINK_SPEED_END;
    setLen = maxLen;
    if( rtk_switch_maxPktLenLinkSpeed_set(speed,setLen) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_switch_maxPktLenLinkSpeed_get(speed,&setLen) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }    
        /*length out of range*/
    speed = MAXPKTLEN_LINK_SPEED_FE;
    setLen = maxLen+1;
    if( rtk_switch_maxPktLenLinkSpeed_set(speed,setLen) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    
    /*2. null pointer*/    
    if( rtk_switch_maxPktLenLinkSpeed_get(speed,NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }     
    
    /*get set test*/
    for (speed = 0; speed < MAXPKTLEN_LINK_SPEED_END; speed++)
    {
        for (randIdx = 0; randIdx <= SWITCH_RANDOM_RUN_TIMES; randIdx++) 
        {
            
            setLen = ut_rand()%maxLen;
            /*set*/
            if( rtk_switch_maxPktLenLinkSpeed_set(speed,setLen) != RT_ERR_OK)
            {
                osal_printf("\n %s %d speed:%d setLen:%d\n",__FUNCTION__,__LINE__,speed,setLen);
                return RT_ERR_FAILED;       
            }
            /*get*/
            if( rtk_switch_maxPktLenLinkSpeed_get(speed,&getLen) != RT_ERR_OK)
            {
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                return RT_ERR_FAILED;       
            }
    
            /*compare*/
            if(setLen != getLen)
            {
            #if 1    
                osal_printf("\n[write]\n");
                osal_printf("\n %d\n",setLen);
    
                osal_printf("\n[read]\n");
                osal_printf("\n %d\n",getLen);
    
            #endif                  
                osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
                
                return RT_ERR_FAILED;
            }
        }
    }    
    
    return RT_ERR_OK;        
}

int32 dal_switch_mgmtMacAddr_test(uint32 testcase)
{  
    int32 ret;
    rtk_mac_t  mac_temp;
    int32  expect_result = 1;
    
    if ((RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_mgmtMacAddr_set(&mac_temp)) 
        || (RT_ERR_CHIP_NOT_SUPPORTED == rtk_switch_mgmtMacAddr_get(&mac_temp)))
    {
        return RT_ERR_OK;
    }
    
    /*error input check*/        
    /*1. null pointer*/
    if( rtk_switch_mgmtMacAddr_get(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
    if( rtk_switch_mgmtMacAddr_set(NULL) == RT_ERR_OK)
    {
        osal_printf("\n %s %d\n",__FUNCTION__,__LINE__);
        return RT_ERR_FAILED;       
    }
        
    {/*get set test*/
        rtk_mac_t mac;
        rtk_mac_t  mac_result;
        int32 randIdx;
      
            
        for (randIdx = 0; randIdx <= SWITCH_RANDOM_RUN_TIMES; randIdx++) 
        {
            mac.octet[0]= ut_rand()& BITMASK_8B;
            mac.octet[1]= ut_rand()& BITMASK_8B;
            mac.octet[2]= ut_rand()& BITMASK_8B;
            mac.octet[3]= ut_rand()& BITMASK_8B;
            mac.octet[4]= ut_rand()& BITMASK_8B;
            mac.octet[5]= ut_rand()& BITMASK_8B;
    
            if((mac.octet[0] & BITMASK_1B) == 1)
                expect_result = RT_ERR_INPUT;
            else
                expect_result = RT_ERR_OK;
            
    
            ret =  rtk_switch_mgmtMacAddr_set(&mac);
            if(RT_ERR_INPUT == expect_result)
            {
                if(ret != expect_result)
                {
                    osal_printf("\n %s %d expect_result:%d ret:%d\n",__FUNCTION__,__LINE__,expect_result,ret);
                    return RT_ERR_FAILED;       
                }
                continue;
            }
                
            ret=rtk_switch_mgmtMacAddr_get(&mac_result);
    
            if (ret != expect_result)
            {
                osal_printf("\n %s %d expect_result:%d ret:%d\n",__FUNCTION__,__LINE__,expect_result,ret);
                return RT_ERR_FAILED;   
            }
        }
    }    

    return RT_ERR_OK;    
}
