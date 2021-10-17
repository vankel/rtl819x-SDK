
#include <linux/interrupt.h>
#include <asm/system.h>

#include "monitor.h"
#include "etherboot.h"
#include "nic.h"


#include <asm/mipsregs.h>	//wei add

#if defined(CONFIG_SPI_FLASH) 
#include "spi_flash.h"
#endif

#include <rtl8196x/asicregs.h>        


#if defined(RTL8198)
#include <asm/rtl8198.h>
#endif

#define SWITCH_CMD 1

#define SYS_BASE 0xb8000000
#define SYS_INI_STATUS (SYS_BASE +0x04)
#define SYS_HW_STRAP (SYS_BASE +0x08)
#define SYS_CLKMANAGE (SYS_BASE +0x10)
//hw strap
#define ST_SYNC_OCP_OFFSET 9
#define CK_M2X_FREQ_SEL_OFFSET 10
#define ST_CPU_FREQ_SEL_OFFSET 13
#define ST_CPU_FREQDIV_SEL_OFFSET 19
#define ST_BOOTPINSEL (1<<0)
#define ST_DRAMTYPE (1<<1)
#define ST_BOOTSEL (1<<2)
#define ST_PHYID (0x3<<3) //11b 
#define ST_EN_EXT_RST (1<<8)
#define ST_SYNC_OCP (1<<9)
#define CK_M2X_FREQ_SEL (0x7 <<10)
#define ST_CPU_FREQ_SEL (0xf<<13)
#define ST_NRFRST_TYPE (1<<17)
#define SYNC_LX (1<<18)
#define ST_CPU_FREQDIV_SEL (0x7<<19)
#define ST_EVER_REBOOT_ONCE (1<<23)
#define ST_SYS_DBG_SEL  (0x3f<<24)
#define ST_PINBUS_DBG_SEL (3<<30)
#define SPEED_IRQ_NO 29
#define SPEED_IRR_NO 3
#define SPEED_IRR_OFFSET 20
#define MAIN_PROMPT						"<RealTek>"
#define putchar(x)	serial_outc(x)
#define IPTOUL(a,b,c,d)	((a << 24)| (b << 16) | (c << 8) | d )

extern unsigned int	_end;
extern unsigned char 	ethfile[20];
extern struct arptable_t	arptable[MAX_ARP];

#ifdef CONFIG_NIC_LOOPBACK
int nic_loopback = 0;
#endif

int YesOrNo(void);
int CmdHelp( int argc, char* argv[] );


#if defined(CONFIG_BOOT_DEBUG_ENABLE)
int CmdDumpWord( int argc, char* argv[] );
int CmdDumpByte( int argc, char* argv[] ); //wei add
int CmdWriteWord( int argc, char* argv[] );
int CmdWriteByte( int argc, char* argv[] );
int CmdWriteHword( int argc, char* argv[] );
int CmdWriteAll( int argc, char* argv[] );
int CmdCmp(int argc, char* argv[]);
int CmdIp(int argc, char* argv[]);
int CmdAuto(int argc, char* argv[]);
#endif
int CmdLoad(int argc, char* argv[]);

int CmdCfn(int argc, char* argv[]);


#ifdef CONFIG_NOR_FLASH
int CmdFlw(int argc, char* argv[]);
int CmdFlr(int argc, char* argv[]);
#endif


#ifdef CONFIG_SPI_FLASH
	int CmdSFlw(int argc, char* argv[]);
	int CmdFlr(int argc, char* argv[]);
	extern void auto_spi_memtest_8198(unsigned long DRAM_starting_addr, unsigned int spi_clock_div_num);
#endif


#ifdef CONFIG_NAND_FLASH
int CmdNANDID(int argc, char* argv[]);
int CmdNANDBE(int argc, char* argv[]);
int CmdNANDR(int argc, char* argv[]);
int CmdNANDW(int argc, char* argv[]);
char* rtk_nand_read_id(void);
int rtk_nand_probe(void);
int rtk_erase_block (int page);                      // 1 block=64 page
int rtk_read_ecc_page (unsigned long flash_address, unsigned char *image_addr,
unsigned int image_size);
int rtk_write_ecc_page (unsigned long flash_address, unsigned char *image_addr,
unsigned int image_size);
#endif


//int CmdTimer(int argc, char* argv[]);
//int CmdMTC0SR(int argc, char* argv[]);  //wei add
//int CmdMFC0SR(int argc, char* argv[]);  //wei add
//int CmdTFTP(int argc, char* argv[]);  //wei add
#if defined(CONFIG_BOOT_DEBUG_ENABLE)
#endif
#ifdef CONFIG_RTL8196D_TAROKO
int CmdIMEM96DTEST(int argc, char* argv[]);
int CmdWBMG(int argc, char* argv[]);
int CmdWATCH(int argc, char* argv[]);
#endif
#ifdef CONFIG_IIS_TEST
int TestCmd_IIS( int argc, char* argv[]);
int TestCmd_IISSETTING( int argc, char* argv[]);
int TestCmd_I2C( int argc, char* argv[]);
int TestCmd_GPIO(int argc, char* argv[]);
int TestCmd_GPIOR(int argc, char* argv[]);
#endif
//Ziv
#ifdef WRAPPER
#ifndef CONFIG_SPI_FLASH
//write bootcode to flash from my content
int CmdWB(int argc, char* argv[]);

#endif
	
#ifdef CONFIG_SPI_FLASH
int CmdSWB(int argc, char* argv[]);
#endif

#ifdef CONFIG_NAND_FLASH
int CmdNWB(int argc, char* argv[]);                  //for NAND Flash
#endif
extern char _bootimg_start, _bootimg_end;
#endif




#ifdef  CONFIG_DRAM_TEST
	void Dram_test(int argc, char* argv[]);
	void Dram_trxdly_test(int argc, char* argv[]);
#endif



#ifdef  CONFIG_SPI_TEST
	int CmdSTEST(int argc, char* argv[]);               //JSW: add for SPI/SDRAM auto-memory-test program
#endif


#if 1//def CONFIG_CPUsleep_PowerManagement_TEST
	int CmdCPUSleep(int argc, char* argv[]);
	void CmdCPUSleepIMEM(void);
#endif


#if defined(CONFIG_PCIE_MODULE) 
int CmdTestPCIE(int argc, char* argv[]);
#endif






#if SWITCH_CMD
int TestCmd_MDIOR(int argc, char* argv[]);  //wei add
int TestCmd_MDIOW(int argc, char* argv[]);  //wei add
int CmdPHYregR(int argc, char* argv[]);
int CmdPHYregW(int argc, char* argv[]);
#endif

int CmdPortP1Patch(int argc, char* argv[]);

#ifdef CONFIG_NIC_LOOPBACK
static int CmdSetLpbk(int argc, char* argv[]);
#endif

/*Cyrus Tsai*/
/*move to ehterboot.h
#define TFTP_SERVER 0
#define TFTP_CLIENT 1
*/
extern struct arptable_t  arptable_tftp[3];
/*Cyrus Tsai*/

//extern int flasherase(unsigned long src, unsigned int length);
//extern int flashwrite(unsigned long dst, unsigned long src, unsigned long length);
//extern int flashread (unsigned long dst, unsigned long src, unsigned long length);

extern int write_data(unsigned long dst, unsigned long length, unsigned char *target);
extern int read_data (unsigned long src, unsigned long length, unsigned char *target);

/*Cyrus Tsai*/
extern unsigned long file_length_to_server;
extern unsigned long file_length_to_client;
extern unsigned long image_address; 
/*this is the file length, should extern to flash driver*/
/*Cyrus Tsai*/

#if 1//defined(RTL8196D)
#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))

#define SWCORE_BASE      0xBB800000        
#define PCRAM_BASE       (0x4100+SWCORE_BASE)
#define PITCR                  (0x000+PCRAM_BASE)       /* Port Interface Type Control Register */
#define PCRP0                 (0x004+PCRAM_BASE)       /* Port Configuration Register of Port 0 */
#define PCRP1                 (0x008+PCRAM_BASE)       /* Port Configuration Register of Port 1 */
#define PCRP2                 (0x00C+PCRAM_BASE)       /* Port Configuration Register of Port 2 */
#define PCRP3                 (0x010+PCRAM_BASE)       /* Port Configuration Register of Port 3 */
#define PCRP4                 (0x014+PCRAM_BASE)       /* Port Configuration Register of Port 4 */
#define EnablePHYIf        (1<<0)                           /* Enable PHY interface.                    */
#endif
 



