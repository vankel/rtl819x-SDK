/*
 * Copyright (C) 2009 Realtek Semiconductor Corp.
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
 * Purpose : Realtek Switch SDK Rtdrv Netfilter Module in the SDK.
 *
 * Feature : Realtek Switch SDK Rtdrv Netfilter Module
 *
 */

#ifndef __RTDRV_NETFILTER_H__
#define __RTDRV_NETFILTER_H__

/*
 * Include Files
*/
#include <hal/mac/reg.h>
#include <rtk/gpon.h>
#include <rtk/rtusr/rtusr_pkt.h>

/*
 * Symbol Definition
 */
#define RTDRV_BASE_CTL	                	    (64+1024+64+64+9000)

#define RTDRV_REG_OFFSET                        1 /* 0 will be used by RTDRV_INIT_RTKAPI */
#define RTDRV_GPON_OFFSET                       (RTDRV_REG_OFFSET + 10)
#define RTDRV_PKT_OFFSET                        (RTDRV_GPON_OFFSET + 100)

#define RTDRV_END_OFFSET                        (RTDRV_PKT_OFFSET + 10)



#define SDK_CFG_ITEM                            32



/***** RTDRV_SET *****/
#define RTDRV_INIT_RTKAPI                       (RTDRV_BASE_CTL)


#define RTDRV_SET_MAX                           (RTDRV_BASE_CTL + RTDRV_END_OFFSET)


enum rtdrv_reg_get_e
{
    RTDRV_REG_REGISTER_GET = (RTDRV_BASE_CTL + RTDRV_REG_OFFSET),
    RTDRV_REG_IDX2ADDR_GET,
    RTDRV_REG_IDXMAX_GET,
    RTDRV_REG_INFO_GET,
    RTDRV_TABLE_READ,
    RTDRV_REG_ADDRESS_GET,
    RTDRV_PHY_REG_GET,
    RTDRV_SOC_ADDRESS_GET
};

enum rtdrv_gpon_get_e
{
    RTDRV_GPON_SN_GET = (RTDRV_BASE_CTL + RTDRV_GPON_OFFSET),
    RTDRV_GPON_PWD_GET,
    RTDRV_GPON_STATE_GET,
    RTDRV_GPON_ALARM_GET,
    RTDRV_GPON_PARA_GET,
    RTDRV_GPON_TCONT_GET,
    RTDRV_GPON_DS_FLOW_GET,
    RTDRV_GPON_US_FLOW_GET,
    RTDRV_GPON_BC_PASS_GET,
    RTDRV_GPON_NON_MC_PASS_GET,
    RTDRV_GPON_IP_PTN_GET,
    RTDRV_GPON_FLT_MODE_GET,
    RTDRV_GPON_FS_MODE_GET,
    RTDRV_GPON_MAC_ENTRY_GET,
    RTDRV_GPON_RDI_GET,
    RTDRV_GPON_PWR_LEVEL_GET,
    RTDRV_GPON_TX_LASER_GET,
    RTDRV_GPON_FS_IDLE_GET,
#if 0
    RTDRV_GPON_FS_PRBS_GET,
#endif
    RTDRV_GPON_DS_FEC_GET,
    RTDRV_GPON_VERSION_SHOW,
    RTDRV_GPON_DEV_SHOW,
    RTDRV_GPON_GTC_SHOW,
    RTDRV_GPON_TCONT_SHOW,
    RTDRV_GPON_DS_FLOW_SHOW,
    RTDRV_GPON_US_FLOW_SHOW,
    RTDRV_GPON_MAC_TABLE_SHOW,
    RTDRV_GPON_GLB_CNT_SHOW,
    RTDRV_GPON_TCONT_CNT_SHOW,
    RTDRV_GPON_FLOW_CNT_SHOW,
    RTDRV_GPON_TEST_GET,
    RTDRV_GPON_AUTO_TCONT_GET,
    RTDRV_GPON_AUTO_BOH_GET,
    RTDRV_GPON_EQD_OFFSET_GET

};

