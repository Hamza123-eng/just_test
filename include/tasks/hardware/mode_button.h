#ifndef _MODE_BUTTON_H
#define _MODE_BUTTON_H

/* <  Carefull !!! ----> This time is in Seconds > */

#define TIME_MULTIPLE 3

#define MAINTINANCE_BUTTON_PRESS_TIME TIME_MULTIPLE * 1
#define APP_MODE_PREES_TIME TIME_MULTIPLE * 2
#define EEPROM_PREES_TIME TIME_MULTIPLE * 3

#define BUTTON_POLARITY (bool)0 // 1 mean it  sense GND

void InitModeButton(void *param);

#endif // _MODE_BUTTON_H
