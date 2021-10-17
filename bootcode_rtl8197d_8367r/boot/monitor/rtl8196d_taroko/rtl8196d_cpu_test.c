#include <linux/types.h>
#include <asm/system.h>
#include <asm/lexraregs.h>


#define IMEM0_SIZE (16*(1<<10)) //16k
#define IMEM0_ENABLE	1
#define IMEM1_SIZE (0*(1<<10)) //0k
#define IMEM1_ENABLE	0
#define DMEM0_SIZE (8*(1<<10)) //8k
#define DMEM0_ENABLE	0
#define DMEM1_SIZE (0*(1<<10)) //0k
#define DMEM1_ENABLE	0

#define L2MEM_SIZE (0*(1<<10)) //0k
#define L2MEM_ENABLE	0
#define PASS	0 //JSW
#define FAIL	1//JSW

#define TO_UNCACHED_ADDR(addr) ((unsigned int)(addr) | (0x20000000))

#define MEM_TEST_ADDR 0x80c00000

#define MEM_TEST_ADDR1 0x80d00000

#define MEM_TEST_ADDR2 0x80d40000
#define MEM_IMEM_T_CASE3_ADDR 0x80e00000

#define DIR_IMEMDMEM_TO_SDRAM 0
#define DIR_SDRAM_TO_IMEMDMEM 1





//#define READ_FLUSH_OCP_WRITE

unsigned int data_patterns[] = { 0xFFFFFFFF, 0x00000000, 0xAAAAAAAA, 0x55555555, 0x01234567, 0x89ABCDEF};

void cpu_do_nothing_4k(void);
void cpu_do_nothing_8k(void);
void cpu_do_nothing_32k(void);
void mem_benchmark(void);


void enable_CP3(void)
{
	__write_32bit_c0_register(CP0_STATUS, 0, __read_32bit_c0_register(CP0_STATUS, 0)|0x80000000);
}

void imem_read(unsigned int* mem_addr, unsigned int* imem_addr, unsigned int length)
{
	unsigned long long temp64bit;

	for ( ; length > 0; length -=8) {
		__asm__ __volatile__ (
		        "cache 6, 0(%1)\n\t"
		        "ssnop\n\t"
		        "mfc0 %M0,$29\n\t"
		        "mfc0 %L0,$28\n\t"
		        : "=r"(temp64bit)
		        : "r"(imem_addr)
		);
		*mem_addr++=(unsigned int)temp64bit;
		*mem_addr++=(unsigned int)(temp64bit>>32);
		imem_addr +=2;
	}
}

void on_Wbufmg(void)
{
	int i;
	i=__read_32bit_c0_register(CP0_CCTL, 0);

	__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_WBUFMG_ON);

	__write_32bit_c0_register(CP0_CCTL, 0, i);
}

void DCACHE_WBinval( void )
{
	int i;
	i=__read_32bit_c0_register(CP0_CCTL, 0);

	__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_DCACHE_WBINVAL);

	__write_32bit_c0_register(CP0_CCTL, 0, i);

	*(volatile unsigned char*) 0xa0000000;
}

void ICACHE_inval( void )
{
	int i;
	i=__read_32bit_c0_register(CP0_CCTL, 0);

	__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_ICACHE_INVAL);

	__write_32bit_c0_register(CP0_CCTL, 0, i);

	*(volatile unsigned char*) 0xa0000000;
}

void DCACHE_inval( void )
{
	int i;
	i=__read_32bit_c0_register(CP0_CCTL, 0);

	__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_DCACHE_WBINVAL);

	__write_32bit_c0_register(CP0_CCTL, 0, i);

	*(volatile unsigned char*) 0xa0000000;
}

void off_L2MEM( void )
{
	int i;
	i=__read_32bit_c0_register(CP0_CCTL, 1);

	__write_32bit_c0_register(CP0_CCTL, 1, i^CCTL_L2MEM_OFF);

	__write_32bit_c0_register(CP0_CCTL, 1, i);
}

void on_L2MEM( void )
{
	int i;
	i=__read_32bit_c0_register(CP0_CCTL, 1);

	__write_32bit_c0_register(CP0_CCTL, 1, i^CCTL_L2MEM_ON);

	__write_32bit_c0_register(CP0_CCTL, 1, i);
}

void set_L2MEM_range(unsigned int l2mem_start, unsigned int l2mem_end)
{

	l2mem_start = l2mem_start & 0x1fffffff;/* to physic */
	l2mem_end = l2mem_end & 0x1fffffff;/* to physic */

	write_32bit_cp3_general_register(CP3_L2MEM_BASE, l2mem_start);
	write_32bit_cp3_general_register(CP3_L2MEM_TOP, l2mem_end);
}

