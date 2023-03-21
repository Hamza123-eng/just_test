
/**
 * @file wifi_platform.c
 * @author Hamxa Islam {hamzaislam170@gamil.com}
 * @brief 
 * @version 0.1
 * @date 2022-11-14
 * 
 * @ copyright --->  EPTeck Tecnologies Gmbh   2022
 * 
 */
#include "wifi_platform.h"

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#define SCAN_SIZE 7

EventGroupHandle_t *e_wifi_event_group = NULL;

e_wifi_config wifi_config;

e_wifi_mode mode_selected=NONE;
/**
 * @brief 
 * 
 * @param config 
 */
void wifi_configure(e_wifi_config *config)
{
    memcpy(&wifi_config,config, sizeof(e_wifi_config));

    wifi_callback = (config->callback);

    e_wifi_event_group = config->wifi_event_group;
}
/**
 * @brief
 *
 * @param param
 */

void wifi_start_none(void *param)
{
    esp_wifi_stop();
    esp_wifi_set_mode(WIFI_MODE_NULL);
    mode_selected=NONE;
}

/**
 * @brief
 *
 * @param param
 */

void wifi_start_sta(void *param)
{
    esp_wifi_stop();

    configure_sta(NULL);

    esp_wifi_set_mode(WIFI_MODE_STA);
    ESP_ERROR_CHECK(esp_wifi_start());
    mode_selected=STA;

}

/**
 * @brief
 *
 * @param param
 */

void wifi_start_app(void *param)
{
    esp_wifi_stop();

    configure_app(NULL);

    esp_wifi_set_mode(WIFI_MODE_AP);
    ESP_ERROR_CHECK(esp_wifi_start());
    mode_selected=AP;
}

/**
 * @brief
 *
 * @param param
 */

void wifi_start_apsta(void *param)
{
    esp_wifi_stop();

    configure_app(NULL);
    configure_sta(NULL);
//  esp_wifi_set_inactive_time(WIFI_IF_AP,3);
    esp_wifi_set_mode(WIFI_MODE_APSTA);
    ESP_ERROR_CHECK(esp_wifi_start());
    mode_selected=APSTA;
}

/**
 * @brief
 *
 * @param param
 */

void wifi_int(void *param)
{
    wifi_init_internal(NULL);
}

/**
 * @brief
 *
 * @param param
 */

void wifi_stop(void *param)
{
   // wifi_deinit(NULL);
}
/**
 * @brief
 *
 */
void wifi_json_doc(char *wifi_list_buffer, wifi_ap_record_t *app_info)
{
    /*< Here system is going to check the count of the wifi index >*/

    // uint16_t ap_count = 0;
    // esp_wifi_scan_get_ap_num(&ap_count);

    // cJSON *scan_info = cJSON_CreateObject(); // create empty JSON packet

    // cJSON *ssid = NULL;
    // cJSON *rssi = NULL;
    // cJSON *json_array = cJSON_CreateArray();

    // cJSON_AddItemToObject(scan_info, "scan", json_array); // link empty array with json packet

    // for (int i = 0; (i < SCAN_SIZE) && (i < ap_count); i++)
    // {
    //     cJSON *json_array_object = cJSON_CreateObject();
    //     cJSON_AddItemToArray(json_array, json_array_object); // put above two objects in an array

    //     ssid = cJSON_CreateString((char *)ap_info[i].ssid);
    //     rssi = cJSON_CreateNumber(ap_info[i].rssi);

    //     cJSON_AddItemToObject(json_array_object, "ssid", ssid); // link empty array with json packet
    //     cJSON_AddItemToObject(json_array_object, "rssi", rssi); // link empty array with json packet
    // }
    // char *print_packet = cJSON_Print(scan_info);
    // sprintf(wifi_list_buffer, "%s", print_packet);
    // free(print_packet);

    // cJSON_Delete(scan_info);

    // ESP_LOGD("str", " %s", pscan_list);
}

/**
 * @brief
 *
 */
void wifi_scan_list(char *wifi_list_buffer)
{
    // wifi_mode_t *mode;
    // uint16_t number = SCAN_SIZE;
    // wifi_ap_record_t ap_info[SCAN_SIZE];

    // esp_wifi_get_mode(mode);

    // switch (*mode)
    // {
    // case WIFI_MODE_NULL:
    //     break;
    // case WIFI_MODE_STA:
    //     /* code */
    //     esp_wifi_scan_start(NULL, true);
    //     esp_wifi_scan_get_ap_records(SCAN_SIZE, ap_info);
    //     break;
    // case WIFI_MODE_AP:
    //     /* code */
    //     break;
    // case WIFI_MODE_APSTA:
    //     /* code */
    //     esp_wifi_scan_start(NULL, true);
    //     esp_wifi_scan_get_ap_records(SCAN_SIZE, ap_info);
    //     break;

    // default:
    //     break;
    // }
}

void recover_network(void *param)
{
     if((mode_selected == APSTA) || (mode_selected == STA))
     {
        esp_wifi_connect();
     }
}
/**
 * @brief 
 * 
 * @param param 
 */
void newtwork_disconnect(void *param)
{
     
        esp_wifi_disconnect();
     
}