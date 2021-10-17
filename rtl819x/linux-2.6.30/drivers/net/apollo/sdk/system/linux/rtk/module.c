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
 * $Revision: 16745 $
 * $Date: 2011-04-12 11:46:26 +0800 (?Ÿæ?äº? 12 ?›æ? 2011) $
 *
 * Purpose : Export the public APIs in lower layer module in the SDK.
 *
 * Feature : Export the public APIs in lower layer module
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#if 0
#include <common/rt_autoconf.h>
#include <hal/mac/reg.h>
#include <hal/mac/mem.h>
#include <hal/mac/mac_debug.h>
#include <hal/chipdef/chip.h>
#include <hal/common/halctrl.h>
#include <hal/chipdef/driver.h>
#include <rtk/dot1x.h>
#include <rtk/eee.h>
#include <rtk/filter.h>
#include <rtk/flowctrl.h>
#include <rtk/l2.h>
#include <rtk/l3.h>
#include <rtk/mirror.h>
#include <rtk/oam.h>
#include <rtk/pie.h>
#include <rtk/port.h>
#include <rtk/qos.h>
#include <rtk/rate.h>
#include <rtk/sec.h>
#include <rtk/stat.h>
#include <rtk/stp.h>
#include <rtk/svlan.h>
#include <rtk/switch.h>
#include <rtk/trap.h>
#include <rtk/trunk.h>
#include <rtk/vlan.h>
#include <rtk/led.h>

#if defined(CONFIG_SDK_DRIVER_TEST_MODULE) || defined(CONFIG_SDK_DRIVER_TEST)
#include <hal/common/miim.h>
#include <hal/phy/identify.h>
#include <hal/mac/mem.h>
#endif

#endif /* if 0 */

#include <common/rt_error.h>
#include <common/util/rt_util.h>
#include <osal/print.h>
#include <rtk/init.h>
#include <rtk/gpon.h>
#include <apollo_reg_struct.h>
#include <hal/mac/reg.h>

/*
 * Symbol Definition
 */


/*
 * Data Declaration
 */

/*
 * Macro Definition
 */
#define REG32(reg)	(*(volatile unsigned int   *)((unsigned int)reg))

/*
 * Function Declaration
 */

#ifdef  CONFIG_RTL_8198B
#include <linux/proc_fs.h>

extern int32 rtk_set_vlan_mode( int mode );
extern void get_br0_ip_mask(void); //from rtl_fueatu.c
extern void rtk_set_gmac_mode(int mode); //from re8686.c
extern int32 rtk_ponmac_init(void);
//from 819x , for fastpath wan
#define GATEWAY_MODE				0
#define BRIDGE_MODE					1
#define WISP_MODE					2

int rtk_linkEvent=0; 
static struct proc_dir_entry *proc_sw_nat=NULL;
static char gSwNatSetting[16];

static int sw_nat_read_proc(char *page, char **start, off_t off,
		     int count, int *eof, void *data)
{
	int len;

	len = sprintf(page, "%s\n", gSwNatSetting);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count) len = count;
	if (len<0) len = 0;
	return len;
}

static int sw_nat_write_proc(struct file *file, const char *buffer,
		      unsigned long count, void *data)
{
	if (count < 2)
		return -EFAULT;

	if (buffer && !copy_from_user(&gSwNatSetting, buffer, 8)) {
		if (gSwNatSetting[0] == '0'){  /* operation mode = GATEWAY */			
			//rtl865x_changeOpMode(0);
			//rtk_lan_wan();
			rtk_set_vlan_mode(GATEWAY_MODE);
			rtk_set_gmac_mode(GATEWAY_MODE);
			
			printk("GATEWAY_MODE\n");
		}
		else if (gSwNatSetting[0] == '1'){  /* operation mode = BRIDGE*/			
			//rtl865x_changeOpMode(1);			
			//rtk_all_lan();
			rtk_set_vlan_mode(BRIDGE_MODE);
			rtk_set_gmac_mode(BRIDGE_MODE);
			
			printk("BRIDGE_MODE\n");
		}
		else if(gSwNatSetting[0] == '2'){ /* operation mode = WISP */
			//rtl865x_changeOpMode(2);
			
			printk("WISP_MODE\n");
		}
#if defined(CONFIG_RTL_HARDWARE_NAT) || defined(CONFIG_RTL_IPTABLES_FAST_PATH) || defined(CONFIG_RTL_WLAN_DOS_FILTER)
		else if(gSwNatSetting[0] == '9'){
			//rtk_lan_wan();
			//printk("get_br0_ip_mask 9\n");
			get_br0_ip_mask();
		}		
#endif
		return count;
	}
	return -EFAULT;
}

static int32 rtk_link_event_read( char *page, char **start, off_t off, int count, int *eof, void *data )
{
		int len;
	len = sprintf(page, "%d\n",rtk_linkEvent);

	if (len <= off+count) *eof = 1;
	*start = page + off;
	len -= off;
	if (len>count)
		len = count;
	if (len<0)
	  	len = 0;

	return len;
}
static int32 rtk_link_event_write( struct file *filp, const char *buff,unsigned long len, void *data )
{
	char 		tmpbuf[4];
	if (buff && !copy_from_user(tmpbuf, buff, len))
	{
		if(0 == (tmpbuf[0]-'0'))			
			rtk_linkEvent = 0;					
	}
	return len;
}

void  rtk_819x_proc_init(void)
{
		struct proc_dir_entry *res_stats_root;
		struct proc_dir_entry *rtk_link_event_entry;
		char *if_name="eth1";

		res_stats_root = proc_mkdir(if_name, NULL);
		if (res_stats_root == NULL)
		{
			printk("proc_mkdir failed!\n");
			return;
		}	
		
              rtk_linkEvent=1; //mark_apo
		rtk_link_event_entry=create_proc_entry("up_event",0,res_stats_root);
		if(rtk_link_event_entry)
		{
			rtk_link_event_entry->read_proc=rtk_link_event_read;
		    rtk_link_event_entry->write_proc=rtk_link_event_write;
		}

 	       proc_sw_nat = create_proc_entry("sw_nat", 0, NULL);
 	      if (proc_sw_nat) {
		proc_sw_nat->read_proc = sw_nat_read_proc;
		proc_sw_nat->write_proc = sw_nat_write_proc;
	        }
	
}


#endif	

static int __init rtk_sdk_init(void)
{ 
    osal_printf("Init RTK Driver Module....");

    if(RT_ERR_OK != rtk_init())
        osal_printf("FAIL\n");
    else
    {
        osal_printf("OK\n");
	#ifdef  CONFIG_RTL_8198B
	 rtk_ponmac_init(); //fix port3 issue
        rtk_819x_proc_init();
       #endif	
    }		

	return 0;
}

static void __exit rtk_sdk_exit(void)
{
    osal_printf("Exit RTK Driver Module....");

    if(RT_ERR_OK != rtk_deinit())
        osal_printf("FAIL\n");
    else
        osal_printf("OK\n");
}

module_init(rtk_sdk_init);
module_exit(rtk_sdk_exit);

MODULE_DESCRIPTION("Switch SDK RTK Driver Module");

/* RTK functions */

EXPORT_SYMBOL(rtk_init);
#if 0
EXPORT_SYMBOL(rtk_dot1x_init);
EXPORT_SYMBOL(rtk_dot1x_unauthPacketOper_get);
EXPORT_SYMBOL(rtk_dot1x_unauthPacketOper_set);
EXPORT_SYMBOL(rtk_dot1x_portUnauthPacketOper_get);
EXPORT_SYMBOL(rtk_dot1x_portUnauthPacketOper_set);
EXPORT_SYMBOL(rtk_dot1x_portUnauthTagPacketOper_get);
EXPORT_SYMBOL(rtk_dot1x_portUnauthTagPacketOper_set);
EXPORT_SYMBOL(rtk_dot1x_portUnauthUntagPacketOper_get);
EXPORT_SYMBOL(rtk_dot1x_portUnauthUntagPacketOper_set);
EXPORT_SYMBOL(rtk_dot1x_eapolFrame2CpuEnable_get);
EXPORT_SYMBOL(rtk_dot1x_eapolFrame2CpuEnable_set);
EXPORT_SYMBOL(rtk_dot1x_portBasedEnable_get);
EXPORT_SYMBOL(rtk_dot1x_portBasedEnable_set);
EXPORT_SYMBOL(rtk_dot1x_portBasedAuthStatus_get);
EXPORT_SYMBOL(rtk_dot1x_portBasedAuthStatus_set);
EXPORT_SYMBOL(rtk_dot1x_portBasedDirection_get);
EXPORT_SYMBOL(rtk_dot1x_portBasedDirection_set);
EXPORT_SYMBOL(rtk_dot1x_macBasedEnable_get);
EXPORT_SYMBOL(rtk_dot1x_macBasedEnable_set);
EXPORT_SYMBOL(rtk_dot1x_macBasedAuthMac_add);
EXPORT_SYMBOL(rtk_dot1x_macBasedAuthMac_del);
EXPORT_SYMBOL(rtk_dot1x_macBasedDirection_get);
EXPORT_SYMBOL(rtk_dot1x_macBasedDirection_set);
EXPORT_SYMBOL(rtk_dot1x_portGuestVlan_get);
EXPORT_SYMBOL(rtk_dot1x_portGuestVlan_set);
EXPORT_SYMBOL(rtk_dot1x_guestVlanBehavior_get);
EXPORT_SYMBOL(rtk_dot1x_guestVlanBehavior_set);
EXPORT_SYMBOL(rtk_dot1x_guestVlanRouteBehavior_get);
EXPORT_SYMBOL(rtk_dot1x_guestVlanRouteBehavior_set);
EXPORT_SYMBOL(rtk_dot1x_trapPri_get);
EXPORT_SYMBOL(rtk_dot1x_trapPri_set);
EXPORT_SYMBOL(rtk_dot1x_trapPriEnable_get);
EXPORT_SYMBOL(rtk_dot1x_trapPriEnable_set);
EXPORT_SYMBOL(rtk_dot1x_trapAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_dot1x_trapAddCPUTagEnable_set);

