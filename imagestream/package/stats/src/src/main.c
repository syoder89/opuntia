/* vi:set ts=3: */
/* This code Liscensed under the GPL Version 2. See COPYING for details. */
#include<ncurses.h>
#include<signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<sys/time.h>
#include<unistd.h>
#include"read_proc.h"
#include"stats.h"
#include"conf.h"
#include <panel.h>
#include <menurt.h>
#include <winops.h>
#include <msgboxes.h>
#include <getopt.h>

mode_struct modes[4];
mode_struct *mode = NULL;
int batch_mode = 0;
int num_iterations = 1;
char *interface = "";
int current = 0;
int interval = 3;
int sort_method = 0;
int sort_ascending = 1;
char hostname[256];
WINDOW *win = NULL;
void display_help();
int num_ifaces = 0;
stat_if *ifaces = NULL, *ifaces_tail = NULL, *detail_nif = NULL;

static struct option long_options[] =
{
	{"batch_mode", no_argument, &batch_mode, 0 },
	{"num_iterations", required_argument,	0, 'n'},
	{"interface", required_argument,	0, 'i'},
	{0,0,0,0}
};

void usage() {
	printf("Usage: stats [ -b | --batch_mode ] [ -n | --num_iterations <num> ] [ -i | --interface <iface> ]\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int key = 0, i = 0;
	int option_index = 0, c;
	struct stat s;

	while(1) {
		c = getopt_long (argc, argv, "bn:i:", long_options, &option_index);
		if (c == -1)
			break;
		switch(c) {
			case 'b':
				batch_mode = 1;
			break;
			case 'n':
				num_iterations = atoi(optarg);
			break;
			case 'i':
				interface = optarg;
			break;
			default:
				usage();
			break;
		}
	}

	if (stat("/usr/share/terminfo", &s) != 0)
	{
		printf("Error: /usr/share/terminfo does not exist! Please create a symbolic link to the\n");
		printf("terminfo directory on your system (probably /usr/lib/terminfo).\n");
		exit(1);
	}
	initConf();
	readConf("/usr/share/etc/stats.conf");
	readConf("/etc/stats.conf");
	gethostname(hostname, 30);
	/* Scott added 9-13-01. Set show=FALSE on all SAND interfaces. */
//	set_sand_ifaces_noshow();

	check_if_configmgr_iface_modified();
	get_if_list();
	get_ppp_list();
	sort_ifaces(0, TRUE);
	setup_curses();

	setup_modes();

	if (batch_mode) {
		nocbreak();
		if (strlen(interface)) {
			int new_current=-1;
			stat_if *stat;
			stat_if *cur;

			for (cur=ifaces,i=0;cur;cur=cur->next,i++) {
				stat = cur; 
				if (!strcasecmp(stat->full_name, interface))
				{
					new_current = i;
					break;
				}
			}
			if (new_current != -1)
				current = new_current;
			mode = &modes[DETAIL];
		}
		else
			mode = &modes[SUMMARY];
		mode->update(0);
		wrefresh(win);
		for (i=1;i<num_iterations;i++) {
				sleep(interval);
				mode->update(0);
				wrefresh(win);
		}
		echo();
		endwin();
		exit(0);
	}

	mode = &modes[SUMMARY];
	mode->update(0);
	overwrite(win, stdscr);

	while(1)
	{
		timeout(-1);
		key = tolower(getch());
		timeout(-1);

		switch(key)
		{
			case QUIT : goto quit;
			case SLEEP:
				setup_sleep();
			break;
			case D_MODE:
				if (&modes[DETAIL_CSU] == mode || &modes[INFO] == mode)
					mode = &modes[DETAIL];
				else if (-1 != setup_detail())
					mode = &modes[DETAIL];
				clear();
				setup_signals();
				mode->update(0);
			break;
			case C_MODE: /* CSU staticstics */
				if(-1 != setup_detail_csu())
					mode = &modes[DETAIL_CSU];
				clear();
				setup_signals();
				mode->update(0);
			break;
#if 0 /* Not ready yet! */
			case I_MODE: /* Card/Port Info */
				mode = &modes[INFO];
				clear();
				setup_signals();
				mode->update(0);
			break;
#endif
			case JUMP:  /* Jump to interface detail by name */
				if (-1 != setup_jump())
					mode = &modes[DETAIL];
				clear();
				setup_signals();
				mode->update(0);
			break;
			case S_MODE:
				mode = &modes[SUMMARY];
				setup_signals();
				mode->update(0);
			break;
			case NEXT:
				mode->next();
				clear();
				setup_signals();
				mode->update(0);
			break;
			case PREV:
				mode->prev();
				clear();
				setup_signals();
				mode->update(0);
			break;
			case ZERO:
				if (&modes[DETAIL_CSU] == mode)
				{
					setup_signals();
					mode->update(0);
				}
				else
				{
					zero(current);
					setup_signals();
					mode->update(0);
				}
			break;
			case HELP:
				display_help();
				clear();
				setup_signals();
				mode->update(0);
			case ERR:
			break;
			case SORT:
				setup_sort();
			break;
			case REVERSE:
				sort_ascending = sort_ascending ? FALSE : TRUE;
				wmove(win, LINES-1, 0);
				wclrtobot(win);
				wprintw(win, "Sorting list in %s order...", sort_ascending ? "ascending" : "descending");
				wrefresh(win);
				sort_ifaces(sort_method, sort_ascending);
				sleep(1);
				clear();
				setup_signals();
				mode->update(0);
			break;
			case KICK:
				if (&modes[DETAIL] == mode && detail_nif && detail_nif->if_type == PPP) {
					ignore_signals();
					wmove(win, LINES-1, 0);
					wclrtobot(win);
					wprintw(win, "Kicking user %s...", detail_nif->description);
					wrefresh(win);
					if (detail_nif->ppp.pppd_pid > 10)
						kill(detail_nif->ppp.pppd_pid, SIGTERM);
					sleep(3);
					get_ppp_list();
					clear();
//					/* Jump back to summary mode */
//					mode = &modes[SUMMARY];
					setup_signals();
					mode->update(0);
				}
			break;
			case DUMP:
				if (&modes[DETAIL] == mode && detail_nif) {
					setup_dump();
					clear();
					setup_signals();
					setup_colors();
					mode->update(0);
				}
				else if (&modes[SUMMARY] == mode) {
					setup_view();
					clear();
					setup_signals();
					setup_colors();
					mode->update(0);
				}
			break;
			default:
				clear();
				mode->update(0);
			break;
		}
//		usleep(100);
	}
quit:
	echo();
	nocbreak();
	endwin();
}

int setup_detail()
{
	int i;
	int t;

	ignore_signals();
	echo();

	wprintw(win, "\n");
	clrtoeol();
	wprintw(win, "Line number of the port to display: ");

	t = wscanw(win, "%d", &i);
	if(1 == t)
	{
		if(((current + i) < num_ifaces) && ((current +i) > -1))
			current += i;
		else
			goto detail_fail;
	}
	else
		goto detail_fail;

	noecho();
	return(0);

detail_fail:
	noecho();
	return(-1);
}

int setup_detail_csu()
{
	int i;
	int t;

	// Need to make sure this port has a CSU!
	return(0);
#if 0
	ignore_signals();
	echo();

	wprintw(win, "\n");
	clrtoeol();
	wprintw(win, "Line number of the port to display: ");

	t = wscanw(win, "%d", &i);
	if(1 == t)
	{
		if(((current + i) < info->num_ifaces) && ((current +i) > -1))
			current += i;
		else
			goto detail_fail;
	}
	else
		goto detail_fail;

	noecho();
	return(0);

detail_fail:
	noecho();
	return(-1);
#endif
}

int setup_jump()
{
	int i, new_current=-1, x, y;
	char name[256];
	stat_if *stat;
	stat_if *cur;

	ignore_signals();
	echo();

	wprintw(win, "\n");
	clrtoeol();
	wprintw(win, "Interface name to jump to: ");

	if (ERR == wgetnstr(win, name, 255))
		goto out;

	for (cur=ifaces,i=0;cur;cur=cur->next,i++)
	{
		stat = cur; 
		if (!strcasecmp(stat->full_name, name))
		{
			new_current = i;
			break;
		}
	}
	if (new_current != -1)
		current = new_current;
	else
	{
		getyx(win, y, x);
		wmove(win, y, 0);
		wclrtoeol(win);

		wattron(win, A_BOLD | COLOR_PAIR(ALARM));
		wprintw(win, "ERROR");
		wattroff(win, COLOR_PAIR(ALARM));
		wprintw(win, ": Interface \"%s\" not found!", name);
		wattroff(win, A_BOLD);

		wrefresh(win);
		sleep(2);
	}

out:
	noecho();
	return new_current; // -1 if not found
}

int setup_sleep()
{
	int i;
	int x, y;

	ignore_signals();
	echo();

	wprintw(win, "\n");
	wclrtoeol(win);
	wprintw(win, "Sleep interval (seconds): ");
	if(!wscanw(win, "%d", &i) || i <= 0)
	{
		getyx(win, y, x);
		wmove(win, y, 0);
		wclrtoeol(win);

		wattron(win, A_BOLD | COLOR_PAIR(ALARM));
		wprintw(win, "ERROR");
		wattroff(win, COLOR_PAIR(ALARM));
		wprintw(win, ": Sleep interval must be greater than 0!");
		wattroff(win, A_BOLD);

		wrefresh(win);
		sleep(2);
	}
	else
		interval = i;

	noecho();
	setup_signals();

	mode->update(0);
}

int setup_sort()
{
	int i;
	int x, y;

	ignore_signals();
	echo();

	wmove(win, LINES-15, 0);
	wclrtobot(win);
	wmove(win, LINES-12, 0);
	wprintw(win, "Available sorting methods:\n");
	wprintw(win, "1..Interface\n");
	wprintw(win, "2..Description\n");
	wprintw(win, "3..Encapsulation\n");
	wprintw(win, "4..Bandwidth\n");
	wprintw(win, "5..Hardware status\n");
	wprintw(win, "6..Protocol status\n");
	wprintw(win, "7..Inbound  usage\n");
	wprintw(win, "8..Outbound usage\n");
	wprintw(win, "9..In + Out usage\n");
	wprintw(win, "Choose a sorting method: ");
	if(!wscanw(win, "%d", &i) || i <= 0 || i > 9)
	{
		getyx(win, y, x);
		wmove(win, y, 0);
		wclrtoeol(win);

		wattron(win, A_BOLD | COLOR_PAIR(ALARM));
		wprintw(win, "ERROR");
		wattroff(win, COLOR_PAIR(ALARM));
		wprintw(win, ": Please choose an option between 1 and 9!");
		wattroff(win, A_BOLD);

		wrefresh(win);
		sleep(2);
	}
	else
	{
		sort_method = i;
		if (sort_method != SORT_INTERFACE)
		{
			wprintw(win, "Checking status of all interfaces...");
			wrefresh(win);
			get_all_stats();
			wprintw(win, "\r");
			wclrtoeol(win);
		}
		wprintw(win, "Sorting list...");
		wrefresh(win);
		switch(sort_method)
		{
			/* Default to descending order for these methods... */
			case SORT_BW:
			case SORT_INUSAGE:
			case SORT_OUTUSAGE:
			case SORT_INOUTUSAGE:
				sort_ascending = 0;
			break;
		}
		sort_ifaces(sort_method, sort_ascending);
		sleep(1);
	}

	noecho();
	setup_signals();

	mode->update(0);
}

void display_help()
{
	int key;

	ignore_signals();
	echo();

	wmove(win, 0, 0);
	werase(win); 
	wprintw(win, "Interactive commands are:\n\n");
	wprintw(win, "y   Summary mode\n");
	wprintw(win, "d   Detail mode\n");
	wprintw(win, "i   Detailed card/port setup information\n");
	wprintw(win, "c   CSU statistics (only valid in detail mode)\n");
	wprintw(win, "z   Zero counters  (only valid in detail mode)\n");
	wprintw(win, "s   Sleep interval\n");
	wprintw(win, "t   Sort interfaces by name, status, etc.\n");
	wprintw(win, "r   Reverse the sort order\n");
	wprintw(win, "j   Jump to an interface by name\n");
	wprintw(win, "p   Previous interface\n");
	wprintw(win, "n   Next interface\n");
	wprintw(win, "v   View traffic detail\n");
	wprintw(win, "q   Quit\n");
	wprintw(win, "\nPress any key to continue...");
	overwrite(win, stdscr);
	while(1)
	{
		timeout(0);
		key = getch();
		timeout(-1);
		switch(key)
		{
			case ERR:
			break;
			default:
				goto out;
			break;
		}
		usleep(100);
	}
out:
	;
}

void setup_colors()
{
	if(has_colors() && (start_color() == OK))
	{
		init_pair(DEFAULT, COLOR_WHITE, COLOR_BLACK);
		init_pair(     UP, COLOR_GREEN, COLOR_BLACK);
		init_pair(  ALARM,   COLOR_RED, COLOR_BLACK);
	}
}

void setup_curses()
{
	initscr();
	cbreak();
	nonl();
	noecho();
	keypad(stdscr, TRUE);

	setup_colors();
	win = newwin(0,0,0,0);
}

void setup_modes()
{
	modes[SUMMARY].update = summary_update;
	modes[SUMMARY].next   = summary_next;
	modes[SUMMARY].prev   = summary_prev;
	modes[DETAIL].update  = detail_update;
	modes[DETAIL].next    = detail_next;
	modes[DETAIL].prev    = detail_prev;
	modes[DETAIL_CSU].update  = detail_csu_update;
	modes[DETAIL_CSU].next    = detail_csu_next;
	modes[DETAIL_CSU].prev    = detail_csu_prev;
	modes[INFO].update  = info_update;
	modes[INFO].next    = info_next;
	modes[INFO].prev    = info_prev;
}

#ifndef PPP_CHECK_INTERVAL
#define PPP_CHECK_INTERVAL 10
#endif

void handle_alarm(int signum)
{
	static time_t next_ppp_check = 0;

	if (check_if_configmgr_iface_modified())
	{
		while(ifaces)
			del_if(ifaces);
		get_if_list();
	}
	if ((time(NULL)) > next_ppp_check) {
		get_ppp_list();
		next_ppp_check = time(NULL) + PPP_CHECK_INTERVAL;
	}
	if (num_ifaces <= 520 && mode == &modes[SUMMARY] && sort_method > 2)
	{
		get_all_stats();
		sort_ifaces(sort_method, sort_ascending);
	}
	mode->update(signum);
	wrefresh(win);
	setup_signals();
}

void setup_signals()
{
	signal(SIGALRM, handle_alarm);
	alarm(interval);
}

int STDATTR;
int HIGHATTR;
int BOXATTR;
int ACTIVEATTR;
int BARSTDATTR;
int BARHIGHATTR;
int BARPTRATTR;
int DLGTEXTATTR;
int DLGBOXATTR;
int DLGHIGHATTR;
int DESCATTR;
int STATUSBARATTR;
int IPSTATLABELATTR;
int IPSTATATTR;
int DESKTEXTATTR;
int PTRATTR;
int FIELDATTR;
int ERRBOXATTR;
int ERRTXTATTR;
int OSPFATTR;
int UDPATTR;
int IGPATTR;
int IGMPATTR;
int IGRPATTR;
int GREATTR;
int ARPATTR;
int UNKNIPATTR;
int UNKNATTR;

void setup_dump()
{
	char buf[256];
	char *cmd = NULL;
	struct MENU menu;

	int endloop = 0;
	int row = 1;
	int aborted;

	endloop = 0;
	do {
		standardcolors(1);
		ignore_signals();
		tx_initmenu(&menu, 11, 35, (LINES - 11) / 2, (COLS - 35) / 2,
			BOXATTR, STDATTR, HIGHATTR, BARSTDATTR, BARHIGHATTR,
			DESCATTR);

		tx_additem(&menu, " I^P^Traf",
			"Display traffic using IPTraf");
		tx_additem(&menu, " I^F^Top",
			"Display traffic using IFTop");
		tx_additem(&menu, NULL, NULL);
		tx_additem(&menu, " IPTraf ^d^etailed statistics",
			"Display detailed interface statistics using IPTraf");
		tx_additem(&menu, " IPTraf ^T^CP/UDP monitor",
			"Display summarized TCP/UDP traffic using IPTraf");
		tx_additem(&menu, " IPTraf ^p^acket size counts",
			"Display interface packet size counts using IPTraf");
		tx_additem(&menu, " IPTraf ^L^AN station monitor",
			"Display traffic by Ethernet MAC address using IPTraf");
		tx_additem(&menu, NULL, NULL);
		tx_additem(&menu, " E^x^it", "Exit to stats");

		tx_showmenu(&menu);
		tx_operatemenu(&menu, &row, &aborted);

		switch (row) {
			case 1:
				cmd = "iptraf -u -i";
			break;
			case 2:
				cmd = "iftop -i";
			break;
			case 4:
				cmd = "iptraf -u -d";
			break;
			case 5:
				cmd = "iptraf -u -s";
			break;
			case 6:
				cmd = "iptraf -u -z";
			break;
			case 7:
				cmd = "iptraf -u -l";
			break;
			case 9:
				endloop = 1;
			break;
		}
		tx_destroymenu(&menu);
		if (cmd) {
			snprintf(buf, 256, "%s %s", cmd, detail_nif->if_name);
			system(buf);
			cmd = NULL;
			clear();
			mode->update(0);
		}
	} while (!endloop);
}

void setup_view()
{
	char buf[256];
	char *cmd = NULL;
	struct MENU menu;

	int endloop = 0;
	int row = 1;
	int aborted;

	endloop = 0;
	do {
		standardcolors(1);
		ignore_signals();
		tx_initmenu(&menu, 5, 35, (LINES - 5) / 2, (COLS - 35) / 2,
			BOXATTR, STDATTR, HIGHATTR, BARSTDATTR, BARHIGHATTR,
			DESCATTR);

		tx_additem(&menu, " I^P^Traf",
			"Display traffic using IPTraf");
		tx_additem(&menu, NULL, NULL);
		tx_additem(&menu, " E^x^it", "Exit to stats");

		tx_showmenu(&menu);
		tx_operatemenu(&menu, &row, &aborted);

		switch (row) {
			case 1:
				cmd = "iptraf -u";
			break;
			case 3:
				endloop = 1;
			break;
		}
		tx_destroymenu(&menu);
		if (cmd) {
			snprintf(buf, 256, "%s", cmd);
			system(buf);
			cmd = NULL;
			clear();
			mode->update(0);
		}
	} while (!endloop);
}

void standardcolors(int color)
{
	if ((color) && (has_colors())) {
		init_pair(1, COLOR_BLUE, COLOR_WHITE);
		init_pair(2, COLOR_BLACK, COLOR_CYAN);
		init_pair(3, COLOR_CYAN, COLOR_BLUE);
		init_pair(4, COLOR_YELLOW, COLOR_RED);
		init_pair(5, COLOR_WHITE, COLOR_RED);
		init_pair(6, COLOR_BLUE, COLOR_CYAN);
		init_pair(7, COLOR_BLUE, COLOR_WHITE);
		init_pair(9, COLOR_RED, COLOR_WHITE);
		init_pair(10, COLOR_GREEN, COLOR_BLUE);
		init_pair(11, COLOR_CYAN, COLOR_BLACK);
		init_pair(12, COLOR_RED, COLOR_CYAN);
		init_pair(14, COLOR_YELLOW, COLOR_BLUE);
		init_pair(15, COLOR_YELLOW, COLOR_BLACK);
		init_pair(16, COLOR_WHITE, COLOR_CYAN);
		init_pair(17, COLOR_YELLOW, COLOR_CYAN);
		init_pair(18, COLOR_GREEN, COLOR_BLACK);
		init_pair(19, COLOR_WHITE, COLOR_BLUE);

		STDATTR = COLOR_PAIR(14) | A_BOLD;
		HIGHATTR = COLOR_PAIR(3) | A_BOLD;
		BOXATTR = COLOR_PAIR(3);
		ACTIVEATTR = COLOR_PAIR(10) | A_BOLD;
		BARSTDATTR = COLOR_PAIR(15) | A_BOLD;
		BARHIGHATTR = COLOR_PAIR(11) | A_BOLD;
		BARPTRATTR = COLOR_PAIR(18) | A_BOLD;
		DESCATTR = COLOR_PAIR(2);
		DLGTEXTATTR = COLOR_PAIR(2);
		DLGBOXATTR = COLOR_PAIR(6);
		DLGHIGHATTR = COLOR_PAIR(12);
		STATUSBARATTR = STDATTR;
		IPSTATLABELATTR = COLOR_PAIR(2);
		IPSTATATTR = COLOR_PAIR(12);
		DESKTEXTATTR = COLOR_PAIR(7);
		PTRATTR = COLOR_PAIR(10) | A_BOLD;
		FIELDATTR = COLOR_PAIR(1);
		ERRBOXATTR = COLOR_PAIR(5) | A_BOLD;
		ERRTXTATTR = COLOR_PAIR(4) | A_BOLD;
		OSPFATTR = COLOR_PAIR(2);
		UDPATTR = COLOR_PAIR(9);
		IGPATTR = COLOR_PAIR(12);
		IGMPATTR = COLOR_PAIR(10) | A_BOLD;
		IGRPATTR = COLOR_PAIR(16) | A_BOLD;
		ARPATTR = COLOR_PAIR(5) | A_BOLD;
		GREATTR = COLOR_PAIR(1);
		UNKNIPATTR = COLOR_PAIR(19) | A_BOLD;
		UNKNATTR = COLOR_PAIR(4) | A_BOLD;
	} else {
		STDATTR = A_REVERSE;
		HIGHATTR = A_REVERSE;
		BOXATTR = A_REVERSE;
		ACTIVEATTR = A_BOLD;
		BARSTDATTR = A_NORMAL;
		BARHIGHATTR = A_BOLD;
		BARPTRATTR = A_NORMAL;
		DESCATTR = A_BOLD;
		DLGBOXATTR = A_REVERSE;
		DLGTEXTATTR = A_REVERSE;
		DLGHIGHATTR = A_BOLD;
		STATUSBARATTR = A_REVERSE;
		IPSTATLABELATTR = A_REVERSE;
		IPSTATATTR = A_STANDOUT;
		DESKTEXTATTR = A_NORMAL;
		PTRATTR = A_REVERSE;
		FIELDATTR = A_BOLD;
		ERRBOXATTR = A_BOLD;
		ERRTXTATTR = A_NORMAL;
		OSPFATTR = A_REVERSE;
		UDPATTR = A_BOLD;
		IGPATTR = A_REVERSE;
		IGMPATTR = A_REVERSE;
		IGRPATTR = A_REVERSE;
		ARPATTR = A_BOLD;
		GREATTR = A_BOLD;
		UNKNIPATTR = A_BOLD;
		UNKNATTR = A_BOLD;
	}

	tx_init_error_attrs(ERRBOXATTR, ERRTXTATTR, ERRBOXATTR);
	tx_init_info_attrs(BOXATTR, STDATTR, HIGHATTR);
}

void ignore_signals()
{
	signal(SIGALRM, SIG_IGN);
}

char *footer(char *str)
{
	if(&modes[SUMMARY] == mode)
		sprintf(str, "d Detail | "); 
	else if(&modes[DETAIL] == mode && detail_nif && detail_nif->if_type == PPP)
		sprintf(str, "k Kick User | y Summary | "); 
	else if(&modes[DETAIL] == mode)
		sprintf(str, "c CSU | y Summary | "); 
	else if(&modes[DETAIL_CSU] == mode)
		sprintf(str, "y Summary | d Detail | "); 
	else if(&modes[INFO] == mode)
		sprintf(str, "c CSU | y Summary | d Detail | "); 
//	else
//		sprintf(str, "MODE IS WRONG  ");

	// Not enough room now on the detail pages!
	if(&modes[SUMMARY] == mode)
		strcat(str, "s Sleep interval | ");

	if(((current < num_ifaces) && (&modes[DETAIL] == mode)) ||
	   ((current < (num_ifaces - (LINES - 5))) && (&modes[SUMMARY] == mode)))
		strcat(str, "n Next | ");

	if(current > 0 && mode != &modes[DETAIL_CSU])
		strcat(str, "p Previous | ");
	
	if(&modes[DETAIL] == mode || &modes[DETAIL_CSU] == mode)
		strcat(str, "z Zero | ");

	strcat(str, "h Help | q Quit");
		
	return(str);
}
