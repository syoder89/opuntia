/* vi:set ts=3: */
#include<string.h>
#include<assert.h>
#include<linux/sockios.h>
#include<stdio.h>
#include<time.h>
#include<ncurses.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<linux/if.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include"stat_if.h"
#include"read_proc.h"
#include"conf.h"
#include"stats.h"

char *alltrim(char *String);
int *data = NULL;
time_t configmgr_iface_mtime = 0;
stat_if *find_iface_name(char *ifname);

/*
 * This is all to deal with the stat_r struct
 * Although I don't think anything will actually *USE* it for much
 */
stat_r *stat_r_new()
{
	stat_r *stat = (stat_r *)malloc(sizeof(stat_r));
	stat->type = STAT_R;

	stat_r_construct(stat);
	return(stat);
}

void stat_r_construct(stat_r *stat)
{
	stat->type |= STAT_R;
}

void stat_r_destruct(stat_r *stat)
{
	if(stat && (STAT_R & stat->type))
	{
		stat->type = 0;
		free(stat);
	}
}

/*
 * This is all to deal with the stat_s struct
 */
stat_s *stat_s_new()
{
	stat_s *stat = (stat_s *)malloc(sizeof(stat_s));
	((stat_r *)stat)->type = STAT_R;

	stat_s_construct(stat);
	return(stat);
}

void stat_s_construct(stat_s *stat)
{
	((stat_r *)stat)->type |= STAT_S;

	stat_r_construct((stat_r *)stat);

	stat->current = 0;
	stat->zero = 0;
}

void stat_s_destruct(stat_s *stat)
{
	if(stat && (STAT_S & ((stat_r *)stat)->type))
	{
		((stat_r *)stat)->type = 0;
		free(stat);
	}
}

void stat_s_set(stat_s *stat, unsigned long set)
{
	if(stat && (STAT_S & ((stat_r *)stat)->type))
		stat->current = set;
}

void stat_s_add(stat_s *stat, unsigned long add)
{
	if(stat && (STAT_S & ((stat_r *)stat)->type))
		stat->current += add;
}

void stat_s_zero(stat_s *stat)
{
	if(stat && (STAT_S & ((stat_r *)stat)->type))
		stat->zero = stat->current;
}

unsigned long stat_s_show(stat_s *stat)
{
	if(stat && (STAT_S & ((stat_r *)stat)->type))
		return(stat->current - stat->zero);
}


/*
 * This is all to deal with the stat_bp struct.
 *   stat_s_<functions> will work with stat_bp as well, 
 *   although you should typecast a stat_bp to stat_s first
 */

stat_bp *stat_bp_new()
{
	stat_bp *stat = (stat_bp *)malloc(sizeof(stat_bp));
	((stat_r *)stat)->type = STAT_BP;

	stat_bp_construct(stat);
	return(stat);
}

void stat_bp_construct(stat_bp *stat)
{
	((stat_r *)stat)->type |= STAT_BP;
	
	stat_s_construct((stat_s *)stat);

	stat->previous = 0;
	stat->rolled = 0;
}

void stat_bp_destruct(stat_bp *stat)
{
	if(stat && (STAT_BP & ((stat_r *)stat)->type))
	{
		((stat_r *)stat)->type = 0;
		free(stat);
	}
}

void stat_bp_set(stat_bp *stat, unsigned long set)
{
	if(stat && (STAT_BP & ((stat_r *)stat)->type))
	{
		stat->previous = stat->parent.current;
		stat->parent.current = set;
	}
}

unsigned long stat_bp_delta(stat_bp *stat)
{
	if(stat && (STAT_BP & ((stat_r *)stat)->type))
	{
		return(((stat_s *)stat)->current - stat->previous);
	}
}

/*
 * This is all to deal with the stat_rt struct.
 *   Use the internals directly, there's no reason not to.
 */
stat_rt *stat_rt_new()
{
	stat_rt *stat = (stat_rt *)malloc(sizeof(stat_rt));
	((stat_r *)stat)->type = STAT_RT;

	stat_rt_construct(stat);
	return(stat);
}

