
// /**
//  * @file firebase_mqtt.cpp
//  * @author Hamza Islam (hamzaislam@170.com)
//  * @brief File handling the the Firebase
//  * @version 0.1
//  * @date 2022-08-30
//  *
//  * @copyright Copyright (c) 2022
//  * @Supporter Scott Jensen
//  */
// // #include "Arduino.h"
// #include "firebase_mqtt.h"
// #include <ArduinoJson.h>
// // #include "mqtt_call_back.h"
// #include "wifi_manager.h"
// #include "main.h"
// #include <freertos/queue.h>
// #include <freertos/task.h>

// #include <Client.h>
// #include <WiFi.h>
// #include <WiFiClientSecure.h>
// #include <ArduinoJson.h>
// #include <Firebase_ESP_Client.h>
// #include <MQTT.h>
// #include <CloudIoTCore.h>
// #include <CloudIoTCoreMqtt.h>
// #include "config.h"
// #include "addons/TokenHelper.h"

// // String UNSET = "UNSET";

// // FirebaseData fbdo;

// struct fb_esp_auth_signin_provider_t auth;
// struct fb_esp_cfg_t config;

// class CloudIoTCoreDevice *device;
// class CloudIoTCoreMqtt *mqtt;
// class MQTTClient *mqttClient;
// class Client *netClient;

// // uint16_t currentFirmwareVersion;
// // uint16_t newVersionInt;

// TaskHandle_t xTaskFireBase = NULL;
// QueueHandle_t xQueueFireBase = NULL;

// bool firebase_status = 0;
// bool read_wifi_status = 0;
// bool read_uuid_status = 0;

// String blue_sender_uuid = "";
// String green_sender_uuid = "";
// String orange_sender_uuid = "";
// String purple_sender_uuid = "";
// String red_sender_uuid = "";
// String yellow_sender_uuid = "";

// RTC_DATA_ATTR uint8_t blue_count = 0;
// RTC_DATA_ATTR uint8_t green_count = 0;
// RTC_DATA_ATTR uint8_t orange_count = 0;
// RTC_DATA_ATTR uint8_t purple_count = 0;
// RTC_DATA_ATTR uint8_t red_count = 0;
// RTC_DATA_ATTR uint8_t yellow_count = 0;
// RTC_DATA_ATTR bool count_changed = true;

// const char *project_id = "sana-e4e82";
// const char *location = "us-central1";
// const char *registry_id = "alpha-registry";

// const char *ntp_primary = "time.google.com";
// const char *ntp_secondary = "time.nist.gov";

// //   Device Id: Linkie-14     Private Key: 52:11:30:a0:25:87:80:df:d8:b2:90:41:e9:55:ff:01:61:d1:80:c4:e4:0f:d9:c9:27:2f:c3:0c:4e:80:7a:2

// int ex_num_topics = 0;
// char *ex_topics;

// const char *private_key_str = "2f:07:3a:51:83:12:51:ef:32:9d:e7:40:1d:e9:fb:"
//                               "ad:41:34:c1:60:0e:5b:cb:5e:ab:8a:f7:43:83:ee:"
//                               "ee:a6";
// const char *device_id = "Linkie-29";

