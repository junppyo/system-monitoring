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

void connect_socket(packet *packet)
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
	p_head header;
	while(1)
	{
		if (packet->cpuqueue->next)
		{
			header.type = 'c';
			snd(&header, sizeof(p_head));
			snd(cpu_pop(packet), sizeof(struct s_cpuinfo));
		}
		if (packet->memqueue->next)
		{
			header.type = 'm';
			snd(&header, sizeof(p_head));
			snd(mem_pop(packet), sizeof(struct s_meminfo));
		}
		if (packet->netqueue->next)
		{
			header.type = 'n';
			snd(&header, sizeof(p_head));
			snd(net_pop(packet), sizeof(struct s_netinfo));
		}
		if (packet->plistqueue->next)
		{
			header.type = 'p';
			snd(&header, sizeof(p_head));
			plist *plist = plist_pop(packet);
			snd(plist, sizeof(struct s_procinfo));
			while ((pinfo = pop(plist)) != 0)
			{
				snd(pinfo, sizeof(procinfo));
				snd(pinfo->cmdline, pinfo->cmdline_len);
			}
		}
	}

	writelog(logfd, DEBUG, "전송 완료");

	close(my_sock);
}

int main()
{
	packet *queue = malloc(sizeof(packet));
	queue->cpuqueue = malloc(sizeof(cpuinfo));
	queue->cpuqueue->next = NULL;
	queue->memqueue = malloc(sizeof(meminfo));
	queue->memqueue->next = NULL;
	queue->netqueue = malloc(sizeof(netinfo));
	queue->netqueue->next = NULL;
	queue->plistqueue = malloc(sizeof(plist));
	queue->plistqueue->next = NULL;

	logfd = fopen("client_log", "a");
	collect(queue);
	connect_socket(queue);
	
	while (1)
	{
		sleep(1);
	}
	return 0;
}
