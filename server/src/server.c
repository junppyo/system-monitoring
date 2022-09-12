#include "server.h"

int serv_sock;

void quit(int sig)
{
	writelog(logfd, DEBUG, "exit by SIGINT");
	fclose(logfd);
	close(serv_sock);
	exit(0);
}

packet *queue_init()
{
	packet *queue = malloc(sizeof(packet));
	queue->cpuqueue = malloc(sizeof(cpuinfo));
	queue->cpuqueue->next = NULL;
	queue->memqueue = malloc(sizeof(meminfo));
	queue->memqueue->next = NULL;
	queue->netqueue = malloc(sizeof(netinfo));
	queue->netqueue->next = NULL;
	queue->plistqueue = malloc(sizeof(plist));
	queue->plistqueue->next = NULL;
	queue->diskqueue = malloc(sizeof(disklist));
	queue->diskqueue->next = NULL;
	queue->udpqueue = malloc(sizeof(udppacket));
	queue->udpqueue->next = NULL;
	queue->matricqueue = malloc(sizeof(udpmatric));
	queue->matricqueue->next = NULL;
	pthread_mutex_unlock(&queue->cpu_mutex);
	pthread_mutex_unlock(&queue->mem_mutex);
	pthread_mutex_unlock(&queue->net_mutex);
	pthread_mutex_unlock(&queue->plist_mutex);
	pthread_mutex_unlock(&queue->udp_mutex);
	
	return queue;
}

void queue_free(packet *queue)
{
	pthread_mutex_destroy(&queue->cpu_mutex);
	pthread_mutex_destroy(&queue->mem_mutex);
	pthread_mutex_destroy(&queue->net_mutex);
	pthread_mutex_destroy(&queue->plist_mutex);
	free_s(queue->cpuqueue);
	free_s(queue->memqueue);
	free_s(queue->netqueue);
	free_s(queue->plistqueue);

	free_s(queue);
}

int main()
{
	// daemon_init();
	setbuf(stdout, NULL);
	static struct sigaction	act;
	act.sa_handler = quit;
	sigaction(SIGINT, &act, NULL);

	pthread_t thread[3];

	packet *queue = queue_init();
	
	logfd = fopen("server_log", "a");
	pthread_create(&thread[0], NULL, saver, (void *)queue);
	pthread_create(&thread[1], NULL, tcp_open, queue);
	pthread_create(&thread[2], NULL, udp_open, queue);

	while (1)
	{
	}
	// pthread_join(thread[0], NULL);
	// pthread_join(thread[1], NULL);
	
	queue_free(queue);

	fclose(logfd);
	return 0;
}
