#ifndef STRUCT_H
# define STRUCT_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


typedef struct s_osinfo
{
	unsigned long cpu_usr;
	unsigned long cpu_sys;
	unsigned long cpu_iowait;
	unsigned long cpu_idle;
	unsigned long mem_free;
	unsigned long mem_total;
	unsigned long mem_used;
	unsigned long mem_swap;
	unsigned long packet_in_cnt;
	unsigned long packet_out_cnt;
	unsigned long packet_in_byte;
	unsigned long packet_out_byte;
} osinfo;

typedef struct s_procinfo
{
	char name[256];
	char uname[32];
	int pid;
	int ppid;
	float cpuusage;
	float cputime;
	int cmdline_len;
	char *cmdline;
	// char cmdline[4096];
	struct s_procinfo *next;	
} procinfo;

typedef struct s_plist
{
	procinfo *HEAD;
	procinfo *TAIL;
} plist;

void append(plist *list, procinfo *node);
procinfo *pop(plist *list);
int getsize(plist *list);

typedef struct s_packet
{
	osinfo *osinfo;
	int proc_len;
	plist *proc;
	struct s_packet *next;
} packet;

void packet_append(packet *HEAD, packet *node);
packet *packet_pop(packet *HEAD);

#endif