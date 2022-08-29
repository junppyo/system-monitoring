#include "sender.h"

void snd(void *message, int size)
{
	int ret;
	char s[10];
	ret = 1;
	if (size <= 0)
		return ;
	ret = write(my_sock, message, size);
	if (ret < 0)
	{
		writelog(logfd, TRACE, "전송 실패");
		return ;
	}
	itoa(ret, s);
	writelog(logfd, TRACE, ft_strjoin(s, "byte send"));
}

void reconnect(int sig)
{
	writelog(logfd, ERROR, "서버와의 연결이 끊어졌습니다.");
	writelog(logfd, DEBUG, "재접속 시도 중...");
	my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	while (connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
	{
		if (errno == EBADF)
			my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
		sleep(1);
		writelog(logfd, DEBUG, "재접속 시도 중...");
	}
	writelog(logfd, DEBUG, "연결 성공");
}

void connect_socket(packet *packet)
{

	static struct sigaction	act;
	act.sa_handler = reconnect;
	sigaction(SIGPIPE, &act, NULL);
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
			if (errno == EBADF)
				my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
			writelog(logfd, DEBUG, "연결 실패. 5초 뒤 재접속 요청합니다.\n");
			sleep(5);
			if (connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) != -1)
				break;
		}	
	}
		writelog(logfd, DEBUG, "연결 성공");
	procinfo *pinfo;
	p_head *header = malloc(sizeof(struct s_packethead));
	plist *list;
	plist *test;
	while(1)
	{
		if (packet->cpuqueue->next)
		{
			header->type = 'c';
			cpuinfo *tmp = cpu_pop(packet);
			char *message = make_packet(header, sizeof(p_head), tmp, sizeof(cpuinfo));
			snd(message, sizeof(p_head) + sizeof(cpuinfo));
			free_s(message);
		}
		if (packet->memqueue->next)
		{
			header->type = 'm';
			meminfo *tmp = mem_pop(packet);
			char *message = make_packet(header, sizeof(p_head), tmp, sizeof(meminfo));
			snd(message, sizeof(p_head) + sizeof(meminfo));
			free_s(message);
		}
		if (packet->netqueue->next)
		{
			header->type = 'n';
			netinfo *tmp = net_pop(packet);
			char *message = make_packet(header, sizeof(p_head), tmp, sizeof(netinfo));
			snd(message, sizeof(p_head) + sizeof(netinfo));
			free_s(message);
		}

		// if (packet->plistqueue->next)
		// {
		// 	header->type = 'p';
		// 	list = plist_pop(packet);
		// 	int size = 0;
		// 	char *tmp = NULL;
		// 	char *message = make_packet(header, sizeof(p_head), list, sizeof(plist));
		// 	char *message;
		// 	size += sizeof(p_head);
		// 	size += sizeof(struct s_plist);
		// 	int i = 0;
		// 	while ((pinfo = pop(list)) != 0)
		// 	{
		// 		tmp = make_packet(message, size, pinfo, sizeof(procinfo));
		// 		free_s(message);
		// 		message = tmp;
		// 		pinfo = (procinfo *)ft_substr(tmp, size, sizeof(procinfo));
		// 		size += sizeof(procinfo);
		// 		if (pinfo->cmdline_len)
		// 		{
		// 			tmp = make_packet(message, size, pinfo->cmdline, pinfo->cmdline_len);
		// 			size += pinfo->cmdline_len;
		// 			free_s(message);
		// 			message = tmp;
		// 		}
		// 		i ++;
		// 		if (i == 100)
		// 			break;
		// 	}
		// 	snd(message, size);
		// 	free_s(message);
		// 	free_s(list->HEAD);
		// 	free_s(list);
		// }

		// if (packet->cpuqueue->next)
		// {
		// 	//lock
		// 	header->type = 'c';
		// 	cpuinfo *tmp = cpu_pop(packet);
		// 	printf("%d\n", sizeof(cpuinfo));
		// 	snd(header, sizeof(struct s_packethead));
		// 	snd(tmp, sizeof(struct s_cpuinfo));
		// 	free_s(tmp);
		// }
		// if (packet->memqueue->next)
		// {
		// 	header->type = 'm';
		// 	meminfo *tmp = mem_pop(packet);
		// 	snd(header, sizeof(struct s_packethead));
		// 	snd(tmp, sizeof(struct s_meminfo));
		// 	free_s(tmp);
		// }
		// if (packet->netqueue->next)
		// {
		// 	header->type = 'n';
		// 	netinfo *tmp = net_pop(packet);
		// 	snd(header, sizeof(struct s_packethead));
		// 	snd(tmp, sizeof(struct s_netinfo));
		// 	free_s(tmp);
		// }
		// if (packet->plistqueue->next)
		// {
		// 	header->type = 'p';
		// 	snd(header, sizeof(struct s_packethead));
		// 	plist *plist = plist_pop(packet);
		// 	printf("%d\n", plist->len);
		// 	snd(plist, sizeof(struct s_plist));

		// 	while ((pinfo = pop(plist)) != 0)
		// 	{
		// 		printf("%d\n", pinfo->pid);
		// 		snd(pinfo, sizeof(procinfo));
		// 		snd(pinfo->cmdline, pinfo->cmdline_len);
		// 		if (pinfo->cmdline_len)
		// 			free_s(pinfo->cmdline);
		// 		free_s(pinfo);
		// 	}
		// 	free_s(plist->HEAD);
		// 	free_s(plist);
		// }
	}
	free_s(header);
	writelog(logfd, DEBUG, "전송 완료");

	close(my_sock);
}
