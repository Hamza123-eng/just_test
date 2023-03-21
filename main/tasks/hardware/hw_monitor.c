

#include <string.h>
#include "stdio.h"
#include "stdlib.h"
#include "esp_sleep.h"

#include "hw_monitor.h"
#include "system_gpio.h"
#include "freertos_handler.h"
#include "parse_event_controller.h"
#include "phase_sensing.h"
#include "parse.h"
#include "indications.h"
#include "nvs_storage.h"
#include "schedule_estimate.h"
#include "hlw8032.h"
#include "system_time.h"
#include "wifi_manger.h"
#include "pcf.h"
#include "parse_websocket.h"

#define NONE 0x00

device_config_t device_config = {1, 0, 10, 170, 260, 0, 0, 1, 1, 0, 0, 0, 0, 0, {0x00}, 0, {0x00}};

live_class_t live_class = {0, 0, 0, 0, 0, 0, 0, 0};
live_class_t prev_live_class = {0, 0, 0, 0, 1, 0, 0, 0};

last_action_t fault_array[6] = {0x00};

TaskHandle_t xHardwareCoreTask = NULL;
TaskHandle_t xRelayControlTask = NULL;

QueueHandle_t xRelayQueue = NULL;
QueueHandle_t xVFDQueue = NULL;
QueueHandle_t xParseQueue = NULL;
QueueHandle_t xAutoModeQueue = NULL;
QueueHandle_t xButtonQueue = NULL;

void SaveFaultPostAction(void *param);
static bool FaultArrayOperation(last_action_t action, bool insert);

static const char *TAG = "HW_MONITOR[]";

last_action_t last_run_action = NONE;
last_action_t last_off_action = NONE;

uint8_t fault_array_index = 0;
uint8_t schedule_cross_run = 0;

bool soft_error_in_system = false;
bool schedule_run = false;
/**
 *
 */
void SuspendSystem(void *param)
{
    (xPhaseTask != NULL) ? vTaskSuspend(xPhaseTask) : 1;
    (xHlwTask != NULL) ? vTaskSuspend(xHlwTask) : 1;
    (xScheduleTask != NULL) ? vTaskSuspend(xScheduleTask) : 1;
    (xButtonTask != NULL) ? vTaskSuspend(xButtonTask) : 1;
    (xBuzzerTask != NULL) ? vTaskSuspend(xBuzzerTask) : 1;
    (xHawkbitTask != NULL) ? vTaskSuspend(xHawkbitTask) : 1;
<<<<<<< HEAD
    websocket_app_stop();
    websocket_sch_stop();
=======
     websocket_app_stop();
     websocket_sch_stop();
>>>>>>> b9dad1e1f8498dc2141c089c0bc981309957ec93
}
/**
 * @brief
 *
 * @param safety_mode
 */
void PerformRealTimeSafetyMode(uint8_t safety_mode)
{
    if (safety_mode)
    {
        if (soft_error_in_system)
        {
            live_class.phase_error ? FaultArrayOperation(PHASE, WRITE) : true;
            live_class.voltage_error ? FaultArrayOperation(VOLTAGE, WRITE) : true;
            live_class.trip_status ? FaultArrayOperation(TRIP, WRITE) : true;
        }
        else
        {
            // no thing is need to perform here .. no fault in system
        }
    }
    else
    {
        /* Safety mode is going to disable but be sure to deassert the fault array*/
        if (fault_array_index)
        {
            /*iterate all the condition irrespective of mnt*/
            FaultArrayOperation(PHASE, READ);
            FaultArrayOperation(VOLTAGE, READ);
            FaultArrayOperation(TRIP, READ);
        }
    }
}
/**
 * @brief
 *
 * @param param
 * @return uint8_t
 */
uint8_t WrapSystemStatus(void *param)
{
    uint8_t status = 0;

    status = live_class.motor_status ? status | MOTOR_STATUS_BIT : status;

    status = live_class.trip_status ? status | TRIP_BIT : status;

    status = live_class.undermnt ? status | UNDERMNT_BIT : status;

    status = (Electricity_ST != 0) ? status | ELECTRICITY_BIT : status;

    status = live_class.phase_error ? status | PHASE_ERROR_BIT : status;

    status = live_class.smart_switch_status ? status | SMART_SWITCH_STATUS_BIT : status;

    status = live_class.voltage_error ? status | VOLTAGE_ERROR_BIT : status;

    return status;
}
/**
 * @brief
 *
 */
void PerformElectricityOff(void *param)
{
    device_config.last_switch_run_state = live_class.smart_switch_status;
    device_config.last_electricity_status = true;
    device_config.last_mnt_status = live_class.undermnt;

    if (prev_live_class.motor_status && last_run_action != SCH)
    {
        device_config.last_motor_run_state = prev_live_class.motor_status;
    }
    if (uxQueueSpacesAvailable(xAutoModeQueue) == 0)
    {
        device_config.last_motor_run_state = true;
    }

    /** Perform the Nvs Electricty off*/

    PerformNvsElectricity(true);
    /* switch status will be off at electricity off*/
    live_class.smart_switch_status = false;
}

/**
 * @brief
 *
 */
