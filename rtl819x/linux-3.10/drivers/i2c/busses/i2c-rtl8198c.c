/* linux/drivers/i2c/busses/i2c-rtl8198c.c
 *
 * Copyright (C) 2004,2005,2009 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *
 * RTL8198c I2C Controller
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>
#include <linux/clk.h>
#include <linux/cpufreq.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/of_i2c.h>
#include <linux/of_gpio.h>
#include <linux/ioctl.h>
#include <linux/i2c-dev.h>
#include <asm/irq.h>

#include <linux/fs.h>

#include <linux/file.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <asm/uaccess.h>

#include <linux/proc_fs.h>
/* i2c controller state */

enum rtl8198c_i2c_state {
	STATE_IDLE,
	STATE_START,
	STATE_READ,
	STATE_WRITE,
	STATE_STOP
};

//enum rtl8198c_i2c_type {	TYPE_RTL8198c,};


//====================================================
//#define I2CBASE 0xb8000600 
#define I2CBASE (i2c->regs) 
#define I2C_CMD 		(I2CBASE+0x00)
#define I2C_STATUS	 (I2CBASE+0x04)
#define I2C_REGADDR (I2CBASE+0x08)
#define I2C_CONFIG 	(I2CBASE+0x0c)

#define I2C_DATAW0 (I2CBASE+0x10)
#define I2C_DATAW1 (I2CBASE+0x14)
#define I2C_DATAW2 (I2CBASE+0x18)
#define I2C_DATAW3 (I2CBASE+0x1c)
#define I2C_DATAW4 (I2CBASE+0x20)
#define I2C_DATAW5 (I2CBASE+0x24)
#define I2C_DATAW6 (I2CBASE+0x28)
#define I2C_DATAW7 (I2CBASE+0x2c)


#define I2C_DATAR0 (I2CBASE+0x30)
#define I2C_DATAR1 (I2CBASE+0x34)
#define I2C_DATAR2 (I2CBASE+0x38)
#define I2C_DATAR3 (I2CBASE+0x3c)
#define I2C_DATAR4 (I2CBASE+0x40)
#define I2C_DATAR5 (I2CBASE+0x44)
#define I2C_DATAR6 (I2CBASE+0x48)
#define I2C_DATAR7 (I2CBASE+0x4c)

#define I2C_DATAR8 (I2CBASE+0x50)
#define I2C_DATAR9 (I2CBASE+0x54)
#define I2C_DATAR10 (I2CBASE+0x58)
#define I2C_DATAR11 (I2CBASE+0x5c)
#define I2C_DATAR12 (I2CBASE+0x60)
#define I2C_DATAR13 (I2CBASE+0x64)
#define I2C_DATAR14 (I2CBASE+0x68)
#define I2C_DATAR15 (I2CBASE+0x6c)

#define MAX_I2C_READ_BYTES (16*4)
#define MAX_I2C_WRITE_BYTES (8*4)


//0x00
#define I2C_CMD_SLAVE_ADDR_OFFSET (0)
#define I2C_CMD_AUTO_POLLING_OFFSET (7)
#define I2C_CMD_READ_LENGTH_OFFSET (16)
#define I2C_CMD_WRITE_LENGTH_OFFSET (23)
#define I2C_CMD_REG_ADDR_OFFSET (29)

//0x04
#define I2C_STATUS_INTSTS 			(1<<0)
#define I2C_STATUS_INTMASK 		(1<<1)
#define I2C_STATUS_POLLING_FAIL 	(1<<2)
#define I2C_STATUS_WDATA_FAIL 	(1<<3)
#define I2C_STATUS_WREG_FAIL 		(1<<4)
#define I2C_STATUS_WSLVADDR_FAIL 	(1<<5)
#define I2C_STATUS_READY 			(1<<6)

//0x0c
#define I2C_SCONFIG_CLKSEL_OFFSET (0)
#define I2C_SCONFIG_POLLINGTIMER_OFFSET (9)

#define REG32(reg)		(*(volatile unsigned int   *)(reg))		

// When you want to dump register, please uncomment the define below.
//#define RTKI2C_DBG

#ifdef RTKI2C_DBG
#define RTKI2C_DBG_MSG printk
#else
#define RTKI2C_DBG_MSG
#endif

u8 IRQ_DBG = 0;
unsigned char msg_buf[128];
struct i2c_msg mymsgs;
//====================================================

 struct rtl8198c_platform_i2c {
         int             bus_num;
         unsigned int    flags;
         unsigned int    slave_addr;
         unsigned long   frequency;
//         unsigned int    sda_delay;
 
//         void    (*cfg_gpio)(struct platform_device *dev);
 };

struct rtl8198c_i2c {
	spinlock_t		lock;
	wait_queue_head_t	wait;
	unsigned int		suspended:1;

	struct i2c_msg		*msg;
	unsigned int		msg_num;
	unsigned int		msg_idx;
	unsigned int		msg_ptr;

	unsigned int		tx_setup;
	unsigned int		irq;

	enum rtl8198c_i2c_state	state;
	unsigned long		clkrate;

	void __iomem		*regs;
	struct clk		*clk;
	struct device		*dev;
	struct resource		*ioarea;
	struct i2c_adapter	adap;

	struct rtl8198c_platform_i2c	*pdata;
};

/* default platform data removed, dev should always carry data. */
//===========================================================
/* rtl8198c_i2c_master_complete
 *
 * complete the message and wake up the caller, using the given return code,
 * or zero to mean ok.
*/
static inline void rtl8198c_i2c_master_complete(struct rtl8198c_i2c *i2c, int ret)
{
	dev_dbg(i2c->dev, "master_complete %d\n", ret);

	i2c->msg_ptr = 0;
	i2c->msg = NULL;
	i2c->msg_idx++;
	i2c->msg_num = 0;
	if (ret)
		i2c->msg_idx = ret;

	wake_up(&i2c->wait);
}
//===========================================================
static inline void rtl8198c_i2c_disable_ack(struct rtl8198c_i2c *i2c)
{
	unsigned long tmp;
#if 0 //wei add
	tmp = readl(i2c->regs + RTL8198c_IICCON);
	writel(tmp & ~RTL8198c_IICCON_ACKEN, i2c->regs + RTL8198c_IICCON);
#endif	
}
//===========================================================
static inline void rtl8198c_i2c_enable_ack(struct rtl8198c_i2c *i2c)
{
	unsigned long tmp;
#if 0 //wei add
	tmp = readl(i2c->regs + RTL8198c_IICCON);
	writel(tmp | RTL8198c_IICCON_ACKEN, i2c->regs + RTL8198c_IICCON);
#endif	
}
//===========================================================
/* irq enable/disable functions */

