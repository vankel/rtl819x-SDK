/*
 * AHCI SATA platform driver
 *
 * Copyright 2004-2005  Red Hat, Inc.
 *   Jeff Garzik <jgarzik@pobox.com>
 * Copyright 2010  MontaVista Software, LLC.
 *   Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 */

#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/gfp.h>
#include <linux/module.h>
#include <linux/pm.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/libata.h>
#include <linux/ahci_platform.h>
#include "ahci.h"

static void ahci_host_stop(struct ata_host *host);

enum ahci_type {
	AHCI,		/* standard platform ahci */
#if 0  //wei add   
	IMX53_AHCI,	/* ahci on i.mx53 */
	STRICT_AHCI,	/* delayed DMA engine start */
#endif    
};

static struct platform_device_id ahci_devtype[] = {
	{
		.name = "ahci",
		.driver_data = AHCI,
	}, 
#if 0    //wei add
    {
		.name = "imx53-ahci",
		.driver_data = IMX53_AHCI,
	}, {
		.name = "strict-ahci",
		.driver_data = STRICT_AHCI,
	},
#endif    
    {
		/* sentinel */
	}
};
MODULE_DEVICE_TABLE(platform, ahci_devtype);

static struct ata_port_operations ahci_platform_ops = {
	.inherits	= &ahci_ops,
	.host_stop	= ahci_host_stop,
};

static struct ata_port_operations ahci_platform_retry_srst_ops = {
	.inherits	= &ahci_pmp_retry_srst_ops,
	.host_stop	= ahci_host_stop,
};

static const struct ata_port_info ahci_port_info[] = {
	/* by features */
	[AHCI] = {
		.flags		= AHCI_FLAG_COMMON,
		.pio_mask	= ATA_PIO4,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &ahci_platform_ops,
	},
#if 0    //wei add
	[IMX53_AHCI] = {
		.flags		= AHCI_FLAG_COMMON,
		.pio_mask	= ATA_PIO4,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &ahci_platform_retry_srst_ops,
	},
	[STRICT_AHCI] = {
		AHCI_HFLAGS	(AHCI_HFLAG_DELAY_ENGINE),
		.flags		= AHCI_FLAG_COMMON,
		.pio_mask	= ATA_PIO4,
		.udma_mask	= ATA_UDMA6,
		.port_ops	= &ahci_platform_ops,
	},
#endif 
};

static struct scsi_host_template ahci_platform_sht = {
	AHCI_SHT("ahci_platform"),
};

//=========================================================================================
#if 1 //wei add


//#define REG32(reg)	(*(volatile unsigned int *)(reg))
#define REG32(reg)   (*(volatile unsigned int *)((unsigned int)reg)) 