void off_DMEM(unsigned int dmem0_1)
{
	int i;
	if (dmem0_1) {
		i=__read_32bit_c0_register(CP0_CCTL, 1);

		__write_32bit_c0_register(CP0_CCTL, 1, i^CCTL_DMEM_OFF);

		__write_32bit_c0_register(CP0_CCTL, 1, i);

	} else {
		i=__read_32bit_c0_register(CP0_CCTL, 0);
		__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_DMEM_OFF);

		__write_32bit_c0_register(CP0_CCTL, 0, i);
	}
}

void on_DMEM(unsigned int dmem0_1)
{
	int i;
	if (dmem0_1) {
		i=__read_32bit_c0_register(CP0_CCTL, 1);
		__write_32bit_c0_register(CP0_CCTL, 1, i^CCTL_DMEM_ON);
		__write_32bit_c0_register(CP0_CCTL, 1, i);
	} else {
		i=__read_32bit_c0_register(CP0_CCTL, 0);
		__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_DMEM_ON);
		__write_32bit_c0_register(CP0_CCTL, 0, i);
	}
}
/* dmem0_1 : 0=> DMEM0, 1=> DMEM1  */
void set_DMEM_range(unsigned int dmem0_1, unsigned int dmem_start, unsigned int dmem_end)
{

	dmem_start = dmem_start & 0x1fffffff;/* to physic */
	dmem_end = dmem_end & 0x1fffffff;/* to physic */
	int i;

	if (dmem0_1) { // dmem 1
		write_32bit_cp3_general_register(CP3_DWBASE1, dmem_start);
		write_32bit_cp3_general_register(CP3_DWTOP1, dmem_end);
	} else {
		write_32bit_cp3_general_register(CP3_DWBASE, dmem_start);
		write_32bit_cp3_general_register(CP3_DWTOP, dmem_end);

	}

}

/* imem0_1 : 0=> IMEM0, 1=> IMEM1  */
void set_and_fill_IMEM(unsigned int imem0_1, unsigned int imem_start, unsigned int imem_end)
{

	imem_start = imem_start & 0x1fffffff;/* to physic */
	imem_end = imem_end & 0x1fffffff;/* to physic */
	int i;

	if (imem0_1) { // imem 1
		i=__read_32bit_c0_register(CP0_CCTL, 1);

		__write_32bit_c0_register(CP0_CCTL, 1, i^CCTL_IMEM_OFF);

		__write_32bit_c0_register(CP0_CCTL, 1, i);


		write_32bit_cp3_general_register(CP3_IWBASE1, imem_start);
		write_32bit_cp3_general_register(CP3_IWTOP1, imem_end);

		__write_32bit_c0_register(CP0_CCTL, 1, i^CCTL_IMEM_FILL);
		__write_32bit_c0_register(CP0_CCTL, 1, i);

	} else {
		i=__read_32bit_c0_register(CP0_CCTL, 0);

		__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_IMEM_OFF);

		__write_32bit_c0_register(CP0_CCTL, 0, i);
		write_32bit_cp3_general_register(CP3_IWBASE, imem_start);
		write_32bit_cp3_general_register(CP3_IWTOP, imem_end);
		__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_IMEM_FILL);
		__write_32bit_c0_register(CP0_CCTL, 0, i);
	}

}

void off_IMEM(unsigned int imem0_1)
{
	int i;
	if (imem0_1) {
		i=__read_32bit_c0_register(CP0_CCTL, 1);

		__write_32bit_c0_register(CP0_CCTL, 1, i^CCTL_IMEM_OFF);

		__write_32bit_c0_register(CP0_CCTL, 1, i);

	} else {
		i=__read_32bit_c0_register(CP0_CCTL, 0);

		__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_IMEM_OFF);

		__write_32bit_c0_register(CP0_CCTL, 0, i);
	}
}

void on_IMEM(unsigned int imem0_1)
{
	int i;
	if (imem0_1) {
		i=__read_32bit_c0_register(CP0_CCTL, 1);
		__write_32bit_c0_register(CP0_CCTL, 1, i^CCTL_IMEM_ON);
		__write_32bit_c0_register(CP0_CCTL, 1, i);
	} else {
		i=__read_32bit_c0_register(CP0_CCTL, 0);
		__write_32bit_c0_register(CP0_CCTL, 0, i^CCTL_IMEM_ON);
		__write_32bit_c0_register(CP0_CCTL, 0, i);
	}
}


