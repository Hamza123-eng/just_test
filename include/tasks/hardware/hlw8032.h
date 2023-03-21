#ifndef HLW8032_H_
#define HLW8032_H_

#define BUFF_SIZE 512

#include "stdint.h"


void HlwInit(void *param);

/**
 * @brief Structure for voltage storage
 *
 */
typedef struct
{
    uint16_t phase_voltage[3];
    uint8_t error_type;
    uint8_t phase_selected;
    bool data_ready;
    bool phase_2_read;
    bool phase_3_read;
} hlw_voltage_t;

extern hlw_voltage_t hlw_voltage;

#endif
