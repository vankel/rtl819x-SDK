#include "utility.h"
#include "rtk.h"

#include <asm/system.h>
#include <rtl8196x/asicregs.h>
#include <asm/mipsregs.h>


//#define UTILITY_DEBUG 1
#define NEED_CHKSUM 1

#ifdef CONFIG_NAND_FLASH_BOOTING
		//debug cl
		//#define DRAM_DIMAGE_ADDR       0x80a00000
		//#define DRAM_DOOB_ADDR           0x81000000
		#include "rtk_nand.h" // rtk_check_nand_space, DRAM_DLOADER_ADDR
		int tftp_img=0;
		//bootloader will use these parameter to note kernel the rootfs image addr.
		long SDKStart=0;
		long SDKStart2=0;
		long RunBackup=0;
		#undef FLASH_BASE
		#define FLASH_BASE 0
#endif

#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE	
#define BANK1_BOOT 1
#define BANK2_BOOT 2

#define GOOD_BANK_MARK_MASK 0x80000000  //goo abnk mark must set bit31 to 1

#define NO_IMAGE_BANK_MARK 0x80000000  
#define OLD_BURNADDR_BANK_MARK 0x80000001 
#define BASIC_BANK_MARK 0x80000002           
#define FORCEBOOT_BANK_MARK 0xFFFFFFF0  //means always boot/upgrade in this bank

#define IN_TFTP_MODE 0
#define IN_BOOTING_MODE 1


int boot_bank=0; 
unsigned long  bank_mark=0;

#endif



#ifdef CONFIG_FPGA_ROMCODE
//unsigned long  glexra_clock=40*1000*1000;  //FPGA
//unsigned long  glexra_clock=30*1000*1000;  //FPGA
unsigned long  glexra_clock=25*1000*1000;  //FPGA
//unsigned long  glexra_clock=33868800;  //FPGA
//unsigned long  glexra_clock=20*1000*1000;  //FPGA
#else
unsigned long  glexra_clock=200*1000*1000;
#endif



//------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------
//check img
unsigned int gCHKKEY_HIT=0;
unsigned int gCHKKEY_CNT=0;
#if defined(CONFIG_NFBI)
// return,  0: not found, 1: linux found, 2:linux with root found
int check_system_image(unsigned long addr,IMG_HEADER_Tp pHeader)
{
	// Read header, heck signature and checksum
	int i, ret=0;
	unsigned short sum=0, *word_ptr;
	unsigned short length=0;
	unsigned short temp16=0;

	if(gCHKKEY_HIT==1)	return 0;
	
    	/*check firmware image.*/
	word_ptr = (unsigned short *)pHeader;
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
		*word_ptr = *((unsigned short *)(addr + i));

	if (!memcmp(pHeader->signature, FW_SIGNATURE, SIG_LEN))
		ret=1;
	else if  (!memcmp(pHeader->signature, FW_SIGNATURE_WITH_ROOT, SIG_LEN))
		ret=2;
	else 
		dprintf("no sys signature at %X!\n",addr);
#if defined(NEED_CHKSUM)	
	if (ret) {
		for (i=0; i<pHeader->len; i+=2) {
			sum += *((unsigned short *)(addr + sizeof(IMG_HEADER_T) + i));
			//prom_printf("x=%x\n", (addr + sizeof(IMG_HEADER_T) + i));
		}

		if ( sum ) {
			//SYSSR: checksum done, but fail
			REG32(NFBI_SYSSR)= (REG32(NFBI_SYSSR)|0x8000) & (~0x4000);
			dprintf("sys checksum error at %X!\n",addr);
			ret=0;
		}
		else {
			//SYSSR: checksum done and OK
			REG32(NFBI_SYSSR)= REG32(NFBI_SYSSR) | 0xc000;
		}
	}
#else
	//SYSSR: checksum done and OK
	REG32(NFBI_SYSSR)= REG32(NFBI_SYSSR) | 0xc000;
#endif
	return (ret);
}

#elif defined(CONFIG_NONE_FLASH)
// return,  0: not found, 1: linux found, 2:linux with root found
int check_system_image(unsigned long addr,IMG_HEADER_Tp pHeader)
{
	// Read header, heck signature and checksum
	int i, ret=0;
	unsigned short sum=0, *word_ptr;
	unsigned short length=0;
	unsigned short temp16=0;

	if(gCHKKEY_HIT==1)	return 0;
	
    	/*check firmware image.*/
	word_ptr = (unsigned short *)pHeader;
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
		*word_ptr = *((unsigned short *)(addr + i));

	if (!memcmp(pHeader->signature, FW_SIGNATURE, SIG_LEN))
		ret=1;
	else if  (!memcmp(pHeader->signature, FW_SIGNATURE_WITH_ROOT, SIG_LEN))
		ret=2;
	else 
		dprintf("no sys signature at %X!\n",addr);
#if defined(NEED_CHKSUM)	
	if (ret) {
		for (i=0; i<pHeader->len; i+=2) {
			sum += *((unsigned short *)(addr + sizeof(IMG_HEADER_T) + i));
			//prom_printf("x=%x\n", (addr + sizeof(IMG_HEADER_T) + i));
		}

		if ( sum ) {
			//SYSSR: checksum done, but fail
			
			dprintf("sys checksum error at %X!\n",addr);
			ret=0;
		}
		else {
			//SYSSR: checksum done and OK
			
		}
	}
#else
	
#endif
	return (ret);
}

#else
#if CHECK_BURN_SERIAL
unsigned long board_rootfs_length=0;
IMG_HEADER_T linux_imghdr;

