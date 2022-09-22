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
		reconnect(0);
		return ;
	}
	itoa(ret, s);
	// printf("%d send\n", ret);
	char *logmessage = ft_strjoin(s, "byte send");
	writelog(logfd, TRACE, logmessage);
	free_s(logmessage);
}

void reconnect(int sig)
{
	// printf("disconnect\n");
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
		{
			writelog(logfd, DEBUG, "remake socket");
			my_sock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
		}
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
	float cpu_usage;
	float mem_usage;
	struct s_usagelist *usagelist = usagelist_init();
	time_t nowtime = time(NULL);
	time_t tmptime = time(NULL);

	while(1)
	{
		if (packet->cpuqueue->next)
		{
			header->type = 'c';
			header->size = sizeof(p_head) + sizeof(cpuinfo);
			cpuinfo *tmp = cpu_pop(packet);
			cpu_usage = ((float)tmp->cpu_usr / (float)(tmp->cpu_usr + tmp->cpu_sys + tmp->cpu_iowait + tmp->cpu_idle));
			tmp->delta_usage = cpuusage_append(usagelist, cpu_usage);
			char *message = make_packet(header, sizeof(p_head), tmp, sizeof(cpuinfo));
			snd(message, sizeof(p_head) + sizeof(cpuinfo));
			free_s(message);
			free_s(tmp);
		}
		if (packet->memqueue->next)
		{
			header->type = 'm';
			header->size = sizeof(p_head) + sizeof(meminfo);
			meminfo *tmp = mem_pop(packet);
			mem_usage = (float)tmp->mem_used / (float)tmp->mem_total;
			tmp->delta_usage = memusage_append(usagelist, mem_usage);
			char *message = make_packet(header, sizeof(p_head), tmp, sizeof(meminfo));
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
			// message = make_packet(header, sizeof(p_head), list, sizeof(plist));
			// size += sizeof(p_head) + sizeof(plist);
			while ((pinfo = pop(list)) != 0)
			{

				i++;
					// printf("pid:%d name:%s cmdlinelen: %d\n", pinfo->pid, pinfo->name, pinfo->cmdline_len);
				tmp = make_packet(message, size, pinfo, sizeof(procinfo));
				if (message)
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
				else
					pinfo->cmdline = NULL;
				free_s(pinfo);
				if (size > 60000)
				{
					list->len = i;
					i = 0;
					header->size = sizeof(p_head) + sizeof(plist) + size;
					tmp = make_packet(header, sizeof(p_head), list, sizeof(plist));
					char *real = make_packet(tmp, sizeof(p_head)+sizeof(plist), message, size);
					snd(real, header->size);
					// snd(message, size);
					free_s(message);
					free_s(tmp);
					free_s(real);
					message = NULL;
					size = 0;
				}
			}
			list->len = i;
			header->size = sizeof(p_head) + sizeof(plist) + size;
			tmp = make_packet(header, sizeof(p_head), list, sizeof(plist));
			char *real = make_packet(tmp, sizeof(p_head)+sizeof(plist), message, size);
			snd(real, header->size);
			// snd(message, size);
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
			char *sndmessage = make_packet(tmp, sizeof(p_head)+sizeof(disklist), message, size);
			snd(sndmessage, header->size);
			free_s(message);
			free_s(dlist->HEAD);
			free_s(dlist);
			free_s(tmp);
			free_s(sndmessage);
		}

		if (tmptime < time(NULL) - 3)
		{
			while (usagelist->cpuTAIL->prev != usagelist->cpuHEAD->next && usagelist->cpuTAIL->prev->collect_time < time(NULL) - 3)
				cpuusage_pop(usagelist);
			while (usagelist->memTAIL->prev != usagelist->memHEAD->next && usagelist->memTAIL->prev->collect_time < time(NULL) - 3)
				memusage_pop(usagelist);
			header->type = 'a';
			header->size = sizeof(p_head) + (sizeof(float) * 2);
			float cpuusage_avg = (float)usagelist->cputotal / (float)usagelist->cpulen;
			float memusage_avg = (float)usagelist->memtotal / (float)usagelist->memlen;
			printf("cpuavg: %f  memavg: %f\n", cpuusage_avg, memusage_avg);
			char *tmp = make_packet(header, sizeof(p_head), &cpuusage_avg, sizeof(float));
			char *message = make_packet(tmp, sizeof(p_head) + sizeof(float), &memusage_avg, sizeof(float));
			snd(message, header->size);
			tmptime = time(NULL);
			free_s(tmp);
			free_s(message);
		}
	}
	free_s(header);
	close(my_sock);
	writelog(logfd, DEBUG, "전송 완료");
	
	return 0;
}
