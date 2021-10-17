#include <linux/module.h>
#include <linux/init.h>
#include <linux/version.h>
#include <asm/delay.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include "i2c_gpio.h"

extern void wifi_audio_nl_init(void);
extern void wifi_audio_nl_exit(void);
#define kMFiAuthReg_ErrorCode								0x05
#define kMFiAuthReg_AuthControlStatus				0x10
	#define kMFiAuthFlagError									0x80
	#define kMFiAuthControl_GenerateSignature	1
#define kMFiAuthReg_SignatureSize						0x11
#define kMFiAuthReg_SignatureData						0x12
#define kMFiAuthReg_ChallengeSize						0x20
#define kMFiAuthReg_ChallengeData						0x21
#define kMFiAuthReg_DeviceCertificateSize		0x30
#define kMFiAuthReg_DeviceCertificateData1	0x31 // Note: auto-increments so next read is Data2, Data3, etc.


#define FIRST_MINOR 0
#define MINOR_CNT 	1
#define EU_VE3 4
#define E_VE3 7
#if defined(CONFIG_RTL_8196E)
extern unsigned int RTL96E_BOND;
#endif
static dev_t dev;
static struct cdev c_dev;
static struct class *cl;


typedef unsigned int uint32;
typedef int			int32;

/* define GPIO direction */
enum GPIO_DIRECTION
{
	GPIO_DIR_IN = 0,
	GPIO_DIR_OUT =1,
};

/* define GPIO Interrupt Type */
enum GPIO_INTERRUPT_TYPE
{
	GPIO_INT_DISABLE = 0,
	GPIO_INT_ENABLE,

};

int DebugEnable = 0;
int SD_MODE = 1;
unsigned char cur_opt_address;
unsigned char *kbuf;
uint8_t mfi_ret_data[2]={0};

static void __exit cleanup_procfs_i2cG(void);
static void rtl8196e_init_reg_gpio( void );
static int  __init rtl8196e_init_procfs(void);

int32 __i2c_initGpioPin( uint32 gpioId, enum GPIO_DIRECTION direction, 
                                           enum GPIO_INTERRUPT_TYPE interruptEnable );
int32 __i2c_setGpioDataBit( uint32 gpioId, uint32 data );
int32 __i2c_getGpioDataBit( uint32 gpioId, uint32* pData );

unsigned int serial_out_i2c(unsigned char addr, int len,  unsigned char * buf);
unsigned int serial_in_i2c(unsigned char addr, int len);

void i2c_start_condition(i2c_dev_t* pI2C_Dev);
void i2c_stop_condition(i2c_dev_t* pI2C_Dev);

#define FAILED	 -1
#define SUCCESS		0
#define ENODEV		19      /* No such device */

#if defined( LINUX_VERSION_CODE ) && (LINUX_VERSION_CODE == KERNEL_VERSION(2,6,30))
#define LINUX_KERNEL_VERSION_2_6_30		1
#endif




#define I2C_GPIO_ID( port, pin )		( ( ( port - 'A' ) << 16 ) | ( pin ) )

#define MFi3959_SCL 		I2C_GPIO_ID( 'B', 6 )	// SCL = B6 (1 00000000 00000000)|0110 = 1 00000000 00000110
#define MFi3959_SDA 		I2C_GPIO_ID( 'A', 4 )	// SDA = A4 (  00000000 00000000)|0100 =   00000000 00000100

#define MFi_CP_I2C_ADDR_WRITE	0x22
#define MFi_CP_I2C_ADDR_READ	0x23


/*************** Define RTL8196EU GPIO Register Set ************************/
#define	PIN_MUX_SEL		0xB8000040
#define PIN_MUX_SEL_2 0xB8000044

#define	GPABCDCNR	0xB8003500
#define	GPABDIR		0xB8003508
#define	GPABDATA	0xB800350c
#define	GPABISR		0xB8003510
#define	GPABIMR		0xB8003514
#define	GPCDIMR		0xB8003518

#define	GPEFGHCNR	0xB800351C
#define	GPEFDIR		0xB8003524
#define	GPEFDATA	0xB8003528
#define	GPEFISR		0xB800352C
#define	GPEFIMR		0xB8003530
#define	GPGHIMR		0xB8003534



/* Register access macro (REG*()).*/

#define REG32(reg) 			(*((volatile uint32 *)(reg)))
#define REG16(reg) 			(*((volatile uint16 *)(reg)))
#define REG8(reg) 			(*((volatile uint8 *)(reg)))
#define RTL_W32(addr, l)		((*(volatile unsigned long*)(addr)) = (l))


static i2c_dev_t MFi3959_i2c_dev = {
	.sclk	= MFi3959_SCL,
	.sdio	= MFi3959_SDA,
};

#define FOOBAR_LEN 8

struct fb_data_t {
        char name[FOOBAR_LEN + 1];
        char value[FOOBAR_LEN + 1];
};
struct fb_data_t i2cG_data;


#define GPIO_PORT(id) (id>>16)
#define GPIO_PIN(id) (id&0xffff)

static uint32 bitStartGpioDirectionRead[] =
{
	0, 			/* Port A */
	8, 			/* Port B */
	16,  		/* Port C */
	24, 		/* Port D */
	0, 			/* Port E */
	8, 			/* Port F */	// according to modified Spec. 
	16, 		/* Port G */
	24			/* Port H */
};

enum GPIO_PORT
{
	GPIO_PORT_A = 0,
	GPIO_PORT_B,
	GPIO_PORT_C,
	GPIO_PORT_D,
	GPIO_PORT_E,
	GPIO_PORT_F,
	GPIO_PORT_G,
	GPIO_PORT_MAX,
};

static uint32 bitStartGpioDataRead[] =
{
	0, 			/* Port A */
	8, 			/* Port B */
	16,  		/* Port C */
	24, 		/* Port D */
	0, 			/* Port E */
	8, 			/* Port F */
	16, 		/* Port G */
	24			/* Port H */
};