void stat_rt_construct(stat_rt *stat)
{
	((stat_r *)stat)->type |= STAT_RT;

	stat_r_construct((stat_r *)stat);

	stat_bp_construct(&(stat->byte));

	stat_bp_construct(&(stat->packet));

	stat_s_construct(&(stat->error));
	stat_s_construct(&(stat->drop));
	stat_s_construct(&(stat->fifo));
	stat_s_construct(&(stat->compressed));
	stat->last = 0;
}

void stat_rt_destruct(stat_rt *stat)
{
	if(stat && (STAT_RT & ((stat_r *)stat)->type))
	{
		((stat_r *)stat)->type = 0;
		free(stat);
	}
}

/*
 * This is all to deal with the stat_if struct.
 *   Use the internals directly, there's no reason not to.
 */

stat_if *stat_if_new()
{
	stat_if *stat = (stat_if *)malloc(sizeof(stat_if));
	memset(stat, 0, sizeof(stat_if));
	((stat_r *)stat)->type = STAT_IF;

	stat_if_construct(stat);
	return(stat);
}

void stat_if_construct(stat_if *stat)
{
	((stat_r *)stat)->type |= STAT_IF;

	stat_r_construct((stat_r *)stat);

	stat_rt_construct(&(stat->rx));
	stat_s_construct(&(stat->rx_frame));
	stat_s_construct(&(stat->rx_crc));
	stat_s_construct(&(stat->rx_multicast));
	stat_rt_construct(&(stat->tx));
	stat_s_construct(&(stat->tx_carrier));
	stat_s_construct(&(stat->tx_collisions));

	stat_s_construct(&(stat->up_timestamp));
	stat_s_construct(&(stat->dcd_transitions));
}

void stat_if_destruct(stat_if *stat)
{
	if(stat && (STAT_IF & ((stat_r *)stat)->type))
	{
		((stat_r *)stat)->type = 0;
		free(stat);
	}
}

void stat_if_name(stat_if *stat, const char *name, const char *f_name)
{
	int i;
	if(stat && (STAT_IF & ((stat_r *)stat)->type))
	{
		strncpy(stat->if_name, name, 15);
		
		if(strlen(f_name))
			strncpy(stat->full_name, f_name, 15);
		else
			strncpy(stat->full_name, name, 15);
	}
#ifdef __DEBUG__
	else
		assert(0);
#endif
}

void stat_if_set(stat_if *stat, unsigned long vars[])
{
	if(stat && (STAT_IF & ((stat_r *)stat)->type))
	{

		stat->hw_status    = (short)vars[HW_STAT];
		stat->proto_status = (short)vars[PR_STAT];
		stat->dcd          = (short)vars[DCD];
		stat->dsr          = (short)vars[DSR];
		stat->dtr          = (short)vars[DTR];
		stat->rts          = (short)vars[RTS];
		stat->cts          = (short)vars[CTS];
 
		stat_bp_set(&(stat->rx.byte),      vars[RX_BYTE]);
		stat_bp_set(&(stat->rx.packet),    vars[RX_PACK]);
		stat_s_set(&(stat->rx.error),      vars[RX_ERROR]);
		stat_s_set(&(stat->rx.drop),       vars[RX_DROP]);
		stat_s_set(&(stat->rx.fifo),       vars[RX_FIFO]);
		stat_s_set(&(stat->rx.compressed), vars[RX_COMP]);
		stat_s_set(&(stat->rx_frame),      vars[RX_FRAME]);
		stat_s_set(&(stat->rx_crc),        vars[RX_CRC]);
		stat_s_set(&(stat->rx_multicast),  vars[RX_MULTI]);
		stat_bp_set(&(stat->tx.byte),      vars[TX_BYTE]);
		stat_bp_set(&(stat->tx.packet),    vars[TX_PACK]);
		stat_s_set(&(stat->tx.error),      vars[TX_ERROR]);
		stat_s_set(&(stat->tx.drop),       vars[TX_DROP]);
		stat_s_set(&(stat->tx.fifo),       vars[TX_FIFO]);
		stat_s_set(&(stat->tx.compressed), vars[TX_COMP]);
		stat_s_set(&(stat->tx_carrier),    vars[TX_CARR]);
		stat_s_set(&(stat->tx_collisions), vars[TX_COLL]);
	}
}

