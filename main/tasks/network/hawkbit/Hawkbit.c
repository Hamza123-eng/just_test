

#include "string.h"
#include "stdlib.h"
#include "stdbool.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Hawkbit.h"
#include "HawkbitHtpp.h"
#include "HawkbitJson.h"
#include "esp_https_ota.h"
#include "esp_log.h"
#include "main.h"
#include "http_client.h"
#include "parse.h"
#include "wifi_manger.h"
#include "parse_websocket.h"
#include "hw_monitor.h"
#include "pcf.h"
#include "freertos_handler.h"
#include "indications.h"

static const char *TAG = "HAWKBIT";

TaskHandle_t xHawkbitTask = NULL;
http_header_t hawkbit_header = {0x00};

/**
 * @brief
 *
 */
void MotorIsRunning(void *param)
{
    while (live_class.motor_status)
    {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
/**
 * @brief Hawkbit Configration Parameter
 *
 */

void HawkbitRestart()
{
    vTaskDelay(60000 * 5 / portTICK_PERIOD_MS);
    esp_restart();
}
static void HawkbitTask(void *pvParameters);

static void HawkbitTask(void *pvParameters)
{
    static bool flag_log_in = true;
    int hawkbit_flag = 0;
    char *data;
    uint8_t status_code = 0;

    sprintf(hawkbit_header.key_1, "%s", "Authorization");
    while (1)
    {
        // printf("FREE HEAP : %d\n", esp_get_free_heap_size());
        if (flag_log_in)
        {
            LogInHawkbit();
            flag_log_in = false;
        }

        while (1)
        {
            // printf("-------------HAWk URL %s:",hawkbit.hawkbit_url);
            if (IsNetworkAvailable(NULL))
            {
                GetNetworkLock(NULL);
                data = HttpPerform(hawkbit.hawkbit_url, GET, &status_code, NULL, &hawkbit_header);
                ReturnNetworkLock(NULL);

                if (status_code == 200)
                {
                    sprintf(hawkbit.http_output_buffer, "%s", data);
                    data != NULL ? free(data) : 1;
                    break;
                }
                else
                {
                    data != NULL ? free(data) : 1;
                }
            }
            vTaskDelay(5000 / portTICK_PERIOD_MS);
        }

        if (GetSecurityToken())
        {
            memset(hawkbit.hawkbit_url, 0, HAWKBIT_URL_SIZE);
            sprintf(hawkbit.hawkbit_url, "%s%s%s%s%s%s",
                    "http://", CONFIG_HAWKBIT_SERVER_PATH,
                    "/", CONFIG_HAWKBIT_TENANTS,
                    "/controller/v1/", device_config.model_data);

            ESP_LOGI(TAG, "HAWKBIT URL:%s", hawkbit.hawkbit_url);

#ifdef HAWKBIT_AUTHENTICATION

            memset(hawkbit.hawkbit_authentication, 0, HAWKBIT_AUTHENTICATIN_TOKEN_SIZE);
            sprintf(hawkbit.hawkbit_authentication, "%s%s%s",
                    CONFIG_SECURITY_TOKEN_TYPE, " ",
                    hawkbit.security_token);

            sprintf(hawkbit_header.value_1, "%s", hawkbit.hawkbit_authentication);

            ESP_LOGD(TAG, "HAWKBIT AUTHENTICATION:%s", hawkbit.hawkbit_authentication);

#endif

            while (1)
            {
                //  printf("FREE HEAP : %d\n", esp_get_free_heap_size());
                flag_log_in = true;
                hawkbit_flag = HawkbitPolling(NULL);
                if (hawkbit_flag == 2)
                {
                    /* controller got unregistered*/
                    break;
                }

                if (hawkbit_flag)
                {
                    MotorIsRunning(NULL);
                    xTaskCreate(HawkbitRestart, "HawkbitRestart", 1024 * 1, NULL, 5, NULL);
                    GetBaseDeployment();
                    GetDeploymentLink();
                    while (1)
                    {
                        if (IsNetworkAvailable(NULL))
                            break;

                        vTaskDelay(3000 / portTICK_PERIOD_MS);
                    }
                    GetNetworkLock(NULL);
                    websocket_app_stop();
                    websocket_sch_stop();

                    pcf_mode_t mode = FIRMWARE_UPDATE;
                    xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);

                    if (esp_ota(hawkbit.artifact_link))
                    {
                        ESP_LOGI(TAG, "FILE DOWNLOADED SUCEESFULLY GOING TO SEND FEED BACK");
                        SendFeedBack();
                        ESP_LOGI(TAG, "ESP GOING TO RESTART AFTER 3s");
                        e_wifi_mode event_to_send = NONE;
                        xQueueSendToBack(xWifiQueue, &(event_to_send), (TickType_t)100);
                        buzzer_beep_t buzzer_event = FIRMWARE_COMPLETE;
                        xQueueSendToBack(xBuzzerQueue, &(buzzer_event), (TickType_t)10);
                        vTaskDelay(5000);
                        esp_restart();
                    }
                    else
                    {
                        esp_restart();
                        ESP_LOGI(TAG, "FIRMWARE DOWNLOAD FAIL TRYING AGIAN AFTER A WHILE");
                    }
                    ReturnNetworkLock(NULL);
                }
                else
                {
                    vTaskDelay(60000 * 2 / portTICK_PERIOD_MS);
                }
            }
        }
        else
        {
            ESP_LOGI(TAG, "CONTROLLER IS NOT RESGISTERED ON HAWKBIT");
            vTaskDelay(60000 * 2 / portTICK_PERIOD_MS);
        }
    }
}
void HawkbitStart()
{
    xTaskCreate(&HawkbitTask, "HawkbitTask", 1042 * 10, NULL, 5, &xHawkbitTask);
}