enum rtdrv_pkt_get_e
{
    RTDRV_PKT_RXDUMP_GET = (RTDRV_BASE_CTL + RTDRV_PKT_OFFSET)
};

/***** RTDRV_GET *****/
#define RTDRV_GET_MAX                           (RTDRV_BASE_CTL + RTDRV_END_OFFSET)


enum rtdrv_reg_set_e
{
    RTDRV_REG_REGISTER_SET = (RTDRV_BASE_CTL + RTDRV_REG_OFFSET),
    RTDRV_TABLE_WRITE,
    RTDRV_REG_ADDRESS_SET,
    RTDRV_PHY_REG_SET,
    RTDRV_SOC_ADDRESS_SET
};

enum rtdrv_gpon_set_e
{
    RTDRV_GPON_SN_SET = (RTDRV_BASE_CTL + RTDRV_GPON_OFFSET),
    RTDRV_GPON_PWD_SET,
    RTDRV_GPON_ACTIVE,
    RTDRV_GPON_DEACTIVE,
    RTDRV_GPON_PARA_SET,
    RTDRV_GPON_TCONT_ADD,
    RTDRV_GPON_TCONT_DEL,
    RTDRV_GPON_DS_FLOW_ADD,
    RTDRV_GPON_US_FLOW_ADD,
    RTDRV_GPON_BC_PASS_SET,
    RTDRV_GPON_NON_MC_PASS_SET,
    RTDRV_GPON_IP_PTN_SET,
    RTDRV_GPON_FLT_MODE_SET,
    RTDRV_GPON_FS_MODE_SET,
    RTDRV_GPON_MAC_ENTRY_ADD,
    RTDRV_GPON_MAC_ENTRY_DEL,
    RTDRV_GPON_RDI_SET,
    RTDRV_GPON_PWR_LEVEL_SET,
    RTDRV_GPON_TX_LASER_SET,
    RTDRV_GPON_FS_IDLE_SET,
#if 0
    RTDRV_GPON_FS_PRBS_SET,
#endif
    RTDRV_GPON_TEST_SET,
    RTDRV_GPON_INITIAL,
    RTDRV_GPON_DEINITIAL,
    RTDRV_GPON_DEBUG_SET,
    RTDRV_GPON_UNIT_TEST,
#if defined(OLD_FPGA_DEFINED)
    RTDRV_GPON_PKTGEN_CFG,
    RTDRV_GPON_PKTGEN_BUF,
#endif
    RTDRV_GPON_OMCI_TX,
    RTDRV_GPON_AUTO_TCONT_SET,
    RTDRV_GPON_AUTO_BOH_SET,
    RTDRV_GPON_EQD_OFFSET_SET
};

enum rtdrv_pkt_set_e
{
    RTDRV_PKT_SEND = (RTDRV_BASE_CTL + RTDRV_PKT_OFFSET),
    RTDRV_PKT_RXDUMP_ENABLE,
    RTDRV_PKT_RXDUMP_CLEAR
};

typedef struct rtdrv_regCfg_s
{
    uint32  reg;
    uint32  bit;
    uint32  value;
} rtdrv_regCfg_t;




typedef struct rtdrv_addrCfg_s
{
    uint32  address;
    uint32  value;
} rtdrv_addrCfg_t;

typedef struct rtdrv_ponAlarm_s
{
    rtk_gpon_alarm_type_t   type;
    int32                   status;
} rtdrv_ponAlarm_t;

typedef union rtdrv_pon_union_u
{
    rtk_gpon_onu_activation_para_t  timer;
    rtk_gpon_laser_para_t           laser;
    rtk_gpon_ds_physical_para_t     ds_phy;
    rtk_gpon_ds_ploam_para_t        ds_plm;
    rtk_gpon_ds_bwMap_para_t        ds_bw;
    rtk_gpon_ds_gem_para_t          ds_gem;
    rtk_gpon_ds_eth_para_t          ds_eth;
    rtk_gpon_ds_omci_para_t         ds_omci;
    rtk_gpon_us_physical_para_t     us_phy;
    rtk_gpon_us_ploam_para_t        us_plm;
    rtk_gpon_us_dbr_para_t          us_dbr;

} rtdrv_pon_union_t;

