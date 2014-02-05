/* vi:set ts=3: */
#include<stdio.h>
#include<error.h>
#include<string.h>
#include<stdlib.h>
#include<sys/time.h>
#include<sys/socket.h>
#include<linux/sockios.h>
#include<linux/if.h>
#include<linux/netdevice.h>
#include"read_proc.h"
#include"conf.h"
#include"sand.h"
extern int errno;
extern stat_if *last_nif;

const char *f_name_lu[] = {
	"",
	"Ethernet",
	"ppp"
};

const char *desc_lu[] = {
	"",
	"100Mbit Ethernet",
	"PPP connection"
};

const char *enc_lu[] = {
	"",
	"Ethernet",
	"PPP"
};

const unsigned long bw_lu[] = {
	0,
	12500000,
	4200
};

stat_if *find_if(stat_if *iface, const char *find)
{
	stat_if *tmp;
	for(tmp = iface; tmp; tmp = tmp->next)
		if(!strcmp(tmp->if_name, find))
			return tmp;
	return NULL;
}

void ifConf(stat_if *nif, confTable *table, char *name)
{
	char tmp[256] = "\0";
	char *tmp2 = NULL;

	if(strlen(table->rename))
	{
			  sscanf(name, "%[^0123456789]", tmp);
			  tmp2 = &(name[strlen(tmp)]);
			  /* BRI devices need some renumbering too (divide by 2)! BRI0 is 0-1, BRI1 is 2-3, etc */
			  if (nif->if_type == BRI) {
					int devnum = atoi(tmp2);
					devnum /= 2;
			      sprintf(tmp, "%s%d",table->rename, devnum);
			  }
			  else
			      sprintf(tmp, "%s%s",table->rename, tmp2);
	}
	strncpy(nif->description, table->description, 50);
	stat_if_name(nif, name, tmp);
	strncpy(nif->encapsulation, table->encapsulation, 15);
	nif->bandwidth = table->bandwidth;
}

int getADSL(stat_if *nif)
{
	FILE *fp;
	char name[256] = "\0";
	char tmp2[256] = "\0";
	char *tmp, *ptr;
	confTable *cur = NULL;
	int DownstreamRate = 0, UpstreamRate = 0, status = 0;
	int i;

	if (nif->if_type != ADSL)
		return -1;

  	fp	= fopen(ADSL_FILE, "r");
	if(!fp)
		return(-1);

/*
 * Read the header information from ADSL_FILE
 */
	fgets(tmp2, 256, fp);

/*
 * Read the first line of ADSL_FILE
 */
	i = fscanf(fp, "%[^:]%*c %d %d", name, &DownstreamRate, &UpstreamRate);
	fgets(tmp2, 256, fp);

/*
 * Go until the interface is found
 */
	while(!feof(fp))
	{
		tmp = alltrim(name);
		if (!strcmp(tmp, nif->if_name))
		{
			/* If either down or up is non-zero, we're up! In reality both should be non-zero */
			if (DownstreamRate != 0 || UpstreamRate != 0)
				status = 1;
			nif->hw_status = status;
			nif->proto_status = status;
			nif->bandwidth = (DownstreamRate * 1000) / 8;
			break;
		}
	
		i = fscanf(fp, "%[^:]%*c %d %d", name, &DownstreamRate, &UpstreamRate);
		fgets(tmp2, 256, fp);
	}
	fclose(fp);
	return(0);
}

