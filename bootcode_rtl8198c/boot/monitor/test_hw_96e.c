
/*==========================================

===========================================*/



#include <linux/interrupt.h>
#include <asm/system.h>
#include "monitor.h"
#include <asm/mipsregs.h>	//wei add

#include "test_hw_96e.h"


#define REG32(reg) (*(volatile unsigned int *)(reg))
#define REG16(reg) (*(volatile unsigned short *)(reg))
#define REG8(reg) (*(volatile unsigned char *)(reg))






#define printf dprintf

#define DBG_PRINT printf


int CmdTestSRAM(int argc, char* argv[]);  //wei add


int at2_errcnt=0;
int at2_mode=0;
int PCIE_Host_RESET(int argc, char* argv[]); 
int PCIE_Host_Init(int argc, char* argv[]); 
int Test_HostPCIE_DataLoopback(int argc, char* argv[]); 
int  PCIE_PowerDown(int argc, char* argv[]); 
int HostPCIe_MDIORead(int argc, char* argv[]); 
int HostPCIe_MDIOWrite(int argc, char* argv[]); 
int PCIE_PHYLoop(int argc, char* argv[]); 


int HostPCIe_TestINT(int argc, char* argv[]); 




COMMAND_TABLE	TestCmdTable[] =
{
	{ "?"	  ,0, NULL			, "HELP (?)				    : Print this help message"					},
	{ "Q"   ,0, NULL			, "Q: return to previous menu"					}, 	//wei add			


	{ "SRAM"   ,1, CmdTestSRAM			, "SRAM <map addr> : test sram"	}, 	//wei add	


	
	{ "NULL"   ,0, NULL			, "-------Host PCI-E test -----------"},	
	{ "HRST"   ,1, PCIE_Host_RESET			, "HRST: Host Pcie Reset <portnum> <mdio_rst>: "},
	{ "HINIT"   ,1, PCIE_Host_Init			, "HINIT: Host init bar <portnum>"},	
	{ "HLOOP"   ,1, Test_HostPCIE_DataLoopback			, "HLOOP: Test Pci-E data loopback <portnum> <cnt> "},	
	{ "EPDN"   ,1, PCIE_PowerDown			, "EPDN: PCIE Power Down test <portnum><mode> "},			
	{ "EMDIOR"   ,1, HostPCIe_MDIORead			, "EMDIOR: Reg Read <portnum>"},	
	{ "EMDIOW"   ,1, HostPCIe_MDIOWrite			, "EMDIOW <portnum> <reg> <val>:  "},	
	{ "ELOOP"   ,1, PCIE_PHYLoop			, "ELOOP <portnum> <start/stop>:  "},	

	{ "EINT"   ,1, HostPCIe_TestINT			, "EINT <portnum> <loops>:  "},	


	
};

//------------------------------------------------------------------------
void Enable_SRAM(unsigned int paddr)
{
	//unmap
	REG32(0xb8001304)=0x5;   //unmap seg len=4K
	REG32(0xb8001300)=(unsigned int)paddr |1;   //unmap seg addr

	//enable sram
	REG32(0xb8004004)=0x05;   //sram seg len=4K		
	REG32(0xb8004000)=(unsigned int)paddr |1;   //sram seg addr
}

void Disable_SRAM()
{
	//unmap
	REG32(0xb8001300)=0;   //unmap seg addr
	//enable sram
	REG32(0xb8004000)=0;   //sram seg addr
}


