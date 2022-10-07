

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "touch_pad_custom.h"
#include "sleep_manager.h"
#include "touch_pad_custom.h"
#include "esp_sleep.h"
#include "driver/touch_pad.h"
#include "Arduino.h"

#define ACTIVE_TIME (1000) * 30
#define SLEEP_TIME (1000 * 1000) * 30

TaskHandle_t xTaskSleepManager = NULL;

uint32_t prev_tick_count = 0;
uint32_t temp_ticks = 0;
/**
 * @brief dummy call back
 * 
 */
void dummy()
{
    /*CXX Complience*/
}

/**
 * @brief 
 * 
 * @param param 
 */

void SleepTask(void *param)
{
    prev_tick_count = xTaskGetTickCount();

    while (1)
    {
        temp_ticks = (xTaskGetTickCount() - prev_tick_count);
        if (temp_ticks > ACTIVE_TIME)
        {
            printf("------- GOING FOR SLEEP BYE BYE--------\n");
            vTaskDelay(10);
            esp_sleep_enable_timer_wakeup(SLEEP_TIME);
            
            touchAttachInterrupt(T4, dummy, (20));
            touchAttachInterrupt(T9, dummy, (20));

            esp_deep_sleep_start();
        }
        else
        {
            printf("System going to sleep after : %d SEC\n", temp_ticks / 1000);
            /* Completeing the CXX compilence*/
        }

        vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

void SleepInit(void *param)
{
    xTaskCreatePinnedToCore(&SleepTask, "I2S_Task", 1024 * 2, NULL, 3, &xTaskSleepManager, 1);
}
