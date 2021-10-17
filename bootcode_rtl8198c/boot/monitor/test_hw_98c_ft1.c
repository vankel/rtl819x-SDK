
                 /*==========================================

                 ===========================================*/

#include <linux/interrupt.h>
#include <asm/system.h>
#include "monitor.h"
#include <asm/mipsregs.h>                            //wei add

                 //#include "test_lib.h"
#include "test_hw_98c_ft1.h"
#include <rtl8196x/asicregs.h>
//#include "wl_type.h"

#ifdef CONFIG_RTL8198C_FT2_TEST
int FT_P0_MII_GMII(int mode )   ;
#endif



unsigned char GetUSBPhy(unsigned char reg);
void SetUSBPhy(unsigned char reg, unsigned char val);
int show_board_info();






int show_board_info()
{
	         /*Show CPU and DRAM Freq*/
#define SYS_HW_STRAP   (0xb8000000 +0x08)
    unsigned int v=REG32(SYS_HW_STRAP);
	 unsigned int go_tftp_loop=0;
#if FT2_DBG
                 //prom_printf("Strap=%x\n",v);
#endif
                 //-----------
	#define GET_BITVAL(v,bitpos,pat) ((v& ((unsigned int)pat<<bitpos))>>bitpos)
	#define RANG1 1
	#define RANG2 3
	#define RANG3  7
	#define RANG4 0xf		
	#define RANG5 0x1f	


//const unsigned char *boot_type_tab[]={ "SPI0", "SPI1", "NFBI", "NAND", "ROM01", "ROM02", "ROM03", "EnAutoOLTtest" };
	const unsigned char *dram_type_tab[]={ "DDR2", "DDR3" };
	//const unsigned int m2x_clksel_table[]={ 250,	290,	330, 370, 410,  450,  490,  530,  390,  595, 625, 655, 685,  720,  755,  785};//[bit14:11]
	const unsigned int m2x_clksel_table[]={ 250,	270,290,310,330,350,370,565, 410,430,450,470,490,510 ,530,550 , 390, 580, 595,610, 625, 640,655,670, \
	685,700, 720, 740 ,755, 770 ,785,800};//[bit14:10]

   
   unsigned char dramtype_sel=GET_BITVAL(v, 3, RANG2);            
   unsigned char m2x_freq_sel=GET_BITVAL(v, 10, RANG5);  //bit [14:10], but bit 10 is not strap pin
  

    //unsigned int m1xclk=m2x_clksel_table[m2x_freq_sel]/2;
    unsigned int m1xclk=m2x_clksel_table[m2x_freq_sel];

                 /*Show Board's information
                     1.8196D FT2 code version
                     2.CPU, DRAM Freq
                 3.Chip ID code
                 */
     #ifdef CONFIG_RTL8198C   
   	 // dprintf("\n{SV001}\n");
     #endif


     /* 20130204: From Yozen
	0xE2: 0x9C=>  增加Output Swing Level
	0xE5: 0x93 => 增加Slew Rate
	0xE6: 0x98--> 0xC8 => Connect/Disconnect Issue
	0xE4: 0x03--> 0x01 => 降低Full_Speed Cross Over Voltage

	 */
     #if 0//USB PHY patch
   	////////////////////EHCI////////////////////////
REG32(0xb8000010)|=0xfffff000;//enable USB IP clock 


#if 1
 #define SYS_USB_SIE 0xb8000034
 #define SYS_USB_PHY 0xb8000090  
 // regw b8000034 b(12)    1
//    regw b8000034 b(18)    0   //oneportsel
//    regw b8000034 b(22)    1
    
REG32(SYS_USB_SIE)|=(1<<12);
//REG32(SYS_USB_SIE)&=~(1<<18);
REG32(SYS_USB_SIE)|=(1<<18);//port 1
REG32(SYS_USB_SIE)|=(1<<22);
    
  
   // regw b8000090 b(10-8)  5
  //  regw b8000090 b(21-19) 5

REG32(SYS_USB_PHY)&=~(7<<8);
REG32(SYS_USB_PHY)|=(5<<8);


REG32(SYS_USB_PHY)&=~(7<<19);
REG32(SYS_USB_PHY)|=(5<<19);

#endif

//E2:9c
 
SetUSBPhy(0xe2,0x9c);

//E5:93
SetUSBPhy(0xe5,0x93);

//E6:c8
 SetUSBPhy(0xe6,0xc8);

//E4:01
SetUSBPhy(0xe4,0x1);

   

GetUSBPhy(0xe2);
GetUSBPhy(0xe5);
GetUSBPhy(0xe6);
GetUSBPhy(0xe4);

     #endif
  
	 
                 //Show CPU current speed
    dprintf("\n{CPU=%d MHZ}\n",check_cpu_speed());
    dprintf("\n{DRAM=%d MHZ}\n",m1xclk);
	

//	prom_printf("Mode=%s\n",boot_type_tab[boot_sel]);
    dprintf("\n{%s=%d MB}\n", dram_type_tab[dramtype_sel],REG32(0xb8000f00));

}




#ifdef CONFIG_RTL8198C_FT1_TEST
              //---------------------------------------------------------------------------
                 //int CmdDump( int argc, char* argv[] )
int FT_P0_MII_GMII_RGMII(int mode )                        //GMII Test
{

     //MIB clear
    REG32(0xbb801000) |= 0xffffffff;

	//REG32(0xb8000010)|=  0x35FFFC00;; //Enable all IP clock
	//prom_printf("\nFT_P0_MII_GMII(),0xb8000010=%x\n",REG32(0xb8000010));
   
#if 0 //set MII-PHY mode
	prom_printf("\nTest_hw_96D_ft2.c:Set mode 0,MII PHY mode loopback test\r\n");
	REG32(PCRP0) |=  (0x10 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;	//external
	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_PHY<<23); 
	REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ;	
	REG32(P0GMIICR) |=(Conf_done);		
	REG32(PITCR) |= (1<<0);   //00: embedded , 01L GMII/MII/RGMII
	/* Enable L2 lookup engine and spanning tree functionality */
	// REG32(MSCR) = EN_L2 | EN_L3 | EN_L4 | EN_IN_ACL;
	REG32(MSCR) = EN_L2;
	REG32(QNUMCR) = P0QNum_1 | P1QNum_1 | P2QNum_1 | P3QNum_1 | P4QNum_1;

	/* Start normal TX and RX */
	REG32(SIRR) |= TRXRDY;
	REG32(FFCR) = EN_UNUNICAST_TOCPU | EN_UNMCAST_TOCPU; // rx broadcast and unicast packet
#endif
    int i=0;
    int port_cnt=5;
    int P0_reg_base=0xbb801100;
    int P0_reg_baseOUT=0xbb801800;

                 // int mode = strtoul((const char*)(argv[0]), (char **)NULL, 16);
                 //  regnum = strtoul((const char*)(argv[1]), (char **)NULL, 16);

                 /*
                 JSW 20110928: For 8196D FT test
                 mode 0 : for 8196D MII PHY mode loopback test
                 RXCTL <->TXCTL
                 RXD[3:0] <-> TXD[3:0]

                 mode 1 : for 8196D GMII MAC mode loopback test
                 P0 External must have a GMII-Phy IC and set to PCSLoopBack mode
                 //Refer to swcore.c , JSW 20110922:Only for 8196D FT2 GMII test , set 8211BN = PCSLoopBack mode
                 */
	#if 1
		/*Check P0/P5 GMII/RGMII setting value*/
		 dprintf("\nPort0 8196D GMII configuration register(0xbb80414c)=0x%x\r\n",REG32( 0xbb80414c ) );
		 dprintf("\nPort5 8198 GMII configuration register(0xbb804150)=0x%x\r\n",REG32( 0xbb804150 ) );
	
	#endif


    switch(mode)
                 //switch(0)
    {
        case 0:
            dprintf("\nmode 0:Send normal packet\r\n");
	     #if 1//def CONFIG_P0_GMII_FT1_8211DG //For 8211DG 
	/*
		Due to auto-learning is weird,so this procedure is set by FT1 PC
	*/
		dprintf("\nSet RTL8196DS Port0 to RGMII 100Mbps LoopBack mode\n" );
		delay_ms(10);

		
		delay_ms(10);
	#endif
            break;

        case 1:
	   
            dprintf("mode 1: 8211BG PCSLoopback\r\n");
	     
       //if((READ_MEM32(Bond_Option_REG)==13)||(READ_MEM32(Bond_Option_REG)==9)||(READ_MEM32(Bond_Option_REG)==8)||(READ_MEM32(Bond_Option_REG)==0)||(READ_MEM32(Bond_Option_REG)==1)) //JSW 20110922:Only for 8196D FT1 GMII test , set 8211BN = PCSLoopBack mode
	{	
		
	#if 0//def CONFIG_P0_GMII_EQC_8211BN  //For 8211BN-For 8197 EQC platform	
		dprintf("\nSet RTL8211BN to PCSGigaLoopback mode\n" );
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,31,0x0); //Set to Page0
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,11,0x2); //Set to PCS Giga Loopback mode	
		delay_ms(10);
		
		//Set Again for safety
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,31,0x0); //Set to Page0
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,11,0x2); //Set to PCS Giga Loopback mode	
		delay_ms(10);
	#else
	/*
		Due to auto-learning is weird,so this procedure is set by FT1 PC
	*/
		 dprintf("\nSet RTL8211DG to PCSGigaLoopback mode\n" );
		rtl8651_setAsicEthernetPHYReg(0x10,31,0x0); //Set to PCS Giga Loopback mode
		delay_ms(10);
		//Set Again for safety
		rtl8651_setAsicEthernetPHYReg(0x10,0,0x4140); //Set to PCS Giga Loopback mode
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,20,0x0042); //Set to PCS Giga Loopback mode
		delay_ms(10);

		
		//Set Again for safety
		rtl8651_setAsicEthernetPHYReg(0x10,31,0x0); //Set to PCS Giga Loopback mode
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,0,0x4140); //Set to PCS Giga Loopback mode
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,20,0x0042); //Set to PCS Giga Loopback mode
		delay_ms(10);
	#endif
	}	  
	     break;

        default:

            prom_printf("Error input, <mode> should be 0~1\r\n");
            break;
    }
#if 1
    for (i=5;i<=port_cnt;i++)
    {
    	 
        dprintf("Before,P%d MIB INcounterSize(%x)=%x\n",i,P0_reg_base+(i*0x80),REG32(P0_reg_base+(i*0x80)));
        dprintf("Before,P%d MIB IN-PacketCnt((%x)=%x\n",i,P0_reg_base+(i*0x80)+0x3c,REG32(P0_reg_base+(i*0x80)+0x3c));

        dprintf("Before,P%d MIB OUTcounterSize(%x)=%x\n",i,P0_reg_baseOUT+(i*0x80),REG32(P0_reg_baseOUT+(i*0x80)));
        dprintf("Before,P%d MIB OUT-PacketCnt(%x)=%x\n",i,P0_reg_baseOUT+(i*0x80)+0xc,REG32(P0_reg_baseOUT+(i*0x80)+0xc));
    }
#endif
    delay_ms(100);   
//#if defined (CONFIG_RTL8198C_FT1_TEST) ||defined( CONFIG_RTL8198C_FT2_TEST)
#if defined( CONFIG_RTL8198C_FT2_TEST)
   // prepare_txpkt2();  
#endif
    delay_ms(100);
    //prom_printf("\nSend a multicast packet \n");
                 //REG32(PCRP0) = REG32(PCRP0) | EnLoopBack;
                 //prom_printf("\nP0 EnLoopBack ,0xbb804104 bit 7=1 \n");

    for (i=5;i<=port_cnt;i++)
    {    	
        dprintf("After,P%d MIB INcounterSize(%x)=%x\n",i,P0_reg_base+(i*0x80),REG32(P0_reg_base+(i*0x80)));
        dprintf("After,P%d MIB IN-PacketCnt((%x)=%x\n",i,P0_reg_base+(i*0x80)+0x3c,REG32(P0_reg_base+(i*0x80)+0x3c));

        dprintf("After,P%d MIB OUTcounterSize(%x)=%x\n",i,P0_reg_baseOUT+(i*0x80),REG32(P0_reg_baseOUT+(i*0x80)));
        dprintf("After,P%d MIB OUT-PacketCnt(%x)=%x\n",i,P0_reg_baseOUT+(i*0x80)+0xc,REG32(P0_reg_baseOUT+(i*0x80)+0xc));

	
        if((REG32(P0_reg_base+(i*0x80))==REG32(P0_reg_baseOUT+(i*0x80)))&&(REG32(P0_reg_base+(i*0x80)+0x3c)==REG32(P0_reg_baseOUT+(i*0x80)+0xc)))
        {
            if(REG32(P0_reg_baseOUT+(i*0x80))==0x0)
                goto GMII_PCS_Fail ;

            if(mode)
            {
                dprintf("\nRG or GMII PCS Loopback Pass\n");
		  return 1;
            }   
            else
                dprintf("\nMII PHY mode Loopback Pass\n");

        }
        else
        {
            GMII_PCS_Fail:
            if(mode)
            {
                dprintf("\nRG or GMII PCSLoopback Fail\n");
		  return 0;
            }
            else
                dprintf("\nMII PHY mode Loopback fail\n");

            return 0;                                //fail

        }

    }

                 //MIB clear
    REG32(0xbb801000) |= 0xffffffff;
    return 1;                                        //pass

}





           
                 /*
                 81A FT1 Test Case: Software Version:                

                 Functionality:  Show current FT1 Version

                 PS.Send {SVxxx} to Tester for beginning test

                 History:

                 SV001:20121122 : 81A FT2 MP Test Program start               
		 
                 */
