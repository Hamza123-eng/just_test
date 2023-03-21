// /**
//  * @file parse_event_controller.c
//  * @author Hamxa Islam {hamzaislam170@gamil.com}
//  * @brief Remotewell_PRO
//  * @version 0.1
//  * @date 2022-11-04
//  *
//  * EPTeck Technologies Gmbh    2022
//  *
//  */
#include "esp_sleep.h"

// #include "wifi_manager.h"
#include "wifi_platform.h"
#include "freertos_handler.h"
#include "parse_event_controller.h"
#include "parse_packet_maker.h"
#include "http_client.h"
#include "parse.h"
#include "fram.h"
#include "nvs_storage.h"
#include "wifi_manger.h"
#include "system_spiffs.h"
#include "esp_system.h"
#include "pcf.h"

TaskHandle_t xParseControllerTask;

uint8_t electricity_tries = 0;

bool network_status_change = false;
parse_event_send_t prev_data = {0x00};

static char *TAG = "PARSE EVENT CONTROLLER []";
/**
 * @brief
 *
 * @param param
 */
void SleepAction(void *param)
{
    pcf_mode_t mode = NONE_PCF;
    xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);
    ESP_LOGE(TAG, "----GOING FOR SLEEP-----");
    e_wifi_mode event_received = NONE;
    xQueueSendToBack(xWifiQueue, &(event_received), (TickType_t)10);
    vTaskDelay(100);
    esp_deep_sleep_start();
}
/**
 * @brief
 *
 * @param event_data
 */
void SaveDataToFram(parse_event_send_t *event_data)
{
    printf(" SAVE :WRITE POINTER code  : %i \n", event_data->history_button_status);
    // printf(" SAVE :READ POINTER BEFORE WRITING  : %i \n",fram_config.read_pointer);
    // printf("TIME IN GOING TIN THE FILE %llu\n",event_data->time);
    // printf("BUTTON IN GOING TIN THE FILE %u\n",event_data->history_button_status);

    if (fram_config.write_pointer < OFFLINE_HSITORY_SIZE)
    {
        WriteToSpiffs(fram_config.write_pointer, sizeof(parse_event_send_t), (void *)event_data);
        fram_config.write_pointer++;
        fram_config.pending_event++;
    }
    PerformNvsFramParameter(true);
    // PrintUserConf(NULL);
}
/**
 * @brief
 *
 * @param event_data
 */
void RetriveDataFromFram(parse_event_send_t *event_data)
{
    //** fram interface but right now we are going to use the spiffs
    /* No Need to write the class information to fram...bcz we are only going to store history class*/
    // FramRead(fram_config.Read_pointer, (event_data + sizeof(parse_class_t)), sizeof(parse_event_send_t) - sizeof(parse_class_t));
    // fram_config.Read_pointer += (sizeof(parse_event_send_t) - sizeof(parse_class_t));
    // fram_config.pending_event--;
    //   printf(" RETRIVE :WRITE POINTER BEFORE WRITING  : %i \n",fram_config.write_pointer);
    // printf("RETRIVE :READ POINTER BEFORE WRITING  : %i \n",fram_config.read_pointer);
    ReadFromSpiffs(fram_config.read_pointer, sizeof(parse_event_send_t), (void *)event_data);
    fram_config.read_pointer++;
    fram_config.pending_event--;
    // printf("TIME IN GOING TIN THE FILE  AFTER %llu\n", event_data->time);
    // printf("BUTTON IN GOING TIN THE FILE  AFTER%u\n", event_data->history_button_status);
    if (fram_config.read_pointer == fram_config.write_pointer)
    {
        fram_config.read_pointer = 0;
        fram_config.write_pointer = 0;
        fram_config.pending_event = 0;
    }

    PerformNvsFramParameter(true);
    printf(" SAVE :READ Pinter code  : %i \n", event_data->history_button_status);

    // printf("STATUS FROM FILE AFTER  : %i", event_data->system_status);
}
/**
 * @brief Remotewell_PRO
 *
 * @param event_data
 * @return true
 * @return false
 */