static inline void rtl8198c_i2c_disable_irq(struct rtl8198c_i2c *i2c)
{	//REG32(I2C_STATUS) = I2C_STATUS_INTMASK;
	writel(I2C_STATUS_INTMASK, I2C_STATUS);
}

static inline void rtl8198c_i2c_enable_irq(struct rtl8198c_i2c *i2c)
{	//REG32(I2C_STATUS) = 0;
	writel(0, I2C_STATUS);
}
static inline void clear_all_fail_and_enable_irq(struct rtl8198c_i2c *i2c)
{
	writel(readl(I2C_STATUS), I2C_STATUS); //clear all fail.
	writel(0, I2C_STATUS);
}
//=========================================================
unsigned int I2C_WriteRead(struct rtl8198c_i2c *i2c,
							unsigned int slv_addr, 
							unsigned int 	reg_addr_len, 	
							unsigned int    reg_addr, 				
							unsigned int 	wlen, 							
							unsigned char *wbuff,
							unsigned int	 rlen, 
							unsigned char *rbuff,
							unsigned int rxpolling)
{

	unsigned int regaddr_sel=reg_addr_len;

	
	if(regaddr_sel==1)		REG32(I2C_REGADDR)=(reg_addr&0xff)<<24;
	if(regaddr_sel==2)		REG32(I2C_REGADDR)=(reg_addr&0xffff)<<16;
	if(regaddr_sel==3)		REG32(I2C_REGADDR)=(reg_addr&0xffffff)<<8;
	if(regaddr_sel==4)		REG32(I2C_REGADDR)=(reg_addr);
				


	if(wlen>0)
	{
		//copy write data buffer to HW FIFO.
		int i;
		unsigned char data;
		for(i=0; i<wlen; i++)
		{	
			data=wbuff[i];
			RTKI2C_DBG_MSG("W:data[%d]=%x\n", i, data);
	
			int idx=(i/4);
			int off=(i%4);
			if(off==0)		REG32(I2C_DATAW0 + idx*4)=(REG32(I2C_DATAW0 + idx*4) & 0xffffff00) | data;
			else if(off==1)	REG32(I2C_DATAW0 + idx*4)=(REG32(I2C_DATAW0 + idx*4) & 0xffff00ff) | (data<<8);
			else if(off==2)	REG32(I2C_DATAW0 + idx*4)=(REG32(I2C_DATAW0 + idx*4) & 0xff00ffff) | (data<<16);
			else if(off==3)	REG32(I2C_DATAW0 + idx*4)=(REG32(I2C_DATAW0 + idx*4) & 0x00ffffff) | (data<<24);

			RTKI2C_DBG_MSG("W: addr=%x, data=%x\n", I2C_DATAW0, REG32(I2C_DATAW0));
		}

	}
	
	//clear ISR
	REG32(I2C_STATUS)=REG32(I2C_STATUS);

	//enable irq
	if(IRQ_DBG)
		REG32(I2C_STATUS)=0;   

        
	RTKI2C_DBG_MSG("Before Status=%x\n", REG32(I2C_STATUS));

	//auto polling device
	int polling=0;
	if(slv_addr&0x8000)
	{
		slv_addr&=0x00ff;
		polling=1;
	}
	
	//kick start	
	REG32(I2C_CMD) =	( slv_addr <<I2C_CMD_SLAVE_ADDR_OFFSET) | 
						( polling<<I2C_CMD_AUTO_POLLING_OFFSET) |     //enable auto polling
						( rlen<<I2C_CMD_READ_LENGTH_OFFSET) |
						( wlen<<I2C_CMD_WRITE_LENGTH_OFFSET) |
						((regaddr_sel&7)<<I2C_CMD_REG_ADDR_OFFSET) ;

	RTKI2C_DBG_MSG("CMD=%x\n", REG32(I2C_CMD)); 

	//enable irq
	if(IRQ_DBG)
		REG32(I2C_STATUS)=0;   

#if 1
if(rxpolling)
{ 	
	//wait interrrupt
	if(rxpolling!=1){
	while(1)
	{
		if(REG32(I2C_STATUS) & I2C_STATUS_INTSTS)
			break;
	} 
     } // if you use rxpolling, you do not need this while loop. This loop is perpetual.
    
	RTKI2C_DBG_MSG("Done, Status=%x\n", REG32(I2C_STATUS));
	
	if(REG32(I2C_STATUS) & (	I2C_STATUS_POLLING_FAIL|\
							I2C_STATUS_WDATA_FAIL|\
							I2C_STATUS_WREG_FAIL|\ 		
							I2C_STATUS_WSLVADDR_FAIL))
	{	
		unsigned int t=REG32(I2C_STATUS);
		printk("Error STATUS=%x, %s %s %s %s\n", t,\
						(t&I2C_STATUS_WSLVADDR_FAIL) ? "Dev," : "" , \
						(t&I2C_STATUS_WREG_FAIL) ? "reg, " : "",  \
						(t&I2C_STATUS_WDATA_FAIL) ? "data, " : "",  \
						(t&I2C_STATUS_POLLING_FAIL) ? "polling," : ""  				);
		REG32(I2C_STATUS)=t;
		return 1;  //fail
	}
	
	if(rlen>0)
	{ 	
		//copy HW FIFO to read data buffer.
		int i;
		unsigned char data;
		for(i=0; i<rlen; i++)
		{	
			int idx=(i/4);
			int off=(i%4);
			if(off==0)		data=REG32(I2C_DATAR0 + idx*4) & (0xff<<0);
			else if(off==1)	data=(REG32(I2C_DATAR0 + idx*4) & (0xff<<8))>>8;
			else if(off==2)	data=(REG32(I2C_DATAR0 + idx*4) & (0xff<<16))>>16;
			else if(off==3)	data=(REG32(I2C_DATAR0 + idx*4) & (0xff<<24))>>24;	

			rbuff[i]=data;
			RTKI2C_DBG_MSG("R:i=%d, data=%x\n", i, data);
			RTKI2C_DBG_MSG("Raddr=%x, data=%x\n", I2C_DATAR0, REG32(I2C_DATAR0));
		}
	}	
	//clear all interrupt.
	REG32(I2C_STATUS)=REG32(I2C_STATUS);	
	return 0;

}
#endif

	return 0;

}

