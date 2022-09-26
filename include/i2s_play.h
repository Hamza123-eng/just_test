#ifndef CUSTOM_AUDIOI2S_
#define CUSTOM_AUDIOI2S_
// #include "freertos/FreeRTOS.h"
// #include <freertos/queue.h>
#include "Arduino.h"

void AudioTask(void *param);

extern QueueHandle_t xQueueAudioPlay;

void AudioInit(void *param);

#endif