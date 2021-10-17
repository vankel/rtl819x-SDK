#ifndef __MONITOR_COMMANDS_H__
#define __MONITOR_COMMANDS_H__


#define COMMANDS_TABLE_EX	\
	{ "UART1",0,uart1_test , "UART1: uart1 test" },						\
	{ "SRAM",4,sram_conf_entry , "sram [<num> <addr> <size> <base>]" },	\
	{ "EN5181",0,enable5181 , "EN5181: enable 5181" },					\
	{ "ZONE",3,zone_conf_entry , "zone [<num> <offset> <max>]" },		\
	{ "BOOT2",1,boot2_5181_entry, "boot2 [<32M|57M> {<SRAM>|<DRAM> <dram_addr>}]" }, \
	{ "MP5181",1,boot_5181_mp_entry, "mp5181: 5181 mp test" }, \
	{ "SHM",0,shm_test_entry, "SHM: shm test" }, \
	{ "MUTEX",0,mutex_test_entry, "MUTEX: mutex test" }, 

extern void uart1_test(int argc, char* argv[]);
extern void sram_conf_entry(int argc, char* argv[]);
extern void enable5181(int argc, char* argv[]);
extern void zone_conf_entry(int argc, char* argv[]);
extern void boot2_5181_entry(int argc, char* argv[]);
extern void boot_5181_mp_entry(int argc, char* argv[]);

//#define RLX5181_MP_TEST		//select "5181-uart0-show_5181_BOOT_OK.bin" in monitor/5181.S
#define RLX5181_MP_ECOS_BOOT	//select "5181_uart0_eCos_nfjrom_mp" in monitor/5181.S

//#define RLX5181_USE_UART0
#define RLX5181_USE_UART1
//#define RLX5181_USE_UART2

#endif 