void Get_I2C_Data(struct rtl8198c_i2c *i2c, unsigned char *buff, unsigned int len)
{
	if(len>0)
	{
		//copy HW FIFO to read data buffer.
		int i;
		unsigned char data;
		for(i=0; i<len; i++)
		{	
			int idx=(i/4);
			int off=(i%4);
			if(off==0)		data=REG32(I2C_DATAR0 + idx*4) & (0xff<<0);
			else if(off==1)	data=(REG32(I2C_DATAR0 + idx*4) & (0xff<<8))>>8;
			else if(off==2)	data=(REG32(I2C_DATAR0 + idx*4) & (0xff<<16))>>16;
			else if(off==3)	data=(REG32(I2C_DATAR0 + idx*4) & (0xff<<24))>>24;	

			buff[i]=data;
			RTKI2C_DBG_MSG("R:i=%d, data=%x\n", i, data);
			RTKI2C_DBG_MSG("Raddr=%x, data=%x\n", I2C_DATAR0, REG32(I2C_DATAR0));
		}
	}
}
//===============================================================
/* rtl8198c_i2c_message_start
 *
 * put the start of a message onto the bus
*/
static void rtl8198c_i2c_message_start(struct rtl8198c_i2c *i2c,      struct i2c_msg *msg)
{
#if 0 //wei add
	unsigned int addr = (msg->addr & 0x7f) << 1;
	unsigned long stat;
	unsigned long iiccon;

	stat = 0;
	stat |=  RTL8198c_IICSTAT_TXRXEN;

	if (msg->flags & I2C_M_RD) 
	{
		stat |= RTL8198c_IICSTAT_MASTER_RX;
		addr |= 1;
	} else
		stat |= RTL8198c_IICSTAT_MASTER_TX;

	if (msg->flags & I2C_M_REV_DIR_ADDR)
		addr ^= 1;

	/* todo - check for wether ack wanted or not */
	rtl8198c_i2c_enable_ack(i2c);

	iiccon = readl(i2c->regs + RTL8198c_IICCON);
	writel(stat, i2c->regs + RTL8198c_IICSTAT);

	dev_dbg(i2c->dev, "START: %08lx to IICSTAT, %02x to DS\n", stat, addr);
	writeb(addr, i2c->regs + RTL8198c_IICDS);

	/* delay here to ensure the data byte has gotten onto the bus
	 * before the transaction is started */

	ndelay(i2c->tx_setup);

	dev_dbg(i2c->dev, "iiccon, %08lx\n", iiccon);
	writel(iiccon, i2c->regs + RTL8198c_IICCON);

	stat |= RTL8198c_IICSTAT_START;
	writel(stat, i2c->regs + RTL8198c_IICSTAT);
#endif	

        if (msg->flags & I2C_M_RD) 
            {  
                I2C_WriteRead(i2c, msg->addr, 0, NULL, 0, NULL, msg->len, msg->buf, 0); //rxpolling 0 to 1
        }
        else
            {  
                I2C_WriteRead(i2c, msg->addr, 0, NULL,  msg->len, msg->buf, 0, NULL, 0);
	}


}

//===========================================================
static inline void rtl8198c_i2c_stop(struct rtl8198c_i2c *i2c, int ret)
{
#if 0 //wei add
	unsigned long iicstat = readl(i2c->regs + RTL8198c_IICSTAT);

	dev_dbg(i2c->dev, "STOP\n");

	/* stop the transfer */
	iicstat &= ~RTL8198c_IICSTAT_START;
	writel(iicstat, i2c->regs + RTL8198c_IICSTAT);
#endif
	i2c->state = STATE_STOP;
	rtl8198c_i2c_master_complete(i2c, ret);
	rtl8198c_i2c_disable_irq(i2c);
}
//===========================================================
/* helper functions to determine the current state in the set of
 * messages we are sending */

/* is_lastmsg()
 *
 * returns TRUE if the current message is the last in the set
*/
static inline int is_lastmsg(struct rtl8198c_i2c *i2c)
{
	return i2c->msg_idx >= (i2c->msg_num - 1);
}
//===========================================================
/* is_msglast
 *
 * returns TRUE if we this is the last byte in the current message
*/
static inline int is_msglast(struct rtl8198c_i2c *i2c)
{
	return i2c->msg_ptr == i2c->msg->len-1;
}
//===========================================================
/* is_msgend
 *
 * returns TRUE if we reached the end of the current message
*/
static inline int is_msgend(struct rtl8198c_i2c *i2c)
{
	return i2c->msg_ptr >= i2c->msg->len;
}
//===========================================================
/* i2c_rtl_irq_nextbyte
 *
 * process an interrupt and work out what to do
 */
