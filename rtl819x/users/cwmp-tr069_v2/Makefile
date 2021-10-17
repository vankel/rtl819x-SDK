
.EXPORT_ALL_VARIABLES:

CWMP_ROOT=$(shell pwd)
GSOAP_DIR=gsoaplib
LIBCWMP_DIR=libcwmp
MATRIXSSL_DIR=matrixssl-1-8-3-open
LIBOPENSSL_INC_DIR=$(CWMP_ROOT)/../openssl-0.9.8b/include
LIBMATRIXSSL_INC_DIR=$(CWMP_ROOT)/$(MATRIXSSL_DIR)

ifeq ($(CONFIG_APP_GOAHEAD), y)
MIB_INCLUDE_DIR=-I$(CWMP_ROOT)/../goahead-2.1.1/LINUX
else
MIB_INCLUDE_DIR=-I$(CWMP_ROOT)/../boa/apmib
endif

LIB_SUFFIX=default

CC += -Os

all:
	if [ -d $(MATRIXSSL_DIR) ]; then \
		$(MAKE) -C $(MATRIXSSL_DIR)/src; \
		if [ ! -d lib ]; then mkdir lib; fi; \
		cp -f $(MATRIXSSL_DIR)/src/libmatrixsslstatic.a $(CWMP_ROOT)/lib/; \
	fi
	$(MAKE) -C matrixssl-1-8-3-open/examples
	if [ -d $(GSOAP_DIR) ]; then \
		cd $(GSOAP_DIR); \
	env CC=mips-linux-gcc STRIP=mips-linux-strip \
	CPPFLAGS='-DLINUX -DCONFIG_APP_TR069 -I$(LIBOPENSSL_INC_DIR) -I$(LIBMATRIXSSL_INC_DIR) $(TR_104_FLAG) $(TR_143_FLAG) $(MIB_INCLUDE_DIR)' ./configure --host=mips-linux --prefix=$(CWMP_ROOT); \
		$(MAKE); \
		$(MAKE) install; \
	fi
	if [ -d $(LIBCWMP_DIR) ]; then \
		cd $(LIBCWMP_DIR); \
		$(MAKE); \
		$(MAKE) install; \
	fi
# To support TR104
ifeq ($(CONFIG_APP_TR104),y)
	$(MAKE) -C $(CWMP_ROOT)/../rtk_voip/tr104/
	$(MAKE) -C $(CWMP_ROOT)/../rtk_voip/voip_manager/
endif
	if [ -d lib_$(LIB_SUFFIX) ]; then \
		rm lib; \
		ln -s lib_$(LIB_SUFFIX) lib; \
	fi
	$(MAKE) -C cwmpClient

romfs:
	cp -f $(CWMP_ROOT)/cwmpClient/cwmpClient $(CWMP_ROOT)/../../romfs/bin
	cp -f $(CWMP_ROOT)/cwmpClient/DefaultCwmpNotify.txt $(CWMP_ROOT)/../../romfs/etc
	$(ROMFSINST) -s /var/cwmp_default /etc/cwmp_default
	$(ROMFSINST) -s /var/cwmp_config /etc/cwmp_config 
ifeq ($(CONFIG_USER_TR143),y)
	cp -f $(CWMP_ROOT)/cwmpClient/udpechoserver $(CWMP_ROOT)/../../romfs/bin
endif
ifeq ($(CONFIG_USER_CWMP_WITH_SSL),y)
	cp -f $(CWMP_ROOT)/cwmpClient/client.pem $(CWMP_ROOT)/../../romfs/etc
	cp -f $(CWMP_ROOT)/cwmpClient/cacert.pem $(CWMP_ROOT)/../../romfs/etc
endif

clean:
	if [ -d $(MATRIXSSL_DIR) ]; then \
		$(MAKE) -C $(MATRIXSSL_DIR)/src clean; \
	fi
	if [ -d $(GSOAP_DIR) ]; then \
		$(MAKE) -C $(GSOAP_DIR) clean; \
	fi
	if [ -d $(LIBCWMP_DIR) ]; then \
		$(MAKE) -C $(LIBCWMP_DIR) clean; \
	fi
	
	$(MAKE) -C cwmpClient clean	
	if [ -d $(LIBCWMP_DIR) -a -d include ]; then \
		rm -rf include; \
		rm -rf lib; \
	fi

