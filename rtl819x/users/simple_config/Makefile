include $(DIR_LINUX)/.config

include $(DIR_USERS)/.config

CFLAGS += -Os
TARGET=simple_config
OBJS = simple_config.o 1x_md5c.o 

ifdef CONFIG_APP_WSC
CFLAGS += -DCONFIG_APP_WSC
endif
ifdef CONFIG_RTL_SIMPLE_CONFIG_USE_WPS_BUTTON
CFLAGS += -DCONFIG_RTL_SIMPLE_CONFIG_USE_WPS_BUTTON
endif

all: $(TARGET) 

$(TARGET) : $(OBJS)
	$(CC) -o $@ $(OBJS)

romfs:
	$(ROMFSINST) /bin/$(TARGET)

clean:
	-rm -f $(TARGET) *.elf *.o
