// /**
//  * @file wifi_manager.cpp
//  * @author Hamza Islam (hamzaislam@170.com)
//  * @brief File handling Wifi Engine for the Linkie
//  * @version 0.1
//  * @date 2022-08-30
//  *
//  * @copyright Copyright (c) 2022
//  * @Supporter Scott Jensen
//  */

#include "wifi_manager.h"
#include "spiffs_system.h"
#include "BLE.h"
#include "main.h"
#include "ArduinoJson.h"
#include "firebase_mqtt.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "touch_pad_custom.h"
#include "i2s_play.h"

#include "SPIFFS.h"

String UNSET = "UNSET";

String SSID_1 = "";
String password_1 = "";
String SSID_2 = "";
String password_2 = "";
String SSID_3 = "";
String password_3 = "";
String SSID_4 = "";
String password_4 = "";
String SSID_5 = "";
String password_5 = "";

void readSenderIDs()
{
    if (SPIFFS.exists("/senderIDs.json"))
    {
        File read_file = SPIFFS.open("/senderIDs.json", "r");
        if (!read_file)
        {
            logln("There was an error opening the Sender IDs File");
        }
        else
        { // reads the stored wifi credentials
            DynamicJsonDocument doc(1024);
            DeserializationError error = deserializeJson(doc, read_file);
            blue_sender_uuid = doc["blue_sender_uuid"].as<String>();
            green_sender_uuid = doc["green_sender_uuid"].as<String>();
            orange_sender_uuid = doc["orange_sender_uuid"].as<String>();
            purple_sender_uuid = doc["purple_sender_uuid"].as<String>();
            red_sender_uuid = doc["red_sender_uuid"].as<String>();
            yellow_sender_uuid = doc["yellow_sender_uuid"].as<String>();
            read_file.close();
        }
    }
    else
    {
        writeSenderIDs();
    }
}

void writeSenderIDs()
{ // on first set up and on download - will allow us to keep current senderIds, even after toy turned off
    printf("Going for writing the uuid\n");
    File write_file = SPIFFS.open("/senderIDs.json", "w");
    DynamicJsonDocument doc(1024);
    doc["blue_sender_uuid"] = blue_sender_uuid;
    doc["green_sender_uuid"] = green_sender_uuid;
    doc["orange_sender_uuid"] = orange_sender_uuid;
    doc["purple_sender_uuid"] = purple_sender_uuid;
    doc["red_sender_uuid"] = red_sender_uuid;
    doc["yellow_sender_uuid"] = yellow_sender_uuid;
    if (serializeJson(doc, write_file) == 0)
    {
        logln("Failed to write to Sender ID file");
    }
    else
    {
        logln("Successfully wrote to Sender ID file");
    }
    write_file.close();
}

void readWifiFromFile()
{
    if (SPIFFS.exists("/wifi.json"))
    {
        File read_file = SPIFFS.open("/wifi.json", "r");
        if (!read_file)
        {
            logln("There was an error opening the Wifi File");
        }
        else
        { // reads the stored wifi credentials
            DynamicJsonDocument wifi(1024);
            DeserializationError error = deserializeJson(wifi, read_file);
            SSID_1 = wifi["SSID_1"].as<String>();
            password_1 = wifi["password_1"].as<String>();
            SSID_2 = wifi["SSID_2"].as<String>();
            password_2 = wifi["password_2"].as<String>();
            SSID_3 = wifi["SSID_3"].as<String>();
            password_3 = wifi["password_3"].as<String>();
            SSID_4 = wifi["SSID_4"].as<String>();
            password_4 = wifi["password_4"].as<String>();
            SSID_5 = wifi["SSID_5"].as<String>();
            password_5 = wifi["password_5"].as<String>();
            read_file.close();
        }
    }
    else
    { // formats the file for the first time - is this taking unneccessary memory
        File write_file = SPIFFS.open("/wifi.json", "w");
        DynamicJsonDocument wifi(1024);
        wifi["SSID_1"] = SSID_1;
        wifi["password_1"] = password_1;
        wifi["SSID_2"] = SSID_2;
        wifi["password_2"] = password_2;
        wifi["SSID_3"] = SSID_3;
        wifi["password_3"] = password_3;
        wifi["SSID_4"] = SSID_4;
        wifi["password_4"] = password_4;
        wifi["SSID_5"] = SSID_5;
        wifi["password_5"] = password_5;
        if (serializeJson(wifi, write_file) == 0)
        {
            logln("Failed to write to Wifi file");
        }
        else
        {
            logln("Successfully wrote to Wifi file");
        }
        write_file.close();
    }
}

bool CheckSetUpStatus()
{
    logln("Checking SSIDs");
    if (SSID_1.length() > 0)
    {
        return true;
    }
    else if (SSID_2.length() > 0)
    {
        return true;
    }
    else if (SSID_3.length() > 0)
    {
        return true;
    }
    else if (SSID_4.length() > 0)
    {
        return true;
    }
    else if (SSID_5.length() > 0)
    {
        return true;
    }
    return false;
}

