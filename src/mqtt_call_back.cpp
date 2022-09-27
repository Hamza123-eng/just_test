// /**
//  * @file mqtt_call_back.cpp
//  * @author Hamza Islam (hamzaislam@170.com)
//  * @brief File handling the the Call back from the MQTT SERVER
//  * @version 0.1
//  * @date 2022-08-30
//  * 
//  * @copyright Copyright (c) 2022
//  * @Supporter Scott Jensen 
//  */

 #include "mqtt_call_back.h"
#include <MQTT.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#include "SPIFFS.h"
#include  "ArduinoJson.h"
#include "Arduino.h"
#include "firebase_mqtt.h"
#include "main.h"
#include "wifi_manager.h"


#include <Client.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include <MQTT.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#include "firebase_mqtt.h"

FirebaseData fbdo;


uint16_t currentFirmwareVersion;
uint16_t newVersionInt;

int getFirmwareVersion();
void updateFirmwareVersion();
void fcsDownloadCallback(FCS_DownloadStatusInfo info);
void fcsOTADownloadCallback(FCS_DownloadStatusInfo info);

String getJwt(){
  String jwt;
  unsigned long iat = 0;
  iat = time(nullptr);
  printf("Refreshing JWT\n");
  jwt = device->createJWT(iat, jwt_exp_secs);
  return jwt;
}

/**
 * @brief This function is used to get the device version
 * 
 * @return int 
 */
int getFirmwareVersion()
{
  if (SPIFFS.exists("/firmwareVersion.json"))
  {
    File read_file = SPIFFS.open("/firmwareVersion.json", "r");
    if (!read_file)
    {
      logln("There was an error opening the Firmware Version File");
      return 0;
    }
    else
    {
      DynamicJsonDocument firmwareVersion(1024);
      deserializeJson(firmwareVersion, read_file);
      return firmwareVersion["current_version"].as<int>();
    }
  }
  else
  {
    logln("Firmware Version File does not exist");
    return 0;
  }
}

/**
 * @brief this function is use to Download the new firmware
 * 
 * @param newVersion 
 */
void writeNewFirmwareVersion(int newVersion)
{
  File write_file = SPIFFS.open("/firmwareVersion.json", "w");
  DynamicJsonDocument doc(1024);
  doc["current_version"] = newVersion;
  if (serializeJson(doc, write_file) == 0)
  {
    logln("Failed to write to Firmware Version file");
  }
  else
  {
    logln("Successfully wrote to Firmware Version file");
  }
  write_file.close();
}

/**
 * @brief this function is used to Update the software version.
 * 
 */

void updateFirmwareVersion()
{
  logln("Update Firmware HEAP  " + String(ESP.getFreeHeap()));

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  bool taskCompleted = false;
  while (1)
  {
    if (Firebase.ready() && !taskCompleted)
    {
      logln("Firebase Ready");
      taskCompleted = true;

      // If you want to get download url to use with your own OTA update process using core update library,
      // see Metadata.ino example

      logln("\nDownload firmware file...\n");

      const String download_path = "printed-version/firmware.bin";
      const String STORAGE_BUCKET_ID = "sana-e4e82-firmware";
      logln("Firebase HEAP  " + String(ESP.getFreeHeap()));

      if (!Firebase.Storage.downloadOTA(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, download_path /* path of firmware file stored in the bucket */, fcsOTADownloadCallback /* callback function */))
        logln(fbdo.errorReason());
    }
  }
}

// /**
//  * @brief  OTA Call Function  
//  * 
//  * @param info 
//  */
void fcsOTADownloadCallback(FCS_DownloadStatusInfo info)
{
  logln("OTA HEAP  " + String(ESP.getFreeHeap()));
  if (info.status == fb_esp_fcs_download_status_init)
  {
    Serial.printf("Downloading firmware %s (%d)\n", info.remoteFileName.c_str(), info.fileSize);
  }
  else if (info.status == fb_esp_fcs_download_status_download)
  {
    Serial.printf("Downloaded %d%s\n", (int)info.progress, "%");
  }
  else if (info.status == fb_esp_fcs_download_status_complete)
  {
    logln("Update firmware completed.");
    writeNewFirmwareVersion(newVersionInt);
    logln();
    logln("Restarting...\n\n");
    delay(2000);
    ESP.restart();
  }
  else if (info.status == fb_esp_fcs_download_status_error)
  {
    Serial.printf("Download firmware failed, %s\n", info.errorMsg.c_str());
  }
}
/**
 * @brief This fuction is used for the callback
 * 
 * @param info 
 */