/* return 0:fail, 2:success, 1: no  burn_serial */
int check_burn_serial(unsigned long addr, IMG_HEADER_Tp pHeader)
{
	int ret = 0;

	if ((pHeader->burnAddr & (1<<31))) {
		unsigned long pad;
		memcpy((void *)(&pad), addr, sizeof(unsigned long));
		BDBG_BSN("\tburnAddr=0x%08x, pad=0x%08x", pHeader->burnAddr, pad);
		if (pHeader->burnAddr == pad) {
			BDBG_BSN(", ok\n");
			ret = 2;
		}
	}
	else {
		BDBG_BSN("\tfail\n");
		ret = 1;
	}

	return ret;
}
#endif
// return,  0: not found, 1: linux found, 2:linux with root found
int check_system_image(unsigned long addr,IMG_HEADER_Tp pHeader,SETTING_HEADER_Tp setting_header)
{
	// Read header, heck signature and checksum
	int i, ret=0;
	unsigned short sum=0, *word_ptr;
	unsigned short length=0;
	unsigned short temp16=0;
	char image_sig_check[1]={0};
	char image_sig[4]={0};
	char image_sig_root[4]={0};
	
	if(gCHKKEY_HIT==1)
		return 0;

#ifdef CONFIG_NAND_FLASH
	word_ptr = (unsigned short *)pHeader;
	unsigned char *ptr_data = (volatile unsigned char *)DRAM_DIMAGE_ADDR;
	if(nflashread(0,addr,block_size,0)< 0){
		prom_printf("nand flash read fail,addr=%x,size=%d\n",addr,block_size);
		return 0;
	}
	
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
	{
		*word_ptr = ((*(ptr_data+1+i))|*(ptr_data+i)<<8)&0xffff;
	}
			
#else

        /*check firmware image.*/
	word_ptr = (unsigned short *)pHeader;
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
		*word_ptr = rtl_inw(addr + i);	
#endif

	memcpy(image_sig, FW_SIGNATURE, SIG_LEN);
	memcpy(image_sig_root, FW_SIGNATURE_WITH_ROOT, SIG_LEN);

	if (!memcmp(pHeader->signature, image_sig, SIG_LEN))
		ret=1;
	else if  (!memcmp(pHeader->signature, image_sig_root, SIG_LEN))
		ret=2;
	else{
		prom_printf("no sys signature at %X!\n",addr-FLASH_BASE);
	}		
	//prom_printf("ret=%d  sys signature at %X!\n",ret,addr-FLASH_BASE);
	
#if CHECK_BURN_SERIAL
	if (ret) {
		int ret_val = 0;
		BDBG_BSN("==> check linux:\n");
		BDBG_BSN("\tby burn_serial\n");

		memcpy((void *)(&linux_imghdr), (void *)(pHeader), sizeof(IMG_HEADER_T));
#ifndef CONFIG_NAND_FLASH
		ret_val = check_burn_serial(addr+mips_io_port_base+sizeof(IMG_HEADER_T)+pHeader->len, pHeader);
#else
		/* for nand */
		goto SKIP_CHECK_BURN_SERIAL;
#endif
		if (ret_val != 1)
			return ret_val;
	}
	BDBG_BSN("\n\tno burn_serial, check by sum\n");

SKIP_CHECK_BURN_SERIAL:
#endif
	
	if (ret) {

#ifdef CONFIG_NAND_FLASH
			if(nflashread(0,addr,pHeader->len+sizeof(IMG_HEADER_T),1) < 0){
				prom_printf("nand flash read fail,addr=%x,size=%d\n",addr,pHeader->len+sizeof(IMG_HEADER_T));
				return 0;
			}
			#ifdef CONFIG_RTK_NAND_BBT
			for (i=0; i<pHeader->len; i+=2){
                 #if CONFIG_ESD_SUPPORT//patch for ESD
                  	 REG32(0xb800311c)|= (1<<23);
                 #endif
			
				#if defined(NEED_CHKSUM)	
				sum +=(unsigned short)(((*(ptr_data+1+i+ sizeof(IMG_HEADER_T)))|(*(ptr_data+i+ sizeof(IMG_HEADER_T)))<<8)&0xffff);
				//sum += rtl_inw(ptr_data + sizeof(IMG_HEADER_T) + i);
				#endif
			
			}
			#endif
			
#else
		for (i=0; i<pHeader->len; i+=2) {
#if 1  //slowly
			gCHKKEY_CNT++;
			if( gCHKKEY_CNT>ACCCNT_TOCHKKEY)
			{	gCHKKEY_CNT=0;
				if ( user_interrupt(0)==1 )  //return 1: got ESC Key
				{
					//prom_printf("ret=%d  ------> line %d!\n",ret,__LINE__);
					return 0;
				}
			}
#else  //speed-up, only support UART, not support GPIO
			if((Get_UART_Data()==ESC)  || (Get_GPIO_SW_IN()!=0))
			{	gCHKKEY_HIT=1; 
				return 0;
			}
#endif
#if defined(NEED_CHKSUM)	
			sum += rtl_inw(addr + sizeof(IMG_HEADER_T) + i);
#endif
		}	
#endif

#if defined(NEED_CHKSUM)			
		if ( sum ) {
			//prom_printf("ret=%d  ------> line %d!\n",ret,__LINE__);
			ret=0;
		}
#endif		
	}
	//prom_printf("ret=%d  sys signature at %X!\n",ret,addr-FLASH_BASE);

	return (ret);
}
//------------------------------------------------------------------------------------------

int check_rootfs_image(unsigned long addr)
{
#ifdef CONFIG_RTK_VOIP
    // Don't check rootfs in voip
         return 1;
#else    
	// Read header, heck signature and checksum
	int i;
	unsigned short sum=0, *word_ptr;
	unsigned long length=0;
	unsigned char tmpbuf[16];	
	

	
	if(gCHKKEY_HIT==1)
		return 0;

#ifdef CONFIG_NAND_FLASH
	word_ptr = (unsigned short *)tmpbuf;
	unsigned char *ptr_data = (volatile unsigned char *)DRAM_DIMAGE_ADDR;
	if(nflashread(0,addr,block_size,0)< 0){
		prom_printf("nand flash read fail,addr=%x,size=%d\n",addr,block_size);
		return 0;
	}
	
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
	{
		*word_ptr = ((*(ptr_data+1+i))|*(ptr_data+i)<<8)&0xffff;
	}
			
#else
	word_ptr = (unsigned short *)tmpbuf;
	for (i=0; i<16; i+=2, word_ptr++)
		*word_ptr = rtl_inw(addr + i);
#endif

	if ( memcmp(tmpbuf, SQSH_SIGNATURE, SIG_LEN) && memcmp(tmpbuf, SQSH_SIGNATURE_LE, SIG_LEN)) {
		prom_printf("no rootfs signature at %X!\n",addr-FLASH_BASE);
		return 0;
	}

#if CHECK_BURN_SERIAL
	board_rootfs_length =
#endif
	length = *(((unsigned long *)tmpbuf) + OFFSET_OF_LEN) + SIZE_OF_SQFS_SUPER_BLOCK + SIZE_OF_CHECKSUM;

#if CHECK_BURN_SERIAL
{
	struct _rootfs_padding rootfs_padding;
	BDBG_BSN("==> check rootfs:\n");
	BDBG_BSN("\tby burn_serial\n");

#ifndef CONFIG_NAND_FLASH
	memcpy((void *)(&rootfs_padding)+sizeof(rootfs_padding.zero_pad), (void *)(mips_io_port_base + addr + length - SIZE_OF_CHECKSUM), sizeof(struct _rootfs_padding)-sizeof(rootfs_padding.zero_pad));
#else
	/* nand */
	goto SKIP_CHECK_BURN_SERIAL;
#endif

	BDBG_BSN("\trootfs_padding.signature[%s]\n", rootfs_padding.signature);
	if (!memcmp(rootfs_padding.signature, ROOT_SIGNATURE, SIG_LEN)) {
		BDBG_BSN("\tburn_serial=0x%08x, length=0x%08x",
			rootfs_padding.len + SIZE_OF_SQFS_SUPER_BLOCK + SIZE_OF_CHECKSUM, length);

		if (rootfs_padding.len + SIZE_OF_SQFS_SUPER_BLOCK + SIZE_OF_CHECKSUM == length) {
			BDBG_BSN(", ok\n");
			return 1;
		}
		else {
			BDBG_BSN(", fail\n");
			return 0;
		}
	}
	BDBG_BSN("\n\tno burn_serial, check by sum\n");
}

SKIP_CHECK_BURN_SERIAL:
#endif
#ifdef CONFIG_NAND_FLASH
			if(nflashread(0,addr,length,1) < 0){
				prom_printf("nand flash read fail,addr=%x,size=%d\n",addr,length);
				return 0;
			}
			#ifdef CONFIG_RTK_NAND_BBT
			for (i=0; i<length; i+=2){
                 #if CONFIG_ESD_SUPPORT//patch for ESD
                  	 REG32(0xb800311c)|= (1<<23);
                  #endif
			
				#if defined(NEED_CHKSUM)	
				sum +=(unsigned short)(((*(ptr_data+1+i))|(*(ptr_data+i))<<8)&0xffff);
				//sum += rtl_inw(ptr_data + sizeof(IMG_HEADER_T) + i);
				#endif
			
			}
			#endif
			
#else

	for (i=0; i<length; i+=2) {
#if 1  //slowly
                 #if CONFIG_ESD_SUPPORT//patch for ESD
                  	 REG32(0xb800311c)|= (1<<23);
                  #endif
			gCHKKEY_CNT++;
			if( gCHKKEY_CNT>ACCCNT_TOCHKKEY)
			{	gCHKKEY_CNT=0;
				if ( user_interrupt(0)==1 )  //return 1: got ESC Key
					return 0;
			}
#else  //speed-up, only support UART, not support GPIO.
			if((Get_UART_Data()==ESC)  || (Get_GPIO_SW_IN()!=0))
			{	gCHKKEY_HIT=1; 
				return 0;
			}
#endif			
#if defined(NEED_CHKSUM)	
		sum += rtl_inw(addr + i);
#endif
	}
#endif

#if defined(NEED_CHKSUM)		
	if ( sum ) {
		prom_printf("rootfs checksum error at %X!\n",addr-FLASH_BASE);
		return 0;
	}	
#endif	
	return 1;
#endif //CONFIG_RTK_VOIP
}

