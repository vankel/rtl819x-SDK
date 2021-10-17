#ifndef _NAND_FLASH_H_
#define _NAND_FLASH_H_


#include <linux/config.h>

#define NAND_DBG 0

#define rtk_readb(offset)         (*(volatile unsigned char *)(offset))
#define rtk_readw(offset)         (*(volatile unsigned short *)(offset))
#define rtk_readl(offset)         (*(volatile unsigned long *)(offset))

#define rtk_writeb(val, offset)    (*(volatile unsigned char *)(offset) = val)
#define rtk_writew(val, offset)    (*(volatile unsigned short *)(offset) = val)
#define rtk_writel(val, offset)    (*(volatile unsigned long *)(offset) = val)

#define REG32(reg)	(*(volatile unsigned int *)(reg))
//#include "../../../autoconf.h"
#define clk_manage_REG	0xb8000010

/*
 * NAND flash controller address
 *  czyao , nand flash address
 */
#define NAND_CTRL_BASE  0xB801A000
#define NACFR  (NAND_CTRL_BASE + 0x0)
#define NACR    (NAND_CTRL_BASE + 0x04)
#define flash_READY  (1<<31)
#define ECC_enable    (1<<30)
#define RBO		     (1<<29)
#define WBO		     (1<<28)
#define CE_TWP(val)   ((val)<<16) /*4 bit */
#define CE_TWH(val) ((val)<<12) /*4 bit */
#define CE_TRR(val) ((val)<<8) /*4 bit */
#define CE_TH(val) ((val)<<4) /*4 bit, */
#define CE_TS(val) ((val)<<0) /*4 bit */

#define NAND_WP_B  (1<<23)


#define NACMR    (NAND_CTRL_BASE + 0x08)
#define CECS1 	(1<<31)
#define CECS0	(1<<30)
#define Chip_Seletc_Base	30
#define NAADR    (NAND_CTRL_BASE + 0x0C)
#define enNextAD		(1<<27)
#define AD2EN		(1<<26)
#define AD1EN		(1<<25)
#define AD0EN		(1<<24)
#define CE_ADDR2		16
#define CE_ADDR1		8
#define CE_ADDR0		0
#define NADCRR   (NAND_CTRL_BASE + 0x10)
#define TAG_SEL		7
#define TAG_DIS		(1<<6)
#define DESC1		(1<<5)
#define DESC0		(1<<4)
#define DMARE		(1<<3)
#define DMAWE		(1<<2)
#define LBC_128		3
#define LBC_64		2
#define LBC_32		1
#define LBC_16		0
#define NADR        (NAND_CTRL_BASE + 0x14)
#define NADFSAR (NAND_CTRL_BASE + 0x18)
#if 0//RTL819xD
#define NADRSAR (NAND_CTRL_BASE + 0x1C)
#define NASR        (NAND_CTRL_BASE + 0x20)
#define NANDPReg	(NAND_CTRL_BASE + 0x24)
#define NADTSAR	(NAND_CTRL_BASE + 0x54)
#else //RTL8198C
#define NADFSAR2   (NAND_CTRL_BASE + 0x1c)
#define NADRSAR (NAND_CTRL_BASE + 0x20)
#define NADTSAR	(NAND_CTRL_BASE + 0x24)
#define NASR        (NAND_CTRL_BASE + 0x28)
#define NANDPReg	(NAND_CTRL_BASE + 0x3c)

#endif
#define NECN			(1<<4)
#define NRER			(1<<3)
#define NWER			(1<<2)
#define NDRS			(1<<1)
#define NDWS		(1<<0)


#define M_mask		0xe0000000

/*NAND flash function*/
/*void rtk_nand_read_id(void)*/

/* NAND Flash Command Sets */
#define CMD_READ_ID				0x90
#define CMD_READ_STATUS		0x70

#define CMD_PG_READ_C1		0x00
#define CMD_PG_READ_C2		0x30
#define CMD_PG_READ_C3		CMD_READ_STATUS

