#ifndef __BSPFLUSH_H_
#define __BSPFLUSH_H_

#define cpu_dcache_line				32
#define cpu_icache_line				32
#define cpu_l2cache_line			32
#define cpu_dcache_size				(32 * 1024)
#define cpu_icache_size				(64 * 1024)
#define cpu_l2cache_size			(512 * 1024)

/* cache operations */
#define CACHE_OPCODE( code, type )		( ((code) << 2) | (type) )

#define ICACHE_INDEX_INVALIDATE                 CACHE_OPCODE(0x0, 0)
#define ICACHE_INDEX_LOAD_TAG                   CACHE_OPCODE(0x1, 0)
#define ICACHE_INDEX_STORE_TAG                  CACHE_OPCODE(0x2, 0)
#define DCACHE_INDEX_WRITEBACK_INVALIDATE       CACHE_OPCODE(0x0, 1)
#define DCACHE_INDEX_LOAD_TAG                   CACHE_OPCODE(0x1, 1)
#define DCACHE_INDEX_STORE_TAG                  CACHE_OPCODE(0x2, 1)
#define SCACHE_INDEX_WRITEBACK_INVALIDATE       CACHE_OPCODE(0x0, 3)
#define SCACHE_INDEX_STORE_TAG                  CACHE_OPCODE(0x2, 3)

#define ICACHE_ADDR_HIT_INVALIDATE              CACHE_OPCODE(0x4, 0)
#define ICACHE_ADDR_FILL                        CACHE_OPCODE(0x5, 0)
#define ICACHE_ADDR_FETCH_LOCK                  CACHE_OPCODE(0x7, 0)
#define DCACHE_ADDR_HIT_INVALIDATE              CACHE_OPCODE(0x4, 1)
#define DCACHE_ADDR_HIT_WRITEBACK_INVALIDATE    CACHE_OPCODE(0x5, 1)
#define DCACHE_ADDR_HIT_WRITEBACK               CACHE_OPCODE(0x6, 1)
#define DCACHE_ADDR_FETCH_LOCK                  CACHE_OPCODE(0x7, 1)

#define SCACHE_ADDR_HIT_INVALIDATE              CACHE_OPCODE(0x4, 3)
#define SCACHE_ADDR_HIT_WRITEBACK_INVALIDATE    CACHE_OPCODE(0x5, 3)
#define SCACHE_ADDR_HIT_WRITEBACK               CACHE_OPCODE(0x6, 3)

/* writeback-invalidate whole dcache */
extern void mips_flush_dcache_all(void);

/* invalidate whole icache */
extern void mips_invalid_icache_all(void);

/* writeback-invalidate whole l2cache */
extern void mips_flush_l2cache_all(void);

#endif
