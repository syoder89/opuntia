/******************************************************************************/
/* (C)1999 Imagestream Internet Solutions, Inc. All rights reserved worldwide.*/
/******************************************************************************/
/* This code Liscensed under the GPL Version 2. See COPYING for details. */

#ifndef STAT_IF_H
#define STAT_IF_H

#include <sys/time.h>
#ifndef u64
#include <linux/types.h>
typedef unsigned long long u64;         /* hack, so we may include kernel's ethtool.h */
#endif
#ifndef u32
typedef __uint32_t u32;         /* ditto */
#endif
#ifndef u16
typedef __uint16_t u16;         /* ditto */
#endif
#ifndef u8
typedef __uint8_t u8;           /* ditto */
#endif
#include <linux/ethtool.h>
#include <linux/sockios.h>

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#define STAT_R  (1<<0)
#define STAT_S  (1<<1)
#define STAT_BP (1<<2)
#define STAT_RT (1<<3)
#define STAT_IF (1<<4)

typedef struct stat_root
{
	int type;
} stat_r;
stat_r *stat_r_new();
void    stat_r_construct(stat_r *stat);
void    stat_r_destruct(stat_r *stat);

typedef struct stat_single
{
	stat_r parent;
	unsigned long current;
	unsigned long zero;
} stat_s;
stat_s       *stat_s_new();
void          stat_s_construct(stat_s *stat);
void          stat_s_destruct(stat_s *stat);
void          stat_s_set(stat_s *stat, unsigned long set);
void          stat_s_add(stat_s *stat, unsigned long add);
void          stat_s_zero(stat_s *stat);
unsigned long stat_s_show(stat_s *stat);

typedef struct stat_byte_packet
{
	stat_s parent;
	unsigned long previous;
	unsigned int  rolled;
} stat_bp;
stat_bp       *stat_bp_new();
void          stat_bp_construct(stat_bp *stat);
void          stat_bp_destruct(stat_bp *stat);
void          stat_bp_set(stat_bp *stat, unsigned long set);
unsigned long stat_bp_delta(stat_bp *stat);


typedef struct rx_tx_stat
{
	stat_r parent;
	stat_bp byte;
	stat_bp packet;
	stat_s error;
	stat_s drop;
	stat_s fifo;
	stat_s compressed;
	unsigned long last;
} stat_rt;

typedef struct
{
   unsigned short mii_val[32];
} stat_eth_mii;

#include <sys/types.h>
#include <signal.h>
typedef struct
{
   char username[64];
   pid_t pppd_pid;
   char master_port[32];
} stat_ppp;

stat_rt *stat_rt_new();
void    stat_rt_construct(stat_rt *stat);
void    stat_rt_destruct(stat_rt *stat);

#define HW_STAT   0
#define PR_STAT   1
#define DCD       2
#define DSR       3
#define DTR       4
#define RTS       5
#define CTS       6
#define RX_BYTE   7
#define RX_PACK   8
#define RX_ERROR  9
#define RX_DROP  10
#define RX_FIFO  11
#define RX_FRAME 12
#define RX_COMP  13
#define RX_MULTI 14
#define RX_CRC   15
#define TX_BYTE  16
#define TX_PACK  17
#define TX_ERROR 18
#define TX_DROP  19
#define TX_FIFO  20
#define TX_COLL  21
#define TX_CARR  22
#define TX_COMP  23

typedef struct interface_stat
{
	stat_r parent;
/*
 * General Information
 */
	struct timeval last_timestamp, cur_timestamp;
	char if_name[32];
	char full_name[32];
	char cmp_name[32];
	int  cmp_num;
	int  cmp_num2;
	char description[128];
	char encapsulation[32];
	unsigned long bandwidth;
	unsigned long rx_bandwidth;
	stat_eth_mii eth_mii;
	int has_ethtool;		/* Do we have ethtool info? - don't use mii then */
	struct ethtool_cmd ecmd;	/* Stores device settings */
	struct ethtool_value edata;	/* Stores the link state info */
	stat_s up_timestamp;
	char duplex; // 0 = full, 1 = half
	char if_type;
	char first_load;
	char pad;
	int proto_sub;
	int hw_sub;
	int vlan_id;
	unsigned int if_index;

/*
 * Receive and Transmit information
 */
	stat_rt rx;
	stat_s  rx_frame;
	stat_s  rx_crc;
	stat_s  rx_multicast;
	stat_rt tx;
	stat_s  tx_carrier;
	stat_s  tx_collisions;

/*
 * DCD up/down
 */
	stat_s dcd_transitions;
	short dcd;
	short rts;
	short cts;
	short dsr;
	short dtr;
	short hw_status;
	short proto_status;
	short pad2;
	stat_ppp ppp; /* PPP statistics */
	int delete;
	struct interface_stat *prev, *next;
} stat_if;
stat_if *stat_if_new();
void    stat_if_construct(stat_if *stat);
void    stat_if_destruct(stat_if *stat);
void    stat_if_name(stat_if *stat, const char *name, const char *f_name);
void    stat_if_set(stat_if *stat, unsigned long vars[]);
extern stat_if *detail_nif;
#endif
