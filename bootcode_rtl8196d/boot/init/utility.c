#include "utility.h"
#include "rtk.h"

#include <asm/system.h>
#include <rtl8196x/asicregs.h>

//#define UTILITY_DEBUG 1
#define NEED_CHKSUM 1

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


unsigned long  glexra_clock=200*1000*1000;
//------------------------------------------------------------------------------------------
#if 0
int enable_10M_power_saving(int phyid , int regnum,int data)
{   
    	unsigned int uid,tmp;  
     	rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
     	uid=tmp;
     	uid =data;
    	rtl8651_setAsicEthernetPHYReg( phyid, regnum, uid );
    	rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
    	uid=tmp;
	return 0;
}
#endif
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

#if defined(CONFIG_OSK)
// Now check image on C000,E000,10000
static int check_image_header(IMG_HEADER_Tp pHeader,SETTING_HEADER_Tp psetting_header,unsigned long bank_offset)
{
	int i,ret=0;
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

	return ret;
}

// return,  0: not found, 1: OSK found
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
        /*check firmware image.*/
	word_ptr = (unsigned short *)pHeader;
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
		*word_ptr = rtl_inw(addr + i);	

	memcpy(image_sig, FW_SIGNATURE, SIG_LEN);

	if (!memcmp(pHeader->signature, image_sig, SIG_LEN))
		ret=1;
	else{
		prom_printf("no sys signature at %X!\n",addr-FLASH_BASE);
	}		
	//prom_printf("ret=%d  sys signature at %X!\n",ret,addr-FLASH_BASE);
	if (ret) {
		for (i=0; i<pHeader->len; i+=2) {
#if 1  //slowly
			if((i%0xa000) == 0)
			{
				gCHKKEY_CNT++;
				if( gCHKKEY_CNT>ACCCNT_TOCHKKEY)
				{	gCHKKEY_CNT=0;
					if ( user_interrupt(0)==1 )  //return 1: got ESC Key
					{
						//prom_printf("ret=%d  ------> line %d!\n",ret,__LINE__);
						return 0;
					}
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
#else
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
        /*check firmware image.*/
	word_ptr = (unsigned short *)pHeader;
	for (i=0; i<sizeof(IMG_HEADER_T); i+=2, word_ptr++)
		*word_ptr = rtl_inw(addr + i);	

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
	if (ret) {
		for (i=0; i<pHeader->len; i+=2) {
#if 1  //slowly
			if((i%0x10000) == 0)
			{
			gCHKKEY_CNT++;
			if( gCHKKEY_CNT>ACCCNT_TOCHKKEY)
			{	gCHKKEY_CNT=0;
				if ( user_interrupt(0)==1 )  //return 1: got ESC Key
				{
					//prom_printf("ret=%d  ------> line %d!\n",ret,__LINE__);
					return 0;
				}
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
	#define SIZE_OF_SQFS_SUPER_BLOCK 640
	#define SIZE_OF_CHECKSUM 2
	#define OFFSET_OF_LEN 2
	
	if(gCHKKEY_HIT==1)
		return 0;
	
	word_ptr = (unsigned short *)tmpbuf;
	for (i=0; i<16; i+=2, word_ptr++)
		*word_ptr = rtl_inw(addr + i);

	if ( memcmp(tmpbuf, SQSH_SIGNATURE, SIG_LEN) && memcmp(tmpbuf, SQSH_SIGNATURE_LE, SIG_LEN)) {
		prom_printf("no rootfs signature at %X!\n",addr-FLASH_BASE);
		return 0;
	}

	length = *(((unsigned long *)tmpbuf) + OFFSET_OF_LEN) + SIZE_OF_SQFS_SUPER_BLOCK + SIZE_OF_CHECKSUM;

	for (i=0; i<length; i+=2) {
#if 1  //slowly
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

#if defined(NEED_CHKSUM)		
	if ( sum ) {
		prom_printf("rootfs checksum error at %X!\n",addr-FLASH_BASE);
		return 0;
	}	
#endif	
	return 1;
#endif //CONFIG_RTK_VOIP
}
//------------------------------------------------------------------------------------------

static int check_image_header(IMG_HEADER_Tp pHeader,SETTING_HEADER_Tp psetting_header,unsigned long bank_offset)
{
	int i,ret=0;
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
                ret=check_rootfs_image((unsigned long)FLASH_BASE+ROOT_FS_OFFSET+bank_offset);
                if(ret==0)
                	ret=check_rootfs_image((unsigned long)FLASH_BASE+ROOT_FS_OFFSET+ROOT_FS_OFFSET_OP1+bank_offset);
                if(ret==0)
                	ret=check_rootfs_image((unsigned long)FLASH_BASE+ROOT_FS_OFFSET+ROOT_FS_OFFSET_OP1+ROOT_FS_OFFSET_OP2+bank_offset);

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
	}
	return ret;
}
//------------------------------------------------------------------------------------------
#endif
#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE	

int get_system_header(unsigned long addr, IMG_HEADER_Tp pImgHdr)
{
	unsigned short *word_ptr;
	char image_sig[4] = {0};
	char image_sig_root[4] = {0};
	int  i;

	word_ptr = (unsigned short *)pImgHdr;
	for(i = 0; i < sizeof(IMG_HEADER_T); i+=2, word_ptr++)
		*word_ptr = rtl_inw(addr +i);
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

//------------------------------------------------------------------------------------------

int check_image(IMG_HEADER_Tp pHeader,SETTING_HEADER_Tp psetting_header)
{
	int ret=0;
#if defined(CONFIG_NFBI) || defined(CONFIG_NONE_FLASH)
	prom_printf("---NFBI or ROM booting---\n");
#else
 	//only one bank
 #ifndef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE	
 	ret=check_image_header(pHeader,psetting_header,0); 
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
	#ifdef CONFIG_RTL8196D
			//REG32(RTL_GPIO_MUX) = (REG32(RTL_GPIO_MUX) & ~(0x7))|0x6;
	#endif
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
	
#ifdef CONFIG_BOOT_RESET_ENABLE
	int button_press_detected=-1;
#endif
	
	tickStart=get_timer_jiffies();
	
	do {
		ret=pollingDownModeKeyword(ESC);
		if(ret == 1) return 1;
#ifdef CONFIG_BOOT_RESET_ENABLE		
		ret=pollingPressedButton(button_press_detected);
		button_press_detected=ret;
		if(ret > 0) return ret;
#endif		
	}while (
#ifdef 	CONFIG_BOOT_RESET_ENABLE
	(get_timer_jiffies() - tickStart) < 100
#else
	0
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
#if (defined(CONFIG_RTL8196D) || defined(CONFIG_RTL8196E))  && !defined(CONFIG_NFBI)
	#ifdef CONFIG_USING_JTAG
	REG32(RTL_GPIO_MUX) = (REG32(RTL_GPIO_MUX) & ~(0x7))|0x1;//This setting will cause Linux booting fail due to JTAG reset pin 
	#else
	REG32(RTL_GPIO_MUX) = (REG32(RTL_GPIO_MUX) & ~(0x7))|0x6;
	#endif
	REG32(PABCDCNR_REG) = REG32(PABCDCNR_REG)& (~(1<<5) ); //set byte F GPIO7 = gpio
	REG32(PABCDDIR_REG) = REG32(PABCDDIR_REG) & (~(1<<5) );  //0 input, 1 output, set F bit 7 input
#else
	REG32(RTL_GPIO_MUX) =  0x0c0f;
	REG32(PEFGHCNR_REG) = REG32(PEFGHCNR_REG)& (~(1<<25) ); //set byte F GPIO7 = gpio
	REG32(PEFGHDIR_REG) = REG32(PEFGHDIR_REG) & (~(1<<25) );  //0 input, 1 output, set F bit 7 input
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
  	*(volatile unsigned long *)(0xa1000000) = dl ; 
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


void goToDownMode()
{
#ifndef CONFIG_SW_NONE


		eth_startup(0);	

		dprintf("\n---Ethernet init Okay!\n");
		sti();

		tftpd_entry();
		
#ifdef DHCP_SERVER			
		dhcps_entry();
#endif
#ifdef HTTP_SERVER
		httpd_entry();
#endif

#endif
	monitor();
	return ;
}
//-------------------------------------------------------

#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE		
void set_bankinfo_register()  //in order to notify kernel
{
#define SYSTEM_CONTRL_DUMMY_REG 0xb8000068
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
#if defined(CONFIG_BOOT_RESET_ENABLE)
		Set_GPIO_LED_OFF();
#endif

		prom_printf("Jump to image start=0x%x...\n", pheader->startAddr);
		
#ifdef CONFIG_RTL_FLASH_DUAL_IMAGE_ENABLE
		set_bankinfo_register();
#endif
		jump = (void *)(pheader->startAddr);

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
	REG32(MCR_REG)=REG32(MCR_REG)|(1<<27);  //new prefetch

	#if defined(CONFIG_NFBI)
    	//clear all bits of SYSSR except bit 5, 6, 7, 11
    	REG32(NFBI_SYSSR)= REG32(NFBI_SYSSR) & 0x08e0;
	#endif

	console_init( glexra_clock);
}
//-------------------------------------------------------
//init heap	
void initHeap(void)
{
#if  defined(RTL8198)
	/* Initialize malloc mechanism */
	UINT32 heap_addr=((UINT32)dl_heap&(~7))+8 ;
	UINT32 heap_end=heap_addr+sizeof(dl_heap)-8;
  	i_alloc((void *)heap_addr, heap_end);
#endif
	cli();  	
	flush_cache(); // david
}
//-------------------------------------------------------
// init interrupt 
void initInterrupt(void)
{
	rtl_outl(GIMR0,0x00);/*mask all interrupt*/
	setup_arch();    /*setup the BEV0,and IRQ */
	exception_init();/*Copy handler to 0x80000080*/
	init_IRQ();      /*Allocate IRQfinder to Exception 0*/
	sti();
}
//-------------------------------------------------------
// init flash 
void initFlash(void)
{
#if defined(CONFIG_SPI_FLASH)
   	spi_probe();                                    //JSW : SPI flash init		
#endif  
#if defined(CONFIG_NOR_FLASH)
	flashinit();
#endif
}
//-------------------------------------------------------
//rtk bootcode and enable post
//copy img to sdram and monitor ESC interrupt

void doBooting(int flag, unsigned long addr, IMG_HEADER_Tp pheader)
{
#if !(defined(CONFIG_NFBI)||defined(CONFIG_NONE_FLASH))
	if(flag)
	{

		switch(user_interrupt(WAIT_TIME_USER_INTERRUPT))
		{
		case LOCALSTART_MODE:
		default:
			goToLocalStartMode(addr,pheader);			
		case DOWN_MODE:
			dprintf("\n---Escape booting by user\n");	
			//cli();
		REG32(GIMR_REG)=0x0;   //add by jiawenjian

			goToDownMode();	
			break;
		}/*switch case */
	}/*if image correct*/
	else
#endif //CONFIG_NFBI
	{
		REG32(GIMR_REG)=0x0;   //add by jiawenjian
		goToDownMode();		
	}
	return;
}
