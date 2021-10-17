


#include <linux/interrupt.h>
#include <asm/system.h>
#include "monitor.h"


#include <asm/mipsregs.h>	//wei add
#include <asm/rtl8196.h>

#include "test_lib.h"

#define DBG_PRINT dprintf

int at_errcnt=0;


inline volatile unsigned int SWAP32(unsigned int data)   //wei add, for sata endian swap
{
	unsigned int cmd=data;
	unsigned char *p=&cmd;	
	return ( (p[3]<<24) |  (p[2]<<16) | (p[1]<<8)  | p[0] );	
}

//-----------------------------------------------------------

void ShowCmdHelp(COMMAND_TABLE *CmdTable, int len)
{
	int	i, LineCount ;

    printf("----------------- COMMAND MODE HELP ------------------\n");
	for( i=0, LineCount = 0 ; i < len ; i++ )
	{
		if( CmdTable[i].msg )
		{
			LineCount++ ;
			printf( "%s\n", CmdTable[i].msg );
#if 0			
			if( LineCount == PAGE_ECHO_HEIGHT )
			{
				printf("[Hit any key]\r");
				WaitKey();
				printf("	     \r");
				LineCount = 0 ;
			}			
#endif			
		}
	}
    
	return TRUE ;

}

//-----------------------------------------------------------

void RunMonitor(char *PROMOPT, COMMAND_TABLE *TestCmdTable, int len)
{
	char		buffer[ MAX_MONITOR_BUFFER +1 ];
	int		argc ;
	char**		argv ;
	int		i, retval ;

	int skip=0;

	while(1)
	{	
		//printf( "%s", TEST_PROMPT );
		dprintf( "%s", PROMOPT );
		memset( buffer, 0, MAX_MONITOR_BUFFER );
		GetLine( buffer, MAX_MONITOR_BUFFER,1);
		dprintf( "\n" );

		//wei add, skip first space of the command.
		int space=0;
		while(*(buffer+space)==' ')
			space++;
			
		argc = GetArgc( (const char *)(buffer+space) );
		argv = GetArgv( (const char *)(buffer+space) );
		if( argc < 1 ) continue ;
		StrUpr( argv[0] );

		if(!strcmp( argv[0], "..") || !strcmp( argv[0], "Q") )
			return;

		if(!strcmp( argv[0], "?") || !strcmp( argv[0], "HELP") )
		{	ShowCmdHelp(TestCmdTable, len);
			continue;
		}

		if(!strcmp( argv[0], "/*")  )
		{	skip=1;
			continue;
		}
		if(!strcmp( argv[0], "*/")  )
		{	skip=0;
			continue;
		}		
		if( ((*(unsigned char *)argv[0])=='#')  || (skip==1) )
		{	continue;
		}	
		
		
		//for( i=0 ; i < (sizeof(TestCmdTable) / sizeof(COMMAND_TABLE)) ; i++ )
		for( i=0 ; i < (len) ; i++ )
		{
			if( ! strcmp( argv[0], TestCmdTable[i].cmd ) )
			{
				if(TestCmdTable[i].func)
					retval = TestCmdTable[i].func( argc - 1 , argv+1 );
				//dprintf("End run code\n");
				memset(argv[0],0,sizeof(argv[0]));
				break;
			}
		}
		//if(i==sizeof(TestCmdTable) / sizeof(COMMAND_TABLE)) printf("Unknown command !\r\n");
		if(i==len) printf("Unknown command !\r\n");
	}
}

//----------------------------------------------------------------

void dwdump(unsigned char * pData, int count)
{
	unsigned int *sbuf = pData;	
	int length=count;  //is word unit

	//dprintf("Addr=%x, len=%d", sbuf, length);	
	dprintf("\n");
	dprintf(" [Addr]    .0.1.2.3    .4.5.6.7    .8.9.A.B    .C.D.E.F" );
	
	{
		int i;		
		for(i=0;i<length; i++)
		{
			if((i%4)==0)
			{	dprintf("\n\r");
				dprintf("%08X:  ", (sbuf+i) );
			}
			
			dprintf("%08X    ", sbuf[i]);
			//sbuf[i];
			
		}
		dprintf("\n\r");
	}	
}

