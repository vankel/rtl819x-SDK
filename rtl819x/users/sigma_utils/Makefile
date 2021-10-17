ifeq ($(x86),1)
#
# x86 simulation for boa & flash
#
# usage: 
# 1. cd boa
# 2. make x86=1 (it will create x86 files, and copy to x86 dir)
# 3. cd x86 
# 
# * if APMIB_SHARED = 0 
# 1. ./flash default (it will create setting.bin to simulate flash)
# 2. ./boa -c . -r . (it will start boa, you may need run 'service httpd stop' first)
#
# * if APMIB_SHARED = 1
# 1. LD_LIBRARY_PATH=. ./flash default 
# 2. LD_LIBRARY_PATH=. ./boa -c . -r .
#
# Use browser to view html now!
#
export CC = gcc
export CPP = gcc -E
export AR = ar
export STRIP = strip
# uncomment it to disable strip behavior for gdb debug
#export STRIP = :
export DIR_USERS := $(shell pwd)/../
export DIR_LINUX := $(shell pwd)/../../linux-2.6.30
else
#export PATH := $(shell pwd)/../../$(CONFIG_RSDKDIR)/bin:$(PATH)
export CC = rsdk-linux-gcc
export CPP = rsdk-linux-gcc -E
export AR = rsdk-linux-ar
export STRIP = rsdk-linux-strip
endif
DEBUG	= -Wall -Os


all:sigmautil

sigmautil: Makefile sigmautil.o
	$(CC) -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $(LDFLAGS) sigmautil.o
	$(STRIP) $@

clean:
	rm -f sigmautil *.o 

romfs:
	$(ROMFSINST) sigmautil /bin/sigmautil
ifeq ($(CONFIG_SIGMA_HS2), y)
sigmautil.o: sigmautil_hs2.c
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
else ifeq ($(CONFIG_SIGMA_PMF), y)
sigmautil.o: sigmautil_pmf.c
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
else
sigmautil.o: sigmautil.c
	$(CC) -c -o $@ $(DEBUG) $(CFLAGS) $(IFLAGS) $<
endif
