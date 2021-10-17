#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <stdarg.h>


static void
die(const char *msg, ...)
{
    va_list ap;

    va_start(ap, msg);
	fprintf(stderr, "%s: ERROR: ", "watchdog");
	vfprintf(stderr, msg, ap);
    va_end(ap);

    exit(1);
}


static void
watchdog_write_pidfile(void)
{
    char pidfile[80];
    char pidbuf[16];
    int fd;
    int ret;

    snprintf(pidfile, sizeof(pidfile), "/var/run/%s.pid", "watchdog");
    fd = open(pidfile, O_RDONLY);
    if (fd < 0)
    {
	if (errno != ENOENT)
	    die("watchdog_write_pidfile: opening pidfile %s for read: %s\n",
		pidfile, strerror(errno));
		/* ENOENT is good: the pidfile doesn't exist */
    }
    else
    {
	/* the pidfile exists: read it and check whether the named pid
	   is still around */
	int pid;
	char *end;

	ret = read(fd, pidbuf, sizeof(pidbuf));
	if (ret < 0)
	    die("watchdog_write_pidfile: read of pre-existing %s failed: %s\n",
		pidfile, strerror(errno));
	pid = strtol(pidbuf, &end, 10);
	if (*end != '\0' && *end != '\n')
	    die("watchdog_write_pidfile: couldn't parse \"%s\" as a pid (from file %s); "
		"aborting\n", pidbuf, pidfile);
	ret = kill(pid, 0); /* try sending signal 0 to the pid to check it exists */
	if (ret == 0)
	    die("watchdog_write_pidfile: %s contains pid %d which is still running; aborting\n",
		pidfile, pid);
	/* pid doesn't exist, looks like we can proceed */
	close(fd);
    }

    /* re-open pidfile for write, possibly creating it */
    fd = open(pidfile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (fd < 0)
	die("watchdog_write_pidfile: open %s for write/create: %s\n", pidfile, strerror(errno));
    snprintf(pidbuf, sizeof(pidbuf), "%d\n", getpid());
    ret = write(fd, pidbuf, strlen(pidbuf));
    if (ret < 0)
	die("watchdog_write_pidfile: writing my PID to lockfile %s: %s\n",
	    pidfile, strerror(errno));
    close(fd);
}


void watchdog_func()
{
	FILE *file;
	file = fopen("/proc/watchdog_kick","w+"); 
	if(file)
	{
		fputs("111", file);
		fclose(file);
	}	
}

int
main(int argc, char **argv)
{ 
	pid_t	pid; 
	char tmpBuff[30] = {0};
	int res = 0;  
	int fd;
	int interval;
	int sec,micro_sec;
	sigset_t sigset;

	if(argc >= 2)
		interval = atoi(argv[1]);
	else
		interval = 500;

	if(interval >= 10000){
		printf("watchdog interval too long,should not more than 10s\n");
		interval = 1000;
	}

	sec = interval/1000;
	micro_sec = (interval % 1000) * 1000;

	watchdog_write_pidfile();

	/* unblock sigalarm and sigterm signal */
	sigaddset(&sigset,SIGALRM);
	if(sigprocmask(SIG_UNBLOCK,&sigset,NULL) < 0)
		printf("sigprocmask error\n");
  
  	// Register watchdog_func to SIGALRM  
	signal(SIGALRM, watchdog_func);
	
	struct itimerval tick;  
	memset(&tick, 0, sizeof(tick));  
	//printf("interval:%d.\n",interval);

	// Timeout to run function first time  
	tick.it_value.tv_sec = sec;  // sec  
	tick.it_value.tv_usec = micro_sec; // micro sec.  

	// Interval time to run function  
	tick.it_interval.tv_sec = sec;  
	tick.it_interval.tv_usec = micro_sec;  

	pid = getpid(); 
	snprintf(tmpBuff,30,"renice -19 %d",pid);
	system(tmpBuff);
	//stop watchdog first
	system("echo 1 > /proc/watchdog_kick");
	system("echo 2 > /proc/watchdog_start");	
	
	// resume watchdog
	system("echo 1 > /proc/watchdog_start");
	system("echo 1 > /proc/watchdog_kick");
	res = setitimer(ITIMER_REAL, &tick, NULL);  
	if (res) {  
		printf("Set watchdog timer failed!!/n"); 
		return -1;  
	}  
 
	while(1) {  
		pause();  
	}  
	return 0;    
}  


