#include "StdAfx.h"
#include "GobiError.h"
#include "GobiConnectionMgmtAPI.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include "pdu.h"

/* minimum required number of parameters */
#define MIN_REQUIRED 2
#define GOBI_SESSION_ID_FILE "/var/state/gobi3k.session"
#define RAT_GSM		4
#define RAT_UMTS	5
#define MSGTYPE_READ	1
#define MSGTYPE_UNREAD	2
#define MAX_RSSQ 31

// status formats
#define STATUS_FORMAT_HUMAN 1
#define STATUS_FORMAT_MACHINE 2

const char *APNLIST = "apnlist.txt";
char MODEM_TTY[256] = { '\0' };
int machine = 0;
void binary2pdu(char* binary, int length, char* pdu);
int get_sim();

void error(char *message){
	printf("Error: %s\n", message);
	exit(1);
}

int getIntOption(char *option){
	if(strcmp(option, "register") == 0){
		return 1;
	}
	if(strcmp(option, "status") == 0){
		return 2;
	}
	if(strcmp(option, "scan") == 0){
		return 3;
	}
	if(strcmp(option, "start-session") == 0){		
		return 4;
	}
	if(strcmp(option, "stop-session") == 0){		
		return 5;
	}
	if(strcmp(option, "--help") == 0){		
		return 6;
	}
	if(strcmp(option, "list-firmware") == 0){		
		return 7;
	}
	if(strcmp(option, "display-serno") == 0){		
		return 8;
	}
	if(strcmp(option, "display-sms") == 0){		
		return 9;
	}
	if(strcmp(option, "load-firmware") == 0){		
		return 10;
	}
	if(strcmp(option, "mark-sms") == 0){		
		return 11;
	}
	if(strcmp(option, "delete-sms") == 0){		
		return 12;
	}
	if(strcmp(option, "set-sim") == 0){		
		return 13;
	}
	if(strcmp(option, "check-firmware-loaded") == 0){
		return 14;
	}
	return 0;
}

int help(){
	printf("Usage: man3g [OPTION] [ARGUMENTS]\n");
	printf("Commands:\n");
	printf("  register | status | scan | start-session | stop-session | set-sim | list-firmware | display-serno | display-sms | mark-sms | delete-sms | load-firmware | check-firmware-loaded\n");
	return 0;
}

int usage() {	
	help();
	return 1;
}

int invalidOption(char *option) {
   printf("Invalid option: '%s'\n", option);
   return 1;
}

void GetModemTTY() {
	int idx = 0;
	struct stat buffer;

	if (!strlen(MODEM_TTY)) {
		while (1) {
			sprintf(MODEM_TTY, "/sys/bus/usb-serial/drivers/GobiSerial driver/ttyUSB%d", idx);
			if ((stat(MODEM_TTY, &buffer) == 0)) {
				sprintf(MODEM_TTY, "/dev/ttyUSB%d", idx);
				return;
			}
			idx++;
		}
	}
}

struct Connection{
	char *carrier;
	char *mcc;
	char *mnc;
	char *apn;
	char *type;
	char *mmsc;
	char *mmsproxy;
	char *mmsport;
	char *user;
	char *password;
	char *proxy;
	char *port;
	char *server;
	char *authtype;
};

char** fSplitStr(char *str, const char *delimiters){
    char * token; 
    char **tokenArray;
    int count=0;
    token = (char *)strtok(str, delimiters); // Get the first token
    tokenArray = (char**)malloc(1 * sizeof(char*));

    if (token == NULL) {       
        return tokenArray;  
	} 

    while (token != NULL ) { // While valid tokens are returned     
        tokenArray[count] = (char*)malloc(sizeof(token));
        tokenArray[count] = token;
        //printf ("%s\n", tokenArray[count]);    
        count++;
        tokenArray = (char **)realloc(tokenArray, sizeof(char *) * (count + 1));      
        token = (char *)strtok(NULL, delimiters); // Get the next token     
	} 
    return tokenArray;
}

int searchApn(int column, char *str){
	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char line[512];
	
	if((fp = fopen(APNLIST, "r")) == NULL){
		printf("Error al leer el archivo: %s\n", APNLIST);
		return -1;
	}
	
	while(fgets(line, 512, fp) != NULL){
		if(strlen(line) > 1){
			//0- carrier^1-mcc^2-mnc^3-apn^4-type^5-mmsc^6-mmsproxy^7-mmsport^8-user^9-password^10-proxy^11-port^12-server^13-authtype
			char **slices = fSplitStr(line, "^");
			if(strstr(slices[column], str) != NULL){
			printf("A match found on line: %d\n", line_num);
				printf("\n%s\n", line);
				find_result++;
			}
		}
		line_num++;
	} 
	
	if(find_result == 0) {
		printf("\nSorry, couldn't find a match.\n");
	}

	if(fp) {
		fclose(fp);
	}
   	return(0);
}

void card_connect() {
  	connect_unlock();
}

void card_disconnect() {
	int ret;
  	ret = QCWWANDisconnect();
  	if (ret)
    		fail(ret, "QCWWANDisconnect");
}