// /*************************cert here**********************************/
// // const char *root_cert =
// //     "-----BEGIN CERTIFICATE-----\n"
// //     "MIIDDDCCArKgAwIBAgIUXIRd61ARosjr5tpYAQK1udlptnswCgYIKoZIzj0EAwIw\n"
// //     "RDELMAkGA1UEBhMCVVMxIjAgBgNVBAoTGUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBM\n"
// //     "TEMxETAPBgNVBAMTCEdUUyBMVFNYMB4XDTIxMDUyNTAwMDAwMFoXDTIyMDUyNDAw\n"
// //     "MDAwMFowbTELMAkGA1UEBhMCVVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNV\n"
// //     "BAcMDU1vdW50YWluIFZpZXcxEzARBgNVBAoMCkdvb2dsZSBMTEMxHDAaBgNVBAMM\n"
// //     "EyouMjAzMC5sdHNhcGlzLmdvb2cwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARR\n"
// //     "VX2FBT6/ZCFlCwom7Pr7jtlh99RHfH0cxO51PZ0gifi8mo2UasKfsw0ikuZvaEkG\n"
// //     "busnKgGwa6TrBElBabLNo4IBVzCCAVMwEwYDVR0lBAwwCgYIKwYBBQUHAwEwDgYD\n"
// //     "VR0PAQH/BAQDAgeAMB4GA1UdEQQXMBWCEyouMjAzMC5sdHNhcGlzLmdvb2cwDAYD\n"
// //     "VR0TAQH/BAIwADAfBgNVHSMEGDAWgBSzK6ugSBx+E4rJCMRAQiKiNlHiCjBpBggr\n"
// //     "BgEFBQcBAQRdMFswLwYIKwYBBQUHMAKGI2h0dHA6Ly9wa2kuZ29vZy9ndHNsdHNy\n"
// //     "L2d0c2x0c3guY3J0MCgGCCsGAQUFBzABhhxodHRwOi8vb2NzcC5wa2kuZ29vZy9H\n"
// //     "VFNMVFNYMCEGA1UdIAQaMBgwDAYKKwYBBAHWeQIFAzAIBgZngQwBAgIwMAYDVR0f\n"
// //     "BCkwJzAloCOgIYYfaHR0cDovL2NybC5wa2kuZ29vZy9HVFNMVFNYLmNybDAdBgNV\n"
// //     "HQ4EFgQUxp0CLjzIieJCqFTXjDc9okXUP80wCgYIKoZIzj0EAwIDSAAwRQIgAIuJ\n"
// //     "1QvJqFZwy6sZCP1+dXOX4YTWAbum6FtqyJwOKIACIQDENBALkXPS9jo0g8X5+eT9\n"
// //     "MlOQcPMMtbXGtK/ENpE2rw==\n"
// //     "-----END CERTIFICATE-----\n"
// //     "-----BEGIN CERTIFICATE-----\n"
// //     "MIIC0TCCAnagAwIBAgINAfQKmcm3qFVwT0+3nTAKBggqhkjOPQQDAjBEMQswCQYD\n"
// //     "VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzERMA8G\n"
// //     "A1UEAxMIR1RTIExUU1IwHhcNMTkwMTIzMDAwMDQyWhcNMjkwNDAxMDAwMDQyWjBE\n"
// //     "MQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExM\n"
// //     "QzERMA8GA1UEAxMIR1RTIExUU1gwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARr\n"
// //     "6/PTsGoOg9fXhJkj3CAk6C6DxHPnZ1I+ER40vEe290xgTp0gVplokojbN3pFx07f\n"
// //     "zYGYAX5EK7gDQYuhpQGIo4IBSzCCAUcwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQW\n"
// //     "MBQGCCsGAQUFBwMBBggrBgEFBQcDAjASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1Ud\n"
// //     "DgQWBBSzK6ugSBx+E4rJCMRAQiKiNlHiCjAfBgNVHSMEGDAWgBQ+/v/MUuu/ND49\n"
// //     "80DQ5CWxX7i7UjBpBggrBgEFBQcBAQRdMFswKAYIKwYBBQUHMAGGHGh0dHA6Ly9v\n"
// //     "Y3NwLnBraS5nb29nL2d0c2x0c3IwLwYIKwYBBQUHMAKGI2h0dHA6Ly9wa2kuZ29v\n"
// //     "Zy9ndHNsdHNyL2d0c2x0c3IuY3J0MDgGA1UdHwQxMC8wLaAroCmGJ2h0dHA6Ly9j\n"
// //     "cmwucGtpLmdvb2cvZ3RzbHRzci9ndHNsdHNyLmNybDAdBgNVHSAEFjAUMAgGBmeB\n"
// //     "DAECATAIBgZngQwBAgIwCgYIKoZIzj0EAwIDSQAwRgIhAPWeg2v4yeimG+lzmZAC\n"
// //     "DJOlalpsiwJR0VOeapY8/7aQAiEAiwRsSQXUmfVUW+N643GgvuMH70o2Agz8w67f\n"
// //     "SX+k+Lc=\n"
// //     "-----END CERTIFICATE-----\n";

