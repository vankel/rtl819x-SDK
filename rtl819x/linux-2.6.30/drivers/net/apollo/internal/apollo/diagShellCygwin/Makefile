#
# Creating a executable file including cleshell and 8370s ASIC driver under x86 CYGWIN platform
#


TARG := diag

CC  := gcc

include ../../../Compiler_Flag


LIB = build/ut/libut.a
LIB += build/sdk/librtk.a
DRV_TESTCASE_DIR := ..
API_SRC_DIR := ../../../sdk/src/rtk
APOLLO_DAL_DIR := ../../../sdk/src/dal
APOLLO_HAL_DIR := ../../../sdk/src/hal
APOLLO_COMMON_DIR := ../../../sdk/src/common
APOLLO_SYSTEM_DIR := ../../../sdk/system/



ifeq ($(CONFIG_DIAGSHELL_MODE),CONFIG_DIAGSHELL_RTK)
DIAGSHELL_DIR := ../../../sdk/src/app/diag_rtk
endif

ifeq ($(CONFIG_DIAGSHELL_MODE),CONFIG_DIAGSHELL_RAW)
DIAGSHELL_DIR := ../../../sdk/src/app/diag
endif

DIAG_INCLUDE  = $(DIAGSHELL_DIR)/include
DIAGSHELL_SRC_DIR := $(DIAGSHELL_DIR)/src



SDK_INCLUDE   = ../../../sdk/include
SYS_INCLUDE   = ../../../sdk/system/include
UT_INCLUDE   = ../../../sdk/unittest

DRV_TESTCASE_DIR := ..
API_SRC_DIR := ../../../sdk/src/rtk
APOLLO_DAL_DIR := ../../../sdk/src/dal
APOLLO_HAL_DIR := ../../../sdk/src/hal
APOLLO_COMMON_DIR := ../../../sdk/src/common
APOLLO_SYSTEM_DIR := ../../../sdk/system/



CFLAGS += -I$(SDK_INCLUDE) \
          -I$(SYS_INCLUDE) \
          -I$(SDK_INCLUDE)/hal/chipdef/apollo \
          -I$(UT_INCLUDE) \
          -I..

CFLAGS += -I$(DIAG_INCLUDE)


CFLAGS +=     -Wcomment \
              -Winit-self \
              -Wmissing-braces \
              -Wparentheses \
              -Wreturn-type \
              -Wunused-value \
              -Wunused-label \
              -Werror \
              -g

DIAGSHELL_OBJS := \
        $(DIAGSHELL_SRC_DIR)/diag_main.o \
	$(DIAGSHELL_SRC_DIR)/diag_util.o \
        $(DIAGSHELL_SRC_DIR)/diag_str.o \
	$(DIAGSHELL_SRC_DIR)/diag_om.o

ifeq ($(CONFIG_DIAGSHELL_MODE),CONFIG_DIAGSHELL_RAW)
ifeq ($(CONFIG_SDK_APOLLO),y)
DIAGSHELL_OBJS += \
        $(DIAGSHELL_SRC_DIR)/parser/cparser.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_fsm.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_io_unix.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_line.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_token.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_token_tbl.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_tree.o \
	$(DIAGSHELL_SRC_DIR)/diag_cmd.o \
        $(DIAGSHELL_SRC_DIR)/diag_debug.o \
        $(DIAGSHELL_SRC_DIR)/diag_sdk.o \
        $(DIAGSHELL_SRC_DIR)/diag_reg.o \
        $(DIAGSHELL_SRC_DIR)/diag_l2.o \
        $(DIAGSHELL_SRC_DIR)/diag_vlan.o \
        $(DIAGSHELL_SRC_DIR)/diag_svlan.o \
        $(DIAGSHELL_SRC_DIR)/diag_mirror.o \
        $(DIAGSHELL_SRC_DIR)/diag_port.o \
        $(DIAGSHELL_SRC_DIR)/diag_qos.o \
        $(DIAGSHELL_SRC_DIR)/diag_dot1x.o \
        $(DIAGSHELL_SRC_DIR)/diag_trunk.o \
        $(DIAGSHELL_SRC_DIR)/diag_rate.o \
        $(DIAGSHELL_SRC_DIR)/diag_oam.o \
        $(DIAGSHELL_SRC_DIR)/diag_trap.o \
        $(DIAGSHELL_SRC_DIR)/diag_counter.o \
        $(DIAGSHELL_SRC_DIR)/diag_acl.o \
        $(DIAGSHELL_SRC_DIR)/diag_ext.o \
        $(DIAGSHELL_SRC_DIR)/diag_gpon.o \
        $(DIAGSHELL_SRC_DIR)/diag_classf.o \
        $(DIAGSHELL_SRC_DIR)/diag_l34.o \
        $(DIAGSHELL_SRC_DIR)/diag_sec.o \
        $(DIAGSHELL_SRC_DIR)/diag_stp.o \
        $(DIAGSHELL_SRC_DIR)/diag_switch.o \
        $(DIAGSHELL_SRC_DIR)/diag_flowctrl.o \
        $(DIAGSHELL_SRC_DIR)/diag_auto_fallback.o \
        $(DIAGSHELL_SRC_DIR)/diag_eee.o \
        $(DIAGSHELL_SRC_DIR)/diag_cpu.o \
        $(DIAGSHELL_SRC_DIR)/diag_rldp.o \
        $(DIAGSHELL_SRC_DIR)/diag_intr.o \
        $(DIAGSHELL_SRC_DIR)/diag_ponmac.o
