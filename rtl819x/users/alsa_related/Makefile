
ifeq ($(DIR_LINUX),)
include ../../linux-2.6.30/.config
else
include $(DIR_LINUX)/.config
endif
CURRENT_DIR=$(shell pwd)
RSDK_LIB_DIR=$(shell  echo $(PATH) | cut -f1 -d:)
SUBDIR_ALSA_LIB := alsa-lib-1.0.25
SUBDIR_ALSA_UTILS := alsa-utils-1.0.25

SUB_EXECS = 
ALSA_LIB = alsa_lib

all:$(ALSA_LIB)
	if [ ! -e ./$(SUBDIR_ALSA_UTILS)/Makefile ]; then \
		cd ./$(SUBDIR_ALSA_UTILS); \
		LD="mips-linux-ld" CC="$(CROSS_COMPILE)gcc -fPIC" ./configure --disable-xmlto --build=i686-pc-linux-gnu --host=mips-linux --disable-alsamixer  --with-alsa-prefix=$(RSDK_LIB_DIR)/../lib  --with-alsa-inc-prefix=$(RSDK_LIB_DIR)/../include/alsa; \
		touch ./configure; \
		touch ./config.status; \
		touch ./Makefile.in; \
		touch ./po/Makefile.in; \
		touch ./po/Makefile.in.in; \
		touch ./po/Makefile; \
		touch ./po/POTFILES.in; \
		touch ./alsaconf/po/Makefile.in; \
		touch ./alsaconf/po/Makefile; \
		touch ./alsaconf/alsaconf.in; \
		touch ./include/aconfig.h.in; \
	fi
	make -C $(SUBDIR_ALSA_UTILS)	
	
	
alsa_lib:
	if [ ! -e ./$(SUBDIR_ALSA_LIB)/Makefile ]; then \
		cd ./$(SUBDIR_ALSA_LIB); \
		LD="mips-linux-ld" CC="$(CROSS_COMPILE)gcc -fPIC" CFLAGS="-Os" ./configure --build=i686-pc-linux-gnu --with-debug --host=mips-linux --enable-static=no --enable-shared=yes  --with-versioned=no --with-debug=yes; \
		touch ./configure; \
		touch ./config.status; \
		touch ./Makefile.in; \
		touch ./Makefile; \
		touch ./include/config.h.in; \
	fi
	make -C $(SUBDIR_ALSA_LIB)
	rm -rf $(RSDK_LIB_DIR)/../include/alsa
	mkdir $(RSDK_LIB_DIR)/../include/alsa
	cp -fp ./$(SUBDIR_ALSA_LIB)/include/use-case.h $(RSDK_LIB_DIR)/../include/alsa/use-case.h
	cp -fp ./$(SUBDIR_ALSA_LIB)/include/control_external.h $(RSDK_LIB_DIR)/../include/alsa/control_external.h 
	cp -fp ./$(SUBDIR_ALSA_LIB)/include/pcm_external.h $(RSDK_LIB_DIR)/../include/alsa/pcm_external.h 
	cp -fp ./$(SUBDIR_ALSA_LIB)/include/pcm_extplug.h $(RSDK_LIB_DIR)/../include/alsa/pcm_extplug.h
	cp -fp ./$(SUBDIR_ALSA_LIB)/include/pcm_ioplug.h $(RSDK_LIB_DIR)/../include/alsa/pcm_ioplug.h
	cp -fp ./$(SUBDIR_ALSA_LIB)/include/pcm_rate.h $(RSDK_LIB_DIR)/../include/alsa/pcm_rate.h
	cp -fp ./$(SUBDIR_ALSA_LIB)/include/asoundlib.h $(RSDK_LIB_DIR)/../include/alsa/asoundlib.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/asoundef.h $(RSDK_LIB_DIR)/../include/alsa/asoundef.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/version.h $(RSDK_LIB_DIR)/../include/alsa/version.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/global.h $(RSDK_LIB_DIR)/../include/alsa/global.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/input.h $(RSDK_LIB_DIR)/../include/alsa/input.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/output.h $(RSDK_LIB_DIR)/../include/alsa/output.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/error.h $(RSDK_LIB_DIR)/../include/alsa/error.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/conf.h $(RSDK_LIB_DIR)/../include/alsa/conf.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/pcm.h $(RSDK_LIB_DIR)/../include/alsa/pcm.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/rawmidi.h $(RSDK_LIB_DIR)/../include/alsa/rawmidi.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/timer.h $(RSDK_LIB_DIR)/../include/alsa/timer.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/hwdep.h $(RSDK_LIB_DIR)/../include/alsa/hwdep.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/control.h $(RSDK_LIB_DIR)/../include/alsa/control.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/mixer.h $(RSDK_LIB_DIR)/../include/alsa/mixer.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/seq_event.h $(RSDK_LIB_DIR)/../include/alsa/seq_event.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/seq.h $(RSDK_LIB_DIR)/../include/alsa/seq.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/seqmid.h $(RSDK_LIB_DIR)/../include/alsa/seqmid.h
	cp -p ./$(SUBDIR_ALSA_LIB)/include/seq_midi_event.h $(RSDK_LIB_DIR)/../include/alsa/seq_midi_event.h
	rm -f $(RSDK_LIB_DIR)/../lib/libasound.a
	#cp -p ./$(SUBDIR_ALSA_LIB)/src/.libs/libasound.a $(RSDK_LIB_DIR)/../lib/libasound.a
	
	rm -f $(RSDK_LIB_DIR)/../lib/libasound.so
	rm -f $(RSDK_LIB_DIR)/../lib/libasound.so.2
	rm -f $(RSDK_LIB_DIR)/../lib/libasound.so.2.0.0
	cp -p ./$(SUBDIR_ALSA_LIB)/src/.libs/libasound.so.2.0.0 $(RSDK_LIB_DIR)/../lib/libasound.so.2.0.0
	ln -s libasound.so.2.0.0 $(RSDK_LIB_DIR)/../lib/libasound.so.2
	ln -s libasound.so.2.0.0 $(RSDK_LIB_DIR)/../lib/libasound.so	
	
	
