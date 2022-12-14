/**
 * @file i2s_play.cpp
 * @author Hamza Islam (hamzaislam@170gmail.com)
 * @brief File handling SOC for the Battery
 * @version 0.1
 * @date 2022-08-30
 *
 * @copyright Copyright (c) 2022
 * @Supporter Scott Jensen
 */

#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include <freertos/task.h>

#include "i2s_play.h"
#include "main.h"
#include "Audio.h"
#include "touch_pad_custom.h"
#include "led_strip.h"
#include "sleep_manager.h"
#include "firebase_mqtt.h"

Audio *audio;

uint8_t volumn = 15;

TaskHandle_t xTaskAudioPlay = NULL;
QueueHandle_t xQueueAudioPlay = NULL;

ButtonPress_t rec_button_i2s;

bool imp_music = false;
bool volumn_stroble = true;
bool music_override = false;
bool end_of_audio = false;
/**
 * @brief This function is used to retur teh file name eith respect to the button call
 *
 *
 * @param rec_button_i2s
 * @return char* File name
 */
char *ExtractFileName(ButtonPress_t rec_button_i2s)
{
  switch (rec_button_i2s)
  {
  case kTouch0:
    // blue_count++;
    imp_music = true;
    return (char *)("call_button_recording.mp3");
  case kTouch1:
    blue_count++;
    imp_music = true;
    return (char *)("blue_button_recording.mp3");

  case kTouch2:
    green_count++;
    imp_music = true;
    return (char *)("green_button_recording.mp3");

  case kTouch3:
    orange_count++;
    imp_music = true;
    return (char *)("orange_button_recording.mp3");

  case kble: // ble
    return (char *)("ble_button_recording.mp3");

  case kTouch5:
    red_count++;
    imp_music = true;
    return (char *)("red_button_recording.mp3");

  case kTouch6:
    purple_count++;
    imp_music = true;
    return (char *)("purple_button_recording.mp3");

  case kTouch7:
    yellow_count++;
    imp_music = true;
    return (char *)("goodbye_button_recording.mp3");

  case kTouch8:
    purple_count++;
    imp_music = true;
    return (char *)("purple_button_recording.mp3");

  case kvolumn:
    return (char *)("volumn_button_recording.mp3");

  case kconnect:
    return (char *)("wificon_recording.mp3");

  case kdisconnect:
    return (char *)("wifidis_recording.mp3");

  default:
    return NULL;
  }
}
/**
 * @brief This  is the Audio Task that take care of all Queue conteent which
 * instruct I2s Engine to Play the music and also take care of the the Memory buffer
 * after sudden stop of music.
 *
 * @param None
 */
void IRAM_ATTR AudioTask(void *param)
{
  audio = new Audio;
  audio->setPinout(16, 26, 25);
  xQueueReset(xQueueAudioPlay);

  if (sleep_wake_press != kTouchIdle)
  {
    xQueueSendToBack(xQueueAudioPlay, &(sleep_wake_press), (TickType_t)10);
  }

  while (1)
  {

    if (xQueueReceive(xQueueAudioPlay, &(rec_button_i2s), (5 / portTICK_PERIOD_MS)) == 1)
    {
      audio->setVolume(volumn);

      char *file_name;
      if (audio->isRunning())
      {

        music_override = true;

        audio->stop_music_running();
        file_name = ExtractFileName(rec_button_i2s);
        if (file_name != NULL)
        {
          audio->connecttoFS(SPIFFS, file_name);
          end_of_audio = false;
          if (xQueueLedStrip != NULL)
          {
            xQueueSendToBack(xQueueLedStrip, &(rec_button_i2s), (TickType_t)10);
          }
        }
      }
      else
      {

        file_name = ExtractFileName(rec_button_i2s);
        if (file_name != NULL)
        {
          audio->connecttoFS(SPIFFS, file_name);
          end_of_audio = false;
          if (xQueueLedStrip != NULL)
          {
            xQueueSendToBack(xQueueLedStrip, &(rec_button_i2s), (TickType_t)10);
          }
        }
      }
    }
    if (!end_of_audio)
    {
      audio->loop();
    }
  }
}
//}

/**
 * @brief THis function is used to initilize the Audio Tunnel.
 * this function also Implement the Queue for the Aduio transfer
 * @param None
 */

void AudioInit(void *param)
{
  vTaskDelay(10 / portTICK_PERIOD_MS);
  /* QUEUE INITILAZATION*/
  xQueueAudioPlay = xQueueCreate(4, sizeof(ButtonPress_t));
  if (xQueueAudioPlay == NULL)
  {
    printf("********PLAY AUDIO QUEUE IS NOT CREATED SUCESSFULLY*******\n");
  }
  else
  {
    printf("********PLAY AUDIO QUEUE CREATED SUCESSFULLY*******\n");

    xTaskCreatePinnedToCore(&AudioTask, "I2S_Task", 1024 * 6, NULL, 3, &xTaskAudioPlay, 0);
  }
}
/**
 * @brief EOF for the Audio file
 *
 */
void audio_eof_mp3(const char *)
{
  end_of_audio = true;
  prev_button = kTouchIdle;

  if (xQueueLedStrip != NULL && !music_override)
  {
    xQueueSendToBack(xQueueLedStrip, &(prev_button), (TickType_t)10);
  }
  /*only for the sendCount from here*/
  if (xQueueFireBase != NULL && imp_music)
  {
    if (uxQueueSpacesAvailable(xQueueFireBase) == 2)
    {
      bool dummy = 1;
      xQueueSendToBack(xQueueFireBase, &(dummy), (TickType_t)10);
    }
  }
  prev_tick_count = xTaskGetTickCount();
  music_override = false;
  imp_music=false;
}