void list_firmware() {
  ULONG firmware, technology, carrier, region, gps;
  ULONG ret;

  ret = GetFirmwareInfo(&firmware, &technology, &carrier, &region, &gps);
  if (ret)
  {
    fail(ret, "GetFirmwareInfo");
  }
  printf("Firmware version: %lu technology: %s carrier: %lu region: %lu gps: %s\n",
         firmware,
         technology == 0 ? "CDMA" : technology == 1 ? "UMTS" : "Unknown",
         carrier, region,
         gps == 0 ? "None" :
         gps == 1 ? "Standalone" :
         gps == 2 ? "Assisted (including XTRA)" :
         gps == 3 ? " Assisted (without XTRA)" :
         "Unknown");
}

int check_firmware_loaded(ULONG FirmwareID) {
  ULONG firmware, technology, carrier, region, gps;
  ULONG l_firmware, l_technology, l_carrier, l_region, l_gps;
  ULONG ret;
  char file[256];

  sprintf(file, "/opt/Qualcomm/Gobi/Images/3000/Generic/%d/", FirmwareID);
  ret = GetImageInfo(file, &firmware, &technology, &carrier, &region, &gps);
  if (ret)
  {
    fail(ret, "GetImageInfo");
  }

  ret = GetFirmwareInfo(&l_firmware, &l_technology, &l_carrier, &l_region, &l_gps);
  if (ret)
  {
    fail(ret, "GetFirmwareInfo");
  }
  if (firmware == l_firmware && technology == l_technology && carrier == l_carrier &&
		region == l_region && gps == l_gps) {
	printf("Firmware %d is loaded.\n", FirmwareID);
	return 0;
  }
  else {
	printf("Firmware %d is NOT loaded.\n", FirmwareID);
  	printf("Firmware %d image version: %lu technology: %s carrier: %lu region: %lu gps: %s\n",
		FirmwareID,
         	firmware,
         	technology == 0 ? "CDMA" : technology == 1 ? "UMTS" : "Unknown",
         	carrier, region,
         	gps == 0 ? "None" :
         	gps == 1 ? "Standalone" :
         	gps == 2 ? "Assisted (including XTRA)" :
         	gps == 3 ? " Assisted (without XTRA)" :
         	"Unknown");
  	printf("Loaded firmware image version: %lu technology: %s carrier: %lu region: %lu gps: %s\n",
         	l_firmware,
         	l_technology == 0 ? "CDMA" : l_technology == 1 ? "UMTS" : "Unknown",
         	l_carrier, l_region,
         	l_gps == 0 ? "None" :
         	l_gps == 1 ? "Standalone" :
         	l_gps == 2 ? "Assisted (including XTRA)" :
         	l_gps == 3 ? " Assisted (without XTRA)" :
         	"Unknown");
	}
	return 1;
}

void scan() {
int ret, i;
// Maximum length for a scanned network description
const ULONG MAX_SNI_DESCRIPTION_LEN = 255;

/*=========================================================================*/
// Struct sScannedNetworkInfo
//    Struct to represent scanned network information
/*=========================================================================*/
// The mDescription array is only 255 bytes so we must pack this struct or
// the compiler will add a byte to the structure and cause a one byte off problem
// for every network after the first.
struct sScannedNetworkInfo
{
   public:
      USHORT mMCC;
      USHORT mMNC;
      ULONG mInUse;
      ULONG mRoaming;
      ULONG mForbidden;
      ULONG mPreferred;
      CHAR mDescription[MAX_SNI_DESCRIPTION_LEN];
} __attribute__ ((__packed__));

  struct sScannedNetworkInfo networks[127];
  BYTE num_networks = 127;
  ret = PerformNetworkScan(&num_networks, (BYTE *)networks);
  if (ret)
  {
    fail(ret, "PerformNetworkScan");
  }
  printf("Found %d networks...\n", num_networks);
  for (i=0;i<num_networks;i++) {
    printf("%d: %d/%d\n", i, networks[i].mMCC, networks[i].mMNC);
    printf("  %s\n", networks[i].mDescription);
  }
}

char *getMessageStatus(ULONG tag)
{
	switch(tag) {
		case 0:
			return "Read";
		break;
		case 1:
			return "Unread";
		break;
		case 2:
			return "Mobile originated and sent";
		break;
		case 3:
			return "Mobile originated but not yet sent";
		break;
		default:
			return "Unknown";
		break;
	}
}

BYTE Messages[100][2048];
ULONG MessageTags[100];
int NumMessages = 0;

void display_sms(int msgid)
{
  	if (msgid >= NumMessages) {
		printf("Invalid message id, max is %d\n", NumMessages);
		return;
	}
    		PDU pdu((const char *)Messages[msgid]);
    		if (!pdu.parse())
    		{
        		printf("PDU parsing failed. Error: %s\n", pdu.getError());
			exit(1);
    		}
	if (machine) {
		printf("%d\n", msgid+1);
		printf("%d\n", MessageTags[msgid]);
    		printf("%s\n", pdu.getSMSC());
    		printf("%s\n", pdu.getNumber());
    		printf("20%s,%s.0,%d:%d\n", pdu.getDate(), pdu.getTime(), atoi(pdu.getTZ()) / 4, (atoi(pdu.getTZ()) %4) * 15);
    		printf("%d\n", pdu.getMessageLen());
    		printf("%s\n", pdu.getMessage());
	}
	else {
		printf("Message %d:\n", msgid+1);
		printf("  Status: %s\n", getMessageStatus(MessageTags[msgid]));
//    		printf("  PDU: %s\n", pdu.getPDU());
    		printf("  SMSC: %s\n", pdu.getSMSC());
    		printf("  Sender: %s\n", pdu.getNumber());
    		printf("  Sender Number Type: %s\n", pdu.getNumberType());
    		printf("  Date: %s\n", pdu.getDate());
    		printf("  Time: %s\n", pdu.getTime());
//    		printf("  UDH Type: %s\n", pdu.getUDHType());
//    		printf("  UDH Data: %s\n", pdu.getUDHData());
    		printf("  Message: %s\n", pdu.getMessage());
    		printf("\n");
	}
}

