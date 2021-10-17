
/*==========================================

===========================================*/



#include <linux/interrupt.h>
#include <asm/system.h>
#include "monitor.h"
#include <asm/mipsregs.h>	//wei add
#include "test_lib.h"
#include "test_hw_96d.h"
#include <rtl8196x/asicregs.h>

#define REG32(reg) (*(volatile unsigned int *)(reg))
#define REG16(reg) (*(volatile unsigned short *)(reg))
#define REG8(reg) (*(volatile unsigned char *)(reg))


int at2_mode=1;


void PCIE_MDIO_Reset(unsigned int portnum)
{
        #define SYS_PCIE_PHY0   (0xb8000000 +0x50)
        #define SYS_PCIE_PHY1   (0xb8000000 +0x54)	
	 
	unsigned int sys_pcie_phy;

	if(portnum==0)	sys_pcie_phy=SYS_PCIE_PHY0;
	else if(portnum==1)	sys_pcie_phy=SYS_PCIE_PHY1;
	else return;
		
       // 3.MDIO Reset
 	   REG32(sys_pcie_phy) = (1<<3) |(0<<1) | (0<<0);     //mdio reset=0,     	    //0x088
 	   REG32(sys_pcie_phy) = (1<<3) |(0<<1) | (1<<0);     //mdio reset=1,           //0x09
 	   REG32(sys_pcie_phy) = (1<<3) |(1<<1) | (1<<0);     //bit1 load_done=1       //0x0a

}
//------------------------------------------------------------------------
void PCIE_PHY_Reset(unsigned int portnum)
{
	 #define PCIE_PHY0 	0xb8b01008
	 #define PCIE_PHY1 	0xb8b21008
	 
	unsigned int pcie_phy;

#if defined(CONFIG_RTL8196E)
	REG32(0xb8000010)|= (1<<12)|(1<<13)|(1<<19)|(1<<20)|(1<<18)|(1<<16);
#endif

	if(portnum==0)	pcie_phy=PCIE_PHY0;
	else if(portnum==1)	pcie_phy=PCIE_PHY1;
	else return;

        //4. PCIE PHY Reset       
	REG32(pcie_phy) = 0x01;	//bit7:PHY reset=0   bit0: Enable LTSSM=1
	REG32(pcie_phy) = 0x81;   //bit7: PHY reset=1   bit0: Enable LTSSM=1
	
}
//------------------------------------------------------------------------
int PCIE_Check_Link(unsigned int portnum)
{
	unsigned int dbgaddr;
	unsigned int cfgaddr;
	
	if(portnum==0)	dbgaddr=0xb8b00728;
	else if(portnum==1)	dbgaddr=0xb8b20728;
	else if(portnum==2)	dbgaddr=0xb8b40728;	
	else return;	

  //wait for LinkUP
      delay_ms(10);
	volatile int i=5;
	while(--i)
	{
		 delay_ms(10);	  
	      if( (REG32(dbgaddr)&0x1f)==0x11)
		  	break;

	}
	if(i==0)
	{	if(at2_mode==0)  //not auto test, show message
		dprintf("PCIE_P%d ->  Cannot LinkUP\n",portnum);
		return 0;
	}
	else  //already  linkup
	{
		if(portnum==0)	      REG32(PCIE0_RC_CFG_BASE + 0x04)= 0x00100007;
		else if(portnum==1) REG32(PCIE1_RC_CFG_BASE + 0x04)= 0x00100007;

		  
		if(portnum==0) cfgaddr=0xb8b10000;
		else if(portnum==1) cfgaddr=0xb8b30000;

		if(at2_mode==0)
		dprintf("Find Port=%x Device:Vender ID=%x\n", portnum, REG32(cfgaddr) );
	}
	return 1;
}


void HostPCIe_SetPhyMdioWrite(unsigned int portnum, unsigned int regaddr, unsigned short val)
{
	unsigned int mdioaddr;

	if(portnum==0)		mdioaddr=PCIE0_MDIO;	
	else if(portnum==1)	mdioaddr=PCIE1_MDIO;
	else if(portnum==2)	mdioaddr=0xb8b410c0;	  //EP MDIO	
	else return 0;
	
	REG32(mdioaddr)= ( (regaddr&0x1f)<<PCIE_MDIO_REG_OFFSET) | ((val&0xffff)<<PCIE_MDIO_DATA_OFFSET)  | (1<<PCIE_MDIO_RDWR_OFFSET) ; 
	//delay 
	volatile int i;
	for(i=0;i<5555;i++)  ;
}


