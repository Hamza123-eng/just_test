#ifndef HLW8032_BTS_H_
#define HLW8032_BTS_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "hlw8032.h"



#define HLW_DATA_SIZE 24
#define VP 2 // voltage parameter index in hlw registers data
#define V 5 // voltage index in hlw registers data
#define IP 8 // Current parameter index in hlw registers data
#define I 11 // Current index in hlw registers data
#define PP 14 // Power parameter index in hlw registers data
#define P 17 // Power index in hlw registers data
#define DUR 20// Data Updata REG

#define HS 16 // High byte shift
#define MS 8 // Medium byte shift


void hlw_data_handle(uint8_t *HLW8032Regs, uint8_t len ,uint8_t phase_num);

#endif
