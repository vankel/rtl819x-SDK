#ifdef __KERNEL__
#include <linux/module.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/version.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/slab.h> 

#include "./include/aipc_mem.h"
#include "./include/aipc_reg.h"
#include "./include/dram_share.h"
#include "./include/aipc_ioctl.h"
#include "./include/aipc_debug.h"

#ifdef CONFIG_RTK_819X_AUTO_ZONE_PLANNING
extern int real_mem_size;
#endif

/*
*	IPC index use SRAM
*/           
#if    defined(CONFIG_RTL8686_IPC_IDX_USE_DRAM) &&  defined(CONFIG_RTL8686_IPC_IDX_USE_SRAM)
#error "CONFIG_RTL8686_IPC_IDX_USE_DRAM and CONFIG_RTL8686_IPC_IDX_USE_SRAM are conflict"
#endif
#if   !defined(CONFIG_RTL8686_IPC_IDX_USE_DRAM) && !defined(CONFIG_RTL8686_IPC_IDX_USE_SRAM)
#error "Please choose CONFIG_RTL8686_IPC_IDX_USE_DRAM or CONFIG_RTL8686_IPC_IDX_USE_SRAM"
#endif

static volatile unsigned int *dsp_boot_ins;

#ifndef CONFIG_RTL8686_IPC_DUAL_LINUX
static unsigned int jump_ins[] = {
                0x3c088000        /*lui     t0,0x8000*/,
                0x350804c4        /*ori     t0,t0,0x04c4*/,
                0x01000008        /*jr      t0*/,
                0x00000000        /*nop       */
              };
#else
static unsigned int jump_ins[] = {
                0x3c088000        /*lui     t0,0x8000*/,
//              0x35082950        /*ori     t0,t0,0x2950*/,
                0x35080000        /*ori     t0,t0,0x0000*/,
                0x01000008        /*jr      t0*/,
                0x00000000        /*nop       */
              };
#endif

#if 1
static const unsigned long opcode_uart0_show_boot_ok[] = {
    0x3c04b800,
    0x34842000,
    0x3c053535,
    0x34a53535,
    0xac850000,
    0x3c053131,
    0x34a53131,    
    0xac850000, 
    0x3c053838,
    0x34a53838,
    0xac850000,
    0x3c053131,
    0x34a53131,
    0xac850000,
    0x1000ffff,	   // b   <stop>
    0x00000000	   // nop
    
};
#endif


static const unsigned long opcode_uart2_show_B[] = {
    0x3c04b800,    // lui a0,0xb800
    0x34842200,    // ori a0,a0,0x2200
    0x3c054242,    // lui a1,0x4242
    0x34a54242,    // ori a1,a1,0x4242
    0xac850000,    // sw  a1,0(a0)
    0x1000ffff,    // b   <stop>
    0x00000000,    // nop
};


void uart2_test(void)
{
    // For 98C 256pin package, uart2 is default for pinmux-2.

    unsigned long div;

    // uart 2 init,
    *( ( volatile unsigned long * )0xB8002204 ) = 0x00000000;
    *( ( volatile unsigned long * )0xB800220C ) = 0x83000000;
#if 0   // clk 25M, uart=38400 --> div = 0x28
    *( ( volatile unsigned long * )0xB8002200 ) = 0x28000000;
    *( ( volatile unsigned long * )0xB8002204 ) = 0x00000000;
#else   // clk use config, uart=38400
#define CONFIG_LXBUS_HZ 200000000
    div = CONFIG_LXBUS_HZ / 16 / 38400;

    *( ( volatile unsigned long * )0xB8002200 ) = ( div & 0xFF ) << 24;
    *( ( volatile unsigned long * )0xB8002204 ) = ( div & 0xFF00 ) << 16;
#endif
    *( ( volatile unsigned long * )0xB800220C ) = 0x03000000;
    *( ( volatile unsigned long * )0xB8002210 ) = 0x03000000;
    *( ( volatile unsigned long * )0xB8002208 ) = 0xC1000000;

    printk( "run uart2 test\n" );

    *( ( volatile unsigned long * )0xB8002200 ) = 0x41000000; //print A

}


