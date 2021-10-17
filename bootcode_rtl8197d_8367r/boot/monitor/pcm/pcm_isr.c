//#include <rtl8650/asicregs.h>
#include <rtl_types.h>
#include <linux/autoconf.h>
#include <linux/interrupt.h>
#include "typedef.h"
#include "g711.h"
#include "conceal_con.h"


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

//#define PCM_SLIC_SI3215 1
//#define PCM_CODEC_ALC5621 1
#if defined(PCM_SLIC_SI3215) || defined(PCM_CODEC_ALC5621) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3217x)
//#define PCM_PLAY_TONE 1
#define PCM_MIC_LOOPBACK_SPEAKER 1
#endif

#define PCM_TEST_TIME_MIN 1


#define Virtual2Physical(x)             (((int)x) & 0x1fffffff)
#define Physical2Virtual(x)             (((int)x) | 0x80000000)
#define Virtual2NonCache(x)             (((int)x) | 0x20000000)
#define Physical2NonCache(x)            (((int)x) | 0xa0000000)

#define REG32(reg) (*(volatile unsigned long *)reg)
#define BIT(x)	( 1 << (x))
#define rtl_outl(address, value)	(REG32(address) = value)
#define rtl_inl(address)			REG32(address)

//define pcm register
#define PCM_BASE		0xB8008000
#define	PCMCR			(PCM_BASE+0x00)
#define PACHCNR03		(PCM_BASE+0x04)
#define PATSAR03		(PCM_BASE+0x08)
#define PABSIZE03		(PCM_BASE+0x0c)
#define	CH03ATXBSA(x)		(PCM_BASE+0x10+(x*4))
#define	CH0ATXBSA		(PCM_BASE+0x10)
#define	CH1ATXBSA		(PCM_BASE+0x14)
#define	CH2ATXBSA		(PCM_BASE+0x18)
#define	CH3ATXBSA		(PCM_BASE+0x1c)
#define	CH03ARXBSA(x)		(PCM_BASE+0x20+(x*4))
#define	CH0ARXBSA		(PCM_BASE+0x20)
#define	CH1ARXBSA		(PCM_BASE+0x24)
#define	CH2ARXBSA		(PCM_BASE+0x28)
#define	CH3ARXBSA		(PCM_BASE+0x2c)
#define	PAIMR03			(PCM_BASE+0x30)
#define	PAISR03			(PCM_BASE+0x34)
#define PACHCNR47		(PCM_BASE+0x38)
#define PATSAR47		(PCM_BASE+0x3c)
#define PABSIZE47		(PCM_BASE+0x40)
#define	CH47ATXBSA(x)		(PCM_BASE+0x44+((x%4)*4))
#define	CH4ATXBSA		(PCM_BASE+0x44)
#define	CH5ATXBSA		(PCM_BASE+0x48)
#define	CH6ATXBSA		(PCM_BASE+0x4c)
#define	CH7ATXBSA		(PCM_BASE+0x50)
#define	CH47ARXBSA(x)		(PCM_BASE+0x54+((x%4)*4))
#define	CH4ARXBSA		(PCM_BASE+0x54)
#define	CH5ARXBSA		(PCM_BASE+0x58)
#define	CH6ARXBSA		(PCM_BASE+0x5c)
#define	CH7ARXBSA		(PCM_BASE+0x60)
#define	PAIMR47			(PCM_BASE+0x64)
#define	PAISR47			(PCM_BASE+0x68)
//PCMCR
#define PCMMODE_LINEAR		BIT(13)		//compander or linear
#define	PCMENABLE		BIT(12)
#define PCMCLK			BIT(11)		//when pcm interface enable ,set this bit.
#define	FSINV			BIT(9)
//PACHCNR03
#define	CH0ILBE			BIT(28)		//channel 0 loopback
#define CH0uA			BIT(26)		//channel 0 u/A-law
#define CH0TE			BIT(25)		//channel 0 TX enable
#define	CH0RE			BIT(24)		//channel 0 RX enable

#define CH1uA			BIT(18)		//channel 1 u/A-law
#define CH1TE			BIT(17)		//channel 1 TX enable
#define	CH1RE			BIT(16)		//channel 1 RX enable

#define CH2uA			BIT(10)		//channel 2 u/A-law
#define CH2TE			BIT(9)		//channel 2 TX enable
#define	CH2RE			BIT(8)		//channel 2 RX enable

#define CH3uA			BIT(2)		//channel 3 u/A-law
#define CH3TE			BIT(1)		//channel 3 TX enable
#define	CH3RE			BIT(0)		//channel 3 RX enable
//PACHCNR47
#define CH4uA			BIT(26)		//channel 4 u/A-law
#define CH4TE			BIT(25)		//channel 4 TX enable
#define	CH4RE			BIT(24)		//channel 4 RX enable

#define CH5uA			BIT(18)		//channel 5 u/A-law
#define CH5TE			BIT(17)		//channel 5 TX enable
#define	CH5RE			BIT(16)		//channel 5 RX enable

#define CH6uA			BIT(10)		//channel 6 u/A-law
#define CH6TE			BIT(9)		//channel 6 TX enable
#define	CH6RE			BIT(8)		//channel 6 RX enable

#define CH7uA			BIT(2)		//channel 7 u/A-law
#define CH7TE			BIT(1)		//channel 7 TX enable
#define	CH7RE			BIT(0)		//channel 7 RX enable


volatile int get_timer_jiffies(void);

int pcm_test_starttime;

#define CH_NUM		8
#define	PAGE_NUM	2
//#define PKTSIZE		39 // 10 ms
#define PKTSIZE		79
//#define PKTSIZE		159
//#define PKTSIZE		255

//#define LINEAR_MODE
//#define PCM_DUMP


#ifndef LINEAR_MODE
//#define ALAW_MODE
#endif

//#define PCMPAGE_SIZE	1024
#define PCMPAGE_SIZE		((PKTSIZE+1)*4)
#define ISR_MASK(channel)	(0xFF000000 >> (8*(channel%4)))
#define TOK_MASK(channel)	(0xC0000000 >> (8*(channel%4)))
#define ROK_MASK(channel)	(0x30000000 >> (8*(channel%4)))
#define TBU_MASK(channel)	( 0xC000000 >> (8*(channel%4)))
#define RBU_MASK(channel)	( 0x3000000 >> (8*(channel%4)))

#define PCM_DATABUFSIZE		(200*PCMPAGE_SIZE)

unsigned char pcm_databuf[PCM_DATABUFSIZE];
uint32_t pcm_tx_read_index, pcm_rx_write_index;

#define printfByPolling prom_printf
#define printf prom_printf


struct pcm_pkt {
	unsigned char 	tx_rx;
	//unsigned char 	*payload;
	short 	*payload;
	short 	buf[PCMPAGE_SIZE/2]; // div 2 because of short type!
};
static struct pcm_pkt tx[CH_NUM];
static struct pcm_pkt rx[CH_NUM];

volatile int pcm_isr_test_flag;

#if 1 //for soft_codec testing by g711. chiminer
	short original = 0x1256, expected_data;
	short comp[CH_NUM][PCMPAGE_SIZE];
	static int break_point = 0;
	static unsigned int test_cnt[CH_NUM];
#endif

static  Word16 seg_end[8] = {0xFF, 0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF};

static int search(int val, short *table, int size)
{
    int     i;

    for (i = 0; i < size; i++) {
        if (val <= *table++)
            return (i);
    }
    return (size);
}

/*
 * linear2alaw() - Convert a 16-bit linear PCM value to 8-bit A-law
 *
 * linear2alaw() accepts an 16-bit integer and encodes it as A-law data.
 *
 *      Linear Input Code   Compressed Code
 *  ------------------------    ---------------
 *  0000000wxyza            000wxyz
 *  0000001wxyza            001wxyz
 *  000001wxyzab            010wxyz
 *  00001wxyzabc            011wxyz
 *  0001wxyzabcd            100wxyz
 *  001wxyzabcde            101wxyz
 *  01wxyzabcdef            110wxyz
 *  1wxyzabcdefg            111wxyz
 */
