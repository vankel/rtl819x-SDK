# $Id: Makefile.in,v 1.59.2.15 2005/02/22 03:02:40 jnelson Exp $

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
.PHONY: clean mrclean distclean all dist depend

# The following gcc warning switches are too noisy to be useful checking
# Boa for lint:
#  -Wtraditional -Wconversion -Wredundant-decls -Wunreachable-code
# The following gcc warning switches should generate no warnings:
GCC_FLAGS = -Wstrict-prototypes -Wpointer-arith -Wcast-align -Wcast-qual \
  -Wshadow -Waggregate-return -Wmissing-prototypes -Wnested-externs \
  -Wall -W -Wno-unused -Winline -Wwrite-strings -Wundef -pedantic


#srcdir = .
#VPATH = ./../extras
LDFLAGS =  -g
#LIBS = 
CFLAGS = -Os -pipe
CPPFLAGS =  -I. -I.
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
REALTEK_FILES = fmtcpip.c fmfwall.c utility.c fmget.c fmwlan.c fmmgmt.c version.c
ifeq ($(GW),1)
REALTEK_FILES += fmddns.c fmroute.c fmipv6.c fmbt.c fmpocketap.c
endif

ifeq ($(CONFIG_HTTP_FILE_SERVER_SUPPORT),y)
REALTEK_FILES += http_files.c
#REALTEK_FILES += fmhttp_files.c
REALTEK_FILES += http_file_dir.c
endif

ifeq ($(CONFIG_DOMAIN_NAME_QUERY_SUPPORT),y)
REALTEK_FILES += fmdomain_query.c
endif

ifeq ($(CONFIG_RTL_P2P_SUPPORT),y)
REALTEK_FILES += fmdomain_query.c
endif

ifeq ($(CONFIG_RTL_ULINKER),y)
REALTEK_FILES += fmdomain_query.c fmulinker.c
endif

ifeq ($(CONFIG_APP_TR069),y)
REALTEK_FILES += fmtr069.c
endif

ifeq ($(CONFIG_IPV6_CE_ROUTER_SUPPORT),y)
REALTEK_FILES += sha1.c
endif

ifeq ($(CONFIG_APP_RTK_VOIP),y)
include $(VOIP_APP)/web/voip_var.mak
REALTEK_FILES += $(VOIP_FILES)
CFLAGS += $(VOIP_CFLAGS)
LDFLAGS += $(VOIP_LDFLAGS)
CFLAGS += -DCONFIG_APP_BOA
endif

ifeq ($(CONFIG_RTL_8812AR_VN_SUPPORT),y)
        COMMON_CFLAGS += -DCONFIG_RTL_8812AR_VN_SUPPORT
endif

ifeq ($(CONFIG_RTL_ETH_802DOT1X_SUPPORT),y)
REALTEK_FILES += fmethdot1x.c
endif
#------------------------------------------------------------------------------
# CFLAGS
#------------------------------------------------------------------------------
DEBUG	= -g -Wall
IFLAGS 	= -I. -I$(APMIB_DIR)
CFLAGS += $(COMMON_CFLAGS) $(EXTRA_CFLAGS)
#include $(DIR_USERS)/.config
ifdef	CONFIG_APP_MULTIPPPOE
		CFLAGS += -D MULTI_PPPOE
endif

ifdef	CONFIG_APP_BOA_IPV6_SUPPORT
CFLAGS +=-DINET6
endif

ifdef CONFIG_APP_BOA_WITH_SSL
SSL_PATH = ./../../openssl-0.9.8i
CFLAGS += -DSERVER_SSL=1 -I$(SSL_PATH)/include/
SSL_LIBS = -lssl -lcrypto
endif

SOURCES = alias.c boa.c buffer.c cgi.c cgi_header.c config.c escape.c \
	get.c hash.c ip.c log.c mmap_cache.c pipe.c queue.c range.c \
	read.c request.c response.c signals.c util.c sublog.c \
	select.c \
	asp_page.c \
	$(REALTEK_FILES)

OBJS = $(SOURCES:.c=.o) timestamp.o 
ifeq ($(CONFIG_HTTP_FILE_SERVER_SUPPORT),y)
OBJS += fmhttp_files.o 
endif
#------------------------------------------------------------------------------
# targets
#------------------------------------------------------------------------------
all: fileserver depend boa release

#boa:	$(OBJS) 
#	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS) 
#	
#boa_indexer:	index_dir.o escape.o   
#	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

boa: Makefile $(OBJS) $(APMIB_DIR)/$(APMIB_LIB)
ifdef CONFIG_APP_BOA_WITH_SSL
	$(CC) -o boa $(OBJS) $(LDFLAGS) -L$(APMIB_DIR) -lapmib -L $(SSL_PATH) $(SSL_LIBS) -ldl
else
	$(CC) -o boa $(OBJS) $(LDFLAGS) -L$(APMIB_DIR) -lapmib
endif
	$(STRIP) $@

clean:
	rm -f $(DEPEND)
	rm -f $(OBJS) boa core *~ boa_indexer
	
distclean:	mrclean

mrclean:	clean
	rm -f config.status config.cache config.h Makefile config.log

# timestamp

timestamp.o:	$(SOURCES)

#------------------------------------------------------------------------------
# depend stuff
#------------------------------------------------------------------------------
#depend: $(SOURCES)
#	$(CPP) $(CPPFLAGS) -MM $^ select.c poll.c access.c > $(DEPEND)
fileserver:
ifeq ($(CONFIG_HTTP_FILE_SERVER_SUPPORT),y)
ifeq ($(CONFIG_RTL_8196E),y)
	if [ ! -e fmhttp_files.o ]; then \
		cp fmhttp_files_4.6.4-4181.obj fmhttp_files.o;\
	fi
endif
ifeq ($(CONFIG_RTL_8198C_GW),y)
	 if [ ! -e fmhttp_files.o ]; then \
                cp fmhttp_files_msdk-4.4.7.obj fmhttp_files.o;\
         fi
endif
endif
depend: $(SOURCES)
	if [ ! -e $(DEPEND) ]; then \
		$(CPP) $(DEBUG) $(CFLAGS) $(IFLAGS) -MM $^ > $(DEPEND); \
	fi
	
-include $(DEPEND)


# tags
tags:	$(SOURCES)
	ctags -o tags $^ *.h

release:
	./mksvnver DIR_USERS=$(DIR_USERS) DIR_LINUX=$(DIR_LINUX)


# object dump
boa.objdump:    boa
	objdump --disassemble-all --source boa > $@

.c.o:
	${CC} -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