//------------------------------------------------------------------
void dwdump_swap(unsigned char * pData, int count)
{
	unsigned int *sbuf = pData;
	unsigned int tmp;
	volatile unsigned char *p=&tmp;
	int length=count;  //is word unit

	//dprintf("Addr=%x, len=%d", sbuf, length);	
	dprintf("\n");	
	dprintf(" [Addr]    .3.2.1.0    .7.6.5.4    .B.A.9.8    .F.E.D.C [SWAP]" );
	
	{
		int i;		
		for(i=0;i<length; i++)
		{
			if((i%4)==0)
			{	dprintf("\n\r");
				dprintf("%08X:  ", (sbuf+i) );
			}
			tmp=sbuf[i];
			//dprintf("%02X%02X%02X%02X    ", p[3], p[2], p[1], p[0] );
			dprintf("%08X    ",  (p[3]<<24) |  (p[2]<<16) | (p[1]<<8)  | p[0] );

			
		}
		dprintf("\n\r");
	}	
}

//---------------------------------------------------------------------------
int CmdLib_DumpWord( int argc, char* argv[] )
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

	dwdump(src,len);

}

//---------------------------------------------------------------------------
int CmdLib_DumpWordSwap( int argc, char* argv[] )
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

				
	if(!argv[1])
		len = 1;
	else
	len= strtoul((const char*)(argv[1]), (char **)NULL, 10);			
	while ( (src) & 0x03)
		src++;
	
	dwdump_swap(src,len);
}
//---------------------------------------------------------------------------
int CmdLib_DumpHword( int argc, char* argv[] )
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


	for(i=0; i< len ; i+=4,src+=16)
	{	
		dprintf("%X:	%04X  %04X  %04X  %04X  :  %04X  %04X  %04X  %04X  \n",
		src, *(unsigned short *)(src), *(unsigned short *)(src+2), 
		*(unsigned short *)(src+4),   *(unsigned short *)(src+6),     
		*(unsigned short *)(src+8),   *(unsigned short *)(src+10), 
		*(unsigned short *)(src+12), *(unsigned short *)(src+14)  );
	}

}
//---------------------------------------------------------------------------
int CmdLib_DumpByte( int argc, char* argv[] )
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
int CmdLib_WriteWord( int argc, char* argv[] )
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
int CmdLib_WriteWordSwap( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned int value,i;
	
	src = strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	while ( (src) & 0x03)
		src++;

	for(i=0;i<argc-1;i++,src+=4)
	{
		value= strtoul((const char*)(argv[i+1]), (char **)NULL, 16);	
		*(volatile unsigned int *)(src) = SWAP32(value);
	}
	
}
//---------------------------------------------------------------------------

int CmdLib_WriteHword( int argc, char* argv[] )
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

//---------------------------------------------------------------------------
int CmdLib_WriteByte( int argc, char* argv[] )
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
//--------------------------------------------------------------------------


int CmdLib_FillPattern( int argc, char* argv[] )
{
	
	unsigned long src;
	unsigned int value,i;
	unsigned int length;
	
	src = strtoul((const char*)(argv[0]), (char **)NULL, 16);		
	while ( (src) & 0x03)
		src++;

	i = 0;
	value = strtoul((const char*)(argv[1]), (char **)NULL, 16);	
	length = strtoul((const char*)(argv[2]), (char **)NULL, 16);	
	printf("Write %x to %x for length %d\n",value,src,length);
	for(i=0;i<length;i+=4,src+=4)
	{
		*(volatile unsigned int *)(src) = value;
	}
	
}

//---------------------------------------------------------------------------
int CmdLib_MemCmp(int argc, char* argv[])
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

int CmdLib_Wait(int argc, char* argv[])
{
	if( argc < 1 ) 
	{
		DBG_PRINT("wait  t msec.\n");		
		DBG_PRINT("wait  r reg mask expval timeout \n");	
		return 0;
	}

	StrUpr( argv[0] );

	if( ! strcmp( argv[0], "T" ) )
	{
		if(argc>=2)
		{	
			int wait = strtoul((const char*)(argv[1]), (char **)NULL, 10);		
			delay_ms(wait);
		}
	}
	else if( ! strcmp( argv[0], "R" ) )	
	{

		if(argc>=4)
		{
			unsigned int regaddr = strtoul((const char*)(argv[1]), (char **)NULL, 10);
			unsigned int mask = strtoul((const char*)(argv[2]), (char **)NULL, 10);
			unsigned int expval = strtoul((const char*)(argv[3]), (char **)NULL, 10);

			while(1)
			{
				if( (REG32(regaddr) & mask ) ==  expval)
					break;
			}
		}

	}
	
	
}; 
//----------------------------------------------------------------
int CmdLib_Echo(int argc, char* argv[])
{
	if(argv[0])
	dprintf("\n%s", argv[0]);

}

//----------------------------------------------------------------
struct AddrName
{
	char name[8];
	unsigned int addr;
};

