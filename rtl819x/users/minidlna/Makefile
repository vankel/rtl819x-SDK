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

-include $(ROOTDIR)/linux-2.6.30/include/linux/config.h

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
	cd $(ZLIB_DIR) && CC=$(DLNA_CROSS_COMPILER)gcc ./configure $(LIBZ_FLAG)
	$(MAKE) -C $(ZLIB_DIR)
	$(MAKE) -C $(ZLIB_DIR) install

jpeg:
	cd $(JPEG_DIR) && ./configure $(LIBJPEG_FLAG)
	$(MAKE) -C $(JPEG_DIR)
	$(MAKE) -C $(JPEG_DIR) install 

exif:
	cd $(EXIF_DIR) && ./configure $(LIBEXIF_FLAG)
	$(MAKE) -C $(EXIF_DIR)
	$(MAKE) -C $(EXIF_DIR) install

id3tag:
	cd $(ID3TAG_DIR) && ./configure $(ID3TAG_FLAG)
	$(MAKE) -C $(ID3TAG_DIR)
	$(MAKE) -C $(ID3TAG_DIR) install
	
ogg:
	cd $(OGG_DIR) && ./configure $(OGG_FLAG)
	$(MAKE) -C $(OGG_DIR)
	$(MAKE) -C $(OGG_DIR) install

flac:
	cd $(FFMPEG_DIR) && ./configure $(FLAC_FLAG)
	$(MAKE) -C $(FFMPEG_DIR)
	$(MAKE) -C $(FFMPEG_DIR) install

vorbis:
	cd $(VORBIS_DIR) && ./configure $(VORBIS_FLAG)
	$(MAKE) -C $(VORBIS_DIR)
	$(MAKE) -C $(VORBIS_DIR) install

sqlite:
	cd $(SQLITE_DIR) && ./configure $(SQLITE_FLAG)
	$(MAKE) -C $(SQLITE_DIR)
	$(MAKE) -C $(SQLITE_DIR) install

ffmpeg:
	cd $(FFMPEG_DIR) && ./configure $(FFMPEG_FLAG)
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
	-$(MAKE) -C $(JPEG_DIR) distclean
	-$(MAKE) -C $(EXIF_DIR) distclean
	-$(MAKE) -C $(ID3TAG_DIR) distclean
	-$(MAKE) -C $(OGG_DIR) distclean
	-$(MAKE) -C $(FFMPEG_DIR) distclean
	-$(MAKE) -C $(VORBIS_DIR) distclean
	-$(MAKE) -C $(SQLITE_DIR) distclean
	-$(MAKE) -C $(FFMPEG_DIR) distclean
	-$(MAKE) -C $(MINIDLNA_DIR) distclean
	$(RM) -r $(DLNA_INSTALL_PREFIX)/*
