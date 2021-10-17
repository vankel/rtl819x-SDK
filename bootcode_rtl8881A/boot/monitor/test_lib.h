
#ifndef _TEST_LIB_H
#define _TEST_LIB_H

#include <linux/interrupt.h>
#include <asm/system.h>
#include "monitor.h"


#include <asm/mipsregs.h>	//wei add
#include <asm/rtl8196.h>

#define DBG_PRINT dprintf
#define REG32(reg)	(*(volatile unsigned int *)(reg))

/*
COMMAND_TABLE	MainCmdTable[] =
{
	{ "?"	  ,0, NULL			, "HELP (?)				    : Print this help message"	},
	{ "HELP"  ,0, NULL			, NULL									},
	{ "DW"	  ,2, CmdDumpWord		, "DW <Address> <Len>"},
	{ "DWS"	  ,2, CmdDumpWordSwap		, "DWS <Address> <Len>"},	
	{ "DH"	  ,2, CmdDumpHword		, "DH <Address> <Len>"}, //wei add		
	{ "DB"	  ,2, CmdDumpByte		, "DB <Address> <Len>"}, //wei add	
	
	{ "EW",2, CmdWriteWord, "EW <Address> <Value1> <Value2>..."},
	{ "EWS",2, CmdLib_WriteWordSwap, "EW <Address> <Value1> <Value2>..."},	
	{ "EH",2, CmdWriteHword, "EH <Address> <Value1> <Value2>..."},
	{ "EB",2, CmdWriteByte, "EB <Address> <Value1> <Value2>..."},
	

	{ "REGR"     ,1,  CmdLib_RegRead		, "REGR: Reg Read <addr> [mask]"},	
	{ "REGRS"   ,1,  CmdLib_RegReadSwap	, "REGRS: Reg Read <addr> [mask]"},		
	{ "REGW"     ,1, CmdLib_RegWrite		, "REGW: Reg Write <mask> <val> "},	
	{ "REGWS"   ,1, CmdLib_RegWriteSwap	, "REGWS: Reg Write <mask> <val> "},

	
	{ "ECHO"   ,1, CmdLib_Echo			, "Wait:  unit : msec "},		
	{ "WAIT"   ,1, CmdLib_Wait			, "Wait:  unit : msec "},	

	{ "AT"   ,1, CMD_AutoTest			, "AUTO: auto test "},		

*/

extern void RunMonitor(char *PROMOPT, COMMAND_TABLE *TestCmdTable, int len);

extern int CmdLib_DumpWord( int argc, char* argv[] );
extern int CmdLib_DumpWordSwap( int argc, char* argv[] );
extern int CmdLib_DumpHword( int argc, char* argv[] );
extern int CmdLib_DumpByte( int argc, char* argv[] );

extern int CmdLib_WriteWord( int argc, char* argv[] );
extern int CmdLib_WriteWordSwap( int argc, char* argv[] );
extern int CmdLib_WriteHword( int argc, char* argv[] );
extern int CmdLib_WriteByte( int argc, char* argv[] );


extern int CmdLib_Wait(int argc, char* argv[]);
extern int CmdLib_Echo(int argc, char* argv[]);

extern int CmdLib_RegRead(int argc, char* argv[]);
extern int CmdLib_RegReadSwap(int argc, char* argv[]);
extern int CmdLib_RegWrite(int argc, char* argv[]);
extern int CmdLib_RegWriteSwap(int argc, char* argv[]);

extern int DoAutoTest(unsigned char *pPattScript, COMMAND_TABLE *TestCmdTable, int len, int loopno, int failstop, int quiet );
/*
int CMD_AutoTest(int argc, char* argv[])
{

	if(argc<1)
	{
		DBG_PRINT("at <pattern no>  <loop times> <1: fail stop>\n");	
		DBG_PRINT("patt=0: download fw\n");			
		DBG_PRINT("patt=1: init dma\n");		
		DBG_PRINT("patt=9: load from memory 0x80500000\n");			
		return;
	}



	
	unsigned int patno=1, loopno=1, failstop=0; 
	if(argc>=1)		patno = strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	if(argc>=2)		loopno = strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	if(argc>=3)		failstop = strtoul((const char*)(argv[2]), (char **)NULL, 16);	
	
	switch(patno)
	{
		case 0:	test_pattern_ptr=SLV_PATT0; 			break;
		case 1:	test_pattern_ptr=SLV_PATT1; 			break;
		case 9:	test_pattern_ptr=0x80500000; 			break;		
		default: test_pattern_ptr=NULL; 					break;
	}

	if(test_pattern_ptr==NULL)		
		return;


	DoAutoTest(test_pattern_ptr, SlvPCIe_CmdTable, sizeof(SlvPCIe_CmdTable)/ sizeof(COMMAND_TABLE), loopno, failstop, 0);

	

}

*/
//---------------------------------------------------------------------------
#endif

