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

#define SIOCGSANDIFACESTATS             SIOCDEVPRIVATE + 5