// const char *root_cert =
//     "-----BEGIN CERTIFICATE-----\n"
//     "MIIEXDCCA0SgAwIBAgINAeOpMBz8cgY4P5pTHTANBgkqhkiG9w0BAQsFADBMMSAw\n"
//     "HgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMjETMBEGA1UEChMKR2xvYmFs\n"
//     "U2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjAeFw0xNzA2MTUwMDAwNDJaFw0yMTEy\n"
//     "MTUwMDAwNDJaMFQxCzAJBgNVBAYTAlVTMR4wHAYDVQQKExVHb29nbGUgVHJ1c3Qg\n"
//     "U2VydmljZXMxJTAjBgNVBAMTHEdvb2dsZSBJbnRlcm5ldCBBdXRob3JpdHkgRzMw\n"
//     "ggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDKUkvqHv/OJGuo2nIYaNVW\n"
//     "XQ5IWi01CXZaz6TIHLGp/lOJ+600/4hbn7vn6AAB3DVzdQOts7G5pH0rJnnOFUAK\n"
//     "71G4nzKMfHCGUksW/mona+Y2emJQ2N+aicwJKetPKRSIgAuPOB6Aahh8Hb2XO3h9\n"
//     "RUk2T0HNouB2VzxoMXlkyW7XUR5mw6JkLHnA52XDVoRTWkNty5oCINLvGmnRsJ1z\n"
//     "ouAqYGVQMc/7sy+/EYhALrVJEA8KbtyX+r8snwU5C1hUrwaW6MWOARa8qBpNQcWT\n"
//     "kaIeoYvy/sGIJEmjR0vFEwHdp1cSaWIr6/4g72n7OqXwfinu7ZYW97EfoOSQJeAz\n"
//     "AgMBAAGjggEzMIIBLzAOBgNVHQ8BAf8EBAMCAYYwHQYDVR0lBBYwFAYIKwYBBQUH\n"
//     "AwEGCCsGAQUFBwMCMBIGA1UdEwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHfCuFCa\n"
//     "Z3Z2sS3ChtCDoH6mfrpLMB8GA1UdIwQYMBaAFJviB1dnHB7AagbeWbSaLd/cGYYu\n"
//     "MDUGCCsGAQUFBwEBBCkwJzAlBggrBgEFBQcwAYYZaHR0cDovL29jc3AucGtpLmdv\n"
//     "b2cvZ3NyMjAyBgNVHR8EKzApMCegJaAjhiFodHRwOi8vY3JsLnBraS5nb29nL2dz\n"
//     "cjIvZ3NyMi5jcmwwPwYDVR0gBDgwNjA0BgZngQwBAgIwKjAoBggrBgEFBQcCARYc\n"
//     "aHR0cHM6Ly9wa2kuZ29vZy9yZXBvc2l0b3J5LzANBgkqhkiG9w0BAQsFAAOCAQEA\n"
//     "HLeJluRT7bvs26gyAZ8so81trUISd7O45skDUmAge1cnxhG1P2cNmSxbWsoiCt2e\n"
//     "ux9LSD+PAj2LIYRFHW31/6xoic1k4tbWXkDCjir37xTTNqRAMPUyFRWSdvt+nlPq\n"
//     "wnb8Oa2I/maSJukcxDjNSfpDh/Bd1lZNgdd/8cLdsE3+wypufJ9uXO1iQpnh9zbu\n"
//     "FIwsIONGl1p3A8CgxkqI/UAih3JaGOqcpcdaCIzkBaR9uYQ1X4k2Vg5APRLouzVy\n"
//     "7a8IVk6wuy6pm+T7HT4LY8ibS5FEZlfAFLSW8NwsVz9SBK2Vqn1N0PIMn5xA6NZV\n"
//     "c7o835DLAFshEWfC7TIe3g==\n"
//     "-----END CERTIFICATE-----\n";

