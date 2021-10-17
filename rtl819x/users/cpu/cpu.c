#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>


//#define MAX_INTERVAL 10
#define CPU_DATA_FLIE_PREFIX	"/web/cpu_data.dat"
#define CPU_CONF				"/var/cpu_info.conf"
#define SAVE_DATA_NUM 200
#define SIGUSR1 10
#define SIGUSR2 12
#define SIG_LEFT 16
#define SIG_RIGHT 17
#ifdef CONFIG_RTL_8198C
#define MAX_CPU_NUM 2
#else
#define MAX_CPU_NUM 1
#endif


#ifdef CONFIG_CPU_UTILIZATION_36K
#define TMP_FILE_SIZE 36000
#else 
#ifdef CONFIG_CPU_UTILIZATION_72K
#define TMP_FILE_SIZE 72000
#else
#define TMP_FILE_SIZE 36000
#endif
#endif

struct cpu_info
{
	unsigned int total;
	unsigned int load;
};

struct cpu_data_record
{
	unsigned char cpu_load_1[TMP_FILE_SIZE];
	unsigned char cpu_load_5[TMP_FILE_SIZE/5];
	unsigned char cpu_load_10[TMP_FILE_SIZE/10];
	unsigned char cpu_load_30[TMP_FILE_SIZE/30];
	unsigned char cpu_load_60[TMP_FILE_SIZE/60];
	unsigned char cpu_load_300[TMP_FILE_SIZE/300];
	unsigned char cpu_load_600[TMP_FILE_SIZE/600];
};

struct cpu_data_save
{
	char start_time[20];
	unsigned char cpu_load[MAX_CPU_NUM][SAVE_DATA_NUM];
};

int sighup_received = 0;
struct cpu_data_record record[MAX_CPU_NUM];
int load_1, load_5, load_10, load_30, load_60, load_300, load_600;
int cpu_num, start_time, start_pos, pause_pos, len;
int interval=1, adjust_time=0;
time_t display_tm;

static int get_counter(struct cpu_info *info, int index);
static int get_cpu_num();
static void sigh_handler(int sig);
static void sigh_left(int sig);
static void sigh_right(int sig);
static void get_interval_and_time(int *interval);
static void set_data(int load, int i, int j);
static void save_data();
int daemon(int nochdir, int noclose);

int main(int argc, char *argv[])
{
	struct cpu_info prev_info[MAX_CPU_NUM], curr_info[MAX_CPU_NUM];
	int is_overflow=0, load;
	int i=0, j=0, debug=0;
	time_t tm, prev_tm, curr_tm;
	char *pid_filename;

	if(argc == 2)
	{
		sscanf(argv[1], "%d", &debug);
	}

	if(!debug)
	{
		if (daemon(0, 0) < 0)
			perror("daemon");
	}

	signal(SIGUSR1, sigh_handler);
	signal(SIG_LEFT, sigh_left);
	signal(SIG_RIGHT, sigh_right);

	pid_filename = "/var/run/cpu.pid";
    if (pid_filename && *pid_filename)
    {
        FILE * fp = fopen (pid_filename, "w+");
        if (fp != NULL)
        {
            fprintf (fp, "%d", (int)getpid ());
            fclose (fp);
        }
        else
            printf("Unable to save pidfile\n");
    }

	time(&tm);
	display_tm = tm;
	get_interval_and_time(&interval);

	memset(record, 0, sizeof(record));
	cpu_num = get_cpu_num();
	if(cpu_num > MAX_CPU_NUM)
	{
		printf("Warning: MAX_CPU_NUM is too small.\n");
		cpu_num = MAX_CPU_NUM;
	}
	
	for(j=0; j<cpu_num; j++)
	{
		if(get_counter(&curr_info[j], j) == 0)
		{
			printf("Get cpu info error!\n");
			return -1;
		}
	}
	prev_tm = tm;
	
	sleep(1);
	start_time = tm+1;
	start_pos = 0;

	while(1)
	{
		time(&curr_tm);
		
		for(j=0; j<cpu_num; j++)
		{
			if((curr_tm-prev_tm)>2)
			{
				int delta_tm = curr_tm-prev_tm-1;

				while(delta_tm>0)
				{
					set_data(100, i, j);
					i++;
					if(i >= TMP_FILE_SIZE)
					{
						i = 0;
						is_overflow = 1;
					}
					delta_tm--;
				}
			}
			prev_info[j] = curr_info[j];
			if(get_counter(&curr_info[j], j) == 0)
			{
				printf("Get cpu info error!\n");
				return -1;
			}

			load = (curr_info[j].load-prev_info[j].load)*100/(curr_info[j].total-prev_info[j].total);
			set_data(load, i, j);

			if(is_overflow == 1)
			{
				start_time++;
				
				start_pos++;
				if(start_pos >= TMP_FILE_SIZE)
				{
					start_pos = 0;
				}
				
				len = TMP_FILE_SIZE;
			}
			else
			{
				len = i+1;
			}

		}
		i++;
		if(i >= TMP_FILE_SIZE)
		{
			i = 0;
			is_overflow = 1;
		}

		prev_tm = curr_tm;

		if((i+1)%interval == 0 && adjust_time == 0)
			save_data();
		sleep(1);
	}
	return 0;
}

