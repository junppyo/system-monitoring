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

#include <fcntl.h>

int rcv(void *message, int size);

int serv_sock;
int client_sock;
FILE *logfd;
#define DEFAULT_PORT 4242

#endif