void addrof(char *working, struct sockaddr_in *sin)
{
   unsigned long addr = ntohl(sin->sin_addr.s_addr);
   unsigned int xbyte;
   int addr_byte[4], i = 0;

   for(i = 0; i < 4; i++)
   {
      xbyte = addr >> (i*8);
      xbyte = xbyte & (unsigned int)0x000000FF;
      addr_byte[i] = xbyte;
   }
   sprintf(working, "%u.%u.%u.%u", addr_byte[3], addr_byte[2], addr_byte[1], addr_byte[0]);
}

int has_ip(char *name)
{
	char working[20] ="\0";
	int inet_sock = socket(AF_INET, SOCK_DGRAM, 0), retval = 0;
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strcpy(ifr.ifr_name, name);

	if(inet_sock != -1)
	{
		sin = (struct sockaddr_in *)(&ifr.ifr_addr);

		if(ioctl(inet_sock, SIOCGIFADDR, &ifr) >=0)
			retval = TRUE;
		else
			retval = FALSE;
	}
	else
		retval = FALSE;

	close(inet_sock);
	
	return(retval);
}

int check_flag(char *name, int mask)
{
	char working[20] ="\0";
	int inet_sock = socket(AF_INET, SOCK_DGRAM, 0), retval = FALSE;
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strcpy(ifr.ifr_name, name);

	if(inet_sock != -1)
	{
		sin = (struct sockaddr_in *)(&ifr.ifr_addr);

		if(ioctl(inet_sock, SIOCGIFFLAGS, &ifr) >=0)
		{
			if (ifr.ifr_flags & mask)
				retval = TRUE;
		}
	}

	close(inet_sock);
	return retval;
}

int is_up(char *name)
{
	return(check_flag(name, IFF_UP));
}

int is_running(char *name)
{
	return(check_flag(name, IFF_RUNNING));
}

int get_if_index(char *name)
{
	char working[20] ="\0";
	int inet_sock = socket(AF_INET, SOCK_DGRAM, 0), retval = FALSE;
	struct ifreq ifr;
	struct sockaddr_in *sin;

	strcpy(ifr.ifr_name, name);

	if(inet_sock != -1)
	{
		sin = (struct sockaddr_in *)(&ifr.ifr_addr);

		if(ioctl(inet_sock, SIOCGIFINDEX, &ifr) >=0)
			return ifr.ifr_ifindex;
	}

	close(inet_sock);
	
	return(retval);
}

int check_if_configmgr_iface_modified(void)
{
	struct stat st;

	if ((stat(CONFIGMGR_IFACE_FILE, &st)) == 0)
	{
		if (st.st_mtime != configmgr_iface_mtime)
		{
			configmgr_iface_mtime = st.st_mtime;
			return TRUE;
		}
	}
	return FALSE;
}

void get_ppp_list(void)
{
	FILE *fp;
	char name[256], tmp[1024];
	confTable *cur;
	stat_if *nif, *tnif;
	int type;

	/* Mark all PPP interfaces for deletion first */
	for (nif = ifaces; nif; nif=nif->next) {
		if (nif->if_type == PPP)
			nif->delete = 1;
	}

	// Don't read from configmgr! Get up-to-date info from proc
	fp = fopen(NET_FILE, "r");
	if (!fp)
		return;
	/*
	 * Read the header information from NET_FILE
	 */
	fscanf(fp, "%*[^\n]%*c");
	fscanf(fp, "%*[^\n]%*c");

	while((fgets(tmp, 1024, fp)))
	{
		sscanf(tmp, "%[^:]%*c", name); 
		strcpy(name, alltrim(name));
		if (!strncasecmp(name, "ppp", 3)) {
			int is_new = 0;
			type = PPP;

			cur = hasConfEntry(name);
			if(type == NONE && cur && cur->show != TRUE)
				continue;

			nif = find_iface_name(name);
			if (!nif) {
				nif = stat_if_new();
				is_new = 1;
			}
			nif->if_type = type;
			if(cur)
				ifConf(nif, cur, name);

			if (is_new)
				add_if(nif);
			else
				nif->delete = 0;
		}
	}
	fclose(fp);
	for (nif = ifaces, tnif=(nif)?nif->next:NULL; nif; nif=tnif, tnif=(nif)?nif->next:NULL) {
		if (nif->delete && nif->if_type == PPP) {
//			del_if(nif);
			/* Avoid problems with interfaces jumping when looking at detail by not removing
			 * interfaces from the list. */
			nif->delete = 0;
			strncpy(nif->description, "Disconnected user", sizeof(nif->description));
			nif->description[sizeof(nif->description)-1] = '\0';
			nif->hw_status = 0;
			nif->proto_status = 0;
		}
	}
	return;
}

