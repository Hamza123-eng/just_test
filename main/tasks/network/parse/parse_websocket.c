#include <stdio.h>

#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"

#include "esp_log.h"
#include "esp_websocket_client.h"
#include "esp_event.h"

#include "parse_websocket.h"
#include "live_query_events.h"
// #include "http_event_handler.h"
#include "cJSON.h"
#include "parse.h"
#include "main.h"
#include "hw_monitor.h"

#define NO_DATA_TIMEOUT_SEC 10
#define WS_PING_INTERVEL 3
#define WEBSOCKET_URI "ws://143.198.190.41:1337/parse/classes"

static const char *TAG = "WEBSOCKET";

WSTracker_t ws_tracker;

char *appId = "remoteWellAppId";
int sub = 0;
int sub_schedule = 0;
<<<<<<< HEAD
esp_websocket_client_handle_t ws_client=NULL, ws_client_sch=NULL;



=======
esp_websocket_client_handle_t ws_client = NULL, ws_client_sch = NULL;
>>>>>>> b9dad1e1f8498dc2141c089c0bc981309957ec93
// esp_websocket_client *ws_client, *ws_client_sch
/**
 * @brief A helper function to subcribe with the provide classname's field
 * fields are hard coded
 * @param char *class_name full URI of classname / table
 * @param sessionToken
 * @return None
 */
static void LiveQuerySubscribeToAField(char *class_name, char *sessionToken)
{
    if (sub == 0)
    {
        cJSON *sub_packet = cJSON_CreateObject();

        if (sub_packet == NULL)
        {
            goto end;
        }

        cJSON_AddStringToObject(sub_packet, "op", "subscribe");
        cJSON_AddNumberToObject(sub_packet, "requestId", 1);

        cJSON *query_object = cJSON_CreateObject();

        if (query_object == NULL)
        {
            goto end;
        }

        cJSON_AddItemToObject(sub_packet, "query", query_object);
        cJSON_AddStringToObject(query_object, "className", class_name);

        cJSON *where_object = cJSON_CreateObject();

        if (where_object == NULL)
        {
            goto end;
        }

        cJSON_AddItemToObject(query_object, "where", where_object);

        cJSON *objectId_object = cJSON_CreateObject();

        if (objectId_object == NULL)
        {
            goto end;
        }

        cJSON_AddItemToObject(where_object, "objectId", objectId_object);
        cJSON_AddStringToObject(objectId_object, "$all", "");

        cJSON *field_array = cJSON_CreateArray();

        cJSON_AddItemToObject(query_object, "fields", field_array);
        cJSON *button = NULL;

        for (uint8_t index = 0; index < 10; index++)
        {
            button = cJSON_CreateString(parse_data_name_array[index]);

            cJSON_AddItemToArray(field_array, button);
        }
        // cJSON_AddStringToObject(sub_packet, "sessionToken", sessionToken);

        char *to_send = cJSON_Print(sub_packet);
        ESP_LOGD(TAG, "%s", to_send);
        esp_websocket_client_send_text(ws_client, to_send, strlen(to_send), portMAX_DELAY);
        sub++;
        free(to_send);
    end:
        cJSON_Delete(sub_packet);
    }
}

/**
 * @brief A helper function to subcribe with the provide classname
 * @param char *class_name full URI of classname / table
 * @param sessionToken
 * @return None
 */
