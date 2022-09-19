#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "struct.h"
#include "utils.h"
#include <time.h>
#include <sys/time.h>

#define DEFAULT_PORT 4243
#define DEFAULT_IP "127.0.0.1"
int my_sock;
struct sockaddr_in serv_addr;
udpbegin *udp_begin_packet;
udpend *udp_end_packet;
int clientid;

static int num = 0;
static int statictime;
static udpmatric matric;

ssize_t (*origin_write)(int fd, const void *buf, size_t n);
int oneminute()
{
	if (!statictime)
	{
		statictime = time(NULL);
		return 0;
	}
	int now = time(NULL);
	if (statictime <= now - 30)
	{
		statictime = now;
		return 1;
	}
	return 0;
}

void __attribute__((constructor)) execute()
{
	udp_begin_packet = malloc(sizeof(udpbegin));
	udp_end_packet = malloc(sizeof(udpend));
	my_sock = socket(PF_INET,SOCK_DGRAM,0);

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(DEFAULT_IP);
	serv_addr.sin_port=htons(DEFAULT_PORT);

	FILE *config = fopen("client.config", "r");
	if (!config)
	{
		clientid = 1;
		return ;
	}
	char buf[128];
	char *tmp;
	fgets(buf, sizeof(buf), config);
	tmp = strtok(buf, "=");
	tmp = strtok(NULL, "=");
	clientid = atoi(tmp);
	matric.id = clientid;
	matric.call_count = 0;
	matric.max_elapse = 0;
	matric.avg_elapse = 0;
	matric.max_byte = 0;
	matric.avg_byte = 0;
}

void getInfo(int fd)
{
	struct sockaddr_in addr;
	int size = sizeof(addr);
	memset(&addr, 0x00, sizeof(addr));

	getpeername(fd, (struct sockaddr *) &addr, &size);
	strcpy(udp_begin_packet->ip, inet_ntoa(addr.sin_addr));
	udp_begin_packet->port = ntohs(addr.sin_port);
	
	udp_begin_packet->pid = getpid();
	udp_begin_packet->pkt_no = num++;
	udp_begin_packet->id = clientid;
	udp_end_packet->id = clientid;
}

ssize_t write (int fd, const void *buf, size_t n)
{
	getInfo(fd);
	
	origin_write = (ssize_t (*)(int, const void *, size_t))dlsym(RTLD_NEXT, "write");

	sendto(my_sock, udp_begin_packet, sizeof(struct s_udpbegin), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	udp_begin_packet->begintime = clock();
	udp_end_packet->byte = (*origin_write)(fd, buf, n);
	udp_end_packet->elapse_time = (double)(clock() - udp_begin_packet->begintime) / CLOCKS_PER_SEC;
	if (oneminute())
		udp_end_packet->flag = 1;
	else
		udp_end_packet->flag = 0;
	sendto(my_sock, udp_end_packet, sizeof(struct s_udpend), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

	matric.call_count++;
	if (matric.max_elapse < udp_end_packet->elapse_time)
		matric.max_elapse = udp_end_packet->elapse_time;
	if (matric.max_byte < udp_end_packet->byte)
		matric.max_byte = udp_end_packet->byte;
	matric.avg_elapse += udp_end_packet->elapse_time;
	matric.avg_byte += udp_end_packet->byte;

	if (udp_end_packet->flag)
	{
		matric.avg_byte /= matric.call_count;
		matric.avg_elapse /= matric.call_count;
		sendto(my_sock, &matric, sizeof(struct s_udpmatric), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
		matric.call_count = 0;
		matric.max_elapse = 0;
		matric.avg_elapse = 0;
		matric.max_byte = 0;
		matric.avg_byte = 0;		
	}
	return udp_end_packet->byte;
}


void __attribute__((destructor)) end()
{
	exit(0);
}