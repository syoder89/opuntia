/* vi: set ts=3: */
/* This code Liscensed under the GPL Version 2. See COPYING for details. */
#include<ncurses.h>
#include<linux/sockios.h>
#include"stat_if.h"
#include"stats.h"
#include"read_proc.h"
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#ifndef IFNAMSIZ
#define IFNAMSIZ        16
#endif

#include "sand_defs.h"
#include "sand_ioctl.h"

#define BIT0	0x01
#define BIT1	0x02
#define BIT2	0x04
#define BIT3	0x08
#define BIT4	0x10
#define BIT5	0x20
#define BIT6	0x40
#define BIT7	0x80

/* Rx & Tx line status definitions */
#define RAI BIT0
#define AIS BIT1
#define PDE BIT2
#define LOS BIT3
#define OOF BIT4
#define B8ZS BIT5

#define CSU_OK(a) (a == 0 || a == BIT7)
#define LOOPBACK_OK(a) (a == 0 || a == 4 || a == 0x40)
#define REMOTE_LOOPBACK_OK(a) (!(a & BIT1))
#define RX_OK(a) (a == 0 || a == B8ZS)
#define TX_OK(a) (a == 0)
#define FE_OK(a) (a == 0)
#define FRAMING_MISMATCH(a) (a & OOF && ! a & LOS)
#define CABLE_UNPLUGGED(rx,tx) (rx&PDE && rx&LOS && rx&OOF && tx&RAI) 


void display_general_info(SAND_CSU_PERFORMANCE_STATS *);
void display_line_status_verbose(SAND_CSU_PERFORMANCE_STATS *);
void display_interval(SAND_CSU_PERFORMANCE_STATS *, int);

extern WINDOW *win;
extern int current;
extern int interval;

void detail_csu_update(int signum)
{
	stat_if *stat = NULL;
	stat_if *cur  = NULL;
	char tmp[128] = "\0";
	char tmp2[128] = "\0";
	char tmp3[128] = "\0";
	int i;
	SAND_CSU_PERFORMANCE_STATS csu_stats;
	int devfd, dev_index = 0, dev_sub_index = 0, retval;
	char dev_name[64];
	
	memset(&csu_stats, 0, sizeof(SAND_CSU_PERFORMANCE_STATS));

	cur = ifaces;

	for(i = 0; i < (current); i++)
	{
		if(cur->next)
			cur = cur->next;
		else
			break;
	}
	stat = cur;

	wmove(win, 0, 0);
	werase(win);
	sprintf(tmp2, "%s CSU Statistics", stat->full_name);
	wprintw(win, "%s\n", header(tmp, tmp2));
	wprintw(win, "-------------------------------------------------------------------------------\n");

	if (stat->if_type != SAND)
	{
		wprintw(win, "\nCSU statistics not available...\n\n");
		goto out;
	}

	//	csu_stats.framer = 0; /* Our csu's stats */
	//	csu_stats.framer = 1; /* Far end csu's stats */
	//	csu_stats.request_type = 0; /* Give me everything */
	//	csu_stats.request_type = 1; /* Give me info on a specific interval */
	//	csu_stats.request_type = 2; /* Give me generic csu info only */
	//	csu_stats.interval = 0; /* Give me info on the current 15 minute interval only */
	//	csu_stats.interval = 1; /* Give me info on the previous 15 minute interval only */
	//	csu_stats.interval = 4; /* Give me info on the 15 minute interval an hour ago only */


	snprintf(dev_name, 64, "/dev/sand/%s", stat->full_name);
	devfd = open(dev_name, O_RDONLY);

	if(devfd < 0){
		sscanf(stat->full_name, "Serial%d", &dev_index);
		snprintf(dev_name, 64, "/dev/wanic%d", dev_index);

		devfd = open(dev_name, O_RDONLY);
	}

	if (devfd != -1)
	{
		csu_stats.request_type = SAND_CSU_REQUEST_TYPE_GENERAL; /* Give me general csu info only */
		retval = ioctl(devfd, SIOCGETCSUSTATS, &csu_stats);
		if (retval)
		{
			wprintw(win, "\nCSU statistics not available.\n\n");
		}
		else
		{
			csu_stats.request_type = SAND_CSU_REQUEST_TYPE_INTERVAL; /* Give me current interval csu info only */
			retval = ioctl(devfd, SIOCGETCSUSTATS, &csu_stats);
			if (retval)
			{
				wprintw(win, "\nCSU statistics not available...\n\n");
			}
			else
			{
				display_general_info(&csu_stats);
#if 0
				/* Loop through all valid intervals */
				/* I'm going to use interval 0 as the current interval, so I loop 
					to valid_intervals since valid_intervals tells how many indicies
					are valid in the previous[] array. I want to display the current
					interval (0) and then all previous intervals (previous[i-1]) */
				for (i=0;i<=csu_stats.valid_intervals;i++)
					display_interval(&csu_stats, i);
#endif
				/* That takes way too long! I'm just going to get current stats! */
				display_interval(&csu_stats, 0);
				display_line_status_verbose(&csu_stats);
			}
		}
		close(devfd);
	}

out:
	wprintw(win, "%s\n",
	       "-------------------------------------------------------------------------------");

	wprintw(win, "%s", footer(tmp));
	overwrite(win, stdscr);
	setup_signals();
}

