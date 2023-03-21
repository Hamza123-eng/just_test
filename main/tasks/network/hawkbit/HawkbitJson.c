#include "stdbool.h"
#include "cJSON.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#include "time.h"
#include "esp_log.h"
#include "esp_system.h"
#include "os.h"
#include "base64.h"
#include "Hawkbit.h"
#include "http_client.h"
#include "hw_monitor.h"
#include "wifi_manger.h"

static const char *TAG = "HAWKBIT_JSON";

char hawkbit_sleep_str[32];
int GetSleepTime();
int CheckDeploymentAvailable();
void GetBaseForCancelAction(void *param);
char cancel_action_id[8];
char cancel_action_url[360] = {0x00};
/**
 *
 */
void CancelActionFeedBack(void *param)
{
    char *feedback_msg[] = {"feedback message"};
    cJSON *feed_back;
    cJSON *status;
    cJSON *progress;
    cJSON *result;
    char *ptr;
    feed_back = cJSON_CreateObject();

    cJSON *stringArray = cJSON_CreateStringArray(feedback_msg, 1);
    cJSON_AddStringToObject(feed_back, "id", cancel_action_id);
    cJSON_AddItemToObject(feed_back, "status", status = cJSON_CreateObject());
    cJSON_AddItemToObject(status, "result", result = cJSON_CreateObject());
    cJSON_AddItemToObject(result, "progress", progress = cJSON_CreateObject());
    cJSON_AddNumberToObject(progress, "of", 1);
    cJSON_AddNumberToObject(progress, "cnt", 1);
    cJSON_AddStringToObject(result, "finished", "success");
    cJSON_AddStringToObject(status, "execution", "closed");
    cJSON_AddItemToObject(status, "details", stringArray);
    ptr = cJSON_Print(feed_back);
    sprintf(param, "%s", ptr);
    free(ptr);

    cJSON_Delete(feed_back);
}
/**
 *
 */

void CancelAction(void *param)
{
    GetBaseForCancelAction(NULL);
}
/**
 * @brief this function is used to poll the HAWKBIT.
 *
 * @return true
 * @return false
 */
