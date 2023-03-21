/**
 * @file wifi_app.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-10-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "wifi_platform.h"

#include "esp_wifi.h"
#include "lwip/sys.h"
#include "esp_system.h"

void configure_app(void *param)
{
    wifi_config_t wifi_config_dummy = {
        .ap = {
            .max_connection = 2,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
             .channel=11,
        },
    };
    

    sprintf((char *)wifi_config_dummy.ap.ssid, "%s", (char *)wifi_config.app_ssid);
    sprintf((char *)wifi_config_dummy.ap.password, "%s", (char *)wifi_config.app_pass);

    wifi_config_dummy.ap.ssid_len = strlen((char *)wifi_config.app_ssid);

    esp_wifi_set_config(WIFI_IF_AP, &wifi_config_dummy);
}