//u8 *aux_base = (void __iomem *)0xb82ea000;
//u8 *base = (void __iomem *)0xb82e8000;
//======================================================================
#ifdef SATA_REG_LITTLE_ENDIAN
//for kernel override read/write function
u32 ahci_readl(unsigned int *addr)
{
	volatile u32 tmp;
	tmp=cpu_to_le32((*(volatile unsigned int *)(addr)));
#ifdef OHCHIP_AHCI_DEBUG
	printk("READ(%x)=%x\n",addr,tmp);
#endif
	return tmp;
}
//-----------------------------------------------------
void ahci_writel(unsigned int value,unsigned int *addr)
{
#ifdef OHCHIP_AHCI_DEBUG
	printk("WRITE(%x)=%x\n",addr,value);
#endif
	(void)((*(volatile unsigned int *) (addr)) = (cpu_to_le32(value)));
}
#endif
//===================================================================
static u32 REG32_R(u32  reg)
{
	volatile u32 tmp;
	tmp=cpu_to_le32(REG32(reg));
//	printk("R:(%x)=%x\n",(u32)reg,tmp);
	return tmp;
}
//-----------------------------------------------------
static void REG32_W(u32  reg,u32 value)
{
	//printk("before write reg(%x)=%x--->%x\n",(u32)reg,REG32_R(reg),value);
	//printk("W:(%x)=%x--->%x\n",(u32)reg,REG32_R(reg),value);
	REG32(reg)=cpu_to_le32(value);

	//int delay_ms=2;	
	//mdelay(delay_ms);
	//printk("after write reg(%x)=%x\n",(u32)reg,REG32_R(reg));	
}
//=================================================================
static u32 AUREG32_R(u32  reg)
{	volatile u32 tmp;
	tmp=REG32(reg);
	//printk("AR:(%x)=%x\n",(u32)reg,tmp);
	return tmp;
}
//-------------------------------------------------------
static void AUREG32_W(u32  reg,u32 value)
{
	//printk("AW:(%x)=%x\n",(u32)reg,value);
	REG32(reg)=(value);
}
//=================================================================
static u32 sata_phy_read(u32 portnum, u32 addr)
{
	//AUREG32_W(0xb82ea008, (0<<31)|(1<<22) | (addr<<16) );	//patch
	u32 pmdio_addr;
	if(portnum==0)		pmdio_addr=0xb82ea008;
	else			pmdio_addr=0xb82ea008+0x20;
	
	AUREG32_W(pmdio_addr, (0<<31)|(1<<22) | (addr<<16) );	
	//mdelay(10);
	while( (AUREG32_R(pmdio_addr) &(1<<31))==0 ) {};  //auto reverse when hw complete. 

	u32 r=(AUREG32_R(pmdio_addr)&0xffff);
	return r;	
}
//--------------------------------------------------------------------
static void sata_phy_write(u32 portnum, u32 addr,u32 val)
{
	u32 tmp;
	u32 pmdio_addr;
	//AUREG32_W(0xb82ea008, (1<<31)|(1<<22)|(addr<<16)|(val&0xffff));   //patch
	
	if(portnum==0)		pmdio_addr=0xb82ea008;
	else			pmdio_addr=0xb82ea008+0x20;
	
	AUREG32_W(pmdio_addr, (1<<31)|(1<<22)|(addr<<16)|(val&0xffff)); 		
	printk("write reg=%x v=%x",addr,val);
		
	//mdelay(10*20);
	while( AUREG32_R(pmdio_addr) &(1<<31) ) {};  //auto reverse when hw complete. 
		
	tmp=sata_phy_read(portnum, addr);	
	
	if(tmp!=val) 	printk("  Error, read_back=%x\n",tmp);
	else		printk("\n");
	
//		printk("write addr=%x, value=%x, read_back=%x\n",addr,val,tmp);
}
//--------------------------------------------------------------------
void dump_ahci_phy_parameter()
{
	int i,data;
	for(i=0;i<=0x1f;i++)
	{	data=sata_phy_read(0,i);
		printk("P%d, r=%x v=%x\n",0,i,data);
	}
	printk("\n");
	for(i=0;i<=0x1f;i++)
	{	data=sata_phy_read(1,i);
		printk("P%d, r=%x v=%x\n",1,i,data);
	}	
}

