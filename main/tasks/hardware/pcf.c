#include <i2cdev.h>
#include <stdio.h>
#include "string.h"
#include "phase_sensing.h"
#include "system_gpio.h"
#include "pcf.h"
#include "hw_monitor.h"
#include "hlw8032.h"
#include "parse.h"
#include "freertos_handler.h"

#define NONE 0x00

i2c_dev_t pcf8574;

TaskHandle_t xPcfTask = NULL;
QueueHandle_t xPcfQueue = NULL;
SemaphoreHandle_t xI2cSemaphore = NULL;

static char *TAG = "PCF[] :";
/**
 *
 */
void i2c_dev_write_1(const void *out_reg, size_t out_reg_size, const void *out_data, size_t out_size)
{

    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, PCF_ADDRESS << 1, true);
    if (out_reg && out_reg_size)
        i2c_master_write(cmd, (void *)out_reg, out_reg_size, true);
    i2c_master_write(cmd, (void *)out_data, out_size, true);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(I2C_PORT, cmd, pdMS_TO_TICKS(2));
    i2c_cmd_link_delete(cmd);
}
/**
 *
 */
void PcfWrite(uint8_t status)
{
    i2c_dev_write_1(NULL, 0, &status, 1);
}

/**
 *
 */
esp_err_t i2c_master_init(void)
{
    int i2c_master_port = I2C_PORT;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_SDA_PIN,
        .scl_io_num = I2C_SCL_PIN,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 100000,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, 256, 256, 0);
}
/**
 * @brief
 *
 * @return uint8_t
 */
void i2cSemaphoreTake(void *param)
{
    while (1)
    {
        if (xSemaphoreTake(xI2cSemaphore, (TickType_t)10) == pdTRUE)
        {
            break;
        }
        vTaskDelay(50);
    }
}
/**
 * @brief
 *
 * @param param
 */
void i2cSemaphoreGive(void *param)
{
    while (1)
    {
        if (xSemaphoreGive(xI2cSemaphore) == pdTRUE)
        {
            break;
        }
        vTaskDelay(100);
    }
}
/**
 * @brief
 *
 * @param status
 */
void WriteToPcf(uint8_t status)
{
    if (buzzer_level)
    {
        printf("Going to write the buzzer on \n");
        status = status | PCF_BUZZER;
    }

    i2cSemaphoreTake(NULL);
    PcfWrite(~status);
    i2cSemaphoreGive(NULL);
}
/**
 * @brief
 *
 * @param mode
 */
void ExecutePcf(pcf_mode_t mode)
{
    uint8_t arr[3] = {0x0C, 0x1E, 0x3F};
    uint8_t status = 0x00;
    switch (mode)
    {
    case NONE_PCF:
        printf("in pcf none\n");
        WriteToPcf(status);
        // while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // }
        break;
    case WRONG_USER:

        status = 0x07;
        WriteToPcf(status);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        status = 0x00;
        WriteToPcf(status);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        break;

    case FACTORY_RESET_PCF:

        for (uint8_t i = 0; i < 6; i++)
        {
            status = status << 1;
            status |= 0x01;
            WriteToPcf(status);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        for (uint8_t i = 0; i < 6; i++)
        {
            status = status >> 1;
            WriteToPcf(status);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        break;
    case NO_SERIAL_NUMBER:

        status = 0;
        for (uint8_t i = 0; i < 3; i++)
        {
            status = status | arr[i];
            WriteToPcf(status);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        for (uint8_t i = 2; i > 0; i--)
        {
            status = status & arr[i - 1];
            WriteToPcf(status);
            vTaskDelay(200 / portTICK_PERIOD_MS);
        }
        status = 0;
        WriteToPcf(status);
        vTaskDelay(200 / portTICK_PERIOD_MS);

        break;

    case RUNNING_MODE:
    {
        status = xEventGroupGetBits(xPhaseEvent) & PHASE1 ? status | PCF_P1 : status;
        status = xEventGroupGetBits(xPhaseEvent) & PHASE2 ? status | PCF_P2 : status;
        status = xEventGroupGetBits(xPhaseEvent) & PHASE3 ? status | PCF_P3 : status;

        status = live_class.motor_status ? status | PCF_RUN : status;
        status = live_class.trip_status ? status | PCF_TRIP : status;
        status = live_class.undermnt ? status | PCF_MNT : status;

        if (hlw_voltage.error_type == UNDERVOLTON)
        {
            status = (status | PCF_UNDER_VOLT);
        }
        if (hlw_voltage.error_type == OVERVOLTON)
        {
            status = (status | PCF_OVER_VOLT);
        }
        WriteToPcf(status);
        vTaskDelay(900 / portTICK_PERIOD_MS);
    }
    break;
    case FIRMWARE_UPDATE:
    {
        status = (status | PCF_TRIP | PCF_MNT);
        WriteToPcf(status);
        vTaskDelay(150 / portTICK_PERIOD_MS);
        status = 0x00;
        WriteToPcf(status);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    break;
    default:
        break;
    }
}
/**
 * @brief
 *
 * @param pvParameters
 */
void PcfTask(void *pvParameters)
{
    ESP_LOGI(TAG, "PCF TASK IS GOING TO INITILIZE");

    i2c_master_init();
    pcf_mode_t pcf_event = RUNNING_MODE;

    while (1)
    {
        /** No Delay is here because each class hold the Delay */
        if (xPcfQueue != NULL)
            xQueueReceive(xPcfQueue, &(pcf_event), (TickType_t)0);

        ExecutePcf(pcf_event);
    }
}
/**
 * @brief
 *
 * @param param
 */
void PcfInit(void *param)
{
    /* initilize the Pcf queue*/

    /*< Going to create i2c semaphore >*/

    xI2cSemaphore = xSemaphoreCreateBinary();

    if (xI2cSemaphore == NULL)
    {
        while (1)
        {
            ESP_LOGI(TAG, "I2C Semaphore fail to initilize");
            vTaskDelay(100);
        }
    }

    i2cSemaphoreGive(NULL);
    pcf_mode_t mode = NONE_PCF;

    if (xPcfQueue != NULL)
        xQueueSendToBack(xPcfQueue, &(mode), (TickType_t)100);

    xTaskCreatePinnedToCore(PcfTask, "pcf_task", PCF_TASK_STACK_SIZE, NULL, PCF_TASK_PRIO, &xPcfTask, PCF_TASK_CORE);
}
