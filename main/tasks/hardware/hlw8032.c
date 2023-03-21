#include <stdio.h>
#include <string.h>

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "hlw8032.h"
#include "hlw8032_bts.h"
#include "freertos_handler.h"
#include "parse.h"
#include "system_gpio.h"
#include "hw_monitor.h"

#define NONE 0x00

TaskHandle_t xHlwTask = NULL;

hlw_voltage_t hlw_voltage = {{220, 220, 220},0, 2, false, false, false};

uart_event_t event;

static const char *TAG = "UART HLW[]: ";

static QueueHandle_t uart_queue[3] = {NULL};
/**
 * @brief Remotewell_PRO
 *
 * @param param
 */
void CheckVoltageError(void *param)
{
   // printf("UNDER LIM :%i \n",device_config.under_volt_lim);
   // printf("over LIM :%i \n",device_config.over_volt_lim);
   // printf("PHASE 1 :%i \n",hlw_voltage.phase_voltage[0]);
   // printf("PHASE 2 :%i \n",hlw_voltage.phase_voltage[1]);
   // printf("PHASE 3 :%i \n",hlw_voltage.phase_voltage[2]);

   if (hlw_voltage.phase_voltage[0] < device_config.under_volt_lim ||
       hlw_voltage.phase_voltage[1] < device_config.under_volt_lim ||
       hlw_voltage.phase_voltage[2] < device_config.under_volt_lim)
   {
      hlw_voltage.error_type = UNDERVOLTON;
   }
   else if (hlw_voltage.phase_voltage[0] > device_config.over_volt_lim ||
            hlw_voltage.phase_voltage[1] > device_config.over_volt_lim ||
            hlw_voltage.phase_voltage[2] > device_config.over_volt_lim)
   {
      hlw_voltage.error_type = OVERVOLTON;
   }
   else
   {
      hlw_voltage.error_type = 0;
   }
   // printf("VOLTAGE : %hhu ERROR\n",hlw_voltage.error_type);
}
/**
 * @brief
 *
 * @param pvParameters
 */
void ParseHlwData(uint8_t uart_num, uint8_t data_size, uint8_t event_id, QueueHandle_t uart_queue)
{
   switch (event_id)
   {

   case UART_DATA:
   {
      uint8_t *regData = (uint8_t *)malloc(200);
      memset(regData, 0, 200);
      uart_read_bytes(uart_num, regData, event.size, portMAX_DELAY);
      hlw_data_handle(regData, data_size, uart_num);
      uart_flush_input(uart_num);
      xQueueReset(uart_queue);
      free(regData);
   }
   break;
   case UART_FIFO_OVF:
   {
      uart_flush_input(uart_num);
      xQueueReset(uart_queue);
   }
   break;
   default:
      break;
   }
}

static void HlwTask(void *pvParameters)
{
   ESP_LOGI(TAG, "HLW TASK GOT INITILIZED");
   for (;;)
   {
      // Waiting for UART event.
      // if (xQueueReceive(uart_queue[0], (void *)&event, (TickType_t)10) == pdPASS)
      // {
      //    // ESP_LOGI(TAG,"DATA IS AVAILABEL ON UART 0");
      //    ParseHlwData(UART_NUM_0, (uint8_t)event.size, (uint8_t)event.type, uart_queue[0]);
      // }
      if (xQueueReceive(uart_queue[1], (void *)&event, (TickType_t)10) == pdPASS)
      {
         ParseHlwData(UART_NUM_1, (uint8_t)event.size, (uint8_t)event.type, uart_queue[1]);
      }
      if (xQueueReceive(uart_queue[2], (void *)&event, (TickType_t)10) == pdPASS)
      {
         ParseHlwData(UART_NUM_2, (uint8_t)event.size, (uint8_t)event.type, uart_queue[2]);
      }

      //CheckVoltageError(NULL);

      vTaskDelay(500 / portTICK_PERIOD_MS);
   }
}

/**
 * @brief
 *
 * @param param
 */
void HlwInit(void *param)
{

   /* Configure parameters of an UART driver,*/
   /* communication pins and install the driver */
   uart_config_t uart_config = {
       .baud_rate = 4800,
       .data_bits = UART_DATA_8_BITS,
       .parity = UART_PARITY_DISABLE,
       .stop_bits = UART_STOP_BITS_1,
       .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
       .source_clk = UART_SCLK_APB,
   };

   /*< UART 0 for HLW>*/
   // vTaskDelay(1000);
   //    uart_param_config(UART_NUM_0, &uart_config);
   //    uart_set_pin(UART_NUM_0, 1, HLW3_RX_PIN, -1, -1);
   //    uart_driver_install(UART_NUM_0, 1024 * 2, 1024*4, 3, &uart_queue[0], 0);

   /*< UART 1 for HLW>*/

   uart_param_config(UART_NUM_1, &uart_config);
   uart_set_pin(UART_NUM_1, -1, HLW2_RX_PIN, -1, -1);
   uart_driver_install(UART_NUM_1, BUFF_SIZE * 2, 0, 3, &uart_queue[2], 0);

   /*< UART 2 for HLW>*/

   uart_param_config(UART_NUM_2, &uart_config);
   uart_set_pin(UART_NUM_2, -1, HLW1_RX_PIN, -1, -1);
   uart_driver_install(UART_NUM_2, BUFF_SIZE * 2, 0, 3, &uart_queue[1], 0);

   // Create a task to handler UART event

   xTaskCreatePinnedToCore(HlwTask, "hlw_event_task", HLW_TASK_STACK_SIZE, NULL, HLW_TASK_PRIO, &xHlwTask, HLW_TASK_CORE);
}