COMMAND_TABLE	MainCmdTable[] =
{
	{ "?"	  ,0, CmdHelp			, "HELP (?)				    : Print this help message"					},
#if defined(CONFIG_BOOT_DEBUG_ENABLE)												
	{ "DB"	  ,2, CmdDumpByte		, "DB <Address> <Len>"}, //wei add	
	{ "DW"	  ,2, CmdDumpWord		, "DW <Address> <Len>"},  //same command with ICE, easy use
	{ "EB",2, CmdWriteByte, "EB <Address> <Value1> <Value2>..."},	
	{ "EW",2, CmdWriteWord, "EW <Address> <Value1> <Value2>..."},
	{ "CMP",3, CmdCmp, "CMP: CMP <dst><src><length>"},
	{ "IPCONFIG",2, CmdIp, "IPCONFIG:<TargetAddress>"},
#ifndef CONFIG_NONE_FLASH
	{ "AUTOBURN"   ,1, CmdAuto			, "AUTOBURN: 0/1" },
#endif
#endif
	{ "LOADADDR"   ,1, CmdLoad			, "LOADADDR: <Load Address>"					},
	{ "J"  ,1, CmdCfn			, "J: Jump to <TargetAddress>"											},


#ifdef CONFIG_NOR_FLASH
	{ "FLR"   ,3, CmdFlr			, "FLR: FLR <dst><src><length>"					},	
	{ "FLW"   ,3, CmdFlw			, "FLW: FLW <dst><src><length>"					},	
#ifdef WRAPPER
	{ "WB", 0, CmdWB, "WB: WB"},
#endif

#endif

#ifdef CONFIG_RTL8196D_TAROKO
	{ "IMEMTEST"   ,0, CmdIMEM96DTEST			, "IMEMTEST "					},
	{ "WBMG"	,0, CmdWBMG			, "WBMG: write buffer merge"				},
	{ "WATCH"	,1, CmdWATCH			, "WATCH: CPU MEM WATCH & PROTECT"			},
#endif
#ifdef CONFIG_IIS_TEST
	{ "IIS"		,0, TestCmd_IIS			, "IIS"							},
	{ "IISSETTING"		,0, TestCmd_IISSETTING		, "IISSETTING mode"					},
	{ "I2C"		,0, TestCmd_I2C			, "I2C read=0/write=1 register value"			},
	{ "GPIO"	,0, TestCmd_GPIO		, "GPIO pin value"					},
	{ "GPIOR"	,0, TestCmd_GPIOR		, "GPIOR pin"					},
#endif
#ifdef CONFIG_SPI_FLASH
	{ "FLR"   ,3, CmdFlr			, "FLR: FLR <dst><src><length>"					},	
	{ "FLW",4, CmdSFlw, "FLW <dst_ROM_offset><src_RAM_addr><length_Byte> <SPI cnt#>: Write offset-data to SPI from RAM"},	 //JSW
#ifdef WRAPPER
	{ "SWB", 1, CmdSWB, "SWB <SPI cnt#> (<0>=1st_chip,<1>=2nd_chip): SPI Flash WriteBack (for MXIC/Spansion)"}, 	//JSW	
#endif	
#endif

#ifdef CONFIG_NAND_FLASH
      { "NANDID",0, CmdNANDID, "NANDID: Read NAND Flash ID"},
    { "NANDBE",3, CmdNANDBE, "NANDBE:<Block start cnt><Block end cnt>"},
    { "NANDR",3, CmdNANDR, "NANDR:<flash_Paddress><image_addr><image_size>"},
    { "NANDW",3, CmdNANDW, "NANDW:<flash_Paddress><image_addr><image_size>"},
#ifdef WRAPPER
	  {"NWB", 1, CmdNWB, "NWB <NWB cnt#> (<0>=1st_chip,<1>=2nd_chip): NAND Flash WriteBack "},	
#endif	
#endif


#if SWITCH_CMD
	{ "MDIOR"   ,0, TestCmd_MDIOR			, "MDIOR:  MDIOR <phyid> <reg>"				}, //wei add, 	
	{ "MDIOW"   ,0, TestCmd_MDIOW			, "MDIOW:  MDIOW <phyid> <reg> <data>"				}, //wei add, 	
  		{ "PHYR",2, CmdPHYregR, "PHYR: PHYR <PHYID><reg>"},
 		{ "PHYW",3, CmdPHYregW, "PHYW: PHYW <PHYID><reg><data>"},
#endif
 		{ "PORT1",3, CmdPortP1Patch, "PORT1: port 1 patch for FT2"},		

#ifdef CONFIG_DRAM_TEST
	{ "DRAMTEST",2,Dram_test , "DRAMTEST <R/W> <enable_random_delay> <PowerManagementMode>" },
	{ "TR",2,Dram_trxdly_test , "TR <rx_shitf> <tx_shitf> " },
#endif


#if defined(CONFIG_PCIE_MODULE) 
	{ "PCIE",0, CmdTestPCIE, "PCIE: Test Host PCI-E"},	
#endif

#ifdef CONFIG_NIC_LOOPBACK
	{ "LPBK",	0,	CmdSetLpbk,	"LPBK: NIC loopback enable/disable"},
#endif
};



//------------------------------------------------------------------------------
/********   caculate CPU clock   ************/
int check_cpu_speed(void);
void timer_init(unsigned long lexra_clock);
static void timer_interrupt(int num, void *ptr, struct pt_regs * reg);
struct irqaction irq_timer = {timer_interrupt, 0, 8, "timer", NULL, NULL};                                   
static volatile unsigned int jiffies=0;
static void timer_interrupt(int num, void *ptr, struct pt_regs * reg)
{
	//dprintf("jiffies=%x\r\n",jiffies);
	//flush_WBcache();
	rtl_outl(TCIR,rtl_inl(TCIR));
	jiffies++;


}
volatile int get_timer_jiffies(void)
{

	return jiffies;
};



//------------------------------------------------------------------------------
void timer_init(unsigned long lexra_clock)
{
    /* Set timer mode and Enable timer */
    REG32(TCCNR_REG) = (0<<31) | (0<<30);	//using time0
    //REG32(TCCNR_REG) = (1<<31) | (0<<30);	//using counter0

	#define DIVISOR     0xE
	#define DIVF_OFFSET                         16		
    REG32(CDBR_REG) = (DIVISOR) << DIVF_OFFSET;
    
    /* Set timeout per msec */

	int SysClkRate = lexra_clock;	 /* CPU 200MHz */

	#define TICK_10MS_FREQ  100 /* 100 Hz */
	#define TICK_100MS_FREQ 1000 /* 1000 Hz */
	#define TICK_FREQ       TICK_10MS_FREQ	
   
                REG32(TC0DATA_REG) = (((SysClkRate / DIVISOR) / TICK_FREQ) + 1) <<4;

       
    /* Set timer mode and Enable timer */
    REG32(TCCNR_REG) = (1<<31) | (1<<30);	//using time0
    /* We must wait n cycles for timer to re-latch the new value of TC1DATA. */
	int c;	
	for( c = 0; c < DIVISOR; c++ );
	

      /* Set interrupt mask register */
    //REG32(GIMR_REG) |= (1<<8);	//request_irq() will set 

    /* Set interrupt routing register */
  // RTL8198
    REG32(IRR1_REG) = 0x00050004;  //uart:IRQ5,  time0:IRQ4
   
    
    /* Enable timer interrupt */
    REG32(TCIR_REG) = (1<<31);
}
//------------------------------------------------------------------------------

__inline__ void
__delay(unsigned long loops)
{
	__asm__ __volatile__ (
		".set\tnoreorder\n"
		"1:\tbnez\t%0,1b\n\t"
		"subu\t%0,1\n\t"
		".set\treorder"
		:"=r" (loops)
		:"0" (loops));
}

/*
80007988 <__delay>:                                             
80007988:	1480ffff 	bnez	a0,80007988 <__delay>           
8000798c:	2484ffff 	addiu	a0,a0,-1                        
80007990:	03e00008 	jr	ra                                  
*/

//---------------------------------------------------------------------------
static unsigned long loops_per_jiffy = (1<<12);
#define LPS_PREC 8
#define HZ 100
#ifdef RTL8198
unsigned long loops_per_sec = 2490368 * HZ;	// @CPU 500MHz (this will be update in check_cpu_speed())
#else
unsigned long loops_per_sec = 0x1db000 * HZ;	// @CPU 390MHz, DDR 195 MHz (this will be update in check_cpu_speed())
#endif

int check_cpu_speed(void)
{
	unsigned long ticks, loopbit;
	int lps_precision = LPS_PREC;
      
  // RTL8198
  	request_IRQ(8, &irq_timer, NULL); 

	extern long glexra_clock;
       timer_init(glexra_clock);	

	loops_per_jiffy = (1<<12);
	while (loops_per_jiffy <<= 1) {
		/* wait for "start of" clock tick */
		ticks = jiffies;
		while (ticks == jiffies)
			/* nothing */;
		/* Go .. */
		ticks = jiffies;
		__delay(loops_per_jiffy);
		ticks = jiffies - ticks;
		if (ticks)
			break;
	}
/* Do a binary approximation to get loops_per_jiffy set to equal one clock
   (up to lps_precision bits) */
	loops_per_jiffy >>= 1;
	loopbit = loops_per_jiffy;
	while ( lps_precision-- && (loopbit >>= 1) ) 
	{
		loops_per_jiffy |= loopbit;
		ticks = jiffies;
		while (ticks == jiffies);
		ticks = jiffies;
		__delay(loops_per_jiffy);
		if (jiffies != ticks)	/* longer than 1 tick */
			loops_per_jiffy &= ~loopbit;
	}


	
	//timer_stop();	//wei del, because not close timer
	//free_IRQ(8);
/* Round the value and print it */	
	//prom_printf("cpu run %d.%d MIPS\n", loops_per_jiffy/(500000/HZ),      (loops_per_jiffy/(5000/HZ)) % 100);
	return ((loops_per_jiffy/(500000/HZ))+1);
	
}
//---------------------------------------------------------------------------


/*
---------------------------------------------------------------------------
;				Monitor
---------------------------------------------------------------------------
*/
extern char** GetArgv(const char* string);

void monitor(void)
{
	char		buffer[ MAX_MONITOR_BUFFER +1 ];
	int		argc ;
	char**		argv ;
	int		i, retval ;
	
//	i = &_end;
//	i = (i & (~4095)) + 4096;
	//printf("Free Mem Start=%X\n", i);
	while(1)
	{	
		printf( "%s", MAIN_PROMPT );
		memset( buffer, 0, MAX_MONITOR_BUFFER );
		GetLine( buffer, MAX_MONITOR_BUFFER,1);
		printf( "\n" );
		argc = GetArgc( (const char *)buffer );
		argv = GetArgv( (const char *)buffer );
		if( argc < 1 ) continue ;
		StrUpr( argv[0] );
		for( i=0 ; i < (sizeof(MainCmdTable) / sizeof(COMMAND_TABLE)) ; i++ )
		{
			
			if( ! strcmp( argv[0], MainCmdTable[i].cmd ) )
			{
#if 0
				if (MainCmdTable[i].n_arg != (argc - 1))
					printf("%s\n", MainCmdTable[i].msg);
				else
					retval = MainCmdTable[i].func( argc - 1 , argv+1 );
#endif
				retval = MainCmdTable[i].func( argc - 1 , argv+1 );
				memset(argv[0],0,sizeof(argv[0]));
				break;
			}
		}
		if(i==sizeof(MainCmdTable) / sizeof(COMMAND_TABLE)) printf("Unknown command !\r\n");
	}
}


//---------------------------------------------------------------------------------------



//---------------------------------------------------------------------------------------


#ifdef WRAPPER

#ifdef CONFIG_NOR_FLASH
int CmdWB(int argc, char* argv[])
{
       char* start = &_bootimg_start;
	char* end  = &_bootimg_end;
        
	   
	unsigned int length = end - start;
	
	printf("Flash wille write %X length of embedded boot code at %X to %X\n", length, start, end);
	printf("(Y)es, (N)o->");
	if (YesOrNo())
		if (flashwrite(0, (unsigned long)start, length))
			printf("Flash Write Successed!\n");
		else
			printf("Flash Write Failed!\n");
	else
		printf("Abort!\n");

}
#endif


#ifdef CONFIG_SPI_FLASH
extern char _bootimg_start, _bootimg_end;
//SPI Write-Back
int CmdSWB(int argc, char* argv[])
{
	unsigned short auto_spi_clock_div_num;//0~7
	unsigned int  cnt=strtoul((const char*)(argv[0]), (char **)NULL, 16);	//JSW check
	char* start = &_bootimg_start;
	char* end  = &_bootimg_end;	   
	unsigned int length = end - start;		
	printf("SPI Flash #%d will write 0x%X length of embedded boot code from 0x%X to 0x%X\n", cnt+1,length, start, end);
	printf("(Y)es, (N)o->");
	if (YesOrNo())
	{
		spi_pio_init();
		 #if defined(SUPPORT_SPI_MIO_8198_8196C)
			spi_flw_image_mio_8198(cnt, 0, (unsigned char*)start , length);	
	  	#else			
			spi_flw_image(cnt, 0, (unsigned char*)start , length);
		#endif
		printf("SPI Flash Burn OK!\n");
	}	
	else 
	{
        	printf("Abort!\n");	
	}	
  }