#define USB_OP_BASE 0xb8021010
struct AddrName AddrNameTable[]=
{
	"UCMD", 		USB_OP_BASE+0x00,
	"USTATUS", 		USB_OP_BASE+0x04,	
	"UINTR",	 	USB_OP_BASE+0x08,	
	"UFRINDEX", 	USB_OP_BASE+0x0c,	
	"USEGMENT",	 USB_OP_BASE+0x10,		
	"UFRLIST", 	USB_OP_BASE+0x14,	
	"UASYNC", 	USB_OP_BASE+0x18,	
	"UCONFIG", 	USB_OP_BASE+0x40,	
	"UPORT", 	USB_OP_BASE+0x44,	
	
	
//	"",0
};
unsigned int GetAddrByName(unsigned char *name)
{
	unsigned int addr;	
	StrUpr( name );

	//number
	unsigned char c=*name;
	if(c<'C')
	{	addr=strtoul(name, (char **)NULL, 16);	
		return addr;
	}

	//name	
	int i=0;
	int len=sizeof(AddrNameTable)/ sizeof(struct AddrName );

	
	for(i=0; i<len; i++)
	{
		if( ! strcmp( name, AddrNameTable[i].name ) )		
		{	addr=AddrNameTable[i].addr;
			break;
		}
	}

	if(i==len)	addr=0x80000000;
	return addr;

}

	
int CmdLib_RegRead(int argc, char* argv[])
{

	if(argc<1)
	{//dump all	
	       dprintf("\n"); 		   
		dprintf("regr addr [~][mask] [:] [value]\n");
		return;
	}

	int addr,val;
	unsigned int check=0,mask=0xffffffff,expval=0;
	
	StrUpr( argv[0] );

	
//	addr= strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	addr=GetAddrByName( argv[0] );
	
	val=REG32(addr);

	//regr iocmd
	if(argc==1)
	{	//dprintf("%x\n", val );
	}

	//regr iocmd 0001
	else if(argc==2)
	{
		if( *(argv[1]) == '~')		mask = 0xffffffff-strtoul((const char*)((argv[1])+1), (char **)NULL, 16);	
		else 					mask = strtoul((const char*)(argv[1]), (char **)NULL, 16);		
	}
	//regr iocmd : 0x0001
	else if(   argc>=3  &&  *(argv[1])==':' )
	{	check=1;
		expval = strtoul((const char*)(argv[2]), (char **)NULL, 16);	
	}
	//regr iocmd 0x0001 : 0x0001
	else if(argc>=3  && *(argv[1]) != '\0')
	{	
		if( *(argv[1]) == '~')		mask = 0xffffffff-strtoul((const char*)((argv[1])+1), (char **)NULL, 16);
		else						mask = strtoul((const char*)(argv[1]), (char **)NULL, 16);		
		if(argc>=3 && *(argv[2]) == ':')
		{	check=1;
			expval = strtoul((const char*)(argv[3]), (char **)NULL, 16);	
		}
	}


	//verify
	if(!check)
	{
		dprintf("Addr %08x, %s=%08x \n", addr, argv[0],val&mask );			
	}
	else
	{
		if( (val&mask) !=expval)
		{	dprintf("Fail, addr=%08x val=%x, expval=%x \n", addr, val, expval);
			at_errcnt++;
		}
		else
			dprintf("Pass \n");

	}		

}


int CmdLib_RegReadSwap(int argc, char* argv[])
{

	if(argc<1)
	{//dump all	
	       dprintf("\n"); 		   
		dprintf("regrs addr [~][mask] [:] [value]\n");
		return;
	}

	int addr,val;
	unsigned int check=0,mask=0xffffffff,expval=0;
	
	StrUpr( argv[0] );

	
//	addr= strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	addr=GetAddrByName( argv[0] );
	
	val=REG32(addr);
	//swap
	val=SWAP32(val);

	
	//regr iocmd
	if(argc==1)
	{	//dprintf("%x\n", val );
	}

	//regr iocmd 0001
	else if(argc==2)
	{
		if( *(argv[1]) == '~')		mask = 0xffffffff-strtoul((const char*)((argv[1])+1), (char **)NULL, 16);	
		else 					mask = strtoul((const char*)(argv[1]), (char **)NULL, 16);		
	}
	//regr iocmd : 0x0001
	else if(   argc>=3  &&  *(argv[1])==':' )
	{	check=1;
		expval = strtoul((const char*)(argv[2]), (char **)NULL, 16);	
	}
	//regr iocmd 0x0001 : 0x0001
	else if(argc>=3  && *(argv[1]) != '\0')
	{	
		if( *(argv[1]) == '~')		mask = 0xffffffff-strtoul((const char*)((argv[1])+1), (char **)NULL, 16);
		else						mask = strtoul((const char*)(argv[1]), (char **)NULL, 16);		
		if(argc>=3 && *(argv[2]) == ':')
		{	check=1;
			expval = strtoul((const char*)(argv[3]), (char **)NULL, 16);	
		}
	}


	//verify
	if(!check)
	{
		dprintf("Addr %08x, %s=%08x \n", addr, argv[0],val&mask );			
	}
	else
	{
		if( (val&mask) !=expval)
		{	dprintf("Fail, addr=%08x val=%x, expval=%x \n", addr, val, expval);
			at_errcnt++;
		}
		else
			dprintf("Pass \n");

	}		


}