int mark_sms(int msgid, ULONG tag)
{
	int ret, i;
	struct sSMSMessageList
	{
   		public:
      		ULONG mMessageIndex;
      		ULONG mMessageTag;
	} __attribute__ ((__packed__));

	ULONG MessageListSize, MessageSize, MessageTag, MessageFormat;
	struct sSMSMessageList MessageList[100];
	int storageType = 0;

	for (storageType = 0; storageType <= 1; storageType++) {
		MessageListSize = 100;
  		ret = GetSMSList( storageType, NULL, &MessageListSize, (BYTE *)MessageList);
  		if (ret)
    			fail(ret, "GetSMSList");
		if (msgid < MessageListSize) {
			printf("Marking storage type %d index %d using message index %d as %d\n", storageType, msgid, MessageList[msgid].mMessageIndex, tag);
			ret = ModifySMSStatus( storageType, MessageList[msgid].mMessageIndex, tag);
  			if (ret)
    				fail(ret, "ModifySMSStatus");
			return 0;
		}
		msgid -= MessageListSize;
	}
	printf("Message not found!\n");
	return -ENOENT;
}

int delete_sms(int msgid)
{
	int ret, i;
	struct sSMSMessageList
	{
   		public:
      		ULONG mMessageIndex;
      		ULONG mMessageTag;
	} __attribute__ ((__packed__));

	ULONG MessageListSize, MessageSize, MessageTag, MessageFormat;
	struct sSMSMessageList MessageList[100];
	int storageType = 0;

	for (storageType = 0; storageType <= 1; storageType++) {
		MessageListSize = 100;
  		ret = GetSMSList( storageType, NULL, &MessageListSize, (BYTE *)MessageList);
  		if (ret)
    			fail(ret, "GetSMSList");
		if (msgid < MessageListSize) {
			printf("Deleting storage type %d index %d using message index %d\n", storageType, msgid, MessageList[msgid].mMessageIndex);
			ret = DeleteSMS( storageType, &MessageList[msgid].mMessageIndex, NULL);
  			if (ret)
    				fail(ret, "DeleteSMS");
			return 0;
		}
		msgid -= MessageListSize;
	}
	printf("Message not found!\n");
	return -ENOENT;
}

void get_sms_all()
{
	int ret, i;
	struct sSMSMessageList
	{
   		public:
      		ULONG mMessageIndex;
      		ULONG mMessageTag;
	} __attribute__ ((__packed__));

	ULONG RequestedTag = 1;
	ULONG MessageListSize, MessageSize, MessageTag, MessageFormat;
	struct sSMSMessageList MessageList[100];
	BYTE c;
	char *MessagesPtr = (char *)Messages[0];
	BYTE Message[256];
	char mtmp[10];
	int j = 0, msgid = 0;
	int storageType = 0;

	for (storageType = 0; storageType <= 1; storageType++) {
		MessageListSize = 100;
  		ret = GetSMSList( storageType, NULL, &MessageListSize, (BYTE *)MessageList);
  		if (ret)
    			fail(ret, "GetSMSList");
  		for (i=0;i<MessageListSize;i++) {
    			MessageSize=256;
    			ret = GetSMS( storageType, MessageList[i].mMessageIndex, &MessageTag, &MessageFormat, &MessageSize, Message );
    			if (ret)
    			{
      				fail(ret, "GetSMS");
    			}
			MessagesPtr = (char *)Messages[msgid];
			MessagesPtr[0] = '\0';
    			for (j=0;j<MessageSize;j++) {
      				c = (BYTE)Message[j];
      				sprintf(mtmp, "%02X", c);
				strcat(MessagesPtr, mtmp);
    			}
			MessageTags[msgid] = MessageTag;
			msgid++;
			NumMessages++;
  		}
	}
}

void display_sms_all()
{
	int i;
	get_sms_all();
	for (i=0;i<NumMessages;i++)
		display_sms(i);
}

void display_sms_single(int msgid)
{
	get_sms_all();
	display_sms(msgid);
}

const char *radio_iface_lookup(ULONG iface)
{
	switch(iface) {
		case 0:
			return "No service";
		break;
		case 1:
			return "CDMA 1xRTT";
		break;
		case 2:
			return "CDMA EV-DO";
		break;
		case 3:
			return "AMPS";
		break;
		case 4:
			return "GSM";
		break;
		case 5:
			return "UMTS";
		break;
		default:
			return "Unknown";
		break;
	}
}

const char *power_mode_lookup(ULONG powerMode)
{
	switch(powerMode) {
		case 0:
			return "Online (default)";
		break;
		case 1:
			return "Low Power (Airplane) mode";
		break;
		case 2:
			return "Factory Test mode";
		break;
		case 3:
			return "Offline";
		break;
		case 4:
			return "Reset";
		break;
		case 5:
			return "Power off";
		break;
		case 6:
			return "Persistent Low Power (Airplane) mode";
		break;
	}
}

