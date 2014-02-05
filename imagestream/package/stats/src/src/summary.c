/* vi:set ts=3: */
/* This code Liscensed under the GPL Version 2. See COPYING for details. */
#include<ncurses.h>
#include"stat_if.h"
#include"read_proc.h"
#include"stats.h"
#include"conf.h"

extern int interval;
extern int current;
extern WINDOW *win;

void summary_update(int signum)
{
	stat_if *cur = NULL;
	char tmp[256] = "\0";
	char tmp2[256] = "\0";
	int i = 0;

	if((LINES - 5) > num_ifaces)
		current = 0;
	wmove(win, 0,0);
	werase(win);
	wprintw(win, "%s\n", header(tmp2, "Interface Summary"));
	wattron(win, A_REVERSE);
	wprintw(win, "%-3s%-10s%-21s%-13s%-12s%4s%-6s%5s%5s",
	       "#  ", "Interface ", "Description        ", "Encaps       ", "Bandwidth",
	       " HW ", " Proto", "  In ", "  Out");
	wattroff(win, A_REVERSE);

	cur = ifaces;
	for(i = 0; i < current; i++)
		cur = cur->next;
//	for(cur; cur; cur = cur->next)
	for(i = 0; i < (LINES - 5); i++)
	{
		if(cur)
		{
			get_stats(cur);
			summary_if(i, cur);
		}
		else
			break;
		cur = cur->next;
	}

	wmove(win, i+2, 0);
	whline(win, '-', 79);
	wmove(win, i+3, 0);

	wprintw(win, "%s", footer(tmp));
	overwrite(win, stdscr);
	setup_signals();
}

int summary_if(int i, stat_if *stat)
{
	char tmp[256];
	char tmp2[256];
	char rx_bw[32], tx_bw[32];
	char description[32];

	sprintf(tmp, "%d", i);
	if(stat->bandwidth == 1)
		sprintf(tmp2, "n/a");
	else {
		if (stat->rx_bandwidth)
			sprintf(tmp2, "%4s/%-4s", bw_format(stat->bandwidth * 8, tx_bw), bw_format(stat->rx_bandwidth * 8, rx_bw));
		else
			sprintf(tmp2, "%s%s", byte_format(stat->bandwidth * 8, 1000000), stat->bandwidth ? "bit" : "\0");
	}

	strncpy(description, stat->description, 21);
	description[20] = ' ';
	description[21] = '\0';
	if(stat->proto_sub)
	{
		wprintw(win, "\n%2s   %-12s %-26s   %-12s",
		       tmp, stat->full_name, stat->description, tmp2);
	}
	else if(stat->vlan_id)
	{
		if (!strlen(stat->description))
		{
			sprintf(stat->description, "VLAN ID %d", stat->vlan_id);
			strcpy(description, stat->description);
		}
		wprintw(win, "\n%2s   %-14s %-24s   %-12s",
		       tmp, stat->full_name, description, tmp2);
	}
	else
	{
		if (stat->proto_status != 128)
			wprintw(win, "\n%2s %-10s%-21s%-13s%-12s",
		       	tmp, stat->full_name, description, stat->encapsulation, tmp2);
		else // If administratively down
			wprintw(win, "\n%2s %-10s%-21s%-13s%-12s",
		       	tmp, stat->full_name, description, "", "");
	}
	up_down(stat->hw_status);
	wprintw(win, " ");
	up_down(stat->proto_status);
	// I don't care about hw_status. If proto_status is up, then it's up!
	if(stat->proto_status == 128) // If administratively down
		wprintw(win, "%11s", ""); // Display nothing for a cleaner page
	else if(stat->proto_status)
	{
		if (stat->rx_bandwidth)
			wprintw(win, "  %3d%% %3d%%",
		       	percent(&(stat->rx.byte), stat->rx_bandwidth, getRealInterval(stat)),
		       	percent(&(stat->tx.byte), stat->bandwidth, getRealInterval(stat)));
		else
			wprintw(win, "  %3d%% %3d%%",
		       	percent(&(stat->rx.byte), stat->bandwidth, getRealInterval(stat)),
		       	percent(&(stat->tx.byte), stat->bandwidth, getRealInterval(stat)));
	}
	else
	{
		int i = stat_s_show(&(stat->up_timestamp));
		if (i)
			wprintw(win, "%11s", show_time(i, TRUE));
		else if (SAND == stat->if_type)
			wprintw(win, "   never up");
	}
}

void summary_next()
{
	if((current + (LINES - 5)) < num_ifaces)
		current += (LINES - 5);
/*
	else
		current = num_ifaces - (LINES - 5);
*/
}

void summary_prev()
{
	if((current - (LINES - 5)) > 0)
		current -= (LINES - 5);
	else
		current = 0;
}