EXPORT_SYMBOL(rtk_filter_init);
EXPORT_SYMBOL(rtk_filter_blkCutline_get);
EXPORT_SYMBOL(rtk_filter_blkCutline_set);
EXPORT_SYMBOL(rtk_filter_pieEnable_get);
EXPORT_SYMBOL(rtk_filter_pieEnable_set);
EXPORT_SYMBOL(rtk_filter_patternMatch_get);
EXPORT_SYMBOL(rtk_filter_patternMatch_set);
EXPORT_SYMBOL(rtk_filter_flowTbl_del);
EXPORT_SYMBOL(rtk_filter_flowTbl_delAll);
EXPORT_SYMBOL(rtk_filter_flowTbl_get);
EXPORT_SYMBOL(rtk_filter_flowTbl_set);
EXPORT_SYMBOL(rtk_filter_flowTbl_add);
EXPORT_SYMBOL(rtk_filter_flowTbl_validate);
EXPORT_SYMBOL(rtk_filter_flowTbl_invalidate);
EXPORT_SYMBOL(rtk_filter_igrAcl_del);
EXPORT_SYMBOL(rtk_filter_igrAcl_delAll);
EXPORT_SYMBOL(rtk_filter_igrAcl_get);
EXPORT_SYMBOL(rtk_filter_igrAcl_set);
EXPORT_SYMBOL(rtk_filter_igrAcl_add);
EXPORT_SYMBOL(rtk_filter_igrAcl_validate);
EXPORT_SYMBOL(rtk_filter_igrAcl_invalidate);
EXPORT_SYMBOL(rtk_filter_igrAclRateLimit_get);
EXPORT_SYMBOL(rtk_filter_igrAclRateLimit_set);
EXPORT_SYMBOL(rtk_filter_stat_get);
EXPORT_SYMBOL(rtk_filter_stat_set);
EXPORT_SYMBOL(rtk_filter_macBasedVlan_add);
EXPORT_SYMBOL(rtk_filter_macBasedVlan_del);
EXPORT_SYMBOL(rtk_filter_macBasedVlan_delAll);
EXPORT_SYMBOL(rtk_filter_igrVlanXlate_add);
EXPORT_SYMBOL(rtk_filter_igrVlanXlate_del);
EXPORT_SYMBOL(rtk_filter_igrVlanXlate_delAll);
EXPORT_SYMBOL(rtk_filter_egrVlanXlate_add);
EXPORT_SYMBOL(rtk_filter_egrVlanXlate_del);
EXPORT_SYMBOL(rtk_filter_egrVlanXlate_delAll);
EXPORT_SYMBOL(rtk_filter_stagVlan_add);
EXPORT_SYMBOL(rtk_filter_stagVlan_del);
EXPORT_SYMBOL(rtk_filter_stagVlan_delAll);
EXPORT_SYMBOL(rtk_filter_ipSubnetBasedVlan_add);
EXPORT_SYMBOL(rtk_filter_ipSubnetBasedVlan_del);
EXPORT_SYMBOL(rtk_filter_ipSubnetBasedVlan_delAll);
EXPORT_SYMBOL(rtk_filter_protoAndPortBasedVlan_add);
EXPORT_SYMBOL(rtk_filter_protoAndPortBasedVlan_del);
EXPORT_SYMBOL(rtk_filter_protoAndPortBasedVlan_delAll);

EXPORT_SYMBOL(rtk_flowctrl_init);
EXPORT_SYMBOL(rtk_flowctrl_portEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_portEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_portPauseForceModeEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_portPauseForceModeEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAction_get);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAction_set);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAllowedPageNum_get);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAllowedPageNum_set);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAllowedPktNum_get);
EXPORT_SYMBOL(rtk_flowctrl_pauseOnAllowedPktNum_set);
EXPORT_SYMBOL(rtk_flowctrl_igrSystemPauseThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrSystemPauseThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_igrPortPauseThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrPortPauseThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrSystemPauseThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrSystemPauseThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortPauseThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortPauseThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortQueuePauseThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortQueuePauseThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropMode_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropMode_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropForceModeEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropForceModeEnable_set);
EXPORT_SYMBOL(rtk_flowctrl_igrSystemCongestThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrSystemCongestThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_igrPortCongestThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_igrPortCongestThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrSystemDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrSystemDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortQueueDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortQueueDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrQueueDropThresh_get);
EXPORT_SYMBOL(rtk_flowctrl_egrQueueDropThresh_set);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropRefCongestEnable_get);
EXPORT_SYMBOL(rtk_flowctrl_egrPortDropRefCongestEnable_set);