unsigned char linear2alaw(int pcm_val)        /* 2's complement (16-bit range) */
{
    int     mask;
    int     seg;
    unsigned char   aval;
/*ic a-law change*/
#define IC_ALAW
#ifdef IC_ALAW
    //pcm_val = (pcm_val +7) & 0xfffffff0;

    if (pcm_val >= 0) {
        mask = 0xD5;        /* sign (7th) bit = 1 */
    } else {
        mask = 0x55;        /* sign bit = 0 */
	pcm_val = (pcm_val +7);
        pcm_val = ~pcm_val;
    }
#else
    if (pcm_val >= 0) {
        mask = 0xD5;        /* sign (7th) bit = 1 */
    } else {
        mask = 0x55;        /* sign bit = 0 */
        pcm_val = ~pcm_val;
    }
#endif
    /* Convert the scaled magnitude to segment number. */
    seg = search(pcm_val, seg_end, 8);

    /* Combine the sign, segment, and quantization bits. */

    if (seg >= 8)       /* out of range, return maximum value. */
        return (0x7F ^ mask);
    else {
        aval = seg << SEG_SHIFT;
        if (seg < 2)
            aval |= (pcm_val >> 4) & QUANT_MASK;
        else
            aval |= (pcm_val >> (seg + 3)) & QUANT_MASK;
        return (aval ^ mask);
    }
}

/*
 * alaw2linear() - Convert an A-law value to 16-bit linear PCM
 *
 */
int alaw2linear(unsigned char a_val)
{
    int     t;
    int     seg;

    a_val ^= 0x55;

    t = (a_val & QUANT_MASK) << 4;
    seg = ((unsigned)a_val & SEG_MASK) >> SEG_SHIFT;
    switch (seg) {
    case 0:
        t += 8;
        break;
    case 1:
        t += 0x108;
        break;
    default:
        t += 0x108;
        t <<= seg - 1;
    }
    return ((a_val & SIGN_BIT) ? t : -t);
}

#define BIAS        (0x84)      /* Bias for linear code. */

/*
 * linear2ulaw() - Convert a linear PCM value to u-law
 *
 * In order to simplify the encoding process, the original linear magnitude
 * is biased by adding 33 which shifts the encoding range from (0 - 8158) to
 * (33 - 8191). The result can be seen in the following encoding table:
 *
 *  Biased Linear Input Code    Compressed Code
 *  ------------------------    ---------------
 *  00000001wxyza           000wxyz
 *  0000001wxyzab           001wxyz
 *  000001wxyzabc           010wxyz
 *  00001wxyzabcd           011wxyz
 *  0001wxyzabcde           100wxyz
 *  001wxyzabcdef           101wxyz
 *  01wxyzabcdefg           110wxyz
 *  1wxyzabcdefgh           111wxyz
 *
 * Each biased linear code has a leading 1 which identifies the segment
 * number. The value of the segment number is equal to 7 minus the number
 * of leading 0's. The quantization interval is directly available as the
 * four bits wxyz.  * The trailing bits (a - h) are ignored.
 *
 * Ordinarily the complement of the resulting code word is used for
 * transmission, and so the code word is complemented before it is returned.
 *
 */
unsigned char linear2ulaw(int pcm_val)    /* 2's complement (16-bit range) */
{
    int     mask;
    int     seg;
    unsigned char   uval;

    /* Get the sign and the magnitude of the value. */
    if (pcm_val < 0) {
        pcm_val = BIAS - pcm_val;
        mask = 0x7F;
    } else {
        pcm_val += BIAS;
        mask = 0xFF;
    }

    /* Convert the scaled magnitude to segment number. */
    seg = search(pcm_val, seg_end, 8);

    /*
     * Combine the sign, segment, quantization bits;
     * and complement the code word.
     */
    if (seg >= 8)       /* out of range, return maximum value. */
        return (0x7F ^ mask);
    else {
        uval = (seg << 4) | ((pcm_val >> (seg + 3)) & 0xF);
        return (uval ^ mask);
    }
}

/*
 * ulaw2linear() - Convert a u-law value to 16-bit linear PCM
 *
 * First, a biased linear code is derived from the code word. An unbiased
 * output can then be obtained by subtracting 33 from the biased code.
 *
 * Note that this function expects to be passed the complement of the
 * original code word. This is in keeping with ISDN conventions.
 */
int ulaw2linear(unsigned char u_val)
{
    int     t;

    /* Complement to obtain normal u-law value. */
    u_val = ~u_val;

    /*
     * Extract and bias the quantization bits. Then
     * shift up by the segment number and subtract out the bias.
     */
    t = ((u_val & QUANT_MASK) << 3) + BIAS;
    t <<= ((unsigned)u_val & SEG_MASK) >> SEG_SHIFT;

    return ((u_val & SIGN_BIT) ? (BIAS - t) : (t - BIAS));
}

void set_page_data(int page, short data)
{
	int i;
	int mid;
	mid=linear2ulaw(data);
	expected_data=ulaw2linear(mid);

	for (i=0;i<PCMPAGE_SIZE/2;i++)
		*(tx[0].payload + PCMPAGE_SIZE/2*page + i)=data;

}

#define printk printfByPolling



#define CH7P1RBU	BIT(0)
#define CH7P0RBU	BIT(1)
#define CH7P1TBU	BIT(2)
#define CH7P0TBU	BIT(3)
#define CH7P1ROK	BIT(4)
#define CH7P0ROK	BIT(5)
#define CH7P1TOK	BIT(6)
#define CH7P0TOK	BIT(7)

#define CH6P1RBU	BIT(8)
#define CH6P0RBU	BIT(9)
#define CH6P1TBU	BIT(10)
#define CH6P0TBU	BIT(11)
#define CH6P1ROK	BIT(12)
#define CH6P0ROK	BIT(13)
#define CH6P1TOK	BIT(14)
#define CH6P0TOK	BIT(15)

#define CH5P1RBU	BIT(16)
#define CH5P0RBU	BIT(17)
#define CH5P1TBU	BIT(18)
#define CH5P0TBU	BIT(19)
#define CH5P1ROK	BIT(20)
#define CH5P0ROK	BIT(21)
#define CH5P1TOK	BIT(22)
#define CH5P0TOK	BIT(23)

#define CH4P1RBU	BIT(24)
#define CH4P0RBU	BIT(25)
#define CH4P1TBU	BIT(26)
#define CH4P0TBU	BIT(27)
#define CH4P1ROK	BIT(28)
#define CH4P0ROK	BIT(29)
#define CH4P1TOK	BIT(30)
#define CH4P0TOK	BIT(31)

#define CH3P1RBU	BIT(0)
#define CH3P0RBU	BIT(1)
#define CH3P1TBU	BIT(2)
#define CH3P0TBU	BIT(3)
#define CH3P1ROK	BIT(4)
#define CH3P0ROK	BIT(5)
#define CH3P1TOK	BIT(6)
#define CH3P0TOK	BIT(7)

#define CH2P1RBU	BIT(8)
#define CH2P0RBU	BIT(9)
#define CH2P1TBU	BIT(10)
#define CH2P0TBU	BIT(11)
#define CH2P1ROK	BIT(12)
#define CH2P0ROK	BIT(13)
#define CH2P1TOK	BIT(14)
#define CH2P0TOK	BIT(15)

#define CH1P1RBU	BIT(16)
#define CH1P0RBU	BIT(17)
#define CH1P1TBU	BIT(18)
#define CH1P0TBU	BIT(19)
#define CH1P1ROK	BIT(20)
#define CH1P0ROK	BIT(21)
#define CH1P1TOK	BIT(22)
#define CH1P0TOK	BIT(23)

