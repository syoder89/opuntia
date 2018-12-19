/* vi:set ts=3: */

/********************************************************************/
/* This code protected by the terms of the GPL Version 2            */
/* See the COPYING file for more information                        */
/********************************************************************/
#include<linux/poll.h>
#include<linux/types.h>

#ifndef SAND_DEFS_H
#define SAND_DEFS_H

#define SAND_MAJOR 32
#define SAND_MAX_MAJORS 8
#define SAND_MTU 1500
#define SAND_MAJOR 32
#define MAXBUSES 12
#define MAXSLOTS 32
#define MAXIRQS 64
/* Flag + Address + Control + 2 octects for CRC = 5 */
#define HDLC_LEN 5

#define MAX_RX_QUEUE_LEN 4096

#define DEV_NAMELEN strlen(wandev_name) + 12

#define PTYPE_MASTER		0
#define PTYPE_HW_SUB 	1
#define PTYPE_PROTO_SUB 2

#define LBTYPE_NONE 0
#define LBTYPE_LOCAL 1
#define LBTYPE_REMOTE 2

#define SAND_IGNORE_DCD 1

#define SAND_GFRA_MULTIPLIER 1000
/* User chains */
#define UC_ARGSIZE 256

#ifndef TRUE
   #define TRUE 1
   #define FALSE 0
#endif

#if defined(__use_altstack__)
#define INETICS_API_CALL __attribute__((regparm(0),altstack(false)))
#else
#define INETICS_API_CALL __attribute__((regparm(0)))
#endif

/***********************************************************************/
/*                            User Space Structures                   */
/***********************************************************************/

typedef struct sand_port_param
{
	int port;
	int hw_sub_index;
	int proto_sub_index;
	int protocol;
	int debug;
	int shutdown;
	char description[50];
	unsigned char irq;
	unsigned char pad1;
	int bandwidth;
// Why does egcs think dev_t is 64 bits in user space but 32 bits in kernel space?
	int major, minor;
//	dev_t major, minor;
	unsigned long flags;
	int rx_bwlimit, tx_bwlimit, combined_bwlimit;
	int rx_bwlimit_latency, tx_bwlimit_latency, combined_bwlimit_latency;
	int rx_bwlimit_buffer, tx_bwlimit_buffer, combined_bwlimit_buffer;
	int proto_debug;
	int hw_debug;
	int hw_type; /* ARPHRD_xxx */
	char rx_link_port[IFNAMSIZ];
} SAND_PORT_PARAM;

#define SAND_CSU_REQUEST_TYPE_ALL 0
#define SAND_CSU_REQUEST_TYPE_INTERVAL 1
#define SAND_CSU_REQUEST_TYPE_GENERAL 2

typedef struct
{
	unsigned short es;	/* Errored Seconds */
	unsigned short bes;	/* Bursty Errored Seconds */
	unsigned short ses;	/* Severely Errored Seconds */
	unsigned short sefs;	/* Severely Errored Framing Seconds */
	unsigned short uas;	/*	Unavailable Seconds */
	unsigned short css;	/* Controlled Slip Seconds */
	unsigned short dms;	/* Degraded Minutes */
	unsigned short pcv;	/* Path Code Violations */
	unsigned short les;	/* Line Errored Seconds */
	unsigned short lcv;	/* Line Code Violations */
} SAND_CSU_INTERVAL_STATS;

