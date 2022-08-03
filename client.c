#include "client.h"

void reconnect(int sig)
{
	printf("서버와의 연결이 끊어졌습니다.\n");
	printf("재접속 시도 중...\n");
	my_sock = socket(PF_INET,SOCK_STREAM,0);
	while (connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
	{
		my_sock = socket(PF_INET,SOCK_STREAM,0);
		sleep(1);
		printf("재접속 시도 중...\n");
	}
	printf("연결 성공\n");
}

void connect_socket()
{
	my_sock = socket(PF_INET,SOCK_STREAM,0);
	if(my_sock == -1)
		printf("socket error \n");
	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(DEFAULT_IP);
	serv_addr.sin_port=htons(DEFAULT_PORT);

	if(connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr))==-1)
	{
		while (1)
		{
			my_sock = socket(PF_INET,SOCK_STREAM,0);
			printf("연결 실패. 5초 뒤 재접속 요청합니다.\n");
			sleep(5);
			if (connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) != -1)
				break;
		}
	}
	
	pid_t pid[2];
	pid[0] = fork();
	if (pid[0] == 0)
	{
		char message[15] = " hello";
		printf("%d\n", write(my_sock,message,6));
		exit(0);
	}
	waitpid(-1, NULL, NULL);
	sleep(1);
		char message[15] = "hello2";
		printf("%d\n", write(my_sock,message,6));
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

int main()
{
	signal(SIGPIPE, reconnect);
	connect_socket();
	return 0;
}