#ifndef CLIENT_H
# define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sender.h"

int my_sock;
struct sockaddr_in serv_addr;

pthread_t collect_thread[5];
FILE *logfd;
unsigned int CPU_CYCLE;
unsigned int MEM_CYCLE;
unsigned int NET_CYCLE;
unsigned int PROC_CYCLE;
unsigned int DISK_CYCLE;
int clientid;
int flag;
pthread_mutex_t logmutex;


#endif