endif
endif


ifeq ($(CONFIG_DIAGSHELL_MODE),CONFIG_DIAGSHELL_RTK)
DIAGSHELL_OBJS += \
        $(DIAGSHELL_SRC_DIR)/parser/cparser.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_fsm.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_io_unix.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_line.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_token.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_token_tbl.o \
        $(DIAGSHELL_SRC_DIR)/parser/cparser_tree.o \
        $(DIAGSHELL_SRC_DIR)/diag_cmd.o \
	$(DIAGSHELL_SRC_DIR)/diag_sdk.o \
	$(DIAGSHELL_SRC_DIR)/diag_reg.o \
        $(DIAGSHELL_SRC_DIR)/diag_acl.o \
        $(DIAGSHELL_SRC_DIR)/diag_vlan.o \
        $(DIAGSHELL_SRC_DIR)/diag_l2.o \
        $(DIAGSHELL_SRC_DIR)/diag_igmp.o \
        $(DIAGSHELL_SRC_DIR)/diag_port.o \
        $(DIAGSHELL_SRC_DIR)/diag_mirror.o \
        $(DIAGSHELL_SRC_DIR)/diag_classf.o \
        $(DIAGSHELL_SRC_DIR)/diag_l34.o \
        $(DIAGSHELL_SRC_DIR)/diag_qos.o \
        $(DIAGSHELL_SRC_DIR)/diag_bandwidth.o \
        $(DIAGSHELL_SRC_DIR)/diag_meter.o \
        $(DIAGSHELL_SRC_DIR)/diag_switch.o \
        $(DIAGSHELL_SRC_DIR)/diag_storm_control.o \
        $(DIAGSHELL_SRC_DIR)/diag_dot1x.o \
        $(DIAGSHELL_SRC_DIR)/diag_stp.o \
        $(DIAGSHELL_SRC_DIR)/diag_counter.o \
        $(DIAGSHELL_SRC_DIR)/diag_trunk.o \
        $(DIAGSHELL_SRC_DIR)/diag_trap.o \
        $(DIAGSHELL_SRC_DIR)/diag_rma.o \
        $(DIAGSHELL_SRC_DIR)/diag_oam.o \
        $(DIAGSHELL_SRC_DIR)/diag_sec.o \
        $(DIAGSHELL_SRC_DIR)/diag_cpu.o \
	$(DIAGSHELL_SRC_DIR)/diag_iol.o \
	$(DIAGSHELL_SRC_DIR)/diag_svlan.o \
	$(DIAGSHELL_SRC_DIR)/diag_debug.o \
	$(DIAGSHELL_SRC_DIR)/diag_auto_fallback.o \
	$(DIAGSHELL_SRC_DIR)/diag_epon.o \
	$(DIAGSHELL_SRC_DIR)/diag_rldp.o \
	$(DIAGSHELL_SRC_DIR)/diag_ponmac.o \
	$(DIAGSHELL_SRC_DIR)/diag_flowctrl.o \
	$(DIAGSHELL_SRC_DIR)/diag_intr.o \
	$(DIAGSHELL_SRC_DIR)/diag_gpon.o
endif

ALL_OBJ := $(DIAGSHELL_OBJS)



.PHONY: all clean sdk unittest shell_tree

sdk:
	$(MAKE) -C build/sdk

unittest:
	$(MAKE) -C build/ut


shell_tree:
	cd $(DIAGSHELL_DIR)/config; \
	./script/mk_parser.py $(DEF) diag_shell.cli -fd ../include/flag/ -f cmd_flag.h; \
	mv ../config/cparser_tree.c ../src/parser/cparser_tree.c; \
	mv ../config/cparser_tree.h ../include/parser/cparser_tree.h; \
	cd - ;

all: clean $(TARG)



$(TARG): $(ALL_OBJ) $(LIB)
	$(CC) -o exe/$(TARG).exe $(ALL_OBJ) $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<
	$(MAKE) -C build/sdk
	$(MAKE) -C build/ut




clean:
	rm -rf exe/$(TARG) $(ALL_OBJ)
	$(MAKE) -C build/ut clean
	$(MAKE) -C build/sdk clean

%.o: %.c
	$(CC) $(CFLAGS) $(EXTRA_CFLAGS) -c -o $@ $<
	$(MAKE) -C build/sdk
	$(MAKE) -C build/ut