static int i2c_rtl_irq_nextbyte(struct rtl8198c_i2c *i2c, unsigned long iicstat)
{
	unsigned long tmp;
	unsigned char byte;
	int ret = 0;
#if 0 //wei add
	switch (i2c->state) 
	{

	case STATE_IDLE:
		dev_err(i2c->dev, "%s: called in STATE_IDLE\n", __func__);
		goto out;

	case STATE_STOP:
		dev_err(i2c->dev, "%s: called in STATE_STOP\n", __func__);
		rtl8198c_i2c_disable_irq(i2c);
		goto out_ack;

	case STATE_START:
		/* last thing we did was send a start condition on the
		 * bus, or started a new i2c message
		 */
		if (iicstat & RTL8198c_IICSTAT_LASTBIT &&    !(i2c->msg->flags & I2C_M_IGNORE_NAK)) 
		{
			/* ack was not received... */
			dev_dbg(i2c->dev, "ack was not received\n");
			rtl8198c_i2c_stop(i2c, -ENXIO);
			goto out_ack;
		}

		if (i2c->msg->flags & I2C_M_RD)	i2c->state = STATE_READ;
		else								i2c->state = STATE_WRITE;

		/* terminate the transfer if there is nothing to do
		 * as this is used by the i2c probe to find devices. */

		if (is_lastmsg(i2c) && i2c->msg->len == 0) 
		{
			rtl8198c_i2c_stop(i2c, 0);
			goto out_ack;
		}

		if (i2c->state == STATE_READ)
			goto prepare_read;

		/* fall through to the write state, as we will need to
		 * send a byte as well */

	case STATE_WRITE:
		/* we are writing data to the device... check for the
		 * end of the message, and if so, work out what to do
		 */
		if (!(i2c->msg->flags & I2C_M_IGNORE_NAK)) 
		{
			if (iicstat & RTL8198c_IICSTAT_LASTBIT) 
			{
				dev_dbg(i2c->dev, "WRITE: No Ack\n");
				rtl8198c_i2c_stop(i2c, -ECONNREFUSED);
				goto out_ack;
			}
		}

 retry_write:

		if (!is_msgend(i2c)) 
		{
			byte = i2c->msg->buf[i2c->msg_ptr++];
			writeb(byte, i2c->regs + RTL8198c_IICDS);

			/* delay after writing the byte to allow the
			 * data setup time on the bus, as writing the
			 * data to the register causes the first bit
			 * to appear on SDA, and SCL will change as
			 * soon as the interrupt is acknowledged */

			ndelay(i2c->tx_setup);
		} 
		else if (!is_lastmsg(i2c)) 
		{
			/* we need to go to the next i2c message */

			dev_dbg(i2c->dev, "WRITE: Next Message\n");

			i2c->msg_ptr = 0;
			i2c->msg_idx++;
			i2c->msg++;

			/* check to see if we need to do another message */
			if (i2c->msg->flags & I2C_M_NOSTART) 
			{

				if (i2c->msg->flags & I2C_M_RD) 
				{
					/* cannot do this, the controller
					 * forces us to send a new START
					 * when we change direction */
					rtl8198c_i2c_stop(i2c, -EINVAL);
				}
				goto retry_write;
			} 
			else 
			{	/* send the new start */
				rtl8198c_i2c_message_start(i2c, i2c->msg);
				i2c->state = STATE_START;
			}

		} 
		else 
		{	/* send stop */
			rtl8198c_i2c_stop(i2c, 0);
		}
		break;

	case STATE_READ:
		/* we have a byte of data in the data register, do
		 * something with it, and then work out wether we are
		 * going to do any more read/write
		 */
		byte = readb(i2c->regs + RTL8198c_IICDS);
		i2c->msg->buf[i2c->msg_ptr++] = byte;

 prepare_read:
		if (is_msglast(i2c)) 
		{	/* last byte of buffer */
			if (is_lastmsg(i2c))
				rtl8198c_i2c_disable_ack(i2c);

		} else if (is_msgend(i2c)) 
		{
			/* ok, we've read the entire buffer, see if there
			 * is anything else we need to do */

			if (is_lastmsg(i2c)) 
			{	/* last message, send stop and complete */
				dev_dbg(i2c->dev, "READ: Send Stop\n");

				rtl8198c_i2c_stop(i2c, 0);
			} 
			else 
			{	/* go to the next transfer */
				dev_dbg(i2c->dev, "READ: Next Transfer\n");

				i2c->msg_ptr = 0;
				i2c->msg_idx++;
				i2c->msg++;
			}
		}

		break;
	}

	/* acknowlegde the IRQ and get back on with the work */

 out_ack:
	tmp = readl(i2c->regs + RTL8198c_IICCON);
	tmp &= ~RTL8198c_IICCON_IRQPEND;
	writel(tmp, i2c->regs + RTL8198c_IICCON);
#else	


	if (i2c->msg->flags & I2C_M_RD)
	{	
		i2c->state = STATE_READ;

		Get_I2C_Data(i2c, i2c->msg->buf, i2c->msg->len);
#if 0		
		if (is_msglast(i2c)) 
		{	/* last byte of buffer */
			if (is_lastmsg(i2c))
				rtl8198c_i2c_disable_ack(i2c);

		} else if (is_msgend(i2c)) 
		{
			/* ok, we've read the entire buffer, see if there
			 * is anything else we need to do */

			if (is_lastmsg(i2c)) 
			{	/* last message, send stop and complete */
				dev_dbg(i2c->dev, "READ: Send Stop\n");

				rtl8198c_i2c_stop(i2c, 0);
			} 
			else 
			{	/* go to the next transfer */
				dev_dbg(i2c->dev, "READ: Next Transfer\n");

				i2c->msg_ptr = 0;
				i2c->msg_idx++;
				i2c->msg++;
			}
		}	
	
#endif
	}
	else
	{	i2c->state = STATE_WRITE;
#if 0
		if (is_lastmsg(i2c) && i2c->msg->len == 0) 
		{
			rtl8198c_i2c_stop(i2c, 0);
			goto out;
		}

		if (!is_msgend(i2c)) 
		{
			ndelay(i2c->tx_setup);
		} 
		else if (!is_lastmsg(i2c)) 
		{
			/* we need to go to the next i2c message */

			dev_dbg(i2c->dev, "WRITE: Next Message\n");

			i2c->msg_ptr = 0;
			i2c->msg_idx++;
			i2c->msg++;

			/* check to see if we need to do another message */
			{	/* send the new start */
				rtl8198c_i2c_message_start(i2c, i2c->msg);
				i2c->state = STATE_START;
			}
		} 
		else 
		{	/* send stop */
			rtl8198c_i2c_stop(i2c, 0);
		}
#endif		
	}

	rtl8198c_i2c_stop(i2c, 0);
	
#endif
 out:
	return ret;
}
//===========================================================
/* rtl8198c_i2c_irq
 *
 * top level IRQ servicing routine
*/