int net_read(stat_if *nif)
{
	FILE *fp = fopen(NET_FILE, "r");
	char name[256] = "\0";
	char tmp2[256] = "\0";
	char *tmp, *ptr;
	unsigned long vars[32];
	confTable *cur = NULL;
	int i;

	if(!fp)
		return(-1);

	for(i = 0; i < 27; i++)
		vars[i] = 0;

/*
 * Read the header information from NET_FILE
 */
	fgets(tmp2, 256, fp);
	fgets(tmp2, 256, fp);

/*
 * Read the first line of NET_FILE
 */
	i = fscanf(fp, 
	   "%[^:]%*c %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
	   name, &vars[RX_BYTE], &vars[RX_PACK], &vars[RX_ERROR], &vars[RX_DROP], 
	   &vars[RX_FIFO], &vars[RX_FRAME], &vars[RX_COMP], &vars[RX_MULTI],
	   &vars[TX_BYTE], &vars[TX_PACK], &vars[TX_ERROR], &vars[TX_DROP], &vars[TX_FIFO],
	   &vars[TX_COLL], &vars[TX_CARR], &vars[TX_COMP]);
	fgets(tmp2, 256, fp);
//printf("name:vars[RX_BYTE] = %s:%lu\n", name, vars[RX_BYTE]);

/*
 * Go until the interface is found
 */
	while(!feof(fp))
	{
		tmp = alltrim(name);
		if (!strcmp(tmp, nif->if_name))
		{
			stat_if_set(nif, vars);
			switch (nif->if_type)
			{
				case ETH:
					ptr = strchr(nif->if_name, '.');
					if(ptr)
						sscanf(ptr, ".%d", &(nif->vlan_id));
					getEthernetMII(nif);
			  	break;
				case BRI:
					getBRI(nif);
				break;
				case PPP:
					getPPP(nif);
				break;
				case ADSL:
					getADSL(nif);
				break;
				default:
					stat_s_set(&(nif->up_timestamp), 0);
					nif->hw_status = 1;
					nif->proto_status = is_running(nif->if_name);
				break;
			}
			break;
		}
	
		i = fscanf(fp, 
		    "%[^:]%*c %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
		name, &vars[RX_BYTE], &vars[RX_PACK], &vars[RX_ERROR], &vars[RX_DROP], 
		&vars[RX_FIFO], &vars[RX_FRAME], &vars[RX_COMP], &vars[RX_MULTI],
		&vars[TX_BYTE], &vars[TX_PACK], &vars[TX_ERROR], &vars[TX_DROP], &vars[TX_FIFO],
		&vars[TX_COLL], &vars[TX_CARR], &vars[TX_COMP]);

		fgets(tmp2, 256, fp);
	}
	fclose(fp);
	return(0);
}

int sand_read(stat_if *stat)
{
	char tmp[256] = "\0";
	char tmp2[256] = "\0";
	char *ptr;
	unsigned long vars[32];
	unsigned long bw;
	unsigned long t = 0;
	int i = 0;
	int inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;
	struct sockaddr_in *sin;
	int retval;
	struct sand_iface_stats s;

	if(inet_sock == -1)
		return 0;
	
	for(i = 0; i < 27; i++)
		vars[i] = 255;


	strcpy(ifr.ifr_name, stat->if_name);
	memset(&s, 0, sizeof(struct sand_iface_stats));
	ifr.ifr_ifru.ifru_data = (char *)&s;

	sin = (struct sockaddr_in *)(&ifr.ifr_addr);
	retval = ioctl(inet_sock, SIOCGSANDIFACESTATS, &ifr);

	close(inet_sock);

  	if (retval < 0)
		return 0;

	ptr = strchr(stat->if_name, '.');
	if(ptr)
		sscanf(ptr, ".%d", &(stat->proto_sub));

	stat->if_type = SAND;

	strncpy(stat->description, s.description, 50);
	strncpy(stat->encapsulation, s.proto_name, 13);

	vars[HW_STAT] = s.hw_status;
	vars[PR_STAT] = s.proto_status;
 	bw = s.bw;

	vars[RX_BYTE] = s.rx_bytes;
	vars[RX_PACK] = s.rx_packets;
	vars[RX_ERROR] = s.rx_errors;
	vars[RX_DROP] = s.rx_dropped;
	vars[RX_FIFO] = s.rx_fifo_errors;
	vars[RX_CRC] = s.rx_crc_errors;
	vars[RX_FRAME] = s.rx_frame_errors;
	vars[TX_BYTE] = s.tx_bytes;
	vars[TX_PACK] = s.tx_packets;
	vars[TX_ERROR] = s.tx_errors;
	vars[TX_DROP] = s.tx_dropped;
	vars[TX_FIFO] = s.tx_fifo_errors;
  	vars[TX_COLL] = s.tx_carrier_errors;
				           
	stat->dcd_transitions.current = s.dcd_transitions;
	vars[DCD] = s.dcd;
	vars[DSR] = s.dsr;
  	vars[DTR] = s.dtr;
  	vars[RTS] = s.rts;
  	vars[CTS] = s.cts;
				
	stat->rx.last = s.last_rx;
	stat->tx.last = s.last_tx;
	stat->up_timestamp.current = s.proto_up;
	stat->bandwidth = bw / 8;
	stat_if_set(stat, vars);
	return(1);
}

