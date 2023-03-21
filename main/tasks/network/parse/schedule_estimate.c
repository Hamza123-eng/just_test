
// #include "esp_attr.h"
// #include <string.h>
// #include <stdlib.h>
// #include "freertos/FreeRTOS.h"
// #include "freertos/task.h"
// #include "esp_log.h"
// #include "esp_system.h"
// #include "nvs_flash.h"
// #include "esp_netif.h"
// #include "esp_tls.h"
// #include "esp_crt_bundle.h"
// #include "esp_http_client.h"
// #include <time.h>
#include "cJSON.h"
// #include "schedule_estimate.h"
// #include <stdio.h>

// #include "driver/gpio.h"
// #include <stdio.h>
// #include <stdbool.h>
// #include "hw_monitor.h"
// #include "delete.h"
#include "parse.h"
// #include "get.h"

#include "freertos_handler.h"
#include "wifi_manger.h"
#include "system_time.h"
#include "http_client.h"
#include "schedule_estimate.h"

#define NONE 0x00
#define NO_SCHEDULE 10

TaskHandle_t xScheduleTask = NULL;

QueueHandle_t xScheduleInQueue = NULL;
QueueHandle_t xScheduleOutQueue = NULL;

bool network_recover = false;

void SortSchedule(bool delete_perform);

static char *TAG = "SCHEDULE";

static parse_schedule_t schedule[NO_SCHEDULE] = {0x00};

scheduling_control_param_t schedule_control_param = {0, 0, false, true, false};

schedule_record_t current_schedule_record = {"", "", ""};
schedule_record_t last_schedule_record = {"", "", ""};

volatile bool previous_on = false;
volatile bool force_deleted = false;

bool loop_round_skip = false;

void ClearAllSchedule(void)
{
    for (uint8_t i = 0; i < 10; i++)
    {
        schedule[i].start_time = 0UL;
        schedule[i].stop_time = 0UL;
        memset(schedule[i].object_id, 0x00, 16);
        memset(schedule[i].phone_num, 0x00, 16);
        memset(schedule[i].tag, 0x00, 16);
    }
    schedule_control_param.selected_schedule = 0;
    schedule_event_t schedule_event = FETCH_SCHEDULE;
    xQueueReset(xScheduleInQueue);
    xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);

    schedule_control_param.schedule_ready_strobe = false;
    previous_on = false;
}
/**
 * @brief
 *
 * @param param
 */
void printcurrentschedule(void *param)
{
    for (uint8_t i = 0; i < schedule_control_param.total_schedule; i++)
    {
        printf("%i : satrt Time :%lli  stop Time :%lli    TAG : %s   PHONE NUM  :%s \n ", i, schedule[i].start_time, schedule[i].stop_time, schedule[i].tag,
               schedule[i].phone_num);
    }
    printf("***Selected schedule : %d  ***\n", schedule_control_param.selected_schedule);
}
/**
 * @brief
 *
 * @param param
 */
void UpdateScheduleStatus(void *param)
{
    char url[256] = {0x00};
    char data_to_send[64] = "{\"schedule_status\":true}";
    uint8_t status_code;
    GetNetworkLock(NULL);

    for (uint8_t index = 0; index < schedule_control_param.total_schedule; index++)
    {
        if (schedule[index].schedule_status != true)
        {
            sprintf(url, "%s%s%s", parse.parse_uri.schedule_uri, "/", schedule[index].object_id);
            // printf("URL FOR SCHEDULE : %s\n",url);
            // printf("DATA FOR SCHEDULE : %s\n",data_to_send);

            HttpPerform(url, PUT, &status_code, data_to_send, &parse_header);
        }
    }
    ReturnNetworkLock(NULL);
}

/**
 * @brief Remotewell_PRO
 * @param param
 * @return uint8_t
 */
uint8_t GetTopSchedule(void *param)
{
    uint64_t time = GetStandardTime(false);

    if (schedule_control_param.total_schedule)
    {
        for (uint8_t index = 0; index < schedule_control_param.total_schedule; index++)
        {
            if (time < schedule[index].stop_time)
                return index + 1;
        }
    }
    return 0;
}
/**
 * @brief
 *
 * @param ptr
 * @return long
 */
