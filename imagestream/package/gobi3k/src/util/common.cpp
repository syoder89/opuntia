#include "StdAfx.h"
#include "GobiError.h"
#include "GobiConnectionMgmtAPI.h"
#include "GobiQMICore.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

const char *gobi_strerr(ULONG gobi_errno)
{
  if (gobi_errno > eGOBI_ERR_ENUM_BEGIN && gobi_errno < eGOBI_ERR_ENUM_END)
  {
    switch (gobi_errno)
    {
        case eGOBI_ERR_NONE:
          return "Success";
        case eGOBI_ERR_GENERAL:
          return "General error";
        case eGOBI_ERR_INTERNAL:
          return "Internal error";
        case eGOBI_ERR_MEMORY:
          return "Memory error";
        case eGOBI_ERR_INVALID_ARG:
          return "Invalid argument";
        case eGOBI_ERR_BUFFER_SZ:
          return "Buffer too small";
        case eGOBI_ERR_NO_DEVICE:
          return "Unable to detect device";
        case eGOBI_ERR_INVALID_DEVID:
          return "Invalid device ID";
        case eGOBI_ERR_NO_CONNECTION:
          return "No connection to device";
        case eGOBI_ERR_IFACE:
          return "Unable to obtain required interace";
        case eGOBI_ERR_CONNECT:
          return "Unable to connect to interface";
        case eGOBI_ERR_REQ_SCHEDULE:
          return "Unable to schedule request";
        case eGOBI_ERR_REQUEST:
          return "Error sending request";
        case eGOBI_ERR_RESPONSE:
          return "Error receiving response";
        case eGOBI_ERR_REQUEST_TO:
          return "Timeout while sending request";
        case eGOBI_ERR_RESPONSE_TO:
          return "Timeout while receiving response";
        case eGOBI_ERR_MALFORMED_RSP:
          return "Malformed response received";
        case eGOBI_ERR_INVALID_RSP:
          return "Invalid/error response received";
        case eGOBI_ERR_INVALID_FILE:
          return "Invalid file path";
        case eGOBI_ERR_FILE_OPEN:
          return "Unable to open file";
        case eGOBI_ERR_FILE_COPY:
          return "Unable to copy file";
        case eGOBI_ERR_QDL_SCM:
          return "Unable to open service control mgr";
        case eGOBI_ERR_NO_QDL_SVC:
          return "Unable to detect QDL service";
        case eGOBI_ERR_NO_QDL_SVC_INFO:
          return "Unable to obtain QDL service info";
        case eGOBI_ERR_NO_QDL_SVC_PATH:
          return "Unable to locate QSL service ";
        case eGOBI_ERR_QDL_SVC_CFG:
          return "Unable to reconfigure QDL service";
        case eGOBI_ERR_QDL_SVC_IFACE:
          return "Unable to interface to QDL service";
        case eGOBI_ERR_OFFLINE:
          return "Unable to set device offline";
        case eGOBI_ERR_RESET:
          return "Unable to reset device";
        case eGOBI_ERR_NO_SIGNAL:
          return "No available signal ";
        case eGOBI_ERR_MULTIPLE_DEVICES:
          return "Multiple devices detected";
        case eGOBI_ERR_DRIVER:
          return "Error interfacing to driver";
        case eGOBI_ERR_NO_CANCELABLE_OP:
          return "No cancelable operation is pending";
        case eGOBI_ERR_CANCEL_OP:
          return "Error canceling outstanding operation";
        case eGOBI_ERR_QDL_CRC:
          return "QDL image data CRC error";
        case eGOBI_ERR_QDL_PARSING:
          return "QDL image data parsing error";
        case eGOBI_ERR_QDL_AUTH:
          return "QDL image authentication error";
        case eGOBI_ERR_QDL_WRITE:
          return "QDL image write error";
        case eGOBI_ERR_QDL_OPEN_SIZE:
          return "QDL image size error";
        case eGOBI_ERR_QDL_OPEN_TYPE:
          return "QDL image type error";
        case eGOBI_ERR_QDL_OPEN_PROT:
          return "QDL memory protection error";
        case eGOBI_ERR_QDL_OPEN_SKIP:
          return "QDL image not required";
        case eGOBI_ERR_QDL_ERR_GENERAL:
          return "QDL general error";
        case eGOBI_ERR_QDL_BAR_MODE:
          return "QDL BAR mode error";
        default:
          return "Unknown API error";
    }
  }
  else if (gobi_errno >= eGOBI_ERR_QMI_OFFSET && gobi_errno < eGOBI_ERR_QDL_OFFSET)
  {
    ULONG tmp = (ULONG)gobi_errno - (ULONG)eGOBI_ERR_QMI_OFFSET;
    switch (tmp)
    {
        case eQMI_ERR_NONE:
          return "eQMI_ERR_NONE";
        case eQMI_ERR_MALFORMED_MSG:
          return "eQMI_ERR_MALFORMED_MSG";
        case eQMI_ERR_NO_MEMORY:
          return "eQMI_ERR_NO_MEMORY";
        case eQMI_ERR_INTERNAL:
          return "eQMI_ERR_INTERNAL";
        case eQMI_ERR_ABORTED:
          return "eQMI_ERR_ABORTED";
        case eQMI_ERR_CLIENT_IDS_EXHAUSTED:
          return "eQMI_ERR_CLIENT_IDS_EXHAUSTED";
        case eQMI_ERR_UNABORTABLE_TRANSACTION:
          return "eQMI_ERR_UNABORTABLE_TRANSACTION";
        case eQMI_ERR_INVALID_CLIENT_ID:
          return "eQMI_ERR_INVALID_CLIENT_ID";
        case eQMI_ERR_NO_THRESHOLDS:
          return "eQMI_ERR_NO_THRESHOLDS";
        case eQMI_ERR_INVALID_HANDLE:
          return "eQMI_ERR_INVALID_HANDLE";
        case eQMI_ERR_INVALID_PROFILE:
          return "eQMI_ERR_INVALID_PROFILE";
        case eQMI_ERR_INVALID_PIN_ID:
          return "eQMI_ERR_INVALID_PIN_ID";
        case eQMI_ERR_INCORRECT_PIN:
          return "eQMI_ERR_INCORRECT_PIN";
        case eQMI_ERR_NO_NETWORK_FOUND:
          return "eQMI_ERR_NO_NETWORK_FOUND";
        case eQMI_ERR_CALL_FAILED:
          return "eQMI_ERR_CALL_FAILED";
        case eQMI_ERR_OUT_OF_CALL:
          return "eQMI_ERR_OUT_OF_CALL";
        case eQMI_ERR_NOT_PROVISIONED:
          return "eQMI_ERR_NOT_PROVISIONED";
        case eQMI_ERR_MISSING_ARG:
          return "eQMI_ERR_MISSING_ARG";
        case eQMI_ERR_18:
          return "eQMI_ERR_18";
        case eQMI_ERR_ARG_TOO_LONG:
          return "eQMI_ERR_ARG_TOO_LONG";
        case eQMI_ERR_20:
          return "eQMI_ERR_20";
        case eQMI_ERR_21:
          return "eQMI_ERR_21";
        case eQMI_ERR_INVALID_TX_ID:
          return "eQMI_ERR_INVALID_TX_ID";
        case eQMI_ERR_DEVICE_IN_USE:
          return "eQMI_ERR_DEVICE_IN_USE";
        case eQMI_ERR_OP_NETWORK_UNSUPPORTED:
          return "eQMI_ERR_OP_NETWORK_UNSUPPORTED";
        case eQMI_ERR_OP_DEVICE_UNSUPPORTED:
          return "eQMI_ERR_OP_DEVICE_UNSUPPORTED";
        case eQMI_ERR_NO_EFFECT:
          return "eQMI_ERR_NO_EFFECT";
        case eQMI_ERR_NO_FREE_PROFILE:
          return "eQMI_ERR_NO_FREE_PROFILE";
        case eQMI_ERR_INVALID_PDP_TYPE:
          return "eQMI_ERR_INVALID_PDP_TYPE";
        case eQMI_ERR_INVALID_TECH_PREF:
          return "eQMI_ERR_INVALID_TECH_PREF";
        case eQMI_ERR_INVALID_PROFILE_TYPE:
          return "eQMI_ERR_INVALID_PROFILE_TYPE";
        case eQMI_ERR_INVALID_SERVICE_TYPE:
          return "eQMI_ERR_INVALID_SERVICE_TYPE";
        case eQMI_ERR_INVALID_REGISTER_ACTION:
          return "eQMI_ERR_INVALID_REGISTER_ACTION";
        case eQMI_ERR_INVALID_PS_ATTACH_ACTION:
          return "eQMI_ERR_INVALID_PS_ATTACH_ACTION";
        case eQMI_ERR_AUTHENTICATION_FAILED:
          return "eQMI_ERR_AUTHENTICATION_FAILED";
        case eQMI_ERR_PIN_BLOCKED:
          return "eQMI_ERR_PIN_BLOCKED";
        case eQMI_ERR_PIN_ALWAYS_BLOCKED:
          return "eQMI_ERR_PIN_ALWAYS_BLOCKED";
        case eQMI_ERR_UIM_UNINITIALIZED:
          return "eQMI_ERR_UIM_UNINITIALIZED";
        case eQMI_ERR_MAX_QOS_REQUESTS_IN_USE:
          return "eQMI_ERR_MAX_QOS_REQUESTS_IN_USE";
        case eQMI_ERR_INCORRECT_FLOW_FILTER:
          return "eQMI_ERR_INCORRECT_FLOW_FILTER";
        case eQMI_ERR_NETWORK_QOS_UNAWARE:
          return "eQMI_ERR_NETWORK_QOS_UNAWARE";
        case eQMI_ERR_INVALID_QOS_ID:
          return "eQMI_ERR_INVALID_QOS_ID";
        case eQMI_ERR_REQUESTED_NUM_UNSUPPORTED:
          return "eQMI_ERR_REQUESTED_NUM_UNSUPPORTED";
        case eQMI_ERR_INTERFACE_NOT_FOUND:
          return "eQMI_ERR_INTERFACE_NOT_FOUND";
        case eQMI_ERR_FLOW_SUSPENDED:
          return "eQMI_ERR_FLOW_SUSPENDED";
        case eQMI_ERR_INVALID_DATA_FORMAT:
          return "eQMI_ERR_INVALID_DATA_FORMAT";
        case eQMI_ERR_GENERAL:
          return "eQMI_ERR_GENERAL";
        case eQMI_ERR_UNKNOWN:
          return "eQMI_ERR_UNKNOWN";
        case eQMI_ERR_INVALID_ARG:
          return "eQMI_ERR_INVALID_ARG";
        case eQMI_ERR_INVALID_INDEX:
          return "eQMI_ERR_INVALID_INDEX";
        case eQMI_ERR_NO_ENTRY:
          return "eQMI_ERR_NO_ENTRY";
        case eQMI_ERR_DEVICE_STORAGE_FULL:
          return "eQMI_ERR_DEVICE_STORAGE_FULL";
        case eQMI_ERR_DEVICE_NOT_READY:
          return "eQMI_ERR_DEVICE_NOT_READY";
        case eQMI_ERR_NETWORK_NOT_READY:
          return "eQMI_ERR_NETWORK_NOT_READY";
        case eQMI_ERR_WMS_CAUSE_CODE:
          return "eQMI_ERR_WMS_CAUSE_CODE";
        case eQMI_ERR_WMS_MESSAGE_NOT_SENT:
          return "eQMI_ERR_WMS_MESSAGE_NOT_SENT";
        case eQMI_ERR_WMS_MESSAGE_DELIVERY_FAILURE:
          return "eQMI_ERR_WMS_MESSAGE_DELIVERY_FAILURE";
        case eQMI_ERR_WMS_INVALID_MESSAGE_ID:
          return "eQMI_ERR_WMS_INVALID_MESSAGE_ID";
        case eQMI_ERR_WMS_ENCODING:
          return "eQMI_ERR_WMS_ENCODING";
        case eQMI_ERR_AUTHENTICATION_LOCK:
          return "eQMI_ERR_AUTHENTICATION_LOCK";
        case eQMI_ERR_INVALID_TRANSITION:
          return "eQMI_ERR_INVALID_TRANSITION";
        case eQMI_ERR_61:
          return "eQMI_ERR_61";
        case eQMI_ERR_62:
          return "eQMI_ERR_62";
        case eQMI_ERR_63:
          return "eQMI_ERR_63";
        case eQMI_ERR_64:
          return "eQMI_ERR_64";
        case eQMI_ERR_SESSION_INACTIVE:
          return "eQMI_ERR_SESSION_INACTIVE";
        case eQMI_ERR_SESSION_INVALID:
          return "eQMI_ERR_SESSION_INVALID";
        case eQMI_ERR_SESSION_OWNERSHIP:
          return "eQMI_ERR_SESSION_OWNERSHIP";
        case eQMI_ERR_INSUFFICIENT_RESOURCES:
          return "eQMI_ERR_INSUFFICIENT_RESOURCES";
        case eQMI_ERR_DISABLED:
          return "eQMI_ERR_DISABLED";
        case eQMI_ERR_INVALID_OPERATION:
          return "eQMI_ERR_INVALID_OPERATION";
        case eQMI_ERR_INVALID_QMI_CMD:
          return "eQMI_ERR_INVALID_QMI_CMD";
        case eQMI_ERR_WMS_TPDU_TYPE:
          return "eQMI_ERR_WMS_TPDU_TYPE";
        case eQMI_ERR_WMS_SMSC_ADDR:
          return "eQMI_ERR_WMS_SMSC_ADDR";
        case eQMI_ERR_INFO_UNAVAILABLE:
          return "eQMI_ERR_INFO_UNAVAILABLE";
        case eQMI_ERR_SEGMENT_TOO_LONG:
          return "eQMI_ERR_SEGMENT_TOO_LONG";
        case eQMI_ERR_SEGMENT_ORDER:
          return "eQMI_ERR_SEGMENT_ORDER";
        case eQMI_ERR_BUNDLING_NOT_SUPPORTED:
          return "eQMI_ERR_BUNDLING_NOT_SUPPORTED";
        case eQMI_ERR_78:
          return "eQMI_ERR_78";
        case eQMI_ERR_POLICY_MISMATCH:
          return "eQMI_ERR_POLICY_MISMATCH";
        case eQMI_ERR_SIM_FILE_NOT_FOUND:
          return "eQMI_ERR_SIM_FILE_NOT_FOUND";
        case eQMI_ERR_EXTENDED_EXTERNAL:
          return "eQMI_ERR_EXTENDED_EXTERNAL";
        case eQMI_ERR_ACCESS_DENIED:
          return "eQMI_ERR_ACCESS_DENIED";
        case eQMI_ERR_HARDWARE_RESTRICTED:
          return "eQMI_ERR_HARDWARE_RESTRICTED";
        case eQMI_ERR_ACK_NOT_SENT:
          return "eQMI_ERR_ACK_NOT_SENT";
        case eQMI_ERR_INCOMPATIBLE_STATE:
          return "eQMI_ERR_INCOMPATIBLE_STATE";
        case eQMI_ERR_FDN_RESTRICT:
          return "eQMI_ERR_FDN_RESTRICT";
        case eQMI_ERR_SUPS_FAILURE_CAUSE:
          return "eQMI_ERR_SUPS_FAILURE_CAUSE";
        case eQMI_ERR_NO_RADIO:
          return "eQMI_ERR_NO_RADIO";
        case eQMI_ERR_NOT_SUPPORTED:
          return "eQMI_ERR_NOT_SUPPORTED";
        case eQMI_ERR_CARD_CALL_CONTROL_FAILED:
          return "eQMI_ERR_CARD_CALL_CONTROL_FAILED";
        case eQMI_ERR_NETWORK_ABORTED:
          return "eQMI_ERR_NETWORK_ABORTED";
        case eQMI_ERR_CAT_EVT_REG_FAILED:
          return "eQMI_ERR_CAT_EVT_REG_FAILED";
        case eQMI_ERR_CAT_INVALID_TR:
          return "eQMI_ERR_CAT_INVALID_TR";
        case eQMI_ERR_CAT_INVALID_ENV_CMD:
          return "eQMI_ERR_CAT_INVALID_ENV_CMD";
        case eQMI_ERR_CAT_ENV_CMD_BUSY:
          return "eQMI_ERR_CAT_ENV_CMD_BUSY";
        case eQMI_ERR_CAT_ENV_CMD_FAIL:
          return "eQMI_ERR_CAT_ENV_CMD_FAIL";
        default:
          return "Unknown QMI error";
    }
  }
  else if (gobi_errno >= eGOBI_ERR_QDL_OFFSET)
  {
      ULONG tmp = (ULONG)gobi_errno - (ULONG)eGOBI_ERR_QDL_OFFSET;
      switch (tmp)
      {
          case eQDL_ERROR_01:
            return "Reserved";
          case eQDL_ERROR_BAD_ADDR:
            return "Invalid destination address";
          case eQDL_ERROR_BAD_LEN:
            return "Invalid length";
          case eQDL_ERROR_BAD_PACKET:
            return "Unexpected end of packet";
          case eQDL_ERROR_BAD_CMD:
            return "Invalid command";
          case eQDL_ERROR_06:
            return "Reserved";
          case eQDL_ERROR_OP_FAILED:
            return "Operation failed";
          case eQDL_ERROR_BAD_FLASH_ID:
            return "Invalid flash intelligent ID";
          case eQDL_ERROR_BAD_VOLTAGE:
            return "Invalid programming voltage";
          case eQDL_ERROR_WRITE_FAILED:
            return "Write verify failed";
          case eQDL_ERROR_11:
            return "Reserved";
          case eQDL_ERROR_BAD_SPC:
            return "Invalid security code";
          case eQDL_ERROR_POWERDOWN:
            return "Power-down failed";
          case eQDL_ERROR_UNSUPPORTED:
            return "NAND flash programming not supported";
          case eQDL_ERROR_CMD_SEQ:
            return "Command out of sequence";
          case eQDL_ERROR_CLOSE:
            return "Close failed";
          case eQDL_ERROR_BAD_FEATURES:
            return "Invalid feature bits";
          case eQDL_ERROR_SPACE:
            return "Out of space";
          case eQDL_ERROR_BAD_SECURITY:
            return "Invalid security mode";
          case eQDL_ERROR_MULTI_UNSUPPORTED:
            return "Multi-image NAND not supported";
          case eQDL_ERROR_POWEROFF:
            return "Power-off command not supported";
          case eQDL_ERROR_CMD_UNSUPPORTED:
            return "Command not supported";
          case eQDL_ERROR_BAD_CRC:
            return "Invalid CRC";
          case eQDL_ERROR_STATE:
            return "Command received in invalid state";
          case eQDL_ERROR_TIMEOUT:
            return "Receive timeout";
          case eQDL_ERROR_IMAGE_AUTH:
            return "Image authentication error";
          default:
            return "Unkown QDL error";
      }
  }
  return "Unknown unknown error!";
}

