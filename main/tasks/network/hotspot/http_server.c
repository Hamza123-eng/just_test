#include <esp_wifi.h>
#include <esp_event.h>
#include <esp_log.h>
#include <esp_system.h>
#include <sys/param.h>
#include <esp_http_server.h>

#include "esp_netif.h"

#include "data_store_nvs.h"
#include "http_server.h"
#include "cJSON.h"
#include "scan.h"
#include "parse.h"
#include "indications.h"
#include "freertos_handler.h"
#include "hw_monitor.h"

#include "nvs_storage.h"

httpd_handle_t server_org = NULL;

static const char *TAG = "HTTP server";
char scan_list[500] = {0};

bool parse_account_received = false;
bool wifi_cren_received = false;

/* An HTTP GET handler */
static esp_err_t wifi_scan_get_handler(httpd_req_t *req)
{
    WiFiScan(scan_list);

    const char *resp_str = (const char *)scan_list;

    httpd_resp_send(req, resp_str, HTTPD_RESP_USE_STRLEN);

    return ESP_OK;
}

static const httpd_uri_t wifiscan = {
    .uri = "/api/wifiscan",
    .method = HTTP_GET,
    .handler = wifi_scan_get_handler,
    .user_ctx = NULL};

/* An HTTP POST handler */
static esp_err_t wifi_cred_post_handler(httpd_req_t *req)
{
    char buf[100];
    int ret, remaining = req->content_len;

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                                  MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        remaining -= ret;

        /* Log data received */
        ESP_LOGD(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGD(TAG, "====================================");

        cJSON *recv_json = cJSON_Parse(buf);
        cJSON *ssid_buff = cJSON_GetObjectItemCaseSensitive(recv_json, "ssid");
        cJSON *pswd_buff = cJSON_GetObjectItemCaseSensitive(recv_json, "pswd");

        if (cJSON_IsString(ssid_buff) && (ssid_buff->valuestring != NULL) && (cJSON_IsString(pswd_buff) && (pswd_buff->valuestring != NULL)))
        {
            ESP_LOGD(TAG, "Checking ssid \"%s\"\n", ssid_buff->valuestring);
            ESP_LOGD(TAG, "Checking password \"%s\"\n", pswd_buff->valuestring);

            sprintf(network_config.ssid1, "%s", ssid_buff->valuestring);
            sprintf(network_config.pswd1, "%s", pswd_buff->valuestring);

            char *basic_auth_resp = "{ \"RESULT\" : \"SUCCESS\" }";
            httpd_resp_set_status(req, HTTPD_200);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");

            if (httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp)) == ESP_OK)
            {
                wifi_cren_received = true;
                if (parse_account_received)
                {
                    PerformNvsNetworkParamter(1);
                    buzzer_beep_t event = ACCOUNT_RECEIVED;
                    xQueueSendToBack(xBuzzerQueue, &(event), (TickType_t)100);
                }
            }
        }
        else
        {
            /* Send back the same data */
            httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Wrong key value pairs");
        }

        cJSON_Delete(recv_json);
    }
    return ESP_OK;
}

static const httpd_uri_t wificredential = {
    .uri = "/api/wificredential",
    .method = HTTP_POST,
    .handler = wifi_cred_post_handler,
    .user_ctx = NULL};

