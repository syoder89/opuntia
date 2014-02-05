/* vi:set ts=3: */
/* Mode: C;
 * mii-diag.c: Examine and set the MII registers of a network interfaces.

	Usage:	mii-diag [-vw] interface.

	Notes:
	The compile-command is at the end of this source file.
	This program only works with drivers that implement MII ioctl() calls.

	Written/copyright 1997-2001 by Donald Becker <becker@scyld.com>

	This program is free software; you can redistribute it
	and/or modify it under the terms of the GNU General Public
	License as published by the Free Software Foundation.

	The author may be reached as becker@scyld.com, or C/O
	 Scyld Computing Corporation
	 410 Severn Ave., Suite 210
	 Annapolis MD 21403

	References
	http://scyld.com/expert/mii-status.html
	http://scyld.com/expert/NWay.html
	http://www.national.com/pf/DP/DP83840.html
*/

static char version[] =
"mii-diag.c:v2.03 11/5/2001 Donald Becker (becker@scyld.com)\n"
" http://www.scyld.com/diag/index.html\n";

static const char usage_msg[] =
"Usage: mii-diag [-aDfrRvVw] [-AF <to-advertise>] [--watch] <interface>.\n";

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
#include "conf.h"

#if defined(SIOCGPARAMS)  && SIOCGPARAMS != SIOCDEVPRIVATE+3
#error Changed definition for SIOCGPARAMS
#else
#define SIOCGPARAMS (SIOCDEVPRIVATE+3) 		/* Read operational parameters. */
#define SIOCSPARAMS (SIOCDEVPRIVATE+4) 		/* Set operational parameters. */
#endif

struct option longopts[] = {
 /* { name  has_arg  *flag  val } */
    {"all-interfaces", 0, 0, 'a'},	/* Show all interfaces. */
	{"Advertise",	1, 0, 'A'},		/* Change the capabilities advertised. */
    {"debug",       0, 0, 'D'},		/* Increase the debug level. */
    {"force",       0, 0, 'f'},		/* Force the operation. */
    {"parameters",  0, 0, 'G'},		/* Write general settings value. */
    {"phy",			1, 0, 'p'},		/* Set the PHY (MII address) to report. */
    {"restart",		0, 0, 'r'},		/* Restart the link negotiation */
    {"Reset",		0, 0, 'R'},		/* Reset the transceiver. */
    {"status",		0, 0, 's'},		/* Non-zero exit status w/ no link beat. */
    {"help", 		0, 0, '?'},		/* Give help */
    {"verbose", 	0, 0, 'v'},		/* Report each action taken.  */
    {"version", 	0, 0, 'V'},		/* Emit version information.  */
    {"watch", 		0, 0, 'w'},		/* Constantly monitor the port.  */
    { 0, 0, 0, 0 }
};

/* Trivial versions if we don't link against libmii.c */
#define MEDIA_OFFSET_10 0
#define MEDIA_OFFSET_100 2
#define MEDIA_OFFSET_1000 6
#define MEDIA_TX_START 0
#define MEDIA_FX_START 8
#define MEDIA_OFFSET_LINK_DOWN MEDIA_OFFSET_10 + 16

static const char *media_names[] = {
	"10baseT", "10baseT-FD", "100baseTx", "100baseTx-FD", "100baseT4",
	"Flow-control", "1000baseTx", "1000baseTx-FD", "10baseFx", "10baseFx-FD",
	"100baseFx", "100baseFx-FD", ""/* Placeholder for 100baseT4 */, ""/* Placeholder for Flow-control */,
	"1000baseFx", "1000baseFx-FD", "Link down", 0,
};

/* Usually in libmii.c, but trivial substitions are below. */
extern int  show_mii_details(long ioaddr, int phy_id);
extern void monitor_mii(long ioaddr, int phy_id);

static int nway_advertise = -1;
static int fixed_speed = -1;
static int override_phy = -1;