int RTL8198C_FT1_TEST_entry()
{


	#define FT1_DBG 0
#define CONFIG_RTL8198C_QA_FAIL_WARNING
int totalCase = 0, okCase = 0, failCase = 0;
	#if 0
	//printf("Core 1 Wakeup\n");
	CmdCore1Wakeup(0,NULL);

	#endif.


	
	unsigned int go_tftp_loop=0;
        //show_board_info();



	// dprintf("\n{SV001}\n");                       //means IDMEM Pass

       /*
                 FT1 Test Case:001

                 Functionality: Check IC Bonding

       */
               

   //    totalCase++; 

		#if 0
    if(check_chip_id_code())
    {
        //dprintf("\n{P001}\n");                       //means IDMEM Pass
       // okCase++;
    }
    else
    {
        //dprintf("\n{F001}\n");                       //means IDMEM Fail
       // failCase++;
    }
#endif
	
	   


   /*Switch L2 test
                  */



#if 0//def CONFIG_RTL8198C


                 /* Full reset and semreset */
                 //FullAndSemiReset();
                 //Setting_RTL8196D_PHY();
                 //swCore_init();

		/*RTL8196DS-VA-CG , QFN88-MII ,RGMII
		    RTL8196ES-VA-CG , QFN88-MII ,RGMII
		*/
          
			eth_startup(0);   //test EQC GMII
			//eth_startup(0);	//OK for 8196D 176 pin FT1		
           


              

                 /* Set bit[0]=0->1 for clear L2 table */
    REG32(0xbb804234) &= 0xFFFFFFFE;
    REG32(0xbb804234) |= 0x1;

                 /* Enable L2 lookup engine and spanning tree functionality */
    REG32(MSCR) = EN_L2;

    REG32(QNUMCR) = P0QNum_1 | P1QNum_1 | P2QNum_1 | P3QNum_1 | P4QNum_1;

                 /* Start normal TX and RX */
    REG32(SIRR) |= TRXRDY;

    dprintf("\n{R}\n");                              //Starting signal to Tester,test port0~4 port 10/100

#endif
 #if 0//def CONFIG_P0_GMII_FT1_8211DG         
	 if(READ_MEM32(Bond_Option_REG)==12)
        {          

	  /*20111214 JSW: For  RTL8197DU-VA-CG switch port0 FT1*/

     #ifdef CONFIG_RTL8881A
   	 dprintf("\n{Set8197DU-QFN88-UTP-FT1}\n");
     #endif

     #ifdef CONFIG_RTL8196E
   	 dprintf("\n{Set8196EU-QFN88-UTP-FT1}\n");
     #endif
	 
	          /*
			   20100902:Maxod for 8196CS 40MHZ FT2 
			   enable 	#(port0 lpbk and disable cpu port)			*/

	/*From Maxod
	由於詹董的測試環境換會single PHY的pacekt gen，所以無法handle half dupelx 
	的collision，麻煩將原本FT1 的mac lpbk 改成phy lpbk，以避免collision 照成的
	packet loss	
		page 0 reg26 bit 13 = 1 for enable 0 for disable
	*/
			//Set_GPHYWB(999, 0, 26, 0, 0x6000);  //bit[14:13]=1
			/*20110104:DS said just use switch loopback*/
			   
			//dprintf("\n{Disable cpu port}\n"); //set to 0x6e
			REG32(0xbb804600) = 0x0000006e;	
			   
			 //dprintf("\n{Enable,96CS port0 lpbk }\n");  //bit[7]=1
			REG32(0xbb804104) =0x02af00bf;
			
			 /*
			   20100902:Maxod for 8196CS 40MHZ FT2 
			   Enable cpu port.
		*/
		
		//REG32(0xbb804600) = 0x0000007e;	
		//prom_printf("\n0xbb804600=%x\n",READ_MEM32(0xbb804600));  
	
        }
#endif  //end of  CONFIG_P0_GMII_FT1_8211DG
          #if 1 //L2 cable test        
   eth_startup(0);   //test EQC GMII

              

                 /* Set bit[0]=0->1 for clear L2 table */
    REG32(0xbb804234) &= 0xFFFFFFFE;
    REG32(0xbb804234) |= 0x1;

                 /* Enable L2 lookup engine and spanning tree functionality */
    REG32(MSCR) = EN_L2;

    REG32(QNUMCR) = P0QNum_1 | P1QNum_1 | P2QNum_1 | P3QNum_1 | P4QNum_1;

                 /* Start normal TX and RX */
    REG32(SIRR) |= TRXRDY;

    dprintf("\n{R}\n");                              //Starting signal to Tester,test port0~4 port 10/100
     

                 /*


                 
                 FT2 Test Case: Linux phase

                 Functionality:  ???

                 PS.Linux will start after Polling UART "0x20" is fed
                 */
    while(1)
    {
        if(Wait_L2_DONE(KEYCODE_ESC))
        {
            dprintf("\n{ ");                       //for ESC
                 //delay_ms(100);  //20110331: JSW : for FT2 Time Reduction
            break;
        }
        else
        {        	
		go_tftp_loop++;
		if (go_tftp_loop>=3)
		{
			prom_printf("\nExit FT1 Test , go TFTP update\n"); 
			return;
			
		}
		prom_printf("\nNot Press ESC count=%d\n",go_tftp_loop); 
        }
    }


	

//////////////////////////////////////////////////////////////
#endif// end of L2 cable test       



#if 0//def CONFIG_P0_GMII_FT1_8211DG         
	 if(READ_MEM32(Bond_Option_REG)==12)
        {          

	  /*20111214 JSW: For  RTL8197DU-VA-CG switch port4 FT1*/
           
		// dprintf("\n{Set8197DU-QFN88-UTP-FT1} ");	              
		/*			
			   20100902:Maxod for 8196CS 40MHZ FT2 
			   Enable cpu port after test finished.
		*/		

	   
		

			//dprintf("\n{Enable CPU port}\n"); //set to 0x7e
		REG32(0xbb804600) = 0x0000007e;	
		//prom_printf("\n0xbb804600=%x\n",READ_MEM32(0xbb804600));  

		//dprintf("\n{Disable,96CS port0 lpbk }\n");  //bit[7]=0
		REG32(0xbb804104) &=0xFFFFFF7F;
	
        }
#endif


    /*
                                      EQC Test Case:005 , for EQC only

                20111214: For RTL8197DU-VA-CG , QFN88-UTP , Bonding ID=12

                Set to Port4 LoopBack Mode

                 */


#if 0//def CONFIG_P0_GMII_EQC_8211BN
	    if(READ_MEM32(Bond_Option_REG)==12)
        {  

	/*20111214 JSW: For  RTL8197DU-VA-CG switch port0 EQC, port0 must have Tx/Rx loopback connector*/
	 
		//dprintf("\n{Set8197DU-QFN88-UTP-EQC-P4LB} ");
		    totalCase++;

		   unsigned short RTL8197DU_Loop_cnt=5;

		   RTL8197DU_P4_LOOP:
		   	
		   if(FT2_8196CS_Switch_PHY_LoopBack())
   		   {
    			   dprintf("\n{P4-LB-P005}\n");                     
  			    okCase++;
			  
    		   }
    		   else
    		   {
    		   	   if(RTL8197DU_Loop_cnt)
    		   	   {
				RTL8197DU_Loop_cnt--;
				goto RTL8197DU_P4_LOOP;
    		   	   }
        		   dprintf("\n{P4-LB-F005}\n");
        		   failCase++;
     		   }
		    eth_startup(0);
	    		
		
	
        }
	else if(READ_MEM32(Bond_Option_REG)==13)
	{
		/*20120118 JSW: For  RTL8196DS-VA-CG switch port0 RGMII-RTL8211E  EQC, 
		test in 100Mbps
		port0 must have Tx/Rx loopback connector*/
	 
		

		#ifdef CONFIG_RTL8881A
   	 		dprintf("\n{Set8196DS-QFN88-RGMII-EQC-P0-100Mbps-LB} ");
     		#endif

     		#ifdef CONFIG_RTL8196E
   	 		dprintf("\n{Set8196ES-QFN88-RGMII-EQC-P0-100Mbps-LB} ");
     		#endif
		    totalCase++;

		   unsigned short RTL8196DS_Loop_cnt=10; //wait 10 secs

		   RTL8196DS_P0_MII_LOOP:
		   	
		   if(FT_P0_MII_GMII_RGMII(1) )
   		   {
    			   dprintf("\n{Po-LB-P005}\n");                     
  			    okCase++;
			  
    		   }
    		   else
    		   {
    		   	   /*due to 8196DS+ 8211E 100Mbps linkup need about 6 secs then pass FT_P0_MII_GMII_RGMII()*/
				   
    		   	   delay_ms(1000); 
    		   	   if(RTL8196DS_Loop_cnt)
    		   	   {
				RTL8196DS_Loop_cnt--;
				goto  RTL8196DS_P0_MII_LOOP;
    		   	   }
        		   dprintf("\n{Po-LB-F005}\n");
        		   failCase++;
     		   }
		    eth_startup(0);
	    		

	
	}
#endif


    /*
                                      FT1 Test Case:006 , for FT1 only

                 Functionality: 8881A FT_P0_MII_GMII + external RTL8211BG

                 */
	
#if (defined (CONFIG_RTL8198C_P0_GMII_PCSLBK_FT1)||defined (CONFIG_RTL8198C_P0_RGMII_PCSLBK_FT1))

  //  int bond_option_value=REG32(Bond_Option_REG);

 //int rtl96d_P0txdly;
// int rtl96d_P0rxdly;
//P006_Loop:	 	

 
 // rtl96d_P0txdly=tolence_P006_loop%2;
//   rtl96d_P0rxdly=tolence_P006_loop/2;

 //   REG32_ANDOR(P0GMIICR, ~((1<<4)|(3<<0)) , (rtl96d_P0txdly<<4) | (rtl96d_P0rxdly<<0) );


/*Case 006: For FT1 only! */
 
    //dprintf("\nP0GMIICR(0x%x)=0x%x\n\n\n",P0GMIICR,REG32(P0GMIICR));    


       /*Don't test Case006*/
//if((bond_option_value==1)||(bond_option_value==0))
//{   
//}
//else
//{
	 totalCase++;
 
    /*8196D only ID 13 get RGMII I/F*/   
#if 1	
    //eth_startup(1);  //test GMII PCSloop back
 
    if(FT_P0_MII_GMII_RGMII(1))
    {
        dprintf("{P006}");                      
        okCase++;
    }
    else
    {		 	
        dprintf("{F006}");                      
        failCase++;
    }
//}


 //eth_startup(0);  //reset to normal
  #endif

	
#endif




                 /*
                                      FT1 Test Case:007

                 Functionality: 8196D IMEM

                 */
#if 0//def CONFIG_RTL8881A_TAROKO
    totalCase++;
    if(imem_test())
    {
        dprintf("\n{P007}\n");                       //means IDMEM Pass
        okCase++;
    }
    else
    {
        dprintf("\n{F007}\n");                       //means IDMEM Fail
        failCase++;
    }
#endif




    /*
                            FT1 Test Case:010 for RTL8881A Wireless MAC

                 Functionality: Test RTL8881A Wireless MAC
                 */
#if 0
extern BOOLEAN WlCtrlOperater(IN  u4Byte  Mode);
totalCase++;
 if(WlCtrlOperater(1))                        //Wireless MAC
        {
            dprintf("\n{P009}\n");                   //means  Pass
            okCase++;
        }
        else
        {
            dprintf("\n{F009}\n");                   //means  Fail
            failCase++;
        }
//REG8(0xb8640522)=0xff; //Force fail
totalCase++;
  if(WlCtrlOperater(2))                        //Wireless MAC
        {
            dprintf("\n{P010}\n");                   //means  Pass
            okCase++;
        }
        else
        {
            dprintf("\n{F010}\n");                   //means  Fail
            failCase++;
        }

#endif

//////////////////////////////////////////////////////////////
#if 1
	//PCIE
	totalCase++;

    if((PCIE_reset_procedure(0,0,1))&&(PCIE_reset_procedure(1,0,1)))      //PCIE port 0 
    {
    	  dprintf("\n{P015}\n");   
		   okCase ++;
    	}
   else
   {
   	   dprintf("\n{F015}\n");   
		    failCase++;
   }
 
   
#endif
//////////////////////////////////////////////////////////////
#if 1

	totalCase++;
  	if(!example(2,0))   
       {                                            //prom_printf("\nPCIE Error\n");
            //return 0;                                //FAIL
            dprintf("\nPCIE Error\n");
	    failCase++;
		       dprintf("\n{F016}\n");                   //means PCIE Pass
        }
        else
       {                                           
                                         
            dprintf("\nPCIE Pass\n");
			      dprintf("\n{P016}\n");                   //means PCIE Pass
			      okCase++;
        }
#endif	
//////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////
 #define POLLING_REG 0xb800006c
  		#define PATT_SLEEP  0x3333		
  		#define PATT_READY  0x5555
  //showBoardInfo();
		
      totalCase++; 		
// if(REG32(POLLING_REG)==PATT_READY)
 //  if(REG32(POLLING_REG)==PATT_SLEEP)
  if((REG32(POLLING_REG)==PATT_READY)||(REG32(POLLING_REG)==PATT_SLEEP))
   {
        dprintf("\n{P020}\n");                       //means 1074k core1 pass
        okCase++;
    }
    else
    {
        dprintf("\n{F020}\n");                        //means 1074k core1 fail
        failCase++;
    }

//////////////////////////////////////////////////////////////
/*5181 Co-processor test*/
	

	#ifdef CONFIG_RLX5181_TEST
	  totalCase++; 		
	 if(boot_5181_mp_entry(0,NULL))
	 {
      	  	
      		okCase++;
   	 }
	 else
    	 {
        	
        	failCase++;
    	  }
	#endif


//////////////////////////////////////////////////////////////


#ifdef CONFIG_RTL8198C_QA_FAIL_WARNING
    prom_printf( ">> Total Case: %d,  Pass: %d,  Fail: %d\n\n", totalCase, okCase, failCase );
#endif

}

#endif

unsigned int check_chip_id_code()
{
                 //dprintf("Bond_Option_REG(0xb800000c)=%x\n",READ_MEM32(Bond_Option_REG) );
    unsigned int chip_id_code=READ_MEM32(ECO_NO_REG);
    unsigned int Bond_Option_value=READ_MEM32(Bond_Option_REG);				 

#if 1
 prom_printf("\n{0x%x}",chip_id_code);          
 prom_printf("\n{0x%x}",Bond_Option_value);       

 return;
#endif


#if defined(CONFIG_RTL8198C)
    switch(chip_id_code)
{
        case 0x8198C000:	
            prom_printf("{A-cut}");          
            break;	

	    case 0x8198C001:	
            prom_printf("{B-cut}");            
            break;

	    case 0x8198C002:		
            prom_printf("{C-cut}");
		  break;

	    case 0x8198C003:	
            prom_printf("{D-cut}");
		
            break;
	
	      default:
           	 prom_printf("\nRead chip_id_code Fail!,ID(0xB8000000)=0x%x\n",chip_id_code);
            	 return 0;       
    }
#endif  




#if defined(CONFIG_RTL8198C)
    switch(Bond_Option_value)
{
	 case 0x00000000:
	     prom_printf("\n{8198C}\n");         //test chip :DRQFN164 or BGA292
            return 0x8198C;
		break;	

	 case 0x00000001:
	     prom_printf("\n{8198C-LQFP216}\n");         //A cut chip :DR-QFN164  (原leadframe & 出pin)   
              return 0x8198C;
		break;	

	 case 0x00000002:
	     prom_printf("\n{8198C}\n");         //A cut chip :DR-QFN164  (原leadframe & 出pin) +MCM_MII    
               return 0x8198C;
		break;	

	 case 0x00000003:
	     prom_printf("\n{8198C}\n");         //A cut chip :DR-QFN164  (原leadframe & 出pin) 
              return 0x8198C;
		break;	

	 case 0x00000009:
	     //prom_printf("\n{8881AQP}\n");         //A cut chip :(新leadframe & 新出pin)
	     prom_printf("\n{8198C}\n");         //20130806:Rename from AQP
              return 0x8198C;

	 case 0x0000000a:
	     prom_printf("\n{8198C}\n");         //A cut chip :DR-QFN164 (新leadframe & 新出pin) +MCM +SDR 8MB
              return 0x8198C;
		break;	
            

	     default:
           	 prom_printf("\nRead bond_id_code Fail!,ID(0xB800000c)=0x%x\n",Bond_Option_value);
            	 return 0;    
    }
#endif  
	return 1;
                 //return (chip_id_code);
}