#endif

#ifdef CONFIG_NAND_FLASH
                 //NAND Write-Back
int CmdNWB(int argc, char* argv[])
{
                                                     //JSW check
    unsigned int  cnt=strtoul((const char*)(argv[0]), (char **)NULL, 16);
    char* start = &_bootimg_start;
    char* end  = &_bootimg_end;
    unsigned int length = end - start;

    printf("NAND Flash #%d will write 0x%X length of embedded boot code from 0x%X to 0x%X\n", cnt,length, start, end);
    printf("(Y)es, (N)o->");
    if (YesOrNo())
    {
        rtk_write_ecc_page (0,start,length);       
    }
    else
    {
        printf("Abort!\n");
    }
}
#endif
#endif
/*/
---------------------------------------------------------------------------
; Ethernet Download
---------------------------------------------------------------------------
*/




extern unsigned long ETH0_ADD;
int CmdCfn(int argc, char* argv[])
{
	unsigned long		Address;
	void	(*jump)(void);
	if( argc > 0 )
	{
		if(!Hex2Val( argv[0], &Address ))
		{
			printf(" Invalid Address(HEX) value.\n");
			return FALSE ;
		}
	}

	dprintf("---Jump to address=%X\n",Address);
	jump = (void *)(Address);
	outl(0,GIMR0); // mask all interrupt
	cli(); 
#if defined(RTL8198)
#ifndef CONFIG_FPGA_PLATFORM
      /* if the jump-Address is BFC00000, then do watchdog reset */
      if(Address==0xBFC00000)
      	{
      	   *(volatile unsigned long *)(0xB800311c)=0; /*this is to enable 865xc watch dog reset*/
          for( ; ; );
      	}
     else /*else disable PHY to prevent from ethernet disturb Linux kernel booting */
     	{
           WRITE_MEM32(PCRP0, (READ_MEM32(PCRP0)&(~EnablePHYIf )) ); 
           WRITE_MEM32(PCRP1, (READ_MEM32(PCRP1)&(~EnablePHYIf )) ); 
           WRITE_MEM32(PCRP2, (READ_MEM32(PCRP2)&(~EnablePHYIf )) ); 
           WRITE_MEM32(PCRP3, (READ_MEM32(PCRP3)&(~EnablePHYIf )) ); 
           WRITE_MEM32(PCRP4, (READ_MEM32(PCRP4)&(~EnablePHYIf )) ); 
	flush_cache();
     	}
#endif
#endif
	jump();	
	
}



//---------------------------------------------------------------------------
#if defined(CONFIG_BOOT_DEBUG_ENABLE)	
//---------------------------------------------------------------------------
/* This command can be used to configure host ip and target ip	*/

extern char eth0_mac[6];
int CmdIp(int argc, char* argv[])
{
	unsigned char  *ptr;
	unsigned int i;
	int  ip[4];
	
	if (argc==0)
	{	
		printf(" Target Address=%d.%d.%d.%d\n",
		arptable_tftp[TFTP_SERVER].ipaddr.ip[0], arptable_tftp[TFTP_SERVER].ipaddr.ip[1], 
		arptable_tftp[TFTP_SERVER].ipaddr.ip[2], arptable_tftp[TFTP_SERVER].ipaddr.ip[3]);
#ifdef HTTP_SERVER
		printf("   Http Address=%d.%d.%d.%d\n",
		arptable_tftp[HTTPD_ARPENTRY].ipaddr.ip[0], arptable_tftp[HTTPD_ARPENTRY].ipaddr.ip[1], 
		arptable_tftp[HTTPD_ARPENTRY].ipaddr.ip[2], arptable_tftp[HTTPD_ARPENTRY].ipaddr.ip[3]);
#endif
		return;	 
	}			
	
	ptr = argv[0];

	for(i=0; i< 4; i++)
	{
		ip[i]=strtol((const char *)ptr,(char **)NULL, 10);		
		ptr = strchr(ptr, '.');
		ptr++;
	}
	arptable_tftp[TFTP_SERVER].ipaddr.ip[0]=ip[0];
	arptable_tftp[TFTP_SERVER].ipaddr.ip[1]=ip[1];
	arptable_tftp[TFTP_SERVER].ipaddr.ip[2]=ip[2];
	arptable_tftp[TFTP_SERVER].ipaddr.ip[3]=ip[3];
/*replace the MAC address middle 4 bytes.*/
	eth0_mac[1]=ip[0];
	eth0_mac[2]=ip[1];
	eth0_mac[3]=ip[2];
	eth0_mac[4]=ip[3];
	arptable_tftp[TFTP_SERVER].node[5]=eth0_mac[5];
	arptable_tftp[TFTP_SERVER].node[4]=eth0_mac[4];
	arptable_tftp[TFTP_SERVER].node[3]=eth0_mac[3];
	arptable_tftp[TFTP_SERVER].node[2]=eth0_mac[2];
	arptable_tftp[TFTP_SERVER].node[1]=eth0_mac[1];
	arptable_tftp[TFTP_SERVER].node[0]=eth0_mac[0];
	prom_printf("Now your Target IP is %d.%d.%d.%d\n", ip[0],ip[1],ip[2],ip[3]);
#if 0	
	ptr = argv[1];
	//prom_printf("You want to setup Host new ip as %s \n", ptr);	
	
	for(i=0; i< 4; i++)
	{
		ip[i]=strtol((const char *)ptr,(char **)NULL, 10);		
		ptr = strchr(ptr, '.');
		ptr++;
	}
	arptable[ARP_SERVER].ipaddr.ip[0]=ip[0];
	arptable[ARP_SERVER].ipaddr.ip[1]=ip[1];
	arptable[ARP_SERVER].ipaddr.ip[2]=ip[2];
	arptable[ARP_SERVER].ipaddr.ip[3]=ip[3];
	prom_printf("Now your Host IP is %d.%d.%d.%d\n", ip[0],ip[1],ip[2],ip[3]);
#endif	
		
}	
int CmdDumpWord( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned int len,i;

	if(argc<1)
	{	dprintf("Wrong argument number!\r\n");
		return;
	}
	
	if(argv[0])	
	{	src = strtoul((const char*)(argv[0]), (char **)NULL, 16);
		if(src <0x80000000)
			src|=0x80000000;
	}
	else
	{	dprintf("Wrong argument number!\r\n");
		return;		
	}
				
	if(!argv[1])
		len = 1;
	else
	len= strtoul((const char*)(argv[1]), (char **)NULL, 10);			
	while ( (src) & 0x03)
		src++;

	for(i=0; i< len ; i+=4,src+=16)
	{	
		dprintf("%08X:	%08X	%08X	%08X	%08X\n",
		src, *(unsigned long *)(src), *(unsigned long *)(src+4), 
		*(unsigned long *)(src+8), *(unsigned long *)(src+12));
	}

}

//---------------------------------------------------------------------------
int CmdDumpByte( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned int len,i;

	if(argc<1)
	{	dprintf("Wrong argument number!\r\n");
		return;
	}
	
	src = strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	if(!argv[1])
		len = 16;
	else
	len= strtoul((const char*)(argv[1]), (char **)NULL, 10);			


	ddump((unsigned char *)src,len);
}

