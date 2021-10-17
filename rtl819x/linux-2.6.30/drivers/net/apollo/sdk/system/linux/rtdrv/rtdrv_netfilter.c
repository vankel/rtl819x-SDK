/*Copyright (C) 2009 Realtek Semiconductor Corp.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Purpose : Realtek Switch SDK Rtdrv Netfilter Module.
 *
 * Feature : Realtek Switch SDK Rtdrv Netfilter Module
 *
 */

/*
 * Include Files
 */
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <asm/uaccess.h>
#include <linux/netfilter.h>
#include <common/rt_error.h>
#include <osal/print.h>
#include <osal/lib.h>

#if 0
#include <common/debug/mem.h>

#include <hal/mac/mem.h>
#endif
#include <ioal/mem32.h>


#include <rtdrv/rtdrv_netfilter.h>

#include <rtk/irq.h>
#include <rtk/gpon.h>
#include <ioal/mac_debug.h>

/*
 * Symbol Definition
 */

/*
 * Data Declaration
 */
extern struct nf_sockopt_ops rtdrv_sockopts;


extern int io_mii_memory_write(uint32 memaddr,uint32 data);
extern uint32 io_mii_memory_read(uint32 memaddr);
extern int io_mii_phy_reg_write(uint8 phy_id,uint8 reg, uint16 value);
extern int io_mii_phy_reg_read(uint8 phy_id,uint8 reg, uint16 *pValue);
extern void gpon_dbg_enable(int32 enable);
extern int32 sdktest_run_id(uint32 unit, uint32 start, uint32 end);

/*
 * Macro Declaration
 */

/*
 * Function Declaration
 */


/* Function Name:
 *      do_rtdrv_set_ctl
 * Description:
 *      This function is called whenever a process tries to do setsockopt
 * Input:
 *      *sk   - network layer representation of sockets
 *      cmd   - ioctl commands
 *      *user - data buffer handled between user and kernel space
 *      len   - data length
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 do_rtdrv_set_ctl(struct sock *sk, int cmd, void *user, unsigned int len)
{
    int32                           ret = RT_ERR_FAILED;
    rtdrv_union_t                   buf;

    switch(cmd)
    {

    /** ADDR **/
        case RTDRV_REG_ADDRESS_SET:

            copy_from_user(&buf.addr_cfg, user, sizeof(rtdrv_addrCfg_t));

#if defined(LINUX_KERNEL_MDIO_IO)
            io_mii_memory_write(buf.addr_cfg.address, buf.addr_cfg.value);
#else
            MEM32_WRITE(buf.addr_cfg.address, buf.addr_cfg.value);
#endif
            ret = RT_ERR_OK;
            break;


        case RTDRV_PHY_REG_SET:

            copy_from_user(&buf.phy_cfg, user, sizeof(rtdrv_phyCfg_t));

            //printk("\nRTDRV_PHY_REG_SET id:%d reg:%d  data 0x%x\n",buf.phy_cfg.phy_id,
            //                                                       buf.phy_cfg.reg,
            //                                                       buf.phy_cfg.value);
#if defined(LINUX_KERNEL_MDIO_IO)
            io_mii_phy_reg_write(buf.phy_cfg.phy_id,buf.phy_cfg.reg,buf.phy_cfg.value);
