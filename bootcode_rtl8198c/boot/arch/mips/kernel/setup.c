

//===============================================================================
//
// This code can be run un-cache address.
//
//================================================================================


#include <asm/asm.h>
#include <asm/bootinfo.h>
#include <asm/cachectl.h>
#include <asm/io.h>
#include <asm/stackframe.h>
#include <asm/system.h>
#include <asm/cpu.h>
#include <asm/mipsregs.h>
#include <asm/cacheops.h>

#include <asm/rtl8198c.h>



//==============================================================================
#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	push					\n"	\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	pop					\n"	\
	:								\
	: "i" (op), "R" (*(unsigned char *)(addr)))


//============================================================================
void init_icache()
{
	#define KSEG0BASE                 0x80000000
	#define CONFIG_SYS_CACHELINE_SIZE 32

	unsigned int t;
	t=0;
	write_32bit_cp0_register_sel( 29,t, 0);  //CP0_ITagHi
	write_32bit_cp0_register_sel( 28,t, 0);  //CP0_ITagLo

	
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = KSEG0BASE;
	unsigned long aend = addr+(64<<10)-lsize;   //DCACHE=64K

	while (1) 
	{
		cache_op(Index_Store_Tag_I, addr);
		if (addr >= aend)
			break;
		addr += lsize;
	}

}
//==============================================================================
void init_dcache()
{
	#define KSEG0BASE                 0x80000000
	#define CONFIG_SYS_CACHELINE_SIZE 32

	unsigned int t;
	t=0;
	write_32bit_cp0_register_sel( 29, t, 2);  //CP0_DTagHi
	write_32bit_cp0_register_sel( 28, t, 2);  //CP0_DTagLo

	
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = KSEG0BASE;
	unsigned long aend = addr+(32<<10)-lsize;   //DCACHE=32K

	while (1) 
	{
		cache_op(Index_Store_Tag_D, addr);
		if (addr >= aend)
			break;
		addr += lsize;
	}

}
//============================================================================
enable_flash_mapping()
{
	REG32(0xb8001000)&=~(1<<18);
}
//============================================================================

init_cpu_config()
{
	unsigned int s;
#if 1	
	//setting K23, KU, MM
	s = read_32bit_cp0_register(CP0_CONFIG);
	s &= ~((3<<28) | (3<<25) | (1<<18));
	s |=  ((3<<28) | (3<<25) | (1<<18));
	write_32bit_cp0_register(CP0_CONFIG, s);
#endif

#if CONFIG_RTL8198C
	//set_cp0_config(0x07, 2); //2:uncache
	//set_cp0_config(0x07, 3); //3:cache
#endif

	
	s = read_32bit_cp0_register(CP0_STATUS);
	s &= ~(ST0_CU1|ST0_CU2|ST0_CU3);
	s |= ST0_CU0;
	write_32bit_cp0_register(CP0_STATUS, s);

	SPECIAL_EHB();

	//---------------------------------------------config2
#if 0	
	//disable L2
	s=(1<<12);
	write_32bit_cp0_register_sel(16, s, 2);
#endif


	//----------------------------------------------config 7
#if 0	
	s = read_32bit_cp0_register_sel(16, 7);
	//s &= ~((3<<28) | (3<<25) | (1<<18));
	s |=  (1<<29);
	write_32bit_cp0_register_sel(16, s,7);
#endif
	

//	sys_init_icache();
//	sys_init_dcache();	
}


//============================================================================

enable_GIC_mapping()
{
	//setting GIC base
	REG32(GCR_BASE_ADDR+0x0080)=Virtual2Physical(GIC_BASE_ADDR)|0x01;

	REG32(GCR_BASE_ADDR+0x0088)=Virtual2Physical(CPC_BASE_ADDR)|0x01;


	//IP inverter
	REG32(GIC_BASE_ADDR+0x100)=0xffffffff;
	REG32(GIC_BASE_ADDR+0x104)=0x0003FFFF;	

	//mask
//	REG32(GIC_BASE_ADDR+0x380)=0xffffffff;
//	REG32(GIC_BASE_ADDR+0x380)=(1<<0);
//	GIC_GIMR_enable(50);
	

	//map2pin
	//REG32(GIC_BASE_ADDR+0x500)=0x80000000;

	//map2vpe
	//REG32(GIC_BASE_ADDR+0x2000)=0x1;
}

//============================================================================
	
asmlinkage void init_arch(int argc, char **argv, char **envp, int *prom_vec)
{
	//init_icache();
	//init_icache();		
	
	enable_flash_mapping();	
	init_cpu_config();	
	enable_GIC_mapping();


   // start_kernel();

	void (*jmp)(void);
	extern void start_kernel();

#ifdef CONFIG_NAND_FLASH_BOOTING
	jmp=((int)start_kernel); //jump to un cache address
#else
	jmp=((int)start_kernel)& ~UNCACHE_MASK;
#endif
	jmp();
}


//============================================================================


