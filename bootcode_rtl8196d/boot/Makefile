##/*-------------------------------------------------------------------
##		Created by REALTEK  
##--------------------------------------------------------------------*/
include ../.config
CROSS = mips-linux-
#CROSS = rsdk-elf-

OUTDIR	= ./Output
CC	= $(CROSS)gcc
AS	= $(CROSS)as
AR	= $(CROSS)ar crs
LD	= $(CROSS)ld
OBJCOPY = $(CROSS)objcopy
#OBJCOPY = objcopy

OBJDUMP	= $(CROSS)objdump
IMG2BIN	= $(CROSS)img2bin
NM	= $(CROSS)nm
RM	= rm


TOOLCFLAGS	=
TOOLLDFLAGS	= -n

OPT	=  -G 0

TEXT	=

INCLUDES	= -I. -I./include

ifeq ($(CONFIG_RTL8196E),y)
CFLAGS	= -march=4181 -g  -fomit-frame-pointer -fno-pic -mno-abicalls $(TOOLCFLAGS) $(OPT) $(INCLUDES)\
          -D__KERNEL__\
          -Dlinux\
          -O 
else
CFLAGS	= -march=5281 -g  -fomit-frame-pointer -fno-pic -mno-abicalls $(TOOLCFLAGS) $(OPT) $(INCLUDES)\
          -D__KERNEL__\
          -Dlinux\
          -O 
endif

       
ifeq ($(CONFIG_DHCP_SERVER),y)
CFLAGS += -DDHCP_SERVER
endif
ifeq ($(CONFIG_HTTP_SERVER),y)
CFLAGS += -DHTTP_SERVER
endif

ifeq ($(CONFIG_BOOT_DEBUG_ENABLE),y)
CFLAGS += -DCONFIG_BOOT_DEBUG_ENABLE
endif

#------------------------------------------------------------------------------------------
ifneq "$(strip $(JUMP_ADDR))" ""
CFLAGS += -DJUMP_ADDR=$(JUMP_ADDR)
endif

ifeq ($(RTL865X),1)
CFLAGS += -DRTL865X=1 -DCONFIG_RTL865X=y -DCONFIG_RTL865XC=1 
CFLAGSW = $(CFLAGS) -DWRAPPER -DRTL865X
endif

ifeq ($(RTL8198),1)
CFLAGS += -DRTL8198=1 -DCONFIG_RTL865XC=1
CFLAGSW = $(CFLAGS) -DWRAPPER -DRTL8198
endif


#--------------------------------------
ifeq ($(CONFIG_DDR_SDRAM),y)
CFLAGS += -DDDR_SDRAM
endif
ifeq ($(CONFIG_DDR1_SDRAM),y)
CFLAGS += -DDDR1_SDRAM
endif
ifeq ($(CONFIG_DDR2_SDRAM),y)
CFLAGS += -DDDR2_SDRAM
endif
#--------------------------------------
ifeq ($(CONFIG_SW_8366GIGA),y)
CFLAGS += -DSW_8366GIGA
endif



#--------------------------------------

ifeq ($(CONFIG_SPI_STD_MODE),y)
CFLAGS += -DCONFIG_SPI_STD_MODE
endif

LDFLAGS = -nostdlib  -T./ld.script  -EB   --static
WLDFLAGS = -nostdlib  -T./ldw.script  -EB   --static
ASFLAGS	=  -D__ASSEMBLY__  -x assembler-with-cpp -G 0

CRT	=
LIBS	=

.SUFFIXES : .s .S .c .o .out .nm .img .sr .sre .text .bin .scr

all:
	@echo "No rules to make target. You should choose:"
	@echo "make { boot | wboot }  boot for general, wboot for wrapped boot code"

.PHONY: banner
banner:	cr-output
	@echo  \#define BOOT_CODE_TIME \"`date "+%Y.%m.%d-%H:%M%z"`\" > ./banner/mk_time	 	
	
cr-output:
	@if [ ! -d Output ]; then \
	mkdir Output; \
	fi

