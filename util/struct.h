#ifndef STRUCT_H
# define STRUCT_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>

typedef struct s_cpuinfo
{
	int id;
	unsigned long cpu_usr;
	unsigned long cpu_sys;
	unsigned long cpu_iowait;
	unsigned long cpu_idle;
	float delta_usage;
	struct s_cpuinfo *next;
} cpuinfo;

typedef struct s_meminfo
{
	int id;
	unsigned long mem_free;
	unsigned long mem_total;
	unsigned long mem_used;
	unsigned long mem_swap;
	float delta_usage;
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

typedef struct s_diskinfo
{
	int id;
	char name[128];
	char type[32];
	unsigned long total;
	unsigned long used;
	unsigned long available;
	char mounted[512];
	struct s_diskinfo *next;
} diskinfo;

typedef struct s_disklist
{
	int id;
	diskinfo *HEAD;
	diskinfo *TAIL;
	int len;
	struct s_disklist *next;
} disklist;

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
	int flag;
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

typedef struct s_udpmatric
{
//	call count, max elapse, avg elapse, max byte, avg byte
	int id;
	int call_count;
	double max_elapse;
	double avg_elapse;
	int max_byte;
	int avg_byte;
	struct s_udpmatric *next;
} udpmatric;

typedef struct s_packet
{
	struct s_cpuinfo *cpuqueue;
	struct s_meminfo *memqueue;
	struct s_netinfo *netqueue;
	struct s_plist *plistqueue;
	struct s_udppacket *udpqueue;
	struct s_udpmatric *matricqueue;
	struct s_disklist *diskqueue;

	pthread_mutex_t cpu_mutex;
	pthread_mutex_t mem_mutex;
	pthread_mutex_t net_mutex;
	pthread_mutex_t plist_mutex;
	pthread_mutex_t udp_mutex;
	pthread_mutex_t matric_mutex;
	pthread_mutex_t disk_mutex;
} packet;

struct s_cpuusage
{
	float usage;
	time_t collect_time;
	struct s_cpuusage *prev;
	struct s_cpuusage *next;
};

struct s_memusage
{
	float usage;
	time_t collect_time;
	struct s_memusage *prev;
	struct s_memusage *next;
};

struct s_usagelist
{
	struct s_cpuusage *cpuHEAD;
	struct s_cpuusage *cpuTAIL;
	struct s_memusage *memHEAD;
	struct s_memusage *memTAIL;

	pthread_mutex_t cpuusage_mutex;
	pthread_mutex_t memusage_mutex;

	int cpulen;
	int memlen;
	float cputotal;
	float memtotal;
};

struct s_usagelist *usagelist_init();
float cpuusage_append(struct s_usagelist *queue, float usage);
float memusage_append(struct s_usagelist *queue, float usage);
void cpuusage_pop(struct s_usagelist *queue);
void memusage_pop(struct s_usagelist *queue);


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

void matric_append(packet *queue, udpmatric *node);
udpmatric *matric_pop(packet *queue);

void disk_append(disklist *list, diskinfo *node);
diskinfo *disk_pop(disklist *list);

void disklist_append(packet *packet, disklist *node);
disklist *disklist_pop(packet *queue);

#endif

extern unsigned int CPU_CYCLE;
extern unsigned int MEM_CYCLE;
extern unsigned int NET_CYCLE;
extern unsigned int PROC_CYCLE;
extern int clientid;

extern FILE *logfd;