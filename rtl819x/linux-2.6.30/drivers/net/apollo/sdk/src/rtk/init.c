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
 * Purpose : Definition of Init API
 *
 * Feature : Initialize All Layers of RTK Module
 *
 */

/*
 * Include Files
 */
#include <common/rt_error.h>
#include <common/debug/rt_log.h>
#include <ioal/ioal_init.h>
#include <hal/common/halctrl.h>
#include <dal/dal_mgmt.h>
#include <rtk/init.h>
#include <rtk/l34.h>
#include <rtk/stp.h>
#include <rtk/svlan.h>
#include <rtk/acl.h>
#include <rtk/led.h>
#include <rtk/mirror.h>
#include <rtk/trunk.h>
#include <rtk/port.h>
#include <rtk/vlan.h>
#include <rtk/mirror.h>
#include <rtk/cpu.h>
#include <rtk/trap.h>
#include <rtk/irq.h>

#if 0
#include <rtk/default.h>
#include <rtk/dot1x.h>
#include <rtk/filter.h>
#include <rtk/flowctrl.h>
#include <rtk/l2.h>
#include <rtk/qos.h>
#include <rtk/rate.h>
#include <rtk/stat.h>
#include <rtk/switch.h>
#endif

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */

/*
 * Function Declaration
 */


int32
rtk_core_init(void)
{
    int32 ret = RT_ERR_FAILED;
    RT_DBG(LOG_EVENT, MOD_INIT, "rtk_core_init Start!!");

    ioal_init();

    /* Initialize the hal layer */
    if ((ret = hal_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "hal_init Failed!!");
        return ret;
    }
    RT_DBG(LOG_EVENT, MOD_INIT, "hal_init Completed!!");

    if ((ret = dal_mgmt_initDevice()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "dal_mgmt_initDevice Failed!!");
        return ret;
    }

    return ret;
}


/* Function Name:
 *      rtk_init
 * Description:
 *      Initialize the driver, hook related driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_all_module_init(void)
{
    int32 ret = RT_ERR_FAILED;

    if ((ret = rtk_switch_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_switch_init Failed!!");
        return ret;
    }

    if ((ret = rtk_svlan_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_svlan_init Failed!!");
        return ret;
    }
    if ((ret = rtk_stp_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_stp_init Failed!!");
        return ret;
    }
    if ((ret = rtk_oam_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_oam_init Failed!!");
        return ret;
    }
    if ((ret = rtk_acl_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_acl_init Failed!!");
        return ret;
    }
    if ((ret = rtk_qos_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_qos_init Failed!!");
        return ret;
    }
    if ((ret = rtk_sec_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_sec_init Failed!!");
        return ret;
    }
    if ((ret = rtk_rate_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_rate_init Failed!!");
        return ret;
    }
#if defined(CONFIG_CLASSFICATION_FEATURE)
    if ((ret = rtk_classify_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_classify_init Failed!!");
        return ret;
    }
#endif
    if ((ret = rtk_stat_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_stat_init Failed!!");
        return ret;
    }
    if ((ret = rtk_trunk_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_trunk_init Failed!!");
        return ret;
    }
    if ((ret = rtk_l2_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_l2_init Failed!!");
        return ret;
    }
    if ((ret = rtk_vlan_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_vlan_init Failed!!");
        return ret;
    }
    if ((ret = rtk_port_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_port_init Failed!!");
        return ret;
    }
    if ((ret = rtk_mirror_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_mirror_init Failed!!");
        return ret;
    }
    if ((ret = rtk_cpu_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_mirror_init Failed!!");
        return ret;
    }
    if ((ret = rtk_trap_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_trap_init Failed!!");
        return ret;
    }

#if defined(CONFIG_SDK_KERNEL_LINUX)
#if defined(CONFIG_SOC_DEPEND_FEATURE)
#if defined(CONFIG_GPON_FEATURE)
    if ((ret = rtk_gpon_driver_initialize()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_gpon_driver_initialize Failed!!");
        return ret;
    }
    if ((ret = rtk_gpon_device_initialize()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_gpon_device_initialize Failed!!");
        return ret;
    }
#endif
#endif
#endif


    return ret;
}




/* Function Name:
 *      rtk_init
 * Description:
 *      Initialize the driver, hook related driver
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_init(void)
{
    int32 ret = RT_ERR_FAILED;

#if defined(CONFIG_SDK_KERNEL_LINUX)
    if ((ret = rtk_core_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_core_init Failed!!");
        return ret;
    }
#if defined(CONFIG_SOC_DEPEND_FEATURE)
	if ((ret = rtk_intr_init()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_intr_init Failed!!");
        return ret;
    }

	if ((ret = rtk_switch_irq_init(IRQ_ID_NO_CARE)) != RT_ERR_OK)
	{
		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_switch_irq_init Failed!!");
        return ret;
	}
#endif /*CONFIG_SOC_DEPEND_FEATURE*/
#endif /*CONFIG_SDK_KERNEL_LINUX*/


