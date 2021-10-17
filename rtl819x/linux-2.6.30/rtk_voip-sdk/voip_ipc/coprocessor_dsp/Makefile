obj-y = script/ coprocessor_dsp_tx.o coprocessor_dsp_rx.o
ifeq ($(CONFIG_RTK_VOIP_IPC_ARCH_IS_DSP),y)
obj-y += aipc_hook.o
endif

EXTRA_CFLAGS += -I$(DIR_VOIP)/include
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_ipc/
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_ipc/coprocessor_dsp/include/
#EXTRA_CFLAGS += -I$(ROOTDIR)/$(LINUXDIR)/drivers/char/aipc/include
EXTRA_CFLAGS += -D_AIPC_DSP_

ifeq ($(CONFIG_RTK_VOIP),m)
EXTRA_CFLAGS += -mlong-calls
EXTRA_CFLAGS += -DMODULE
endif

EXTRA_CFLAGS += -DMODULE_NAME=\"IPC\"

include $(DIR_VOIP)/rules.mak
ifeq ($(CONFIG_RTK_VOIP_IPC_ARCH_IS_DSP),y)
ifneq ($(CONFIG_RTK_VOIP_IPC_ARCH_DSP_IN_DUAL_LINUX),y)
include $(APP_DIR)/.config
endif
endif

ifeq ($(CONFIG_RTL8686_IPC_IDX_USE_DRAM),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_IPC_IDX_USE_DRAM
else
ifeq ($(CONFIG_RTL8686_IPC_IDX_USE_SRAM),y)
EXTRA_CFLAGS += -DCONFIG_RTL8686_IPC_IDX_USE_SRAM
endif
endif
