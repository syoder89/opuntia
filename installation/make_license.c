#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <time.h>

#define MAX_MAC_ADDRS 10
#define JSON_OFFSET 256

unsigned char base_mac[6] = { 0xb0, 0x91, 0x37, 0x05, 0x00, 0x08};
char *serno = "24803029";
char *model = "AP3500";
char *name = "AP3500";
char *revision = "1";
int mac_addrs_to_allocate = 2;
FILE *fp;

struct header {
	uint32_t magic;
	char serno[12];
	struct {
		uint8_t mac_addr[6];
		uint16_t csum;
	} mac_addrs[MAX_MAC_ADDRS];
	char model[16];
	char revision[4];
} hdr;

void write_header(void) {
int i, j;
	memset(&hdr, 0xff, sizeof(hdr));
	hdr.magic = htonl(0x49536973);
	strcpy(hdr.serno, serno);
	for (i=0;i<mac_addrs_to_allocate;i++) {
		memcpy(hdr.mac_addrs[i].mac_addr, base_mac, 6);
		hdr.mac_addrs[i].csum = 0;
		j=5;
		while (j >= 0) {
			base_mac[j]++;
			if (base_mac[j] != 0)
				break;	
			j--;
		}
	}
	strcpy(hdr.model, model);
	strcpy(hdr.revision, revision);
	fwrite(&hdr, sizeof(hdr), 1, fp);
}

void write_json(void) {
int i;
uint8_t buf = 0xff;
time_t t = time(NULL);
struct tm *tm = localtime(&t);
char build_date[64] = { 0, };

	strftime(build_date, sizeof(build_date), "%F %T", tm);
	for (i=0;i<(JSON_OFFSET-sizeof(hdr)); i++)
		fwrite(&buf, 1, 1, fp);
	fprintf(fp, "{");
	fprintf(fp, "\"mac_addrs\": [");
	for (i=0;i<mac_addrs_to_allocate;i++) {
		if (i > 0)
			fprintf(fp, ", ");
		fprintf(fp, "\"%x:%x:%x:%x:%x:%x\"", hdr.mac_addrs[i].mac_addr[0], 
			hdr.mac_addrs[i].mac_addr[1], hdr.mac_addrs[i].mac_addr[2], 
			hdr.mac_addrs[i].mac_addr[3], hdr.mac_addrs[i].mac_addr[4], 
			hdr.mac_addrs[i].mac_addr[5]);
	}
	fprintf(fp, "], ");
	fprintf(fp, "\"serial_number\": \"%s\", ", serno);
	fprintf(fp, "\"build_date\": \"%s\", ", build_date);
	fprintf(fp, "\"product_id\": \"%s\", ", model);
	fprintf(fp, "\"product_rev\": \"%s\", ", revision);
	fprintf(fp, "\"product_name\": \"%s\", ", name);
	fprintf(fp, "}");
}

void main(int argc, char **argv) {
int c;
int values[6];
int i;
	
	fp = stdout;
	while ((c = getopt(argc, argv, "b:s:m:r:n:t:")) != -1) {
		switch (c) {
			case 'b':
				if( 6 == sscanf( optarg, "%x:%x:%x:%x:%x:%x%*c",
					&values[0], &values[1], &values[2],
					&values[3], &values[4], &values[5] ) ) {
					for( i = 0; i < 6; ++i )
						base_mac[i] = (uint8_t) values[i];
				}
				else {
					printf("Invalid MAC address %s!\n", optarg);
				}
			break;
			case 's':
				serno = optarg;
			break;
			case 'm':
				model = optarg;
			break;
			case 'r':
				revision = optarg;
			break;
			case 'n':
				name = optarg;
			break;
			case 't':
				mac_addrs_to_allocate = atoi(optarg);
			break;

		}
	}
	write_header();
	write_json();
}
