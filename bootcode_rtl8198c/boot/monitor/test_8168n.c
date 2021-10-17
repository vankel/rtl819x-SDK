#include "test_8168n.h"
#include <asm/rtl8196.h>
#include <linux/config.h>



#define TEST_8196_PCIE_P1 1
#define DBG 1
#define USING_INTERRUPT 1


#if USING_INTERRUPT
#include <linux/interrupt.h>
#endif
/*
 * Register Definitions
 */

/* GPIO */
#define GPIO_BASE       0xB8003500
#define PABCD_CNR       (GPIO_BASE + 0x00)
#define PABCD_DIR       (GPIO_BASE + 0x08)
#define PABCD_DAT       (GPIO_BASE + 0x0C)
#define PABCD_ISR       (GPIO_BASE + 0x10)
#define PAB_IMR         (GPIO_BASE + 0x14)
#define PCD_IMR         (GPIO_BASE + 0x18)


/*
 * Utility Macros
 */
#define htonl(A)                 ((((unsigned int)(A) & 0xff000000) >> 24) | \
                                  (((unsigned int)(A) & 0x00ff0000) >> 8)  | \
                                  (((unsigned int)(A) & 0x0000ff00) << 8)  | \
                                  (((unsigned int)(A) & 0x000000ff) << 24) )

#define WRITE_MEM32(addr, val)   (*(volatile unsigned int *) (addr)) = (val)
#define READ_MEM32(addr)         (*(volatile unsigned int *) (addr))
#define WRITE_MEM16(addr, val)   (*(volatile unsigned short *) (addr)) = (val)
#define READ_MEM16(addr)         (*(volatile unsigned short *) (addr))
#define WRITE_MEM8(addr, val)    (*(volatile unsigned char *) (addr)) = (val)
#define READ_MEM8(addr)          (*(volatile unsigned char *) (addr))


/*
 * Tunable Parameters
 */

#define DEBUG_PRINT   1

//===============================================================================
static __inline void *memset(void *s, int c, int size)
{
   unsigned char *__s = (unsigned char *) s;
   unsigned char __c = (unsigned char) c;

   while (size--)
      *__s++ = __c;

   return s;
}
//===============================================================================
static __inline void *memcpy(void *dest, void *src, int size)
{
   unsigned char *__d = (unsigned char *) dest;
   unsigned char *__s = (unsigned char *) src;

   while (size--)
      *__d++ = *__s++;

   return dest;
}
//===============================================================================
unsigned char *tx1_desc_addr, *rx1_desc_addr;
unsigned char *tx1_buff_addr, *rx1_buff_addr;

unsigned char *tx2_desc_addr, *rx2_desc_addr;
unsigned char *tx2_buff_addr, *rx2_buff_addr;



//===============================================================================
#if USING_INTERRUPT

#define PCIE_IRQ_NO 21 //Host PCIE0 bit 21
//#define PCIE_IRQ_NO 22 
#define PCIE_IRR_NO (PCIE_IRQ_NO/8)  // 2
#define PCIE_IRR_OFFSET ((PCIE_IRQ_NO-PCIE_IRR_NO*8)*4)   //bit 6 [27-24]


volatile unsigned int grxok=0;
volatile unsigned int gtxok=0;
static void PCIE_isr(void)
{
	unsigned int addr,portnum=0;
	if(portnum==0)
	{
		addr=PCIE1_EP_MEM;
	}
	else
	{
		addr=PCIE2_EP_MEM;
	}


//	printf("PCIE ISR\n");	
	unsigned int r=READ_MEM16(addr + IntrStatus);
	if((r==0) || (r==0xffff))
		return 0;
	
	printf("\n=>ISR=%x \n", r);

	if(r & RxOK)   // bit 0
	{
		printf("RxOK\n");	
		grxok=1;
	}
	if(r &RxErr)   // bit 1
	{
		printf("RxErr\n");	
	}
	if(r & TxOK)   //bit 2
	{
		printf("TxOK\n");	
		gtxok=1;
	}
	if(r &TxErr)  //bit 3
	{
		printf("TxErr\n");	
	}
	
	if(r & RxDescUnavail)  //bit 4
	{
		printf("RxDescUnavail\n");	
	}
	if(r &LinkChg)   //bit 5
	{
		printf("LinkChg\n");	
	}
	if(r &RxFIFOOver)   //bit 6
	{
		printf("RxFIFOOver\n");	
	}	
	if(r &TxDescUnavail)   //bit 7
	{
		printf("TxDescUnavail\n");	
	}	
/*	
	if(r &SWInt)   // bit 8
	{
		printf("SWInt\n");	
	}	
	
	if(r &PCSTimeout)
	{
		printf("PCSTimeout\n");	
	}	
	if(r &SYSErr)
	{
		printf("SYSErr\n");	
	}		
*/
	
	WRITE_MEM16(addr + IntrStatus, 0xffff);
}

