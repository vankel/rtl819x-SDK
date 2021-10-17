/*
 *	Realtek IIS Controller Driver
 *
 *
 *
 *
 *
 *
 */

//#define IIS_BASE (0xb8009000) //8198 base address
#define IIS_BASE (0xb801F000)  //8196d base address

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

#define REG32(reg) (*(volatile unsigned long *)reg)
#define BIT(x)	( 1 << (x))
#define rtl_outl(address, value)	(REG32(address) = value)
#define rtl_inl(address)			REG32(address)

#define Virtual2NonCache(x)             (((int)x) | 0x20000000)


#define IIS_CODEC_ALC5621 1
#define IIS_TEST_TIME_MIN 1
#ifdef IIS_CODEC_ALC5621
#define IIS_PLAY_TONE 1
//#define IIS_MIC_LOOPBACK_SPEAKER 1
#endif
//#include <rtl8650/asicregs.h>
#include <rtl_types.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/types.h>

#include <asm/lexraregs.h>

//#define PAGE_SIZE	40	// 80 * 32bit, 160sample, 20ms
#define PAGE_SIZE	(80*3)	// 80 * 32bit, 160sample, 20ms
//#define PAGE_SIZE	81	// 80 * 32bit, 160sample, 20ms
//#define PAGE_SIZE	(84)	// 84 * 32bit, 168sample, 21ms
//#define PAGE_SIZE	(4000)	// 84 * 32bit, 168sample, 21ms
#if (PAGE_SIZE>4096)
  #error "max page size supported is 4096"
#endif

#define PAGE_NUM	4
#define IIS_SAMPLE_RATE	1	//0->8khz , 1->16khz

#define DATABUFSIZE	(200*PAGE_SIZE*4)

static short iis_tx_buf[PAGE_NUM*PAGE_SIZE*2*4+256]__attribute__ ((aligned (32)));
static short iis_rx_buf[PAGE_NUM*PAGE_SIZE*2*4+256]__attribute__ ((aligned (32)));

volatile int iis_isr_test_flag;

#define printfByPolling prom_printf
#define printf prom_printf

struct iis_pkt {
	unsigned char 	tx_rx;
	//unsigned char 	*payload;
	short 	*payload;
	short 	buf[PAGE_SIZE*2]; // mult 2 because of short type!
};
static struct iis_pkt iis_tx;
static struct iis_pkt iis_rx;

unsigned char databuf[DATABUFSIZE];
uint32_t tx_read_index, rx_write_index;

#define printk printfByPolling

int iis_txpage;
int iis_rxpage;
int iis_tr_cnt;

static int iis_trx_short = 0;
static int iis_allchannel = 0;

static int iis_isr_cnt = 0;
static int iis_cpy_cnt = 0;

static unsigned long IISChanTxPage[4] = {IIS_TX_P0OK, IIS_TX_P1OK, IIS_TX_P2OK, IIS_TX_P3OK};
static unsigned long IISChanRxPage[4] = {IIS_RX_P0OK, IIS_RX_P1OK, IIS_RX_P2OK, IIS_RX_P3OK};

short iis_comp[PAGE_NUM*PAGE_SIZE*2];
short iis_dump[PAGE_NUM*PAGE_SIZE*2];
static int iis_break_point = 0;
static unsigned int iis_test_cnt;
static unsigned int iis_test_cnt_tx;

static int iis_tx_index;
static int iis_24bit_flag;

static int iis_test_starttime;
volatile int get_timer_jiffies(void);

void tone_gens(int32_t sample_num, int16_t *buffPtr);
void tone_gens_24bit(int32_t sample_num, int32_t *buffPtr);
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

#endif

static void enable_CP3(void)
{
	__write_32bit_c0_register(CP0_STATUS, 0, __read_32bit_c0_register(CP0_STATUS, 0)|0x80000000);
}

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