long ConvertParseTimeToUnix(char *ptr)
{
    struct tm result;
    memset(&result, 0, sizeof(struct tm));
    time_t epoch;
    strptime(ptr, "%Y-%m-%d %R", &result);
    epoch = mktime(&result);
    return (epoch);
}

// static void initialize_sntp(void)
// {
//     ESP_LOGI(TAG, "Initializing SNTP");
//     sntp_setoperatingmode(SNTP_OPMODE_POLL);
//     sntp_setservername(0, "time.google.com");
//     sntp_set_time_sync_notification_cb(time_sync_notification_cb);
//     sntp_init();
// }
/**
 * @brief Remotewell_PRO
 *
 * @param param
 */
static void DeleteExpireSchedule(void *param)
{

    uint64_t time = GetStandardTime(false);
    uint8_t status_code = 0;
    schedule_event_t event = NONE;

    GetNetworkLock(NULL);
    schedule_control_param.entertain_query_callback = false;

    if (schedule_control_param.total_schedule)
    {
        for (uint8_t i = 0; i < schedule_control_param.total_schedule; i++)
        {
            if (time >= schedule[i].stop_time)
            {
                char url[256];
                memset(url, 0, 256);
                sprintf(url, "%s%s%s", parse.parse_uri.schedule_uri, "/", schedule[i].object_id);
                HttpPerform(url, DELETE, &status_code, NULL, &parse_header);
                schedule[i].start_time = 0;
                schedule[i].stop_time = 0;
            }
        }
    }

    ReturnNetworkLock(NULL);

    SortSchedule(false);

    /*< Refetch command for New Ones>*/
    event = FETCH_SCHEDULE;
    (status_code == 200) ? xQueueSendToBack(xScheduleInQueue, &(event), (TickType_t)100) : 1;
    (status_code == 0) ? UpdateScheduleStatus(NULL) : 1;
    vTaskDelay(3000 / portTICK_PERIOD_MS);
    schedule_control_param.entertain_query_callback = true;
}
/**
 * @brief
 *
 */
bool CheckRunningSchedule()
{
    if (strcmp(current_schedule_record.schedule_id, ""))
    {

        for (int i = 0; i < schedule_control_param.total_schedule; i++)
        {
            if (!strcmp(schedule[i].object_id, current_schedule_record.schedule_id) && previous_on)
            {
                return true;
            }
        }
        sprintf(last_schedule_record.schedule_phone, "%s", schedule[schedule_control_param.selected_schedule - 1].phone_num);
        sprintf(last_schedule_record.schedule_tag, "%s", schedule[schedule_control_param.selected_schedule - 1].tag);
        sprintf(last_schedule_record.schedule_id, "%s", current_schedule_record.schedule_id);

        sprintf(current_schedule_record.schedule_phone, "%s", "");
        sprintf(current_schedule_record.schedule_tag, "%s", "");
        sprintf(current_schedule_record.schedule_id, "%s", "");
        printf("FORM 1 \n");
        uint8_t motor_data = 2;
        xQueueSendToBack(xScheduleOutQueue, &(motor_data), (TickType_t)100);
        previous_on = false;
    }
    return 0;
}
/**
 * @brief Remotewell_PRO
 *
 * @param output_buffer_schedule
 */
