/*
 * Realtek Semiconductor Corp.
 *
 * bsp/prom.c
 *     bsp early initialization code
 *
 * Copyright (C) 2006-2012 Tony Wu (tonywu@realtek.com)
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <asm/addrspace.h>
#include <asm/bootinfo.h>

#include "bspcpu.h"

extern char arcs_cmdline[];

#ifdef CONFIG_EARLY_PRINTK
static int promcons_output __initdata = 0;

void unregister_prom_console(void)
{
	if (promcons_output)
		promcons_output = 0;
}

void disable_early_printk(void)
    __attribute__ ((alias("unregister_prom_console")));
#endif

const char *get_system_type(void)
{
	return "Sheipa Platform";
}

void __init bsp_free_prom_memory(void)
{
	return;
}

#define REG32(reg)	(*(volatile unsigned int *)(reg))
#define DCR			(0xB8001004)
#define DCR_BANKCNT_FD_S	(28)
#define DCR_DBUSWID_FD_S	(24)
#define DCR_ROWCNT_FD_S		(20)
#define DCR_COLCNT_FD_S		(16)
#define DCR_DCHIPSEL_FD_S	(15)
#define DCR_BANKCNT_MASK	(0xF << DCR_BANKCNT_FD_S)
#define DCR_DBUSWID_MASK	(0xF << DCR_DBUSWID_FD_S)
#define DCR_ROWCNT_MASK		(0xF << DCR_ROWCNT_FD_S)
#define DCR_COLCNT_MASK		(0xF << DCR_COLCNT_FD_S)
#define DCR_DCHIPSEL_MASK	(1 << DCR_DCHIPSEL_FD_S)

unsigned int _DCR_get_buswidth(void)
{
	unsigned int buswidth;

	buswidth = ((REG32(DCR) & DCR_DBUSWID_MASK) >> DCR_DBUSWID_FD_S);

	switch (buswidth) {
		case 0:
			return (8);
		case 1:
			return (16);
		case 2:
			return (32); 
		default:
			return 0;
	}
}
unsigned int _DCR_get_chipsel(void)
{
	unsigned int chipsel;

	chipsel = ((REG32(DCR) & DCR_DCHIPSEL_MASK) >> DCR_DCHIPSEL_FD_S);

	if(chipsel)
		return 2;
	else
		return 1;
}
unsigned int _DCR_get_rowcnt(void)
{
	unsigned int rowcnt;

	rowcnt = ((REG32(DCR) & DCR_ROWCNT_MASK) >> DCR_ROWCNT_FD_S);
	
	return (2048 << rowcnt);
}
unsigned int _DCR_get_colcnt(void)
{
	unsigned int colcnt;

	colcnt = ((REG32(DCR) & DCR_COLCNT_MASK) >> DCR_COLCNT_FD_S);

	if(4 < colcnt)
		return 0;
	else
		return (256 << colcnt);
}
unsigned int _DCR_get_bankcnt(void)
{
	unsigned int bankcnt;

	bankcnt = ((REG32(DCR) & DCR_BANKCNT_MASK) >> DCR_BANKCNT_FD_S);
	
	switch (bankcnt)
	{
		case 0:
			return 2;
		case 1:
			return 4;
		case 2:
			return 8;
	}
	return 0;
}

#ifdef CONFIG_RTK_819X_AUTO_ZONE_PLANNING
int real_mem_size;
#endif

/* Do basic initialization */
void __init bsp_init(void)
{
	u_long mem_size;
	unsigned int	buswidth, chipsel, rowcnt;
	unsigned int	colcnt, bankcnt;

	arcs_cmdline[0] = '\0';
	strcpy(arcs_cmdline, "console=ttyS0,38400");

#if 1
	/* 
	 * Check DCR
	 */
	/* 1. Bus width     */
	buswidth = _DCR_get_buswidth();

	/* 2. Chip select   */
	chipsel = _DCR_get_chipsel();

	/* 3. Row count     */
	rowcnt = _DCR_get_rowcnt();

	/* 4. Column count  */
	colcnt = _DCR_get_colcnt();

	/* 5. Bank count    */
	bankcnt = _DCR_get_bankcnt();

	if ((buswidth == 0) || (colcnt == 0) || (bankcnt == 0))
		mem_size = cpu_mem_size;	
	else
		mem_size = rowcnt*colcnt*(buswidth/8)*chipsel*bankcnt;

#else	
	mem_size = cpu_mem_size;
#endif

#ifdef CONFIG_RTK_819X_AUTO_ZONE_PLANNING
	real_mem_size = mem_size;
#endif

#ifdef  CONFIG_RTL8198C_OVER_256MB
	mem_size = (256 << 20);
#ifdef CONFIG_RTK_VOIP
    if (mem_size <= (256 << 20)) // <= 256M
    {
        mem_size = mem_size - 0x800000; // reserve 8M
        printk("mem_size = 0x%x. VoIP share memory config is ok.\n", mem_size);
    }
    else
    {
        int i;
        for (i=0; i < 10; i++)
            printk("Note!! %s, line%d: mem size(=0x%x)is not expected, need to re-config VoIP share memory\n", __FUNCTION__, __LINE__, mem_size);
    }
#endif
	add_memory_region(0, mem_size, BOOT_MEM_RAM);
	add_memory_region(0x30000000, 256 * 1024 * 1024, BOOT_MEM_RAM);
	
#else
	if(mem_size > (320<<20)) //patch for Memory address bigger than 256 MB
		mem_size = (320 << 20);	

#ifdef CONFIG_RTK_VOIP
    if (mem_size <= (256 << 20)) // <= 256M
    {
        mem_size = mem_size - 0x800000; // reserve 8M
        printk("mem_size = 0x%x. VoIP share memory config is ok.\n", mem_size);
    }
    else
    {
        int i;
        for (i=0; i < 10; i++)
            printk("Note!! %s, line%d: mem size(=0x%x)is not expected, need to re-config VoIP share memory\n", __FUNCTION__, __LINE__, mem_size);
    }
#endif
	add_memory_region(0, mem_size, BOOT_MEM_RAM);
#endif
}