const char *radio_band_class_lookup(ULONG band_class)
{
	static char s_class[256];

	if (band_class <= 16) {
		sprintf(s_class, "CDMA Band Class %d", band_class);
		return s_class;
	}
	else switch(band_class) {
		case 40:
			return "GSM 450";
		break;
		case 41:
			return "GSM 480";
		break;
		case 42:
			return "GSM 750";
		break;
		case 43:
			return "GSM 850";
		break;
		case 44:
			return "GSM 900 (Extended)";
		break;
		case 45:
			return "GSM 900 (Primary)";
		break;
		case 46:
			return "GSM 900 (Railways)";
		break;
		case 47:
			return "GSM 1800";
		break;
		case 48:
			return "GSM 1900";
		break;
		case 80:
			return "WCDMA 2100";
		break;
		case 81:
			return "WCDMA PCS 1900";
		break;
		case 82:
			return "WCDMA DCS 1800";
		break;
		case 83:
			return "WCDMA 1700 (US)";
		break;
		case 84:
			return "WCDMA 850";
		break;
		case 85:
			return "WCDMA 800";
		break;
		case 86:
			return "WCDMA 2600";
		break;
		case 87:
			return "WCDMA 900";
		break;
		case 88:
			return "WCDMA 1700 (Japan)";
		break;
		default:
			return "Unknown";
		break;
	}
}

const char *data_bearer_tech_lookup(ULONG dataBearer)
{
	switch(dataBearer) {
		case 1:
			return "CDMA 1xRTT";
		break;
		case 2:
			return "CDMA 1xEV-DO Rev 0";
		break;
		case 3:
			return "GPRS";
		break;
		case 4:
			return "WCDMA";
		break;
		case 5:
			return "CDMA 1xEV-DO Rev A";
		break;
		case 6:
			return "EDGE";
		break;
		case 7:
			return "HSDPA DL, WCDMA UL";
		break;
		case 8:
			return "WCDMA DL, HSUPA UL";
		break;
		case 9:
			return "HSDPA";
		break;
		default:
			return "Unknown";
		break;
	}
}

const char *strength_to_bars(INT8 strength)
{
	if (strength >= -90)
		return "XXXX";
	else if (strength >= -97)
		return "XXX";
	else if (strength >= -103)
		return "XX";
	else if (strength >= -107)
		return "X";
	else
		return "O";
	
}

// the conversion is not perfect because the strength is a
// logarithmic value, but the percentage will be more granular
int strength_to_percentage(INT8 strength)
{
	if (strength >= -90) {
		return 100;
	} else if (strength >= -97) {
		return 75 + ((25 / 7.0f) * (97 + strength));
	} else if (strength >= -103) {
		return 50 + ((25 / 6.0f) * (103 + strength));
	} else if (strength >= -107) {
		return 25 + ((25 / 4.0f) * (107 + strength));
	} else {
		return 0;
	}
}

void getConnectRate() {
	ULONG currentTxRate, currentRxRate, maxTxRate, maxRxRate;
	int ret;

  	ret = GetConnectionRate(&currentTxRate, &currentRxRate, &maxTxRate, &maxRxRate);
	printf("Cur_tx: %d\n", currentTxRate);
	printf("Cur_rx: %d\n", currentRxRate);
	printf("Max_tx: %d\n", maxTxRate);
	printf("Max_rx: %d\n", maxRxRate);
}

void getRSSI() {
	ULONG numSignals = 64;
	INT8 SignalStrengths[64];
	ULONG SignalIfaces[64];
	int ret;

  	ret = GetSignalStrengths(&numSignals, SignalStrengths, SignalIfaces);
}

struct sRFInfo
{
   public:
      ULONG mRadioInterface;
      ULONG mActiveBandClass;
      ULONG mActiveChannel;
} __attribute__ ((__packed__));

void getRFInfo() {
	BYTE numRFInfos = 64;
	struct sRFInfo infos[127];
	int ret, i;

  	ret = GetRFInfo(&numRFInfos, (BYTE *)infos);
  	for (i=0;i<numRFInfos;i++) {
  		printf("Radio information (%s): Band Class: %s, Channel: %d\n", radio_iface_lookup(infos[i].mRadioInterface), radio_band_class_lookup(infos[i].mActiveBandClass), infos[i].mActiveChannel);
	}
}

void getPowerMode() {
	ULONG powerMode;
	int ret;

  	ret = GetPower(&powerMode);
  	printf("Power Mode: %s\n", power_mode_lookup(powerMode));
}

void getDataBearer() {
	ULONG dataBearer;
	int ret;

  	ret = GetDataBearerTechnology( &dataBearer );
  	if (!ret)
  		printf("Data Bearer Technology: %s\n", data_bearer_tech_lookup(dataBearer));
}

