/*
 * Copyright (C) 2011 Realtek Semiconductor Corp. 
 * All Rights Reserved.
 *
 * This program is the proprietary software of Realtek Semiconductor
 * Corporation and/or its licensors, and only be used, duplicated, 
 * modified or distributed under the authorized license from Realtek. 
 *
 * ANY USE OF THE SOFTWARE OTHER THAN AS AUTHORIZED UNDER 
 * THIS LICENSE OR COPYRIGHT LAW IS PROHIBITED. 
 *
 * Purpose : For linux kernel mode
 *           I/O read/write APIs using MDIO interface in the SDK.
 *
 * Feature : I/O read/write APIs, by access swich register by MDIO interface
 *
 */

#if defined(RTL_RLX_IO) || defined(LINUX_KERNEL_MDIO_IO) || defined(CYGWIN_MDIO_IO)
/*
 * Include Files
 */
#include <linux/kernel.h>
#include <common/error.h>
#include <common/util.h>
#include <ioal/ioal_init.h>
#include <ioal/io_mii.h>
#include <osal/time.h>
#include <osal/spl.h>

/*
 * Symbol Definition
 */
#define MII_ADDR  10

#define MII_REG0 0
#define MII_REG1 1
#define MII_REG2 2
#define MII_REG3 3
#define MII_REG4 4
#define MII_REG5 5
#define MII_REG6 6




#if defined(LINUX_KERNEL_SPI_IO)
    #include <ioal/io_spi.h>
    #define IO_MII_PHYREG_READ     io_spi_phyReg_read 
    #define IO_MII_PHYREG_WRITE    io_spi_phyReg_write
#else
extern void miiar_read(unsigned char phyaddr,unsigned char regaddr,unsigned short *value);
extern void miiar_write(unsigned char phyaddr,unsigned char regaddr,unsigned short value);

    #define IO_MII_PHYREG_READ     miiar_read 
    #define IO_MII_PHYREG_WRITE    miiar_write
#endif


/*
 * Data Declaration
 */
osal_spinlock_t mii_spinlock;
int32 mii_lock=0;
int32 lock_reentry=0;
/*
 * Macro Declaration
 */

void io_mii_init(void)
{
#if defined(LINUX_KERNEL_SPI_IO)    
    spi_init();
#endif    
}

void io_mii_deinit(void)
{
}

int io_mii_phy_reg_write(uint8 phy_id,uint8 reg, uint16 value)
{
    osal_spl_spin_lock(&mii_spinlock);

    IO_MII_PHYREG_WRITE(phy_id,reg,value);    
    osal_time_mdelay(10);

    osal_spl_spin_unlock(&mii_spinlock);
    return RT_ERR_OK;

}

int io_mii_phy_reg_read(uint8 phy_id,uint8 reg, uint16 *pValue)
{
    osal_spl_spin_lock(&mii_spinlock);

    IO_MII_PHYREG_READ(phy_id,reg,pValue); 

    osal_spl_spin_unlock(&mii_spinlock);
    return RT_ERR_OK;
}


int io_mii_memory_write(uint32 memaddr,uint32 data)
{
#if 0
    unsigned short value;
    value = (unsigned short)data;
    IO_MII_PHYREG_WRITE(8,(unsigned char) memaddr,value);    
    printk("\nio_mii_memory_write:0x%x value:0x%x\n",memaddr,value); 

    return RT_ERR_OK; 
#else
    unsigned short value;
    uint32 tmp_data;

    //printk("\nio_mii_memory_write:0x%x\n",memaddr);

    osal_spl_spin_lock(&mii_spinlock);

    /*address msb to REG1  lsb to REG0*/
    tmp_data = memaddr >>16;
    tmp_data = tmp_data&0xFFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG1,value);    


    tmp_data = memaddr & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG0,value);    

    /*data msb to REG3  lsb to REG2*/
    tmp_data = data & 0xFFFF0000;
    tmp_data = tmp_data>>16;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG3,value);    


    tmp_data = data & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG2,value);    


    /*set REG6 to 0x804b trigger write procedure*/
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG6,0x804B);    

    osal_spl_spin_unlock(&mii_spinlock);

    return RT_ERR_OK; 
#endif    
}

uint32 io_mii_memory_read(uint32 memaddr)
{
#if 0
   unsigned short value; 
   IO_MII_PHYREG_READ(8,(unsigned char) memaddr,&value); 

   printk("\nio_mii_memory_read:0x%x value:0x%x\n",memaddr,value);
   return value; 
#else
    unsigned short value;
    uint32 tmp_data;
 
    osal_spl_spin_lock(&mii_spinlock);

    /*address msb to REG1  lsb to REG0*/
    tmp_data = memaddr >>16;
    tmp_data = tmp_data&0xFFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG1,value);    

    tmp_data = memaddr & 0x0000FFFF;
    value = (unsigned short)tmp_data;
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG0,value);    

    /*set REG6 to 0x800b trigger read procedure*/
    IO_MII_PHYREG_WRITE(MII_ADDR,MII_REG6,0x800B);    


    /*read data from data msb from REG5  lsb from REG4*/
    tmp_data = 0;
    IO_MII_PHYREG_READ(MII_ADDR,MII_REG5,&value);
    tmp_data = value;
    tmp_data = tmp_data <<16;
    IO_MII_PHYREG_READ(MII_ADDR,MII_REG4,&value);
    tmp_data = tmp_data | value;
    
    osal_spl_spin_unlock(&mii_spinlock);

    return tmp_data; 


#endif
}

#endif

