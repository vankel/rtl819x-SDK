all:
	@echo "builded"
clean:
	@echo "It's cleaned"

romfs:
	cp storeWapiFiles_$(TOOLCHAIN) storeWapiFiles
	$(ROMFSINST) /bin/storeWapiFiles
	cp loadWapiFiles_$(TOOLCHAIN) loadWapiFiles
	$(ROMFSINST) /bin/loadWapiFiles

