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
#include "receiver.h"
#include <fcntl.h>


extern FILE *logfd;

#endif