#define REG32(reg)	(*(volatile unsigned int *)(reg))
#define NULL 0
struct irqaction irq_PCIE = {PCIE_isr, (unsigned long)NULL, (unsigned long)PCIE_IRQ_NO,"PCIE", (void *)NULL, (struct irqaction *)NULL};   


void EnIRQ(int portnum, int enableirq)
{
	unsigned int addr;
	if(portnum==0)
	{
		addr=PCIE1_EP_MEM;
	}
	else
	{
		addr=PCIE2_EP_MEM;
	}

	
	if(enableirq)
	{

		WRITE_MEM16(addr + IntrMask, 0xffff);

		
		int irraddr=IRR_REG+PCIE_IRR_NO*4;		
	  	REG32(irraddr) = (REG32(irraddr) &~(0x0f<<PCIE_IRR_OFFSET)) | (4<<PCIE_IRR_OFFSET);	
		request_IRQ(PCIE_IRQ_NO, &irq_PCIE, NULL); 
		dprintf("Enable PCIE ISR \n");
	}
	else
	{
		WRITE_MEM16(addr + IntrMask, 0);
		free_IRQ(PCIE_IRQ_NO);
		dprintf("Disable PCIE ISR\r\n");

	}
}

#endif

//===============================================================================
//unsigned short example();
unsigned short example(int portnum, int quietmode)     // 0:port 0, 1:port 1,  2:port 0 and port 1, rc=0, fail, rc=1 pass
{
	int en_loopback=1;
#if 0
//ARP protocol
	unsigned char tx_buffer[64]={0xff, 0xff, 0xff, 0xff, 0xff, 0xff,  //DMAC
							 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,  //SMAC
							 0x08, 0x06, //ARP
							 0x00, 0x01, //ETH
							//----------
							 0x08, 0x00, //IP Protocol
							 0x06,0x04,
							 0x00,0x01, //OPcode request
							 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,  //SMAC
							 0xc0, 0xa8,0x01,0x06,  //Sender IP:192.168.1.6
							 //-------------
							 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //Target MAC
							 0xc0, 0xa8, 0x01, 0x07,  //Target IP	:192.168.1.7	
							 //0x00,0x04,0x00,0x01,  //checksum
							 
							}; 
#endif
   /*
    * It seems for 8111C, first byte shall not be multicast.
    * Set 0x00 for safety
    */
   	
   static unsigned char *rx1_str = (unsigned char *) TX1_BUFF_ADDR;  
   static unsigned char *rx2_str = (unsigned char *) TX2_BUFF_ADDR;   


   int length1;
   int i,test_cnt;




//-----------------------------------------------
//giga NIC card:  811C  811D  limit len=4K   DeviceID=8168
//10,100M NIC : linit len=1500

   /* Get Random Data Length */
   #if PCIE_Test_With_8102E
          length1 = rand2() & 0x5ff; // limit to1535 ,OK for 8102E FT2
  	   //length1 = rand2() & 0x700; // limit to 1972 ,OK with 8102E
   #else
        length1 = rand2() & 0xfff; // limit to 64~4095
        //length1 = 1024 ; // fixed to 4092 ,error
	 //dprintf("Len=%x \n", length1);
   #endif   
   length1 = (length1 < 64) ? 64 : length1; 
//-----------------------------------------------

   /* Set MAC */
  if ( (portnum == 0) || (portnum== 2) )//PORT1
  {

   rx1_str[0] = 0x00;
   rx1_str[1] = 0x11;
   rx1_str[2] = 0x22;
   rx1_str[3] = 0x33;
   rx1_str[4] = 0x44;
   rx1_str[5] = 0x55;

   
  }
	
    #if (TEST_8196_PCIE_P1)
	  if ( (portnum == 1) || (portnum== 2) )//PORT1
	   {
	   	   //prom_printf("\nTest PCIE P1 \n");
		   rx2_str[0] = 0x00;
		   rx2_str[1] = 0x11;
		   rx2_str[2] = 0x22;
		   rx2_str[3] = 0x33;
		   rx2_str[4] = 0x44;
		   rx2_str[5] = 0x55;
	   }
   #endif
//-----------------------------------------------


   
   /* Set Sequential Data */   
   for (i = 6; i < length1; i++)  //wei add
	
   {
            rx1_str[i] = (i - 6) & 0xFF; //default
        //   rx1_str[i] = ((i - 6) & 0xFF)|0x10;//JSW 20090214: test for PCIE_MacLoopBack ,bit4 always 1

		 
	 #if (TEST_8196_PCIE_P1)
	  if ( (portnum == 1) || (portnum== 2) )//PORT1
	   {
     		 rx2_str[i] = (i - 6) & 0xFF;	
	   }
	#endif
   }



   //-------------------------------------------------------------------------------
   /*
    * READ ID Test:
    * Read 8111C Vendor/Device ID
    */
 
   if ( (portnum == 0) || (portnum== 2) )
   {
	  if ((READ_MEM32(PCIE1_EP_CFG) == 0x816810EC) |(READ_MEM32(PCIE1_EP_CFG) == 0x819210EC)|\
		(READ_MEM32(PCIE1_EP_CFG) == 0x813610EC) )//  //
	   {
	      // Successful 	     
	       #if DBG
		      dprintf("\n======================================\n");
		      dprintf("\nRead 8111/8192/8102 ID PASS !");	      
		      dprintf("\n=>PASS,PCIE P0's ID (0xb8b10000)=%x\n",READ_MEM32(PCIE1_EP_CFG)); 
		      dprintf("\n======================================\n");
		#endif
	   }
	   else
	   {

	      // Failed 
	     
	      #if DBG
		      dprintf("\nRead 8111/8192/8102 ID Fail ! \n");				
	      #endif
		  
		  if(quietmode==0)
		 dprintf("\n=>Fail,PCIE P0's ID (0xb8b10000)=%x\n",READ_MEM32(PCIE1_EP_CFG)); 	

			return 0;
	   }
   }
//---------------------------------------------------------------------------------------------

 //Auto-test PCIE Port1 by recognize Bond_Option	
 #if (TEST_8196_PCIE_P1)	
	if ( (portnum == 1) || (portnum== 2) )//PORT1
	 {
	   if ((READ_MEM32(PCIE2_EP_CFG) == 0x816810EC) |(READ_MEM32(PCIE2_EP_CFG) == 0x819210EC)|\
			(READ_MEM32(PCIE2_EP_CFG) == 0x813610EC) )//  //
	   {
	      // Successful 

	       #if DBG
		      dprintf("\n======================================\n");
		      dprintf("\nRead 8111/8192/8102 ID PASS !");		
		      dprintf("\n=>PASS,PCIE P1's ID (0xb8b30000)=%x\n",READ_MEM32(PCIE2_EP_CFG)); 	
		      dprintf("\n======================================\n");
		#endif	
	   }
	   else
	   {
	      // Failed 
	      
	       #if DBG
		      dprintf("\nRead 8111/8192/8102 ID Fail !\n");		     
		      dprintf("\n=>Fail,PCIE P1's ID (0xb8b30000)=%x\n",READ_MEM32(PCIE2_EP_CFG)); 	
			
	      #endif
			return 0;
	   }
	 }
#endif
//---------------------------------------------------------------------------------------------
		
	   /*	
	    * MAC Loopback Test:
	    * TX 1 Packet and then RX compare
	    */

	  /*"2"=test P1 and P0  , "1"=test P1 , "0"= test P0  */
	   rtl8168_init(portnum, en_loopback ); 
//-----------------------------------
#if 0 //for bokai
	length1=64;
      for (i = 0; i < 64; i++)  //wei add
            rx1_str[i] = tx_buffer[i]; 
            
#endif    	  
	   //-------------------------------------------------------------------	
	  if ( (portnum == 0) || (portnum== 2) )
	  {
	  	 rtl8168_tx(rx1_str, length1, portnum);
	  }
	   //-------------------------------------------------------------------
	    #if (TEST_8196_PCIE_P1)
		if ( (portnum == 1) || (portnum== 2) )
	   	  {
	 	  	rtl8168_tx(rx2_str, length1, portnum);
	   	  }
	   #endif
	   //-------------------------------------------------------------------
	   rtl8168_tx_trigger(portnum);
	   //-------------------------------------------------------------------
	  if ( (portnum == 0) || (portnum== 2) )
	   {
		   if (rtl8168_rx(rx1_str, length1, portnum) == 0)   //"0"==compare OK,"-1"=fail
		   {		      
			// if(quietmode==0)
			  dprintf("PCIE_P0 => PASS !\n");		
		   }
		   else
		   {		      
			 //if(quietmode==0)			      
    			      dprintf("PCIE_P0 => Fail ! \n");			

				return 0;
		   }
	   }	
	   //-------------------------------------------------------------------
	    #if (TEST_8196_PCIE_P1)		   	  
  		 if ( (portnum == 1) || (portnum== 2) )
	   	  {
			  if (rtl8168_rx(rx2_str, length1, portnum) == 0)   //"0"==compare OK,"-1"=fail
			   {		
			 	if(quietmode==0)			   
				 	dprintf("PCIE_P1 => PASS !\n");				
			   }
			   else
			   { 
			   	//if(quietmode==0)
					dprintf("PCIE_P1 => Fail !\n");					
					return 0;

			   }	   
	   	   }
	   #endif  		
	   //-------------------------------------------------------------------
   			

		  return 1;
	

	
}
//===============================================================================
void rtl8168_init(int portnum, int en_loopback)
{
   int i;

   if( (portnum == 0) || (portnum == 2) )
   {
      // 0. Set PCIE RootComplex
      WRITE_MEM32(PCIE1_RC_CFG + 0x04, 0x00100007);
      WRITE_MEM8(PCIE1_RC_CFG + 0x78, (READ_MEM8(PCIE1_EP_CFG + 0x78) & (~0xE0)) | MAX_PAYLOAD_SIZE_128B);  // Set MAX_PAYLOAD_SIZE to 128B,default
      //WRITE_MEM8(PCIE1_RC_CFG + 0x78, (READ_MEM8(PCIE1_EP_CFG + 0x78) & (~0xE0)) | MAX_PAYLOAD_SIZE_256B);  // Set MAX_PAYLOAD_SIZE to 256B

      // 1. Set 8111C EP
      WRITE_MEM32(PCIE1_EP_CFG + 0x04, 0x00180007);  // Mem, IO Enable
      WRITE_MEM32(PCIE1_EP_CFG + 0x10, (PCIE1_EP_IO | 0x00000001) & 0x1FFFFFFF);  // Set BAR
      WRITE_MEM32(PCIE1_EP_CFG + 0x18, (PCIE1_EP_MEM | 0x00000004) & 0x1FFFFFFF);  // Set BAR

      WRITE_MEM8(PCIE1_EP_CFG + 0x78, (READ_MEM8(PCIE1_EP_CFG + 0x78) & (~0xE0)) | MAX_PAYLOAD_SIZE_128B);  // Set MAX_PAYLOAD_SIZE to 128B
     // WRITE_MEM8(PCIE1_EP_CFG + 0x79, (READ_MEM8(PCIE1_EP_CFG + 0x79) & (~0x70)) | MAX_READ_REQSIZE_128B);  // Set MAX_REQ_SIZE to 128B 
      WRITE_MEM8(PCIE1_EP_CFG + 0x79, (READ_MEM8(PCIE1_EP_CFG + 0x79) & (~0x70)) | MAX_READ_REQSIZE_256B);  // Set MAX_REQ_SIZE to 256B,default

      // 2. Reset EP
      WRITE_MEM8(PCIE1_EP_MEM + ChipCmd, 0x10);

      // 3. Set MAC Loopback & Disable TX CRC & TxDMA Size
      if(en_loopback)      WRITE_MEM32(PCIE1_EP_MEM + TxConfig, (READ_MEM32(PCIE1_EP_MEM + TxConfig) & (~0x700)) | TxDMA1KB | TxMACLoopBack | (1 << 16));  //wei del
     else 				     WRITE_MEM32(PCIE1_EP_MEM + TxConfig, (READ_MEM32(PCIE1_EP_MEM + TxConfig) & (~0x700)) | TxDMA1KB |  (0 << 16));
	  
      // Enable Runt & Error Accept of RX Config
      WRITE_MEM32(PCIE1_EP_MEM + RxConfig, (READ_MEM32(PCIE1_EP_MEM + RxConfig) & (~0x700)) | RxDMA512B | AcceptErr | AcceptRunt | (1 << 7));

      // 4. Set TX/RX Desciptor Starting Address
      WRITE_MEM32(PCIE1_EP_MEM + TxDescStartAddrLow, PADDR(TX1_DESC_ADDR));
      WRITE_MEM32(PCIE1_EP_MEM + TxDescStartAddrHigh, 0);
      WRITE_MEM32(PCIE1_EP_MEM + RxDescAddrLow, PADDR(RX1_DESC_ADDR));
      WRITE_MEM32(PCIE1_EP_MEM + RxDescAddrHigh, 0);

      // 5. Set TX Ring - Descriptor Assigned to CPU
      memset((unsigned char *) TX1_DESC_ADDR, 0x0, NUM_TX_DESC * TX1_DESC_SIZE);

      for (i = 0; i < NUM_TX_DESC; i++)
      {
         if(i == (NUM_TX_DESC - 1))
            WRITE_MEM32(TX1_DESC_ADDR + TX1_DESC_SIZE * i, htonl(PADDR(RingEnd)));
      }

      // 6. Set RX Ring - Descriptor Assigned to NIC
      memset((unsigned char *) RX1_DESC_ADDR, 0x0, NUM_RX_DESC * RX1_DESC_SIZE);

      for (i = 0; i < NUM_RX_DESC; i++)
      {
         if(i == (NUM_RX_DESC - 1))
            WRITE_MEM32(RX1_DESC_ADDR + RX1_DESC_SIZE * i, htonl(DescOwn | RingEnd | RX1_BUFF_SIZE));
         else
            WRITE_MEM32(RX1_DESC_ADDR + RX1_DESC_SIZE * i, htonl(DescOwn | RX1_BUFF_SIZE));
      }

      tx1_desc_addr = (unsigned char *) TX1_DESC_ADDR;
      tx1_buff_addr = (unsigned char *) TX1_BUFF_ADDR;
      rx1_desc_addr = (unsigned char *) RX1_DESC_ADDR;
      rx1_buff_addr = (unsigned char *) RX1_BUFF_ADDR;
	  
#if USING_INTERRUPT
	EnIRQ(0, 1);
#endif
	  
   }

   //else  
	//------------------------------------------------------------------	
   
   #if (TEST_8196_PCIE_P1)
   if ( (portnum == 1) || (portnum== 2) )//PORT1
   {
      // 0. Set PCIE RootComplex
      WRITE_MEM32(PCIE2_RC_CFG + 0x04, 0x00100007);
      WRITE_MEM8(PCIE2_RC_CFG + 0x78, (READ_MEM8(PCIE2_EP_CFG + 0x78) & (~0xE0)) | MAX_PAYLOAD_SIZE_128B);  // Set MAX_PAYLOAD_SIZE to 128B

      // 1. Set 8111C EP
      WRITE_MEM32(PCIE2_EP_CFG + 0x04, 0x00180007);  // Mem, IO Enable
      WRITE_MEM32(PCIE2_EP_CFG + 0x10, (PCIE2_EP_IO | 0x00000001) & 0x1FFFFFFF);  // Set BAR
      WRITE_MEM32(PCIE2_EP_CFG + 0x18, (PCIE2_EP_MEM | 0x00000004) & 0x1FFFFFFF);  // Set BAR

      WRITE_MEM8(PCIE2_EP_CFG + 0x78, (READ_MEM8(PCIE2_EP_CFG + 0x78) & (~0xE0)) | MAX_PAYLOAD_SIZE_128B);  // Set MAX_PAYLOAD_SIZE to 128B
      WRITE_MEM8(PCIE2_EP_CFG + 0x79, (READ_MEM8(PCIE2_EP_CFG + 0x79) & (~0x70)) | MAX_READ_REQSIZE_256B);  // Set MAX_REQ_SIZE to 128B

      // 2. Reset EP
      WRITE_MEM8(PCIE2_EP_MEM + ChipCmd, 0x10);

      // 3. Set MAC Loopback & Disable TX CRC & TxDMA Size
     if(en_loopback)      	WRITE_MEM32(PCIE2_EP_MEM + TxConfig, (READ_MEM32(PCIE2_EP_MEM + TxConfig) & (~0x700)) | TxDMA1KB | TxMACLoopBack | (1 << 16));
	else  				WRITE_MEM32(PCIE2_EP_MEM + TxConfig, (READ_MEM32(PCIE2_EP_MEM + TxConfig) & (~0x700)) | TxDMA1KB |  (1 << 16));
	
      // Enable Runt & Error Accept of RX Config
      WRITE_MEM32(PCIE2_EP_MEM + RxConfig, (READ_MEM32(PCIE2_EP_MEM + RxConfig) & (~0x700)) | RxDMA512B | AcceptErr | AcceptRunt | (1 << 7));

      // 4. Set TX/RX Desciptor Starting Address
      WRITE_MEM32(PCIE2_EP_MEM + TxDescStartAddrLow, PADDR(TX2_DESC_ADDR));
      WRITE_MEM32(PCIE2_EP_MEM + TxDescStartAddrHigh, 0);
      WRITE_MEM32(PCIE2_EP_MEM + RxDescAddrLow, PADDR(RX2_DESC_ADDR));
      WRITE_MEM32(PCIE2_EP_MEM + RxDescAddrHigh, 0);

      // 5. Set TX Ring - Descriptor Assigned to CPU
      memset((unsigned char *) TX2_DESC_ADDR, 0x0, NUM_TX_DESC * TX2_DESC_SIZE);

      for (i = 0; i < NUM_TX_DESC; i++)
      {
         if(i == (NUM_TX_DESC - 1))
            WRITE_MEM32(TX2_DESC_ADDR + TX2_DESC_SIZE * i, htonl(PADDR(RingEnd)));
      }

      // 6. Set RX Ring - Descriptor Assigned to NIC
      memset((unsigned char *) RX2_DESC_ADDR, 0x0, NUM_RX_DESC * RX2_DESC_SIZE);

      for (i = 0; i < NUM_RX_DESC; i++)
      {
         if(i == (NUM_RX_DESC - 1))
            WRITE_MEM32(RX2_DESC_ADDR + RX2_DESC_SIZE * i, htonl(DescOwn | RingEnd | RX2_BUFF_SIZE));
         else
            WRITE_MEM32(RX2_DESC_ADDR + RX2_DESC_SIZE * i, htonl(DescOwn | RX2_BUFF_SIZE));
      }

      tx2_desc_addr = (unsigned char *) TX2_DESC_ADDR;
      tx2_buff_addr = (unsigned char *) TX2_BUFF_ADDR;
      rx2_desc_addr = (unsigned char *) RX2_DESC_ADDR;
      rx2_buff_addr = (unsigned char *) RX2_BUFF_ADDR;
   }
  #endif
	//------------------------------------------------------------------------   
}

