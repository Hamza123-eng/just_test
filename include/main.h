#ifndef MAIN_H_
#define MAIN_H_

#ifdef DEBUG_MODE
#define logln(x) Serial.println(x)
#define log(x) Serial.print(x)
#define logf(x, y, z) Serial.printf(x, y, z)
#else
#define logln(x)
#define log(x)
#define logf(x, y, z)
#endif

extern esp_sleep_wakeup_cause_t wakeup_reason;

/*TASK HANDLER ARE HERE*/
extern TaskHandle_t xTaskLedStrip;
#endif