bool ParseSend(parse_event_send_t *event_data)
{
    /*< First take the mutex to invert the task prio*/
    GetNetworkLock(NULL);

    uint8_t status_code = 0;
    switch (event_data->class_type)
    {
    case HISTORY_LIVE_CLASS:
    {
        // ESP_LOGI(TAG,"HISTORY & LIVE CLASS TRANSMIT");
        ESP_LOGI(TAG, " URI OF LIVE : %s ", parse.parse_uri.put_uri);
        event_data->class_type = LIVE_CLASS;
        ParsePakcetMaker(event_data);
        HttpPerform(parse.parse_uri.put_uri, PUT, &status_code, parse.parse_put_post_buff, &parse_header);
        //  ESP_LOGI(TAG,"-----> PACKET OF LIVE CLASS : \n%s",parse.parse_put_post_buff);

        free(parse.parse_put_post_buff);
        parse.parse_put_post_buff = NULL; // aviod from wild pointer

        ESP_LOGI(TAG, " URI OF HISTORY : %s ", parse.parse_uri.post_uri);
        event_data->class_type = HISTORY_CLASS;
        ParsePakcetMaker(event_data);
        HttpPerform(parse.parse_uri.post_uri, POST, &status_code, parse.parse_put_post_buff, &parse_header);
        free(parse.parse_put_post_buff);
        parse.parse_put_post_buff = NULL; // aviod from wild pointer

        event_data->class_type = HISTORY_LIVE_CLASS;
    }
    break;
    case HISTORY_CLASS:
    {
        ESP_LOGI(TAG, " --- ONLY HISTORY  CLASS TRANSMIT ----");
        ParsePakcetMaker(event_data);
        HttpPerform(parse.parse_uri.post_uri, POST, &status_code, parse.parse_put_post_buff, &parse_header);
        free(parse.parse_put_post_buff);
        parse.parse_put_post_buff = NULL; // aviod from wild pointer
    }
    break;
    case LIVE_CLASS:
    {
        ParsePakcetMaker(event_data);
        HttpPerform(parse.parse_uri.put_uri, PUT, &status_code, parse.parse_put_post_buff, &parse_header);
        free(parse.parse_put_post_buff);
        parse.parse_put_post_buff = NULL; // aviod from wild pointer
    }
    break;
    case VOLTAGE_TIME_CLASS:
    {
        ParsePakcetMaker(event_data);
        HttpPerform(parse.parse_uri.put_uri, PUT, &status_code, parse.parse_put_post_buff, &parse_header);
        free(parse.parse_put_post_buff);
        parse.parse_put_post_buff = NULL; // aviod from wild pointer
    }
    break;
    case INFO_CLASS:
    {
        ParsePakcetMaker(event_data);
        HttpPerform(parse.parse_uri.put_uri, PUT, &status_code, parse.parse_put_post_buff, &parse_header);
        free(parse.parse_put_post_buff);
        parse.parse_put_post_buff = NULL; // aviod from wild pointer
    }
    break;
    default:
        break;
    }
    ReturnNetworkLock(NULL);
    if (status_code != 200)
        ESP_LOGE(TAG, "---- PARSE CONTROLLER REQUEST FAIL ---- ");
    return (status_code == 200) ? true : false;
}
/**
 * @brief
 *
 * @param param
 */
void SendFramEventToParse(void *param)
{
    parse_event_send_t data_receive;

    // PrintUserConf(NULL);

    while (IsNetworkAvailable(NULL) && fram_config.pending_event)
    {
        // PrintUserConf(NULL);

        // ESP_LOGE(TAG, "FRAM NEW ROUTUNE HERE");

        if (xQueueReceive(xParseQueue, &(data_receive), (TickType_t)10) == pdPASS)
        {
            if (data_receive.class_type == HISTORY_CLASS || data_receive.class_type == HISTORY_LIVE_CLASS)
            {
                //** send data to fram
                SaveDataToFram(&data_receive);
                //** send live class status
                data_receive.class_type = LIVE_CLASS;
                if (!ParseSend(&data_receive))
                {
                    ESP_LOGE(TAG, "FAILED TO SEND DATA OFFLINE HISTORY");
                }
            }
        }
        else
        {
            PrintUserConf(NULL);

            bool status = false;
            parse_event_send_t fram_event;
            RetriveDataFromFram(&fram_event);

            fram_event.class_type = HISTORY_CLASS;
            status = ParseSend(&fram_event);

            /** if not sucessfull in the first try then second try if agin fail no way to save data uptill now !!! TODO */
            if (!status)
            {
                status = ParseSend(&fram_event);
            }
        }
    }
}
/**
 * @brief
 *
 * @param param
 */
