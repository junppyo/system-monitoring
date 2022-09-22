#include "struct.h"


void append(plist *list, procinfo *node)
{
	if (!list->HEAD->next)
	{
		node->next = NULL;
		list->HEAD->next = node;
		list->TAIL = node;
	}
	else
	{
		node->next = NULL;
		list->TAIL->next = node;
		list->TAIL = node;
	}
}

procinfo *pop(plist *list)
{
	if (!list->TAIL)
		return 0;
	procinfo *tmp;
	tmp = list->HEAD->next;
	if (list->HEAD->next == list->TAIL)
	{
		list->TAIL = NULL;
		list->HEAD->next = NULL;
	}
	else{
		list->HEAD->next = list->HEAD->next->next;
	}
	return tmp;
}

int getsize(plist *list)
{
	procinfo *tmp = list->HEAD;
	int i = 0;
	while (tmp->next != list->TAIL)
	{
		tmp = tmp->next;
		i++;
	}
	return i;
}


void cpu_append(packet *packet, cpuinfo *node)
{
	pthread_mutex_lock(&packet->cpu_mutex);
	node->next = NULL;
	int i =0;
	if (!packet->cpuqueue->next)
	{
		packet->cpuqueue->next = node;
	}
	else
	{
		cpuinfo *tmp = packet->cpuqueue;
		while (tmp->next){
			tmp = tmp->next;
			i++;
		}
		tmp->next = node;
	}
	pthread_mutex_unlock(&packet->cpu_mutex);
}

cpuinfo *cpu_pop(packet *packet)
{
	pthread_mutex_lock(&packet->cpu_mutex);
	cpuinfo *ret = packet->cpuqueue->next;
	packet->cpuqueue->next = packet->cpuqueue->next->next;
	pthread_mutex_unlock(&packet->cpu_mutex);
	return ret;
}

void mem_append(packet *packet, meminfo *node)
{
	pthread_mutex_lock(&packet->mem_mutex);
	node->next = NULL;
	if (!packet->memqueue->next)
	{
		packet->memqueue->next = node;
	}
	else
	{
		meminfo *tmp = packet->memqueue;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = node;
	}
	pthread_mutex_unlock(&packet->mem_mutex);
}

meminfo *mem_pop(packet *packet)
{
	pthread_mutex_lock(&packet->mem_mutex);
	meminfo *ret = packet->memqueue->next;
	packet->memqueue->next = packet->memqueue->next->next;
	pthread_mutex_unlock(&packet->mem_mutex);
	return ret;
}

void net_append(packet *packet, netinfo *node)
{
	pthread_mutex_lock(&packet->net_mutex);
	node->next = NULL;
	if (!packet->netqueue->next)
	{
		packet->netqueue->next = node;
	}
	else
	{
		netinfo *tmp = packet->netqueue;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = node;
	}
	pthread_mutex_unlock(&packet->net_mutex);
}

netinfo *net_pop(packet *packet)
{
	pthread_mutex_lock(&packet->net_mutex);
	netinfo *ret = packet->netqueue->next;
	packet->netqueue->next = packet->netqueue->next->next;
	pthread_mutex_unlock(&packet->net_mutex);
	return ret;
}

void plist_append(packet *packet, plist *node)
{
	pthread_mutex_lock(&packet->plist_mutex);
	node->next = NULL;
	if (!packet->plistqueue->next)
	{
		packet->plistqueue->next = node;
	}
	else
	{
		plist *tmp = packet->plistqueue;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = node;
	}
	pthread_mutex_unlock(&packet->plist_mutex);
}

plist *plist_pop(packet *packet)
{
	pthread_mutex_lock(&packet->plist_mutex);
	plist *ret = packet->plistqueue->next;
	packet->plistqueue->next = packet->plistqueue->next->next;
	pthread_mutex_unlock(&packet->plist_mutex);
	return ret;
}

void udp_append(packet *queue, udppacket *node)
{
	node->next = NULL;
	pthread_mutex_lock(&queue->udp_mutex);
	if (!queue->udpqueue->next)
		queue->udpqueue->next = node;
	else
	{
		udppacket *tmp = queue->udpqueue->next;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = node;
	}
	pthread_mutex_unlock(&queue->udp_mutex);

}
udppacket *udp_pop(packet *queue)
{
	pthread_mutex_lock(&queue->udp_mutex);
	udppacket *ret = queue->udpqueue->next;
	queue->udpqueue->next = queue->udpqueue->next->next;
	pthread_mutex_unlock(&queue->udp_mutex);
	return ret;
}


void matric_append(packet *queue, udpmatric *node)
{
	node->next = NULL;
	pthread_mutex_lock(&queue->matric_mutex);
	if (!queue->matricqueue->next)
		queue->matricqueue->next = node;
	else
	{
		udpmatric *tmp = queue->matricqueue->next;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = node;
	}
	pthread_mutex_unlock(&queue->matric_mutex);

}
udpmatric *matric_pop(packet *queue)
{
	pthread_mutex_lock(&queue->matric_mutex);
	udpmatric *ret = queue->matricqueue->next;
	queue->matricqueue->next = queue->matricqueue->next->next;
	pthread_mutex_unlock(&queue->matric_mutex);
	return ret;	
}

void disk_append(disklist *list, diskinfo *node)
{
	if (!list->HEAD->next)
	{
		node->next = NULL;
		list->HEAD->next = node;
		list->TAIL = node;
	}
	else
	{
		node->next = NULL;
		list->TAIL->next = node;
		list->TAIL = node;
	}
}