/* Internal values. */
int new_ioctl_nums;
int skfd = -1;					/* AF_INET socket for ioctl() calls.	*/
struct ifreq ifr;

int do_one_xcvr(int skfd);
int show_basic_mii(long ioaddr, int phy_id);
int mdio_read(int skfd, int phy_id, int location);
static int parse_advertise(const char *capabilities);

extern WINDOW *win;

int getEthTool(stat_if *nif)
{
	int err;
	int max_capability = -1;

	nif->has_ethtool = 0;

	/* Hardware up/down? */
	nif->edata.cmd = ETHTOOL_GLINK;
	ifr.ifr_data = (caddr_t)&nif->edata;
	err = ioctl(skfd, SIOCETHTOOL, &ifr);
	if (err < 0)
		return err;

	nif->hw_status = (nif->edata.data != 0);

	nif->ecmd.cmd = ETHTOOL_GSET;
	ifr.ifr_data = (caddr_t)&nif->ecmd;
	err = ioctl(skfd, SIOCETHTOOL, &ifr);
	if (err < 0)
		return err;

	if (nif->bandwidth == 0) {
		if (nif->hw_status == 0) { /* Link is down. Use highest supported bandwidth */
		  	if (nif->ecmd.supported & (SUPPORTED_1000baseT_Half | SUPPORTED_1000baseT_Full))
				nif->bandwidth = 1000000000 / 8;
		  	else if (nif->ecmd.supported & (SUPPORTED_100baseT_Half | SUPPORTED_100baseT_Full))
				nif->bandwidth = 100000000 / 8;
		  	else if (nif->ecmd.supported & (SUPPORTED_10baseT_Half | SUPPORTED_10baseT_Full))
				nif->bandwidth = 10000000 / 8;
			else
				nif->bandwidth = 0; /* Huh? Card doesn't support anything? */
		}
		else switch(nif->ecmd.speed) { /* Link is up. What are we running at? */
			case SPEED_10:
				nif->bandwidth = 10000000 / 8;
			break;
			default:
			case SPEED_100:
				nif->bandwidth = 100000000 / 8;
			break;
			case SPEED_1000:
				nif->bandwidth = 1000000000 / 8;
			break;
		}
	}
	if (nif->ecmd.duplex == DUPLEX_HALF)
		nif->duplex = 1;
	else
		nif->duplex = 0;

	if (nif->hw_status != 0) { /* Link is up. */
		if (nif->ecmd.speed == SPEED_1000)
			max_capability = 5 + MEDIA_OFFSET_1000 + ((nif->ecmd.port == PORT_FIBRE) ? MEDIA_FX_START : MEDIA_TX_START) +
				  		((nif->duplex == 0) ? 1 : 0);
		if (nif->ecmd.speed == SPEED_100)
			max_capability = 5 + MEDIA_OFFSET_100 + ((nif->ecmd.port == PORT_FIBRE) ? MEDIA_FX_START : MEDIA_TX_START) +
				  		((nif->duplex == 0) ? 1 : 0);
		if (nif->ecmd.speed == SPEED_10)
			max_capability = 5 + MEDIA_OFFSET_10 + ((nif->ecmd.port == PORT_FIBRE) ? MEDIA_FX_START : MEDIA_TX_START) +
				  		((nif->duplex == 0) ? 1 : 0);
	}
	if (max_capability != -1)
		strcpy(nif->encapsulation, media_names[max_capability - 5]);
	nif->has_ethtool = 1;
	if (nif->hw_status == 0)
		nif->proto_status = 0;
	else
		nif->proto_status = is_up(nif->if_name) ? 1 : 128;
	return 0;
}

static inline void mii_read(stat_if *nif)
{
	u16 *data = (u16 *)(&ifr.ifr_data);
	unsigned phy_id = data[0];
	long ioaddr = (long)skfd;
	int i;

	for (i=0;i<16;i++)
		nif->eth_mii.mii_val[i] = mdio_read(ioaddr, phy_id, i);
}

