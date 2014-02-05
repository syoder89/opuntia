/* vi: set ts=3: */
/* This code Liscensed under the GPL Version 2. See COPYING for qoss. */
#include<ncurses.h>
#include<linux/sockios.h>
#include"stat_if.h"
#include"stats.h"
#include"read_proc.h"

extern WINDOW *win;
extern int current;
extern int interval;

void qos_update(int signum)
{
	stat_if *stat = NULL;
	stat_if *cur  = NULL;
	char tmp[128] = "\0";
	char tmp2[128] = "\0";
	char tmp3[128] = "\0";
	int i;
	int real_interval, rx_pps, tx_pps;
	double d_real_interval;

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
	sprintf(tmp2, "%s QoS Summary", stat->full_name);
	wprintw(win, "%s\n", header(tmp, tmp2));
	wprintw(win, "%s\n%s is ",
	       "-------------------------------------------------------------------------------",
	       stat->full_name);
	if(16 == stat->hw_status)
	{
		wattron(win, A_BOLD);
		wprintw(win, "initializing");
		wattroff(win, A_BOLD);
	}
	else if(128 == stat->hw_status)
		wprintw(win, "down");
	else
		up_down(stat->hw_status);

	wprintw(win, ", protocol is ");

	if(128 == stat->proto_status)
		wprintw(win, "administratively down");
	else
		up_down(stat->proto_status);

	wprintw(win, "\n%s%s\n%s%s\n%s%s %s\n%s%s\n\n",
	       "     Description       : ", stat->description,
	       "     Encapsulation     : ", stat->encapsulation,
	       "     IP address        : ", getaddr(tmp, stat->if_name, SIOCGIFADDR),
	                                    getaddr(tmp2, stat->if_name, SIOCGIFNETMASK),
	       "     Broadcast address : ", getaddr(tmp3,stat->if_name, SIOCGIFBRDADDR));

	if(SAND == stat->if_type)
	{
		i = stat_s_show(&(stat->up_timestamp));
//		i = (stat->up_timestamp.current);
		strcpy(tmp, ctime((time_t *)&i));
		tmp[strlen(tmp) - 1] = '\0';

		if(1 == stat->proto_status)
			wprintw(win, "%s %s (%s)", "     Line has been up since", tmp, show_time(i, TRUE));
		else if(128 != stat->proto_status)
		{
			wattron(win, A_BOLD);
			if(i)
			{
				wprintw(win, "%s %s (%s)", "     Line has been down since", tmp, show_time(i, TRUE));
			}
			else
			{
				wprintw(win, "     Line has not been up since the drivers were loaded.");
			}
			wattroff(win, A_BOLD);
		}
		else
			wprintw(win, "     The line is administratively down.");
		
		wprintw(win, "\n%s %s  ", "     Last input :  ", show_time((time_t)(stat->rx.last), FALSE));
		wprintw(win, "  %s %s\n", "Last output:  ", show_time((time_t)(stat->tx.last), FALSE));
	}
	else if (ETH == stat->if_type)
	{
		displayDetailMII(stat);
	}
	else if (BRI == stat->if_type)
	{
		displayDetailBRI(stat);
	}
	else if (PPP == stat->if_type)
	{
		displayDetailPPP(stat);
	}
	else
		wprintw(win, "\n\n");

	real_interval = getRealInterval(stat);
	d_real_interval = real_interval / 1000000.0;
	if (d_real_interval <= 0.0)
		d_real_interval = 1.0;
	rx_pps = RoundInt((double)stat_bp_delta(&(stat->rx.packet))/d_real_interval);
	tx_pps = RoundInt((double)stat_bp_delta(&(stat->tx.packet))/d_real_interval);

	if (stat->rx_bandwidth) {
		char rx_bw[32], tx_bw[32];
		wprintw(win, "\n%s Rx: %-4s Tx: %-4s  %s %3d%% %s %3d%%\n",
	       	"     Bandwidth:", bw_format(stat->rx_bandwidth * 8, rx_bw), bw_format(stat->bandwidth * 8, tx_bw),
	       	"Load in:",  percent(&(stat->rx.byte), stat->rx_bandwidth, real_interval),
	       	"Load out:", percent(&(stat->tx.byte), stat->bandwidth, real_interval));
	}
	else
		wprintw(win, "\n%s %s%s  %s %3d%% %s %3d%%\n",
	       	"     Bandwidth:", byte_format(stat->bandwidth * 8, 1000000), stat->bandwidth ? "bit" : "\0",
	       	"Load in:",  percent(&(stat->rx.byte), stat->bandwidth, real_interval),
	       	"Load out:", percent(&(stat->tx.byte), stat->bandwidth, real_interval));

	strcpy(tmp, bps(&(stat->rx.byte), real_interval, 1));
	wprintw(win, "     %d %s %12s, %12s, %d %s\n",
	       interval, "second average input rate :",
	       bps(&(stat->rx.byte), real_interval, 8),
	       tmp,
			 rx_pps, "packets/s");

	strcpy(tmp, bps(&(stat->tx.byte), real_interval, 1));
	wprintw(win, "     %d %s %12s, %12s, %d %s\n",
	       interval, "second average output rate:",
	       bps(&(stat->tx.byte), real_interval, 8),
	       tmp,
			 tx_pps, "packets/s");
	       

	strcpy(tmp, comma_format(stat_s_show((stat_s *)&(stat->rx.byte))));
	wprintw(win, "%s %16s %16s bytes\n",
	       "          Rx Packets", comma_format(stat_s_show((stat_s *)&(stat->rx.packet))), tmp);

	strcpy(tmp, comma_format(stat_s_show((stat_s *)&(stat->tx.byte))));
	wprintw(win, "%s %16s %16s bytes\n",
	       "          Tx Packets", comma_format(stat_s_show((stat_s *)&(stat->tx.packet))), tmp);

	wprintw(win, "%s %d  (%d CRC  %d frame  %d fifo  %d dropped)\n",
	       "          Rx Errors:", stat_s_show((stat_s *)&(stat->rx.error)),
	       stat_s_show((stat_s *)&(stat->rx_crc)),
	       stat_s_show((stat_s *)&(stat->rx_frame)),
	       stat_s_show((stat_s *)&(stat->rx.fifo)),
	       stat_s_show((stat_s *)&(stat->rx.drop)));

	wprintw(win, "%s %d  (%d collisions  %d fifo  %d dropped)\n",
	       "          Tx Errors:", stat_s_show((stat_s *)&(stat->tx.error)),
	       stat_s_show((stat_s *)&(stat->tx_collisions)),
	       stat_s_show((stat_s *)&(stat->tx.fifo)),
	       stat_s_show(&(stat->tx.drop)));
	if(SAND == stat->if_type)
	{
		wprintw(win, "%s %d\n",
		       "          Carrier transitions", stat_s_show((stat_s *)&(stat->dcd_transitions)));

		wprintw(win, "%s", "          DCD = ");
		up_down(stat->dcd);
		wprintw(win, "%s", " DSR = ");
		up_down(stat->dsr);
		wprintw(win, "%s", " DTR = ");
		up_down(stat->dtr);
		wprintw(win, "%s", " RTS = ");
		up_down(stat->rts);
		wprintw(win, "%s", " CTS = ");
		up_down(stat->cts);
		wprintw(win, "\n");
	}
	else if (ETH == stat->if_type)
		wprintw(win, "\n");
	else
		wprintw(win, "\n\n");
	
	wprintw(win, "%s\n",
	       "-------------------------------------------------------------------------------");

	wprintw(win, "%s", footer(tmp));
	overwrite(win, stdscr);
	setup_signals();
}

void qos_next()
{
	if((current + 1) < num_ifaces)
		current++;
}

void qos_prev()
{
	if((current - 1) > -1)
		current--;
}
