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
		writelog(logfd, TRACE, "수신 실패");
		return 0;
	}
	itoa(size, s);
	writelog(logfd, TRACE, ft_strjoin(s, "byte 받음"));
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
	// p_head *header = malloc(sizeof(struct s_packethead));
	p_head *header;
	char *buf = malloc(165483);
	while (1)
	{
		int n = 0;
		if (rcv(buf, 165483) <= 0)
		{
			client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);
			rcv(buf, 165483);
		}
		header = (p_head*) ft_substr(buf, 0, sizeof(p_head));
		// printf("type: %c\n", header->type);
		if (header->type == 'c')
		{
			cpu_append(queue, (cpuinfo *)ft_substr(buf, sizeof(p_head), sizeof(cpuinfo)));
		}
		if (header->type == 'm')
		{
			mem_append(queue, (meminfo *)ft_substr(buf, sizeof(p_head), sizeof(meminfo)));
		}
		if (header->type == 'n')
		{
			net_append(queue, (netinfo *)ft_substr(buf, sizeof(p_head), sizeof(netinfo)));
		}
		// if (header->type == 'p')
		// {
		// 	int i = 0;
		// 	int start = 0;
		// 	list = ft_substr(buf, sizeof(p_head), sizeof(plist));
		// 	list->HEAD = malloc(sizeof(struct s_procinfo));
		// 	list->HEAD->next = NULL;
		// 	start += sizeof(p_head) + sizeof(struct s_plist);
		// 	printf("len %d\n", list->len);
		// 	while (i <= 100)
		// 	{
		// 		pinfo = (procinfo *)ft_substr(buf, start, sizeof(procinfo));
		// 		printf("%d\n", pinfo->cmdline_len);
		// 		start += sizeof(procinfo);
		// 		if (pinfo->cmdline_len){
		// 			pinfo->cmdline = ft_substr(buf, start, pinfo->cmdline_len);
		// 			start += pinfo->cmdline_len;
		// 		}
		// 		i++;
		// 		append(list, pinfo);
		// 	}
		// 	plist_append(queue, list);
		// }
		// if (rcv(header, sizeof(struct s_packethead)) <= 0)
		// {
		// 	client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);_
		// 	rcv(header, sizeof(struct s_packethead));
		// }
		// if (header->type == 'c')
		// {
		// 	cpuinfo *tmp = malloc(sizeof(struct s_cpuinfo));
		// 	rcv(tmp, sizeof(struct s_cpuinfo));
		// 	cpu_append(queue, tmp);
		// }
		// else if (header->type == 'm')
		// {
		// 	meminfo *tmp = malloc(sizeof(struct s_meminfo));
		// 	rcv(tmp, sizeof(struct s_meminfo));
		// 	mem_append(queue, tmp);
		// }
		// else if (header->type == 'n')
		// {
		// 	netinfo *tmp = malloc(sizeof(struct s_netinfo));
		// 	rcv(tmp, sizeof(struct s_netinfo));
		// 	net_append(queue, tmp);
		// }
		// else if (header->type == 'p')
		// {
		// 	int i = 0;
		// 	plist *tmp = malloc(sizeof(struct s_plist));
		// 	rcv(tmp, sizeof(struct s_plist));
		// 	tmp->HEAD = malloc(sizeof(struct s_procinfo));
		// 	tmp->HEAD->next = NULL;
		// 	while (i <= tmp->len)
		// 	{
		// 		pinfo = malloc(sizeof(struct s_procinfo));
		// 		rcv(pinfo, sizeof(struct s_procinfo));
		// 		pinfo->cmdline = malloc(sizeof(char) * (pinfo->cmdline_len + 1));
		// 		rcv(pinfo->cmdline, pinfo->cmdline_len);
		// 		pinfo->cmdline[pinfo->cmdline_len] = '\0';
		// 		append(tmp, pinfo);

		// 		i++;
		// 	}
		// 	plist_append(queue, tmp);
		// }
	}
	close(serv_sock);
	close(client_sock);
	return 0;
}

int main()
{
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
	pthread_create(&p_thread, NULL, tcp_open, queue);
	pthread_create(&thread, NULL, saver, (void *)queue);
	
	while (1)
	{
		// if (queue->cpuqueue->next)
		// {
		// 	cpuinfo *tmp = cpu_pop(queue);
		// 	printf("%lu %lu %lu\n", tmp->cpu_usr, tmp->cpu_sys, tmp->cpu_iowait);
		// }
		// if (queue->memqueue->next)
		// {
		// 	meminfo *tmp = mem_pop(queue);
		// 	printf("%lu %lu %lu\n", tmp->mem_free, tmp->mem_total, tmp->mem_used);
		// }
	}
	fclose(logfd);
	return 0;
}