static int get_counter(struct cpu_info *info, int index)
{
	FILE *fh;
  	char buf[512], cpu_index[10];
	unsigned int d1, d2, d3, d4, d5, d6, d7, d8, d9, sum;
	int isfound=0;

	fh = fopen("/proc/stat", "r");
	if (!fh) {
		printf("Warning: cannot open /proc/stat\n");
		return 0;
	}

	sprintf(cpu_index, "cpu%d", index);

	while(!feof(fh))
	{
		fgets(buf, sizeof buf, fh);	/* get line */

		if(strncmp(buf, cpu_index, strlen(cpu_index)) == 0)
		{
			sscanf(buf, "%*s%d %d %d %d %d %d %d %d %d",
					&d1, &d2, &d3, &d4, &d5, &d6, &d7, &d8, &d9);
			isfound = 1;
			break;
		}
	}

 	fclose(fh);

	if(isfound)
	{
		sum = d1+d2+d3+d4+d5+d6+d7+d8+d9;
		info->total = sum;
		info->load = sum - d4;
		return 1;
	}
	else
	{
		printf("Warning: cannot get %s infomation!\n", cpu_index);
		return 0;
	}
}

static int get_cpu_num()
{
	FILE *fh;
  	char buf[64], tmp[3];
	char *p, *q;
	int cpu_num=-1;

	fh = fopen("/proc/cpuinfo", "r");
	if (!fh) {
		printf("Warning: cannot open /proc/cpuinfo\n");
		return 0;
	}
	
	while(!feof(fh))
	{
		fgets(buf, sizeof buf, fh);

		if(strncmp(buf, "processor", strlen("processor")) == 0)
		{
			p = buf + 9;
			q = tmp;
			while(*p)
			{
				if(*p >= '0' && *p <= '9')
				{
					*q = *p;
					q++;
				}
				p++;
			}
			*q='\0';
			cpu_num = atoi(tmp);
		}
	}

	fclose(fh);

	cpu_num++;
	return cpu_num;
	
}

static void sigh_handler(int sig)
{
	signal(SIGUSR1, sigh_handler);
	get_interval_and_time(&interval);
	adjust_time = 0;
//	printf("signal up!\n");
	save_data();
}

static void sigh_left(int sig)
{
	signal(SIG_LEFT, sigh_left);

	adjust_time += interval;
	if(adjust_time>0)
		adjust_time=0;

//	printf("signal left!\n");
	save_data();
}

static void sigh_right(int sig)
{
	signal(SIG_RIGHT, sigh_right);

	if(len/interval > SAVE_DATA_NUM)
		adjust_time -= interval;

//	printf("signal right!\n");
	
	save_data();
}


static void get_interval_and_time(int *interval)
{
	FILE *fh;
  	char buf[64];
	int d;
		
	fh = fopen(CPU_CONF, "r");
	if (!fh) {
		printf("Warning: cannot open %s.\n", CPU_CONF);
		return;
	}

	fgets(buf, sizeof buf, fh);
	sscanf(buf, "interval: %d", &d);
	*interval = d;

 	fclose(fh);
}

