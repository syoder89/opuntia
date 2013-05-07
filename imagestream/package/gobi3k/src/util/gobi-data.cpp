#include "StdAfx.h"
#include "GobiError.h"
#include "GobiConnectionMgmtAPI.h"
#include "common.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <iostream>

int main()
{
  ULONG ret;
  connect_unlock();

printf("APN: %s\n", APN);
  ret = SetDefaultProfile(0, NULL, NULL, NULL, NULL, NULL, NULL,
                          (char *)APN, (char *)"", (char *)"");
  if (ret)
  {
    fail(ret, "SetDefaultProfile");
  }   

  ULONG state;
  ret = GetSessionState(&state);
  if (ret)
  {
    fail(ret, "GetSessionState");
  }
  printf("State: %s\n",
         state == 1 ? "Disconnected" :
         state == 2 ? "Connected" :
         state == 3 ? "Suspended (not supported)" :
         state == 4 ? "Authenticating" :
         "Unknown");

  if (state != 2)
  {
    printf("Enabling enhanced autoconnect\n");
    ULONG foo = 1;
    ret = SetEnhancedAutoconnect(1, &foo);
    if (ret)
    {
      fail(ret, "SetEnhancedAutoconnect");
    }
  }
  
  ret = QCWWANDisconnect();
  if (ret)
  {
    fail(ret, "QCWWANDisconnect");
  }

  return 0;
}