int imem_test_case1(int type)
{
	unsigned int *uc_ins_addr, *uc_dist, *imem_cache_read_addr;
	unsigned int tmp;
	int i, test_j, j, k;
	int test_mode = 0; /* 0:STORE_FORWARD_MODE or 1:ACCELERATION_MODE */
	int target =4; /* SEL_IMEM0 0, SEL_IMEM1 1, SEL_DMEM0 2, SEL_DMEM1 3, SEL_L2MEM 4 */
	int size;
	int pattern;
	int test_round=20, status=PASS;
	unsigned long long cpu_fill_cycle, dma_fill_cycle, dma_out_cycle, now_cycle, pre_cycle;
		test_round=16;
	for (k=0; k<test_round ; k++) {
		test_mode = k&1;
		//target = (k>>1)%5;
		//pattern = k/10;
			target = (k>>1)&3;
			pattern = k>>3;
		dprintf("mode=%d, target=%d\n", test_mode, target);
		change_cp3_con_control0(0xff, CP3_COUNT_STOP);
		write_32bit_cp3_general_register(CP3_MONCNT0LO, 0);
		write_32bit_cp3_general_register(CP3_MONCNT0HI, 0);
		change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);
		memset((unsigned char *)MEM_TEST_ADDR, 0, IMEM0_SIZE);
	//dprintf("(line=%d)", __LINE__);
		*(volatile unsigned char*) 0xa0000000;
	//dprintf("(line=%d)", __LINE__);
		if (target < 2) {
			size = ( target==0 ? IMEM0_SIZE : IMEM1_SIZE);
			off_IMEM(target^1);
			if ((target ==1) && (!IMEM1_ENABLE))
				continue;
				pre_cycle=cp3_counter0_get_64bit();
				set_and_fill_IMEM(target, MEM_TEST_ADDR, MEM_TEST_ADDR+size-1);
				now_cycle=cp3_counter0_get_64bit();
				cpu_fill_cycle=now_cycle-pre_cycle;
	//dprintf("(line=%d)", __LINE__);
		} else if (target < 4) {
			if ((target ==3) && (!DMEM1_ENABLE))
				continue;

			size = (target==2 ? DMEM0_SIZE: DMEM1_SIZE);
			off_DMEM((target-2)^1);
			set_DMEM_range(target-2, MEM_TEST_ADDR2, MEM_TEST_ADDR2+size-1);
			on_DMEM(target-2);
		} else {
			dprintf("\n\t%s %d: %d, skip l2mem test\n", \
			             __FUNCTION__, __LINE__, k);
			if (!L2MEM_ENABLE)
				continue;
			size = L2MEM_SIZE;
			off_DMEM(0); off_DMEM(1);
			set_L2MEM_range(MEM_TEST_ADDR2, MEM_TEST_ADDR2+size-1);
			on_L2MEM();
		}
		uc_ins_addr = TO_UNCACHED_ADDR(MEM_TEST_ADDR);
		uc_dist = TO_UNCACHED_ADDR(MEM_TEST_ADDR1);
		imem_cache_read_addr = TO_UNCACHED_ADDR(MEM_TEST_ADDR2);
		//imem_cache_read_addr = MEM_TEST_ADDR2;
		now_cycle=cp3_counter0_get_64bit();
		//dprintf("cycle:%X%08X, {%d}\n", (unsigned int)(now_cycle>>32), (unsigned int)(now_cycle), __LINE__);
		for (test_j=0; test_j<6 ; test_j++) {

			for (i=0; i < size/4; i++) {
				uc_ins_addr[i] = 0xFFFFFFFF;
			}
			now_cycle=cp3_counter0_get_64bit();
			//dprintf("cycle:%X%08X, {%d}\n", (unsigned int)(now_cycle>>32), (unsigned int)(now_cycle), __LINE__);

			for (i=0; i < size/4; i++) {
				if (pattern)
					uc_ins_addr[i] = ((i<<2) | (1<<i) ) ^ (0x5aa55555<<test_j);
				else
					uc_ins_addr[i] = data_patterns[test_j];
			}
			now_cycle=cp3_counter0_get_64bit();
			//dprintf("cycle:%X%08X, {%d}\n", (unsigned int)(now_cycle>>32), (unsigned int)(now_cycle), __LINE__);
#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
			tmp = uc_ins_addr[i-1];
#else
			*((volatile unsigned int *)(0xB8001030)) = \
			                *((volatile unsigned int *)(0xB8001030)) | 0x80000000;

			while ((*((volatile unsigned int *)(0xB8001030))&0x80000000) != 0);
#endif

			now_cycle=cp3_counter0_get_64bit();
			//dprintf("cycle:%X%08X, {%d}\n", (unsigned int)(now_cycle>>32), (unsigned int)(now_cycle), __LINE__);
			pre_cycle=cp3_counter0_get_64bit();
			imemDmemDMA((unsigned int)uc_ins_addr, uc_ins_addr, uc_ins_addr, \
			            size, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);
			now_cycle=cp3_counter0_get_64bit();
			dma_fill_cycle=now_cycle-pre_cycle;
			pre_cycle=cp3_counter0_get_64bit();
			/* DMA Data out form MEM into distination */
	//dprintf("(line=%d)", __LINE__);
			imemDmemDMA((unsigned int)uc_dist, uc_ins_addr, uc_ins_addr, \
			            size, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);
			now_cycle=cp3_counter0_get_64bit();
			dma_out_cycle=now_cycle-pre_cycle;
			dprintf("cpu_fill:%X%08X, dma_fill:%X%08X, dma_out:%X%08X\n", (unsigned int)(cpu_fill_cycle>>32),
			        (unsigned int)(cpu_fill_cycle),  (unsigned int)(dma_fill_cycle>>32),  (unsigned int)(dma_fill_cycle),
				(unsigned int)(dma_out_cycle>>32),  (unsigned int)(dma_out_cycle));
			if (target < 2)
				imem_read(MEM_TEST_ADDR2 , MEM_TEST_ADDR, size);

			now_cycle=cp3_counter0_get_64bit();
			//dprintf("cycle:%X%08X, {%d}\n", (unsigned int)(now_cycle>>32), (unsigned int)(now_cycle), __LINE__);

			for (j=0; j<(size/4); j++) {
				if (uc_ins_addr[j] != uc_dist[j]) {
					dprintf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
					        __FUNCTION__, __LINE__, j, uc_ins_addr[j], j, uc_dist[j]);
					goto test_fail;
				}
			}
			now_cycle=cp3_counter0_get_64bit();
			//dprintf("cycle:%X%08X, {%d}\n", (unsigned int)(now_cycle>>32), (unsigned int)(now_cycle), __LINE__);
			if (target == 4)
				DCACHE_WBinval();

			for (j=0; j<(size/4); j++) {
				if (uc_ins_addr[j] != imem_cache_read_addr[j]) {
					dprintf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
					        __FUNCTION__, __LINE__, j, uc_ins_addr[j], j, imem_cache_read_addr[j]);
					goto test_fail;
				}
			}
			now_cycle=cp3_counter0_get_64bit();
			//dprintf("cycle:%X%08X, {%d}\n", (unsigned int)(now_cycle>>32), (unsigned int)(now_cycle), __LINE__);
			dprintf("src=%x,dsr=%x,cache=%x,testok\n", uc_ins_addr[0], uc_dist[0], imem_cache_read_addr[0]);
		}
	}
	goto test_pass;
