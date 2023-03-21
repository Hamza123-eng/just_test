#ifndef _HW_MONITOR_H
#define _HW_MONITOR_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
// #include "freertos/message_buffer.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "sdkconfig.h"

#define MAX_FAULT_COUNT 5

#define WRITE 1
#define READ 0

#define NC_RELAY_OPEN 0
#define NC_RELAY_CLOSE 1

#define NC_OPEN ON
#define NC_CLOSE OFF

#define NO_OPEN OFF
#define NO_CLOSE ON

#define NO_RELAY_OPEN NO_OPEN
#define NO_RELAY_CLOSE NO_CLOSE
#define NC_RELAY_OPEN_RFID 2

extern bool time_update;
/**
 * @brief NormallyOpenRelayTask() is FreeRTOS thread that implements the finite state machine for Normally Open Relay. State changes depending on the input signal.
 * Sends high or low signal to GPIO pin of Normally Open Relay
 * @param void *ptr
 * @return None
 */
void NormallyOpenRelayTask(void *ptr);
/**
 * @brief NormallyCloseRelayTask() is FreeRTOS thread that implements the finite state machine for Normally Close Relay. State changes depending on the input signal.
 * Sends high or low signal to GPIO pin of Normally Close Relay
 * @param void *ptr
 * @return None
 */
void NormallyCloseRelayTask(void *ptr);
/**
 * @brief Read current input and update the structure of parse
 * @param None
 * @return None
 */
void ReadCurrentHWState();

void ToggleWifiLed(bool state);
void ToggleMntLed(bool state);
void ToggleBuzzer(bool state);

/**
 * @brief This fuction is used for last connection update to server from device side.
 *
 */
void PingNotification();

typedef struct
{
    uint8_t idle;
    uint8_t app;
    uint8_t manual;
    uint8_t trip;
    uint8_t undermnt;
    uint8_t phase;
    uint8_t schedule;
    uint8_t run;
} Inputs_t;

typedef enum
{
    APP = 0x01,
    MANNUEL,
    SCH,
    PHASE,
    TRIP,
    MNT,
    VOLTAGE,
    AUTOMODE
} last_action_t;

typedef struct
{
    /*< Related to VFD Control >*/

    uint8_t normal_device_mode; // vfd (0) or normal(1)
    uint8_t latch_vfd;          // simple vfd (0) and
    uint8_t vfd_latch_time;     // time for vfd latch

    /*< Related to voltage >*/

    uint16_t under_volt_lim;
    uint16_t over_volt_lim;

    /*< Smart Switch >*/

    uint8_t smart_switch;

    /*< Auto Mode >*/

    uint8_t auto_mode;

    uint8_t safety_mode; // safety mode

    uint8_t phase_fault; // phase fault on or not

    uint8_t last_motor_run_state; // last motor state

    uint8_t last_switch_run_state; // last motor state

    uint8_t last_electricity_status; // save wheter elctricity is gone

    uint8_t last_mnt_status;

    uint8_t automode_fault_count;

    char hardware_version[8];
    uint32_t hardware_serial;

    char model_data[32];

} device_config_t;

void HardwareCoreInit(void *param);
void PerformRealTimeSafetyMode(uint8_t safety_mode);
uint8_t WrapSystemStatus(void *param);

extern device_config_t device_config;

extern bool soft_error_in_system;
extern volatile bool buzzer_level;

#endif // HW_MONITOR_H
