#include "saver.h"

MYSQL *conn;
FILE *logfd;

int connect_db(void)
{
	conn = mysql_init(NULL);
	if (!mysql_real_connect(conn, "127.0.0.1", "root", "root", "exem", 0, NULL, 0))
//	if (!mysql_real_connect(conn, server, user, password, 0, NULL, 0))
		return 1;
	return 0;
}

void send_query(char *s)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	write(1, s, ft_strlen(s));
	write(1, "\n", 1);
	mysql_query(conn, s);
	res = mysql_use_result(conn);
}

void *saver(void *queu)
{
	packet *queue = (struct s_packet*) queu;
	char buf[5192];

	while (connect_db())
	{
		writelog(logfd, ERROR, "Fail connect DB");
		sleep(1);
	}

	while (1)
	{
		int i = 0;
		if (queue->cpuqueue->next)
		{
			cpuinfo *tmp = cpu_pop(queue);
			sprintf(buf, "INSERT INTO cpuinfo (id, usr, sys, iowait, idle) VALUES (%d, %lu, %lu, %lu, %lu);", tmp->id, tmp->cpu_usr, tmp->cpu_sys, tmp->cpu_iowait, tmp->cpu_idle);
			send_query(buf);
		}
		if (queue->memqueue->next)
		{
			meminfo *tmp = mem_pop(queue);
			sprintf(buf, "INSERT INTO meminfo (id, free, total, used, swap) VALUES (%d, %lu, %lu, %lu, %lu);", tmp->id, tmp->mem_free, tmp->mem_total, tmp->mem_used, tmp->mem_swap);
			send_query(buf);
		}
		if (queue->netqueue->next)
		{
			netinfo *tmp = net_pop(queue);
			sprintf(buf, "INSERT INTO netinfo (id, in_cnt, in_byte, out_cnt, out_byte) VALUES (%d, %lu, %lu, %lu, %lu);", tmp->id, tmp->packet_in_cnt, tmp->packet_in_byte, tmp->packet_out_cnt, tmp->packet_out_byte);
			send_query(buf);
		}
		if (queue->plistqueue->next)
		{
			plist *tmp = plist_pop(queue);
			char *s = NULL;
			char *tmp2;
			procinfo *pinfo;
			while ((pinfo = pop(tmp)) != NULL)
			{
				if (pinfo->cmdline_len)
				{
					sprintf(buf, "INSERT INTO procinfo (id, name, uname, pid, ppid, cpuusage, cputime, cmdline) VALUES (%d, \"%s\", \"%s\", %d, %d, %f, %f, \"%s\");\n", tmp->id, pinfo->name, pinfo->uname, pinfo->pid, pinfo->ppid, pinfo->cpuusage, pinfo->cputime, pinfo->cmdline);
					send_query(buf);
					tmp2 = ft_strjoin(s, buf);
					free_s(s);
					s = tmp2;
				}
				else
				{

					sprintf(buf, "INSERT INTO procinfo (id, name, uname, pid, ppid, cpuusage, cputime) VALUES (%d, \"%s\", \"%s\", %d, %d, %f, %f);\n", tmp->id, pinfo->name, pinfo->uname, pinfo->pid, pinfo->ppid, pinfo->cpuusage, pinfo->cputime);
					send_query(buf);
					tmp2 = ft_strjoin(s, buf);
					free_s(s);
					s = tmp2;
				}
			}
			send_query(s);
			free_s(s);
		}
	}

	return 0;

}