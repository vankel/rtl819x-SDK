#CFLAGS = -Wall -g -Os
#CFLAGS += -DRT_WLAN
CFLAGS = -I./
#include ../../linux-2.4.18/.config
ifeq ($(wildcard $(DIR_USERS)/.config),)
include ../.config
else
include $(DIR_USERS)/.config
endif

CFLAGS += -Os -DINBAND_HOST -fPIC

ifeq ($(CONFIG_APP_RTK_INBAND_CTL_REPORT_ERROR), y)
CFLAGS += -DCONFIG_REPORT_ERROR
endif
ifeq ($(CONFIG_APP_ADAPTER_API), y)
CFLAGS += -DCONFIG_APP_ADAPTER_API
CFLAGS += -DQOS_BY_BANDWIDTH
ifeq ($(CONFIG_APP_QOS_MNP),y)
CFLAGS += -DCONFIG_RTL_QOS_MONOPOLY_SUPPORT
endif
endif
SRCS = ioh.c inband_if.c inband_api.c

OBJS = $(SRCS:.c $(CFLAGS) =.o)

#CC = mips-linux-gcc
#LD = mips-linux-gcc
CC = rsdk-linux-gcc
LD = rsdk-linux-gcc

all: inband.a 

inband.a:inband_api.o ioh.o inband_if.o
	$(AR) rcs $@ inband_api.o ioh.o inband_if.o

clean:
	-rm -f *.elf *.gdb *.o *.a $(EXEC)