static void set_data(int load, int i, int j)
{
	int a=i+1;
	int b,c;
	load += 1;
	
	load_1 = load;
	load_5 += load;
	load_10 += load;
	load_30 += load;
	load_60 += load;
	load_300 += load;
	load_600 += load;

	//set load_1
	record[j].cpu_load_1[i] = load_1;

	//set load_5
	if(a%5 == 0)
	{
		b = i/5;
		c = load_5/5;
		record[j].cpu_load_5[b] = c;
		load_5 = 0;
	}

	//set load_10
	if(a%10 == 0)
	{
		b = i/10;
		c = load_10/10;
		record[j].cpu_load_10[b] = c;
		load_10 = 0;
	}

	//set load_30
	if(a%30 == 0)
	{
		b = i/30;
		c = load_30/30;
		record[j].cpu_load_30[b] = c;
		load_30 = 0;
	}

	//set load_60
	if(a%60 == 0)
	{
		b = i/60;
		c = load_60/60;
		record[j].cpu_load_60[b] = c;
		load_60 = 0;
	}

	//set load_300
	if(a%300 == 0)
	{
		b = i/300;
		c = load_300/300;
		record[j].cpu_load_300[b] = c;
		load_300 = 0;
	}
	
	//set load_600
	if(a%600 == 0)
	{
		b = i/600;
		c = load_600/600;
		record[j].cpu_load_600[b] = c;
		load_600 = 0;
	}
}

static void save_data()
{
	struct cpu_data_save data;
	int i, j, k;
	unsigned char *p;
	int max_num, max_len, start_pos_l, end_pos_l;
	int offset = 0;
	FILE *fh;

//	printf("save data to file!\n");
	memset(&data, 1, sizeof(data));
	offset = adjust_time/interval;
	
	fh = fopen(CPU_DATA_FLIE_PREFIX, "w");
	sprintf(data.start_time, "%20d", start_time);

	for(j=0; j<cpu_num; j++)
	{
		if(interval == 1)
		{
			p = &record[j].cpu_load_1[0];
			max_num = TMP_FILE_SIZE;
			start_pos_l = start_pos;
		}
		else if(interval == 5)
		{
			p = &record[j].cpu_load_5[0];
			max_num = TMP_FILE_SIZE/5;
			start_pos_l = start_pos/5;
		}
		else if(interval == 10)
		{
			p = &record[j].cpu_load_10[0];
			max_num = TMP_FILE_SIZE/10;
			start_pos_l = start_pos/10;
		}
		else if(interval == 30)
		{
			p = &record[j].cpu_load_30[0];
			max_num = TMP_FILE_SIZE/30;
			start_pos_l = start_pos/30;
		}
		else if(interval == 60)
		{
			p = &record[j].cpu_load_60[0];
			max_num = TMP_FILE_SIZE/60;
			start_pos_l = start_pos/60;
		}
		else if(interval == 300)
		{
			p = &record[j].cpu_load_300[0];
			max_num = TMP_FILE_SIZE/300;
			start_pos_l = start_pos/300;
		}
		else if(interval == 600)
		{
			p = &record[j].cpu_load_600[0];
			max_num = TMP_FILE_SIZE/600;
			start_pos_l = start_pos/600;
		}
		else
		{
			printf("default interval 1!\n");
			p = &record[j].cpu_load_1[0];
			interval = 1;
			max_num = TMP_FILE_SIZE;
			start_pos_l = start_pos;
		}

		max_len = len/interval;
		end_pos_l = (start_pos_l + max_len - 1) % max_len;

		if(offset != 0)
			end_pos_l = pause_pos + offset;
		else
			pause_pos = end_pos_l;
			
		k = (max_len < SAVE_DATA_NUM) ? max_len : SAVE_DATA_NUM;
//		printf("k is %d.\n", k);
	
		for(i=k; i>0; i--)
		{
			if(p[end_pos_l])
				data.cpu_load[j][i-1] = p[end_pos_l];
			else
				data.cpu_load[j][i-1] = 127;
			end_pos_l--;
			if(end_pos_l < 0)
				end_pos_l = max_num-1;
		}
	}

	fwrite(&data, sizeof(data), 1, fh);

	fclose(fh);
}

int daemon( int nochdir, int noclose )
{
	pid_t pid;

	if ( !nochdir && chdir("/") != 0 )
		return -1;
   
	if ( !noclose )
	{
		int fd = open("/dev/null", O_RDWR);

		if ( fd < 0 )
			return -1;

		if ( dup2( fd, 0 ) < 0 ||
			 dup2( fd, 1 ) < 0 ||
			 dup2( fd, 2 ) < 0 ) 
		{
			close(fd);
			return -1;
		}

		close(fd);
	}
  
	pid = fork();
	if (pid < 0)
		return -1;

	if (pid > 0)
		_exit(0);

	if ( setsid() < 0 )
		return -1;

	return 0;
}