void PerformAutoModeInit(void *param)
{

    // static bool Auto_mode_perfrormed = false;

    while (!network_config.go_automode)
    {
        // ESP_LOGI(TAG, "--- WAITING FOR AUTOMODE STROBE---");
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
    // ESP_LOGI(TAG, " --- PERFORMING AUTOMODE STROBE ---");

    // Auto_mode_perfrormed = true;
    network_config.go_automode = 0;

    /* if global setting is open */

    if (device_config.auto_mode)
    {
        if (device_config.last_motor_run_state)
        {
            ESP_LOGI(TAG, "****MOTOR WILL RUN AFTER EECTRICTY******");

            bool input = true;
            xQueueSendToBack(xAutoModeQueue, &(input), (TickType_t)10);
            device_config.last_motor_run_state = 0;
        }
        if (device_config.last_switch_run_state)
        {
            // ESP_LOGI(TAG,"****SMART AFTER ELEC  SENT COMMAND******");

            bool input = true;
            xQueueSendToBack(xVFDQueue, &(input), (TickType_t)100);
            device_config.last_switch_run_state = 0;
        }
    }
    /* Reversial condition here !!! if removed panic in ststatus of elctricity on system*/

    if (device_config.last_electricity_status)
    {
        // ESP_LOGI(TAG,"****ELCECTRICITY STATUS SENT COMMAND******");
        live_class.elcectricity = true;
        prev_live_class.elcectricity = false;
    }
    /* need to on the maintince but no status is needed*/
    if (device_config.last_mnt_status)
    {
        soft_error_in_system = true;

        FaultArrayOperation(MNT, WRITE);
        //  SaveFaultPostAction(NULL);
        /* no need for history*/
        live_class.undermnt = true;
        prev_live_class.undermnt = true;
        last_off_action = MNT;
    }

    pcf_mode_t mode = RUNNING_MODE;
    xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);
}

static void StartRunTime(void *param)
{
    // going to use the prevoius class run time on
    prev_live_class.run_time = 0;
    prev_live_class.run_time = xTaskGetTickCount();
}
/**
 * @brief
 *
 */
uint64_t StopRunTime(void *param)
{
    if (prev_live_class.run_time != 0)
    {
        uint64_t dummy = 0;
        dummy = xTaskGetTickCount() - prev_live_class.run_time;
        live_class.run_time = dummy / (100);
        prev_live_class.run_time = 0;
        return live_class.run_time;
    }

    return 0;
}
/**
 * @brief
 *
 * @param param
 */
void SendTimeVoltage(void *param)
{

    /*< first of all conform the time is correct>*/

    parse_event_send_t data_to_send = {.class_type = VOLTAGE_TIME_CLASS,
                                       .history_button_status = 0,
                                       .system_status = WrapSystemStatus(NULL),
                                       .time = GetStandardTime(false),
                                       .run_time = 0,
                                       .tag = "",
                                       .phone_tag = ""};
    xQueueSendToBack(xParseQueue,
                     (void *)&data_to_send,
                     (TickType_t)10);
}
/**
 * @brief
 *
 */

/**
 * @brief This function initializes all the queues that are being used between different threads to send
 * and receive data.
 * @param None
 * @return None
 */
void SaveFaultPostAction(void *param)
{
    ESP_LOGI(TAG, "Saving fault POST OPERATION");
    if (last_run_action == SCH)
    {
        schedule_cross_run = 0;
        schedule_event_t schedule_event = SCHEDULE_HALT_BY_FAULT;
        xQueueSendToFront(xScheduleInQueue, &(schedule_event), (TickType_t)100);
        last_run_action = NONE;
    }
    else if (device_config.auto_mode)
    {
        // ESP_LOGI(TAG, "LAST RUN BY: %i",last_run_action);
        if (last_run_action == AUTOMODE || last_run_action == APP || last_run_action == MANNUEL)
        {
            ESP_LOGI(TAG, "GOING TO SAVE FAULT IN AUTOMODE ARRAY");
            bool input = true;
            xQueueSendToBack(xAutoModeQueue, &(input), (TickType_t)10);
            last_run_action = NONE;
        }
        else
        {
            /* Motor is not Running previously just the Fault Occur */
        }
    }
    else
    {
        // neither schedule nor auto/mannuel/app
    }
}

/**
 * @brief
 *
 * @param Button_state
 */
void SendDataToParseController(parse_class_t class_type, uint8_t Button_state)
{

    parse_event_send_t data_to_send = {.class_type = class_type,
                                       .history_button_status = Button_state,
                                       .system_status = WrapSystemStatus(NULL),
                                       .time = GetStandardTime(false),
                                       .run_time = StopRunTime(NULL),
                                       .tag = "",
                                       .phone_tag = ""};
    if (xParseQueue != NULL)
    {
        xQueueSendToBack(xParseQueue,
                         (void *)&data_to_send,
                         (TickType_t)10);
    }
}
/**
 * @brief Remotewell_PRO
 *
 * @param action
 * @param insert
 * @return true
 * @return false
 */