void FillSchedule(char *output_buffer_schedule)
{
    if (strlen(output_buffer_schedule) > 0)
    {
        schedule_control_param.total_schedule = 0;

        cJSON *schedule_parse;

        cJSON *root = cJSON_Parse(output_buffer_schedule);
        cJSON *array = cJSON_GetObjectItem(root, "results");
        uint8_t no_of_schedule = cJSON_GetArraySize(array);

        ESP_LOGI(TAG, "\nNo fo Schdeule=%d", no_of_schedule);

        no_of_schedule = (no_of_schedule > 10 ? 10 : no_of_schedule); // verification of number of schedule

        cJSON *sch_on_ptr, *sch_off_ptr, *obj_id_ptr, *phone_ptr, *tag_ptr, *schedule_st;

        /**    just do clear all schedule */

        for (uint8_t i = 0; i < 10; i++)
        {
            schedule[i].start_time = 0UL;
            schedule[i].stop_time = 0UL;
            memset(schedule[i].object_id, 0x00, 16);
            memset(schedule[i].phone_num, 0x00, 16);
            memset(schedule[i].tag, 0x00, 16);
        }

        for (uint8_t i = 0; i < no_of_schedule; i++)
        {
            schedule_parse = cJSON_GetArrayItem(array, i);

            sch_on_ptr = cJSON_GetObjectItem(schedule_parse, parse_data_name_array[schedule_on]);
            sch_off_ptr = cJSON_GetObjectItem(schedule_parse, parse_data_name_array[schedule_off]);
            obj_id_ptr = cJSON_GetObjectItem(schedule_parse, "objectId");
            phone_ptr = cJSON_GetObjectItem(schedule_parse, parse_data_name_array[phone_tag]);
            tag_ptr = cJSON_GetObjectItem(schedule_parse, parse_data_name_array[tag]);
            schedule_st = cJSON_GetObjectItem(schedule_parse, parse_data_name_array[schedule_status]);

            if (sch_on_ptr != NULL && cJSON_IsString(sch_on_ptr))
                schedule[i].start_time = ConvertParseTimeToUnix(sch_on_ptr->valuestring);
            if (sch_off_ptr != NULL && cJSON_IsString(sch_off_ptr))
                schedule[i].stop_time = ConvertParseTimeToUnix(sch_off_ptr->valuestring);
            if (obj_id_ptr != NULL && cJSON_IsString(obj_id_ptr))
                strcpy(schedule[i].object_id, obj_id_ptr->valuestring);
            if (phone_ptr != NULL && cJSON_IsString(phone_ptr))
                strcpy(schedule[i].phone_num, phone_ptr->valuestring);
            if (tag_ptr != NULL && cJSON_IsString(tag_ptr))
                strcpy(schedule[i].tag, tag_ptr->valuestring);
            if (schedule_st != NULL)
                schedule[i].schedule_status = schedule_st->valueint;
        }
        cJSON_Delete(root);

        schedule_control_param.total_schedule = no_of_schedule;

        CheckRunningSchedule();

        (schedule_control_param.total_schedule != 0) ? SortSchedule(true) : 1;

        // break;
    }
}
/**
 * @brief Remotewell_PRO
 *
 * @param param
 */