#if defined(CONFIG_WEBPAGE_BACKUP)
int check_webpage_image(unsigned long addr)
{
	int i, ret=0;
	unsigned char sum=0;
	unsigned short *word_ptr;
	unsigned short length=0;
	unsigned short temp16=0;
	IMG_HEADER_T pHeader;
	char	image_web[4] = {0};



#ifdef CONFIG_NAND_FLASH
	word_ptr = (unsigned short *)&pHeader;
	unsigned char *ptr_data = (volatile unsigned char *)DRAM_DIMAGE_ADDR;
	if(nflashread(0,addr,block_size,0)< 0){
		prom_printf("nand flash read fail,addr=%x,size=%d\n",addr,block_size);
		return 0;
	}
	
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
	{
		*word_ptr = ((*(ptr_data+1+i))|*(ptr_data+i)<<8)&0xffff;
	}
			
#else
	word_ptr = (unsigned short *)&pHeader;
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
		*word_ptr = rtl_inw(addr + i);	
#endif

	memcpy(image_web,WEBPAGE_SIGNATURE,SIG_LEN);
	if (!memcmp(pHeader.signature, image_web, SIG_LEN))
		ret=1;
	else{
		prom_printf("no webpage signature at %X!\n",addr-FLASH_BASE);
	}
#ifdef CONFIG_NAND_FLASH
			if(nflashread(0,addr,pHeader->len+sizeof(IMG_HEADER_T),1) < 0){
				prom_printf("nand flash read fail,addr=%x,size=%d\n",addr,pHeader->len+sizeof(IMG_HEADER_T));
				return 0;
			}
			#ifdef CONFIG_RTK_NAND_BBT
			for (i=0; i<pHeader->len; i++){
                 #if CONFIG_ESD_SUPPORT//patch for ESD
                  	 REG32(0xb800311c)|= (1<<23);
                  #endif
			
				#if defined(NEED_CHKSUM)	
				sum +=(unsigned char)(((*(ptr_data+1+i+ sizeof(IMG_HEADER_T)))|(*(ptr_data+i+ sizeof(IMG_HEADER_T)))<<8)&0xffff);
				//sum += rtl_inw(ptr_data + sizeof(IMG_HEADER_T) + i);
				#endif
			
			}
			#endif
			
#else
	if (ret) {
		for (i=0; i<pHeader.len; i++){
		#if defined(NEED_CHKSUM)
			sum += rtl_inb(addr+sizeof(IMG_HEADER_T)+i);
		#endif
		}
	}
#endif

#if defined(NEED_CHKSUM)
	if(sum){
		prom_printf("webpage checksum error at %X!\n",addr-FLASH_BASE);
		ret = 0;
	}
#endif
		
	return ret;
}
#endif
//------------------------------------------------------------------------------------------