test_fail:
	off_IMEM(0);
	off_IMEM(1);
	off_DMEM(0);
	off_DMEM(1);
	off_L2MEM();
	return 0;
	//status=FAIL;
test_pass:
	off_IMEM(0);
	off_IMEM(1);
	off_DMEM(0);
	off_DMEM(1);
	off_L2MEM();
	return 1;
	//return status;
}


/* dma 2x imem block in one time */
int imem_test_case2(int type)
{
	unsigned int *uc_ins_addr, *uc_dist, *imem_cache_read_addr;
	unsigned int tmp;
	int i, test_j, j, k;
	int test_mode = 0; /* 0:STORE_FORWARD_MODE or 1:ACCELERATION_MODE */
	int target =4; /* SEL_IMEM0 0, SEL_IMEM1 1, SEL_DMEM0 2, SEL_DMEM1 3, SEL_L2MEM 4 */
	int size;
	int pattern;
	unsigned long long cpu_fill_cycle, dma_fill_cycle, dma_out_cycle, now_cycle, pre_cycle;
	int status=PASS;

	change_cp3_con_control0(0xff, CP3_COUNT_STOP);
	write_32bit_cp3_general_register(CP3_MONCNT0LO, 0);
	write_32bit_cp3_general_register(CP3_MONCNT0HI, 0);
	change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);
	for (k=0; k<8 ; k++) {
		test_mode = k&1;
		target = k&2;
		pattern = k>>2;
		dprintf("mode=%d, target=%d\n", test_mode, target);
		memset((unsigned char *)MEM_TEST_ADDR, 0, IMEM0_SIZE+IMEM1_SIZE);

		*(volatile unsigned char*) 0xa0000000;

		if (target < 2) {
			size = IMEM0_SIZE+IMEM1_SIZE;
			if (!IMEM1_ENABLE)
				continue;
			off_IMEM(target^1);
			pre_cycle=cp3_counter0_get_64bit();
			set_and_fill_IMEM(target, MEM_TEST_ADDR, MEM_TEST_ADDR+IMEM0_SIZE-1);
			set_and_fill_IMEM(target+1, MEM_TEST_ADDR+IMEM0_SIZE, MEM_TEST_ADDR+IMEM0_SIZE+IMEM1_SIZE-1);
				now_cycle=cp3_counter0_get_64bit();
				cpu_fill_cycle=now_cycle-pre_cycle;
		} else if (target < 4) {
			size = DMEM0_SIZE+ DMEM1_SIZE;
			if (!DMEM1_ENABLE)
				continue;
			off_DMEM((target-2)^1);
			set_DMEM_range(target-2, MEM_TEST_ADDR2, MEM_TEST_ADDR2+DMEM0_SIZE-1);
			set_DMEM_range(target-2+1, MEM_TEST_ADDR2+DMEM0_SIZE, MEM_TEST_ADDR2+DMEM0_SIZE+DMEM1_SIZE-1);
			on_DMEM(target-2);
			on_DMEM(target-2+1);
		}
		uc_ins_addr = TO_UNCACHED_ADDR(MEM_TEST_ADDR);
		uc_dist = TO_UNCACHED_ADDR(MEM_TEST_ADDR1);
		imem_cache_read_addr = TO_UNCACHED_ADDR(MEM_TEST_ADDR2);
		//imem_cache_read_addr = MEM_TEST_ADDR2;

		for (test_j=0; test_j<6 ; test_j++) {

			for (i=0; i < size/4; i++) {
				uc_ins_addr[i] = 0xFFFFFFFF;
			}

			for (i=0; i < size/4; i++) {
				if (pattern)
					uc_ins_addr[i] = ((i<<2) | (1<<i) ) ^ (0x5aa55555<<test_j);
				else
					uc_ins_addr[i] = data_patterns[test_j];
			}

#ifdef READ_FLUSH_OCP_WRITE /* flush OCP pending write */
			tmp = uc_ins_addr[i-1];
#else
			*((volatile unsigned int *)(0xB8001030)) = \
			                *((volatile unsigned int *)(0xB8001030)) | 0x80000000;

			while ((*((volatile unsigned int *)(0xB8001030))&0x80000000) != 0);
#endif

			pre_cycle=cp3_counter0_get_64bit();
			imemDmemDMA((unsigned int)uc_ins_addr, uc_ins_addr, uc_ins_addr, \
			            size, DIR_SDRAM_TO_IMEMDMEM, target, test_mode);
			now_cycle=cp3_counter0_get_64bit();
			dma_fill_cycle=now_cycle-pre_cycle;

			pre_cycle=cp3_counter0_get_64bit();
			/* DMA Data out form MEM into distination */
			imemDmemDMA((unsigned int)uc_dist, uc_ins_addr, uc_ins_addr, \
			            size, DIR_IMEMDMEM_TO_SDRAM, target, test_mode);
			now_cycle=cp3_counter0_get_64bit();
			dma_out_cycle=now_cycle-pre_cycle;

			if (target < 2)
				imem_read(MEM_TEST_ADDR2 , MEM_TEST_ADDR, size);

			for (j=0; j<(size/4); j++) {
				if (uc_ins_addr[j] != uc_dist[j]) {
					dprintf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
					        __FUNCTION__, __LINE__, j, uc_ins_addr[j], j, uc_dist[j]);
					goto test_fail;
				}
			}

			if (target == 4)
				DCACHE_WBinval();

			for (j=0; j<(size/4); j++) {
				if (uc_ins_addr[j] != imem_cache_read_addr[j]) {
					dprintf("\n\t%s %d:src[%d](0x%x) != dist[%d](0x%x)\n", \
					        __FUNCTION__, __LINE__, j, uc_ins_addr[j], j, imem_cache_read_addr[j]);
					goto test_fail;
				}
			}
			dprintf("cpu_fill:%X%08X, dma_fill:%X%08X, dma_out:%X%08X\n", (unsigned int)(cpu_fill_cycle>>32),
			        (unsigned int)(cpu_fill_cycle),  (unsigned int)(dma_fill_cycle>>32),  (unsigned int)(dma_fill_cycle),
				(unsigned int)(dma_out_cycle>>32),  (unsigned int)(dma_out_cycle));
			dprintf("src=%x,dsr=%x,cache=%x,testok\n", uc_ins_addr[0], uc_dist[0], imem_cache_read_addr[0]);
		}
	}
	goto test_pass;

