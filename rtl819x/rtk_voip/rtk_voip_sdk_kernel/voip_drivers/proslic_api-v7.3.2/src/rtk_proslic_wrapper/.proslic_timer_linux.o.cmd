cmd_rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.o := msdk-linux-gcc -Wp,-MD,rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/.proslic_timer_linux.o.d  -nostdinc -isystem /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/bin/../lib/gcc/mips-linux-uclibc/4.4.7/include -I/home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include -Iarch/mips/include/generated  -Iinclude -I/home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi -Iarch/mips/include/generated/uapi -I/home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/include/uapi -Iinclude/generated/uapi -include /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/include/linux/kconfig.h -D__KERNEL__ -DVMLINUX_LOAD_ADDRESS=0x80000000 -DDATAOFFSET=0 -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -Os -mno-check-zero-division -mabi=32 -G 0 -mno-abicalls -fno-pic -pipe -msoft-float -ffreestanding -EB -UMIPSEB -U_MIPSEB -U__MIPSEB -U__MIPSEB__ -UMIPSEL -U_MIPSEL -U__MIPSEL -U__MIPSEL__ -DMIPSEB -D_MIPSEB -D__MIPSEB -D__MIPSEB__ -U_MIPS_ISA -D_MIPS_ISA=_MIPS_ISA_MIPS32 -Wa,-mips32r2 -Wa,--trap -Iinclude/asm-mips -Iarch/mips/bsp/ -I/home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/mach-generic -Wframe-larger-than=1024 -fno-stack-protector -fomit-frame-pointer -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -Irtk_voip/include -Irtk_voip/voip_drivers -Irtk_voip/voip_drivers/proslic_api/inc -Irtk_voip/voip_drivers/proslic_api/config_inc -Irtk_voip/voip_drivers/proslic_api/config_src -DREALTEK_PATCH_FOR_SILAB -DSI3217X   -ffunction-sections -fdata-sections  -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(proslic_timer_linux)"  -D"KBUILD_MODNAME=KBUILD_STR(proslic_timer_linux)" -c -o rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.o rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.c

source_rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.o := rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.c

