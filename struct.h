#include <stdlib.h>
#include <unistd.h>

typedef struct s_osinfo
{
	float cpu_usr;
	float cpu_sys;
	float cpu_iowait;
	float cpu_idle;
	int mem_free;
	int mem_total;
	int mem_used;
	int mem_swap;
	int packet_in_cnt;
	int pakcet_out_cnt;
	int packet_in_byte;
	int pakcet_out_byte;
} osinfo;

typedef struct s_procinfo
{
	char name[256];
	int pid;
	int ppid;
	unsigned long utime;
	unsigned long stime;
	long cutime;
	long cstime;
	int cpu;
	int cputime;
	char *username;
	char *cmdline;
	struct s_procinfo *next;	
} procinfo;

void append(procinfo *node);
procinfo *pop();

