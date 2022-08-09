#include <stdio.h>
#include <string.h>

int main()
{
	char *str = "asfd,asdf,adsf";
	char *ptr = strtok(str, ",");
	printf("%s\n", ptr);
/*
	while (ptr != NULL)
	{
		printf("%s\n", ptr);
		ptr = strtok(str, " ");
	}
*/
}
