#ifndef UTILS_H
# define UTILS_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#define TRACE 0
#define DEBUG 1
#define INFO 2
#define ERROR 3


char	**ft_split(char const *s, char c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strdup(char *src);
char	*ft_strjoin(char const *s1, char const *s2);
int	ft_strlen(const char *s);
int rowcnt(char **matrix);
void writelog(FILE *fd, int type, char *message);

void itoa(int n, char *s);
void free_s(void *a);
char *make_packet(void *s1, int size1, void *s2, int size2);
int daemon_init(void);

#endif