//===============================================================================
void rtl8168_tx(unsigned char *content, unsigned int size, int portnum)
{
   unsigned int i;

   if ( (portnum == 0) || (portnum== 2) )
   {
      // Fill RX Descriptor
      WRITE_MEM32(RX1_DESC_ADDR + 0x08, htonl(PADDR(RX1_BUFF_ADDR)));  // RX Buffer Address
      WRITE_MEM32(RX1_DESC_ADDR + 0x18, htonl(PADDR(RX1_BUFF_ADDR)));  // RX Buffer Address

      // Ensure Descriptor is updated
      READ_MEM32(RX1_DESC_ADDR + 0x8);

      // Enable TX/RX (This seems to trigger NIC prefetching RX descriptor)
      WRITE_MEM8(PCIE1_EP_MEM + ChipCmd, 0x0C);

      #if 1
      // Fill Descriptor
      WRITE_MEM32(TX1_DESC_ADDR + 0x0, htonl(DescOwn | FirstFrag | LastFrag | RingEnd | size));
      WRITE_MEM32(TX1_DESC_ADDR + 0x8, htonl(PADDR((unsigned int) TX1_BUFF_ADDR)));

      // Ensure Descriptor is updated
      READ_MEM32(TX1_DESC_ADDR + 0x8);
      #else
      // Fill TX Descriptor, Buffer
      while (size > 0)
      {
         unsigned int buf_size = MIN(size, TX1_BUFF_SIZE);

         // Fill Buffer
         memcpy(tx1_buff_addr, content, buf_size);
         content += buf_size;
         size -= buf_size;

         // Fill Descriptor
         WRITE_MEM32(TX1_DESC_ADDR + 0x0, htonl(DescOwn | FirstFrag | LastFrag | RingEnd | buf_size));
         WRITE_MEM32(TX1_DESC_ADDR + 0x8, htonl(PADDR((unsigned int) TX1_BUFF_ADDR)));

         // Ensure Descriptor is updated
         READ_MEM32(TX1_DESC_ADDR + 0x8);

         tx1_desc_addr += TX1_DESC_SIZE;
         tx1_buff_addr += TX1_BUFF_SIZE;
      }
      #endif
   }

  #if (TEST_8196_PCIE_P1)
   if ( (portnum == 1) || (portnum== 2) )//PORT1
   {
      // Fill RX Descriptor
      WRITE_MEM32(RX2_DESC_ADDR + 0x08, htonl(PADDR(RX2_BUFF_ADDR)));  // RX Buffer Address
      WRITE_MEM32(RX2_DESC_ADDR + 0x18, htonl(PADDR(RX2_BUFF_ADDR)));  // RX Buffer Address

      // Ensure Descriptor is updated
      READ_MEM32(RX2_DESC_ADDR + 0x8);

      // Enable TX/RX (This seems to trigger NIC prefetching RX descriptor)
      WRITE_MEM8(PCIE2_EP_MEM + ChipCmd, 0x0C);

      #if 1
      // Fill Descriptor
      WRITE_MEM32(TX2_DESC_ADDR + 0x0, htonl(DescOwn | FirstFrag | LastFrag | RingEnd | size));
      WRITE_MEM32(TX2_DESC_ADDR + 0x8, htonl(PADDR((unsigned int) TX2_BUFF_ADDR)));

      // Ensure Descriptor is updated
      READ_MEM32(TX2_DESC_ADDR + 0x8);
      #else
      // Fill TX Descriptor, Buffer
      while (size > 0)
      {
         unsigned int buf_size = MIN(size, TX2_BUFF_SIZE);

         // Fill Buffer
         memcpy(tx2_buff_addr, content, buf_size);
         content += buf_size;
         size -= buf_size;

         // Fill Descriptor
         WRITE_MEM32(TX2_DESC_ADDR + 0x0, htonl(DescOwn | FirstFrag | LastFrag | RingEnd | buf_size));
         WRITE_MEM32(TX2_DESC_ADDR + 0x8, htonl(PADDR((unsigned int) TX2_BUFF_ADDR)));

         // Ensure Descriptor is updated
         READ_MEM32(TX2_DESC_ADDR + 0x8);

         tx2_desc_addr += TX2_DESC_SIZE;
         tx2_buff_addr += TX2_BUFF_SIZE;
      }
      #endif
     }
 #endif
}
//===============================================================================

 