static bool FaultArrayOperation(last_action_t action, bool insert)
{
    /* Enquueue the error to error to phase array */
    if (insert)
    {
        ESP_LOGI(TAG, "IN FAULT ARRAY INSERT");
        fault_array_index++;
        fault_array[fault_array_index] = action;

        /*< Ensure to accert Hard Falut here >*/
        gpio_set_level(OFF_RELAY_PIN, ON);

        /* in the case of lacthced*/

        if (device_config.latch_vfd)
        {
            /*VFD ON IS ON R3*/
            gpio_set_level(VFD_RELAY_PIN, OFF);
        }
        else
        {
            /* NO latched Vfd is found so go */
            gpio_set_level(ON_RELAY_PIN, OFF);
        }

        return true;
    }
    else
    {
        for (uint8_t index = 1; index < fault_array_index + 1; index++)
        {
            if (fault_array[index] == action)
            {
                ESP_LOGI(TAG, "IN FAULT ARRAY DEASSERT");
                fault_array[index] = fault_array[fault_array_index];
                fault_array[fault_array_index] = NONE;
                fault_array_index--;

                /*< Deasscert Hard Falut here >*/

                fault_array_index ? 1 : gpio_set_level(OFF_RELAY_PIN, OFF);
                ESP_LOGI(TAG, "ARRAY FAULT INDEX : %i", fault_array_index);
                return true;
            }
            else
            {
                /* No Action Got map at iterator index */
            }
        }
    }
    //  PrintFaultArray(NULL);
    return false;
}
/**
 * @brief
 *
 * @param data_to_send
 */
void AttachScheduleTag(parse_event_send_t *data_to_send)
{

    sprintf(data_to_send->tag, "%s", last_schedule_record.schedule_tag);
    sprintf(data_to_send->phone_tag, "%s", last_schedule_record.schedule_phone);

    /*< clearing the schdule tag / phone >*/

    sprintf(last_schedule_record.schedule_tag, "%s", "");
    sprintf(last_schedule_record.schedule_phone, "%s", "");
}
/**
 * @brief Remotewell_PRO
 *
 */
void OperateRealy(void *param)
{

    bool relay = false;
    bool vfd_relay = false;
    uint8_t voltage_time_count = 55;

    while (1)
    {
        if (!network_config.verified_user)
            goto end;
        switch (device_config.normal_device_mode)
        {
        case 1:
        {
            if (xRelayQueue != NULL)
            {
                if (xQueueReceive(xRelayQueue, &(relay), (TickType_t)0) == pdPASS)
                {
                    relay ? gpio_set_level(ON_RELAY_PIN, ON) : gpio_set_level(OFF_RELAY_PIN, ON);
                    vTaskDelay(4000 / portTICK_PERIOD_MS);
                    relay ? gpio_set_level(ON_RELAY_PIN, OFF) : gpio_set_level(OFF_RELAY_PIN, OFF);
                    SendTimeVoltage(NULL);
                    voltage_time_count = 0;
                }
            }
            if (xVFDQueue != NULL)
            {
                if (xQueueReceive(xVFDQueue, &(vfd_relay), (TickType_t)0) == pdPASS)
                {
                    vfd_relay ? gpio_set_level(VFD_RELAY_PIN, ON) : gpio_set_level(VFD_RELAY_PIN, OFF);
                    vfd_relay ? live_class.smart_switch_status = 1 : 1;
                    !vfd_relay ? live_class.smart_switch_status = 0 : 1;
                    SendTimeVoltage(NULL);
                    voltage_time_count = 0;
                }
            }
            break;
        }
        default:
            if (device_config.latch_vfd)
            {
                // ESP_LOGI(TAG, "IN LATEHED VFD");

                //  ON --> CONTACT ,OFF --> CONTACT, SWITCH --> VFD
                if (xQueueReceive(xRelayQueue, &(relay), (TickType_t)0) == pdPASS)
                {
                    relay ? gpio_set_level(ON_RELAY_PIN, ON) : gpio_set_level(VFD_RELAY_PIN, OFF);
                    relay ? vTaskDelay(1000 * (device_config.vfd_latch_time) / portTICK_PERIOD_MS) : vTaskDelay(2000 / portTICK_PERIOD_MS);
                    if (relay)
                    {
                        gpio_set_level(VFD_RELAY_PIN, ON);
                        gpio_set_level(ON_RELAY_PIN, OFF);
                    }
                    else
                    {
                        gpio_set_level(OFF_RELAY_PIN, ON);
                        vTaskDelay(4000 / portTICK_PERIOD_MS);
                        gpio_set_level(OFF_RELAY_PIN, OFF);
                    }
                    SendTimeVoltage(NULL);
                    voltage_time_count = 0;
                }
                /* Discard The event of if lacthed relay */
                xQueueReceive(xVFDQueue, &(vfd_relay), (TickType_t)0);
            }
            else
            {
                if (xQueueReceive(xRelayQueue, &(relay), (TickType_t)0) == pdPASS)
                {
                    relay ? gpio_set_level(ON_RELAY_PIN, ON) : gpio_set_level(ON_RELAY_PIN, OFF);
                    SendTimeVoltage(NULL);
                    voltage_time_count = 0;
                }
                if (xQueueReceive(xVFDQueue, &(vfd_relay), (TickType_t)0) == pdPASS)
                {
                    vfd_relay ? gpio_set_level(VFD_RELAY_PIN, ON) : gpio_set_level(VFD_RELAY_PIN, OFF);
                    vfd_relay ? live_class.smart_switch_status = 1 : 1;
                    !vfd_relay ? live_class.smart_switch_status = 0 : 1;
                    // send status of smart switch by just sending the Time  & voltage
                    SendTimeVoltage(NULL);
                    voltage_time_count = 0;
                }
            }
            break;
        }

        /* only for volatge status to cloud*/
        voltage_time_count++;

        if (voltage_time_count >= 60)
        {
            if (xEventGroupGetBits(xPhaseEvent) & ELECTRICITY_ST)
            {
                if (uxQueueSpacesAvailable(xParseQueue) >= 5)
                    SendTimeVoltage(NULL);
            }
            hlw_voltage.data_ready = false;
            voltage_time_count = 0;
        }

    end:
        vTaskDelay(200 / portTICK_PERIOD_MS);
    }
}
/**
 * @brief
 *
 * @param event
 */