#endif
            ret = RT_ERR_OK;
            break;

        case RTDRV_SOC_ADDRESS_SET:
            copy_from_user(&buf.phy_cfg, user, sizeof(rtdrv_addrCfg_t));
            MEM32_WRITE(buf.addr_cfg.address, buf.addr_cfg.value);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_SN_SET:
            copy_from_user(&buf.sn_cfg, user, sizeof(rtk_gpon_serialNumber_t));
            ret = rtk_gpon_serialNumber_set(&buf.sn_cfg);
            break;

        case RTDRV_GPON_PWD_SET:
            copy_from_user(&buf.pwd_cfg, user, sizeof(rtk_gpon_password_t));
            ret = rtk_gpon_password_set(&buf.pwd_cfg);
            break;

        case RTDRV_GPON_PARA_SET:
            copy_from_user(&buf.pon_cfg, user, sizeof(rtdrv_ponPara_t));
            ret = rtk_gpon_parameter_set(buf.pon_cfg.type, &buf.pon_cfg.para);
            break;

        case RTDRV_GPON_ACTIVE:
            copy_from_user(&buf.init_state_cfg, user, sizeof(rtk_gpon_initialState_t));
            ret = rtk_gpon_activate(buf.init_state_cfg);
            break;

        case RTDRV_GPON_DEACTIVE:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_deActivate();
            break;

        case RTDRV_GPON_TCONT_ADD:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gpon_tcont_create(&buf.tcont_cfg.ind, &buf.tcont_cfg.attr);
            copy_to_user(user, &buf.tcont_cfg, sizeof(rtdrv_tcont_t));
            break;

        case RTDRV_GPON_TCONT_DEL:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gpon_tcont_destroy(&buf.tcont_cfg.ind);
            break;

        case RTDRV_GPON_DS_FLOW_ADD:
            copy_from_user(&buf.ds_flow_cfg, user, sizeof(rtdrv_dsFlow_t));
            ret = rtk_gpon_dsFlow_set(buf.ds_flow_cfg.flowId, &buf.ds_flow_cfg.attr);
            break;

        case RTDRV_GPON_US_FLOW_ADD:
            copy_from_user(&buf.us_flow_cfg, user, sizeof(rtdrv_usFlow_t));
            ret = rtk_gpon_usFlow_set(buf.us_flow_cfg.flowId, &buf.us_flow_cfg.attr);
            break;

        case RTDRV_GPON_BC_PASS_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_broadcastPass_set(buf.state);
            break;

        case RTDRV_GPON_NON_MC_PASS_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_nonMcastPass_set(buf.state);
            break;

        case RTDRV_GPON_IP_PTN_SET:
            copy_from_user(&buf.ip_ptn, user, sizeof(rtdrv_ip_ptn_t));
            ret = rtk_gpon_multicastAddrCheck_set(buf.ip_ptn.ipv4_pattern, buf.ip_ptn.ipv6_pattern);
            break;

        case RTDRV_GPON_FLT_MODE_SET:
            copy_from_user(&buf.filter_mode, user, sizeof(rtk_gpon_macTable_exclude_mode_t));
            ret = rtk_gpon_macFilterMode_set(buf.filter_mode);
            break;

        case RTDRV_GPON_FS_MODE_SET:
            copy_from_user(&buf.fs_mode, user, sizeof(rtdrv_fs_mode_t));
            ret = rtk_gpon_mcForceMode_set(buf.fs_mode.ipv4, buf.fs_mode.ipv6);
            break;

        case RTDRV_GPON_MAC_ENTRY_ADD:
            copy_from_user(&buf.mac_entry.entry, user, sizeof(rtk_gpon_macTable_entry_t));
            ret = rtk_gpon_macEntry_add(&buf.mac_entry.entry);
            break;

        case RTDRV_GPON_MAC_ENTRY_DEL:
            copy_from_user(&buf.mac_entry.entry, user, sizeof(rtk_gpon_macTable_entry_t));
            ret = rtk_gpon_macEntry_del(&buf.mac_entry.entry);
            break;

        case RTDRV_GPON_RDI_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_rdi_set(buf.state);
            break;

        case RTDRV_GPON_PWR_LEVEL_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_powerLevel_set((uint8)(buf.state));
            break;

        case RTDRV_GPON_TX_LASER_SET:
            copy_from_user(&buf.tx_laser, user, sizeof(rtk_gpon_laser_status_t));
            ret = rtk_gpon_txForceLaser_set(buf.state);
            break;

        case RTDRV_GPON_FS_IDLE_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_txForceIdle_set(buf.state);
            break;
