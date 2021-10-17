#include "monitor_commands.h"

// ======================= Instruction OP code ===================== 

static const unsigned long opcode_jmp_0x8000_0000[] = {
	0x3c088000,    // lui t0,0x8000
	0x01000008,    // jr  t0
	0x00000000,    // nop
};

static const unsigned long opcode_jmp_0x8000_0100[] = {
	0x3c088000,    // lui t0,0x8000
	0x35080100,    // ori t0,t0,0x0100
	0x01000008,    // jr  t0
	0x00000000,    // nop
};

static const unsigned long opcode_jmp_0x80c0_0000[] = {
	0x3c0880c0,    // lui t0,0x80c0
	0x35080000,    // ori t0,t0,0x0000
	0x01000008,    // jr  t0
	0x00000000,    // nop
};
static const unsigned long opcode_uart0_show_A[] = {
	0x3c04b800,    // lui a0,0xb800
	0x34842000,    // ori a0,a0,0x2000
	0x3c054141,    // lui a1,0x4141
	0x34a54141,    // ori a1,a1,0x4141
	0xac850000,    // sw  a1,0(a0)
	0x1000ffff,    // b   <stop>
	0x00000000,    // nop
};

static const unsigned long opcode_uart0_show_B[] = {
	0x3c04b800,    // lui a0,0xb800
	0x34842000,    // ori a0,a0,0x2000
	0x3c054242,    // lui a1,0x4242
	0x34a54242,    // ori a1,a1,0x4242
	0xac850000,    // sw  a1,0(a0)
	0x1000ffff,    // b   <stop>
	0x00000000,    // nop
};

static const unsigned long opcode_uart1_show_A[] = {
	0x3c04b800,    // lui a0,0xb800
	0x34842100,    // ori a0,a0,0x2100
	0x3c054141,    // lui a1,0x4141
	0x34a54141,    // ori a1,a1,0x4141
	0xac850000,    // sw  a1,0(a0)
	0x1000ffff,    // b   <stop>
	0x00000000,    // nop
};

static const unsigned long opcode_uart1_show_B[] = {
	0x3c04b800,    // lui a0,0xb800
	0x34842100,    // ori a0,a0,0x2100
	0x3c054242,    // lui a1,0x4242
	0x34a54242,    // ori a1,a1,0x4242
	0xac850000,    // sw  a1,0(a0)
	0x1000ffff,    // b   <stop>
	0x00000000,    // nop
};

static const unsigned long opcode_uart2_show_A[] = {
	0x3c04b800,    // lui a0,0xb800
	0x34842200,    // ori a0,a0,0x2200
	0x3c054141,    // lui a1,0x4141
	0x34a54141,    // ori a1,a1,0x4141
	0xac850000,    // sw  a1,0(a0)
	0x1000ffff,    // b   <stop>
	0x00000000,    // nop
};

static const unsigned long opcode_uart2_show_B[] = {
	0x3c04b800,    // lui a0,0xb800
	0x34842200,    // ori a0,a0,0x2200
	0x3c054242,    // lui a1,0x4242
	0x34a54242,    // ori a1,a1,0x4242
	0xac850000,    // sw  a1,0(a0)
	0x1000ffff,    // b   <stop>
	0x00000000,    // nop
};

#define TEST_5181_PROGRAM_DRAM_ENTRY	0x80000000

// ======================= UART1 ===================== 

void uart1_test(int argc, char* argv[])
{ 
#if 0
	extern int lx5181_binary_start;
	extern int lx5181_binary_end;
	typedef void ( *fn_uart1_t )( void );
	fn_uart1_t fn_uart1_test;
#endif
	unsigned long div;

#if 1
#if 0
	//pin-mux sel-2
	*( ( volatile unsigned long * )0xB8000104 ) = 0x22222;
#else
	//pin-mux sel-4, UART1 config for 8198C V100 demo board.
	*( ( volatile unsigned long * )0xB800010c ) = 0x00011FB8;
#endif
#endif

	// uart 1 init, 
	*( ( volatile unsigned long * )0xB8002104 ) = 0x00000000;
	*( ( volatile unsigned long * )0xB800210C ) = 0x83000000;
#if 0	// clk 25M, uart=38400 --> div = 0x28
	*( ( volatile unsigned long * )0xB8002100 ) = 0x28000000;
	*( ( volatile unsigned long * )0xB8002104 ) = 0x00000000;
#else	// clk use config, uart=38400
	div = CONFIG_LXBUS_HZ / 16 / 38400;

	*( ( volatile unsigned long * )0xB8002100 ) = ( div & 0xFF ) << 24;
	*( ( volatile unsigned long * )0xB8002104 ) = ( div & 0xFF00 ) << 16;
#endif
	*( ( volatile unsigned long * )0xB800210C ) = 0x03000000;
	*( ( volatile unsigned long * )0xB8002110 ) = 0x03000000;
	*( ( volatile unsigned long * )0xB8002108 ) = 0xC1000000;

	prom_printf( "run uart1 test\n" );

#if 0
	prom_printf( "lx5181 binary @ %x ~ %x\n", &lx5181_binary_start, &lx5181_binary_end );

	fn_uart1_test = ( fn_uart1_t)( ( char * )&lx5181_binary_start + 0x1c );

	prom_printf( "uart1 test @ %x\n", fn_uart1_test );

	( *fn_uart1_test )(); //print A
#else
	*( ( volatile unsigned long * )0xB8002100 ) = 0x41000000; //print A
#endif
}