#if !defined(CONFIG_PURE_HW_INIT)
	if ((ret = rtk_all_module_init()) != RT_ERR_OK)
	{
		 RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_all_module_init Failed!!");
        return ret;
	}
#endif



    return ret;

} /* end of rtk_init */

/* Function Name:
 *      rtk_deinit
 * Description:
 *      De-Initialize the driver, release irq
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      INIT must be initialized before using all of APIs in each modules
 */
int32
rtk_deinit(void)
{
    int32 ret = RT_ERR_FAILED;

    RT_DBG(LOG_EVENT, MOD_INIT, "rtk_deinit Start!!");

#if defined(CONFIG_SDK_KERNEL_LINUX)
#if defined(CONFIG_SOC_DEPEND_FEATURE)
    /* IRQ deinit */
    rtk_switch_irq_exit();

#if defined(CONFIG_GPON_FEATURE)
    if ((ret = rtk_gpon_device_deInitialize()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_gpon_device_deInitialize Failed!!");
        return ret;
    }

    if ((ret = rtk_gpon_driver_deInitialize()) != RT_ERR_OK)
    {
        RT_DBG(LOG_MAJOR_ERR, MOD_INIT, "rtk_gpon_driver_deInitialize Failed!!");
        return ret;
    }
#endif
#endif
#endif

    return ret;

} /* end of rtk_init */

#ifdef CONFIG_RTL_8198B
#define RTK_WAN_VID 8
#define RTK_LAN_VID 9

int32
rtk_set_vlan_mode( int mode )
{
    int32 ret=0 ;
    rtk_portmask_t memberPortmask;
    rtk_portmask_t untagPortmask;
    uint32 port ;

    rtk_vlan_destroy(RTK_LAN_VID) ;
    rtk_vlan_destroy(RTK_WAN_VID) ;     
   
    if((ret= rtk_vlan_create (RTK_LAN_VID)) != RT_ERR_OK)
    {
        return ret;
    }   
    
    /*add UTP 0,1,2,3 and CPU to RTK_LAN_VID*/
    RTK_PORTMASK_RESET(memberPortmask);
    RTK_PORTMASK_RESET(untagPortmask);
    rtk_switch_port2PortMask_set(&memberPortmask, RTK_PORT_UTP0);
    rtk_switch_port2PortMask_set(&untagPortmask, RTK_PORT_UTP0);

    rtk_switch_port2PortMask_set(&memberPortmask, RTK_PORT_UTP1);
    rtk_switch_port2PortMask_set(&untagPortmask, RTK_PORT_UTP1);

    rtk_switch_port2PortMask_set(&memberPortmask, RTK_PORT_UTP2);
    rtk_switch_port2PortMask_set(&untagPortmask, RTK_PORT_UTP2);

    rtk_switch_port2PortMask_set(&memberPortmask, RTK_PORT_UTP3);
    rtk_switch_port2PortMask_set(&untagPortmask, RTK_PORT_UTP3);

    rtk_switch_port2PortMask_set(&memberPortmask, RTK_PORT_CPU);
    rtk_switch_port2PortMask_set(&untagPortmask, RTK_PORT_CPU);  

    if(mode >=1 ) // not GW
    {
	rtk_switch_port2PortMask_set(&memberPortmask, RTK_PORT_PON);
       rtk_switch_port2PortMask_set(&untagPortmask, RTK_PORT_PON);
    }
	
    if((ret= rtk_vlan_port_set(RTK_LAN_VID, &memberPortmask, &untagPortmask)) != RT_ERR_OK)
    {
        return ret;
    }

    /*UTP0~3 port based vlan set to RTK_LAN_VID*/
    rtk_switch_phyPortId_get(RTK_PORT_UTP0, &port);
    if((ret= rtk_vlan_portPvid_set(port, RTK_LAN_VID)) != RT_ERR_OK)
    {
        return ret;
    }
    rtk_switch_phyPortId_get(RTK_PORT_UTP1, &port);
    if((ret= rtk_vlan_portPvid_set(port, RTK_LAN_VID)) != RT_ERR_OK)
    {
        return ret;
    }
    rtk_switch_phyPortId_get(RTK_PORT_UTP2, &port);
    if((ret= rtk_vlan_portPvid_set(port, RTK_LAN_VID)) != RT_ERR_OK)
    {
        return ret;
    }
    rtk_switch_phyPortId_get(RTK_PORT_UTP3, &port);
    if((ret= rtk_vlan_portPvid_set(port, RTK_LAN_VID)) != RT_ERR_OK)
    {
        return ret;
    }
    // if CPU need PVID  
    rtk_switch_phyPortId_get(RTK_PORT_CPU, &port);
    if((ret= rtk_vlan_portPvid_set(port, RTK_LAN_VID)) != RT_ERR_OK)
    {
        return ret;
    }		

    if(mode >=1 ) // not GW
    {
	rtk_switch_phyPortId_get(RTK_PORT_PON, &port);
    	if((ret= rtk_vlan_portPvid_set(port, RTK_LAN_VID)) != RT_ERR_OK)
    	{
        return ret;
    	}

	 return 0 ;  //not GW mode will bypass below WAN setting  
    }	

     // mode == 0 GW mode do below
     if((ret= rtk_vlan_create (RTK_WAN_VID)) != RT_ERR_OK)
    {
        return ret;
    }
    /*add PON port and CPU to vlan RTK_WAN_VID*/
    RTK_PORTMASK_RESET(memberPortmask);
    RTK_PORTMASK_RESET(untagPortmask);

    rtk_switch_port2PortMask_set(&memberPortmask, RTK_PORT_PON);
    rtk_switch_port2PortMask_set(&untagPortmask, RTK_PORT_PON);

    rtk_switch_port2PortMask_set(&memberPortmask, RTK_PORT_CPU); //to cpu port will tag Wan vid
    rtk_switch_port2PortMask_set(&untagPortmask, RTK_PORT_CPU); // mark_apo , Gamc can differ LAN WAN using port_num

    if((ret= rtk_vlan_port_set(RTK_WAN_VID, &memberPortmask, &untagPortmask)) != RT_ERR_OK) 
    {
        return ret;
    }        
    /*PON port port based vlan set to RTK_WAN_VID*/
    rtk_switch_phyPortId_get(RTK_PORT_PON, &port);
    if((ret= rtk_vlan_portPvid_set(port, RTK_WAN_VID)) != RT_ERR_OK)
    {
        return ret;
    }
    return RT_ERR_OK;    
}
#if 0 // isolation 
void rtk_lan_wan(void)
{
    rtk_portmask_t extPortmask;
    rtk_portmask_t portmaskLan;
    rtk_portmask_t portmaskWan;
    rtk_portmask_t portmaskCpu;

    rtk_port_t port,checkPort;
    rtk_port_t cfgPort;
   
    
    
    RTK_PORTMASK_RESET(extPortmask);
    
    /*set all ext port*/
    HAL_SCAN_ALL_EXT_PORT(port)
    {
        RTK_PORTMASK_PORT_SET((extPortmask), port);
    }
    
    /*init port mask*/    

    /*isolate UTP0~UTP3 to UTP0~UTP3 and cpu*/
    RTK_PORTMASK_RESET(portmaskLan);
    HAL_SCAN_ALL_PORT(port)
    {
        /*skip pon port and ext port*/
        rtk_switch_phyPortId_get(RTK_PORT_EXT0,&checkPort);
        
        if(port == checkPort)
            continue;
        rtk_switch_phyPortId_get(RTK_PORT_PON,&checkPort);
        if(port == checkPort)
            continue;
        RTK_PORTMASK_PORT_SET((portmaskLan), port);
    }

    /*isolate pon port to cpu*/
    RTK_PORTMASK_RESET(portmaskWan);
    rtk_switch_port2PortMask_set(&portmaskWan,RTK_PORT_CPU);
    rtk_switch_port2PortMask_set(&portmaskWan,RTK_PORT_PON);


    /*isolate cpu port to UTP0~UTP3 and pon*/
    RTK_PORTMASK_RESET(portmaskCpu);
    HAL_SCAN_ALL_PORT(port)
    {
        /*skip pon port and ext port*/
        rtk_switch_phyPortId_get(RTK_PORT_EXT0,&checkPort);
        if(port == checkPort)
            continue;
        RTK_PORTMASK_PORT_SET((portmaskCpu), port);
    }
    
   
    HAL_SCAN_ALL_PORT(cfgPort)
    {
        /*skip pon port and ext port*/
        rtk_switch_phyPortId_get(RTK_PORT_EXT0,&checkPort);
        if(cfgPort == checkPort)
            continue;

        rtk_switch_phyPortId_get(RTK_PORT_PON,&checkPort);
        if(cfgPort == checkPort)
        {
            rtk_port_isolation_set(cfgPort, &portmaskWan, &extPortmask);
            continue;
        }        

        rtk_switch_phyPortId_get(RTK_PORT_CPU,&checkPort);
        if(cfgPort == checkPort)
        {
            rtk_port_isolation_set(cfgPort, &portmaskCpu, &extPortmask);

            continue;
        }
        rtk_port_isolation_set(cfgPort, &portmaskLan, &extPortmask);
    }

}

void rtk_all_lan(void)
{
    rtk_portmask_t extPortmask;
    rtk_portmask_t portmask;
    rtk_port_t port;
    
    
    RTK_PORTMASK_RESET(extPortmask);
    RTK_PORTMASK_RESET(portmask);
    
    /*set all ext port*/
    HAL_SCAN_ALL_EXT_PORT(port)
    {
        RTK_PORTMASK_PORT_SET((extPortmask), port);
    }
    /*select all port mask*/    
    HAL_SCAN_ALL_PORT(port)
    {
        RTK_PORTMASK_PORT_SET((portmask), port);
    }    

    HAL_SCAN_ALL_PORT(port)
    {
        rtk_port_isolation_set(port, &portmask, &extPortmask);
    }    

}
#endif

#endif