void init_iis_core(void)
{
	int i;
#if 1
	iis_tx.payload = (unsigned short *) Virtual2NonCache(iis_tx_buf);
	iis_rx.payload = (unsigned short *) Virtual2NonCache(iis_rx_buf);
#else
	//tx[chid].payload = (unsigned short *) (0xa0c00000 + chid * 0x10000);
	//rx[chid].payload = (unsigned short *) (0xa0d00000 + chid * 0x10000);
	iis_tx.payload = (unsigned short *) (0xa0300000 );
	iis_rx.payload = (unsigned short *) (0xa0340000 );//0x40000=256kbyte = 64k word
	//iis_tx.payload = (unsigned short *) (0xa0180000 ); //0x8000=32kbyte, = 8k word
	//iis_rx.payload = (unsigned short *) (0xa0188000 );
#endif
	iis_tx_index=0;
//printk("(%d)", __LINE__);
#if 1 //for g711 testing. chiminer
		//printf("comp_expand: \n");
		//random seed,
		short random_add;
		random_add=(short)(get_timer_jiffies()&0xffff);

		for (i=0;i<(PAGE_SIZE*PAGE_NUM)*2;i++) {
#if 0
			short raw_pcm = (i+101)*30-67;//(i+9)*7-19;
			if (i%2 == 1)
				raw_pcm |= 0x8000;
#else
			short raw_pcm = iis_test_data[(i>>1)%16];
			//short raw_pcm = iis_test_data[i%40];
			//short raw_pcm = i;
#endif
			short back;


			iis_tx.payload[i] = raw_pcm;
			//iis_tx.payload[i] = i+5;
			//iis_tx.payload[i] = 0;
			//iis_tx.payload[i] = 0x5550 | iis_tx_index;
			//iis_tx.payload[i] = 0xA5B0 | iis_tx_index;
			//iis_tx_index = (iis_tx_index +1 )%6;
			iis_comp[i]=raw_pcm;
		}
#if 1
//printk("(%d)", __LINE__);
  #if defined(IIS_CODEC_ALC5621) && 1
		if (iis_24bit_flag)
			tone_gens_24bit((PAGE_SIZE*PAGE_NUM), iis_tx.payload);
		else
			tone_gens((PAGE_SIZE*PAGE_NUM), iis_tx.payload);
  #endif
//printk("(%d)", __LINE__);
		int print_size;
		print_size = (PAGE_SIZE>90)? 90:PAGE_SIZE;
		//if(PAGE_SIZE < 90){
		printk("iis tx page: \n");
		for (i=0;i<(print_size*PAGE_NUM)*2+8;i++) {
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
		}
		//}

#if 0
		memset(iis_rx.payload,0, (PAGE_SIZE*PAGE_NUM)*4);
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
		for (i=0;i<(PAGE_SIZE*PAGE_NUM)*2;i++) {
			if ((i%8) == 7)
				printk(" %x\n",iis_comp[i]);
			else
				printk(" %x ",iis_comp[i]);

		}
*/
#endif

#endif

#if 1
		// allocate buffer address
		rtl_outl(TX_PAGE_PTR,(unsigned int)iis_tx.payload & 0xfffffff);
		rtl_outl(RX_PAGE_PTR,(unsigned int)iis_rx.payload & 0xfffffff);
		// set TX RX owned by IIS controller
		rtl_outl(IIS_TX_P0OWN,BIT(31));
#if 1
		rtl_outl(IIS_TX_P1OWN,BIT(31));
		rtl_outl(IIS_TX_P2OWN,BIT(31));
		rtl_outl(IIS_TX_P3OWN,BIT(31));
#endif

#if 0
		rtl_outl(IIS_RX_P0OWN,BIT(31));
#if 1
		rtl_outl(IIS_RX_P1OWN,BIT(31));
		rtl_outl(IIS_RX_P2OWN,BIT(31));
		rtl_outl(IIS_RX_P3OWN,BIT(31));
#endif
#endif

		printf("enable IIS  interrupt\n");
		rtl_outl(IIS_TX_IMR, 0x0f);
		//rtl_outl(IIS_RX_IMR, 0x3f);
#endif

	memset(databuf,0,DATABUFSIZE);


	rx_write_index=0;
	tx_read_index=PAGE_SIZE*4;

	iis_txpage = 0;
	iis_rxpage = 0;
	iis_tr_cnt = 0;
	iis_test_cnt = 0;
	iis_test_cnt_tx = 0;

}

void print_iis_regs(void)
{
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
}

void iis_interrupt(void);

struct irqaction irq26 = {iis_interrupt, NULL, 26,
             "iis", NULL, NULL};

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