int
aipc_cpu_sram_map(
	u8_t  seg_no , 
	u32_t map_addr , 
	u32_t size , 
	u8_t  enable , 
	u32_t base_addr , 
	u8_t  lx_match)
{
	void *seg_addr=NULL;
	volatile u32_t tmp=0;
	if ((void*)map_addr==NULL || seg_no>=SRAM_SEG_MAX){
		printk( "wrong sram map setting\n" );
		return NOK;	
		}

	if (seg_no==SRAM_SEG_IDX_0){
		seg_addr = (void*)R_C0SRAMSAR0;
		}
	else if (seg_no==SRAM_SEG_IDX_1){
		seg_addr = (void*)R_C0SRAMSAR1;
		}
	else if (seg_no==SRAM_SEG_IDX_2){
		seg_addr = (void*)R_C0SRAMSAR2;
		}
	else if (seg_no==SRAM_SEG_IDX_3){
		seg_addr = (void*)R_C0SRAMSAR3;
		}
	else{
		return NOK;	
		}

	tmp	= Virtual2Physical(map_addr);
	
	if (enable==SRAM_SEG_ENABLE){
		tmp	|= SRAM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~SRAM_SEG_ENABLE_BIT;
		}
	
	if (lx_match==SRAM_LX_MATCH_ENABLE){
		tmp	|= SRAM_LX_MATCH_BIT;
		}
	else{
		tmp	&= ~SRAM_LX_MATCH_BIT;
		}


	REG32(seg_addr)		=	tmp;	
	REG32(seg_addr+4)	=	size;
	REG32(seg_addr+8)	=	base_addr;	//base

	SDEBUG("SRAM map_addr v2p = 0x%08x\n" , Virtual2Physical(map_addr));
	SDEBUG("SRAM SEG  = 0x%08x\n"         , REG32(seg_addr));
	SDEBUG("SRAM Size = 0x%08x\n"         , REG32(seg_addr+4));
	SDEBUG("SRAM Base = 0x%08x\n"         , REG32(seg_addr+8));

	return OK;
}

int
aipc_cpu_dram_unmap(
	u8_t  seg_no , 
	u32_t unmap_addr , 
	u32_t size , 
	u8_t  enable , 
	u8_t  lx_match)
{
	void *seg_addr=NULL;
	volatile u32_t tmp=0;	
	if ((void*)unmap_addr==NULL || seg_no>=DRAM_SEG_MAX){
		printk( "wrong dram unmap setting\n" );
		return NOK;	
		}

	if (seg_no==DRAM_SEG_IDX_0){
		seg_addr = (void*)R_C0UMSAR0;
		}
	else if (seg_no==DRAM_SEG_IDX_1){
		seg_addr = (void*)R_C0UMSAR1;
		}
	else if (seg_no==DRAM_SEG_IDX_2){
		seg_addr = (void*)R_C0UMSAR2;
		}
	else if (seg_no==DRAM_SEG_IDX_3){
		seg_addr = (void*)R_C0UMSAR3;
		}
	else{
		return NOK;	
		}
	
	tmp	= Virtual2Physical(unmap_addr);

	if (enable==DRAM_SEG_ENABLE){
		tmp	|= DRAM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~DRAM_SEG_ENABLE_BIT;
		}

	if (lx_match==DRAM_LX_MATCH_ENABLE){
		tmp	|= DRAM_LX_MATCH_BIT;
		}
	else{
		tmp	&= ~DRAM_LX_MATCH_BIT;
		}

	REG32(seg_addr)		=	tmp;
	REG32(seg_addr+4)	=	size;

	SDEBUG("DRAM unmap_addr v2p = 0x%08x\n" , Virtual2Physical(unmap_addr));	
	SDEBUG("DRAM SEG  = 0x%08x\n"           , REG32(seg_addr));
	SDEBUG("DRAM Size = 0x%08x\n"           , REG32(seg_addr+4));

	return OK;
}