static uint32 regGpioInterruptStatusRead[] =
{
	GPABISR, 	/* Port A */
	GPABISR, 	/* Port B */
	GPABISR, 	/* Port A */
	GPABISR, 	/* Port B */
	GPEFISR,
	GPEFISR,
	GPEFISR,
	GPEFISR
};

enum GPIO_FUNC	
{
	GPIO_FUNC_DIRECTION,
	GPIO_FUNC_DATA,
	GPIO_FUNC_INTERRUPT_STATUS,
	GPIO_FUNC_INTERRUPT_ENABLE,
	GPIO_FUNC_MAX,
};

//#define _GPIO_DEBUG_

#ifdef _GPIO_DEBUG_ 
#if defined(CONFIG_RTK_VOIP_DRIVERS_PCM8651)
#define GPIO_PRINT(level, fmt, args...) do { if (gpio_debug >= level) rtlglue_printf(fmt, ## args); } while (0)
#else
#define GPIO_PRINT(level, fmt, args...) do { if (gpio_debug >= level) printk(fmt, ## args); } while (0)
#endif
#else
#define GPIO_PRINT(fmt, args...)
#endif


static uint32 regGpioDirectionRead[] =
{
	GPABDIR, 	/* Port A */
	GPABDIR, 	/* Port B */
	GPABDIR, 	/* Port C */
	GPABDIR, 	/* Port D */
	GPEFDIR,	/* Port E */
	GPEFDIR,	/* Port F */
	GPEFDIR,	/* Port G */
	GPEFDIR		/* Port H */
};

static uint32 bitStartGpioInterruptStatusRead[] =
{
	0, 			/* Port A */
	16, 			/* Port B */
	0,  			/* Port C */
	16, 			/* Port D */
	0, 			/* Port E */
	16, 			/* Port F */
	0, 			/* Port G */
	16, 			/* Port H */
};


static uint32 regGpioDirectionWrite[] =
{
	GPABDIR, 	/* Port A */
	GPABDIR, 	/* Port B */
	GPABDIR, 	/* Port C */
	GPABDIR, 	/* Port D */
	GPEFDIR,	/* Port E */
	GPEFDIR,	/* Port F */
	GPEFDIR,	/* Port G */
	GPEFDIR		/* Port H */
};

static uint32 regGpioDataRead[] =
{
	GPABDATA, 	/* Port A */
	GPABDATA, 	/* Port B */
	GPABDATA, 	/* Port C */
	GPABDATA, 	/* Port D */
	GPEFDATA, 	/* Port E */
	GPEFDATA, 	/* Port F */
	GPEFDATA, 	/* Port G */
	GPEFDATA, 	/* Port H */
};


static uint32 bitStartGpioDirectionWrite[] =
{
	0, 			/* Port A */
	8, 			/* Port B */
	16,  		/* Port C */
	24, 		/* Port D */
	0, 			/* Port E*/
	8, 			/* Port F */
	16,  		/* Port G */
	24, 		/* Port H */    
};

static uint32 regGpioDataWrite[] =
{
	GPABDATA, 	/* Port A */
	GPABDATA, 	/* Port B */
	GPABDATA, 	/* Port C */
	GPABDATA, 	/* Port D */
	GPEFDATA, 	/* Port E */
	GPEFDATA, 	/* Port F */
	GPEFDATA, 	/* Port G */
	GPEFDATA, 	/* Port H */
};

/* need check */
static uint32 bitStartGpioInterruptEnableRead[] =
{
	0, 			/* Port A */
	16,  			/* Port B */
	0,			/* Port C */
	16, 			/* Port D */
	0,  			/* Port E */
	16, 			/* Port F */
	0,  			/* Port G */
	16
};

static uint32 regGpioInterruptEnableRead[] =
{
	GPABIMR,	/* Port A */
	GPABIMR,	/* Port B */
	GPCDIMR, 	/* Port C */
	GPCDIMR,	/* Port D */
	GPEFIMR,
	GPEFIMR,
	GPGHIMR,
	GPGHIMR

};

static uint32 bitStartGpioDataWrite[] =
{
	0, 			/* Port A */
	8, 			/* Port B */
	16,  		/* Port C */
	24, 		/* Port D */
	0, 			/* Port E */
	8, 			/* Port F */
	16,  		/* Port G */
	24, 		/* Port H */
};

static uint32 regGpioInterruptEnableWrite[] =
{
	GPABIMR,	/* Port A */
	GPABIMR,	/* Port B */
	GPCDIMR, 	/* Port C */
	GPCDIMR,	/* Port D */
	GPEFIMR,
	GPEFIMR,
	GPGHIMR,
	GPGHIMR

};

static uint32 bitStartGpioInterruptEnableWrite[] =
{
	0, 			/* Port A */
	16,  			/* Port B */
	0,			/* Port C */
	16, 			/* Port D */
	0,  			/* Port E */
	16, 			/* Port F */
	0,  			/* Port G */
	16				/* Port H */
};

static uint32 regGpioInterruptStatusWrite[] =
{
	GPABISR, 	/* Port A */
	GPABISR, 	/* Port B */
	GPABISR, 	/* Port C */
	GPABISR, 	/* Port D */
	GPEFISR,/* Port E */
	GPEFISR,/* Port F*/
	GPEFISR,/* Port G*/
	GPEFISR	/* Port H*/

};

static uint32 bitStartGpioInterruptStatusWrite[] =
{
	0, 			/* Port A */
	0, 			/* Port B */
	0,  			/* Port C */
	0, 			/* Port D */
	0, 			/* Port E */
	0, 			/* Port F */
	0, 			/* Port G */
};

