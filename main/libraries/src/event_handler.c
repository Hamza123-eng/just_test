/**
 * @file event_handler.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-10-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "wifi_platform.h"


#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
// #include "esp_mac.h"
#include "esp_log.h"



void wifi_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    uint8_t id=0;

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        id= STA_NOT_READY;
        

    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        // ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        // ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        // // xEventGroupSetBits(e_wifi_event_group, STA_READY);
         id= STA_READY;
       // s_retry_num = 0;
      //  xEventGroupSetBits(e_wifi_event_group, WIFI_CONNECTED_BIT);
    }

    else if (event_id == WIFI_EVENT_AP_STACONNECTED)
        {
        // xEventGroupSetBits(e_wifi_event_group, AP_STA_READY);

        // wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        // ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
        //          MAC2STR(event->mac), event->aid);
       // ESP_LOGE("WIFI MAIN []","CONNECTED");
        id= AP_STA_READY;

    }
    else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
    {
        // xEventGroupClearBits(e_wifi_event_group, AP_STA_READY);

        // wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        // ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
        //          MAC2STR(event->mac), event->aid);
        //ESP_LOGE("WIFI MAIN []","DISCONNETED");

        id= AP_STA_NOT_READY;

    }

    else if (event_id == WIFI_EVENT_AP_START)
    {
        // xEventGroupSetBits(e_wifi_event_group, AP_READY);
        id= AP_READY;

    }

    else if (event_id == WIFI_EVENT_AP_STOP)
    {
        // xEventGroupClearBits(e_wifi_event_group, AP_NOT_READY);
        id= AP_NOT_READY;

    }

    else
    {
    }


    wifi_callback(id);

}
