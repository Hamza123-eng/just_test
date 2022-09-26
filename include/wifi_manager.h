#ifndef CUSTOM_WIFIMANGER_
#define CUSTOM_WIFIMANGER_

/*Wifi String*/


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

#endif