#define CMD_PG_WRITE_C1		0x80
#define CMD_PG_WRITE_C2		0x10
#define CMD_PG_WRITE_C3		CMD_READ_STATUS

#define CMD_BLK_ERASE_C1		0x60	
#define CMD_BLK_ERASE_C2		0xd0	
#define CMD_BLK_ERASE_C3		CMD_READ_STATUS	

#define CMD_RESET                 0xff
#define CMD_RANDOM_DATA_INPUT     0x85    /* RANDOM DATA write */ 

#define CMD_RANDOM_DATA_OUTPUT_C1 0x05    /* RANDOM DATA read */
#define CMD_RANDOM_DATA_OUTPUT_C2 0xe0 

//porting cl which one is true?
#define	BB_INIT		0xFFFE
#define	RB_INIT		0xFFFD
#define	BBT_TAG		0xBB
#define 	RSV_TAG 	0xCC
#define 	BB_DIE_INIT	0xEEEE
#define 	RB_DIE_INIT	BB_DIE_INIT

#if 0
struct nand_chip {

	u_char oob_shift;
	void (*read_id) (struct mtd_info *mtd, unsigned char id[5]);
	int (*read_ecc_page) (struct mtd_info *mtd, u16 chipnr, unsigned int page, u_char *data, 
									u_char *oob_buf);
	int (*read_oob) (struct mtd_info *mtd, u16 chipnr, int page, int len, u_char *buf);
	int (*write_ecc_page) (struct mtd_info *mtd, u16 chipnr, unsigned int page, const u_char *data,
										const u_char *oob_buf, int isBBT);										
	int (*write_oob) (struct mtd_info *mtd, u16 chipnr, int page, int len, const u_char *buf);
	int (*erase_block) (struct mtd_info *mtd, u16 chipnr, int page);
	void (*sync) (struct mtd_info *mtd);
	/* CMYu, 20090422 */
	void (*suspend) (struct mtd_info *mtd);
	void (*resume) (struct mtd_info *mtd);
	/* Ken.Yu, 20080615 */
	void (*select_chip) (struct mtd_info *mtd, int chip);
	void	(*read_buf)(struct mtd_info *mtd, u_char *buf, int len);
	void 	(*cmdfunc)(struct mtd_info *mtd, unsigned command, int column, int page_addr);
	int  (*dev_ready)(struct mtd_info *mtd);
	int (*scan_bbt)(struct mtd_info *mtd);
	int		eccmode;
	int		eccsize;
	int		eccbytes;
	int		eccsteps;
	int 		chip_delay;
	spinlock_t	chip_lock;
	wait_queue_head_t wq;
	nand_state_t 	state;
	int 		page_shift;
	int		phys_erase_shift;
	int		bbt_erase_shift;
	int		chip_shift;
	u_char 		*g_databuf;
	u_char		*g_oobbuf;
	int		oobdirty;
	u_char		*data_poi;
	unsigned int	options;
	int		badblockpos;
	int		numchips;
	unsigned long	chipsize;
	/* Ken-Yu, 20090108 */
	unsigned long	device_size;
	int		pagemask;
	int		pagebuf;
	struct nand_oobinfo	*autooob;
	uint8_t		*bbt;
	struct nand_bbt_descr	*bbt_td;
	struct nand_bbt_descr	*bbt_md;
	struct nand_bbt_descr	*badblock_pattern;	
	struct nand_hw_control  *controller;
	void		*priv;
	/* Ken-Yu, 20080618 */
	unsigned char maker_code;	
	unsigned char device_code; 
	unsigned int ppb;	/*page per block*/
	unsigned int oob_size;	/*spare area size*/
	unsigned int block_num;
	unsigned int page_num;
	BB_t *bbt;
	unsigned int RBA;
	unsigned int RBA_PERCENT;
	/* Ken-Yu, 20081004 */
	__u32 *erase_page_flag;
	/* Ken-Yu, 20081013 */
	unsigned char active_chip;
	unsigned int BBs;
	/* CMYu, 20090720 */
	unsigned int mcp;
};
#endif