void PCIE_reset_procedure(int portnum, int Use_External_PCIE_CLK, int mdio_reset)
{
 //	dprintf("port=%x, mdio_rst=%x \n", portnum, mdio_reset);

 #if 1
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	unsigned int v=REG32(SYS_HW_STRAP);
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	unsigned char sel40m=GET_BITVAL(v, 25, RANG1);
#endif


 	//first, Turn On PCIE IP
	 #define CLK_MANAGE 	0xb8000010
	REG32(CLK_MANAGE) |= (1<<12) | (1<<13) | (1<<18);
	 
	if(portnum==0)		    REG32(CLK_MANAGE) |=  (1<<14);        //enable active_pcie0
	else if(portnum==1)	    REG32(CLK_MANAGE) |=  (1<<16);        //enable active_pcie1	
	else return;
		    

			
      // __delay(1000*1000*1);
       delay_ms(10);

        //add compatible, slove sata pcie card.
	if(portnum==0)	  REG32(0xb8b0100c)=(1<<3);  //set target Device Num=1;
	if(portnum==1)	  REG32(0xb8b2100c)=(2<<3);  //set target Device Num=1;

 	//REG32(SYS_PCIE_ANA)=0x3f39;   //PCIE_ANA  bokai tell me to do this.

	if(mdio_reset)
	{
		if(at2_mode==0)  //no auto test, show message
			dprintf("Do MDIO_RESET\n");
		
		 delay_ms(10);
       	// 3.MDIO Reset
		PCIE_MDIO_Reset(portnum);
	}  
	
	  delay_ms(10);
 	PCIE_PHY_Reset(portnum);	
	
       // __delay(1000*1000);
        delay_ms(10);
       
 
	  //----------------------------------------
	  if(mdio_reset)
	  	{

//------------------------------------------------------

		HostPCIe_SetPhyMdioWrite(portnum, 0, 0xD087);  //bokai tell, and fix

		HostPCIe_SetPhyMdioWrite(portnum, 1, 0x0003);
		HostPCIe_SetPhyMdioWrite(portnum, 2, 0x4d18);
//#if  PHY_EAT_40MHZ
		if(sel40m==1)
		HostPCIe_SetPhyMdioWrite(portnum, 5, 0x0BCB);   //40M
//#endif

//#if  PHY_EAT_40MHZ
		if(sel40m==1) {
#if defined(CONFIG_RTL8196E)
			HostPCIe_SetPhyMdioWrite(portnum, 6, 0x0148);  //40M
#else
			HostPCIe_SetPhyMdioWrite(portnum, 6, 0xF148);  //40M
#endif
		}
//#else
		else
		HostPCIe_SetPhyMdioWrite(portnum, 6, 0xf848);  //25M
//#endif

		HostPCIe_SetPhyMdioWrite(portnum, 7, 0x31ff);
//		HostPCIe_SetPhyMdioWrite(portnum, 8, 0x18d5);  //peisi tune
		HostPCIe_SetPhyMdioWrite(portnum, 8, 0x18d6);  //slove high temp	


		HostPCIe_SetPhyMdioWrite(portnum, 0x09, 0x539c); 	
		HostPCIe_SetPhyMdioWrite(portnum, 0x0a, 0x20eb); 	
//		HostPCIe_SetPhyMdioWrite(portnum, 0x0d, 0x1764); 	     //tx amp=111, over eye mask
		HostPCIe_SetPhyMdioWrite(portnum, 0x0d, 0x1464);     //tx amp=100


		HostPCIe_SetPhyMdioWrite(portnum, 0x0b, 0x0711);   //for sloving low performance


#if defined(CONFIG_RTL8196E)
		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0f0f);
#else
		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0a00);
