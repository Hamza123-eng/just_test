

#
#include "SPIFFS.h"

#include "custom_ble.h"
#include "wifi_manager.h"
#include "touch_pad_custom.h"
#include "sleep_manager.h"
#include "i2s_play.h"
#include "led_strip.h"
#include "firebase_mqtt.h"
#include "spiffs_system.h"
#include "battery_control.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void setup() ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  
  print_wakeup_reason((void *)NULL);

   Serial.begin(115200);

SpiffsInit((void *)NULL);
  if (!SPIFFS.begin()) // initializing SPIFFS
  {
    SPIFFS.begin(true);
  }

   InitTouchPad((void *)NULL);
   AudioInit((void *)NULL);
    StripInit((void *)NULL);
    SleepInit((void *)NULL);
  // BatteryMonitorInit((void *)NULL);
   FireBaseInit((void *)NULL);
  BleInit((void *)NULL);

}

void loop()
{

}
