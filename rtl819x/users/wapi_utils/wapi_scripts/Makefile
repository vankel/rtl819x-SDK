SCRIPTS = initCAFiles.sh genUserCert.sh revokeUserCert.sh

include $(DIR_LINUX)/.config

all :
	echo "wapi script"

romfs :
ifdef CONFIG_RTL_WAPI_SUPPORT
ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
	@cp $(SCRIPTS) $(DIR_ROMFS)/bin/
endif
endif
	@echo "Go here."