#endif
		HostPCIe_SetPhyMdioWrite(portnum, 0x19, 0xFCE0); 

		HostPCIe_SetPhyMdioWrite(portnum, 0x1a, 0x7e4f);   //formal chip, reg 0x1a.4=0
		HostPCIe_SetPhyMdioWrite(portnum, 0x1b, 0xFC01);   //formal chip	 reg 0x1b.0=1
		
		HostPCIe_SetPhyMdioWrite(portnum, 0x1e, 0xC280);	


		if(Use_External_PCIE_CLK==1)
		{
		//external clock
		//reg6=fc48
		//reg 1a=bit 4=1   7e40
		//reg 1b=bit 0=0
		HostPCIe_SetPhyMdioWrite(portnum, 0x06, 0xfc48);	
		HostPCIe_SetPhyMdioWrite(portnum, 0x1a, 0x7e50);	
		HostPCIe_SetPhyMdioWrite(portnum, 0x1b, 0xfc00);			

		}

	  	}
 
	//---------------------------------------
         // 6. PCIE Device Reset
       delay_ms(10);
	PCIE_PHY_Reset(portnum);
       REG32(CLK_MANAGE) &= ~(1<<26);    //perst=0 off.
     	delay_ms(300);
       REG32(CLK_MANAGE) |=  (1<<26);   //PERST=1 
	//---------------------------------------	  
	PCIE_Check_Link(portnum);

}


//--------------------------------------------------------------------------
int PCIE_Host_Init(int argc, char* argv[])
{
	int portnum=0;
	if(argc >= 1) 
	{	portnum= strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	}



#define MAX_READ_REQSIZE_128B    0x00
#define MAX_READ_REQSIZE_256B    0x10
#define MAX_READ_REQSIZE_512B    0x20
#define MAX_READ_REQSIZE_1KB     0x30
#define MAX_READ_REQSIZE_2KB     0x40
#define MAX_READ_REQSIZE_4KB     0x50


#define MAX_PAYLOAD_SIZE_128B    0x00
#define MAX_PAYLOAD_SIZE_256B    0x20
#define MAX_PAYLOAD_SIZE_512B    0x40
#define MAX_PAYLOAD_SIZE_1KB     0x60
#define MAX_PAYLOAD_SIZE_2KB     0x80
#define MAX_PAYLOAD_SIZE_4KB     0xA0



	int rc_cfg, cfgaddr;
	int iomapaddr;
	int memmapaddr;

	if(portnum==0)
	{	rc_cfg=PCIE0_RC_CFG_BASE;
		cfgaddr=PCIE0_EP_CFG_BASE;
		iomapaddr=PCIE0_MAP_IO_BASE;
		memmapaddr=PCIE0_MAP_MEM_BASE;
	}
	else if(portnum==1)
	{	rc_cfg=PCIE1_RC_CFG_BASE;
		cfgaddr=PCIE1_EP_CFG_BASE;
		iomapaddr=PCIE1_MAP_IO_BASE;
		memmapaddr=PCIE1_MAP_MEM_BASE;	
	}
	else 
	{	return 0;
	}
	
	int t=REG32(rc_cfg);
	unsigned int vid=t&0x0000ffff;   //0x819810EC
	unsigned int pid=(t&0xffff0000) >>16;
	
	if( (vid!= 0x10ec) || (pid!=0x8196))
	{	DBG_PRINT("VID=%x, PID=%x \n", vid, pid );
		DBG_PRINT(" !!!  Read VID PID fail !!! \n");
		//at_errcnt++;
		return;
	}

	//STATUS
	//bit 4: capabilties List

	//CMD
	//bit 2: Enable Bys master, 
	//bit 1: enable memmap, 
	//bit 0: enable iomap

	REG32(rc_cfg + 0x04)= 0x00100007;   

	//Device Control Register 
	//bit [7-5]  payload size
	REG32(rc_cfg + 0x78)= (REG32(rc_cfg + 0x78 ) & (~0xE0)) | MAX_PAYLOAD_SIZE_128B;  // Set MAX_PAYLOAD_SIZE to 128B,default
	  
      REG32(cfgaddr + 0x04)= 0x00100007;    //0x00180007

	//bit 0: 0:memory, 1 io indicate
      REG32(cfgaddr + 0x10)= (iomapaddr | 0x00000001) & 0x1FFFFFFF;  // Set BAR0

	//bit 3: prefetch
	//bit [2:1] 00:32bit, 01:reserved, 10:64bit 11:reserved
      REG32(cfgaddr + 0x18)= (memmapaddr | 0x00000004) & 0x1FFFFFFF;  // Set BAR1  


	//offset 0x78 [7:5]
      REG32(cfgaddr + 0x78) = (REG32(cfgaddr + 0x78) & (~0xE0)) | (MAX_PAYLOAD_SIZE_128B);  // Set MAX_PAYLOAD_SIZE to 128B

	//offset 0x79: [6:4] 
      REG32(cfgaddr + 0x78) = (REG32(cfgaddr + 0x78) & (~0x7000)) | (MAX_READ_REQSIZE_256B<<8);  // Set MAX_REQ_SIZE to 256B,default

	  
	//check
//      if(REG32(cfgaddr + 0x10) != ((iomapaddr | 0x00000001) & 0x1FFFFFFF))
      {	//at_errcnt++;
      		//DBG_PRINT("Read Bar0=%x \n", REG32(cfgaddr + 0x10)); //for test
      	}
	  

//	if(REG32(cfgaddr + 0x18)!=((memmapaddr| 0x00000004) & 0x1FFFFFFF))
	{	//at_errcnt++;
      		//DBG_PRINT("Read Bar1=%x \n", REG32(cfgaddr + 0x18));      //for test
	}
	//DBG_PRINT("Set BAR finish \n");


	//io and mem limit, setting to no litmit
	REG32(rc_cfg+ 0x1c) = (2<<4) | (0<<12);   //  [7:4]=base  [15:12]=limit
	REG32(rc_cfg+ 0x20) = (2<<4) | (0<<20);   //  [15:4]=base  [31:20]=limit	
	REG32(rc_cfg+ 0x24) = (2<<4) | (0<<20);   //  [15:4]=base  [31:20]=limit		

}

