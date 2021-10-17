DLNA_INSTALL_PREFIX=$(shell pwd)/tmp
DLNA_ARCH=mips
DLNA_CROSS_COMPILER=$(CROSS_COMPILE)
LIBZ_FLAG=--prefix=$(DLNA_INSTALL_PREFIX) 
LIBJPEG_FLAG=--prefix=$(DLNA_INSTALL_PREFIX) CC=$(DLNA_CROSS_COMPILER)gcc --host=$(DLNA_ARCH)-linux --enable-static --disable-shared
LIBEXIF_FLAG=--prefix=$(DLNA_INSTALL_PREFIX) CC=$(DLNA_CROSS_COMPILER)gcc --host=$(DLNA_ARCH)-linux --enable-static --disable-shared
ID3TAG_FLAG=--prefix=$(DLNA_INSTALL_PREFIX) CC=$(DLNA_CROSS_COMPILER)gcc --host=$(DLNA_ARCH)-linux CPPFLAGS="-I$(DLNA_INSTALL_PREFIX)/include" LDFLAGS="-L$(DLNA_INSTALL_PREFIX)/lib" --enable-static --disable-shared --disable-debugging
OGG_FLAG=--prefix=$(DLNA_INSTALL_PREFIX) CC=$(DLNA_CROSS_COMPILER)gcc --host=$(DLNA_ARCH)-linux --enable-static --disable-shared
FLAC_FLAG=--prefix=$(DLNA_INSTALL_PREFIX) CC=$(DLNA_CROSS_COMPILER)gcc --host=$(DLNA_ARCH)-linux --disable-cpplibs --disable-ogg --disable-xmms-plugin --disable-oggtest --enable-static --disable-shared
VORBIS_FLAG=--prefix=$(DLNA_INSTALL_PREFIX) CC=$(DLNA_CROSS_COMPILER)gcc --host=$(DLNA_ARCH)-linux OGG_CFLAGS="-I$(DLNA_INSTALL_PREFIX)/include" OGG_LIBS="-L$(DLNA_INSTALL_PREFIX)/lib" --enable-static --disable-shared
SQLITE_FLAG=--prefix=$(DLNA_INSTALL_PREFIX) CC=$(DLNA_CROSS_COMPILER)gcc --host=$(DLNA_ARCH)-linux --enable-static --disable-shared 
#FFMPEG_FLAG=--cross-prefix=$(DLNA_CROSS_COMPILER) --target-os=linux --enable-cross-compile --arch=$(DLNA_ARCH) --prefix=$(DLNA_INSTALL_PREFIX) --enable-static --disable-shared  --enable-gpl --disable-mips32r2 --disable-mipsdspr1 --disable-mipsdspr2 --disable-network --disable-zlib --disable-ffmpeg --disable-ffserver --disable-ffplay --enable-small --disable-encoders --disable-decoders --disable-muxers --disable-bsfs --disable-devices --enable-debug --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp --disable-protocol=pipe --disable-protocol=http --disable-parser=cavsvideo --disable-parser=dca --disable-parser=dirac --disable-parser=dnxhd --disable-parser=mjpeg --disable-parser=mlp --disable-parser=pnm --disable-parser=vp3 --disable-demuxers --enable-demuxer=matroska --enable-decoder=rawvideo --enable-decoder=h264 \
			--enable-demuxer=mpegps --enable-demuxer=mpegts --enable-demuxer=mpegtsraw --enable-demuxer=mpegvideo \
			--enable-demuxer=m4v --enable-demuxer=mlp --enable-demuxer=mov --enable-demuxer=mp3 --enable-demuxer=mpc 			