void ParseSendLiveClass(device_system_event_t event)
{
    bool relay_status_to_send;

    /*< Faults Will be on Top Piority >*/
    switch (event)
    {
    case ELECTRICITYON:
    {
        ESP_LOGI(TAG, "ELECTRICTY ON");

        /** Action that needs to perform for electricity on*/

        /*-------------------*/

        /*< status to send>*/
        SendDataToParseController(HISTORY_LIVE_CLASS, ELECTRICITYON);
        /*-----------------*/
    }
    break;
    case ELECTRICITYOFF:
    {
        ESP_LOGI(TAG, "ELECTRICTY OFF");

<<<<<<< HEAD
        SuspendSystem(NULL);

        pcf_mode_t mode = NONE_PCF;
        xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);

=======
       SuspendSystem(NULL);
  pcf_mode_t mode = NONE_PCF;
    xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);
>>>>>>> b9dad1e1f8498dc2141c089c0bc981309957ec93
        // Action that needs to perform for electricity off
        PerformElectricityOff(NULL);
        /*-------------------*/

        // //***   just for testing dummy sleep
        // ESP_LOGI(TAG, "------STARTING DEEP SLEEP-----");
        // e_wifi_mode event_received = NONE;
        // xQueueSendToBack(xWifiQueue, &(event_received), (TickType_t)100);

        // // vTaskDelay(100);
        // esp_deep_sleep_start();
        ////***

        /*< status to send>*/
        SendDataToParseController(HISTORY_LIVE_CLASS, ELECTRICITYOFF);
        /*-----------------*/

        /*Trap electricity is gone*/
        while (1)
        {
            ESP_LOGE(TAG, "--- HARDWARE TASK IS TRAP DUE  TO ELEC ---");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    break;
    case UNDERMNTON:
    {
        ESP_LOGI(TAG, "UNDERMNT ON");

        FaultArrayOperation(MNT, WRITE);

        /* Ensuring the Error Post Operation */
        /*-------------------*/

        /*< status to send>*/
        //** This status is only for the reason when motor is on previouly on the also send the status
        last_run_action == NONE ? SendDataToParseController(HISTORY_LIVE_CLASS, UNDERMNTON) : 1;
        last_run_action != NONE ? last_off_action = MNT : 1;
        /*-----------------*/
        SaveFaultPostAction(NULL);
    }
    break;
    case UNDERMNTOFF:
    {
        ESP_LOGI(TAG, "UNDERMNT OFF");

        FaultArrayOperation(MNT, false);

        /*< status to send>*/

        SendDataToParseController(HISTORY_LIVE_CLASS, UNDERMNTOFF);

        /**-----------------*/
    }

    break;
    case PHASEERRORON:
    {
        ESP_LOGI(TAG, "PHASE ERROR ON");

        relay_status_to_send = false;
        device_config.safety_mode ? FaultArrayOperation(PHASE, WRITE) : 1;
        if (!device_config.safety_mode && !fault_array_index) // just to validate the condition if undermant prevous
            xQueueSendToBack(xRelayQueue, (void *)&relay_status_to_send, (TickType_t)100);

        /*< status to send>*/
        //** This status is only for the reason when motor is on previouly on the also send the status
        last_run_action == NONE ? SendDataToParseController(HISTORY_LIVE_CLASS, PHASEERRORON) : 1;
        last_run_action != NONE ? last_off_action = PHASE : 1;
        /**-----------------*/

        /* Ensuring the Error Post Operation */

        SaveFaultPostAction(NULL);

        /*-------------------*/
    }
    break;
    case PHASEERROROFF:
    {
        ESP_LOGI(TAG, "PHASE ERROR OFF");

        FaultArrayOperation(PHASE, READ);

        /*< status to send >*/

        SendDataToParseController(HISTORY_LIVE_CLASS, PHASEERROROFF);

        /**-----------------*/
    }
    break;
    case TRIPON:
    {
        ESP_LOGI(TAG, "TRIP ON ");
        relay_status_to_send = false;
        device_config.safety_mode ? FaultArrayOperation(TRIP, WRITE) : 1;
        if (!device_config.safety_mode && !fault_array_index) // just to validate the condition if undermant prevous
            xQueueSendToBack(xRelayQueue, (void *)&relay_status_to_send, (TickType_t)100);

        /*< status to send>*/

        //** This status is only for the reason when motor is on previouly on the also send the status
        last_run_action == NONE ? SendDataToParseController(HISTORY_LIVE_CLASS, TRIPON) : 1;
        last_run_action != NONE ? last_off_action = TRIP : 1;

        // SendDataToParseController(HISTORY_LIVE_CLASS, TRIPON);

        /*-----------------*/

        /* Ensuring the Error Post Operation */

        SaveFaultPostAction(NULL);

        /*-------------------*/
    }
    break;
    case TRIPOFF:
    {
        ESP_LOGI(TAG, "TRIP OFF");

        FaultArrayOperation(TRIP, READ);

        /*< status to send>*/

        SendDataToParseController(HISTORY_LIVE_CLASS, TRIPOFF);

        /*-----------------*/
    }
    break;
    case UNDERVOLTON:
    {
        ESP_LOGI(TAG, "UNDERVOLTAGE ERROR OFF");

        relay_status_to_send = false;
        device_config.safety_mode ? FaultArrayOperation(VOLTAGE, WRITE) : 1;
        if (!device_config.safety_mode && !fault_array_index) // just to validate the condition if undermant prevous
            xQueueSendToBack(xRelayQueue, (void *)&relay_status_to_send, (TickType_t)100);

        /*< status to send>*/
        //** This status is only for the reason when motor is on previouly on the also send the status
        last_run_action == NONE ? SendDataToParseController(HISTORY_LIVE_CLASS, UNDERVOLTON) : 1;
        last_run_action != NONE ? last_off_action = VOLTAGE : 1;

        /*-----------------*/
        /* Ensuring the Error Post Operation */

        SaveFaultPostAction(NULL);

        /*-------------------*/
    }
    break;
    case OVERVOLTON:
    {
        ESP_LOGI(TAG, "OVERVOLTAGE ERROR ON");

        relay_status_to_send = false;
        device_config.safety_mode ? FaultArrayOperation(VOLTAGE, WRITE) : 1;
        if (!device_config.safety_mode && !fault_array_index) // just to validate the condition if undermant prevous
            xQueueSendToBack(xRelayQueue, (void *)&relay_status_to_send, (TickType_t)100);

        /*< status to send>*/
        //** This status is only for the reason when motor is on previouly on the also send the status
        last_run_action == NONE ? SendDataToParseController(HISTORY_LIVE_CLASS, UNDERVOLTON) : 1;
        last_run_action != NONE ? last_off_action = VOLTAGE : 1;
        /*-----------------*/
        /* Ensuring the Error Post Operation */

        SaveFaultPostAction(NULL);

        /*-------------------*/
    }
    break;
    case VOLTERROROFF:
    {
        ESP_LOGI(TAG, "VOLTAGE ERROR OFF");

        relay_status_to_send = false;
        FaultArrayOperation(VOLTAGE, READ); //** Not providing enough logic here to desert the error..saftey for actual

        /*< status to send>*/

        SendDataToParseController(HISTORY_LIVE_CLASS, VOLTERROROFF);

        /*-----------------*/
    }

    break;
    case RUNON:
    {
        ESP_LOGI(TAG, "RUN ON");
        if (!soft_error_in_system) //** This condition only save gerks for false alarm on app.
        {

            parse_event_send_t data_to_send = {.class_type = HISTORY_LIVE_CLASS,
                                               .system_status = WrapSystemStatus(NULL),
                                               .time = GetStandardTime(false),
                                               .run_time = 0,
                                               .tag = "",
                                               .phone_tag = ""};
            StartRunTime(NULL);
            /*< Deciding the reason of ON >*/

            if (last_run_action == APP)
            {
                ESP_LOGI(TAG, "TRUN ON BY APP");
                data_to_send.history_button_status = APPON;
                /* Fault automode action should clear here*/
                device_config.automode_fault_count = 0;
            }
            else if (last_run_action == SCH)
            {
                ESP_LOGI(TAG, "TRUN ON BY SCH");
                data_to_send.history_button_status = SCHEDULEON;
                /*< copying the schdule tag / phone >*/
                sprintf(data_to_send.tag, "%s", current_schedule_record.schedule_tag);
                sprintf(data_to_send.phone_tag, "%s", current_schedule_record.schedule_phone);
            }
            else if (last_run_action == AUTOMODE)
            {
                ESP_LOGI(TAG, "TRUN ON BY AUTOMODE");
                data_to_send.history_button_status = AUTOMODEON;
            }
            else
            {

                ESP_LOGI(TAG, "TRUN ON MANUUELY ");
                last_run_action = MANNUEL;
                data_to_send.history_button_status = MANNUELON;
                //  device_config.automode_fault_count = 0;
            }

            last_off_action = NONE; // no need of last off action here

            xQueueSendToBack(xParseQueue,
                             (void *)&data_to_send,
                             (TickType_t)10);
        }
        else if (!fault_array_index) //** this condition only when if any way motor on in disable safety mode to stop
        {
            relay_status_to_send = false;
            xQueueSendToBack(xRelayQueue, (void *)&relay_status_to_send, (TickType_t)100);
        }
        else
        {
            // CXX COMPLIENCE
        }
        // ESP_LOGI(TAG, " LEAVING RUN ON");
    }
    break;
    case RUNOFF:
    {
        ESP_LOGI(TAG, "RUN OFF :%i\n", last_off_action);

        // uint64_t time = 0U;

        parse_event_send_t data_to_send = {.class_type = HISTORY_LIVE_CLASS,
                                           .system_status = WrapSystemStatus(NULL),
                                           .time = GetStandardTime(false),
                                           .run_time = StopRunTime(NULL),
                                           .tag = "",
                                           .phone_tag = ""};
        /*< Deciding the reason of OFF >*/

        if (last_off_action == APP)
        {

            ESP_LOGI(TAG, "TRUN OFF BY APP");

            if (last_run_action == SCH)
            {
                data_to_send.history_button_status = SCHEDULEAPPOFF;
                AttachScheduleTag(&data_to_send);
            }
            else
            {
                data_to_send.history_button_status = APPOFF;
            }

            /*<if schedule force fully stop by app >*/
            if (last_run_action == SCH)
            {
                schedule_event_t schedule_event = DELETE_CURRENT_SCHEDULE;
                xQueueReset(xScheduleInQueue);
                xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);
                schedule_cross_run = 0;
            }
        }
        else if (last_off_action == SCH)
        {
            ESP_LOGI(TAG, "TRUN OFF BY SCH");
            data_to_send.history_button_status = SCHEDULEOFF;

            AttachScheduleTag(&data_to_send);
        }
        else if (last_off_action == MNT)
        {
            ESP_LOGI(TAG, "TRUN OFF BY MNT");
            if (last_run_action == SCH)
            {
                data_to_send.history_button_status = SCHEDULEMNTOFF;
                AttachScheduleTag(&data_to_send);
            }
            else
            {
                data_to_send.history_button_status = UNDERMNTON;
            }
        }
        else if (last_off_action == TRIP)
        {
            ESP_LOGI(TAG, "TRUN OFF BY TRIP");
            if (last_run_action == SCH)
            {
                data_to_send.history_button_status = SCHEDULETRIPOFF;
                AttachScheduleTag(&data_to_send);
            }
            else
            {
                data_to_send.history_button_status = TRIPON;
            }
        }
        else if (last_off_action == VOLTAGE)
        {
            ESP_LOGI(TAG, "TRUN OFF BY VOLATGE");

            if (last_run_action == SCH)
            {
                if (hlw_voltage.error_type == UNDERVOLTON)
                    data_to_send.history_button_status = SCHEDULEUNDEROFF;
                else
                    data_to_send.history_button_status = SCHEDULEOVEROFF;

                AttachScheduleTag(&data_to_send);
            }
            else
            {
                data_to_send.history_button_status = hlw_voltage.error_type;
            }
        }
        else if (last_off_action == PHASE)
        {
            ESP_LOGI(TAG, "TRUN OFF BY PHASE");
            if (last_run_action == SCH)
            {
                data_to_send.history_button_status = SCHEDULEPHASEOFF;
                AttachScheduleTag(&data_to_send);
            }
            else
            {
                data_to_send.history_button_status = PHASEERRORON;
            }
        }
        else
        {
            ESP_LOGI(TAG, "TRUN OFF BY MANNUELLY");
            if (last_run_action == SCH)
            {
                data_to_send.history_button_status = SCHEDULEMNLOFF;
                AttachScheduleTag(&data_to_send);
            }
            else
            {
                if (!soft_error_in_system)
                    data_to_send.history_button_status = MANNUELOFF;
                //** we can provide the  malfunction function to user here
            }
            //** Extra check for the deleting the schedule if mannuely turned of
            if (last_run_action == SCH)
            {
                schedule_event_t schedule_event = DELETE_CURRENT_SCHEDULE;
                xQueueReset(xScheduleInQueue);
                xQueueSendToBack(xScheduleInQueue, &(schedule_event), (TickType_t)100);
                schedule_cross_run = 0;
            }
        }
        // if (!soft_error_in_system) //** This condition only save gerks for false alarm on app.
        // {
        xQueueSendToBack(xParseQueue,
                         (void *)&data_to_send,
                         (TickType_t)100);
        // }
        last_off_action = NONE;
        last_run_action = NONE;
    }
    break;
    case APPON:
    {
        // if (!live_class.motor_status)
        // {
        ESP_LOGI(TAG, "ON BY APP");

        if (!fault_array_index && !soft_error_in_system)
        {
            bool status_to_send = true;
            xQueueSendToBack(xRelayQueue, (void *)&status_to_send, (TickType_t)100);

            /*< status to send>*/

            /*-----------------*/
            last_run_action = APP;
            // }
        }
    }
    break;
    case APPOFF:
    {
        // if (live_class.motor_status)
        // {
        bool status_to_send = false;
        xQueueSendToBack(xRelayQueue, (void *)&status_to_send, (TickType_t)100);
        last_off_action = APP;

        /*< status to send>*/

        /*-----------------*/

        last_off_action = APP;
        // }
    }
    break;
    case SCHEDULEON:
    {
        // if (!live_class.motor_status)
        // {
        ESP_LOGI(TAG, "ON BY SCHEDULE");

        if (!fault_array_index && !soft_error_in_system)
        {
            bool status_to_send = true;
            xQueueSendToBack(xRelayQueue, (void *)&status_to_send, (TickType_t)100);

            /*< status to send>*/

            /*-----------------*/
            last_run_action = SCH;
        }
        // }
    }
    break;
    case SCHEDULEOFF:
    {
        // if (live_class.motor_status)
        // {
        bool status_to_send = false;
        xQueueSendToBack(xRelayQueue, (void *)&status_to_send, (TickType_t)100);
        last_off_action = SCH;
        // }
        /*< status to send>*/

        /*-----------------*/
    }
    break;

    case AUTOMODEON:
    {
        ESP_LOGI(TAG, "ON BY AUTOMODE");

        bool status_to_send = true;
        xQueueSendToBack(xRelayQueue, (void *)&status_to_send, (TickType_t)100);

        /*< status to send>*/

        /*-----------------*/
        last_run_action = AUTOMODE;
    }
    break;
    // case AUTOMODEFF:
    // {
    //     // status_to_send = true;
    //     // xQueueSendToBack(xRelayQueue, (void *)&status_to_send, (TickType_t)100);

    //     // /*< status to send>*/

    //     // /*-----------------*/
    //     // last_run_action = AUTOMODE;
    // }
    // break;
    default:
        break;
    }
}
/**
 * @brief
 *
 * @param ptr
 */