static irqreturn_t rtl8198c_i2c_irq(int irqno, void *dev_id)
{ 
	struct rtl8198c_i2c *i2c = dev_id;
	unsigned long status;
	unsigned long tmp;
	unsigned int err=0;

#if 0 //wei add
	status = readl(i2c->regs + RTL8198c_IICSTAT);

	if (status & RTL8198c_IICSTAT_ARBITR) {
		/* deal with arbitration loss */
		dev_err(i2c->dev, "deal with arbitration loss\n");
	}

	if (i2c->state == STATE_IDLE) {
		dev_dbg(i2c->dev, "IRQ: error i2c->state == IDLE\n");

		tmp = readl(i2c->regs + RTL8198c_IICCON);
		tmp &= ~RTL8198c_IICCON_IRQPEND;
		writel(tmp, i2c->regs +  RTL8198c_IICCON);
		goto out;
	}

	/* pretty much this leaves us with the fact that we've
	 * transmitted or received whatever byte we last sent */
	 
	i2c_rtl_irq_nextbyte(i2c, status);	 
#endif

	status =REG32(I2C_STATUS);

	if(status&I2C_STATUS_POLLING_FAIL)
	{
		if(i2c)
			dev_err(i2c->dev, "polling fail\n");
		else
			//printk("polling fail\n");
		err=1;
	}
	if(status&I2C_STATUS_WDATA_FAIL)
	{
		if(i2c)
			dev_err(i2c->dev, "write data fail\n");
		else
			//printk("write data fail\n");
		err=1;
	}
	if(status&I2C_STATUS_WREG_FAIL)
	{
		if(i2c)
			dev_err(i2c->dev, "write reg fail\n");
		else
			//printk("write reg fail\n");
		err=1;
	}
	if(status&I2C_STATUS_WSLVADDR_FAIL)
	{ 
		if(i2c)
			dev_err(i2c->dev, "write slave addr fail\n");
		else
		{
		//	printk("write slave addr fail\n");
		}
		err=1;
	}

	if( (err==0) && (status&I2C_STATUS_READY))
	{


		if(i2c){ 

			if (i2c->msg->flags & I2C_M_RD)
			{
				dev_dbg(i2c->dev, "read data.\n");	
				Get_I2C_Data(i2c, i2c->msg->buf, i2c->msg->len);
			}
		}
		else
		{
		//	printk("read data.\n");
		}
	}

	if(i2c)
		rtl8198c_i2c_stop(i2c, 0); 	
	else
		rtl8198c_i2c_disable_irq(i2c);
	
 out:
 	REG32(I2C_STATUS)=status;	
	return IRQ_HANDLED;
}

//===========================================================
/* rtl8198c_i2c_set_master
 *
 * get the i2c bus for a master transaction
*/
static int rtl8198c_i2c_set_master(struct rtl8198c_i2c *i2c)
{
	unsigned long iicstat;
	int timeout = 400;

	while (timeout-- > 0) {		
		iicstat=REG32(I2C_STATUS);
		if ((iicstat & I2C_STATUS_READY)==I2C_STATUS_READY)
			return 0;

		msleep(1);
	}
	return -ETIMEDOUT;
}
//===========================================================


/* rtl8198c_i2c_doxfer
 *
 * this starts an i2c transfer
*/

static int rtl8198c_i2c_doxfer(struct rtl8198c_i2c *i2c,  struct i2c_msg *msgs, int num)
{
	unsigned long iicstat, timeout;
	int spins = 20;
	int ret;
	int count=0;

	if (i2c->suspended)
		return -EIO;

	ret = rtl8198c_i2c_set_master(i2c);

	if (ret != 0) 
	{	dev_err(i2c->dev, "cannot get bus (error %d)\n", ret);
		ret = -EAGAIN;
		goto out;
	}

for(count=0;count<num;count++)
{
	spin_lock_irq(&i2c->lock);

	struct i2c_msg *m;
      	m = &msgs[ count ];
	i2c->msg     = m;
	i2c->msg_num = num;
	i2c->msg_ptr = 0;

       if (count==0){
	i2c->msg_idx = 0;
       }else{
             i2c->msg_idx = i2c->msg_idx;
       }

	i2c->state   = STATE_START;

	spin_unlock_irq(&i2c->lock);

	rtl8198c_i2c_enable_irq(i2c); 

    	rtl8198c_i2c_message_start(i2c, m);

	timeout = wait_event_timeout(i2c->wait, i2c->msg_num == 0, HZ * 5);
//	timeout = wait_event_timeout(i2c->wait, i2c->msg_num == 0, HZ * 500);   //FPGA, too slow
	ret = i2c->msg_idx;

	/* having these next two as dev_err() makes life very
	 * noisy when doing an i2cdetect */

	if (timeout == 0)
		dev_dbg(i2c->dev, "timeout\n");
	else if (ret != num)
		dev_dbg(i2c->dev, "incomplete xfer (%d)\n", ret);

}
#if 0 //wei del
	/* ensure the stop has been through the bus */

	dev_dbg(i2c->dev, "waiting for bus idle\n");

	/* first, try busy waiting briefly */
	do {
		cpu_relax();
		iicstat = readl(i2c->regs + RTL8198c_IICSTAT);
	} while ((iicstat & RTL8198c_IICSTAT_START) && --spins);

	/* if that timed out sleep */
	if (!spins) {
		msleep(1);
		iicstat = readl(i2c->regs + RTL8198c_IICSTAT);
	}

	if (iicstat & RTL8198c_IICSTAT_START)
		dev_warn(i2c->dev, "timeout waiting for bus idle\n");
#endif
 out:
	return ret;
}