int CmdTestSRAM(int argc, char* argv[])
{
	#define VIR2PHY(x) ((unsigned int)x&0x1fffffff)
	#define VIR2NOC(x) ((unsigned int)x|0xa0000000)
	#define REG32(reg)	(*(volatile unsigned int *)(reg))

	unsigned char *addr;
	unsigned int len=(1<<12);  //SRAM size=4K
	unsigned int i;
	
	if( argc < 1 ) 
	{
		dprintf("Usage: sram <map addr> \n");		
		return;	
	}
	addr = strtoul((const char*)(argv[0]), (char **)NULL, 16);	

	unsigned char *paddr=VIR2PHY(addr);
	unsigned char  *n8addr=VIR2NOC(addr);
	unsigned short *n16addr=VIR2NOC(addr);		
	unsigned int     *n32addr=VIR2NOC(addr);	
	
	//===============================================
	//stage 1: r/w
	memset(n8addr, 0xff, len);
#if 1
	Enable_SRAM(paddr);
#endif
	//-----------------------------------------------------
	//test fix pattern
	unsigned int patt_array[]={ 0x00000000, 0x55555555, 0xaaaaaaaa, 0xffffffff, 0x1234567, 0xa5a55a5a  };
	unsigned int patt_idx=0; 

	#define ACC8 0
	#define ACC16 1
	#define ACC32 2
	#define ACCEND 3
	unsigned int accmode;
	
	for(accmode=ACC8; accmode<ACCEND; accmode++)  // 8 bit, 16bit, 32bit
	{
	
	for(patt_idx=0; patt_idx< sizeof(patt_array)/sizeof(patt_array[0]) ; patt_idx++)
	{
		if(accmode==ACC8)  //pattern fix value, 8 bit access
		{			
			unsigned char patt=patt_array[patt_idx];
			printf("Test Patt %02x, byte w/r \n", patt);	
			memset(n8addr, patt, len);	
			for(i=0; i<len; i++)
			{
				if(REG8(n8addr+i) != patt)
					printf("%x err, exp=%x, read=%x\n", n8addr+i, patt, REG8(n8addr+i));
			}
		}
		else if(accmode==ACC16)			//pattern fix value, 16 bit access
		{
			unsigned short patt=patt_array[patt_idx];
			printf("Test Patt %04x, short w/r \n", patt);	

			for(i=0; i<len; i+=2)			
				REG16(n8addr+i)= patt;	
			
			for(i=0; i<len; i+=2)
			{
				if(REG16(n8addr+i) != patt)
					printf("%x err, exp=%x, read=%x\n", n8addr+i, patt, REG16(n8addr+i));
			}
		}
		else if(accmode==ACC32)   //pattern fix value, 32 bit access
		{			
			unsigned int patt=patt_array[patt_idx];
			printf("Test Patt %08x, int w/r \n", patt);	

			for(i=0; i<len; i+=4)			
				REG32(n8addr+i)= patt;	

			for(i=0; i<len; i+=4)
			{
				if(REG32(n8addr+i) != patt)
					printf("%x err, exp=%x, read=%x\n", n8addr+i, patt, REG32(n8addr+i));
			}
		}		
	}
	}
	//-----------------------------------------------------	
	//test inc patt
	for(accmode=ACC8; accmode<ACCEND; accmode++)  // 8 bit, 16bit, 32bit
	{
		if(accmode==ACC8)  //pattern fix value, 8 bit access
		{			
			printf("Test inc Patt, byte w/r \n");	
			for(i=0; i<len; i++)		
				REG8(n8addr+i)=i;
			
			for(i=0; i<len; i++)
			{
				if(REG8(n8addr+i) != (i&0xff) )
					printf("%x err, exp=%x, read=%x\n", n8addr+i, (i&0xff), REG8(n8addr+i));
			}
		}
		else if(accmode==ACC16)			//pattern fix value, 16 bit access
		{
			printf("Test inc Patt, short w/r \n");	

			for(i=0; i<len/2; i++)			
				REG16(n16addr+i)= i;	
			
			for(i=0; i<len/2; i++)
			{
				if(REG16(n16addr+i) != (i&0xffff) )
					printf("%x err, exp=%x, read=%x\n", n16addr+i, (i&0xffff), REG16(n16addr+i));
			}
		}
		else if(accmode==ACC32)   //pattern fix value, 32 bit access
		{			
			printf("Test inc Patt, int w/r \n");	

			for(i=0; i<len/4; i++)			
				REG32(n32addr+i)= i;	

			for(i=0; i<len/4; i++)
			{
				if(REG32(n32addr+i) != i)
					printf("%x err, exp=%x, read=%x\n", n32addr+i, i, REG32(n32addr+i));
			}
		}		
	}
	//-----------------------------------------------------	
#if 0	
	//pattern rand, byte access
	printf("Test Pattern rand, byte w/r \n");
	
	srand(0x55aa);
	for(i=0; i<len; i++)		
	{	unsigned int data;
		data=random(0xff);
		n8addr[i]=data;
		if(n8addr[i] !=data)
			printf("%x err, exp=%x, read=%x\n", n8addr+i, data, n8addr[i]);
	}
	//-----------------------------------------------------
	
	//pattern rand, 4 byte access
	printf("Test Pattern rand, 4 bytes w/r \n");
	
	srand(0xaa55);
	for(i=0; i<len; i+=4)		
	{	unsigned int data;
		data=rand2();
		n32addr[i]=data;
		if(n32addr[i] !=data)
			printf("%x err, exp=%x, read=%x\n", n8addr+i, data, n8addr[i]);
	}
#endif
	//-----------------------------------------------------
	memset(n8addr,0xaa,len);
	Disable_SRAM();


	//===========================================
	//stage 2: test on/off
	printf("Test Pattern 0x55 in DRAM, 0xaa in SRAM \n");
	
	memset(n8addr,0x55,len);
	
#if 1
	printf("Enable SRAM, do compare\n");
	Enable_SRAM(paddr);
#endif

	
	//compare the original sdram
	for(i=0; i<len; i++)
	{
		if(n8addr[i] != 0xaa)
			printf("%x err, exp=%x, read=%x\n", n8addr+i, 0xaa, n8addr[i]);
	}
	
	printf("Disable SRAM, do compare\n");
	Disable_SRAM();
	
	for(i=0; i<len; i++)
	{
		if(n8addr[i] != 0x55)
			printf("%x err, exp=%x, read=%x\n", n8addr+i, 0x55, n8addr[i]);
	}

	//-----------------------------------------------------

	
	printf("No error is pass\n");
}

