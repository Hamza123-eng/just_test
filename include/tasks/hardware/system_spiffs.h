#ifndef _SPIFFS_SYSTEM_
#define _SPIFFS_SYSTEM_

#include "stdint.h"

void Init_Spiffs(void *param);

void ReadFromSpiffs(uint16_t read_pointer, uint16_t size, void *data);

void WriteToSpiffs(uint16_t write_pointer, uint16_t size, void *data);
void FillDummy(uint16_t size,uint16_t index);

#endif //_SPIFFS_SYSTEM_