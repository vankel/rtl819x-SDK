#ifndef I2C_IOCTL_H
#define I2C_IOCTL_H

#include <linux/ioctl.h>
 
typedef struct
{
	unsigned char reg;
	unsigned char buf_doubule[2];
	unsigned char buf_fourfold[4];
	unsigned char chlg_rspn_data[128];
	unsigned char acc_cert_data[1280];
	unsigned char apple_cert_data[1024];
} mfi_arg_t;

/*********************** I2C data struct ********************************************/
typedef struct i2c_dev_s
{
	//unsigned int i2c_reset;		//output
	unsigned int sclk;		//output
	unsigned int sdio;		//input or output	
	unsigned int irq;		// interrupt (optional)
	unsigned int reset;		// reset (optional)
} i2c_dev_t;	

#define MFi_GET_VARIABLES _IOR('q', 1, mfi_arg_t *)
#define MFi_CLR_VARIABLES _IO('q', 2)
#define MFi_SET_VARIABLES _IOW('q', 3, mfi_arg_t *)
 
#endif