typedef struct rtdrv_ponPara_s
{
    rtk_gpon_patameter_type_t   type;
    rtdrv_pon_union_t           para;
} rtdrv_ponPara_t;

typedef struct rtdrv_tcont_s
{
    rtk_gpon_tcont_ind_t        ind;
    rtk_gpon_tcont_attr_t       attr;
} rtdrv_tcont_t;

typedef struct rtdrv_dsFlow_s
{
    uint32                  flowId;
    rtk_gpon_dsFlow_attr_t  attr;
} rtdrv_dsFlow_t;

typedef struct rtdrv_usFlow_s
{
    uint32                  flowId;
    rtk_gpon_usFlow_attr_t  attr;
} rtdrv_usFlow_t;

typedef struct rtdrv_tcont_cnt_s
{
    uint32                              tcont;
    rtk_gpon_tcont_performance_type_t   cnt_type;
} rtdrv_tcont_cnt_t;

typedef struct rtdrv_flow_cnt_s
{
    uint32                              flow;
    rtk_gpon_flow_performance_type_t    cnt_type;
} rtdrv_flow_cnt_t;

typedef struct rtdrv_ip_ptn_s
{
    uint32 ipv4_pattern;
    uint32 ipv6_pattern;

} rtdrv_ip_ptn_t;

typedef struct rtdrv_fs_mode_s
{
    rtk_gpon_mc_force_mode_t ipv4;
    rtk_gpon_mc_force_mode_t ipv6;

} rtdrv_fs_mode_t;


typedef struct rtdrv_mac_entry_s
{
    uint32                      index;
    rtk_gpon_macTable_entry_t   entry;
} rtdrv_mac_entry_t;


typedef struct rtdrv_phyCfg_s
{
    uint8   phy_id;
    uint8   reg;
    uint16  value;
} rtdrv_phyCfg_t;

typedef struct rtdrv_pktGen_s
{
    uint32  item;
    uint32  tcont;
    uint32  gem;
    int32   omci;
    uint32  buf_len;
    uint8   buf[2048];
} rtdrv_pktGen_t;

typedef struct rtdrv_pktdbg_s
{
    struct  pkt_dbg_tx_info tx_info;
    struct  pkt_dbg_rx_info rx_info;
    uint32  enable;
    uint16  length;
    uint16  rx_length;
    uint8   buf[2048];
} rtdrv_pktdbg_t;

typedef union rtdrv_union_u
{
    rtdrv_regCfg_t          reg_cfg;
    rtdrv_addrCfg_t         addr_cfg;
    rtdrv_phyCfg_t          phy_cfg;
    rtk_gpon_serialNumber_t sn_cfg;
    rtk_gpon_password_t     pwd_cfg;
    rtk_gpon_initialState_t init_state_cfg;
    rtdrv_ponAlarm_t                    alarm_state;
    rtdrv_ponPara_t                     pon_cfg;
    rtk_gpon_fsm_status_t               pon_state;
    rtdrv_tcont_t                       tcont_cfg;
    rtdrv_dsFlow_t                      ds_flow_cfg;
    rtdrv_usFlow_t                      us_flow_cfg;
    rtdrv_ip_ptn_t                      ip_ptn;
    rtk_gpon_macTable_exclude_mode_t    filter_mode;
    rtdrv_fs_mode_t                     fs_mode;
    rtdrv_mac_entry_t                   mac_entry;
    int32                               state;
    rtk_gpon_laser_status_t             tx_laser;
    uint32                              index;
    rtk_gpon_global_performance_type_t  glb_cnt;
    rtdrv_tcont_cnt_t                   tcont_cnt;
    rtdrv_flow_cnt_t                    flow_cnt;
    rtdrv_pktGen_t                      pkt_gen;
    rtk_gpon_omci_msg_t                 omci;
    rtdrv_pktdbg_t                      pkt_info;

} rtdrv_union_t;

#endif /* __RTDRV_NETFILTER_H__ */
