#include "client.h"


packet *init(void)
{
	logfd = fopen("client_log", "w");
	flag = 1;
	packet *queue = malloc(sizeof(packet));
	queue->cpuqueue = malloc(sizeof(cpuinfo));
	queue->cpuqueue->next = NULL;
	queue->memqueue = malloc(sizeof(meminfo));
	queue->memqueue->next = NULL;
	queue->plistqueue = malloc(sizeof(plist));
	queue->plistqueue->next = NULL;
	queue->netqueue = malloc(sizeof(netinfo));
	queue->netqueue->next = NULL;
	queue->matricqueue = malloc(sizeof(udpmatric));
	queue->matricqueue->next = NULL;
	queue->diskqueue = malloc(sizeof(diskinfo));
	queue->diskqueue->next = NULL;

	pthread_mutex_init(&queue->cpu_mutex, NULL);
	pthread_mutex_init(&queue->mem_mutex, NULL);
	pthread_mutex_init(&queue->net_mutex, NULL);
	pthread_mutex_init(&queue->plist_mutex, NULL);
	pthread_mutex_init(&queue->udp_mutex, NULL);
	pthread_mutex_init(&queue->matric_mutex, NULL);
	pthread_mutex_init(&queue->disk_mutex, NULL);

	FILE *config = fopen("client.config", "r");
	if (!config)
	{
		writelog(logfd, ERROR, "Can't Read Config File");
		clientid = 1;
		CPU_CYCLE = 1000000;
		MEM_CYCLE = 1000000;
		NET_CYCLE = 1000000;
		PROC_CYCLE = 1000000;
	}
	else
	{
		char buf[512];
		char *tmp;

		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		clientid = atoi(tmp);

		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		CPU_CYCLE = atof(tmp) * 1000000;
		// CPU_CYCLE = atof(tmp) * 100000;

		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		MEM_CYCLE = atof(tmp) * 1000000;
		// MEM_CYCLE = atof(tmp) * 100000;
		
		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		NET_CYCLE = atof(tmp) * 1000000;
		// NET_CYCLE = atof(tmp) * 100000;
		
		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		PROC_CYCLE = atof(tmp) * 1000000;
		// PROC_CYCLE = atof(tmp) * 100000;

		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		DISK_CYCLE = atof(tmp) * 1000000;
		// DISK_CYCLE = atof(tmp) * 100000;
	}
	
	return (queue);
}

void quit(int sig)
{
	if (sig == SIGSEGV)	
		printf("segfault");
	fclose(logfd);
	exit(0);
}


int main()
{
	setbuf(stdout, NULL);

	static struct sigaction	act;
	act.sa_handler = quit;
	sigaction(SIGINT, &act, NULL);
	sigaction(SIGSEGV, &act, NULL);
	signal(SIGCHLD, SIG_IGN);
	
	// daemon_init();
	packet *queue = init();
	pthread_t thread;

	char s[100];

	sprintf(s, "client id %d is start", clientid);
	writelog(logfd, DEBUG, s);
	collect(queue);
	pthread_create(&thread, NULL, connect_socket,(void*)queue);
	while (1)
	{
		if (flag == 2)
		{
			collect(queue);
			flag = 1;
		}
	}
	pthread_join(thread, 0);
	pthread_join(collect_thread[0], 0);
	pthread_join(collect_thread[1], 0);
	pthread_join(collect_thread[2], 0);
	pthread_join(collect_thread[3], 0);
	pthread_join(collect_thread[4], 0);
	
	free_s(queue->cpuqueue);
	free_s(queue->memqueue);
	free_s(queue->netqueue);
	free_s(queue->plistqueue);
	free_s(queue->udpqueue);
	free_s(queue->matricqueue);
	free_s(queue->diskqueue);
	pthread_mutex_destroy(&queue->cpu_mutex);
	pthread_mutex_destroy(&queue->mem_mutex);
	pthread_mutex_destroy(&queue->net_mutex);
	pthread_mutex_destroy(&queue->plist_mutex);
	pthread_mutex_destroy(&queue->udp_mutex);
	pthread_mutex_destroy(&queue->matric_mutex);
	pthread_mutex_destroy(&queue->disk_mutex);

	free_s(queue);

	writelog(logfd, DEBUG, "exit");
	fclose(logfd);
	
	return 0;
}
