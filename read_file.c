#include "read_file.h"

static void copy(char *s1, char *s2)
{
	int i = -1;
	while (s2[++i])
		s1[i] = s2[i];
	s1[i] = '\0';
}

void read_cpu(packet *node)
{
	FILE *fd;
	char *tmp;
	char buf[4096];
	int i = 0;
	unsigned long buffer, cache;
	cpuinfo *cpuinfo = malloc(sizeof(struct s_cpuinfo));

	fd = fopen("/proc/stat", "r");
	fgets(buf, sizeof(buf), fd);
	tmp = strtok(buf, " ");
	while (tmp != NULL)
	{
		i++;
		if (i == 2)
			cpuinfo->cpu_usr = atoi(tmp);
		else if (i == 4)
			cpuinfo->cpu_sys = atoi(tmp);
		else if (i == 5)
			cpuinfo->cpu_idle = atoi(tmp);
		else if (i == 6)
			cpuinfo->cpu_iowait = atoi(tmp);
		tmp = strtok(NULL, " ");
	}
	fclose(fd);
	cpuinfo->id = clientid;
	cpu_append(node, cpuinfo);
}

void read_mem(packet *node)
{
	FILE *fd;
	char *tmp;
	char buf[4096];
	int i = 0;
	unsigned long buffer, cache;

	meminfo *meminfo = malloc(sizeof(struct s_meminfo));

	fd = fopen("/proc/meminfo", "r");
	char *s = NULL;
	while (fgets(buf, sizeof(buf), fd))
	{
		char *s2 = ft_strjoin(s, buf);
		free_s(s);
		s = s2;
	}
	tmp = strtok(s, "\n ");
	while (tmp != NULL)
	{
		i++;
		if (i == 0 * 3 + 2)
			meminfo->mem_total = atol(tmp);
		else if (i == 1 * 3 + 2)
			meminfo->mem_free = atol(tmp);
		else if (i == 3 * 3 + 2)
			buffer = atoi(tmp);
		else if (i == 4 * 3 + 2)
			cache = atoi(tmp);
		else if (i == 14 * 3 + 2)
			meminfo->mem_swap = atol(tmp);
		tmp = strtok(NULL, "\n ");
	}
	free_s(s);
	meminfo->mem_used = meminfo->mem_total - meminfo->mem_free - buffer - cache;
	fclose(fd);
	meminfo->id = clientid;
	mem_append(node, meminfo);
}

void read_net(packet *node)
{
	FILE *fd;
	char *tmp;
	char buf[4096];
	int i = 0;

	netinfo *netinfo = malloc(sizeof(struct s_netinfo));
	netinfo->packet_in_cnt = 0;
	netinfo->packet_in_byte = 0;
	netinfo->packet_out_cnt = 0;
	netinfo->packet_out_byte = 0;
	fd = fopen("/proc/net/dev", "r");
	fgets(buf, sizeof(buf), fd);
	fgets(buf, sizeof(buf), fd);
	while (fgets(buf, sizeof(buf), fd))
	{
		i = 0;
		tmp = strtok(buf, " ");
		while (tmp != NULL)
		{
			i++;
			if (i == 2)
				netinfo->packet_in_byte += atoi(tmp);
			else if (i == 3)
				netinfo->packet_in_cnt += atoi(tmp);
			else if (i == 10)
				netinfo->packet_out_byte += atoi(tmp);
			else if (i == 11)
				netinfo->packet_out_cnt += atoi(tmp);
			tmp = strtok(NULL, " ");
		}
	}
	netinfo->id = clientid;
	net_append(node, netinfo);
	fclose(fd);
}

void read_proc(packet *node)
{
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
	char buf[8192];
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
		fgets(buf, sizeof(buf), fd);
		char *tmp = strtok(buf, " ");
		int i = 0;
		while (tmp != NULL)
		{
			i++;
			if (i == 1)
				proc->pid = atoi(tmp);
			else if (i == 2)
				copy(&proc->name[0], tmp);
			else if (i == 4)
				proc->ppid = atoi(tmp);
			else if (i == 14)
				utime = atol(tmp);
			else if (i == 15)
				stime = atol(tmp);
			else if (i == 22)
				starttime = atol(tmp);
			tmp = strtok(NULL, " ");
		}
		struct stat st;
		if (stat(file_name, &st) == -1)
			printf("stat get fail\n");
		uname = getpwuid(st.st_uid)->pw_name;
		i = -1;
		while (uname[++i])
			proc->uname[i] = uname[i];
		proc->uname[i] = '\0';
		fclose(fd);
		fd = fopen("/proc/uptime", "r");
		fscanf(fd, "%f %*f", &uptime);
		fclose(fd);
		proc->cputime = (float)(utime + stime) / hertz;
		proc->cpuusage = (((utime + stime) / hertz) / (uptime - (starttime / hertz))) * 100;
		char *proc_cmdline = ft_strjoin(file_name, "/cmdline");
		fd = fopen(proc_cmdline, "r");
		fscanf(fd, "%s", cmdline);
		proc->cmdline_len = ft_strlen(cmdline);
		proc->cmdline = ft_strdup(cmdline);
		fclose(fd);
		append(list, proc);
		cmdline[0] = '\0';
		free_s(file_name);
		free_s(proc_stat);
		free_s(proc_cmdline);
	}
	closedir(dp);
	list->id = clientid;
	list->len = getsize(list);
	plist_append(node, list);
}