int GetSignalQuality(BYTE *rssq)
{
	pid_t pid = getpid();
	FILE *fp;
	int ret, err, tmp;
	char Command[256], *ptr, chat_fname[128], results_fname[128];

	sprintf(chat_fname, "/tmp/chat.%d", pid);
	sprintf(results_fname, "/tmp/results.%d", pid);

	fp = fopen(chat_fname, "w");
	if (!fp) {
		printf("Unable to open chat file, error %d!\n", errno);
		return errno;
	}
	fprintf(fp, "REPORT +CSQ:\n");
	fprintf(fp, "TIMEOUT 6\n");
	fprintf(fp, "'' 'AT+CSQ'\n");
	fprintf(fp, "OK ''\n");
	fclose(fp);
	fp = NULL;
	err = -1;
	GetModemTTY();
	sprintf(Command, "/usr/sbin/chat -f /tmp/chat.%d -r /tmp/results.%d < %s > %s",
			pid, pid, MODEM_TTY, MODEM_TTY);
	if ((ret = system(Command)) != 0) {
		printf("Unable to determine signal quality, error %d.\n", ret);
		goto out;
	}
	fp = fopen(results_fname, "r");
	if (!fp) {
		printf("Unable to open results file, error %d!\n", errno);
		goto out;
	}

	while ((fgets(Command, 255, fp)) != NULL) {
		if ((ptr = strstr(Command, "+CSQ:")) != NULL) {
			if ((sscanf(ptr, "+CSQ: %d", &tmp)) == 1) {
				*rssq = (BYTE)tmp;
				err = 0;
				goto out;
			}
		}
	}
	printf("Unable to determine signal quality, improper result from modem!\n");

out:
	if (fp)
		fclose(fp);
	unlink(chat_fname);
	unlink(results_fname);
	return err;
}

int GetAPN(char *APNName)
{
	pid_t pid = getpid();
	FILE *fp;
	int ret, err;
	char Command[256], *ptr, chat_fname[128], results_fname[128];

	sprintf(chat_fname, "/tmp/chat.%d", pid);
	sprintf(results_fname, "/tmp/results.%d", pid);

	fp = fopen(chat_fname, "w");
	if (!fp) {
		printf("Unable to open chat file, error %d!\n", errno);
		return errno;
	}
	fprintf(fp, "REPORT +CGDCONT:\n");
	fprintf(fp, "TIMEOUT 6\n");
	fprintf(fp, "'' 'AT+CGDCONT?'\n");
	fprintf(fp, "OK ''\n");
	fclose(fp);
	fp = NULL;

	err = -1;
	GetModemTTY();
	sprintf(Command, "/usr/sbin/chat -f /tmp/chat.%d -r /tmp/results.%d < %s > %s",
			pid, pid, MODEM_TTY, MODEM_TTY);
	if ((ret = system(Command)) != 0) {
		printf("Unable to determine APN, error %d.\n", ret);
		goto out;
	}
	fp = fopen(results_fname, "r");
	if (!fp) {
		printf("Unable to open results file, error %d!\n", errno);
		goto out;
	}

	while ((fgets(Command, 255, fp)) != NULL) {
		if ((ptr = strstr(Command, "+CGDCONT:")) != NULL) {
			int radio;
			char APNID[256];
			if ((ret = sscanf(ptr, "+CGDCONT: %d,\"%[^\"]\",\"%[^\"]", &radio, APNID, APNName)) == 3) {
				err = 0;
				goto out;
			}
		}
	}
	printf("Unable to determine APN, improper result from modem!\n");

out:
	if (fp)
		fclose(fp);
	unlink(chat_fname);
	unlink(results_fname);
	return err;
}

