#ifndef _PCF_SLEEP_
#define _PCF_SLEEP_

#define I2C_PORT 0
#define PCF_ADDRESS 0x20

typedef enum
{
    NONE_PCF=0x00,
   FACTORY_RESET_PCF=0x01,
   RUNNING_MODE=0x02,

   WRONG_USER=0x03,
   NO_SERIAL_NUMBER,
   FIRMWARE_UPDATE,
}pcf_mode_t;

void PcfInit(void *param);

void i2cSemaphoreGive(void *param);
void i2cSemaphoreTake(void *param);

#endif ///_PCF_SLEEP_