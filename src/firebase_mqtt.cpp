
/**
 * @file firebase_mqtt.cpp
 * @author Hamza Islam (hamzaislam@170.com)
 * @brief File handling the the Firebase
 * @version 0.1
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022
 * @Supporter Scott Jensen
 */

#include "firebase_mqtt.h"
#include <ArduinoJson.h>
#include "mqtt_call_back.h"
#include "wifi_manager.h"
#include "main.h"
#include <freertos/queue.h>
#include <freertos/task.h>

#include <Client.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Firebase_ESP_Client.h>
#include <MQTT.h>
#include <CloudIoTCore.h>
#include <CloudIoTCoreMqtt.h>
#include "config.h"
#include "addons/TokenHelper.h"

// String UNSET = "UNSET";

// FirebaseData fbdo;
class fb_esp_auth_signin_provider_t auth;
class fb_esp_cfg_t config;

class CloudIoTCoreDevice *device;
class CloudIoTCoreMqtt *mqtt;
class MQTTClient *mqttClient;
Client *netClient;

// uint16_t currentFirmwareVersion;
// uint16_t newVersionInt;

TaskHandle_t xTaskFireBase = NULL;
QueueHandle_t xQueueFireBase = NULL;

bool firebase_status = 0;
bool read_wifi_status = 0;
bool read_uuid_status = 0;

String blue_sender_uuid = "";
String green_sender_uuid = "";
String orange_sender_uuid = "";
String purple_sender_uuid = "";
String red_sender_uuid = "";
String yellow_sender_uuid = "";

RTC_DATA_ATTR uint8_t blue_count = 0;
RTC_DATA_ATTR uint8_t green_count = 0;
RTC_DATA_ATTR uint8_t orange_count = 0;
RTC_DATA_ATTR uint8_t purple_count = 0;
RTC_DATA_ATTR uint8_t red_count = 0;
RTC_DATA_ATTR uint8_t yellow_count = 0;
RTC_DATA_ATTR bool count_changed = true;

char *project_id = "sana-e4e82";
char *location = "us-central1";
char *registry_id = "alpha-registry";

char *ntp_primary = "pool.ntp.org";
char *ntp_secondary = "time.nist.gov";

//   Device Id: Linkie-14     Private Key: 52:11:30:a0:25:87:80:df:d8:b2:90:41:e9:55:ff:01:61:d1:80:c4:e4:0f:d9:c9:27:2f:c3:0c:4e:80:7a:2

int ex_num_topics = 0;
char *ex_topics;

std::string private_key_str = "UNSET";
std::string device_id = "UNSET";