//--------------------------------------------------------------------
static void sata_mdio_reset(u32 portnum)
{
	u32 aux_base = (void __iomem *)0xb82ea000;

	// Reset MDC/MDIO
	if(portnum==0)
	{
		AUREG32_W(aux_base, AUREG32_R(aux_base) & ~(1<<25)); //phy mdc/mdio reset
		mdelay(100);
		AUREG32_W(aux_base, AUREG32_R(aux_base) | (1<<25)); //phy mdc/mdio reset clear
		mdelay(100);
	}
	else
	{
		AUREG32_W(aux_base, AUREG32_R(aux_base) & ~(1<<11)); //phy mdc/mdio reset
		mdelay(100);
		AUREG32_W(aux_base, AUREG32_R(aux_base) | (1<<11)); //phy mdc/mdio reset clear
		mdelay(100);
	}
}
//========================================================================
void set_sata_phy_param_for_port(u32 portnum)
{
 spinlock_t my_lock;
 spin_lock_init(&my_lock);
 spin_lock(&my_lock);

	int i;
	printk("===========================\nSet PHY Parameter PORT %d\n", portnum);
#if 0 // 2012/6/12 new PHY params from Johnny_Chiang
	sata_phy_write(0x00, 0x1067);
	sata_phy_write(0x01, 0x0003);
	sata_phy_write(0x02, 0x2d18);
	sata_phy_write(0x04, 0x5000);
	sata_phy_write(0x05, 0x08d3);
	sata_phy_write(0x06, 0xdc48);
	sata_phy_write(0x08, 0x18db);
	sata_phy_write(0x0d, 0xfd62);
	//sata_phy_write(0x1d, 0xfd10);

#else

#if 0
	sata_phy_write(portnum, 0x01, 0x0003);
	sata_phy_write(portnum, 0x0B, 0x0111);
//	sata_phy_write(0x1E, 0x04EA);
	sata_phy_write(portnum, 0x1E, 0x84Eb);
//	sata_phy_write(0x1E, 0x84Ea);
	
	sata_phy_write(portnum, 0x07, 0x31FE);
	sata_phy_write(portnum, 0x0C, 0x3FF9);
	sata_phy_write(portnum, 0x03, 0x6D09);
	sata_phy_write(portnum, 0x02, 0x2D18);
	sata_phy_write(portnum, 0x08, 0x18DA);
	sata_phy_write(portnum, 0x0A, 0x03A0);
	sata_phy_write(portnum, 0x09, 0x9310);
//	sata_phy_write(0x09, 0x9300); //wei add	, new
	sata_phy_write(portnum, 0x0D, 0xFA60);
	//sata_phy_write(0x0D, 0xFA61); //debug only
	sata_phy_write(portnum, 0x0F, 0x0000);
	sata_phy_write(portnum, 0x05, 0x0BA3);
	sata_phy_write(portnum, 0x06, 0xF4C8);

	//sata_phy_write(0x1d, 0xFF14);	 //RDC 2011/5/2: add this phy parameter.
	//sata_phy_write(0x1d, 0xFF11);	 //RDC 2011/5/2: add this phy parameter.	
	//printk("phy:0x1d=%x\n",sata_phy_read(0x1d));


	//sata_phy_write(0x0e, 0x18f9);
	//printk("oe=%x\n",sata_phy_read(portnum, 0x0e));
	//debug for phy state 1~b: 0x10--->0x1e00

	sata_phy_write(portnum, 0x00, 0x0067);

	//for rdc test
	sata_phy_write(portnum, 0x0a, 0x0ba0);
	sata_phy_write(portnum, 0x0a, 0x03a0);
	sata_phy_write(portnum, 0x10, 0x1e00);
	sata_phy_write(portnum, 0x10, 0x1e80);
#endif	
#endif	
#if 1 //8198C ASIC
	sata_phy_write(portnum, 0x04, 0x2000);
	sata_phy_write(portnum, 0x06, 0x9438);

#endif

//	sata_phy_write(0x1d, 0xfc90);  //0x19[10:6] = 10010

#if 1
//wei add
//dbg out
//	sata_phy_write(portnum, 0x09, 0x9300); //wei add	, new
	//sata_phy_write(0x10, 0x1F80);   //phy dbg port

	AUREG32_W(0xb82ea000, AUREG32_R(0xb82ea000)| (0x7<<16));   //mac dbg port
	//AUREG32_W(0xb82ea00c, AUREG32_R(0xb82ea00c)| (1<<17));  //force phy reset

	AUREG32_W(0xb82ea000, AUREG32_R(0xb82ea000)& ~(0x3<<22));   //180 degree
	AUREG32_W(0xb82ea000, AUREG32_R(0xb82ea000)| (0x2<<22));   //	
#endif

//dump all phy reg
#if 0
	for(i=0; i<=0x1f; i++)
	printk("=> phy reg %x, v=%x\n", i, sata_phy_read(portnum, i));
#endif	

 spin_unlock(&my_lock);

}
//-----------------------------------------------------------------
#if 0
static void phy_reset(void)
{
	void __iomem *base, __iomem *aux_base;
	base = (void __iomem *)0xb82e8000;
	aux_base = (void __iomem *)0xb82ea000;

	sata_mdio_reset(0);

#if 1 //wei del, phy conjection,
	// Set PHY Param
	set_sata_init_phy_param_for_ahci();
#endif	
	// P0CTL: 0x300-Transitions to both Parital and Slumber states disabled, 1: Perform interface communication
	REG32_W(base+0x12c,REG32_R(base+0x12c)|1);
	mdelay(10);
#if 0 //wei del, phy conjection,	
	//REG32(aux_base+0x20)=0x88000000;
	AUREG32_W(aux_base+0x20, 0x88000000);
	mdelay(1);
#endif
	// P0SCTL: 1: Perform interface communication (disable)
	REG32_W(base+0x12c,0);
	mdelay(1);

}
#endif
//-----------------------------------------------------------------
void realtek_sata_phypatch()
{
u8 *base = (void __iomem *)0xb82e8000;
#if 0
	if((REG32_R(base+0x110)&0x80000080)==0x80000080)
	{
		REG32_W(base+0x110,0x80000080);
		mdelay(10);
	}
	
	printk("110=%x 118=%x hotplug_change=%d\n",REG32_R(base+0x110),REG32_R(base+0x118),hotplug_change);
	REG32(aux_base+0xc)=0x10011; //for hot-plug test (by hanyi: 10011:by outside signal, 10022:force 0, 10033: force 1)

#endif



	printk("set PHY Params\n");	

#if 1
	//REG32(0xb8000010) &= ~(1<<11);  //disable switch
	REG32(0xb800010c)= (REG32(0xb800010c)&~(3<<21)) | (1<<21);  //P5TXC -> SATA
#endif

	#define CLK_MGR 0xb8000010	
	REG32(CLK_MGR)|=(1<<24) | (1<<19) | (1<<20);  //enable sata ip

#ifndef SATA_REG_LITTLE_ENDIAN
	//swap register
	REG32(0xb82ea000) &= ~(1<<30);
#endif

	//enable sata phy power
	#define SYS_SATA_0 0xb8000040
	#define SYS_SATA_1 0xb8000044	
/*	
	REG32(SYS_SATA_0)|=(1<<0);
	REG32(SYS_SATA_1)|=(1<<0);	
*/
	REG32(SYS_SATA_0)=0xff;
	REG32(SYS_SATA_1)=0xff;	

#if 1 //I'm BIOS setting HWInit & HOTPLUG
	//HWINIT
	writel( (1<<28),base+0x00);	//CAP
	writel( (1<<20)|(1<<19),base+0x118);	//P0CMD
	writel( (1<<20)|(1<<19),base+0x198);	//P1CMD

#endif
	sata_mdio_reset(0);
	set_sata_phy_param_for_port(0);
	sata_mdio_reset(1);
	set_sata_phy_param_for_port(1);	
/*	
	REG32(base)=REG32(base)|0x10000000; //enable mp switch
*/
	//printk("power on ...wait port reset\n");
	//mdelay(1000);
	
}
#endif
//=========================================================================================
#if AHCI_PROC_DBG
#include <linux/proc_fs.h>

