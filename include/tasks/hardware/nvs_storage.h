#ifndef _NVS_STORAGE_H
#define _NVS_STORAGE_H

void PerformNvsElectricity(bool read_write);
void PerformNvsFeatureParamter(bool read_write);
void PerformNvsPostInit(void *param);
void PerformNvsVerifiedUser(bool read_write);
void PerformNvsDeviceConfigParamter(bool read_write);
void PerformNvsNetworkParamter(bool read_write);
void PerformNvsFramParameter(bool read_write);
void PerformNvsHardwareDetail(bool read_write);
void PerformNvsFactoryReset(void *param);
void PerformNvsMnt(void *param);
void NvsInit(void *param);
void PrintUserConf(void *param);

#endif