/*************************cert here**********************************/
const char *root_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDDDCCArKgAwIBAgIUXIRd61ARosjr5tpYAQK1udlptnswCgYIKoZIzj0EAwIw\n"
    "RDELMAkGA1UEBhMCVVMxIjAgBgNVBAoTGUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBM\n"
    "TEMxETAPBgNVBAMTCEdUUyBMVFNYMB4XDTIxMDUyNTAwMDAwMFoXDTIyMDUyNDAw\n"
    "MDAwMFowbTELMAkGA1UEBhMCVVMxEzARBgNVBAgMCkNhbGlmb3JuaWExFjAUBgNV\n"
    "BAcMDU1vdW50YWluIFZpZXcxEzARBgNVBAoMCkdvb2dsZSBMTEMxHDAaBgNVBAMM\n"
    "EyouMjAzMC5sdHNhcGlzLmdvb2cwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARR\n"
    "VX2FBT6/ZCFlCwom7Pr7jtlh99RHfH0cxO51PZ0gifi8mo2UasKfsw0ikuZvaEkG\n"
    "busnKgGwa6TrBElBabLNo4IBVzCCAVMwEwYDVR0lBAwwCgYIKwYBBQUHAwEwDgYD\n"
    "VR0PAQH/BAQDAgeAMB4GA1UdEQQXMBWCEyouMjAzMC5sdHNhcGlzLmdvb2cwDAYD\n"
    "VR0TAQH/BAIwADAfBgNVHSMEGDAWgBSzK6ugSBx+E4rJCMRAQiKiNlHiCjBpBggr\n"
    "BgEFBQcBAQRdMFswLwYIKwYBBQUHMAKGI2h0dHA6Ly9wa2kuZ29vZy9ndHNsdHNy\n"
    "L2d0c2x0c3guY3J0MCgGCCsGAQUFBzABhhxodHRwOi8vb2NzcC5wa2kuZ29vZy9H\n"
    "VFNMVFNYMCEGA1UdIAQaMBgwDAYKKwYBBAHWeQIFAzAIBgZngQwBAgIwMAYDVR0f\n"
    "BCkwJzAloCOgIYYfaHR0cDovL2NybC5wa2kuZ29vZy9HVFNMVFNYLmNybDAdBgNV\n"
    "HQ4EFgQUxp0CLjzIieJCqFTXjDc9okXUP80wCgYIKoZIzj0EAwIDSAAwRQIgAIuJ\n"
    "1QvJqFZwy6sZCP1+dXOX4YTWAbum6FtqyJwOKIACIQDENBALkXPS9jo0g8X5+eT9\n"
    "MlOQcPMMtbXGtK/ENpE2rw==\n"
    "-----END CERTIFICATE-----\n"
    "-----BEGIN CERTIFICATE-----\n"
    "MIIC0TCCAnagAwIBAgINAfQKmcm3qFVwT0+3nTAKBggqhkjOPQQDAjBEMQswCQYD\n"
    "VQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzERMA8G\n"
    "A1UEAxMIR1RTIExUU1IwHhcNMTkwMTIzMDAwMDQyWhcNMjkwNDAxMDAwMDQyWjBE\n"
    "MQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExM\n"
    "QzERMA8GA1UEAxMIR1RTIExUU1gwWTATBgcqhkjOPQIBBggqhkjOPQMBBwNCAARr\n"
    "6/PTsGoOg9fXhJkj3CAk6C6DxHPnZ1I+ER40vEe290xgTp0gVplokojbN3pFx07f\n"
    "zYGYAX5EK7gDQYuhpQGIo4IBSzCCAUcwDgYDVR0PAQH/BAQDAgGGMB0GA1UdJQQW\n"
    "MBQGCCsGAQUFBwMBBggrBgEFBQcDAjASBgNVHRMBAf8ECDAGAQH/AgEAMB0GA1Ud\n"
    "DgQWBBSzK6ugSBx+E4rJCMRAQiKiNlHiCjAfBgNVHSMEGDAWgBQ+/v/MUuu/ND49\n"
    "80DQ5CWxX7i7UjBpBggrBgEFBQcBAQRdMFswKAYIKwYBBQUHMAGGHGh0dHA6Ly9v\n"
    "Y3NwLnBraS5nb29nL2d0c2x0c3IwLwYIKwYBBQUHMAKGI2h0dHA6Ly9wa2kuZ29v\n"
    "Zy9ndHNsdHNyL2d0c2x0c3IuY3J0MDgGA1UdHwQxMC8wLaAroCmGJ2h0dHA6Ly9j\n"
    "cmwucGtpLmdvb2cvZ3RzbHRzci9ndHNsdHNyLmNybDAdBgNVHSAEFjAUMAgGBmeB\n"
    "DAECATAIBgZngQwBAgIwCgYIKoZIzj0EAwIDSQAwRgIhAPWeg2v4yeimG+lzmZAC\n"
    "DJOlalpsiwJR0VOeapY8/7aQAiEAiwRsSQXUmfVUW+N643GgvuMH70o2Agz8w67f\n"
    "SX+k+Lc=\n"
    "-----END CERTIFICATE-----\n";
/******************************************************************/
bool publishTelemetry(String data)
{
    return mqtt->publishTelemetry(data);
}

bool publishTelemetry(String subfolder, String data)
{
    return mqtt->publishTelemetry(subfolder, data);
}
void setupCloudIoT()
{
    printf("Going to setup the IOT CLOUD\n");

    device = new CloudIoTCoreDevice(
        project_id, location, registry_id, device_id.c_str(),
        private_key_str.c_str());
    netClient = new WiFiClientSecure();
    ((WiFiClientSecure *)netClient)->setCACert(root_cert); // necessary to properly connect to IoTCore
    mqttClient = new MQTTClient(512);
    mqttClient->setOptions(180, true, 1000); // keepAlive, cleanSession, timeout
    mqtt = new CloudIoTCoreMqtt(mqttClient, netClient, device);

    config.api_key = "AIzaSyA6TYcNhd3F38I3zaU7vNIYXXWimPovnrc";
    auth.user.email = "scott@caravelassociate.com";
    auth.user.password = "baedar11.0";
    config.token_status_callback = tokenStatusCallback;
}

