#ifndef SENDER_H
# define SENDER_H

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include "collector.h"
#include <fcntl.h>
#include <errno.h>

#define DEFAULT_PORT 4242
#define DEFAULT_IP "127.0.0.1"

extern int my_sock;
extern struct sockaddr_in serv_addr;


void snd(void *message, int size);
void connect_socket(packet *packet);
void reconnect(int sig);


#endif