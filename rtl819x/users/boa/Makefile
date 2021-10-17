# $Id: Makefile.in,v 1.1.2.6 2002/12/12 21:29:04 jnelson Exp $

COMMON_CFLAGS =

ifeq ($(wildcard $(DIR_LINUX)/.config),)
COMMON_CFLAGS += -DKERNEL_2_6_30
include ../../linux-2.6.30/.config
DIR_LINUX_KERNEL = ../../linux-2.6.30
else
KERNEL_VER=$(patsubst $(DIR_ROOT)/%,%,$(DIR_LINUX))
ifeq ($(KERNEL_VER),linux-2.6.30)
COMMON_CFLAGS += -DKERNEL_2_6_30
endif
ifeq ($(KERNEL_VER),linux-3.10)
COMMON_CFLAGS += -DKERNEL_3_10
endif
include $(DIR_LINUX)/.config
DIR_LINUX_KERNEL = $(DIR_LINUX)
endif

ifeq ($(wildcard $(DIR_USERS)/.config),)
include ../.config
else
include $(DIR_USERS)/.config
endif


ifndef DIR_RSDK
include ../../.config
ifeq ($(x86),1)
#
# x86 simulation for boa & flash
#
# usage: 
# 1. cd boa
# 2. make x86=1 (it will create x86 files, and copy to x86 dir)
# 3. cd x86 
# 
# * if APMIB_SHARED = 0 
# 1. ./flash default (it will create setting.bin to simulate flash)
# 2. ./boa -c . -r . (it will start boa, you may need run 'service httpd stop' first)
#
# * if APMIB_SHARED = 1
# 1. LD_LIBRARY_PATH=. ./flash default 
# 2. LD_LIBRARY_PATH=. ./boa -c . -r .
#
# Use browser to view html now!
#
export CC = gcc
export CPP = gcc -E
export AR = ar
export STRIP = strip
# uncomment it to disable strip behavior for gdb debug
#export STRIP = :
export DIR_USERS := $(shell pwd)/../
export DIR_LINUX := $(shell pwd)/../../linux-2.6.30
else
export PATH := $(shell pwd)/../../$(CONFIG_RSDKDIR)/bin:$(PATH)
CROSS_COMPILE ?= rsdk-linux-
export CC = $(CROSS_COMPILE)gcc
export CPP = $(CROSS_COMPILE)gcc -E
export AR = $(CROSS_COMPILE)ar
export STRIP = $(CROSS_COMPILE)strip
endif
export x86 = 1
endif

ifdef CONFIG_RTL_8198_AP_ROOT
CONFIG_RTL_AP = CONFIG_RTL_8198_AP_ROOT
endif

ifdef CONFIG_RTL_8197D_AP
CONFIG_RTL_AP = CONFIG_RTL_8197D_AP
endif

ifdef CONFIG_RTL_AP
AP=1
SKUTAG=ap
else
GW=1
SKUTAG=gw
endif

#------------------------------------------------------------------------------

MAKE=make

.PHONY: clean mrclean distclean configcheck boa docs apmib utils system defconfig html tools

#------------------------------------------------------------------------------
# APMIB_SHARED, APMIB_LIB and COMMON_CFLAGS
#------------------------------------------------------------------------------
APMIB_SHARED = 1
DYNAMIC_WAN_IP = 1
GET_SERVER_IP_BY_DOMAIN = 1
#COMMON_CFLAGS =
ifeq ($(x86),1)
COMMON_CFLAGS += -DNO_ACTION
endif

ifeq ($(APMIB_SHARED),1)
COMMON_CFLAGS += -DCONFIG_APMIB_SHARED
APMIB_LIB = libapmib.so
else
APMIB_LIB = libapmib.a
endif

# User Management switch
UMSW	= -DUSER_MANAGEMENT_SUPPORT

# TLS client mode support
#TLS= -DTLS_CLIENT


# WLAN support
#-DWLAN_EASY_CONFIG
WLANSUPP = -DUNIVERSAL_REPEATER -DWLAN_WPA -DWLAN_WDS -DWLAN_8185AG -DWLAN_WPA2 $(TLS)
WLANSUPP += -DWIFI_SIMPLE_CONFIG

ifeq ($(CONFIG_RTL_11W_SUPPORT), y)
WLANSUPP += -DCONFIG_IEEE80211W
endif

ifeq ($(CONFIG_APP_SIMPLE_CONFIG), y)
WLANSUPP += -DCONFIG_APP_SIMPLE_CONFIG
COMMON_CFLAGS += -DSET_MIB_FROM_FILE
ifeq ($(CONFIG_SIMPLE_CONFIG_SYNC_VXD_PROFILE), y)
COMMON_CFLAGS += -DSYNC_VXD_PROFILE
endif
endif

ifeq ($(CONFIG_RTL_HS2_SUPPORT), y)
WLANSUPP += -DWLAN_HS2_CONFIG
endif