EXPORT_SYMBOL(rtk_l2_init);
EXPORT_SYMBOL(rtk_l2_flushLinkDownPortAddrEnable_get);
EXPORT_SYMBOL(rtk_l2_flushLinkDownPortAddrEnable_set);
EXPORT_SYMBOL(rtk_l2_ucastAddr_flush);
EXPORT_SYMBOL(rtk_l2_learningCnt_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCntEnable_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCntEnable_set);
EXPORT_SYMBOL(rtk_l2_limitLearningCnt_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCnt_set);
EXPORT_SYMBOL(rtk_l2_limitLearningCntAction_get);
EXPORT_SYMBOL(rtk_l2_limitLearningCntAction_set);
EXPORT_SYMBOL(rtk_l2_portLastLearnedMac_get);
EXPORT_SYMBOL(rtk_l2_fidLimitLearningEntry_get);
EXPORT_SYMBOL(rtk_l2_fidLimitLearningEntry_set);
EXPORT_SYMBOL(rtk_l2_fidLearningCnt_get);
EXPORT_SYMBOL(rtk_l2_fidLearningCnt_reset);
EXPORT_SYMBOL(rtk_l2_fidLastLearnedMac_get);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapPri_get);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapPri_set);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapPriEnable_get);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapPriEnable_set);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapDP_get);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapDP_set);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapDPEnable_get);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapDPEnable_set);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_l2_limitLearningTrapAddCPUTagEnable_set);
EXPORT_SYMBOL(rtk_l2_aging_get);
EXPORT_SYMBOL(rtk_l2_aging_set);
EXPORT_SYMBOL(rtk_l2_camEnable_get);
EXPORT_SYMBOL(rtk_l2_camEnable_set);
EXPORT_SYMBOL(rtk_l2_hashAlgo_get);
EXPORT_SYMBOL(rtk_l2_hashAlgo_set);
EXPORT_SYMBOL(rtk_l2_vlanMode_get);
EXPORT_SYMBOL(rtk_l2_vlanMode_set);
EXPORT_SYMBOL(rtk_l2_learningEnable_get);
EXPORT_SYMBOL(rtk_l2_learningEnable_set);
EXPORT_SYMBOL(rtk_l2_newMacOp_get);
EXPORT_SYMBOL(rtk_l2_newMacOp_set);
EXPORT_SYMBOL(rtk_l2_LRUEnable_get);
EXPORT_SYMBOL(rtk_l2_LRUEnable_set);
EXPORT_SYMBOL(rtk_l2_ucastLookupMode_get);
EXPORT_SYMBOL(rtk_l2_ucastLookupMode_set);
EXPORT_SYMBOL(rtk_l2_addr_init);
EXPORT_SYMBOL(rtk_l2_addr_add);
EXPORT_SYMBOL(rtk_l2_addr_del);
EXPORT_SYMBOL(rtk_l2_addr_get);
EXPORT_SYMBOL(rtk_l2_addr_set);
EXPORT_SYMBOL(rtk_l2_addr_delAll);
EXPORT_SYMBOL(rtk_l2_nextValidAddr_get);
EXPORT_SYMBOL(rtk_l2_nextValidMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_nextValidIpMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_mcastLookupMode_get);
EXPORT_SYMBOL(rtk_l2_mcastLookupMode_set);
EXPORT_SYMBOL(rtk_l2_mcastBlockPortmask_get);
EXPORT_SYMBOL(rtk_l2_mcastBlockPortmask_set);
EXPORT_SYMBOL(rtk_l2_mcastAddr_init);
EXPORT_SYMBOL(rtk_l2_mcastAddr_add);
EXPORT_SYMBOL(rtk_l2_mcastAddr_del);
EXPORT_SYMBOL(rtk_l2_mcastAddr_get);
EXPORT_SYMBOL(rtk_l2_mcastAddr_set);
EXPORT_SYMBOL(rtk_l2_mcastAddr_add_with_index);
EXPORT_SYMBOL(rtk_l2_mcastAddr_get_with_index);
EXPORT_SYMBOL(rtk_l2_ipmcEnable_get);
EXPORT_SYMBOL(rtk_l2_ipmcEnable_set);
EXPORT_SYMBOL(rtk_l2_ipmcMode_get);
EXPORT_SYMBOL(rtk_l2_ipmcMode_set);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_init);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_add);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_del);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_get);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_set);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_add_with_index);
EXPORT_SYMBOL(rtk_l2_ipMcastAddr_get_with_index);
EXPORT_SYMBOL(rtk_l2_ipmc_routerPorts_get);
EXPORT_SYMBOL(rtk_l2_ipmc_routerPorts_set);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchAction_get);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchAction_set);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchPri_get);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchPri_set);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchPriEnable_get);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchPriEnable_set);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchDP_get);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchDP_set);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchDPEnable_get);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchDPEnable_set);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_l2_ipmcDstAddrMismatchAddCPUTagEnable_set);
EXPORT_SYMBOL(rtk_l2_mcastFwdIndex_alloc);
EXPORT_SYMBOL(rtk_l2_mcastFwdIndex_free);
EXPORT_SYMBOL(rtk_l2_mcastFwdIndexFreeCount_get);
EXPORT_SYMBOL(rtk_l2_mcastFwdPortmask_get);
EXPORT_SYMBOL(rtk_l2_mcastFwdPortmask_set);
EXPORT_SYMBOL(rtk_l2_cpuMacAddr_add);
EXPORT_SYMBOL(rtk_l2_cpuMacAddr_del);
EXPORT_SYMBOL(rtk_l2_legalMoveToPorts_get);
EXPORT_SYMBOL(rtk_l2_legalMoveToPorts_set);
EXPORT_SYMBOL(rtk_l2_illegalPortMoveAction_get);
EXPORT_SYMBOL(rtk_l2_illegalPortMoveAction_set);
EXPORT_SYMBOL(rtk_l2_legalPortMoveAction_get);
EXPORT_SYMBOL(rtk_l2_legalPortMoveAction_set);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_get);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_set);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_add);
EXPORT_SYMBOL(rtk_l2_lookupMissFloodPortMask_del);
EXPORT_SYMBOL(rtk_l2_lookupMissAction_get);
EXPORT_SYMBOL(rtk_l2_lookupMissAction_set);
EXPORT_SYMBOL(rtk_l2_lookupMissPri_get);
EXPORT_SYMBOL(rtk_l2_lookupMissPri_set);
EXPORT_SYMBOL(rtk_l2_lookupMissPriEnable_get);
EXPORT_SYMBOL(rtk_l2_lookupMissPriEnable_set);
EXPORT_SYMBOL(rtk_l2_lookupMissDP_get);
EXPORT_SYMBOL(rtk_l2_lookupMissDP_set);
EXPORT_SYMBOL(rtk_l2_lookupMissDPEnable_get);
EXPORT_SYMBOL(rtk_l2_lookupMissDPEnable_set);
EXPORT_SYMBOL(rtk_l2_lookupMissAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_l2_lookupMissAddCPUTagEnable_set);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_get);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_set);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_add);
EXPORT_SYMBOL(rtk_l2_srcPortEgrFilterMask_del);
EXPORT_SYMBOL(rtk_l2_exceptionAddrAction_get);
EXPORT_SYMBOL(rtk_l2_exceptionAddrAction_set);
EXPORT_SYMBOL(rtk_l2_trapPri_get);
EXPORT_SYMBOL(rtk_l2_trapPri_set);
EXPORT_SYMBOL(rtk_l2_trapPriEnable_get);
EXPORT_SYMBOL(rtk_l2_trapPriEnable_set);
EXPORT_SYMBOL(rtk_l2_trapAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_l2_trapAddCPUTagEnable_set);

EXPORT_SYMBOL(rtk_mirror_init);
EXPORT_SYMBOL(rtk_mirror_portBased_create);
EXPORT_SYMBOL(rtk_mirror_portBased_destroy);
EXPORT_SYMBOL(rtk_mirror_portBased_destroyAll);
EXPORT_SYMBOL(rtk_mirror_portBased_get);
EXPORT_SYMBOL(rtk_mirror_portBased_set);
EXPORT_SYMBOL(rtk_mirror_group_init);
EXPORT_SYMBOL(rtk_mirror_group_get);
EXPORT_SYMBOL(rtk_mirror_group_set);
EXPORT_SYMBOL(rtk_mirror_egrMode_get);
EXPORT_SYMBOL(rtk_mirror_egrMode_set);
EXPORT_SYMBOL(rtk_mirror_portRspanIgrMode_get);
EXPORT_SYMBOL(rtk_mirror_portRspanIgrMode_set);
EXPORT_SYMBOL(rtk_mirror_portRspanEgrMode_get);
EXPORT_SYMBOL(rtk_mirror_portRspanEgrMode_set);
EXPORT_SYMBOL(rtk_mirror_rspanIgrTag_get);
EXPORT_SYMBOL(rtk_mirror_rspanIgrTag_set);
EXPORT_SYMBOL(rtk_mirror_rspanEgrTag_get);
EXPORT_SYMBOL(rtk_mirror_rspanEgrTag_set);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSeed_get);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSeed_set);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSampleEnable_get);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSampleEnable_set);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSampleRate_get);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSampleRate_set);
EXPORT_SYMBOL(rtk_mirror_sflowMirrorSampleStat_get);
EXPORT_SYMBOL(rtk_mirror_sflowPortSeed_get);
EXPORT_SYMBOL(rtk_mirror_sflowPortSeed_set);
EXPORT_SYMBOL(rtk_mirror_sflowPortIgrSampleEnable_get);
EXPORT_SYMBOL(rtk_mirror_sflowPortIgrSampleEnable_set);
EXPORT_SYMBOL(rtk_mirror_sflowPortIgrSampleRate_get);
EXPORT_SYMBOL(rtk_mirror_sflowPortIgrSampleRate_set);
EXPORT_SYMBOL(rtk_mirror_sflowPortEgrSampleEnable_get);
EXPORT_SYMBOL(rtk_mirror_sflowPortEgrSampleEnable_set);
EXPORT_SYMBOL(rtk_mirror_sflowPortEgrSampleRate_get);
EXPORT_SYMBOL(rtk_mirror_sflowPortEgrSampleRate_set);
EXPORT_SYMBOL(rtk_mirror_sflowAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_mirror_sflowAddCPUTagEnable_set);

