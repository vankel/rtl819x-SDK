/*
 *	Realtek I2S Controller Driver
 *
 *
 * author: SD4-VoIP jwsyu@realtek.com #5402
 * version 01 date 2012/09/19 AM 11:25:16
 * version 02 date 2013/07/19 PM 04:19:23
 *
 *
 */

//config start
//#define SOC_TYPE_8196D	1
//#define SOC_TYPE_8881A	1
#define SOC_TYPE_8198C	1


#ifdef SOC_TYPE_8198C
  #ifdef __m5181
    #define SOC_TYPE_8198C_CPU1_LEXRA5181 1
    #define PLATFORM_APOLLO_NON_LINUX	1
  #else
    #define SOC_TYPE_8198C_CPU0_MIPS1074K 1
    #define LOGICAL_PHYSICAL_OFFSET 0
  #endif
#endif


#define IIS_DBG_LVL 3  //range 0~3
//#define LOOPBACK_SIMPLE_PAT 1  //use lrclk(word selec) as data input (when NO ALC5621)

//#define IIS_CODEC_ALC5621 1
#define ONE_WEEK_SEC (7*24*60*60)
#define IIS_TEST_TIME_SEC 20
#ifdef IIS_CODEC_ALC5621
#define IIS_PLAY_TONE 1
//#define IIS_MIC_LOOPBACK_SPEAKER 1
#endif

#define I2S_IN_FPGA 1 // fpga is slow, check less sample
//#define I2S_EN_SRAM 1 // enable sram and dma data
//#define DELAY_OWNBIT 1	//delay enable ownbit, 

// lock bus before disalbe iis
#define LOCK_LX_BUS 1

//config end

//#define IIS_BASE (0xb8009000) //8198 base address
#if defined(SOC_TYPE_8198C) || defined(SOC_TYPE_8881A) || defined(SOC_TYPE_8196D)
#define IIS_BASE (0xb801F000)  //8196d, 8881a, 8198c base address
#endif
#define IISCR		(IIS_BASE + 0x00)	//IIS Interface Control Register
#define TX_PAGE_PTR	(IIS_BASE + 0x04)	//TX Page Pointer Register
#define RX_PAGE_PTR	(IIS_BASE + 0x08)	//RX Page Pointer Register
#define IIS_SETTING	(IIS_BASE + 0x0C)	//IIS Page size and Sampling rate setting  Register
#define IIS_TX_IMR	(IIS_BASE + 0x10)	//IIS TX Interrupt Mask Register
#define IIS_TX_ISR	(IIS_BASE + 0x14)	//IIS TX Interrupt Status Register
#define IIS_RX_IMR	(IIS_BASE + 0x18)	//IIS RX Interrupt Mask Register
#define IIS_RX_ISR	(IIS_BASE + 0x1C)	//IIS RX Interrupt Status Register
#define IIS_TX_P0OWN	(IIS_BASE + 0x20)	//IIS TX Page 0 Own bit
#define IIS_TX_P1OWN	(IIS_BASE + 0x24)	//IIS TX Page 1 Own bit
#define IIS_TX_P2OWN	(IIS_BASE + 0x28)	//IIS TX Page 2 Own bit
#define IIS_TX_P3OWN	(IIS_BASE + 0x2C)	//IIS TX Page 3 Own bit
#define IIS_RX_P0OWN	(IIS_BASE + 0x30)	//IIS RX Page 0 Own bit
#define IIS_RX_P1OWN	(IIS_BASE + 0x34)	//IIS RX Page 1 Own bit
#define IIS_RX_P2OWN	(IIS_BASE + 0x38)	//IIS RX Page 2 Own bit
#define IIS_RX_P3OWN	(IIS_BASE + 0x3C)	//IIS RX Page 3 Own bit

//IISCR
#define DACLRSWAP	BIT(10)
#define IIS_LOOP_BACK	BIT(7)
#define IIS_WL_16BIT	BIT(6)
#define IIS_EDGE_N	BIT(5)
#define IIS_MODE_MONO	BIT(4)
#define IIS_TXRXACT	BIT(2)
#define IIS_ENABLE	BIT(0)

//IIS_TX_ISR
#define IIS_TX_P0OK	BIT(0)
#define IIS_TX_P1OK	BIT(1)
#define IIS_TX_P2OK	BIT(2)
#define IIS_TX_P3OK	BIT(3)

//IIS_RX_ISR
#define IIS_RX_P0OK	BIT(0)
#define IIS_RX_P1OK	BIT(1)
#define IIS_RX_P2OK	BIT(2)
#define IIS_RX_P3OK	BIT(3)

#if defined(SOC_TYPE_8198C) || defined(SOC_TYPE_8881A) || defined(SOC_TYPE_8196D)
#define BIST_CONTROL	0xb8000014
#define LOCK_LX2_BUS	(1<<4)
#define BIST_DONE	0xb8000020
#define LOCK_LX2_OK	(1<<2)
#endif
#ifndef REG32
#define REG32(reg) (*(volatile unsigned long *)(reg))
#endif
#define BIT(x)	( 1 << (x))
#define rtl_outl(address, value)	(REG32(address) = value)
#define rtl_inl(address)			REG32(address)

#define Virtual2NonCache(x)             (((int)x) | 0x20000000)

#define Logical2Physical(x)				( ( x ) + LOGICAL_PHYSICAL_OFFSET )


//#include <rtl8650/asicregs.h>
#include <rtl_types.h>
#ifndef PLATFORM_APOLLO_NON_LINUX
#include <asm/io.h>
#include <linux/interrupt.h>
#endif
#include <linux/types.h>

#ifndef NULL
#define NULL    (void*)0
#endif
#if defined(__m5280) || defined(__m4181) || defined(__m5181) || defined(__m5281)
#include <asm/lexraregs.h>
#elif defined(SOC_TYPE_8198C_CPU0_MIPS1074K)
#include <asm/mipsregs.h>
#include <asm/rtl8198c.h>
//#define MIPS_COUNTER 1
//#define GIC_COUNTER 1
#endif

#define I2S_PAGE_TIME	100	// unit 0.1ms, test case 10, 25, 50 100, 200, 400 

#if defined(IIS_CODEC_ALC5621) && defined(IIS_PLAY_TONE)
#define PAGE_SIZE	(96*I2S_PAGE_TIME*6/10) // max 96kbps, max 6channel, 24bit wordlength
#else
#define PAGE_SIZE	(96*I2S_PAGE_TIME*2/10) // max 96kbps, max 2channel, 24bit wordlength
#endif

//#define PAGE_SIZE	40	// 40 * 32bit, 80sample, 10ms
//#define PAGE_SIZE	(80*3)	// 80*3 * 32bit, 480sample, 60ms
//#define PAGE_SIZE	(80*6)	// 80*6 * 32bit, 960sample, 120ms
//#define PAGE_SIZE	(80*3*17)	// 80*3*17 * 32bit, 8160sample (bigger page size for 96khz test)
//#define PAGE_SIZE	81	// 81 * 32bit, 162sample, 20.25ms
//#define PAGE_SIZE	(84)	// 84 * 32bit, 168sample, 21ms
//#define PAGE_SIZE	(4000)	// 4000 * 32bit, 8000sample, 1000ms
//note IF in 96khz test. PAGE_SIZE = 80*3 may be timing not good change bigger 
#if (PAGE_SIZE>4096)
  #undef PAGE_SIZE
  #define PAGE_SIZE 4096
  //#error "max page size supported is 4096"
#endif

#define MAX_PAGE_NUM	4
#define PAGE_NUM	4

#define DATABUFSIZE	(50*PAGE_SIZE*4)

#ifdef I2S_EN_SRAM
static short iis_tx_buf[MAX_PAGE_NUM*PAGE_SIZE*2+32]__attribute__ ((aligned (0x1000)));
static short iis_rx_buf[MAX_PAGE_NUM*PAGE_SIZE*2+32]__attribute__ ((aligned (0x1000)));
static short iis_rx_buf_dummy[2048];
#else
static short iis_tx_buf[MAX_PAGE_NUM*PAGE_SIZE*2+32]__attribute__ ((aligned (0x1000)));
static short iis_rx_buf[MAX_PAGE_NUM*PAGE_SIZE*2+32]__attribute__ ((aligned (0x800)));
#endif

volatile int i2s_isr_test_flag;
int i2s__cnt;

#define printfByPolling prom_printf
#define printf prom_printf

struct iis_pkt {
	//unsigned char 	tx_rx;
	//unsigned char 	*payload;
	short 	*payload;
	int	*payload_24bit;
//	short 	buf[PAGE_SIZE*2]; // mult 2 because of short type!
};
static struct iis_pkt iis_tx;
static struct iis_pkt iis_rx;

#if defined(IIS_CODEC_ALC5621) && defined(IIS_MIC_LOOPBACK_SPEAKER)
unsigned char databuf[DATABUFSIZE];
#endif
uint32_t tx_read_index, rx_write_index;

#define printk printfByPolling

int iis_txpage;
int iis_rxpage;
#if 0  //disable tr count check
int iis_tr_cnt;
#endif

static int iis_trx_short = 0;
static int iis_allchannel = 0;
//static int iis_24bit_wordlength;
static int iis_rate;
static int iis_24bit_flag;
static int iis_internal_loopback;
static int iis_page_size;
static int iis_44p1khz_flag;

static int iis_isr_cnt = 0;
static int iis_cpy_cnt = 0;

