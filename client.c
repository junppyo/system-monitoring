#include "client.h"

procinfo *HEAD = NULL;
procinfo *TAIL = NULL;

void reconnect(int sig)
{
	printf("서버와의 연결이 끊어졌습니다.\n");
	printf("재접속 시도 중...\n");
	my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	while (connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
	{
		my_sock = socket(PF_INET,SOCK_STREAM,0);
		sleep(1);
		printf("재접속 시도 중...\n");
	}
	printf("연결 성공\n");
}

void connect_socket(packet *queue)
{
	my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
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
	procinfo *pinfo;
	packet *packet = packet_pop(queue);
	write(my_sock, packet, sizeof(struct s_packet));
	write(my_sock, packet->osinfo, sizeof(osinfo));
	
	while ((pinfo = pop(packet->proc)) != 0)
	{
	//	printf("%d %s %s %s\n", pinfo->pid, pinfo->name, pinfo->uname, pinfo->cmdline);
		write(my_sock, pinfo, sizeof(procinfo));
		write(my_sock, pinfo->cmdline, pinfo->cmdline_len);
	}


	close(my_sock);
}

int main()
{
	signal(SIGPIPE, reconnect);


	packet *queue = malloc(sizeof(packet));
	queue->next = NULL;
	collect(queue);
	while (1)
	{
		if (queue->next == NULL)
			continue;
		connect_socket(queue);
		break;
	}
	return 0;
}
