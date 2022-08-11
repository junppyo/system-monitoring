#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <pwd.h>

int main()
{
	register struct passwd *pw;
	register uid_t uid;

	printf("%d\n", geteuid());
	pw = getpwuid(32);
	printf("%s\n", pw->pw_name);

}