typedef struct
{
	unsigned char firmware_major;		/* Major/minor version numbers for the CSU
													firmware. 1.02 would be major 1,
													minor 2 */
	unsigned char firmware_minor;
	unsigned char csu_failure;			/* If the CSU has encountered a failure.
													1=ROM checksum failure
													2=EEPROM checksum failure
													16=Framer type is incorrect
													32=Framer hardware failure
													64=Framer internal loopback failure
													128=CSU is in OFFLINE mode */
	unsigned char loopback_configuration;	/* Loopback state for the near end:
															0=Loopback requests will be honored
																but not currently looped back.
															1=Loopback requests will be honored
																and in payload loopback.
															2=Loopback requests will be honored
																and in line loopback.
															3=Loopback requests will not be
																honored */
												/* Alarm definitions:
														1=RAI (remote alarm indication aka yellow alarm)
														2=AIS (alarm indication signal aka blue alarm)
														4=PDE (pulse density error)
														8=LOS (loss of signal)
														16=OOF (out of frame)
														32=B8ZS (receive B8ZS detected)
												*/
	unsigned char rx_line_status;		/* Alarms (RAI/AIS/PDE/LOS/OOF/B8ZS) */
	unsigned char tx_line_status;		/* Alarms (RAI/AIS/PDE/LOS) */
	unsigned char fe_status;			/* Far end status 0=normal, 1=PRM
													(performance report monitoring failure),
													2=Payload loopback */
	unsigned char pad;					/* Longword alignment */
} SAND_CSU_INFO;

#if 0
/* Character device ioctl SIOCGETCSUSTATS takes the following structure as its
	argument.
	Caller must fill in framer, request_type and interval if request_type == 1.
*/

Example:

SAND_CSU_PERFORMANCE_STATS csu_stats;
	
	memset(csu_stats, 0, sizeof(SAND_CSU_PERFORMANCE_STATS));
	csu_stats.framer = 0; /* Our csu's stats */
//	csu_stats.framer = 1; /* Far end csu's stats */
	csu_stats.request_type = 0; /* Give me everything */
//	csu_stats.request_type = 1; /* Give me info on a specific interval */
//	csu_stats.request_type = 2; /* Give me generic csu info only */
//	csu_stats.interval = 0; /* Give me info on the current 15 minute interval only */
//	csu_stats.interval = 1; /* Give me info on the previous 15 minute interval only */
//	csu_stats.interval = 4; /* Give me info on the 15 minute interval an hour ago only */

	devfd = open("/dev/wanicXXX", O_RDONLY);
	if (devfd)
	{
		retval = ioctl(devfd, SIOCGETCSUSTATS, &csu_stats);
		if (retval)
			// failed!
		else
		{
			/* This is just an example of looking at errored seconds... */
			printf("Current 15 minute interval\n");
			printf("%d seconds have elapsed in this interval\n", csu_stats.time_elapsed);
			printf("Errored seconds: %d\n", csu_stats.current[i].es);
			...
			/* Loop through all valid intervals */
			for (i=0;i<csu_stats.valid_intervals;i++)
			{
				printf("15 minute interval starting %d minutes ago\n", (i+1)*15);
				/* This is just an example... */
				printf("Errored seconds: %d\n", csu_stats.previous[i].es);
			}
		}
		close(devfd);
	}
#endif

typedef struct
{
	/* Input fields */
	unsigned char framer;				/* 0=near end framer (local csu statistics),
													1=far end framer */
	unsigned char request_type;		/* 0=request info for all intervals and
													csu_info,
													1=request info for a single interval (set
													interval field to the interval you want
													to retrieve.
													2=request csu_info only */
	unsigned char interval;				/* Which 15 minute interval. 0-96 (0=current,
													1=previous interval, etc) */
	/* Output fields */
	unsigned char valid_intervals;	/* Number of valid intervals in the current
													24 hour period (0-96). Use this field to
													determine which indicies in the
													previous[] array below are valid. 0 means
													only the current interval is valid, 3 means
													that previous[0] through previous[2] are
													valid */
	unsigned short time_elapsed;		/* Number of seconds elapsed in current 15
													minute interval (0-899) */
	unsigned char pad1[2];				/* Longword alignment */

	SAND_CSU_INFO csu_info;				/* Not filled in for request_type == 1 */
	SAND_CSU_INTERVAL_STATS current_interval;	/* request_type == 0 (all intervals):
														Stats for current 15 minute interval
													request_type == 1 (specific interval):
														Stats for the interval requested */
	SAND_CSU_INTERVAL_STATS previous[96];	/* Stats for previous 24 hours worth
															of 15 minute intervals only if 
															request_type == 0 */
} SAND_CSU_PERFORMANCE_STATS;

