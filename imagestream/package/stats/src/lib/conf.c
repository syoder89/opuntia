#include"conf.h"
#include"read_proc.h"
#include<stdio.h>
#include<stdlib.h>

confTable *tableHead = NULL;
confTable *tableTail = NULL;
confTable *cur = NULL;

confTable *hasConfEntry(char *name)
{
	confTable *retval = NULL;
	char temp[256];
	cur = tableHead;

//printf("Looking up %s.\n", name);
	while(cur)
	{
//printf("Checking for %s against %s.\n", name, cur->name);
		if(((sscanf(name, cur->name, temp)) > 0) || (!strcmp(name, cur->name)))
{
//printf("Found it!\n");
			retval = cur;
}
		cur = cur->next;
	}
//printf("Returning 0x%x!\n", retval);
	return retval;
}

confTable *new_confTable()
{
	confTable *tmp = malloc(sizeof(confTable));
	if(tmp)
		bzero(tmp, sizeof(confTable));
	return(tmp);
}

int addEntry(char *name)
{
	confTable *tmp = hasConfEntry(name);
	tableTail->next = new_confTable();
	tableTail = tableTail->next;
	cur = tableTail;
	if(tmp)
	{
			  memcpy(cur, tmp, sizeof(confTable));
			  cur->next = NULL;
	}
}

int initConf()
{
	tableHead = new_confTable();
	tableTail = tableHead;

	addEntry("Blank");
	strcpy(cur->name,"%s");
	strcpy(cur->description, "");
	strcpy(cur->encapsulation, "");
	cur->show = TRUE;
	cur = NULL;
}

int readConf(char *file)
{
	FILE *fd = fopen(file, "r");
	char readvar[128] = "\0";
	unsigned long i = 0;
	int scan = 0;

	if(!fd)
		return(5);
	
	scan = fscanf(fd, "%s", readvar);
	while(scan != EOF)
	{
		if(readvar[0] == '#')
			fscanf(fd, " %128[^\n]%*c", readvar);
		else if(!strcmp(NAME, readvar))
		{
			fscanf(fd, " %20[^\n]%*c", readvar);
			if(!cur)
				addEntry(readvar);
			strncpy(cur->name, readvar, 20);
		}
		else if(!strcmp(RNME, readvar))
		{
			if(!cur)
				goto err_conf;
			fscanf(fd, " %20[^\n]%*c", cur->rename);
		}
		else if(!strcmp(DESC, readvar))
		{
			if(!cur)
				goto err_conf;
			fscanf(fd, " %50[^\n]%*c", cur->description);
		}
		else if(!strcmp(BAND, readvar))
		{
			if(!cur)
				goto err_conf;
			fscanf(fd, "%lu", &(cur->bandwidth));
		}
		else if(!strcmp(ENCP, readvar))
		{
			if(!cur)
				goto err_conf;
			fscanf(fd, " %20[^\n]%*c", cur->encapsulation);
		}
		else if(!strcmp(SHOW, readvar))
		{
			if(!cur)
				goto err_conf;
			cur->show = TRUE;
		}
		else if(!strcmp(NOSH, readvar))
		{
			if(!cur)
				goto err_conf;
			cur->show = FALSE;
		}
		else if(!strcmp(DONE, readvar))
			cur = NULL;
		else
			goto err_conf;
		scan = fscanf(fd, "%s", readvar);
	}
	return(FALSE);

err_conf:
printf("Error in %s\n", file);
printf("\t%s is not a valid config option\n", readvar);
exit(1);
}

/* This function was written by Scott on 9-13-01. I use it to scan the
 * /proc/net/sand/interfaces file and set the show flag to false for each
 * type of interface found there. This eliminates the need to have a
 * stats.conf file with Serial%s and Bonder%d entries! I found out the hard
 * way that any SAND interface without a config option or with the config
 * option's show flag set to TRUE will not display the utilization and
 * packet per second statistics properly!
 */
int set_sand_ifaces_noshow(void)
{
	FILE *fp = fopen(SAND_FILE, "r");
	char name[256] = "\0";
	char *ptr;
	unsigned long vars[27];
	int i = 0;

	if(!fp)
		return(-1);
	
	for(i = 0; i < 27; i++)
		vars[i] = 255;
/*
 * Read the header line.
 */
	fscanf(fp, "%*[^\n]%*c");

	while(!feof(fp))
	{
		/* Read in the interface name */
		i = fscanf(fp, "%[^:]%*c", name);
		if(-1 != i)
		{
			/* Replace a name like Serial10 with Serial */
			/* Search for the first non-alpha character and null terminate there */

			for (ptr = name; *ptr && isalpha(*ptr); ptr++)
				; /* Nothing */
			/* Null terminate at the non-alpha character */
			if (*ptr)
				*ptr = '\0';

			/* Append %s to the end so we will match all interfaces
			 * that begin with name i.e. if name=Bonder then match
			 * Bonder0, Bonder1, etc. */

			strcat(name, "%s");

			/* Did the user already have a conf entry set up?
			 * If so, don't override their settings. */
			cur = hasConfEntry(name);

			/* Ok. I hate this, but... There is a blank entry
			 * in the list with the name %s. That will cause a
			 * match on everything! Thus I need to check if
			 * cur->name exactly matches the name we're trying to
			 * look for. If it doesn't, it means that we've matched
			 * the blank entry and must add our own entry */
			if(!cur || (cur && strcmp(name, cur->name)))
			{
				/* Add our new entry for this SAND interface */
				addEntry(name);
				if (cur)
				{
					/* Set the show flag to false */
					cur->show = FALSE;
					/* Set the name */
					strncpy(cur->name, name, 20);
				}
			}
		}
		/* If not at end of file, read rest of line */
		if (!feof(fp))
			fscanf(fp, "%*[^\n]%*c");
	}
	/* Reset cur just in case */
	cur = NULL;
	fclose(fp);
	return 0;
}