//------------------------------------------------------------------------

//===========================================================

//------------------------------------------------------------------------
void PCIE_Device_PERST(void)
{
	 #define CLK_MANAGE 	0xb8000010
        // 6. PCIE Device Reset       
     REG32(CLK_MANAGE) &= ~(1<<26);    //perst=0 off.    
        __delay(100000);   //PCIE standadrd: poweron: 100us, after poweron: 100ms
        __delay(100000);   
        __delay(100000);   		
    REG32(CLK_MANAGE) |=  (1<<26);   //PERST=1

}
//------------------------------------------------------------------------

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
 	   REG32(sys_pcie_phy) = (1<<3) |(1<<1) | (1<<0);     //bit1 load_done=1       //0x0b

}
//------------------------------------------------------------------------
void PCIE_PHY_Reset(unsigned int portnum)
{
	 #define PCIE_PHY0 	0xb8b01008
	 #define PCIE_PHY1 	0xb8b21008
	 
	unsigned int pcie_phy;

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
	{	//if(at2_mode==0)  //not auto test, show message
		prom_printf("PCIE_P%d ->  Cannot LinkUP\n",portnum);
		return 0;
	}
	else  //already  linkup
	{
		if(portnum==0)	      REG32(PCIE0_RC_CFG_BASE + 0x04)= 0x00100007;
		else if(portnum==1) REG32(PCIE1_RC_CFG_BASE + 0x04)= 0x00100007;

		  
		if(portnum==0) cfgaddr=0xb8b10000;
		else if(portnum==1) cfgaddr=0xb8b30000;

		//if(at2_mode==0)
		dprintf("Find Port=%x Device:Vender ID=%x\n", portnum, REG32(cfgaddr) );
	}
	return 1;
}