EXPORT_SYMBOL(rtk_port_init);
EXPORT_SYMBOL(rtk_port_link_get);
EXPORT_SYMBOL(rtk_port_adminEnable_get);
EXPORT_SYMBOL(rtk_port_adminEnable_set);
EXPORT_SYMBOL(rtk_port_speedDuplex_get);
EXPORT_SYMBOL(rtk_port_flowctrl_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoEnable_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoEnable_set);
EXPORT_SYMBOL(rtk_port_phyAutoNegoAbility_get);
EXPORT_SYMBOL(rtk_port_phyAutoNegoAbility_set);
EXPORT_SYMBOL(rtk_port_phyForceModeAbility_get);
EXPORT_SYMBOL(rtk_port_phyForceModeAbility_set);
EXPORT_SYMBOL(rtk_port_phyReg_get);
EXPORT_SYMBOL(rtk_port_phyReg_set);
EXPORT_SYMBOL(rtk_port_cpuPortId_get);
EXPORT_SYMBOL(rtk_port_isolation_get);
EXPORT_SYMBOL(rtk_port_isolation_set);
EXPORT_SYMBOL(rtk_port_isolation_add);
EXPORT_SYMBOL(rtk_port_isolation_del);
EXPORT_SYMBOL(rtk_port_phyComboPortMedia_get);
EXPORT_SYMBOL(rtk_port_phyComboPortMedia_set);
EXPORT_SYMBOL(rtk_port_macRemoteLoopbackEnable_get);
EXPORT_SYMBOL(rtk_port_macRemoteLoopbackEnable_set);
EXPORT_SYMBOL(rtk_port_macLocalLoopbackEnable_get);
EXPORT_SYMBOL(rtk_port_macLocalLoopbackEnable_set);
EXPORT_SYMBOL(rtk_port_backpressureEnable_get);
EXPORT_SYMBOL(rtk_port_backpressureEnable_set);
EXPORT_SYMBOL(rtk_port_rtctResult_get);
EXPORT_SYMBOL(rtk_port_rtctEnable_set);
EXPORT_SYMBOL(rtk_port_greenEnable_get);
EXPORT_SYMBOL(rtk_port_greenEnable_set);
EXPORT_SYMBOL(rtk_port_udldEnable_get);
EXPORT_SYMBOL(rtk_port_udldEnable_set);
EXPORT_SYMBOL(rtk_port_udldLinkUpAutoTriggerEnable_get);
EXPORT_SYMBOL(rtk_port_udldLinkUpAutoTriggerEnable_set);
EXPORT_SYMBOL(rtk_port_udldTrigger_start);
EXPORT_SYMBOL(rtk_port_udldStatus_get);
EXPORT_SYMBOL(rtk_port_udldAutoDisableFailedPortEnable_get);
EXPORT_SYMBOL(rtk_port_udldAutoDisableFailedPortEnable_set);
EXPORT_SYMBOL(rtk_port_udldInterval_get);
EXPORT_SYMBOL(rtk_port_udldInterval_set);
EXPORT_SYMBOL(rtk_port_udldRetryCount_get);
EXPORT_SYMBOL(rtk_port_udldRetryCount_set);
EXPORT_SYMBOL(rtk_port_udldLedIndicateEnable_get);
EXPORT_SYMBOL(rtk_port_udldLedIndicateEnable_set);
EXPORT_SYMBOL(rtk_port_udldEchoAction_get);
EXPORT_SYMBOL(rtk_port_udldEchoAction_set);
EXPORT_SYMBOL(rtk_port_udldLinkStatus_get);
EXPORT_SYMBOL(rtk_port_udldLinkStatus_set);
EXPORT_SYMBOL(rtk_port_rldpEnable_get);
EXPORT_SYMBOL(rtk_port_rldpEnable_set);
EXPORT_SYMBOL(rtk_port_rldpStatus_get);
EXPORT_SYMBOL(rtk_port_rldpAutoBlockEnable_get);
EXPORT_SYMBOL(rtk_port_rldpAutoBlockEnable_set);
EXPORT_SYMBOL(rtk_port_rldpInterval_get);
EXPORT_SYMBOL(rtk_port_rldpInterval_set);
EXPORT_SYMBOL(rtk_port_rldpSelfLoopAgingTime_get);
EXPORT_SYMBOL(rtk_port_rldpSelfLoopAgingTime_set);
EXPORT_SYMBOL(rtk_port_rldpNormalLoopAgingTime_get);
EXPORT_SYMBOL(rtk_port_rldpNormalLoopAgingTime_set);
EXPORT_SYMBOL(rtk_port_txEnable_get);
EXPORT_SYMBOL(rtk_port_txEnable_set);
EXPORT_SYMBOL(rtk_port_rxEnable_get);
EXPORT_SYMBOL(rtk_port_rxEnable_set);
EXPORT_SYMBOL(rtk_port_specialCongest_set);
EXPORT_SYMBOL(rtk_port_linkMon_enable);
EXPORT_SYMBOL(rtk_port_linkMon_disable);
EXPORT_SYMBOL(rtk_port_linkMon_register);
EXPORT_SYMBOL(rtk_port_linkMon_unregister);
EXPORT_SYMBOL(rtk_port_linkMon_swScanPorts_set);
EXPORT_SYMBOL(rtk_port_linkMon_swScanPorts_get);
EXPORT_SYMBOL(rtk_port_phyCrossOverMode_get);
EXPORT_SYMBOL(rtk_port_phyCrossOverMode_set);

EXPORT_SYMBOL(rtk_qos_init);
EXPORT_SYMBOL(rtk_qos_priSel_get);
EXPORT_SYMBOL(rtk_qos_priSel_set);
EXPORT_SYMBOL(rtk_qos_priSelGroup_get);
EXPORT_SYMBOL(rtk_qos_priSelGroup_set);
EXPORT_SYMBOL(rtk_qos_portPriSelGroup_get);
EXPORT_SYMBOL(rtk_qos_portPriSelGroup_set);
EXPORT_SYMBOL(rtk_qos_portPri_get);
EXPORT_SYMBOL(rtk_qos_portPri_set);
EXPORT_SYMBOL(rtk_qos_portDp_get);
EXPORT_SYMBOL(rtk_qos_portDp_set);
EXPORT_SYMBOL(rtk_qos_portInnerPri_get);
EXPORT_SYMBOL(rtk_qos_portInnerPri_set);
EXPORT_SYMBOL(rtk_qos_portOuterPri_get);
EXPORT_SYMBOL(rtk_qos_portOuterPri_set);
EXPORT_SYMBOL(rtk_qos_portOuterDEI_get);
EXPORT_SYMBOL(rtk_qos_portOuterDEI_set);
EXPORT_SYMBOL(rtk_qos_dscpPriRemap_get);
EXPORT_SYMBOL(rtk_qos_dscpPriRemap_set);
EXPORT_SYMBOL(rtk_qos_dscpPriRemapGroup_get);
EXPORT_SYMBOL(rtk_qos_dscpPriRemapGroup_set);
EXPORT_SYMBOL(rtk_qos_portDscpPriRemapGroup_get);
EXPORT_SYMBOL(rtk_qos_portDscpPriRemapGroup_set);
EXPORT_SYMBOL(rtk_qos_1pPriRemap_get);
EXPORT_SYMBOL(rtk_qos_1pPriRemap_set);
EXPORT_SYMBOL(rtk_qos_1pPriRemapGroup_get);
EXPORT_SYMBOL(rtk_qos_1pPriRemapGroup_set);
EXPORT_SYMBOL(rtk_qos_port1pPriRemapGroup_get);
EXPORT_SYMBOL(rtk_qos_port1pPriRemapGroup_set);
EXPORT_SYMBOL(rtk_qos_outer1pPriRemapGroup_get);
EXPORT_SYMBOL(rtk_qos_outer1pPriRemapGroup_set);
EXPORT_SYMBOL(rtk_qos_portOuter1pPriRemapGroup_get);
EXPORT_SYMBOL(rtk_qos_portOuter1pPriRemapGroup_set);
EXPORT_SYMBOL(rtk_qos_queueNum_get);
EXPORT_SYMBOL(rtk_qos_queueNum_set);
EXPORT_SYMBOL(rtk_qos_priMap_get);
EXPORT_SYMBOL(rtk_qos_priMap_set);
EXPORT_SYMBOL(rtk_qos_portPriMap_get);
EXPORT_SYMBOL(rtk_qos_portPriMap_set);
EXPORT_SYMBOL(rtk_qos_1pRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_1pRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_1pRemark_get);
EXPORT_SYMBOL(rtk_qos_1pRemark_set);
EXPORT_SYMBOL(rtk_qos_1pRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_1pRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_port1pRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_port1pRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_port1pPriMapGroup_get);
EXPORT_SYMBOL(rtk_qos_port1pPriMapGroup_set);
EXPORT_SYMBOL(rtk_qos_out1pRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_out1pRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_outer1pRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_outer1pRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_portOuter1pRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_portOuter1pRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_portOuter1pPriMapGroup_get);
EXPORT_SYMBOL(rtk_qos_portOuter1pPriMapGroup_set);
EXPORT_SYMBOL(rtk_qos_dscpRemarkEnable_get);
EXPORT_SYMBOL(rtk_qos_dscpRemarkEnable_set);
EXPORT_SYMBOL(rtk_qos_dscpRemark_get);
EXPORT_SYMBOL(rtk_qos_dscpRemark_set);
EXPORT_SYMBOL(rtk_qos_dscpRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_dscpRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_portdscpRemarkGroup_get);
EXPORT_SYMBOL(rtk_qos_portdscpRemarkGroup_set);
EXPORT_SYMBOL(rtk_qos_schedulingAlgorithm_get);
EXPORT_SYMBOL(rtk_qos_schedulingAlgorithm_set);
EXPORT_SYMBOL(rtk_qos_schedulingQueue_get);
EXPORT_SYMBOL(rtk_qos_schedulingQueue_set);
EXPORT_SYMBOL(rtk_qos_wfqFixedBandwidthEnable_get);
EXPORT_SYMBOL(rtk_qos_wfqFixedBandwidthEnable_set);
EXPORT_SYMBOL(rtk_qos_congAvoidAlgo_get);
EXPORT_SYMBOL(rtk_qos_congAvoidAlgo_set);
EXPORT_SYMBOL(rtk_qos_congAvoidQueueThreshEnable_get);
EXPORT_SYMBOL(rtk_qos_congAvoidQueueThreshEnable_set);
EXPORT_SYMBOL(rtk_qos_congAvoidPortThreshEnable_get);
EXPORT_SYMBOL(rtk_qos_congAvoidPortThreshEnable_set);
EXPORT_SYMBOL(rtk_qos_congAvoidSysThreshEnable_get);
EXPORT_SYMBOL(rtk_qos_congAvoidSysThreshEnable_set);
EXPORT_SYMBOL(rtk_qos_congAvoidSysThresh_get);
EXPORT_SYMBOL(rtk_qos_congAvoidSysThresh_set);
EXPORT_SYMBOL(rtk_qos_congAvoidPortThresh_get);
EXPORT_SYMBOL(rtk_qos_congAvoidPortThresh_set);
EXPORT_SYMBOL(rtk_qos_congAvoidQueueThresh_get);
EXPORT_SYMBOL(rtk_qos_congAvoidQueueThresh_set);
EXPORT_SYMBOL(rtk_qos_wredSysThresh_get);
EXPORT_SYMBOL(rtk_qos_wredSysThresh_set);
EXPORT_SYMBOL(rtk_qos_wredWeight_get);
EXPORT_SYMBOL(rtk_qos_wredWeight_set);
EXPORT_SYMBOL(rtk_qos_wredMpd_get);
EXPORT_SYMBOL(rtk_qos_wredMpd_set);
EXPORT_SYMBOL(rtk_qos_wredEcnEnable_get);
EXPORT_SYMBOL(rtk_qos_wredEcnEnable_set);
EXPORT_SYMBOL(rtk_qos_wredCntReverseEnable_get);
EXPORT_SYMBOL(rtk_qos_wredCntReverseEnable_set);