static unsigned long IISChanTxPage[4] = {IIS_TX_P0OK, IIS_TX_P1OK, IIS_TX_P2OK, IIS_TX_P3OK};
static unsigned long IISChanRxPage[4] = {IIS_RX_P0OK, IIS_RX_P1OK, IIS_RX_P2OK, IIS_RX_P3OK};
#if !defined(IIS_CODEC_ALC5621)
static short iis_comp[PAGE_NUM*PAGE_SIZE*2+4];
static int iis_comp_24bit[PAGE_NUM*PAGE_SIZE+4];
#endif
#if 0
short iis_dump[PAGE_NUM*PAGE_SIZE*2];
#endif
static int iis_break_point = 0;
static unsigned int iis_test_cnt;
static unsigned int iis_test_cnt_tx;

static int iis_tx_index;


int i2s_test_done = 1;

static int iis_test_starttime;
#if defined(PLATFORM_APOLLO_NON_LINUX)
unsigned int i2s_test_jiffies = 1;
static volatile int get_timer_jiffies( void )
{
	return i2s_test_jiffies;
}

void tick_timer_jiffies( void )
{
	i2s_test_jiffies ++;
}
#else
volatile int get_timer_jiffies(void);
#define tick_timer_jiffies()
#endif
#if defined(IIS_CODEC_ALC5621) && defined(IIS_PLAY_TONE)
void tone_gens(int32_t sample_num, int16_t *buffPtr);
void tone_gens_24bit(int32_t sample_num, int32_t *buffPtr);
#endif
#if 0
//static short iis_test_data[]={0x03, 0xefef, 0x0c, 0xefef,
//                              0x0f, 0xefef, 0x30, 0xefef,
//                              0xc0, 0xefef, 0x00, 0xefef};
static short iis_test_data[]={0xf0, 0x03, 0x00ef, 0xefef, 0x0f, 0x0c, 0x00ef, 0xefef,
                              0xf0, 0x0f, 0x00ef, 0xefef, 0x0f, 0x30, 0x00ef, 0xefef,
                              0xf0, 0xc0, 0x00ef, 0xefef, 0x0f, 0xf0, 0x00ef, 0xefef,
                              0xf0, 0x300, 0x00ef, 0xefef, 0x0f, 0xc00, 0x00ef, 0xefef,
                              0xf0, 0xf00, 0x00ef, 0xefef, 0x0f, 0x000, 0x00ef, 0xefef};
#else
static short iis_test_data[]={0x0000, 0x1111, 0x2222, 0x3333,
                              0x4444, 0x5555, 0x6666, 0x7777,
                              0x8888, 0x9999, 0xaaaa, 0xbbbb,
                              0xcccc, 0xdddd, 0xeeee, 0xffff};
static int iis_test_data_24bit[]={0x000000, 0x111111, 0x222222, 0x333333,
                                    0x444444, 0x555555, 0x66666, 0x777777,
                                    0x888888, 0x999999, 0xaaaaaa, 0xbbbbbb,
                                    0xcccccc, 0xdddddd, 0xeeeeee, 0xffffff};

#endif

#if defined(IIS_CODEC_ALC5621) && defined(IIS_PLAY_TONE)
int32_t tone_phase=0;

int32_t tone_phase_ad = (1014 * 16777) >> 11;		// 65535/8000 in Q11

//int32_t play_channel;
int32_t play_channel_now;

static int sample_count;

#endif
int32_t play_channel;

#define START_TEST_STR \
"========================>>start test<<===================================\n"
#define END_TEST_STR \
"========================>>end   test<<===================================\n"

#if defined(__m5280) || defined(__m4181) || defined(__m5181) || defined(__m5281)
static void enable_CP3(void)
{
	__write_32bit_c0_register(CP0_STATUS, 0, __read_32bit_c0_register(CP0_STATUS, 0)|0x80000000);
}
#endif
void iis_breakpoint(void)
{

	iis_break_point ++;
	return;
}

void iis_set_tx_own_bit(unsigned int pageindex)
{
	//unsigned long address;
	//address = (IIS_TX_P0OWN + 4*pageindex);
	rtl_outl( (IIS_TX_P0OWN + 4*pageindex), BIT(31));

}

void iis_set_rx_own_bit(unsigned int pageindex)
{
	//unsigned long address;
	//address = (IIS_RX_P0OWN + 4*pageindex );
	rtl_outl( (IIS_RX_P0OWN + 4*pageindex ), BIT(31));

}

void lock_lx2_bus(void)
{

	int i, temp, temp1;

	temp=rtl_inl(BIST_CONTROL);
	rtl_outl(BIST_CONTROL, temp|LOCK_LX2_BUS);
	for (i=0 ; i<2000 ; i++) {
		temp1 = rtl_inl(BIST_DONE);
		if (temp1&LOCK_LX2_OK)
			break;
	}
}

void unlock_lx2_bus(void)
{
	int temp;

	temp=rtl_inl(BIST_CONTROL);
	rtl_outl(BIST_CONTROL, temp&(~LOCK_LX2_BUS));
}

