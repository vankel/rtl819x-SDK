
#include <linux/interrupt.h>
#include <asm/system.h>

#include "monitor.h"
#include "etherboot.h"
#include "nic.h"


#include <asm/mipsregs.h>	//wei add
#include <asm/cacheops.h>	//wei add

#if defined(CONFIG_SPI_FLASH) 
#include "spi_flash.h"
#endif

#include <rtl8196x/asicregs.h>        


#if defined(RTL8198)
#include <asm/rtl8198.h>
#endif

#ifdef CONFIG_RLX5181_TEST
#include "monitor_commands.h"
#include "monitor_commands.c"
#endif

#ifdef CONFIG_RTL8196E
#define SWITCH_CMD 0
#else
#define SWITCH_CMD 1
#endif

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
extern unsigned int	_end;

extern unsigned char 	ethfile[20];
extern struct arptable_t	arptable[MAX_ARP];
#define MAIN_PROMPT						"<RealTek>"
#define putchar(x)	serial_outc(x)
#define IPTOUL(a,b,c,d)	((a << 24)| (b << 16) | (c << 8) | d )

#ifdef CONFIG_NIC_LOOPBACK
int nic_loopback = 0;
#endif

#if 1 //wei add for 8198C
extern int CmdL2Disable( int argc, char* argv[] );
extern int CmdCPUCLK( int argc, char* argv[] );
extern int CmdCore1Wakeup( int argc, char* argv[] );
extern int Cmd_Test_TimerX(int argc, char* argv[]);
extern int GPHY_BIST(int argc, char* argv[]);  //wei add
extern int GPHY_DRF_BIST(int argc, char* argv[]);  //wei add

extern int Cmd_AllBistTest(int argc, char* argv[]);  //wei add

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


#define CONFIG_PCIE_MODULE 1
#ifdef CONFIG_PCIE_MODULE
extern int PCIE_Host_RESET(int argc, char *argv[]);
extern int PCIE_Host_Init(int argc, char *argv[]);
extern int Test_HostPCIE_DataLoopback(int argc, char *argv[]);
extern int PCIE_PowerDown(int argc, char *argv[]);
extern int HostPCIe_MDIORead(int argc, char* argv[]); 
extern int HostPCIe_MDIOWrite(int argc, char* argv[]); 
extern int PCIE_PHYLoop(int argc, char *argv[]);
extern int HostPCIe_TestINT(int argc, char *argv[]);
#endif



#ifdef CONFIG_SPI_FLASH
	int CmdSFlw(int argc, char* argv[]);
	int CmdFlr(int argc, char* argv[]);
	extern void auto_spi_memtest_8198(unsigned long DRAM_starting_addr, unsigned int spi_clock_div_num);
#endif

#if defined (CONFIG_NAND_FLASH) ||(CONFIG_NAND_FLASH_BOOTING)
int CmdNANDID(int argc, char* argv[]);
int CmdNANDBE(int argc, char* argv[]);
int CmdNAND_PIO_SINGLE_PAGE_READ(int argc, char* argv[]);
int CmdNAND_PIO_WRITE(int argc, char* argv[]);
int CmdNANDR(int argc, char* argv[]);
int CmdNANDW(int argc, char* argv[]);
int  CmdNANDBadBlockDetect(int argc, char* argv[]);
extern char* rtk_nand_read_id(void);
extern int rtk_nand_probe(void);
extern int rtk_erase_block (int page);                      // 1 block=64 page
extern int rtk_read_ecc_page (unsigned long flash_address, unsigned char *image_addr,
unsigned int image_size,char ecc_enable);
extern int rtk_write_ecc_page (unsigned long flash_address, unsigned char *image_addr,
unsigned int image_size);
#if defined(SUPPORT_TFTP_CLIENT)
int CmdTFTPC(int argc, char* argv[]);
int check_tftp_client_state();
#endif
#endif

//int CmdTimer(int argc, char* argv[]);
//int CmdMTC0SR(int argc, char* argv[]);  //wei add
//int CmdMFC0SR(int argc, char* argv[]);  //wei add
//int CmdTFTP(int argc, char* argv[]);  //wei add
#if defined(CONFIG_BOOT_DEBUG_ENABLE)
#endif

#ifdef CONFIG_IIS_TEST
int TestCmd_IIS( int argc, char* argv[]);
int TestCmd_IISSTOP( int argc, char* argv[]);
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
extern char _bootimg_start, _bootimg_end;
#endif




#ifdef  CONFIG_DRAM_TEST
	void Dram_test(int argc, char* argv[]);
#endif



#ifdef  CONFIG_SPI_TEST
	int CmdSTEST(int argc, char* argv[]);               //JSW: add for SPI/SDRAM auto-memory-test program
#endif


#ifdef CONFIG_CPUsleep_PowerManagement_TEST
	int CmdCPUSleep(int argc, char* argv[]);
	void CmdCPUSleepIMEM(void);
#endif



#if SWITCH_CMD
int TestCmd_MDIOR(int argc, char* argv[]);  //wei add
int TestCmd_MDIOW(int argc, char* argv[]);  //wei add
#endif

#ifndef CONFIG_RTL8196E
int CmdXModem(int argc, char* argv[]);  //wei add
#endif

void CmdEthStartup(int argc, char* argv[]);

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

#if defined(RTL8198)
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




//------------------------------------------------------------------------------
/********   caculate CPU clock   ************/
int check_cpu_speed(void);
void timer_init(unsigned long lexra_clock);
static void timer_interrupt(int num, void *ptr, struct pt_regs * reg);
struct irqaction irq_timer = {timer_interrupt, 0, 8, "timer", NULL, NULL};                                   
static volatile unsigned int jiffies=0;
static void timer_interrupt(int num, void *ptr, struct pt_regs * reg)
{
	//dprintf("jiff=%x\r\n",jiffies);
	//flush_WBcache();
	//rtl_outl(TCIR,rtl_inl(TCIR));
	REG32(TCIR_REG)|=(1<<29);

	//if(jiffies==0x80)
	//REG32(GIMR_REG)&= ~(1<<8);
	
	jiffies++;


}
volatile unsigned int get_timer_jiffies(void)
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

	#define TICK_100MS_FREQ  4    /* 10 Hz */
	#define TICK_10MS_FREQ  100  /* 100 Hz */
	#define TICK_1MS_FREQ   1000 /* 1K Hz */
	
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
    //REG32(IRR1_REG) = 0x00050004;  //uart:IRQ5,  time0:IRQ4
    REG32(IRR1_REG) = 0x02;  
   
    
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



//---------------------------------------------------------------------------
volatile unsigned long loops_per_jiffy = (1<<12);
#define LPS_PREC 8
#define HZ 100
#ifdef RTL8198
unsigned long loops_per_sec = 2490368 * HZ;	// @CPU 500MHz (this will be update in check_cpu_speed())
#else
unsigned long loops_per_sec = 0x1db000 * HZ;	// @CPU 390MHz, DDR 195 MHz (this will be update in check_cpu_speed())
#endif

