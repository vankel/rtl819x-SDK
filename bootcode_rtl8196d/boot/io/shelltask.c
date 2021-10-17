#include	"monitor.h"
#define CMD_BUF_USE_MALLOC 1
#define SHELL_NAME		"<REALTEK>"		/* The default command line */
#ifdef CMD_BUF_USE_MALLOC
static int com_buf_len = 128; /*current buffer len*/
#define COM_BUF_LEN_INIT    128 /*init buffer len*/
#else
#define COM_BUF_LEN		128
#endif
#define SHELL_HIST_SIZE		10

int run_clicmd(char *command_buf,unsigned table_count);

#define MAX_ARGV				20	
static char*		ArgvArray[MAX_ARGV];

int isprint(char ch)
{
    if(ch>=0x20 && ch <=0x7e)
	{
		//dprintf("is print char\n");
		return 1;
	}
	else
	{
		//dprintf("not print char\n");
		return 0;
	}
}

#define GetChar()	serial_inc()
#define PutChar(x)	serial_outc(x)
typedef struct _shell_hist_t {
#ifdef CMD_BUF_USE_MALLOC
    char command_buf[COM_BUF_LEN_INIT + 1];
#else
    char command_buf[COM_BUF_LEN + 1];
#endif
    unsigned int pos;
    struct _shell_hist_t *prev, *next;
} shell_hist_t;

static char * strncpy(char * dest,const char *src,unsigned count)
{
	char *tmp = dest;

	bzero(dest,count);
	while (count-- && (*dest++ = *src++) != '\0')
		/* nothing */;

	return tmp;
}

shell_hist_t *
add_shell_history_node(shell_hist_t *history, unsigned int pos)
{
    shell_hist_t *s;

    if(!(s = (shell_hist_t *)malloc(sizeof(shell_hist_t))))
	return(NULL);

    memset(s, 0, sizeof(shell_hist_t));

    if(history) 
	history->next = s;

    s->prev = history;
    s->pos = pos;

    return s;
}

shell_hist_t *
build_hist_list(unsigned int size)
{
    int i = size;
    shell_hist_t *sh = NULL, *start = NULL;

    while(i) {
	if(i == size) {
	    if(!(start = sh = add_shell_history_node(NULL, i))) {
		dprintf("Aiee -- add_shell_history_node() returned NULL\n");
		return(NULL);
	    }
	}
	else {
	    if(!(sh = add_shell_history_node(sh, i))) {
		dprintf("Aiee -- add_shell_history_node() returned NULL\n");
		return(NULL);
	    }
	}
	i--;
    }

	if(start != NULL){
	    sh->next = start;
	    start->prev = sh;
	}
    return start;
}

