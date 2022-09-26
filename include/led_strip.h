#ifndef CUSTOM_LEDSTRIP_
#define CUSTOM_LEDSTRIP_

#include <freertos/queue.h>

void StripInit(void *param);

struct strip_param_t
{
    uint8_t p1,p2,p3;
};


extern QueueHandle_t xQueueLedStrip;


#endif