void status(int format) {
	int ret, i;
	ULONG RegistrationState;
	ULONG CSDomain;
	ULONG PSDomain;
	ULONG RAN;
	BYTE RadioIfacesSize=64;
	ULONG RadioIfaces[64];
	ULONG Roaming;
	WORD MCC;
	WORD MNC;
	BYTE nameSize = 255;
	CHAR Name[255];
	BYTE APNNameSize = 255;
	CHAR APNName[255] = { "CDMA" };
	CHAR VoiceNumber[255], MIN_string[255];
	ULONG state;
	ULONG numSignals = 64;
	INT8 SignalStrengths[64];
	ULONG SignalIfaces[64];
	ULONGLONG Tx, Rx, duration;
	ULONG currentTxRate, currentRxRate, maxTxRate, maxRxRate;
	BYTE rssq;
	int simid;

  	ret = GetVoiceNumber(255, VoiceNumber, 255, MIN_string);
  	if (ret) {
		strcpy(VoiceNumber, "None");
	}
//    		fail(ret, "GetVoiceNumber");
  
  	ret = GetServingNetwork( &RegistrationState, &CSDomain, &PSDomain, &RAN, &RadioIfacesSize, (BYTE *)RadioIfaces, &Roaming, &MCC, &MNC, nameSize, Name );

  	ret = GetSignalStrengths(&numSignals, SignalStrengths, SignalIfaces);
  
  	ret = GetConnectionRate(&currentTxRate, &currentRxRate, &maxTxRate, &maxRxRate);

	if (currentTxRate == -1)
		currentTxRate = maxTxRate;
	if (currentRxRate == -1)
		currentRxRate = maxRxRate;

	ret = GetSessionState(&state);

	if (RAN == 2) {
		GetAPN(APNName);
/*
  		ret = GetDefaultProfile( 0, NULL, NULL, NULL, NULL, NULL, 0, NULL, APNNameSize, APNName, 0, NULL);
  		if (ret)
    			fail(ret, "GetDefaultProfile");
*/
	}

	GetSignalQuality(&rssq);
	simid = get_sim();

  	if(format == STATUS_FORMAT_HUMAN) {
		printf("SIM: %s (%d)\n", simid == 0 ? "Internal" : "External", simid);
  		switch (RAN) {
			case 1: // CDMA
				printf("Carrier: CDMA\n");
				break;
			case 2: // UMTS
				// MCC/MNC/Name only valid for UMTS
				printf("Carrier: %s (MCC %d, MNC %d)\n", Name, MCC, MNC);
				printf("APN: %s\n", APNName);
				break;
  		}
  		printf("Mobile number: %s\n", VoiceNumber);
  		printf("State: %s\n",
  			RegistrationState == 0 ?  "Not registered" :
  			RegistrationState == 1 ?  "Registered" :
  			RegistrationState == 2 ?  "Searching/not registered" :
  			RegistrationState == 3 ?  "Registration denied" :
  			"Unknown");
		getPowerMode();
  		printf("Radio Service type: ");
  		for (i=0;i<RadioIfacesSize;i++)
  			printf("%s ", radio_iface_lookup(RadioIfaces[i]));
  		printf("\n");
		getRFInfo();
		getDataBearer();
  		printf("Data session: %s\n",
         	state == 1 ? "Disconnected" :
         	state == 2 ? "Connected" :
         	state == 3 ? "Suspended (not supported)" :
         	state == 4 ? "Authenticating" :
         	"Unknown");
  	  	ret = GetSessionDuration(&duration);
		if (ret == 0) {
			duration /= 1000;
			time_t connectTime = time(NULL) - duration;
			printf("Connected since %s", ctime(&connectTime));
		}
  		printf("Network: %s\n",
  			Roaming == 0 ? "Roaming" :
  			Roaming == 1 ? "Home" :
  			Roaming == 2 ? "Roaming partner" :
  			"Unknown");
		printf("Signal quality: %d/%d (%d%%)\n", rssq, MAX_RSSQ, rssq <= MAX_RSSQ ? (int)((rssq * 100) / MAX_RSSQ) : 0);
  		for (i=0;i<numSignals;i++) {
  			printf("Signal strength (%s): %ddBm, bars: %s\n", radio_iface_lookup(SignalIfaces[i]), SignalStrengths[i], strength_to_bars(SignalStrengths[i]));
  		}
  		printf("Maximum Connection Rate Rx/Tx: %lu/%lu Kbps\n", maxRxRate/1000, maxTxRate/1000);
  		ret = GetByteTotals(&Tx, &Rx);
  		if (!ret)
  			printf("Session Statistics: Bytes Tx: %llu, Rx: %llu\n", Tx, Rx);
  	} else {
		ULONGLONG duration = 0;;
		time_t connectTime = 0;
		ULONG dataBearer = 0;
		BYTE numRFInfos = 64;
		struct sRFInfo infos[127];

  		GetRFInfo(&numRFInfos, (BYTE *)infos);

  		GetDataBearerTechnology( &dataBearer );
  	  	GetSessionDuration(&duration);
		duration /= 10;

  	  	printf("%s\n", RAN == 1 ? "CDMA" : (RAN == 2 ? Name : "Unknown"));
  	  	printf("%s\n", RadioIfacesSize > 0 ? radio_iface_lookup(RadioIfaces[0]) : "Unknown");
  		printf("%d\n", MCC);
  		printf("%d\n", MNC);
  		printf("%d\n", numSignals > 0 ? SignalStrengths[0] : 0);
  		printf("%lu\n", maxRxRate/1000);
  		printf("%lu\n", maxTxRate/1000);
		printf("%s\n", APNName);
  		printf("%d\n", rssq);
  		printf("%d\n", dataBearer);
  		printf("%d\n", infos[0].mActiveBandClass);
  		printf("%d\n", RegistrationState);
  		printf("%d\n", state);
  		printf("%d\n", duration);
  		printf("%d\n", Roaming);
  		printf("%s\n", VoiceNumber);
		printf("%d\n", simid);
/*
  	  	ret = GetSessionDuration(&duration);
		if (ret == 0) {
			duration /= 1000;
			connectTime = time(NULL) - duration;
		}
  		printf("%d\n", state);
  		printf("%s\n",
         	state == 1 ? "Disconnected" :
         	state == 2 ? "Connected" :
         	state == 3 ? "Suspended (not supported)" :
         	state == 4 ? "Authenticating" :
         	"Unknown");
		if (duration != 0)
			printf("%s", ctime(&connectTime));
		else
			printf("Not connected\n");
*/
  	}
}

void display_serial_numbers()
{
	int ret;
	BYTE esnSize = 255;
	CHAR ESNString[256];
	BYTE imeiSize = 255;
	CHAR IMEIString[256];
	BYTE meidSize = 255;
	CHAR MEIDString[256];

	ret = GetSerialNumbers( esnSize, ESNString, imeiSize, IMEIString, meidSize, MEIDString);
	if (ret)
		fail(ret, "GetSerialNumbers");
	printf("ESN: %s\n", ESNString);
	printf("IMEI: %s\n", IMEIString);
	printf("MEID: %s\n", MEIDString);
}

void unload_modules(void)
{
	system("rmmod GobiNet");
	system("rmmod GobiSerial");
}

void load_modules(void)
{
	system("insmod GobiSerial");
	system("insmod GobiNet");
}

