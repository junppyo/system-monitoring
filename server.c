#include "server.h"

int rcv(void *message, int size)
{
	char s[10];
	int ret = 0;

	if (size <= 0)
		return 0;
	ret = read(client_sock, message, size);

	if (ret < 0)
	{
		if (ret < 0)
			writelog(logfd, TRACE, "수신 실패");
		return 0;
	}
	if (ret < size)
	{
		char *s;
		int tmp;
		char *buf = malloc(size - ret);
		while ((tmp = read(client_sock, buf, size - ret)) > 0)
		{
			s = make_packet(message, ret, buf, tmp);
			ret += tmp;
			free_s(message);
			free_s(buf);
			message = s;
		}
	}
	if (ret != size)
	{
		return -1;
	}
	itoa(ret, s);
	writelog(logfd, TRACE, ft_strjoin(s, "byte received"));
	return ret;
}

void *tcp_open(void *queu)
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;
	packet *queue = (struct s_packet*) queu;

	serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serv_sock == -1)
		writelog(logfd, ERROR, "socket error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DEFAULT_PORT);

	if(bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
	{
		printf("bind error\n");
		writelog(logfd, ERROR, "bind error");
	}

	if(listen(serv_sock,5) == -1)
		writelog(logfd, ERROR, "listen error");

	socklen_t clnt_addr_size = sizeof(client_addr);
	client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);

	procinfo *pinfo;
	plist *list;
	p_head *header = malloc(sizeof(struct s_packethead));
	char *buf = malloc(165483);
	while (1)
	{
		int n = 0;
		if (rcv(header, sizeof(struct s_packethead)) <= 0)
		{
			client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);
			rcv(header, sizeof(struct s_packethead));
		}
		printf("type: %c, size: %d\n", header->type, header->size);
		if (header->type != 'c' && header->type != 'm' && header->type != 'n' && header->type != 'p')
		{
			close(client_sock);
			writelog(logfd, ERROR, "wrong packet received");
			continue;
		}
		if (header->type == 'c')
		{
			cpuinfo *tmp = malloc(sizeof(struct s_cpuinfo));
			if (rcv(tmp, sizeof(struct s_cpuinfo)) != sizeof(struct s_cpuinfo))
			{
				close(client_sock);
				writelog(logfd, ERROR, "wrong packet received");
				continue;
			}
			cpu_append(queue, tmp);
		}
		if (header->type == 'm')
		{	
			meminfo *tmp = malloc(sizeof(struct s_meminfo));
			if (rcv(tmp, sizeof(struct s_meminfo)) != sizeof(struct s_meminfo))
			{
				close(client_sock);
				writelog(logfd, ERROR, "wrong packet received");
				continue;
			}
			mem_append(queue, tmp);
		}
		if (header->type == 'n')
		{
			netinfo *tmp = malloc(sizeof(struct s_netinfo));
			if (rcv(tmp, sizeof(struct s_netinfo)) != sizeof(struct s_netinfo))
			{
				close(client_sock);
				writelog(logfd, ERROR, "wrong packet received");
				continue;
			}
			net_append(queue, tmp);
		}
		if (header->type == 'p')
		{
			int i = 0;
			plist *tmp = malloc(sizeof(struct s_plist));
			if (rcv(tmp, sizeof(struct s_plist)) != sizeof(struct s_plist))
			{
				close(client_sock);
				writelog(logfd, ERROR, "wrong packet received");
				continue;
			}
			tmp->HEAD = malloc(sizeof(struct s_procinfo));
			tmp->HEAD->next = NULL;
			while (i <= tmp->len)
			{
				pinfo = malloc(sizeof(procinfo));
				if (rcv(pinfo, sizeof(procinfo)) != sizeof(procinfo))
				{
					close(client_sock);
					writelog(logfd, ERROR, "wrong packet received");
					continue;
				}
				if (pinfo->cmdline_len)
				{
					pinfo->cmdline = malloc(sizeof(char) * (pinfo->cmdline_len));
					if (rcv(pinfo->cmdline, pinfo->cmdline_len) != pinfo->cmdline_len)
					{
						close(client_sock);
						writelog(logfd, ERROR, "wrong packet received");
						continue;
					}
				}
				append(tmp, pinfo);
				i++;
			}
			plist_append(queue, tmp);
			break ;
		}
	}
	close(serv_sock);
	close(client_sock);
	return 0;
}

void quit(int sig)
{
	fclose(logfd);
	close(serv_sock);
	close(client_sock);
	exit(0);
}

int main()
{
	static struct sigaction	act;
	act.sa_handler = quit;
	sigaction(SIGINT, &act, NULL);
	// daemon_init();
	pthread_t p_thread;
	pthread_t thread;
	packet *queue = malloc(sizeof(packet));
	queue->cpuqueue = malloc(sizeof(cpuinfo));
	queue->cpuqueue->next = NULL;
	queue->memqueue = malloc(sizeof(meminfo));
	queue->memqueue->next = NULL;
	queue->netqueue = malloc(sizeof(netinfo));
	queue->netqueue->next = NULL;
	queue->plistqueue = malloc(sizeof(plist));
	queue->plistqueue->next = NULL;

	logfd = fopen("server_log", "a");
	pthread_create(&thread, NULL, saver, (void *)queue);
	pthread_create(&p_thread, NULL, tcp_open, queue);
	
	// while (1)
	// {
	// }
		sleep(3);
	
	pthread_mutex_destroy(&queue->cpu_mutex);
	pthread_mutex_destroy(&queue->mem_mutex);
	pthread_mutex_destroy(&queue->net_mutex);
	pthread_mutex_destroy(&queue->plist_mutex);
	free_s(queue->cpuqueue);
	free_s(queue->memqueue);
	free_s(queue->netqueue);
	free_s(queue->plistqueue);

	free_s(queue);
	fclose(logfd);
	
	return 0;
}
