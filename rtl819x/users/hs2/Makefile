#CC=rsdk-linux-gcc
CFLAGS := -Os $(CFLAGS)
LDFLAGS = -s -Wall

ifeq ($(SLINK),1)
CFLAGS  += -ffunction-sections -fdata-sections
LDFLAGS += --static -s -Wl,--gc-sections
endif

all:		hs2

clean:
			rm -f *.o
			rm -f hs2
romfs:
	$(ROMFSINST) hs2 /bin/hs2
	$(ROMFSINST) hs2_wlan0.conf /etc/hs2_wlan0.conf
	$(ROMFSINST) hs2_wlan1.conf /etc/hs2_wlan1.conf
	$(ROMFSINST) icon_orange_eng.png /etc/icon_orange_eng.png
	$(ROMFSINST) icon_orange_zxx.png /etc/icon_orange_zxx.png
	$(ROMFSINST) icon_red_eng.png /etc/icon_red_eng.png
	$(ROMFSINST) icon_red_zxx.png /etc/icon_red_zxx.png
	$(ROMFSINST) wifi-abgn-logo_270x73.png /etc/wifi-abgn-logo_270x73.png

hs2:		hs2.c hs2_utility.c hs2.h
			$(CC) $(CFLAGS) -o hs2 hs2.c hs2_utility.c