#define CH0P1RBU	BIT(24)
#define CH0P0RBU	BIT(25)
#define CH0P1TBU	BIT(26)
#define CH0P0TBU	BIT(27)
#define CH0P1ROK	BIT(28)
#define CH0P0ROK	BIT(29)
#define CH0P1TOK	BIT(30)
#define CH0P0TOK	BIT(31)

int txpage[CH_NUM];
int rxpage[CH_NUM];
int tr_cnt[CH_NUM];
#if CH_NUM == 8
static unsigned long ChanTxPage[16] = {CH0P0TOK, CH0P1TOK, CH1P0TOK, CH1P1TOK, CH2P0TOK, CH2P1TOK, CH3P0TOK, CH3P1TOK, CH4P0TOK, CH4P1TOK, CH5P0TOK, CH5P1TOK, CH6P0TOK, CH6P1TOK, CH7P0TOK, CH7P1TOK};
static unsigned long ChanRxPage[16] = {CH0P0ROK, CH0P1ROK, CH1P0ROK, CH1P1ROK, CH2P0ROK, CH2P1ROK, CH3P0ROK, CH3P1ROK, CH4P0ROK, CH4P1ROK, CH5P0ROK, CH5P1ROK, CH6P0ROK, CH6P1ROK, CH7P0ROK, CH7P1ROK};
#else
static unsigned long ChanTxPage[8] = {CH0P0TOK, CH0P1TOK, CH1P0TOK, CH1P1TOK, CH2P0TOK, CH2P1TOK, CH3P0TOK, CH3P1TOK};
static unsigned long ChanRxPage[8] = {CH0P0ROK, CH0P1ROK, CH1P0ROK, CH1P1ROK, CH2P0ROK, CH2P1ROK, CH3P0ROK, CH3P1ROK};
#endif

void tone_gens(int32_t sample_num, int16_t *buffPtr);
unsigned short pcm_tx_buf[CH_NUM][PCMPAGE_SIZE+2]__attribute__((aligned(16)));
unsigned short pcm_rx_buf[CH_NUM][PCMPAGE_SIZE+2]__attribute__((aligned(16)));
//unsigned short pcm_tx_buf[CH_NUM][384]__attribute__((aligned(4096)));
//unsigned short pcm_rx_buf[CH_NUM][384]__attribute__((aligned(4096)));
#ifdef PCM_DUMP
short tx_dump[2][65800];
short rx_dump[2][65800];
short tx_index[CH_NUM],rx_index[CH_NUM];
unsigned long tx_i[CH_NUM],rx_i[CH_NUM];
short alaw_mid[65800];
short ulaw_mid[65800];
#endif
// mode[8] : 0-> linear, 1-> enable compand
// mode[7:0]: bit0 0-> ch0 a law, 1-> ch0 u-law
void init_pcm_ch(int chid, unsigned int mode)
{
	int i;
#if 1 // Dump!!
	tx[chid].payload = (unsigned short *) Virtual2NonCache(&pcm_tx_buf[chid][2]);
	rx[chid].payload = (unsigned short *) Virtual2NonCache(&pcm_rx_buf[chid][2]);
#else
	//tx[chid].payload = (unsigned short *) (0xa0c00000 + chid * 0x10000);
	//rx[chid].payload = (unsigned short *) (0xa0d00000 + chid * 0x10000);
	tx[chid].payload = (unsigned short *) (0xa0300000 + chid * 0x10000);
	rx[chid].payload = (unsigned short *) (0xa0308000 + chid * 0x10000);
#endif


#if 1 //for g711 testing. chiminer
		//printf("comp_expand: \n");
		for (i=0;i<(PKTSIZE+1)*4;i++) {
			short raw_pcm = ((chid*100)+i+101)*30-67;//(i+9)*7-19;
			if (i%2 == 1)
				raw_pcm |= 0x8000;
			short back;
			unsigned char mid;

			tx[chid].payload[i] = raw_pcm;
/*#ifdef LINEAR_MODE
			comp[chid][i]=raw_pcm;
#else // !LINEAR_MODE
#ifdef ALAW_MODE
			mid=linear2alaw(raw_pcm);
			back=alaw2linear(mid);
#else
			mid=linear2ulaw(raw_pcm);
			back=ulaw2linear(mid);
#endif
			comp[chid][i]=back;
#endif // LINEAR_MODE	*/

			if((mode&0x100)==0)
				//comp[chid][i]=raw_pcm;
				back=raw_pcm;
			else if((mode&BIT(chid))==0){
				mid=linear2alaw(raw_pcm);
				back=alaw2linear(mid);
			}else{
				mid=linear2ulaw(raw_pcm);
				back=ulaw2linear(mid);
			}
			comp[chid][i]=back;


		}
		if(chid == 0){
#ifdef PCM_PLAY_TONE
			tone_gens((PKTSIZE+1)*2*2,  tx[chid].payload);
#endif
#ifdef PCM_MIC_LOOPBACK_SPEAKER
			memset(tx[chid].payload, 0, 2*PCMPAGE_SIZE);
#endif
		}

#if 1
		printk("\nchid = %d\n", chid);
		printk("tx page: \n");
		for (i=0;i<(PKTSIZE+1)*4;i++) {
			if ((i%8) == 7)
				printk(" %x\n",tx[chid].payload[i]);
			else
				printk(" %x ",tx[chid].payload[i]);
		}

		printk("comp:\n");
		for (i=0;i<(PKTSIZE+1)*4;i++) {
			if ((i%8) == 7)
				printk(" %x\n",comp[chid][i]);
			else
				printk(" %x ",comp[chid][i]);

		}

#endif

#endif

	if (chid < 4) {
		// allocate buffer address
		rtl_outl(CH03ATXBSA(chid),(unsigned int)tx[chid].payload & 0xffffff);
		rtl_outl(CH03ARXBSA(chid),(unsigned int)rx[chid].payload & 0xffffff);
		// set RX owned by PCM controller
		rtl_outl(CH03ARXBSA(chid), rtl_inl(CH03ARXBSA(chid)) | 0x3);
		rtl_outl(CH03ATXBSA(chid), rtl_inl(CH03ATXBSA(chid)) | 0x3);
		printf("enable PCM Channel-%d interrupt\n", chid);
		rtlRegMask(PAISR03, 0xFF000000 >> (chid*8), rtlRegRead(PAISR03));/* reset pcm isr */
		rtl_outl(PAIMR03,rtl_inl(PAIMR03) |0x30000000 >> (chid*8));
		rtl_outl(PABSIZE03,rtl_inl(PABSIZE03) | PKTSIZE << (24 - (chid*8)));	//set page size
	} else { // 4-7
		// allocate buffer address
		rtl_outl(CH47ATXBSA(chid),(unsigned int)tx[chid].payload & 0xffffff);
		rtl_outl(CH47ARXBSA(chid),(unsigned int)rx[chid].payload & 0xffffff);
		// set RX owned by PCM controller
		rtl_outl(CH47ARXBSA(chid), rtl_inl(CH47ARXBSA(chid)) | 0x3);
		rtl_outl(CH47ATXBSA(chid), rtl_inl(CH47ATXBSA(chid)) | 0x3);
		printf("enable PCM Channel-%d interrupt\n", chid);
		rtlRegMask(PAISR47, 0xFF000000 >> ((chid&3)*8), rtlRegRead(PAISR47));/* reset pcm isr */
		rtl_outl(PAIMR47,rtl_inl(PAIMR47) |0x30000000 >> ((chid%4)*8));
		rtl_outl(PABSIZE47,rtl_inl(PABSIZE47) | PKTSIZE << (24 - ((chid%4)*8)));	//set page size
	}

	txpage[chid] = 0;
	rxpage[chid] = 0;
	tr_cnt[chid] = 0;
	test_cnt[chid] = 0;

}
#ifdef PCM_DUMP
// mode[8] : 0-> linear, 1-> enable compand
// mode[7:0]: bit0 0-> ch0 a law, 1-> ch0 u-law
void init_pcm_dump_ch(int chid, unsigned int mode)
{
	int i;
#if 0 // Dump!!
	tx[chid].payload = (unsigned short *) Virtual2NonCache(pcm_tx_buf);
	rx[chid].payload = (unsigned short *) Virtual2NonCache(pcm_rx_buf);
#else
	//tx[chid].payload = (unsigned short *) (0xa0c00000 + chid * 0x10000);
	//rx[chid].payload = (unsigned short *) (0xa0d00000 + chid * 0x10000);
	tx[chid].payload = (unsigned short *) (0xa0300000 + chid * 0x10000);
	rx[chid].payload = (unsigned short *) (0xa0308000 + chid * 0x10000);
#endif


#if 1 //for g711 testing. chiminer
		//printf("comp_expand: \n");
		//tx_index[chid]=-32768;
		tx_index[chid]=-31745;
		tx_i[chid]=0;
		rx_i[chid]=0;
		for (i=0;i<(PKTSIZE+1)*4;i++) {
			short raw_pcm = ((chid*100)+i+101)*30-67;//(i+9)*7-19;
			if (i%2 == 1)
				raw_pcm |= 0x8000;
			short back;
			unsigned char mid;
			if(chid < 2)
				tx_dump[chid][ tx_i[chid]++ ]=tx_index[chid];
			//tx[chid].payload[i] = tx_index[chid]++;
			tx[chid].payload[i] = tx_index[chid];
/*#ifdef LINEAR_MODE
			comp[chid][i]=raw_pcm;
#else // !LINEAR_MODE
#ifdef ALAW_MODE
			mid=linear2alaw(raw_pcm);
			back=alaw2linear(mid);
#else
			mid=linear2ulaw(raw_pcm);
			back=ulaw2linear(mid);
#endif
			comp[chid][i]=back;
#endif // LINEAR_MODE	*/

			if((mode&0x100)==0)
				//comp[chid][i]=raw_pcm;
				back=raw_pcm;
			else if((mode&BIT(chid))==0){
				mid=linear2alaw(raw_pcm);
				back=alaw2linear(mid);
			}else{
				mid=linear2ulaw(raw_pcm);
				back=ulaw2linear(mid);
			}
			comp[chid][i]=back;


		}
		short temp;
		temp=-32768;
		for(i=0;i<65536;i++){
			unsigned char mid;
			mid=linear2alaw(temp);
			alaw_mid[i]=alaw2linear(mid);
			mid=linear2ulaw(temp++);
			ulaw_mid[i]=ulaw2linear(mid);
		}


#if 1
		printk("\nchid = %d\n", chid);
		printk("tx page: \n");
/*		for (i=0;i<(PKTSIZE+1)*4;i++) {
			if ((i%8) == 7)
				printk(" %x\n",tx[chid].payload[i]);
			else
				printk(" %x ",tx[chid].payload[i]);
		}

		printk("comp:\n");
		for (i=0;i<(PKTSIZE+1)*4;i++) {
			if ((i%8) == 7)
				printk(" %x\n",comp[chid][i]);
			else
				printk(" %x ",comp[chid][i]);

		}*/
		if(chid ==0){
			printk("\nalaw:\n");
			for(i=0;i<65536;i++){
				if ((i%8) == 7)
					printk(" %x\n",alaw_mid[i]);
				else
					printk(" %x ",alaw_mid[i]);
			}

			printk("\nulaw:\n");
			for(i=0;i<65536;i++){
				if ((i%8) == 7)
					printk(" %x\n",ulaw_mid[i]);
				else
					printk(" %x ",ulaw_mid[i]);
			}
		}
#endif

#endif

	if (chid < 4) {
		// allocate buffer address
		rtl_outl(CH03ATXBSA(chid),(unsigned int)tx[chid].payload & 0xffffff);
		rtl_outl(CH03ARXBSA(chid),(unsigned int)rx[chid].payload & 0xffffff);
		// set RX owned by PCM controller
		rtl_outl(CH03ARXBSA(chid), rtl_inl(CH03ARXBSA(chid)) | 0x3);
		rtl_outl(CH03ATXBSA(chid), rtl_inl(CH03ATXBSA(chid)) | 0x3);
		printf("enable PCM Channel-%d interrupt\n", chid);
		rtl_outl(PAIMR03,rtl_inl(PAIMR03) |0x30000000 >> (chid*8));
		rtl_outl(PABSIZE03,rtl_inl(PABSIZE03) | PKTSIZE << (24 - (chid*8)));	//set page size
	} else { // 4-7
		// allocate buffer address
		rtl_outl(CH47ATXBSA(chid),(unsigned int)tx[chid].payload & 0xffffff);
		rtl_outl(CH47ARXBSA(chid),(unsigned int)rx[chid].payload & 0xffffff);
		// set RX owned by PCM controller
		rtl_outl(CH47ARXBSA(chid), rtl_inl(CH47ARXBSA(chid)) | 0x3);
		rtl_outl(CH47ATXBSA(chid), rtl_inl(CH47ATXBSA(chid)) | 0x3);
		printf("enable PCM Channel-%d interrupt\n", chid);
		rtl_outl(PAIMR47,rtl_inl(PAIMR47) |0x30000000 >> ((chid%4)*8));
		rtl_outl(PABSIZE47,rtl_inl(PABSIZE47) | PKTSIZE << (24 - ((chid%4)*8)));	//set page size
	}

	txpage[chid] = 0;
	rxpage[chid] = 0;
	tr_cnt[chid] = 0;
	test_cnt[chid] = 0;

}
#endif