static int sata_enable=1;  
int sata_err_flag=0;
int sata_err_cnt=0;

//======================================================================
static void dbdump(unsigned char * pData, int len)
{
	unsigned char *sbuf = pData;	
	int length=len;

	int i=0,j,offset;
	printk(" [Addr]   .0 .1 .2 .3 .4 .5 .6 .7 .8 .9 .A .B .C .D .E .F\r\n" );

	while(i< length)
	{		
			
			printk("%08X: ", (int)(sbuf+i) );

			if(i+16 < length)
				offset=16;
			else			
				offset=length-i;
			

			for(j=0; j<offset; j++)
				printk("%02x ", sbuf[i+j]);	

			for(j=0;j<16-offset;j++)	//a last line
			printk("   ");


			printk("    ");		//between byte and char
			
			for(j=0;  j<offset; j++)
			{	
				if( ' ' <= sbuf[i+j]  && sbuf[i+j] <= '~')
					printk("%c", sbuf[i+j]);
				else
					printk(".");
			}
			printk("\n\r");
			i+=16;
	}

	//printk("\n\r");	
}
//----------------------------------------------------------------
static void dwdump(unsigned int * pData, int count)
{
	unsigned int *sbuf = pData;	
	int length=count;  //is word unit

	//printk("Addr=%x, len=%d", sbuf, length);	
	printk(" [Addr]    .0.1.2.3    .4.5.6.7    .8.9.A.B    .C.D.E.F" );
	
	{
		int i;		
		for(i=0;i<length; i++)
		{
			if((i%4)==0)
			{	printk("\n\r");
				printk("%08X:  ", (int)(sbuf+i) );
			}
			
			printk("%08X    ", sbuf[i]);
			//sbuf[i];
			
		}
		printk("\n\r");
	}	
}
//----------------------------------------------------------------

#define  _atoi(x,z) simple_strtol(x,NULL,z)

