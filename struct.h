#ifndef STRUCT_H
# define STRUCT_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

typedef struct s_cpuinfo
{
	int id;
	unsigned long cpu_usr;
	unsigned long cpu_sys;
	unsigned long cpu_iowait;
	unsigned long cpu_idle;
	struct s_cpuinfo *next;
} cpuinfo;

typedef struct s_meminfo
{
	int id;
	unsigned long mem_free;
	unsigned long mem_total;
	unsigned long mem_used;
	unsigned long mem_swap;
	struct s_meminfo *next;
} meminfo;

typedef struct s_netinfo
{
	int id;
	unsigned long packet_in_cnt;
	unsigned long packet_out_cnt;
	unsigned long packet_in_byte;
	unsigned long packet_out_byte;
	struct s_netinfo *next;
} netinfo;

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
	int id;
	procinfo *HEAD;
	procinfo *TAIL;
	int len;
	struct s_plist *next;
} plist;

void append(plist *list, procinfo *node);
procinfo *pop(plist *list);
int getsize(plist *list);

typedef struct s_packethead
{
	char type;
	int size;
} p_head;

typedef struct s_udpbegin
{
	int id;
	unsigned long pid;
	char ip[15];
	int port;
	clock_t begintime;
	int pkt_no;
} udpbegin;

typedef struct s_udpend
{
	int id;
	unsigned long pid;
	int byte;
	double elapse_time;
} udpend;

typedef struct s_udppacket
{
	int id;
	unsigned long pid;
	char ip[15];
	int port;
	unsigned long begintime;
	int pkt_no;
	int byte;
	double elapse_time;
	struct s_udppacket *next;

} udppacket;

typedef struct s_packet
{
	struct s_cpuinfo *cpuqueue;
	struct s_meminfo *memqueue;
	struct s_netinfo *netqueue;
	struct s_plist *plistqueue;
	struct s_udppacket *udpqueue;

	pthread_mutex_t cpu_mutex;
	pthread_mutex_t mem_mutex;
	pthread_mutex_t net_mutex;
	pthread_mutex_t plist_mutex;
	pthread_mutex_t udp_mutex;
} packet;

void cpu_append(packet *packet, cpuinfo *node);
cpuinfo *cpu_pop(packet *packet);

void mem_append(packet *packet, meminfo *node);
meminfo *mem_pop(packet *packet);

void net_append(packet *packet, netinfo *node);
netinfo *net_pop(packet *packet);

void plist_append(packet *packet, plist *node);
plist *plist_pop(packet *packet);

void udp_append(packet *queue, udppacket *node);
udppacket *udp_pop(packet *queue);

#endif

extern unsigned int CPU_CYCLE;
extern unsigned int MEM_CYCLE;
extern unsigned int NET_CYCLE;
extern unsigned int PROC_CYCLE;
extern int clientid;

extern FILE *logfd;