struct device_type{
    unsigned char  *name;
    unsigned int id;
    unsigned int size;		
    unsigned long chipsize;		
    unsigned short PageSize;
    unsigned int BlockCnt;
    unsigned int BlockSize;    
    unsigned short OobSize;
    unsigned char num_chips;
    unsigned char isLastPage;	
    unsigned char CycleID5th; 
    unsigned char T1;
    unsigned char T2;
    unsigned char T3;
};

struct Boot_Rsv{
    unsigned int num;
    unsigned int start_block;
};
struct  BBT_v2r{
    unsigned int block_r;
};

struct  BB_t{
    unsigned short BB_die;
    unsigned short bad_block;
    unsigned short RB_die;
    unsigned short remap_block;
};
#define BOOT_SIZE	           0x100000 //jasonwang 0706
#define NULL			0
#define SUCCESS		0
#define FAIL			-1


#define Boot_Select			0xB801a000
#define NAND_ADDR_MASK		(1<<31)

//porting cl,for 128 can not be 3 address cycle
/*NAND_ADDR_CYCLE = 1, address cycle=3, NAND_ADDR_CYCLE=0, address cycle=4 or 5*/
//#define NAND_ADDR_CYCLE	(((*(volatile unsigned int *)((Boot_Select)) & NAND_ADDR_MASK) == 0) ? 1:0)
#define NAND_ADDR_CYCLE	0

#define USER_SPACE_SIZE        0x800000   //jasonwang must modified 
#define VIMG_SPACE_SIZE        0x600000
//debug cl redefined
//#define CMD_PG_READ_C1		0x00
//#define CMD_PG_READ_C2		0x30
//#define CMD_PG_READ_C3		CMD_READ_STATUS

#define CMD_PG_READ_A		0x00
#define CMD_PG_READ_B		0x01
#define CMD_PG_READ_C		0x50

//debug cl redefined
//#define CMD_PG_WRITE_C1		0x80
//#define CMD_PG_WRITE_C2		0x10
//#define CMD_PG_WRITE_C3		CMD_READ_STATUS
//
//#define CMD_BLK_ERASE_C1		0x60	
//#define CMD_BLK_ERASE_C2		0xd0	
//#define CMD_BLK_ERASE_C3		CMD_READ_STATUS	

//debug cl redefined
//#define CMD_RESET                 		0xff
#define BACKUP_LOADER 3
//#define BACKUP_IMAGE 3  //jasonwang must modified it 0713
//debug cl should be 1 not bake
#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_STATIC
#define BACKUP_IMAGE 2  //jasonwang must modified it 0713
#define IMG_BACKUP_ADDR  CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET
#else
#define BACKUP_IMAGE 1  //jasonwang must modified it 0713
#define IMG_BACKUP_ADDR  0x1800000
#endif
extern int nand_erase_nand (unsigned int addr, unsigned int len);
extern int nand_read_ecc_ob (unsigned int from, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf);
extern unsigned int page_size;
extern unsigned int oob_size ;
extern unsigned int ppb ;   
extern int nand_select;
extern int block_size;
extern int block_shift;
extern int pagemask;
extern int page_shift;

/*winfred_wang*/
#define CONFIG_RTK_NAND_PAGE_2K
#ifdef CONFIG_RTK_NAND_PAGE_2K
#define KEEP_ORI_BBI /*keep original bad block indicator, for page size 2K*/
#endif

#ifdef KEEP_ORI_BBI
#define DATA_BBI_OFF  ((512*4)- 48)/*(0~1999)512+512+512+512-48*/
#define OOB_BBI_OFF  ((16*4) - 10 -1) /*(0~23)6+[10]+6+[10]+6+[10]+6+[10] []:ecc bytes*/
#else
#define OOB_BBI_OFF  0 /*(0~23)6+[10]+6+[10]+6+[10]+6+[10] []:ecc bytes*/
#define DATA_BBI_OFF 0
#endif

#define DRAM_DIMAGE_ADDR       0xa0a00000
#define DRAM_DOOB_ADDR           0xa1000000


#endif
