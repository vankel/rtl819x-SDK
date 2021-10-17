#include "apmib.h"
#include "mibtbl.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned int time_count;

#if defined(APP_WATCHDOG)
static int is_watchdog_alive(void)
{
	int is_alive = 0;
	int pid = -1;
	pid = find_pid_by_name("watchdog");
	if(pid > 0)
		is_alive = 1;
	return is_alive;
}

#endif

void timeout_handler() 
{
	char tmpBuf[128];
	time_count++;
	if(!(time_count%1))
	{
#if defined(APP_WATCHDOG)
		if(is_watchdog_alive() == 0)
		{
			//printf("watchdog is not alive\n");
			system("watchdog 1000&");
		}
#endif

	}	

	if(!(time_count%60))
 	{

	}
 	alarm(1);
}

int main(int argc, char** argv)
{
	char	line[300];
	char action[16];
	int i;
	if ( !apmib_init()) {
		printf("Initialize AP MIB failed !\n");
		return -1;
	}	
	signal(SIGALRM,timeout_handler);
	alarm(1);
	while(1)
	{
		#ifdef SEND_GRATUITOUS_ARP
		checkWanStatus();
		#endif
		sleep(1);
	}
}