void print_pcm_regs(void)
{
	printk("PCMCR= 0x%x\n", rtl_inl(PCMCR));
	printk("PACHCNR03= 0x%x\n", rtl_inl(PACHCNR03));
	printk("PACHCNR47= 0x%x\n", rtl_inl(PACHCNR47));
	printk("PATSAR03= 0x%x\n", rtl_inl(PATSAR03));
	printk("PATSAR47= 0x%x\n", rtl_inl(PATSAR47));
	printk("PABSIZE03= 0x%x\n", rtl_inl(PABSIZE03));
	printk("PABSIZE47= 0x%x\n", rtl_inl(PABSIZE47));
	printk("CH0ATXBSA= 0x%x\n", rtl_inl(CH0ATXBSA));
	printk("CH1ATXBSA= 0x%x\n", rtl_inl(CH1ATXBSA));
	printk("CH2ATXBSA= 0x%x\n", rtl_inl(CH2ATXBSA));
	printk("CH3ATXBSA= 0x%x\n", rtl_inl(CH3ATXBSA));
	printk("CH4ATXBSA= 0x%x\n", rtl_inl(CH4ATXBSA));
	printk("CH5ATXBSA= 0x%x\n", rtl_inl(CH5ATXBSA));
	printk("CH6ATXBSA= 0x%x\n", rtl_inl(CH6ATXBSA));
	printk("CH7ATXBSA= 0x%x\n", rtl_inl(CH7ATXBSA));
	printk("CH0ARXBSA= 0x%x\n", rtl_inl(CH0ARXBSA));
	printk("CH1ARXBSA= 0x%x\n", rtl_inl(CH1ARXBSA));
	printk("CH2ARXBSA= 0x%x\n", rtl_inl(CH2ARXBSA));
	printk("CH3ARXBSA= 0x%x\n", rtl_inl(CH3ARXBSA));
	printk("CH4ARXBSA= 0x%x\n", rtl_inl(CH4ARXBSA));
	printk("CH5ARXBSA= 0x%x\n", rtl_inl(CH5ARXBSA));
	printk("CH6ARXBSA= 0x%x\n", rtl_inl(CH6ARXBSA));
	printk("CH7ARXBSA= 0x%x\n", rtl_inl(CH7ARXBSA));
	printk("PAIMR03= 0x%x\n", rtl_inl(PAIMR03));
	printk("PAIMR47= 0x%x\n", rtl_inl(PAIMR47));
	printk("PAISR03= 0x%x\n", rtl_inl(PAISR03));
	printk("PAISR47= 0x%x\n", rtl_inl(PAISR47));
}

void pcm_interrupt();

