
obj-y	:= 	vp886_adaptive_ringing.o 	\
			vp886_calibration_common.o 	\
			vp886_common.o 	\
			vp886_control.o	\
			vp886_events.o	\
			vp886_init.o	\
			vp886_query.o	\
			vp886_seq.o	\
			vp886_slac.o	\
			vp886_timers.o

EXTRA_CFLAGS += -I$(DIR_VOIP)/include/
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_drivers/zarlink/$(ZARLINK_LIB)/vp886_api/
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_drivers/zarlink/$(ZARLINK_LIB)/includes/
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_drivers/zarlink/$(ZARLINK_LIB)/arch/rtl89xxb/

ifeq "$(CONFIG_RTK_VOIP_SLIC_ZARLINK_890_SERIES)" "y"
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_drivers/zarlink/$(ZARLINK_LIB)/vp890_api/
endif

include $(DIR_VOIP)/rules.mak

