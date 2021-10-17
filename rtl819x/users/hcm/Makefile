#include $(DIR_LINUX)/.config

all : $(EXEC)
	make -C hcd
clean:
	make -C hcd clean

romfs:
	make -C hcd romfs
	$(ROMFSINST) script/ /bin