void getEthernetMII(stat_if *nif)
{
	int retval;
	int old_hw = nif->hw_status;
	char **spp, *ptr;
	char vlan = FALSE;
	confTable *cur;

	/* Reset bandwidth/description from stats.conf file. */
	nif->bandwidth = 0;
	nif->description[0] = '\0';
	cur = hasConfEntry(nif->if_name);
	if(cur)
		ifConf(nif, cur, nif->if_name);

	/* Open a basic socket. */
	if ((skfd = socket(AF_INET, SOCK_DGRAM,0)) < 0) {
		goto out_bad;
	}

	/* Get the vitals from the interface. */
	strncpy(ifr.ifr_name, nif->if_name, IFNAMSIZ);
	/* If it's a VLAN interface, find the stats for the master by
	 * dropping the .X part of the name */
	if ((ptr = strchr(ifr.ifr_name, '.')))
	{
		*ptr = '\0';
		vlan = TRUE;
	}
	/* Need mii registers for link partner capabilities */
	retval = getEthTool(nif);

	/* Find if SIOCGMIIPHY is the new or old constant. */
	if (ioctl(skfd, 0x8947, &ifr) >= 0) {
		new_ioctl_nums = 1;
	} else if (ioctl(skfd, SIOCDEVPRIVATE, &ifr) >= 0) {
		new_ioctl_nums = 0;
	} else if (!nif->has_ethtool) /* No ethtool and no MII? */
		goto out_bad;
	mii_read(nif);

	if (retval != 0) {
		/* Get details from MII registers instead of ethtool */
		retval = getBasicMII(nif);
	}
out_bad:
	if (retval != 0)
	{
		// No mii? Check device flags for IFF_RUNNING. Set it to N/A at 1Gb/s.
		nif->hw_status = is_running(nif->if_name);
		if (nif->hw_status == 0)
			nif->proto_status = 0;
		else
			nif->proto_status = is_up(nif->if_name) ? 1 : 128;
		if (nif->bandwidth == 0)
			nif->bandwidth = 1000000000 / 8;
		nif->duplex = 0;
		nif->eth_mii.mii_val[0] = 0xffff;
	}
	if (skfd >= 0)
		(void) close(skfd);
#if 0 /* This would be cool, but it's only good for the current
			stats session! This needs to come from the driver itself! */
	// If the hardware status has changed, reset the timestamp!
	if (old_hw != nif->hw_status)
		stat_s_set(&(nif->up_timestamp), time(NULL));
#endif
	// Make the description match the interface speed.
	if (!vlan)
	{
		if (!strcmp(nif->description, "100Mb Ethernet") ||
				!strcmp(nif->description, "10Mb Ethernet") ||
				!strcmp(nif->description, "1Gb Ethernet") ||
				!strlen(nif->description))
		{
			if (nif->bandwidth > (100000000 / 8)) // Bw is > 100Mbit
				strcpy(nif->description, "1Gb Ethernet");
			else if (nif->bandwidth > (10000000 / 8)) // Bw is > 10 Mbit
				strcpy(nif->description, "100Mb Ethernet");
			else if (nif->bandwidth > 0) // Bw is <= 10 Mbit
				strcpy(nif->description, "10Mb Ethernet");
		}
	}
	return;
}

