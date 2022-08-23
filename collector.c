#include "collector.h"

pthread_mutex_t os_mutex;
pthread_mutex_t proc_mutex;

void *os_collect(void *packe)
{
	pthread_mutex_lock(&os_mutex);
	packet *packet = (struct s_packet*) packe;
	read_os(packet);
	pthread_mutex_unlock(&os_mutex);
	return (0);
}

void *proc_collect(void *packe)
{
	pthread_mutex_lock(&proc_mutex);
	packet *packet = (struct s_packet *)packe;
	read_proc(packet);
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