void uart2_test(int argc, char* argv[])
{ 
	// For 98C 256pin package, uart2 is default for pinmux-2.

	unsigned long div;

	// uart 2 init, 
	*( ( volatile unsigned long * )0xB8002204 ) = 0x00000000;
	*( ( volatile unsigned long * )0xB800220C ) = 0x83000000;
#if 0	// clk 25M, uart=38400 --> div = 0x28
	*( ( volatile unsigned long * )0xB8002200 ) = 0x28000000;
	*( ( volatile unsigned long * )0xB8002204 ) = 0x00000000;
#else	// clk use config, uart=38400
	div = CONFIG_LXBUS_HZ / 16 / 38400;

	*( ( volatile unsigned long * )0xB8002200 ) = ( div & 0xFF ) << 24;
	*( ( volatile unsigned long * )0xB8002204 ) = ( div & 0xFF00 ) << 16;
#endif
	*( ( volatile unsigned long * )0xB800220C ) = 0x03000000;
	*( ( volatile unsigned long * )0xB8002210 ) = 0x03000000;
	*( ( volatile unsigned long * )0xB8002208 ) = 0xC1000000;

	prom_printf( "run uart2 test\n" );

#if 0
	prom_printf( "lx5181 binary @ %x ~ %x\n", &lx5181_binary_start, &lx5181_binary_end );

	fn_uart1_test = ( fn_uart1_t)( ( char * )&lx5181_binary_start + 0x1c );

	prom_printf( "uart1 test @ %x\n", fn_uart1_test );

	( *fn_uart1_test )(); //print A
#else
	*( ( volatile unsigned long * )0xB8002200 ) = 0x41000000; //print A
#endif
}
// ======================= SRAM + UNMAP ===================== 

typedef struct {
	unsigned long *addr;
	unsigned long *size;
	unsigned long *base;
} sram_conf_t;

static sram_conf_t sram_conf[] = {
	// cpu0: 1074k 
	{ 0xB8004000, 0xB8004004, 0xB8004008 },
	{ 0xB8004010, 0xB8004014, 0xB8004018 },
	{ 0xB8004020, 0xB8004024, 0xB8004028 },
	{ 0xB8004030, 0xB8004034, 0xB8004038 },
	// cpu1: 5181
	{ 0xB8004040, 0xB8004044, 0xB8004048 },
	{ 0xB8004050, 0xB8004054, 0xB8004058 },
	{ 0xB8004060, 0xB8004064, 0xB8004068 },
	{ 0xB8004070, 0xB8004074, 0xB8004078 },
};

#define SRAM_CONF_NUM	5

typedef struct {
	unsigned long *addr;
	unsigned long *size;
} unmap_dram_t;

static unmap_dram_t unmap_dram[] = {
	// cpu0 
	{ 0xB8001300, 0xB8001304 }, 
	{ 0xB8001310, 0xB8001314 }, 
	{ 0xB8001320, 0xB8001324 },
	{ 0xB8001330, 0xB8001334 }, 
	// cpu1 
	{ 0xB8001340, 0xB8001344 },
	{ 0xB8001350, 0xB8001354 },
	{ 0xB8001360, 0xB8001364 },
	{ 0xB8001370, 0xB8001374 }, 
};

#define UNMAP_DRAM_NUM	5

static unsigned long decode_size( unsigned long encoded_size )
{
	if( encoded_size < 1 || encoded_size > 9 )
		return 0;
	
	return 256 << ( encoded_size - 1 );
}

static char *size_str( unsigned long size )
{
	static char buf[ 64 ];
	static const char *unit_str[] = {
		"B",
		"KB",
		"MB",
		"GB",
		"TB", 
	};
	#define NUM_OF_UNIT		5
	
	int i;
	int fraction = 0;
	
	for( i = 0; i < NUM_OF_UNIT - 1; i ++ ) {
		if( size < 1024 )
			break;
		
		if( size & 0x000003FF )
			fraction = 1;
		
		size >>= 10;
	}
	
	if( fraction )
		SprintF( buf, "%d.x %s", size, unit_str[ i ] );
	else
		SprintF( buf, "%d %s", size, unit_str[ i ] );
	
	return buf;
}

static void show_sran_conf( int i, sram_conf_t *psram, unmap_dram_t *pumap )
{
	prom_printf( "SRAM[%d]\n", i );
	prom_printf( "	addr(%08X): %08X (%s)\n", psram ->addr, *( psram ->addr ), size_str( *( psram ->addr ) & ~1 ) );
	prom_printf( "	size(%08X): %08X (%s)\n", psram ->size, *( psram ->size ), size_str( decode_size( *psram ->size ) ) );
	prom_printf( "	base(%08X): %08X (%s)\n", psram ->base, *( psram ->base ), size_str( *( psram ->base ) ) );
	
	prom_printf( "UNMAP[%d]\n", i );
	prom_printf( "	addr(%08X): %08X (%s)\n", pumap ->addr, *( pumap ->addr ), size_str( *( pumap ->addr ) & ~1 ) );
	prom_printf( "	size(%08X): %08X (%s)\n", pumap ->size, *( pumap ->size ), size_str( decode_size( *pumap ->size ) ) );
}

