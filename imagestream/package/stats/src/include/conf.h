/******************************************************************************/
/* (C)1999 Imagestream Internet Solutions, Inc. All rights reserved worldwide.*/
/******************************************************************************/
/* This code Liscensed under the GPL Version 2. See COPYING for details. */

#ifndef CONF_H
#define CONF_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define NAME "interface"
#define RNME "rename"
#define DESC "description"
#define BAND "bandwidth"
#define ENCP "encapsulation"
#define NOSH "noshow"
#define SHOW "show"
#define DONE "!"

typedef struct confTable
{
	char              name[32];
	char              rename[32];
	char              description[128];
	unsigned long     bandwidth;
	char              encapsulation[32];
	char              show;
	char		  pad[3];
	struct confTable *next;
} confTable;

confTable *hasConfEntry(char *name);
int readConf(char *file);
int initConf();
#endif
