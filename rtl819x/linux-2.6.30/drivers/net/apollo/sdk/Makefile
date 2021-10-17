#
# Makefile for the Linux Wireless network device drivers.
#
SDKDIR = $(DIR_LINUX)/drivers/net/apollo/sdk
SDK_RTCOREDIR = $(SDKDIR)/system/linux/rtcore
SDK_RTKDIR    = $(SDKDIR)/src
SDK_UTDIR     = $(SDKDIR)/unittest
SDK_INCLUDE   = $(SDKDIR)/include
SYS_INCLUDE   = $(SDKDIR)/system/include
DRV_INCLUDE   = $(SDKDIR)/system/linux
UT_INCLUDE    = $(SDKDIR)/unittest


CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE=y


export SDKDIR SDK_INCLUDE SYS_INCLUDE DRV_INCLUDE UT_INCLUDE
export CONFIG_SDK_KERNEL_LINUX_KERNEL_MODE

obj-y += src/
obj-y += unittest/
obj-y += system/linux/rtcore/
obj-y += system/linux/rtdrv/

clean:
	$(MAKE) -C $(SDK_RTKDIR) clean ; \
	$(MAKE) -C $(SDK_RTCOREDIR) clean ; \
	$(MAKE) -C $(SDK_UTDIR) clean ;