// // const char *root_cert =
// //     "-----BEGIN CERTIFICATE-----\n"
// //     "MIICvTCCAmOgAwIBAgIUfXAmIec7M7FjzbdZBZjU0wbRPR8wCgYIKoZIzj0EAwIw\n"
// //     "RDELMAkGA1UEBhMCVVMxIjAgBgNVBAoTGUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBM\n"
// //     "TEMxETAPBgNVBAMTCEdUUyBMVFNYMB4XDTIyMDMyOTAwMDAwMFoXDTIzMDMyODAw\n"
// //     "MDAwMFowHjEcMBoGA1UEAwwTKi4yMDMwLmx0c2FwaXMuZ29vZzBZMBMGByqGSM49\n"
// //     "AgEGCCqGSM49AwEHA0IABJb3OTDbhSah6bxSnBE1GIN8fdcKnyaMkOWym8MC/DYb\n"
// //     "xn8PO16tuykiIAfyudGV/clMSSD7r50wBCzwmL3/+G+jggFXMIIBUzATBgNVHSUE\n"
// //     "DDAKBggrBgEFBQcDATAOBgNVHQ8BAf8EBAMCB4AwHgYDVR0RBBcwFYITKi4yMDMw\n"
// //     "Lmx0c2FwaXMuZ29vZzAMBgNVHRMBAf8EAjAAMB8GA1UdIwQYMBaAFLMrq6BIHH4T\n"
// //     "iskIxEBCIqI2UeIKMGkGCCsGAQUFBwEBBF0wWzAvBggrBgEFBQcwAoYjaHR0cDov\n"
// //     "L3BraS5nb29nL2d0c2x0c3IvZ3RzbHRzeC5jcnQwKAYIKwYBBQUHMAGGHGh0dHA6\n"
// //     "Ly9vY3NwLnBraS5nb29nL0dUU0xUU1gwIQYDVR0gBBowGDAMBgorBgEEAdZ5AgUD\n"
// //     "MAgGBmeBDAECATAwBgNVHR8EKTAnMCWgI6Ahhh9odHRwOi8vY3JsLnBraS5nb29n\n"
// //     "L0dUU0xUU1guY3JsMB0GA1UdDgQWBBTjW5lTveo1QiBCQ7jslByIITAlUTAKBggq\n"
// //     "hkjOPQQDAgNIADBFAiBn/6GkEkYTej1fYmG/qFRlm9HvuhEAKfTKPvXvCfCufwIh\n"
// //     "AL/M3xuaDoWqbliWgeH5Ig+JjUT1Kw8P1Jh/EIcfAFuZ\n"
// //     "-----END CERTIFICATE-----\n"
// //     "-----BEGIN CERTIFICATE-----\n"
// //     "MIIC0TCCAnagAwIBAgINAfQKmcm3qFVwT0+3nTAKBggqhkjOPQQDAjBEMQswCQYD\n"
// //     "VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzERMA8G\n"
// //     "A1UEAxMIR1RTIExUU1IwHhcNMTkwMTIzMDAwMDQyWhcNMjkwNDAxMDAwMDQyWjBE\n"
// //     "MQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExM\n"
// //     "QzERMA8GA1UEAxMIR1RTIExUU1gwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARr\n"
// //     "6/PTsGoOg9fXhJkj3CAk6C6DxHPnZ1I+ER40vEe290xgTp0gVplokojbN3pFx07f\n"
// //     "zYGYAX5EK7gDQYuhpQGIo4IBSzCCAUcwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQW\n"
// //     "MBQGCCsGAQUFBwMBBggrBgEFBQcDAjASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1Ud\n"
// //     "DgQWBBSzK6ugSBx+E4rJCMRAQiKiNlHiCjAfBgNVHSMEGDAWgBQ+/v/MUuu/ND49\n"
// //     "80DQ5CWxX7i7UjBpBggrBgEFBQcBAQRdMFswKAYIKwYBBQUHMAGGHGh0dHA6Ly9v\n"
// //     "Y3NwLnBraS5nb29nL2d0c2x0c3IwLwYIKwYBBQUHMAKGI2h0dHA6Ly9wa2kuZ29v\n"
// //     "Zy9ndHNsdHNyL2d0c2x0c3IuY3J0MDgGA1UdHwQxMC8wLaAroCmGJ2h0dHA6Ly9j\n"
// //     "cmwucGtpLmdvb2cvZ3RzbHRzci9ndHNsdHNyLmNybDAdBgNVHSAEFjAUMAgGBmeB\n"
// //     "DAECATAIBgZngQwBAgIwCgYIKoZIzj0EAwIDSQAwRgIhAPWeg2v4yeimG+lzmZAC\n"
// //     "DJOlalpsiwJR0VOeapY8/7aQAiEAiwRsSQXUmfVUW+N643GgvuMH70o2Agz8w67f\n"
// //     "SX+k+Lc=\n"
// //     "-----END CERTIFICATE-----\n";
// // //     /******************************************************************/
// bool publishTelemetry(String data)
// {
//     return mqtt->publishTelemetry(data);
// }

// bool publishTelemetry(String subfolder, String data)
// {
//     return mqtt->publishTelemetry(subfolder, data);
// }
// void setupCloudIoT()
// {
//     printf("Going to setup the IOT CLOUD\n");

//     // Client *netClient;
//     device = new CloudIoTCoreDevice(
//         project_id, location, registry_id, device_id,
//         private_key_str);
//     netClient = new WiFiClientSecure();
//     ((WiFiClientSecure *)netClient)->setCACert(root_cert); // necessary to properly connect to IoTCore
//     mqttClient = new MQTTClient(500);
//     mqttClient->setOptions(1800, true, 10000); // keepAlive, cleanSession, timeout
//     mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);

//     config.api_key = "AIzaSyA6TYcNhd3F38I3zaU7vNIYXXWimPovnrc";
//     auth.user.email = "scott@caravelassociate.com";
//     auth.user.password = "baedar11.0";
//     config.token_status_callback = tokenStatusCallback;
// }