FFMPEG_FLAG=--cross-prefix=$(DLNA_CROSS_COMPILER) --target-os=linux --enable-cross-compile --arch=$(DLNA_ARCH) --prefix=$(DLNA_INSTALL_PREFIX) --enable-static --disable-shared  --enable-gpl --disable-network --disable-zlib --disable-vhook --disable-ffmpeg --disable-ffserver --disable-ffplay --enable-small --disable-encoders --disable-decoders --disable-muxers --disable-bsfs --disable-devices --enable-debug --disable-protocol=udp --disable-protocol=tcp --disable-protocol=rtp --disable-protocol=pipe --disable-protocol=http --disable-parser=cavsvideo --disable-parser=dca --disable-parser=dirac --disable-parser=dnxhd --disable-parser=mjpeg --disable-parser=mlp --disable-parser=pnm --disable-parser=vp3 --disable-demuxers --enable-demuxer=matroska --enable-decoder=rawvideo --enable-decoder=h264 \
                        --enable-demuxer=mpegps --enable-demuxer=mpegts --enable-demuxer=mpegtsraw --enable-demuxer=mpegvideo \
                        --enable-demuxer=m4v --enable-demuxer=mlp --enable-demuxer=mov --enable-demuxer=mp3 --enable-demuxer=mpc               			
export DLNA_INSTALL_PREFIX
export DLNA_MAKE=$(MAKE)
export DLNA_INSTALL=$(INSTALL)
export ROOTDIR
export DLNA_LDFLAGS=$(LDFLAGS)
export DLNA_LNXOBJS=$(LNXOBJS)
export DLNA_CFLAGS=$(CFLAGS)
export CFLAGS
#export LDFLAGS+=-L$(ROOTDIR)/lib/zlib
export LDFLAGS+=-L$(DLNA_INSTALL_PREFIX)/lib

ifeq ($(wildcard $(DIR_LINUX)/.config),)
DIR_LINUX_KERNEL = $(DIR_ROOT)/linux-2.6.30
else
DIR_LINUX_KERNEL = $(DIR_LINUX)
endif

-include $(DIR_LINUX_KERNEL)/include/linux/config.h

#ifeq ($(CONFIG_APP_MINI_UPNP),y)
#export USE_MINI_UPNPD = 1
#export CFLAGS+=-DUSE_MINI_UPNPD
#endif
ZLIB_DIR=zlib-1.2.5
JPEG_DIR=jpeg-7
EXIF_DIR=libexif-0.6.21
ID3TAG_DIR=libid3tag-0.15.1b
SQLITE_DIR=sqlite-3.7.2-src
FFMPEG_DIR=ffmpeg-0.5.2
MINIDLNA_DIR=minidlna-1.0.25
OGG_DIR=libogg-1.3.0
VORBIS_DIR=libvorbis-1.3.3

#.PHONY: all jpeg exif id3tag ogg flac vorbis sqlite ffmpeg minidlna
.PHONY: all zlib jpeg exif id3tag sqlite ffmpeg minidlna
#all: jpeg exif id3tag ogg flac vorbis sqlite ffmpeg minidlna
all: zlib jpeg exif id3tag sqlite ffmpeg minidlna
#all: minidlna

romfs:
	$(ROMFSINST) ./tmp/sbin/minidlna /bin/minidlna

