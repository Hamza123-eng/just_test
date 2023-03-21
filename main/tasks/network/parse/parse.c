// #include <string.h>
// #include "esp_log.h"

// #include "parse_websocket.h"
// #include "http_event_handler.h"
// #include "http_event_handler_put.h"
// #include "http_event_handler_post.h"
// #include "data_store_nvs.h"
// #include "get.h"
// #include "put.h"
// #include "post.h"
// #include "main.h"
// #include "hw_monitor.h"
// #include "schedule_estimate.h"
// #include "network_task_manager.h"
#include "cJSON.h"

#include "parse.h"
#include "http_client.h"
#include "wifi_manger.h"
#include "system_time.h"
#include "pcf.h"
#include "nvs_storage.h"
#include "freertos/timers.h"
#include "hw_monitor.h"
#include "freertos_handler.h"
#include "system_time.h"
#include "parse_websocket.h"
#include "parse_event_controller.h"

static const char *TAG = "Parse []";
// // uint8_t button_value = 0;

parse_network_config_t network_config = {0x00};
http_header_t parse_header = {0x00};
Parse_t parse;

TimerHandle_t xAutoModeTimer = NULL;
/**
 * @brief
 *
 * @param xTimer
 */
void vTimerCallback(TimerHandle_t xTimer)
{
    /**Relase the automode event */
    network_config.go_automode = 1;

    ESP_LOGI(TAG, "!!!!----TIMER GOING TO DELETE----");
    xTimerDelete(xAutoModeTimer,
                 5000 / portTICK_PERIOD_MS);
}
/**
 * @brief A helper function to update the following parameters of the parse structure
 * 1. server url 2. server port 3. server app id 4. username from nvs 5. password from nvs
 * 6. device number
 * @param None
 * @return None
 */
static void ParseSetSettings()
{
    sprintf(parse.server_settings.url, "%s", PARSE_SERVER_HOST_URL);
    sprintf(parse.server_settings.port, "%s", PARSE_SERVER_PORT);
    sprintf(parse.server_settings.appId, "%s", PARSE_SERVER_APP_ID);

    sprintf(parse.user_login_info.username, "%s", network_config.user_name);
    sprintf(parse.user_login_info.password, "%s", network_config.user_pass);
    sprintf(parse.user_login_info.device_num, "%s", network_config.device_num);
    sprintf(device_config.model_data, "EPT%s_%.5u", device_config.hardware_version, device_config.hardware_serial);

    sprintf(parse_header.key_1, "%s", "X-Parse-Application-Id");
    sprintf(parse_header.value_1, "%s", PARSE_SERVER_APP_ID);

    ESP_LOGD(TAG, "%s", parse.server_settings.url);
    ESP_LOGD(TAG, "%s", parse.server_settings.port);
    ESP_LOGD(TAG, "%s", parse.server_settings.appId);
    ESP_LOGD(TAG, "%s", parse.user_login_info.username);
    ESP_LOGD(TAG, "%s", parse.user_login_info.password);
    ESP_LOGD(TAG, "%s", parse.user_login_info.device_num);
}

/**
 * @brief A helper function to setup the parse login URI of parse structure.
 * Before calling this function make sure to call ParseSetSettings() and SetHttpClient()  because its
 * info is used in this function
 * @param None
 * @return None
 */
void ParseSetLoginURI()
{
    sprintf(parse.parse_uri.login_uri, "http://%s:%s/parse/login?username=%s&password=%s", parse.server_settings.url, parse.server_settings.port, parse.user_login_info.username, parse.user_login_info.password);

    ESP_LOGI(TAG, " LOG IN URL : %s", parse.parse_uri.login_uri);
}

/**
 * @brief A helper function to setup the parse GET URI of parse structure
 * before calling this function make sure you have called the ParseSetLoginURI()and Login()
 * It uses the information from mention functions
 * @param None
 * @return None
 */
static void ParseSetGetURI()
{
    sprintf(parse.parse_uri.get_uri, "http://%s:%s/parse/classes/device_%s_%s_%s", parse.server_settings.url, parse.server_settings.port,
            parse.user_login_info.device_num, parse.user_login_info.username, parse.user_login_receive.objectId);

    ESP_LOGI(TAG, "%s", parse.parse_uri.get_uri);
}

/**
 * @brief A helper function to setup the parse PUT URI of parse structure
 * before calling this function make sure you have called the ParseSetGetURI() and GET_objectId()
 * It uses the information from the mention functions
 * @param None
 * @return None
 */
