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
#include "custom_ble.h"

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

TaskHandle_t xTaskTouchPoll = NULL;
QueueHandle_t xQueueBleOperation = NULL;

touch_pad_t touchPin;

ButtonPress_t rec_button = kTouchIdle;
ButtonPress_t prev_button = kTouchIdle;
ButtonPress_t sleep_wake_press = kTouchIdle;

RTC_DATA_ATTR uint8_t threshold1 = 20;

bool touch_event_happened = false;

uint8_t touch_count = TOUCH_TIME_TO_ENABLE_BLE / 1000;
uint8_t volumn_count = TOUCH_TIME_TO_ENABLE_VOLUMN / 1000;

void GetVolumn(bool read);
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
    wifi_audio = false;
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
 * @brief For volumn Direction control
 *
 */
void DecideVolumnStrobe()
{
  if (volumn_stroble)
  {
    volumn += 4;
    if (volumn > 19)
    {
      volumn = 21;
      volumn_stroble = false;
    }
  }
  else
  {
    volumn -= 4;
    if (volumn < 4)
    {
      volumn = 5;
      volumn_stroble = true;
    }
  }
  ButtonPress_t rec_button = kvolumn;
  xQueueSendToBack(xQueueAudioPlay, &(rec_button), 0);
  GetVolumn(false); // write the new volumn
}

void SendNotification()
{
  /*Send notifaction to respective queue for mannuel press*/
  if (touch_count == 0)
  {
    bool dummy = true;
    xQueueSendToBack(xQueueBleOperation, &(dummy), 0);
    touch_count = TOUCH_TIME_TO_ENABLE_BLE / 1000;
  }
  if (volumn_count == 0)
  {
    DecideVolumnStrobe();
    volumn_count = TOUCH_TIME_TO_ENABLE_VOLUMN / 1000;
  }
}

void GetVolumn(bool read)
{
  printf("----- GOING TO READ VOLUMN FROM FLASH -----\n");
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }

  nvs_handle my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);

  uint8_t dummy;

  if (read)
  {
    err = nvs_get_u8(my_handle, "volumn", &dummy);
    switch (err)
    {
    case ESP_OK:
      volumn = dummy;
      break;
    case ESP_ERR_NVS_NOT_FOUND:
      nvs_set_u8(my_handle, "volumn", volumn);
      break;
    default:
      break;
    }
  }

  // Write
  else
  {
    nvs_set_u8(my_handle, "volumn", volumn);
  }

  err = nvs_commit(my_handle);
  nvs_close(my_handle);
}

void TouchPoll(void *param)
{
  GetVolumn(true);
  bool state = false;
  while (1)
  {
    if (touchRead(T4) < threshold1)
    {
      touch_count--;
      state = true;
    }

    /* for sound volumn*/
    if ((touchRead(T9) < threshold1))
    {
      volumn_count--;
      state = true;
    }

    if (state)
    {
      SendNotification();
      state = false;
    }

    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }
}

/**
 * @brief Thsis function is used to Initilization of the Interupts on the TouchPad
 * @param None
 */

void InitTouchPad(void *param)
{
  printf("------- TOUCH PAD INITLIZATION ---------\n");
  touchAttachInterrupt(T0, GotTouch0, threshold1);
  touchAttachInterrupt(T1, GotTouch1, threshold1);
  touchAttachInterrupt(T2, GotTouch2, threshold1);
  touchAttachInterrupt(T3, GotTouch3, (threshold1));
  //  touchAttachInterrupt(T4, GotTouch4, (threshold1));
  touchAttachInterrupt(T5, GotTouch5, threshold1);
  touchAttachInterrupt(T6, GotTouch6, (threshold1));
  touchAttachInterrupt(T7, GotTouch7, threshold1);
  touchAttachInterrupt(T8, GotTouch8, threshold1);

  xQueueBleOperation = xQueueCreate(2, sizeof(bool));

  if (xQueueBleOperation == NULL)
  {
    printf("******** BLE NOTIFIY  QUEUE IS NOT CREATED SUCESSFULLY *******\n");
  }
  else
  {
    printf("******** BLE NOTIFIY QUEUE CREATED SUCESSFULLY *******\n");

    xTaskCreatePinnedToCore(&TouchPoll, "Touch", 1024 * 3, NULL, 3, &xTaskTouchPoll, 1);
  }

  //   touchAttachInterrupt(T9, GotTouch9, threshold1);
}
