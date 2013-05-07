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
#include <syslog.h>
#include <signal.h>
#include <sys/wait.h>

#define log_msg syslog

/* minimum required number of parameters */
#define MIN_REQUIRED 2
#define GOBI_SESSION_ID_FILE "/var/state/gobi3k.session"
#define RAT_GSM		4
#define RAT_UMTS	5
#define MSGTYPE_READ	1
#define MSGTYPE_UNREAD	2
#define MODEM_TTY "/dev/ttyUSB0"
#define MAX_RSSQ 31

const char *APNLIST = "apnlist.txt";
int machine = 0;
void binary2pdu(char* binary, int length, char* pdu);
int min_rssq = 0;

void error(char *message){
	printf("Error: %s\n", message);
	exit(1);
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

void card_connect() {
  	connect_unlock();
}

void card_disconnect() {
	int ret;
  	ret = QCWWANDisconnect();
//  	if (ret)
//   		fail(ret, "QCWWANDisconnect");
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

const char *call_end_reason_lookup(ULONG reason)
{
	switch(reason) {
		case 1:
			return "Reason unspecified";
		break;
		case 2:
			return "Client ended the call";
		break;
		case 3:
			return "Device has no service";
		break;
		case 4:
			return "Call has ended abnormally";
		break;
		case 5:
			return "Received release from base station; no reason given";
		break;
		case 6:
			return "Access attempt already in progress";
		break;
		case 7:
			return "Access attempt failure for reason other than the above";
		break;
		case 8:
			return "Call rejected because of redirection or handoff";
		break;
		case 9:
			return "Call failed because close is in progress";
		break;
		case 10:
			return "Authentication failed";
		break;
		case 11:
			return "Call ended because of internal error";
		break;
		case 500:
			return "Device is CDMA locked until power cycle";
		break;
		case 501:
			return "Received intercept from base station; origination only";
		break;
		case 502:
			return "Received reorder from base station; origination only";
		break;
		case 503:
			return "Received release from base station; service option reject";
		break;
		case 504:
			return "Received incoming call from base station";
		break;
		case 505:
			return "Received alert stop from base station; incoming only";
		break;
		case 506:
			return "Received end activation; OTASP call only";
		break;
		case 507:
			return "Max access probes transmitted";
		break;
		case 508:
			return "Concurrent service is not supported by base station";
		break;
		case 509:
			return "No response received from base station";
		break;
		case 510:
			return "Call rejected by the base station";
		break;
		case 511:
			return "Concurrent services requested were not compatible";
		break;
		case 512:
			return "Call manager subsystem already in TC";
		break;
		case 513:
			return "Call manager subsystem is ending a GPS call in favor of a user call";
		break;
		case 514:
			return "Call manager subsystem is ending a SMS call in favor of a user call";
		break;
		case 515:
			return "Device has no CDMA service";
		break;
		case 1000:
			return "Call origination request failed";
		break;
		case 1001:
			return "Client rejected the incoming call";
		break;
		case 1002:
			return "Device has no UMTS service";
		break;
		case 1003:
			return "Network ended the call";
		break;
		case 1004:
			return "LLC or SNDCP failure";
		break;
		case 1005:
			return "Insufficient resources";
		break;
		case 1006:
			return "Service option temporarily out of order";
		break;
		case 1007:
			return "NSAPI already used";
		break;
		case 1008:
			return "Regular PDP context deactivation";
		break;
		case 1009:
			return "Network failure";
		break;
		case 1010:
			return "Reactivation requested";
		break;
		case 1011:
			return "Protocol error, unspecified";
		break;
		case 1012:
			return "Operator-determined barring (exclusion)";
		break;
		case 1013:
			return "Unknown or missing Access Point Name (APN)";
		break;
		case 1014:
			return "Unknown PDP address or PDP type";
		break;
		case 1015:
			return "Activation rejected by GGSN";
		break;
		case 1016:
			return "Activation rejected, unspecified";
		break;
		case 1017:
			return "Service option not supported";
		break;
		case 1018:
			return "Requested service option not subscribed";
		break;
		case 1019:
			return "Quality of Service (QoS) not accepted";
		break;
		case 1020:
			return "Semantic error in the TFT operation";
		break;
		case 1021:
			return "Syntactical error in the TFT operation";
		break;
		case 1022:
			return "Unknown PDP context";
		break;
		case 1023:
			return "Semantic errors in packet filter(s)";
		break;
		case 1024:
			return "Syntactical error in packet filter(s)";
		break;
		case 1025:
			return "PDP context without TFT already activated";
		break;
		case 1026:
			return "Invalid transaction identifier value";
		break;
		case 1027:
			return "Semantically incorrect message";
		break;
		case 1028:
			return "Invalid mandatory information";
		break;
		case 1029:
			return "Message type nonexistent or not implemented";
		break;
		case 1030:
			return "Message not compatible with state";
		break;
		case 1031:
			return "Information element nonexistent or not implemented";
		break;
		case 1032:
			return "Conditional information element error";
		break;
		case 1033:
			return "Message not compatible with protocol state";
		break;
		case 1034:
			return "APN restriction value incompatible with active PDP context";
		break;
		case 1035:
			return "No GPRS context present";
		break;
		case 1036:
			return "Requested feature not supported";
		break;
		case 1500:
			return "Abort connection setup due to the reception of a ConnectionDeny message with deny code set to either general or network busy";
		break;
		case 1501:
			return "Abort connection setup due to the reception of a ConnectionDeny message with deny code set to either billing or authentication failure";
		break;
		case 1502:
			return "Change EV-DO system due to redirection or PRL not preferred";
		break;
		case 1503:
			return "Exit EV-DO due to redirection or PRL not preferred";
		break;
		case 1504:
			return "No EV-DO session";
		break;
		case 1505:
			return "Call manager is ending a EV-DO call origination in favor of a GPS call";
		break;
		case 1506:
			return "Connection setup timeout";
		break;
		case 1507:
			return "Call manager released EV-DO call";
		break;
		default:
			return "Unknown";
		break;
	}
}

const char *activation_status_lookup(ULONG status)
{
	switch(status) {
		case 0:
		       return "Service not activated";
		break;
		case 1:
	       		return "Service activated";
		break;
		case 2:
	       		return "Activation connecting";
		break;
		case 3:
	       		return "Activation connected";
		break;
		case 4:
	       		return "OTASP security authenticated";
		break;
		case 5:
	       		return "OTASP NAM downloaded";
		break;
		case 6:
	       		return "OTASP MDN downloaded";
		break;
		case 7:
	       		return "OTASP IMSI downloaded";
		break;
		case 8:
	       		return "OTASP PRL downloaded";
		break;
		case 9:
	       		return "OTASP SPC downloaded";
		break;
		case 10:
	       		return "OTASP settings committed";
		break;
		default:
			return "Unknown";
		break;
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
	sprintf(Command, "/usr/bin/chat -f /tmp/chat.%d -r /tmp/results.%d < %s > %s",
			pid, pid, MODEM_TTY, MODEM_TTY);
	if ((ret = system(Command)) > 0) {
		printf("Unable to determine signal quality, error %d.\n", ret);
		printf("Command: %s\n", Command);
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
	sprintf(Command, "/usr/bin/chat -f /tmp/chat.%d -r /tmp/results.%d < %s > %s",
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

  	if(format == 1) {
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

int link_state = -1;

void down_3g(char *message) {
	if (link_state != 0) {
		log_msg(LOG_ERR, "3G link down due to %s\n", message);
		link_state = 0;
		system("/bin/net_carrier usb0 off");
	}
}

void up_3g() {
	if (link_state != 1) {
		log_msg(LOG_ERR, "3G link up.\n");
		link_state = 1;
		system("/bin/net_carrier usb0 on");
		// Tell the DHCP client to renew
		system("/bin/kill -10 `cat /var/run/udhcpc.usb0.pid`");
	}
}

void connect_3g() {
	log_msg(LOG_INFO, "3G data session not active, starting...\n");
	system("man3g stop-session");
	system("man3g start-session --auto-connect");
}

void check_status() {
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
	int link_ok = 1;
	char message[256];

  	ret = GetServingNetwork( &RegistrationState, &CSDomain, &PSDomain, &RAN, &RadioIfacesSize, (BYTE *)RadioIfaces, &Roaming, &MCC, &MNC, nameSize, Name );

//  	ret = GetSignalStrengths(&numSignals, SignalStrengths, SignalIfaces);
  
	ret = GetSessionState(&state);

//	GetSignalQuality(&rssq);

	if (RegistrationState != 1) {
		sprintf(message, "registration state: %s", 
  			RegistrationState == 0 ?  "Not registered" :
  			RegistrationState == 1 ?  "Registered" :
  			RegistrationState == 2 ?  "Searching/not registered" :
  			RegistrationState == 3 ?  "Registration denied" :
  			"Unknown");
		down_3g(message);
		return;
	}
#if 0
	// Too Risky for now
	if (rssq == 99 || rssq < min_rssq) {
		sprintf(message, "signal quality");
		down_3g(message);
		return;
	}
#endif
	if (state != 2) {
  		sprintf(message, "data session: %s",
         		state == 1 ? "Disconnected" :
         		state == 2 ? "Connected" :
         		state == 3 ? "Suspended (not supported)" :
         		state == 4 ? "Authenticating" :
         		"Unknown");
		down_3g(message);
		connect_3g();
		return;
	}
	up_3g();
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

pid_t master_pid = 0;

void term_handler(int sig) {
	if (master_pid == getpid()) {
		log_msg(LOG_INFO, "Process %d exiting on signal %d.\n", getpid(), sig);
		card_disconnect();
		pthread_exit(NULL);
	}
//	log_msg(LOG_ERR, "Thread Process %d exiting on signal %d.\n", getpid(), sig);
	pthread_exit(NULL);
}

void setup_signals(void) {
	signal(SIGTERM, term_handler);
	signal(SIGBUS, term_handler);
	signal(SIGINT, term_handler);
	signal(SIGSEGV, term_handler);
}
void ignore_signals(void) {
	signal(SIGTERM, SIG_IGN);
	signal(SIGBUS, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGSEGV, SIG_IGN);
}

void loop(void) {
	while (1) {
		ignore_signals();
		check_status();
		setup_signals();
		sleep(5);
	}
}

void SessionStateCallback(ULONG state, ULONG sessionEndReason) {
	if (state == 1) {
		log_msg(LOG_ERR, "Session state change: Disconnected, reason: %s\n", 
         		call_end_reason_lookup(sessionEndReason));
	}
	else {
		log_msg(LOG_ERR, "Session state change: %s\n", 
         		state == 2 ? "Connected" :
         		state == 3 ? "Suspended (not supported)" :
         		state == 4 ? "Authenticating" :
         			"Unknown");
	}
}
void DataBearerCallback(ULONG dataBearer) {
  	log_msg(LOG_ERR, "3G New Data Bearer Technology: %s\n", data_bearer_tech_lookup(dataBearer));
}

void DormancyStatusCallback(ULONG dormancyStatus) {
  	log_msg(LOG_ERR, "3G New Dormancy Status: %s\n", dormancyStatus == 1 ? "Traffic channel dormant" : dormancyStatus == 2 ? "Traffic channel active" : "Unknown");
}

void ActivationStatusCallback(ULONG activationStatus) {
  	log_msg(LOG_ERR, "3G New Activation Status: %s\n", activation_status_lookup(activationStatus));
}

void PowerCallback(ULONG operatingMode ){
  	log_msg(LOG_ERR, "3G New Power Mode: %s\n", power_mode_lookup(operatingMode));
}

void RoamingIndicatorCallback(ULONG roaming){
	if (roaming > 2)
  		log_msg(LOG_ERR, "3G New Roaming Indicator: Operator defined value %d\n", roaming);
	else
  		log_msg(LOG_ERR, "3G New Roaming Indicator: %s\n",
  			roaming == 0 ? "Roaming" :
  			roaming == 1 ? "Home" :
  			roaming == 2 ? "Roaming partner" :
  			"Unknown");
}

void SignalStrengthCallback(INT8 signalStrength, ULONG radioInterface) {
	static INT8 last_strength = 0;

	// Odd where we keep getting called back if the signal is real close to a threshold
	if (last_strength != signalStrength) {
  		log_msg(LOG_ERR, "3G New Signal strength (%s): %ddBm, bars: %s\n", radio_iface_lookup(radioInterface), signalStrength, strength_to_bars(signalStrength));
		last_strength = signalStrength;
	}
}

void RFInfoCallback(ULONG radioInterface, ULONG activeBandClass, ULONG activeChannel) {
  	log_msg(LOG_ERR, "3G New Radio information (%s): Band Class: %s, Channel: %d\n", radio_iface_lookup(radioInterface), radio_band_class_lookup(activeBandClass), activeChannel);
}

void NMEACallback(const CHAR *pNMEA) {
	log_msg(LOG_ERR, "3G New NMEA Position: %s\n", pNMEA);
}

void PDSStateCallback(ULONG enabledStatus, ULONG trackingStatus ) {
	log_msg(LOG_ERR, "3G New PDS State: %s\n", enabledStatus == 1 ?
				trackingStatus == 1 ? "GPS tracking inactive" :
				trackingStatus == 2 ? "GPS tracking active" :
				"GPS tracking status unknown" :
				"GPS disabled");
}

void NewSMSCallback(ULONG storageType, ULONG messageIndex) {
	log_msg(LOG_ERR, "3G New SMS received index: %d\n", messageIndex);
}

void register_callbacks(void) {
INT8 thresholds[5] = { -100,-90,-80,-70,-60 };

	SetSessionStateCallback(SessionStateCallback);
	SetDataBearerCallback(DataBearerCallback);
	SetDormancyStatusCallback(DormancyStatusCallback);
	SetActivationStatusCallback(ActivationStatusCallback);
	SetPowerCallback(PowerCallback);
	SetRoamingIndicatorCallback(RoamingIndicatorCallback);
	SetSignalStrengthCallback(SignalStrengthCallback, 5, thresholds);
	SetRFInfoCallback(RFInfoCallback);
	SetNMEACallback(NMEACallback);
	SetPDSStateCallback(PDSStateCallback);
	SetNewSMSCallback(NewSMSCallback);
}

int main(int argc, char **argv){		
	int ret;

	int counter;

	char *mcc=NULL, *mnc=NULL, *apn=NULL, *user=NULL, *password=NULL, *firmware=NULL;
	char *rat_str=NULL, *msgtype_str;
	int auto_connect = 0;
	int status_arg = 0;
	int msgid = 0;
	ULONG rat = RAT_UMTS;
	ULONG msgtype = 0;
	int i;

	if (argc > 2) {
		if (!strcmp(argv[1], "--min-rssi"))
			min_rssq = atoi(argv[2]);
	}
	master_pid = getpid();
	log_msg(LOG_INFO, "ImageStream 3G Connection Manager starting (pid %d)\n", master_pid);
	if (min_rssq)
		log_msg(LOG_INFO, "Setting minimum signal quality to %d\n", min_rssq);
//	for (i=0;i<32;i++)
//		signal(i, term_handler);
	setup_signals();
	signal(SIGCHLD, SIG_IGN);
	card_connect();
	register_callbacks();
	loop();
}
