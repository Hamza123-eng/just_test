#ifndef HAWKBIT_H_
#define HAWKBIT_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "http_client.h"


#define HAWKBIT_USER "remotewell"
#define HAWKBIT_PASSCODE "remotewell@123"

#define HAWKBIT_AUTHENTICATION /*Enable Hawkbit Authentication*/
/**
 * @brief Hawkbit Parameter defination
 *
 */
#define HTTP_OUTPUT_BUFFER_SIZE 2048
#define HTTP_INPUT_BUFFER_SIZE 1024
#define HAWKBIT_ARTIFACT_LINK_SIZE 256
#define HAWKBIT_FEED_BACK_URL_SIZE 256
#define HAWKBIT_URL_SIZE 256
#define HAWKBIT_AUTHENTICATIN_TOKEN_SIZE 128
#define HAWKBIT_DEPLOYMENt_ID_SIZE 8
#define HAWKBIT_MAC_SIZE 20
#define Hawkbit_SECURITY_TOKEN_SIZE 40

/*Task Handler For Hawkbit*/

/**
 * @brief Hawkbit Handler Structure
 *
 */
typedef struct
{

    int hawkbit_sleep_time;

    char security_token[Hawkbit_SECURITY_TOKEN_SIZE];
    char deployment_id[HAWKBIT_DEPLOYMENt_ID_SIZE];
    char hawkbit_url[HAWKBIT_URL_SIZE];
    char hawkbit_authentication[HAWKBIT_AUTHENTICATIN_TOKEN_SIZE];
    char hawkbit_feed_Back_url[HAWKBIT_FEED_BACK_URL_SIZE];
    char artifact_link[HAWKBIT_ARTIFACT_LINK_SIZE];
    // char http_input_buffer[HTTP_INPUT_BUFFER_SIZE];
    char http_output_buffer[HTTP_OUTPUT_BUFFER_SIZE];

    char mac_address[HAWKBIT_MAC_SIZE];

} hawkbit_handle_t;

hawkbit_handle_t hawkbit; /*Hawkbit Instance*/


void HawkbitStart();


extern http_header_t hawkbit_header;

#endif // HAWKBIT_H_