//----------------------------------------------------------------------------

int CmdLib_RegWrite(int argc, char* argv[])
{
	if(argc<2)
	{	 
		dprintf("regw <addr> <val> \n");		
		dprintf("regw <addr> <mask> <value>\n");			
		dprintf("ex: regw b8001000  ffffffff \n");			
		return;	
	}

//	int off = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	unsigned int addr,mask=0,val;

	StrUpr( argv[0] );
//	addr= strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	addr=GetAddrByName( argv[0] );
		
	//regw iocmd 0x0001
	if(argc==2)
	{	val = strtoul((const char*)(argv[1]), (char **)NULL, 16);		
	}
	//regw iocmd 0x0001 0x0001
	else if(argc>=3)
	{
		if( *(argv[1]) == '~')		mask = 0xffffffff-strtoul((const char*)((argv[1])+1), (char **)NULL, 16);
		else						mask = strtoul((const char*)(argv[1]), (char **)NULL, 16);
		val = strtoul((const char*)(argv[2]), (char **)NULL, 16);
	}

	if(mask==0)
		REG32(addr)=val ;		//avoid DR issue.
	else		
		REG32(addr)= (REG32(addr) & mask) | val ;	
	
}
//----------------------------------------------------------------------------
int CmdLib_RegWriteSwap(int argc, char* argv[])
{
	if(argc<2)
	{	 
		dprintf("regws <addr> <val> \n");		
		dprintf("regws <addr> <mask> <value>\n");			
		dprintf("ex: regws b8001000  ffffffff \n");			
		return;	
	}

//	int off = strtoul((const char*)(argv[0]), (char **)NULL, 16);
	unsigned int addr,mask=0,val;

	StrUpr( argv[0] );
//	addr= strtoul((const char*)(argv[0]), (char **)NULL, 16);	
	addr=GetAddrByName( argv[0] );
		
	//regw iocmd 0x0001
	if(argc==2)
	{	val = strtoul((const char*)(argv[1]), (char **)NULL, 16);		
	}
	//regw iocmd 0x0001 0x0001
	else if(argc>=3)
	{
		if( *(argv[1]) == '~')		mask = 0xffffffff-strtoul((const char*)((argv[1])+1), (char **)NULL, 16);
		else						mask = strtoul((const char*)(argv[1]), (char **)NULL, 16);
		val = strtoul((const char*)(argv[2]), (char **)NULL, 16);
	}

	//swap



	if(mask==0)
		REG32(addr)=SWAP32(val) ;		//avoid DR issue.
	else		
	{	volatile unsigned int rv=SWAP32(REG32(addr));
		REG32(addr)=SWAP32(  (rv & mask) | val );	
	}
	
}
//----------------------------------------------------------------------------
//=================================================================


