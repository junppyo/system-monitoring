#include <stdio.h>
#include <string.h>

int main()
{
	char *s;
	int a, b, c;
	FILE *fd;
	fd = fopen("./a.txt", "r");
	fscanf(fd, "%d %*[^\n]d", &a);
	printf("%d\n", a);
	fscanf(fd, "%d", &b);
	printf("%d\n", b);
/*	
fscanf(fd, "%[^ ] %[^ ] %[^ ]", &a, &b, &c);
	printf("%d %d %d\n", a, b, c);
	fscanf(fd, "%d %d %d", &a, &b, &c);
	printf("%d %d %d\n", a, b, c);
*/
}