int check_cpu_speed(void)
{

//#define jiffies REG32(0xb8000000)

	unsigned volatile long ticks, loopbit;
	int lps_precision = LPS_PREC;
      
  // RTL8198
  	request_IRQ(14, &irq_timer, NULL); 

	extern long glexra_clock;
//	printf("timer init\n");
    timer_init(glexra_clock);	

//return 999;

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
	//prom_printf("cpu run %d.%d MIPS\n", loops_per_jiffy/(500000/HZ),      (loops_per_jiffy/(5000/HZ)) % 100);
//	return ((loops_per_jiffy/(500000/HZ))+1);
	return ((loops_per_jiffy/(500000/HZ))+1)*2;	 //for 1074k 
	
}
//---------------------------------------------------------------------------


/*
---------------------------------------------------------------------------
;				Monitor
---------------------------------------------------------------------------
*/
extern char** GetArgv(const char* string);


//---------------------------------------------------------------------------------------


#ifdef WRAPPER




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
		printf("\nSPI Flash Burn OK!\n");
#if 0		
		if(memcpy(0xbd000000, start, length))
			printf("Verify Fail\n");
		else
			printf("Verify OK\n");		

#endif
	}	
	else 
	{
        	printf("Abort!\n");	
	}	
  }

#endif
#if defined (CONFIG_NAND_FLASH) ||(CONFIG_NAND_FLASH_BOOTING)
int CmdNWB(int argc, char* argv[])
{
                                                     //JSW check
    unsigned int  cnt=strtoul((const char*)(argv[0]), (char **)NULL, 16);
    char* start = &_bootimg_start;
    char* end  = &_bootimg_end;
    unsigned int length = end - start;

    prom_printf("NAND Flash #%d will write 0x%X length of embedded boot code from 0x%X to 0x%X\n", cnt,length, start, end);
    prom_printf("(Y)es, (N)o->");
    if (YesOrNo())
    {
        rtk_write_ecc_page (0,start,length);       
    }
    else
    {
        prom_printf("Abort!\n");
    }
}
#endif
#endif

#if defined(SUPPORT_TFTP_CLIENT)
unsigned int tftp_from_command = 0;
char tftpfilename[128];
char errmsg[512];
unsigned short errcode = 0;
unsigned int tftp_client_recvdone = 0;
extern int jump_to_test;
extern int retry_cnt;
extern volatile unsigned int last_sent_time;
int CmdTFTPC(int argc, char* argv[])
{
    if(argc != 2)
	{
		dprintf("[usage:] tftp <memroyaddress> <filename>\n");
		tftpd_entry(0);
		return 0;
	}
	unsigned int  address=strtoul((const char*)(argv[0]), (char **)NULL, 16);
	unsigned int len = 0;
	image_address = address;
	memset(tftpfilename,0,128);
	len = strlen(tftpfilename);
	if(len+1 > 128)
	{
		dprintf("filename too long\n");
		return 0;
	}
	memset(errmsg,0,512);
	errcode = 0;
    retry_cnt = 0;
    last_sent_time = 0;
	tftp_client_recvdone = 0;
    jump_to_test = 0;
	strcpy(tftpfilename,(char*)(argv[1]));
	tftpd_entry(1);
	int tickStart = 0;
	int ret = 0;

	tftp_from_command = 1;
	tickStart=get_timer_jiffies();
	do 
    {
		ret=pollingDownModeKeyword(ESC);
		if(ret == 1) break;
	}
	while (
    (!tftp_client_recvdone)&&
    (check_tftp_client_state() >= 0
	||(get_timer_jiffies() - tickStart) < 2000)//20s
	);

	if(!tftp_client_recvdone)
	{
        if(ret == 1)
            dprintf("cancel by user ESC\n");
        else
            dprintf("TFTP timeout\n");
	}
	tftpd_entry(0);
	retry_cnt = 0;
	tftp_from_command = 0;
	tftp_client_recvdone = 0;
    image_address = 0xa0500000;
	return 0;
}
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
	flush_cache(); 
	prom_printf("\nreboot.......\n");
	//REG32(0xb8003114)=0;  //disable timer interrupt
	//REG32(0xb8000010)&=~(1<<11);
	//	
	//REG32(0xbbdc0300)=0xFFFFFFFF;
	//REG32(0xbbdc0304)=0xFFFFFFFF;

#if 0	
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
#endif
	//flush_cache();
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

	if(argc < 1) 
	{
		printf("TFTP Load Addr 0x%x\n",image_address);
		return 1;
	}

	
	image_address= strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	printf("Set TFTP Load Addr 0x%x\n",image_address);
}

/*
--------------------------------------------------------------------------
Flash Utility
--------------------------------------------------------------------------
*/
#if defined(CONFIG_SPI_FLASH)
int CmdFli(int argc, char* argv[])
{
	initFlash();
}



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



//---------------------------------------------------------------------------
#endif //RTL8197B

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

#if !defined(CONFIG_BOOT_DEBUG_ENABLE)
extern char eth0_mac[6];
#endif


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
	return 0;
}

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

	return 0;
}

int CmdPHYregR(int argc, char* argv[])
{
    unsigned long phyid, regnum;
    unsigned int uid,tmp;

    phyid = strtoul((const char*)(argv[0]), (char **)NULL, 16);
    regnum = strtoul((const char*)(argv[1]), (char **)NULL, 16);

    rtl8651_getAsicEthernetPHYReg( phyid, regnum, &tmp );
    uid=tmp;
    dprintf("PHYID=0x%x, regID=0x%x, data=0x%x\r\n", phyid, regnum, uid);
	return 0;
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
    dprintf("PHYID=0x%x ,regID=0x%x, read back data=0x%x\r\n", phyid, regnum, uid);
	return 0;
}

int CmdPhyPageRegR(int argc, char* argv[])
{
    unsigned long phyid, regnum, page;
    unsigned int uid;

    phyid = strtoul((const char*)(argv[0]), (char **)NULL, 16);
    page = strtoul((const char*)(argv[1]), (char **)NULL, 16);
    regnum = strtoul((const char*)(argv[2]), (char **)NULL, 16);

	if (phyid == 0) phyid = 8;
	if(page > 0)
		rtl8651_setAsicEthernetPHYReg( phyid, 31, page  );
	
    rtl8651_getAsicEthernetPHYReg( phyid, regnum, &uid );

	if(page > 0)
		rtl8651_setAsicEthernetPHYReg( phyid, 31, 0  );
	
    dprintf("PHYID=0x%x, page=0x%x, regID=0x%x, data=0x%x\r\n", phyid, page, regnum, uid);
	return 0;
}