#wboot(wrapped) means boot code will be wrapped in this code
wboot: banner $(OUTDIR)/wboot.out
	$(NM)	./Output/wboot.out | sort > ./Output/wboot.nm
	$(OBJCOPY) -g -Obinary ./Output/wboot.out ./Output/wboot.img
	mips-linux-objdump -h -S ./Output/wboot.out > ./Output/wboot.text


#all :	"$(OUTDIR)" boot.out
boot: banner $(OUTDIR)/boot.out
	$(NM)	./Output/boot.out | sort > ./Output/boot.nm
	# $(OBJCOPY) -g -Obinary ./Output/boot.out ./Output/boot.img
	mips-linux-objcopy -g -Obinary ./Output/boot.out ./Output/boot.img
	mips-linux-objdump -h -S ./Output/boot.out > ./Output/boot.text
#david
	cp ./Output/boot.img ../btcode/boot.img -f

#"$(OUTDIR)" :

##Assembly file######################################################
ASMLIST =
ASMLIST += ./arch/mips/kernel/head.S
ASMLIST += ./init/inthandler.S 
	

ifeq ($(CONFIG_RTL8196D_TAROKO),y)
ASMLIST += ./monitor/rtl8196d_taroko/lexra_load_store.S
ASMLIST += ./monitor/rtl8196d_taroko/my_eyes.S
endif
###Assembly OBJ file, auto create ###################################

ASMOBJS = $(addprefix $(OUTDIR)/,$(addsuffix .o,$(notdir $(basename $(ASMLIST))) ))

$(ASMOBJS):$(ASMLIST)
	$(CC) -c $(CFLAGS) $(ASFLAGS) $(filter %$(addsuffix .S,$(notdir $(basename $@))),$(ASMLIST) ) -o $@

##C file#############################################################
SRCLIST = 
#SRCLIST += ./monitor/test_idmem.c
SRCLIST += ./init/irq.c 
SRCLIST += ./init/eth_tftpd.c 


#-----------------------------------------------------------------------------

SRCLIST += ./init/ethInt_865x.c 
SRCLIST += ./rtl8196x/swCore.c 
SRCLIST += ./rtl8196x/swNic_poll.c 
SRCLIST += ./rtl8196x/swTable.c 
SRCLIST += ./rtl8196x/vlanTable.c 
	
#----------------------------------------------------------------------------
ifdef CONFIG_SPI_FLASH
SRCLIST += ./flash/spi_flash.c
SRCLIST += ./flash/spi_common.c
endif

ifdef CONFIG_NOR_FLASH
SRCLIST += ./flash/flash.c
endif
#----------------------------------------------------------------------------
#ifdef CONFIG_PCIE_MODULE
#SRCLIST += ./monitor/test_8168n.c 
#endif
#--------------------------------------
ifeq ($(CONFIG_DHCP_SERVER),y)
SRCLIST += ./init/dhcpd.c
endif
#--------------------------------------
ifeq ($(CONFIG_HTTP_SERVER),y)
SRCLIST += ./init/utcp.c
endif
#--------------------------------------
ifeq ($(CONFIG_RTL8196D_TAROKO),y)
SRCLIST += ./monitor/rtl8196d_taroko/rtl8196d_cpu_test.c
SRCLIST += ./monitor/rtl8196d_taroko/idmem_dma_op.c
CFLAGS += -DCONFIG_RTL8196D_TAROKO
endif
ifeq ($(CONFIG_IIS_TEST),y)
SRCLIST += ./monitor/pcm/iis_isr.c
SRCLIST += ./monitor/pcm/ALC5621.c
SRCLIST += ./monitor/pcm/fpga_gpio.c
CFLAGS += -DCONFIG_IIS_TEST
endif

ifeq ($(CONFIG_PCM_TEST),y)
SRCLIST += ./pcm/pcm_isr.c
SRCLIST += ./pcm/iis_isr.c
SRCLIST += ./pcm/ALC5621.c			
SRCLIST += ./pcm/fpga_gpio.c	
endif

