/**
 *
 */
#include <string.h>
#include <stdlib.h>
#include "stdio.h"

#include "esp_log.h"
#include "freertos_handler.h"
#include "wifi_manger.h"
#include "http_client.h"
#include "dns_ping.h"

TaskHandle_t xDnsTask;

#define DNS_PING_TIME_SEC 10 // Dns ping tiome in seconds

bool dns_network_status = false;

/**
 *
 */
void DnsTask(void *param)
{
    uint8_t status_code = 0;
    char *data = NULL;

    while (1)
    {
        while (!IsSystemNetworkAvailable(NULL))
        {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }

        while (1)
        {
            data = HttpPerform("http://www.google.com", HEAD, &status_code, NULL, NULL);
            data != NULL ? free(data) : 1;

            if (status_code == 200)
            {
                dns_network_status = true;
                break;
            }
            else
            {
                dns_network_status = false;
            }
            vTaskDelay(3000 / portTICK_PERIOD_MS);
        }
        vTaskDelay(1000 * DNS_PING_TIME_SEC / portTICK_PERIOD_MS);
    }
}
/**
 *
 */
void DnsInit(void *param)
{
    xTaskCreatePinnedToCore(DnsTask,
                            "dns_task",
                            DNS_TASK_STACK_SIZE,
                            NULL, DNS_TASK_PRIO,
                            &xDnsTask,
                            DNS_TASK_CORE);
}