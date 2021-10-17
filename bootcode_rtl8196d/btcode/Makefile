##/*-------------------------------------------------------------------
##		Created by REALTEK  
##--------------------------------------------------------------------*/

#CROSS= rsdk-elf-
include ../.config
CROSS= mips-linux-
CC	= $(CROSS)gcc

LD_SCRIPT=
ifeq ($(CONFIG_NAND_FLASH),y)
LD_SCRIPT=ld.nand_script
else
LD_SCRIPT=ld.script
endif

ifeq ($(CONFIG_LZMA_ENABLE),y)
LZMA= 1
else
LZMA= 0
endif
ifeq ($(LZMA),1)
EXTRA_CFLAGS += -DLZMA_COMPRESS -D_LZMA_PROB32
else

endif
ifeq ($(CONFIG_DEFAULTS_REALTEK_RTL8652_5G),y)
CFLAGS += -DRTL_8652_5PORT
endif

#ifeq ($(RTL_8652_5PORT),1)
#EXTRA_CFLAGS += -DRTL_8652_5PORT
#else
#endif


ifeq ($(CONFIG_POST_ENABLE),y)
CFLAGS += -DCONFIG_POST_ENABLE
endif

ifeq ($(CONFIG_BOOT_DEBUG_ENABLE),y)
CFLAGS += -DCONFIG_BOOT_DEBUG_ENABLE
endif


ifeq ($(RTL8196),1)
EXTRA_CFLAGS += -DRTL8196=1
endif


ifeq ($(CONFIG_RTL8196E),y)
MARCH_FLAG = -march=4181 -Os
else
MARCH_FLAG = -march=5281 -Os
endif


ifeq ($(CONFIG_FPGA_PLATFORM),1)
EXTRA_CFLAGS += -DCONFIG_FPGA_PLATFORM=1
endif

ifeq ($(LZMA),1)
bootload:	bootload.c start.S LzmaDecode.c piggy.S
	sync;sync;sync
	$(CC) $(MARCH_FLAG) -g  -I../boot/include -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c piggy.S -o piggy.o 
	$(CC) $(MARCH_FLAG) -g  -fomit-frame-pointer -nostdinc -fno-pic -mno-abicalls  -D__KERNEL__ $(EXTRA_CFLAGS)  -c bootload.c -o bootload.o
	#$(CC) -march=5181 -g  -I../boot/include -fno-inline -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c gunzip.c -o gunzip.o	
	$(CC) $(MARCH_FLAG) -g  -I../boot/include -fno-inline -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c LzmaDecode.c -o LzmaDecode.o
	#cat boot.img | gzip -9 > boot.img.gz
	./lzma e boot.img boot.img.gz
	$(CROSS)objcopy --add-section .initrd=boot.img.gz piggy.o
	$(CROSS)ld  -X -nostdlib -Tpiggy.script  --static piggy.o bootload.o LzmaDecode.o -o piggy.elf
	$(CROSS)objcopy  -Obinary piggy.elf piggy.bin
	$(CROSS)objdump -h -S piggy.elf > piggy.elf.txt
	$(CC) $(MARCH_FLAG) -I../boot/include -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c start.S -o start.o 
	$(CC) $(MARCH_FLAG) -g  -fomit-frame-pointer -nostdinc -fno-pic -mno-abicalls -D__KERNEL__ $(EXTRA_CFLAGS) -c start_c.c -o start_c.o 	
	$(CROSS)objcopy --add-section .initrd=piggy.bin start.o	# Compress
ifeq ($(CONFIG_NFBI),y)
	$(CROSS)ld -X -nostdlib -Tld-nfbi.script start.o -o boot.elf
else
ifeq ($(CONFIG_NAND_FLASH),y)
	$(CROSS)ld -X -nostdlib -T$(LD_SCRIPT) start.o -o boot.elf
else
	$(CROSS)ld -X -nostdlib -T$(LD_SCRIPT) start.o -o boot.elf
endif
endif
	$(CROSS)objcopy -Obinary boot.elf boot
	cp boot ../boot/Output/boot -f 			#sync 
	$(CROSS)objdump -h -S   boot.elf > boot.elf.txt 
	$(CROSS)nm boot.elf  | sort > system.map
ifeq ($(CONFIG_NFBI),y)
	cp boot boot.bin
else
	./cvimg boot boot boot.bin 0 0
endif
	rm -rf rescue.bin
	./cvimg boot piggy.bin rescue.bin 0 0
else
bootload:	bootload.c start.S gunzip.c piggy.S
	sync;sync;sync
	$(CC) $(MARCH_FLAG) -g  -I../boot/include -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c piggy.S -o piggy.o 
	$(CC) $(MARCH_FLAG) -g  -fomit-frame-pointer -nostdinc -fno-pic -O2 -mno-abicalls  -D__KERNEL__ $(EXTRA_CFLAGS)  -c bootload.c -o bootload.o
	$(CC) $(MARCH_FLAG) -g  -I../boot/include -fno-pic -mno-abicalls -O2 -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c gunzip.c -o gunzip.o	
		
	cat boot.img | gzip -9 > boot.img.gz
	$(CROSS)objcopy --add-section .initrd=boot.img.gz piggy.o
	$(CROSS)ld  -X -nostdlib -Tpiggy.script  --static piggy.o bootload.o gunzip.o -o piggy.elf
	$(CROSS)objcopy  -Obinary piggy.elf piggy.bin
	$(CROSS)objdump -h -S piggy.elf > piggy.elf.txt

	$(CC) $(MARCH_FLAG) -I../boot/include -fno-pic -mno-abicalls -Os -fomit-frame-pointer -D__KERNEL__ $(EXTRA_CFLAGS) -c start.S -o start.o 
ifeq ($(CONFIG_NAND_FLASH),y)
else

	$(CC) $(MARCH_FLAG) -g  -fomit-frame-pointer -nostdinc -fno-pic -mno-abicalls -D__KERNEL__ $(EXTRA_CFLAGS) -c start_c.c -o start_c.o 
endif
	$(CROSS)objcopy --add-section .initrd=piggy.bin start.o	# Compress
ifeq ($(CONFIG_NAND_FLASH),y)
	$(CROSS)ld -X -nostdlib -T$(LD_SCRIPT) start.o -o boot.elf #JSW:No start.c
else
	$(CROSS)ld -X -nostdlib -T$(LD_SCRIPT) start.o start_c.o -o boot.elf
endif	
	
	$(CROSS)objcopy -Obinary boot.elf boot
	cp boot ../boot/Output/boot -f 			#sync 
	$(CROSS)objdump -h -S   boot.elf > boot.elf.txt 
	$(CROSS)nm boot.elf  | sort > system.map
	./cvimg boot boot boot.bin 0 0
endif
	cp boot.bin ../boot/Output/boot.bin -f 			#copy file to Output for easy use.

clean:
	rm -f boot boot.elf *.txt system.map piggy.bin piggy.elf piggy.o nfjrom.elf nfjrom
ifeq ($(LZMA),1)
	rm -f bootload.o start.o boot.bin  LzmaDecode.o boot.img.gz *.img
else
	rm -f bootload.o start.o boot.bin  gunzip.o boot.img.gz *.img
endif	 


nfjrom: bootload
	$(CROSS)ld -X -nostdlib -Tnfjrom.script start.o -o nfjrom.elf
	$(CROSS)objcopy -Obinary nfjrom.elf nfjrom