deps_rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.o := \
  include/linux/kernel.h \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/atomic/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/toolchain/msdk-4.4.7-mips-EB-3.10-0.9.33-m32t-131227b/bin/../lib/gcc/mips-linux-uclibc/4.4.7/include/stdarg.h \
  include/linux/linkage.h \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
    $(wildcard include/config/kprobes.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
    $(wildcard include/config/arch/use/builtin/bswap.h) \
  include/linux/stringify.h \
  include/linux/export.h \
    $(wildcard include/config/have/underscore/symbol/prefix.h) \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/modversions.h) \
    $(wildcard include/config/unused/symbols.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/linkage.h \
  include/linux/stddef.h \
  include/uapi/linux/stddef.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  include/uapi/linux/types.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/types.h \
    $(wildcard include/config/64bit/phys/addr.h) \
  include/asm-generic/int-ll64.h \
  include/uapi/asm-generic/int-ll64.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/uapi/asm-generic/bitsperlong.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi/asm/types.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/include/uapi/linux/posix_types.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi/asm/posix_types.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi/asm/sgidefs.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/include/uapi/asm-generic/posix_types.h \
  include/linux/bitops.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/bitops.h \
    $(wildcard include/config/cpu/mipsr2.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/barrier.h \
    $(wildcard include/config/cpu/has/sync.h) \
    $(wildcard include/config/cpu/cavium/octeon.h) \
    $(wildcard include/config/sgi/ip28.h) \
    $(wildcard include/config/cpu/has/wb.h) \
    $(wildcard include/config/weak/ordering.h) \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/weak/reordering/beyond/llsc.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/addrspace.h \
    $(wildcard include/config/cpu/r8000.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/mach-generic/spaces.h \
    $(wildcard include/config/32bit.h) \
    $(wildcard include/config/kvm/guest.h) \
    $(wildcard include/config/dma/noncoherent.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/include/uapi/linux/const.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi/asm/byteorder.h \
  include/linux/byteorder/big_endian.h \
  include/uapi/linux/byteorder/big_endian.h \
  include/linux/swab.h \
  include/uapi/linux/swab.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi/asm/swab.h \
  include/linux/byteorder/generic.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/cpu-features.h \
    $(wildcard include/config/cpu/mips4k.h) \
    $(wildcard include/config/cpu/mips24k.h) \
    $(wildcard include/config/cpu/mips34k.h) \
    $(wildcard include/config/cpu/mips74k.h) \
    $(wildcard include/config/cpu/mips1004k.h) \
    $(wildcard include/config/cpu/mips1074k.h) \
    $(wildcard include/config/cpu/has/fpu.h) \
    $(wildcard include/config/cpu/has/watch.h) \
    $(wildcard include/config/cpu/has/ar7.h) \
    $(wildcard include/config/cpu/mips14k.h) \
    $(wildcard include/config/cpu/has/dsp.h) \
    $(wildcard include/config/cpu/has/mipsmt.h) \
    $(wildcard include/config/cpu/has/tls.h) \
    $(wildcard include/config/cpu/mipsr2/irq/vi.h) \
    $(wildcard include/config/cpu/mipsr2/irq/ei.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/cpu.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/cpu-info.h \
    $(wildcard include/config/mips/mt/smp.h) \
    $(wildcard include/config/mips/mt/smtc.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/cache.h \
    $(wildcard include/config/mips/l1/cache/shift.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/mach-generic/kmalloc.h \
    $(wildcard include/config/dma/coherent.h) \
  arch/mips/bsp/bspcpu.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/war.h \
    $(wildcard include/config/cpu/r4000/workarounds.h) \
    $(wildcard include/config/cpu/r4400/workarounds.h) \
    $(wildcard include/config/cpu/daddi/workarounds.h) \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/ffz.h \
  include/asm-generic/bitops/find.h \
    $(wildcard include/config/generic/find/first/bit.h) \
  include/asm-generic/bitops/sched.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/arch_hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/le.h \
  include/asm-generic/bitops/ext2-atomic.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/typecheck.h \
  include/linux/printk.h \
    $(wildcard include/config/early/printk.h) \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/printk/func.h) \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/init.h \
    $(wildcard include/config/broken/rodata.h) \
  include/linux/kern_levels.h \
  include/linux/dynamic_debug.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  include/uapi/linux/string.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/string.h \
    $(wildcard include/config/cpu/r3000.h) \
  include/linux/errno.h \
  include/uapi/linux/errno.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/errno.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi/asm/errno.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/include/uapi/asm-generic/errno-base.h \
  include/uapi/linux/kernel.h \
    $(wildcard include/config/rtl/8881a.h) \
    $(wildcard include/config/rtl/8196e.h) \
    $(wildcard include/config/rtl/819xd.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/include/uapi/linux/sysinfo.h \
    $(wildcard include/config/rtl/819x.h) \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/div64.h \
  include/asm-generic/div64.h \
  include/linux/delay.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/asm/delay.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/include/uapi/linux/param.h \
  /home/steve1097/branch-rtl819x-sdk-v3.4.7.3/linux-3.10/arch/mips/include/uapi/asm/param.h \
  include/asm-generic/param.h \
    $(wildcard include/config/hz.h) \
  include/uapi/asm-generic/param.h \
  rtk_voip/include/voip_timer.h \
  rtk_voip/voip_drivers/proslic_api/config_inc/rtk_sys_driv_type.h \
  rtk_voip/voip_drivers/proslic_api/config_inc/si_voice_datatypes.h \
  rtk_voip/include/voip_types.h \
    $(wildcard include/config/rtl865xb.h) \
  include/generated/uapi/linux/version.h \
  rtk_voip/voip_drivers/spi/spi.h \
    $(wildcard include/config/rtk/voip/dect/spi/support.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/zarlink.h) \
    $(wildcard include/config/rtk/voip/8676/shared/spi.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8676.h) \
    $(wildcard include/config/rtk/voip/8676/spi/gpio.h) \
    $(wildcard include/config/rtk/voip/drivers/slic/silab.h) \
  rtk_voip/voip_drivers/gpio/gpio.h \
    $(wildcard include/config/rtk/voip/drivers/pcm8186.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8651.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8671.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8672.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm865xc.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8972b/family.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm89xxc.h) \
    $(wildcard include/config/rtk/voip/platform/8686.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm89xxd.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm89xxe.h) \
    $(wildcard include/config/rtk/voip/soc/8686/cpu0.h) \
    $(wildcard include/config/rtk/voip/soc/8686/cpu1.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8954e/cpu0.h) \
    $(wildcard include/config/rtk/voip/drivers/pcm8954e/cpu1.h) \
  rtk_voip/voip_drivers/gpio/gpio_8954e.h \

rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.o: $(deps_rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.o)

$(deps_rtk_voip/voip_drivers/proslic_api/src/rtk_proslic_wrapper/proslic_timer_linux.o):