diskinfo *disk_pop(disklist *list)
{
	if (!list->TAIL)
		return 0;
	diskinfo *tmp;
	tmp = list->HEAD->next;
	if (list->HEAD->next == list->TAIL)
	{
		list->TAIL = NULL;
		list->HEAD->next = NULL;
	}
	else{
		list->HEAD->next = list->HEAD->next->next;
	}
	return tmp;
}

void disklist_append(packet *packet, disklist *node)
{
	pthread_mutex_lock(&packet->disk_mutex);
	node->next = NULL;
	if (!packet->diskqueue->next)
	{
		packet->diskqueue->next = node;
	}
	else
	{
		disklist *tmp = packet->diskqueue;
		while (tmp->next)
			tmp = tmp->next;
		tmp->next = node;
	}
	pthread_mutex_unlock(&packet->disk_mutex);
}

disklist *disklist_pop(packet *packet)
{
	pthread_mutex_lock(&packet->disk_mutex);
	disklist *ret = packet->diskqueue->next;
	packet->diskqueue->next = packet->diskqueue->next->next;
	pthread_mutex_unlock(&packet->disk_mutex);
	return ret;
}

struct s_usagelist *usagelist_init()
{
	struct s_usagelist *queue;

	queue->cpuHEAD = malloc(sizeof(struct s_cpuusage));
	queue->cpuTAIL = malloc(sizeof(struct s_cpuusage));
	queue->memHEAD = malloc(sizeof(struct s_memusage));
	queue->memTAIL = malloc(sizeof(struct s_memusage));

	queue->cpuHEAD->prev = NULL;
	queue->cpuHEAD->next = queue->cpuTAIL;
	queue->cpuTAIL->prev = queue->cpuHEAD;
	queue->cpuTAIL->next = NULL;

	queue->memHEAD->prev = NULL;
	queue->memHEAD->next = queue->memTAIL;
	queue->memTAIL->prev = queue->memHEAD;
	queue->memTAIL->next = NULL;

	queue->cpulen = 0;
	queue->memlen = 0;
	queue->cputotal = 0;
	queue->memtotal = 0;

	pthread_mutex_init(&queue->cpuusage_mutex, NULL);
	pthread_mutex_init(&queue->memusage_mutex, NULL);
	pthread_mutex_unlock(&queue->cpuusage_mutex);
	pthread_mutex_unlock(&queue->memusage_mutex);

	return queue;
}

float cpuusage_append(struct s_usagelist *queue, float usage)
{
	pthread_mutex_lock(&queue->cpuusage_mutex);
	struct s_cpuusage *node = malloc(sizeof(struct s_cpuusage));
	node->usage = usage;
	node->collect_time = time(NULL);
	node->prev = queue->cpuHEAD;
	node->next = queue->cpuHEAD->next;
	queue->cpuHEAD->next->prev = node;
	queue->cpuHEAD->next = node;
	queue->cpulen++;
	queue->cputotal += usage;
	if (queue->cpuTAIL->prev == queue->cpuHEAD)
	{
		queue->cpuTAIL->prev = node;
		pthread_mutex_unlock(&queue->cpuusage_mutex);
		return usage;
	}
	else
	{
		pthread_mutex_unlock(&queue->cpuusage_mutex);
		return queue->cpuTAIL->prev->usage - usage;
	}
}

float memusage_append(struct s_usagelist *queue, float usage)
{
	pthread_mutex_lock(&queue->memusage_mutex);
	struct s_memusage *node = malloc(sizeof(struct s_memusage));
	node->usage = usage;
	node->collect_time = time(NULL);
	node->prev = queue->memHEAD;
	node->next = queue->memHEAD->next;
	queue->memHEAD->next->prev = node;
	queue->memHEAD->next = node;
	queue->memlen++;
	queue->memtotal += usage;
	if (queue->memTAIL->prev == queue->memHEAD)
	{
		queue->memTAIL->prev = node;
		pthread_mutex_unlock(&queue->memusage_mutex);
		return usage;
	}
	else
	{
		pthread_mutex_unlock(&queue->memusage_mutex);
		return queue->memTAIL->prev->usage - usage;
	}
}

void cpuusage_pop(struct s_usagelist *queue)
{
	pthread_mutex_lock(&queue->cpuusage_mutex);
	if (queue->cpuHEAD->next == queue->cpuTAIL)
		return ;
	struct s_cpuusage *ret = queue->cpuTAIL->prev;

	queue->cpulen--;
	queue->cputotal -= ret->usage;
	queue->cpuTAIL->prev->prev->next = queue->cpuTAIL;
	queue->cpuTAIL->prev = queue->cpuTAIL->prev->prev;
	free(ret);
	pthread_mutex_unlock(&queue->cpuusage_mutex);

}

void memusage_pop(struct s_usagelist *queue)
{
	pthread_mutex_lock(&queue->memusage_mutex);
	if (queue->memHEAD->next == queue->memTAIL)
	{
		pthread_mutex_unlock(&queue->memusage_mutex);
		return ;
	}
	struct s_memusage *ret = queue->memTAIL->prev;

	queue->memlen--;
	queue->memtotal -= ret->usage;
	queue->memTAIL->prev->prev->next = queue->memTAIL;
	queue->memTAIL->prev = queue->memTAIL->prev->prev;
	free(ret);
	pthread_mutex_unlock(&queue->memusage_mutex);
}