int CmdPhyPageRegW(int argc, char* argv[])
{
    unsigned long phyid, regnum, page;
    unsigned long data;
    unsigned int uid;

    phyid = strtoul((const char*)(argv[0]), (char **)NULL, 16);
    page = strtoul((const char*)(argv[1]), (char **)NULL, 16);
    regnum = strtoul((const char*)(argv[2]), (char **)NULL, 16);
    data= strtoul((const char*)(argv[3]), (char **)NULL, 16);

	if (phyid == 0) phyid = 8;
	if(page > 0)
		rtl8651_setAsicEthernetPHYReg( phyid, 31, page  );

    rtl8651_setAsicEthernetPHYReg( phyid, regnum, data );
    rtl8651_getAsicEthernetPHYReg( phyid, regnum, &uid );

	if(page > 0)
		rtl8651_setAsicEthernetPHYReg( phyid, 31, 0  );

    dprintf("PHYID=0x%x, page=0x%x, regID=0x%x, read back data=0x%x\r\n", phyid, page, regnum, uid);
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
// config start.
//#define IIS_CODEC_ALC5621 1
#define SOC_TYPE_8881A	1
// config end.

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
		ALC5621_init();
#endif
}

extern void stop_iis(void);

int TestCmd_IISSTOP( int argc, char* argv[])
{
	stop_iis();
	return 0;
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
extern volatile int i2s_isr_test_flag;
extern void init_iis(unsigned int setting);
//iis config
//setting[30]44.1kHz: 0->48khz(24.576Mhz) 1->44.1khz(22.579Mhz)
//setting[16:14], 0'b000->8k, 0'b001->16k, 0'b010->24k, 0'b011->32k, 0'b101->48k, 0'b110->96k, sampling_rate
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
#ifdef SOC_TYPE_8881A
		//do 8881a soc iis part init
		rtlRegMask(0xb8003014, 0x00000F00, 0x00000200);//route iis interrupt
		rtlRegMask(0xb8000010, 0x02580000, 0x00580000);//enable iis controller clock select internal pll clk 24p576, active lx2 and lx2_arb
		rtlRegMask(0xb8000058, 0x00000801, 0x00000801);//enable 24p576mHz and 22p579mHz clock
		
		rtlRegMask(0xb8000040, 0x00000380, 0x00000280);//change pin mux to iis-voice pin (p0-mii as iis pin)
		rtlRegMask(0xb8000044, 0x000001ff, 0x00000049);//change pin mux to iis-voice pin (led-sig0~2 as iis pin)

		rtlRegMask(0xb800004c, 0x000FFFFF, 0x00033333);//change pin mux Configure JTAG PAD as IIS
#endif 
#if 0
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
	init_i2s(mode);
	i2s_isr_test_flag=1;
	//while (1)
	while (0)
	{
		if(i2s_isr_test_flag==0)
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

//==============================================================



void MxSpdupThanLexra()
{

	#define SYS_BASE 0xb8000000
	#define SYS_INT_STATUS (SYS_BASE +0x04)
	#define SYS_HW_STRAP   (SYS_BASE +0x08)
	#define SYS_BIST_CTRL   (SYS_BASE +0x14)
	#define SYS_BIST_DONE   (SYS_BASE +0x20)


	//printf("MxSpdupThanLexra\n");

	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG5  0x1f
	unsigned char m2x_freq_sel=GET_BITVAL(REG32(SYS_HW_STRAP), 10, RANG5);
	

	if(m2x_freq_sel>= 0x0f)           // M2x > lexra=200M   
		printf("Mx clk > Lexra clk\n");
	else
		return ;

	//-------------------------
  	request_IRQ(8, &irq_timer, NULL); 


	extern long glexra_clock;
       timer_init(glexra_clock);	   //run 10msec
	//--------------------------
	
	#define SYS_HS0_CTRL 0xb80000a0
	#define BIT(x)	(1 << x)	
	REG32(SYS_HS0_CTRL) |= BIT(0) | BIT(1) | BIT(2);   // LX0 > Mx clock
	
	
		#if 1			
			//printf("llx0\n");
			REG32(SYS_BIST_CTRL) |= (1<<2) ;	  //lock bus arb2
			while( (REG32(SYS_BIST_DONE)&(1<<0))==0)  {}; //wait bit to 1, is mean lock ok	

			//printf("llx1\n");
			//REG32(SYS_BIST_CTRL) |= (1<<3) ;	  //lock bus arb4
			//while( (REG32(SYS_BIST_DONE)&(1<<1))==0)  {}; //wait bit to 1, is mean lock ok		

			//printf("llx2\n");
			//REG32(SYS_BIST_CTRL) |= (1<<4) ;	  //lock bus arb6
			//while( (REG32(SYS_BIST_DONE)&(1<<2))==0)  {}; //wait bit to 1, is mean lock ok				
		#endif
		
	//	__asm__ volatile("sleep");	 //need 10 usec to guaretee
	//	__asm__ volatile("nop");


		#if 1
			//printf("ulx0\n");	
			REG32(SYS_BIST_CTRL) &= ~(1<<2);	//unlock
			while( (REG32(SYS_BIST_DONE)&(1<<0))==(1<<0)) {};  //wait bit to 0  unlock

			//printf("ulx1\n");
			//REG32(SYS_BIST_CTRL) &= ~(1<<3);	//unlock
			//while( (REG32(SYS_BIST_DONE)&(1<<1))==(1<<1)) {};  //wait bit to 0  unlock

			//printf("ulx2\n");
			//REG32(SYS_BIST_CTRL) &= ~(1<<4);	//unlock
			//while( (REG32(SYS_BIST_DONE)&(1<<2))==(1<<2)) {};  //wait bit to 0  unlock				
		#endif

			//printf("done\n");

}
//==============================================================

//------------------------------------------------------------------------
#ifndef CONFIG_RTL8196E
int CmdXModem(int argc, char* argv[])
{
	unsigned char *load_buf = (char*)0x80300000;
	unsigned int jump=0;
	//unsigned char *dest_buf = (char*)0xbd000000;

	if( argc < 1 ) 
	{
		dprintf("Usage: xmodem <buf_addr> [jump]\n");		
		return;	
	}
	load_buf = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	
	if(argc>1)	
	jump = strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	

	int len;
	len=xmodem_receive(load_buf);
		if(len!=0)
		{	printf("Rx len=%d \n", len);			
			return  len;			
		}
		else
			printf("Download failed!!\n");


	if(jump)
	{	
		void (*jumpF)(void);
		jumpF = (void *)(load_buf);
	
		REG32(GIMR_REG)=0; // mask all interrupt	    
		cli();
	
		flush_cache(); 
		prom_printf("\nJump to.......\n");

		jumpF();
	}
}; 
#endif
//==============================================================================
#define cache_op(op,addr)						\
	__asm__ __volatile__(						\
	"	.set	push					\n"	\
	"	.set	noreorder				\n"	\
	"	.set	mips3\n\t				\n"	\
	"	cache	%0, %1					\n"	\
	"	.set	pop					\n"	\
	:								\
	: "i" (op), "R" (*(unsigned char *)(addr)))


//============================================================================

int CmdTimerInit(int argc, char* argv[])
{
#if 0

  	request_IRQ(8, &irq_timer, NULL); 

	extern long glexra_clock;
	printf("=> init timer...\n");
    timer_init(glexra_clock*4);	

#if 0
	jiffies=0;
	int volatile j=jiffies;
	while(1)
	{
		if(j!=jiffies)
		{
			printf("j=%d\n", jiffies);
			j=jiffies;
		}
	}
#endif
	
#else
	int clk=check_cpu_speed();
	printf("CPU=%d MHz\n", clk);
#endif




}
//============================================================================

int CmdTest(int argc, char* argv[])
{

#if 0
	#define KSEG0BASE                 0x80000000
	#define CONFIG_SYS_CACHELINE_SIZE 32

	unsigned int t;
	t=0;
	//write_32bit_cp0_register_sel( 29, t, 2);  //CP0_DTagHi
	//write_32bit_cp0_register_sel( 28, t, 2);  //CP0_DTagLo

	
	unsigned long lsize = CONFIG_SYS_CACHELINE_SIZE;
	unsigned long addr = 0;//KSEG0BASE;
	unsigned long aend = addr+(32<<10)-lsize;   //DCACHE=32K

	int i;
//	while (1) 
	for(i=0; i<32; i++)
	{
		cache_op(Index_Load_Tag_D, addr);
		printf("Tag=%x:%x\n", read_32bit_cp0_register_sel(29, 2), read_32bit_cp0_register_sel(28, 2));
		
			
		if (addr >= aend)
			break;
		addr += lsize;
	}
#endif


#if 1
	int i,j,s,size,loop,st=0,ed=0;

	if( argc < 1 ) 
	{
		dprintf("Usage: test <len> <loop>\n");		
		return;	
	}
	size = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	loop = strtoul((const char*)(argv[1]), (char **)NULL, 16);



	flush_cache();
	//invalidate_cache();
#if 0	
	//disable L2
	s=(1<<12);
	write_32bit_cp0_register_sel(16, s, 2);
	printf("Disable L2 cache\n");
#else
	s=read_32bit_cp0_register_sel(16,  2);
	printf("L2 cache ByPass=%d\n", (s&(1<<12))>>12);
#endif

	for(j=0; j<loop; j++)
	{
		st=jiffies;
/*		
		for(i=0x80300000;i<0x80300000+size; i+=4)    //seq
		{
			REG32(i)=REG32(i);
		}
*/
		for(i=0x80300000;i<0x80300000+size; i+=32)   //cacheline
		{
			REG32(i)=REG32(i);
		}

		
		ed=jiffies;
		printf("loop=%d, st=%d, ed=%d, spend j=%d\n", j, st,ed,ed-st);		
	}

	
#endif

	
}

unsigned int rand2(void)
{
    static unsigned int x = 123456789;
    static unsigned int y = 362436;
    static unsigned int z = 521288629;
    static unsigned int c = 7654321;

    unsigned long long t, a= 698769069;

    x = 69069 * x + 12345;
    y ^= (y << 13); y ^= (y >> 17); y ^= (y << 5);
    t = a * z + c; c = (t >> 32); z = t;

    return x + y + z;
}

#ifdef CONFIG_DRAM_TEST
/* Function Name: 
 * 	_get_DRAM_csnum
 * Descripton:
 *	return DRAN total number of bytes.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	DRAM total byte number.
 */
unsigned int _get_DRAM_csnum(void)
{
    unsigned int dcr;

    dcr = *((unsigned int *)(DCR_REG));

    return (((dcr>>15)&1) + 1);

}


/* Function Name: 
 * 	memctlc_dram_size
 * Descripton:
 *	return DRAN total number of bytes.
 * Input:
 *	None
 * Output:
 * 	None
 * Return:
 *	DRAM total byte number.
 */
unsigned int memctlc_dram_size(void)
{
    unsigned int dcr;
    int total_bit = 0;


    dcr = *((unsigned int *)(DCR_REG));
    total_bit = 0;
    total_bit += ((dcr>>24)&0x3); //bus width
   // total_bit += ((dcr>>20)&0x3)+11; //row count
    total_bit += ((dcr>>20)&0xf)+11; //row count
    
   // total_bit += ((dcr>>16)&0x7)+8 ; //col count
    total_bit += ((dcr>>16)&0xf)+8 ; //col count

	
    total_bit += ((dcr>>28)&0x3)+1;  //bank count
    total_bit += (dcr>>15)&1;        //Dram Chip Select

    return ((1<<total_bit));


    //return(1<<total_bit);
}

#endif

#ifdef CONFIG_DRAM_TEST
    #define MAX_SAMPLE  0x8000
//#define START_ADDR  0x100000               //1MB
#define START_ADDR  0x700000              //7MB, 0~7MB can't be tested
//#define END_ADDR      0x800000		//8MB
//#define END_ADDR      0x1000000         //16MB
//#define END_ADDR      0x2000000        //32MB
//#define END_ADDR      0x4000000       //64MB
//#define END_ADDR      0x8000000         //128MB      
#define BURST_COUNTS  256

void Dram_test(int argc, char* argv[])
{
    unsigned int i, j,k,k2=0;
    unsigned int cache_type=0;
    unsigned int access_type=0;
    unsigned int Data_pattern=0;
     unsigned int random_test=1;
    unsigned int addr;
    unsigned int burst=0;
    unsigned long int wdata;
    unsigned int samples,test_range;

    unsigned int enable_delay,delay_time,PM_MODE;//JSW:For DRAM Power Management
       
    unsigned int wdata_array[BURST_COUNTS];         //JSW:It must equal to Burst size


	//#define RTL8198C_DRAM_TEST_GPIO_B1_PCIE_RSTN
    #ifdef RTL8198C_DRAM_TEST_GPIO_B1_PCIE_RSTN
	 #define GPIO_B1_1 REG32(PABCDDAT_REG)|=0x200     //Output "1"
	 #define GPIO_B1_0 REG32(PABCDDAT_REG)&=0xFFFFFDFF//Output "0"
	REG32(0xb8000104)|=(3<<20);//RTL8198C GPIO_B1 (PCIE_RSTN) trugger , pgin[0]/datg[0] , pin mux setting

	prom_printf("\nRTL8198C  FT2 GPIO init \n");

	
	RTL8196D_FT2_TEST_GPIOinit();   						  
     #endif
  

    /*JSW: Auto set DRAM test range*/
   
    unsigned int END_ADDR,DCR_VALUE;

#if 1 //For RTL8881A only
   END_ADDR= memctlc_dram_size()/_get_DRAM_csnum();

  if (END_ADDR==0x20000000)//512MB
  	END_ADDR/=2;
  

   prom_printf("Set dramtest size from DCR=0x%x \n",END_ADDR);       
#else
	//END_ADDR=0x00800000; //Test 8MB  

	END_ADDR=0x08000000; //Test 128MB  
	

	
#endif

    unsigned int keep_W_R_mode;
   
    	
	if(argc<7)
	{	 		
		prom_printf("ex:dramtest <1-R/W> <2-enable_random_delay> <3-PowerManagementMode><4-cache_type><5-bit_type><6-Data_Pattern><7-Random_mode>\r\n");
	       prom_printf("<1-R/W>:<0>=R+W, <1>=R,<2>=W\r\n");
		prom_printf("<2-enable_random_delay>: <0>=Disable, <1>=Enable\r\n");
		prom_printf("<3-PowerManagementMode> : <0>=Normal, <1>=PowerDown, <2>=Self Refresh\r\n");
		prom_printf("   		 <3>:Reserved,<4>:CPUSleep + Self Refresh in IMEM   \r\n"); 
		prom_printf("<4-cache_type>:<0>=cached, <1>=un-cached \r\n"); 
		prom_printf("<5-Access_type>:<0>=8bit, <1>=16bit , <2>=32bit \r\n");
		prom_printf("<6-Data_pattern>:<0>=random, <1>=sequential , <2>=0x5a5a5a5a \r\n");
		prom_printf("<7-Enable random_mode>:<0>=disable, <1>=enable  \r\n");
		
		
		return;	
	}

	 keep_W_R_mode= strtoul((const char*)(argv[0]), (char **)NULL, 16);
        enable_delay= strtoul((const char*)(argv[1]), (char **)NULL, 16);
	 PM_MODE= strtoul((const char*)(argv[2]), (char **)NULL, 16);
	 cache_type=strtoul((const char*)(argv[3]), (char **)NULL, 16);
	 access_type=strtoul((const char*)(argv[4]), (char **)NULL, 16);
	 Data_pattern=strtoul((const char*)(argv[5]), (char **)NULL, 16);
	 random_test=strtoul((const char*)(argv[6]), (char **)NULL, 16);
	 



	
    while(1)
    {

      

#if 1                                     //RTL8196_208PIN_SUPPORT_DDR
        prom_printf("\n================================\n");
        k2++;
        prom_printf("\nBegin DRAM Test : %d\n",k2);
        prom_printf("Dram Test parameter:\n" );
	 prom_printf("0.CLK_MANAGE(0xb8000010)=%x\n",READ_MEM32(CLK_MANAGE) );
	 //prom_printf("0.PAD_CONTROL(0xb8000048)=%x\n",READ_MEM32(PAD_CONTROL_REG) );
	 //prom_printf("0.PAD_CONTROL(0xb8000048)=%x\n",READ_MEM32(0xb8000048) );
        prom_printf("1.DIDER(0xb8001050)=%x\n",READ_MEM32(DDCR_REG) );
        prom_printf("2.DTR(0xb8001008)=%x\n",READ_MEM32(DTR_REG) );
        prom_printf("3.DCR(0xb8001004)=%x\n",READ_MEM32(DCR_REG) );
       // prom_printf("4.HS0_CONTROL(0x%x)=0x%x\n", HS0_CONTROL,REG32(HS0_CONTROL));
        prom_printf("5.Burst times=%d \n",burst);
        prom_printf("6.cache_type(0:cached)(1:Un-cached)=%d \n",cache_type);
        prom_printf("7.Access_type(0:8bit)(1:16bit)(2:32bit)=%d \n",access_type);
	 prom_printf("8.Tested size=0x%x \n",END_ADDR);        
        prom_printf("9.Tested addr =0x%x \n",addr);
	
#endif

       

        for (samples = 0; samples < MAX_SAMPLE; samples++)
        {
         #ifdef RTL8198C_DRAM_TEST_GPIO_B1_PCIE_RSTN
		  GPIO_B1_0;//PCIE_RSTN_output "1"
 	  #endif		
            if(random_test==1)
            {
            	       cache_type = rand2() % ((unsigned int) 2);
		       access_type = rand2()  % ((unsigned int) 3);            	      
            }	    
           

           // burst = rand2() % (unsigned int) BURST_COUNTS;	
            burst =BURST_COUNTS;	

     

            addr = 0x80000000 + START_ADDR + (rand2() % (unsigned int) (END_ADDR - START_ADDR));


   	     cache_type=1;//uncache

		 
            addr = cache_type ? (addr | 0x20000000) : addr;
            wdata = rand2();

	  
            if (access_type == 0)  //8 bit
            {
                wdata = wdata & 0xFF;
            }
            else if (access_type == 1) //16 bit
            {
                addr = (addr) & 0xFFFFFFFE;
                wdata = wdata & 0xFFFF;
            }
            else //32 bit
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
				    // CmdCPUSleepIMEM();
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
            		
            		if(Data_pattern==0)
            		{
                  		 wdata = (unsigned int)(rand2());/* Prepare random data */
            		}
			else if (Data_pattern==1)
				 wdata =( (i<<0)| (i<<8) |(i<<16) |(i<<23));  /* Prepare Sequential Data */   
			else if (Data_pattern==2)
				wdata=0x5a5aa5a5;//fixed data
			else
			{
				 prom_printf("\nError Data_pattern Input,return \n");
				 return;
			}
				
			

                if (access_type == 0)               //8 bit
                    wdata = wdata & 0xFF;
                else if (access_type == 1)          //16bit
                    wdata = wdata & 0xFFFF;

                wdata_array[i] = wdata;
     
            }            

	    
            for (i = 0, j = addr; i < burst ; i++)
            {
                if (access_type == 0)
                    *(volatile unsigned char *) (j) = wdata_array[i];//8bit
                else if (access_type == 1)
                    *(volatile unsigned short *) (j) = wdata_array[i];//16bit
                else
                    *(volatile unsigned int *) (j) = wdata_array[i];//32bit

                j = j + (1 << access_type);
        //keep reading
                if (keep_W_R_mode==1)
                {

		
				
                	 for (i = 0; i < burst ; i++)	
                	 {
                	 	if(Data_pattern==0)
            			{
            				WRITE_MEM32(0xa0700000+(i*4), rand2());                  			
            			}
				else if (Data_pattern==1)
					 /* Prepare Sequential Data */   
					WRITE_MEM32(0xa0700000+(i*4), ( (i<<0)| (i<<8) |(i<<16) |(i<<23)));					  
				else if (Data_pattern==2)
					WRITE_MEM32(0xa0700000+(i*4), wdata); 			

			 }
                		//WRITE_MEM32(0xa0700000+(i*4), 0x5aa5a55a);
					 
                	prom_printf("\nkeep reading\n");

			keep_reading:
			 for (i = 0; i < burst ; i++)	
			 {
			 		prom_printf("\naddr(0x%x),value=0x%x\n",0xa0700000+(i*4),REG32(0xa0700000+(i*4)));
					
			 }

					 goto keep_reading;
                }
            }

	  //keep writing
            if (keep_W_R_mode==2)
            {
            	  prom_printf("\nkeep writing,writing addr(0xa0800000)=0xa5a55a5a\n");
            	  prom_printf("\nkeep writing...\n");	
	
				
		 for (i = 0; i < burst ; i++)
		 {
			  wdata = rand2();
			 // wdata=0xa5a55a5a;
			 // wdata =( (i<<0)| (i<<8) |(i<<16) |(i<<23));  /* Prepare Sequential Data */  

			  #if 0
			  if (access_type == 0)               //8 bit
	                    wdata = wdata & 0xFF;
	                else if (access_type == 1)          //16bit
	                    wdata = wdata & 0xFFFF;
			#endif
			

               	 wdata_array[i] = wdata;				 
		 	  	
		 } 

		 keep_writing:	  
		  for (i = 0, j = addr; i < burst ; i++)
		 {
		  if (access_type == 0)
                    *(volatile unsigned char *)  (j) = wdata_array[i];//8bit
                else if (access_type == 1)
                    *(volatile unsigned short *)  (j)= wdata_array[i];//16bit
                else
                    *(volatile unsigned int *)  (j) = wdata_array[i];//32bit

		    j = j + (1 << access_type);
		 } 
              
       
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

		      #ifdef RTL8198C_DRAM_TEST_GPIO_B1_PCIE_RSTN
			GPIO_B1_1;//PCIE_RSTN_output "1" means fail
			  
		      #endif
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
#endif

//===============================================================================
//========================================================
int CmdPHY_Script(int argc, char* argv[])
{
/*
phy_begin
phyID 0
A 1
w 31 2 0 0x00
w 17 3 0 0x01
w 17 3 0 0x01
r 29 2 0
phy_end

*/
	#define WDBG 
	#define LEN 128
	unsigned char buffer[LEN];
	unsigned int phyid=0;   
    	unsigned int cnt=10;
	unsigned int len=0;

	unsigned char *ptr=0x80500000;
	unsigned int pidx=0;

	unsigned int rec=1;	

	WDBG("PHY begin...\n");

	
    while(1)
    {


	if(rec==1)
	{	
		memset(buffer,0,LEN);
		GetLine(buffer,LEN,1);
		printf("\n");
		len=strlen(buffer);
		
		memcpy(ptr+pidx,buffer, len); 
		ptr[pidx+len]=0x0a;
		pidx+=(len+1);
	}
	else
	{		
			ExtractACmdLine(ptr, buffer, 0);
	}

	
	WDBG("\n");
	argc = GetArgc( (const char *)buffer );
	argv = GetArgv( (const char *)buffer );


	StrUpr(argv[0]);
	WDBG("cmd=%s\n", argv[0] );
	
	if( (rec==1)  &&  !strcmp(argv[0],  "PHYID") )
	{
		phyid=strtoul((const char*)(argv[1]), (char **)NULL, 16);
		WDBG("id=%x\n", phyid);		
	}
	
	if(  (rec==1)  && !strcmp(argv[0],  "A") )
	{
		cnt=strtoul((const char*)(argv[1]), (char **)NULL, 10);
		WDBG("rep=%x\n", cnt);		
	}
	if(  (rec==0)  && !strcmp(argv[0],  "W") )
	{
		unsigned int reg,bit2,bit1,i,mask=0,val,tmp;
		reg=strtoul((const char*)(argv[1]), (char **)NULL, 10);
		bit2=strtoul((const char*)(argv[2]), (char **)NULL, 10);
		bit1=strtoul((const char*)(argv[3]), (char **)NULL, 10);
		val=strtoul((const char*)(argv[4]), (char **)NULL, 16);

		for(i=0; i<=(bit2-bit1); i++)
			mask= (mask<<1)|1;
		mask=0xffffffff-(mask<<bit1);
		
		rtl8651_getAsicEthernetPHYReg( phyid, reg, &tmp );	
		WDBG("mask=%x\n", mask);
		val=(tmp&mask)|val;
		WDBG("W reg=%x\n",val);		
		rtl8651_setAsicEthernetPHYReg( phyid, reg,  val);	

	}
	if(  (rec==0)  && !strcmp(argv[0],  "R") )
	{
		unsigned int reg,bit2,bit1,mask=0,i,val,tmp;
		reg=strtoul((const char*)(argv[1]), (char **)NULL, 10);
		bit2=strtoul((const char*)(argv[2]), (char **)NULL, 10);
		bit1=strtoul((const char*)(argv[3]), (char **)NULL, 10);		


		for(i=0; i<=(bit2-bit1); i++)
			mask= (mask<<1)|1;
		mask=mask<<bit1;
		WDBG("mask=%x\n", mask);
		
		rtl8651_getAsicEthernetPHYReg( phyid, reg, &tmp );	
		tmp=(tmp&mask)>>bit1;
		WDBG("val=%x\n",tmp);
		printf("0x%x\n", tmp);
	}	
	if( !strcmp(argv[0],  "PHY_END") )
	{	
		if(rec==1)
		{

			ExtractACmdLine(ptr, buffer, 1);
			rec=0;
		}
		else
		{
			ExtractACmdLine(ptr, buffer, 1);
			cnt--;
			if(cnt==0)
				break;
		}

	}	

	
    }

}

#ifdef CONFIG_NIC_LOOPBACK
static int CmdSetLpbk(int argc, char* argv[])
{
	nic_loopback ^= 1;
	prom_printf("NIC loopback %s.\n", (nic_loopback) ? "enabled" : "disabled");
}
#endif

//=========================================================

int CmdCPUCLK(int argc, char* argv[]);


#if defined(CONFIG_TFTP_COMMAND)
extern void autoreboot();
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
#if defined(CONFIG_TFTP_COMMAND)
	{ "REBOOT"  ,0, autoreboot, "reboot"											},
#endif





#ifdef CONFIG_SPI_FLASH
	{ "FLI"   ,3, CmdFli			, "FLI: Flash init"					},	

	{ "FLR"   ,3, CmdFlr			, "FLR: FLR <dst><src><length>"					},	
	{ "FLW",4, CmdSFlw, "FLW <dst_ROM_offset><src_RAM_addr><length_Byte> <SPI cnt#>: Write to SPI"},	 //JSW
#ifdef WRAPPER
	{ "SWB", 1, CmdSWB, "SWB <SPI cnt#> (<0>=1st_chip,<1>=2nd_chip): SPI Flash WriteBack (for MXIC/Spansion)"}, 	//JSW	
#endif	
#endif

#if defined (CONFIG_NAND_FLASH) ||(CONFIG_NAND_FLASH_BOOTING)
    { "NANDID",0, CmdNANDID, "NANDID: Read NAND Flash ID"},
    { "NANDBE",2, CmdNANDBE, "NANDBE:<Block start cnt><Block end cnt>"},
   
    { "NANDPIOR",3,  CmdNAND_PIO_SINGLE_PAGE_READ, "NANDPIOR:<flash_Paddress><enable_page_content><report_bad_block>"},
    { "NANDPIOW",3,  CmdNAND_PIO_WRITE, "NANDPIOW:<flash_Paddress><image_addr><image_size>"},
    { "NANDR",3, CmdNANDR, "NANDR:<flash_Paddress><image_addr><image_size><ECC_Enable>"},
    { "NANDW",3, CmdNANDW, "NANDW:<flash_Paddress><image_addr><image_size>"},
    { "NANDBBD",3, CmdNANDBadBlockDetect, "NANDBBD:<block_test_start_cnt><block_test_end_cnt>"},
    #ifdef WRAPPER	
	{"NWB", 1, CmdNWB, "NWB <NWB cnt#> (<0>=1st_chip,<1>=2nd_chip): NAND Flash WriteBack "},	
   #endif	
#endif
#if defined(SUPPORT_TFTP_CLIENT)
    {"TFTP", 2, CmdTFTPC, "tftp <memoryaddress> <filename>  "},
#endif
#if SWITCH_CMD
	{ "MDIOR"   ,0, TestCmd_MDIOR			, "MDIOR:  MDIOR phyid reg"				}, //wei add, 	
	{ "MDIOW"   ,0, TestCmd_MDIOW			, "MDIOW:  MDIOW phyid reg data"				}, //wei add, 	
	{ "PHYR",    2, CmdPHYregR, 			  "PHYR: PHYR <PHYID><reg>"},
	{ "PHYW",    3, CmdPHYregW, 			  "PHYW: PHYW <PHYID><reg><data>"},
	{ "PHYPR",   3, CmdPhyPageRegR, 		  "PHYPR: PHYPR <PHYID><page><reg>"},
	{ "PHYPW",   4, CmdPhyPageRegW, 		  "PHYPW: PHYPW <PHYID><page><reg><data>"},
#endif

#ifndef CONFIG_RTL8196E
	{ "XMOD"   ,1, CmdXModem			, "XMOD <addr>  [jump] "	}, 	//wei add	
#endif
	{ "TI"   ,1, CmdTimerInit			, "TI : timer init "	}, 	//wei add	

	{ "T"   ,1, CmdTest			, "T : test "	}, 	//wei add	

#ifdef  CONFIG_DRAM_TEST
	{ "DRAMTEST",7,Dram_test , "dramtest <1-R/W> <2-enable_random_delay> <3-PowerManagementMode><4-cache_type><5-bit_type><6-Data_Pattern><7-Random_mode>" },
#endif

#ifdef CONFIG_IIS_TEST
	{ "IIS"		,0, TestCmd_IIS			, "IIS"							},
	{ "IISSTOP"		,0, TestCmd_IISSTOP			, "IISSTOP"							},
	{ "IISSETTING"		,0, TestCmd_IISSETTING		, "IISSETTING mode"					},
	//{ "I2C"		,0, TestCmd_I2C			, "I2C read=0/write=1 register value"			},
	//{ "GPIO"	,0, TestCmd_GPIO		, "GPIO pin value"					},
	//{ "GPIOR"	,0, TestCmd_GPIOR		, "GPIOR pin"					},
#endif
#ifdef CONFIG_PCIE_MODULE
    {"HRST",  1, PCIE_Host_RESET,"HRST: Host Pcie Reset <portnum> <mdio_rst>: "},
    {"HINIT", 1, PCIE_Host_Init, "HINIT: Host init bar <portnum>"},
    {"HLOOP", 1, Test_HostPCIE_DataLoopback,"HLOOP: Test Pci-E data loopback <portnum> <cnt> "},
    {"EPDN",  1, PCIE_PowerDown, "EPDN: PCIE Power Down test <portnum><mode> "},
	{ "EMDIOR"   ,1, HostPCIe_MDIORead			, "EMDIOR: Reg Read <portnum>"},	
	{ "EMDIOW"   ,1, HostPCIe_MDIOWrite			, "EMDIOW <portnum> <reg> <val>:  "},    
    {"ELOOP", 1, PCIE_PHYLoop, "ELOOP <portnum> <start/stop>:  "},
    {"EINT",  1, HostPCIe_TestINT, "EINT <portnum> <loops>:  "},
#endif	
	{ "PHY_BEGIN"   ,1, CmdPHY_Script			, "PHY_BEGIN: "}, 	//wei add	

#ifdef CONFIG_NIC_LOOPBACK
	{ "LPBK",	0,	CmdSetLpbk,	"LPBK: NIC loopback enable/disable"},
#endif

	{ "ETH"   ,1, CmdEthStartup			, "ETH : startup Ethernet"	},


#if 1
//	{ "L2DIS"   ,1, CmdL2Disable,		 "L2DIS: L2 disable/enable"	},
	{ "CPUCLK"   ,1, CmdCPUCLK			, "CPUClk: "	},
	{ "C1WAKE"   ,1, CmdCore1Wakeup			, "C1Wake : Core 1 wake Up"	},
//	{ "TIMX"   ,1, Cmd_Test_TimerX			, "TIMX: TimerX : "	},	

	{ "GBIST"   ,1, GPHY_BIST			, "GBIST: GPHY BIST "	},
	{ "GDRF"   ,1, GPHY_DRF_BIST			, "GDRF: GPHY DRF BIST "	},
	
	{ "BISTALL"   ,1, Cmd_AllBistTest			, "BISTALL:  "	},	



#endif		
#ifdef CONFIG_RLX5181_TEST
#ifdef COMMANDS_TABLE_EX
	COMMANDS_TABLE_EX
#endif
#endif
};


//==============================================================================
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

//==============================================================================

#if defined(CONFIG_TFTP_COMMAND)
unsigned int maincmd_table_count = 0;
#define MAX_CMD_LEN 256
#endif
#ifdef CONFIG_NEW_CONSOLE_SUPPORT
extern void monitor_real(unsigned int table_count);
void monitor(void)
{
    unsigned int table_count = (sizeof(MainCmdTable) / sizeof(COMMAND_TABLE));
#if defined(CONFIG_TFTP_COMMAND)
	memset(image_address,0,MAX_CMD_LEN);
	maincmd_table_count = (sizeof(MainCmdTable) / sizeof(COMMAND_TABLE));
#endif
	monitor_real(table_count);
}
#else
void monitor(void)
{
	char		buffer[ MAX_MONITOR_BUFFER +1 ];
	int		argc ;
	char**		argv ;
	int		i, retval ;
	
//	i = &_end;
//	i = (i & (~4095)) + 4096;
	//printf("Free Mem Start=%X\n", i);
#if defined(CONFIG_TFTP_COMMAND)
	memset(image_address,0,MAX_CMD_LEN);
	maincmd_table_count = (sizeof(MainCmdTable) / sizeof(COMMAND_TABLE));
#endif
	while(1)
	{	
		 #if CONFIG_ESD_SUPPORT//patch for ESD
                         REG32(0xb800311c)|= (1<<23);
        	#endif
	
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
#endif

#if defined (CONFIG_NAND_FLASH) ||(CONFIG_NAND_FLASH_BOOTING)

int CmdNANDID(int argc, char* argv[])
{

    //rtk_nand_read_id();

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
    unsigned int NAND_Erase_Block_num,NAND_Erase_Block_times;

     prom_printf("NAND flash block erase from block:0x%X to block:0x%X ?\n",block_start_num,block_end_num);
    prom_printf("(Y)es, (N)o->");
   if (YesOrNo())
   {    

                 //NAND_Erase_Block_times=block_end_num-block_start_num;
    for (NAND_Erase_Block_num=block_start_num;NAND_Erase_Block_num<=block_end_num;NAND_Erase_Block_num++)
    {
	#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
      	  rtk_erase_block (NAND_Erase_Block_num*32);   //JSW 1block= 32(0x20) pages
	#endif	

	#if (defined(CONFIG_NAND_Flash_Large_Page_256MBto1GB_5cycles) || defined(CONFIG_NAND_Flash_Large_Page_128MB_4cycles))
		 rtk_erase_block (NAND_Erase_Block_num*64);   //JSW 1block= 64(0x40) pages
	#endif

	#if defined(CONFIG_NAND_Flash_Large_Page_256MBto2GB_5cycles)
		rtk_erase_block (NAND_Erase_Block_num*64);   //JSW 1block= 64(0x40) pages
	#endif
	#ifdef CONFIG_NAND_Flash_Large_Page_5cycles_Pages4KB
		rtk_erase_block (NAND_Erase_Block_num*128);   //JSW 1block= 128(0x80) pages
	#endif

	#ifdef CONFIG_NAND_Flash_Large_Page_5cycles_Pages8KB_4GB
		//rtk_erase_block (NAND_Erase_Block_num*128);   //JSW 1block= 128(0x80) pages
		rtk_erase_block (NAND_Erase_Block_num*256);   //JSW 1block= 256(0x100) pages
	#endif	

	#ifdef CONFIG_NAND_Flash_Large_Page_5cycles_Pages8KB_8GB
		rtk_erase_block (NAND_Erase_Block_num*256);   //JSW 1block= 256(0x100) pages
	#endif	


    }
   }
   else
   {
	 prom_printf("Abort!\n");
   }
               
}







int CmdNAND_PIO_SINGLE_PAGE_READ(int argc, char* argv[])
{
   if(argc< 3)
   {	 		
		prom_printf("ex:NANDPIOR:<flash_Paddress><enable_page_content>\r\n");
		prom_printf("<flash_Paddress>:NAND Flash's physical address\r\n");
		prom_printf("<enable_page_content>:<1:show page content><0:show nothing>\r\n");
		prom_printf("<report_bad_block>:<1:report bad block><0:report nothing>\r\n");
		
	     
		return;	
   }   
 

    unsigned int flash_Paddress_start= strtoul((const char*)(argv[0]), (char **)NULL, 16);
    unsigned int enable_page_content= strtoul((const char*)(argv[1]), (char **)NULL, 16);
    unsigned int report_bad_block= strtoul((const char*)(argv[2]), (char **)NULL, 16);
  
    //unsigned int length= strtoul((const char*)(argv[1]), (char **)NULL, 16);  
 

    prom_printf("NAND flash PIO read from flash_Paddress 0x%X \n",flash_Paddress_start);
   // prom_printf("(Y)es, (N)o->");
//   if (YesOrNo())
   //{                 
   //NAND_Erase_Block_times=block_end_num-block_start_num; 
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
       rtk_PIO_read_page (flash_Paddress_start,enable_page_content,report_bad_block);   //JSW 1block= 32(0x20) pages
#else
       rtk_PIO_read_page (flash_Paddress_start,enable_page_content,report_bad_block);   //JSW 1block= 64(0x40) pages
#endif    
//   }

               
}




int CmdNAND_PIO_WRITE(int argc, char* argv[])
{
   if(argc< 3)
   {	 		
		prom_printf("ex:CmdNAND_PIO_WRITE:<flash_Paddress><image_addr><image_size>\r\n");
		prom_printf("<flash_Paddress>:NAND Flash's physical address\r\n");
		prom_printf("<image_addr>:source data\r\n");
		prom_printf("<image_size>:data length\r\n");		
	     
		return;	
   }   
 

    unsigned int flash_Paddress_start= strtoul((const char*)(argv[0]), (char **)NULL, 16);
    unsigned int image_addr= strtoul((const char*)(argv[1]), (char **)NULL, 16);
    unsigned int image_size= strtoul((const char*)(argv[2]), (char **)NULL, 16);
  
    //unsigned int length= strtoul((const char*)(argv[1]), (char **)NULL, 16);  
 

    prom_printf("NAND flash PIO write size 0x%X from DRAM 0x%X to flash_Paddress 0x%X \n",image_size,image_addr,flash_Paddress_start);
    prom_printf("(Y)es, (N)o->");
   if (YesOrNo())
   {                 
   //NAND_Erase_Block_times=block_end_num-block_start_num; 
#if (defined(CONFIG_NAND_Flash_Small_Page_32MB_3cycles) || defined(CONFIG_NAND_Flash_Small_Page_64MB_4cycles))
       rtk_PIO_write_word (flash_Paddress_start,image_addr,image_size);   
#else
	   /*rtk_PIO_write_word seems can only write a page more safe,cl*/
	   int i;
	   for(i =0; i < (image_size)/2112;i++)
	   {
		   rtk_PIO_write_word (flash_Paddress_start,image_addr,2112); 
		   flash_Paddress_start += 2112;
		   image_addr += 2112;
	   }
#endif    
	}


               
}


int CmdNANDR(int argc, char* argv[])
{

    if(argc < 4 )
    {
        prom_printf("Parameters not enough!\n");
        return 1;
    }

                 //rtk_nand_read_id();

    unsigned long flash_address= strtoul((const char*)(argv[0]), (char **)NULL, 16);
    unsigned char *image_addr = strtoul((const char*)(argv[1]), (char **)NULL, 16);
    unsigned int image_size= strtoul((const char*)(argv[2]), (char **)NULL, 16);
    char ecc_enable=strtoul((const char*)(argv[3]), (char **)NULL, 16);

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
    rtk_read_ecc_page (flash_address, image_addr,image_size,ecc_enable);
#endif

                 //void rtk_read_ecc_page (unsigned long flash_address, unsigned char *image_addr,
                 //	unsigned int image_size);

#if 0

    unsigned long src=image_addr;
    unsigned int len=image_size;
    unsigned i;
                 /*
                     if(argc<1)
                     {	prom_printf("Wrong argument number!\r\n");
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
        prom_printf("%X:	%X	%X	%X	%X\n",
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

    prom_printf("Program NAND flash addr %X from %X with %X bytes ?\n",flash_address,image_addr,image_size);
    prom_printf("(Y)es, (N)o->");
   if (YesOrNo())
    {
        rtk_write_ecc_page (flash_address,image_addr, image_size);       
    }
    else
    {
        prom_printf("Abort!\n");
    }

}


int  CmdNANDBadBlockDetect(int argc, char* argv[])
{
	 if(argc < 2 )
    {
        prom_printf("Parameters not enough!\n");
        return 1;
    }

    //unsigned long flash_address= strtoul((const char*)(argv[0]), (char **)NULL, 16);
    //unsigned char *image_addr = strtoul((const char*)(argv[1]), (char **)NULL, 16);
    //unsigned int image_size= strtoul((const char*)(argv[2]), (char **)NULL, 16);

   unsigned int block_start_cnt= strtoul((const char*)(argv[0]), (char **)NULL, 16);

   unsigned int block_end_cnt= strtoul((const char*)(argv[1]), (char **)NULL, 16);

    prom_printf("NAND flash bad block detect from block:0x%X to block:0x%X ?\n",block_start_cnt,block_end_cnt);
    prom_printf("(Y)es, (N)o->");
   if (YesOrNo())
    {
        isBadBlock(block_start_cnt,block_end_cnt);     
    }
    else
    {
        prom_printf("Abort!\n");
    }

}

#endif
//---------------------------------------------------------------------------------------



