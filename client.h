#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define DEFAULT_PORT 4242
#define DEFAULT_IP "127.0.0.1"


	int my_sock;
	struct sockaddr_in serv_addr;