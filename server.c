#include "server.h"

int rcv(void *message, int size)
{
	char s[10];
	int ret = 0;

	if (size <= 0)
		return 0;
	ret = read(client_sock, message, size);
	if (ret <= 0)
		return 0;
	itoa(size, s);
	writelog(logfd, TRACE, ft_strjoin(s, "byte 받음"));
	return ret;
}

void tcp_open(packet *queue)
{
	struct sockaddr_in serv_addr;
	struct sockaddr_in client_addr;

	serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(serv_sock == -1)
		writelog(logfd, ERROR, "socket error");

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(DEFAULT_PORT);

	if(bind(serv_sock,(struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
		printf("bind error\n");

	if(listen(serv_sock,5) == -1)
		printf("listen error\n");

	socklen_t clnt_addr_size = sizeof(client_addr);
	client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);

	procinfo *pinfo;
	char *s = NULL;


	while (1)
	{
		packet *node = malloc(sizeof(struct s_packet));
		int i = 0;

		if (rcv(node, sizeof(struct s_packet)) <= 0)
		{
			client_sock = accept(serv_sock,(struct sockaddr*)&client_addr,&clnt_addr_size);
			rcv(node, sizeof(struct s_packet));
		}
		// printf("%d\n", node->proc_len);
		plist *list = malloc(sizeof(struct s_plist));
		list->HEAD = malloc(sizeof(struct s_procinfo));
		list->HEAD->next = NULL;
		node->osinfo = malloc(sizeof(struct s_osinfo));
		rcv(node->osinfo, sizeof(struct s_osinfo));
		while (i <= node->proc_len)
		{
			pinfo = malloc(sizeof(struct s_procinfo));
			rcv(pinfo, sizeof(struct s_procinfo));
			pinfo->cmdline = malloc(sizeof(char) * (pinfo->cmdline_len + 1));
			rcv(pinfo->cmdline, pinfo->cmdline_len);
			pinfo->cmdline[pinfo->cmdline_len] = '\0';
			append(list, pinfo);
			i++;
		}
		node->proc = list;
		packet_append(queue, node);
		break;
	}
	close(serv_sock);
	close(client_sock);

//	procinfo *proc_tmp;
}

int main()
{
	pthread_t p_thread;
	packet *queue = malloc(sizeof(packet));


	logfd = fopen("server_log", "a");
	queue->next = NULL;
//	tcp_open(queue);
	pthread_create(&p_thread, NULL, tcp_open, NULL);
	fclose(logfd);
	return 0;
}