static void ParseSetPutURI()
{
    sprintf(parse.parse_uri.put_uri, "http://%s:%s/parse/classes/device_%s_%s_%s/%s", parse.server_settings.url, parse.server_settings.port,
            parse.user_login_info.device_num, parse.user_login_info.username, parse.user_login_receive.objectId, parse.single_objectId);

    ESP_LOGD(TAG, "%s", parse.parse_uri.put_uri);
}

/**
 * @brief A helper function to setup the parse  POST URI of parse structure
 * before calling this function make sure you have called the ParseSetGetURI() and GET_objectId()
 * It uses the information from the mention functions
 * @param None
 * @return None
 */
static void ParseSetPostURI()
{
    sprintf(parse.parse_uri.post_uri, "http://%s:%s/parse/classes/device_%s_%s_%s_History", parse.server_settings.url, parse.server_settings.port,
            parse.user_login_info.device_num, parse.user_login_info.username, parse.user_login_receive.objectId);

    ESP_LOGD(TAG, " History  : %s \n", parse.parse_uri.post_uri);
}

/**
 * @brief A helper function to setup the parse Schedule URI of parse structure
 * before calling this function make sure you have called the ParseSetGetURI() and GET_objectId()
 * It uses the information from the mention functions
 * @param None
 * @return None
 */
static void ParseSetScheduleURI()
{
    sprintf(parse.parse_uri.schedule_uri, "http://%s:%s/parse/classes/device_%s_%s_%s_readFullSchedule", parse.server_settings.url, parse.server_settings.port,
            parse.user_login_info.device_num, parse.user_login_info.username, parse.user_login_receive.objectId);
    ESP_LOGI(TAG, "SCHEDULE URL : %s \n", parse.parse_uri.schedule_uri);
}

/**
 * @brief A helper function that Sets the Web Socket URI. This function must be called before
 * calling websocket_app_start()
 * @param None
 * @return None
 */
static void SetWebSocketURI()
{
    sprintf(parse.parse_ws_uri.web_socket_uri, "ws://%s:%s/parse/classes", PARSE_SERVER_HOST_URL, PARSE_SERVER_PORT);
}

/**
 * @brief A helper function that Sets the class name for livequery. This function must be called before
 * calling SetWebSocketClassURI()
 * @param None
 * @return None
 */
static void SetWebSocketClassURI()
{
    sprintf(parse.parse_ws_uri.ws_uri_real_time_data, "device_%s_%s_%s", parse.user_login_info.device_num, parse.user_login_info.username, parse.user_login_receive.objectId);
}

/**
 * @brief A helper function that Sets the class name for livequery. This function must be called before
 * calling SetWebSocketClassURI()
 * @param None
 * @return None
 */
static void SetScheduleWebSocketClassURI()
{
    sprintf(parse.parse_ws_uri.ws_uri_schedule, "device_%s_%s_%s_readFullSchedule", parse.user_login_info.device_num, parse.user_login_info.username, parse.user_login_receive.objectId);
}

/**
 * @brief This function will initializes all the necessary information to communicate with web sockets
 * for livequery
 * @param None
 * @return None
 */
void WebSocketInit()
{
    SetWebSocketURI();
    SetWebSocketClassURI();
    websocket_app_start();
    SetScheduleWebSocketClassURI();
    websocket_schedule_start();
}

/**
 * @brief This is a FreeRTOS task that calls PUT init function. The thread suspends itself after completing
 * the PUT request
 * @param void *ptr
 * @return None
 */

/**
 * @brief This is a FreeRTOS task that calls POST init function. The thread suspends itself after completing
 * the POST request
 * @param void *ptr
 * @return None
 */

/**
 * @brief
 *
 * @param response
 * @return true
 * @return false
 */
