/**
 * @file wifi_platform.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2022-10-20
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef _WIFI_PLATFORM_
#define _WIFI_PLATFORM_

#include "stdlib.h"
#include "stdint.h"
#include <stdio.h>
#include <string.h>
#include "stdbool.h"

#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "freertos/event_groups.h"
#include "lwip/err.h"
#include "lwip/sys.h"

/************************* Configuration Parameter ***********************/

/*STA MODE CONFIG*/

#define DRV_ESP_WIFI_SSID CONFIG_ESP_WIFI_SSID
#define DRV_ESP_WIFI_PASS CONFIG_ESP_WIFI_PASSWORD
#define DRV_ESP_MAXIMUM_RETRY CONFIG_ESP_MAXIMUM_RETRY

#define DRV_AP_SSID CONFIG_ESP_WIFI_SSID
#define DRV_AP_PASS CONFIG_ESP_WIFI_PASSWORD
#define DRV_AP_CHANNEL CONFIG_ESP_WIFI_CHANNEL
#define DRV_MAX_STA_CONN CONFIG_ESP_MAX_STA_CONN

/*:) Internal function for interface ------------------------------------------*/

typedef enum 
{
    NONE,
    STA,
    AP,
   APSTA
}e_wifi_mode;

typedef enum
{
    STA_NOT_READY = 0x01,
    STA_READY = 0x02,
    AP_NOT_READY = 0x04,
    AP_READY = 0x08,
    AP_STA_READY = 0x10,
    AP_STA_NOT_READY = 0x20
} e_wifi_event;

typedef struct
{
    uint8_t sta_ssid[32];
    uint8_t sta_pass[32];

    uint8_t app_ssid[32];
    uint8_t app_pass[32];

    void (*callback)(uint8_t id);
    EventGroupHandle_t *wifi_event_group;

} e_wifi_config;


void (*wifi_callback)(uint8_t id);

// void wifi_deinit(void *param);        // internal
void wifi_init_internal(void *param); // internal

void wifi_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);
void configure_sta(void *param); // internal
void configure_app(void *param); // internal

/*:) External function for interface ------------------------------------------*/

void wifi_configure(e_wifi_config *config);

void wifi_start_sta(void *param);
void wifi_start_app(void *param);
void wifi_start_apsta(void *param);
void wifi_start_none(void *param);
void recover_network(void *param);
void newtwork_disconnect(void *param);


void wifi_int(void *param);
void wifi_stop(void *param);

/********************************  NONE ************************************/

extern e_wifi_config wifi_config;
extern EventGroupHandle_t *e_wifi_event_group;

/****************************************************************************/

#endif // _WIFI_PLATFORM_H_