#if defined(CONFIG_RTL8198) || defined(CONFIG_RTL8196B)
struct irqaction irq19 = {pcm_interrupt, NULL, 19,
             "pcm", NULL, NULL};
#endif
#if defined(CONFIG_RTL8196C)
struct irqaction irq17 = {pcm_interrupt, NULL, 17,
             "pcm", NULL, NULL};
#endif

#ifdef PCM_DUMP
void pcm_dump_interrupt();
struct irqaction irq19_dump = {pcm_dump_interrupt, NULL, 19,
             "pcm_dump", NULL, NULL};
#endif
// mode[8] : 0-> linear, 1-> enable compand
// mode[7:0]: bit0 0-> ch0 a law, 1-> ch0 u-law
// mode[9]: 0-> internal loopback, 1-> normal tx,rx
void init_pcm(unsigned int mode)
{
	int i;

/*#ifdef LINEAR_MODE
	rtl_outl(PCMCR,PCMMODE_LINEAR | PCMENABLE | PCMCLK);	// 0->1 enable PCM
	rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
	rtl_outl(PCMCR,PCMMODE_LINEAR | PCMENABLE | PCMCLK);	// 0->1 enable PCM
#else
	rtl_outl(PCMCR,PCMENABLE | PCMCLK);	// 0->1 enable PCM
	rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
	rtl_outl(PCMCR,PCMENABLE | PCMCLK);	// 0->1 enable PCM
#endif	*/

	if ((mode&0x100)==0 ){
#if defined(PCM_CODEC_ALC5621)
		ALC5621linear();
#endif
#if defined(PCM_SLIC_SI3215)
		si3215_linear();
#endif
#if defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3217x)
		prom_printf("(%s:%d)", __FUNCTION__, __LINE__);
		si3217x_linear();
#endif
		//rtl_outl(PCMCR,PCMMODE_LINEAR | PCMENABLE | PCMCLK);	// 0->1 enable PCM
		//rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
		rtl_outl(PCMCR,PCMMODE_LINEAR | PCMENABLE | PCMCLK);	// 0->1 enable PCM
	}else{
		//rtl_outl(PCMCR,PCMENABLE | PCMCLK);	// 0->1 enable PCM
		//rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
		rtl_outl(PCMCR,PCMENABLE | PCMCLK);	// 0->1 enable PCM
#if defined(PCM_CODEC_ALC5621)
		if (mode & 1)
			ALC5621ulaw();
		else
			ALC5621alaw();
#endif
#if defined(PCM_SLIC_SI3215)
		if (mode & 1)
			si3215_mulaw();
		else
			si3215_alaw();
#endif
#if defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3217x)
		if (mode & 1)
			si3217x_mulaw();
		else
			si3217x_alaw();
#endif
	}


#if 1
	/* Install interrupt handler */
	//int_Register(7, (1 << 19), 0, pcm_interrupt);		init interrupt in 8672

#if defined(CONFIG_RTL8198) || defined(CONFIG_RTL8196B)
	request_IRQ(19, &irq19,NULL);
#endif
#if defined(CONFIG_RTL8196C)
	request_IRQ(17, &irq17,NULL);
#endif

	/* Enable timer interrupt */
	//if ( 7 < getIlev() )
	//d	setIlev(7);
#endif

	memset(pcm_databuf, 0, PCM_DATABUFSIZE);
	pcm_tx_read_index=PCMPAGE_SIZE;
	pcm_rx_write_index=0;



	// Enable PCM Channel
	for (i=0; i < CH_NUM; i++)
		init_pcm_ch(i, mode);

	for (i = 0; i < CH_NUM; i++) {
		printk("tx[%d].payload = %X\n", i, tx[i].payload);
		printk("rx[%d].payload = %X\n", i, rx[i].payload);
		printk("comp[%d] = %X\n", i, comp[i]);
	}

	//set time slot
	//rtl_outl(PATSAR03,0x1000000);
	//rtl_outl(PATSAR03,0x0103090b);
	rtl_outl(PATSAR03,0x00020406);
#if CH_NUM == 8
	//rtl_outl(PATSAR47,0x05070d0f);
	rtl_outl(PATSAR47,0x080a0c0e);
#endif

	print_pcm_regs();
#if 0 // debug
	while(1) ;
#endif

#if CH_NUM == 1
#ifdef ALAW_MODE
	rtl_outl(PACHCNR03, CH0ILBE | CH0uA| CH0RE | CH0TE);
#else
	rtl_outl(PACHCNR03, CH0RE | CH0TE);
#endif
#elif 	CH_NUM == 2

#ifdef ALAW_MODE

#if 0
	rtl_outl(PACHCNR03, CH1uA| CH1RE | CH1TE
				);
#else
	rtl_outl(PACHCNR03, CH0uA| CH0RE | CH0TE
				| CH1uA| CH1RE | CH1TE
				);
#endif

#else
	rtl_outl(PACHCNR03, CH0RE | CH0TE
				| CH1RE | CH1TE
				);
#endif

#elif 	CH_NUM == 4

#ifdef ALAW_MODE
	rtl_outl(PACHCNR03, CH0uA| CH0RE | CH0TE
				| CH1uA| CH1RE | CH1TE
				| CH2uA| CH2RE | CH2TE
				| CH3uA| CH3RE | CH3TE
				);
#else
	rtl_outl(PACHCNR03, CH0RE | CH0TE
				| CH1RE | CH1TE
				| CH2RE | CH2TE
				| CH3RE | CH3TE
				);

#endif

#elif 	CH_NUM == 8

	unsigned int uamode03,uamode47;
	uamode03 =0;
	uamode47 = 0;

	if((mode&1)==0)
		uamode03=CH0uA;
	if((mode&2)==0)
		uamode03|=CH1uA;
	if((mode&4)==0)
		uamode03|=CH2uA;
	if((mode&8)==0)
		uamode03|=CH3uA;
	if((mode&0x10)==0)
		uamode47=CH4uA;
	if((mode&0x20)==0)
		uamode47|=CH5uA;
	if((mode&0x40)==0)
		uamode47|=CH6uA;
	if((mode&0x80)==0)
		uamode47|=CH7uA;
	if((mode&0x200)==0)
		uamode03|=CH0ILBE;

	pcm_test_starttime = get_timer_jiffies();

	printk("pachcnr03 = %X\npachcnr47 = %X\n", uamode03  |  CH0RE | CH0TE
				| CH1RE | CH1TE
				| CH2RE | CH2TE
				| CH3RE | CH3TE, uamode47 | CH4RE | CH4TE
				|  CH5RE | CH5TE
				|  CH6RE | CH6TE
				|  CH7RE | CH7TE);

	rtl_outl(PACHCNR03,uamode03  |  CH0RE | CH0TE
				| CH1RE | CH1TE
				| CH2RE | CH2TE
				| CH3RE | CH3TE
				);
	rtl_outl(PACHCNR47,uamode47 | CH4RE | CH4TE
				|  CH5RE | CH5TE
				|  CH6RE | CH6TE
				|  CH7RE | CH7TE
				);

/*
#ifdef ALAW_MODE
#if 1
	rtl_outl(PACHCNR03, CH0ILBE | CH0uA| CH0RE | CH0TE
	//rtl_outl(PACHCNR03, CH0uA| CH0RE | CH0TE
				| CH1uA| CH1RE | CH1TE
				| CH2uA| CH2RE | CH2TE
				| CH3uA| CH3RE | CH3TE
				);
#endif
	rtl_outl(PACHCNR47, CH4uA| CH4RE | CH4TE
				| CH5uA| CH5RE | CH5TE
				| CH6uA| CH6RE | CH6TE
				| CH7uA| CH7RE | CH7TE
				);
#else
	rtl_outl(PACHCNR03, CH0ILBE |  CH0RE | CH0TE
	//rtl_outl(PACHCNR03,  CH0RE | CH0TE
				| CH1RE | CH1TE
				| CH2RE | CH2TE
				| CH3RE | CH3TE
				);
	rtl_outl(PACHCNR47, CH4RE | CH4TE
				|  CH5RE | CH5TE
				|  CH6RE | CH6TE
				|  CH7RE | CH7TE
				);
#endif	*/
#else
xxxxxx
#endif

	//rtl_outl(PACHCNR03, CH0RE | CH0TE); 		// MU-LAW
	//rtl_outl(PACHCNR03, CH0RE | CH0TE | CH0ILBE); 		// MU-LAW + internal loopback
	//rtl_outl(PACHCNR03,CH0RE | CH0TE | CH0ILBE); rtl_outl(PCMCR,rtl_inl(PCMCR)|PCMMODE);	// Linear Mode + internal loopback
	//rtl_outl(PACHCNR03,CH0RE | CH0TE); rtl_outl(PCMCR,rtl_inl(PCMCR)|PCMMODE);	// Linear Mode


	//printf("enable PCM....OK\n");
	//ALC5621_init();
	//proslic_initialize(0);
}
#ifdef PCM_DUMP
void init_pcm_dump(unsigned int mode)
{
	int i;

/*#ifdef LINEAR_MODE
	rtl_outl(PCMCR,PCMMODE_LINEAR | PCMENABLE | PCMCLK);	// 0->1 enable PCM
	rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
	rtl_outl(PCMCR,PCMMODE_LINEAR | PCMENABLE | PCMCLK);	// 0->1 enable PCM
#else
	rtl_outl(PCMCR,PCMENABLE | PCMCLK);	// 0->1 enable PCM
	rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
	rtl_outl(PCMCR,PCMENABLE | PCMCLK);	// 0->1 enable PCM
#endif	*/

	if ((mode&0x100)==0 ){
		rtl_outl(PCMCR,PCMMODE_LINEAR | PCMENABLE | PCMCLK);	// 0->1 enable PCM
		rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
		rtl_outl(PCMCR,PCMMODE_LINEAR | PCMENABLE | PCMCLK);	// 0->1 enable PCM
	}else{
		rtl_outl(PCMCR,PCMENABLE | PCMCLK);	// 0->1 enable PCM
		rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
		rtl_outl(PCMCR,PCMENABLE | PCMCLK);	// 0->1 enable PCM
	}


#if 1
	/* Install interrupt handler */
	//int_Register(7, (1 << 19), 0, pcm_interrupt);		init interrupt in 8672

	request_IRQ(19, &irq19_dump,NULL);

	/* Enable timer interrupt */
	//if ( 7 < getIlev() )
	//d	setIlev(7);
#endif


	// Enable PCM Channel
	for (i=0; i < CH_NUM; i++)
		init_pcm_dump_ch(i, mode);

	for (i = 0; i < CH_NUM; i++) {
		printk("tx[%d].payload = %X\n", i, tx[i].payload);
		printk("rx[%d].payload = %X\n", i, rx[i].payload);
		printk("comp[%d] = %X\n", i, comp[i]);
	}

	//set time slot
	//rtl_outl(PATSAR03,0x1000000);
	//rtl_outl(PATSAR03,0x0103090b);
	rtl_outl(PATSAR03,0x0004080c);
#if CH_NUM == 8
	rtl_outl(PATSAR47,0x02060a0e);
#endif

	print_pcm_regs();
#if 0 // debug
	while(1) ;
#endif

#if CH_NUM == 1
#ifdef ALAW_MODE
	rtl_outl(PACHCNR03, CH0ILBE | CH0uA| CH0RE | CH0TE);
#else
	rtl_outl(PACHCNR03, CH0RE | CH0TE);
#endif
#elif 	CH_NUM == 2

#ifdef ALAW_MODE

#if 0
	rtl_outl(PACHCNR03, CH1uA| CH1RE | CH1TE
				);
#else
	rtl_outl(PACHCNR03, CH0uA| CH0RE | CH0TE
				| CH1uA| CH1RE | CH1TE
				);
#endif

#else
	rtl_outl(PACHCNR03, CH0RE | CH0TE
				| CH1RE | CH1TE
				);
#endif

#elif 	CH_NUM == 4

#ifdef ALAW_MODE
	rtl_outl(PACHCNR03, CH0uA| CH0RE | CH0TE
				| CH1uA| CH1RE | CH1TE
				| CH2uA| CH2RE | CH2TE
				| CH3uA| CH3RE | CH3TE
				);
#else
	rtl_outl(PACHCNR03, CH0RE | CH0TE
				| CH1RE | CH1TE
				| CH2RE | CH2TE
				| CH3RE | CH3TE
				);

#endif

#elif 	CH_NUM == 8

	unsigned int uamode03,uamode47;
	uamode03 =0;
	uamode47 = 0;

	if((mode&1)==0)
		uamode03=CH0uA;
	if((mode&2)==0)
		uamode03|=CH1uA;
	if((mode&4)==0)
		uamode03|=CH2uA;
	if((mode&8)==0)
		uamode03|=CH3uA;
	if((mode&0x10)==0)
		uamode47=CH4uA;
	if((mode&0x20)==0)
		uamode47|=CH5uA;
	if((mode&0x40)==0)
		uamode47|=CH6uA;
	if((mode&0x80)==0)
		uamode47|=CH7uA;
	if((mode&0x200)==0)
		uamode03|=CH0ILBE;
	printk("pachcnr03 = %X\npachcnr47 = %X\n", uamode03  |  CH0RE | CH0TE
				| CH1RE | CH1TE
				| CH2RE | CH2TE
				| CH3RE | CH3TE, uamode47 | CH4RE | CH4TE
				|  CH5RE | CH5TE
				|  CH6RE | CH6TE
				|  CH7RE | CH7TE);

	rtl_outl(PACHCNR03,uamode03  |  CH0RE | CH0TE
				| CH1RE | CH1TE
				| CH2RE | CH2TE
				| CH3RE | CH3TE
				);
	rtl_outl(PACHCNR47,uamode47 | CH4RE | CH4TE
				|  CH5RE | CH5TE
				|  CH6RE | CH6TE
				|  CH7RE | CH7TE
				);

/*
#ifdef ALAW_MODE
#if 1
	rtl_outl(PACHCNR03, CH0ILBE | CH0uA| CH0RE | CH0TE
	//rtl_outl(PACHCNR03, CH0uA| CH0RE | CH0TE
				| CH1uA| CH1RE | CH1TE
				| CH2uA| CH2RE | CH2TE
				| CH3uA| CH3RE | CH3TE
				);
#endif
	rtl_outl(PACHCNR47, CH4uA| CH4RE | CH4TE
				| CH5uA| CH5RE | CH5TE
				| CH6uA| CH6RE | CH6TE
				| CH7uA| CH7RE | CH7TE
				);
#else
	rtl_outl(PACHCNR03, CH0ILBE |  CH0RE | CH0TE
	//rtl_outl(PACHCNR03,  CH0RE | CH0TE
				| CH1RE | CH1TE
				| CH2RE | CH2TE
				| CH3RE | CH3TE
				);
	rtl_outl(PACHCNR47, CH4RE | CH4TE
				|  CH5RE | CH5TE
				|  CH6RE | CH6TE
				|  CH7RE | CH7TE
				);
#endif	*/
#else
xxxxxx
#endif

	//rtl_outl(PACHCNR03, CH0RE | CH0TE); 		// MU-LAW
	//rtl_outl(PACHCNR03, CH0RE | CH0TE | CH0ILBE); 		// MU-LAW + internal loopback
	//rtl_outl(PACHCNR03,CH0RE | CH0TE | CH0ILBE); rtl_outl(PCMCR,rtl_inl(PCMCR)|PCMMODE);	// Linear Mode + internal loopback
	//rtl_outl(PACHCNR03,CH0RE | CH0TE); rtl_outl(PCMCR,rtl_inl(PCMCR)|PCMMODE);	// Linear Mode


	//printf("enable PCM....OK\n");
}
#endif
void breakpoint(void)
{

	break_point ++;
	return;
}


