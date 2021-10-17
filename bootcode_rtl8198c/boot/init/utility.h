#include <linux/types.h>
#include "rtk.h"
#if defined(RTL8198)
#include <asm/rtl8198.h>
#endif
#include "etherboot.h"


/* Setting image header */
typedef struct _setting_header_ {
	Int8  Tag[2];
	Int8  Version[2];
	Int16 len;
} SETTING_HEADER_T, *SETTING_HEADER_Tp;

//------------------------------------------------------------------------------------------
#define BAUD_RATE	  		(38400)  
#define ACCCNT_TOCHKKEY 	(128*1024)  //128K
unsigned long return_addr;
#define WAIT_TIME_USER_INTERRUPT	(3*CPU_CLOCK)

#define LOCALSTART_MODE 0
#define DOWN_MODE 1
#define DEBUG_LOCALSTART_MODE 3

/*Cyrus Tsai*/
unsigned long kernelsp;
#if  defined(RTL8198)
#ifdef CONFIG_NAND_FLASH_BOOTING
	#define _SYSTEM_HEAP_SIZE	1024*64*4	//wei add
#else
	#define _SYSTEM_HEAP_SIZE	1024*64	//wei add
#endif
	char dl_heap[_SYSTEM_HEAP_SIZE];	//wei add
#endif


//check
#define HS_IMAGE_OFFSET		(24*1024)	//0x6000
#define DS_IMAGE_OFFSET		(25*1024)	//0x6400
#define CS_IMAGE_OFFSET		(32*1024)	//0x8000

#define CODE_IMAGE_OFFSET		(64*1024)	//0x10000
#define CODE_IMAGE_OFFSET2	(128*1024)	//0x20000
#define CODE_IMAGE_OFFSET3	(192*1024)	//0x30000
#define CODE_IMAGE_OFFSET4	(0x8000)

//flash mapping
#define ROOT_FS_OFFSET		(0xE0000)
#define ROOT_FS_OFFSET_OP1		(0x10000)
#define ROOT_FS_OFFSET_OP2		(0x40000)

#if defined(CONFIG_WEBPAGE_BACKUP)
#define WEBPAGE_OFFSET		(0x10000)
#define WEBPAGE_OFFSET_OP1	(0x20000)
#define WEBPAGE_OFFSET_OP2	(0x30000)
#endif



int enable_10M_power_saving(int phyid , int regnum,int data);
int user_interrupt(unsigned long time);

#if defined( CONFIG_NFBI) || defined(CONFIG_NONE_FLASH)
// return,  0: not found, 1: linux found, 2:linux with root found
int check_system_image(unsigned long addr,IMG_HEADER_Tp pHeader);
#else
int check_system_image(unsigned long addr,IMG_HEADER_Tp pHeader,SETTING_HEADER_Tp setting_header);
int check_rootfs_image(unsigned long addr);
#endif
//------------------------------------------------------------------------------------------

//ddr
#define DDR_DBG 0
#define PROMOS_DDR_CHIP 1
#define IMEM_DDR_CALI_LIMITS 60 //1sec=30 times
#define __IRAM_IN_865X      __attribute__ ((section(".iram-rtkwlan")))
#define __IRAM_FASTEXTDEV        __IRAM_IN_865X



//------------------------------------------------------------------------------------------
//need move to regs.h
#if defined(RTL8198)
#define RTL_GPIO_MUX 0xb8000040
#endif
#define RTL_GPIO_MUX_DATA 			0x00340000//for WIFI ON/OFF and GPIO

#define RTL_W32(addr, l)			((*(volatile unsigned long*)(addr)) = (l))
#define RTL_R32(addr)				(*(volatile unsigned long *)(addr))

#define GPIO_BASE       0xB8003500
#define PABCD_CNR       (GPIO_BASE + 0x00)
#define PABCD_DIR       (GPIO_BASE + 0x08)
#define PABCD_DAT       (GPIO_BASE + 0x0C)

#define PEFGH_CNR		(GPIO_BASE + 0x1C)
#define PEFGH_DIR		(GPIO_BASE + 0x24)
#define PEFGH_DAT		(GPIO_BASE + 0x28)

#define RTL_GPIO_MUX4   	0xB800010C
#define RTL_GPIO_MUX5   	0xB8000110


#ifdef CONFIG_BOOT_RESET_ENABLE
	#define RTL_GPIO_RESET_BTN_MUX	(3<<23)
	#define RESET_BTN_PIN			29
	
	#define RESET_PIN_IOBASE    PEFGH_CNR 	
	#define RESET_PIN_DIRBASE   PEFGH_DIR 
	#define RESET_PIN_DATABASE  PEFGH_DAT
	
#endif


  #define Check_UART_DataReady() (rtl_inl(UART_LSR) & (1<<24))
  #define Get_UART_Data() ((rtl_inl(UART_RBR) & 0xff000000)>>24)

	

	
	


#if  defined(RTL8198)		
	//#define Get_GPIO_SW_IN() (!((REG32(PEFGHDAT_REG) & (1<<25))>>25) )  //return 0 if non-press	
	#define Get_GPIO_SW_IN() (!(RTL_R32(RESET_PIN_DATABASE) & (1 << RESET_BTN_PIN)))
#endif


#if  defined(RTL8198)	
	//#define REG32(reg) (*(volatile unsigned int *)(reg))
	#define SYS_BASE 0xb8000000
	#define SYS_HW_STRAP (SYS_BASE +0x08)
#endif
//------------------------------------------------------------------------------------------
//gpio
#if defined(RTL8198)
#define RESET_LED_PIN 24
#endif



#if  defined(RTL8198)
#define Set_GPIO_LED_ON()       (REG32(PEFGHDAT_REG) =  REG32(PEFGHDAT_REG)  & (~(1<<RESET_LED_PIN)) )
#define Set_GPIO_LED_OFF()      (REG32(PEFGHDAT_REG) =  REG32(PEFGHDAT_REG)  | (1<<RESET_LED_PIN))
#endif
void Init_GPIO();

