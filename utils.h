#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

char	**ft_split(char const *s, char c);
char	*ft_substr(char const *s, unsigned int start, size_t len);
char	*ft_strdup(const char *src);
char	*ft_strjoin(char const *s1, char const *s2);
int	ft_strlen(const char *s);
int rowcnt(char **matrix);