typedef struct sand_list{
	struct sand_list *next, *prev, *list;
	int num_entries;
	char priority;
	char pad[3];
} SAND_LIST;

#ifdef __KERNEL__

enum SAND_TX_CHAIN_PRIORITIES
{
	TX_FROMSYS_1 = 10,
	TX_FROMSYS_2,
	TX_FROMSYS_3,
	TX_FROMSYS_4,
	TX_FROMSYS_5,
	TX_DDP_1 = 20,
	TX_DDP_2,
	TX_DDP_3,
	TX_DDP_4,
	TX_DDP_5,
	TX_DLP_1 = 30,
	TX_DLP_2,
	TX_DLP_3,
	TX_DLP_4,
	TX_DLP_5,
	TX_EDP_1 = 40,
	TX_EDP_2,
	TX_EDP_3,
	TX_EDP_4,
	TX_EDP_5,
	TX_HIC_1 = 50,
	TX_HIC_2,
	TX_HIC_3,
	TX_HIC_4,
	TX_HIC_5,
	TX_PIC_1 = 60,
	TX_PIC_2,
	TX_PIC_3,
	TX_PIC_4,
	TX_PIC_5,
};

enum SAND_RX_CHAIN_PRIORITIES
{
	RX_TOSYS_1 = 60,
	RX_TOSYS_2,
	RX_TOSYS_3,
	RX_TOSYS_4,
	RX_TOSYS_5,
	RX_DDP_1 = 50,
	RX_DDP_2,
	RX_DDP_3,
	RX_DDP_4,
	RX_DDP_5,
	RX_DLP_1 = 40,
	RX_DLP_2,
	RX_DLP_3,
	RX_DLP_4,
	RX_DLP_5,
	RX_EDP_1 = 30,
	RX_EDP_2,
	RX_EDP_3,
	RX_EDP_4,
	RX_EDP_5,
	RX_HIC_1 = 20,
	RX_HIC_2,
	RX_HIC_3,
	RX_HIC_4,
	RX_HIC_5,
	RX_PIC_1 = 10,
	RX_PIC_2,
	RX_PIC_3,
	RX_PIC_4,
	RX_PIC_5,
};


#define TX_FROMSYS_DEFAULT TX_FROMSYS_3
#define TX_DDP_DEFAULT TX_DDP_3
#define TX_DLP_DEFAULT TX_DLP_3
#define TX_EDP_DEFAULT TX_EDP_3
#define TX_HIC_DEFAULT TX_HIC_3
#define TX_PIC_DEFAULT TX_PIC_3

#define RX_TOSYS_DEFAULT RX_TOSYS_3
#define RX_DDP_DEFAULT RX_DDP_3
#define RX_DLP_DEFAULT RX_DLP_3
#define RX_EDP_DEFAULT RX_EDP_3
#define RX_HIC_DEFAULT RX_HIC_3
#define RX_PIC_DEFAULT RX_PIC_3

#define SAND_CHAIN_TX 0
#define SAND_CHAIN_RX 1

typedef struct {
	struct sk_buff_head queue;
   wait_queue_head_t wqueue;
   struct task_struct *tsk;
	struct semaphore *sem;
	char id;
	char exit;
	char pad[2];
} SAND_RX_THREAD;

extern SAND_RX_THREAD *sand_rx_threads[NR_CPUS];

typedef struct {
	rwlock_t lock;
//	unsigned long flags;
} SAND_LOCK;

typedef struct sand_chain {
	SAND_LIST	execute_list;
	int			(*function)(struct sand_chain *, void **data);
	void			*data;
	SAND_LOCK	lock;
	struct sand_port *sand_port;
	SAND_LIST	list;
	int			(*init)(struct sand_chain *);
	int			(*cleanup)(struct sand_chain *);
	int			(*config)(struct sand_chain *, char *arguments);
	int			type; /* SAND_CHAIN_TX | SAND_CHAIN_RX */
} SAND_CHAIN;

