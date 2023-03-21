
#include "driver/gpio.h"

#include "system_gpio.h"
#include "mode_button.h"
#include "freertos_handler.h"
#include "hw_monitor.h"
#include "wifi_manger.h"
#include "nvs_storage.h"
#include "parse.h"
#include "indications.h"

TaskHandle_t xButtonTask = NULL;
TaskHandle_t xAppModeOffTask = NULL;

static char *TAG = "MODE BUTTON[]";

bool ap_mode_enable = false;

e_wifi_mode prev_wifi_mode = NONE;
/**
 * @brief
 *
 * @param param
 */
void ApModeOffTask(void *param)
{
    vTaskDelay(5 * 60 * 1000 / portTICK_PERIOD_MS);
    xQueueSendToBack(xWifiQueue, &(prev_wifi_mode), (TickType_t)100);
    ap_mode_enable = !ap_mode_enable;
    vTaskDelete(NULL);
}

void DecideUserMode(uint16_t press_time)
{
    if (press_time >= TIME_MULTIPLE * 3)
    {
        /*ERASE FLASH FROM HERE*/
        PerformNvsFactoryReset(NULL);
        ESP_LOGI(TAG, "EEPROM  IS PEROFRMED");
        vTaskDelay(10);
        esp_restart();
    }
    else if (press_time >= TIME_MULTIPLE * 2)
    {
        /* APP MODE HERE */
        ESP_LOGI(TAG, "AP  IS PEROFRMED");
        ap_mode_enable = !ap_mode_enable; // Change the state of apmode
        /*< This Task is used to ensure the the 5 min apmode >*/
        if (ap_mode_enable)
        {
            prev_wifi_mode = current_wifi_mode;
            e_wifi_mode event_to_send = AP;

            if (xWifiQueue != NULL)
                xQueueSendToBack(xWifiQueue, &(event_to_send), (TickType_t)100);

            xTaskCreatePinnedToCore(ApModeOffTask, "ap_mode_off_task",
                                    512, NULL,
                                    3, &xAppModeOffTask,
                                    0);
        }
        else
        {
            if (xWifiQueue != NULL)
                xQueueSendToBack(xWifiQueue, &(prev_wifi_mode), (TickType_t)100);

            prev_wifi_mode = NONE;
            vTaskDelete(xAppModeOffTask);
        }
    }
    else if (press_time >= TIME_MULTIPLE * 1)
    {
        ESP_LOGI(TAG, "MANITNANCE I PERFORMED");
        /*Just change the status of Maintinance*/
        if (network_config.verified_user)
        {
            /*only verified user approach here*/
            live_class.undermnt = !live_class.undermnt;
            PerformNvsMnt(NULL);
        }
    }
    else
    {
        /* CXX COMPLIENCE */
    }
}

void ButtonTask(void *param)
{
    bool press = false;
    while (1)
    {
        press = gpio_get_level(MULTI_BUTTON_PIN);

        if (press == BUTTON_POLARITY)
        {
            buzzer_beep_t event_send = NONE;

            if (xBuzzerQueue != NULL)
                xQueueSendToBack(xBuzzerQueue, &(event_send), (TickType_t)10);

            uint8_t count = 0;
            while (1)
            {
                count++;
                vTaskDelay(1000 / portTICK_PERIOD_MS);

                press = gpio_get_level(MULTI_BUTTON_PIN);

                if (count < (TIME_MULTIPLE * 3 + 2) && ((count % TIME_MULTIPLE) == 0))
                {
                    event_send = MULTI_BUTTON;

                    if (xBuzzerQueue != NULL)
                        xQueueSendToBack(xBuzzerQueue, &(event_send), (TickType_t)10);

                    event_send = NONE;

                    if (xBuzzerQueue != NULL)
                        xQueueSendToBack(xBuzzerQueue, &(event_send), (TickType_t)10);
                }
                if (press != BUTTON_POLARITY)
                {
                    (count >= 3) ? DecideUserMode(count) : 1;
                    break;
                }
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void InitModeButton(void *param)
{
    /* Some Thing need to initilize here */

    xTaskCreatePinnedToCore(ButtonTask,
                            "hlw_event_task",
                            BUTTON_TASK_STACK_SIZE,
                            NULL,
                            BUTTON_TASK_PRIO,
                            &xHlwTask,
                            BUTTON_TASK_CORE);
}