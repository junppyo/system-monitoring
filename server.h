#ifndef SERVER_H
# define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include<arpa/inet.h>
# include <pthread.h>
#include "utils.h"
#include "struct.h"
#include "saver.h"
#include <fcntl.h>

int rcv(int fd, void *message, int size);

int num = 0;
extern FILE *logfd;
int serv_sock;
struct sockaddr_in serv_addr;
#define DEFAULT_PORT 4242

#endif