void init_i2s_core(void)
{
	int i;
#if 1
	iis_tx.payload = (unsigned short *) Virtual2NonCache(iis_tx_buf);
	iis_rx.payload = (unsigned short *) Virtual2NonCache(iis_rx_buf);
	iis_tx.payload_24bit = (unsigned int *) Virtual2NonCache(iis_tx_buf);
	iis_rx.payload_24bit = (unsigned int *) Virtual2NonCache(iis_rx_buf);
#else
	//tx[chid].payload = (unsigned short *) (0xa0c00000 + chid * 0x10000);
	//rx[chid].payload = (unsigned short *) (0xa0d00000 + chid * 0x10000);
	iis_tx.payload = (unsigned short *) (0xa0300000 );
	iis_rx.payload = (unsigned short *) (0xa0340000 );//0x40000=256kbyte = 64k word
	//iis_tx.payload = (unsigned short *) (0xa0180000 ); //0x8000=32kbyte, = 8k word
	//iis_rx.payload = (unsigned short *) (0xa0188000 );
#endif

  #ifdef I2S_EN_SRAM
#if defined(SOC_TYPE_8198C_CPU0_MIPS1074K)
  rtl_outl( 0xb8001300, 0);
  rtl_outl( 0xb8004000, 0);
		//add sram support 8198c have 8(fpga), 16(asic)kbyte, use 4k for tx , 4k for rx
		rtl_outl( 0xb8001304, 0x5);//unmap seg len=4K
		rtl_outl( 0xb8001300, Logical2Physical((unsigned int)iis_tx.payload & 0xffff000) | 1 );////unmap seg addr
		//rtl_outl( 0xb8001350, ((unsigned int)iis_tx.payload & 0xffff800) | 1 );////unmap seg addr

		rtl_outl( 0xb8004008, 0x30000);
		rtl_outl( 0xb8004004, 0x5);//sram seg len=4K
		//rtl_outl( 0xb8004050, Logical2Physical((unsigned int)iis_tx.payload & 0xffff800) | 1 );////sram seg addr
		rtl_outl( 0xb8004000, ((unsigned int)iis_tx.payload & 0xffff000) | 0x00000001 );////sram seg addr

		rtl_outl( 0xb8001324, 0x5);//unmap seg len=4K
		rtl_outl( 0xb8001320, Logical2Physical((unsigned int)iis_rx.payload & 0xffff000) | 1 );////unmap seg addr

		rtl_outl( 0xb8004028, 0x31000);
		rtl_outl( 0xb8004024, 0x5);//sram seg len=4K
		rtl_outl( 0xb8004020, ((unsigned int)iis_rx.payload & 0xffff000) | 0x00000001 );////sram seg addr

#endif
#if defined(SOC_TYPE_8198C_CPU1_LEXRA5181)
  rtl_outl( 0xb8001340, 0);
  rtl_outl( 0xb8004040, 0);
		//add sram support 8198c have ?kbyte, use 2k for tx , 2k for rx
		rtl_outl( 0xb8001354, 0x5);//unmap seg len=4K
		rtl_outl( 0xb8001350, Logical2Physical((unsigned int)iis_tx.payload & 0xffff000) | 1 );////unmap seg addr
		//rtl_outl( 0xb8001350, ((unsigned int)iis_tx.payload & 0xffff800) | 1 );////unmap seg addr

		rtl_outl( 0xb8004048, 0x30000);
		rtl_outl( 0xb8004044, 0x5);//sram seg len=4K
		//rtl_outl( 0xb8004050, Logical2Physical((unsigned int)iis_tx.payload & 0xffff800) | 1 );////sram seg addr
		rtl_outl( 0xb8004040, ((unsigned int)iis_tx.payload & 0xffff000) | 0x80000001 );////sram seg addr


		rtl_outl( 0xb8004058, 0x30000);
		rtl_outl( 0xb8004054, 0x5);//sram seg len=4K
		rtl_outl( 0xb8004050, Logical2Physical((unsigned int)iis_tx.payload & 0xffff000) | 1 );////sram seg addr
		//rtl_outl( 0xb8004050, ((unsigned int)iis_tx.payload & 0xffff800) | 1 );////sram seg addr

		rtl_outl( 0xb8001374, 0x5);//unmap seg len=4K
		rtl_outl( 0xb8001370, Logical2Physical((unsigned int)iis_rx.payload & 0xffff000) | 1 );////unmap seg addr

		rtl_outl( 0xb8004068, 0x31000);
		rtl_outl( 0xb8004064, 0x5);//sram seg len=4K
		rtl_outl( 0xb8004060, ((unsigned int)iis_rx.payload & 0xffff000) | 0x80000001 );////sram seg addr

		rtl_outl( 0xb8004078, 0x31000);
		rtl_outl( 0xb8004074, 0x5);//sram seg len=4K
		rtl_outl( 0xb8004070, Logical2Physical((unsigned int)iis_rx.payload & 0xffff000) | 1 );////sram seg addr
#endif
  #endif /* defined(I2S_EN_SRAM) */
	iis_tx_index=0;
//printk("(%d)", __LINE__);
//#if 1 //for g711 testing. chiminer
#ifndef IIS_CODEC_ALC5621
		//printf("comp_expand: \n");
		//random seed,
		short random_add;
		random_add=(short)(get_timer_jiffies()&0xffff);

		for (i=0 ; i<((iis_page_size*PAGE_NUM)*2+16) ; i++) {
  #if 1
			int raw_pcm;
			if (iis_24bit_flag==0) {
    #ifdef LOOPBACK_SIMPLE_PAT
				if (i%2 == 1)
					raw_pcm = (short)0xfffe;
				else
					raw_pcm = (short)0x0001;
    #else
				raw_pcm = (i+101)*30-67;//(i+9)*7-19;
				if (i%2 == 1)
					raw_pcm ^= 0x8000;
					raw_pcm = (short) raw_pcm;
    #endif
			} else {
    #ifdef LOOPBACK_SIMPLE_PAT
				if (i%2 == 1)
					raw_pcm = 0xffffff;
				else
					raw_pcm = 0x000000;
    #else
				raw_pcm = ((i+101)*1705-67)&0xffffff;
				if (i%2 == 1)
					raw_pcm ^= 0x800000;
    #endif
			}
  #else
			int raw_pcm;
			if (iis_24bit_flag==0)
				raw_pcm = iis_test_data[(i>>1)%16];
			else
				raw_pcm = iis_test_data_24bit[i%16];
			//short raw_pcm = iis_test_data[i%40];
			//short raw_pcm = i;
  #endif
			short back;


			if (iis_24bit_flag==0) {
				iis_tx.payload[i] = raw_pcm;
			//iis_tx.payload[i] = i+5;
			//iis_tx.payload[i] = 0;
			//iis_tx.payload[i] = 0x5550 | iis_tx_index;
			//iis_tx.payload[i] = 0xA5B0 | iis_tx_index;
			//iis_tx_index = (iis_tx_index +1 )%6;
				iis_comp[i]=raw_pcm;
			} else {
				if (i >= (iis_page_size*PAGE_NUM+8))
					break;
				iis_tx.payload_24bit[i] = raw_pcm;
				iis_comp_24bit[i]=raw_pcm;
			}
		}
#endif /* IIS_CODEC_ALC5621 */
#if 1
//printk("(%d)", __LINE__);
  #if defined(IIS_CODEC_ALC5621) && defined(IIS_PLAY_TONE)
    #ifdef I2S_IN_FPGA
		memset(iis_tx.payload, 0, iis_page_size*PAGE_NUM*4+16);
    #endif
		if (iis_24bit_flag)
			tone_gens_24bit((iis_page_size*PAGE_NUM), iis_tx.payload_24bit);
		else
			tone_gens((iis_page_size*PAGE_NUM*2), iis_tx.payload);
  #endif
//printk("(%d)", __LINE__);
		int print_size;
		print_size = (iis_page_size>270)? 270:iis_page_size;
#if (IIS_DBG_LVL>=3)
		printk("iis tx page: \n");
		for (i=0;i<(print_size*PAGE_NUM)*2+8;i++) {
#if 0
			if (iis_24bit_flag) {
				if ((i&1)==0)
					printk(" ");
			} else
				printk(" ");
			if (iis_24bit_flag)
				printk("%4x",iis_tx.payload[i]);
			else
				printk("%x",iis_tx.payload[i]);
			if (iis_24bit_flag) {
				if ((i&1)==1)
					printk(" ");
			} else
				printk(" ");

			if ((i%8) == 7)
				printk("\n");
#else
			if (iis_24bit_flag) {
				if (i > print_size*PAGE_NUM + 16)
					break;
				if ((i%8) == 7)
					printk(" %x\n",iis_tx.payload_24bit[i]);
				else
					printk(" %x ",iis_tx.payload_24bit[i]);
			} else {
				if ((i%8) == 7)
					printk(" %x\n",iis_tx.payload[i]&0xffff);
				else
					printk(" %x ",iis_tx.payload[i]&0xffff);
			}

#endif
		}
		//}
#endif /* (IIS_DBG_LVL>=3) */
#if 0
		memset(iis_rx.payload, 0xa5, (iis_page_size*PAGE_NUM+8)*4);
		printk("LINE:(%d)", __LINE__);
		printk("iis rx page: \n");
		for (i=0;i<(print_size*PAGE_NUM)*2+8;i++) {
			if ((i%8) == 7)
				printk(" %x\n",iis_rx.payload[i]);
			else
				printk(" %x ",iis_rx.payload[i]);
		}
#endif

/*
		printk("iis comp:\n");
		for (i=0;i<(iis_page_size*PAGE_NUM+16)*2;i++) {
			if ((i%8) == 7)
				printk(" %x\n",iis_comp[i]);
			else
				printk(" %x ",iis_comp[i]);

		}
*/
#endif

//#endif

#if 1
		// allocate buffer address
		rtl_outl(TX_PAGE_PTR,Logical2Physical((unsigned int)iis_tx.payload & 0xfffffff));
		rtl_outl(RX_PAGE_PTR,Logical2Physical((unsigned int)iis_rx.payload & 0xfffffff));
		// set TX RX owned by IIS controller
#if !defined(DELAY_OWNBIT)
		rtl_outl(IIS_TX_P0OWN,BIT(31));
#if 1
		rtl_outl(IIS_TX_P1OWN,BIT(31));
		rtl_outl(IIS_TX_P2OWN,BIT(31));
		rtl_outl(IIS_TX_P3OWN,BIT(31));
#endif
		//rtl_outl(IIS_TX_P0OWN,0);
		//rtl_outl(IIS_TX_P1OWN,0);
		//rtl_outl(IIS_TX_P2OWN,0);
		//rtl_outl(IIS_TX_P3OWN,0);

#if !defined(IIS_PLAY_TONE)
		rtl_outl(IIS_RX_P0OWN,BIT(31));
#if 1
		rtl_outl(IIS_RX_P1OWN,BIT(31));
		rtl_outl(IIS_RX_P2OWN,BIT(31));
		rtl_outl(IIS_RX_P3OWN,BIT(31));
#endif
#else
		rtl_outl(IIS_RX_P0OWN, 0);
		rtl_outl(IIS_RX_P1OWN, 0);
		rtl_outl(IIS_RX_P2OWN, 0);
		rtl_outl(IIS_RX_P3OWN, 0);

#endif
#else /* defined(DELAY_OWNBIT) */
		rtl_outl(IIS_TX_P0OWN, 0);
		rtl_outl(IIS_TX_P1OWN, 0);
		rtl_outl(IIS_TX_P2OWN, 0);
		rtl_outl(IIS_TX_P3OWN, 0);
		
		rtl_outl(IIS_RX_P0OWN, 0);
		rtl_outl(IIS_RX_P1OWN, 0);
		rtl_outl(IIS_RX_P2OWN, 0);
		rtl_outl(IIS_RX_P3OWN, 0);
#endif /* !defined(DELAY_OWNBIT) */

		printf("enable IIS  interrupt\n");
		rtl_outl(IIS_TX_IMR, 0x0f);
		rtl_outl(IIS_RX_IMR, 0x0f);
#endif
#if defined(IIS_CODEC_ALC5621) && defined(IIS_MIC_LOOPBACK_SPEAKER)
	memset(databuf, 0, DATABUFSIZE);
#endif

	rx_write_index=0;
	tx_read_index=iis_page_size*4;

	iis_txpage = 0;
	iis_rxpage = 0;
#if 0  //disable tr count check
	iis_tr_cnt = 0;
#endif
	iis_test_cnt = 0;
	iis_test_cnt_tx = 0;

}

void print_iis_regs(void)
{
#if (IIS_DBG_LVL>=2)
	printk("IISCR= 0x%x\n", rtl_inl(IISCR));
	printk("TX_PAGE_PTR= 0x%x\n", rtl_inl(TX_PAGE_PTR));
	printk("RX_PAGE_PTR= 0x%x\n", rtl_inl(RX_PAGE_PTR));
	printk("IIS_SETTING= 0x%x\n", rtl_inl(IIS_SETTING));
	printk("IIS_TX_IMR= 0x%x\n", rtl_inl(IIS_TX_IMR));
	printk("IIS_TX_ISR= 0x%x\n", rtl_inl(IIS_TX_ISR));
	printk("IIS_RX_IMR= 0x%x\n", rtl_inl(IIS_RX_IMR));
	printk("IIS_RX_ISR= 0x%x\n", rtl_inl(IIS_RX_ISR));
	printk("IIS_TX_P0OWN= 0x%x\n", rtl_inl(IIS_TX_P0OWN));
	printk("IIS_TX_P1OWN= 0x%x\n", rtl_inl(IIS_TX_P1OWN));
	printk("IIS_TX_P2OWN= 0x%x\n", rtl_inl(IIS_TX_P2OWN));
	printk("IIS_TX_P3OWN= 0x%x\n", rtl_inl(IIS_TX_P3OWN));
	printk("IIS_RX_P0OWN= 0x%x\n", rtl_inl(IIS_RX_P0OWN));
	printk("IIS_RX_P1OWN= 0x%x\n", rtl_inl(IIS_RX_P1OWN));
	printk("IIS_RX_P2OWN= 0x%x\n", rtl_inl(IIS_RX_P2OWN));
	printk("IIS_RX_P3OWN= 0x%x\n", rtl_inl(IIS_RX_P3OWN));
  #ifdef I2S_EN_SRAM
	printk("*0xb8001300= 0x%x, *0xb8001310= 0x%x\n", rtl_inl(0xb8001300), rtl_inl(0xb8001310));
	printk("*0xb8001320= 0x%x, *0xb8001330= 0x%x\n", rtl_inl(0xb8001320), rtl_inl(0xb8001330));
	printk("*0xb8004000= 0x%x, *0xb8004010= 0x%x\n", rtl_inl(0xb8004000), rtl_inl(0xb8004010));
	printk("*0xb8004020= 0x%x, *0xb8004030= 0x%x\n", rtl_inl(0xb8004020), rtl_inl(0xb8004030));
	printk("*0xb8001340= 0x%x, *0xb8001350= 0x%x\n", rtl_inl(0xb8001340), rtl_inl(0xb8001350));
	printk("*0xb8001360= 0x%x, *0xb8001370= 0x%x\n", rtl_inl(0xb8001360), rtl_inl(0xb8001370));
	printk("*0xb8004040= 0x%x, *0xb8004050= 0x%x\n", rtl_inl(0xb8004040), rtl_inl(0xb8004050));
	printk("*0xb8004060= 0x%x, *0xb8004070= 0x%x\n", rtl_inl(0xb8004060), rtl_inl(0xb8004070));
  #endif
#endif
}