void monitor_real(unsigned int table_count)
{   
    shell_hist_t *sh;
	char ch = 0 ;
	unsigned shellnamelong = strlen(SHELL_NAME);
	unsigned char blackdone = 1;
	unsigned char firstime = 1;

#ifdef CMD_BUF_USE_MALLOC
	unsigned int i = 0; //char will limit max 256
	char *command_buf = NULL;
	com_buf_len = COM_BUF_LEN_INIT; 
	command_buf = malloc(com_buf_len + 1); 
	if(!command_buf){
		dprintf("shell out of memory\n");
        return;
	}
	bzero(command_buf,com_buf_len + 1);
#else
    unsigned char i = 0, num;
    
    char command_buf[COM_BUF_LEN + 1];	/* store '\0' */
#endif

    if(!(sh = build_hist_list(SHELL_HIST_SIZE))) {
		dprintf("build_hist_list() failed\nplease Resetting\n");
    }
    command_buf[0] = '\0';
    dprintf("\n"SHELL_NAME);

    while(1) {
	do{ 
		ch = GetChar() ;
	//	dprintf("[%s:] %d  i = %d, ch = 0x%x\n",__func__,__LINE__,i,ch);
	} while(!isprint(ch) && 
		(ch != '\b') &&		/* Backspace */ 
		(ch != 0x7F) &&		/* Backspace */
		(ch != 0x0A) &&		/* LF */
		(ch != 0x0D) &&		/* CR */
		(ch != 0x01) &&		/* Ctrl-A */
		(ch != 0x02) &&		/* Ctrl-B */
		(ch != 0x15) &&		/* Ctrl-U */
		(ch != 0x17) &&		/* Ctrl-W */
		(ch != 0x1b));

	while((i != 0&&command_buf[i-1] == 0x1b) 
			&&(ch == '\b'||
		       ch == 0x7F||
		       ch == 0x0A||
		       ch == 0x0D||
		       ch == 0x01||
		       ch == 0x02||
		       ch == 0x15||
			   ch == ' ' ||
			   ch == 0x41||
			   ch == 0x17||
			   ch == 0x42
		       ))/*i-1 is the last index saved char,chis is for esc bug patch*/
	{
		i--;
		command_buf[i] = '\0';
	}

	switch(ch) {
	case 0x01:		/* Ctrl-A */
	    if(i) {
		int x = i;
		while(x) {
		    PutChar('\b');
		    x--;
		    i--;
		}
	    }
	    break;
	case 0x02:		/* Ctrl-B */	
	    if(i) {
		i--;
		PutChar('\b');
	    }
	    break;
    case 0x17:     /*Ctrl-w*/
		blackdone = 0;
		firstime = 1;
		while(i != 0)
		{
		    if(command_buf[i-1] != ' ')
			{
				if(blackdone&&!firstime)
				    break;
				firstime = 0;
				blackdone = 0;
			}else{
				blackdone = 1;
			}
		    PutChar('\b');
		    PutChar(' ');
		    PutChar('\b');
			i--;
		}
		break;
	case 0x0A:				/* LF */
	case 0x0D:				/* CR */
	    if(!i) {				/* commandbuf is NULL, begin a new line */
		dprintf("\n"SHELL_NAME);
	    }
	    else {
		/* Get rid of the end space */
		if(command_buf[i - 1]==' ') 
		    i--;

		command_buf[i] = '\0';
		
		/* Add to the shell history */
#ifdef CMD_BUF_USE_MALLOC
		if(i <= COM_BUF_LEN_INIT) /*if cmd too big,do not add history*/
		{
		    strncpy(sh->command_buf, command_buf, sizeof(sh->command_buf) - 1);
            sh->command_buf[COM_BUF_LEN_INIT] = '\0';
		    sh = sh->next;
		}
#else
		strncpy(sh->command_buf, command_buf, sizeof(sh->command_buf) - 1);
		sh = sh->next;
#endif

		run_clicmd(command_buf,table_count);

#ifdef CMD_BUF_USE_MALLOC
		if((i < com_buf_len /2) && (com_buf_len > COM_BUF_LEN_INIT)){
		    com_buf_len = com_buf_len/2;
		    char *ptr = NULL;
			ptr = malloc(com_buf_len + 1);
			if(!ptr){
			//if(ptr) //for test
				dprintf("shell outof memory\n");
				com_buf_len = 2*com_buf_len;
				i = 0;
				command_buf[i] = '\0';
				dprintf(SHELL_NAME);
                //free(ptr); //for test
				break;
			}
			bzero(ptr,com_buf_len+1);
			free(command_buf);
			command_buf = ptr;
		}
#endif

		i = 0;
		command_buf[i] = '\0';
		dprintf(SHELL_NAME);
	    }
	    break;
	    
	case 0x15:			/* Ctrl-U */
	    if(i) {
		while(i) {
		    PutChar('\b');
		    PutChar(' ');
		    PutChar('\b');
		    i--;
		}
	    }
	    break;

	case 0x7F:
	case '\b':		/* Backspace */
	    if(i) {
		i--;		/* Pointer back once */
		PutChar('\b');	/* Cursor back once */
		PutChar(' ');	/* Erase last char in screen */
		PutChar('\b');	/* Cursor back again */
	    }
	    break;
	
	case ' ': /*blank*/
	    /* Don't allow continuous or begin space(' ') */
	    if((!i) || 
#ifdef CMD_BUF_USE_MALLOC
	       (i > com_buf_len) || 
#else
	       (i > COM_BUF_LEN) || 
#endif
	       (command_buf[i - 1] == ' ')) {
		/* do nothing */
	    }
	    else {
		command_buf[i] = ch;
		i++;
		PutChar(ch);	/* display and store ch */
	    }
	    break;

	case 0x41: /*up*/
	case 0x42: /*down*/
		if (i >= 2) {
			//check if users presses Up arrow or Down arrow
			if ((command_buf[i-1]==0x5b) && (command_buf[i-2]==0x1b)) {
				int x;
				/* If there's any input on the line already, erase it */
				if(i) {
					//x = i+2; /*ecos SHELL_NAME is "# ",so x = i+2,will count the two world*/
					x = i + shellnamelong ; /*here add for bootloader*/
					PutChar('\r');
					while(x) {
				    		PutChar(' ');
				    		x--;
					}
					dprintf("\r"SHELL_NAME);
				}
				
				if (ch == 0x41)
					sh = sh->prev;
				else
					sh = sh->next;
#ifdef CMD_BUF_USE_MALLOC
                strncpy(command_buf, sh->command_buf, sizeof(sh->command_buf) - 1);
                command_buf[COM_BUF_LEN_INIT] = '\0'; //for strlen can get the end
#else
				strncpy(command_buf, sh->command_buf, sizeof(command_buf) - 1);
#endif
				i = strlen(command_buf);
				//dprintf("%s", command_buf);
				for (x=0; x<i; x++) {
					if (isprint(command_buf[x]))
						PutChar(command_buf[x]);
				}
				break;
			}
		}
		//It's a Normal key. Fall through!!
	default:			/* Normal key */
#ifdef CMD_BUF_USE_MALLOC
		if(i >= com_buf_len ){
		    char *ptr = NULL;
		    com_buf_len = 2 * com_buf_len;
			ptr = malloc(com_buf_len+1);
			if(!ptr){
			//if(ptr)  //for test
				dprintf("cmd may too long, shell outof memory\n");
				com_buf_len = com_buf_len/2;
                //free(ptr); //for test
				break;
			}
			bzero(ptr,com_buf_len+1);
			memcpy(ptr,command_buf,i);
			free(command_buf);
			command_buf = ptr;
		}
		if((i == 0) || (command_buf[i -1] != 0x1b) ||(command_buf[i-1] == 0x1b &&ch == 0x5b))
		    command_buf[i] = ch;
		else{
		    command_buf[i--] = '\0'; /*restore*/
		    command_buf[i] = ch;
		}
		i++;
		if (isprint(ch))
		    PutChar(ch);  /* Display and store ch */
		break;
		/*do nothing else*/
#else
	    if(i < COM_BUF_LEN) {	/* The buf is less than MAX length */
		command_buf[i] = ch;
		i++;
		if (isprint(ch))
			PutChar(ch);  /* Display and store ch */
	    }
	    break;
#endif
	}
    }
}

int run_clicmd(char *command_buf,unsigned table_count)
{
	int	argc  = 0;
	char** argv = NULL;
	int i = 0,retval = 0;
    argc = GetArgc( (const char *)command_buf);
	argv = GetArgv( (const char *)command_buf);
	dprintf("\n");
	if(argc < 1)
	{
		dprintf("Command error\n");
		return 0;
	}
	StrUpr( argv[0] );

    extern COMMAND_TABLE MainCmdTable[];
	for(i = 0 ; i < table_count; i++)
	{
		if( ! strcmp( argv[0], MainCmdTable[i].cmd ) )
		{
		    retval = MainCmdTable[i].func( argc - 1 , argv+1 );
			memset(argv[0],0,sizeof(argv[0]));
			break;
		}
	}

	if(i == table_count) dprintf("unknown command: ' %s ' \n",argv[0]);
	return 0;
}
