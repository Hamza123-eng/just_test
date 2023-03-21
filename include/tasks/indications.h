#ifndef INDICATIONS_H_
#define INDICATIONS_H_

// FORMAT FOR   ----> LED_BLINK[ON_TIME][OFF_TIME]



#define BUZZER_BEEP_TIME 700   // time in millli seconds
#define BUZZER_TASK_DELAY 1000 // time in ms

void WifiIndicationInit();

void MntIndicationInit();



typedef enum
{
    FACTORY_RESET =0x01,
    USER_CONNECTED = 0x02,
    APP_MODE_ENABLE = 0x03,
    AP_STA_ENABLE = 0x04,
    STA_CONNECTED = 0x05
    
} wifi_led_indication_t;

typedef enum
{
    FAULT_DETECTED = 0x01,
    AP_STA_BUZZER_ENABLE = 0x02,
    MULTI_BUTTON=0x03,
    ACCOUNT_RECEIVED =0x04,

    FAULT_REMOVED=0x05,
    AP_STA_BUZZER_DISABLE=0x06,
    FIRMWARE_COMPLETE,

} buzzer_beep_t;

void WifiIndicationInit(void *param);
void BuzzerInit(void *param);

#endif // INDICATIONS_H_