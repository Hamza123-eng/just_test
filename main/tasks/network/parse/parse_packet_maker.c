#include <string.h>
#include <stdlib.h>
#include "esp_log.h"
#include "cJSON.h"
#include "parse.h"
#include "hlw8032.h"
#include "system_time.h"
#include "parse_event_controller.h"
#include "parse.h"
#include "freertos_handler.h"
#include "hw_monitor.h"

static const char *TAG = "PARSE PACKET MAKER []";

/**
 * @brief
 *
 * @param to_send
 * @param button_status
 * @param motor_status
 * @param trip_status
 * @param undermnt
 * @param alive_status
 * @param phase_error
 * @param electricity_st
 */

/**
 * @brief
 *
 * @param param
 */
void HistoryClassParsePacket(parse_event_send_t *event_data)
{
    cJSON *packet_to_send = cJSON_CreateObject();

    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[motor_button], event_data->history_button_status);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[motor_status], event_data->system_status & MOTOR_STATUS_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[motor_trip_status], event_data->system_status & TRIP_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[under_mnt_status], event_data->system_status & UNDERMNT_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[electricity_error_status], event_data->system_status & ELECTRICITY_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[voltage_error], event_data->system_status & VOLTAGE_ERROR_BIT);
    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[run_time], event_data->run_time);
    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[history_time], event_data->time);

    if (!strcmp(event_data->phone_tag, ""))
        sprintf(event_data->phone_tag, "%s", "NO-TAG");
    if (!strcmp(event_data->tag, ""))
        sprintf(event_data->tag, "%s", "NO-TAG");

    cJSON_AddStringToObject(packet_to_send, parse_data_name_array[phone_tag], event_data->phone_tag);
    cJSON_AddStringToObject(packet_to_send, parse_data_name_array[tag], event_data->tag);

    // if (event_data->system_status & PHASE_ERROR_BIT)
    // {
    uint8_t dummy = xEventGroupGetBits(xPhaseEvent);
    dummy = 0x07 & ~dummy; // first revert then send only first three bits
    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[phase_error_code], dummy);
    // }
    // else
    // {
    //     cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[phase_error_code], 0);
    // }

    char *print_packet = cJSON_Print(packet_to_send);

    parse.parse_put_post_buff = (char *)malloc(strlen(print_packet) + 1);
    memset(parse.parse_put_post_buff, 0, strlen(print_packet) + 1);

    sprintf(parse.parse_put_post_buff, "%s", print_packet);

    free(print_packet);

    cJSON_Delete(packet_to_send);
    ESP_LOGI(TAG, "-----> PACKET OF HSTORY CLASS : \n%s", parse.parse_put_post_buff);
}
/**
 * @brief
 *
 */
void InfoParsePacket(parse_event_send_t *event_data)
{
    cJSON *packet_to_send = cJSON_CreateObject();

    // char buff[64];

    cJSON_AddStringToObject(packet_to_send, parse_data_name_array[device_mac], parse.mac_address);
    cJSON_AddStringToObject(packet_to_send, "hardware_version", device_config.hardware_version);
    cJSON_AddStringToObject(packet_to_send, "serial_number", device_config.model_data);
    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[motor_button], 0);
    
    char *print_packet = cJSON_Print(packet_to_send);

    parse.parse_put_post_buff = (char *)malloc(strlen(print_packet) + 1);
    memset(parse.parse_put_post_buff, 0, strlen(print_packet) + 1);

    sprintf(parse.parse_put_post_buff, "%s", print_packet);

    free(print_packet);

    cJSON_Delete(packet_to_send);
}
/**
 * @brief
 *
 * @param param
 */
