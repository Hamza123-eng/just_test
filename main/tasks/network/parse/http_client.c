#include <string.h>
#include <stdlib.h>
#include "stdio.h"

#include "esp_log.h"
// #include "esp_system.h"
// #include "nvs_flash.h"
// #include "esp_event.h"
// #include "esp_netif.h"
// #include "esp_tls.h"
#include "esp_http_client.h"

// #include "post.h"
// #include "http_event_handler_post.h"
// #include "get.h"
// #include "cJSON.h"
#include "parse.h"
#include "http_client.h"
#include "freertos_handler.h"
// #include "wifi_manger.h"

static const char *TAG = "HTTP []";

SemaphoreHandle_t xNetworkMutex = NULL;

void InitNetworkLock(void *param)
{
    xNetworkMutex = xSemaphoreCreateBinary();
    if (xNetworkMutex == NULL)
        ESP_LOGE(TAG, "MUTEX CREATE FAIL");
}
void GetNetworkLock(void *param)
{
    while (1)
    {
        if (xSemaphoreTake(xNetworkMutex, (TickType_t)10) == pdTRUE)
            break;
        vTaskDelay(100);
    }
}
void ReturnNetworkLock(void *param)
{
    while (1)
    {
        if (xSemaphoreGive(xNetworkMutex) == pdTRUE)
            break;
        else
        {
            vTaskDelay(30);
        }
    }
}

char *HttpPerform(char *url, http_request_type_t request_type, uint8_t *status_code, char *data_buffer, http_header_t *header)
{
    uint16_t status_http = 0;
    uint16_t content_length = 0;
    uint8_t http_tries_max = 2;

    esp_err_t err = ESP_FAIL;

    char *get_buffer = NULL;
    esp_http_client_handle_t http_client; // its being used in the get post and put method files

    esp_http_client_config_t config = {.url = "http://www.goggle.com"};

    http_client = esp_http_client_init(&config);

    esp_http_client_set_url(http_client, url);

    /* only for post request*/

    /*< deciding which request system have to perform >*/
    esp_http_client_set_header(http_client, "Content-Type", "application/json");

    if (header != NULL)
    {
        if (strcmp(header->key_1, ""))
            esp_http_client_set_header(http_client, header->key_1, header->value_1);
        if (strcmp(header->key_2, ""))
            esp_http_client_set_header(http_client, header->key_2, header->value_2);
    }

    while (http_tries_max)
    {
        switch (request_type)
        {
        case GET:
            esp_http_client_set_method(http_client, HTTP_METHOD_GET);
            err = esp_http_client_open(http_client, 0);
            break;
        case POST:
            esp_http_client_set_method(http_client, HTTP_METHOD_POST);
            err = esp_http_client_open(http_client, strlen(data_buffer));
            break;
        case PUT:
            esp_http_client_set_method(http_client, HTTP_METHOD_PUT);
            err = esp_http_client_open(http_client, strlen(data_buffer));
            break;
        case DELETE:
            esp_http_client_set_method(http_client, HTTP_METHOD_DELETE);
            err = esp_http_client_open(http_client, 0);
            break;
        case HEAD:
            esp_http_client_set_method(http_client, HTTP_METHOD_HEAD);
            err = esp_http_client_open(http_client, 0);
            break;

        default:
            break;
        }

        /* main data desicion here */

        if (err == ESP_OK)
        {
            /*Only for GET CASE*/
            if (request_type == GET || request_type == HEAD)
            {
                content_length = esp_http_client_fetch_headers(http_client);

                if (content_length < 0)
                {
                    ESP_LOGE(TAG, "HTTP http_client fetch headers failed");
                    *status_code = 0;
                }
                else if (request_type == GET)
                {
                    get_buffer = (char *)malloc(content_length + 1);
                    if (get_buffer != NULL)
                    {
                        memset(get_buffer, 0x00, (content_length + 1));
                        int data_read = esp_http_client_read_response(http_client, get_buffer, (content_length));
                        if (data_read >= 0)
                        {
                            // Only here when all sucess is there
                            status_http = esp_http_client_get_status_code(http_client);
                        }
                        else
                        {
                            ESP_LOGE(TAG, "Failed to read response");
                            status_http = 0;
                        }
                    }
                }
                else if (request_type == HEAD)
                {
                    status_http= esp_http_client_get_status_code(http_client);
                }
                else
                {
                    status_http = 0;
                }
            }

            /*POST . DELETE . PUT*/
            else
            {

                int wlen = 0;
                (request_type != DELETE) ? wlen = esp_http_client_write(http_client, data_buffer, strlen(data_buffer))
                                         : 1;
                if (wlen < 0)
                {
                    ESP_LOGE(TAG, "Write failed");
                    status_http = 0;
                }
                else
                {
                    content_length = esp_http_client_fetch_headers(http_client);
                    if (content_length < 0)
                    {
                        ESP_LOGE(TAG, "HTTP http_client fetch headers failed");
                        status_http = 0;
                    }
                    else
                    {
                        // Only here when all sucess is there
                        status_http= esp_http_client_get_status_code(http_client);
                    }
                }
            }
            /*Sucess respose*/
            if (status_http == 200 || status_http == 201)
            {
                *status_code = 200;
                break;
            }
            else if (status_http == 404)
            {
                /* Vage logic only have uint8_t to handle the 404 error*/
                *status_code = 100;
                break;
            }
            else
            {
                *status_code = 0;
                http_tries_max--;
               /** memeory Leak assumpution on faliure */
                if (get_buffer != NULL)
                {
                    free(get_buffer);
                    get_buffer = NULL;
                }
            }
        }

        else
        {
            ESP_LOGE(TAG, "HTTP client open  failed: %s", esp_err_to_name(err));
            /* < only for the scenioro when http_client is not able to open > */
            *status_code = 0;
            http_tries_max--;
        }
    }
    /*If open is fail*/
    esp_http_client_close(http_client);
    esp_http_client_cleanup(http_client);

    // printf("%s",(char *)get_buffer);

    return (get_buffer != NULL) ? get_buffer : NULL;
}