//------------------------------------------------------------------------
unsigned int HostPCIe_SetPhyMdioRead(unsigned int portnum, unsigned int regaddr)
{
	unsigned int mdioaddr;

	if(portnum==0)		mdioaddr=PCIE0_MDIO;	
	else if(portnum==1)	mdioaddr=PCIE1_MDIO;
	else if(portnum==2)	mdioaddr=0xb8b410c0;	  //EP MDIO
	else return 0;
	
	REG32(mdioaddr)= ((regaddr&0x1f)<<PCIE_MDIO_REG_OFFSET)  | (0<<PCIE_MDIO_RDWR_OFFSET); 
	//delay 
	volatile int i;
	for(i=0;i<5555;i++)  ;

	int val;
	val=REG32(mdioaddr)&  (0xffff <<PCIE_MDIO_DATA_OFFSET) ;
	return ((val>>PCIE_MDIO_DATA_OFFSET)&0xffff);
	
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

//------------------------------------------------------------------------
//#ifdef CONFIG_EPHY_40MHZ
//#define PHY_EAT_40MHZ 1
//#else  //25MHz
//#define PHY_EAT_40MHZ 0
//#endif

void PCIE_reset_procedure(int portnum, int Use_External_PCIE_CLK, int mdio_reset)
{
 	//dprintf("port=%x, mdio_rst=%x \n", portnum, mdio_reset);

	
 #if 1
	#define SYS_HW_STRAP   (0xb8000000 +0x08)
	unsigned int v=REG32(SYS_HW_STRAP);
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	//unsigned char sel40m=GET_BITVAL(v, 25, RANG1);
	unsigned char sel40m=GET_BITVAL(v, 24, RANG1);
#endif

	//for RTL8198c pin_mux2 (0xb8000104) [21:20]=01
	REG32(0xb8000104)&=~(3<<20);
	REG32(0xb8000104)|=(1<<20);
	



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
#if 0
		HostPCIe_SetPhyMdioWrite(portnum, 0, 0xD087);  //bokai tell, and fix

		HostPCIe_SetPhyMdioWrite(portnum, 1, 0x0003);
		HostPCIe_SetPhyMdioWrite(portnum, 2, 0x4d18);
//#if  PHY_EAT_40MHZ
		if(sel40m==1)
		HostPCIe_SetPhyMdioWrite(portnum, 5, 0x0BCB);   //40M
//#endif

//#if  PHY_EAT_40MHZ
		if(sel40m==1)
		HostPCIe_SetPhyMdioWrite(portnum, 6, 0xF148);  //40M
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


		//for sloving low performance
		//JSW:Without TX 0.1u Cap,for RTL8111C
		#if 0
		HostPCIe_SetPhyMdioWrite(portnum, 0x0b, 0x0511);
		#else
		//With TX 0.1u Cap ,for RTL8111DL
		HostPCIe_SetPhyMdioWrite(portnum, 0x0b, 0x0711);   
		#endif


		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0a00);				
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
#endif
#if 1  //v6 FPGA 98C PCIE PHY 40MHZ
		{
		int phy40M=0;
		
		 phy40M=(REG32(0xb8000008)&(1<<24))>>24;
		if(phy40M)
		{
		dprintf("PCIE PHY=40MHZ,portnum=%d\n",portnum);
		HostPCIe_SetPhyMdioWrite(portnum, 0x3, 0x7b01);
		HostPCIe_SetPhyMdioWrite(portnum, 0x6, 0x0268);
		//f26c
		
		HostPCIe_SetPhyMdioWrite(portnum, 0x19, 0xFC70); 
		// HostPCIe_SetPhyMdioWrite(portnum, 0x13, 0x026c);
		}
		else
		{
			dprintf("PCIE PHY=25MHZ,portnum=%d\n",portnum);
			HostPCIe_SetPhyMdioWrite(portnum, 0x3, 0x3001);
        		HostPCIe_SetPhyMdioWrite(portnum, 0x6, 0xe0b8); //Hannah

        		HostPCIe_SetPhyMdioWrite(portnum, 0xF, 0x400F);
        		HostPCIe_SetPhyMdioWrite(portnum, 0x19, 0xFC70);	
		}
		}
#endif
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


//------------------------------------------------------------------------
int Test_HostPCIE_DataLoopback(int argc, char* argv[])
{
//	extern void example();

//    PCIE_reset_procedure(0,0,1);
	unsigned int portnum=0;
	unsigned int test_packet_num=1;
	
	if(argc >= 1) 
	{	portnum= strtoul((const char*)(argv[0]), (char **)NULL, 16);
	}

	if(argc >= 2) 
	{	test_packet_num= strtoul((const char*)(argv[1]), (char **)NULL, 16);
	}

    unsigned int cnt;

    unsigned int PCIE_Test_cnt;
	unsigned int pcie_err=0;
	
	if(test_packet_num==0)
	{
		while(1)
       	 if(example(portnum, at2_mode)==0)   //0: mean fail
       	 {
				//printf("FAIL, and hang!\n");
				printf("FAIL!\n");
				//while(1)  {};
		 }
	}
	
    for (PCIE_Test_cnt=1; PCIE_Test_cnt<=test_packet_num; PCIE_Test_cnt++)
    {
#if DBG
        prom_printf("\n==================(Start)======================\n");
        prom_printf("\nPCIE_Test_cnt:%d\n",PCIE_Test_cnt);
#endif        

        if(example(portnum, at2_mode)==0)   //0: mean fail

	{		at2_errcnt++;
		pcie_err++;
        }


#if 0  //for bokai
	if 	(rtl_inb(UART_LSR) & 0x1)
	{	
		int ch=rtl_inb(UART_RBR)&0xff;
		if(ch=='q')
			return;
	}
#endif
		
    }
	printf("====> Total test cnt=%d, test fail=%d\n", test_packet_num,  pcie_err);
	
}; 
//------------------------------------------------------------------------
int PCIE_Host_RESET(int argc, char* argv[])
{
	int  portnum= 0;  //0: one port, 1: two port
	int Use_External_PCIE_CLK=0;
	int mdio_reset=0;
	if(argc<2)
	{//dump all	
	       dprintf("\n"); 		   
		dprintf("hrst <portnum> <mdio_reset>\n");
		return;
	}
	
	if(argc >= 1) 
	{	portnum= strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	}
	if(argc >= 2) 
	{	mdio_reset= strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	}

	if(portnum==2)
	{
		PCIE_reset_procedure(0, Use_External_PCIE_CLK, mdio_reset);
		PCIE_reset_procedure(1, Use_External_PCIE_CLK, mdio_reset);
		//PCIE_reset_procedure(portnum, 1, mdio_reset);		
	}		
	else
		PCIE_reset_procedure(portnum, Use_External_PCIE_CLK, mdio_reset);

}; 


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
	
	if( (vid!= 0x10ec) || (pid!=0x8198))
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
      		DBG_PRINT("Read Bar0=%x \n", REG32(cfgaddr + 0x10)); //for test
      	}
	  