test_fail:
	off_IMEM(0);
	off_IMEM(1);
	off_DMEM(0);
	off_DMEM(1);
	//status=FAIL;
	return 0;
test_pass:
	off_IMEM(0);
	off_IMEM(1);
	off_DMEM(0);
	off_DMEM(1);
	//return status;
	return 1;
}


extern unsigned long __load_start_IMEM;
int imem_test_case3(int type)
{
	unsigned int iram_start_addr;
	unsigned int iram_end_addr;
	unsigned int imem_size;
	int imem_test01=1, status=PASS;
	int i;
	unsigned long long imem_cycle, noimem_cycle;
	void (*function_pointer)(void);
	int *src_addr;

	enable_CP3();

	for (i=0; i<2; i++) {
		imem_test01=i;
		ICACHE_inval();
	if ((i==1) && (!IMEM1_ENABLE))
		continue;
	iram_start_addr = MEM_IMEM_T_CASE3_ADDR;
			function_pointer = (void (*)(void))(MEM_IMEM_T_CASE3_ADDR);
	imem_size = imem_test01==0? IMEM0_SIZE : IMEM1_SIZE;
	src_addr = imem_test01==0? &cpu_do_nothing_8k : &cpu_do_nothing_8k;
	iram_end_addr = iram_start_addr + imem_size-1;
				memcpy(MEM_IMEM_T_CASE3_ADDR, src_addr, imem_size);
			DCACHE_WBinval();

		change_cp3_con_control0(0xff, CP3_COUNT_STOP);
		write_32bit_cp3_general_register(CP3_MONCNT0LO, 0);
		write_32bit_cp3_general_register(CP3_MONCNT0HI, 0);
		set_and_fill_IMEM(imem_test01, iram_start_addr, iram_end_addr);

	change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);
			(*function_pointer)();
	change_cp3_con_control0(0xff, CP3_COUNT_STOP);
	imem_cycle=cp3_counter0_get_64bit();
	off_IMEM(imem_test01);

		write_32bit_cp3_general_register(CP3_MONCNT0LO, 0);
		write_32bit_cp3_general_register(CP3_MONCNT0HI, 0);

	change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);
			(*function_pointer)();
	change_cp3_con_control0(0xff, CP3_COUNT_STOP);
	noimem_cycle=cp3_counter0_get_64bit();


		dprintf("imem:%X%08X, noimem:%X%08X\n", (unsigned int)(imem_cycle>>32),
		        (unsigned int)(imem_cycle),  (unsigned int)(noimem_cycle>>32),  (unsigned int)(noimem_cycle));
		if (imem_cycle>(noimem_cycle>>2))
			status=FAIL;
	}

	return status;
}