static int check_image_header(IMG_HEADER_Tp pHeader,SETTING_HEADER_Tp psetting_header,unsigned long bank_offset)
{
	int i,ret=0;
#ifndef CONFIG_NAND_FLASH_BOOTING
	//flash mapping
	return_addr = (unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET+bank_offset;
	ret = check_system_image((unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET+bank_offset,pHeader, psetting_header);

	if(ret==0) {
		return_addr = (unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET2+bank_offset;		
		ret=check_system_image((unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET2+bank_offset,  pHeader, psetting_header);
	}
	if(ret==0) {
		return_addr = (unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET3+bank_offset;				
		ret=check_system_image((unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET3+bank_offset,  pHeader, psetting_header);
	}			
#endif

#ifdef CONFIG_RTL_FLASH_MAPPING_ENABLE	
	i=CONFIG_LINUX_IMAGE_OFFSET_START;	
	while(i<=CONFIG_LINUX_IMAGE_OFFSET_END && (0==ret))
	{
		return_addr=(unsigned long)FLASH_BASE+i+bank_offset; 
		if(CODE_IMAGE_OFFSET == i || CODE_IMAGE_OFFSET2 == i || CODE_IMAGE_OFFSET3 == i){
			i += CONFIG_LINUX_IMAGE_OFFSET_STEP; 
			continue;
		}
		ret = check_system_image((unsigned long)FLASH_BASE+i+bank_offset, pHeader, psetting_header);
		i += CONFIG_LINUX_IMAGE_OFFSET_STEP; 
	}
#endif

	if(ret==2)
        {
#ifndef CONFIG_NAND_FLASH_BOOTING
                ret=check_rootfs_image((unsigned long)FLASH_BASE+ROOT_FS_OFFSET+bank_offset);
                if(ret==0)
                	ret=check_rootfs_image((unsigned long)FLASH_BASE+ROOT_FS_OFFSET+ROOT_FS_OFFSET_OP1+bank_offset);
                if(ret==0)
                	ret=check_rootfs_image((unsigned long)FLASH_BASE+ROOT_FS_OFFSET+ROOT_FS_OFFSET_OP1+ROOT_FS_OFFSET_OP2+bank_offset);
#else
				ret = 0;
#endif	

#ifdef CONFIG_RTL_FLASH_MAPPING_ENABLE
		i = CONFIG_ROOT_IMAGE_OFFSET_START;
		while((i <= CONFIG_ROOT_IMAGE_OFFSET_END) && (0==ret))
		{
			if( ROOT_FS_OFFSET == i ||
			    (ROOT_FS_OFFSET + ROOT_FS_OFFSET_OP1) == i ||
		            (ROOT_FS_OFFSET + ROOT_FS_OFFSET_OP1 + ROOT_FS_OFFSET_OP2) == i){
				i += CONFIG_ROOT_IMAGE_OFFSET_STEP;
				continue;
			}
			ret = check_rootfs_image((unsigned long)FLASH_BASE+i+bank_offset);
			i += CONFIG_ROOT_IMAGE_OFFSET_STEP;
		}
#endif

#if defined(CONFIG_WEBPAGE_BACKUP)
		if(ret)
		{
#ifndef CONFIG_NAND_FLASH_BOOTING
			ret=check_webpage_image((unsigned long)FLASH_BASE+WEBPAGE_OFFSET+bank_offset);
            if(ret==0)
            	ret=check_webpage_image((unsigned long)FLASH_BASE+WEBPAGE_OFFSET+WEBPAGE_OFFSET_OP1+bank_offset);
            if(ret==0)
            	ret=check_webpage_image((unsigned long)FLASH_BASE+WEBPAGE_OFFSET+WEBPAGE_OFFSET_OP1+WEBPAGE_OFFSET_OP2+bank_offset);
#endif

#ifdef CONFIG_RTL_FLASH_MAPPING_ENABLE
		i = CONFIG_WEBPAGE_IMAGE_OFFSET_START;
		while((i <= CONFIG_WEBPAGE_IMAGE_OFFSET_END) && (0==ret))
		{
			if( ROOT_FS_OFFSET == i ||
			    (ROOT_FS_OFFSET + ROOT_FS_OFFSET_OP1) == i ||
		            (ROOT_FS_OFFSET + ROOT_FS_OFFSET_OP1 + ROOT_FS_OFFSET_OP2) == i){
				i += CONFIG_WEBPAGE_IMAGE_OFFSET_STEP;
				continue;
			}
			ret = check_webpage_image((unsigned long)FLASH_BASE+i+bank_offset);
			i += CONFIG_WEBPAGE_IMAGE_OFFSET_STEP;
		}
#endif
			
		}
#endif

	}
	return ret;
}
//------------------------------------------------------------------------------------------

#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE	

int get_system_header(unsigned long addr, IMG_HEADER_Tp pImgHdr)
{
	unsigned short *word_ptr;
	char image_sig[4] = {0};
	char image_sig_root[4] = {0};
	int  i;

#ifdef CONFIG_NAND_FLASH
	word_ptr = (unsigned short *)pImgHdr;
	unsigned char *ptr_data = (volatile unsigned char *)DRAM_DIMAGE_ADDR;
	if(nflashread(0,addr,block_size,0)< 0){
		prom_printf("nand flash read fail,addr=%x,size=%d\n",addr,block_size);
		return 0;
	}
	
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
	{
		*word_ptr = ((*(ptr_data+1+i))|*(ptr_data+i)<<8)&0xffff;
	}
			
#else
	word_ptr = (unsigned short *)pImgHdr;
	for(i = 0; i < sizeof(IMG_HEADER_T); i+=2, word_ptr++)
		*word_ptr = rtl_inw(addr +i);
#endif
		
	memcpy(image_sig, FW_SIGNATURE, SIG_LEN);
	memcpy(image_sig_root, FW_SIGNATURE_WITH_ROOT, SIG_LEN);
	
	if(!memcmp(pImgHdr->signature, image_sig, SIG_LEN))
		return 1;
	else if(!memcmp(pImgHdr->signature, image_sig_root, SIG_LEN))
		return 2;
	else {
		//prom_printf("MOT: n o sys signature at %X!\n", addr-FLASH_BASE);
		return 0;
	}
}
//------------------------------------------------------------------------------------------

int find_system_header(IMG_HEADER_Tp pImgHdr, unsigned long bank_offset, unsigned long *addr)
{
	int  ret = 0;
	int i=0;
	unsigned long rAddr;

#ifndef CONFIG_NAND_FLASH
	rAddr = (unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET+bank_offset;
	ret = get_system_header(rAddr, pImgHdr);
	if(0 == ret) {
		rAddr = (unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET2+bank_offset;
		ret = get_system_header(rAddr, pImgHdr);
	}
	if(0 == ret) {
		rAddr = (unsigned long)FLASH_BASE+CODE_IMAGE_OFFSET3+bank_offset;
		ret = get_system_header(rAddr, pImgHdr);
	}
#endif
	
#ifdef CONFIG_RTL_FLASH_MAPPING_ENABLE	
	i=CONFIG_LINUX_IMAGE_OFFSET_START;	
	while(i<=CONFIG_LINUX_IMAGE_OFFSET_END && (0==ret))
	{
		rAddr=(unsigned long)FLASH_BASE+i+bank_offset; 

		if(CODE_IMAGE_OFFSET == i || CODE_IMAGE_OFFSET2 == i || CODE_IMAGE_OFFSET3 == i){
			i += CONFIG_LINUX_IMAGE_OFFSET_STEP; 
			continue;
		}
		ret = get_system_header(rAddr, pImgHdr);
		i += CONFIG_LINUX_IMAGE_OFFSET_STEP; 
	}
#endif

	if(0 != ret) {
		*addr = rAddr;
		//return_addr = rAddr;
	}
	
	return ret;
}
//------------------------------------------------------------------------------------------

unsigned long sel_burnbank_offset()
{
	unsigned long burn_offset=0;

	if( ((boot_bank == BANK1_BOOT) && ( bank_mark != FORCEBOOT_BANK_MARK)) ||
	     ((boot_bank == BANK2_BOOT) && ( bank_mark == FORCEBOOT_BANK_MARK))) //burn to bank2
		 burn_offset = CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET;

	return burn_offset;
}
//------------------------------------------------------------------------------------------

unsigned long get_next_bank_mark()
{
	if( bank_mark < BASIC_BANK_MARK)
		return BASIC_BANK_MARK;
	else if( bank_mark ==  FORCEBOOT_BANK_MARK)	 	
		return bank_mark;
	else
		return bank_mark+1;  
}
//------------------------------------------------------------------------------------------

unsigned long header_to_mark(int  flag, IMG_HEADER_Tp pHeader)
{
	unsigned long ret_mark=NO_IMAGE_BANK_MARK;
	//mark_dual ,  how to diff "no image" "image with no bank_mark(old)" , "boot with lowest priority"
	if(flag) //flag ==0 means ,header is illegal
	{
		if( (pHeader->burnAddr & GOOD_BANK_MARK_MASK) )
			ret_mark=pHeader->burnAddr;	
		else
			ret_mark = OLD_BURNADDR_BANK_MARK;
	}
	return ret_mark;
}
//------------------------------------------------------------------------------------------

int check_dualbank_setting(int in_mode)
{	
	int ret1=0,ret2=0,ret=0;
	unsigned long tmp_returnaddr;	
	IMG_HEADER_T tmp_bank_Header,Header,*pHeader=&Header; //0 :bank1 , 1 : bank2
	SETTING_HEADER_T setting_header,*psetting_header=&setting_header;
	unsigned long  tmp_bank_mark1,tmp_bank_mark2; 

	/* MOT debug */
	unsigned long  retAddr1, retAddr2, back_bank_offset = 0;
	int back_bank = 0;
	unsigned long back_bank_mark = 0, bank_offset;

	ret1 = find_system_header(&tmp_bank_Header, 0, &retAddr1);
	ret2 = find_system_header(&Header, CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET, &retAddr2);
	tmp_bank_mark1 = header_to_mark(ret1, &tmp_bank_Header);
	tmp_bank_mark2 = header_to_mark(ret2, &Header);
	
	if(tmp_bank_mark2 > tmp_bank_mark1) {
		boot_bank = BANK2_BOOT;
		back_bank = BANK1_BOOT;
		bank_mark = tmp_bank_mark2;
		back_bank_mark = tmp_bank_mark1;
		bank_offset = CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET;
		back_bank_offset = 0;
	} else {	
		boot_bank = BANK1_BOOT;
		back_bank = BANK2_BOOT;
		bank_mark = tmp_bank_mark1;
		back_bank_mark = tmp_bank_mark2;
		bank_offset = 0;
		back_bank_offset = CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET;
	}
	
	prom_printf("bootbank is %d, bankmark %X\n", boot_bank, bank_mark);
	/*TFTP MODE no need to checksum*/
	if(IN_TFTP_MODE == in_mode)
		return (ret1 || ret2);
	
	ret = check_image_header(pHeader, psetting_header, bank_offset);

	if(0 == ret) {
		ret = check_image_header(pHeader, psetting_header, back_bank_offset);
		if(0 != ret) {
			boot_bank = back_bank;
			bank_mark = back_bank_mark;
		}
	}
	
	return ret;
}
#endif

#endif //mark_nfbi

#ifdef CONFIG_RTK_BOOTINFO_DUALIMAGE //mark_boot
#define CONFIG_RTK_BOOTINFO_SUPPORT 1
#endif

#ifdef CONFIG_RTK_BOOTINFO_SUPPORT
#include "rtk_bootinfo.c"
BOOTINFO_T bootinfo_ram;
#define FLASH_MEM_MAP_ADDR 0xbd000000
#ifdef CONFIG_RTL8198C
#define FLASH_BOOTINFO_OFFSET 0x2a000
#else
#define FLASH_BOOTINFO_OFFSET 0xC000
#endif

//#### bootinfo user define 
//#define USER_MAXBOOTCNT 0    //change to non-zero will  set to bootinfo fixed.

void rtk_flash_write_data(unsigned int flash_addr,unsigned int len,unsigned char* data)
{		
	//ComSrlCmd_ComWriteData(0,flash_addr,len,data);
#ifdef CONFIG_SPI_FLASH
	#ifdef SUPPORT_SPI_MIO_8198_8196C
		spi_flw_image_mio_8198(0,flash_addr,data,len);
	#else
		spi_flw_image(0,flash_addr,data,len);
	#endif
#endif
}

static void  rtk_init_bootinfo(BOOTINFO_P boot)
{
	rtk_read_bootinfo_from_flash(FLASH_MEM_MAP_ADDR+FLASH_BOOTINFO_OFFSET,boot);

	 if(!rtk_check_bootinfo(boot)) // if not valid bootinfo header use default setting
	 	rtk_reset_bootinfo(boot);	 
}

static void rtk_inc_bootcnt() //increase by 1 to indicate booting record
{
	BOOTINFO_P boot=&bootinfo_ram ;
       boot->data.field.bootcnt ++;        
       prom_printf("bootbank = %d ,bootcnt=%d \n",boot->data.field.bootbank, boot->data.field.bootcnt );//mark_boot	   
       rtk_write_bootinfo_to_flash(FLASH_BOOTINFO_OFFSET,boot,rtk_flash_write_data); 
}

void rtk_update_bootbank(char bank)
{
	BOOTINFO_P boot=&bootinfo_ram ;
	boot->data.field.bootbank = bank;
	boot->data.field.bootcnt = 0;
	rtk_write_bootinfo_to_flash(FLASH_BOOTINFO_OFFSET,boot,rtk_flash_write_data); 
}	

unsigned int  rtk_get_next_bootbank()
{
	unsigned int next_bank=0;
	BOOTINFO_P boot=&bootinfo_ram ;

#ifdef CONFIG_RTK_BOOTINFO_DUALIMAGE	
	//if  toggle mode ,  next bank is toggle
	if( boot->data.field.bootmode == 1 ) 
	{
		if( boot->data.field.bootbank == 0 )
			next_bank = 1;
		else
			next_bank = 0;
	}	
	else	//if normal mode  , nextbank = active bank
#endif
  	      next_bank = boot->data.field.bootbank;	

	return next_bank;
}

static int rtk_check_bank_image(IMG_HEADER_Tp pHeader,SETTING_HEADER_Tp psetting_header,BOOTINFO_P boot)
{	
	int ret=0;
	unsigned int bank_offset=0;	
#ifdef CONFIG_RTK_BOOTINFO_DUALIMAGE
	unsigned char next_bank ;
       
	if(boot->data.field.bootbank == 1 ) // check image depend on bootbank
		bank_offset = CONFIG_RTK_DUALIMAGE_FLASH_OFFSET;
#endif	

	ret=check_image_header(pHeader,psetting_header,bank_offset); 
       prom_printf("rtk_check_bank_image ret=%d\n", ret);//mark_boot	

#ifdef CONFIG_RTK_BOOTINFO_DUALIMAGE
	if(!ret) //checksum errot
	{
		next_bank= (unsigned char)rtk_get_next_bootbank();
		prom_printf("checksum error switch to backup bank%d\n", next_bank);//mark_boot	   
		boot->data.field.bootbank = next_bank;
		boot->data.field.bootcnt = 0;
		bank_offset = 0 ;
		if(next_bank == 1 ) // check image depend on bootbank
			bank_offset = CONFIG_RTK_DUALIMAGE_FLASH_OFFSET;		
		ret=check_image_header(pHeader,psetting_header,bank_offset);	
	}
#endif	
	return ret;
}

static int  rtk_ckeck_booting(BOOTINFO_P boot)
{	
	unsigned char next_bank ;
	next_bank= (unsigned char)rtk_get_next_bootbank();

    //if( !boot->data.field.bootmaxcnt )	
	 	//return 1;  //if max=0 mean ignore check

     if(boot->data.field.bootcnt >=  boot->data.field.bootmaxcnt )
     {
#ifdef CONFIG_RTK_BOOTINFO_DUALIMAGE
	  	if(  next_bank != boot->data.field.bootbank  )  
	  	{
	  		prom_printf("bootinf fail maxcnt reached switch to backup bank%d\n", next_bank);//mark_boot
			boot->data.field.bootbank = next_bank;
			boot->data.field.bootcnt = 0;
	  	}	
		 else	
#endif
	    	   return 0;  //reach max booting count , return 0 to indecater fail boot.
      }

     return 1;	  //ok booting
 }
#endif

//------------------------------------------------------------------------------------------

int check_image(IMG_HEADER_Tp pHeader,SETTING_HEADER_Tp psetting_header)
{
	int ret=0;
#if defined(CONFIG_NFBI) || defined(CONFIG_NONE_FLASH)
	prom_printf("---NFBI or ROM booting---\n");
#else
#ifdef CONFIG_RTK_BOOTINFO_SUPPORT
	rtk_init_bootinfo(&bootinfo_ram);
	ret = rtk_ckeck_booting(&bootinfo_ram);
	if(!ret)
		return ret;
#endif
 	//only one bank
	 #ifndef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE	

 	#ifndef CONFIG_RTK_BOOTINFO_SUPPORT
 	ret=check_image_header(pHeader,psetting_header,0); 
	#else		
	ret=rtk_check_bank_image(pHeader,psetting_header,&bootinfo_ram); 
	#endif

	/* winfred_wang static mode */
	#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_STATIC
	#ifndef CONFIG_NAND_FLASH
	#define DST_BUFFER_ADDR	0x80800000	// place to put flash read image
	#endif
	
	if (ret == 0
		#ifdef CONFIG_BOOT_RESET_ENABLE
			&& !gCHKKEY_HIT
		#endif	
		) {
		printf("Checking bank2...\n");
	 	ret = check_image_header(pHeader, psetting_header, CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET); 
		if (ret) {
			unsigned long src_addr = return_addr - FLASH_BASE;			
			unsigned long length = CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET*2 - src_addr;
			#ifndef CONFIG_NAND_FLASH
			printf("Flash read from %X to %X with %X bytes ?\n",src_addr, DST_BUFFER_ADDR, length);
			flashread(DST_BUFFER_ADDR, src_addr, length);
		
			printf("Flash Program from %X to %X with %X bytes ?\n",DST_BUFFER_ADDR, src_addr-CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET, length);
			flashwrite(src_addr-CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET, DST_BUFFER_ADDR, length);
			#else
			printf("Flash read from %X to %X with %X bytes ?\n",src_addr, DRAM_DIMAGE_ADDR, length);
			nflashread(0, src_addr, length,0);
		
			printf("Flash Program from %X to %X with %X bytes ?\n",DRAM_DIMAGE_ADDR, src_addr-CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET, length);
			nflashwrite(src_addr-CONFIG_RTL_FLASH_DUAL_IMAGE_OFFSET, DRAM_DIMAGE_ADDR, length);
			#endif
			
			ret = check_image_header(pHeader,psetting_header,0); 
			if (ret == 0)
				printf("Bank1 image is still invalid after update from bank2!!\n");	
			else
				printf("Copy bank2 to bank1 successfully!\n");				
		}
		else 
			printf("Bank2 is corrupted!\n");
		
	}
	#endif
	
 	#else
       	ret = check_dualbank_setting(IN_BOOTING_MODE);
	#endif
#endif //end of NFBI else
	return ret;
}

//------------------------------------------------------------------------------------------
//monitor user interrupt
int pollingDownModeKeyword(int key)
{
	int i;
                 #if CONFIG_ESD_SUPPORT//patch for ESD
                  	 REG32(0xb800311c)|= (1<<23);
                  #endif
	if  (Check_UART_DataReady() )
	{
		i=Get_UART_Data();
		Get_UART_Data();
		if( i == key )
		{ 	
#if defined(UTILITY_DEBUG)		
			dprintf("User Press ESC Break Key\r\n");
#endif			
			gCHKKEY_HIT=1;
			return 1;
		}
	}
	return 0;
}
//------------------------------------------------------------------------------------------

#ifdef CONFIG_BOOT_RESET_ENABLE
int pollingPressedButton(int pressedFlag)
{
#ifndef CONFIG_NFBI
#ifndef CONFIG_FPGA_PLATFORM
		// polling if button is pressed --------------------------------------
    		if (pressedFlag == -1 ||  pressedFlag == 1) 
		{

#if defined(RTL8198)
	// vincent: already done in Init_GPIO(). do nothing here
	//		REG32(RTL_GPIO_MUX) =  0x0c0f;
	//		REG32(PEFGHCNR_REG) = REG32(PEFGHCNR_REG)& (~(1<<25) ); //set byte F GPIO7 = gpio
        //     		REG32(PEFGHDIR_REG) = REG32(PEFGHDIR_REG) & (~(1<<25) );  //0 input, 1 out
#endif
		
			if ( Get_GPIO_SW_IN() )			
			{// button pressed
#if defined(UTILITY_DEBUG)			
	    			dprintf("User Press GPIO Break Key\r\n");
#endif	    			
				if (pressedFlag == -1) 
				{
					//SET_TIMER(1*CPU_CLOCK); // wait 1 sec
				}
				pressedFlag = 1;
				gCHKKEY_HIT=1;
#if defined(UTILITY_DEBUG)				
				dprintf("User Press Break Button\r\n",__LINE__);
#endif
				return 1;	//jasonwang//wei add				

			}
			else
		      		pressedFlag = 0;
		}
#if defined(UTILITY_DEBUG)
	dprintf("j=%x\r\n",get_timer_jiffies());
#endif
#endif
#endif //CONFIG_NFBI

	return pressedFlag;
}
#endif
//------------------------------------------------------------------------------------------

//return 0: do nothing; 1: jump to down load mode; 3 jump to debug down load mode
int user_interrupt(unsigned long time)
{
	int i,ret;
	int tickStart=0;
#ifdef SUPPORT_TFTP_CLIENT
	extern int check_tftp_client_state();	
#endif
	
#ifdef CONFIG_BOOT_RESET_ENABLE
	int button_press_detected=-1;
#endif
	
	tickStart=get_timer_jiffies();
#ifdef  SUPPORT_TFTP_CLIENT
	do 
#endif
    {
		ret=pollingDownModeKeyword(ESC);
		if(ret == 1) return 1;
#ifdef CONFIG_BOOT_RESET_ENABLE		
		ret=pollingPressedButton(button_press_detected);
		button_press_detected=ret;
		if(ret > 0) return ret;
#endif		
	}while (
#ifdef SUPPORT_TFTP_CLIENT
	check_tftp_client_state() >= 0
#else
#if 0//def 	CONFIG_BOOT_RESET_ENABLE
	(get_timer_jiffies() - tickStart) < 100
#else
	0
#endif
#endif
	);  // 1 sec
#if defined(UTILITY_DEBUG)
	dprintf("timeout\r\n");
#endif	
#ifdef CONFIG_BOOT_RESET_ENABLE
	if (button_press_detected>0)
	{   
		gCHKKEY_HIT=1;    
		return 1;
	}
#endif	
	return 0;
}
//------------------------------------------------------------------------------------------



//------------------------------------------------------------------------------------------
//init gpio[96c not fix gpio, so close first. fix CPU 390MHz cannot boot from flash.]
void Init_GPIO()
{
#if 0  //rom code disable
#if defined(CONFIG_RTL8198)
#ifndef CONFIG_NFBI
#ifndef CONFIG_RTL8198
	REG32(PABCDCNR_REG) = REG32(PABCDCNR_REG)& (~(1<<5) ); //set byte F GPIO7 = gpio
	REG32(PABCDDIR_REG) = REG32(PABCDDIR_REG) & (~(1<<5) );  //0 input, 1 output, set F bit 7 input
	//modify for light reset led pin in output mode
	REG32(PABCDCNR_REG) = REG32(PABCDCNR_REG)& (~(1<<RESET_LED_PIN) ); 
	REG32(PABCDDIR_REG) = REG32(PABCDDIR_REG) | ((1<<RESET_LED_PIN) ); 
	REG32(PABCDDAT_REG) = REG32(PABCDDAT_REG) | ((1<<RESET_LED_PIN) );  
#else
	REG32(RTL_GPIO_MUX) =  0x0c0f;
	REG32(PEFGHCNR_REG) = REG32(PEFGHCNR_REG)& (~(1<<25) ); //set byte F GPIO7 = gpio
	REG32(PEFGHDIR_REG) = REG32(PEFGHDIR_REG) & (~(1<<25) );  //0 input, 1 output, set F bit 7 input
#endif	
#endif
#endif
#endif
}
//------------------------------------------------------------------------------------------
//-------------------------------------------------------
void console_init(unsigned long lexea_clock)
{
	int i;
	unsigned long dl;
	unsigned long dll;     
	unsigned long dlm;       


  	REG32(UART_LCR_REG)=0x03000000;		//Line Control Register  8,n,1
  			
  	REG32( UART_FCR_REG)=0xc7000000;		//FIFO Ccontrol Register
  	REG32( UART_IER_REG)=0x00000000;
  	dl = (lexea_clock /16)/BAUD_RATE-1;
  	//*(volatile unsigned long *)(0xa1000000) = dl ; 
  	dll = dl & 0xff;
  	dlm = dl / 0x100;
  	REG32( UART_LCR_REG)=0x83000000;		//Divisor latch access bit=1
  	REG32( UART_DLL_REG)=dll*0x1000000;
   	REG32( UART_DLM_REG)=dlm*0x1000000; 
    	REG32( UART_LCR_REG)=0x83000000& 0x7fffffff;	//Divisor latch access bit=0
   	//rtl_outl( UART_THR,0x41000000);	

	//dprintf("\n\n-------------------------------------------");
	//dprintf("\nUART1 output test ok\n");
}
//-------------------------------------------------------

void CmdEthStartup(int argc, char* argv[])
{
	eth_startup(0);	

	dprintf("\n---Ethernet init Okay!\n");
	sti();

#ifdef SUPPORT_TFTP_CLIENT	
	tftpd_entry(0);		
#else
	tftpd_entry();		
#endif
	return ;		
}

void goToDownMode()
{
#ifndef CONFIG_SW_NONE

	if(pollingDownModeKeyword('m')==0)
	{
		eth_startup(0);	

		dprintf("\n---Ethernet init Okay!\n");
		sti();

#ifdef SUPPORT_TFTP_CLIENT	
		tftpd_entry(0);
#else
		tftpd_entry();		
#endif

#ifdef DHCP_SERVER			
		dhcps_entry();
#endif
#ifdef HTTP_SERVER
		httpd_entry();
#endif

	}
#endif
	monitor();
	return ;
}
//-------------------------------------------------------

#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE		
void set_bankinfo_register()  //in order to notify kernel
{
#define SYSTEM_CONTRL_DUMMY_REG 0xb8003504
	prom_printf("return_addr = %x ,boot bank=%d, bank_mark=0x%x...\n",return_addr,boot_bank,bank_mark);	
	if(boot_bank == BANK2_BOOT)
		REG32(SYSTEM_CONTRL_DUMMY_REG) = (REG32(SYSTEM_CONTRL_DUMMY_REG) | 0x00000001); //mark_dul, issue use function is better
	//prom_printf("2SYSTEM_CONTRL_DUMMY_REG = %x",REG32(SYSTEM_CONTRL_DUMMY_REG));	
}			
#endif		
//-------------------------------------------------------

#if !defined(CONFIG_NONE_FLASH)
void goToLocalStartMode(unsigned long addr,IMG_HEADER_Tp pheader)
{
	unsigned short *word_ptr;
	void	(*jump)(void);
	int i;
	
	//prom_printf("\n---%X\n",return_addr);
#ifdef CONFIG_NAND_FLASH
	word_ptr = (unsigned short *)pheader;
	unsigned char *ptr_data = (volatile unsigned char *)DRAM_DIMAGE_ADDR;
	if(nflashread(0,addr,block_size,0)< 0){
		prom_printf("nand flash read fail,addr=%x,size=%d\n",addr,block_size);
		return 0;
	}
	
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
	{
		*word_ptr = ((*(ptr_data+1+i))|*(ptr_data+i)<<8)&0xffff;
	}			
#else

	word_ptr = (unsigned short *)pheader;
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
	*word_ptr = rtl_inw(addr + i);
#endif

	// move image to SDRAM
#if !defined(CONFIG_NONE_FLASH)	
#ifdef CONFIG_NAND_FLASH
	nflashread( pheader->startAddr|0x20000000,(unsigned int)(addr-FLASH_BASE+sizeof(IMG_HEADER_T)),pheader->len-2,0);
#else
	flashread( pheader->startAddr|0x20000000,	(unsigned int)(addr-FLASH_BASE+sizeof(IMG_HEADER_T)), pheader->len-2);
#endif
#endif		
	if ( !user_interrupt(0) )  // See if user escape during copy image
	{
		outl(0,GIMR0); // mask all interrupt
#if defined(CONFIG_BOOT_RESET_ENABLE)
		Set_GPIO_LED_OFF();
#endif

		prom_printf("Jump to image start=0x%x...\n", pheader->startAddr);
		
#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE
		set_bankinfo_register();
#endif
#ifdef CONFIG_RTK_BOOTINFO_SUPPORT
		rtk_inc_bootcnt();
#endif
		jump = (void *)(pheader->startAddr);

		 REG32(0xb8003114)=0;
  		REG32(0xb8000010)&=~(1<<11);
  		REG32(0xbbdc0300)=0xffffffff;
  		REG32(0xbbdc0304)=0xffffffff;
		cli();
		flush_cache(); 
		jump();				 // jump to start
		return ;
	}
	return;
}
#endif

//-------------------------------------------------------

#if 0
void debugGoToLocalStartMode(unsigned long addr,IMG_HEADER_Tp pheader)
{
	unsigned short *word_ptr;
	void	(*jump)(void);
	int i, count=500;

	//prom_printf("\n---%X\n",return_addr);
	word_ptr = (unsigned short *)pheader;
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
	*word_ptr = rtl_inw(addr + i);
			
	// move image to SDRAM
#if !defined(CONFIG_NONE_FLASH)	
	flashread( pheader->startAddr,	(unsigned int)(addr-FLASH_BASE+sizeof(IMG_HEADER_T)), 	pheader->len-2);
#endif			
	if ( !user_interrupt(0) )  // See if user escape during copy image
	{
		outl(0,GIMR0); // mask all interrupt
#ifdef CONFIG_BOOT_RESET_ENABLE
		Set_GPIO_LED_OFF();
#endif


		REG32(0xb8019004)=0xFE;
		while(count--)
		{continue;}
		
		if(REG32(0xb8019004)!=0xFE)
			prom_printf("fail debug-Jump to image start=0x%x...\n", pheader->startAddr);
		prom_printf("Debug-Jump to image start=0x%x...\n", pheader->startAddr);
		jump = (void *)(pheader->startAddr);
				
		cli();
		flush_cache(); 
		jump();				 // jump to start
	}
}
#endif
//-------------------------------------------------------
//set clk and init console	
void setClkInitConsole(void)
{
	console_init( glexra_clock);
//	printf("\n=>init console ok\n");
}
//-------------------------------------------------------
//init heap	
void initHeap(void)
{
#if defined(RTL8198)
	/* Initialize malloc mechanism */
	UINT32 heap_addr=((UINT32)dl_heap&(~7))+8 ;
	UINT32 heap_end=heap_addr+sizeof(dl_heap)-8;
  	i_alloc((void *)heap_addr, heap_end);
#endif
	cli();  	
	flush_cache(); // david
}
//-------------------------------------------------------

//-------------------------------------------------------
// init flash 
void initFlash(void)
{
#if defined(CONFIG_SPI_FLASH)
   	spi_probe();                                  
#endif  

#if defined CONFIG_NAND_FLASH_BOOTING ||defined(CONFIG_NAND_FLASH)
		rtk_nand_probe ();

#ifdef CONFIG_RTK_NAND_BBT	
		//dprintf("[%s:] %d NAND_ADDR_CYCLE = %d\n",__func__,__LINE__,NAND_ADDR_CYCLE);
		rtk_scan_v2r_bbt();
		rtk_nand_scan_bbt();
#endif

#endif
}
//-------------------------------------------------------
//rtk bootcode and enable post
//copy img to sdram and monitor ESC interrupt

#define RTL_8198C 0
#define RTL_DEFAULT 1
#define RTL_8198CD 0x2
#define RTL_8954E 0x3
#define RTL_8198CS 0x80
#define RTL_8954ES 0x83


void doBooting(int flag, unsigned long addr, IMG_HEADER_Tp pheader)
{
	unsigned int id = REG32(0xb800000c) & 0xff;
                #if CONFIG_ESD_SUPPORT//patch for ESD
                       REG32(0xb800311c)=0xa5000000 ;

                 #endif
				
#if 1//!(defined(CONFIG_NFBI)||defined(CONFIG_NONE_FLASH))
#ifdef SUPPORT_TFTP_CLIENT	
	extern int check_tftp_client_state();

	if(flag || check_tftp_client_state() >= 0)
#else
	if(flag)
#endif
	{
		switch(user_interrupt(WAIT_TIME_USER_INTERRUPT))
		{
		case LOCALSTART_MODE:
		default:
			if ((id==RTL_8198C)||(id==RTL_8198CS||(id==RTL_8954E)||(id==RTL_8954ES)))
			{
				SettingCPUClk(0xb,0);      //chg f1000MHz
			}
			else if (id==RTL_8198CD)
			{
				SettingCPUClk(3,0);      //chg f600MHz	
			}
			else
			{
				SettingCPUClk(0x7,0);      //chg f800MHz			
			}
			CmdCore1Wakeup(0,NULL);		
#ifdef SUPPORT_TFTP_CLIENT
			/* disable Ethernet switch */
			REG32(0xb8000010)= REG32(0xb8000010)&(~(1<<11));
			if (!flag) {
				REG32(GIMR_REG)=0x0;   //add by jiawenjian
				goToDownMode(); 	
			}	
#endif			
			goToLocalStartMode(addr,pheader);
			break;
			
		case DOWN_MODE:
		 	CmdCore1Wakeup(0,NULL);	
			dprintf("\n---Escape booting by user\n");	
			//cli();
			//REG32(GIMR_REG)=0x0;   //add by jiawenjian
			 #if CONFIG_ESD_SUPPORT//patch for ESD
                         REG32(0xb800311c)=0xa5000000 ;
        		 #endif
			#ifdef CONFIG_NAND_FLASH
			REG32(0xb8019004) = 0;
			#endif
			goToDownMode();	
			break;
		}/*switch case */
	}/*if image correct*/
	else
#endif //CONFIG_NFBI
	{
		CmdCore1Wakeup(0,NULL);		
		flush_cache();
		 #if CONFIG_ESD_SUPPORT//patch for ESD
                       REG32(0xb800311c)=0xa5000000 ;
                 #endif	
		//REG32(GIMR_REG)=0x0;   //add by jiawenjian
		#ifdef CONFIG_NAND_FLASH
		REG32(0xb8019004) = 0;
		#endif
		goToDownMode();		
	}
	return;
}