romfs:
	$(ROMFSINST) ./$(SUBDIR_ALSA_UTILS)/alsaconf/alsaconf /bin/alsaconf
	$(ROMFSINST) ./$(SUBDIR_ALSA_UTILS)/alsactl/alsactl /bin/alsactl
	$(ROMFSINST) ./$(SUBDIR_ALSA_UTILS)/aplay/aplay /bin/aplay
	
	$(ROMFSINST) -s /bin/aplay /bin/arecord
	$(ROMFSINST) ./audio_src_16k_le.wav /bin/audio_src_16k_le.wav
	#$(ROMFSINST) ./audio_src_16k_be.pcm /bin/audio_src_16k_be.pcm
	#$(ROMFSINST) ./audio_src_48k_le.wav /bin/audio_src_48k_le.wav
	
	
	
	mkdir -p $(DIR_ROMFS)/usr/share/alsa
	mkdir -p $(DIR_ROMFS)/usr/share/alsa/cards
	mkdir -p $(DIR_ROMFS)/usr/share/alsa/pcm
	cp ./$(SUBDIR_ALSA_LIB)/src/conf/asound.conf $(DIR_ROMFS)/etc/asound.conf
	#cp ./$(SUBDIR_ALSA_LIB)/src/conf/asound.conf $(DIR_ROMFS)/home/root/.asoundrc
	cp ./$(SUBDIR_ALSA_LIB)/src/conf/alsa.conf $(DIR_ROMFS)/usr/share/alsa/alsa.conf
	cp ./$(SUBDIR_ALSA_LIB)/src/conf/cards/aliases.conf $(DIR_ROMFS)/usr/share/alsa/cards/aliases.conf
	cp ./$(SUBDIR_ALSA_LIB)/src/conf/pcm/default.conf $(DIR_ROMFS)/usr/share/alsa/pcm/default.conf
	cp ./$(SUBDIR_ALSA_LIB)/src/conf/pcm/dmix.conf $(DIR_ROMFS)/usr/share/alsa/pcm/dmix.conf
	cp ./$(SUBDIR_ALSA_LIB)/src/conf/pcm/dsnoop.conf $(DIR_ROMFS)/usr/share/alsa/pcm/dsnoop.conf

clean:
	make -C $(SUBDIR_ALSA_LIB) distclean
	make -C $(SUBDIR_ALSA_UTILS) distclean
	rm -f $(RSDK_LIB_DIR)/../lib/libasound.a
	rm -f $(RSDK_LIB_DIR)/../lib/libasound.so
	rm -f $(RSDK_LIB_DIR)/../lib/libasound.so.2
	rm -f $(RSDK_LIB_DIR)/../lib/libasound.so.2.0.0