static char test_l2mem_data[1024*8];

#define TEST_DATA1 0x11223344

#define TEST_DATA2 0x667755aa

#define TEST_DATA3 0x88aa9955

#define ALIGN_4K_ADDR(x) ((((unsigned int)x)+4095)& ~4095)

int l2mem_test(int type)
{
	unsigned int l2mem_align; 
	unsigned int uncache_l2mem_align;
	int i, j, status;
	unsigned long flags;

	status=PASS;
	l2mem_align = ALIGN_4K_ADDR(test_l2mem_data);
	uncache_l2mem_align = l2mem_align|0x20000000;

	//write_32bit_cp3_general_register(CP3_L2MEM_BASE, l2mem_align);
	//write_32bit_cp3_general_register(CP3_L2MEM_TOP, l2mem_align+4095);

	set_L2MEM_range(l2mem_align, l2mem_align+4095);
	dprintf("l2mem_base=%x, l2mem_top=%x\n", l2mem_align, l2mem_align+4095);

	on_L2MEM();

	//save_and_cli(flags);

	DCACHE_WBinval();

	/* write data to l2mem */
	for (i=1023 ; i>=0 ; i--)
		*(unsigned int *)(l2mem_align + i*4) = TEST_DATA1;

	/* read data for l2mem to d-cache */
	for (i=1023 ; i>=0 ; i--)
		j=*(unsigned int *)(l2mem_align + i*4);

	/* write data to d-cache */
	for (i=1023 ; i>=0 ; i--)
		*(unsigned int *)(l2mem_align + i*4) = TEST_DATA2;

	/* write back & invalid d-cache */
	DCACHE_WBinval();

	/* read verify l2-mem VERIFY@1 */
	for (i=1023 ; i>=0 ; i--) {
		if (*(unsigned int *)(l2mem_align + i*4) != TEST_DATA2) {
			dprintf("ERROR");
			status = FAIL;
		}
	}

	/* write data to d-cache */
	for (i=1023 ; i>=0 ; i--)
		*(unsigned int *)(l2mem_align + i*4) = TEST_DATA3;

	/* uncache read, flush d-cache and read l2mem */
	for (i=1023 ; i>=0 ; i--) {
		if (*(unsigned int *)(uncache_l2mem_align + i*4) != TEST_DATA3) {
			dprintf("ERROR");
			status = FAIL;
		}
	}
	dprintf("data=%x, read=%x\n", TEST_DATA3, *(unsigned int *)uncache_l2mem_align);

	//restore_flags(flags);
	return status;
}

