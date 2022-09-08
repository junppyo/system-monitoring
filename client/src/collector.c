#include "collector.h"

void *cpu_collect(void *packe)
{
	writelog(logfd, DEBUG, "cpu collector is start");
	packet *packet = (struct s_packet*) packe;
	while (flag)
	{
		read_cpu(packet);
		writelog(logfd, TRACE, "collected cpu info");
		usleep(CPU_CYCLE);
	}
	writelog(logfd, DEBUG, "cpu collector is stopped");
	return (0);
}

void *mem_collect(void *packe)
{
	writelog(logfd, DEBUG, "mem collector is start");
	packet *packet = (struct s_packet*) packe;
	while (flag)
	{
		read_mem(packet);
		writelog(logfd, TRACE, "collected memory info");
		usleep(MEM_CYCLE);
	}
	writelog(logfd, DEBUG, "mem collector is stopped");
	return (0);
}

void *net_collect(void *packe)
{
	writelog(logfd, DEBUG, "net collector is start");
	packet *packet = (struct s_packet*) packe;
	while (flag)
	{
		read_net(packet);
		writelog(logfd, TRACE, "collected network info");
		usleep(NET_CYCLE);
	}
	writelog(logfd, DEBUG, "net collector is stopped");
	return (0);
}

void *proc_collect(void *packe)
{
	writelog(logfd, DEBUG, "proc collector is start");
	packet *packet = (struct s_packet *)packe;
	while (flag)
	{
		read_proc(packet);
		writelog(logfd, TRACE, "collected process info");
		usleep(PROC_CYCLE);
	}
	writelog(logfd, DEBUG, "proc collector is stopped");
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