static void LiveQuerySubscribeToAClass(char *class_name, char *sessionToken)
{

    if (sub_schedule == 0)
    {
        cJSON *sub_packet = cJSON_CreateObject();

        if (sub_packet == NULL)
        {
            goto end;
        }

        cJSON_AddStringToObject(sub_packet, "op", "subscribe");
        cJSON_AddNumberToObject(sub_packet, "requestId", 1);

        cJSON *query_object = cJSON_CreateObject();

        if (query_object == NULL)
        {
            goto end;
        }

        cJSON_AddItemToObject(sub_packet, "query", query_object);
        cJSON_AddStringToObject(query_object, "className", class_name);

        cJSON *where_object = cJSON_CreateObject();

        if (where_object == NULL)
        {
            goto end;
        }

        cJSON_AddItemToObject(query_object, "where", where_object);

        cJSON *objectId_object = cJSON_CreateObject();

        if (objectId_object == NULL)
        {
            goto end;
        }

        cJSON_AddItemToObject(where_object, "objectId", objectId_object);
        cJSON_AddStringToObject(objectId_object, "$all", "");

        cJSON_AddStringToObject(sub_packet, "sessionToken", sessionToken);

        char *to_send = cJSON_Print(sub_packet);
        ESP_LOGD(TAG, "%s", to_send);
        esp_websocket_client_send_text(ws_client_sch, to_send, strlen(to_send), portMAX_DELAY);
        sub_schedule++;
        free(to_send);
    end:
        cJSON_Delete(sub_packet);
    }
}

/**
 * @brief A helper function to establish a live query connection
 * @param None
 * @return None
 */
static void LiveQueryConnect(esp_websocket_client_handle_t websocket_client)
{

    cJSON *connect = cJSON_CreateObject();
    cJSON_AddStringToObject(connect, "op", "connect");
    cJSON_AddStringToObject(connect, "applicationId", parse.server_settings.appId);

    char *con_str = cJSON_Print(connect);

    ESP_LOGD(TAG, "Sending %s", con_str);

    esp_websocket_client_send_text(websocket_client, con_str, strlen(con_str), portMAX_DELAY);
    free(con_str);
    cJSON_Delete(connect);
}

static char buff_prev[3000] = {0};

/**
 * @brief Event handler for websocket. Different functions are called in it i.e.
 * LiveQueryConnect() and LiveQuerySubscribeToAField()
 * @param handler_args
 * @param base
 * @param event_id
 * @param event_data
 */
static void websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{

    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;
    // data->payload_offset=0;

    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGD(TAG, "WEBSOCKET_EVENT_CONNECTED");
        LiveQueryConnect(ws_client);

        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        sub = 0; // make it 0 so that we can subscribe again
        break;
    case WEBSOCKET_EVENT_DATA:
    {
        ESP_LOGD(TAG, "WEBSOCKET_EVENT_DATA");

        LiveQuerySubscribeToAField(parse.parse_ws_uri.ws_uri_real_time_data, parse.user_login_receive.sessionToken);

        ESP_LOGD(TAG, "RTD Received opcode=%d", data->op_code);
        if (data->data_len)
            ESP_LOGW(TAG, "RTD Received=%.*s", 20, (char *)data->data_ptr);
        else
            ESP_LOGW(TAG, "RTD Received=%.*s", (data->data_len), (char *)data->data_ptr);

        if (strcmp(buff_prev, data->data_ptr) != 0)
        {
            JsonParser((char *)data->data_ptr);
        }
        memset(buff_prev, 0x00, 3000);
        strcpy(buff_prev, data->data_ptr);

        data->data_ptr = NULL;
    }
    break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGD(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}
static char buff_prevd[3000] = {0};

/**
 * @brief Event handler for websocket. Different functions are called in it i.e.
 * LiveQueryConnect() and LiveQuerySubscribeToAField()
 * @param handler_args
 * @param base
 * @param event_id
 * @param event_data
 */
static void sch_websocket_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_websocket_event_data_t *data = (esp_websocket_event_data_t *)event_data;

    switch (event_id)
    {
    case WEBSOCKET_EVENT_CONNECTED:
        ESP_LOGD(TAG, "WEBSOCKET_EVENT_CONNECTED");
        ws_tracker.sch_event_tracker = WEBSOCKET_EVENT_CONNECTED;

        LiveQueryConnect(ws_client_sch);

        break;
    case WEBSOCKET_EVENT_DISCONNECTED:
        ESP_LOGD(TAG, "WEBSOCKET_EVENT_DISCONNECTED");
        ws_tracker.sch_event_tracker = WEBSOCKET_EVENT_DISCONNECTED;
        sub_schedule = 0; // make it 0 so that we can subscribe again

        break;
    case WEBSOCKET_EVENT_DATA:

        ESP_LOGD(TAG, "WEBSOCKET_EVENT_DATA");

        LiveQuerySubscribeToAClass(parse.parse_ws_uri.ws_uri_schedule, parse.user_login_receive.sessionToken);

        ESP_LOGD(TAG, "Schedule Received opcode=%d", data->op_code);
        if (data->data_len)
            ESP_LOGW(TAG, "Schedule Received=%.*s", 20, (char *)data->data_ptr);
        else
            ESP_LOGW(TAG, "Schedule Received=%.*s", data->data_len, (char *)data->data_ptr);

        if (strcmp(buff_prevd, data->data_ptr) != 0)

        {
            JsonParserSchedule((char *)data->data_ptr);
        }
        memset(buff_prevd, 0x00, 3000);

        strcpy(buff_prevd, data->data_ptr);

        data->data_ptr = NULL;

        ws_tracker.sch_event_tracker = WEBSOCKET_EVENT_DATA;

        break;
    case WEBSOCKET_EVENT_ERROR:
        ESP_LOGD(TAG, "WEBSOCKET_EVENT_ERROR");
        break;
    }
}

