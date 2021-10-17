/*
* Copyright (C) 2009 Realtek Semiconductor Corp.
* All Rights Reserved.
*
* This program is the proprietary software of Realtek Semiconductor
* Corporation and/or its licensors, and only be used, duplicated,
* modified or distributed under the authorized license from Realtek.
*
* ANY USE OF THE SOFTWARE OTEHR THAN AS AUTHORIZED UNDER
* THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED.
*
* $Revision: 8334 $
* $Date: 2010-02-09 13:22:42 +0800 (Tue, 09 Feb 2010) $
*
* Purpose :This header defines VERY general macro, structure, enumeration and variable types that
*              could be used both by applications, APIs and ASIC driver.
*              Be ware that this file could be released to customer whenever you add new contents.
*
*  Feature :
*
*/

#include <asicdrv/rtl8316d_types.h>
#include <rtk_api.h>
#include <rtl8316d_general_reg.h>
#include <rtl8316d_table_struct.h>
#include <rtl8316d_asicDrv.h>
#include <rtl8316d_asicdrv_nic.h>
#include <rtl8316d_reg_struct.h>
#include <rtl8316d_asicdrv_trunk.h>
#include <rtl8316d_debug.h>

void rtk_switch_init(void)
{
    uint16 t;

    reg_write(RTL8316D_UNIT, PHY_AUTO_ACCESS_MASK1, 0x1ffffff);

    for(t=0;t<16;t++)
        phy_reg_write(t,18,1,0x9270);
    for(t=16;t<24;t++)
        phy_reg_write(t,18,1,0x9230);

    reg_write(RTL8316D_UNIT, GLOBAL_MAC_INTERFACE_INTERNAL3, 0x10);
    reg_write(RTL8316D_UNIT, INTERFACE_DMY_REGSITER, 0x100);

    reg_write(RTL8316D_UNIT, PORT0_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT1_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT2_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT3_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT4_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT5_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT6_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT7_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT8_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT9_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT10_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT11_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT12_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT13_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT14_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT15_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT16_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT17_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT18_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT19_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT20_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT21_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT22_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT23_L2_MISC0, 0x4f7);
    reg_write(RTL8316D_UNIT, PORT24_L2_MISC0, 0x4f7);
}

void rtl8316d_sys_reboot(void)
{
    reg_write(RTL8316D_UNIT, RESET_GLOBAL_CONTROL1, 0x0);
}

int RTL8325D_init(void)
{
	uint32 value;

	/* Initial Chip */
	rtk_switch_init();

	// set RTL8325D RGMII port to Force linkup, set reg PORT24_PROPERTY_CONFIGURE bit[2:1] to 0b11, bit8=0.
	// disable EEE
	if (rtlglue_reg32_read_data(0xbb040088, &value) == RT_ERR_OK)
		rtlglue_reg32_write(0xbb040088, ((value & ~0x60170) | 0x6));

	// disable the polling mask for port16~port23. set reg PHY_AUTO_ACCESS_MASK1 to 0xFFFF
	rtlglue_reg32_write(0xbb040024, 0xFFFF);

	return RT_ERR_OK; 
}

