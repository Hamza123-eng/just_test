// #include "stdio.h"
// #include "esp_log.h"
// #include "string.h"
// #include <stdlib.h>
// #include "esp_http_client.h"
// #include "Hawkbit.h"
// #include "HawkbitHtpp.h"

// static char *TAG = "HAWKBIT_HTTP";
// char hawkbit_url_str[128];
// /**
//  * @brief This function is used to get Data from the Hawkbit server.
//  *
//  * @param URL
//  */
// bool HttpGet(char *URL, char *buffer, int buffer_size, char *authentication_token)
// {
//     bool flag = 0;
//     ESP_LOGI(TAG, "GET_URL:%s", URL);
//     memset(buffer, 0, buffer_size);
//     esp_err_t err = ESP_FAIL;

//     esp_http_client_config_t config = {
//         .url = URL,
//     };

//     esp_http_client_handle_t client = esp_http_client_init(&config);
// #ifdef HAWKBIT_AUTHENTICATION

//     esp_http_client_set_header(client, "Authorization", authentication_token);

// #endif

//     esp_http_client_set_method(client, HTTP_METHOD_GET);
//     while (err != ESP_OK)
//     {

//         err = esp_http_client_open(client, 0);
//         if (err != ESP_OK)
//         {
//             ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
//         }
//         else
//         {
//             int content_length;
//             content_length = esp_http_client_fetch_headers(client);
//             if (content_length < 0)
//             {
//                 ESP_LOGE(TAG, "HTTP client fetch headers failed");
//                 flag = 0;
//             }
//             else
//             {
//                 int data_read = esp_http_client_read_response(client, buffer, buffer_size);
//                 if (data_read > 0)
//                 {
//                     ESP_LOGD(TAG, "HTTP_DATA........%s", buffer);
//                     ESP_LOGD(TAG, "HTTP GET Status = %d, content_length = %lld",
//                              esp_http_client_get_status_code(client),
//                              esp_http_client_get_content_length(client));
//                     flag = 1;
//                     // ESP_LOG_BUFFER_HEX(TAG, output_buffer, data_read);
//                 }
//                 else
//                 {
//                     ESP_LOGE(TAG, "Failed to read response");
//                     flag = 0;
//                 }
//             }
//         }
//     }
//     esp_http_client_close(client);
//     esp_http_client_cleanup(client);
//     return flag;
// }
// /**
//  * @brief  This function is used for the posting of data to Hawkbit Server.
//  *
//  * @param URL
//  */

// bool HttpPost(char *URL, char *buffer, int buffer_size, char *authentication_token)
// {
//     bool flag = 0;
//     ESP_LOGI(TAG, "POST_URL:%s", URL);
//     esp_err_t err = ESP_FAIL;

//     esp_http_client_config_t config = {
//         .url = URL,
//     };

//     esp_http_client_handle_t client = esp_http_client_init(&config);

// #ifdef HAWKBIT_AUTHENTICATION

//     esp_http_client_set_header(client, "Authorization", authentication_token);

// #endif

//     esp_http_client_set_method(client, HTTP_METHOD_POST);

//     while (err != ESP_OK)
//     {
//         ESP_LOGI(TAG, "POST_BUFFER:%s", buffer);
//         int content_length = 0;
//         esp_http_client_set_header(client, "Content-Type", "application/json");

//         err = esp_http_client_open(client, strlen(buffer));
//         if (err != ESP_OK)
//         {
//             ESP_LOGE(TAG, "Failed to open HTTP connection: %s", esp_err_to_name(err));
//             flag = 0;
//         }
//         else
//         {
//             int wlen = esp_http_client_write(client, buffer, strlen(buffer));
//             if (wlen < 0)
//             {
//                 ESP_LOGE(TAG, "Write failed");
//                 flag = 0;
//             }

//             content_length = esp_http_client_fetch_headers(client);
//             if (content_length < 0)
//             {
//                 ESP_LOGE(TAG, "HTTP client fetch headers failed");
//                 flag = 0;
//             }
//             else
//             {
//                 int data_read = esp_http_client_read_response(client, buffer, buffer_size);
//                 if (data_read >= 0)
//                 {

//                     ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
//                              esp_http_client_get_status_code(client),
//                              esp_http_client_get_content_length(client));

//                     if (esp_http_client_get_status_code(client) == 200)
//                         flag = 1;
//                     else
//                         flag = 0;
//                     // ESP_LOG_BUFFER_HEX(TAG, output_buffer, strlen(output_buffer));
//                 }
//                 else
//                 {
//                     ESP_LOGE(TAG, "Failed to read response");
//                 }
//             }
//         }
//     }
//     esp_http_client_close(client);
//     esp_http_client_cleanup(client);
//     return flag;
// }
