#include "struct.h"

struct s_procinfo *HEAD = NULL;
struct s_procinfo *TAIL = NULL;


void append(procinfo *node)
{
	if (!HEAD)
	{
		HEAD = node;
		TAIL = node;
	}
	else
	{
		TAIL->next = node;
		TAIL = node;
	}
	node->next = NULL;
}

procinfo *pop()
{
	if (!TAIL)
		return 0;
	procinfo *tmp = HEAD;
	if (tmp == TAIL)
	{
		TAIL = NULL;
		HEAD->next = NULL;
	}
	else
		HEAD = HEAD->next;
	return tmp;
}
	
