// #include "esp_wifi_types.h"
// #include "esp_timer.h"
// #include "esp_wifi.h"
// #include "esp_websocket_client.h"
// #include "esp_sleep.h"

#include "string.h"
#include "esp_system.h"

#include "indications.h"
#include "hw_monitor.h"
#include "parse.h"
#include "main.h"
#include "parse_websocket.h"
#include "driver/gpio.h"
#include "freertos_handler.h"
#include "system_gpio.h"

#define NONE 0x00

TaskHandle_t xWifiLedTask = NULL;
TaskHandle_t xBuzzerTask = NULL;

QueueHandle_t xBuzzerQueue = NULL;
QueueHandle_t xWifiLedQueue = NULL;

volatile bool buzzer_level=false;

static const char *TAG = "INDICATION";
//** {round, r_on,r_off,maj_on,maj_off}
uint16_t LED_BLINK[6][5] = {{0, 0, 0, 0, 1000}, {0, 0, 0, 300, 300}, {2, 100, 100, 0, 2000}, {0, 0, 0, 1000, 1000}, {0, 0, 0, 1000, 1000}, {0, 0, 0, 1000, 0}};

// NONE,   FACTORY RESET,   USER_CONNECTED,   APP_MODE,   AP_STA,  ALL_THING_CONNECTED

uint16_t BUZZER_BEEP[8][2] = {{0, 0}, {1000, 2000}, {1000, 1000}, {1000, 1000}, {1000, 1000}, {0, 0}, {0, 0},{1000,0}};
// NONE , FAULT_DETECT,    AP_STA,   MULTIMODE, ACCOUNT_RECEIVED, FIRMWARE_UPFDATE

uint8_t buzzer_array[5] = {0x00};

void performBuzzer(void *param)
{
   /**
    * @brief prio are here
    * ---> 1.Reserved for futrue
    * ---> 2.mode_button
    * ---> 3.Fault
    * ---> 4.AP-STA
    * ---> 5.Reserved
    */
}
/**
 * @brief Construct a new Buzzerbeep object
 *
 * @param count
 */
static void BuzzerTask(uint8_t count)
{
   /* Initial setting */

   buzzer_beep_t event_received = NONE;

   while (1)
   {
      xQueueReceive(xBuzzerQueue, &(event_received), (TickType_t)100);
      if (BUZZER_BEEP[event_received][0])
      {
         
         // printf("BUZZER IS ON COMMAND : %d \n",event_received);
         buzzer_level=true;
         vTaskDelay(BUZZER_BEEP[event_received][0] / portTICK_PERIOD_MS);
      }
      /*< First check the off time is available here >*/

      if (BUZZER_BEEP[event_received][1])
      {
         // printf("BUZZER IS OFF COMMAND : %d \n",event_received);
        
                  buzzer_level=false;
         vTaskDelay(BUZZER_BEEP[event_received][1] / portTICK_PERIOD_MS);
      }
      /*Speicoal case for app notification*/
      if (event_received == ACCOUNT_RECEIVED || event_received == FIRMWARE_COMPLETE)
      {
         buzzer_level=false;
         esp_restart();
      }
   }
}
void PerformWifiLed(wifi_led_indication_t event_received)
{
   //** {round ,on,off,maj_on,maj_delay}
   if (LED_BLINK[event_received][0])
   {
      for (uint8_t i = 0; i < LED_BLINK[event_received][0]; i++)
      {
         if (LED_BLINK[event_received][1])
         {
            gpio_set_level(WIFI_LED_PIN, true);
            vTaskDelay(LED_BLINK[event_received][1] / portTICK_PERIOD_MS);
         }
         if (LED_BLINK[event_received][2])
         {
            gpio_set_level(WIFI_LED_PIN, false);
            vTaskDelay(LED_BLINK[event_received][2] / portTICK_PERIOD_MS);
         }
      }
   }
   if (LED_BLINK[event_received][3])
   {
      gpio_set_level(WIFI_LED_PIN, true);
      vTaskDelay(LED_BLINK[event_received][3] / portTICK_PERIOD_MS);
   }
   if (LED_BLINK[event_received][4])
   {
      gpio_set_level(WIFI_LED_PIN, false);
      vTaskDelay(LED_BLINK[event_received][4] / portTICK_PERIOD_MS);
   }
}

/**
 * @brief This Task is responsible for WiFi Led blinking depending upon its state.
 * @param ptr
 * @return None
 */
static void WiFiLedIndicationTask(void *ptr)
{
   /* Initial setting */

   wifi_led_indication_t event_received = NONE;
   while (1)
   {
      if(xWifiLedQueue != NULL)
      xQueueReceive(xWifiLedQueue, &(event_received), (TickType_t)10);
      
    PerformWifiLed(event_received);
   }
}

/**
 * @brief This function creates a freertos task that is responsible for wifi and server connectivity indication
 * @param None
 * @return None
 */
void WifiIndicationInit(void *param)
{

   xTaskCreatePinnedToCore(&WiFiLedIndicationTask,
                           "wifi_indication_task",
                           WIFILED_TASK_STACK_SIZE,
                           NULL,
                           WIFILED_TASK_PRIO,
                           &xWifiLedTask,
                           WIFILED_TASK_CORE);

   /** check if the system is the factory reset here */
   if (strcmp(network_config.ssid1, "") && strcmp(network_config.user_name, ""))
   {
      wifi_led_indication_t event_received = FACTORY_RESET;
      xQueueSendToBack(xWifiLedQueue, &(event_received), (TickType_t)0);
   }
}

/**
 * @brief This function creates a freertos task that is responsible for undermaintenance indication
 * @param None
 * @return None
 */
void BuzzerInit(void *param)
{
   xTaskCreatePinnedToCore(BuzzerTask,
                           "buzzer_task",
                           BUZZER_TASK_STACK_SIZE,
                           NULL,
                           BUZZER_TASK_PRIO,
                           &xBuzzerTask,
                           BUZZER_TASK_CORE);
}
