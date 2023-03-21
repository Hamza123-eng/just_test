#ifndef _MAIN_H
#define _MAIN_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
// #define LED_PIN                 13 // LED Pin for wifi but still confusion!


extern TaskHandle_t PingNotificationTaskHandler;

void websocket_app_stop(void);
void websocket_sch_stop(void);
#endif //  _MAIN_H