int PCIE_TEST_FT1(unsigned int test_packet_num)
{
   
    delay_ms(10);
    PCIE_reset_procedure(0,0,1);//PCIE port 0 
    PCIE_reset_procedure(1,0,1);//PCIE port 1

 
      int bond_option_value=REG32(Bond_Option_REG);   


	 delay_ms(10);
                 // dprintf("\nPCIE delay \n");
   // __delay(1000*1000);                              //JSW @20100202:Must add this for 8198 PCIE Reset

                 //dprintf("Re-Check PIN_MUX_SEL(0xb8000030)=%x\n",*(volatile unsigned int*)PIN_MUX_SEL);

#if 1
    REG32(0xb8b00110)=0xffff;                        /*20100811:PCIE write clear*/

    unsigned int i,fail_record_cnt=0;
    for (i=1;i<=test_packet_num;i++)
    {  
  	if(!example(2,0))   
       {                                            //prom_printf("\nPCIE Error\n");
            //return 0;                                //FAIL
            dprintf("\nPCIE Error\n");
	     fail_record_cnt++;
        }
        else
       {                                           
                                         
            dprintf("\nPCIE Pass\n");
        }
    



    }                                                //END of for test_packet_num

	if(fail_record_cnt==0)
	{
		return 1; //Total pass
	}
	else
	{
		return 0;//fail once
	}
		
    //prom_printf("\nPCIE Pass\n");
#endif

}


int Wait_L2_DONE(unsigned char ASCII_CODE)
{

                 //unsigned char iChar[2];
                 //GetLine( iChar, 2,1);
    unsigned char iChar[1];
    GetLine( iChar, 1,1);

    prom_printf("\n");                               //vicadd

    if ((iChar[0] == ASCII_CODE ))                   //It's "ESC"
        return 1;
    else
    {
                 //prom_printf("Input iChar[0]=%x,iChar[1]=%x,iChar[2]=%x",iChar[0],iChar[1],iChar[2]);
        return 0;
    }
}



                

#ifdef CONFIG_RTL8198C_FT2_TEST
#define FISH_DELAY 10
#if 0
int mlt3_dc()
{

    RTL8196D_FT2_TEST_GPIOinit();
                 //change to page0
    REG32(0xbb804004 ) = 0x801F0000;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811F0000;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821F0000;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831F0000;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841F0000;
    delay_ms(FISH_DELAY);

                 //disable 10M power saving
                 //page0, reg24, bit15
    REG32(0xbb804004 ) = 0x80180310;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x81180310;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x82180310;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x83180310;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x84180310;
    delay_ms(FISH_DELAY);

                 //pag0, reg0, [13]Speed 100M, [8]Full duplex, [12]disable auto negotiation
                 //100M
    REG32(0xbb804004 ) = 0x80002100;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x81002100;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x82002100;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x83002100;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x84002100;
    delay_ms(FISH_DELAY);

	//RTL8881A modify
                 //mdi
                 //DC_Level 0, pag0, reg28,
    #if 0
    REG32(0xbb804004 ) = 0x801C50C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811C50C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821C50C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831C50C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841C50C2;
    delay_ms(FISH_DELAY);

                 //DC_Level +1, pag0, reg28,
    REG32(0xbb804004 ) = 0x801C54C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811C54C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821C54C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831C54C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841C54C2;
    delay_ms(FISH_DELAY);
    #else
    //mdi
//pag0, reg28, [1]force mdi, [2]disable auto mdi/mdix
//mdi
    REG32(0xbb804004 ) = 0x801C40C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811C40C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821C40C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831C40C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841C40C2;
    delay_ms(FISH_DELAY);

    //change to page5
    REG32(0xbb804004 ) = 0x801f0005;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811f0005;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821f0005;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831f0005;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841f0005;
    delay_ms(FISH_DELAY);

                 
//DC_Level +1, pag5, reg16,bit14=1, bit[13:12]=01
    REG32(0xbb804004 ) = 0x8010d024;
    delay_ms(FISH_DELAY);
     REG32(0xbb804004 ) = 0x8110d024;
    delay_ms(FISH_DELAY);
     REG32(0xbb804004 ) = 0x8210d024;
    delay_ms(FISH_DELAY);
     REG32(0xbb804004 ) = 0x8310d024;
    delay_ms(FISH_DELAY);
     REG32(0xbb804004 ) = 0x8410d024;
    delay_ms(FISH_DELAY);
    #endif

    
    REG32(PABCDISR_REG) |=0xffffffff;                //Write clear once
    while(1)
    {
    	
        delay_ms(DLY_8196D_FT2_TIME);                //Keep test result 100ms
        if(REG32(PABCDISR_REG) &0x2000)
        {
            prom_printf("\nGPIO_B5 rising edge interrupt happened,go to next test case\n" );
            REG32(PABCDISR_REG) |=0x2000;            //Write clear
            break;
        }
        else
        {
            prom_printf("\n4-4,DC_Level+1(No GPIO_B5 rising INT)=>delay %dms, measure TX+\n",DLY_8196D_FT2_TIME);
	     delay_ms(DLY_8196D_FT2_TIME);  		
        }
    }
                 //delay 100ms, DC_Level+1 , measure D+




                 //DC_Level -1, pag0, reg28,
    #if 0
    REG32(0xbb804004 ) = 0x801C58C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811C58C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821C58C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831C58C2;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841C58C2;
    delay_ms(FISH_DELAY);
    #else //DC_Level -1, pag5, reg16,bit14=1, bit[13:12]=10
    REG32(0xbb804004 ) = 0x8010E024;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x8110E024;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x8210E024;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x8310E024;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x8410E024;
    delay_ms(FISH_DELAY);
    #endif
    while(1)
    {        
        delay_ms(DLY_8196D_FT2_TIME);                //Keep test result 100ms
        if(REG32(PABCDISR_REG) &0x2000)
        {
            prom_printf("\nGPIO_B5 rising edge interrupt happened,go to next test case\n" );
            REG32(PABCDISR_REG) |=0x2000;            //Write clear
            break;
        }
        else
        {
            prom_printf("\n4-3,DC_Level-1(No GPIO_B5 rising INT)=>delay 100ms, measure TX-\n");
	     delay_ms(DLY_8196D_FT2_TIME);  
        }
    }
                 //delay 100ms, DC_Level-1,measure D-



                 //mdix
                

    //change to page0
     REG32(0xbb804004 ) = 0x801F0000;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811F0000;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821F0000;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831F0000;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841F0000;
    delay_ms(FISH_DELAY);

   //mdix
	//pag0, reg28, [1]force mdi, [2]disable auto mdi/mdix
	//mdix

   REG32(0xbb804004 ) = 0x801C40C0;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811C40C0;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821C40C0;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831C40C0;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841C40C0;
    delay_ms(FISH_DELAY);

    //change to page5
      REG32(0xbb804004 ) = 0x801f0005;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x811f0005;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x821f0005;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x831f0005;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x841f0005;
    delay_ms(FISH_DELAY);


	   REG32(0xbb804004 ) = 0x8010d024;
    delay_ms(FISH_DELAY);
     REG32(0xbb804004 ) = 0x8110d024;
    delay_ms(FISH_DELAY);
     REG32(0xbb804004 ) = 0x8210d024;
    delay_ms(FISH_DELAY);
     REG32(0xbb804004 ) = 0x8310d024;
    delay_ms(FISH_DELAY);
     REG32(0xbb804004 ) = 0x8410d024;
    delay_ms(FISH_DELAY);

    while(1)
    {
        delay_ms(DLY_8196D_FT2_TIME);                //Keep test result 100ms
        if(REG32(PABCDISR_REG) &0x2000)
        {
            prom_printf("\nGPIO_B5 rising edge interrupt happened,go to next test case\n" );
            REG32(PABCDISR_REG) |=0x2000;            //Write clear
            break;
        }
        else
        {
            prom_printf("\n4-2,DC_Level +1(No GPIO_B5 rising INT)=>delay 100ms, measure RX+\n");
	     delay_ms(DLY_8196D_FT2_TIME);  
        }
    }

    REG32(0xbb804004 ) = 0x8010E024;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x8110E024;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x8210E024;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x8310E024;
    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x8410E024;
    delay_ms(FISH_DELAY);
        
#if 1
    while(1)
    {        
        delay_ms(DLY_8196D_FT2_TIME);                //Keep test result 100ms
        if(REG32(PABCDISR_REG) &0x2000)
        {
            prom_printf("\nGPIO_B5 rising edge interrupt happened,go to next test case\n" );
            REG32(PABCDISR_REG) |=0x2000;            //Write clear
            break;
        }
        else
        {
            prom_printf("\n4-1,DC_Level -1(No GPIO_B5 rising INT)=>delay 100ms, measure RX-\n");
	     delay_ms(DLY_8196D_FT2_TIME);  
        }
    }
#else
    prom_printf("\n4-1,DC_Level -1(No GPIO_B5 rising INT)=>delay 100ms, measure D-\n");
#endif

                 //delay 100ms, DC_Level -1 , measure RX-

}
#else

int mlt3_dc()

{

 

    RTL8196D_FT2_TEST_GPIOinit();

    delay_ms(FISH_DELAY);
    REG32(0xbb804004 ) = 0x881F0a46;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x811F0a46;
    delay_ms(FISH_DELAY);	

    REG32(0xbb804004 ) = 0x821F0a46;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x831F0a46;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x841F0a46;
    delay_ms(FISH_DELAY);
	


    REG32(0xbb804004 ) = 0x88150302;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x81150302;
    delay_ms(FISH_DELAY);	

    REG32(0xbb804004 ) = 0x82150302;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x83150302;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x84150302;
    delay_ms(FISH_DELAY);




    REG32(0xbb804004 ) = 0x881F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x811F0a58;
    delay_ms(FISH_DELAY);	

    REG32(0xbb804004 ) = 0x821F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x831F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x841F0a58;
    delay_ms(FISH_DELAY);




    REG32(0xbb804004 ) = 0x88101000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x81101000;
    delay_ms(FISH_DELAY);	

    REG32(0xbb804004 ) = 0x82101000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x83101000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x84101000;
    delay_ms(FISH_DELAY);



    REG32(0xbb804004 ) = 0x881F0a46;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x811F0a46;
    delay_ms(FISH_DELAY);	

    REG32(0xbb804004 ) = 0x821F0a46;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x831F0a46;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x841F0a46;
    delay_ms(FISH_DELAY);



    REG32(0xbb804004 ) = 0x88150300;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x81150300;
    delay_ms(FISH_DELAY);	

    REG32(0xbb804004 ) = 0x82150300;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x83150300;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x84150300;
    delay_ms(FISH_DELAY);


    
  ////DC +1
//dec2hex(reg_pw('a58', 16, 15, 0,'1010'));
/*
ew bb804004 881F0a58
ew bb804004 811F0a58
ew bb804004 821F0a58
ew bb804004 831F0a58
ew bb804004 841F0a58


*/

    REG32(0xbb804004 ) = 0x881F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x811F0a58;
    delay_ms(FISH_DELAY);	

    REG32(0xbb804004 ) = 0x821F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x831F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x841F0a58;
    delay_ms(FISH_DELAY);

/*
ew bb804004 88101010
ew bb804004 81101010
ew bb804004 82101010
ew bb804004 83101010
ew bb804004 84101010
*/

    REG32(0xbb804004 ) = 0x88101010;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x81101010;
    delay_ms(FISH_DELAY);	

    REG32(0xbb804004 ) = 0x82101010;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x83101010;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x84101010;
    delay_ms(FISH_DELAY);
  
    

    REG32(PABCDISR_REG) |=0xffffffff;                //Write clear once

    while(1)

    {

     

        delay_ms(DLY_8196D_FT2_TIME);                //Keep test result 100ms

        if(REG32(PABCDISR_REG) &0x2000)

        {

		 RTL8196D_FT2_GPIO_low_loop();

            prom_printf("\nGPIO_B5 rising edge interrupt happened,go to next test case\n" );

            REG32(PABCDISR_REG) |=0x2000;            //Write clear

            break;

        }

        else

        {

            prom_printf("\n2-1,DC_Level+1 (No GPIO_B5 rising INT)=>delay %dms, measure TX+\n",DLY_8196D_FT2_TIME);

            delay_ms(DLY_8196D_FT2_TIME);    

        }

    }

                

 

 

 

 

                 //DC_Level -1, pag0, reg28,

    #if 0

   ////DC -1
//dec2hex(reg_pw('a58', 16, 15, 0,'1000'));

ew bb804004 881F0a58
ew bb804004 811F0a58
ew bb804004 821F0a58
ew bb804004 831F0a58
ew bb804004 841F0a58

ew bb804004 88101000
ew bb804004 81101000
ew bb804004 82101000
ew bb804004 83101000
ew bb804004 84101000

    #else //DC_Level -1, pag5, reg16,bit14=1, bit[13:12]=10

    REG32(0xbb804004 ) = 0x881F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x811F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x821F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x831F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x841F0a58;
    delay_ms(FISH_DELAY);




     REG32(0xbb804004 ) = 0x88101000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x81101000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x82101000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x83101000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x84101000;
    delay_ms(FISH_DELAY);

    #endif

    while(1)

    {        

        delay_ms(DLY_8196D_FT2_TIME);                //Keep test result 100ms

        if(REG32(PABCDISR_REG) &0x2000)

        {


		 RTL8196D_FT2_GPIO_low_loop();
		 
            prom_printf("\nGPIO_B5 rising edge interrupt happened,go to next test case\n" );

            REG32(PABCDISR_REG) |=0x2000;            //Write clear

            break;

        }

        else

        {

              prom_printf("\n2-2,DC_Level-1 (No GPIO_B5 rising INT)=>delay %dms, measure TX+\n",DLY_8196D_FT2_TIME);

           delay_ms(DLY_8196D_FT2_TIME);  

        }

    }

                 //delay 100ms, DC_Level-1,measure D-




    //change to page0				 
   /*
//DC mode finish, reset to default value
//dec2hex(reg_pw('a58', 16, 15, 0,'0000'));

ew bb804004 881F0a58
ew bb804004 811F0a58
ew bb804004 821F0a58
ew bb804004 831F0a58
ew bb804004 841F0a58

ew bb804004 88100000
ew bb804004 81100000
ew bb804004 82100000
ew bb804004 83100000
ew bb804004 84100000
*/               

 

  REG32(0xbb804004 ) = 0x881F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x811F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x821F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x831F0a58;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x841F0a58;
    delay_ms(FISH_DELAY);




     REG32(0xbb804004 ) = 0x88100000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x81100000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x82100000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x83100000;
    delay_ms(FISH_DELAY);

    REG32(0xbb804004 ) = 0x84100000;
    delay_ms(FISH_DELAY);

  
 

 

 
 



 

        

#if 0

    while(1)

    {        

        delay_ms(DLY_8196D_FT2_TIME);                //Keep test result 100ms

        if(REG32(PABCDISR_REG) &0x2000)

        {

            prom_printf("\nGPIO_B5 rising edge interrupt happened,go to next test case\n" );

            REG32(PABCDISR_REG) |=0x2000;            //Write clear

            break;

        }

        else

        {

            prom_printf("\n4-1,DC_Level -1(No GPIO_B5 rising INT)=>delay 100ms, measure RX-\n");

      delay_ms(DLY_8196D_FT2_TIME);  

        }

    }

#else

    prom_printf("\nDC mode test end\n\n");
    delay_ms(DLY_8196D_FT2_TIME);  	
#endif

 

                

 

}




