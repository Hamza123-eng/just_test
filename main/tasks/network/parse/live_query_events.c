#include <string.h>

#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "live_query_events.h"
#include "cJSON.h"
#include "parse.h"
#include "schedule_estimate.h"
#include "system_gpio.h"
#include "freertos_handler.h"
#include "hw_monitor.h"
#include "phase_sensing.h"
#include "nvs_storage.h"
#include "schedule_estimate.h"

const char *TAG = "livequeryevent";

char *parse_data_name_array[NO_OF_DATA_READ_FROM_PARSE] = {
    "button_motor",
    "safety_mode",
    "auto_mode",
    "under_lim",
    "over_lim",
    "normal_mode",
    "enable_vfd",
    "on_delay_r3",
    "switch_r3",
    "phase_safety",
    "status_trip",
    //  "PhaseErrorStatus",
    "electricity",
    "hard_ping_time",
    "device_mac",
    "phase_error",
    "voltage_error",
    "voltage_p1",
    "voltage_p2",
    "voltage_p3",
    "maintenance",
    "UnderMaintinanceButton",
    "status_r3", // smart switch
    "status_run",
    "device_ip",
    //** histroy class
    "motor_run_time",
    "history_time",
    "mobile_tag",
    "name_tag",

    //** schedule
    "schedule_on",
    "schedule_off",
    "schedule_status"};

/**
 * @brief This enum is for constants with particular values and this is used in function(ParseMainData) for checking the status comming from server
 *
 */
enum ParseDataStatus
{
    kConnected = 0,
    kSubscribed = 1,
    kCreate = 2,
    kUpdate = 3,
    kLeave = 4,
    kDelete = 5,
    kUnSubscribe = 6,
    kDefault = 7

} ParseDataStatus;

/**
 * @brief This function depend on the "OP" status coming from the function(ParseDataStatusCheck), decides which event is happened in parse server,
 * So far now only UPDATE data event is handled in that function, and for other events just ESP LOG is generated,
 * In UPDATE Event, the data comming from server is "parse" using cJson library and saved in an array(parse_data_name_array) in particilar order as defined:
 * 0-5 are the indexex of that particular array
 * 0. buttonStatus
 * 1. threshold
 * 2. deviceStatus
 * 3. HotSpot
 * 4. button
 * 5. autoMode
 *
 * @param parse_data_status
 * @param monitor_json
 */
void ParseIntBoolData(enum ParseDataStatus parse_data_status, char *monitor_json)
{
    uint32_t dummy = 0;
    bool setting_change = false;

    ESP_LOGI(TAG, "--- GOING TO PARSE LIVE QUERY LIVE CLASS ---");
    switch (parse_data_status)
    {
    case kConnected:
        ESP_LOGD(TAG, "Live Query Connect Event");
        break;
    case kSubscribed:
        ESP_LOGD(TAG, "Live Query Subscribe Event");
        break;
    case kCreate:
        ESP_LOGD(TAG, "Live Query Create Event");
        break;
    case kUpdate:
    {
        ESP_LOGD(TAG, "Live Query Update Event");
        cJSON *object = NULL;
        cJSON *original = NULL;
        cJSON *object_keys = NULL;
        cJSON *original_keys = NULL;

        //   cJSON *monitor_json1 = cJSON_Parse(monitor_json);
        cJSON *monitor_json1 = monitor_json;

        object = cJSON_GetObjectItemCaseSensitive(monitor_json1, "object");
        original = cJSON_GetObjectItemCaseSensitive(monitor_json1, "original");

        if (object != NULL && original != NULL)
        {
            for (int i = 0; i < 10; i++)
            {
                object_keys = cJSON_GetObjectItemCaseSensitive(object, parse_data_name_array[i]);
                // original_keys = cJSON_GetObjectItemCaseSensitive(original, parse_data_name_array[i]);
                switch (i)
                {
                case 0:
                {
                    dummy = object_keys->valueint;
                    if (!soft_error_in_system) // only pass the error to system when it is out of error
                    {
                        ESP_LOGD(TAG, "---------------------------------Sending in queue:%d", dummy);
                        xQueueSendToBack(xButtonQueue,
                                         (uint8_t *)&dummy,
                                         (TickType_t)10);
                    }
                }
                break;
                case 1:
                {
                    dummy = object_keys->valueint;
                    if (device_config.safety_mode != dummy)
                    {
                        device_config.safety_mode = object_keys->valueint;
                        setting_change = true;
                        /* Safety mode Real time assert or deassert*/
                        PerformRealTimeSafetyMode(dummy);
                    }
                }
                break;
                case 2:
                {
                    dummy = object_keys->valueint;
                    if (device_config.auto_mode != dummy)
                    {

                        device_config.auto_mode = object_keys->valueint;
                        setting_change = true;
                    }
                }
                break;
                case 3:
                {
                    dummy = object_keys->valueint;
                    if (device_config.under_volt_lim != dummy)
                    {

                        device_config.under_volt_lim = object_keys->valueint;
                        setting_change = true;
                    }
                }
                break;
                case 4:
                {
                    dummy = object_keys->valueint;
                    if (device_config.over_volt_lim != dummy)
                    {
                        device_config.over_volt_lim = object_keys->valueint;
                        setting_change = true;
                    }
                }
                break;
                case 5:
                {
                    dummy = object_keys->valueint;
                    if (device_config.normal_device_mode != dummy)
                    {

                        device_config.normal_device_mode = object_keys->valueint;
                        setting_change = true;
                    }
                }
                break;
                case 6:
                {
                    dummy = object_keys->valueint;
                    if (device_config.latch_vfd != dummy)
                    {

                        device_config.latch_vfd = object_keys->valueint;
                        setting_change = true;
                    }
                }
                break;
                case 7:
                {
                    dummy = object_keys->valueint;
                    if (device_config.vfd_latch_time != dummy)
                    {
                        device_config.vfd_latch_time = object_keys->valueint;
                        setting_change = true;
                    }
                }
                break;
                case 8:
                {
                    dummy = object_keys->valueint;
                    if (dummy != 0)
                    {
                        if (dummy == 2)
                            dummy = 0;
                        xQueueSendToBack(xVFDQueue,
                                         (uint8_t *)&dummy,
                                         (TickType_t)10);
                    }
                }
                break;
                case 9:
                {
                    dummy = object_keys->valueint;

                    if (device_config.phase_fault != dummy)
                    {
                        device_config.phase_fault = dummy;
                        setting_change = true;
                    }
                }
                break;
                default:
                    break;
                }
            }
        }
        //  cJSON_Delete(monitor_json1);
    }
    break;
    case kLeave:
        ESP_LOGD(TAG, "the kLeave");
        break;
    case kDelete:
        ESP_LOGD(TAG, "the kDelete");
        break;
    case kUnSubscribe:
        ESP_LOGD(TAG, "the kUnSubscribe");
        break;
    case kDefault:
        ESP_LOGD(TAG, "the kDefault");
        break;
    }

    /* if some seeting got change then go to save them */
    if (setting_change)
    {
        printf("LIVE QUERY SETTING CHANGE\n");
        PerformNvsFeatureParamter(1);
    }
    //   PrintUserConf(NULL);
}

