
#ifndef SCHEDULE_HEADER_H_
#define SCHEDULE_HEADER_H_

#define MAX_SCHEDULE 10
/**
 * @brief 
 * 
 */
typedef struct
{
  uint64_t start_time;
  uint64_t stop_time;
  char phone_num[16];
  char object_id[16];
  char tag[16];
 // bool motor_start_flag;
 bool schedule_status;
} parse_schedule_t;
/**
 * @brief 
 * 
 */
typedef struct
{
  uint8_t selected_schedule;
  uint8_t total_schedule;
  bool software_on_flag;
  bool entertain_query_callback;
  bool schedule_ready_strobe;
} scheduling_control_param_t;
/**
 * @brief 
 * 
 */
typedef struct
{
  char schedule_id[16];
  char schedule_tag[16];
  char schedule_phone[16];
} schedule_record_t;
/**
 * @brief 
 * 
 */
typedef enum
{
  FETCH_SCHEDULE,
  DELETE_CURRENT_SCHEDULE,

  REFRESH_SCHEDULE,
  SCHEDULE_HALT_BY_FAULT,
  HALT_SCHEDULE_OPERATION,
  RESUME_SCHEDULE_OPERATION

} schedule_event_t;

void ScheduleTaskInit();
void LiveQuerryCall();

void ScheduleTaskInit(void *param);

long ConvertSleepTime(char *ptr);

extern scheduling_control_param_t schedule_control_param;
extern schedule_record_t last_schedule_record;
extern schedule_record_t current_schedule_record;

extern bool schedule_run;

volatile bool force_deleted;
void ClearAllSchedule(void);

#endif // SCHEDULE_HEADER_H_