#if 0
int do_one_xcvr(int skfd)
{
	u16 *data = (u16 *)(&ifr.ifr_data);
	unsigned phy_id = data[0];

	if (override_phy >= 0) {
		printf("Using the specified MII PHY index %d.\n", override_phy);
		phy_id = override_phy;
	}

	show_basic_mii(skfd, phy_id);
#ifdef LIBMII
	if (verbose)
		show_mii_details(skfd, phy_id);
#else
	if (verbose || debug) {
		int mii_reg, eth_mii.mii_val;
		printf(" MII PHY #%d transceiver registers:", phy_id);
		for (mii_reg = 0; mii_reg < 32; mii_reg++) {
			eth_mii.mii_val = mdio_read(skfd, phy_id, mii_reg);
			printf("%s %4.4x", (mii_reg % 8) == 0 ? "\n  " : "",
				   eth_mii.mii_val);
		}
		printf("\n");
	}
#endif

	if (opt_watch)
		monitor_mii(skfd, phy_id);
	if (opt_status &&
		(mdio_read(skfd, phy_id, 1) & 0x0004) == 0)
		exit(2);
	return 0;
}
#endif

int mdio_read(int skfd, int phy_id, int location)
{
	u16 *data = (u16 *)(&ifr.ifr_data);

	data[0] = phy_id;
	data[1] = location;

	if (ioctl(skfd, new_ioctl_nums ? 0x8948 : SIOCDEVPRIVATE+1, &ifr) < 0) {
		return -1;
	}
	return data[3];
}

#if 0
/* Parse the command line argument for advertised capabilities. */
static int parse_advertise(const char *capabilities)
{
	const char *mtypes[] = {
		"100baseT4", "100baseTx", "100baseTx-FD", "100baseTx-HD",
		"10baseT", "10baseT-FD", "10baseT-HD", 0,
	};
	int cap_map[] = { 0x0200, 0x0180, 0x0100, 0x0080, 0x0060, 0x0040, 0x0020,};
	int i;
	if ( ! capabilities) {
		fprintf(stderr, "You passed -A 'NULL'.  You must provide a media"
				" list to advertise!\n");
		return -1;
	}
	if (debug)
		fprintf(stderr, "Advertise string is '%s'.\n", capabilities);
	for (i = 0; mtypes[i]; i++)
		if (strcasecmp(mtypes[i], capabilities) == 0)
			return cap_map[i];
	if ((i = strtol(capabilities, NULL, 16)) <= 0xffff)
		return i;
	fprintf(stderr, "Invalid media advertisement '%s'.\n", capabilities);
	return -1;
}
#endif

// Rates in bits per second
static const unsigned long media_rates[] = {
	10000000, 10000000, 100000000, 100000000, 100000000,
	100000000, 0,
};
// 0 = full, 1 = half
static const char media_duplex[] = {
	1, 0, 1, 0, 0, 1, 0,
};
/* Various non-good bits in the command register. */
static const char *bmcr_bits[] = {
	"  Internal Collision-Test enabled!\n", "",		/* 0x0080,0x0100 */
	"  Restarted auto-negotiation in progress!\n",
	"  Transceiver isolated from the MII!\n",
	"  Transceiver powered down!\n", "", "",
	"  Transceiver in loopback mode!\n",
	"  Transceiver currently being reset!\n",
};