/* An HTTP POST handler */
static esp_err_t parse_cred_post_handler(httpd_req_t *req)
{
    char buf[128] = {0x00};
    int ret, remaining = req->content_len;
    bool restart_flag = false;

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                                  MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
       // httpd_resp_send_chunk(req, buf, ret);

        remaining -= ret;

        /* Log data received */
        ESP_LOGD(TAG, "=========== RECEIVED DATA ==========");
        ESP_LOGI(TAG, "%.*s", ret, buf);
        ESP_LOGD(TAG, "====================================");

        cJSON *recv_json = cJSON_Parse(buf);
        const cJSON *user_buff = cJSON_GetObjectItemCaseSensitive(recv_json, "userID");
        const cJSON *pswd_buff = cJSON_GetObjectItemCaseSensitive(recv_json, "userPassword");
        const cJSON *device_buff = cJSON_GetObjectItemCaseSensitive(recv_json, "deviceNum");

        if (cJSON_IsString(user_buff) && (user_buff->valuestring != NULL) && (cJSON_IsString(pswd_buff) && (pswd_buff->valuestring != NULL)) && (cJSON_IsString(device_buff) && (device_buff->valuestring != NULL)))
        {
            ESP_LOGD(TAG, "Checking userID \"%s\"\n", user_buff->valuestring);
            ESP_LOGI(TAG, "Checking userPassword \"%s\"\n", pswd_buff->valuestring);
            ESP_LOGD(TAG, "Checking deviceNum \"%s\"\n", device_buff->valuestring);
            /* if New User found please go with factory reset*/
            if (strcmp(network_config.user_name, user_buff->valuestring) ||
                strcmp(network_config.user_pass, pswd_buff->valuestring) ||
                strcmp(network_config.device_num, device_buff->valuestring))
            {
                PerformNvsFactoryReset(NULL);
                sprintf(network_config.user_name, "%s", user_buff->valuestring);
                sprintf(network_config.user_pass, "%s", pswd_buff->valuestring);
                sprintf(network_config.device_num, "%s", (device_buff->valuestring));
            }
            char *basic_auth_resp = "{ \"RESULT\" : \"SUCCESS\" }";
            httpd_resp_set_status(req, HTTPD_200);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");

            if (httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp)) == ESP_OK)
            {
                // esp_restart();
                parse_account_received = true;
                if (wifi_cren_received)
                {
                    PerformNvsNetworkParamter(1);
                    buzzer_beep_t event = ACCOUNT_RECEIVED;
                    xQueueSendToBack(xBuzzerQueue, &(event), (TickType_t)100);
                }
            }
        }
        else
        {
            /* Send back the same data */
            httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Wrong key value pairs");
        }
        cJSON_Delete(recv_json);
    }

    return ESP_OK;
}

/**
 * @brief
 *
 */
static const httpd_uri_t parsecredential = {
    .uri = "/api/parsecredential",
    .method = HTTP_POST,
    .handler = parse_cred_post_handler,
    .user_ctx = NULL};

static esp_err_t set_hardware_detail(httpd_req_t *req)
{
    char buf[128] = {0x00};
    int ret, remaining = req->content_len;

    while (remaining > 0)
    {
        /* Read the data for the request */
        if ((ret = httpd_req_recv(req, buf,
                                  MIN(remaining, sizeof(buf)))) <= 0)
        {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT)
            {
                /* Retry receiving if timeout occurred */
                continue;
            }
            return ESP_FAIL;
        }

        /* Send back the same data */
        // httpd_resp_send_chunk(req, buf, ret);

        remaining -= ret;
        char auth_buff[32];

        if (httpd_req_get_hdr_value_str(req, "Authorization", auth_buff, 32) == ESP_OK)
        {
            ESP_LOGI(TAG, "Found header => Authorzation: %s", auth_buff);
            if (!strcmp(auth_buff, "confidential"))
            {
                /* Log data received */
                ESP_LOGD(TAG, "=========== RECEIVED DATA HARDWARE ==========");
                ESP_LOGD(TAG, "%.*s", ret, buf);
                ESP_LOGD(TAG, "====================================");

                cJSON *recv_json = cJSON_Parse(buf);
                const cJSON *revision_no = cJSON_GetObjectItemCaseSensitive(recv_json, "revision_no");
                int serial_no = cJSON_GetObjectItem(recv_json, "serial_no")->valueint;

                if ((revision_no->valuestring != NULL))
                {
                    // printf("Going to save the int eh NVS HARDWARE Detail\n");
                    /* if New User found please go with factory reset*/

                    sprintf(device_config.hardware_version, "%s", revision_no->valuestring);
                    device_config.hardware_serial = serial_no;
                    PerformNvsHardwareDetail(true);
                    /* sending the status */
                    char *basic_auth_resp = "{ Data Submitted Sucessfully }";
                    httpd_resp_set_status(req, HTTPD_200);
                    httpd_resp_set_type(req, "application/json");
                    httpd_resp_set_hdr(req, "Connection", "keep-alive");
                    // asprintf(&basic_auth_resp, "{confidential Submitted sucessfully}", basic_auth_info->username);

                    httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp));
                    // free(basic_auth_resp);
                }
                else
                {
                    /* Send back the same data */
                    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not authentated");
                }
                cJSON_Delete(recv_json);
            }
        }
        else
        {
            char *basic_auth_resp = "{Not authenticated}";
            httpd_resp_set_status(req, HTTPD_404);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            // asprintf(&basic_auth_resp, "{Not authenticated}", basic_auth_info->username);

            httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp));
            // free(basic_auth_resp);
        }
    }
    return ESP_OK;
}
/**
 * @brief
 *
 */
