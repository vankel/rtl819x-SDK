all:
	@echo "builded"
clean:
	@echo "It's cleaned"

romfs:
ifdef CONFIG_RTL_WAPI_SUPPORT
ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
	cp readFileSize_$(TOOLCHAIN) readFileSize
	$(ROMFSINST) /bin/readFileSize
endif
endif
	@echo "Go here."
