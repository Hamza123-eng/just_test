/**
 * @file touch_pad.cpp
 * @author Hamza Islam (hamzaislam@170.com)
 * @brief File handling the Through Working of the Touch Pad
 * @version 0.1
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022
 * @Supporter Scott Jensen
 */

#include "touch_pad_custom.h"
#include "esp_attr.h"
#include "Arduino.h"
#include "freertos/FreeRTOS.h"
#include "firebase_mqtt.h"
#include "main.h"
#include "sleep_manager.h"
#include "driver/touch_pad.h"
#include "i2s_play.h"
#include "esp_sleep.h"

touch_pad_t touchPin;

ButtonPress_t rec_button = kTouchIdle;
ButtonPress_t prev_button = kTouchIdle;
ButtonPress_t sleep_wake_press = kTouchIdle;

RTC_DATA_ATTR uint8_t threshold1 = 20;

bool touch_event_happened = false;

/**
 * @brief  function below are the Touch Pad Interupt handler
 * When interupt happened that interuupt is propagate to the Audio Queue
 * also update the time to go into the sleep
 * @param None
 */

void IRAM_ATTR GotTouch0()
{
  rec_button = kTouch0;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch1()
{

  rec_button = kTouch1;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch2()
{

  rec_button = kTouch2;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch3()
{
  rec_button = kTouch3;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch4()
{
  rec_button = kTouch4;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch5()
{
  rec_button = kTouch5;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch6()
{
  rec_button = kTouch6;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch7()
{
  rec_button = kTouch7;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch8()
{
  rec_button = kTouch8;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

void IRAM_ATTR GotTouch9()
{
  rec_button = kTouch9;
  /*saving the multiple interupts */
  if ((prev_button != rec_button) && xQueueAudioPlay != NULL)
  {
    prev_button = rec_button;
    xQueueSendToBackFromISR(xQueueAudioPlay, &rec_button, 0);
    touch_event_happened = true;
    prev_tick_count = xTaskGetTickCount();
  }
}

/**
 * @brief This function is used o determine which pad got press and
 * information of that pad is pass to the I2s Queue by the help of the Button_t handler
 * @param None
 */
void wakeup_touchpad(void *param)
{

  touchPin = esp_sleep_get_touchpad_wakeup_status();

  switch (touchPin)
  {
  case 0:
    Serial.println("Touch detected on GPIO 4");
    sleep_wake_press = kTouch0;
    break;
  case 1:
    Serial.println("Touch detected on GPIO 0");
    sleep_wake_press = kTouch1;
    break;
  case 2:
    Serial.println("Touch detected on GPIO 2");
    sleep_wake_press = kTouch2;
    break;
  case 3:
    Serial.println("Touch detected on GPIO 15");
    sleep_wake_press = kTouch3;
    break;
  case 4:
    Serial.println("Touch detected on GPIO 13");
    sleep_wake_press = kTouch4;
    break;
  case 5:
    Serial.println("Touch detected on GPIO 12");
    sleep_wake_press = kTouch5;
    break;
  case 6:
    Serial.println("Touch detected on GPIO 14");
    sleep_wake_press = kTouch6;
    break;
  case 7:
    Serial.println("Touch detected on GPIO 27");
    sleep_wake_press = kTouch7;
    break;
  case 8:
    Serial.println("Touch detected on GPIO 33");
    sleep_wake_press = kTouch8;
    break;
  case 9:
    Serial.println("Touch detected on GPIO 32");
    sleep_wake_press = kTouch9;
    break;
  default:
    sleep_wake_press = kTouchIdle;
    Serial.println("Wakeup not by touchpad");
    break;
  }
}
/**
 * @brief This function is use to Only take count of the WakeUp reason
 * if wake is happpened by Touch pad then the We are intrested in the Pin
 *which is the cause of wakeup
 * @ Param None
 */
void print_wakeup_reason(void *param)
{

 esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason)
  {
  case ESP_SLEEP_WAKEUP_EXT0:
    Serial.println("Wakeup caused by external signal using RTC_IO");
    break;
  case ESP_SLEEP_WAKEUP_EXT1:
    Serial.println("Wakeup caused by external signal using RTC_CNTL");
    break;
  case ESP_SLEEP_WAKEUP_TIMER:
    Serial.println("Wakeup caused by timer");
    break;
  case ESP_SLEEP_WAKEUP_TOUCHPAD:
    wakeup_touchpad((void *)NULL);
    Serial.println("********Wakeup caused by touchpad**********");
    break;
  case ESP_SLEEP_WAKEUP_ULP:
    Serial.println("Wakeup caused by ULP program");
    break;
  case ESP_SLEEP_WAKEUP_UNDEFINED:
    Serial.println("Wakeup caused is undefined");
    break;
  default:
    Serial.println("Wakeup was not caused by deep sleep");
    break;
  }
}

/**
 * @brief Thsis function is used to Initilization of the Interupts on the TouchPad
 * @param None
 */

void InitTouchPad(void *param)
{
  touchAttachInterrupt(T0, GotTouch0, threshold1);
    touchAttachInterrupt(T1, GotTouch1, threshold1);
    touchAttachInterrupt(T2, GotTouch2, threshold1);
    touchAttachInterrupt(T3, GotTouch3, (threshold1 ));
    touchAttachInterrupt(T4, GotTouch4, (threshold1 ));
    touchAttachInterrupt(T5, GotTouch5, threshold1);
    touchAttachInterrupt(T6, GotTouch6, (threshold1));
  touchAttachInterrupt(T7, GotTouch7, threshold1);
  touchAttachInterrupt(T8, GotTouch8, threshold1);
   touchAttachInterrupt(T9, GotTouch7, threshold1);
}