static void CMD_BitRead(int argc, char* argv[])
{

	if(argc<3)
	{//dump all	
	       printk("\n"); 	   
		printk("br addr 2 1 \n");		   
		printk("br addr 2 1 : exp_value \n");
		return ;
	}

	int addr,val;
	int i;
	unsigned int check=0,mask=1,expval=0;
	unsigned int bit_st=0,bit_end=0;
	
	
	addr= _atoi(argv[0], 16);		
	val=REG32(addr);
	//printk("%x\n", val );
	
	bit_st= _atoi(argv[1], 10);	
	bit_end= _atoi(argv[2], 10);	

	for(i=0; i<(bit_st-bit_end); i++)
		mask |= (mask<<1);
	mask=(mask<<bit_end);	
	//printk("mask=%x\n", mask );

	if(argc>=4 && *(argv[3]) == ':')
	{	check=1;
		expval = _atoi( argv[4],16 );						
	}

	if(!check)
	{	printk("addr %08x=%x, b(%d-%d)=%x \n", addr, val, bit_st,bit_end, (val&mask)>>bit_end  );			
		
	}
	else
	{
		if( (val&mask) !=(expval<<bit_end) )
		{	
			printk("Fail, addr=%08x val=%x, b(%d-%d) expval=%x \n", addr, val, bit_st, bit_end, expval);
		}
		else
			printk("Pass \n");

	}		

}
//----------------------------------------------------------------
static void CMD_BitWrite(int argc, char* argv[])
{
	if(argc<4)
	{	printk("ex: bw b8001000  3 1  7 \n");			
		return;	
	}

	unsigned int addr,mask=1,val;
	unsigned int bit_st=0,bit_end=0;
	int i;
	

	addr= _atoi(argv[0], 16);		
	bit_st= _atoi(argv[1], 10);	
	bit_end= _atoi(argv[2], 10);	

	for(i=0; i<(bit_st-bit_end); i++)
		mask |= (mask<<1);
	mask=(mask<<bit_end);	
	//printk("mask=%x\n", mask );
	
	val = _atoi( argv[3],16 );		

	REG32(addr)= (REG32(addr) & (~mask)) | (val<<bit_end) ;	
}
//----------------------------------------------------------------