#if 0
        case RTDRV_GPON_FS_PRBS_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_txForcePRBS_set(buf.state);
            break;
#endif
        case RTDRV_GPON_INITIAL:
            copy_from_user(&buf.index, user, sizeof(uint32));
            ret = rtk_gpon_driver_initialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_driver_initialize err: 0x%x\n\r",ret);
                return ret;
            }

            ret = rtk_gpon_device_initialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_device_initialize err: 0x%x\n\r",ret);
                return ret;
            }
			/*remove it, because if init irq will implict other irq such as rldp, ptp, ...etc*/
			#if 0
	        ret = rtk_switch_irq_init(buf.index);
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_irq_reg err: 0x%x\n\r",ret);
                return ret;
            }
			#endif
            break;

        case RTDRV_GPON_DEINITIAL:
			
			/*remove it, because if init irq will implict other irq such as rldp, ptp, ...etc*/
			/*
            			ret = rtk_switch_irq_exit();
			*/
            ret = rtk_gpon_device_deInitialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_device_deInitialize err: 0x%x\n\r",ret);
                return ret;
            }

            ret = rtk_gpon_driver_deInitialize();
            if(ret != RT_ERR_OK)
            {
                osal_printf("rtk_gpon_driver_deInitialize err: 0x%x\n\r",ret);
                return ret;
            }

            break;

        case RTDRV_GPON_DEBUG_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            gpon_dbg_enable(buf.state);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_UNIT_TEST:
            copy_from_user(&buf.index, user, sizeof(uint32));
            sdktest_run_id(0, buf.index, buf.index);
            ret = RT_ERR_OK;
            break;

#if defined(OLD_FPGA_DEFINED)
        case RTDRV_GPON_PKTGEN_CFG:
            copy_from_user(&buf.pkt_gen, user, sizeof(rtdrv_pktGen_t));
            ret = rtk_gpon_pktGen_cfg_set(buf.pkt_gen.item, buf.pkt_gen.tcont, buf.pkt_gen.buf_len, buf.pkt_gen.gem, buf.pkt_gen.omci);
            break;

        case RTDRV_GPON_PKTGEN_BUF:
            copy_from_user(&buf.pkt_gen, user, sizeof(rtdrv_pktGen_t));
            ret = rtk_gpon_pktGen_buf_set(buf.pkt_gen.item, buf.pkt_gen.buf, buf.pkt_gen.buf_len);
            break;

#endif
        case RTDRV_GPON_OMCI_TX:
            copy_from_user(&buf.omci, user, sizeof(rtk_gpon_omci_msg_t));
            ret = rtk_gpon_omci_tx(&buf.omci);
            break;

        case RTDRV_GPON_AUTO_TCONT_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_autoTcont_set(buf.state);
            break;

        case RTDRV_GPON_AUTO_BOH_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_autoBoh_set(buf.state);
            break;

        case RTDRV_GPON_EQD_OFFSET_SET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_eqdOffset_set(buf.state);
            break;

        case RTDRV_PKT_RXDUMP_ENABLE:
            copy_from_user(&buf.index, user, sizeof(uint32));
            ret = mac_debug_rxPkt_enable (buf.index);
            break;

        case RTDRV_PKT_SEND:
            copy_from_user(&buf.pkt_info, user, sizeof(rtdrv_pktdbg_t));
            ret = mac_debug_txPkt_send(&buf.pkt_info.buf[0], buf.pkt_info.length, &buf.pkt_info.tx_info);
            break;

        case RTDRV_PKT_RXDUMP_CLEAR:
            ret = mac_debug_rxPkt_clear();
            break;

        default:
            break;
    }

	return ret;
}




