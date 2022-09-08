#include "client.h"


packet *init(void)
{
	logfd = fopen("client_log", "a");
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

	FILE *config = fopen("client.config", "r");
	if (!config)
	{
		writelog(logfd, ERROR, "Can't Read Config File");
		clientid = 1;
		CPU_CYCLE = 1;
		MEM_CYCLE = 1;
		NET_CYCLE = 1;
		PROC_CYCLE = 1;
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

		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		MEM_CYCLE = atof(tmp) * 1000000;
		
		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		NET_CYCLE = atof(tmp) * 1000000;
		
		fgets(buf, sizeof(buf), config);
		tmp = strtok(buf, "=");
		tmp = strtok(NULL, "=");
		PROC_CYCLE = atof(tmp) * 1000000;
	}
	
	return (queue);
}

void quit(int sig)
{
	fclose(logfd);
	exit(0);
}


int main()
{
	setbuf(stdout, NULL);

	static struct sigaction	act;
	act.sa_handler = quit;
	sigaction(SIGINT, &act, NULL);
	
	// daemon_init();
	packet *queue = init();
	pthread_t thread;

	char s[100];

	sprintf(s, "client id %d is start", clientid);
	writelog(logfd, DEBUG, s);
	collect(queue);
	pthread_create(&thread, NULL, connect_socket,(void*)queue);
	// connect_socket(queue);
	
	while (1)
	{
		if (flag == 2)
		{
			collect(queue);
			flag = 1;
		}
	}
	
	free_s(queue->cpuqueue);
	free_s(queue->memqueue);
	free_s(queue->netqueue);
	free_s(queue->plistqueue);
	pthread_mutex_destroy(&queue->cpu_mutex);
	pthread_mutex_destroy(&queue->mem_mutex);
	pthread_mutex_destroy(&queue->net_mutex);
	pthread_mutex_destroy(&queue->plist_mutex);
	free_s(queue);
	fclose(logfd);
	return 0;
}