typedef struct sand_user_chain {
	SAND_LIST	list;
	char			identifier[16];
	int			(*function)(struct sand_chain *, void **data);
	int			(*init)(struct sand_chain *);
	int			(*cleanup)(struct sand_chain *);
	int			(*config)(struct sand_chain *, char *arguments);
	int			use_count;
} SAND_USER_CHAIN;

/***********************************************************************/
/*                            Card & Port Structures                   */
/***********************************************************************/

typedef struct sand_card
{
	int debug_level;
	void *hw_priv;
	void *proto_priv;
	struct sand_port *ports;
	struct sand_card *Next;  // Points to next card with same irq
	struct sand_card *next_sand_card;  // For our linked list of all sand cards
	struct net_device *irq_dev;
	int num_ports;
	char *model;
	char *model_verbose;
	int card_id;
	int vendor_id, dev_id;
	int bus, slot;
	int base_address;
	int interrupt;
	unsigned char irq;
} SAND_CARD;

/* Used in the proto_driver list */
struct sand_port_proto_driver {
	SAND_LIST list;
	struct sand_proto_driver *proto_driver;
};

struct sand_sched {
	spinlock_t sched_lock;
	int bytes_queued;
//	int bytes_staged;
//	unsigned int bwlimit;
//	unsigned int MaxBurstSize;
//	struct sk_buff_head staging_queue;		/* Tx hic chain places frames here */
	struct sk_buff_head ready_queue;			/* Rate-limiting scheduler moves frames from staging to here */
//	struct timer_list bwlimit_timer;			/* Timer for scheduler */
//	int bwlimit_timer_running;
//	long bucket;									/* Rate-limit leaky bucket value */
//	unsigned long I, L, dt;						/* GFRA values for I, L, dt */
//	unsigned long max_stage_len;				/* Max number of bytes to stage = I / 1000 */
	unsigned long max_ready_len;				/* Max number of bytes to allow in ready queue = L / 1000 */
	int pending;									/* How many frames do we have on the master's queue? */
};

