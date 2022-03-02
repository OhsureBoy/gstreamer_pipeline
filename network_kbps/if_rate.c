/*
 version 2.0.7wt
 by mihvoi@rdsnet.ro
 first-level cleanups, timer fixes and few enhancements by willy tarreau.
 2005/11/20: addition of interface selection and logging output by w.t.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#define MAX_LINE_FIS_PROC_SIZE 1024
#define MAX_NR_INTERFETE 100
#define MAX_NR_COUNTERI 20
#define SEPARATORI " \t\n\r"

unsigned long long get_send_byte(char *netwrok_name);

void usage()
{
	fprintf(stderr, "Usage: if_rate [-l] [ -i ifname ]* [interval_in_seconds] (1..60, default 3)\n");
	exit(1);
}

int main(int argc, char **argv)
{

	unsigned long long before = 0;
	unsigned long long after = 0;

	while(1) {
		
		before = get_send_byte("eth0");
		sleep(1);

		after = get_send_byte("eth0");

		after = after - before;
		printf("\n value : %3.1f \n", after * 8.0 / 1024.0  );
	}

	
}

unsigned long long get_send_byte(char *netwrok_name)
{

	int nr_crt, i;
	char *fis_dev = "/proc/net/dev";
	char *p;
	char *p_supp;
	char *if_list = NULL;
	int arg_log = 0;
	FILE *f;

	unsigned long long int counteri[MAX_NR_INTERFETE][MAX_NR_COUNTERI];
	unsigned long long int counteri_anterior[MAX_NR_INTERFETE][MAX_NR_COUNTERI];

	unsigned long long int tmp_uint;
	char buff[MAX_LINE_FIS_PROC_SIZE + 1]; //  +1 pentru '\0'

	int if_name_size = strlen(netwrok_name);

	if (if_list)
	{
		/* add 'ifname,' to the string */
		if_list = realloc(if_list, strlen(if_list) + 1 + if_name_size + 1);
		sprintf(if_list + strlen(if_list), "%s,", netwrok_name);
	}
	else
	{
		/* start the string with ',ifname,' */
		if_list = malloc(if_name_size + 3);
		sprintf(if_list, ",%s,", netwrok_name);
	}

	f = fopen(fis_dev, "r");
	if (f == NULL)
	{
		fprintf(stderr, "Can not open file:\"%s\"", fis_dev);
		exit(1);
	}

	nr_crt = -1;

	while (nr_crt < MAX_NR_INTERFETE - 2)
	{	
		//같은 이름 찾는 곳

		char search_name[32];
		char *p1;

		nr_crt++;
		p1 = p = fgets(buff, MAX_LINE_FIS_PROC_SIZE, f);
		if (p == NULL)
			break;

		p = strchr(buff, ':');
		if (p == NULL)
			continue;

		*p++ = '\0';
		while (p1 < p && (*p1 == ' ' || *p1 == '\t'))
			p1++;

		/* the user has selected only some interfaces, let's check */
		if (if_list && *if_list != 0)
		{
			snprintf(search_name, sizeof(search_name) - 1, ",%s,", p1);
			search_name[sizeof(search_name) - 1] = 0;
			// printf("if_list=<%s>, search_name=<%s>\n",if_list, search_name);
			if (strstr(if_list, search_name) == NULL)
				continue;
		}

		i = -1;

		p = strtok(p, SEPARATORI);

		//값 가져오는 곳
		while (p != NULL)
		{
			i++;
			if (i > MAX_NR_COUNTERI)
				break;

			tmp_uint = strtoull(p, &p_supp, 10);
			if (p_supp == NULL)
			{
				/* fprintf(stderr, "Invalid format for number argument :\"%s\"\n", p); */
				break;
			}

			counteri[nr_crt][i] = tmp_uint;

			unsigned long long delta;

			if (i == 8)
			{ // bytes
				return counteri[nr_crt][i] - counteri_anterior[nr_crt][i];
			}

			counteri_anterior[nr_crt][i] = counteri[nr_crt][i];
			p = strtok(NULL, SEPARATORI);
		}
		if (!arg_log)
			printf("\n");
	}
	if (fseek(f, 0, SEEK_SET) != 0)
	{
		fprintf(stderr, "Can not fseek to the start of the file %s\n", fis_dev);
		exit(1);
	}
}