static size_t mfi_write(struct file * file, const char __user *buffer, size_t count, loff_t *offset) 
{ 
	//int len; 
  unsigned char *buf=NULL;
  size_t			signatureLen=0;

	  
	//panic_printk( "\nkernel write receive len %x\n", count); 
	  
	if ( (buf=kmalloc( sizeof(unsigned char)* count,GFP_KERNEL) ) == NULL ) {
		panic_printk( "\nENOMEM \n"); 
		return -ENOMEM; 
	}

 // len = count < MAX_BUF ? count : MAX_BUF; 
 
 	if ( copy_from_user( (void*)(buf), (void*)buffer, count ) != 0 ){ 
 		printk( "Copy from user Error!\n" ); 
 		return -ENOMEM; 
 	} 

	//printk( "kernel driver mfi write buf 0x%2x len 0x%x 0x%x\n", buf[0], buf[1], buf[2] ); 

  if(count == 1)
  	cur_opt_address = *buf;
  else if (buf[0] == kMFiAuthReg_AuthControlStatus){
  	if(serial_out_i2c(buf[0], 1, &buf[1]) == 0){
  		if(buf != NULL)
			kfree(buf);
  		return -1;
  	}
  }else if (buf[0] == kMFiAuthReg_ChallengeSize)
  {  	
  	signatureLen = buf[1];
  	signatureLen = (signatureLen << 8) | buf[2];  	
  	if(serial_out_i2c(buf[0], 2 , &buf[1]) == 0){
  		if(buf != NULL)
			kfree(buf);
  		return -1;
  	}
  	if(serial_out_i2c(buf[0]+1, signatureLen , &buf[3]) == 0){
  		if(buf != NULL)
  			kfree( buf ); 
  		return -1;
  	}
  }

	kfree( buf ); 
 	return count; 
}

static size_t mfi_read(struct file * file,  char __user * buffer, size_t count, loff_t * offset) 
{ 
 
		
        //panic_printk( "kernel driver mfi read count 0x%x cur_opt_address 0x%2x\n", count ,cur_opt_address); 
	 	if( (cur_opt_address == kMFiAuthReg_AuthControlStatus) || cur_opt_address == kMFiAuthReg_ErrorCode ){
			if(serial_in_i2c(cur_opt_address,1) == 0)
				return -1;
		}else if(cur_opt_address == kMFiAuthReg_SignatureSize){
	 		if(serial_in_i2c(cur_opt_address,2) == 0)
	 			return -1;
	 	}else if(cur_opt_address == kMFiAuthReg_SignatureData){
	 		if(serial_in_i2c(cur_opt_address,count) == 0)
	 			return -1;
	 	}else if(cur_opt_address == kMFiAuthReg_DeviceCertificateSize){
	 		if(serial_in_i2c(cur_opt_address,count)==0)
	 			return -1;
	 	}else if(cur_opt_address == kMFiAuthReg_DeviceCertificateData1){
	 		if(serial_in_i2c(cur_opt_address,count) == 0)
	 			return -1;
	 	}
	 		
	 cur_opt_address=0x0;
		//printk( "\nKernel:mfi_read  before copy to user!\n" ); 
	 if(count <= 2){		
	 	//printk( "\nKernel:mfi_read count <= 2  before copy to user!\n" ); 
		 if ( copy_to_user( (void*)buffer, (void*)mfi_ret_data , count ) != 0 ){ 
			 printk( "Copy to User Error!\n" ); 
			 return -1; 
		 } 
	 }else{
	 	//printk( "\nKernel:mfi_read count > 2  before copy to user! count %d bufval %x\n",count ,kbuf[count-1]); 
	 	if ( copy_to_user( (void*)buffer, (void*)kbuf , count ) != 0 ){ 
			 printk( "Copy to User Error!\n" ); 
			 return -1; 
		 } 	
		 kfree(kbuf);
	 }	  
	 //panic_printk( "\nKernel:finish mfi_read!\n" ); 
	 return count; 
}

static int mfi_open(struct inode *i, struct file *f)
{
		rtl8196e_init_reg_gpio();
    return 0;
}
static int mfi_close(struct inode *i, struct file *f)
{
    return 0;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
static int mfi_ioctl(struct inode *i, struct file *f, unsigned int cmd, unsigned long arg)
#else
static long mfi_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
#endif
{
    mfi_arg_t p;
 
    switch (cmd)
    {
        case MFi_GET_VARIABLES:

            if (copy_to_user((mfi_arg_t *)arg, &p, sizeof(mfi_arg_t)))
            {
                return -EACCES;
            }
            break;
        case MFi_CLR_VARIABLES:

            break;
        case MFi_SET_VARIABLES:
            if (copy_from_user(&p, (mfi_arg_t *)arg, sizeof(mfi_arg_t)))
            {
                return -EACCES;
            }

            break;
        default:
            return -EINVAL;
    }
 
    return 0;
}

static struct file_operations mfi_fops =
{
    .owner = THIS_MODULE,
    .open = mfi_open,
    .read = mfi_read, 
    .write = mfi_write, 
    .release = mfi_close,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,35))
    .ioctl = mfi_ioctl
#else
    .unlocked_ioctl = mfi_ioctl
#endif
};


static int __init mfi_ioctl_init(void)
{
    int ret;
    struct device *dev_ret;
 
    printk("mfi_ioctl_init start\n");
    rtl8196e_init_procfs();
    if ((ret = alloc_chrdev_region(&dev, FIRST_MINOR, MINOR_CNT, "mfi_cp")) < 0)
    {
        printk("mfi_ioctl_init error!, ret=%d\n",ret);
        return ret;
    }
 
    cdev_init(&c_dev, &mfi_fops);
 
    if ((ret = cdev_add(&c_dev, dev, MINOR_CNT)) < 0)
    {
        return ret;
    }
     
    if (IS_ERR(cl = class_create(THIS_MODULE, "char")))
    {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(cl);
    }
    if (IS_ERR(dev_ret = device_create(cl, NULL, dev, NULL, "cp")))
    {
        class_destroy(cl);
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, MINOR_CNT);
        return PTR_ERR(dev_ret);
    }
	wifi_audio_nl_init();
    printk("mfi_ioctl_init succeed\n");
    return 0;
}


