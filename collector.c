#include "collector.h"

void *cpu_collect(void *packe)
{
	packet *packet = (struct s_packet*) packe;
	while (1)
	{
		read_cpu(packet);
		usleep(CPU_CYCLE);
	}
	return (0);
}

void *mem_collect(void *packe)
{
	packet *packet = (struct s_packet*) packe;
	while (1)
	{
		read_mem(packet);
		usleep(MEM_CYCLE);
	}
	return (0);
}

void *net_collect(void *packe)
{
	packet *packet = (struct s_packet*) packe;
	while (1)
	{
		read_net(packet);
		usleep(NET_CYCLE);
	}
	return (0);
}

void *proc_collect(void *packe)
{
	packet *packet = (struct s_packet *)packe;
	while (1)
	{
		read_proc(packet);
		usleep(PROC_CYCLE);
	}
	return 0;
}

void collect(packet *queue)
{
	pthread_t thread[4];
	pthread_create(&thread[0], NULL, cpu_collect, (void *)queue);
	pthread_create(&thread[1], NULL, mem_collect, (void *)queue);
	pthread_create(&thread[2], NULL, net_collect, (void *)queue);
	pthread_create(&thread[3], NULL, proc_collect, (void *)queue);
}