/* uncache load/store twin word test(8-byte) */
int twinword_load_store_test( void )
{
	int i, status=PASS;
	unsigned int *uc_src, *uc_dist;
	
	uc_src=TO_UNCACHED_ADDR(MEM_TEST_ADDR);
	uc_dist=TO_UNCACHED_ADDR(MEM_TEST_ADDR1);
	memset64s( uc_src, TEST_DATA1, 1024);
	memcpy64s( uc_dist, uc_src, 1024);
	for (i=0; i<1024*2/4 ; i++) {
		if (uc_src[i]!=TEST_DATA1) {
			dprintf("ERROR(%x,%x)", TEST_DATA1, uc_src[i]);
			status=FAIL;
		}
		if (uc_src[i]!=uc_dist[i]) {
			dprintf("ERROR1(%x,%x)", uc_src[i], uc_dist[i]);
			status=FAIL;
		}

	}
	dprintf("1src=%x,%x;dist=%x,%x\n", uc_src[0], uc_src[1], uc_dist[0], uc_dist[1]);

	memset64s( uc_src, TEST_DATA2, 1024);
	memcpy64s( uc_dist, uc_src, 1024);
	for (i=0; i<1024*2/4 ; i++) {
		if (uc_src[i]!=TEST_DATA2) {
			dprintf("ERROR(%x,%x)", TEST_DATA2, uc_src[i]);
			status=FAIL;
		}
		if (uc_src[i]!=uc_dist[i]) {
			dprintf("ERROR1(%x,%x)", uc_src[i], uc_dist[i]);
			status=FAIL;
		}

	}
	dprintf("2src=%x,%x;dist=%x,%x\n", uc_src[0], uc_src[1], uc_dist[0], uc_dist[1]);

	return status;
}

int unalign_load_store_test(void)
{
	short random_seed=0;
	int i, j, status;
	unsigned int *uc_src, *uc_dist;
	short *pointer;
	int test_mode;/*[src, dist] 0:uncache, 1:cache*/


	for (j=0; j<4*5; j++) {
		test_mode=j&0x3;
		if (test_mode&2)
			pointer=MEM_TEST_ADDR;
		else
			pointer=TO_UNCACHED_ADDR(MEM_TEST_ADDR);
		for (i=0; i<1024*2/2; i++) {
			random_seed=(short)(random_seed* 31821+ 13849L);
			pointer[i]=random_seed;
		}
		if (test_mode&2)
			uc_src=MEM_TEST_ADDR;
		else
			uc_src=TO_UNCACHED_ADDR(MEM_TEST_ADDR);
		if (test_mode&1)
			uc_dist=MEM_TEST_ADDR1;
		else
			uc_dist=TO_UNCACHED_ADDR(MEM_TEST_ADDR1);

		memcpy_unaligns( uc_dist, uc_src, 1024);
		for (i=0; i<1024*2/4 ; i++) {
			if (uc_src[i]!=uc_dist[i]) {
				dprintf("ERROR%d(%x,%x)", i, uc_src[i], uc_dist[i]);
				status=FAIL;
			}
		}
		dprintf("%dsrc=%x,%x;dist=%x,%x\n", j, uc_src[0], uc_src[1], uc_dist[0], uc_dist[1]);
	}

	return status;

}


void imem_test( void )
{
	int chip_type=0;
	unsigned long flags;
	int cp0_count;

__write_32bit_c0_register(CP0_COUNT, 0, 0);

	enable_CP3(); //
	save_and_cli(flags); //JSW
cp0_count=__read_32bit_c0_register(CP0_COUNT, 0);
dprintf("{%d, %08X}\n", __LINE__, cp0_count);
	imem_test_case1(chip_type); //
cp0_count=__read_32bit_c0_register(CP0_COUNT, 0);
dprintf("{%d, %08X}\n", __LINE__, cp0_count);
	imem_test_case2(chip_type);//
cp0_count=__read_32bit_c0_register(CP0_COUNT, 0);
dprintf("{%d, %08X}\n", __LINE__, cp0_count);
	imem_test_case3(chip_type);
cp0_count=__read_32bit_c0_register(CP0_COUNT, 0);
dprintf("{%d, %08X}\n", __LINE__, cp0_count);
	if (L2MEM_ENABLE)
		l2mem_test(chip_type);
cp0_count=__read_32bit_c0_register(CP0_COUNT, 0);
dprintf("{%d, %08X}\n", __LINE__, cp0_count);
	twinword_load_store_test();
cp0_count=__read_32bit_c0_register(CP0_COUNT, 0);
dprintf("{%d, %08X}\n", __LINE__, cp0_count);
	unalign_load_store_test();
cp0_count=__read_32bit_c0_register(CP0_COUNT, 0);
dprintf("{%d, %08X}\n", __LINE__, cp0_count);
	mem_benchmark();
	restore_flags(flags);    //
}







