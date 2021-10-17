# $Id: Makefile,v 1.1.1.1 2007-08-06 10:04:43 root Exp $
# http://miniupnp.free.fr/
# Author : Thomas Bernard
# for use with GNU Make
#CFLAGS = -Wall -O2 -D_GNU_SOURCE -g
#CFLAGS = -Wall

ifeq ($(wildcard $(DIR_LINUX)/.config),)
DIR_LINUX_KERNEL = $(DIR_ROOT)/linux-2.6.30
INCLUDEDIR = $(DIR_LINUX_KERNEL)/include
CFLAGS += -DKERNEL_2_6_30
else
KERNEL_VER=$(patsubst $(DIR_ROOT)/%,%,$(DIR_LINUX))
ifeq ($(KERNEL_VER),linux-2.6.30)
INCLUDEDIR = $(DIR_LINUX_KERNEL)/include
CFLAGS += -DKERNEL_2_6_30
endif
ifeq ($(KERNEL_VER),linux-3.10)
INCLUDEDIR = $(DIR_LINUX_KERNEL)/include/uapi
CFLAGS += -DKERNEL_3_10
endif
DIR_LINUX_KERNEL = $(DIR_LINUX)
endif

CFLAGS := -Wall -D_GNU_SOURCE $(CFLAGS)
#CFLAGS += $(EXTRA_CFLAGS)
#reduce code
CFLAGS += -Os
#INCLUDEDIR = $(DIR_LINUX_KERNEL)/include/uapi
CFLAGS += -I$(INCLUDEDIR)

#CC = mips-uclibc-gcc
#CC = mips-uclibc-gcc-skylark
#CC = mips-linux-gcc
#CC = rsdk-uclibc-gcc
#CC = rsdk-linux-gcc

RM = rm -f
INSTALL = install
EVENTING_ENABLED=1
NATPMP_ENABLED=1

GW=1
ifeq ($(GW),1)
CFLAGS += -DCONFIG_RTL8186_GW
endif

ifeq ($(RTL8186_TR),1)
	CFLAGS		+= -DSUPPORT_HNAP
endif

ifeq ($(RTL8186_KB),1)
	CFLAGS          += -DSUPPORT_HNAP
	CFLAGS          += -DCONFIG_RTL8186_KB
endif

ifeq ($(RTL865X_GRC),1)
	CFLAGS          += -DSUPPORT_HNAP
	CFLAGS          += -DCONFIG_RTL8186_GR
endif
ifeq ($(EVENTING_ENABLED),1)
CFLAGS          += -DENABLE_EVENTS
endif

ifeq ($(NATPMP_ENABLED),1)
CFLAGS          += -DENABLE_NATPMP
endif
ifeq ($(CONFIG_APP_MINIIGD_IPV6),y)
CFLAGS		+= -DENABLE_IPV6
CFLAGS		+= -DENABLE_6FC_SERVICE
CFLAGS		+= -DUSE_NETFILTER
endif

# For mini_upnp stand alone
CFLAGS		+= -DSTAND_ALONE

#dzh begin
include $(DIR_USERS)/.config
ifdef CONFIG_APP_MULTIPPPOE
        CFLAGS += -D MULTI_PPPOE
endif
#dzh end
BASEOBJS = miniigd.o upnphttp.o upnpdescgen.o upnpsoap.o \
           upnpreplyparse.o minixml.o \
		   upnpredirect.o getifaddr.o daemonize.o upnpglobalvars.o upnpevents.o \
		   upnputils.o ifaddrs.o 
	
ifeq ($(CONFIG_APP_MINIIGD_IPV6),y)
BASEOBJS += 	 upnppinhole.o
endif
ifeq ($(NATPMP_ENABLED),1)
BASEOBJS += natpmp.o upnppermissions.o
endif

#LNXOBJS = linux/getifstats.o linux/iptcrdr.o
LNXOBJS = linux/getifstats.o linux/iptcrdr.o linux/libiptc_ipv4/libip4tc.o

ifeq ($(CONFIG_APP_MINIIGD_IPV6),y)
LNXOBJS += linux/iptpinhole.o linux/libiptc_ipv6/libip6tc.o
endif
 

