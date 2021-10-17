ifeq ($(wildcard $(DIR_LINUX)/.config),)
include $(DIR_ROOT)/linux-2.6.30/.config
else
include $(DIR_LINUX)/.config
endif

ifeq ($(wildcard $(DIR_USERS)/.config),)
include ../../.config
else
include $(DIR_USERS)/.config
endif

ifndef DIR_RSDK
include ../../.config
export PATH := $(shell pwd)/../../$(CONFIG_RSDKDIR)/bin:$(PATH)
export CC = $(CROSS_COMPILE)gcc
export CPP = $(CROSS_COMPILE)gcc -E
export AR =$(CROSS_COMPILE)ar
endif

MAKE=make
COMMON_CFLAGS =

# Gateway flag
GATEWAY = -DHOME_GATEWAY -DDOS_SUPPORT
# WLAN support
WLANSUPP = -DUNIVERSAL_REPEATER -DWLAN_WPA -DWLAN_WDS -DWLAN_8185AG -DWLAN_WPA2 $(TLS)
WLANSUPP += -DWIFI_SIMPLE_CONFIG

# routing support
ifeq ($(CONFIG_APP_ROUTED), y)
COMMON_CFLAGS += -DROUTE_SUPPORT
endif

ifeq ($(CONFIG_RTK_VLAN_SUPPORT),y)
COMMON_CFLAGS += -DVLAN_CONFIG_SUPPORTED
endif

#for 11s mesh
ifeq ($(CONFIG_RTL_MESH_SUPPORT),y)
        SUBDIRS += 11s-PathSelection
        MESH_EXTRA_CONFIG_FLAGS += -DCONFIG_RTK_MESH -D_MESH_ACL_ENABLE_ -DCONFIG_NEW_MESH_UI
        ifeq ($(CONFIG_TEST_PKG),y)
               MESH_EXTRA_CONFIG_FLAGS += -D_11s_TEST_MODE_
                TESTPKG=1
        endif
        ifeq ($(CONFIG_NMS),y)
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

ifeq ($(CONFIG_USB_SERIAL_OPTION),y)
COMMON_CFLAGS += -DRTK_USB3G
#ifeq ($(CONFIG_POCKET_ROUTER_SUPPORT),y)
#COMMON_CFLAGS += -DRTK_USB3G_PORT5_LAN
#endif
endif

#CONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D
ifeq ($(CONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D),y)
COMMON_CFLAGS += -DCONFIG_RTL_92D_DMDP  -DCONFIG_RTL_DUAL_PCIESLOT_BIWLAN_D -DFOR_DUAL_BAND
endif

ifeq ($(CONFIG_RTL_P2P_SUPPORT),y)
COMMON_CFLAGS += -DCONFIG_RTL_P2P_SUPPORT
endif

ifeq ($(CONFIG_RTL_92D_DMDP),y)
COMMON_CFLAGS += -DFOR_DUAL_BAND
endif

#
# Extra Feature Configuration
#
ifeq ($(CONFIG_HTTP_FILE_SERVER_SUPPORT),y)
COMMON_CFLAGS += -DHTTP_FILE_SERVER_SUPPORTED
COMMON_CFLAGS += -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -DENABLE_LFS
endif

ifeq ($(CONFIG_RTL_STP),y)
COMMON_CFLAGS += -DCONFIG_RTL_STP
endif

ifeq ($(CONFIG_BASEON_SCHEDULE_FILTER),y)
COMMON_CFLAGS += -DCONFIG_SCH_FILTER
endif

ifeq ($(CONFIG_IPV6),y)
COMMON_CFLAGS += -DCONFIG_IPV6
endif

ifeq ($(CONFIG_RTL_819X),y)
	COMMON_CFLAGS += -DCONFIG_RTL_819X
endif

