/**
 * @file system_spiffs.c
 * @author Hamxa Islam {hamzaislam170@gamil.com}
 * @brief
 * @version 0.1
 * @date 2022-12-06
 *
 * @ copyright --->  EPTeck Tecnologies Gmbh   2022
 *
 */
#include "stdio.h"
#include "stdlib.h"
#include <sys/unistd.h>
#include <sys/stat.h>

#include "esp_err.h"
#include "esp_log.h"

#include "parse_event_controller.h"
#include "esp_spiffs.h"

static char *TAG = "SPIFFS []";
/**
 * @brief
 *
 * @param param
 */
void Init_Spiffs(void *param)
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false};

    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK)
    {
        if (ret == ESP_FAIL)
        {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        }
        else if (ret == ESP_ERR_NOT_FOUND)
        {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        }
        else
        {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
    }

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(NULL, &total, &used);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    }
    else
    {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
}
/**
 * @brief
 *
 * @param read_pointer
 * @param size
 * @param data
 */
void ReadFromSpiffs(uint16_t read_pointer, uint16_t size, void *data)
{
    FILE *f = fopen("/spiffs/ep01.txt", "r+");
    // int size = sizeof(parse_event_send_t);
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open ep01.txt");
        return;
    }
    /*< if sucessfully opened the file system now go for data read>*/
    else
    {
        fseek(f, (long)read_pointer * size, SEEK_SET);
        //  printf("----------------->INTERLA READ PTR : 0x%li\n", ftell(f));
        fread((void *)data, 1, size, f);
        fclose(f);
    }
}
/**
 * @brief
 *
 * @param write_pointer
 * @param size
 * @param data
 */
void WriteToSpiffs(uint16_t write_pointer, uint16_t size, void *data)
{

    FILE *f = fopen("/spiffs/ep01.txt", "r+b");
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open ep01");
        return;
    }
    /*< if sucessfully opened the file system now go for data write>*/
    else
    {
        fseek(f, (long)write_pointer * size, SEEK_SET);
        //  printf( "%i----> INTERANL write Ptr 0x%ld \n ",write_pointer,ftell(f));
        fwrite(data, 1, size, f);
        fclose(f);
    }
}
/**
 *
 */
bool CheckFileExistance()
{
    FILE *f = fopen("/spiffs/ep01.txt", "r+");
    // int size = sizeof(parse_event_send_t);
    if (f == NULL)
    {
        ESP_LOGE(TAG, "Failed to open ep01.txt");
    }
    /*< if sucessfully opened the file system now go for data read>*/
    else
    {
        uint32_t index = 0;
        fseek(f, 1000, SEEK_SET);
        index = ftell(f);
        fclose(f);
        if (index)
        {
            return true;
        }
    }
    return false;
}
/**
 * @brief
 *
 */
void FillDummy(uint16_t size, uint16_t index)
{
    if (!CheckFileExistance())
    {
        FILE *f = fopen("/spiffs/ep01.txt", "a");
        if (f == NULL)
        {
            ESP_LOGE(TAG, "Failed to open ep01");
            return;
        }
        /*< if sucessfully opened the file system now go for data write>*/
        else
        {
    printf(" FILL DUMMY");

            char test[100] = {0x00};
            for (size_t i = 0; i < index; i++)
            {

                fseek(f, (long)i * size, SEEK_SET);
                // printf( "%i----> INTERANL write Ptr 0x%ld \n ",write_pointer,ftell(f));
                fwrite(test, 1, size, f);
            }
            fclose(f);
        }
    }
}