uint8_t HawkbitPolling(void *param)
{
    uint8_t action_required = 0;
    char *data;
    uint8_t status_code = 0;

    while (1)
    {
        if (IsNetworkAvailable(NULL))
        {
            GetNetworkLock(NULL);
            data = HttpPerform(hawkbit.hawkbit_url, GET, &status_code, NULL, &hawkbit_header);
            ReturnNetworkLock(NULL);

            if (status_code == 200)
            {
                sprintf(hawkbit.http_output_buffer, "%s", data);
                data != NULL ? free(data) : 1;
                action_required = CheckDeploymentAvailable();
            }
            else
            {
                data != NULL ? free(data) : 1;
            }
            return action_required;
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
/**
 * @brief This function is use to check either deployment is available or not.
 *
 * @return true
 * @return false
 */

int CheckDeploymentAvailable()
{
    char dummy;

    // START:

    int action_required = 0;

    cJSON *root = cJSON_Parse(hawkbit.http_output_buffer);
    cJSON *config = cJSON_GetObjectItem(root, "config");
    cJSON *polling = cJSON_GetObjectItem(config, "polling");

    cJSON *ptr = cJSON_GetObjectItem(polling, "sleep");
    if (ptr == NULL)
    {
        action_required = 2;
        cJSON_Delete(root);
        return action_required;
    }

    //  ptr = cJSON_GetObjectItem(polling, "sleep")->valuestring;
    strcpy(hawkbit_sleep_str, ptr->valuestring);

    cJSON *_links = cJSON_GetObjectItem(root, "_links");
    cJSON *deployment_base = cJSON_GetObjectItem(_links, "deploymentBase");
    cJSON *cancel_action = cJSON_GetObjectItem(_links, "cancelAction");

    if (deployment_base != NULL)
    {
        ESP_LOGI(TAG, "HAWKBIT DEPLOYMENT IS AVAILABELE GOING FOR NEXT CHECK");
        action_required = 1;
    }
    else
    {
        if (cancel_action != NULL)
        {
            CancelAction(NULL);
            HawkbitPolling(NULL);
            // goto START;
        }
        ESP_LOGI(TAG, "HAWKBIT DEPLOYMENT IS NOT AVAILABLE");
        action_required = 0;
    }

    cJSON_Delete(root);

    return action_required;
}
/**
 * @brief
 *
 * @param param
 */
void ExtractCancelId(char *param)
{
    memcpy(cancel_action_url, param, strlen(param));

    uint16_t index = strlen(cancel_action_url);
    uint8_t id_index = 0;

    while (cancel_action_url[index] != '/')
    {
        index--;
    }
    memset(cancel_action_id, 0x00, 8);
    // sprintf(cancel_action_id,"%s",&cancel_action_url[index + 1]);
    memcpy(cancel_action_id, (char *)&cancel_action_url[index + 1], strlen(cancel_action_url));
    memset(cancel_action_url, 0x00, 360);

    /*append url*/
    sprintf(cancel_action_url, "%s%s", param, "/feedback");
}
/**
 * @brief
 *
 */
void GetBaseForCancelAction(void *param)
{
    char data_buffer[1024] = {0x00};
    char *href;
    char *data = "uyutyu";
    uint8_t status_code = 0;

    cJSON *root = cJSON_Parse(hawkbit.http_output_buffer);
    cJSON *_links = cJSON_GetObjectItem(root, "_links");
    cJSON *deployment_base = cJSON_GetObjectItem(_links, "cancelAction");
    href = cJSON_GetObjectItem(deployment_base, "href")->valuestring;
    ExtractCancelId(href);
    CancelActionFeedBack(data_buffer);
    // while (1)
    // {
    //     if (IsNetworkAvailable(NULL))
    //         break;

    //     vTaskDelay(3000 / portTICK_PERIOD_MS);
    // }

    // free(data);

    cJSON_Delete(root);

    GetNetworkLock(NULL);

    HttpPerform(cancel_action_url, POST, &status_code, data_buffer, &hawkbit_header);
    // sprintf(hawkbit.http_output_buffer, "%s", data);

    ReturnNetworkLock(NULL);
}

/**
 * @brief this Functioin is use to bring artifact link.
 *
 */
void GetBaseDeployment()
{
    char *href;
    char *data = "uyutyu";
    uint8_t status_code = 0;

    cJSON *root = cJSON_Parse(hawkbit.http_output_buffer);
    cJSON *_links = cJSON_GetObjectItem(root, "_links");
    cJSON *deployment_base = cJSON_GetObjectItem(_links, "deploymentBase");
    href = cJSON_GetObjectItem(deployment_base, "href")->valuestring;

    while (1)
    {
        if (IsNetworkAvailable(NULL))
        {

            GetNetworkLock(NULL);
            data = HttpPerform(href, GET, &status_code, NULL, &hawkbit_header);
            ReturnNetworkLock(NULL);

            if (status_code == 200)
            {
                sprintf(hawkbit.http_output_buffer, "%s", data);
                data != NULL ? free(data) : 1;
                break;
            }
            else
            {
                data != NULL ? free(data) : 1;
            }
        }
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    cJSON_Delete(root);
}
/**
 * @brief This function is used to get base Deployment.
 *
 */
void GetDeploymentLink()
{
    char *ptr;

    cJSON *root = cJSON_Parse(hawkbit.http_output_buffer);
    char *feed_id = cJSON_GetObjectItem(root, "id")->valuestring;
    strcpy(hawkbit.deployment_id, feed_id);
    cJSON *deployment = cJSON_GetObjectItem(root, "deployment");
    cJSON *Chunk_array = cJSON_GetObjectItem(deployment, "chunks");
    cJSON *Chunk = cJSON_GetArrayItem(Chunk_array, 0);
    cJSON *Artifacts_array = cJSON_GetObjectItem(Chunk, "artifacts");
    cJSON *Artifacts = cJSON_GetArrayItem(Artifacts_array, 0);
    cJSON *lin_obj = cJSON_GetObjectItem(Artifacts, "_links");
    cJSON *download_http_obj = cJSON_GetObjectItem(lin_obj, "download-http");
    ptr = cJSON_GetObjectItem(download_http_obj, "href")->valuestring;
    strcpy(hawkbit.artifact_link, ptr);
    ESP_LOGI(TAG, "ARTIFACT_LINK:%s", hawkbit.artifact_link);

    cJSON_Delete(root);
}

bool GetSecurityToken()
{
    if (hawkbit.http_output_buffer > 0)
    {

        cJSON *root = cJSON_Parse(hawkbit.http_output_buffer);
        cJSON *ptr = cJSON_GetObjectItem(root, "securityToken");
        if (ptr == NULL)
        {
            cJSON_Delete(root);
            return 0;
        }
        else
        {
            // char *test = cJSON_GetObjectItem(root, "securityToken")->valuestring;
            strcpy(hawkbit.security_token, ptr->valuestring);
            cJSON_Delete(root);
            return 1;
        }
        cJSON_Delete(root);
        return 0;
    }
}
/**
 * @brief
 *
 */
void LogInHawkbit()
{
    char temp[100];
    char *ptr;
    sprintf(hawkbit.hawkbit_url, "%s%s%s%s%s",
            "http://", CONFIG_HAWKBIT_SERVER_PATH,
            "/rest/v1/", "targets/",
            device_config.model_data);

    sprintf(temp, "%s%s%s", HAWKBIT_USER, ":", HAWKBIT_PASSCODE);
    ptr = base64_encode(temp, strlen(temp), NULL);
    ESP_LOGD(TAG, "BASE64 ENCODE %s:", ptr);
    sprintf(hawkbit.hawkbit_authentication, "%s%s", "Basic ", ptr);
    ESP_LOGD(TAG, "%s:", hawkbit.hawkbit_authentication);

    sprintf(hawkbit_header.value_1, "%s", hawkbit.hawkbit_authentication);
}
/**
 * @brief This function is used to get feed back object for Hawkbit.
 *
 */
void FeedBackObj(char *http_input_buffer)
{
    char *feedback_msg[] = {"feedback message"};
    cJSON *feed_back;
    cJSON *status;
    cJSON *progress;
    cJSON *result;
    char *ptr;
    feed_back = cJSON_CreateObject();

    cJSON *stringArray = cJSON_CreateStringArray(feedback_msg, 1);
    cJSON_AddStringToObject(feed_back, "id", hawkbit.deployment_id);
    cJSON_AddItemToObject(feed_back, "status", status = cJSON_CreateObject());
    cJSON_AddItemToObject(status, "result", result = cJSON_CreateObject());
    cJSON_AddItemToObject(result, "progress", progress = cJSON_CreateObject());
    cJSON_AddNumberToObject(progress, "of", 1);
    cJSON_AddNumberToObject(progress, "cnt", 1);
    cJSON_AddStringToObject(result, "finished", "success");
    cJSON_AddStringToObject(status, "execution", "closed");
    cJSON_AddItemToObject(status, "details", stringArray);
    ptr = cJSON_Print(feed_back);
    sprintf(http_input_buffer, "%s", ptr);
    free(ptr);

    cJSON_Delete(feed_back);
}
/**
 * @brief This function is used to get feedbackurl.
 *
 */
void FeedBackUrl()
{
    memset(hawkbit.hawkbit_feed_Back_url, 0, HAWKBIT_FEED_BACK_URL_SIZE);
    sprintf(hawkbit.hawkbit_feed_Back_url, "%s%s%s%s%s%s%s%s%s%s",
            "http://", CONFIG_HAWKBIT_SERVER_PATH,
            "/", CONFIG_HAWKBIT_TENANTS, "/controller/v1/",
            device_config.model_data, "/deploymentBase/",
            hawkbit.deployment_id, "/", "feedback");
    ESP_LOGI(TAG, "FEED_BACK_LINK:%s", hawkbit.hawkbit_feed_Back_url);
}
/**
 * @brief
 *
 */
void SendFeedBack()
{
    uint8_t status_code = 0;
    char feedback_buffer[1500];

    FeedBackObj(&feedback_buffer);
    FeedBackUrl();
    // GetNetworkLock(NULL);
    while (1)
    {
        ESP_LOGI(TAG, "CREATED JSON PACKECTE:%s", feedback_buffer);
        HttpPerform(hawkbit.hawkbit_feed_Back_url, POST, &status_code, feedback_buffer, &hawkbit_header);
        if (status_code != 200)
        {
            ESP_LOGI(TAG, "FEED BACK IS NOT SEND TO HAWKBIT AGAIN TRYING");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else
        {
            ESP_LOGI(TAG, "FEED TO HAWKBIT IS SUCESSFULLY SEND");
            break;
        }
    }
    ReturnNetworkLock(NULL);
}
/**
 * @brief  This function is used to convert time into UNIX.
 *
 * @param sleep
 * @return int
 */

/**
 * @brief Thsi function is used to get skeep time
 *
 */
int GetSleepTime()
{
    // GetStandardTime();
    // char ptr[50];

    // sprintf(ptr, "%s%s%s", "1/1/20", " ",
    //         (char *)hawkbit_sleep_str);
    // ESP_LOGI(TAG, "UNIX STRING %s  ", ptr);
    // return (int)ConvertSleepTime(ptr);
    return 10000;
}