#define IIS_DEBUG_MODE	0
//setting[16:14], 0'b000->8k, 0'b001->16k, 0'b010->24k, 0'b011->32k, 0'b101->48k, sampling_rate
//setting[10:1], iiscr config
//setting[10]DACLRSWAP: 0-> left phase, 1-> right phase.
//setting[9:8]FORMAT: 00-> I2S, 01->Left Justified, 10->Right Justified
//setting[7]LOOP_BACK: 0->disable, 1-> enable loop back
//setting[6]WL: 0-> 16bits, 1-> 24bits.
//setting[5]EDGE_SW: 0->negative edge, 1->positive edge
//setting[4:3]Audio_Mono: 00->stereo audio, 01->5.1 audio, 10->mono
//setting[2:1]TX_ACT: 00->RX_PATH, 01->TX_PATH, 10->TX_RX_PATH (not involve 5.1 audio)
void init_iis(unsigned int setting)
{
	int i,j;

	int sampling_rate;
	int tx_isr_arr[20];
	int rx_isr_arr[20];
	int cp3_read[20];
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
	//rtl_outl(IIS_SETTING, PAGE_SIZE | ((PAGE_NUM-1)<<12) | (IIS_SAMPLE_RATE << 14) );	//set page size
#if 1
	rtl_outl(IIS_SETTING, (PAGE_SIZE - 1) | ((PAGE_NUM-1)<<12) | sampling_rate);	//set page size
#else
	rtl_outl(IIS_SETTING, (PAGE_SIZE - 1) | ((2-1)<<12) | sampling_rate);	//set page size
#endif
//printk("(%d)", __LINE__);
#if 1
	/* Install interrupt handler */
	//int_Register(7, (1 << 19), 0, pcm_interrupt);		init interrupt in 8672

	request_IRQ(26, &irq26,NULL);

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

	printk("iis_allchannel=%d, 24bit=%d\n", iis_allchannel, iis_24bit_flag);

	// Enable IIS Channel
//printk("(%d)", __LINE__);
		init_iis_core();

	//rtl_outl(IISCR, IIS_LOOP_BACK | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS

		printk("iis_tx.payload = %X\n", iis_tx.payload);
		printk("iis_rx.payload = %X\n", iis_rx.payload);
		printk("iis_comp = %X\n", iis_comp);



	print_iis_regs();

	iis_isr_cnt = 0;
	iis_cpy_cnt = 0;
	//printk("iiscr = %X\n",IIS_LOOP_BACK | DACLRSWAP | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);
	//rtl_outl(IISCR, IIS_LOOP_BACK | DACLRSWAP | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS
	//printk("iiscr = %X\n",DACLRSWAP | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);
	//rtl_outl(IISCR, DACLRSWAP | IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS
	//printk("iiscr = %X\n", IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);
	//rtl_outl(IISCR,  IIS_WL_16BIT | IIS_MODE_MONO | IIS_TXRXACT | IIS_ENABLE);	// 0->1 enable IIS

	if (setting &(1<<7))
		iis_trx_short = 0;//internal loopback
	else
		iis_trx_short = 1;//trx short loopback

	printk("iiscr = %X\n",0x80000000 |(setting & 0x7fe) | IIS_ENABLE | (IIS_DEBUG_MODE<<15));

	enable_CP3();
	change_cp3_con_control0(0xff, CP3_COUNT_STOP);
	write_32bit_cp3_general_register(CP3_MONCNT0LO, 0);
	write_32bit_cp3_general_register(CP3_MONCNT0HI, 0);


	unsigned int start_time;
	unsigned int pre_time;
	//volatile unsigned int get_timer_jiffies(void);
	start_time = get_timer_jiffies();
	iis_test_starttime = start_time;
	printk("star_time=%x\n",start_time);
#if 0
    rtlRegMask(0xb8000094, 0x00000FFF, 0x00000090);//case new iis
#endif
	change_cp3_con_control0(0xff, CP3_COUNT_CYCLE);
	rtl_outl(IISCR,0x80000000 | (setting & 0x7fe) | IIS_ENABLE | (IIS_DEBUG_MODE<<15) );	// 0->1 enable IIS

	unsigned int status_val_tx;
	unsigned int status_val_rx;


	status_val_tx = rtl_inl(IIS_TX_ISR);
	status_val_rx = rtl_inl(IIS_RX_ISR);

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
	cp3_read[0]=0;
re_start:
	cp3_read[i]=(int)cp3_counter0_get_64bit();
	tx_isr_arr[i]=rtl_inl(IIS_TX_ISR);
	rx_isr_arr[i]=rtl_inl(IIS_RX_ISR);
	if ((tx_isr_arr[i]!=tx_isr_arr[i-1]) || (rx_isr_arr[i]!=rx_isr_arr[i-1]))
		i++;

	if(get_timer_jiffies() < (start_time+200))
		goto re_start;


	//while(get_timer_jiffies() > (start_time+200) )
	{
	status_val_tx = rtl_inl(IIS_TX_ISR);
	status_val_rx = rtl_inl(IIS_RX_ISR);
	for (j=0;j<i; j++)
		printk(" [%X]trx_isr = %X,%X \n",cp3_read[j], tx_isr_arr[j], rx_isr_arr[j]);

		printk(" iis_txisr = %X \n", status_val_tx);
		printk(" iis_rxisr = %X \n", status_val_rx);
			int print_size;
			print_size = (PAGE_SIZE>90)? 90:PAGE_SIZE;
			printk("iis rx page: \n");
			for (i=0;i<(print_size*PAGE_NUM)*2+8;i++) {
				if ((i%8) == 7)
					printk(" %x\n",iis_rx.payload[i]);
				else
					printk(" %x ",iis_rx.payload[i]);
			}

	}
	iis_isr_test_flag = 0;
#endif
}