//	if(REG32(cfgaddr + 0x18)!=((memmapaddr| 0x00000004) & 0x1FFFFFFF))
	{	//at_errcnt++;
      		DBG_PRINT("Read Bar1=%x \n", REG32(cfgaddr + 0x18));      //for test
	}
	DBG_PRINT("Set BAR finish \n");


	//io and mem limit, setting to no litmit
	REG32(rc_cfg+ 0x1c) = (2<<4) | (0<<12);   //  [7:4]=base  [15:12]=limit
	REG32(rc_cfg+ 0x20) = (2<<4) | (0<<20);   //  [15:4]=base  [31:20]=limit	
	REG32(rc_cfg+ 0x24) = (2<<4) | (0<<20);   //  [15:4]=base  [31:20]=limit		

}


//----------------------------------------------------------------------------
int HostPCIe_MDIORead(int argc, char* argv[])
{

	unsigned int i,val,j;
/*
	if(argc<1)
	{//dump all	
	       dprintf("\n"); 		   
		dprintf("emdior <portnum> \n");
		return;
	}
*/	
	unsigned int portnum=0;
	
	if(argc>=1)
		portnum	= strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	
	for(i=0; i<=0x1f; i++)
	{/*
		REG32(PCIE0_MDIO)= (i<<PCIE_MDIO_REG_OFFSET) | (0<<PCIE_MDIO_RDWR_OFFSET) ;   
		for(j=0;j<0x5555;j++) ;
		val=REG32(PCIE0_MDIO);
		val=( val& PCIE_MDIO_DATA_MASK ) >> PCIE_MDIO_DATA_OFFSET;		
	*/
		val=HostPCIe_SetPhyMdioRead(portnum, i);
		dprintf("MDIO Reg %x=%x \n", i,val);

	}



}; 
//----------------------------------------------------------------------------
int HostPCIe_MDIOWrite(int argc, char* argv[])
{

	if(argc<3)
	{	 
		dprintf("mdiow <portnum> <addr> <val> \n");		
		dprintf("ex: mdiow 0 00  ffff \n");			
		return;	
	}

	unsigned int portnum = strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	unsigned int addr = strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	unsigned int val = strtoul((const char*)(argv[2]), (char **)NULL, 16);	
/*
	REG32(PCIE0_MDIO)= (addr<<PCIE_MDIO_REG_OFFSET) | (val<<PCIE_MDIO_DATA_OFFSET)  | (1<<PCIE_MDIO_RDWR_OFFSET) ;   ;   
*/
	HostPCIe_SetPhyMdioWrite(portnum, addr, val);

}; 
//---------------------------------------------------------------------------