static void __exit mfi_ioctl_exit(void)
{
		cleanup_procfs_i2cG();
    device_destroy(cl, dev);
    class_destroy(cl);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, MINOR_CNT);
    wifi_audio_nl_exit();
}

/************************* I2C read/write function ************************/
void i2c_serial_write_byte(i2c_dev_t* pI2C_Dev, unsigned char data)
{

	char j;
	
	
	for (j=7;j>=0;j--) {
		if(DebugEnable)			
		printk("transfer bit %d\n",j);		
		__i2c_setGpioDataBit( pI2C_Dev->sdio, ((data)>>j)& 0x01/*0x00000001*/);//write data,from MSB to LSB
		if(SD_MODE)
			//ndelay(1450);
			ndelay(10000);
		else
			ndelay(600);
		__i2c_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
		//delay 20 us.
		
		if(SD_MODE)	
			ndelay(10000);
		else		
			ndelay(600);
		
		__i2c_setGpioDataBit( pI2C_Dev->sclk, 0);  /* fall down sclk*/
		//delay 10 us.
		
		if(SD_MODE)		
			ndelay(10000);
		else
			ndelay(700);

	}	
	
	return;
}


unsigned char i2c_ACK(i2c_dev_t* pI2C_Dev)	
{
#ifdef __test_program_used__
	int i;
#endif
	unsigned int buf=0;
//	__i2c_setGpioDataBit( pI2C_Dev->sdio, 0);	
//	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0);	
	
//  udelay(10);
  
	__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_IN, GPIO_INT_DISABLE);//change sdio to input
	
	__i2c_getGpioDataBit(pI2C_Dev->sdio,&buf);
	
//udelay(150);
	
	//delay 10 us.
	if(SD_MODE)
			//ndelay(1450);
			ndelay(10000);
		else
			ndelay(600);
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/	
	
	
		if(SD_MODE)	
			ndelay(10000);
		else		
			ndelay(600);	
	
	
	
	if(DebugEnable)
	printk("finish getGpioDataBi\n");
	//__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_INT_DISABLE);//change sdio to output
//	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	

	
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0);  /* fall down sclk*/
	
	if (buf != 0 && DebugEnable) 
		printk("NO ACK\n");
	
	//delay 10 us.
		
		if(SD_MODE)			
			ndelay(10000);
		else
			ndelay(700);
			
	
	
	return buf;
}


void i2c_ACK_w(i2c_dev_t* pI2C_Dev, unsigned char data)	
{
#ifdef __test_program_used__
	int i;
#endif

#if 0
	__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	__i2c_setGpioDataBit( pI2C_Dev->sdio, data); /* sdio send 0 for ACK, 1 for NACK*/
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
#endif
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/

	
	__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	__i2c_setGpioDataBit( pI2C_Dev->sdio, data); /* sdio send 0 for ACK, 1 for NACK*/

	if(SD_MODE)
			udelay(10);
		else
			ndelay(400);
	
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	//delay 10 us.
	
		if(SD_MODE)
			udelay(10);
		else		
			ndelay(600);		
	
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	
	if(SD_MODE)
		udelay(10);
	else
		ndelay(700);
		
	return;
}

void i2c_serial_read(i2c_dev_t* pI2C_Dev, unsigned short int *data)
{
#ifdef __test_program_used__
	int i;
#endif
	char j;
	unsigned int buf;
	
	*data = 0;
	
	__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_IN, GPIO_INT_DISABLE);//change sdio to input
//	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/
	for (j=7;j>=0;j--) {		

		__i2c_getGpioDataBit( pI2C_Dev->sdio, &buf);//read data,from MSB to LSB
		*data |= (buf<<j); 
		if(SD_MODE)
			udelay(10);
		else
			ndelay(600);
			
		__i2c_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
		//delay 10 us.
		if(SD_MODE)
			udelay(10);
		else		
			ndelay(600);
			
		__i2c_setGpioDataBit( pI2C_Dev->sclk, 0);  /* fall down sclk*/
		//delay 10 us.
		
		if(SD_MODE)
			udelay(10);
		else
			ndelay(700);
		

	}
	__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output	
	return;
}	


/*
@func int32 | _setGpio | abstract GPIO registers 
@parm enum GPIO_FUNC | func | control/data/interrupt register
@parm enum GPIO_PORT | port | GPIO port
@parm uint32 | pin | pin number
@parm uint32 | data | value
@rvalue NONE
@comm
This function is for internal use only. You don't need to care what register address of GPIO is.
This function abstracts these information.
*/