static int ahci_dbg_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    int  len = 0;				 
 //       len += sprintf(buf+len,"\nDevice  \n" );
	printk("Please input: echo help > /proc/usb\n");
 
	return len;
}
//------------------------------------------------------------------------------
 int ahci_dbg_write(struct file *file, const char *buff, unsigned long len, void *data)
{
	char 		tmpbuf[64];
	unsigned int	port=0,addr=0, val=0;

	unsigned char       *cmd;	
	unsigned char *p;
	#define MAX_ARGV 10
	unsigned char *argv[MAX_ARGV];
	int argc=0;

	memset(tmpbuf, 0, 64);
	if (buff && !copy_from_user(tmpbuf, buff, len)) 
	{
		tmpbuf[len] = '\0';
#if 1
	//--------------------calc argc & argv
	p=tmpbuf;
	memset( argv, 0, MAX_ARGV*sizeof(char *) );	
	while( *p )
	{
		argv[argc] = p ;
		while( *p && *p != ' '  && *p != 0x0d &&  *p!=0x0a ) p++ ;   //skip word
		*p++ = '\0';
		while( *p && *p == ' '  &&  *p==0x0d && *p==0x0a ) p++ ;  //skip space
		argc++ ;
		if (argc == MAX_ARGV) break;
	}
	//printk("argc=%d, argv[0]=%s, len=%d\n", argc, argv[0], strlen(argv[0]) );
#endif
		

		cmd = argv[0];
		//if (cmd==NULL)		goto errout;

		if(!strcmp(cmd, "phyr"))
		{
			if(argc>1)	port=simple_strtol(argv[1], NULL, 16);
			if(argc>2)	addr=simple_strtol(argv[2], NULL, 16);			
			if(port==0x9999)
			{
				dump_ahci_phy_parameter();
                                return len;
			}
			val=sata_phy_read(port, addr);  
			printk("Read port%d, reg=%x, val=%04x\n",port, addr, val);
		}
		else if(!strcmp(cmd, "phyw"))
		{
			if(argc>1)	port=simple_strtol(argv[1], NULL, 16);			
			if(argc>2)	addr=simple_strtol(argv[2], NULL, 16);	
			if(argc>3)	val=simple_strtol(argv[3], NULL, 16);  
			
			printk("Write port%d, addr=%x val=%x\n", port, addr, val); 			
            		sata_phy_write(port, addr,val);   
		}
		//-----------------------------------
		else if(!strcmp(cmd, "dw"))
		{
			if(argc>1)	addr=simple_strtol(argv[1], NULL, 16);	
			val=4;
			if(argc>2)	val=simple_strtol(argv[2], NULL, 16);			
			dwdump(addr, val);						
		}
		else if(!strcmp(cmd, "ew"))
		{
			if(argc>1)	addr=simple_strtol(argv[1], NULL, 16);	
			if(argc>2)	val=simple_strtol(argv[2], NULL, 16);   			
			printk("w %x=%x\n", addr, val); 
			REG32(addr)=val;
            		//write_reg(addr,val);   
		}	
		//-----------------------------------
		else if(!strcmp(cmd, "br"))
		{	CMD_BitRead(argc-1, argv+1);						
		}
		else if(!strcmp(cmd, "bw"))
		{	CMD_BitWrite(argc-1, argv+1);	
		}		
		//-----------------------------------
		else if(!strcmp(cmd, "ahci"))
		{
#if 0		
			if(argc>1)	
			{	addr=simple_strtol(argv[1], NULL, 16);	
				sata_enable=addr;
			}
			printk("en =%x\n", sata_enable);
			
			if(sata_enable==1) 
			{	int ahci_init(void);
				ahci_init();
			}
			if(sata_enable==0)
			{	void ahci_exit(void);
				ahci_exit();				
			}				
#endif			
		}	
		//-----------------------------------		
//		else if(!strcmp(cmd, "phyinit")) 	{	set_ahci_phy_parameter();	}			
		//-----------------------------------		
		
		else if(!strcmp(cmd, "pinmux"))
		{	if(argc>1)	addr=simple_strtol(argv[1], NULL, 16);

			if(addr==0)
			{	REG32(0xb8000100)= (REG32(0xb8000100)&~(7<<0)) | (1<<0);
				REG32(0xb8000100)= (REG32(0xb8000100)&~(7<<3)) | (1<<3);
				REG32(0xb8000100)= (REG32(0xb8000100)&~(7<<6)) | (1<<6);
				REG32(0xb8000100)= (REG32(0xb8000100)&~(7<<9)) | (1<<9);
				printk( "	GPIO_0	asphy_cp_det_n    (I)\n"\
					"	GPIO_1  asphy_cp_det_n_p1 (I)\n"\
					"	GPIO_2  asphy_cp_pod      (O)\n"\
					"	GPIO_3  asphy_cp_pod_p1   (O)\n"  );
			}
			else
			{
				REG32(0xb8000110)= (REG32(0xb8000110)&~(3<<0)) | (1<<0);
				REG32(0xb8000110)= (REG32(0xb8000110)&~(3<<14)) | (1<<14);
				REG32(0xb8000110)= (REG32(0xb8000110)&~(3<<22)) | (1<<22);
				REG32(0xb800010c)= (REG32(0xb800010c)&~(3<<21)) | (1<<21);
				printk( "	P5_RXCTL  asphy_cp_det_n    (I)\n"\
					"	P5_RXD3   asphy_cp_det_n_p1 (I)\n"\
					"	P5_RXC    asphy_cp_pod      (O)\n"\
					"	P5_TXC    asphy_cp_pod_p1   (O)\n"  );				

			}

		}		
		else if(!strcmp(cmd, "err"))
		{	
			if(argc>1)
			{	addr=simple_strtol(argv[1], NULL, 16);
		
				if(addr==0)
				{	sata_err_flag=0;
					sata_err_cnt=0;
				}
			}
			else
				printk("err cnt=%d, flag=%x\n", sata_err_cnt, sata_err_flag);
	
		}		
	
		
		
		else
		{
errout:		
			printk("phyr \n");
			printk("phyw \n");				
			printk("dw addr len\n");
			printk("ew addr val\n");	
			printk("ahci val:  enable mac\n");				
		}
		
	}
	return len;
}


static struct proc_dir_entry *ahci_dbg_proc_dir_t=NULL;


static void ahci_proc_debug_init(void)
{
	if(ahci_dbg_proc_dir_t==NULL)
	{	ahci_dbg_proc_dir_t = create_proc_entry("sata",0, NULL);
		if(ahci_dbg_proc_dir_t!=NULL)	
		{	ahci_dbg_proc_dir_t->read_proc = ahci_dbg_read;
			ahci_dbg_proc_dir_t->write_proc= ahci_dbg_write;			
		}
		else
		{	printk("can't create proc entry for ahci dbg");
		}		
	}
}

