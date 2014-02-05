/******************************************************************************/
/* (C)1999 Imagestream Internet Solutions, Inc. All rights reserved worldwide.*/
/******************************************************************************/
/* This code Liscensed under the GPL Version 2. See COPYING for details. */

#ifndef READ_PROC_H
#define READ_PROC_H
#include "stat_if.h"

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define NET_FILE  "/proc/net/dev"
#define SAND_FILE "/proc/net/sand/interfaces"
#define ADSL_FILE  "/proc/net/adsl_interfaces"
#define CONF_FILE "stats.conf"
#define CONFIGMGR_IFACE_FILE	"/tmp/configmgr.ifaces"

#define NUM_FILES 4
#define NONE      0
#define ETH       1
#define PPP       2
#define SAND      3
#define BRI	  4
#define BRIDGE    5
#define ADSL	  6

extern int num_ifaces;
extern stat_if *ifaces, *ifaces_tail;

void add_if(stat_if *stat);
stat_if *find_if(stat_if *iface, const char *find);
int has_ip(char *name);
int file_read(char *read);

#endif /* READ_PROC_H */