EXPORT_SYMBOL(rtk_rate_init);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_portIgrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_igrBandwidthFlowctrlEnable_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthFlowctrlEnable_set);
EXPORT_SYMBOL(rtk_rate_igrBandwidthFlowctrlThresh_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthFlowctrlThresh_set);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlFPEntry_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthCtrlFPEntry_set);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_egrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_portEgrBandwidthCtrlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_portEgrBandwidthCtrlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlEnable_get);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlEnable_set);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlRate_get);
EXPORT_SYMBOL(rtk_rate_egrQueueBwCtrlRate_set);
EXPORT_SYMBOL(rtk_rate_stormControlRate_get);
EXPORT_SYMBOL(rtk_rate_stormControlRate_set);
EXPORT_SYMBOL(rtk_rate_stormControlEnable_get);
EXPORT_SYMBOL(rtk_rate_stormControlEnable_set);
EXPORT_SYMBOL(rtk_rate_stormControlRateMode_get);
EXPORT_SYMBOL(rtk_rate_stormControlRateMode_set);
EXPORT_SYMBOL(rtk_rate_stormControlBurstRate_get);
EXPORT_SYMBOL(rtk_rate_stormControlBurstRate_set);
EXPORT_SYMBOL(rtk_rate_stormControlIncludeIfg_get);
EXPORT_SYMBOL(rtk_rate_stormControlIncludeIfg_set);
EXPORT_SYMBOL(rtk_rate_stormControlExceed_get);
EXPORT_SYMBOL(rtk_rate_igrBandwidthFCOffRate_set);
EXPORT_SYMBOL(rtk_rate_stormControlRefreshMode_get);
EXPORT_SYMBOL(rtk_rate_stormControlRefreshMode_set);

EXPORT_SYMBOL(rtk_stat_init);
EXPORT_SYMBOL(rtk_stat_global_reset);
EXPORT_SYMBOL(rtk_stat_port_reset);
EXPORT_SYMBOL(rtk_stat_global_get);
EXPORT_SYMBOL(rtk_stat_global_getAll);
EXPORT_SYMBOL(rtk_stat_port_get);
EXPORT_SYMBOL(rtk_stat_port_getAll);
EXPORT_SYMBOL(rtk_stat_smon_get);
EXPORT_SYMBOL(rtk_stat_smon_getAll);

EXPORT_SYMBOL(rtk_stp_init);
EXPORT_SYMBOL(rtk_stp_mstpInstance_create);
EXPORT_SYMBOL(rtk_stp_mstpInstance_destroy);
EXPORT_SYMBOL(rtk_stp_isMstpInstanceExist_get);
EXPORT_SYMBOL(rtk_stp_mstpState_get);
EXPORT_SYMBOL(rtk_stp_mstpState_set);

EXPORT_SYMBOL(rtk_svlan_init);
EXPORT_SYMBOL(rtk_svlan_create);
EXPORT_SYMBOL(rtk_svlan_destroy);
EXPORT_SYMBOL(rtk_svlan_portSvid_get);
EXPORT_SYMBOL(rtk_svlan_portSvid_set);
EXPORT_SYMBOL(rtk_svlan_servicePort_add);
EXPORT_SYMBOL(rtk_svlan_servicePort_del);
EXPORT_SYMBOL(rtk_svlan_servicePort_get);
EXPORT_SYMBOL(rtk_svlan_servicePort_set);
EXPORT_SYMBOL(rtk_svlan_memberPort_add);
EXPORT_SYMBOL(rtk_svlan_memberPort_del);
EXPORT_SYMBOL(rtk_svlan_memberPort_get);
EXPORT_SYMBOL(rtk_svlan_memberPort_set);
EXPORT_SYMBOL(rtk_svlan_memberPortEntry_get);
EXPORT_SYMBOL(rtk_svlan_memberPortEntry_set);
EXPORT_SYMBOL(rtk_svlan_nextValidMemberPortEntry_get);
EXPORT_SYMBOL(rtk_svlan_tpidEntry_get);
EXPORT_SYMBOL(rtk_svlan_tpidEntry_set);

EXPORT_SYMBOL(rtk_switch_init);
EXPORT_SYMBOL(rtk_switch_deviceInfo_get);
EXPORT_SYMBOL(rtk_switch_maxPktLen_get);
EXPORT_SYMBOL(rtk_switch_maxPktLen_set);
EXPORT_SYMBOL(rtk_switch_portMaxPktLen_get);
EXPORT_SYMBOL(rtk_switch_portMaxPktLen_set);
EXPORT_SYMBOL(rtk_switch_portSnapMode_get);
EXPORT_SYMBOL(rtk_switch_portSnapMode_set);
EXPORT_SYMBOL(rtk_switch_chksumFailAction_get);
EXPORT_SYMBOL(rtk_switch_chksumFailAction_set);
EXPORT_SYMBOL(rtk_switch_recalcCRCEnable_get);
EXPORT_SYMBOL(rtk_switch_recalcCRCEnable_set);
EXPORT_SYMBOL(rtk_switch_mgmtVlanId_get);
EXPORT_SYMBOL(rtk_switch_mgmtVlanId_set);
EXPORT_SYMBOL(rtk_switch_outerMgmtVlanId_get);
EXPORT_SYMBOL(rtk_switch_outerMgmtVlanId_set);
EXPORT_SYMBOL(rtk_switch_mgmtMacAddr_get);
EXPORT_SYMBOL(rtk_switch_mgmtMacAddr_set);
EXPORT_SYMBOL(rtk_switch_IPv4Addr_get);
EXPORT_SYMBOL(rtk_switch_IPv4Addr_set);
EXPORT_SYMBOL(rtk_switch_IPv6Addr_get);
EXPORT_SYMBOL(rtk_switch_IPv6Addr_set);
EXPORT_SYMBOL(rtk_switch_hwInterfaceDelayEnable_get);
EXPORT_SYMBOL(rtk_switch_hwInterfaceDelayEnable_set);