void iis_isr_test(int pindex);
void iis_isr_tx_test(int pindex);

void iis_ISR(uint32 iis_txisr, uint32 iis_rxisr)
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

			iis_tr_cnt++;
		} // end of tx

		if( iis_rxisr & IISChanRxPage[iis_rxpage] ) {
			iis_isr_test(iis_rxpage);
			iis_cpy_cnt ++;
			iis_set_rx_own_bit(iis_rxpage);
			//iis_set_tx_own_bit(iis_txpage);
			//need_PCM_RX[chid] = 1;
			iis_rxisr &= ~IISChanRxPage[iis_rxpage];
			iis_rxpage = (iis_rxpage+1) % PAGE_NUM;

			iis_tr_cnt--;
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
		twiddle();
		//printk("%d",iis_isr_cnt);
	}
#endif

}



void iis_interrupt()
{
	unsigned int status_val_tx;
	unsigned int status_val_rx;
	int i;

	status_val_tx = rtl_inl(IIS_TX_ISR);
	status_val_rx = rtl_inl(IIS_RX_ISR);
	if( status_val_tx || status_val_rx )
//	if( status_val_rx )
	{
		rtl_outl(IIS_TX_ISR, status_val_tx);
		rtl_outl(IIS_RX_ISR, status_val_rx);

		iis_ISR(status_val_tx & 0x0F, status_val_rx & 0x0F);

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
			printk("iis rx page: \n");
			for (i=0;i<(PAGE_SIZE*PAGE_NUM)*2+8;i++) {
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
#if 0
			for (i=0;i<PAGE_SIZE*2;i=i+32)
#else // full test
			for (i=0;i<PAGE_SIZE*2;i++)
#endif
			{
				short rxi = *(iis_rx.payload + PAGE_SIZE*2*pindex + i);
				short cmp;
				if (iis_trx_short)
					if (i==0 && pindex==0) {
						if (iis_isr_cnt == 0)
							cmp = 0;
						else
							cmp = iis_comp[PAGE_SIZE*2*4 -1];
					} else {
						cmp = iis_comp[PAGE_SIZE*2*pindex + i -1];
					}
				else
					cmp = iis_comp[PAGE_SIZE*2*pindex + i];

				if ( rxi != cmp) { //u-law fixed pattern
					//if(0){
					//printfByPolling("\n%d-%d-%d\n", iis_test_cnt , i, pindex);
					printfByPolling("rxi =%x\n", rxi);
					printfByPolling("cmp =%x\n", cmp);
					//printfByPolling("iis_rx.payload=%x\n", iis_rx.payload);
					//printfByPolling("iis_comp=%x\n", iis_comp);
					//print_iis_regs();
					//while (1)
							;
					if ( err_cnt_in == 0)
						err_cnt++;
					err_cnt_in++;
				}
			}

			err_sample += err_cnt_in; // the sum of error sample.
			//printf("\nj=%d (%d,%d)\n", j, err_cnt, err_cnt_in); // induces ERROR if the line above not exist!
			if (err_cnt_in)
				iis_breakpoint();
				//goto label1;
			if (err_cnt_in) {
				printfByPolling("\niis_test_cnt=%d (%d,%d)\n", iis_test_cnt, err_cnt, err_sample);
			} else {
				//twiddle();
				//printk("%d", chid+1);
			}
#endif
#if defined(IIS_CODEC_ALC5621)
			memcpy(&databuf[rx_write_index], iis_rx.payload + PAGE_SIZE*2*pindex,PAGE_SIZE*4);
			rx_write_index=(rx_write_index+PAGE_SIZE*4)%DATABUFSIZE;
#endif
#if 0
			if(iis_cpy_cnt < PAGE_NUM)
				memcpy(iis_dump + PAGE_SIZE*2*iis_test_cnt ,iis_rx.payload + PAGE_SIZE*2*pindex,PAGE_SIZE*4);
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
			if (temp_v > IIS_TEST_TIME_MIN*60*100) {
				rtl_outl(IIS_TX_IMR,0);
				rtl_outl(IIS_RX_IMR,0);
				rtl_outl(IISCR,0x0000);	// stop IIS

				//volatile unsigned int get_timer_jiffies(void);
				printk("time=%x\n",get_timer_jiffies());

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
				printk("iis rx page: \n");
				for (i=0;i<(PAGE_SIZE*PAGE_NUM)*2;i++) {
					if ((i%8) == 7)
						printk(" %x\n", iis_rx.payload[i]);
					else
						printk(" %x ", iis_rx.payload[i]);
				}
#endif
				iis_isr_test_flag = 0;
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
				for (i=0;i<(PAGE_SIZE*PAGE_NUM)*2;i++) {
					if ((i%8) == 7)
						printk(" %x\n",iis_dump[i]);
					else
						printk(" %x ",iis_dump[i]);
				}

				iis_isr_test_flag = 0;
			}


}

void iis_isr_tx_test(int pindex)
{
#if defined(IIS_PLAY_TONE)
	if (iis_24bit_flag)
		tone_gens_24bit(PAGE_SIZE, iis_tx.payload);
	else
		tone_gens( PAGE_SIZE*2, iis_tx.payload + PAGE_SIZE*2*pindex);
#endif
#if defined(IIS_MIC_LOOPBACK_SPEAKER)
	memcpy(iis_tx.payload + PAGE_SIZE*2*pindex, &databuf[tx_read_index], PAGE_SIZE*4);
	tx_read_index= (tx_read_index+PAGE_SIZE*4)%DATABUFSIZE;
		iis_test_cnt_tx++;
#endif
	int temp_v;
	temp_v = get_timer_jiffies();
	temp_v = temp_v - iis_test_starttime;
			//if (iis_test_cnt_tx >1000)
			//if (iis_test_cnt_tx >10000)
			if (temp_v > IIS_TEST_TIME_MIN*60*100){
				rtl_outl(IIS_TX_IMR,0);
				rtl_outl(IIS_RX_IMR,0);
				rtl_outl(IISCR,0x0000);	// stop IIS

				//volatile unsigned int get_timer_jiffies(void);
				printk("time=%x\n",get_timer_jiffies());

				print_iis_regs();
				printk("iis rx page: \n");
/*				for (i=0;i<(PAGE_SIZE*PAGE_NUM)*2;i++) {
					if ((i%8) == 7)
						printk(" %x\n",iis_dump[i]);
					else
						printk(" %x ",iis_dump[i]);
				}
*/
				iis_isr_test_flag = 0;
			}

}



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

int32_t tone_phase=0;

int32_t tone_phase_ad;

int32_t play_channel;
int32_t play_channel_now;

static int sample_count;
void tone_gens(int32_t sample_num, int16_t *buffPtr)
{
	int32_t i;

	tone_phase_ad = (1014 * 16777) >> 11;		// 65535/8000 in Q11

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
			*buffPtr=0;
			//printk("-");
		}
		sample_count++;
		if (sample_count==iis_allchannel)
			sample_count=0;
		buffPtr++;
	}
}



void tone_gens_24bit(int32_t sample_num, int32_t *buffPtr)
{
	int32_t i;

	tone_phase_ad = (1014 * 16777) >> 11;		// 65535/8000 in Q11

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
			*buffPtr=(int)(sinus(tone_phase))<<6;
			tone_phase += tone_phase_ad;
			if (tone_phase > 65535)
				tone_phase -= 65535;
		} else {
			*buffPtr=0;
			//printk("-");
		}
		sample_count++;
		if (sample_count==iis_allchannel)
			sample_count=0;
		buffPtr++;
	}
}