# Digest Access switch
DASW	= -DDIGEST_ACCESS_SUPPORT

# Gateway flag
GATEWAY = -DHOME_GATEWAY -DDOS_SUPPORT
ifeq ($(GW),1)
GATEWAY += -DROOT_FS
endif

# samba web support
ifeq ($(CONFIG_APP_SAMBA_WEB), y)
COMMON_CFLAGS += -DSAMBA_WEB_SUPPORT
endif
ifeq ($(CONFIG_URL_FILTER_USER_MODE_SUPPORT),y)
COMMON_CFLAGS += -DURL_FILTER_USER_MODE_SUPPORT
endif

# routing support
ifeq ($(CONFIG_APP_ROUTED), y)
COMMON_CFLAGS += -DROUTE_SUPPORT
endif
ifeq ($(CONFIG_APP_BIRD), y)
COMMON_CFLAGS += -DRIP6_SUPPORT
endif

ifeq ($(CONFIG_APP_UPDATEDD), y)
COMMON_CFLAGS += -DDDNS_SUPPORT
endif

ifeq ($(CONFIG_APP_PPTP), y)
COMMON_CFLAGS += -DPPTP_SUPPORT
endif

ifeq ($(CONFIG_APP_L2TPD), y)
COMMON_CFLAGS += -DL2TP_SUPPORT
endif

ifeq ($(CONFIG_RTL_DEF_SETTING_IN_FW),y)
COMMON_CFLAGS += -DRTL_DEF_SETTING_IN_FW
endif

# boa cgi support
#export BOA_CGI_SUPPORT=y
ifeq ($(BOA_CGI_SUPPORT), y)
COMMON_CFLAGS += -DBOA_CGI_SUPPORT
endif


ifeq ($(DYNAMIC_WAN_IP), 1)
COMMON_CFLAGS += -DCONFIG_DYNAMIC_WAN_IP
ifeq ($(GET_SERVER_IP_BY_DOMAIN), 1)
COMMON_CFLAGS += -DCONFIG_GET_SERVER_IP_BY_DOMAIN
endif
endif

ifeq ($(CONFIG_MTD_NAND),y)
COMMON_CFLAGS += -DCONFIG_MTD_NAND
endif

ifeq ($(CONFIG_RPS),y)
COMMON_CFLAGS += -DCONFIG_RPS
endif

ifeq ($(CONFIG_RTK_VLAN_SUPPORT),y)
COMMON_CFLAGS += -DVLAN_CONFIG_SUPPORTED
endif

ifeq ($(CONFIG_RTL_SUPPORT_ACCESS_PORT_FORWARD_FROM_LAN),y)
COMMON_CFLAGS += -DCONFIG_RTL_SUPPORT_ACCESS_PORT_FORWARD_FROM_LAN
endif

ifeq ($(CONFIG_RTL_ULINKER),y)
ifeq ($(CONFIG_WLAN_HAL_8881A),y)
COMMON_CFLAGS += -DCONFIG_RTL_ULINKER
else
COMMON_CFLAGS += -DCONFIG_RTL_ULINKER_NOT_8881A
endif
endif

#for 11s mesh
ifeq ($(CONFIG_RTL_MESH_SUPPORT),y)
        SUBDIRS += 11s-PathSelection
        #MESH_EXTRA_CONFIG_FLAGS += -DCONFIG_RTK_MESH -D_MESH_ACL_ENABLE_ -DSNMP_SUPPORT -DCONFIG_NEW_MESH_UI
        MESH_EXTRA_CONFIG_FLAGS += -DCONFIG_RTK_MESH -D_MESH_ACL_ENABLE_ -DCONFIG_NEW_MESH_UI
        ifeq ($(CONFIG_TEST_PKG),y)
                #SUBDIRS += test_traffic netperf-2.4.4 
                #SUBDIRS += test_traffic
               MESH_EXTRA_CONFIG_FLAGS += -D_11s_TEST_MODE_
                TESTPKG=1
        endif
        ifeq ($(CONFIG_NMS),y)
                #SUBDIRS += net-snmp-5.2.3 nmsd burn_image
		MESH_EXTRA_CONFIG_FLAGS += -DSNMP_SUPPORT
                NMS=1
        endif
        TESTPKG?=0
        NMS?=0
endif
export MESH_EXTRA_CONFIG_FLAGS

WLANSUPP += $(MESH_EXTRA_CONFIG_FLAGS)

ifeq ($(CONFIG_RTL8192CD),y)
        WLANSUPP += -DCONFIG_RTL8192CD
endif

ifeq ($(CONFIG_RTL8196B_GW_8M),y)
	WLANSUPP += -DCONFIG_RTL8196B_GW_8M
endif

COMMON_CFLAGS += $(WLANSUPP) $(UMSW) $(DASW) -DDOWN_SIZE

ifdef CONFIG_DEFAULTS_KERNEL_2_6
COMMON_CFLAGS += -DCONFIG_DEFAULTS_KERNEL_2_6
endif