/*
void twiddle(void)
{
	static int twiddle_count = 0;
        static const char tiddles[]="-\\|/";
#if 1
        printfByPolling("%c", tiddles[(twiddle_count++)&3]);
        printfByPolling("%c", '\b');
#else
        putchar(tiddles[(twiddle_count++)&3]);
        putchar('\b');
#endif
}
*/


void pcm_set_tx_own_bit(unsigned int chid, unsigned int pageindex)
{
	if (chid < 4) {
		rtl_outl(CH03ATXBSA(chid), rtl_inl(CH03ATXBSA(chid))|BIT(pageindex));
	} else {
		rtl_outl(CH47ATXBSA(chid), rtl_inl(CH47ATXBSA(chid))|BIT(pageindex));
	}
}

void pcm_set_rx_own_bit(unsigned int chid, unsigned int pageindex)
{
	if (chid < 4) {
		rtl_outl(CH03ARXBSA(chid), rtl_inl(CH03ARXBSA(chid))|BIT(pageindex));
	} else {
		rtl_outl(CH47ARXBSA(chid), rtl_inl(CH47ARXBSA(chid))|BIT(pageindex));
	}
}

void pcm_stop_check(void)
{
	int temp_v;
		temp_v = get_timer_jiffies();
		temp_v = temp_v - pcm_test_starttime;
		//if (dot_cnt > 2400)
		if (temp_v > PCM_TEST_TIME_MIN*60*100) {
			//rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
			rtlRegMask(PACHCNR03, 0x03030303, 0);
			rtlRegMask(PACHCNR47, 0x03030303, 0);
			if (pcm_isr_test_flag)
				print_pcm_regs();
			pcm_isr_test_flag = 0;
		}
}

