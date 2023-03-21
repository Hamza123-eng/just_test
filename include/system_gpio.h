#ifndef __SYSTEM_GPIO_H
#define __SYSTEM_GPIO_H

#include "stdint.h"
/*< Designed PIN Structure >*/

// HLW

#define HLW1_RX_PIN 16
#define HLW2_RX_PIN 15
#define HLW3_RX_PIN 3

// Relay

#define ON_RELAY_PIN 4
#define OFF_RELAY_PIN 14
#define VFD_RELAY_PIN 15

// Phase

#define PHASE1_OPAMP_PIN 18
#define PHASE2_OPAMP_PIN 19
#define PHASE3_OPAMP_PIN 23

// I2C LINE

#define I2C_SDA_PIN 21
#define I2C_SCL_PIN 22

// Output status

#define RUN_PIN 32
#define TRIP_PIN 33
#define WIFI_LED_PIN 13
#define BUZZER_PIN 27    // not there

//input button 

#define MULTI_BUTTON_PIN 25
#define DEEP_SLEEP_PIN 26
#define BATTERY_CUT_PIN 0 //

// Pcf signal

#define PCF_P1 0x01
#define PCF_P2 0x02
#define PCF_P3 0x04
#define PCF_RUN 0x08
#define PCF_TRIP 0x10
#define PCF_MNT 0x20
#define PCF_BUZZER 0x40
#define PCF_UNDER_VOLT 0x00
#define PCF_OVER_VOLT 0x00

/*<Internal Operantion for GPIO>*/

#define OUT 0
#define IN 1

#define PULLDOWN 0
#define PULLUP 1

#define ON 0
#define OFF 1

/*<define array size >*/

#define PIN_ARRAY_SIZE 7

typedef struct 
{
    uint8_t pin_num;
    bool mode;
    bool pull_state;
}e_gpio_config;

void GpioInit(void *param);


#endif //_ESP_GPIO_H