#endif

//=========================================================================================
//static int __init ahci_probe(struct platform_device *pdev)
static int ahci_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct ahci_platform_data *pdata = dev_get_platdata(dev);
	const struct platform_device_id *id = platform_get_device_id(pdev);
	struct ata_port_info pi = ahci_port_info[id ? id->driver_data : 0];
	const struct ata_port_info *ppi[] = { &pi, NULL };
	struct ahci_host_priv *hpriv;
	struct ata_host *host;
	struct resource *mem;
	int irq;
	int n_ports;
	int i;
	int rc;
#if 1

    realtek_sata_phypatch();
#endif
	mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!mem) {
		dev_err(dev, "no mmio space\n");
		return -EINVAL;
	}

	irq = platform_get_irq(pdev, 0);
	if (irq <= 0) {
		dev_err(dev, "no irq\n");
		return -EINVAL;
	}

	if (pdata && pdata->ata_port_info)
		pi = *pdata->ata_port_info;

	hpriv = devm_kzalloc(dev, sizeof(*hpriv), GFP_KERNEL);
	if (!hpriv) {
		dev_err(dev, "can't alloc ahci_host_priv\n");
		return -ENOMEM;
	}

	hpriv->flags |= (unsigned long)pi.private_data;

	hpriv->mmio = devm_ioremap(dev, mem->start, resource_size(mem));
	if (!hpriv->mmio) {
		dev_err(dev, "can't map %pR\n", mem);
		return -ENOMEM;
	}

	hpriv->clk = clk_get(dev, NULL);
	if (IS_ERR(hpriv->clk)) {
		dev_err(dev, "can't get clock\n");
	} else {
		rc = clk_prepare_enable(hpriv->clk);
		if (rc) {
			dev_err(dev, "clock prepare enable failed");
			goto free_clk;
		}
	}

	/*
	 * Some platforms might need to prepare for mmio region access,
	 * which could be done in the following init call. So, the mmio
	 * region shouldn't be accessed before init (if provided) has
	 * returned successfully.
	 */
	if (pdata && pdata->init) {
		rc = pdata->init(dev, hpriv->mmio);
		if (rc)
			goto disable_unprepare_clk;
	}

	ahci_save_initial_config(dev, hpriv,
		pdata ? pdata->force_port_map : 0,
		pdata ? pdata->mask_port_map  : 0);

	/* prepare host */
	if (hpriv->cap & HOST_CAP_NCQ)
		pi.flags |= ATA_FLAG_NCQ;

	if (hpriv->cap & HOST_CAP_PMP)
		pi.flags |= ATA_FLAG_PMP;

	ahci_set_em_messages(hpriv, &pi);

	/* CAP.NP sometimes indicate the index of the last enabled
	 * port, at other times, that of the last possible port, so
	 * determining the maximum port number requires looking at
	 * both CAP.NP and port_map.
	 */
	n_ports = max(ahci_nr_ports(hpriv->cap), fls(hpriv->port_map));

	host = ata_host_alloc_pinfo(dev, ppi, n_ports);
	if (!host) {
		rc = -ENOMEM;
		goto pdata_exit;
	}

	host->private_data = hpriv;

	if (!(hpriv->cap & HOST_CAP_SSS) || ahci_ignore_sss)
		host->flags |= ATA_HOST_PARALLEL_SCAN;
	else
		printk(KERN_INFO "ahci: SSS flag set, parallel bus scan disabled\n");

	if (pi.flags & ATA_FLAG_EM)
		ahci_reset_em(host);

	for (i = 0; i < host->n_ports; i++) {
		struct ata_port *ap = host->ports[i];

		ata_port_desc(ap, "mmio %pR", mem);
		ata_port_desc(ap, "port 0x%x", 0x100 + ap->port_no * 0x80);

		/* set enclosure management message type */
		if (ap->flags & ATA_FLAG_EM)
			ap->em_message_type = hpriv->em_msg_type;

		/* disabled/not-implemented port */
		if (!(hpriv->port_map & (1 << i)))
			ap->ops = &ata_dummy_port_ops;
	}

	rc = ahci_reset_controller(host);
	if (rc)
		goto pdata_exit;

	ahci_init_controller(host);
	ahci_print_info(host, "platform");

	rc = ata_host_activate(host, irq, ahci_interrupt, IRQF_SHARED,
			       &ahci_platform_sht);
	if (rc)
		goto pdata_exit;

	return 0;
