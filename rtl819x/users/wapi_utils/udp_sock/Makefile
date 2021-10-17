all:
	@echo "builded"
clean:
	@echo "It's cleaned"

romfs:
	cp aeUdpClient_$(TOOLCHAIN) aeUdpClient
	$(ROMFSINST) /bin/aeUdpClient
ifdef CONFIG_RTL_WAPI_SUPPORT
ifdef CONFIG_RTL_WAPI_LOCAL_AS_SUPPORT
	cp aseUdpServer_$(TOOLCHAIN) aseUdpServer
	$(ROMFSINST) /bin/aseUdpServer
endif
endif