//===========================================================
/* rtl8198c_i2c_xfer
 *
 * first port of call from the i2c bus code when an message needs
 * transferring across the i2c bus.
*/
static int rtl8198c_i2c_xfer(struct i2c_adapter *adap,	struct i2c_msg *msgs, int num)
{
	struct rtl8198c_i2c *i2c = (struct rtl8198c_i2c *)adap->algo_data;
	int retry;
	int ret;

#if 1 //wei add
	pm_runtime_get_sync(&adap->dev);
//	clk_enable(i2c->clk);

	for (retry = 0; retry < adap->retries; retry++) {
		ret = rtl8198c_i2c_doxfer(i2c, msgs, num);

		if (ret != -EAGAIN) {
//			clk_disable(i2c->clk);
			pm_runtime_put_sync(&adap->dev);

			return ret;
		}

		dev_dbg(i2c->dev, "Retrying transmission (%d)\n", retry);

		udelay(100);
	}
//	clk_disable(i2c->clk);
	pm_runtime_put_sync(&adap->dev);
#endif	
	return -EREMOTEIO;
}
//===========================================================

/* declare our i2c functionality */
static u32 rtl8198c_i2c_func(struct i2c_adapter *adap)
{
	return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL | I2C_FUNC_PROTOCOL_MANGLING;
//I2C_FUNC_I2C | BI2C_FUNC_10BIT_ADDR | BI2C_FUNC_SMBUS_READ_BYTE | BI2C_FUNC_SUMBUS_WRITE_BYTE
}

//===========================================================

/* i2c bus registration info */
static const struct i2c_algorithm rtl8198c_i2c_algorithm = {
	.master_xfer		= rtl8198c_i2c_xfer,
	.functionality		= rtl8198c_i2c_func,
};


//===========================================================


//===========================================================
/* rtl8198c_i2c_init
 *
 * initialise the controller, set the IO lines and frequency
*/

static int rtl8198c_i2c_init(struct rtl8198c_i2c *i2c)
{
#if 0 //wei add
	unsigned long iicon = RTL8198c_IICCON_IRQEN | RTL8198c_IICCON_ACKEN;
	struct rtl8198c_platform_i2c *pdata;
	unsigned int freq;

	/* get the plafrom data */

	pdata = i2c->pdata;



	/* write slave address */
	writeb(pdata->slave_addr, i2c->regs + RTL8198c_IICADD);

	dev_info(i2c->dev, "slave address 0x%02x\n", pdata->slave_addr);

	writel(iicon, i2c->regs + RTL8198c_IICCON);

	/* we need to work out the divisors for the clock... */

	if (rtl8198c_i2c_clockrate(i2c, &freq) != 0) {
		writel(0, i2c->regs + RTL8198c_IICCON);
		dev_err(i2c->dev, "cannot meet bus frequency required\n");
		return -EINVAL;
	}

	/* todo - check that the i2c lines aren't being dragged anywhere */

	dev_info(i2c->dev, "bus frequency set to %d KHz\n", freq);

#endif
#if  1//wei add

	//enable i2c ip
	char * baseptr = ioremap( 0xf8000000, 1024*4);
	writel((readl(baseptr+0x18) | 0x2), baseptr+0x18);
	iounmap(baseptr);
	if((readl(I2C_STATUS)&I2C_STATUS_READY) ==0)
		printk("I2C hardware not ready\n");
/*
	REG32(I2C_CONFIG) = (0x04<<I2C_SCONFIG_CLKSEL_OFFSET)  |
						(0x0f<<I2C_SCONFIG_POLLINGTIMER_OFFSET);
*/
/*  //FPGA
	REG32(I2C_CONFIG) = (0x1ff<<I2C_SCONFIG_CLKSEL_OFFSET)  |
						(0x0f<<I2C_SCONFIG_POLLINGTIMER_OFFSET);
*/
/* yllin
	REG32(I2C_CONFIG) = (0x1ff<<I2C_SCONFIG_CLKSEL_OFFSET)  |
						(0x1ff<<I2C_SCONFIG_POLLINGTIMER_OFFSET);
*/

	writel((0x1ff<<I2C_SCONFIG_CLKSEL_OFFSET) | (0x1ff<<I2C_SCONFIG_POLLINGTIMER_OFFSET), I2C_CONFIG);
       writel(readl(I2C_STATUS), I2C_STATUS); //clear all fail.
	
/* yllin	
	REG32(I2C_STATUS) = REG32(I2C_STATUS);  //clear all fail.
*/
#endif	

//Set I2C pin MUX    
    REG32(0xb8000108) =  (REG32(0xb8000108)&(0x7<<15))|(0x4<<15); 
    REG32(0xb8000018) = REG32(0xb8000018)|(1<<1); 

	return 0;
}