#endif

#endif

#if 0
int cmd_gen_pauseframe(int argc, char *argv[])
{
    int portnum, second, i;
    int val;

    portnum = atoi(argv[2]);
    second = atoi(argv[3]);
#if 1                                            //ndef NFBI_HOST_IS_PANABOARD
                 //use Embedded SmartBit-like function to generate Pause frame
    if (portnum == 0)
    {
        read_word(0xbb804104, &val);
        write_word(0xbb804104, (val|0x00000300));    //enable P0 pause flow control
        write_word(0xbb806900, 0x00000100);          //"force send Pause frame" enable for P0
        for (i=0; i<second; i++)
        {
            write_word(0xbb806904, 0x01000001);      //SMB mode enable, start Tx
            usleep(100);
            write_word(0xbb806904, 0x01000002);      //SMB mode enable, stop Tx
            usleep(100);                             //0.1ms, the delay is necessary
            write_word(0xbb806904, 0x00000000);      //SMB mode disable
            real_sleep(1);
        }
    }
    else if (portnum == 3)
    {
        read_word(0xbb804110, &val);
        write_word(0xbb804110, (val|0x00000300));    //enable P3 pause flow control
        write_word(0xbb806900, 0x00000800);          //"force send Pause frame" enable for P3
        for (i=0; i<second; i++)
        {
            write_word(0xbb806904, 0x08001000);      //SMB mode enable, start Tx
            usleep(100);
            write_word(0xbb806904, 0x08002000);      //SMB mode enable, stop Tx
            usleep(100);                             //0.1ms, the delay is necessary
            write_word(0xbb806904, 0x00000000);      //SMB mode disable
            real_sleep(1);
        }
    }
    else if (portnum == 5)
    {
        read_word(0xbb804118, &val);
        write_word(0xbb804118, (val|0x00000300));    //enable P5 pause flow control
        write_word(0xbb806900, 0x00002000);          //"force send Pause frame" enable for P5
        for (i=0; i<second; i++)
        {
            write_word(0xbb806904, 0x20100000);      //SMB mode enable, start Tx
            usleep(100);
            write_word(0xbb806904, 0x20200000);      //SMB mode enable, stop Tx
            usleep(100);                             //0.1ms, the delay is necessary
            write_word(0xbb806904, 0x00000000);      //SMB mode disable
            real_sleep(1);
        }
    }
    else
    {
        return -1;
    }
    write_word(0xbb806900, 0x00000000);              //"force send Pause frame" disable
#else
                 //TBD
#endif
    return 0;
}
#endif

#ifdef CONFIG_RTL8198C_FT2_TEST
#define REG32_ANDOR(x,y,z)   (REG32(x)=(REG32(x)& (y))|(z))
                 //---------------------------------------------------------------------------
                 //int CmdDump( int argc, char* argv[] )
int FT_P0_MII_GMII(int mode )                        //GMII Test
{
	


   REG32(0xb8000010)|=  0xfffffc00;; //Enable all IP clock
	//prom_printf("\nFT_P0_MII_GMII(),0xb8000010=%x\n",REG32(0xb8000010));

                  //MIB clear
    //REG32(0xbb801000) |= 0xffffffff;



#if 0//def CONFIG_RTL8881A_FT2_TEST  //DS
if((READ_MEM32(Bond_Option_REG)==13)||(READ_MEM32(Bond_Option_REG)==8)) //JSW 20120103:Only for 8196D FT2 GMII test 
{
	//rtl8651_setAsicEthernetPHYReg(uint32 phyId, uint32 regId, uint32 wData);
	
	#if 0//def CONFIG_P0_GMII_FT1_8211DG //For 8211DG 
	/*
		Due to auto-learning is weird,so this procedure is set by FT1 PC
	*/
		 dprintf("\nSet RTL8211DG to PCSGigaLoopback mode\n" );
		delay_ms(10);
		//Set Again for safety
		rtl8651_setAsicEthernetPHYReg(0x10,0,0x4140); //Set to PCS Giga Loopback mode
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,20,0x0042); //Set to PCS Giga Loopback mode
		delay_ms(10);

		delay_ms(10);
		//Set Again for safety
		rtl8651_setAsicEthernetPHYReg(0x10,0,0x4140); //Set to PCS Giga Loopback mode
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,20,0x0042); //Set to PCS Giga Loopback mode
		delay_ms(10);
	#endif
	
	#if 0//def CONFIG_P0_GMII_EQC_8211BN  //For 8211BN-For 8197 EQC platform	
		 dprintf("\nSet RTL8211BN to PCSGigaLoopback mode\n" );
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,31,0x0); //Set to Page0
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,11,0x2); //Set to PCS Giga Loopback mode	
		delay_ms(10);
		
		//Set Again for safety
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,31,0x0); //Set to Page0
		delay_ms(10);
		rtl8651_setAsicEthernetPHYReg(0x10,11,0x2); //Set to PCS Giga Loopback mode	
		delay_ms(10);
	#endif
}
	
#endif

#ifdef CONFIG_RTL8198C_FT2_TEST  //1F //set MII-PHY mode
	//prom_printf("\nTest_hw_81a_ft1.c:Set mode 0,MII PHY mode loopback test\r\n");
	REG32(PCRP0) |=  (0x10 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;	//external
	REG32(PCRP5) |=  (0x10 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;	//external
	
	REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_PHY<<23); 
	REG32_ANDOR(P5GMIICR, ~(3<<23)  , LINK_MII_PHY<<23); 
	
	REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ;	
	REG32(P0GMIICR) |=(Conf_done);		

	REG32_ANDOR(PCRP5, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed100M |ForceDuplex) ;	
	REG32(P5GMIICR) |=(Conf_done);	

	
	REG32(PITCR) |= (1<<0);   //00: embedded , 01L GMII/MII/RGMII
	/* Enable L2 lookup engine and spanning tree functionality */
	// REG32(MSCR) = EN_L2 | EN_L3 | EN_L4 | EN_IN_ACL;
	REG32(MSCR) = EN_L2;
	REG32(QNUMCR) = P0QNum_1 | P1QNum_1 | P2QNum_1 | P3QNum_1 | P4QNum_1;

	/* Start normal TX and RX */
	REG32(SIRR) |= TRXRDY;
	REG32(FFCR) = EN_UNUNICAST_TOCPU | EN_UNMCAST_TOCPU; // rx broadcast and unicast packet
#endif
    int i=0;
    //int port_cnt=0; // Just test port 0
     int port_cnt=5; // Just test port 5
    int P0_reg_base=0xbb801100;
    int P0_reg_baseOUT=0xbb801800;

                 // int mode = strtoul((const char*)(argv[0]), (char **)NULL, 16);
                 //  regnum = strtoul((const char*)(argv[1]), (char **)NULL, 16);

                 /*
                 JSW 20110928: For 8196D FT2 test
                 mode 0 : for 8196D MII PHY mode loopback test
                 RXCTL <->TXCTL
                 RXD[3:0] <->TXD[3:0]

                 mode 1 : for 8196D GMII MAC mode loopback test
                 P0 External must have a GMII-Phy IC and set to PCSLoopBack mode
                 //Refer to swcore.c , JSW 20110922:Only for 8196D FT2 GMII test , set 8211BN = PCSLoopBack mode
                 */
	#if 1
		/*Check P0/P5 GMII/RGMII setting value*/
		 dprintf("\nPort0 8196D GMII configuration register(0xbb80414c)=0x%x\r\n",REG32( 0xbb80414c ) );

		 dprintf("\nPort5 8198 GMII configuration register(0xbb804150)=0x%x\r\n",REG32( 0xbb804150 ) );
	
	#endif


    switch(mode)
                 //switch(0)
    {
        case 0:
            dprintf("\nmode 0:MII PHY mode loopback test\r\n");
            break;

        case 1:
	   
            dprintf("mode 1: RG-GMII MAC mode\r\n");
	       #if 0
                                                     //Set external PHYID
            REG32(PCRP0) |=  (0x10 << ExtPHYID_OFFSET) | MIIcfg_RXER |  EnablePHYIf | MacSwReset;
                                                     //GMII
            REG32_ANDOR(P0GMIICR, ~(3<<23)  , LINK_MII_MAC<<23);
                                                     //Set Force Giga
            REG32_ANDOR(PCRP0, ~AutoNegoSts_MASK, EnForceMode| ForceLink|ForceSpeed1000M |ForceDuplex );
            REG32(P0GMIICR) |=(Conf_done);

            REG32(PITCR) |= (1<<0);                  //00: embedded , 01L GMII/MII/RGMII
            REG32(0xbb804000) |= (1<<12);            //giga link
           
	    #endif
	     break;

        default:

            prom_printf("Error input, <mode> should be 0~1\r\n");
            break;
    }
#if 1
    for (i=5;i<=port_cnt;i++)
    {
    	                 //MIB clear
    	 //REG32(0xbb801000) |= 0xffffffff;
        dprintf("Before,P%d MIB INcounterSize(%x)=%x\n",i,P0_reg_base+(i*0x80),REG32(P0_reg_base+(i*0x80)));
        dprintf("Before,P%d MIB IN-PacketCnt((%x)=%x\n",i,P0_reg_base+(i*0x80)+0x3c,REG32(P0_reg_base+(i*0x80)+0x3c));

        dprintf("Before,P%d MIB OUTcounterSize(%x)=%x\n",i,P0_reg_baseOUT+(i*0x80),REG32(P0_reg_baseOUT+(i*0x80)));
        dprintf("Before,P%d MIB OUT-PacketCnt(%x)=%x\n",i,P0_reg_baseOUT+(i*0x80)+0xc,REG32(P0_reg_baseOUT+(i*0x80)+0xc));
    }
#endif
    delay_ms(100);
    prepare_txpkt2();
    delay_ms(100);
    //prom_printf("\nSend a multicast packet \n");

                 //REG32(PCRP0) = REG32(PCRP0) | EnLoopBack;
                 //prom_printf("\nP0 EnLoopBack ,0xbb804104 bit 7=1 \n");

    for (i=5;i<=port_cnt;i++)
    {
        dprintf("After,P%d MIB INcounterSize(%x)=%x\n",i,P0_reg_base+(i*0x80),REG32(P0_reg_base+(i*0x80)));
        dprintf("After,P%d MIB IN-PacketCnt((%x)=%x\n",i,P0_reg_base+(i*0x80)+0x3c,REG32(P0_reg_base+(i*0x80)+0x3c));

        dprintf("After,P%d MIB OUTcounterSize(%x)=%x\n",i,P0_reg_baseOUT+(i*0x80),REG32(P0_reg_baseOUT+(i*0x80)));
        dprintf("After,P%d MIB OUT-PacketCnt(%x)=%x\n",i,P0_reg_baseOUT+(i*0x80)+0xc,REG32(P0_reg_baseOUT+(i*0x80)+0xc));

        if((REG32(P0_reg_base+(i*0x80))==REG32(P0_reg_baseOUT+(i*0x80)))&&(REG32(P0_reg_base+(i*0x80)+0x3c)==REG32(P0_reg_baseOUT+(i*0x80)+0xc)))
        {
            if(REG32(P0_reg_baseOUT+(i*0x80))==0x0)
                goto GMII_PCS_Fail ;

            if(mode)
                dprintf("\nRG or GMII PCS Loopback Pass\n");
            else
                dprintf("\nMII PHY mode Loopback Pass\n");

        }
        else
        {
            GMII_PCS_Fail:
            if(mode)
                dprintf("\nRG or GMII PCSLoopback Fail\n");
            else
                dprintf("\nMII PHY mode Loopback fail\n");

            return 0;                                //fail

        }

    }

                 //MIB clear
    REG32(0xbb801000) |= 0xffffffff;
    return 1;                                        //pass

}
#endif



#if 1

//=========================================================

//=========================================================