void sram_conf_entry(int argc, char* argv[])
{
	int i;
	unsigned long arg_num, arg_addr, arg_size, arg_base;
	sram_conf_t *psram;
	unmap_dram_t *pumap;
	
	
	if( argc != 4 ) {
		
		// dump all sram info 
		
		for( i = 0; i < SRAM_CONF_NUM; i ++ ) {
			
			psram = &sram_conf[ i ];
			pumap = &unmap_dram[ i ];
			
			show_sran_conf( i, psram, pumap );
		}
		
		return;
	}
	
	// regular write "sram [<num> <addr> <size> <base>]"
	arg_num = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	arg_addr = strtoul((const char*)(argv[1]), (char **)NULL, 16);
	arg_size = strtoul((const char*)(argv[2]), (char **)NULL, 16);
	arg_base = strtoul((const char*)(argv[3]), (char **)NULL, 16);
	
	if( arg_num >= SRAM_CONF_NUM ) {
		prom_printf( "number is too large (%u>=%u)\n", arg_num, SRAM_CONF_NUM );
		return;
	}

	psram = &sram_conf[ arg_num ];
	pumap = &unmap_dram[ arg_num ];

	*psram ->addr = arg_addr;
	*psram ->size = arg_size;
	*psram ->base = arg_base;
	
	*pumap ->addr = arg_addr;
	*pumap ->size = arg_size;
	
	show_sran_conf( arg_num, psram, pumap );
}

// ======================= ZONE ===================== 

typedef struct {
	unsigned long *offset;
	unsigned long *max;
} zone_conf_t;

zone_conf_t zone_conf[] = {
	// cpu0
	{ 0xB8001700, 0xB8001704 },
	{ 0xB8001710, 0xB8001714 },
	{ 0xB8001720, 0xB8001724 },
	
	// cpu1 
	{ 0xB8001740, 0xB8001744 },
	{ 0xB8001750, 0xB8001754 },
	{ 0xB8001760, 0xB8001764 },
};

#define CPU0_ZONE_NUM		3
#define ALL_ZONE_NUM		6

static void show_zone_conf( int i, zone_conf_t *pzone )
{
	prom_printf( "%s ZONE[%d] index=%d\n", ( i < CPU0_ZONE_NUM ? "CPU0" : "CPU1" )
								, ( i < CPU0_ZONE_NUM ? i : i - CPU0_ZONE_NUM )
								, i );
	prom_printf( "	addr(%08X): %08X (%s)\n", pzone ->offset, *pzone ->offset, size_str( *pzone ->offset ) );
	prom_printf( "	max (%08X): %08X (%s)\n", pzone ->max, *pzone ->max, size_str( *pzone ->max + 1 ) );
}

void zone_conf_entry(int argc, char* argv[])
{
	// zone [<num> <offset> <max>]
	int i;
	zone_conf_t *pzone;
	unsigned long arg_num, arg_offset, arg_max;
	
	if( argc != 3 ) {
		
		for( i = 0; i < ALL_ZONE_NUM; i ++ ) {
		
			pzone = &zone_conf[ i ];
			
			show_zone_conf( i, pzone );
		}
		
		return;
	}

	arg_num = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	arg_offset = strtoul((const char*)(argv[1]), (char **)NULL, 16);
	arg_max = strtoul((const char*)(argv[2]), (char **)NULL, 16);
	
	if( arg_num >= ALL_ZONE_NUM ) {
		prom_printf( "number is too large (%u>=%u)\n", arg_num, ALL_ZONE_NUM );
		return;
	}
	
	pzone = &zone_conf[ arg_num ];
	
	*pzone ->offset = arg_offset;
	*pzone ->max = arg_max;
	
	show_zone_conf( arg_num, pzone );
}

// ======================= Reset 5181 ===================== 

void reset5181(int argc, char* argv[])
{
	*( ( volatile unsigned long * )0xB8000010 ) &= ~0xC0000000; // bit30, bit31
	
	prom_printf( "0xB8000010=%08X\n", *( ( volatile unsigned long * )0xB8000010 ) );
	
	*( ( volatile unsigned long * )0xB8000010 ) |= 0x80000000; // bit31
	
	prom_printf( "0xB8000010=%08X\n", *( ( volatile unsigned long * )0xB8000010 ) );
}

// ======================= Enable 5181 ===================== 

void enable5181(int argc, char* argv[])
{
	*( ( volatile unsigned long * )0xB8000010 ) &= ~0x40000000; // bit30

#if !defined (RLX5181_MP_TEST) && !defined (RLX5181_MP_ECOS_BOOT)
	prom_printf( "0xB8000010=%08X\n", *( ( volatile unsigned long * )0xB8000010 ) );
#endif
	
	*( ( volatile unsigned long * )0xB8000010 ) |= 0x40000000; // bit30

#if !defined (RLX5181_MP_TEST) && !defined (RLX5181_MP_ECOS_BOOT)
	prom_printf( "0xB8000010=%08X\n", *( ( volatile unsigned long * )0xB8000010 ) );
#endif
}

// ======================= Config & Boot 5181 ===================== 

void jtag1_for_5181(void)
{
	*( ( volatile unsigned long * )0xB80000a0 ) |= 0x04000000; //bit26=1
	*( ( volatile unsigned long * )0xB80000a0 ) &= ~0x0A000000; //bit25, 27=0
	
	prom_printf("Set JTAG1 for CPU 5181\n");
}