#LIBS = -liptc

TESTUPNPDESCGENOBJS = testupnpdescgen.o upnpdescgen.o

#EXECUTABLES = miniigd testupnpdescgen
EXECUTABLES = miniigd

.PHONY:	all clean install depend

all:  BUILT_TIME need_modify $(EXECUTABLES)
#	mips-uclibc-strip miniigd
#all:
#	echo "++++++build nothing here: miniigd+++++++++"

need_modify:    
	@echo "" > ./.diff_users_config;
 
	@if [ -e .users_config ]; then \
		diff .users_config $(DIR_USERS)/.config > .diff_users_config; \
		if [ -s .diff_users_config ]; then \
			cp $(DIR_USERS)/.config .users_config; \
			make    clean; \
		fi; \
	else \
		cp $(DIR_USERS)/.config ./.users_config; \
	fi;

BUILT_TIME:
	@echo  \#define BUILT_TIME \"`TZ=UTC date -u "+%Y.%m.%d-%H:%M%z" `\" > ./built_time	

clean:
	$(RM) $(BASEOBJS) $(LNXOBJS) $(EXECUTABLES) testupnpdescgen.o

install:	miniigd
	@echo no install for now...

miniigd: $(BASEOBJS) $(LNXOBJS) $(LIBS)
#miniigd: $(BASEOBJS) $(LNXOBJS)

testupnpdescgen:	$(TESTUPNPDESCGENOBJS)

depend:
	makedepend -f$(MAKEFILE_LIST) -Y \
	$(BASEOBJS:.o=.c) $(LNXOBJS:.o=.c) $(TESTUPNPDESCGENOBJS:.o=.c) \
	2>/dev/null

# DO NOT DELETE

miniigd.o: upnpglobalvars.h upnphttp.h upnpdescgen.h miniupnpdpath.h upnputils.h
miniigd.o: getifaddr.h daemonize.h upnpsoap.h
upnphttp.o: upnphttp.h upnpdescgen.h miniupnpdpath.h upnpsoap.h
upnpdescgen.o: upnpdescgen.h miniupnpdpath.h upnpglobalvars.h
upnpsoap.o: upnpglobalvars.h upnphttp.h upnpsoap.h upnpreplyparse.h
upnpsoap.o: upnpredirect.h openbsd/getifstats.h
upnpreplyparse.o: upnpreplyparse.h minixml.h
minixml.o: minixml.h
upnpredirect.o: upnpredirect.h upnpglobalvars.h openbsd/obsdrdr.h
daemonize.o: daemonize.h
upnpglobalvars.o: upnpglobalvars.h
linux/getifstats.o: openbsd/getifstats.h
linux/iptcrdr.o: linux/iptcrdr.h
linux/libiptc_ipv4/libip4tc.o: linux/libiptc_ipv4/libiptc.h linux/libiptc_ipv4/libiptc.c
testupnpdescgen.o: upnpdescgen.h
upnpdescgen.o: upnpdescgen.h miniupnpdpath.h upnpglobalvars.h
upnpevents.o: upnpevents.h miniupnpdpath.h upnpglobalvars.h upnpdescgen.h
upnputils.o: upnputils.h
getifaddr.o: getifaddr.h ifaddrs.h
linux/libiptc_ipv6/libip6tc.o: linux/libiptc_ipv6/libip6tc.h linux/libiptc_ipv6/libiptc.c
upnppinhole.o: upnppinhole.h upnpredirect.h upnpglobalvars.h upnpevents.h linux/iptpinhole.h
linux/iptpinhole.o: linux/iptpinhole.h  upnpglobalvars.h
romfs:
	$(ROMFSINST) /bin/$(EXECUTABLES)
	$(ROMFSINST) -s /var/linuxigd /etc/
	@mkdir -p $(DIR_ROMFS)/etc/tmp
	@cp ./picsdesc.* $(DIR_ROMFS)/etc/tmp
ifeq ($(CONFIG_APP_MINIIGD_IPV6),y)
	@cp ./picsdesc6.* $(DIR_ROMFS)/etc/tmp	
endif

