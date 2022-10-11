#ifndef CUSTOM_AUDIOI2S_
#define CUSTOM_AUDIOI2S_
// #include "freertos/FreeRTOS.h"
// #include <freertos/queue.h>
#include "Arduino.h"

void AudioTask(void *param);

extern QueueHandle_t xQueueAudioPlay;

void AudioInit(void *param);

extern bool imp_music;
extern uint8_t volumn;
extern bool volumn_stroble;

#endif