/* Function Name:
 *      do_rtdrv_get_ctl
 * Description:
 *      This function is called whenever a process tries to do getsockopt
 * Input:
 *      *sk   - network layer representation of sockets
 *      cmd   - ioctl commands
 * Output:
 *      *user - data buffer handled between user and kernel space
 *      len   - data length
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
int32 do_rtdrv_get_ctl(struct sock *sk, int cmd, void *user, int *len)
{
    int32               ret = RT_ERR_FAILED;
    rtdrv_union_t       buf;


    switch(cmd)
    {
    /** ADDR **/
        case RTDRV_REG_ADDRESS_GET:
            copy_from_user(&buf.addr_cfg, user, sizeof(rtdrv_addrCfg_t));

#if defined(LINUX_KERNEL_MDIO_IO)
            buf.addr_cfg.value = io_mii_memory_read(buf.addr_cfg.address);
#else
            buf.addr_cfg.value = MEM32_READ(buf.addr_cfg.address);
#endif

            copy_to_user(user, &buf.addr_cfg, sizeof(rtdrv_addrCfg_t));
            ret = RT_ERR_OK;
            break;

        case RTDRV_PHY_REG_GET:
            copy_from_user(&buf.phy_cfg, user, sizeof(rtdrv_phyCfg_t));
#if defined(LINUX_KERNEL_MDIO_IO)
            io_mii_phy_reg_read(buf.phy_cfg.phy_id,buf.phy_cfg.reg,&buf.phy_cfg.value);