static void _setGpio( enum GPIO_FUNC func, enum GPIO_PORT port, uint32 pin, uint32 data )
{
	//assert( port < GPIO_PORT_MAX );
	//assert( func < GPIO_FUNC_MAX );
	//assert( pin < 8 );
  if(DebugEnable)
	printk( "[%s():%d] func=%d port=%d pin=%d data=%d REG32(PIN_MUX_SEL_2)=0x%08x\n", __FUNCTION__, __LINE__, func, port, pin, data,(uint32) REG32(PIN_MUX_SEL_2) );
	switch( func )
	{
		
		case GPIO_FUNC_DIRECTION:
			if(DebugEnable)
			printk( "[%s():%d] regGpioDirectionWrite[port]=0x%08x  bitStartGpioDirectionWrite[port]=%d\n", __FUNCTION__, __LINE__, regGpioDirectionWrite[port], bitStartGpioDirectionWrite[port] );

			if ( data ){
				REG32(regGpioDirectionWrite[port]) |= (uint32)1 << (pin+bitStartGpioDirectionWrite[port]);// 1 << (6+8=14)
				if(DebugEnable)
				printk("REG32(regGpioDirectionWrite[%d]) 0x%08x \n operbit=0x%08x\n",port,REG32(regGpioDirectionWrite[port]),(uint32)1 << (pin+bitStartGpioDirectionWrite[port]));
			}else{
				REG32(regGpioDirectionWrite[port]) &= ~((uint32)1 << (pin+bitStartGpioDirectionWrite[port]));
				if(DebugEnable)
				printk("REG32(regGpioDirectionWrite[%d]) 0x%08x \n operbit=0x%08x\n",port,REG32(regGpioDirectionWrite[port]),~((uint32)1 << (pin+bitStartGpioDirectionWrite[port])));
			}
			
			break;
		case GPIO_FUNC_DATA:
			if(DebugEnable)
				printk( "[%s():%d] regGpioDataWrite[port]=0x%08x  bitStartGpioDataWrite[port]=%d\n", __FUNCTION__, __LINE__, regGpioDataWrite[port], bitStartGpioDataWrite[port]);

	
				if ( data ){
					REG32(regGpioDataWrite[port]) |= (uint32)1 << (pin+bitStartGpioDataWrite[port]);
					if(DebugEnable)
					printk("REG32(regGpioDataWrite[%d]) 0x%08x shift %d\n operbit=0x%08x\n", port, REG32(regGpioDataWrite[port]),(pin+bitStartGpioDataWrite[port]),(uint32)1 << (pin+bitStartGpioDataWrite[port]));
				}else{
					REG32(regGpioDataWrite[port]) &= ~((uint32)1 << (pin+bitStartGpioDataWrite[port]));
					if(DebugEnable)
					printk("REG32(regGpioDataWrite[%d]) 0x%08x shift %d\n operbit=0x%08x\n", port, REG32(regGpioDataWrite[port]),(pin+bitStartGpioDataWrite[port]),~((uint32)1 << (pin+bitStartGpioDataWrite[port])));
				}
				
				break;
			

		case GPIO_FUNC_INTERRUPT_ENABLE:
			if(DebugEnable)
			printk( "[%s():%d] regGpioInterruptEnableWrite[port]=0x%08x  bitStartGpioInterruptEnableWrite[port]=%d\n", __FUNCTION__, __LINE__, regGpioInterruptEnableWrite[port], bitStartGpioInterruptEnableWrite[port] );

			if (data)
				REG32(regGpioInterruptEnableWrite[port]) |= (uint32)0b11 << ((pin*2)+bitStartGpioInterruptEnableWrite[port]);
			else
				REG32(regGpioInterruptEnableWrite[port]) &= ~((uint32)0b11 << ((pin*2)+bitStartGpioInterruptEnableWrite[port]));
			if(DebugEnable)
			printk("REG32(regGpioInterruptEnableWrite[%d]) 0x%08x shift %d\n operbit=0x%08x\n", port, REG32(regGpioInterruptEnableWrite[port]),(pin*2)+bitStartGpioInterruptEnableWrite[port],~((uint32)0b11 << ((pin*2)+bitStartGpioInterruptEnableWrite[port])));
			break;

		case GPIO_FUNC_INTERRUPT_STATUS:
			if(DebugEnable)
			printk( "[%s():%d] regGpioInterruptStatusWrite[port]=0x%08x  bitStartGpioInterruptStatusWrite[port]=%d\n", __FUNCTION__, __LINE__, regGpioInterruptStatusWrite[port], bitStartGpioInterruptStatusWrite[port] );

			if ( data )
				REG32(regGpioInterruptStatusWrite[port]) |= (uint32)1 << (pin+bitStartGpioInterruptStatusWrite[port]);
			else
				REG32(regGpioInterruptStatusWrite[port]) &= ~((uint32)1 << (pin+bitStartGpioInterruptStatusWrite[port]));
			break;

		case GPIO_FUNC_MAX:
			//assert( 0 );
			break;
	}
}


/*
@func int32 | _getGpio | abstract GPIO registers 
@parm enum GPIO_FUNC | func | control/data/interrupt register
@parm enum GPIO_PORT | port | GPIO port
@parm uint32 | pin | pin number
@rvalue uint32 | value
@comm
This function is for internal use only. You don't need to care what register address of GPIO is.
This function abstracts these information.
*/
static uint32 _getGpio( enum GPIO_FUNC func, enum GPIO_PORT port, uint32 pin )
{
	//assert( port < GPIO_PORT_MAX );		
	//assert( func < GPIO_FUNC_MAX );
	//assert( pin < 8 );	

	GPIO_PRINT(4, "[%s():%d] func=%d port=%d pin=%d\n", __FUNCTION__, __LINE__, func, port, pin);
	switch( func )
	{
				
		case GPIO_FUNC_DIRECTION:
			if(DebugEnable)
			printk( "[%s():%d] regGpioDirectionRead[port]=0x%08x  bitStartGpioDirectionRead[port]=%d\n", __FUNCTION__, __LINE__, regGpioDirectionRead[port], bitStartGpioDirectionRead[port] );

			if ( REG32(regGpioDirectionRead[port]) & ( (uint32)1 << (pin+bitStartGpioDirectionRead[port]) ) )
				return 1;
			else
				return 0;
			break;
			
		case GPIO_FUNC_DATA:
			if(DebugEnable)
			printk( "[%s():%d] regGpioDataRead[port]=0x%08x  bitStartGpioDataRead[port]=%d get=0x%08x REG32(regGpioDataRead[port]) 0x%08x\n", __FUNCTION__, __LINE__, regGpioDataRead[port], bitStartGpioDataRead[port] ,REG32(regGpioDataRead[port]) & ( (uint32)1 << (pin+bitStartGpioDataRead[port]) ),REG32(regGpioDataRead[port]));

			if ( REG32(regGpioDataRead[port]) & ( (uint32)1 << (pin+bitStartGpioDataRead[port]) ) )
				return 1;
			else
				return 0;
			break;
			
		case GPIO_FUNC_INTERRUPT_ENABLE:
			if(DebugEnable)
			printk( "[%s():%d] regGpioInterruptEnableRead[port]=0x%08x  bitStartGpioInterruptEnableRead[port]=%d\n", __FUNCTION__, __LINE__, regGpioInterruptEnableRead[port], bitStartGpioInterruptEnableRead[port] );

			if ( REG32(regGpioInterruptEnableRead[port]) & ( (uint32)1 <<(pin+bitStartGpioInterruptEnableRead[port]) ))
				return 1;
			else
				return 0;
			break;

		case GPIO_FUNC_INTERRUPT_STATUS:
			if(DebugEnable)
			printk( "[%s():%d] regGpioInterruptStatusRead[port]=0x%08x  bitStartGpioInterruptEnableRead[port]=%d\n", __FUNCTION__, __LINE__, regGpioInterruptStatusRead[port], bitStartGpioInterruptStatusRead[port] );

			if ( REG32(regGpioInterruptStatusRead[port]) & ( (uint32)1 << (pin+bitStartGpioInterruptStatusRead[port]) ) )
				return 1;
			else
				return 0;
			break;
			
		case GPIO_FUNC_MAX:
			//assert( 0 );
			break;
	}
	return 0xeeeeeeee;
}

