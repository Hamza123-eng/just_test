
#include "freertos_handler.h"
#include "wifi_platform.h"
#include "wifi_manger.h"
#include "indications.h"
#include "parse.h"
#include "pcf.h"
#include "http_server.h"
#include "dns_ping.h"

TaskHandle_t xWifiTask = NULL;
QueueHandle_t xWifiQueue = NULL;
EventGroupHandle_t xWifiEvent = NULL;

e_wifi_mode current_wifi_mode = NONE;

static const char *TAG = "WIFI DRIVER []:";

uint8_t wifi_tries_count = 0;
uint8_t connected_user = 0;

bool recover_network_enable = true;
bool http_server_running = false;
/**
 * @brief
 *
 */
void ReadMacSTA()
{
    // Get the base MAC address from different sources
    uint8_t base_mac_addr[6] = {0};
    esp_err_t ret = ESP_OK;
    // Get base MAC address from EFUSE BLK3
    ret = esp_efuse_mac_get_custom(base_mac_addr);

    //     if (ret != ESP_OK)
    //     {
    //         ESP_LOGW(TAG, "Failed to get base MAC address from EFUSE BLK3. (%s)", esp_err_to_name(ret));
    // #ifdef CONFIG_BASE_MAC_STORED_EFUSE_BLK3_ERROR_ABORT
    //         ESP_LOGE(TAG, "Aborting");
    //         abort();
    // #else
    //         ESP_LOGD(TAG, "Defaulting to base MAC address in BLK0 of EFUSE");
    //         esp_efuse_mac_get_default(base_mac_addr);
    //         ESP_LOGD(TAG, "Base MAC Address read from EFUSE BLK0");
    // #endif // CONFIG_BASE_MAC_STORED_EFUSE_BLK3_ERROR_ABORT
    //     }
    //     else
    //     {
    //         ESP_LOGD(TAG, "Base MAC Address read from EFUSE BLK3");
    // }
    // Set the base MAC address using the retrieved MAC address
    esp_base_mac_addr_set(base_mac_addr);
    // Get the derived MAC address for each network interface
    uint8_t derived_mac_addr[6] = {0};
    // Get MAC address for WiFi Station interface
    ESP_ERROR_CHECK(esp_read_mac(derived_mac_addr, ESP_MAC_WIFI_STA));

    ESP_LOGD("WIFI_STA MAC", "%x%x%x%x%x%x",
             derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
             derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);

    sprintf(parse.mac_address, "%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", derived_mac_addr[0], derived_mac_addr[1], derived_mac_addr[2],
            derived_mac_addr[3], derived_mac_addr[4], derived_mac_addr[5]);

    ESP_LOGI(TAG, "MAC %s", parse.mac_address);
}
/**
 * @brief
 *
 * @param param
 */

void RefreshData(void *param)
{
    /*< feed schedule to fetch the  data>*/
}

/**
 * @brief Remotewell_PRO
 *
 * @param id
 */
void WifiUserCallBack(uint8_t id)
{
    /**
     * @brief Note Here we also have to take care of WIFI LED & BUZZER for visual indication.
     *
     */
    wifi_led_indication_t event_received = NONE;
    buzzer_beep_t event_received_buzzer = NONE;
    switch (id)
    {
    case STA_READY:
        ESP_LOGI(TAG, "-----STA READY----");

        ReadMacSTA();

        event_received = STA_CONNECTED;
        xEventGroupSetBits(xWifiEvent, STA_READY);
        wifi_tries_count = 0;

        e_wifi_mode dummy = STA;
        (current_wifi_mode == APSTA) ? xQueueSendToBack(xWifiQueue, &(dummy), (TickType_t)100) : 1;
        (current_wifi_mode != APSTA && current_wifi_mode != AP) ? xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)10) : 1;

        break;
    case STA_NOT_READY:
        ESP_LOGI(TAG, "-----STA DISCONNECT----");
        xEventGroupClearBits(xWifiEvent, STA_READY);
        (wifi_tries_count < WIFI_MAX_TRIES) ? wifi_tries_count++ : 1;
        vTaskDelay(20);
        recover_network_enable ? recover_network(NULL) : 1;
        break;
    case AP_READY:
        /* Special case here buzzer alo need to beep */
        ESP_LOGI(TAG, "-----AP MODE ENABLE-----");
        event_received = APP_MODE_ENABLE;
        event_received_buzzer = AP_STA_BUZZER_ENABLE;

        xEventGroupSetBits(xWifiEvent, AP_READY);
        /* Force enable by user */
        (current_wifi_mode != APSTA) ? xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)10) : 1;
        /* APSTA need the wifi led and buzzer also*/
        event_received = AP_STA_ENABLE;
        (current_wifi_mode == APSTA) ? xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)10) : 1;
        (current_wifi_mode == APSTA) ? xQueueSendToBack(xBuzzerQueue, &(event_received_buzzer), (TickType_t)10) : 1;
        break;
    case AP_NOT_READY:
        /* Note Ap to disable the beep */
        ESP_LOGI(TAG, "-----AP MODE DISABLE-----");
        event_received_buzzer = AP_STA_BUZZER_DISABLE;
        xQueueSendToBack(xBuzzerQueue, &(event_received_buzzer), (TickType_t)10);
        xEventGroupClearBits(xWifiEvent, AP_READY);
        break;
    case AP_STA_READY:
        ESP_LOGI(TAG, "-----USER CONNECTED-----");
        connected_user++;
        event_received = USER_CONNECTED;
        xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)10);
        break;
    case AP_STA_NOT_READY:
        ESP_LOGI(TAG, "-----USER DISCONECTED-----");
        event_received = AP_STA_ENABLE;
        connected_user--;
        printf("CURRENT MODE WIFIF :%u  : CONNECTED USER : %u\n", current_wifi_mode, connected_user);
        (current_wifi_mode == APSTA && !connected_user) ? xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)10) : 1;
        event_received = APP_MODE_ENABLE;
        (current_wifi_mode == AP && !connected_user) ? xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)10) : 1;
        break;

    default:
        break;
    }
}
/**
 * @brief Remotewell_PRO
 *
 * @param event
 */
