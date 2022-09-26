

#
#include "SPIFFS.h"
#include "main.h"
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
#include "esp32-mqtt.h"

void handleDeviceIOTInfo() {
  logln("handle identifiers");
  if ((device_id == UNSET) || (private_key_str == UNSET)) {
    logln("identifiers are unset");
    if (SPIFFS.exists("/deviceIdentifiers.json")) {
      File read_file = SPIFFS.open("/deviceIdentifiers.json", "r");
      if (!read_file) {
        logln("There was an error opening the Device Identifiers File");
        return;
      }
      else {
        DynamicJsonDocument deviceIdentifiers(1024);
        DeserializationError error = deserializeJson(deviceIdentifiers, read_file);
        device_id = deviceIdentifiers["device_id"].as<std::string>();
        private_key_str = deviceIdentifiers["private_key"].as<std::string>();
        logln("New device ID: " + String(device_id.c_str()));
        // logln("New private key: " + String(private_key_str.c_str()));
        return;
      }
    }
    else {
      logln("Firmware Version File does not exist");
      return;
    }
  }
  else {
    return;
  }

}

void setup() ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
{
  
//   print_wakeup_reason((void *)NULL);
  Serial.begin(115200);

// // SpiffsInit((void *)NULL);
  if (!SPIFFS.begin()) // initializing SPIFFS
  {
    SPIFFS.begin(true);
  }
  setupWifi();
  handleDeviceIOTInfo();
  setupCloudIoT();
   
    // delay(10);
    while(1)
    {
    // if(!mqttClient->connected()) {
    //   //connect();
    // }
    }

  // InitTouchPad((void *)NULL);
  // AudioInit((void *)NULL);
  // StripInit((void *)NULL);
  // SleepInit((void *)NULL);
  // BatteryMonitorInit((void *)NULL);
 // FireBaseInit((void *)NULL);
  // SleepInit((void *)NULL);

  // WifiManagerInit((void *)NULL);
}

void loop()
{
  printf("In while looop");

}
void messageReceived(String &topic, String &payload){
}
