#ifndef _PARSE_WEBSOCKET_H_
#define _PARSE_WEBSOCKET_H_

void websocket_app_start(void);
void websocket_app_stop(void);
void websocket_schedule_start(void);
void websocket_sch_stop(void);

typedef struct
{
    int32_t rt_event_tracker;  // it will be used in indications if websocket is not connected
    int32_t sch_event_tracker; // it will be used in indications if websocket is not connected
} WSTracker_t;

extern WSTracker_t ws_tracker; // it will be used in indications if websocket is not connected

#endif //_PARSE_WEBSOCKET_H_