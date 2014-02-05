/* vi:set ts=3: */
/* This code Liscensed under the GPL Version 2. See COPYING for details. */
#include<stdio.h>
#include<time.h>
#include<ncurses.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<linux/if.h>
#include<arpa/inet.h>
#include<sys/time.h>
#include"stats.h"
#include"stat_if.h"
#include"read_proc.h"

extern char hostname[31];
extern WINDOW *win;
extern mode_struct modes[4];
extern mode_struct *mode; 

int getRealInterval(stat_if *nif)
{
	int micros = nif->cur_timestamp.tv_usec - nif->last_timestamp.tv_usec;
	int secs = nif->cur_timestamp.tv_sec - nif->last_timestamp.tv_sec;
//printf("getRealInterval: %d\n", (secs * 1000000) + micros);
//fflush(stdout);
	return ((secs * 1000000) + micros);
}

int RoundInt(double val)
{
	double tmp;

	tmp = val * 10;
   if (((int)tmp % 10) >= 5) /* Round up */
		tmp += 10;
	tmp = (int)tmp / 10;
	return ((int)(tmp));
}

char *header(char *head, char *title)
{
	time_t now = time(NULL);
	struct tm *tm_now = localtime(&now);

	if(tm_now)
		sprintf(head, "%02d:%02d:%02d%14s%-25s%32s",
		        tm_now->tm_hour, tm_now->tm_min, tm_now->tm_sec,
		        " ", title, hostname);
	else
		sprintf(head, "%02d:%02d:%02d%14s%-25s%32s", 00, 00, 00, " ", title, hostname);
	return(head);
}

// Interval is now in microseconds!
char *byte_format(unsigned long bandwidth, int l_interval)
{
	static char str[16];
	double d_interval = (double)l_interval / 1000000.0; // Convert microseconds to seconds
	if (d_interval <= 0.0)
		d_interval = 1.0;
	if(!bandwidth)
		strcpy(str, "Not set");
	else
	{
		if(bandwidth >= 1000000000)
			snprintf(str, 15, "%6.2lf G", ((double)bandwidth / 1000000000)/d_interval);
		else if (bandwidth >= 1000000)
			snprintf(str, 15, "%6.2lf M", ((double)bandwidth / 1000000)/d_interval);
		else if (bandwidth >= 1000)
			snprintf(str, 15, "%6.2lf K", ((double)bandwidth / 1000)/d_interval);
		else
			snprintf(str, 15, "%6.2lf ",   ((double)bandwidth)/d_interval);
	}
	return(str);
}

char *bw_format(unsigned long bandwidth, char *str)
{
	if(!bandwidth)
		strcpy(str, "Not set");
	else
	{
		if(bandwidth >= 1000000000)
			snprintf(str, 15, "%dG", bandwidth / 1000000000);
		else if (bandwidth >= 1000000)
			snprintf(str, 15, "%dM", bandwidth / 1000000);
		else if (bandwidth >= 1000)
			snprintf(str, 15, "%dK", bandwidth / 1000);
		else
			snprintf(str, 15, "%d",   bandwidth);
	}
	return(str);
}

void up_down(short status)
{
	if(128 == status)
	{
		wprintw(win, "shut");
	}
	else if(16 == status)
	{
		wattron(win, A_BOLD);
		wprintw(win, "init");
		wattroff(win, A_BOLD);
	}
	else if(2 == status)
	{
		wprintw(win, " n/a");
	}
	else if(130 == status) // Local loopback
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(ALARM));
		wattron(win, A_BOLD);
		if(&modes[SUMMARY] == mode) // Summary mode has limited space
			wprintw(win, "loop");
		else
			wprintw(win, "down (local loopback)");
		wattroff(win, A_BOLD);
		if(has_colors())
			wattroff(win, COLOR_PAIR(ALARM));
	}
	else if(131 == status) // Remote loopback
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(ALARM));
		wattron(win, A_BOLD);
		if(&modes[SUMMARY] == mode) // Summary mode has limited space
			wprintw(win, "loop");
		else
			wprintw(win, "down (remote loopback)");
		wattroff(win, A_BOLD);
		if(has_colors())
			wattroff(win, COLOR_PAIR(ALARM));
	}
	else if(132 == status) // Local and Remote loopback
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(ALARM));
		wattron(win, A_BOLD);
		if(&modes[SUMMARY] == mode) // Summary mode has limited space
			wprintw(win, "loop");
		else
			wprintw(win, "down (local and remote loopback)");
		wattroff(win, A_BOLD);
		if(has_colors())
			wattroff(win, COLOR_PAIR(ALARM));
	}
	/* BRI used bit0 for 1st B chan, bit 2 for 2nd B chan */
	else if(status & 0x5)
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(UP));
		wprintw(win, " up ");
		if(has_colors())
			wattroff(win, COLOR_PAIR(UP));
	}
	else
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(ALARM));
		wattron(win, A_BOLD);

		wprintw(win, "down");

		wattroff(win, A_BOLD);
		if(has_colors())
			wattroff(win, COLOR_PAIR(ALARM));
	}
}