void getWifiIndex(bool read)
{
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
        err = nvs_get_u8(my_handle, "wifi_entry_index", &dummy);
        switch (err)
        {
        case ESP_OK:
            wifi_entry_index = dummy;
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            nvs_set_u8(my_handle, "wifi_entry_index", wifi_entry_index);
            break;
        default:
            break;
        }
    }

    // Write
    else
    {
        nvs_set_u8(my_handle, "wifi_entry_index", wifi_entry_index);
    }

    err = nvs_commit(my_handle);
    nvs_close(my_handle);
}
/* @brief Set the Wifi Configuration object this is the call back which is
 * called form the BLE.
 *
 * @param status
 * @param password
 * @param ssid
 */

void setWifiConfiguration(bool status, std::string password, std::string ssid)
{
    logln("setWifiConfiguration");
    String ssid_string = String(ssid.c_str());
    String pass_string = String(password.c_str());

    getWifiIndex(true);

    if (!status)
    {
        return;
    }
    else
    {
        if (wifi_entry_index == 1)
        {
            SSID_1 = ssid_string;
            password_1 = pass_string;
        }
        else if (wifi_entry_index == 2)
        {
            SSID_2 = ssid_string;
            password_2 = pass_string;
        }
        else if (wifi_entry_index == 3)
        {
            SSID_3 = ssid_string;
            password_3 = pass_string;
        }
        else if (wifi_entry_index == 4)
        {
            SSID_4 = ssid_string;
            password_4 = pass_string;
        }
        else if (wifi_entry_index == 5)
        {
            SSID_5 = ssid_string;
            password_5 = pass_string;
        }
        else
        {
            logln("All 5 wifi spots are full");
        }

        /*here going to control the */

        if (wifi_entry_index == 5)
        {
            wifi_entry_index = 1;
        }
        else
        {
            wifi_entry_index++;
        }

        getWifiIndex(false);

        File write_file = SPIFFS.open("/wifi.json", "w");
        DynamicJsonDocument wifi(1024);
        wifi["SSID_1"] = SSID_1;
        wifi["password_1"] = password_1;
        wifi["SSID_2"] = SSID_2;
        wifi["password_2"] = password_2;
        wifi["SSID_3"] = SSID_3;
        wifi["password_3"] = password_3;
        wifi["SSID_4"] = SSID_4;
        wifi["password_4"] = password_4;
        wifi["SSID_5"] = SSID_5;
        wifi["password_5"] = password_5;
        if (serializeJson(wifi, write_file) == 0)
        {
            logln("Failed to write to Wifi file");
        }
        else
        {
            logln("Successfully wrote to Wifi file");
        }
        write_file.close();
        return;
    }
}
/**
 * @brief This function is used to Setup the Wifi with the Desired Count on
 * each SIID and PSWD
 *
 */
// char * ss="PTCL-BB"
bool SetUpWifiFromList()
{
    // wifi_try_count++;
    logln("WIFI MANAGER : GOING TO SET THE WIFI BY TRANSVERING the List");

    WiFi.mode(WIFI_STA);

    for (int i = 1; i < 6; i++)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            String ssid;
            String password;

            switch (i)
            {
            case 1:
                ssid = SSID_1;
                password = password_1;
                break;
            case 2:
                ssid = SSID_2;
                password = password_2;
                break;
            case 3:
                ssid = SSID_3;
                password = password_3;
                break;
            case 4:
                ssid = SSID_4;
                password = password_4;
                break;
            case 5:
                ssid = SSID_5;
                password = password_5;
                break;
            }

            log("Going to Connect with:");
            logln(ssid);

            if (ssid.length() > 0 && password.length() > 0)
            {
                WiFi.begin(ssid.c_str(), password.c_str());
                //   Serial.println("HEAP 25  " + String(ESP.getFreeHeap()));
                int count = 0;
                while (WiFi.status() != WL_CONNECTED && count < 15)
                {
                    Serial.print(".");
                    delay(300);
                    count++;
                }
                if (WiFi.status() != WL_CONNECTED)
                {
                    Serial.println("WiFi " + String(i) + "not connected");
                }
                else
                {
                    Serial.println("Wifi " + String(i) + ssid + " connected");
                    configTime(0, 0, ntp_primary, ntp_secondary); // necessary to connect to the librar

                    return 1;
                }
            }
            else
            {
                Serial.println("Missing SSID or PASSWORD");
            }
        }
        else
        { // meaning it is already connected
            return 1;
        }
    }
    if (WiFi.status() != WL_CONNECTED)
    {
        return 0;
    }
    return 1;
}

void WifiManagerInit(void *param)
{
    readWifiFromFile();
}
bool TryConnection(void *param)
{
    while (1)
    {

        if (SetUpWifiFromList())
        {
            logln("******** SYSTEM WIFI IS NOW CONNECTED *******");
            while (1)
            {
                if (wifi_audio && !imp_music)
                {
                    ButtonPress_t rec_button = kconnect;
                    // xQueueSendToBack(xQueueAudioPlay, &(rec_button), 0);
                    wifi_audio = false;
                    break;
                }
                vTaskDelay(1000 / portTICK_PERIOD_MS);
            }
            break;
        }
        else
        {
            logln("************  Trying to connect wifi  **************");
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }

    return 1;
}