/*************************************************************************************/
/*
@func int32 | __i2c_initGpioPin | Initiate a specifed GPIO port.
@parm uint32 | gpioId | The GPIO port that will be configured
@parm enum GPIO_PERIPHERAL | dedicate | Dedicated peripheral type
@parm enum GPIO_DIRECTION | direction | Data direction, in or out
@parm enum GPIO_INTERRUPT_TYPE | interruptEnable | Interrupt mode
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
This function is used to initialize GPIO port.
*/
int32 __i2c_initGpioPin( uint32 gpioId, enum GPIO_DIRECTION direction, 
                                           enum GPIO_INTERRUPT_TYPE interruptEnable )
 /* Note: In RTL8186, parm "dedicate" is not make sense. It can be any value because of nothing happen. */                                          
{

	uint32 port = GPIO_PORT( gpioId );
	uint32 pin = GPIO_PIN( gpioId );

	if ( port >= GPIO_PORT_MAX ) return FAILED;
	//if ( pin >= 8 ) return FAILED;	/* This judgment does not suit to RTL8186. GPIO PIN definition is not the same as RTL865x. */
  if(DebugEnable)
  printk("__i2c_initGpioPin port %d pin %d \n",port,pin);
		
	_setGpio( GPIO_FUNC_DIRECTION, port, pin, direction );

	_setGpio( GPIO_FUNC_INTERRUPT_ENABLE, port, pin, interruptEnable );
	

	return SUCCESS;
}


/*
@func int32 | __i2c_getGpioDataBit | Get the bit value of a specified GPIO ID.
@parm uint32 | gpioId | GPIO ID
@parm uint32* | data | Pointer to store return value
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
*/
int32 __i2c_getGpioDataBit( uint32 gpioId, uint32* pData )
{
	uint32 port = GPIO_PORT( gpioId );
	uint32 pin = GPIO_PIN( gpioId );

	if ( port >= GPIO_PORT_MAX ) return FAILED;
	//if ( pin >= 8 ) return FAILED;
	if ( pData == NULL ) return FAILED;
	
	*pData = _getGpio( GPIO_FUNC_DATA, port, pin );
	if(DebugEnable)
	printk( "[%s():%d] (port=%d,pin=%d)=%d\n", __FUNCTION__, __LINE__, port, pin, *pData );

	return SUCCESS;
}



/*
@func int32 | __i2c_setGpioDataBit | Set the bit value of a specified GPIO ID.
@parm uint32 | gpioId | GPIO ID
@parm uint32 | data | Data to write
@rvalue SUCCESS | success.
@rvalue FAILED | failed. Parameter error.
@comm
*/
int32 __i2c_setGpioDataBit( uint32 gpioId, uint32 data )
{
	uint32 port = GPIO_PORT( gpioId );
	uint32 pin = GPIO_PIN( gpioId );

	if ( port >= GPIO_PORT_MAX ) return FAILED;
	//if ( pin >= 8 ) return FAILED;
#if 0
	if ( _getGpio( GPIO_FUNC_DIRECTION, port, pin ) == GPIO_DIR_IN ) return FAILED; /* read only */  
#endif

  if(DebugEnable)
	printk("[%s():%d] (port=%d,pin=%d)=%d\n", __FUNCTION__, __LINE__, port, pin, data );

	_setGpio( GPIO_FUNC_DATA, port, pin, data );


	return SUCCESS;
}