int rtl8168_rx(unsigned char *content, unsigned int size, int portnum)
{
   ////unsigned int i,j=3000,k=3000;
   unsigned int i;
  unsigned int P0_PCIE_error_count=0;
  unsigned int P1_PCIE_error_count=0;
 

   
   
  #if DBG	  
     prom_printf("\nTest Rx Bytes=%d\n",size); //OK   
     prom_printf("P0 PCIE Error count: %d\n",P0_PCIE_error_count);
  #if (TEST_8196_PCIE_P1) 
     prom_printf("P1 PCIE Error count: %d\n",P1_PCIE_error_count);
  #endif
  #endif 
   
  
    if ((portnum == 0) || (portnum == 2) )
   {
      // Wait RX Packet   
		  //loop here when OWN bit=1 (means Memory is owned by PCIE IP)
		 //dprintf("Wait Rx own bit \n");
		  while ((READ_MEM32(RX1_DESC_ADDR) & htonl(DescOwn)) != 0)
	   ;//Memory own,"0":CPU,"1":PCIE IP
#if 0
	 //leroy ori
	  while ((READ_MEM32(RX1_DESC_ADDR) & htonl(DescOwn)) != 0)
	  {
	  	j--;
		delay_ms(10);
		if(j==0)
		{
			prom_printf("\nPCIE Rx1 overtime => test fail\n");    //original j=2000
			break;
		}
	  }
#endif		  	
		 //dprintf("got Rx own bit \n");
		 
      // Check RX Packet Content
      int i2;
	   
      for (i = 0; i < size; i++)        
      {
      
         if (READ_MEM8(RX1_BUFF_ADDR + i) != content[i])
         {
           #if DBG//JSW:DEBUG_PRINT
		  P0_PCIE_error_count++;
		     		  
	         prom_printf("Compare Error, No: Port0, Size: %d, Offset: %d, Content: 0x%X, Expected: 0x%X \n",
	                     size, i, READ_MEM8(RX1_BUFF_ADDR + i), content[i]);
		   
		   for (i2=1;i2<=4;i2++)
		   {
		   	 prom_printf("\nOffset(%d),Content:%x,Expected:%x\n",(i+i2),READ_MEM8(RX1_BUFF_ADDR + i+i2),content[i+i2]);

		   }
		   
		  
		   //prom_printf("\ncontent=%x\n",&content);
		   prom_printf("\n==================================\n");
		   prom_printf("\nOK Tx(0x%x) content=%x\n",(0xa0420000+i),READ_MEM32(0xa0420000+i));
		   prom_printf("\nFail Rx(0x%x) content=%x\n",(0xa0630000+i),READ_MEM32(0xa0630000+i));
		   prom_printf("\n==================================\n");

		
		   prom_printf("\nCheck last 4 Bytes,Tx(0x%x) content=%x\n",(0xa0420000+i-4),READ_MEM32(0xa0420000+i-4));
		   prom_printf("\nCheck last 4 Bytes,Rx(0x%x) content=%x\n",(0xa0630000+i-4),READ_MEM32(0xa0630000+i-4));
		   
		   
		 //#if 1 //for 2 port
		 		 	
		 	return -1; // RX Error
		 
		  
	    #else
 		   dprintf("Compare Error, No: Port0, Size: %d, Offset: %d, Content: 0x%X, Expected: 0x%X\n",
	                     size, i, READ_MEM8(RX1_BUFF_ADDR + i), content[i]);
			return -1;
	   #endif
	
            
//   halt_rx1:
//            goto halt_rx1;
         }
      }
   }
//-------------------------------------------------
  #if (TEST_8196_PCIE_P1)
    if( (portnum == 1) || (portnum == 2) )//PORT1
   {
      // Wait RX Packet

       	// #if PCIE_Test_With_8102E
	 #if 0
	      while ((READ_MEM32(RX1_DESC_ADDR) & htonl(DescOwn)) != 0)
	      {
		  	    //for 8102E
		  	    if (READ_MEM32(PCIE1_EP_CFG) == 0x813610EC) 
		  	   {
		  	   	   __delay(10);
				   break;
			    }
	      }		 	 
	    
	#else
		  while ((READ_MEM32(RX2_DESC_ADDR) & htonl(DescOwn)) != 0);
#if 0 //leroy ori
		  while ((READ_MEM32(RX2_DESC_ADDR) & htonl(DescOwn)) != 0)
		 {
	  		k--;
			delay_ms(10);
			if(k==0)
			{
				prom_printf("\nPCIE Rx2 overtime => test fail\n");    
				break;
			}
		  }
#endif
       #endif

       int i3;
      // Check RX Packet Content
        for (i = 0; i < size; i++)        
      {
      
         if (READ_MEM8(RX2_BUFF_ADDR + i) != content[i])
         {
           #if DBG//JSW:DEBUG_PRINT
		   P1_PCIE_error_count++;	
		  
	         prom_printf("Compare Error, No: Port1, Size: %d, Offset: %d, Content: 0x%X, Expected: 0x%X\n",
	                     size, i, READ_MEM8(RX2_BUFF_ADDR + i), content[i]);
		   //prom_printf("PCIE Mac LoopBack Compare Error!!! \n");
		   for (i3=1;i3<=4;i3++)
		   {
		   	 prom_printf("\nOffset(%d),Content:%x,Expected:%x\n",(i+i3),READ_MEM8(RX2_BUFF_ADDR + i+i3),content[i+i3]);

		   }
		   		   
			  
  		   
		   //prom_printf("\ncontent=%x\n",&content);
		   prom_printf("\n================================================\n");
		   prom_printf("\nOK Tx(0x%x) content=%x\n",(0xa0820000+i),READ_MEM32(0xa0820000+i));
		   prom_printf("\nFail Rx(0x%x) content=%x\n",(0xa0a30000+i),READ_MEM32(0xa0a30000+i));
		   prom_printf("\n================================================\n");

		   prom_printf("\nCheck last 4 Bytes,Tx(0x%x) content=%x\n",(0xa0820000+i-4),READ_MEM32(0xa0820000+i-4));
		   prom_printf("\nCheck last 4 Bytes,Rx(0x%x) content=%x\n",(0xa0a30000+i-4),READ_MEM32(0xa0a30000+i-4));
		   prom_printf("\n================================================\n");
		   return -1;
	    #else
 		   dprintf("Compare Error, No: Port1, Size: %d, Offset: %d, Content: 0x%X, Expected: 0x%X\n",
	                     size, i, READ_MEM8(RX2_BUFF_ADDR + i), content[i]);
		   return -1;
	   #endif
	
            
//   halt_rx1:
//            goto halt_rx1;
         }
      }
   }
 #endif

   return 0;
}
//===============================================================================
void rtl8168_tx_trigger(int portnum)
{
	if ((portnum == 0) |(portnum== 2) )   //Port 0
	{		
		WRITE_MEM8(PCIE1_EP_MEM + TxPoll, 0x40);	// Indicate TX Packet
	}
  
	if ((portnum == 1) |(portnum == 2) )   //PORT1
	{	     
	      WRITE_MEM8(PCIE2_EP_MEM + TxPoll, 0x40);	 // Indicate TX Packet
	}

}
//===============================================================================