void load_firmware(ULONG loadFirmwareID)
{
	int ret, i;
	char file[256];

  	sprintf(file, "/opt/Qualcomm/Gobi/Images/3000/Generic/%d/", loadFirmwareID);
  	ret = UpgradeFirmware( file );

  	if (ret)
    		fail(ret, "UpgradeFirmware");
  	printf("Firmware %d loaded.\n", loadFirmwareID);
	card_disconnect();
	unload_modules();
	printf("Resetting device");
	for (i=0;i<30;i++) {
		printf(".");
		fflush(stdout);
		sleep(1);
	}
	printf("\n");
	load_modules();
	sleep(3);
	card_connect();
}

int do_register(char *firmware, char *mcc, char *mnc, char *apn, ULONG rat, char *user, char *password, int auto_connect)
{
	int ret;
	if (firmware != NULL)
		load_firmware(atoi(firmware));
	if (mcc == NULL || mnc == NULL) {
		ret = InitiateNetworkRegistration( 1, 0, 0, rat);
	}
	else {
		ret = InitiateNetworkRegistration( 2, atoi(mcc), atoi(mnc), rat);
	}
	if (ret)
		fail(ret, "Network Registration");
	if (apn != NULL || user != NULL) {
		ret = SetDefaultProfile(0, NULL, NULL, NULL, NULL, NULL, NULL,
				apn, user, password);
		if (ret)
			fail(ret, "SetDefaultProfile");
		ret = SetEnhancedAutoconnect(auto_connect, NULL);
		if (ret)
			fail(ret, "SetEnhancedAutoconnect");
	}
	return 0;
}

void start_session(char *apn, char *user, char *password)
{
	ULONG SessionID, FailureReason;
	int ret;
	FILE *fp;
	ULONG roam;

	ret = StartDataSession( NULL, NULL, NULL, NULL, NULL, apn, NULL, NULL, user, password, &SessionID, &FailureReason );
	if (ret) {
  		printf("Failure reason: %d\n", FailureReason);
  		fail(ret, "StartDataSession");
	}
	printf("Session id %d started.\n", SessionID);
	fp = fopen(GOBI_SESSION_ID_FILE, "w");
	if (fp) {
		fprintf(fp, "%d", SessionID);
		fclose(fp);
	}
	roam = 1;
    	ret = SetEnhancedAutoconnect(1, &roam);
    	if (ret)
      		fail(ret, "SetEnhancedAutoconnect");
}

void stop_session()
{
	ULONG SessionID, FailureReason;
	int ret, id;
	FILE *fp;
    	ULONG autoconnect, roam;
	
    	ret = GetEnhancedAutoconnect(&autoconnect, &roam);
    	if (ret)
      		fail(ret, "GetEnhancedAutoconnect");

	if (autoconnect) {
		roam = 1;
    		ret = SetEnhancedAutoconnect(0, &roam);
    		if (ret)
      			fail(ret, "SetEnhancedAutoconnect");
	}

	fp = fopen(GOBI_SESSION_ID_FILE, "r");
	if (!fp) {
		printf("No session active!\n");
		return;
	}
	fscanf(fp, "%d", &id);
	fclose(fp);
	ret = StopDataSession( id );
	if (ret) {
  		fail(ret, "StopDataSession");
	}
	printf("Session stopped\n");
}

void write_sysctl(char *file, char *value)
{
	FILE *fp;
	if (!(fp = fopen(file, "w"))) {
		printf("Unable to write to %s!\n", file);
		return;
	}
	fprintf(fp, "%s", value);
	fclose(fp);
}

void read_sysctl(char *file, char *value, int len)
{
	FILE *fp;
	if (!(fp = fopen(file, "r"))) {
		return;
	}
	fgets(value, len, fp);
	fclose(fp);
}

int get_sim()
{
	char csimid[10] = "0";
	read_sysctl("/sys/class/gpio/GPIO1/value", csimid, 9);
	return atoi(csimid);
}

int set_sim(int simid)
{
	pid_t pid = getpid();
	FILE *fp;
	int ret, err;
	char Command[256], *ptr, chat_fname[128], results_fname[128], csimid[10];

	snprintf(csimid, 3, "%d", simid);
	/* Set the GPIO1 pin to 0 for internal, 1 to external */
	write_sysctl("/sys/class/gpio/export", "1");
        write_sysctl("/sys/class/gpio/GPIO1/direction", "out");
        write_sysctl("/sys/class/gpio/GPIO1/value", csimid);

	/* Reset the modem by issuing AT+CFUN=1,1 */
	sprintf(chat_fname, "/tmp/chat.%d", pid);
	sprintf(results_fname, "/tmp/results.%d", pid);

	fp = fopen(chat_fname, "w");
	if (!fp) {
		printf("Unable to open chat file, error %d!\n", errno);
		return errno;
	}
	fprintf(fp, "TIMEOUT 6\n");
	fprintf(fp, "'' 'AT+CFUN=1,1'\n");
	fprintf(fp, "OK ''\n");
	fclose(fp);
	fp = NULL;

	err = -1;
	GetModemTTY();
	sprintf(Command, "/usr/sbin/chat -f /tmp/chat.%d -r /tmp/results.%d < %s > %s",
			pid, pid, MODEM_TTY, MODEM_TTY);
	if ((ret = system(Command)) != 0) {
		printf("Unable to reset modem, error %d.\n", ret);
		goto out;
	}

out:
	if (fp)
		fclose(fp);
	unlink(chat_fname);
	unlink(results_fname);
	return err;
}