zlib:
	if [ ! -e ./$(ZLIB_DIR)/.generatedMakefile ]; then \
		cd ./$(ZLIB_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(LIBZ_FLAG) > .generatedMakefile;\
	fi
	$(MAKE) -C $(ZLIB_DIR)
	$(MAKE) -C $(ZLIB_DIR) install

jpeg:
	if [ ! -e ./$(JPEG_DIR)/.generatedMakefile ]; then \
		cd ./$(JPEG_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(LIBJPEG_FLAG) > .generatedMakefile;\
	fi
	$(MAKE) -C $(JPEG_DIR)
	$(MAKE) -C $(JPEG_DIR) install 

exif:
	if [ ! -e ./$(EXIF_DIR)/.generatedMakefile ]; then \
		cd ./$(EXIF_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(LIBEXIF_FLAG) > .generatedMakefile;\
	fi
	$(MAKE) -C $(EXIF_DIR)
	$(MAKE) -C $(EXIF_DIR) install

id3tag:
	if [ ! -e ./$(ID3TAG_DIR)/.generatedMakefile ]; then \
		cd ./$(ID3TAG_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(ID3TAG_FLAG) > .generatedMakefile;\
	fi
	-rm $(ID3TAG_DIR)/frametype.c $(ID3TAG_DIR)/compat.c $(ID3TAG_DIR)/genre.dat
	$(MAKE) -C $(ID3TAG_DIR)
	$(MAKE) -C $(ID3TAG_DIR) install
	
ogg:
	if [ ! -e ./$(OGG_DIR)/.generatedMakefile ]; then \
		cd ./$(OGG_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(OGG_FLAG) > .generatedMakefile;\
	fi
	$(MAKE) -C $(OGG_DIR)
	$(MAKE) -C $(OGG_DIR) install

flac:
	if [ ! -e ./$(FFMPEG_DIR)/.generatedMakefile ]; then \
		cd ./$(FFMPEG_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(FLAC_FLAG) > .generatedMakefile;\
	fi
	$(MAKE) -C $(FFMPEG_DIR)
	$(MAKE) -C $(FFMPEG_DIR) install

vorbis:
	if [ ! -e ./$(VORBIS_DIR)/.generatedMakefile ]; then \
		cd ./$(VORBIS_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(VORBIS_FLAG) > .generatedMakefile;\
	fi
	$(MAKE) -C $(VORBIS_DIR)
	$(MAKE) -C $(VORBIS_DIR) install

sqlite:
	if [ ! -e ./$(SQLITE_DIR)/.generatedMakefile ]; then \
		cd ./$(SQLITE_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(SQLITE_FLAG) > .generatedMakefile;\
	fi
	$(MAKE) -C $(SQLITE_DIR)
	$(MAKE) -C $(SQLITE_DIR) install

ffmpeg:
	if [ ! -e ./$(FFMPEG_DIR)/.generatedMakefile ]; then \
		cd ./$(FFMPEG_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(FFMPEG_FLAG) > .generatedMakefile;\
	fi
	$(MAKE) -C $(FFMPEG_DIR)
	$(MAKE) -C $(FFMPEG_DIR) install

minidlna:
	$(MAKE) -C $(MINIDLNA_DIR)
	$(MAKE) -C $(MINIDLNA_DIR) install
	$(DLNA_CROSS_COMPILER)strip $(DLNA_INSTALL_PREFIX)/sbin/minidlna
	
clean:
	-$(MAKE) -C $(ZLIB_DIR) clean
	-$(MAKE) -C $(JPEG_DIR) clean
	-$(MAKE) -C $(EXIF_DIR) clean
	-$(MAKE) -C $(ID3TAG_DIR) clean
	-$(MAKE) -C $(OGG_DIR) clean
	-$(MAKE) -C $(FFMPEG_DIR) clean
	-$(MAKE) -C $(VORBIS_DIR) clean
	-$(MAKE) -C $(SQLITE_DIR) clean
	-$(MAKE) -C $(FFMPEG_DIR) clean
	-$(MAKE) -C $(MINIDLNA_DIR) clean
	$(RM) -r $(DLNA_INSTALL_PREFIX)/*

distclean:
	-$(MAKE) -C $(ZLIB_DIR) distclean
	-rm $(ZLIB_DIR)/.generatedMakefile
	-$(MAKE) -C $(JPEG_DIR) distclean
	-rm $(JPEG_DIR)/.generatedMakefile
	-$(MAKE) -C $(EXIF_DIR) distclean
	-rm $(EXIF_DIR)/.generatedMakefile
	-$(MAKE) -C $(ID3TAG_DIR) distclean
	-rm $(ID3TAG_DIR)/.generatedMakefile
	-$(MAKE) -C $(OGG_DIR) distclean
	-rm $(OGG_DIR)/.generatedMakefile
	-$(MAKE) -C $(FFMPEG_DIR) distclean
	-rm $(FFMPEG_DIR)/.generatedMakefile
	-$(MAKE) -C $(VORBIS_DIR) distclean
	-rm $(VORBIS_DIR)/.generatedMakefile
	-$(MAKE) -C $(SQLITE_DIR) distclean
	-rm $(SQLITE_DIR)/.generatedMakefile
	-$(MAKE) -C $(FFMPEG_DIR) distclean
	-rm $(FFMPEG_DIR)/.generatedMakefile
	-$(MAKE) -C $(MINIDLNA_DIR) distclean
	$(RM) -r $(DLNA_INSTALL_PREFIX)/*