//---------------------------------------------------------------------------
int CmdWriteWord( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned int value,i;
	
	src = strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	while ( (src) & 0x03)
		src++;

	for(i=0;i<argc-1;i++,src+=4)
	{
		value= strtoul((const char*)(argv[i+1]), (char **)NULL, 16);	
		*(volatile unsigned int *)(src) = value;
	}
	
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

int CmdWriteHword( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned short value,i;
	
	src = strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	
	src &= 0xfffffffe;	

	for(i=0;i<argc-1;i++,src+=2)
	{
		value= strtoul((const char*)(argv[i+1]), (char **)NULL, 16);	
		*(volatile unsigned short *)(src) = value;
	}
	
}
#endif
//---------------------------------------------------------------------------
int CmdWriteByte( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned char value,i;
	
	src = strtoul((const char*)(argv[0]), (char **)NULL, 16);		


	for(i=0;i<argc-1;i++,src++)
	{
		value= strtoul((const char*)(argv[i+1]), (char **)NULL, 16);	
		*(volatile unsigned char *)(src) = value;
	}
	
}
int CmdCmp(int argc, char* argv[])
{
	int i;
	unsigned long dst,src;
	unsigned long dst_value, src_value;
	unsigned int length;
	unsigned long error;

	if(argc < 3) {
		printf("Parameters not enough!\n");
		return 1;
	}
	dst = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	src = strtoul((const char*)(argv[1]), (char **)NULL, 16);
	length= strtoul((const char*)(argv[2]), (char **)NULL, 16);		
	error = 0;
	for(i=0;i<length;i+=4) {
		dst_value = *(volatile unsigned int *)(dst+i);
		src_value = *(volatile unsigned int *)(src+i);
		if(dst_value != src_value) {		
			printf("%dth data(%x %x) error\n",i, dst_value, src_value);
			error = 1;
		}
	}
	if(!error)
		printf("No error found\n");

}

//---------------------------------------------------------------------------
#ifndef RTL8197B
extern int autoBurn;
int CmdAuto(int argc, char* argv[])
{
	unsigned long addr;


	if(argv[0][0] == '0')
		autoBurn = 0 ;
	else
		autoBurn = 1 ;
	printf("AutoBurning=%d\n",autoBurn);
}
#endif


//---------------------------------------------------------------------------
int CmdLoad(int argc, char* argv[])
{
	unsigned long addr;


	image_address= strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	printf("Set TFTP Load Addr 0x%x\n",image_address);
}

/*
--------------------------------------------------------------------------
Flash Utility
--------------------------------------------------------------------------
*/
#if defined(CONFIG_NOR_FLASH) || defined(CONFIG_SPI_FLASH)
int CmdFlr(int argc, char* argv[])
{
	int i;
	unsigned long dst,src;
	unsigned int length;
	//unsigned char TARGET;
//#define  FLASH_READ_BYTE	4096

	dst = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	src = strtoul((const char*)(argv[1]), (char **)NULL, 16);
	length= strtoul((const char*)(argv[2]), (char **)NULL, 16);		
	//length= (length + (FLASH_READ_BYTE - 1)) & FLASH_READ_BYTE;

/*Cyrus Tsai*/
/*file_length_to_server;*/
//length=file_length_to_client;
//length=length & (~0xffff)+0x10000;
//dst=image_address;
file_length_to_client=length;
/*Cyrus Tsai*/

	printf("Flash read from %X to %X with %X bytes	?\n",src,dst,length);
	printf("(Y)es , (N)o ? --> ");

	if (YesOrNo())
	        //for(i=0;i<length;i++)
	        //   {
		//    if ( flashread(&TARGET, src+i,1) )
		//	printf("Flash Read Successed!, target %X\n",TARGET);
		//    else
		//	printf("Flash Read Failed!\n");
		//  }	
		    if (flashread(dst, src, length))
			printf("Flash Read Successed!\n");
		    else
			printf("Flash Read Failed!\n");
	else
		printf("Abort!\n");
//#undef	FLASH_READ_BYTE		4096

}
#endif


#ifndef RTL8197B
/* Setting image header */


#ifdef CONFIG_NOR_FLASH
int CmdFlw(int argc, char* argv[])
{
	unsigned long dst,src;
	unsigned long length;

#define FLASH_WRITE_BYTE 4096

	dst = strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	src = strtoul((const char*)(argv[1]), (char **)NULL, 16);		
	length= strtoul((const char*)(argv[2]), (char **)NULL, 16);		
//	length= (length + (FLASH_WRITE_BYTE - 1)) & FLASH_WRITE_BYTE;

/*Cyrus Tsai*/
/*file_length_to_server;*/
//length=file_length_to_server;
//length=length & (~0xffff)+0x10000;
/*Cyrus Tsai*/

	
	printf("Flash Program from %X to %X with %X bytes	?\n",src,dst,length);
	printf("(Y)es, (N)o->");
	if (YesOrNo())
		if (flashwrite(dst, src, length))
			printf("Flash Write Successed!\n");
		else
			printf("Flash Write Failed!\n");
	else
		printf("Abort!\n");
#undef FLASH_WRITE_BYTE //4096

        //---------------------------------------------------------------------------

}




#endif
//---------------------------------------------------------------------------
#endif //RTL8197B

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#if !defined(CONFIG_BOOT_DEBUG_ENABLE)
extern char eth0_mac[6];
#endif
/*
------------------------------------------  ---------------------------------
; Command Help
---------------------------------------------------------------------------
*/
  

int CmdHelp( int argc, char* argv[] )
{
	int	i ;

    printf("----------------- COMMAND MODE HELP ------------------\n");
	for( i=0  ; i < (sizeof(MainCmdTable) / sizeof(COMMAND_TABLE)) ; i++ )
	{
		if( MainCmdTable[i].msg )
		{
			printf( "%s\n", MainCmdTable[i].msg );
		}
	}
	/*Cyrus Tsai*/
    
	return TRUE ;
}



//---------------------------------------------------------------------------


int YesOrNo(void)
{
	unsigned char iChar[2];

	GetLine( iChar, 2,1);
	printf("\n");//vicadd
	if ((iChar[0] == 'Y') || (iChar[0] == 'y'))
		return 1;
	else
		return 0;
}
//---------------------------------------------------------------------------
#ifdef CONFIG_SPI_FLASH
int CmdSFlw(int argc, char* argv[])
{
	unsigned int  cnt2=0;//strtoul((const char*)(argv[3]), (char **)NULL, 16);	
	unsigned int  dst_flash_addr_offset=strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	unsigned int  src_RAM_addr=strtoul((const char*)(argv[1]), (char **)NULL, 16);
	unsigned int  length=strtoul((const char*)(argv[2]), (char **)NULL, 16);
	unsigned int  end_of_RAM_addr=src_RAM_addr+length;	
	printf("Write 0x%x Bytes to SPI flash#%d, offset 0x%x<0x%x>, from RAM 0x%x to 0x%x\n" ,length,cnt2+1,dst_flash_addr_offset,dst_flash_addr_offset+0xbd000000,src_RAM_addr,end_of_RAM_addr);
	printf("(Y)es, (N)o->");
	if (YesOrNo())
	{
		spi_pio_init();
		  #if defined(SUPPORT_SPI_MIO_8198_8196C) && defined(CONFIG_SPI_FLASH)
			spi_flw_image_mio_8198(cnt2, dst_flash_addr_offset, (unsigned char*)src_RAM_addr , length);	
		  #else			
			spi_flw_image(cnt2, dst_flash_addr_offset, (unsigned char*)src_RAM_addr , length);	
		 #endif
	}//end if YES
	else
		printf("Abort!\n");
}
#endif
//---------------------------------------------------------------------------
#if SWITCH_CMD
int TestCmd_MDIOR( int argc, char* argv[] )
{
	if(argc < 1) {
		printf("Parameters not enough!\n");
		return 1;
	}

//	unsigned int phyid = strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	unsigned int reg = strtoul((const char*)(argv[0]), (char **)NULL, 10);		
	unsigned int data;
	int i,phyid;
	for(i=0;i<32;i++)
	{
		phyid=i;
		//REG32(PABCDDAT_REG) =  0xffff<<8;
	rtl8651_getAsicEthernetPHYReg(phyid,reg,&data); 	
		//REG32(PABCDDAT_REG) =  0<<8;	
	dprintf("PhyID=0x%02x Reg=%02d Data =0x%04x\r\n", phyid, reg,data);

	}

}

#ifdef CONFIG_RTL8196D_TAROKO
int CmdIMEM96DTEST(int argc, char* argv[])
{
	imem_test();
	return 0;
}

int CmdWBMG(int argc, char* argv[])
{
	write_buf_merge();
	return 0;
}

int CmdWATCH(int argc, char* argv[])
{
	unsigned long addr;

	if (argv[0]) {
		addr = strtoul((const char*)(argv[0]), (char **)NULL, 16);
		memwatch(addr, 0x7);

	} else {
		prom_printf("Wrong argument number!\r\n");
		return 1;
	}
	return 0;
}

#endif


#ifdef CONFIG_IIS_TEST
#define rtlRegRead(addr)        \
        (*(volatile u32 *)addr)

#define rtlRegWrite(addr, val)  \
        ((*(volatile u32 *)addr) = (val))

static inline u32 rtlRegMask(u32 addr, u32 mask, u32 value)
{
	u32 reg;

	reg = rtlRegRead(addr);
	reg &= ~mask;
	reg |= value & mask;
	rtlRegWrite(addr, reg);
	reg = rtlRegRead(addr); /* flush write to the hardware */

	return reg;
}
#define IIS_CODEC_ALC5621 1

int TestCmd_I2C( int argc, char* argv[])
{
#if defined(IIS_CODEC_ALC5621)
	unsigned int read_write;
	unsigned int register_addr;
	unsigned int register_value;
	unsigned int tmp;

	static unsigned int init_vari=0;

	if(init_vari==0)
	{
		rtlRegMask(0xb8003014, 0x00000F00, 0x00000200);//route iis interrupt
		rtlRegMask(0xb8000010, 0x03DCB000, 0x01DCB000);//enable iis controller clock
		rtlRegMask(0xb8000058, 0x00000001, 0x00000001);//enable 24p576mHz clock
#if 1
		rtlRegMask(0xb8000040, 0x00000007, 0x00000003);//change pin mux to iis-voice pin
		rtlRegMask(0xb8000044, 0x001F80DB, 0x00000049);//change pin mux to iis-voice pin
#endif
		for (tmp=0 ; tmp<5000 ; tmp++);
		init_i2c_gpio();
		init_vari=1;
	}

	read_write = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	register_addr = strtoul((const char*)(argv[1]), (char **)NULL, 16);


	if(read_write==0){

		tmp=ALC5621_fake_read(register_addr);
		//prom_printf( "\n%x\n",tmp);
	}else if(read_write==1){
		register_value = strtoul((const char*)(argv[2]), (char **)NULL, 16);
		write_ALC5621(register_addr, register_value);
	}

	if(0)
		ALC5621_init();
#endif
}

//[2:0] setting channel 0~5
//
extern int32_t play_channel;
int TestCmd_IISSETTING( int argc, char* argv[])
{
	int temp;
	temp=strtoul((const char*)(argv[0]), (char **)NULL, 16);
	play_channel=temp;
	prom_printf("[c%d]", play_channel);

}
extern volatile int iis_isr_test_flag;
//iis config
//setting[16:14], 0'b000->8k, 0'b001->16k, 0'b010->24k, 0'b011->32k, 0'b101->48k, sampling_rate
//setting[10:1], iiscr config
//setting[10]DACLRSWAP: 0-> left phase, 1-> right phase.
//setting[9:8]FORMAT: 00-> I2S, 01->Left Justified, 10->Right Justified
//setting[7]LOOP_BACK: 0->disable, 1-> enable loop back
//setting[6]WL: 0-> 16bits, 1-> 24bits.
//setting[5]EDGE_SW: 0->negative edge, 1->positive edge
//setting[4:3]Audio_Mono: 00->stereo audio, 01->5.1 audio, 10->mono
//setting[2:1]TX_ACT: 00->RX_PATH, 01->TX_PATH, 10->TX_RX_PATH (not involve 5.1 audio)
// setting = 0x  92 -> 8k,left,i2s,enable loopback,16bit,negative edge,mono,TX
// setting = 0x4092 -> 16k,left,i2s,enable loopback,16bit,negative edge,mono,TX
// setting = 0x  14 -> 8k,left,i2s,disable loopback,16bit,negative edge,mono,TX_RX
// setting = 0x4014 -> 16k,left,i2s,disable loopback,16bit,negative edge,mono,TX_RX
// setting = 0x 414 -> 8k,right,i2s,disable loopback,16bit,negative edge,mono,TX_RX
// setting = 0x14004 -> 48k,left,i2s,disable loopback,16bit,negative edge,2ch,TX_RX
// setting = 0x  82 -> 8k,left,i2s,enable loopback,16bit,negative edge,2ch,TX
int TestCmd_IIS( int argc, char* argv[])
{
	unsigned int mode;
	static unsigned int init_vari=0;
	unsigned int tmp;

	if(init_vari==0)
	{
		rtlRegMask(0xb8003014, 0x00000F00, 0x00000200);//route iis interrupt
		rtlRegMask(0xb8000010, 0x03DCB000, 0x01DCB000);//enable iis controller clock
		rtlRegMask(0xb8000058, 0x00000001, 0x00000001);//enable 24p576mHz clock
#if 1
		rtlRegMask(0xb8000040, 0x00000007, 0x00000003);//change pin mux to iis-voice pin
    #if 1
		rtlRegMask(0xb8000044, 0x001F80DB, 0x00000049);//change pin mux to iis-voice pin (led-sig0~2 as iis pin)
			//ew b8000044 3649
		//rtlRegMask(0xb8000044, 0x001F80DB, 0x00010000);//change pin mux to iis-voice pin (led-p1 as iis-voice pin)
			//ew b8000044 13600
		//rtlRegMask(0xb8000044, 0x001F80DB, 0x000D8000);//change pin mux to iis-voice pin (led-p1~2 as iis-audio pin)
			//ew b8000044 db600
    #endif
#endif
#if 0	//iis debug mode
		//rtlRegMask(0xb8000094, 0x00000FFF, 0x000000A8);//case A lexra 2 bus
		//rtlRegMask(0xb8000094, 0x00000FFF, 0x00000094);//case A
		//rtlRegMask(0xb8000094, 0x00000FFF, 0x00000090);//case new iis
		rtlRegMask(0xb8000094, 0x00000FFF, 0x00000030);//case C
		rtlRegMask(0xb8000040, 0x3FFF3F1F, 0x2aaa2a17);//change pin mux to debug
		rtlRegMask(0xb8000044, 0x001FB6DB, 0x0016a492);//change pin mux to debug
		
#endif

		for (tmp=0 ; tmp<5000 ; tmp++);
	}

#ifdef IIS_CODEC_ALC5621
#if 1
	if(init_vari==0)
	{
		init_i2c_gpio();
		init_vari=1;
	}

	//init_pcm(0);
	ALC5621_init(0);
#endif
#endif
	mode = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	init_iis(mode);
	iis_isr_test_flag=1;
	//while (1)
	while (0)
	{
		if(iis_isr_test_flag==0)
			break;

	}

	return 0;
}

//#include "pcm/fpga_gpio.h"
int TestCmd_GPIO( int argc, char* argv[])
{
#if 0
#if 1
	static unsigned int init_vari=0;
	static unsigned int data=0;
	if (init_vari==0) {
		init_vari=1;
		_rtl8954C_initGpioPin(GPIO_ID(GPIO_PORT_B, 0), GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
		_rtl8954C_initGpioPin(GPIO_ID(GPIO_PORT_B, 1), GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
		_rtl8954C_initGpioPin(GPIO_ID(GPIO_PORT_C, 1), GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
		_rtl8954C_initGpioPin(GPIO_ID(GPIO_PORT_C, 2), GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
		_rtl8954C_initGpioPin(GPIO_ID(GPIO_PORT_C, 3), GPIO_CONT_GPIO, GPIO_DIR_OUT, GPIO_INT_DISABLE);
	}
	data ^=1;

	_rtl8954C_setGpioDataBit(GPIO_ID(GPIO_PORT_B, 0), data);
	_rtl8954C_setGpioDataBit(GPIO_ID(GPIO_PORT_B, 1), data);
	_rtl8954C_setGpioDataBit(GPIO_ID(GPIO_PORT_C, 1), data);
	_rtl8954C_setGpioDataBit(GPIO_ID(GPIO_PORT_C, 2), data);
	_rtl8954C_setGpioDataBit(GPIO_ID(GPIO_PORT_C, 3), data);
#else

	unsigned int gpio_pin;

	unsigned int gpio_value;

	unsigned int gpio_id;

	static unsigned int init_vari=0;

	if(init_vari==0)
	{
		init_i2c_gpio();
		init_vari=1;
	}

	gpio_pin = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	gpio_value = strtoul((const char*)(argv[1]), (char **)NULL, 16);

	gpio_id = GPIO_ID(GPIO_PORT_F, gpio_pin);


	_rtl8954C_setGpioDataBit(gpio_id, gpio_value);

#endif
#endif
}


int TestCmd_GPIOR( int argc, char* argv[])
{
#if 0
	unsigned int gpio_pin;

	unsigned int gpio_value;

	unsigned int gpio_id;

	static unsigned int init_vari=0;

	if(init_vari==0)
	{
		init_i2c_gpio();
		init_vari=1;
	}

	gpio_pin = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	//gpio_value = strtoul((const char*)(argv[1]), (char **)NULL, 16);

	gpio_id = GPIO_ID(GPIO_PORT_D, gpio_pin);


	_rtl8954C_getGpioDataBit(gpio_id, &gpio_value);


	prom_printf("gpio_pin%d= %d",gpio_pin, gpio_value);
#endif
}

#endif


int TestCmd_MDIOW( int argc, char* argv[] )
{
	if(argc < 3) {
		printf("Parameters not enough!\n");
		return 1;
	}
	
	unsigned int phyid = strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	unsigned int reg = strtoul((const char*)(argv[1]), (char **)NULL, 10);		
	unsigned int data = strtoul((const char*)(argv[2]), (char **)NULL, 16);		

	dprintf("Write PhyID=0x%x Reg=%02d data=0x%x\r\n",phyid, reg,data);
	rtl8651_setAsicEthernetPHYReg(phyid,reg,data); 


}


int CmdPHYregR(int argc, char* argv[])
{
    unsigned long phyid, regnum;
    unsigned int uid,tmp;

    phyid = strtoul((const char*)(argv[0]), (char **)NULL, 16);
    regnum = strtoul((const char*)(argv[1]), (char **)NULL, 16);

    rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
    uid=tmp;
    prom_printf("PHYID=0x%x, regID=0x%x ,Find PHY Chip! UID=0x%x\r\n", phyid, regnum, uid);

}

int CmdPHYregW(int argc, char* argv[])
{
    unsigned long phyid, regnum;
    unsigned long data;
    unsigned int uid,tmp;

    phyid = strtoul((const char*)(argv[0]), (char **)NULL, 16);
    regnum = strtoul((const char*)(argv[1]), (char **)NULL, 16);
    data= strtoul((const char*)(argv[2]), (char **)NULL, 16);

    rtl8651_setAsicEthernetPHYReg( phyid, regnum, data );
    rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
    uid=tmp;
    prom_printf("PHYID=0x%x ,regID=0x%x, Find PHY Chip! UID=0x%x\r\n", phyid, regnum, uid);

}
#endif




#define MAX_SAMPLE  0x8000
//#define START_ADDR  0x100000               //1MB
#define START_ADDR  0x700000              //7MB, 0~7MB can't be tested
//#define END_ADDR      0x800000		//8MB
//#define END_ADDR      0x1000000         //16MB
//#define END_ADDR      0x2000000        //32MB
//#define END_ADDR      0x4000000       //64MB
//#define END_ADDR      0x8000000         //128MB      
#define BURST_COUNTS  256


 /*JSW: Auto set DRAM test range*/
   #if 1	//Auto set test range by DRAM size		
		   #ifdef CONFIG_D8_16
			#define END_ADDR 0x00800000 //8MB
		   #endif

		   #ifdef CONFIG_D16_16
			#define END_ADDR 0x01000000 //16MB
		   #endif

		    #ifdef CONFIG_D32_16
			#define END_ADDR 0x02000000 //32MB
		   #endif

		   #ifdef CONFIG_D64_16
			#define END_ADDR 0x04000000 //64MB
		   #endif	

		   #ifdef CONFIG_D128_16
			#define END_ADDR 0x08000000 //128MB
		   #endif	

		   #ifdef CONFIG_D8_16x2
			#define END_ADDR 0x01000000 //8MBx2
		   #endif

		    #ifdef CONFIG_D16_16x2
			#define END_ADDR 0x02000000 //16MBx2
		   #endif

		    #ifdef CONFIG_D32_16x2
			#define END_ADDR 0x04000000 //32MBx2
		   #endif

		    #ifdef CONFIG_D64_16x2
			#define END_ADDR 0x08000000 //64MBx2
		   #endif	

		    #ifdef CONFIG_D128_16x2
			#define END_ADDR 0x10000000 //128MBx2
		   #endif	
		   
    #endif



#ifdef CONFIG_DRAM_TEST
void Dram_test(int argc, char* argv[])
{
    unsigned int i, j,k,k2=0;
    unsigned int cache_type=0;
    unsigned int access_type=0;
    unsigned int addr;
    unsigned int burst=0;
    unsigned long int wdata;
    unsigned int samples,test_range;

    unsigned int enable_delay,delay_time,PM_MODE;//JSW:For DRAM Power Management
       
    unsigned int wdata_array[BURST_COUNTS];         //JSW:It must equal to Burst size
    
   

    /*JSW: Auto set DRAM test range*/
    /*
    //unsigned int END_ADDR;
    if (REG32(MCR_REG)==0x52080000)//8MB
	END_ADDR=0x800000;
    else if ((REG32(MCR_REG)==0x5a080000) ||(REG32(MCR_REG)==0x52480000)) //16MB
       END_ADDR=0x1000000;
    else if ((REG32(MCR_REG)==0x5a480000) ||(REG32(MCR_REG)==0x54480000)) //32MB
       END_ADDR=0x2000000;
    else if ((REG32(MCR_REG)==0x5c480000) ||(REG32(MCR_REG)==0x54880000)) //64MB
       END_ADDR=0x4000000;
    else if(REG32(MCR_REG)==0x5c880000)  //128MB
       END_ADDR=0x4000000;
    */


    unsigned int keep_W_R_mode;
   
    //prom_printf("argc=%d\n",argc);

	
	if(argc<=1)
	{	 		
		prom_printf("ex:dramtest <R/W> <enable_random_delay> <PowerManagementMode>\r\n");
	       prom_printf("<R/W>:<0>=R+W, <1>=R,<2>=W\r\n");
		prom_printf("<enable_random_delay>: <0>=Disable, <1>=Enable\r\n");
		prom_printf("<PowerManagementMode> : <0>=Normal, <1>=PowerDown, <2>=Self Refresh\r\n");
		prom_printf("    <3>:Reserved,<4>:CPUSleep + Self Refresh in IMEM   \r\n"); 				
		return;	
	}

	 keep_W_R_mode= strtoul((const char*)(argv[0]), (char **)NULL, 16);
        enable_delay= strtoul((const char*)(argv[1]), (char **)NULL, 16);
	 PM_MODE= strtoul((const char*)(argv[2]), (char **)NULL, 16);

    while(1)
    {

      

#if 1                                     //RTL8196_208PIN_SUPPORT_DDR
        prom_printf("\n================================\n");
        k2++;
        prom_printf("\nBegin DRAM Test : %d\n",k2);
        prom_printf("Dram Test parameter:\n" );
	 prom_printf("0.CLK_MANAGE(0xb8000010)=%x\n",READ_MEM32(CLK_MANAGE) );
	 //prom_printf("0.PAD_CONTROL(0xb8000048)=%x\n",READ_MEM32(PAD_CONTROL_REG) );
	 prom_printf("0.PAD_CONTROL(0xb8000048)=%x\n",READ_MEM32(0xb8000048) );
        prom_printf("1.DDCR(0xb8001050)=%x\n",READ_MEM32(DDCR_REG) );
        prom_printf("2.DTR(0xb8001008)=%x\n",READ_MEM32(DTR_REG) );
        prom_printf("3.DCR(0xb8001004)=%x\n",READ_MEM32(DCR_REG) );
       // prom_printf("4.DRAM Freq=%d MHZ\n",check_dram_freq_reg());
        prom_printf("5.Burst Size=%d \n",burst);
        prom_printf("6.cache_type(0:cache)(1:Un-cache)=%d \n",cache_type);
        prom_printf("7.Access_type(0:8bit)(1:16bit)(2:32bit)=%d \n",access_type);
	 prom_printf("8.Tested size=0x%x \n",END_ADDR);        
        prom_printf("9.Tested addr =0x%x \n",addr);
	 prom_printf("10.Tested pattern=0x%x \n",wdata);
        prom_printf("\n===============================\n");
#endif

     

        for (samples = 0; samples < MAX_SAMPLE; samples++)
        {
            cache_type = rand2() % ((unsigned int) 2);

            access_type = rand2()  % ((unsigned int) 3);
            burst = rand2() % (unsigned int) BURST_COUNTS;

     

            addr = 0x80000000 + START_ADDR + (rand2() % (unsigned int) (END_ADDR - START_ADDR));

   
            addr = cache_type ? (addr | 0x20000000) : addr;
            wdata = rand2();

            if (access_type == 0)
            {
                wdata = wdata & 0xFF;
            }
            else if (access_type == 1)
            {
                addr = (addr) & 0xFFFFFFFE;
                wdata = wdata & 0xFFFF;
            }
            else
            {
                addr = (addr) & 0xFFFFFFFC;
            }

        /* Check if Exceed Limit */
            if ( ((addr + (burst << access_type)) & 0x1FFFFFFF) > END_ADDR)
            {
                burst = (END_ADDR - ((addr) & 0x1FFFFFFF)) >> access_type;
		  prom_printf("11.Exceed Limit,burst=%d \n", burst);
            }

#if 1
            if (samples % 100 == 0)
            {
                prom_printf("\nSamples: %d", samples);
		  
		
		 
		   #if 1 //JSW @20091106 :For DRAM Test + Power Management 
		 	if(enable_delay)
		 	{
			     delay_time=rand2() % ((unsigned int) 1000*1000);
			     prom_printf(" Delay_time=%d\n",delay_time);
			     for(k=0;k<=delay_time;k++); //delay_loop	

			    // CmdCPUSleepIMEM();
		 	}

			#if 1
		 	if(PM_MODE)
			{
				
			  //set bit[31:30]=0 for default "Normal Mode" and prevent unstable state transition
			  //  REG32(MPMR_REG)= 0x3FFFFFFF ;
			  REG32(MPMR_REG)= 0x040FFFFF ;

			    switch(PM_MODE)
			    {
			        case 0:
			            prom_printf("\nDRAM : Normal mode\n");
				     //return 0;
			            break;

			        case 1:
			            prom_printf("\nDRAM :Auto Power Down mode\n");
			            REG32(MPMR_REG)= READ_MEM32(MPMR_REG)|(0x1 <<30) ;

				    
			            break;

			        case 2:
			            prom_printf("\nDRAM : Set to Self Refresh mode\n");				    
			            REG32(MPMR_REG)|= (0x2 <<30) ;	
				     
				    
			            break;

			        case 3:
			            prom_printf("\nReserved!\n");			            
			            REG32(MPMR_REG)= 0x3FFFFFFF ;
				     //return 0;
			            break;

				case 4:
			            prom_printf("\nCPUSleep + Self Refresh in IMEM!\n");
				     CmdCPUSleepIMEM();
			            //return 0;
			            break;

			        default :
			            prom_printf("\nError Input,should be 0~4\n");
			            break;
			     }   //end of switch(PM_MODE)
		 	}//end of if(PM_MODE)	
		 	#endif
		 #endif
		 
            }//end of switch(PM_MODE)
#endif

     

        /* Prepare Write Data */
            for (i = 0; i < burst ; i++)
            {            
                    wdata = (unsigned int)(rand2());// ???

                if (access_type == 0)               //8 bit
                    wdata = wdata & 0xFF;
                else if (access_type == 1)          //16bit
                    wdata = wdata & 0xFFFF;

                wdata_array[i] = wdata;
     
            }            
        
            for (i = 0, j = addr; i < burst ; i++)
            {
                if (access_type == 0)
                    *(volatile unsigned char *) (j) = wdata_array[i];
                else if (access_type == 1)
                    *(volatile unsigned short *) (j) = wdata_array[i];
                else
                    *(volatile unsigned int *) (j) = wdata_array[i];

                j = j + (1 << access_type);
        //keep reading
                if (keep_W_R_mode==1)
                {
                	WRITE_MEM32(0xa0700000,0x5a5aa5a5);
                	prom_printf("\nkeep reading,reading addr(0xa0700000),value=0x5a5aa5a5\n");
                	keep_reading:
					READ_MEM32(0xa0700000);

					 goto keep_reading;
                }
            }

	  //keep writing
            if (keep_W_R_mode==2)
            {
            	  prom_printf("\nkeep writing,writing addr(0xa0800000)=0xa5a55a5a\n");
                keep_writing:
		  
                WRITE_MEM32(0xa0800000,0xa5a55a5a);
       
                goto keep_writing;
            }

        /* Read Verify */
            for (i = 0, j = addr; i < burst ; i++)
            {
                unsigned rdata;

                if (access_type == 0)
                {
                    rdata = *(volatile unsigned char *) (j);
                }
                else if (access_type == 1)
                {
                    rdata = *(volatile unsigned short *) (j);
                }
                else
                {
                    rdata = *(volatile unsigned int *) (j);
                }
        //prom_printf("\n==========In Read Verify========= \n");
        // prom_printf("\nrdata: %d\n", rdata);
        //prom_printf("\nwdata_array[i]: %d\n",wdata_array[i]);
        // prom_printf("\n==========End Read Verify========= \n");

                if (rdata != wdata_array[i])
                {
                    prom_printf("\nWrite Data Array: 0x%X", wdata_array[i]);

                    if (cache_type)
                        prom_printf("\n==> Uncached Access Address: 0x%X, Type: %d bit, Burst: %d",
                            addr, (access_type == 0) ? 8 : (access_type == 1) ? 16 : 32, burst);
                    else
                        prom_printf("\n==>   Cached Access Address: 0x%X, Type: %d bit, Burst: %d",
                            addr, (access_type == 0) ? 8 : (access_type == 1) ? 16 : 32, burst);

                    prom_printf("\n====> Verify Error! Addr: 0x%X = 0x%X, expected to be 0x%X\n", j, rdata, wdata_array[i]);

        //HaltLoop:
        //goto HaltLoop;
                    return 0;

                }

                j = j + (1 << access_type);

            }                                       //end of reading

        }

    }                                               //end while(1)
}



void Dram_trxdly_test(int argc, char* argv[])
{
	#if 0
	if(argc<=1)
	{	 		
		prom_printf("ex:Dram_trxdly_test <tx_shitf> <rx_shitf> \r\n");
	       // prom_printf("<R/W>:<0>=R+W, <1>=R,<2>=W\r\n");
		//prom_printf("<enable_random_delay>: <0>=Disable, <1>=Enable\r\n");
		//prom_printf("<PowerManagementMode> : <0>=Normal, <1>=PowerDown, <2>=Self Refresh\r\n");
		//prom_printf("    <3>:Reserved,<4>:CPUSleep + Self Refresh in IMEM   \r\n"); 				
		return;	
	}
	#endif

	 const unsigned int CLK_manage_reg=0xb8000010;
	 const unsigned int Test_CLK_manage_reg=0xb8000068;
	 const unsigned int RW_pattern1=0xa5a55a5a;
	 const unsigned int RW_pattern2=0x5a5aa5a5;
	 unsigned int RW_pattern0,check_RW_pattern;

	 unsigned int trx_delay_value;

	 unsigned short score1,score2,score3,score4;
	
	 unsigned short rx_start_bit=0;	
	 unsigned short tx_start_bit;

	 //unsigned short tx_shitf= strtoul((const char*)(argv[0]), (char **)NULL, 16);
	 //unsigned short rx_shitf= strtoul((const char*)(argv[1]), (char **)NULL, 16);

	 //#define RTL8196D 
	 #define RTL8198 

	 #ifdef RTL8196D
	 	unsigned short tx_shitf=4;
	 	unsigned short rx_shitf=4;
	#endif


	 #ifdef RTL8198
	 	unsigned short tx_shitf=5;
	 	unsigned short rx_shitf=5;
	#endif
	 	
	 	
       

	 unsigned short rx_end_bit=(rx_start_bit+rx_shitf)-1;
	 tx_start_bit=rx_end_bit+1;
	 unsigned short tx_end_bit=(tx_start_bit+tx_shitf)-1;

	 //prom_printf("\n\nRX bit-range from <%d> to <%d> \n",rx_start_bit,rx_end_bit);
	 //prom_printf("\nTX bit-range from <%d> to <%d> \n",tx_start_bit,tx_end_bit);

	 unsigned short i,j,k=0;
	 for(i=0;i<(1<<tx_shitf);i++)  //i:tx
	 {
		for(j=0;j<(1<<rx_shitf);j++)  //j:rx
		{
			
			trx_delay_value=(i<<tx_start_bit)+(j<<rx_start_bit);
			//(*(volatile unsigned int *)(CLK_manage_reg))=;
			//prom_printf("\n\nTest(%d),tx(%d)rx(%d)_delay_value= <0x%x> \n",k,i,j,trx_delay_value);
			
		

			//trx_delay_value|=((READ_MEM32(Test_CLK_manage_reg))&0xFFFFFC00);
			//REG32(Test_CLK_manage_reg)=trx_delay_value;
			trx_delay_value|=((READ_MEM32(CLK_manage_reg))&0xFFFFFC00);

			

			prom_printf("\nTest(%d),tx(%d)rx(%d)\n",k,i,j);
			//prom_printf("\nTest_CLK_manage_reg value= <0x%x> \n",READ_MEM32(Test_CLK_manage_reg));
			prom_printf("\nmanage_reg value= <0x%x> \n",READ_MEM32(CLK_manage_reg));
			k++;

			REG32(CLK_manage_reg)=trx_delay_value; //change trx_delay regisster


			#if 1
				if (k%2)
				{
					RW_pattern0=RW_pattern1;
					
				}
				else
				{
					RW_pattern0=RW_pattern2;
					
				}
				WRITE_MEM32(0x80700000,RW_pattern0);
				
				check_RW_pattern=(READ_MEM32(0x80700000));
				
				if(RW_pattern0==check_RW_pattern)
					prom_printf("\nPass,check_RW_pattern= <0x%x> \n\n\n",check_RW_pattern);	
				else
				{
					prom_printf("\nFail,check_RW_pattern= <0x%x> \n\n\n",check_RW_pattern);
					break;
				}
				
			#endif

			

			
		}
	 }

	 
}


#endif //end of CONFIG_DRAM_TEST


 // 1 sec won't sleep,5 secs will sleep
#define MPMR_REG 0xB8001040
#if 1//def CONFIG_CPUsleep_PowerManagement_TEST
int CmdCPUSleep(int argc, char* argv[])
{



   /*
            PM_MODE=0 , normal
            PM_MODE=1 , auto power down
            PM_MODE=2 , seld refresh
        */
    if( !argv[0])                                   //read
    {
        prom_printf("Usage: sleep <0~2>  \r\n");
	 prom_printf("sleep <0>:CPU sleep + DRAM Normal mode \r\n"); 
	 prom_printf("sleep <1>:CPU sleep + DRAM Power down  \r\n"); 
	 prom_printf("sleep <2>:CPU sleep + DRAM Self refresh  \r\n"); 
	 prom_printf("sleep <3>:Only DRAM Power down  \r\n"); 
	 prom_printf("sleep <4>:CPUSleep + Self Refresh in IMEM  \r\n"); 
        return;
    }   
#if 0//def timer_test
    unsigned short sleep_time = strtoul((const char*)(argv[0]), (char **)NULL, 16);
    prom_printf("About to sleep 0x%x secs\n",sleep_time );


    tc0_init(sleep_time);
        //tc1_init(sleep_time);
        //tc2_init(sleep_time);
        //tc3_init(sleep_time);


        // REG32(GIMR_REG) =  0x0;//Disable all interrupt

        //prom_printf("GIMR_REG = 0x%x\n",*(volatile unsigned int *)GIMR_REG);
    prom_printf("GIMR_REG = 0x%x\n",*(volatile unsigned int *)GIMR_REG);
    prom_printf("GISR_REG = 0x%x\n",*(volatile unsigned int *)GISR_REG);
    prom_printf("TCCNR_REG = 0x%x\n",*(volatile unsigned int *)TCCNR_REG);
    prom_printf("TCIR_REG = 0x%x\n",*(volatile unsigned int *)TCIR_REG);

    unsigned short  i;
    for (i=0;i<=100;i++)
        prom_printf("TC1CNT_REG = 0x%x\n",*(volatile unsigned int *)TC1CNT_REG);

    prom_printf("GIMR_REG = 0x%x\n",*(volatile unsigned int *)GIMR_REG);
    prom_printf("GISR_REG = 0x%x\n",*(volatile unsigned int *)GISR_REG);
    prom_printf("TCCNR_REG = 0x%x\n",*(volatile unsigned int *)TCCNR_REG);
    prom_printf("TCIR_REG = 0x%x\n",*(volatile unsigned int *)TCIR_REG);

        // Enter Sleep Mode
    prom_printf("\nCPU Enter Sleep Mode ,it will wake up after %x secs.....\n",sleep_time);
#endif

 unsigned short PM_MODE = strtoul((const char*)(argv[0]), (char **)NULL, 16);


if(PM_MODE)
{
	
	  //set bit[31:30]=0 for default "Normal Mode"
    REG32(MPMR_REG)= 0x3FFFFFFF ;

    switch(PM_MODE)
    {
        case 0:
            prom_printf("\nDRAM : Normal mode\n");
            break;

        case 1:
            prom_printf("\nDRAM :Auto Power Down mode\n");
            REG32(MPMR_REG)= READ_MEM32(MPMR_REG)|(0x1 <<30) ;
            break;

        case 2:
            prom_printf("\nDRAM : Self Refresh mode\n");
	     REG32(MPMR_REG)= 0x3FFFFFFF ;
	      delay_ms(1000);
            REG32(MPMR_REG)|= (0x2 <<30) ;
	    delay_ms(1000);
            REG32(MPMR_REG)|= (0x2 <<30) ;
			
            break;

        case 3:
            prom_printf("\nDRAM :Only Power Down mode!\n");
            REG32(MPMR_REG)= READ_MEM32(MPMR_REG)|(0x1 <<30) ;
            return 0;

	case 4:
            prom_printf("\nCPUSleep + Self Refresh in IMEM!\n");
	     CmdCPUSleepIMEM();
            
            break;

        default :
            prom_printf("\nError Input,should be 0~3\n");
            break;
    }

    //prom_printf("After setting, MPMR(0xB8001040)=%x\n",READ_MEM32(MPMR_REG) );
}                                                   //End of DRAMPM



   
     REG32(GIMR_REG)=0x0;
	 prom_printf("CPU Enter Sleep...\n");

	//cli();    
   
        //JSW: SLEEP
  
    __asm__ __volatile__ (    "sleep\n\t"   );    

      //JSW: Make sure CPU do sleep and below won't be printed
     delay_ms(1000);                         //delay 1.25 sec in 40MHZ(current OSC), 25/40=1.25 sec
        // After Counter Trigger interrupt
      prom_printf("Counter Trigger interrupt,CPU Leave Sleep...\n");

}



#define __IRAM_IN_865X      __attribute__ ((section(".iram-rtkwlan")))
#define __IRAM_FASTEXTDEV        __IRAM_IN_865X
__IRAM_FASTEXTDEV
void CmdCPUSleepIMEM()
{

	 
      //while( (*((volatile unsigned int *)(0xb8001050))& 0x40000000) != 0x40000000);

      //prom_printf("\nDRAM : Self Refresh mode IMEM01\n");

	     REG32(MPMR_REG)= 0x3FFFFFFF ;
	     delay_ms(1000);
            REG32(MPMR_REG)|= (0x2 <<30) ;
	     delay_ms(1000);
            REG32(MPMR_REG)|= (0x2 <<30) ;


   
     REG32(GIMR_REG)=0x0;
	//cli();    
   
        //JSW: SLEEP  
    __asm__ __volatile__ (    "sleep\n\t"   );    

      //JSW: Just make sure CPU do sleep and below won't be printed
     delay_ms(1000);                        
     prom_printf("Counter Trigger interrupt,CPI Leave Sleep...\n");

}



#endif  //end if CONFIG_CPUsleep_PowerManagement_TEST


#ifdef CONFIG_NAND_FLASH

int CmdNANDID(int argc, char* argv[])
{

    rtk_nand_read_id();

    rtk_nand_probe();
}


int CmdNANDBE(int argc, char* argv[])
{

    if(argc < 2)
    {
        prom_printf("Parameters not enough!\n");
        return 1;
    }

    unsigned int block_start_num= strtoul((const char*)(argv[0]), (char **)NULL, 16);
    unsigned int block_end_num= strtoul((const char*)(argv[1]), (char **)NULL, 16);

#if 1
    unsigned int NAND_Erase_Block_num,NAND_Erase_Block_times;
                 //NAND_Erase_Block_times=block_end_num-block_start_num;
    for (NAND_Erase_Block_num=block_start_num;NAND_Erase_Block_num<=block_end_num;NAND_Erase_Block_num++)
    {
#ifdef NAND_Flash_Small_Page_32MB_3cycles
        rtk_erase_block (NAND_Erase_Block_num*32);   //JSW 1block= 32(0x20) pages
#else
        rtk_erase_block (NAND_Erase_Block_num*64);   //JSW 1block= 64(0x40) pages
#endif

    }
#endif

                 //rtk_erase_block (page);
}


int CmdNANDR(int argc, char* argv[])
{

    if(argc < 3 )
    {
        prom_printf("Parameters not enough!\n");
        return 1;
    }

                 //rtk_nand_read_id();

    unsigned long flash_address= strtoul((const char*)(argv[0]), (char **)NULL, 16);
    unsigned char *image_addr = strtoul((const char*)(argv[1]), (char **)NULL, 16);
    unsigned int image_size= strtoul((const char*)(argv[2]), (char **)NULL, 16);

    prom_printf("Read NAND Flash from 0x%X to 0x%X with 0x%X bytes ?\n",flash_address,image_addr,image_size);
#if 0
    prom_printf("(Y)es , (N)o ? --> ");

    if (YesOrNo())
        if (rtk_read_ecc_page (flash_address, image_addr,image_size))
            prom_printf("Read NAND Flash Successed!\n");
    else
        prom_printf("Read NAND Flash Failed!\n");
    else
        prom_printf("Abort!\n");
#else
    rtk_read_ecc_page (flash_address, image_addr,image_size);
#endif

                 //void rtk_read_ecc_page (unsigned long flash_address, unsigned char *image_addr,
                 //	unsigned int image_size);

#if 0

    unsigned long src=image_addr;
    unsigned int len=image_size;
    unsigned i;
                 /*
                     if(argc<1)
                     {	dprintf("Wrong argument number!\r\n");
                 return;
                 }
                 */
    if(1)
        src = strtoul((const char*)(argv[0]), (char **)NULL, 16);
    else
        src = 0x80000000;

    while ( (src) & 0x03)
        src++;

    for(i=0; i< len ; i+=4,src+=16)
    {
        printf("%X:	%X	%X	%X	%X\n",
            src, *(unsigned long *)(src), *(unsigned long *)(src+4),
            *(unsigned long *)(src+8), *(unsigned long *)(src+12));
    }
#endif

}


int CmdNANDW(int argc, char* argv[])
{

    if(argc <3 )
    {
        prom_printf("Parameters not enough!\n");
        return 1;
    }

    unsigned long flash_address= strtoul((const char*)(argv[0]), (char **)NULL, 16);
    unsigned char *image_addr = strtoul((const char*)(argv[1]), (char **)NULL, 16);
    unsigned int image_size= strtoul((const char*)(argv[2]), (char **)NULL, 16);

    prom_printf("Program NAND flash from %X to %X with %X bytes ?\n",flash_address,image_addr,image_size);
#if 0
    prom_printf("(Y)es, (N)o->");
    if (YesOrNo())
        if (rtk_write_ecc_page (flash_address,image_addr, image_size))
            prom_printf("Write Nand Flash Successed!\n");
    else
        prom_printf("Write Nand Flash Failed!\n");
    else
        prom_printf("Abort!\n");
#else
    rtk_write_ecc_page (flash_address,image_addr, image_size);
#endif

}

#endif


void PatchFT2()
{
#if 0
 
 set total_code_list {0x5400 0x5440 0x54c0 0x5480 0x5580 0x55c0 0x5540 0x5500 0x5700 0x5740 0x57c0 0x5780 0x5680 0x56c0 0x5640 0x5600 0x5400}
 set port_list {0 2 3 4}
 set reg20 0xb20
 for {set i 0} {$i <= 16} {incr i} 
 { 
    set code_list [lrange $total_code_list 0 $i]
   foreach phyID $port_list 
   {
     #p4 to page 1
     mdcmdio_cmd write 4 31 0x1
     #enable force gary code
     mdcmdio_cmd write 4 20 [expr $reg20 + (0x1<<$phyID)]
     set value [mdcmdio_cmd read 4 20]
     puts "reg4 20 = $value"
     #per port to page 1
     mdcmdio_cmd write $phyID 31 0x1
     foreach gary_code $code_list 
     {
       mdcmdio_cmd write $phyID 19 $gary_code
       puts "$i $phyID $gary_code"
     }
     mdcmdio_cmd write $phyID 31 0x0
   }
  #dealy, TBD
  after 10
 }
#release force mode
 mdcmdio_cmd write 4 31 0x1
 mdcmdio_cmd write 4 20 0xb20 
 mdcmdio_cmd write 4 31 0x0
  
 
#endif
 #define mdcmdio_cmd_write rtl8651_setAsicEthernetPHYReg
 #define mdcmdio_cmd_read rtl8651_getAsicEthernetPHYReg
 
 unsigned int i,g;
 
 unsigned int total_code_list[]= {0x5400, 
        0x5440, 0x54c0, 0x5480,
        0x5580, 0x55c0, 0x5540, 0x5500, 
        0x5700, 0x5740, 0x57c0, 0x5780, 
        0x5680, 0x56c0, 0x5640, 0x5600, 
        0x5400};
 
 unsigned char port_list[]= {0, 2, 3, 4};
 unsigned int p;
 unsigned int value;
 
 unsigned int *code_list_x, *code_list_y;
 #define reg20 0xb20
  
 for( i=0; i<=16; i++) 
 { 
    code_list_x=&total_code_list[0];
  code_list_y=&total_code_list[i];
    
   for( p=0; p<sizeof(port_list)/sizeof(unsigned char) ; p++) 
   {
   unsigned int phyID=port_list[p];
   
     //#p4 to page 1
     mdcmdio_cmd_write( 4, 31, 0x1);
     //#enable force gary code
     mdcmdio_cmd_write( 4, 20,  reg20 + (0x1<<phyID)   );
     mdcmdio_cmd_read( 4, 20, &value );
     //dprintf( "reg4 20 = %x\n", value);
     
     //#per port to page 1
     mdcmdio_cmd_write( phyID, 31, 0x1 );
  
     for(g=0; g<= (code_list_y-code_list_x) ; g++) 
    {
     unsigned int gary_code=code_list_x[g];
       mdcmdio_cmd_write( phyID, 19, gary_code );
       dprintf( "i=%d phyid=%d gray_code=%x\n" ,i, phyID, gary_code);
     }
     mdcmdio_cmd_write( phyID, 31, 0x0 );
  }
  //#dealy, TBD
  __delay(10000);
 }
//#release force mode
 mdcmdio_cmd_write( 4, 31, 0x1 );
 mdcmdio_cmd_write( 4, 20, 0xb20); 
 mdcmdio_cmd_write( 4, 31, 0x0 );
  
  
 

}
int CmdPortP1Patch(int argc, char* argv[])
{
	PatchFT2();
}

 #if defined(CONFIG_PCIE_MODULE) 
int CmdTestPCIE(int argc, char* argv[])
{
	extern void TestMonitorEntry();
	TestMonitorEntry();		
};
#endif

#ifdef CONFIG_NIC_LOOPBACK
static int CmdSetLpbk(int argc, char* argv[])
{
	nic_loopback ^= 1;
	prom_printf("NIC loopback %s.\n", (nic_loopback) ? "enabled" : "disabled");
}
#endif



//=====================================================================
#if 1//def CONFIG_RTL8196E


//System register Table
#define SYS_BASE 0xb8000000
#define SYS_INT_STATUS (SYS_BASE +0x04)
#define SYS_HW_STRAP   (SYS_BASE +0x08)
#define SYS_BIST_CTRL   (SYS_BASE +0x14)
#define SYS_DRF_BIST_CTRL   (SYS_BASE +0x18)
#define SYS_BIST_OUT   (SYS_BASE +0x1c)
#define SYS_BIST_DONE   (SYS_BASE +0x20)
#define SYS_BIST_FAIL   (SYS_BASE +0x24)
#define SYS_DRF_BIST_DONE   (SYS_BASE +0x28)
#define SYS_DRF_BIST_FAIL   (SYS_BASE +0x2c)
#define SYS_PLL_REG   (SYS_BASE +0x30)



//hw strap register

#define CK_M2X_FREQ_SEL (0x7 <<10)
#define ST_CPU_FREQ_SEL (0xf<<13)

#define ST_FW_CPU_FREQDIV_SEL (0x1<<18) //new
#define ST_CK_CPU_FREQDIV_SEL (0x1<<19) //new

#define ST_CLKLX_FROM_CLKM (1<<21)
#define ST_CLKLX_FROM_HALFOC (1<<22)

#define ST_CLKOC_FROM_CLKM (1<<24)

#define CK_M2X_FREQ_SEL_OFFSET 10
#define ST_CPU_FREQ_SEL_OFFSET 13
#define ST_CPU_FREQDIV_SEL_OFFSET 18
#define ST_CLKLX_FROM_CLKM_OFFSET 21

#define SPEED_IRQ_NO 27  //PA0
#define SPEED_IRR_NO (SPEED_IRQ_NO/8)   //IRR3
#define SPEED_IRR_OFFSET ((SPEED_IRQ_NO-SPEED_IRR_NO*8)*4)   //12


#define GICR_BASE                           0xB8003000
#define GIMR_REG                                (0x000 + GICR_BASE)       /* Global interrupt mask */
#define GISR_REG                                (0x004 + GICR_BASE)       /* Global interrupt status */
#define IRR_REG                                 (0x008 + GICR_BASE)       /* Interrupt routing */
#define IRR1_REG                                (0x00C + GICR_BASE)       /* Interrupt routing */
#define IRR2_REG                                (0x010 + GICR_BASE)       /* Interrupt routing */
#define IRR3_REG                                (0x014 + GICR_BASE)       /* Interrupt routing */


static void SPEED_isr(void)
{
	
	unsigned int isr=REG32(GISR_REG);
	unsigned int cpu_status=REG32(SYS_INT_STATUS);
	
	//dprintf("=>CPU Wake-up interrupt happen! GISR=%08x \n", isr);

	if( (isr & (1<<SPEED_IRQ_NO))==0)   //check isr==1
	{	dprintf("Fail, ISR=%x bit %d is not 1\n", isr, SPEED_IRQ_NO);
		while(1) ;
	}

	if((cpu_status & (1<<1))==0)  //check source==1
	{	//dprintf("Fail, Source=%x bit %d is not 1 \n", cpu_status, 1);
		while(1) ;
	}
		
	REG32(SYS_INT_STATUS)=(1<<1);  //enable cpu wakeup interrupt mask
//	REG32(GISR_REG)=1<<SPEED_IRQ_NO;	//write to clear, but cannot clear


	REG32(GIMR_REG)= REG32(GIMR_REG) & ~(1<<SPEED_IRQ_NO);	//so, disable interrupt		
}

struct irqaction irq_SPEED = {SPEED_isr, (unsigned long)NULL, (unsigned long)SPEED_IRQ_NO,"SPEED", (void *)NULL, (struct irqaction *)NULL};   

//---------------------------------------------------------------------------


int SettingCPUClk(int clk_sel, int clk_div, int sync_oc)
{
	int clk_curr, clk_exp;	
	unsigned int old_clk_sel;
	unsigned int mask;
	unsigned int sysreg;



	//dprintf("\nInput : CLK_SEL=0x%x, DIV=0x%x, SYNC_OC=0x%x \n", clk_sel, clk_div, sync_oc);
	//dprintf("Want to chage to CPU clock %d\r\n",clk_curr/clk_div);

	
	//clk_curr = check_cpu_speed();
	//dprintf("Now CPU Speed=%d \n",clk_curr);	
	//----------------------------
	REG32(SYS_INT_STATUS)=(1<<1);  //enable cpu wakeup interrupt mask
	while(REG32(GISR_REG)&(1<<SPEED_IRQ_NO));  //wait speed bit to low.
	//-------------------------------
	

	mask=REG32(GIMR_REG);
	//open speed irq

	
	int irraddr=IRR_REG+SPEED_IRR_NO*4;		
	REG32(irraddr) = (REG32(irraddr) &~(0x0f<<SPEED_IRR_OFFSET)) | (3<<SPEED_IRR_OFFSET);		
	request_IRQ(SPEED_IRQ_NO, &irq_SPEED, NULL); 	

	//be seure open interrupt first.
	REG32(GIMR_REG)=(1<<SPEED_IRQ_NO);  //accept speed interrupt	
	//REG32(GIMR_REG)=(1<<NFBI_IRQ_NO) ;  //only accept NFBI to interrupt	
	//REG32(GIMR_REG)=(1<<NFBI_IRQ_NO) | (1<<SPEED_IRQ_NO);  //accept speed and NFBI to interrupt		

	

	//-------------
	sysreg=REG32(SYS_HW_STRAP);
	//dprintf("Read  SYS_HW_STRAP=%08x\r\n", sysreg);	
	old_clk_sel=(sysreg & ST_CPU_FREQ_SEL) >>ST_CPU_FREQ_SEL_OFFSET;

	sysreg&= ~(ST_FW_CPU_FREQDIV_SEL);
	sysreg&= ~(ST_CK_CPU_FREQDIV_SEL);	
	sysreg&= ~(ST_CPU_FREQ_SEL);
	//sysreg&= ~(ST_SYNC_OCP);

	sysreg|=  (clk_div & 0x03) <<ST_CPU_FREQDIV_SEL_OFFSET ;
	sysreg|=	 (clk_sel&0x0f)<<ST_CPU_FREQ_SEL_OFFSET ;
	//sysreg|=  (sync_oc&01)<<ST_SYNC_OCP_OFFSET;
	
	//dprintf("Write SYS_HW_STRAP=%08x \n", sysreg);
	REG32(SYS_HW_STRAP)=sysreg  ;
	//dprintf("Read  SYS_HW_STRAP=%08x \n", REG32(SYS_HW_STRAP));
	
	//--------------
	if(old_clk_sel != clk_sel)
	{
	
		REG32(GISR_REG)=0xffffffff;	
		//dprintf("before sleep, Read  SYS_HW_STRAP=%08x \n", REG32(SYS_HW_STRAP));	
		//dprintf("GISR=%08x \n",REG32(GISR_REG));
		//dprintf("GIMR=%08x \n",REG32(GIMR_REG));	

		#if 1			
			REG32(SYS_BIST_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_BIST_DONE)&(1<<0))==0)  ; //wait bit to 1, is mean lock ok	
		#endif
		
		__asm__ volatile("sleep");	
		__asm__ volatile("nop");


		#if 1
			REG32(SYS_BIST_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<0))==(1<<0)) ;  //wait bit to 0  unlock
		#endif

		//dprintf("GISR=%08x\r\n",REG32(GISR_REG));
		//dprintf("GIMR=%08x\r\n",REG32(GIMR_REG));		
	
		//dprintf("after  sleep, Read  SYS_HW_STRAP=%08x  \n", REG32(SYS_HW_STRAP));
	
	}

	//-----------------------
		REG32(GIMR_REG)=mask;



}
//---------------------------------------------------------------------------
#endif   // end of CONFIG_RTL8196E