static bool ObjectIdParser(char *response)
{
    // ESP_LOGI(TAG, "object data received %s", response);

    bool user_verified = true; // just local varale for return handling

    cJSON *monitor_json = cJSON_Parse(response);
    cJSON *json_array = NULL;

    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL)
        {
            ESP_LOGE(TAG, "Error before: %s\n", error_ptr);
        }

        goto end;
    }

    json_array = cJSON_GetObjectItemCaseSensitive(monitor_json, "results");

    if (json_array == NULL)
    {
        user_verified = false;
        goto end;
    }
    cJSON *single_object;

    cJSON_ArrayForEach(single_object, json_array)
    {

        cJSON *objectId = cJSON_GetObjectItemCaseSensitive(single_object, "objectId");

        if (cJSON_IsString(objectId) && (objectId->valuestring != NULL))
        {
            sprintf(parse.single_objectId, "%s", objectId->valuestring);

            ESP_LOGD(TAG, "ObjectId \"%s\"\n", parse.single_objectId);
        }
        else
        {
            user_verified = false;
            goto end;
        }
        /* Now the user fetch user custom setting */

        cJSON_GetObjectItem(single_object, parse_data_name_array[auto_mode_motor])
            ? device_config.auto_mode = cJSON_GetObjectItem(single_object, parse_data_name_array[auto_mode_motor])->valueint
            : 1;
        cJSON_GetObjectItem(single_object, parse_data_name_array[normal_mode])
            ? device_config.normal_device_mode = cJSON_GetObjectItem(single_object, parse_data_name_array[normal_mode])->valueint
            : 1;
        cJSON_GetObjectItem(single_object, parse_data_name_array[latched_vfd])
            ? device_config.latch_vfd = cJSON_GetObjectItem(single_object, parse_data_name_array[latched_vfd])->valueint
            : 1;
        cJSON_GetObjectItem(single_object, parse_data_name_array[safety_mode])
            ? device_config.safety_mode = cJSON_GetObjectItem(single_object, parse_data_name_array[safety_mode])->valueint
            : 1;
        cJSON_GetObjectItem(single_object, parse_data_name_array[latched_delay])
            ? device_config.vfd_latch_time = cJSON_GetObjectItem(single_object, parse_data_name_array[latched_delay])->valueint
            : 1;
        cJSON_GetObjectItem(single_object, parse_data_name_array[over_vol_lim])
            ? device_config.over_volt_lim = cJSON_GetObjectItem(single_object, parse_data_name_array[over_vol_lim])->valueint
            : 1;
        cJSON_GetObjectItem(single_object, parse_data_name_array[under_volt_lim])
            ? device_config.under_volt_lim = cJSON_GetObjectItem(single_object, parse_data_name_array[under_volt_lim])->valueint
            : 1;
        cJSON_GetObjectItem(single_object, parse_data_name_array[phase_fault])
            ? device_config.phase_fault = cJSON_GetObjectItem(single_object, parse_data_name_array[phase_fault])->valueint
            : 1;
        network_config.go_automode = 1;
        network_config.verified_user = 1;
        PerformNvsFeatureParamter(true);
        PerformNvsVerifiedUser(true); // writing the verifed user here
    }

end:
    cJSON_Delete(monitor_json);
    return user_verified;
}
/**
 * @brief This function will initializes all the necessary information to communicate with parse server
 * such as login and getting the object id to create all paths for communication. It also get the schedules
 * and starts the schedule task
 * @param void *ptr
 * @return None
 */
static bool LoginParser(char *response) // FIXME verify memory leaks cJSON here
{
    // ESP_LOGI(TAG, "LOG IN DATA %s", response);

    bool user_verified = true;

    cJSON *monitor_json = cJSON_Parse(response);
    cJSON *objectId = NULL;
    cJSON *rfid_tag = NULL;
    cJSON *sessionToken = NULL;

    if (monitor_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();

        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        goto end;
    }

    objectId = cJSON_GetObjectItemCaseSensitive(monitor_json, "objectId");

    if (cJSON_IsString(objectId) && (objectId->valuestring != NULL))
    {
        sprintf(parse.user_login_receive.objectId, "%s", objectId->valuestring);

        ESP_LOGD(TAG, "ObjectId \"%s\"\n", parse.user_login_receive.objectId);
    }
    else
    {
        user_verified = false;
        goto end;
    }

    sessionToken = cJSON_GetObjectItemCaseSensitive(monitor_json, "sessionToken");

    if (cJSON_IsString(sessionToken) && (sessionToken->valuestring != NULL))
    {
        sprintf(parse.user_login_receive.sessionToken, "%s", sessionToken->valuestring);

        ESP_LOGD(TAG, "SessionToken \"%s\"\n", sessionToken->valuestring);
        /*< filling the sessio token>*/

        sprintf(parse_header.key_2, "%s", "X-Parse-Session-Token");
        sprintf(parse_header.value_2, "%s", parse.user_login_receive.sessionToken);
    }
end:
    cJSON_Delete(monitor_json);
    return user_verified;
}
/**
 * @brief
 *
 * @param param
 */