void display_general_info(SAND_CSU_PERFORMANCE_STATS *csu_stats)
{
	SAND_CSU_INFO *csu_info = &csu_stats->csu_info;
	unsigned char temp, printed = 0;

	wprintw(win, "Firmware version: %d.%02d\n", csu_info->firmware_major, csu_info->firmware_minor);
//	wprintw(win, "The CSU has statistics for %d 15 minute intervals from the past 24 hours\n", csu_stats->valid_intervals+1);

	wprintw(win, "CSU self test: ");
	if (csu_info->csu_failure == 0)
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(UP));
		wprintw(win, "no failures\n");
		if(has_colors())
			wattroff(win, COLOR_PAIR(UP));
	}
	else
	{
		wattron(win, A_BOLD);
		temp = csu_info->csu_failure;
		if (temp & BIT0)
		{
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "ROM checksum failure");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			printed = 1;
		}
		if (temp & BIT1)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "EEPROM checksum failure");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			printed = 1;
		}
		if (temp & BIT4)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Framer type is incorrect");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			printed = 1;
		}
		if (temp & BIT5)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Framer hardware failure");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			printed = 1;
		}
		if (temp & BIT6)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Internal loopback failure");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			printed = 1;
		}
		if (temp & BIT7)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "CSU is initializing");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		}
		wattroff(win, A_BOLD);
		wprintw(win, "\n");
	}
	
	printed = 0;
	wprintw(win, "Rx status: ");
	if (csu_info->rx_line_status == 0 || csu_info->rx_line_status == B8ZS)
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(UP));
		wprintw(win, "no alarms\n");
		if(has_colors())
			wattroff(win, COLOR_PAIR(UP));
	}
	else
	{
		wattron(win, A_BOLD);
		temp = csu_info->rx_line_status;
		if (temp & RAI)
		{
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "RAI");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		  	wprintw(win, " - remote alarm indication (yellow alarm)");
			printed = 1;
		}
		if (temp & AIS)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "AIS");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		  	wprintw(win, " - alarm indication signal (blue alarm)");
			printed = 1;
		}
		if (temp & PDE)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "PDE");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wprintw(win, " - pulse density error");
			printed = 1;
		}
		if (temp & LOS)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "LOS");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		  	wprintw(win, " - loss of signal");
			printed = 1;
		}
		if (temp & OOF)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "OOF");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wprintw(win, " - out of frame");
			printed = 1;
		}
#if 0 /* Skip this as it's not really an error! It means that
			the framer is autodetecting B8ZS on the line! */
		if (temp & B8ZS)
		{
			if (printed)
				wprintw(win, ", ");
			wprintw(win, "B8ZS - receive B8ZS detected");
			printed = 1;
		}
