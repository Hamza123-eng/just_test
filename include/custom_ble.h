#ifndef CUSTOM_BLE_
#define CUSTOM_BLE_

void BleTask(void *param);

enum BleStatus_t
{
    
    kIdle=0,
    kTimeOut,
    kSuccess,
    kScanFailed,
    kGotdata,
    kDeviceFalied,
    kDeviceSucess,
};

#endif