ifeq ($(CONFIG_APP_USBMOUNT),y)
COMMON_CFLAGS += -DCONFIG_APP_USBMOUNT
endif

ifeq ($(CONFIG_RTL_HIGH_PERFORMANCE_FILESYSTEM),y)
COMMON_CFLAGS += -DCONFIG_RTL_HIGH_PERFORMANCE_FILESYSTEM
endif

ifeq ($(CONFIG_USBDISK_UPDATE_IMAGE),y)
COMMON_CFLAGS += -DCONFIG_USBDISK_UPDATE_IMAGE
endif

ifeq ($(CONFIG_APP_SAMBA),y)
COMMON_CFLAGS += -DCONFIG_APP_SAMBA 
endif

ifeq ($(CONFIG_APP_VSFTPD),y)
COMMON_CFLAGS += -DCONFIG_APP_VSFTPD
endif

ifeq ($(CONFIG_APP_DLNA_DMS),y)
COMMON_CFLAGS += -DCONFIG_APP_DLNA_DMS
endif

ifeq ($(CONFIG_APP_CTORRENT), y)
COMMON_CFLAGS += -DCONFIG_RTL_BT_CLIENT
endif

ifeq ($(CONFIG_APP_SNMP),y)
COMMON_CFLAGS += -DCONFIG_SNMP
endif