ifeq ($(CONFIG_RTL_8196B_GW),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8196B
endif

ifeq ($(CONFIG_RTL_8196C_GW),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8196C
endif

ifeq ($(CONFIG_RTL_8196E),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8196E
endif

ifeq ($(CONFIG_RTL_8198_GW),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8198
endif

ifeq ($(CONFIG_RTL_8198B),y)
	COMMON_CFLAGS += -DCONFIG_RTL_8198B
endif

ifeq ($(CONFIG_RTL_8198_AP_ROOT),y)
        COMMON_CFLAGS += -DCONFIG_RTL_8198 -DCONFIG_RTL_8198_AP_ROOT
ifdef CONFIG_8198_PORT5_GMII
	COMMON_CFLAGS += -DGMII_ENABLED
endif
endif

ifeq ($(CONFIG_RTL_819XD),y)
	COMMON_CFLAGS += -DCONFIG_RTL_819XD
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

ifeq ($(CONFIG_RTL_802_1X_CLIENT_SUPPORT),y)
	COMMON_CFLAGS += -DCONFIG_RTL_802_1X_CLIENT_SUPPORT
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

ifeq ($(CONFIG_RTL_92D_DMDP),y)
COMMON_CFLAGS += -DCONFIG_RTL_92D_DMDP
endif

ifeq ($(CONFIG_ENABLE_EFUSE),y)
COMMON_CFLAGS += -DWLAN_MAC_FROM_EFUSE
endif

ifeq ($(CONFIG_RTL_FAST_FILTER), y)
COMMON_CFLAGS += -DCONFIG_RTL_FAST_FILTER 
endif

ifeq ($(CONFIG_RTL_DUAL_PCIESLOT_BIWLAN),y)
COMMON_CFLAGS += -DCONFIG_RTL_DUAL_PCIESLOT_BIWLAN
endif

ifeq ($(CONFIG_RTL_WLAN_DOS_FILTER),y)
COMMON_CFLAGS += -DCONFIG_RTL_WLAN_DOS_FILTER
endif

#zj: add for QoS
ifeq ($(GW),1)
	GATEWAY += -DENABLE_QOS
                GATEWAY += -DQOS_BY_BANDWIDTH
endif

ifeq ($(CONFIG_NETFILTER_XT_MATCH_LAYER7),y)
GATEWAY += -DCONFIG_NETFILTER_XT_MATCH_LAYER7
endif

ifdef CONFIG_APP_TR069
COMMON_CFLAGS += -DCONFIG_APP_TR069

ifeq ($(CONFIG_USER_CWMP_WITH_SSL),y)
COMMON_CFLAGS += -D_CWMP_WITH_SSL_
endif

ifeq ($(CONFIG_USER_CWMP_WITH_TR181), y)
COMMON_CFLAGS += -DCONFIG_USER_CWMP_WITH_TR181
endif

ifeq ($(CONFIG_USER_CWMP_WITH_TR181_V6), y)
COMMON_CFLAGS += -DTR181_SUPPORT
endif

ifeq ($(CONFIG_USER_CWMP_WITH_TR069_ANNEX_G), y)
COMMON_CFLAGS += -DTR069_ANNEX_G
endif

ifneq ($(CONFIG_MTD_CHAR), y)
ifeq ($(KERNEL_VER),linux-2.6.30)
$(error You have to turn on the kernel option "CONFIG_MTD_CHAR" for TR069)
endif
endif

ifeq ($(KERNEL_VER),linux-2.6.30)
COMMON_CFLAGS += -DKERNEL_2630
endif

#ifneq ($(CONFIG_MULTIPLE_WAN_SUPPORT), y)
#$(error You have to turn on Multiple_Wan_Support for TR069)
#endif

ifeq ($(CONFIG_USER_CWMP_WITH_TR143),y)
COMMON_CFLAGS += -DCONFIG_USER_CWMP_WITH_TR143
endif

endif

ifdef CONFIG_RTL_WEBPAGES_IN_ROOTFS
COMMON_CFLAGS += -DCONFIG_RTL_WEBPAGES_IN_ROOTFS
endif

ifdef CONFIG_APP_AUTH_2NDSRV
COMMON_CFLAGS += -DCONFIG_APP_AUTH_2NDSRV
endif

COMMON_CFLAGS += $(GATEWAY)

ifeq ($(CONFIG_IPV4_DHCP_OPTION),y)
COMMON_CFLAGS += -DCHECK_IPV4_DHCP_OPTION
endif

ifeq ($(CONFIG_IPV6_DHCP_OPTION),y)
COMMON_CFLAGS += -DCHECK_IPV6_DHCP_OPTION
endif

ifdef CONFIG_MULTIPLE_WAN_SUPPORT
	COMMON_CFLAGS += -D MULTI_WAN_SUPPORT
endif


#------------------------------------------------------------------------------
# Export variables
#------------------------------------------------------------------------------
export COMMON_CFLAGS

CFLAGS += $(COMMON_CFLAGS)

CWMPCLIENT_ROOT=$(shell pwd)

CC = mips-linux-gcc 
STRIP = mips-linux-strip

ifeq ($(CONFIG_USER_CWMP_WITH_TR143),y)
TR143_EXEC = udpechoserver
TR143_LIBOBJS = udpechoserverlib.o
TR143_LIBDEPH = udpechoserverlib.h
else
TR143_EXEC =
TR143_LIBOBJS =
TR143_LIBDEPH =
endif

EXEC = cwmpClient
DEPEND = ./.depend

TOOLCHAIN_PATH =$(shell pwd)/../../../$(CONFIG_RSDKDIR)



ifneq ($(CONFIG_USER_CWMP_WITH_TR181), y)
OBJS =	prmt_igd.o prmt_deviceinfo.o prmt_mngmtserver.o \
	prmt_layer3fw.o prmt_landevice.o prmt_landevice_wlan.o \
	prmt_wandevice.o prmt_wancondevice.o \
	prmt_time.o prmt_ddns.o \
	prmt_services.o prmt_userif.o \
	prmt_captiveportal.o prmt_traceroute.o prmt_landevice_usb.o \
	prmt_ippingdiag.o prmt_queuemnt.o prmt_tr143.o prmt_apply.o porting.o main.o
OBJS +=	prmt_landevice_eth.o
	#prmt_wandsldiagnostics.o prmt_wanatmf5loopback.o

SOURCES =	prmt_igd.c prmt_deviceinfo.c prmt_mngmtserver.c \
	prmt_layer3fw.c prmt_landevice.c prmt_landevice_wlan.c \
	prmt_wandevice.c prmt_wancondevice.c \
	prmt_time.c prmt_ddns.c \
	prmt_services.c prmt_userif.c \
	prmt_captiveportal.c prmt_traceroute.c prmt_landevice_usb.c \
	prmt_ippingdiag.c prmt_queuemnt.c prmt_tr143.c prmt_apply.c porting.c main.c
SOURCES +=	prmt_landevice_eth.c


DEPH =	$(CWMPCLIENT_ROOT)/../../../config/autoconf.h \
	$(CWMPCLIENT_ROOT)/../../boa/src-project/LINUX/options.h \
	prmt_igd.h prmt_deviceinfo.h prmt_landevice_eth.h prmt_mngmtserver.h \
	prmt_layer3fw.h prmt_landevice.h prmt_landevice_wlan.h \
	prmt_wandevice.h prmt_wandsldiagnostics.h \
	prmt_wancondevice.h prmt_ippingdiag.h prmt_time.h prmt_ddns.h \
	prmt_wanatmf5loopback.h prmt_services.h prmt_userif.h \
	prmt_captiveportal.h prmt_traceroute.h prmt_landevice_usb.h \
	prmt_queuemnt.h prmt_apply.h
	
else
OBJS = tr181_device.o tr181_deviceInfo.o tr181_gatewayInfo.o tr181_userInterface.o \
	tr181_mgmtServer.o tr181_mgableDev.o tr181_eth.o tr181_ethIF.o tr181_MoCA.o \
	tr181_MoCA_IF.o tr181_wifi.o tr181_wifiRadio.o tr181_bridging.o tr181_bridge.o \
	tr181_ip.o tr181_ipIF.o tr181_dhcpv4.o tr181_dhcpv4Client.o tr181_dhcpv6.o tr181_dhcpv6Client.o \
	prmt_landevice_wlan.o prmt_ippingdiag.o prmt_wancondevice.o prmt_landevice.o prmt_landevice_eth.o \
	porting.o main.o prmt_deviceinfo.o prmt_mngmtserver.o tr181_dns.o tr181_dnsClient.o \
	tr181_lanConfigSecurity.o tr181_deviceInfoVendorLogFile.o

SOURCES = tr181_device.c tr181_deviceInfo.c tr181_gatewayInfo.c tr181_userInterface.c \
	tr181_mgmtServer.c tr181_mgableDev.c tr181_eth.c tr181_ethIF.c tr181_MoCA.c \
	tr181_MoCA_IF.c tr181_wifi.c tr181_wifiRadio.c tr181_bridging.c tr181_bridge.c \
	tr181_ip.c tr181_ipIF.c tr181_dhcpv4.c tr181_dhcpv4Client.c tr181_dhcpv6.c tr181_dhcpv6Client.c \
	prmt_landevice_wlan.c prmt_ippingdiag.c prmt_wancondevice.c prmt_landevice.c prmt_landevice_eth.c \
	porting.c main.c prmt_deviceinfo.c prmt_mngmtserver.c tr181_dns.c tr181_dnsClient.c \
	tr181_lanConfigSecurity.c tr181_deviceInfoVendorLogFile.c

DEPH = tr181_device.h tr181_deviceInfo.h tr181_gatewayInfo.h tr181_userInterface.h \
	tr181_mgmtServer.h tr181_mgableDev.h tr181_eth.h tr181_ethIF.h tr181_MoCA.h \
	tr181_MoCA_IF.h tr181_wifi.h tr181_wifiRadio.h tr181_bridging.h tr181_bridge.h \
	tr181_ip.h tr181_ipIF.h tr181_dhcpv4.h tr181_dhcpv4Client.h tr181_dhcpv6.h tr181_dhcpv6Client.h \
	prmt_landevice_wlan.h prmt_ippingdiag.h prmt_wancondevice.h prmt_landevice.h prmt_landevice_eth.h \
	prmt_deviceinfo.h prmt_mngmtserver.h tr181_dns.h tr181_dnsClient.h tr181_lanConfigSecurity.h \
	tr181_deviceInfoVendorLogFile.h

ifeq ($(CONFIG_USER_CWMP_WITH_TR069_ANNEX_G), y)
OBJS += $(CWMPCLIENT_ROOT)/stun.o $(CWMPCLIENT_ROOT)/udp.o

SOURCES += $(CWMPCLIENT_ROOT)/stun.c $(CWMPCLIENT_ROOT)/udp.c

DEPH += $(CWMPCLIENT_ROOT)/stun.h $(CWMPCLIENT_ROOT)/udp.h
endif

CFLAGS += -DCONFIG_RTL_92D_SUPPORT -DCONFIG_RTL_DUAL_PCIESLOT_BIWLAN
endif

ifeq ($(CONFIG_APP_GOAHEAD), y)
CFLAGS += -I$(CWMPCLIENT_ROOT)/../include -I$(CWMPCLIENT_ROOT)/../../goahead-2.1.1/LINUX
else
CFLAGS += -I$(CWMPCLIENT_ROOT)/../include -I$(CWMPCLIENT_ROOT)/../../boa/apmib -I$(CWMPCLIENT_ROOT)/../../boa/src -I$(CWMPCLIENT_ROOT)/../gsoaplib/soapcpp2 -I$(CWMPCLIENT_ROOT)/../gsoaplib/soapcpp2/plugin
endif


ifeq ($(CONFIG_APP_TR104),y)
PWD := $(shell pwd)
export VOIP_APP = $(PWD)/../../rtk_voip
COMMON_CFLAGS += -DCONFIG_APP_TR104 -DVOIP_SUPPORT -I$(VOIP_APP)/tr104 -I$(VOIP_APP)/includes -I$(VOIP_APP)/web -I$(PWD) -I$(PWD)/..
ifdef CONFIG_RTK_VOIP_PACKAGE_867X
COMMON_CFLAGS += -DATA867x
endif
ifdef CONFIG_RTK_VOIP_IP_PHONE
COMMON_CFLAGS += -I$(VOIP_APP)/ui/include
endif
endif

ifeq ($(CONFIG_USER_CWMP_WITH_TR181), y)
#set the ssl libraries
ifeq ($(CONFIG_USER_CWMP_WITH_OPENSSL),y)
LIBCWMP = ../lib/TR181_libcwmpssl.a ../lib/libgsoapssl.a

ifeq ($(CONFIG_APP_OPENSSL-0.9.8B),y)
LIBCWMPFLAG = -L$(CWMPCLIENT_ROOT)/../../openssl-0.9.8b/ -lssl -lcrypto -ldl
CFLAGS += -I$(CWMPCLIENT_ROOT)/../../openssl-0.9.8b/include -DHAS_MESSAGE_INTEGRITY
endif

ifeq ($(CONFIG_APP_OPENSSL-0.9.8I),y)
LIBCWMPFLAG = -L$(CWMPCLIENT_ROOT)/../../openssl-0.9.8i/ -lssl -lcrypto -ldl
CFLAGS += -I$(CWMPCLIENT_ROOT)/../../openssl-0.9.8i/include -DHAS_MESSAGE_INTEGRITY
endif

endif
ifeq ($(CONFIG_USER_CWMP_WITH_MATRIXSSL),y)
LIBCWMP = ../lib/TR181_libcwmpmssl.a ../lib/libgsoapmssl.a ../lib/libmatrixsslstatic.a
#LIBCWMPFLAG = -lc -L$(CWMPCLIENT_ROOT)/../matrixssl-1-8-3-open/src/ -lmatrixssl
LIBCWMPFLAG =
endif
ifneq ($(CONFIG_USER_CWMP_WITH_SSL),y)
LIBCWMP = ../lib/TR181_libcwmp.a ../lib/libgsoapck.a
LIBCWMPFLAG =
endif
else
#set the ssl libraries
ifeq ($(CONFIG_USER_CWMP_WITH_OPENSSL),y)
LIBCWMP = ../lib/libcwmpssl.a ../lib/libgsoapssl.a

ifeq ($(CONFIG_APP_OPENSSL-0.9.8B),y)
LIBCWMPFLAG = -L$(CWMPCLIENT_ROOT)/../../openssl-0.9.8b/ -lssl -lcrypto -ldl
CFLAGS += -I$(CWMPCLIENT_ROOT)/../../openssl-0.9.8b/include -DHAS_MESSAGE_INTEGRITY
endif

ifeq ($(CONFIG_APP_OPENSSL-0.9.8I),y)
LIBCWMPFLAG = -L$(CWMPCLIENT_ROOT)/../../openssl-0.9.8i/ -lssl -lcrypto -ldl
CFLAGS += -I$(CWMPCLIENT_ROOT)/../../openssl-0.9.8i/include -DHAS_MESSAGE_INTEGRITY
endif

endif
ifeq ($(CONFIG_USER_CWMP_WITH_MATRIXSSL),y)
LIBCWMP = ../lib/libcwmpmssl.a ../lib/libgsoapmssl.a ../lib/libmatrixsslstatic.a
#LIBCWMPFLAG = -lc -L$(CWMPCLIENT_ROOT)/../matrixssl-1-8-3-open/src/ -lmatrixssl
LIBCWMPFLAG =
endif
ifneq ($(CONFIG_USER_CWMP_WITH_SSL),y)
LIBCWMP = ../lib/libcwmp.a ../lib/libgsoapck.a
LIBCWMPFLAG =
endif
endif

ifeq ($(CONFIG_RTL_8196C),y)
CFLAGS += -DCONFIG_RTL_8196C
endif

ifeq ($(CONFIG_APP_GOAHEAD), y)
APMIBLIB=`pwd`/../../goahead-2.1.1/LINUX/apmib.so
else
APMIBLIB=`pwd`/../../boa/apmib/libapmib.so
endif

ifeq ($(CONFIG_APP_GOAHEAD), y)
OBJS += version.o utility.o 
else
OBJS += version.o prmt_utility.o arping.o debug.o
endif
CFLAGS += -DHOME_GATEWAY
CFLAGS += -DSUPER_NAME_SUPPORT

#add by bruce
include ../../.config
#include ../../../linux-2.6.30/.config
#CFLAGS += -D_PRMT_USERINTERFACE_ -DUNIVERSAL_REPEATER
ifeq ($(CONFIG_APP_ROUTED), y)
# routing support
CFLAGS += -DROUTE_SUPPORT
endif
ifeq ($(CONFIG_RTL_92D_SUPPORT),y)
CFLAGS  += -DCONFIG_RTL_92D_SUPPORT 
endif
ifeq ($(CONFIG_RTK_VLAN_SUPPORT),y)
CFLAGS += -DVLAN_CONFIG_SUPPORTED
endif

ifeq ($(CONFIG_RTL_8881A),y)
ifeq ($(CONFIG_SLOT_0_8192EE),y)
COMMON_CFLAGS += -DCONFIG_RTL8881A_RTL8192E -DCONFIG_RTL_DUAL_PCIESLOT_BIWLAN
endif
endif
CFLAGS += -D_CWMP_MAC_FILTER_
#CFLAGS += -D_ALPHA_DUAL_WAN_SUPPORT_

# To Support TR104
ifeq ($(CONFIG_APP_TR104),y)
LIBCWMP += ../../rtk_voip/tr104/voip_tr104.a ../../rtk_voip/voip_manager/libvoip_manager.so
endif

#additional linking library
#-lcrypt for crypt() when creating  the /etc/passwd file
LIBCWMP += -lcrypt

all: depend $(EXEC) $(TR143_EXEC)

udpechoserver.o: udpechoserver.c $(TR143_LIBDEPH)
	$(CC) -Os $(CFLAGS) -c -o $@ $<

$(TR143_LIBOBJS): udpechoserverlib.c $(TR143_LIBDEPH)
	$(CC) -Os $(CFLAGS) -c -o $@ $<

$(TR143_EXEC): udpechoserver.o $(TR143_LIBOBJS)
	$(CC) -Os $(LDFLAGS) -o $@ udpechoserver.o $(TR143_LIBOBJS)

%.o: %.c $(DEPH) $(TR143_LIBDEPH)
	$(CC) -Os $(CFLAGS) -c -o $@ $<

$(EXEC): $(OBJS) $(LIBCWMP) $(TR143_LIBOBJS) $(TR143_LIBDEPH)
	@#$(CC) -Os $(LDFLAGS) -o $@ $(OBJS) $(LIBCWMP) $(TR143_LIBOBJS) -lmib $(TOOLCHAIN_PATH)/lib/libm.a $(APMIBLIB) -lpthread $(LIBCWMPFLAG)
	$(CC) -Os $(LDFLAGS) -o $@ $(OBJS) $(LIBCWMP) $(TR143_LIBOBJS) $(TOOLCHAIN_PATH)/lib/libm.a $(APMIBLIB) -lpthread $(LIBCWMPFLAG)



ifeq ($(CONFIG_APP_GOAHEAD), y)
version.o : $(CWMPCLIENT_ROOT)/../../goahead-2.1.1/LINUX/version.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
utility.o : $(CWMPCLIENT_ROOT)/../../goahead-2.1.1/LINUX/utility.c
	echo $(CFLAGS)
	$(CC) $(CFLAGS) -c -o $@ $<
	cp -f $(CWMPCLIENT_ROOT)/../../goahead-2.1.1/LINUX/utility_gw.o $(CWMPCLIENT_ROOT)/utility.o
else
version.o : $(CWMPCLIENT_ROOT)/../../boa/src/version.c
	$(CC) $(CFLAGS) -c -o $@ $<
	
prmt_utility.o : prmt_utility.c
	echo $(CFLAGS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
arping.o : arping.c
	echo $(CFLAGS)
	$(CC) $(CFLAGS) -c -o $@ $<
	
debug.o : debug.c
	echo $(CFLAGS)
	$(CC) $(CFLAGS) -c -o $@ $<
	

endif



depend: $(SOURCES)
	if [ ! -e $(DEPEND) ]; then \
		$(CPP) $(DEBUG) $(CFLAGS) $(IFLAGS) -MM $^ > $(DEPEND); \
	fi
	
-include $(DEPEND)

clean:
	rm -f $(DEPEND)
	rm -f $(EXEC) *.o udpechoserver

