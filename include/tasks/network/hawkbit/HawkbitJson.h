#ifndef HAWKBIT_JSON_H_
#define HAWKBIT_JSON_H_

uint8_t  HawkbitPolling(void *param);
void GetBaseDeployment();
void GetDeploymentLink();
void SendFeedBack();
int GetSleepTime();
bool GetSecurityToken();
void ReadMacSTA1();
void LogInHawkbit();

#endif //HAWKBIT_JSON_H_