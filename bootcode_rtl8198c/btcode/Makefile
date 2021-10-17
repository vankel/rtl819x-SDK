##/*-------------------------------------------------------------------
##		Created by REALTEK  
##--------------------------------------------------------------------*/

#CROSS= rsdk-elf-
include ../.config
#CROSS= mips-linux-

CC	= $(CROSS)gcc

ifeq ($(CONFIG_LZMA_ENABLE),y)
EXTRA_CFLAGS += -DLZMA_COMPRESS -D_LZMA_PROB32
endif


ifeq ($(CONFIG_POST_ENABLE),y)
CFLAGS += -DCONFIG_POST_ENABLE
endif

ifeq ($(CONFIG_BOOT_DEBUG_ENABLE),y)
CFLAGS += -DCONFIG_BOOT_DEBUG_ENABLE
endif


ifeq ($(RTL8196),1)
EXTRA_CFLAGS += -DRTL8196=1
endif

ifeq ($(CONFIG_RTL8196D),y)
MARCH_FLAG = -march=5281
endif

ifeq ($(CONFIG_RTL8196E),y)
MARCH_FLAG = -march=4181
endif

#ifeq ($(CONFIG_RTL8881A),y)
#MARCH_FLAG = -march=5281
#endif

ifeq ($(CONFIG_RTL8881A),y)
MARCH_FLAG = -march=mips32r2
endif


ifeq ($(CONFIG_FPGA_PLATFORM),1)
EXTRA_CFLAGS += -DCONFIG_FPGA_PLATFORM=1
endif
#=============================================================================
ifeq ($(CONFIG_LZMA_ENABLE),y)
bootload:	bootload.c start.S LzmaDecode.c piggy.S
	sync;sync;sync
	$(CC) $(MARCH_FLAG) -g  -I../boot/include -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c piggy.S -o piggy.o 
	$(CC) $(MARCH_FLAG) -g  -fomit-frame-pointer -nostdinc -fno-pic -mno-abicalls  -D__KERNEL__ $(EXTRA_CFLAGS)  -c bootload.c -o bootload.o
	$(CC) $(MARCH_FLAG) -g  -I../boot/include -fno-inline -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c LzmaDecode.c -o LzmaDecode.o
	
	./lzma e boot.img boot.img.gz
	
	$(CROSS)objcopy --add-section .initrd=boot.img.gz piggy.o
ifeq ($(CONFIG_NAND_FLASH_BOOTING),y)
	$(CROSS)ld  -X -nostdlib -Tpiggy_NAND.script  --static piggy.o bootload.o LzmaDecode.o -o piggy.elf
else
	$(CROSS)ld  -X -nostdlib -Tpiggy.script  --static piggy.o bootload.o LzmaDecode.o -o piggy.elf
endif
	
	$(CROSS)objcopy  -Obinary piggy.elf piggy.bin
	$(CROSS)objdump -h -S piggy.elf > piggy.elf.txt
endif
#==================================================================================
ifeq ($(CONFIG_GZIP_ENABLE),y)
bootload:	bootload.c start.S gunzip.c piggy.S
	sync;sync;sync
	$(CC) $(MARCH_FLAG) -g  -I../boot/include -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c piggy.S -o piggy.o 
	$(CC) $(MARCH_FLAG) -g  -fomit-frame-pointer -nostdinc -fno-pic -mno-abicalls  -D__KERNEL__ $(EXTRA_CFLAGS)  -c bootload.c -o bootload.o
	$(CC) $(MARCH_FLAG) -g  -I../boot/include -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c gunzip.c -o gunzip.o	
		
	cat boot.img | gzip -9 > boot.img.gz
	
	$(CROSS)objcopy --add-section .initrd=boot.img.gz piggy.o
ifeq ($(CONFIG_NAND_FLASH_BOOTING),y)
	$(CROSS)ld  -X -nostdlib -Tpiggy_NAND.script  --static piggy.o bootload.o gunzip.o -o piggy.elf
else
	$(CROSS)ld  -X -nostdlib -Tpiggy.script  --static piggy.o bootload.o gunzip.o -o piggy.elf
endif

	$(CROSS)objcopy  -Obinary piggy.elf piggy.bin
	$(CROSS)objdump -h -S piggy.elf > piggy.elf.txt
endif
#==================================================================================
ifeq ($(CONFIG_NONO_COMPRESS),y)
bootload:	bootload.c start.S
endif
#------------------------------------------------------------------------------------

#------------------------------------------------------------------------------------
	$(CC) $(MARCH_FLAG) -I../boot/include -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c start.S -o start.o 

ifeq ($(CONFIG_NAND_FLASH_BOOTING),y)
		
else
	$(CC) $(MARCH_FLAG) -g  -fomit-frame-pointer -nostdinc -fno-pic -mno-abicalls -D__KERNEL__ $(EXTRA_CFLAGS) -c start_c.c -o start_c.o 
endif
	
	
ifeq ($(CONFIG_NONO_COMPRESS),y)
	$(CROSS)objcopy --add-section .initrd=boot.img start.o	# Compress
endif	
ifeq ($(CONFIG_LZMA_ENABLE),y)
	$(CROSS)objcopy --add-section .initrd=piggy.bin start.o	# Compress
endif
ifeq ($(CONFIG_GZIP_ENABLE),y)
	$(CROSS)objcopy --add-section .initrd=piggy.bin start.o	# Compress
endif



ifeq ($(CONFIG_NAND_FLASH_BOOTING),y)
	$(CROSS)ld -X -nostdlib -Tld.script start.o -o boot.elf	
else
	$(CROSS)ld -X -nostdlib -Tld.script start.o start_c.o -o boot.elf
endif
#------------------------------------------------------------------------------------	
	$(CROSS)objcopy -Obinary boot.elf boot
	cp boot ../boot/Output/boot -f 			#sync 
	$(CROSS)objdump -h -S   boot.elf > boot.elf.txt 
	$(CROSS)nm boot.elf  | sort > system.map


	./cvimg boot boot boot.bin 0 0

#	rm -rf rescue.bin
#	./cvimg boot piggy.bin rescue.bin 0 0	
	hexdump -v -e '4/1 "%02X"' -e '",\r\n"' boot > romcode.txt  #wei add

	cp boot.bin ../boot/Output/boot.bin -f 			#copy file to Output for easy use.
#==================================================================================
clean:
	rm -f boot boot.elf *.txt system.map piggy.bin piggy.elf piggy.o nfjrom.elf nfjrom start_c.o
	
ifeq ($(CONFIG_LZMA_ENABLE),y)
	rm -f LzmaDecode.o boot.img.gz *.img
endif
ifeq ($(CONFIG_GZIP_ENABLE),y)
	rm -f gunzip.o boot.img.gz *.img
endif	 
	rm -f bootload.o start.o boot.bin  boot.img.gz *.img 

nfjrom: bootload
	$(CROSS)ld -X -nostdlib -Tnfjrom.script start.o -o nfjrom.elf
	$(CROSS)objcopy -Obinary nfjrom.elf nfjrom
