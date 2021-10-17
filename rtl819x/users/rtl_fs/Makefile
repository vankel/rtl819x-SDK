include $(DIR_LINUX)/.config

ifeq ($(CONFIG_RTL_8198C),y)
  MODULE_TARGET:=r245248
  COMPILER_NAME:=mips-linux
  COMPILER_TARGET="mips"
  CONFIGURE_ARGS=CFLAGS="-I$(DIR_LINUX)/arch/mips/include/asm/mach-realtek -I$(DIR_LINUX)/arch/mips/include -I$(DIR_LINUX)/arch/mips/include/generated -I$(DIR_LINUX)/include -I$(DIR_LINUX)/arch/mips/include/uapi -I$(DIR_LINUX)/arch/mips/include/generated/uapi -I$(DIR_LINUX)/include/uapi -I$(DIR_LINUX)/include/generated/uapi -DVMLINUX_LOAD_ADDRESS=0x80000000 -DDATAOFFSET=0 -fno-strict-aliasing -fno-common -fno-delete-null-pointer-checks -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -U_MIPS_ISA -D_MIPS_ISA=_MIPS_ISA_MIPS32 -I$(DIR_LINUX)/include/asm-mips -I$(DIR_LINUX)/arch/mips/bsp/ -I$(DIR_LINUX)/arch/mips/include/asm/mach-generic -fno-stack-protector -fomit-frame-pointer -fno-strict-overflow -fconserve-stack -ffunction-sections -fdata-sections -mlong-calls -DUFSD_BIGENDIAN -O1" CC=${COMPILER_NAME}-gcc --target=${COMPILER_TARGET} --host=${COMPILER_NAME} --with-ks-dir=${DIR_LINUX} --with-kb-dir=${DIR_LINUX}
  TARGET_MIPS_CFLAGS:=PACKAGE_TAG="lke_9.2.0_r245248_b7" ARCH="${COMPILER_TARGET}" CROSS_COMPILE=${COMPILER_NAME}- CROSSCOMPILE=${COMPILER_NAME}- TARGET="${COMPILER_TARGET}" CFLAGS="-DCONFIG_RTL_8198C\ -I$(DIR_LINUX)/arch/mips/include\ -I$(DIR_LINUX)/arch/mips/include/generated\ -I$(DIR_LINUX)/include\ -I$(DIR_LINUX)/arch/mips/include/uapi\ -I$(DIR_LINUX)/arch/mips/include/generated/uapi\ -I$(DIR_LINUX)/include/uapi\ -I$(DIR_LINUX)/include/generated/uapi\ -DVMLINUX_LOAD_ADDRESS=0x80000000\ -DDATAOFFSET=0\ -fno-strict-aliasing\ -fno-common\ -fno-delete-null-pointer-checks\ -mno-check-zero-division\ -mabi=32\ -G\ 0\ -mno-abicalls\ -fno-pic\ -pipe\ -msoft-float\ -ffreestanding\ -EB\ -UMIPSEB\ -U_MIPSEB\ -U__MIPSEB\ -U__MIPSEB__\ -UMIPSEL\ -U_MIPSEL\ -U__MIPSEL\ -U__MIPSEL__\ -DMIPSEB\ -D_MIPSEB\ -D__MIPSEB\ -D__MIPSEB__\ -U_MIPS_ISA\ -D_MIPS_ISA=_MIPS_ISA_MIPS32\ -I$(DIR_LINUX)/include/asm-mips\ -I$(DIR_LINUX)/arch/mips/bsp/\ -I$(DIR_LINUX)/arch/mips/include/asm/mach-generic\ -fno-stack-protector\ -fomit-frame-pointer\ -fno-strict-overflow\ -fconserve-stack\ -ffunction-sections\ -fdata-sections\ -mlong-calls\ -DUFSD_BIGENDIAN" EXT_MODULE_FLAGS="-DUFSD_DISABLE_UGM,-DUFSD_CHECK_BDI,-DUFSD_TRACE"
  TARGET_CFLAGS:=$(TARGET_MIPS_CFLAGS)
else
  $(error rtl_fs only supports for RTL8198C)
endif

MAKE_FLAGS:=$(MODULE_TARGET) $(TARGET_CFLAGS)

all: $(MODULE_TARGET)/Makefile $(MODULE_TARGET)/config.log
	make -C $(MAKE_FLAGS) driver

$(MODULE_TARGET)/Makefile: $(MODULE_TARGET)/Makefile.in
	cd $(MODULE_TARGET) && ./configure $(CONFIGURE_ARGS)

$(MODULE_TARGET)/config.log: $(DIR_LINUX)/.config
	make -C $(MAKE_FLAGS) clean
	cd $(MODULE_TARGET) && ./configure $(CONFIGURE_ARGS)

install:
	$(ROMFSINST) $(MODULE_TARGET)/jnl.ko /lib/modules
	$(ROMFSINST) $(MODULE_TARGET)/ufsd.ko /lib/modules
	$(ROMFSINST) $(MODULE_TARGET)/mkntfs /bin/
	$(ROMFSINST) $(MODULE_TARGET)/mkexfat /bin/
	$(ROMFSINST) $(MODULE_TARGET)/mkhfs /bin/
#	$(ROMFSINST) $(MODULE_TARGET)/sysdump_home_nas /bin/

clean:
	@if [ -e r245248/Makefile ]; then \
		make -C r245248 $(TARGET_MIPS_CFLAGS) clean ; \
		rm -f r245248/config.log ; \
	fi