//read
unsigned int serial_in_i2c(unsigned char addr, int len)
{
	unsigned short int data_byte=0;
	
	int retry_count=0;
	int k;

	
		
	
	
	//printk( "serial_in_i2c(%X):%X\n", addr, len );

	
	if(len > 2){
		if ( (kbuf=kmalloc( sizeof(unsigned char)* len,GFP_KERNEL) ) == NULL ) 
			return -ENOMEM; 
	}
	
	// start 
start_condition:
	i2c_start_condition( &MFi3959_i2c_dev );

	i2c_serial_write_byte( &MFi3959_i2c_dev, MFi_CP_I2C_ADDR_WRITE );
	


	// read ACK 
	if(i2c_ACK( &MFi3959_i2c_dev ) != 0 ){			
		udelay(5000);
		//printk("goto start_condition!\n");	
		__i2c_initGpioPin(MFi3959_i2c_dev.sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
		__i2c_setGpioDataBit( MFi3959_i2c_dev.sclk, 1);
		retry_count++;
		if(retry_count >20){
			if(len > 2 && kbuf != NULL)
				kfree(kbuf);
			return 0;
		}
		goto start_condition;
	}
	
	//udelay(40);
	
	__i2c_setGpioDataBit( MFi3959_i2c_dev.sdio, 0);
	__i2c_initGpioPin(MFi3959_i2c_dev.sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	

	
	// write register address 
	i2c_serial_write_byte( &MFi3959_i2c_dev, addr );
	
	i2c_ACK( &MFi3959_i2c_dev ) ;

	// stop
	
	retry_count=0;
read_start_condition:
		
	// start 
	i2c_start_condition( &MFi3959_i2c_dev );

	// read addr
	i2c_serial_write_byte( &MFi3959_i2c_dev, MFi_CP_I2C_ADDR_READ );
	
//	udelay(40);
	
	// read ACK 
	if( i2c_ACK( &MFi3959_i2c_dev ) != 0 ){
		udelay(5000);
		retry_count++;
		if(retry_count > 20){
			if(len > 2 && kbuf != NULL)
				kfree(kbuf);
			return 0;
		}
		goto read_start_condition;
		
	}
	

//	udelay(40);
		
	// read data_byte
	for (k=1;k<=len;k++)
	{
		i2c_serial_read( &MFi3959_i2c_dev, &data_byte );
//		printk("data_byte 0x%02X\n",data_byte);
		if(len <= 2){			
	  	mfi_ret_data[k-1] =  data_byte;	  	
  	}
  	else
  	{
  		kbuf[k-1] = data_byte;
  	}
  	if(k != len)
	  		i2c_ACK_w( &MFi3959_i2c_dev, 0);
	}

	// write negative-ACK
	i2c_ACK_w( &MFi3959_i2c_dev, 1 );
	

	if (addr == 0x05)
		printk( "error code in %X \n", mfi_ret_data[0]);
	else if(len <= 2)
		printk( "in[%X %X]\n", mfi_ret_data[0],mfi_ret_data[1] );
	//return data;
	
	return 1;
}
EXPORT_SYMBOL_GPL(serial_in_i2c);

//write
unsigned int serial_out_i2c(unsigned char addr, int len,  unsigned char * buf)
{
	//printk( "serial_out_i2c(%X):%X\n", addr, len  );
	int retry_count=0;
	int k=0;
	
start_condition:
	
	//printk("(%d)", __LINE__);
	// start 
	i2c_start_condition( &MFi3959_i2c_dev );
	
	// addr + write
	i2c_serial_write_byte( &MFi3959_i2c_dev, MFi_CP_I2C_ADDR_WRITE);
	
	// read ACK 
	if( i2c_ACK( &MFi3959_i2c_dev ) != 0 ){
		udelay(5000);
		//printk("goto start_condition!\n");
		__i2c_initGpioPin(MFi3959_i2c_dev.sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
		__i2c_setGpioDataBit(MFi3959_i2c_dev.sclk, 1);
		retry_count++;
		if(retry_count > 20)
			return 0;
		goto start_condition;
	}

	retry_count = 0;
addr_repeat:
	
	__i2c_setGpioDataBit( MFi3959_i2c_dev.sdio, 0);
	__i2c_initGpioPin(MFi3959_i2c_dev.sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	
	
	//printk("(%d)", __LINE__);
	// write register address 
	i2c_serial_write_byte( &MFi3959_i2c_dev, addr );
	
	// read ACK 

	if( i2c_ACK( &MFi3959_i2c_dev ) != 0 ){
			udelay(5000);
			//printk("goto addr_repeat!\n");
			__i2c_initGpioPin(MFi3959_i2c_dev.sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
			__i2c_setGpioDataBit( MFi3959_i2c_dev.sclk, 1);
			retry_count++;
			if(retry_count > 20)
				return 0;
			goto addr_repeat;
		}	
	//printk("(%d)", __LINE__);
	// write data hibyte
	
	
	__i2c_setGpioDataBit( MFi3959_i2c_dev.sdio, 0);
	__i2c_initGpioPin(MFi3959_i2c_dev.sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	
	
	for(k = 0 ;k < len;k++){	
//		if(len >= 2)
//			printk("serial_out_i2c write addr %x  buf %x\n",addr,*buf);
		retry_count = 0;
loop_repeat:
		i2c_serial_write_byte( &MFi3959_i2c_dev, *buf );	
			
		if( i2c_ACK( &MFi3959_i2c_dev ) != 0 ){
			udelay(5000);
			//printk("goto loop_repeat!\n");
			__i2c_initGpioPin(MFi3959_i2c_dev.sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
			__i2c_setGpioDataBit( MFi3959_i2c_dev.sclk, 1);
			retry_count++;
			if(retry_count > 20){
				break;
			}
			goto loop_repeat;
		}		
	
	__i2c_setGpioDataBit( MFi3959_i2c_dev.sdio, 0);
	__i2c_initGpioPin(MFi3959_i2c_dev.sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	  if(len != 1)
		buf++;	
	}
	if(retry_count > 20)
		return 0;
	//printk("(%d)", __LINE__);
	// stop
	i2c_stop_condition( &MFi3959_i2c_dev );
	
	return 1;
}
EXPORT_SYMBOL_GPL(serial_out_i2c);


void i2c_start_condition(i2c_dev_t* pI2C_Dev)
{

	__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
//	__i2c_initGpioPin(pI2C_Dev->sclk, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output	
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 1); // raise sclk
	__i2c_setGpioDataBit( pI2C_Dev->sdio, 1); // raise sdio
  
  
	//delay 10 us.
	
	udelay(100); //1us
	
	

	__i2c_setGpioDataBit( pI2C_Dev->sdio, 0); /* fall down sdio*//*start condition*/

	if(SD_MODE)
		udelay(10);
	else
		ndelay(600);
		

	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0);
	//delay 20 us.
	
	if(SD_MODE)
		ndelay(3550);
	else
		ndelay(2500);
	

}

void i2c_stop_condition(i2c_dev_t* pI2C_Dev)
{


	__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);//change sdio to output
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 0); /* fall down sclk*/


	__i2c_setGpioDataBit( pI2C_Dev->sdio, 0); /* fall down sdio*/
	//delay 10 us.
	if(SD_MODE)
		udelay(10);
	else
		ndelay(2500);


	__i2c_setGpioDataBit( pI2C_Dev->sclk, 1); /* raise sclk*/
	
	//delay 10 us.
	if(SD_MODE)
		udelay(10);
	else
		ndelay(2500);

	__i2c_setGpioDataBit( pI2C_Dev->sdio, 1); /* raise sdio*//*stop condition*/
	if(SD_MODE)
		udelay(40);
	else
		ndelay(2500);
}

void i2c_init_SCL_SDA(i2c_dev_t* pI2C_Dev)
{		

	__i2c_initGpioPin(pI2C_Dev->sclk, GPIO_DIR_OUT, GPIO_INT_DISABLE);
	__i2c_initGpioPin(pI2C_Dev->sdio, GPIO_DIR_OUT, GPIO_INT_DISABLE);
  
	__i2c_setGpioDataBit( pI2C_Dev->sclk, 1 ); // 
  __i2c_setGpioDataBit( pI2C_Dev->sdio, 1 );


}

static void rtl8196e_init_reg_gpio( void )
{
#if defined(CONFIG_RTL_8196E)	
	if(RTL96E_BOND == EU_VE3){
		REG32(PIN_MUX_SEL_2) |= (uint32)0b11000000000000;  //SET SHARED PIN
		REG32(GPABCDCNR) &= ~((uint32)0b100000000010000);	 //SET GPIO FUNCTION
		MFi3959_i2c_dev.sdio = I2C_GPIO_ID( 'A', 4 );
		MFi3959_i2c_dev.sclk = I2C_GPIO_ID( 'B', 6 );
	}else if (RTL96E_BOND == E_VE3){
		RTL_W32(PIN_MUX_SEL_2, (REG32(PIN_MUX_SEL_2) | 0x600));//SET SHARED PIN
		RTL_W32(GPABCDCNR, (REG32(GPABCDCNR) & (~(0x2008))));//SET GPIO FUNCTION
		MFi3959_i2c_dev.sdio = I2C_GPIO_ID( 'A', 3 );
		MFi3959_i2c_dev.sclk = I2C_GPIO_ID( 'B', 5 );
		
	}
#elif defined(CONFIG_RTL_8881A_ULINKER) || defined(CONFIG_RTL_8881A_SELECTIVE)
		//RTL_W32(PIN_MUX_SEL_2, (REG32(PIN_MUX_SEL_2) | 0x603));//SET SHARED PIN
		RTL_W32(GPEFGHCNR, (REG32(GPEFGHCNR) & (~(0x1800))));//SET GPIO FUNCTION
		MFi3959_i2c_dev.sdio = I2C_GPIO_ID( 'F', 4 );
		MFi3959_i2c_dev.sclk = I2C_GPIO_ID( 'F', 3 );
#endif	
	i2c_init_SCL_SDA( &MFi3959_i2c_dev );
}

static int proc_read_i2cG(char *page, char **start,
                            off_t off, int count, 
                            int *eof, void *data)
{
				int len;
        unsigned char buf1[2]={0x14,0x0};
        unsigned char buf2[2]={0x0,0x14};
        rtl8196e_init_reg_gpio();
        udelay(500);
        i2c_init_SCL_SDA( &MFi3959_i2c_dev ); 
        udelay(500);
        len = serial_in_i2c(0x05, 1 );
        udelay(500);
        len = serial_out_i2c(0x20, 2, buf1 );
        udelay(500);
        len = serial_in_i2c(0x05, 1 );
        udelay(500);
        len = serial_out_i2c(0x20, 2, buf2 );
        udelay(500);
        len = serial_in_i2c(0x05, 1 );

        return 0;
}


static int proc_write_i2cG(struct file *file,
                             const char *buffer,
                             unsigned long count, 
                             void *data)
{
        int len;
        struct fb_data_t *fb_data = (struct fb_data_t *)data;

        if(count > FOOBAR_LEN)
                len = FOOBAR_LEN;
        else
                len = count;

        if(copy_from_user(fb_data->value, buffer, len))
                return -EFAULT;

        fb_data->value[len] = '\0';
        
        return len;
}


static int __init rtl8196e_init_procfs(void)
{
	int ret;
	
	rtl8196e_init_reg_gpio();	
	
/**************************************************/
//procfs for controling I2C 

  struct proc_dir_entry *i2cGProcEntry;

  i2cGProcEntry = create_proc_entry("i2cG", 0777, NULL );
  if(i2cGProcEntry == NULL) {
  	ret = -ENOMEM;
  	goto no_i2cG;
 	}
 
 //i2cGProcEntry->owner = THIS_MODULE;
 strcpy(i2cG_data.name, "i2c-GPIO");
 strcpy(i2cG_data.value, "i2c-GPIO");
 i2cGProcEntry->data = &i2cG_data;
 i2cGProcEntry->read_proc = proc_read_i2cG;
 i2cGProcEntry->write_proc = proc_write_i2cG;
 

	return 0;
no_i2cG:
  remove_proc_entry("i2cG", NULL);
  return ret;
}


static void __exit cleanup_procfs_i2cG(void)
{
       remove_proc_entry("i2cG", NULL);

       // printk(KERN_INFO "%s %s removed\n", MODULE_NAME, MODULE_VERS);
}


module_init(mfi_ioctl_init);
module_exit(mfi_ioctl_exit);
MODULE_AUTHOR("Realtek");
MODULE_DESCRIPTION("gpio-i2c driver for MFi CP");
MODULE_LICENSE("GPL");

