#ifndef COLLECTOR_H
# define COLLECTOR_H

#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <sys/stat.h>
#include <pthread.h>
#include "../lib/read_file.h" 
#include "utils.h"
#include "struct.h"

void *cpu_collect(void *packe);
void *mem_collect(void *packe);
void *net_collect(void *packe);
void *proc_collect(void *packe);
void collect(packet *queue);

extern int flag;

extern pthread_t collect_thread[5];
extern pthread_mutex_t logmutex;

#endif