#if 0
int
aipc_dsp_sram_map(
	u8_t  seg_no , 
	u32_t map_addr , 
	u32_t size , 
	u8_t  enable , 
	u32_t base_addr , 
	u8_t  lx_match)
{
	void *seg_addr=NULL;
	volatile u32_t tmp=0;	
	if ((void*)map_addr==NULL || seg_no>=SRAM_SEG_MAX){
		printk( "wrong sram map setting\n" );
		return NOK;	
		}

	if (seg_no==SRAM_SEG_IDX_0){
		seg_addr = (void*)R_C1SRAMSAR0;
		}
	else if (seg_no==SRAM_SEG_IDX_1){
		seg_addr = (void*)R_C1SRAMSAR1;
		}
	else if (seg_no==SRAM_SEG_IDX_2){
		seg_addr = (void*)R_C1SRAMSAR2;
		}
	else if (seg_no==SRAM_SEG_IDX_3){
		seg_addr = (void*)R_C1SRAMSAR3;
		}
	else{
		return NOK;	
		}

	tmp	= Virtual2Physical(map_addr);
	if (enable==SRAM_SEG_ENABLE){
		tmp	|= SRAM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~SRAM_SEG_ENABLE_BIT;
		}

	if (lx_match==SRAM_LX_MATCH_ENABLE){
		tmp	|= SRAM_LX_MATCH_BIT;
		}
	else{
		tmp	&= ~SRAM_LX_MATCH_BIT;
		}
	
	REG32(seg_addr)		=	tmp;
	REG32(seg_addr+4)	=	size;
	REG32(seg_addr+8)	=	base_addr;	//base
		
	return OK;
}

int
aipc_dsp_dram_unmap(
	u8_t  seg_no , 
	u32_t unmap_addr , 
	u32_t size , 
	u8_t  enable , 
	u8_t  lx_match)
{
	void *seg_addr=NULL;
	volatile u32_t tmp=0;		
	if ((void*)unmap_addr==NULL || seg_no>=DRAM_SEG_MAX){
		printk( "wrong dram unmap setting\n" );
		return NOK;	
		}
 
	if (seg_no==DRAM_SEG_IDX_0){
		seg_addr = (void*)R_C1UMSAR0;
		}
	else if (seg_no==DRAM_SEG_IDX_1){
		seg_addr = (void*)R_C1UMSAR1;
		}
	else if (seg_no==DRAM_SEG_IDX_2){
		seg_addr = (void*)R_C1UMSAR2;
		}
	else if (seg_no==DRAM_SEG_IDX_3){
		seg_addr = (void*)R_C1UMSAR3;
		}
	else{
		return NOK;
		}
	
	tmp	= Virtual2Physical(unmap_addr);
	if (enable==DRAM_SEG_ENABLE){
		tmp	|= DRAM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~DRAM_SEG_ENABLE_BIT;
		}
	
	if (lx_match==DRAM_LX_MATCH_ENABLE){
		tmp	|= DRAM_LX_MATCH_BIT;
		}
	else{
		tmp	&= ~DRAM_LX_MATCH_BIT;
		}

	REG32(seg_addr)		= 	tmp;
	REG32(seg_addr+4)	=	size;

	return OK;
}
#endif

