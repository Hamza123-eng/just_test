#ifndef F_RAM_H
#define F_RAM_H

#include "stdint.h"

#define FRAM_DEPTH 1024
#define OFFLINE_HSITORY_SIZE 1024



typedef struct
{
  uint16_t pending_event;
  uint16_t write_pointer;
  uint16_t read_pointer;

}fram_config_t;

extern fram_config_t fram_config;

#endif