//=========================================================
int GPHY_Openlpbk()
{

	/*
	// set trxrdy
wr addr 0x1b80_4204, wdat=0x0000_0001
// enable force mode
wr addr 0x1b80_4104, wdta=0x427f_0038
wr addr 0x1b80_4108, wdta=0x467f_0038
wr addr 0x1b80_410c, wdta=0x4a7f_0038
wr addr 0x1b80_4110, wdta=0x4e7f_0038
wr addr 0x1b80_4114, wdta=0x527f_0038
wr addr 0x1b80_4118, wdta=0x566f_0038
// enable phyid0
wr addr 0x1b80_4004, wdat=0x8818_2198
wr addr 0x1b80_4004, wdat=0x8118_2198
wr addr 0x1b80_4004, wdat=0x8218_2198
wr addr 0x1b80_4004, wdat=0x8318_2198
wr addr 0x1b80_4004, wdat=0x8418_2198


// wr phyid(0), page(0xa40), reg(0x0) -> to disable rg_pwrdn
wr addr 0x1b80_4004, wdat=0x801f_0a40
wr addr 0x1b80_4004, wdat=0x8000_1140
// wr phyid(0), page(0xa46), reg(0x14)-> to disable ext_ini_done and spdup ext_ini_timer(for 100m)
wr addr 0x1b80_4004, wdat=0x801f_0a46
wr addr 0x1b80_4004, wdat=0x8014_0003


// wr phyid(0), page(0xa4a), reg(0x13)-> to spdup PCS timer, only for simulation
//wr addr 0x1b80_4004, wdat=0x801f_0a4a
//wr addr 0x1b80_4004, wdat=0x8013_001f
// wr phyid(0), page(0xb80), reg(0x17)-> to spdup uC timer, only for simulation
//wr addr 0x1b80_4004, wdat=0x801f_0b80
//wr addr 0x1b80_4004, wdat=0x8017_000e
// check port0~port4 page(0a40)reg(0)'s data
wr addr 0x1b80_4004, wdat=0x801f_0a40
wr addr 0x1b80_4004, wdat=0x0800_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_1140
wr addr 0x1b80_4004, wdat=0x0100_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_1140
wr addr 0x1b80_4004, wdat=0x0200_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_1140
wr addr 0x1b80_4004, wdat=0x0300_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_1140
wr addr 0x1b80_4004, wdat=0x0400_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_1140


// check port0~port4 phy_status=0b011
wr addr 0x1b80_4004, wdat=0x801f_0a42
wr addr 0x1b80_4004, wdat=0x0810_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011
wr addr 0x1b80_4004, wdat=0x0110_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011
wr addr 0x1b80_4004, wdat=0x0210_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011
wr addr 0x1b80_4004, wdat=0x0310_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011
wr addr 0x1b80_4004, wdat=0x0410_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011

// set addr =  maxtimer_mdi10m ~ maxtimer_nwaygiga for new rom code
wr addr 0x1b80_4004, wdat=0x801f_0000
wr addr 0x1b80_4004, wdat=0x801b_81cd

// disable maxtimer
wr addr 0x1b80_4004, wdat=0x801c_ffff
wr addr 0x1b80_4004, wdat=0x801c_ffff

// set addr = mpbist_item_sel
wr addr 0x1b80_4004, wdat=0x801b_8184

// set test item selection (10m/100m/giga)
wr addr 0x1b80_4004, wdat=0x801c_0f00

// set mpbist_en = 1
wr addr 0x1b80_4004, wdat=0x801f_0a47
wr addr 0x1b80_4004, wdat=0x8011_0100

// polling port0~port4 mpbist_en = 0
wr addr 0x1b80_4004, wdat=0x0811_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0
wr addr 0x1b80_4004, wdat=0x0111_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0
wr addr 0x1b80_4004, wdat=0x0211_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0
wr addr 0x1b80_4004, wdat=0x0311_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0
wr addr 0x1b80_4004, wdat=0x0411_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0


// set addr = mpbist_pass
wr addr 0x1b80_4004, wdat=0x801f_0000
wr addr 0x1b80_4004, wdat=0x801b_8186


// check port0~port4 mpbist result (pass)
wr addr 0x1b80_4004, wdat=0x081c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
wr addr 0x1b80_4004, wdat=0x011c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
wr addr 0x1b80_4004, wdat=0x021c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
wr addr 0x1b80_4004, wdat=0x031c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
wr addr 0x1b80_4004, wdat=0x041c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00

// set addr = mpbist_fail
wr addr 0x1b80_4004, wdat=0x801b_8188

// check port0~port4 mpbist result (fail)
wr addr 0x1b80_4004, wdat=0x081c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
wr addr 0x1b80_4004, wdat=0x011c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
wr addr 0x1b80_4004, wdat=0x021c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
wr addr 0x1b80_4004, wdat=0x031c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
wr addr 0x1b80_4004, wdat=0x041c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000


	*/
	volatile unsigned int phyid=4,rdat=0;
	int i;
	int err=0;

	
	// set trxrdy
	REG32(0xbb804204)= 0x00000001;


	// enable force mode
	REG32(0xbb804104)= 0x427f0038;
	REG32(0xbb804108)= 0x467f0038;	
	REG32(0xbb80410c)= 0x4a7f0038;
	REG32(0xbb804110)= 0x4e7f0038;
	REG32(0xbb804114)= 0x527f0038;
	REG32(0xbb804118)= 0x566f0038;	


	// enable phyid0
	REG32(0xbb804004)= 0x88182198;
	REG32(0xbb804004)= 0x81182198;
	REG32(0xbb804004)= 0x82182198;
	REG32(0xbb804004)= 0x83182198;
	REG32(0xbb804004)= 0x84182198;
	
	
	// wr phyid(0), page(0xa40), reg(0x0) -> to disable rg_pwrdn
	REG32(0xbb804004)= 0x801f0a40;
	REG32(0xbb804004)= 0x80001140;

	
	// wr phyid(0), page(0xa46), reg(0x14)-> to disable ext_ini_done and spdup ext_ini_timer(for 100m)
	REG32(0xbb804004)= 0x801f0a46;
	REG32(0xbb804004)= 0x80140003;



	// wr phyid(0), page(0xa4a), reg(0x13)-> to spdup PCS timer, only for simulation
	//wr addr 0x1b80_4004, wdat=0x801f_0a4a
	//wr addr 0x1b80_4004, wdat=0x8013_001f
	// wr phyid(0), page(0xb80), reg(0x17)-> to spdup uC timer, only for simulation
	//wr addr 0x1b80_4004, wdat=0x801f_0b80
	//wr addr 0x1b80_4004, wdat=0x8017_000e
	// check port0~port4 page(0a40)reg(0)'s data
	REG32(0xbb804004)= 0x801f0a40;
	REG32(0xbb804004)= 0x80000000;
	if(REG32(0xbb804008)==0x1140) 	prom_printf("PASS 000\n");
	else 			{ prom_printf("FAIL 000\n");	err++; }

	REG32(0xbb804004)= 0x01000000;
	if(REG32(0xbb804008)==0x1140) 	prom_printf("PASS 001\n");
	else 			{ prom_printf("FAIL 001\n");	err++; }

	REG32(0xbb804004)= 0x02000000;
	if(REG32(0xbb804008)==0x1140) 	prom_printf("PASS 002\n");
	else 			{ prom_printf("FAIL 002\n");	err++; }

	REG32(0xbb804004)= 0x03000000;
	if(REG32(0xbb804008)==0x1140) 	prom_printf("PASS 003\n");
	else 			{ prom_printf("FAIL 003\n");	err++; }

	REG32(0xbb804004)= 0x04000000;
	if(REG32(0xbb804008)==0x1140) 	prom_printf("PASS 004\n");
	else 			{ prom_printf("FAIL 004\n");	err++; }
	

	/* check port0~port4 phy_status=0b011
wr addr 0x1b80_4004, wdat=0x801f_0a42
wr addr 0x1b80_4004, wdat=0x0810_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011

wr addr 0x1b80_4004, wdat=0x0110_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011

wr addr 0x1b80_4004, wdat=0x0210_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011

wr addr 0x1b80_4004, wdat=0x0310_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011

wr addr 0x1b80_4004, wdat=0x0410_0000
rd addr 0x1b80_4008 -> check bit[2:0]=0b011
*/

	REG32(0xbb804004)= 0x801f0a42;
	REG32(0xbb804004)= 0x08100000;
	if(REG32(0xbb804008)&0x3) 	prom_printf("PASS 100\n");
	else 			{ prom_printf("FAIL 100\n");	err++; }

	REG32(0xbb804004)= 0x01100000;
	if(REG32(0xbb804008)&0x3) 	prom_printf("PASS 101\n");
	else 			{ prom_printf("FAIL 101\n");	err++; }

	REG32(0xbb804004)= 0x02100000;
	if(REG32(0xbb804008)&0x3) 	prom_printf("PASS 102\n");
	else 			{ prom_printf("FAIL 102\n");	err++; }

	REG32(0xbb804004)= 0x03100000;
	if(REG32(0xbb804008)&0x3) 	prom_printf("PASS 103\n");
	else 			{ prom_printf("FAIL 103\n");	err++; }
	
	REG32(0xbb804004)= 0x04100000;
	if(REG32(0xbb804008)&0x3) 	prom_printf("PASS 104\n");
	else 			{ prom_printf("FAIL 104\n");	err++; }
	
/*
// set addr =  maxtimer_mdi10m ~ maxtimer_nwaygiga for new rom code
wr addr 0x1b80_4004, wdat=0x801f_0000
wr addr 0x1b80_4004, wdat=0x801b_81cd

// disable maxtimer
wr addr 0x1b80_4004, wdat=0x801c_ffff
wr addr 0x1b80_4004, wdat=0x801c_ffff

// set addr = mpbist_item_sel
wr addr 0x1b80_4004, wdat=0x801b_8184

// set test item selection (10m/100m/giga)
wr addr 0x1b80_4004, wdat=0x801c_0f00

// set mpbist_en = 1
wr addr 0x1b80_4004, wdat=0x801f_0a47
wr addr 0x1b80_4004, wdat=0x8011_0100
*/
	
	REG32(0xbb804004)= 0x801f0000;
	REG32(0xbb804004)= 0x801b81cd;
	
	REG32(0xbb804004)= 0x801cffff;
	REG32(0xbb804004)= 0x801cffff;
	
	REG32(0xbb804004)= 0x801b8184;

	REG32(0xbb804004)= 0x801c0f00;

	REG32(0xbb804004)= 0x801f0a47;
	REG32(0xbb804004)= 0x80110100;
	
/*
// polling port0~port4 mpbist_en = 0
wr addr 0x1b80_4004, wdat=0x0811_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0

wr addr 0x1b80_4004, wdat=0x0111_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0

wr addr 0x1b80_4004, wdat=0x0211_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0

wr addr 0x1b80_4004, wdat=0x0311_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0

wr addr 0x1b80_4004, wdat=0x0411_0000
rd addr 0x1b80_4008 -> check bit[8]=0b0
*/
	REG32(0xbb804004)= 0x08110000;
	while(!REG32(0xbb804008)&(1<<8));

	REG32(0xbb804004)= 0x01110000;
	while(!REG32(0xbb804008)&(1<<8));

	REG32(0xbb804004)= 0x02110000;
	while(!REG32(0xbb804008)&(1<<8));

	REG32(0xbb804004)= 0x03110000;
	while(!REG32(0xbb804008)&(1<<8));

	REG32(0xbb804004)= 0x04110000;
	while(!REG32(0xbb804008)&(1<<8));

	prom_printf("mpbist_pass\n");
	// set addr = mpbist_pass
/*
wref_type addr 0x1b80_4004, wdat=0x801f_0000
wr addr 0x1b80_4004, wdat=0x801b_8186
*/

	REG32(0xbb804004)= 0x801f0000;
	REG32(0xbb804004)= 0x801b8186;


// check port0~port4 mpbist result (pass)
/*
wr addr 0x1b80_4004, wdat=0x081c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
wr addr 0x1b80_4004, wdat=0x011c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
wr addr 0x1b80_4004, wdat=0x021c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
wr addr 0x1b80_4004, wdat=0x031c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
wr addr 0x1b80_4004, wdat=0x041c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0f00
*/
	REG32(0xbb804004)= 0x081c0000;
	if(REG32(0xbb804008)==0x0f00) 	prom_printf("PASS 200\n");
	else 			{ prom_printf("FAIL 200\n");	err++; }


	REG32(0xbb804004)= 0x011c0000;
	if(REG32(0xbb804008)==0x0f00) 	prom_printf("PASS 201\n");
	else 			{ prom_printf("FAIL 201\n");	err++; }


	REG32(0xbb804004)= 0x021c0000;
	if(REG32(0xbb804008)==0x0f00) 	prom_printf("PASS 202\n");
	else 			{ prom_printf("FAIL 202\n");	err++; }


	REG32(0xbb804004)= 0x031c0000;
	if(REG32(0xbb804008)==0x0f00) 	prom_printf("PASS 203\n");
	else 			{ prom_printf("FAIL 203\n");	err++; }


	REG32(0xbb804004)= 0x041c0000;
	if(REG32(0xbb804008)==0x0f00) 	prom_printf("PASS 204\n");
	else 			{ prom_printf("FAIL 204\n");	err++; }
	
/*
// set addr = mpbist_fail
wr addr 0x1b80_4004, wdat=0x801b_8188
*/
	REG32(0xbb804004)= 0x801b8188;

/*
// check port0~port4 mpbist result (fail)
wr addr 0x1b80_4004, wdat=0x081c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
wr addr 0x1b80_4004, wdat=0x011c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
wr addr 0x1b80_4004, wdat=0x021c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
wr addr 0x1b80_4004, wdat=0x031c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
wr addr 0x1b80_4004, wdat=0x041c_0000
rd addr 0x1b80_4008 -> check bit[31:0]=0x0000_0000
*/

	REG32(0xbb804004)= 0x081c0000;
	if(REG32(0xbb804008)==0x0000) 	prom_printf("PASS 300\n");
	else 			{ prom_printf("FAIL 300\n");	err++; }


	REG32(0xbb804004)= 0x011c0000;
	if(REG32(0xbb804008)==0x0000) 	prom_printf("PASS 301\n");
	else 			{ prom_printf("FAIL 301\n");	err++; }


	REG32(0xbb804004)= 0x021c0000;
	if(REG32(0xbb804008)==0x0000) 	prom_printf("PASS 302\n");
	else 			{ prom_printf("FAIL 302\n");	err++; }


	REG32(0xbb804004)= 0x031c0000;
	if(REG32(0xbb804008)==0x0000) 	prom_printf("PASS 303\n");
	else 			{ prom_printf("FAIL 303\n");	err++; }


	REG32(0xbb804004)= 0x041c0000;
	if(REG32(0xbb804008)==0x0000) 	prom_printf("PASS 304\n");
	else 			{ prom_printf("FAIL 304\n");	err++; }
	
	delay_ms(100);
	prom_printf("\n");
	

	
	if(err==0)
	{
		prom_printf("==> GPHY_Openlpbk PASS <== \n");
		return 1;
	}
	else
	{
		prom_printf("==> GPHY_Openlpbk FAIL count=%d <== \n", err);		
		return 0;
	}
}



