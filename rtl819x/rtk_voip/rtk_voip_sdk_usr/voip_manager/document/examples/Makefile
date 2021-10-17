include ../../../.linux_config

CFLAGS += -g -I../../../includes
#LDFLAGS += --static
#LIBS = ../../../voip_manager/libvoip_manager.a
LDFLAGS += -L../../../voip_manager -lvoip_manager

ifdef CONFIG_RTK_VOIP_IP_PHONE
CFLAGS += -I../../../ui/include
endif

ifdef CONFIG_RTK_VOIP_DRIVERS_ATA_DECT
LDFLAGS += -lpthread -ldl
endif

CFLAGS += $(ECOS_SHELL_CFLAGS)

CFLAGS += -Werror

#----------------------------------------------------------------------
all:
	$(CC) ring.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o ring
	$(CC) phone.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o phone
	$(CC) rtp.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o rtp
	$(CC) rtp2.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o rtp2
ifdef CONFIG_RTK_VOIP_IPV6_SUPPORT
	$(CC) rtp_v6.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o rtp_v6
	$(CC) t38_v6.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o t38_v6
endif
	$(CC) daa.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o daa
	$(CC) caller_id.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o caller_id
	$(CC) fxo.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxo
	$(CC) fxo_int.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxo_int
	$(CC) conference.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o conference
	$(CC) custom_tone.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o custom_tone	
	$(CC) fxs.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxs
	$(CC) fxs_int.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxs_int
	$(CC) pulse_dial.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o pulse_dial
	$(CC) pcm_dump.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o pcm_dump
	$(CC) gain_test.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o gain_test
	$(CC) sip_reg_led.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o sip_reg_led
	$(CC) fxo_callerid_det.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fxo_callerid_det
	$(CC) dis_tone.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o dis_tone
	$(CC) dtmfdet_dump.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o dtmfdet_dump
	$(CC) send_2833.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o send_2833
	$(CC) proslic_set.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o proslic_set
	$(CC) fskgen.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o fskgen
	$(CC) vmwigen.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o vmwigen
	$(CC) led_ctrl.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o led_ctrl
	$(CC) ivr_text_play.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o ivr_text_play
	#$(CC) t38.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o t38
	$(CC) dtmf_det_cfg.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o dtmf_det_cfg
	$(CC) dtmf_det_test.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o dtmf_det_test
	$(CC) detbuf2_dump.c $(CFLAGS) $(LIBS) $(LDFLAGS) -o detbuf2_dump

ecos-shell:
	$(CC) rtp.c $(CFLAGS) -c -o rtp.o 

romfs:
	$(ROMFSINST) fxs /bin/fxs
	$(ROMFSINST) fxs_int /bin/fxs_int
	$(ROMFSINST) fxo /bin/fxo
	$(ROMFSINST) fxo_int /bin/fxo_int
	$(ROMFSINST) rtp /bin/rtp
	$(ROMFSINST) rtp2 /bin/rtp2
ifdef CONFIG_RTK_VOIP_IPV6_SUPPORT
	$(ROMFSINST) rtp_v6 /bin/rtp_v6
	$(ROMFSINST) t38_v6 /bin/t38_v6
endif
	$(ROMFSINST) pcm_dump /bin/pcm_dump
	$(ROMFSINST) caller_id /bin/caller_id
	$(ROMFSINST) dtmfdet_dump /bin/dtmfdet_dump
	$(ROMFSINST) send_2833 /bin/send_2833
	$(ROMFSINST) proslic_set /bin/proslic_set
	$(ROMFSINST) fskgen /bin/fskgen
	$(ROMFSINST) vmwigen /bin/vmwigen
	$(ROMFSINST) led_ctrl /bin/led_ctrl
	$(ROMFSINST) ivr_text_play /bin/ivr_text_play
	#$(ROMFSINST) t38 /bin/t38
	$(ROMFSINST) dtmf_det_cfg /bin/dtmf_det_cfg
	$(ROMFSINST) dtmf_det_test /bin/dtmf_det_test
	$(ROMFSINST) conference /bin/conference
	$(ROMFSINST) detbuf2_dump /bin/detbuf2_dump

clean:
	rm -f *.o ring phone rtp rtp2 rtp_v6 t38_v6 daa caller_id vmwi fxo fxo_int conference fxs fxs_int custom_tone pulse_dial pcm_dump gain_test sip_reg_led fxo_callerid_det dis_tone dtmfdet_dump send_2833 proslic_set fskgen vmwigen led_ctrl ivr_text_play t38 dtmf_det_cfg dtmf_det_test detbuf2_dump
