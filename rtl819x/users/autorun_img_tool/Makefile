ifeq ($(wildcard $(DIR_LINUX)/.config),)
DIR_LINUX_KERNEL = $(DIR_ROOT)/linux-2.6.30
else
DIR_LINUX_KERNEL = $(DIR_LINUX)
endif

CC=gcc
DEBUG = -g
CFLAGS += -Os -Wall
CFLAGS += $(EXTRA_CFLAGS) -I $(DIR_LINUX_KERNEL)/include
LDFLAGS += -s

EXEC = gen_autorun_img
OBJS = gen_autorun_img.o
all:  $(EXEC)

$(EXEC): $(OBJS)

$(OBJS): gen_autorun_img.c \
	$(DIR_LINUX_KERNEL)/drivers/usb/gadget_cathy/usb_ulinker.h \
	$(DIR_LINUX_KERNEL)/include/linux/autoconf.h	

romfs:
	-./gen_autorun_img
	$(ROMFSINST) autorun.img /etc/autorun.img

clean:
	-rm -f $(EXEC) *.o Setup.ini
	-rm -fr files mount_img