void get_if_list(void)
{
	FILE *fp = fopen(CONFIGMGR_IFACE_FILE, "r");
	char name[256], tmp[1024];
	confTable *cur;
	stat_if *nif;
	int type;

	if (!fp)	// Configmgr not running? Get all then!
	{
		fp = fopen(NET_FILE, "r");
		if (!fp)
			return;
		/*
		 * Read the header information from NET_FILE
		 */
		fscanf(fp, "%*[^\n]%*c");
		fscanf(fp, "%*[^\n]%*c");

		while((fgets(tmp, 1024, fp)))
		{
			sscanf(tmp, "%[^:]%*c", name); 
			strcpy(name, alltrim(name));
			/* If configmgr isn't running then we don't have any
			 * Sand interfaces! */
			if (!strncasecmp(name, "eth", 3))
				type = ETH;
			else if (!strncasecmp(name, "ippp", 4))
				type = BRI;
			else if (!strncasecmp(name, "ppp", 3))
				type = PPP;
			else
			{
				// Only add this interface if it's up!
				if (!is_up(name))
					continue;
				type = NONE;
			}
			cur = hasConfEntry(name);
			if(type == NONE && cur && cur->show != TRUE)
				continue;

			nif = stat_if_new();
			nif->if_type = type;
			if(cur)
				ifConf(nif, cur, name);
	
			add_if(nif);
		}
		fclose(fp);
		return;
	}
	while((fgets(tmp, 1024, fp)))
	{
		type = 0;
		sscanf(tmp, "%255s %d", name, &type);
		strcpy(name, alltrim(name));
		// Only add this interface if it's up!
		if (type == NONE && !is_up(name))
			continue;
		cur = hasConfEntry(name);
		if(type == NONE && cur && cur->show != TRUE)
			continue;

		nif = stat_if_new();
		if ((!strncasecmp(name, "ppp", 3)))
			type = PPP;
		nif->if_type = type;
		if(cur)
			ifConf(nif, cur, name);

		add_if(nif);
	}
	fclose(fp);
}

char *alltrim(char *String) {
	char *ptr = String, *ptr2 = NULL, *returnptr = String;

	if (ptr) {
		while(*ptr && *ptr == ' ')
			ptr++;
		returnptr = ptr;
		while(1) {
			while(*ptr && *ptr != ' ')
				ptr++;
			if (*ptr) {
				ptr2 = ptr;
				while(*ptr && *ptr == ' ')
					ptr++;
				if (*ptr)
					ptr2 = NULL;
			}
			else break;
		}
		if (ptr2)
			*ptr2 = '\0';
	}
	return(returnptr);
}

void get_stats(stat_if *nif)
{
	if (nif->first_load)
	{
		gettimeofday(&nif->cur_timestamp, NULL);
		nif->first_load = FALSE;
		get_stats(nif);
	}
	memcpy(&nif->last_timestamp, &nif->cur_timestamp, sizeof(struct timeval));

	gettimeofday(&nif->cur_timestamp, NULL);

	switch (nif->if_type)
	{
		case SAND:
			if (!(sand_read(nif)))
				if (!(net_ioctl_read(nif)))
					net_read(nif);
	  	break;
		default:
			if (!(net_ioctl_read(nif)))
				net_read(nif);
		break;
	}
}

void get_all_stats(void)
{
	stat_if *nif;

	for (nif=ifaces; nif; nif = nif->next)
		get_stats(nif);
}

inline link_if(stat_if **head, stat_if **tail, stat_if *stat, stat_if *next)
{
	if (!next) // Link to the tail
	{
		if(!*tail)
		{
			*head = stat;
			*tail = stat;
			stat->prev = stat->next = NULL;
		}
		else
		{
			((stat_if *)*tail)->next = stat;
			stat->prev = *tail;
			*tail = stat;
			stat->next = NULL;
		}
	}
	else
	{
		stat->next = next;
		if (next->prev)
			next->prev->next = stat;
		else // Linking before the head node?
			*head = stat;
		stat->prev = next->prev;
		next->prev = stat;
	}
}

