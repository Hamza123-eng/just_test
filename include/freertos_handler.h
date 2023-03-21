
#ifndef _FREERTOS_HANDLER_
#define _FREERTOS_HANDLER_


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"

/*< TASK CORE >*/
#define _TASK_CORE 0

#define HLW_TASK_CORE 0
#define PHASE_TASK_CORE 1
#define WIFILED_TASK_CORE 0
#define BUZZER_TASK_CORE 0
#define BUTTON_TASK_CORE 0
#define WIFI_TASK_CORE 1
#define PCF_TASK_CORE 1
#define HARDWARECORE_TASK_CORE 0
#define PARSECONTOLLER_TASK_CORE 1
#define SCHEDULE_TASK_CORE 0
#define RELAY_TASK_CORE 0
#define TIMEMANGER_TASK_CORE 0
#define DNS_TASK_CORE 0





/*---------------------------------*/

/*<  TASK STACK SIZE >*/
#define _TASK_STACK_SIZE 1028 * 2

#define HLW_TASK_STACK_SIZE 1028 * 6
#define PHASE_TASK_STACK_SIZE 1028 * 3
#define WIFILED_TASK_STACK_SIZE 1028 * 2
#define BUZZER_TASK_STACK_SIZE 1028 * 2
#define BUTTON_TASK_STACK_SIZE 1028 * 2
#define WIFI_TASK_STACK_SIZE 1028 * 3
#define PCF_TASK_STACK_SIZE 1028 * 2
#define HARDWARECORE_TASK_STACK_SIZE 1028 * 4
#define PARSECONTOLLER_TASK_STACK_SIZE 1028 * 3
#define SCHEDULE_TASK_STACK_SIZE 1028 * 6
#define RELAY_TASK_STACK_SIZE 1028 * 3
#define TIMEMANGER_TASK_STACK_SIZE 1028 * 4
#define DNS_TASK_STACK_SIZE 1028 * 3




/*---------------------------------*/

/*< TASK PRIORITY>*/
#define _TASK_PRIO 5

#define HLW_TASK_PRIO 5
#define PHASE_TASK_PRIO 5
#define WIFILED_TASK_PRIO 5
#define BUZZER_TASK_PRIO 5
#define BUTTON_TASK_PRIO 5
#define WIFI_TASK_PRIO 5
#define PCF_TASK_PRIO 5
#define HARDWARECORE_TASK_PRIO 5
#define PARSECONTOLLER_TASK_PRIO 5
#define SCHEDULE_TASK_PRIO 5
#define RELAY_TASK_PRIO 5
#define TIMEMANGER_TASK_PRIO 5
#define DNS_TASK_PRIO 5




/*---------------------------------*/

/*< TASK Handler >*/

extern TaskHandle_t xPhaseTask;
extern TaskHandle_t xHlwTask;
extern TaskHandle_t xWifiLedTask;
extern TaskHandle_t xWifiTask;
extern TaskHandle_t xBuzzerTask;
extern TaskHandle_t xButtonTask;
extern TaskHandle_t xPcfTask;
extern TaskHandle_t xHardwareCoreTask;
extern TaskHandle_t xParseControllerTask;
extern TaskHandle_t xScheduleTask;
extern TaskHandle_t xRelayControlTask;
extern TaskHandle_t xTimeManagerTask;
extern TaskHandle_t xHawkbitTask;
extern TaskHandle_t xDnsTask;



/*---------------------------------*/

/*< EVENT Handler >*/

extern EventGroupHandle_t xPhaseEvent;
extern EventGroupHandle_t xWifiEvent;

/*---------------------------------*/

/*< Object QUEUE >*/

extern QueueHandle_t xWifiQueue;
extern QueueHandle_t xParseQueue;
extern QueueHandle_t xButtonQueue;
extern QueueHandle_t xScheduleInQueue;
extern QueueHandle_t xScheduleOutQueue;
extern QueueHandle_t xAutoModeQueue;
extern QueueHandle_t xBuzzerQueue;
extern QueueHandle_t xWifiLedQueue;
extern QueueHandle_t xPcfQueue;
extern QueueHandle_t xVFDQueue;
extern QueueHandle_t xRelayQueue;



/*---------------------------------*/

/*< Object Semaphore & Mutex >*/

extern SemaphoreHandle_t xNetworkMutex;
extern SemaphoreHandle_t xI2cSemaphore;

/*---------------------------------*/

#endif