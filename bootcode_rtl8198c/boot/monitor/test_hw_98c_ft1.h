

#include "../../autoconf.h"

typedef unsigned char			BOOLEAN,*PBOOLEAN;
#define IN
#define OUT
typedef unsigned int			u4Byte,*pu4Byte;


#ifndef _HW_TEST_8196D_FT2_H
#define _HW_TEST_8196D_FT2_H


#define KEYCODE_BS          0x08
#define KEYCODE_TAB         0x09
#define KEYCODE_ESC         0x1B
#define KEYCODE_SP          0x20
#define KEYCODE_CR          0x0D
#define KEYCODE_LF          0x0A


#define ChipIDCode_RTL8197D_176pin_EPAD 0x0
#define ChipIDCode_RTL8197D_176pin 0x8
#define ChipIDCode_RTL8196D_128pin 0x7
#define ChipIDCode_RTL8197DU_88pin 0xc
#define REG32_ANDOR(x,y,z)  { REG32(x)=( REG32(x) & (y) ) | (z);  }

int RTL8196D_FT2_TEST_entry();
int RTL8196D_FT1_TEST_entry();
unsigned int check_chip_id_code();

int Wait_L2_DONE(unsigned char ASCII_CODE);




#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM16(addr)         (*(volatile unsigned short *) (addr))
#define WRITE_MEM8(addr, val)    (*(volatile unsigned char *) (addr)) = (val)
#define READ_MEM8(addr)          (*(volatile unsigned char *) (addr))
#define REG32(reg) (*(volatile unsigned int *)(reg))



//For 819D6  DRAM
	#define DDCR_REG 0xb8001050
	#define MPMR_REG 0xB8001040
	#define MCR_REG 0xb8001000
	#define DCR_REG 0xb8001004
	#define DTR_REG 0xb8001008


	
	//For Crypto register
	#define CRYPTO_REG 0xb800c000


	//For system register
	#define ECO_NO_REG 0xb8000000
	
	#define HW_STRAP_REG 0xb8000008
	
	#define Bond_Option_REG 0xb800000c

	

	#define BIST_CONTROL_REG 0xb8000014
	#define PLL_REG 0xb8000020
	#define PIN_MUX_SEL_REG 0xb8000030



	                 /*GPIO register */
#define GPIO_BASE                           0xB8003500
                                                     /* Port ABCD control */
#define PABCDCNR_REG                            (0x000 + GPIO_BASE)
                                                     /* Port ABCD type */
#define PABCDPTYPE_REG                          (0x004 + GPIO_BASE)
                                                     /* Port ABCD direction */
#define PABCDDIR_REG                             (0x008 + GPIO_BASE)
                                                     /* Port ABCD data */
#define PABCDDAT_REG                             (0x00C + GPIO_BASE)
                                                     /* Port ABCD interrupt status */
#define PABCDISR_REG                             (0x010 + GPIO_BASE)
                                                     /* Port AB interrupt mask */
#define PABIMR_REG                               (0x014 + GPIO_BASE)
                                                     /* Port CD interrupt mask */
#define PCDIMR_REG                               (0x018 + GPIO_BASE)
                                                     /* Port ABCD control */
#define PEFGHCNR_REG                             (0x01C + GPIO_BASE)
                                                     /* Port ABCD type */
#define PEFGHPTYPE_REG                           (0x020 + GPIO_BASE)
                                                     /* Port ABCD direction */
#define PEFGHDIR_REG                             (0x024 + GPIO_BASE)
                                                     /* Port ABCD data */
#define PEFGHDAT_REG                             (0x028 + GPIO_BASE)
                                                     /* Port ABCD interrupt status */
#define PEFGHISR_REG                             (0x02C + GPIO_BASE)
                                                     /* Port AB interrupt mask */
#define PEFIMR_REG                               (0x030 + GPIO_BASE)
                                                     /* Port CD interrupt mask */
#define PGHIMR_REG                               (0x034 + GPIO_BASE)




#endif



#ifdef CONFIG_RTL8198C_FT2_TEST

/*

SW Version: 20110929-V1.0
	First released.
	Case5~1.

SW Version: 201300204-V1.1
Modified:
1.Add Case6:CKGEN
2.128 pin don't test external Port0	

*/

#define DLY_8196D_FT2_TIME 10                    //unit:ms

#define GPIO_B1_1 REG32(PABCDDAT_REG)|=0x200     //Output "1"
#define GPIO_B1_0 REG32(PABCDDAT_REG)&=0xFFFFFDFF//Output "0"

#define CONF_SKIP_BIST_HV 1
#define CONF_FT2_FAIL_STOP_TEST 0
#endif                                               //end of defined (CONFIG_RTL8881A_FT1_TEST)

