ifeq ($(wildcard $(DIR_LINUX)/.config),)
DIR_LINUX_KERNEL = $(DIR_ROOT)/linux-2.6.30
else
DIR_LINUX_KERNEL = $(DIR_LINUX)
endif
CC	=	$(CROSS_COMPILE)gcc
CXX 	=	$(CROSS_COMPILE)g++
LD 	=	$(CROSS_COMPILE)gcc
LDFLAGS =	-s -Wall
#CFLAGS	:=	-s -Wall -c -Os -DIPC_BASED_PATHSEL -D_RTL865X_ -I../../linux-2.6.30/include -Os -D__BIG_ENDIAN_BITFIELD -DCONFIG_CPU_BIG_ENDIAN $(CFLAGS)
ifeq ($(KERNEL_VER),linux-2.6.30)
CFLAGS	:=	-s -Wall -c -Os -DIPC_BASED_PATHSEL -D_RTL865X_ -I$(DIR_LINUX_KERNEL)/include -Os -D__BIG_ENDIAN_BITFIELD -DCONFIG_CPU_BIG_ENDIAN $(CFLAGS)
endif
ifeq ($(KERNEL_VER),linux-3.10)
CFLAGS	:=	-s -Wall -c -Os -DIPC_BASED_PATHSEL -D_RTL865X_ -I$(DIR_LINUX_KERNEL)/include/uapi -Os -D__BIG_ENDIAN_BITFIELD -DCONFIG_CPU_BIG_ENDIAN $(CFLAGS)
endif

all:		pathsel

ifeq ($(SLINK),1)
CFLAGS  += -ffunction-sections -fdata-sections
LDFLAGS += --static -Wl,--gc-sections
endif


pathsel:		pathselection.o		Path_OnDemand.o Tree_Proactive.o util_PathUpdate.o
			$(CC) $(LDFLAGS) -o pathsel pathselection.o Path_OnDemand.o Tree_Proactive.o util_PathUpdate.o


pathselection.o:		pathselection.c pathselection.h
			$(CC) $(CFLAGS) -o pathselection.o pathselection.c
			
Path_OnDemand.o:		Path_OnDemand.c pathselection.h
			$(CC) $(CFLAGS) -o Path_OnDemand.o  Path_OnDemand.c
			
Tree_Proactive.o:		Tree_Proactive.c Tree.h
			$(CC) $(CFLAGS) -o Tree_Proactive.o  Tree_Proactive.c

util_PathUpdate.o:		util_PathUpdate.h util_PathUpdate.c
			$(CC) $(CFLAGS) -o util_PathUpdate.o  util_PathUpdate.c

romfs:
			$(ROMFSINST) /bin/pathsel

clean:
			rm -f *.o
			rm -f pathsel
			rm -f core