//=========================================================
int GPHY_BIST_98C()
{
	volatile unsigned int phyid=4,rdat=0;
	int i;
	int err=0;

	prom_printf("Set P0-P4 force mode...... \n");
	REG32(0xbb804104)= 0x427f0038;
	REG32(0xbb804108)= 0x467f0038;
	REG32(0xbb80410c)= 0x4a7f0038;
	REG32(0xbb804110)= 0x4e7f0038;
	REG32(0xbb804114)= 0x527f0038;
	REG32(0xbb804118)= 0x566f0038;	
	
	//
	rtl8651_setAsicEthernetPHYReg(8, 24, 0x2198 );		
	rtl8651_setAsicEthernetPHYReg(1, 24, 0x2198 );	
	rtl8651_setAsicEthernetPHYReg(2, 24, 0x2198 );	
	rtl8651_setAsicEthernetPHYReg(3, 24, 0x2198 );
	rtl8651_setAsicEthernetPHYReg(4, 24, 0x2198 );	
	
	//
	Set_GPHYWB(0, 0xA40, 0, 0, 0x1140);
	Set_GPHYWB(0, 0xA46, 20, 0, 0x0003);	
//	Set_GPHYWB(0, 0xA4A, 19, 0, 0x001f);	
//	Set_GPHYWB(0, 0xB80, 23, 0, 0x000e);	
	
	//
	rtl8651_setAsicEthernetPHYReg(0, 31,0x0a42 );		
	for(i=0;i<5;i++)
	{
		if(i==0) phyid=8;
		rtl8651_getAsicEthernetPHYReg(phyid, 16, &rdat );
		prom_printf("get data=%x\n", rdat);
		if((rdat&0x7)==	0x3) 	prom_printf("Port %d PCS ready PASS\n",i);
		else 			{ prom_printf("Port %d PCS ready FAIL\n",i); err++; }
	}
	
	//3. m3 bist
	Set_GPHYWB(0, 0xc40, 21, 0, 0xc000);
	Set_GPHYWB(0, 0xA00, 20, 0, 0x0000);	
	rtl8651_setAsicEthernetPHYReg(0, 20, 0x0060 );			
	rtl8651_setAsicEthernetPHYReg(0, 23, 0x0000 );	
	rtl8651_setAsicEthernetPHYReg(0, 23, 0x00a0 );	
	Set_GPHYWB(0, 0xb81, 18, 0, 0x0000);
	rtl8651_setAsicEthernetPHYReg(0, 18, 0x001b );					
	Set_GPHYWB(0, 0xc84, 22, 0, 0x0000);	
	rtl8651_setAsicEthernetPHYReg(0, 22, 0x0005 );	
	
	delay_ms(100);
	prom_printf("\n");
	
	for(i=0;i<5;i++)
	{	
		if(i==0) phyid=8;
		else phyid=i;		
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0a00 );
		rtl8651_getAsicEthernetPHYReg(phyid, 23, &rdat );	
		prom_printf("get data=%x\n", rdat);
		if((rdat&(0xf<<9))== (0x8<<9)) 	prom_printf("Port %d BIST PASS\n",i);
		else 			{ prom_printf("Port %d BIST FAIL\n",i); err++; }
			
			
		//	
		rtl8651_getAsicEthernetPHYReg(phyid, 22, &rdat );	
		prom_printf("get data=%x\n", rdat);
		if(rdat== 0xd279) 	prom_printf("Port %d BIST_ROM [31:16] PASS\n",i);
		else 			{ prom_printf("Port %d BIST_ROM [31:16] FAIL\n",i);	err++; }
			
		//
		rtl8651_getAsicEthernetPHYReg(phyid, 21, &rdat );	
		prom_printf("get data=%x\n", rdat);
		if(rdat== 0xa555) 	prom_printf("Port %d BIST_ROM [15:0] PASS\n",i);
		else 			{ prom_printf("Port %d BIST_ROM [15:0] FAIL\n",i);	err++; }
		
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0b81 );
		rtl8651_getAsicEthernetPHYReg(phyid, 18, &rdat );				
		prom_printf("get data=%x\n", rdat);
		if((rdat&(0xf<<12))== (0x1<<12)) 	prom_printf("Port %d GPHY BIST PASS\n",i);
		else 			{		prom_printf("Port %d GPHY BIST FAIL\n",i);	err++; }
			
			
		rtl8651_getAsicEthernetPHYReg(phyid, 19, &rdat );				
		prom_printf("get data=%x\n", rdat);
		if(rdat== 0x2c34) 	prom_printf("Port %d GPHY BIST_ROM PASS\n",i);
		else 			{ prom_printf("Port %d GPHY BIST_ROM FAIL\n",i);	err++; }
			
		rtl8651_setAsicEthernetPHYReg(phyid, 31,0x0c84 );
		rtl8651_getAsicEthernetPHYReg(phyid, 23, &rdat );				
		prom_printf("get data=%x\n", rdat);
		if((rdat&(0x7<<0))== (0x4<<0)) 	prom_printf("Port %d GPHY BIST PASS\n",i);
		else 			{	prom_printf("Port %d GPHY BIST FAIL\n",i);	err++; }
					
						
	}	
	
	if(err==0)
	{
		prom_printf("==> GPHY BIST ALL PASS <== \n");
		return 1;
	}
	else
	{
		prom_printf("==> GPHY BIST FAIL count=%d <== \n", err);		
		return 0;
	}
}

#endif




#if 1		
		
//=============================================================================
int Cmd_AllBistTest98C()
{
	#define REG32(reg)	(*(volatile unsigned int *)(reg))
	int err=0;

	#define CLK_MANAGER  0xb8000010

	#define BIST_CTRL  0xb8000200

	#define HS0_BIST_CTRL  0xb8000208
	#define HS0_BIST_CTRL2  0xb800020c

	#define BIST_DONE  	0xb8000210

	#define HS0_BIST_DONE  0xb8000218


	#define BIST_FAIL  	0xb8000220

	#define HS0_BIST_FAIL1  0xb8000230
	#define HS0_BIST_FAIL2  0xb8000234
	#define HS0_BIST_FAIL3  0xb8000238
	#define HS0_BIST_FAIL4  0xb800023c
	#define HS0_BIST_FAIL5  0xb8000240
	#define HS0_BIST_FAIL6  0xb8000244

	#define DRF_PAUSE   	0xb8000270
	#define HS0_DRF_PAUSE   0xb8000278
	#define DRF_RESUME 	0xb8000280
	#define HS0_DRF_RESUME  0xb8000288
	#define DRF_DONE    	0xb8000290
	#define HS0_DRF_DONE    0xb8000298

	#define DRF_FAIL    	 0xb80002a0
	#define HS0_DRF_FAIL1    0xb80002b0
	#define HS0_DRF_FAIL2    0xb80002b4
	#define HS0_DRF_FAIL3    0xb80002b8
	#define HS0_DRF_FAIL4    0xb80002bc
	#define HS0_DRF_FAIL5    0xb80002c0
	#define HS0_DRF_FAIL6    0xb80002c4


	prom_printf( "========================\n");
	prom_printf( "Mode 1 BIST : cpu1 mbr \n");
	REG32(HS0_BIST_CTRL) = 0;
		prom_printf( "W:HS0_BIST_CTRL=%08x \n", REG32(HS0_BIST_CTRL) );

#if 0		
	REG32(HS0_BIST_CTRL) |=  (0x7f);
#else
	REG32(HS0_BIST_CTRL) |=  (0x7c);  //skip mbr0, mbr1
#endif
		prom_printf( "W:HS0_BIST_CTRL=%08x \n", REG32(HS0_BIST_CTRL) );
	
	REG32(HS0_BIST_CTRL2) = 0;
		prom_printf( "W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
		
	REG32(HS0_BIST_CTRL2) |=  (0x1e0103ff);
		prom_printf( "W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	

	delay_ms(10);

	prom_printf( "\n");
	prom_printf( "R:HS0_BIST DONE=%08x \n",  REG32(HS0_BIST_DONE) );
	prom_printf( "R:HS0_BIST FAIL1=%08x \n", REG32(HS0_BIST_FAIL1) );
	prom_printf( "R:HS0_BIST FAIL2=%08x \n", REG32(HS0_BIST_FAIL2) );
	prom_printf( "R:HS0_BIST FAIL3=%08x \n", REG32(HS0_BIST_FAIL3) );
	prom_printf( "R:HS0_BIST FAIL4=%08x \n", REG32(HS0_BIST_FAIL4) );
	prom_printf( "R:HS0_BIST FAIL5=%08x \n", REG32(HS0_BIST_FAIL5) );
#if 0	
	if ( (REG32(HS0_BIST_DONE)&0x3ff) != 0x3ff) { prom_printf( " ==>DONE FAIL \n"); }
	if ( REG32(HS0_BIST_FAIL1) != 0) { prom_printf( " ==>FAIL1 FAIL \n"); }	
	if ( REG32(HS0_BIST_FAIL2) != 0) { prom_printf( " ==>FAIL2 FAIL \n"); }	
	if ( REG32(HS0_BIST_FAIL3) != 0) { prom_printf( " ==>FAIL3 FAIL \n"); }	
	if ( REG32(HS0_BIST_FAIL4) != 0) { prom_printf( " ==>FAIL4 FAIL \n"); }	
	if ( REG32(HS0_BIST_FAIL5) != 0) { prom_printf( " ==>FAIL5 FAIL \n"); }	

	if ( ((REG32(HS0_BIST_DONE)&0x3ff)==0x3ff) &&
		  (REG32(HS0_BIST_FAIL1)==0)&&
		  (REG32(HS0_BIST_FAIL2)==0)&&
		  (REG32(HS0_BIST_FAIL3)==0)&&
		  (REG32(HS0_BIST_FAIL4)==0)&&
		  (REG32(HS0_BIST_FAIL5)==0)  )
		  { prom_printf( " ==>BIST PASS \n"); }	 
#endif		
	//============================================
	
	prom_printf( "========================\n");
	prom_printf( "Mode 2 BIST : L2,SRAM,ROM,CPU2 \n");

	prom_printf( "R:HS0_BIST FAIL6=%08x \n", REG32(HS0_BIST_FAIL6));

	if ( (REG32(HS0_BIST_DONE)&(0x79<<10)) != (0x79<<10)) { prom_printf( " ==>DONE FAIL \n"); }	
	if (  REG32(HS0_BIST_FAIL6) != 0) { prom_printf( " ==>FAIL FAIL \n"); }	

	if ((( REG32(HS0_BIST_DONE)&(0x79<<10)) == (0x79<<10)) && 
		 ( REG32(HS0_BIST_FAIL6) == 0) )
	{ prom_printf( " ==>BIST PASS \n"); }
	else
	{ prom_printf( " ==>BIST FAIL \n"); err++; }		
	//============================================
	prom_printf( "========================\n");
	prom_printf( "Mode 3 BIST : NAND,FFT,SATA, PCS ROM, PCS RAM, USB3, OTG, PCIE(ep10),VOIP\n");
	REG32(BIST_CTRL) = 0;
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL));
			
	REG32(BIST_CTRL) |=  (0x01);
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL));

	REG32(BIST_CTRL) |= (0x03ff0001);
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL   )); 

	delay_ms(10);
	prom_printf( "\n");
	prom_printf( "R:BIST DONE=%08x \n", REG32(BIST_DONE));
	prom_printf( "R:BIST FAIL=%08x \n", REG32(BIST_FAIL));


	
	if ( (REG32(BIST_DONE) == 0xffff3fff) && 
		( REG32(BIST_FAIL) == 0x00000000)) { prom_printf( " ==>BIST PASS \n"); }	 
	else 								   
	{ 	prom_printf( " ==>BIST FAIL \n"); err++;
		if ( REG32(BIST_DONE) != 0xffff3fff) { prom_printf( " ==>DONE FAIL \n"); }
		if ( REG32(BIST_FAIL) != 0x00000000) { prom_printf( " ==>FAIL FAIL \n"); }	
	}	
		
	//============================================
	prom_printf( "============================== \n");
	prom_printf( "Mode 4 BIST :switch\n");
	REG32(BIST_CTRL) = 0;
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );	
	REG32(BIST_CTRL) |=  (0x01);
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );
	REG32(BIST_CTRL) |=  (1<<26);
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );
	    
	delay_ms(10);  
	prom_printf( "\n");
	prom_printf( "R:BIST DONE=%08x \n", REG32(BIST_DONE));
	prom_printf( "R:BIST FAIL=%08x \n", REG32(BIST_FAIL));


	
	if ( (REG32(BIST_DONE) == 0xffff4418) && 
		( REG32(BIST_FAIL) == 0x00000000)) { prom_printf( " ==>BIST PASS \n"); }	 
	else								   
	{ 	prom_printf( " ==>BIST FAIL \n"); err++;
		if ( REG32(BIST_DONE) != 0xffff4418) { prom_printf( " ==>DONE FAIL \n"); }
		if ( REG32(BIST_FAIL) != 0x00000000) { prom_printf( " ==>FAIL FAIL \n"); }	
	}	 
	//============================================
	prom_printf( "============================== \n");
	prom_printf( "Mode 5 BIST :switch bist-r \n");
	REG32(BIST_CTRL) = 0;
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );		
	REG32(BIST_CTRL) |=  (0x01);
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );
	REG32(BIST_CTRL) |= (1<<27);
		prom_printf( "W:BIST_CTRL=%08x \n", REG32(BIST_CTRL) );
	    
	delay_ms(10);   
	prom_printf( "\n");
	prom_printf( "R:BIST DONE=%08x \n", REG32(BIST_DONE) );
	prom_printf( "R:BIST FAIL=%08x \n", REG32(BIST_FAIL) );


	
	if ( (REG32(BIST_DONE) == 0xffff8418) && ( REG32(BIST_FAIL) == 0x00000000)) { prom_printf( " ==>BIST PASS \n"); }	 
	else 							
	{ 	prom_printf( " ==>BIST FAIL \n");  err++;
		if ( REG32(BIST_DONE) != 0xffff8418) { prom_printf( " ==>DONE FAIL \n"); }
		if ( REG32(BIST_FAIL) != 0x00000000) { prom_printf( " ==>FAIL FAIL \n"); }	
	}	 

	//goto end
	//============================================
