

#include "parse.h"

#include "esp_log.h"
#include "string.h"
#include "stdio.h"
#include "esp_system.h"

#include "http_client.h"
#include "indications.h"
#include "freertos_handler.h"
#include "manage_system.h"
#include "wifi_manger.h"
#include "hw_monitor.h"
#include "http_server.h"
#include "parse_event_controller.h"
#include "schedule_estimate.h"
#include "parse.h"
#include "pcf.h"
#include "phase_sensing.h"
#include "sdkconfig.h"

static const char *TAG = "SYSTEM MANGER []:";

void InitSystemTransport(void *param)
{
    xBuzzerQueue = xQueueCreate(2, sizeof(buzzer_beep_t));
    if (xBuzzerQueue == NULL)
        ESP_LOGE(TAG, "BUZZER QUEUE FAIL");

    xWifiLedQueue = xQueueCreate(2, sizeof(wifi_led_indication_t));
    if (xWifiLedQueue == NULL)
        ESP_LOGE(TAG, "WIFI QUEUE FAIL");

    xRelayQueue = xQueueCreate(2, sizeof(bool));
    if (xRelayQueue == NULL)
        ESP_LOGE(TAG, " GENERAL RELAY QUEUE FAIL");

    xVFDQueue = xQueueCreate(2, sizeof(bool));
    if (xVFDQueue == NULL)
        ESP_LOGE(TAG, "VFD QUEUE FAIL");

    xParseQueue = xQueueCreate(10, sizeof(parse_event_send_t));
    if (xParseQueue == NULL)
        ESP_LOGE(TAG, "PARSE EVENT QUEUE FAIL");

    xButtonQueue = xQueueCreate(3, sizeof(uint8_t));
    if (xButtonQueue == NULL)
        ESP_LOGE(TAG, "BUTTON QUEUE FAIL");

    xAutoModeQueue = xQueueCreate(1, sizeof(bool));
    if (xAutoModeQueue == NULL)
        ESP_LOGE(TAG, "AUTO MODE QUEUE FAIL");

    xScheduleInQueue = xQueueCreate(4, sizeof(schedule_event_t));
    if (xScheduleInQueue == NULL)
        ESP_LOGE(TAG, "SCHEDULE IN QUEUE FAIL");

    xScheduleOutQueue = xQueueCreate(1, sizeof(uint8_t));
    if (xScheduleOutQueue == NULL)
        ESP_LOGE(TAG, "SCHEUULE OUT QUEUE FAIL");

    xPcfQueue = xQueueCreate(2, sizeof(pcf_mode_t));
    if (xPcfQueue == NULL)
        ESP_LOGE(TAG, "PCF QUEUE FAIL");

    xWifiQueue = xQueueCreate(3, sizeof(e_wifi_mode));
    if (xVFDQueue == NULL)
        ESP_LOGE(TAG, "WIFI QUEUE FAIL");

    xPhaseEvent = xEventGroupCreate();
    if (xPhaseEvent == NULL)
        ESP_LOGE(TAG, "PHASE EVENT FAIL ");
    else
    {
        xEventGroupSetBits(xPhaseEvent, 0xff);          // first all thing is true
        xEventGroupClearBits(xPhaseEvent, PHASE_FAULT); // first false the phase fault
        !device_config.phase_fault ? xEventGroupClearBits(xPhaseEvent, PHASE_FAULT_ENABLE) : true;
    }

    xWifiEvent = xEventGroupCreate();
    if (xWifiEvent == NULL)
        ESP_LOGE(TAG, "WIFI EVENT FAIL ");
    else
        xEventGroupClearBits(xWifiEvent, 0xff);

    InitNetworkLock(NULL);
    ReturnNetworkLock(NULL);
}
// /**
//  * @brief
//  *
//  * @param param
//  */

void MangeUserAndNetwork(void *param)
{

    /*< first check if user is registered >*/
    if (!strcmp(device_config.hardware_version, "") || device_config.hardware_serial == 0)
    {
        pcf_mode_t mode = NO_SERIAL_NUMBER;
        xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);
        while (1)
        {
            ESP_LOGE(TAG,"---NO SERIAL NUMBER---");
            vTaskDelay(100);
        }
    }

    if (strcmp(network_config.user_name, ""))
    {
        
       

        /*< Now Create the Hardware Core Task>*/

        HardwareCoreInit(NULL);
        ParseControllerInit(NULL);

        ParseInitTask(NULL);
        ScheduleTaskInit(NULL);
        WebSocketInit();
    }

    else
    {
         pcf_mode_t mode = FACTORY_RESET_PCF;
        xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);
        while (1)
        {
            ESP_LOGE(TAG, "!!!!--- NO USER IS HERE....PLEASE REGISTERTED A USER ---!!!!");
            vTaskDelay(100);
        }
    }
}
