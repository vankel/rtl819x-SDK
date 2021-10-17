
#include <linux/autoconf2.h>

#include "rtk_nand.h"
  
#include <asm/rtl8196.h>        //define in boot's MAKEFILE

#define SWAP_2K_DATA 1
#define nand_index      0       /*select the nand flash */

#define BOOT_BLOCK		2

/* bbt location */
#define REMAP_BBT_POS			0x100000	/*1M*/
#define NORMAL_BBT_POSITION     0x200000 	/*2M*/
  
  //#define K9WAG08U1A      0xECD35195/*Samsung MLC, 2048 MB, 1 dies*/   /*Same ID with 8Gb  from Samsung's datasheet?*/
  
  /*NAND_Flash_Large_Page_256MBto2GB_5cycles */ 
#define K9GAG08U0M      0xECD514B6      /*Samsung MLC, 2048 MB, 1 dies */
#define K9K8G08U0A      0xECD35195      /*Samsung MLC, 1024 MB, 1 dies */
#define K9F4G08U0A      0xECDC1095      /*Samsung SLC, 512 MB, 1 dies */
#define K9F2G08U0A      0xECDA1095      /*Samsung SLC, 256 MB, 1 dies */
  //#define HY27UF082G2B  0xADDA1095  /*Hynix SLC, 256 MB, 1 dies*/
  
  /*NAND_Flash_Large_Page_128MB_4cycles */ 
#define K9F1G08U0M      0xECF10095      /*Samsung SLC, 128 MB, 1 dies */
#define MX30LF1G08      0xc2f1801d      /*MXIC, 128 MB, 1 dies */
#define MT29F1G08ABADAWP      0x2CF18095      /* ? Micron SLC, 128 MB, 1 dies */
#define H27U1G8F2CTR      0xADF1801D      /* ? Hynix, 128 MB, 1 dies */
  
  /*NAND_Flash_Small_Page_64MB_4cycles */ 
#define K9F1208U0C      0xEC765A3F      /*Samsung SLC, 512 Mbit, 1 dies */
  
  /*NAND_Flash_Small_Page_32MB_3cycles */ 
#define K9F5608U0D      0xEC75A5BD      /*Samsung SLC, 256 Mbit, 1 dies */
#define HY27US08561A      0xAD75AD75    /*Hynix SLC, 256 Mbit, 1 dies */
#define F59L1G81A               0x92F18095 
#define F59L4G81A  		0xc8dc9095
#define F59L1G81LA  0xc8d18095

//porting cl
#define S34ML01G200TFI000 0x01F1801D

#define S34ML02G200TFI00  0x01DA9095
  //01h/DCh/90h/95h/56h
#define S34ML04G200TFI00  0x01DC9095
  //typedef unsigned int uint32;
extern void *malloc (uint32 nbytes );  /* bytes to allocate */
extern void free (void *ap );
#ifdef SWAP_2K_DATA
/*winfred_wang*/
#define BBI_DMA_OFFSET		((512*4)- 48)
//#define BBI_SWAP_OFFSET		5//test
#define BBI_SWAP_OFFSET		((16*4) - 10 -1)
//#define BBI_DMA_OFFSET  (0x800-((0x200)*3))+1
//#define BBI_SWAP_OFFSET 5
#endif

  //NAND flash structure
struct device_type nand_info;
const struct device_type nand_device[] = { 
    //{ "K9WAG08U1A", K9WAG08U1A, 0x80000000, ((2048)*64*16384), 2048, 64 * 2048, 64, 1, 0, 0x58, 0x00, 0x00, 0x00 },

    /*2048MB*/
  {"K9GAG08U0M", K9GAG08U0M, 0x80000000, ((2048) * 64 * 16384), 2048, 16384,64 * 2048, 64, 1, 0, 0x74, 0x00, 0x00, 0x00}, 

   /*1024MB*/
  {"K9K8G08U0A", K9K8G08U0A, 0x40000000,  ((2048) * 64 * 8192),  2048, 8192, 64 * 2048, 64, 1, 0, 0x58, 0x00, 0x00, 0x00},

  /*512MB*/
  {"K9F4G08U0A", K9F4G08U0A, 0x20000000, ((2048) * 64 * 4096), 2048, 4096, 64 * 2048, 64, 1, 0, 0x54, 0x00, 0x00, 0x00}, 

  /*256MB*/
  {"K9F2G08U0A", K9F2G08U0A,0x10000000, ((2048) * 64 * 2048), 2048, 2048, 64 * 2048, 64, 1, 0, 0x44, 0x00, 0x00, 0x00}, 
    //  { "HY27UF082G2B", HY27UF082G2B, 0x10000000, 0x10000000, 2048, 64 * 2048, 64, 1, 0, 0x44, 0x00, 0x00, 0x00 },
  
  /*128MB*/
  {"K9F1G08U0M", K9F1G08U0M, 0x08000000, ((2048) * 64 * 1024), 2048, 1024, 64 * 2048, 64, 1, 0, 0x0, 0x00, 0x00, 0x00}, 
  {"MX30LF1G08", MX30LF1G08, 0x08000000,  ((2048) * 64 * 1024), 2048, 1024, 64 * 2048, 64, 1, 0, 0x0, 0x00, 0x00, 0x00},
  {"MT29F1G08ABADAWP", MT29F1G08ABADAWP, 0x08000000,  ((2048) * 64 * 1024), 2048, 1024, 64 * 2048, 64, 1, 0, 0x0, 0x00, 0x00, 0x00},
  {"H27U1G8F2CTR", H27U1G8F2CTR, 0x08000000,  ((2048) * 64 * 1024), 2048, 1024, 64 * 2048, 64, 1, 0, 0x0, 0x00, 0x00, 0x00},
/*debug cl*/
  {"F59L1G81A", F59L1G81A, 128, ((2048) * 64 * 1024), 2048, 1024, 64 * 2048,
   64, 1, 0, 0x0, 0x00, 0x00, 0x00},
  {"F59L4G81A", F59L4G81A, 0x20000000, ((4096) * 64 * 2048), 2048, 4096, 64 * 2048,64, 1, 0, 0x0, 0x00, 0x00, 0x00},

  {"F59L1G81LA", F59L1G81LA, 0x20000000, ((4096) * 64 * 2048), 2048, 4096, 64 * 2048,64, 1, 0, 0x0, 0x00, 0x00, 0x00},
/*debug cl*/
  {"S34ML01G200TFI000", S34ML01G200TFI000, 128, ((2048) * 64 * 1024), 2048, 1024, 64 * 2048,
   64, 1, 0, 0x0, 0x00, 0x00, 0x00},

  /*64MB*/
  {"K9F1208U0C", K9F1208U0C, 0x04000000, ((512) * 32 * 4096), 512, 4096, 32 * 512, 16, 1, 0, 0x0, 0x00, 0x00, 0x00}, 

  /*32MB*/
  {"K9F5608U0D", K9F5608U0D, 0x02000000, ((512) * 32 * 2048), 512, 2048, 32 * 512, 16, 1, 0, 0x0, 0x00, 0x00, 0x00},
  {"HY27US08561A", HY27US08561A, 0x02000000, ((512) * 32 * 2048), 512, 2048, 32 * 512, 16, 1, 0, 0x0, 0x00, 0x00, 0x00}, 

  /* 256M 2048+128 */
  {"S34ML02G200TI00",S34ML02G200TFI00,0x10000000,((2048) * 64 * 2048), 2048, 2048, 64 * 2048, 64, 1, 0, 0x44, 0x00, 0x00, 0x00}, 
	
  {"S34ML04G200TI00",S34ML04G200TFI00,0x20000000,((2048) * 64 * 2048), 2048, 2048, 64 * 2048, 64, 1, 0, 0x44, 0x00, 0x00, 0x00}, 
};


//jason0709
int nand_select=-1;
int block_size=131072;
int block_shift=0, pagemask=0,page_shift=0;
unsigned int isLastPage = 0;
unsigned int chip_size = 0;
unsigned char mybuf[64];
unsigned char mydatabuf[2048];
#if defined(CONFIG_NAND_Flash_Large_Page_128MB_4cycles)  || defined (CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles)// CONFIG_NAND_BLOCK_2K
unsigned char NfDataBuf[2048];
unsigned char NfSpareBuf[64];
#else
unsigned char NfDataBuf[512];
unsigned char NfSpareBuf[16];
#endif

  /*
     struct device_type{
     unsigned char  *name;
     unsigned int id;
     unsigned int size;
     unsigned int chipsize; //chipsize=((page size)Byte *  PagesNumbers * BlocksNumbers)
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
     
   */ 
   
#ifdef CONFIG_RTK_NAND_BBT



int static_for_create_v2r_bbt = 0;
struct BB_t *bbt;
struct BBT_v2r *bbt_v2r;
int RBA_PERCENT = 5; //reserve 5% area in the end for bbt
unsigned int RBA;// = 102; 102 = (32MB/16K) * 5%
int read_has_check_bbt = 0;
unsigned int read_block = 0XFFFFFFFF;
unsigned int read_remap_block = 0XFFFFFFFF;
int write_has_check_bbt = 0;
unsigned int write_block = 0XFFFFFFFF;
unsigned int write_remap_block = 0XFFFFFFFF;
unsigned int erase_block = 0XFFFFFFFF;
unsigned int erase_remap_block = 0XFFFFFFFF;
int erase_has_check_bbt = 0;
static int rtk_create_bbt( int page);
static void dump_BBT(void);
#define BACKUP_BBT 3
#endif
#if 1
#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
unsigned int page_size = 2048;
unsigned int oob_size = 64;
unsigned int ppb = 64;         //page per block
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles
unsigned int page_size = 2048;
unsigned int oob_size = 64;
unsigned int ppb = 64;         //page per block
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Small_Page_64MB_4cycles
unsigned int page_size = 512;
unsigned int oob_size = 16;
unsigned int ppb = 32;         //page per block
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Small_Page_32MB_3cycles
unsigned int page_size = 512;
unsigned int oob_size = 16;
unsigned int ppb = 32;         //page per block
#endif  /*  */
#else   /*  */
#endif  /*  */
void 
check_ready_nand (void ) 
{
  while (1)
    {
      if ((rtk_readl (NACR) & 0x80000000) == 0x80000000)
        break;
    }
}

int 
rtk_nand_read_id () 
{
  int i;
  int id_chain;
  
    //unsigned char id[5];
    check_ready_nand ();
  
    //rtk_writel( (rtk_readl(NACR) |ECC_enable|RBO|WBO), NACR);
    rtk_writel ((rtk_readl (NACR) | ECC_enable), NACR);
  rtk_writel (0x0, NACMR);
  rtk_writel ((CECS0 | CMD_READ_ID), NACMR);   //read ID command
  check_ready_nand ();
  rtk_writel (0x0, NAADR);
  rtk_writel ((0x0 | AD2EN | AD1EN | AD0EN), NAADR);   //dummy address cycle
  check_ready_nand ();
  id_chain = rtk_readl (NADR);
  
    //prom_printf("\nIn rtk_nand_read_id(),NACR(0xb801a004) =%x",rtk_readl(NACR) );
    //prom_printf ("\nNAND probe,get original NAND ID =%x\n", id_chain);
  
#if 0
    id[0] = id_chain & 0xff;
  id[1] = (id_chain >> 8) & 0xff;
  id[2] = (id_chain >> 16) & 0xff;
  id[3] = (id_chain >> 24) & 0xff;
  id[4] = id_chain & 0xff;
  
#else   /*  */
    rtk_writel (0x0, NACMR);
  rtk_writel (0x0, NAADR);
  return id_chain;
  
#endif  /*  */
}
unsigned int shift_value(unsigned int x)
{
	int r = 1;

        if (!x)
                return 0;
        if (!(x & 0xffff)) {
                x >>= 16;
                r += 16;
        }
        if (!(x & 0xff)) {
                x >>= 8;
                r += 8;
        }
        if (!(x & 0xf)) {
                x >>= 4;
                r += 4;
        }
        if (!(x & 3)) {
                x >>= 2;
                r += 2;
        }
        if (!(x & 1)) {
                x >>= 1;
                r += 1;
        }
        return r;
}

#define REG32(reg)	(*(volatile unsigned int *)(reg))
#if 1
int rtk_erase_block (int page ) 
{
  //debug cl
  //dprintf("[%s]:%d for boot safe debug return directly\n",__func__,__LINE__);
  //return FAIL;

    //debug cl for safe not write in 0,1 block that is for boot.
	//if(page < 128||page > chip_size/page_size - 1)
	if(page > chip_size/page_size - 1)
	{
		dprintf("[%s]:%d panic:fatal error *************,try to write in 0,1 block for boot,protect for debug\n",__func__,__LINE__);
		return FAIL;
	}
  int addr_cycle[5], page_shift;
  
    //int block=page/64;
#ifdef CONFIG_SHOW_NAND_DBG_MSG
    dprintf ("\n/////////////////////////////////\n\r");
  
#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
    dprintf ("\nStarting Erase Block=%d\n\r", page / 64);
  dprintf ("Erase NAND Flash page=%d ~ %d\n\r", page, ((page + 63)));
  dprintf ("Erase NAND Flash Size=0x%x ~ 0x%x\n\n\r", (2112) * page, 
            ((2112) * (page + 64)) - 1);
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles
    dprintf ("\nStarting Erase Block=%d\n\r", page / 64);
  dprintf ("Erase NAND Flash page=%d ~ %d\n\r", page, ((page + 63)));
  dprintf ("Erase NAND Flash Size=0x%x ~ 0x%x\n\n\r", (2112) * page, 
            ((2112) * (page + 64)) - 1);
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Small_Page_64MB_4cycles
    dprintf ("\nStarting Erase Block=%d\n\r", page / 32);
  dprintf ("Erase NAND Flash page=%d ~ %d\n\r", page, ((page + 31)));
  dprintf ("Erase NAND Flash Size=0x%x ~ 0x%x\n\n\r", (528) * page, 
            ((528) * (page + 32)) - 1);
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Small_Page_32MB_3cycles
    dprintf ("\nStarting Erase Block=%d\n\r", page / 32);
  dprintf ("Erase NAND Flash page=%d ~ %d\n\r", page, ((page + 31)));
  dprintf ("Erase NAND Flash Size=0x%x ~ 0x%x\n\n\r", (528) * page, 
            ((528) * (page + 32)) - 1);
  
#endif  /*  */
#endif  /*  */
    check_ready_nand ();
  
#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
    if (page & (ppb - 1))
    {
      dprintf ("page %d is not block alignment (1 block=64=0x40 pages)!!\n",
                page);
      return;
    }
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles
    if (page & (ppb - 1))
    {
      dprintf ("page %d is not block alignment (1 block=64=0x40 pages)!!\n",
                page);
      return;
    }
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Small_Page_64MB_4cycles
    if (page & (ppb - 1))
    {
      dprintf ("page %d is not block alignment (1 block=32=0x20 pages)!!\n",
                page);
      return;
    }
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Small_Page_32MB_3cycles
    if (page & (ppb - 1))
    {
      dprintf ("page %d is not block alignment (1 block=32=0x20 pages)!!\n",
                page);
      return;
    }
  
#endif  /*  */
    check_ready_nand ();
  
    //rtk_writel( (rtk_readl(NACR) |ECC_enable|RBO), NACR);
    //JSW RBO=0 ,WBO=0
    rtk_writel ((rtk_readl (NACR) | ECC_enable & ~(3 << 28)), NACR);
  rtk_writel ((NWER | NRER | NDRS | NDWS), NASR);
  rtk_writel (0x0, NACMR);
  rtk_writel ((CECS0 | CMD_BLK_ERASE_C1), NACMR);
  check_ready_nand ();
  
#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
    addr_cycle[0] = addr_cycle[1] = 0;
  for (page_shift = 0; page_shift < 3; page_shift++)
    {
      addr_cycle[page_shift + 2] = (page >> (8 * page_shift)) & 0xff;
    }
  
    //rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|(addr_cycle[2]<<CE_ADDR2)),NAADR);
    rtk_writel 
    (((~enNextAD) & AD2EN | AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR0) | 
      (addr_cycle[3] << CE_ADDR1) | (addr_cycle[4] << CE_ADDR2) ), NAADR);
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles
    addr_cycle[0] = addr_cycle[1] = 0;
  for (page_shift = 0; page_shift < 2; page_shift++)
    {
      addr_cycle[page_shift + 2] = (page >> (8 * page_shift)) & 0xff;
    }
  
    //rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|(addr_cycle[2]<<CE_ADDR2)),NAADR);
    rtk_writel 
    (((~enNextAD) &  AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR0) | 
      (addr_cycle[3] << CE_ADDR1)  ), NAADR);
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Small_Page_64MB_4cycles
    addr_cycle[0] = addr_cycle[1] = 0;
  for (page_shift = 0; page_shift < 3; page_shift++)
    {
      addr_cycle[page_shift + 2] = (page >> (8 * page_shift)) & 0xff;
    }
  
    //rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|(addr_cycle[2]<<CE_ADDR2)),NAADR);
    rtk_writel 
    (((~enNextAD) & AD2EN | AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR0) | 
      (addr_cycle[3] << CE_ADDR1) | (addr_cycle[4] << CE_ADDR2) ), NAADR);
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Small_Page_32MB_3cycles
    addr_cycle[0] = addr_cycle[1] = 0;
  for (page_shift = 0; page_shift < 2; page_shift++)
    {
      addr_cycle[page_shift + 2] = (page >> (8 * page_shift)) & 0xff;
    }
  
    //rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|(addr_cycle[2]<<CE_ADDR2)),NAADR);
    #if 0
    rtk_writel (((~enNextAD) & AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR0) | 
                 (addr_cycle[3] << CE_ADDR1)), NAADR);
    #else

     int NAADR_data=(~enNextAD) & AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR0) | (addr_cycle[3] << CE_ADDR1); 
     REG32(NAADR)= NAADR_data;
    #endif

   //dprintf ("page=0x%x\n",page);
   //dprintf ("NAADR_data=0x%x\n",NAADR_data);
   //dprintf ("NAADR=0x%x\n",REG32(NAADR));
  
#endif  /*  */
    rtk_writel ((CECS0 | CMD_BLK_ERASE_C2), NACMR);
  check_ready_nand ();
  rtk_writel ((CECS0 | CMD_BLK_ERASE_C3), NACMR);
  check_ready_nand ();
  return;
}




#endif  /*  */

 #ifdef CONFIG_RTK_NAND_BBT
