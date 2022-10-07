
/**
 * @file battery_control.cpp
 * @author Hamza Islam (hamzaislam@170.com)
 * @brief File handling SOC for the Battery
 * @version 0.1
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022
 * @Supporter Scott Jensen
 */

#include "battery_control.h"
#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include <freertos/task.h>
#include "Arduino.h"

const int MAX_ANALOG_VAL = 4095;
const float MAX_BATTERY_VOLTAGE = 4.2;

#define BATTERY_PIN 34

#define red_light_pin 23
#define green_light_pin 22
#define blue_light_pin 21

void RGB_color(int red_value, int green_value, int blue_value)
{
    digitalWrite(red_light_pin, !red_value);
    digitalWrite(green_light_pin, !green_value);
    digitalWrite(blue_light_pin, !blue_value);
}

/**
 * @brief Battery SOC Measuremnt
 * This function Tell How Much Charge is prenst in the % and Display the Output to the
 * LED
 *
 */

void BatterySoc(void *param)
{
    float voltageLevel = 0;
    float batteryFraction = 0;
    int rawValue = 0;
    while (1)
    {
        /* code */
        rawValue = analogRead(BATTERY_PIN);
        // voltageLevel = (rawValue / 4095.0) * 2 * 1.1 * 3.3;
        // batteryFraction = voltageLevel / MAX_BATTERY_VOLTAGE;
        // batteryFraction *= 100;

        printf("RAW %d \n",rawValue);
       
        //     if (batteryFraction > 80)
        //         RGB_color(0, 1, 0);       ////Green

        //     else if (batteryFraction > 50)
        //         RGB_color(0, 0, 1);       // blue

        //     else if (batteryFraction > 20)
        //         RGB_color(1, 1, 0);     // yellow

        //     else if (batteryFraction > 5)
        //         RGB_color(1, 0, 0);      // red

        //     else
        //         RGB_color(1, 1, 1);      // red
        
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void BatteryMonitorInit(void *param)
{

    /* QUEUE INITILAZATION*/
    pinMode(BATTERY_PIN, INPUT);
    // pinMode(red_light_pin, OUTPUT);
    // pinMode(green_light_pin, OUTPUT);
    // pinMode(blue_light_pin, OUTPUT);

  // xTaskCreatePinnedToCore(&BatterySoc, "BATTERY_Task", 1024 * 3, NULL, 2, NULL, 1);
}
