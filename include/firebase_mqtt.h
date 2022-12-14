#ifndef CUSTOM_FIREBASE_
#define CUSTOM_FIREBASE_

#include "Arduino.h"
#include "freertos/queue.h"
#include "freertos/task.h"

// #include <Firebase_ESP_Client.h>
extern TaskHandle_t xTaskFireBase;
extern bool firebase_status;
extern QueueHandle_t xQueueFireBase;

 extern class fb_esp_auth_signin_provider_t auth;
 extern class fb_esp_cfg_t config;

extern class CloudIoTCoreDevice *device;
extern class CloudIoTCoreMqtt *mqtt;
extern class MQTTClient *mqttClient;

extern String blue_sender_uuid;
extern String green_sender_uuid;
extern String orange_sender_uuid ;
extern String purple_sender_uuid;
extern String red_sender_uuid;
extern String yellow_sender_uuid;

extern RTC_DATA_ATTR uint8_t blue_count ;
extern RTC_DATA_ATTR uint8_t green_count ;
extern RTC_DATA_ATTR uint8_t orange_count ;
extern RTC_DATA_ATTR uint8_t purple_count ;
extern RTC_DATA_ATTR uint8_t red_count ;
extern RTC_DATA_ATTR uint8_t yellow_count ;
extern RTC_DATA_ATTR bool count_changed ;

extern char *project_id ;
extern  char *location ;
extern  char *registry_id ;

extern  char *ntp_primary ;
extern char *ntp_secondary ;

extern  int ex_num_topics;
extern  char *ex_topics;

// RTC_DATA_ATTR std::string private_key_str = "UNSET";
// RTC_DATA_ATTR std::string device_id = "UNSET";

const int jwt_exp_secs = 60 * 20;

void FireBaseInit(void *param);
void sendPlayCounts();

bool publishTelemetry(String subfolder, String data);
bool publishTelemetry(String data);
/*Certificate is here*/

extern uint8_t wifi_entry_index;

extern bool wifi_audio;

#endif