ifeq ($(CONFIG_RTL_IVL_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_IVL_SUPPORT
endif

ifeq ($(CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE),y)
COMMON_CFLAGS += -DCONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE 
endif

ifeq ($(CONFIG_RTL_FLASH_DUAL_IMAGE_WEB_BACKUP_ENABLE),y)
COMMON_CFLAGS += -DCONFIG_RTL_FLASH_DUAL_IMAGE_WEB_BACKUP_ENABLE
endif

ifeq ($(CONFIG_RTL_MULTI_REPEATER_MODE_SUPPORT),y)
COMMON_CFLAGS += -DRTL_MULTI_REPEATER_MODE_SUPPORT
endif

ifeq ($(CONFIG_RTL_MULTI_CLONE_SUPPORT),y)
COMMON_CFLAGS += -DRTL_MULTI_CLONE_SUPPORT
endif

ifeq ($(CONFIG_USB_SERIAL_OPTION),y)
COMMON_CFLAGS += -DRTK_USB3G
#ifeq ($(CONFIG_POCKET_ROUTER_SUPPORT),y)
#COMMON_CFLAGS += -DRTK_USB3G_PORT5_LAN
#endif
endif

#CONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D
#ifeq ($(CONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D),y)
ifeq ($(CONFIG_USE_PCIE_SLOT_0),y)
ifeq ($(CONFIG_USE_PCIE_SLOT_1),y)
ifeq ($(CONFIG_RTL_92D_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_92D_DMDP  -DCONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D -DFOR_DUAL_BAND
endif
endif
endif
ifeq ($(CONFIG_RTL_P2P_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_P2P_SUPPORT
endif

ifeq ($(CONFIG_RTL_92D_DMDP),y)
COMMON_CFLAGS += -DFOR_DUAL_BAND
endif

ifeq ($(CONFIG_WLAN_HAL_8881A),y)
ifeq ($(CONFIG_RTL_8881A_SELECTIVE),y)
COMMON_CFLAGS += -DCONFIG_RTL_8881A_SELECTIVE
else
COMMON_CFLAGS += -DCONFIG_RTL_92D_SUPPORT
endif
COMMON_CFLAGS += -DCONFIG_RTL_8812_SUPPORT
COMMON_CFLAGS += -DCONFIG_RTL_11AC_SUPPORT

ifeq ($(CONFIG_USE_PCIE_SLOT_0),y)
COMMON_CFLAGS += -DFOR_DUAL_BAND
COMMON_CFLAGS += -DCONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D
endif
endif

#ifeq ($(CONFIG_RTL_DUAL_92C_8812E),y)
ifeq ($(CONFIG_USE_PCIE_SLOT_0),y)
ifeq ($(CONFIG_USE_PCIE_SLOT_1),y)
ifeq ($(CONFIG_RTL_8812_SUPPORT),y)
COMMON_CFLAGS += -DFOR_DUAL_BAND
COMMON_CFLAGS += -DCONFIG_RTL_92D_SUPPORT
COMMON_CFLAGS += -DCONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D
endif
endif
endif
ifeq ($(CONFIG_USE_PCIE_SLOT_0),y)                                                            
ifeq ($(CONFIG_USE_PCIE_SLOT_1),y)                                                            
ifeq ($(CONFIG_RTL_8367R_SUPPORT),y)
COMMON_CFLAGS += -DRTL_8367R_DUAL_BAND
endif
endif
endif

ifeq ($(CONFIG_USE_PCIE_SLOT_0),y)
ifeq ($(CONFIG_RTL_8881A),y)
ifeq ($(CONFIG_RTL_8367R_SUPPORT),y)
COMMON_CFLAGS += -DRTL_8367R_8881a_DUAL_BAND
endif
endif
endif

ifeq ($(CONFIG_SLOT_0_8192EE),y)
COMMON_CFLAGS += -DCONFIG_RTL8192E
endif
ifeq ($(CONFIG_SLOT_1_8192EE),y)
COMMON_CFLAGS += -DCONFIG_RTL8192E
endif
#
# Extra Feature Configuration
#
ifeq ($(CONFIG_HTTP_FILE_SERVER_SUPPORT),y)
COMMON_CFLAGS += -DHTTP_FILE_SERVER_SUPPORTED
COMMON_CFLAGS += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DENABLE_LFS
COMMON_CFLAGS += -DHTTP_FILE_SERVER_HTM_UI
endif
ifeq ($(CONFIG_RTL_USB_IP_HOST_SPEEDUP),y)
COMMON_CFLAGS += -DRTL_USB_IP_HOST_SPEEDUP
COMMON_CFLAGS += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DENABLE_LFS
endif

ifeq ($(CONFIG_RTL_STP),y)
COMMON_CFLAGS += -DCONFIG_RTL_STP
endif

ifeq ($(CONFIG_BASEON_SCHEDULE_FILTER),y)
COMMON_CFLAGS += -DCONFIG_SCH_FILTER
endif

ifneq ($(CONFIG_APP_APPLE_MFI_WAC),y)
ifeq ($(CONFIG_IPV6),y)
COMMON_CFLAGS += -DCONFIG_IPV6

ifeq ($(CONFIG_APP_NDP_PROXY),y)
COMMON_CFLAGS += -DCONFIG_APP_NDP_PROXY
endif
endif
endif

ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_IPV6_CE_ROUTER_SUPPORT
endif

ifeq ($(CONFIG_RTL_819X),y)
	COMMON_CFLAGS += -DCONFIG_RTL_819X
endif

ifeq ($(CONFIG_RTL_819XD),y)
	COMMON_CFLAGS += -DCONFIG_RTL_819XD
endif
ifeq ($(CONFIG_RTL_8196B_GW),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8196B
endif

ifeq ($(CONFIG_RTL_8196C_GW),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8196C
endif

ifeq ($(CONFIG_RTL_8198C),y)
     COMMON_CFLAGS += -DCONFIG_RTL_8198C
endif

ifeq ($(CONFIG_DSLITE_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_DSLITE_SUPPORT
endif

ifeq ($(CONFIG_SIXRD_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_SIXRD_SUPPORT
endif

ifeq ($(CONFIG_RTL_8198_GW),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8198
endif

ifeq ($(CONFIG_RTL_8198_AP_ROOT),y)
        COMMON_CFLAGS += -DCONFIG_RTL_8198 -DCONFIG_RTL_8198_AP_ROOT
ifdef CONFIG_8198_PORT5_GMII
	COMMON_CFLAGS += -DGMII_ENABLED
endif
endif

ifeq ($(CONFIG_RTL_8197D_AP),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8197D_AP
endif

ifeq ($(CONFIG_RTL_AP_PACKAGE),y)
	COMMON_CFLAGS += -DCONFIG_RTL_AP_PACKAGE
endif

ifeq ($(CONFIG_RTL_819XD),y)
	COMMON_CFLAGS += -DCONFIG_RTL_819XD
endif

ifeq ($(CONFIG_RTL_8196E),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8196E
endif

ifeq ($(CONFIG_RTL_8871AM),y)
        COMMON_CFLAGS += -DCONFIG_RTL_8871AM
endif

ifeq ($(CONFIG_RTL_8198B),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8198B
endif

ifeq ($(CONFIG_POCKET_ROUTER_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_POCKET_ROUTER_SUPPORT
endif

ifeq ($(CONFIG_POCKET_AP_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_POCKET_AP_SUPPORT
endif

ifeq ($(CONFIG_DOMAIN_NAME_QUERY_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_DOMAIN_NAME_QUERY_SUPPORT
endif

ifeq ($(CONFIG_RTL_DFS_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_RTL_DFS_SUPPORT
endif

ifeq ($(CONFIG_RTL_CLIENT_MODE_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_WLAN_CLIENT_MODE
endif
 
ifeq ($(CONFIG_RTL_REPEATER_MODE_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_WLAN_REPEATER_MODE
endif

ifeq ($(CONFIG_RTL_WDS_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_WLAN_WDS_SUPPORT
endif

ifeq ($(CONFIG_RTL_VAP_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_WLAN_VAP_SUPPORT
endif

ifeq ($(CONFIG_PCIE_POWER_SAVING),y)
COMMON_CFLAGS += -DPOWER_CONSUMPTION_SUPPORT
endif

ifeq ($(CONFIG_RTL_8196_SPI_FLASH),y)
	COMMON_CFLAGS += -DCONFIG_RTL8196_SPI
endif

ifeq ($(CONFIG_RTL_WAPI_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_WAPI_SUPPORT
ifeq ($(CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
endif
endif

ifeq ($(CONFIG_APP_RTK_INBAND_CTL), y)
COMMON_CFLAGS += -DSET_MIB_FROM_FILE
COMMON_CFLAGS += -DCONFIG_APP_RTK_INBAND_CTL
endif

ifeq ($(CONFIG_RTL_802_1X_CLIENT_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_RTL_802_1X_CLIENT_SUPPORT
endif
ifeq ($(CONFIG_RTL_ETH_802DOT1X_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_RTL_ETH_802DOT1X_SUPPORT
endif
ifeq ($(CONFIG_RTL_ETH_802DOT1X_CLIENT_MODE_SUPPORT),y)
    COMMON_CFLAGS += -DCONFIG_RTL_ETH_802DOT1X_CLIENT_MODE_SUPPORT
endif
#add mld snooping support
ifeq ($(CONFIG_IGMPV3_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_IGMPV3_SUPPORT
endif

#add mld snooping support
ifeq ($(CONFIG_RTL_MLD_SNOOPING),y)
	COMMON_CFLAGS += -DCONFIG_RTL_MLD_SNOOPING
endif

ifeq ($(CONFIG_RTL_COMAPI_CFGFILE),y)
	COMMON_CFLAGS += -DCONFIG_IWPRIV_INTF
	COMMON_CFLAGS += -DCONFIG_RTL_COMAPI_CFGFILE
endif

COMMON_CFLAGS += -DCONFIG_FLASH_SIZE=$(CONFIG_RTL_FLASH_SIZE)

ifeq ($(CONFIG_RTL_FLASH_MAPPING_ENABLE),y)
	COMMON_CFLAGS += -DCONFIG_RTL_FLASH_MAPPING_ENABLE
ifdef CONFIG_RTL_HW_SETTING_OFFSET
	COMMON_CFLAGS += -DCONFIG_RTL_HW_SETTING_OFFSET=$(CONFIG_RTL_HW_SETTING_OFFSET)
endif

ifdef CONFIG_RTL_DEFAULT_SETTING_OFFSET
	COMMON_CFLAGS += -DCONFIG_RTL_DEFAULT_SETTING_OFFSET=$(CONFIG_RTL_DEFAULT_SETTING_OFFSET)
endif

ifdef CONFIG_RTL_CURRENT_SETTING_OFFSET
	COMMON_CFLAGS += -DCONFIG_RTL_CURRENT_SETTING_OFFSET=$(CONFIG_RTL_CURRENT_SETTING_OFFSET)
endif

ifdef CONFIG_RTL_WEB_PAGES_OFFSET
	COMMON_CFLAGS += -DCONFIG_RTL_WEB_PAGES_OFFSET=$(CONFIG_RTL_WEB_PAGES_OFFSET)
endif

ifdef CONFIG_RTL_LINUX_IMAGE_OFFSET  
	COMMON_CFLAGS += -DCONFIG_RTL_CODE_IMAGE_OFFSET=$(CONFIG_RTL_LINUX_IMAGE_OFFSET)
endif

ifdef CONFIG_RTL_ROOT_IMAGE_OFFSET 
	COMMON_CFLAGS += -DCONFIG_RTL_ROOT_IMAGE_OFFSET=$(CONFIG_RTL_ROOT_IMAGE_OFFSET)
endif
endif

ifeq ($(CONFIG_RTL_MULTI_LAN_DEV),y)
	COMMON_CFLAGS += -DCONFIG_RTL_MULTI_LAN_DEV
endif

ifeq ($(CONFIG_RTL_HW_NAPT), y)
	COMMON_CFLAGS += -DCONFIG_RTL_HW_NAPT
endif

ifeq ($(CONFIG_RTL_LAYERED_DRIVER_ACL),y)
	COMMON_CFLAGS += -DCONFIG_RTL_LAYERED_DRIVER_ACL
endif

ifeq ($(CONFIG_RTK_VLAN_SUPPORT), y)
	COMMON_CFLAGS += -DCONFIG_RTK_VLAN_SUPPORT
endif

ifeq ($(CONFIG_RTK_VLAN_WAN_TAG_SUPPORT), y)
	COMMON_CFLAGS += -DCONFIG_RTK_VLAN_WAN_TAG_SUPPORT
endif

ifeq ($(CONFIG_RTK_VLAN_NEW_FEATURE),y)
        COMMON_CFLAGS += -DCONFIG_RTK_VLAN_NEW_FEATURE
endif
ifeq ($(CONFIG_RTL_HW_VLAN_SUPPORT),y)
        COMMON_CFLAGS += -DCONFIG_RTL_HW_VLAN_SUPPORT
endif
# For mini_upnp stand alone
COMMON_CFLAGS += -DSTAND_ALONE_MINIUPNP

ifeq ($(CONFIG_RTL_92D_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_92D_SUPPORT 
endif

ifeq ($(CONFIG_RTL_8812_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_8812_SUPPORT 
ifeq ($(CONFIG_RTL_DUAL_BAND_SELECTIVE),y)
COMMON_CFLAGS += -DCONFIG_RTL_8881A_SELECTIVE
else
COMMON_CFLAGS += -DCONFIG_RTL_92D_SUPPORT 
endif
endif

ifeq ($(CONFIG_RTL_AC2G_256QAM),y)
COMMON_CFLAGS += -DCONFIG_RTL_AC2G_256QAM
endif

ifeq ($(CONFIG_RTL_8812AR_VN_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_8812AR_VN_SUPPORT
ifeq ($(CONFIG_RTL_DUAL_BAND_SELECTIVE),y)
COMMON_CFLAGS += -DCONFIG_RTL_8881A_SELECTIVE
else
COMMON_CFLAGS += -DCONFIG_RTL_92D_SUPPORT 
endif
endif

ifeq ($(CONFIG_RTL_DUAL_BAND_SELECTIVE),y)
COMMON_CFLAGS += -DCONFIG_RTL_8881A_SELECTIVE
endif

ifeq ($(CONFIG_RTL_8812_1T1R_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_8812_1T1R_SUPPORT
endif

ifeq ($(CONFIG_RTL_92D_DMDP),y)
COMMON_CFLAGS += -DCONFIG_RTL_92D_DMDP
endif

ifeq ($(CONFIG_ENABLE_EFUSE),y)
COMMON_CFLAGS += -DWLAN_MAC_FROM_EFUSE
endif

ifeq ($(CONFIG_RTL_FAST_FILTER), y)
COMMON_CFLAGS += -DCONFIG_RTL_FAST_FILTER 
endif

ifeq ($(CONFIG_BAND_2G_ON_WLAN0), y)
COMMON_CFLAGS += -DCONFIG_BAND_2G_ON_WLAN0
endif

ifeq ($(CONFIG_BAND_5G_ON_WLAN0), y)
COMMON_CFLAGS += -DCONFIG_BAND_5G_ON_WLAN0
endif

#ifeq ($(CONFIG_RTL_DUAL_PCIESLOT_BIWLAN),y)
ifeq ($(CONFIG_USE_PCIE_SLOT_0),y)
ifeq ($(CONFIG_USE_PCIE_SLOT_1),y)
ifeq ($(CONFIG_SLOT_0_92C),y)
ifeq ($(CONFIG_SLOT_1_92C),y)
COMMON_CFLAGS += -DCONFIG_RTL_DUAL_PCIESLOT_BIWLAN
endif
endif
endif
endif

ifeq ($(CONFIG_RTL_WLAN_DOS_FILTER),y)
COMMON_CFLAGS += -DCONFIG_RTL_WLAN_DOS_FILTER
endif

#ifeq ($(CONFIG_APP_APPLE_MFI_WAC),y)
#COMMON_CFLAGS += -DWAC_CONFIGURING_REPEATER
#endif


#zj: add for QoS
ifeq ($(GW),1)
	GATEWAY += -DENABLE_QOS
#	ifneq ($(CONFIG_QOS_UI_BY_BANDWIDTH),n)
#	ifneq ($(CONFIG_QOS_UI_BY_BANDWIDTH),)
                GATEWAY += -DQOS_BY_BANDWIDTH
#	endif
#	endif
endif

ifeq ($(CONFIG_NETFILTER_XT_MATCH_LAYER7),y)
GATEWAY += -DCONFIG_NETFILTER_XT_MATCH_LAYER7
endif

ifeq ($(CONFIG_APP_FWD),y)
COMMON_CFLAGS += -DCONFIG_APP_FWD
endif

ifeq ($(CONFIG_APP_BOA_NEW_UI),y)
COMMON_CFLAGS += -DCONFIG_APP_BOA_NEW_UI
endif

ifeq ($(CONFIG_APP_TR069),y)
COMMON_CFLAGS += -DCONFIG_APP_TR069
COMMON_CFLAGS += -D_PRMT_X_TELEFONICA_ES_DHCPOPTION_
#COMMON_CFLAGS += -D_ALPHA_DUAL_WAN_SUPPORT_

ifeq ($(CONFIG_USER_CWMP_WITH_SSL),y)
COMMON_CFLAGS += -D_CWMP_WITH_SSL_
endif

ifneq ($(CONFIG_MTD_CHAR), y)
ifeq ($(KERNEL_VER),linux-2.6.30)
$(error You have to turn on the kernel option "CONFIG_MTD_CHAR" for TR069)
endif
endif

#ifneq ($(CONFIG_MTD_CHAR), y)
#$(error You have to turn on the kernel option "CONFIG_MTD_CHAR" for TR069)
#endif

ifeq ($(CONFIG_USER_CWMP_WITH_TR143),y)
COMMON_CFLAGS += -DCONFIG_USER_CWMP_WITH_TR143
endif

endif

ifdef CONFIG_USER_CWMP_WITH_TR181_V6
COMMON_CFLAGS += -DTR181_SUPPORT
endif

ifdef CONFIG_RTL_WEBPAGES_IN_ROOTFS
COMMON_CFLAGS += -DCONFIG_RTL_WEBPAGES_IN_ROOTFS
endif

ifdef CONFIG_APP_AUTH_2NDSRV
COMMON_CFLAGS += -DCONFIG_APP_AUTH_2NDSRV
endif

ifeq ($(GW),1)
COMMON_CFLAGS += $(GATEWAY)
endif

ifeq ($(CONFIG_RTL_SUPPORT_MULTI_PROFILE),y)
COMMON_CFLAGS += -DWLAN_PROFILE
endif

ifeq ($(CONFIG_APP_MINIIGD-1.8),y)
COMMON_CFLAGS += -DUSE_MINIUPNPD_V1.8
endif

ifeq ($(CONFIG_APP_EBTABLES),y)
COMMON_CFLAGS += -DCONFIG_APP_EBTABLES
endif

ifeq ($(CONFIG_APP_SMTP_CLIENT),y)
COMMON_CFLAGS += -DCONFIG_APP_SMTP_CLIENT
endif

ifeq ($(CONFIG_APP_RTK_VOIP),y)
PWD := $(shell pwd)
ifeq ($(CONFIG_APP_RTK_VOIP_E8C),y)
export VOIP_APP = $(PWD)/../rtk_voip-e8c
else
ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxE
export VOIP_APP = $(PWD)/../rtk_voip-89xxE
else
export VOIP_APP = $(PWD)/../rtk_voip
endif
endif
COMMON_CFLAGS += -DVOIP_SUPPORT -I$(VOIP_APP)/includes -I$(VOIP_APP)/web -I$(PWD) -I$(PWD)/..
ifdef CONFIG_RTK_VOIP_PACKAGE_867X
COMMON_CFLAGS += -DATA867x
endif
ifdef CONFIG_RTK_VOIP_IP_PHONE
COMMON_CFLAGS += -I$(VOIP_APP)/ui/include
endif
endif

ifdef CONFIG_EBTABLES_KERNEL_SUPPORT
COMMON_CFLAGS += -DCONFIG_EBTABLES_KERNEL_SUPPORT
endif

ifndef CONFIG_RTL_AP
COMMON_CFLAGS += -DSEND_GRATUITOUS_ARP
endif
ifeq ($(CONFIG_APP_CPU_UTILIZATION), y)
COMMON_CFLAGS += -DCONFIG_CPU_UTILIZATION
endif
ifeq ($(CONFIG_SUPPORT_RUSSIA_FEATURES),y)
COMMON_CFLAGS += -DSUPPORT_ZIONCOM_RUSSIA
endif

ifeq ($(CONFIG_APP_APPLE_MFI_WAC),y)
COMMON_CFLAGS += -DCONFIG_APP_APPLE_MFI_WAC
endif

ifeq ($(CONFIG_APP_RADVD),y)
ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
COMMON_CFLAGS += -DSUPPORT_RDNSS_OPTION -DSUPPORT_DNSSL_OPTION
endif
endif

ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
COMMON_CFLAGS += -DSUPPORT_DEFEAT_IP_SPOOL_DOS 
endif

ifeq ($(CONFIG_APP_CAPWAP),y)
COMMON_CFLAGS += -DRTK_CAPWAP
endif

ifeq ($(CONFIG_RTL_USERSPACE_WTDOG),y)
COMMON_CFLAGS += -DAPP_WATCHDOG
endif
#------------------------------------------------------------------------------
# Export variables
#------------------------------------------------------------------------------
export COMMON_CFLAGS
export APMIB_SHARED
export APMIB_LIB

#------------------------------------------------------------------------------
# Targets
#------------------------------------------------------------------------------
ifeq ($(CONFIG_RTL_8196C_iNIC),y)
#iNIC no boa ,only need tools to build code
TARGET = tools
else
ifeq ($(CONFIG_RTL_8198_INBAND_AP),y)
TARGET = tools
else
TARGET = tools defconfig apmib utils system boa html 
endif
endif

all: configcheck generate_mk $(TARGET)
ifeq ($(x86),1)
	rm -rf x86
	mkdir x86
	cp -av html x86/web
	cp src/boa x86/
	cp utils/flash x86/
	cp src/boa.conf.x86 x86/boa.conf
	cp src/mime.types x86/
ifeq ($(APMIB_SHARED),1)
	cp apmib/libapmib.so x86/
endif
endif

generate_mk:
	@echo "SYS_COMMON_CFLAGS=$(COMMON_CFLAGS)" > ./wfaudio.include.mk
	@echo "export SYS_COMMON_CFLAGS" >> ./wfaudio.include.mk

configcheck:
	@echo "" > ./.diff_config;
	@if [ -e .kernel_config ]; then \
		diff .kernel_config $(DIR_LINUX_KERNEL)/.config > .diff_config; \
		if [ -s .diff_config ]; then \
		        cp $(DIR_LINUX_KERNEL)/.config .kernel_config; \
			make clean; \
                fi; \
        else \
                cp $(DIR_LINUX_KERNEL)/.config ./.kernel_config; \
	fi;

	@echo "" > ./.diff_users_config;
	@if [ -e .users_config ]; then \
		diff .users_config $(DIR_USERS)/.config > .diff_users_config; \
		if [ -s .diff_users_config ]; then \
			cp $(DIR_USERS)/.config .users_config; \
			make	clean; \
		fi; \
	else \
		cp $(DIR_USERS)/.config ./.users_config; \
	fi;	
#boa:
#	(cd src && $(MAKE) $(MFLAGS))

boa:
	make -C src

boa-clean:
	make -C src clean

apmib:
	make -C apmib

apmib-clean:
	make -C apmib clean

utils:
	make -C utils

utils-clean:
	make -C utils clean
	
system:
	make -C system

system-clean:
	make -C system clean

defconfig:
	make -C defconfig 
	
defconfig-clean:
	make -C defconfig clean
	
tools:
	make -C tools

tools-clean:
	make -C tools clean
	
#html: tools
html:
	make -C html

html-clean:
	make -C html clean

docs:
	(cd docs && $(MAKE) $(MFLAGS))

clean:
	-(cd src && $(MAKE) $(MFLAGS) clean)
	-(cd docs && $(MAKE) clean)
	make apmib-clean
	make utils-clean
	make system-clean
	make boa-clean
	make defconfig-clean
	make html-clean
	make tools-clean

distclean:	mrclean

mrclean:	clean
	-(cd src && $(MAKE) $(MFLAGS) mrclean)
	-(cd docs && $(MAKE)$(MFLAGS) mrclean)
	rm -f config.status config.cache config.h config.log 
	rm -f Makefile *~

# dummy target for install
install:
	@echo Please read the documentation then install manually.

romfs:
ifeq ($(APMIB_SHARED),1)
	$(ROMFSINST) apmib/$(APMIB_LIB) /lib/libapmib.so
endif
ifneq ($(CONFIG_RTL_8196C_iNIC),y)
ifneq ($(CONFIG_RTL_8198_INBAND_AP),y)
ifneq ($(CONFIG_RTL_NFJROM_MP),y)
ifneq ($(CONFIG_RTL_8198_AP_ROOT),y)
ifneq ($(CONFIG_RTL_8197D_AP),y)
	$(ROMFSINST) system/ppp_inet /bin/ppp_inet
	$(ROMFSINST) system/ntp_inet /bin/ntp_inet
	$(ROMFSINST) system/ddns_inet /bin/ddns_inet
ifeq  ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
	$(ROMFSINST) system/ipv6_manage_inet /bin/ipv6_manage_inet
endif
endif  #CONFIG_RTL_8197D_AP
endif	#CONFIG_RTL_8198_AP_ROOT
	$(ROMFSINST) src/boa /bin/boa
	$(ROMFSINST) -d src/boa.conf /etc/boa/boa.conf
	$(ROMFSINST) -d src/mime.types /etc/boa/mime.types
ifdef CONFIG_RTL_WEBPAGES_IN_ROOTFS
	rm -rf tmp-html
	mkdir tmp-html
	cd tmp-html && tar jxvf ../html/www_files.tar.bz2 && cd -
	mv tmp-html $(DIR_ROMFS)/web
	$(ROMFSINST) -s /var/config.dat /web/config.dat
else
	$(ROMFSINST) -s /var/web /web
endif
endif	#CONFIG_RTL_NFJROM_MP
	$(ROMFSINST) utils/flash /bin/flash
ifeq ($(CONFIG_APP_FWD),y)	
	$(ROMFSINST) utils/fwd /bin/fwd
endif
ifeq ($(CONFIG_APP_CAPWAP),y)	
	$(ROMFSINST) utils/fwupg /bin/fwupg
endif

ifneq ($(CONFIG_RTL_NFJROM_MP),y)
	$(ROMFSINST) system/sysconf /bin/sysconf
endif
	$(ROMFSINST) system/timelycheck /bin/timelycheck
ifeq ($(CONFIG_AUTO_DHCP_CHECK),y)
	$(ROMFSINST) system/Auto_DHCP_Check /bin/Auto_DHCP_Check
endif
endif	#CONFIG_RTL_8198_INBAND_AP
endif	#CONFIG_RTL_8196C_iNIC
ifeq ($(CONFIG_RTL_COMAPI_CFGFILE),y)
	rm -rf Wireless
	mkdir Wireless
	mv Wireless $(DIR_ROMFS)/etc
	$(ROMFSINST) -s /var/RTL8192CD.dat  /etc/Wireless/RTL8192CD.dat
endif
	cp ./src/certificate.crt ./../../romfs/etc
	cp ./src/privateKey.key ./../../romfs/etc