/**
 * @brief Initialize the web socket protocol and attache's the event handler
 * @param None
 * @return None
 */
void websocket_app_start(void)
{
    sub = 0; // used in subscription
             //    memset(ws_client->payload_offset,0x00,4) //=0;
             // ws_client->payload_offset=0;
    esp_websocket_client_config_t websocket_cfg = {};

    websocket_cfg.uri = parse.parse_ws_uri.web_socket_uri;
    // websocket_cfg.ping_interval_sec = WS_PING_INTERVEL;
    websocket_cfg.disable_auto_reconnect = false;
    websocket_cfg.buffer_size = 1024 * 2;
    websocket_cfg.task_stack = 1028 * 5;
    websocket_cfg.task_prio = 5;

    ESP_LOGI(TAG, "Connecting to %s...", websocket_cfg.uri);

    ws_client = esp_websocket_client_init(&websocket_cfg);

    esp_websocket_register_events(ws_client, WEBSOCKET_EVENT_ANY, websocket_event_handler, (void *)ws_client);

    esp_websocket_client_start(ws_client);
    
}

/**
 * @brief Initialize the web socket protocol and attache's the event handler
 * with schedule class
 * @param None
 * @return None
 */
void websocket_schedule_start(void)
{
    sub_schedule = 0; // used in subscription
    esp_websocket_client_config_t websocket_sch_cfg = {};

    websocket_sch_cfg.uri = parse.parse_ws_uri.web_socket_uri;
    // websocket_sch_cfg.ping_interval_sec = WS_PING_INTERVEL;
    websocket_sch_cfg.disable_auto_reconnect = false;

    websocket_sch_cfg.disable_auto_reconnect = false;
    websocket_sch_cfg.buffer_size = 1024 * 2;
    websocket_sch_cfg.task_stack = 1028 * 5;
    websocket_sch_cfg.task_prio = 5;

    ESP_LOGI(TAG, "Connecting to %s...", websocket_sch_cfg.uri);

    ws_client_sch = esp_websocket_client_init(&websocket_sch_cfg);

    esp_websocket_register_events(ws_client_sch, WEBSOCKET_EVENT_ANY, sch_websocket_event_handler, (void *)ws_client_sch);

    esp_websocket_client_start(ws_client_sch);
    
}

/**
 * @brief A helper function to close the websocket and release resources
 * @param None
 * @return None
 */
void websocket_app_stop(void)
{
    if (ws_client != NULL)
    {
        esp_websocket_client_stop(ws_client);
        ESP_LOGD(TAG, "Websocket Stopped");
        esp_websocket_client_destroy(ws_client);
    }
}
void websocket_sch_stop(void)
{
    if (ws_client_sch != NULL)
    {
        esp_websocket_client_stop(ws_client_sch);
        ESP_LOGD(TAG, "Websocket Stopped");
        esp_websocket_client_destroy(ws_client_sch);
    }
}
