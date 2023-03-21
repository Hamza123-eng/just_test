// #include <string.h>
// #include "esp_log.h"

// #include "history_events.h"
// #include "hw_monitor.h"
// #include "parse.h"
// #include "put.h"
// #include "post.h"
// #include "cJSON.h"

// static const char *TAG = "History Events";
// bool allow_history = true;

// HistoryEvents_t current_history_state = kIdle;
// HistoryEvents_t previous;
// Parse_t parse_recv, prev_parse_recv;
// Parse_t parse_recv_put, prev_parse_recv_put;
// Parse_t parse_recv_post, prev_parse_recv_post;

// void HistoryEventsTask(void *ptr)
// {
//     while (1)
//     {
//         if (allow_history)
//         {
//             if (xQueueReceive(xHistoryEventQueue, &parse_recv, (portTickType)portMAX_DELAY))
//             {
//                 if (parse_recv.send_fields.button_status != prev_parse_recv.send_fields.button_status || parse_recv.send_fields.motor_status != prev_parse_recv.send_fields.motor_status || parse_recv.send_fields.phase_error != prev_parse_recv.send_fields.phase_error || parse_recv.send_fields.trip_status != prev_parse_recv.send_fields.trip_status || parse_recv.send_fields.undermnt != prev_parse_recv.send_fields.undermnt)
//                 {
//                     prev_parse_recv.send_fields.button_status = parse_recv.send_fields.button_status;
//                     prev_parse_recv.send_fields.motor_status = parse_recv.send_fields.motor_status;
//                     prev_parse_recv.send_fields.phase_error = parse_recv.send_fields.phase_error;
//                     prev_parse_recv.send_fields.trip_status = parse_recv.send_fields.trip_status;
//                     prev_parse_recv.send_fields.undermnt = parse_recv.send_fields.undermnt;
//                     //    prev_parse_recv.send_fields.elcectricity = parse_recv.send_fields.elcectricity;

//                     char buff[500];

//                     void (*fp_put_json_packet)(char *, int, bool, bool, bool, bool, bool, bool) = &PUT_JSON_Packet; // creating function pointer and assigning address of the function

//                     (*fp_put_json_packet)(buff, 0, parse_recv.send_fields.motor_status,
//                                           parse_recv.send_fields.trip_status, parse_recv.send_fields.undermnt, parse_recv.send_fields.alive_status, parse_recv.send_fields.phase_error, parse_recv.send_fields.elcectricity);

//                     // send record of current hardware state
//                     if (xSemaphoreTake(put_control_mutex, portMAX_DELAY) == pdTRUE)
//                     {
//                         ESP_LOGI(TAG, "History PUT Take Mutex");

//                         while (PUT(buff) != 200)
//                         {
//                             vTaskDelay(100 / portTICK_PERIOD_MS);
//                         }

//                         memset(buff, 0, 1);

//                         xSemaphoreGive(put_control_mutex);

//                         ESP_LOGI(TAG, "History PUT Give Mutex");
//                     }

//                     if (xSemaphoreTake(post_control_mutex, portMAX_DELAY) == pdTRUE)
//                     {
//                         ESP_LOGI(TAG, "History POST Take Mutex");

//                         void (*fp_post_json_packet)(char *, int, bool, bool, bool, bool, bool, char *, char *, char *, bool) = &POST_JSON_Packet; // creating function pointer and assigning address of the function

//                         (*fp_post_json_packet)(buff, parse_recv.send_fields.button_status, parse_recv.send_fields.motor_status,
//                                                parse_recv.send_fields.trip_status, parse_recv.send_fields.undermnt, parse_recv.send_fields.alive_status,
//                                                parse_recv.send_fields.phase_error, parse_recv.send_fields.tag, parse_recv.send_fields.tag_phone, parse_recv.send_fields.run_time, parse.send_fields.elcectricity);

//                         while (POST(buff) != 201)
//                         {
//                             vTaskDelay(100 / portTICK_PERIOD_MS);
//                         }

//                         xSemaphoreGive(post_control_mutex);