int getBasicMII(stat_if *nif)
{
	int i;
	u16 bmcr, bmsr, new_bmsr, nway_advert, lkpar;
	int hw_status = 0;

	bmcr			= nif->eth_mii.mii_val[0];
	bmsr			= nif->eth_mii.mii_val[1];
	nway_advert	= nif->eth_mii.mii_val[4];
	lkpar 		= nif->eth_mii.mii_val[5];

	// Link status register.
	if ((bmsr & 0x0016) == 0x0004)
		hw_status = 1;

	if (bmcr & 0x1000 && lkpar & 0x4000)
	{
		int negotiated = nway_advert & lkpar & 0x3e0;
		int max_capability = 0;
		/* Scan for the highest negotiated capability, highest priority
		   (100baseTx-FDX) to lowest (10baseT-HDX). */
		int media_priority[] = {8, 9, 7, 6, 5}; 	/* media_names[i-5] */
		for (i = 0; media_priority[i]; i++)
			if (negotiated & (1 << media_priority[i])) {
				max_capability = media_priority[i];
				break;
			}
		if (max_capability)
		{
			if (nif->bandwidth == 0)
				nif->bandwidth = media_rates[max_capability - 5] / 8;
			nif->duplex = media_duplex[max_capability - 5];
			strcpy(nif->encapsulation, media_names[max_capability - 5]);
		}
		else
		{
//			nif->bandwidth = 0;
			nif->duplex = 0;
			hw_status = 0;
		}
	}
	else
	{
		if (bmcr & 0x2000) // 100 Mbps
		{
			if (nif->bandwidth == 0)
				nif->bandwidth = 100000000 / 8;
			strcpy(nif->encapsulation, "100baseTx");
		}
   	else if (lkpar & 0x00A0) // See if it's a strange 10/100 hub
		{
			if (lkpar & 0x0080)
			{
				if (nif->bandwidth == 0)
					nif->bandwidth = 100000000 / 8;
				strcpy(nif->encapsulation, "100baseTx");
			}
			else
			{
				if (nif->bandwidth == 0)
					nif->bandwidth = 10000000 / 8;
				strcpy(nif->encapsulation, "10baseT");
			}
			// Duplex is misreported in this case. It should be half.
			bmcr = 0;
		}
		else // 10 Mbps
		{
			if (nif->bandwidth == 0)
				nif->bandwidth = 10000000 / 8;
			strcpy(nif->encapsulation, "10baseT");
		}
		if (bmcr & 0x0100) // Full duplex
		{
			nif->duplex = 0;
			strcat(nif->encapsulation, "-FD");
		}
		else // Half duplex
			nif->duplex = 1;
	}
	nif->hw_status = hw_status;
	nif->proto_status = is_up(nif->if_name) ? 1 : 128;
	return 0;
}

#define check_remote_fault(nif) \
	(nif->has_ethtool) ? 0 : (bmsr & 0x0010)

#define check_link_jabber(nif) \
	(nif->has_ethtool) ? 0 : (bmsr & 0x0002)

#define check_link_beat(nif) \
	(nif->has_ethtool) ? nif->hw_status : ((bmsr & 0x0016) == 0x0004)

#define check_autonegotiation(nif) \
	(nif->has_ethtool) ?  (nif->ecmd.autoneg != AUTONEG_DISABLE) : (bmcr & 0x1000)

//#define check_link_partner_autonegotiation(nif) \
//	(nif->has_ethtool) ? (nif->ecmd.advertising & ADVERTISED_Autoneg): (lkpar & 0x4000)

#define check_link_partner_autonegotiation(nif) \
	(lkpar & 0x4000)

#define media_print(printed, i) \
{ \
	if (printed == 4) { \
		printed = 1; \
		line_str = ""; \
		wprintw(win, "\n                           "); \
	} \
	else if (printed) \
		wprintw(win, ", "); \
	printed++; \
	wprintw(win, "%s", media_names[i]); \
}

