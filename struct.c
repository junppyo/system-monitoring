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
	else
		list->HEAD->next = list->HEAD->next->next;
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
	if (!packet->cpuqueue->next)
	{
		packet->cpuqueue->next = node;
	}
	else
	{
		cpuinfo *tmp = packet->cpuqueue;
		while (!tmp->next)
			tmp = tmp->next;
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
		while (!tmp->next)
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
		while (!tmp->next)
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
		while (!tmp->next)
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
