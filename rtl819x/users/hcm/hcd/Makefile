#CFLAGS = -Wall -g -Os
#CFLAGS += -DRT_WLAN
#include $(DIR_LINUX)/.config

ifeq ($(wildcard $(DIR_LINUX)/.config),)
CFLAGS += -DKERNEL_2_6_30
include ../../linux-2.6.30/.config
DIR_LINUX_KERNEL = ../../linux-2.6.30
else
KERNEL_VER=$(patsubst $(DIR_ROOT)/%,%,$(DIR_LINUX))
ifeq ($(KERNEL_VER),linux-2.6.30)
CFLAGS += -DKERNEL_2_6_30
endif
ifeq ($(KERNEL_VER),linux-3.10)
CFLAGS += -DKERNEL_3_10
endif
include $(DIR_LINUX)/.config
DIR_LINUX_KERNEL = $(DIR_LINUX)
endif
ifeq ($(wildcard $(DIR_USERS)/.config),)
include ../.config
else
include $(DIR_USERS)/.config
endif
SRCS = hcd.c mib.c cmd.c wlan_if.c site_survey.c sys_init.c

ifeq ($(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK),y)
SRCS += RgbLedBlink.c
endif
EXEC = hcd
OBJS = $(SRCS:.c=.o)

WLANSUPP= -DUNIVERSAL_REPEATER -DWLAN_WPA -DWLAN_WDS -DWLAN_8185AG -DWLAN_WPA2 $(TLS)
#WLANSUPP += -DWIFI_SIMPLE_CONFIG -DCONFIG_RTL8196C_AP_ROOT
WLANSUPP += -DWIFI_SIMPLE_CONFIG
#ifdef CONFIG_RTL_LITE_CLNT
WLANSUPP += -DCONFIG_RTL_LITE_CLNT
#endif

CFLAGS += $(WLANSUPP)
ifdef CONFIG_RTL_8198_AP_ROOT
CONFIG_RTL_AP = CONFIG_RTL_8198_AP_ROOT
endif

ifdef CONFIG_RTL_8197D_AP
CONFIG_RTL_AP = CONFIG_RTL_8197D_AP
endif
ifdef CONFIG_RTL_AP
AP=1
else
CFLAGS += -DHOME_GATEWAY
endif
ifeq ($(CONFIG_RTL_819X),y)
    CFLAGS += -DCONFIG_RTL_819X
endif
ifeq ($(CONFIG_RTL_819XD),y)
    CFLAGS += -DCONFIG_RTL_819XD
endif
ifeq ($(CONFIG_RTL_8196B_GW),y)
    CFLAGS += -DCONFIG_RTL_8196B
endif
ifeq ($(CONFIG_RTL_8196C_GW),y)
    CFLAGS += -DCONFIG_RTL_8196C
endif

ifeq ($(CONFIG_RTL_8198C),y)
    CFLAGS += -DCONFIG_RTL_8198C
endif

ifeq ($(CONFIG_RTL_8198_GW),y)
	CFLAGS += -DCONFIG_RTL_8198
endif

ifeq ($(CONFIG_RTL_8198_AP_ROOT),y)
        CFLAGS += -DCONFIG_RTL_8198 -DCONFIG_RTL_8198_AP_ROOT
ifdef CONFIG_8198_PORT5_GMII
	CFLAGS += -DGMII_ENABLED
endif
endif

ifeq ($(CONFIG_RTL_8197D_AP),y)
	CFLAGS += -DCONFIG_RTL_8197D_AP
endif

ifeq ($(CONFIG_RTL_AP_PACKAGE),y)
	CFLAGS += -DCONFIG_RTL_AP_PACKAGE
endif

ifeq ($(CONFIG_RTL_819XD),y)
	CFLAGS += -DCONFIG_RTL_819XD
endif

ifeq ($(CONFIG_RTL_8196E),y)
	CFLAGS += -DCONFIG_RTL_8196E
endif

ifeq ($(CONFIG_RTL_8198B),y)
	CFLAGS += -DCONFIG_RTL_8198B
endif
#CFLAGS += -DCONFIG_RTL8196B -DCONFIG_RTL8196C -DCONFIG_RTL8198
#CFLAGS += -DCONFIG_HCD_FLASH_SUPPORT
#CFLAGS += -DINBAND_WPS_OVER_HOST

ifeq ($(CONFIG_RTL_92D_SUPPORT),y)
CFLAGS += -DCONFIG_RTL_92D_SUPPORT
endif

ifeq ($(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK),y)
CFLAGS += -DCONFIG_FIRMWARE_UPGRADE_LEB_BLINK
endif

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
#mark_issue
#LDFLAGS                =  -static
#LDLIBS += ../../inband_lib/inband.a
LDLIBS += ../../rtk_inbandLib/inband.a

ifeq ($(CONFIG_APP_ADAPTER_API), y)
LDLIBS += ../../boa/apmib/libapmib.so
LDLIBS += ../../adapter-api/rtk_api/librtk_adapter.so
CFLAGS += -DCONFIG_APP_ADAPTER_API
CFLAGS += -DQOS_BY_BANDWIDTH
ifeq ($(CONFIG_APP_QOS_MNP),y)
CFLAGS += -DCONFIG_RTL_QOS_MONOPOLY_SUPPORT
endif
endif

ifeq ($(CONFIG_APP_RTK_INBAND_CTL_REPORT_ERROR), y)
CFLAGS += -DCONFIG_REPORT_ERROR
endif

all: $(EXEC)

$(EXEC): $(OBJS) $(LDLIBS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(CFLAGS) $(LDLIBS)

#ioh.a: ioh.o
#	$(AR) rcs $@ ioh.o

#inband.a: ioh.o inband_if.o
#	$(AR) rcs $@ ioh.o inband_if.o

clean:
	-rm -f $(EXEC) *.elf *.gdb *.o *.a

romfs:
	$(ROMFSINST) /bin/$(EXEC)