int main(int argc, char **argv){		
	int ret;

	if(argc < MIN_REQUIRED){
		return usage();
	}
	
	int counter;
	int intOption = getIntOption(argv[1]);

	char *mcc=NULL, *mnc=NULL, *apn=NULL, *user=NULL, *password=NULL, *firmware=NULL;
	char *rat_str=NULL, *msgtype_str;
	int auto_connect = 0;
	int status_arg = 0;
	int msgid = 0;
	int simid=0;
	ULONG rat = RAT_UMTS;
	ULONG msgtype = 0;
	for(counter = 2; counter < argc; counter++){
		if(strcmp("--mcc", argv[counter]) == 0){
			mcc = argv[++counter];
			continue;
		}
		if(strcmp("--mnc", argv[counter]) == 0){
			mnc = argv[++counter];
			continue;
		}
		if (strcmp("--apn", argv[counter]) == 0){
			apn = argv[++counter];
			continue;
		}
		if (strcmp("--user", argv[counter]) == 0){
			user = argv[++counter];
			continue;
		}
		if(strcmp("--password", argv[counter]) == 0){
			password = argv[++counter];
			continue;
		}
		if(strcmp("--auto-connect", argv[counter]) == 0){
			auto_connect = 1;
			continue;
		}
		if(strcmp("--firmware", argv[counter]) == 0){
			firmware = argv[++counter];
			continue;
		}
		// status arguments
		if(strcmp("--connect-rate", argv[counter]) == 0){
			status_arg = 1;
			continue;
		}
		if(strcmp("--machine", argv[counter]) == 0){
			status_arg = 2;
			machine = 1;
			continue;
		}
		/* RAT: Radio Access Technology. GSM/UMTS 
		 * does not apply to CDMA2000 */
		if(strcmp("--rat", argv[counter]) == 0){
			rat_str = argv[++counter];
			if(strcmp("gsm", rat_str) == 0)
				rat = RAT_GSM;
			else
				rat = RAT_UMTS;
			continue;
		}
		if(strcmp("--msgid", argv[counter]) == 0){
			msgid = atoi(argv[++counter]);
			continue;
		}
		if(strcmp("--msgtype", argv[counter]) == 0){
			msgtype_str = argv[++counter];
			if(strcmp("read", msgtype_str) == 0)
				msgtype = MSGTYPE_READ;
			else
				msgtype = MSGTYPE_UNREAD;
			continue;
		}
		if(strcmp("--sim", argv[counter]) == 0){
			simid = atoi(argv[++counter]);
			continue;
		}
		return invalidOption(argv[counter]);
	}

	switch(intOption){ 
		case 1:
			card_connect();
			do_register(firmware, mcc, mnc, apn, rat, user, password, auto_connect);
			card_disconnect();
//			return searchApn(1, mcc);
			
			break;
		case 6:
			return usage();
			break;
		case 2:
			card_connect();
			if (status_arg == 1)
				getConnectRate();
			else
				status(!status_arg ? STATUS_FORMAT_HUMAN : STATUS_FORMAT_MACHINE);
			card_disconnect();
			break;
		case 3:
			card_connect();
			scan();
			card_disconnect();
			break;
		case 4:
			card_connect();
			start_session(apn, user, password);
			card_disconnect();
			break;
		case 5:
			card_connect();
			stop_session();
			card_disconnect();
			break;
		case 7:
			card_connect();
			list_firmware();
			card_disconnect();
			break;
		case 8:
			card_connect();
			display_serial_numbers();
			card_disconnect();
			break;
		case 9:
			card_connect();
			if (msgid == 0)
				display_sms_all();
			else
				display_sms_single(msgid-1);
			card_disconnect();
			break;
		case 10:
			card_connect();
			if (firmware == NULL) {
				printf("Must specify firmware id! (--firmware)\n");
				exit(1);
			}
			load_firmware(atoi(firmware));
			card_disconnect();
			break;
		case 11: // mark-sms
			card_connect();
			if (msgid == 0)
				printf("Must specify message id!\n");
			else {
				if (msgtype == 0)
					printf("Must specify message type (--msgtype read, --msgtype unread)!\n");
				else
					mark_sms(msgid-1, msgtype-1);
			}
			card_disconnect();
			break;
		case 12: // delete-sms
			card_connect();
			if (msgid == 0)
				printf("Must specify message id!\n");
			else
				delete_sms(msgid-1);
			card_disconnect();
			break;
		case 13: // set-sim
			card_connect();
			if (simid == 0 || simid == 1)
				set_sim(simid);
			else
				printf("Must specify SIM id (0=internal, 1=external)!\n");
			card_disconnect();
			break;
		case 14: // check-firmware-loaded
			card_connect();
			if (firmware == NULL) {
				printf("Must specify firmware id! (--firmware)\n");
				exit(1);
			}
			ret = check_firmware_loaded(atoi(firmware));
			card_disconnect();
			exit(ret);
			break;
		default:
			printf("Unknown option '%s'.\n", argv[1]);
			break;
	}

/*
  	if (state != 2) {
    		printf("Enabling enhanced autoconnect\n");
    		ULONG foo = 1;
    		ret = SetEnhancedAutoconnect(1, &foo);
    		if (ret)
      			fail(ret, "SetEnhancedAutoconnect");
  	}
*/ 

	return 0;
}
