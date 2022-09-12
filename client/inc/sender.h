#ifndef SENDER_H
# define SENDER_H

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include "collector.h"

#define DEFAULT_PORT 4242
#define DEFAULT_IP "127.0.0.1"

extern int my_sock;
extern struct sockaddr_in serv_addr;
extern int flag;
extern pthread_t collect_thread[5];

void snd(void *message, int size);
void *connect_socket(void *packe);
void reconnect(int sig);


#endif