//return position 
unsigned int ExtractACmdLine(const char *pPattScript,  char *pOneCmdString, int first)
{
	//first=1 reset index, to buffer head
	//first=0 continue read a line

	static unsigned int idx=0;
	unsigned char *p=pPattScript+idx;
	int push=0;
	
	if(first==1)
	{	idx=0;
		return 0;
	}
	
	memset( pOneCmdString, 0, MAX_MONITOR_BUFFER );


	int n=0;
	while( *p )
	{
		if(n==0)
		{
			//skip first return-line
			while( *p && ((*p == 0x0d) ||(*p==0x0a) ||(*p=='\t') ||(*p==' ') ) )
				p++;
		}
#if 0
		//mark
		if( (*p == '/')  || (*(p+1) == '/' ))
		{	pOneCmdString[n] = 0 ;
			//search until reurn-line
			while( *p && (*p != 0x0d) && (*p!=0x0a) )
				p++;
			break;
		}		
#endif
		if ((n==0) && (*p =='~') )
			return 0;
		else if(*p =='#')
		{	//skip word until to newline
			while( *p && (*p != 0x0d) && (*p!=0x0a) )
				p++;
			continue;			
		}
		
		else if( (*p == '/')  && (*(p+1) == '/' ))	//search mark
		{
			p+=2;
			while( *p && (*p != 0x0d) && (*p!=0x0a) )
				p++;	
			continue;
		}
		else if( (*p == '/')  && (*(p+1) == '*' ))		//search mark
		{
			p+=2;
			while( *p && ((*p != '*') || (*(p+1)!='/')) )
				p++;			
			p+=2;
			continue;

		}	
		//end
		if(n!=0)
		{
			if( (*p == 0x0d)  || (*p == 0x0a)  || (*p == '#'))
			{	pOneCmdString[n] = 0 ;
				break;
			}
		}
	
		pOneCmdString[n] = *p ;
		n++;		
		p++;	
		if (n == 80) break;
	}
	idx= (int)p-(int)pPattScript+1;

	//thrim last space
	for(;n>1;n--)
		if( (pOneCmdString[n-1]!=' ')  &&  (pOneCmdString[n-1]!='\t') )
		{	pOneCmdString[n]=0;
			break;
		}

	//dprintf("test=> %s \r\n", pOneCmdString);
	return idx;	

}
//================================================================
int RunACmdLine(const char *cmdstr, COMMAND_TABLE *pTestCmdTable, int len )
{
	int		argc ;
	char**		argv ;
	int		i, retval ;
		
		argc = GetArgc( (const char *)cmdstr );
		argv = GetArgv( (const char *)cmdstr );
		if( argc < 1 ) return 0;
		StrUpr( argv[0] );



		//----		
		if(!strcmp( argv[0], "~") || !strcmp( argv[0], "Q") )	//return 1 to go back up directory
			return 1;
		if( *(argv[0])=='#' )	
			return 0;		
		if( *(argv[0])=='/' && *(argv[0]+1)=='/' )		// meet "//" to do nothing
			return 0;
/*
		if(argv[1])	cmd_line.fr= strtoul((const char*)(argv[1]), (char **)NULL, 16);	
		else			cmd_line.fr=0;
		if(argv[2])	cmd_line.sc= strtoul((const char*)(argv[2]), (char **)NULL, 16);	
*/
		//execute function
		//int len=sizeof(pTestCmdTable) / sizeof(COMMAND_TABLE);

		for( i=0 ; i < len ; i++ )
		{
			if( ! strcmp( argv[0], pTestCmdTable[i].cmd ) )
			{				
				if(pTestCmdTable[i].func)
					retval = pTestCmdTable[i].func( argc - 1 , argv+1 );
				memset(argv[0],0,sizeof(argv[0]));
				break;
			}
		}
		if(i==len) dprintf("Unknown command !\r\n");

		return 0;
}

//-------------------------------------------------------------------------------

int DoAutoTest(unsigned char *PatternBufferPtr, COMMAND_TABLE *TestCmdTable, int len, int loopno, int failstop, int quiet )
{

	char		OneCmdLine[ MAX_MONITOR_BUFFER +1 ];

	const char	*test_pattern_ptr=PatternBufferPtr; 
	
	if(test_pattern_ptr==NULL)		
		return;

	int i;
	at_errcnt=0;

	for(i=0;i<loopno;i++)
	{
	int rc=0;
	ExtractACmdLine(test_pattern_ptr,  OneCmdLine, 1);  //init

	while(1)
	{
		memset(OneCmdLine,0,MAX_MONITOR_BUFFER+1);
		rc=ExtractACmdLine(test_pattern_ptr,  OneCmdLine, 0);
		if(rc==0)
			break;

		if(quiet==0)	
			dprintf("\n=> %s  ....",OneCmdLine);
		
		rc=RunACmdLine(OneCmdLine, TestCmdTable, len );
		if(rc==1)
			break;

		if((failstop==1) && (at_errcnt!=0))
			break;

		
	}

	if(quiet==0)	
		dprintf("\n\n ****** Test %d times, result: Error count=%d \n\n", i,  at_errcnt);
	if((failstop==1) && (at_errcnt!=0))
			break;
	
	}

	if(quiet==0)	
		dprintf(" ****** Total test %d times, result: Error count=%d \n", i,  at_errcnt);

	if(quiet==1)
	{
		if(at_errcnt==0)	dprintf("PASS\n");
		else					dprintf("FAIL\n");			
	}

}


//---------------------------------------------------------------------------