#endif
		wattroff(win, A_BOLD);
		wprintw(win, "\n");
	}

	printed = 0;
	wprintw(win, "Tx status: ");
	if (csu_info->tx_line_status == 0)
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(UP));
		wprintw(win, "no alarms\n");
		if(has_colors())
			wattroff(win, COLOR_PAIR(UP));
	}
	else
	{
		wattron(win, A_BOLD);
		temp = csu_info->tx_line_status;
		if (temp & RAI)
		{
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "RAI");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		  	wprintw(win, " - remote alarm indication (yellow alarm)");
			printed = 1;
		}
		if (temp & AIS)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "AIS");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		  	wprintw(win, "	- alarm indication signal (blue alarm)");
			printed = 1;
		}
		if (temp & PDE)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "PDE");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		  	wprintw(win, "	- pulse density error");
			printed = 1;
		}
		if (temp & LOS)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "LOS");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		  	wprintw(win, "	- loss of signal");
			printed = 1;
		}
		wattroff(win, A_BOLD);
		wprintw(win, "\n");
	}

	printed = 0;
	wprintw(win, "Far end CSU status: ");
	if (csu_info->fe_status == 0)
	{
		// The CSU usually reports normal far end even when it's unplugged! That's confusing!
		if (CABLE_UNPLUGGED(csu_info->rx_line_status, csu_info->tx_line_status))
		{
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "unknown\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		}
		else
		{
			if(has_colors())
				wattron(win, COLOR_PAIR(UP));
			wprintw(win, "normal\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(UP));
		}
	}
	else
	{
		wattron(win, A_BOLD);
		temp = csu_info->fe_status;
		if (temp & BIT0)
		{
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "PRM - performance report monitoring failure");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			printed = 1;
		}
		if (temp & BIT1)
		{
			if (printed)
				wprintw(win, ", ");
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "payload loopback");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			printed = 1;
		}
		wattroff(win, A_BOLD);
		wprintw(win, "\n");
	}
	wprintw(win, "Loopback: ");
	switch(csu_info->loopback_configuration)
	{
		case 0x00:
		/* I've been told that 0x04 is a bug and shouldn't appear, but that it
			also means that the csu is not in loopback. */
		case 0x04:
		case 0x40: // Apparently 0x40 is another bug!
			wprintw(win, "csu will respond to loop up command, not currently looped up\n");
		break;
		case 0x01:
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "csu is currently in payload loopback state\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		break;
		case 0x02:
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "csu is currently in line loopback state\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		break;
		case 0x03:
			wprintw(win, "csu will not respond to loop up command\n");
		break;
		default:
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "unknown loopback configuration (0x%x)!\n", csu_info->loopback_configuration);
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		break;
	}

}

void display_interval(SAND_CSU_PERFORMANCE_STATS *csu_stats, int interval)
{
	SAND_CSU_INTERVAL_STATS *interval_ptr;
	unsigned char temp, printed = 0;

//	wprintw(win, "%d seconds have elapsed in the current 15 minute interval\n", csu_stats->time_elapsed);
	wprintw(win, "\n");
	if (interval == 0)
	{
		wprintw(win, "Statistics for current interval (%d seconds elapsed):\n", csu_stats->time_elapsed);
 		interval_ptr = &csu_stats->current_interval;
	}
	else
	{
		wprintw(win, "Statistics for interval %d (%d seconds elapsed):\n", interval, csu_stats->time_elapsed);
 		interval_ptr = &csu_stats->previous[interval-1];
	}
	wmove(win, 12, 0);
	wprintw(win, "                 Errored seconds: %d\n", interval_ptr->es);
	wprintw(win, "          Bursty errored seconds: %d\n", interval_ptr->bes);
	wprintw(win, "        Severely errored seconds: %d\n", interval_ptr->ses);
	wprintw(win, "Severely errored framing seconds: %d\n", interval_ptr->sefs);
	wprintw(win, "             Unavailable seconds: %d\n", interval_ptr->uas);
	wmove(win, 12, 40);
	wprintw(win, "Controlled slip seconds: %d\n", interval_ptr->css);
	wmove(win, 13, 40);
	wprintw(win, "       Degraded minutes: %d\n", interval_ptr->dms);
	wmove(win, 14, 40);
	wprintw(win, "   Path code violations: %d\n", interval_ptr->pcv);
	wmove(win, 15, 40);
	wprintw(win, "   Line errored seconds: %d\n", interval_ptr->les);
	wmove(win, 16, 40);
	wprintw(win, "   Line code violations: %d\n", interval_ptr->lcv);
}

