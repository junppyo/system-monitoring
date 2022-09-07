#ifndef RECEIVER_H
# define RECEIVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include<arpa/inet.h>
# include <pthread.h>
#include "utils.h"
#include "struct.h"
#include <fcntl.h>

int rcv(int fd, void *message, int size);
void *udp_open(void *queu);
void *tcp_open(void *queu);

extern int serv_sock;
extern struct sockaddr_in serv_addr;
#define DEFAULT_PORT 4242
#define DEFAULT_IP "127.0.0.1"
#endif