void LiveClassParsePacket(parse_event_send_t *event_data)
{
    cJSON *packet_to_send = cJSON_CreateObject();

    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[motor_status], event_data->system_status & MOTOR_STATUS_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[motor_trip_status], event_data->system_status & TRIP_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[under_mnt_status], event_data->system_status & UNDERMNT_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[electricity_error_status], event_data->system_status & ELECTRICITY_BIT);
    // cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[electricity_error_status], ~(event_data->system_status & ELECTRICITY_BIT));
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[voltage_error], event_data->system_status & VOLTAGE_ERROR_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[smart_switch_status], event_data->system_status & SMART_SWITCH_STATUS_BIT);
    cJSON_AddStringToObject(packet_to_send, parse_data_name_array[device_mac], parse.mac_address);

    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[smart_switch], 0);
    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[motor_button], 0);
    // cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[ping_time], GetStandardTime(false));

    cJSON_AddStringToObject(packet_to_send, "hardware_version", device_config.hardware_version);

    // cJSON_AddNumberToObject(packet_to_send, "serial_number", hlw_voltage.phase_voltage[2]);

    /* just for the elcetricity checks */
    if (event_data->system_status & ELECTRICITY_BIT)
    {
    uint8_t dummy = xEventGroupGetBits(xPhaseEvent);
        dummy = 0x07 & ~dummy; // first revert then send only first three bits
        cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[phase_error_code], dummy);
    }
    else
    {
        cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[voltage_p1], 0);
        cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[voltage_p2], 0);
        cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[voltage_p3], 0);
        cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[phase_error_code], 0);
    }
    // }
    // else
    // {
    //     cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[phase_error_code], 0);
    // }

    char *print_packet = cJSON_Print(packet_to_send);

    parse.parse_put_post_buff = (char *)malloc(strlen(print_packet) + 1);
    memset(parse.parse_put_post_buff, 0, strlen(print_packet) + 1);

    sprintf(parse.parse_put_post_buff, "%s", print_packet);

    free(print_packet);

    cJSON_Delete(packet_to_send);

    // ESP_LOGI(TAG, "-----> PACKET OF LIVE CLASS : \n%s", parse.parse_put_post_buff);
}
/**
 * @brief
 *
 * @param param
 */
void VoltageTimeParsePacket(parse_event_send_t *event_data)
{
    cJSON *packet_to_send = cJSON_CreateObject();

    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[smart_switch], 0);
    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[motor_button], 0);

    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[smart_switch_status], event_data->system_status & SMART_SWITCH_STATUS_BIT);
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[motor_status], event_data->system_status & MOTOR_STATUS_BIT);

    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[ping_time], GetStandardTime(false));

    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[voltage_p1], hlw_voltage.phase_voltage[0]);
    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[voltage_p2], hlw_voltage.phase_voltage[1]);
    cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[voltage_p3], hlw_voltage.phase_voltage[2]);
    
    cJSON_AddBoolToObject(packet_to_send, parse_data_name_array[electricity_error_status], event_data->system_status & ELECTRICITY_BIT);

    //** only for the reason when phase safety off so phase event will update from here

    
        uint8_t dummy = xEventGroupGetBits(xPhaseEvent);
        dummy = 0x07 & ~dummy; // first revert then send only first three bits
        cJSON_AddNumberToObject(packet_to_send, parse_data_name_array[phase_error_code], dummy);
    
    char *print_packet = cJSON_Print(packet_to_send);

    parse.parse_put_post_buff = (char *)malloc(strlen(print_packet) + 1);
    memset(parse.parse_put_post_buff, 0, strlen(print_packet) + 1);

    sprintf(parse.parse_put_post_buff, "%s", print_packet);
    // ESP_LOGI(TAG, "-----> PACKET OF VOLTAGE TIME CLASS CLASS : \n%s", parse.parse_put_post_buff);

    free(print_packet);

    cJSON_Delete(packet_to_send);
}
/**
 * @brief
 *
 */
void ParsePakcetMaker(parse_event_send_t *event_data)
{

    switch (event_data->class_type)
    {
    case HISTORY_CLASS:
        HistoryClassParsePacket(event_data);
        break;
    case LIVE_CLASS:
        LiveClassParsePacket(event_data);
        break;
    case VOLTAGE_TIME_CLASS:
        VoltageTimeParsePacket(event_data);
        break;
    case INFO_CLASS:
        InfoParsePacket(event_data);
        break;
    default:
        break;
    }
}
