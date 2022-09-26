// /**
//  * @file led_strip.cpp
//  * @author Hamza Islam (hamzaislam@170.com)
//  * @brief File handling the Led STRIP
//  * @version 0.1
//  * @date 2022-08-30
//  *
//  * @copyright Copyright (c) 2022
//  * @Supporter Scott Jensen
//  */

#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>

#include <Adafruit_NeoPixel.h>

#include "main.h"
#include "led_strip.h"
#include "touch_pad_custom.h"

#define PIN 17
#define NUMPIXELS 8

TaskHandle_t xTaskLedStrip = NULL;
QueueHandle_t xQueueLedStrip = NULL;

ButtonPress_t rec_button_led = kTouchIdle;

struct strip_param_t rx_led_strip = {0, 0, 0};

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

/**
 * @brief This Fuction is ued to Set the Strip color according to Touch button
 * operation which is received by the the Audio play Task.
 *
 */
void SetStripColor(ButtonPress_t led_strip_request)
{
    switch (led_strip_request)
    {
    case 0:
        rx_led_strip = {255, 255, 135};
        break;
    case 1:
        rx_led_strip = {255, 0, 135};
        break;
    case 2:
        rx_led_strip = {0, 0, 255};
        break;
    case 3:
        rx_led_strip = {0, 235, 0};
        break;
    case 4:
        rx_led_strip = {134, 0, 67};
        break;
    case 5:
        rx_led_strip = {125, 10, 179};
        break;
    case 6:
        rx_led_strip = {30, 255, 0};
        break;
    case 7:
        rx_led_strip = {0, 255, 0};
        break;
    case 9:
        rx_led_strip = {120, 0, 10};
        break;
    case 10:
        rx_led_strip = {0, 0, 0};
        break;
    default:
        rx_led_strip = {0, 0, 0};
    }
}

/**
 * @brief This is the Led Strip task which takes the input form the Queue
 * and This take the intruction from the Audio queue to execute the patrticular light format.
 *
 * @param param
 */
void LedStripTask(void *param)
{

    while (1)
    {
        if (xQueueReceive(xQueueLedStrip, &(rec_button_led), (1000 / portTICK_PERIOD_MS)) == pdPASS)
        {
            /*Message For Strip is available*/
            SetStripColor(rec_button_led);
            strip.fill(strip.Color(rx_led_strip.p1, rx_led_strip.p2, rx_led_strip.p3));
            strip.show();
        }
        // vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

/**
 * @brief This is the Fucntion is used to Led Strip Initilization
 * with Queue.
 *
 */
void StripInit(void *param)
{
    /* QUEUE INITILAZATION*/
    xQueueLedStrip = xQueueCreate(4, sizeof(ButtonPress_t));
    if (xQueueLedStrip == NULL)
    {
        printf("********LED STRIP QUEUE IS NOT CREATED SUCESSFULLY*******\n");
    }
    else
    {
        printf("********LED STRIP QUEUE CREATED SUCESSFULLY*******\n");

        strip.begin();
        strip.fill(strip.Color(0, 0, 0)); // Nothing
        strip.show();

        xTaskCreatePinnedToCore(LedStripTask, "LED_STRIP", 1024 * 4, NULL, 3, &xTaskLedStrip, 1);
    }
}