EXPORT_SYMBOL(rtk_trap_init);
EXPORT_SYMBOL(rtk_trap_1xMacChangePort2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_1xMacChangePort2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_igmpCtrlPkt2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_igmpCtrlPkt2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_l2McastPkt2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_l2McastPkt2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_ipMcastPkt2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_ipMcastPkt2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_reasonTrapToCpuPriority_get);
EXPORT_SYMBOL(rtk_trap_reasonTrapToCpuPriority_set);
EXPORT_SYMBOL(rtk_trap_pkt2CpuEnable_get);
EXPORT_SYMBOL(rtk_trap_pkt2CpuEnable_set);
EXPORT_SYMBOL(rtk_trap_rmaAction_get);
EXPORT_SYMBOL(rtk_trap_rmaAction_set);
EXPORT_SYMBOL(rtk_trap_rmaPri_get);
EXPORT_SYMBOL(rtk_trap_rmaPri_set);
EXPORT_SYMBOL(rtk_trap_rmaPriEnable_get);
EXPORT_SYMBOL(rtk_trap_rmaPriEnable_set);
EXPORT_SYMBOL(rtk_trap_rmaCpuTagAddEnable_get);
EXPORT_SYMBOL(rtk_trap_rmaCpuTagAddEnable_set);
EXPORT_SYMBOL(rtk_trap_rmaVlanCheckEnable_get);
EXPORT_SYMBOL(rtk_trap_rmaVlanCheckEnable_set);
EXPORT_SYMBOL(rtk_trap_userDefineRma_get);
EXPORT_SYMBOL(rtk_trap_userDefineRma_set);
EXPORT_SYMBOL(rtk_trap_userDefineRmaAction_get);
EXPORT_SYMBOL(rtk_trap_userDefineRmaAction_set);
EXPORT_SYMBOL(rtk_trap_userDefineRmaPri_get);
EXPORT_SYMBOL(rtk_trap_userDefineRmaPri_set);
EXPORT_SYMBOL(rtk_trap_userDefineRmaPriEnable_get);
EXPORT_SYMBOL(rtk_trap_userDefineRmaPriEnable_set);
EXPORT_SYMBOL(rtk_trap_userDefineRmaVlanCheckEnable_get);
EXPORT_SYMBOL(rtk_trap_userDefineRmaVlanCheckEnable_set);
EXPORT_SYMBOL(rtk_trap_userDefineRmaStpBlockEnable_get);
EXPORT_SYMBOL(rtk_trap_userDefineRmaStpBlockEnable_set);
EXPORT_SYMBOL(rtk_trap_mgmtFrameAction_get);
EXPORT_SYMBOL(rtk_trap_mgmtFrameAction_set);
EXPORT_SYMBOL(rtk_trap_mgmtFramePri_get);
EXPORT_SYMBOL(rtk_trap_mgmtFramePri_set);
EXPORT_SYMBOL(rtk_trap_mgmtFramePriEnable_get);
EXPORT_SYMBOL(rtk_trap_mgmtFramePriEnable_set);
EXPORT_SYMBOL(rtk_trap_mgmtFrameVlanCheck_get);
EXPORT_SYMBOL(rtk_trap_mgmtFrameVlanCheck_set);
EXPORT_SYMBOL(rtk_trap_userDefineMgmt_get);
EXPORT_SYMBOL(rtk_trap_userDefineMgmt_set);
EXPORT_SYMBOL(rtk_trap_userDefineMgmtAction_get);
EXPORT_SYMBOL(rtk_trap_userDefineMgmtAction_set);
EXPORT_SYMBOL(rtk_trap_userDefineMgmtPri_get);
EXPORT_SYMBOL(rtk_trap_userDefineMgmtPri_set);
EXPORT_SYMBOL(rtk_trap_userDefineMgmtPriEnable_get);
EXPORT_SYMBOL(rtk_trap_userDefineMgmtPriEnable_set);
EXPORT_SYMBOL(rtk_trap_userDefineMgmtVlanCheck_get);
EXPORT_SYMBOL(rtk_trap_userDefineMgmtVlanCheck_set);
EXPORT_SYMBOL(rtk_trap_portMgmtFrameAction_get);
EXPORT_SYMBOL(rtk_trap_portMgmtFrameAction_set);
EXPORT_SYMBOL(rtk_trap_portMgmtFramePri_get);
EXPORT_SYMBOL(rtk_trap_portMgmtFramePri_set);
EXPORT_SYMBOL(rtk_trap_portMgmtFramePriEnable_get);
EXPORT_SYMBOL(rtk_trap_portMgmtFramePriEnable_set);
EXPORT_SYMBOL(rtk_trap_portMgmtFrameVlanCheck_get);
EXPORT_SYMBOL(rtk_trap_portMgmtFrameVlanCheck_set);
EXPORT_SYMBOL(rtk_trap_portMgmtFrameCrossVlan_get);
EXPORT_SYMBOL(rtk_trap_portMgmtFrameCrossVlan_set);
EXPORT_SYMBOL(rtk_trap_ipWithOptionHeaderAction_get);
EXPORT_SYMBOL(rtk_trap_ipWithOptionHeaderAction_set);
EXPORT_SYMBOL(rtk_trap_ipWithOptionHeaderPri_get);
EXPORT_SYMBOL(rtk_trap_ipWithOptionHeaderPri_set);
EXPORT_SYMBOL(rtk_trap_ipWithOptionHeaderPriEnable_get);
EXPORT_SYMBOL(rtk_trap_ipWithOptionHeaderPriEnable_set);
EXPORT_SYMBOL(rtk_trap_ipWithOptionHeaderAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_trap_ipWithOptionHeaderAddCPUTagEnable_set);
EXPORT_SYMBOL(rtk_trap_pktWithCFIAction_get);
EXPORT_SYMBOL(rtk_trap_pktWithCFIAction_set);
EXPORT_SYMBOL(rtk_trap_pktWithCFIPri_get);
EXPORT_SYMBOL(rtk_trap_pktWithCFIPri_set);
EXPORT_SYMBOL(rtk_trap_pktWithCFIPriEnable_get);
EXPORT_SYMBOL(rtk_trap_pktWithCFIPriEnable_set);
EXPORT_SYMBOL(rtk_trap_pktWithCFIAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_trap_pktWithCFIAddCPUTagEnable_set);
EXPORT_SYMBOL(rtk_trap_cfmFrameAction_get);
EXPORT_SYMBOL(rtk_trap_cfmFrameAction_set);
EXPORT_SYMBOL(rtk_trap_cfmFrameTrapPri_get);
EXPORT_SYMBOL(rtk_trap_cfmFrameTrapPri_set);
EXPORT_SYMBOL(rtk_trap_cfmFrameTrapPriEnable_get);
EXPORT_SYMBOL(rtk_trap_cfmFrameTrapPriEnable_set);
EXPORT_SYMBOL(rtk_trap_cfmFrameTrapAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_trap_cfmFrameTrapAddCPUTagEnable_set);
EXPORT_SYMBOL(rtk_trap_oamPduAction_get);
EXPORT_SYMBOL(rtk_trap_oamPduAction_set);
EXPORT_SYMBOL(rtk_trap_oamPduPri_get);
EXPORT_SYMBOL(rtk_trap_oamPduPri_set);
EXPORT_SYMBOL(rtk_trap_oamPDUPriEnable_get);
EXPORT_SYMBOL(rtk_trap_oamPDUPriEnable_set);
EXPORT_SYMBOL(rtk_trap_oamPDUTrapAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_trap_oamPDUTrapAddCPUTagEnable_set);
EXPORT_SYMBOL(rtk_trap_mgmtIpCheck_get);
EXPORT_SYMBOL(rtk_trap_mgmtIpCheck_set);

EXPORT_SYMBOL(rtk_trunk_init);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithm_get);
EXPORT_SYMBOL(rtk_trunk_distributionAlgorithm_set);
EXPORT_SYMBOL(rtk_trunk_hashMappingTable_get);
EXPORT_SYMBOL(rtk_trunk_hashMappingTable_set);
EXPORT_SYMBOL(rtk_trunk_mode_get);
EXPORT_SYMBOL(rtk_trunk_mode_set);
EXPORT_SYMBOL(rtk_trunk_port_get);
EXPORT_SYMBOL(rtk_trunk_port_set);
EXPORT_SYMBOL(rtk_trunk_port_link_notification);
EXPORT_SYMBOL(rtk_trunk_representPort_get);
EXPORT_SYMBOL(rtk_trunk_representPort_set);
EXPORT_SYMBOL(rtk_trunk_floodMode_get);
EXPORT_SYMBOL(rtk_trunk_floodMode_set);
EXPORT_SYMBOL(rtk_trunk_floodPort_get);
EXPORT_SYMBOL(rtk_trunk_floodPort_set);