void display_line_status_verbose(SAND_CSU_PERFORMANCE_STATS *csu_stats)
{
	SAND_CSU_INFO *csu_info = &csu_stats->csu_info;
	int printed;

	wprintw(win, "\n");
	wprintw(win, "Line status information:\n");

	if (CSU_OK(csu_info->csu_failure) && RX_OK(csu_info->rx_line_status) &&
			TX_OK(csu_info->tx_line_status) &&
			LOOPBACK_OK(csu_info->loopback_configuration) &&
			REMOTE_LOOPBACK_OK(csu_info->fe_status))
	{
		if(has_colors())
			wattron(win, COLOR_PAIR(UP));
		wprintw(win, "  The line appears to be up.\n");
		if(has_colors())
			wattroff(win, COLOR_PAIR(UP));
	}
	else
	{
		wattron(win, A_BOLD);
		if(has_colors())
			wattron(win, COLOR_PAIR(ALARM));
		if (!CSU_OK(csu_info->csu_failure))
		{
			if (csu_info->csu_failure == BIT6) /* Loopback failure */
				wprintw(win, "  Your CSU has a hardware failure, however loopback failure is probably not\n    the cause of the line failure.\n");
			else
				wprintw(win, "  Your CSU has a hardware failure. This is most likely the cause of the line\n    failure.\n");
		}
		if (!LOOPBACK_OK(csu_info->loopback_configuration) && !REMOTE_LOOPBACK_OK(csu_info->fe_status))
		{
			wprintw(win, "  Both CSUs are currently looped up. No data can be received by either router\n    while the CSUs are looped up.\n");
		}
		else if (!LOOPBACK_OK(csu_info->loopback_configuration))
		{
			wprintw(win, "  Your CSU is currently looped up. No data can be received by your router while\n    the CSU is looped up.\n");
		}
		else if (!REMOTE_LOOPBACK_OK(csu_info->fe_status))
		{
			wprintw(win, "  The remote CSU is currently looped up. No data can be received by the remote\n    router while the CSU is looped up.\n");
		}
		printed = 0;
		// Rx and Tx no good. Probably unplugged or misconfigured.
		if (!RX_OK(csu_info->rx_line_status) && !TX_OK(csu_info->tx_line_status))
		{
			if (FRAMING_MISMATCH(csu_info->rx_line_status))
				wprintw(win, "  Your CSU appears to have mismatched framing. Verify the proper framing setting\n    with your line provider.");
			else if (CABLE_UNPLUGGED(csu_info->rx_line_status, csu_info->tx_line_status))
				wprintw(win, "  Your CSU appears to be unplugged. Check your cabling.");
			else
				wprintw(win, "  Your CSU shows Rx and Tx problems. Check your cabling and configuration.");
		}
		else
		{
			if (!RX_OK(csu_info->rx_line_status))
			{
				if (printed)
					wprintw(win, ", ");
				else
					wprintw(win, "  ");
				printed = 1;
				wprintw(win, "Rx problem");
			}
			if (!TX_OK(csu_info->tx_line_status))
			{
				if (printed)
					wprintw(win, ", ");
				else
					wprintw(win, "  ");
				printed = 1;
				wprintw(win, "Tx problem");
			}
			if (!FE_OK(csu_info->fe_status))
			{
				if (printed)
					wprintw(win, ", ");
				else
					wprintw(win, "  ");
				printed = 1;
				wprintw(win, "Far end problem");
			}
		}
		wattroff(win, A_BOLD);
		if(has_colors())
			wattroff(win, COLOR_PAIR(ALARM));
		wprintw(win, "\n");
	}
}

void detail_csu_next()
{
	if((current + 1) < num_ifaces)
		current++;
}

void detail_csu_prev()
{
	if((current - 1) > -1)
		current--;
}