static void ReadHardwareStatus(void *param)
{
    // printf("LEVEL OF RUN PIN : %i\n", gpio_get_level(RUN_PIN));
    // printf("LEVEL OF TRIP PIN : %i\n", gpio_get_level(TRIP_PIN));

    live_class.motor_status = !gpio_get_level(RUN_PIN);
    live_class.trip_status = !gpio_get_level(TRIP_PIN);
    // live_class.undermnt = xEventGroupGetBits(xEventMnt) & MNT;
    live_class.elcectricity = Electricity_ST != 0 ? 1 : 0;
    device_config.phase_fault ? live_class.phase_error = (xEventGroupGetBits(xPhaseEvent) & PHASE_FAULT) : 0;
    !device_config.phase_fault ? live_class.phase_error = 0 : 1;
    live_class.voltage_error = (hlw_voltage.error_type != NONE) ? true : false;

    soft_error_in_system = (live_class.trip_status ||
                            live_class.undermnt ||
                            live_class.phase_error ||
                            live_class.voltage_error)
                               ? true
                               : false;
    if (soft_error_in_system)
        ESP_LOGE(TAG, "--- SOFT ERROR IN SYSTEM ---");
}
/**
 * @brief
 *
 * @param param
 */
void HardwareCoreTask(void *param)
{
    /* Initilization of pre-thing */
    static bool beep_strobe = false;
    static bool startup_stroke = true;

    while (1)
    {

        while (1)
        {
            if (!network_config.verified_user)
                break;
            /* Do All the hardware up for verified User */

            if (startup_stroke)
            {
                PerformAutoModeInit(NULL); /* Startup stroke for Automode // May take 10s for network reasons */
                startup_stroke = false;
            }

            ReadHardwareStatus(NULL); /* Normal Status update on which this loop perform it operation */

            /**
             * Operation Performance Order.
             * 1.Electricty anaysis should be first on system startup.
             * 2.Motor fault in the second prio.
             * 3.instruct operation in the third place.
             *  !!! if these instructions order is going to change then there is no surety how history is going to manupulate.
             */

            if (live_class.elcectricity != prev_live_class.elcectricity)
            {
                ESP_LOGE(TAG, "----- ELECTRIC PREV CHANGE ----");
                live_class.elcectricity ? ParseSendLiveClass(ELECTRICITYON) : ParseSendLiveClass(ELECTRICITYOFF);
                prev_live_class.elcectricity = live_class.elcectricity;
            }

            if (live_class.undermnt != prev_live_class.undermnt)
            {
                ESP_LOGE(TAG, "----- MNT PREV CHANGE ----");
                live_class.undermnt ? ParseSendLiveClass(UNDERMNTON) : ParseSendLiveClass(UNDERMNTOFF);
                prev_live_class.undermnt = live_class.undermnt;
            }

            if (live_class.phase_error != prev_live_class.phase_error)
            {
                ESP_LOGE(TAG, "----- PHASE PREV CHANGE ----");
                live_class.phase_error ? ParseSendLiveClass(PHASEERRORON) : ParseSendLiveClass(PHASEERROROFF);
                prev_live_class.phase_error = live_class.phase_error;
            }

            if (live_class.trip_status != prev_live_class.trip_status)
            {
                ESP_LOGE(TAG, "----- TRIP PREV CHANGE ----");
                live_class.trip_status ? ParseSendLiveClass(TRIPON) : ParseSendLiveClass(TRIPOFF);
                prev_live_class.trip_status = live_class.trip_status;
            }

            if (live_class.voltage_error != prev_live_class.voltage_error)
            {
                ESP_LOGE(TAG, "----- VOLTAGE PREV CHANGE ----");
                live_class.voltage_error ? ParseSendLiveClass(hlw_voltage.error_type) : ParseSendLiveClass(VOLTERROROFF);
                prev_live_class.voltage_error = live_class.voltage_error;
            }

            if (live_class.motor_status != prev_live_class.motor_status)
            {
                ESP_LOGE(TAG, "----- MOTOR STATUS PREV CHANGE ----");

                /** very critiacal delay here it decide wheter electricity is gone or motor off action*/
                vTaskDelay(500 / portTICK_PERIOD_MS);

                /* some time trip system autooff the motor hndleed here*/
                if (!gpio_get_level(TRIP_PIN) != prev_live_class.trip_status)
                {
                    goto END;
                }

                live_class.motor_status ? ParseSendLiveClass(RUNON) : 1;

                if (!live_class.motor_status)
                {
                    (Electricity_ST != 0) ? ParseSendLiveClass(RUNOFF)
                                          : ParseSendLiveClass(ELECTRICITYOFF);
                }
                prev_live_class.motor_status = live_class.motor_status;
                ESP_LOGE(TAG, "----- MOTOR STATUS PREV CHANGE LEAVING ----");
            }

            //* !!! Critical section with respect to app / schedule / automode functioning */

            if (!soft_error_in_system)
            {
                bool auto_mode_event = false;
                uint8_t button_event = 0;
                uint8_t schedule_event = 0;

                if (xAutoModeQueue != NULL)
                {
                    if (xQueueReceive(xAutoModeQueue, &(auto_mode_event), (TickType_t)10) == pdPASS)
                    {
                        // last_run_action == AUTOMODE ?  last_run_action=NONE: 1; //** if not here it will disturb mannuel event

                        //**trash  the event if motor already running */
                        if (!live_class.motor_status && device_config.auto_mode && last_run_action != AUTOMODE)
                        {
                            // if previous run by automode no need to run it again by auto mode
                            (auto_mode_event) ? ParseSendLiveClass(AUTOMODEON) : 1;
                        }
                    }
                }

                if (xButtonQueue != NULL)
                {
                    if (xQueueReceive(xButtonQueue, &(button_event), (TickType_t)10) == pdPASS)
                    {
                        ESP_LOGI(TAG, "APP INSTRUCTIOn RECEIVED");
                        if (button_event == 1)
                        {
                            ParseSendLiveClass(APPON);
                        }
                        else if (button_event == 2)
                        {
                            ParseSendLiveClass(APPOFF);
                        }
                        else
                        {
                            // This condition is only when ) is received
                        }
                    }
                }

                if (xScheduleOutQueue != NULL)
                {
                    if (xQueueReceive(xScheduleOutQueue, &(schedule_event), (TickType_t)10) == pdPASS)
                    {
                        ESP_LOGI(TAG, "SCHEDULE INSTRUCTION RECEIVED");

                        if (schedule_event == 1)
                        {
                            schedule_run = true;
                            if (!schedule_cross_run)
                            {
                                ParseSendLiveClass(SCHEDULEON);
                                schedule_cross_run = 1;
                            }
                        }
                        else if (schedule_event == 2)
                        {
                            if (schedule_cross_run)
                            {
                                schedule_run = false;
                                ParseSendLiveClass(SCHEDULEOFF);
                                schedule_cross_run = 0;
                            }
                            else
                            {
                                /* to overcome the isssue now */
                                ClearAllSchedule();
                            }
                        }
                        else
                        {
                            // CXX Malfunction event
                        }
                    }
                }
            }
            /**---------------------------------------------------------------*/

            /**Beep Section will call user on the Fault Event only*/

            if (soft_error_in_system && beep_strobe)
            {
                buzzer_beep_t buzzer_event = FAULT_DETECTED;
                xQueueSendToBack(xBuzzerQueue, &(buzzer_event), (TickType_t)10);
                beep_strobe = false;
            }
            else if (!soft_error_in_system && !beep_strobe)
            {
                beep_strobe = true;
                buzzer_beep_t buzzer_event = FAULT_REMOVED;
                xQueueSendToBack(xBuzzerQueue, &(buzzer_event), (TickType_t)10);
            }

        END:
            /**---------------------------------------------------------*/
            // printf("TIME : %llu\n",GetStandardTime(false));
            //    ESP_LOGI(TAG, "--- ITERATING HW TASK ----");
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        ESP_LOGI(TAG, "--- PLEASE GO FOR VERIFIED USER ---");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void HardwareCoreInit(void *param)
{

    xTaskCreatePinnedToCore(HardwareCoreTask, "hardware_core_task", HARDWARECORE_TASK_STACK_SIZE, NULL, HARDWARECORE_TASK_PRIO, &xHardwareCoreTask, HARDWARECORE_TASK_CORE);
    xTaskCreatePinnedToCore(OperateRealy, "realy_control_task", RELAY_TASK_STACK_SIZE, NULL, RELAY_TASK_PRIO, &xRelayControlTask, RELAY_TASK_CORE);
}