/**
 * @brief This function depend on the "OP" status coming from the function(ParseDataStatusCheck), decides which event is happened in parse server,
 * So far now only UPDATE data event is handled in that function, and for other events just ESP LOG is generated,
 * In UPDATE Event, the data comming from server is "parse" using cJson library and saved in an array(parse_data_name_array) in particilar order
 * @param enum ParseDataStatus
 * @param char *monitor_json
 */
void ParseIntBoolDataSchedule(enum ParseDataStatus parse_data_status, char *monitor_json)
{
    ESP_LOGD(TAG, "--- GOING TO PARSE SCHEDULE DATA ---");
    schedule_event_t schedule_event;
    switch (parse_data_status)
    {
    case kConnected:
        ESP_LOGD(TAG, "Live Query Connect Event");
        break;
    case kSubscribed:
        ESP_LOGD(TAG, "Live Query Subscribe Event");
        break;
    case kCreate:
        ESP_LOGD(TAG, "Live Query Create Event");
        schedule_event = REFRESH_SCHEDULE;
        /*< insert the fetching command to queue>*/
        xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);
        break;
    case kUpdate:
        ESP_LOGD(TAG, "Live Query Update Event");

        // schedule_event = REFRESH_SCHEDULE;
        // /*< insert the fetching command to queue>*/
        // xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);
        break;
    case kLeave:
        ESP_LOGD(TAG, "the kLeave");
        break;
    case kDelete:
        ESP_LOGD(TAG, "the kDelete");
        if (schedule_control_param.entertain_query_callback)
        {
            force_deleted = true;
            schedule_event = REFRESH_SCHEDULE;
            /*< insert the fetching command to queue>*/
            xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);
        }
        break;
    case kUnSubscribe:
        ESP_LOGD(TAG, "the kUnSubscribe");
        break;
    case kDefault:
        ESP_LOGD(TAG, "the kDefault");
        break;
    }
}

/**
 * @brief This function receiving the "OP" status and then after comparing return the constant value depend
 * on the "OP" status comming from the parse server
 * @param char *check_status looks for which event occured?
 * @return int
 */
int ParseEventStatusCheck(char *check_status)
{
    ESP_LOGD(TAG, "Parse data status  = %s ", (char *)check_status);

    if (!(strcmp(check_status, (char *)"connected")))
        return kConnected;
    else if (!(strcmp(check_status, (char *)"subscribed")))
        return kSubscribed;
    else if (!(strcmp(check_status, (char *)"create")))
        return kCreate;
    else if (!(strcmp(check_status, (char *)"update")))
        return kUpdate;
    else if (!(strcmp(check_status, (char *)"leave")))
        return kLeave;
    else if (!(strcmp(check_status, (char *)"delete")))
        return kDelete;
    else if (!(strcmp(check_status, (char *)"unsubscribe")))
        return kUnSubscribe;

    return kDefault;
}