#if 0	//skip mbr drf
	prom_printf( "============================== \n");
	prom_printf( "Mode 6 DRF_BIST TEST : mbr \n");
	prom_printf( "1.bist_r \n");
	REG32(HS0_BIST_CTRL2) = 0;
		prom_printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	REG32(HS0_BIST_CTRL2) |=  (0x00060000);
		prom_printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	delay_ms(10);
	prom_printf( "\n");
	prom_printf( " R:HS0_BIST_DONE=%08x \n", REG32(HS0_BIST_DONE));
	if ( REG32(HS0_BIST_DONE) == 0xffff1800) { prom_printf( " ==>DONE PASS \n"); }
	if ( REG32(HS0_BIST_DONE) != 0xffff1800) { prom_printf( " ==>DONE FAIL \n"); }	

	prom_printf( "2.second run \n");

		prom_printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	REG32(HS0_BIST_CTRL2) = 0x00180000;
		prom_printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	REG32(HS0_BIST_CTRL2) = 0x001e0000;
		prom_printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );    
	    
	delay_ms(10);
	prom_printf( "\n");
	prom_printf( " R:HS0_BIST_DONE=%08x \n", REG32(HS0_BIST_DONE));
	if ( REG32(HS0_BIST_DONE) == 0xffff1800) { prom_printf( " ==>DONE PASS \n"); }
	if ( REG32(HS0_BIST_DONE) != 0xffff1800) { prom_printf( " ==>DONE FAIL \n"); }	

	prom_printf( " R:HS0_BIST_FAIL6=%08x \n", REG32(HS0_BIST_FAIL6));
	//prom_printf( "R:HS0_BIST_FAIL6[12:5]=%08x \n", REG32(HS0_BIST_FAIL6
	if ( (REG32(HS0_BIST_DONE) == 0xffff1800) && ( REG32(HS0_BIST_FAIL6) == 0)) 
		{ prom_printf( " ==>BIST PASS \n"); }	 
	//-------------
	prom_printf( "3.remap\n");
	REG32(HS0_BIST_CTRL2) = 0x00600000;
		prom_printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) );
	REG32(HS0_BIST_CTRL2) = 0x00ff0000;
		prom_printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) ); 
	REG32(HS0_BIST_CTRL2) = 0x0e6703ff;
		prom_printf( " W:HS0_BIST_CTRL2=%08x \n", REG32(HS0_BIST_CTRL2) ); 
	    
	prom_printf( "4.pause\n");    
	prom_printf( " R:HS0_DRF_PAUSE=%08x \n", REG32(HS0_DRF_PAUSE ));   
	if ( REG32(HS0_DRF_PAUSE) == 0xffffffff) { prom_printf( " ==>PAUSE PASS \n"); }
	if ( REG32(HS0_DRF_PAUSE) != 0xffffffff) { prom_printf( " ==>PAUSE FAIL \n"); }

	prom_printf( "5.resume\n");   
	REG32(HS0_DRF_RESUME) = 0xffffffff;
		prom_printf( " W:HS0_DRF_RESUME=%08x \n", REG32(HS0_DRF_RESUME) );   

	prom_printf( "6.two pause\n");      
	prom_printf( "R:HS0_DRF_PAUSE=%08x \n", REG32(HS0_DRF_PAUSE ));     
	if ( REG32(HS0_DRF_PAUSE) == 0xffffffff) { prom_printf( " ==>PAUSE PASS \n"); }
	if ( REG32(HS0_DRF_PAUSE) != 0xffffffff) { prom_printf( " ==>PAUSE FAIL \n"); }
	    
	prom_printf( "7.two pause\n");      
	prom_printf( " R:HS0_DRF_DONE=%08x \n", REG32(HS0_DRF_DONE ) );
	prom_printf( " R:HS0_DRF_FAIL1=%08x \n", REG32(HS0_DRF_FAIL1) );
	prom_printf( " R:HS0_DRF_FAIL2=%08x \n", REG32(HS0_DRF_FAIL2) );
	prom_printf( " R:HS0_DRF_FAIL3=%08x \n", REG32(HS0_DRF_FAIL3) );
	prom_printf( " R:HS0_DRF_FAIL4=%08x \n", REG32(HS0_DRF_FAIL4) );
	prom_printf( " R:HS0_DRF_FAIL5=%08x \n", REG32(HS0_DRF_FAIL5) );
	prom_printf( " R:HS0_DRF_FAIL6=%08x \n", REG32(HS0_DRF_FAIL6) );

	if ( REG32(HS0_DRF_DONE) == 0xffffffff) { prom_printf( " ==>DRF DONE PASS \n"); }
	if ( REG32(HS0_DRF_DONE) != 0xffffffff) { prom_printf( " ==>DRF DONE FAIL \n"); }
	    
	if ( REG32(HS0_DRF_FAIL1) != 0) { prom_printf( " ==>FAIL1 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL2) != 0) { prom_printf( " ==>FAIL2 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL3) != 0) { prom_printf( " ==>FAIL3 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL4) != 0) { prom_printf( " ==>FAIL4 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL5) != 0) { prom_printf( " ==>FAIL5 FAIL \n"); }	
	if ( REG32(HS0_DRF_FAIL6) != 0) { prom_printf( " ==>FAIL6 FAIL \n"); }

	if ( (REG32(HS0_DRF_DONE)==0xffffffff) &&
		(REG32(HS0_DRF_FAIL1)==0)&&
		(REG32(HS0_DRF_FAIL2)==0)&&
		(REG32(HS0_DRF_FAIL3)==0)&&
		(REG32(HS0_DRF_FAIL4)==0)&&
		(REG32(HS0_DRF_FAIL5)==0)&&
		(REG32(HS0_DRF_FAIL6)==0)  )
		{ prom_printf( " ==>DRF PASS \n"); }	 
	prom_printf( "============================== \n");	    
#endif
		
		
	prom_printf( "============================== \n");
	prom_printf( "Mode x DRF_BIST TEST : IP \n");
	REG32( 0xb800020c)=0x001e0000;
	REG32( 0xb8000200)=0x00000002;
//	REG32( 0xb8000208)=0x007f007f;   //skip mbr
	//
	REG32( 0xb8000200)=0x0fff0002;


	prom_printf( "4.pause\n");    
	prom_printf( " R:DRF_PAUSE=%08x \n", REG32(DRF_PAUSE    ));
//	if ( REG32(DRF_PAUSE) == 0xffffffff) { prom_printf( " ==>PAUSE PASS \n"); }
//	if ( REG32(DRF_PAUSE) != 0xffffffff) { prom_printf( " ==>PAUSE FAIL \n"); }
#if 0	//skip mbr     
	prom_printf( " R:HS0_DRF_PAUSE=%08x \n", REG32(HS0_DRF_PAUSE ));
	if ( REG32(HS0_DRF_PAUSE) == 0xffffffff) { prom_printf( " ==>PAUSE PASS \n"); }
	if ( REG32(HS0_DRF_PAUSE) != 0xffffffff) { prom_printf( " ==>PAUSE FAIL \n"); }
#endif


	prom_printf( "5.resume\n");   
	REG32(DRF_RESUME) = 0xffffffff;
		prom_printf( " W:DRF_RESUME=%08x \n", REG32(DRF_RESUME) );    

	prom_printf( "6.two pause\n");      
	prom_printf( " R:DRF_PAUSE=%08x \n", REG32(DRF_PAUSE) );    
//	if ( REG32(DRF_PAUSE) == 0xffffffff) { prom_printf( " ==>PAUSE PASS \n"); }
//	if ( REG32(DRF_PAUSE) != 0xffffffff) { prom_printf( " ==>PAUSE FAIL \n"); }
	
#if 0 //skip mbr	 
	prom_printf( " R:HS0_DRF_PAUSE=%08x \n", REG32(HS0_DRF_PAUSE ));
	if ( REG32(HS0_DRF_PAUSE) == 0xffffffff) { prom_printf( " ==>PAUSE PASS \n"); }
	if ( REG32(HS0_DRF_PAUSE) != 0xffffffff) { prom_printf( " ==>PAUSE FAIL \n"); }
#endif	    
	prom_printf( "7.two resume\n");    
	REG32(DRF_RESUME) = 0xffffffff;
		prom_printf( " W:DRF_RESUME=%08x \n", REG32(DRF_RESUME ) ); 
	  
	prom_printf( " R:DRF_DONE=%08x \n", REG32(DRF_DONE) );
	prom_printf( " R:DRF_FAIL=%08x \n", REG32(DRF_FAIL) );



	if ( (REG32(DRF_DONE)==0xffffffff) &&( REG32(DRF_FAIL)==0) ) 
		{ prom_printf( " ==>DRF PASS \n"); }	 
	else	
	{ 
		prom_printf( " ==>DRF FAIL \n");  err++;
		if ( REG32(DRF_DONE) != 0xffffffff) { prom_printf( " ==>DRF DONE FAIL \n"); }	    
		if ( REG32(DRF_FAIL) != 0) { prom_printf( " ==>FAIL FAIL \n"); }		
	}	


	if(err==0)
	{
		prom_printf("==> IP BIST ALL PASS <== return 1\n");
		return 1;
	}
	else
	{
		prom_printf("==> IP BIST FAIL count=%d <== return 0 \n", err);	
		return 0;
	}

}
#endif


int RTL8196D_FT2_TEST_GPIOinit()
{

	 #define GPIO_B1_1 REG32(PABCDDAT_REG)|=0x200     //Output "1"
	 #define GPIO_B1_0 REG32(PABCDDAT_REG)&=0xFFFFFDFF//Output "0"
	REG32(0xb8000104)|=(3<<20);//RTL8198C GPIO_B1 (PCIE_RSTN) trigger , pin mux setting of GPIO
	REG32(0xb8000108)|=(3<<9);//RTL8198C GPIO_B5 (LEDSIG3) trigger , pin mux setting of GPIO

	//prom_printf("\nRTL8198C FT2 GPIO init \n");

     //prom_printf("\nRTL8196D_FT2_TEST_GPIOinit()\n");
#if 1 //RTL8196D_FT2_TEST_entry GPIO init

                 //Set 8196D shared pin to GPIO function
   // REG32(0xb8000040) |= 0x40;                       //PCIE reset pin acts as GPIO
   // REG32(0xb8000044)|=0x600;                        //LEDSIG3 acts as GPIO



                 //GPIO negotiate with TESTER
                 /*GPIO configuration

                 1.Input test-item pin:
                 CTRL[0]=  LEDSIGN[3] =  GPIO_B5

                 2.Output test-result pin:
                 RESULT[0]= PCIE_RSTN = GPIO_B1
                 */

                 //GPIO ABCD Input init =0 = GPIO pin
    REG32(PABCDCNR_REG) = 0x0;

                 //[31:24]=D[7:0] , [23:16]=C[7:0] , [15:8]=B[7:0] , [7:0]=A[7:0] ,"0"=input pin , "1"=Output pin
    REG32(PABCDDIR_REG) |= 0x200;                    //set B[1]=1 = Output pin
    REG32(PABCDDIR_REG) &= 0xFFFFdFFF;               //set B[5]=0 = input pin

    //REG32(PABIMR_REG)= 0x08000000;                   //Enable GPIO_B5 rising edge interript
    #if 1
    	REG32(PABIMR_REG)= 0x0C000000;                   //Enable GPIO_B5 falling rising edge interript
    #endif
    REG32(PABCDISR_REG) |=0xffffffff;                //Write clear once as default

#endif
}



#ifdef CONFIG_RTL8198C_FT2_TEST
int RTL8196D_FT2_GPIO_low_loop()
{
	
		  GPIO_B1_0;
		    delay_ms(DLY_8196D_FT2_TIME);            //Keep test result 10ms , it's a must  
		   REG32(PABCDISR_REG) |=0xffffffff;                //Write clear once

		GPIO_B1_0_LOW_LOOP:
		   if(REG32(PABCDISR_REG) &0x2000)
		   {
			
		   }
		   else
		   {
			 prom_printf("\nGPIO_B5 falling edge interrupt happened, GPIO_B1_0 => low\n" );
			   GPIO_B1_0;
			  
			   goto GPIO_B1_0_LOW_LOOP; 
		   }
}





