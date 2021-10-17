
include ../../linux-2.6.30/.config
DIR_ROMFS = ../../target/romfs
ROMFSINST = ../../config/romfsinst
-include .config

export PATH := ../../toolchain/rsdk-1.3.6-4181-EB-2.6.30-0.9.30/bin:$(PATH)

CC = rsdk-linux-gcc
CFLAGS := -s -Wall -Os $(CFLAGS)
LDFLAGS = -s -Wall
CFLAGS += -I../libpcap-1.4.0
LIBS = ../libpcap-1.4.0/libpcap.a

ifeq ($(SLINK),1)
CFLAGS  += -ffunction-sections -fdata-sections
LDFLAGS += --static -s -Wl,--gc-sections
endif

all:	pcap

pcap:	packet_capture.c 
	$(CC) -static -o pcap packet_capture.c $(LIBS)	

clean:
	rm -f *.o
	rm -f pcap

romfs:
	cp pcap ../../romfs/bin
#$(ROMFSINST) pcap /bin
	