void fcsDownloadCallback(FCS_DownloadStatusInfo info)
{
  if (info.status == fb_esp_fcs_download_status_init)
  {
    Serial.printf("Downloading file %s (%d) to %s\n", info.remoteFileName.c_str(), info.fileSize, info.localFileName.c_str());
  }
  else if (info.status == fb_esp_fcs_download_status_download)
  {
    Serial.printf("Downloaded %d%s\n", (int)info.progress, "%");
  }
  else if (info.status == fb_esp_fcs_download_status_complete)
  {
    Serial.println("Download completed\n");
  }
  else if (info.status == fb_esp_fcs_download_status_error)
  {
    Serial.printf("Download failed, %s\n", info.errorMsg.c_str());
  }
}

/**
 * @brief Mqtt callback Function.
 *
 * @param topic
 * @param payload
 */
void messageReceived(String &topic, String &payload)
{
    logln("Topic: " + topic + "Payload: " + payload);
    if (count_changed)
    {
        sendPlayCounts();
    }
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, payload);
    // Test if parsing succeeds.
    if (error)
    {
        log(F("serializeJson() failed: "));
        logln(error.f_str());
        return;
    }
    String type = doc["type"];

    if (type == "DOWNLOAD")
    { // differentiate between types of MQTT messages
        String updateCount = doc["updateCount"];
        String deviceId = doc["deviceId"];
        uint8_t updateCountInt = updateCount.toInt();

        if (updateCountInt > 0)
        {
            DynamicJsonDocument messDoc(1024);
            messDoc["orange"] = false;
            messDoc["blue"] = false;
            messDoc["purple"] = false;
            messDoc["yellow"] = false;
            messDoc["red"] = false;
            messDoc["green"] = false;

            Firebase.begin(&config, &auth);
            Firebase.reconnectWiFi(true);

            while (1)
            {
                if (Firebase.ready())
                {
                    for (uint8_t i = 0; i < updateCountInt; i++)
                    {
                        String button_json = doc["buttonArray"][i];
                        bool check;
                        String button_color = "";
                        String blue_uuid = doc["senderArray"]["blue_sender_id"];
                        String green_uuid = doc["senderArray"]["green_sender_id"];
                        String orange_uuid = doc["senderArray"]["orange_sender_id"];
                        String purple_uuid = doc["senderArray"]["purple_sender_id"];
                        String red_uuid = doc["senderArray"]["red_sender_id"];
                        String yellow_uuid = doc["senderArray"]["yellow_sender_id"];
                        switch (button_json[0])
                        {
                        case 'b':
                            button_color = "blue";
                            blue_sender_uuid = blue_uuid;
                            blue_count = 0;
                            break;
                        case 'g':
                            button_color = "green";
                            green_sender_uuid = green_uuid;
                            green_count = 0;
                            break;
                        case 'o':
                            button_color = "orange";
                            orange_sender_uuid = orange_uuid;
                            orange_count = 0;
                            break;
                        case 'p':
                            button_color = "purple";
                            purple_sender_uuid = purple_uuid;
                            purple_count = 0;
                            break;
                        case 'r':
                            button_color = "red";
                            red_sender_uuid = red_uuid;
                            red_count = 0;
                            break;
                        case 'y':
                            button_color = "yellow";
                            yellow_sender_uuid = yellow_uuid;
                            yellow_count = 0;
                            break;
                        default:
                            logln("Button Read Error");
                            break;
                        }
                        const String path = "/" + button_color + "_button_recording.mp3";
                        const String download_path = deviceId + "/mp3/" + button_color + "_button_recording.mp3";
                        const String STORAGE_BUCKET_ID = "sana-e4e82.appspot.com";
                        logln("HEAP Before Downloading " + String(ESP.getFreeHeap()));
                        check = Firebase.Storage.download(&fbdo, STORAGE_BUCKET_ID, download_path, path, mem_storage_type_flash, fcsDownloadCallback);
                        if (!check)
                        {
                            // Serial.print("Trying download again with HEAP: " + String(ESP.getFreeHeap()));
                            check = Firebase.Storage.download(&fbdo, STORAGE_BUCKET_ID, download_path, path, mem_storage_type_flash, fcsDownloadCallback);
                        }
                        Serial.printf("Download %s\n file... %s\n", download_path.c_str(), check ? "ok" : fbdo.errorReason().c_str());
                        messDoc[button_color] = !check;
                        writeSenderIDs();
                    }
                    break;
                }
            }
            String statusMessage;
            serializeJson(messDoc, Serial);
            serializeJson(messDoc, statusMessage);
            publishTelemetry(statusMessage);
            updateCountInt = 0;
        }
    }
    else if (type == "OTA")
    {
        String newVersion = doc["newVersion"];
        newVersionInt = newVersion.toInt();
        currentFirmwareVersion = getFirmwareVersion();

        if (currentFirmwareVersion != newVersionInt)
        {
            // maybe set a light flashing here
            updateFirmwareVersion();
        }
    }
}
