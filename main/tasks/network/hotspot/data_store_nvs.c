// #include <string.h>
// #include <stdio.h>
// #include "esp_system.h"
// #include "nvs_flash.h"
// #include "nvs.h"
// #include <esp_log.h>
// #include "data_store_nvs.h"

// static const char *TAG = "Data Store NVS";

// char ssid[30] = {0}, pswd[30] = {0};
// char user_name[30] = {0}, user_pswd[30] = {0}, user_device_num[5] = {0};

// /**
//  * @brief ReadStringNVS() is local function that reads the specified string key and returns the string value of the key. Always call free()
//  * after calling this function or memory leak will occur
//  *
//  * @param char key
//  * @return char*
//  */
// static char *ReadStringNVS(char *key)
// {
//     esp_err_t err = nvs_flash_init();
//     nvs_handle_t my_handle;

//     err = nvs_open("storage", NVS_READWRITE, &my_handle);

//     if (err != ESP_OK)
//     {
//         ESP_LOGE(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     }
//     else
//     {
//         ESP_LOGD(TAG, "Done\n");
//         ESP_LOGD(TAG, "Reading SSID from NVS ... ");

//         size_t len;

//         nvs_get_str(my_handle, key, NULL, &len);

//         char *value = malloc(len);

//         err = nvs_get_str(my_handle, key, value, &len);

//         switch (err)
//         {
//         case ESP_OK:
//             ESP_LOGD(TAG, "Done\n");
//             return value;
//             break;
//         case ESP_ERR_NVS_NOT_FOUND:
//             ESP_LOGD(TAG, "The value is not initialized yet!\n");
//             break;
//         default:
//             ESP_LOGD(TAG, "Error (%s) reading!\n", esp_err_to_name(err));
//         }

//         nvs_close(my_handle);
//     }
//     return NULL;
// }
// void WriteStrNVS(char *key, char *value)
// {
//     esp_err_t err = nvs_flash_init();

//     ESP_LOGD(TAG, "\n");
//     ESP_LOGD(TAG, "Opening Non-Volatile Storage (NVS) handle... ");

//     nvs_handle_t my_handle;

//     err = nvs_open("storage", NVS_READWRITE, &my_handle);

//     if (err != ESP_OK)
//     {
//         ESP_LOGD(TAG, "Error (%s) opening NVS handle!\n", esp_err_to_name(err));
//     }
//     else
//     {
//         ESP_LOGD(TAG, "Done\n");
//         ESP_LOGD(TAG, "Storing key (%s) and value (%s) in NVS ... ", key, value);
//         // Write
//         err = nvs_set_str(my_handle, key, value);
//         // Commit written value.
//         nvs_close(my_handle);
//     }

//     ESP_LOGD(TAG, "\n");
// }
// void GetWifiCred()
// {
//     char *ssid_buff = ReadStringNVS("ssid");
//     char *pswd_buff = ReadStringNVS("pswd");

//     if (ssid_buff != NULL && pswd_buff != NULL)
//     {
//         sprintf(ssid, "%s", ssid_buff);
//         sprintf(pswd, "%s", pswd_buff);
//     }
//     else
//     {
//         memset(ssid, '0', 1);
//         memset(pswd, '0', 1);
//     }

//     ESP_LOGD("data_nvs", "ssid %s", ssid);
//     ESP_LOGD("data_nvs", "pswd %s", pswd);

//     free(ssid_buff);
//     free(pswd_buff);
// }
// void GetParseCred()
// {
//     char *user_buff = ReadStringNVS("userID");
//     char *pswd_buff = ReadStringNVS("userPassword");
//     char *device_buff = ReadStringNVS("deviceNum");

//     if (user_buff != NULL && pswd_buff != NULL && device_buff != NULL)
//     {
//         sprintf(user_name, "%s", user_buff);
//         sprintf(user_pswd, "%s", pswd_buff);
//         sprintf(user_device_num, "%s", device_buff);

//         ESP_LOGD("data_nvs", "userID %s", user_buff);
//         ESP_LOGD("data_nvs", "userPswd %s", pswd_buff);
//         ESP_LOGD("data_nvs", "deviceNum %s", device_buff);
//     }
//     else
//     {
//         memset(user_name, '0', 1);
//         memset(user_pswd, '0', 1);
//         memset(user_device_num, '0', 1);
//     }

//     ESP_LOGD("data_nvs", "user_name %s", user_name);
//     ESP_LOGD("data_nvs", "user_pswd %s", user_pswd);
//     ESP_LOGD("data_nvs", "user_device_num %s", user_device_num);

//     free(user_buff);
//     free(pswd_buff);
//     free(device_buff);
// }

// void NVSInit()
// {
//     // Initialize NVS
//     esp_err_t ret = nvs_flash_init();
//     if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
//     {
//         ESP_ERROR_CHECK(nvs_flash_erase());
//         ret = nvs_flash_init();
//     }

//     ESP_ERROR_CHECK(ret);
// }