void i2s_interrupt(void);

#ifndef PLATFORM_APOLLO_NON_LINUX
struct irqaction irq26 = {i2s_interrupt, NULL, 26,
             "iis", NULL, NULL};
#endif

#define rtlRegRead(addr)        \
        (*(volatile u32 *)addr)

#define rtlRegWrite(addr, val)  \
        ((*(volatile u32 *)addr) = (val))

static inline u32 rtlRegMask(u32 addr, u32 mask, u32 value)
{
	u32 reg;

	reg = rtlRegRead(addr);
	reg &= ~mask;
	reg |= value & mask;
	rtlRegWrite(addr, reg);
	reg = rtlRegRead(addr); /* flush write to the hardware */

	return reg;
}
int iis_rate_tab[]={8000, 16000, 24000, 32000, 0, 48000, 96000, 0};
int iis_rate_tab_44p1[]={0, 0, 0, 0, 0, 44100, 0, 0};
#define IIS_DEBUG_MODE	0
//setting[30]44.1kHz: 0->48khz(24.576Mhz) 1->44.1khz(22.579Mhz)
//setting[16:14], 0'b000->8k, 0'b001->16k, 0'b010->24k, 0'b011->32k, 0'b101->48k, 0'b110->96k, sampling_rate
//setting[10:1], iiscr config
//setting[10]DACLRSWAP: 0-> left phase, 1-> right phase.
//setting[9:8]FORMAT: 00-> I2S, 01->Left Justified, 10->Right Justified
//setting[7]LOOP_BACK: 0->disable, 1-> enable loop back
//setting[6]WL: 0-> 16bits, 1-> 24bits.
//setting[5]EDGE_SW: 0->negative edge, 1->positive edge
//setting[4:3]Audio_Mono: 00->stereo audio, 01->5.1 audio, 10->mono
//setting[2:1]TX_ACT: 00->RX_PATH, 01->TX_PATH, 10->TX_RX_PATH (not involve 5.1 audio)
void init_i2s(unsigned int setting)
{
	int i,j;

#if 0
	int *int_dest;
	int *int_src;
	unsigned char *byte_dest;
	unsigned char *byte_src;
#endif
	int sampling_rate;
	int tx_isr_arr[20];
	int rx_isr_arr[20];
#if defined(__m5280) || defined(__m4181) || defined(__m5181) || defined(__m5281)
	int cp3_read[20];
#endif
//printk("(%d)", __LINE__);
	sampling_rate = 0x1c000 & setting;
	//rtl_outl(IISCR,IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS
	rtl_outl(IISCR,0x80000000| (IIS_DEBUG_MODE<<15));	// stop IIS
for (j=0;j<5000;j++);
	rtl_outl(IISCR,0x0000| (IIS_DEBUG_MODE<<15));	// stop IIS
for (j=0;j<5000;j++);
	rtl_outl(IISCR,0x80000000| (IIS_DEBUG_MODE<<15));
for (j=0;j<5000;j++);
	rtl_outl(IIS_TX_ISR,rtl_inl(IIS_TX_ISR));// clear pending interrupt
for (j=0;j<5000;j++);
	rtl_outl(IIS_RX_ISR,rtl_inl(IIS_RX_ISR));// clear pending interrupt
for (j=0;j<5000;j++);
#if 1
	//rtl_outl(IIS_SETTING, (PAGE_SIZE - 1) | ((PAGE_NUM-1)<<12) | sampling_rate);	//set page size
#else
	rtl_outl(IIS_SETTING, (PAGE_SIZE - 1) | ((2-1)<<12) | sampling_rate);	//set page size
#endif
//printk("(%d)", __LINE__);
#ifndef PLATFORM_APOLLO_NON_LINUX
	/* Install interrupt handler */
	//int_Register(7, (1 << 19), 0, pcm_interrupt);		init interrupt in 8672

	request_IRQ(26, &irq26, NULL);

	/* Enable timer interrupt */
	//if ( 7 < getIlev() )
	//d	setIlev(7);
#endif
//printk("(%d)", __LINE__);
	if ((setting &(3<<3)) == 0)
		iis_allchannel=2;
	else if ((setting &(3<<3)) == (1<<3))
		iis_allchannel=6;
	else
		iis_allchannel=1;

	if (setting & (1<<6))
		iis_24bit_flag = 1;
	else
		iis_24bit_flag = 0;

	if (setting & (1<<30))
		iis_44p1khz_flag = 1;
	else
		iis_44p1khz_flag = 0;

	if (setting &(1<<7)) {
		iis_trx_short = 0;//internal loopback
		//printk("t");
		iis_internal_loopback=1;
	} else {
		iis_trx_short = 1;//trx short loopback
		//printk("T");
		iis_internal_loopback=0;
	}

	if (iis_44p1khz_flag)
		iis_rate=iis_rate_tab_44p1[sampling_rate>>14];
	else
		iis_rate=iis_rate_tab[sampling_rate>>14];
	//iis_24bit_wordlength=iis_24bit_flag;

	iis_page_size=((iis_rate/1000)*I2S_PAGE_TIME*iis_allchannel*(1<<iis_24bit_flag))/20;
	if (iis_page_size > 4096)
		iis_page_size = 4096;

rtl_outl(IIS_SETTING, (iis_page_size - 1) | ((PAGE_NUM-1)<<12) | sampling_rate);	//set page size
#if (IIS_DBG_LVL>=1)
	printk("%s", START_TEST_STR);
	printk("[test count:%d]", i2s__cnt++);
#endif
#ifndef IIS_CODEC_ALC5621
	printk("test mode:loopback test\n");
#else
  #ifdef IIS_PLAY_TONE
	printk("test mode: play tone, play_channel=%d\n", play_channel);
	sample_count = 0;
	tone_phase = 0;
  #else
	printk("test mode: mic loopback speaker\n");
  #endif
#endif
#if (IIS_DBG_LVL>=1)
	printk("sampling rate=%dHz, internal loopback=%s, ", iis_rate, iis_internal_loopback?"ON":"OFF");
	printk("allchannel=%d, word length=%s\n", iis_allchannel, iis_24bit_flag? "24bit" : "16bit");
	printk("PAGE_TIME=%d, page_size=%d, max_page_size=%d, page_num=%d\n", I2S_PAGE_TIME, iis_page_size, PAGE_SIZE, PAGE_NUM);
#endif
	// Enable IIS Channel
//printk("(%d)", __LINE__);
		init_i2s_core();

	//rtl_outl(IISCR, IIS_LOOP_BACK | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS
#if (IIS_DBG_LVL>=2)
		printk("iis_tx.payload = %X\n", iis_tx.payload);
		printk("iis_rx.payload = %X\n", iis_rx.payload);
  #if !defined(IIS_CODEC_ALC5621)
		printk("iis_comp = %X\n", iis_comp);
  #endif
#endif


	print_iis_regs();

	iis_isr_cnt = 0;
	iis_cpy_cnt = 0;
	//printk("iiscr = %X\n",IIS_LOOP_BACK | DACLRSWAP | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);
	//rtl_outl(IISCR, IIS_LOOP_BACK | DACLRSWAP | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS
	//printk("iiscr = %X\n",DACLRSWAP | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);
	//rtl_outl(IISCR, DACLRSWAP | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS
	//printk("iiscr = %X\n", IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);
	//rtl_outl(IISCR,  IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS
#if (IIS_DBG_LVL>=1)
	printk("iiscr = %X\n",0x80000000 |(setting & 0x400007fe) | IIS_ENABLE | (IIS_DEBUG_MODE<<15));
#endif
#if defined(MIPS_COUNTER)
	//write_32bit_cp0_register_sel(CP0_PERFORMANCE, 0, 0); // stop counter
	write_32bit_cp0_register_sel(25, 0, 0); // stop counter
	//write_32bit_cp0_register_sel(CP0_PERFORMANCE, 0, 1); // reset counter
	write_32bit_cp0_register_sel(25, 0, 1); // reset counter
	//write_32bit_cp0_register_sel(CP0_PERFORMANCE, ((CE0_CYCLES<<5)|CEB_KERNEL), 0);
	write_32bit_cp0_register_sel(25, ((CE0_CYCLES<<5)|CEB_KERNEL), 0);
	memset(iis_rx.payload, 0, 4096);
	//i=read_32bit_cp0_register_sel(CP0_PERFORMANCE, 1);
	i=read_32bit_cp0_register_sel(25, 1);
	j=0;
	printk("MIPS[%x,%x]", i, j);
#endif
#if defined(GIC_COUNTER)
	i = REG32(GIC_BASE_ADDR+0x10);
	j = REG32(GIC_BASE_ADDR+0x14);
	printk("GIC0[%x,%x]", j, i);
	REG32(GIC_BASE_ADDR+0) = 1<<28;//stop counter
	REG32(GIC_BASE_ADDR+0x10) = 0;//reset clear counter lo
	REG32(GIC_BASE_ADDR+0x14) = 0;//reset clear counter hi
	REG32(GIC_BASE_ADDR+0) = 0;//start counter
	memset(iis_rx.payload, 0, 4096);
	i = REG32(GIC_BASE_ADDR+0x10);
	j = REG32(GIC_BASE_ADDR+0x14);
	printk("GIC[%x,%x]", j, i);
#if 1
rtl_outl( 0xb8001300, 0 );////unmap seg addr
rtl_outl( 0xb8001310, 0 );////unmap seg addr
rtl_outl( 0xb8001320, 0 );////unmap seg addr
rtl_outl( 0xb8001330, 0 );////unmap seg addr
rtl_outl( 0xb8004000, 0 );////sram seg addr
rtl_outl( 0xb8004010, 0 );////sram seg addr
rtl_outl( 0xb8004020, 0 );////sram seg addr
rtl_outl( 0xb8004030, 0 );////sram seg addr

	REG32(GIC_BASE_ADDR+0) = 1<<28;//stop counter
	REG32(GIC_BASE_ADDR+0x10) = 0;//reset clear counter lo
	REG32(GIC_BASE_ADDR+0x14) = 0;//reset clear counter hi
	REG32(GIC_BASE_ADDR+0) = 0;//start counter
	memset(iis_rx.payload, 0, 4096);
	i = REG32(GIC_BASE_ADDR+0x10);
	j = REG32(GIC_BASE_ADDR+0x14);
	printk("GIC_DDR2[%x,%x]", j, i);
#endif
#endif
#if defined(__m5280) || defined(__m4181) || defined(__m5181) || defined(__m5281)
	enable_CP3();
	change_cp3_con_control0(0xff, CP3_COUNT_STOP);
	write_32bit_cp3_general_register(CP3_MONCNT0LO, 0);
	write_32bit_cp3_general_register(CP3_MONCNT0HI, 0);

#if 0
printk("[b]");
	change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);
	memset(iis_rx.payload, 0, 4096);