struct rtl8198c_i2c * myi2c;
//===========================================================
/* rtl8198c_i2c_probe
 *
 * called by the bus driver when a suitable device is found
*/
static int rtl8198c_i2c_probe(struct platform_device *pdev)
{

	struct rtl8198c_i2c *i2c;
	struct rtl8198c_platform_i2c *pdata = NULL;
	struct resource *res;
	struct resource *mem;
	int irq;
	int ret;

	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(&pdev->dev, "no mem resource?\n");
		printk(KERN_INFO "RTK I2C probe: return error 1\n");
		return -EINVAL;
	}


	irq = platform_get_irq(pdev, 0);
	if (irq < 0) {
		dev_err(&pdev->dev, "no irq resource?\n");
		printk(KERN_INFO "RTK I2C probe: return error 2\n");
		return irq; /* -ENXIO */
	}
	

	i2c = devm_kzalloc(&pdev->dev, sizeof(struct rtl8198c_i2c), GFP_KERNEL);
	if (!i2c) 
	{
		dev_err(&pdev->dev, "no memory for state\n");
		printk(KERN_INFO "RTK I2C probe: return error 3\n");
		return -ENOMEM;
	}

	i2c->pdata = devm_kzalloc(&pdev->dev, sizeof(*pdata), GFP_KERNEL);
	if (!i2c->pdata) {
		ret = -ENOMEM;
		goto err_noclk;
	}

	if (pdata)
		memcpy(i2c->pdata, pdata, sizeof(*pdata));

	i2c_set_adapdata(&i2c->adap, i2c);
	strlcpy(i2c->adap.name, "rtl8198c-i2c", sizeof(i2c->adap.name));
	i2c->adap.owner   = THIS_MODULE;
	i2c->adap.algo    = &rtl8198c_i2c_algorithm;
	i2c->adap.retries = 2;
	i2c->adap.class   = I2C_CLASS_HWMON | I2C_CLASS_SPD;
	i2c->tx_setup     = 50;
	
	spin_lock_init(&i2c->lock);
	init_waitqueue_head(&i2c->wait);

	/* find the clock and enable it */
	i2c->dev = &pdev->dev;
	i2c->irq = irq;

	i2c->ioarea = request_mem_region(mem->start, resource_size(mem),	 pdev->name);
	if (i2c->ioarea == NULL) 
	{	dev_err(&pdev->dev, "cannot request IO\n");
		ret = -ENXIO;
		goto err_clk;
	}

	myi2c = i2c;
	i2c->regs = ioremap(mem->start, resource_size(mem));
	if (i2c->regs == NULL) 
	{	dev_err(&pdev->dev, "cannot map IO\n");
		ret = -ENXIO;
		goto err_ioarea;
	}
	else
		myi2c->regs = i2c->regs;


	/* setup info block for the i2c core */
	i2c->adap.algo_data = i2c;
	i2c->adap.dev.parent = &pdev->dev;

	/* initialise the i2c controller */
	ret = rtl8198c_i2c_init(i2c);
	if (ret != 0)
		goto err_iomap;
#if 1	
	/* find the IRQ for this unit (note, this relies on the init call to
	 * ensure no current IRQs pending
	 */
	rtl8198c_i2c_disable_irq(&i2c);	
	ret = devm_request_irq(&pdev->dev, i2c->irq, rtl8198c_i2c_irq, IRQF_SHARED,
			pdev->name, i2c);
	if (ret) {
		printk(KERN_INFO "RTK I2C probe: return error 7\n");
		dev_err(&pdev->dev, "failure requesting irq %i\n", i2c->irq);
		return ret;
	}
		

	/* Note, previous versions of the driver used i2c_add_adapter()
	 * to add the bus at any number. We now pass the bus number via
	 * the platform data, so if unset it will now default to always
	 * being bus 0.
	 */
	i2c->adap.nr = -1;//i2c->pdata->bus_num;
	i2c->adap.dev.of_node = pdev->dev.of_node;

	ret = i2c_add_numbered_adapter(&i2c->adap);
	if (ret < 0) 
	{	dev_err(&pdev->dev, "failed to add bus to i2c core\n");
		goto err_irq;
	}

	platform_set_drvdata(pdev, i2c);
	of_i2c_register_devices(&i2c->adap);
	pm_runtime_enable(&pdev->dev);

	//dev_info(&pdev->dev, "%s: Realtek I2C adapter\n", dev_name(&i2c->adap.dev));
	
#endif
	return 0;



 err_irq:
	free_irq(i2c->irq, i2c);

 err_iomap:
 
	iounmap(i2c->regs);

 err_ioarea:
 
	release_resource(i2c->ioarea);
	kfree(i2c->ioarea);


err_clk:
 err_noclk:
	return ret;
}
//===========================================================
/* rtl8198c_i2c_remove
 *
 * called when device is removed from the bus
*/
static int rtl8198c_i2c_remove(struct platform_device *pdev)
{
	struct rtl8198c_i2c *i2c = platform_get_drvdata(pdev);

	pm_runtime_disable(&i2c->adap.dev);
	pm_runtime_disable(&pdev->dev);


	i2c_del_adapter(&i2c->adap);
	free_irq(i2c->irq, i2c);

	iounmap(i2c->regs);

	return 0;
}
//===========================================================
/* device driver for platform bus bits */

MODULE_DEVICE_TABLE(platform, rtl8198c_driver_ids);

//===========================================================
static const struct of_device_id rtl8198c_i2c_match[] = {
	{ .compatible = "rtk,rtl-i2c" },
	{ /* Sentinel */ }
};
MODULE_DEVICE_TABLE(of, dw8250_of_match);

static struct platform_driver rtl8198c_i2c_driver = {
	.probe		= rtl8198c_i2c_probe,
	.remove		= rtl8198c_i2c_remove,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "rtl-i2c",
//		.pm	= RTL24XX_DEV_PM_OPS,
		.of_match_table = rtl8198c_i2c_match,
	},
};

//===========================================================

