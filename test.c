#include "client.h"

int main()
{
	my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(my_sock == -1)
		printf("socket error \n");
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(DEFAULT_IP);
	serv_addr.sin_port=htons(DEFAULT_PORT);
		connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));

	while (1)
	{
		write(my_sock, "asdfsf", 6);
		sleep(1);
	}
	// pid[1] = fork();
	// if (pid[1] == 0)
	// {
	// 	my_sock = socket(PF_INET,SOCK_STREAM,0);
	// 	connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	// 	char message[15] = "hello2";
	// 	printf("%d\n", write(my_sock,message,6));
	// 	exit(0);
	// }

	close(my_sock);
}