#if 0
printk("[cache word read]");
	int_dest = databuf;
	int_src = iis_tx_buf;
	for (i=0; i<32; i++) {
		*int_dest ++ = *int_src ++;
	}

printk("[cache short read]");
	int_dest = databuf;
	for (i=0; i<32; i++) {
		*int_dest ++ = iis_tx_buf[i];
	}

printk("[cache byte read]\n");
	//int_dest = databuf;
	byte_src = iis_tx_buf;
	for (i=0; i<32; i++) {
		databuf[i] = *byte_src ++;
	}


printk("[uncache word read]");
	int_dest = databuf;
	int_src = iis_tx.payload;
	for (i=0; i<32; i++) {
		*int_dest ++ = *int_src ++;
	}

printk("[uncache short read]");
	int_dest = databuf;
	for (i=0; i<32; i++) {
		*int_dest ++ = iis_tx.payload[i];
	}

printk("[uncache byte read]\n");
	int_dest = databuf;
	byte_src = iis_tx.payload;
	for (i=0; i<32; i++) {
		databuf[i] = *byte_src ++;
	}

printk("[cache word write]");
	int_src = databuf;
	int_dest = iis_tx_buf;
	for (i=0; i<32; i++) {
		*int_dest ++ = *int_src ++;
	}

//printk("[cache short write]");
	int_src = databuf;
	for (i=0; i<32; i++) {
		//iis_tx_buf[i] = *int_src ++;
	}

//printk("[cache byte write]\n");
	//int_src = databuf;
	byte_dest = iis_tx_buf;
	for (i=0; i<32; i++) {
//		*byte_dest ++ = databuf[i];
	}


printk("[uncache word write]");
	int_src = databuf;
	int_dest = iis_tx.payload;
	for (i=0; i<32; i++) {
		*int_dest ++ = *int_src ++;
	}

printk("[uncache short write]");
	int_src = databuf;
	for (i=0; i<32; i++) {
		iis_tx.payload[i] = *int_src ++;
	}

//printk("[uncache byte write]\n");
	//int_dest = databuf;
	byte_dest = iis_tx.payload;
	for (i=0; i<32; i++) {
//		*byte_dest ++ = databuf[i];
	}
#endif
	i=(int)cp3_counter0_get_64bit();

printk("[c]");
rtl_outl( 0xb8001340, 0 );////unmap seg addr
rtl_outl( 0xb8001350, 0 );////unmap seg addr
rtl_outl( 0xb8001360, 0 );////unmap seg addr
rtl_outl( 0xb8001370, 0 );////unmap seg addr
rtl_outl( 0xb8004040, 0 );////sram seg addr
rtl_outl( 0xb8004050, 0 );////sram seg addr
rtl_outl( 0xb8004060, 0 );////sram seg addr
rtl_outl( 0xb8004070, 0 );////sram seg addr

printk("[d]");
	change_cp3_con_control0(0xff, CP3_COUNT_STOP);
	write_32bit_cp3_general_register(CP3_MONCNT0LO, 0);
	write_32bit_cp3_general_register(CP3_MONCNT0HI, 0);

printk("[e]");
	change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);
	memset(iis_rx.payload, 0, 4096);
	j=(int)cp3_counter0_get_64bit();

printk("[f]");
	printk("[%x,%x]", i, j);
#endif
#endif
	unsigned int start_time;
	unsigned int pre_time;
	//volatile unsigned int get_timer_jiffies(void);
	start_time = get_timer_jiffies();
	iis_test_starttime = start_time;
#if (IIS_DBG_LVL>=1)
	printk("star_time=%x\n",start_time);
#endif
	i2s_test_done = 0;
#if 0
    rtlRegMask(0xb8000094, 0x00000FFF, 0x00000090);//case new iis
#endif
#if defined(__m5280) || defined(__m4181) || defined(__m5181) || defined(__m5281)
	change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);
#endif
	rtl_outl(IISCR,0x80000000 | (setting & 0x400007fe) | IIS_ENABLE | (IIS_DEBUG_MODE<<15) );	// 0->1 enable IIS

	unsigned int status_val_tx;
	unsigned int status_val_rx;


	status_val_tx = rtl_inl(IIS_TX_ISR);
	status_val_rx = rtl_inl(IIS_RX_ISR);

#ifdef DELAY_OWNBIT
		rtl_outl(IIS_RX_P0OWN, BIT(31));
		rtl_outl(IIS_TX_P0OWN, BIT(31));
		rtl_outl(IIS_RX_P1OWN, BIT(31));
		rtl_outl(IIS_RX_P2OWN, BIT(31));
		rtl_outl(IIS_RX_P3OWN, BIT(31));

		rtl_outl(IIS_TX_P1OWN, BIT(31));
		rtl_outl(IIS_TX_P2OWN, BIT(31));
		rtl_outl(IIS_TX_P3OWN, BIT(31));
#endif
	//printk("IISCR= 0x%x\n", rtl_inl(IISCR));
	pre_time = get_timer_jiffies();
#if 0
	while(get_timer_jiffies() < (start_time+20) )
	{
		if(get_timer_jiffies() > (pre_time ) )
		{
			rtl_outl(IISCR,0);
			printk("pre_time=%x,time=%x\n",pre_time,get_timer_jiffies());
			pre_time=get_timer_jiffies();
			rtl_outl(IISCR,(setting & 0x7fe) | IIS_ENABLE);	// 0->1 enable IIS
		}
	}
#endif



#if 0
	i=1;
	tx_isr_arr[0]=status_val_tx;
	rx_isr_arr[0]=status_val_rx;
  #if defined(__m5280) || defined(__m4181) || defined(__m5181) || defined(__m5281)
	cp3_read[0]=0;
  #endif
re_start:
  #if defined(__m5280) || defined(__m4181) || defined(__m5181) || defined(__m5281)
	cp3_read[i]=(int)cp3_counter0_get_64bit();
  #endif
	tx_isr_arr[i]=rtl_inl(IIS_TX_ISR);
	rx_isr_arr[i]=rtl_inl(IIS_RX_ISR);
	if ((tx_isr_arr[i]!=tx_isr_arr[i-1]) || (rx_isr_arr[i]!=rx_isr_arr[i-1]))
		i++;

	//if(get_timer_jiffies() < (start_time+200))
	if (!(rtl_inl(IIS_TX_ISR) & (1<<(PAGE_NUM-1))))
		goto re_start;

print_i2s_regs();

	//while(get_timer_jiffies() > (start_time+200) )
	{
	status_val_tx = rtl_inl(IIS_TX_ISR);
	status_val_rx = rtl_inl(IIS_RX_ISR);
	for (j=0;j<i; j++)
		printk(" [%X]trx_isr = %X,%X \n",cp3_read[j], tx_isr_arr[j], rx_isr_arr[j]);

		printk(" iis_txisr = %X \n", status_val_tx);
		printk(" iis_rxisr = %X \n", status_val_rx);
			int print_size;
			print_size = (iis_page_size>270)? 270:iis_page_size;
		//printk("LINE:(%d)", __LINE__);
			printk("iis rx page: \n");

			for (i=0;i<(print_size*PAGE_NUM)*2+8;i++) {
			    if (iis_24bit_flag) {
				if ((i%8) == 7)
					printk(" %x\n",iis_rx.payload_24bit[i]);
				else
					printk(" %x ",iis_rx.payload_24bit[i]);

			    } else {

				if ((i%8) == 7)
					printk(" %x\n",iis_rx.payload[i]);
				else
					printk(" %x ",iis_rx.payload[i]);
			    }
			}

	}
	i2s_isr_test_flag = 0;
#endif
}

