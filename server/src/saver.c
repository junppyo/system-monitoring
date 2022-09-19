#include "saver.h"

MYSQL *conn;
FILE *logfd;

void print_delta(char c, float n)
{

}

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
	// MYSQL_RES *res;
	// MYSQL_ROW row;
	// write(1, s, ft_strlen(s));
	// write(1, "\n", 1);
	mysql_query(conn, s);
}

void chk_data(cpuinfo *info)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	if (info->cpu_usr >= 100000)
	{
		mysql_query(conn, "select idx from cpuinfo order by idx desc limit 1;");
		res = mysql_use_result(conn);
		row = mysql_fetch_row(res);
		FILE *fp = fopen("specialcase", "a");
		fprintf(fp, "%s\n", row[0]);
		writelog(logfd, TRACE, "Saved Special Case");
		mysql_free_result(res);
		fclose(fp);
	}
}


// create new thread in saver, for delta, avg

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
		if (queue->cpuqueue->next)
		{
			cpuinfo *tmp = cpu_pop(queue);
			print_delta('c', tmp->delta_usage);
			sprintf(buf, "INSERT INTO cpuinfo (id, usr, sys, iowait, idle, deltausage) VALUES (%d, %lu, %lu, %lu, %lu, %f);", tmp->id, tmp->cpu_usr, tmp->cpu_sys, tmp->cpu_iowait, tmp->cpu_idle, tmp->delta_usage);
			// sprintf(buf, "INSERT INTO cpuinfo (id, usr, sys, iowait, idle) VALUES (%d, %lu, %lu, %lu, %lu);", tmp->id, (unsigned long)111000, tmp->cpu_sys, tmp->cpu_iowait, tmp->cpu_idle);
			send_query(buf);
			chk_data(tmp);
			free_s(tmp);
		}
		if (queue->memqueue->next)
		{
			meminfo *tmp = mem_pop(queue);
			// print_delta('m', tmp->delta_usage);
			sprintf(buf, "INSERT INTO meminfo (id, free, total, used, swap, deltausage) VALUES (%d, %lu, %lu, %lu, %lu, %f);", tmp->id, tmp->mem_free, tmp->mem_total, tmp->mem_used, tmp->mem_swap, tmp->delta_usage);
			send_query(buf);
			free_s(tmp);
		}
		if (queue->netqueue->next)
		{
			netinfo *tmp = net_pop(queue);
			sprintf(buf, "INSERT INTO netinfo (id, in_cnt, in_byte, out_cnt, out_byte) VALUES (%d, %lu, %lu, %lu, %lu);", tmp->id, tmp->packet_in_cnt, tmp->packet_in_byte, tmp->packet_out_cnt, tmp->packet_out_byte);
			send_query(buf);
			free_s(tmp);
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
					free_s(pinfo->cmdline);
				}
				else
				{
					sprintf(buf, "INSERT INTO procinfo (id, name, uname, pid, ppid, cpuusage, cputime) VALUES (%d, \"%s\", \"%s\", %d, %d, %f, %f);\n", tmp->id, pinfo->name, pinfo->uname, pinfo->pid, pinfo->ppid, pinfo->cpuusage, pinfo->cputime);
					send_query(buf);
				}
				free_s(pinfo);
			}
			send_query(s);
			free_s(s);
			free_s(tmp);
		}
		if (queue->diskqueue->next)
		{
			disklist *tmp = disklist_pop(queue);
			char *s = NULL;
			char *tmp2;
			diskinfo *dinfo;
			while ((dinfo = disk_pop(tmp)) != NULL)
			{
				sprintf(buf, "INSERT INTO diskinfo (id, name, type, total, used, available, mount) VALUES (%d, \'%s\', \'%s\', %lu, %lu, %lu, \'%s\')", dinfo->id, dinfo->name, dinfo->type, dinfo->total, dinfo->used, dinfo->available, dinfo->mounted);
				send_query(buf);
				// printf("%s\n",buf);
				free_s(dinfo);
			}
			free_s(tmp);
		}
		if (queue->udpqueue->next)
		{
			udppacket *tmp = udp_pop(queue);
			sprintf(buf, "INSERT INTO udp (id, pid, ip, port, begintime, pkt_no, byte, elapse_time) VALUES (%d, %lu, \'%s\', %d, %lu, %d, %d, %lf);", tmp->id, tmp->pid, tmp->ip, tmp->port, tmp->begintime, tmp->pkt_no, tmp->byte, tmp->elapse_time);
			send_query(buf);
			free_s(tmp);
		}
		if (queue->matricqueue->next)
		{
			udpmatric *tmp = matric_pop(queue);
			sprintf(buf, "INSERT INTO udpmatric (id, call_count, max_elapse, avg_elapse, max_byte, avg_byte) VALUES (%d, %d, %lf, %lf, %d, %d);", tmp->id, tmp->call_count, tmp->max_elapse, tmp->avg_elapse, tmp->max_byte, tmp->avg_byte);
			send_query(buf);
			free_s(tmp);
		}
	}
	mysql_close(conn);
	return 0;

}