typedef struct sand_port
{
	struct net_device *dev;
	void *hw_priv;
	void *proto_priv;
	int hw_initialized;
	unsigned char hw_status;
	unsigned char proto_status;
	unsigned char debug;
	unsigned char hw_debug;
	unsigned char proto_debug;
	unsigned char port_type;  /* Bit mask: master=0, hw_sub=1, proto_sub=2
										  (Serial0:1 = 1, Serial0.1 = 2, Serial0:1.1 = 3) */
	unsigned char dcd;
	unsigned char rts;
	struct sand_hw_driver *hw_driver;
	struct sand_proto_driver *proto_driver;
	struct sand_card *card;
	struct sand_port *Next; // Points to next port on card
	struct sand_port *next_sand_port;  // For our linked list of all sand ports
	struct sand_port *master; /* For all hardware and protocol subs points to the master
											sand_port structure. */
	SAND_RX_THREAD *last_rx_thread;
	int rx_execute_direct; /* Do not use an Rx tasklet, execute the chain directly */
	SAND_LIST rx_chain_execute;
	SAND_LIST tx_chain_execute;
	SAND_LIST hw_status_execute;
	SAND_LIST proto_status_execute;
	unsigned long rx_packets_queued;
	unsigned long rx_bwlimit;
	unsigned long combined_bwlimit;
	struct sand_port *rx_link_port;
	struct net_device_stats stats;
	unsigned long last_rx;
	unsigned long last_tx;
	struct sand_port *hw_sub;
	struct sand_port *proto_sub;
	struct sand_port *bonder_master;
	SAND_LIST tx_busy_list;
//	struct sand_port *tx_busy_cur; /* Let sandcore know which port is currently being transmitted
//												 by the tasklet. Only this port will be allowed to transmit
//												 if there's an active list. */
//	spinlock_t tx_busy_lock;
//	struct tasklet_struct tx_sched_tasklet;
	spinlock_t _xmit_lock;
	int xmit_lock_owner;
	struct sk_buff_head tx_control_queue;
	SAND_LIST hw_status_change;
	SAND_LIST proto_status_change;
	SAND_LIST tx_ddp;
	SAND_LIST tx_dlp;
	SAND_LIST tx_edp;
	SAND_LIST tx_hic;
	SAND_LIST tx_pic;
	SAND_LIST rx_edp;
	SAND_LIST rx_dlp;
	SAND_LIST rx_ddp;
	SAND_LIST rx_hic;
	SAND_LIST rx_pic;
	SAND_LIST ioctl;
	SAND_LIST proc;
	SAND_LIST to_system;
	SAND_LIST from_system;
	void (*install_default_chains)(struct sand_port *sand_port);
	SAND_LIST route_update; /* Used by sand_remove/sand_add_routes() */
	struct sk_buff_head rx_bwlimit_queue;
	struct sk_buff_head tx_bwlimit_queue;
	struct sk_buff_head combined_bwlimit_queue;
	/* limit in bits per second */
	unsigned long tx_bwlimit;
	unsigned long rx_bwlimit_jiffies;
	unsigned long tx_bwlimit_jiffies;
	unsigned long combined_bwlimit_jiffies;
	unsigned long rx_jiffies;
	unsigned long tx_jiffies;
	unsigned long combined_jiffies;
	spinlock_t rx_bwlimit_lock;
	spinlock_t tx_bwlimit_lock;
	spinlock_t combined_bwlimit_lock;
	unsigned long rx_bytes_queued;
	unsigned long tx_bytes_queued;
	unsigned long rx_max_bytes_queued;
	unsigned long tx_max_bytes_queued;
	unsigned long rx_bytes_dequeued;
	unsigned long tx_bytes_dequeued;
	unsigned long rx_bytes_per_jiffie;
	unsigned long tx_bytes_per_jiffie;
	unsigned long rx_bytes_accum;
	unsigned long tx_bytes_accum;
	unsigned long tx_bytes_this_interval;
	struct timer_list rx_bwlimit_timer;
	struct timer_list tx_bwlimit_timer;
	int rx_bwlimit_timer_running;
	int tx_bwlimit_timer_running;
	struct sand_sched tx_sched, rx_sched;
#define SAND_NUM_TX_QUEUES 2
	struct sk_buff_head tx_queue[SAND_NUM_TX_QUEUES];			/* Main transmit queue used by protocol subs. */
	struct tasklet_struct tx_sched_tasklet; /* Tx busy tasklet has become this tasklet - pull vs push */
	int bw, default_bw;
	int port_number;
	int hw_sub_index;		// Base 1: ex: for interface Serial0:1 hw_sub_index is 1.
	int proto_sub_index; // Base 1: ex: for interface Serial0.1 proto_sub_index is 1.
								// for interface Serial0:1.1 hw_sub_index is 1, proto_sub_index is 1.
	unsigned long queue_bytes;

	/* Notes for hardware and protocol sub interface linked lists:

		- Hardware sub interfaces are used for multiplexing. An example is to take
			a DS3 on a WANic 850 card and split it into 28 T1 channels. You can
			reference the first T1 using interface Serial0:1. The second T1 would be
			Serial0:2, etc.
		- Protocol sub interfaces are generally used with the frame relay protocol.
			Configure a sub interface for each connection to the frame cloud. A PVC
			will be established for each sub interface. Each sub interface must have
			a unique DLCI. Statistics will be kept for each sub interface. The master
			interface will sum statistics from all its protocol sub interfaces.

		- Master port hw_sub pointer will point to head of linked list of
			hardware sub interfaces or NULL if no subs.
		- Master port proto_sub pointer will point to head of linked list of
			protocol sub interfaces or NULL if no subs.
		- Hardware sub interface hw_sub pointer will point to next hardware
			sub interface or NULL if no more subs.
		- Hardware sub interface proto_sub pointer will point to head of
			linked list of protocol sub interfaces or NULL if no subs.
		- Protocol sub interface hw_sub pointer will point to the master hardware
			sub interface or NULL if no master hardware sub interface exists. In this
			case the sand_port *master is the master interface.
		- Protocol sub interface proto_sub pointer will point to the next protocol
			sub interface or NULL if no more subs.
	*/

	dev_t major, minor;
//	char *model;
	rwlock_t port_lock;
//	unsigned long lock_flags;
	unsigned long dcd_transitions;
	unsigned long proto_up;
	struct timer_list dcd_timer;
	unsigned char cts;
	unsigned char dsr;
	unsigned char dtr;
	char loopback;
	unsigned char routes_removed;
	unsigned char is_mmaped;
	unsigned char pad[2];
	unsigned long mmap_paddr;		/* Used by the mmap char callback */
	unsigned long mmap_len;			/* Used by the mmap char callback */
	struct vm_area_struct *vma;	/* Retain a pointer to the vma for the mmap char callback */
        char description[50];
        char used_hw_id;
	void *ppp_proto_priv;
	SAND_LIST proto_driver_list; /* Now able to add multiple protocols like PPPoATM. This list is
										   * used for net/char open/close/ioctl routines */

#define SOFTATM_F_HW_ALL		1	/* Hardware performs all ATM processing (AAL5/PVC/QOS) */
#define SOFTATM_F_HW_AAL5		2	/* Hardware performs AAL5 processing but no PVC/QOS (implies cell
											   delineation) */
