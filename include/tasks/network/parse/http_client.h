#ifndef HTTP_CLIENT_H_
#define HTTP_CLIENT_H_

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    POST,
    DELETE,
    PUT,
    GET,
    HEAD
}http_request_type_t; 

typedef struct 
{
    char key_1[48];
    char key_2[48];

    char value_1[132];  //need to fix just for hawkbit header
    char value_2[132];

}http_header_t;

char *HttpPerform(char *url, http_request_type_t request_type, uint8_t *status_code, char *data_buffer, http_header_t *header);
void GetNetworkLock(void *param);
void ReturnNetworkLock(void *param);
void InitNetworkLock(void *param);


#endif //HTTP_CLIENT_H