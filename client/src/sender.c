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
	// printf("%d send\n", ret);
	writelog(logfd, TRACE, ft_strjoin(s, "byte send"));
}

void reconnect(int sig)
{
	writelog(logfd, ERROR, "서버와의 연결이 끊어졌습니다.");
	writelog(logfd, DEBUG, "재접속 시도 중...");
	flag = 0;
	pthread_join(collect_thread[0], 0);
	pthread_join(collect_thread[1], 0);
	pthread_join(collect_thread[2], 0);
	pthread_join(collect_thread[3], 0);
	pthread_join(collect_thread[4], 0);
	close(my_sock);
	my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
	while (connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) == -1)
	{
		if (errno == EBADF)
			my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
		sleep(1);
		writelog(logfd, DEBUG, "재접속 시도 중...");
	}
	writelog(logfd, DEBUG, "연결 성공");
	flag = 2;
}

void *connect_socket(void *packe)
{
	static struct sigaction	act;
	act.sa_handler = reconnect;
	sigaction(SIGPIPE, &act, NULL);
	packet *packet = (struct s_packet *)packe;
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
	plist *list;
	diskinfo *dinfo;
	disklist *dlist;
	p_head *header = malloc(sizeof(struct s_packethead));
	int cpu_flag = 0;
	int mem_flag = 0;
	float cpu_usage;
	float mem_usage; 
	
	while(1)
	{
		if (packet->cpuqueue->next)
		{
			header->type = 'c';
			header->size = sizeof(p_head) + sizeof(cpuinfo);
			cpuinfo *tmp = cpu_pop(packet);
			if (cpu_flag == 0)
			{
				cpu_usage = (tmp->cpu_usr / (tmp->cpu_usr + tmp->cpu_sys + tmp->cpu_iowait + tmp->cpu_idle));
				tmp->delta_usage = cpu_usage + 1;
				cpu_flag = 1;
			}
			else
			{
				tmp->delta_usage = (tmp->cpu_usr / (tmp->cpu_usr + tmp->cpu_sys + tmp->cpu_iowait + tmp->cpu_idle)) - cpu_usage;
				cpu_usage = (tmp->cpu_usr / (tmp->cpu_usr + tmp->cpu_sys + tmp->cpu_iowait + tmp->cpu_idle));
			}
			char *message = make_packet(header, sizeof(p_head), tmp, sizeof(cpuinfo));
			// printf("send cpu\n");
			snd(message, sizeof(p_head) + sizeof(cpuinfo));
			free_s(message);
			free_s(tmp);
		}
		if (packet->memqueue->next)
		{
			header->type = 'm';
			header->size = sizeof(p_head) + sizeof(meminfo);
			meminfo *tmp = mem_pop(packet);
			if (mem_flag == 0)
			{
				mem_usage = tmp->mem_used / tmp->mem_total;
				tmp->delta_usage = mem_usage + 1;
				mem_flag = 1;
			}
			else
			{
				tmp->delta_usage = (tmp->mem_used / tmp->mem_total) - mem_usage;
				mem_usage = tmp->mem_used / tmp->mem_total;
			}
			char *message = make_packet(header, sizeof(p_head), tmp, sizeof(meminfo));
			// printf("send mem\n");
			snd(message, sizeof(p_head) + sizeof(meminfo));
			free_s(message);
			free_s(tmp);
		}
		if (packet->netqueue->next)
		{
			header->type = 'n';
			header->size = sizeof(p_head) + sizeof(netinfo);
			netinfo *tmp = net_pop(packet);
			char *message = make_packet(header, sizeof(p_head), tmp, sizeof(netinfo));
			// printf("send net\n");
			snd(message, sizeof(p_head) + sizeof(netinfo));
			free_s(message);
			free_s(tmp);
			// break;
		}

		if (packet->plistqueue->next)
		{
			header->type = 'p';
			list = plist_pop(packet);
			int size = 0;
			char *tmp = NULL;
			char *message = NULL;
			int i = 0;		
			while ((pinfo = pop(list)) != 0)
			{
				// printf("pid: %d cmdline_lne: %d cmdline: %s ft_strlen: %d\n", pinfo->pid, pinfo->cmdline_len, pinfo->cmdline, ft_strlen(pinfo->cmdline));
				tmp = make_packet(message, size, pinfo, sizeof(procinfo));
				free_s(message);
				message = tmp;
				size += sizeof(procinfo);
				if (pinfo->cmdline_len)
				{
					tmp = make_packet(message, size, pinfo->cmdline, pinfo->cmdline_len);
					size += pinfo->cmdline_len;
					free_s(message);
					message = tmp;
					free_s(pinfo->cmdline);
				}
				free_s(pinfo);
				i++;
			}
			header->size = sizeof(p_head) + sizeof(plist) + size;
			tmp = make_packet(header, sizeof(p_head), list, sizeof(plist));
			char *real = make_packet(tmp, sizeof(p_head)+sizeof(plist), message, size);
			snd(real, header->size);
			free_s(message);
			free_s(list->HEAD);
			free_s(list);
			free_s(tmp);
			free_s(real);
			// break ;
		}

		if (packet->diskqueue->next)
		{
			header->type = 'd';
			dlist = disklist_pop(packet);
			int size = 0;
			char *tmp = NULL;
			char *message = NULL;
			int i = 0;
			while ((dinfo = disk_pop(dlist)) != 0)
			{
				if (!dinfo)
					break;
				tmp = make_packet(message, size, dinfo, sizeof(diskinfo));
				free_s(message);
				message = tmp;
				size += sizeof(diskinfo);
				free_s(dinfo);
				i++;
			}
			header->size = sizeof(p_head) + sizeof(disklist) + size;
			tmp = make_packet(header, sizeof(p_head), dlist, sizeof(disklist));
			char *real = make_packet(tmp, sizeof(p_head)+sizeof(disklist), message, size);
			snd(real, header->size);
			free_s(message);
			free_s(dlist->HEAD);
			free_s(dlist);
			free_s(tmp);
			free_s(real);
			// break ;
		}
	}
	free_s(header);
	writelog(logfd, DEBUG, "전송 완료");

	close(my_sock);
	return 0;
}