#define SOFTATM_F_HW_CELL		4	/* Hardare performs cell delineation */
	unsigned long softatm_features;
} SAND_PORT;

struct sand_iface_stats
{
	char proto_name[32];
	char description[50];
	unsigned char hw_status;
	unsigned char proto_status;
	unsigned long long bw;
	unsigned long long rx_bytes;
	unsigned long long rx_packets;
	unsigned long long rx_errors;
	unsigned long long rx_dropped;
	unsigned long long rx_fifo_errors;
	unsigned long long rx_crc_errors;
	unsigned long long rx_frame_errors;
	unsigned long long tx_bytes;
	unsigned long long tx_packets;
	unsigned long long tx_errors;
	unsigned long long tx_dropped;
	unsigned long long tx_fifo_errors;
	unsigned long long tx_carrier_errors;
	unsigned long dcd_transitions;
	unsigned long last_rx;
	unsigned long last_tx;
	unsigned long proto_up;
	unsigned char dcd;
	unsigned char dtr;
	unsigned char dsr;
	unsigned char rts;
	unsigned char cts;
	unsigned char pad[3];
};

/* skb cb struct used by the tx bwlimit code */
struct sand_bwlimit_cb {
	unsigned long cookie; /* Sanity check */
	int pass_through; /* Ok to tx */
};
#define SAND_BWLIMIT_COOKIE 0xBA2DB9EF

/***********************************************************************/
/*                            Hardware Driver                          */
/***********************************************************************/


