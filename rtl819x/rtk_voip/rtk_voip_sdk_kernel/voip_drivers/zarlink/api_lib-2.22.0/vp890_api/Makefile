
obj-y	:= 	vp890_apiCal.o  	\
			vp890_common.o  	\
			vp890_config.o  	\
			vp890_control.o  	\
			vp890_fxo_control.o	\
			vp890_fxs_control.o	\
			vp890_lp_control.o	\
			vp890_seq.o	\
			vp890_init.o  		\
			vp890_query.o


EXTRA_CFLAGS += -I$(DIR_VOIP)/include/
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_drivers/zarlink/$(ZARLINK_LIB)/vp890_api/
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_drivers/zarlink/$(ZARLINK_LIB)/includes/
EXTRA_CFLAGS += -I$(DIR_VOIP)/voip_drivers/zarlink/$(ZARLINK_LIB)/arch/rtl89xxb/

include $(DIR_VOIP)/rules.mak