SRCLIST += ./arch/mips/kernel/setup.c	
SRCLIST += ./arch/mips/kernel/traps.c	

SRCLIST += ./init/main.c	
SRCLIST += ./init/calloc.c		


#SRCLIST += ./io/vsprintf.c 
SRCLIST += ./io/init.c 
SRCLIST += ./io/string.c 
SRCLIST += ./io/strtol.c 
SRCLIST += ./io/strtoul.c 
SRCLIST += ./io/ctool.c 
SRCLIST += ./io/misc.c 
#SRCLIST += ./io/ctype.c

SRCLIST += ./init/utility.c 
#SRCLIST += ./nand/nand.c 

ifdef CONFIG_RTL8196D	
#SRCLIST += ./monitor/test_lib.c 
#SRCLIST += ./monitor/test_hw_96d.c    
endif

ifdef CONFIG_RTL8196E_ULINKER_BOOT_LED
SRCLIST += ./monitor/power_on_led.c
endif

ifdef CONFIG_RTL8196E
SRCLIST += ./monitor/power_on_led.c
endif

###C OBJ file, auto create ###############################################
OBJS	= $(addprefix $(OUTDIR)/,$(addsuffix .o,$(notdir $(basename $(SRCLIST))) ))


define PROGRAM_template
$(addprefix $(OUTDIR)/,$(addsuffix .o,$(notdir $(basename $(1))) )) : $(1)	
#	@echo $(1)
#	@echo $(addprefix $(OUTDIR)/,$(addsuffix .o,$(notdir $(basename $(1))) ))
	$(CC) -c $(CFLAGS) $(1) -o $(addprefix $(OUTDIR)/,$(addsuffix .o,$(notdir $(basename $(1))) ))
endef

$(foreach fname,$(SRCLIST),$(eval $(call PROGRAM_template,$(fname))))

#-----------------------------------------------------------------------------
$(OUTDIR)/monitor.o: ./monitor/monitor.c 
	$(CC) -c $(CFLAGS) -o $(OUTDIR)/monitor.o ./monitor/monitor.c

$(OUTDIR)/wmonitor.o: ./monitor/monitor.c 
	$(CC) -c $(CFLAGSW) -o $(OUTDIR)/wmonitor.o ./monitor/monitor.c


$(OUTDIR)/boot.out :	  $(ASMOBJS) $(OBJS) $(OUTDIR)/monitor.o
	$(LD) $(LDFLAGS)  $(ASMOBJS) $(OBJS) $(OUTDIR)/monitor.o  $(LIBS) -o $(OUTDIR)/boot.out

$(OUTDIR)/temp.o: temp.c
	$(CC) -c $(CFLAGS) -o $(OUTDIR)/temp.o temp.c
	mips-linux-objcopy --add-section .btimg=$(OUTDIR)/boot $(OUTDIR)/temp.o

$(OUTDIR)/wboot.out :	   $(ASMOBJS) $(OBJS) $(OUTDIR)/temp.o $(OUTDIR)/wmonitor.o 
	$(LD) $(WLDFLAGS)  $(ASMOBJS) $(OBJS) $(OUTDIR)/temp.o $(OUTDIR)/wmonitor.o $(LIBS) -o $(OUTDIR)/wboot.out

#------------------------------------------------------------------------------------------------------	
clean :
	$(RM) -f $(OBJS)
	$(RM) -f $(OUTDIR)/boot.out $(OUTDIR)/boot.text $(OUTDIR)/boot.nm $(OUTDIR)/boot.img
	$(RM) -f $(OUTDIR)/boot.cdb $(OUTDIR)/monitor.o $(OUTDIR)/wmonitor.o $(OUTDIR)/wboot.*
	$(RM) -f $(OUTDIR)/temp.o $(OUTDIR)/boot
	@find $(OUTDIR) -type f -name '*.o' -exec rm -f {} \;
	
	