#endif
            //printk("\nRTDRV_PHY_REG_GET id:%d reg:%d  data 0x%x\n",buf.phy_cfg.phy_id,
            //                                                       buf.phy_cfg.reg,
            //                                                       buf.phy_cfg.value);

            copy_to_user(user, &buf.phy_cfg, sizeof(rtdrv_phyCfg_t));
            ret = RT_ERR_OK;
            break;

        case RTDRV_SOC_ADDRESS_GET:
            copy_from_user(&buf.addr_cfg, user, sizeof(rtdrv_addrCfg_t));
            buf.addr_cfg.value = MEM32_READ(buf.addr_cfg.address);
            copy_to_user(user, &buf.addr_cfg, sizeof(rtdrv_addrCfg_t));
            ret = RT_ERR_OK;
            break;


        case RTDRV_GPON_SN_GET:
            copy_from_user(&buf.sn_cfg, user, sizeof(rtk_gpon_serialNumber_t));
            ret = rtk_gpon_serialNumber_get(&buf.sn_cfg);
            copy_to_user(user, &buf.sn_cfg, sizeof(rtk_gpon_serialNumber_t));
            break;

        case RTDRV_GPON_PWD_GET:
            copy_from_user(&buf.pwd_cfg, user, sizeof(rtk_gpon_password_t));
            ret = rtk_gpon_password_get(&buf.pwd_cfg);
            copy_to_user(user, &buf.pwd_cfg, sizeof(rtk_gpon_password_t));
            break;

        case RTDRV_GPON_PARA_GET:
            copy_from_user(&buf.pon_cfg, user, sizeof(rtdrv_ponPara_t));
            osal_memset(&buf.pon_cfg.para, 0x0, sizeof(buf.pon_cfg.para));
            ret = rtk_gpon_parameter_get(buf.pon_cfg.type, &buf.pon_cfg.para);
            copy_to_user(user, &buf.pon_cfg, sizeof(rtdrv_ponPara_t));
            break;

        case RTDRV_GPON_STATE_GET:
            copy_from_user(&buf.pon_state, user, sizeof(rtk_gpon_fsm_status_t));
            ret = rtk_gpon_ponStatus_get(&buf.pon_state);
            copy_to_user(user, &buf.pon_state, sizeof(rtk_gpon_fsm_status_t));
            break;

        case RTDRV_GPON_TCONT_GET:
            copy_from_user(&buf.tcont_cfg, user, sizeof(rtdrv_tcont_t));
            ret = rtk_gpon_tcont_get(&buf.tcont_cfg.ind, &buf.tcont_cfg.attr);
            copy_to_user(user, &buf.tcont_cfg, sizeof(rtdrv_tcont_t));
            break;

        case RTDRV_GPON_DS_FLOW_GET:
            copy_from_user(&buf.ds_flow_cfg, user, sizeof(rtdrv_dsFlow_t));
            osal_memset(&buf.ds_flow_cfg.attr, 0x0, sizeof(buf.ds_flow_cfg.attr));
            ret = rtk_gpon_dsFlow_get(buf.ds_flow_cfg.flowId, &buf.ds_flow_cfg.attr);
            copy_to_user(user, &buf.ds_flow_cfg, sizeof(rtdrv_dsFlow_t));
            break;

        case RTDRV_GPON_US_FLOW_GET:
            copy_from_user(&buf.us_flow_cfg, user, sizeof(rtdrv_usFlow_t));
            osal_memset(&buf.us_flow_cfg.attr, 0x0, sizeof(buf.us_flow_cfg.attr));
            ret = rtk_gpon_usFlow_get(buf.us_flow_cfg.flowId, &buf.us_flow_cfg.attr);
            copy_to_user(user, &buf.us_flow_cfg, sizeof(rtdrv_usFlow_t));
            break;

        case RTDRV_GPON_BC_PASS_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_broadcastPass_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_NON_MC_PASS_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_nonMcastPass_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_IP_PTN_GET:
            copy_from_user(&buf.ip_ptn, user, sizeof(rtdrv_ip_ptn_t));
            ret = rtk_gpon_multicastAddrCheck_get(&buf.ip_ptn.ipv4_pattern, &buf.ip_ptn.ipv6_pattern);
            copy_to_user(user, &buf.ip_ptn, sizeof(rtdrv_ip_ptn_t));
            break;

        case RTDRV_GPON_FLT_MODE_GET:
            copy_from_user(&buf.filter_mode, user, sizeof(rtk_gpon_macTable_exclude_mode_t));
            ret = rtk_gpon_macFilterMode_get(&buf.filter_mode);
            copy_to_user(user, &buf.filter_mode, sizeof(rtk_gpon_macTable_exclude_mode_t));
            break;

        case RTDRV_GPON_FS_MODE_GET:
            copy_from_user(&buf.fs_mode, user, sizeof(rtdrv_fs_mode_t));
            ret = rtk_gpon_mcForceMode_get(&buf.fs_mode.ipv4, &buf.fs_mode.ipv6);
            copy_to_user(user, &buf.fs_mode, sizeof(rtdrv_fs_mode_t));
            break;

        case RTDRV_GPON_MAC_ENTRY_GET:
            copy_from_user(&buf.mac_entry, user, sizeof(rtdrv_mac_entry_t));
            ret = rtk_gpon_macEntry_get(buf.mac_entry.index, &buf.mac_entry.entry);
            copy_to_user(user, &buf.mac_entry, sizeof(rtdrv_mac_entry_t));
            break;

        case RTDRV_GPON_RDI_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_rdi_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_PWR_LEVEL_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            {
                uint8 level;
                ret = rtk_gpon_powerLevel_get(&level);
                buf.state = level;
            }
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_ALARM_GET:
            copy_from_user(&buf.alarm_state, user, sizeof(rtdrv_ponAlarm_t));
            ret = rtk_gpon_alarmStatus_get(buf.alarm_state.type, &buf.alarm_state.status);
            copy_to_user(user, &buf.alarm_state, sizeof(rtdrv_ponAlarm_t));
            break;

        case RTDRV_GPON_TX_LASER_GET:
            copy_from_user(&buf.tx_laser, user, sizeof(rtk_gpon_laser_status_t));
            ret = rtk_gpon_txForceLaser_get(&buf.tx_laser);
            copy_to_user(user, &buf.tx_laser, sizeof(rtk_gpon_laser_status_t));
            break;

        case RTDRV_GPON_FS_IDLE_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_txForceIdle_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;
#if 0
        case RTDRV_GPON_FS_PRBS_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_txForcePRBS_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;
