#ifndef WIFI_MANAGER_H_
#define WIFI_MANGER_H_

#include "stdint.h"
#include "wifi_platform.h"

#define WIFI_MAX_TRIES 5
//  typedef enum
//  {
//     AP,
//     APSTA,
//     STA

//  }wifi_mode_t;

bool IsNetworkAvailable(void *param);
bool IsSystemNetworkAvailable(void *param);

void InitWifiManger(void *param);

extern e_wifi_mode current_wifi_mode;


#endif