static const httpd_uri_t sethardwaredetail = {
    .uri = "/api/sethardwaredetail",
    .method = HTTP_POST,
    .handler = set_hardware_detail,
    .user_ctx = NULL};

static esp_err_t get_hardware_detail(httpd_req_t *req)
{

    char auth_buff[32];

    if (httpd_req_get_hdr_value_str(req, "Authorization", auth_buff, 32) == ESP_OK)
    {
        //  ESP_LOGI(TAG, "Found header => Authorzation: %s", buf);
        if (!strcmp(auth_buff, "confidential"))
        {
            char basic_auth_resp[256] = {0x00};
            httpd_resp_set_status(req, HTTPD_200);
            httpd_resp_set_type(req, "application/json");
            httpd_resp_set_hdr(req, "Connection", "keep-alive");
            sprintf(basic_auth_resp, "{\nHardware Version : %s \n Serial Number : %hhu \n}", device_config.hardware_version, device_config.hardware_serial);

            httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp));
        }
        else
        {
            /* Send back the same data */
            httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Not authentated");
        }
    }
    else
    {
        char *basic_auth_resp = "{Not authenticated}";
        httpd_resp_set_status(req, HTTPD_404);
        httpd_resp_set_type(req, "application/json");
        httpd_resp_set_hdr(req, "Connection", "keep-alive");
        // asprintf(&basic_auth_resp, "{Not authenticated}", basic_auth_info->username);

        httpd_resp_send(req, basic_auth_resp, strlen(basic_auth_resp));
        // free(basic_auth_resp);
    }
    return ESP_OK;
}

/**
 * @brief
 *
 * @param req
 * @param err
 * @return esp_err_t
 */
static const httpd_uri_t gethardwaredetail = {
    .uri = "/api/gethardwaredetail",
    .method = HTTP_GET,
    .handler = get_hardware_detail,
    .user_ctx = NULL};

esp_err_t http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    /* For any other URI send 404 and close socket */
    httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "404 Error Not Found");
    return ESP_FAIL;
}

httpd_handle_t start_webserver(void)
{

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.lru_purge_enable = true;

    // Start the httpd server
    ESP_LOGD(TAG, "Starting server on port: '%d'", config.server_port);

    if (httpd_start(&server_org, &config) == ESP_OK)
    {
        // Set URI handlers
        ESP_LOGI(TAG, "Registering URI handlers");

        httpd_register_uri_handler(server_org, &wifiscan);
        httpd_register_uri_handler(server_org, &wificredential);
        httpd_register_uri_handler(server_org, &parsecredential);
        httpd_register_uri_handler(server_org, &gethardwaredetail);
        httpd_register_uri_handler(server_org, &sethardwaredetail);

        return server_org;
    }

    ESP_LOGD(TAG, "Error starting server!");
    return NULL;
}
static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}
void Shutdown_server_service(void *param)
{
    stop_webserver(server_org);
}

void disconnect_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;

    if (*server)
    {
        ESP_LOGD(TAG, "Stopping webserver");

        stop_webserver(*server);

        *server = NULL;
    }
}

void connect_handler(void *arg, esp_event_base_t event_base,
                     int32_t event_id, void *event_data)
{
    httpd_handle_t *server = (httpd_handle_t *)arg;

    if (*server == NULL)
    {
        ESP_LOGD(TAG, "Starting webserver");

        *server = start_webserver();
    }
}
void WebServerEventRegister()
{
    static httpd_handle_t server = NULL;

    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_AP_STAIPASSIGNED, &connect_handler, &server));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_CONNECTED, &disconnect_handler, &server));
}