pdata_exit:
	if (pdata && pdata->exit)
		pdata->exit(dev);
disable_unprepare_clk:
	if (!IS_ERR(hpriv->clk))
		clk_disable_unprepare(hpriv->clk);
free_clk:
	if (!IS_ERR(hpriv->clk))
		clk_put(hpriv->clk);
	return rc;
}

static void ahci_host_stop(struct ata_host *host)
{
	struct device *dev = host->dev;
	struct ahci_platform_data *pdata = dev_get_platdata(dev);
	struct ahci_host_priv *hpriv = host->private_data;

	if (pdata && pdata->exit)
		pdata->exit(dev);

	if (!IS_ERR(hpriv->clk)) {
		clk_disable_unprepare(hpriv->clk);
		clk_put(hpriv->clk);
	}
}

#ifdef CONFIG_PM_SLEEP
static int ahci_suspend(struct device *dev)
{
	struct ahci_platform_data *pdata = dev_get_platdata(dev);
	struct ata_host *host = dev_get_drvdata(dev);
	struct ahci_host_priv *hpriv = host->private_data;
	void __iomem *mmio = hpriv->mmio;
	u32 ctl;
	int rc;

	if (hpriv->flags & AHCI_HFLAG_NO_SUSPEND) {
		dev_err(dev, "firmware update required for suspend/resume\n");
		return -EIO;
	}

	/*
	 * AHCI spec rev1.1 section 8.3.3:
	 * Software must disable interrupts prior to requesting a
	 * transition of the HBA to D3 state.
	 */
	ctl = readl(mmio + HOST_CTL);
	ctl &= ~HOST_IRQ_EN;
	writel(ctl, mmio + HOST_CTL);
	readl(mmio + HOST_CTL); /* flush */

	rc = ata_host_suspend(host, PMSG_SUSPEND);
	if (rc)
		return rc;

	if (pdata && pdata->suspend)
		return pdata->suspend(dev);

	if (!IS_ERR(hpriv->clk))
		clk_disable_unprepare(hpriv->clk);

	return 0;
}

static int ahci_resume(struct device *dev)
{
	struct ahci_platform_data *pdata = dev_get_platdata(dev);
	struct ata_host *host = dev_get_drvdata(dev);
	struct ahci_host_priv *hpriv = host->private_data;
	int rc;

	if (!IS_ERR(hpriv->clk)) {
		rc = clk_prepare_enable(hpriv->clk);
		if (rc) {
			dev_err(dev, "clock prepare enable failed");
			return rc;
		}
	}

	if (pdata && pdata->resume) {
		rc = pdata->resume(dev);
		if (rc)
			goto disable_unprepare_clk;
	}

	if (dev->power.power_state.event == PM_EVENT_SUSPEND) {
		rc = ahci_reset_controller(host);
		if (rc)
			goto disable_unprepare_clk;

		ahci_init_controller(host);
	}

	ata_host_resume(host);

	return 0;

disable_unprepare_clk:
	if (!IS_ERR(hpriv->clk))
		clk_disable_unprepare(hpriv->clk);

	return rc;
}
#endif

static SIMPLE_DEV_PM_OPS(ahci_pm_ops, ahci_suspend, ahci_resume);

static const struct of_device_id ahci_of_match[] = {
	{ .compatible = "snps,spear-ahci", },
	{},
};
MODULE_DEVICE_TABLE(of, ahci_of_match);

static struct platform_driver ahci_driver = {
	.probe = ahci_probe,
	.remove = ata_platform_remove_one,
	.driver = {
		.name = "ahci",
#if 0
		.owner = THIS_MODULE,
		.of_match_table = ahci_of_match,
#endif  
		.pm = &ahci_pm_ops,
	},
#if 0    //wei add
	.id_table	= ahci_devtype,
#endif    
};
module_platform_driver(ahci_driver);

MODULE_DESCRIPTION("AHCI SATA platform driver");
MODULE_AUTHOR("Anton Vorontsov <avorontsov@ru.mvista.com>");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:ahci");