EXPORT_SYMBOL(rtk_vlan_init);
EXPORT_SYMBOL(rtk_vlan_create);
EXPORT_SYMBOL(rtk_vlan_destroy);
EXPORT_SYMBOL(rtk_vlan_destroyAll);
EXPORT_SYMBOL(rtk_vlan_fid_get);
EXPORT_SYMBOL(rtk_vlan_fid_set);
EXPORT_SYMBOL(rtk_vlan_port_add);
EXPORT_SYMBOL(rtk_vlan_port_del);
EXPORT_SYMBOL(rtk_vlan_port_get);
EXPORT_SYMBOL(rtk_vlan_port_set);
EXPORT_SYMBOL(rtk_vlan_stg_get);
EXPORT_SYMBOL(rtk_vlan_stg_set);
EXPORT_SYMBOL(rtk_vlan_portAcceptFrameType_get);
EXPORT_SYMBOL(rtk_vlan_portAcceptFrameType_set);
EXPORT_SYMBOL(rtk_vlan_portOuterAcceptFrameType_get);
EXPORT_SYMBOL(rtk_vlan_portOuterAcceptFrameType_set);
EXPORT_SYMBOL(rtk_vlan_vlanFunctionEnable_get);
EXPORT_SYMBOL(rtk_vlan_vlanFunctionEnable_set);
EXPORT_SYMBOL(rtk_vlan_portIgrFilterEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrFilterEnable_set);
EXPORT_SYMBOL(rtk_vlan_igrFilterEnable_get);
EXPORT_SYMBOL(rtk_vlan_igrFilterEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrFilterEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrFilterEnable_set);
EXPORT_SYMBOL(rtk_vlan_mcastLeakyEnable_get);
EXPORT_SYMBOL(rtk_vlan_mcastLeakyEnable_set);
EXPORT_SYMBOL(rtk_vlan_mcastLeakyPortEnable_get);
EXPORT_SYMBOL(rtk_vlan_mcastLeakyPortEnable_set);
EXPORT_SYMBOL(rtk_vlan_portPvid_get);
EXPORT_SYMBOL(rtk_vlan_portPvid_set);
EXPORT_SYMBOL(rtk_vlan_portOuterPvid_get);
EXPORT_SYMBOL(rtk_vlan_portOuterPvid_set);
EXPORT_SYMBOL(rtk_vlan_protoGroup_get);
EXPORT_SYMBOL(rtk_vlan_protoGroup_set);
EXPORT_SYMBOL(rtk_vlan_portProtoVlan_get);
EXPORT_SYMBOL(rtk_vlan_portProtoVlan_set);
EXPORT_SYMBOL(rtk_vlan_portOuterProtoVlan_get);
EXPORT_SYMBOL(rtk_vlan_portOuterProtoVlan_set);
EXPORT_SYMBOL(rtk_vlan_portTpidEntry_get);
EXPORT_SYMBOL(rtk_vlan_portTpidEntry_set);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTpidMode_get);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTpidMode_set);
EXPORT_SYMBOL(rtk_vlan_portIgrInnerTpid_get);
EXPORT_SYMBOL(rtk_vlan_portIgrInnerTpid_set);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTpid_get);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTpid_set);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTpidMode_get);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTpidMode_set);
EXPORT_SYMBOL(rtk_vlan_portIgrOuterTpid_get);
EXPORT_SYMBOL(rtk_vlan_portIgrOuterTpid_set);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTpid_get);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTpid_set);
EXPORT_SYMBOL(rtk_vlan_portIgrExtraTpid_get);
EXPORT_SYMBOL(rtk_vlan_portIgrExtraTpid_set);
EXPORT_SYMBOL(rtk_vlan_portIgrIgnoreInnerTagEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrIgnoreInnerTagEnable_set);
EXPORT_SYMBOL(rtk_vlan_portIgrIgnoreOuterTagEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrIgnoreOuterTagEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTagEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerTagEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTagEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterTagEnable_set);
EXPORT_SYMBOL(rtk_vlan_portIgrExtraTagEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrExtraTagEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrExtraTagEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrExtraTagEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerVidSource_get);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerVidSource_set);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerPriSource_get);
EXPORT_SYMBOL(rtk_vlan_portEgrInnerPriSource_set);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterVidSource_get);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterVidSource_set);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterPriSource_get);
EXPORT_SYMBOL(rtk_vlan_portEgrOuterPriSource_set);
EXPORT_SYMBOL(rtk_vlan_tagMode_get);
EXPORT_SYMBOL(rtk_vlan_tagMode_set);
EXPORT_SYMBOL(rtk_vlan_portIgrTagKeepEnable_get);
EXPORT_SYMBOL(rtk_vlan_portIgrTagKeepEnable_set);
EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepEnable_get);
EXPORT_SYMBOL(rtk_vlan_portEgrTagKeepEnable_set);
EXPORT_SYMBOL(rtk_vlan_fwdMode_get);
EXPORT_SYMBOL(rtk_vlan_fwdMode_set);

EXPORT_SYMBOL(rtk_eee_init);
EXPORT_SYMBOL(rtk_eee_portEnable_get);
EXPORT_SYMBOL(rtk_eee_portEnable_set);

EXPORT_SYMBOL(rtk_l3_init);
EXPORT_SYMBOL(rtk_l3_ttlExpireAction_get);
EXPORT_SYMBOL(rtk_l3_ttlExpireAction_set);
EXPORT_SYMBOL(rtk_l3_ttlExpireTrapPri_get);
EXPORT_SYMBOL(rtk_l3_ttlExpireTrapPri_set);
EXPORT_SYMBOL(rtk_l3_ttlExpireTrapPriEnable_get);
EXPORT_SYMBOL(rtk_l3_ttlExpireTrapPriEnable_set);
EXPORT_SYMBOL(rtk_l3_ttlExpireTrapDP_get);
EXPORT_SYMBOL(rtk_l3_ttlExpireTrapDP_set);
EXPORT_SYMBOL(rtk_l3_ttlExpireTrapDPEnable_get);
EXPORT_SYMBOL(rtk_l3_ttlExpireTrapDPEnable_set);
EXPORT_SYMBOL(rtk_l3_ttlExpireAddCPUTagEnable_get);
EXPORT_SYMBOL(rtk_l3_ttlExpireAddCPUTagEnable_set);

EXPORT_SYMBOL(rtk_oam_init);
EXPORT_SYMBOL(rtk_oam_oamCounter_get);
EXPORT_SYMBOL(rtk_oam_txTestFrame_start);
EXPORT_SYMBOL(rtk_oam_txTestFrame_stop);
EXPORT_SYMBOL(rtk_oam_txStatusOfTestFrame_get);
EXPORT_SYMBOL(rtk_oam_loopbackMode_get);
EXPORT_SYMBOL(rtk_oam_loopbackMode_set);
EXPORT_SYMBOL(rtk_oam_loopbackCtrl_get);
EXPORT_SYMBOL(rtk_oam_loopbackCtrl_set);
EXPORT_SYMBOL(rtk_oam_DyingGaspSend_start);
EXPORT_SYMBOL(rtk_oam_autoDyingGaspEnable_get);
EXPORT_SYMBOL(rtk_oam_autoDyingGaspEnable_set);
EXPORT_SYMBOL(rtk_oam_dyingGaspTLV_get);
EXPORT_SYMBOL(rtk_oam_dyingGaspTLV_set);
EXPORT_SYMBOL(rtk_oam_dyingGaspWaitTime_get);
EXPORT_SYMBOL(rtk_oam_dyingGaspWaitTime_set);
EXPORT_SYMBOL(rtk_oam_cfmEntry_get);
EXPORT_SYMBOL(rtk_oam_cfmEntry_set);
EXPORT_SYMBOL(rtk_oam_cfmPortEntry_get);
EXPORT_SYMBOL(rtk_oam_cfmPortEntry_set);
EXPORT_SYMBOL(rtk_oam_cfmMepEnable_get);
EXPORT_SYMBOL(rtk_oam_cfmMepEnable_set);
EXPORT_SYMBOL(rtk_oam_txCCMFrame_start);
EXPORT_SYMBOL(rtk_oam_txCCMFrame_stop);
EXPORT_SYMBOL(rtk_oam_cfmCCMFrame_get);
EXPORT_SYMBOL(rtk_oam_cfmCCMFrame_set);
EXPORT_SYMBOL(rtk_oam_cfmCCMSnapOui_get);
EXPORT_SYMBOL(rtk_oam_cfmCCMSnapOui_set);
EXPORT_SYMBOL(rtk_oam_cfmCCMEtype_get);
EXPORT_SYMBOL(rtk_oam_cfmCCMEtype_set);
EXPORT_SYMBOL(rtk_oam_cfmCCMOpcode_get);
EXPORT_SYMBOL(rtk_oam_cfmCCMOpcode_set);
EXPORT_SYMBOL(rtk_oam_cfmCCMFlag_get);
EXPORT_SYMBOL(rtk_oam_cfmCCMFlag_set);
EXPORT_SYMBOL(rtk_oam_cfmCCMInterval_get);
EXPORT_SYMBOL(rtk_oam_cfmCCMInterval_set);
EXPORT_SYMBOL(rtk_oam_cfmIntfStatus_get);
EXPORT_SYMBOL(rtk_oam_cfmIntfStatus_set);
EXPORT_SYMBOL(rtk_oam_cfmPortStatus_get);
EXPORT_SYMBOL(rtk_oam_cfmPortStatus_set);
EXPORT_SYMBOL(rtk_oam_cfmRemoteMep_del);
EXPORT_SYMBOL(rtk_oam_cfmRemoteMep_add);
EXPORT_SYMBOL(rtk_oam_cfmRemoteMep_get);
EXPORT_SYMBOL(rtk_oam_cfmRemoteMep_set);
EXPORT_SYMBOL(rtk_oam_cfmCCStatus_get);
EXPORT_SYMBOL(rtk_oam_cfmCCStatus_reset);
EXPORT_SYMBOL(rtk_oam_cfmLoopbackReplyEnable_get);
EXPORT_SYMBOL(rtk_oam_cfmLoopbackReplyEnable_set);
EXPORT_SYMBOL(rtk_oam_cfmLoopbackReplyCtrl_get);
EXPORT_SYMBOL(rtk_oam_cfmLoopbackReplyCtrl_set);