typedef struct sand_hw_driver
{
	int id;
	int debug_level;

/* Callbacks */
	int (*hw_xmit)(SAND_PORT *sand_port, struct sk_buff *skb);
	int (*hw_init)(struct net_device *dev);
	int (*hw_pci_probe)(int bus, int slot, void **priv);
        int (*hw_isa_probe)(unsigned int io, void **priv);
        int (*hw_usb_probe)(void);
	int (*hw_shutdown_port)(SAND_PORT *);
	int (*hw_shutdown_card)(SAND_CARD *);
	int (*hw_enable_port)(SAND_PORT *);
	int (*hw_disable_port)(SAND_PORT *);
	int (*hw_get_port_params)(SAND_PORT *, unsigned long);
	int (*hw_display_port_params)(SAND_PORT *, char *buffer, char *indent);
	int (*hw_set_port_params)(SAND_PORT *, unsigned long);
	int (*hw_get_port_stats)(SAND_PORT *, unsigned long);
	int (*hw_reset_port)(SAND_PORT *);
	int (*hw_init_proto_sub)(SAND_PORT *, void *);
	int (*hw_shutdown_proto_sub)(SAND_PORT *);
	int (*hw_init_vp_pvc)(SAND_PORT *, void *);
	int (*hw_shutdown_vp_pvc)(SAND_PORT *, void *);
	int (*hw_get_csu_stats)(SAND_PORT *, SAND_CSU_PERFORMANCE_STATS *);
	int (*hw_configure_transparent_mode)(SAND_PORT *, int on);
	int (*hw_configure_ring)(SAND_PORT *, int ntx_bufs, int nrx_bufs, int tx_buflen, int rx_buflen);
	int (*hw_start_ring)(SAND_PORT *);
	int (*hw_stop_ring)(SAND_PORT *);
	int (*hw_reset_ring)(SAND_PORT *);
	int (*hw_free_ring)(SAND_PORT *);
	int (*hw_tx_ring_allocate_idx)(SAND_PORT *, int idx, struct sk_buff *);
        int (*hw_tx_ring_release_idx)(SAND_PORT *, int idx);
        void (*hw_adjust_last_sched_tx)(SAND_PORT *, int offset);
        int (*hw_net_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
        int (*hw_get_serialnumber)(SAND_PORT *, char *serial, int serial_len, char *busid, int busid_len, unsigned int *port);
	struct sand_hw_driver *Next;
} SAND_HW_DRIVER;

/***********************************************************************/
/*                            Hardware Driver Port Info                */
/***********************************************************************/


typedef struct sand_hardware_id {
    struct sand_hardware_id *next;
    unsigned char intf_name[25];
    unsigned int port_number;
    unsigned char port_id[25];
    unsigned int port;
    unsigned short major;
    unsigned int minor;
    unsigned int used;
} SAND_HW_ID;

/***********************************************************************/
/*                            Protocol Driver                          */
/***********************************************************************/


typedef struct sand_proto_driver
{
	int id;
	int debug_level;

/* Callbacks */
	int (*proto_header)(struct sk_buff * skb, struct net_device *dev,
               unsigned short type, void *daddr,
               void *saddr, unsigned len );
	int (*proto_encapsulate)(struct sk_buff *skb);
	int (*proto_decapsulate)(struct sk_buff *skb);
	int (*proto_rx_callback)(struct sand_port *Port, struct sk_buff *skb);
	void (*proto_tx_callback)(struct sand_port *Port);
	struct enet_statistics * (*proto_stats)(struct net_device *dev);
	int (*proto_net_open)(struct net_device *dev);
	int (*proto_char_open)(struct inode *inode, struct file *filp);
	int (*proto_net_close)(struct net_device *dev);
	int (*proto_char_close)(struct inode *inode, struct file *filp);
	int (*proto_net_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
	int (*proto_char_ioctl)(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
	int (*proto_char_write)(struct file *filp, const char *buf, size_t len, loff_t *ppos);
	int (*proto_char_read)(struct file *filp, const char *buf, size_t len, loff_t *ppos);
	unsigned int (*proto_char_select)(struct file *filp, poll_table *wait);
	int (*proto_char_async)(int fd, struct file *filp, int on);
	int (*proto_init)(struct net_device *dev);
	int (*proto_cleanup)(struct net_device *dev);
	int (*proto_get_port_params)(SAND_PORT *, unsigned long);
	int (*proto_set_port_params)(SAND_PORT *, unsigned long);
	int (*proto_get_port_stats)(SAND_PORT *, unsigned long);
	void (*proto_dcd_change)(SAND_PORT *, unsigned long);
	int (*proto_display_port_params)(SAND_PORT *, char *buffer, char *indent);
	struct sand_proto_driver *Next;
        int checksum;
} SAND_PROTO_DRIVER;

/***********************************************************************/
/*                        Decoded Data Processor Module                */
/***********************************************************************/


typedef struct sand_ddp_module
{
	int id;
	int debug_level;

/* Chains */

	SAND_CHAIN *hw_status_change;
	SAND_CHAIN *proto_status_change;
	SAND_CHAIN *rx;
	SAND_CHAIN *tx;
	SAND_CHAIN *ioctl;
	SAND_CHAIN *proc;
	SAND_CHAIN *to_system;
	SAND_CHAIN *from_system;

/* Callbacks */
	int (*ddp_net_ioctl)(struct net_device *dev, struct ifreq *ifr, int cmd);
	int (*ddp_char_ioctl)(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);

	int (*ddp_init)(struct net_device *dev);
	int (*ddp_cleanup)(struct net_device *dev);
	struct sand_ddp_module *Next;
} SAND_DDP_MODULE;


/***********************************************************************/
/*                            Safety Functions                         */
/***********************************************************************/


extern void * VERIFY_POINTER(void *ptr);

extern int VERIFY_RANGE(int val, int min, int max);


/***********************************************************************/
/*                            Linked List Functions                    */
/***********************************************************************/

#define LOCK_SAND_PORT_W(Port)
#define LOCK_SAND_PORT_R(Port)
#define UNLOCK_SAND_PORT_W(Port)
#define UNLOCK_SAND_PORT_R(Port)
#define sand_lock_r(lock)
#define sand_lock_w(lock)
#define sand_unlock_r(lock)
#define sand_unlock_w(lock)
//extern __inline__ void sand_lock_r(SAND_LOCK *lock);
//extern __inline__ void sand_unlock_r(SAND_LOCK *lock);
//extern __inline__ void sand_lock_w(SAND_LOCK *lock);
//extern __inline__ void sand_unlock_w(SAND_LOCK *lock);


#define SAND_INSERT_HEAD(a, b) \
        b->Next = a; a = b
#define SAND_REMOVE_HEAD(a, b) \
        a = b->Next
#define SAND_REMOVE_NODE(a,b,c) \
        for (b = a; b && b->Next && b->Next->id != c; b = b->Next); \
        if (b && b->Next && b->Next->id == c) { \
                b->Next = b->Next->Next; \
        }

#define SAND_HW_UP(x) \
   ((x == HW_STATUS_UP) || (x == HW_STATUS_IGNORE) || (x == HW_STATUS_INIT))

/***********************************************************************/
/*                            Debug Functions                          */
/***********************************************************************/


//extern void SAND_DEBUG(int message_level, int system_level, char *format, ...);
//extern void SAND_DISPLAY_PORT(int message_level, int system_level, char *format, ...);
//extern void PROTO_DEBUG(SAND_PORT *sand_port, int message_level, char *format, ...);
//extern void HW_DEBUG(SAND_PORT *sand_port, int message_level, char *format, ...);

#define ENTER(system_level, message) \
	SAND_DEBUG(9, system_level, "Entering %s.\n", message);

#define LEAVE(system_level, message) \
	SAND_DEBUG(9, system_level, "Leaving %s.\n", message);

/***********************************************************************/
/*                            External Shared Variables                */
/***********************************************************************/


extern int sand_transmit_timeout;
extern char wandev_name[IFNAMSIZ];
extern int softatm_loaded;

#endif /* __KERNEL__ */
enum SAND_CHAINS
{
	TX_FROMSYS = 10,
	TX_DDP,
	TX_DLP,
	TX_EDP,
	TX_HIC,
	TX_PIC,
	RX_TOSYS = 20,
	RX_DDP,
	RX_DLP,
	RX_EDP,
	RX_HIC,
	RX_PIC
};
typedef struct sand_net_ioctl
{
	int cmd;
	void *data[16]; /* 16 general use pointers! Very useful! */
} SAND_NET_IOCTL;

struct sand_net_majmin
{
	int major;
        int minor;
};

#define SAND_COPYRIGHT "Copyright (C)1999-2008 ImageStream Internet Solutions, Inc. All rights reserved worldwide."
#endif /* SAND_DEFS_H */