#endif
        case RTDRV_GPON_DS_FEC_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_dsFecSts_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_VERSION_SHOW:
            rtk_gpon_version_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_DEV_SHOW:
            rtk_gpon_devInfo_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_GTC_SHOW:
            rtk_gpon_gtc_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_TCONT_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gpon_tcont_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_DS_FLOW_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gpon_dsFlow_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_US_FLOW_SHOW:
            copy_from_user(&buf.index, user, sizeof(uint32));
            rtk_gpon_usFlow_show(buf.index);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_MAC_TABLE_SHOW:
            rtk_gpon_macTable_show();
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_GLB_CNT_SHOW:
            copy_from_user(&buf.glb_cnt, user, sizeof(rtk_gpon_global_performance_type_t));
            rtk_gpon_globalCounter_show(buf.glb_cnt);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_TCONT_CNT_SHOW:
            copy_from_user(&buf.tcont_cnt, user, sizeof(rtdrv_tcont_cnt_t));
            rtk_gpon_tcontCounter_show(buf.tcont_cnt.tcont, buf.tcont_cnt.cnt_type);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_FLOW_CNT_SHOW:
            copy_from_user(&buf.flow_cnt, user, sizeof(rtdrv_flow_cnt_t));
            rtk_gpon_flowCounter_show(buf.flow_cnt.flow, buf.flow_cnt.cnt_type);
            ret = RT_ERR_OK;
            break;

        case RTDRV_GPON_AUTO_TCONT_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_autoTcont_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_AUTO_BOH_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_autoBoh_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_GPON_EQD_OFFSET_GET:
            copy_from_user(&buf.state, user, sizeof(int32));
            ret = rtk_gpon_eqdOffset_get(&buf.state);
            copy_to_user(user, &buf.state, sizeof(int32));
            break;

        case RTDRV_PKT_RXDUMP_GET:
            copy_from_user(&buf.pkt_info, user, sizeof(rtdrv_pktdbg_t));
            ret = RT_ERR_OK;
            mac_debug_rxPkt_get(&buf.pkt_info.buf[0], buf.pkt_info.length,
                (void *)&buf.pkt_info.rx_length, &buf.pkt_info.rx_info,
                &buf.pkt_info.enable);
            copy_to_user(user, &buf.pkt_info, sizeof(rtdrv_pktdbg_t));
            break;

        default:
            break;
    }

	return ret;
}




/* Function Name:
 *      rtdrv_init
 * Description:
 *      Init driver and register netfilter socket option
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      RT_ERR_OK
 *      RT_ERR_FAILED
 * Note:
 *      None
 */
static int32 __init rtdrv_init(void)
{
	printk("\n rtdrv_init !!! for netfilter\n");

	/* register netfilter socket option */
    if (nf_register_sockopt(&rtdrv_sockopts))
    {
        osal_printf("[%s]: nf_register_sockopt failed.\n", __FUNCTION__);
        return RT_ERR_FAILED;
    }

#if defined(__RTDRV_MODULE__)
    osal_printf("Init RTDRV Driver Module....OK\n");
#endif

    return RT_ERR_OK;
}

/* Function Name:
 *      rtdrv_exit
 * Description:
 *      Exit driver and unregister netfilter socket option
 * Input:
 *      None
 * Output:
 *      None
 * Return:
 *      None
 * Note:
 *      None
 */
static void __exit rtdrv_exit(void)
{
    nf_unregister_sockopt(&rtdrv_sockopts);

#if defined(__RTDRV_MODULE__)
    osal_printf("Exit RTDRV Driver Module....OK\n");
#endif

}

struct nf_sockopt_ops rtdrv_sockopts = {
	{ NULL, NULL }, PF_INET,
	RTDRV_BASE_CTL, RTDRV_SET_MAX+1, do_rtdrv_set_ctl, NULL,
	RTDRV_BASE_CTL, RTDRV_GET_MAX+1, do_rtdrv_get_ctl, NULL
};

module_init(rtdrv_init);
module_exit(rtdrv_exit);

MODULE_DESCRIPTION ("Switch SDK User/Kernel Driver Module");

