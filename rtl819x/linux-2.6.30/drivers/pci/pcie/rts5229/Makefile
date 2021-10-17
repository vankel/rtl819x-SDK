#
# Makefile for PCI-Express Root Port Advanced Error Reporting Driver
#

obj-$(CONFIG_PCIE_RTS5229) += rts5229driver.o
EXTRA_CFLAGS := -Idrivers/scsi 
rts5229driver-objs := rtsx.o rtsx_chip.o rtsx_transport.o rtsx_scsi.o rtsx_card.o \
			 general.o sd.o ms.o

