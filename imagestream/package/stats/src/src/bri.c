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

void getBRI(stat_if *nif)
{
	int retval, devid;
	int old_hw = nif->hw_status;
	char proto_status_file[256], hw_status_file[256];
	int proto_status = 0, hw_status = 0, bandwidth = 0;
	/* 2nd B channel */
	int hw_status2 = 0, proto_status2 = 0;
	char first_b_devname[256], second_b_devname[256];
	char *encaps = "";
	FILE *fp;
	char Command[256];

	/* Someone's hacking! */
	if (strlen(nif->if_name) > 200)
		goto out;

	devid = 0;
	strcpy(first_b_devname, nif->if_name);
	sscanf(first_b_devname, "ippp%d", &devid);
	devid++;
	sprintf(second_b_devname, "ippp%d", devid);

	sprintf(hw_status_file, "/tmp/%s.connected", first_b_devname);
	/* Issue an isdnctrl command to check if the port is dialing. */
	sprintf(Command, "isdnctrl status %s > %s 2>/dev/null", first_b_devname, hw_status_file);
	system(Command);

	if (!(fp = fopen(hw_status_file, "r")))
		goto out;

	if (fgets(Command, 255, fp))
	{
		if (strstr(Command, "connected to"))
			hw_status = 1;
	}
	fclose(fp);
	unlink(hw_status_file);

	/* Check to see if the 2nd B channel is dialing */
	sprintf(hw_status_file, "/tmp/%s.connected", second_b_devname);

	sprintf(Command, "isdnctrl status %s > %s 2>/dev/null", second_b_devname, hw_status_file);
	system(Command);

	if (!(fp = fopen(hw_status_file, "r")))
		goto out;

	if (fgets(Command, 255, fp))
	{
		if (strstr(Command, "connected to"))
			hw_status2 = 1;
	}
	fclose(fp);
	unlink(hw_status_file);

	/* Check the protocol status */

	sprintf(proto_status_file, "/tmp/%s.status", first_b_devname);
	if ((fp = fopen(proto_status_file, "r")))
	{
		if (fgets(Command, 255, fp))
		{
			if (!strncasecmp(Command, "up", 2))
				proto_status = 1;
			else if (!strncasecmp(Command, "auth fail", 9))
				proto_status = 32;
		}
		fclose(fp);
	}

	/* Check the protocol status of the 2nd B channel */

	sprintf(proto_status_file, "/tmp/%s.status", second_b_devname);
	if ((fp = fopen(proto_status_file, "r")))
	{
		if (fgets(Command, 255, fp))
		{
			if (!strncasecmp(Command, "up", 2))
				proto_status2 = 1;
			else if (!strncasecmp(Command, "auth fail", 9))
				proto_status2 = 16; /* Will turn into 64 by <<2 below */
		}
		fclose(fp);
	}

out:
	if (proto_status == 1)
		bandwidth = 8000;
	if (proto_status2 == 1)
		bandwidth += 8000;
	nif->hw_status = hw_status | (hw_status2 << 2);
	nif->proto_status = proto_status | (proto_status2 << 2);
	nif->bandwidth = bandwidth;
}

int displayDetailBRI(stat_if *nif)
{
	int i, printed;

	wprintw(win, "     1st B channel status: ");

	if (nif->hw_status & 0x1)
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(UP));
		wprintw(win, "Active");
		if(has_colors())
			wattroff(win, COLOR_PAIR(UP));
		if (nif->proto_status & 32)
		{
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, ", protocol authentication failure!");
			wattroff(win, A_BOLD);
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		}
		else if (nif->proto_status & 1)
		{
			if(has_colors())
				wattron(win, COLOR_PAIR(UP));
			wprintw(win, ", protocol up");
			if(has_colors())
				wattroff(win, COLOR_PAIR(UP));
		}
		else
		{
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, ", protocol down");
			wattroff(win, A_BOLD);
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		}
		wprintw(win, "\n");
	}
	else
	{
		wattron(win, A_BOLD);
		if(has_colors())
			wattron(win, COLOR_PAIR(ALARM));
		wprintw(win, "Idle\n");
		wattroff(win, A_BOLD);
		if(has_colors())
			wattroff(win, COLOR_PAIR(ALARM));
	}

	wprintw(win, "     2st B channel status: ");

	if (nif->hw_status & 0x4)
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(UP));
		wprintw(win, "Active");
		if(has_colors())
			wattroff(win, COLOR_PAIR(UP));
		if (nif->proto_status & 64)
		{
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, ", protocol authentication failure!");
			wattroff(win, A_BOLD);
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		}
		else if (nif->proto_status & 4)
		{
			if(has_colors())
				wattron(win, COLOR_PAIR(UP));
			wprintw(win, ", protocol up");
			if(has_colors())
				wattroff(win, COLOR_PAIR(UP));
		}
		else
		{
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, ", protocol down");
			wattroff(win, A_BOLD);
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		}
		wprintw(win, "\n");
	}
	else
	{
		wattron(win, A_BOLD);
		if(has_colors())
			wattron(win, COLOR_PAIR(ALARM));
		wprintw(win, "Idle\n");
		wattroff(win, A_BOLD);
		if(has_colors())
			wattroff(win, COLOR_PAIR(ALARM));
	}
	return 0;
}
