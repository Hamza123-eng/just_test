#ifndef CUSTOM_WIFIMANGER_
#define CUSTOM_WIFIMANGER_

/*Wifi String*/
#include "Arduino.h"

/*Going to End Here*/


void WifiManagerInit(void *param);
bool  CheckSetUpStatus();

enum SystemSatate{

          kIdleState=0,
          kUninitilized,
          kWifiInitilized,

};

bool TryConnection(void *param);

void readSenderIDs();
void writeSenderIDs();

void WifiManagerInit(void *param);
void setWifiConfiguration(bool status, std::string password, std::string ssid);

#endif