#ifdef CONFIG_RTL8196E
unsigned int gpio_pin_ctrl_reg;

int rtl8196e_get_gpio_sw_in(void)
{
	if ((REG32(BOND_OPTION) & BOND_ID_MASK) == BOND_8196ES) {
		return (!((*(volatile u32 *)(gpio_pin_ctrl_reg + 0x44)) & (1<<0)));
	}
	else {
#if defined(CONFIG_RTL8196E_GPIOB5_RESET)
		return (!((REG32(PABCDDAT_REG) & (1<<13))>>13) ); //return 0 if non-press
#else
		return (!((REG32(PABCDDAT_REG) & (1<<5))>>5) ); //return 0 if non-press
#endif
	}
}

void rtl8196e_gpio_init(void)
{
	char *arg_v[] = {"hinit", "0"};

	if ((REG32(BOND_OPTION) & BOND_ID_MASK) == BOND_8196ES) {
		// reset button
		PCIE_reset_procedure(0, 0, 1);
		PCIE_Host_Init(2, arg_v);
		gpio_pin_ctrl_reg = (REG32(PCIE0_EP_CFG_BASE + 0x18) & 0xffff0000) | 0xb0000000;
		(*(volatile u32 *)(gpio_pin_ctrl_reg + 0x44)) = 0x0;
	}
}
#endif
#ifdef CONFIG_RTL8196E_ULINKER_BOOT_LED
void power_on_led(void)
{
	char *arg_v[] = {"hinit", "0"};
	unsigned int reg;

	// wlan led
	PCIE_reset_procedure(0, 0, 1);
	PCIE_Host_Init(2, arg_v);
	reg = (REG32(PCIE0_EP_CFG_BASE + 0x18) & 0xffff0000) | 0xb0000000;
	(*(volatile u32 *)(reg + 0x44)) = 0x30300000;
	// ethernet led
	(*(volatile u32 *)0xb8000044) = (*(volatile u32 *)0xb8000044) | 0x00003000;
	(*(volatile u32 *)0xb8003500) = (*(volatile u32 *)0xb8003500) & ~(0x00004000);
	(*(volatile u32 *)0xb8003508) = (*(volatile u32 *)0xb8003508) | 0x00004000;
	(*(volatile u32 *)0xb800350c) = (*(volatile u32 *)0xb800350c) & ~(0x00004000);
}
#endif