int
aipc_cpu_rom_map( 
	u32_t map_addr , 
	u32_t clk , 
	u32_t wait ,
	u32_t size , 
	u8_t  enable)
{
	volatile u32_t tmp=0;
	
	if ((void*)map_addr==NULL){
		printk( "wrong rom map setting\n" );
		return NOK;
	}

	//Setup segment address	register
#if 0
	tmp	= Virtual2Physical(map_addr) & ROM_SEG_MASK;
#else
	tmp	= Virtual2Physical(map_addr);
#endif
	
	if (enable==ROM_SEG_ENABLE){
		tmp	|= ROM_SEG_ENABLE_BIT;		//enable
		}
	else{
		tmp	&= ~ROM_SEG_ENABLE_BIT;
		}
	REG32(R_ROMSAR) = tmp;
	
	
	//Setup segment size register
	tmp = REG32(R_ROMSSR);
	
	if (clk==ROM_CLOCK_DIV_2){
		tmp &= ~ROM_CLOCK_DIV_BIT;
	}
	else{	//ROM_CLOCK_DIV_4
		tmp |= ROM_CLOCK_DIV_BIT;
	}
	
	if (wait>ROM_WAIT_TIME_NONE && wait<=ROM_WAIT_TIME_7T){
		tmp |= (wait<<7);
	}
	
#ifdef CONFIG_RTL8686_FPGA		/*in FPGA phase*/
	if (clk==ROM_CLOCK_DIV_NONE){
		tmp &= ~ROM_CLOCK_DIV_BIT;
	}
	if (wait==ROM_WAIT_TIME_NONE){
		tmp &= (0<<7);
		tmp &= (0<<8);
		tmp &= (0<<9);
	}
#endif
		
	if (size>=ROM_SIZE_32K && size<=ROM_SIZE_1M){
		tmp |= size;
	}
	
	REG32(R_ROMSSR) = tmp;

//	SDEBUG("ROM Address Reg=%x\n" , REG32(R_ROMSAR));
//	SDEBUG("ROM Size Reg=%x\n"    , REG32(R_ROMSSR));
	
	return OK;	
}

int
aipc_dsp_entry(void)
{
	int i=0;
#define MALLOC_DSP_BOOT_INS_SRAM_MAPPING_ADDR_SPACE
#ifdef MALLOC_DSP_BOOT_INS_SRAM_MAPPING_ADDR_SPACE
	void *dsp_boot_ins_sram_mapping_addr;

	dsp_boot_ins_sram_mapping_addr = kmalloc(512, GFP_KERNEL);
	if (!dsp_boot_ins_sram_mapping_addr) {
		printk("%s:kmalloc failed\n", __FUNCTION__);
		return -1;
	}
	// mapping addr must be an integral multiple of the segment size
	dsp_boot_ins = (unsigned int *)PTR_ALIGN(dsp_boot_ins_sram_mapping_addr, 256);
	dsp_boot_ins = KSEG1ADDR(dsp_boot_ins);
	//printk("dsp_boot_ins=%p\n", dsp_boot_ins);
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_0, 
		dsp_boot_ins, 
		SRAM_SIZE_256B,
		SRAM_SEG_ENABLE, 
		GPON_SRAM_BASE, 		//98c: 0x30000 
		~SRAM_LX_MATCH_ENABLE);

	aipc_cpu_dram_unmap(
		DRAM_SEG_IDX_0,
		dsp_boot_ins, 
		SRAM_SIZE_256B,
		DRAM_SEG_ENABLE, 
		~DRAM_LX_MATCH_ENABLE);
#else
#ifdef GPON_RESV
#ifdef CONFIG_RTL8686_IPC_IDX_USE_DRAM	
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxE
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_0, 
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		SRAM_SIZE_8KB,
		SRAM_SEG_ENABLE , 
		GPON_SRAM_BASE , 		//Add GPON SRAM BASE
		~SRAM_LX_MATCH_ENABLE);

	aipc_cpu_dram_unmap(
		DRAM_SEG_IDX_0,
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		DRAM_SIZE_8KB,
		DRAM_SEG_ENABLE , 
		~DRAM_LX_MATCH_ENABLE);
