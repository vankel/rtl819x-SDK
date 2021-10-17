ifeq ($(wildcard $(DIR_LINUX)/.config),)
DIR_LINUX_KERNEL = $(DIR_ROOT)/linux-2.6.30
else
DIR_LINUX_KERNEL = $(DIR_LINUX)
endif
ifeq ($(wildcard $(DIR_USERS)/.config),)
include ../.config
else
include $(DIR_USERS)/.config
endif
CC		= $(CROSS_COMPILE)gcc
COPT		= -O2 -Wall
CFLAGS		= ${COPT}
#CFLAGS		+= -DINBAND_HOST
#CFLAGS		+= -DINBAND_WPS_OVER_HOST
CFLAGS		+= -DMULTI2UNI
#LDFLAGS	=  -static
#STRIP		= mips-uclibc-strip
LIBS		= ../rtk_inbandLib/inband.a
CFLAGS += -DCONFIG_RTL_LITE_CLNT
ifeq ($(CONFIG_APP_RTK_INBANDHOST_PTHREAD_LOCK), y)
CFLAGS += -DINBANDHOST_PTHREAD_LOCK
endif

ifeq ($(CONFIG_APP_ADAPTER_API), y)
#LIBS += ../../bdapter-api/rtk_api/rtk_adapter.so
CFLAGS += -DCONFIG_APP_ADAPTER_API
CFLAGS += -DQOS_BY_BANDWIDTH
ifeq ($(CONFIG_APP_QOS_MNP),y)
CFLAGS += -DCONFIG_RTL_QOS_MONOPOLY_SUPPORT
endif
endif
ifeq ($(CONFIG_FIRMWARE_UPGRADE_LEB_BLINK),y)
CFLAGS += -DCONFIG_FIRMWARE_UPGRADE_LEB_BLINK
endif
ifeq ($(CONFIG_APP_RTK_INBAND_HOST_LIBRARY), y)
LIB_DIR := ../rtk_inbandLib
LIB_OBJS := $(LIB_DIR)/inband_api.o \
			$(LIB_DIR)/ioh.o \
			$(LIB_DIR)/inband_if.o
CFLAGS += -DCONFIG_INBAND_HOST_LIBRARY -fPIC -lpthread
LD := $(CROSS_COMPILE)ld
LDFLAGS	= -shared -fPIC
SOURCES := main.c 
TARGET_LIB := libInbandHost.so
TARGET_EXE := InbandHostTest
ifeq ($(CONFIG_APP_RTK_INBAND_HOST_LIBRARY_TEST), y)
CFLAGS += -DCONFIG_LIBRARY_TEST
endif
all: $(TARGET_LIB) $(TARGET_EXE) $(LIBS) $(SOURCES:.c=.o) 
	make -C eventd
$(TARGET_LIB): main.o
	$(LD) main.o $(LIB_OBJS) $(LDFLAGS) -o $(TARGET_LIB)
ifeq ($(CONFIG_APP_RTK_INBAND_HOST_LIBRARY_TEST), y)
ifeq ($(CONFIG_APP_RTK_INBAND_CTL_REPORT_ERROR), y)
SOURCES += ../cloud/ipc/cloud_ipc.c
$(TARGET_EXE): main.o inband_host_test.o ../cloud/ipc/cloud_ipc.o
	$(CC) $(CFLAGS) -L./ -lInbandHost -o $@ $^ -Wl
else
$(TARGET_EXE): main.o inband_host_test.o 
	$(CC) $(CFLAGS) -L./ -lInbandHost -o $@ $^ -Wl
endif
else
$(TARGET_EXE):
endif
.PHONY: clean
clean:
	make -C eventd clean
	rm *.so *.o -rf
ifeq ($(CONFIG_APP_RTK_INBAND_HOST_LIBRARY_TEST), y)
	rm $(TARGET_EXE)
endif
romfs:
	make -C eventd romfs
	$(ROMFSINST) $(TARGET_LIB) /lib/$(TARGET_LIB)
ifeq ($(CONFIG_APP_RTK_INBAND_HOST_LIBRARY_TEST), y)
	$(ROMFSINST) /bin/$(TARGET_EXE)
endif
else
all: inband
	make -C eventd


inband_sources = main.c 

inband:	$(inband_sources:.c=.o) $(LIBS)
	$(CC) -s $(CFLAGS) $(LDFLAGS) $^ -o $@ $(LIBS)	

clean:
	make -C eventd clean
	rm -f inband *.o *.a

romfs:
	$(ROMFSINST) /bin/inband
	$(ROMFSINST) mib_sample/ /etc
	make -C eventd romfs
endif

