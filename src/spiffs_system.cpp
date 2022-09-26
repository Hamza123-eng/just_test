// /**
//  * @file spiffs_system.cpp
//  * @author Hamza Islam (hamzaislam@170.com)
//  * @brief File handling the SFFIPS Control
//  * @version 0.1
//  * @date 2022-08-30
//  *
//  * @copyright Copyright (c) 2022
//  * @Supporter Scott Jensen
//  */
// #include "SPIFFS.h"

// #include "spiffs_system.h"
// #include <ArduinoJson.h>
// #include "SPIFFS.h"
// #include "main.h"
// #include "config.h"
// #include "firebase_mqtt.h"
// #include "wifi_manager.h"

// void Init_spiffs();
// bool ReadWifiFromFile();
// bool readSenderIDs();
// bool writeSenderIDs();
// int getFirmwareVersion();
// void writeNewFirmwareVersion(int newVersion);

// bool wifi_read = false;
// bool sender_id_read = false;

// /**
//  * @brief  This function is used Initization of The Spiffs
//  *
//  */

// void SpiffsInit(void *param)
// {
//     if (!SPIFFS.begin()) // initializing SPIFFS
//     {
//         SPIFFS.begin(true);
//     }
//     if (ReadWifiFromFile())
//     {
//         wifi_read = 1;
//     }
//     else
//     {
//         wifi_read = 0;
//     }

//     if (readSenderIDs())
//     {
//         sender_id_read = 1;
//     }
//     else
//     {
//         sender_id_read = 0;
//     }
// }
// /**
//  * @brief This Function is used to read wifi SSID and PSWD redential fron the SPIFFS.
//  *
//  */

// bool ReadWifiFromFile()
// {
//     bool status = false;

//     if (SPIFFS.exists("/wifi.json"))
//     {
//         File read_file = SPIFFS.open("/wifi.json", "r");
//         if (!read_file)
//         {
//             logln("SPIFFS FILE SYSTEM: ERROR IN OPENING WIFI CRENDIATIAL FILE");
//             status = 0;
//         }
//         else
//         { // reads the stored wifi credentials
//             DynamicJsonDocument wifi(1024);
//             DeserializationError error = deserializeJson(wifi, read_file);
//             SSID_1 = wifi["SSID_1"].as<String>();
//             password_1 = wifi["password_1"].as<String>();
//             SSID_2 = wifi["SSID_2"].as<String>();
//             password_2 = wifi["password_2"].as<String>();
//             SSID_3 = wifi["SSID_3"].as<String>();
//             password_3 = wifi["password_3"].as<String>();
//             SSID_4 = wifi["SSID_4"].as<String>();
//             password_4 = wifi["password_4"].as<String>();
//             SSID_5 = wifi["SSID_5"].as<String>();
//             password_5 = wifi["password_5"].as<String>();
//             read_file.close();

//             status = 1;
//         }
//     }
//     else
//     {

//         File write_file = SPIFFS.open("/wifi.json", "w");
//         DynamicJsonDocument wifi(1024);
//         wifi["SSID_1"] = SSID_1;
//         wifi["password_1"] = password_1;
//         wifi["SSID_2"] = SSID_2;
//         wifi["password_2"] = password_2;
//         wifi["SSID_3"] = SSID_3;
//         wifi["password_3"] = password_3;
//         wifi["SSID_4"] = SSID_4;
//         wifi["password_4"] = password_4;
//         wifi["SSID_5"] = SSID_5;
//         wifi["password_5"] = password_5;
//         if (serializeJson(wifi, write_file) == 0)
//         {
//             logln("SPIFFS FILE SYSTEM: FAILED TO WRITE WIFI FILE");
//             status = 0;
//         }
//         else
//         {
//             logln("SPIFFS FILE SYSTEM: SUCCESS TO WRITE WIFI FILE");
//             status = 1;
//         }
//         write_file.close();
//     }