void stop_iis(void)
{
	rtl_outl(IIS_TX_IMR,0);
	rtl_outl(IIS_RX_IMR,0);

#ifdef LOCK_LX_BUS
	lock_lx2_bus();
#endif

	rtl_outl(IISCR,0x0000);	// stop IIS

#ifdef LOCK_LX_BUS
	unlock_lx2_bus();
#endif

#if (IIS_DBG_LVL>=1)
	printk("%s", END_TEST_STR);
	printk("time=%x\n", get_timer_jiffies());
	printk("LINE:(%d)", __LINE__);
#endif
}

void iis_isr_test(int pindex);
void iis_isr_tx_test(int pindex);

static int shift=0;
void i2s_ISR(uint32 iis_txisr, uint32 iis_rxisr)
{
	uint32 chid = 0, i, j;

	uint32 tx_isrpage, rx_isrpage;


#if 0
	//printk("cnt=%d\n", isr_cnt);
    	if ( isr_cnt == 0) {
    		printk("iis_txisr = %x\n", iis_txisr);
    		printk("iis_rxisr = %x\n", iis_rxisr);
    	} else {
    		//printk("(%x, %x)\n", iis_txisr, iis_rxisr);
    	}
#endif

//printk("a");

	for (i=0; i < PAGE_NUM; i++) // page0/page1/page2/page3
	{
#if 0
		int need_PCM_RX[CH_NUM];
		memset(need_PCM_RX, 0, sizeof(need_PCM_RX));
#endif
	    	//printk("%d: chid =%d\n", isr_cnt, chid);
	    	//printk("%d", chid);

		if( iis_txisr & IISChanTxPage[iis_txpage] )
		{
#ifdef IIS_CODEC_ALC5621
			iis_isr_tx_test(iis_txpage);
#endif
			iis_set_tx_own_bit(iis_txpage);
			iis_txisr &= ~IISChanTxPage[iis_txpage];
			iis_txpage = (iis_txpage +1 ) % PAGE_NUM;
#if 0  //disable tr count check
			iis_tr_cnt++;
#endif
//printk("b");
		} // end of tx

		if( iis_rxisr & IISChanRxPage[iis_rxpage] ) {
//jwsyu 20130711 disable			iis_isr_test(iis_rxpage);
#if (I2S_PAGE_TIME>49)
			if (iis_rate < 50000)
				iis_isr_test(iis_rxpage);
#endif
			iis_cpy_cnt ++;
			iis_set_rx_own_bit(iis_rxpage);
			//iis_set_tx_own_bit(iis_txpage);
			//need_PCM_RX[chid] = 1;
			iis_rxisr &= ~IISChanRxPage[iis_rxpage];
			iis_rxpage = (iis_rxpage+1) % PAGE_NUM;
#if 0  //disable tr count check
			iis_tr_cnt--;
#endif
		}



#if 0
	    // Do PCM_RX()
	    for (j = 0; j < CH_NUM; j++)
	    {
		if (need_PCM_RX[j]) {
			need_PCM_RX[j] = 0;
			pcm_isr_test(chid, rxpage[chid]);
		}
	    }
#endif

	} // end of for i

	//printk("%d: 2 pcm_isr = %X \n", isr_cnt, pcm_isr);
#if 1
	if (iis_txisr != 0 || iis_rxisr != 0) {
		printk(" iis_txisr = %X \n", iis_txisr);
		printk(" iis_rxisr = %X \n", iis_rxisr);
	} else {
		//twiddle();
		//printk("%d",iis_isr_cnt);
	}
#endif

//if (iis_isr_cnt>2000){
if (iis_isr_cnt>(10000/I2S_PAGE_TIME*IIS_TEST_TIME_SEC)){
	//printk("iis_isr_cnt=%d, %d\n", iis_isr_cnt, iis_rate/8000*IIS_TEST_TIME_SEC);
				rtl_outl(IIS_TX_IMR,0);
				rtl_outl(IIS_RX_IMR,0);
				rtl_outl(IISCR,0x0000);	// stop IIS

				printk("%s", END_TEST_STR);
				printk("time=%x\n", get_timer_jiffies());

				print_iis_regs();
#if defined(IIS_CODEC_ALC5621) && defined(IIS_PLAY_TONE)
				printk("iis tx page: \n");
				if (iis_24bit_flag) {
					for (i=0;i<(iis_page_size*PAGE_NUM+16);i++) {
						if ((i%8) == 7)
							printk(" %x\n", iis_tx.payload_24bit[i]);
						else
							printk(" %x ", iis_tx.payload_24bit[i]);
					}
				} else {
					for (i=0;i<(iis_page_size*PAGE_NUM+8)*2;i++) {
						if ((i%8) == 7)
							printk(" %x\n", iis_tx.payload[i]&0xffff);
						else
							printk(" %x ", iis_tx.payload[i]&0xffff);
					}
				}

#else
				printk("iis rx page: \n");
				if (iis_24bit_flag) {
					for (i=0;i<(iis_page_size*PAGE_NUM+16);i++) {
						if ((i%8) == 7)
							printk(" %x\n", iis_rx.payload_24bit[i]);
						else
							printk(" %x ", iis_rx.payload_24bit[i]);
					}
				} else {
					for (i=0;i<(iis_page_size*PAGE_NUM+8)*2;i++) {
						if ((i%8) == 7)
							printk(" %x\n", iis_rx.payload[i]&0xffff);
						else
							printk(" %x ", iis_rx.payload[i]&0xffff);
					}
				}
#endif /* defined(IIS_CODEC_ALC5621) && defined(IIS_PLAY_TONE)  */
				i2s_isr_test_flag = 0;
				i2s_test_done = 1;
#ifndef IIS_CODEC_ALC5621
//check offset
//#if 0
	if(iis_24bit_flag) {
		if (iis_rx.payload_24bit[0] == iis_tx.payload_24bit[0]) {
			printk("\noffset=0\n"); shift=0;
		} else if (iis_rx.payload_24bit[1] == iis_tx.payload_24bit[0]) {
			printk("\noffset=1\n"); shift=1;
		} else if (iis_rx.payload_24bit[2] == iis_tx.payload_24bit[0]) {
			printk("\noffset=2\n"); shift=2;
		} else if (iis_rx.payload_24bit[3] == iis_tx.payload_24bit[0]) {
			printk("\noffset=3\n"); shift=3;
		} else if (iis_rx.payload_24bit[4] == iis_tx.payload_24bit[0]) {
			printk("\noffset=4\n"); shift=4;
		} else
			printk("\noffset error\n");
	} else {
		if (iis_rx.payload[0] == iis_tx.payload[0]) {
			printk("\noffset=0\n"); shift=0;
		} else if (iis_rx.payload[1] == iis_tx.payload[0]) {
			printk("\noffset=1\n"); shift=1;
		} else if (iis_rx.payload[2] == iis_tx.payload[0]) {
			printk("\noffset=2\n"); shift=2;
		} else if (iis_rx.payload[3] == iis_tx.payload[0]) {
			printk("\noffset=3\n"); shift=3;
		} else if (iis_rx.payload[4] == iis_tx.payload[0]) {
			printk("\noffset=4\n"); shift=4;
		} else
			printk("\noffset error\n");
	}
//#if (I2S_PAGE_TIME<50)
// check off line
#if defined(I2S_IN_FPGA) || (I2S_PAGE_TIME<50)
	if(iis_24bit_flag) {
		j=0;
				for (i=0;i<((iis_page_size*PAGE_NUM)-shift);i++) {

							if (iis_rx.payload_24bit[i+shift]!=iis_tx.payload_24bit[i]) {
								printk(" %d,%x!%x\n", i, iis_rx.payload_24bit[i+shift], iis_tx.payload_24bit[i]);
								j++;
							}
					if (j > 30)
						break;
				}
	
	
	} else {
		j=0;
				for (i=0;i<((iis_page_size*PAGE_NUM)*2-shift);i++) {

							if (iis_rx.payload[i+shift]!=iis_tx.payload[i]) {
								printk(" %d,%x!%x\n", i, iis_rx.payload[i+shift]&0xffff, iis_tx.payload[i]);
								j++;
							}
					if (j > 30)
						break;
				}
	
	}
#endif
#endif

  #ifdef I2S_EN_SRAM
  //test finish disable sram
#if defined(SOC_TYPE_8198C_CPU0_MIPS1074K)
  rtl_outl( 0xb8001300, 0);
  rtl_outl( 0xb8001310, 0);
  rtl_outl( 0xb8001320, 0);
  rtl_outl( 0xb8001330, 0);
  rtl_outl( 0xb8004000, 0);
  rtl_outl( 0xb8004010, 0);
  rtl_outl( 0xb8004020, 0);
  rtl_outl( 0xb8004030, 0);

#endif
#if defined(SOC_TYPE_8198C_CPU1_LEXRA5181)
  rtl_outl( 0xb8001340, 0);
  rtl_outl( 0xb8001350, 0);
  rtl_outl( 0xb8001360, 0);
  rtl_outl( 0xb8001370, 0);
  rtl_outl( 0xb8004040, 0);
  rtl_outl( 0xb8004050, 0);
  rtl_outl( 0xb8004060, 0);
  rtl_outl( 0xb8004070, 0);
#endif
  #endif

}

}



