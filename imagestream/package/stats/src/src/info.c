/* vi: set ts=3: */
/* This code Liscensed under the GPL Version 2. See COPYING for details. */
#include<ncurses.h>
#include<linux/sockios.h>
#include"stat_if.h"
#include"stats.h"
#include"read_proc.h"
#include <fcntl.h>
#include <sys/ioctl.h>
#include "sand_ioctl.h"

extern WINDOW *win;
extern int current;
extern int interval;

void info_update(int signum)
{
	stat_if *stat = NULL;
	stat_if *cur  = NULL;
	char tmp[128] = "\0";
	char tmp2[128] = "\0";
	char tmp3[128] = "\0";
	int i, dev_index, devfd, retval;
	char buffer[4096], dev_name[64];

	cur = ifaces;

	for(i = 0; i < (current); i++)
	{
		if(cur->next)
			cur = cur->next;
		else
			break;
	}
	stat = cur;
	get_stats(stat);

	wmove(win, 0, 0);
	werase(win);
	sprintf(tmp2, "%s Information", stat->full_name);
	wprintw(win, "%s\n", header(tmp, tmp2));

	if(SAND == stat->if_type)
	{
		sscanf(stat->full_name, "Serial%d", &dev_index);
		snprintf(dev_name, 64, "/dev/wanic%d", dev_index);

		devfd = open(dev_name, O_RDONLY);
		if (devfd != -1)
		{
			memset(buffer, 0, 4096);
			retval = ioctl(devfd, SIOCGETCARDINFO, &buffer);
			if (retval)
			{
				wprintw(win, "\nDetailed setup information not available.\n\n");
				goto out;
			}
			else
			{
				wprintw(win, "\n  Card information:\n");
				wprintw(win, "%s\n", buffer);
			}
			memset(buffer, 0, 4096);
			retval = ioctl(devfd, SIOCGETPORTINFO, &buffer);
			wprintw(win, "  Port information:\n");
			if (retval)
			{
				wprintw(win, "       Detailed port setup information not available.\n\n");
				goto out;
			}
			else
			{
				wprintw(win, "%s\n", buffer);
			}
out:
			close(devfd);
		}
	}
	else if (ETH == stat->if_type)
	{
		wprintw(win, "\n\n");
	}
	else
		wprintw(win, "\n\n");

	wprintw(win, "%s\n",
	       "-------------------------------------------------------------------------------");

	wprintw(win, "%s", footer(tmp));
	overwrite(win, stdscr);
	setup_signals();
}

void info_next()
{
	if((current + 1) < num_ifaces)
		current++;
}

void info_prev()
{
	if((current - 1) > -1)
		current--;
}