#else
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_1, 
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		SRAM_SIZE_8KB,
		SRAM_SEG_ENABLE , 
		GPON_SRAM_BASE , 		//Add GPON SRAM BASE
		~SRAM_LX_MATCH_ENABLE);

	aipc_cpu_dram_unmap(
		DRAM_SEG_IDX_1,
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		DRAM_SIZE_8KB,
		DRAM_SEG_ENABLE , 
		~DRAM_LX_MATCH_ENABLE);
#endif
#else  /* Use SRAM Only */

	aipc_cpu_sram_map(
		SRAM_SEG_IDX_1, 
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		SRAM_SIZE_8KB,
		SRAM_SEG_ENABLE , 
		GPON_SRAM_BASE , 		//Add GPON SRAM BASE
		~SRAM_LX_MATCH_ENABLE);

#endif
#else
	aipc_cpu_sram_map(
		SRAM_SEG_IDX_0 , 
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		0 , 
		~SRAM_LX_MATCH_ENABLE);

	aipc_cpu_dram_unmap(
		DRAM_SEG_IDX_0 , 
		DSP_BOOT_INS_SRAM_MAPPING_ADDR , 
		DRAM_SIZE_128KB , 
		DRAM_SEG_ENABLE , 
		~DRAM_LX_MATCH_ENABLE);
#endif

	dsp_boot_ins = ((volatile unsigned int *)DSP_BOOT_INS_SRAM_MAPPING_ADDR);
#endif

#if 0
#if 0
	// boot ok
	for(i=0 ; i<sizeof(opcode_uart2_show_B) ; i++){
		*dsp_boot_ins = opcode_uart2_show_B[i];
		dsp_boot_ins++;
	}
#else
	// boot ok
	for(i=0 ; i<sizeof(opcode_uart0_show_boot_ok) ; i++){
		*dsp_boot_ins = opcode_uart0_show_boot_ok[i];
		dsp_boot_ins++;
	}
#endif
#else
	for(i=0 ; i<sizeof(jump_ins) ; i++){
		*dsp_boot_ins = jump_ins[i];
		dsp_boot_ins++;
	}
#endif

#ifdef MALLOC_DSP_BOOT_INS_SRAM_MAPPING_ADDR_SPACE
	//after coping dsp boot instructions to sram, disable mapped and unmapped settings and free memory
	REG32(R_C0SRAMSAR0) &= ~SRAM_SEG_ENABLE_BIT; //disable sram segment mapping
	REG32(R_C0UMSAR0) &= ~DRAM_SEG_ENABLE_BIT; //disable dram segment unmapping
	kfree(dsp_boot_ins_sram_mapping_addr);
#endif

	return 0;
}

int
aipc_rom_set(unsigned int rom_addr)
{
#ifdef CONFIG_RTL8686_ASIC

	aipc_cpu_rom_map(
		rom_addr , //DSP_ROMCODE_ADDR
		ROM_CLOCK_DIV_NONE , 
		ROM_WAIT_TIME_NONE , 
		ROM_SIZE_256K , 
		ROM_SEG_ENABLE);

#else		//in FPGA phase

	aipc_cpu_sram_map(
		SRAM_SEG_IDX_0 ,
		rom_addr , 			//DSP_ROMCODE_ADDR
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		0 , 
		~SRAM_LX_MATCH_ENABLE);

	aipc_cpu_sram_map(
		SRAM_SEG_IDX_1 ,
		rom_addr+ROM_BASE_128K , 	//DSP_ROMCODE_ADDR+0x20000
		SRAM_SIZE_128KB , 
		SRAM_SEG_ENABLE , 
		ROM_BASE_128K , 			//base=0x20000
		~SRAM_LX_MATCH_ENABLE);

#endif
	return OK;
}