void SortSchedule(bool delete_perform)
{

    uint8_t i, j, min_idx;
    parse_schedule_t copy;
    // One by one move boundary of unsorted subarray
    if (schedule_control_param.total_schedule > 0)
    {
        for (i = 0; i < schedule_control_param.total_schedule - 1; i++)
        {
            // Find the minimum element in unsorted array

            min_idx = i;
            for (j = i + 1; j < schedule_control_param.total_schedule; j++)
                if (schedule[j].start_time < schedule[min_idx].start_time)
                    min_idx = j;

            // Swap the found minimum element
            // with the first element
            memcpy(&copy, &schedule[i], sizeof(parse_schedule_t));
            memcpy(&schedule[i], &schedule[min_idx], sizeof(parse_schedule_t));
            memcpy(&schedule[min_idx], &copy, sizeof(parse_schedule_t));

            memset(&copy, 0x00, sizeof(parse_schedule_t));
        }
    }
    if (delete_perform)
    {
        IsNetworkAvailable(NULL) ? DeleteExpireSchedule(NULL) : 1;
    }
}
bool ScheduleMotor(uint8_t schedule_num, bool on_check, uint64_t time)
{

    if (time >= schedule[schedule_num - 1].start_time && time < (schedule[schedule_num - 1].stop_time + 3) && on_check)
    {
        return true;
    }
    else if (time >= schedule[schedule_num - 1].stop_time && time <= (schedule[schedule_num - 1].stop_time + 10) && !on_check)
    {
        return true;
    }
    else
    {
        /*< CXX COMPLIENCE >*/
    }
    return false;
}
void PerformScheduleEvent(schedule_event_t event)
{
    switch (event)
    {
    case FETCH_SCHEDULE:
    {
        ESP_LOGE(TAG, "--- FETCH CURRENT SCHEDULE ----");

        uint8_t status_code = 0U;
        char *received_data = NULL;
        if (!IsNetworkAvailable(NULL))
        {
            xQueueSendToBack(xScheduleInQueue, &(event), (TickType_t)100);
            break;
        }
        GetNetworkLock(NULL);
        received_data = HttpPerform(parse.parse_uri.schedule_uri, GET, &status_code, NULL, &parse_header);
        ReturnNetworkLock(NULL);

        (status_code == 200) ? FillSchedule(received_data)
                             : xQueueSendToBack(xScheduleInQueue, &(event), (TickType_t)100);
        /*< Assert Ready Strobe>*/
        if (status_code == 200)
        {
            schedule_control_param.schedule_ready_strobe = true;
            schedule_control_param.selected_schedule = GetTopSchedule(NULL);
        }
        /*< Critical section !!! MEMORY LEAK IF NOT FREE OPERATION IS PERFORM !!!!>*/

        (received_data != NULL) ? free(received_data) : 1;
    }
    break;
    case DELETE_CURRENT_SCHEDULE:
    {
        schedule_run = false;
        ESP_LOGE(TAG, "--- DELETE CURRENT SCHEDULE ----");

        schedule_control_param.entertain_query_callback = false;

        /*< first of all Change the on time and of time value >*/

        schedule[schedule_control_param.selected_schedule - 1].start_time = 0;
        schedule[schedule_control_param.selected_schedule - 1].stop_time = 0;

        sprintf(last_schedule_record.schedule_phone, "%s", schedule[schedule_control_param.selected_schedule - 1].phone_num);
        sprintf(last_schedule_record.schedule_tag, "%s", schedule[schedule_control_param.selected_schedule - 1].tag);
        sprintf(last_schedule_record.schedule_id, "%s", current_schedule_record.schedule_id);

        sprintf(current_schedule_record.schedule_phone, "%s", "");
        sprintf(current_schedule_record.schedule_tag, "%s", "");
        sprintf(current_schedule_record.schedule_id, "%s", "");

        SortSchedule(true);

        schedule_control_param.selected_schedule = GetTopSchedule(NULL);
        previous_on = false;
        schedule_control_param.entertain_query_callback = true;

        schedule_event_t schedule_event = FETCH_SCHEDULE;
        xQueueReset(xScheduleInQueue);
        xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);
    }
    break;
    case SCHEDULE_HALT_BY_FAULT:
    {
        schedule_run = false;
        previous_on = false;
        sprintf(current_schedule_record.schedule_id, "%s", "");
    }
    break;
    case REFRESH_SCHEDULE:
    {
        if (force_deleted)
        {
            force_deleted = false;
        }
        loop_round_skip = true;
        schedule_event_t event = FETCH_SCHEDULE;
        xQueueReset(xScheduleInQueue);
        xQueueSendToBack(xScheduleInQueue, &(event), (TickType_t)100);
    }
    break;
    case HALT_SCHEDULE_OPERATION:
    {
    }
    break;
    case RESUME_SCHEDULE_OPERATION:
    {
    }
    break;

    default:
        break;
    }
}
static void ScheduleTask(void *pvParameters)
{
    // GetStandardTime(true);
    schedule_event_t schedule_event = NONE;

    while (1)
    {
        if (GetStandardTime(NULL) != 0)
        {
            break;
        }
        vTaskDelay(100);
    }

    /*< Just for the the Control purpose in future>*/

    while (1)
    {
        while (1)
        {
            if (!network_config.verified_user)
                break;
            if (xScheduleInQueue != NULL)
                (xQueueReceive(xScheduleInQueue, &(schedule_event), (TickType_t)10) == pdPASS) ? PerformScheduleEvent(schedule_event) : 1;
            printcurrentschedule(NULL);
            /*< reasonable schedule to entertain >*/
            if (loop_round_skip)
            {
                goto END;
            }

            if (schedule_control_param.selected_schedule > 0)
            {
                if (!previous_on && schedule_control_param.schedule_ready_strobe)
                {
                    ESP_LOGI(TAG, "---- NO SCHEDULE IS ACTIVE ----");
                    /*< command to turn on the motor >*/

                    if (ScheduleMotor(schedule_control_param.selected_schedule, true, GetStandardTime(false)) && strcmp(current_schedule_record.schedule_id, schedule[schedule_control_param.selected_schedule - 1].object_id))
                    {
                        previous_on = true;

                        uint8_t motor_data = 1;
                        xQueueSendToBack(xScheduleOutQueue, &(motor_data), (TickType_t)100);

                        sprintf(current_schedule_record.schedule_phone, "%s", schedule[schedule_control_param.selected_schedule - 1].phone_num);
                        sprintf(current_schedule_record.schedule_tag, "%s", schedule[schedule_control_param.selected_schedule - 1].tag);
                        sprintf(current_schedule_record.schedule_id, "%s", schedule[schedule_control_param.selected_schedule - 1].object_id);
                    }
                    else if (!strcmp(current_schedule_record.schedule_id, schedule[schedule_control_param.selected_schedule - 1].object_id) &&
                             strcmp(current_schedule_record.schedule_id, ""))
                    {
                        /*only special case is happened here if schedule is got update and then it and same schedule is again selected*/
                        previous_on = true; // previously some schedule was ON -> turn it off
                    }
                    else
                    {
                        // CXX COMPLIENCE
                    }
                }
                else if (previous_on && schedule_control_param.schedule_ready_strobe)
                {
                    ESP_LOGI(TAG, "---- SCHEDULE IS ACTIVE ----");

                    /*< command to turn off the motor >*/
                    if (ScheduleMotor(schedule_control_param.selected_schedule, false, GetStandardTime(false)))
                    {
                        /**
                         * Case1 : if schdule still not get entertain till yet.
                         * other wise go smmoth
                         */
                        uint8_t motor_data = 2;

                        /* Be sure that on data is consumed by hardware task*/
                        if (uxQueueSpacesAvailable(xScheduleOutQueue) == 1)
                        {
                            sprintf(last_schedule_record.schedule_phone, "%s", current_schedule_record.schedule_phone);
                            sprintf(last_schedule_record.schedule_tag, "%s", current_schedule_record.schedule_tag);
                            sprintf(last_schedule_record.schedule_id, "%s", current_schedule_record.schedule_id);
                            printf("FORM 2 \n");

                            motor_data = 2;
                            xQueueSendToBack(xScheduleOutQueue, &(motor_data), (TickType_t)100);
                        }
                        else
                        {
                            /*< Actually schedule not entertained due to fault >*/
                            xQueueReset(xScheduleOutQueue);
                            /*< you can send the label here if schedule not got entertail for future development >*/
                        }
                        previous_on = false;

                        sprintf(current_schedule_record.schedule_phone, "%s", "");
                        sprintf(current_schedule_record.schedule_tag, "%s", "");
                        sprintf(current_schedule_record.schedule_id, "%s", "");

                        schedule_event = DELETE_CURRENT_SCHEDULE;
                        xQueueReset(xScheduleInQueue);
                        xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);
                    }
                }
                else
                {
                    /*< CXX COMPILENCE >*/
                }
            }
            else
            {
                // //** no schedule is from server.but remember to handle to case if running schedule is get delete
                // if (previous_on)
                // {
                //     sprintf(last_schedule_record.schedule_phone, "%s", schedule[schedule_control_param.selected_schedule - 1].phone_num);
                //     sprintf(last_schedule_record.schedule_tag, "%s", schedule[schedule_control_param.selected_schedule - 1].tag);
                //     sprintf(last_schedule_record.schedule_id, "%s", current_schedule_record.schedule_id);

                //     sprintf(current_schedule_record.schedule_phone, "%s", "");
                //     sprintf(current_schedule_record.schedule_tag, "%s", "");
                //     sprintf(current_schedule_record.schedule_id, "%s", "");

                //     uint8_t motor_data = 0;
                //     xQueueSendToBack(xScheduleOutQueue, &(motor_data), (TickType_t)100);
                //     previous_on = false;
                // }
            }
        END:
            loop_round_skip = false;
            vTaskDelay(2000 / portTICK_PERIOD_MS);
        }
        ESP_LOGE(TAG, "--- PLEASE GO FOR VERIFIED USER ---");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void ScheduleTaskInit(void *param)
{
    /* Preffered initilzation here */

    /*<  Queue Creation Here >*/
    schedule_event_t schedule_event = FETCH_SCHEDULE;

    /*< insert the fetching command to queue>*/
    xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);

    xTaskCreatePinnedToCore(ScheduleTask,
                            "schedule_task",
                            SCHEDULE_TASK_STACK_SIZE,
                            NULL,
                            SCHEDULE_TASK_PRIO,
                            &xScheduleTask,
                            SCHEDULE_TASK_CORE);
}
