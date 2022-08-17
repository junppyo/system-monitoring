#ifndef READ_FILE_H
# define READ_FILE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include "utils.h"
#include "struct.h"
#include <sys/stat.h>
#include <pwd.h>


void read_os(packet *node);
void read_proc(packet *node);

#endif