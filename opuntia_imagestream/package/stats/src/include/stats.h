/******************************************************************************/
/* (C)1999 Imagestream Internet Solutions, Inc. All rights reserved worldwide.*/
/******************************************************************************/
/* This code Liscensed under the GPL Version 2. See COPYING for details. */

#ifndef STATS_H
#define STATS_H

#include<time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include"stat_if.h"

#define SUMMARY 0
#define DETAIL  1
#define DETAIL_CSU 2
#define INFO	3

#define DEFAULT 1
#define UP      2
#define ALARM   3

#define QUIT    'q'
#define SLEEP   's'
#define D_MODE  'd'
#define C_MODE	'c'
#define JUMP    'j'
#define S_MODE  'y'
#define NEXT    'n'
#define PREV    'p'
#define ZERO    'z'
#define HELP    'h'
#define I_MODE	'i'
#define SORT	't'
#define REVERSE	'r'
#define KICK	'k'
#define DUMP	'v'

#define SORT_INTERFACE		1
#define SORT_DESC		2
#define SORT_ENCAPS		3
#define SORT_BW			4
#define SORT_HW			5
#define SORT_PROTO		6
#define SORT_INUSAGE		7
#define SORT_OUTUSAGE		8
#define SORT_INOUTUSAGE		9

typedef struct mode_struct
{  
   void (*update)(int signum);
   void (*next)();
   void (*prev)();
} mode_struct;

void setup_curses();
void setup_modes();
void setup_signals();
void ignore_signals();

void summary_update(int signum);
void summary_next();
void summary_prev();

void detail_update(int signum);
void detail_next();
void detail_prev();

void detail_csu_update(int signum);
void detail_csu_next();
void detail_csu_prev();

void info_update(int signum);
void info_next();
void info_prev();

void up_down(short status);
char *byte_format(unsigned long bandwidth, int interval);
char *bps(stat_bp *stat, int interval, int scale);
int percent(stat_bp *stat, unsigned long bandwidth, int interval);
char *comma_format(unsigned long num);

char *show_time(time_t timestamp, int type);

char *header(char *str, char *title);
char *footer(char *str);
char *getaddr(char *working, char *if_name, int request);
char *bw_format(unsigned long bandwidth, char *str);
int RoundInt(double val);
#endif /* STATS_H */