inline void unlink_if(stat_if **head, stat_if **tail, stat_if *stat)
{
	if (stat->prev)
		stat->prev->next = stat->next;
	else // If prev is NULL, then it had better be the head node!
		*head = stat->next;

	if (stat->next)
		stat->next->prev = stat->prev;
	else // If next is NULL, then it had better be the tail node!
		*tail = stat->prev;
}

void add_if(stat_if *stat)
{
	char *ptr;
	stat->first_load = TRUE;

	strcpy(stat->cmp_name, stat->full_name);
	if ((ptr = strchr(stat->cmp_name, '.')))
	{
		*ptr++ = '\0';
		stat->cmp_num2 = atoi(ptr);
		ptr = stat->cmp_name;
		while (*ptr && !isdigit(*ptr))
			ptr++;
		stat->cmp_num = atoi(ptr);
		*ptr = '\0';
	}
	else
	{
		ptr = stat->cmp_name;
		while (*ptr && !isdigit(*ptr))
			ptr++;
		stat->cmp_num = atoi(ptr);
		*ptr = '\0';
	}

	link_if(&ifaces, &ifaces_tail, stat, NULL); // Always add at the tail
	num_ifaces++;
}

void del_if(stat_if *stat)
{
	unlink_if(&ifaces, &ifaces_tail, stat);
	free(stat);
	num_ifaces--;
}

inline int ifaces_alpha_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int retval;

	if (ascending)
	{
		retval = strcmp(stat->cmp_name, cmp->cmp_name);
		if (!retval) // If they match, we must compare num
		{
			if (stat->cmp_num == cmp->cmp_num &&
					stat->cmp_num2 < cmp->cmp_num2)
				return TRUE;
			if (stat->cmp_num < cmp->cmp_num)
				return TRUE;
		}
		else if (retval < 0)
			return TRUE;
	}
	else
	{
		retval = strcmp(stat->cmp_name, cmp->cmp_name);
		if (!retval) // If they match, we must compare num
		{
			if (stat->cmp_num == cmp->cmp_num &&
					stat->cmp_num2 > cmp->cmp_num2)
				return TRUE;
			if (stat->cmp_num > cmp->cmp_num)
				return TRUE;
		}
		else if (retval > 0)
			return TRUE;
	}
	return FALSE;
}

inline int ifaces_description_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int retval = strcmp(stat->description, cmp->description);

	if (!retval)
		return (ifaces_alpha_sort_cmp(cmp, stat, TRUE));

	if (ascending)
		return (retval < 0);
	else
		return (retval > 0);
}

inline int ifaces_encapsulation_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int retval = strcmp(stat->encapsulation, cmp->encapsulation);

	if (!retval)
		return (ifaces_alpha_sort_cmp(cmp, stat, TRUE));

	if (ascending)
		return (retval < 0);
	else
		return (retval > 0);
}

inline int ifaces_bandwidth_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int cmp_status = cmp->bandwidth;
	int stat_status = stat->bandwidth;

	if (stat_status == cmp_status)
		return (ifaces_alpha_sort_cmp(cmp, stat, TRUE));
	if (ascending)
		return (stat_status < cmp_status);
	else
		return (stat_status > cmp_status);
}

inline int ifaces_hw_status_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int cmp_status, stat_status;

	// Must compare against TRUE/FALSE instead of the many different status values!
	cmp_status = cmp->hw_status ? TRUE : FALSE;
	stat_status = stat->hw_status ? TRUE : FALSE;

	if (stat_status == cmp_status)
		return (ifaces_alpha_sort_cmp(cmp, stat, TRUE));
	if (ascending)
		return (stat_status < cmp_status);
	else
		return (stat_status > cmp_status);
}