int nand_erase_nand (unsigned int addr, unsigned int len)
{

	int page, chipnr;
	int i, old_page, block;
	int elen = 0;
	int rc = 0;
	int realpage, chipnr_remap;
	int err_chipnr = 0, err_chipnr_remap = 1;
	int numchips=1, page_offset=0;

	{
	unsigned int real_addr;
	i = (addr >> block_shift);
	
	real_addr = (bbt_v2r[i].block_r << block_shift);//real block index, addr.
	addr = real_addr;
	//printf("%s: blockr:%x addr:%x pagemask%x \n\r",__FUNCTION__,bbt_v2r[i].block_r, addr,pagemask);
	}
	realpage = (int) (addr >> page_shift);
	chipnr = chipnr_remap = 0;
	old_page = page = realpage & pagemask;
	page_offset = page & (ppb-1);
	block = page/ppb;

	if ((addr + len) > chip_size) {
		printf("%s: Attempt erase beyond end of device add:%x len:%x cs:%x\n\r",__FUNCTION__,addr,len,chip_size);
		return FAIL;
	}
	
 	while (elen < len) {
		for ( i=0; i<RBA; i++){			
			if ( bbt[i].bad_block != BB_INIT ){
				if ( block == bbt[i].bad_block ){
					block = bbt[i].remap_block;
				}			
			}else				
			break;		
		}

		page = block*ppb;
		//printf("Ready to Erase blk %x\n\r",page/ppb);
		
		rc = rtk_erase_block_a(page);
			
		if (rc) {
		    printf("%s: block erase failed at page address=0x%08x\n", __FUNCTION__, addr);
			int block_remap = 0x12345678;
            /* update BBT */
			if(check_BBT(page/ppb)==0)
			{				
                for( i=0; i<RBA; i++){
	                if ( bbt[i].bad_block == BB_INIT && bbt[i].remap_block != RB_INIT){
		                err_chipnr = chipnr;
		                bbt[i].BB_die = err_chipnr;
		                bbt[i].bad_block = page/ppb;
		                err_chipnr_remap = bbt[i].RB_die;
		                block_remap = bbt[i].remap_block;
		                break;
	                }
                }

                if( block_remap == 0x12345678 ){
	                 printf("[%s] RBA do not have free remap block\n\r", __FUNCTION__);
	                 return FAIL;
                }

                dump_BBT();

                if ( rtk_update_bbt(bbt)){
	                printf("[%s] rtk_update_bbt() fails\n", __FUNCTION__);
	                return FAIL;
                }
			}
		
		     if(!NAND_ADDR_CYCLE)
		     #ifdef SWAP_2K_DATA
			     NfSpareBuf[BBI_SWAP_OFFSET] = 0x00;
			 #else
				NfSpareBuf[0] = 0x00;
			 #endif
		     else
			     NfSpareBuf[5] = 0x00;
		
		     if ( isLastPage){
				 unsigned char *temp_buf = (unsigned char *)malloc(page_size);
				 memset(temp_buf,0xba,sizeof(char)*page_size);
			     rtk_write_ecc_page_a(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
			     rtk_write_ecc_page_a(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);
	             if(temp_buf)
                     free(temp_buf);				 
		     }else{
				 unsigned char *temp_buf = (unsigned char *)malloc(page_size);
				 memset(temp_buf,0xba,sizeof(char)*page_size);
			     rtk_write_ecc_page_a(block*ppb,temp_buf ,&NfSpareBuf , page_size);
			     rtk_write_ecc_page_a(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
				 if(temp_buf)
					 free(temp_buf);

		     }
			 //erase the remapping block!!
			 rc = rtk_erase_block_a(block_remap*ppb);
			 
		}
		
			
		elen += block_size;

		old_page += ppb;
		
		if ( elen<len && !(old_page & pagemask)) {
			old_page &= pagemask;
		}

		block = old_page/ppb;
	}

	return rc;
}  
#endif
#if 1
void 
rtk_read_ecc_page (unsigned long flash_address, unsigned char *image_addr,
                   unsigned int image_size, char ecc_enable ) 
{
  //debug cl
  //dprintf("[%s]:%d for boot safe debug return directly\n",__func__,__LINE__);
  //return FAIL;
  flush_cache ();

    int page = flash_address / (page_size + oob_size);
	//if(page < 128||page > chip_size/page_size - 1)
	//{
	//	dprintf("[%s]:%d panic:fatal error *************,try to write in 0,1 block for boot,protect for debug\n",__func__,__LINE__);
	//	return FAIL;
	//}
  
    //default enable
    rtk_writel ((rtk_readl (NACR) | ECC_enable), NACR); //Enable ECC function
  if (ecc_enable == 0)
    
      //disable ECC function
      rtk_writel ((rtk_readl (NACR) & ~(ECC_enable)), NACR);
  
    //dprintf("\n\recc_enable=%d\n",ecc_enable);
    
    /* Clear NAND Flash Status Register (NASR , 0xb801a020)
     *   NECN write "0" to clear bit [7:4] and NRER write "1" to clear bit[3].
     */ 
    rtk_writel ((rtk_readl (NASR) & 0xFFFFFF0F), NASR);
  check_ready_nand ();
  
    //page_size = nand_device[nand_index].PageSize;
    //oob_size = nand_device[nand_index].OobSize;
    //ppb = nand_device[nand_index].BlockSize/nand_device[nand_index].PageSize;
  int dram_sa;
  int dma_counter = page_size >> 9;    //Move unit=512Byte
  int dma_size;
  int buf_pos = 0;
  int page_counter = 0;
  int page_num[3], page_shift = 0, page_start;
  unsigned long flash_addr_t = 0, flash_addr_t1;
  int dma_total_Rcounter = 0;
  
    //Page size alignment
    dma_size = image_size;
  
    //dprintf("\nNAND READ DMA,image size=0x%x,dma_times=0x%x\n",image_size,image_size/512);
#if 0
    if (image_size % page_size)
    {
      dma_size += (image_size % 2048);
    }
  
#else   /*  */
   if ((flash_address % (page_size + oob_size)) != 0)
    {
     dprintf ("\n\rflash_address must be page(0x%x+0x%x Bytes) aligned!\n",
               page_size, oob_size);
        
     return;
    }
  
#endif  /*  */
    
    /*Translate nand flash address formula */ 
#if 0                           //def NAND_Flash_Small_Page_32MB_3cycles
    flash_addr_t = flash_address;
  
#else   /*  */
    page_start = flash_address / (page_size + oob_size);
  for (page_shift = 0; page_shift < 3; page_shift++)
    {
      page_num[page_shift] = ((page_start >> (8 * page_shift)) & 0xff);
      flash_addr_t |= (page_num[page_shift] << (12 + 8 * page_shift));
    }
  
    #if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
	flash_addr_t=flash_addr_t>>3;
    #endif
    //Setting default value of flash_addr_t1
    flash_addr_t1 = flash_addr_t;
  
#endif  /*  */
    
    //DMA read
    //dprintf("\n\n(DMA Read)\r");
    while (dma_size > 0)
    {
      dma_counter = page_size >> 9;
      while (dma_counter > 0) //Move 1 page
        {
          if (dma_size <= 0x210)
            {
              
                //dprintf("\nFinal,NAND_dma_Rcounter_512Bytes=0x%x\n",dma_total_Rcounter);
                //dprintf("\nTotal read size=0x%x\n",dma_total_Rcounter*0x210); //0x210 include spare space size(OOB+16)
                //break;
            }
          
          else                  //JSW 20110731: Add for Roger NAND R/W throuhput measurement
            {
              dma_total_Rcounter++;
              
                //dprintf("\nNAND_dma_Rcounter_512Bytes=0x%x\n",dma_total_Rcounter);
            }
          check_ready_nand ();
          
            //set DMA RAM start address
            dram_sa = ((unsigned int) image_addr + buf_pos * 512) & (~M_mask);
          rtk_writel (dram_sa, NADRSAR);
          
             //dprintf("\nDMA-R:SDRAM address: 0x%X\n ",(dram_sa));
            
            //set DMA flash start address
            rtk_writel (flash_addr_t, NADFSAR);
          
             //dprintf("\nFlash Laddress: 0x%X\n\n\r",(flash_addr_t));
            
            //DMA read
            //rtk_writel((~TAG_DIS) & (DESC0|DMARE|LBC_128),NADCRR);       //enable tag access
            //disable tag access
            rtk_writel ((TAG_DIS | DESC0 | DMARE | LBC_128), NADCRR);
            check_ready_nand ();
          
            /*Show ECC un-recovery message
             *Note: If spare space is not programmed by ECC-hareware circuit , enable this will cause NECN and NRER happen
             */ 
#if 0
            if ((rtk_readl (NASR) & 0xf0) >> 4) //if (NECN[7:4])==4~1 ,means NECN happen
            {
              
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
                dprintf 
                ("\nGet NECN errors,DMA count=%d,page=%x,addr=0x%x~0x%x",
                 buf_pos, buf_pos, (buf_pos) * 528, ((buf_pos) + 1) * 528);
              dprintf ("\nNASR's NECN ECC count=%d",
                        (rtk_readl (NASR) & 0xf0) >> 4);
              dprintf ("\nNADRSAR address: 0x%X ", (dram_sa));
              dprintf ("\nNADFSAR: 0x%X\n\n\r", (flash_addr_t - 512));
              
#else   /*  */
                dprintf 
                ("\nGet NECN errors,DMA count=%d,page=%x,addr=0x%x~0x%x",
                 buf_pos, (buf_pos / 4), (buf_pos / 4) * 2112,
                 ((buf_pos / 4) + 1) * 2112);
              dprintf ("\nNASR's NECN ECC count=%d",
                        (rtk_readl (NASR) & 0xf0) >> 4);
              dprintf ("\nNADRSAR: 0x%X ", (dram_sa));
              dprintf ("\nNADFSAR: 0x%X\n\n\r", (flash_addr_t - 528));
              
#endif  /*  */
                // __delay(1000 * 1000 * 10);
            }
          if (rtk_readl (NASR) & 0x8) //if NRER=1 means ECC happen un-recovery errors
            {
              dprintf ("\nNASR(0x%x): 0x%X\n ", NASR, rtk_readl (NASR));
              
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
                dprintf 
                ("\nGet NRER errors,DMA count=%d,page=%x,Paddr=0x%x~0x%x",
                 buf_pos, buf_pos, buf_pos * 528, ((buf_pos) + 1) * 528);
              dprintf ("\nNADRSAR: 0x%X ", (dram_sa));
              dprintf ("\nNADFSAR: 0x%X\n\n\r", (flash_addr_t - 512));
              
#else   /*  */
                dprintf 
                ("\nGet NRER errors,DMA count=%d,page=%x,Paddr=0x%x~0x%x",
                 buf_pos, (buf_pos / 4), (buf_pos / 4) * 2112,
                 ((buf_pos / 4) + 1) * 2112);
              dprintf ("\nNADRSAR: 0x%X ", (dram_sa));
              dprintf ("\nNADFSAR: 0x%X\n\n\r", (flash_addr_t - 528));
              
#endif  /*  */
                // __delay(1000 * 1000);
                
                //NRER write "1" to clear bit[3] and NECN write "0" to clear bit [7:4]
                // rtk_writel((rtk_readl(NASR) & 0xFFFFFF0F), NASR);
            }
          
#endif  /*  */
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
            flash_addr_t += 512;
          
#else   /*  */
            flash_addr_t += 528;
          
#endif  /*  */
            dma_counter--;
          buf_pos++;
          dma_size -= 512;
          flush_cache ();
        }
      page_counter += 1;
      
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
        flash_addr_t1 = (page_counter * 0x200) + (page_start * 0x200);
      
#else   /*  */
        flash_addr_t1 = (page_counter * 0x1000) + (page_start * 0x1000);
           
#endif  /*  */
	 flash_addr_t = flash_addr_t1;
    }
  flush_cache ();
}


#endif  /*  */
  
#if 1

void 
rtk_write_ecc_page (unsigned long flash_address, unsigned char *image_addr,
                    unsigned int image_size ) 
{
  //debug cl
  //dprintf("[%s]:%d for boot safe debug return directly\n",__func__,__LINE__);
  //return FAIL;
  //end debug cl
  flush_cache ();


    int page = flash_address / (page_size + oob_size);
	//if(page < 128||page > chip_size/page_size - 1)
	if(page > chip_size/page_size - 1)
	{
		dprintf("[%s]:%d panic:fatal error *************,try to write in 0,1 block for boot,protect for debug\n",__func__,__LINE__);
		return FAIL;
	}
  
    //page_size = nand_device[nand_index].PageSize;
    //oob_size = nand_device[nand_index].OobSize;
    //ppb = nand_device[nand_index].BlockSize/nand_device[nand_index].PageSize;
  int dram_sa, oob_sa;
  int dma_counter = page_size >> 9;    //Move unit=512Byte
  int i = 0;
  int dma_size;
  int buf_pos = 0;
  int page_counter = 0;
  int page_num[3], page_shift = 0, page_start;
  unsigned long flash_addr_t = 0, flash_addr_t1;
  char *oob_buf;
  int dma_total_Wcounter = 0;
  
  if ((flash_address % (page_size + oob_size)) != 0)
    {
      dprintf ("\n\rflash_address must be page(0x%x+0x%x Bytes) aligned!\n",
                page_size, oob_size);
      return;
    }
  
  oob_buf = malloc (oob_size);
  if (!oob_buf)
    {
      dprintf ("allocate fail!!\n\r");
      return;
    }
  oob_sa = ((unsigned int) oob_buf) & (~M_mask);
  
    //Page size alignment
    dma_size = image_size;
//  dprintf ("\nNAND WRITE DMA,image size=0x%x,dma_times=0x%x\n", image_size,image_size / 512);
  
#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
    if (image_size % page_size)
    {
      dma_size += (image_size % 2048);
    }
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles
    if (image_size % page_size)
    {
      dma_size += (image_size % 2048);
    }
  
#endif  /*  */
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
    if (image_size % page_size)
    {
      dma_size += (image_size % 512);
    }
  
#endif  /*  */

    
    /*Translate nand flash address formula */ 
#if 0                           //def NAND_Flash_Small_Page_32MB_3cycles
    flash_addr_t = flash_address;
  
#else   /*  */
    page_start = flash_address / (page_size + oob_size);
  for (page_shift = 0; page_shift < 3; page_shift++)
    {
      page_num[page_shift] = ((page_start >> (8 * page_shift)) & 0xff);
      flash_addr_t |= (page_num[page_shift] << (12 + 8 * page_shift));
    }
  
     #if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))

	 flash_addr_t=flash_addr_t>>3;
    #endif
    //Setting default value of flash_addr_t1
    flash_addr_t1 = flash_addr_t;
  
#endif  /*  */
    
#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
    
    //Erase NAND
  unsigned int NAND_Blk_ecnt;
  unsigned int NAND_Blk_ecnt_start = flash_address / (2112 * 64);
  for (NAND_Blk_ecnt = NAND_Blk_ecnt_start;
        NAND_Blk_ecnt <= (NAND_Blk_ecnt_start + (image_size / (2048 * 64)));
        NAND_Blk_ecnt++)
    {
      rtk_erase_block (NAND_Blk_ecnt * 64);    //JSW 1block=64=0x40 pages , it needs input page count
      
        //dprintf("Erase NAND Flash page=%d ~ %d\n\r",page,((page+63)));
        //dprintf("Erase NAND Flash Size=0x%x ~ 0x%x\n\n\r",(2112)*page,((2112)*(page+63))-1);
    }
  
#endif  /*  */
#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles
    
    //Erase NAND
  unsigned int NAND_Blk_ecnt;
  unsigned int NAND_Blk_ecnt_start = flash_address / (2112 * 64);
  for (NAND_Blk_ecnt = NAND_Blk_ecnt_start;
        NAND_Blk_ecnt <= (NAND_Blk_ecnt_start + (image_size / (2048 * 64)));
        NAND_Blk_ecnt++)
    {
      rtk_erase_block (NAND_Blk_ecnt * 64);     //JSW:Large page's block=64=0x40 pages , it needs input page count
      
        //dprintf("Erase NAND Flash page=%d ~ %d\n\r",page,((page+63)));
        //dprintf("Erase NAND Flash Size=0x%x ~ 0x%x\n\n\r",(2112)*page,((2112)*(page+63))-1);
    }
  
#endif  /*  */
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
    
    //Erase NAND
  unsigned int NAND_Blk_ecnt;
  unsigned int NAND_Blk_ecnt_start = flash_address / (528 * 32);
  for (NAND_Blk_ecnt = NAND_Blk_ecnt_start;
        NAND_Blk_ecnt <= (NAND_Blk_ecnt_start + (image_size / (512 * 32)));
        NAND_Blk_ecnt++)
    {
      rtk_erase_block (NAND_Blk_ecnt * 32);    //JSW:Small page's block=32=0x20 pages , it needs input page count
      
        //dprintf("Erase NAND Flash page=%d ~ %d\n\r",page,((page+63)));
        //dprintf("Erase NAND Flash Size=0x%x ~ 0x%x\n\n\r",(2112)*page,((2112)*(page+63))-1);
    }
  
#endif  /*  */


    while (dma_size > 0)
    {
      dma_counter = page_size >> 9;
      while (dma_counter > 0) //Move 1 page
        {
          
            //prom_printf("\n DMA(512 Bytes) Start, DMA total count=%d\n\n\r",buf_pos);
            check_ready_nand ();
          rtk_writel ((rtk_readl (NACR) | ECC_enable), NACR);
          
            //set DMA RAM start address
            dram_sa = ((unsigned int) image_addr + buf_pos * 512) & (~M_mask);
          rtk_writel (dram_sa, NADRSAR);
          
           //dprintf("\nDMA-W:SDRAM address: 0x%X\n ",(dram_sa));
            //set DMA flash start address
            //flash_addr_t &= 0x1fffffff;
          rtk_writel (flash_addr_t, NADFSAR);
          
            //dprintf("\nFlash Laddress: 0x%X\n\n\r",(flash_addr_t));
            //set OOB address
            //rtk_writel(oob_sa, NADTSAR);
            //DMA write
            rtk_writel ((~TAG_DIS) &
                        ((0 << TAG_SEL) | DESC0 | DMAWE | LBC_128), NADCRR);
          check_ready_nand ();
          
#if 0//def CONFIG_SHOW_NAND_DBG_MSG
            if (rtk_readl (NASR) & 0x4) //if NWER=1 means ECC code can't write into spare space
            {
              dprintf 
                ("\nGet NWER errors,DMA count=%d,page=%x,addr=0x%x~0x%x\n\r",
                 buf_pos, (buf_pos / 4), (buf_pos / 4) * 2112,
                 ((buf_pos / 4) + 1) * 2112);
              dprintf ("\nSDRAM address: 0x%X\n ", (dram_sa));
              dprintf ("\nFlash address: 0x%X\n\n\r", (flash_addr_t));
              
                //__delay(1000 * 1000);
                
                //NWER write "1" to clear
                rtk_writel ((rtk_readl (NASR) | NWER), NASR);
            }
          if (dma_size <= 512)
            {
              dprintf ("\nFinal,NAND_dma_Wcounter_512Bytes=0x%x\n",
                        dma_total_Wcounter);
              
                //0x210 include spare space size(OOB+16)
                dprintf ("\nTotal write size=0x%x\n",
                         dma_total_Wcounter * 0x210);
              
                //break;
            }
          
          else                  //JSW 20110731: Add for Roger NAND R/W throuhput measurement
            {
              dma_total_Wcounter++;
              
                //dprintf("\nNAND_dma_Wcounter_512Bytes=0x%x\n",dma_total_Wcounter);
            }
          
#endif  /*  */
            
        

            
         

#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
            flash_addr_t += 512;
          
#else   /*  */
            flash_addr_t += 528;
          
#endif  /*  */
            dma_counter--;
          buf_pos++;
          dma_size -= 512;
        }
      page_counter += 1;
      
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
          flash_addr_t1 = (page_counter * 0x200) + (page_start * 0x200);     
#else   /*  */

        flash_addr_t1 = (page_counter * 0x1000) + (page_start * 0x1000);
           
#endif  /*  */
	 flash_addr_t = flash_addr_t1;
    }
  flush_cache ();

 
}
 #ifdef CONFIG_RTK_NAND_BBT
/*
 * from: flash address offset
 * len:  read len from flash
 * data_buf:DRAM space for data
 * oob_buff:DRAM space for oob
 *
 * */
int nand_read_ecc_ob (unsigned int from, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf)
{
    //dprintf("[%s]:%d\n",__func__,__LINE__);
	unsigned int page, realpage;
	int data_len, oob_len;
	int rc=0;
	int i, old_page, page_offset, block;
	int chipnr, chipnr_remap;
	int numchips=1;
{
    unsigned int offset=0, aa=0;	
	i = (from >> block_shift); //virtual block index
	aa = from & ~(block_size -1);
	offset = from - aa;
	from =  (bbt_v2r[i].block_r << block_shift) + offset;//real block index, addr.
//printf("%s: blockr:%x from:%x offset %x len:%x\n\r",__FUNCTION__,bbt_v2r[i].block_r, from, offset,len);
}
	if ((from + len) > chip_size) {
		printf ("nand_read_ecc: Attempt read beyond end of device\n");
		return FAIL;
	}

	if ((from & (page_size-1)) ||(len & (page_size-1))) {
		printf("nand_read_ecc: Attempt to read not page aligned data\n");
		return FAIL;
	}

    //dprintf("[%s]:%d page_shift = %d\n",__func__,__LINE__,page_shift );
    //dprintf("[%s]:%d pagemask = 0x%x\n",__func__,__LINE__,pagemask);
	realpage = (int)(from >> page_shift);
	//chipnr = chipnr_remap = (int)(from >> this->chip_shift);
	old_page = page = realpage & pagemask;
	page_offset = page & (ppb-1);
	block = page/ppb;
	//printf("realpage:%x old_page:%x  page_offset:%x\r\n",realpage,old_page,page_offset);

	if (numchips==1 && block != read_block ){
		read_block = block;
		read_remap_block = 0xFFFFFFFF;
		read_has_check_bbt = 0;
	}

	data_len = oob_len = 0;

	while(data_len<len){
		if( numchips==1){
			if ( (page>=block*ppb) && (page<(block+1)*ppb) && read_has_check_bbt==1 )
				goto SKIP_BBT_CHECK;
		}

		for ( i=0; i<RBA; i++){
			if ( bbt[i].bad_block != BB_INIT ){
				if ( block == bbt[i].bad_block ){
					read_remap_block = block = bbt[i].remap_block;
				}
			}else
				break;
		}
		read_has_check_bbt = 1;

SKIP_BBT_CHECK:

		if (  read_has_check_bbt==1 ){
			if ( read_remap_block == 0xFFFFFFFF )
				page = block*ppb + page_offset;
			else	
				page = read_remap_block*ppb + page_offset;
		}else
			page = block*ppb + page_offset;  

		//if((page % ppb)==0)  printf("$");
	
		rc = rtk_read_ecc_page_a(page, &data_buf[data_len], &oob_buf[oob_len], page_size);
		//dprintf("[%s]:%d rc = %d\n",__func__,__LINE__,rc);
		//while(1); //for safe sake

		if (rc < 0) {
		    //dprintf("[%s]:%d for safe sake,how can read get good block to bad block now cancel\n",__func__,__LINE__);
		    //while(1); //for safe sake,how can read get good block to bad block
			if(rc==-1){
				//printf("%s: page %d Un-correctable HW ECC\n\r", __FUNCTION__, page);
				//update BBT
				if(check_BBT(page/ppb)==0){
				    for( i=0; i<RBA; i++){
					    if ( bbt[i].bad_block == BB_INIT && bbt[i].remap_block != RB_INIT){
						    bbt[i].BB_die = numchips-1;
						    bbt[i].bad_block = page/ppb;
						    break;
					    }
				    }
					dump_BBT();
					
					//if ( rtk_update_bbt ( &NfDataBuf, &NfSpareBuf, bbt) )
					if ( rtk_update_bbt (bbt) ){
							printf("[%s] rtk_update_bbt() fails\n\r", __FUNCTION__);
							return -1;
					}

			    }//check_BBT
			    
			    if(!NAND_ADDR_CYCLE)
			    #ifdef SWAP_2K_DATA
			        NfSpareBuf[BBI_SWAP_OFFSET] = 0x00;
			    #else
					NfSpareBuf[0] = 0x00;
			    #endif
			    else
				    NfSpareBuf[5] = 0x00;

				block = page/ppb;

			    if ( isLastPage){
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					rtk_erase_block_a(block*ppb);
				    rtk_write_ecc_page_a(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
				    rtk_write_ecc_page_a(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);
					if(temp_buf)
					    free(temp_buf);
			    }else{
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					rtk_erase_block_a(block*ppb);
				    rtk_write_ecc_page_a(block*ppb,temp_buf ,&NfSpareBuf , page_size);
				    rtk_write_ecc_page_a(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
					if(temp_buf)
					    free(temp_buf);
			    }
			    printf("%s: Un-correctable HW ECC Error at page=%d\n\r",__FUNCTION__, page);
				rc = 0;
		    }else{
				printf("%s: page %d failed\n", __FUNCTION__, page);
				return -1;
			}
		}
		if(data_buf)//add by alexchang 0524-2010
		data_len += page_size;

		if(oob_buf)//add by alexchang 0524-2010
		oob_len += oob_size;
		
		old_page++;
		page_offset = old_page & (ppb-1);
		if ( data_len<len && !(old_page &  pagemask)) {
			old_page &= pagemask;
		}
		
		block = old_page/ppb;
		//printf("$$$ block:%x pagemask:%x len:%x old_page:%x data_len:%x\r\n",block,pagemask,len,old_page,data_len);

	}
    //while(1); //debug cl for safe sake
	//printf("out rx:%x\r\n",rc);
	return rc;

}
int nand_write_ecc_ob (unsigned int to, unsigned int len, unsigned char *data_buf, unsigned char *oob_buf)
{
	unsigned int page, realpage;
	int data_len, oob_len;
	int rc;
	int i, old_page, page_offset, block;
	int chipnr, chipnr_remap, err_chipnr = 0, err_chipnr_remap = 1;
	int numchips=1;
	int backup_offset;
	unsigned int rsv_block = 0;

	//printf("%s-%d: to %d, len %d oob:%x\n\r",__FUNCTION__,__LINE__,to,len,oob_buf);
{
        unsigned int offset=0, aa=0;
		i = (to >> block_shift);//virtual block index
        aa = to & ~(block_size - 1);
        offset = to - aa;		
		to = (bbt_v2r[i].block_r << block_shift) + offset;//real block index, addr.
		//printf("%s: blockr:%x to:%x offset %x\n\r",__FUNCTION__,bbt_v2r[i].block_r, to, offset);
}

	if ((to + len) > chip_size) {
		printf("nand_write_ecc: Attempt write beyond end of device\n\r");
		return FAIL;
	}

	if ((to & (page_size-1)) ||(len & (page_size-1))) {
		printf("nand_write_ecc: Attempt to write not page aligned data, to = 0x%08X, len = %d\n",to,len);
		return FAIL;
	}

	realpage = (int)(to >> page_shift);
	chipnr = chipnr_remap = 0;
	old_page = page = realpage & pagemask;
	page_offset = page & (ppb-1);
	block = page/ppb;

	//printf("[%s] wirte page %x, len %x, data_buf = 0x%x page_offset=%x, block=%x,page_shift=%x\n\r",__func__, realpage, len , data_buf,page_offset,block,page_shift);
//ccwei 111116
//	rsv_block = RESERVED_AREA/block_size;


	
	if ( numchips == 1 && block != write_block ){
		//printf("@@@\r\n");
		write_block = block;
		write_remap_block = 0xFFFFFFFF;
		write_has_check_bbt = 0;
	}
	
	data_len = oob_len = 0;

	//dprintf("----------page: %x-->%x\r\n",page,&data_buf[data_len]);
	while ( data_len < len) {

/*
		if(block < (rsv_block-2)){
			//printf("[%s] wirte page %x, len %x, data_buf = 0x%p\n\r",__func__, realpage, len , data_buf);
			goto SKIP_BBT_CHECK;
		}
*/		
		if (numchips == 1){
			if ( (page>=block*ppb) && (page<(block+1)*ppb) && write_has_check_bbt==1 )
				goto SKIP_BBT_CHECK;
		}

		for ( i=0; i<RBA; i++){
			if ( bbt[i].bad_block != BB_INIT ){
				if ( block == bbt[i].bad_block ){
						write_remap_block = block = bbt[i].remap_block;
				}
			}else
				break;
		}
		write_has_check_bbt = 1;
SKIP_BBT_CHECK:
		if ( numchips == 1 && write_has_check_bbt==1 ){
				if ( write_remap_block == 0xFFFFFFFF )
					page = block*ppb + page_offset;
				else	
					page = write_remap_block*ppb + page_offset;
				
		}else
			{
				page = block*ppb + page_offset;
			}

		//if( (page % ppb) == 0) printf(".");
		//printf("page: %x-->%x\r\n",page,&data_buf[data_len]);

		//dprintf("page: %x-->%x\r\n",page,&data_buf[data_len]);
		rc = rtk_write_ecc_page_a ( page, &data_buf[data_len], &oob_buf[oob_len], page_size);

		if(rc<0){
/*			
			if(block<(rsv_block-2)){
				printf("[%s] wirte page %x, len %x, data_buf = 0x%p\n\r",__func__, realpage, len , data_buf);
				return -1;		
			}
*/			
			if(rc == -1){
				printf ("%s: write_ecc_page:  write failed\n\r", __FUNCTION__);
				int block_remap = 0x12345678;
				/* update BBT */
			    if(check_BBT(page/ppb)==0)
			    {				
				    for( i=0; i<RBA; i++){
					    if ( bbt[i].bad_block == BB_INIT && bbt[i].remap_block != RB_INIT){
						    err_chipnr = chipnr;
						    bbt[i].BB_die = err_chipnr;
						    bbt[i].bad_block = page/ppb;
						    err_chipnr_remap = bbt[i].RB_die;
						    block_remap = bbt[i].remap_block;
						    break;
					    }
				    }

				if ( block_remap == 0x12345678 ){
					printf("[%s] RBA do not have free remap block\n\r", __FUNCTION__);
					return FAIL;
				}
			
				dump_BBT();

				    if ( rtk_update_bbt(bbt)){
					    printf("[%s] rtk_update_bbt() fails\n", __FUNCTION__);
					    return FAIL;
				    }
			    }

				backup_offset = page&(ppb-1);
				rtk_erase_block_a(block_remap*ppb);
				//printf("[%s] Start to Backup old_page from %d to %d\n\r", __FUNCTION__, block*ppb, block*ppb+backup_offset-1);

				for ( i=0; i<backup_offset; i++){
					rtk_read_ecc_page_a(block*ppb+i ,&NfDataBuf ,&NfSpareBuf ,page_size);
#if 0
					if ( NfSpareBuf )
						reverse_to_Yaffs2Tags(&NfSpareBuf); //czyao
#endif
					if(!NAND_ADDR_CYCLE)
					#ifdef SWAP_2K_DATA
						NfSpareBuf[BBI_SWAP_OFFSET] = 0xff;
					#else
						NfSpareBuf[0] = 0xff;
					#endif
					else
						NfSpareBuf[5] = 0xff;
					rtk_write_ecc_page_a(block_remap*ppb+i ,&NfDataBuf ,&NfSpareBuf ,page_size);
				}
				//Write the written failed page to new block
				rtk_write_ecc_page_a ( block_remap*ppb+backup_offset, &data_buf[data_len], &oob_buf[oob_len], page_size);
				//printf("[%s] write failure page = %d to %d\n", __FUNCTION__, page, block_remap*ppb+backup_offset);

				if(!NAND_ADDR_CYCLE)
				#ifdef SWAP_2K_DATA
					NfSpareBuf[BBI_SWAP_OFFSET] = 0x00;
				#else
					NfSpareBuf[0] = 0x00;
				#endif
				else
					NfSpareBuf[5] = 0x00;

				block = page/ppb;

				if ( isLastPage){
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					rtk_erase_block_a(block*ppb);
					rtk_write_ecc_page_a(block*ppb+ppb-1,temp_buf ,&NfSpareBuf , page_size);
					rtk_write_ecc_page_a(block*ppb+ppb-2,temp_buf ,&NfSpareBuf , page_size);
                    if(temp_buf)
 					    free(temp_buf);
				}else{
					unsigned char *temp_buf = (unsigned char *)malloc(page_size);
					memset(temp_buf,0xba,sizeof(char)*page_size);
					rtk_erase_block_a(block*ppb);
					rtk_write_ecc_page_a(block*ppb,temp_buf ,&NfSpareBuf , page_size);
					rtk_write_ecc_page_a(block*ppb+1,temp_buf ,&NfSpareBuf , page_size);
                    if(temp_buf)
 					    free(temp_buf);
				}
			}else{
				//printf("%s: write_ecc_page:  rc=%d\n\r", __FUNCTION__, rc);
				return -1;
			}

		}

		if(data_buf)//add by alexchang 0524-2010
			data_len += page_size;
		if(oob_buf) //add by alexchang 0524-2010
			oob_len += oob_size;
		
		old_page++;
		page_offset = old_page & (ppb-1);
		//printf("page_offset:%x old_page:%x  ppb:%x \r\n",page_offset,old_page,ppb);
		if ( data_len<len && !(old_page & pagemask)) {
			old_page &= pagemask;
		}
		block = old_page/ppb;

	}

	return 0;

}
#endif
#endif  /*  */

int
rtk_nand_strapin_setting(void){
#if 1
  /*Set PIN_MUX to NAND Flash pin and disable P0 GMII
     PIN_MUX_SEL_REG(0xb8000040)-[13:12][9:8]=[01][01]
   */

  /*Enable NAND Flash IP clock */
#define REG32(reg)  (*(volatile unsigned int *)(reg))
  //REG32(clk_manage_REG) |= 1 << 28;

REG32(clk_manage_REG) |= ((1 << 28)| (3<<12)| (7<<18));

/*=============Pre-setting for NAND Flash mode===============================*/

 
//dprintf        ("\nrtk_nand.c:Set PIN_MUX to NAND Flash pin (Todo:Disable P0 GMII)\n");

#define PINMUX_SEL_1 0xb8000100
#define PINMUX_SEL_2 0xb8000104
#define PINMUX_SEL_3 0xb8000108

  REG32 (PINMUX_SEL_1) |=((1<<26)  | (1<<24)  );  //set MFCS1# to NAND CS0#
  //REG32 (PINMUX_SEL_2) |=((1<<23) | (1<<25) );  //set UART0_CTS # to NAND MD[7] ,  UART0_RTS to NF_CE1#
  REG32 (PINMUX_SEL_2) |=((1<<23)  );  //set UART0_CTS # to NAND MD[7]
  REG32 (PINMUX_SEL_3) |=((1<<15) | (1<<18) | (1<<21) | (1<<24) | (1<<27)  | (1<<30) );  //set Port0 RGMII I/F to NAND

/*=============NAND flash AC/RC setting reference===============================*/
/*
NAFC_AC[1:0]
Address Cycle
00: 3 address cycle
01: 4 address cycle
10: 5 address cycle
11: reserved

NAFC_RC[1:0]
NAND flash page read command
00: 1 cycle command{00h} (512Byte per page)
01: 2 cycle command{00h, 30h} (2048Byte per page)
10: 2 cycle command{00h, 30h} (4096Byte per page)
11: 2 cycle command{00h, 30h} (8192Byte per page)

HW_Strap(0xb8000008)
[29:28]=RC[1:0]
[27:26]=AC[1:0]

"NAND_Flash_Large_Page_5cycles_Pages8KB_8GB CONFIG_NAND_Flash_Large_Page_5cycles_Pages8KB_8GB \
					 NAND_Flash_Large_Page_5cycles_Pages8KB_4GB CONFIG_NAND_Flash_Large_Page_5cycles_Pages8KB_4GB \
					 NAND_Flash_Large_Page_5cycles_Pages4KB CONFIG_NAND_Flash_Large_Page_5cycles_Pages4KB \
					 NAND_Flash_Large_Page_256MBto2GB_5cycles CONFIG_NAND_Flash_Large_Page_256MBto1GB_5cycles \
					 NAND_Flash_Large_Page_128MB_4cycles CONFIG_NAND_Flash_Large_Page_128MB_4cycles \
					 NAND_Flash_Small_Page_64MB_4cycles CONFIG_NAND_Flash_Small_Page_64MB_4cycles \
					 NAND_Flash_Small_Page_32MB_3cycles CONFIG_NAND_Flash_Small_Page_32MB_3cycles \
				      " NAND_Flash_Large_Page_256MBto2GB_5cycles
fi
*/


/*=============NAND flash AC/RC setting to hw_strap(0xb8000008[29:26]) ===============================*/
#define HW_Strap_REG 0xb8000008
#define RC_Pages8KB  (3<<28)
#define RC_Pages4KB (2<<28)
#define RC_Pages2KB (1<<28)
#define RC_Pages512B  (0<<28)

#define AC_5cycles  (2<<26)
#define AC_4cycles  (1<<26)
#define AC_3cycles (0<<26)

#define ECC_12T (1<<31)

REG32 (HW_Strap_REG) &= ~(0xf <<26);

#ifdef CONFIG_NAND_Flash_Large_Page_5cycles_Pages8KB_8GB 
REG32 (HW_Strap_REG) |=( RC_Pages8KB |AC_5cycles ); 
#endif

#ifdef CONFIG_NAND_Flash_Large_Page_5cycles_Pages8KB_4GB 
REG32 (HW_Strap_REG) |=( RC_Pages8KB |AC_5cycles ); 
#endif

#define NAND_12T_ECC_538_BYTE 0
#ifdef CONFIG_NAND_Flash_Large_Page_5cycles_Pages4KB 
	#if NAND_12T_ECC_538_BYTE
		REG32 (HW_Strap_REG) |=( RC_Pages4KB |AC_5cycles|ECC_12T); 
	#else
		REG32 (HW_Strap_REG) |=( RC_Pages4KB |AC_5cycles ); 
	#endif
#endif

#ifdef CONFIG_NAND_Flash_Large_Page_256MBto1GB_5cycles 
REG32 (HW_Strap_REG) |=( RC_Pages2KB |AC_5cycles ); 
#endif

#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles 
REG32 (HW_Strap_REG) |=( RC_Pages2KB |AC_4cycles ); 
#endif

// please check
#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
REG32 (HW_Strap_REG) |=( RC_Pages2KB |AC_5cycles ); //need check
#endif

#ifdef CONFIG_NAND_Flash_Small_Page_64MB_4cycles 
#error //no support
REG32 (HW_Strap_REG) |=( RC_Pages2KB |AC_4cycles ); 
#endif

#ifdef CONFIG_NAND_Flash_Small_Page_32MB_3cycles 
REG32 (HW_Strap_REG) |=( RC_Pages512B |AC_3cycles ); 
#endif


#define LX_control_REG 0xb8000014
 // printf("\nTurn on Bus TimeOut Arbiter\n");

  //REG32(LX_control_REG) |= (1 << 15);
  //dprintf("\nPINMUX_REG_8196D_97D(%x)=0x%x \n",PINMUX_REG_8196D_97D,REG32(PINMUX_REG_8196D_97D));
#endif /*  */
}

int
rtk_nand_probe(void
)
{

rtk_nand_strapin_setting();
rtk_nand_strapin_setting();
rtk_PIO_read_page(0,0,0);
rtk_nand_strapin_setting();


  int id_chain = rtk_nand_read_id();
  int i;


#if 0
  for (i = 0; i < 5; i++) {
    dprintf("%x ", id[i]);
  }
#endif /*  */

  //int i=0;
  //dprintf("[%s:] %d id_chain = 0x%X\n",__func__,__LINE__,id_chain);
  memset(&nand_info, 0, sizeof(struct device_type));
  unsigned int ui, uiCount;

  //unsigned char pucBuffer[4];
  uiCount = sizeof(nand_device) / sizeof(struct device_type);
  dprintf("\n\nScanning NAND registered database ...  ");
  for (ui = 0; ui < uiCount; ui++) {
    //dprintf("\n(Total:%d , Scan = %d)", uiCount, ui + 1);
    if ((nand_device[ui].id) == (id_chain)) {

#if 0
      dprintf("\nFound registered NAND Flash, Info as below:\n");
      dprintf
          ("chip_name=%s,id=0x%x; chip_size=0x%x; page_size=0x%x; block_size=0x%x;\n",
           nand_device[ui].name, nand_device[ui].id,
           nand_device[ui].chipsize, nand_device[ui].PageSize,
           nand_device[ui].BlockSize);

#else /*  */
      dprintf("\n=> Found registed NAND Flash, info as below:\n");
      dprintf("1.Chip_name= %s\n", nand_device[ui].name);
      dprintf("2.ID= 0x%x\n", nand_device[ui].id);
      dprintf("3.Chip_size= %d MB\n", nand_device[ui].size);
      dprintf("4.Block_cnt= %d \n", nand_device[ui].BlockCnt);
      dprintf("5.Block_size= %d KB\n", nand_device[ui].BlockSize / 0x400);
      dprintf("6.Page_size= %d Bytes\n", nand_device[ui].PageSize);
      dprintf("7.OobSize= %d KB\n", nand_device[ui].OobSize);

	  /*debug cl add from patch, global va init*/
	  nand_select=ui;
	  block_size= nand_device[ui].BlockSize;
	  chip_size=nand_device[ui].chipsize;
	  block_shift = shift_value(nand_device[ui].BlockSize) -1 ;
	  page_shift = shift_value(nand_device[ui].PageSize) -1;
	  isLastPage = nand_device[ui].isLastPage;
	  pagemask = (nand_device[ui].chipsize/nand_device[ui].PageSize) -1;
#endif /*  */
      break;
    }

    else {
      if ((ui + 1) == uiCount)
	  {
        dprintf("\nNo registered NAND Flash found!\n");
	    while(1);//debug cl not go on for debug
	  }
    }
  }

  /*JSW: Set NAND Flash Control Register
   *ECC=1, RBO=0 ,WBO=0
   *Safe Timing Parameter
   * rtk_writel(0xc00fffff, NACR);
   */
  //JSW 20110814:Safe parameter OK

  #if 0
  rtk_writel(ECC_enable | CE_TWP(15) | CE_TWH(15) | CE_TRR(15) |
             CE_TH(15) | CE_TS(15), NACR);
#else
  //JSW 20110814:Aggressive parameter (based on K9F2G08R0A and LX 200MHZ=5ns)
 // rtk_writel(ECC_enable | CE_TWP(4) | CE_TWH(4) | CE_TRR(4) | CE_TH(7) | CE_TS(5), NACR);


 //JSW 20140227:Aggressive parameter (based on K9F2G08R0A and LX 200MHZ=5ns)
  //rtk_writel(ECC_enable | CE_TWP(5) | CE_TWH(5) | CE_TRR(5) | CE_TH(7) | CE_TS(5), NACR);//less error
  //rtk_writel(ECC_enable | CE_TWP(15) | CE_TWH(15) | CE_TRR(15) | CE_TH(15) | CE_TS(1), NACR); //more error
  //rtk_writel(ECC_enable | CE_TWP(3) | CE_TWH(3) | CE_TRR(3) | CE_TH(3) | CE_TS(3), NACR);//more error
   rtk_writel(ECC_enable | CE_TWP(4) | CE_TWH(4) | CE_TRR(5) | CE_TH(5) | CE_TS(3), NACR);//Aggressive parameter
#endif


//  dprintf("\nAfter NAND probed,set safe parameter NACR=0x%X",rtk_readl(NACR));
//    dprintf("\nNACFR=0x%X",rtk_readl(NACFR));
//  dprintf("\n========================================= \n");
//   dprintf("\nhw_srtap=0x%X\n",rtk_readl(0xb8000008));
   //debug cl
//   dprintf("\nNASR=0x%X\n",rtk_readl(NASR)); //should be clear ? to check
   //debug cl
  rtk_writel(0x0000000f, NASR); //clear NAND flash status register
//   dprintf("\nNASR=0x%X\n",rtk_readl(NASR)); //should be clear ? to check
 // dprintf("\n========================================= \n");
  //rtk_writel(0x0000000f, NASR); //clear NAND flash status register

  /*Bad block scan
   * Method1: Samsung/AMD , 6th byte of  Page1 or 2's spare space is 0xff =>good block
   *                MXIC /Hynix , 1th byte of  Page1 or 2's spare space is 0xff =>good block
   *                Otherwise: bad block
   *
   *Method2: As default , page's() content should be all 0xff=>good block
   *               Otherwise: bad block
   *               =>So we can use DMA read(without ECC) to check it
   */

#if 0
  memcpy(&nand_info, &nand_device[nand_index], sizeof(struct device_type));
  prom_printf
      ("nand part=%s, id=%x, device_size=0x%, chip_size=0x%, num_chips=%d , isLastPage=%d \n\r",
       nand_info.name, nand_info.id, nand_info.size, nand_info.chipsize,
       nand_info.num_chips, nand_info.isLastPage);
  page_size = nand_info.PageSize;
  oob_size = nand_info.OobSize;
  ppb = nand_info.BlockSize / nand_info.PageSize;
#endif
#if 0
  int j = 0;
  int start_addr = 0xa0600000;  //6MB
  for (j = 0; j <= 2047; j++) {
    if (j == 0)
      REG32(start_addr + (j * 4)) = 0x3c1bb800;
    else if (j == 1)
      REG32(start_addr + (j * 4)) = 0x377b0104;
    else if (j == 2)
      REG32(start_addr + (j * 4)) = 0x3c1a0040;
    else if (j == 3)
      REG32(start_addr + (j * 4)) = 0x375a0000;
    else if (j == 4)
      REG32(start_addr + (j * 4)) = 0xaf7a0000;
    else if (j == 5)
      REG32(start_addr + (j * 4)) = 0x3c1bb800;
    else if (j == 6)
      REG32(start_addr + (j * 4)) = 0x377b3508;
    else if (j == 7)
      REG32(start_addr + (j * 4)) = 0x3c1a0000;
    else if (j == 8)
      REG32(start_addr + (j * 4)) = 0x375a0180;
    else if (j == 9)
      REG32(start_addr + (j * 4)) = 0xaf7a0000;
    else if (j == 10)
      REG32(start_addr + (j * 4)) = 0x3c100000;
    else if (j == 11)
      REG32(start_addr + (j * 4)) = 0x36100000;
    else if (j == 12)
      REG32(start_addr + (j * 4)) = 0x3c110000;
    else if (j == 13)
      REG32(start_addr + (j * 4)) = 0x363107e3;
    else if (j == 14) {
      for (j = 14; j <= 2032; j++)
        REG32(start_addr + (j * 4)) = 0x26100001;

    }
    else if (j == 2033)
      REG32(start_addr + (j * 4)) = 0x3c1bb800;
    else if (j == 2034)
      REG32(start_addr + (j * 4)) = 0x377b350c;
    else if (j == 2035)
      REG32(start_addr + (j * 4)) = 0x3c1a0000;
    else if (j == 2036)
      REG32(start_addr + (j * 4)) = 0x375a0000;
    else if (j == 2037)
      REG32(start_addr + (j * 4)) = 0x12110002;
    else if (j == 2038)
      REG32(start_addr + (j * 4)) = 0x00000000;
    else if (j == 2039)
      REG32(start_addr + (j * 4)) = 0x275a0080;
    else if (j == 2040)
      REG32(start_addr + (j * 4)) = 0x275a0100;
    else if (j == 2041)
      REG32(start_addr + (j * 4)) = 0xaf7a0000;
    else if (j == 2042)
      REG32(start_addr + (j * 4)) = 0x3c1abfc0;
    else if (j == 2043)
      REG32(start_addr + (j * 4)) = 0x375a1ff0;
    else if (j == 2044)
      REG32(start_addr + (j * 4)) = 0x03400008;
    else if (j == 2045)
      REG32(start_addr + (j * 4)) = 0x0;
    else if (j == 2046)
      REG32(start_addr + (j * 4)) = 0x0;
    else if (j == 2047)
      REG32(start_addr + (j * 4)) = 0x0;

    REG32(0xa0601fc4) = 0x3c1bb800;     //j=2033


  }
#endif /*  */
}

#if 1
#define Bad_Block_Table_Size 200
int 
isBadBlock (int block_start_cnt, int block_end_cnt ) 
{
  //debug cl
  //dprintf("[%s]:%d for boot safe debug return directly\n",__func__,__LINE__);
  return FAIL;
  char *nand_verify_buf;
  nand_verify_buf = malloc (page_size + oob_size);     //Based on minimum NAND DMA size
  int block_indicator;         //"1':good block , "0":bad block
  unsigned int block_scan_loop;
  unsigned int ppb_cnt;        //page per block
  unsigned int page_byte_loop;
  unsigned int pflash_addr_start, pflash_addr_start_offset;
  unsigned int block_scan_loop_good = 0;
  unsigned int block_scan_loop_bad = 0;
  unsigned int initial_bad_block_table[Bad_Block_Table_Size];  //
  for (block_scan_loop = block_start_cnt; block_scan_loop <= block_end_cnt;
        block_scan_loop++)
    {
      block_indicator = 1;    //good block
      // rtk_read_ecc_page(unsigned long flash_address, unsigned char *image_addr, unsigned int image_size,char ecc_enable)
      pflash_addr_start = block_scan_loop * (ppb * (page_size + oob_size));
      
        //prom_printf("\nScan Block(0x%x),block start address=0x%x", block_scan_loop, pflash_addr_start);
        for (ppb_cnt = 0; ppb_cnt < ppb; ppb_cnt++)
        {
          pflash_addr_start_offset = pflash_addr_start + (ppb_cnt * (page_size + oob_size));
          rtk_read_ecc_page (pflash_addr_start_offset, nand_verify_buf,
                              (page_size + oob_size), 0);
          for (page_byte_loop = 0; page_byte_loop < (page_size); page_byte_loop++)    //Only compare byte[0:2047]
            {
              if ((*(nand_verify_buf + page_byte_loop) & 0xff) != 0xff)
                {
                  
                    //bad block
                    block_indicator = 0;
                  
                    //prom_printf("=>Bad! \n");
                    block_scan_loop_bad++;
			//prom_printf("Fail, ppb_cnt(0x%x)&0xff,data=0x%x,page_byte_loop=%d\n",ppb_cnt,(*(nand_verify_buf+page_byte_loop)&0xff),page_byte_loop);
                    break;
                }
              
              else
                {
                  
                    //prom_printf("OK, ppb_cnt(0x%x)&0xff,data=0x%x,page_byte_loop=%d\n",ppb_cnt,(*(nand_verify_buf+page_byte_loop)&0xff),page_byte_loop);
                }
            }                  //end of check 512 byte 0xff
#if 1
          if (block_indicator == 0)     //bad block
            {
              
                /*Record bad block location */ 
                initial_bad_block_table[block_scan_loop_bad] =
                block_scan_loop;
                break;
            }
          
#endif  /*  */
        }                       //end of ppb_cnt
      if (block_indicator == 1)
        {
          
            //prom_printf("=>Good!\n");
            block_scan_loop_good++;
        }
      prom_printf (".");
    }
  prom_printf ("\n###################################\n");
  prom_printf ("*Scan Summary:");
  prom_printf ("\nTotal block nums:0x%x", block_scan_loop);
  prom_printf ("\npage_size=0x%x,page per block =0x%x", page_size,ppb);
  prom_printf ("\nGood nums:0x%x ", block_scan_loop_good);
  prom_printf ("\nBad nums:0x%x", block_scan_loop_bad);
  prom_printf ("\n\n*Initial Bad Block Location :");
  unsigned int bbt_loop;
  for (bbt_loop = 1; bbt_loop <= block_scan_loop_bad; bbt_loop++)
    prom_printf ("\nBlock[0x%x],faddr=0x%x",
                  initial_bad_block_table[bbt_loop],
                  initial_bad_block_table[bbt_loop] * ppb * (page_size +
                                                              oob_size));
  prom_printf ("\n###################################\n");
  free (nand_verify_buf);
}


#endif  /*  */
  
#if 0
  
                 /**
                  * nand_block_bad - [DEFAULT] Read bad block marker from the chip
                  */ 
int 
nand_block_bad () 
{
  if ((bad & 0xFF) != 0xff)
    res = 1;
}


#endif  /*  */
int rtk_check_allone(int page, int offset)
{
	unsigned int flash_addr1, flash_addr2;
	unsigned int data_out;
	int real_page, i, rlen;;

	real_page = page;

	/* rlen is equal to (512 + 16) */
	rlen = 528; 
	
	rtk_writel(0xc00fffff, NACR);

	/* Command cycle 1*/
	rtk_writel((CECS0|CMD_PG_READ_C1), NACMR);

	check_ready_nand();//check_ready();

	flash_addr1 =  ((real_page & 0xff) << 16) | offset;
	flash_addr2 = (real_page >> 8) & 0xffffff;

	/* Give address */
	rtk_writel( (enNextAD|AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);
	rtk_writel( (AD1EN|AD0EN|flash_addr2), NAADR);

	/* Command cycle 2*/
	rtk_writel((CECS0|CMD_PG_READ_C2), NACMR);

	check_ready_nand();//check_ready();

	for(i=0; i<(rlen/4); i++){
		data_out = rtk_readl(NADR);	
		if( data_out != 0xffffffff){
			
			printf("%s, page %d offset %x i %x da:%x\n",__FUNCTION__, page, offset, i,data_out);
			return -1;
		}
	}

	check_ready_nand();//check_ready();
	rtk_writel(0, NACMR);

	return 0;

}
int rtk_PIO_read_basic(int page, int offset)
{
	int i;
	unsigned int flash_addr1=0;
	unsigned int data_out;
	int rlen, real_page;
	unsigned int cmd;

	real_page = page;

	rlen = page_size + oob_size;
	cmd = CMD_PG_READ_A;

	rtk_writel(0xc00fffff, NACR);

	/* Command cycle 1*/
	rtk_writel((CECS0|cmd), NACMR);

	check_ready_nand();//check_ready();

	flash_addr1 |= ((real_page & 0xffffff) << 8) ;

	/* Give address */
	rtk_writel( (AD2EN|AD1EN|AD0EN|flash_addr1), NAADR);

	check_ready_nand();//check_ready();

	for(i=0; i<(rlen/4); i++){
		data_out = rtk_readl(NADR);
		//printf("[%3d] 0x%08X \n",i, data_out);
		if(data_out!=0xffffffff){
			printf("[%3d] 0x%08X \n",i, data_out);
			printf("%s: page %d offset %d i %d rlen %d\n",__FUNCTION__, page, offset, i, rlen);
			return -1;
		}
	}

	rtk_writel(0, NACMR);
	check_ready_nand(); //check_ready();
	rtk_writel (0x0, NACMR);    
	rtk_writel (0x0, NAADR);    
	rtk_writel ((CECS0 | CMD_RESET), NACMR);         //reset                 //rtk_nand_read_id () ;    
	check_ready_nand ();
	
	return 0;
	
}

int rtk_check_allone_512(int page)
{
	int rc=0;

	rc = rtk_PIO_read_basic(page,0);
	if(rc < 0)
		goto read_finish;

read_finish:
	return rc;
}
int rtk_check_pageData(int page, int offset)
{
	int rc = 0;
	int error_count,status;

	status = rtk_readl(NASR);

	if( (status & NDRS)== NDRS){		

		 if( status & NRER) {
			error_count = (status & 0xf0) >> 4;
			
			if(error_count <=4 && error_count > 0 ) {
				printf("[%s] boot: Correctable HW ECC Error at page=%x, status=0x%08X\n\r", __FUNCTION__, page,status);
				status &= 0x0f; //clear NECN
				rtk_writel(status, NASR);
				return 0;
			}else{			
//#ifdef CONFIG_NAND_BLOCK_512
//jasonwang  0709
if(NAND_ADDR_CYCLE)   
{
				if( rtk_check_allone_512(page) == 0 ){
					status &= 0x0f; //clear NECN
					rtk_writel(status, NASR);
				    	//printf("[%s] boot: Page %d is all one page, bypass it !!\n\r",__func__,page);
				    	return 0;
				}
}
else
//#else
{
				if( rtk_check_allone(page,offset) == 0 ){
					status &= 0x0f; //clear NECN
					rtk_writel(status, NASR);
				    	//printf("[%s] Page %d is all one page, bypass it !!\n\r",__func__,page);
				    	return 0;
				}
}
//#endif			
				printf("[%s] boot: Un-Correctable HW ECC Error at page=%x, status=0x%08X error_count %d\n\r", __FUNCTION__, page,status, error_count);
				status &= 0x0f; //clear NECN
				rtk_writel(status, NASR);
				return -1;				
			}
		}
		
	}
	else if( (status & NDWS)== NDWS){
		 if( status & NWER) {
			printf("[%s] boot: NAND Flash write failed at page=%x, status=0x%08X\n\r", __FUNCTION__, page,status);
			rtk_writel(status, NASR);
			return -1;
		}
	}

	rtk_writel(status, NASR);

	return rc;
}
//####################################################################
// Function : rtk_read_ecc_page
// Description : Read image from NAND flash to DRAM
// Input:
//		flash_address : address of flash page
//		image_addr     : address of image, in dram address
//		oob_addr		: address of oob, in dram address
//		image_size	: the length of image
// Output:
//		BOOL: 0=>OK, -1=>FAIL
//####################################################################
int  rtk_read_ecc_page_a (unsigned int flash_page, unsigned char *image_addr, unsigned char *oob_addr, unsigned int image_size)
{
    //debug cl for safe not write in 0,1 block that is for boot.
	if((flash_page < 128||flash_page > chip_size/page_size - 1)
#ifdef CONFIG_RTK_NAND_BBT
	&& (static_for_create_v2r_bbt = 0)
#endif
	)
	{
		//rtk_block_isbad will think block 0 and 1 is bad
		dprintf("[%s]:%d panic:fatal error *************,try to write in 0,1 block for boot,protect for debug\n",__func__,__LINE__);
		return FAIL;
	}
	
    //printf("[%s:] %d enter\n",__func__,__LINE__);
	int dram_sa, oob_sa;	
	int dma_counter = page_size >> 9;	//Move unit=512Byte 
	int dma_size;
	int buf_pos=0;
	int page_counter=0;
	int page_num[3], page_shift=0, page_start;
	unsigned char * oob_buf;
	unsigned long flash_addr_t=0, flash_addr_t1;
	#ifdef SWAP_2K_DATA
	int block= flash_page/ppb;
	#endif
	
	//Page size alignment
	dma_size = image_size;
	if( image_size%page_size ){
		dma_size +=(image_size%page_size);
	}

	//Translate nand flash address
	page_start = flash_page;
	
	for(page_shift=0;page_shift<3; page_shift++) {
             	page_num[page_shift] = ((page_start>>(8*page_shift)) & 0xff);
		if(!NAND_ADDR_CYCLE)
             		flash_addr_t |= (page_num[page_shift] << (12+8*page_shift));
		else
			flash_addr_t |= (page_num[page_shift] << (9+8*page_shift));
    	}

	//Setting default value of flash_addr_t1
	flash_addr_t1 = flash_addr_t;
	
	while( dma_size>0 ){

		//printf("[%s:] %d\n",__func__,__LINE__);
		dma_counter = page_size >> 9;

		if(oob_addr){
			flush_cache ();
				//dma_cache_writeb();
		}

		while(dma_counter >0) {  //Move 1 page
			flush_cache ();
			check_ready_nand();//check_ready();
			rtk_writel( (rtk_readl(NACR) |ECC_enable & (~RBO) & (~WBO)), NACR);

			//set DMA RAM start address
			dram_sa = ((unsigned int)image_addr+buf_pos*512) & (~M_mask);
			rtk_writel( dram_sa, NADRSAR);
			//printf("SDRAM address: 0x%08X ",dram_sa);

			//set DMA oob start address
			//oob_sa = ((unsigned int)oob_buf+buf_pos*16) & (~M_mask);
			oob_sa = ((unsigned int)oob_addr+buf_pos*16) & (~M_mask);
			rtk_writel( oob_sa, NADTSAR);

			//set DMA flash start address
			rtk_writel( flash_addr_t, NADFSAR);
			//printf("Flash address: 0x%08X \n\r",flash_addr_t);
			flash_addr_t +=528;
			
			//DMA read
			rtk_writel((~TAG_DIS) & (DESC0|DMARE|LBC_128),NADCRR);	
			check_ready_nand(); //check_ready();

			if(FAIL== rtk_check_pageData((page_start+page_counter), buf_pos*(512+16))) {
				flush_cache ();//dma_cache_writeb();
				return FAIL;
			}
			
			dma_counter--;
			buf_pos++;

		}

		page_counter +=1;

		if(!NAND_ADDR_CYCLE){
			flash_addr_t1 +=page_counter*0x1000;
		}else{
			flash_addr_t1 +=page_counter*0x200;
		}
		
		flash_addr_t = flash_addr_t1;
		dma_size -= page_size;
		
	}

	flush_cache ();
	#ifdef SWAP_2K_DATA
	if(block>=BOOT_BLOCK){
		unsigned int read_bbi;
		unsigned char switch_bbi = 0;
		if(isLastPage){
			read_bbi = flash_page & (ppb-1);
			if((read_bbi == (ppb-2)) || (read_bbi ==(ppb-1))){
				switch_bbi = 1;
			}
		}else{
			read_bbi = flash_page & (ppb-1);
			if((read_bbi == 0) || (read_bbi ==1)){
				switch_bbi = 1;
			}
		}

		/*test code*/
		/*because image convert app cannot decise which is the first page of block,so need do switch in every page*/
		switch_bbi = 1;
		
		if(!NAND_ADDR_CYCLE)
		{
			/*switch bad block info*/
			unsigned char temp_val=0;
			if(switch_bbi){
				temp_val = image_addr[DATA_BBI_OFF];
				image_addr[DATA_BBI_OFF] = oob_addr[BBI_SWAP_OFFSET]; 
				oob_addr[BBI_SWAP_OFFSET] = temp_val;
				//prom_printf("after swap,flash_page=%x,oob=%x,data=%x\n",flash_page,oob_addr[BBI_SWAP_OFFSET],image_addr[DATA_BBI_OFF]);
			}			
		}
		
		#if 0
		if(image_size>=BBI_DMA_OFFSET)
		image_addr[BBI_DMA_OFFSET]=oob_addr[BBI_SWAP_OFFSET];
		oob_addr[BBI_SWAP_OFFSET]=0xFF;
		#endif
		
		flush_cache ();
		
	}
	#endif
	//free(oob_buf);
	return SUCCESS;
}
//####################################################################
// Function : rtk_block_isbad
// Description : check bad block
// Input:
//		ofs: offset of data address
// Output:
//		BOOL: 0=>OK, 
//			 -1=>This block is bad, 
//			   1=>Read oob area fail,
//####################################################################
int rtk_block_isbad(unsigned int ofs)
{
	unsigned int page, block, page_offset;
	unsigned char block_status_p1;
	int i;

	unsigned char * oob_buf = mybuf;
	unsigned char * data_buf = mydatabuf;

	page = ((int) ofs) >> page_shift;
	page_offset = page & (ppb-1);
	block = page/ppb;

	if ( isLastPage ){
		page = block*ppb + (ppb-1);	
		if(rtk_read_ecc_page_a(page, data_buf, oob_buf, page_size)){
			printf("%s: read_oob page=%d failed\n", __FUNCTION__, page);
			return 1;
		}
		//ccwei 111116
        if(!NAND_ADDR_CYCLE)
		#ifdef SWAP_2K_DATA
			block_status_p1 = oob_buf[BBI_SWAP_OFFSET];
		#else
			block_status_p1 = oob_buf[0];
		#endif
		else
			block_status_p1 = oob_buf[5];
		
	}else{	
		if ( rtk_read_ecc_page_a(page, data_buf, oob_buf, page_size) ){
			printf ("%s: read_oob page=%d failed\n", __FUNCTION__, page);
			return 1;
		}

	    //ccwei 111116
	    if(!NAND_ADDR_CYCLE)
	    #ifdef SWAP_2K_DATA
			block_status_p1 = oob_buf[BBI_SWAP_OFFSET];
		#else
			block_status_p1 = oob_buf[0];
		#endif
	    else
		    block_status_p1 = oob_buf[5];
	}

	if( block_status_p1 == BBT_TAG){
		//printf("Reserved area for BBT: block=%d, block_status_p1=0x%x\n\r",block,block_status_p1);
		//printf("[%s:] %d block=%d is BBT_TAG already init ok\n",__func__,__LINE__,block);
		return 0;
	}else 
	if ( block_status_p1 != 0xff){		
		printf ("WARNING: Die 0: block=%d is bad, block_status_p1=0x%x\n\r", block, block_status_p1);
		return -1;
	}
	//printf("[%s:] %d block is normal good ok\n",__func__,__LINE__);
	return 0;

}
#if 1

                 /*

                 PIO mode for CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles

                 Example1:
                 step-1. check ready
                 step-2. ew $start_addr1	 = 0x0F000000  //page 0 ,block 0 , set the target address via NAADR
                 [check ready]

                 step-3. ew $start_addr2	 = 0x03000000  //page 0 ,block 0 , set the target address via NAADR
                 [check ready]

                 step-4. Set CE_CMD = 30H {optional: Cmd Set1}, [check ready]

                 step-5. Read NADR{ 4 bytes a time, from specified address to the end of the page}

                 Note: before setting CES0 to 1 (step.6) , user can successively
                 read NADR for the continue next 4 bytes

                 Phase-6. Set CECS0 = 1, [check ready]

                 Example2:
                 ew $start_addr1	 = 0x0F010000  //page 1
                 ew $start_addr2	 = 0x03000000  //page 1
                 ...

                 Example3:
                 ew $start_addr1	 = 0x0F400000  //page 0x40 ,block 1
                 ew $start_addr2	 = 0x03000000  //page 0x40
                 ...
                 */

int rtk_PIO_read_page (int flash_address,int enable_show_page_content,int report_bad_block)
{
    flush_cache ();

    if ((flash_address % (page_size + oob_size)) != 0)
    {
        dprintf ("\n\rflash_address must be page(0x%x+0x%x Bytes) aligned!\n",
            page_size, oob_size);
        return;
    }

    int addr_cycle[5], page_shift;
    int NAADR_REG_value;

    int block_indicator=1;                           //"1':good block , "0":bad block
    int bad_block_byte_index=0;

                                                     //pre-allocat
    int* ptr_PIO_READ_NADR= malloc (page_size + oob_size);

    int page_PIO_num = flash_address / (page_size + oob_size);

                 //dprintf ("page_PIO_num=0x%x\n", page_PIO_num);

    int i,j;

                 /*PIO read step-1.*/
    check_ready_nand ();

                 //rtk_writel ((rtk_readl (NACR) & ~(ECC_enable)), NACR); //disable ECC function
    check_ready_nand ();

                 /*PIO read step-2.*/
    rtk_writel (0x0, NACMR);
    rtk_writel ((CECS0 ), NACMR);

                 /*PIO read step-3.*/
#if 0

                 /*AD0/1/2EN = 1, enNextADcyc = 1, [CE_ADDR0] [CE_ADDR1] [CE_ADDR2], [check ready]*/
    rtk_writel((0x0f000000),NAADR);
    check_ready_nand ();

                 /*AD0/1EN = 1, enNextADcyc = 0, [CE_ADDR0] [CE_ADDR1] , [check ready]*/
    rtk_writel((0x03000000),NAADR);
    check_ready_nand ();
#endif

#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
    addr_cycle[0] = addr_cycle[1] = 0;               //Basic PIO read is "Page"

                 //Large page(size=0x840 bytes ) , block 1 , address = 0x21000 , page_PIO_num=0x40
                 //Large page(size=0x840 bytes ) , block 4 , address = 0x84000 , page_PIO_num=0x0100
                 //Large page(size=0x840 bytes ) , block 1024=0x400 , address = 0x8400000 , page_PIO_num=0x010000

    for (page_shift = 0; page_shift < 3; page_shift++)
    {
        addr_cycle[page_shift + 2] = (page_PIO_num >> (8 * page_shift)) & 0xff;
    }

    NAADR_REG_value=enNextAD|AD2EN|AD1EN|AD0EN|(addr_cycle[2]<<CE_ADDR2);
    rtk_writel(NAADR_REG_value,NAADR);
    check_ready_nand ();
                 //dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);

    NAADR_REG_value=(~enNextAD) & AD1EN | AD0EN |(addr_cycle[3]<<CE_ADDR0)|(addr_cycle[4]<<CE_ADDR1) ;
    rtk_writel(NAADR_REG_value,NAADR);
    check_ready_nand ();
                 //dprintf (" c1-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif

#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles
    addr_cycle[0] = addr_cycle[1] = 0;               //Basic PIO read is "Page"

    for (page_shift = 0; page_shift < 2; page_shift++)
    {
        addr_cycle[page_shift + 2] = (page_PIO_num >> (8 * page_shift)) & 0xff;
    }

    NAADR_REG_value=enNextAD|AD2EN|AD1EN|AD0EN|(addr_cycle[2]<<CE_ADDR2);
    rtk_writel(NAADR_REG_value,NAADR);
    check_ready_nand ();
                 //dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);

    NAADR_REG_value=(~enNextAD) & AD0EN |(addr_cycle[3]<<CE_ADDR0) ;
    rtk_writel(NAADR_REG_value,NAADR);
    check_ready_nand ();
                 //dprintf (" c1-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif

#ifdef CONFIG_NAND_Flash_Small_Page_64MB_4cycles
#error                                           //819xD not support , but pre-coding here based on Samsung's K9F1208U0C
    addr_cycle[0] = 0;                               //Basic PIO read is "Page"

    for (page_shift = 0; page_shift < 3; page_shift++)
    {
        addr_cycle[page_shift + 1] = (page_PIO_num >> (8 * page_shift)) & 0xff;
    }

    NAADR_REG_value=(~enNextAD) &|AD2EN|AD1EN|AD0EN|(addr_cycle[1]<<CE_ADDR0)|(addr_cycle[2]<<CE_ADDR1)|(addr_cycle[3]<<CE_ADDR2);
    rtk_writel(NAADR_REG_value,NAADR);
    check_ready_nand ();
                 //dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif

#ifdef CONFIG_NAND_Flash_Small_Page_32MB_3cycles
    addr_cycle[0] = 0;                               //Basic PIO read is "Page"

                 //Small page(size=0x210 bytes ) , block 1 , address = 0x21000 , page_PIO_num=0x40
                 //Small page(size=0x210 bytes ) , block 4 , address = 0x84000 , page_PIO_num=0x0100
                 //Small page(size=0x210 bytes ) , block 1024=0x400 , address = 0x8400000 , page_PIO_num=0x010000

    for (page_shift = 0; page_shift < 2; page_shift++)
    {
        addr_cycle[page_shift + 1] = (page_PIO_num >> (8 * page_shift)) & 0xff;
    }

    NAADR_REG_value=(~enNextAD)& AD2EN|AD1EN|AD0EN|(addr_cycle[2]<<CE_ADDR2)|(addr_cycle[1]<<CE_ADDR1);
    rtk_writel(NAADR_REG_value,NAADR);
    check_ready_nand ();
                   //dprintf ("NAADR_REG_value=0x%x\n", NAADR_REG_value);
                   //dprintf ("addr_cycle[1]=0x%x\n", addr_cycle[1]);
                    //dprintf ("addr_cycle[2]=0x%x\n", addr_cycle[2]);
#endif                                           /*  */

                 /*PIO read step-4.*/
    rtk_writel ((CECS0 | CMD_PG_READ_C2), NACMR);
    check_ready_nand ();

                 /*PIO read step-5.*/
#if 1
                 /*before setting CES0 to 1 (step.6) , user can successively read NADR for the continue next 4 bytes*/
    for (i=0;i<((page_size + oob_size)/4);i++)
    {
        *(ptr_PIO_READ_NADR+i) = rtk_readl (NADR);

        check_ready_nand ();

        if(enable_show_page_content)
        {
#if 0
            dprintf ("*(ptr_PIO_READ_NADR+%d)=0x%x\n",i,*(ptr_PIO_READ_NADR+i));
#else

            if(i%4==0)
            {
                dprintf ("\n%08X:    ",(i*4)+flash_address);

            }
            dprintf ("%08X    ",*(ptr_PIO_READ_NADR+i));
#endif
        }

                 //Show Bad block information and location
        if((*(ptr_PIO_READ_NADR+i)!=0xffffffff)&&(report_bad_block==1))
        {
            block_indicator=0;                       //bad block
            bad_block_byte_index=i;
                 //dprintf ("\nTotal page size=[0x%x]\n",(page_size + oob_size));
            dprintf ("\nNot equal to 0xffffffff at Byte[%d] , data =0x%x\n",i*4,*(ptr_PIO_READ_NADR+bad_block_byte_index));

            free(ptr_PIO_READ_NADR);

            return block_indicator;
        }

    }
#endif

    if(enable_show_page_content)
        dprintf ("\n");

                 /*PIO read step-6.*/

    rtk_writel (0x0, NACMR);
    rtk_writel (0x0, NAADR);
    rtk_writel ((CECS0 | CMD_RESET), NACMR);         //reset
                 //rtk_nand_read_id () ;
    check_ready_nand ();

                 //dprintf ("page=0x%x\n",page_PIO_num);

                 //dprintf ("ptr_PIO_READ_NADR=0x%x\n",ptr_PIO_READ_NADR);

    free(ptr_PIO_READ_NADR);

    return block_indicator;

}
#endif

#ifdef CONFIG_RTK_NAND_BBT
int check_BBT(unsigned int blk)
{	
	int i;	
	printf("[%s] blk:%d\n", __FUNCTION__, blk);	

	for ( i=0; i<RBA; i++)	
	{		
	    if ( bbt[i].bad_block == blk )
        {			
            printf("blk 0x%x already exist\n",blk);			
		    return -1;			
	    }	
	}	
	return 0;
}

//####################################################################
// Function : dump_BBT
// Description : Dump Bad Block Table
//			  [ a ] ( b, c, d, e )
//			  a: index;  b: BB_die, c : bad_block; d : RB_die; e : remap_block
//			  ex: [0] (0, 168, 0, 2047)
// Input:
//		NON
// Output:
//		NON
//####################################################################
static void dump_BBT(void)
{
	int i;
	int BBs=0;

	printf("[%s] Nand BBT Content\n\r", __FUNCTION__);

	for ( i=0; i<RBA; i++){
		if ( i==0 && bbt[i].BB_die == BB_DIE_INIT && bbt[i].bad_block == BB_INIT ){
			printf("Congratulation!! No BBs in this Nand.\n\r");
			break;
		}
		if ( bbt[i].bad_block != BB_INIT ){
			printf("[%d] (%d, %x, %x, %x)\n\r", i, bbt[i].BB_die,bbt[i].bad_block, 
				bbt[i].RB_die, bbt[i].remap_block);
			BBs++;
		}
	}
	//this->BBs = BBs;
	return;
}
#if 0

//####################################################################
// Function : rtk_block_isbad
// Description : check bad block
// Input:
//		ofs: offset of data address
// Output:
//		BOOL: 0=>OK, 
//			 -1=>This block is bad, 
//			   1=>Read oob area fail,
//####################################################################
int rtk_block_isbad(unsigned int ofs)
{
	unsigned int page, block, page_offset;
	unsigned char block_status_p1;
	int i;

	unsigned char * oob_buf = mybuf;
	unsigned char * data_buf = mydatabuf;

	page = ((int) ofs) >> page_shift;
	page_offset = page & (ppb-1);
	block = page/ppb;

	if ( isLastPage ){
		page = block*ppb + (ppb-1);	
		if(rtk_read_ecc_page_a(page, data_buf, oob_buf, page_size)){
			printf("%s: read_oob page=%d failed\n", __FUNCTION__, page);
			return 1;
		}
		//ccwei 111116
        if(!NAND_ADDR_CYCLE)		
		    block_status_p1 = oob_buf[0];
		else
			block_status_p1 = oob_buf[5];
		
	}else{	
		if ( rtk_read_ecc_page_a(page, data_buf, oob_buf, page_size) ){
			printf ("%s: read_oob page=%d failed\n", __FUNCTION__, page);
			return 1;
		}

	    //ccwei 111116
	    if(!NAND_ADDR_CYCLE)		
		    block_status_p1 = oob_buf[0];
	    else
		    block_status_p1 = oob_buf[5];
	}

	if( block_status_p1 == BBT_TAG){
		//printf("Reserved area for BBT: block=%d, block_status_p1=0x%x\n\r",block,block_status_p1);
		return 0;
	}else 
	if ( block_status_p1 != 0xff){		
		printf ("WARNING: Die 0: block=%d is bad, block_status_p1=0x%x\n\r", block, block_status_p1);
		return -1;
	}
	return 0;

}
#endif




int  rtk_erase_block_a (int page)
{
    //debug cl for safe not write in 0,1 block that is for boot.
    //printf("[%s:] %d \n",__func__,__LINE__);
	if(page < 128||page > chip_size/page_size - 1)
	{
		dprintf("[%s]:%d panic:fatal error *************,try to write in 0,1 block for boot,protect for debug\n",__func__,__LINE__);
		return FAIL;
	}
	int addr_cycle[5], page_shift;

	//printf("!!!!! rtk_erase_block !!!!!, block = %d\n\r",page/ppb);

	if ( page & (ppb-1) ){
		printf("page %d is not block alignment !!\n", page);
		return 0;
	}
	printf(".");
	check_ready_nand();

	rtk_writel( (rtk_readl(NACR) |ECC_enable|RBO), NACR);
	rtk_writel((NWER|NRER|NDRS|NDWS), NASR);
	rtk_writel(0x0, NACMR);

	rtk_writel((CECS0|CMD_BLK_ERASE_C1),NACMR);
	check_ready_nand();


	if(!NAND_ADDR_CYCLE){
		addr_cycle[0] = addr_cycle[1] =0;
		for(page_shift=0; page_shift<3; page_shift++){
			addr_cycle[page_shift+2] = (page>>(8*page_shift)) & 0xff;
		}

		rtk_writel( ((~enNextAD) & AD2EN|AD1EN|AD0EN|
				(addr_cycle[2]<<CE_ADDR0) |(addr_cycle[3]<<CE_ADDR1)|(addr_cycle[4]<<CE_ADDR2)),NAADR);
	}else{
		addr_cycle[0] = 0;
		for(page_shift=0; page_shift<4; page_shift++){
			addr_cycle[page_shift+1] = (page>>(8*page_shift)) & 0xff;
		}

		rtk_writel( ((~enNextAD) & AD2EN|AD1EN|AD0EN|
				(addr_cycle[1]<<CE_ADDR0) |(addr_cycle[2]<<CE_ADDR1)|(addr_cycle[3]<<CE_ADDR2)),NAADR);
	}



	rtk_writel((CECS0|CMD_BLK_ERASE_C2),NACMR);
	check_ready_nand();

	rtk_writel((CECS0|CMD_BLK_ERASE_C3),NACMR);
	check_ready_nand();

	if(rtk_readl(NADR) & 0x01){
		if( page>=0 && page < ppb)
			return SUCCESS;
		else
			return FAIL;
	}
		
	return SUCCESS;

}


//####################################################################
// Function : rtk_write_ecc_page
// Description : Write image from DRAM to NAND flash
// Input:
//		flash_page	: address of flash page, unit:page num
//		image_addr     : address of image, in dram address
//		oob_addr		: address of oob, in dram address
//		image_size	: the length of image
// Output:
//		BOOL: 0=>OK, -1=>FAIL
//####################################################################
int  rtk_write_ecc_page_a (unsigned int flash_page, unsigned char *image_addr, unsigned char *oob_addr,
		unsigned int image_size)
{
    //debug cl for safe not write in 0,1 block that is for boot.
	//printf("[%s:] %d\n",__func__,__LINE__);
	if(flash_page < 128||flash_page > chip_size/page_size - 1)
	{
		dprintf("[%s]:%d panic:fatal error *************,try to write in 0,1 block for boot,protect for debug\n",__func__,__LINE__);
		return FAIL;
	}
	
	int dram_sa, oob_sa;	
	int dma_counter = page_size >> 9;	//Move unit=512Byte 
	int dma_size;
	int buf_pos=0;
	int page_counter=0;
	int page_num[3], page_shift=0, page_start;
	unsigned long flash_addr_t=0, flash_addr_t1;
	#ifdef SWAP_2K_DATA
	int block=0;
	unsigned int write_bbi;
	unsigned char switch_bbi = 0;
	#endif
	//unsigned char *oob_buf;

	//debug cl
	rtk_writel(0xc00fffff, NACR);


	/*if ( (flash_address % (page_size))!=0 ){
		printf("\n\rflash_address must be 2KB aligned!!");
		return FAIL;
	}*/

	//Page size alignment
	dma_size = image_size;
	if( image_size%page_size ){
		dma_size +=(image_size%page_size);
	}
	
	//Translate nand flash address
	page_start = flash_page;   
	#ifdef SWAP_2K_DATA
	block = flash_page/ppb;
	
	if(block>=BOOT_BLOCK)
	{
		//printf("W before chunk[%d] is %x \n",BBI_DMA_OFFSET,image_addr[BBI_DMA_OFFSET]);
		//printf("W oob_buf [%d] is %x \n",BBI_SWAP_OFFSET, oob_addr[BBI_SWAP_OFFSET]); 

		if(isLastPage){
			/*bad block indicator is located in page (ppb-2) and page (ppb-1)*/
			write_bbi = flash_page & (ppb-1);
			if((write_bbi == (ppb-2)) || (write_bbi ==(ppb-1))){
				switch_bbi = 1;
			}
		}else{
			/*bad block indicator is located in page 0 and page 1*/
			write_bbi = flash_page & (ppb-1);
			if((write_bbi == 0) || (write_bbi ==1)){
				switch_bbi = 1;
			}
		}

		/*test code*/
		/*because image convert app cannot decise which is the first page of block,so need do switch in every page*/
		switch_bbi = 1;

		if(!NAND_ADDR_CYCLE)
		{
			unsigned char temp_val;
			if(switch_bbi){
				temp_val = image_addr[BBI_DMA_OFFSET];
				image_addr[BBI_DMA_OFFSET] = oob_addr[BBI_SWAP_OFFSET];
				oob_addr[BBI_SWAP_OFFSET] = temp_val;

				//prom_printf("after swap,flash_page=%x,oob=%x,data=%x\n",flash_page,oob_addr[BBI_SWAP_OFFSET],image_addr[DATA_BBI_OFF]);
			}
		}

		#if 0
		if(image_size>=BBI_DMA_OFFSET)
		oob_addr[BBI_SWAP_OFFSET]=image_addr[BBI_DMA_OFFSET];
		image_addr[BBI_DMA_OFFSET]=0xFF;
		#endif
		
		flush_cache ();

		
		//printf("W after  chunk[%d] is %x \n",BBI_DMA_OFFSET,image_addr[BBI_DMA_OFFSET]);
		//printf("W  oob_buf [%d] is %x \n",BBI_SWAP_OFFSET, oob_addr[BBI_SWAP_OFFSET]); 
		
	}
	#endif
	
	for(page_shift=0;page_shift<3; page_shift++) {
		 page_num[page_shift] = ((page_start>>(8*page_shift)) & 0xff);
		 if(!NAND_ADDR_CYCLE){
		 	 flash_addr_t |= (page_num[page_shift] << (12+8*page_shift));
		 	}
		 else{
		 	 flash_addr_t |= (page_num[page_shift] << (9+8*page_shift));
			 
		 	}
    	}

	//Setting default value of flash_addr_t1
	flash_addr_t1 = flash_addr_t;

	while( dma_size>0 ){

		if( (dma_size%(ppb*page_size)) == 0)
			printf(".");

		dma_counter = page_size >> 9;
		//printf("dma_counter:%x image:%x\r\n",dma_counter,page_size);
		

		flush_cache ();
		if(oob_addr){
			flush_cache ();
		}

		while(dma_counter >0) {  //Move 1 page
		//printf("-->dma_counter:%x r\n",dma_counter);
#if 1
			check_ready_nand();
			rtk_writel( (rtk_readl(NACR) |ECC_enable & (~RBO) & (~WBO)), NACR);

			//set DMA RAM start address
			dram_sa = ((unsigned int)image_addr+buf_pos*512) & (~M_mask);
			rtk_writel( dram_sa, NADRSAR);
			//printf("NADRSAR : 0x%08X ",rtk_readl(NADRSAR));

			//set DMA OOB start address
			oob_sa = ((unsigned int) oob_addr+buf_pos*16) & (~M_mask);
			rtk_writel( oob_sa, NADTSAR);

			//set DMA flash start address
			   //flash_addr_t &= 0x1fffffff;
			rtk_writel( flash_addr_t, NADFSAR);
			//printf("flash_addr_t:0x%08X NADFSAR : 0x%08X  c:%x\n\r",flash_addr_t,rtk_readl(NADFSAR),dma_counter);
			flash_addr_t +=528;
		
			//set OOB address
				rtk_writel(oob_sa, NADTSAR);
	 flush_cache ();		
			//DMA write
			rtk_writel( (~TAG_DIS) & (DESC0|DMAWE|LBC_128),NADCRR);	
			check_ready_nand();
#else
			   //prom_printf("\n DMA(512 Bytes) Start, DMA total count=%d\n\n\r",buf_pos);
            check_ready_nand ();
          rtk_writel ((rtk_readl (NACR) | ECC_enable), NACR);
          
            //set DMA RAM start address
            dram_sa = ((unsigned int) image_addr + buf_pos * 512) & (~M_mask);
          rtk_writel (dram_sa, NADRSAR);
          
           //dprintf("\nDMA-W:SDRAM address: 0x%X\n ",(dram_sa));
            //set DMA flash start address
            //flash_addr_t &= 0x1fffffff;
          rtk_writel (flash_addr_t, NADFSAR);
          
            //dprintf("\nFlash Laddress: 0x%X\n\n\r",(flash_addr_t));
            //set OOB address
            //rtk_writel(oob_sa, NADTSAR);
            //DMA write
            rtk_writel ((~TAG_DIS) &
                        ((0 << TAG_SEL) | DESC0 | DMAWE | LBC_128), NADCRR);
          check_ready_nand ();
#endif
			if(FAIL== rtk_check_pageData((page_start+page_counter), buf_pos*(512+16)))
				return FAIL;
			
			dma_counter--;
			buf_pos++;
			//printf("<--\n");

		}

		page_counter +=1;

		if(!NAND_ADDR_CYCLE)
			flash_addr_t1 =page_counter*0x1000;
		else
			flash_addr_t1 =page_counter*0x200;
		
		
		flash_addr_t = flash_addr_t1;
		dma_size -= page_size;
		//printf("flash_addr_t:%x dma_size %x\r\n",flash_addr_t,dma_size);

	}
	 flush_cache ();
	
	/*swap restore*/
	#ifdef SWAP_2K_DATA
	block = flash_page/ppb;
	//unsigned int write_bbi;
	//unsigned char switch_bbi = 0;
	
	if(block>=BOOT_BLOCK)
	{
		//printf("W before chunk[%d] is %x \n",BBI_DMA_OFFSET,image_addr[BBI_DMA_OFFSET]);
		//printf("W oob_buf [%d] is %x \n",BBI_SWAP_OFFSET, oob_addr[BBI_SWAP_OFFSET]); 

		if(isLastPage){
			/*bad block indicator is located in page (ppb-2) and page (ppb-1)*/
			write_bbi = flash_page & (ppb-1);
			if((write_bbi == (ppb-2)) || (write_bbi ==(ppb-1))){
				switch_bbi = 1;
			}
		}else{
			/*bad block indicator is located in page 0 and page 1*/
			write_bbi = flash_page & (ppb-1);
			if((write_bbi == 0) || (write_bbi ==1)){
				switch_bbi = 1;
			}
		}

		/*test code*/
		/*because image convert app cannot decise which is the first page of block,so need do switch in every page*/
		switch_bbi = 1;

		if(!NAND_ADDR_CYCLE)
		{
			unsigned char temp_val;
			if(switch_bbi){
				temp_val = image_addr[BBI_DMA_OFFSET];
				image_addr[BBI_DMA_OFFSET] = oob_addr[BBI_SWAP_OFFSET];
				oob_addr[BBI_SWAP_OFFSET] = temp_val;

				//prom_printf("after swap,flash_page=%x,oob=%x,data=%x\n",flash_page,oob_addr[BBI_SWAP_OFFSET],image_addr[DATA_BBI_OFF]);
			}
		}

		#if 0
		if(image_size>=BBI_DMA_OFFSET)
			oob_addr[BBI_SWAP_OFFSET]=image_addr[BBI_DMA_OFFSET];
		image_addr[BBI_DMA_OFFSET]=0xFF;
		#endif
		
		flush_cache ();

		
		//printf("W after  chunk[%d] is %x \n",BBI_DMA_OFFSET,image_addr[BBI_DMA_OFFSET]);
		//printf("W  oob_buf [%d] is %x \n",BBI_SWAP_OFFSET, oob_addr[BBI_SWAP_OFFSET]); 
		
	}
	#endif

	//free(oob_buf);
	return SUCCESS;
}

//####################################################################
// Function : scan_last_die_BB
// Description : Scan the Bad Block
// Input:
//		NON
// Output:
//		BOOL: 0=>OK, -1=>FAIL 
//####################################################################
static int scan_last_die_BB(void)
{
    dprintf("[%s:] %d\n",__func__,__LINE__);
	unsigned int start_page;
	unsigned int block_num;
	unsigned int addr;
	unsigned int rc;
	//int remap_block[RBA];
	int block_offset=0;
	int remap_count = 0;
	int i,j, table_index=0 , numchips=1;
	//start_page = BOOT_SIZE;
	block_num = (chip_size/block_size);

    dprintf("[%s:] %d block_num = %d\n",__func__,__LINE__,block_num );
	unsigned char *block_status = (unsigned char *) malloc( block_num );
	if ( !block_status ){
		printf("%s: Error, no enough memory for block_status\n\r",__FUNCTION__);
		rc = FAIL;
		goto EXIT;
	}
	memset ( (unsigned int *)block_status, 0, block_num );

	int *remap_block = (unsigned int*)malloc(sizeof(int)*RBA);
	if ( !remap_block ){
		printf("%s: Error, no enough memory for remap_block\n\r",__FUNCTION__);
		rc = FAIL;
		goto EXIT;
	}
	memset ( (unsigned int *)remap_block, 0, sizeof(int)*RBA );


	/*search bad block of all Nand flash area.*/
	for( addr=0; addr<chip_size; addr+=block_size ){
		int bb = addr >> block_shift;
		if ( rtk_block_isbad(addr) ){
			block_status[bb] = 0xff;
			printf("block[%d] is bad\n",bb);
		}
	}




    /*check  bad block in RBA;*/
	for ( i=0; i<RBA; i++){
		if ( block_status[(block_num-1)-i] == 0x00){
			remap_block[remap_count] = (block_num-1)-i;
			//printf("A: remap_block[%d]=%x %x\n",remap_count, remap_block[remap_count], (block_num-1)-i);
			remap_count++;
		}
	}

    /*If there are some bad blocks in RBA, the remain remap block just map to RB_INIT*/
	if (remap_count<RBA+1){
		for (j=remap_count+1; j<RBA+1; j++){
			remap_block[j-1] = RB_INIT;
			//printf("B: remap_block[%d]=%x\n",j-1, remap_block[j-1]);
		}
	}

#if 0
	//skip 1MB bootloader region
    block_offset = start_page >> block_shift;
//	for ( i=0; i<(block_num-RBA); i++){//remab bad block from start_page and before RBA
	for ( i=block_offset; i<(block_num-RBA_ori); i++){//remab bad block from start_page and before RBA

		if (block_status[i] == 0xff){
			bbt[table_index].bad_block = i;
			bbt[table_index].BB_die = numchips-1;
			bbt[table_index].remap_block = remap_block[table_index];
			bbt[table_index].RB_die = numchips-1;
printf("A:bbt[%d].bad_block = %x ",table_index, bbt[table_index].bad_block);
printf("A:bbt[%d].remap_block = %x \n",table_index, bbt[table_index].remap_block);			
			table_index++;
		}
	}
#endif
	for( i=table_index; table_index<RBA; table_index++){
		bbt[table_index].bad_block = BB_INIT;
		bbt[table_index].BB_die = BB_DIE_INIT;
		bbt[table_index].remap_block = remap_block[table_index];
		bbt[table_index].RB_die = numchips-1;
//printf("B:bbt[%d].bad_block = %x ",table_index, bbt[table_index].bad_block);
//printf("B:bbt[%d].remap_block = %x \n",table_index, bbt[table_index].remap_block);
	}
	
EXIT:
	if (rc){
		if (block_status)
			free(block_status);	
	}
	if(remap_block)
		free(remap_block);
	return 0;	
	
}

//####################################################################
// Function : rtk_create_bbt
// Description : Create Bad Block Table in specific block
// Input:
//		page: the page we want to put the BBT, it must be block alignment
// Output:
//		BOOL: 0=>OK, -1=>FAIL 
//####################################################################
static int rtk_create_bbt( int page)
{
    dprintf("[%s:] %d\n",__func__,__LINE__);
	int rc = 0;
	unsigned char *temp_BBT = 0;
	unsigned char mem_page_num, page_counter=0;

	if ( scan_last_die_BB() ){
		printf("[%s] scan_last_die_BB() error !!\n\r", __FUNCTION__);
		return -1;
	}

	mem_page_num = (sizeof(struct BB_t)*RBA + page_size-1 )/page_size;
	temp_BBT = (unsigned char *) malloc( mem_page_num*page_size);
	if ( !temp_BBT ){
		printf("%s: Error, no enough memory for temp_BBT\n\r",__FUNCTION__);
		return -1;
	}

	memset( temp_BBT, 0xff, mem_page_num*page_size);
	//while(1);//debug cl

  	if ( rtk_erase_block_a( page)){
		printf("[%s]erase block %d failure !!\n\r", __FUNCTION__, page/ppb);
		rc =  -1;
		goto EXIT;
	}
    if(!NAND_ADDR_CYCLE)
	#ifdef SWAP_2K_DATA
		NfSpareBuf[BBI_SWAP_OFFSET] = BBT_TAG;
	#else
		NfSpareBuf[0] = BBT_TAG;
	#endif
    else
		NfSpareBuf[5] = BBT_TAG;
	memcpy( temp_BBT, bbt, sizeof(struct BB_t)*RBA );
	while( mem_page_num>0 ){
		//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
		//	this->g_oobbuf, 1) )
		if(rtk_write_ecc_page_a(page+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
				printf("[%s] write BBT page %d failure!!\n\r", __FUNCTION__, page+page_counter);
				rc =  -1;
				goto EXIT;
		}
		page_counter++;
		mem_page_num--;		
	}

EXIT:
	if (temp_BBT)
		free(temp_BBT);
		
	return rc;	

}
//####################################################################
// Function : create_v2r_remapping
// Description : Create virtual block to real good block mapping table in specific block
// Input:
//		page: the page we want to put the V2R mapping table, it must be block alignment
//         block_v2r_num: from block 0 to the specified block number 
// Output:
//		BOOL: 0=>OK, -1=>FAIL 
//####################################################################
static int create_v2r_remapping(unsigned int page, unsigned int block_v2r_num)
{
    printf("[%s:] %d block_v2r_num = %d\n",__func__,__LINE__,block_v2r_num);
	//unsigned int block_v2r_num=0;
	unsigned char mem_page_num, page_counter=0;
	unsigned char *temp_BBT = 0;
    int rc=0;
	unsigned int offs=0, offs_real=0;
	unsigned int search_region=0, count=0;
    //offs = start_page;
	//offs_real = start_page;
#if 1
	count = 0;
	//debug cl
	int iii = 0;
	search_region = (block_v2r_num << block_shift);
	//just create [bootloader+user+rootfs+rootfs2] region remapping
    while(offs_real < search_region){
		if ( rtk_block_isbad(offs_real) ){
			offs_real += block_size;
			iii++;
		}else{
    		//bbt_v2r[count].block_v = (offs >> block_shift);
    		bbt_v2r[count].block_r = (offs_real >> block_shift);			
			offs+=block_size;
			offs_real += block_size;			
//printf("bbt_v2r[%d].block_v %d,  bbt_v2r[%d].block_r %d\n",count,bbt_v2r[count].block_v,count,bbt_v2r[count].block_r);
//printf("bbt_v2r[%d].block_r = %d\n",count,bbt_v2r[count].block_r);
			count++;
		}
	}
	
	printf("[%s:] %d bad block number iii = %d\n",__func__,__LINE__,iii);
	//printf("[%s, line %d] block_v2r_num %d\n\r",__FUNCTION__,__LINE__, block_v2r_num);
#endif
	mem_page_num = ((sizeof(struct BBT_v2r)*block_v2r_num) + page_size-1 )/page_size;
	//printf("[%s, line %d] mem_page_num = %d\n\r",__FUNCTION__,__LINE__,mem_page_num);

	temp_BBT = (unsigned char *) malloc( mem_page_num*page_size);
	if ( !temp_BBT ){
		printf("%s: Error, no enough memory for temp_BBT v2r\n\r",__FUNCTION__);
		rc = FAIL;
		goto EXIT_V2R;
	}
	memset( temp_BBT, 0xff, mem_page_num*page_size);
	//while(1); //debug cl

	if ( rtk_erase_block_a(page)){
		printf("[%s]erase block %d failure !!\n\r", __FUNCTION__, page/ppb);
		rc =  -1;
		goto EXIT_V2R;
	}
	if(!NAND_ADDR_CYCLE)
	#ifdef SWAP_2K_DATA
		NfSpareBuf[BBI_SWAP_OFFSET] = BBT_TAG;
	#else
		NfSpareBuf[0] = BBT_TAG;
	#endif
	else
		NfSpareBuf[5] = BBT_TAG;
	memcpy( temp_BBT, bbt_v2r, sizeof(struct BBT_v2r)*block_v2r_num );
	//dump_mem((unsigned int)temp_BBT,512);
	while( mem_page_num>0 ){
		//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
		//	this->g_oobbuf, 1) )
		if(rtk_write_ecc_page_a(page+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
				printf("[%s] write BBT page %d failure!!\n\r", __FUNCTION__, page+page_counter);
				rc =  -1;
				goto EXIT_V2R;
		}
//printf("[%s, line %d] mem_page_num = %d page_counter %d\n\r",__FUNCTION__,__LINE__,mem_page_num, page_counter);
			page_counter++;
			mem_page_num--; 	
	}
	EXIT_V2R:
	if(temp_BBT)
		free(temp_BBT);
	return rc;

}


int rtk_scan_v2r_bbt(void)
{
	unsigned int bbt_v2r_page;
    int rc=0, i=0, error_count=0;
	unsigned char isbbt=0;
	unsigned char mem_page_num=0, page_counter=0, mem_page_num_tmp=0;
	unsigned char *temp_BBT=NULL;
	unsigned int block_v2r_num=0;
	unsigned char load_bbt_error = 0, is_first_boot=1;
    //bbt_v2r_page = ((BOOT_SIZE/block_size)-(2*BACKUP_BBT))*ppb;
    bbt_v2r_page = (REMAP_BBT_POS/block_size)*ppb;

	//block_v2r_num = ((BOOT_SIZE + USER_SPACE_SIZE + VIMG_SPACE_SIZE + VIMG_SPACE_SIZE) >> block_shift);//jasonwang
	//block_v2r_num = ((chip_size) >> block_shift);//jasonwang
	/* winfred_wang */
	RBA = ((unsigned int)chip_size/block_size)*RBA_PERCENT / 100;
	block_v2r_num = ((chip_size) >> block_shift) - RBA;
#if 0	
    dprintf("[%s:] %d  nand_select = %d\n",__func__,__LINE__,nand_select);
    dprintf("[%s:] %d  block_size = 0x%x\n",__func__,__LINE__,block_size);
    dprintf("[%s:] %d  chip_size = 0x%x\n",__func__,__LINE__,chip_size);
    dprintf("[%s:] %d  block_shift = %d\n",__func__,__LINE__,block_shift);
    dprintf("[%s:] %d  page_shift = %d\n",__func__,__LINE__,page_shift);
    dprintf("[%s:] %d  isLastPage = %d\n",__func__,__LINE__,isLastPage);
    dprintf("[%s:] %d  pagemask = 0x%x\n",__func__,__LINE__,pagemask);

    dprintf("[%s:] %d  BOOT_SIZE = 0x%x\n",__func__,__LINE__,BOOT_SIZE);
    dprintf("[%s:] %d  USER_SPACE_SIZE = 0x%x\n",__func__,__LINE__,USER_SPACE_SIZE);
    dprintf("[%s:] %d  VIMG_SPACE_SIZE = 0x%x\n",__func__,__LINE__,VIMG_SPACE_SIZE);
	dprintf("[%s, line %d] block_v2r_num %d bbt_v2r_page %d\n\r",__FUNCTION__,__LINE__, block_v2r_num, bbt_v2r_page);
#endif	
	//while(1);//debug cl

	//create virtual block to real good block remapping table!!!
	bbt_v2r = (unsigned char *) malloc(sizeof(struct BBT_v2r)*(block_v2r_num));
	if(!bbt_v2r){
		printf("%s-%d: Error, no enough memory for bbt_v2r\n",__FUNCTION__,__LINE__);
		return FAIL;
	}

	mem_page_num = ((sizeof(struct BBT_v2r)*block_v2r_num) + page_size-1 )/page_size;
	//printf("[%s, line %d] mem_page_num = %d\n\r",__FUNCTION__,__LINE__,mem_page_num);


	temp_BBT =(unsigned char *)malloc( mem_page_num*page_size );
	if(!temp_BBT){
		printf("%s: Error, no enough memory for temp_BBT_v2r\n",__FUNCTION__);
		return FAIL;
	}
//test NEW method!
	for(i=0;i<BACKUP_BBT;i++){
	    rc = rtk_read_ecc_page_a(bbt_v2r_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
	    if(!NAND_ADDR_CYCLE)
		#ifdef SWAP_2K_DATA
			isbbt = NfSpareBuf[BBI_SWAP_OFFSET];
		#else
			isbbt = NfSpareBuf[0];
		#endif
	    else
		    isbbt = NfSpareBuf[5];
		if(!rc){
		    if(isbbt==BBT_TAG)
				is_first_boot = 0;
		}
	}
	//debug cl here
	//dprintf("[%s:] %d is_first_boot:%d, isbbt = 0x%X\n\r",__FUNCTION__,__LINE__, is_first_boot,isbbt);
	//isbbt = *((unsigned char *)(((unsigned int)NfSpareBuf)|0x20000000));
	//dprintf("[%s:] %d is_first_boot:%d, isbbt = 0x%X\n\r",__FUNCTION__,__LINE__, is_first_boot,isbbt);
	//unsigned char *ptr_v = 0x80000000;
	//unsigned char *ptr_p = 0xa0000000;
	//dprintf("[%s:] %d , *ptr_v = 0x%X\n\r",__FUNCTION__,__LINE__, *ptr_v);
	//dprintf("[%s:] %d , *ptr_p = 0x%X\n\r",__FUNCTION__,__LINE__, *ptr_p);
	//*ptr_v = 0x11;
	//*ptr_p = 0xaa;
	//dprintf("[%s:] %d\n",__func__,__LINE__);
	//dprintf("[%s:] %d , *ptr_v = 0x%X\n\r",__FUNCTION__,__LINE__, *ptr_v);
	//dprintf("[%s:] %d , *ptr_p = 0x%X\n\r",__FUNCTION__,__LINE__, *ptr_p);
	//flush_cache_range((void *)NfSpareBuf,64);
	//flush_cache_range(0x80000000,64);
	//flush_scache_range((void *)NfSpareBuf,64);
	//flush_scache_range(0x80000000,64);
	//dprintf("\n after flush invalidate:\n");
	//isbbt = NfSpareBuf[0];
	//dprintf("[%s:] %d is_first_boot:%d, isbbt = 0x%X\n\r",__FUNCTION__,__LINE__, is_first_boot,NfSpareBuf[0]);
	//isbbt = *((unsigned char *)(((unsigned int)NfSpareBuf)|0x20000000));
	//dprintf("[%s:] %d is_first_boot:%d, isbbt = 0x%X\n\r",__FUNCTION__,__LINE__, is_first_boot,isbbt);
	//dprintf("check if write back\n");
	//dprintf("[%s:] %d , *ptr_v = 0x%X\n\r",__FUNCTION__,__LINE__, *ptr_v);
	//dprintf("[%s:] %d , *ptr_p = 0x%X\n\r",__FUNCTION__,__LINE__, *ptr_p);
	////flush_cache_tmp((void *)0x80000000,64);
	//flush_cache_range(0x80000000,64);
	//flush_scache_range(0x80000000,64);
	//*ptr_p = 0x2e;
	//dprintf("check if invalidate\n");
	//dprintf("[%s:] %d , *ptr_v = 0x%X\n\r",__FUNCTION__,__LINE__, *ptr_v);
	//dprintf("[%s:] %d , *ptr_p = 0x%X\n\r",__FUNCTION__,__LINE__, *ptr_p);

	//dprintf("[%s:] %d, NfDataBuf start address = 0x%X\n",__func__,__LINE__,(unsigned long)(void *)&NfSpareBuf);
	//dprintf("[%s:] %d, NfDataBuf end address = 0x%X\n",__func__,__LINE__,(unsigned long)(void *)(&NfSpareBuf + 1));
	//is_first_boot = 1;
	//while(1);


	//dprintf("[%s:] %d is_first_boot:%d, isbbt = 0x%X\n\r",__FUNCTION__,__LINE__, is_first_boot,isbbt);
	for(i=0;i<BACKUP_BBT;i++){
		mem_page_num_tmp = mem_page_num;
		page_counter=0;load_bbt_error=0;
		
		rc = rtk_block_isbad((bbt_v2r_page+(i*ppb))*page_size);
		if(!rc){
			//printf("load v2r bbt table:%d page:%d\n\r",i, (bbt_v2r_page+(i*ppb)));
		    rc = rtk_read_ecc_page_a(bbt_v2r_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
		    if(!NAND_ADDR_CYCLE)
		    #ifdef SWAP_2K_DATA
				isbbt = NfSpareBuf[BBI_SWAP_OFFSET];
			#else
				isbbt = NfSpareBuf[0];
			#endif
		    else
			    isbbt = NfSpareBuf[5];
	        if(!rc){
			    //if(isbbt == BBT_TAG && 0)
			    if(isbbt == BBT_TAG){
			        //printf("[%s] have created bbt_v2r table:%d on block %d, just loads it !!\n\r", __FUNCTION__,i,(bbt_v2r_page/ppb)+i);
			        memcpy( temp_BBT, &NfDataBuf, page_size );
			        page_counter++;
			        mem_page_num_tmp--;
			        while( mem_page_num_tmp>0 ){
				        if( rtk_read_ecc_page_a((bbt_v2r_page+(i*ppb)+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					        printf("[%s] load bbt_v2r table%d error!!\n\r", __FUNCTION__,i);
					        //free(temp_BBT);
					        //load_bbt1 = 1;
					        load_bbt_error = 1;
					        //return -1;
					        //goto TRY_LOAD_BBT1;
			                error_count++;					        
					        break;
				        }
				        memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				        page_counter++;
				        mem_page_num_tmp--;
			        }
					if(!load_bbt_error){
			            memcpy( bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
					    //printf("check bbt_v2r table:%d OK\n\r",i);
					    goto CHECK_V2R_BBT_OK;
					}
			    }else{
					//printf("Create bbt_v2r table:%d is_first_boot %d\n\r",i,is_first_boot);
					if(is_first_boot)
					{
							//while(1);//debug cl
						//porting cl for debug not heart first two block
						static_for_create_v2r_bbt  = 1;
					    create_v2r_remapping(bbt_v2r_page+(i*ppb),block_v2r_num);
						static_for_create_v2r_bbt  = 0;
					}
			    }
		    }
		}else{
            //printf("bbt_v2r table:%d block:%d page:%d is bad\n\r",i,(bbt_v2r_page/ppb)+i,bbt_v2r_page+(i*ppb));
			error_count++;
		}
	}
CHECK_V2R_BBT_OK:	
//end!
#if 0
	rc = rtk_read_ecc_page(bbt_v2r_page, &NfDataBuf, &NfSpareBuf,page_size);
    if(!NAND_ADDR_CYCLE)
	    isbbt = NfSpareBuf[0];
	else
		isbbt = NfSpareBuf[5];

    //printf("[%s, line %d] isbbt_b0 = %d rc %d\n\r",__FUNCTION__,__LINE__,isbbt, rc);

	if(!rc){
		if(isbbt == BBT_TAG){
			printf("[%s] have created bbt_v2r B0 on block %d, just loads it !!\n\r", __FUNCTION__,bbt_v2r_page/ppb);
			memcpy( temp_BBT, &NfDataBuf, page_size );
			page_counter++;
			mem_page_num--;
			while( mem_page_num>0 ){
				if( rtk_read_ecc_page((bbt_v2r_page+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					printf("[%s] load bbt_v2r B0 error!!\n\r", __FUNCTION__);
					//free(temp_BBT);
					load_bbt1 = 1;
					//return -1;
					goto TRY_LOAD_BBT1;
				}
				memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;
			}
			memcpy( bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
		}else{
			printf("[%s] read bbt_v2r B0 tags fails, try to load bbt_v2r B1\n\r", __FUNCTION__);
			rc = rtk_read_ecc_page(bbt_v2r_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
            if(!NAND_ADDR_CYCLE)
	            isbbt = NfSpareBuf[0];
	        else
		        isbbt = NfSpareBuf[5];	
			if ( !rc ){
				if ( isbbt == BBT_TAG ){
					printf("[%s] have created bbt_v2r B1 on block %d, just loads it !!\n", __FUNCTION__, (bbt_v2r_page/ppb)+1);
					memcpy( temp_BBT, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;

					while( mem_page_num>0 ){
						if(rtk_read_ecc_page((bbt_v2r_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
							printf("[%s] load bbt_v2r B1 error!!\n\r", __FUNCTION__);
							free(temp_BBT);
							return -1;
						}
						memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
						page_counter++;
						mem_page_num--;
					}
					memcpy( bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
				}else{
					printf("[%s] read bbt_v2r B1 tags fails, nand driver will creat bbt_v2r B0 and B1\n\r", __FUNCTION__);
					create_v2r_remapping(bbt_v2r_page,block_v2r_num);
					create_v2r_remapping(bbt_v2r_page+ppb,block_v2r_num);
				} //BBT2_TAG
			}else{
				printf("[%s] read bbt_v2r B1 with HW ECC fails, nand driver will creat BBT B0\n", __FUNCTION__);
				create_v2r_remapping(bbt_v2r_page, block_v2r_num);
			}
		}// if BBT_TAG
	}else{
		printf("[%s] read bbt_v2r B0 with HW ECC error, try to load BBT B1\n\r", __FUNCTION__);
		rc = rtk_read_ecc_page(bbt_v2r_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
        if(!NAND_ADDR_CYCLE)
	        isbbt = NfSpareBuf[0];
	    else
		    isbbt = NfSpareBuf[5];	
		if ( !rc ){
			if ( isbbt == BBT_TAG ){
				printf("[%s] have created bbt_v2r B1 on block %d, just loads it !!\n\r", __FUNCTION__,(bbt_v2r_page/ppb)+1);
				memcpy( temp_BBT, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;

				while( mem_page_num>0 ){
					if(rtk_read_ecc_page((bbt_v2r_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
						printf("[%s] load bbt_v2r B1 error!!\n\r", __FUNCTION__);
						free(temp_BBT);
						return -1;
					}
					memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;
				}
				memcpy(bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
			}else{
				printf("[%s] read bbt_v2r B1 tags fails, nand driver will creat BBT B1\n\r", __FUNCTION__);
				create_v2r_remapping(bbt_v2r_page+ppb,block_v2r_num);//fix me! later
			}
		}else{
			printf("[%s-%d:] read bbt_v2r B0 and B1 with HW ECC fails\n\r", __FUNCTION__,__LINE__);
			free(temp_BBT);
			return -1;
		}
	}

TRY_LOAD_BBT1:
    if(load_bbt1){
		rc = rtk_read_ecc_page(bbt_v2r_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
        if(!NAND_ADDR_CYCLE)
	        isbbt = NfSpareBuf[0];
	    else
		    isbbt = NfSpareBuf[5];	
		if ( !rc ){
			if ( isbbt == BBT_TAG ){
				printf("[%s] have created bbt_v2r B1 on block %d, just loads it !!\n\r", __FUNCTION__,(bbt_v2r_page/ppb)+1);
				memcpy( temp_BBT, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;

				while( mem_page_num>0 ){
					if(rtk_read_ecc_page((bbt_v2r_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
						printf("[%s] load bbt_v2r B1 error!!\n\r", __FUNCTION__);
						free(temp_BBT);
						return -1;
					}
					memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;
				}
				memcpy(bbt_v2r, temp_BBT, sizeof(struct BBT_v2r)*(block_v2r_num));
			}
		}
	}
#endif
	if (temp_BBT)
		free(temp_BBT);
    if(error_count >= BACKUP_BBT){
        rc = -1;
		printf("%d v2r table are all bad!(T______T)\n\r", BACKUP_BBT);
		while(1); //debug cl porting for safe
	}
    return rc;
}



//####################################################################
// Function : rtk_nand_scan_bbt
// Description : Scan for bad block table
//			  bootcode @ block 0, BBT0 @ block 1, BBT1 @ block 2
// Input:
//		NON
// Output:
//		BOOL: 0=>OK, -1=>FAIL
//####################################################################
int rtk_nand_scan_bbt(void)
{
	unsigned char *temp_BBT=0;
	unsigned char mem_page_num, page_counter=0, mem_page_num_tmp=0;
	unsigned int rc=0, i=0, error_count=0;
	unsigned char isbbt;
	unsigned char load_bbt_error = 0, is_first_boot=1;
	//unsigned char check0, check1, check2, check3;
	unsigned int bbt_page;
    unsigned int addr;
	extern int block_size;
		{
    /*czyao, reserve RESERVED_AREA bytes area for bootloader, 
      and the last 'BACKUP_BBT' blocks of RESERVED_AREA area is for bad block table*/
	//bbt_page = ((BOOT_SIZE/block_size)-BACKUP_BBT)*ppb;
	bbt_page = ((REMAP_BBT_POS/block_size)+BACKUP_BBT)*ppb;
		}
	
	//printf("[%s, line %d] bbt_page = %d \n\r",__FUNCTION__,__LINE__,bbt_page);
	//printf("[%s, line %d] bbt_block = %d \n\r",__FUNCTION__,__LINE__,bbt_page/ppb);

	RBA = ((unsigned int)chip_size/block_size)*RBA_PERCENT / 100;

	//printf("[%s, line %d] RBA = %d \n\r",__FUNCTION__,__LINE__,RBA);

	//czyao 
	bbt = (unsigned char *) malloc(sizeof(struct BB_t)*RBA);
	if(!bbt){
		printf("%s-%d: Error, no enough memory for bbt\n",__FUNCTION__,__LINE__);
		return FAIL;
	}

	mem_page_num = (sizeof(struct BB_t)*RBA + page_size-1 )/page_size;
	//printf("[%s, line %d] mem_page_num = %d\n\r",__FUNCTION__,__LINE__,mem_page_num);

	temp_BBT =(unsigned char *)malloc( mem_page_num*page_size );
	if(!temp_BBT){
		printf("%s: Error, no enough memory for temp_BBT\n",__FUNCTION__);
		return FAIL;
	}

	memset( temp_BBT, 0xff, mem_page_num*page_size);
//test NEW method!
	for(i=0;i<BACKUP_BBT;i++){
	    rc = rtk_read_ecc_page_a(bbt_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
	    if(!NAND_ADDR_CYCLE)
	    #ifdef SWAP_2K_DATA
			isbbt = NfSpareBuf[BBI_SWAP_OFFSET];
		#else
			isbbt = NfSpareBuf[0];
		#endif
	    else
		    isbbt = NfSpareBuf[5];
		if(!rc){
		    if(isbbt==BBT_TAG)//check bbt has already created
				is_first_boot = 0;
		}
	}
	//printf("%s: is_first_boot:%d\n\r",__FUNCTION__, is_first_boot);
	//is_first_boot = 1;
	for(i=0;i<BACKUP_BBT;i++){
		mem_page_num_tmp = mem_page_num;
		page_counter=0;load_bbt_error=0;
		rc = rtk_block_isbad((bbt_page+(i*ppb))*page_size);
		if(!rc){
			//printf("load bbt table:%d page:%d\n\r",i, (bbt_page+(i*ppb)));
		    rc = rtk_read_ecc_page_a(bbt_page+(i*ppb), &NfDataBuf, &NfSpareBuf,page_size);
		    if(!NAND_ADDR_CYCLE)
		    #ifdef SWAP_2K_DATA
				isbbt = NfSpareBuf[BBI_SWAP_OFFSET];
			#else
				isbbt = NfSpareBuf[0];
			#endif
		    else
			    isbbt = NfSpareBuf[5];
	        if(!rc){
			    //if(isbbt == BBT_TAG &&0) //if v2r bbt table write error,can use the code to rebuild table
			    if(isbbt == BBT_TAG){
			        //printf("[%s] have created bbt table:%d on block %d, just loads it !!\n\r", __FUNCTION__,i,(bbt_page/ppb)+i);
			        memcpy( temp_BBT, &NfDataBuf, page_size );
			        page_counter++;
			        mem_page_num_tmp--;
			        while( mem_page_num_tmp>0 ){
				        if( rtk_read_ecc_page_a((bbt_page+(i*ppb)+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					        printf("[%s] load bbt table%d error!!\n\r", __FUNCTION__,i);
					        //free(temp_BBT);
					        //load_bbt1 = 1;
                            load_bbt_error=1;
					        //return -1;
					        //goto TRY_LOAD_BBT1;
			                error_count++;					        
					        break;
				        }
				        memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				        page_counter++;
				        mem_page_num_tmp--;
			        }
					if(!load_bbt_error){
					    memcpy( bbt, temp_BBT, sizeof(struct BB_t)*RBA );
					    //printf("check bbt table:%d OK\n\r",i);
					    goto CHECK_BBT_OK;
					}
			    }else{
					//printf("Create bbt table:%d is_first_boot:%d\n\r",i, is_first_boot);
					if(is_first_boot)
					{
						//debug cl for safe boot
						static_for_create_v2r_bbt = 1;
				        rtk_create_bbt(bbt_page+(i*ppb));
						static_for_create_v2r_bbt = 0;
					}
			    }
		    }
		}else{
            //printf("bbt table:%d block:%d page:%d is bad\n\r",i,(bbt_page/ppb)+i,bbt_page+(i*ppb));
			error_count++;
		}
	}
CHECK_BBT_OK:		
#if 0
    /*check bbt0*/
	rc = rtk_read_ecc_page(bbt_page, &NfDataBuf, &NfSpareBuf,page_size);

    if(!NAND_ADDR_CYCLE)
	    isbbt = NfSpareBuf[0];
	else
		isbbt = NfSpareBuf[5];
	//check0 = NfSpareBuf[0];
	//check1 = NfSpareBuf[1];
	//check2 = NfSpareBuf[2];
	//check3 = NfSpareBuf[3];
	//printf("[%s, line %d] isbbt_b0 = %d rc %d\n\r",__FUNCTION__,__LINE__,isbbt_b0, rc);

	if(!rc){
		if(isbbt == BBT_TAG){
			printf("[%s] have created bbt B0 on block %d, just loads it !!\n\r", __FUNCTION__,bbt_page/ppb);
			memcpy( temp_BBT, &NfDataBuf, page_size );
			page_counter++;
			mem_page_num--;

			while( mem_page_num>0 ){
				if( rtk_read_ecc_page((bbt_page+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					printf("[%s] load bbt B0 error!!\n\r", __FUNCTION__);
					//free(temp_BBT);
					/*runtime error in loading bbt0, we should try to load BBT1*/
					load_bbt1 = 1;
					//return -1;
					goto TRY_LOAD_BBT1;
				}
				memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;
			}
			memcpy( bbt, temp_BBT, sizeof(struct BB_t)*RBA );
		}else{
            /*condition: (1):in the first time, to load bbt1, and bbt0 doesn't have BBT_TAG*/
			printf("[%s] read BBT B0 tags fails, try to load BBT B1\n\r", __FUNCTION__);
			rc = rtk_read_ecc_page(bbt_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
            if(!NAND_ADDR_CYCLE)
	            isbbt = NfSpareBuf[0];
	        else
		        isbbt = NfSpareBuf[5];	
			if ( !rc ){
				if ( isbbt == BBT_TAG ){
					printf("[%s] have created bbt B1 on block %d, just loads it !!\n", __FUNCTION__, (bbt_page/ppb)+1);
					memcpy( temp_BBT, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;

					while( mem_page_num>0 ){
						if(rtk_read_ecc_page((bbt_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
							printf("[%s] load bbt B1 error!!\n\r", __FUNCTION__);
							free(temp_BBT);
			                /*condition: bbt0 and bbt1 are all bad, goodbye!!!
			                             change another flash chip!!!!*/							
							return -1;
						}
						memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
						page_counter++;
						mem_page_num--;
					}
					memcpy( bbt, temp_BBT, sizeof(struct BB_t)*RBA );
				}else{
		            /*the only condition: (1):in the first time to create bbt */
					printf("[%s] read BBT B1 tags fails, nand driver will creat BBT B0 and B1\n\r", __FUNCTION__);
					rtk_create_bbt(bbt_page);
					rtk_create_bbt(bbt_page+ppb);
				}
			}else{
	            /*condition: in first time we found that bbt1 is bad,  try to create bbt0*/			
				printf("[%s] read BBT B1 with HW ECC fails, nand driver will creat BBT B0\n", __FUNCTION__);
				rtk_create_bbt(bbt_page);
			}
		}// if BBT_TAG
	}else{
		/*condition: bbt0 is bad*/
		printf("[%s] read BBT B0 with HW ECC error, try to load BBT B1\n\r", __FUNCTION__);
		rc = rtk_read_ecc_page(bbt_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
        if(!NAND_ADDR_CYCLE)
	        isbbt = NfSpareBuf[0];
	    else
		    isbbt = NfSpareBuf[5];	
		if ( !rc ){
			if ( isbbt == BBT_TAG ){
				printf("[%s] have created bbt B1 on block %d, just loads it !!\n\r", __FUNCTION__,(bbt_page/ppb)+1);
				memcpy( temp_BBT, &NfDataBuf, page_size );
				page_counter++;
				mem_page_num--;

				while( mem_page_num>0 ){
					if(rtk_read_ecc_page((bbt_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
						printf("[%s] load bbt B1 error!!\n\r", __FUNCTION__);
						free(temp_BBT);
			            /*condition: bbt0 and bbt1 are all bad, goodbye!!!
			                      change another flash chip!!!!*/							
						return -1;
					}
					memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
					page_counter++;
					mem_page_num--;
				}
				memcpy( bbt , temp_BBT, sizeof(struct BB_t)*RBA );
			}else{
				printf("[%s] read BBT B1 tags fails, nand driver will creat BBT B1\n\r", __FUNCTION__);
				rtk_create_bbt( bbt_page+ppb);
			}
		}else{
			/*condition: bbt0 and bbt1 are all bad, goodbye!!!change another flash chip!!!!*/
			printf("[%s-%d:] read BBT B0 and B1 with HW ECC fails\n\r", __FUNCTION__,__LINE__);
			free(temp_BBT);
			return -1;
		}
	}
TRY_LOAD_BBT1:
	if(load_bbt1){
		printf("[%s] read BBT B0 have runtime error, try to load BBT B1\n\r", __FUNCTION__);
		rc = rtk_read_ecc_page(bbt_page+ppb, &NfDataBuf, &NfSpareBuf, page_size);
        if(!NAND_ADDR_CYCLE)
	        isbbt = NfSpareBuf[0];
	    else
		    isbbt = NfSpareBuf[5];
		if ( !rc ){
		    if ( isbbt == BBT_TAG ){
			    printf("[%s] have created bbt B1 on block %d, just loads it !!\n\r", __FUNCTION__,(bbt_page/ppb)+1);
			    memcpy( temp_BBT, &NfDataBuf, page_size );
			    page_counter++;
			    mem_page_num--;
			    while( mem_page_num>0 ){
				    if(rtk_read_ecc_page((bbt_page+ppb+page_counter), &NfDataBuf, &NfSpareBuf, page_size)){
					    printf("[%s] load bbt B1 error!!\n\r", __FUNCTION__);
					    free(temp_BBT);
					    /*condition: bbt0 and bbt1 are all bad, goodbye!!!
					       change another flash chip!!!!*/							
					    return -1;
				    }
				    memcpy( temp_BBT+page_counter*page_size, &NfDataBuf, page_size );
				    page_counter++;
				    mem_page_num--;
			    }
			    memcpy( bbt , temp_BBT, sizeof(struct BB_t)*RBA );
		    }
		}
	}
#endif	
	//dump_BBT();

	if (temp_BBT)
		free(temp_BBT);
    if(error_count >= BACKUP_BBT){
        rc = -1;
		printf("%d bbt table are all bad!(T______T)\n\r", BACKUP_BBT);
		while(1); //debug cl for safe boot.
	}		
	return rc;

}
int rtk_update_bbt (struct BB_t *bbt)
{
	int rc = 0, i=0, error_count=0;
	unsigned char *temp_BBT = 0;
	unsigned int bbt_page;
	unsigned char mem_page_num=0, page_counter=0;
	unsigned char mem_page_num_tmp=0, page_counter_tmp=0;

	//czyao, reserve 1M bytes area for bootloader, and the last 2 blocks of 1M area is for bad block table
	//bbt_page = ((BOOT_SIZE/block_size)-BACKUP_BBT)*ppb;
	bbt_page = ((REMAP_BBT_POS/block_size)+BACKUP_BBT)*ppb;
	
	//dprintf("[%s]:%d (BOOT_SIZE/block_size) = %d\n",__func__,__LINE__,BOOT_SIZE/block_size); //should be 8
	mem_page_num = (sizeof(struct BB_t)*RBA + page_size-1 )/page_size;
	//printf("[%s] mem_page_num %d\n\r", __FUNCTION__, mem_page_num);
	
	temp_BBT = (unsigned char *)malloc(mem_page_num*page_size);
	if ( !(temp_BBT) ){
		printf("%s: Error, no enough memory for temp_BBT\n",__FUNCTION__);
		return FAIL;
	}	
	memset(temp_BBT, 0xff, mem_page_num*page_size);
	memcpy(temp_BBT, bbt, sizeof(struct BB_t)*RBA );
	//memcpy(data_buf, temp_BBT, page_size);
/*
	if(!NAND_ADDR_CYCLE)
	    NfSpareBuf[0] = BBT_TAG;
	else
	    NfSpareBuf[5] = BBT_TAG;
*/	    
//test new method
    for(i=0;i<BACKUP_BBT;i++){
		mem_page_num_tmp = mem_page_num;
		page_counter=0;
		if(!NAND_ADDR_CYCLE)
		#ifdef SWAP_2K_DATA
			NfSpareBuf[BBI_SWAP_OFFSET] = BBT_TAG;
		#else
			NfSpareBuf[0] = BBT_TAG;
		#endif
		else
			NfSpareBuf[5] = BBT_TAG;
		if(rtk_erase_block_a(bbt_page+(ppb*i))){
			printf("[%s]error: erase BBT%d page %d failure\n\r", __FUNCTION__,i, bbt_page+(ppb*i));
			/*erase fail: mean this block is bad, so do not write data!!!*/
			mem_page_num_tmp = 0; 
			error_count++;
		}
		while( mem_page_num_tmp>0 ){
			//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
			//	this->g_oobbuf, 1) )
			if(rtk_write_ecc_page_a(bbt_page+(ppb*i)+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
					printf("[%s] write BBT%d page %d failure!!\n\r", __FUNCTION__,i, bbt_page+(ppb*i)+page_counter);
					//rc =  -1;
					//goto EXIT;
					error_count++;
					break;
			}
		//printf("[%s, line %d] mem_page_num = %d page_counter %d\n\r",__FUNCTION__,__LINE__,mem_page_num, page_counter);
			page_counter++;
			mem_page_num_tmp--; 	
		}	 

	}
//end
#if 0	
	if ( sizeof(struct BB_t)*RBA <= page_size){
		memcpy( data_buf, bbt, sizeof(struct BB_t)*RBA );
	}else{
		temp_BBT = (unsigned char *)malloc(2*page_size);
		if ( !(temp_BBT) ){
			printf("%s: Error, no enough memory for temp_BBT\n",__FUNCTION__);
			return FAIL;
		}
		memset(temp_BBT, 0xff, 2*page_size);
		memcpy(temp_BBT, bbt, sizeof(struct BB_t)*RBA );
		memcpy(data_buf, temp_BBT, page_size);
	}
#endif
#if 0 //test new method	
	if(rtk_erase_block(bbt_page)){
		printf("[%s]error: erase block 1 page %d failure\n\r", __FUNCTION__, bbt_page);
	}
#endif	
#if 0
	if(rtk_write_ecc_page(bbt_page, data_buf, oob_buf, page_size)){
		printf("[%s]update BBT B0 page 0 failure\n\r", __FUNCTION__);
	}else{
		if ( sizeof(struct BB_t)*RBA > page_size){
			memset(data_buf, 0xff, page_size);
			memcpy( data_buf, temp_BBT+page_size, sizeof(struct BB_t)*RBA - page_size );
			if(rtk_write_ecc_page(bbt_page+1, data_buf,  oob_buf, page_size)){
				printf("[%s]update BBT B0 page 1 failure\n\r", __FUNCTION__);
			}
		}	
	}
#endif
#if 0 //test new method	

	while( mem_page_num>0 ){
		//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
		//	this->g_oobbuf, 1) )
		if(rtk_write_ecc_page(bbt_page+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
				printf("[%s] write BBT1 page %d failure!!\n\r", __FUNCTION__, bbt_page+page_counter);
				rc =  -1;
				goto EXIT;
		}
//printf("[%s, line %d] mem_page_num = %d page_counter %d\n\r",__FUNCTION__,__LINE__,mem_page_num, page_counter);
		page_counter++;
		mem_page_num--;		
	}

	if ( rtk_erase_block(ppb+bbt_page)){
		printf("[%s]error: erase block 2, page %d failure\n\r", __FUNCTION__, ppb+bbt_page);
        rc = -1;
		goto EXIT;
	}
	while( mem_page_num>0 ){
		//if ( this->write_ecc_page(mtd, 0, page+page_counter, temp_BBT+page_counter*page_size, 
		//	this->g_oobbuf, 1) )
		if(rtk_write_ecc_page(bbt_page+ppb+page_counter,temp_BBT+page_counter*page_size, &NfSpareBuf, page_size))	{
				printf("[%s] write BBT2 page %d failure!!\n\r", __FUNCTION__, bbt_page+ppb+page_counter);
				rc =  -1;
				goto EXIT;
		}
//printf("[%s, line %d] mem_page_num = %d page_counter %d\n\r",__FUNCTION__,__LINE__,mem_page_num, page_counter);
		page_counter++;
		mem_page_num--;		
	}
#endif	
#if 0	
	if(rtk_write_ecc_page(ppb+bbt_page, data_buf, oob_buf, page_size)){
		printf("[%s]update BBT B1 failure\n\r", __FUNCTION__);
		return FAIL;
	}else{
		if ( sizeof(struct BB_t)*RBA > page_size){
			memset(data_buf, 0xff, page_size);
			memcpy( data_buf, temp_BBT+page_size, sizeof(struct BB_t)*RBA - page_size );
			if(rtk_write_ecc_page(ppb+bbt_page+1, data_buf,  oob_buf, page_size)){
				printf("[%s]error: erase block 0 failure\n\r", __FUNCTION__);
				return FAIL;
			}
		}		
	}
#endif
EXIT:
	if (temp_BBT)
		free(temp_BBT);
    if(error_count >= BACKUP_BBT){
		rc = -1;
		printf("%d table are all bad!(T______T)\n\r", BACKUP_BBT);
	}
	return rc;
}
#endif   


//debug cl
int rtk_PIO_write_word(unsigned long flash_address, unsigned char *image_addr, unsigned int image_size)
{
  //debug cl
  //dprintf("[%s]:%d for boot safe debug return directly\n",__func__,__LINE__);
  //return FAIL;
  flush_cache();
 

  if ((flash_address % (page_size + oob_size)) != 0) {
    dprintf("\n\rflash_address must be page(0x%x+0x%x Bytes) aligned!\n",
            page_size, oob_size);
    return;
  }

  int addr_cycle[5], page_shift;
  int NAADR_REG_value;

  int block_indicator = 1;      //"1':good block , "0":bad block
  int bad_block_byte_index = 0;

  //pre-allocat
  //int* ptr_PIO_WRITE_NADR= malloc (0x4);
  int *ptr_PIO_WRITE_NADR = image_addr;

  int page_PIO_num = flash_address / (page_size + oob_size);

  //dprintf ("page_PIO_num=0x%x\n", page_PIO_num);

  int i, j;


  /*PIO write step-0. */
#if 1
  rtk_writel(0x0, NACMR);       //Clear NACMR, set CECS0=0, CE_CMD = 0x00
  rtk_writel(0x0, NAADR);
  rtk_writel((CECS0 | CMD_RESET), NACMR);       //Reset for break PIO successively read 4Byte data from NADR      

#endif



  /*PIO write step-1. */
  check_ready_nand();

  //rtk_writel ((rtk_readl (NACR) & ~(ECC_enable)), NACR); //disable ECC function
  rtk_writel((rtk_readl(NACR) | (ECC_enable)), NACR);   //enable ECC function


  check_ready_nand();

  /*PIO write step-2. */
  rtk_writel(0x0, NACMR);
  rtk_writel((CECS0 | CMD_PG_WRITE_C1), NACMR); //JSW:0x80 is sequential data input command for 1st cycle.

  /*PIO write step-3. *///Send address
#if 0

  /*AD0/1/2EN = 1, enNextADcyc = 1, [CE_ADDR0] [CE_ADDR1] [CE_ADDR2], [check ready] */
  rtk_writel((0x0f000000), NAADR);
  check_ready_nand();

  /*AD0/1EN = 1, enNextADcyc = 0, [CE_ADDR0] [CE_ADDR1] , [check ready] */
  rtk_writel((0x03000000), NAADR);
  check_ready_nand();
#endif


#if  (defined(CONFIG_NAND_Flash_Large_Page_5cycles_Pages8KB_4GB)||defined(CONFIG_NAND_Flash_Large_Page_5cycles_Pages8KB_8GB))
  addr_cycle[0] = addr_cycle[1] = 0;    //Basic PIO read is "Page"

  //Large page(size=0x840 bytes ) , block 1 , address = 0x21000 , page_PIO_num=0x40
  //Large page(size=0x840 bytes ) , block 4 , address = 0x84000 , page_PIO_num=0x0100
  //Large page(size=0x840 bytes ) , block 1024=0x400 , address = 0x8400000 , page_PIO_num=0x010000

  for (page_shift = 0; page_shift < 3; page_shift++) {
    addr_cycle[page_shift + 2] = (page_PIO_num >> (8 * page_shift)) & 0xff;
  }

  NAADR_REG_value =
      enNextAD | AD2EN | AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR2);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);

  NAADR_REG_value =
      (~enNextAD) & AD1EN | AD0EN | (addr_cycle[3] << CE_ADDR0) |
      (addr_cycle[4] << CE_ADDR1);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf (" c1-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif


#ifdef CONFIG_NAND_Flash_Large_Page_5cycles_Pages4KB
  addr_cycle[0] = addr_cycle[1] = 0;    //Basic PIO read is "Page"

  //Large page(size=0x840 bytes ) , block 1 , address = 0x21000 , page_PIO_num=0x40
  //Large page(size=0x840 bytes ) , block 4 , address = 0x84000 , page_PIO_num=0x0100
  //Large page(size=0x840 bytes ) , block 1024=0x400 , address = 0x8400000 , page_PIO_num=0x010000

  for (page_shift = 0; page_shift < 3; page_shift++) {
    addr_cycle[page_shift + 2] = (page_PIO_num >> (8 * page_shift)) & 0xff;
  }

  NAADR_REG_value =
      enNextAD | AD2EN | AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR2);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);

  NAADR_REG_value =
      (~enNextAD) & AD1EN | AD0EN | (addr_cycle[3] << CE_ADDR0) |
      (addr_cycle[4] << CE_ADDR1);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf (" c1-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif

#ifdef CONFIG_NAND_Flash_Large_Page_256MBto1GB_5cycles
  addr_cycle[0] = addr_cycle[1] = 0;    //Basic PIO read is "Page"

  //Large page(size=0x840 bytes ) , block 1 , address = 0x21000 , page_PIO_num=0x40
  //Large page(size=0x840 bytes ) , block 4 , address = 0x84000 , page_PIO_num=0x0100
  //Large page(size=0x840 bytes ) , block 1024=0x400 , address = 0x8400000 , page_PIO_num=0x010000

  for (page_shift = 0; page_shift < 3; page_shift++) {
    addr_cycle[page_shift + 2] = (page_PIO_num >> (8 * page_shift)) & 0xff;
  }

  NAADR_REG_value =
      enNextAD | AD2EN | AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR2);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);

  NAADR_REG_value =
      (~enNextAD) & AD1EN | AD0EN | (addr_cycle[3] << CE_ADDR0) |
      (addr_cycle[4] << CE_ADDR1);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf (" c1-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif

#ifdef CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles
  addr_cycle[0] = addr_cycle[1] = 0;    //Basic PIO read is "Page"

  //Large page(size=0x840 bytes ) , block 1 , address = 0x21000 , page_PIO_num=0x40
  //Large page(size=0x840 bytes ) , block 4 , address = 0x84000 , page_PIO_num=0x0100
  //Large page(size=0x840 bytes ) , block 1024=0x400 , address = 0x8400000 , page_PIO_num=0x010000

  for (page_shift = 0; page_shift < 3; page_shift++) {
    addr_cycle[page_shift + 2] = (page_PIO_num >> (8 * page_shift)) & 0xff;
  }

  NAADR_REG_value =
      enNextAD | AD2EN | AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR2);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);

  NAADR_REG_value =
      (~enNextAD) & AD1EN | AD0EN | (addr_cycle[3] << CE_ADDR0) |
      (addr_cycle[4] << CE_ADDR1);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf (" c1-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif
#ifdef CONFIG_NAND_Flash_Large_Page_128MB_4cycles
  addr_cycle[0] = addr_cycle[1] = 0;    //Basic PIO read is "Page"

  for (page_shift = 0; page_shift < 2; page_shift++) {
    addr_cycle[page_shift + 2] = (page_PIO_num >> (8 * page_shift)) & 0xff;
  }

  NAADR_REG_value =
      enNextAD | AD2EN | AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR2);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  // dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);

  NAADR_REG_value = (~enNextAD) & AD0EN | (addr_cycle[3] << CE_ADDR0);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf (" c1-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif

#ifdef CONFIG_NAND_Flash_Small_Page_64MB_4cycles
//#error                                           //819xD not support , but pre-coding here based on Samsung's K9F1208U0C
  addr_cycle[0] = 0;    //Basic PIO read is "Page"

  for (page_shift = 0; page_shift < 3; page_shift++) {
    addr_cycle[page_shift + 1] = (page_PIO_num >> (8 * page_shift)) & 0xff;
  }

  NAADR_REG_value =
      (~enNextAD) & AD2EN | AD1EN | AD0EN | (addr_cycle[1] << CE_ADDR0) |
      (addr_cycle[2] << CE_ADDR1) | (addr_cycle[3] << CE_ADDR2);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //dprintf ("c0-NAADR_REG_value=0x%x\n", NAADR_REG_value);
#endif

#ifdef CONFIG_NAND_Flash_Small_Page_32MB_3cycles
  addr_cycle[0] = 0;    //Basic PIO read is "Page"

  //Small page(size=0x210 bytes ) , block 1 , address = 0x21000 , page_PIO_num=0x40
  //Small page(size=0x210 bytes ) , block 4 , address = 0x84000 , page_PIO_num=0x0100
  //Small page(size=0x210 bytes ) , block 1024=0x400 , address = 0x8400000 , page_PIO_num=0x010000

  for (page_shift = 0; page_shift < 2; page_shift++) {
    addr_cycle[page_shift + 1] = (page_PIO_num >> (8 * page_shift)) & 0xff;
  }

  NAADR_REG_value =
      (~enNextAD) & AD2EN | AD1EN | AD0EN | (addr_cycle[2] << CE_ADDR2) |
      (addr_cycle[1] << CE_ADDR1);
  rtk_writel(NAADR_REG_value, NAADR);
  check_ready_nand();
  //   dprintf ("NAADR_REG_value=0x%x\n", NAADR_REG_value);
  //  dprintf ("addr_cycle[1]=0x%x\n", addr_cycle[1]);
  //   dprintf ("addr_cycle[2]=0x%x\n", addr_cycle[2]);
#endif /*  */



#if 0

//Write NADR
  ew $w_data = 0xCC00DD88       //JSW:Write Data
ew @ $NADR = @$w_data:check_ready_4
      if ((@@$NACR & 0x80000000) != 0x80000000) {
  goto check_ready_4}

//JSW:0x10 is page program command for 2 cycle.
ew @ $NACMR = 0x40000010:check_ready_5
      if ((@@$NACR & 0x80000000) != 0x80000000) {
  goto check_ready_5}

  dv "[Write 0x%08x = 0x%08x]\n", @$NADR, @$w_data
//ew @$NACMR = 0x00000000
:    check_ready_6 if ((@@$NACR & 0x80000000) != 0x80000000) {
  goto check_ready_6}

#endif


  /*PIO write step-4. */
#if 1
  /*before setting CES0 to 1 (step.6) , user can successively read NADR for the continue next 4 bytes */
  for (i = 0; i < ((image_size) / 4); i++) {

#if 0   //Show ECC error msg
    /*Show ECC un-recovery message
     *Note: If spare space is not programmed by ECC-hareware circuit , enable this will cause NECN and NRER happen
     */
#if 1
    if ((rtk_readl(NASR) & 0xf0) >> 4)  //if (NECN[7:4])==4~1 ,means NECN happen
    {
      dprintf("\nNASR(0x%x): 0x%X\n ", NASR, rtk_readl(NASR));
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))

      dprintf("\nNASR's NECN ECC count=%d", (rtk_readl(NASR) & 0xf0) >> 4);


#else /*  */

      dprintf("\nNASR's NECN ECC count=%d", (rtk_readl(NASR) & 0xf0) >> 4);

#endif /*  */
      // __delay(1000 * 1000 * 10);
    }
    if (rtk_readl(NASR) & 0x8)  //if NRER=1 means ECC happen un-recovery errors
    {
      dprintf("\nNASR(0x%x): 0x%X\n ", NASR, rtk_readl(NASR));

      /*  */
      // __delay(1000 * 1000);

      //NRER write "1" to clear bit[3] and NECN write "0" to clear bit [7:4]
      rtk_writel((rtk_readl(NASR) & 0xFFFFFF0F), NASR);

      dprintf("\nAfter clear,NASR(0x%x): 0x%X\n ", NASR, rtk_readl(NASR));
      dprintf("\nNADTSAR(0x%x): 0x%X\n ", NADTSAR, rtk_readl(NADTSAR));
    }
#endif //show NECN message

#endif //end of show NECN message
    rtk_readl(NADR) = *(ptr_PIO_WRITE_NADR + i);

    check_ready_nand();



  }
#endif

  /*PIO write step-5. */
  rtk_writel((CECS0 | CMD_PG_WRITE_C2), NACMR); //JSW:0x10 is page program command for 2 cycle.
  check_ready_nand();


  //dprintf ("page=0x%x\n",page_PIO_num);

  //dprintf("ptr_PIO_WRITE_NADR=0x%x\n", ptr_PIO_WRITE_NADR);

  // free(ptr_PIO_WRITE_NADR);

  /*PIO write step-6. */
  rtk_writel(0x0, NACMR);       //Not select NAND CS0


}

/* must block size aligned */
extern unsigned int gCHKKEY_CNT;
#include "../init/utility.h"

/* src must 4 byte aligned ,and dst must block aligned */
int nflashwrite(unsigned long dst, unsigned long src, unsigned long length)
{
	/* nand flash write function */
	unsigned int start_block,block_end,start_page;
	unsigned int addr,page,offset = 0;
	unsigned char *tmp_oob = NULL;
	int i;

	start_block = dst>>block_shift;
	block_end = (length+dst-1)>>block_shift;
	start_page = ((dst)/page_size);

	if(length == 0)
		return 0;

	tmp_oob = (unsigned char*)malloc((sizeof(char)*ppb*oob_size));
	if(tmp_oob == NULL){
		prom_printf("malloc tmpoob fail\n");
		return -1;
	}
	
	//prom_printf("form %x to %x\n",start_block,(block_end));
	memset(tmp_oob,0xff,(sizeof(char)*ppb*oob_size));
	
#ifdef CONFIG_RTK_NAND_BBT
	for(i=start_block;i<block_end+1;i++){ //caculate how many block.
		addr = (i << block_shift);
		page = i * ppb;
		if(nand_erase_nand(addr, block_size)){
			printf("%s: erase blockv:%x pagev:%x fail!\n",__FUNCTION__, i, page);
			if(tmp_oob)
				free(tmp_oob);
			return -1;
		}
		if(nand_write_ecc_ob(addr, block_size, src+offset, tmp_oob)){
			printf("%s: nand_write_ecc addrv :%x error\n",__FUNCTION__, addr);
			if(tmp_oob)
				free(tmp_oob);
			return -1;
		}	
		offset += block_size;//shift buffer ptr one block each time.
	}
#else //CONFIG_RTK_NAND_BBT
	 for(i=0;i<(block_end-start_block+1);i++){ //caculate how many block.
			NEXT_BLOCK:
			#if 0	//donot need IMG_BACKUP_ADDR
			if(!j && (start_page*page_size >= IMG_BACKUP_ADDR)){
				printf("Warning: block[%d] overwrite IMG_BACKUP_ADDR region!!\n",i);
				return -1;
			}
			#endif
			while(rtk_block_isbad(start_page*page_size)){
				start_page+=ppb;
			}

			if(rtk_write_ecc_page(start_page*page_size, src+offset, block_size)){
				printf("HW ECC error on this block %d, just skip it!\n", (start_page/ppb));
				goto NEXT_BLOCK;
			}
			offset += block_size;//shift buffer ptr one block each time.
	}
#endif //CONFIG_RTK_NAND_BBT			
	if(tmp_oob)
		free(tmp_oob);
		
	return 0;
}


/* 	
	default read at DRAM_DIMAGE_ADDR=0xa0a00000 tmp value
*/
int nflashread (unsigned long dst, unsigned int src, unsigned long length,int checkEsc)
{
	int i;
	unsigned int start_block=0,block_end = 0, count=0, addr_bt=0,start_page=0,offset=0;
	
	unsigned char *ptr_data = (volatile unsigned char *)DRAM_DIMAGE_ADDR;
	unsigned char *ptr_oob  = (unsigned char*)malloc((sizeof(char)*ppb*oob_size));

	if(length == 0)
		return 0;

	offset = src % block_size;
	start_block = (src >> block_shift);
	block_end = (src+length-1+offset)>>block_shift;
	start_page = ((src)/page_size);
	
	if(ptr_oob == NULL){
		return 0;
	}
	
	#ifdef CONFIG_RTK_NAND_BBT
	count=0;	
	for(i=start_block;i< block_end+1;i++){ //caculate how many block.
		addr_bt = (i << block_shift);//real block index, addr.
		if(nand_read_ecc_ob(addr_bt, block_size, ptr_data+(count*block_size), ptr_oob)){
			if(ptr_oob)
				free(ptr_oob);
            return -1;
		}
		count++;

		if(checkEsc){
			if((i%0x10000) == 0){
				gCHKKEY_CNT++;
				if( gCHKKEY_CNT>ACCCNT_TOCHKKEY)
				{	gCHKKEY_CNT=0;
					if ( user_interrupt(0)==1 )  //return 1: got ESC Key
					{
						#if CONFIG_ESD_SUPPORT//patch for ESD
              	 		REG32(0xb800311c)|= (1<<23);
             			#endif
						//prom_printf("ret=%d  ------> line %d!\n",ret,__LINE__);
						if(ptr_oob)
							free(ptr_oob);
						return -1;
					}
				}
			}
		}
	}
	#else // CONFIG_RTK_NAND_BBT
	for(i=0;i<(block_end-start_block+1);i++){
		while(rtk_block_isbad(start_page*page_size)){
		        start_page+=ppb;
		}
		for(j=0;j<ppb;j++){
			if(rtk_read_ecc_page(start_page+j , ptr_data+ (block_size*i) + (j * page_size), ptr_oob, page_size)){
			    //printf("read ecc page :%d error\n", start_page+j);
				break;
			}
			 if(checkEsc){
				if((i%0x10000) == 0){
					gCHKKEY_CNT++;
					if( gCHKKEY_CNT>ACCCNT_TOCHKKEY)
					{	gCHKKEY_CNT=0;
						if ( user_interrupt(0)==1 )  //return 1: got ESC Key
						{
	             			#if CONFIG_ESD_SUPPORT//patch for ESD
	              	 		REG32(0xb800311c)|= (1<<23);
	             			#endif
							//prom_printf("ret=%d  ------> line %d!\n",ret,__LINE__);
							if(ptr_oob)
								free(ptr_oob);
							return -1;
						}
					}
				}			
		    }     
		}
	}
	#endif
	
	if(dst != 0){
		prom_printf("do copy,dst=%x,src=%x\n",dst,src);
		memcpy(dst,ptr_data+offset,length);
	}
	if(ptr_oob)
		free(ptr_oob);
	return 0;
}

		
