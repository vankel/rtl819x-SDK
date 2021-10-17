CFLAGS =

include ../.config
include ../../.config

ifeq ($(CONFIG_RTL_8198C),y)
CFLAGS += -D CONFIG_RTL_8198C
endif

ifeq ($(CONFIG_APP_CPU_UTILIZATION_36K), y)
CFLAGS += -D CONFIG_CPU_UTILIZATION_36K
endif

ifeq ($(CONFIG_APP_CPU_UTILIZATION_72K), y)
CFLAGS += -D CONFIG_CPU_UTILIZATION_72K
endif

#CLESHELL = cleshell
CPU = cpu

#CC=mips-uclibc-gcc
#STRIP=mips-uclibc-strip

#SOURCES  = cleshell.c  

#SOURCES +=  cmdset.c        \
#            cmdset/topdir.c \
#            cmdset/help.c
SOURCES  = cpu.c  

#CFLAGS  += -I ./ -Wall -Werror -O2

OBJS = ${SOURCES:.c=.o}

all: $(CPU)

$(CPU): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS$(LDLIBS-$(@)))
	$(STRIP) $@

romfs:
	$(ROMFSINST) /bin/cpu
	
clean:
	-rm -f $(CPU) *.elf *.gdb $(OBJS)

