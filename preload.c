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
#define DEFAULT_PORT 4243
#define DEFAULT_IP "127.0.0.1"
int my_sock;
struct sockaddr_in serv_addr;
udpbegin *udp_begin_packet;
udpend *udp_end_packet;
int clientid;

static int num = 0;

ssize_t (*origin_write)(int fd, const void *buf, size_t n);
ssize_t (*new_write)(int fd, const void *buf, size_t n);

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
	sendto(my_sock, udp_end_packet, sizeof(struct s_udpend), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	// printf("id: %d\npid: %lu\nip: %s\nport: %d\nbegintime: %ld\npkt_no: %d\nbyte: %d, elapsetime: %lf\n", udp_begin_packet->id, udp_begin_packet->pid, udp_begin_packet->ip, udp_begin_packet->port, udp_begin_packet->begintime, udp_begin_packet->pkt_no, udp_end_packet->byte, udp_end_packet->elapse_time);
	return udp_end_packet->byte;
}


void __attribute__((destructor)) end()
{
	exit(0);
}