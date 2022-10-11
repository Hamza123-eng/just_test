#ifndef CUSTOM_BLE_
#define CUSTOM_BLE_

#include "freertos/queue.h"

extern QueueHandle_t xQueueBleOperation;

#define BLE_WAIT 1000 * 90                // Ble TimeOUT in second
#define SCAN_REFRESH_TIME 1000 * 30       // in the  second
#define TOUCH_TIME_TO_ENABLE_BLE 1000 * 6 // time in second
#define TOUCH_TIME_TO_ENABLE_VOLUMN 1000 * 5 // time in second



void BleInit(void *param);

enum BleStatus_t
{

    kIdle = 0,
    kTimeOut,
    kSuccess,
    kScanFailed,
    kGotdata,
    kDeviceFalied,
    kDeviceSucess,
};

#endif
