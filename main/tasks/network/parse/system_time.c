// /**
//  * @file system_time.c
//  * @author Hamxa Islam {hamzaislam170@gamil.com}
//  * @brief Remotewell_PRO
//  * @version 0.1
//  * @date 2022-11-08
//  *
//  * @ copyright --->  EPTeck Tecnologies Gmbh   2022
//  *
//  */

// #include "stdlib.h"
// #include "stdio.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include <time.h>
#include <sys/time.h>
#include "esp_log.h"

#include "esp_system.h"
#include "esp_sntp.h"

#include "system_time.h"
#include "freertos_handler.h"
#include "wifi_manger.h"

#include "parse_event_controller.h"
#include "hw_monitor.h"
#include "parse.h"
#include "cJSON.h"

TaskHandle_t xTimeManagerTask;

static char *TAG = "TIME []";

bool time_updated = false;

bool time_try_completed = false;

/**
 * @brief Remotewell_PRO
 *
 * @param tv
 */

void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Notification of a time synchronization event");
}

/**
 * @brief
 *
 */
static void initialize_sntp(void)
{
    ESP_LOGI(TAG, "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "time.google.com");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}
/**
 * @brief Remotewell_PRO
 *
 */
void obtain_time()
{
    char *data = NULL;
    time_t now = 0;
    uint8_t retry = 0;
    uint8_t status = 0;
    const uint8_t retry_count = 10;
    static bool sntp_restart = false;
    uint64_t uinx_time = 0;
    uint8_t retry_online = 0;

    if (!sntp_restart)
    {
        initialize_sntp();
        sntp_restart = true;
    }
    else
    {
        // CXX COMPILANCE
    }

    /** parse Local network data*/
    while (status != 200)
    {
        /* code */
        if (IsNetworkAvailable(NULL))
        {
            GetNetworkLock(NULL);
            data = HttpPerform("http://worldtimeapi.org/api/timezone/Asia/Karachi", GET, &status, NULL, NULL);
            if (status != 200)
            {
                data != NULL ? free(data) : 1;
            }
            ReturnNetworkLock(NULL);
        }

        // this is wrong process of trap

        if (retry_online > 8)
        {
            time_try_completed = true;
        }
        else
        {
            retry_online++;
        }

        vTaskDelay(1500 / portTICK_PERIOD_MS);
    }

    cJSON *root = cJSON_Parse(data);
    uinx_time = cJSON_GetObjectItem(root, "unixtime")->valueint;
    sntp_set_system_time(uinx_time, 0);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    cJSON_Delete(root);
    data != NULL ? free(data) : 1;

    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count)
    {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        if (retry_count >= 9)
        {
            sntp_restart = true;
            time_try_completed = true;
        }
    }
    /*Only for Time Notion*/
    if (retry < 9)
    {
        time_updated = true;
    }
    time(&now);
}

/**
 * @brief Remotewell_PRO
 *
 * @param sync_time
 * @return uint64_t
 */
uint64_t GetTime(bool sync_time)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    // Is time set? If not, tm_year will be (1970 - 1900).

    if (timeinfo.tm_year < (2016 - 1900) || sync_time)
    {
        ESP_LOGI(TAG, "Time is not set yet. Connecting to WiFi and getting time over NTP.");
        obtain_time();
        time(&now);
        setenv("TZ", "GMT-5", 1);
        tzset();
    }

    char strftime_buf[64];
    localtime_r(&now, &timeinfo);
    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
    ESP_LOGI(TAG, "The current date/time in Islamabad is: %s", strftime_buf);

    if (sntp_get_sync_mode() == SNTP_SYNC_MODE_SMOOTH)
    {
        struct timeval outdelta;
        while (sntp_get_sync_status() == SNTP_SYNC_STATUS_IN_PROGRESS)
        {
            adjtime(NULL, &outdelta);
            ESP_LOGI(TAG, "Waiting for adjusting time ... outdelta = %li sec: %li ms: %li us",
                     outdelta.tv_sec,
                     outdelta.tv_usec / 1000,
                     outdelta.tv_usec % 1000);
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
    }
    return (now);
}

/**
 * @brief Remotewell_PRO
 *
 * @param param
 * @return true
 * @return false
 */
bool TimeIsAccurate(void *param)
{
    return (1667895185 < GetStandardTime(false)) && time_updated ? true : false;
}

uint64_t GetStandardTime(bool sync_time)
{
    uint8_t tries = 8; //** no of tries to block the task on time

    if (!time_updated && !time_try_completed)
    {
        while (tries)
        {
            vTaskDelay(2000 / portTICK_PERIOD_MS);
            if (time_updated)
            {
                return GetTime(false);
            }
            tries--;
        }
    }
    else if (time_updated)
    {
        return GetTime(false);
    }
    else
    {
        return 0;
    }
    return 0;
}

void TimeManger(void *param)
{
    bool status_required = 1;
    bool first_time_iteration = true;
    vTaskDelay(4000 / portTICK_PERIOD_MS);

    while (1)
    {
        if (!time_updated && first_time_iteration && network_config.verified_user)
        {
            GetTime(true);
            first_time_iteration = false;
        }
        else if (!time_updated && IsNetworkAvailable(NULL))
        {
            GetTime(true);
        }
        else
        {
            // CXX compilence
        }

        if (!IsNetworkAvailable(NULL))
        {
            status_required = true;
        }
        else if (status_required)
        {
            status_required = false;
            parse_event_send_t data_to_send = {.class_type = LIVE_CLASS,
                                               .history_button_status = 0,
                                               .system_status = WrapSystemStatus(NULL),
                                               .time = 0,
                                               .run_time = 0,
                                               .tag = "",
                                               .phone_tag = ""};
            if (xParseQueue != NULL)
                xQueueSendToBack(xParseQueue,
                                 (void *)&data_to_send,
                                 (TickType_t)10);
        }
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}
/**
 * @brief
 *
 * @param param
 */
void InitTimeManger(void *param)
{
    xTaskCreatePinnedToCore(TimeManger, "time_manger_task", TIMEMANGER_TASK_STACK_SIZE, NULL, TIMEMANGER_TASK_PRIO, &xTimeManagerTask, TIMEMANGER_TASK_CORE);
}