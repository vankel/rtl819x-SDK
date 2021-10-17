include ../../.linux_config

TARGET		=	fwupdate
OBJS		=	fwupdate.o
#LIBS		=	../../flash/libvoip_flash_client.a
ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxE
LIBS		=	$(DIR_USERS)/rtk_voip-89xxE/flash/libvoip_flash_client.a
else
LIBS		=	$(DIR_USERS)/rtk_voip/flash/libvoip_flash_client.a
endif
DEPENDENCY	=	$(OBJS:.o=.d)
#LDFLAGS		=	-lpthread -ldl -L../../voip_manager/ -lvoip_manager 
ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxE
LDFLAGS		=	-lpthread -ldl -L$(DIR_USERS)/rtk_voip-89xxE/voip_manager/ -lvoip_manager 
else
LDFLAGS		=	-lpthread -ldl -L$(DIR_USERS)/rtk_voip/voip_manager/ -lvoip_manager 
endif
#CFLAGS		=	-I../../includes/
ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxE
CFLAGS		=   -I$(DIR_USERS)/rtk_voip-89xxE/includes/ 
else
CFLAGS		=   -I$(DIR_USERS)/rtk_voip/includes/ 
endif
CFLAGS		+=  -I$(DIR_USERS)/boa/apmib/

ifdef CONFIG_RTK_VOIP_IP_PHONE
#CFLAGS		+=	-I../../ui/include/
ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxE
CFLAGS 		+= -I$(DIR_USERS)/rtk_voip-89xxE/ui/include
else
CFLAGS 		+= -I$(DIR_USERS)/rtk_voip/ui/include
endif
endif

# 865x only
CFLAGS		+=	-I../../../../include/

CFLAGS		+=	-DVOIP_SUPPORT -DHOME_GATEWAY

ifneq ($(CONFIG_RTK_VOIP_PACKAGE_867X), y)
all: $(OBJS)
	$(CC) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS)
endif

clean:
	rm -f $(TARGET) $(OBJS) $(DEPENDENCY)

%.o: %.c
	$(CC) -c -o $@ $(CFLAGS) $<
	$(CC) -M $(CFLAGS) $< | sed -e "s#^$(*F).o[ :]*#$(@D)\/$(*F).o : #" > $(@:.o=.d)

-include $(DEPENDENCY)
