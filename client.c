#include "client.h"

int my_sock;
struct sockaddr_in serv_addr;
FILE *logfd;

int main()
{
	packet *queue = malloc(sizeof(packet));
	queue->cpuqueue = malloc(sizeof(cpuinfo));
	queue->cpuqueue->next = NULL;
	queue->memqueue = malloc(sizeof(meminfo));
	queue->memqueue->next = NULL;
	queue->plistqueue = malloc(sizeof(plist));
	queue->plistqueue->next = NULL;

	queue->netqueue = malloc(sizeof(netinfo));
	queue->netqueue->next = NULL;
	logfd = fopen("client_log", "a");
	collect(queue);
	connect_socket(queue);
	
	free_s(queue->cpuqueue);
	free_s(queue->memqueue);
	free_s(queue->netqueue);
	free_s(queue->plistqueue);
	pthread_mutex_destroy(&queue->cpu_mutex);
	pthread_mutex_destroy(&queue->mem_mutex);
	pthread_mutex_destroy(&queue->net_mutex);
	pthread_mutex_destroy(&queue->plist_mutex);
	free_s(queue);
	sleep(2);
	while (1)
	{
	}
	fclose(logfd);
	return 0;
}