/**
 * @brief This function getting as input the raw packet comming from the server, then it check the status means "op-code"(op is the status from parse according to the events)
 * This hits when data event hits in websockets,
 * Check the status and give control to other function(ParseMainData) which parse the data and save this data in an array
 * @param data_for_parsing
 */
void JsonParser(char *data_for_parsing)
{

    cJSON *monitor_json = cJSON_Parse(data_for_parsing);
    cJSON *parse_status_op = cJSON_GetObjectItemCaseSensitive(monitor_json, "op");

    if (parse_status_op != NULL)
    {
        ESP_LOGD(TAG, "Parse status = %s ", parse_status_op->valuestring);

        // char *monitor_json1 = cJSON_Print(monitor_json);
        /*check and parse the raw data comming from the parse server*/
        ParseIntBoolData(ParseEventStatusCheck((char *)parse_status_op->valuestring), monitor_json);
        // free(monitor_json1);
    }
    data_for_parsing = NULL;

    cJSON_Delete(monitor_json);
}
void JsonParserSchedule(char *data_for_parsing)
{

    cJSON *monitor_json = cJSON_Parse(data_for_parsing);
    cJSON *parse_status_op = cJSON_GetObjectItemCaseSensitive(monitor_json, "op");

    if (parse_status_op != NULL)
    {
        ESP_LOGD(TAG, "Parse status = %s ", parse_status_op->valuestring);
        /*check and parse the raw data comming from the parse server*/
        ParseIntBoolDataSchedule(ParseEventStatusCheck((char *)parse_status_op->valuestring), data_for_parsing);
    }
    data_for_parsing = NULL;

    cJSON_Delete(monitor_json);
}

/**
 * @brief JsonParserEvent() is used to extract the event type from the packet
 *
 * @param json_packet
 * @param event
 */
void JsonParserEvent(char *json_packet, char *event)
{
    cJSON *lq_json = cJSON_Parse(json_packet);

    if (lq_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        event = NULL;
    }
    else
    {
        cJSON *op = cJSON_GetObjectItemCaseSensitive(lq_json, "op");

        if (cJSON_IsString(op) && (op->valuestring != NULL))
        {
            sprintf(event, "%s", op->valuestring);
        }
        else
        {
            event = NULL;
        }
    }

    cJSON_Delete(lq_json);
}

/**
 * @brief Parse boolean value
 *
 * @param lq_json
 * @param event
 * @param key
 * @return int
 */
int JsonParserBool(char *lq_json, char *event, char *key)
{
    cJSON *json_packet = cJSON_Parse(lq_json);

    if (strcmp(event, "update") == 0)
    {
        cJSON *json_value = NULL;
        cJSON *object = cJSON_GetObjectItemCaseSensitive(json_packet, "object");

        if (cJSON_IsObject(object))
        {
            json_value = cJSON_GetObjectItemCaseSensitive(object, key);

            if (cJSON_IsBool(json_value))
            {
                int value = json_value->valueint;

                cJSON_Delete(json_packet);

                return value;
            }
        }
    }

    cJSON_Delete(json_packet);

    return -1;
}

/**
 * @brief Parse string value
 *
 * @param lq_json
 * @param event
 * @param key
 * @param value
 */
void JsonParserString(char *lq_json, char *event, char *key, char *value)
{
    cJSON *json_packet = cJSON_Parse(lq_json);

    if (strcmp(event, "update") == 0)
    {
        cJSON *json_value = NULL;
        cJSON *object = cJSON_GetObjectItemCaseSensitive(json_packet, "object");

        if (cJSON_IsObject(object))
        {
            json_value = cJSON_GetObjectItemCaseSensitive(object, key);

            if (cJSON_IsString(json_value) && json_value->valuestring != NULL)
            {
                // ESP_LOGD("Object \"%s\"\n", json_value->valuestring);
                sprintf(value, "%s", json_value->valuestring);
            }
        }
    }
    else
    {
        value = NULL;
    }

    cJSON_Delete(json_packet);
}

/**
 * @brief Parse Int value
 *
 * @param lq_json
 * @param event
 * @param key
 * @return int
 */
int JsonParserInt(char *lq_json, char *event, char *key)
{
    cJSON *json_packet = cJSON_Parse(lq_json);

    if (strcmp(event, "update") == 0)
    {
        cJSON *json_value = NULL;
        cJSON *object = cJSON_GetObjectItemCaseSensitive(json_packet, "object");

        if (cJSON_IsObject(object))
        {
            json_value = cJSON_GetObjectItemCaseSensitive(object, key);

            if (cJSON_IsNumber(json_value))
            {
                int value = json_value->valueint;

                cJSON_Delete(json_packet);

                return value;
            }
        }
    }

    cJSON_Delete(json_packet);
    return -1;
}