int
aipc_zone_set(zone_plan_t zp)
{
#ifdef CONFIG_RTK_819X_AUTO_ZONE_PLANNING
	int cpu0_mem_base, cpu0_mem_size;
	int ipc_mem_base, ipc_mem_size;
	int dsp_mem_base, dsp_mem_size;

	cpu0_mem_base = 0x00000000;
	ipc_mem_size =  0x00100000; // 1 MB
	dsp_mem_size = 0x00700000; // 7 MB
	if (real_mem_size == (64<<20)) {
		cpu0_mem_size = 0x03800000;
		ipc_mem_base = 0x03800000;
		dsp_mem_base = 0x03900000;
	}
	else if (real_mem_size == (128<<20)) {
		cpu0_mem_size = 0x07800000;
		ipc_mem_base = 0x07800000;
		dsp_mem_base = 0x07900000;
	}
	else { // >= 256MB
		cpu0_mem_size = 0x0F800000;
		ipc_mem_base = 0x0F800000;
		dsp_mem_base = 0x0F900000;
	}
#endif

	if (zp==zp_dsp_init){
#ifdef CONFIG_RTK_819X_AUTO_ZONE_PLANNING
		//CPU Zone 0
		REG32(C0DOR0)  = cpu0_mem_base;
		REG32(C0DMAR0) = cpu0_mem_size-1;
		
		//CPU Zone 1
		if (real_mem_size == (512<<20))
			REG32(C0DOR1)  = dsp_mem_base+0x10000000;
		else
			REG32(C0DOR1)  = dsp_mem_base;
		REG32(C0DMAR1) = dsp_mem_size-1;

		//DSP Zone 0
		REG32(C1DOR0)  = dsp_mem_base;
		REG32(C1DMAR0) = dsp_mem_size-1;

		//DSP Zone 1
		if (real_mem_size == (512<<20))
			REG32(C1DOR1)  = ipc_mem_base+0x10000000;
		else
			REG32(C1DOR1)  = ipc_mem_base;	
		REG32(C1DMAR1) = ipc_mem_size-1;
#else
		//CPU Zone 0
		REG32(C0DOR0)  = CONFIG_RTL8686_CPU_MEM_BASE;
		REG32(C0DMAR0) = CONFIG_RTL8686_CPU_MEM_SIZE-1;
		
		//CPU Zone 1
		REG32(C0DOR1)  = CONFIG_RTL8686_DSP_MEM_BASE;
		REG32(C0DMAR1) = CONFIG_RTL8686_DSP_MEM_SIZE-1;

		//DSP Zone 0
		REG32(C1DOR0)  = CONFIG_RTL8686_DSP_MEM_BASE;
		REG32(C1DMAR0) = CONFIG_RTL8686_DSP_MEM_SIZE-1;

		//DSP Zone 1
		REG32(C1DOR1)  = CONFIG_RTL8686_IPC_MEM_BASE;	
		REG32(C1DMAR1) = CONFIG_RTL8686_IPC_MEM_SIZE-1;
#endif
	}
	else {
#ifdef CONFIG_RTK_819X_AUTO_ZONE_PLANNING
		//CPU Zone 1
		if (real_mem_size == (512<<20))
			REG32(C0DOR1)  = ipc_mem_base+0x10000000;
		else
			REG32(C0DOR1)  = ipc_mem_base;
		REG32(C0DMAR1) = ipc_mem_size-1;
#else
		//CPU Zone 1
		REG32(C0DOR1)  = CONFIG_RTL8686_IPC_MEM_BASE;
		REG32(C0DMAR1) = CONFIG_RTL8686_IPC_MEM_SIZE-1;
#endif
	}

#if 1
	printk("CPU zone config:\n");
	printk("  C0DOR0=%08x C0DMAR0=%08x\n" , REG32(C0DOR0) , REG32(C0DMAR0));
	printk("  C0DOR1=%08x C0DMAR1=%08x\n" , REG32(C0DOR1) , REG32(C0DMAR1));
	
	printk("DSP zone config:\n");
	printk("  C1DOR0=%08x C1DMAR0=%08x\n" , REG32(C1DOR0) , REG32(C1DMAR0));
	printk("  C1DOR1=%08x C1DMAR1=%08x\n" , REG32(C1DOR1) , REG32(C1DMAR1));
#endif
	return 0;
}

