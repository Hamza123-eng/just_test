#ifndef CUSTOM_SPIFFSSYSTEM_
#define CUSTOM_SPIFFSSYSTEM_

/*CUSTOM FUNCTION IS HERE*/
void SpiffsInit(void *param);

int getFirmwareVersion();
void writeNewFirmwareVersion(int newVersion);
void handleDeviceIOTInfo();


#endif