__attribute__ ((section (".test96diram"))) void cpu_do_nothing_4k(void)
{
#if 0 /* nop and nop is single issue */
	//998xnop
	asm volatile (
/*1*/	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" //20x nop
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
/*11*/	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" //20x nop
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
/*21*/	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" //20x nop
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
/*31*/	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" //20x nop
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
/*41*/	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t" //20x nop
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t"
	        "nop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\tnop\n\t");
#endif

	//499x ("nop" "lui $2, 1")
	asm volatile (
/*1*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t" //10x ("nop" "lui $2, 1")
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*11*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*21*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*31*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*41*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t");


}

void cpu_do_nothing_8k(void)
{
	//1023x ("nop" "lui $2, 1")
	asm volatile (
/*1*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t" //10x ("nop" "lui $2, 1")
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*11*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*21*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*31*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*41*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*51*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*61*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*71*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*81*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*91*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*100*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t");


}

void cpu_do_nothing_32k(void)
{
	//4095x ("nop" "lui $2, 1")
	asm volatile (
/*1*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t" //10x ("nop" "lui $2, 1")
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*11*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*21*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*31*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*41*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*51*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*61*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*71*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*81*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*91*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*101*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*111*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*121*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*131*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*141*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*151*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*161*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*171*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*181*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*191*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*201*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*211*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*221*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*231*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*241*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*251*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*261*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*271*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*281*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*291*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*301*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*311*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*321*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*331*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*341*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*351*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*361*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*371*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*381*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*391*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
/*401*/	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t"
	        "nop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\tnop\n\tlui $2, 1\n\t");


}

void write_buf_merge(void)
{
	unsigned long long wbufmg_cnt;
	enable_CP3();
	change_cp3_con_control0(0xff<<8, CP3_COUNT_STOP<<8);
	wbufmg_cnt = cp3_counter1_get_64bit();

	write_32bit_cp3_general_register(CP3_MONCNT1LO, 0);
	write_32bit_cp3_general_register(CP3_MONCNT1HI, 0);

	change_cp3_con_control0(0xff<<8, CP3_COUNT_WBUF_MERGE<<8);

	on_Wbufmg();

	dprintf("merge_cnt=%x", (unsigned int)wbufmg_cnt);

}


void memwatch(char* vaddr, unsigned int flag)
{

	vaddr = (char*) (((unsigned int)vaddr) &0xfffffff8);
	flag &=0x7;
	__write_32bit_c0_register(CP0_WATCHLO, 0, (((unsigned int)vaddr)|flag));
	__write_32bit_c0_register(CP0_WATCHHI, 0, WATCHHI_GLOBAL);
	__write_32bit_lxc0_register(LX0_WMPCTL, 0, (WMPCTL_ENTRYENABLE_0| WMPCTL_WATCHMODE| WMPCTL_KERNELENABLE));

}

#define FLASH_TEST_ADDR 0x9fc10000
#define FLASH_TEST_SIZE (2<<20) /* 2MB  */

void mem_benchmark( void )
{
	int i;
	unsigned long long test_start_cycle[8][2];
	unsigned long long test_end_cycle[8][2];

	for (i=0 ; i<2 ; i++) {
		DCACHE_WBinval();
		change_cp3_con_control0(0xff, CP3_COUNT_STOP);
		write_32bit_cp3_general_register(CP3_MONCNT0LO, 0);
		write_32bit_cp3_general_register(CP3_MONCNT0HI, 0);
		change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);

		test_start_cycle[0][i]=cp3_counter0_get_64bit();
		short_r(TO_UNCACHED_ADDR(FLASH_TEST_ADDR), FLASH_TEST_SIZE);
		test_end_cycle[0][i]=cp3_counter0_get_64bit()-test_start_cycle[0][i];

		test_start_cycle[1][i]=cp3_counter0_get_64bit();
		word_r(TO_UNCACHED_ADDR(FLASH_TEST_ADDR), FLASH_TEST_SIZE);
		test_end_cycle[1][i]=cp3_counter0_get_64bit()-test_start_cycle[1][i];

		test_start_cycle[2][i]=cp3_counter0_get_64bit();
		tword_r(TO_UNCACHED_ADDR(FLASH_TEST_ADDR), FLASH_TEST_SIZE);
		test_end_cycle[2][i]=cp3_counter0_get_64bit()-test_start_cycle[2][i];

		test_start_cycle[3][i]=cp3_counter0_get_64bit();
		short_r(FLASH_TEST_ADDR, FLASH_TEST_SIZE);
		test_end_cycle[3][i]=cp3_counter0_get_64bit()-test_start_cycle[3][i];

		DCACHE_WBinval();

		test_start_cycle[4][i]=cp3_counter0_get_64bit();
		word_r(FLASH_TEST_ADDR, FLASH_TEST_SIZE);
		test_end_cycle[4][i]=cp3_counter0_get_64bit()-test_start_cycle[4][i];

		DCACHE_WBinval();

		test_start_cycle[5][i]=cp3_counter0_get_64bit();
		tword_r(FLASH_TEST_ADDR, FLASH_TEST_SIZE);
		test_end_cycle[5][i]=cp3_counter0_get_64bit()-test_start_cycle[5][i];
	}


	for (i=0 ; i<2*6 ; i++) {
		dprintf("{%d, %X%08X}\n", i, test_end_cycle[i>>1][i&1]);
	}

}

