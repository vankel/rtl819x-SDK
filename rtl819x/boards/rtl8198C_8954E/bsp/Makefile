# Realtek Semiconductor Corp.
#
# bsp/Makefile
#     bsp Makefile
#
# Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
#

CPPFLAGS_vmlinux.lds := $(KBUILD_CFLAGS)

#
# Sheipa
#
core-y   += arch/mips/bsp/
cflags-y += -Iinclude/asm-mips -Iarch/mips/bsp/
#load-y   += 0x80010000
load-y   += 0x80000000
#load-y   += 0x80500000

extra-y  += vmlinux.lds

obj-y := prom.o setup.o irq.o timer.o serial.o flushall.o
#obj-$(CONFIG_PCI) += pci-fixup.o pci-ops.o
obj-$(CONFIG_PCI) += pci.o
obj-$(CONFIG_USB) += usb.o
obj-$(CONFIG_SATA_AHCI_PLATFORM) += sata.o
obj-$(CONFIG_I2C) += i2c.o
obj-$(CONFIG_SMP) += smp.o
obj-$(CONFIG_MIPS_MT_SMTC) += smtc.o
#obj-$(CONFIG_MIPS_CMP) += amon.o