void UnRegisteredUserFound(void *param)
{
    ReturnNetworkLock(NULL);

    network_config.verified_user = 0;
    PerformNvsVerifiedUser(true);

    /** Force PCF to Display Wrong-USER Mode*/
    pcf_mode_t mode = WRONG_USER;
    xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);

    /*Shift mode to APP */
    e_wifi_mode event_received = AP;
    xQueueSendToBack(xWifiQueue, &(event_received), (TickType_t)100);

    /*< just go for webserver >*/

    // start_webserver(NULL);

    while (1)
    {
        ESP_LOGI(TAG, " ---- WRONG USER FOUND ---- ");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

// /**
//  * @brief
//  *
//  * @param ptr
//  */
void ParseInitTask(void *ptr)
{
    printf("in prse init\n");
    vTaskDelay(200);
    bool local_verified_user = true;

    ParseSetSettings();
    ParseSetLoginURI();

    uint8_t status_code = 0;
    char *data_received = NULL;

    /*----------------------->*/
    /* CRITICAL SECTIN */
    // LOGIN CONFORMATION
    xAutoModeTimer = xTimerCreate("Timer",
                                  5000 / portTICK_PERIOD_MS,
                                  pdTRUE,
                                  (void *)0,
                                  vTimerCallback);
    xTimerStart(xAutoModeTimer, 0);
    GetNetworkLock(NULL);

    //** First go With login

    while (1)
    {
        if (IsNetworkAvailable(NULL))
        {

            data_received = HttpPerform(parse.parse_uri.login_uri, GET, &status_code, NULL, &parse_header);

            if (status_code == 200  || status_code == 100)
            {
                local_verified_user = LoginParser(data_received);
                (local_verified_user) ? 1 : UnRegisteredUserFound(NULL);
            }

            if (status_code == 200)
                break;
        }

        ESP_LOGE(TAG, " ---- SYSTEM TRYING FOR LOGIN ---- ");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ESP_LOGI(TAG, " ---- SYSTEM GOT LOGIN ---- ");
    free(data_received);
    data_received = NULL;

    // LIVE CLASS CONFORMATION
    ParseSetGetURI();
    while (1)
    {
        if (IsNetworkAvailable(NULL))
        {
            data_received = HttpPerform(parse.parse_uri.get_uri, GET, &status_code, NULL, &parse_header);

            if (status_code == 200)
            {
                local_verified_user = ObjectIdParser(data_received);
                (local_verified_user) ? 1 : UnRegisteredUserFound(NULL);
            }
            
            if (status_code == 200)
                break;
        }
        ESP_LOGE(TAG, " ---- SYSTEM TRYING FOR OBJECT ID ---- ");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI(TAG, " ---- SYSTEM GOT OBJECT ID ---- ");
    free(data_received);
    data_received = NULL;

    ReturnNetworkLock(NULL);
    /** END CRITAICAL SECTION*/
    /*<-----------------------*/

    // GetStandardTime(true); // going for forced time manadatory here to sync the time for schedule

    network_config.verified_user = 1; // now user got fully verified
    PerformNvsVerifiedUser(NULL);
    ParseSetPutURI();
    ParseSetPostURI();
    ParseSetScheduleURI();

    /*Sending info to server*/

    parse_event_send_t data_to_send = {
        .class_type = INFO_CLASS,
    };
    xQueueSendToBack(xParseQueue,
                     (void *)&data_to_send,
                     (TickType_t)10);
}

// /**
//  * @brief Get the Button Value Task that checks if new value from server is received and send the data to the hardware queue.
//  * It also updates the server with current hardware status by making a PUT request
//  * @param void *ptr
//  * @retval None
//  */
// void GetButtonValueTask(void *ptr)
// {
//     while (1)
//     {
//         if ((parse.receive_fields.cur_button_status != parse.receive_fields.pre_button_status)) //&& (parse.receive_fields.cur_button_status != 0)
//         {
//             xQueueSend(xGetButtonQueue1, &parse.receive_fields.cur_button_status, portMAX_DELAY); // send button value to NO Task
//             xQueueSend(xGetButtonQueue2, &parse.receive_fields.cur_button_status, portMAX_DELAY); // send button value to NC Task

//             parse.receive_fields.pre_button_status = parse.receive_fields.cur_button_status;

//             ESP_LOGD(TAG, "Button Value%d", parse.receive_fields.cur_button_status);

//             char buff[500];

//             ReadCurrentHWState();

//             void (*fp_put_json_packet)(char *, int, bool, bool, bool, bool, bool, bool) = &PUT_JSON_Packet; // creating function pointer and assigning address of the function

//             (*fp_put_json_packet)(buff, parse.send_fields.button_status, parse.send_fields.motor_status,
//                                   parse.send_fields.trip_status, parse.send_fields.undermnt, parse.send_fields.alive_status, parse.send_fields.phase_error, parse.send_fields.elcectricity);

//             if (xSemaphoreTake(put_control_mutex, portMAX_DELAY) == pdTRUE)
//             {
//                 ESP_LOGD(TAG, "Get Button Take put Mutex");

//                 while (PUT(buff) != 200)
//                 {
//                     vTaskDelay(10 / portTICK_PERIOD_MS);
//                 }

//                 xSemaphoreGive(put_control_mutex);

//                 ESP_LOGD(TAG, "Get Button Give put Mutex");
//             }
//         }

//         vTaskDelay(400 / portTICK_PERIOD_MS);
//     }
// }