void i2s_interrupt()
{
	unsigned int status_val_tx;
	unsigned int status_val_rx;
	int i;

	tick_timer_jiffies();

	status_val_tx = rtl_inl(IIS_TX_ISR);
	status_val_rx = rtl_inl(IIS_RX_ISR);
	if( status_val_tx || status_val_rx )
//	if( status_val_rx )
	{
		rtl_outl(IIS_TX_ISR, status_val_tx);
		rtl_outl(IIS_RX_ISR, status_val_rx);

		i2s_ISR(status_val_tx & 0x0F, status_val_rx & 0x0F);

		if( ((status_val_tx & 0x30)==0x30 )|| ((status_val_rx & 0x30)==0x30 )) // Buffer Unavailable only
		{
			//printk("\n%s-%d: BU, isr_cnt = %d: status_val_tx=%x, status_val_rx=%x\n", __FUNCTION__, __LINE__, iis_isr_cnt, status_val_tx, status_val_rx);
			//printk("U");
#if 0
			print_iis_regs();
			rtl_outl(IIS_TX_IMR, 0xf0);
			rtl_outl(IIS_RX_IMR, 0xf0);
			rtl_outl(IISCR,0x0000);	// stop IIS

			print_iis_regs();
			while (1) ;
#endif
		}

		if( ((status_val_tx & 0x10) )|| ((status_val_rx & 0x10))) // page Unavailable only
		{
			//printk("\n%s-%d: BU, isr_cnt = %d: status_val_tx=%x, status_val_rx=%x\n", __FUNCTION__, __LINE__, iis_isr_cnt, status_val_tx, status_val_rx);
#if 0
			printk("PUtx=%x,rx=%x",status_val_tx,status_val_rx);
#else
			//printk("PU");
#endif
		}

		if( ((status_val_tx & 0x20) )|| ((status_val_rx & 0x20))) // fifo Unavailable only
		{
			//printk("\n%s-%d: BU, isr_cnt = %d: status_val_tx=%x, status_val_rx=%x\n", __FUNCTION__, __LINE__, iis_isr_cnt, status_val_tx, status_val_rx);
			//printk("FUtx=%x\n",status_val_tx);
			//printk("FUrx=%x\n",status_val_rx);
			//printk("FU");
		}
#if 0
		if(status_val_rx & 0x10)
		{
		printk("LINE:(%d)", __LINE__);
			printk("iis rx page: \n");
			for (i=0;i<(iis_page_size*PAGE_NUM)*2+8;i++) {
				if ((i%8) == 7)
					printk(" %x\n",iis_rx.payload[i]);
				else
					printk(" %x ",iis_rx.payload[i]);
			}
		}
#endif
	}

	iis_isr_cnt++;
}




