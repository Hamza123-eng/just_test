#include <string.h>

#include "esp_wifi.h"
#include "esp_log.h"

#include "cJSON.h"
#include "scan.h"
#include "http_server.h"

#define DEFAULT_SCAN_LIST_SIZE 10

static const char *TAG = "scan";

wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];

uint16_t ap_count = 0;
cJSON *scan_info; // create empty JSON packet

static void CreateScanJsonPacket(char *pscan_list)
{
  scan_info = cJSON_CreateObject(); // create empty JSON packet

  cJSON *ssid = NULL;
  cJSON *rssi = NULL;
  cJSON *json_array = cJSON_CreateArray();

  cJSON_AddItemToObject(scan_info, "scan", json_array); // link empty array with json packet

  for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++)
  {
    cJSON *json_array_object = cJSON_CreateObject();
    cJSON_AddItemToArray(json_array, json_array_object); // put above two objects in an array

    ssid = cJSON_CreateString((char *)ap_info[i].ssid);
    rssi = cJSON_CreateNumber(ap_info[i].rssi);

    cJSON_AddItemToObject(json_array_object, "ssid", ssid); // link empty array with json packet
    cJSON_AddItemToObject(json_array_object, "rssi", rssi); // link empty array with json packet
  }
  char *print_packet = cJSON_Print(scan_info);
  sprintf(pscan_list, "%s", print_packet);
  free(print_packet);

  cJSON_Delete(scan_info);

  ESP_LOGD("str", " %s", pscan_list);
}

void WiFiScan(char *pscan_list)
{
  bool re_ena = false;

  if (recover_network_enable)
  {
    re_ena = true;
    recover_network_enable = false;
  }
  esp_wifi_disconnect(); // call it otherwise scan does not work

  uint16_t number = DEFAULT_SCAN_LIST_SIZE;

  memset(ap_info, 0, sizeof(ap_info));

  esp_wifi_scan_start(NULL, true); // scan for all APs, NULL means defualt configurations, true means code will block here until completion of scan

  esp_wifi_scan_get_ap_records(&number, ap_info);
  esp_wifi_scan_get_ap_num(&ap_count);

  ESP_LOGI(TAG, "Total APs scanned = %u", ap_count);

  CreateScanJsonPacket(pscan_list);
  
  if (re_ena)
  {
    esp_wifi_connect(); // call it to enable reconnect otherwise device will not connect again
    recover_network_enable = true;
  }
}
