#ifndef EVENT_CONTROLLER_H_
#define EVENT_CONTROLLER_H_

typedef enum
{
     NETWORK_MODE,
     OFFLINE_MODE
} working_mode_t;

typedef enum
{
     TRIP_BIT = 0x01,
     PHASE_ERROR_BIT = 0x02,
     UNDERMNT_BIT = 0x04,
     MOTOR_STATUS_BIT = 0x08,

     VOLTAGE_ERROR_BIT = 0x10,
   
     ELECTRICITY_BIT = 0x40,
     SMART_SWITCH_STATUS_BIT = 0x80
} device_status_bits_t;

typedef enum
{
     HISTORY_LIVE_CLASS,
     HISTORY_CLASS,
     LIVE_CLASS,
     VOLTAGE_TIME_CLASS,
     INFO_CLASS,
     SCHEDULE_CLASS
} parse_class_t;

typedef struct
{
     parse_class_t class_type;
     uint8_t history_button_status;
     uint8_t system_status;
     uint64_t time;
     uint32_t run_time;
     char tag[16];
     char phone_tag[16];
} parse_event_send_t;

void ParseControllerInit(void *param);
#endif