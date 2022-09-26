#ifndef CUSTOM_TOUCHPAD_
#define CUSTOM_TOUCHPAD_

void print_wakeup_reason(void *param);

void TouchPadTask(void *param);

void InitTouchPad(void *param);

// uint8_t redButtonHoldCount = 0;

enum ButtonPress_t
{
    kTouch0 = 0,
    kTouch1,
    kTouch2,
    kTouch3,
    kTouch4,
    kTouch5,
    kTouch6,
    kTouch7,
    kTouch8,
    kTouch9,
    kTouchIdle,

};

// enum ButtonPress_t *sleep_wake_press ;
// enum ButtonPress_t press = kNoTouch;
// bool touch_event_happened;
extern ButtonPress_t sleep_wake_press;
 extern  ButtonPress_t prev_button;
#endif