int displayDetailMII(stat_if *nif)
{
	int i, printed;
	u16 bmcr, bmsr, new_bmsr, nway_advert, lkpar;

	if (nif->eth_mii.mii_val[0] == 0xffff) {
//		wprintw(win, "  No MII transceiver present!.\n");
		wprintw(win, "\n\n\n");
		return -1;
	}
	/* Descriptive rename. */
	bmcr = nif->eth_mii.mii_val[0];
	bmsr = nif->eth_mii.mii_val[1];
	nway_advert = nif->eth_mii.mii_val[4];
	lkpar = nif->eth_mii.mii_val[5];

	wprintw(win, "     Link status: ");
	printed = 0;

	wattron(win, A_BOLD);
	if(has_colors())
		wattron(win, COLOR_PAIR(ALARM));
	if (check_remote_fault(nif))
	{
		wprintw(win, "Remote fault detected");
		printed = 1;
	}
	if (check_link_jabber(nif))
	{
		if (printed)
			wprintw(win, ", ");
		wprintw(win, "Link jabber detected");
		printed = 1;
	}
	wattroff(win, A_BOLD);
	if(has_colors())
		wattroff(win, COLOR_PAIR(ALARM));

	if (!printed)
	{
		if (check_link_beat(nif))
		{
			if(has_colors())
				wattron(win, COLOR_PAIR(UP));
			wprintw(win, "Link beat established\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(UP));
		}
		else
		{
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Link beat not established\n");
			wattroff(win, A_BOLD);
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
		}
	}

	wprintw(win, "     Auto-negotiation: ");
	if (check_autonegotiation(nif))
	{
		wprintw(win, "enabled (");
		if (check_link_partner_autonegotiation(nif))
		{
			int max_capability = MEDIA_OFFSET_LINK_DOWN;
			if (nif->has_ethtool && nif->hw_status) {
				switch (nif->ecmd.speed) {
					case SPEED_10:
						max_capability = 5 + MEDIA_OFFSET_10 + ((nif->ecmd.port == PORT_FIBRE) ? MEDIA_FX_START : MEDIA_TX_START) +
							  	((nif->duplex == 0) ? 1 : 0);
					break;
					default:
					case SPEED_100:
						max_capability = 5 + MEDIA_OFFSET_100 + ((nif->ecmd.port == PORT_FIBRE) ? MEDIA_FX_START : MEDIA_TX_START) +
							  	((nif->duplex == 0) ? 1 : 0);
					break;
					case SPEED_1000:
						max_capability = 5 + MEDIA_OFFSET_1000 + ((nif->ecmd.port == PORT_FIBRE) ? MEDIA_FX_START : MEDIA_TX_START) +
							  	((nif->duplex == 0) ? 1 : 0);
					break;
				}
			}
			else {
				int negotiated = nway_advert & lkpar & 0x3e0;
				/* Scan for the highest negotiated capability, highest priority
		   		(100baseTx-FDX) to lowest (10baseT-HDX). */
				int media_priority[] = {8, 9, 7, 6, 5}; 	/* media_names[i-5] */
				for (i = 0; media_priority[i]; i++)
					if (negotiated & (1 << media_priority[i])) {
						max_capability = media_priority[i];
						break;
				}
			}
			if (max_capability)
				wprintw(win, "%s)\n", media_names[max_capability - 5]);
			else
			{
				wattron(win, A_BOLD);
				if(has_colors())
					wattron(win, COLOR_PAIR(ALARM));
				wprintw(win, "negotiation failure!");
				if(has_colors())
					wattroff(win, COLOR_PAIR(ALARM));
				wattroff(win, A_BOLD);
				wprintw(win, ")\n");
			}
		}
		else
		{
			if (nif->hw_status == 0) {
				wprintw(win, "Link down");
				wprintw(win, ")\n");
			}
			else {
				wprintw(win, "Link partner does not support auto-negotiation");
				wprintw(win, ")\n");
			}
		}
	}
	else
	{
		wprintw(win, "disabled (Forced speed is");
		wprintw(win, " 10%s Mbps, %s-duplex)\n",
			   bmcr & 0x2000 ? "0" : "",
			   bmcr & 0x0100 ? "full":"half");
	}

	wprintw(win, "     Partner capabilities: ");
	printed = 0;
	if (check_link_partner_autonegotiation(nif)) {
		int idx = (nif->ecmd.port == PORT_FIBRE) ? MEDIA_FX_START : MEDIA_TX_START;
		char *line_str = "\n";
/* This is what we're advertising! Not what the link partner is...
		if (nif->has_ethtool) {
			u_int32_t mask = nif->ecmd.advertising;

			if (mask & ADVERTISED_1000baseT_Full)
				media_print(printed, MEDIA_OFFSET_1000 + idx + 1);
			if (mask & ADVERTISED_1000baseT_Half)
				media_print(printed, MEDIA_OFFSET_1000 + idx);
			if (mask & ADVERTISED_100baseT_Full)
				media_print(printed, MEDIA_OFFSET_100 + idx + 1);
			if (mask & ADVERTISED_100baseT_Half)
				media_print(printed, MEDIA_OFFSET_100 + idx);
			if (mask & ADVERTISED_10baseT_Full)
				media_print(printed, MEDIA_OFFSET_10 + idx + 1);
			if (mask & ADVERTISED_10baseT_Half)
				media_print(printed, MEDIA_OFFSET_10 + idx);
		}
		else {
*/
		if (lkpar & 0x8000) { /* Check for gigE */
			u16 mii_mssr = nif->eth_mii.mii_val[10];
			if (mii_mssr & 0x0800) /* 1000base-FD */
				media_print(printed, MEDIA_OFFSET_1000 + idx + 1);
			if (mii_mssr & 0x0400) /* 1000base-HD */
				media_print(printed, MEDIA_OFFSET_1000 + idx);
		}
		for (i = 4; i >= 0; i--) {
			if (lkpar & (0x20<<i))
				media_print(printed, i);
		}
		wprintw(win, "%s", line_str);
	} 
	else if (nif->has_ethtool) {
		if (nif->hw_status == 0) {
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Link partner not detected\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		}
		else if (!(check_autonegotiation(nif)))
			wprintw(win, "Information not exchanged in forced speed mode\n");
		else {
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Link partner does not support auto-negotiation\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		}
	}
	else { /* MII */
		if (lkpar & 0x00A0)
			wprintw(win, "%s (no auto-negotiation)\n",
			   	lkpar & 0x0080 ? "100baseTx" : "10baseT");
		else if ( ! (bmcr & 0x1000))
			wprintw(win, "Information not exchanged in forced speed mode\n");
		else if ( ! (new_bmsr & 0x004))
		{
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Link partner not detected\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		}
		else if (lkpar == 0x0001  ||  lkpar == 0x0000) {
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Link partner does not support auto-negotiation\n");
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		}
		else
		{
			wattron(win, A_BOLD);
			if(has_colors())
				wattron(win, COLOR_PAIR(ALARM));
			wprintw(win, "Link partner is sending an unknown status %4.4x\n", lkpar);
			if(has_colors())
				wattroff(win, COLOR_PAIR(ALARM));
			wattroff(win, A_BOLD);
		}
	}

	return 0;
}


#if 0
int show_basic_mii(long ioaddr, int phy_id)
{
	int mii_reg, i;
	u16 eth_mii.mii_val[32];
	u16 bmcr, bmsr, new_bmsr, nway_advert, lkpar;

	for (mii_reg = 0; mii_reg < 8; mii_reg++)
		eth_mii.mii_val[mii_reg] = mdio_read(ioaddr, phy_id, mii_reg);
	if ( ! verbose) {
		printf("Basic registers of MII PHY #%d: ", phy_id);
		for (mii_reg = 0; mii_reg < 8; mii_reg++)
			printf(" %4.4x", eth_mii.mii_val[mii_reg]);
		printf(".\n");
	}

	if (eth_mii.mii_val[0] == 0xffff) {
		printf("  No MII transceiver present!.\n");
		return -1;
	}
	/* Descriptive rename. */
	bmcr = eth_mii.mii_val[0];
	bmsr = eth_mii.mii_val[1];
	nway_advert = eth_mii.mii_val[4];
	lkpar = eth_mii.mii_val[5];

	if (lkpar & 0x4000) {
		int negotiated = nway_advert & lkpar & 0x3e0;
		int max_capability = 0;
		/* Scan for the highest negotiated capability, highest priority
		   (100baseTx-FDX) to lowest (10baseT-HDX). */
		int media_priority[] = {8, 9, 7, 6, 5}; 	/* media_names[i-5] */
		printf(" The autonegotiated capability is %4.4x.\n", negotiated);
		for (i = 0; media_priority[i]; i++)
			if (negotiated & (1 << media_priority[i])) {
				max_capability = media_priority[i];
				break;
			}
		if (max_capability)
			printf("The autonegotiated media type is %s.\n",
				   media_names[max_capability - 5]);
		else
			printf("No common media type was autonegotiated!\n"
				   "This is extremely unusual and typically indicates a "
				   "configuration error.\n" "Perhaps the advertised "
				   "capability set was intentionally limited.\n");
	}
	printf(" Basic mode control register 0x%4.4x:", bmcr);
	if (bmcr & 0x1000)
		printf(" Auto-negotiation enabled.\n");
	else
		printf(" Auto-negotiation disabled, with\n"
			   " Speed fixed at 10%s mbps, %s-duplex.\n",
			   bmcr & 0x2000 ? "0" : "",
			   bmcr & 0x0100 ? "full":"half");
	for (i = 0; i < 9; i++)
		if (bmcr & (0x0080<<i))
			printf(bmcr_bits[i]);

	new_bmsr = mdio_read(ioaddr, phy_id, 1);
	if ((bmsr & 0x0016) == 0x0004)
		printf( " You have link beat, and everything is working OK.\n");
	else
		printf(" Basic mode status register 0x%4.4x ... %4.4x.\n"
			   "   Link status: %sestablished.\n",
			   bmsr, new_bmsr,
			   bmsr & 0x0004 ? "" :
			   (new_bmsr & 0x0004) ? "previously broken, but now re" : "not ");
	if (verbose) {
		printf("   This transceiver is capable of ");
		if (bmsr & 0xF800) {
			for (i = 15; i >= 11; i--)
				if (bmsr & (1<<i))
					printf(" %s", media_names[i-11]);
		} else
			printf("<Warning! No media capabilities>");
		printf(".\n");
		printf("   %s to perform Auto-negotiation, negotiation %scomplete.\n",
			   bmsr & 0x0008 ? "Able" : "Unable",
			   bmsr & 0x0020 ? "" : "not ");
	}

	if (bmsr & 0x0010)
		printf(" Remote fault detected!\n");
	if (bmsr & 0x0002)
		printf("   *** Link Jabber! ***\n");

	if (lkpar & 0x4000) {
		printf(" Your link partner advertised %4.4x:",
			   lkpar);
		for (i = 5; i >= 0; i--)
			if (lkpar & (0x20<<i))
				printf(" %s", media_names[i]);
		printf("%s.\n", lkpar & 0x0400 ? ", w/ 802.3X flow control" : "");
	} else if (lkpar & 0x00A0)
		printf(" Your link partner is generating %s link beat  (no"
			   " autonegotiation).\n",
			   lkpar & 0x0080 ? "100baseTx" : "10baseT");
	else if ( ! (bmcr & 0x1000))
		printf(" Link partner information is not exchanged when in"
			   " fixed speed mode.\n");
	else if ( ! (new_bmsr & 0x004))
							;	/* If no partner, do not report status. */
	else if (lkpar == 0x0001  ||  lkpar == 0x0000) {
		printf(" Your link partner does not do autonegotiation, and this "
			   "transceiver type\n  does not report the sensed link "
			   "speed.\n");
	} else
		printf(" Your link partner is strange, status %4.4x.\n", lkpar);

	printf("   End of basic transceiver information.\n\n");
	return 0;
}
#endif


/*
 * Local variables:
 *  version-control: t
 *  kept-new-versions: 5
 *  c-indent-level: 4
 *  c-basic-offset: 4
 *  tab-width: 4
 *  compile-command: "gcc -Wall -Wstrict-prototypes -O mii-diag.c -DLIBMII libmii.c -o mii-diag"
 *  simple-compile-command: "gcc mii-diag.c -o mii-diag"
 * End:
 */
