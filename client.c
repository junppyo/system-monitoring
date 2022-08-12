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

void connect_socket(osinfo *os)
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
	while ((pinfo = pop()) != 0)
	{
		write(my_sock, pinfo, sizeof(procinfo));
		printf("%s %d %f\n", pinfo->name, pinfo->pid, pinfo->cputime);
	}

	// while (1)
	// {
	// 	write(my_sock, "www", 3);
	// 	sleep(2);
	// }
/*
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
*/	// waitpid(-1, NULL, NULL);
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

osinfo *os_collect()
{
	osinfo *ret = malloc(sizeof(osinfo));
	unsigned long buffer, cache;
	unsigned long rbyte, sbyte, rpacket, spacket;
	FILE *fd;
	char tmp[256];

	fd = fopen("/proc/stat", "r");
	fscanf(fd, "%*s  %lu %*lu %lu %lu %lu", &ret->cpu_usr, &ret->cpu_sys, &ret->cpu_idle, &ret->cpu_iowait);
	fclose(fd);

	fd = fopen("/proc/meminfo", "r");
	fscanf(fd, "%*s\t%lu%*s\n%*s\t%lu%*s\n%*s\t%*d%*s\n%*s\t%lu%*s\n%*s\t%lu%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s\t%lu",&ret->mem_total, &ret->mem_free, &buffer, &cache, &ret->mem_swap);
	ret->mem_used = ret->mem_total - ret->mem_free - buffer - cache;
//	printf("%lu %lu\n", ret->cpu_usr, ret->cpu_sys);
	fclose(fd);

	ret->packet_in_cnt = 0;
	ret->packet_out_cnt = 0;
	ret->packet_in_byte = 0;
	ret->packet_out_byte = 0;
	fd = fopen("/proc/net/dev", "r");
	fgets(tmp, sizeof(tmp), fd);
	fgets(tmp, sizeof(tmp), fd);
	while (EOF != fscanf(fd, "%*s %lu %lu %*lu %*lu %*lu %*lu %*lu %*lu %lu %lu %*lu %*lu %*lu %*lu %*lu %*lu", &rbyte, &rpacket, &sbyte, &spacket))
	{
		ret->packet_in_byte += rbyte;
		ret->packet_in_cnt += rpacket;
		ret->packet_out_byte += sbyte;
		ret->packet_out_cnt += spacket;
	}
	fclose(fd);
	
	return ret;
}

void proc_collect()
{
	FILE *fd;
	DIR *dp;
	struct dirent *dirp;
	char *file_name;
	float uptime;
	char buf[128];
	char name[256];
	char uname[32];
	char cmdline[4096];
	unsigned long utime;
	unsigned long stime;
	long cutime;
	long cstime;
	int hertz = sysconf(_SC_CLK_TCK);
	unsigned long long starttime;
	dp = opendir("/proc");
	if (!dp)
		printf("open /proc error\n");
	while ((dirp = readdir(dp)) != NULL)
	{
		char *proc_stat = NULL;
		if (dirp->d_type != DT_DIR)
			continue ;
		if (dirp->d_name[0] < '0' || dirp->d_name[0] > '9')
			continue ;
		file_name = ft_strjoin("/proc/", dirp->d_name);
		proc_stat = ft_strjoin(file_name, "/stat");
		fd = fopen(proc_stat, "r");
		if (!fd)
			continue ;
		procinfo *proc = malloc(sizeof(procinfo));

		fscanf(fd, "%d %*c%[^)]s", &proc->pid, proc->name);
		fscanf(fd, ") %*c %d %*d %*d %*d %*d %*u %*lu %*lu %*lu %*lu %lu %lu %ld %ld %*ld %*ld %*ld %*ld %llu %*lu %*ld %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*lu %*d %*d %*u %*u %*llu %*llu %*ld", &proc->ppid, &utime, &stime, &cutime, &cstime, &starttime);
//		proc->name = ft_strdup(name);
		register struct passwd *pw;
		struct stat st;
		char *proc_attr = ft_strjoin(file_name, "/attr");
		if (stat(proc_attr, &st) == -1)
			printf("asfdsf\n");
		proc->uname = ft_strdup(getpwuid(st.st_uid)->pw_name);
		fclose(fd);

		fd = fopen("/proc/uptime", "r");
		fscanf(fd, "%f %*f", &uptime);
		fclose(fd);
		proc->cputime = (float)(utime + stime) / hertz;
		proc->cpuusage = (((utime + stime + cutime + cstime) / hertz) / (uptime - (starttime / hertz))) * 100;
		char *proc_cmdline = ft_strjoin(file_name, "/cmdline");
		fd = fopen(proc_cmdline, "r");
		fscanf(fd, "%s", cmdline);
		proc->cmdline = ft_strdup(cmdline);
		if (cmdline)
			cmdline[0] = '\0';
		fclose(fd);
		append(proc);
		free(file_name);
		free(proc_stat);
		free(proc_attr);
		free(proc_cmdline);
		free(pw);
	}
	procinfo *proc_tmp;
	float c = 0;
/*
	while ((proc_tmp = pop()) != 0)
	{
//	printf("%d %s\n", proc_tmp->pid, proc_tmp->cmdline);
//	printf("%d %s %d %f %s %f %f\n", proc_tmp->pid, proc_tmp->name, proc_tmp->ppid, proc_tmp->cpuusage, proc_tmp->uname, proc_tmp->cpuusage, proc_tmp->cputime);
	c+= proc_tmp->cpuusage;

		if (proc_tmp->name)
			free(proc_tmp->name);
		if (proc_tmp->uname)
			free(proc_tmp->uname);
		if (proc_tmp->cmdline)
			free(proc_tmp->cmdline);
		free(proc_tmp);
	}
	printf("cpu usage : %f", c);
*/
	closedir(dp);
}

int main()
{
	signal(SIGPIPE, reconnect);
	osinfo *s = os_collect();
	proc_collect();
	connect_socket(s);
	return 0;
}