void PerformWifiMode(e_wifi_mode event)
{
    wifi_led_indication_t event_received = NONE;
    switch (event)
    {
    case STA:
        recover_network_enable = true;

        wifi_start_sta(NULL);
        current_wifi_mode = STA;
        /*Shutdown the server*/
        if (http_server_running)
        {
            Shutdown_server_service(NULL);
            http_server_running = 0;
        }
        break;
    case AP:
        recover_network_enable = false;
        wifi_start_apsta(NULL);
        newtwork_disconnect(NULL);  //if sta get connected by prev passcode 
        current_wifi_mode = AP;
        if (!http_server_running)
        {
            start_webserver();
            http_server_running = 1;
        }
        break;
    case APSTA:
        recover_network_enable = true;
        wifi_start_apsta(NULL);
        current_wifi_mode = APSTA;

        event_received = AP_STA_ENABLE;
        xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)10);
        /*Turn on the server*/
        if (!http_server_running)
        {
            start_webserver();
            http_server_running = 1;
        }
        break;
    case NONE:
        event_received = NONE;
        xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)2);

        wifi_start_none(NULL);
        current_wifi_mode = NONE;
        break;
    default:
        break;
    }
}
/**
 * 
*/
bool IsSystemNetworkAvailable(void *param)
{
    return xEventGroupGetBits(xWifiEvent) & STA_READY ? true : false;
}
/**
 * @brief Remotewell_PRO
 *
 * @param param
 * @return true
 * @return false
 */
bool IsNetworkAvailable(void *param)
{
    if((xEventGroupGetBits(xWifiEvent) & STA_READY) && dns_network_status)
    {
        return true;
    }
    return false;
}
/**
 * @brief Remotewell_PRO
 *
 * @param param
 */
void WifiTask(void *param)
{
    ESP_LOGI(TAG, "WIFI DRIVER GOT INITILIZED");

    e_wifi_mode event_received = NONE;

    /*<  if SSID IS availble then go with out STA>*/

    if (strcmp(network_config.ssid1, "") && strcmp(network_config.user_name, ""))
    { /* Wifi is now able to go into STA section */
        event_received = STA;
        /*< Starting the STA MODE HERE >*/
        xQueueSendToBack(xWifiQueue, &(event_received), (TickType_t)100);
    }
    else
    {
        ESP_LOGE(TAG, "---NO USER NAME AND PASSCODE---");
        /* PCF MODE AS A FACTORY RESET */

        /*< Starting the STA MODE HERE >*/
        event_received = AP;
        xQueueSendToBack(xWifiQueue, &(event_received), (TickType_t)100);
    }

    while (1)
    {
        if (xQueueReceive(xWifiQueue, &(event_received), (TickType_t)100) == pdPASS)
        {
            PerformWifiMode(event_received);
        }
        /* < GOING for Count check > */

        if (wifi_tries_count >= WIFI_MAX_TRIES && current_wifi_mode == STA)
        {
            event_received = APSTA;
            xQueueSendToBack(xWifiQueue, &(event_received), (TickType_t)100);
        }
        vTaskDelay(10);
    }
}
void InitWifiDriver(void *param)
{
    ESP_LOGI(TAG, "--- NETWORK SSID :%s ---", network_config.ssid1);
    ESP_LOGI(TAG, "--- NETWORK PASS :%s ---", network_config.pswd1);

    e_wifi_config config = {0x00};

    sprintf((char *)config.sta_ssid, "%s", network_config.ssid1);
    sprintf((char *)config.sta_pass, "%s", network_config.pswd1);
    //  sprintf((char *)config.sta_ssid, "%s", "hamza");
    // sprintf((char *)config.sta_ssid, "%s", "hamza123");

    sprintf((char *)config.app_ssid, "%s", "Remotewell-Pro");
    sprintf((char *)config.app_pass, "%s", "remotewell");

    config.callback = &WifiUserCallBack;
    config.wifi_event_group = NULL;

    wifi_configure(&config);

    wifi_int(NULL);
}
/**
 * @brief Remotewell_PRO
 *
 * @param param
 */
void InitWifiManger(void *param)
{
    /*< Initilization of Wifi Mandatory >*/

    xEventGroupClearBits(xWifiEvent, 0xff);

    InitWifiDriver(NULL);

    xTaskCreatePinnedToCore(WifiTask, "wifi_handling_task", WIFI_TASK_STACK_SIZE, NULL, WIFI_TASK_PRIO, &xWifiTask, WIFI_TASK_CORE);
}
