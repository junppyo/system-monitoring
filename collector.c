#include "collector.h"

pthread_mutex_t os_mutex;
pthread_mutex_t proc_mutex;

void *os_collect(void *packe)
{
	pthread_mutex_lock(&os_mutex);
	packet *node;
	node = (packet*) packe;
	osinfo *ret = malloc(sizeof(osinfo));
	unsigned long buffer, cache;
	unsigned long rbyte, sbyte, rpacket, spacket;
	FILE *fd;
	char tmp[256];

	fd = fopen("/proc/stat", "r");
	fscanf(fd, "%*s  %lu %*u %lu %lu %lu", &ret->cpu_usr, &ret->cpu_sys, &ret->cpu_idle, &ret->cpu_iowait);
	fclose(fd);

	fd = fopen("/proc/meminfo", "r");
	fscanf(fd, "%*s\t%lu%*s\n%*s\t%lu%*s\n%*s\t%*d%*s\n%*s\t%lu%*s\n%*s\t%lu%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s%*d%*s\n%*s\t%lu",&ret->mem_total, &ret->mem_free, &buffer, &cache, &ret->mem_swap);
	ret->mem_used = ret->mem_total - ret->mem_free - buffer - cache;
	fclose(fd);

	ret->packet_in_cnt = 0;
	ret->packet_out_cnt = 0;
	ret->packet_in_byte = 0;
	ret->packet_out_byte = 0;
	fd = fopen("/proc/net/dev", "r");
	fgets(tmp, sizeof(tmp), fd);
	fgets(tmp, sizeof(tmp), fd);
	while (EOF != fscanf(fd, "%*s %lu %lu %*u %*u %*u %*u %*u %*u %lu %lu %*u %*u %*u %*u %*u %*u", &rbyte, &rpacket, &sbyte, &spacket))
	{
		ret->packet_in_byte += rbyte;
		ret->packet_in_cnt += rpacket;
		ret->packet_out_byte += sbyte;
		ret->packet_out_cnt += spacket;
	}
	fclose(fd);
	node->osinfo = ret;
	pthread_mutex_unlock(&os_mutex);
	return (0);
}

void *proc_collect(void *packe)
{
	pthread_mutex_lock(&proc_mutex);
	packet *node = (packet *)packe;
	FILE *fd;
	DIR *dp;
	struct dirent *dirp;
	char *file_name;
	float uptime;
	char name[256];
	char *uname;
	char cmdline[4096];
	plist *list = malloc(sizeof(plist));
	list->HEAD = malloc(sizeof(procinfo));
	list->HEAD->next = NULL;
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
		fscanf(fd, ") %*c %d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu %lu %ld %ld %*d %*d %*d %*d %llu %*u %*d %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*u %*d %*d %*u %*u %*u %*u %*d", &proc->ppid, &utime, &stime, &cutime, &cstime, &starttime);
		register struct passwd *pw;
		struct stat st;
		char *proc_attr = ft_strjoin(file_name, "/attr");
		if (stat(proc_attr, &st) == -1)
			printf("stat get fail\n");
		uname = getpwuid(st.st_uid)->pw_name;
		int i = -1;
		while (uname[++i])
			proc->uname[i] = uname[i];
		proc->uname[i] = '\0';
		fclose(fd);
		fd = fopen("/proc/uptime", "r");
		fscanf(fd, "%f %*f", &uptime);
		fclose(fd);
		proc->cputime = (float)(utime + stime) / hertz;
		proc->cpuusage = (((utime + stime + cutime + cstime) / hertz) / (uptime - (starttime / hertz))) * 100;
		char *proc_cmdline = ft_strjoin(file_name, "/cmdline");
		fd = fopen(proc_cmdline, "r");
		fscanf(fd, "%s", cmdline);
		proc->cmdline_len = ft_strlen(cmdline);
		proc->cmdline = ft_strdup(cmdline);
		fclose(fd);
		append(list, proc);
		cmdline[0] = '\0';
		free(file_name);
		free(proc_stat);
		free(proc_attr);
		free(proc_cmdline);
	//	free(pw);
	}
	closedir(dp);
	node->proc = list;
	node->proc_len = getsize(list);

	pthread_mutex_unlock(&proc_mutex);
	return 0;
}

void collect(packet *queue)
{
	pthread_t thread[2];
	packet *packet = malloc(sizeof(packet));
	packet->osinfo = NULL;
	packet->proc = NULL;
	pthread_create(&thread[0], NULL, os_collect, (void *)packet);
	pthread_create(&thread[1], NULL, proc_collect, (void *)packet);
	sleep(0.01);
	// printf("%d\n", packet->proc->HEAD->next->pid);
	// while (packet->osinfo == NULL && packet->proc == NULL)
	// {printf("b");
	// }printf("a");
	pthread_mutex_lock(&os_mutex);
	pthread_mutex_lock(&proc_mutex);
	packet_append(queue, packet);
}

// int main()
// {
// 	packet *queue = malloc(sizeof(packet));
// 	queue->next = NULL;
// 	collect(queue);
// }