const char *pin_status_str(ULONG pin_status)
{
  switch (pin_status)
  {
      case 0:
        return "PIN not initialized";
      case 1:
        return "PIN enabled, not verified";
      case 2:
        return "PIN enabled, verified";
      case 3:
        return "PIN disabled";
      case 4:
        return "PIN blocked";
      case 5:
        return "PIN permanently blocked";
      default:
        return "PIN Unknown";
  }
}

void fail(ULONG err, const char *desc)
{
  // Ignore NO_EFFECT errors
  if (err - (ULONG)eGOBI_ERR_QMI_OFFSET == eQMI_ERR_NO_EFFECT)
	return;
  char *buffer = (char *)malloc(512);
  std::cout << desc << ": " << gobi_strerr(err) << " (" << err << ")" << std::endl;
  free(buffer);
  exit(1);
}

void connect_unlock()
{
  static const int NDEVS = 16;
  struct {
    char node[256];
    char key[16];
  } devs[NDEVS];
  ULONG r;
  BYTE len = NDEVS;
  int i;
  ULONG ret;
  std::string pin;
  
  ret = QCWWANEnumerateDevices(&len, (BYTE *)devs);
  if (ret != eGOBI_ERR_NONE)
  {
    fail(ret, "QCWWANEnumerateDevices");
  }
  
  if (len == 0)
  {
    printf("No devices found!\n");
    exit(1);
  }
  
//  printf("Found %d device(s)\n", len);
//  printf("Connecting to: %s with %s\n", devs[0].node, devs[0].key);
  ret = QCWWANConnect(devs[0].node, devs[0].key);
  if (ret != eGOBI_ERR_NONE)
  {
    fail(ret, "QCWWANConnect");
  }
//  printf("Connected.\n");
  
#if 0
  ULONG status, retriesLeft, unblockLeft;
  ret = UIMGetPINStatus(1, &status, &retriesLeft, &unblockLeft);
  if (ret != eGOBI_ERR_NONE)
  {
    fail(ret, "UIMGetPINStatus");
  }
  printf("Pin status: %s (%lu)\n", pin_status_str(status), status);
  while (status == 1)
  {
    printf("Enter pin, %lu tries left: ", retriesLeft);
    std::cin >> pin;
    ret = UIMVerifyPIN(1, (char *)pin.c_str(), &retriesLeft, &unblockLeft);
    if (ret != eGOBI_ERR_NONE)
    {
      fail(ret, "UIMVerifyPIN");
    }
    ret = UIMGetPINStatus(1, &status, &retriesLeft, &unblockLeft);
    if (ret != eGOBI_ERR_NONE)
    {
      fail(ret, "UIMGetPINStatus");
    }
    if (status == 1)
    {
      printf("Bad pin\n");
      if (retriesLeft == 0)
      {
        printf("No tries left..\n");
        exit(1);
      }
    }
  }
#endif


#if 0
  // AT&T
  ULONG loadFirmwareID = 2;
  sprintf(file, "/opt/Qualcomm/Gobi/firmware/%d/", loadFirmwareID);
  printf("Loading firmware %d from %s\n", loadFirmwareID, file);
  ret = UpdateFirmware2k( file );

  if (ret)
  {
    fail(ret, "UpgradeFirmware");
  }
  printf("Firmware %d loaded.\n", loadFirmwareID);
#endif

#if 0
  ret = InitiateNetworkRegistration( 2, 310, 410, 5);

  if (ret)
  {
    fail(ret, "Register");
  }
  printf("Registered with AT&T\n");
#endif

#if 0
ULONG SessionID, FailureReason;
char *APN = "isp.cingular";

ret = StartDataSession( NULL, NULL, NULL, NULL, NULL, APN, NULL, NULL, NULL, NULL, &SessionID, &FailureReason );
#if 0
if (ret) {
  printf("Failure reason: %d\n", FailureReason);
  fail(ret, "StartDataSession");
}
#endif
printf("Session %d\n", SessionID);
#endif

}