#if defined (SIOCGIFSTATS)
int net_ioctl_read(stat_if *stat)
{
	char tmp[256] = "\0";
	char tmp2[256] = "\0";
	char *ptr;
	unsigned long vars[32];
	unsigned long bw;
	unsigned long t = 0;
	int i = 0;
	int inet_sock = socket(AF_INET, SOCK_DGRAM, 0);
	struct ifreq ifr;
	struct sockaddr_in *sin;
	int retval;
	struct net_device_stats s;

	if(inet_sock == -1)
		return 0;
	
	for(i = 0; i < 27; i++)
		vars[i] = 255;


	strcpy(ifr.ifr_name, stat->if_name);
	memset(&s, 0, sizeof(struct net_device_stats));
	ifr.ifr_ifru.ifru_data = (char *)&s;

	sin = (struct sockaddr_in *)(&ifr.ifr_addr);
	retval = ioctl(inet_sock, SIOCGIFSTATS, &ifr);

	close(inet_sock);

  	if (retval < 0)
		return 0;

	vars[RX_BYTE] = s.rx_bytes;
	vars[RX_PACK] = s.rx_packets;
	vars[RX_ERROR] = s.rx_errors;
	vars[RX_DROP] = s.rx_dropped;
	vars[RX_FIFO] = s.rx_fifo_errors;
	vars[RX_CRC] = s.rx_crc_errors;
	vars[RX_FRAME] = s.rx_frame_errors;
	vars[TX_BYTE] = s.tx_bytes;
	vars[TX_PACK] = s.tx_packets;
	vars[TX_ERROR] = s.tx_errors;
	vars[TX_DROP] = s.tx_dropped;
	vars[TX_FIFO] = s.tx_fifo_errors;
  	vars[TX_COLL] = s.collisions;
				           
	stat_if_set(stat, vars);
	switch (stat->if_type)
	{
		case ETH:
			getEthernetMII(stat);
			ptr = strchr(stat->if_name, '.');
			if(ptr)
				sscanf(ptr, ".%d", &(stat->vlan_id));
	  	break;
		case BRI:
			getBRI(stat);
		break;
		case PPP:
			getPPP(stat);
		break;
		case ADSL:
			getADSL(stat);
		break;
		default:
			stat_s_set(&(stat->up_timestamp), 0);
			stat->hw_status = 1;
			stat->proto_status = is_running(stat->if_name);
		break;
	}

	return(1);
}
#else

int net_ioctl_read(stat_if *stat)
{
	return 0;
}
#endif

#if 0 /* This doesn't work! It only gets the first interface (lo) */
int netlink_read(stat_if *stat)
{
	char tmp[256] = "\0";
	char tmp2[256] = "\0";
	char *ptr;
	unsigned long vars[32];
	unsigned long bw;
	unsigned long t = 0;
	int i = 0;
	struct rtnl_handle rth;
	struct ifreq ifr;
	struct {
		struct nlmsghdr 	n;
		struct ifinfomsg	ifi;
	} req;
	struct nlmsghdr 	answer;
	int retval;
	struct net_device_stats *s;
	struct ifinfomsg *ifi;
	struct rtattr * tb[IFLA_MAX+1];
	int len;

	if (rtnl_open(&rth, 0) < 0)
		return 0;
	
	for(i = 0; i < 27; i++)
		vars[i] = 255;

	memset(&req, 0, sizeof(req));

	req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
	req.n.nlmsg_flags = NLM_F_MATCH|NLM_F_REQUEST;
	req.n.nlmsg_type = RTM_GETLINK;
	req.ifi.ifi_family = AF_UNSPEC;
	if (!stat->if_index)
		stat->if_index = get_if_index(stat->if_name);
	req.ifi.ifi_index = stat->if_index;

	if (rtnl_talk(&rth, &req.n, 0, 0, &answer, NULL, NULL) < 0)
	{
		rtnl_close(&rth);
		return 0;
	}

  	ifi = NLMSG_DATA(&answer);
	memset(tb, 0, sizeof(tb));
  	len = answer.nlmsg_len;
	len -= NLMSG_LENGTH(sizeof(*ifi));
	if (len < 0)
		return 0;
	parse_rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), len);

	s = RTA_DATA(tb[IFLA_STATS]);

	vars[RX_BYTE] = s->rx_bytes;
	vars[RX_PACK] = s->rx_packets;
	vars[RX_ERROR] = s->rx_errors;
	vars[RX_DROP] = s->rx_dropped;
	vars[RX_FIFO] = s->rx_fifo_errors;
	vars[RX_CRC] = s->rx_crc_errors;
	vars[RX_FRAME] = s->rx_frame_errors;
	vars[TX_BYTE] = s->tx_bytes;
	vars[TX_PACK] = s->tx_packets;
	vars[TX_ERROR] = s->tx_errors;
	vars[TX_DROP] = s->tx_dropped;
	vars[TX_FIFO] = s->tx_fifo_errors;
  	vars[TX_COLL] = s.collisions;
				           
	stat_if_set(stat, vars);
	switch (stat->if_type)
	{
		case ETH:
			getEthernetMII(stat);
	  	break;
		case BRI:
			getBRI(stat);
		break;
		default:
			stat_s_set(&(stat->up_timestamp), 0);
			stat->hw_status = 1;
			stat->proto_status = 1;
		break;
	}
	rtnl_close(&rth);
	return(1);
}
#endif