int  PCIE_PowerDown(int argc, char* argv[])
{
	#define PCIE0_RC_CFG_BASE (0xb8b00000)
	#define PCIE0_EP_CFG_BASE (0xb8b10000)
	#define PCIE1_RC_CFG_BASE (0xb8b20000)
	#define PCIE1_EP_CFG_BASE (0xb8b30000)
	#define DEVICE_OFFSET (0x10000)

	 #define PCIE_PHY0 	0xb8b01008
	 
	 int portnum=0;
	 int mode=0;
	int baseaddr=0xb8b00000;
	 
	if( argc < 2 ) 
	{
		dprintf("epdn <portnum> <mode>.\n");	
		dprintf("epdn 0: D0 ->L0 \n");			
		dprintf("epdn 3: D3hot ->L1 \n");
		dprintf("epdn 4: board cast PME_TurnOff \n");	
		
		dprintf("epdn 7: enable aspm and L0s entry \n");	
		dprintf("epdn 8: enable aspm and L1 entry \n");	
		dprintf("epdn 9: diable  aspm \n");	

		dprintf("epdn 010: measure L0->L1->L0 \n");		
		dprintf("epdn 020: measure L0->L2->L0 \n");			
		dprintf("P0 Link status=%x \n", REG32(0xb8b00728)&0x1f );		
		dprintf("P1 Link status=%x \n", REG32(0xb8b20728)&0x1f );		
		return 0;
	}
	if(argc>=1)	portnum = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	if(argc>=2)   mode = strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	int tmp;

	if(portnum==0) baseaddr=PCIE0_RC_CFG_BASE;
	else if(portnum==1) baseaddr=PCIE1_RC_CFG_BASE;
	else { dprintf("Only support P0 and P1 \n"); return 0; }


	if(mode==0)
	{

		#if 0 //saving more power, leave L1 write
		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0f0f);
		#endif	
		
		tmp = REG32(baseaddr+DEVICE_OFFSET+0x44) &( ~(3));  //D0
		REG32(baseaddr+DEVICE_OFFSET+0x44) = tmp|  (0);  //D0	
		dprintf("D0 \n");
	
	}

	
	if(mode==3)
	{

		#if 1 //saving more power
		REG32(baseaddr+DEVICE_OFFSET+0x80)|= (0x100);  //enable clock PM
		#endif
		
		tmp = REG32(baseaddr+DEVICE_OFFSET+0x44) &( ~(3));  //D0
		REG32(baseaddr+DEVICE_OFFSET+0x44) = tmp|  (3);  //D3	
		//HostPCIe_SetPhyMdioWrite(0xd, 0x15a6);
		dprintf("D3 hot \n");		

		#if 0 //saving more power		
		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0708);
		#endif
	}

	if(mode==4)
	{	
		#if 0 //saving more power   leave L1 write
		HostPCIe_SetPhyMdioWrite(portnum, 0xf, 0x0f0f);
		#endif	
	
		REG32(baseaddr+0x1008) |= (0x200);  		
		dprintf("Host boardcase PME_TurnOff \n");		
	}
	if(mode==7)
	{
		REG32(baseaddr+DEVICE_OFFSET+0x070c) &= ~  ((0x7 <<27)|(0x7<<24));
		REG32(baseaddr+DEVICE_OFFSET+0x070c) |=  ((3)<<27) | ((1)<<24);   //L1=3us, L0s=1us

		REG32(baseaddr+0x80) &= ~(0x3);
		REG32(baseaddr+DEVICE_OFFSET+0x80) &= ~(0x3);		

		REG32(baseaddr+0x80) |= 1;   //L0s
		REG32(baseaddr+DEVICE_OFFSET+0x80) |= 1;				

	}



	if(mode==8)
	{
		REG32(baseaddr+DEVICE_OFFSET+0x070c) &= ~  ((0x7 <<27)|(0x7<<24));
		REG32(baseaddr+DEVICE_OFFSET+0x070c) |=  ((1)<<27) | ((3)<<24);   //L1=1us, L0s=3us

		REG32(baseaddr+0x80) &= ~(0x3);
		REG32(baseaddr+DEVICE_OFFSET+0x80) &= ~(0x3);		

		REG32(baseaddr+0x80) |= 3;   //L1
		REG32(baseaddr+DEVICE_OFFSET+0x80) |= 3;	//L1			

	}

	if(mode==9)
	{
		REG32(baseaddr+0x80) &= ~(0x3);
		REG32(baseaddr+DEVICE_OFFSET+0x80) &= ~(0x3);
	}
		
	
