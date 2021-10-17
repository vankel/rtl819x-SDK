ifeq ($(wildcard $(DIR_LINUX)/.config),)
include ../../../linux-2.6.30/.config
else
include $(DIR_LINUX)/.config
endif

ifeq ($(wildcard $(DIR_USERS)/.config),)
include ../../.config
else
include $(DIR_USERS)/.config
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

APMIB_SHARED = 1

#------------------------------------------------------------------------------
ifndef COMMON_CFLAGS
$(error COMMON_CFLAGS is not defined)
endif

ifndef APMIB_SHARED
$(error APMIB_SHARED is not defined)
endif

ifndef APMIB_LIB
$(error APMIB_LIB is not defined)
endif

#------------------------------------------------------------------------------
.SUFFIXES:
.SUFFIXES: .o .c
.PHONY: clean all depend

LDFLAGS =  -g
CFLAGS = -Os -pipe
DEPEND = ./.depend

#------------------------------------------------------------------------------
APMIB_DIR=../apmib

#------------------------------------------------------------------------------
# EXTRA_CFLAGS
#------------------------------------------------------------------------------
EXTRA_CFLAGS =

#------------------------------------------------------------------------------
# Files
#------------------------------------------------------------------------------
INCLUDE_SYSCONF = 1

ifeq ($(INCLUDE_SYSCONF),1)
SYSCONFIG = sysconf
TIMELYCHECK = timelycheck
INTERNET_PPP=ppp_inet
INTERNET_NTP=ntp_inet
INTERNET_DDNS=ddns_inet
ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
INTERNET_IPV6_MANAGE=ipv6_manage_inet
endif
else
SYSCONFIG =
TIMELYCHECK = 
INTERNET_PPP = 
INTERNET_NTP= 
INTERNET_DDNS=
ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
INTERNET_IPV6_MANAGE=
endif
endif

ifeq ($(INCLUDE_SYSCONF),1)
REALTEK_SYS_FILES = sysconf.c set_init.c set_bridge.c set_wlanapp.c sys_utility.c
ifeq ($(GW),1)
REALTEK_SYS_FILES += set_firewall.c set_wan.c
ifeq ($(CONFIG_IPV6), y)
REALTEK_SYS_FILES += set_ipv6.c
endif
endif
REALTEK_INTERNET_PPP_FILES = ppp_inet.c sys_utility.c
REALTEK_INTERNET_NTP_FILES = ntp_inet.c sys_utility.c
REALTEK_INTERNET_DDNS_FILES = ddns_inet.c sys_utility.c
ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
REALTEK_INTERNET_IPV6_MANAGE_FILES= ipv6_manage_inet.c sys_utility.c
endif
REALTEK_TIMELY_CHECK_FILES = timelycheck.c
REALTEK_TIMELY_CHECK_FILES += sys_utility.c set_init.c set_bridge.c set_wlanapp.c
ifeq ($(GW),1)
REALTEK_TIMELY_CHECK_FILES += set_firewall.c set_wan.c
ifeq ($(CONFIG_IPV6), y)
REALTEK_TIMELY_CHECK_FILES += set_ipv6.c
endif
endif
else
REALTEK_SYS_FILES =
REALTEK_INTERNET_PPP_FILES =
REALTEK_INTERNET_NTP_FILES =
REALTEK_INTERNET_DDNS_FILES =
REALTEK_TIMELY_CHECK_FILES =
ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
REALTEK_INTERNET_IPV6_MANAGE_FILES=
endif 
endif

ifeq ($(CONFIG_DOMAIN_NAME_QUERY_SUPPORT),y)
REALTEK_SYS_FILES += set_domain_query.c
REALTEK_TIMELY_CHECK_FILES += set_domain_query.c
endif

ifeq ($(CONFIG_RTL_P2P_SUPPORT),y)
REALTEK_SYS_FILES += set_domain_query.c
endif

ifeq ($(CONFIG_RTL_ULINKER),y)
REALTEK_SYS_FILES += set_ulinker.c
## for pppoe discover
REALTEK_SYS_FILES += discovery.c if.c
endif

ifeq ($(CONFIG_RTL_MESH_CROSSBAND),y)
CFLAGS += -DCONFIG_RTL_MESH_CROSSBAND
endif

ifeq ($(CONFIG_RTL_MESH_SUPPORT),y)
CFLAGS += -DCONFIG_RTK_MESH
endif

#------------------------------------------------------------------------------
# CFLAGS
#------------------------------------------------------------------------------
DEBUG	= -g -Wall
IFLAGS 	= -I. -I$(APMIB_DIR)
CFLAGS += $(COMMON_CFLAGS) $(EXTRA_CFLAGS)