extern int lx5181_binary_start;
extern int lx5181_binary_end;

void boot2_5181_entry(int argc, char* argv[])
{
	// "boot2 [<32M|57M> {<SRAM>|<DRAM> <dram_addr>}]"
	int i;
	unsigned long *p;
	struct {
		int d57M:1;
		int d32M:1;
		int SRAM:1;
		int DRAM:1;
		unsigned long dram_addr;
	} setting;
	
	// 0. parse argc 
	memset( &setting, 0, sizeof( setting ) );
	
	if( argc == 0 ) {	// default
	
		setting.d32M = 1;
		//setting.d57M = 1;
		setting.DRAM = 1;
		setting.dram_addr = 0;
		
		goto label_start;
	}
	
	if( argc < 1 )
		goto label_error;
	
	if( memcmp( argv[ 0 ], "57M", 3 ) == 0 )
		setting.d57M = 1;
	else if( memcmp( argv[ 0 ], "32M", 3 ) == 0 )
		setting.d32M = 1;
	else
		goto label_error;
	
	if( argc < 2 )
		goto label_error;
	
	if( memcmp( argv[ 1 ], "SRAM", 4 ) == 0 )
		setting.SRAM = 1;
	else if( memcmp( argv[ 1 ], "DRAM", 4 ) == 0 )
	{
		setting.DRAM = 1;
		
		if( argc < 3 )
			setting.dram_addr = 0; // default jump to 0x8000,0000
		else
			setting.dram_addr = strtoul((const char*)(argv[2]), (char **)NULL, 16);
	} else
		goto label_error;

label_start:

	//reset5181(0, NULL);

	jtag1_for_5181();

	// fix clk issues
	//*( ( volatile unsigned long * )0xB80000A0 ) = 0x28000000;

#ifdef RLX5181_USE_UART0
	// uart 0 setup & test
	//uart0_test( 0, NULL ); // skip, 1074K has init uart0 ok
#endif

#ifdef RLX5181_USE_UART1
	// uart 1 setup & test
	uart1_test( 0, NULL );
#endif

#ifdef RLX5181_USE_UART2
	// uart 2 setup & test
	uart2_test( 0, NULL );
#endif
	
	// 1. sram config: 
	//    CPU0 [0] -> 0xa4000000 8k
	//    CPU1 [0] -> 0xbfc00000 8k (default) 
	{
		prom_printf( "\n===SRAM config!===\n" );
		#define ARGC_SRAM		4
		#define ARGV_SRAM_NUM	( sizeof( argv_sram ) / sizeof( argv_sram[ 0 ] ) )
		static const char *argv_sram[][ ARGC_SRAM ] = {
			{ "0", "04000001", "6", "00030000"},
			//sram num, sram size, sram size 8K, sram base
		};
		
		for( i = 0; i < ARGV_SRAM_NUM; i ++ )
			sram_conf_entry( ARGC_SRAM, argv_sram[ i ] );
		
		#undef ARGC_SRAM
		#undef ARGV_SRAM_NUM
	}
	
	// 2. fill program to sram 0xa4000000
	if( setting.SRAM ) {
		
		prom_printf( "\n===SRAM setting: filling!===\n" );

		p = ( void * )0xa4000000;

#ifdef RLX5181_USE_UART0
		for( i = 0; i < sizeof( opcode_uart0_show_A ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart0_show_A[ i ];		
#elif defined RLX5181_USE_UART1
		for( i = 0; i < sizeof( opcode_uart1_show_A ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart1_show_A[ i ];
#elif defined RLX5181_USE_UART2
		for( i = 0; i < sizeof( opcode_uart2_show_A ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart2_show_A[ i ];
#endif
		
			
	} else if( setting.DRAM ) {

		prom_printf( "\n===DRAM setting!===\n" );

#if 1
		// jump to 0x8000,0000
		p = ( void * )0xa4000000;
		for( i = 0; i < sizeof( opcode_jmp_0x8000_0000 ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_jmp_0x8000_0000[ i ];
#else
		// jump to 0x8000,0100
		p = ( void * )0xa4000000;
		for( i = 0; i < sizeof( opcode_jmp_0x8000_0100 ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_jmp_0x8000_0100[ i ];
#endif
		
		// modify jump addr
		if( setting.dram_addr )
		{
		
			p = ( void * )0xa4000000;
			*p = ( *p & ~0x0000FFFF ) | setting.dram_addr >> 16;
			
			p = ( void * )0xa4000004;
			*p = ( *p & ~0x0000FFFF ) | ( setting.dram_addr & 0x0000FFFF );
		}
#if 0
		else
		{
			// jump to 0x8000,0000	
			p = ( void * )0xa4000000;
			*p = ( *p & ~0x0000FFFF ) | ( TEST_5181_PROGRAM_DRAM_ENTRY ) >> 16;
			
			p = ( void * )0xa4000004;
			*p = ( *p & ~0x0000FFFF ) | ( TEST_5181_PROGRAM_DRAM_ENTRY & 0x0000FFFF );
		}
#endif
	}
	
	// 3. zone config: (totoal 64M)
	//    57M:
	//        CPU0 -> zone 0 offset 0   size 56M
	//             -> zone 1 offset 56M size 1M
	//        CPU1 -> zone 0 offset 57M size 7M
	//             -> zone 1 offset 56M size 1M
	//    32M:
	//        CPU0 -> zone 0 offset 0   size 31M
	//             -> zone 1 offset 31M size 1M
	//        CPU1 -> zone 0 offset 32M size 32M
	//             -> zone 1 offset 31M size 1M
	{
		prom_printf( "\n=== Zone config! ===\n" );

		#define ARGC_ZONE			3
		#define ARGV_ZONE_57M_NUM	( sizeof( argv_zone_57M ) / sizeof( argv_zone_57M[ 0 ] ) )
		#define ARGV_ZONE_32M_NUM	( sizeof( argv_zone_32M ) / sizeof( argv_zone_32M[ 0 ] ) )
		static const char *argv_zone_57M[][ ARGC_ZONE ] = {
			{ "0", "00000000", "037FFFFF" },
			{ "1", "03800000", "000FFFFF" },
			{ "3", "03900000", "006FFFFF" }, 
			{ "4", "03800000", "000FFFFF" },
		};
		
		static const char *argv_zone_32M[][ ARGC_ZONE ] = {
			{ "0", "00000000", "01EFFFFF" },
			{ "1", "01F00000", "000FFFFF" },
			{ "3", "02000000", "01FFFFFF" }, 
			{ "4", "01F00000", "000FFFFF" },
		};
		
		
		if( setting.d57M ) {
			for( i = 0; i < ARGV_ZONE_57M_NUM; i ++ )
				zone_conf_entry( ARGC_ZONE, argv_zone_57M[ i ] );
		} else if( setting.d32M ) {
			for( i = 0; i < ARGV_ZONE_32M_NUM; i ++ )
				zone_conf_entry( ARGC_ZONE, argv_zone_32M[ i ] );
		}
		
		#undef ARGC_ZONE
		#undef ARGV_ZONE_57M_NUM
		#undef ARGV_ZONE_32M_NUM
	}
	
	// 4. fill test program to dram
	if( setting.DRAM && !setting.dram_addr )
	{
		prom_printf( "\n===DRAM filling.===\n" );
		// test program is uart 1 output 'B'
		if( setting.d57M )
			p = ( void * )0xa3900000;	//57M
		else if( setting.d32M )
			p = ( void * )0xa2000000;	//32M
		else
			goto label_error;

#ifdef RLX5181_USE_UART0
		for( i = 0; i < sizeof( opcode_uart0_show_B ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart0_show_B[ i ];
#elif defined RLX5181_USE_UART1
		for( i = 0; i < sizeof( opcode_uart1_show_B ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart1_show_B[ i ];
#elif defined RLX5181_USE_UART2
		for( i = 0; i < sizeof( opcode_uart2_show_B ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart2_show_B[ i ];
#endif
	}

	// 5. reset shm data (before zone config)
	{
		prom_printf( "\n===SHM reset.===\n" );
		extern void shm_reset_data( void );
		
		shm_reset_data();
	}
	
	// 6. enable 5181 running on sram 
	prom_printf( "\n===5181 enable.===\n" );
	enable5181( 0, NULL );

	return;

label_error:	
	prom_printf( "format error!\n" );
	
	return;
}


void boot_5181_mp_entry(int argc, char* argv[])
{
	// "boot2 [<32M|57M> {<SRAM>|<DRAM> <dram_addr>}]"
	int i;
	unsigned long *p;
	struct {
		int d57M:1;
		int d32M:1;
		int SRAM:1;
		int DRAM:1;
		unsigned long dram_addr;
	} setting;
	
	// 0. parse argc 
	memset( &setting, 0, sizeof( setting ) );
	
	if( argc == 0 ) {	// default
	
		setting.d32M = 1;
		//setting.d57M = 1;
		setting.DRAM = 1;
		setting.dram_addr = 0;
		
		goto label_start;
	}
	
	if( argc < 1 )
		goto label_error;
	
	if( memcmp( argv[ 0 ], "57M", 3 ) == 0 )
		setting.d57M = 1;
	else if( memcmp( argv[ 0 ], "32M", 3 ) == 0 )
		setting.d32M = 1;
	else
		goto label_error;
	
	if( argc < 2 )
		goto label_error;
	
	if( memcmp( argv[ 1 ], "SRAM", 4 ) == 0 )
		setting.SRAM = 1;
	else if( memcmp( argv[ 1 ], "DRAM", 4 ) == 0 )
	{
		setting.DRAM = 1;
		
		if( argc < 3 )
			setting.dram_addr = 0; // default jump to 0x8000,0000
		else
			setting.dram_addr = strtoul((const char*)(argv[2]), (char **)NULL, 16);
	} else
		goto label_error;

label_start:

	//reset5181(0, NULL);

	//jtag1_for_5181();

	// fix clk issues
	//*( ( volatile unsigned long * )0xB80000A0 ) = 0x28000000;
	
	uart1_test( 0, NULL );

#if 0	//MP test use uart0
#ifdef RLX5181_USE_UART0
	// uart 0 setup & test
	//uart0_test( 0, NULL ); // skip, 1074K has init uart0 ok
#endif

#ifdef RLX5181_USE_UART1
	// uart 1 setup & test
	uart1_test( 0, NULL );
#endif

#ifdef RLX5181_USE_UART2
	// uart 2 setup & test
	uart2_test( 0, NULL );
#endif
#endif
	
	// 1. sram config: 
	//    CPU0 [0] -> 0xa4000000 8k
	//    CPU1 [0] -> 0xbfc00000 8k (default) 
	{
		prom_printf( "\n===SRAM config!===\n" );
		#define ARGC_SRAM		4
		#define ARGV_SRAM_NUM	( sizeof( argv_sram ) / sizeof( argv_sram[ 0 ] ) )
		static const char *argv_sram[][ ARGC_SRAM ] = {
			{ "0", "04000001", "6", "00030000"},
			//sram num, sram size, sram size 8K, sram base
		};
		
		for( i = 0; i < ARGV_SRAM_NUM; i ++ )
			sram_conf_entry( ARGC_SRAM, argv_sram[ i ] );
		
		#undef ARGC_SRAM
		#undef ARGV_SRAM_NUM
	}
	
	// 2. fill program to sram 0xa4000000
	if( setting.SRAM ) {
		
		prom_printf( "\n===SRAM setting: filling!===\n" );

		p = ( void * )0xa4000000;


#ifdef RLX5181_MP_TEST
		memcpy((char*)p, ( char * )&lx5181_binary_start + 0x1c, 
			(char *)&lx5181_binary_end - (char *)&lx5181_binary_start - 0x1c);
#elif defined (RLX5181_MP_ECOS_BOOT)
		prom_printf("Not support eCos MP boot from SRAM.\n");
#else

#ifdef RLX5181_USE_UART0
		for( i = 0; i < sizeof( opcode_uart0_show_A ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart0_show_A[ i ];		
#elif defined RLX5181_USE_UART1
		for( i = 0; i < sizeof( opcode_uart1_show_A ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart1_show_A[ i ];
#elif defined RLX5181_USE_UART2
		for( i = 0; i < sizeof( opcode_uart2_show_A ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart2_show_A[ i ];
#endif

#endif
		
			
	} else if( setting.DRAM ) {

		prom_printf( "\n===DRAM setting!===\n" );

#if 1
#ifdef RLX5181_MP_ECOS_BOOT
		// jump to 0x80c0,0000 to decompress eCos nfjrom
		p = ( void * )0xa4000000;
		for( i = 0; i < sizeof( opcode_jmp_0x80c0_0000 ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_jmp_0x80c0_0000[ i ];
#else
		// jump to 0x8000,0000
		p = ( void * )0xa4000000;
		for( i = 0; i < sizeof( opcode_jmp_0x8000_0000 ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_jmp_0x8000_0000[ i ];
#endif
#else
		// jump to 0x8000,0100
		p = ( void * )0xa4000000;
		for( i = 0; i < sizeof( opcode_jmp_0x8000_0100 ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_jmp_0x8000_0100[ i ];
#endif
		
		// modify jump addr
		if( setting.dram_addr )
		{
		
			p = ( void * )0xa4000000;
			*p = ( *p & ~0x0000FFFF ) | setting.dram_addr >> 16;
			
			p = ( void * )0xa4000004;
			*p = ( *p & ~0x0000FFFF ) | ( setting.dram_addr & 0x0000FFFF );
		}
#if 0
		else
		{
			// jump to 0x8000,0000	
			p = ( void * )0xa4000000;
			*p = ( *p & ~0x0000FFFF ) | ( TEST_5181_PROGRAM_DRAM_ENTRY ) >> 16;
			
			p = ( void * )0xa4000004;
			*p = ( *p & ~0x0000FFFF ) | ( TEST_5181_PROGRAM_DRAM_ENTRY & 0x0000FFFF );
		}
#endif
	}
	
	// 3. zone config: (totoal 64M)
	//    57M:
	//        CPU0 -> zone 0 offset 0   size 56M
	//             -> zone 1 offset 56M size 1M
	//        CPU1 -> zone 0 offset 57M size 7M
	//             -> zone 1 offset 56M size 1M
	//    32M:
	//        CPU0 -> zone 0 offset 0   size 31M
	//             -> zone 1 offset 31M size 1M
	//        CPU1 -> zone 0 offset 32M size 32M
	//             -> zone 1 offset 31M size 1M
	{
		prom_printf( "\n=== Zone config! ===\n" );

		#define ARGC_ZONE			3
		#define ARGV_ZONE_57M_NUM	( sizeof( argv_zone_57M ) / sizeof( argv_zone_57M[ 0 ] ) )
		#define ARGV_ZONE_32M_NUM	( sizeof( argv_zone_32M ) / sizeof( argv_zone_32M[ 0 ] ) )
		static const char *argv_zone_57M[][ ARGC_ZONE ] = {
			{ "0", "00000000", "037FFFFF" },
			{ "1", "03800000", "000FFFFF" },
			{ "3", "03900000", "006FFFFF" }, 
			{ "4", "03800000", "000FFFFF" },
		};
		
		static const char *argv_zone_32M[][ ARGC_ZONE ] = {
			{ "0", "00000000", "01EFFFFF" },
			{ "1", "01F00000", "000FFFFF" },
			{ "3", "02000000", "01FFFFFF" }, 
			{ "4", "01F00000", "000FFFFF" },
		};
		
		
		if( setting.d57M ) {
			for( i = 0; i < ARGV_ZONE_57M_NUM; i ++ )
				zone_conf_entry( ARGC_ZONE, argv_zone_57M[ i ] );
		} else if( setting.d32M ) {
			for( i = 0; i < ARGV_ZONE_32M_NUM; i ++ )
				zone_conf_entry( ARGC_ZONE, argv_zone_32M[ i ] );
		}
		
		#undef ARGC_ZONE
		#undef ARGV_ZONE_57M_NUM
		#undef ARGV_ZONE_32M_NUM
	}
	
	// 4. fill test program to dram
	if( setting.DRAM && !setting.dram_addr )
	{
		prom_printf( "\n===DRAM filling.===\n" );
		// test program is uart 1 output 'B'
		if( setting.d57M )
			p = ( void * )0xa3900000;	//57M
		else if( setting.d32M )
			p = ( void * )0xa2000000;	//32M
		else
			goto label_error;



#ifdef RLX5181_MP_ECOS_BOOT
		memcpy((char*)p+0xc00000, ( char * )&lx5181_binary_start , 
			(char *)&lx5181_binary_end - (char *)&lx5181_binary_start);
#elif defined RLX5181_MP_TEST
		memcpy((char*)p, ( char * )&lx5181_binary_start + 0x1c, 
			(char *)&lx5181_binary_end - (char *)&lx5181_binary_start - 0x1c);
#else

#ifdef RLX5181_USE_UART0
		for( i = 0; i < sizeof( opcode_uart0_show_B ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart0_show_B[ i ];			
#elif defined RLX5181_USE_UART1
		for( i = 0; i < sizeof( opcode_uart1_show_B ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart1_show_B[ i ];
#elif defined RLX5181_USE_UART2
		for( i = 0; i < sizeof( opcode_uart2_show_B ) / sizeof( unsigned long ); i ++ )
			*p ++ = opcode_uart2_show_B[ i ];
#endif

#endif
	}

	// 5. reset shm data (before zone config)
	{
		prom_printf( "\n===SHM reset.===\n" );
		extern void shm_reset_data( void );
		
		shm_reset_data();
	}
	
	// 6. enable 5181 running on sram 
	prom_printf( "\n===5181 enable.===\n" );
	enable5181( 0, NULL );

#ifdef RLX5181_MP_TEST
	volatile int x;
	for (x=0x50000; x < 0; x--);
	prom_printf("\n\n");
#endif
	
	return;

label_error:	
	prom_printf( "format error!\n" );
	
	return;
}

// ======================= shm test ===================== 

#include "shm.h"

void shm_reset_data( void )
{
	// call it before boot 5181 
	
	shm_t *pshm = ( shm_t * )CPU0_SHM_BASEADDR;
	
	prom_printf( "Shm base=%X size=%s!\n", pshm, size_str( sizeof( shm_t ) ) );

#if 1
	memset( &pshm ->shm_cpu0to1.header, 0, sizeof( pshm ->shm_cpu0to1.header ) );
	memset( &pshm ->shm_cpu1to0.header, 0, sizeof( pshm ->shm_cpu1to0.header ) );
#else	
	prom_printf( "Clean memory take a long time...." );
	memset( pshm, 0, sizeof( shm_t ) );
	prom_printf( "Done!\n" );
#endif
}

static void shm_initial_header( void )
{
	shm_t *pshm = ( shm_t * )CPU0_SHM_BASEADDR;
	
	pshm ->shm_cpu0to1.header.a.ri = 0;
	pshm ->shm_cpu0to1.header.a.wi = 0;

	pshm ->shm_cpu1to0.header.a.ri = 0;
	pshm ->shm_cpu1to0.header.a.wi = 0;
}

#if 0
inline void __delay(unsigned int loops)
{
    __asm__ __volatile__ (
    "   .set    noreorder               \n"
    "   .align  3                   \n"
    "1: bnez    %0, 1b                  \n"
    "   subu    %0, 1                   \n"
    "   .set    reorder                 \n"
    : "=r" (loops)
    : "0" (loops));
}
#endif

void shm_test_entry(int argc, char* argv[])
{
	shm_t volatile *pshm = ( shm_t * )CPU0_SHM_BASEADDR;
	shm_oneway_t volatile *psend = &pshm ->shm_cpu0to1;
	shm_oneway_t volatile *precv = &pshm ->shm_cpu1to0;
	shm_test_data_t volatile *pdata;
	
	unsigned long next_wi, next_ri;
	unsigned long loop = 0;
	int delay;
	//#define DELAY_COUNT		500000		// 328_2
	//#define DELAY_COUNT		1
	#define DELAY_COUNT			240000	// 48M / 200
	
	// assume cpu1 5181 is up 
	
	// initial header 
	shm_initial_header();
	
	// set cpu0 ready 
	pshm ->shm_cpu0to1.header.a.ready_magic = CPU0_READY_MAGIC;
	
	// wait for cpu1 ready 
	prom_printf( "wait for cpu1 ready..." );
	
	while( pshm ->shm_cpu1to0.header.a.ready_magic != CPU1_READY_MAGIC );
	
	prom_printf( "cpu1 ready! (delay=%d)\n", DELAY_COUNT );
	
	// do tx/rx
	while( 1 ) {

		// recv 
		while( precv ->header.a.ri != precv ->header.a.wi ) {
		
			// counter 
			psend ->header.a.rx_count ++;
			
			// recv and check 
			pdata = ( shm_test_data_t * )precv ->data.a[ precv ->header.a.ri ].data;
			
			// process content 
			{
				int i;
				unsigned long checksum = 0;
				
				for( i = 0; i < 200; i ++ ) {
					checksum += pdata ->data[ i ];
				}
				
				if( checksum != pdata ->checksum )
					psend ->header.a.rx_error ++;
			}
			
			// recv it done! 
			precv ->header.a.ri = ( precv ->header.a.ri + 1 ) % SHM_DATA_NUM;
		}
		
		// send one 
		next_wi = ( psend ->header.a.wi + 1 ) % SHM_DATA_NUM;
		
		if( next_wi == psend ->header.a.ri ) {
		
			psend ->header.a.tx_full ++; // full
		} else {
			
			// send one 
			psend ->header.a.tx_count ++;
			
			// fill content 
			pdata = ( shm_test_data_t * )psend ->data.a[ psend ->header.a.wi ].data;
			
			{
				static unsigned long seq = 0x00456789;
				static unsigned long seed = 0xAB67CD89;
				int i;
				unsigned long checksum = 0;
				
				for( i = 0; i < 200; i ++ ) {
					pdata ->data[ i ] ^= seed;
					seed += ( seed << 4 ) + ( seed >> 4 ) + pdata ->data[ i ];
					checksum += pdata ->data[ i ];
				}
				
				pdata ->seq = seq ++;;
				pdata ->len = 200;
				pdata ->checksum = checksum;
			}
			
			// send it!
			psend ->header.a.wi = next_wi;
		}
		
		// summary 
		if( ( ++ loop & 0xFF ) == 0xFF ) {
			prom_printf( "%X tx=%d %d rx=%d %d\n", loop, 
							psend ->header.a.tx_count, psend ->header.a.tx_full, 
							psend ->header.a.rx_count, psend ->header.a.rx_error );
		}
		
		if (psend ->header.a.tx_full || psend ->header.a.rx_error)
		{
			printf("tx full or rx empty !\n");

			prom_printf( "%X tx=%d %d rx=%d %d\n", loop, 
							psend ->header.a.tx_count, psend ->header.a.tx_full, 
							psend ->header.a.rx_count, psend ->header.a.rx_error );

			goto label_done;
		}
		
		// delay 
		//for( delay = 0; delay < DELAY_COUNT; delay ++ );
		__delay( DELAY_COUNT );
		
		// break 
		extern unsigned char Get_UartData();

		if( Get_UartData() == 'x' )
			goto label_done;
		
	} // infinite loop 

label_done:	
	prom_printf( "leave shm!\n" );
	
}

// ======================= mutex test ===================== 

static void self_random( int *rand, int mask )
{
	*rand = ( ( *rand + 0x12345678 ) + ( *rand >> 4 ) + ( *rand << 5 ) ) & mask;
}

void mutex_test_entry(int argc, char* argv[])
{
	//#define DELAY_COUNT		50000	// hang 
	//#define DELAY_COUNT			100000	// shm328_2
	//#define DELAY_COUNT			1
	#define DELAY_COUNT			240000	// 48M / 200
	
	#define MAX_DELAY_MASK		0xFFFFF	// about 5 * 240000
	
	int delay_yield = DELAY_COUNT;
	int delay_try = DELAY_COUNT;
		
	//CPU0 IPC MUTEX   (0xB8141040)
	//CPU1 IPC MUTEX   (0xB8141044)
	//MUTEX Owner   (0xB8141048)
	#define MUTEX_OWNER_ID		1

	unsigned long volatile *pCPU0Mutex = ( unsigned long volatile * )0xB8141040;
	//unsigned long volatile *pCPU1Mutex = ( unsigned long volatile * )0xB8141044;
	unsigned long volatile *pMutexOwner = ( unsigned long volatile * )0xB8141048;
	
	struct {
		unsigned long loop;
		unsigned long yield;
		unsigned long yield_error;
		unsigned long locked;
	} mutex_info = { 0, 0, 0, 0 };
	
	//NS16550_init(0 , 0);
	
	//printf( "************************************************************\n" );
	//printf( "mutex test start: %s @%s %s------\n\n", __FILE__, __DATE__, __TIME__ );
		
	while( 1 ) {
		
		mutex_info.loop ++;
		
		if( *pCPU0Mutex == 0 ) {
			// read unlock, and yield auto lock 
			mutex_info.yield ++;
			
			if( *pMutexOwner != MUTEX_OWNER_ID )
				mutex_info.yield_error ++;
			
			__delay( delay_yield );	// delay 
			
			self_random( &delay_yield, MAX_DELAY_MASK );
			
			*pCPU0Mutex = 0;	// unlock 
			
		} else {
			// locked
			mutex_info.locked ++;
		}
		
		__delay( delay_try );
		
		self_random( &delay_try, MAX_DELAY_MASK );
		
		// show something 
		if( ( mutex_info.loop & 0xff ) == 0 ) {
			
			printf( "loop=%d, yield=%d (err=%d), locked=%d\n", 
					mutex_info.loop, mutex_info.yield, mutex_info.yield_error, 
					mutex_info.locked );
			
		}
		
		if (mutex_info.yield_error)
		{
			printf("yield error!\n");
			
			printf( "loop=%d, yield=%d (err=%d), locked=%d\n", 
				mutex_info.loop, mutex_info.yield, mutex_info.yield_error, 
				mutex_info.locked );

			goto label_stop;
		}
		
		// break;
		extern unsigned char Get_UartData();
		
		if( Get_UartData() == 'x' )
			break;
	}

label_stop:
	
	printf( "stop\n" );
	//while( 1 );	
	
}