//     return status;
// }
// /**
//  * @brief This Function Is used to READ senders IDS.
//  *
//  */
// bool readSenderIDs()
// {
//     if (SPIFFS.exists("/senderIDs.json"))
//     {
//         File read_file = SPIFFS.open("/senderIDs.json", "r");
//         if (!read_file)
//         {
//             logln("There was an error opening the Sender IDs File");
//             return 0;
//         }
//         else
//         { // reads the stored wifi credentials
//             DynamicJsonDocument doc(1024);
//             DeserializationError error = deserializeJson(doc, read_file);
//             blue_sender_uuid = doc["blue_sender_uuid"].as<String>();
//             green_sender_uuid = doc["green_sender_uuid"].as<String>();
//             orange_sender_uuid = doc["orange_sender_uuid"].as<String>();
//             purple_sender_uuid = doc["purple_sender_uuid"].as<String>();
//             red_sender_uuid = doc["red_sender_uuid"].as<String>();
//             yellow_sender_uuid = doc["yellow_sender_uuid"].as<String>();
//             read_file.close();
//         }
//         return 1;
//     }
//     else
//     {
//         return (writeSenderIDs());
//     }
// }

// /**
//  * @brief This is used to Record the senders IDS.
//  *
//  */

// bool writeSenderIDs()
// { // on first set up and on download - will allow us to keep current senderIds, even after toy turned off
//     File write_file = SPIFFS.open("/senderIDs.json", "w");
//     DynamicJsonDocument doc(1024);
//     doc["blue_sender_uuid"] = blue_sender_uuid;
//     doc["green_sender_uuid"] = green_sender_uuid;
//     doc["orange_sender_uuid"] = orange_sender_uuid;
//     doc["purple_sender_uuid"] = purple_sender_uuid;
//     doc["red_sender_uuid"] = red_sender_uuid;
//     doc["yellow_sender_uuid"] = yellow_sender_uuid;
//     if (serializeJson(doc, write_file) == 0)
//     {
//         logln("Failed to write to Sender ID file");
//         write_file.close();
//         return 0;
//     }
//     else
//     {
//         logln("Successfully wrote to Sender ID file");
//         write_file.close();
//         return 1;
//     }
// }

// int getFirmwareVersion()
// {
//     if (SPIFFS.exists("/firmwareVersion.json"))
//     {
//         File read_file = SPIFFS.open("/firmwareVersion.json", "r");
//         if (!read_file)
//         {
//             logln("There was an error opening the Firmware Version File");
//             return 0;
//         }
//         else
//         {
//             DynamicJsonDocument firmwareVersion(1024);
//             DeserializationError error = deserializeJson(firmwareVersion, read_file);
//             return firmwareVersion["current_version"].as<int>();
//         }
//     }
//     else
//     {
//         logln("Firmware Version File does not exist");
//         return 0;
//     }
// }

// // Writes the new Firmware Version into SPIFFS
// void writeNewFirmwareVersion(int newVersion)
// {
//     File write_file = SPIFFS.open("/firmwareVersion.json", "w");
//     DynamicJsonDocument doc(1024);
//     doc["current_version"] = newVersion;
//     if (serializeJson(doc, write_file) == 0)
//     {
//         logln("Failed to write to Firmware Version file");
//     }
//     else
//     {
//         logln("Successfully wrote to Firmware Version file");
//     }
//     write_file.close();
// }

// void handleDeviceIOTInfo()
// {
//     logln("handle identifiers");
//     if ((device_id == UNSET) || (private_key_str == UNSET))
//     {
//         logln("identifiers are unset");
//         if (SPIFFS.exists("/deviceIdentifiers.json"))
//         {
//             File read_file = SPIFFS.open("/deviceIdentifiers.json", "r");
//             if (!read_file)
//             {
//                 logln("There was an error opening the Device Identifiers File");
//                 return;
//             }
//             else
//             {
//                 DynamicJsonDocument deviceIdentifiers(1024);
//                 DeserializationError error = deserializeJson(deviceIdentifiers, read_file);
//                 device_id = deviceIdentifiers["device_id"].as<std::string>();
//                 private_key_str = deviceIdentifiers["private_key"].as<std::string>();
//                 logln("New device ID: " + String(device_id.c_str()));
//                 // logln("New private key: " + String(private_key_str.c_str()));
//                 return;
//             }
//         }
//         else
//         {
//             logln("Firmware Version File does not exist");
//             return;
//         }
//     }
//     else
//     {
//         return;
//     }
// }