bool FillFireBaseCredential()
{
    logln("handle identifiers");
    if ((device_id == "UNSET") || (private_key_str == "UNSET"))
    {
        logln("identifiers are unset");
        if (SPIFFS.exists("/deviceIdentifiers.json"))
        {
            File read_file = SPIFFS.open("/deviceIdentifiers.json", "r");
            if (!read_file)
            {
                logln("There was an error opening the Device Identifiers File");
                return 0;
            }
            else
            {
                DynamicJsonDocument deviceIdentifiers(1024);
                deserializeJson(deviceIdentifiers, read_file);
                device_id = deviceIdentifiers["device_id"].as<std::string>();
                private_key_str = deviceIdentifiers["private_key"].as<std::string>();
                logln("New device ID: " + String(device_id.c_str()));
                // logln("New private key: " + String(private_key_str.c_str()));
                return 1;
            }
        }
        else
        {
            logln("Firmware Version File does not exist");
            return 0;
        }
    }
    else
    {
        return 0;
    }
    return 1;
}
void ConnectToTopics()
{
    mqtt->setUseLts(true);
    mqtt->startMQTT();

    sendPlayCounts();

    mqtt->loop();
    mqtt->mqttConnect();
    printf("TOPICS SUBSCRIBED\n");
}

/**
 * @brief This fucntion is used to Send PlayCount
 *
 */
void sendPlayCounts()
{
    printf("IN SEND PLAY COUNT\n");
    if (blue_count + green_count + orange_count + purple_count + red_count + yellow_count > 0)
    {
        DynamicJsonDocument message(1024);
        if (blue_count > 0 && blue_sender_uuid.length() > 0)
        {
            message[blue_sender_uuid] = blue_count;
            blue_count = 0;
        }
        if (green_count > 0 && green_sender_uuid.length() > 0)
        {
            message[green_sender_uuid] = green_count;
            green_count = 0;
        }
        if (orange_count > 0 && orange_sender_uuid.length() > 0)
        {
            message[orange_sender_uuid] = orange_count;
            orange_count = 0;
        }
        if (purple_count > 0 && purple_sender_uuid.length() > 0)
        {
            message[purple_sender_uuid] = purple_count;
            purple_count = 0;
        }
        if (red_count > 0 && red_sender_uuid.length() > 0)
        {
            message[purple_sender_uuid] = purple_count;
            red_count = 0;
        }
        if (yellow_count > 0 && yellow_sender_uuid.length() > 0)
        {
            message[yellow_sender_uuid] = yellow_count;
            yellow_count = 0;
        }

        String messageString;
        serializeJson(message, Serial);
        serializeJson(message, messageString);
        publishTelemetry("/playCounts", messageString);
        count_changed = false;
    }
}

void FireBaseTask(void *param)
{

    WifiManagerInit((void *)NULL);
    readSenderIDs();

     FillFireBaseCredential();

    /*Setup the cloud*/

    setupCloudIoT();

    /* */

    bool dummy;

    while (1)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            while (1)
            {
                if (!firebase_status)
                {
                    printf("Going to Connect to the topics\n");
                    ConnectToTopics();
                    firebase_status = true;
                }
                else if (!mqttClient->connected())
                {
                    printf("In desired\n");
                    firebase_status = false;
                    break;
                }
                /*Every thisng here is good do Action here*/
                else
                {
                    if ((xQueueReceive(xQueueFireBase, &(dummy), (5 / portTICK_PERIOD_MS)) == 1))
                    {
                        sendPlayCounts();
                    }
                }
                printf("Waiitng for the messages\n");
                mqtt->loop();
                vTaskDelay(1000);
            }
        }
        else
        {
            TryConnection(NULL);
        }
        vTaskDelay(300);
    }
}

void FireBaseInit(void *param)
{
    xQueueFireBase = xQueueCreate(2, sizeof(bool));
    if (xQueueFireBase == NULL)
    {
        printf("********FireBase  QUEUE IS NOT CREATED SUCESSFULLY*******\n");
    }
    else
    {
        printf("********FireBasePLAY AUDIO QUEUE CREATED SUCESSFULLY*******\n");

        xTaskCreatePinnedToCore(&FireBaseTask, "FireBase_Task", 1024 * 10, NULL, 3, &xTaskFireBase, 1);
    }
}