//                         ESP_LOGI(TAG, "History POST Give Mutex");
//                     }

//                     memset(parse.send_fields.run_time, 0, strlen(parse.send_fields.run_time));
//                 }
//             }
//         }
//         vTaskDelay(400/ portTICK_PERIOD_MS);
//     }
// }

// void ElectricitySend(int x)

// {
//     CalculateRunTime();
//     // printf("enter in electricity\n");

//     int retry = 15;
//     char buff[500];

//     void (*fp_put_json_packet)(char *, int, bool, bool, bool, bool, bool, bool) = &PUT_JSON_Packet; // creating function pointer and assigning address of the function
//     if (x)
//     {
//         (*fp_put_json_packet)(buff, kElectricityON, 0,
//                               0, parse_recv.send_fields.undermnt, 0, 0, 0);
//     }
//     else
//     {
//         (*fp_put_json_packet)(buff, kElectricityOFF, 0,
//                               0, parse_recv.send_fields.undermnt, 0, 0, 1);
//     }

//     // send record of current hardware state
//     if (xSemaphoreTake(put_control_mutex, portMAX_DELAY) == pdTRUE)
//     {
//         ESP_LOGI(TAG, "History PUT Take Mutex");
//    /// printf("%s",buff);
//         while (PUT(buff) != 200  && retry!=1 )
//         {
//             retry--;
//             // printf("retry put=%d",retry);
//            // printf("agian put\n");
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//         }

//         retry = 15;

//         memset(buff, 0, 500);

//         xSemaphoreGive(put_control_mutex);

//         ESP_LOGI(TAG, "History PUT Give Mutex");
//     }
//     memset(buff, 0, 500);
//     if (xSemaphoreTake(post_control_mutex, portMAX_DELAY) == pdTRUE)
//     {
//         ESP_LOGI(TAG, "History POST Take Mutex");

//         void (*fp_post_json_packet)(char *, int, bool, bool, bool, bool, bool, char *, char *, char *, bool) = &POST_JSON_Packet; // creating function pointer and assigning address of the function
//         if (x)
//         {
//             (*fp_post_json_packet)(buff, kElectricityON, 0,
//                                    0, parse_recv.send_fields.undermnt, 0,
//                                    0, "", "", parse.send_fields.run_time, 0);
//         }
//         else
//         {
//             (*fp_post_json_packet)(buff, kElectricityOFF, 0,
//                                    0, parse_recv.send_fields.undermnt, 0,
//                                    0, "", "", parse.send_fields.run_time, 1);
//         }
//         while (POST(buff) != 201 && retry!=1)
//         {
//             retry--;
//          //   printf("agian post\n");
//            //  printf("retry post=%d",retry);
//             vTaskDelay(100 / portTICK_PERIOD_MS);
//         }

//         xSemaphoreGive(post_control_mutex);

//         ESP_LOGI(TAG, "History POST Give Mutex");
//     }
//     // if (x != 1)
//     // {
//     //     vTaskDelay(2000 / portTICK_PERIOD_MS);
//     //     memset(buff, 0, 500);
//     //     void (*fp_put_json_packet)(char *, int, bool, bool, bool, bool, bool, bool) = &PUT_JSON_Packet; // creating function pointer and assigning address of the function

//     //     (*fp_put_json_packet)(buff, 0, 0,
//     //                           0, parse_recv.send_fields.undermnt, 0, 0, 1);
//     //     if (xSemaphoreTake(put_control_mutex, portMAX_DELAY) == pdTRUE)
//     //     {
//     //         ESP_LOGI(TAG, "History PUT Take Mutex");

//     //         while (PUT(buff) != 200)
//     //         {
//     //             retry--;
//     //             printf("agian put\n");
//     //             vTaskDelay(100 / portTICK_PERIOD_MS);
//     //         }

//     //         xSemaphoreGive(put_control_mutex);

//     //         ESP_LOGI(TAG, "History PUT Give Mutex");
//     //     }
//     // }

//   //  printf("send is scuceesful\n");
// }