// Interval is now in microseconds!
int percent(stat_bp *stat, unsigned long bandwidth, int l_interval)
{
	int tmp = 0;
	double d_interval = (double)l_interval / 1000000.0; // Convert microseconds to seconds

	if(bandwidth)
	{
		tmp = ((((double)stat_bp_delta(stat) * 100.0)/(double)bandwidth)/d_interval);
		if(tmp > 100)
			tmp = 100;
		if(tmp < 0)
			tmp = 0;
	}
	return(tmp);
}

char *getaddr(char *working, char *if_name, int request)
{
	int inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strcpy(ifr.ifr_name, if_name);

	if(inet_sock != -1)
	{
		sin = (struct sockaddr_in *)(&ifr.ifr_addr);

		if(ioctl(inet_sock, request, &ifr) >=0)
			addrof(working, sin);
		else
			sprintf(working, "none");
	}
	else
		sprintf(working, "none");

	close(inet_sock);
	
	return(working);
}

char *comma_format(unsigned long num)
{
	unsigned int value = num;
	static char str[256];
	char *ptr = str;
	unsigned short one = 0, two = 0, three = 0, four = 0;

	one = value % 1000;
	value /= 1000;

	two = value % 1000;
	value /= 1000;

	three = value % 1000;
	value /= 1000;

	four = value % 1000;

	if (four)
	{
		ptr += snprintf(ptr, 256, "%d,", four);
		ptr += snprintf(ptr, 256, "%03d,", three);
		ptr += snprintf(ptr, 256, "%03d,", two);
		ptr += snprintf(ptr, 256, "%03d", one);
	}
	else if (three) {
		ptr += snprintf(ptr, 256, "%d,", three);
		ptr += snprintf(ptr, 256, "%03d,", two);
		ptr += snprintf(ptr, 256, "%03d", one);
	}
	else if (two) {
		ptr += snprintf(ptr, 256, "%d,", two);
		ptr += snprintf(ptr, 256, "%03d", one);
	}
	else
		ptr += snprintf(ptr, 256, "%d", one);

	return str;
}

char *bps(stat_bp *stat, int l_interval, int scale)
{
	static char str[80] ="\0";

	snprintf(str, 79, "%s%s/s",
	         stat_bp_delta(stat) ? byte_format(stat_bp_delta(stat) * scale, l_interval) : "0 ",
	         (8 == scale) ? "b" : "B");
	return(str);
}

char *show_time(time_t timestamp, int type)
{
	static char retval[64] = "\0";
	char tmp[16] = "\0";
	time_t now = time(NULL), diff;
	int seconds = 0, minutes = 0, hours = 0, days = 0, weeks = 0, years = 0;

	retval[0] = '\0';

	if(!timestamp)
	{
		if(type)
			sprintf(retval, "never up");
		else
			sprintf(retval, "never");
		return(retval);
	}

	diff = now - timestamp;

	seconds = diff % 60;
	diff /= 60;
	minutes = diff % 60;
	diff /= 60;
	hours = diff % 24;
	diff /= 24;
	days = diff % 7;
	diff /= 7;
	weeks = diff % 52;
	diff /= 52;
	years = diff;

	if(years || weeks || days)
	{
		if(years)
			sprintf(retval, "%dy", years);
		if(weeks)
		{
			sprintf(tmp, "%dw", weeks);
			strcat(retval, tmp);
		}
		if(days)
		{
			sprintf(tmp, "%dd", days);
			strcat(retval, tmp);
		}
		sprintf(tmp, "%dh", hours);
		strcat(retval, tmp);
	}
	else
		snprintf(retval, 31, "%02d:%02d:%02d", hours, minutes, seconds);
	return(retval);
}

void zero(int current)
{
	stat_if *cur = ifaces;
	stat_if *stat = NULL;
	int i = 0;

	for(i; i < current; i++)
		cur = cur->next;
	stat = cur;

//	stat_s_zero(&(stat->up_timestamp));
	stat_s_zero(&(stat->dcd_transitions));

/*
 * Zero recieve information
 */
	stat_s_zero((stat_s *)&(stat->rx.byte));
	stat_s_zero((stat_s *)&(stat->rx.packet));
	stat_s_zero(&(stat->rx.error));
	stat_s_zero(&(stat->rx.drop));
	stat_s_zero(&(stat->rx.fifo));
	stat_s_zero(&(stat->rx.compressed));
	stat_s_zero(&(stat->rx_frame));
	stat_s_zero(&(stat->rx_crc));
	stat_s_zero(&(stat->rx_multicast));

/*
 * Zero transmit information
 */
	stat_s_zero((stat_s *)&(stat->tx.byte));
	stat_s_zero((stat_s *)&(stat->tx.packet));
	stat_s_zero(&(stat->tx.error));
	stat_s_zero(&(stat->tx.drop));
	stat_s_zero(&(stat->tx.fifo));
	stat_s_zero(&(stat->tx.compressed));
	stat_s_zero(&(stat->tx_carrier));
	stat_s_zero(&(stat->tx_collisions));
}