void PerformOfflineMode(void *param)
{
    network_status_change = true;
    // PrintUserConf(NULL);
    //  ESP_LOGI(TAG,"GOING TO PERFORM OFFLINE MODE");
    if (uxQueueMessagesWaiting(xParseQueue) != 0)
    {
        ESP_LOGE(TAG, "---- DATA IN OFFLINE MODE FOUND ---");
        for (uint8_t i = 0; i < uxQueueMessagesWaiting(xParseQueue); i++)
        {
            parse_event_send_t data_receive;

            if (xQueueReceive(xParseQueue, &(data_receive), (TickType_t)10) == pdPASS)
            {
                (data_receive.class_type == HISTORY_CLASS || data_receive.class_type == HISTORY_LIVE_CLASS) ? SaveDataToFram(&(data_receive))
                                                                                                            : 1;
                (data_receive.history_button_status == ELECTRICITYOFF) ? SleepAction(NULL)
                                                                       : 1;
            }
        }
    }
}
/**
 * @brief
 *
 * @param param
 */
void PerformNetworkMode(void *param)
{
    //  bool status = false;
    network_status_change = false;
    /* if History in the fram is available */

    if (fram_config.pending_event)
    {
        //** now first send the app status
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        ESP_LOGE(TAG, "--- **** GOING TO PROCESS FRAM EVENT ***** ---");
        SendFramEventToParse(NULL);
    }
    /* Previous hiostory just go smooth */
    else
    {
        parse_event_send_t data_receive;
        if (xQueueReceive(xParseQueue, &(data_receive), (TickType_t)10) == pdPASS)
        {
            ESP_LOGE(TAG, "---  ***** NETWORK MODE EVENT FOUND GOING TO SEND THE DATA TO PARSE ****---");
            if (memcmp(&prev_data, &data_receive, sizeof(parse_event_send_t)) != 0)
            {
                /*< if failed then again save the data to >*/
                if (!ParseSend(&data_receive))
                {
                    /* Here Some Issue is found in the system probably server is down here !! NOT Handle Yet data Loss chance */

                    xQueueSendToFront(xParseQueue, &(data_receive), (TickType_t)100);
                }
                else
                {
                    memcpy(&prev_data, &data_receive, sizeof(parse_event_send_t));
                    /* here only if electricity is send to server is sucessfully*/
                    (data_receive.history_button_status == ELECTRICITYOFF) ? SleepAction(NULL)
                                                                           : 1;
                }
            }
            else
            {
                /* repeted history*/
            }
        }
    }
}

/**
 * @brief
 *
 * @param param
 * @return working_mode_t
 */

working_mode_t ChosseWorkingMode(void *param)
{
    if (IsNetworkAvailable(NULL))
    {
        return NETWORK_MODE;
    }
    else
    {
        return OFFLINE_MODE;
    }
    return NONE;
}
/**
 * @brief
 *
 * @param param
 */
void ParseControllerTask(void *param)
{

    /**< initial setting >*/

    working_mode_t working_mode;
    while (1)
    {

        /* Control loop*/
        while (1)
        {
            /*If no user is verifed donot stay here*/
            if (!network_config.verified_user)
                break;
            /*  -------------------> */

            working_mode = ChosseWorkingMode(NULL);
            switch (working_mode)
            {
            case NETWORK_MODE:
                network_status_change ? vTaskDelay(3000 / portTICK_PERIOD_MS) : 1; //** necessary for network recovery
                PerformNetworkMode(NULL);
                break;
            case OFFLINE_MODE:
                PerformOfflineMode(NULL);
                break;
            // case NONE:
            //     /* Uptill Now Empty but we can handle some malfunction here*/
            //     break;
            default:
                break;
            }
            printf("FREE HEAP AVAILABLE :%i\n", esp_get_free_heap_size());
            vTaskDelay(500 / portTICK_PERIOD_MS);

            /* <----------------*/
        }
        ESP_LOGI(TAG, "---- PLEASE GO FOR VERIFIED USER -----");
        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}
/**
 * @brief
 *
 * @param param
 */
void ParseControllerInit(void *param)
{
    /*<  initilization here >*/

    xTaskCreatePinnedToCore(ParseControllerTask,
                            "parse_controller_task",
                            PARSECONTOLLER_TASK_STACK_SIZE,
                            NULL, PARSECONTOLLER_TASK_PRIO,
                            &xParseControllerTask,
                            PARSECONTOLLER_TASK_CORE);
    // ReadMacSTA();
}