#if 0
	if(powerdown==1)
	{
//		REG32(0xb8b00044) &= ~(3);	//D0	
		HostPCIe_SetPhyMdioWrite(0xd, 0x15b6);

    REG32(PCIE_PHY0) = 0x1;	//bit7 PHY reset=0   bit0 Enable LTSSM=1
    REG32(PCIE_PHY0) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1


//	__delay(9000000);  //OK
	__delay(9000000);	
//	HostPCIe_SetPhyMdioWrite(0xd, 0x15b6);

	REG32(PCIE_PHY0) = 0x1;	//bit7 PHY reset=0   bit0 Enable LTSSM=1
        REG32(PCIE_PHY0) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1

//    REG32(PCIE_PHY0) = 0x1;	//bit7 PHY reset=0   bit0 Enable LTSSM=1
//    REG32(PCIE_PHY0) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1
	
	
	//	REG32(0xb8b10044) &= ~(3);	//D0		
		dprintf("333 \n");	
		dprintf("status=%x \n", REG32(0xb8b00728) );		
	}
#endif

       //-------------------------------------------------------------	
	if(mode==0x010)
	{
             //L0->L1->L0
		tmp = REG32(0xb8b10044) &( ~(3));  //D0
	/*	
		REG32(0xb8b10044) = tmp|  (0);  //D0	


		while(1)
		{	if((REG32(0xb8b00728)&0x1f)==0x11)   //wait to L0
			break;
		}	
	
       */
		REG32(0xb8b10044) = tmp|  (3);  //D3	
		/*
		while(1)
		{	if((REG32(0xb8b00728)&0x1f)==0x14)   //wait to L1
			break;
		}	
		*/
		delay_ms(100);
		//REG32(0xb8b10000);
		REG32(0xb8b10044) = tmp|  (0);  //D0, wakeup
		
		while(1)
		{	if((REG32(0xb8b00728)&0x1f)==0x11)   //wait to L0
			break;
		}	
		
		//delay_ms(100);
		dprintf("DID/VID=%x\n", REG32(0xb8b10000));
	}
	//-------------------------------------------------------------	
	if(mode==0x020)
	{
             //L0->L2->L0
		tmp = REG32(0xb8b10044) &( ~(3));  //D0

		REG32(0xb8b10044) = tmp|  (3);  //D3	
		delay_ms(100);

		REG32(0xb8b01008) |= (0x200);  
        __delay(100000);  		
		//dprintf("Host boardcase PME_TurnOff \n");	
		//delay_ms(100);

	//wakeup
     REG32(SYS_CLKMANAGE) &= ~(1<<12);    //perst=0 off.
            //dprintf("CLK_MANAGE=%x \n",  REG32(CLK_MANAGE));
        __delay(100000);   
        __delay(100000);   
        __delay(100000);   
		
    REG32(SYS_CLKMANAGE) |=  (1<<12);   //PERST=1
    //prom_printf("\nCLK_MANAGE(0x%x)=0x%x\n\n",CLK_MANAGE,READ_MEM32(CLK_MANAGE));

	
        //4. PCIE PHY Reset       
    REG32(PCIE_PHY0) = 0x01;	//bit7 PHY reset=0   bit0 Enable LTSSM=1
    REG32(PCIE_PHY0) = 0x81;   //bit7 PHY reset=1   bit0 Enable LTSSM=1

#if 0  //wait for LinkUP
	int i=1000;
	while(--i)
	{
	      if( (REG32(0xb8b00728)&0x1f)==0x11)
		  	break;
      		__delay(100000);		  

	}
	if(i==0)
	{	dprintf("i=%x  Cannot LinkUP \n",i);
	}
#else

	while(1)
	{
		if( (REG32(0xb8b00728)&0x1f)==0x11)
		  	break;
	}
#endif



		dprintf("DID/VID=%x\n", REG32(0xb8b10000));
	}
		





		dprintf("Port%d Link status=%x \n", portnum, REG32(baseaddr+0x728)&0x1f );			
		
};

