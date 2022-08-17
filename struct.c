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
	

void packet_append(packet *HEAD, packet *node)
{
	node->next = NULL;
	if (!HEAD->next)
	{
		HEAD->next = node;
	}
	else
	{
		packet *tmp = HEAD;
		while (!tmp->next)
			tmp = tmp->next;
		tmp->next = node;
	}
}

packet *packet_pop(packet *HEAD)
{
	packet *ret = HEAD->next;
	HEAD->next = HEAD->next->next;
	return ret;
}