EXPORT_SYMBOL(rtk_pie_init);
EXPORT_SYMBOL(rtk_pie_pieRuleEntryFieldSize_get);
EXPORT_SYMBOL(rtk_pie_pieRuleEntrySize_get);
EXPORT_SYMBOL(rtk_pie_pieRuleFieldId_get);
EXPORT_SYMBOL(rtk_pie_pieRuleEntryField_get);
EXPORT_SYMBOL(rtk_pie_pieRuleEntryField_set);
EXPORT_SYMBOL(rtk_pie_pieRuleEntryField_read);
EXPORT_SYMBOL(rtk_pie_pieRuleEntryField_write);
EXPORT_SYMBOL(rtk_pie_piePreDefinedRuleEntry_get);
EXPORT_SYMBOL(rtk_pie_piePreDefinedRuleEntry_set);
EXPORT_SYMBOL(rtk_pie_pieRuleEntry_read);
EXPORT_SYMBOL(rtk_pie_pieRuleEntry_write);
EXPORT_SYMBOL(rtk_pie_pieRuleEntry_del);
EXPORT_SYMBOL(rtk_pie_pieRuleEntry_move);
EXPORT_SYMBOL(rtk_pie_pieRuleEntry_swap);
EXPORT_SYMBOL(rtk_pie_pieRuleAction_get);
EXPORT_SYMBOL(rtk_pie_pieRuleAction_set);
EXPORT_SYMBOL(rtk_pie_pieRuleAction_del);
EXPORT_SYMBOL(rtk_pie_pieRuleAction_move);
EXPORT_SYMBOL(rtk_pie_pieRuleAction_swap);
EXPORT_SYMBOL(rtk_pie_pieRulePolicer_get);
EXPORT_SYMBOL(rtk_pie_pieRulePolicer_set);
EXPORT_SYMBOL(rtk_pie_pieHitIndication_get);
EXPORT_SYMBOL(rtk_pie_pieStat_get);
EXPORT_SYMBOL(rtk_pie_pieStat_set);
EXPORT_SYMBOL(rtk_pie_pieStat_clearAll);
EXPORT_SYMBOL(rtk_pie_pieTemplateSelector_get);
EXPORT_SYMBOL(rtk_pie_pieTemplateSelector_set);
EXPORT_SYMBOL(rtk_pie_pieUserTemplate_get);
EXPORT_SYMBOL(rtk_pie_pieUserTemplate_set);
EXPORT_SYMBOL(rtk_pie_pieL34ChecksumErr_get);
EXPORT_SYMBOL(rtk_pie_pieL34ChecksumErr_set);
EXPORT_SYMBOL(rtk_pie_pieUserTemplatePayloadOffset_get);
EXPORT_SYMBOL(rtk_pie_pieUserTemplatePayloadOffset_set);
EXPORT_SYMBOL(rtk_pie_pieResultReverse_get);
EXPORT_SYMBOL(rtk_pie_pieResultReverse_set);
EXPORT_SYMBOL(rtk_pie_pieResultAggregator_get);
EXPORT_SYMBOL(rtk_pie_pieResultAggregator_set);
EXPORT_SYMBOL(rtk_pie_pieBlockPriority_get);
EXPORT_SYMBOL(rtk_pie_pieBlockPriority_set);
EXPORT_SYMBOL(rtk_pie_pieGroupCtrl_get);
EXPORT_SYMBOL(rtk_pie_pieGroupCtrl_set);
EXPORT_SYMBOL(rtk_pie_pieEgrAclLookupCtrl_get);
EXPORT_SYMBOL(rtk_pie_pieEgrAclLookupCtrl_set);
EXPORT_SYMBOL(rtk_pie_piePortLookupPhaseEnable_get);
EXPORT_SYMBOL(rtk_pie_piePortLookupPhaseEnable_set);
EXPORT_SYMBOL(rtk_pie_piePortLookupPhaseMiss_get);
EXPORT_SYMBOL(rtk_pie_piePortLookupPhaseMiss_set);
EXPORT_SYMBOL(rtk_pie_pieCounterIndicationMode_get);
EXPORT_SYMBOL(rtk_pie_pieCounterIndicationMode_set);
EXPORT_SYMBOL(rtk_pie_piePolicerCtrl_get);
EXPORT_SYMBOL(rtk_pie_piePolicerCtrl_set);
EXPORT_SYMBOL(rtk_pie_rangeCheckL4Port_get);
EXPORT_SYMBOL(rtk_pie_rangeCheckL4Port_set);
EXPORT_SYMBOL(rtk_pie_rangeCheckVid_get);
EXPORT_SYMBOL(rtk_pie_rangeCheckVid_set);
EXPORT_SYMBOL(rtk_pie_rangeCheckIp_get);
EXPORT_SYMBOL(rtk_pie_rangeCheckIp_set);
EXPORT_SYMBOL(rtk_pie_rangeCheckSrcPort_get);
EXPORT_SYMBOL(rtk_pie_rangeCheckSrcPort_set);
EXPORT_SYMBOL(rtk_pie_fieldSelectorEnable_get);
EXPORT_SYMBOL(rtk_pie_fieldSelectorEnable_set);
EXPORT_SYMBOL(rtk_pie_fieldSelectorContent_get);
EXPORT_SYMBOL(rtk_pie_fieldSelectorContent_set);
EXPORT_SYMBOL(rtk_pie_patternMatchEnable_get);
EXPORT_SYMBOL(rtk_pie_patternMatchEnable_set);
EXPORT_SYMBOL(rtk_pie_patternMatchContent_get);
EXPORT_SYMBOL(rtk_pie_patternMatchContent_set);

EXPORT_SYMBOL(rtk_sec_init);
EXPORT_SYMBOL(rtk_sec_attackPrevent_get);
EXPORT_SYMBOL(rtk_sec_attackPrevent_set);
EXPORT_SYMBOL(rtk_sec_minIPv6FragLen_get);
EXPORT_SYMBOL(rtk_sec_minIPv6FragLen_set);
EXPORT_SYMBOL(rtk_sec_maxPingLen_get);
EXPORT_SYMBOL(rtk_sec_maxPingLen_set);
EXPORT_SYMBOL(rtk_sec_minTCPHdrLen_get);
EXPORT_SYMBOL(rtk_sec_minTCPHdrLen_set);
EXPORT_SYMBOL(rtk_sec_smurfNetmaskLen_get);
EXPORT_SYMBOL(rtk_sec_smurfNetmaskLen_set);

EXPORT_SYMBOL(rtk_led_init);
EXPORT_SYMBOL(rtk_led_portEnable_get);
EXPORT_SYMBOL(rtk_led_portEnable_set);
EXPORT_SYMBOL(rtk_led_sysEnable_get);
EXPORT_SYMBOL(rtk_led_sysEnable_set);

EXPORT_SYMBOL(reg_idx2Addr_get);
EXPORT_SYMBOL(reg_idxMax_get);
EXPORT_SYMBOL(reg_info_get);
EXPORT_SYMBOL(rt_error_numToStr);
EXPORT_SYMBOL(table_write);
EXPORT_SYMBOL(table_read);

EXPORT_SYMBOL(dumpHsb);
#if defined(CONFIG_SDK_RTL8328)
EXPORT_SYMBOL(dumpPmi);
EXPORT_SYMBOL(dumpPpi);
#endif
#if defined(CONFIG_SDK_RTL8389)
EXPORT_SYMBOL(dumpHsa);
#endif

#if defined(CONFIG_SDK_DRIVER_TEST_MODULE) || defined(CONFIG_SDK_DRIVER_TEST)
EXPORT_SYMBOL(hal_init);
EXPORT_SYMBOL(phy_speed_get);
EXPORT_SYMBOL(hal_miim_read);
EXPORT_SYMBOL(hal_find_device);
EXPORT_SYMBOL(phy_identify_find);
EXPORT_SYMBOL(phy_identify_int_find);
EXPORT_SYMBOL(table_size_get);
EXPORT_SYMBOL(reg_write);

EXPORT_SYMBOL(reg_field_get);
EXPORT_SYMBOL(table_field_set);
EXPORT_SYMBOL(hal_isPpBlock_check);
EXPORT_SYMBOL(reg_field_set);
EXPORT_SYMBOL(table_field_byte_get);
EXPORT_SYMBOL(reg_array_field_write);
EXPORT_SYMBOL(hal_miim_write);
EXPORT_SYMBOL(table_field_get);
EXPORT_SYMBOL(phy_media_get);
EXPORT_SYMBOL(phy_duplex_get);
EXPORT_SYMBOL(hal_get_driver_id);
EXPORT_SYMBOL(reg_read);
EXPORT_SYMBOL(phy_enable_set);
EXPORT_SYMBOL(phy_speed_set);
EXPORT_SYMBOL(reg_array_read);
EXPORT_SYMBOL(reg_field_write);
EXPORT_SYMBOL(phy_identify_OUI_check);
EXPORT_SYMBOL(table_field_byte_set);
EXPORT_SYMBOL(reg_field_read);
EXPORT_SYMBOL(reg_array_field_read);
EXPORT_SYMBOL(reg_array_write);
EXPORT_SYMBOL(phy_autoNegoEnable_get);
EXPORT_SYMBOL(hal_ctrlInfo_get);
EXPORT_SYMBOL(hal_find_driver);
EXPORT_SYMBOL(phy_autoNegoAbility_set);
EXPORT_SYMBOL(phy_duplex_set);
EXPORT_SYMBOL(phy_identify_phyid_get);
EXPORT_SYMBOL(phy_autoNegoAbility_get);

EXPORT_SYMBOL(phy_autoNegoEnable_set);
EXPORT_SYMBOL(phy_media_set);
EXPORT_SYMBOL(hal_miim_portmask_write);

#endif

#if defined(CONFIG_SDK_DRIVER_TEST_MODULE) || defined(CONFIG_SDK_DRIVER_TEST) || defined(CONFIG_SDK_RTL8231)
/* Data */
EXPORT_SYMBOL(hal_ctrl);
#endif

#endif /* if 0 */