int RTL8198C_FT2_TEST_entry()
{

   prom_printf("\n===8198C FT2 Begin===\n" );
   prom_printf("\nSW Version: 20140113_FT2\n" );

    prom_printf("\nIC Information:\n" );
    show_board_info();	
   // prom_printf("\n1.ECO_NO_REG=0x%x\n",REG32(ECO_NO_REG) );
	
   // int bond_option_value=REG32(Bond_Option_REG);
    //bond_option_value=13; //Test specific bonding flow
    //prom_printf("\n2.BOND_OPTION=0x%x\n",bond_option_value );

  
   prom_printf("\n3.Chip ID and cut version:\n" );
   check_chip_id_code();

	
    prom_printf("\n===============================================\n" );

    unsigned short FT1_loop_cnt=7;
    unsigned short cnt;

	/*FT1_Exit_Cnt_to_TFTP x DLY_8196D_FT2_TIME= TFTP exit left time*/
    unsigned short FT1_Exit_Cnt_to_TFTP=300;  
    //unsigned int GPIO_B5_IN,GPIO_B1_OUT;
    unsigned int First_GPIO_Ignore=1;
    //prom_printf("\nDefault PABCDISR_REG=0x%x\n",REG32(PABCDISR_REG));

	prom_printf("\n1st FT2 GPIO init \n");
	RTL8196D_FT2_TEST_GPIOinit();   

	
	
    while(FT1_loop_cnt)                              //test loop
    //for(cnt=FT1_loop_cnt;cnt <0;cnt--)
    {
	
	  
        prom_printf("\n================================\n" );
        prom_printf("\nFT1_loop_cnt=%d\n",FT1_loop_cnt);
        prom_printf("\n================================\n" );

        switch(FT1_loop_cnt)
        {          
	      
            case 7:		
		
                while(1)
                {
                 //show IC version		 
		 // prom_printf("\n PABCDISR_REG=0x%x\n",REG32(PABCDISR_REG));                  	
		  if(First_GPIO_Ignore)
		  {		  		  
			    	     GPIO_B1_1;                                       
		 		     First_GPIO_Ignore=0;
				     delay_ms(10);
				     REG32(PABCDISR_REG) |=0xffffffff;                //Clear 1st GPIO init INT
				     delay_ms(10);
				     REG32(PABCDISR_REG) |=0xffffffff;                 //Clear 1st GPIO init INT
				     delay_ms(10);
				     REG32(PABCDISR_REG) |=0xffffffff;                 //Clear 1st GPIO init INT
		  }
			
                    if(REG32(PABCDISR_REG) &0x2000)
                    {

			 RTL8196D_FT2_GPIO_low_loop();
					
                 //prom_printf("\nGo 8196D FT1 test!\n" );
                        prom_printf("\nFT1 case%d: Show IC version\n",FT1_loop_cnt );
                        REG32(PABCDISR_REG) |=0x2000;//Write clear

                        int ID_value=REG32(ECO_NO_REG);
			   
                        switch (ID_value)
                        {
                            prom_printf("\n\n" );
                            case 0x8198c000:         //show "01-0001"means test chip
                          
				#if 1//def CONFIG_RTL8196E				
				  prom_printf("\n{A cut} , ID=0x%x,\n",ID_value );
				
                                //prom_printf("\n{TestChip} , ID=0x%x,\n",ID_value );
				#endif
                                prom_printf("\nLED=>{0-1-0-0-0-0}\n" );

                                GPIO_B1_0;
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;   
                                delay_ms(DLY_8196D_FT2_TIME);

                                goto NEXT_FT2_CASE;

                            case 0x8198c001:         //show "01-0001"means A cut chip
                          

		             	#if 1				
                                 prom_printf("\n{B-cut} , ID=0x%x,\n",ID_value );
				#endif
                              
                                prom_printf("\nLED=>{0-1-0-0-0-1}\n" );

                                GPIO_B1_0;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;
                                delay_ms(DLY_8196D_FT2_TIME);

                                goto NEXT_FT2_CASE;

                            case 0x8198c002:         //show "01-0011"means B cut chip
                            
                            #if 1//def CONFIG_RTL8196E				
                                 prom_printf("\n{C-cut} , ID=0x%x,\n",ID_value );
				#endif
                                prom_printf("\nLED=>{0-1-0-0-1-0}\n" );

                                GPIO_B1_0;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;
                                delay_ms(DLY_8196D_FT2_TIME);

                                goto NEXT_FT2_CASE;

                             case 0x8198c003:
                            #if 1//def CONFIG_RTL8196E
				
                                 prom_printf("\n{D-cut} , ID=0x%x,\n",ID_value );
				#endif
                                prom_printf("\nLED =>{0-1-0-0-1-1}\n" );

                         	   GPIO_B1_0;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;
                                delay_ms(DLY_8196D_FT2_TIME);


                                goto NEXT_FT2_CASE;


				         case 0x8881a004:
                            #if 1//def CONFIG_RTL8196E
				
                                 prom_printf("\n{E-cut} , ID=0x%x,\n",ID_value );
				#endif
                                prom_printf("\nLED =>{0-1-0-1-0-0}\n" );

                         	   GPIO_B1_0;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_1;           
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;          
                                delay_ms(DLY_8196D_FT2_TIME);
								
                                GPIO_B1_0;
                                delay_ms(DLY_8196D_FT2_TIME);


                                goto NEXT_FT2_CASE;

                            default:
                 ///show IC version
                                prom_printf("\nUnknown or Error ID=0x%x\n",ID_value );
				      #if CONF_FT2_FAIL_STOP_TEST
                   			     goto  FT2_Fail;
		    		      #else
					     GPIO_B1_0;
		     		     #endif

                        }                            //end of ID switch case

                    }
                    else                             //JSW: Exit FT1
                    {             

			 //prom_printf("\n000-PABCDISR_REG(0x%x)=0x%x\n",PABCDISR_REG,REG32(PABCDISR_REG) );	
                    	//delay_ms(DLY_8196D_FT2_TIME);    //Keep test result =100ms	

		

			// prom_printf("\n001-PABCDISR_REG(0x%x)=0x%x\n",PABCDISR_REG,REG32(PABCDISR_REG) );	
			// prom_printf("(0xb8000040)=%x\n", REG32( 0xb8000040 ) );
			//prom_printf("(0xb8000044)=%x\n", REG32( 0xb8000044 ) );
		 	 //if(REG32(PABCDISR_REG) &0x2000)

			 //  if((FT1_Exit_Cnt_to_TFTP%100==0))
			  // {
                        	prom_printf("\n\nWaiting Tester start signal (No GPIO_B5 rising INT)\n");
                        	prom_printf("\n...%d\n",FT1_Exit_Cnt_to_TFTP);
			 //  }
                        FT1_Exit_Cnt_to_TFTP--;
                        if(!FT1_Exit_Cnt_to_TFTP)
                        {
                        	prom_printf("\n\n\n===>  Time over, FT2 Bye\n\n\n\n\n",FT1_Exit_Cnt_to_TFTP);
                            return;
                        }

                    }
                }                                    //end of while(1)


	  case 6:		 
		  
                prom_printf("\nFT2 case%d: P0_MII_LBK_FT2\n",FT1_loop_cnt );	
		#if 0//ndef CONFIG_RTL8198C_P0_MII_LBK_FT2		
			prom_printf("\nSkipped!\n" );
		 	FT1_loop_cnt--;
			continue; 
		
		#endif

	
		  //eth_startup(1);		  
		  eth_startup(0);		 
		  
                if(FT_P0_MII_GMII(0))
                {
                    GPIO_B1_1;		     
                }
                else
                {
                	 #if CONF_FT2_FAIL_STOP_TEST
                   		goto  FT2_Fail;
		    	 #else
				GPIO_B1_0;
		     	#endif
                }
                break;


	case 5:
		
		 prom_printf("\nFT2 case%d : GPHY_Openlpbk \n",FT1_loop_cnt );
	#if 0//
		 prom_printf("\nSkipped!\n" );
		 FT1_loop_cnt--;
		continue; 
	#else
		 //FT1_loop_cnt--;
		  eth_startup(0);
		 if(GPHY_Openlpbk())
                {
                    GPIO_B1_1;                       //Output "1",means result pass
                    prom_printf("\n{GPHY_Openlpbk Pass}\n");			
                 //prom_printf("\nbist_done(0xb8000020)=0x%x\n",REG32(0xb8000020));
                 //prom_printf("\nbist_done(0xb8000024)=0x%x\n",REG32(0xb8000024));
                }
                else
                {

                    prom_printf("\n{GPHY_Openlpbk Fail}\n");
                 //prom_printf("\nbist_done(0xb8000020)=0x%x\n",REG32(0xb8000020));
                 //prom_printf("\nbist_done(0xb8000024)=0x%x\n",REG32(0xb8000024));

		
                    #if CONF_FT2_FAIL_STOP_TEST
                       goto  FT2_Fail;
		      #else
		         GPIO_B1_0;
		      #endif
             
                }
		    goto NEXT_FT2_CASE;
	
	#endif

	 case 4:
		 #if 0//def CONFIG_RTL8196D		  
			prom_printf("\n8196D FT2 bond_option_value=0x%x \n", bond_option_value);
			if(bond_option_value==13)  //20110103:RTL8196DS-VA-CG QFN88 only RGMII I/F ,so don't test MLT3
			{
				 FT1_loop_cnt--;
				continue; 
			}				
		#endif		

		
                prom_printf("\nFT2 case%d : MLT3 , 2 items\n",FT1_loop_cnt );	
		  eth_startup(0); //Pre init for MLT3
		   
		  //REG32(PCRP0)=0x007f0039 ; //set 8196D P0 to embedded phy mode		
                mlt3_dc();

		  #if 0
		  	//break;
		  	  goto NEXT_FT2_CASE;
		  #else
		  	 /*without trigger signal*/
		  	 FT1_loop_cnt--;
		  	goto CASE_3;
		  #endif

		  
            case 3:
		 CASE_3:
		 prom_printf("\nFT2 case%d : GHY BIST \n",FT1_loop_cnt );
	#if 0//CONF_SKIP_BIST_HV
		 prom_printf("\nSkipped!\n" );
		 FT1_loop_cnt--;
		continue; //20111006: Elvis agrees only perform LV-BIST
	#else
		 //FT1_loop_cnt--;
		  eth_startup(0);
		 if(GPHY_BIST_98C())
                {
                    GPIO_B1_1;                       //Output "1",means result pass
                    prom_printf("\n{GPHY BIST Pass}\n");			
                 //prom_printf("\nbist_done(0xb8000020)=0x%x\n",REG32(0xb8000020));
                 //prom_printf("\nbist_done(0xb8000024)=0x%x\n",REG32(0xb8000024));
                }
                else
                {

                    prom_printf("\n{GPHY BIST Fail}\n");
                 //prom_printf("\nbist_done(0xb8000020)=0x%x\n",REG32(0xb8000020));
                 //prom_printf("\nbist_done(0xb8000024)=0x%x\n",REG32(0xb8000024));

		
                    #if CONF_FT2_FAIL_STOP_TEST
                       goto  FT2_Fail;
		      #else
		         GPIO_B1_0;
		      #endif
             
                }
		    goto NEXT_FT2_CASE;
	
	#endif
            
             
				
            case 2:
		 prom_printf("\nFT2 case%d : AllBistTest98C\n",FT1_loop_cnt );
	#if 0//CONF_SKIP_BIST_HV

	 	prom_printf("\nSkipped!\n" );
		FT1_loop_cnt--;
		continue; //20111006: Elvis agrees only perform LV-BIST
	#else
		// prom_printf("\nSkipped!\n" );
		 //FT1_loop_cnt--;
		   // if(((*(volatile unsigned int*)(0xb8000020)==0x0FF5FE98)&&(*(volatile unsigned int*)(0xb8000024)==0x0)) \ 
                 //    ||((*(volatile unsigned int*)(0xb8000020)==0x0FF5BE98)&&(*(volatile unsigned int*)(0xb8000024)==0x0)) )
                if(Cmd_AllBistTest98C())
                {
                    GPIO_B1_1;                       //Output "1",means result pass
                    prom_printf("\n{IP BIST Pass}\n");			
                 //prom_printf("\nbist_done(0xb8000020)=0x%x\n",REG32(0xb8000020));
                 //prom_printf("\nbist_done(0xb8000024)=0x%x\n",REG32(0xb8000024));
                }
                else
                {

                    prom_printf("\n{IP BIST Fail}\n");
                 //prom_printf("\nbist_done(0xb8000020)=0x%x\n",REG32(0xb8000020));
                 //prom_printf("\nbist_done(0xb8000024)=0x%x\n",REG32(0xb8000024));

		
                    #if CONF_FT2_FAIL_STOP_TEST
                       goto  FT2_Fail;
		      #else
		         GPIO_B1_0;
		      #endif
             
                }

                  goto NEXT_FT2_CASE;
				
	#endif
			


            case 1:
		  //JUMP_CASE_1:
		 	 
		  prom_printf("\nFT2 case%d: CKGEN\n",FT1_loop_cnt );
		  GPIO_B1_1;
		  #if 1
		  	//REG32(0xb8000094)=0x000000C0; //set debug bus select//RTL819xD
		  	//REG32(0xb8000094)=0x000000E0; //set debug bus select//RTL8881A
		  	REG32(0xb8000094)=0x000000f0; //set debug bus select//RTL8198C

			#if 0
			//set PIN_MUX_SEL2  LEDSIG3=DBG ,bit[10:9]=10 //RTL8881A
		  	REG32(0xb8000044)=0xFFFFFDFF; //bit9=0
		  	REG32(0xb8000044)|=0x400; //bit10=1
		  	#else
			//set PIN_MUX_SEL2  LEDSIG3=DBG ,bit[10:9]=10 //RTL8198C
		  	REG32(0xb8000108)&=~(7<<9);//bit9=0
		  	REG32(0xb8000108)|=(1<<10); //bit10=1
				
			#endif
		  	

		  #endif                
                
              
		  prom_printf("\n\n ==> Please measure LEDSIG3's freq should be CPU-Freq/32!\n" );
		  //prom_printf("\nEX1: CPU-Freq 500MHZ/32=15.625MHZ\n\n" );
		  prom_printf("\nEX2: CPU-Freq 800MHZ/32=25MHZ\n\n" );
		  //prom_printf("\nEX3: CPU-Freq 660MHZ/32=20.625MHZ\n\n" );
		 prom_printf("\nDelay 100 ms for CKGEN measurement\n" );
		 delay_ms(1000*10);            //delay 100 msecs for tester measurement
		
		  prom_printf("\nExit, SoC FT2 done\n" );
		    goto FT2_8196D_DONE;
		   //goto NEXT_FT2_CASE;

            default:
                prom_printf("\nError FT2 Input case,your input= %d\n", FT1_loop_cnt);
                goto FT2_8196D_DONE;

        }

        NEXT_FT2_CASE:
        while(1)
        {
             //RTL8196D_FT2_TEST_GPIOinit();
           // delay_ms(DLY_8196D_FT2_TIME);            //Keep test result 10ms , it's a must  
            if(REG32(PABCDISR_REG) &0x2000)
            {

		   RTL8196D_FT2_GPIO_low_loop();
			
                 //prom_printf("\nDefault PABCDISR_REG=0x%x\n",REG32(PABCDISR_REG));
                prom_printf("\nGPIO_B5 rising edge interrupt happened,go to next test case\n" );
                FT1_loop_cnt--;
                //REG32(PABCDISR_REG) |=0x2000;        //Write clear
                REG32(PABCDISR_REG) |=0xffffffff;                //Write clear once
                break;
            }
            else
            {
                 //prom_printf("\nPABCDDIR_REG=0x%x\n",REG32(PABCDDIR_REG));
                 //prom_printf("\nPABCDDAT_REG=0x%x\n",REG32(PABCDDAT_REG));
                 //prom_printf("\nNo GPIO_B5 rising INT,PABCDISR_REG=0x%x\n",REG32(PABCDISR_REG));
                prom_printf("\nNo GPIO_B5 rising interrupt=> Idle \n");
               
            }
        }

              
    }                                                //end of while loop(FT1_loop_cnt)

    FT2_8196D_DONE:

    //prom_printf("\n\n==========Wireless LAN FT2 start===============\n");
  //#define CONFIG_WL_MP_SUPPORT
    //WlCtrlOperater(1);
    //WlCtrlOperater();

	
   // prom_printf("\n\n==========Wireless LAN FT2 end===============\n");   	
    return;
  
  

#if 1
    FT2_Fail:
    GPIO_B1_0;                                       //Output "0" ,means result fail
    prom_printf("\n{Fail} => EXIT!\n\n");
    //REG32(PCRP0)=0x007f0039 ; //set 8196D P0 to embedded phy mode
    //delay_ms(DLY_8196D_FT2_TIME);
    //return;
#endif

}                                                    //end of RTL8196D_FT1_TEST_entry


#endif


//USB phy R/W
#if 0//defined(CONFIG_RTL_8196C)
void SetUSBPhy(unsigned char reg, unsigned char val)
{
  prom_printf("SetUSBPhy(),reg=%08x,val=%08x\n", reg,val);  
 #define USB2_PHY_DELAY {delay_ms(50);}
 //8196C demo board: 0xE0:99, 0xE1:A8, 0xE2:98, 0xE3:C1,  0xE5:91,  
#if 0//!CONFIG_RTL8196D //8198 
 REG32(0xb8000034) = (0x1f00 | val); USB2_PHY_DELAY;
#else  //8196D
 #define SYS_USB_SIE 0xb8000034
 #define SYS_USB_PHY 0xb8000090  
  int oneportsel=(REG32(SYS_USB_SIE) & (1<<18))>>18;


oneportsel=1;
 
 unsigned int tmp = REG32(SYS_USB_PHY);  //8672 only 
 tmp = tmp & ~((0xff<<11)|(0xff<<0));
 
 
 //if(oneportsel==0)
 { //REG32(SYS_USB_PHY) = (val << 0) | tmp;   //phy 0
 }
 //else
 {REG32(SYS_USB_PHY) = (val << 11) | tmp;  //phy1
 }
 
 USB2_PHY_DELAY;
#endif
 //printk("0xb8000034=%08x\n", REG32(0xb8000034));  
 
 unsigned char reg_h=(reg &0xf0)>>4;
 unsigned char reg_l=(reg &0x0f);
  
 delay_ms(100); 
 REG32(0xb80210A4) = (0x00300000 | (reg_l<<16)); USB2_PHY_DELAY;
 REG32(0xb80210A4) = (0x00200000 | (reg_l<<16)); USB2_PHY_DELAY; 
 REG32(0xb80210A4) = (0x00300000 | (reg_l<<16)); USB2_PHY_DELAY; 
 REG32(0xb80210A4) = (0x00300000 | (reg_h<<16)); USB2_PHY_DELAY; 
 REG32(0xb80210A4) = (0x00200000 | (reg_h<<16)); USB2_PHY_DELAY; 
 REG32(0xb80210A4) = (0x00300000 | (reg_h<<16)); USB2_PHY_DELAY;
 
}


 
unsigned char GetUSBPhy(unsigned char reg)
{
 #define USB2_PHY_DELAY {delay_ms(50);}
 
 unsigned char reg_h=((reg &0xf0)>>4)-2;
 unsigned char reg_l=(reg &0x0f);
  
 REG32(0xb80210A4) = (0x00300000 | (reg_l<<16)); USB2_PHY_DELAY;
 REG32(0xb80210A4) = (0x00200000 | (reg_l<<16)); USB2_PHY_DELAY;
 REG32(0xb80210A4) = (0x00300000 | (reg_l<<16)); USB2_PHY_DELAY; 
 REG32(0xb80210A4) = (0x00300000 | (reg_h<<16)); USB2_PHY_DELAY; 
 REG32(0xb80210A4) = (0x00200000 | (reg_h<<16)); USB2_PHY_DELAY; 
 REG32(0xb80210A4) = (0x00300000 | (reg_h<<16)); USB2_PHY_DELAY; 
 
 unsigned char val;
 val=REG32(0xb80210A4)>>24;
 prom_printf("reg=%x val=%x\n",reg, val);
 return val;
}
#endif






