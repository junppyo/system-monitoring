#ifndef CLIENT_H
# define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sender.h"

int my_sock;
struct sockaddr_in serv_addr;
FILE *logfd;
unsigned int CPU_CYCLE;
unsigned int MEM_CYCLE;
unsigned int NET_CYCLE;
unsigned int PROC_CYCLE;
int clientid;

#endif