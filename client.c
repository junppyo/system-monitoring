#include "client.h"

void snd(void *message, int size)
{
	int ret;
	char *s;

	if (size <= 0)
		return ;
	ret = write(my_sock, message, size);
	if (ret < 0)
	{
		writelog(logfd, TRACE, "전송 실패");
		return ;
	}
	itoa(ret, s);
	writelog(logfd, TRACE, ft_strjoin(s, "byte 전송"));
}


void reconnect(int sig)
{
	writelog(logfd, ERROR, "서버와의 연결이 끊어졌습니다.");
	writelog(logfd, DEBUG, "재접속 시도 중...");
	my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	while (connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
	{
		my_sock = socket(PF_INET,SOCK_STREAM,0);
		sleep(1);
		writelog(logfd, DEBUG, "재접속 시도 중...");
	}
	writelog(logfd, DEBUG, "연결 성공");
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
			writelog(logfd, DEBUG, "연결 실패. 5초 뒤 재접속 요청합니다.\n");
			sleep(5);
			if (connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) != -1)
				break;
		}	
	}
		writelog(logfd, DEBUG, "연결 성공");
	procinfo *pinfo;
	packet *packet = packet_pop(queue);

	snd(packet, sizeof(struct s_packet));
	snd(packet->osinfo, sizeof(osinfo));
	while ((pinfo = pop(packet->proc)) != 0)
	{
		snd(pinfo, sizeof(procinfo));
		snd(pinfo->cmdline, pinfo->cmdline_len);
	}

	writelog(logfd, DEBUG, "전송 완료");

	close(my_sock);
}

int main()
{
	packet *queue = malloc(sizeof(packet));
	queue->next = NULL;

	logfd = fopen("client_log", "a");
	while (1)
	{
		collect(queue);
		if (queue->next == NULL)
			continue;
		connect_socket(queue);
		sleep(1);
	}
	return 0;
}
