// /**
//  * @file phase_sensing.c
//  * @author your name (you@domain.com)
//  * @brief
//  * @version 0.1
//  * @date 2022-11-03
//  *
//  * @copyright Copyright (c) 2022
//  *
//  */

#include <stdio.h>
#include "stdlib.h"

#include "esp_sleep.h"
#include "driver/timer.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "freertos_handler.h"
#include "parse.h"
#include "system_gpio.h"
#include "phase_sensing.h"
#include "hw_monitor.h"


EventGroupHandle_t xPhaseEvent = NULL;
TaskHandle_t xPhaseTask = NULL; // Task & Event handler

uint64_t phase1_timer = 0;
uint64_t phase2_timer = 0;
uint64_t phase3_timer = 0;

float time_slice = .00125;

float Electricity_ST = 0;


float f_phase1_timer = 0;
float f_phase2_timer = 0;
float f_phase3_timer = 0;

static char *TAG = "PHASE DETECTION";

uint8_t tries_count_fault = 0;
uint8_t tries_count_fault_clear = 0;
uint8_t P1_tries_electricity = 0;
uint8_t P1_tries=0;
uint8_t P2_tries = 0;
uint8_t P3_tries = 0;
 
bool flag = false;
/**
 * @brief
 *
 * @param arg
 */
void IRAM_ATTR PhaseIsrHandler1(void *arg)
{
    if (!flag)
    {

        timer_start(TIMER_GROUP_0, TIMER_0);
        gpio_intr_enable(PHASE2_OPAMP_PIN);
        gpio_intr_enable(PHASE3_OPAMP_PIN);
        flag = 1;
    }
    else
    {
        timer_pause(TIMER_GROUP_0, TIMER_0);
        gpio_intr_disable(PHASE1_OPAMP_PIN);
        phase1_timer = 0;
        timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &phase1_timer);
        flag = 0;
    }
}

void IRAM_ATTR PhaseIsrHandler2(void *arg)
{
    phase2_timer = 0;
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &phase2_timer);
    gpio_intr_disable(PHASE2_OPAMP_PIN);
}

void IRAM_ATTR PhaseIsrHandler3(void *arg)
{
    phase3_timer = 0;
    timer_get_counter_value(TIMER_GROUP_0, TIMER_0, &phase3_timer);
    gpio_intr_disable(PHASE3_OPAMP_PIN);
}

/**
 * @brief
 *
 * @param param
 */
void IsrInit(void *param)
{

    gpio_config_t io_conf = {0};
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = INTR_PIN_MASK;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_down_en = 1;
    gpio_config(&io_conf);

    gpio_intr_disable(PHASE1_OPAMP_PIN);
    gpio_intr_disable(PHASE2_OPAMP_PIN);
    gpio_intr_disable(PHASE3_OPAMP_PIN);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(PHASE1_OPAMP_PIN, PhaseIsrHandler1, (void *)PHASE1_OPAMP_PIN);
    gpio_isr_handler_add(PHASE2_OPAMP_PIN, PhaseIsrHandler2, (void *)PHASE2_OPAMP_PIN);
    gpio_isr_handler_add(PHASE3_OPAMP_PIN, PhaseIsrHandler3, (void *)PHASE3_OPAMP_PIN);

    /* Initilization of the Deep Sleep*/

    esp_sleep_enable_ext0_wakeup(DEEP_SLEEP_PIN, 0);
}
/**
 * @brief
 *
 * @param group
 * @param timer
 * @param auto_reload
 * @param timer_interval_sec
 */
static void PhaseTimerInit(int group, int timer, bool auto_reload, int timer_interval_sec)
{

    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = 0,
        .auto_reload = auto_reload,
    };

    timer_init(group, timer, &config);
    timer_set_counter_value(group, timer, 0);
}
/**
 * @brief
 *
 */
