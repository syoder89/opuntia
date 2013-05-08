#include "StdAfx.h"
#include "GobiError.h"
#include "GobiConnectionMgmtAPI.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

int main()
{
  ULONG ret;
  connect_unlock();
  
  ULONG operation, interval, accuracy;
  BYTE timeout;
  ret = GetPDSDefaults(&operation, &timeout, &interval, &accuracy);
  if (ret)
  {
    fail(ret, "GetPDSDefaults");
  }
  printf("PDS operation: %s timeout: %i interval: %lu accuracy: %lu\n",
         operation == 0 ? "Standalone" :
         operation == 1 ? "MS Based" :
         "Unknown",
         timeout, interval, accuracy);
  
  ULONG enabled, tracking;
  ret = GetPDSState(&enabled, &tracking);
  if (ret)
  {
    fail(ret, "GetPDSState");
  }
  printf("GPS status: %s tracking: %s\n",
         enabled == 0 ? "Disabled" : "Enabled",
         tracking == 0 ? "Unknown" : tracking == 1 ? "Inactive" : "Active");

  ret = SetPDSDefaults(1, timeout, interval, accuracy);
  if (ret)
  {
    fail(ret, "SetPDSDefaults");
  }
  
  // ret = ForceXTRADownload();
  // if (ret)
  // {
  //   fail(ret, "ForceXTRADownload");
  // }
  
  ret = SetServiceAutomaticTracking(1);
  if (ret)
  {
    fail(ret, "SetServiceAutomaticTracking");
  }
         
  ret = SetPortAutomaticTracking(1);
  if (ret)
  {
    fail(ret, "SetPortAutomaticTracking");
  }
         
  ret = SetPDSState(1);
  if (ret)
  {
    fail(ret, "SetPDSState");
  }

  ret = QCWWANDisconnect();
  if (ret)
  {
    fail(ret, "QCWWANDisconnect");
  }

  return 0;
}

