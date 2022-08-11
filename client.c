#include "client.h"

char ***column;

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

void connect_socket()
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
	
	// while (1)
	// {
	// 	write(my_sock, "www", 3);
	// 	sleep(2);
	// }

	pid_t pid[2];
	pid[0] = fork();
	if (pid[0] == 0)
	{
		int i = 0;
		while (column[i])
		{
			write(my_sock, column[i][0], ft_strlen(column[i][0]));
			write(my_sock, ",", 1);
			write(my_sock, column[i][1], ft_strlen(column[i][1]));
			write(1, column[i][0], ft_strlen(column[i][0]));
			write(1, ",", 1);
			write(1, column[i][1], ft_strlen(column[i][1]));
			if (column[++i])
			{
				write(my_sock, "\n", 1);
				write(1, "\n", 1);
			}
		}
		sleep(3);
		exit(0);
	}
	// waitpid(-1, NULL, NULL);
	// sleep(1);
	// 	char message[15] = "hello2\n";
	// 	printf("%d\n", write(my_sock,message,7));
	// pid[1] = fork();
	// if (pid[1] == 0)ffffff
	// {nnnnknnnnnnnnvkvkvkvvvvv
	// 	my_sock = socket(PF_INET,SOCK_STREAM,0);
	// 	connect(my_sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr));
	// 	char message[15] = "hello2";
	// 	printf("%d\n", write(my_sock,message,6));
	// 	exit(0);
	// }

	close(my_sock);
}

void free_collect(char* ret, char **row, char ***column)
{
	if (ret)
		free(ret);
	if (row)
	{
		int i = 0;
		while (row[i])
		{
			free(row[i]);
			i++;
		}
		free(row);
	}
	if (column)
	{
		int i = 0;
		while (column[i])
		{
			int j = 0;
			while (column[i][j])
			{
				free(column[i][j]);
				j++;
			}
			free(column[i]);
			i++;
		}
		free(column);
	}
}

void collect()
{
	FILE *fd;
	DIR *dp;
	struct dirent *dirp;
	char *file_name;
	char buf[128];
	dp = opendir("/proc");
	if (!dp)
		printf("open /proc error\n");
	while ((dirp = readdir(dp)) != NULL)
	{
		char *ret = NULL;
		if (dirp->d_type != DT_DIR)
			continue ;
		if (dirp->d_name[0] < '0' || dirp->d_name[0] > '9')
			continue ;
		file_name = ft_strjoin("/proc/", dirp->d_name);
		ret = ft_strjoin(file_name, "/stat");
		fd = fopen(ret, "r");
		if (!fd)
			continue ;
		procinfo *proc = malloc(sizeof(procinfo));
		fscanf(fd, "%d %*c%[^)]s", &proc->pid, proc->name);
		fscanf(fd, ") %*c %d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu %ld %ld %*ld %*ld %*ld %*ld %*llu %*lu %*ld %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*d %*d %*u %*u %*llu %*llu %*ld", &proc->ppid, &proc->utime, &proc->stime, &proc->cutime, &proc->cstime);
		fclose(fd);
		append(proc);
	}
	procinfo *proc_tmp;
	while ((proc_tmp = pop()) != 0)
	{
	printf("%d %s %d %lu %lu %ld %ld\n", proc_tmp->pid, proc_tmp->name, proc_tmp->ppid, proc_tmp->utime, proc_tmp->stime, proc_tmp->cutime, proc_tmp->cstime);
	}
		
	/*	
		
	char *s = malloc(sizeof(char) * 20);
	fp = popen("ls /proc -l | grep ^d | awk '{print $9}'", "r");
	while (fgets(s, sizeof(s), fp) != NULL)
	{
		char *tmp;
		tmp = ft_strjoin(ret, s);
		if (ret)
			free(ret);
		ret = tmp;
	}
	printf("%s\n", ret);
	char **row = ft_split(ret, '\n');
	int row_n = rowcnt(row);
	column = malloc(sizeof(char **) * (row_n + 1));
	column[row_n] = 0;
	int i = 0;
	while (row[i])
	{
		column[i] = ft_split(row[i], ' ');
		i++;
	}
	i = 10;
	while (column[i])
	{
		int j = 0;
		while (column[i][j])
	 	{
	 		printf("%s ", column[i][j]);
	 		j++;
	 	}
	 	printf("\n");
	 	i++;
	 }
*/
//	free_collect(ret, row, column);
}

int main()
{
	signal(SIGPIPE, reconnect);
	collect();
	connect_socket();
	return 0;
}