#include $(DIR_USERS)/.config
ifdef CONFIG_APP_MULTIPPPOE
	CFLAGS += -D MULTI_PPPOE
endif
SOURCES = $(REALTEK_SYS_FILES) $(REALTEK_INTERNET_PPP_FILES) \
		$(REALTEK_INTERNET_NTP_FILES) $(REALTEK_INTERNET_DDNS_FILES)\
		$(REALTEK_TIMELY_CHECK_FILES)
ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
SOURCES += $(REALTEK_INTERNET_IPV6_MANAGE_FILES)
endif
REALTEK_SYS_OBJS = $(REALTEK_SYS_FILES:.c=.o)
REALTEK_INTERNET_PPP_OBJS = $(REALTEK_INTERNET_PPP_FILES:.c=.o)
REALTEK_INTERNET_NTP_OBJS = $(REALTEK_INTERNET_NTP_FILES:.c=.o)
REALTEK_INTERNET_DDNS_OBJS = $(REALTEK_INTERNET_DDNS_FILES:.c=.o)
ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
REALTEK_INTERNET_IPV6_MANAGE_OBJS = $(REALTEK_INTERNET_IPV6_MANAGE_FILES)
endif
REALTEK_TIMELY_CHECK_OBJS = $(REALTEK_TIMELY_CHECK_FILES:.c=.o)

ifeq ($(CONFIG_APP_RTK_VOIP),y)
LDFLAGS += -lpthread
endif

#------------------------------------------------------------------------------
# targets
#------------------------------------------------------------------------------
TARGET = $(SYSCONFIG) $(TIMELYCHECK)
ifeq ($(GW),1)
TARGET += $(INTERNET_PPP) $(INTERNET_NTP) $(INTERNET_DDNS)
endif
ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
TARGET += $(INTERNET_IPV6_MANAGE)
endif
all: depend $(TARGET)

$(SYSCONFIG): Makefile $(REALTEK_SYS_OBJS) $(APMIB_DIR)/$(APMIB_LIB)
	$(CC) -o $@ $(REALTEK_SYS_OBJS) $(LDFLAGS) -L$(APMIB_DIR) -lapmib
	$(STRIP) $@

$(INTERNET_PPP): Makefile $(REALTEK_INTERNET_PPP_OBJS)	$(APMIB_DIR)/$(APMIB_LIB)
	$(CC) -o $@ $(REALTEK_INTERNET_PPP_OBJS) $(LDFLAGS) -L$(APMIB_DIR) -lapmib
	$(STRIP) $@

$(INTERNET_NTP): Makefile $(REALTEK_INTERNET_NTP_OBJS)
	$(CC) -o $@ $(REALTEK_INTERNET_NTP_OBJS) $(LDFLAGS)
	$(STRIP) $@

$(INTERNET_DDNS): Makefile $(REALTEK_INTERNET_DDNS_OBJS)
	$(CC) -o $@ $(REALTEK_INTERNET_DDNS_OBJS) $(LDFLAGS)
	$(STRIP) $@

$(INTERNET_IPV6_MANAGE): Makefile $(REALTEK_INTERNET_IPV6_MANAGE_OBJS) $(APMIB_DIR)/$(APMIB_LIB)
	$(CC) -c -o ipv6_manage_inet.o $(DEBUG) $(CFLAGS) -DCONFIG_IPV6_CE_ROUTER_SUPPORT $(IFLAGS) -I$(DIR_LINUX)/include ipv6_manage_inet.c 
	$(CC) -o $@ ipv6_manage_inet.o sys_utility.o $(LDFLAGS)  -L$(APMIB_DIR) -lapmib
	$(STRIP) $@

$(TIMELYCHECK): Makefile $(REALTEK_TIMELY_CHECK_OBJS) $(APMIB_DIR)/$(APMIB_LIB)
	$(CC) -o $@ $(REALTEK_TIMELY_CHECK_OBJS) $(LDFLAGS) -L$(APMIB_DIR) -lapmib
	$(STRIP) $@

clean:
	rm -f $(DEPEND)
	rm -f *.o $(SYSCONFIG) $(INTERNET_PPP) $(INTERNET_NTP) $(INTERNET_DDNS) $(INTERNET_IPV6_MANAGE) $(TIMELYCHECK)

depend: $(SOURCES)
	if [ ! -e $(DEPEND) ]; then \
		$(CPP) $(DEBUG) $(CFLAGS) $(IFLAGS) -MM $^ > $(DEPEND); \
	fi

-include $(DEPEND)

.c.o:
	${CC} -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
