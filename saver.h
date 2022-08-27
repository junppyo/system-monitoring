#ifndef SAVER_H
# define SAVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "struct.h"
#include "/usr/include/mysql/mysql.h"
#include "utils.h"

void *saver(void *queu);
int connect_db(void);

#endif