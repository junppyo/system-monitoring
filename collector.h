#include <stdio.h> 
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include "utils.h"
#include "struct.h"
#include <pwd.h>
#include <sys/stat.h>
#include <pthread.h>

void *os_collect(void *packe);
void *proc_collect(void *packe);
void collect(packet *queue);