inline int ifaces_proto_status_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int cmp_status, stat_status;

	// Must compare against TRUE/FALSE instead of the many different status values!
	cmp_status = cmp->proto_status ? TRUE : FALSE;
	stat_status = stat->proto_status ? TRUE : FALSE;

	if (stat_status == cmp_status)
		return (ifaces_alpha_sort_cmp(cmp, stat, TRUE));
	if (ascending)
		return (stat_status < cmp_status);
	else
		return (stat_status > cmp_status);
}

inline int ifaces_inusage_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int cmp_status = percent(&(cmp->rx.byte), cmp->bandwidth, getRealInterval(cmp));
	int stat_status = percent(&(stat->rx.byte), stat->bandwidth, getRealInterval(stat));

	if (stat_status == cmp_status)
		return (ifaces_alpha_sort_cmp(cmp, stat, TRUE));
	if (ascending)
		return (stat_status < cmp_status);
	else
		return (stat_status > cmp_status);
}

inline int ifaces_outusage_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int cmp_status = percent(&(cmp->tx.byte), cmp->bandwidth, getRealInterval(cmp));
	int stat_status = percent(&(stat->tx.byte), stat->bandwidth, getRealInterval(stat));

	if (stat_status == cmp_status)
		return (ifaces_alpha_sort_cmp(cmp, stat, TRUE));
	if (ascending)
		return (stat_status < cmp_status);
	else
		return (stat_status > cmp_status);
}

inline int ifaces_inoutusage_sort_cmp(stat_if *cmp, stat_if *stat, int ascending)
{
	int cmp_status = percent(&(cmp->rx.byte), cmp->bandwidth, getRealInterval(cmp)) +
			  				percent(&(cmp->tx.byte), cmp->bandwidth, getRealInterval(cmp));
	int stat_status = percent(&(stat->rx.byte), stat->bandwidth, getRealInterval(stat)) +
							percent(&(stat->tx.byte), stat->bandwidth, getRealInterval(stat));

	if (stat_status == cmp_status)
		return (ifaces_alpha_sort_cmp(cmp, stat, TRUE));
	if (ascending)
		return (stat_status < cmp_status);
	else
		return (stat_status > cmp_status);
}

void sort_ifaces(int how, int ascending)
{
	stat_if *sorted_head, *sorted_tail;
	stat_if *stat, *next;
	int (*cmp_fn)(stat_if *, stat_if *, int) = NULL;

	switch(how)
	{
		default:
		case SORT_INTERFACE: // Straight alpha sort
			cmp_fn = ifaces_alpha_sort_cmp;
		break;
		case SORT_DESC: // Description sort
			cmp_fn = ifaces_description_sort_cmp;
		break;
		case SORT_ENCAPS: // Encapsulation sort
			cmp_fn = ifaces_encapsulation_sort_cmp;
		break;
		case SORT_BW: // Bandwidth sort
			cmp_fn = ifaces_bandwidth_sort_cmp;
		break;
		case SORT_HW: // Hardware status sort
			cmp_fn = ifaces_hw_status_sort_cmp;
		break;
		case SORT_PROTO: // Protocol status sort
			cmp_fn = ifaces_proto_status_sort_cmp;
		break;
		case SORT_INUSAGE: // Inbound usage sort
			cmp_fn = ifaces_inusage_sort_cmp;
		break;
		case SORT_OUTUSAGE: // Outbound usage sort
			cmp_fn = ifaces_outusage_sort_cmp;
		break;
		case SORT_INOUTUSAGE: // Inbound + Outbound usage sort
			cmp_fn = ifaces_inoutusage_sort_cmp;
		break;
	}

	sorted_head = sorted_tail = NULL;

	while(ifaces)
	{
		/* Remove from the unsorted list... */
		stat = ifaces;
		unlink_if(&ifaces, &ifaces_tail, stat);
		/* Find the link to insert before in the sorted list. */
		for (next = sorted_head; next; next = next->next)
		{
			if ((cmp_fn(next, stat, ascending)))
				break;
		}
		/* Do the insert into the sorted list. */
		link_if(&sorted_head, &sorted_tail, stat, next);
	}
	ifaces = sorted_head;
	ifaces_tail = sorted_tail;
}
stat_if *find_iface_name(char *ifname)
{
	stat_if *stat;

	for (stat = ifaces; stat; stat = stat->next)
	{
		if (!(strcmp(stat->if_name, ifname)))
			return stat;
	}
	return NULL;
}