static int i2c_dbg_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int  len = 0;				 
 //       len += sprintf(buf+len,"\nDevice  \n" );
	printk("Please input: echo help > /proc/usb\n");
 
	return len;
}
//------------------------------------------------------------------------------
static int i2c_dbg_write(struct file *file, const char *buff, unsigned long len, void *data)
{
	char 		tmpbuf[64];
	unsigned int	addr, reg, val;

	char       *cmd;	
	char *p;
	char *argv[10];
	int argc=0;
	int i;

struct rtl8198c_i2c *i2c = myi2c;
rtl8198c_i2c_disable_irq(myi2c);	


	memset(tmpbuf, '\0', 64);
	if (buff && !copy_from_user(tmpbuf, buff, len)) 
	{
		tmpbuf[len] = '\0';
#if 1
	//--------------------calc argc
	p=tmpbuf;
	while( *p )
	{
		if( *p != ' '  &&  *p )
		{
			argc++ ;
			while( *p != ' '  &&  *p ) p++ ;
			continue ;
		}
		p++ ;
	}	
	//------------------calc argv
	p=tmpbuf;
	int n=0;
	while( *p )
	{
		argv[n] = p ;
		while( *p != ' '  &&  *p ) p++ ;
		*p++ = '\0';
		while( *p == ' '  &&  *p ) p++ ;
		n++ ;
		if (n == 10) break;
	}
#endif


		cmd = argv[0];
		//if (cmd==NULL)		goto errout;




		if(!strcmp(cmd, "scan"))
		{	unsigned char t[1];
			for(i=0; i<0x7f; i++)
			{ 
			if(I2C_WriteRead(myi2c, i, 0, NULL, 0, NULL, 1, t, 1)==0)
				{	//printk("Device addr=%x\n", i);
					break;
				}
			}
			//devaddr=i;
		
		}
		//-----------------------------------
		else if(!strcmp(cmd, "r"))
		{	unsigned char t[128];
			if(argc>1)	addr=simple_strtol(argv[1], NULL, 16);	
			if(argc>2)	reg=simple_strtol(argv[2], NULL, 16);  

			myi2c->msg = &mymsgs;
			myi2c->msg->buf = msg_buf;
			myi2c->msg->len = 2;

			if(IRQ_DBG)
				I2C_WriteRead(myi2c, addr, 1, reg, 0, NULL,  2,  t, 0);
			else
				I2C_WriteRead(myi2c, addr, 1, reg, 0, NULL,  2,  t, 1);
			//printk("data=%x %x\n",t[0],t[1]);
		}
		//-----------------------------------
		else if(!strcmp(cmd, "w"))
		{	unsigned char t[128];
			if(argc>1)	addr=simple_strtol(argv[1], NULL, 16);	
			if(argc>2)	reg=simple_strtol(argv[2], NULL, 16);  
			if(argc>3)	val=simple_strtol(argv[3], NULL, 16);  			
			t[0]=val>>8;
			t[1]=(val&0x00ff);
			printk("data=%x %x\n",t[0],t[1]);
			/*	//write mode, if enable irq, system will crash --->
				// Unable to handle kernel NULL pointer dereference at virtual address 00000002
			if(IRQ_DBG)
				I2C_WriteRead(myi2c, addr, 1, reg, 2, t,  0,  NULL, 0);
			else */
			u8 tmpDBG = IRQ_DBG;
			IRQ_DBG = 0;
				I2C_WriteRead(myi2c, addr, 1, reg, 2, t,  0,  NULL, 1);
			IRQ_DBG = tmpDBG;
		}		
		//-----------------------------------		
		else if(!strcmp(cmd, "eer"))
		{	unsigned char t[128];
			if(argc>1)	addr=simple_strtol(argv[1], NULL, 16);			
			I2C_WriteRead(myi2c, addr, 0, NULL, 0, NULL,  1,  t, 1);
			//printk("data=%x \n",t[0]);			
			
		}
		//-----------------------------------
		else if(!strcmp(cmd, "eew"))
		{	unsigned char t[128];
			if(argc>1)	addr=simple_strtol(argv[1], NULL, 16);	
			if(argc>2)	reg=simple_strtol(argv[2], NULL, 16);  
			if(argc>3)	val=simple_strtol(argv[3], NULL, 16);  			
			t[0]=val;
			//printk("data=%x \n",t[0]);		
			
			I2C_WriteRead(myi2c, addr, 1, reg, 1, t,  0,  NULL, 1);
			
		}		
		//-----------------------------------	
		else if((!strcmp(cmd, "devr")))
		{
			/*unsigned char mybuf[4]={0};
			if(read_regs(file_master,0, mybuf, 4))
				printk("data=%x %x %x %x\n",mybuf[3],mybuf[2],mybuf[1],mybuf[0]);	
			else
				printk("devr error.\n");*/
		}
		else if(!strcmp(cmd, "r32"))
		{
			//printk("cmd r32: data=%x\n",REG32(I2C_DATAR0));
		}
		else if(!strcmp(cmd, "clear"))
		{
			REG32(I2C_STATUS)=REG32(I2C_STATUS); //clear all fail.
			//printk("irq cleared.\n");
		}
		else if(!strcmp(cmd, "enirq"))
		{
			REG32(I2C_STATUS)=0;
		}
		else if(!strcmp(cmd, "irqdbg"))
		{
			IRQ_DBG=1;
		}
		else if(!strcmp(cmd, "-irqdbg"))
		{
			IRQ_DBG=0;
		}
		else
		{
errout:	
			//printk("scan \n");		
			//printk("r \n");
			//printk("w \n");
			//printk("eer \n");
			//printk("eew \n");			
		}
		
	}
rtl8198c_i2c_enable_irq(myi2c);	
	return len;
}

//===========================================================
static int __init i2c_adap_rtl_init(void)
{

	struct proc_dir_entry *u3_dbg_proc_dir_t=NULL;
#if 0
	u3_dbg_proc_dir_t = create_proc_entry("i2c",0, NULL);
	if(u3_dbg_proc_dir_t!=NULL)	
	{	u3_dbg_proc_dir_t->read_proc = i2c_dbg_read;
		u3_dbg_proc_dir_t->write_proc= i2c_dbg_write;			
	}
	else
	{	printk("can't create proc entry for u3dbg");
	}		
#else
	static const struct file_operations i2c_proc_fops = {
	.owner = THIS_MODULE,
	.read = i2c_dbg_read,
	.write = i2c_dbg_write,
	};
	if ((u3_dbg_proc_dir_t = proc_create("i2c",0, NULL,&i2c_proc_fops)) == NULL) {
		//printk("can't create proc entry for u3dbg");
	}
	
#endif

	//return platform_driver_register(&rtl8198c_i2c_driver);
	platform_driver_probe(&rtl8198c_i2c_driver,rtl8198c_i2c_probe);
}
subsys_initcall(i2c_adap_rtl_init);
//===========================================================
static void __exit i2c_adap_rtl_exit(void)
{
	platform_driver_unregister(&rtl8198c_i2c_driver);
}
module_exit(i2c_adap_rtl_exit);
//===========================================================

MODULE_DESCRIPTION("Realtek I2C Bus driver");
MODULE_LICENSE("GPL");
//using proc
//# echo "scan" > /proc/i2c 