void CalculatePhase()
{
    // ESP_LOGI(TAG, "FREQ 1 %.3f ", f_phase1_timer);
    // ESP_LOGI(TAG, "FREQ 2 %.3f ", f_phase3_timer);
    // ESP_LOGI(TAG, "FREQ 3 %.3f ", f_phase2_timer);

    bool mismatch_phase1 = 1;
    bool mismatch_phase2 = 0;
    bool mismatch_phase3 = 0;

    // Indication Variable

    float deg_1 = f_phase1_timer / 180;

    float cal_time_1 = deg_1 * DIFF_1_DEG;
    float cal_time_2 = deg_1 * DIFF_2_DEG;
    float tolrence = deg_1 * TOL_DEG;

    float tol_1_n = cal_time_1 - tolrence;
    float tol_1_p = cal_time_1 + tolrence;

    float tol_2_n = cal_time_2 - tolrence;
    float tol_2_p = cal_time_2 + tolrence;

    /*< Taking comparison form here>*/
    if ((f_phase2_timer < tol_1_p && f_phase2_timer > tol_1_n) || (f_phase2_timer < tol_2_p && f_phase2_timer > tol_2_n))
    {
        mismatch_phase2 = 1;
    }

    if ((f_phase3_timer < tol_1_p && f_phase3_timer > tol_1_n) || (f_phase3_timer < tol_2_p && f_phase3_timer > tol_2_n))
    {
        mismatch_phase3 = 1;
    }
    if ((8 >= (uint8_t)f_phase2_timer && (uint8_t)f_phase2_timer >= 7 && 5 >= (uint8_t)f_phase3_timer && (uint8_t)f_phase3_timer >= 4) ||
        (5 >= (uint8_t)f_phase2_timer && (uint8_t)f_phase2_timer >= 4 && 8 >= (uint8_t)f_phase3_timer && (uint8_t)f_phase3_timer >= 7))
    {
        mismatch_phase1 = 0;
        mismatch_phase2=1;
        mismatch_phase3=1;
    }

    if ((uint8_t)f_phase2_timer == (uint8_t)(f_phase3_timer))
    {
        mismatch_phase2 = 0;
        mismatch_phase3 = 0;
    }

    /*< For Indication  >*/
// Electricity status
    if (f_phase1_timer == 0)
    {
        if (P1_tries_electricity >= 2)
        {
            ESP_LOGE(TAG, "--- ELECTRICITY GONE ---");
            ESP_LOGE(TAG, "--- PHASE 1 DISAPPEAR ---");
            xEventGroupClearBits(xPhaseEvent, PHASE1);
            xEventGroupClearBits(xPhaseEvent, ELECTRICITY_ST);
            P1_tries_electricity = 0;
        }

        P1_tries_electricity++;
    }
    else
    {
        xEventGroupSetBits(xPhaseEvent, ELECTRICITY_ST);
        xEventGroupSetBits(xPhaseEvent, PHASE1);
        P1_tries_electricity = 0;
    }
// Phase 1 deassert
     if (!mismatch_phase1)
    {
        if (P1_tries >= 30)
        {
            ESP_LOGE(TAG, "--- PHASE 1 DISAPPEAR ---");
            xEventGroupClearBits(xPhaseEvent, PHASE1);
            P1_tries = 0;
        }
        P1_tries++;
    }
    else
    {
        xEventGroupSetBits(xPhaseEvent, PHASE1);
        P1_tries = 0;
    }
 // Phase 2 deassert
    if (!mismatch_phase2)
    {
        if (P2_tries >= 30)
        {
            ESP_LOGE(TAG, "--- PHASE 2 DISAPPEAR ---");
            xEventGroupClearBits(xPhaseEvent, PHASE2);
            P2_tries = 0;
        }
        P2_tries++;
    }
    else
    {
        xEventGroupSetBits(xPhaseEvent, PHASE2);
        P2_tries = 0;
    }
// Phase 3 deassert
    if (!mismatch_phase3)
    {
        if (P3_tries >= 30)
        {
            ESP_LOGE(TAG, "--- PHASE 3 DISAPPEAR ---");
            xEventGroupClearBits(xPhaseEvent, PHASE3);
            P3_tries = 0;
        }
        P3_tries++;
    }
    else
    {
        xEventGroupSetBits(xPhaseEvent, PHASE3);
        P3_tries = 0;
    }

    /*---------------------------------------------------*/

    /*< For Phase Fault >*/

if(xEventGroupGetBits(xPhaseEvent) & PHASE_FAULT_ENABLE)            //check if user want to enable the phase fault
{
    if (mismatch_phase3 && mismatch_phase2 && mismatch_phase1 )
    {
        tries_count_fault_clear++;
        tries_count_fault = 0;
        if (tries_count_fault_clear >= 30)
        {
          //  ESP_LOGI(TAG, "--- PHASE ANGLE IS CORRECT ---");
            tries_count_fault_clear = 0;
            xEventGroupClearBits(xPhaseEvent, PHASE_FAULT);
        }
    }
    else
    {
        tries_count_fault_clear = 0;
        tries_count_fault++;
        if (tries_count_fault >= 30)
        {
            tries_count_fault = 0;
            ESP_LOGE(TAG, "--- PHASE ANGLE IS NOT CORRECT ---");
            xEventGroupSetBits(xPhaseEvent, PHASE_FAULT);
        }
    }
}
else
{
            xEventGroupClearBits(xPhaseEvent, PHASE_FAULT);
}
}
/**
 * @brief
 *
 * @param arg
 */
static void PhaseTask(void *arg)
{
    /*< Initilization of ISR >*/

    IsrInit((void *)NULL);
    ESP_LOGI(TAG, "PHASE TASK GOT INITILIZE");

    while (1)
    {
        device_config.phase_fault ? xEventGroupSetBits(xPhaseEvent, PHASE_FAULT_ENABLE) : xEventGroupClearBits(xPhaseEvent, PHASE_FAULT_ENABLE);
        vTaskDelay(100 / portTICK_PERIOD_MS);

        timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);

        f_phase1_timer = phase1_timer * time_slice;
        f_phase2_timer = phase2_timer * time_slice;
        f_phase3_timer = phase3_timer * time_slice;

       Electricity_ST=f_phase1_timer;

        CalculatePhase();

        f_phase1_timer = 0;
        f_phase2_timer = 0;
        f_phase3_timer = 0;

        phase1_timer = 0;
        phase2_timer = 0;
        phase3_timer = 0;

        gpio_intr_enable(PHASE1_OPAMP_PIN);
    }
}
/**
 * @brief
 *
 */
void PhaseSensingInit(void *param)
{
    PhaseTimerInit(TIMER_GROUP_0, TIMER_0, true, 3);
    xTaskCreatePinnedToCore(PhaseTask, "PhaseTask", PHASE_TASK_STACK_SIZE, NULL, PHASE_TASK_PRIO, &xPhaseTask,HARDWARECORE_TASK_CORE);
   

}
/**
 * @brief
 *
 */
void CheckPowerStatus(void *param)
{
        vTaskDelay(500/ portTICK_PERIOD_MS);

    if (!(*((uint8_t *)(xPhaseEvent)) & ELECTRICITY_ST))
    {
        ESP_LOGE(TAG, "--- PLEASE ENSURE POWER ----");
        vTaskDelay(10/ portTICK_PERIOD_MS);

        esp_deep_sleep_start();
    }
}