// bool FillFireBaseCredential()
// {
//     logln("handle identifiers");
//     if ((device_id == "UNSET") || (private_key_str == "UNSET"))
//     {
//         logln("identifiers are unset");
//         if (SPIFFS.exists("/deviceIdentifiers.json"))
//         {
//             File read_file = SPIFFS.open("/deviceIdentifiers.json", "r");
//             if (!read_file)
//             {
//                 logln("There was an error opening the Device Identifiers File");
//                 return 0;
//             }
//             else
//             {
//                 DynamicJsonDocument deviceIdentifiers(1024);
//                 deserializeJson(deviceIdentifiers, read_file);
//                // device_id = deviceIdentifiers["device_id"].as<std::string>();
//                // private_key_str = deviceIdentifiers["private_key"].as<std::string>();
//                 logln("New device ID: " + String(device_id.c_str()));
//                 // logln("New private key: " + String(private_key_str.c_str()));
//                 return 1;
//             }
//         }
//         else
//         {
//             logln("Firmware Version File does not exist");
//             return 0;
//         }
//     }
//     else
//     {
//         return 0;
//     }
//     return 1;
// }
// void ConnectToTopics()
// {

//     mqtt->setUseLts(true);
//     mqtt->startMQTT();
//     sendPlayCounts();
//     mqtt->loop();
//     mqtt->mqttConnect();
//     printf("TOPICS SUBSCRIBED\n");
// }

// /**
//  * @brief This fucntion is used to Send PlayCount
//  *
//  */
// void sendPlayCounts()
// {
//     printf("IN SEND PLAY COUNT\n");
//     if (blue_count + green_count + orange_count + purple_count + red_count + yellow_count > 0)
//     {
//         DynamicJsonDocument message(1024);
//         if (blue_count > 0 && blue_sender_uuid.length() > 0)
//         {
//             message[blue_sender_uuid] = blue_count;
//             blue_count = 0;
//         }
//         if (green_count > 0 && green_sender_uuid.length() > 0)
//         {
//             message[green_sender_uuid] = green_count;
//             green_count = 0;
//         }
//         if (orange_count > 0 && orange_sender_uuid.length() > 0)
//         {
//             message[orange_sender_uuid] = orange_count;
//             orange_count = 0;
//         }
//         if (purple_count > 0 && purple_sender_uuid.length() > 0)
//         {
//             message[purple_sender_uuid] = purple_count;
//             purple_count = 0;
//         }
//         if (red_count > 0 && red_sender_uuid.length() > 0)
//         {
//             message[purple_sender_uuid] = purple_count;
//             red_count = 0;
//         }
//         if (yellow_count > 0 && yellow_sender_uuid.length() > 0)
//         {
//             message[yellow_sender_uuid] = yellow_count;
//             yellow_count = 0;
//         }

//         String messageString;
//         serializeJson(message, Serial);
//         serializeJson(message, messageString);
//         publishTelemetry("/playCounts", messageString);
//         count_changed = false;
//     }
// }

// void FireBaseTask(void *param)
// {

//     WifiManagerInit((void *)NULL);
//     readSenderIDs();

//     // FillFireBaseCredential();

//     /*Setup the cloud*/

//     setupCloudIoT();

//     /* */

//     bool dummy;

//     while (1)
//     {
//         if (WiFi.status() == WL_CONNECTED)
//         {

//             while (time(nullptr) < 1510644967)
//             {
//                 delay(10);
//             }

//             while (1)
//             {
//                 if (!firebase_status)
//                 {
//                     printf("Going to Connect to the topics\n");
//                     ConnectToTopics();
//                     firebase_status = true;
//                 }
//                 else if (!mqttClient->connected())
//                 {
//                     printf("In desired\n");
//                     firebase_status = false;
//                     break;
//                 }
//                 /*Every thisng here is good do Action here*/
//                 else
//                 {
//                     if ((xQueueReceive(xQueueFireBase, &(dummy), (5 / portTICK_PERIOD_MS)) == 1))
//                     {
//                         sendPlayCounts();
//                     }
//                 }
//                 printf("Waiitng for the messages\n");
//                 vTaskDelay(500);
//             }
//         }
//         else
//         {
//             TryConnection(NULL);
//         }
//         vTaskDelay(300);
//     }
// }

// void FireBaseInit(void *param)
// {
//     xQueueFireBase = xQueueCreate(2, sizeof(bool));
//     if (xQueueFireBase == NULL)
//     {
//         printf("********FireBase  QUEUE IS NOT CREATED SUCESSFULLY*******\n");
//     }
//     else
//     {
//         printf("********FireBasePLAY AUDIO QUEUE CREATED SUCESSFULLY*******\n");

//         xTaskCreatePinnedToCore(&FireBaseTask, "FireBase_Task", 1024 * 50, NULL, 3, &xTaskFireBase, 1);
//     }
// }