ifeq ($(wildcard $(DIR_USERS)/.config),)
include ../../.config
else
include $(DIR_USERS)/.config
endif

TARGET = librtk_adapter.so
SRCS = ../common/common_utility.c rtk_lan_adapter.c rtk_wan_adapter.c rtk_wifi_adapter.c rtk_misc_adapter.c rtk_other_adapter.c rtk_firewall_adapter.c rtk_disk_adapter.c rtk_wifi_drvmib.c# rtk_cgi_adapter.c rtk_other_adapter.c

OBJS = $(SRCS:.c=.o)

RTK_EVENTD=rtk_eventd
RTK_EVENTD_SRC=rtk_eventd.c
RTK_EVENTD_OBJ=$(RTK_EVENTD_SRC:.c=.o)
CFLAGES += -DHAVE_RTK_APMIB
#CFLAGES += -DHAVE_RTK_DRVMIB

ifeq ($(CONFIG_LIB_MXML),y)
COMMON_CFLAGS += -DLIB_MXML_SUPPORT
MXML_LIB = ../../mxml-2.8/libmxml.a
LDLIBS += $(MXML_LIB)
ifeq ($(CONFIG_CGI_CLIENT),y)
ifeq ($(CONFIG_APP_CURL),y)
SRCS+= rtk_cgi_adapter.c
endif
endif
else
MXML_LIB =
endif

#APMIB_SO = ../../boa/apmib/libapmib.so
CFLAGES += -DGET_DEVICE_BRAND_FROM_MAC
COMMON_CFLAGS += -DCONFIG_RTL_8198C
all: $(TARGET) $(RTK_EVENTD)

$(TARGET): $(OBJS) $(MXML_LIB)
	$(CC) -s -shared -o $@ $(OBJS) $(MXML_LIB)

$(RTK_EVENTD): $(RTK_EVENTD_OBJ)
	$(CC) -o $@ $(RTK_EVENTD_OBJ)

.c.o:
	$(CC) $(COMMON_CFLAGS) $(CFLAGES) -c -o $@ -fPIC $<	

clean:
	@rm -f $(OBJS) $(TARGET)
	@rm -f $(RTK_EVENTD) $(RTK_EVENTD_OBJ)

romfs:
	$(ROMFSINST) /lib/$(TARGET)
	$(ROMFSINST) /bin/$(RTK_EVENTD)
	cp ./device_mac_brand.txt $(DIR_ROMFS)/etc
#	@echo "need to do with rtk adapter"
