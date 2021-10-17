#
# Makefile for the Linux/MIPS-specific parts of the memory manager.
#

obj-y				+= cache.o dma-default.o extable.o fault.o \
				   gup.o init.o mmap.o page.o page-funcs.o \
				   tlbex.o tlbex-fault.o uasm-mips.o

obj-$(CONFIG_32BIT)		+= ioremap.o pgtable-32.o
obj-$(CONFIG_HIGHMEM)		+= highmem.o
obj-$(CONFIG_HUGETLB_PAGE)	+= hugetlbpage.o

obj-$(CONFIG_CPU_R4K_CACHE_TLB) += c-r4k.o cex-gen.o tlb-r4k.o
obj-$(CONFIG_CPU_R3000)		+= c-r3k.o tlb-r3k.o

obj-$(CONFIG_MIPS_CPU_SCACHE)	+= sc-mips.o

obj-$(CONFIG_SYS_SUPPORTS_MICROMIPS) += uasm-micromips.o