void iis_isr_test(int pindex)
{
	int 		i;
	static unsigned int txpindex = 0;

		{  // check rx data
#if !defined(IIS_CODEC_ALC5621)
			static unsigned int err_cnt = 0;//error page
			unsigned int err_cnt_in = 0;
			static unsigned int err_sample=0;

			//rtl_outl(CH0ATXBSA,  rtl_inl(CH0ATXBSA) | BIT(txpindex)); // set tx own bit ASAP, solve ERROR!!

//#if PKTSIZE == 39 // partial test for reducing computation
#if (I2S_PAGE_TIME<50)
			for (i=0;i<iis_page_size*2;i=i+60)
#else
  #if defined(I2S_IN_FPGA)
			for (i=0;i<iis_page_size*2;i+=40)
  #else // full test
			for (i=0;i<iis_page_size*2;i++)
  #endif
#endif
			{

				int rxi;
				int cmp;
				int cmp2=0;
				int cmp3=0;
			    if (iis_24bit_flag) {
			    	if (i >= iis_page_size)
			    		break;
				rxi  = *(iis_rx.payload_24bit + iis_page_size*pindex + i);
				if (iis_trx_short)
#if defined(SOC_TYPE_8881A)
					cmp = iis_comp_24bit[iis_page_size*pindex + i];
#else
					if (i<2 && pindex==0) {
						if (iis_test_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp_24bit[iis_page_size*PAGE_NUM +i -2];
					} else {
						cmp = iis_comp_24bit[iis_page_size*pindex + i -2];
					}
#endif
				else {
#if 1
					if (i<2 && pindex==0) {
						if (iis_test_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp_24bit[iis_page_size*PAGE_NUM +i -2];
					} else {
						cmp = iis_comp_24bit[iis_page_size*pindex + i -2];
					}

#else
					cmp = iis_comp_24bit[iis_page_size*pindex + i];

#endif
				}

					cmp2 = iis_comp_24bit[iis_page_size*pindex + i];
				if (( rxi != cmp) && ( rxi != cmp2 ) ) { //u-law fixed pattern
					//printfByPolling("\n%d-%d-%d\n", iis_test_cnt , i, pindex);
					//if (err_cnt_in<3) {
					printfByPolling("\n%d-%d-%d-%d\n", iis_isr_cnt, iis_test_cnt , i, pindex);
					printfByPolling("rxi =%x\n", rxi);
					printfByPolling("cmp =%x\n", cmp);
					//}
					//printfByPolling("iis_rx.payload=%x\n", iis_rx.payload);
					//printfByPolling("iis_comp=%x\n", iis_comp);
					//print_iis_regs();
					//while (1)
							;
					if ( err_cnt_in == 0)
						err_cnt++;
					err_cnt_in++;
				}
			    }else {
				rxi = *(iis_rx.payload + iis_page_size*2*pindex + i);
				if (iis_trx_short) {
#if 1
				    if(iis_allchannel==1) {
					if (i<2 && pindex==0) {
						if (iis_test_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp[iis_page_size*2*PAGE_NUM + i -2];
							cmp3 = iis_comp[iis_page_size*2*PAGE_NUM + i -1];
					} else {
						cmp = iis_comp[iis_page_size*2*pindex + i -2];
						cmp3 = iis_comp[iis_page_size*2*pindex + i -1];
					}
				    	
				    } else {
					if (i<2 && pindex==0) {
						if (iis_test_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp[iis_page_size*2*PAGE_NUM + i -2];
					} else {
						cmp = iis_comp[iis_page_size*2*pindex + i -2];
					}
				    	
				    }

#else
				    if(iis_allchannel==1) {
					if (i==0 && pindex==0) {
						if (iis_test_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp[iis_page_size*2*PAGE_NUM -1];
					} else {
						cmp = iis_comp[iis_page_size*2*pindex + i -1];
					}
				    } else {
#if defined(SOC_TYPE_8881A)
					cmp = iis_comp[iis_page_size*2*pindex + i];
#else
					if (i<2 && pindex==0) {
						if (iis_test_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp[iis_page_size*2*PAGE_NUM -2];
					} else {
						cmp = iis_comp[iis_page_size*2*pindex + i -2];
					}
#endif
				    	
				    }
#endif
				} else {
#if 1
				    if(iis_allchannel==1) {
					if (i<2 && pindex==0) {
						if (iis_test_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp[iis_page_size*2*PAGE_NUM + i -2];
						if (i<1)
							cmp3 = iis_comp[iis_page_size*2*PAGE_NUM + i -1];
						else
							cmp3 = iis_comp[0];
					} else {
						cmp = iis_comp[iis_page_size*2*pindex + i -2];
						cmp3 = iis_comp[iis_page_size*2*pindex + i -1];
					}
				    	
				    } else {
					if (i<2 && pindex==0) {
						if (iis_test_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp[iis_page_size*2*PAGE_NUM + i -2];
							cmp3 = iis_comp[iis_page_size*2*PAGE_NUM + i -4];
					} else {
						cmp = iis_comp[iis_page_size*2*pindex + i -2];
						cmp3 = iis_comp[iis_page_size*2*pindex + i -4];
					}
				    	
				    }

#else
					cmp = iis_comp[iis_page_size*2*pindex + i];
#endif
				}

					cmp2 = iis_comp[iis_page_size*2*pindex + i];
				if (( rxi != cmp) && ( rxi != cmp2 ) && ( rxi != cmp3 ) ) { //u-law fixed pattern
					//if(0){
					printfByPolling("\n%d-%d-%d-%d\n", iis_isr_cnt, iis_test_cnt , i, pindex);
					//if (err_cnt_in<3) {
					printfByPolling("%d, rxi =%x,", i, rxi);
					printfByPolling("cmp =%x\n", cmp);
					//}
					//printfByPolling("iis_rx.payload=%x\n", iis_rx.payload);
					//printfByPolling("iis_comp=%x\n", iis_comp);
					//print_iis_regs();
					//while (1)
							;
					if ( err_cnt_in == 0)
						err_cnt++;
					err_cnt_in++;
				} else {
					//printfByPolling(".", cmp);

				}
			    }
#if (I2S_PAGE_TIME>=50) && defined(I2S_IN_FPGA)
				if (iis_rate > 8000)
					i+=31;
				if (iis_rate > 48000)
					i+=480;
#endif
			}

			err_sample += err_cnt_in; // the sum of error sample.
			//printf("\nj=%d (%d,%d)\n", j, err_cnt, err_cnt_in); // induces ERROR if the line above not exist!
			if (err_cnt_in)
				iis_breakpoint();
				//goto label1;
			if (err_cnt_in) {
#ifndef I2S_IN_FPGA
				printfByPolling("\niis_test_cnt=%d (%d,%d)\n", iis_test_cnt, err_cnt, err_sample);
#endif
			} else {
#ifndef I2S_IN_FPGA
				twiddle();
#else
				if ((iis_test_cnt&0xff)==0x40)
					printk(".");
#endif
				//printk("%d", chid+1);
			}

			int temp_v;
			temp_v = get_timer_jiffies();
			temp_v = temp_v - iis_test_starttime;
			//if (dot_cnt >10)
			//if (0)
			if ( (temp_v > IIS_TEST_TIME_SEC*100)
//			    ||(err_cnt_in)){
				) {
				rtl_outl(IIS_TX_IMR,0);
				rtl_outl(IIS_RX_IMR,0);
#ifdef LOCK_LX_BUS
				lock_lx2_bus();
#endif
				rtl_outl(IISCR,0x0000);	// stop IIS
#ifdef LOCK_LX_BUS
				unlock_lx2_bus();
#endif

				//volatile unsigned int get_timer_jiffies(void);
#if (IIS_DBG_LVL>=1)

				printk("%s", END_TEST_STR);
				printk("time=%x\n", get_timer_jiffies());
#endif
				print_iis_regs();
#if 0
				printk("iis rx page: \n");
				for (i=0;i<(PAGE_SIZE*PAGE_NUM)*2;i++) {
					if ((i%8) == 7)
						printk(" %x\n",iis_dump[i]);
					else
						printk(" %x ",iis_dump[i]);
				}
#else
#if (IIS_DBG_LVL>=3)
		printk("LINE:(%d)", __LINE__);
				printk("iis rx page: \n");
				if (iis_24bit_flag) {
					for (i=0;i<(iis_page_size*PAGE_NUM+8);i++) {
						if ((i%8) == 7)
							printk(" %x\n", iis_rx.payload_24bit[i]);
						else
							printk(" %x ", iis_rx.payload_24bit[i]);
					}
				} else {
					for (i=0;i<((iis_page_size*PAGE_NUM)*2+8);i++) {
						if ((i%8) == 7)
							printk(" %x\n", iis_rx.payload[i]);
						else
							printk(" %x ", iis_rx.payload[i]);
					}
				}
#endif /* (IIS_DBG_LVL>=3) */
#endif
				i2s_isr_test_flag = 0;
			}
#endif /* !defined(IIS_CODEC_ALC5621) */
#if defined(IIS_CODEC_ALC5621) && defined(IIS_MIC_LOOPBACK_SPEAKER)
			memcpy(&databuf[rx_write_index], iis_rx.payload + iis_page_size*2*pindex, iis_page_size*4);
			rx_write_index=(rx_write_index+iis_page_size*4)%DATABUFSIZE;
#endif
#if 0
			if(iis_cpy_cnt < PAGE_NUM)
				memcpy(iis_dump + iis_page_size*2*iis_test_cnt ,iis_rx.payload + iis_page_size*2*pindex,iis_page_size*4);
#endif

#if PAGE_SIZE != 40
			// clean RX buf
			//memset(iis_rx.payload + PAGE_SIZE*4*pindex, 0, PAGE_SIZE*4);
#endif
		} // check rx data

		if (iis_test_cnt%1000 == 0) {
			//printfByPolling("j=%d\n", j);
			static int dot_cnt = 0;
			int temp_v;
			if (dot_cnt++ % 80 == 0)
				printfByPolling("\n");
			temp_v = get_timer_jiffies();
			temp_v = temp_v - iis_test_starttime;
			//if (dot_cnt >10)
			//if (0)
			if (temp_v > IIS_TEST_TIME_SEC*100) {
				rtl_outl(IIS_TX_IMR,0);
				rtl_outl(IIS_RX_IMR,0);
#ifdef LOCK_LX_BUS
				lock_lx2_bus();
#endif
				rtl_outl(IISCR,0x0000);	// stop IIS

#ifdef LOCK_LX_BUS
				unlock_lx2_bus();
#endif

				//volatile unsigned int get_timer_jiffies(void);
				printk("%s", END_TEST_STR);
				printk("time=%x\n", get_timer_jiffies());

				print_iis_regs();
#if 0
				printk("iis rx page: \n");
				for (i=0;i<(iis_page_size*PAGE_NUM)*2;i++) {
					if ((i%8) == 7)
						printk(" %x\n",iis_dump[i]);
					else
						printk(" %x ",iis_dump[i]);
				}
#else
#if (IIS_DBG_LVL>=3)
		printk("LINE:(%d)", __LINE__);
				printk("iis rx page: \n");
				if (iis_24bit_flag) {
					for (i=0;i<(iis_page_size*PAGE_NUM);i++) {
						if ((i%8) == 7)
							printk(" %x\n", iis_rx.payload_24bit[i]);
						else
							printk(" %x ", iis_rx.payload_24bit[i]);
					}
				} else {
					for (i=0;i<(iis_page_size*PAGE_NUM)*2;i++) {
						if ((i%8) == 7)
							printk(" %x\n", iis_rx.payload[i]);
						else
							printk(" %x ", iis_rx.payload[i]);
					}
				}
#endif
#endif
				i2s_isr_test_flag = 0;
				i2s_test_done = 1;
			}
		}

		iis_test_cnt++;

			//if (iis_test_cnt >1000)
			if(0)
			{
				rtl_outl(IIS_TX_IMR,0);
				rtl_outl(IIS_RX_IMR,0);
				rtl_outl(IISCR,0x0000);	// stop IIS

				//volatile unsigned int get_timer_jiffies(void);
				printk("time=%x\n",get_timer_jiffies());

				print_iis_regs();
				printk("iis rx page: \n");
				for (i=0;i<(iis_page_size*PAGE_NUM)*2;i++) {
#if 0
					if ((i%8) == 7)
						printk(" %x\n",iis_dump[i]);
					else
						printk(" %x ",iis_dump[i]);
#endif
				}

				i2s_isr_test_flag = 0;
				i2s_test_done = 1;
			}


}

void iis_isr_tx_test(int pindex)
{
#if defined(IIS_PLAY_TONE)
	if (iis_24bit_flag)
		tone_gens_24bit(iis_page_size, iis_tx.payload_24bit+iis_page_size*pindex);
	else
		tone_gens( iis_page_size*2, iis_tx.payload + iis_page_size*2*pindex);
  #ifndef I2S_IN_FPGA
	twiddle();
  #endif
#endif
#if defined(IIS_CODEC_ALC5621) && defined(IIS_MIC_LOOPBACK_SPEAKER)
	memcpy(iis_tx.payload + iis_page_size*2*pindex, &databuf[tx_read_index], iis_page_size*4);
	tx_read_index= (tx_read_index+iis_page_size*4)%DATABUFSIZE;
		iis_test_cnt_tx++;
  #ifndef I2S_IN_FPGA
	twiddle();
  #endif
#endif
	int temp_v;
	temp_v = get_timer_jiffies();
	temp_v = temp_v - iis_test_starttime;
			//if (iis_test_cnt_tx >1000)
			//if (iis_test_cnt_tx >10000)
			//if (temp_v > IIS_TEST_TIME_SEC*100)
			if (0)
			{
				rtl_outl(IIS_TX_IMR,0);
				rtl_outl(IIS_RX_IMR,0);
#ifdef LOCK_LX_BUS
				lock_lx2_bus();
#endif
				rtl_outl(IISCR,0x0000);	// stop IIS

#ifdef LOCK_LX_BUS
				unlock_lx2_bus();
#endif

				//volatile unsigned int get_timer_jiffies(void);
#if (IIS_DBG_LVL>=1)
				printk("time=%x\n",get_timer_jiffies());
				printk("LINE:(%d)\n", __LINE__);
				print_iis_regs();
#endif
/*				printk("iis rx page: \n");
				for (i=0;i<(iis_page_size*PAGE_NUM)*2;i++) {
					if ((i%8) == 7)
						printk(" %x\n",iis_dump[i]);
					else
						printk(" %x ",iis_dump[i]);
				}
*/
				i2s_isr_test_flag = 0;
				i2s_test_done = 1;
			}

}


#if defined(IIS_CODEC_ALC5621) && defined(IIS_PLAY_TONE)
int16_t sinus(int32_t x)
{
	int16_t i;
	int32_t x2;												// Q15
	int32_t q;
	int32_t res=0;
	int16_t coef[5] = { (int16_t)0x3240, (int16_t)0x0054, (int16_t)0xaacc,
					   (int16_t)0x08B7, (int16_t)0x1cce };	// Q12
	if (x > 0x00008000L)
		x2 = x - 0x00008000L;
	else
		x2 = x;

	if (x2 > 0x00004000L)
		x2 = 0x00008000L - x2;
	q = x2;


	for (i=0; i<5; i++)
	{
		res += coef[i]*x2;											// Q27
		x2 *= q;													// Q30
		x2 >>= 15;													// Q15
	}

	res >>= 12;	 /* back to 0x0000-0xFFFF */						// Q15
	if (x > 0x00008000L)
		res = -res;
	if (res > 0 && res > 32767)
		res = 32767;
	else
		if (res < 0 && res < -32768)
			res = -32768;

	return (int16_t)res;
}


void tone_gens(int32_t sample_num, int16_t *buffPtr)
{
	int32_t i;

	if (play_channel>=iis_allchannel) {
		if (iis_allchannel==1)
			play_channel_now=0;
		else
			play_channel_now=play_channel%iis_allchannel;
	} else
		play_channel_now = play_channel;

	for(i=0;i<sample_num;i++)
	{

		if (sample_count==play_channel_now) {
			//printk(".");
			*buffPtr=sinus(tone_phase)>>2;
			tone_phase += tone_phase_ad;
			if (tone_phase > 65535)
				tone_phase -= 65535;
		} else {
#ifndef I2S_IN_FPGA
			*buffPtr=0;
#endif
			//printk("-");
		}
		sample_count++;
		if (sample_count>=iis_allchannel)
			sample_count=0;
		buffPtr++;
	}
}



void tone_gens_24bit(int32_t sample_num, int32_t *buffPtr)
{
	int32_t i;

	if (play_channel>=iis_allchannel) {
		if (iis_allchannel==1)
			play_channel_now=0;
		else
			play_channel_now=play_channel%iis_allchannel;
	} else
		play_channel_now = play_channel;

	for(i=0;i<sample_num;i++)
	{

		if (sample_count==play_channel_now) {
			//printk(".");
#if 1
			*buffPtr=((int)(sinus(tone_phase))<<6)&0x00ffffff;
			tone_phase += tone_phase_ad;
			if (tone_phase > 65535)
				tone_phase -= 65535;
#else
			*buffPtr=0xffffff;
#endif
		} else {
#ifndef I2S_IN_FPGA
			*buffPtr=0;
#endif
			//printk("-");
		}
		sample_count++;
		if (sample_count>=iis_allchannel)
			sample_count=0;
		buffPtr++;
	}
}
#endif