#ifndef CONFIG_RTL8686_IPC_DUAL_LINUX
int aipc_dsp_boot(void)
#else
int aipc_dual_linux_boot(void)
#endif
{
#ifdef CONFIG_RTK_VOIP_DRIVERS_PCM89xxE
	volatile unsigned int *dsp_reg;
	volatile u32_t tmp=0;

	/*
	*	1. add clock to DSP and set DSP TAP on.
	*/
	printk("aipc_dsp_boot step 1: add clock\n");
	dsp_reg 	= 	(volatile unsigned int *)(R_CLK_MANAGE);
	tmp			=   *dsp_reg;
	
	if ((tmp & BIT_CPU2_ACTIVE) == 0)
	{
		tmp			|= 	BIT_CPU2_ACTIVE;		// Set BIT(31) to active CPU2(DSP)
		*dsp_reg 	= 	tmp;
		
		printk("active CPU2 ...\n");
	}
	
	tmp			&= 	~BIT_CPU2_SYS_EN;		// Set BIT(30) to disable CPU2(DSP)
	*dsp_reg 	= 	tmp;

	tmp			|= 	BIT_CPU2_SYS_EN;		// Set BIT(30) to enable CPU2(DSP)
	*dsp_reg 	= 	tmp;

	printk("aipc_dsp_boot done\n");
	
	uart2_test();

#elif defined (CONFIG_RTK_VOIP_PLATFORM_8686)

#ifdef CONFIG_RTL8686_ASIC
    volatile unsigned int *dsp_reg;
    volatile u32_t tmp=0;

	/*
	*	1. add clock to DSP and set DSP TAP on.
	*/
    dsp_reg 	= 	(volatile unsigned int *)(R_AIPC_ASIC_ENABLE_DSP_CLK);
    tmp			=   *dsp_reg;
    
    tmp			&=  ~BIT_ENABLE_DSP_TAP;	// Set BIT(6) as 0 to enable DSP TAP.
    tmp			|= 	BIT_ENABLE_DSP_CLOCK;
    
    *dsp_reg 	= 	tmp;

	/*
	*	2. delay 4 ms
	*/
	mdelay(KICK_DSP_DELAY_TIME);

	/*
	*	3. kick DSP
	*/ 
    dsp_reg 	= 	(volatile unsigned int *)(R_AIPC_ASIC_KICK_DSP);
    *dsp_reg 	|= 	BIT_KICK_DSP;


	/*
	*	4. kernel delay time
	*/
#ifdef KERNEL_BOOT_DELAY_TIME
	mdelay(KERNEL_BOOT_DELAY_TIME);
#endif

#else // !CONFIG_RTL8686_ASIC

    volatile unsigned int *dsp_kick_reg;

    dsp_kick_reg 	= (volatile unsigned int *)(R_AIPC_BOOT_DSP);
    *dsp_kick_reg 	|= BOOT_DSP_BIT;
	
#endif // end of CONFIG_RTL8686_ASIC
#endif
	
	return OK;
}
#ifdef AIPC_BOOT
/*
*	Shared memory init function
*/
int aipc_boot_init(void)
{									//DSP booting address
//	ABOOT.cmd[0] = 0x3c08bfc0;		/*lui 	t0,0xbfc0*/
	ABOOT.cmd[0] = 0x3c088000;		/*lui 	t0,0x8000*/
	ABOOT.cmd[1] = 0x35080000;		/*ori 	t0,t0,0x0000*/
	ABOOT.cmd[2] = 0x01000008;		/*jr	t0*/
	ABOOT.cmd[3] = 0x00000000;		/*nop 	  */
	
	return OK;
}
#endif

#endif 

