# Source Makefile for ecmh - Jeroen Massar <jeroen@unfix.org>
#
# ECMH_VERSION needs to be defined, gets done by toplevel Makefile

BINS	= ecmh
SRCS	= ecmh.c linklist.c common.c interfaces.c groups.c grpint.c subscr.c
INCS	= ecmh.h linklist.h common.h interfaces.h groups.h grpint.h subscr.h mld.h
OBJS	= ecmh.o linklist.o common.o interfaces.o groups.o grpint.o subscr.o
FLAGS	= -W -Wall -Wno-unused -D_GNU_SOURCE -O9

ifeq ($(KERNEL_VER),linux-2.6.30)
CFLAGS += -DKERNEL_2_6_30
endif
ifeq ($(KERNEL_VER),linux-3.10)
CFLAGS += -DKERNEL_3_10
endif

all:	$(BINS)

ecmh:	$(OBJS) ${INCS}
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS);
	
romfs:
	$(ROMFSINST) /bin/ecmh

clean:
	$(RM) -f $(OBJS) $(BINS)