//---------------------------------------------------------------------------
int PCIE_PHYLoop(int argc, char* argv[])
{
	int portnum=0;
	int mode=1; 
	if( argc < 2 ) 
	{
		dprintf("epdn <portnum> <stop/start>.\n");	
		return 0;
	}
	if(argc>=1)	portnum = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	if(argc>=2)   mode = strtoul((const char*)(argv[1]), (char **)NULL, 16);	




	unsigned int v;
	v=HostPCIe_SetPhyMdioRead(portnum, 0);  


	if(mode==1)
	{	
		dprintf("Start... port=%d   ", portnum);

		v=v|(1<<8);  //bit 8
		HostPCIe_SetPhyMdioWrite(portnum, 0x00, v);  
		
		// start bist
		HostPCIe_SetPhyMdioWrite(portnum, 0x13, 0x8497);  
		delay_ms(100);
		HostPCIe_SetPhyMdioWrite(portnum, 0x14, 0xde01);  
		delay_ms(100);		
		HostPCIe_SetPhyMdioWrite(portnum, 0x13, 0x8c97);  
		delay_ms(100);		
		HostPCIe_SetPhyMdioWrite(portnum, 0x13, 0xcc97);  	
		delay_ms(100);		
		//end bist


	
	}
	else if(mode==0)
	{	
		dprintf("Stop...  port=%d  ", portnum);	
		v=v&0xffffffff-(1<<8);
		HostPCIe_SetPhyMdioWrite(portnum, 0, v);  	
	}


	//if(mode==1)
	{
		delay_ms(500);
		
		v=HostPCIe_SetPhyMdioRead(portnum, 0x17);	
		dprintf("LOOP CNT=%x ",v);		
		v=HostPCIe_SetPhyMdioRead(portnum, 0x18);
		v=(v &  (0xff<<4)) >>4;  //bit [11:04]
		dprintf("ERR=%x\n",v);
	}
	


}; 

//---------------------------------------------------------------------------
int  HostPCIe_TestINT(int argc, char* argv[])
{
	if( argc < 2 ) 
	{
		dprintf("eint <portnum> <loops>.\n");	
	
		return 0;
	}
	unsigned int portnum=0, loops=10;
	if(argc>=1)	portnum = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	if(argc>=2)   loops = strtoul((const char*)(argv[1]), (char **)NULL, 16);


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

	REG32(memmapaddr + 0x3c) |= (1<<8);  //swint mask

	#define SWINT (1<<24)
	#define PCIE0INT (1<<21)
	#define PCIE1INT (1<<22)	

	unsigned int  PCIEISR;
	if(portnum==0)	PCIEISR=PCIE0INT;
	else 			PCIEISR=PCIE1INT;

	
	#define GISR (0xb8003004)
	
	int i;
	for(i=0; i<loops; i++)
	{

		REG32(memmapaddr + 0x38) |= (1<<0);  //kick swint
		
		while(1) 
		{
			if( REG32(memmapaddr + 0x3c) & SWINT)   //check interrupt status swINT=1
				break;
		}

		dprintf("Got   SWINT %x=%x \n",memmapaddr + 0x3c, REG32(memmapaddr + 0x3c) );

		
		while(1) 
		{
			if( REG32(GISR) & PCIEISR )  //check pcie port 0
				break;
		}
		dprintf("Got   GISR=%x \n", REG32(0xb8003004) );


		//-----------------
		REG32(memmapaddr + 0x3c) |= SWINT;  //write to clear



		while(1) 
		{
			if( (REG32(memmapaddr + 0x3c) & SWINT) ==0)  //check interrupt status swINT=1
				break;
		}

		dprintf("Clear SWINT %x=%x \n",memmapaddr + 0x3c, REG32(memmapaddr + 0x3c) );

		
		while(1) 
		{
			if( (REG32(GISR) & PCIEISR )==0)  //check pcie port 0
				break;
		}
		dprintf("Clear GISR=%x \n", REG32(0xb8003004) );
	}
	

}


//--------------------------------------------------------------------------------