void pcm_isr_test(int chid, int pid);
static int isr_cnt = 0;

void pcm_ISR(uint32 pcm_isr03, uint32 pcm_isr47)
{
	uint32 chid = 0, i, j;
	static uint32 last_chid = CH_NUM - 1;
	uint32 tx_isrpage, rx_isrpage;
	uint32* pcm_isr = NULL;

#if 0
	//printk("cnt=%d\n", isr_cnt);
    	if ( isr_cnt == 0) {
    		printk("pcm_isr03 = %x\n", pcm_isr03);
    		printk("1pcm_isr47 = %x\n", pcm_isr47);
    	} else {
    		//printk("(%x, %x)\n", pcm_isr03, pcm_isr47);
    	}
#endif

	chid = (++last_chid) % CH_NUM;

	for (i=0; i < 2; i++) // page0/page1
	{
#if 0
	    int need_PCM_RX[CH_NUM];
	    memset(need_PCM_RX, 0, sizeof(need_PCM_RX));
#endif
	    for (j = 0; j < CH_NUM; j++, chid = (++chid)%CH_NUM)
	    {

	    	//printk("%d: chid =%d\n", isr_cnt, chid);
	    	//printk("%d", chid);

	    	if (chid < 4)
	    		pcm_isr = &pcm_isr03;
		else
	    		pcm_isr = &pcm_isr47;

		tx_isrpage = 2*chid + txpage[chid];
		rx_isrpage = 2*chid + rxpage[chid];


		if( ( (*pcm_isr) & ChanTxPage[tx_isrpage]) )
		{
			if(chid == 0)
			{
#ifdef PCM_PLAY_TONE
				tone_gens((PKTSIZE+1)*2,tx[chid].payload + PCMPAGE_SIZE/2*txpage[chid]);
#endif
#ifdef PCM_MIC_LOOPBACK_SPEAKER
				memcpy(tx[chid].payload + PCMPAGE_SIZE/2*txpage[chid], &pcm_databuf[pcm_tx_read_index], PCMPAGE_SIZE);
				pcm_tx_read_index = (pcm_tx_read_index + PCMPAGE_SIZE)%PCM_DATABUFSIZE;
#endif
			}
			pcm_set_tx_own_bit(chid, txpage[chid]);
			*pcm_isr &= ~ChanTxPage[tx_isrpage];
			txpage[chid] ^= 1;

			tr_cnt[chid]++;
		} // end of tx

		if( (*pcm_isr) & ChanRxPage[rx_isrpage] ) {
#if defined(PCM_SLIC_SI3215) || defined(PCM_CODEC_ALC5621) || defined(CONFIG_RTK_VOIP_DRIVERS_SLIC_SI3217x)
			if(chid == 0)
			{
    #ifndef PCM_PLAY_TONE
				memcpy(&pcm_databuf[pcm_rx_write_index], rx[chid].payload + PCMPAGE_SIZE/2*rxpage[chid], PCMPAGE_SIZE);
				pcm_rx_write_index = (pcm_rx_write_index + PCMPAGE_SIZE)%PCM_DATABUFSIZE;
    #endif
				pcm_stop_check();
			}
#else
			pcm_isr_test(chid, rxpage[chid]);
#endif

			pcm_set_rx_own_bit(chid, rxpage[chid]);
			//need_PCM_RX[chid] = 1;
			*pcm_isr &= ~ChanRxPage[rx_isrpage];
			rxpage[chid] ^= 1;
			last_chid = chid;
			tr_cnt[chid]--;
		}

	    } // end of for j

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
	if (pcm_isr03 != 0 || pcm_isr47 != 0) {
		printk(" pcm_isr03 = %X \n", pcm_isr03);
		printk(" pcm_isr47 = %X \n", pcm_isr47);
	} else {
		twiddle();
	}
#endif

}
#ifdef PCM_DUMP
void pcm_dump_ISR(uint32 pcm_isr03, uint32 pcm_isr47)
{
	uint32 chid = 0, i, j;
	static uint32 last_chid = CH_NUM - 1;
	uint32 tx_isrpage, rx_isrpage;
	uint32* pcm_isr = NULL;

#if 0
	//printk("cnt=%d\n", isr_cnt);
    	if ( isr_cnt == 0) {
    		printk("pcm_isr03 = %x\n", pcm_isr03);
    		printk("1pcm_isr47 = %x\n", pcm_isr47);
    	} else {
    		//printk("(%x, %x)\n", pcm_isr03, pcm_isr47);
    	}
#endif

	chid = (++last_chid) % CH_NUM;

	for (i=0; i < 2; i++) // page0/page1
	{
#if 0
	    int need_PCM_RX[CH_NUM];
	    memset(need_PCM_RX, 0, sizeof(need_PCM_RX));
#endif
	    for (j = 0; j < CH_NUM; j++, chid = (++chid)%CH_NUM)
	    {

	    	//printk("%d: chid =%d\n", isr_cnt, chid);
	    	//printk("%d", chid);

	    	if (chid < 4)
	    		pcm_isr = &pcm_isr03;
		else
	    		pcm_isr = &pcm_isr47;

		tx_isrpage = 2*chid + txpage[chid];
		rx_isrpage = 2*chid + rxpage[chid];


		if( ( (*pcm_isr) & ChanTxPage[tx_isrpage]) )
		{
			for (i=0;i<PCMPAGE_SIZE/2;i++){
			if((tx_i[chid]<65600) && (chid<2))
				tx_dump[chid][ tx_i[chid]++ ]=tx_index[chid];
			else
				tx_i[chid]++;
			//*(tx[chid].payload + PCMPAGE_SIZE/2*txpage[chid] + i)=tx_index[chid]++;
			*(tx[chid].payload + PCMPAGE_SIZE/2*txpage[chid] + i)=tx_index[chid];
			}
			pcm_set_tx_own_bit(chid, txpage[chid]);
			*pcm_isr &= ~ChanTxPage[tx_isrpage];
			txpage[chid] ^= 1;

			tr_cnt[chid]++;
		} // end of tx

		if( (*pcm_isr) & ChanRxPage[rx_isrpage] ) {
			//pcm_isr_test(chid, rxpage[chid]);
			if((rx_i[chid]<65600)&& (chid<2)){

				memcpy(&rx_dump[chid][ rx_i[chid] ],rx[chid].payload + PCMPAGE_SIZE/2*rxpage[chid], PCMPAGE_SIZE);
				rx_i[chid]+=PCMPAGE_SIZE/2;
			}else
				rx_i[chid]+=PCMPAGE_SIZE/2;
			memset(rx[chid].payload + (PCMPAGE_SIZE/2)*rxpage[chid], 0, PCMPAGE_SIZE);
			pcm_set_rx_own_bit(chid, rxpage[chid]);
			//need_PCM_RX[chid] = 1;
			*pcm_isr &= ~ChanRxPage[rx_isrpage];
			rxpage[chid] ^= 1;
			last_chid = chid;
			tr_cnt[chid]--;
		}

	    } // end of for j

	} // end of for i

	//printk("%d: 2 pcm_isr = %X \n", isr_cnt, pcm_isr);
#if 1
	if (pcm_isr03 != 0 || pcm_isr47 != 0) {
		printk(" pcm_isr03 = %X \n", pcm_isr03);
		printk(" pcm_isr47 = %X \n", pcm_isr47);
	} else {
		twiddle();
	}
#endif

	if((rx_i[0]>66000) && (pcm_isr_test_flag>0) ){
		rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
		for(j=0;j<2;j++){	// chid
			printk("\nTX[%d]:\n",j);
			for(i=0;i<65536;i++){
				if ((i%8) == 7)
					printk(" %x\n",tx_dump[j][i]);
				else
					printk(" %x ",tx_dump[j][i]);
			}
			printk("\nRX[%d]:\n",j);
			for(i=0;i<65536;i++){
				if ((i%8) == 7)
					printk(" %x\n",rx_dump[j][i]);
				else
					printk(" %x ",rx_dump[j][i]);
			}
		}
		print_pcm_regs();
		pcm_isr_test_flag = 0;
	}

}
#endif
void pcm_interrupt()
{
	unsigned int status_val;
	unsigned int status_val47;
#if CH_NUM > 4
	status_val = rtl_inl(PAISR03);
	status_val47 = rtl_inl(PAISR47);
	if( status_val || status_val47 )
	{
		rtl_outl(PAISR03, status_val);
		rtl_outl(PAISR47, status_val47);

		pcm_ISR(status_val & 0xF0F0F0F0, status_val47 & 0xF0F0F0F0);

		if( (status_val & 0x0F0F0F0F) || (status_val47 & 0x0F0F0F0F)) // Buffer Unavailable only
		{
			printk("\n%s-%d: BU, isr_cnt = %d: status_val=%x, status_val47=%x\n", __FUNCTION__, __LINE__, isr_cnt, status_val, status_val47);
			print_pcm_regs();
			//while (1) ;
		}
	}
#else
	if( (status_val = rtl_inl(PAISR03)) )
	{
		rtl_outl(PAISR03, status_val);
		pcm_ISR(status_val & 0xF0F0F0F0, 0);

		if( (status_val & 0x0F0F0F0F) ) // Buffer Unavailable only
		{
			printk("\n%s-%d: BU, isr_cnt = %d: status_val=%x\n", __FUNCTION__, __LINE__, isr_cnt, status_val);
			while (1) ;
		}
	}
#endif
	isr_cnt++;
}
#ifdef PCM_DUMP
void pcm_dump_interrupt()
{
	unsigned int status_val;
	unsigned int status_val47;
#if CH_NUM > 4
	status_val = rtl_inl(PAISR03);
	status_val47 = rtl_inl(PAISR47);
	if( status_val || status_val47 )
	{
		rtl_outl(PAISR03, status_val);
		rtl_outl(PAISR47, status_val47);

		pcm_dump_ISR(status_val & 0xF0F0F0F0, status_val47 & 0xF0F0F0F0);

		if( (status_val & 0x0F0F0F0F) || (status_val47 & 0x0F0F0F0F)) // Buffer Unavailable only
		{
			printk("\n%s-%d: BU, isr_cnt = %d: status_val=%x, status_val47=%x\n", __FUNCTION__, __LINE__, isr_cnt, status_val, status_val47);
			print_pcm_regs();
			while (1) ;
		}
	}
#else
	if( (status_val = rtl_inl(PAISR03)) )
	{
		rtl_outl(PAISR03, status_val);
		pcm_ISR(status_val & 0xF0F0F0F0, 0);

		if( (status_val & 0x0F0F0F0F) ) // Buffer Unavailable only
		{
			printk("\n%s-%d: BU, isr_cnt = %d: status_val=%x\n", __FUNCTION__, __LINE__, isr_cnt, status_val);
			while (1) ;
		}
	}
#endif
	isr_cnt++;
}
#endif
void pcm_isr_test(int chid, int pindex)
{
	int 		i;
	static unsigned int txpindex = 0;
	int temp_v;

			{  // chech rx data

				static unsigned int err_cnt = 0;//error page
				unsigned int err_cnt_in = 0;
				static unsigned int err_sample=0;

				//rtl_outl(CH0ATXBSA,  rtl_inl(CH0ATXBSA) | BIT(txpindex)); // set tx own bit ASAP, solve ERROR!!

//#if PKTSIZE == 39 // partial test for reducing computation
#if 0
				for (i=0;i<PCMPAGE_SIZE/2;i=i+32)
#else // full test
				for (i=0;i<PCMPAGE_SIZE/2;i++)
#endif
				{
					short rxi = *(rx[chid].payload + PCMPAGE_SIZE/2*pindex + i);
					short cmp = comp[chid][PCMPAGE_SIZE/2*pindex + i];
					//if ( rxi != 0x1256) {	// linear
					//if ( rxi != 0x1280) {	// a-law
					//if ( rxi != 0x11FC) {	// u-law
					//if ( rxi != 0x8284) {	// u-law + Rx of 8186 connect with Gnd.
					//if ( rxi != 0x0000) {	// u-law + Rx of 8186 connect with Vcc.
					if ( rxi != cmp) { //u-law fixed pattern
						//printfByPolling("\n%d-%d-%d-%d\n", chid, test_cnt[chid] , i, pindex);
						printfByPolling("rxi =%x\n", rxi);
						printfByPolling("cmp =%x\n", cmp);
						//printfByPolling("rx[%d].payload=%x\n", chid, rx[chid].payload);
						//printfByPolling("comp[%d]=%x\n", chid, comp[chid]);
						//print_pcm_regs();
						//while (1)
						//	;
						if ( err_cnt_in == 0)
							err_cnt++;
						err_cnt_in++;
					}
				}

				err_sample += err_cnt_in; // the sum of error sample.
				//printf("\nj=%d (%d,%d)\n", j, err_cnt, err_cnt_in); // induces ERROR if the line above not exist!
				if (err_cnt_in)
					breakpoint();
					//goto label1;
				if (err_cnt_in) {
					printfByPolling("\ntest_cnt[%d]=%d (%d,%d)\n", chid, test_cnt[chid], err_cnt, err_sample);
				} else {
					//twiddle();
					//printk("%d", chid+1);
				}
#if PKTSIZE != 39
				// clean RX buf
				memset(rx[chid].payload + (PCMPAGE_SIZE/2)*pindex, 0, PCMPAGE_SIZE);
#endif
			} // check rx data

		if (test_cnt[chid]%1000 == 0) {
			//printfByPolling("j=%d\n", j);
			static int dot_cnt = 0;
			if (dot_cnt++ % 80 == 0)
				printfByPolling("\n");
			//printfByPolling("%d", chid);
			temp_v = get_timer_jiffies();
			temp_v = temp_v - pcm_test_starttime;
			//if (dot_cnt > 2400)
			if (temp_v > PCM_TEST_TIME_MIN*60*100) {
				rtl_outl(PCMCR,0x0000);	// 1->0 reset PCM
				if (pcm_isr_test_flag)
					print_pcm_regs();
				pcm_isr_test_flag = 0;
			}
		}

		test_cnt[chid]++;
}
