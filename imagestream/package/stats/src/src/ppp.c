/* vi:set ts=3: */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#ifdef use_linux_libc5
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#endif
#include<ncurses.h>
#include "stat_if.h"
#include "stats.h"

extern WINDOW *win;

#include <sys/file.h>
#define PPP_USERS_FILE "/tmp/ppp-users"


void getPPP(stat_if *nif)
{
	FILE *fp;
	char start_time[64];
	char buffer[1024], *tmp;
	struct tm tm;
	stat_if *tnif;

	/* Someone's hacking! */
	if (strlen(nif->if_name) > 200)
		return;

	fp = fopen(PPP_USERS_FILE, "r");
	if (!fp)
		return;

	/* Grab the header */
	fscanf(fp, "%*[^\n]%*c");

//	while ((fscanf(fp, "%31s %255s %31s %63[^\n]%*c", ifname, username, ip_address, start_time)) == 4) {
	while ((fgets( buffer, 1024, fp))) {
		tmp = strtok(buffer, "\t");
		if (!tmp)
			continue;
		if (!strcmp(tmp, nif->if_name)) {
			/* username */
			tmp = strtok(NULL, "\t");
			if (!tmp)
				break;
			strncpy(nif->ppp.username, tmp, sizeof(nif->ppp.username));
			nif->ppp.username[sizeof(nif->ppp.username)-1] = '\0';

			/* ip address not used here */
			tmp = strtok(NULL, "\t");
			if (!tmp)
				break;

			/* start time */
			tmp = strtok(NULL, "\t");
			if (!tmp)
				break;
			memset(&tm, 0, sizeof(tm));
			strcpy(start_time, tmp);
			if ((strptime(start_time, "%a, %d %b %Y %H:%M:%S", &tm)))
				stat_s_set(&(nif->up_timestamp), mktime(&tm));
			else
				stat_s_set(&(nif->up_timestamp), 0);
			nif->hw_status = 1;
			nif->proto_status = is_running(nif->if_name);

			/* master port */
			tmp = strtok(NULL, "\t");
			if (!tmp)
				break;
			strncpy(nif->ppp.master_port, tmp, sizeof(nif->ppp.master_port));
			nif->ppp.master_port[sizeof(nif->ppp.master_port)-1] = '\0';

			strncpy(nif->description, nif->ppp.username, sizeof(nif->description));
			nif->description[sizeof(nif->description)-1] = '\0';
			strncpy(nif->encapsulation, nif->ppp.master_port, sizeof(nif->encapsulation));
			nif->encapsulation[sizeof(nif->encapsulation)-1] = '\0';

			/* pppd pid */
			tmp = strtok(NULL, "\t");
			if (!tmp)
				break;
			nif->ppp.pppd_pid = atoi(tmp);

			/* Tx rate */
			tmp = strtok(NULL, "\t");
			if (!tmp)
				break;
			nif->bandwidth = atoi(tmp) / 8;

			/* Rx rate */
			tmp = strtok(NULL, "\t");
			if (!tmp)
				break;
			nif->rx_bandwidth = atoi(tmp) / 8;

			if (nif->bandwidth == 0) {
				tnif = find_iface_name(nif->ppp.master_port);
				if (tnif) {
					nif->bandwidth = tnif->bandwidth;
					nif->rx_bandwidth = tnif->bandwidth;
				}
			}

			break;
		}
	}
	fclose(fp);
}

int displayDetailPPP(stat_if *nif)
{
	int i;
	char tmp[128] = "\0";

	i = stat_s_show(&(nif->up_timestamp));
	strcpy(tmp, ctime((time_t *)&i));
	tmp[strlen(tmp) - 1] = '\0';

	if (i) {
		wprintw(win, "%s %s %s %s (%s)\n", "     User connected via ", nif->ppp.master_port, "on", tmp, show_time(i, TRUE));
		wprintw(win, "%s %s\n", "     User's IP address: ", getaddr(tmp, nif->if_name, SIOCGIFDSTADDR));
	}
	else
		wprintw(win, "\n\n");
	return 0;
}
