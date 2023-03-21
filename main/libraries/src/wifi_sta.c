/**
 * @file wifi_sta.c
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
#include "esp_system.h"

char *pass ="Hamza";

/**
 * @brief
 *
 * @param param
 */
void configure_sta(void *param)
{

    wifi_config_t wifi_config_dummy = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false},
        },
    };
    sprintf((char *)wifi_config_dummy.sta.ssid,"%s",wifi_config.sta_ssid);
    sprintf((char *)wifi_config_dummy.sta.password,"%s",(char *)wifi_config.sta_pass);

    esp_wifi_set_config(WIFI_IF_STA, &wifi_config_dummy);
}
